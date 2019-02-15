////////////////////////////////////////////////////////////////////////////////
// ChaosCastleFinal.cpp
#include "StdAfx.h"
#include "ChaosCastleFinal.h"
#include "GameMain.h"
#include "GameProtocol.h"
#include "TNotice.h"
#include "User/CUserData.h"
#include "BloodCastle.h"
#include "LargeRand.h"
#include "BuffEffectSlot.h"
#include "CrywolfSync.h"
#include "MasterLevelSkillTreeSystem.h"
#include "MapServerManager.h"
#include "GremoryCase.h"
#include "ItemOptionTypeMng.h"
#include "CustomMaxStats.h"

ChaosCastleFinal g_ChaosCastleFinal;

BYTE ChaosCastleFinal::m_byCCFTimer;
bool ChaosCastleFinal::m_bCCFDayChange;

ChaosCastleFinal::ChaosCastleFinal()
{
	this->m_FirstRankingInfoLoad = false;
	this->m_bCCF_EVENT_ENABLE = false;
	this->m_bFinalDayEnd = 0;
	this->m_iCCF_TIME_MIN_ALL_NOTIFY = 15;
	this->m_iCCF_TIME_MIN_OPEN = 10;
	this->m_bIsCCF_Ranking_Renew = false;
	this->m_iCCF_BoxRatio1 = 30;
	this->m_iCCF_BoxRatio2 = 45;
	this->m_iCCF_BoxRatio3 = 60;
	this->m_iCCF_BoxRatio4 = 75;
	this->m_iCCF_BoxRatio5 = 90;
	this->m_iCCF_BoxRatio6 = 100;
	this->m_byBoxDropCnt = 0;
	this->m_CCF_MOB_CNT_SCRIPT = 0;
	this->m_stCCFData.m_byCCF_TYPE = 0;
	this->m_stCCFData.m_nCCF_STATE = 0;
	this->m_stCCFData.m_nMapNumber = MAP_INDEX_CHAOSCASTLE_SURVIVAL;
	this->m_stCCFData.m_nCCF_REMAIN_MSEC = -1;
	this->m_stCCFData.m_nCCF_TICK_COUNT = -1;
	this->m_stCCFData.m_fRewardExpRate = 1.0;
	this->m_byTryOut1 = 0;
	this->m_byTryOut2 = 0;
	this->m_byTryOut3 = 0;
	this->m_bySemiFinal1 = 0;
	this->m_bySemiFinal2 = 0;
	this->m_byFinal = 0;
	this->m_bCCF_CHEAT_BLOW = true;
	this->m_bCCF_CHEAT_TRAP = true;

	for (int i = 0; i < 50; i++)
	{
		this->m_CCFRanking[i].byRank = 0;
		this->m_CCFRanking[i].nPoint = 0;
		memset(this->m_CCFRanking[i].szCharName, 0, MAX_ACCOUNT_LEN + 1);
	}
	
	this->ClearCCFData();
	srand(time(NULL));

	this->m_nCCF_MinUserCount = 5;
	this->m_nCCF_Level = 280;
}

ChaosCastleFinal::~ChaosCastleFinal()
{
	
}

void ChaosCastleFinal::Run()
{
	if (this->m_bCCF_EVENT_ENABLE == false)
	{
		return;
	}

	switch (this->m_stCCFData.m_nCCF_STATE)
	{
		case 0:
			this->ProcState_None();
			break;
		case 1:
			this->ProcState_Closed();
			break;
		case 2:
			this->ProcState_Playing();
			break;
		case 3:
			this->ProcState_PlayEnd();
			break;
		default: 
			return;
	}
}

void ChaosCastleFinal::Init()
{
	if (g_ConfigRead.server.GetStateFromEventTable(g_ConfigRead.server.GetServerType(), EV_TABLE_CCF) == false)
	{
		this->m_bCCF_EVENT_ENABLE = false;
		return;
	}

	this->SetOpenTodaySchedule();
	this->SetState(1, 0);

	for (int iTRAP_STEP = 0; iTRAP_STEP < 4; iTRAP_STEP++)
	{
		g_rt_CCF_POINT_FRAME[0].left = (::g_iCCF_GroundAxis[2] + ::g_iCCF_GroundAxis[0]) / 2 + 1;
		g_rt_CCF_POINT_FRAME[0].top = g_iCCF_GroundAxis[1];
		g_rt_CCF_POINT_FRAME[0].right = ::g_iCCF_GroundAxis[2];
		g_rt_CCF_POINT_FRAME[0].bottom = (::g_iCCF_GroundAxis[3] + ::g_iCCF_GroundAxis[1]) / 2;

		g_rt_CCF_POINT_FRAME[1].left = (::g_iCCF_GroundAxis[2] + ::g_iCCF_GroundAxis[0]) / 2 + 1;
		g_rt_CCF_POINT_FRAME[1].top = (::g_iCCF_GroundAxis[3] + ::g_iCCF_GroundAxis[1]) / 2 + 1;
		g_rt_CCF_POINT_FRAME[1].right = g_iCCF_GroundAxis[2];
		g_rt_CCF_POINT_FRAME[1].bottom = ::g_iCCF_GroundAxis[3];

		g_rt_CCF_POINT_FRAME[2].left = ::g_iCCF_GroundAxis[0];
		g_rt_CCF_POINT_FRAME[2].top = (::g_iCCF_GroundAxis[3] + ::g_iCCF_GroundAxis[1]) / 2 + 1;
		g_rt_CCF_POINT_FRAME[2].right = (::g_iCCF_GroundAxis[2] + ::g_iCCF_GroundAxis[0]) / 2;
		g_rt_CCF_POINT_FRAME[2].bottom = ::g_iCCF_GroundAxis[3];

		g_rt_CCF_POINT_FRAME[3].left = ::g_iCCF_GroundAxis[0];
		g_rt_CCF_POINT_FRAME[3].top = ::g_iCCF_GroundAxis[1];
		g_rt_CCF_POINT_FRAME[3].right = (::g_iCCF_GroundAxis[2] + ::g_iCCF_GroundAxis[0]) / 2;
		g_rt_CCF_POINT_FRAME[3].bottom = (::g_iCCF_GroundAxis[3] + ::g_iCCF_GroundAxis[1]) / 2;

		g_rt_CCF_POINT_TRAP[0].left = (::g_iCCF_DamageAxis[iTRAP_STEP][2] + ::g_iCCF_DamageAxis[iTRAP_STEP][0]) / 2 + 1;
		g_rt_CCF_POINT_TRAP[0].top = ::g_iCCF_DamageAxis[iTRAP_STEP][1];
		g_rt_CCF_POINT_TRAP[0].right = ::g_iCCF_DamageAxis[iTRAP_STEP][2];
		g_rt_CCF_POINT_TRAP[0].bottom = (::g_iCCF_DamageAxis[iTRAP_STEP][3] + ::g_iCCF_DamageAxis[iTRAP_STEP][1]) / 2;

		g_rt_CCF_POINT_TRAP[1].left = (::g_iCCF_DamageAxis[iTRAP_STEP][2] + ::g_iCCF_DamageAxis[iTRAP_STEP][0]) / 2 + 1;
		g_rt_CCF_POINT_TRAP[1].top = (::g_iCCF_DamageAxis[iTRAP_STEP][3] + ::g_iCCF_DamageAxis[iTRAP_STEP][1]) / 2 + 1;
		g_rt_CCF_POINT_TRAP[1].right = g_iCCF_DamageAxis[iTRAP_STEP][2];
		g_rt_CCF_POINT_TRAP[1].bottom = ::g_iCCF_DamageAxis[iTRAP_STEP][3];

		g_rt_CCF_POINT_TRAP[2].left = ::g_iCCF_DamageAxis[iTRAP_STEP][0];
		g_rt_CCF_POINT_TRAP[2].top = (::g_iCCF_DamageAxis[iTRAP_STEP][3] + ::g_iCCF_DamageAxis[iTRAP_STEP][1]) / 2 + 1;
		g_rt_CCF_POINT_TRAP[2].right = (::g_iCCF_DamageAxis[iTRAP_STEP][2] + ::g_iCCF_DamageAxis[iTRAP_STEP][0]) / 2;
		g_rt_CCF_POINT_TRAP[2].bottom = ::g_iCCF_DamageAxis[iTRAP_STEP][3];

		g_rt_CCF_POINT_TRAP[3].left = ::g_iCCF_DamageAxis[iTRAP_STEP][0];
		g_rt_CCF_POINT_TRAP[3].top = ::g_iCCF_DamageAxis[iTRAP_STEP][1];
		g_rt_CCF_POINT_TRAP[3].right = (::g_iCCF_DamageAxis[iTRAP_STEP][2] + ::g_iCCF_DamageAxis[iTRAP_STEP][0]) / 2;
		g_rt_CCF_POINT_TRAP[3].bottom = (::g_iCCF_DamageAxis[iTRAP_STEP][3] + ::g_iCCF_DamageAxis[iTRAP_STEP][1]) / 2;
	}
}

void ChaosCastleFinal::Load_CCFMonster(char *Filename)
{
	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(Filename);

	if (res.status != pugi::status_ok)
	{
		sLog->outError("[ChaosCastleFinal] SetBase File Load Fail %s (%s)", Filename, res.description());
		return;
	}

	pugi::xml_node mainXML_section = file.child("ChaosCastleSurvival");
	int nCount = 0;

	for (pugi::xml_node monster = main_section.child("Monster"); monster; monster = monster.next_sibling(), nCount++)
	{
		this->m_CCFMP[nCount].m_Type = monster.attribute("Index").as_int();
		this->m_CCFMP[nCount].m_MapNumber = monster.attribute("MapNumber").as_int();
		this->m_CCFMP[nCount].m_Dis = monster.attribute("Distance").as_int();
		this->m_CCFMP[nCount].m_X = monster.attribute("StartX").as_int();
		this->m_CCFMP[nCount].m_Y = monster.attribute("StartY").as_int();
		this->m_CCFMP[nCount].m_Dir = monster.attribute("Dir").as_int();
	}

	if (nCount == 100 || nCount < 0)
	{
		sLog->outBasic("[ChaosCastleFinal][LoadMonster] Monster Count Invalid");
		nCount = 100;
	}

	this->m_CCF_MOB_CNT_SCRIPT = nCount;
}

void ChaosCastleFinal::Load_CCFInfo(char *Filename)
{
	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(Filename);

	if (res.status != pugi::status_ok)
	{
		sLog->outError("[ChaosCastleFinal] Info File Load Fail %s (%s)", Filename, res.description());
		return;
	}

	int nTimeTableCnt = 0;
	this->m_vtCCFTime.clear();

	pugi::xml_node mainXML_section = file.child("ChaosCastleSurvival");
	pugi::xml_node time = main_section.child("Time");
	pugi::xml_node tournament = main_section.child("TournamentCycleDay");
	pugi::xml_node rewardrate = main_section.child("RewardRate");
	pugi::xml_node eventschedule = main_section.child("EventSchedule");

	this->m_bCCF_EVENT_ENABLE = g_ConfigRead.server.GetStateFromEventTable(g_ConfigRead.server.GetServerType(), EV_TABLE_CCF) == true ? main_section.attribute("Enable").as_int() : false;
	this->m_stCCFData.m_fRewardExpRate = main_section.attribute("ExpMultipler").as_float();
	this->m_nCCF_MinUserCount = main_section.attribute("MinPlayer").as_int();
	this->m_nCCF_Level = main_section.attribute("MinLevel").as_int();

	this->m_iCCF_TIME_MIN_OPEN = time.attribute("StartIn").as_int();
	this->m_iCCF_TIME_MIN_PLAY = time.attribute("Duration").as_int();
	this->m_iCCF_TIME_MIN_REST = time.attribute("TownSpawn").as_int();
	this->m_iCCF_TIME_MIN_ALL_NOTIFY = time.attribute("Notify").as_int();

	this->m_byTryOut1 = tournament.attribute("EliminationOne").as_int();
	this->m_byTryOut2 = tournament.attribute("EliminationTwo").as_int();
	this->m_byTryOut3 = tournament.attribute("EliminationThree").as_int();
	this->m_bySemiFinal1 = tournament.attribute("SemiFinalOne").as_int();
	this->m_bySemiFinal2 = tournament.attribute("SemiFinalTwo").as_int();
	this->m_byFinal = tournament.attribute("Final").as_int();

	pugi::xml_node gift = rewardrate.child("Gift");
	pugi::xml_node finalaward = rewardrate.child("FinalAward");

	this->m_iCCF_BoxRatio1 = gift.attribute("First").as_int();
	this->m_iCCF_BoxRatio2 = gift.attribute("Second").as_int();
	this->m_iCCF_BoxRatio3 = gift.attribute("Third").as_int();
	this->m_iCCF_BoxRatio4 = gift.attribute("Fourth").as_int();
	this->m_iCCF_BoxRatio5 = gift.attribute("Fifth").as_int();
	this->m_iCCF_BoxRatio6 = gift.attribute("Sixth").as_int();

	this->m_stCCFFinalReward.m_btCat = finalaward.attribute("Cat").as_int(13);
	this->m_stCCFFinalReward.m_wIndex = finalaward.attribute("Index").as_int(276);
	this->m_stCCFFinalReward.m_btLevel = finalaward.attribute("Level").as_int(0);
	this->m_stCCFFinalReward.m_btDurability = finalaward.attribute("Durability").as_int(0);
	this->m_stCCFFinalReward.m_btSkill = finalaward.attribute("Skill").as_int(0);
	this->m_stCCFFinalReward.m_btLuck = finalaward.attribute("Luck").as_int(0);
	this->m_stCCFFinalReward.m_btOption = finalaward.attribute("Option").as_int(0);

	std::string szExcOptions = finalaward.attribute("Exc").as_string();
	std::deque<int> exc_opt_list;
	strtk::parse(szExcOptions, ";", exc_opt_list);

	memset(this->m_stCCFFinalReward.m_btNewOption, -1, sizeof(this->m_stCCFFinalReward.m_btNewOption));

	for (int i = 0; i < exc_opt_list.size(); i++)
	{
		this->m_stCCFFinalReward.m_btNewOption[i] = exc_opt_list.at(i);
	}

	this->m_stCCFFinalReward.m_dwExpiryTime = finalaward.attribute("Duration").as_int(0);

	CCF_START_TIME pRET;

	for (pugi::xml_node start = eventschedule.child("Start"); start; start = start.next_sibling())
	{
		pRET.m_nHour = start.attribute("Hour").as_int();
		pRET.m_nMinute = start.attribute("Minute").as_int();
		pRET.m_nTryout = start.attribute("Stage1").as_int();
		pRET.m_nSemiFinal = start.attribute("Stage2").as_int();
		pRET.m_nFinal = start.attribute("Stage3").as_int();

		nTimeTableCnt++;

		if (nTimeTableCnt > 40)
		{
			sLog->outError("ChaosCastleFinal TimeTable MaxOver [%s] ", Filename);
			break;
		}

		this->m_vtCCFTime.push_back(pRET);
	}
}

void ChaosCastleFinal::ClearCCFData()
{
	this->m_stCCFData.Clear();

	for (int i = 0; i < 70; i++)
	{
		if (this->m_stCCFData.m_UserData[i].m_nIndex != -1)
		{
			getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->m_bCCF_UI_Using = false;
			getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->m_byCCF_CanEnter = 0;
			getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->m_nCCF_CertiTick = 0;
			getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->m_nCCF_UserIndex = -1;
			getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->m_nCCF_BlowTime = 0;
			getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->m_byKillUserCount = 0;
			getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->m_byKillMobCount = 0;
			getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->m_bCCF_Quit_Msg = false;
		}

		this->m_stCCFData.m_UserData[i].Clear();
	}
}

void ChaosCastleFinal::ProcState_None()
{
	return;
}

bool ChaosCastleFinal::CheckOpenOrNotOpenCCF_Final(BYTE bCCFType)
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	CCF_START_TIME pRET;

	int iMIN_HOUR = 24;
	int iMIN_MINUTE = 60;
	int Old_Hour = 0;
	int Old_Min = 0;

	for (std::vector<CCF_START_TIME>::iterator it = this->m_vtCCFOpenTime.begin(); it != this->m_vtCCFOpenTime.end(); it++)
	{
		pRET = *it;

		if (pRET.m_nFinal == 3
			&& sysTime.wMinute + 60 * sysTime.wHour > Old_Min + 60 * Old_Hour
			&& sysTime.wMinute + 60 * sysTime.wHour < pRET.m_nMinute + 60 * pRET.m_nHour)
		{
			return FALSE;
		}

		Old_Hour = pRET.m_nHour;
		Old_Min = pRET.m_nMinute;
	}

	return true;
}

