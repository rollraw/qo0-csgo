#pragma once

inline int UtlMemory_CalcNewAllocationCount(int nAllocationCount, int nGrowSize, int nNewSize, int nBytesItem)
{
	if (nGrowSize)
		nAllocationCount = ((1 + ((nNewSize - 1) / nGrowSize)) * nGrowSize);
	else
	{
		if (!nAllocationCount)
			nAllocationCount = (31 + nBytesItem) / nBytesItem;

		while (nAllocationCount < nNewSize)
			nAllocationCount *= 2;
	}

	return nAllocationCount;
}

template <class T, class N = int>
class CUtlMemory
{
public:
	T& operator[](N i)
	{
		return pMemory[i];
	}

	const T& operator[](N i) const
	{
		return pMemory[i];
	}

	T* Base()
	{
		return pMemory;
	}

	int NumAllocated() const
	{
		return iAllocationCount;
	}

	void Grow(int iNum = 1)
	{
		if (IsExternallyAllocated())
			return;

		int iOldAllocationCount = iAllocationCount;
		int iAllocationRequested = iAllocationCount + iNum;
		int iNewAllocationCount = UtlMemory_CalcNewAllocationCount(iAllocationCount, iGrowSize, iAllocationRequested, sizeof(T));

		if ((int)(N)iNewAllocationCount < iAllocationRequested)
		{
			if ((int)(N)iNewAllocationCount == 0 && (int)(N)(iNewAllocationCount - 1) >= iAllocationRequested)
				--iNewAllocationCount;
			else
			{
				if ((int)(N)iAllocationRequested != iAllocationRequested)
				{
					return;
				}

				while ((int)(N)iNewAllocationCount < iAllocationRequested)
					iNewAllocationCount = (iNewAllocationCount + iAllocationRequested) / 2;
			}
		}

		iAllocationCount = iNewAllocationCount;

		if (pMemory != nullptr)
		{
			// fuck msvc
			//pMemory = (T*)I::MemAlloc->Realloc(pMemory, iAllocationCount * sizeof(T));

			auto pData = new std::byte[iAllocationCount * sizeof(T)];
			memcpy(pData, pMemory, iOldAllocationCount * sizeof(T));
			pMemory = (T*)pData;
		}
		else
			//pMemory = (T*)I::MemAlloc->Alloc(iAllocationCount * sizeof(T));
			pMemory = (T*)new std::byte[iAllocationCount * sizeof(T)];
	}

	bool IsExternallyAllocated() const
	{
		return iGrowSize < 0;
	}

protected:
	T*	pMemory;
	int iAllocationCount;
	int iGrowSize;
};
