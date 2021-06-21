#pragma once
// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/dt_common.h
// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/dt_recv.h

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

struct RecvProp_t;
struct DataVariant_t
{
	union
	{
		float	Float;
		long	Int;
		char*	String;
		void*	Data;
		float	Vector[3];
		int64_t Int64;
	};

	ESendPropType iType;
};

class CRecvProxyData
{
public:
	const RecvProp_t*	pRecvProp;	// the property it's receiving
	DataVariant_t		Value;		// the value given to you to store
	int					iElement;	// which array element you're getting
	int					nObjectID;	// the object being referred to
};

using RecvVarProxyFn = void(__cdecl*)(const CRecvProxyData*, void*, void*);
using ArrayLengthProxyFn = void(__cdecl*)(void*, int, int);
using DataTableProxyFn = void(__cdecl*)(const RecvProp_t*, void**, void*, int);

class CStandartRecvProxies
{
public:
	RecvVarProxyFn pInt32ToInt8;
	RecvVarProxyFn pInt32ToInt16;
	RecvVarProxyFn pInt32ToInt32;
	RecvVarProxyFn pInt64ToInt64;
	RecvVarProxyFn pFloatToFloat;
	RecvVarProxyFn pVectorToVector;
};

// receive data table
struct RecvTable_t
{
	RecvProp_t*		pProps;
	int				nProps;
	void*			pDecoder;
	char*			szNetTableName;
	bool			bInitialized;
	bool			bInMainList;
};

// receive prop comes from the receive data table
struct RecvProp_t
{
	char*				szVarName;
	ESendPropType		iRecvType;
	int					iFlags;
	int					nStringBufferSize;
	bool				bInsideArray;
	const void*			pExtraData;
	RecvProp_t*			pArrayProp;
	ArrayLengthProxyFn* oArrayLengthProxyFn;
	RecvVarProxyFn		oProxyFn;
	DataTableProxyFn*	oDataTableProxyFn;
	RecvTable_t*		pDataTable;
	int					iOffset;
	int					iElementStride;
	int					iElements;
	const char*			szParentArrayPropName;
};
