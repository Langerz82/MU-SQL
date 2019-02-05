////////////////////////////////////////////////////////////////////////////////
// BattleGround.cpp
#include "stdafx.h"
#include "BattleGround.h"
// GS-N 0.99.60T 0x004596B0 Completed
// GS-N 1.00.18 0x00468DC0 - Completed

CBattleGround::CBattleGround()
{
	this->m_Used = FALSE;
	this->m_Set = FALSE;
}

CBattleGround::~CBattleGround()
{
	return;
}

void CBattleGround::SetGroundRect(int map, int x, int y, int xl, int yl)
{
	this->m_MapNumber = map;
	this->m_rect.left = x;
	this->m_rect.top = y;
	this->m_rect.right = xl;
	this->m_rect.bottom = yl;
	this->m_Set = true;
}

void CBattleGround::SetBattleType(int type)
{
	this->m_Type = type;
}

BOOL CBattleGround::GetUsed()
{
	return this->m_Used;
}

void CBattleGround::Enable(BOOL enable)
{
	this->m_Used = enable;

	if ( this->m_Used == TRUE)
	{
		this->m_Score1 = 0;
		this->m_Score2 = 0;
	}
}

void CBattleGround::SetTeamMaster(int n, char *szId)
{
	if ( n == 0 )
	{
		strcpy(this->m_szID1 , szId );
	}
	else
	{
		strcpy( this->m_szID2 , szId);
	}
}

void CBattleGround::SetTeamName(int n, char *szName)
{
	if ( n == 0 )
	{
		strcpy(this->m_szTeamName1 , szName );
	}
	else
	{
		strcpy( this->m_szTeamName2 , szName);
	}
}

char * CBattleGround::GetTeamName(int n)
{
	if ( n==0)
	{
		return this->m_szTeamName1;
	}
	else
	{
		return this->m_szTeamName2;
	}
}

int CBattleGround::GetScore(int team)
{
	if ( team == 0 )
	{
		return this->m_Score1;
	}
	else
	{
		return this->m_Score2;
	}
}

void CBattleGround::SetScore(int team, int score)
{
	if ( team == 0 )
	{
		this->m_Score1 = score;
	}
	else
	{
		this->m_Score2 = score;
	}
}

void CBattleGround::SetTimer(int g_time)
{
	this->m_nTimer = g_time;
	this->m_i64CurTimer = GetTickCount64();
}

int CBattleGround::GetTimer()
{
	return (GetTickCount64() - this->m_i64CurTimer) / 1000;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

