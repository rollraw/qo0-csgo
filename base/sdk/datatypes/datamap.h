#pragma once

#pragma region datamap_enumerations
enum EFieldType : int
{
	FIELD_VOID = 0,			// No type or value
	FIELD_FLOAT,			// Any floating point value
	FIELD_STRING,			// A string ID (return from ALLOC_STRING)
	FIELD_VECTOR,			// Any vector, QAngle, or AngularImpulse
	FIELD_QUATERNION,		// A quaternion
	FIELD_INTEGER,			// Any integer or enum
	FIELD_BOOLEAN,			// boolean, implemented as an int, I may use this as a hint for compression
	FIELD_SHORT,			// 2 byte integer
	FIELD_CHARACTER,		// a byte
	FIELD_COLOR32,			// 8-bit per channel r,g,b,a (32bit color)
	FIELD_EMBEDDED,			// an embedded object with a datadesc, recursively traverse and embedded class/structure based on an additional typedescription
	FIELD_CUSTOM,			// special type that contains function pointers to it's read/write/parse functions
	FIELD_CLASSPTR,			// CBaseEntity*
	FIELD_EHANDLE,			// Entity handle
	FIELD_EDICT,			// edict_t*
	FIELD_POSITION_VECTOR,	// A world coordinate (these are fixed up across level transitions automagically)
	FIELD_TIME,				// a floating point time (these are fixed up automatically too!)
	FIELD_TICK,				// an integer tick count( fixed up similarly to time)
	FIELD_MODELNAME,		// Engine string that is a model name (needs precache)
	FIELD_SOUNDNAME,		// Engine string that is a sound name (needs precache)
	FIELD_INPUT,			// a list of inputed data fields (all derived from CMultiInputVar)
	FIELD_FUNCTION,			// A class function pointer (Think, Use, etc)
	FIELD_VMATRIX,			// a vmatrix (output coords are NOT worldspace)
	FIELD_VMATRIX_WORLDSPACE,// A VMatrix that maps some local space to world space (translation is fixed up on level transitions)
	FIELD_MATRIX3X4_WORLDSPACE,	// matrix3x4_t that maps some local space to world space (translation is fixed up on level transitions)
	FIELD_INTERVAL,			// a start and range floating point interval (e.g., 3.2->3.6 == 3.2 and 0.4)
	FIELD_MODELINDEX,		// a model index
	FIELD_MATERIALINDEX,	// a material index (using the material precache string table)
	FIELD_VECTOR2D,			// 2 floats
	FIELD_INTEGER64,		// 64bit integer
	FIELD_VECTOR4D,			// 4 floats
	FIELD_TYPECOUNT
};

enum
{
	TD_OFFSET_NORMAL = 0,
	TD_OFFSET_PACKED = 1,
	TD_OFFSET_COUNT
};
#pragma endregion

// forward declarations
struct DataMap_t;
struct OptimizedDataMap_t; // not implemented

#pragma pack(push, 4)
// functions used to verify offsets:
// @ida DescribeFlattenedList(): client.dll -> "55 8B EC 83 EC 20 83 3D ? ? ? ? ? B8" @xref: "->Sorted %s for copy type: %s, packing: %s\n"
struct TypeDescription_t
{
public:
	EFieldType nFieldType; // 0x00
	const char* szFieldName; // 0x04
	int iFieldOffset; // 0x08
	unsigned short nFieldSize; // 0x0C
	short nFlags; // 0x0E
	const char* szExternalName; // 0x10
	void* pSaveRestoreOps; // 0x14
	void* pInputFunc; // 0x18
	DataMap_t* pTypeDescription; // 0x1C
	int nFieldSizeInBytes; // 0x20
	TypeDescription_t* pOverrideField; // 0x24
	int nOverrideCount; // 0x28
	float flFieldTolerance; // 0x2C
	int iFlatOffset[TD_OFFSET_COUNT]; // 0x30
	unsigned short nFlatGroup; // 0x38
};
static_assert(sizeof(TypeDescription_t) == 0x3C); // size verify @ida: client.dll -> U8["8D 76 ? 66 8B C3 47" + 0x2]

// functions used to verify offsets:
// @ida CPredictionCopy::TransferData(): client.dll -> "55 8B EC 8B 45 10 53 56 8B F1 57"
struct DataMap_t
{
	TypeDescription_t* pDataDesc; // 0x00
	int nDataFieldCount; // 0x04
	const char* szDataClassName; // 0x08 // @ida: client.dll -> U8["51 50 FF 72 ? 68" + 0x4]
	DataMap_t* pBaseMap; // 0x0C
	int nPackedSize; // 0x10 // @ida: client.dll -> U8["89 47 ? 83 FB 60 7C CF" + 0x2]
	OptimizedDataMap_t* pOptimizedDataMap; // 0x14 // @ida: U8["83 78 ? 00 75 0A 8B C8 E8" + 0x2]
};
static_assert(sizeof(DataMap_t) == 0x18); // @todo: size verify
#pragma pack(pop)
