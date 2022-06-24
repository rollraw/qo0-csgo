#pragma once

#define INVALID_KEY_SYMBOL (-1)
using HKeySymbol = int;

class IKeyValuesSystem
{
public:
	virtual void RegisterSizeofKeyValues(int iSize) = 0;
private:
	virtual void function0() = 0;
public:
	virtual void* AllocKeyValuesMemory(int iSize) = 0;
	virtual void FreeKeyValuesMemory(void* pMemory) = 0;
	virtual HKeySymbol GetSymbolForString(const char* szName, bool bCreate = true) = 0;
	virtual const char* GetStringForSymbol(HKeySymbol hSymbol) = 0;
	virtual void AddKeyValuesToMemoryLeakList(void* pMemory, HKeySymbol hSymbolName) = 0;
	virtual void RemoveKeyValuesFromMemoryLeakList(void* pMemory) = 0;
	virtual void SetKeyValuesExpressionSymbol(const char* szName, bool bValue) = 0;
	virtual bool GetKeyValuesExpressionSymbol(const char* szName) = 0;
	virtual HKeySymbol GetSymbolForStringCaseSensitive(HKeySymbol& hCaseInsensitiveSymbol, const char* szName, bool bCreate = true) = 0;
};

using KeyValuesSystemFn = IKeyValuesSystem*(__cdecl*)();
