// KanturuBattleOfMaya.cpp: implementation of the CKanturuBattleOfMaya class.
//	GS-N	1.00.18	JPN	0x0057A2C0	-	Completed
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "KanturuBattleOfMaya.h"
#include "Gamemain.h"
#include "TMonsterAIGroup.h"
#include "Kanturu.h"
#include "KanturuMonsterMng.h"
#include "KanturuBattleUserMng.h"
#include "TLog.h"
#include "KanturuUtil.h"

static CKanturuUtil KANTURU_UTIL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKanturuBattleOfMaya::CKanturuBattleOfMaya()
{
	this->SetBattleOfMayaState(0);
	this->SetBattleSceneState(-1);
	this->m_iMayaLeftHandObjIndex = -1;
	this->m_iMayaRightHandObjIndex = -1;

	for ( int iCount=0;iCount<KANTURU_MAYA_AI_CHANGE_TIME;iCount++)
	{
		this->m_iMayaHandAIChangeTime[iCount] = -1;
	}

	this->m_iMayaHandAIAppliedTime = 0;
	this->m_iMayaHandCurrentAI = 0;

	for ( int iCount=0;iCount<KANTURU_MAYA_GROUP_NUMBER;iCount++)
	{
		this->m_iMayaHandGroupNumber[iCount] = -1;
	}

	this->m_iMayaHandDieTimeCounter = 0;
	this->ResetAllData();
}



CKanturuBattleOfMaya::~CKanturuBattleOfMaya()
{
	return;
}

void CKanturuBattleOfMaya::ResetAllData()
{
	this->m_bFileDataLoad = FALSE;

	for ( int iCount=0;iCount<KANTURU_MAYA_TIME_INFO;iCount++)
	{
		this->m_BattleTimeInfo[iCount].ResetTimeInfo();
	}

	for ( int iCount=0;iCount<KANTURU_MAYA_STATE_INFO;iCount++)
	{
		this->m_StateInfo[iCount].ResetTimeInfo();
	}

	this->m_StateInfoCount = 0;
	this->m_BattleTimeInfoCount = 0;

	this->SetSuccessValue(FALSE);
	this->SetSceneSuccessValue(FALSE);
	this->SetEntrancePermit(FALSE);

	this->m_iMayaHandAIChangeTimeCount = 0;
	this->m_iMayaHAndGroupNumberCount = 0;
	this->m_iMayaIceStormUsingRate = 0;
}