void ChaosCastleFinal::ProcState_Closed()
{
	DWORD iTICK_MSEC = GetTickCount() - this->m_stCCFData.m_nCCF_TICK_COUNT;

	if (iTICK_MSEC >= 1000)
	{
		this->m_stCCFData.m_nCCF_REMAIN_MSEC -= iTICK_MSEC;
		this->m_stCCFData.m_nCCF_TICK_COUNT = GetTickCount();

		if (this->m_bCCF_EVENT_ENABLE)
		{
			if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE && this->Check_CCF_DayType())
			{
				BYTE BattleType = this->Check_CCF_DayType();
				bool IsNowFinal = false;

				if (BattleType == 3)
				{
					IsNowFinal = this->CheckOpenOrNotOpenCCF_Final(BattleType);
				}

				if (!IsNowFinal)
				{
					if (this->m_stCCFData.m_nCCF_REMAIN_MSEC <= 60000 * this->m_iCCF_TIME_MIN_ALL_NOTIFY && this->m_stCCFData.m_bCCF_CAN_ENTER == false && this->m_stCCFData.m_bCCF_MSG_ALL_NOTIFY == false)
					{
						sLog->outBasic("[Chaos Castle Survival] Notify = 1");

						this->m_stCCFData.m_bCCF_MSG_ALL_NOTIFY = true;
						this->GD_SendCCFInfoAllServer(this->m_iCCF_TIME_MIN_ALL_NOTIFY, this->Check_CCF_DayType());
						this->m_stCCFData.m_byCCF_TYPE = this->Check_CCF_DayType();
					}

					if (this->m_stCCFData.m_nCCF_REMAIN_MSEC <= 60000 * this->m_iCCF_TIME_MIN_OPEN && !this->m_stCCFData.m_bCCF_CAN_ENTER)
					{
						this->m_stCCFData.m_bCCF_CAN_ENTER = 1;
						this->m_stCCFData.m_bCCF_CAN_PARTY = 1;
						this->m_stCCFData.m_byCCF_TYPE = this->Check_CCF_DayType();

						sLog->outBasic("[Chaos Castle Survival] Opened.");
					}

					if (this->m_stCCFData.m_nCCF_REMAIN_MSEC <= 60000 * this->m_iCCF_TIME_MIN_OPEN && this->m_stCCFData.m_nCCF_REMAIN_MSEC > 0 && this->m_stCCFData.m_nCCF_REMAIN_MSEC / 60000 != this->m_stCCFData.m_nCCF_NOTIFY_COUNT)
					{
						this->m_stCCFData.m_nCCF_NOTIFY_COUNT = this->m_stCCFData.m_nCCF_REMAIN_MSEC / 60000;

						PMSG_NOTICE pNotice;
						TNotice::MakeNoticeMsgEx(&pNotice, 0, Lang.GetText(0,590), this->m_stCCFData.m_nCCF_NOTIFY_COUNT + 1);

						this->SendAllUserAnyMsg((BYTE*)&pNotice, pNotice.h.size);
						sLog->outBasic("[Chaos Castle Survival] Event starts in %d minutes", this->m_stCCFData.m_nCCF_NOTIFY_COUNT + 1);
					}

					if (this->m_stCCFData.m_nCCF_REMAIN_MSEC <= 30000 && this->m_stCCFData.m_nCCF_REMAIN_MSEC > 0 && !this->m_stCCFData.m_bCCF_MSG_BEFORE_ENTER)
					{
						this->m_stCCFData.m_bCCF_MSG_BEFORE_ENTER = TRUE;

						PMSG_SET_DEVILSQUARE pMsg;

						PHeadSetB((BYTE*)&pMsg, 0x92, sizeof(pMsg));
						pMsg.Type = 20;

						for (int n = g_ConfigRead.server.GetObjectStartUserIndex(); n < g_ConfigRead.server.GetObjectMax(); n++)
						{
							if (getGameObject(n)->Connected == PLAYER_PLAYING && getGameObject(n)->Type == OBJ_USER && getGameObject(n)->MapNumber != MAP_INDEX_CHAOSCASTLE_SURVIVAL)
							{
								IOCP.DataSend(n, (BYTE*)&pMsg, pMsg.h.size);
							}
						}
					}
				}
			}
		}
	}

	if (this->m_stCCFData.m_nCCF_REMAIN_MSEC <= 0)
	{
		if (this->m_bCCF_EVENT_ENABLE)
		{
			this->SetState(2, 0);
		}

		else
		{
			this->SetState(1, 0);
		}
	}
}

void ChaosCastleFinal::SetState(int iCCF_STATE, int nFlag)
{
	char * temp1;
	char * temp2;

	if (iCCF_STATE >= 0 && iCCF_STATE <= 3)
	{
		switch (iCCF_STATE)
		{
			case 0:
				temp1 = "None";
				break;
			case 1:
				temp1 = "Closed";
				break;
			case 2:
				temp1 = "Playing";
				break;
			case 3:
				temp1 = "PlayEnd";
				break;
		}

		switch (this->m_stCCFData.m_nCCF_STATE)
		{
			case 0:
				temp2 = "None";
				break;
			case 1:
				temp2 = "Closed";
				break;
			case 2:
				temp2 = "Playing";
				break;
			case 3:
				temp2 = "PlayEnd";
				break;
		}

		sLog->outBasic("[Chaos Castle Survival] Change State: %s -> %s", temp2, temp1);
		this->m_stCCFData.m_nCCF_STATE = iCCF_STATE;

		switch (this->m_stCCFData.m_nCCF_STATE)
		{
			case 0:
				this->SetState_None();
				break;
			case 1:
				this->SetState_Closed();
				break;
			case 2:
				this->SetState_Playing();
				break;
			case 3:
				this->SetState_PlayEnd(nFlag);
				break;
			default:
				return;
		}
	}
}

void ChaosCastleFinal::ProcState_Playing()
{
	DWORD iTICK_MSEC = GetTickCount() - this->m_stCCFData.m_nCCF_TICK_COUNT;

	if (iTICK_MSEC >= 1000)
	{
		this->m_stCCFData.m_nCCF_REMAIN_MSEC -= iTICK_MSEC;
		this->m_stCCFData.m_nCCF_TICK_COUNT = GetTickCount();

		if (this->m_stCCFData.m_nCCF_REMAIN_MSEC <= ((this->m_iCCF_TIME_MIN_PLAY * 60 - 30) * 1000) &&
			this->m_stCCFData.m_bCCF_MSG_BEFORE_PLAY == false)
		{
			this->m_stCCFData.m_bCCF_MSG_BEFORE_PLAY = true;

			PMSG_SET_DEVILSQUARE pMsg;
			PHeadSetB((BYTE*)&pMsg, 0x92, sizeof(pMsg));
			pMsg.Type = 21;
			this->SendChaosCastleAnyMsg((BYTE*)&pMsg, sizeof(pMsg));
		}

		if (this->m_stCCFData.m_nCCF_REMAIN_MSEC <= 30000 &&
			this->m_stCCFData.m_nCCF_REMAIN_MSEC > 0 &&
			this->m_stCCFData.m_bCCF_MSG_BEFORE_END == false)
		{
			this->m_stCCFData.m_bCCF_MSG_BEFORE_END = true;
			PMSG_SET_DEVILSQUARE pMsg;
			PHeadSetB((BYTE*)&pMsg, 0x92, sizeof(pMsg));
			pMsg.Type = 22;
			this->SendChaosCastleAnyMsg((BYTE*)&pMsg, sizeof(pMsg));
		}

		if (this->m_stCCFData.m_nCCF_REMAIN_MSEC <= ((this->m_iCCF_TIME_MIN_PLAY * 60 - 60) * 1000) &&
			this->m_stCCFData.m_bCCF_PLAY_START == false)
		{
			PMSG_NOTICE pNotice;

			this->m_stCCFData.m_nCCF_REMAIN_MSEC = (this->m_iCCF_TIME_MIN_PLAY * 60) * 1000;
			this->m_stCCFData.m_bCCF_CAN_PARTY = false;
			TNotice::MakeNoticeMsgEx((TNotice*)&pNotice, 0, Lang.GetText(0,591));
			this->SendChaosCastleAnyMsg((BYTE*)&pNotice, pNotice.h.size);
			this->UnSafetyCastleZone();
			this->SendCastleZoneSafetyInfo(0);
			this->m_stCCFData.m_bCCF_PLAY_START = true;
			this->m_stCCFData.m_nCCF_FIRST_USER_COUNT = this->ReCalcUserCount();
			this->m_stCCFData.m_lCCF_CURRENT_MONSTER_COUNT = this->SetMonster();
			this->m_stCCFData.m_nCCF_FIRST_MONSTER_COUNT = this->m_stCCFData.m_lCCF_CURRENT_MONSTER_COUNT;
			this->SetItemsToMonster(this->Check_CCF_DayType());
			this->SendNoticeState(5);
			this->m_bIsCCF_Ranking_Renew = false;

			sLog->outBasic("[Chaos Castle Survival] Started. (UserCount:%d)", this->GetCurPlayUser());
		}

		if (this->m_stCCFData.m_bCCF_PLAY_START == false &&
			this->m_stCCFData.m_bCCF_CAN_ENTER == false)
		{
			int iCurPlayUser = this->GetCurPlayUser();

			if (iCurPlayUser == 0)
			{
				this->SetState(1, 0);
				sLog->outBasic("[Chaos Castle Survival] Closed - No User");
				return;
			}
		}

		if (this->m_stCCFData.m_bCCF_PLAY_START == true)
		{
			this->ProcessFallUser();
			this->CheckUserInDieTile();
			this->CheckMonsterInDieTile();
			this->ProcessTrapStatus();

			if (this->m_stCCFData.m_nCCF_TRAP_STATUS != this->m_stCCFData.m_nCCF_TRAP_OLD_STATUS)
			{
				this->m_stCCFData.m_nCCF_TRAP_OLD_STATUS = this->m_stCCFData.m_nCCF_TRAP_STATUS;

				this->SetMapAttrHollow(this->m_stCCFData.m_nCCF_TRAP_STATUS);
				this->PullObjInnerPlace(this->m_stCCFData.m_nCCF_TRAP_STATUS);
			}

			int iCUR_PLAY_STATE = this->CheckPlayEnded();

			if (iCUR_PLAY_STATE == 1)
			{
				int iCCF_WINNER_INDEX = -1;
				int iCCF_LEFT_MONSTER_COUNT = this->GetMonsterListCount();

				if (iCCF_LEFT_MONSTER_COUNT > 0)
				{
					sLog->outBasic("[Chaos Castle Survival][CCS1] Has No Winner : Monster Left (%d)", this->m_stCCFData.m_lCCF_CURRENT_MONSTER_COUNT);
				}

				else
				{
					int nPoint = 0;
					int iCCF_WINNER_INDEX = this->GetCurrentCCFWinUser(&nPoint);

					if (iCCF_WINNER_INDEX == -1)
					{
						sLog->outBasic("[Chaos Castle Survival][CCS1] Has No Winner.");
					}

					else
					{
						sLog->outBasic("[Chaos Castle Survival][CCS1] [%s][%s][%s][ServerCode: %d] is Winner. [CharInfoSave : Class=%d, Level=%d, LvPoint=%d, Exp=%d, Str=%d, Dex=%d, Vit=%d, Energy=%d, LeaderShip=%d, Map=%d, Pk=%d]",
							getGameObject(iCCF_WINNER_INDEX)->AccountID, getGameObject(iCCF_WINNER_INDEX)->Name, getGameObject(iCCF_WINNER_INDEX)->m_PlayerData->m_RealNameOfUBF,
							getGameObject(iCCF_WINNER_INDEX)->m_PlayerData->m_nServerCodeOfHomeWorld, getGameObject(iCCF_WINNER_INDEX)->m_PlayerData->DbClass,
							getGameObject(iCCF_WINNER_INDEX)->Level, getGameObject(iCCF_WINNER_INDEX)->m_PlayerData->LevelUpPoint, getGameObject(iCCF_WINNER_INDEX)->m_PlayerData->Experience,
							getGameObject(iCCF_WINNER_INDEX)->m_PlayerData->Strength, getGameObject(iCCF_WINNER_INDEX)->m_PlayerData->Dexterity, getGameObject(iCCF_WINNER_INDEX)->m_PlayerData->Vitality,
							getGameObject(iCCF_WINNER_INDEX)->m_PlayerData->Energy, getGameObject(iCCF_WINNER_INDEX)->Leadership, getGameObject(iCCF_WINNER_INDEX)->MapNumber, getGameObject(iCCF_WINNER_INDEX)->m_PK_Level);

						this->SendCCFWinMessage(iCCF_WINNER_INDEX, nPoint, 1);
						this->GiveCCFWinnerItem(iCCF_WINNER_INDEX, this->m_stCCFData.m_byCCF_TYPE, 1);
					}
				}

				this->SendAllLoserFailMessage(iCCF_WINNER_INDEX);
				this->SetState(3, 1);
				return;
			}

			if (iCUR_PLAY_STATE == 2)
			{
				int iCCF_WINNER_INDEX = -1;
				int iCCF_LEFT_MONSTER_COUNT = this->GetMonsterListCount();

				if (iCCF_LEFT_MONSTER_COUNT > 0)
				{
					sLog->outBasic("[Chaos Castle Survival][CCS2] Has No Winner : Monster Left (%d)", this->m_stCCFData.m_lCCF_CURRENT_MONSTER_COUNT);
				}

				else
				{
					int nPoint = 0;
					int iCCF_WINNER_INDEX = this->GetCurrentCCFWinUser(&nPoint);

					if (iCCF_WINNER_INDEX == -1)
					{
						sLog->outBasic("[Chaos Castle Survival][CCS2] Has No Winner.");
					}

					else
					{
						sLog->outBasic("[Chaos Castle Survival][CCS2] [%s][%s][%s][ServerCode: %d] is Winner. [CharInfoSave : Class=%d, Level=%d, LvPoint=%d, Exp=%d, Str=%d, Dex=%d, Vit=%d, Energy=%d, LeaderShip=%d, Map=%d, Pk=%d]",
							getGameObject(iCCF_WINNER_INDEX)->AccountID, getGameObject(iCCF_WINNER_INDEX)->Name, getGameObject(iCCF_WINNER_INDEX)->m_PlayerData->m_RealNameOfUBF,
							getGameObject(iCCF_WINNER_INDEX)->m_PlayerData->m_nServerCodeOfHomeWorld, getGameObject(iCCF_WINNER_INDEX)->m_PlayerData->DbClass,
							getGameObject(iCCF_WINNER_INDEX)->Level, getGameObject(iCCF_WINNER_INDEX)->m_PlayerData->LevelUpPoint, getGameObject(iCCF_WINNER_INDEX)->m_PlayerData->Experience,
							getGameObject(iCCF_WINNER_INDEX)->m_PlayerData->Strength, getGameObject(iCCF_WINNER_INDEX)->m_PlayerData->Dexterity, getGameObject(iCCF_WINNER_INDEX)->m_PlayerData->Vitality,
							getGameObject(iCCF_WINNER_INDEX)->m_PlayerData->Energy, getGameObject(iCCF_WINNER_INDEX)->Leadership, getGameObject(iCCF_WINNER_INDEX)->MapNumber, getGameObject(iCCF_WINNER_INDEX)->m_PK_Level);

						this->SendCCFWinMessage(iCCF_WINNER_INDEX, nPoint, 0);
						this->GiveCCFWinnerItem(iCCF_WINNER_INDEX, this->m_stCCFData.m_byCCF_TYPE, 1);
					}
				}

				this->SendAllLoserFailMessage(iCCF_WINNER_INDEX);
				this->SetState(3, 1);
				return;
			}

			if (iCUR_PLAY_STATE == 0)
				this->SendNoticeState(6);
		}
	}

	if (this->m_stCCFData.m_nCCF_REMAIN_MSEC <= 0)
	{
		sLog->outBasic("[Chaos Castle Survival] is Over : TIME-OUT (Left User:%d, Monster:%d)",
			this->GetMonsterListCount(), this->GetCurPlayUser());

		int iCCF_WINNER_INDEX = -1;

		if (this->GetMonsterListCount() > 0)
		{
			if (this->GetCurEnteredUser() == 1)
			{
				for (int iMON = 0; iMON < g_ConfigRead.server.GetObjectMaxMonster(); iMON++)
				{
					if (this->m_stCCFData.m_lCCF_MONSTER_COUNT[iMON] != -1)
					{
						int iMonsterIndex = this->m_stCCFData.m_lCCF_MONSTER_COUNT[iMON];

						if (ObjectMaxRange(iMonsterIndex))
						{
							if (getGameObject(iMonsterIndex)->Life > 0.0 &&
								getGameObject(iMonsterIndex)->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL &&
								getGameObject(iMonsterIndex)->Connected > 0)
							{
								sLog->outBasic("[Chaos Castle Survival][Bug Tracker] Left Monster AttrInfo %d/%d",
									getGameObject(iMonsterIndex)->X, getGameObject(iMonsterIndex)->Y);
							}
						}

						else
						{
							this->m_stCCFData.m_lCCF_MONSTER_COUNT[iMON] = -1;
						}
					}
				}
			}

			else
			{
				sLog->outBasic("[Chaos Castle Survival][CCS4]  Has No Winner : Monster Left (%d)",
					this->m_stCCFData.m_lCCF_MONSTER_COUNT);
			}
		}

		else
		{
			int nPoint = 0;
			iCCF_WINNER_INDEX = this->GetCurrentCCFWinUser(&nPoint);

			if (iCCF_WINNER_INDEX == -1)
			{
				sLog->outBasic("[Chaos Castle Survival][CCS3] Has No Winner.");
			}

			else
			{
				sLog->outBasic("[Chaos Castle Survival][CCS3] [%s][%s][%s][ServerCode: %d] is Winner. [CharInfoSave : Class=%d, Level=%d, LvPoint=%d, Exp=%d, Str=%d, Dex=%d, Vit=%d, Energy=%d, LeaderShip=%d, Map=%d, Pk=%d]",
					getGameObject(iCCF_WINNER_INDEX)->AccountID, getGameObject(iCCF_WINNER_INDEX)->Name, getGameObject(iCCF_WINNER_INDEX)->m_PlayerData->m_RealNameOfUBF,
					getGameObject(iCCF_WINNER_INDEX)->m_PlayerData->m_nServerCodeOfHomeWorld, getGameObject(iCCF_WINNER_INDEX)->m_PlayerData->DbClass,
					getGameObject(iCCF_WINNER_INDEX)->Level, getGameObject(iCCF_WINNER_INDEX)->m_PlayerData->LevelUpPoint, getGameObject(iCCF_WINNER_INDEX)->m_PlayerData->Experience,
					getGameObject(iCCF_WINNER_INDEX)->m_PlayerData->Strength, getGameObject(iCCF_WINNER_INDEX)->m_PlayerData->Dexterity, getGameObject(iCCF_WINNER_INDEX)->m_PlayerData->Vitality,
					getGameObject(iCCF_WINNER_INDEX)->m_PlayerData->Energy, getGameObject(iCCF_WINNER_INDEX)->Leadership, getGameObject(iCCF_WINNER_INDEX)->MapNumber, getGameObject(iCCF_WINNER_INDEX)->m_PK_Level);

				this->SendCCFWinMessage(iCCF_WINNER_INDEX, nPoint, 1);
				this->GiveCCFWinnerItem(iCCF_WINNER_INDEX, this->m_stCCFData.m_byCCF_TYPE, 1);
			}
		}

		this->SendAllLoserFailMessage(iCCF_WINNER_INDEX);
		this->SetState(3, 0);
	}
}

