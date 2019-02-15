// CrywolfUtil.cpp: implementation of the CCrywolfUtil class.
//	GS-N	1.00.18	JPN	0x0056F8E0	-	Completed
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CrywolfUtil.h"
#include "GameProtocol.h"
#include "MapServerManager.h"
#include "TNotice.h"
#include "classdef.h"
#include "Logging/Log.h"
#include "Gamemain.h"
#include "MasterLevelSkillTreeSystem.h"
#include "configread.h"
#include "CustomMaxStats.h"

CCrywolfUtil UTIL;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCrywolfUtil::CCrywolfUtil()
{
	return;
}

CCrywolfUtil::~CCrywolfUtil()
{
	return;
}

void CCrywolfUtil::SendMapServerGroupMsg(LPSTR lpszMsg, ...)
{
	if ( !lpszMsg )
		return;

	char szBuffer[512] = "";
	va_list	pArguments;

	va_start(pArguments, lpszMsg);
	vsprintf(szBuffer, lpszMsg, pArguments);
	va_end(pArguments);

	GS_GDReqMapSvrMsgMultiCast(g_MapServerManager.GetMapSvrGroup(), szBuffer);
}

void CCrywolfUtil::SendAllUserAnyData(BYTE* lpMsg, int iSize)
{
	for (int i = g_ConfigRead.server.GetObjectStartUserIndex(); i < g_ConfigRead.server.GetObjectMax(); i++)
	{
		if ( getGameObject(i)->Connected == PLAYER_PLAYING )
		{
			if ( getGameObject(i)->Type == OBJ_USER )
			{
				IOCP.DataSend(i, lpMsg, iSize);
			}
		}
	}
}


void CCrywolfUtil::SendAllUserAnyMsg(int iType, LPSTR lpszMsg, ...)
{
	if ( !lpszMsg )
		return;

	char szBuffer[512] = "";
	va_list	pArguments;

	va_start(pArguments, lpszMsg);
	vsprintf(szBuffer, lpszMsg, pArguments);
	va_end(pArguments);

	PMSG_NOTICE pNotice;

	switch ( iType )
	{
		case 1:
			TNotice::MakeNoticeMsg(&pNotice, 0, szBuffer);
			TNotice::SetNoticeProperty(&pNotice, 10, _ARGB(255, 255, 200, 80), 1, 0, 20);
			TNotice::SendNoticeToAllUser(&pNotice);
			break;

		case 2:
			TNotice::MakeNoticeMsg(&pNotice, 0, szBuffer);
			TNotice::SendNoticeToAllUser(&pNotice);
			break;
	}
}


void CCrywolfUtil::SendCrywolfUserAnyData(BYTE* lpMsg, int iSize)
{
	for ( int i=g_ConfigRead.server.GetObjectStartUserIndex();i<g_ConfigRead.server.GetObjectMax();i++)
	{
		if ( getGameObject(i)->Connected == PLAYER_PLAYING )
		{
			if ( getGameObject(i)->Type == OBJ_USER )
			{
				if ( getGameObject(i)->MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE )
				{
					IOCP.DataSend(i, lpMsg, iSize);
				}
			}
		}
	}
}


void CCrywolfUtil::SendCrywolfUserAnyMsg(int iType, LPSTR lpszMsg, ...)
{
	if ( !lpszMsg )
		return;

	char szBuffer[512] = "";
	va_list	pArguments;

	va_start(pArguments, lpszMsg);
	vsprintf(szBuffer, lpszMsg, pArguments);
	va_end(pArguments);

	PMSG_NOTICE pNotice;

	switch ( iType )
	{
		case 1:
			TNotice::MakeNoticeMsg(&pNotice, 0, szBuffer);
			TNotice::SetNoticeProperty(&pNotice, 10, _ARGB(255, 255, 200, 80), 1, 0, 20);
			break;

		case 2:
			TNotice::MakeNoticeMsg(&pNotice, 0, szBuffer);
			break;
	}

	for ( int i=g_ConfigRead.server.GetObjectStartUserIndex();i<g_ConfigRead.server.GetObjectMax();i++)
	{
		if ( getGameObject(i)->Connected == PLAYER_PLAYING )
		{
			if ( getGameObject(i)->Type == OBJ_USER )
			{
				if ( getGameObject(i)->MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE )
				{
					IOCP.DataSend(i, (BYTE*)&pNotice, pNotice.h.size);
				}
			}
		}
	}

}

#pragma warning ( disable : 4101 )
void CCrywolfUtil::SendCrywolfChattingMsg(int iObjIndex, LPSTR lpszMsg, ...)
{
	return;

	CGameObject lpObj;
	char szBuffer[512];
	va_list pArguments;
	char szChat[60];

	for (int i;i<MaxViewportMonster;i++)
	{
		if ( lpObj.VpPlayer2[i].state != FALSE )
		{
			int iTargetNumber = lpObj.VpPlayer2[i].number;

			if ( ObjectMaxRange(iTargetNumber) != FALSE )
			{
				gGameProtocol.ChatTargetSend(lpObj, szChat, iTargetNumber);
			}
		}
	}
}
#pragma warning ( default : 4101 )


