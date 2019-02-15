////////////////////////////////////////////////////////////////////////////////
// AcheronGuardianEvent.cpp
#include "stdafx.h"
#include "AcheronGuardianEvent.h"
#include "ArcaBattle.h"
#include "Logging/Log.h"
#include "GameMain.h"
#include "TNotice.h"
#include "CUserData.h"
#include "GameProtocol.h"
#include "BagManager.h"
#include "LargeRand.h"
//#include "winutil.h"
#include "MapServerManager.h"
//
#include "configread.h"

CAcheronGuardianEvent g_AcheronGuardianEvent;

CAcheronGuardianEvent::CAcheronGuardianEvent()
{
	this->m_bAEEnable = false;
	this->m_i64_REMAIN_MSEC = 0;
	this->m_i64_TICK_COUNT = 0;
	this->m_iState = 1;
	this->m_bAE_PLAY_START = FALSE;
	this->m_bAEInitState = FALSE;
	this->m_bWindowClose = false;
	this->m_iCheatAttr = 0;
}

CAcheronGuardianEvent::~CAcheronGuardianEvent()
{

}

void CAcheronGuardianEvent::Clear()
{
	for (int i = 0; i < 5; i++)
	{
		this->m_stObeliskInfo[i].Clear();
	}

	for (int nGroupNumber = 0; nGroupNumber < 5; nGroupNumber++)
	{
		this->m_stMonsterGroupInfo[nGroupNumber].m_nGenTotCnt = 0;
	}

	this->m_bAE_PLAY_START = false;
	this->m_bWindowClose = false;
	this->m_iCheatAttr = 0;
}

