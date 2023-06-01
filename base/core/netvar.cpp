#include "netvar.h"

// used: [stl] sort, lower_bound
#include <algorithm>
// used: [stl] vector
#include <vector>
// used: [crt] time_t, time, localtime_s
#include <ctime>

// used: getworkingpath
#include "../core.h"
// used: stringcopy, stringcat
#include "../utilities/crt.h"

// used: interface handles
#include "interfaces.h"
// used: interface declarations
#include "../sdk/interfaces/ibaseclientdll.h"

// @todo: much possible store/dump improvements, workaround over "baseclass" props (e.g. in storing case, we can use it to determine is ptr base/parent class to other, in dump case, we can print them near table name to show hierarchy), get rid of stl

struct PropertyObject_t
{
	FNV1A_t uHash = 0U;
	RecvProp_t* pRecvProp = nullptr;
	std::uintptr_t uOffset = 0U;
};

static std::vector<PropertyObject_t> vecProperties = { };
static std::size_t nTablesCount = 0U;

// recursively go through table and child tables properties and store their offsets
static void StoreTableProperties(const RecvTable_t* pRecvTable, const FNV1A_t uTableHash)
{
	// reserve memory for all table properties
	vecProperties.reserve(vecProperties.size() + pRecvTable->nPropCount);

	const FNV1A_t uDelimiterHash = FNV1A::Hash("::", uTableHash);
	const FNV1A_t uBaseClassHash = FNV1A::Hash("baseclass", uDelimiterHash);

	for (int i = 0; i < pRecvTable->nPropCount; i++)
	{
		RecvProp_t* const pCurrentProp = &pRecvTable->vecProps[i];

		// concat variable name to our netvar format just by hash
		const FNV1A_t uVariableHash = FNV1A::Hash(pCurrentProp->szVarName, uDelimiterHash);

		// skip baseclass table
		if (uVariableHash == uBaseClassHash)
			continue;

		const std::uintptr_t uOffset = static_cast<std::uintptr_t>(pCurrentProp->iOffset);

		// check if the property is a child data table
		if (pCurrentProp->nRecvType == DPT_DATATABLE)
		{
			static_assert(std::endian::native == std::endian::little); // following code assume little-endian

			// check is child table aren't empty and have "DT" prefix
			if (const RecvTable_t* pChildTable = pCurrentProp->pDataTable; pChildTable->nPropCount > 0 && *reinterpret_cast<std::uint16_t*>(pChildTable->szNetTableName) == 0x5444)
				// recursively get properties from it, if it has no base use parent table name
				// @note: we don't accumulate offset! follow parent base variable to properly get child offsets
				StoreTableProperties(pChildTable, uOffset == 0U ? uTableHash : FNV1A::Hash(pChildTable->szNetTableName));
		}
		else if (pCurrentProp->nRecvType == DPT_ARRAY)
		{
			// @todo: the way valve doing this is fucking dumb, and even isn't same as for send tables @source: master/engine/dt.h#L469-471
		}

		vecProperties.emplace_back(PropertyObject_t{ uVariableHash, pCurrentProp, uOffset });
	}

	// count total stored tables
	++nTablesCount;
}

