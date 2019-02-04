// ------------------------------
// Decompiled by Deathway
// Date : 2007-03-09
// ------------------------------
// GS-N 0.99.60T 0x00524310 Status : Completed
//	GS-N	1.00.18	JPN	0x00596140	-	Completed
// TSync.cpp

#include "stdafx.h"
#include "TSync.h"

TSync::TSync()
{
	this->m_nLock=0;
	InitializeCriticalSection(&this->m_cs);
}

TSync::~TSync()
{
	DeleteCriticalSection(&this->m_cs);
}

void TSync::Lock()
{
	EnterCriticalSection(&this->m_cs);
	InterlockedIncrement(&this->m_nLock);
}

void TSync::Unlock()
{
	long nResult=InterlockedDecrement(&this->m_nLock );
	LeaveCriticalSection(&this->m_cs );
}





////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

