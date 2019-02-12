// TMonsterAI.cpp: implementation of the TMonsterAI class.
//	GS-N	1.00.18	JPN	0x0055A7A0	-	Completed
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "gObjMonster.h"
#include "TMonsterAI.h"
#include "TMonsterAIRule.h"
#include "TMonsterAIUnit.h"
#include "TMonsterAIUtil.h"
#include "CrywolfUtil.h"
#include "BuffEffectSlot.h"
#include "BuffEffect.h"
#include "User/CUserData.h"
#include "configread.h"

static TMonsterAIUtil MONSTER_UTIL;
static CCrywolfUtil UTIL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TMonsterAI::TMonsterAI()
{

}

TMonsterAI::~TMonsterAI()
{

}


void TMonsterAI::MonsterAIProc()
{
	TMonsterAIRule::MonsterAIRuleProc();

	for ( int n=0;n<g_ConfigRead.server.GetObjectMaxMonster();n++)
	{
		CGameObject lpObj = &gGameObjects[n];

		if ( lpObj.m_iCurrentAI == 0 || lpObj.Live == FALSE ||lpObj.Connected != PLAYER_PLAYING )
		{
			if ( lpObj.m_iCurrentAI != 0 )
			{
				TMonsterAI::MonsterStateMsgProc(lpObj.m_Index);
			}
			continue;
		}
		else
		{
			TMonsterAI::UpdateCurrentAIUnit(lpObj.m_Index);
			TMonsterAI::RunAI(lpObj.m_Index, lpObj.Class);
		}
	}
}


bool TMonsterAI::RunAI(CGameObject &Obj, int iMonsterClass)
{
	CGameObject lpObj = Obj;

	TMonsterAI::MonsterStateMsgProc(iIndex);

	if ( lpObj.m_iCurrentAI == 0 )
		return FALSE;

	TMonsterAIUnit * lpAIUnit = TMonsterAIUnit::FindAIUnit(lpObj.m_iCurrentAI);

	if ( lpAIUnit == NULL )
		return FALSE;

	if ( (GetTickCount() - lpObj.m_iLastAIRunTime) < lpAIUnit->m_iDelayTime )
		return FALSE;

	if ( lpAIUnit->RunAIUnit(iIndex) == FALSE )
		return FALSE;

	lpObj.m_iLastAIRunTime = GetTickCount();

	return TRUE;
}


void TMonsterAI::MonsterStateMsgProc(CGameObject &Obj)
{
	CGameObject lpObj = Obj;

	for ( int i=0;i<MAX_MONSTER_SEND_MSG;i++)
	{
		if ( gSMMsg[iIndex][i].MsgCode >= 0 )
		{
			if ( GetTickCount64() > gSMMsg[iIndex][i].MsgTime )
			{
				TMonsterAI::ProcessStateMsg(lpObj, gSMMsg[iIndex][i].MsgCode, gSMMsg[iIndex][i].SendUser, gSMMsg[iIndex][i].SubCode);
				gSMMsg[iIndex][i].MsgCode = -1;
			}
		}
	}
}



void TMonsterAI::ProcessStateMsg(CGameObject &lpObj, int iMsgCode, int iIndex, int aMsgSubCode)
{
	switch ( iMsgCode )
	{
		case 1:
			gObjMonsterDieGiveItem(lpObj, Obj);
			break;

		case 2:
			if ( Obj.Live != FALSE )
			{
				gObjBackSpring(lpObj, Obj);
			}
			break;

		case 3:
			if ( lpObj.TargetNumber != -1 )
			{
				lpObj.m_Agro->DelAgro(lpObj.TargetNumber);
			}

			lpObj.TargetNumber = -1;
			lpObj.LastAttackerID = -1;
			lpObj.m_ActState.Emotion = 0;
			lpObj.m_ActState.Attack = 0;
			lpObj.m_ActState.Move = 0;
			lpObj.NextActionTime = 1000;
			break;

		case 4:
			lpObj.m_ActState.Emotion = 3;
			lpObj.m_ActState.EmotionCount = 1;
			break;

		case 55:
			gObjAttack(lpObj, Obj, NULL, FALSE, FALSE, 0, FALSE, 0, 0);
			break;

		case 56:
			{
				CGameObject lpTargetObj = Obj;
				
				if(!gObjCheckUsedBuffEffect(lpTargetObj, BUFFTYPE_POISON))
				{
					if( retResistance(lpTargetObj, 1) == FALSE )
					{
						lpTargetObj.lpAttackObj = lpObj;
						gObjAddBuffEffect(lpTargetObj, BUFFTYPE_POISON, EFFECTTYPE_POISON_DMG_TICK, 3, 0, 0, aMsgSubCode);
					}
				}
			}
			break;

		case 57:
			{
				CGameObject lpTargetObj = Obj;
				gObjBackSpring2(lpTargetObj, lpObj, aMsgSubCode);
			}
			break;
	}
}




