#pragma once
#include "utlmemory.h"
#include "utlfixedmemory.h"

// @source: master/public/tier1/utllinkedlist.h

template <class T, class I>
struct UtlLinkedListElement_t
{
	UtlLinkedListElement_t(const UtlLinkedListElement_t&) = delete;

	T element;
	I iPrevious;
	I iNext;
};

template <class T, class S = unsigned short, bool ML = false, class I = S, class M = CUtlMemory<UtlLinkedListElement_t<T, S>, I>>
class CUtlLinkedList
{
public:
	using ElemType_t = T;
	using IndexType_t = S;
	using IndexLocalType_t = I;
	using MemoryAllocator_t = M;

	template <typename List_t>
	class ConstIterator_t
	{
	public:
		typedef typename List_t::ElemType_t ElemType_t;
		typedef typename List_t::IndexType_t IndexType_t;

		ConstIterator_t() :
			pList(nullptr), nIndex(List_t::InvalidIndex()) { }

		ConstIterator_t(const List_t& list, IndexType_t nIndex) :
			pList(&list), nIndex(nIndex) { }

		ConstIterator_t& operator++()
		{
			nIndex = pList->Next(nIndex);
			return *this;
		}

		ConstIterator_t operator++(int)
		{
			ConstIterator_t pCopy = *this;
			++(*this);
			return pCopy;
		}

		ConstIterator_t& operator--()
		{
			Q_ASSERT(nIndex != pList->Head());
			nIndex = (nIndex == pList->InvalidIndex() ? pList->Tail() : pList->Previous(nIndex));
			return *this;
		}

		ConstIterator_t operator--(int)
		{
			ConstIterator_t pCopy = *this;
			--(*this);
			return pCopy;
		}

		bool operator==(const ConstIterator_t& other) const
		{
			Q_ASSERT(pList == other.pList);
			return nIndex == other.nIndex;
		}

		bool operator!=(const ConstIterator_t& other) const
		{
			Q_ASSERT(pList == other.pList);
			return nIndex != other.nIndex;
		}

		const ElemType_t& operator*() const
		{
			return pList->Element(nIndex);
		}

		const ElemType_t* operator->() const
		{
			return (&**this);
		}

	protected:
		const List_t* pList;
		IndexType_t nIndex;
	};

	template <typename List_t>
	class Iterator_t : public ConstIterator_t<List_t>
	{
	public:
		using ElemType_t = typename List_t::ElemType_t;
		using IndexType_t = typename List_t::IndexType_t;
		using Base_t = ConstIterator_t<List_t>;

		Iterator_t() { }

		Iterator_t(const List_t& list, IndexType_t nIndex) :
			ConstIterator_t<List_t>(list, nIndex) { }

		Iterator_t& operator++()
		{
			Base_t::nIndex = Base_t::pList->Next(Base_t::nIndex);
			return *this;
		}

		Iterator_t operator++(int)
		{
			Iterator_t pCopy = *this;
			++(*this);
			return pCopy;
		}

		Iterator_t& operator--()
		{
			Base_t::nIndex = (Base_t::nIndex == Base_t::pList->InvalidIndex() ? Base_t::pList->Tail() : Base_t::pList->Previous(Base_t::nIndex));
			return *this;
		}

		Iterator_t operator--(int)
		{
			Iterator_t pCopy = *this;
			--(*this);
			return pCopy;
		}

		ElemType_t& operator*() const
		{
			List_t* pMutableList = const_cast<List_t*>(Base_t::pList);
			return pMutableList->Element(Base_t::nIndex);
		}

		ElemType_t* operator->() const
		{
			return (&**this);
		}
	};

	CUtlLinkedList(int nGrowSize = 0, int nSize = 0) :
		memory(nGrowSize, nSize), iHead(InvalidIndex()), iTail(InvalidIndex()), iFirstFree(InvalidIndex()), nElementCount(0), nAllocated(0), itLastAlloc(memory.InvalidIterator()), pElements(memory.Base()) { }

	~CUtlLinkedList()
	{
		RemoveAll();
	}

	CUtlLinkedList(const CUtlLinkedList&) = delete;
	CUtlLinkedList& operator=(const CUtlLinkedList&) = delete;

	T& operator[](const I nIndex)
	{
		Q_ASSERT(IsValidIndex(nIndex));
		return memory[nIndex].element;
	}

	const T& operator[](const I nIndex) const
	{
		Q_ASSERT(IsValidIndex(nIndex));
		return memory[nIndex].element;
	}

	[[nodiscard]] T& Element(const I nIndex)
	{
		Q_ASSERT(IsValidIndex(nIndex));
		return memory[nIndex].element;
	}

	[[nodiscard]] const T& Element(const I nIndex) const
	{
		Q_ASSERT(IsValidIndex(nIndex));
		return memory[nIndex].element;
	}

	[[nodiscard]] I Head() const
	{
		return iHead;
	}

	[[nodiscard]] I Tail() const
	{
		return iTail;
	}