void ChaosCastleFinal::ProcState_PlayEnd()
{
	DWORD iTICK_MSEC = GetTickCount() - this->m_stCCFData.m_nCCF_TICK_COUNT;

	if (iTICK_MSEC >= 1000)
	{
		this->m_stCCFData.m_nCCF_REMAIN_MSEC -= iTICK_MSEC;
		this->m_stCCFData.m_nCCF_TICK_COUNT = GetTickCount();

		this->CheckUserInDieTile();

		if (this->m_stCCFData.m_nCCF_REMAIN_MSEC <= 30000)
		{
			if (this->m_stCCFData.m_nCCF_REMAIN_MSEC > 0 && !this->m_stCCFData.m_bCCF_MSG_BEFORE_QUIT)
			{
				this->m_stCCFData.m_bCCF_MSG_BEFORE_QUIT = TRUE;

				PMSG_SET_DEVILSQUARE pMsg;

				PHeadSetB((BYTE*)&pMsg, 0x92, sizeof(pMsg));
				pMsg.Type = 23;

				this->SendChaosCastleAnyMsg((BYTE*)&pMsg, sizeof(pMsg));
			}
		}
	}

	if (this->m_stCCFData.m_nCCF_REMAIN_MSEC <= 0)
	{
		this->SetState(1, 0);
	}
}

void ChaosCastleFinal::SendAllUserAnyMsg(BYTE* lpMsg, int iSize)
{
	for each (std::pair<int,CGameObject*> ObjEntry in gGameObjects)
	{
		if (getGameObject(i)->Connected == PLAYER_PLAYING)
		{
			if (getGameObject(i)->Type == OBJ_USER)
			{
				if (DG_MAP_RANGE(getGameObject(i)->MapNumber) == FALSE)
				{
					IOCP.DataSend(i, lpMsg, iSize);
				}
			}
		}
	}
}

void ChaosCastleFinal::SetState_PlayEnd(int nFlag)
{
	this->SendNoticeState(7);

	if (nFlag == FALSE)
	{
		this->ClearMonster();
	}

	this->m_stCCFData.m_bCCF_CAN_ENTER = false;
	this->m_stCCFData.m_bCCF_MSG_ALL_NOTIFY = false;

	this->m_stCCFData.m_byCCF_TYPE = this->Check_CCF_DayType();
	sLog->outBasic("[Chaos Castle Survival] [ItemDropCount Reset] m_byBoxDropCnt: %d ",
		this->m_byBoxDropCnt);

	if (!this->m_bIsCCF_Ranking_Renew)
	{
		sLog->outBasic("[Chaos Castle Survival] Ranking list START");

		for (int i = 0; i < 50; i++)
		{
			if (this->m_CCFRanking[i].byRank > 0)
				sLog->outBasic("[Chaos Castle Survival] No.%d Name: %s Point: %d", this->m_CCFRanking[i].byRank, this->m_CCFRanking[i].szCharName, this->m_CCFRanking[i].nPoint);
		}

		sLog->outBasic("[Chaos Castle Survival] Ranking list END");

		this->GD_Renew_CCF_Ranking(this->m_stCCFData.m_byCCF_TYPE);
		this->m_bIsCCF_Ranking_Renew = true;
		sLog->outBasic("[Chaos Castle Survival][SetState_PlayEnd][REQ DataServer Renew Ranking] tick :%d", GetTickCount());
	}

	if (this->Check_CCF_DayType() == 3)
	{
		sLog->outBasic("[Chaos Castle Survival][FINALDAY WAR END]");

		if (this->IsRealFinalDayEnd() == 1)
		{
			this->m_bFinalDayEnd = true;
		}
	}

	this->m_stCCFData.m_nCCF_REMAIN_MSEC = 60000 * this->m_iCCF_TIME_MIN_REST;

	sLog->outBasic("[Chaos Castle Survival] SetState PLAYEND");
}

void ChaosCastleFinal::SetState_Playing()
{
	this->m_stCCFData.m_bCCF_CAN_ENTER = false;
	this->m_stCCFData.m_nCCF_REMAIN_MSEC = 60000 * this->m_iCCF_TIME_MIN_PLAY;
	this->SendCastleZoneSafetyInfo(1);

	if (this->CheckCanStart_CCF())
	{
		sLog->outBasic("[Chaos Castle Survival] SetState PLAYING");
	}
	else
	{
		this->CCF_Start_Fail_So_RollBack();
		this->SetState_Closed();
		sLog->outBasic("[Chaos Castle Survival] Failed to Start ChaosCastleFinal : Lack of User");
	}
}

void ChaosCastleFinal::SetState_Closed()
{
	this->SendNoticeState(7);
	this->ClearCCFData();
	this->ClearMonster();

	for (int n = g_ConfigRead.server.GetObjectStartUserIndex(); n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		if (getGameObject(n)->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
		{
			gObjMoveGate(n, 333);
		}
	}

	this->SafetyCastleZone();
	this->SetMapAttrFill();
	this->CheckSync();
	this->GD_Load_CCF_RankingList(this->CCF_Ranking_DayType());

	sLog->outBasic("[Chaos Castle Survival][SetState_Closed][GD_Load_CCF_RankingList] tick %d ", GetTickCount());
	sLog->outBasic("[Chaos Castle Survival] SetState CLOSED");
}

void ChaosCastleFinal::SetState_None()
{
	return;
}

void ChaosCastleFinal::SendAllLoserFailMessage(int iWinnerIndex)
{
	if (iWinnerIndex != -1 && !ObjectMaxRange(iWinnerIndex))
	{
		sLog->outBasic("[Chaos Castle Survival][SendAllLoserFailMessage] return ERROR iWinnerIndex : %d", iWinnerIndex);
		return;
	}

	for (int i = 0; i < 70; i++)
	{
		if (this->m_stCCFData.m_UserData[i].m_nIndex != -1
			&& this->m_stCCFData.m_UserData[i].m_nIndex != iWinnerIndex
			&& !getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->m_bCCF_Quit_Msg
			&& gObjIsConnected(this->m_stCCFData.m_UserData[i].m_nIndex))
		{
			if (getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->m_nCCF_UserIndex != -1)
			{
				PMSG_ANS_CCF_RESULT pMsg;
				int nPoint = getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex]->m_byKillMobCount + 2 * getGameObject(this->m_stCCFData.m_UserData[i)->m_nIndex)->m_byKillUserCount;

				this->GD_Req_Save_CCF_Result(this->m_stCCFData.m_UserData[i].m_nIndex, getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->Name,
					nPoint, this->Check_CCF_DayType());

				gGameProtocol.GCServerMsgStringSend(Lang.GetText(0,557), this->m_stCCFData.m_UserData[i].m_nIndex, 1);

				PHeadSubSetB((BYTE*)&pMsg, 0xAF, 0x09, sizeof(pMsg));
				pMsg.point = nPoint;
				pMsg.result = 2;
				pMsg.type = this->Check_CCF_DayType();
				pMsg.MobKillCnt = getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->m_byKillMobCount;
				pMsg.PCKillCnt = getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->m_byKillUserCount;
				IOCP.DataSend(this->m_stCCFData.m_UserData[i].m_nIndex, (BYTE*)&pMsg, pMsg.h.size);

				getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->m_bCCF_Quit_Msg = true;
				sLog->outBasic("[Chaos Castle Survival][GAME-RESULT][FAIL-CASE] NAME:[%s] UBFNAME:[%s] SERVERCODE:[%d],MOBKILL:[%d],USERKILL:[%d],POINT:[%d]",
					getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->Name,
					getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->m_PlayerData->m_RealNameOfUBF,
					getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->m_PlayerData->m_nServerCodeOfHomeWorld,
					getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->m_byKillMobCount,
					getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->m_byKillUserCount,
					nPoint);
			}
		}
	}
}

void ChaosCastleFinal::SendCCFWinMessage(int iWinnerIndex, int nPoint, bool bFinalWinner)
{
	if (ObjectMaxRange(iWinnerIndex) == false)
	{
		return;
	}

	if (gObjIsConnected(iWinnerIndex) == false)
	{
		return;
	}

	if (getGameObject(iWinnerIndex)->m_bCCF_Quit_Msg == false)
	{
		if (bFinalWinner != 4 || this->Check_CCF_DayType() != 3)
		{
			this->GD_Req_Save_CCF_Result(iWinnerIndex, getGameObject(iWinnerIndex)->Name, nPoint, this->Check_CCF_DayType());
		}

		else
		{
			this->GD_Req_Save_CCF_Result(iWinnerIndex, getGameObject(iWinnerIndex)->Name, nPoint, 4);
		}

		PMSG_ANS_CCF_RESULT pMsg;
		int nPoint = getGameObject(iWinnerIndex)->m_byKillMobCount + 2 * getGameObject(iWinnerIndex)->m_byKillUserCount;

		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0,558), iWinnerIndex, 1);

		PHeadSubSetB((BYTE*)&pMsg, 0xAF, 0x09, sizeof(pMsg));
		pMsg.point = nPoint;
		pMsg.result = 1;
		pMsg.type = this->Check_CCF_DayType();
		pMsg.MobKillCnt = getGameObject(iWinnerIndex)->m_byKillMobCount;
		pMsg.PCKillCnt = getGameObject(iWinnerIndex)->m_byKillUserCount;
		IOCP.DataSend(iWinnerIndex, (BYTE*)&pMsg, pMsg.h.size);

		getGameObject(iWinnerIndex)->m_bCCF_Quit_Msg = true;
		sLog->outBasic("[Chaos Castle Survival][GAME-RESULT][WIN-CASE] NAME:[%s] UBFNAME:[%s] SERVERCODE:[%d],MOBKILL:[%d],USERKILL:[%d],POINT:[%d]",
			getGameObject(iWinnerIndex)->Name,
			getGameObject(iWinnerIndex)->m_PlayerData->m_RealNameOfUBF,
			getGameObject(iWinnerIndex)->m_PlayerData->m_nServerCodeOfHomeWorld,
			getGameObject(iWinnerIndex)->m_byKillMobCount,
			getGameObject(iWinnerIndex)->m_byKillUserCount,
			nPoint);
	}
}

void ChaosCastleFinal::AddFallUser(CGameObject &Obj)
{
	for (int iFALL = 0; iFALL < 70; iFALL++)
	{
		if (this->m_stCCFData.m_lFallUser[iFALL] == -1)
		{
			InterlockedExchange(&this->m_stCCFData.m_lFallUser[iFALL], iUserIndex);
			return;
		}
	}
}

void ChaosCastleFinal::SearchNBlowObjs(int iMapNumber, int iX, int iY)
{
	if (!CHECK_LIMIT(iX, 256) || !CHECK_LIMIT(iY, 256))
		return;

	int iMIN_X = iX - 3;
	int iMAX_X = iX + 3;
	int iMIN_Y = iY - 3;
	int iMAX_Y = iY + 3;

	if (iMIN_X < 0) iMIN_X = 0;
	if (iMIN_Y < 0) iMIN_Y = 0;
	if (iMIN_X > 255) iMIN_X = 255;
	if (iMIN_Y > 255) iMIN_Y = 255;

	for (int i = 0; i<70; i++)
	{
		if (this->m_stCCFData.m_UserData[i].m_nIndex == -1)
		{
			continue;
		}

		int iIndex = this->m_stCCFData.m_UserData[i].m_nIndex;

		if (Obj.MapNumber == iMapNumber && Obj.Connected > PLAYER_LOGGED)
		{
			if (Obj.X >= iMIN_X && Obj.X <= iMAX_X &&
				Obj.Y >= iMIN_Y && Obj.Y <= iMAX_Y)
			{
				this->BlowObjsFromPoint(Obj.m_Index, iMapNumber, iX, iY);

				BYTE btMapAttr = MapC[iMapNumber].m_attrbuf[iY * 256 + iX] & 0x08;

				if (btMapAttr == 8)
				{
					this->AddFallUser(Obj.m_Index);

					sLog->outBasic("[Chaos Castle Survival] [%s][%s] User Dead In Chaos Castle : Fall from Castle (X:%d, Y:%d)",
						Obj.AccountID, Obj.Name, iX, iY);
				}
			}
		}
	}
}

int ChaosCastleFinal::BlowObjsFromPoint(CGameObject &Obj, int iMapNumber, int & iX, int & iY)
{
	if (this->m_bCCF_CHEAT_BLOW == false)
		return FALSE;

	if (!gObjIsConnected(Obj.m_Index))
		return FALSE;

	if (!CHECK_LIMIT(iX, 256) || !CHECK_LIMIT(iY, 256))
		return FALSE;

	CGameObject lpObj = Obj;

	if (Obj.DieRegen)
		return FALSE;

	if (Obj.Teleport)
		return FALSE;

	if (Obj.MapNumber != iMapNumber)
		return FALSE;

	int iOBJ_DIST = this->CalDistance(Obj.X, Obj.Y, iX, iY);

	if (!CHECK_LIMIT(iOBJ_DIST, 4))
		return FALSE;

	int iSIGN_X = 1;
	int iSIGN_Y = 1;
	int iUX = Obj.X;
	int iUY = Obj.Y;

	if (iUX > iX)
		iSIGN_X = 1;
	else if (iUX < iX)
		iSIGN_X = -1;
	else
	{
		int iRND = rand() % 2;
		(!iRND) ? iSIGN_X = 1 : iSIGN_X = -1;
	}

	if (iUY > iY)
		iSIGN_Y = 1;
	else if (iUY < iY)
		iSIGN_Y = -1;
	else
	{
		int iRND = rand() % 2;
		(!iRND) ? iSIGN_Y = 1 : iSIGN_Y = -1;
	}

	BOOL bSuccessBlowOut = FALSE;

	for (int iBLOWOUT_COUNT = 0; iBLOWOUT_COUNT < 5; iBLOWOUT_COUNT++)
	{
		int iBLOW_MIN = g_nCCF_BlowOutDistance[iOBJ_DIST][0];
		int iBLOW_MAX = g_nCCF_BlowOutDistance[iOBJ_DIST][1];
		int iBLOW_X = rand() % (iBLOW_MAX - iBLOW_MIN + 1) + iBLOW_MIN;
		int iBLOW_Y = rand() % (iBLOW_MAX - iBLOW_MIN + 1) + iBLOW_MIN;
		int iRND = rand() % 2;

		if (iRND)
		{
			if (iBLOW_X >= iBLOW_MAX)
			{
				iBLOW_X = iBLOW_MAX;
				iBLOW_Y = iBLOW_MIN + (rand() % 2 - 1);

				if (iBLOW_Y < 0)
					iBLOW_Y = 0;
			}
		}
		else if (iBLOW_Y >= iBLOW_MAX)
		{
			iBLOW_Y = iBLOW_MAX;
			iBLOW_X = iBLOW_MIN + (rand() % 2 - 1);

			if (iBLOW_X < 0)
				iBLOW_X = 0;
		}

		int iTX = Obj.X + iBLOW_X * iSIGN_X;
		int iTY = Obj.Y + iBLOW_Y * iSIGN_Y;

		if (iTX < 0) iTX = 0;
		if (iTY < 0) iTY = 0;
		if (iTX > 255) iTX = 255;
		if (iTY > 255) iTY = 255;

		bSuccessBlowOut = this->ObjSetPosition(iIndex, iTX, iTY);

		if (bSuccessBlowOut)
		{
			Obj.m_iChaosCastleBlowTime = GetTickCount();
			iX = iTX;
			iY = iTY;

			this->GiveUserDamage(Obj.m_Index, g_nCCF_BlowOutDamage[iOBJ_DIST % 4]);
			break;
		}
	}

	return TRUE;
}

int ChaosCastleFinal::CalDistance(int iX1, int iY1, int iX2, int iY2)
{
	if (iX1 == iX2 && iY1 == iY2)
	{
		return 0;
	}

	float fTX = (float)(iX1 - iX2);
	float fTY = (float)(iY1 - iY2);

	return sqrt((fTX * fTX) + (fTY * fTY));
}

