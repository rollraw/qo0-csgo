#pragma once

class IKeyValuesSystem
{
public:
	virtual void RegisterSizeofKeyValues(int iSize) = 0;
	virtual void* AllocKeyValuesMemory(int iSize) = 0;
	virtual void FreeKeyValuesMemory(void* pMemory) = 0;
	virtual int GetSymbolForString(const char* szName, bool bCreate = true) = 0;
	virtual const char* GetStringForSymbol(int hSymbol) = 0;
	virtual void AddKeyValuesToMemoryLeakList(void* pMemory, int hSymbolName) = 0;
	virtual void RemoveKeyValuesFromMemoryLeakList(void* pMemory) = 0;
	virtual void SetKeyValuesExpressionSymbol(const char* szName, bool bValue) = 0;
	virtual bool GetKeyValuesExpressionSymbol(const char* szName) = 0;
	virtual int GetSymbolForStringCaseSensitive(int& hCaseInsensitiveSymbol, const char* szName, bool bCreate = true) = 0;
};

using KeyValuesSystemFn = IKeyValuesSystem*(__cdecl*)();
