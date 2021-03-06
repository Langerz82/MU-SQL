////////////////////////////////////////////////////////////////////////////////
// ChaosCastle.cpp
#include "StdAfx.h"
#include "ChaosCastle.h"
#include "BloodCastle.h"
#include "Main.h"
#include "IOCP.h"
#include "GameProtocol.h"
#include "Logging/Log.h"
#include "TNotice.h"
#include "CrywolfSync.h"
#include "BuffEffectSlot.h"
#include "DevilSquareGround.h"
#include "CashShop.h"
#include "configread.h"
#include "Authorization.h"
#include "util.h"
#include "MasterLevelSkillTreeSystem.h"
#include "QuestExpProgMng.h"
#include "BagManager.h"
#include "CustomMaxStats.h"

CChaosCastle g_ChaosCastle;

CChaosCastle::CChaosCastle()
{
	this->m_bCC_EVENT_ENABLE = FALSE;
	this->m_iCC_TIME_MIN_OPEN = 10;
	this->m_iCC_MIN_USER_START = 2;

	for ( int i=0;i<MAX_CHAOSCASTLE_LEVEL;i++)
	{
		InitializeCriticalSection(&this->m_stChaosCastleData[i].m_TrapCriti);
		this->m_stChaosCastleData[i].m_iCC_STATE = CC_STATE_NONE;
		this->m_stChaosCastleData[i].m_iMapNumber = this->GetChaosCastleMapNumber(i);
		this->m_stChaosCastleData[i].m_iChaosCastleIndex = i;
		this->m_stChaosCastleData[i].m_iCC_REMAIN_MSEC = -1;
		this->m_stChaosCastleData[i].m_iCC_TICK_COUNT = -1;
		this->m_iCC_MOB_CNT_SCRIPT[i] = 0;
		this->ClearChaosCastleData(i);
	}

	srand(time(NULL));
}

CChaosCastle::~CChaosCastle()
{
	for ( int i=0;i<MAX_CHAOSCASTLE_LEVEL;i++)
	{
		DeleteCriticalSection(&this->m_stChaosCastleData[i].m_TrapCriti);
	}
}


void CChaosCastle::Run()
{
	if ( this->m_bCC_EVENT_ENABLE != FALSE )
	{
		//VM_TIGER_LONDON_START <-- WTF

		if ( g_AuthSys.IsLongDisconnect() == true )
		{
			return;
		}

		//VM_TIGER_LONDON_END

		for ( int i=0;i<MAX_CHAOSCASTLE_LEVEL;i++)
		{
			switch ( this->m_stChaosCastleData[i].m_iCC_STATE )
			{
				case CC_STATE_NONE:
					this->ProcState_None(i);
					break;

				case CC_STATE_CLOSED:
					this->ProcState_Closed(i);
					break;

				case CC_STATE_PLAYING:
					this->ProcState_Playing(i);
					break;

				case CC_STATE_PLAYEND:
					this->ProcState_PlayEnd(i);
					break;
			}
		}
	}
}

void CChaosCastle::Init()
{
	if (g_ConfigRead.server.GetStateFromEventTable(g_ConfigRead.server.GetServerType(), EV_TABLE_CC) == false)
	{
		return;
	}

	for ( int i=0;i<MAX_CHAOSCASTLE_LEVEL;i++)
	{
		if ( this->IsEventEnabled() == FALSE )
		{
			this->SetState(i, CC_STATE_NONE);
		}

		else
		{
			this->SetState(i, CC_STATE_CLOSED);
		}
	}

	for ( int iTRAP_STEP = 0; iTRAP_STEP<MAX_CC_TRAP_STEP;iTRAP_STEP++)
	{
		g_rtPOINT_FRAME[0].left = (::g_iChaosCastle_GroundAxis[2] + ::g_iChaosCastle_GroundAxis[0]) / 2 + 1;
		g_rtPOINT_FRAME[0].top = g_iChaosCastle_GroundAxis[1];
		g_rtPOINT_FRAME[0].right = ::g_iChaosCastle_GroundAxis[2];
		g_rtPOINT_FRAME[0].bottom = (::g_iChaosCastle_GroundAxis[3] + ::g_iChaosCastle_GroundAxis[1] ) / 2 ;

		g_rtPOINT_FRAME[1].left = (::g_iChaosCastle_GroundAxis[2] + ::g_iChaosCastle_GroundAxis[0]) / 2 + 1;
		g_rtPOINT_FRAME[1].top = (::g_iChaosCastle_GroundAxis[3] + ::g_iChaosCastle_GroundAxis[1] ) / 2 + 1;
		g_rtPOINT_FRAME[1].right = g_iChaosCastle_GroundAxis[2];
		g_rtPOINT_FRAME[1].bottom = ::g_iChaosCastle_GroundAxis[3];
		
		g_rtPOINT_FRAME[2].left = ::g_iChaosCastle_GroundAxis[0];
		g_rtPOINT_FRAME[2].top = (::g_iChaosCastle_GroundAxis[3] + ::g_iChaosCastle_GroundAxis[1] ) / 2 + 1;
		g_rtPOINT_FRAME[2].right = (::g_iChaosCastle_GroundAxis[2] + ::g_iChaosCastle_GroundAxis[0] ) / 2 ;
		g_rtPOINT_FRAME[2].bottom = ::g_iChaosCastle_GroundAxis[3];

		g_rtPOINT_FRAME[3].left = ::g_iChaosCastle_GroundAxis[0];
		g_rtPOINT_FRAME[3].top = ::g_iChaosCastle_GroundAxis[1];
		g_rtPOINT_FRAME[3].right = (::g_iChaosCastle_GroundAxis[2] + ::g_iChaosCastle_GroundAxis[0] ) / 2;
		g_rtPOINT_FRAME[3].bottom = (::g_iChaosCastle_GroundAxis[3] + ::g_iChaosCastle_GroundAxis[1] ) / 2;



		g_rtPOINT_TRAP[0].left = (::g_iChaosCastle_DamageAxis[iTRAP_STEP][2] + ::g_iChaosCastle_DamageAxis[iTRAP_STEP][0]) / 2 + 1;
		g_rtPOINT_TRAP[0].top = ::g_iChaosCastle_DamageAxis[iTRAP_STEP][1];
		g_rtPOINT_TRAP[0].right = ::g_iChaosCastle_DamageAxis[iTRAP_STEP][2];
		g_rtPOINT_TRAP[0].bottom = (::g_iChaosCastle_DamageAxis[iTRAP_STEP][3] + ::g_iChaosCastle_DamageAxis[iTRAP_STEP][1]) / 2;

		g_rtPOINT_TRAP[1].left = (::g_iChaosCastle_DamageAxis[iTRAP_STEP][2] + ::g_iChaosCastle_DamageAxis[iTRAP_STEP][0]) / 2 + 1;
		g_rtPOINT_TRAP[1].top = (::g_iChaosCastle_DamageAxis[iTRAP_STEP][3] + ::g_iChaosCastle_DamageAxis[iTRAP_STEP][1] ) / 2 + 1;
		g_rtPOINT_TRAP[1].right = g_iChaosCastle_DamageAxis[iTRAP_STEP][2];
		g_rtPOINT_TRAP[1].bottom = ::g_iChaosCastle_DamageAxis[iTRAP_STEP][3];

		g_rtPOINT_TRAP[2].left = ::g_iChaosCastle_DamageAxis[iTRAP_STEP][0];
		g_rtPOINT_TRAP[2].top = (::g_iChaosCastle_DamageAxis[iTRAP_STEP][3] + ::g_iChaosCastle_DamageAxis[iTRAP_STEP][1] ) / 2 + 1;
		g_rtPOINT_TRAP[2].right = (::g_iChaosCastle_DamageAxis[iTRAP_STEP][2] + ::g_iChaosCastle_DamageAxis[iTRAP_STEP][0] ) / 2;
		g_rtPOINT_TRAP[2].bottom = ::g_iChaosCastle_DamageAxis[iTRAP_STEP][3];

		g_rtPOINT_TRAP[3].left = ::g_iChaosCastle_DamageAxis[iTRAP_STEP][0];
		g_rtPOINT_TRAP[3].top = ::g_iChaosCastle_DamageAxis[iTRAP_STEP][1];
		g_rtPOINT_TRAP[3].right = (::g_iChaosCastle_DamageAxis[iTRAP_STEP][2] + ::g_iChaosCastle_DamageAxis[iTRAP_STEP][0] ) / 2;
		g_rtPOINT_TRAP[3].bottom = (::g_iChaosCastle_DamageAxis[iTRAP_STEP][3] + ::g_iChaosCastle_DamageAxis[iTRAP_STEP][1] ) / 2;
	}
}


void CChaosCastle::Load(LPSTR filename)
{
	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(filename);

	if ( res.status != pugi::status_ok )
	{
		sLog->outError("[Chaos Castle] Info file Load Fail [%s] [%s]", filename, res.description());
		return;
	}

	int Token;
	int type = -1;
	int iChaosCastleCount = -1;

	this->m_vtChaosCastleOpenTime.erase( this->m_vtChaosCastleOpenTime.begin(), this->m_vtChaosCastleOpenTime.end());

	pugi::xml_node mainXML = file.child("ChaosCastle");

	this->m_bCC_EVENT_ENABLE = g_ConfigRead.server.GetStateFromEventTable(g_ConfigRead.server.GetServerType(), EV_TABLE_CC) == true ? mainXML.attribute("Enable").as_int() : false;
	this->m_iCC_MIN_USER_START = mainXML.attribute("MinPlayer").as_int();
	this->m_iCC_MAX_ENTER_COUNT = mainXML.attribute("MaxEnterCount").as_int(4);

	pugi::xml_node time = mainXML.child("Time");

	this->m_iCC_TIME_MIN_OPEN = time.attribute("StartIn").as_int();
	this->m_iCC_TIME_MIN_PLAY = time.attribute("Duration").as_int();
	this->m_iCC_TIME_MIN_REST = time.attribute("TownSpawn").as_int();

	pugi::xml_node schedule = mainXML.child("EventSchedule");

	for (pugi::xml_node start = schedule.child("Start"); start; start = start.next_sibling())
	{			
		CHAOSCASTLE_START_TIME Schedule;

		Schedule.m_iHour = start.attribute("Hour").as_int();
		Schedule.m_iMinute = start.attribute("Minute").as_int();

		this->m_vtChaosCastleOpenTime.push_back(Schedule);
	}

	pugi::xml_node expmultiplier = mainXML.child("RewardExpSettings");

	for (pugi::xml_node castle = expmultiplier.child("Castle"); castle; castle = castle.next_sibling())
	{
		int CastleNum = castle.attribute("Level").as_int();

		if(CC_FLOOR_RANGE(CastleNum))
		{
			this->m_stChaosCastleData[CastleNum].m_iCastleExperience = castle.attribute("Multiplier").as_float();
		}
	}

}

void CChaosCastle::LoadMonster(char* filename)
{
	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(filename);

	if (res.status != pugi::status_ok)
	{
		sLog->outError("[ChaosCastle] SetBase File Load Fail %s (%s)", filename, res.description());
		return;
	}

	pugi::xml_node mainXML_section = file.child("ChaosCastle");
	int nCount = 0;

	for (pugi::xml_node castle = main_section.child("Castle"); castle; castle = castle.next_sibling())
	{
		int iCastleLevel = castle.attribute("Level").as_int();
		nCount = 0;

		if (CC_FLOOR_RANGE(iCastleLevel) == FALSE)
		{
			continue;
		}

		for (pugi::xml_node monster = castle.child("Monster"); monster; monster = monster.next_sibling())
		{
			this->m_CCMP[iCastleLevel][nCount].m_Type = monster.attribute("Index").as_int();
			this->m_CCMP[iCastleLevel][nCount].m_MapNumber = monster.attribute("MapNumber").as_int();
			this->m_CCMP[iCastleLevel][nCount].m_Dis = monster.attribute("Distance").as_int();
			this->m_CCMP[iCastleLevel][nCount].m_X = monster.attribute("StartX").as_int();
			this->m_CCMP[iCastleLevel][nCount].m_Y = monster.attribute("StartY").as_int();
			this->m_CCMP[iCastleLevel][nCount].m_Dir = monster.attribute("Dir").as_int();
			nCount++;
		}

		if (nCount == 100 || nCount < 0)
		{
			sLog->outBasic("[ChaosCastle][LoadMonster] Monster Count Invalid");
			nCount = 100;
		}

		this->m_iCC_MOB_CNT_SCRIPT[iCastleLevel] = nCount;
	}

}

void CChaosCastle::CheckSync(int iChaosCastleIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return;
	}

	if ( this->m_vtChaosCastleOpenTime.empty() != false )
	{
		//sLog->outError(  "[Chaos Castle] (%d) No Schedule Time Data - Chaos Castle Terminated (m_vtChaosCastleOpenTime.empty())", iChaosCastleIndex);
		this->SetState(iChaosCastleIndex, CC_STATE_NONE);
		
		return;
	}

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	int iMIN_HOUR = 24;
	int iMIN_MINUTE = 60;
	BOOL bTIME_CHANGED = FALSE;
	std::vector<CHAOSCASTLE_START_TIME>::iterator it;

	for (it = this->m_vtChaosCastleOpenTime.begin() ; it != this->m_vtChaosCastleOpenTime.end() ; it++ ) // like this? YES
	{
		CHAOSCASTLE_START_TIME & stCCTime = *it;

		if ( (sysTime.wHour * 60 + sysTime.wMinute) < (stCCTime.m_iHour * 60 + stCCTime.m_iMinute) )
		{
			if ( ( iMIN_HOUR * 60 + iMIN_MINUTE ) > ( stCCTime.m_iHour * 60 + stCCTime.m_iMinute ) )
			{
				bTIME_CHANGED = TRUE;
				iMIN_HOUR = stCCTime.m_iHour;
				iMIN_MINUTE = stCCTime.m_iMinute;
			}
		}
	}

	if ( bTIME_CHANGED == FALSE )
	{
		it = this->m_vtChaosCastleOpenTime.begin();
		iMIN_HOUR = 24;
		iMIN_MINUTE = 60;

		for ( ; it != this->m_vtChaosCastleOpenTime.end() ; it++ )
		{
			CHAOSCASTLE_START_TIME & stCCTime = *it;

			if ( ( iMIN_HOUR * 60 + iMIN_MINUTE ) > ( stCCTime.m_iHour * 60 + stCCTime.m_iMinute ) )
			{
				bTIME_CHANGED = 2;
				iMIN_HOUR = stCCTime.m_iHour;
				iMIN_MINUTE = stCCTime.m_iMinute;
			}
		}
	}

	switch ( bTIME_CHANGED )
	{
		case TRUE:
			this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_REMAIN_MSEC = (((iMIN_HOUR * 60 * 60) + (iMIN_MINUTE * 60)) - ((sysTime.wHour * 60 * 60) + (sysTime.wMinute * 60) + (sysTime.wSecond))) * 1000;
			break;

		case 2:
			this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_REMAIN_MSEC = ((((iMIN_HOUR+24) * 60 * 60) + (iMIN_MINUTE * 60)) - ((sysTime.wHour * 60 * 60) + (sysTime.wMinute * 60) + (sysTime.wSecond))) * 1000;
			break;

		default:
			//sLog->outError(  "[Chaos Castle] (%d) No Schedule Time Data - Chaos Castle Terminated (bTIME_CHANGED=%d)",	iChaosCastleIndex + 1, bTIME_CHANGED);
			this->SetState(iChaosCastleIndex, CC_STATE_NONE);
			return;

	}

	this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_TICK_COUNT = GetTickCount();

	//sLog->outBasic("[Chaos Castle] (%d) Sync Start Time. [%d] min remain (START HOUR:%d, MIN:%d)",iChaosCastleIndex+1, this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_REMAIN_MSEC / 60000,
	//	iMIN_HOUR, iMIN_MINUTE);

}



void CChaosCastle::ProcState_None(int iChaosCastleIndex)
{
	return;
}



