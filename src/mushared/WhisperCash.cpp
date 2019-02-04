////////////////////////////////////////////////////////////////////////////////
// WhisperCash.cpp
// ------------------------------
// Decompiled by Deathway
// Date : 2007-03-09
// ------------------------------

// GS-N 0.99.60T Status Completed
//	GS-N	1.00.18	JPN	0x004A5610	-	Completed

#include "stdafx.h"
#include "WhisperCash.h"
#include "user.h"

CWhisperCash::CWhisperCash()
{
	return;
}

CWhisperCash::~CWhisperCash()
{
	return;
}

int CWhisperCash::AddCash(char* id)
{
	int count =0;
	int maxcount = 2;
	int result;

	result=this->SearchCash(id);

	if (result != -1)
	{
		if (gObjUserIdConnectCheck(id,result) == 0)
		{
			return -1;
		}
		return result;
	}

	result=gObjGetIndex(id);

	if (result == -1)
	{
		return -1;
	}

	count=this->WChashCount ;

	while ( maxcount-- != 0)
	{
		count++;

		if (count > 1)
		{
			count=0;
		}

		if ( this->WChash[count].Live == 0 )
		{
			memcpy(&this->WChash[count].szId[0], id, 10 );
			this->WChash[count].Live = 1;
			this->WChash[count].m_Index = result;
			this->WChash[count].m_Time = GetTickCount();
			this->WChashCount++;

			if ( this->WChashCount > 1)
			{
				this->WChashCount = 0;
			}
			return result;
		}
	}
	return result;
}

void CWhisperCash::DelCash(char* id)
{
	int n = 0;

	for ( n=0; n<2;n++)
	{
		if ( this->WChash[n].Live == 1 )
		{
			if ( this->WChash[n].szId[0] == id[0] )
			{
				if ( strcmp(&this->WChash[n].szId[0], id) == 0)
				{
					this->WChash[n].Live = 0;
					return;
				}
			}
		}
	}
}

int CWhisperCash::SearchCash(char* id)
{
	int n = 0;

	for ( n=0; n<2;n++)
	{
		if ( this->WChash[n].Live == 1 )
		{
			if ( this->WChash[n].szId[0] == id[0] )
			{
				if ( strcmp(&this->WChash[n].szId[0], id) == 0)
				{
					this->WChash[n].m_Time = GetTickCount();
					return this->WChash[n].m_Index ;
				}
			}
		}
	}
	
	return -1;
}

void CWhisperCash::TimeCheckCashDelete()
{
	int n = 0;
	unsigned long CurTime = GetTickCount();

	for ( n=0;n<2;n++)
	{
		if ( this->WChash[n].Live == 1 )
		{
			if ( (CurTime-this->WChash[n].m_Time ) > 60000)
			{
				this->WChash[n].Live=0;
			}
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

