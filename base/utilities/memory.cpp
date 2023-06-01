#include "memory.h"

// used: __readfsdword
#include <intrin.h>

// used: l_print
#include "log.h"
// used: chartohexint
#include "crt.h"
// used: [win] extra api
#include "win.h"

std::uintptr_t ROP::EngineGadget_t::uReturnGadget = 0xDEADC0DE;
std::uintptr_t ROP::ClientGadget_t::uReturnGadget = 0xDEADC0DE;

bool MEM::Setup()
{
	// @todo: store pointers of the all used game modules once
	bool bSuccess = true;

	// @note: make sure that 'FindPattern()' and all nested calls inside it doesn't attempt to call any of game virtual functions, otherwise it will cause crash
	// look for the gadget in the needed modules, we have the exact instruction for the selected register, but if this is not the case, then even some operand of another instruction will work just fine
	ROP::EngineGadget_t::uReturnGadget = reinterpret_cast<std::uintptr_t>(FindPattern(ENGINE_DLL, Q_XOR("83 C3 10 FF 23")) + 0x3);
	bSuccess &= (ROP::EngineGadget_t::uReturnGadget != 0U);

	ROP::ClientGadget_t::uReturnGadget = reinterpret_cast<std::uintptr_t>(FindPattern(CLIENT_DLL, Q_XOR("83 C3 10 FF 23")) + 0x3);
	bSuccess &= (ROP::ClientGadget_t::uReturnGadget != 0U);

	fnRTDynamicCast = reinterpret_cast<decltype(fnRTDynamicCast)>(FindPattern(CLIENT_DLL, Q_XOR("6A 18 68 ? ? ? ? E8 ? ? ? ? 8B 7D 08")));
	bSuccess &= (fnRTDynamicCast != nullptr);

	const void* hVstdLib = GetModuleBaseHandle(VSTDLIB_DLL);
	const void* hDbgHelp = GetModuleBaseHandle(DBGHELP_DLL);

	if (hVstdLib == nullptr || hDbgHelp == nullptr)
		return false;

	fnRandomSeed = reinterpret_cast<decltype(fnRandomSeed)>(GetExportAddress(hVstdLib, Q_XOR("RandomSeed")));
	bSuccess &= (fnRandomSeed != nullptr);

	fnRandomFloat = reinterpret_cast<decltype(fnRandomFloat)>(GetExportAddress(hVstdLib, Q_XOR("RandomFloat")));
	bSuccess &= (fnRandomFloat != nullptr);

	fnRandomFloatExp = reinterpret_cast<decltype(fnRandomFloatExp)>(GetExportAddress(hVstdLib, Q_XOR("RandomFloatExp")));
	bSuccess &= (fnRandomFloatExp != nullptr);

	fnRandomInt = reinterpret_cast<decltype(fnRandomInt)>(GetExportAddress(hVstdLib, Q_XOR("RandomInt")));
	bSuccess &= (fnRandomInt != nullptr);

	fnRandomGaussianFloat = reinterpret_cast<decltype(fnRandomGaussianFloat)>(GetExportAddress(hVstdLib, Q_XOR("RandomGaussianFloat")));
	bSuccess &= (fnRandomGaussianFloat != nullptr);

	// @todo: move to win.cpp (or platform.cpp?)
	fnUnDecorateSymbolName = reinterpret_cast<decltype(fnUnDecorateSymbolName)>(GetExportAddress(hDbgHelp, Q_XOR("UnDecorateSymbolName")));
	bSuccess &= (fnUnDecorateSymbolName != nullptr);

	return bSuccess;
}

#pragma region memory_allocation
/*
 * overload global new/delete operators with our allocators
 * - @note: ensure that all sdk classes that can be instantiated have an overloaded constructor and/or game allocator, otherwise marked as non-constructible
 */
void* __cdecl operator new(const std::size_t nSize)
{
	return MEM::HeapAlloc(nSize);
}

void* __cdecl operator new[](const std::size_t nSize)
{
	return MEM::HeapAlloc(nSize);
}

void __cdecl operator delete(void* pMemory) noexcept
{
	MEM::HeapFree(pMemory);
}