bool CAcheronGuardianEvent::LoadScriptAcheronEvent(char *lpszFileName)
{
	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(lpszFileName);

	if (res.status != pugi::status_ok)
	{
		sLog->outError("Error loading %s file (%s)", lpszFileName, res.description());
		return false;
	}

	pugi::xml_node mainXML_section = file.child("AcheronGuardianEvent");
	this->m_bAEEnable = g_ConfigRead.server.GetStateFromEventTable(g_ConfigRead.server.GetServerType(), EV_TABLE_ACHERON) == true ? main_section.attribute("Enable").as_bool() : false;
	this->m_iAB_PlayTimeMin = main_section.attribute("Duration").as_int();
	this->m_iAB_ChCloseMin = main_section.attribute("ChannelClose").as_int();

	pugi::xml_node activeday = main_section.child("ActiveDay");

	int iDay = 0;
	for (pugi::xml_attribute day = activeday.first_attribute(); day; day = day.next_attribute())
	{
		this->m_iDayOfWeekCycle[iDay] = day.as_int();
		iDay++;
	}

	pugi::xml_node schedule = main_section.child("Schedule");

	for (pugi::xml_node start = schedule.child("Start"); start; start = start.next_sibling())
	{
		AcheronEventTime pRET;

		pRET.m_iHour = start.attribute("Hour").as_int();
		pRET.m_iMinute = start.attribute("Minute").as_int();

		this->m_vtAcheronEventTime.push_back(pRET);
	}

	pugi::xml_node obeliskinfo = main_section.child("ObeliskInformation");

	for (pugi::xml_node obelisk = obeliskinfo.child("Obelisk"); obelisk; obelisk = obelisk.next_sibling())
	{
		int nGroupNumber = obelisk.attribute("Index").as_int() - 1;

		if (nGroupNumber < 0 || nGroupNumber > 5)
		{
			sLog->outError("[AcheronGuardian][MonsterGroupRegen] Invalid Obelisk groupNumber");
			return false;
		}

		this->m_stObeliskInfo[nGroupNumber].m_iGroupNumber = nGroupNumber;
		this->m_stObeliskInfo[nGroupNumber].m_iAttrKind = 1;
		this->m_stObeliskInfo[nGroupNumber].m_iMapIndex = obelisk.attribute("MapNumber").as_int();
		this->m_stObeliskInfo[nGroupNumber].m_iPosX = obelisk.attribute("PosX").as_int();
		this->m_stObeliskInfo[nGroupNumber].m_iPosY = obelisk.attribute("PosY").as_int();
	}

	pugi::xml_node monstergroupinfo = main_section.child("MonsterGroupInformation");

	for (pugi::xml_node monster = monstergroupinfo.child("Monster"); monster; monster = monster.next_sibling())
	{
		int nGroupNumber = monster.attribute("Group").as_int();

		if (nGroupNumber < 0 || nGroupNumber > 5)
		{
			sLog->outError("[AcheronGuardian][MonsterGroupRegen] Invalid Monster groupNumber");
			return false;
		}

		this->m_stMonsterGroupInfo[nGroupNumber].m_nGroupNumber = nGroupNumber;
		this->m_stMonsterGroupInfo[nGroupNumber].m_nMapNumber = monster.attribute("MapNumber").as_int();
		this->m_stMonsterGroupInfo[nGroupNumber].m_nCnt = monster.attribute("Count").as_int();
		this->m_stMonsterGroupInfo[nGroupNumber].m_i64_RegenTimeMin = monster.attribute("RegenTime").as_int();
	}

	int nAreaCnt = 0;
	int nTempGroupNumber = 0;
	pugi::xml_node monstergroupspawn = main_section.child("MonsterGroupSpawn");

	for (pugi::xml_node spawn = monstergroupspawn.child("Spawn"); spawn; spawn = spawn.next_sibling())
	{
		int nGroupNumber = spawn.attribute("MonsterGroup").as_int();

		if (nGroupNumber < 0 || nGroupNumber > 5)
		{
			sLog->outError("[AcheronGuardian][MonsterGroupRegen] Invalid Monster groupNumber");
			return false;
		}

		if (nTempGroupNumber != nGroupNumber)
		{
			nTempGroupNumber = nGroupNumber;
			nAreaCnt = 0;
		}

		this->m_stMonsterPosition[nGroupNumber].m_nGroupNumber = nGroupNumber;
		this->m_stMonsterPosition[nGroupNumber].m_stRegenArea[nAreaCnt].m_nBeginX = spawn.attribute("StartX").as_int();
		this->m_stMonsterPosition[nGroupNumber].m_stRegenArea[nAreaCnt].m_nBeginY = spawn.attribute("StartY").as_int();
		this->m_stMonsterPosition[nGroupNumber].m_stRegenArea[nAreaCnt].m_nEndX = spawn.attribute("EndX").as_int();
		this->m_stMonsterPosition[nGroupNumber].m_stRegenArea[nAreaCnt].m_nEndY = spawn.attribute("EndY").as_int();

		nAreaCnt++;
		this->m_stMonsterPosition[nGroupNumber].m_nRegenAreaCnt = nAreaCnt;
	}
	
	nTempGroupNumber = 0;
	int nArrayIndex = 0;
	pugi::xml_node monstergroupmember = main_section.child("MonsterGroupMember");

	for (pugi::xml_node monster = monstergroupmember.child("Monster"); monster; monster = monster.next_sibling())
	{
		int nGroupNumber = monster.attribute("Group").as_int();

		if (nGroupNumber < 0 || nGroupNumber > 5)
		{
			sLog->outError("[AcheronGuardian][MonsterGroupRegen] Invalid Monster groupNumber");
			return false;
		}

		if (nTempGroupNumber != nGroupNumber)
		{
			nTempGroupNumber = nGroupNumber;
			nArrayIndex = 0;
		}

		if (this->m_stMonsterPosition[nGroupNumber].m_nGroupNumber < 0)
		{
			sLog->outError("[AcheronGuardianEvent][MonsterGroupRegen] Invalid groupNumber(%d)", nGroupNumber);
			return false;
		}

		this->m_stMonsterPosition[nGroupNumber].m_stMonsterIndexInfo[nArrayIndex].m_nMonsterIndex = monster.attribute("Index").as_int();
		this->m_stMonsterPosition[nGroupNumber].m_stMonsterIndexInfo[nArrayIndex].m_nGenSqe = monster.attribute("SpawnSeq").as_int();
		this->m_stMonsterPosition[nGroupNumber].m_stMonsterIndexInfo[nArrayIndex].m_nMonAttr = monster.attribute("Element").as_int();

		nArrayIndex++;

		if (nArrayIndex >= 20)
		{
			sLog->outError("[AcheronGuardianEvent][MonsterGroupRegen] Out of Max MonsterKindIndex");
			return false;
		}

		this->m_stMonsterPosition[nGroupNumber].m_nMonsterKindCnt = nArrayIndex;
	}

	return true;
}

