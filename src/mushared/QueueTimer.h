////////////////////////////////////////////////////////////////////////////////
// QueueTimer.h
#ifndef QUEUETIMER_H
#define QUEUETIMER_H

#include "TSync.h"

#pragma once

class CQueueTimer
{
public:
	CQueueTimer();
	virtual ~CQueueTimer();

	void RegCallBack(WAITORTIMERCALLBACK callback);
	void QueueTimerInit();
	
	void PushEventID(int eventID);
	void PopEventID();
	int GetEventID();

	int CreateTimer(int eventID, DWORD period, DWORD flags);
	void DeleteQueueTimer();

	void SyncLock() { this->m_Sync.Lock(); }
	void SyncUnlock() { this->m_Sync.Unlock(); }

private:

	WAITORTIMERCALLBACK m_callback;
	HANDLE m_hTimerQueue;
	std::stack<int> m_EventIdStack;
	TSync m_Sync;
};

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