	[[nodiscard]] I Previous(const I nIndex) const
	{
		Q_ASSERT(IsValidIndex(nIndex));
		return InternalElement(nIndex).iPrevious;
	}

	[[nodiscard]] I Next(const I nIndex) const
	{
		Q_ASSERT(IsValidIndex(nIndex));
		return InternalElement(nIndex).iNext;
	}

	[[nodiscard]] static S InvalidIndex()
	{
		return static_cast<S>(M::InvalidIndex());
	}

	[[nodiscard]] bool IsValidIndex(const I nIndex) const
	{
		if (!memory.IsValidIndex(nIndex))
			return false;

		if (memory.IsIndexAfter(nIndex, itLastAlloc))
			return false; // don't read values that have been allocated, but not constructed

		return (memory[nIndex].iPrevious != nIndex) || (memory[nIndex].iNext == nIndex);
	}

	[[nodiscard]] static bool IsIndexInRange(I nIndex)
	{
		static_assert(sizeof(I) >= sizeof(S));
		static_assert(sizeof(S) > 2 || static_cast<S>(-1) > 0);
		static_assert(M::InvalidIndex() == -1 || M::InvalidIndex() == static_cast<S>(M::InvalidIndex()));

		return (static_cast<S>(nIndex) == nIndex && static_cast<S>(nIndex) != InvalidIndex());
	}

	[[nodiscard]] I Find(const T& source) const
	{
		for (I i = iHead; i != InvalidIndex(); i = Next(i))
		{
			if (Element(i) == source)
				return i;
		}

		return InvalidIndex();
	}

	void RemoveAll()
	{
		if (itLastAlloc == memory.InvalidIterator())
		{
			Q_ASSERT(iHead == InvalidIndex() && iTail == InvalidIndex() && iFirstFree == InvalidIndex() && nElementCount == 0);
			return;
		}

		if constexpr (ML)
		{
			for (typename M::Iterator_t it = memory.First(); it != memory.InvalidIterator(); it = memory.Next(it))
			{
				I i = memory.GetIndex(it);

				// skip elements already in the free list
				if (IsValidIndex(i))
				{
					ListElement_t& internalElement = InternalElement(i);
					(&internalElement.element)->~T();
					internalElement.iPrevious = i;
					internalElement.iNext = iFirstFree;
					iFirstFree = i;
				}

				// don't destruct elements that haven't ever been constructed
				if (it == itLastAlloc)
					break;
			}
		}
		else
		{
			I i = iHead, iNext;
			while (i != InvalidIndex())
			{
				ListElement_t& internalElement = InternalElement(i);
				(&internalElement.element)->~T();

				internalElement.iPrevious = i;
				iNext = Next(i);
				internalElement.iNext = iNext == InvalidIndex() ? iFirstFree : iNext;
				i = iNext;
			}

			if (iHead != InvalidIndex())
				iFirstFree = iHead;
		}

		// clear everything else out
		iHead = InvalidIndex();
		iTail = InvalidIndex();
		nElementCount = 0;
	}

	[[nodiscard]] auto begin() const
	{
		return ConstIterator_t<CUtlLinkedList<T, S, ML, I, M>>(*this, Head());
	}

	[[nodiscard]] auto begin()
	{
		return Iterator_t<CUtlLinkedList<T, S, ML, I, M>>(*this, Head());
	}

	[[nodiscard]] auto end() const
	{
		return ConstIterator_t<CUtlLinkedList<T, S, ML, I, M>>(*this, InvalidIndex());
	}

	[[nodiscard]] auto end()
	{
		return Iterator_t<CUtlLinkedList<T, S, ML, I, M>>(*this, InvalidIndex());
	}

protected:
	using ListElement_t = UtlLinkedListElement_t<T, S>;

	[[nodiscard]] ListElement_t& InternalElement(const I nIndex)
	{
		return memory[nIndex];
	}

	[[nodiscard]] const ListElement_t& InternalElement(const I nIndex) const
	{
		return memory[nIndex];
	}

	M memory;
	I iHead;
	I iTail;
	I iFirstFree;
	I nElementCount;
	I nAllocated;
	typename M::Iterator_t itLastAlloc;
	ListElement_t* pElements;
};

template <class T>
class CUtlFixedLinkedList : public CUtlLinkedList<T, std::intptr_t, true, std::intptr_t, CUtlFixedMemory<UtlLinkedListElement_t<T, std::intptr_t>>>
{
public:
	CUtlFixedLinkedList(int nGrowSize = 0, int nInitAllocationCount = 0) :
		CUtlLinkedList<T, std::intptr_t, true, std::intptr_t, CUtlFixedMemory<UtlLinkedListElement_t<T, std::intptr_t>>>(nGrowSize, nInitAllocationCount) { }

	[[nodiscard]] bool IsValidIndex(std::intptr_t nIndex) const
	{
		if (!this->memory.IsIndexValid(nIndex))
			return false;

		return (this->memory[nIndex].iPrevious != nIndex) || (this->memory[nIndex].iNext == nIndex);
	}
};