int CCrywolfUtil::CrywolfMVPLevelUp(CGameObject &Obj, int iAddExp)
{
	if ( !ObjectMaxRange(iUserIndex ) )
		return 0;

	if(getGameObject(iUserIndex)->Type != OBJ_USER)
	{
		return 0;
	}

	int iLEFT_EXP = 0;

	sLog->outBasic("[ Crywolf ][MVP Exp.] : [%s][%s](%d) %u %d",
		getGameObject(iUserIndex)->AccountID, getGameObject(iUserIndex)->Name,
		getGameObject(iUserIndex)->Level, getGameObject(iUserIndex)->m_PlayerData->Experience,
		iAddExp);

	if ( g_MasterLevelSkillTreeSystem.IsMasterLevelUser(&getGameObject(iUserIndex)) == true )
	{
		return g_MasterLevelSkillTreeSystem.MasterLevelUp(&getGameObject(iUserIndex), iAddExp, 0, "Crywolf");
	}

	sLog->outBasic("[Crywolf] Experience : [%s][%s](%d) Experience: %d + %d",
		getGameObject(iUserIndex)->AccountID, getGameObject(iUserIndex)->Name,
		getGameObject(iUserIndex)->Level, getGameObject(iUserIndex)->m_PlayerData->Experience, iAddExp);

	gObjSetExpPetItem(iUserIndex, iAddExp);

	if ( getGameObject(iUserIndex)->Level >= g_ConfigRead.data.common.UserMaxLevel)
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0,45), getGameObject(iUserIndex)->m_Index, 1);
		return 0;
	}

	if ( (getGameObject(iUserIndex)->m_PlayerData->Experience + iAddExp) < getGameObject(iUserIndex)->m_PlayerData->NextExp )
	{
		getGameObject(iUserIndex)->m_PlayerData->Experience += iAddExp;
	}
	else
	{
		iLEFT_EXP = getGameObject(iUserIndex)->m_PlayerData->Experience + iAddExp - getGameObject(iUserIndex)->m_PlayerData->NextExp;
		getGameObject(iUserIndex)->m_PlayerData->Experience = getGameObject(iUserIndex)->m_PlayerData->NextExp;
		getGameObject(iUserIndex)->Level++;

		if ( g_ConfigRead.data.reset.iBlockLevelUpPointAfterResets == -1 || getGameObject(iUserIndex)->m_PlayerData->m_iResets < g_ConfigRead.data.reset.iBlockLevelUpPointAfterResets )
		{
			if (getGameObject(iUserIndex)->Class == CLASS_DARKLORD || getGameObject(iUserIndex)->Class == CLASS_MAGUMSA || getGameObject(iUserIndex)->Class == CLASS_RAGEFIGHTER || getGameObject(iUserIndex)->Class == CLASS_GROWLANCER)
			{
				getGameObject(iUserIndex)->m_PlayerData->LevelUpPoint += g_MaxStatsInfo.GetClass.LevelUpPointMGDL;
			}

			else
			{
				getGameObject(iUserIndex)->m_PlayerData->LevelUpPoint += g_MaxStatsInfo.GetClass.LevelUpPointNormal;
			}

			if (getGameObject(iUserIndex)->m_PlayerData->PlusStatQuestClear != false)
			{
				getGameObject(iUserIndex)->m_PlayerData->LevelUpPoint++;

				//sLog->outBasic("[ Crywolf ][MVP Exp.] [%s][%s] LevelUp PlusStatQuest Clear AddStat %d",
				//	getGameObject(iUserIndex)->AccountID, getGameObject(iUserIndex)->Name,getGameObject(iUserIndex)->m_PlayerData->LevelUpPoint);
			}

			if ( getGameObject(iUserIndex)->m_PlayerData->PlusStatQuestClear != false )
			{
			}
		}

		getGameObject(iUserIndex)->MaxLife += DCInfo.DefClass[getGameObject(iUserIndex)->Class)->LevelLife;
		getGameObject(iUserIndex)->MaxMana += DCInfo.DefClass[getGameObject(iUserIndex)->Class)->LevelMana;
		getGameObject(iUserIndex)->Life = getGameObject(iUserIndex)->MaxLife;
		getGameObject(iUserIndex)->Mana = getGameObject(iUserIndex)->MaxMana;
		gObjNextExpCal(&getGameObject(iUserIndex));
		gObjSetBP(iUserIndex);
		gGameProtocol.GCLevelUpMsgSend(getGameObject(iUserIndex)->m_Index, 1);
		gObjCalcMaxLifePower(getGameObject(iUserIndex)->m_Index);

		sLog->outBasic("Level Up [%s][%s][%d]", getGameObject(iUserIndex)->AccountID,
			getGameObject(iUserIndex)->Name, getGameObject(iUserIndex)->Level);
	}

	return iLEFT_EXP;
}
	





////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

