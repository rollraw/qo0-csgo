#pragma once

template <class T>
struct CUtlReference
{
	CUtlReference* pNext;
	CUtlReference* pPrevious;
	T* pObject;
};

template <class T>
struct CUtlIntrusiveList
{
	T* pHead;
};

template <class T>
struct CUtlIntrusiveDList : public CUtlIntrusiveList<T>
{

};

template <class T>
struct CUtlReferenceList : public CUtlIntrusiveDList<CUtlReference<T>>
{

};
