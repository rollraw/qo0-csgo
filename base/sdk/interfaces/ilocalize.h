#pragma once

class ILocalize : ROP::VirtualCallable_t<ROP::ClientGadget_t>
{
public:
	wchar_t* Find(const char* szTokenName)
	{
		return CallVFunc<wchar_t*, 11U>(this, szTokenName);
	}

	const wchar_t* FindSafe(const char* szTokenName)
	{
		return CallVFunc<wchar_t*, 12U>(this, szTokenName);
	}

	int ConvertAnsiToUnicode(const char* szAnsi, wchar_t* wszUnicode, int nUnicodeBufferSizeInBytes)
	{
		return CallVFunc<int, 15U>(this, szAnsi, wszUnicode, nUnicodeBufferSizeInBytes);
	}

	int ConvertUnicodeToAnsi(const wchar_t* wszUnicode, char* szAnsi, int nAnsiBufferSize)
	{
		return CallVFunc<int, 16U>(this, wszUnicode, szAnsi, nAnsiBufferSize);
	}

	const char* FindAsUTF8(const char* szTokenName)
	{
		return CallVFunc<const char*, 47U>(this, szTokenName);
	}
};