BOOL CKanturuBattleOfMaya::LoadData(LPSTR lpszFileName)
{
	this->m_bFileDataLoad = FALSE;

	if ( !lpszFileName || !strcmp(lpszFileName , "") )
	{
		g_Log.MsgBox("[Kanturu][Battle of Maya] - File load error : File Name Error");
		return FALSE;
	}

	try
	{
		pugi::xml_document file;
		pugi::xml_parse_result res = file.load_file(lpszFileName);

		if ( res.status != pugi::status_ok )
		{
			g_Log.MsgBox("[Kanturu][Battle of Maya] - Can't Load %s (%s)", lpszFileName, res.description());
			return FALSE;
		}

		this->ResetAllData();

		pugi::xml_node main = file.child("KanturuEvent");
		pugi::xml_node battle_maya = main.child("BattleOfMaya");

		this->m_iMayaIceStormUsingRate = battle_maya.attribute("IceStormCastRate").as_int();

		pugi::xml_node state = battle_maya.child("State");

		for (pugi::xml_node cycle = state.child("Cycle"); cycle; cycle = cycle.next_sibling())
		{
			int iState = cycle.attribute("State").as_int();
			int iCondition = cycle.attribute("SwitchType").as_int();
			int iValue = cycle.attribute("Time").as_int();

			if ( this->m_StateInfoCount < 0 || this->m_StateInfoCount >= KANTURU_MAYA_STATE_INFO )
			{
				g_Log.MsgBox("[Kanturu][Battle of Maya] - Exceed Max State Time (%d)", this->m_StateInfoCount);
				break;
			}

			this->m_StateInfo[this->m_StateInfoCount].SetStateInfo(iState);
			this->m_StateInfo[this->m_StateInfoCount].SetCondition(iCondition);

			if ( iCondition == 1 )
				iValue *= 1000;

			this->m_StateInfo[this->m_StateInfoCount].SetValue(iValue);
			this->m_StateInfoCount++;
		}

		pugi::xml_node battle_times = battle_maya.child("BattleTimes");

		for (pugi::xml_node battle = battle_times.child("Battle"); battle; battle = battle.next_sibling())
		{
			int iType = battle.attribute("Type").as_int();
			int iCondition = battle.attribute("SwitchType").as_int();
			int iValue = battle.attribute("Time").as_int();

			if ( this->m_BattleTimeInfoCount < 0 || this->m_BattleTimeInfoCount >= KANTURU_MAYA_TIME_INFO )
			{
				g_Log.MsgBox("[Kanturu][Battle of Maya] - Exceed Max Scene (%d)", this->m_BattleTimeInfoCount);
				break;
			}

			this->m_BattleTimeInfo[this->m_BattleTimeInfoCount].SetStateInfo(iType);
			this->m_BattleTimeInfo[this->m_BattleTimeInfoCount].SetCondition(iCondition);

			if ( iCondition == 1 )
				iValue *= 1000;

			this->m_BattleTimeInfo[this->m_BattleTimeInfoCount].SetValue(iValue);
			this->m_BattleTimeInfoCount++;
		}

		pugi::xml_node ai_settings = battle_maya.child("AISettings");

		for (pugi::xml_attribute ai_state = ai_settings.first_attribute(); ai_state; ai_state = ai_state.next_attribute())
		{
			int iAIState = ai_state.as_int();

			if ( this->m_iMayaHAndGroupNumberCount < 0 || this->m_iMayaHAndGroupNumberCount >= KANTURU_MAYA_GROUP_NUMBER )
			{
				g_Log.MsgBox("[Kanturu][Battle of Maya] - Maya Hand AI Group Read Error(%d)", this->m_iMayaHAndGroupNumberCount);
				break;
			}
					
			this->m_iMayaHandGroupNumber[this->m_iMayaHAndGroupNumberCount] = iAIState;
			this->m_iMayaHAndGroupNumberCount++;
		}

		for (pugi::xml_node ai = ai_settings.child("AI"); ai; ai = ai.next_sibling())
		{
			int iAIChangeTime = ai.attribute("Time").as_int();

			if ( this->m_iMayaHandAIChangeTimeCount < 0 || this->m_iMayaHandAIChangeTimeCount >= KANTURU_MAYA_AI_CHANGE_TIME )
			{
				g_Log.MsgBox("[Kanturu][Battle of Maya] - Exceed Maya Hand AI Change Time(%d)", this->m_iMayaHandAIChangeTimeCount);
				break;
			}
					
			this->m_iMayaHandAIChangeTime[this->m_iMayaHandAIChangeTimeCount] = iAIChangeTime * 1000;
			this->m_iMayaHandAIChangeTimeCount++;
		}

		this->m_bFileDataLoad = TRUE;
	}

	catch ( DWORD )
	{
		g_Log.MsgBox("[Kanturu][Battle of Maya] - Loading Exception Error (%s) File. ", lpszFileName);
	}

	return this->m_bFileDataLoad;
}



