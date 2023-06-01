#pragma once
// used: memorycopy
#include "../../utilities/crt.h"

// @test: using interfaces in the header | not critical but could blow up someday with thousands of errors or affect to compilation time etc
// used: interface handles
#include "../../core/interfaces.h"
// used: interface declarations
#include "../interfaces/imemalloc.h"

// @source: master/public/tier1/utlmemory.h

template <class T, class N = int>
class CUtlMemory
{
	enum
	{
		EXTERNAL_BUFFER_MARKER = -1,
		EXTERNAL_CONST_BUFFER_MARKER = -2,
	};

public:
	class Iterator_t
	{
	public:
		Iterator_t(const N nIndex) :
			nIndex(nIndex) { }

		bool operator==(const Iterator_t it) const
		{
			return nIndex == it.nIndex;
		}

		bool operator!=(const Iterator_t it) const
		{
			return nIndex != it.nIndex;
		}

		N nIndex;
	};

	CUtlMemory(const int nInitialGrowSize, const int nAllocationCount) :
		pMemory(nullptr), nAllocationCount(nAllocationCount), nGrowSize(nInitialGrowSize)
	{
		Q_ASSERT(nInitialGrowSize >= 0);

		if (nAllocationCount > 0)
			pMemory = static_cast<T*>(I::MemAlloc->Alloc(nAllocationCount * sizeof(T)));
	}

	CUtlMemory(T* pMemory, const int nElements) :
		pMemory(pMemory), nAllocationCount(nElements), nGrowSize(EXTERNAL_BUFFER_MARKER) { }

	CUtlMemory(const T* pMemory, const int nElements) :
		pMemory(pMemory), nAllocationCount(nElements), nGrowSize(EXTERNAL_CONST_BUFFER_MARKER) { }

	~CUtlMemory()
	{
		Purge();
	}

	CUtlMemory(const CUtlMemory&) = delete;

	CUtlMemory(CUtlMemory&& moveFrom) noexcept :
		pMemory(moveFrom.pMemory), nAllocationCount(moveFrom.nAllocationCount), nGrowSize(moveFrom.nGrowSize)
	{
		moveFrom.pMemory = nullptr;
		moveFrom.nAllocationCount = 0;
		moveFrom.nGrowSize = 0;
	}

	void* operator new(const std::size_t nSize)
	{
		return I::MemAlloc->Alloc(nSize);
	}

	void operator delete(void* pMemory)
	{
		I::MemAlloc->Free(pMemory);
	}

	CUtlMemory& operator=(const CUtlMemory&) = delete;

	CUtlMemory& operator=(CUtlMemory&& moveFrom) noexcept
	{
		// copy member variables to locals before purge to handle self-assignment
		T* pMemoryTemp = moveFrom.pMemory;
		const int nAllocationCountTemp = moveFrom.nAllocationCount;
		const int nGrowSizeTemp = moveFrom.nGrowSize;

		moveFrom.pMemory = nullptr;
		moveFrom.nAllocationCount = 0;
		moveFrom.nGrowSize = 0;

		// if this is a self-assignment, Purge() is a no-op here
		Purge();

		pMemory = pMemoryTemp;
		nAllocationCount = nAllocationCountTemp;
		nGrowSize = nGrowSizeTemp;
		return *this;
	}

	[[nodiscard]] T& operator[](const N nIndex)
	{
		Q_ASSERT(IsValidIndex(nIndex));
		return pMemory[nIndex];
	}

	[[nodiscard]] const T& operator[](const N nIndex) const
	{
		Q_ASSERT(IsValidIndex(nIndex));
		return pMemory[nIndex];
	}

	[[nodiscard]] T& Element(const N nIndex)
	{
		Q_ASSERT(IsValidIndex(nIndex));
		return pMemory[nIndex];
	}

	[[nodiscard]] const T& Element(const N nIndex) const
	{
		Q_ASSERT(IsValidIndex(nIndex));
		return pMemory[nIndex];
	}

	[[nodiscard]] T* Base()
	{
		return pMemory;
	}

	[[nodiscard]] const T* Base() const
	{
		return pMemory;
	}

	[[nodiscard]] int AllocationCount() const
	{
		return nAllocationCount;
	}

	[[nodiscard]] bool IsExternallyAllocated() const
	{
		return nGrowSize <= EXTERNAL_BUFFER_MARKER;
	}

	[[nodiscard]] static N InvalidIndex()
	{
		return static_cast<N>(-1);
	}

	[[nodiscard]] bool IsValidIndex(N nIndex) const
	{
		return (nIndex >= 0) && (nIndex < nAllocationCount);
	}

	[[nodiscard]] Iterator_t First() const
	{
		return Iterator_t(IsValidIndex(0) ? 0 : InvalidIndex());
	}

	[[nodiscard]] Iterator_t Next(const Iterator_t& it) const
	{
		return Iterator_t(IsValidIndex(it.nIndex + 1) ? it.nIndex + 1 : InvalidIndex());
	}

