#pragma once
#include "vector.h"

// @source: master/public/dt_common.h
// master/public/dt_recv.h

#pragma region datatable_enumerations
enum ESendPropType : int
{
	DPT_INT = 0,
	DPT_FLOAT,
	DPT_VECTOR,
	DPT_VECTOR2D,
	DPT_STRING,
	DPT_ARRAY,
	DPT_DATATABLE,
	DPT_INT64,
	DPT_SENDPROPTYPEMAX
};
#pragma endregion

// forward declarations
class CRecvProxyData;
struct RecvProp_t;

using RecvVarProxyFn_t = void(Q_CDECL*)(const CRecvProxyData*, void*, void*);
using ArrayLengthProxyFn_t = void(Q_CDECL*)(void*, int, int);
using DataTableProxyFn_t = void(Q_CDECL*)(const RecvProp_t*, void**, void*, int);

#pragma pack(push, 8) // @todo: why it fits perfectly with 8 bytes packing?
struct DataVariant_t
{
	union
	{
		float flValue;
		long lValue;
		char* szValue;
		void* pValue;
		Vector_t vecValue;
		std::int64_t ullValue;
	}; // 0x00

	ESendPropType nType; // 0x0C
};
static_assert(sizeof(DataVariant_t) == 0x18);

// functions used to verify offsets:
// @ida Generic_FastCopy(): engine.dll -> "55 8B EC 83 E4 F8 83 EC 2C 8B 55 08"
class CRecvProxyData
{
public:
	const RecvProp_t* pRecvProp; // 0x00 // the property it's receiving
	DataVariant_t Value; // 0x04 // the value given to you to store
	int iElement; // 0x20 // which array element you're getting
	int nObjectID; // 0x24 // the object being referred to
};
static_assert(sizeof(CRecvProxyData) == 0x28);
#pragma pack(pop)

#pragma pack(push, 4)
class CStandartRecvProxies
{
public:
	RecvVarProxyFn_t pInt32ToInt8; // 0x00
	RecvVarProxyFn_t pInt32ToInt16; // 0x04
	RecvVarProxyFn_t pInt32ToInt32; // 0x08
	RecvVarProxyFn_t pInt64ToInt64; // 0x0C
	RecvVarProxyFn_t pFloatToFloat; // 0x10
	RecvVarProxyFn_t pVectorToVector; // 0x14
};
static_assert(sizeof(CStandartRecvProxies) == 0x18);

// receive data table
struct RecvTable_t // @todo: verify
{
	RecvProp_t* vecProps; // 0x00
	int nPropCount; // 0x04
	void* pDecoder; // 0x08
	char* szNetTableName; // 0x0C
	bool bInitialized; // 0x10
	bool bInMainList; // 0x11
};
static_assert(sizeof(RecvTable_t) == 0x14);

// receive prop comes from the receive data table
struct RecvProp_t
{
	char* szVarName; // 0x00
	ESendPropType nRecvType; // 0x04
	int nFlags; // 0x08
	int nStringBufferSize; // 0x0C
	bool bInsideArray; // 0x10
	const void* pExtraData; // 0x14
	RecvProp_t* pArrayProp; // 0x18
	ArrayLengthProxyFn_t fnArrayLengthProxy; // 0x1C
	RecvVarProxyFn_t fnProxy; // 0x20
	DataTableProxyFn_t fnDataTableProxy; // 0x24
	RecvTable_t* pDataTable; // 0x28
	int iOffset; // 0x2C
	int iElementStride; // 0x30
	int nElements; // 0x34
	const char* szParentArrayPropName; // 0x38
};
static_assert(sizeof(RecvProp_t) == 0x3C);
#pragma pack(pop)