void __cdecl operator delete[](void* pMemory) noexcept
{
	MEM::HeapFree(pMemory);
}

void* MEM::HeapAlloc(const std::size_t nSize)
{
	const HANDLE hHeap = ::GetProcessHeap();
	return ::HeapAlloc(hHeap, 0UL, nSize);
}

void MEM::HeapFree(void* pMemory)
{
	if (pMemory != nullptr)
	{
		const HANDLE hHeap = ::GetProcessHeap();
		::HeapFree(hHeap, 0UL, pMemory);
	}
}

void* MEM::HeapRealloc(void* pMemory, const std::size_t nNewSize)
{
	if (pMemory == nullptr)
		return HeapAlloc(nNewSize);

	if (nNewSize == 0UL)
	{
		HeapFree(pMemory);
		return nullptr;
	}

	const HANDLE hHeap = ::GetProcessHeap();
	return ::HeapReAlloc(hHeap, 0UL, pMemory, nNewSize);
}
#pragma endregion

// @todo: move to win.cpp (or platform.cpp?) except getsectioninfo
#pragma region memory_get
void* MEM::GetModuleBaseHandle(const wchar_t* wszModuleName)
{
	const _PEB32* pPEB = reinterpret_cast<_PEB32*>(__readfsdword(0x30));

	if (wszModuleName == nullptr)
		return pPEB->ImageBaseAddress;

	::EnterCriticalSection(pPEB->LoaderLock);

	void* pModuleBase = nullptr;
	for (LIST_ENTRY* pListEntry = pPEB->Ldr->InLoadOrderModuleList.Flink; pListEntry != &pPEB->Ldr->InLoadOrderModuleList; pListEntry = pListEntry->Flink)
	{
		const _LDR_DATA_TABLE_ENTRY32* pEntry = CONTAINING_RECORD(pListEntry, _LDR_DATA_TABLE_ENTRY32, InLoadOrderLinks);

		if (pEntry->BaseDllName.Buffer != nullptr && CRT::StringCompare(wszModuleName, pEntry->BaseDllName.Buffer) == 0)
		{
			pModuleBase = pEntry->DllBase;
			break;
		}
	}

	::LeaveCriticalSection(pPEB->LoaderLock);

	if (pModuleBase == nullptr)
		L_PRINT(LOG_ERROR) << Q_XOR("module base not found: \"") << wszModuleName << Q_XOR("\"");

	return pModuleBase;
}

std::size_t MEM::GetModuleBaseSize(const void* hModuleBase)
{
	const _PEB32* pPEB = reinterpret_cast<_PEB32*>(__readfsdword(0x30)); // mov eax, fs:[0x30]

	if (hModuleBase == nullptr)
		hModuleBase = pPEB->ImageBaseAddress;

	const auto pIDH = static_cast<const IMAGE_DOS_HEADER*>(hModuleBase);
	if (pIDH->e_magic != IMAGE_DOS_SIGNATURE)
	{
		L_PRINT(LOG_ERROR) << Q_XOR("failed to get module size, image is invalid");
		return 0U;
	}

	const auto pINH = reinterpret_cast<const IMAGE_NT_HEADERS*>(static_cast<const std::uint8_t*>(hModuleBase) + pIDH->e_lfanew);
	if (pINH->Signature != IMAGE_NT_SIGNATURE)
	{
		L_PRINT(LOG_ERROR) << Q_XOR("failed to get module size, image is invalid");
		return 0U;
	}

	return pINH->OptionalHeader.SizeOfImage;
}