void CChaosCastle::ProcState_Closed(int iChaosCastleIndex)
{
	int iTICK_MSEC = GetTickCount() - this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_TICK_COUNT;

	if ( iTICK_MSEC >= 1000 )
	{
		this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_REMAIN_MSEC -= iTICK_MSEC;
		this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_TICK_COUNT = GetTickCount();

		if ( this->IsEventEnabled() != FALSE )
		{
			if ( this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_REMAIN_MSEC <= ( this->m_iCC_TIME_MIN_OPEN * 60 * 1000 ) && this->m_stChaosCastleData[iChaosCastleIndex].m_bCC_CAN_ENTER == false)
			{
				this->m_stChaosCastleData[iChaosCastleIndex].m_bCC_CAN_ENTER = true;
				this->m_stChaosCastleData[iChaosCastleIndex].m_bCC_CAN_PARTY = true;
			}

			GSProtocol.CGEventEntryNotice(EVENT_NOTIFICATION_CHAOS_CASTLE, this->m_stChaosCastleData[iChaosCastleIndex].m_bCC_CAN_ENTER ? 1 : 0);

			if ( this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_REMAIN_MSEC <= ( this->m_iCC_TIME_MIN_OPEN * 60 * 1000 ) && this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_REMAIN_MSEC > 0 && (this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_REMAIN_MSEC/60000) !=this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_NOTIFY_COUNT)
			{
				this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_NOTIFY_COUNT = this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_REMAIN_MSEC / 60000;

				if ( iChaosCastleIndex == 0 )
				{
					PMSG_NOTICE pNotice;

					TNotice::MakeNoticeMsgEx(&pNotice, 0, Lang.GetText(0,126), this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_NOTIFY_COUNT+1);
					this->SendAllUserAnyMsg( (BYTE*)&pNotice, pNotice.h.size);

				}
			}

			if ( this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_REMAIN_MSEC <= 30000 && this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_REMAIN_MSEC > 0 && this->m_stChaosCastleData[iChaosCastleIndex].m_bCC_MSG_BEFORE_ENTER == false )
			{
				this->m_stChaosCastleData[iChaosCastleIndex].m_bCC_MSG_BEFORE_ENTER = true;

				if ( iChaosCastleIndex == 0 )
				{
					PMSG_SET_DEVILSQUARE pMsg;

					PHeadSetB((BYTE*)&pMsg, 0x92, sizeof(pMsg));
					pMsg.Type = 10;

					for (int i= g_ConfigRead.server.GetObjectStartUserIndex();i<g_ConfigRead.server.GetObjectMax();i++)
					{
						if ( getGameObject(i)->Connected == PLAYER_PLAYING && getGameObject(i)->Type == OBJ_USER)
						{
							if ( CC_MAP_RANGE(getGameObject(i)->MapNumber) == FALSE )
							{
								if ( BC_MAP_RANGE(getGameObject(i)->MapNumber) == FALSE )
								{
									if ( IT_MAP_RANGE(getGameObject(i)->MapNumber) == FALSE )
									{
										if ( DG_MAP_RANGE(getGameObject(i)->MapNumber) == FALSE )
										{
											if ( IMPERIAL_MAP_RANGE(getGameObject(i)->MapNumber) == FALSE )
											{
												IOCP.DataSend(i, (UCHAR*)&pMsg, pMsg.h.size);
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if ( this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_REMAIN_MSEC <= 0 )
	{
		if ( this->IsEventEnabled() != FALSE )
		{
			this->SetState(iChaosCastleIndex, CC_STATE_PLAYING);
		}
		else
		{
			this->SetState(iChaosCastleIndex, CC_STATE_CLOSED);
		}
	}
}



void CChaosCastle::ProcState_Playing(int iChaosCastleIndex)
{
	int iTICK_MSEC = GetTickCount() - this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_TICK_COUNT;

	if ( iTICK_MSEC >= 1000 )
	{
		this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_REMAIN_MSEC -= iTICK_MSEC;
		this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_TICK_COUNT = GetTickCount();

		if ( this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_REMAIN_MSEC <= ((this->m_iCC_TIME_MIN_PLAY*60-30)*1000) &&
			 this->m_stChaosCastleData[iChaosCastleIndex].m_bCC_MSG_BEFORE_PLAY == false )
		{
			this->m_stChaosCastleData[iChaosCastleIndex].m_bCC_MSG_BEFORE_PLAY = true;
			
			PMSG_SET_DEVILSQUARE pMsg;
			PHeadSetB((BYTE*)&pMsg, 0x92, sizeof(pMsg));
			pMsg.Type = 11;
			this->SendChaosCastleAnyMsg((BYTE*)&pMsg, sizeof(pMsg), iChaosCastleIndex);
		}

		if ( this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_REMAIN_MSEC <= 30000 &&
			 this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_REMAIN_MSEC > 0  &&
			 this->m_stChaosCastleData[iChaosCastleIndex].m_bCC_MSG_BEFORE_END == false )
		{
			this->m_stChaosCastleData[iChaosCastleIndex].m_bCC_MSG_BEFORE_END = true;
			PMSG_SET_DEVILSQUARE pMsg;
			PHeadSetB((BYTE*)&pMsg, 0x92, sizeof(pMsg));
			pMsg.Type = 12;
			this->SendChaosCastleAnyMsg((BYTE*)&pMsg, sizeof(pMsg), iChaosCastleIndex);
		}

		// Set Play Quest
		if ( this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_REMAIN_MSEC <= ((this->m_iCC_TIME_MIN_PLAY*60-60)*1000) &&
			this->m_stChaosCastleData[iChaosCastleIndex].m_bCC_PLAY_START == false )
		{
			PMSG_NOTICE pNotice;

			this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_REMAIN_MSEC = (this->m_iCC_TIME_MIN_PLAY*60)*1000;
			this->m_stChaosCastleData[iChaosCastleIndex].m_bCC_CAN_PARTY = false;
			TNotice::MakeNoticeMsgEx((TNotice*)&pNotice, 0, Lang.GetText(0,127), iChaosCastleIndex+1);
			this->SendChaosCastleAnyMsg( (BYTE*)&pNotice, pNotice.h.size, iChaosCastleIndex);
			this->UnSafetyCastleZone(iChaosCastleIndex);
			this->SendCastleZoneSafetyInfo(iChaosCastleIndex, 0);
			this->m_stChaosCastleData[iChaosCastleIndex].m_bCC_PLAY_START = true;
			this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_FIRST_USER_COUNT = this->ReCalcUserCount(iChaosCastleIndex);
			this->m_stChaosCastleData[iChaosCastleIndex].m_lCC_CURRENT_MONSTER_COUNT = this->SetMonster(iChaosCastleIndex);
			this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_FIRST_MONSTER_COUNT = this->m_stChaosCastleData[iChaosCastleIndex].m_lCC_CURRENT_MONSTER_COUNT; 
			this->SetItemsToMonster(iChaosCastleIndex);
			this->SendNoticeState(iChaosCastleIndex, 5);
			
			//sLog->outBasic("[Chaos Castle] (%d) Chaos Castle Quest Start (UserCount:%d)",iChaosCastleIndex+1, this->GetCurPlayUser(iChaosCastleIndex));
		}

		if ( this->m_stChaosCastleData[iChaosCastleIndex].m_bCC_PLAY_START == false &&
			 this->m_stChaosCastleData[iChaosCastleIndex].m_bCC_CAN_ENTER == false )
		{
			int iCurPlayUser = this->GetCurPlayUser(iChaosCastleIndex);

			if (iCurPlayUser == 0 )
			{
				this->SetState(iChaosCastleIndex, CC_STATE_CLOSED);
				//sLog->outBasic("[Chaos Castle] (%d) Chaos Castle Quest Closed - No User",iChaosCastleIndex+1);

				return;
			}
		}

		if ( this->m_stChaosCastleData[iChaosCastleIndex].m_bCC_PLAY_START == true )
		{
			this->ProcessFallUser(iChaosCastleIndex);
			this->CheckUserInDieTile(iChaosCastleIndex);
			this->CheckMonsterInDieTile(iChaosCastleIndex);

			EnterCriticalSection(&this->m_stChaosCastleData[iChaosCastleIndex].m_TrapCriti);

			this->ProcessTrapStatus(iChaosCastleIndex);

			if ( this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_TRAP_STATUS != this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_TRAP_OLD_STATUS )
			{
				this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_TRAP_OLD_STATUS = this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_TRAP_STATUS;
				this->SetMapAttrHollow(iChaosCastleIndex, this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_TRAP_STATUS);
				this->PullObjInnerPlace(iChaosCastleIndex, this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_TRAP_STATUS);
			}

			LeaveCriticalSection(&this->m_stChaosCastleData[iChaosCastleIndex].m_TrapCriti);

			BOOL bPlayEnded = this->CheckPlayEnded(iChaosCastleIndex);

			if ( bPlayEnded == TRUE )
			{
				int iWinnerIndex = -1;
				int iMonsterCount = this->GetMonsterListCount(iChaosCastleIndex);

				if ( iMonsterCount <= 0 )
				{
					iWinnerIndex = this->GetCurrentWinUser(iChaosCastleIndex);

					if ( iWinnerIndex != -1 )
					{
						//sLog->outBasic("[Chaos Castle] (%d) [%s][%s] is Winner. [CharInfoSave : Class=%d, Level=%d, LvPoint=%d, Exp=%d, Str=%d, Dex=%d, Vit=%d, Energy=%d, LeaderShip=%d, Map=%d, Pk=%d]",
						//	iChaosCastleIndex+1, getGameObject(iWinnerIndex)->AccountID, getGameObject(iWinnerIndex)->Name,	getGameObject(iWinnerIndex)->Class, getGameObject(iWinnerIndex)->Level, getGameObject(iWinnerIndex)->m_PlayerData->LevelUpPoint,
						//	getGameObject(iWinnerIndex)->m_PlayerData->Experience, getGameObject(iWinnerIndex)->m_PlayerData->Strength,	getGameObject(iWinnerIndex)->m_PlayerData->Dexterity, getGameObject(iWinnerIndex)->m_PlayerData->Vitality, getGameObject(iWinnerIndex)->m_PlayerData->Energy,
						//	getGameObject(iWinnerIndex)->Leadership, getGameObject(iWinnerIndex)->MapNumber, getGameObject(iWinnerIndex)->m_PK_Level);

						this->SendWinMessage(iChaosCastleIndex, iWinnerIndex);
						this->GiveWinnerItem(iChaosCastleIndex, iWinnerIndex);
					}
					else
					{
						//sLog->outBasic("[Chaos Castle] (%d) Has No Winner.", iChaosCastleIndex+1);
					}
				}
				else
				{
					//sLog->outBasic("[Chaos Castle] (%d) Has No Winner : Monster Left (%d)",	iChaosCastleIndex+1, this->m_stChaosCastleData[iChaosCastleIndex].m_lCC_CURRENT_MONSTER_COUNT);
				}

				this->SendAllLoserFailMessage(iChaosCastleIndex, iWinnerIndex);
				this->SetState(iChaosCastleIndex, CC_STATE_PLAYEND);

				return;
			}
		
			if ( bPlayEnded == 2 )
			{
				int iWinnerIndex = -1;
				int iMonsterCount = this->GetMonsterListCount(iChaosCastleIndex);

				if ( iMonsterCount <= 0 )
				{
					iWinnerIndex = this->GetCurrentWinUser(iChaosCastleIndex);

					if ( iWinnerIndex != -1 )
					{
						//sLog->outBasic("[Chaos Castle] (%d) [%s][%s] is Winner. [CharInfoSave : Class=%d, Level=%d, LvPoint=%d, Exp=%d, Str=%d, Dex=%d, Vit=%d, Energy=%d, LeaderShip=%d, Map=%d, Pk=%d]",
						//	iChaosCastleIndex+1, getGameObject(iWinnerIndex)->AccountID, getGameObject(iWinnerIndex)->Name,getGameObject(iWinnerIndex)->Class, getGameObject(iWinnerIndex)->Level, getGameObject(iWinnerIndex)->m_PlayerData->LevelUpPoint,
						//	getGameObject(iWinnerIndex)->m_PlayerData->Experience, getGameObject(iWinnerIndex)->m_PlayerData->Strength,	getGameObject(iWinnerIndex)->m_PlayerData->Dexterity, getGameObject(iWinnerIndex)->m_PlayerData->Vitality, getGameObject(iWinnerIndex)->m_PlayerData->Energy,
						//	getGameObject(iWinnerIndex)->Leadership, getGameObject(iWinnerIndex)->MapNumber, getGameObject(iWinnerIndex)->m_PK_Level);

						this->SendWinMessage(iChaosCastleIndex, iWinnerIndex);
						this->GiveWinnerItem(iChaosCastleIndex, iWinnerIndex);
					}
					else
					{
						//sLog->outBasic("[Chaos Castle] (%d) Has No Winner.", iChaosCastleIndex+1);
					}
				}
				else
				{
					//sLog->outBasic("[Chaos Castle] (%d) Has No Winner : Monster Left (%d)",	iChaosCastleIndex+1, this->m_stChaosCastleData[iChaosCastleIndex].m_lCC_CURRENT_MONSTER_COUNT);
				}

				this->SendAllLoserFailMessage(iChaosCastleIndex, iWinnerIndex);
				this->SetState(iChaosCastleIndex, CC_STATE_PLAYEND);

				return;
			}
		
			if ( bPlayEnded == FALSE )
			{
				this->SendNoticeState(iChaosCastleIndex, 6);
			}
		}
	}

	if ( this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_REMAIN_MSEC <= 0 )
	{
		//sLog->outBasic("[Chaos Castle] (%d) is Over : TIME-OUT (Left User:%d, Monster:%d)",iChaosCastleIndex+1, this->GetCurPlayUser(iChaosCastleIndex), this->GetMonsterListCount(iChaosCastleIndex));

		int iWinnerIndex = -1;
		int iMonsterCount = this->GetMonsterListCount(iChaosCastleIndex);

		if ( iMonsterCount <= 0 )
		{
			iWinnerIndex = this->GetCurrentWinUser(iChaosCastleIndex);

			if ( iWinnerIndex != -1 )
			{
				//sLog->outBasic("[Chaos Castle] (%d) [%s][%s] is Winner. [CharInfoSave : Class=%d, Level=%d, LvPoint=%d, Exp=%d, Str=%d, Dex=%d, Vit=%d, Energy=%d, LeaderShip=%d, Map=%d, Pk=%d]",
				//	iChaosCastleIndex+1, getGameObject(iWinnerIndex)->AccountID, getGameObject(iWinnerIndex)->Name,	getGameObject(iWinnerIndex)->Class, getGameObject(iWinnerIndex)->Level, getGameObject(iWinnerIndex)->m_PlayerData->LevelUpPoint,
				//	getGameObject(iWinnerIndex)->m_PlayerData->Experience, getGameObject(iWinnerIndex)->m_PlayerData->Strength,	getGameObject(iWinnerIndex)->m_PlayerData->Dexterity, getGameObject(iWinnerIndex)->m_PlayerData->Vitality, getGameObject(iWinnerIndex)->m_PlayerData->Energy,
				//	getGameObject(iWinnerIndex)->Leadership, getGameObject(iWinnerIndex)->MapNumber, getGameObject(iWinnerIndex)->m_PK_Level);

				this->SendWinMessage(iChaosCastleIndex, iWinnerIndex);
				this->GiveWinnerItem(iChaosCastleIndex, iWinnerIndex);
			}
			else
			{
				//sLog->outBasic("[Chaos Castle] (%d) Has No Winner.", iChaosCastleIndex+1);
			}
		}
		else
		{
			if( this->GetCurrentState(iChaosCastleIndex) == 1) //season 2.5 add-on
			{
				for ( int i=0;i<MAX_CHAOSCASTLE_MONSTER;i++)
				{
					if(this->m_stChaosCastleData[iChaosCastleIndex].m_lCC_MONSTER_COUNT[i] == -1)
					{
						continue;
					}

					int CC_MONSTER_COUNT = this->m_stChaosCastleData[iChaosCastleIndex].m_lCC_MONSTER_COUNT[i];
					
					if ( ((CC_MONSTER_COUNT<0)?FALSE:(CC_MONSTER_COUNT>g_ConfigRead.server.GetObjectMaxMonster()-1)?FALSE:TRUE) == FALSE )
					{
						this->m_stChaosCastleData[iChaosCastleIndex].m_lCC_MONSTER_COUNT[i] = -1;
						continue;
					}

					if(getGameObject(CC_MONSTER_COUNT)->Life > 0.0f)
					{
						if(CC_MAP_RANGE(getGameObject(CC_MONSTER_COUNT)->MapNumber) != FALSE)
						{
							if(getGameObject(CC_MONSTER_COUNT)->Connected > 0)
							{
								//sLog->outBasic("[Chaos Castle][Bug Tracer] (%d) Left Monster AttrInfo %d/%d", iChaosCastleIndex+1, getGameObject(CC_MONSTER_COUNT)->X, getGameObject(CC_MONSTER_COUNT)->Y);
							}
						}
					}
				}
			}

			else
			{
				//sLog->outBasic("[Chaos Castle] (%d) Has No Winner : Monster Left (%d)",
				//	iChaosCastleIndex+1, this->m_stChaosCastleData[iChaosCastleIndex].m_lCC_CURRENT_MONSTER_COUNT);
			}
		}

		this->SendAllLoserFailMessage(iChaosCastleIndex, iWinnerIndex);
		this->SetState(iChaosCastleIndex, CC_STATE_PLAYEND);
	}
}



void CChaosCastle::PullObjInnerPlace(int iChaosCastleIndex, int iTRAP_STEP)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
		return;

	if ( CC_TRAP_STEP_RANGE(iTRAP_STEP) == FALSE )
		return;

	if ( iTRAP_STEP == 0 )
		return;

	int iUSER_AXIS = 0;
	std::vector<POINT> vtMAP_UNTRAP[MAX_CC_TRAP_STEP];

	for ( int iAXIS = 0; iAXIS < MAX_CC_TRAP_STEP ; iAXIS++)
	{
		for ( int iMAPX = g_rtPOINT_TRAP[iAXIS].left ; iMAPX <= g_rtPOINT_TRAP[iAXIS].right ; iMAPX ++ )
		{
			for ( int iMAPY = g_rtPOINT_TRAP[iAXIS].top ; iMAPY <= g_rtPOINT_TRAP[iAXIS].bottom ; iMAPY++ )
			{
				int iMapNumber = this->GetChaosCastleMapNumber(iChaosCastleIndex);

				if (MAX_MAP_RANGE(iMapNumber) == FALSE)
				{
					//sLog->outError( "[ERROR] iMapNumber is out of range %s %d", __FILE__, __LINE__);
					return;
				}

				BYTE btMapAttr = MapC[iMapNumber].GetAttr(iMAPX, iMAPY);

				if ( (btMapAttr&2) != 2 && (btMapAttr&4) != 4 && (btMapAttr&8) != 8 ) 
				{
					POINT iPT = {iMAPX, iMAPY};
					vtMAP_UNTRAP[iAXIS].push_back(iPT);
				}
			}
		}
	}

	for ( int i=0;i<MAX_CHAOSCASTLE_USER;i++)
	{
		if ( this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex != -1 )
		{
			if ( gObjIsConnected(this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex) ==TRUE && getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->MapNumber == (this->GetChaosCastleMapNumber(iChaosCastleIndex)) )
			{
				int iSX = getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->X;
				int iSY = getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->Y;

				int iMapNumber = this->GetChaosCastleMapNumber(iChaosCastleIndex);

				BYTE btMapAttr = MapC[iMapNumber].m_attrbuf[iSY * 256 + iSX] & 0x08;

				if ( btMapAttr == 8 )
				{
					int n;
					for ( n=0;n<MAX_CC_TRAP_STEP;n++)
					{
						if ( iSX >= g_rtPOINT_FRAME[n].left && iSX <= g_rtPOINT_FRAME[n].right &&
							 iSY >= g_rtPOINT_FRAME[n].top && iSY <= g_rtPOINT_FRAME[n].bottom )
						{
							iUSER_AXIS = n;
							break;
						}
					}

					if (n == MAX_CC_TRAP_STEP )
						continue;

					if ( vtMAP_UNTRAP[iUSER_AXIS].empty() == false )
					{
						POINT ptEMPTY = vtMAP_UNTRAP[iUSER_AXIS].back();
						vtMAP_UNTRAP[iUSER_AXIS].pop_back();

						if ( this->ObjSetPosition(this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex, ptEMPTY.x, ptEMPTY.y) == TRUE )
						{
							getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->m_iChaosCastleBlowTime = GetTickCount();
						}
					}
				}
			}
		}
	}

	for ( int iMON = 0; iMON < MAX_CHAOSCASTLE_MONSTER ; iMON++ )
	{
		if ( this->m_stChaosCastleData[iChaosCastleIndex].m_lCC_MONSTER_COUNT[iMON] == -1 )
			continue;

		int iMON_INDEX = this->m_stChaosCastleData[iChaosCastleIndex].m_lCC_MONSTER_COUNT[iMON];

		if ( !CHECK_LIMIT(iMON_INDEX, g_ConfigRead.server.GetObjectMaxMonster()))
		{
			this->m_stChaosCastleData[iChaosCastleIndex].m_lCC_MONSTER_COUNT[iMON] = -1;
			continue;
		}

		if ( getGameObject(iMON_INDEX)->Life > 0.0f && CC_MAP_RANGE(getGameObject(iMON_INDEX)->MapNumber) != FALSE && getGameObject(iMON_INDEX)->Connected > PLAYER_EMPTY )
		{
			int iSX = getGameObject(iMON_INDEX)->X;
			int iSY = getGameObject(iMON_INDEX)->Y;
			int iMapNumber = this->GetChaosCastleMapNumber(iChaosCastleIndex);

			BYTE btMapAttr = MapC[iMapNumber].m_attrbuf[iSY * 256 + iSX] & 0x08;

			if ( btMapAttr == 8 )
			{
				int n;
				for ( n=0;n<MAX_CC_TRAP_STEP;n++)
				{
					if ( iSX >= g_rtPOINT_FRAME[n].left && iSX <= g_rtPOINT_FRAME[n].right &&
						 iSY >= g_rtPOINT_FRAME[n].top && iSY <= g_rtPOINT_FRAME[n].bottom )
					{
						iUSER_AXIS = n;
						break;
					}
				}

				if ( n == MAX_CC_TRAP_STEP )
					continue;

				if ( vtMAP_UNTRAP[iUSER_AXIS].empty() == false )
				{
					POINT ptEMPTY = vtMAP_UNTRAP[iUSER_AXIS].back();
					vtMAP_UNTRAP[iUSER_AXIS].pop_back();

					if ( this->ObjSetPosition(iMON_INDEX, ptEMPTY.x, ptEMPTY.y) == TRUE )
					{
						getGameObject(iMON_INDEX)->m_iChaosCastleBlowTime = GetTickCount();
					}
				}
			}
		}
	}

	for ( int iMAP_ITEM =0;iMAP_ITEM<g_ConfigRead.server.GetObjectMaxItem();iMAP_ITEM++)
	{
		int iMapNumber = this->GetChaosCastleMapNumber(iChaosCastleIndex);

		if ( MapC[iMapNumber].m_cItem[iMAP_ITEM].IsItem() )
		{
			int x = MapC[this->m_stChaosCastleData[iChaosCastleIndex].m_iMapNumber].m_cItem[iMAP_ITEM].px;
			int y = MapC[this->m_stChaosCastleData[iChaosCastleIndex].m_iMapNumber].m_cItem[iMAP_ITEM].py;

			if ( x < ::g_iChaosCastle_DamageAxis[iTRAP_STEP][0] || x > ::g_iChaosCastle_DamageAxis[iTRAP_STEP][2] ||
				 y < ::g_iChaosCastle_DamageAxis[iTRAP_STEP][1] || y > ::g_iChaosCastle_DamageAxis[iTRAP_STEP][3] )
			{
				MapC[this->m_stChaosCastleData[iChaosCastleIndex].m_iMapNumber].m_cItem[iMAP_ITEM].m_State = 8;
				MapC[this->m_stChaosCastleData[iChaosCastleIndex].m_iMapNumber].m_cItem[iMAP_ITEM].Give = true;
				MapC[this->m_stChaosCastleData[iChaosCastleIndex].m_iMapNumber].m_cItem[iMAP_ITEM].live = false;
			}
		}
	}
}



void CChaosCastle::SetMapAttrHollow(int iChaosCastleIndex, int iTRAP_STEP)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return;
	}

	if ( CC_TRAP_STEP_RANGE(iTRAP_STEP) == FALSE )
	{
		return;
	}

	if ( iTRAP_STEP == 0 )
	{
		return;
	}

	for ( int a=0;a<MAX_CC_HOLLOW_ZONE;a++)
	{
		for ( int b=::g_iChaosCastle_MapHollowZone[iTRAP_STEP][a][0] ; b<=::g_iChaosCastle_MapHollowZone[iTRAP_STEP][a][2] ; b++)
		{
			for ( int c=::g_iChaosCastle_MapHollowZone[iTRAP_STEP][a][1] ; c<=::g_iChaosCastle_MapHollowZone[iTRAP_STEP][a][3] ; c++)
			{
				int iMapNumber = this->GetChaosCastleMapNumber(iChaosCastleIndex);
				MapC[iMapNumber].m_attrbuf[ c * 256 + b] |= 8;
			}
		}
	}
	
}



void CChaosCastle::SetMapAttrFill(int iChaosCastleIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return;
	}
	
	for ( int iTRAP_STEP=1;iTRAP_STEP<MAX_CC_TRAP_STEP;iTRAP_STEP++)
	{
		for ( int a=0;a<MAX_CC_HOLLOW_ZONE;a++)
		{
			for ( int b=::g_iChaosCastle_MapHollowZone[iTRAP_STEP][a][0] ; b<=::g_iChaosCastle_MapHollowZone[iTRAP_STEP][a][2] ; b++)
			{
				for ( int c=::g_iChaosCastle_MapHollowZone[iTRAP_STEP][a][1] ; c<=::g_iChaosCastle_MapHollowZone[iTRAP_STEP][a][3] ; c++)
				{
					int iMapNumber = this->GetChaosCastleMapNumber(iChaosCastleIndex);
					MapC[iMapNumber].m_attrbuf[ c * 256 + b] &= ~8;
				}
			}
		}
	}
}



void CChaosCastle::ProcState_PlayEnd(int iChaosCastleIndex)
{
	int iTICK_MSEC = GetTickCount() - this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_TICK_COUNT;

	if ( iTICK_MSEC >= 1000 )
	{
		this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_REMAIN_MSEC -= iTICK_MSEC;
		this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_TICK_COUNT = GetTickCount();
		this->CheckUserInDieTile(iChaosCastleIndex);

		if ( this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_REMAIN_MSEC <= 30000 && this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_REMAIN_MSEC > 0 && this->m_stChaosCastleData[iChaosCastleIndex].m_bCC_MSG_BEFORE_QUIT == false)
		{
			this->m_stChaosCastleData[iChaosCastleIndex].m_bCC_MSG_BEFORE_QUIT = true;

			PMSG_SET_DEVILSQUARE pMsg;

			PHeadSetB((BYTE*)&pMsg, 0x92, sizeof(pMsg));
			pMsg.Type = 13;

			this->SendChaosCastleAnyMsg((BYTE *)&pMsg, sizeof(pMsg), iChaosCastleIndex);
		}

	}

	if ( this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_REMAIN_MSEC <= 0 )
	{
		this->SetState(iChaosCastleIndex, CC_STATE_CLOSED);
	}
}



void CChaosCastle::SetState(int iChaosCastleIndex, int iCC_STATE)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return;
	}

	if ( iCC_STATE < CC_STATE_NONE || iCC_STATE > CC_STATE_PLAYEND )
	{
		return;
	}

	this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_STATE = iCC_STATE;

	switch ( this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_STATE )
	{
		case CC_STATE_NONE:
			this->SetState_None(iChaosCastleIndex);
			break;

		case CC_STATE_CLOSED:
			this->SetState_Closed(iChaosCastleIndex);
			break;

		case CC_STATE_PLAYING:
			this->SetState_Playing(iChaosCastleIndex);
			break;

		case CC_STATE_PLAYEND:
			this->SetState_PlayEnd(iChaosCastleIndex);
	}
	
}



void CChaosCastle::SetState_None(int iChaosCastleIndex)
{
	return;
}



void CChaosCastle::SetState_Closed(int iChaosCastleIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return;
	}

	this->SendNoticeState(iChaosCastleIndex, 7);
	this->ClearChaosCastleData(iChaosCastleIndex);
	this->ClearMonster(iChaosCastleIndex);

	for ( int n=g_ConfigRead.server.GetObjectStartUserIndex();n<g_ConfigRead.server.GetObjectMax();n++)
	{
		if ( getGameObject(n)->MapNumber == this->GetChaosCastleMapNumber(iChaosCastleIndex) )
		{
			if ( getGameObject(n)->Connected > PLAYER_LOGGED )
			{
				if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
				{
					gObjMoveGate(n, 333);
				}

				else
				{
					gObjMoveGate(n, 22);
				}
			}
		}
	}

	this->SafetyCastleZone(iChaosCastleIndex);
	this->SetMapAttrFill(iChaosCastleIndex);
	this->CheckSync(iChaosCastleIndex);

	//sLog->outBasic("[Chaos Castle] (%d) SetState CLOSED", iChaosCastleIndex+1);
}



void CChaosCastle::SetState_Playing(int iChaosCastleIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return;
	}

	this->m_stChaosCastleData[iChaosCastleIndex].m_bCC_CAN_ENTER = false;
	this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_REMAIN_MSEC = this->m_iCC_TIME_MIN_PLAY * 60 * 1000;
	this->SendCastleZoneSafetyInfo(iChaosCastleIndex, true);

	if ( this->CheckCanStartPlay(iChaosCastleIndex) == false )
	{
		this->PlayFailedRollBack(iChaosCastleIndex);
		//sLog->outBasic("[Chaos Castle] (%d) Failed to Start Chaos Castle : Lack of User", iChaosCastleIndex +1 );
		this->SetState_Closed(iChaosCastleIndex);
	}
	else
	{
		//sLog->outBasic("[Chaos Castle] (%d) SetState PLAYING", iChaosCastleIndex + 1 );
	}
}



void CChaosCastle::SetState_PlayEnd(int iChaosCastleIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return;
	}

	this->SendNoticeState(iChaosCastleIndex, 7);
	this->ClearMonster(iChaosCastleIndex);
	this->m_stChaosCastleData[iChaosCastleIndex].m_bCC_CAN_ENTER = false;
	this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_REMAIN_MSEC = this->m_iCC_TIME_MIN_REST * 60 * 1000;

	//sLog->outBasic("[Chaos Castle] (%d) SetState PLAYEND", iChaosCastleIndex+1);

}



void CChaosCastle::ClearChaosCastleData(int iChaosCastleIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return;
	}

	this->m_stChaosCastleData[iChaosCastleIndex].Clear();

	for ( int i=0;i<MAX_CHAOSCASTLE_USER;i++ )
	{
		if ( this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex != -1 )
		{
			getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->m_cChaosCastleIndex = -1;
			getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->m_cChaosCastleSubIndex = -1;
			getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->m_iChaosCastleBlowTime = 0;
			getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->m_cKillUserCount = 0;
			getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->m_cKillMonsterCount = 0;
		}

		this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].Clear();
	}
}