int ChaosCastleFinal::GetCurrentCCFWinUser(int *nPoint)
{
	int iCCF_SCORE = -1;
	int iCCF_WINNER_INDEX = -1;
	int OldWinnerLevel = 0;
	UINT64 Oldi64MasterLevelExp = 0;

	for (int i = 0; i < 70; i++)
	{
		if (this->m_stCCFData.m_UserData[i].m_nIndex != -1
			&& gObjIsConnected(this->m_stCCFData.m_UserData[i].m_nIndex) == TRUE
			&& getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL
			&& !this->m_stCCFData.m_UserData[i].m_nUserState)
		{
			CGameObject lpObj = &getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex);
			int iCCF_TEMP_SCORE = Obj.m_byKillMobCount + 2 * Obj.m_byKillUserCount;

			if (iCCF_SCORE >= iCCF_TEMP_SCORE)
			{
				if (iCCF_SCORE == iCCF_TEMP_SCORE)
				{
					int tempNewAllLevel = Obj.m_PlayerData->MasterLevel + Obj.Level;
					if (OldWinnerLevel <= tempNewAllLevel)
					{
						if (OldWinnerLevel == tempNewAllLevel)
						{
							if (Oldi64MasterLevelExp > Obj.m_PlayerData->MasterExperience)
							{
								iCCF_SCORE = Obj.m_byKillMobCount + 2 * Obj.m_byKillUserCount;
								iCCF_WINNER_INDEX = Obj.m_Index;
								OldWinnerLevel = Obj.m_PlayerData->MasterLevel + Obj.Level;
								*nPoint = iCCF_TEMP_SCORE;
								this->m_stCCFData.m_UserData[i].m_nScore = iCCF_TEMP_SCORE;
								Oldi64MasterLevelExp = Obj.m_PlayerData->MasterExperience;
							}
						}
					}
					else
					{
						iCCF_SCORE = Obj.m_byKillMobCount + 2 * Obj.m_byKillUserCount;
						iCCF_WINNER_INDEX = Obj.m_Index;
						OldWinnerLevel = Obj.m_PlayerData->MasterLevel + Obj.Level;
						*nPoint = iCCF_TEMP_SCORE;
						this->m_stCCFData.m_UserData[i].m_nScore = iCCF_TEMP_SCORE;
						Oldi64MasterLevelExp = Obj.m_PlayerData->MasterExperience;
					}
				}
			}
			else
			{
				iCCF_SCORE = Obj.m_byKillMobCount + 2 * Obj.m_byKillUserCount;
				iCCF_WINNER_INDEX = Obj.m_Index;
				OldWinnerLevel = Obj.m_PlayerData->MasterLevel + Obj.Level;
				*nPoint = iCCF_TEMP_SCORE;
				this->m_stCCFData.m_UserData[i].m_nScore = iCCF_TEMP_SCORE;
				Oldi64MasterLevelExp = Obj.m_PlayerData->MasterExperience;
			}
		}
	}

	if (iCCF_WINNER_INDEX != -1)
	{
		if (!getGameObject(iCCF_WINNER_INDEX)->m_byKillUserCount)
		{
			if (!getGameObject(iCCF_WINNER_INDEX)->m_byKillMobCount)
			{
				if (this->GetCurPlayUser() != TRUE || this->GetMonsterListCount())
					iCCF_WINNER_INDEX = -1;
			}
		}
	}
	
	this->Check_CCF_DayType();
	*nPoint += 3;
	return iCCF_WINNER_INDEX;
}

