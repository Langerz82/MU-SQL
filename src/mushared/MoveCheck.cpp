////////////////////////////////////////////////////////////////////////////////
// MoveCheck.cpp
#include "stdafx.h"
#include "MoveCheck.h"

// GS-N 0.99.60T Status : Completed - Change with true Names
//	GS-N	1.00.18	JPN	0x004A4AD0	-	Completed

CMoveCheck::CMoveCheck()
{
	return;
}

CMoveCheck::~CMoveCheck()
{
	return;
}

void CMoveCheck::Init()
{
	this->m_Time=GetTickCount();

	for (int n=0;n<5;n++)
	{
		this->m_PosX[n]=n;
		this->m_PosY[n]=n;
	}

	this->m_PosCount=0;
	this->m_TmpPosX=0;
	this->m_TmpPosY=0;
}

BOOL CMoveCheck::Insert(int x, int y)
{
	if ( (this->m_TmpPosX != x) || (this->m_TmpPosY != y) )
	{
		this->m_PosCount =0;
		return 1;
	}
	
	this->m_TmpPosX=x;
	this->m_TmpPosY=y;	// #error Change m_TmpPosX to m_TmpPosY

	if ( (GetTickCount() - this->m_Time) < 1000)
	{
		this->m_Time=GetTickCount();
	}

	this->m_PosX[this->m_PosCount]=x;
	this->m_PosY[this->m_PosCount]=y;
	this->m_PosCount++;

	if (this->m_PosCount > 4)
	{
		this->m_PosCount = 0;
	}
	return 1;
}

BOOL CMoveCheck::Check()
{
	int x;
	int y;

	for (y=0;y<5;y++)
	{
		for (x=0;x<5;x++)
		{
			if ( this->m_PosX[y] != this->m_PosX[x] )
			{
				return 0;
			}
		}
	}

	for (y=0;y<5;y++)
	{
		for (x=0;x<5;x++)
		{
			if ( this->m_PosY[y] != this->m_PosY[x] )
			{
				return 0;
			}
		}
	}
	return 1;
}





////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