bool TMonsterAI::UpdateCurrentAIUnit(CGameObject &Obj)
{
	CGameObject lpObj = Obj;

	if ( lpObj.Live == FALSE )
		return FALSE;

	int iOldCurrentAI = lpObj.m_iCurrentAI;
	int iCurrentAI = TMonsterAIRule::GetCurrentAIUnit(lpObj.Class);

	if ( iCurrentAI == 0 )
	{
		if ( lpObj.m_iBasicAI != 0 )
		{
			iCurrentAI = lpObj.m_iBasicAI;
		}
	}

	lpObj.m_iCurrentAI = iCurrentAI;

	if ( iOldCurrentAI != iCurrentAI )
	{
		UTIL.SendCrywolfChattingMsg(lpObj.m_Index, "��AI %s ���� %s �� �ٲ۴�!!", TMonsterAIUnit::FindAIUnit(iOldCurrentAI)->m_szUnitName, TMonsterAIUnit::FindAIUnit(iCurrentAI)->m_szUnitName);
	}

	return TRUE;
}


void TMonsterAI::MonsterMoveProc()
{
	for ( int i=0;i<g_ConfigRead.server.GetObjectMaxMonster();i++)
	{
		if ( gGameObjects[i]->m_iCurrentAI !=  0 )
		{
			TMonsterAI::MonsterMove(i);
		}
	}
}



void TMonsterAI::MonsterMove(CGameObject &Obj)
{
	CGameObject lpObj = Obj;

	if ( MONSTER_UTIL.CheckMovingCondition(lpObj) == FALSE )
	{
		lpObj.PathCur = 0;
		lpObj.PathCount = 0;
		lpObj.PathStartEnd = 0;
		memset(lpObj.PathX, 0, sizeof(lpObj.PathX));
		memset(lpObj.PathY, 0, sizeof(lpObj.PathY));
		memset(lpObj.PathDir, 0, sizeof(lpObj.PathDir));

		return ;
	}

	if ( lpObj.PathCount != 0 )
	{
		DWORD dwMoveTime = 0;
		DWORD dwDelayTime = 0;

		if ( lpObj.DelayLevel != 0 )
			dwDelayTime = 300;
		else
			dwDelayTime = 0;

		lpObj.m_MoveSpeed = 300;
		
		if ( (lpObj.PathDir[lpObj.PathCur] % 2 ) == 0 )
			dwMoveTime = (DWORD)((double)(lpObj.m_MoveSpeed + dwDelayTime) * 1.3);
		else
			dwMoveTime = lpObj.m_MoveSpeed + dwDelayTime;

		if ( (GetTickCount() - lpObj.PathTime) > dwMoveTime )
		{
			if ( lpObj.PathCur < 15 )
			{
				lpObj.X = lpObj.PathX[lpObj.PathCur];
				lpObj.Y = lpObj.PathY[lpObj.PathCur];
				lpObj.Dir = lpObj.PathDir[lpObj.PathCur];
				lpObj.PathTime = GetTickCount();
				lpObj.PathCur++;

				if ( lpObj.PathCur >= lpObj.PathCount )
				{
					lpObj.PathCur = 0;
					lpObj.PathCount = 0;
					lpObj.PathStartEnd = 0;
				}
			}

			CreateFrustrum(lpObj.X, lpObj.Y, iIndex);
		}

		return;

	}

	lpObj.PathCur = 0;
	lpObj.PathCount = 0;
	lpObj.PathStartEnd = 0;
	memset(lpObj.PathX, 0, sizeof(lpObj.PathX));
	memset(lpObj.PathY, 0, sizeof(lpObj.PathY));
	memset(lpObj.PathDir, 0, sizeof(lpObj.PathDir));
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