const wchar_t* MEM::GetModuleBaseFileName(const void* hModuleBase)
{
	const _PEB32* pPEB = reinterpret_cast<_PEB32*>(__readfsdword(0x30));

	if (hModuleBase == nullptr)
		hModuleBase = pPEB->ImageBaseAddress;

	::EnterCriticalSection(pPEB->LoaderLock);

	const wchar_t* wszModuleName = nullptr;
	for (LIST_ENTRY* pListEntry = pPEB->Ldr->InLoadOrderModuleList.Flink; pListEntry != &pPEB->Ldr->InLoadOrderModuleList; pListEntry = pListEntry->Flink)
	{
		const _LDR_DATA_TABLE_ENTRY32* pEntry = CONTAINING_RECORD(pListEntry, _LDR_DATA_TABLE_ENTRY32, InLoadOrderLinks);

		if (pEntry->DllBase == hModuleBase)
		{
			wszModuleName = pEntry->BaseDllName.Buffer;
			break;
		}
	}

	::LeaveCriticalSection(pPEB->LoaderLock);

	return wszModuleName;
}

void* MEM::GetExportAddress(const void* hModuleBase, const char* szProcedureName)
{
	const auto pBaseAddress = static_cast<const std::uint8_t*>(hModuleBase);

	const auto pIDH = static_cast<const IMAGE_DOS_HEADER*>(hModuleBase);
	if (pIDH->e_magic != IMAGE_DOS_SIGNATURE)
		return nullptr;

	const auto pINH = reinterpret_cast<const IMAGE_NT_HEADERS*>(pBaseAddress + pIDH->e_lfanew);
	if (pINH->Signature != IMAGE_NT_SIGNATURE)
		return nullptr;

	const IMAGE_OPTIONAL_HEADER* pIOH = &pINH->OptionalHeader;
	const std::uintptr_t nExportDirectorySize = pIOH->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
	const std::uintptr_t uExportDirectoryAddress = pIOH->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

	if (nExportDirectorySize == 0U || uExportDirectoryAddress == 0U)
	{
		L_PRINT(LOG_ERROR) << Q_XOR("module has no exports: \"") << GetModuleBaseFileName(hModuleBase) << Q_XOR("\"");
		return nullptr;
	}

	const auto pIED = reinterpret_cast<const IMAGE_EXPORT_DIRECTORY*>(pBaseAddress + uExportDirectoryAddress);
	const auto pNamesRVA = reinterpret_cast<const std::uintptr_t*>(pBaseAddress + pIED->AddressOfNames);
	const auto pNameOrdinalsRVA = reinterpret_cast<const std::uint16_t*>(pBaseAddress + pIED->AddressOfNameOrdinals);
	const auto pFunctionsRVA = reinterpret_cast<const std::uintptr_t*>(pBaseAddress + pIED->AddressOfFunctions);

	// since all names are sorted, perform binary search @test: is it really faster than hash comparison
	std::size_t nRight = pIED->NumberOfNames, nLeft = 0U;
	while (nRight != nLeft)
	{
		// avoid INT_MAX/2 overflow
		const std::size_t uMiddle = nLeft + ((nRight - nLeft) >> 1U);
		const int iResult = CRT::StringCompare(szProcedureName, reinterpret_cast<const char*>(pBaseAddress + pNamesRVA[uMiddle]));

		if (iResult == 0)
		{
			const std::uintptr_t uFunctionRVA = pFunctionsRVA[pNameOrdinalsRVA[uMiddle]];

			// check is forwarded export
			if (uFunctionRVA >= uExportDirectoryAddress && uFunctionRVA - uExportDirectoryAddress < nExportDirectorySize)
				// @note: forwarded exports aren't supported
				break;

			return const_cast<std::uint8_t*>(pBaseAddress) + uFunctionRVA;
		}

		if (iResult > 0)
			nLeft = uMiddle;
		else
			nRight = uMiddle;
	}

	L_PRINT(LOG_ERROR) << Q_XOR("export not found: \"") << szProcedureName << Q_XOR("\"");
	return nullptr;
}