int ChaosCastleFinal::GetMonsterListCount()
{
	int iAliveMonsterCount = 0;
	for (int iMON = 0; iMON < 100; iMON++)
	{
		if (this->m_stCCFData.m_lCCF_MONSTER_COUNT[iMON] != -1)
		{
			int iMonsterIndex = this->m_stCCFData.m_lCCF_MONSTER_COUNT[iMON];

			if (ObjectMaxRange(iMonsterIndex))
			{
				if (getGameObject(iMonsterIndex)->Life > 0.0 && getGameObject(iMonsterIndex)->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
				{
					if (getGameObject(iMonsterIndex)->Connected > 0)
						++iAliveMonsterCount;
				}
			}
			else
			{
				this->m_stCCFData.m_lCCF_MONSTER_COUNT[iMON] = -1;
			}
		}
	}
	return iAliveMonsterCount;
}

int ChaosCastleFinal::CheckPlayEnded()
{
	int iCUR_USER_COUNT = this->GetCurPlayUser();
	int iCUR_MONSTER_COUNT = this->GetMonsterListCount();

	if (iCUR_USER_COUNT > 0)
		return iCUR_USER_COUNT == 1 && iCUR_MONSTER_COUNT <= 0;
	else
		return 2;
}

void ChaosCastleFinal::PullObjInnerPlace(int iTRAP_STEP)
{
	if (CCF_TRAP_STEP_RANGE(iTRAP_STEP) == FALSE)
		return;

	if (iTRAP_STEP == 0)
		return;

	int iUSER_AXIS = 0;
	std::vector<POINT> vtMAP_UNTRAP[MAX_CCF_TRAP_STEP];

	for (int iAXIS = 0; iAXIS < MAX_CCF_TRAP_STEP; iAXIS++)
	{
		for (int iMAPX = g_rt_CCF_POINT_TRAP[iAXIS].left; iMAPX <= g_rt_CCF_POINT_TRAP[iAXIS].right; iMAPX++)
		{
			for (int iMAPY = g_rt_CCF_POINT_TRAP[iAXIS].top; iMAPY <= g_rt_CCF_POINT_TRAP[iAXIS].bottom; iMAPY++)
			{
				BYTE btMapAttr = MapC[MAP_INDEX_CHAOSCASTLE_SURVIVAL].GetAttr(iMAPX, iMAPY);

				if ((btMapAttr & 2) != 2 && (btMapAttr & 4) != 4 && (btMapAttr & 8) != 8)
				{
					POINT iPT = { iMAPX, iMAPY };
					vtMAP_UNTRAP[iAXIS].push_back(iPT);
				}
			}
		}
	}

	for (int i = 0; i<70; i++)
	{
		if (this->m_stCCFData.m_UserData[i].m_nIndex != -1)
		{
			if (gObjIsConnected(this->m_stCCFData.m_UserData[i].m_nIndex) == TRUE && getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
			{
				int iSX = getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->X;
				int iSY = getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->Y;

				BYTE btMapAttr = MapC[MAP_INDEX_CHAOSCASTLE_SURVIVAL].m_attrbuf[iSY * 256 + iSX] & 0x08;

				if (btMapAttr == 8)
				{
					int n;
					for (n = 0; n<MAX_CCF_TRAP_STEP; n++)
					{
						if (iSX >= g_rt_CCF_POINT_FRAME[n].left && iSX <= g_rt_CCF_POINT_FRAME[n].right &&
							iSY >= g_rt_CCF_POINT_FRAME[n].top && iSY <= g_rt_CCF_POINT_FRAME[n].bottom)
						{
							iUSER_AXIS = n;
							break;
						}
					}

					if (n == MAX_CCF_TRAP_STEP)
						continue;

					if (vtMAP_UNTRAP[iUSER_AXIS].empty() == false)
					{
						POINT ptEMPTY = vtMAP_UNTRAP[iUSER_AXIS].back();
						vtMAP_UNTRAP[iUSER_AXIS].pop_back();

						if (this->ObjSetPosition(this->m_stCCFData.m_UserData[i].m_nIndex, ptEMPTY.x, ptEMPTY.y) == TRUE)
						{
							getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->m_nCCF_BlowTime = GetTickCount();
						}
					}
				}
			}
		}
	}

	for (int iMON = 0; iMON < 100; iMON++)
	{
		if (this->m_stCCFData.m_lCCF_MONSTER_COUNT[iMON] == -1)
			continue;

		int iMON_INDEX = this->m_stCCFData.m_lCCF_MONSTER_COUNT[iMON];

		if (!CHECK_LIMIT(iMON_INDEX, g_ConfigRead.server.GetObjectMaxMonster()))
		{
			this->m_stCCFData.m_lCCF_MONSTER_COUNT[iMON] = -1;
			continue;
		}

		if (getGameObject(iMON_INDEX)->Life > 0.0f && getGameObject(iMON_INDEX)->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL != FALSE && getGameObject(iMON_INDEX)->Connected > PLAYER_EMPTY)
		{
			int iSX = getGameObject(iMON_INDEX)->X;
			int iSY = getGameObject(iMON_INDEX)->Y;

			BYTE btMapAttr = MapC[MAP_INDEX_CHAOSCASTLE_SURVIVAL].m_attrbuf[iSY * 256 + iSX] & 0x08;

			if (btMapAttr == 8)
			{
				int n;
				for (n = 0; n<MAX_CCF_TRAP_STEP; n++)
				{
					if (iSX >= g_rt_CCF_POINT_FRAME[n].left && iSX <= g_rt_CCF_POINT_FRAME[n].right &&
						iSY >= g_rt_CCF_POINT_FRAME[n].top && iSY <= g_rt_CCF_POINT_FRAME[n].bottom)
					{
						iUSER_AXIS = n;
						break;
					}
				}

				if (n == MAX_CCF_TRAP_STEP)
					continue;

				if (vtMAP_UNTRAP[iUSER_AXIS].empty() == false)
				{
					POINT ptEMPTY = vtMAP_UNTRAP[iUSER_AXIS].back();
					vtMAP_UNTRAP[iUSER_AXIS].pop_back();

					if (this->ObjSetPosition(iMON_INDEX, ptEMPTY.x, ptEMPTY.y) == TRUE)
					{
						getGameObject(iMON_INDEX)->m_iChaosCastleBlowTime = GetTickCount();
					}
				}
			}
		}
	}

	for (int iMAP_ITEM = 0; iMAP_ITEM < g_ConfigRead.server.GetObjectMaxItem(); iMAP_ITEM++)
	{
		if (MapC[MAP_INDEX_CHAOSCASTLE_SURVIVAL].m_CItemObject[iMAP_ITEM].IsItem())
		{
			int x = MapC[MAP_INDEX_CHAOSCASTLE_SURVIVAL].m_CItemObject[iMAP_ITEM].px;
			int y = MapC[MAP_INDEX_CHAOSCASTLE_SURVIVAL].m_CItemObject[iMAP_ITEM].py;

			if (x < ::g_iCCF_DamageAxis[iTRAP_STEP][0] || x > ::g_iCCF_DamageAxis[iTRAP_STEP][2] ||
				y < ::g_iCCF_DamageAxis[iTRAP_STEP][1] || y > ::g_iCCF_DamageAxis[iTRAP_STEP][3])
			{
				MapC[MAP_INDEX_CHAOSCASTLE_SURVIVAL].m_CItemObject[iMAP_ITEM].m_State = 8;
				MapC[MAP_INDEX_CHAOSCASTLE_SURVIVAL].m_CItemObject[iMAP_ITEM].Give = true;
				MapC[MAP_INDEX_CHAOSCASTLE_SURVIVAL].m_CItemObject[iMAP_ITEM].live = false;
			}
		}
	}
}

void ChaosCastleFinal::SetMapAttrHollow(int iTRAP_STEP)
{
	if (CCF_TRAP_STEP_RANGE(iTRAP_STEP) == FALSE)
	{
		return;
	}

	if (iTRAP_STEP == 0)
	{
		return;
	}

	for (int a = 0; a<MAX_CCF_HOLLOW_ZONE; a++)
	{
		for (int b = ::g_iCCF_MapHollowZone[iTRAP_STEP][a][0]; b <= ::g_iCCF_MapHollowZone[iTRAP_STEP][a][2]; b++)
		{
			for (int c = ::g_iCCF_MapHollowZone[iTRAP_STEP][a][1]; c <= ::g_iCCF_MapHollowZone[iTRAP_STEP][a][3]; c++)
			{
				MapC[MAP_INDEX_CHAOSCASTLE_SURVIVAL].m_attrbuf[c * 256 + b] |= 8;
			}
		}
	}
}

void ChaosCastleFinal::SetMapAttrFill()
{
	for (int iTRAP_STEP = 1; iTRAP_STEP<MAX_CCF_TRAP_STEP; iTRAP_STEP++)
	{
		for (int a = 0; a<MAX_CCF_HOLLOW_ZONE; a++)
		{
			for (int b = ::g_iCCF_MapHollowZone[iTRAP_STEP][a][0]; b <= ::g_iCCF_MapHollowZone[iTRAP_STEP][a][2]; b++)
			{
				for (int c = ::g_iCCF_MapHollowZone[iTRAP_STEP][a][1]; c <= ::g_iCCF_MapHollowZone[iTRAP_STEP][a][3]; c++)
				{
					MapC[MAP_INDEX_CHAOSCASTLE_SURVIVAL].m_attrbuf[c * 256 + b] &= ~8;
				}
			}
		}
	}
}

void ChaosCastleFinal::ProcessTrapStatus()
{
	int iCUR_LIVES = this->GetCurPlayUser() + this->GetMonsterListCount();

	if (iCUR_LIVES > 40)
	{
		this->m_stCCFData.m_nCCF_TRAP_STATUS = 0;
	}

	else if (iCUR_LIVES > 30)
	{
		this->m_stCCFData.m_nCCF_TRAP_STATUS = 1;
		this->SendNoticeState(8);
	}

	else if (iCUR_LIVES > 20)
	{
		this->m_stCCFData.m_nCCF_TRAP_STATUS = 2;
		this->SendNoticeState(9);
	}

	else
	{
		this->m_stCCFData.m_nCCF_TRAP_STATUS = 3;
		this->SendNoticeState(10);
	}
}

BOOL ChaosCastleFinal::CheckWearingMOPH(CGameObject &Obj)
{
	if (!gObjIsConnected(iUserIndex))
		return FALSE;

	if (getGameObject(iUserIndex)->pInventory[10)->IsItem() == TRUE)
	{
		if (getGameObject(iUserIndex)->pInventory[10)->m_Type == ITEMGET(13,10))
			return TRUE;
		if (getGameObject(iUserIndex)->pInventory[10)->m_Type == ITEMGET(13,39))
			return TRUE;
		if (getGameObject(iUserIndex)->pInventory[10)->m_Type == ITEMGET(13,40))
			return TRUE;
		if (getGameObject(iUserIndex)->pInventory[10)->m_Type == ITEMGET(13,41))
			return TRUE;
		if (getGameObject(iUserIndex)->pInventory[10)->m_Type == ITEMGET(13,68))
			return TRUE;
		if (getGameObject(iUserIndex)->pInventory[10)->m_Type == ITEMGET(13,76))
			return TRUE;
		if (getGameObject(iUserIndex)->pInventory[10)->m_Type == ITEMGET(13,77))
			return TRUE;
		if (getGameObject(iUserIndex)->pInventory[10)->m_Type == ITEMGET(13,78))
			return TRUE;
		if (getGameObject(iUserIndex)->pInventory[10)->m_Type == ITEMGET(13,122))
			return TRUE;
		if (getGameObject(iUserIndex)->pInventory[10)->m_Type == ITEMGET(13,163))
			return TRUE;
		if (getGameObject(iUserIndex)->pInventory[10)->m_Type == ITEMGET(13,164))
			return TRUE;
		if (getGameObject(iUserIndex)->pInventory[10)->m_Type == ITEMGET(13,166))
			return TRUE;
		if (getGameObject(iUserIndex)->pInventory[10)->m_Type == ITEMGET(13,169))
			return TRUE;
		if (getGameObject(iUserIndex)->pInventory[10)->m_Type == ITEMGET(13,170))
			return TRUE;
		if (getGameObject(iUserIndex)->pInventory[10)->m_Type == ITEMGET(13,268))
			return TRUE;
	}

	if (getGameObject(iUserIndex)->pInventory[11)->IsItem() == TRUE)
	{
		if (getGameObject(iUserIndex)->pInventory[11)->m_Type == ITEMGET(13,10))
			return TRUE;
		if (getGameObject(iUserIndex)->pInventory[11)->m_Type == ITEMGET(13,39))
			return TRUE;
		if (getGameObject(iUserIndex)->pInventory[11)->m_Type == ITEMGET(13,40))
			return TRUE;
		if (getGameObject(iUserIndex)->pInventory[11)->m_Type == ITEMGET(13,41))
			return TRUE;
		if (getGameObject(iUserIndex)->pInventory[11)->m_Type == ITEMGET(13,68))
			return TRUE;
		if (getGameObject(iUserIndex)->pInventory[11)->m_Type == ITEMGET(13,76))
			return TRUE;
		if (getGameObject(iUserIndex)->pInventory[11)->m_Type == ITEMGET(13,77))
			return TRUE;
		if (getGameObject(iUserIndex)->pInventory[11)->m_Type == ITEMGET(13,78))
			return TRUE;
		if (getGameObject(iUserIndex)->pInventory[11)->m_Type == ITEMGET(13,122))
			return TRUE;
		if (getGameObject(iUserIndex)->pInventory[11)->m_Type == ITEMGET(13,163))
			return TRUE;
		if (getGameObject(iUserIndex)->pInventory[11)->m_Type == ITEMGET(13,164))
			return TRUE;
		if (getGameObject(iUserIndex)->pInventory[10)->m_Type == ITEMGET(13,166))
			return TRUE;
		if (getGameObject(iUserIndex)->pInventory[11)->m_Type == ITEMGET(13,169))
			return TRUE;
		if (getGameObject(iUserIndex)->pInventory[11)->m_Type == ITEMGET(13,170))
			return TRUE;
		if (getGameObject(iUserIndex)->pInventory[11)->m_Type == ITEMGET(13,268))
			return TRUE;
	}

	return FALSE;
}

void ChaosCastleFinal::CheckMonsterInDieTile()
{
	if (this->m_bCCF_CHEAT_TRAP == false)
	{
		return;
	}

	int iUSER_AXIS = 0;
	std::vector<POINT> vtMAP_UNTRAP[4];

	for (int iAXIS = 0; iAXIS < 4; iAXIS++)
	{
		for (int iMAPX = g_rt_CCF_POINT_TRAP[iAXIS].left; iMAPX <= g_rt_CCF_POINT_TRAP[iAXIS].right; iMAPX++)
		{
			for (int iMAPY = g_rt_CCF_POINT_TRAP[iAXIS].top; iMAPY <= g_rt_CCF_POINT_TRAP[iAXIS].bottom; iMAPY++)
			{
				BYTE btMapAttr = MapC[MAP_INDEX_CHAOSCASTLE_SURVIVAL].GetAttr(iMAPX, iMAPY);

				if ((btMapAttr & 2) != 2 && (btMapAttr & 4) != 4 && (btMapAttr & 8) != 8)
				{
					POINT iPT = { iMAPX, iMAPY };
					vtMAP_UNTRAP[iAXIS].push_back(iPT);
				}
			}
		}
	}

	for (int iMON = 0; iMON < 100; iMON++)
	{
		if (this->m_stCCFData.m_lCCF_MONSTER_COUNT[iMON] == -1)
			continue;

		int iMON_INDEX = this->m_stCCFData.m_lCCF_MONSTER_COUNT[iMON];

		if (!CHECK_LIMIT(iMON_INDEX, g_ConfigRead.server.GetObjectMaxMonster()))
		{
			this->m_stCCFData.m_lCCF_MONSTER_COUNT[iMON] = -1;
			continue;
		}

		if (getGameObject(iMON_INDEX)->Life > 0.0f && getGameObject(iMON_INDEX)->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL && getGameObject(iMON_INDEX)->Connected > PLAYER_EMPTY)
		{
			int iSX = getGameObject(iMON_INDEX)->X;
			int iSY = getGameObject(iMON_INDEX)->Y;

			BYTE btMapAttr = MapC[MAP_INDEX_CHAOSCASTLE_SURVIVAL].m_attrbuf[iSY * 256 + iSX] & 0x08;

			if (btMapAttr == 8)
			{
				int n;
				for (n = 0; n<MAX_CCF_TRAP_STEP; n++)
				{
					if (iSX >= g_rt_CCF_POINT_FRAME[n].left && iSX <= g_rt_CCF_POINT_FRAME[n].right &&
						iSY >= g_rt_CCF_POINT_FRAME[n].top && iSY <= g_rt_CCF_POINT_FRAME[n].bottom)
					{
						iUSER_AXIS = n;
						break;
					}
				}

				if (n == MAX_CCF_TRAP_STEP)
					continue;

				if (vtMAP_UNTRAP[iUSER_AXIS].empty() == false)
				{
					POINT ptEMPTY = vtMAP_UNTRAP[iUSER_AXIS].back();
					vtMAP_UNTRAP[iUSER_AXIS].pop_back();

					if (this->ObjSetPosition(iMON_INDEX, ptEMPTY.x, ptEMPTY.y) == TRUE)
					{
						getGameObject(iMON_INDEX)->m_nCCF_BlowTime = GetTickCount();
					}
				}
			}
		}
	}
}

void ChaosCastleFinal::CheckUserInDieTile()
{
	for (int i = 0; i<70; i++)
	{
		if (this->m_stCCFData.m_UserData[i].m_nIndex != -1)
		{
			if (gObjIsConnected(this->m_stCCFData.m_UserData[i].m_nIndex) == TRUE &&
				getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
			{
				if (getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->Life > 0.0)
				{
					int iSX = getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->X;
					int iSY = getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->Y;

					BYTE btMapAttr = MapC[MAP_INDEX_CHAOSCASTLE_SURVIVAL].m_attrbuf[iSY * 256 + iSX] & 0x08;

					if (btMapAttr == 8)
					{
						getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->Life = 0;
						getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->m_State = 4;
						getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->KillerType = 1;
						getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->RegenTime = GetTickCount();
						getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->DieRegen = 1;
						getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->PathCount = 0;

						gGameProtocol.GCDiePlayerSend(&getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex),
							this->m_stCCFData.m_UserData[i].m_nIndex, 0, 0);

						sLog->outBasic("[Chaos Castle Survival] [%s][%s] User Dead In Chaos Castle : Fall from Castle (X:%d, Y:%d)",
							getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->AccountID,
							getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->Name,
							iSX, iSY);

						this->SendFailMessage(this->m_stCCFData.m_UserData[i].m_nIndex);

					}
				}
			}
		}
	}
}

void ChaosCastleFinal::ProcessFallUser()
{
	for (int iFALL = 0; iFALL<70; iFALL++)
	{
		if (this->m_stCCFData.m_lFallUser[iFALL] != -1)
		{
			int iFALL_INDEX = this->m_stCCFData.m_lFallUser[iFALL];
			InterlockedExchange(&this->m_stCCFData.m_lFallUser[iFALL], -1);

			if (getGameObject(iFALL_INDEX)->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL && gObjIsConnected(iFALL_INDEX) && getGameObject(iFALL_INDEX)->Life > 0)
			{
				getGameObject(iFALL_INDEX)->Life = 0;
				getGameObject(iFALL_INDEX)->m_State = 4;
				getGameObject(iFALL_INDEX)->KillerType = 1;
				getGameObject(iFALL_INDEX)->RegenTime = GetTickCount();
				getGameObject(iFALL_INDEX)->DieRegen = 1;
				getGameObject(iFALL_INDEX)->PathCount = 0;

				gGameProtocol.GCDiePlayerSend(&getGameObject(iFALL_INDEX), getGameObject(iFALL_INDEX)->m_Index, 0, 0);
			}
		}
	}
}

void ChaosCastleFinal::SetUserState(CGameObject &Obj, int iState)
{
	if (!ObjectMaxRange(iUserIndex))
		return;

	switch (iState)
	{
	case 0:
		this->m_stCCFData.m_UserData[getGameObject(iUserIndex)->m_nCCF_UserIndex)->m_nUserState = 0;
		break;
	case 1:
		this->m_stCCFData.m_UserData[getGameObject(iUserIndex)->m_nCCF_UserIndex)->m_nUserState = 1;
		break;
	}
}

int ChaosCastleFinal::GetCurPlayUser()
{
	int iPlayUser = 0;

	for (int i = 0; i<70; i++)
	{
		if (this->m_stCCFData.m_UserData[i].m_nIndex != -1)
		{
			if (gObjIsConnected(this->m_stCCFData.m_UserData[i].m_nIndex) == TRUE &&
				getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
			{
				iPlayUser++;
			}
		}
	}

	return iPlayUser;
}

int ChaosCastleFinal::GetFinalRemainTime(CGameObject &Obj)
{
	CCF_START_TIME pRET;
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	int iREMAIN_MINUTE = 0;

	for (std::vector<CCF_START_TIME>::iterator it = this->m_vtCCFOpenTime.begin(); it != this->m_vtCCFOpenTime.end(); it++)
	{
		pRET = *it;

		if (pRET.m_nFinal == 3)
		{
			iREMAIN_MINUTE = (60 * pRET.m_nMinute + 3600 * pRET.m_nHour - (sysTime.wSecond + 60 * sysTime.wMinute + 3600 * sysTime.wHour)) / 60 + 1;
		}
	}

	return iREMAIN_MINUTE;
}

int ChaosCastleFinal::GetRemainTime(CGameObject &Obj)
{
	int iREMAIN_MINUTE = 0;

	if (this->GetCurrentState() == 1)
	{
		iREMAIN_MINUTE = this->m_stCCFData.m_nCCF_REMAIN_MSEC / 60000 - this->m_iCCF_TIME_MIN_OPEN + 1;
	}
	else
	{
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		int iMIN_HOUR = 24;
		int iMIN_MINUTE = 60;
		BOOL bTIME_CHANGED = FALSE;
		std::vector<CCF_START_TIME>::iterator it;

		for (it = this->m_vtCCFOpenTime.begin(); it != this->m_vtCCFOpenTime.end(); it++)
		{
			CCF_START_TIME & pRET = *it;

			if ((sysTime.wHour * 60 + sysTime.wMinute) < (pRET.m_nHour * 60 + pRET.m_nMinute))
			{
				if ((iMIN_HOUR * 60 + iMIN_MINUTE) > (pRET.m_nHour * 60 + pRET.m_nMinute))
				{
					bTIME_CHANGED = 1;
					iMIN_HOUR = pRET.m_nHour;
					iMIN_MINUTE = pRET.m_nMinute;
				}
			}
		}


		if (bTIME_CHANGED == 0)
		{
			it = this->m_vtCCFOpenTime.begin();
			iMIN_HOUR = 24;
			iMIN_MINUTE = 60;

			for (; it != this->m_vtCCFOpenTime.end(); it++)
			{
				CCF_START_TIME & pRET = *it;

				if ((iMIN_HOUR * 60 + iMIN_MINUTE) > (pRET.m_nHour * 60 + pRET.m_nMinute))
				{
					bTIME_CHANGED = 2;
					iMIN_HOUR = pRET.m_nHour;
					iMIN_MINUTE = pRET.m_nMinute;
				}
			}
		}

		switch (bTIME_CHANGED)
		{
		case 1:
			iREMAIN_MINUTE = (((iMIN_HOUR * 60 * 60 + iMIN_MINUTE * 60) - (sysTime.wHour * 60 * 60 + sysTime.wMinute * 60 + sysTime.wSecond))) / 60 + 1;
			break;
		case 2:
			iREMAIN_MINUTE = ((((iMIN_HOUR + 24) * 60 * 60 + iMIN_MINUTE * 60) - (sysTime.wHour * 60 * 60 + sysTime.wMinute * 60 + sysTime.wSecond))) / 60 + 1;
			break;
		default:
			iREMAIN_MINUTE = -1;
			break;
		}
	}

	if (iREMAIN_MINUTE < 0)
		iREMAIN_MINUTE = 0;

	return iREMAIN_MINUTE;
}

void ChaosCastleFinal::SendNoticeState(int iPlayState)
{
	PMSG_STATEBLOODCASTLE pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x9B, sizeof(PMSG_STATEBLOODCASTLE));
	pMsg.btPlayState = iPlayState;
	pMsg.wRemainSec = this->m_stCCFData.m_nCCF_REMAIN_MSEC / 1000;
	pMsg.wMaxKillMonster = this->m_stCCFData.m_nCCF_FIRST_USER_COUNT + this->m_stCCFData.m_nCCF_FIRST_MONSTER_COUNT;
	pMsg.wCurKillMonster = this->GetCurPlayUser() + this->GetMonsterListCount();
	pMsg.wUserHaveWeapon = -1;
	pMsg.btWeaponNum = -1;

	for (int i = 0; i < 70; i++)
	{
		if (this->m_stCCFData.m_UserData[i].m_nIndex != -1)
		{
			if (getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->Connected > PLAYER_LOGGED)
			{
				if (getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->m_nCCF_UserIndex != -1)
				{
					IOCP.DataSend(this->m_stCCFData.m_UserData[i].m_nIndex, (UCHAR*)&pMsg, pMsg.h.size);
				}
			}
		}
	}
}

void ChaosCastleFinal::SearchCCFNDropMonsterItem(int iMonsterIndex, int iMaxHitUserIndex)
{
	if (ObjectMaxRange(iMonsterIndex) == false)
	{
		return;
	}

	if (ObjectMaxRange(iMaxHitUserIndex) == false)
	{
		return;
	}

	if (this->m_stCCFData.m_nCCF_STATE != 2)
	{
		return;
	}

	EnterCriticalSection(&this->m_stCCFData.m_CCFDropCriti);

	std::map<int, _CCF_MONSTER_ITEM_DROP>::iterator it = this->m_stCCFData.m_mapCCFMonsterItemList.find(iMonsterIndex);

	if (it == this->m_stCCFData.m_mapCCFMonsterItemList.end())
	{
		LeaveCriticalSection(&this->m_stCCFData.m_CCFDropCriti);
		return;
	}

	int iItemType = it->second.m_nItemKind;
	int iLevel = 0;

	if (iItemType == ITEMGET(13, 15))
	{
		iLevel = rand() % 4;
	}

	BOOL bDrop = FALSE;
	int nRand = rand() % 100;

	if (nRand >= this->m_iCCF_BoxRatio1 || this->m_byBoxDropCnt)
	{
		if (this->m_iCCF_BoxRatio1 >= nRand || nRand >= this->m_iCCF_BoxRatio2 || this->m_byBoxDropCnt != 1)
		{
			if (this->m_iCCF_BoxRatio2 >= nRand || nRand >= this->m_iCCF_BoxRatio3 || this->m_byBoxDropCnt != 2)
			{
				if (this->m_iCCF_BoxRatio3 >= nRand || nRand >= this->m_iCCF_BoxRatio4 || this->m_byBoxDropCnt != 3)
				{
					if (this->m_iCCF_BoxRatio4 >= nRand || nRand >= this->m_iCCF_BoxRatio5 || this->m_byBoxDropCnt != 4)
					{
						if (this->m_iCCF_BoxRatio5 >= nRand || nRand >= this->m_iCCF_BoxRatio6 || this->m_byBoxDropCnt != 5)
						{
							if (this->m_byBoxDropCnt > 6)
							{
								return;
							}
						}
			
						else
						{
							bDrop = TRUE;
							this->m_byBoxDropCnt++;
						}
					}
		
					else
					{
						bDrop = TRUE;
						this->m_byBoxDropCnt++;
					}
				}

				else
				{
					bDrop = TRUE;
					this->m_byBoxDropCnt++;
				}
			}
			
			else
			{
				bDrop = TRUE;
				this->m_byBoxDropCnt++;
			}
		}

		else
		{
			bDrop = TRUE;
			this->m_byBoxDropCnt++;
		}
	}
	
	else
	{
		bDrop = TRUE;
		this->m_byBoxDropCnt++;
	}

	if (bDrop == TRUE)
	{
		ItemCreate(iMaxHitUserIndex, getGameObject(iMaxHitUserIndex)->MapNumber, getGameObject(iMaxHitUserIndex)->X, getGameObject(iMaxHitUserIndex)->Y, iItemType, iLevel, 0, 0, 0, 0, iMaxHitUserIndex, 0, 0, 0, 0, 0);

		sLog->outBasic("[Chaos Castle Survival] Monster Dropped ChaosCastleFinal Item To [%s][%s] (%d)(Item:%s)",
			getGameObject(iMaxHitUserIndex)->AccountID, getGameObject(iMaxHitUserIndex)->Name, iMonsterIndex, ItemAttribute[iItemType)->Name);
	}

	sLog->outBasic("[Chaos Castle Survival][SearchCCFNDropMonsterItem] BoxCount :%d, Random %d ",
		this->m_byBoxDropCnt, nRand);

	LeaveCriticalSection(&this->m_stCCFData.m_CCFDropCriti);
}

void ChaosCastleFinal::SetItemsToMonster(int iCCFType)
{
	int iMAX_MONSTER = this->GetMonsterListCount();

	if (iMAX_MONSTER <= 0)
	{
		return;
	}

	EnterCriticalSection(&this->m_stCCFData.m_CCFDropCriti);

	for (int iB = 0; iB < ::g_iCCF_MonsterItems[iCCFType-1][1]; iB++)
	{
		for (int iC = 0; iC < 200; iC++)
		{
			int iMONSTER_INDEX = this->m_stCCFData.m_lCCF_MONSTER_COUNT[rand() % iMAX_MONSTER];
			std::map<int, _CCF_MONSTER_ITEM_DROP>::iterator it = this->m_stCCFData.m_mapCCFMonsterItemList.find(iMONSTER_INDEX);
			
			if (it == this->m_stCCFData.m_mapCCFMonsterItemList.end())
			{
				_CCF_MONSTER_ITEM_DROP MID;
				
				MID.m_nIndex = iMONSTER_INDEX;
				MID.m_nItemKind = g_iCCF_MonsterItems[iCCFType-1][0];
				this->m_stCCFData.m_mapCCFMonsterItemList.insert(std::pair<int, _CCF_MONSTER_ITEM_DROP>(iMONSTER_INDEX, MID));

				sLog->outBasic("[Chaos Castle Survival] Set Monster Drop Item (%d)(Item:%s)",
					iMONSTER_INDEX, ItemAttribute[MID.m_nItemKind].Name);
				break;
			}
		}
	}

	LeaveCriticalSection(&this->m_stCCFData.m_CCFDropCriti);
	
	sLog->outBasic("[Chaos Castle Survival] Set Monster Drop Item Count (%d)", this->m_stCCFData.m_mapCCFMonsterItemList.size());
}

int ChaosCastleFinal::SetMonster()
{
	int iNOW_ADDED_MONSTER_COUNT = 0;
	int iMAX_ADDABLE_MONSTER_COUNT = 100 - this->GetCurPlayUser();

	if (iMAX_ADDABLE_MONSTER_COUNT < 0)
		iMAX_ADDABLE_MONSTER_COUNT = 0;

	for (int n = 0; n < this->m_CCF_MOB_CNT_SCRIPT && iNOW_ADDED_MONSTER_COUNT < iMAX_ADDABLE_MONSTER_COUNT; n++)
	{
		if (this->m_CCFMP[n].m_MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
		{
			WORD btMonsterType = this->m_CCFMP[n].m_Type;
			int result = gObjAddMonster(this->m_CCFMP[n].m_MapNumber);

			if (result >= 0)
			{
				getGameObject(result)->m_PosNum = n;
				getGameObject(result)->X = this->m_CCFMP[n)->m_X;
				getGameObject(result)->Y = this->m_CCFMP[n)->m_Y;
				getGameObject(result)->MapNumber = this->m_CCFMP[n)->m_MapNumber;
				getGameObject(result)->TX = getGameObject(result)->X;
				getGameObject(result)->TY = getGameObject(result)->Y;
				getGameObject(result)->m_OldX = getGameObject(result)->X;
				getGameObject(result)->m_OldY = getGameObject(result)->Y;
				getGameObject(result)->Dir = this->m_CCFMP[n)->m_Dir;
				getGameObject(result)->StartX = getGameObject(result)->X;
				getGameObject(result)->StartY = getGameObject(result)->Y;
				gObjSetMonster(result, btMonsterType);
				getGameObject(result)->MaxRegenTime = 0;
				getGameObject(result)->Dir = rand() % 8;

				iNOW_ADDED_MONSTER_COUNT++;
				this->AddMonsterList(result);
			}

			sLog->outBasic("gObjAddMonster return %d", result);
		}
	}

	return iNOW_ADDED_MONSTER_COUNT;
}

void ChaosCastleFinal::AddMonsterList(int iMonsterIndex)
{
	for (int iMON = 0; iMON < 100; iMON++)
	{
		if (this->m_stCCFData.m_lCCF_MONSTER_COUNT[iMON] == -1)
		{
			InterlockedExchange(&this->m_stCCFData.m_lCCF_MONSTER_COUNT[iMON], iMonsterIndex);
			return;
		}
	}
}

void ChaosCastleFinal::DelMonsterList(int iMonsterIndex)
{
	sLog->outBasic("[Chaos Castle Survival] [Bug Tracker] MONSTER DEL Count(%d) : ENTER - Index(%d)",
		this->m_stCCFData.m_lCCF_CURRENT_MONSTER_COUNT, iMonsterIndex);

	for (int iMON = 0; iMON < 100; iMON++)
	{
		if (this->m_stCCFData.m_lCCF_MONSTER_COUNT[iMON] == iMonsterIndex)
		{
			InterlockedExchange(&this->m_stCCFData.m_lCCF_MONSTER_COUNT[iMON], -1);
			break;
		}
	}

	InterlockedDecrement(&this->m_stCCFData.m_lCCF_CURRENT_MONSTER_COUNT);

	if (this->m_stCCFData.m_lCCF_CURRENT_MONSTER_COUNT < 0)
	{
		this->m_stCCFData.m_lCCF_CURRENT_MONSTER_COUNT = 0;
	}

	sLog->outBasic("[Chaos Castle Survival] [Bug Tracker] MONSTER DEL Count(%d) : OUT - Index(%d)",
		this->m_stCCFData.m_lCCF_CURRENT_MONSTER_COUNT, iMonsterIndex);
}

int ChaosCastleFinal::ReCalcUserCount()
{
	int iPlayUser = this->GetCurPlayUser();
	int iRET_VAL;

	if (iPlayUser <= 0)
	{
		this->m_stCCFData.m_nCCF_ENTERED_USER = 0;
		iRET_VAL = 0;
	}
	else
	{
		this->m_stCCFData.m_nCCF_ENTERED_USER = iPlayUser;
		iRET_VAL = iPlayUser;
	}

	return iRET_VAL;
}

void ChaosCastleFinal::GetRandomDropPos(BYTE & btPosX, BYTE & btPosY)
{
	int nRandValue = 0;

	for (int n = 0; n < 5; n++)
	{
		int nDiffX = g_iCCF_Jewel_Random_Drop[n][4] - btPosX;
		int nDiffY = g_iCCF_Jewel_Random_Drop[n][5] - btPosY;

		int nDiffPos = (nDiffX * nDiffX) - (nDiffY - nDiffY);

		if (nDiffPos <= 0)
			nRandValue = n;
	}

	btPosX = GetLargeRand() % (g_iCCF_Jewel_Random_Drop[nRandValue][2] - g_iCCF_Jewel_Random_Drop[nRandValue][0] + g_iCCF_Jewel_Random_Drop[nRandValue][0]);
	btPosY = GetLargeRand() % (g_iCCF_Jewel_Random_Drop[nRandValue][3] - g_iCCF_Jewel_Random_Drop[nRandValue][1] + g_iCCF_Jewel_Random_Drop[nRandValue][1]);
}

static const BYTE g_byCCFSafetyZoneMapXY[4] = { 23, 75, 44, 108 };
static const BYTE g_byCCFCenterHollowZoneMapXY[2][4] = { 32, 84, 35, 87, 32, 96, 35, 99 };

void ChaosCastleFinal::SendCastleZoneSafetyInfo(bool bDoSet)
{
	char cTEMP_BUF[256];
	PMSG_SETMAPATTR_COUNT * lpMsg = (PMSG_SETMAPATTR_COUNT *)cTEMP_BUF;

	PHeadSetB((BYTE*)lpMsg, 0x46, sizeof(PMSG_SETMAPATTR_COUNT) + sizeof(PMSG_SETMAPATTR) * 6);
	PMSG_SETMAPATTR * lpMsgBody = (PMSG_SETMAPATTR *)&cTEMP_BUF[7];
	lpMsg->btType = 0;
	lpMsg->btCount = 1;
	lpMsg->btMapAttr = 1;
	(bDoSet) ? (lpMsg->btMapSetType = 0) : (lpMsg->btMapSetType = 1);

	lpMsgBody[0].btX = ::g_byCCFSafetyZoneMapXY[0];
	lpMsgBody[0].btY = ::g_byCCFSafetyZoneMapXY[1];
	lpMsgBody[1].btX = ::g_byCCFSafetyZoneMapXY[2];
	lpMsgBody[1].btY = ::g_byCCFSafetyZoneMapXY[3];

	for each (std::pair<int,CGameObject*> ObjEntry in gGameObjects)
	{
		if (getGameObject(i)->Connected > PLAYER_LOGGED)
		{
			IOCP.DataSend(i, (BYTE*)lpMsg, lpMsg->h.size);
		}
	}
}

void ChaosCastleFinal::UnSafetyCastleZone()
{
	for (int i = g_byCCFSafetyZoneMapXY[0]; i <= g_byCCFSafetyZoneMapXY[2]; i++)
	{
		for (int j = g_byCCFSafetyZoneMapXY[1]; j <= g_byCCFSafetyZoneMapXY[3]; j++)
		{
			MapC[MAP_INDEX_CHAOSCASTLE_SURVIVAL].m_attrbuf[j * 256 + i] &= ~1;
		}
	}

	for (int iHOLE_NUM = 0; iHOLE_NUM <2; iHOLE_NUM++)
	{
		for (int i = g_byCCFCenterHollowZoneMapXY[iHOLE_NUM][0]; i <= g_byCCFCenterHollowZoneMapXY[iHOLE_NUM][2]; i++)
		{
			for (int j = g_byCCFCenterHollowZoneMapXY[iHOLE_NUM][1]; j <= g_byCCFCenterHollowZoneMapXY[iHOLE_NUM][3]; j++)
			{
				MapC[MAP_INDEX_CHAOSCASTLE_SURVIVAL].m_attrbuf[j * 256 + i] |= 8;
			}
		}
	}
}

void ChaosCastleFinal::SendChaosCastleAnyMsg(BYTE* lpMsg, int iSize)
{
	for (int i = 0; i < 70; i++)
	{
		if (this->m_stCCFData.m_UserData[i].m_nIndex != -1)
		{
			if (gObjIsConnected(this->m_stCCFData.m_UserData[i].m_nIndex) != FALSE)
			{
				if (getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->m_nCCF_UserIndex != -1)
				{
					IOCP.DataSend(this->m_stCCFData.m_UserData[i].m_nIndex, lpMsg, iSize);
				}
			}
		}
	}
}

void ChaosCastleFinal::ClearMonster()
{
	for (int n = 0; n < g_ConfigRead.server.GetObjectMaxMonster(); n++)
	{
		if (getGameObject(n)->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
		{
			gObjDel(n);
		}
	}

	sLog->outBasic("[Chaos Castle Survival]  Clear Monster");
}

void ChaosCastleFinal::CheckSync()
{
	if (this->m_vtCCFOpenTime.empty() != false)
	{
		sLog->outError( "[Chaos Castle Survival] No Schedule Time Data - Chaos Castle Survival Terminated (m_vtCCFOpenTime.empty())");
		this->SetState(0, 0);
		return;
	}

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	int iMIN_HOUR = 24;
	int iMIN_MINUTE = 60;
	BOOL bTIME_CHANGED = FALSE;
	std::vector<CCF_START_TIME>::iterator it;

	for (it = this->m_vtCCFOpenTime.begin(); it != this->m_vtCCFOpenTime.end(); it++)
	{
		CCF_START_TIME & stCCTime = *it;

		if ((sysTime.wHour * 60 + sysTime.wMinute) < (stCCTime.m_nHour * 60 + stCCTime.m_nMinute))
		{
			if ((iMIN_HOUR * 60 + iMIN_MINUTE) > (stCCTime.m_nHour * 60 + stCCTime.m_nMinute))
			{
				bTIME_CHANGED = TRUE;
				iMIN_HOUR = stCCTime.m_nHour;
				iMIN_MINUTE = stCCTime.m_nMinute;
			}
		}
	}

	if (bTIME_CHANGED == FALSE)
	{
		it = this->m_vtCCFOpenTime.begin();
		iMIN_HOUR = 24;
		iMIN_MINUTE = 60;

		for (; it != this->m_vtCCFOpenTime.end(); it++)
		{
			CCF_START_TIME & stCCTime = *it;

			if ((iMIN_HOUR * 60 + iMIN_MINUTE) > (stCCTime.m_nHour * 60 + stCCTime.m_nMinute))
			{
				bTIME_CHANGED = 2;
				iMIN_HOUR = stCCTime.m_nHour;
				iMIN_MINUTE = stCCTime.m_nMinute;
			}
		}
	}

	switch (bTIME_CHANGED)
	{
	case TRUE:
		this->m_stCCFData.m_nCCF_REMAIN_MSEC = (((iMIN_HOUR * 60 * 60) + (iMIN_MINUTE * 60)) - ((sysTime.wHour * 60 * 60) + (sysTime.wMinute * 60) + (sysTime.wSecond))) * 1000;
		break;

	case 2:
		this->m_stCCFData.m_nCCF_REMAIN_MSEC = ((((iMIN_HOUR + 24) * 60 * 60) + (iMIN_MINUTE * 60)) - ((sysTime.wHour * 60 * 60) + (sysTime.wMinute * 60) + (sysTime.wSecond))) * 1000;
		break;

	default:
		sLog->outError( "[Chaos Castle Survival] No Schedule Time Data - Chaos Castle Terminated (bTIME_CHANGED=%d)", bTIME_CHANGED);
		this->SetState(0, 0);
		return;

	}

	this->m_stCCFData.m_nCCF_TICK_COUNT = GetTickCount();

}

void ChaosCastleFinal::SafetyCastleZone()
{
	for (int i = g_byCCFSafetyZoneMapXY[0]; i <= g_byCCFSafetyZoneMapXY[2]; i++)
	{
		for (int j = g_byCCFSafetyZoneMapXY[1]; j <= g_byCCFSafetyZoneMapXY[3]; j++)
		{
			MapC[MAP_INDEX_CHAOSCASTLE_SURVIVAL].m_attrbuf[j * 256 + i] |= 1;
		}
	}

	for (int iHOLE_NUM = 0; iHOLE_NUM <2; iHOLE_NUM++)
	{
		for (int i = g_byCCFCenterHollowZoneMapXY[iHOLE_NUM][0]; i <= g_byCCFCenterHollowZoneMapXY[iHOLE_NUM][2]; i++)
		{
			for (int j = g_byCCFCenterHollowZoneMapXY[iHOLE_NUM][1]; j <= g_byCCFCenterHollowZoneMapXY[iHOLE_NUM][3]; j++)
			{
				MapC[MAP_INDEX_CHAOSCASTLE_SURVIVAL].m_attrbuf[j * 256 + i] |= 8;
			}
		}
	}
}

static const int g_nCCF_ExpTable[2] = {4000, 5000};

void ChaosCastleFinal::RewardUserEXP(BOOL bWinner)
{
	for (int i = 0; i < 70; i++)
	{
		if (this->m_stCCFData.m_UserData[i].m_nIndex != -1
			&& gObjIsConnected(this->m_stCCFData.m_UserData[i].m_nIndex) == TRUE)
		{
			if (getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->m_nCCF_UserIndex == -1)
				return;

			int iKILLCOUNT_USER = getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->m_byKillUserCount;
			int iKILLCOUNT_MONSTER = getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->m_byKillMobCount;
			int iTOT_EXP = iKILLCOUNT_USER * g_nCCF_ExpTable[0] + iKILLCOUNT_MONSTER * g_nCCF_ExpTable[1];
			int iREWARD_EXP = this->CalcSendRewardEXP(this->m_stCCFData.m_UserData[i].m_nIndex, iTOT_EXP, iKILLCOUNT_USER, iKILLCOUNT_MONSTER);

			sLog->outBasic("[Chaos Castle Survival] [%s][%s] Reward User EXP (USER_KILL:%d, MON_KILL:%d, TOT_EXP:%d, TOT_REWARD_EXP:%d)",
				getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex]->AccountID, getGameObject(this->m_stCCFData.m_UserData[i)->m_nIndex)->Name,
				iKILLCOUNT_USER, iKILLCOUNT_MONSTER, iTOT_EXP, iREWARD_EXP);
		}
	}
}

int ChaosCastleFinal::ObjSetPosition(CGameObject &Obj, int iX, int iY)
{
	if (!ObjectMaxRange(Obj.m_Index))
		return TRUE;

	CGameObject lpObj = Obj;

	if (Obj.MapNumber != MAP_INDEX_CHAOSCASTLE_SURVIVAL)
		return FALSE;

	if (Obj.Teleport)
		return TRUE;

	BYTE btMapAttr = MapC[Obj.MapNumber].GetAttr(iX, iY);

	if ((btMapAttr & 2) == 2 || (btMapAttr & 4) == 4)
		return FALSE;

	PMSG_POSISTION_SET pMove;

	pMove.h.c = 0xC1;
	pMove.h.headcode = 0x10;
	pMove.h.size = sizeof(pMove);
	pMove.X = iX;
	pMove.Y = iY;

	Obj.m_Rest = 0;

	MapC[Obj.MapNumber].ClearStandAttr(Obj.m_OldX, Obj.m_OldY);
	MapC[Obj.MapNumber].SetStandAttr(iX, iY);

	Obj.m_OldX = iX;
	Obj.m_OldY = iY;

	Obj.X = pMove.X;
	Obj.Y = pMove.Y;

	PMSG_RECV_POSISTION_SET pMove2;

	PHeadSetB((BYTE*)&pMove2, 0x10, sizeof(pMove2));
	pMove2.NumberH = SET_NUMBERH(Obj.m_Index);
	pMove2.NumberL = SET_NUMBERL(Obj.m_Index);
	pMove2.X = pMove.X;
	pMove2.Y = pMove.Y;
	Obj.TX = pMove.X;
	Obj.TY = pMove.Y;

	CreateFrustrum(Obj.X, Obj.Y, iIndex);

	if (Obj.Type == OBJ_USER)
	{
		IOCP.DataSend(Obj.m_Index, (UCHAR *)&pMove2, pMove2.h.size);
	}

	gGameProtocol.MsgSendV2(Obj, (UCHAR *)&pMove2, pMove2.h.size);

	MapC[Obj.MapNumber].ClearStandAttr(Obj.m_OldX, Obj.m_OldY);
	MapC[Obj.MapNumber].SetStandAttr(Obj.TX, Obj.TY);

	Obj.m_OldX = Obj.TX;
	Obj.m_OldY = Obj.TY;

	return TRUE;
}

void ChaosCastleFinal::GiveUserDamage(CGameObject &Obj, int iDamage)
{
	if (!gObjIsConnected(iUserIndex))
		return;

	if (getGameObject(iUserIndex)->Life <= 0.0)
		return;

	PMSG_ATTACKRESULT pResult;

	PHeadSetB((BYTE*)&pResult, 0x11, sizeof(pResult));
	pResult.NumberH = SET_NUMBERH(iUserIndex);
	pResult.NumberL = SET_NUMBERL(iUserIndex);
	pResult.DamageH = SET_NUMBERH(iDamage);
	pResult.DamageL = SET_NUMBERL(iDamage);
	pResult.btShieldDamageH = 0;
	pResult.btShieldDamageL = 0;
	pResult.IGCDamage = iDamage;
	
	if (getGameObject(iUserIndex)->Type == OBJ_USER)
		IOCP.DataSend(iUserIndex, (BYTE*)&pResult, pResult.h.size);

	getGameObject(iUserIndex)->Life -= iDamage;

	if (getGameObject(iUserIndex)->Life <= 0.0)
	{
		getGameObject(iUserIndex)->Life = 0;
		getGameObject(iUserIndex)->m_State = 4;
		getGameObject(iUserIndex)->KillerType = 1;
		getGameObject(iUserIndex)->RegenTime = GetTickCount();
		getGameObject(iUserIndex)->DieRegen = 1;
		getGameObject(iUserIndex)->PathCount = 0;

		gGameProtocol.GCDiePlayerSend(&getGameObject(iUserIndex), getGameObject(iUserIndex)->m_Index, 0, 0);
	}
}

void ChaosCastleFinal::SendFailMessage(int iLoserIndex)
{
	if (!ObjectMaxRange(iLoserIndex))
	{
		return;
	}

	if (getGameObject(iLoserIndex)->m_bCCF_Quit_Msg == true)
	{
		return;
	}

	int nPoint = getGameObject(iLoserIndex)->m_byKillMobCount + 2 * getGameObject(iLoserIndex)->m_byKillUserCount;
	this->GD_Req_Save_CCF_Result(iLoserIndex, getGameObject(iLoserIndex)->Name, nPoint, this->Check_CCF_DayType());
	gGameProtocol.GCServerMsgStringSend(Lang.GetText(0,559), iLoserIndex, 1);

	PMSG_ANS_CCF_RESULT pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xAF, 0x09, sizeof(pMsg));
	pMsg.point = nPoint;
	pMsg.result = 2;
	pMsg.type = this->Check_CCF_DayType();
	pMsg.MobKillCnt = getGameObject(iLoserIndex)->m_byKillMobCount;
	pMsg.PCKillCnt = getGameObject(iLoserIndex)->m_byKillUserCount;

	IOCP.DataSend(iLoserIndex, (BYTE*)&pMsg, pMsg.h.size);
	sLog->outBasic("[Chaos Castle Survival][GAME-RESULT] NAME:[%s],MOBKILL:[%d],USERKILL:[%d],POINT[%d]",
		getGameObject(iLoserIndex)->Name, getGameObject(iLoserIndex)->m_byKillMobCount, getGameObject(iLoserIndex)->m_byKillUserCount, nPoint);

	getGameObject(iLoserIndex)->m_bCCF_Quit_Msg = true;
}

int ChaosCastleFinal::CalcSendRewardEXP(CGameObject &Obj, int iEXP, int iKILLCOUNT_USER, int iKILLCOUNT_MONSTER)
{
	if (iEXP <= 0)
		return 0;

	if (g_CrywolfSync.GetOccupationState() == 1 && g_CrywolfSync.GetApplyPenalty() == TRUE)
	{
		iEXP = iEXP * g_CrywolfSync.GetGettingExpPenaltyRate() / 100;
	}

	UINT64 iRET_EXP = 0;
	UINT64 iCAL_EXP = iEXP;
	int iMAX_LEVCOUNT = 0;

	if (!gObjIsConnected(iUserIndex))
		return 0;

	if (g_MasterLevelSkillTreeSystem.IsMasterLevelUser(&getGameObject(iUserIndex)) == false)
	{
		if (iCAL_EXP > getGameObject(iUserIndex)->m_PlayerData->Experience + getGameObject(iUserIndex)->m_PlayerData->NextExp)
		{
			iCAL_EXP = getGameObject(iUserIndex)->m_PlayerData->NextExp;
		}
	}
	else
	{
		if (iCAL_EXP > getGameObject(iUserIndex)->m_PlayerData->MasterExperience + getGameObject(iUserIndex)->m_PlayerData->MasterNextExp)
		{
			iCAL_EXP = getGameObject(iUserIndex)->m_PlayerData->MasterNextExp;
		}
	}

	iRET_EXP = iCAL_EXP;

	if (getGameObject(iUserIndex)->Type == OBJ_USER)
	{
		while (iCAL_EXP > 0)
		{
			if (iCAL_EXP > 0)
			{
				CheckItemOptForGetExpExRenewal(&getGameObject(iUserIndex), NULL, iCAL_EXP, 0, true);

				iCAL_EXP = this->LevelUp(iUserIndex, iCAL_EXP);
			}

			if (iCAL_EXP == 0)
			{
				return iRET_EXP;
			}

			iMAX_LEVCOUNT++;

			if (iMAX_LEVCOUNT > 1000)
				break;
		}

		PMSG_KILLPLAYER_EXT pkillMsg;

		PHeadSetBE((BYTE*)&pkillMsg, 0x9C, sizeof(pkillMsg));
		pkillMsg.NumberH = -1;
		pkillMsg.NumberL = -1;
		pkillMsg.ExpH = SET_NUMBERHW(iEXP);
		pkillMsg.ExpL = SET_NUMBERLW(iEXP);
		pkillMsg.DamageH = 0;
		pkillMsg.DamageL = 0;

		IOCP.DataSend(iUserIndex, (BYTE*)&pkillMsg, pkillMsg.h.size);
	}

	return iRET_EXP;
}

BOOL ChaosCastleFinal::LevelUp(CGameObject &Obj, int iAddExp)
{
	if (ObjectMaxRange(iUserIndex) == FALSE)
	{
		return 0;
	}

	if (g_MasterLevelSkillTreeSystem.IsMasterLevelUser(&getGameObject(iUserIndex)))
	{
		g_MasterLevelSkillTreeSystem.MasterLevelUp(&getGameObject(iUserIndex), iAddExp, 0, "Chaos Castle Survival");
		return 0;
	}

	int iLEFT_EXP = 0;

	sLog->outBasic("[Chaos Castle Survival] Experience : [%s][%s](%d) Experience: %d + %d",
		getGameObject(iUserIndex)->AccountID, getGameObject(iUserIndex)->Name,
		getGameObject(iUserIndex)->Level, getGameObject(iUserIndex)->m_PlayerData->Experience,
		iAddExp);

	::gObjSetExpPetItem(iUserIndex, iAddExp);

	if (getGameObject(iUserIndex)->Level >= g_ConfigRead.data.common.UserMaxLevel)
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0,45), getGameObject(iUserIndex)->m_Index, 1);
		return 0;
	}

	if ((getGameObject(iUserIndex)->m_PlayerData->Experience + iAddExp) < getGameObject(iUserIndex)->m_PlayerData->NextExp)
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

				//sLog->outBasic("[%s][%s] LevelUp PlusStatQuest Clear AddStat %d",getGameObject(iUserIndex)->AccountID, getGameObject(iUserIndex)->Name, getGameObject(iUserIndex)->m_PlayerData->LevelUpPoint);
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
		sLog->outBasic("Level Up [%s][%s][%d]", getGameObject(iUserIndex)->AccountID, getGameObject(iUserIndex)->Name, getGameObject(iUserIndex)->Level);
		return 0;
	}

	return iLEFT_EXP;
}