bool CAcheronGuardianEvent::CheckSync()
{
	if (g_ArcaBattle.IsArcaBattleServer() == FALSE)
	{
		return false;
	}

	SYSTEMTIME tmToDay;
	SYSTEMTIME systime;
	AcheronEventTime pRET;
	int bTIME_CHANGED = 0;
	int iMIN_HOUR;
	int iMIN_MINUTE;

	GetLocalTime(&tmToDay);

	int iTODAY_DATE_NUM = MACRO2((MACRO1(tmToDay.wDay) | MACRO1(tmToDay.wMonth) << 8) & 0xFFFF) | MACRO2(tmToDay.wYear) << 16;

	WORD wDayOfWeek = tmToDay.wDayOfWeek;

	if (wDayOfWeek < 7)
	{
		if (this->m_iDayOfWeekCycle[wDayOfWeek] == TRUE)
		{
			if (this->m_vtAcheronEventTime.empty() == true)
			{
				sLog->outError("[AcheronEvent] No Schedule Time Data - Acheron Event terminated (m_vtArcaBattleOpenTime.empty())");
				return false;
			}

			GetLocalTime(&systime);

			for (std::vector<AcheronEventTime>::iterator It = this->m_vtAcheronEventTime.begin(); It != this->m_vtAcheronEventTime.end(); It++)
			{
				pRET = *It;

				if (systime.wMinute + 60 * systime.wHour < pRET.m_iMinute + 60 * pRET.m_iHour)
				{
					bTIME_CHANGED = TRUE;
					iMIN_HOUR = pRET.m_iHour;
					iMIN_MINUTE = pRET.m_iMinute;
				}
			}

			if (!bTIME_CHANGED)
			{
				iMIN_HOUR = 24;
				iMIN_MINUTE = 60;

				for (std::vector<AcheronEventTime>::iterator It = this->m_vtAcheronEventTime.begin(); It != this->m_vtAcheronEventTime.end(); It++)
				{
					if (iMIN_MINUTE + 60 * iMIN_HOUR > It->m_iMinute + 60 * It->m_iHour)
					{
						bTIME_CHANGED = 2;
						iMIN_HOUR = It->m_iHour;
						iMIN_MINUTE = It->m_iMinute;
					}
				}
			}

			if (bTIME_CHANGED == TRUE)
			{
				this->m_i64_REMAIN_MSEC = 1000 * (60 * iMIN_MINUTE + 3600 * iMIN_HOUR - (systime.wSecond + 60 * systime.wMinute + 3600 * systime.wHour));
				this->SetProcState(2);
				this->m_i64_TICK_COUNT = GetTickCount64();

				return true;
			}

			else if (bTIME_CHANGED == 2)
			{
				this->m_i64_REMAIN_MSEC = -100;
				return false;
			}

			else
			{
				sLog->outError("[AcheronEvent] No Schedule Time Data - Acheron Event Terminated (bTIME_CHANGED=%d)", bTIME_CHANGED);
				return false;
			}
		}

		else
		{
			return false;
		}
	}

	else
	{
		sLog->outError("[AcheronEvent] Error DayOfWeek -  wDayOfWeek [%d]", wDayOfWeek);
		return false;
	}
}

void CAcheronGuardianEvent::SetProcState(int iState)
{
	this->m_iState = iState;
}

void CAcheronGuardianEvent::Init()
{
	if (g_ConfigRead.server.GetStateFromEventTable(g_ConfigRead.server.GetServerType(), EV_TABLE_ACHERON) == false)
	{
		this->m_bAEEnable = false;
	}
}

void CAcheronGuardianEvent::Run()
{
	if (g_ArcaBattle.IsArcaBattleServer() == FALSE)
	{
		return;
	}

	if (this->m_bAEEnable == false)
	{
		return;
	}

	switch (this->m_iState)
	{
		case 1:
			this->CheckSync();
			break;
		case 2:
			this->ProcStateClosed();
			break;
		case 3:
			this->ProcStateReady();
			break;
		case 4:
			this->ProcStatePlaying();
			break;
		case 5:
			this->ProcStateChannelClose();
			break;
		case 6:
			this->ProcStateEnd();
			break;
		default:
			return;
	}
}

