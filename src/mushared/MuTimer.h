////////////////////////////////////////////////////////////////////////////////
// MuTimer.h
#ifndef MUTIMER_H
#define MUTIMER_H

#include "QueueTimer.h"

#pragma once

class CMuTimer
{
public:
	friend VOID WINAPI TimerProcQueue(LPVOID lpParameter, BOOL TimerOrWaitFired);
	friend VOID WINAPI TimerProcQueue2(LPVOID lpParameter, BOOL TimerOrWaitFired);
	CMuTimer();
	virtual ~CMuTimer();

	void SetMuTimer();

	void SetTimerEnd() { this->m_bTimerExpire = TRUE; }
	int GetTimerEnd() { return this->m_bTimerExpire; }

private:
	CQueueTimer m_MuTimer1;
	CQueueTimer m_MuTimer2;
	int m_bTimerExpire;
};

VOID WINAPI TimerProcQueue(LPVOID lpParameter, BOOL TimerOrWaitFired);
VOID WINAPI TimerProcQueue2(LPVOID lpParameter, BOOL TimerOfWaitFired);

extern CMuTimer g_MuTimer;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

