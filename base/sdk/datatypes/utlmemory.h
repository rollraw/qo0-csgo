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

		if (static_cast<int>(static_cast<N>(iNewAllocationCount)) < iAllocationRequested)
		{
			if (static_cast<int>(static_cast<N>(iNewAllocationCount)) == 0 && static_cast<int>(static_cast<N>(iNewAllocationCount - 1)) >= iAllocationRequested)
				--iNewAllocationCount;
			else
			{
				if (static_cast<int>(static_cast<N>(iAllocationRequested)) != iAllocationRequested)
				{
					return;
				}

				while (static_cast<int>(static_cast<N>(iNewAllocationCount)) < iAllocationRequested)
					iNewAllocationCount = (iNewAllocationCount + iAllocationRequested) / 2;
			}
		}

		iAllocationCount = iNewAllocationCount;

		if (pMemory != nullptr)
		{
			//pMemory = reinterpret_cast<T*>(I::MemAlloc->Realloc(pMemory, iAllocationCount * sizeof(T)));

			std::byte* pData = new std::byte[iAllocationCount * sizeof(T)];
			memcpy(pData, pMemory, iOldAllocationCount * sizeof(T));
			pMemory = reinterpret_cast<T*>(pData);
		}
		else
			//pMemory = reinterpret_cast<T*>(I::MemAlloc->Alloc(iAllocationCount * sizeof(T)));
			pMemory = reinterpret_cast<T*>(new std::byte[iAllocationCount * sizeof(T)]);
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