	[[nodiscard]] N GetIndex(const Iterator_t& it) const
	{
		return it.nIndex;
	}

	[[nodiscard]] bool IsIndexAfter(N nIndex, const Iterator_t& it) const
	{
		return nIndex > it.nIndex;
	}

	[[nodiscard]] bool IsValidIterator(const Iterator_t& it) const
	{
		return IsValidIndex(it.index);
	}

	[[nodiscard]] Iterator_t InvalidIterator() const
	{
		return Iterator_t(InvalidIndex());
	}

	void Grow(const int nCount = 1)
	{
		if (IsExternallyAllocated())
			return;

		int nAllocationRequested = nAllocationCount + nCount;
		int nNewAllocationCount = 0;

		if (nGrowSize)
			nAllocationCount = ((1 + ((nAllocationRequested - 1) / nGrowSize)) * nGrowSize);
		else
		{
			if (nAllocationCount == 0)
				nAllocationCount = (31 + sizeof(T)) / sizeof(T);

			while (nAllocationCount < nAllocationRequested)
				nAllocationCount <<= 1;
		}

		if (static_cast<int>(static_cast<N>(nNewAllocationCount)) < nAllocationRequested)
		{
			if (static_cast<int>(static_cast<N>(nNewAllocationCount)) == 0 && static_cast<int>(static_cast<N>(nNewAllocationCount - 1)) >= nAllocationRequested)
				--nNewAllocationCount;
			else
			{
				if (static_cast<int>(static_cast<N>(nAllocationRequested)) != nAllocationRequested)
					return;

				while (static_cast<int>(static_cast<N>(nNewAllocationCount)) < nAllocationRequested)
					nNewAllocationCount = (nNewAllocationCount + nAllocationRequested) / 2;
			}
		}

		nAllocationCount = nNewAllocationCount;

		// @test: we can always call realloc, since it must allocate instead when passed null ptr
		if (pMemory != nullptr)
			pMemory = static_cast<T*>(I::MemAlloc->Realloc(pMemory, nAllocationCount * sizeof(T)));
		else
			pMemory = static_cast<T*>(I::MemAlloc->Alloc(nAllocationCount * sizeof(T)));
	}

	void EnsureCapacity(const int nCapacity)
	{
		if (nAllocationCount >= nCapacity)
			return;

		if (IsExternallyAllocated())
		{
			// can't grow a buffer whose memory was externally allocated
			Q_ASSERT(false);
			return;
		}

		nAllocationCount = nCapacity;

		// @test: we can always call realloc, since it must allocate instead when passed null ptr
		if (pMemory != nullptr)
			pMemory = static_cast<T*>(I::MemAlloc->Realloc(pMemory, nAllocationCount * sizeof(T)));
		else
			pMemory = static_cast<T*>(I::MemAlloc->Alloc(nAllocationCount * sizeof(T)));
	}

	void ConvertToGrowableMemory(int nInitialGrowSize)
	{
		if (!IsExternallyAllocated())
			return;

		nGrowSize = nInitialGrowSize;

		if (nAllocationCount > 0)
		{
			const int nByteCount = nAllocationCount * sizeof(T);
			T* pGrowableMemory = static_cast<T*>(I::MemAlloc->Alloc(nByteCount));
			CRT::MemoryCopy(pGrowableMemory, pMemory, nByteCount);
			pMemory = pGrowableMemory;
		}
		else
			pMemory = nullptr;
	}

	void Purge()
	{
		if (IsExternallyAllocated())
			return;

		if (pMemory != nullptr)
		{
			I::MemAlloc->Free(static_cast<void*>(pMemory));
			pMemory = nullptr;
		}

		nAllocationCount = 0;
	}

	void Purge(const int nElements)
	{
		Q_ASSERT(nElements >= 0);

		if (nElements > nAllocationCount)
		{
			// ensure this isn't a grow request in disguise
			Q_ASSERT(nElements <= nAllocationCount);
			return;
		}

		if (nElements == 0)
		{
			Purge();
			return;
		}

		if (IsExternallyAllocated() || nElements == nAllocationCount)
			return;

		if (pMemory == nullptr)
		{
			// allocation count is non zero, but memory is null
			Q_ASSERT(false);
			return;
		}

		nAllocationCount = nElements;
		pMemory = static_cast<T*>(I::MemAlloc->Realloc(pMemory, nAllocationCount * sizeof(T)));
	}

protected:
	T* pMemory; // 0x00
	int nAllocationCount; // 0x04
	int nGrowSize; // 0x08
};
static_assert(sizeof(CUtlMemory<void*>) == 0xC);

template <class T, int nAlignment>
class CUtlMemoryAligned : public CUtlMemory<T>
{
public:
	// @note: not implemented
	Q_CLASS_NO_INITIALIZER(CUtlMemoryAligned);
};