void CChaosCastle::ClearMonster(int iChaosCastleIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return;
	}

	for ( int n=0;n<g_ConfigRead.server.GetObjectMaxMonster();n++)
	{
		if ( getGameObject(n)->MapNumber == this->GetChaosCastleMapNumber(iChaosCastleIndex))
		{
			gObjDel(n);
		}
	}
}



int  CChaosCastle::SetMonster(int iChaosCastleIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return -1;
	}

	int iNOW_ADDED_MONSTER_COUNT = 0;
	int iMAX_ADDABLE_MONSTER_COUNT = MAX_CC_MONSTERS - this->GetCurPlayUser(iChaosCastleIndex);
	int result;

	if ( iMAX_ADDABLE_MONSTER_COUNT < 0 )
	{
		iMAX_ADDABLE_MONSTER_COUNT = 0;
	}

	for (int n = 0; n < this->m_iCC_MOB_CNT_SCRIPT[iChaosCastleIndex] && iNOW_ADDED_MONSTER_COUNT < iMAX_ADDABLE_MONSTER_COUNT; n++)
	{
		if ( CC_MAP_RANGE(this->m_CCMP[iChaosCastleIndex][n].m_MapNumber) != FALSE )
		{
			int iMonsterIndex = this->m_CCMP[iChaosCastleIndex][n].m_Type;
			BYTE btMapNumber = this->m_CCMP[iChaosCastleIndex][n].m_MapNumber;
			BYTE btChaosCastleIndex = this->GetChaosCastleIndex(btMapNumber);

			if ( btChaosCastleIndex != iChaosCastleIndex )
			{
				continue;
			}

			result = gObjAddMonster(btMapNumber);

			if ( result >= 0 )
			{
				getGameObject(result)->m_PosNum = n;
				getGameObject(result)->X = this->m_CCMP[iChaosCastleIndex][n)->m_X;
				getGameObject(result)->Y = this->m_CCMP[iChaosCastleIndex][n)->m_Y;
				getGameObject(result)->MapNumber = this->m_CCMP[iChaosCastleIndex][n)->m_MapNumber;
				getGameObject(result)->TX = getGameObject(result)->X;
				getGameObject(result)->TY = getGameObject(result)->Y;
				getGameObject(result)->m_OldX = getGameObject(result)->X;
				getGameObject(result)->m_OldY = getGameObject(result)->Y;
				getGameObject(result)->Dir = this->m_CCMP[iChaosCastleIndex][n)->m_Dir;
				getGameObject(result)->StartX = getGameObject(result)->X;
				getGameObject(result)->StartY = getGameObject(result)->Y;
				gObjSetMonster(result, iMonsterIndex);
				getGameObject(result)->MaxRegenTime = 0;
				getGameObject(result)->m_cChaosCastleIndex = btChaosCastleIndex;
				getGameObject(result)->Dir = rand() % 8;
				iNOW_ADDED_MONSTER_COUNT++;
				this->AddMonsterList(iChaosCastleIndex, result);
			}
		}
	}

	return iNOW_ADDED_MONSTER_COUNT;
}