int ChaosCastleFinal::CCF_Ranking_DayType()
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	int nReturnDayType = 0;

	if (sysTime.wDay != this->m_byTryOut1 && sysTime.wDay != this->m_byTryOut2 && sysTime.wDay != this->m_byTryOut3)
	{
		if (sysTime.wDay == this->m_bySemiFinal1 || sysTime.wDay == this->m_bySemiFinal2 || sysTime.wDay == this->m_byFinal)
		{
			nReturnDayType = 2;
		}
	}

	else
	{
		nReturnDayType = 1;
	}

	return nReturnDayType;
}

int ChaosCastleFinal::Check_CCF_DayType()
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	int nReturnDayType = 0;

	if (sysTime.wDay != this->m_byTryOut1 && sysTime.wDay != this->m_byTryOut2 && sysTime.wDay != this->m_byTryOut3)
	{
		if (sysTime.wDay != this->m_bySemiFinal1 && sysTime.wDay != this->m_bySemiFinal2)
		{
			if (sysTime.wDay == this->m_byFinal)
				nReturnDayType = 3;
			else
				nReturnDayType = 0;
		}

		else
		{
			nReturnDayType = 2;
		}
	}

	else
	{
		nReturnDayType = 1;
	}

	return nReturnDayType;
}

bool ChaosCastleFinal::CheckCanStart_CCF()
{
	int nEnteredUserCount;

	nEnteredUserCount = this->GetCurPlayUser();

	if (nEnteredUserCount >= this->m_nCCF_MinUserCount)
	{
		return true;
	}

	else
	{
		sLog->outBasic("[Chaos Castle Survival][CheckCanStart_CCS] GetCurPlayUser() FAILED (UserCount:%d) (NeedMinCount:%d)", nEnteredUserCount, this->m_nCCF_MinUserCount);
		return false;
	}
}

