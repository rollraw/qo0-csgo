#include "keyvalues.h"

// used: findpattern, callvfunc, getmodulebasehandle
#include "../../core/interfaces.h"

CKeyValues::CKeyValues(const char* szKeyName)
{
	using InitKeyValuesFn = void(__thiscall*)(void*, const char*);
	static auto oInitKeyValues = reinterpret_cast<InitKeyValuesFn>(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 51 33 C0 C7 45"))); // @xref: "OldParticleSystem_Destroy"
	assert(oInitKeyValues != nullptr);

	oInitKeyValues(this, szKeyName);
}

void* CKeyValues::operator new(std::size_t nAllocSize)
{
	return I::KeyValuesSystem->AllocKeyValuesMemory(nAllocSize);
}

void CKeyValues::operator delete(void* pMemory)
{
	I::KeyValuesSystem->FreeKeyValuesMemory(pMemory);
}

const char* CKeyValues::GetName()
{
	return I::KeyValuesSystem->GetStringForSymbol(this->uKeyNameCaseSensitive1 | (this->uKeyNameCaseSensitive2 << 8));
}

CKeyValues* CKeyValues::FromString(const char* szName, const char* szValue)
{
	static auto oFromString = MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 81 EC ? ? ? ? 85 D2 53")); // @xref: "#empty#", "#int#"
	CKeyValues* pKeyValues = nullptr;

	if (oFromString == 0U)
		return nullptr;

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

void CKeyValues::LoadFromBuffer(char const* szResourceName, const char* szBuffer, void* pFileSystem, const char* szPathID, GetSymbolProcFn pfnEvaluateSymbolProc)
{
	using LoadFromBufferFn = void(__thiscall*)(void*, const char*, const char*, void*, const char*, void*, void*);
	static auto oLoadFromBuffer = reinterpret_cast<LoadFromBufferFn>(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89"))); // @xref: "KeyValues::LoadFromBuffer(%s%s%s): Begin"
	assert(oLoadFromBuffer != nullptr);

	oLoadFromBuffer(this, szResourceName, szBuffer, pFileSystem, szPathID, pfnEvaluateSymbolProc, nullptr);
}

bool CKeyValues::LoadFromFile(void* pFileSystem, const char* szResourceName, const char* szPathID, GetSymbolProcFn pfnEvaluateSymbolProc)
{
	using LoadFromFileFn = bool(__thiscall*)(void*, void*, const char*, const char*, void*);
	static auto oLoadFromFile = reinterpret_cast<LoadFromFileFn>(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 E4 F8 83 EC 14 53 56 8B 75 08 57 FF"))); // @xref: "rb"
	assert(oLoadFromFile != nullptr);

	return oLoadFromFile(this, pFileSystem, szResourceName, szPathID, pfnEvaluateSymbolProc);
}

CKeyValues* CKeyValues::FindKey(const char* szKeyName, const bool bCreate)
{
	using FindKeyFn = CKeyValues * (__thiscall*)(void*, const char*, bool);
	static auto oFindKey = reinterpret_cast<FindKeyFn>(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 EC 1C 53 8B D9 85 DB")));
	assert(oFindKey != nullptr);

	return oFindKey(this, szKeyName, bCreate);
}

int CKeyValues::GetInt(const char* szKeyName, const int iDefaultValue)
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

float CKeyValues::GetFloat(const char* szKeyName, const float flDefaultValue)
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

const char* CKeyValues::GetString(const char* szKeyName, const char* szDefaultValue)
{
	using GetStringFn = const char* (__thiscall*)(void*, const char*, const char*);
	static auto oGetString = reinterpret_cast<GetStringFn>(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC 83 E4 C0 81 EC ? ? ? ? 53 8B 5D 08")));
	assert(oGetString != nullptr);

	return oGetString(this, szKeyName, szDefaultValue);
}

void CKeyValues::SetString(const char* szKeyName, const char* szStringValue)
{
	CKeyValues* pSubKey = FindKey(szKeyName, true);

	if (pSubKey == nullptr)
		return;

	using SetStringFn = void(__thiscall*)(void*, const char*);
	static auto oSetString = reinterpret_cast<SetStringFn>(MEM::FindPattern(CLIENT_DLL, XorStr("55 8B EC A1 ? ? ? ? 53 56 57 8B F9 8B 08 8B 01")));
	assert(oSetString != nullptr);

	oSetString(pSubKey, szStringValue);
}

void CKeyValues::SetInt(const char* szKeyName, const int iValue)
{
	CKeyValues* pSubKey = FindKey(szKeyName, true);

	if (pSubKey == nullptr)
		return;

	pSubKey->iValue = iValue;
	pSubKey->chType = TYPE_INT;
}

void CKeyValues::SetUint64(const char* szKeyName, const int nLowValue, const int nHighValue)
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