void CChaosCastle::AddMonsterList(int iChaosCastleIndex, int iMonsterIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return;
	}

	for ( int iMON =0;iMON<MAX_CC_MONSTERS;iMON++)
	{
		if ( this->m_stChaosCastleData[iChaosCastleIndex].m_lCC_MONSTER_COUNT[iMON] == -1 )
		{
			InterlockedExchange((LPLONG)&this->m_stChaosCastleData[iChaosCastleIndex].m_lCC_MONSTER_COUNT[iMON], iMonsterIndex);
			break;
		}
	}
}



void CChaosCastle::DelMonsterList(int iChaosCastleIndex, int iMonsterIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return;
	}

	//sLog->outBasic("[Chaos Castle][Bug Tracer] MONSTER DEL (%d) Count(%d) : ENTER - Index(%d)",
	//	iChaosCastleIndex+1, this->m_stChaosCastleData[iChaosCastleIndex].m_lCC_CURRENT_MONSTER_COUNT, iMonsterIndex);

	for ( int iMON =0;iMON<MAX_CC_MONSTERS;iMON++)
	{
		if ( this->m_stChaosCastleData[iChaosCastleIndex].m_lCC_MONSTER_COUNT[iMON] == iMonsterIndex )
		{
			InterlockedExchange(&this->m_stChaosCastleData[iChaosCastleIndex].m_lCC_MONSTER_COUNT[iMON], -1);
			break;
		}
	}

	InterlockedDecrement(&this->m_stChaosCastleData[iChaosCastleIndex].m_lCC_CURRENT_MONSTER_COUNT);

	if ( this->m_stChaosCastleData[iChaosCastleIndex].m_lCC_CURRENT_MONSTER_COUNT < 0 )
	{
		this->m_stChaosCastleData[iChaosCastleIndex].m_lCC_CURRENT_MONSTER_COUNT = 0;
	}

	//sLog->outBasic("[Chaos Castle][Bug Tracer] MONSTER DEL (%d) Count(%d) : OUT - Index(%d)",
	//	iChaosCastleIndex+1, this->m_stChaosCastleData[iChaosCastleIndex].m_lCC_CURRENT_MONSTER_COUNT, iMonsterIndex);
}



int  CChaosCastle::GetMonsterListCount(int iChaosCastleIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return -1;
	}

	int iAliveMonsterCount = 0;

	for ( int i=0;i<MAX_CHAOSCASTLE_MONSTER;i++) //loc3
	{
		if(this->m_stChaosCastleData[iChaosCastleIndex].m_lCC_MONSTER_COUNT[i] == -1)
		{
			continue;
		}

		int CC_MONSTER_COUNT = this->m_stChaosCastleData[iChaosCastleIndex].m_lCC_MONSTER_COUNT[i]; //loc4
					
		if ( ((CC_MONSTER_COUNT<0)?FALSE:(CC_MONSTER_COUNT>g_ConfigRead.server.GetObjectMaxMonster()-1)?FALSE:TRUE) == FALSE )
		{
			this->m_stChaosCastleData[iChaosCastleIndex].m_lCC_MONSTER_COUNT[i] = -1;
			continue;
		}

		if(getGameObject(CC_MONSTER_COUNT)->Life > 0.0f)
		{
			if(CC_MAP_RANGE(getGameObject(CC_MONSTER_COUNT)->MapNumber) != FALSE)
			{
				if(getGameObject(CC_MONSTER_COUNT)->Connected > 0)
				{
					iAliveMonsterCount++;
				}
			}
		}
	}
	
	return iAliveMonsterCount;
}



void CChaosCastle::SetItemsToMonster(int iChaosCastleIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return;
	}

	int iMAX_MONSTER = this->GetMonsterListCount(iChaosCastleIndex);

	if ( iMAX_MONSTER <= 0 )
	{
		return;
	}

	std::map<int, _MONSTER_ITEM_DROP>::iterator it;
	_MONSTER_ITEM_DROP MID;

	if (g_ConfigRead.server.GetServerType() != SERVER_BATTLECORE)
	{
		for ( int iA = 0;iA < MAX_CC_MONSTER_DROP_ITEM ; iA++)
		{
			for ( int iB=0; iB< ::g_iChaosCastle_MonsterItems[iChaosCastleIndex][iA][1] ; iB++)
			{
				for ( int iC=0;iC<200;iC++)	// Try 200 times
				{
					int iMONSTER_INDEX = this->m_stChaosCastleData[iChaosCastleIndex].m_lCC_MONSTER_COUNT[rand() % iMAX_MONSTER];
					it = this->m_stChaosCastleData[iChaosCastleIndex].m_mapMonsterItemList.find(iMONSTER_INDEX);

					if ( it == this->m_stChaosCastleData[iChaosCastleIndex].m_mapMonsterItemList.end() )
					{
						MID.m_iIndex = iMONSTER_INDEX;
						MID.m_iItemKind = ::g_iChaosCastle_MonsterItems[iChaosCastleIndex][iA][0];
						this->m_stChaosCastleData[iChaosCastleIndex].m_mapMonsterItemList.insert( std::pair<int,_MONSTER_ITEM_DROP>(iMONSTER_INDEX, MID) );
						break;
					}
				}
			}
		}
	}

	else
	{
		for ( int iB=0; iB < g_iChaosCastle_MonsterItems_UBF[iChaosCastleIndex][1] ; iB++)
		{
			for ( int iC=0;iC<200;iC++)	// Try 200 times
			{
				int iMONSTER_INDEX = this->m_stChaosCastleData[iChaosCastleIndex].m_lCC_MONSTER_COUNT[rand() % iMAX_MONSTER];
				it = this->m_stChaosCastleData[iChaosCastleIndex].m_mapMonsterItemList.find(iMONSTER_INDEX);

				if ( it == this->m_stChaosCastleData[iChaosCastleIndex].m_mapMonsterItemList.end() )
				{
					MID.m_iIndex = iMONSTER_INDEX;
					MID.m_iItemKind = ::g_iChaosCastle_MonsterItems_UBF[iChaosCastleIndex][0];
					this->m_stChaosCastleData[iChaosCastleIndex].m_mapMonsterItemList.insert( std::pair<int,_MONSTER_ITEM_DROP>(iMONSTER_INDEX, MID) );
					break;
				}
			}
		}
	}
}



void CChaosCastle::SearchNDropMonsterItem(int iChaosCastleIndex, int iMonsterIndex, int iMaxHitUserIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return;
	}

	if ( ObjectMaxRange(iMonsterIndex) == FALSE )
	{
		return;
	}

	if ( ObjectMaxRange(iMaxHitUserIndex) == FALSE )
	{
		return;
	}

	std::map<int, _MONSTER_ITEM_DROP>::iterator it = this->m_stChaosCastleData[iChaosCastleIndex].m_mapMonsterItemList.find(iMonsterIndex);

	if ( it == this->m_stChaosCastleData[iChaosCastleIndex].m_mapMonsterItemList.end() )
	{
		return;
	}

	_MONSTER_ITEM_DROP MID = it->second;
	int iType = MID.m_iItemKind;
	int iLevel = 0;
	
	if ( iType == ITEMGET(13,15) ) // Fruit
	{
		iLevel = rand()%4;
	}

	::ItemCreate(getGameObject(iMaxHitUserIndex)->m_Index, getGameObject(iMaxHitUserIndex)->MapNumber, getGameObject(iMaxHitUserIndex)->X,
		getGameObject(iMaxHitUserIndex)->Y, iType, iLevel, 0, 0, 0, 0, iMaxHitUserIndex, 0, 0, 0, 0, 0);

	if ( ObjectMaxRange(iMaxHitUserIndex) != FALSE )
	{
		//sLog->outBasic("[Chaos Castle] (%d) Monster Dropped ChaosCastle Item To [%s][%s] (Item:%s)",
		//	iChaosCastleIndex+1, getGameObject(iMaxHitUserIndex)->AccountID, getGameObject(iMaxHitUserIndex)->Name,	ItemAttribute[iType)->Name);
	}
}



void CChaosCastle::SendAllUserAnyMsg(BYTE* lpMsg, int iSize)
{
	for ( int i=g_ConfigRead.server.GetObjectStartUserIndex();i<g_ConfigRead.server.GetObjectMax();i++)
	{
		if ( getGameObject(i)->Connected == PLAYER_PLAYING )
		{
			if ( getGameObject(i)->Type == OBJ_USER )
			{
				if ( DG_MAP_RANGE(getGameObject(i)->MapNumber) == FALSE )
				{
					IOCP.DataSend(i, lpMsg, iSize);
				}
			}
		}
	}
}



void CChaosCastle::SendChaosCastleAnyMsg(BYTE* lpMsg, int iSize, int iChaosCastleIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return;
	}

	for ( int i=0;i<MAX_CHAOSCASTLE_USER;i++)
	{
		if ( this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex != -1 )
		{
			if ( gObjIsConnected(this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex) != FALSE )
			{
				if ( getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex]->m_cChaosCastleIndex != -1 && getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i)->m_iIndex)->m_cChaosCastleSubIndex != -1 )
				{
					IOCP.DataSend(this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex, lpMsg, iSize);
				}
			}
		}
	}
}



void CChaosCastle::SendNoticeMessage(int iChaosCastleIndex, LPSTR lpszMSG)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return;
	}

	PMSG_NOTICE pNotice;
	TNotice::MakeNoticeMsgEx((TNotice *)&pNotice, 0, lpszMSG);

	for ( int i=0;i<MAX_CHAOSCASTLE_USER;i++)
	{
		if ( this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex != -1 )
		{
			if ( getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->Connected > PLAYER_LOGGED )
			{
				if ( getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex]->m_cChaosCastleIndex != -1 && getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i)->m_iIndex)->m_cChaosCastleSubIndex != -1 )
				{
					IOCP.DataSend(this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex, (UCHAR*)&pNotice, pNotice.h.size);
				}
			}
		}
	}

}



void CChaosCastle::SendNoticeState(int iChaosCastleIndex, int iPlayState)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return;
	}

	PMSG_STATEBLOODCASTLE pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x9B, sizeof(PMSG_STATEBLOODCASTLE));
	pMsg.btPlayState = iPlayState;
	pMsg.wRemainSec = this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_REMAIN_MSEC / 1000;
	pMsg.wMaxKillMonster = this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_FIRST_USER_COUNT + this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_FIRST_MONSTER_COUNT;
	pMsg.wCurKillMonster = this->GetCurPlayUser(iChaosCastleIndex) + this->GetMonsterListCount(iChaosCastleIndex);
	pMsg.wUserHaveWeapon = -1;
	pMsg.btWeaponNum = -1;

	for ( int i=0;i<MAX_CHAOSCASTLE_USER;i++)
	{
		if ( this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex != -1 )
		{
			if ( getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->Connected > PLAYER_LOGGED )
			{
				if ( getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex]->m_cChaosCastleIndex != -1 && getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i)->m_iIndex)->m_cChaosCastleSubIndex != -1 )
				{
					IOCP.DataSend(this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex, (UCHAR*)&pMsg, pMsg.h.size);

					if (iPlayState == 7)
					{
						if (m_stChaosCastleData[iChaosCastleIndex].m_lCC_CURRENT_MONSTER_COUNT == 0)
						{
							g_QuestExpProgMng.ChkUserQuestTypeEventMap(QUESTEXP_ASK_CHAOSCASTLE_CLEAR, getGameObject(m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex), iChaosCastleIndex, 0);
						}
					}
				}
			}
		}
	}
}



void CChaosCastle::SendWinMessage(int iChaosCastleIndex, int iWinnerIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return;
	}

	if ( ObjectMaxRange(iWinnerIndex) == FALSE )
	{
		return;
	}

	int iChaosCastleSubIndex = getGameObject(iWinnerIndex)->m_cChaosCastleSubIndex;

	if ( CC_SUB_FLOOR_RANGE(iChaosCastleSubIndex) == FALSE )
	{
		return;
	}

	if ( this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[iChaosCastleSubIndex].m_bSendQuitMsg == true )
	{
		return;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		this->GD_Req_Save_KillPoint_UBF(iWinnerIndex, getGameObject(iWinnerIndex)->Name, 
			3 * getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[iChaosCastleSubIndex]->m_iIndex]->m_cKillUserCount + 2 * getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[iChaosCastleSubIndex)->m_iIndex)->m_cKillMonsterCount + 50,
			iChaosCastleIndex);
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		this->RewardUserKillPointUBF(iChaosCastleIndex, iChaosCastleSubIndex, 1);
	}

	else
	{
		this->RewardUserEXP(iChaosCastleIndex, getGameObject(iWinnerIndex)->m_cChaosCastleSubIndex, true );
	}

	g_QuestExpProgMng.ChkUserQuestTypeEventMap(QUESTEXP_ASK_CHAOSCASTLE_CLEAR, getGameObject(iWinnerIndex), iChaosCastleIndex, 0);
	GSProtocol.GCServerMsgStringSend(Lang.GetText(0,128), iWinnerIndex, 1);
	this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[iChaosCastleSubIndex].m_bSendQuitMsg = true;
}



void CChaosCastle::SendFailMessage(int iChaosCastleIndex, int iLoserIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return;
	}

	if ( ObjectMaxRange(iLoserIndex) == FALSE )
	{
		return;
	}

	int iChaosCastleSubIndex = getGameObject(iLoserIndex)->m_cChaosCastleSubIndex;

	if ( CC_SUB_FLOOR_RANGE(iChaosCastleSubIndex) == FALSE )
	{
		return;
	}

	if ( this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[iChaosCastleSubIndex].m_bSendQuitMsg == true )
	{
		return;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		this->GD_Req_Save_KillPoint_UBF(iLoserIndex, getGameObject(iLoserIndex)->Name, 
			3 * getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[iChaosCastleSubIndex]->m_iIndex]->m_cKillUserCount + 2 * getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[iChaosCastleSubIndex)->m_iIndex)->m_cKillMonsterCount + 50,
			iChaosCastleIndex);
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		this->RewardUserKillPointUBF(iChaosCastleIndex, iChaosCastleSubIndex, 0);
	}

	else
	{
		this->RewardUserEXP(iChaosCastleIndex, getGameObject(iLoserIndex)->m_cChaosCastleSubIndex, false );
	}

	this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[iChaosCastleSubIndex].m_bSendQuitMsg = true;
}



void CChaosCastle::SendAllLoserFailMessage(int iChaosCastleIndex, int iWinnerIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return;
	}

	if ( ObjectMaxRange(iWinnerIndex) == FALSE && iWinnerIndex != -1)	
	{
		return;
	}

	for ( int i=0;i<MAX_CHAOSCASTLE_USER;i++)
	{
		if ( this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex != -1 )
		{
			if ( this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex != iWinnerIndex )
			{
				if ( this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_bSendQuitMsg == false )
				{
					if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
					{
						int iChaosCastleSubIndex = getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->m_cChaosCastleSubIndex;

						this->GD_Req_Save_KillPoint_UBF(this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[iChaosCastleSubIndex].m_iIndex, getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[iChaosCastleSubIndex]->m_iIndex)->Name, 
							3 * getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[iChaosCastleSubIndex]->m_iIndex]->m_cKillUserCount + 2 * getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[iChaosCastleSubIndex)->m_iIndex)->m_cKillMonsterCount + 50,
							iChaosCastleIndex);
					}

					if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
					{
						this->RewardUserKillPointUBF(iChaosCastleIndex, getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->m_cChaosCastleSubIndex, 0);
					}

					else
					{
						this->RewardUserEXP(iChaosCastleIndex, getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->m_cChaosCastleSubIndex, false);
					}

					g_QuestExpProgMng.ChkUserQuestTypeEventMap(QUESTEXP_ASK_CHAOSCASTLE_CLEAR, getGameObject(m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex), iChaosCastleIndex, 0);
					GSProtocol.GCServerMsgStringSend(Lang.GetText(0,129), this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex, 1);
					this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_bSendQuitMsg = true;
				}
			}
		}
	}
}



