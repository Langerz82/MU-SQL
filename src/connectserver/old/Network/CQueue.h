////////////////////////////////////////////////////////////////////////////////
// CQueue.h
#ifndef __CQUEUE_H__
#define __CQUEUE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
; // TODO - ?

struct ST_LISTNODE;


class  CQueue
{
public:
	CQueue(unsigned int nMaxNode = MAX_NODE);
	~CQueue();

	unsigned int	GetCount();
	unsigned int	GetCount_NoLock();
	BOOL			IsEmpty();
	BOOL			AddToQueue(const BYTE* pObject, unsigned int nSize, BYTE headcode, int uindex, int iSessionId);
	ST_LISTNODE*		AddToQueueList(const BYTE* pObject, unsigned int nSize, BYTE headcode, int uindex);
	BOOL			GetFromQueue(BYTE * pObject, unsigned int * pSize, BYTE * headcode, int * uindex, int * iSessionId);
	//BOOL			GetFromQueue(BYTE* pObject, unsigned int * pSize);
	void			Init();
	void			Free();
	
	BOOL        StartQ();
	BOOL		NextQ();
	ST_LISTNODE*	GetCurData(BYTE* pObject, unsigned int * pSize, BYTE * headcode, int * uindex);
	BOOL		Pop(ST_LISTNODE* pCur, BYTE *pObject, int nOfs, int *nSize, int *sentbytes);
	void		DeleteNode(ST_LISTNODE* pCur);
	void		SetCurDataSending(ST_LISTNODE* pNode);

private:
	
	ST_LISTNODE*	GetHeadNode();
    BOOL		AddTail(ST_LISTNODE* pNewNode);
	void		DeleteAllNodes();

	

	unsigned int		m_Count;
	unsigned int		m_Max_Node;
    ST_LISTNODE*		m_pHead;
	ST_LISTNODE*		m_pTail;
	ST_LISTNODE*		m_pCur;

	CRITICAL_SECTION	m_CriticalSection;
};

#endif


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

