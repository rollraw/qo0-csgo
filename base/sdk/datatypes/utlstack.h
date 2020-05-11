#pragma once
// used: utlmemory
#include "utlmemory.h"

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
	T& operator[](int i)
	{
		return pMemory[i];
	}

	const T& operator[](int i) const
	{
		return pMemory[i];
	}

	T& Element(int i)
	{
		return pMemory[i];
	}

	const T& Element(int i) const
	{
		return pMemory[i];
	}

	T* Base()
	{
		return pMemory.Base();
	}

	const T* Base() const
	{
		return pMemory.Base();
	}

	T& Top()
	{
		return Element(iSize - 1);
	}

	const T& Top() const
	{
		return Element(iSize - 1);
	}

	int Count() const
	{
		return iSize;
	}

private:
	M	pMemory;
	int	iSize;
	T*	pElements;
};
