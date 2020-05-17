#include "memory.h"

// used: logging
#include "logging.h"

std::uintptr_t MEM::FindPattern(const char* szModuleName, const char* szPattern)
{
	static auto PatternByte = [](const char* szPattern) -> std::vector<int>
	{
		std::vector<int> vecBytes{ };
		char* chStart = (char*)szPattern;
		char* chEnd = (char*)szPattern + strlen(szPattern);

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
	};

	const HMODULE hModule = GetModuleHandle(szModuleName);
	if (hModule == nullptr)
	{
		throw std::runtime_error(fmt::format(XorStr("failed to get handle for: {}"), szModuleName));
		return 0U;
	}

	auto pDosHeader = (PIMAGE_DOS_HEADER)hModule;
	auto pNtHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)hModule + pDosHeader->e_lfanew);

	DWORD dwSizeOfImage = pNtHeaders->OptionalHeader.SizeOfImage;
	std::vector<int> vecBytes = PatternByte(szPattern);
	std::uint8_t* puBytes = (std::uint8_t*)(hModule);

	std::size_t uSize = vecBytes.size();
	int* pBytes = vecBytes.data();

	// check for bytes sequence match
	for (unsigned long i = 0UL; i < dwSizeOfImage - uSize; ++i)
	{
		bool bByteFound = true;

		for (unsigned long j = 0UL; j < uSize; ++j)
		{
			// check if doenst match or byte isnt a wildcard
			if (puBytes[i + j] != pBytes[j] && pBytes[j] != -1)
			{
				bByteFound = false;
				break;
			}
		}

		// return valid address
		if (bByteFound)
			return (std::uintptr_t)(&puBytes[i]);
	}

	#if DEBUG_CONSOLE && _DEBUG
	L::PushConsoleColor(FOREGROUND_RED);
	L::Print(fmt::format(XorStr("[error] failed get pattern: [{}] [{}]"), szModuleName, szPattern));
	L::PopConsoleColor();
	#endif

	return 0U;
}

bool MEM::IsValidCodePtr(std::uintptr_t uAddress)
{
	if (uAddress == 0U)
		return false;

	MEMORY_BASIC_INFORMATION memInfo = { };

	if (VirtualQuery((LPCVOID)uAddress, &memInfo, sizeof(memInfo)) == 0U)
		return false;

	if (!(memInfo.Protect & PAGE_EXECUTE_READWRITE || memInfo.Protect & PAGE_EXECUTE_READ))
		return false;

	return true;
}
