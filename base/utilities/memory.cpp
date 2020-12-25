// std::setfill, setw
#include <iomanip>

#include "memory.h"
// used: bad patterns logging
#include "logging.h"

std::uintptr_t MEM::FindPattern(const char* szModuleName, const char* szPattern)
{
	const HMODULE hModule = GetModuleHandle(szModuleName);

	if (hModule == nullptr)
		throw std::runtime_error(fmt::format(XorStr("failed to get handle for: {}"), szModuleName));

	const auto uModuleAdress = reinterpret_cast<std::uint8_t*>(hModule);
	const auto pDosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(hModule);
	const auto pNtHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(uModuleAdress + pDosHeader->e_lfanew);
	const std::uintptr_t uOffset = FindPattern(uModuleAdress, pNtHeaders->OptionalHeader.SizeOfImage, szPattern);

	if (!uOffset)
	{
		L::PushConsoleColor(FOREGROUND_INTENSE_RED);
		L::Print(fmt::format(XorStr("[error] failed get pattern: [{}] [{}]"), szModuleName, szPattern));
		L::PopConsoleColor();
	}

	return uOffset;
}

std::uintptr_t MEM::FindPattern(std::uint8_t* uRegionStart, std::uintptr_t uRegionSize, const char* szPattern)
{
	std::vector<int> vecBytes = PatternToBytes(szPattern);

	// check for bytes sequence match
	for (unsigned long i = 0UL; i < uRegionSize - vecBytes.size(); ++i)
	{
		bool bByteFound = true;

		for (unsigned long s = 0UL; s < vecBytes.size(); ++s)
		{
			// check if doesn't match or byte isn't a wildcard
			if (uRegionStart[i + s] != vecBytes[s] && vecBytes[s] != -1)
			{
				bByteFound = false;
				break;
			}
		}

		// return valid address
		if (bByteFound)
			return reinterpret_cast<std::uintptr_t>(&uRegionStart[i]);
	}

	return 0U;
}

std::vector<std::uintptr_t> MEM::GetXrefs(std::uintptr_t uAddress, std::uintptr_t uStart, std::size_t uSize)
{
	std::vector<std::uintptr_t> vecXrefs = { };

	// convert the address over to an ida pattern string
	const std::string szPattern = BytesToPattern((std::uint8_t*)&uAddress, 4U);
	// get the end of the section (in our case the end of the .rdata section)
	const std::uintptr_t uEnd = uStart + uSize;

	while (uStart && uStart < uEnd)
	{
		std::uintptr_t uXrefAddress = FindPattern((std::uint8_t*)uStart, uSize, szPattern.c_str());

		// if the xref is 0 it means that there either were no xrefs, or there are no remaining xrefs.
		if (!uXrefAddress)
			break;

		// we've found an xref, save it in the vector, and add 4 to start, so it wil now search for xrefs
		// from the previously found xref untill we're at the end of the section, or there aren't any xrefs left.
		vecXrefs.push_back(uXrefAddress);
		uStart = uXrefAddress + 4U;
	}

	return vecXrefs;
}

bool MEM::GetSectionInfo(std::uintptr_t uBaseAddress, const std::string& szSectionName, std::uintptr_t& uSectionStart, std::uintptr_t& uSectionSize)
{
	const auto pDosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(uBaseAddress);
	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return false;

	const auto pNtHeaders = reinterpret_cast<PIMAGE_NT_HEADERS32>(uBaseAddress + pDosHeader->e_lfanew);
	if (pNtHeaders->Signature != IMAGE_NT_SIGNATURE)
		return false;

	IMAGE_SECTION_HEADER* pSectionHeader = IMAGE_FIRST_SECTION(pNtHeaders);
	std::uint16_t uNumberOfSections = pNtHeaders->FileHeader.NumberOfSections;

	while (uNumberOfSections > 0U)
	{
		// if we're at the right section
		if (!strcmp(szSectionName.c_str(), reinterpret_cast<const char*>(pSectionHeader->Name)))
		{
			uSectionStart = uBaseAddress + pSectionHeader->VirtualAddress;
			uSectionSize = pSectionHeader->SizeOfRawData;
			return true;
		}

		pSectionHeader++;
		uNumberOfSections--;
	}

	return false;
}

