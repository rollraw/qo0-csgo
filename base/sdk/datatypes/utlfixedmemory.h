#pragma once
#include "../../common.h"

// @test: using interfaces in the header | not critical but could blow up someday with thousands of errors or affect to compilation time etc
// used: interface handles
#include "../../core/interfaces.h"
// used: interface declarations
#include "../interfaces/imemalloc.h"

// @source: master/public/tier1/utlfixedmemory.h

template <class T>
class CUtlFixedMemory
{
protected:
	struct BlockHeader_t
	{
		BlockHeader_t* pNext;
		std::intptr_t nBlockSize;
	};

public:
	class Iterator_t
	{
	public:
		Iterator_t(BlockHeader_t* pBlockHeader, const std::intptr_t nIndex) :
			pBlockHeader(pBlockHeader), nIndex(nIndex) { }

		bool operator==(const Iterator_t it) const
		{
			return pBlockHeader == it.pBlockHeader && nIndex == it.nIndex;
		}

		bool operator!=(const Iterator_t it) const
		{
			return pBlockHeader != it.pBlockHeader || nIndex != it.nIndex;
		}

		BlockHeader_t* pBlockHeader;
		std::intptr_t nIndex;
	};

	CUtlFixedMemory(const int nGrowSize = 0, const int nInitAllocationCount = 0) :
		pBlocks(nullptr), nAllocationCount(0), nGrowSize(nGrowSize)
	{
		Purge();
		Grow(nInitAllocationCount);
	}

	~CUtlFixedMemory()
	{
		Purge();
	}

	Q_CLASS_NO_ASSIGNMENT(CUtlFixedMemory)

	[[nodiscard]] T* Base()
	{
		return nullptr;
	}

	[[nodiscard]] const T* Base() const
	{
		return nullptr;
	}

	T& operator[](std::intptr_t nIndex)
	{
		Q_ASSERT(IsValidIndex(nIndex));
		return *reinterpret_cast<T*>(nIndex);
	}

	const T& operator[](std::intptr_t nIndex) const
	{
		Q_ASSERT(IsValidIndex(nIndex));
		return *reinterpret_cast<T*>(nIndex);
	}

	[[nodiscard]] T& Element(const std::intptr_t nIndex)
	{
		Q_ASSERT(IsValidIndex(nIndex));
		return *reinterpret_cast<T*>(nIndex);
	}

	[[nodiscard]] const T& Element(const std::intptr_t nIndex) const
	{
		Q_ASSERT(IsValidIndex(nIndex));
		return *reinterpret_cast<T*>(nIndex);
	}

	[[nodiscard]] Iterator_t First() const
	{
		return (pBlocks != nullptr ? Iterator_t(pBlocks, InvalidIndex()) : InvalidIterator());
	}

	[[nodiscard]] Iterator_t Next(const Iterator_t& it) const
	{
		if (!IsValidIterator(it))
			return InvalidIterator();

		BlockHeader_t* pHeader = it.pBlockHeader;

		if (it.nIndex + 1 < pHeader->nBlockSize)
			return Iterator_t(pHeader, it.nIndex + 1);

		return (pHeader->pNext != nullptr ? Iterator_t(pHeader->pNext, InvalidIndex()) : InvalidIterator());
	}

	[[nodiscard]] std::intptr_t GetIndex(const Iterator_t& it) const
	{
		if (!IsValidIterator(it))
			return InvalidIndex();

		return reinterpret_cast<std::intptr_t>(HeaderToBlock(it.pBlockHeader) + it.nIndex);
	}

	[[nodiscard]] bool IsIndexAfter(std::intptr_t nIndex, const Iterator_t& it) const
	{
		if (!IsValidIterator(it))
			return false;

		if (IsInBlock(nIndex, it.pBlockHeader))
			return nIndex > GetIndex(it);

		for (BlockHeader_t* pBlockHeader = it.pBlockHeader->pNext; pBlockHeader != nullptr; pBlockHeader = pBlockHeader->pNext)
		{
			if (IsInBlock(nIndex, pBlockHeader))
				return true;
		}

		return false;
	}

	[[nodiscard]] bool IsValidIterator(const Iterator_t& it) const
	{
		return it.pBlockHeader != nullptr && it.nIndex >= 0 && it.nIndex < it.pBlockHeader->nBlockSize;
	}

	[[nodiscard]] Iterator_t InvalidIterator() const
	{
		return Iterator_t(nullptr, InvalidIndex());
	}

	[[nodiscard]] bool IsValidIndex(const std::intptr_t nIndex) const
	{
		return nIndex != InvalidIndex();
	}

	[[nodiscard]] static std::intptr_t InvalidIndex()
	{
		return 0;
	}

	[[nodiscard]] int Count() const
	{
		return nAllocationCount;
	}

	void EnsureCapacity(const int nCapacity)
	{
		Grow(nCapacity - Count());
	}

	void Grow(const int nCount = 1)
	{
		if (nCount <= 0)
			return;

		int nBlockSize = (nGrowSize == 0 ? (nAllocationCount > 0 ? nAllocationCount : (31 + sizeof(T)) / sizeof(T)) : nGrowSize);

		if (nBlockSize < nCount)
			nBlockSize *= (nCount + nBlockSize - 1) / nBlockSize;

		nAllocationCount += nBlockSize;

		BlockHeader_t* pNewBlockHeader = static_cast<BlockHeader_t*>(I::MemAlloc->Alloc(sizeof(BlockHeader_t) + nBlockSize * sizeof(T)));
		Q_ASSERT(pNewBlockHeader != nullptr); // container overflow

		pNewBlockHeader->pNext = nullptr;
		pNewBlockHeader->nBlockSize = nBlockSize;

		if (pBlocks == nullptr)
			pBlocks = pNewBlockHeader;
		else
		{
			BlockHeader_t* pBlockHeader = pBlocks;

			while (pBlockHeader->pNext != nullptr)
				pBlockHeader = pBlockHeader->pNext;

			pBlockHeader->pNext = pNewBlockHeader;
		}
	}

	void Purge()
	{
		if (pBlocks == nullptr)
			return;

		for (BlockHeader_t* pBlockHeader = pBlocks; pBlockHeader != nullptr; )
		{
			BlockHeader_t* pFree = pBlockHeader;
			pBlockHeader = pBlockHeader->pNext;
			I::MemAlloc->Free(pFree);
		}

		pBlocks = nullptr;
		nAllocationCount = 0;
	}

protected:
	[[nodiscard]] bool IsInBlock(std::intptr_t nIndex, BlockHeader_t* pBlockHeader) const
	{
		T* pCurrent = reinterpret_cast<T*>(nIndex);
		const T* pStart = HeaderToBlock(pBlockHeader);
		const T* pEnd = pStart + pBlockHeader->nBlockSize;

		return (pCurrent >= pStart && pCurrent < pEnd);
	}

	[[nodiscard]] const T* HeaderToBlock(const BlockHeader_t* pHeader) const
	{
		return reinterpret_cast<const T*>(pHeader + 1);
	}

	[[nodiscard]] const BlockHeader_t* BlockToHeader(const T* pBlock) const
	{
		return reinterpret_cast<const BlockHeader_t*>(pBlock) - 1;
	}

	BlockHeader_t* pBlocks;
	int nAllocationCount;
	int nGrowSize;
};
