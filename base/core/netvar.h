#pragma once
// used: std::map
#include <map>
// used: std::ifstream
#include <fstream>

// used: winapi includes
#include "../common.h"
// used: fnv1a hashing
#include "../sdk/hash/fnv1a.h"
// used: data table, prop, data type
#include "../sdk/datatypes/datatable.h"
// used: datamap struct
#include "../sdk/datatypes/datamap.h"

#pragma region netvar_definitions
/* add function to get variable with additional offset from netvar offset */
#define N_ADD_VARIABLE_OFFSET( Type, szFunctionName, szNetVar, uAdditional )								\
	[[nodiscard]] std::add_lvalue_reference_t<Type> szFunctionName()										\
	{																										\
		constexpr FNV1A_t uHash = FNV1A::HashConst(szNetVar);												\
		static std::uintptr_t uOffset = CNetvarManager::Get().mapProps[uHash].uOffset;						\
		return *(std::add_pointer_t<Type>)((std::uintptr_t)this + uOffset + uAdditional);					\
	}

/* add function to get netvar variable */
#define N_ADD_VARIABLE( Type, szFunctionName, szNetVar ) N_ADD_VARIABLE_OFFSET( Type, szFunctionName, szNetVar, 0U )

/* add function to get variable pointer with additional offset from netvar offset */
#define N_ADD_PVARIABLE_OFFSET( Type, szFunctionName, szNetVar, uAdditional )								\
	[[nodiscard]] std::add_pointer_t<Type> szFunctionName()													\
	{																										\
		constexpr FNV1A_t uHash = FNV1A::HashConst(szNetVar);												\
		static std::uintptr_t uOffset = CNetvarManager::Get().mapProps[uHash].uOffset;						\
		return (std::add_pointer_t<Type>)((std::uintptr_t)this + uOffset + uAdditional);					\
	}

/* add function to get netvar variable pointer */
#define N_ADD_PVARIABLE( Type, szFunctionName, szNetVar ) N_ADD_PVARIABLE_OFFSET( Type, szFunctionName, szNetVar, 0U )

/* add function to get datamap variable */
#define N_ADD_DATAFIELD( Type, szFunctionName, pMap, szDataField )											\
	[[nodiscard]] std::add_lvalue_reference_t<Type> szFunctionName()										\
	{																										\
		constexpr FNV1A_t uHash = FNV1A::HashConst(szDataField);											\
		static std::uintptr_t uOffset = CNetvarManager::Get().FindInDataMap(pMap, uHash);					\
		return *(std::add_pointer_t<Type>)((std::uintptr_t)this + uOffset);									\
	}

/* add function to get datamap variable pointer */
#define N_ADD_PDATAFIELD( Type, szFunctionName, pMap, szDataField )											\
	[[nodiscard]] std::add_pointer_t<Type> szFunctionName()													\
	{																										\
		constexpr FNV1A_t uHash = FNV1A::HashConst(szDataField);											\
		static std::uintptr_t uOffset = CNetvarManager::Get().FindInDataMap(pMap, uHash);					\
		return (std::add_pointer_t<Type>)((std::uintptr_t)this + uOffset);									\
	}
#pragma endregion

class CRecvPropHook
{
public:
	CRecvPropHook() = default;

	explicit CRecvPropHook(RecvProp_t* pRecvProp, const RecvVarProxyFn pNewProxyFn)
		: pRecvProp(pRecvProp), pOriginalFn(pRecvProp->oProxyFn)
	{
		SetProxy(pNewProxyFn);
	}

	/* restore original function */
	~CRecvPropHook()
	{
		this->pRecvProp->oProxyFn = this->pOriginalFn;
	}

	// Get
	/* replace with our function */
	void Replace(RecvProp_t* pRecvProp)
	{
		this->pRecvProp = pRecvProp;
		this->pOriginalFn = pRecvProp->oProxyFn;
	}

	const void SetProxy(const RecvVarProxyFn pNewProxyFn)
	{
		this->pRecvProp->oProxyFn = pNewProxyFn;
	}

	const RecvVarProxyFn GetOriginal()
	{
		return this->pOriginalFn;
	}

private:
	// Values
	RecvProp_t* pRecvProp; // in future that is being modified and replace the original prop
	RecvVarProxyFn pOriginalFn; // save current proxy function to get available restore it later
};

class CNetvarManager : public CSingleton<CNetvarManager>
{
public:
	struct NetvarObject_t
	{
		RecvProp_t* pRecvProp;
		std::uintptr_t uOffset;
	};

	// Get
	/* fill map with netvars and also dump it to given file */
	bool Setup(std::string_view szDumpFileName);
	/*
	 * stores the variables of objects in the hierarchy
	 * used to iterate through an object's data descriptions from data map
	 */
	std::uintptr_t FindInDataMap(DataMap_t* pMap, const FNV1A_t uFieldHash);

	// Values
	/* logging counters */
	int iStoredProps = 0;
	int iStoredTables = 0;
	/* networkable properties map */
	std::map<FNV1A_t, NetvarObject_t> mapProps;

private:
	/*
	 * recursively stores networked properties info from data tables in our map
	 * and also format our dump and write values to file
	 */
	void StoreProps(const char* szClassName, RecvTable_t* pRecvTable, const std::uintptr_t uOffset, int nDumpTabs);

	// Extra
	std::string GetPropertyType(ESendPropType nPropertyType, int iElements, int nStringBufferSize);

	// Values
	/* output file */
	std::ofstream fsDumpFile;
};
