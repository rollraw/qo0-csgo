#pragma once
#include "../common.h"
#include "../sdk/hash/fnv1a.h"

// used: recvtable_t, recvprop_t
#include "../sdk/datatypes/datatable.h"
// used: datamap_t
#include "../sdk/datatypes/datamap.h"

#pragma region netvar_definitions
// add function to get variable by straight offset
#define N_ADD_OFFSET(TYPE, NAME, OFFSET)																	\
[[nodiscard]] Q_INLINE std::add_lvalue_reference_t<TYPE> NAME()												\
{																											\
	return *reinterpret_cast<std::add_pointer_t<TYPE>>(reinterpret_cast<std::uint8_t*>(this) + (OFFSET));	\
}

// add function to get variable pointer by straight offset
#define N_ADD_POFFSET(TYPE, NAME, OFFSET)																	\
[[nodiscard]] Q_INLINE std::add_pointer_t<TYPE> NAME()														\
{																											\
	return reinterpret_cast<std::add_pointer_t<TYPE>>(reinterpret_cast<std::uint8_t*>(this) + (OFFSET));	\
}

// add function to get networkable variable with additional offset
#define N_ADD_VARIABLE_OFFSET(TYPE, NAME, VARIABLE, ADDITIONAL) N_ADD_OFFSET(TYPE, NAME, NETVAR::GetOffset(FNV1A::HashConst(VARIABLE)) + (ADDITIONAL))

// add function to get networkable variable
#define N_ADD_VARIABLE(TYPE, NAME, VARIABLE) N_ADD_VARIABLE_OFFSET(TYPE, NAME, VARIABLE, 0U)

// add function to get variable pointer with additional offset from netvar offset
#define N_ADD_PVARIABLE_OFFSET(TYPE, NAME, VARIABLE, ADDITIONAL) N_ADD_POFFSET(TYPE, NAME, NETVAR::GetOffset(FNV1A::HashConst(VARIABLE)) + (ADDITIONAL))

// add function to get networkable variable pointer
#define N_ADD_PVARIABLE(TYPE, NAME, VARIABLE) N_ADD_PVARIABLE_OFFSET(TYPE, NAME, VARIABLE, 0U)

// add function to get datamap field variable
#define N_ADD_DATAFIELD(TYPE, NAME, DATAMAP, DATAFIELD)														\
[[nodiscard]] std::add_lvalue_reference_t<TYPE> NAME()														\
{																											\
	static std::uintptr_t uOffset = NETVAR::FindInDataMap(DATAMAP, FNV1A::HashConst(DATAFIELD));			\
	return *reinterpret_cast<std::add_pointer_t<TYPE>>(reinterpret_cast<std::uint8_t*>(this) + uOffset);	\
}

// add function to get datamap field variable pointer
#define N_ADD_PDATAFIELD(TYPE, NAME, DATAMAP, DATAFIELD)													\
[[nodiscard]] std::add_pointer_t<TYPE> NAME()																\
{																											\
	static std::uintptr_t uOffset = NETVAR::FindInDataMap(DATAMAP, FNV1A::HashConst(DATAFIELD));			\
	return reinterpret_cast<std::add_pointer_t<TYPE>>(reinterpret_cast<std::uint8_t*>(this) + uOffset);		\
}
#pragma endregion

class CRecvPropHook
{
public:
	bool Create(RecvProp_t* pRecvProp, const RecvVarProxyFn_t pNewProxyFn)
	{
		if (pRecvProp == nullptr)
			return false;

		pProperty = pRecvProp;
		fnOriginal = pRecvProp->fnProxy;

		Replace(pNewProxyFn);
		return true;
	}

	// replace property proxy with our function
	void Replace(const RecvVarProxyFn_t pNewProxyFn) const
	{
		pProperty->fnProxy = pNewProxyFn;
	}

	// restore original property proxy function
	void Restore() const
	{
		if (fnOriginal != nullptr)
			pProperty->fnProxy = fnOriginal;
	}

	/// @returns: original property proxy function
	[[nodiscard]] RecvVarProxyFn_t GetOriginal() const
	{
		return this->fnOriginal;
	}

private:
	// current property
	RecvProp_t* pProperty = nullptr;
	// original proxy function to have ability to restore it later
	RecvVarProxyFn_t fnOriginal = nullptr;
};

/*
 * NETVAR MANAGER
 * - game networkable variables
 *
 * use command "cl_pdump 1" to see prediction dump for local player:
 *    [color]      [meaning]
 * 255, 255, 255 - nothing of below
 * 180, 180, 225 - networked, error checked
 * 150, 180, 150 - networked, not error checked
 * 255, 255,   0 - differs, within tolerance
 * 255,   0,   0 - differs, with no tolerance, networked
 * 180, 180, 100 - differs, with no tolerance, not networked
 */
namespace NETVAR
{
	/// store networkable properties and their offsets
	/// @returns: true if the properties were successfully stored, false otherwise
	bool Setup();
	void Dump(const wchar_t* wszFileName);

	/* @section: get */
	/// search for the field offset of object in the data map hierarchy
	/// @returns: offset of the field matched to given hash on success, null otherwise
	std::uintptr_t FindInDataMap(const DataMap_t* pMap, const FNV1A_t uFieldHash);
	// format property type to string
	void GetPropertyType(const RecvProp_t* pRecvProp, char* szOutBuffer);
	/// @returns: property of variable matched to given hash if it exist, null otherwise
	RecvProp_t* GetProperty(const FNV1A_t uFieldHash);
	/// @returns: offset of variable matched to given hash if it exist, null otherwise
	std::uintptr_t GetOffset(const FNV1A_t uFieldHash);
	/// @returns: count of stored tables during setup
	std::size_t GetTablesCount();
	/// @returns: count of stored properties during setup
	std::size_t GetPropertiesCount();
}
