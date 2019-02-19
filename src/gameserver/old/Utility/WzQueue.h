////////////////////////////////////////////////////////////////////////////////
// WzQueue.h
// ------------------------------
// Decompiled by Deathway
// Date : 2007-03-09
// ------------------------------
#ifndef WZQUEUE_H
#define WZQUEUE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"

class WZQueue
{
public:

	WZQueue(UINT nMaxNode);
	~WZQueue();

	UINT GetCount();
	//BOOL __thiscall IsEmpty();	// Never Used
	int AddToQueue(LPBYTE const pObject , UINT nSize, BYTE headcode, int uindex);
	LPLISTNODE AddToQueueList(LPBYTE pObject, UINT nSize, BYTE headcode, int uindex);
	int GetFromQueue(LPBYTE pObject, UINT* pSize, BYTE* headcode,  int* uindex);
	void Init();
	void Free();
	int StartQ();
	int NextQ();
	LPLISTNODE GetCurData(LPBYTE pObject, UINT* pSize, BYTE * headcode, int* uindex);
	int Pop( LPLISTNODE pCur, LPBYTE pObject, int nOfs, int* nSize, int* sentbytes);
	void DeleteNode( LPLISTNODE pCur);
	void SetCurDataSending( LPLISTNODE pNode);

private:

	LPLISTNODE GetHeadNode();
	int AddTail( LPLISTNODE pNewNode);
	void DeleteAllNodes();

private:

	UINT m_Count;	// 0
	UINT m_Max_Node;	// 4
	LPLISTNODE m_pHead;	// 8
	LPLISTNODE m_pTail;	// C
	LPLISTNODE m_pCur;	// 10
	CRITICAL_SECTION m_CriticalSection;	// 14

/*	UINT m_QueueCount; // unk0
	int m_MaxNode; // unk4
	LPWZNODE m_lpWzNode; // unk8
	LPWZNODE m_lpWzNode2; // unkC
	LPWZNODE m_lpStartQ; // unk10
	CRITICAL_SECTION mWzqcriti;*/
};

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

