#pragma once

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

	[[nodiscard]] int NumAllocated() const
	{
		return iAllocationCount;
	}
	
	[[nodiscard]] bool IsExternallyAllocated() const
	{
		return iGrowSize < 0;
	}

	void Grow(const int iCount = 1)
	{
		if (IsExternallyAllocated())
			return;

		int iAllocationRequested = iAllocationCount + iCount;
		int iNewAllocationCount = 0;

		if (iGrowSize)
			iAllocationCount = ((1 + ((iAllocationRequested - 1) / iGrowSize)) * iGrowSize);
		else
		{
			if (!iAllocationCount)
				iAllocationCount = (31 + sizeof(T)) / sizeof(T);

			while (iAllocationCount < iAllocationRequested)
				iAllocationCount <<= 1;
		}

		if (static_cast<int>(static_cast<N>(iNewAllocationCount)) < iAllocationRequested)
		{
			if (static_cast<int>(static_cast<N>(iNewAllocationCount)) == 0 && static_cast<int>(static_cast<N>(iNewAllocationCount - 1)) >= iAllocationRequested)
				--iNewAllocationCount;
			else
			{
				if (static_cast<int>(static_cast<N>(iAllocationRequested)) != iAllocationRequested)
					return;

				while (static_cast<int>(static_cast<N>(iNewAllocationCount)) < iAllocationRequested)
					iNewAllocationCount = (iNewAllocationCount + iAllocationRequested) / 2;
			}
		}

		iAllocationCount = iNewAllocationCount;

		if (pMemory != nullptr)
			pMemory = static_cast<T*>(realloc(pMemory, iAllocationCount * sizeof(T)));
		else
			pMemory = static_cast<T*>(malloc(iAllocationCount * sizeof(T)));
	}

protected:
	T*	pMemory;
	int iAllocationCount;
	int iGrowSize;
};
