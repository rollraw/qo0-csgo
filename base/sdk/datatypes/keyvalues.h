#pragma once
// used: std::runtime_error
#include <stdexcept>

// used: winapi includes, call vfunc
#include "../../common.h"

using KeyValuesSystemFn = void*(__cdecl*)();
using GetSymbolProcFn = bool(__cdecl*)(const char*);

class CKeyValues
{
public:
	enum EKeyTypes : int
	{
		TYPE_NONE = 0,
		TYPE_STRING,
		TYPE_INT,
		TYPE_FLOAT,
		TYPE_PTR,
		TYPE_WSTRING,
		TYPE_COLOR,
		TYPE_UINT64,
		TYPE_COMPILED_INT_BYTE,
		TYPE_COMPILED_INT_0,
		TYPE_COMPILED_INT_1,
		TYPE_NUMTYPES
	};

	void* operator new(std::size_t nAllocSize)
	{
		static void* pKeyValuesSystem = nullptr;
		if (pKeyValuesSystem == nullptr)
		{
			KeyValuesSystemFn oKeyValuesSystem = (KeyValuesSystemFn)GetProcAddress(GetModuleHandle(VSTDLIB_DLL), XorStr("KeyValuesSystem"));

			if (oKeyValuesSystem == nullptr)
				throw std::runtime_error(XorStr("failed to get keyvaluessystem export"));

			pKeyValuesSystem = oKeyValuesSystem();
		}

		return MEM::CallVFunc<void*>(pKeyValuesSystem, 1, nAllocSize);
	}

	void operator delete(void* pMemory)
	{
		static void* pKeyValuesSystem = nullptr;
		if (pKeyValuesSystem == nullptr)
		{
			KeyValuesSystemFn oKeyValuesSystem = (KeyValuesSystemFn)GetProcAddress(GetModuleHandle(VSTDLIB_DLL), XorStr("KeyValuesSystem"));

			if (oKeyValuesSystem == nullptr)
				throw std::runtime_error(XorStr("failed to get keyvaluessystem export"));

			pKeyValuesSystem = oKeyValuesSystem();
		}

		MEM::CallVFunc<void>(pKeyValuesSystem, 2, pMemory);
	}

	const char* GetName()
	{
		static void* pKeyValuesSystem = nullptr;
		if (pKeyValuesSystem == nullptr)
		{
			KeyValuesSystemFn oKeyValuesSystem = (KeyValuesSystemFn)GetProcAddress(GetModuleHandle(VSTDLIB_DLL), XorStr("KeyValuesSystem"));

			if (oKeyValuesSystem == nullptr)
				throw std::runtime_error(XorStr("failed to get keyvaluessystem export"));

			pKeyValuesSystem = oKeyValuesSystem();
		}

		typedef const char* (__thiscall* oGetName)(PVOID, int);
		return MEM::CallVFunc<const char*>(pKeyValuesSystem, 4, *(std::uint8_t*)((std::uintptr_t)this + 0x3) | (*(std::uint16_t*)((std::uintptr_t)this + 0x12) << 8));
	}

	void Init(const char* szKeyName)
	{
		using InitKeyValuesFn = void(__thiscall*)(void*, const char*);
		static auto oInitKeyValues = (InitKeyValuesFn)(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 51 33 C0 C7 45"))); // @xref: "OldParticleSystem_Destroy"

		if (oInitKeyValues == nullptr)
			return;

		oInitKeyValues(this, szKeyName);
	}

	void LoadFromBuffer(char const* szResourceName, const char* szBuffer, void* szFileSystem = nullptr, const char* szPathID = nullptr, void* pfnEvaluateSymbolProc = nullptr)
	{
		using LoadFromBufferFn = void(__thiscall*)(void*, const char*, const char*, void*, const char*, void*, void*);
		static auto oLoadFromBuffer = (LoadFromBufferFn)(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89"))); // @xref: "KeyValues::LoadFromBuffer(%s%s%s): Begin"

		if (oLoadFromBuffer == nullptr)
			return;

		oLoadFromBuffer(this, szResourceName, szBuffer, szFileSystem, szPathID, pfnEvaluateSymbolProc, nullptr);
	}

	CKeyValues* FindKey(const char* szKeyName, bool bCreate)
	{
		using FindKeyFn = CKeyValues * (__thiscall*)(CKeyValues*, const char*, bool);
		static auto oFindKey = (FindKeyFn)(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 EC 1C 53 8B D9 85 DB")));
		return oFindKey(this, szKeyName, bCreate);
	}

	void SetString(const char* szKeyName, const char* szValue)
	{
		CKeyValues* pKey = FindKey(szKeyName, true);

		if (pKey == nullptr)
			return;

		using SetStringFn = void(__thiscall*)(void*, const char*);
		static auto oSetString = (SetStringFn)(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC A1 ? ? ? ? 53 56 57 8B F9 8B 08 8B 01")));
		oSetString(pKey, szValue);
	}

	void SetInt(const char* szKeyName, int iValue)
	{
		CKeyValues* pKey = FindKey(szKeyName, true);

		if (pKey == nullptr)
			return;

		*(int*)((uintptr_t)pKey + 0xC) = iValue;
		*(char*)((uintptr_t)pKey + 0x10) = 2;
	}

	inline void SetBool(const char* szKeyName, bool bValue)
	{
		SetInt(szKeyName, bValue ? 1 : 0);
	}

private:
	std::byte		pad0[0x24];
}; // Size: 0x0024
