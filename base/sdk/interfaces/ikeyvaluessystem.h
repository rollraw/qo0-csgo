#pragma once
// used: callvfunc
#include "../../utilities/memory.h"

#define INVALID_KEY_SYMBOL (-1)
using HKeySymbol = int;

// forward declarations
class IKeyValuesSystem;

using KeyValuesSystemFn_t = IKeyValuesSystem*(Q_CDECL*)();

class IKeyValuesSystem : ROP::VirtualCallable_t<ROP::ClientGadget_t>
{
public:
	virtual ~IKeyValuesSystem() { }

	void RegisterSizeofKeyValues(int iSize)
	{
		CallVFunc<void, 1U>(this, iSize);
	}

	void* AllocKeyValuesMemory(int iSize)
	{
		return CallVFunc<void*, 2U>(this, iSize);
	}

	void FreeKeyValuesMemory(void* pMemory)
	{
		CallVFunc<void, 3U>(this, pMemory);
	}

	HKeySymbol GetSymbolForString(const char* szName, bool bCreate = true)
	{
		return CallVFunc<HKeySymbol, 4U>(this, szName, bCreate);
	}

	const char* GetStringForSymbol(HKeySymbol hSymbol)
	{
		return CallVFunc<const char*, 5U>(this, hSymbol);
	}

	void AddKeyValuesToMemoryLeakList(void* pMemory, HKeySymbol hSymbolName)
	{
		CallVFunc<void, 6U>(this, pMemory, hSymbolName);
	}

	void RemoveKeyValuesFromMemoryLeakList(void* pMemory)
	{
		CallVFunc<void, 7U>(this, pMemory);
	}

	void SetKeyValuesExpressionSymbol(const char* szName, bool bValue)
	{
		CallVFunc<void, 8U>(this, szName, bValue);
	}

	bool GetKeyValuesExpressionSymbol(const char* szName)
	{
		return CallVFunc<bool, 9U>(this, szName);
	}

	HKeySymbol GetSymbolForStringCaseSensitive(HKeySymbol& hCaseInsensitiveSymbol, const char* szName, bool bCreate = true)
	{
		return CallVFunc<HKeySymbol, 10U>(this, &hCaseInsensitiveSymbol, szName, bCreate);
	}
};
