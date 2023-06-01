#pragma once
#include "utlmemory.h"

// @source: master/public/tier1/utlrbtree.h

template <typename T>
class CDefLess
{
public:
	CDefLess() { }
	CDefLess(int) { }

	Q_INLINE bool operator()(const T& left, const T& right) const
	{
		return (left < right);
	}

	Q_INLINE bool operator!() const
	{
		return false;
	}
};

template <class I>
struct UtlRBTreeLinks_t
{
	I iLeft;
	I iRight;
	I iParent;
	I iTag;
};

template <class T, class I>
struct UtlRBTreeNode_t : public UtlRBTreeLinks_t<I>
{
	T data;
};

template <class T, class I = unsigned short, typename L = bool(Q_CDECL*)(const T&, const T&), class M = CUtlMemory<UtlRBTreeNode_t<T, I>, I>>
class CUtlRBTree
{
public:
	using KeyType_t = T;
	using ElementType_t = T;
	using IndexType_t = I;
	using LessCallbackFn_t = L;

	enum NodeColor_t
	{
		RED = 0,
		BLACK
	};

	explicit CUtlRBTree(int nGrowSize = 0, int nInitialSize = 0, const LessCallbackFn_t& fnLessCallback = nullptr) :
		fnLessCallback(fnLessCallback), memory(nGrowSize, nInitialSize), iRoot(InvalidIndex()), nElements(0), iFirstFree(InvalidIndex()), itLastAlloc(memory.InvalidIterator()), pElements(memory.Base()) { }

	explicit CUtlRBTree(const LessCallbackFn_t& fnLessCallback) :
		fnLessCallback(fnLessCallback), memory(0, 0), iRoot(InvalidIndex()), nElements(0), iFirstFree(InvalidIndex()), itLastAlloc(memory.InvalidIterator()), pElements(memory.Base()) { }

	~CUtlRBTree()
	{
		Purge();
	}

	[[nodiscard]] T& operator[](I nIndex)
	{
		Q_ASSERT(IsValidIndex(nIndex));
		return memory[nIndex].data;
	}

	[[nodiscard]] const T& operator[](I nIndex) const
	{
		Q_ASSERT(IsValidIndex(nIndex));
		return memory[nIndex].data;
	}

	[[nodiscard]] T& Element(I nIndex)
	{
		Q_ASSERT(IsValidIndex(nIndex));
		return memory[nIndex].data;
	}

	[[nodiscard]] const T& Element(I nIndex) const
	{
		Q_ASSERT(IsValidIndex(nIndex));
		return memory[nIndex].data;
	}

	[[nodiscard]] const UtlRBTreeLinks_t<I>& Links(I nIndex) const
	{
		constexpr UtlRBTreeLinks_t<I> linksSentinel =
		{
			M::INVALID_INDEX, M::INVALID_INDEX, M::INVALID_INDEX, BLACK
		};

		return (nIndex != InvalidIndex()) ? memory[nIndex] : linksSentinel;
	}

	[[nodiscard]] UtlRBTreeLinks_t<I>& Links(I nIndex)
	{
		Q_ASSERT(nIndex != InvalidIndex());
		return memory[nIndex];
	}

	[[nodiscard]] I Parent(I nIndex) const
	{
		return (nIndex != InvalidIndex() ? memory[nIndex].iParent : InvalidIndex());
	}

	[[nodiscard]] I LeftChild(I nIndex) const
	{
		return (nIndex != InvalidIndex() ? memory[nIndex].iLeft : InvalidIndex());
	}

	[[nodiscard]] I RightChild(I nIndex) const
	{
		return (nIndex != InvalidIndex() ? memory[nIndex].iRight : InvalidIndex());
	}

	void SetParent(I nIndex, I iParent)
	{
		Links(nIndex).iParent = iParent;
	}

	void SetLeftChild(I nIndex, I iChild)
	{
		Links(nIndex).iLeft = iChild;
	}

	void SetRightChild(I nIndex, I iChild)
	{
		Links(nIndex).iRight = iChild;
	}

	[[nodiscard]] bool IsRoot(I nIndex) const
	{
		return nIndex == iRoot;
	}

	[[nodiscard]] bool IsLeaf(I nIndex) const
	{
		return (LeftChild(nIndex) == InvalidIndex()) && (RightChild(nIndex) == InvalidIndex());
	}

	[[nodiscard]] unsigned int Count() const
	{
		return nElements;
	}

	[[nodiscard]] I MaxElement() const
	{
		return static_cast<I>(memory.NumAllocated());
	}

	[[nodiscard]] bool IsValidIndex(I nIndex) const
	{
		if (!memory.IsValidIndex(nIndex))
			return false;

		// don't read values that have been allocated, but not constructed
		if (memory.IsIndexAfter(nIndex, itLastAlloc))
			return false;

		return LeftChild(nIndex) != nIndex;
	}

	[[nodiscard]] static I InvalidIndex()
	{
		return static_cast<I>(M::InvalidIndex());
	}

	void EnsureCapacity(const int nCapacity)
	{
		memory.EnsureCapacity(nCapacity);
	}

	[[nodiscard]] I Find(const T& search) const
	{
		Q_ASSERT(!!fnLessCallback);

		I iCurrent = iRoot;
		while (iCurrent != InvalidIndex())
		{
			if (fnLessCallback(search, Element(iCurrent)))
				iCurrent = LeftChild(iCurrent);
			else if (fnLessCallback(Element(iCurrent), search))
				iCurrent = RightChild(iCurrent);
			else
				break;
		}

		return iCurrent;
	}

	void RemoveAll()
	{
		if (itLastAlloc == memory.InvalidIterator())
		{
			Q_ASSERT(iRoot == InvalidIndex());
			Q_ASSERT(iFirstFree == InvalidIndex());
			Q_ASSERT(nElements == 0);
			return;
		}

		for (typename M::Iterator_t it = memory.First(); it != memory.InvalidIterator(); it = memory.Next(it))
		{
			// skip elements in the free list
			if (I nIndex = memory.GetIndex(it); IsValidIndex(nIndex))
			{
				(&Element(nIndex))->~T();
				SetRightChild(nIndex, iFirstFree);
				SetLeftChild(nIndex, nIndex);
				iFirstFree = nIndex;
			}

			// don't destruct elements that haven't ever been constucted
			if (it == itLastAlloc)
				break;
		}

		// clear everything else out
		iRoot = InvalidIndex();
		nElements = 0;
		//Q_ASSERT(IsValid());
	}

	void Purge()
	{
		RemoveAll();
		iFirstFree = InvalidIndex();

		memory.Purge();
		itLastAlloc = memory.InvalidIterator();
	}

public:
	LessCallbackFn_t fnLessCallback; // 0x00
	M memory; // 0x04
	I iRoot; // 0x10
	I nElements;
	I iFirstFree;
	typename M::Iterator_t itLastAlloc;
	UtlRBTreeNode_t<T, I>* pElements;
};