void CKanturuBattleOfMaya::Run()
{
	this->CheckStateTime();
	this->CheckMayaBattleUser();
	g_KanturuBattleUserMng.CheckUserState();

	switch ( this->m_iBattleOfMayaState )
	{
		case KANTURU_MAYA_NONE:
			this->ProcState_NONE();
			break;
		case KANTURU_MAYA_STANBY1:
			this->ProcState_STANBY1();
			break;
		case KANTURU_MAYA_NOTIFY1:
			this->ProcState_NOTIFY1();
			break;
		case KANTURU_MAYA_START1:
			this->ProcState_START1();
			break;
		case KANTURU_MAYA1:
			this->ProcState_MAYA1();
			break;
		case KANTURU_MAYA_END1:
			this->ProcState_END_MAYA1();
			break;
		case KANTURU_MAYA_ENDCYCLE1:
			this->ProcState_ENDCYCLE_MAYA1();
			break;
		case KANTURU_MAYA_STANBY2:
			this->ProcState_STANBY2();
			break;
		case KANTURU_MAYA_START2:
			this->ProcState_START2();
			break;
		case KANTURU_MAYA2:
			this->ProcState_MAYA2();
			break;
		case KANTURU_MAYA_END2:
			this->ProcState_END_MAYA2();
			break;
		case KANTURU_MAYA_ENDCYCLE2:
			this->ProcState_ENDCYCLE_MAYA2();
			break;
		case KANTURU_MAYA_STANBY3:
			this->ProcState_STANBY3();
			break;
		case KANTURU_MAYA_START3:
			this->ProcState_START3();
			break;
		case KANTURU_MAYA3:
			this->ProcState_MAYA3();
			break;
		case KANTURU_MAYA_END3:
			this->ProcState_END_MAYA3();
			break;
		case KANTURU_MAYA_ENDCYCLE3:
			this->ProcState_ENDCYCLE_MAYA3();
			break;
		case KANTURU_MAYA_END:
			this->ProcState_END();
			break;
		case KANTURU_MAYA_ENDCYCLE:
			this->ProcState_ENDCYCLE();
			break;
	}
}


void CKanturuBattleOfMaya::SetState(int iBattleOfMayaState)
{
	if ( this->m_StateInfo[iBattleOfMayaState].GetCondition() > 0 )
	{
		this->m_StateInfo[iBattleOfMayaState].SetAppliedTime();
	}

	g_KanturuMonsterMng.ResetRegenMonsterObjData();
	this->m_KanturuMaya.Init();
	this->ResetAIMonster();

	switch ( iBattleOfMayaState )
	{
		case KANTURU_MAYA_NONE:
			this->SetState_NONE();
			break;
		case KANTURU_MAYA_STANBY1:
			this->SetState_STANBY1();
			break;
		case KANTURU_MAYA_NOTIFY1:
			this->SetState_NOTIFY1();
			break;
		case KANTURU_MAYA_START1:
			this->SetState_START1();
			break;
		case KANTURU_MAYA1:
			this->SetState_MAYA1();
			break;
		case KANTURU_MAYA_END1:
			this->SetState_END_MAYA1();
			break;
		case KANTURU_MAYA_ENDCYCLE1:
			this->SetState_ENDCYCLE_MAYA1();
			break;
		case KANTURU_MAYA_STANBY2:
			this->SetState_STANBY2();
			break;
		case KANTURU_MAYA_START2:
			this->SetState_START2();
			break;
		case KANTURU_MAYA2:
			this->SetState_MAYA2();
			break;
		case KANTURU_MAYA_END2:
			this->SetState_END_MAYA2();
			break;
		case KANTURU_MAYA_ENDCYCLE2:
			this->SetState_ENDCYCLE_MAYA2();
			break;
		case KANTURU_MAYA_STANBY3:
			this->SetState_STANBY3();
			break;
		case KANTURU_MAYA_START3:
			this->SetState_START3();
			break;
		case KANTURU_MAYA3:
			this->SetState_MAYA3();
			break;
		case KANTURU_MAYA_END3:
			this->SetState_END_MAYA3();
			break;
		case KANTURU_MAYA_ENDCYCLE3:
			this->SetState_ENDCYCLE_MAYA3();
			break;
		case KANTURU_MAYA_END:
			this->SetState_END();
			break;
		case KANTURU_MAYA_ENDCYCLE:
			this->SetState_ENDCYCLE();
			break;
	}

	KANTURU_UTIL.NotifyKanturuChangeState(KANTURU_STATE_BATTLE_OF_MAYA, this->m_iBattleOfMayaState);
}

void CKanturuBattleOfMaya::SetNextState(int iCurrentState)
{
	int iNextState = iCurrentState + 1;

	if ( iNextState >= KANTURU_MAYA_STATE_INFO )
	{
		iNextState = KANTURU_MAYA_NONE;
	}

	this->SetState(iNextState);
}

void CKanturuBattleOfMaya::SetState_NONE()
{
	this->SetBattleOfMayaState(KANTURU_MAYA_NONE);
	this->SetEntrancePermit(FALSE);
}

