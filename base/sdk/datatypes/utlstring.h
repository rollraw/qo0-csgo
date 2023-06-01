#pragma once
#include "utlmemory.h"

class CUtlBinaryBlock
{
public:
	CUtlBinaryBlock(const int nInitialGrowSize = 0, const int nInitialSize = 0) :
		memory(nInitialGrowSize, nInitialSize), nLength(0) { }

	CUtlMemory<unsigned char> memory;
	int nLength;
};

class CUtlString
{
public:
	CUtlString() { }

	[[nodiscard]] const char* Get() const
	{
		if (storage.nLength == 0)
			return "";

		return reinterpret_cast<const char*>(storage.memory.Base());
	}

	[[nodiscard]] int Length() const
	{
		return storage.nLength;
	}

private:
	CUtlBinaryBlock storage;
};

template <typename T = char>
class CUtlConstStringBase
{
public:
	CUtlConstStringBase() :
		pString(nullptr) { }

	[[nodiscard]] const T* Get() const
	{
		return (pString != nullptr ? pString : static_cast<T*>(""));
	}

	[[nodiscard]] operator const T*() const
	{
		return (pString != nullptr ? pString : static_cast<T*>(""));
	}

	[[nodiscard]] bool Empty() const
	{
		return (pString == nullptr);
	}

protected:
	const T* pString;
};

using CUtlConstString = CUtlConstStringBase<char>;
using CUtlConstWideString = CUtlConstStringBase<wchar_t>;
