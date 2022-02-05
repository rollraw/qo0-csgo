// used: __readfsdword
#include <intrin.h>

#include "memory.h"

// used: _PEB struct
#include "memory/pe32.h"
// used: bad patterns logging
#include "logging.h"

std::uintptr_t MEM::FindPattern(const std::string_view szModuleName, const std::string_view szPattern)
{
	void* hModule = GetModuleBaseHandle(szModuleName);

	if (hModule == nullptr)
		throw std::runtime_error(std::format(XorStr("failed to get handle for: {}"), szModuleName));

	const std::uint8_t* uModuleAddress = static_cast<std::uint8_t*>(hModule);
	const IMAGE_DOS_HEADER* pDosHeader = static_cast<IMAGE_DOS_HEADER*>(hModule);
	const IMAGE_NT_HEADERS* pNtHeaders = reinterpret_cast<const IMAGE_NT_HEADERS*>(uModuleAddress + pDosHeader->e_lfanew);

	return FindPattern(uModuleAddress, pNtHeaders->OptionalHeader.SizeOfImage, szPattern);
}

std::uintptr_t MEM::FindPattern(const std::uint8_t* uRegionStart, const std::uintptr_t uRegionSize, const std::string_view szPattern)
{
	const std::vector<std::optional<std::uint8_t>> vecBytes = PatternToBytes(szPattern);

	// check for bytes sequence match
	for (std::uintptr_t i = 0U; i < uRegionSize - vecBytes.size(); ++i)
	{
		bool bByteFound = true;

		for (std::uintptr_t s = 0U; s < vecBytes.size(); ++s)
		{
			// compare byte and skip if wildcard
			if (vecBytes[s].has_value() && uRegionStart[i + s] != vecBytes[s].value())
			{
				bByteFound = false;
				break;
			}
		}

		// return valid address
		if (bByteFound)
			return reinterpret_cast<std::uintptr_t>(&uRegionStart[i]);
	}

	L::PushConsoleColor(FOREGROUND_INTENSE_RED);
	L::Print(XorStr("[error] pattern not found: [{}]"), szPattern);
	L::PopConsoleColor();
	return 0U;
}

