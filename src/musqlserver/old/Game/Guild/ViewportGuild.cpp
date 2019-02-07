////////////////////////////////////////////////////////////////////////////////
// ViewportGuild.cpp
// ------------------------------
// Decompiled by Deathway
// Date : 2007-05-09
// ------------------------------
// GS-N 0.99.60T 4964D0// Status Completed
//	GS-N	1.00.18	JPN	0x004B14E0	-	Completed
#include "StdAfx.h"
#include "ViewportGuild.h"



CViewportGuild::CViewportGuild()
{
	return;
}

CViewportGuild::~CViewportGuild()
{
	return;
}

void CViewportGuild::Init()
{
	memset(&this->m_GuildNumber , 0, sizeof(this->m_GuildNumber ) );
	this->m_GuildCount =0;
}

BOOL CViewportGuild::Add(int number, int TargetObj)
{
	int blank=-1;
	int n;

	if (number < 1)
	{
		return 0;
	}

	for (n=0;n<MAX_VIEWPORT;n++)
	{
		if (this->m_GuildNumber[n] > 0)
		{
			if (this->m_GuildNumber[n] == number)
			{
				return 0;
			}
		}
		else if (blank < 0)
		{
			blank=n;
		}
	}

	if (blank < 0)
	{
		return 0;
	}

	if (this->m_GuildNumber[blank] == 0)
	{
		this->m_GuildNumber[blank]=number;
		this->m_GuildCount ++;
		return 1;
	}
	return 0;
}

		

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

