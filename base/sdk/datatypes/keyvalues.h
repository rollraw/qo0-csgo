#pragma once
// used: std::size_t, std::byte
#include <cstddef>
// used: std::uint16_t
#include <cstdint>

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

	CKeyValues(const char* szKeyName);

	void* operator new(std::size_t nAllocSize);
	void operator delete(void* pMemory);

	const char* GetName();

	static CKeyValues* FromString(const char* szName, const char* szValue);
	void LoadFromBuffer(char const* szResourceName, const char* szBuffer, void* pFileSystem = nullptr, const char* szPathID = nullptr, GetSymbolProcFn pfnEvaluateSymbolProc = nullptr);
	bool LoadFromFile(void* pFileSystem, const char* szResourceName, const char* szPathID = nullptr, GetSymbolProcFn pfnEvaluateSymbolProc = nullptr);

	CKeyValues* FindKey(const char* szKeyName, const bool bCreate);

	int GetInt(const char* szKeyName, const int iDefaultValue);
	float GetFloat(const char* szKeyName, const float flDefaultValue);
	const char* GetString(const char* szKeyName, const char* szDefaultValue);

	void SetString(const char* szKeyName, const char* szStringValue);
	void SetInt(const char* szKeyName, const int iValue);
	void SetUint64(const char* szKeyName, const int nLowValue, const int nHighValue);

	inline void SetBool(const char* szKeyName, const bool bValue)
	{
		SetInt(szKeyName, bValue ? 1 : 0);
	}

private:
	std::uint32_t uKeyName : 24; // 0x00
	std::uint32_t uKeyNameCaseSensitive1 : 8; // 0x3 // byte, explicitly specify bits due to packing
	char* szValue;	// 0x04
	wchar_t* wszValue;	// 0x08

	union
	{
		int iValue;
		float flValue;
		void* pValue;
		unsigned char arrColor[4];
	}; // 0x0C

	char chType; // 0x10
	bool bHasEscapeSequences; // 0x11
	std::uint16_t uKeyNameCaseSensitive2; // 0x12
	CKeyValues* pPeer; // 0x14
	CKeyValues* pSub;	// 0x18
	CKeyValues* pChain;// 0x1C
	GetSymbolProcFn	pExpressionGetSymbolProc; // 0x20
};
static_assert(sizeof(CKeyValues) == 0x24);