int  CChaosCastle::EnterUserChaosCastle(int iChaosCastleIndex, CGameObject &Obj)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return -1;
	}

	int iRET_VAL = -1;

	for ( int i=0;i<MAX_CHAOSCASTLE_USER;i++)
	{
		if ( this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex != -1 )
		{
			if ( this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex == iUserIndex )
			{
				iRET_VAL = i;
				break;
			}
		}

		if ( this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex == -1 )
		{
			iRET_VAL = i;
			this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex = iUserIndex;
			this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iEXP = 0;
			this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iScore = 0;
			this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iUserState = 0;
			this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_bSendQuitMsg = false;
			this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_ENTERED_USER++;
	
			if ( this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_ENTERED_USER > MAX_CHAOSCASTLE_USER)
			{
				this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_ENTERED_USER = MAX_CHAOSCASTLE_USER;
			}

			break;
		}
	}

	return iRET_VAL;
}



int  CChaosCastle::LeaveUserChaosCastle(int iChaosCastleIndex, CGameObject &Obj)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return -1;
	}

	int iRET_VAL = -1;

	for ( int i=0;i<MAX_CHAOSCASTLE_USER;i++)
	{
		if ( this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex == iUserIndex )
		{
			iRET_VAL = iUserIndex;	// #error Instead of iUserIndex put i
			this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex = -1;
			this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iEXP = 0;
			this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iScore = 0;
			this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iUserState = 0;
			this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_bSendQuitMsg = false;
			this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_ENTERED_USER--;
	
			if ( this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_ENTERED_USER < 0)
			{
				this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_ENTERED_USER = 0;
			}

			break;
		}
	}

	return iRET_VAL;
}



void CChaosCastle::AddFallUser(int iChaosCastleIndex, CGameObject &Obj)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return;
	}

	for ( int iFALL=0;iFALL<MAX_CHAOSCASTLE_USER;iFALL++)
	{
		if ( this->m_stChaosCastleData[iChaosCastleIndex].m_lFallUser[iFALL] == -1 )
		{
			InterlockedExchange(&this->m_stChaosCastleData[iChaosCastleIndex].m_lFallUser[iFALL], iUserIndex);
			break;
		}
	}
}



void CChaosCastle::ProcessFallUser(int iChaosCastleIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return;
	}

	for ( int iFALL=0;iFALL<MAX_CHAOSCASTLE_USER;iFALL++)
	{
		if ( this->m_stChaosCastleData[iChaosCastleIndex].m_lFallUser[iFALL] != -1 )
		{
			int iFALL_INDEX = this->m_stChaosCastleData[iChaosCastleIndex].m_lFallUser[iFALL];
			InterlockedExchange(&this->m_stChaosCastleData[iChaosCastleIndex].m_lFallUser[iFALL], -1);

			if ( (getGameObject(iFALL_INDEX)->MapNumber) == this->GetChaosCastleMapNumber(iChaosCastleIndex) &&  gObjIsConnected(iFALL_INDEX) &&  getGameObject(iFALL_INDEX)->Life > 0 )
			{
				getGameObject(iFALL_INDEX)->Life = 0;
				getGameObject(iFALL_INDEX)->m_State = 4;
				getGameObject(iFALL_INDEX)->KillerType = 1;
				getGameObject(iFALL_INDEX)->RegenTime = GetTickCount();
				getGameObject(iFALL_INDEX)->DieRegen = 1;
				getGameObject(iFALL_INDEX)->PathCount = 0;
				
				GSProtocol.GCDiePlayerSend(getGameObject(iFALL_INDEX), getGameObject(iFALL_INDEX)->m_Index, 0, 0);
			}
		}
	}
}



void CChaosCastle::SetUserState(CGameObject &Obj, int iState)
{
	if ( !ObjectMaxRange(iUserIndex))
		return;

	int iChaosCastleIndex = getGameObject(iUserIndex)->m_cChaosCastleIndex;
	int iChaosCastleSubIndex = getGameObject(iUserIndex)->m_cChaosCastleSubIndex;

	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
		return;

	if ( CC_SUB_FLOOR_RANGE(iChaosCastleSubIndex) == FALSE )
		return;

	switch ( iState )
	{
		case 0:
			this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[iChaosCastleSubIndex].m_iUserState = 0;
			break;
		case 1:
			this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[iChaosCastleSubIndex].m_iUserState = 1;
	}
}



int  CChaosCastle::CalcSendRewardEXP(CGameObject &Obj, int iEXP, int iKILLCOUNT_USER, int iKILLCOUNT_MONSTER)
{
	if ( iEXP <= 0 )
		return 0;

	if ( g_CrywolfSync.GetOccupationState() == 1 && g_CrywolfSync.GetApplyPenalty() == TRUE )
	{
		iEXP = iEXP * g_CrywolfSync.GetGettingExpPenaltyRate() / 100;
	}

	UINT64 iRET_EXP = 0;
	UINT64 iCAL_EXP = iEXP;
	int iMAX_LEVCOUNT = 0;

	if ( !gObjIsConnected(iUserIndex))
		return 0;

	if(g_MasterLevelSkillTreeSystem.IsMasterLevelUser(getGameObject(iUserIndex)) == false)
	{
		if(iCAL_EXP > getGameObject(iUserIndex)->m_PlayerData->Experience+getGameObject(iUserIndex)->m_PlayerData->NextExp)
		{
			iCAL_EXP = getGameObject(iUserIndex)->m_PlayerData->NextExp;
		}
	}
	else
	{
		if(iCAL_EXP > getGameObject(iUserIndex)->m_PlayerData->MasterExperience+getGameObject(iUserIndex)->m_PlayerData->MasterNextExp)
		{
			iCAL_EXP = getGameObject(iUserIndex)->m_PlayerData->MasterNextExp;
		}
	}

	iRET_EXP = iCAL_EXP;

	if ( getGameObject(iUserIndex)->Type == OBJ_USER )
	{
		while ( iCAL_EXP > 0 )
		{
			if ( iCAL_EXP > 0 )
			{
				CheckItemOptForGetExpExRenewal(getGameObject(iUserIndex), 0, iCAL_EXP, 0, true);

				iCAL_EXP = this->LevelUp(iUserIndex, iCAL_EXP);
			}

			if ( iCAL_EXP == 0)
			{
				return iRET_EXP;
			}

			iMAX_LEVCOUNT++;

			if ( iMAX_LEVCOUNT > 1000 )
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



int  CChaosCastle::GetUserLevelToEnter(CGameObject &Obj)
{
	if (!gObjIsConnected(iUserIndex))
		return -1;

	int iENTER_LEVEL = -1;

	for (int i = 0; i < MAX_CHAOSCASTLE_LEVEL; i++)
	{
		if (getGameObject(iUserIndex)->Level + getGameObject(iUserIndex)->m_PlayerData->MasterLevel >= g_sttCHAOSCASTLE_LEVEL[i)->iLOWER_BOUND &&
			getGameObject(iUserIndex)->Level + getGameObject(iUserIndex)->m_PlayerData->MasterLevel <= g_sttCHAOSCASTLE_LEVEL[i)->iUPPER_BOUND)
		{
			iENTER_LEVEL = i;
			break;
		}
	}

	return iENTER_LEVEL;
}



BOOL CChaosCastle::CheckUserEnterMoney(CGameObject &Obj, int iEnterLevel)
{
	if ( ObjectMaxRange(iUserIndex) == FALSE )
	{
		return FALSE;
	}

	if ( CC_FLOOR_RANGE(iEnterLevel) == FALSE )
	{
		return FALSE;
	}

	if ( getGameObject(iUserIndex)->m_PlayerData->Money >= ::g_iChaosCastle_EnterCost[iEnterLevel) )
	{
		return TRUE;
	}

	return FALSE;
}



BOOL CChaosCastle::PayUserEnterMoney(CGameObject &Obj, int iEnterLevel)
{
	if ( ObjectMaxRange(iUserIndex) == FALSE )
	{
		return FALSE;
	}

	if ( CC_FLOOR_RANGE(iEnterLevel) == FALSE )
	{
		return FALSE;
	}

	if ( getGameObject(iUserIndex)->m_PlayerData->Money >= ::g_iChaosCastle_EnterCost[iEnterLevel) )
	{
		getGameObject(iUserIndex)->m_PlayerData->Money -= ::g_iChaosCastle_EnterCost[iEnterLevel);
		GSProtocol.GCMoneySend(iUserIndex, getGameObject(iUserIndex)->m_PlayerData->Money);

		return TRUE;
	}

	return FALSE;
}



void CChaosCastle::GiveUserDamage(CGameObject &Obj, int iDamage)
{
	if ( !gObjIsConnected(iUserIndex))
		return;

	if ( getGameObject(iUserIndex)->Life <= 0.0 )
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

	if ( getGameObject(iUserIndex)->Type == OBJ_USER )
		IOCP.DataSend(iUserIndex, (BYTE*)&pResult, pResult.h.size);

	getGameObject(iUserIndex)->Life -= iDamage;

	if ( getGameObject(iUserIndex)->Life <= 0.0 )
	{
		getGameObject(iUserIndex)->Life = 0;
		getGameObject(iUserIndex)->m_State = 4;
		getGameObject(iUserIndex)->KillerType = 1;
		getGameObject(iUserIndex)->RegenTime = GetTickCount();
		getGameObject(iUserIndex)->DieRegen = 1;
		getGameObject(iUserIndex)->PathCount = 0;
		
		GSProtocol.GCDiePlayerSend(getGameObject(iUserIndex), getGameObject(iUserIndex)->m_Index, 0, 0);
	}
}



BOOL CChaosCastle::LevelUp(CGameObject &Obj, int iAddExp)
{
	if ( ObjectMaxRange(iUserIndex) == FALSE )
	{
		return 0;
	}

	if ( g_MasterLevelSkillTreeSystem.IsMasterLevelUser(getGameObject(iUserIndex)))
	{
		g_MasterLevelSkillTreeSystem.MasterLevelUp(getGameObject(iUserIndex), iAddExp, 0, "Chaos Castle");
		return 0;
	}

	int iLEFT_EXP = 0;

	//sLog->outBasic("[Chaos Castle] Experience : [%s][%s](%d) Experience: %d + %d",getGameObject(iUserIndex)->AccountID,	getGameObject(iUserIndex)->Name,
	//	getGameObject(iUserIndex)->Level, getGameObject(iUserIndex)->m_PlayerData->Experience,iAddExp);

	::gObjSetExpPetItem(iUserIndex, iAddExp);

	if ( getGameObject(iUserIndex)->Level >= g_ConfigRead.data.common.UserMaxLevel)
	{
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0,45), getGameObject(iUserIndex)->m_Index, 1);
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
			if ( getGameObject(iUserIndex)->Class == CLASS_DARKLORD || getGameObject(iUserIndex)->Class == CLASS_MAGUMSA || getGameObject(iUserIndex)->Class == CLASS_RAGEFIGHTER || getGameObject(iUserIndex)->Class == CLASS_GROWLANCER )
			{
				getGameObject(iUserIndex)->m_PlayerData->LevelUpPoint += g_MaxStatsInfo.GetClass.LevelUpPointMGDL;
			}

			else
			{
				getGameObject(iUserIndex)->m_PlayerData->LevelUpPoint += g_MaxStatsInfo.GetClass.LevelUpPointNormal;
			}

			if ( getGameObject(iUserIndex)->m_PlayerData->PlusStatQuestClear != false )
			{
				getGameObject(iUserIndex)->m_PlayerData->LevelUpPoint++;

				//sLog->outBasic("[%s][%s] LevelUp PlusStatQuest Clear AddStat %d",getGameObject(iUserIndex)->AccountID, getGameObject(iUserIndex)->Name, getGameObject(iUserIndex)->m_PlayerData->LevelUpPoint);
			}
		}

		getGameObject(iUserIndex)->MaxLife += DCInfo.DefClass[getGameObject(iUserIndex)->Class)->LevelLife;
		getGameObject(iUserIndex)->MaxMana += DCInfo.DefClass[getGameObject(iUserIndex)->Class)->LevelMana;
		getGameObject(iUserIndex)->Life = getGameObject(iUserIndex)->MaxLife;
		getGameObject(iUserIndex)->Mana = getGameObject(iUserIndex)->MaxMana;
		gObjNextExpCal(getGameObject(iUserIndex));
		gObjSetBP(iUserIndex);
		GSProtocol.GCLevelUpMsgSend(getGameObject(iUserIndex)->m_Index, 1);
		gObjCalcMaxLifePower(getGameObject(iUserIndex)->m_Index);
		sLog->outBasic("Level Up [%s][%s][%d]", getGameObject(iUserIndex)->AccountID, getGameObject(iUserIndex)->Name, getGameObject(iUserIndex)->Level);
		return 0;
	}

	return iLEFT_EXP;
}



int  CChaosCastle::GetCurrentWinUser(int iChaosCastleIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return -1;
	}

	int iCC_SCORE = -1;
	int iCC_WINNER_INDEX = -1;

	for ( int i=0;i<MAX_CHAOSCASTLE_USER;i++)
	{
		if ( this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex != -1 )
		{
			if ( gObjIsConnected(this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex) == TRUE &&
				 getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->MapNumber == this->GetChaosCastleMapNumber(iChaosCastleIndex) &&	
				this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iUserState == 0 )
			{
				CGameObject lpObj = getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex);

				int iUSER_SCORE = Obj.m_cKillUserCount + Obj.m_cKillMonsterCount * 2;

				if ( iCC_SCORE < iUSER_SCORE )
				{
					iCC_SCORE = iUSER_SCORE;
					iCC_WINNER_INDEX = Obj.m_Index;
				}
			}
		}
	}

	if ( iCC_WINNER_INDEX != -1 )
	{
		if ( getGameObject(iCC_WINNER_INDEX)->m_cKillUserCount == 0 &&
			 getGameObject(iCC_WINNER_INDEX)->m_cKillMonsterCount == 0 )
		{
			int iUSER_COUNT = this->GetCurPlayUser(iChaosCastleIndex);
			int iMONSTER_COUNT = this->GetMonsterListCount(iChaosCastleIndex);

			if ( iUSER_COUNT != 1 || iMONSTER_COUNT != 0 )
			{
				iCC_WINNER_INDEX = -1;
			}
		}
	}

	return iCC_WINNER_INDEX;
}

float CChaosCastle::GetExperienceBonus(int iChaosCastleIndex)
{
	return this->m_stChaosCastleData[iChaosCastleIndex].m_iCastleExperience;
}

int  CChaosCastle::CheckEnterLevel(CGameObject &Obj, int iLevel)
{
	if (ObjectMaxRange(Obj.m_Index) == FALSE)
	{
		return 2;
	}

	if (Obj.Type != OBJ_USER || Obj.Connected <= PLAYER_LOGGED)
	{
		return 2;
	}

	if (Obj.Level + Obj.m_PlayerData->MasterLevel >= g_sttCHAOSCASTLE_LEVEL[iLevel - 1]->iLOWER_BOUND && Obj.Level + Obj.m_PlayerData->MasterLevel <= g_sttCHAOSCASTLE_LEVEL[iLevel - 1]->iUPPER_BOUND)
	{
		return 0;
	}

	if (Obj.Level + Obj.m_PlayerData->MasterLevel < g_sttCHAOSCASTLE_LEVEL[iLevel - 1]->iLOWER_BOUND)
	{
		return -1;
	}

	if (Obj.Level + Obj.m_PlayerData->MasterLevel > g_sttCHAOSCASTLE_LEVEL[iLevel - 1]->iUPPER_BOUND)
	{
		return 1;
	}

	return 2;
}




int  CChaosCastle::CheckEnterItem(CGameObject &Obj)
{
	int iITEM_LEVEL = 0;

	if ( ObjectMaxRange(Obj.m_Index) == FALSE )
	{
		return 0;
	}

	if ( Obj.Type != OBJ_USER || Obj.Connected <= PLAYER_LOGGED )
	{
		return 0;
	}

	for ( int x=0;x<MAIN_INVENTORY_SIZE;x++)
	{
		if ( Obj.pntInventory[x]->IsItem() == TRUE )
		{
			if ( Obj.pntInventory[x]->m_Type == ITEMGET(13,29) )
			{
				iITEM_LEVEL = Obj.pntInventory[x]->m_Level;

				if ( CHECK_LIMIT(iITEM_LEVEL, MAX_CHAOSCASTLE_LEVEL+2) == FALSE )	// #error Change 2 to 1
				{
					iITEM_LEVEL = 0;

				}

				if ( iITEM_LEVEL != 0 )
				{
					return iITEM_LEVEL;
				}
			}
		}
	}

	return iITEM_LEVEL;
}



