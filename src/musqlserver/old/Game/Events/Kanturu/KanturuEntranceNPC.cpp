// KanturuEntranceNPC.cpp: implementation of the CKanturuEntranceNPC class.
//	GS-N	1.00.18	JPN	0x00581670	-	Completed
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KanturuEntranceNPC.h"
#include "Kanturu.h"
#include "KanturuBattleUserMng.h"
#include "Logging/Log.h"
#include "KanturuUtil.h"
#include "util.h"
#include "configread.h"

static CKanturuUtil KANTURU_UTIL;

CKanturuEntranceNPC g_KanturuEntranceNPC;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKanturuEntranceNPC::CKanturuEntranceNPC()
{
	return;
}

CKanturuEntranceNPC::~CKanturuEntranceNPC()
{
	return;
}

struct PMSG_ANS_KANTURU_STATE_INFO
{
	PBMSG_HEAD2 h;	// C1:D1:00
	BYTE btState;	// 4
	BYTE btDetailState;	// 5
	BYTE btEnter;	// 6
	BYTE btUserCount;	// 7
	int iRemainTime;	// 8
};

void CKanturuEntranceNPC::NotifyEntranceInfo(CGameObject &Obj)
{
	PMSG_ANS_KANTURU_STATE_INFO pMsg = {0};

	PHeadSubSetB((BYTE*)&pMsg, 0xD1, 0x00, sizeof(pMsg));
	int iCurrentState = g_Kanturu.GetKanturuState();
	int iCurrentDetailState = g_Kanturu.GetKanturuDetailState();
	pMsg.btState = iCurrentState;
	pMsg.btDetailState = iCurrentDetailState;
	pMsg.btEnter = g_Kanturu.CheckCanEnterKanturuBattle();
	pMsg.btUserCount = g_KanturuBattleUserMng.GetUserCount();
	pMsg.iRemainTime = g_Kanturu.GetRemainTime();

	KANTURU_UTIL.SendDataToUser(iIndex, (BYTE*)&pMsg, sizeof(pMsg));
}


void CKanturuEntranceNPC::NotifyResultEnterKanturuBossMap(CGameObject &Obj)
{
	if ( !gObjIsConnectedGP(Obj.m_Index))
	{
		return;
	}

	CGameObject lpObj = Obj;
	BOOL bMoveGateSuccess = FALSE;
	int iResult = g_Kanturu.CheckEnterKanturu(Obj.m_Index);

	if ( iResult == 0 )
	{
		int iCurrentState = g_Kanturu.GetKanturuState();

		if ( iCurrentState == KANTURU_STATE_BATTLE_OF_MAYA )
		{
			bMoveGateSuccess = gObjMoveGate(iIndex, 133);

			if ( bMoveGateSuccess ==TRUE )
			{
				g_KanturuBattleUserMng.AddUserData(Obj.m_Index);

				KANTURU_UTIL.NotifyKanturuCurrentState(iIndex, 
					g_Kanturu.GetKanturuState(),g_Kanturu.GetKanturuDetailState());

				if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
				{
					Obj.m_PlayerData->m_bKanturuEntranceByNPC = TRUE;
				}
			}
		}
		else if ( iCurrentState == KANTURU_STATE_TOWER_OF_REFINEMENT )
		{
			bMoveGateSuccess = gObjMoveGate(iIndex, 135);

			KANTURU_UTIL.NotifyKanturuCurrentState(iIndex, 
				g_Kanturu.GetKanturuState(),g_Kanturu.GetKanturuDetailState());

			if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
			{
				Obj.m_PlayerData->m_bKanturuEntranceByNPC = TRUE;
			}
		}
		else
		{
		}
	}
	else
	{
		KANTURU_UTIL.NotifyKanturuEntranceReqResult(iIndex, iResult);

		if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
		{
			Obj.m_PlayerData->m_bKanturuEntranceByNPC = FALSE;
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