void* MEM::GetImportAddress(const void* hModuleBase, const char* szProcedureName)
{
	const auto pBaseAddress = static_cast<const std::uint8_t*>(hModuleBase);

	const auto pIDH = static_cast<const IMAGE_DOS_HEADER*>(hModuleBase);
	if (pIDH->e_magic != IMAGE_DOS_SIGNATURE)
		return nullptr;

	const auto pINH = reinterpret_cast<const IMAGE_NT_HEADERS*>(pBaseAddress + pIDH->e_lfanew);
	if (pINH->Signature != IMAGE_NT_SIGNATURE)
		return nullptr;

	const IMAGE_OPTIONAL_HEADER* pIOH = &pINH->OptionalHeader;
	const std::uintptr_t nImportDirectorySize = pIOH->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size;
	const std::uintptr_t uImportDirectoryAddress = pIOH->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;

	if (nImportDirectorySize == 0U || uImportDirectoryAddress == 0U)
	{
		L_PRINT(LOG_ERROR) << Q_XOR("module has no imports: \"") << GetModuleBaseFileName(hModuleBase) << Q_XOR("\"");
		return nullptr;
	}

	const auto pIID = reinterpret_cast<const IMAGE_IMPORT_DESCRIPTOR*>(pBaseAddress + uImportDirectoryAddress);
	auto pOriginalFirstThunk = reinterpret_cast<const IMAGE_THUNK_DATA*>(pBaseAddress + pIID->OriginalFirstThunk);
	auto pFirstThunk = reinterpret_cast<const IMAGE_THUNK_DATA*>(pBaseAddress + pIID->FirstThunk);

	while (!(pOriginalFirstThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG) && pOriginalFirstThunk->u1.AddressOfData)
	{
		const auto pImageImport = reinterpret_cast<const IMAGE_IMPORT_BY_NAME*>(pBaseAddress + pOriginalFirstThunk->u1.AddressOfData);

		if (CRT::StringCompare(szProcedureName, pImageImport->Name) == 0)
			return reinterpret_cast<void*>(pFirstThunk->u1.Function);

		++pOriginalFirstThunk;
		++pFirstThunk;
	}

	L_PRINT(LOG_ERROR) << Q_XOR("import not found: \"") << szProcedureName << Q_XOR("\"");
	return nullptr;
}

bool MEM::GetSectionInfo(const void* hModuleBase, const char* szSectionName, std::uint8_t** ppSectionStart, std::size_t* pnSectionSize)
{
	const auto pBaseAddress = static_cast<const std::uint8_t*>(hModuleBase);

	const auto pIDH = static_cast<const IMAGE_DOS_HEADER*>(hModuleBase);
	if (pIDH->e_magic != IMAGE_DOS_SIGNATURE)
		return false;

	const auto pINH = reinterpret_cast<const IMAGE_NT_HEADERS*>(pBaseAddress + pIDH->e_lfanew);
	if (pINH->Signature != IMAGE_NT_SIGNATURE)
		return false;

	const IMAGE_SECTION_HEADER* pISH = IMAGE_FIRST_SECTION(pINH);

	// go through all code sections
	for (WORD i = 0U; i < pINH->FileHeader.NumberOfSections; i++, pISH++)
	{
		// @test: use case insensitive comparison instead?
		if (CRT::StringCompareN(szSectionName, reinterpret_cast<const char*>(pISH->Name), IMAGE_SIZEOF_SHORT_NAME) == 0)
		{
			if (ppSectionStart != nullptr)
				*ppSectionStart = const_cast<std::uint8_t*>(pBaseAddress) + pISH->VirtualAddress;

			if (pnSectionSize != nullptr)
				*pnSectionSize = pISH->SizeOfRawData;

			return true;
		}
	}

	L_PRINT(LOG_ERROR) << Q_XOR("code section not found: \"") << szSectionName << Q_XOR("\"");
	return false;
}
#pragma endregion

#pragma region memory_search
std::uint8_t* MEM::FindPattern(const wchar_t* wszModuleName, const char* szPattern)
{
	// convert pattern string to byte array
	const std::size_t nApproximateBufferSize = (CRT::StringLength(szPattern) >> 1U) + 1U;
	std::uint8_t* arrByteBuffer = static_cast<std::uint8_t*>(MEM_STACKALLOC(nApproximateBufferSize));
	char* szMaskBuffer = static_cast<char*>(MEM_STACKALLOC(nApproximateBufferSize));
	PatternToBytes(szPattern, arrByteBuffer, szMaskBuffer);

	// @test: use search with straight in-place conversion? do not think it will be faster, cuz of bunch of new checks that gonna be performed for each iteration
	return FindPattern(wszModuleName, reinterpret_cast<const char*>(arrByteBuffer), szMaskBuffer);
}