int  CChaosCastle::CheckPlayEnded(int iChaosCastleIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return FALSE;
	}

	int iCUR_USER_COUNT = this->GetCurPlayUser(iChaosCastleIndex);
	int iCUR_MONSTER_COUNT = this->GetMonsterListCount(iChaosCastleIndex);

	if ( iCUR_USER_COUNT <= 0 )
	{
		return 2;
	}

	if ( iCUR_USER_COUNT == 1 && iCUR_MONSTER_COUNT <= 0)
	{
		return 1;
	}

	return 0;
}



bool CChaosCastle::CheckCanEnter(int iChaosCastleIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return false;
	}

	return this->m_stChaosCastleData[iChaosCastleIndex].m_bCC_CAN_ENTER;
}



bool CChaosCastle::CheckPlayStart(int iChaosCastleIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return false;
	}

	return this->m_stChaosCastleData[iChaosCastleIndex].m_bCC_PLAY_START;
}



bool CChaosCastle::CheckCanStartPlay(int iChaosCastleIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return false;
	}

	int iEnteredUser = this->GetCurPlayUser(iChaosCastleIndex);

	if ( iEnteredUser < this->m_iCC_MIN_USER_START )
	{
		//sLog->outBasic("[Chaos Castle] (%d) GetCurPlayUser() FAILED (UserCount:%d)", iChaosCastleIndex+1, iEnteredUser);

		return false;
	}

	return true;
}



int  CChaosCastle::GetCurrentState(int iChaosCastleIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return -1;
	}

	return this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_STATE;
}



int  CChaosCastle::GetCurEnteredUser(int iChaosCastleIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return -1;
	}

	return this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_ENTERED_USER;
}



int  CChaosCastle::GetCurPlayUser(int iChaosCastleIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
	{
		return -1;
	}

	int iPlayUser = 0;

	for ( int i=0;i<MAX_CHAOSCASTLE_USER;i++)
	{
		if ( this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex != -1 )
		{
			if ( gObjIsConnected(this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex) == TRUE &&
				 getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->MapNumber == this->GetChaosCastleMapNumber(iChaosCastleIndex) )
			{
				iPlayUser++;
			}
		}
	}

	return iPlayUser;
}



/*typedef  _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
}*/

int  CChaosCastle::GetRemainTime(int iChaosCastleIndex)
{
	int iREMAIN_MINUTE = 0;

	if ( this->GetCurrentState(iChaosCastleIndex) == 1 )
	{
		iREMAIN_MINUTE = this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_REMAIN_MSEC / 60000 - this->m_iCC_TIME_MIN_OPEN + 1;
	}
	else
	{
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		int iMIN_HOUR = 24;
		int iMIN_MINUTE = 60;
		BOOL bTIME_CHANGED = FALSE;
		std::vector<CHAOSCASTLE_START_TIME>::iterator it;

		for( it = this->m_vtChaosCastleOpenTime.begin();  it != this->m_vtChaosCastleOpenTime.end(); it++)
		{
			CHAOSCASTLE_START_TIME & pRET = *it;
			
			if ( (sysTime.wHour * 60 + sysTime.wMinute) < (pRET.m_iHour * 60 + pRET.m_iMinute))
			{
				if ( (iMIN_HOUR * 60 + iMIN_MINUTE) > (pRET.m_iHour * 60 + pRET.m_iMinute))
				{
					bTIME_CHANGED = 1;
					iMIN_HOUR = pRET.m_iHour;
					iMIN_MINUTE = pRET.m_iMinute;
				}
			}
		}

		
					
		if ( bTIME_CHANGED == 0 )
		{
			it = this->m_vtChaosCastleOpenTime.begin();
			iMIN_HOUR = 24;
			iMIN_MINUTE = 60;

			for( ;  it != this->m_vtChaosCastleOpenTime.end(); it++)
			{
				CHAOSCASTLE_START_TIME & pRET = *it;
			
				if ( (iMIN_HOUR * 60 + iMIN_MINUTE) > (pRET.m_iHour * 60 + pRET.m_iMinute))
				{
					bTIME_CHANGED = 2;
					iMIN_HOUR = pRET.m_iHour;
					iMIN_MINUTE = pRET.m_iMinute;
				}
			}
		}

		switch ( bTIME_CHANGED )
		{
			case 1:
				iREMAIN_MINUTE = (((iMIN_HOUR * 60 * 60 + iMIN_MINUTE * 60) - (sysTime.wHour * 60 * 60 + sysTime.wMinute * 60 + sysTime.wSecond))) /60 + 1;
				break;
			case 2:
				iREMAIN_MINUTE = ((((iMIN_HOUR + 24) * 60 * 60 + iMIN_MINUTE * 60) - (sysTime.wHour * 60 * 60 + sysTime.wMinute * 60 + sysTime.wSecond)) ) / 60 + 1;
				break;
			default:
				iREMAIN_MINUTE = -1;
				break;
		}
	}

	if ( iREMAIN_MINUTE < 0 )
		iREMAIN_MINUTE = 0;

	return iREMAIN_MINUTE;
}



int  CChaosCastle::GetCurrentRemainSec(int iChaosCastleIndex)
{
	return this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_REMAIN_MSEC / 1000;
}



BOOL CChaosCastle::ObjSetPosition(CGameObject &Obj, int iX, int iY)
{
	if ( !ObjectMaxRange(Obj.m_Index))
		return TRUE;

	CGameObject* lpObj = Obj;

	if (!MAX_MAP_RANGE(Obj.MapNumber))
		return FALSE;

	if ( !CC_MAP_RANGE(Obj.MapNumber))
		return FALSE;

	if ( Obj.Teleport )
		return TRUE;

	BYTE btMapAttr = MapC[Obj.MapNumber].GetAttr(iX, iY);

	if ( (btMapAttr&2) == 2 || (btMapAttr&4) == 4 )
		return FALSE;

	PMSG_POSISTION_SET pMove;
	
	pMove.h.c = 0xC1;
	pMove.h.headcode = 0x10;
	pMove.h.size = sizeof(pMove);
	pMove.X = iX;
	pMove.Y = iY;

	Obj.m_Rest = 0;

	MapC[Obj.MapNumber].ClearStandAttr(Obj.m_OldX,Obj.m_OldY);
	MapC[Obj.MapNumber].SetStandAttr(iX,iY);

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

	if ( Obj.Type == OBJ_USER )
	{
		IOCP.DataSend(Obj.m_Index, (UCHAR *)&pMove2, pMove2.h.size);
	}

	GSProtocol.MsgSendV2(Obj, (UCHAR *)&pMove2, pMove2.h.size);

	MapC[Obj.MapNumber].ClearStandAttr(Obj.m_OldX, Obj.m_OldY);
	MapC[Obj.MapNumber].SetStandAttr(Obj.TX, Obj.TY);

	Obj.m_OldX = Obj.TX;
	Obj.m_OldY = Obj.TY;

	return TRUE;
}




void CChaosCastle::SearchNBlowObjs(int iMapNumber, int iX, int iY)
{
	if ( !CC_MAP_RANGE(iMapNumber))
		return;

	if ( !CHECK_LIMIT(iX, 256) || !CHECK_LIMIT(iY, 256))
		return;

	int iMIN_X = iX-3;
	int iMAX_X = iX+3;
	int iMIN_Y = iY-3;
	int iMAX_Y = iY+3;

	if ( iMIN_X < 0 ) iMIN_X = 0;
	if ( iMIN_Y < 0 ) iMIN_Y = 0;
	if ( iMIN_X > 255 ) iMIN_X = 255;
	if ( iMIN_Y > 255 ) iMIN_Y = 255;
	
	for ( int i=0;i<MAX_CHAOSCASTLE_USER;i++)
	{
		int iChaosCastleIndex = this->GetChaosCastleIndex(iMapNumber);

		if ( this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex == -1 )
		{
			continue;
		}

		int iIndex = this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex;

		if ( Obj.MapNumber == iMapNumber && Obj.Connected > PLAYER_LOGGED )
		{
			if ( Obj.X >= iMIN_X && Obj.X <= iMAX_X &&
				 Obj.Y >= iMIN_Y && Obj.Y <= iMAX_Y )
			{
				this->BlowObjsFromPoint(Obj.m_Index, iMapNumber, iX, iY);

				BYTE btMapAttr = MapC[iMapNumber].m_attrbuf[iY * 256 + iX]&0x08;

				if ( btMapAttr == 8 )
				{
					this->AddFallUser(iChaosCastleIndex, iIndex);

					//sLog->outBasic("[Chaos Castle] (%d) [%s][%s] User Dead In Chaos Castle : Fall from Castle (X:%d, Y:%d)",
					//	iChaosCastleIndex+1, Obj.AccountID, Obj.Name, iX, iY);
				}
			}
		}
	}
}



BOOL CChaosCastle::BlowObjsFromPoint(CGameObject &Obj, int iMapNumber, int& iX, int& iY)
{
	if ( !gObjIsConnected(Obj.m_Index))
		return FALSE;

	if ( !CHECK_LIMIT(iX, 256) || !CHECK_LIMIT(iY, 256))
		return FALSE;

	CGameObject* lpObj = Obj;

	if ( Obj.DieRegen )
		return FALSE;

	if ( Obj.Teleport )
		return FALSE;

	if ( Obj.MapNumber != iMapNumber )
		return FALSE;

	int iOBJ_DIST = this->CalDistance(Obj.X, Obj.Y, iX, iY);

	if ( !CHECK_LIMIT(iOBJ_DIST, 4))
		return FALSE;

	int iSIGN_X = 1;
	int iSIGN_Y = 1;
	int iUX = Obj.X;
	int iUY = Obj.Y;

	if ( iUX > iX )
		iSIGN_X = 1;
	else if ( iUX < iX )
		iSIGN_X = -1;
	else
	{
		int iRND = rand() % 2;
		(!iRND)?iSIGN_X = 1:iSIGN_X = -1;
	}
		
	if ( iUY > iY )
		iSIGN_Y = 1;
	else if ( iUY < iY )
		iSIGN_Y = -1;
	else
	{
		int iRND = rand() % 2;
		(!iRND)?iSIGN_Y = 1:iSIGN_Y = -1;
	}

	BOOL bSuccessBlowOut = FALSE;

	for ( int iBLOWOUT_COUNT = 0;iBLOWOUT_COUNT < 5 ;iBLOWOUT_COUNT++)
	{
		int iBLOW_MIN = g_iChaosCastle_BlowOutDistance[iOBJ_DIST][0];
		int iBLOW_MAX = g_iChaosCastle_BlowOutDistance[iOBJ_DIST][1];
		int iBLOW_X = rand() % (iBLOW_MAX - iBLOW_MIN + 1) + iBLOW_MIN;
		int iBLOW_Y = rand() % (iBLOW_MAX - iBLOW_MIN + 1) + iBLOW_MIN;
		int iRND = rand() % 2;

		if ( iRND )
		{
			if ( iBLOW_X >= iBLOW_MAX )
			{
				iBLOW_X = iBLOW_MAX;
				iBLOW_Y = iBLOW_MIN + (rand() % 2 -1);

				if ( iBLOW_Y < 0 )
					iBLOW_Y = 0;
			}
		}
		else if ( iBLOW_Y >= iBLOW_MAX )
		{
			iBLOW_Y = iBLOW_MAX;
			iBLOW_X = iBLOW_MIN + (rand() % 2 -1);

			if ( iBLOW_X < 0 )
				iBLOW_X = 0;
		}

		int iTX = Obj.X  + iBLOW_X * iSIGN_X;
		int iTY = Obj.Y  + iBLOW_Y * iSIGN_Y;

		if ( iTX < 0 ) iTX = 0;
		if ( iTY < 0 ) iTY = 0;
		if ( iTX > 255 ) iTX = 255;
		if ( iTY > 255 ) iTY = 255;

		bSuccessBlowOut = this->ObjSetPosition(iIndex, iTX, iTY);
		
		if ( bSuccessBlowOut )
		{
			Obj.m_iChaosCastleBlowTime = GetTickCount();
			iX = iTX;
			iY = iTY;

			this->GiveUserDamage(Obj.m_Index, g_iChaosCastle_BlowOutDamage[iOBJ_DIST % 4]);
			break;
		}
	}

	return TRUE;
}



BOOL CChaosCastle::BlowObjsToPoint(CGameObject &Obj, int iMapNumber, int iX, int iY)
{
	return TRUE;
}



int  CChaosCastle::CalDistance(int iX1, int iY1, int iX2, int iY2)
{
	if ( iX1 == iX2 && iY1 == iY2 )
	{
		return 0;
	}

	float fTX = (float)(iX1 - iX2);
	float fTY = (float)(iY1 - iY2);

	return sqrt( (fTX * fTX) + (fTY * fTY) );
}


static const BYTE g_btCastleSafetyZoneMapXY[MAX_CHAOSCASTLE_LEVEL][4] =
{
	{23, 75, 44, 108},
	{23, 75, 44, 108},
	{23, 75, 44, 108},
	{23, 75, 44, 108},
	{23, 75, 44, 108},
	{23, 75, 44, 108},
	{23, 75, 44, 108}
};

static const BYTE g_btCastleCenterHollowZoneMapXY[MAX_CHAOSCASTLE_LEVEL][2][4] =
{
	{32, 84, 35, 87, 32, 96, 35, 99},
	{32, 84, 35, 87, 32, 96, 35, 99},
	{32, 84, 35, 87, 32, 96, 35, 99},
	{32, 84, 35, 87, 32, 96, 35, 99},
	{32, 84, 35, 87, 32, 96, 35, 99},
	{32, 84, 35, 87, 32, 96, 35, 99},
	{32, 84, 35, 87, 32, 96, 35, 99}
};

void CChaosCastle::SafetyCastleZone(int iChaosCastleIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
		return ;

	int iMapNumber = this->GetChaosCastleMapNumber(iChaosCastleIndex);

	for ( int i=g_btCastleSafetyZoneMapXY[iChaosCastleIndex][0] ; i <= g_btCastleSafetyZoneMapXY[iChaosCastleIndex][2] ; i++ )
	{
		for(int j=g_btCastleSafetyZoneMapXY[iChaosCastleIndex][1] ; j <= g_btCastleSafetyZoneMapXY[iChaosCastleIndex][3] ; j++ )
		{
			MapC[iMapNumber].m_attrbuf[ j * 256 + i] |= 1;
		}
	}

	for ( int iHOLE_NUM = 0; iHOLE_NUM <2;iHOLE_NUM++)
	{
		for(int i=g_btCastleCenterHollowZoneMapXY[iChaosCastleIndex][iHOLE_NUM][0] ; i<= g_btCastleCenterHollowZoneMapXY[iChaosCastleIndex][iHOLE_NUM][2] ; i++ )
		{
			for (int j=g_btCastleCenterHollowZoneMapXY[iChaosCastleIndex][iHOLE_NUM][1] ; j<= g_btCastleCenterHollowZoneMapXY[iChaosCastleIndex][iHOLE_NUM][3] ; j++ )
			{
				MapC[iMapNumber].m_attrbuf[ j * 256 + i] |= 8;
			}
		}
	}
}



void CChaosCastle::UnSafetyCastleZone(int iChaosCastleIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
		return ;

	int iMapNumber = this->GetChaosCastleMapNumber(iChaosCastleIndex);

	for ( int i=g_btCastleSafetyZoneMapXY[iChaosCastleIndex][0] ; i <= g_btCastleSafetyZoneMapXY[iChaosCastleIndex][2] ; i++ )
	{
		for(int j=g_btCastleSafetyZoneMapXY[iChaosCastleIndex][1] ; j <= g_btCastleSafetyZoneMapXY[iChaosCastleIndex][3] ; j++ )
		{
			MapC[iMapNumber].m_attrbuf[ j * 256 + i] &= ~1;
		}
	}

	for ( int iHOLE_NUM = 0; iHOLE_NUM <2;iHOLE_NUM++)
	{
		for(int i=g_btCastleCenterHollowZoneMapXY[iChaosCastleIndex][iHOLE_NUM][0] ; i<= g_btCastleCenterHollowZoneMapXY[iChaosCastleIndex][iHOLE_NUM][2] ; i++ )
		{
			for (int j=g_btCastleCenterHollowZoneMapXY[iChaosCastleIndex][iHOLE_NUM][1] ; j<= g_btCastleCenterHollowZoneMapXY[iChaosCastleIndex][iHOLE_NUM][3] ; j++ )
			{
				MapC[iMapNumber].m_attrbuf[ j * 256 + i] |= 8;
			}
		}
	}
}



