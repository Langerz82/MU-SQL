// RaklionUtil.cpp: implementation of the CRaklionUtil class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RaklionUtil.h"
#include "util.h"
#include "Raklion.h"
#include "User/CUserData.h"
#include "TNotice.h"
#include "RaklionBattleUserMng.h"
#include "GameMain.h"
#include "configread.h"

CRaklionUtil	g_RaklionUtil;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRaklionUtil::CRaklionUtil()
{

}

CRaklionUtil::~CRaklionUtil()
{

}

void CRaklionUtil::NotifyRaklionChangeState(int iState, int iDetailState)
{
	PMSG_ANS_RAKLION_STATE_CHANGE pMsg = {0};

	PHeadSubSetB((BYTE*)&pMsg, 0xD1, 0x12, sizeof(pMsg));
	pMsg.btState = iState;
	pMsg.btDetailState = iDetailState;

	if(iState == RAKLION_STATE_NOTIFY_1)
	{
		SendDataRaklionMapUser((BYTE*)&pMsg,sizeof(pMsg));	
		return;
	}

	SendDataRaklionBossMapUser((BYTE*)&pMsg, sizeof(pMsg));
}

void CRaklionUtil::NotifyRaklionSuccessValue(int iSuccessValue)
{
	PMSG_ANS_RAKLION_BATTLE_RESULT pMsg = {0};

	PHeadSubSetB((BYTE*)&pMsg, 0xD1, 0x13, sizeof(pMsg));
	pMsg.btResult = iSuccessValue;

	SendDataRaklionBossMapUser((BYTE*)&pMsg, sizeof(pMsg));
}

void CRaklionUtil::NotifyRaklionCurrentState(CGameObject &Obj, int iState, int iDetailState)
{
	PMSG_ANS_RAKLION_CURRENT_STATE pMsg = {0};

	PHeadSubSetB((BYTE*)&pMsg, 0xD1, 0x11, sizeof(pMsg));
	pMsg.btCurrentState = iState;
	pMsg.btCurrentDetailState = iDetailState;

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, sizeof(pMsg));
}

void CRaklionUtil::NotifyRaklionWideAreaAttack(CGameObject &Obj, int iTargetIndex, int iSkillType)
{
	PMSG_NOTIFY_RAKLION_WIDE_AREA_ATTACK pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xD1, 0x14, sizeof(pMsg));
	pMsg.btObjClassH = HIBYTE(Obj.Class);
	pMsg.btObjClassL = LOBYTE(Obj.Class);
	pMsg.btType = iSkillType;

	SendDataToUser(iTargetIndex, (BYTE*)&pMsg, sizeof(pMsg));
}

void CRaklionUtil::NotifyRaklionUserMonsterCount(int iMonsterCount, int iUserCount)
{
	PMSG_NOTIFY_RAKLION_USER_MONSTER_COUNT pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xD1, 0x15, sizeof(pMsg));
	pMsg.btMonsterCount = iMonsterCount;
	pMsg.btUserCount = iUserCount;

	SendDataRaklionBossMapUser((BYTE*)&pMsg, sizeof(pMsg));
}

void CRaklionUtil::NotifyLeftTopMsgRaklionBattleUser(char *lpszMsg, ...)
{
	if(lpszMsg == NULL)	return;

	char szBuffer[512] = "";
	va_list pArguments;

	va_start(pArguments, lpszMsg);
	vsprintf(szBuffer, lpszMsg, pArguments);
	va_end(pArguments);

	PMSG_NOTICE pNotice;
	TNotice::MakeNoticeMsg(&pNotice, 1, szBuffer);

	for(int iCount=0; iCount<g_RaklionBattleUserMng.m_BattleUser.size(); iCount++)
	{
		if( gGameObjects[g_RaklionBattleUserMng.m_BattleUser[iCount]]->Connected == PLAYER_PLAYING &&
			gGameObjects[g_RaklionBattleUserMng.m_BattleUser[iCount]]->Type == OBJ_USER)
		{
			TNotice::SendNoticeToUser(g_RaklionBattleUserMng.m_BattleUser[iCount], &pNotice);
		}
	}
}

void CRaklionUtil::SendMsgRaklionBossMapUser(char *lpszMsg, ...)
{
	if(lpszMsg == NULL)	return;

	char szBuffer[512] = "";
	va_list pArguments;

	va_start(pArguments, lpszMsg);
	vsprintf(szBuffer, lpszMsg, pArguments);
	va_end(pArguments);

	PMSG_NOTICE pNotice;
	TNotice::MakeNoticeMsg(&pNotice, 0, szBuffer);

	for(int iCount=g_ConfigRead.server.GetObjectStartUserIndex(); iCount<g_ConfigRead.server.GetObjectMax(); iCount++)
	{
		if( gGameObjects[iCount]->Connected == PLAYER_PLAYING &&
			gGameObjects[iCount]->Type == OBJ_USER &&
			gGameObjects[iCount]->MapNumber == MAP_INDEX_HATCHERY)
		{
			TNotice::SendNoticeToUser(iCount, &pNotice);
		}
	}
}

