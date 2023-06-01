#pragma once
#include "utlmemory.h"

// @source: master/public/tier1/utlstack.h

/*
 * a growable stack class which doubles in size by default
 * it will always keep all elements consecutive in memory, and may move the
 * elements around in memory (via a realloc) when elements are pushed or popped
 * clients should therefore refer to the elements of the stack
 * by index (they should *never* maintain pointers to elements in the stack).
 */
template <class T, class M = CUtlMemory<T>>
class CUtlStack
{
public:
	T& operator[](const int nIndex)
	{
		return memory[nIndex];
	}

	const T& operator[](const int nIndex) const
	{
		return memory[nIndex];
	}

	[[nodiscard]] T& Element(const int nIndex)
	{
		return memory[nIndex];
	}

	[[nodiscard]] const T& Element(const int nIndex) const
	{
		return memory[nIndex];
	}

	[[nodiscard]] T* Base()
	{
		return memory.Base();
	}

	[[nodiscard]] const T* Base() const
	{
		return memory.Base();
	}

	[[nodiscard]] T& Top()
	{
		return Element(nSize - 1);
	}

	[[nodiscard]] const T& Top() const
	{
		return Element(nSize - 1);
	}

	[[nodiscard]] int Count() const
	{
		return nSize;
	}

	int Push()
	{
		GrowStack();
		Construct(&Element(nSize - 1));
		return nSize - 1;
	}

	int Push(const T& source)
	{
		GrowStack();
		new (&Element(nSize - 1)) T(source);
		return nSize - 1;
	}

	void Pop()
	{
		Q_ASSERT(nSize > 0);
		(&Element(nSize - 1))->~T();
		--nSize;
	}

	void Pop(T& oldTop)
	{
		Q_ASSERT(nSize > 0);
		oldTop = Top();
		Pop();
	}

	void EnsureCapacity(const int nCapacity)
	{
		memory.EnsureCapacity(nCapacity);
	}

	void Clear()
	{
		for (int i = nSize; --i >= 0;)
			(&Element(i))->~T();

		nSize = 0;
	}

	void Purge()
	{
		Clear();
		memory.Purge();
	}

private:
	void GrowStack()
	{
		if (nSize >= memory.AllocationCount())
			memory.Grow();

		++nSize;
	}

	M memory;
	int	nSize;
	T* pElements;
};