void CChaosCastle::SendCastleZoneSafetyInfo(int iChaosCastleIndex, bool bDoSet)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
		return ;

	char cTEMP_BUF[256];
	PMSG_SETMAPATTR_COUNT * lpMsg = (PMSG_SETMAPATTR_COUNT *)cTEMP_BUF;

	PHeadSetB((BYTE*)lpMsg, 0x46, sizeof(PMSG_SETMAPATTR_COUNT)+sizeof(PMSG_SETMAPATTR)*6);
	PMSG_SETMAPATTR * lpMsgBody = (PMSG_SETMAPATTR *)&cTEMP_BUF[7];
	lpMsg->btType = 0;
	lpMsg->btCount = 1;
	lpMsg->btMapAttr = 1;
	(bDoSet)?(lpMsg->btMapSetType=0) :( lpMsg->btMapSetType=1);

	lpMsgBody[0].btX = ::g_btCastleSafetyZoneMapXY[iChaosCastleIndex][0];
	lpMsgBody[0].btY = ::g_btCastleSafetyZoneMapXY[iChaosCastleIndex][1];
	lpMsgBody[1].btX   = ::g_btCastleSafetyZoneMapXY[iChaosCastleIndex][2];
	lpMsgBody[1].btY   = ::g_btCastleSafetyZoneMapXY[iChaosCastleIndex][3];

	for (int i=g_ConfigRead.server.GetObjectStartUserIndex();i<g_ConfigRead.server.GetObjectMax();i++)
	{
		if ( getGameObject(i)->MapNumber == this->GetChaosCastleMapNumber(iChaosCastleIndex) )
		{
			if ( getGameObject(i)->Connected > PLAYER_LOGGED )
			{
				IOCP.DataSend(i, (BYTE*)lpMsg, lpMsg->h.size);
			}
		}
	}
}



void CChaosCastle::CheckMonsterInDieTile(int iChaosCastleIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
		return ;

	int iUSER_AXIS = 0;
	std::vector<POINT> vtMAP_UNTRAP[4];

	for ( int iAXIS =0;iAXIS <	4 ; iAXIS++)
	{
		for(int iMAPX = g_rtPOINT_TRAP[iAXIS].left; iMAPX <= g_rtPOINT_TRAP[iAXIS].right ; iMAPX++)
		{
			for(int iMAPY = g_rtPOINT_TRAP[iAXIS].top; iMAPY <= g_rtPOINT_TRAP[iAXIS].bottom ; iMAPY++)
			{
				int iMapNumber = this->GetChaosCastleMapNumber(iChaosCastleIndex);

				if (!MAX_MAP_RANGE(iMapNumber))
				{
					//sLog->outError( "[ERROR] iMapNumber is out of range %s %d", __FILE__, __LINE__);
					return;
				}

				BYTE btMapAttr = MapC[iMapNumber].GetAttr(iMAPX, iMAPY);

				if ( (btMapAttr&2)!= 2 && (btMapAttr&4)!= 4 && (btMapAttr&8)!= 8 )
				{
					POINT iPT = {iMAPX, iMAPY};
					vtMAP_UNTRAP[iAXIS].push_back(iPT);
				}
			}
		}
	}

	for(int iMON = 0;iMON < MAX_CHAOSCASTLE_MONSTER ; iMON ++ )
	{
		if ( this->m_stChaosCastleData[iChaosCastleIndex].m_lCC_MONSTER_COUNT[iMON] == -1 )
			continue;

		int iMON_INDEX = this->m_stChaosCastleData[iChaosCastleIndex].m_lCC_MONSTER_COUNT[iMON];

		if ( !CHECK_LIMIT(iMON_INDEX, g_ConfigRead.server.GetObjectMaxMonster()))
		{
			this->m_stChaosCastleData[iChaosCastleIndex].m_lCC_MONSTER_COUNT[iMON] = -1;
			continue;
		}

		if ( getGameObject(iMON_INDEX)->Life > 0.0f && CC_MAP_RANGE(getGameObject(iMON_INDEX)->MapNumber) != FALSE && getGameObject(iMON_INDEX)->Connected > PLAYER_EMPTY )
		{
			int iSX = getGameObject(iMON_INDEX)->X;
			int iSY = getGameObject(iMON_INDEX)->Y;

			int iMapNumber = this->GetChaosCastleMapNumber(iChaosCastleIndex);

			BYTE btMapAttr = MapC[iMapNumber].m_attrbuf[iSY * 256 + iSX] & 0x08;

			if ( btMapAttr == 8 )
			{
				int n;
				for ( n=0;n<MAX_CC_TRAP_STEP;n++)
				{
					if ( iSX >= g_rtPOINT_FRAME[n].left && iSX <= g_rtPOINT_FRAME[n].right &&
						 iSY >= g_rtPOINT_FRAME[n].top && iSY <= g_rtPOINT_FRAME[n].bottom )
					{
						iUSER_AXIS = n;
						break;
					}
				}

				if ( n == MAX_CC_TRAP_STEP )
					continue;

				if ( vtMAP_UNTRAP[iUSER_AXIS].empty() == false )
				{
					POINT ptEMPTY = vtMAP_UNTRAP[iUSER_AXIS].back();
					vtMAP_UNTRAP[iUSER_AXIS].pop_back();

					if ( this->ObjSetPosition(iMON_INDEX, ptEMPTY.x, ptEMPTY.y) == TRUE )
					{
						getGameObject(iMON_INDEX)->m_iChaosCastleBlowTime = GetTickCount();
					}
				}
			}
		}
	}
}



void CChaosCastle::CheckUserInDieTile(int iChaosCastleIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
		return;

	for ( int i=0;i<MAX_CHAOSCASTLE_USER;i++)
	{
		if ( this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex != -1 )
		{
			if ( gObjIsConnected(this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex) ==TRUE &&
				 getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->MapNumber == this->GetChaosCastleMapNumber(iChaosCastleIndex) )
			{
				if ( getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->Life > 0.0 )
				{
					int iSX = getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->X;
					int iSY = getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->Y;
					int iMapNumber = this->GetChaosCastleMapNumber(iChaosCastleIndex);

					BYTE btMapAttr = MapC[iMapNumber].m_attrbuf[iSY * 256 + iSX] & 0x08;

					if ( btMapAttr == 8 )
					{
						getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->Life = 0;
						getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->m_State = 4;
						getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->KillerType = 1;
						getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->RegenTime = GetTickCount();
						getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->DieRegen = 1;
						getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->PathCount = 0;

						GSProtocol.GCDiePlayerSend(getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex),
							this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex, 0, 0);

						//sLog->outBasic("[Chaos Castle] (%d) [%s][%s] User Dead In Chaos Castle : Fall from Castle (X:%d, Y:%d)",	iChaosCastleIndex+1, getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->AccountID,
						//	getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->Name,iSX, iSY);

						this->SendFailMessage(iChaosCastleIndex, this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex);

					}
				}
			}
		}
	}
}



void CChaosCastle::PlayFailedRollBack(int iChaosCastleIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
		return;

	for ( int i=0;i<MAX_CHAOSCASTLE_USER;i++)
	{
		if ( this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex != -1 )
		{
			if ( gObjIsConnected(this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex) ==TRUE &&
				 getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->MapNumber == this->GetChaosCastleMapNumber(iChaosCastleIndex) )
			{
				int iFIRST_MONEY = getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->m_PlayerData->Money;
				int iPAYBACK_MONEY = g_iChaosCastle_EnterCost[iChaosCastleIndex] + g_iChaosCastle_GuardSetValue[iChaosCastleIndex];
				getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex]->m_PlayerData->Money += g_iChaosCastle_EnterCost[iChaosCastleIndex] + g_iChaosCastle_GuardSetValue[iChaosCastleIndex);
				int iNOW_MONEY = getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->m_PlayerData->Money;

				GSProtocol.GCMoneySend(this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex, getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->m_PlayerData->Money);
				
				if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
					gObjMoveGate(this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex, 333);
				else
					gObjMoveGate(this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex, 22);

				PMSG_NOTICE Notice;

				TNotice::MakeNoticeMsgEx(&Notice, 1, Lang.GetText(0,130));

				IOCP.DataSend(this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex, (BYTE*)&Notice, Notice.h.size);

				//sLog->outBasic("[Chaos Castle] (%d) [%s][%s] Payback to User ChaosCastle Money (FIRST:%d, PAYBACK:%d, FINAL:%d)",	iChaosCastleIndex+1, getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->AccountID,
				//	getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->Name,	iFIRST_MONEY, iPAYBACK_MONEY, iNOW_MONEY);

			}
		}
	}
}

static const int g_iChaosCastle_ExpTable[MAX_CHAOSCASTLE_LEVEL][2] =
{
	0x1F4, 0x3E8,
	0x3E8, 0x5DC,
	0x5DC, 0x7D0,
	0x7D0, 0x9C4,
	0x9C4, 0xBB8,
	0xBB8, 0xDAC,
	0xFA0, 0x1194
};

void CChaosCastle::RewardUserEXP(int iChaosCastleIndex, int iChaosCastleSubIndex, BOOL bWinner)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
		return;

	if ( CC_SUB_FLOOR_RANGE(iChaosCastleSubIndex) == FALSE )
		return;

	if ( this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[iChaosCastleSubIndex].m_iIndex != -1 )
	{
		if ( gObjIsConnected(this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[iChaosCastleSubIndex].m_iIndex) ==TRUE )
		{
			if ( getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[iChaosCastleSubIndex]->m_iIndex)->m_cChaosCastleIndex == -1 ||
				 getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[iChaosCastleSubIndex]->m_iIndex)->m_cChaosCastleSubIndex == -1 )
			{
				return;
			}

			int iKILLCOUNT_USER = getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[iChaosCastleSubIndex]->m_iIndex)->m_cKillUserCount;
			int iKILLCOUNT_MONSTER = getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[iChaosCastleSubIndex]->m_iIndex)->m_cKillMonsterCount;
			int iTOT_EXP = iKILLCOUNT_USER * g_iChaosCastle_ExpTable[iChaosCastleIndex][0] + iKILLCOUNT_MONSTER * g_iChaosCastle_ExpTable[iChaosCastleIndex][1];

			int iREWARD_EXP = this->CalcSendRewardEXP(this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[iChaosCastleSubIndex].m_iIndex,
				iTOT_EXP, iKILLCOUNT_USER, iKILLCOUNT_MONSTER);

			PMSG_DEVILSQUARERESULT pMsg;

			pMsg.MyRank = bWinner;
			pMsg.Count = 0xFE;
			std::memcpy(pMsg.Score[0].Name, getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[iChaosCastleSubIndex]->m_iIndex)->Name, 10);
			pMsg.Score[0].BonusExp = iTOT_EXP;
			pMsg.Score[0].BonusZen = iKILLCOUNT_USER;
			pMsg.Score[0].TotalScore = iKILLCOUNT_MONSTER;
			PHeadSetB((BYTE*)&pMsg, 0x93, 0x35);

			IOCP.DataSend(this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[iChaosCastleSubIndex].m_iIndex, (BYTE*)&pMsg, pMsg.h.size);

			this->ChaosCastleRank(this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[iChaosCastleSubIndex].m_iIndex, iKILLCOUNT_USER,
				iKILLCOUNT_MONSTER, iTOT_EXP, bWinner);

			//sLog->outBasic("[Chaos Castle] (%d) [%s][%s] Reward User EXP (USER_KILL:%d, MON_KILL:%d, TOT_EXP:%d, TOT_REWARD_EXP:%d)",iChaosCastleIndex+1, getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[iChaosCastleSubIndex]->m_iIndex)->AccountID,
			//	getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[iChaosCastleSubIndex]->m_iIndex)->Name,	iKILLCOUNT_USER, iKILLCOUNT_MONSTER, iTOT_EXP, iREWARD_EXP);
		}
	}
}




void CChaosCastle::ProcessTrapStatus(int iChaosCastleIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
		return;

	int iCUR_LIVES = this->GetCurPlayUser(iChaosCastleIndex) + this->GetMonsterListCount(iChaosCastleIndex);

	if ( iCUR_LIVES > 40 )
	{
		this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_TRAP_STATUS = 0;
	}
	else if ( iCUR_LIVES > 30 )
	{
		this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_TRAP_STATUS = 1;
		this->SendNoticeState(iChaosCastleIndex, 8);
	}
	else if ( iCUR_LIVES > 20 )
	{
		this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_TRAP_STATUS = 2;	

		this->SendNoticeState(iChaosCastleIndex, 9);
	}
	else
	{
		this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_TRAP_STATUS = 3;
		this->SendNoticeState(iChaosCastleIndex, 10);
	}

}



void CChaosCastle::CalUsersInTrap(int iChaosCastleIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
		return;

	if (this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_TRAP_STATUS== 0 )
		return;

	if ( !CHECK_LIMIT(this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_TRAP_STATUS, 4))
		return;

	int iTRAP_STEP = this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_TRAP_STATUS;

	for ( int i=0;i<MAX_CHAOSCASTLE_USER;i++)
	{
		if ( this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex != -1 )
		{
			if ( gObjIsConnected(this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex) == TRUE &&
				 getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->MapNumber == this->GetChaosCastleMapNumber(iChaosCastleIndex) )
			{
				int iSX = getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->X;
				int iSY = getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->Y;
				
				if ( iSX >= g_iChaosCastle_GroundAxis[0] &&
					 iSX <= g_iChaosCastle_GroundAxis[2] &&
					 iSY >= g_iChaosCastle_GroundAxis[1] &&
					 iSY <= g_iChaosCastle_GroundAxis[3] &&
					 ( iSX < g_iChaosCastle_DamageAxis[iTRAP_STEP][0] ||
					   iSX > g_iChaosCastle_DamageAxis[iTRAP_STEP][2] ||
					   iSY < g_iChaosCastle_DamageAxis[iTRAP_STEP][1] ||
					   iSY > g_iChaosCastle_DamageAxis[iTRAP_STEP][3] ) )
				{
					this->GiveUserDamage(this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[i].m_iIndex, 10);
					
					if ( gObjCheckUsedBuffEffect(getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex), BUFFTYPE_FREEZE) != 0 )
					{
						getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->DelayActionTime = 800;
						getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex)->DelayLevel = 1;
						gObjAddBuffEffect(getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[i]->m_iIndex), BUFFTYPE_FREEZE, 0, 0, 0, 0, 3);
					}
				}
			}
		}
	}
}