std::uint8_t* MEM::FindPattern(const wchar_t* wszModuleName, const char* szBytePattern, const char* szByteMask)
{
	const void* hModuleBase = GetModuleBaseHandle(wszModuleName);

	if (hModuleBase == nullptr)
	{
		L_PRINT(LOG_ERROR) << Q_XOR("failed to get module handle for: \"") << wszModuleName << Q_XOR("\"");
		return nullptr;
	}

	const auto pBaseAddress = static_cast<const std::uint8_t*>(hModuleBase);

	const auto pIDH = static_cast<const IMAGE_DOS_HEADER*>(hModuleBase);
	if (pIDH->e_magic != IMAGE_DOS_SIGNATURE)
	{
		L_PRINT(LOG_ERROR) << Q_XOR("failed to get module size, image is invalid");
		return nullptr;
	}

	const auto pINH = reinterpret_cast<const IMAGE_NT_HEADERS*>(pBaseAddress + pIDH->e_lfanew);
	if (pINH->Signature != IMAGE_NT_SIGNATURE)
	{
		L_PRINT(LOG_ERROR) << Q_XOR("failed to get module size, image is invalid");
		return nullptr;
	}

	const std::uint8_t* arrByteBuffer = reinterpret_cast<const std::uint8_t*>(szBytePattern);
	const std::size_t nByteCount = CRT::StringLength(szByteMask);

	std::uint8_t* pFoundAddress = nullptr;

	// perform little overhead to keep all patterns unique
#ifdef Q_PARANOID_PATTERN_UNIQUENESS
	const std::vector<std::uint8_t*> vecFoundOccurrences = FindPatternAllOccurrencesEx(pBaseAddress, pINH->OptionalHeader.SizeOfImage, arrByteBuffer, nByteCount, szByteMask);

	// notify user about non-unique pattern
	if (!vecFoundOccurrences.empty())
	{
		// notify user about non-unique pattern
		if (vecFoundOccurrences.size() > 1U)
		{
			char* szPattern = static_cast<char*>(MEM_STACKALLOC((nByteCount << 1U) + nByteCount));
			const std::size_t nConvertedPatternLength = BytesToPattern(arrByteBuffer, nByteCount, szPattern);

			L_PRINT(LOG_WARNING) << Q_XOR("found more than one occurrence with \"") << szPattern << Q_XOR("\" pattern, consider updating it!");

			MEM_STACKFREE(szPattern);
		}

		// return first found occurrence
		pFoundAddress = vecFoundOccurrences[0];
	}
#else
	// @todo: we also can go through code sections and skip noexec pages, but will it really improve performance? / or at least for all occurrences search
	// https://docs.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-image_section_header
#if 0
	IMAGE_SECTION_HEADER* pCurrentSection = IMAGE_FIRST_SECTION(pINH);
	for (WORD i = 0U; i != pINH->FileHeader.NumberOfSections; i++)
	{
		// check does page have executable code
		if (pCurrentSection->Characteristics & IMAGE_SCN_CNT_CODE || pCurrentSection->Characteristics & IMAGE_SCN_MEM_EXECUTE)
		{
			pFoundAddress = FindPatternEx(pBaseAddress + pCurrentSection->VirtualAddress, pCurrentSection->SizeOfRawData, arrByteBuffer, nByteCount, szByteMask);

			if (pFoundAddress != nullptr)
				break;
		}

		++pCurrentSection;
	}
#else
	pFoundAddress = FindPatternEx(pBaseAddress, pINH->OptionalHeader.SizeOfImage, arrByteBuffer, nByteCount, szByteMask);
#endif
#endif

	if (pFoundAddress == nullptr)
	{
		char* szPattern = static_cast<char*>(MEM_STACKALLOC((nByteCount << 1U) + nByteCount));
		[[maybe_unused]] const std::size_t nConvertedPatternLength = BytesToPattern(arrByteBuffer, nByteCount, szPattern);

		L_PRINT(LOG_ERROR) << Q_XOR("pattern not found: \"") << szPattern << Q_XOR("\"");

		MEM_STACKFREE(szPattern);
	}

	return pFoundAddress;
}

