////////////////////////////////////////////////////////////////////////////////
// QueueTimer.cpp
#include "StdAfx.h"
#include "QueueTimer.h"

CQueueTimer::CQueueTimer()
{
	this->m_callback = NULL;
	this->QueueTimerInit();
}

CQueueTimer::~CQueueTimer()
{
	if (this->m_hTimerQueue != INVALID_HANDLE_VALUE)
	{
		this->DeleteQueueTimer();
	}

	while (!this->m_EventIdStack.empty())
	{
		this->m_EventIdStack.pop();
	}
}

void CQueueTimer::RegCallBack(WAITORTIMERCALLBACK callback)
{
	this->m_callback = callback;
}

void CQueueTimer::QueueTimerInit()
{
	this->m_hTimerQueue = CreateTimerQueue();
}

void CQueueTimer::PushEventID(int eventID)
{
	this->m_EventIdStack.push(eventID);
}

void CQueueTimer::PopEventID()
{
	this->m_EventIdStack.pop();
}

int CQueueTimer::GetEventID()
{
	return this->m_EventIdStack.top();
}

int CQueueTimer::CreateTimer(int eventID, DWORD period, DWORD flags)
{
	HANDLE hTimer = INVALID_HANDLE_VALUE;
	this->PushEventID(eventID);
	int tmpEventID = this->GetEventID();

	int result = CreateTimerQueueTimer(&hTimer, this->m_hTimerQueue, this->m_callback, (PVOID)tmpEventID, 1000, period, flags);

	if (result == 0)
	{
		this->PopEventID();
	}

	return result;
}

void CQueueTimer::DeleteQueueTimer()
{
	DeleteTimerQueue(this->m_hTimerQueue);
	this->m_hTimerQueue = INVALID_HANDLE_VALUE;
}




////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

