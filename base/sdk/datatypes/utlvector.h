#pragma once
// used: utlmemory
#include "utlmemory.h"

// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/public/tier1/utlvector.h

template <class T>
void Destruct(T* pMemory)
{
	pMemory->~T();
}

template <class T>
T* Construct(T* pMemory)
{
	return new(pMemory) T;
}

template< class T >
T* Copy(T* pMemory, T const& src)
{
	return new(pMemory) T(src);
}

template <class T, class A = CUtlMemory<T>>
class CUtlVector
{
	using CAllocator = A;
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

	int Count() const
	{
		return iSize;
	}

	int& Size()
	{
		return iSize;
	}

	void GrowVector(int iNum = 1)
	{
		if (iSize + iNum > pMemory.NumAllocated())
			pMemory.Grow(iSize + iNum - pMemory.NumAllocated());

		iSize += iNum;
	}

	void ShiftElementsRight(int nElement, int iNum = 1)
	{
		int nToMove = iSize - nElement - iNum;
		if (nToMove > 0 && iNum > 0)
			memmove(&Element(nElement + iNum), &Element(nElement), nToMove * sizeof(T));
	}

	void ShiftElementsLeft(int nElement, int iNum = 1)
	{
		int nToMove = iSize - nElement - iNum;
		if (nToMove > 0 && iNum > 0)
			memmove(&Element(nElement), &Element(nElement + iNum), nToMove * sizeof(T));
	}

	int InsertBefore(int nElement)
	{
		// can insert at the end
		GrowVector();
		ShiftElementsRight(nElement);
		Construct(&Element(nElement));
		return nElement;
	}

	int InsertBefore(int nElement, const T& src)
	{
		// reallocate if can't insert something that's in the list
		// can insert at the end
		GrowVector();
		ShiftElementsRight(nElement);
		Copy(&Element(nElement), src);
		return nElement;
	}

	int AddToTail()
	{
		return InsertBefore(iSize);
	}

	int AddToTail(const T& src)
	{
		return InsertBefore(iSize, src);
	}

	int Find(const T& src) const
	{
		for (int i = 0; i < Count(); ++i)
		{
			if (Element(i) == src)
				return i;
		}

		return -1;
	}

	void Remove(int nElement)
	{
		Destruct(&Element(nElement));
		ShiftElementsLeft(nElement);
		--iSize;
	}

	void RemoveAll()
	{
		for (int i = iSize; --i >= 0;)
			Destruct(&Element(i));

		iSize = 0;
	}

	bool FindAndRemove(const T& src)
	{
		int nElement = Find(src);
		if (nElement != -1)
		{
			Remove(nElement);
			return true;
		}

		return false;
	}

protected:
	CAllocator pMemory;
	int iSize;
	T* pElements;
};