static int g_nCCF_EnterFee[3] = { 0, 0, 0 };

void ChaosCastleFinal::CCF_Start_Fail_So_RollBack()
{
	int nCCF_Type = this->Check_CCF_DayType();

	for (int i = 0; i < 70; i++)
	{
		if (this->m_stCCFData.m_UserData[i].m_nIndex != -1 && gObjIsConnected(this->m_stCCFData.m_UserData[i].m_nIndex) == TRUE && getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
		{
			if (this->m_stCCFData.m_UserData[i].m_nIndex != -1)
			{
				int iFIRST_MONEY = getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->m_PlayerData->Money;
				int iPAYBACK_MONEY = g_nCCF_EnterFee[nCCF_Type];
				getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->m_PlayerData->Money += iPAYBACK_MONEY;
				int iNOW_MONEY = getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->m_PlayerData->Money;

				gGameProtocol.GCMoneySend(this->m_stCCFData.m_UserData[i].m_nIndex, getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->m_PlayerData->Money);
				gObjMoveGate(this->m_stCCFData.m_UserData[i].m_nIndex, 333);
				sLog->outBasic("[Chaos Castle Survival][CCF_Start_Fail_So_RollBack] INDEX: %d ", this->m_stCCFData.m_UserData[i].m_nIndex);

				PMSG_NOTICE pNotice;
				TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0,592));
				IOCP.DataSend(this->m_stCCFData.m_UserData[i].m_nIndex, (BYTE*)&pNotice, pNotice.h.size);
			}
		}
	}
}

void ChaosCastleFinal::Send_CCF_Schedule(CGameObject &Obj)
{
	char sendbuf[4096];
	memset(sendbuf, 0x00, sizeof(sendbuf));

	PMSG_ANS_CCF_TIME pMsgTime;
	PMSG_ANS_CCF_SCHEDULE pMsg;
	int lOfs = sizeof(pMsg);

	PHeadSubSetW((BYTE*)&pMsg, 0xAF, 0x03, sizeof(pMsg));
	pMsg.byCCFType = this->Check_CCF_DayType();
	pMsg.Tryout1 = this->m_byTryOut1;
	pMsg.Tryout2 = this->m_byTryOut2;
	pMsg.Tryout3 = this->m_byTryOut3;
	pMsg.SemiFinal1 = this->m_bySemiFinal1;
	pMsg.SemiFinal2 = this->m_bySemiFinal2;
	pMsg.Final = this->m_byFinal;

	int nCount = 0;

	for (std::vector<CCF_START_TIME>::iterator it = this->m_vtCCFOpenTime.begin(); it != this->m_vtCCFOpenTime.end(); it++)
	{
		pMsgTime.Hour = it->m_nHour;
		pMsgTime.Min = it->m_nMinute;
		pMsgTime.TryOut = it->m_nTryout;
		pMsgTime.SemiFinal = it->m_nSemiFinal;
		pMsgTime.Final = it->m_nFinal;
		nCount++;

		memcpy(&sendbuf[lOfs], &pMsgTime, sizeof(pMsgTime));
		lOfs += sizeof(pMsgTime);
	}

	pMsg.nCount = nCount;
	pMsg.h.sizeH = HIBYTE(lOfs);
	pMsg.h.sizeL = LOBYTE(lOfs);
	memcpy(&sendbuf, &pMsg, sizeof(pMsg));
	IOCP.DataSend(Obj.m_Index, (BYTE*)sendbuf, lOfs);
}

int ChaosCastleFinal::EnterCheck_CCF_User(CGameObject &Obj)
{
	for (int i = 0; i < 70; i++)
	{
		if (this->m_stCCFData.m_UserData[i].m_nIndex > 0 && this->m_stCCFData.m_UserData[i].m_nIndex == iUserIndex)
		{
			return -2;
		}

		if (this->m_stCCFData.m_UserData[i].m_nIndex > 0 && this->m_stCCFData.m_nCCF_ENTERED_USER > 70)
		{
			this->m_stCCFData.m_nCCF_ENTERED_USER = 70;
			return -1;
		}
	}

	return 0;
}

BOOL ChaosCastleFinal::CheckUserEnterMoney(CGameObject &Obj, BYTE byBattleType)
{
	int nEnterFee = g_nCCF_EnterFee[byBattleType];

	if (getGameObject(iUserIndex)->m_PlayerData->Money < nEnterFee)
	{
		sLog->outBasic("[Chaos Castle Survival] %s don't have EnterCCF_Fee: %d , User Money: %d ",
			getGameObject(iUserIndex)->Name, nEnterFee, getGameObject(iUserIndex)->m_PlayerData->Money);

		return FALSE;
	}

	return TRUE;
}

void ChaosCastleFinal::GD_Req_Save_CCF_Result(int index, char *Name, int Point, int nCCFtype)
{
	if (ObjectMaxRange(index) == false)
	{
		sLog->outBasic("[Chaos Castle Survival][GD_Req_Save_CCF_Result] CHECK_LIMIT index: %d", index);
		return;
	}

	if (gObjIsConnected(index) == false)
	{
		sLog->outBasic("[Chaos Castle Survival][GD_Req_Save_CCF_Result] gObjIsConnected is NOT index: %d", index);
		return;
	}

	if (getGameObject(index)->Type != OBJ_USER)
	{
		sLog->outBasic("[Chaos Castle Survival][GD_Req_Save_CCF_Result] index is NOT USER: %d", index);
		return;
	}

	if (Point < 0)
	{
		sLog->outBasic("[Chaos Castle Survival][GD_Req_Save_CCF_Result] index: %d, Under Zero Point: %d", index, Point);
		return;
	}

	if (Point > 200)
	{
		sLog->outBasic("[Chaos Castle Survival][GD_Req_Save_CCF_Result] index: %d, Over 200 Point: %d", index, Point);
		return;
	}

	PMSG_REQ_SAVE_CCF_RESULT pMsg;
	pMsg.szCharName[MAX_CHARNAME_LEN] = 0;
	memcpy(pMsg.szCharName, getGameObject(index)->Name, MAX_ACCOUNT_LEN);
	PHeadSubSetB((BYTE*)&pMsg, 0xF9, 0xA1, sizeof(pMsg));
	pMsg.nPoint = Point;
	pMsg.nCharClass = getGameObject(index)->Class;
	pMsg.nCharLevel = getGameObject(index)->Level;
	pMsg.nCharExp = getGameObject(index)->m_PlayerData->Experience;

	if (nCCFtype)
	{
		pMsg.byCCFType = nCCFtype;
	}

	else
	{
		pMsg.byCCFType = this->Check_CCF_DayType();
	}

	sLog->outBasic("[Chaos Castle Survival][GD_Req_Save_CCF_Result] pMsg.szCharName:%s, obj.Name :%s", pMsg.szCharName, getGameObject(index)->Name);
	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
}

void ChaosCastleFinal::GD_Renew_CCF_Ranking(BYTE byCCFType)
{
	SDHP_RENEW_RANKING pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xF9;
	pMsg.h.subcode = 0xA4;
	pMsg.byRankingType = byCCFType;

	wsDataCli.DataSend((char *)&pMsg, sizeof(pMsg));
}

void ChaosCastleFinal::GD_Load_CCF_RankingList(BYTE byCCFType)
{
	if (byCCFType != 2 && byCCFType != 1)
	{
		sLog->outBasic("[Chaos Castle Survival][GD_Load_CCF_RankingList] byCCF_Type : %d ", byCCFType);
		return;
	}

	SDHP_REQ_CCF_RANKING pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xF9;
	pMsg.h.subcode = 0xA3;
	pMsg.byRankingType = byCCFType;
	pMsg.nServerCategory = g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE ? TRUE : FALSE;

	wsDataCli.DataSend((char *)&pMsg, sizeof(pMsg));
}

void ChaosCastleFinal::GD_Req_Get_Permission(CGameObject &Obj)
{
	if (ObjectMaxRange(Obj.m_Index) == false)
	{
		return;
	}

	if (gObjIsConnected(Obj.m_Index) == false)
	{
		return;
	}

	PMSG_REQ_CCF_PERMISSION pMsg;

	pMsg.szCharName[MAX_ACCOUNT_LEN] = 0;
	memcpy(pMsg.szCharName, Obj.Name, MAX_ACCOUNT_LEN);
	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xF9;
	pMsg.h.subcode = 0xA2;
	pMsg.byCCFType = this->Check_CCF_DayType() - 1;
	pMsg.nIndex = Obj.m_Index;

	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
	sLog->outBasic("[Chaos Castle Survival][GD_Req_Get_Permission] NAME:%s,CCFTYPE:%d",
		Obj.Name, pMsg.byCCFType);
}

BOOL ChaosCastleFinal::PayUserEnterMoney(CGameObject &Obj, int nCCFType)
{
	if (ObjectMaxRange(iUserIndex) == false)
	{
		return FALSE;
	}

	if (getGameObject(iUserIndex)->m_PlayerData->Money < g_nCCF_EnterFee[nCCFType))
	{
		return FALSE;
	}

	getGameObject(iUserIndex)->m_PlayerData->Money -= g_nCCF_EnterFee[nCCFType);
	gGameProtocol.GCMoneySend(iUserIndex, getGameObject(iUserIndex)->m_PlayerData->Money);

	return TRUE;
}

