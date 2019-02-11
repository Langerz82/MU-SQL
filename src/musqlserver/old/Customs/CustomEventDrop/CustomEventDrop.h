// CustomEventDrop.h: interface for the CCustomEventDrop class.
//
//////////////////////////////////////////////////////////////////////
#ifndef _CUSTOM_EVENT_DROP_H
#define _CUSTOM_EVENT_DROP_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"



class CCustomEventDrop
{
public:
	CCustomEventDrop();
	virtual ~CCustomEventDrop();
	void Init();
	void Load(char* path);
	void MainProc();
	void ProcState_BLANK(CUSTOM_EVENT_DROP_INFO* lpInfo);
	void ProcState_EMPTY(CUSTOM_EVENT_DROP_INFO* lpInfo);
	void ProcState_START(CUSTOM_EVENT_DROP_INFO* lpInfo);
	void SetState(CUSTOM_EVENT_DROP_INFO* lpInfo, int state);
	void SetState_BLANK(CUSTOM_EVENT_DROP_INFO* lpInfo);
	void SetState_EMPTY(CUSTOM_EVENT_DROP_INFO* lpInfo);
	void SetState_START(CUSTOM_EVENT_DROP_INFO* lpInfo);
	void CheckSync(CUSTOM_EVENT_DROP_INFO* lpInfo);
	LONG GetDummyUserIndex();
	bool GetRandomItemDropLocation(int map, int* ox, int* oy, int tx, int ty, int count);
private:
	int m_CustomEventDropSwitch;
	char m_CustomEventDropText1[128];
	char m_CustomEventDropText2[128];
	char m_CustomEventDropText3[128];
	CUSTOM_EVENT_DROP_INFO m_CustomEventDropInfo[MAX_CUSTOM_EVENT_DROP];
};

extern CCustomEventDrop gCustomEventDrop;

#endif