// @todo: msvc poorly optimizes this, it looks even better w/o optimization at all
std::uint8_t* MEM::FindPatternEx(const std::uint8_t* pRegionStart, const std::size_t nRegionSize, const std::uint8_t* arrByteBuffer, const std::size_t nByteCount, const char* szByteMask)
{
	std::uint8_t* pCurrentAddress = const_cast<std::uint8_t*>(pRegionStart);
	const std::uint8_t* pRegionEnd = pRegionStart + nRegionSize - nByteCount;
	const bool bIsMaskUsed = (szByteMask != nullptr);

	while (pCurrentAddress < pRegionEnd)
	{
		// check the first byte before entering the loop, otherwise if there two consecutive bytes of first byte in the buffer, we may skip both and fail the search
		if ((bIsMaskUsed && *szByteMask == '?') || *pCurrentAddress == *arrByteBuffer)
		{
			if (nByteCount == 1)
				return pCurrentAddress;

			// compare the least byte sequence and continue on wildcard or skip forward on first mismatched byte
			std::size_t nComparedBytes = 0U;
			while ((bIsMaskUsed && szByteMask[nComparedBytes + 1U] == '?') || pCurrentAddress[nComparedBytes + 1U] == arrByteBuffer[nComparedBytes + 1U])
			{
				// check does byte sequence match
				if (++nComparedBytes == nByteCount - 1U)
					return pCurrentAddress;
			}

			// skip non suitable bytes
			pCurrentAddress += nComparedBytes;
		}

		++pCurrentAddress;
	}

	return nullptr;
}

std::vector<std::uint8_t*> MEM::FindPatternAllOccurrencesEx(const std::uint8_t* pRegionStart, const std::size_t nRegionSize, const std::uint8_t* arrByteBuffer, const std::size_t nByteCount, const char* szByteMask)
{
	const std::uint8_t* pRegionEnd = pRegionStart + nRegionSize - nByteCount;
	const bool bIsMaskUsed = (szByteMask != nullptr);

	// container for addresses of the all found occurrences
	std::vector<std::uint8_t*> vecOccurrences = { };

	for (std::uint8_t* pCurrentByte = const_cast<std::uint8_t*>(pRegionStart); pCurrentByte < pRegionEnd; ++pCurrentByte)
	{
		// do a first byte check before entering the loop, otherwise if there two consecutive bytes of first byte in the buffer, we may skip both and fail the search
		if ((!bIsMaskUsed || *szByteMask != '?') && *pCurrentByte != *arrByteBuffer)
			continue;

		// check for bytes sequence match
		bool bSequenceMatch = true;
		for (std::size_t i = 1U; i < nByteCount; i++)
		{
			// compare sequence and continue on wildcard or skip forward on first mismatched byte
			if ((!bIsMaskUsed || szByteMask[i] != '?') && pCurrentByte[i] != arrByteBuffer[i])
			{
				// skip non suitable bytes
				pCurrentByte += i - 1U;

				bSequenceMatch = false;
				break;
			}
		}

		// check did we found address
		if (bSequenceMatch)
			vecOccurrences.push_back(pCurrentByte);
	}

	return vecOccurrences;
}