void CAcheronGuardianEvent::ProcStateChannelClose()
{
	ULONGLONG i64TICK_MSEC = GetTickCount64() - this->m_i64_TICK_COUNT;

	if (i64TICK_MSEC >= 1000)
	{
		this->m_i64_REMAIN_MSEC -= i64TICK_MSEC;
		this->m_i64_TICK_COUNT = GetTickCount64();

		if (!this->m_bWindowClose && this->m_i64_REMAIN_MSEC <= 10000 && this->m_i64_REMAIN_MSEC > 0)
		{
			this->m_bWindowClose = true;
			PMSG_SEND_WINDOW_CLOSE pMsg;

			pMsg.h.c = 0xC1;
			pMsg.h.size = sizeof(pMsg);
			pMsg.h.headcode = 0xF8;
			pMsg.h.subcode = 0x4D;

			for (int n = g_ConfigRead.server.GetObjectStartUserIndex(); n < g_ConfigRead.server.GetObjectMax(); n++)
			{
				if (getGameObject(n)->Connected == PLAYER_PLAYING && getGameObject(n)->Type == OBJ_USER)
				{
					IOCP.DataSend(n, (BYTE*)&pMsg, pMsg.h.size);
				}
			}

			sLog->outBasic("[AcheronGuardianEvent] Send Close Window Sec[%d]", this->m_i64_REMAIN_MSEC / 1000);
		}

		if (this->m_i64_REMAIN_MSEC <= 0)
		{
			for (int n = g_ConfigRead.server.GetObjectStartUserIndex(); n < g_ConfigRead.server.GetObjectMax(); n++)
			{
				if (getGameObject(n)->Connected == PLAYER_PLAYING && getGameObject(n)->Type == OBJ_USER)
				{
					gObjMoveGate(n, 27);
				}
			}

			this->Clear();
			this->SetProcState(1);
		}
	}
}

void CAcheronGuardianEvent::ProcStateEnd()
{
	PMSG_NOTICE pNotice;
	TNotice::MakeNoticeMsg(&pNotice, 0, Lang.GetText(0,583));
	TNotice::SendNoticeToAllUser(&pNotice);

	this->DelObelisk();
	this->DeleteAcheronEventAllMonster();
	this->m_bAE_PLAY_START = false;
	this->GDReqAcheronEventProcMultiCast(this->m_bAE_PLAY_START);
	this->m_i64_REMAIN_MSEC = 60000 * this->m_iAB_ChCloseMin;
	this->m_i64_TICK_COUNT = GetTickCount64();
	this->SetProcState(5);
}

void CAcheronGuardianEvent::ProcStateClosed()
{
	if (this->m_i64_REMAIN_MSEC != -100)
	{
		ULONGLONG i64TICK_MSEC = GetTickCount64() - this->m_i64_TICK_COUNT;

		if (i64TICK_MSEC >= 1000)
		{
			this->m_i64_REMAIN_MSEC -= i64TICK_MSEC;
			this->m_i64_TICK_COUNT = GetTickCount64();

			if (this->m_i64_REMAIN_MSEC <= 0)
			{
				this->SetProcState(3);
			}
		}
	}
}

void CAcheronGuardianEvent::ProcStateReady()
{
	this->m_i64_REMAIN_MSEC = 60000 * this->m_iAB_PlayTimeMin;
	this->m_i64_TICK_COUNT = GetTickCount64();
	this->m_bAE_PLAY_START = true;
	this->GDReqAcheronEventProcMultiCast(this->m_bAE_PLAY_START);

	this->m_iGroupNum = rand() % 3;

	if (this->m_iGroupNum < 0 || this->m_iGroupNum > 2)
	{
		sLog->outBasic("[AcheronGuardianEvent] Error Obelisk GroupNum [%d]", this->m_iGroupNum);
		this->m_iGroupNum = 0;
	}

	sLog->outBasic("[AcheronGuardianEvent] Obelisk GroupNum %d", this->m_iGroupNum);

	this->SetRandomObeliskAttr();
	this->GenObelisk(this->m_iGroupNum);
	this->RegenMonster(this->m_iGroupNum, 1, 10);
	this->ResetGenTime();

	this->SendMapServerGroupMsg(Lang.GetText(0,582));
	this->SetProcState(4);
}