// recursively go through table and child tables properties, format them and print into the file
// used separate function to have variables sorted by their offsets, don't mess storing code with debug checks and follow general principles 
static void DumpTableProperties(HANDLE hFileOut, const RecvTable_t* pRecvTable, const int iDepth = 0)
{
	char szTableBuffer[64];
	char* szTableEnd = szTableBuffer;

	// insert depth tabulation
	int iTableDepth = iDepth;
	while (iTableDepth-- > 0)
		*szTableEnd++ = '\t';

	// insert table name
	*szTableEnd++ = '[';
	szTableEnd = CRT::StringCopy(szTableEnd, pRecvTable->szNetTableName);
	*szTableEnd++ = ']';
	*szTableEnd++ = '\n';

	// zero-terminate string
	*szTableEnd = '\0';

	Q_ASSERT(szTableEnd - szTableBuffer < sizeof(szTableBuffer)); // stack overflow
	::WriteFile(hFileOut, szTableBuffer, szTableEnd - szTableBuffer, nullptr, nullptr);

	// create copy of variables in the current table
	int nSortedPropertiesCount = pRecvTable->nPropCount;
	RecvProp_t* vecPropsSorted = static_cast<RecvProp_t*>(MEM::HeapAlloc(nSortedPropertiesCount * sizeof(RecvProp_t)));
	CRT::MemoryCopy(vecPropsSorted, pRecvTable->vecProps, nSortedPropertiesCount * sizeof(RecvProp_t));

	// flatten child tables without base offset into our copied table
	for (int i = 0; i < pRecvTable->nPropCount; i++)
	{
		const RecvProp_t* pCurrentProp = &vecPropsSorted[i];

		// skip baseclass table
		if (CRT::StringCompare(pCurrentProp->szVarName, "baseclass") == 0)
			continue;

		const std::uintptr_t uOffset = static_cast<std::uintptr_t>(pCurrentProp->iOffset);

		// check if the property is a child data table
		if (pCurrentProp->nRecvType == DPT_DATATABLE)
		{
			static_assert(std::endian::native == std::endian::little); // following code assume little-endian

			// check is child table aren't empty, doesn't have base offset and have "DT" prefix
			if (const RecvTable_t* pChildTable = pCurrentProp->pDataTable; pChildTable->nPropCount > 0 && uOffset == 0U && *reinterpret_cast<std::uint16_t*>(pChildTable->szNetTableName) == 0x5444)
			{
				// add child table properties to the copied table buffer
				vecPropsSorted = static_cast<RecvProp_t*>(MEM::HeapRealloc(vecPropsSorted, (nSortedPropertiesCount + pChildTable->nPropCount) * sizeof(RecvProp_t)));
				CRT::MemoryCopy(vecPropsSorted + nSortedPropertiesCount, pChildTable->vecProps, pChildTable->nPropCount * sizeof(RecvProp_t));
				nSortedPropertiesCount += pChildTable->nPropCount;
			}
		}
	}

	// sort the copied variables by their offset
	// @todo: crt
	std::sort(vecPropsSorted, vecPropsSorted + nSortedPropertiesCount, [](const auto& leftProperty, const auto& rightProperty) { return leftProperty.iOffset < rightProperty.iOffset; });
	//std::partial_sort_copy(pRecvTable->vecProps, pRecvTable->vecProps + pRecvTable->nPropCount, vecPropsSorted, vecPropsSorted + pRecvTable->nPropCount, [](const auto& leftProperty, const auto& rightProperty) { return leftProperty.iOffset < rightProperty.iOffset; });

	for (int i = 0; i < nSortedPropertiesCount; i++)
	{
		const RecvProp_t* pCurrentProp = &vecPropsSorted[i];

		// skip baseclass table
		if (CRT::StringCompare(pCurrentProp->szVarName, "baseclass") == 0)
			continue;

		const std::uintptr_t uOffset = static_cast<std::uintptr_t>(pCurrentProp->iOffset);

		// check if the property is a child data table
		if (pCurrentProp->nRecvType == DPT_DATATABLE)
		{
			static_assert(std::endian::native == std::endian::little); // following code assume little-endian

			// check is child table aren't empty, have base offset and have "DT" prefix
			if (const RecvTable_t* pChildTable = pCurrentProp->pDataTable; pChildTable->nPropCount > 0 && uOffset != 0U && *reinterpret_cast<std::uint16_t*>(pChildTable->szNetTableName) == 0x5444)
				// recursively get properties from it
				DumpTableProperties(hFileOut, pChildTable, iDepth + 1);
		}
		else if (pCurrentProp->nRecvType == DPT_ARRAY)
		{
			// @todo: the way valve doing this is fucking dumb, and even isn't same as for send tables @source: master/engine/dt.h#L469-471
		}

		char szVariableBuffer[128];
		char* szVariableEnd = szVariableBuffer;

		// insert depth tabulation
		int iVariableDepth = iDepth + 1;
		while (iVariableDepth-- > 0)
			*szVariableEnd++ = '\t';

		char szPropertyType[64];
		NETVAR::GetPropertyType(pCurrentProp, szPropertyType);

		// insert variable type
		szVariableEnd = CRT::StringCopy(szVariableEnd, szPropertyType);
		*szVariableEnd++ = ' ';

		// insert variable name
		szVariableEnd = CRT::StringCopy(CRT::StringCopy(szVariableEnd, pCurrentProp->szVarName), " = 0x");

		// insert variable offset
		char szPropertyOffsetBuffer[CRT::IntegerToString_t<std::uintptr_t, 16U>::MaxCount()];
		szVariableEnd = CRT::StringCopy(szVariableEnd, CRT::IntegerToString(uOffset, szPropertyOffsetBuffer, sizeof(szPropertyOffsetBuffer), 16));
		*szVariableEnd++ = ';';
		*szVariableEnd++ = '\n';

		// zero-terminate string
		*szVariableEnd = '\0';

		Q_ASSERT(szVariableEnd - szVariableBuffer < sizeof(szVariableBuffer)); // stack overflow
		::WriteFile(hFileOut, szVariableBuffer, szVariableEnd - szVariableBuffer, nullptr, nullptr);
	}

	MEM::HeapFree(vecPropsSorted);
}

