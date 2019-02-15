#include "stdafx.h"
#include "CQueue.h"

CQueue::CQueue(unsigned int nMaxNode)
{
	m_Max_Node = nMaxNode;
	m_pHead    = m_pTail = NULL;
	m_Count    = 0;

	BOOL bSuccess =	InitializeCriticalSectionAndSpinCount(&m_CriticalSection, SPIN_COUNT);
	if(!bSuccess)
	{
		if( GetLastError() == STATUS_NO_MEMORY )
			InitializeCriticalSection(&m_CriticalSection);
	}
}

CQueue::~CQueue()
{
	DeleteAllNodes();
	DeleteCriticalSection(&m_CriticalSection);
}

unsigned int CQueue::GetCount()
{
	EnterCriticalSection(&m_CriticalSection);
	DWORD nNum = m_Count;
	LeaveCriticalSection(&m_CriticalSection);
	return nNum;
}

unsigned int CQueue::GetCount_NoLock()
{
	return this->m_Count;
}

BOOL CQueue::AddTail(ST_LISTNODE* pNewNode)
{
	EnterCriticalSection(&m_CriticalSection);
	BOOL bRet = TRUE;
	if( m_Count < MAX_NODE )
	{
		// crosslink two nodes.
		pNewNode->pUpLink	= m_pTail;
		pNewNode->pDownLink = NULL;

		// check headnode.
		if(m_pHead == NULL)
			m_pHead = pNewNode;
		else
			m_pTail->pDownLink = pNewNode;

		m_pTail = pNewNode;

		m_Count++;
	}
	else 
		bRet = FALSE;
	LeaveCriticalSection(&m_CriticalSection);
	return bRet;
}

ST_LISTNODE* CQueue::GetHeadNode()
{
	EnterCriticalSection(&m_CriticalSection);	
	ST_LISTNODE* pNode = NULL;

	if(m_pHead)
	{
		pNode = m_pHead;
		if(m_pHead->pDownLink)
		{
			m_pHead->pDownLink->pUpLink = NULL;
			m_pHead = m_pHead->pDownLink;
		}
		else
		{
			m_pHead = m_pTail = NULL;
		}

		m_Count--;
	}	
	LeaveCriticalSection(&m_CriticalSection);
	return pNode;
}

BOOL CQueue::AddToQueue(const BYTE* pObject, unsigned int nSize, BYTE headcode, int uindex, int iSessionId)
{
	BOOL bRet = FALSE;

	////ASSERT( pObject );	
	if( nSize < 1 || nSize > 65536 ) 
	{
		return FALSE;
	}

	EnterCriticalSection(&m_CriticalSection);
	ST_LISTNODE* pNewNode = (ST_LISTNODE*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(ST_LISTNODE));
	
	if( pNewNode )
	{
		BYTE* pMsg = (BYTE*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nSize);
		if( pMsg )
		{
			std::memcpy(pMsg, pObject, nSize);

			pNewNode->pObject	= pMsg;
			pNewNode->nSize		= nSize;
			pNewNode->headcode	= headcode;
			pNewNode->uindex	= uindex;
			pNewNode->bSending	= 0;
			pNewNode->nOfs		= 0;
			pNewNode->iSessionId = iSessionId; //new
			if( AddTail(pNewNode) ) bRet = TRUE;
		}
		else
		{
			HeapFree(GetProcessHeap(), 0, pMsg);
			HeapFree(GetProcessHeap(), 0, pNewNode);
		}
	}
	else
		HeapFree(GetProcessHeap(), 0, pNewNode);
	
	LeaveCriticalSection(&m_CriticalSection);
	return bRet;
}

ST_LISTNODE* CQueue::AddToQueueList(const BYTE* pObject, unsigned int nSize, BYTE headcode, int uindex)
{
	BOOL bRet = FALSE;

	////ASSERT( pObject );
	if( nSize < 1 || nSize > 65536 ) 
	{
		return NULL;
	}

	EnterCriticalSection(&m_CriticalSection);
	ST_LISTNODE* pNewNode = (ST_LISTNODE*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(ST_LISTNODE));
	
	if( pNewNode )
	{
		BYTE* pMsg = (BYTE*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nSize);
		if( pMsg )
		{
			std::memcpy(pMsg, pObject, nSize);

			pNewNode->pObject	= pMsg;
			pNewNode->nSize		= nSize;
			pNewNode->headcode	= headcode;
			pNewNode->uindex	= uindex;
			pNewNode->bSending	= 0;
			pNewNode->nOfs		= 0;
			if( AddTail(pNewNode) ) bRet = TRUE;
		}
		else
		{
			HeapFree(GetProcessHeap(), 0, pMsg);
			HeapFree(GetProcessHeap(), 0, pNewNode);
		}
	}
	else
		HeapFree(GetProcessHeap(), 0, pNewNode);
	
	LeaveCriticalSection(&m_CriticalSection);
	return pNewNode;
}