void CKanturuBattleOfMaya::SetState_STANBY1()
{
	this->SetBattleOfMayaState(KANTURU_MAYA_STANBY1);
	g_KanturuBattleUserMng.ResetAllData();
	this->SetEntrancePermit(TRUE);
}

void CKanturuBattleOfMaya::SetState_NOTIFY1()
{
	this->SetBattleOfMayaState(KANTURU_MAYA_NOTIFY1);
	this->SetEntrancePermit(FALSE);
}


void CKanturuBattleOfMaya::SetState_START1()
{
	this->SetBattleOfMayaState(KANTURU_MAYA_START1);
	this->SetBattleSceneState(0);
	this->SetEntrancePermit(FALSE);
	this->SetSuccessValue(FALSE);
	this->SetSceneSuccessValue(FALSE);
	this->m_BattleTimeInfo[this->m_iSceneState].SetAppliedTime();

	if ( this->m_BattleTimeInfo[this->m_iSceneState].GetCondition() == 1 )
	{
		KANTURU_UTIL.NotifyKanturuBattleTime(this->m_BattleTimeInfo[this->m_iSceneState].GetValue());
	}

	g_KanturuMonsterMng.SetKanturuMonster(0);
	this->m_KanturuMaya.SetMayaObjIndex(g_KanturuMonsterMng.GetMayaObjIndex());
}

void CKanturuBattleOfMaya::SetState_MAYA1()
{
	this->SetBattleOfMayaState(KANTURU_MAYA1);
	this->SetEntrancePermit(FALSE);
	this->SetSuccessValue(FALSE);
	this->SetSceneSuccessValue(FALSE);
	this->m_iMayaHandDieTimeCounter = 0;
	g_KanturuMonsterMng.SetKanturuMonster(3);
	this->m_KanturuMaya.SetMayaObjIndex(g_KanturuMonsterMng.GetMayaObjIndex());
	this->SetAIMonsterGroup(this->m_iMayaHandGroupNumber[0]);
	this->m_iMayaLeftHandObjIndex = TMonsterAIGroup::FindGroupLeader(this->m_iMayaHandGroupNumber[0]);
}


void CKanturuBattleOfMaya::SetState_END_MAYA1()
{
	this->SetBattleSceneState(-1);
	this->SetBattleOfMayaState(KANTURU_MAYA_END1);
	this->SetEntrancePermit(FALSE);
	
	KANTURU_UTIL.NotifyKanturuSuccessValue(this->GetSuccessValue());
}

void CKanturuBattleOfMaya::SetState_ENDCYCLE_MAYA1()
{
	this->SetBattleOfMayaState(KANTURU_MAYA_ENDCYCLE1);
	this->SetEntrancePermit(FALSE);
	
	KANTURU_UTIL.SendMsgKauturuMapUser(Lang.GetText(0,263));
}


void CKanturuBattleOfMaya::SetState_STANBY2()
{
	this->SetBattleOfMayaState(KANTURU_MAYA_STANBY2);
	this->SetEntrancePermit(TRUE);
}


void CKanturuBattleOfMaya::SetState_START2()
{
	this->SetBattleOfMayaState(KANTURU_MAYA_START2);
	this->SetEntrancePermit(FALSE);
	this->SetBattleSceneState(1);
	this->SetSuccessValue(FALSE);
	this->SetSceneSuccessValue(FALSE);
	this->m_BattleTimeInfo[this->m_iSceneState].SetAppliedTime();

	if ( this->m_BattleTimeInfo[this->m_iSceneState].GetCondition() == 1 )
	{
		KANTURU_UTIL.NotifyKanturuBattleTime(this->m_BattleTimeInfo[this->m_iSceneState].GetValue());
	}

	g_KanturuMonsterMng.SetKanturuMonster(0);
	this->m_KanturuMaya.SetMayaObjIndex(g_KanturuMonsterMng.GetMayaObjIndex());
}


