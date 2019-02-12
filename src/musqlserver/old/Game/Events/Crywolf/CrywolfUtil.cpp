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

void CCrywolfUtil::SendAllUserAnyData(LPBYTE lpMsg, int iSize)
{
	for (int i = g_ConfigRead.server.GetObjectStartUserIndex(); i < g_ConfigRead.server.GetObjectMax(); i++)
	{
		if ( gGameObjects[i]->Connected == PLAYER_PLAYING )
		{
			if ( gGameObjects[i]->Type == OBJ_USER )
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


void CCrywolfUtil::SendCrywolfUserAnyData(LPBYTE lpMsg, int iSize)
{
	for ( int i=g_ConfigRead.server.GetObjectStartUserIndex();i<g_ConfigRead.server.GetObjectMax();i++)
	{
		if ( gGameObjects[i]->Connected == PLAYER_PLAYING )
		{
			if ( gGameObjects[i]->Type == OBJ_USER )
			{
				if ( gGameObjects[i]->MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE )
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
		if ( gGameObjects[i]->Connected == PLAYER_PLAYING )
		{
			if ( gGameObjects[i]->Type == OBJ_USER )
			{
				if ( gGameObjects[i]->MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE )
				{
					IOCP.DataSend(i, (LPBYTE)&pNotice, pNotice.h.size);
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
				GSProtocol.ChatTargetSend(lpObj, szChat, iTargetNumber);
			}
		}
	}
}
#pragma warning ( default : 4101 )


int CCrywolfUtil::CrywolfMVPLevelUp(int iUserIndex, int iAddExp)
{
	if ( !ObjectMaxRange(iUserIndex ) )
		return 0;

	if(gGameObjects[iUserIndex]->Type != OBJ_USER)
	{
		return 0;
	}

	int iLEFT_EXP = 0;

	sLog->outBasic("[ Crywolf ][MVP Exp.] : [%s][%s](%d) %u %d",
		gGameObjects[iUserIndex]->AccountID, gGameObjects[iUserIndex]->Name,
		gGameObjects[iUserIndex]->Level, gGameObjects[iUserIndex]->m_PlayerData->Experience,
		iAddExp);

	if ( g_MasterLevelSkillTreeSystem.IsMasterLevelUser(&gGameObjects[iUserIndex]) == true )
	{
		return g_MasterLevelSkillTreeSystem.MasterLevelUp(&gGameObjects[iUserIndex], iAddExp, 0, "Crywolf");
	}

	sLog->outBasic("[Crywolf] Experience : [%s][%s](%d) Experience: %d + %d",
		gGameObjects[iUserIndex]->AccountID, gGameObjects[iUserIndex]->Name,
		gGameObjects[iUserIndex]->Level, gGameObjects[iUserIndex]->m_PlayerData->Experience, iAddExp);

	gObjSetExpPetItem(iUserIndex, iAddExp);

	if ( gGameObjects[iUserIndex]->Level >= g_ConfigRead.data.common.UserMaxLevel)
	{
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0,45), gGameObjects[iUserIndex]->m_Index, 1);
		return 0;
	}

	if ( (gGameObjects[iUserIndex]->m_PlayerData->Experience + iAddExp) < gGameObjects[iUserIndex]->m_PlayerData->NextExp )
	{
		gGameObjects[iUserIndex]->m_PlayerData->Experience += iAddExp;
	}
	else
	{
		iLEFT_EXP = gGameObjects[iUserIndex]->m_PlayerData->Experience + iAddExp - gGameObjects[iUserIndex]->m_PlayerData->NextExp;
		gGameObjects[iUserIndex]->m_PlayerData->Experience = gGameObjects[iUserIndex]->m_PlayerData->NextExp;
		gGameObjects[iUserIndex]->Level++;

		if ( g_ConfigRead.data.reset.iBlockLevelUpPointAfterResets == -1 || gGameObjects[iUserIndex]->m_PlayerData->m_iResets < g_ConfigRead.data.reset.iBlockLevelUpPointAfterResets )
		{
			if (gGameObjects[iUserIndex]->Class == CLASS_DARKLORD || gGameObjects[iUserIndex]->Class == CLASS_MAGUMSA || gGameObjects[iUserIndex]->Class == CLASS_RAGEFIGHTER || gGameObjects[iUserIndex]->Class == CLASS_GROWLANCER)
			{
				gGameObjects[iUserIndex]->m_PlayerData->LevelUpPoint += g_MaxStatsInfo.GetClass.LevelUpPointMGDL;
			}

			else
			{
				gGameObjects[iUserIndex]->m_PlayerData->LevelUpPoint += g_MaxStatsInfo.GetClass.LevelUpPointNormal;
			}

			if (gGameObjects[iUserIndex]->m_PlayerData->PlusStatQuestClear != false)
			{
				gGameObjects[iUserIndex]->m_PlayerData->LevelUpPoint++;

				//sLog->outBasic("[ Crywolf ][MVP Exp.] [%s][%s] LevelUp PlusStatQuest Clear AddStat %d",
				//	gGameObjects[iUserIndex]->AccountID, gGameObjects[iUserIndex]->Name,gGameObjects[iUserIndex]->m_PlayerData->LevelUpPoint);
			}

			if ( gGameObjects[iUserIndex]->m_PlayerData->PlusStatQuestClear != false )
			{
			}
		}

		gGameObjects[iUserIndex]->MaxLife += DCInfo.DefClass[gGameObjects[iUserIndex]->Class]->LevelLife;
		gGameObjects[iUserIndex]->MaxMana += DCInfo.DefClass[gGameObjects[iUserIndex]->Class]->LevelMana;
		gGameObjects[iUserIndex]->Life = gGameObjects[iUserIndex]->MaxLife;
		gGameObjects[iUserIndex]->Mana = gGameObjects[iUserIndex]->MaxMana;
		gObjNextExpCal(&gGameObjects[iUserIndex]);
		gObjSetBP(iUserIndex);
		GSProtocol.GCLevelUpMsgSend(gGameObjects[iUserIndex]->m_Index, 1);
		gObjCalcMaxLifePower(gGameObjects[iUserIndex]->m_Index);

		sLog->outBasic("Level Up [%s][%s][%d]", gGameObjects[iUserIndex]->AccountID,
			gGameObjects[iUserIndex]->Name, gGameObjects[iUserIndex]->Level);
	}

	return iLEFT_EXP;
}
	





////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

