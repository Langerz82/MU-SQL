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
		CGameObject lpObj = getGameObject(n);

		if ( Obj.m_iCurrentAI == 0 || Obj.Live == FALSE ||Obj.Connected != PLAYER_PLAYING )
		{
			if ( Obj.m_iCurrentAI != 0 )
			{
				TMonsterAI::MonsterStateMsgProc(Obj.m_Index);
			}
			continue;
		}
		else
		{
			TMonsterAI::UpdateCurrentAIUnit(Obj.m_Index);
			TMonsterAI::RunAI(Obj.m_Index, Obj.Class);
		}
	}
}


bool TMonsterAI::RunAI(CGameObject &Obj, int iMonsterClass)
{
	CGameObject* lpObj = Obj;

	TMonsterAI::MonsterStateMsgProc(Obj.m_Index);

	if ( Obj.m_iCurrentAI == 0 )
		return FALSE;

	TMonsterAIUnit * lpAIUnit = TMonsterAIUnit::FindAIUnit(Obj.m_iCurrentAI);

	if ( lpAIUnit == NULL )
		return FALSE;

	if ( (GetTickCount() - Obj.m_iLastAIRunTime) < lpAIUnit->m_iDelayTime )
		return FALSE;

	if ( lpAIUnit->RunAIUnit(Obj.m_Index) == FALSE )
		return FALSE;

	Obj.m_iLastAIRunTime = GetTickCount();

	return TRUE;
}


void TMonsterAI::MonsterStateMsgProc(CGameObject &Obj)
{
	CGameObject* lpObj = Obj;

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



void TMonsterAI::ProcessStateMsg(CGameObject &Obj, int iMsgCode, CGameObject &Obj, int aMsgSubCode)
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
			if ( Obj.TargetNumber != -1 )
			{
				Obj.m_Agro->DelAgro(Obj.TargetNumber);
			}

			Obj.TargetNumber = -1;
			Obj.LastAttackerID = -1;
			Obj.m_ActState.Emotion = 0;
			Obj.m_ActState.Attack = 0;
			Obj.m_ActState.Move = 0;
			Obj.NextActionTime = 1000;
			break;

		case 4:
			Obj.m_ActState.Emotion = 3;
			Obj.m_ActState.EmotionCount = 1;
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
	CGameObject* lpObj = Obj;

	if ( Obj.Live == FALSE )
		return FALSE;

	int iOldCurrentAI = Obj.m_iCurrentAI;
	int iCurrentAI = TMonsterAIRule::GetCurrentAIUnit(Obj.Class);

	if ( iCurrentAI == 0 )
	{
		if ( Obj.m_iBasicAI != 0 )
		{
			iCurrentAI = Obj.m_iBasicAI;
		}
	}

	Obj.m_iCurrentAI = iCurrentAI;

	if ( iOldCurrentAI != iCurrentAI )
	{
		UTIL.SendCrywolfChattingMsg(Obj.m_Index, "★AI %s 에서 %s 로 바꾼다!!", TMonsterAIUnit::FindAIUnit(iOldCurrentAI)->m_szUnitName, TMonsterAIUnit::FindAIUnit(iCurrentAI)->m_szUnitName);
	}

	return TRUE;
}


void TMonsterAI::MonsterMoveProc()
{
	for ( int i=0;i<g_ConfigRead.server.GetObjectMaxMonster();i++)
	{
		if ( getGameObject(i)->m_iCurrentAI !=  0 )
		{
			TMonsterAI::MonsterMove(i);
		}
	}
}



void TMonsterAI::MonsterMove(CGameObject &Obj)
{
	CGameObject* lpObj = Obj;

	if ( MONSTER_UTIL.CheckMovingCondition(lpObj) == FALSE )
	{
		Obj.PathCur = 0;
		Obj.PathCount = 0;
		Obj.PathStartEnd = 0;
		memset(Obj.PathX, 0, sizeof(Obj.PathX));
		memset(Obj.PathY, 0, sizeof(Obj.PathY));
		memset(Obj.PathDir, 0, sizeof(Obj.PathDir));

		return ;
	}

	if ( Obj.PathCount != 0 )
	{
		DWORD dwMoveTime = 0;
		DWORD dwDelayTime = 0;

		if ( Obj.DelayLevel != 0 )
			dwDelayTime = 300;
		else
			dwDelayTime = 0;

		Obj.m_MoveSpeed = 300;
		
		if ( (Obj.PathDir[Obj.PathCur] % 2 ) == 0 )
			dwMoveTime = (DWORD)((double)(Obj.m_MoveSpeed + dwDelayTime) * 1.3);
		else
			dwMoveTime = Obj.m_MoveSpeed + dwDelayTime;

		if ( (GetTickCount() - Obj.PathTime) > dwMoveTime )
		{
			if ( Obj.PathCur < 15 )
			{
				Obj.X = Obj.PathX[Obj.PathCur];
				Obj.Y = Obj.PathY[Obj.PathCur];
				Obj.Dir = Obj.PathDir[Obj.PathCur];
				Obj.PathTime = GetTickCount();
				Obj.PathCur++;

				if ( Obj.PathCur >= Obj.PathCount )
				{
					Obj.PathCur = 0;
					Obj.PathCount = 0;
					Obj.PathStartEnd = 0;
				}
			}

			CreateFrustrum(Obj.X, Obj.Y, iIndex);
		}

		return;

	}

	Obj.PathCur = 0;
	Obj.PathCount = 0;
	Obj.PathStartEnd = 0;
	memset(Obj.PathX, 0, sizeof(Obj.PathX));
	memset(Obj.PathY, 0, sizeof(Obj.PathY));
	memset(Obj.PathDir, 0, sizeof(Obj.PathDir));
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