void CKanturuBattleOfMaya::SetState_MAYA2()
{
	this->SetBattleOfMayaState(KANTURU_MAYA2);
	this->SetEntrancePermit(FALSE);
	this->SetSuccessValue(FALSE);
	this->SetSceneSuccessValue(FALSE);
	this->m_iMayaHandDieTimeCounter = 0;
	g_KanturuMonsterMng.SetKanturuMonster(4);
	this->m_KanturuMaya.SetMayaObjIndex(g_KanturuMonsterMng.GetMayaObjIndex());
	this->SetAIMonsterGroup(this->m_iMayaHandGroupNumber[1]);
	this->m_iMayaRightHandObjIndex = TMonsterAIGroup::FindGroupLeader(this->m_iMayaHandGroupNumber[1]);
}

void CKanturuBattleOfMaya::SetState_END_MAYA2()
{
	this->SetBattleSceneState(-1);
	this->SetBattleOfMayaState(KANTURU_MAYA_END2);
	this->SetEntrancePermit(FALSE);
	
	KANTURU_UTIL.NotifyKanturuSuccessValue(this->GetSuccessValue());
}

void CKanturuBattleOfMaya::SetState_ENDCYCLE_MAYA2()
{
	this->SetBattleOfMayaState(KANTURU_MAYA_ENDCYCLE2);
	this->SetEntrancePermit(FALSE);
	
	KANTURU_UTIL.SendMsgKauturuMapUser(Lang.GetText(0,263));
}


void CKanturuBattleOfMaya::SetState_STANBY3()
{
	this->SetBattleOfMayaState(KANTURU_MAYA_STANBY3);
	this->SetEntrancePermit(TRUE);
}


void CKanturuBattleOfMaya::SetState_START3()
{
	this->SetBattleOfMayaState(KANTURU_MAYA_START3);
	this->SetBattleSceneState(2);
	this->SetEntrancePermit(FALSE);
	this->SetSuccessValue(FALSE);
	this->SetSceneSuccessValue(FALSE);
	this->m_BattleTimeInfo[this->m_iSceneState].SetAppliedTime();

	if ( this->m_BattleTimeInfo[this->m_iSceneState].GetCondition() == 1 )
	{
		KANTURU_UTIL.NotifyKanturuBattleTime(this->m_BattleTimeInfo[this->m_iSceneState].GetValue());
	}

	g_KanturuMonsterMng.SetKanturuMonster(1);
	this->m_KanturuMaya.SetMayaObjIndex(g_KanturuMonsterMng.GetMayaObjIndex());
}


void CKanturuBattleOfMaya::SetState_MAYA3()
{
	this->SetBattleOfMayaState(KANTURU_MAYA3);
	this->SetEntrancePermit(FALSE);
	this->SetSuccessValue(FALSE);
	this->SetSceneSuccessValue(FALSE);
	this->m_iMayaHandDieTimeCounter = 0;
	g_KanturuMonsterMng.SetKanturuMonster(5);
	this->m_KanturuMaya.SetMayaObjIndex(g_KanturuMonsterMng.GetMayaObjIndex());
	this->SetAIMonsterGroup(this->m_iMayaHandGroupNumber[2]);
	this->m_iMayaLeftHandObjIndex = TMonsterAIGroup::FindGroupLeader(this->m_iMayaHandGroupNumber[2]);
	this->m_iMayaRightHandObjIndex = TMonsterAIGroup::FindGroupMemberObjectIndex(this->m_iMayaHandGroupNumber[2], 1);
}

void CKanturuBattleOfMaya::SetState_END_MAYA3()
{
	this->SetBattleSceneState(-1);
	this->SetBattleOfMayaState(KANTURU_MAYA_END3);
	this->SetEntrancePermit(FALSE);
	
	KANTURU_UTIL.NotifyKanturuSuccessValue(this->GetSuccessValue());
}

void CKanturuBattleOfMaya::SetState_ENDCYCLE_MAYA3()
{
	this->SetBattleOfMayaState(KANTURU_MAYA_ENDCYCLE3);
	this->SetEntrancePermit(FALSE);
}