void* MEM::GetModuleBaseHandle(const std::string_view szModuleName)
{
	const _PEB32* pPEB = reinterpret_cast<_PEB32*>(__readfsdword(0x30)); // mov eax, fs:[0x30]
	//const _TEB32* pTEB = reinterpret_cast<_TEB32*>(__readfsdword(0x18)); // mov eax, fs:[0x18]
	//const _PEB32* pPEB = pTEB->ProcessEnvironmentBlock;

	if (szModuleName.empty())
		return pPEB->ImageBaseAddress;
	
	const std::wstring wszModuleName(szModuleName.begin(), szModuleName.end());

	for (LIST_ENTRY* pListEntry = pPEB->Ldr->InLoadOrderModuleList.Flink; pListEntry != &pPEB->Ldr->InLoadOrderModuleList; pListEntry = pListEntry->Flink)
	{
		const _LDR_DATA_TABLE_ENTRY* pEntry = CONTAINING_RECORD(pListEntry, _LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

		if (pEntry->BaseDllName.Buffer && wszModuleName.compare(pEntry->BaseDllName.Buffer) == 0)
			return pEntry->DllBase;
	}

	L::PushConsoleColor(FOREGROUND_INTENSE_RED);
	L::Print(XorStr("[error] module base not found: [{}]"), szModuleName);
	L::PopConsoleColor();
	return nullptr;
}

// @todo: GetImportAddress also
void* MEM::GetExportAddress(const void* pModuleBase, const std::string_view szProcedureName)
{
	const std::uint8_t* pAddress = static_cast<const std::uint8_t*>(pModuleBase);
	const IMAGE_DOS_HEADER* pDosHeader = static_cast<const IMAGE_DOS_HEADER*>(pModuleBase);
	const IMAGE_NT_HEADERS* pNtHeaders = reinterpret_cast<const IMAGE_NT_HEADERS*>(pAddress + pDosHeader->e_lfanew);
	const IMAGE_OPTIONAL_HEADER* pOptionalHeader = &pNtHeaders->OptionalHeader;

	const std::uintptr_t uExportSize = pOptionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
	const std::uintptr_t uExportAddress = pOptionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

	if (uExportSize <= 0U)
		return nullptr;

	const IMAGE_EXPORT_DIRECTORY* pExportDirectory = reinterpret_cast<const IMAGE_EXPORT_DIRECTORY*>(pAddress + uExportAddress);
	const std::uintptr_t* pNamesRVA = reinterpret_cast<const std::uintptr_t*>(pAddress + pExportDirectory->AddressOfNames);
	const std::uintptr_t* pFunctionsRVA = reinterpret_cast<const std::uintptr_t*>(pAddress + pExportDirectory->AddressOfFunctions);
	const std::uint16_t* pNameOrdinals = reinterpret_cast<const std::uint16_t*>(pAddress + pExportDirectory->AddressOfNameOrdinals);

	// perform binary search
	std::uintptr_t uRight = pExportDirectory->NumberOfNames;
	std::uintptr_t uLeft = 0;
	
	while (uRight != uLeft)
	{
		const std::uintptr_t uMiddle = uLeft + ((uRight - uLeft) >> 1U);
		const int iResult = szProcedureName.compare(reinterpret_cast<const char*>(pAddress + pNamesRVA[uMiddle]));

		if (iResult == 0)
			return const_cast<void*>(static_cast<const void*>(pAddress + pFunctionsRVA[pNameOrdinals[uMiddle]]));

		if (iResult > 0)
			uLeft = uMiddle;
		else
			uRight = uMiddle;
	}

	L::PushConsoleColor(FOREGROUND_INTENSE_RED);
	L::Print(XorStr("[error] export not found: [{}]"), szProcedureName);
	L::PopConsoleColor();
	return nullptr;
}

std::vector<std::uintptr_t> MEM::GetCrossReferences(const std::uintptr_t uAddress, std::uintptr_t uRegionStart, const std::size_t uRegionSize)
{
	std::vector<std::uintptr_t> vecCrossReferences = { };

	// convert the address over to an ida pattern string
	const std::string szPattern = BytesToPattern(reinterpret_cast<const std::uint8_t*>(&uAddress), sizeof(std::uintptr_t));
	// get the end of the section (in our case the end of the .rdata section)
	const std::uintptr_t uRegionEnd = uRegionStart + uRegionSize;

	while (uRegionStart > 0U && uRegionStart < uRegionEnd)
	{
		// @todo: findpattern shouldn't report on fail
		std::uintptr_t uReferenceAddress = FindPattern(reinterpret_cast<std::uint8_t*>(uRegionStart), uRegionSize, szPattern.c_str());

		// if the xref is 0 it means that there either were no xrefs, or there are no remaining xrefs
		if (uReferenceAddress == 0U)
			break;

		vecCrossReferences.push_back(uReferenceAddress);

		// skip current xref for next search
		uRegionStart = uReferenceAddress + sizeof(std::uintptr_t);
	}

	return vecCrossReferences;
}

bool MEM::GetSectionInfo(const std::uintptr_t uBaseAddress, const std::string_view szSectionName, std::uintptr_t* puSectionStart, std::uintptr_t* puSectionSize)
{
	const IMAGE_DOS_HEADER* pDosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(uBaseAddress);
	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return false;

	const IMAGE_NT_HEADERS* pNtHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(uBaseAddress + pDosHeader->e_lfanew);
	if (pNtHeaders->Signature != IMAGE_NT_SIGNATURE)
		return false;

	IMAGE_SECTION_HEADER* pSectionHeader = IMAGE_FIRST_SECTION(pNtHeaders);
	std::uint16_t uNumberOfSections = pNtHeaders->FileHeader.NumberOfSections;

	while (uNumberOfSections > 0U)
	{
		// if we're at the right section
		if (szSectionName.starts_with(reinterpret_cast<const char*>(pSectionHeader->Name)))
		{
			if (puSectionStart != nullptr)
				*puSectionStart = uBaseAddress + pSectionHeader->VirtualAddress;

			if (puSectionSize != nullptr)
				*puSectionSize = pSectionHeader->SizeOfRawData;

			return true;
		}

		pSectionHeader++;
		uNumberOfSections--;
	}

	return false;
}

std::uintptr_t MEM::GetVTableTypeDescriptor(const std::string_view szModuleName, const std::string_view szTableName)
{
	// type descriptor names look like this: '.?AVC_CSPlayer@@'
	const std::string szTypeDescriptorName = std::string(".?AV").append(szTableName).append("@@");
	const std::string szPattern = BytesToPattern(reinterpret_cast<const std::uint8_t*>(szTypeDescriptorName.data()), szTypeDescriptorName.size());

	// get rtti type descriptor, located 0x8 bytes before the string
	if (const std::uintptr_t uTypeDescriptor = FindPattern(szModuleName, szPattern); uTypeDescriptor != 0U)
		return uTypeDescriptor - 0x8;

	L::PushConsoleColor(FOREGROUND_INTENSE_RED);
	L::Print(XorStr("[error] virtual table type descriptor not found: [{}] [{}]"), szModuleName, szTableName);
	L::PopConsoleColor();
	return 0U;
}

std::uintptr_t* MEM::GetVTablePointer(const std::string_view szModuleName, const std::string_view szTableName)
{
	const std::uintptr_t uBaseAddress = reinterpret_cast<std::uintptr_t>(GetModuleBaseHandle(szModuleName));

	if (uBaseAddress == 0U)
		return nullptr;

	const std::uintptr_t uTypeDescriptor = GetVTableTypeDescriptor(szModuleName, szTableName);

	if (uTypeDescriptor == 0U)
		return nullptr;

	// we only need to get xrefs that are inside the .rdata section (there sometimes are xrefs in .text, so we have to filter them out)
	std::uintptr_t uRDataStart = 0U, uRDataSize = 0U;
	if (!GetSectionInfo(uBaseAddress, XorStr(".rdata"), &uRDataStart, &uRDataSize))
		return nullptr;

	// go through all xrefs of the type descriptor
	for (const auto& uCrossReference : GetCrossReferences(uTypeDescriptor, uRDataStart, uRDataSize))
	{
		// get offset of vtable in complete class, 0 means it's the class we need, and not some class it inherits from
		if (const int uVTableOffset = *reinterpret_cast<int*>(uCrossReference - 0x8); uVTableOffset != 0)
			continue;

		// get the object locator
		const std::uintptr_t uObjectLocator = uCrossReference - 0xC;

		// get a pointer to the vtable
		std::string szPattern = BytesToPattern(reinterpret_cast<const std::uint8_t*>(&uObjectLocator), sizeof(std::uintptr_t));
		const std::uintptr_t uVTableAddress = FindPattern(reinterpret_cast<std::uint8_t*>(uRDataStart), uRDataSize, szPattern.c_str()) + 0x4;

		// check is valid offset
		if (uVTableAddress <= sizeof(std::uintptr_t))
			return nullptr;

		// check for .text section
		std::uintptr_t uTextStart = 0U, uTextSize = 0U;
		if (!GetSectionInfo(uBaseAddress, XorStr(".text"), &uTextStart, &uTextSize))
			return nullptr;

		// convert the vtable address to an ida pattern
		szPattern = BytesToPattern(reinterpret_cast<const std::uint8_t*>(&uVTableAddress), sizeof(std::uintptr_t));
		return reinterpret_cast<std::uintptr_t*>(FindPattern(reinterpret_cast<std::uint8_t*>(uTextStart), uTextSize, szPattern.c_str()));
	}

	L::PushConsoleColor(FOREGROUND_INTENSE_RED);
	L::Print(XorStr("[error] virtual table pointer not found: [{}] [{}]"), szModuleName, szTableName);
	L::PopConsoleColor();
	return nullptr;
}

bool MEM::IsValidCodePtr(const void* pPointer)
{
	if (pPointer == nullptr)
		return false;

	MEMORY_BASIC_INFORMATION memInfo = { };

	if (VirtualQuery(pPointer, &memInfo, sizeof(MEMORY_BASIC_INFORMATION)) == 0U)
		return false;

	if (!(memInfo.Protect & PAGE_EXECUTE_READWRITE || memInfo.Protect & PAGE_EXECUTE_READ))
		return false;

	return true;
}

std::vector<std::optional<std::uint8_t>> MEM::PatternToBytes(const std::string_view szPattern)
{
	std::vector<std::optional<std::uint8_t>> vecBytes = { };
	auto itBegin = szPattern.cbegin();
	const auto itEnd = szPattern.cend();

	// convert pattern into bytes
	while (itBegin < itEnd)
	{
		// check is current byte a wildcard
		if (*itBegin == '?')
		{
			// check is two-character wildcard
			if (++itBegin; *itBegin == '?')
				++itBegin;

			// ignore that
			vecBytes.emplace_back(std::nullopt);
		}
		// check is not space
		else if (*itBegin != ' ')
		{
			// convert current 4 bits to hex
			std::uint8_t uByte = static_cast<std::uint8_t>(((*itBegin >= 'A' ? (((*itBegin - 'A') & (~('a' ^ 'A'))) + 10) : (*itBegin <= '9' ? *itBegin - '0' : 0x0)) | 0xF0) << 4);
			
			// convert next 4 bits to hex and assign to byte
			if (++itBegin; *itBegin != ' ')
				uByte |= static_cast<std::uint8_t>(*itBegin >= 'A' ? (((*itBegin - 'A') & (~('a' ^ 'A'))) + 10) : (*itBegin <= '9' ? *itBegin - '0' : 0x0));

			vecBytes.emplace_back(uByte);
		}

		++itBegin;
	}

	return vecBytes;
}

std::string MEM::BytesToPattern(const std::uint8_t* arrBytes, const std::size_t uSize)
{
	constexpr const char* szHexDigits = "0123456789ABCDEF";
	const std::size_t nHexLength = (uSize << 1U) + uSize;

	// construct pre-reserved string filled with spaces
	std::string szPattern(nHexLength - 1U, ' ');

	for (std::size_t i = 0U, n = 0U; i < nHexLength; n++, i += 3U)
	{
		const std::uint8_t uCurrentByte = arrBytes[n];

		// manually convert byte to chars
		szPattern[i] = szHexDigits[((uCurrentByte & 0xF0) >> 4)];
		szPattern[i + 1U] = szHexDigits[(uCurrentByte & 0x0F)];
	}

	return szPattern;
}