std::uintptr_t* MEM::GetVTablePointer(std::string_view szModuleName, std::string_view szTableName)
{
	std::uintptr_t uBaseAddress = reinterpret_cast<std::uintptr_t>(GetModuleHandle(szModuleName.data()));

	if (!uBaseAddress)
		return nullptr;

	// type descriptor names look like this: .?AVC_CSPlayer@@ (so: ".?AV" + szTableName + "@@")
	const std::string szTypeDescriptorName = std::string(".?AV").append(szTableName).append("@@");
	std::string szPattern = BytesToPattern((std::uint8_t*)szTypeDescriptorName.data(), szTypeDescriptorName.size());

	// get rtti type descriptor
	std::uintptr_t uTypeDescriptor = FindPattern(szModuleName.data(), szPattern.data());

	if (!uTypeDescriptor)
		return nullptr;

	// we're doing - 0x8 here, because the location of the rtti typedescriptor is 0x8 bytes before the string
	uTypeDescriptor -= 0x8;

	// we only need to get xrefs that are inside the .rdata section (there sometimes are xrefs in .text, so we have to filter them out)
	std::uintptr_t uRDataStart = 0U, uRDataSize = 0U;
	if (!GetSectionInfo(uBaseAddress, XorStr(".rdata"), uRDataStart, uRDataSize))
		return nullptr;

	// get all xrefs to the type_descriptor
	const std::vector<std::uintptr_t> vecXrefs = GetXrefs(uTypeDescriptor, uRDataStart, uRDataSize);

	for (const auto& uXref : vecXrefs)
	{
		// get offset of this vtable in complete class
		const int uVTableOffset = *reinterpret_cast<int*>(uXref - 0x8);

		// so if it's 0 it means it's the class we need, and not some class it inherits from
		if (uVTableOffset != 0)
			continue;

		// get the object locator
		const std::uintptr_t uObjectLocator = uXref - 0xC;

		szPattern = BytesToPattern((std::uint8_t*)&uObjectLocator, 4U);
		const std::uintptr_t uVTableAddress = FindPattern((std::uint8_t*)uRDataStart, uRDataSize, szPattern.c_str()) + 0x4;

		// check is valid offset
		if (uVTableAddress <= 4U)
			return nullptr;

		// get a pointer to the vtable
		std::uintptr_t uTextStart = 0U, uTextSize = 0U;
		if (!GetSectionInfo(uBaseAddress, XorStr(".text"), uTextStart, uTextSize))
			return nullptr;

		// convert the vtable address to an ida pattern
		szPattern = BytesToPattern((std::uint8_t*)&uVTableAddress, 4U);
		return reinterpret_cast<std::uintptr_t*>(FindPattern((std::uint8_t*)uTextStart, uTextSize, szPattern.c_str()));
	}

	return nullptr;
}

bool MEM::IsValidCodePtr(std::uintptr_t uAddress)
{
	if (uAddress == 0U)
		return false;

	MEMORY_BASIC_INFORMATION memInfo = { };

	if (VirtualQuery(reinterpret_cast<LPCVOID>(uAddress), &memInfo, sizeof(memInfo)) == 0U)
		return false;

	if (!(memInfo.Protect & PAGE_EXECUTE_READWRITE || memInfo.Protect & PAGE_EXECUTE_READ))
		return false;

	return true;
}

std::vector<int> MEM::PatternToBytes(const char* szPattern)
{
	std::vector<int> vecBytes = { };
	char* chStart = const_cast<char*>(szPattern);
	char* chEnd = chStart + strlen(szPattern);

	// convert pattern into bytes
	for (char* chCurrent = chStart; chCurrent < chEnd; ++chCurrent)
	{
		// check is current byte a wildcard
		if (*chCurrent == '?')
		{
			++chCurrent;

			// check is next byte is also wildcard
			if (*chCurrent == '?')
				++chCurrent;

			// ignore that
			vecBytes.push_back(-1);
		}
		else
			// convert byte to hex
			vecBytes.push_back(strtoul(chCurrent, &chCurrent, 16));
	}

	return vecBytes;
}

std::string MEM::BytesToPattern(std::uint8_t* arrBytes, std::size_t uSize)
{
	std::stringstream ssPattern;
	ssPattern << std::hex << std::setfill('0');

	for (std::size_t i = 0U; i < uSize; i++)
	{
		const int iCurrentByte = arrBytes[i];
		if (iCurrentByte != 255)
			ssPattern << std::setw(2) << iCurrentByte;
		else
			ssPattern << std::setw(1) << '?';

		if (i != uSize - 1U)
			ssPattern << ' ';
	}

	return ssPattern.str();
}
