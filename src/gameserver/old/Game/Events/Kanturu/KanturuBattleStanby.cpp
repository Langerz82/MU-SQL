// KanturuBattleStanby.cpp: implementation of the CKanturuBattleStanby class.
//	GS-N	1.00.18	JPN	0x0057E8B0	-	Completed
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KanturuBattleStanby.h"
#include "Kanturu.h"
#include "KanturuMonsterMng.h"
#include "KanturuBattleUserMng.h"
#include "Main.h"
#include "Logging/Log.h"
#include "KanturuUtil.h"
#include "configread.h"

static CKanturuUtil KANTURU_UTIL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKanturuBattleStanby::CKanturuBattleStanby()
{
	this->SetBattleStanbyState(KANTURU_STANBY_NONE);
}

CKanturuBattleStanby::~CKanturuBattleStanby()
{
	return;
}


void CKanturuBattleStanby::Init()
{
	return;
}


void CKanturuBattleStanby::ResetAllData()
{
	for ( int iCount=0;iCount < KANTURU_STANBY_STATE_INFO ; iCount++ )
	{
		this->m_StateInfo[iCount].ResetTimeInfo();
	}

	this->m_StateInfoCount = 0;
}



BOOL CKanturuBattleStanby::LoadData(LPSTR lpszFileName)
{
	this->m_bFileDataLoad = FALSE;

	if ( !lpszFileName || !strcmp(lpszFileName, ""))
	{
		sLog->outError("[Kanturu][Battle Standby] - File load error : File Name Error");
		return FALSE;
	}

	try
	{
		pugi::xml_document file;
		pugi::xml_parse_result res = file.load_file(lpszFileName);

		if ( res.status != pugi::status_ok )
		{
			sLog->outError("[Kanturu][Battle Standby] - Can't Load %s (%s)", lpszFileName, res.description());
			return FALSE;
		}

		this->ResetAllData();

		pugi::xml_node mainXML = file.child("KanturuEvent");
		pugi::xml_node battle_standby = mainXML.child("BattleStandBy");

		pugi::xml_node state = battle_standby.child("State");

		for (pugi::xml_node cycle = state.child("Cycle"); cycle; cycle = cycle.next_sibling())
		{
			int iState = cycle.attribute("State").as_int();
			int iCondition = cycle.attribute("SwitchType").as_int();
			int iValue = cycle.attribute("Time").as_int();

			if ( this->m_StateInfoCount < 0 || this->m_StateInfoCount >= KANTURU_STANBY_STATE_INFO )
			{
				sLog->outError("[Kanturu][Battle Standby] - Exceed Max State Time (%d)", this->m_StateInfoCount);
				break;
			}

			this->m_StateInfo[this->m_StateInfoCount].SetStateInfo(iState);
			this->m_StateInfo[this->m_StateInfoCount].SetCondition(iCondition);

			if ( iCondition == 1 )
				iValue *= 1000;

			this->m_StateInfo[this->m_StateInfoCount].SetValue(iValue);
			this->m_StateInfoCount++;
		}

		this->m_bFileDataLoad = TRUE;
	}

	catch (DWORD)
	{
		sLog->outError("[Kanturu][Battle Standby] - Loading Exception Error (%s) File. ", lpszFileName);
	}

	return this->m_bFileDataLoad;
}


void CKanturuBattleStanby::Run()
{
	this->CheckStateTime();

	switch ( this->m_iBattleStanbyState )
	{
		case KANTURU_STANBY_NONE:
			this->ProcState_NONE();
			break;
		case KANTURU_STANBY_START:
			this->ProcState_START();
			break;
		case KANTURU_STANBY_NOTIFY:
			this->ProcState_NOTIFY();
			break;
		case KANTURU_STANBY_END:
			this->ProcState_END();
			break;
	}
}


void CKanturuBattleStanby::SetState(int iBattleStanbyState)
{
	if ( this->m_StateInfo[iBattleStanbyState].GetCondition() > 0 )
	{
		this->m_StateInfo[iBattleStanbyState].SetAppliedTime();
	}

	g_KanturuMonsterMng.ResetRegenMonsterObjData();

	for ( int iCount= g_ConfigRead.server.GetObjectStartUserIndex();iCount<g_ConfigRead.server.GetObjectMax();iCount++)
	{
		if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
		{
			getGameObject(iCount)->m_PlayerData->m_bKanturuEntranceByNPC = FALSE;
		}
	}

	g_KanturuBattleUserMng.ResetAllData();

	switch ( iBattleStanbyState )
	{
		case KANTURU_STANBY_NONE:
			this->SetState_NONE();
			break;
		case KANTURU_STANBY_START:
			this->SetState_START();
			break;
		case KANTURU_STANBY_NOTIFY:
			this->SetState_NOTIFY();
			break;
		case KANTURU_STANBY_END:
			this->SetState_END();
			break;
	}

	KANTURU_UTIL.NotifyKanturuChangeState(KANTURU_STATE_BATTLE_STANTBY, this->m_iBattleStanbyState);
	g_KanturuBattleUserMng.ResetAllData();
}


void CKanturuBattleStanby::SetNextState(int iCurrentState)
{
	int iNextState = iCurrentState + 1;

	if ( iNextState >= KANTURU_STANBY_STATE_INFO )
	{
		iNextState = KANTURU_STANBY_NONE;
	}

	this->SetState(iNextState);
}


void CKanturuBattleStanby::SetState_NONE()
{
	this->SetBattleStanbyState(KANTURU_STANBY_NONE);
}


void CKanturuBattleStanby::SetState_START()
{
	this->SetBattleStanbyState(KANTURU_STANBY_START);
}

void CKanturuBattleStanby::SetState_NOTIFY()
{
	this->SetBattleStanbyState(KANTURU_STANBY_NOTIFY);
	KANTURU_UTIL.SendMsgKauturuMapUser(Lang.GetText(0,262));
}

void CKanturuBattleStanby::SetState_END()
{
	this->SetBattleStanbyState(KANTURU_STANBY_END);
}

void CKanturuBattleStanby::ProcState_NONE()
{
	return;
}

void CKanturuBattleStanby::ProcState_START()
{
	return;
}

void CKanturuBattleStanby::ProcState_NOTIFY()
{
	return;
}

void CKanturuBattleStanby::ProcState_END()
{
	return;
}


void CKanturuBattleStanby::CheckStateTime()
{
	int iCurrentState = this->GetBattleStanbyState();

	if ( iCurrentState )
	{
		if ( this->m_StateInfo[iCurrentState].GetCondition() == 1 )
		{
			if ( this->m_StateInfo[iCurrentState].IsTimeOut() == TRUE )
			{
				this->SetNextState(iCurrentState);
			}
		}
	}
}


int CKanturuBattleStanby::GetRemainTime()
{
	int iCurrentState = this->GetBattleStanbyState();
	return this->m_StateInfo[iCurrentState].GetRemainTime();
}


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