void CAcheronGuardianEvent::ProcStatePlaying()
{
	ULONGLONG i64TICK_MSEC = GetTickCount64() - this->m_i64_TICK_COUNT;

	if (i64TICK_MSEC >= 1000)
	{
		this->m_i64_REMAIN_MSEC -= i64TICK_MSEC;
		this->m_i64_TICK_COUNT = GetTickCount64();
		this->RegenMonsterRun();
	}

	if (this->m_i64_REMAIN_MSEC <= 0)
	{
		this->SendMapServerGroupMsg(Lang.GetText(0,586));
		this->m_i64_REMAIN_MSEC = 0;
		this->DeleteAcheronEventAllMonster();
		this->SetProcState(6);
	}
}

void CAcheronGuardianEvent::ProcStatePlayEnd()
{
	return;
}

void CAcheronGuardianEvent::GenObelisk(int iGroupNum)
{
	int result = gObjAddMonster(this->m_stObeliskInfo[iGroupNum].m_iMapIndex);

	if (result >= 0)
	{
		gObjSetMonster(result, this->m_stObeliskInfo[iGroupNum].m_iMonsterNum);

		this->m_stObeliskInfo[iGroupNum].m_iMonIndex = result;
		getGameObject(result)->Class = this->m_stObeliskInfo[iGroupNum)->m_iMonsterNum;
		getGameObject(result)->MapNumber = this->m_stObeliskInfo[iGroupNum)->m_iMapIndex;
		getGameObject(result)->X = this->m_stObeliskInfo[iGroupNum)->m_iPosX;
		getGameObject(result)->Y = this->m_stObeliskInfo[iGroupNum)->m_iPosY;
		getGameObject(result)->TX = this->m_stObeliskInfo[iGroupNum)->m_iPosX;
		getGameObject(result)->TY = this->m_stObeliskInfo[iGroupNum)->m_iPosY;
		getGameObject(result)->m_OldX = this->m_stObeliskInfo[iGroupNum)->m_iPosX;
		getGameObject(result)->m_OldY = this->m_stObeliskInfo[iGroupNum)->m_iPosY;
		getGameObject(result)->StartX = this->m_stObeliskInfo[iGroupNum)->m_iPosX;
		getGameObject(result)->StartY = this->m_stObeliskInfo[iGroupNum)->m_iPosY;
		getGameObject(result)->Dir = 1;
		getGameObject(result)->Live = TRUE;
		getGameObject(result)->DieRegen = 0;
		getGameObject(result)->m_State = 1;
		getGameObject(result)->MaxRegenTime = 0;
		sLog->outBasic("[AcheronGuardianEvent] (%d) Obelisk be created.", result);
	}
}

void CAcheronGuardianEvent::DelObelisk()
{
	for (int iCnt = 0; iCnt < 5; iCnt++)
	{
		int iIndex = this->m_stObeliskInfo[iCnt].m_iMonIndex;

		if (iIndex >= 0)
		{
			sLog->outBasic("[AcheronGuardianEvent][Obelisk] Delete Monster - %d ", iIndex);
			gObjDel(Obj.m_Index);
		}
	}
}

void CAcheronGuardianEvent::SetRandomObeliskAttr()
{
	int iArrObeliskAttr[5];
	int iArrMonsterIndex[5];

	iArrObeliskAttr[0] = 0;
	iArrObeliskAttr[1] = 0;
	iArrObeliskAttr[2] = 0;
	iArrObeliskAttr[3] = 0;
	iArrObeliskAttr[4] = 0;
	iArrMonsterIndex[0] = 627;
	iArrMonsterIndex[1] = 628;
	iArrMonsterIndex[2] = 629;
	iArrMonsterIndex[3] = 630;
	iArrMonsterIndex[4] = 631;

	int iObeliskAttrCnt = 0;
	do
	{
		int iRand = rand() % 5 + 1;
		bool bflag = false;

		for (int i = 0; i < 5; i++)
		{
			if (iArrObeliskAttr[i] == iRand)
			{
				bflag = true;
			}
		}

		if (!bflag)
		{
			iArrObeliskAttr[iObeliskAttrCnt] = iRand;
			iObeliskAttrCnt++;
		}
	} while (iObeliskAttrCnt != 5);

	for (int j = 0; j < 5; j++)
	{
		this->m_stObeliskInfo[j].m_iAttrKind = iArrObeliskAttr[j];
		this->m_stObeliskInfo[j].m_iMonsterNum = iArrMonsterIndex[iArrObeliskAttr[j] - 1];
	}
}

