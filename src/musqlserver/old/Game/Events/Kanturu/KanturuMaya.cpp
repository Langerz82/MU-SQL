// KanturuMaya.cpp: implementation of the CKanturuMaya class.
//	GS-N	1.00.18	JPN	0x005785B0	-	Completed
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KanturuMaya.h"
#include "TMonsterSkillManager.h"
#include "Logging/Log.h"
#include "KanturuUtil.h"
#include "configread.h"

static CKanturuUtil KANTURU_UTIL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKanturuMaya::CKanturuMaya()
{
	this->Init();
}

CKanturuMaya::~CKanturuMaya()
{
	return;
}

void CKanturuMaya::Init()
{
	this->m_iMayaObjIndex = -1;
	this->m_iMayaSkillTime = 0;
	this->m_iIceStormCount = 0;
}

void CKanturuMaya::KanturuMayaAct_IceStorm(int iSkillUsingRate)
{
	if ( (rand()%10000) > iSkillUsingRate )
		return;

	if ( this->m_iMayaObjIndex < 0 || this->m_iMayaObjIndex > g_ConfigRead.server.GetObjectMax()-1 )	
		return;

	for ( int iCount= g_ConfigRead.server.GetObjectStartUserIndex();iCount < g_ConfigRead.server.GetObjectMax();iCount++)
	{
		if ( getGameObject(iCount)->Type == OBJ_USER &&
			 gObjIsConnected(iCount) &&
			 getGameObject(iCount)->MapNumber == MAP_INDEX_KANTURU_BOSS )
		{
			KANTURU_UTIL.NotifyKanturuWideAreaAttack(this->m_iMayaObjIndex, iCount, 0);
			TMonsterSkillManager::UseMonsterSkill(this->m_iMayaObjIndex, iCount, 31, -1, NULL);
			this->m_iIceStormCount++;
		}
	}

}



void CKanturuMaya::KanturuMayaAct_Hands()
{
	int TickCount = GetTickCount() - this->m_iMayaSkillTime;

	if ( TickCount < 20000 )
		return;

	this->m_iMayaSkillTime = GetTickCount();

	if ( this->m_iMayaObjIndex < 0 || this->m_iMayaObjIndex > g_ConfigRead.server.GetObjectMax()-1 )	// #errot change to ObjectMax-1
		return;

	for ( int iCount= g_ConfigRead.server.GetObjectStartUserIndex();iCount < g_ConfigRead.server.GetObjectMax();iCount++)
	{
		if (getGameObject(iCount)->Type == OBJ_USER &&
			gObjIsConnected(iCount) &&
			getGameObject(iCount)->MapNumber == MAP_INDEX_KANTURU_BOSS)
		{
			KANTURU_UTIL.NotifyKanturuWideAreaAttack(this->m_iMayaObjIndex, iCount, 1);
			TMonsterSkillManager::UseMonsterSkill(this->m_iMayaObjIndex, iCount, 1, -1, NULL);

			if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
			{
				if (getGameObject(iCount)->pntInventory[10)->m_Type == ITEMGET(13, 38) &&
					getGameObject(iCount)->pntInventory[10)->m_Durability != 0.0f)
				{
					continue;
				}

				if (getGameObject(iCount)->pntInventory[11)->m_Type == ITEMGET(13, 38) &&
					getGameObject(iCount)->pntInventory[11)->m_Durability != 0.0f)
				{
					continue;
				}

				if (getGameObject(iCount)->MapNumber == MAP_INDEX_KANTURU_BOSS)	// #warning unuseful if
				{
					CGameObject lpMayaHandObj = getGameObject(this->m_iMayaObjIndex);
					getGameObject(iCount)->Life = 0;

					gObjLifeCheck(getGameObject(iCount), lpMayaHandObj, getGameObject(iCount)->Life,
						0, 0, 0, 0, 0, 0);

				}
			}
		}
	}

}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

