#pragma once
// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/tier1/bitbuf.h

class bf_write
{
public:
	bf_write() = default;

	// the current buffer
	unsigned long* pData;
	int				nDataBytes;
	int				nDataBits;

	// where we are in the buffer
	int				iCurrentBit;
};

class bf_read
{
public:
	std::uintptr_t uBaseAddress;
	std::uintptr_t uCurrentOffset;

	bf_read(std::uintptr_t uAddress) : uBaseAddress(uAddress), uCurrentOffset(0U) {}

	void SetOffset(std::uintptr_t uOffset)
	{
		uCurrentOffset = uOffset;
	}

	void Skip(std::uintptr_t uLength)
	{
		uCurrentOffset += uLength;
	}

	int ReadByte()
	{
		char dValue = *(char*)(uBaseAddress + uCurrentOffset);
		++uCurrentOffset;
		return dValue;
	}

	bool ReadBool()
	{
		bool bValue = *(bool*)(uBaseAddress + uCurrentOffset);
		++uCurrentOffset;
		return bValue;
	}

	const char* ReadString()
	{
		char szBuffer[256];
		char chLength = *(char*)(uBaseAddress + uCurrentOffset);
		++uCurrentOffset;
		memcpy(szBuffer, (void*)(uBaseAddress + uCurrentOffset), chLength > 255 ? 255 : chLength);
		szBuffer[chLength > 255 ? 255 : chLength] = '\0';
		uCurrentOffset += chLength + 1;
		return szBuffer;
	}
};
