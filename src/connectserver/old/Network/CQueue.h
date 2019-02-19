////////////////////////////////////////////////////////////////////////////////
// CQueue.h
#ifndef __CQUEUE_H__
#define __CQUEUE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "generalStructs.h"

struct _LISTNODE;

class  CQueue
{
public:
	CQueue(unsigned int nMaxNode = MAX_NODE);
	~CQueue();

	unsigned int	GetCount();
	unsigned int	GetCount_NoLock();
	BOOL			IsEmpty();
	BOOL			AddToQueue(const BYTE* pObject, unsigned int nSize, BYTE headcode, int uindex, int iSessionId);
	_LISTNODE*		AddToQueueList(const BYTE* pObject, unsigned int nSize, BYTE headcode, int uindex);
	BOOL			GetFromQueue(BYTE * pObject, unsigned int * pSize, BYTE * headcode, int * uindex, int * iSessionId);
	//BOOL			GetFromQueue(BYTE* pObject, unsigned int * pSize);
	void			Init();
	void			Free();
	
	BOOL        StartQ();
	BOOL		NextQ();
	_LISTNODE*	GetCurData(BYTE* pObject, unsigned int * pSize, BYTE * headcode, int * uindex);
	BOOL		Pop(_LISTNODE* pCur, BYTE *pObject, int nOfs, int *nSize, int *sentbytes);
	void		DeleteNode(_LISTNODE* pCur);
	void		SetCurDataSending(_LISTNODE* pNode);

private:
	
	_LISTNODE*	GetHeadNode();
    BOOL		AddTail(_LISTNODE* pNewNode);
	void		DeleteAllNodes();

	

	unsigned int		m_Count;
	unsigned int		m_Max_Node;
    _LISTNODE*		m_pHead;
	_LISTNODE*		m_pTail;
	_LISTNODE*		m_pCur;

	CRITICAL_SECTION	m_CriticalSection;
};

#endif


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

