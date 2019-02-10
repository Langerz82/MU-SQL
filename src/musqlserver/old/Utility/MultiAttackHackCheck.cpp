////////////////////////////////////////////////////////////////////////////////
// MultiAttackHackCheck.cpp
//#include "StdAfx.h"
#include "MultiAttackHackCheck.h"
#include "SkillAdditionInfo.h"
#include "gObjMonster.h"
#include "User/CUserData.h"

// GS-N 0x00452273 - 0.99.60T
// GS-N	1.00.18	JPN	0x00461400	-	Completed

// sizeof ( this->class = 0x2858 )
CMultiAttackHackCheck * gMultiAttackHackCheck;

CMultiAttackHackCheck::CMultiAttackHackCheck()
{
	this->Init();
}


CMultiAttackHackCheck::~CMultiAttackHackCheck()
{
	return;
}

void CMultiAttackHackCheck::Init()
{
	this->m_iCount = 0;

	for ( int i=0;i<MAX_MULTI_ATTACK_INFO;i++)
	{
		this->m_Table[i].number = -1;
		memset(this->m_Table[i].Serial, 0, sizeof(this->m_Table[i].Serial));
	}
}



BOOL CMultiAttackHackCheck::Insert(LPGameObject &aTarget, BYTE skillnum, BYTE serial)
{
	if (aTarget->Type == OBJ_MONSTER )
	{
		return TRUE;
	}

	if ( skillnum == 12 )
	{
		return TRUE;
	}

	for ( int i=0;i<MAX_MULTI_ATTACK_INFO;i++)
	{
		if ( this->m_Table[i].number == aTarget->UserNumber)
		{
			if ( serial == (BYTE)-1  && this->m_Table[i].Serial[serial]  != 0 )
			{
				memset(this->m_Table[i].Serial, 0, MULTI_ATTACK_SERIAL_SIZE);
			}

			this->m_Table[i].Serial[serial]++;

			if ( skillnum == 9 )
			{
				if ( this->m_Table[i].Serial[serial] > 6 )
				{
					return FALSE;
				}
			}
			else if ( this->m_Table[i].Serial[serial] > 6 )
			{
				return FALSE;
			}
		}
	}

	this->m_Table[this->m_iCount].number = aTarget->UserNumber;
	this->m_Table[this->m_iCount].Serial[serial]++;
	this->m_iCount++;

	if ( this->m_iCount >= MAX_MULTI_ATTACK_INFO )
	{
		this->m_iCount = 0;
	}

	return TRUE;
}



BOOL CMultiAttackHackCheck::CheckPenetrationSkill(int aTargetIndex, BYTE skillnum, BYTE serial)
{
	if ( skillnum != 52 )
	{
		return TRUE;
	}

	if ( serial == 10 )
	{
		this->Init();
	}

	for ( int i=0;i<MAX_MULTI_ATTACK_INFO;i++)
	{
		if ( this->m_Table[i].number == aTargetIndex )
		{
			if ( serial == 10 && this->m_Table[i].Serial[serial] != 0 )
			{
				memset(this->m_Table[i].Serial , 0, MULTI_ATTACK_SERIAL_SIZE);
			}
			
			this->m_Table[i].Serial[serial]++;

			if ( this->m_Table[i].Serial[serial] > 1 )
			{
				return FALSE;
			}
		}
	}

	this->m_Table[this->m_iCount].number = aTargetIndex;
	this->m_Table[this->m_iCount].Serial[serial]++;
	this->m_iCount++;

	if ( this->m_iCount >= MAX_MULTI_ATTACK_INFO )
	{
		this->m_iCount = 0;
	}

	return TRUE;
}



BOOL CMultiAttackHackCheck::CheckFireScreamSkill(int iTargetIndex, BYTE btSkillNum, BYTE btSerial)
{
	if ( g_SkillAdditionInfo.GetCheckFireScreamSkill() == FALSE )
		return TRUE;

	if ( btSerial > g_SkillAdditionInfo.GetFireScreamSerialInitCount() )
		this->Init();

	for ( int iCount = 0 ; iCount < MAX_MULTI_ATTACK_INFO ; iCount++)
	{
		if ( this->m_Table[iCount].number == iTargetIndex )
		{
			this->m_Table[iCount].Serial[btSerial]+=1;

			if ( this->m_Table[iCount].Serial[btSerial] > g_SkillAdditionInfo.GetFireScreamMaxAttackCountSameSerial() )
			{
				return FALSE;
			}
		}
	}

	this->m_Table[this->m_iCount].number = iTargetIndex;
	this->m_Table[this->m_iCount].Serial[btSerial]+=1;
	this->m_iCount++;

	if ( this->m_iCount >= MAX_MULTI_ATTACK_INFO )
		this->m_iCount = 0;

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