void CAcheronGuardianEvent::ResetGenTime()
{
	for (int nGroupNumber = 0; nGroupNumber < 5; nGroupNumber++)
	{
		if (this->m_stMonsterGroupInfo[nGroupNumber].m_nGroupNumber >= 0)
		{
			if (nGroupNumber < 0 || nGroupNumber > 4)
			{
				return;
			}

			this->m_stMonsterGroupInfo[nGroupNumber].m_i64_NextRegenTime = GetTickCount64() + 60000 * this->m_stMonsterGroupInfo[nGroupNumber].m_i64_RegenTimeMin;
		}
	}
}

void CAcheronGuardianEvent::RegenMonsterRun()
{
	ULONGLONG dwTick = GetTickCount64();

	if (this->m_stMonsterGroupInfo[this->m_iGroupNum].m_nGroupNumber > -1)
	{
		if (dwTick >= this->m_stMonsterGroupInfo[this->m_iGroupNum].m_i64_NextRegenTime)
		{
			this->DeleteMonster(this->m_iGroupNum);
			this->RegenMonster(this->m_iGroupNum, 2, 0);
			this->m_stMonsterGroupInfo[this->m_iGroupNum].m_i64_NextRegenTime = GetTickCount64() + 60000 * this->m_stMonsterGroupInfo[this->m_iGroupNum].m_i64_RegenTimeMin;
		}
	}
}

void CAcheronGuardianEvent::RegenMonster(int nGroupNumber, int iGenSeq, int iGenMonCnt)
{
	if (nGroupNumber < 0 || nGroupNumber >= 50)
	{
		return;
	}

	int nMapNumber = this->m_stMonsterGroupInfo[nGroupNumber].m_nMapNumber;
	int nMonsterArrayIndex = this->m_stMonsterGroupInfo[nGroupNumber].m_nGenTotCnt;
	int nMonsterCount = this->m_stMonsterGroupInfo[nGroupNumber].m_nCnt;
	int nMonsterKindCnt = this->m_stMonsterPosition[nGroupNumber].m_nMonsterKindCnt;

	if (iGenMonCnt)
	{
		nMonsterCount = iGenMonCnt;
	}

	for (int n = 0; n < nMonsterCount; n++)
	{
		int nArea = rand() % this->m_stMonsterPosition[nGroupNumber].m_nRegenAreaCnt;
		int nBeginX = this->m_stMonsterPosition[nGroupNumber].m_stRegenArea[nArea].m_nBeginX;
		int nBeginY = this->m_stMonsterPosition[nGroupNumber].m_stRegenArea[nArea].m_nBeginY;
		int nEndX = this->m_stMonsterPosition[nGroupNumber].m_stRegenArea[nArea].m_nEndX;
		int nEndY = this->m_stMonsterPosition[nGroupNumber].m_stRegenArea[nArea].m_nEndY;

		if (nMapNumber == -1)
		{
			break;
		}

		int nResult = gObjAddMonster(nMapNumber);
		int nMonsterIndex = 0;

		for (int i = 0; i < nMonsterKindCnt; i++)
		{
			if (this->m_stMonsterPosition[nGroupNumber].m_stMonsterIndexInfo[i].m_nGenSqe == iGenSeq
				&& this->m_stMonsterPosition[nGroupNumber].m_stMonsterIndexInfo[i].m_nMonAttr == this->m_stObeliskInfo[this->m_iGroupNum].m_iAttrKind)
			{
				nMonsterIndex = this->m_stMonsterPosition[nGroupNumber].m_stMonsterIndexInfo[i].m_nMonsterIndex;
				break;
			}
		}

		this->m_stMonsterGroupInfo[nGroupNumber].m_stMonsterAccountNumInfo[nMonsterArrayIndex].m_nIndex = nResult;

		if (nResult < 0)
		{
			sLog->outBasic("[AcheronGuardianEvent][MonsterGroupRegen] AddMonster fail!! [GROUP]:%d, [INDEX]:%d", nGroupNumber, nMonsterIndex);
			return;
		}

		if (this->SetPosMonster(nResult, nMapNumber, nBeginX, nBeginY, nEndX, nEndY) == FALSE)
		{
			gObjDel(nResult);
			sLog->outBasic("[AcheronGuardianEvent] error : %s %d", __FILE__, __LINE__);
			return;
		}

		if (gObjSetMonster(nResult, nMonsterIndex) == FALSE)
		{
			gObjDel(nResult);
			sLog->outBasic("[AcheronGuardianEvent] error : %s %d", __FILE__, __LINE__);
			return;
		}

		nMonsterArrayIndex++;
		sLog->outBasic("[AcheronGuardianEvent][MonsterGroupRegen] Group: %d, MonsterIndex: %d, MapNumber: %d, X: %d, Y: %d",
			nGroupNumber, nMonsterIndex, nMapNumber, getGameObject(nResult)->X, getGameObject(nResult)->Y);
	}

	this->m_stMonsterGroupInfo[nGroupNumber].m_nGenTotCnt = nMonsterArrayIndex;
	sLog->outBasic("[AcheronGuardianEvent][MonsterGroupRegen] Group: %d, MonsterTotCnt: %d",
		nGroupNumber, this->m_stMonsterGroupInfo[nGroupNumber].m_nGenTotCnt);
}