void CRaklionUtil::SendDataRaklionBossMapUser(BYTE *lpMsg, int iSize)
{
	for(int iCount=g_ConfigRead.server.GetObjectStartUserIndex(); iCount<g_ConfigRead.server.GetObjectMax(); iCount++)
	{
		if( gGameObjects[iCount]->Connected == PLAYER_PLAYING &&
			 gGameObjects[iCount]->Type == OBJ_USER &&
			 gGameObjects[iCount]->MapNumber == MAP_INDEX_HATCHERY)
		{
			IOCP.DataSend(iCount, lpMsg, iSize);
		}
	}
}

void CRaklionUtil::SendMsgRaklionMapUser(char *lpszMsg, ...)
{
	if(lpszMsg == NULL)	return;

	char szBuffer[512] = "";
	va_list pArguments;

	va_start(pArguments, lpszMsg);
	vsprintf(szBuffer, lpszMsg, pArguments);
	va_end(pArguments);

	PMSG_NOTICE pNotice;
	TNotice::MakeNoticeMsg(&pNotice, 0, szBuffer);

	for(int iCount=g_ConfigRead.server.GetObjectStartUserIndex(); iCount<g_ConfigRead.server.GetObjectMax(); iCount++)
	{
		if( gGameObjects[iCount]->Connected == PLAYER_PLAYING &&
			gGameObjects[iCount]->Type == OBJ_USER &&
			(gGameObjects[iCount]->MapNumber == MAP_INDEX_RAKLION || gGameObjects[iCount]->MapNumber == MAP_INDEX_HATCHERY) )
		{
			TNotice::SendNoticeToUser(iCount, &pNotice);
		}
	}
}

void CRaklionUtil::SendDataRaklionMapUser(BYTE *lpMsg, int iSize)
{
	for(int iCount=g_ConfigRead.server.GetObjectStartUserIndex(); iCount<g_ConfigRead.server.GetObjectMax(); iCount++)
	{
		if( gGameObjects[iCount]->Connected == PLAYER_PLAYING &&
			 gGameObjects[iCount]->Type == OBJ_USER &&
			 (gGameObjects[iCount]->MapNumber == MAP_INDEX_RAKLION || gGameObjects[iCount]->MapNumber == MAP_INDEX_HATCHERY) )
		{
			IOCP.DataSend(iCount, lpMsg, iSize);
		}
	}
}

void CRaklionUtil::SendMsgAllUser(char *lpszMsg, ...)
{
	if(lpszMsg == NULL)	return;

	char szBuffer[512] = "";
	va_list pArguments;

	va_start(pArguments, lpszMsg);
	vsprintf(szBuffer, lpszMsg, pArguments);
	va_end(pArguments);

	PMSG_NOTICE pNotice;
	TNotice::MakeNoticeMsg(&pNotice, 0, szBuffer);
	TNotice::SendNoticeToAllUser(&pNotice);
}

void CRaklionUtil::SendDataAllUser(BYTE *lpMsg, int iSize)
{
	for(int iCount=g_ConfigRead.server.GetObjectStartUserIndex(); iCount<g_ConfigRead.server.GetObjectMax(); iCount++)
	{
		if( gGameObjects[iCount]->Connected == PLAYER_PLAYING &&
			gGameObjects[iCount]->Type == OBJ_USER )
		{
			IOCP.DataSend(iCount, lpMsg, iSize);
		}
	}
}

void CRaklionUtil::SendMsgToUser(CGameObject &Obj, char *lpszMsg, ...)
{
	if(lpszMsg == NULL)	return;

	char szBuffer[512] = "";
	va_list pArguments;

	va_start(pArguments, lpszMsg);
	vsprintf(szBuffer, lpszMsg, pArguments);
	va_end(pArguments);

	PMSG_NOTICE pNotice;
	TNotice::MakeNoticeMsg(&pNotice, 0, szBuffer);
	TNotice::SendNoticeToUser(iIndex, &pNotice);
}

void CRaklionUtil::SendDataToUser(CGameObject &Obj, BYTE *lpMsg, int iSize)
{
	if ( Obj.Connected == PLAYER_PLAYING &&
		 Obj.Type == OBJ_USER )
	{
		IOCP.DataSend(Obj.m_Index, lpMsg, iSize);
	}
}

void CRaklionUtil::SendSystemMsgToUser(CGameObject &Obj, char *lpszMsg, ...)
{
	if(lpszMsg == NULL)	return;

	char szBuffer[512] = "";
	va_list pArguments;
	
	va_start( pArguments,  lpszMsg);
	vsprintf(szBuffer, lpszMsg, pArguments);
	va_end(pArguments);

	TNotice::GCServerMsgStringSend(lpszMsg, iIndex, 1);
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

