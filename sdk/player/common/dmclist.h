//dmCList.h
#ifndef DMCLIST_H
#define DMCLIST_H
#pragma warning (disable:4786)


typedef unsigned long dmPOSITION;

template <class TYPE>
class dmCList
{
protected:

	struct CNode
	{
		CNode* pNext;
		CNode* pPrev;
		TYPE value;
	};

	CNode* m_pHead;
	CNode* m_pTail;
	int m_nCount;

	CNode* NewNode();
	void FreeNode(CNode*);

public:
	dmCList();
	dmCList(const dmCList& list);
	~dmCList();
	dmPOSITION AddHead(const TYPE &t);
	dmPOSITION AddTail(const TYPE &t);
	void RemoveAll();

	dmPOSITION GetHeadPosition() const;
	dmPOSITION GetTailPosition() const;
	TYPE& GetNext(dmPOSITION& pos) const;
	TYPE& GetPrev(dmPOSITION& pos) const;

	TYPE& GetAt(dmPOSITION pos) const;
	void SetAt(dmPOSITION pos,TYPE t);
	void RemoveAt(dmPOSITION pos);

	int GetCount() const { return m_nCount; }
	bool IsEmpty() const;
	dmPOSITION Find(const TYPE &t) const;
	int PosToIndex(dmPOSITION pos) const;
	dmPOSITION IndexToPos(int nIndex) const;

	const dmCList& operator=(const dmCList& list);
	const dmCList& operator+=(const dmCList& list);
	bool operator==(const dmCList& list);
};

template <class TYPE>
typename dmCList<TYPE>::CNode* 
dmCList<TYPE>::NewNode()
{
	CNode* pNode=new CNode;
	if (pNode) m_nCount++;
	return pNode;
}

template <class TYPE>
void dmCList<TYPE>::FreeNode(typename dmCList::CNode* pNode)
{
	delete pNode;
	m_nCount--;
}

template <class TYPE>
dmCList<TYPE>::dmCList()
{
	m_pHead=m_pTail=NULL;
	m_nCount=0;
}

template <class TYPE>
dmCList<TYPE>::dmCList(const dmCList& list)
{
	m_pHead=m_pTail=NULL;
	m_nCount=0;
	operator=(list);
}

template <class TYPE>
dmCList<TYPE>::~dmCList()
{
	RemoveAll();
}

template <class TYPE>
dmPOSITION dmCList<TYPE>::AddHead(const TYPE &t)
{
	CNode* pNode=NewNode();
	if (NULL==pNode) return NULL;
	pNode->value=t;
	pNode->pPrev=NULL;
	pNode->pNext=m_pHead;
	if (NULL==m_pHead)
		m_pTail=pNode;
	else
		m_pHead->pPrev=pNode;
	m_pHead=pNode;
	return (dmPOSITION)pNode;
}

template <class TYPE>
dmPOSITION dmCList<TYPE>::AddTail(const TYPE &t)
{
	CNode* pNode=NewNode();
	if (NULL==pNode) return NULL;
	pNode->value=t;
	pNode->pPrev=m_pTail;
	pNode->pNext=NULL;
	if (NULL==m_pTail)
		m_pHead=pNode;
	else
		m_pTail->pNext=pNode;
	m_pTail=pNode;
	return (dmPOSITION)pNode;

}

template <class TYPE>
void dmCList<TYPE>::RemoveAll()
{
	while (NULL!=m_pHead) {
		CNode* pNode=m_pHead;
		m_pHead=m_pHead->pNext;
		FreeNode(pNode);
	}
	m_pHead=m_pTail=NULL;
	m_nCount=0;
}

template <class TYPE>
dmPOSITION dmCList<TYPE>::GetHeadPosition() const
{
	return (dmPOSITION)m_pHead;
}

template <class TYPE>
dmPOSITION dmCList<TYPE>::GetTailPosition() const
{
	return (dmPOSITION)m_pTail;
}

template <class TYPE>
TYPE& dmCList<TYPE>::GetNext(dmPOSITION& pos) const
{
	CNode* pNode=(CNode*)pos;
	pos=(dmPOSITION)pNode->pNext;
	return pNode->value;
}