int CAcheronGuardianEvent::SetPosMonster(CGameObject &Obj, int nMapNumber, int nBeginX, int nBeginY, int nEndX, int nEndY)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		sLog->outBasic("error : %s %d", __FILE__, __LINE__);
		return false;
	}

	

	Obj.m_PosNum = -1;
	Obj.MapNumber = nMapNumber;
	this->GetBoxPosition(nMapNumber, nBeginX, nBeginY, nEndX, nEndY, Obj.X, Obj.Y);
	Obj.TX = Obj.X;
	Obj.TY = Obj.Y;
	Obj.m_OldX = Obj.X;
	Obj.m_OldY = Obj.Y;
	Obj.Dir = rand() % 8;
	Obj.StartX = Obj.X;
	Obj.StartY = Obj.Y;
	Obj.DieRegen = 0;
	Obj.MaxRegenTime = 0;

	return true;
}

int CAcheronGuardianEvent::GetBoxPosition(int mapnumber, int ax, int ay, int aw, int ah, short &mx, short &my)
{
	if (MAX_MAP_RANGE(mapnumber) == FALSE)
	{
		return false;
	}

	int count = 100;
	int w;
	int h;
	int tx;
	int ty;
	BYTE attr;

	while (count-- != 0)
	{
		w = aw - ax;
		h = ah - ay;

		__try
		{
			tx = ax + (rand() % w);
			ty = ay + (rand() % h);
		}
		__except (w = 1, h = 1, 1)
		{

		}

		attr = MapC[mapnumber].GetAttr(tx, ty);

		if (((attr & 1) != 1) && ((attr & 4) != 4) && ((attr & 8) != 8))
		{
			mx = tx;
			my = ty;
			return true;
		}
	}

	return false;
}

void CAcheronGuardianEvent::DeleteMonster(int nGroupNumber)
{
	if (nGroupNumber < 0 || nGroupNumber > 4)
	{
		return;
	}

	for (int i = 0; i < 50; i++)
	{
		if (this->m_stMonsterGroupInfo[nGroupNumber].m_stMonsterAccountNumInfo[i].m_nIndex > -1)
		{
			int nIndex = this->m_stMonsterGroupInfo[nGroupNumber].m_stMonsterAccountNumInfo[i].m_nIndex;

			if (getGameObject(nIndex)->Live == FALSE)
			{
				gObjDel(nIndex);
				sLog->outBasic("[AcheronGuardianEvent][MonsterGroupRegen] Delete Monster - %d ",
					this->m_stMonsterGroupInfo[nGroupNumber].m_stMonsterAccountNumInfo[i].m_nIndex);

				this->m_stMonsterGroupInfo[nGroupNumber].m_stMonsterAccountNumInfo[i].m_nIndex = -1;
			}
		}
	}
}

void CAcheronGuardianEvent::DeleteAcheronEventAllMonster()
{
	for (int nGroupNumber = 0; nGroupNumber < 5; nGroupNumber++)
	{
		if (this->m_stMonsterGroupInfo[nGroupNumber].m_nGroupNumber >= 0)
		{
			if (nGroupNumber < 0 || nGroupNumber >4)
			{
				return;
			}

			for (int i = 0; i < 50; i++)
			{
				if (this->m_stMonsterGroupInfo[nGroupNumber].m_stMonsterAccountNumInfo[i].m_nIndex > -1)
				{
					gObjDel(this->m_stMonsterGroupInfo[nGroupNumber].m_stMonsterAccountNumInfo[i].m_nIndex);
					sLog->outBasic("[AcheronGuardianEvent][MonsterGroupRegen] Delete Monster - %d ", this->m_stMonsterGroupInfo[nGroupNumber].m_stMonsterAccountNumInfo[i].m_nIndex);
					this->m_stMonsterGroupInfo[nGroupNumber].m_stMonsterAccountNumInfo[i].m_nIndex = -1;
				}
			}

			this->m_stMonsterGroupInfo[nGroupNumber].m_i64_NextRegenTime = 0;
		}
	}
}