int ChaosCastleFinal::EnterCCF(CGameObject &Obj, int & nCCFUserIndex)
{
	for (int i = 0; i < 70; i++)
	{
		if (this->m_stCCFData.m_UserData[i].m_nIndex != -1 && this->m_stCCFData.m_UserData[i].m_nIndex == iUserIndex)
			return -2;

		if (this->m_stCCFData.m_UserData[i].m_nIndex == -1)
		{
			if (this->m_stCCFData.m_nCCF_ENTERED_USER <= 70)
			{
				this->m_stCCFData.m_UserData[i].m_nIndex = iUserIndex;
				this->m_stCCFData.m_UserData[i].m_nEXP = 0;
				this->m_stCCFData.m_UserData[i].m_nScore = 0;
				this->m_stCCFData.m_UserData[i].m_nUserState = 0;
				getGameObject(iUserIndex)->m_bCCF_Quit_Msg = false;
				nCCFUserIndex = i;
				this->m_stCCFData.m_nCCF_ENTERED_USER++;

				return 0;
			}

			else
			{
				this->m_stCCFData.m_nCCF_ENTERED_USER = 70;
				return -1;
			}
		}
	}
}

int ChaosCastleFinal::CheckRankingReqValid(BYTE byCCFType)
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	if (byCCFType != 1 || sysTime.wDay != this->m_byTryOut1 && sysTime.wDay != this->m_byTryOut2 && sysTime.wDay != this->m_byTryOut3)
	{
		if (byCCFType != 2 || sysTime.wDay != this->m_bySemiFinal1 && sysTime.wDay != this->m_bySemiFinal2 && sysTime.wDay != this->m_byFinal)
		{
			sLog->outBasic("[Chaos Castle Survival][CheckRankingReqValid] USER req :%d Today :%d ", byCCFType, sysTime.wDay);
			return -1;
		}

		else
		{
			return 0;
		}
	}

	else
	{
		return 0;
	}
}

void ChaosCastleFinal::DG_Ans_CCF_Rank(PMSG_ANS_CCF_RANK * lpMsg)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_BATTLECORE)
	{
		return;
	}

	int nUserCnt = lpMsg->byUserCnt;
	sLog->outBasic("[Chaos Castle Survival] Received ranking, count:%d", nUserCnt);

	for (int i = 0; i < nUserCnt; i++)
	{
		if (lpMsg->RankingInfo[i].byRank <= 0)
		{
			memset(this->m_CCFRanking[i].szCharName, 0x00, MAX_ACCOUNT_LEN + 1);
			this->m_CCFRanking[i].byRank = 0;
			this->m_CCFRanking[i].nPoint = 0;
		}

		else
		{
			memcpy(this->m_CCFRanking[i].szCharName, lpMsg->RankingInfo[i].szCharName, MAX_ACCOUNT_LEN + 1);
			this->m_CCFRanking[i].byRank = lpMsg->RankingInfo[i].byRank;
			this->m_CCFRanking[i].nPoint = lpMsg->RankingInfo[i].nPoint;
			sLog->outBasic("[Chaos Castle Survival] (%d) rank: (Name:%s) (Point:%d)",
				this->m_CCFRanking[i].byRank, this->m_CCFRanking[i].szCharName, this->m_CCFRanking[i].nPoint);
		}
	}

	if (this->m_FirstRankingInfoLoad == false)
	{
		this->m_FirstRankingInfoLoad = true;
	}
}

void ChaosCastleFinal::Send_RankingInfo(BYTE byCCFType, CGameObject &Obj)
{
	_stPMSG_CCF_RANK_INFO pMsg;
	_CCFRankingInfo pRankInfo;
	char sendbuf[4096];

	PHeadSubSetW((BYTE*)&pMsg, 0xAF, 0x07, sizeof(pMsg));
	memset(sendbuf, 0x00, sizeof(sendbuf));
	int lOfs = sizeof(pMsg);
	int nUserCnt = 0;

	int CanIsSeeRankingInfo = this->CheckRankingReqValid(byCCFType);

	if (CanIsSeeRankingInfo == FALSE)
	{
		for (int i = 0; i < 50; i++)
		{
			if (this->m_CCFRanking[i].byRank > 0)
			{
				memcpy(pRankInfo.szCharName, this->m_CCFRanking[i].szCharName, MAX_ACCOUNT_LEN + 1);
				pRankInfo.byRank = this->m_CCFRanking[i].byRank;
				pRankInfo.nPoint = this->m_CCFRanking[i].nPoint;
				memcpy(&sendbuf[lOfs], &pRankInfo, sizeof(pRankInfo));
				lOfs += sizeof(pRankInfo);
				nUserCnt++;
			}
		}
	}

	if (nUserCnt || CanIsSeeRankingInfo == -1)
	{
		if (CanIsSeeRankingInfo == -1)
		{
			pMsg.btResult = 2;
			sLog->outBasic("[Chaos Castle Survival][Send_RangkingInfo][ERROR] CANNOT SEE RANKING==> CanIseeRankInfo:%d, nUserCnt:%d ", -1, nUserCnt);
		}

		else
		{
			pMsg.btResult = 0;
			sLog->outBasic("[Chaos Castle Survival][Send_RangkingInfo] RankingInfo ==> CanIseeRankInfo:%d, nUserCnt:%d ", CanIsSeeRankingInfo, nUserCnt);
		}
	}

	else
	{
		pMsg.btResult = 1;
		sLog->outBasic("[Chaos Castle Survival][Send_RangkingInfo][ERROR] No RankingInfo ==> CanIseeRankInfo:%d, nUserCnt:%d ", CanIsSeeRankingInfo, nUserCnt);
	}

	if (this->Check_CCF_DayType() != 3 || this->m_stCCFData.m_bCCF_PLAY_START != true)
	{
		if (this->m_bFinalDayEnd == true)
		{
			pMsg.btResult = 2;
			sLog->outBasic("[Chaos Castle Survival][Send_RangkingInfo][ERROR] FINAL WAR END");
		}
	}
	else
	{
		pMsg.btResult = 2;
		sLog->outBasic("[Chaos Castle Survival][Send_RangkingInfo][ERROR] NOW FINAL FIGHT");
	}

	pMsg.btCnt = nUserCnt;
	pMsg.h.sizeH = HIBYTE(lOfs);
	pMsg.h.sizeL = LOBYTE(lOfs);
	memcpy(&sendbuf, &pMsg, sizeof(pMsg));
	IOCP.DataSend(Obj.m_Index, (BYTE*)sendbuf, lOfs);
}

void ChaosCastleFinal::GiveCCFWinnerItem(int iWinnerIndex, BYTE byCCFType, BYTE byRewardType)
{
	if (!gObjIsConnected(iWinnerIndex))
	{
		return;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		this->GDReqSetCCFReward_UBF(iWinnerIndex, byCCFType, 1);
		return;
	}

	_stGremoryCaseItem stItem;
	WORD wItemType = -1;

	if (byCCFType == 1) // Tryout
	{
		if (byRewardType == 0) // Monster Drop
		{
			wItemType = ITEMGET(14,254);
		}

		else if (byRewardType == 1) // Winner Prize
		{
			wItemType = ITEMGET(14,256);
		}
	}

	else if (byCCFType == 2) // Semi-Final
	{
		if (byRewardType == 0) // Monster Drop
		{
			wItemType = ITEMGET(14,256);
		}

		else if (byRewardType == 1) // Winner Prize
		{
			wItemType = ITEMGET(14,257);
		}
	}

	else if (byCCFType == 3) // Final
	{
		if (byRewardType == 0) // Monster Drop
		{
			wItemType = ITEMGET(14,257);
		}

		else if (byRewardType == 1) // Winner Prize
		{
			wItemType = ITEMGET(13,276);
		}
	}

	stItem.btStorageType = GC_STORAGE_CHARACTER;
	stItem.btRewardSource = GC_REWARD_CHAOS_CASTLE_SURVIVAL;
	stItem.wItemID = wItemType;

	g_GremoryCase.GDReqAddItemToGremoryCase(iWinnerIndex, stItem, 30);
}

void ChaosCastleFinal::SendNoticeMessage(char* lpszMSG)
{
	PMSG_NOTICE pNotice;
	TNotice::MakeNoticeMsgEx(&pNotice, 0, lpszMSG);

	for (int i = 0; i < 70; i++)
	{
		if (this->m_stCCFData.m_UserData[i].m_nIndex != -1)
		{
			if (getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->Connected > PLAYER_LOGGED)
			{
				IOCP.DataSend(this->m_stCCFData.m_UserData[i].m_nIndex, (BYTE*)&pNotice, pNotice.h.size);
			}
		}
	}
}

void ChaosCastleFinal::GD_SendCCFInfoAllServer(int nMinute, int nType)
{
	PMSG_SEND_CCF_INFO_ALL_SVR pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xF9;
	pMsg.h.subcode = 0xA5;
	pMsg.nMin = nMinute;
	pMsg.nType = nType;
	pMsg.wMapSvrNum = g_MapServerManager.GetMapSvrGroup();

	wsDataCli.DataSend((char*)&pMsg, sizeof(pMsg));
	sLog->outBasic("[CCS][GD_SendCCFInfoAllServer] Send Notify To DATA svr TYPE:%d, SVR_GRP %d", nType, pMsg.wMapSvrNum);
}

void ChaosCastleFinal::SendAllMemberOfCCF(int index)
{
	if (ObjectMaxRange(index) == false)
	{
		return;
	}

	if (gObjIsConnected(index) == false)
	{
		return;
	}

	for (int i = 0; i < 70; i++)
	{
		if (this->m_stCCFData.m_UserData[i].m_nIndex != -1 && getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex)->Connected > PLAYER_LOGGED)
		{
			CGameObject lpObj = &getGameObject(this->m_stCCFData.m_UserData[i]->m_nIndex);

			if (lpObj)
			{
				char szMsg[128];
				wsprintf(szMsg, "[CCF Entry]-> No.%d, Name:%s, Level:%d, MasterLevel:%d, Class%d", i, Obj.Name, Obj.Level, Obj.m_PlayerData->MasterLevel, Obj.Class);
				gGameProtocol.GCServerMsgStringSend(szMsg, index, 1);
			}
		}
	}
}

void ChaosCastleFinal::MakeChaosCastleRewardWing(CGameObject &Obj)
{
	BYTE btNewOption;
	BYTE btNewExcOption[5];

	if (ItemGetNumberMake(this->m_stCCFFinalReward.m_btCat, this->m_stCCFFinalReward.m_wIndex) == -1)
	{
		return;
	}

	ITEM_ATTRIBUTE * lpItemInfo = &ItemAttribute[ITEMGET(this->m_stCCFFinalReward.m_btCat, this->m_stCCFFinalReward.m_wIndex)];

	if (this->m_stCCFFinalReward.m_btNewOption[0] == (BYTE)-2)
	{
		if (lpItemInfo->ItemKindA == ITEM_KIND_A_WING)
		{
			btNewOption = g_ItemOptionTypeMng.WingExcOptionRand(lpItemInfo->ItemKindA, lpItemInfo->ItemKindB, btNewExcOption);
		}

		else
		{
			btNewOption = g_ItemOptionTypeMng.CommonExcOptionRand(lpItemInfo->ItemKindA, btNewExcOption);
		}
	}

	else if (this->m_stCCFFinalReward.m_btNewOption[0] == (BYTE)-3)
	{
		if (lpItemInfo->ItemKindA == ITEM_KIND_A_WING)
		{
			btNewOption = g_ItemOptionTypeMng.WingExcOptionGetCount(lpItemInfo->ItemKindA, lpItemInfo->ItemKindB, btNewExcOption, this->m_stCCFFinalReward.m_btNewOption[1]);
		}

		else
		{
			btNewOption = g_ItemOptionTypeMng.CommonExcOptionGetCount(lpItemInfo->ItemKindA, btNewExcOption, this->m_stCCFFinalReward.m_btNewOption[1]);
		}
	}

	else
	{
		for (int k = 0; k < 9; k++)
		{
			if (this->m_stCCFFinalReward.m_btNewOption[k] == -1)
			{
				continue;
			}

			if (this->m_stCCFFinalReward.m_btNewOption[k] >= 6)
			{
				bool bOptionFound = false;

				for (int j = 0; j < 5; j++)
				{
					if (btNewExcOption[j] == this->m_stCCFFinalReward.m_btNewOption[k])
					{
						bOptionFound = true;
						break;
					}
				}

				if (bOptionFound == true)
				{
					continue;
				}

				for (int j = 0; j < 5; j++)
				{
					if (btNewExcOption[j] == 0xFF)
					{
						btNewExcOption[j] = this->m_stCCFFinalReward.m_btNewOption[k];
						break;
					}
				}
			}

			else
			{
				btNewOption |= (1 << (5 - this->m_stCCFFinalReward.m_btNewOption[k]));
			}
		}
	}

	ItemCreate(iUserIndex, getGameObject(iUserIndex)->MapNumber, getGameObject(iUserIndex)->X, getGameObject(iUserIndex)->Y, ITEMGET(this->m_stCCFFinalReward.m_btCat, this->m_stCCFFinalReward.m_wIndex), this->m_stCCFFinalReward.m_btLevel, this->m_stCCFFinalReward.m_btDurability, this->m_stCCFFinalReward.m_btSkill, this->m_stCCFFinalReward.m_btLuck, this->m_stCCFFinalReward.m_btOption, iUserIndex, btNewOption, 0, this->m_stCCFFinalReward.m_dwExpiryTime, btNewExcOption, 0);
}

bool ChaosCastleFinal::IsRealFinalDayEnd()
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	for (std::vector<CCF_START_TIME>::iterator it = this->m_vtCCFOpenTime.begin(); it != this->m_vtCCFOpenTime.end(); it++)
	{
		if (it->m_nFinal == 3 && sysTime.wMinute + 60 * sysTime.wHour > it->m_nMinute + 60 * it->m_nHour)
		{
			sLog->outBasic("[Chaos Castle Survival][RealFinalDayEnd] SystemTime %dh:%dm,FinalTime %dh:%dm",
				sysTime.wHour, sysTime.wMinute, it->m_nHour, it->m_nMinute);

			return true;
		}
	}

	return false;
}

void ChaosCastleFinal::GDReqSetCCFReward_UBF(CGameObject &Obj, BYTE btCCFType, BYTE btRewardType)
{
	if (!ObjectMaxRange(iUserIndex))
	{
		return;
	}

	CGameObject lpObj = &getGameObject(iUserIndex);

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	sLog->outBasic("[UBF][ChaosCastleFinal][GDReqSetCCFWinerInformation]ACC:%s , NAME :%s,Real NAME :%s, CCFType:%d, RewardType:%d",
		Obj.AccountID, Obj.Name, Obj.m_PlayerData->m_RealNameOfUBF, btCCFType, btRewardType);

	SDHP_REQ_SET_CCF_WINNER_INFO pMsg;
	
	memcpy(pMsg.UBFName, Obj.Name, MAX_ACCOUNT_LEN+1);
	pMsg.btCCFType = btCCFType;
	pMsg.btRewardType = btRewardType;

	PHeadSubSetB((BYTE*)&pMsg, 0xF9, 0xB0, sizeof(pMsg));
	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
}

void ChaosCastleFinal::SetOpenTodaySchedule()
{
	BYTE byToday = this->Check_CCF_DayType();
	BYTE byCount = 0;

	this->m_vtCCFOpenTime.clear();

	if (byToday != 1 && byToday != 2 && byToday != 3)
	{
		sLog->outBasic("[ChaosCastleFinal][SetOpenTodaySchedule] No related ChaosCastleFinal Day, Count:[%d]", byCount);
		return;
	}

	for (std::vector<CCF_START_TIME>::iterator It = this->m_vtCCFTime.begin(); It != this->m_vtCCFTime.end(); It++)
	{
		CCF_START_TIME & pRET = *It;

		if (pRET.m_nTryout != byToday && pRET.m_nSemiFinal != byToday && pRET.m_nFinal != byToday)
		{
			sLog->outBasic("[ChaosCastleFinal][SetOpenTodaySchedule] NO DayType:[%d], Hour:[%d],minute[%d],Count:[%d] ",
				byToday, pRET.m_nHour, pRET.m_nMinute, byCount);
		}

		else
		{
			this->m_vtCCFOpenTime.push_back(pRET);

			sLog->outBasic("[ChaosCastleFinal][SetOpenTodaySchedule] DayType:[%d], Hour:[%d],minute[%d],Count:[%d] ",
				byToday, pRET.m_nHour, pRET.m_nMinute, byCount);
			byCount++;
		}
	}
}

void ChaosCastleFinal::SetUBFGetReward(CGameObject &Obj, WORD wItemCode, UINT64 ItemSerial, BYTE btItemPos)
{
	if (!ObjectMaxRange(iUserIndex))
	{
		return;
	}

	CGameObject lpObj = &getGameObject(iUserIndex);

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (wItemCode != ITEMGET(14,254) && wItemCode != ITEMGET(14,256) && wItemCode != ITEMGET(14,257))
	{
		return;
	}

	gObjInventoryDeleteItem(iUserIndex, btItemPos);
	gGameProtocol.GCInventoryItemDeleteSend(iUserIndex, btItemPos, 0);

	this->GDReqSetCCFReward_UBF(iUserIndex, this->Check_CCF_DayType(), 0);
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

