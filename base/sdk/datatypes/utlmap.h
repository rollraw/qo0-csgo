#pragma once
#include "utlrbtree.h"

// @source: master/public/tier1/utlmap.h

template <typename K, typename T, typename I = unsigned short, typename LessCallbackFn_t = bool(Q_CDECL*)(const K&, const K&)>
class CUtlMap
{
public:
	using KeyType_t = K;
	using ElementType_t = T;
	using IndexType_t = I;

	CUtlMap(int nGrowSize = 0, int nInitialSize = 0, const LessCallbackFn_t& fnLessCallback = nullptr) :
		tree(nGrowSize, nInitialSize, CKeyLess(fnLessCallback)) { }

	CUtlMap(LessCallbackFn_t fnLessCallback) :
		tree(CKeyLess(fnLessCallback)) { }

	[[nodiscard]] ElementType_t& operator[](IndexType_t nIndex)
	{
		return tree.Element(nIndex).nElement;
	}

	[[nodiscard]] const ElementType_t& operator[](IndexType_t nIndex) const
	{
		return tree.Element(nIndex).nElement;
	}

	[[nodiscard]] ElementType_t& Element(IndexType_t nIndex)
	{
		return tree.Element(nIndex).nElement;
	}

	[[nodiscard]] const ElementType_t& Element(IndexType_t nIndex) const
	{
		return tree.Element(nIndex).nElement;
	}

	[[nodiscard]] KeyType_t& Key(IndexType_t nIndex)
	{
		return tree.Element(nIndex).key;
	}

	[[nodiscard]] const KeyType_t& Key(IndexType_t nIndex) const
	{
		return tree.Element(nIndex).key;
	}

	[[nodiscard]] unsigned int Count() const
	{
		return tree.Count();
	}

	[[nodiscard]] IndexType_t MaxElement() const
	{
		return tree.MaxElement();
	}

	[[nodiscard]] bool IsValidIndex(IndexType_t nIndex) const
	{
		return tree.IsValidIndex(nIndex);
	}

	[[nodiscard]] bool IsValid() const
	{
		return tree.IsValid();
	}

	[[nodiscard]] static IndexType_t InvalidIndex()
	{
		return CUtlRBTree<Node_t, I, CKeyLess>::InvalidIndex();
	}

	void EnsureCapacity(const int nCapacity)
	{
		tree.EnsureCapacity(nCapacity);
	}

	[[nodiscard]] IndexType_t Find(const KeyType_t& key) const
	{
		Node_t dummyNode;
		dummyNode.key = key;
		return tree.Find(dummyNode);
	}

	void RemoveAt(IndexType_t nIndex)
	{
		tree.RemoveAt(nIndex);
	}

	bool Remove(const KeyType_t& key)
	{
		Node_t dummyNode = { key };
		return tree.Remove(dummyNode);
	}

	void RemoveAll()
	{
		tree.RemoveAll();
	}

	void Purge()
	{
		tree.Purge();
	}

	struct Node_t
	{
		KeyType_t key;
		ElementType_t nElement;
	};

	class CKeyLess
	{
	public:
		CKeyLess(const LessCallbackFn_t& fnLessCallback) :
			fnLessCallback(fnLessCallback) { }

		bool operator!() const
		{
			return !fnLessCallback;
		}

		bool operator()(const Node_t& left, const Node_t& right) const
		{
			return fnLessCallback(left.key, right.key);
		}

		LessCallbackFn_t fnLessCallback;
	};

protected:
	CUtlRBTree<Node_t, I, CKeyLess> tree; // 0x00
};