BOOL CChaosCastle::CheckWearingMOPH(CGameObject &Obj)
{
	if (  gObjIsConnected(iUserIndex) == FALSE )
	{
		return FALSE;
	}

	if ( getGameObject(iUserIndex)->pntInventory[10)->IsItem()  )
	{
		if ( getGameObject(iUserIndex)->pntInventory[10)->m_Type == ITEMGET(13,10) ) // Tranformation Ring
		{
			return TRUE;
		}

		if ( getGameObject(iUserIndex)->pntInventory[10)->m_Type == ITEMGET(13,39) ) 
		{
			return TRUE;
		}

		if ( getGameObject(iUserIndex)->pntInventory[10)->m_Type == ITEMGET(13,40) ) 
		{
			return TRUE;
		}

		if ( getGameObject(iUserIndex)->pntInventory[10)->m_Type == ITEMGET(13,41) ) 
		{
			return TRUE;
		}

		if ( getGameObject(iUserIndex)->pntInventory[10)->m_Type == ITEMGET(13,42) ) 
		{
			return TRUE;
		}

		if ( getGameObject(iUserIndex)->pntInventory[10)->m_Type == ITEMGET(13,68) ) 
		{
			return TRUE;
		}

		if ( getGameObject(iUserIndex)->pntInventory[10)->m_Type == ITEMGET(13,76) ) 
		{
			return TRUE;
		}

		if ( getGameObject(iUserIndex)->pntInventory[10)->m_Type == ITEMGET(13,77) ) 
		{
			return TRUE;
		}

		if ( getGameObject(iUserIndex)->pntInventory[10)->m_Type == ITEMGET(13,78) ) 
		{
			return TRUE;
		}

		if ( getGameObject(iUserIndex)->pntInventory[10)->m_Type == ITEMGET(13,122) ) 
		{
			return TRUE;
		}
		if (getGameObject(iUserIndex)->pntInventory[10)->m_Type == ITEMGET(13, 163))
		{
			return TRUE;
		}
		if (getGameObject(iUserIndex)->pntInventory[10)->m_Type == ITEMGET(13, 164))
		{
			return TRUE;
		}
		if (getGameObject(iUserIndex)->pntInventory[10)->m_Type == ITEMGET(13, 165))
		{
			return TRUE;
		}
		if (getGameObject(iUserIndex)->pntInventory[10)->m_Type == ITEMGET(13, 166))
		{
			return TRUE;
		}
	}

	if ( getGameObject(iUserIndex)->pntInventory[11)->IsItem() )
	{
		if ( getGameObject(iUserIndex)->pntInventory[11)->m_Type == ITEMGET(13,10) ) // Tranformation Ring
		{
			return TRUE;
		}

		if ( getGameObject(iUserIndex)->pntInventory[11)->m_Type == ITEMGET(13,39) ) 
		{
			return TRUE;
		}

		if ( getGameObject(iUserIndex)->pntInventory[11)->m_Type == ITEMGET(13,40) ) 
		{
			return TRUE;
		}

		if ( getGameObject(iUserIndex)->pntInventory[11)->m_Type == ITEMGET(13,41) ) 
		{
			return TRUE;
		}

		if ( getGameObject(iUserIndex)->pntInventory[11)->m_Type == ITEMGET(13,42) ) 
		{
			return TRUE;
		}

		if ( getGameObject(iUserIndex)->pntInventory[11)->m_Type == ITEMGET(13,68) ) 
		{
			return TRUE;
		}

		if ( getGameObject(iUserIndex)->pntInventory[11)->m_Type == ITEMGET(13,76) ) 
		{
			return TRUE;
		}

		if ( getGameObject(iUserIndex)->pntInventory[11)->m_Type == ITEMGET(13,77) ) 
		{
			return TRUE;
		}

		if ( getGameObject(iUserIndex)->pntInventory[11)->m_Type == ITEMGET(13,78) ) 
		{
			return TRUE;
		}

		if ( getGameObject(iUserIndex)->pntInventory[11)->m_Type == ITEMGET(13,122) ) 
		{
			return TRUE;
		}
		if (getGameObject(iUserIndex)->pntInventory[11)->m_Type == ITEMGET(13, 163))
		{
			return TRUE;
		}
		if (getGameObject(iUserIndex)->pntInventory[11)->m_Type == ITEMGET(13, 164))
		{
			return TRUE;
		}
		if (getGameObject(iUserIndex)->pntInventory[11)->m_Type == ITEMGET(13, 165))
		{
			return TRUE;
		}
		if (getGameObject(iUserIndex)->pntInventory[11)->m_Type == ITEMGET(13, 166))
		{
			return TRUE;
		}
	}

	return FALSE;
}

void CChaosCastle::GiveWinnerItem(int iChaosCastleIndex, int iWinnerIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
		return;

	if ( !gObjIsConnected(iWinnerIndex))
		return;

	if ( g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE )
	{
		this->GDReqSetUBFReward_CCBattle(iWinnerIndex, 1);
		return;
	}

	g_CashShop.AddCoin(getGameObject(iWinnerIndex), EVENT_CC);

	switch (iChaosCastleIndex)
	{
		case 0:
			g_BagManager.UseBag_GremoryCase(iWinnerIndex, BAG_EVENT, EVENTBAG_CC1, iWinnerIndex, GC_STORAGE_CHARACTER, GC_REWARD_CHAOS_CASTLE, 30);
			break;
		case 1:
			g_BagManager.UseBag_GremoryCase(iWinnerIndex, BAG_EVENT, EVENTBAG_CC2, iWinnerIndex, GC_STORAGE_CHARACTER, GC_REWARD_CHAOS_CASTLE, 30);
			break;
		case 2:
			g_BagManager.UseBag_GremoryCase(iWinnerIndex, BAG_EVENT, EVENTBAG_CC3, iWinnerIndex, GC_STORAGE_CHARACTER, GC_REWARD_CHAOS_CASTLE, 30);
			break;
		case 3:
			g_BagManager.UseBag_GremoryCase(iWinnerIndex, BAG_EVENT, EVENTBAG_CC4, iWinnerIndex, GC_STORAGE_CHARACTER, GC_REWARD_CHAOS_CASTLE, 30);
			break;
		case 4:
			g_BagManager.UseBag_GremoryCase(iWinnerIndex, BAG_EVENT, EVENTBAG_CC5, iWinnerIndex, GC_STORAGE_CHARACTER, GC_REWARD_CHAOS_CASTLE, 30);
			break;
		case 5:
			g_BagManager.UseBag_GremoryCase(iWinnerIndex, BAG_EVENT, EVENTBAG_CC6, iWinnerIndex, GC_STORAGE_CHARACTER, GC_REWARD_CHAOS_CASTLE, 30);
			break;
		case 6:
			g_BagManager.UseBag_GremoryCase(iWinnerIndex, BAG_EVENT, EVENTBAG_CC7, iWinnerIndex, GC_STORAGE_CHARACTER, GC_REWARD_CHAOS_CASTLE, 30);
			break;
	}
}


int  CChaosCastle::ReCalcUserCount(int iChaosCastleIndex)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
		return -1;

	int iRET_VAL = 0;
	int iPlayUser = this->GetCurPlayUser(iChaosCastleIndex);

	if ( iPlayUser > 0 )
	{
		this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_ENTERED_USER = iPlayUser;
		iRET_VAL = iPlayUser;
	}
	else
	{
		this->m_stChaosCastleData[iChaosCastleIndex].m_iCC_ENTERED_USER = 0;
		iRET_VAL = 0;
	}
	return iRET_VAL;
}

int CChaosCastle::GetChaosCastleMapNumber(int iChaosCastleIndex)
{
	int iMapNumber = 0;

	switch(iChaosCastleIndex)
	{
	case 0:
		iMapNumber = MAP_INDEX_CHAOSCASTLE1;
		break;
	case 1:
		iMapNumber = MAP_INDEX_CHAOSCASTLE2;
		break;
	case 2:
		iMapNumber = MAP_INDEX_CHAOSCASTLE3;
		break;
	case 3:
		iMapNumber = MAP_INDEX_CHAOSCASTLE4;
		break;
	case 4:
		iMapNumber = MAP_INDEX_CHAOSCASTLE5;
		break;
	case 5:
		iMapNumber = MAP_INDEX_CHAOSCASTLE6;
		break;
	case 6:
		iMapNumber = MAP_INDEX_CHAOSCASTLE7;
		break;
	}

	return iMapNumber;
}

int CChaosCastle::GetChaosCastleIndex(int iMAP_NUM)
{
	int iChaosCastleIndex = -1;

	switch(iMAP_NUM)
	{
	case MAP_INDEX_CHAOSCASTLE1:
		iChaosCastleIndex = 0;
		break;
	case MAP_INDEX_CHAOSCASTLE2:
		iChaosCastleIndex = 1;
		break;
	case MAP_INDEX_CHAOSCASTLE3:
		iChaosCastleIndex = 2;
		break;
	case MAP_INDEX_CHAOSCASTLE4:
		iChaosCastleIndex = 3;
		break;
	case MAP_INDEX_CHAOSCASTLE5:
		iChaosCastleIndex = 4;
		break;
	case MAP_INDEX_CHAOSCASTLE6:
		iChaosCastleIndex = 5;
		break;
	case MAP_INDEX_CHAOSCASTLE7:
		iChaosCastleIndex = 6;
		break;
	}

	return iChaosCastleIndex;
}

CHAOSCASTLE_MONSTER_POSITION * CChaosCastle::GetMonsterPosData(int iPosNum, int iChaosCastleIndex)
{
	if (CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE)
	{
		return NULL;
	}

	if (iPosNum < 0 || iPosNum >= 100)
	{
		return NULL;
	}

	return &this->m_CCMP[iChaosCastleIndex][iPosNum];
}

struct PMSG_ANS_CHAOSCASTLE_RANKING {
	PBMSG_HEAD2 h;
	char AccountID[10];
	char GameID[10];
	int ServerCode;
	int Class;
	int Castle;
	int PlayerKill;
	int MonsterKill;
	__int64 Experience;
	bool IsWinner;
};

void CChaosCastle::ChaosCastleRank(CGameObject &Obj, int iPlayer, int iMonster, __int64 iExp, bool bWinner)
{
	PMSG_ANS_CHAOSCASTLE_RANKING pMsg = {0};

	PHeadSubSetB((BYTE*)&pMsg, 0xBD, 0x05, sizeof(pMsg));

	pMsg.Castle = getGameObject(iUserIndex)->m_cChaosCastleIndex;
	pMsg.Class = getGameObject(iUserIndex)->Class;
	pMsg.PlayerKill = iPlayer;
	pMsg.MonsterKill = iMonster;
	pMsg.Experience = iExp;
	pMsg.IsWinner = bWinner;
	std::memcpy(pMsg.AccountID, getGameObject(iUserIndex)->AccountID, MAX_ACCOUNT_LEN);
	std::memcpy(pMsg.GameID, getGameObject(iUserIndex)->Name, MAX_ACCOUNT_LEN);

	wsDataCli.DataSend(reinterpret_cast<char *>(&pMsg), pMsg.h.size);
}

void CChaosCastle::GD_Req_Save_KillPoint_UBF(int index, char *Name, int KillPoint, int CastleIndex)
{
	if (!ObjectMaxRange(index))
	{
		return;
	}

	if (!gObjIsConnected(index))
	{
		return;
	}

	if ( KillPoint <= 0 )
	{
		return;
	}

	PMSG_REQ_SAVE_CHAOSCASTLE_KILLPOINT_UBF pMsg;

	std::memcpy(pMsg.szCharName, Name, MAX_ACCOUNT_LEN);
	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xF9;
	pMsg.h.subcode = 0x01;
	pMsg.nPoint = KillPoint;
	pMsg.nIndex = index;
	pMsg.nCastleIndex = CastleIndex;

	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
	sLog->outBasic("[CChaosCastle][GD_Req_Save_KillPoint][%s][%s][%s] Save Requested To DB, KillPoint:%d Castle Index:%d",
		getGameObject(index)->AccountID, getGameObject(index)->m_PlayerData->m_RealNameOfUBF, getGameObject(index)->Name, KillPoint, CastleIndex);
}

struct PMSG_UBFCHAOSCASTLERESULT
{
	PBMSG_HEAD2 h;
	char szName[MAX_ACCOUNT_LEN];
	BYTE btWinner;
	int nTotalScore;
	int nKillCounterUser;
	int nKillCounterMonster;
};

void CChaosCastle::RewardUserKillPointUBF(int iChaosCastleIndex, int iChaosCastleSubIndex, bool bWinner)
{
	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
		return;

	if ( CC_SUB_FLOOR_RANGE(iChaosCastleSubIndex) == FALSE )
		return;

	if (this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[iChaosCastleSubIndex].m_iIndex == -1)
	{
		return;
	}

	if (gObjIsConnected(this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[iChaosCastleSubIndex].m_iIndex) == FALSE)
	{
		return;
	}

	if (getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[iChaosCastleSubIndex]->m_iIndex)->m_cChaosCastleIndex == -1)
	{
		return;
	}

	if (getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[iChaosCastleSubIndex]->m_iIndex)->m_cChaosCastleSubIndex == -1)
	{
		return;
	}

	int iKILLCOUNT_USER = getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[iChaosCastleSubIndex]->m_iIndex)->m_cKillUserCount;
	int iKILLCOUNT_MONSTER = getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[iChaosCastleSubIndex]->m_iIndex)->m_cKillMonsterCount;
	int iTOT_EXP = (g_iChaosCastle_ExpTable[iChaosCastleIndex][1] * iKILLCOUNT_MONSTER) + (g_iChaosCastle_ExpTable[iChaosCastleIndex][0] * iKILLCOUNT_USER);
	int iREWARD_EXP = g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE ? 0 : this->CalcSendRewardEXP(this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[iChaosCastleSubIndex].m_iIndex,
		iTOT_EXP, iKILLCOUNT_USER, iKILLCOUNT_MONSTER);

	if ( g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE )
	{
		PMSG_UBFCHAOSCASTLERESULT pUBFMsg;

		pUBFMsg.btWinner = bWinner;
		std::memcpy(pUBFMsg.szName, getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[iChaosCastleSubIndex]->m_iIndex)->Name, MAX_ACCOUNT_LEN);
		pUBFMsg.nKillCounterMonster = iKILLCOUNT_MONSTER;
		pUBFMsg.nKillCounterUser = iKILLCOUNT_USER;
		int Tot_Point = 0;

		if (bWinner)
		{
			Tot_Point = 50;
		}

		Tot_Point += 3 * iKILLCOUNT_USER + 2 * iKILLCOUNT_MONSTER;
		pUBFMsg.nTotalScore = Tot_Point;

		PHeadSubSetB((BYTE*)&pUBFMsg, 0x93, 0x01, sizeof(pUBFMsg));
		IOCP.DataSend(this->m_stChaosCastleData[iChaosCastleIndex].m_UserData[iChaosCastleSubIndex].m_iIndex, (BYTE*)&pUBFMsg, pUBFMsg.h.size);

		sLog->outBasic("[UBF][Chaos Castle] (%d) [%s][%s][%s] GameResult (USER_KILL:%d, MON_KILL:%d, TOT_KILLPOINT:%d, TOT_REWARD_EXP:%d)",
			iChaosCastleIndex + 1, getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[iChaosCastleSubIndex]->m_iIndex)->AccountID,
			getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[iChaosCastleSubIndex]->m_iIndex)->Name,
			getGameObject(this->m_stChaosCastleData[iChaosCastleIndex]->m_UserData[iChaosCastleSubIndex]->m_iIndex)->m_PlayerData->m_RealNameOfUBF,
			iKILLCOUNT_USER, iKILLCOUNT_MONSTER, Tot_Point, iREWARD_EXP);
	}
}

struct SDHP_REQ_SET_CC_WINNER_INFO_UBF
{
	PBMSG_HEAD2 h;
	char UBFName[MAX_ACCOUNT_LEN + 1];
	BYTE btRewardType;
};

void CChaosCastle::GDReqSetUBFReward_CCBattle(CGameObject &Obj, BYTE byRewardType)
{
	if (!ObjectMaxRange(iUserIndex))
	{
		return;
	}

	CGameObject lpObj = getGameObject(iUserIndex);

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	sLog->outBasic("[UBF][Chaos Castle][GDReqSetUBFReward_CCBattle]ACC:%s , NAME :%s,Real NAME :%s, RewardType:%d",
		Obj.AccountID, Obj.Name, Obj.m_PlayerData->m_RealNameOfUBF, byRewardType);

	SDHP_REQ_SET_CC_WINNER_INFO_UBF pMsg;
	
	std::memcpy(pMsg.UBFName, Obj.Name, MAX_ACCOUNT_LEN+1);
	pMsg.btRewardType = byRewardType;

	PHeadSubSetB((BYTE*)&pMsg, 0xF9, 0x02, sizeof(pMsg));
	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
}

void CChaosCastle::SetUBFGetReward(CGameObject &Obj, WORD wItemCode, UINT64 ItemSerial, BYTE btItemPos)
{
	if (!ObjectMaxRange(iUserIndex))
	{
		return;
	}

	CGameObject lpObj = getGameObject(iUserIndex);

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	int iChaosCastleIndex = getGameObject(iUserIndex)->m_cChaosCastleIndex;
	int iChaosCastleSubIndex = getGameObject(iUserIndex)->m_cChaosCastleSubIndex;

	if ( CC_FLOOR_RANGE(iChaosCastleIndex) == FALSE )
		return;

	if ( CC_SUB_FLOOR_RANGE(iChaosCastleSubIndex) == FALSE )
		return;

	if (wItemCode != ITEMGET(14,261) && wItemCode != ITEMGET(14,262))
	{
		return;
	}

	gObjInventoryDeleteItem(iUserIndex, btItemPos);
	GSProtocol.GCInventoryItemDeleteSend(iUserIndex, btItemPos, 0);
	
	this->GDReqSetUBFReward_CCBattle(iUserIndex, 0);
}

void CChaosCastle::GiveUBFReward(CGameObject &Obj, BYTE byRewardType)
{
	_stGremoryCaseItem stItem;
	WORD wItemType = -1;

	if (byRewardType == 0) // Monster Drop
	{
		wItemType = ITEMGET(14,261);
	}

	else if (byRewardType == 1) // Winner Prize
	{
		wItemType = ITEMGET(14,262);
	}

	stItem.btStorageType = GC_STORAGE_CHARACTER;
	stItem.btRewardSource = GC_REWARD_CHAOS_CASTLE_UBF;
	stItem.wItemID = wItemType;

	g_GremoryCase.GDReqAddItemToGremoryCase(iUserIndex, stItem, 30);
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

