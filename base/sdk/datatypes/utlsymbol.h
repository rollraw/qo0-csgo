#pragma once
// @source: master/public/tier1/utlsymbol.h

using FileNameHandle_t = void*;
using UtlSymbolID_t = unsigned short;

#define INVALID_UTL_SYMBOL static_cast<UtlSymbolID_t>(~0)

class CUtlSymbol
{
public:
	CUtlSymbol() :
		nIndex(INVALID_UTL_SYMBOL) { }

	CUtlSymbol(const UtlSymbolID_t nIndex) :
		nIndex(nIndex) { }

	CUtlSymbol(const CUtlSymbol& other) :
		nIndex(other.nIndex) { }

	CUtlSymbol& operator=(const CUtlSymbol& other)
	{
		nIndex = other.nIndex;
		return *this;
	}

	bool operator==(const CUtlSymbol& other) const
	{
		return nIndex == other.nIndex;
	}

	[[nodiscard]] bool IsValid() const
	{
		return nIndex != INVALID_UTL_SYMBOL;
	}

	explicit operator UtlSymbolID_t() const
	{
		return nIndex;
	}

protected:
	UtlSymbolID_t nIndex; // 0x00
};
static_assert(sizeof(CUtlSymbol) == 0x2);