BOOL CQueue::GetFromQueue(BYTE* pObject, unsigned int * pSize, BYTE * headcode, int * uindex, int * iSessionId)
{
	ST_LISTNODE*	pNode = NULL;
	////ASSERT( pObject );
	
	EnterCriticalSection(&m_CriticalSection); //wz removed criti for some reason hmm...
	pNode = GetHeadNode();
	if(pNode)
	{
		
		std::memcpy(pObject, pNode->pObject, pNode->nSize);
		*pSize = pNode->nSize;
		*headcode =pNode->headcode;
		*uindex = pNode->uindex;
		*iSessionId = pNode->iSessionId; //new
		
		HeapFree(GetProcessHeap(), 0, pNode->pObject);
		HeapFree(GetProcessHeap(), 0, pNode);
		LeaveCriticalSection(&m_CriticalSection);
		return TRUE;
	}

	LeaveCriticalSection(&m_CriticalSection);
	return FALSE;
}

void CQueue::DeleteAllNodes()
{
	ST_LISTNODE*	pNode = NULL;

	EnterCriticalSection(&m_CriticalSection);
	// DO NOT DELETE Object that pointed by pObject!!!!
	while(pNode = GetHeadNode())
	{
		HeapFree(GetProcessHeap(), 0, pNode);
	}
	LeaveCriticalSection(&m_CriticalSection);
}

BOOL CQueue::StartQ()
{
	if( m_pHead )
	{
		EnterCriticalSection(&m_CriticalSection);
		m_pCur = m_pHead;
		LeaveCriticalSection(&m_CriticalSection);
		return TRUE;
	}
	return FALSE;
}

BOOL CQueue::NextQ()
{
	if( m_pCur )
	{
		if( m_pCur->pDownLink != NULL )
		{
			EnterCriticalSection(&m_CriticalSection);
			m_pCur = m_pCur->pDownLink;
			LeaveCriticalSection(&m_CriticalSection);
			return TRUE;
		}
	}
	return FALSE;
}

ST_LISTNODE* CQueue::GetCurData(BYTE* pObject, unsigned int * pSize, BYTE * headcode, int * uindex)
{
	if( m_pCur )
	{		
		if( m_pCur->bSending == 1 ) return FALSE;

		std::memcpy(pObject, m_pCur->pObject, m_pCur->nSize);
		*pSize = m_pCur->nSize;
		*headcode =m_pCur->headcode;
		*uindex = m_pCur->uindex;
		return m_pCur;
	}
	return NULL;
}

BOOL CQueue::Pop(ST_LISTNODE* pCur, BYTE *pObject, int nOfs, int *nSize, int *sentbytes)
{
	BOOL bRet=FALSE;

	if( pCur )
	{
		__try
		{
			EnterCriticalSection(&m_CriticalSection);
			pCur->nOfs += nOfs;
			if( pCur->nSize-nOfs )
			{
				std::memcpy(pObject, pCur->pObject+pCur->nOfs, pCur->nSize-pCur->nOfs);
				*nSize = pCur->nSize-pCur->nOfs;
				*sentbytes = pCur->nOfs;
				bRet = TRUE;
				__leave;
			}
		}
		__finally
		{
			LeaveCriticalSection(&m_CriticalSection);
		}
	}
	return bRet;
}

void CQueue::SetCurDataSending(ST_LISTNODE* pNode)
{	
	if( pNode )
	{
		EnterCriticalSection(&m_CriticalSection);
		pNode->bSending = 1;
		LeaveCriticalSection(&m_CriticalSection);
	}
}

void CQueue::DeleteNode(ST_LISTNODE* pCur)
{
	ST_LISTNODE* pNode = NULL;
	ST_LISTNODE* pPre = NULL;
	ST_LISTNODE* pNext = NULL;
	EnterCriticalSection(&m_CriticalSection);
	
	if( pCur )
	{
		pNode = pCur;
		pPre  = pCur->pUpLink;
		pNext = pCur->pDownLink;

		if( pPre == NULL && pNext == NULL )
		{
			m_pHead = pCur;
		}
		else if( pPre == NULL && pNext != NULL )
		{
			pNext->pUpLink = NULL;
			pCur = pNext;
			m_pHead = pCur;
			if( pNext->pDownLink == NULL ) {
				m_pTail = pNext;
				m_pTail->pDownLink = NULL;
			}
		}
		else if( pPre != NULL && pNext == NULL )
		{
			pPre->pDownLink = NULL;
			pCur = pPre;
			m_pTail = pCur;
			if( pPre->pUpLink == NULL ) {
				m_pHead = pPre;
				m_pHead->pUpLink = NULL;
			}
		}		
		else if( pPre && pNext )
		{
			pPre->pDownLink = pNext;
			pNext->pUpLink  = pPre;
			if( pPre->pUpLink == NULL ) {
				m_pHead = pPre;
				m_pHead->pUpLink = NULL;
			}
			if( pNext->pDownLink == NULL ) {
				m_pTail = pNext;
				m_pTail->pDownLink = NULL;
			}
		}
		HeapFree(GetProcessHeap(), 0, pNode->pObject);
		HeapFree(GetProcessHeap(), 0, pNode);
		if( pPre == NULL && pNext == NULL ) {
			m_pHead = NULL;
			pCur    = NULL;
			m_pTail = NULL;
		}
		m_Count--;
	}
	LeaveCriticalSection(&m_CriticalSection);
}