bool NETVAR::Setup()
{
	for (const CClientClass* pClass = I::Client->GetAllClasses(); pClass != nullptr; pClass = pClass->pNext)
	{
		const RecvTable_t* pRecvTable = pClass->pRecvTable;
		StoreTableProperties(pRecvTable, FNV1A::Hash(pRecvTable->szNetTableName));
	}

	// deallocate over-reserved memory
	vecProperties.shrink_to_fit();

	// sort grabbed variables to use faster binary search later
	std::ranges::sort(vecProperties, std::ranges::less{ }, &PropertyObject_t::uHash);

	return !vecProperties.empty();
}

void NETVAR::Dump(const wchar_t* wszFileName)
{
	wchar_t wszDumpFilePath[MAX_PATH];
	if (!CORE::GetWorkingPath(wszDumpFilePath))
		return;

	CRT::StringCat(wszDumpFilePath, wszFileName);

	// open our dump file to write in
	const HANDLE hFileOut = ::CreateFileW(wszDumpFilePath, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (hFileOut == INVALID_HANDLE_VALUE)
		return;

	const std::time_t time = std::time(nullptr);
	std::tm timePoint;
	localtime_s(&timePoint, &time);

	char szInfoBuffer[64];
	const std::size_t nInfoSize = CRT::TimeToString(szInfoBuffer, sizeof(szInfoBuffer), "[%d-%m-%Y %T] qo0 | netvars dump\n\n", &timePoint);

	// write current date, time and info
	::WriteFile(hFileOut, szInfoBuffer, nInfoSize, nullptr, nullptr);

	for (const CClientClass* pClass = I::Client->GetAllClasses(); pClass != nullptr; pClass = pClass->pNext)
		DumpTableProperties(hFileOut, pClass->pRecvTable);

	CloseHandle(hFileOut);
}

#pragma region netvar_get
// @todo: dump, but cuz we can get it only when localplayer is valid makes things harder | either dump with memory scanner or just mess with entity listener and ensure its called once
std::uintptr_t NETVAR::FindInDataMap(const DataMap_t* pMap, const FNV1A_t uFieldHash)
{
	while (pMap != nullptr)
	{
		for (int i = 0; i < pMap->nDataFieldCount; i++)
		{
			const TypeDescription_t* pDataDesc = &pMap->pDataDesc[i];

			if (pDataDesc->szFieldName == nullptr)
				continue;

			if (FNV1A::Hash(pDataDesc->szFieldName) == uFieldHash)
				return pDataDesc->iFieldOffset;

			if (pDataDesc->nFieldType == FIELD_EMBEDDED)
			{
				if (pDataDesc->pTypeDescription != nullptr)
				{
					// recursively get embedded fields
					if (const auto uOffset = FindInDataMap(pDataDesc->pTypeDescription, uFieldHash); uOffset != 0U)
						return uOffset;
				}
			}
		}

		pMap = pMap->pBaseMap;
	}

	return 0U;
}

void NETVAR::GetPropertyType(const RecvProp_t* pRecvProp, char* szOutBuffer)
{
	static CStandartRecvProxies* pStandartRecvProxies = I::Client->GetStandardRecvProxies();

	switch (pRecvProp->nRecvType)
	{
	case DPT_INT:
	{
		// @credits: hinnie
		if (const RecvVarProxyFn_t fnProxy = pRecvProp->fnProxy; fnProxy == pStandartRecvProxies->pInt32ToInt8)
			CRT::StringCopy(szOutBuffer, Q_XOR("byte"));
		else if (fnProxy == pStandartRecvProxies->pInt32ToInt16)
			CRT::StringCopy(szOutBuffer, Q_XOR("short"));
		else
			CRT::StringCopy(szOutBuffer, Q_XOR("int"));

		break;
	}
	case DPT_FLOAT:
		CRT::StringCopy(szOutBuffer, Q_XOR("float"));
		break;
	case DPT_VECTOR:
		CRT::StringCopy(szOutBuffer, Q_XOR("Vector_t"));
		break;
	case DPT_VECTOR2D:
		CRT::StringCopy(szOutBuffer, Q_XOR("Vector2D_t"));
		break;
	case DPT_STRING:
	{
		char szStringSizeBuffer[CRT::IntegerToString_t<int, 10U>::MaxCount()];
		CRT::StringCat(CRT::StringCat(CRT::StringCopy(szOutBuffer, Q_XOR("char[")), CRT::IntegerToString(pRecvProp->nStringBufferSize, szStringSizeBuffer, sizeof(szStringSizeBuffer), 10)), Q_XOR("]"));
		break;
	}
	case DPT_ARRAY:
	{
		// @todo: the way valve handle this is diff
		char szArraySizeBuffer[CRT::IntegerToString_t<int, 10U>::MaxCount()];
		CRT::StringCat(CRT::StringCat(CRT::StringCopy(szOutBuffer, Q_XOR("array[")), CRT::IntegerToString(pRecvProp->nElements, szArraySizeBuffer, sizeof(szArraySizeBuffer), 10)), Q_XOR("]"));
		break;
	}
	case DPT_DATATABLE:
	{
		static_assert(std::endian::native == std::endian::little); // following code assume little-endian

		const RecvTable_t* pChildTable = pRecvProp->pDataTable;
		
		// check if the child table starts with 'DT' prefix
		if (const char* szChildTableName = pChildTable->szNetTableName; *reinterpret_cast<const std::uint16_t*>(szChildTableName) == 0x5444)
		{
			// convert data table name to class name with pointer, e.g. "DT_CSPlayer" -> "C_CSPlayer*"
			char* szOutBufferBegin = szOutBuffer;
			szOutBuffer = CRT::StringCopy(szOutBuffer, szChildTableName + 1);
			*szOutBufferBegin = 'C';
			*szOutBuffer++ = '*';
			*szOutBuffer = '\0';
		}
		else // for some reason valve also uses it just for arrays
		{
			Q_ASSERT(pChildTable->nPropCount > 0);

			// recursively get base type from first array element
			GetPropertyType(pChildTable->vecProps, szOutBuffer);

			// get the array size
			char szArraySizeBuffer[CRT::IntegerToString_t<int, 10U>::MaxCount()];
			const char* szArraySize = CRT::IntegerToString(pChildTable->nPropCount, szArraySizeBuffer, sizeof(szArraySizeBuffer), 10);
			const std::size_t nArraySizeLength = szArraySizeBuffer + sizeof(szArraySizeBuffer) - szArraySize;

			// check if the property has sub-array size
			if (char* szSubArraySize = CRT::StringChar(szOutBuffer, '['); szSubArraySize != nullptr)
			{
				// move sub-array size to put array size before it to follow C-style formatting
				const std::size_t nSubArraySizeLength = CRT::StringLength(szSubArraySize) + 1U;
				CRT::MemoryMove(szSubArraySize + nArraySizeLength + 1U, szSubArraySize, nSubArraySizeLength);

				// note that in this case string is already zero-terminated
				szOutBuffer = szSubArraySize;
			}
			else
			{
				szOutBuffer += CRT::StringLength(szOutBuffer);
				szOutBuffer[nArraySizeLength + 1U] = '\0';
			}
			
			*szOutBuffer++ = '[';
			szOutBuffer = CRT::StringCopy(szOutBuffer, szArraySize);
			*szOutBuffer = ']';
		}

		break;
	}
	case DPT_INT64:
		CRT::StringCopy(szOutBuffer, Q_XOR("int64_t"));
		break;
	default:
		Q_ASSERT(false); // unknown property type
		CRT::StringCopy(szOutBuffer, Q_XOR("void*"));
		break;
	}
}

RecvProp_t* NETVAR::GetProperty(const FNV1A_t uFieldHash)
{
	if (const auto it = std::ranges::lower_bound(vecProperties, uFieldHash, { }, &PropertyObject_t::uHash); it != vecProperties.end() && it->uHash == uFieldHash)
		return it->pRecvProp;

	Q_ASSERT(false); // netvar isn't found
	return nullptr;
}

std::uintptr_t NETVAR::GetOffset(const FNV1A_t uFieldHash)
{
	if (const auto it = std::ranges::lower_bound(vecProperties, uFieldHash, std::ranges::less{ }, &PropertyObject_t::uHash); it != vecProperties.end() && it->uHash == uFieldHash)
		return it->uOffset;

	Q_ASSERT(false); // netvar isn't found
	return 0U;
}

std::size_t NETVAR::GetTablesCount()
{
	return nTablesCount;
}

std::size_t NETVAR::GetPropertiesCount()
{
	return vecProperties.size();
}
#pragma endregion