template <class TYPE>
TYPE& dmCList<TYPE>::GetPrev(dmPOSITION& pos) const
{
	CNode* pNode=(CNode*)pos;
	pos=(dmPOSITION)pNode->pPrev;
	return pNode->value;
}

template <class TYPE>
TYPE& dmCList<TYPE>::GetAt(dmPOSITION pos) const
{
	return ((CNode*)pos)->value;
}

template <class TYPE>
void dmCList<TYPE>::SetAt(dmPOSITION pos,TYPE t)
{
	((CNode*)pos)->value=t;
}

template <class TYPE>
void dmCList<TYPE>::RemoveAt(dmPOSITION pos)
{
	if (NULL==pos) return;
	CNode* pNode=(CNode*)pos;
	if (pNode==m_pHead) {
		m_pHead=pNode->pNext;
	} else {
		pNode->pPrev->pNext = pNode->pNext;
	}
	if (pNode==m_pTail) {
		m_pTail=pNode->pPrev;
	} else {
		pNode->pNext->pPrev = pNode->pPrev;
	}
	FreeNode(pNode);
}

template <class TYPE>
bool dmCList<TYPE>::IsEmpty() const
{
	DM_ASSERT( 0==m_nCount ? (NULL==m_pHead && NULL==m_pTail) : (NULL!=m_pHead && NULL!=m_pTail) );
	return 0==m_nCount;
}

template <class TYPE>
dmPOSITION dmCList<TYPE>::Find(const TYPE &t) const
{
	dmPOSITION savepos=NULL;
	dmPOSITION pos=GetHeadPosition();
	while (pos) {
		savepos=pos;
		if (t==GetNext(pos))
			return savepos;
	}
	return NULL;
}

template <class TYPE>
int dmCList<TYPE>::PosToIndex(dmPOSITION pos) const
{
	int nIndex=0;
	dmPOSITION lpos=GetHeadPosition();
	while (lpos) {
		if (pos==lpos) return nIndex;
		nIndex++;
		GetNext(lpos);
	}
	return -1;
}

template <class TYPE>
dmPOSITION dmCList<TYPE>::IndexToPos(int nIndex) const
{
	if (nIndex<0) return NULL;
	dmPOSITION pos=GetHeadPosition();
	while (pos) {
		if ( 0 == nIndex-- ) return pos;
		GetNext(pos);
	}
	return NULL;
}

template <class TYPE>
const dmCList<TYPE>& dmCList<TYPE>::operator=(const dmCList& list)
{
	RemoveAll();
	dmPOSITION pos=list.GetHeadPosition();
	while (pos) AddTail(list.GetNext(pos));
	return *this;
}

template <class TYPE>
const dmCList<TYPE>& dmCList<TYPE>::operator+=(const dmCList& list)
{
	dmPOSITION pos=list.GetHeadPosition();
	while (pos) AddTail(list.GetNext(pos));
	return *this;
}

template <class TYPE>
bool dmCList<TYPE>::operator==(const dmCList& list)
{
	if (GetCount()!=list.GetCount()) return false;
	dmPOSITION pos=GetHeadPosition();
	dmPOSITION pos2=list.GetHeadPosition();
	while (pos) {
		if (GetNext(pos)!=list.GetNext(pos2)) return false;
	}
	return true;
}


//sort dmlist, elem1 and elem2 should use as: TYPE*
template <class TYPE>
bool dmSort( dmCList<TYPE>& list, int comp(const void *elem1, const void *elem2 ) )
{
	int nItemNum=list.GetCount();
	TYPE* pItem=new TYPE[nItemNum];
	if (NULL==pItem) return false;
	
	int i=0;
	dmPOSITION pos=list.GetHeadPosition();
	while (pos) pItem[i++]=list.GetNext(pos);

	qsort(pItem,nItemNum,sizeof(TYPE*),comp);

	pos=list.GetHeadPosition();
	for (i=0;i<nItemNum;i++) {
		list.SetAt(pos,pItem[i]);
		list.GetNext(pos);
	}

	delete [] pItem;
	return true;
}

#endif