RTTITypeDescriptor_t* MEM::FindClassTypeDescriptor(const void* hModuleHandle, const char* szClassName)
{
	std::uint8_t* pDataStart;
	std::size_t nDataSize;

	// type descriptors are located in '.data' section
	if (GetSectionInfo(hModuleHandle, Q_XOR(".data"), &pDataStart, &nDataSize))
	{
		const std::size_t nTypeDescriptorNameLength = CRT::StringLength(szClassName) + 7U;
		char* szTypeDescriptorName = static_cast<char*>(MEM_STACKALLOC(nTypeDescriptorNameLength));

		// build mangled type name for a class
		CRT::StringCat(CRT::StringCat(CRT::StringCopy(szTypeDescriptorName, Q_XOR(".?AV")), szClassName), Q_XOR("@@"));

		std::uint8_t* pTypeDescriptorNameAddress = FindPatternEx(pDataStart, nDataSize, reinterpret_cast<const std::uint8_t*>(szTypeDescriptorName), nTypeDescriptorNameLength);
		MEM_STACKFREE(szTypeDescriptorName);

		if (pTypeDescriptorNameAddress != nullptr)
			return reinterpret_cast<RTTITypeDescriptor_t*>(pTypeDescriptorNameAddress - Q_OFFSETOF(RTTITypeDescriptor_t, szName));
	}

	L_PRINT(LOG_ERROR) << Q_XOR("class type descriptor not found: \"") << szClassName << Q_XOR("\"");
	return nullptr;
}

std::uint8_t* MEM::FindVTable(const wchar_t* wszModuleName, const char* szVTableName)
{
	/*
	 * rtti (run-time type information) memory layout:
	 *
	 *                     *---------------------------*      *----------------------------*
	 *                     |       .rdata section      |  o--->  complete object locator   |
	 * *---------------*   *---------------------------*  |   *----------------------------*
	 * |     this      |   | complete object locator   ---o   | signature                  |
	 * *---------------*   |---------------------------|      |----------------------------|
	 * | virtual table ----> virtual method pointer #n ---o   | offset                     |
	 * |---------------|   *---------------------------*  |   |----------------------------|
	 * | attribute #n  |                                  |   | constructor disp offset    |
	 * *---------------*     o----------------------------o   |----------------------------|
	 *                       |                                | type descriptor            --o
	 * *-------------------* | *----------------------------* |----------------------------| |
	 * |   .text section   | | | class hierarchy descriptor <-- class hierarchy descriptor | |
	 * *-------------------* | *----------------------------* |----------------------------| |
	 * | method #n         | | | signature                  | | object base offset [1]     | |
	 * |-------------------| | |----------------------------| *----------------------------* |
	 * | virtual method #n <-o | attributes                 |                                | *-----------------*
	 * |-------------------|   |----------------------------|                                o-> type descriptor |
	 * | static method #n  |   | count of base classes      |                                | *-----------------*
	 * *-------------------*   |----------------------------| *----------------------------* | | type info       |
	 *                         | base class descriptor #n   -->   base class descriptor    | | |-----------------|
	 * *-------------------*   *----------------------------* *----------------------------* | | data            |
	 * |   .data section   |                                  | type descriptor            --o |-----------------|
	 * *-------------------*                                  |----------------------------|   | class name [3]  |
	 * | static attributes |                                  | count of contained bases   |   *-----------------*
	 * *-------------------*                                  |----------------------------|
	 *                                                        | pointer to member [2]      |
	 *                                                        |----------------------------|
	 *                                                        | attributes                 |
	 *                                                        *----------------------------*
	 *
	 * [1]: object base offset, unique for 64-bit platform
	 * [2]: pointer-to-member descriptor with following values:
	 *      1. 'mdisp' - member offset
	 *      2. 'pdisp' - offset of the virtual base table (-1 if not a virtual base)
	 *      3. 'vdisp' - offset to the displacement value inside the virtual base table
	 * [3]: mangled type name, compiler-specific, e.g. on MSVC ".H" = "int", ".?AUName@@" = "struct Name", ".?AVName@@" = "class Name"
	 */

	if (const void* hModuleHandle = GetModuleBaseHandle(wszModuleName); hModuleHandle != nullptr)
	{
		if (const RTTITypeDescriptor_t* pTypeDescriptor = FindClassTypeDescriptor(hModuleHandle, szVTableName); pTypeDescriptor != nullptr)
		{
			std::uint8_t *pRDataStart, *pTextStart;
			std::size_t nRDataSize, nTextSize;

			if (GetSectionInfo(hModuleHandle, Q_XOR(".rdata"), &pRDataStart, &nRDataSize) && GetSectionInfo(hModuleHandle, Q_XOR(".text"), &pTextStart, &nTextSize))
			{
				// go through all cross-references of the type descriptor inside the '.rdata' section
				for (const std::uint8_t* pCrossReference : FindPatternAllOccurrencesEx(pRDataStart, nRDataSize, reinterpret_cast<const std::uint8_t*>(&pTypeDescriptor), sizeof(std::uintptr_t)))
				{
					const auto pObjectLocator = reinterpret_cast<const RTTICompleteObjectLocator_t*>(pCrossReference - Q_OFFSETOF(RTTICompleteObjectLocator_t, pTypeDescriptor));

					// get offset of the complete object locator, 0 means it's the base class, and not some class it inherits from
					// @todo: ability to search vtables with inheritances | we will be also able to rebuild dynamic_cast completely
					if (pObjectLocator->uOffset != 0U)
						continue;

					// get a pointer to the object locator in the '.rdata' section
					const std::uint8_t* pVTableRData = FindPatternEx(pRDataStart, nRDataSize, reinterpret_cast<const std::uint8_t*>(&pObjectLocator), sizeof(std::uintptr_t));

					// check is address valid
					if (pVTableRData == nullptr)
						break;

					// skip object locator itself, to point vtable
					pVTableRData += sizeof(std::uintptr_t);

					// get a pointer to the vtable in the '.text' section
					std::uint8_t* pVTableText = FindPatternEx(pTextStart, nTextSize, reinterpret_cast<const std::uint8_t*>(&pVTableRData), sizeof(std::uintptr_t));

					if (pVTableText == nullptr)
						break;

					return pVTableText;
				}
			}
		}
	}

	L_PRINT(LOG_ERROR) << Q_XOR("virtual table pointer not found: \"") << szVTableName << Q_XOR("\"");
	return nullptr;
}
#pragma endregion