void CKanturuBattleOfMaya::SetState_END()
{
	this->SetBattleSceneState(-1);
	this->SetBattleOfMayaState(KANTURU_MAYA_END);
	this->SetEntrancePermit(FALSE);

	if ( this->GetSuccessValue() == TRUE )
	{
		KANTURU_UTIL.SendMsgKanturuBattleUser(Lang.GetText(0,264));
	}
	else
	{
		KANTURU_UTIL.NotifyKanturuSuccessValue(this->GetSuccessValue());
		KANTURU_UTIL.SendMsgKanturuBattleUser(Lang.GetText(0,266));
	}
	
}


void CKanturuBattleOfMaya::SetState_ENDCYCLE()
{
	this->SetBattleOfMayaState(KANTURU_MAYA_ENDCYCLE);
	this->SetEntrancePermit(FALSE);

	if ( this->GetSuccessValue() == FALSE )
	{
		g_KanturuBattleUserMng.MoveAllUser(137);
		g_KanturuBattleUserMng.ResetAllData();
	}
}



void CKanturuBattleOfMaya::ProcState_NONE()
{
	return;
}

void CKanturuBattleOfMaya::ProcState_STANBY1()
{
	return;
}

void CKanturuBattleOfMaya::ProcState_NOTIFY1()
{
	return;
}

void CKanturuBattleOfMaya::ProcState_START1()
{
	if ( g_KanturuMonsterMng.IsExistAliveMonster() == FALSE )
	{
		this->SetSuccessValue(TRUE);

		this->SetNextState(this->m_iBattleOfMayaState);
	}
	else
	{
		this->m_KanturuMaya.KanturuMayaAct_IceStorm(this->m_iMayaIceStormUsingRate);
		KANTURU_UTIL.NotifyKanturuUserMonsterCount(g_KanturuMonsterMng.GetAliveMonsterCount(),
			g_KanturuBattleUserMng.GetUserCount());
	}
}

void CKanturuBattleOfMaya::ProcState_MAYA1()
{
	if ( ObjectMaxRange(this->m_iMayaLeftHandObjIndex) == FALSE)
	{
		return;
	}

	if ( gObj[this->m_iMayaLeftHandObjIndex].Live == FALSE )
	{
		if ( this->m_iMayaHandDieTimeCounter == 0 )
		{
			this->SetSuccessValue(TRUE);
			this->SetSceneSuccessValue(TRUE);

			g_KanturuBattleUserMng.LogBattleWinnerUserInfo(0,
				this->m_BattleTimeInfo[this->m_iSceneState].GetElapsedTime());

			KANTURU_UTIL.NotifyKanturuUserMonsterCount(0,
				g_KanturuBattleUserMng.GetUserCount());

			this->m_iMayaHandDieTimeCounter = GetTickCount();
		}

		if ( (GetTickCount() - this->m_iMayaHandDieTimeCounter ) >= 3000 )
		{
			this->SetNextState(this->m_iBattleOfMayaState);
		}
	}
	else
	{
		KANTURU_UTIL.NotifyKanturuUserMonsterCount(1,
			g_KanturuBattleUserMng.GetUserCount());

		this->ChangeAI(this->m_iMayaHandGroupNumber[0]);
		this->m_KanturuMaya.KanturuMayaAct_Hands();
	}
}


void CKanturuBattleOfMaya::ProcState_END_MAYA1()
{
	return;
}

void CKanturuBattleOfMaya::ProcState_ENDCYCLE_MAYA1()
{
	return;
}

void CKanturuBattleOfMaya::ProcState_STANBY2()
{
	return;
}


void CKanturuBattleOfMaya::ProcState_START2()
{
	if ( g_KanturuMonsterMng.IsExistAliveMonster() == FALSE )
	{
		this->SetSuccessValue(TRUE);

		this->SetNextState(this->m_iBattleOfMayaState);
	}
	else
	{
		this->m_KanturuMaya.KanturuMayaAct_IceStorm(this->m_iMayaIceStormUsingRate);
		KANTURU_UTIL.NotifyKanturuUserMonsterCount(g_KanturuMonsterMng.GetAliveMonsterCount(),
			g_KanturuBattleUserMng.GetUserCount());
	}
}