template <class T, std::size_t SIZE, class I = int>
class CUtlMemoryFixedGrowable : public CUtlMemory<T, I>
{
	typedef CUtlMemory<T, I> BaseClass;

public:
	CUtlMemoryFixedGrowable(int nInitialGrowSize = 0, int nInitialSize = SIZE) :
		BaseClass(arrFixedMemory, SIZE)
	{
		Q_ASSERT(nInitialSize == 0 || nInitialSize == SIZE);
		nMallocGrowSize = nInitialGrowSize;
	}

	void Grow(int nCount = 1)
	{
		if (this->IsExternallyAllocated())
			this->ConvertToGrowableMemory(nMallocGrowSize);

		BaseClass::Grow(nCount);
	}

	void EnsureCapacity(int nCapacity)
	{
		if (CUtlMemory<T>::nAllocationCount >= nCapacity)
			return;

		if (this->IsExternallyAllocated())
			// can't grow a buffer whose memory was externally allocated
			this->ConvertToGrowableMemory(nMallocGrowSize);

		BaseClass::EnsureCapacity(nCapacity);
	}

private:
	int nMallocGrowSize;
	T arrFixedMemory[SIZE];
};

template <typename T, std::size_t SIZE, int nAlignment = 0>
class CUtlMemoryFixed
{
public:
	CUtlMemoryFixed(const int nGrowSize = 0, const int nInitialCapacity = 0)
	{
		Q_ASSERT(nInitialCapacity == 0 || nInitialCapacity == SIZE);
	}

	CUtlMemoryFixed(const T* pMemory, const int nElements)
	{
		Q_ASSERT(false);
	}

	[[nodiscard]] static constexpr bool IsValidIndex(const int nIndex)
	{
		return (nIndex >= 0) && (nIndex < SIZE);
	}

	// specify the invalid ('null') index that we'll only return on failure
	static constexpr int INVALID_INDEX = -1;
	[[nodiscard]] static constexpr int InvalidIndex() { return INVALID_INDEX; }

	[[nodiscard]] T* Base()
	{
		if (nAlignment == 0)
			return reinterpret_cast<T*>(&pMemory[0]);

		return reinterpret_cast<T*>((reinterpret_cast<std::uintptr_t>(&pMemory[0]) + nAlignment - 1) & ~(nAlignment - 1));
	}

	[[nodiscard]] const T* Base() const
	{
		if (nAlignment == 0)
			return reinterpret_cast<T*>(&pMemory[0]);

		return reinterpret_cast<T*>((reinterpret_cast<std::uintptr_t>(&pMemory[0]) + nAlignment - 1) & ~(nAlignment - 1));
	}

	[[nodiscard]] T& operator[](int nIndex)
	{
		Q_ASSERT(IsValidIndex(nIndex));
		return Base()[nIndex];
	}

	[[nodiscard]] const T& operator[](int nIndex) const
	{
		Q_ASSERT(IsValidIndex(nIndex));
		return Base()[nIndex];
	}

	[[nodiscard]] T& Element(int nIndex)
	{
		Q_ASSERT(IsValidIndex(nIndex));
		return Base()[nIndex];
	}

	[[nodiscard]] const T& Element(int nIndex) const
	{
		Q_ASSERT(IsValidIndex(nIndex));
		return Base()[nIndex];
	}

	[[nodiscard]] int AllocationCount() const { return SIZE; }
	[[nodiscard]] int Count() const { return SIZE; }

	void Grow(int nCount = 1) { Q_ASSERT(false); }
	void EnsureCapacity(const int nCapacity) { Q_ASSERT(nCapacity <= SIZE); }

	void Purge() { }
	void Purge(const int nElements) { Q_ASSERT(false); }
	[[nodiscard]] bool IsExternallyAllocated() const { return false; }

	class Iterator_t
	{
	public:
		Iterator_t(const int nIndex) :
			nIndex(nIndex) { }

		bool operator==(const Iterator_t it) const
		{
			return nIndex == it.nIndex;
		}

		bool operator!=(const Iterator_t it) const
		{
			return nIndex != it.nIndex;
		}

		int nIndex;
	};

	[[nodiscard]] Iterator_t First() const
	{
		return Iterator_t(IsValidIndex(0) ? 0 : InvalidIndex());
	}

	[[nodiscard]] Iterator_t Next(const Iterator_t& it) const
	{
		return Iterator_t(IsValidIndex(it.nIndex + 1) ? it.nIndex + 1 : InvalidIndex());
	}

	[[nodiscard]] int GetIndex(const Iterator_t& it) const
	{
		return it.nIndex;
	}

	[[nodiscard]] bool IsIndexAfter(int i, const Iterator_t& it) const
	{
		return i > it.nIndex;
	}

	[[nodiscard]] bool IsValidIterator(const Iterator_t& it) const
	{
		return IsValidIndex(it.nIndex);
	}

	[[nodiscard]] Iterator_t InvalidIterator() const
	{
		return Iterator_t(InvalidIndex());
	}

private:
	char pMemory[SIZE * sizeof(T) + nAlignment];
};