void CAcheronGuardianEvent::DestroyObelisk(CGameObject lpObj, BYTE btMapNumber, BYTE cX, BYTE cY, int nMaxHitUser)
{
	sLog->outBasic("[AcheronGuardianEvent] Destroyed an Obelisk. [%s][%s]", Obj.AccountID, Obj.Name);

	g_BagManager.SearchAndUseBag(Obj.m_Index, BAG_EVENT, EVENTBAG_ACHERONGUARDIAN, Obj.m_Index);

	char szTemp[256];
	sprintf(szTemp, Lang.GetText(0,584), Obj.Name);

	this->SendMapServerGroupMsg(szTemp);
	this->SendMapServerGroupMsg(Lang.GetText(0,585));
	this->SetProcState(6);
}

void CAcheronGuardianEvent::CGReqAcheronEventEnter(PMSG_REQ_ACHERON_EVENT_ENTER *lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	CGameObject* lpObj = getGameObject(iIndex);

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (gObjIsConnected(Obj.m_Index) == FALSE)
	{
		return;
	}

	if (Obj.Teleport != FALSE)
	{
		return;
	}

	BYTE btAttr = MapC[lpObj->MapNumber].GetAttr(lpObj->X, lpObj->Y);

	if ((btAttr & 1) != 1)
	{
		return;
	}

	if (lpObj->m_PlayerData->m_bUsePartyMatching == true)
	{
		PMSG_ANS_PARTYMATCHING_ERROR pMsg;
		PHeadSubSetB((BYTE*)&pMsg, 0xEF, 0x09, sizeof(pMsg));
		pMsg.iResult = -1;

		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	PMSG_ANS_ACHERON_EVENT_ENTER pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xF8;
	pMsg.h.subcode = 0x4C;
	pMsg.btResult = TRUE;

	if (this->IsPlayStart() == TRUE)
	{
		sLog->outBasic("[AcheronGuardianEvent] [%s][%s] Acheron Event Enter",
			Obj.AccountID, Obj.Name);

		gObjMoveGate(*getGameObject(iIndex), 426);
	}

	else
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
	}
}

void CAcheronGuardianEvent::GDReqAcheronEventProcMultiCast(BYTE btPlay)
{
	PMSG_REQ_AE_PLAY_DS pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xF8;
	pMsg.h.subcode = 0xF0;
	pMsg.btPlay = btPlay;
	pMsg.wMapSvrNum = g_MapServerManager.GetMapSvrGroup();

	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
}

void CAcheronGuardianEvent::DGAnsAcheronEventProcMultiCast(PMSG_ANS_AE_PLAY_DS *lpMsg)
{
	if (g_ArcaBattle.IsArcaBattleServer() == FALSE)
	{
		this->SetPlayStart(lpMsg->btPlay);
	}

	sLog->outBasic("[AcheronGuardianEvent] Acheron Event Play [%d]", lpMsg->btPlay);
}

void CAcheronGuardianEvent::SetAcheronEventProcMultiCast(int bABInitState)
{
	this->m_bAEInitState = bABInitState;
	this->GDReqAcheronEventProcMultiCast(this->IsPlayStart());
}

void CAcheronGuardianEvent::SendMapServerGroupMsg(void *lpszMsg)
{
	GS_GDReqMapSvrMsgMultiCast(g_MapServerManager.GetMapSvrGroup(), (char *)lpszMsg);
}

void CAcheronGuardianEvent::SendMapServerGroupMsg(char *szMsg)
{
	GS_GDReqMapSvrMsgMultiCast(g_MapServerManager.GetMapSvrGroup(), szMsg);
}

int CAcheronGuardianEvent::IsAcheronGuardianDay()
{
	SYSTEMTIME tmToDay;
	GetLocalTime(&tmToDay);

	return this->m_iDayOfWeekCycle[tmToDay.wDayOfWeek];
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