void CKanturuBattleOfMaya::ProcState_MAYA2()
{
	if ( ObjectMaxRange(this->m_iMayaRightHandObjIndex) == FALSE)
	{
		return;
	}

	if ( gObj[this->m_iMayaRightHandObjIndex].Live == FALSE )
	{
		if ( this->m_iMayaHandDieTimeCounter == 0 )
		{
			this->SetSuccessValue(TRUE);
			this->SetSceneSuccessValue(TRUE);

			g_KanturuBattleUserMng.LogBattleWinnerUserInfo(1,
				this->m_BattleTimeInfo[this->m_iSceneState].GetElapsedTime());

			KANTURU_UTIL.NotifyKanturuUserMonsterCount(0,
				g_KanturuBattleUserMng.GetUserCount());

			this->m_iMayaHandDieTimeCounter = GetTickCount();
		}

		if ( (GetTickCount() - this->m_iMayaHandDieTimeCounter ) >= 3000 )
		{
			this->SetNextState(this->m_iBattleOfMayaState);
		}
	}
	else
	{
		KANTURU_UTIL.NotifyKanturuUserMonsterCount(1,
			g_KanturuBattleUserMng.GetUserCount());

		this->ChangeAI(this->m_iMayaHandGroupNumber[1]);
		this->m_KanturuMaya.KanturuMayaAct_Hands();
	}
}

void CKanturuBattleOfMaya::ProcState_END_MAYA2()
{
	return;
}

void CKanturuBattleOfMaya::ProcState_ENDCYCLE_MAYA2()
{
	return;
}

void CKanturuBattleOfMaya::ProcState_STANBY3()
{
	return;
}

void CKanturuBattleOfMaya::ProcState_START3()
{
	if ( g_KanturuMonsterMng.IsExistAliveMonster() == FALSE )
	{
		this->SetSuccessValue(TRUE);

		this->SetNextState(this->m_iBattleOfMayaState);
	}
	else
	{
		this->m_KanturuMaya.KanturuMayaAct_IceStorm(this->m_iMayaIceStormUsingRate);
		KANTURU_UTIL.NotifyKanturuUserMonsterCount(g_KanturuMonsterMng.GetAliveMonsterCount(),
			g_KanturuBattleUserMng.GetUserCount());
	}
}


void CKanturuBattleOfMaya::ProcState_MAYA3()
{
	int iAliveCount = 0;

	if ( ObjectMaxRange(this->m_iMayaLeftHandObjIndex) == FALSE)
	{
		return;
	}

	if ( ObjectMaxRange(this->m_iMayaRightHandObjIndex) == FALSE)
	{
		return;
	}

	if ( gObj[this->m_iMayaLeftHandObjIndex].Live != FALSE )
	{
		iAliveCount++;
	}

	if ( gObj[this->m_iMayaRightHandObjIndex].Live != FALSE )
	{
		iAliveCount++;
	}

	if ( iAliveCount == 0 )
	{
		if ( this->m_iMayaHandDieTimeCounter == 0 )
		{
			this->SetSuccessValue(TRUE);
			this->SetSceneSuccessValue(TRUE);

			g_KanturuBattleUserMng.LogBattleWinnerUserInfo(2,
				this->m_BattleTimeInfo[this->m_iSceneState].GetElapsedTime());

			this->m_iMayaHandDieTimeCounter = GetTickCount();
		}

		if ( (GetTickCount() - this->m_iMayaHandDieTimeCounter ) >= 3000 )
		{
			this->SetNextState(this->m_iBattleOfMayaState);
			return;
		}
	}

	KANTURU_UTIL.NotifyKanturuUserMonsterCount(iAliveCount,
		g_KanturuBattleUserMng.GetUserCount());

	this->ChangeAI(this->m_iMayaHandGroupNumber[2]);
	this->m_KanturuMaya.KanturuMayaAct_Hands();
}

void CKanturuBattleOfMaya::ProcState_END_MAYA3()
{
	return;
}

void CKanturuBattleOfMaya::ProcState_ENDCYCLE_MAYA3()
{
	return;
}

void CKanturuBattleOfMaya::ProcState_END()
{
	return;
}

void CKanturuBattleOfMaya::ProcState_ENDCYCLE()
{
	return;
}

