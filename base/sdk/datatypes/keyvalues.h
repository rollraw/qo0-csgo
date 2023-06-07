#pragma once
#include "color.h"

// used: stringlength, memorycopy
#include "../../utilities/crt.h"

// @test: using interfaces in the header | not critical but could blow up someday with thousands of errors or affect to compilation time etc
// used: interface handles
#include "../../core/interfaces.h"
// used: interface declarations
#include "../interfaces/imemalloc.h"
#include "../interfaces/ikeyvaluessystem.h"
#include "../interfaces/ifilesystem.h"

// @source: master/public/tier1/keyvalues.h
// master/tier1/keyvalues.cpp

using GetSymbolProcFn_t = bool(Q_CDECL*)(const char*);

// simple tree-node container with serializer/deserializer
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
		TYPE_COUNT
	};

	CKeyValues(const char* szKeyName, IKeyValuesSystem* pKeyValuesSystem = I::KeyValuesSystem, bool bIsSystemOwner = false)
	{
		static auto fnConstructor = reinterpret_cast<CKeyValues*(Q_THISCALL*)(CKeyValues*, const char*, IKeyValuesSystem*, bool)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 56 8B F1 33 C0 8B 4D 0C 81")));
		fnConstructor(this, szKeyName, pKeyValuesSystem, bIsSystemOwner);
	}

	~CKeyValues()
	{
		// @ida KeyValues::~KeyValues(): client.dll -> "56 8B F1 E8 ? ? ? ? 8B 4E 14"
		// @ida KeyValues::deleteThis(): client.dll -> "56 8B F1 85 F6 74 2D"

		RemoveEverything();

		if (pKeySystem != nullptr && bIsSystemOwner)
		{
			pKeySystem->~IKeyValuesSystem();
			pKeySystem = nullptr;
		}
	}

	void* operator new(const std::size_t nSize)
	{
		return I::KeyValuesSystem->AllocKeyValuesMemory(static_cast<int>(nSize));
	}

	void operator delete(void* pMemory)
	{
		I::KeyValuesSystem->FreeKeyValuesMemory(pMemory);
	}

	[[nodiscard]] const char* GetName() const
	{
		// @ida KeyValues::GetName(): client.dll -> ABS["E8 ? ? ? ? 8B 4B 78" + 0x1]

		IKeyValuesSystem* pActiveSystem = pKeySystem;
		if (pActiveSystem == nullptr)
			pActiveSystem = I::KeyValuesSystem;

		return pActiveSystem->GetStringForSymbol(static_cast<HKeySymbol>(uKeyNameCaseSensitive1 | (uKeyNameCaseSensitive2 << 8)));
	}

	void SetName(const char* szKeyName, HKeySymbol hCaseInsensitiveKeyName = INVALID_KEY_SYMBOL)
	{
		// @ida KeyValues::SetName(): client.dll -> ABS["E8 ? ? ? ? 8B 45 F8 83 F8 07" + 0x1]

		IKeyValuesSystem* pActiveSystem = pKeySystem;
		if (pActiveSystem == nullptr)
			pActiveSystem = I::KeyValuesSystem;

		const HKeySymbol hCaseSensitiveKeyName = pActiveSystem->GetSymbolForStringCaseSensitive(hCaseInsensitiveKeyName, szKeyName);

		uKeyName = hCaseInsensitiveKeyName;
		uKeyNameCaseSensitive1 = hCaseSensitiveKeyName;
		uKeyNameCaseSensitive2 = static_cast<std::uint16_t>(hCaseSensitiveKeyName >> 8);
	}

	[[nodiscard]] static CKeyValues* FromString(const char* szName, const char* szBuffer, const char** pszEndOfParse = nullptr)
	{
		static auto fnFromString = MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 83 E4 F8 81 EC ? ? ? ? 85")); // @xref: " system {  network LIVE  }  options {  action joinsession  } "

		CKeyValues* pKeyValues = nullptr;
		__asm
		{
			push pszEndOfParse
			mov edx, szBuffer
			mov ecx, szName
			call fnFromString
			mov pKeyValues, eax
			add esp, 4
		}

		return pKeyValues;
	}

	[[nodiscard]] bool LoadFromBuffer(const char* szResourceName, const char* szBuffer, IBaseFileSystem* pBaseFileSystem = nullptr, const char* szPathID = nullptr, GetSymbolProcFn_t pfnEvaluateSymbolProc = nullptr)
	{
		static auto fnLoadFromBuffer = reinterpret_cast<bool(Q_THISCALL*)(CKeyValues*, const char*, const char*, void*, const char*, void*, void*)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89"))); // @xref: "KeyValues::LoadFromBuffer(%s%s%s): Begin"

#ifdef Q_COMPILER_CLANG
		std::uintptr_t uLoadFromBuffer = reinterpret_cast<std::uintptr_t>(fnLoadFromBuffer);
		bool bReturn;

		// @todo: still crashes sometimes, probably still messes with some reg
		__asm
		{
			push 0
			mov edx, pfnEvaluateSymbolProc
			push edx
			mov eax, szPathID
			push eax
			mov ecx, pBaseFileSystem
			push ecx
			mov edx, szBuffer
			push edx
			mov eax, szResourceName
			push eax
			mov ecx, this
			call uLoadFromBuffer
			mov bReturn, al
		}

		return bReturn;
#else
		return fnLoadFromBuffer(this, szResourceName, szBuffer, pBaseFileSystem, szPathID, reinterpret_cast<void*>(pfnEvaluateSymbolProc), nullptr);
#endif
	}

	#pragma region keyvalues_file_access
	[[nodiscard]] bool LoadFromFile(IBaseFileSystem* pBaseFileSystem, const char* szResourceName, const char* szPathID = nullptr, GetSymbolProcFn_t pfnEvaluateSymbolProc = nullptr)
	{
		// @ida KeyValues::LoadFromFile: client.dll -> "55 8B EC 83 E4 F8 83 EC 14 53 56 8B 75 08 57 FF" @xref: "rb"

		const FileHandle_t hFile = pBaseFileSystem->Open(szResourceName, Q_XOR("rb"), szPathID);

		if (hFile == nullptr)
			return false;

		// @todo: if 'IFileSystem' would have a vtable, static_cast should work just fine
		IFileSystem* pFullFileSystem = static_cast<IFileSystem*>(static_cast<void*>(&pBaseFileSystem[-1]));

		// load file into a null-terminated buffer
		const unsigned int nFileSize = pBaseFileSystem->Size(hFile);
		const unsigned int nBufferSize = pFullFileSystem->GetOptimalReadSize(hFile, nFileSize + 2U);

		char* szBuffer = static_cast<char*>(pFullFileSystem->AllocOptimalReadBuffer(hFile, nBufferSize));
		Q_ASSERT(szBuffer != nullptr);

		// read into local buffer
		bool bReturn = (pFullFileSystem->ReadEx(szBuffer, nBufferSize, nFileSize, hFile) != 0);
		pBaseFileSystem->Close(hFile);

		if (bReturn)
		{
			// null terminate file as EOF (double NULL in case this is a unicode file)
			*reinterpret_cast<std::uint16_t*>(&szBuffer[nFileSize]) = 0;
			bReturn = LoadFromBuffer(szResourceName, szBuffer, pBaseFileSystem, szPathID, pfnEvaluateSymbolProc);
		}

		pFullFileSystem->FreeOptimalReadBuffer(szBuffer);
		return bReturn;
	}
	#pragma endregion

	#pragma region keyvalues_key_access
	[[nodiscard]] Q_INLINE CKeyValues* CreateKey(const char* szKeyName)
	{
		CKeyValues* pLastChild = FindLastSubKey();
		return CreateKeyUsingKnownLastChild(szKeyName, pLastChild);
	}

	[[nodiscard]] Q_INLINE CKeyValues* CreateKeyUsingKnownLastChild(const char* szKeyName, CKeyValues* pLastChild)
	{
		CKeyValues* pNewKey = new CKeyValues(szKeyName);

		pNewKey->bHasEscapeSequences = bHasEscapeSequences;

		Q_ASSERT(pNewKey != nullptr);
		Q_ASSERT(pNewKey->pPeer == nullptr); // make sure the subkey isn't a child of some other keyvalues

		// add subkey using last known child
		if (pLastChild == nullptr)
		{
			Q_ASSERT(pSub == nullptr);
			pSub = pNewKey;
		}
		else
		{
			Q_ASSERT(pSub != nullptr && pLastChild->pPeer == nullptr);
			pLastChild->pPeer = pNewKey;
		}

		return pNewKey;
	}

	[[nodiscard]] Q_INLINE CKeyValues* FindLastSubKey() const
	{
		if (pSub == nullptr)
			return nullptr;

		CKeyValues* pLastChild = pSub;

		while (pLastChild->pPeer != nullptr)
			pLastChild = pLastChild->pPeer;

		return pLastChild;
	}

	[[nodiscard]] CKeyValues* FindKey(const char* szKeyName, const bool bCreate)
	{
		static auto fnFindKey = reinterpret_cast<CKeyValues*(Q_THISCALL*)(CKeyValues*, const char*, bool)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 83 EC 1C 53 8B D9 85 DB")));
		return fnFindKey(this, szKeyName, bCreate);
	}
	#pragma endregion

	#pragma region keyvalues_key_read
	[[nodiscard]] const char* GetString(const char* szKeyName, const char* szDefaultValue)
	{
		static auto fnGetString = reinterpret_cast<const char*(Q_THISCALL*)(CKeyValues*, const char*, const char*)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 83 E4 C0 81 EC ? ? ? ? 53 8B 5D 08")));
		return fnGetString(this, szKeyName, szDefaultValue);
	}

	[[nodiscard]] const wchar_t* GetWString(const char* szKeyName, const wchar_t* wszDefaultValue)
	{
		static auto fnGetWString = reinterpret_cast<const wchar_t*(Q_THISCALL*)(CKeyValues*, const char*, const wchar_t*)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 83 E4 C0 81 EC ? ? ? ? 53 56 57 8B 7D 08 8B")));
		return fnGetWString(this, szKeyName, wszDefaultValue);
	}

	[[nodiscard]] void* GetPtr(const char* szKeyName, void* pDefaultValue)
	{
		// @ida KeyValues::GetPtr(): client.dll -> ABS["E8 ? ? ? ? E9 ? ? ? ? 0F 10 4C 33" + 0x1]

		const CKeyValues* pSubKey = FindKey(szKeyName, false);

		if (pSubKey == nullptr)
			return pDefaultValue;

		if (pSubKey->iDataType == TYPE_PTR)
			return pSubKey->pValue;

		return nullptr;
	}

	[[nodiscard]] int GetInt(const char* szKeyName, int iDefaultValue)
	{
		static auto fnGetInt = reinterpret_cast<int(Q_THISCALL*)(CKeyValues*, const char*, int)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 6A 00 FF 75 08 E8 ? ? ? ? 85 C0 74 42")));
		return fnGetInt(this, szKeyName, iDefaultValue);
	}

	[[nodiscard]] std::uint64_t GetUint64(const char* szKeyName, std::uint64_t ullDefaultValue)
	{
		static auto fnGetUint64 = reinterpret_cast<std::uint64_t(Q_THISCALL*)(CKeyValues*, const char*, std::uint64_t)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 83 E4 F8 83 EC 08 6A")));
		return fnGetUint64(this, szKeyName, ullDefaultValue);
	}

	[[nodiscard]] float GetFloat(const char* szKeyName, float flDefaultValue)
	{
		static auto fnGetFloat = MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 51 6A 00 FF 75 08"));

		float flReturn;
		__asm
		{
			mov ecx, this
			movss xmm2, flDefaultValue
			push szKeyName
			call fnGetFloat
			movss flReturn, xmm0
		}
		return flReturn;
	}

	[[nodiscard]] Color_t GetColor(const char* szKeyName, const Color_t& colDefaultValue)
	{
		static auto fnGetColor = reinterpret_cast<Color_t(Q_THISCALL*)(CKeyValues*, const char*, const Color_t&)>(MEM::FindPattern(CLIENT_DLL, Q_XOR("55 8B EC 8B 45 10 83 EC 08 8B 00")));
		return fnGetColor(this, szKeyName, colDefaultValue);
	}
	#pragma endregion

	#pragma region keyvalues_key_write
	void SetString(const char* szKeyName, const char* szNewValue)
	{
		// @ida KeyValues::SetString(): client.dll -> "55 8B EC 6A 01 FF 75 08 E8 ? ? ? ? 85 C0 74 0A FF"
		// @ida KeyValues::SetStringValue(): client.dll -> "55 8B EC A1 ? ? ? ? 53 56 57 8B F9 8B 08 8B 01"

		CKeyValues* pSubKey = FindKey(szKeyName, true);

		if (pSubKey == nullptr)
			return;

		// delete the old value
		I::MemAlloc->Free(pSubKey->szValue);
		// make sure we're not storing the WSTRING - as we're converting over to STRING
		I::MemAlloc->Free(pSubKey->wszValue);
		pSubKey->wszValue = nullptr;

		// ensure a valid value
		if (szNewValue == nullptr)
			szNewValue = "";

		// allocate memory for the new value and copy it in
		const std::size_t nLength = CRT::StringLength(szNewValue) + 1U;
		pSubKey->szValue = static_cast<char*>(I::MemAlloc->Alloc(nLength * sizeof(char)));
		CRT::MemoryCopy(pSubKey->szValue, szNewValue, nLength * sizeof(char));

		pSubKey->iDataType = TYPE_STRING;
	}

	void SetWString(const char* szKeyName, const wchar_t* wszNewValue)
	{
		// @ida KeyValues::SetWString(): client.dll -> "55 8B EC 53 6A 01"

		CKeyValues* pSubKey = FindKey(szKeyName, true);

		if (pSubKey == nullptr)
			return;

		// delete the old value
		I::MemAlloc->Free(pSubKey->wszValue);
		// make sure we're not storing the STRING - as we're converting over to WSTRING
		I::MemAlloc->Free(pSubKey->szValue);
		pSubKey->szValue = nullptr;

		// ensure a valid value
		if (wszNewValue == nullptr)
			wszNewValue = L"";

		// allocate memory for the new value and copy it in
		const std::size_t nLength = CRT::StringLength(wszNewValue) + 1U;
		pSubKey->wszValue = static_cast<wchar_t*>(I::MemAlloc->Alloc(nLength * sizeof(wchar_t)));
		CRT::MemoryCopy(pSubKey->wszValue, wszNewValue, nLength * sizeof(wchar_t));

		pSubKey->iDataType = TYPE_WSTRING;
	}

	void SetPtr(const char* szKeyName, void* pNewValue)
	{
		// @ida KeyValues::SetPtr(): client.dll -> ABS["E8 ? ? ? ? FF 15 ? ? ? ? 33 C9" + 0x1]

		CKeyValues* pSubKey = FindKey(szKeyName, true);

		if (pSubKey == nullptr)
			return;

		pSubKey->pValue = pNewValue;
		pSubKey->iDataType = TYPE_PTR;
	}

	void SetInt(const char* szKeyName, const int iNewValue)
	{
		// @ida KeyValues::SetInt(): client.dll -> ABS["E8 ? ? ? ? FF 74 24 28 8B CE" + 0x1]

		CKeyValues* pSubKey = FindKey(szKeyName, true);

		if (pSubKey == nullptr)
			return;

		pSubKey->iValue = iNewValue;
		pSubKey->iDataType = TYPE_INT;
	}

	void SetUint64(const char* szKeyName, const std::uint64_t ullNewValue)
	{
		// @ida KeyValues::SetUint64(): client.dll -> "55 8B EC 56 6A 01 FF"

		CKeyValues* pSubKey = FindKey(szKeyName, true);

		if (pSubKey == nullptr)
			return;

		// delete the old value
		I::MemAlloc->Free(pSubKey->szValue);
		// make sure we're not storing the WSTRING - as we're converting over to STRING
		I::MemAlloc->Free(pSubKey->wszValue);
		pSubKey->wszValue = nullptr;

		pSubKey->szValue = static_cast<char*>(I::MemAlloc->Alloc(sizeof(std::uint64_t)));
		*reinterpret_cast<std::uint64_t*>(pSubKey->szValue) = ullNewValue;
		pSubKey->iDataType = TYPE_UINT64;
	}

	void SetFloat(const char* szKeyName, const float flNewValue)
	{
		// @ida KeyValues::SetFloat(): client.dll -> "55 8B EC 53 6A 01"

		CKeyValues* pSubKey = FindKey(szKeyName, true);

		if (pSubKey == nullptr)
			return;

		pSubKey->flValue = flNewValue;
		pSubKey->iDataType = TYPE_FLOAT;
	}

	void SetColor(const char* szKeyName, const Color_t& colNewValue)
	{
		// @ida KeyValues::SetColor(): client.dll -> ABS["E8 ? ? ? ? 8B 4D 0C 53" + 0x1]

		CKeyValues* pSubKey = FindKey(szKeyName, true);

		if (pSubKey == nullptr)
			return;

		pSubKey->colValue = colNewValue;
		pSubKey->iDataType = TYPE_COLOR;
	}

	Q_INLINE void SetBool(const char* szKeyName, const bool bValue)
	{
		SetInt(szKeyName, bValue);
	}
	#pragma endregion

	void RemoveEverything()
	{
		// @ida KeyValues::RemoveEverything(): client.dll -> "56 57 8B F9 8B 4F 20 85"

		CKeyValues* pCurrent;
		CKeyValues* pNext;

		for (pCurrent = pSub; pCurrent != nullptr; pCurrent = pNext)
		{
			pNext = pCurrent->pPeer;
			pCurrent->pPeer = nullptr;
			delete pCurrent;
		}

		for (pCurrent = pPeer; pCurrent != nullptr && pCurrent != this; pCurrent = pNext)
		{
			pNext = pCurrent->pPeer;
			pCurrent->pPeer = nullptr;
			delete pCurrent;
		}

		I::MemAlloc->Free(szValue);
		szValue = nullptr;
		I::MemAlloc->Free(wszValue);
		wszValue = nullptr;
	}

private:
	std::uint32_t uKeyName : 24; // 0x00
	std::uint32_t uKeyNameCaseSensitive1 : 8; // 0x3 // byte, explicitly specify bits due to packing
	char* szValue; // 0x04
	wchar_t* wszValue; // 0x08

	union
	{
		int iValue;
		float flValue;
		void* pValue;
		Color_t colValue;
	}; // 0x0C

	std::int8_t iDataType; // 0x10
	bool bHasEscapeSequences; // 0x11
	std::uint16_t uKeyNameCaseSensitive2; // 0x12
	IKeyValuesSystem* pKeySystem; // 0x14 // why do valve store global ptr for each instance? | fuck lol i just released that them doing this to prevent allockeyvalues from hooking | even now when they moved retadr info to protos they didnt revert this... makes me think that valve just missed previous devs and the current ones are just doing this pointless shit
	bool bIsSystemOwner; // 0x18 // if true, destructor will free current system
	CKeyValues* pPeer; // 0x1C
	CKeyValues* pSub; // 0x20
	CKeyValues* pChain; // 0x24
	GetSymbolProcFn_t pExpressionGetSymbolProc; // 0x28
};
static_assert(sizeof(CKeyValues) == 0x2C);
