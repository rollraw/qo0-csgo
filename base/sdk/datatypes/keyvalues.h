#pragma once
// used: winapi includes, call vfunc
#include "../../common.h"

using KeyValuesSystemFn = void*(__cdecl*)();
using GetSymbolProcFn = bool(__cdecl*)(const char*);

class CKeyValues
{
public:
	enum EKeyType : int
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
			KeyValuesSystemFn oKeyValuesSystem = reinterpret_cast<KeyValuesSystemFn>(GetProcAddress(GetModuleHandle(VSTDLIB_DLL), XorStr("KeyValuesSystem")));

			if (oKeyValuesSystem != nullptr)
				pKeyValuesSystem = oKeyValuesSystem();
		}

		return MEM::CallVFunc<void*>(pKeyValuesSystem, 1, nAllocSize);
	}

	void operator delete(void* pMemory)
	{
		static void* pKeyValuesSystem = nullptr;
		if (pKeyValuesSystem == nullptr)
		{
			KeyValuesSystemFn oKeyValuesSystem = reinterpret_cast<KeyValuesSystemFn>(GetProcAddress(GetModuleHandle(VSTDLIB_DLL), XorStr("KeyValuesSystem")));

			if (oKeyValuesSystem != nullptr)
				pKeyValuesSystem = oKeyValuesSystem();
		}

		MEM::CallVFunc<void>(pKeyValuesSystem, 2, pMemory);
	}

	const char* GetName()
	{
		static void* pKeyValuesSystem = nullptr;
		if (pKeyValuesSystem == nullptr)
		{
			KeyValuesSystemFn oKeyValuesSystem = reinterpret_cast<KeyValuesSystemFn>(GetProcAddress(GetModuleHandle(VSTDLIB_DLL), XorStr("KeyValuesSystem")));

			if (oKeyValuesSystem != nullptr)
				pKeyValuesSystem = oKeyValuesSystem();
		}

		return MEM::CallVFunc<const char*>(pKeyValuesSystem, 4, *reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uintptr_t>(this) + 0x3) | (*reinterpret_cast<std::uint16_t*>(reinterpret_cast<std::uintptr_t>(this) + 0x12) << 8));
	}

	void Init(const char* szKeyName)
	{
		using InitKeyValuesFn = void(__thiscall*)(void*, const char*);
		static auto oInitKeyValues = reinterpret_cast<InitKeyValuesFn>(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 51 33 C0 C7 45"))); // @xref: "OldParticleSystem_Destroy"

		if (oInitKeyValues == nullptr)
			return;

		oInitKeyValues(this, szKeyName);
	}

	void LoadFromBuffer(char const* szResourceName, const char* szBuffer, void* pFileSystem = nullptr, const char* szPathID = nullptr, GetSymbolProcFn pfnEvaluateSymbolProc = nullptr)
	{
		using LoadFromBufferFn = void(__thiscall*)(void*, const char*, const char*, void*, const char*, void*, void*);
		static auto oLoadFromBuffer = reinterpret_cast<LoadFromBufferFn>(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89"))); // @xref: "KeyValues::LoadFromBuffer(%s%s%s): Begin"

		if (oLoadFromBuffer == nullptr)
			return;

		oLoadFromBuffer(this, szResourceName, szBuffer, pFileSystem, szPathID, pfnEvaluateSymbolProc, nullptr);
	}

	static CKeyValues* FromString(const char* szName, const char* szValue)
	{
		static auto oFromString = MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 81 EC ? ? ? ? 85 D2 53")); // @xref: "#empty#", "#int#"
		CKeyValues* pKeyValues = nullptr;

		__asm
		{
			push 0
			mov edx, szValue
			mov ecx, szName
			call oFromString
			add esp, 4
			mov pKeyValues, eax
		}

		return pKeyValues;
	}

	bool LoadFromFile(void* pFileSystem, const char* szResourceName, const char* szPathID = nullptr, GetSymbolProcFn pfnEvaluateSymbolProc = nullptr)
	{
		using LoadFromFileFn = bool(__thiscall*)(void*, void*, const char*, const char*, void*);
		static auto oLoadFromFile = reinterpret_cast<LoadFromFileFn>(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 E4 F8 83 EC 14 53 56 8B 75 08 57 FF"))); // @xref: "rb"
		return oLoadFromFile(this, pFileSystem, szResourceName, szPathID, pfnEvaluateSymbolProc);
	}

	CKeyValues* FindKey(const char* szKeyName, bool bCreate)
	{
		using FindKeyFn = CKeyValues*(__thiscall*)(void*, const char*, bool);
		static auto oFindKey = reinterpret_cast<FindKeyFn>(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 EC 1C 53 8B D9 85 DB")));
		return oFindKey(this, szKeyName, bCreate);
	}

	int GetInt(const char* szKeyName, const int iDefaultValue)
	{
		if (CKeyValues* pSubKey = FindKey(szKeyName, false); pSubKey != nullptr)
		{
			switch (pSubKey->chType)
			{
			case TYPE_STRING:
				return std::atoi(pSubKey->szValue);
			case TYPE_WSTRING:
				return _wtoi(pSubKey->wszValue);
			case TYPE_FLOAT:
				return static_cast<int>(pSubKey->flValue);
			case TYPE_UINT64:
				// can't convert, since it would lose data
				assert(0);
				return 0;
			case TYPE_INT:
			case TYPE_PTR:
			default:
				return pSubKey->iValue;
			}
		}

		return iDefaultValue;
	}

	float GetFloat(const char* szKeyName, const float flDefaultValue)
	{
		if (CKeyValues* pSubKey = FindKey(szKeyName, false); pSubKey != nullptr)
		{
			switch (pSubKey->chType)
			{
			case TYPE_STRING:
				return static_cast<float>(std::atof(pSubKey->szValue));
			case TYPE_WSTRING:
				return std::wcstof(pSubKey->wszValue, nullptr);
			case TYPE_FLOAT:
				return pSubKey->flValue;
			case TYPE_INT:
				return static_cast<float>(pSubKey->iValue);
			case TYPE_UINT64:
				return static_cast<float>(*reinterpret_cast<std::uint64_t*>(pSubKey->szValue));
			case TYPE_PTR:
			default:
				return 0.0f;
			}
		}

		return flDefaultValue;
	}

	const char* GetString(const char* szKeyName, const char* szDefaultValue)
	{
		using GetStringFn = const char* (__thiscall*)(void*, const char*, const char*);
		static auto oGetString = reinterpret_cast<GetStringFn>(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 E4 C0 81 EC ? ? ? ? 53 8B 5D 08")));
		return oGetString(this, szKeyName, szDefaultValue);
	}

	void SetString(const char* szKeyName, const char* szStringValue)
	{
		CKeyValues* pSubKey = FindKey(szKeyName, true);

		if (pSubKey == nullptr)
			return;

		using SetStringFn = void(__thiscall*)(void*, const char*);
		static auto oSetString = reinterpret_cast<SetStringFn>(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC A1 ? ? ? ? 53 56 57 8B F9 8B 08 8B 01")));
		oSetString(pSubKey, szStringValue);
	}

	void SetInt(const char* szKeyName, const int iValue)
	{
		CKeyValues* pSubKey = FindKey(szKeyName, true);

		if (pSubKey == nullptr)
			return;

		pSubKey->iValue = iValue;
		pSubKey->chType = TYPE_INT;
	}

	void SetUint64(const char* szKeyName, const int nLowValue, const int nHighValue)
	{
		CKeyValues* pSubKey = FindKey(szKeyName, true);

		if (pSubKey == nullptr)
			return;

		// delete the old value
		delete[] pSubKey->szValue;

		// make sure we're not storing the WSTRING - as we're converting over to STRING
		delete[] pSubKey->wszValue;
		pSubKey->wszValue = nullptr;

		pSubKey->szValue = new char[sizeof(std::uint64_t)];
		*reinterpret_cast<std::uint64_t*>(pSubKey->szValue) = static_cast<std::uint64_t>(nHighValue) << 32ULL | nLowValue;
		pSubKey->chType = TYPE_UINT64;
	}

	inline void SetBool(const char* szKeyName, const bool bValue)
	{
		SetInt(szKeyName, bValue ? 1 : 0);
	}

private:
	std::byte		pad0[0x4];	// 0x00
	char*			szValue;	// 0x04
	wchar_t*		wszValue;	// 0x08

	union
	{
		int iValue;
		float flValue;
		void* pValue;
		unsigned char arrColor[4];
	}; // 0x0C

	char			chType;		// 0x10
	bool			bHasEscapeSequences; // 0x11
	std::uint16_t	uKeyNameCaseSensitive; // 0x12
	CKeyValues*		pPeer; // 0x14
	CKeyValues*		pSub;	// 0x18
	CKeyValues*		pChain;// 0x1C
	GetSymbolProcFn	pExpressionGetSymbolProc; // 0x20
}; // Size: 0x24