BOOL CKanturuBattleOfMaya::CheckStateTime()
{
	if ( this->GetBattleOfMayaState() )
	{
		if ( this->m_StateInfo[this->m_iBattleOfMayaState].GetCondition() == 1 )
		{
			if ( this->m_StateInfo[this->m_iBattleOfMayaState].IsTimeOut() == TRUE )
			{
				this->SetNextState(this->m_iBattleOfMayaState);
			}
		}
	}

	if ( this->GetBattleSceneState() != -1 )
	{
		if ( this->m_BattleTimeInfo[this->m_iSceneState].GetCondition() != 1 )
		{
			return TRUE;
		}

		if ( this->GetSceneSuccessValue() == TRUE )
		{
			return TRUE;
		}

		if ( this->m_BattleTimeInfo[this->m_iSceneState].IsTimeOut() == TRUE )
		{
			if ( this->GetBattleOfMayaState() != KANTURU_MAYA_END )
			{
				this->SetSceneSuccessValue(FALSE);
				this->SetSuccessValue(FALSE);

				this->SetState(KANTURU_MAYA_END);
			}
		}
	}

	return TRUE;
}


int CKanturuBattleOfMaya::GetRemainTime()
{
	int iCurrentState = this->GetBattleOfMayaState();
	return this->m_StateInfo[iCurrentState].GetRemainTime();
}

void CKanturuBattleOfMaya::CheckMayaBattleUser()
{
	if ( this->GetBattleOfMayaState() == KANTURU_MAYA_NOTIFY1 ||
		 this->GetBattleOfMayaState() == KANTURU_MAYA_START1 ||
		 this->GetBattleOfMayaState() == KANTURU_MAYA1 ||
		 this->GetBattleOfMayaState() == KANTURU_MAYA_START2 ||
		 this->GetBattleOfMayaState() == KANTURU_MAYA2 ||
		 this->GetBattleOfMayaState() == KANTURU_MAYA_START3 ||
		 this->GetBattleOfMayaState() == KANTURU_MAYA3 )
	{
		if ( g_KanturuBattleUserMng.IsEmpty() == TRUE )
		{
			this->SetSuccessValue(FALSE);
			this->SetSceneSuccessValue(FALSE);

			this->SetState(KANTURU_MAYA_END);
		}
	}
}


void CKanturuBattleOfMaya::SetAIMonsterGroup(int iGroupNumber)
{
	TMonsterAIGroup::Init(iGroupNumber);
	TMonsterAIGroup::ChangeAIOrder(iGroupNumber, 0);

	this->m_iMayaHandAIAppliedTime = GetTickCount();
}

void CKanturuBattleOfMaya::ResetAIMonster()
{
	if(this->m_iMayaLeftHandObjIndex != -1 && this->m_iMayaRightHandObjIndex != -1)
	{
		gObj[this->m_iMayaLeftHandObjIndex].Life = 0;
		gObj[this->m_iMayaRightHandObjIndex].Life = 0;
	}

	for ( int iCount=0;iCount < KANTURU_MAYA_AI_CHANGE_TIME ; iCount++)
	{
		TMonsterAIGroup::DelGroupInstance(this->m_iMayaHandGroupNumber[iCount]);
	}

	this->m_iMayaLeftHandObjIndex = -1;
	this->m_iMayaRightHandObjIndex = -1;
	this->m_iMayaHandAIAppliedTime = 0;
	this->m_iMayaHandCurrentAI = 0;
}


void CKanturuBattleOfMaya::ChangeAI(int iGroupNumber)
{
	if ( (GetTickCount() - this->m_iMayaHandAIAppliedTime) >= this->m_iMayaHandAIChangeTime[this->m_iMayaHandCurrentAI] )
	{
		this->m_iMayaHandAIAppliedTime = GetTickCount();

		if ( this->m_iMayaHandCurrentAI != KANTURU_MAYA_GROUP_NUMBER )
		{
			this->m_iMayaHandCurrentAI++;
			TMonsterAIGroup::ChangeAIOrder(iGroupNumber, this->m_iMayaHandCurrentAI);
		}
	}
}



////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

