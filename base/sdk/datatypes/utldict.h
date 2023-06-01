#pragma once
#include "utlmap.h"

template <class T, class I = int>
class CUtlDict
{
public:
	using KeyType_t = const char*;
	using ElementType_t = T;
	using IndexType_t = I;

	enum EDictCompareType
	{
		DICT_COMPARE_TYPE_CASESENSITIVE = 0,
		DICT_COMPARE_TYPE_CASEINSENSITIVE,
		DICT_COMPARE_TYPE_FILENAMES
	};

	CUtlDict(int nCompareType = DICT_COMPARE_TYPE_CASEINSENSITIVE, int nGrowSize = 0, int nInitialSize = 0)
	{
		Q_ASSERT(false); // not implemented
	}

	~CUtlDict()
	{
		Q_ASSERT(false); // not implemented
		//Purge();
	}

	[[nodiscard]] T& operator[](I nIndex)
	{
		return mapElements[nIndex];
	}

	[[nodiscard]] const T& operator[](I nIndex) const
	{
		return mapElements[nIndex];
	}

	[[nodiscard]] T& Element(I nIndex)
	{
		return mapElements[nIndex];
	}

	[[nodiscard]] const T& Element(I nIndex) const
	{
		return mapElements[nIndex];
	}

	[[nodiscard]] unsigned int Count() const
	{
		return mapElements.Count();
	}

	[[nodiscard]] I MaxElement() const
	{
		return mapElements.MaxElement();
	}

	[[nodiscard]] bool IsValidIndex(I nIndex) const
	{
		return mapElements.IsValidIndex(nIndex);
	}

	static I InvalidIndex()
	{
		return CUtlMap<const char*, T, I>::InvalidIndex();
	}

	void EnsureCapacity(const int nCapacity)
	{
		return mapElements.EnsureCapacity(nCapacity);
	}

	[[nodiscard]] const char* GetElementName(I nIndex) const
	{
		return mapElements.Key(nIndex);
	}

	[[nodiscard]] I Find(const char* szName) const
	{
		if (szName != nullptr)
			return mapElements.Find(szName);

		return InvalidIndex();
	}

protected:
	CUtlMap<const char*, T, I> mapElements;
};