#pragma region memory_extra
std::size_t MEM::PatternToBytes(const char* szPattern, std::uint8_t* pOutByteBuffer, char* szOutMaskBuffer)
{
	std::uint8_t* pCurrentByte = pOutByteBuffer;

	while (*szPattern != '\0')
	{
		// check is a wildcard
		if (*szPattern == '?')
		{
			++szPattern;
		#ifdef Q_PARANOID
			Q_ASSERT(*szPattern == '\0' || *szPattern == ' ' || *szPattern == '?'); // we're expect that next character either terminating null, whitespace or part of double wildcard (note that it's required if your pattern written without whitespaces)
		#endif

			// ignore that
			*pCurrentByte++ = 0U;
			*szOutMaskBuffer++ = '?';
		}
		// check is not space
		else if (*szPattern != ' ')
		{
			// convert two consistent numbers in a row to byte value
			std::uint8_t uByte = static_cast<std::uint8_t>(CRT::CharToHexInt(*szPattern) << 4);

			++szPattern;
		#ifdef Q_PARANOID
			Q_ASSERT(*szPattern != '\0' && *szPattern != '?' && *szPattern != ' '); // we're expect that byte always represented by two numbers in a row
		#endif

			uByte |= static_cast<std::uint8_t>(CRT::CharToHexInt(*szPattern));

			*pCurrentByte++ = uByte;
			*szOutMaskBuffer++ = 'x';
		}

		++szPattern;
	}

	// zero terminate both buffers
	*pCurrentByte = 0U;
	*szOutMaskBuffer = '\0';

	return pCurrentByte - pOutByteBuffer;
}

std::size_t MEM::BytesToPattern(const std::uint8_t* pByteBuffer, const std::size_t nByteCount, char* szOutBuffer)
{
	char* szCurrentPattern = szOutBuffer;

	for (std::size_t i = 0U; i < nByteCount; i++)
	{
		// manually convert byte to chars
		const char* szHexByte = &CRT::_TWO_DIGITS_HEX_LUT[pByteBuffer[i] * 2U];
		*szCurrentPattern++ = szHexByte[0];
		*szCurrentPattern++ = szHexByte[1];
		*szCurrentPattern++ = ' ';
	}
	*--szCurrentPattern = '\0';

	return szCurrentPattern - szOutBuffer;
}
#pragma endregion
