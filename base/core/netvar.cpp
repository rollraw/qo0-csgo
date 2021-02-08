#include "netvar.h"

// used: working path
#include "config.h"
// used: client interface
#include "interfaces.h"

bool CNetvarManager::Setup(std::string_view szDumpFileName)
{
	// clear values
	mapProps.clear();
	iStoredProps = 0;
	iStoredTables = 0;

	// format time
	const std::string szTime = fmt::format(XorStr("[{:%d-%m-%Y %X}] "), fmt::localtime(std::time(nullptr)));

	#ifdef _DEBUG
	// open our dump file to write in (here is not exception handle because dump is not critical)
	fsDumpFile.open(C::GetWorkingPath().append(szDumpFileName), std::ios::out | std::ios::trunc);

	if (fsDumpFile.good())
		// write current date, time and info
		fsDumpFile << szTime << XorStr("qo0 | netvars dump\n\n");
	#endif

	for (auto pClass = I::Client->GetAllClasses(); pClass != nullptr; pClass = pClass->pNext)
	{
		if (pClass->pRecvTable == nullptr)
			continue;

		StoreProps(pClass->szNetworkName, pClass->pRecvTable, 0U, 0);
	}

	#ifdef _DEBUG
	// close dump file
	fsDumpFile.close();
	#endif

	return !mapProps.empty();
}

void CNetvarManager::StoreProps(const char* szClassName, RecvTable_t* pRecvTable, const std::uintptr_t uOffset, int nDumpTabs)
{
	#ifdef _DEBUG
	std::string szTable = { };

	for (int i = 0; i < nDumpTabs; i++)
		szTable.append(XorStr("\t"));

	if (fsDumpFile.good())
		fsDumpFile << szTable << XorStr("[") << pRecvTable->szNetTableName << XorStr("]\n");
	#endif

	for (int i = 0; i < pRecvTable->nProps; ++i)
	{
		const auto pCurrentProp = &pRecvTable->pProps[i];

		// base tables filter
		if (pCurrentProp == nullptr || isdigit(pCurrentProp->szVarName[0]))
			continue;

		// skip baseclass
		if (FNV1A::Hash(pCurrentProp->szVarName) == FNV1A::HashConst("baseclass"))
			continue;

		// has child table
		if (const auto pChildTable = pCurrentProp->pDataTable; pChildTable != nullptr &&
			// has props
			pChildTable->nProps > 0 &&
			// first char is 'D' ("DT" - "DataTable")
			pChildTable->szNetTableName[0] == 'D' &&
			// type is data table
			pCurrentProp->iRecvType == DPT_DATATABLE)
			// recursively get props in all child tables
			StoreProps(szClassName, pChildTable, static_cast<std::uintptr_t>(pCurrentProp->iOffset) + uOffset, nDumpTabs + 1);

		// make own netvar pushing format
		const FNV1A_t uHash = FNV1A::Hash(fmt::format(XorStr("{}->{}"), szClassName, pCurrentProp->szVarName).c_str());
		const std::uintptr_t uTotalOffset = static_cast<std::uintptr_t>(pCurrentProp->iOffset) + uOffset;

		// check if we not already grabbed property pointer and offset
		if (!mapProps[uHash].uOffset)
		{
			#ifdef _DEBUG
			if (fsDumpFile.good())
				fsDumpFile << szTable << XorStr("\t") << GetPropertyType(pCurrentProp->iRecvType, pCurrentProp->iElements, pCurrentProp->nStringBufferSize) << " " << pCurrentProp->szVarName << XorStr(" = 0x") << std::uppercase << std::hex << uTotalOffset << ";\n";
			#endif

			// write values to map entry
			mapProps[uHash] = { pCurrentProp, uTotalOffset };
			// count total stored props
			iStoredProps++;
		}
	}

	// count total stored tables
	iStoredTables++;
}

std::string CNetvarManager::GetPropertyType(ESendPropType nPropertyType, int iElements, int nStringBufferSize) const
{
	switch (nPropertyType)
	{
	case DPT_INT:
		return XorStr("int");
	case DPT_FLOAT:
		return XorStr("float");
	case DPT_VECTOR:
		return XorStr("vector");
	case DPT_VECTOR2D:
		return XorStr("vector2d");
	case DPT_STRING:
		return XorStr("char[") + std::to_string(nStringBufferSize) + XorStr("]");
	case DPT_ARRAY:
		return XorStr("std::array<") + std::to_string(iElements) + XorStr(">");
	case DPT_DATATABLE:
		return XorStr("void*");
	case DPT_INT64:
		return XorStr("std::int64_t");
	default:
		return "";
	}
}

std::uintptr_t CNetvarManager::FindInDataMap(DataMap_t* pMap, const FNV1A_t uFieldHash)
{
	while (pMap != nullptr)
	{
		for (int i = 0; i < pMap->nDataFields; i++)
		{
			if (pMap->pDataDesc[i].szFieldName == nullptr)
				continue;

			if (FNV1A::Hash(pMap->pDataDesc[i].szFieldName) == uFieldHash)
				return pMap->pDataDesc[i].iFieldOffset[TD_OFFSET_NORMAL];

			if (pMap->pDataDesc[i].iFieldType == FIELD_EMBEDDED)
			{
				if (pMap->pDataDesc[i].pTypeDescription != nullptr)
				{
					if (const auto uOffset = FindInDataMap(pMap->pDataDesc[i].pTypeDescription, uFieldHash); uOffset != 0U)
						return uOffset;
				}
			}
		}

		pMap = pMap->pBaseMap;
	}

	return 0U;
}
