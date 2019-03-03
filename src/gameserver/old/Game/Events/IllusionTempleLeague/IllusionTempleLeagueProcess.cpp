////////////////////////////////////////////////////////////////////////////////
// IllusionTempleLeagueProcess.cpp
#include "StdAfx.h"
#include "IllusionTempleLeagueProcess.h"
#include "IllusionTempleLeagueEvent.h"
#include "PartyClass.h"
#include "Main.h"
#include "SendNPCInfo.h"
#include "BuffEffectSlot.h"
#include "GameProtocol.h"
#include "TNotice.h"
#include "GuildClass.h"

CIllusionTempleLeagueProcess::CIllusionTempleLeagueProcess()
{
	this->m_nTempleNumber = -1;
	this->m_btIllusionTempleLeagueState = 0;
	this->m_byITLRound = 1;
	this->m_bITLRoundStartFail = false;
	this->m_bNobodyWinLose = false;
	this->m_nNextTimeMin = 0;
	this->m_nRoundBreakCount = 0;
	this->m_bRoundBreak = false;
	this->m_byNpcCount = 0;
	this->m_bIsSetNpc = false;
	this->m_bFirstRegen_CursedStone = false;
	this->m_bFirstRegen_OccStone = false;
	InitializeCriticalSection(&this->m_critUserData);
	InitializeCriticalSection(&this->m_criGuildData);
	this->m_nRemainTimeMsec = -1;

	this->ResetITLPosNum();
}

CIllusionTempleLeagueProcess::~CIllusionTempleLeagueProcess()
{
	DeleteCriticalSection(&this->m_critUserData);
	DeleteCriticalSection(&this->m_criGuildData);
}

void CIllusionTempleLeagueProcess::ITL_ProcessInit(int nTempleNum)
{
	if (this->m_nTempleNumber == -1)
	{
		this->m_nTempleNumber = nTempleNum;
	}

	for (int nIndex = 0; nIndex < 10; nIndex++)
	{
		this->ClearUserData(nIndex);
	}

	this->m_nTickCount = GetTickCount();
	this->m_nAlliedUserCount = 0;
	this->m_nIllusionUserCount = 0;
	this->m_StatusRegenTime = -1;
	this->m_btWinTeam = -1;
	this->m_btLoseTeam = -1;
	this->m_bEntrance = false;
	this->m_nRemainTimeMsec = -1;
	this->m_bGameEndPosMove = false;
	this->m_bSendMsgBeforeReady = false;
	this->m_bSendMsgBeforePlay = false;
	this->m_nEventEndRemainTime = 0;

	if (CIllusionTempleLeagueEvent::m_bITL_Event_Enable)
	{
		this->CheckSync();
	}

	this->m_bAllCheckSync = false;
	this->m_bSendResult = false;

	EnterCriticalSection(&this->m_criGuildData);

	for (int i = 0; i < 2; i++)
	{
		this->m_GuildDataSave[i].bEntered = false;
		this->m_GuildDataSave[i].bRequest = false;
		this->m_GuildDataSave[i].byITLType = 0;
		this->m_GuildDataSave[i].nGuild = -1;

		if (this->m_GuildDataSave[i].nPartyNo > 0)
		{
			gParty.ClearPartyAuthITL(this->m_GuildDataSave[i].nPartyNo);
			this->m_GuildDataSave[i].nPartyNo = -1;
		}

		this->m_GuildDataSave[i].bDBSave = false;
		this->m_GuildDataSave[i].byTeamIndex = -1;
		this->m_GuildDataSave[i].byOccupiedCount = 0;
		this->m_GuildDataSave[i].byEnterCount = 0;
		this->m_GuildDataSave[i].nRequestUserIndex = -1;
		memset(this->m_GuildDataSave[i].szGName, 0, MAX_GUILD_LEN + 1);
	}

	LeaveCriticalSection(&this->m_criGuildData);
	this->DeleteAllName();

	if (nTempleNum == 0 && g_IllusionTempleLeagueEvent.m_bGuildRankGetDone == true)
	{
		g_IllusionTempleLeagueEvent.m_bGuildRankGetDone = false;
		g_IllusionTempleLeagueEvent.m_bGuildRankRenew = false;
	}

	this->m_bNobodyWinLose = false;
	this->m_byITLRound = 1;
	this->m_byIllusionTeamPoint = 0;
	this->m_byAlliedTeamPoint = 0;
}

void CIllusionTempleLeagueProcess::ClearUserData(int nIndex)
{
	if (!ObjectMaxRange(nIndex))
	{
		sLog->outBasic("return %s %d", __FILE__, __LINE__);
		return;
	}

	this->m_dwViewSkillState[nIndex] = 0;
	this->m_UserData[nIndex].m_dwSkillProdectionTime = 0;
	this->m_UserData[nIndex].m_dwSkillRestraintTime = 0;
	this->m_UserData[nIndex].m_nIndex = -1;
	this->m_UserData[nIndex].m_btTeam = -1;
	this->m_UserData[nIndex].m_btUserKillCount = 0;
	this->m_UserData[nIndex].m_bReward = 0;
	this->m_UserData[nIndex].m_bEnterOk = 0;
	this->m_UserData[nIndex].m_nGuildIdx = -1;
	this->m_UserData[nIndex].m_nPartyIdx = -1;
	this->m_UserData[nIndex].m_nRelicsInvenPos = -1;
	this->m_UserData[nIndex].m_byInvalidationCount = 0;
	this->m_UserData[nIndex].m_byOccupiedCount = 0;
}

void CIllusionTempleLeagueProcess::CheckSync()
{
	std::list<__ITL_TIME_TABLE>::iterator iter;

	__ITL_TIME_TABLE prevStartTime;
	__ITL_TIME_TABLE nextStartTime;

	DWORD dwPrevTime = 0;
	DWORD dwNextTime = 0;
	DWORD dwCurTime = 0;

	if (this->m_lst_ITL_Today_StartTime.size() == 0)
	{
		if (g_IllusionTempleLeagueEvent.m_nITL_TYPE == 1 ||
			g_IllusionTempleLeagueEvent.m_nITL_TYPE == 2 ||
			g_IllusionTempleLeagueEvent.m_nITL_TYPE == 3)
		{
			sLog->outError("Error : IllusionTempleLeague Day But StartTime size is 0");
			return;
		}
	}

	if (g_IllusionTempleLeagueEvent.m_nITL_TYPE == 0 ||
		g_IllusionTempleLeagueEvent.m_nITL_TYPE == 4)
	{
		return;
	}


	tm* today;
	time_t ltime;

	time(&ltime);
	today = localtime(&ltime);

	dwCurTime = (today->tm_hour * 60) + today->tm_min;
	prevStartTime = *this->m_lst_ITL_Today_StartTime.begin();

	for (iter = this->m_lst_ITL_Today_StartTime.begin(); iter != this->m_lst_ITL_Today_StartTime.end(); ++iter)
	{
		nextStartTime = *iter;

		dwPrevTime = (prevStartTime.nHour * 60) + prevStartTime.nMin;
		dwNextTime = (nextStartTime.nHour * 60) + nextStartTime.nMin;

		if (dwPrevTime == dwNextTime)
		{
			if (dwCurTime < dwNextTime)
			{
				nextStartTime = *iter;
				break;
			}
			continue;
		}

		if (dwCurTime >= dwPrevTime &&
			dwCurTime < dwNextTime)
		{
			break;
		}

		prevStartTime = *iter;
	}

	int Cnt = 2;

	while (Cnt-- != 0)
	{
		if (iter == this->m_lst_ITL_Today_StartTime.end())
		{
			iter = this->m_lst_ITL_Today_StartTime.begin();

			nextStartTime = (*iter);
		}

		dwNextTime = nextStartTime.nHour * 60 + nextStartTime.nMin;

		if (today->tm_hour <= nextStartTime.nHour)
		{
			m_nRemainTimeMin = dwNextTime - dwCurTime;
		}
		else
		{
			m_nRemainTimeMin = ((24 * 60) - dwCurTime) + dwNextTime;
		}

		if (m_nRemainTimeMin <= CIllusionTempleLeagueEvent::m_nITLOpenTime)
		{
			iter++;

			if (iter != this->m_lst_ITL_Today_StartTime.end())
			{
				nextStartTime = (*iter);
			}
		}
		else
		{
			break;
		}
	}

	int tmpTime = GetTickCount();
	this->m_nNextTimeMin = tmpTime + ((1000 * 60) - (today->tm_sec * 1000));
	
	sLog->outBasic("[ ITL ] TYPE:%d , (%d) ITL Sync Open Time. [%d]min remain, m_nRemainTimeMsec:[%d]",
		g_IllusionTempleLeagueEvent.m_nITL_TYPE, this->m_nTempleNumber + 1, this->m_nRemainTimeMin, this->m_nRemainTimeMsec);

}

void CIllusionTempleLeagueProcess::Proc_ITL(int nCurTime)
{
	switch (this->m_btIllusionTempleLeagueState)
	{
	case 0:
		this->Proc_ITLState_None(nCurTime);
		break;
	case 1:
		this->Proc_ITLState_Ready(nCurTime);
		break;
	case 2:
		this->Proc_ITLState_Playing(nCurTime);
		break;
	case 3:
		this->Proc_ITLState_End(nCurTime);
		break;
	case 4:
		this->Proc_ITLState_RoundBreak(nCurTime);
		break;
	}
}

void CIllusionTempleLeagueProcess::Proc_ITLState_RoundBreak(int nCurTime)
{
	int iTICK_MSEC = nCurTime - this->m_nTickCount;

	if (iTICK_MSEC >= 1000)
	{
		this->m_nRemainTimeMsec -= iTICK_MSEC;
		this->m_nTickCount = nCurTime;
		this->m_nRoundBreakCount++;

		if (this->m_nRoundBreakCount == 5)
		{
			this->m_byITLRound++;
			this->Set_ITLState(2);
			this->m_nRoundBreakCount = 0;
			this->m_bRoundBreak = false;
		}

		this->Send_ITL_PlayState(0);
		this->Send_ITL_PlayState(1);
	}

	if (!this->m_nUserCount)
	{
		this->Set_ITLState(3);
	}
}

void CIllusionTempleLeagueProcess::Proc_ITLState_None(int nCurTime)
{
	int iTICK_MSEC = nCurTime - this->m_nTickCount;

	if (g_IllusionTempleLeagueEvent.WhatIsTodayTypeOfITL() == 1 ||
		g_IllusionTempleLeagueEvent.WhatIsTodayTypeOfITL() == 2 ||
		g_IllusionTempleLeagueEvent.WhatIsTodayTypeOfITL() == 3)
	{
		if (this->m_nNextTimeMin < nCurTime)
		{
			sLog->outBasic(" [ ITL ] m_nNextTimeMin %d, nCurTime %d", this->m_nNextTimeMin, nCurTime);
			this->m_nRemainTimeMin--;
			this->m_nNextTimeMin = nCurTime + 60000;

			if (this->m_nRemainTimeMin <= CIllusionTempleLeagueEvent::m_nITLOpenTime)
			{
				if (this->m_nRemainTimeMin > 0 && this->m_nTempleNumber == 0)
				{
					char szTemp[256];
					sprintf(szTemp, Lang.GetText(0,494), this->m_nRemainTimeMin);

					this->SendAllUserAnyMsg(szTemp);
				}
			}
		}

		if (iTICK_MSEC >= 1000)
		{
			if (this->m_nRemainTimeMsec > 0)
			{
				this->m_nRemainTimeMsec -= iTICK_MSEC;
			}

			this->m_nTickCount = nCurTime;

			if (this->m_nRemainTimeMin <= CIllusionTempleLeagueEvent::m_nITLOpenTime + 1 && this->m_nRemainTimeMsec == -1)
			{
				this->m_nRemainTimeMsec = 60000 * (CIllusionTempleLeagueEvent::m_nITLOpenTime + 1);
			}

			if (this->m_nRemainTimeMin <= CIllusionTempleLeagueEvent::m_nITLOpenTime && this->m_nRemainTimeMsec > 5000 && this->m_bEntrance == false)
			{
				this->m_bEntrance = true;
			}

			if (this->m_nRemainTimeMsec <= 30000 && this->m_nRemainTimeMsec > 0 && this->m_nTempleNumber == 0 && this->m_bSendMsgBeforeReady == false)
			{
				this->m_bSendMsgBeforeReady = true;

				PMSG_SET_DEVILSQUARE pMsg;
				PHeadSetB((BYTE*)&pMsg, 0x92, sizeof(pMsg));

				pMsg.Type = 24;

				for (int n = g_ConfigRead.server.GetObjectStartUserIndex(); n < g_ConfigRead.server.GetObjectMax(); n++)
				{
					if (getGameObject(n)->Connected == PLAYER_PLAYING && getGameObject(n)->Type == OBJ_USER)
					{
						if (getGameObject(n)->MapNumber == MAP_INDEX_LORENMARKET)
						{
							IOCP.DataSend(n, (BYTE*)&pMsg, pMsg.h.size);
						}
					}
				}
			}

			if (this->m_nRemainTimeMsec <= 5000 && this->m_bEntrance == true)
			{
				this->m_bEntrance = false;

				if (this->m_nTempleNumber == 0)
				{
					char szTemp[256];
					sprintf(szTemp, Lang.GetText(0,495), this->m_nRemainTimeMin);

					this->SendAllUserAnyMsg(szTemp);
				}
			}
		}

		if (this->m_nRemainTimeMin <= 0)
		{
			this->m_bEntrance = false;
			this->CheckUsersOnConnect();

			BYTE byAllied = 0;
			BYTE byIllusion = 0;
			bool bCanStart = this->CanStartITLBattle(byAllied, byIllusion);

			if (this->m_nUserCount >= 2 && bCanStart == true)
			{
				for (int nG = 0; nG < 2; nG++)
				{
					this->m_GuildDataSave[nG].byTeamIndex = nG;
				}

				for (int i = 0; i < 10; i++)
				{
					if (this->m_UserData[i].m_nIndex != -1 && getGameObject(this->m_UserData[i]->m_nIndex)->m_nITLIndex == i)
					{
						if (this->m_UserData[i].m_btTeam == 1)
						{
							g_SendNPCInfo.SendITLNpcCoordinate(this->m_UserData[i].m_nIndex, this->m_nTempleNumber + 98);
							gObjMoveGate(this->m_UserData[i].m_nIndex, this->m_nTempleNumber + 448);
							getGameObject(this->m_UserData[i]->m_nIndex)->m_Change = 405;
							gObjViewportListProtocolCreate(getGameObject(this->m_UserData[i]->m_nIndex));
							this->m_nIllusionUserCount++;
						}

						else if (this->m_UserData[i].m_btTeam == 0)
						{
							g_SendNPCInfo.SendITLNpcCoordinate(this->m_UserData[i].m_nIndex, this->m_nTempleNumber + 98);
							gObjMoveGate(this->m_UserData[i].m_nIndex, this->m_nTempleNumber + 446);
							getGameObject(this->m_UserData[i]->m_nIndex)->m_Change = 404;
							gObjViewportListProtocolCreate(getGameObject(this->m_UserData[i]->m_nIndex));
							this->m_nAlliedUserCount++;
						}
					}
				}

				this->Set_ITLState(1);
			}

			else
			{
				BYTE byWinTeamIndex = 0xFF;

				if (byAllied == 0 || byIllusion == 0)
				{
					if (g_IllusionTempleLeagueEvent.WhatIsTodayTypeOfITL() == 2 ||
						g_IllusionTempleLeagueEvent.WhatIsTodayTypeOfITL() == 3)
					{
						if (byAllied && !byIllusion)
						{
							byWinTeamIndex = 0;
						}

						else if (!byAllied && byIllusion)
						{
							byWinTeamIndex = 1;
						}

						sLog->outBasic("[ ITL ] FINAL/SEMIFINAL START FAIL BattleType:[%d],ALLIED:ILLUSION == [%d VS %d],WINTEAM : [%d]",
							g_IllusionTempleLeagueEvent.WhatIsTodayTypeOfITL(), byAllied, byIllusion, byWinTeamIndex);
					}

					if ((byWinTeamIndex == 0 || byWinTeamIndex == 1) && this->m_GuildDataSave[byWinTeamIndex].bDBSave == false)
					{
						this->GDSaveITLGuildPoint(
							this->m_GuildDataSave[byWinTeamIndex].szGName,
							this->m_GuildDataSave[byWinTeamIndex].byITLType,
							1,
							0,
							this->m_GuildDataSave[byWinTeamIndex].byOccupiedCount,
							1);

						sLog->outBasic("[ ITL ][GDSaveITLGuildPoint][START_FAIL_SAVE] G.NAME :%s ",
							this->m_GuildDataSave[byWinTeamIndex].szGName);

						EnterCriticalSection(&this->m_criGuildData);
						this->m_GuildDataSave[byWinTeamIndex].bDBSave = true;
						LeaveCriticalSection(&this->m_criGuildData);
					}
				}

				for (int nIndex = 0; nIndex < 10; nIndex++)
				{
					CGameObject &Obj = this->m_UserData[nIndex].m_nIndex;

					if (this->m_UserData[nIndex].m_nIndex != -1 && getGameObject(this->m_UserData[nIndex]->m_nIndex]->Connected > PLAYER_LOGGED && getGameObject(this->m_UserData[nIndex)->m_nIndex)->MapNumber == this->m_nTempleNumber + 98)
					{
						if (byWinTeamIndex == 0 || byWinTeamIndex == 1)
						{
							if (g_IllusionTempleLeagueEvent.m_nITL_TYPE == 1)
							{
								getGameObject(this->m_UserData[nIndex]->m_nIndex)->m_byEnterITLUserCount++;
							}

							this->GDSaveITLUserPoint(
								nIndex,
								getGameObject(this->m_UserData[nIndex]->m_nIndex)->Name,
								getGameObject(this->m_UserData[nIndex]->m_nIndex)->m_PlayerData->GuildName,
								0,
								0,
								0,
								g_IllusionTempleLeagueEvent.m_nITL_TYPE,
								1,
								byWinTeamIndex);

							sLog->outBasic("[ ITL ][GDSaveITLUserPoint][START_FAIL_SAVE] NAME :%s G.NAME:%s ",
								getGameObject(this->m_UserData[nIndex]->m_nIndex]->Name, getGameObject(this->m_UserData[nIndex)->m_nIndex)->m_PlayerData->GuildName);
						}

						Obj.m_nITLIndex = -1;
						Obj.m_bITL_ScheduleInfoSend = false;
						Obj.m_bITL_GuildRankInfoSend = false;
						Obj.m_bITL_TournamentInfoSend = false;;
						Obj.m_byEnterITLCount = 0;
						Obj.m_bEnterCountLoad = false;

						gObjMoveGate(Obj.m_Index, 333);
						this->ClearUserData(nIndex);
					}
				}

				sLog->outBasic("[ ITL ] (%d) Failed to Start ITL (UserCount: %d) (m_nRemainTimeMin:%d)(m_nRemainTimeMSec:%d),AlliedCnt:[%d],IllusionCnt:[%d]",
					this->m_nTempleNumber, this->m_nUserCount, this->m_nRemainTimeMin, this->m_nRemainTimeMsec, this->m_nAlliedUserCount, this->m_nIllusionUserCount);

				this->Set_ITLState(3);

				this->m_bITLRoundStartFail = true;

				if (g_IllusionTempleLeagueEvent.m_nITL_TYPE == 3 && this->m_nTempleNumber == 0)
				{
					CIllusionTempleLeagueEvent::m_byITLFinalStart = 2;
				}
			}
		}
	}
}

void CIllusionTempleLeagueProcess::Proc_ITLState_Ready(int nCurTime)
{
	int iTICK_MSEC = nCurTime - this->m_nTickCount;

	if (iTICK_MSEC >= 1000)
	{
		this->m_nRemainTimeMsec -= iTICK_MSEC;
		this->m_nTickCount = nCurTime;

		if (this->m_nRemainTimeMsec <= 30000 && this->m_nRemainTimeMsec > 0 && this->m_bSendMsgBeforePlay == false)
		{
			this->m_bSendMsgBeforePlay = true;

			PMSG_SET_DEVILSQUARE pMsg;
			PHeadSetB((BYTE*)&pMsg, 0x92, sizeof(pMsg));

			pMsg.Type = 25;

			for (int i = 0; i < 10; i++)
			{
				if (this->m_UserData[i].m_nIndex != -1 && getGameObject(this->m_UserData[i]->m_nIndex)->Connected > PLAYER_LOGGED)
				{
					if (getGameObject(this->m_UserData[i]->m_nIndex)->MapNumber == this->m_nTempleNumber + 98)
					{
						IOCP.DataSend(this->m_UserData[i].m_nIndex, (BYTE*)&pMsg, pMsg.h.size);
					}
				}
			}
		}
	}

	if (this->m_nUserCount > 0)
	{
		if (this->m_nRemainTimeMsec <= 0)
		{
			this->Set_ITLState(2);
			this->Send_ITL_PlayState(0);
			this->Send_ITL_PlayState(1);
		}
	}

	else
	{
		this->Set_ITLState(3);
	}
}

void CIllusionTempleLeagueProcess::Proc_ITLState_Playing(int nCurTime)
{
	int iTICK_MSEC = nCurTime - this->m_nTickCount;
	this->m_bRoundBreak = false;

	if (iTICK_MSEC >= 1000)
	{
		this->m_nRemainTimeMsec -= iTICK_MSEC;
		this->m_nTickCount = nCurTime;

		if (this->m_nRemainTimeMsec > 0)
		{
			this->Send_ITL_PlayState(0);
			this->Send_ITL_PlayState(1);
		}
	}

	if (this->m_nUserCount > 0)
	{
		if (this->m_nRemainTimeMsec <= 0 || this->m_nAlliedUserCount == 0 || this->m_nIllusionUserCount == 0)
		{
			if (this->m_nRemainTimeMsec > 0)
			{
				this->m_nEventEndRemainTime = this->m_nRemainTimeMsec / 1000;
			}

			else
			{
				this->m_nEventEndRemainTime = 0;
			}

			this->Send_ITL_PlayState(0);
			this->Send_ITL_PlayState(1);
			this->Set_ITLState(3);
		}
	}

	else
	{
		this->Set_ITLState(3);
	}
}

void CIllusionTempleLeagueProcess::Proc_ITLState_End(int nCurTime)
{
	int iTICK_MSEC = nCurTime - this->m_nTickCount;

	if (iTICK_MSEC >= 1000)
	{
		this->m_nRemainTimeMsec -= iTICK_MSEC;
		this->m_nTickCount = nCurTime;

		if (this->m_nRemainTimeMsec <= 60000 * CIllusionTempleLeagueEvent::m_nITLEndTime - 3000)
		{
			if (this->m_nRemainTimeMsec > 0 && this->m_bGameEndPosMove == false)
			{
				this->m_bGameEndPosMove = true;

				for (int i = 0; i < 10; i++)
				{
					if (this->m_UserData[i].m_nIndex != -1 && getGameObject(this->m_UserData[i]->m_nIndex]->Connected > PLAYER_LOGGED && getGameObject(this->m_UserData[i)->m_nIndex)->MapNumber == this->m_nTempleNumber + 98)
					{
						if (this->m_UserData[i].m_btTeam == 1)
						{
							gObjMoveGate(this->m_UserData[i].m_nIndex, this->m_nTempleNumber + 448);
						}

						else if (this->m_UserData[i].m_btTeam == 0)
						{
							gObjMoveGate(this->m_UserData[i].m_nIndex, this->m_nTempleNumber + 446);
						}

						gObjSetPosition(this->m_UserData[i].m_nIndex, getGameObject(this->m_UserData[i]->m_nIndex]->X, getGameObject(this->m_UserData[i)->m_nIndex)->Y);
					}

					if (this->m_UserData[i].m_nRelicsInvenPos > 0)
					{
						if (getGameObject(this->m_UserData[i]->m_nIndex]->pntInventory[this->m_UserData[i]->m_nRelicsInvenPos)->m_Type == ITEMGET(14, 223))
						{
							gObjInventoryDeleteItem(this->m_UserData[i].m_nIndex, this->m_UserData[i].m_nRelicsInvenPos);
							GSProtocol.GCInventoryItemDeleteSend(this->m_UserData[i].m_nIndex, this->m_UserData[i].m_nRelicsInvenPos, 0);
							this->m_UserData[i].m_nRelicsInvenPos = -1;
						}
					}
				}

				if (this->m_bITLRoundStartFail == false)
				{
					this->CalCursetTempleResult();
				}
			}
		}
	}

	if (this->m_nRemainTimeMsec <= 60000 * CIllusionTempleLeagueEvent::m_nITLEndTime - 6000 && this->m_nRemainTimeMsec > 0 && this->m_bGameEndPosMove == true && this->m_bSendResult == false)
	{
		if (this->m_bITLRoundStartFail == false)
		{
			this->SendITLResult();
		}

		this->m_bSendResult = true;

		if (g_IllusionTempleLeagueEvent.WhatIsTodayTypeOfITL() == 2)
		{
			if (this->m_nTempleNumber == 0)
			{
				CIllusionTempleLeagueEvent::m_bITL_SemiFinalEnd_0 = true;
			}

			else if (this->m_nTempleNumber == 1)
			{
				CIllusionTempleLeagueEvent::m_bITL_SemiFinalEnd_1 = true;
			}
		}
	}

	if (this->m_nRemainTimeMsec <= 0)
	{
		this->Set_ITLState(0);

		if (g_IllusionTempleLeagueEvent.WhatIsTodayTypeOfITL() == 2 &&
			CIllusionTempleLeagueEvent::m_bITL_SemiFinalEnd_0 == true &&
			CIllusionTempleLeagueEvent::m_bITL_SemiFinalEnd_1 == true)
		{
			CIllusionTempleLeagueEvent::m_bITL_SemiFinalEnd = true;
		}

		if (g_IllusionTempleLeagueEvent.WhatIsTodayTypeOfITL() == 3 &&
			this->m_nTempleNumber == 0 &&
			CIllusionTempleLeagueEvent::m_byITLFinalStart)
		{
			CIllusionTempleLeagueEvent::m_bITL_FinalEnd = true;
			CIllusionTempleLeagueEvent::m_byITLFinalStart = 0;
		}
	}
}

void CIllusionTempleLeagueProcess::Set_ITLState(BYTE btState)
{
	this->m_btIllusionTempleLeagueState = btState;

	switch (btState)
	{
	case 0:
		this->Set_ITLState_None();
		break;
	case 1:
		this->Set_ITLState_Ready();
		break;
	case 2:
		this->Set_ITLState_Playing();
		break;
	case 3:
		this->Set_ITLState_End();
		break;
	case 4:
		this->Set_ITLState_RoundBreak();
		break;
	}
}

BYTE CIllusionTempleLeagueProcess::GetIllusionTempleLeagueState()
{
	return this->m_btIllusionTempleLeagueState;
}

void CIllusionTempleLeagueProcess::Set_ITLState_None()
{
	this->SendIllusionTempleLeagueState(0, -1);

	for (int i = 0; i < 10; i++)
	{
		if (this->m_UserData[i].m_nIndex != -1 && getGameObject(this->m_UserData[i]->m_nIndex)->Connected > PLAYER_LOGGED)
		{
			getGameObject(this->m_UserData[i]->m_nIndex)->m_nITLIndex = -1;
			getGameObject(this->m_UserData[i]->m_nIndex)->m_bITL_ScheduleInfoSend = false;
			getGameObject(this->m_UserData[i]->m_nIndex)->m_bITL_GuildRankInfoSend = false;
			getGameObject(this->m_UserData[i]->m_nIndex)->m_bITL_TournamentInfoSend = false;
			getGameObject(this->m_UserData[i]->m_nIndex)->m_nITLRelicsTick = 0;
			getGameObject(this->m_UserData[i]->m_nIndex)->m_bGettingRelics = 0;
			getGameObject(this->m_UserData[i]->m_nIndex)->m_bRegisteringRelics = 0;
			getGameObject(this->m_UserData[i]->m_nIndex)->m_wITLNpcType = 0;
			getGameObject(this->m_UserData[i]->m_nIndex)->m_byStoneState = 99;
			getGameObject(this->m_UserData[i]->m_nIndex)->m_byEnterITLCount = 0;
			getGameObject(this->m_UserData[i]->m_nIndex)->m_bEnterCountLoad = false;

			gObjMoveGate(this->m_UserData[i].m_nIndex, 333);
			getGameObject(this->m_UserData[i]->m_nIndex)->m_Change = -1;
			gObjViewportListProtocolCreate(getGameObject(this->m_UserData[i]->m_nIndex));

		}
	}

	this->ITL_ProcessInit(this->m_nTempleNumber);
}

void CIllusionTempleLeagueProcess::Set_ITLState_Ready()
{
	this->m_nRemainTimeMsec = 60000 * CIllusionTempleLeagueEvent::m_nITLReadyTime;
	this->m_bITLRoundStartFail = false;
	this->SendIllusionTempleLeagueState(1, -1);
}

void CIllusionTempleLeagueProcess::Set_ITLState_RoundBreak()
{
	if (this->m_bRoundBreak == false)
	{
		this->MoveToStartPoint_WhenRoundBreak();
		this->DeleteAllRelicsItem();
		this->RegenCursedStoneStatus(this->m_byITLRound);
		this->RegenOccupiedStoneStatus();
		this->m_bRoundBreak = true;
	}

	this->SendIllusionTempleLeagueState(4, -1);
}

void CIllusionTempleLeagueProcess::Set_ITLState_Playing()
{
	if (this->m_byITLRound == 1)
	{
		this->m_nRemainTimeMsec = 60000 * CIllusionTempleLeagueEvent::m_nITLPlayTime;

		for (int i = 0; i < 10; i++)
		{
			if (this->m_UserData[i].m_nIndex != -1 && getGameObject(this->m_UserData[i]->m_nIndex)->Connected > PLAYER_LOGGED)
			{
				if (getGameObject(this->m_UserData[i]->m_nIndex)->MapNumber == this->m_nTempleNumber + 98)
				{
					sLog->outBasic("[ ITL ] MAP:[%d], ACC:[%s], NAME:[%s], TEAM:[%d], CLASS:[%d]",
						this->m_nTempleNumber, getGameObject(this->m_UserData[i]->m_nIndex]->AccountID, getGameObject(this->m_UserData[i)->m_nIndex)->Name,
						this->m_UserData[i].m_btTeam, getGameObject(this->m_UserData[i]->m_nIndex)->m_PlayerData->DbClass);
				}
			}
		}

		if (this->m_nTempleNumber == 0 && g_IllusionTempleLeagueEvent.m_nITL_TYPE == 3)
		{
			CIllusionTempleLeagueEvent::m_byITLFinalStart = TRUE;
		}
	}

	if (this->m_bIsSetNpc == false)
	{
		this->SetNpc();
	}

	if (this->m_bFirstRegen_CursedStone == false)
	{
		this->FirstRegen_CursedStone();
		sLog->outBasic("[ ITL ][FirstRegen_CursedStone]");
	}

	if (this->m_bFirstRegen_OccStone == false)
	{
		this->FirstRegen_OccupiedStone();
		sLog->outBasic("[ ITL ][FirstRegen_OccupiedStone]");
	}

	this->SetStatusRegenTime();
	this->SendIllusionTempleLeagueState(2, -1);
}

void CIllusionTempleLeagueProcess::Set_ITLState_End()
{
	this->m_nRemainTimeMsec = 60000 * CIllusionTempleLeagueEvent::m_nITLEndTime;
	this->SendIllusionTempleLeagueState(3, -1);

	for (int i = 0; i < 10; i++)
	{
		if (this->m_UserData[i].m_nIndex != -1 && getGameObject(this->m_UserData[i]->m_nIndex]->Connected > PLAYER_LOGGED && getGameObject(this->m_UserData[i)->m_nIndex)->MapNumber == this->m_nTempleNumber + 98)
		{
			if (this->m_UserData[i].m_dwSkillProdectionTime != 0)
			{
				this->m_UserData[i].m_dwSkillProdectionTime = 0;
				gObjRemoveBuffEffect(getGameObject(this->m_UserData[i]->m_nIndex), BUFFTYPE_TEMPLE_PROTECTION);
				this->Send_ITL_SkillEnd(getGameObject(this->m_UserData[i]->m_nIndex), 618);
			}

			if (this->m_UserData[i].m_dwSkillRestraintTime != 0)
			{
				this->m_UserData[i].m_dwSkillRestraintTime = 0;
				gObjRemoveBuffEffect(getGameObject(this->m_UserData[i]->m_nIndex), BUFFTYPE_RESTRICTION);
				this->Send_ITL_SkillEnd(getGameObject(this->m_UserData[i]->m_nIndex), 619);
			}
		}
	}

	this->GetOccupiedStoneCount(this->m_byAlliedTeamPoint, this->m_byIllusionTeamPoint);
	sLog->outBasic("[ ITL ][WarResult] Allied Team : %d , IllusionTeam :%d", this->m_byAlliedTeamPoint, this->m_byIllusionTeamPoint);

	this->RemoveAllObj();
	this->m_bIsSetNpc = false;
	this->m_bFirstRegen_CursedStone = false;
	this->m_bFirstRegen_OccStone = false;
}

BOOL CIllusionTempleLeagueProcess::EnterUserIllusionTempleLeague(CGameObject &Obj, BYTE btTempleIndex, int nGuildIdx, int nPartyIdx)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		sLog->outBasic("return %s %d", __FILE__, __LINE__);
		return FALSE;
	}

	if (this->GetIllusionTempleLeagueState() >= 1)
	{
		return FALSE;
	}

	if (Obj.m_nITLIndex != -1)
	{
		sLog->outBasic("[ ITL ] Enter Error: IllusionTempleIndex Fail %d", Obj.m_nITLIndex);
		return FALSE;
	}

	int nItemPos = this->SearchRelicsItem(Obj.m_Index);

	if (nItemPos >= 0 && Obj.pntInventory[nItemPos]->m_Type == ITEMGET(14, 223))
	{
		gObjInventoryDeleteItem(Obj.m_Index, nItemPos);
		GSProtocol.GCInventoryItemDeleteSend(Obj.m_Index, nItemPos, 0);
	}

	BOOL bResult = FALSE;
	int ArrayIndex = -1;
	EnterCriticalSection(&this->m_critUserData);

	for (int i = 0; i < 10; i++)
	{
		if (this->m_UserData[i].m_nIndex == -1)
		{
			this->m_UserData[i].m_nIndex = Obj.m_Index;
			this->m_UserData[i].m_nPartyIdx = nPartyIdx;
			this->m_UserData[i].m_nGuildIdx = nGuildIdx;
			this->m_nUserCount++;
			Obj.m_nITLIndex = i;
			this->m_UserData[i].m_bEnterOk = true;
			bResult = TRUE;
			ArrayIndex = i;

			if (this->m_GuildDataSave[0].nGuild == getGameObject(this->m_UserData[i]->m_nIndex)->m_PlayerData->GuildNumber)
			{
				this->m_UserData[i].m_btTeam = 0;
			}

			else if (this->m_GuildDataSave[1].nGuild == getGameObject(this->m_UserData[i]->m_nIndex)->m_PlayerData->GuildNumber)
			{
				this->m_UserData[i].m_btTeam = 1;
			}

			break;
		}
	}

	LeaveCriticalSection(&this->m_critUserData);

	if (ArrayIndex != -1)
	{
		if (ArrayIndex < 10)
		{
			this->m_EnteredPlayerName[ArrayIndex].EnteredIndex = ArrayIndex;
			std::memcpy(this->m_EnteredPlayerName[ArrayIndex].PlayerName, Obj.Name, MAX_ACCOUNT_LEN + 1);
		}
	}
	return bResult;
}

BOOL CIllusionTempleLeagueProcess::LeaveUserIllusionTempleLeague(CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		sLog->outBasic("return %s %d", __FILE__, __LINE__);
		return FALSE;
	}

	if (Obj.m_nITLIndex != -1 && this->m_UserData[Obj.m_nITLIndex].m_nIndex == Obj.m_Index && gObjIsConnected(Obj.m_Index))
	{
		if (this->m_UserData[Obj.m_nITLIndex].m_dwSkillProdectionTime != 0)
		{
			gObjRemoveBuffEffect(getGameObject(Obj.m_Index), BUFFTYPE_TEMPLE_PROTECTION);
			this->Send_ITL_SkillEnd(getGameObject(Obj.m_Index), 618);
		}

		if (this->m_UserData[Obj.m_nITLIndex].m_dwSkillRestraintTime != 0)
		{
			gObjRemoveBuffEffect(getGameObject(Obj.m_Index), BUFFTYPE_RESTRICTION);
			this->Send_ITL_SkillEnd(getGameObject(Obj.m_Index), 619);
		}

		EnterCriticalSection(&this->m_critUserData);

		if (this->m_UserData[Obj.m_nITLIndex].m_btTeam == 0)
		{
			this->m_nAlliedUserCount--;
		}

		else if (this->m_UserData[Obj.m_nITLIndex].m_btTeam == 1)
		{
			this->m_nIllusionUserCount--;
		}

		this->ClearUserData(Obj.m_nITLIndex);
		Obj.m_nITLIndex = -1;
		Obj.m_wITLNpcType = 0;
		Obj.m_byStoneState = 99;
		this->m_nUserCount--;

		LeaveCriticalSection(&this->m_critUserData);
		sLog->outBasic("[ ITL ] (%d) LeaveUser: (%s)(%s)", this->m_nTempleNumber + 1, Obj.AccountID, Obj.Name);
	}

	Obj.m_Change = -1;
	gObjViewportListProtocolCreate(getGameObject(Obj.m_Index));
	Obj.m_nITLRelicsTick = 0;
	Obj.m_byStoneState = 99;
	Obj.m_wITLNpcType = 0;
	Obj.m_bGettingRelics = false;
	Obj.m_bRegisteringRelics = false;
	Obj.m_nITLIndex = -1;
	Obj.m_bITL_ScheduleInfoSend = false;
	Obj.m_bITL_GuildRankInfoSend = false;
	Obj.m_bITL_TournamentInfoSend = false;
	Obj.m_byEnterITLCount = 0;
	Obj.m_bEnterCountLoad = false;

	return TRUE;
}

bool CIllusionTempleLeagueProcess::CanStartITLBattle(BYTE &ALLIED, BYTE &ILLUSION)
{
	int nAlliedCount = 0;
	int nIllusionCount = 0;

	for (int i = 0; i < 10; i++)
	{
		if (this->m_UserData[i].m_nIndex != -1 &&
			this->m_UserData[i].m_bEnterOk == true &&
			gObjIsConnected(this->m_UserData[i].m_nIndex))
		{
			if (this->m_UserData[i].m_btTeam == 0)
			{
				nAlliedCount++;
			}

			else if (this->m_UserData[i].m_btTeam == 1)
			{
				nIllusionCount++;
			}
		}
	}

	if (nAlliedCount <= 1 || nIllusionCount <= 1)
	{
		sLog->outBasic("[ ITL ] MAP : %d[CanStartITLBattle] FALSE nAlliedCount:%d,nIllusionCount:%d ",
			this->m_nTempleNumber + 1, nAlliedCount, nIllusionCount);

		ALLIED = nAlliedCount;
		ILLUSION = nIllusionCount;

		return FALSE;
	}

	else
	{
		sLog->outBasic("[ ITL ] MAP : %d[CanStartITLBattle] TRUE nAlliedCount:%d,nIllusionCount:%d ",
			this->m_nTempleNumber + 1, nAlliedCount, nIllusionCount);

		ALLIED = nAlliedCount;
		ILLUSION = nIllusionCount;

		return TRUE;
	}
}

void CIllusionTempleLeagueProcess::CheckUsersOnConnect()
{
	EnterCriticalSection(&this->m_critUserData);

	this->m_nUserCount = 0;

	for (int i = 0; i < 10; i++)
	{
		if (this->m_UserData[i].m_nIndex != -1)
		{
			if (gObjIsConnected(this->m_UserData[i].m_nIndex))
			{
				if (getGameObject(this->m_UserData[i]->m_nIndex)->MapNumber == this->m_nTempleNumber + 98)
				{
					this->m_nUserCount++;
				}

				else
				{
					getGameObject(this->m_UserData[i]->m_nIndex)->m_nITLIndex = -1;
					this->ClearUserData(i);
				}
			}

			else
			{
				getGameObject(this->m_UserData[i]->m_nIndex)->m_nITLIndex = -1;
				this->ClearUserData(i);
			}
		}
	}

	LeaveCriticalSection(&this->m_critUserData);
}

int CIllusionTempleLeagueProcess::GetRemainTimeMin()
{
	if (this->GetIllusionTempleLeagueState() || this->GetEntrance() != true)
	{
		return 0;
	}

	return this->m_nRemainTimeMin;
}

int CIllusionTempleLeagueProcess::GetEnterRemainTimeMin()
{
	if (this->GetIllusionTempleLeagueState() == 0 && this->GetEntrance() == true)
	{
		return 0;
	}

	if (this->GetIllusionTempleLeagueState() == 0 && this->GetEntrance() == false)
	{
		return this->m_nRemainTimeMin - CIllusionTempleLeagueEvent::m_nITLOpenTime;
	}

	std::list<__ITL_TIME_TABLE>::iterator iter;

	__ITL_TIME_TABLE prevStartTime;
	__ITL_TIME_TABLE nextStartTime;

	DWORD dwPrevTime = 0;
	DWORD dwNextTime = 0;
	DWORD dwCurTime = 0;
	int btRemainTime = 0;

	if (this->m_lst_ITL_Today_StartTime.size() == 0)
	{
		sLog->outError("Error : Illusion Temple StartTime size is 0");
		return 0;
	}

	tm* today;
	time_t ltime;

	time(&ltime);
	today = localtime(&ltime);

	dwCurTime = (today->tm_hour * 60) + today->tm_min;
	prevStartTime = *this->m_lst_ITL_Today_StartTime.begin();

	for (iter = this->m_lst_ITL_Today_StartTime.begin(); iter != this->m_lst_ITL_Today_StartTime.end(); ++iter)
	{
		nextStartTime = *iter;

		dwPrevTime = (prevStartTime.nHour * 60) + prevStartTime.nMin;
		dwNextTime = (nextStartTime.nHour * 60) + nextStartTime.nMin;

		if (dwPrevTime == dwNextTime)
		{
			if (dwCurTime < dwNextTime)
			{
				nextStartTime = *iter;
				break;
			}
			continue;
		}

		if (dwCurTime >= dwPrevTime &&
			dwCurTime < dwNextTime)
		{
			break;
		}

		prevStartTime = *iter;
	}

	int Cnt = 2;

	while (Cnt-- != 0)
	{
		if (iter == this->m_lst_ITL_Today_StartTime.end())
		{
			iter = this->m_lst_ITL_Today_StartTime.begin();

			nextStartTime = (*iter);
		}

		dwNextTime = nextStartTime.nHour * 60 + nextStartTime.nMin;

		if (today->tm_hour <= nextStartTime.nHour)
		{
			btRemainTime = dwNextTime - dwCurTime;
		}
		else
		{
			btRemainTime = ((24 * 60) - dwCurTime) + dwNextTime;
		}

		if (btRemainTime <= CIllusionTempleLeagueEvent::m_nITLOpenTime)
		{
			iter++;

			if (iter != this->m_lst_ITL_Today_StartTime.end())
			{
				nextStartTime = (*iter);
			}
		}
		else
		{
			btRemainTime -= CIllusionTempleLeagueEvent::m_nITLOpenTime;
			break;
		}
	}

	int tmpTime = GetTickCount();
	m_nNextTimeMin = tmpTime + ((1000 * 60) - (today->tm_sec * 1000));

	return btRemainTime;
}

int CIllusionTempleLeagueProcess::SearchRelicsItem(CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		sLog->outBasic("return %s %d", __FILE__, __LINE__);
		return -1;
	}

	for (int x = 0; x < INVENTORY_SIZE; x++)
	{
		if (Obj.pntInventory[x]->IsItem() == TRUE &&
			Obj.pntInventory[x]->m_Type == ITEMGET(14, 223))
		{
			return x;
		}
	}

	return -1;
}

void CIllusionTempleLeagueProcess::DeleteAllRelicsItem()
{
	for (int i = 0; i < 10; i++)
	{
		if (this->m_UserData[i].m_nRelicsInvenPos > 0)
		{
			CGameObject lpObj = getGameObject(this->m_UserData[i]->m_nIndex);

			if (lpObj)
			{
				if (Obj.pntInventory[this->m_UserData[i]->m_nRelicsInvenPos]->m_Type == ITEMGET(14, 223))
				{
					this->SendRelicsUserInfo(lpObj, 1);

					sLog->outBasic("[ ITL ] (%d) (%s)(%s) to Delete All Relics Item [Serial:%I64d]",
						this->m_nTempleNumber + 1, Obj.AccountID, Obj.Name, Obj.pntInventory[this->m_UserData[i]->m_nRelicsInvenPos]->m_Number);

					gObjInventoryDeleteItem(this->m_UserData[i].m_nIndex, this->m_UserData[i].m_nRelicsInvenPos);
					GSProtocol.GCInventoryItemDeleteSend(this->m_UserData[i].m_nIndex, this->m_UserData[i].m_nRelicsInvenPos, 0);
					this->m_UserData[i].m_nRelicsInvenPos = -1;
				}
			}
		}
	}
}

int CIllusionTempleLeagueProcess::FindITLUser(int index)
{
	for (int i = 0; i < 10; i++)
	{
		if (this->m_UserData[i].m_nIndex == index)
		{
			return i;
		}
	}

	return -1;
}

void CIllusionTempleLeagueProcess::DropRelicsItem(CGameObject &Obj, BYTE byAct)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		sLog->outBasic("return %s %d", __FILE__, __LINE__);
		return;
	}

	CGameObject* lpObj = Obj;
	int nITL_USER_ARRAY = this->FindITLUser(Obj.m_Index);

	if (nITL_USER_ARRAY == -1)
	{
		return;
	}

	int iItemPos = this->m_UserData[nITL_USER_ARRAY].m_nRelicsInvenPos;

	if (Obj.pntInventory[iItemPos]->m_Type != ITEMGET(14, 223))
	{
		return;
	}

	this->SendRelicsUserInfo(lpObj, 1);

	PMSG_ITEMTHROW_RESULT pResult;

	PHeadSetB((BYTE*)&pResult, 0x23, sizeof(pResult));
	pResult.Result = TRUE;
	pResult.Ipos = iItemPos;

	int map_num = Obj.MapNumber;
	int type = Obj.pntInventory[iItemPos]->m_Type;
	int level = Obj.pntInventory[iItemPos]->m_Level;
	float dur = Obj.pntInventory[iItemPos]->m_Durability;
	BOOL ret = Obj.pntInventory[iItemPos]->IsItem();
	BYTE Option1 = Obj.pntInventory[iItemPos]->m_Option1;
	BYTE Option2 = Obj.pntInventory[iItemPos]->m_Option2;
	BYTE Option3 = Obj.pntInventory[iItemPos]->m_Option3;
	BYTE NOption = Obj.pntInventory[iItemPos]->m_NewOption;
	UINT64 s_num = Obj.pntInventory[iItemPos]->m_Number;
	BYTE ItemExOption = g_kJewelOfHarmonySystem.GetItemStrengthenOption(&Obj.pntInventory[iItemPos]);
	BYTE ItemExLevel = g_kJewelOfHarmonySystem.GetItemOptionLevel(&Obj.pntInventory[iItemPos]);

	BYTE NewOption[8];

	::ItemIsBufExOption(NewOption, &Obj.pntInventory[iItemPos]);

	int PetLevel = Obj.pntInventory[iItemPos]->m_PetItem_Level;

	UINT64 PetExp = Obj.pntInventory[iItemPos]->m_PetItem_Exp;
	BYTE SOption = Obj.pntInventory[iItemPos]->m_SetOption;

	BYTE ItemEffectEx = Obj.pntInventory[iItemPos]->m_ItemOptionEx;
	int item_number = Obj.pntInventory[iItemPos]->m_Number;

	char szItemName[50] = "Relics Item";

	int aAntiLootIndex = -1;
	short X = 0;
	short Y = 0;

	BYTE btMapAttr = MapC[map_num].GetAttr(Obj.X, Obj.Y);

	if ((btMapAttr & 4) == 4 || (btMapAttr & 8) == 8)
	{
		X = 168;
		Y = 85;
	}

	else
	{
		X = Obj.X;
		Y = Obj.Y;
	}

	if (MapC[map_num].ItemDrop(type, level, dur, X, Y, Option1, Option2, Option3, NOption, SOption, item_number, aAntiLootIndex, PetLevel, PetExp, ItemEffectEx, NULL, 0xFF, 0) == TRUE)
	{
		sLog->outBasic("[ ITL ] (%d) (%s)(%s) to Drop Relics Item [Serial:%I64d]", this->m_nTempleNumber + 1, Obj.AccountID, Obj.Name, Obj.pntInventory[iItemPos]->m_Number);

		gObjInventoryDeleteItem(iIndex, iItemPos);
		pResult.Result = TRUE;

	}

	else
	{
		pResult.Result = FALSE;
	}

	this->m_UserData[nITL_USER_ARRAY].m_nRelicsInvenPos = -1;
	IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
}

void CIllusionTempleLeagueProcess::SetRelicsInventoryPos(CGameObject &Obj, BYTE btPos)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		sLog->outBasic("return %s %d", __FILE__, __LINE__);
		return;
	}

	int nITL_USER_ARRAY = this->FindITLUser(Obj.m_Index);

	if (nITL_USER_ARRAY != -1)
	{
		this->m_UserData[nITL_USER_ARRAY].m_nRelicsInvenPos = btPos;
	}
}

void CIllusionTempleLeagueProcess::SendRelicsUserInfo(CGameObject &Obj, BYTE byGet)
{
	PMSG_ITL_RELICS_GET_USER pRelicsUser;
	PHeadSubSetB((BYTE*)&pRelicsUser, 0xBF, 0x61, sizeof(pRelicsUser));

	pRelicsUser.wUserIndex = Obj.m_Index;
	pRelicsUser.byGet = byGet;
	std::memcpy(pRelicsUser.Name, Obj.Name, MAX_ACCOUNT_LEN);

	for (int i = 0; i < 10; i++)
	{
		if (this->m_UserData[i].m_nIndex != -1 && getGameObject(this->m_UserData[i]->m_nIndex)->Connected > PLAYER_LOGGED)
		{
			IOCP.DataSend(this->m_UserData[i].m_nIndex, (BYTE*)&pRelicsUser, sizeof(pRelicsUser));
		}
	}

	sLog->outBasic("[ ITL ][SendRelicsUserInfo] NAME:%s, ACT:%d", Obj.Name, byGet);
}

void CIllusionTempleLeagueProcess::SetRemainTime()
{
	this->m_nRemainTimeMin = 0;
	this->m_nRemainTimeMsec = 1000;
}

void CIllusionTempleLeagueProcess::AddITLNpcInfo(WORD wNpcType, BYTE byMapNumber, BYTE byDistance, BYTE byX, BYTE byY, BYTE byDir)
{
	if (this->m_byNpcCount > 11)
	{
		sLog->outBasic("[ ITL ][AddITLNpcInfo] Over NpcCount :%d ", this->m_byNpcCount);
		return;
	}

	BYTE byMapTagNpcIdentNo = -1;

	if (wNpcType == 658 || (wNpcType >= 659 && wNpcType <= 668))
	{
		this->m_ITLNpcInfo[this->m_byNpcCount].wNpcType = wNpcType;
		this->m_ITLNpcInfo[this->m_byNpcCount].byMapNumber = byMapNumber;
		this->m_ITLNpcInfo[this->m_byNpcCount].byDistance = byDistance;
		this->m_ITLNpcInfo[this->m_byNpcCount].byX = byX;
		this->m_ITLNpcInfo[this->m_byNpcCount].byY = byY;
		this->m_ITLNpcInfo[this->m_byNpcCount].byDir = byDir;

		g_SendNPCInfo.Find_ITL_Npc(byMapNumber, wNpcType, byMapTagNpcIdentNo);
		this->m_ITLNpcInfo[this->m_byNpcCount].byMapTagIndex = byMapTagNpcIdentNo;

		sLog->outBasic("[ ITL ][AddITLNpcInfo] NpcID:%d, Map:%d, Dis:%d, X:%d, Y:%d, Dir:%d ,byMapTagNpcIdentNo:%d",
			wNpcType, byMapNumber, byDistance, byX, byY, byDir, byMapTagNpcIdentNo);

		this->m_byNpcCount++;
	}
}

void CIllusionTempleLeagueProcess::ResetITLPosNum()
{
	this->m_Cursed_StoneStatus.m_nIndex = -1;
	this->m_Cursed_StoneStatus.m_wPosNum = -1;
	this->m_Cursed_StoneStatus.m_wType = -1;
	this->m_Cursed_StoneStatus.byMapTagIndex = -1;

	for (int i = 0; i < 10; i++)
	{
		this->m_Occupied_StoneStatus[i].m_wType = -1;
		this->m_Occupied_StoneStatus[i].m_nIndex = -1;
		this->m_Occupied_StoneStatus[i].m_wPosNum = -1;
		this->m_Occupied_StoneStatus[i].byMapTagIndex = -1;
	}
}

void CIllusionTempleLeagueProcess::RemoveAllObj()
{
	for (int i = 0; i < 10; i++)
	{
		if (this->m_Occupied_StoneStatus[i].m_nIndex != -1)
		{
			sLog->outBasic("[ ITL ] (%d) Remove Occupied_StoneStatus Name: %s, (%d: %d/%d)",
				this->m_nTempleNumber + 1, getGameObject(this->m_Occupied_StoneStatus[i]->m_nIndex)->Name,
				getGameObject(this->m_Occupied_StoneStatus[i]->m_nIndex]->MapNumber, getGameObject(this->m_Occupied_StoneStatus[i)->m_nIndex)->X,
				getGameObject(this->m_Occupied_StoneStatus[i]->m_nIndex)->Y);

			gObjDel(this->m_Occupied_StoneStatus[i].m_nIndex);
			this->m_Occupied_StoneStatus[i].m_nIndex = -1;
			this->m_Occupied_StoneStatus[i].m_wPosNum = -1;
			this->m_Occupied_StoneStatus[i].m_wType = -1;
			this->m_Occupied_StoneStatus[i].m_byStatus = -1;
			this->m_Occupied_StoneStatus[i].byMapTagIndex = -1;
		}
	}

	if (this->m_Cursed_StoneStatus.m_nIndex != -1)
	{
		sLog->outBasic("[ ITL ] (%d) Remove Cursed_StoneStatus Name: %s, (%d: %d/%d)",
			this->m_nTempleNumber + 1, getGameObject(this->m_Cursed_StoneStatus.m_nIndex)->Name,
			getGameObject(this->m_Cursed_StoneStatus.m_nIndex]->MapNumber, getGameObject(this->m_Cursed_StoneStatus.m_nIndex)->X,
			getGameObject(this->m_Cursed_StoneStatus.m_nIndex)->Y);

		gObjDel(this->m_Cursed_StoneStatus.m_nIndex);
		this->m_Cursed_StoneStatus.m_nIndex = -1;
		this->m_Cursed_StoneStatus.m_byStatus = -1;
		this->m_Cursed_StoneStatus.m_wPosNum = -1;
		this->m_Cursed_StoneStatus.m_wType = -1;
		this->m_Cursed_StoneStatus.byMapTagIndex = -1;
	}

	sLog->outBasic("[ ITL ] (%d) Remove All Object", this->m_nTempleNumber + 1);
}

void CIllusionTempleLeagueProcess::SendRelicsError(CGameObject &Npc, int index, BYTE byError, BYTE byAct)
{
	if (!lpNpc)
	{
		sLog->outBasic("[ ITL ][SendRelicsError] lpNpc Is NULL,index:%d Error:%d,Act:%d", index, byError, byAct);
		return;
	}

	PMSG_ANS_ITL_RELATE_RELICS pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xBF, 0x6A, sizeof(pMsg));

	pMsg.wNpcType = lpNpc->Class;
	pMsg.byNumberH = HIBYTE(lpNpc->m_Index);
	pMsg.byNumberL = LOBYTE(lpNpc->m_Index);
	pMsg.byError = byError;
	pMsg.byAct = byAct;

	IOCP.DataSend(index, (BYTE*)&pMsg, pMsg.h.size);
	sLog->outBasic("[ ITL ][SendRelicsError]index:%d, Error:%d,Act:%d,NPC_Class:%d,NpcNumber:%d",
		index, byError, byAct, lpNpc->Class, lpNpc->m_Index);
}

void CIllusionTempleLeagueProcess::Check_GetRelics(CGameObject &Npc, CGameObject lpObj, WORD wNpcType)
{
	if (!lpNpc)
	{
		return;
	}

	if (!lpObj)
	{
		return;
	}

	if (lpNpc->m_State == 0)
	{
		return;
	}

	if (lpNpc->m_PosNum == -1)
	{
		return;
	}

	if (abs(Obj.Y - lpNpc->Y) > 3)
	{
		return;
	}

	if (abs(Obj.X - lpNpc->X) > 3)
	{
		return;
	}

	BYTE byUserTeamIndex = this->m_UserData[this->FindITLUser(Obj.m_Index)].m_btTeam;

	if (Obj.m_bGettingRelics == true)
	{
		sLog->outBasic("[ ITL ][Check_GetRelics]NAME:%s,m_bGettingRelics%d", Obj.Name, Obj.m_bGettingRelics);
		return;
	}

	if (Obj.m_byStoneState != 99)
	{
		sLog->outBasic("[ ITL ][Check_GetRelics]NAME:%s,byStoneState%d", Obj.Name, Obj.m_byStoneState);
		return;
	}

	if (Obj.m_nITLRelicsTick != 0)
	{
		this->SendRelicsError(lpNpc, Obj.m_Index, 1, -1);
		Obj.m_nITLRelicsTick = 0;
		return;
	}

	if (wNpcType >= 659 && wNpcType <= 668)
	{
		for (int i = 0; i < 10; i++)
		{
			if (lpNpc->m_Index == this->m_Occupied_StoneStatus[i].m_nIndex)
			{
				if (this->m_Occupied_StoneStatus[i].m_byStatus && this->m_Occupied_StoneStatus[i].m_byStatus != 1)
				{
					this->SendRelicsError(lpNpc, Obj.m_Index, 6, -1);
				}

				else if (this->m_Occupied_StoneStatus[i].m_byStatus == byUserTeamIndex)
				{
					this->SendRelicsError(lpNpc, Obj.m_Index, 5, -1);
				}

				else
				{
					this->m_Occupied_StoneStatus[i].m_byStatus = byUserTeamIndex + 10;
					Obj.m_byStoneState = byUserTeamIndex + 10;
					Obj.m_nITLRelicsTick = GetTickCount() + 10000;
					Obj.m_bGettingRelics = true;
					Obj.m_wITLNpcType = this->m_Occupied_StoneStatus[i].m_wType;

					this->Send_ITL_StoneInfo(Obj.m_Index,
						this->m_Occupied_StoneStatus[i].m_wType,
						this->m_Occupied_StoneStatus[i].m_byStatus,
						this->m_Occupied_StoneStatus[i].m_nIndex,
						this->m_Occupied_StoneStatus[i].byMapTagIndex);

					this->SendRelicsError(lpNpc, Obj.m_Index, 0, 1);
				}

				return;
			}
		}
	}

	else if (wNpcType == 658)
	{
		if (this->m_Cursed_StoneStatus.m_byStatus)
		{
			this->SendRelicsError(lpNpc, Obj.m_Index, 3, -1);
			Obj.m_nITLRelicsTick = 0;
		}

		else
		{
			Obj.m_nITLRelicsTick = GetTickCount() + 10000;
			this->m_Cursed_StoneStatus.m_byStatus = byUserTeamIndex + 10;
			Obj.m_byStoneState = byUserTeamIndex + 10;
			Obj.m_nITLRelicsTick = GetTickCount() + 10000;
			Obj.m_bGettingRelics = 1;
			Obj.m_wITLNpcType = this->m_Cursed_StoneStatus.m_wType;

			this->Send_ITL_StoneInfo(Obj.m_Index,
				this->m_Cursed_StoneStatus.m_wType,
				this->m_Cursed_StoneStatus.m_byStatus,
				this->m_Cursed_StoneStatus.m_nIndex,
				this->m_Cursed_StoneStatus.byMapTagIndex);

			this->SendRelicsError(lpNpc, Obj.m_Index, 0, 1);
		}
	}

	else
	{
		this->SendRelicsError(lpNpc, Obj.m_Index, 4, -1);
		Obj.m_nITLRelicsTick = 0;
	}
}

void CIllusionTempleLeagueProcess::GetRelics(CGameObject &Npc, CGameObject lpObj, WORD wNpcType)
{
	if (!lpNpc || !lpObj)
	{
		return;
	}

	BOOL bFail = FALSE;

	if (Obj.m_wITLNpcType < 0)
	{
		Obj.m_wITLNpcType = 0;
		sLog->outBasic("[ITL ][GetRelics] ERROR CASE 1");
		bFail = TRUE;
	}

	if (Obj.m_bGettingRelics == false)
	{
		sLog->outBasic("[ITL ][GetRelics] ERROR CASE 2");
		bFail = TRUE;
	}

	if (bFail == TRUE)
	{
		this->ResetUserFlag_DoingRelicsThing(lpObj);
		return;
	}

	if (lpNpc->m_State == 0 || lpNpc->m_PosNum == -1)
	{
		sLog->outBasic("[ITL ][GetRelics] ERROR CASE 3");
		this->ResetUserFlag_DoingRelicsThing(lpObj);
		return;
	}

	if (abs(Obj.Y - lpNpc->Y) > 3 || abs(Obj.X - lpNpc->X) > 3)
	{
		sLog->outBasic("[ITL ][GetRelics] ERROR CASE 4");
		this->ResetUserFlag_DoingRelicsThing(lpObj);
		return;
	}

	if (Obj.m_nITLRelicsTick > GetTickCount())
	{
		sLog->outBasic("[ITL ][GetRelics] ERROR CASE 5");
		this->SendRelicsError(lpNpc, Obj.m_Index, 7, -1);
		this->ResetUserFlag_DoingRelicsThing(lpObj);
		return;
	}

	BOOL bCanGetRelics = FALSE;
	int nUserIndex = this->FindITLUser(Obj.m_Index);
	BYTE byUserTeamIndex = this->m_UserData[nUserIndex].m_btTeam;

	if (wNpcType == 658 && wNpcType == Obj.m_wITLNpcType)
	{
		if (this->m_Cursed_StoneStatus.m_byStatus - 10 != byUserTeamIndex ||
			Obj.m_byStoneState != this->m_Cursed_StoneStatus.m_byStatus)
		{
			sLog->outBasic("[ITL ][GetRelics] ERROR CASE 6");
			this->SendRelicsError(lpNpc, Obj.m_Index, 11, -1);
			this->ResetUserFlag_DoingRelicsThing(lpObj);
			return;
		}

		bCanGetRelics = TRUE;
		this->m_Cursed_StoneStatus.m_byStatus = -1;
		Obj.m_bGettingRelics = 0;

		this->SendRelicsError(lpNpc, Obj.m_Index, 0, 2);
		this->Send_ITL_StoneInfo(Obj.m_Index,
			this->m_Cursed_StoneStatus.m_wType,
			this->m_Cursed_StoneStatus.m_byStatus,
			this->m_Cursed_StoneStatus.m_nIndex,
			this->m_Cursed_StoneStatus.byMapTagIndex);

		lpNpc->m_PosNum = -1;
		lpNpc->Life = 0.0;

		gObjLifeCheck(lpNpc, lpObj, 0, 1, 0, 0, 0, 0, 0);
	}

	else if ((wNpcType >= 659 && wNpcType <= 668) && wNpcType == Obj.m_wITLNpcType)
	{
		for (int i = 0; i < 10; i++)
		{
			if (lpNpc->Class == this->m_Occupied_StoneStatus[i].m_wType)
			{
				if (this->m_Occupied_StoneStatus[i].m_byStatus != 10 &&
					this->m_Occupied_StoneStatus[i].m_byStatus != 11)
				{
					sLog->outBasic("[ITL ][GetRelics] ERROR CASE 8");
					this->SendRelicsError(lpNpc, Obj.m_Index, 10, -1);
					this->ResetUserFlag_DoingRelicsThing(lpObj);
					return;
				}

				if (this->m_Occupied_StoneStatus[i].m_byStatus - 10 == byUserTeamIndex)
				{
					bCanGetRelics = TRUE;

					this->m_Occupied_StoneStatus[i].m_byStatus = -2;
					Obj.m_nITLRelicsTick = 0;
					Obj.m_bGettingRelics = false;

					this->SendRelicsError(lpNpc, Obj.m_Index, 0, 2);
					this->Send_ITL_StoneInfo(Obj.m_Index,
						this->m_Occupied_StoneStatus[i].m_wType,
						this->m_Occupied_StoneStatus[i].m_byStatus,
						this->m_Occupied_StoneStatus[i].m_nIndex,
						this->m_Occupied_StoneStatus[i].byMapTagIndex);

					this->m_UserData[nUserIndex].m_byInvalidationCount++;
					break;
				}
			}
		}
	}

	else
	{
		sLog->outBasic("[ITL ][GetRelics] ERROR CASE 9");
		this->SendRelicsError(lpNpc, Obj.m_Index, 8, -1);
		this->ResetUserFlag_DoingRelicsThing(lpObj);
		return;
	}

	if (bCanGetRelics == TRUE)
	{
		ItemCreate(Obj.m_Index, Obj.MapNumber, lpNpc->X, lpNpc->Y, ItemGetNumberMake(14, 223), 0, 0, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
	}

	this->ResetUserFlag_DoingRelicsThing(lpObj);
}

void CIllusionTempleLeagueProcess::Check_RegisterRelics(CGameObject &Npc, CGameObject lpObj)
{
	if (!lpNpc || !lpObj)
	{
		return;
	}

	if (abs(Obj.Y - lpNpc->Y) > 3 || abs(Obj.X - lpNpc->X) > 3)
	{
		return;
	}

	BOOL bFail = FALSE;

	if (Obj.m_bRegisteringRelics == true)
		bFail = TRUE;
	if (Obj.m_wITLNpcType)
		bFail = TRUE;
	if (Obj.m_byStoneState != 99)
		bFail = TRUE;

	if (bFail == TRUE)
	{
		return;
	}

	int nITL_USER_ARRAY = this->FindITLUser(Obj.m_Index);

	if (this->m_UserData[nITL_USER_ARRAY].m_nRelicsInvenPos <= 0 ||
		Obj.pntInventory[this->m_UserData[nITL_USER_ARRAY]->m_nRelicsInvenPos]->m_Type != ITEMGET(14, 223))
	{
		this->SendRelicsError(lpNpc, Obj.m_Index, 12, -1);
		return;
	}

	if (lpNpc->Class < 659 || lpNpc->Class > 668)
	{
		this->SendRelicsError(lpNpc, Obj.m_Index, 13, -1);
		return;
	}

	for (int i = 0; i < 10; i++)
	{
		if (this->m_Occupied_StoneStatus[i].m_wType == lpNpc->Class)
		{
			if (this->m_Occupied_StoneStatus[i].m_byStatus == 0xFF)
			{
				if (Obj.m_nITLRelicsTick != 0)
				{
					this->SendRelicsError(lpNpc, Obj.m_Index, 16, -1);
				}

				else
				{
					this->m_Occupied_StoneStatus[i].m_byStatus = this->m_UserData[nITL_USER_ARRAY].m_btTeam + 100;
					Obj.m_byStoneState = this->m_UserData[nITL_USER_ARRAY].m_btTeam + 100;
					Obj.m_wITLNpcType = this->m_Occupied_StoneStatus[i].m_wType;
					Obj.m_nITLRelicsTick = GetTickCount() + 10000;
					Obj.m_bRegisteringRelics = TRUE;

					this->SendRelicsError(lpNpc, Obj.m_Index, 0, 3);
					this->Send_ITL_StoneInfo(Obj.m_Index,
						this->m_Occupied_StoneStatus[i].m_wType,
						this->m_Occupied_StoneStatus[i].m_byStatus,
						this->m_Occupied_StoneStatus[i].m_nIndex,
						this->m_Occupied_StoneStatus[i].byMapTagIndex);

				}
			}
		}
	}
}

void CIllusionTempleLeagueProcess::RegisterRelics(CGameObject &Npc, CGameObject lpObj)
{
	if (lpNpc->m_State == 0)
	{
		this->ResetUserFlag_DoingRelicsThing(lpObj);
		return;
	}

	if (lpNpc->m_PosNum == -1)
	{
		this->ResetUserFlag_DoingRelicsThing(lpObj);
		return;
	}

	if (abs(Obj.Y - lpNpc->Y) > 3)
	{
		this->ResetUserFlag_DoingRelicsThing(lpObj);
		return;
	}

	if (abs(Obj.X - lpNpc->X) > 3)
	{
		this->ResetUserFlag_DoingRelicsThing(lpObj);
		return;
	}

	int nRegisterRelicsUserArray = this->FindITLUser(Obj.m_Index);

	if (nRegisterRelicsUserArray == -1)
	{
		this->ResetUserFlag_DoingRelicsThing(lpObj);
		return;
	}

	if (this->m_UserData[nRegisterRelicsUserArray].m_nRelicsInvenPos > 0 &&
		Obj.pntInventory[this->m_UserData[nRegisterRelicsUserArray]->m_nRelicsInvenPos]->m_Type == ITEMGET(14, 223))
	{
		bool bFail = false;

		if (Obj.m_bRegisteringRelics == false)
		{
			bFail = true;
		}

		if (Obj.m_wITLNpcType < 0)
		{
			bFail = true;
		}

		if (Obj.m_byStoneState == 99)
		{
			bFail = true;
		}

		if (bFail == true)
		{
			this->ResetUserFlag_DoingRelicsThing(lpObj);
		}

		else
		{
			if (Obj.m_nITLRelicsTick <= GetTickCount())
			{
				if (lpNpc->Class >= 659 && lpNpc->Class <= 668)
				{
					for (int i = 0; i < 10; i++)
					{
						if (this->m_Occupied_StoneStatus[i].m_wType == lpNpc->Class &&
							this->m_UserData[nRegisterRelicsUserArray].m_btTeam == this->m_Occupied_StoneStatus[i].m_byStatus - 100)
						{
							this->m_Occupied_StoneStatus[i].m_byStatus = this->m_UserData[nRegisterRelicsUserArray].m_btTeam;
							this->m_UserData[nRegisterRelicsUserArray].m_byOccupiedCount++;

							this->SendRelicsError(lpNpc, Obj.m_Index, 0, 4);
							this->Send_ITL_StoneInfo(Obj.m_Index,
								this->m_Occupied_StoneStatus[i].m_wType,
								this->m_Occupied_StoneStatus[i].m_byStatus,
								this->m_Occupied_StoneStatus[i].m_nIndex,
								this->m_Occupied_StoneStatus[i].byMapTagIndex);

							Obj.m_bRegisteringRelics = 0;

							if (this->m_UserData[nRegisterRelicsUserArray].m_nRelicsInvenPos > 0)
							{
								if (Obj.pntInventory[this->m_UserData[nRegisterRelicsUserArray]->m_nRelicsInvenPos]->m_Type == ITEMGET(14, 223))
								{
									this->SendRelicsUserInfo(lpObj, 1);
									sLog->outBasic("[ ITL ][RegisterRelics] (%d) (%s)(%s) to Delete Relics Item [Serial:%I64d]",
										this->m_nTempleNumber + 1, Obj.AccountID, Obj.Name, Obj.pntInventory[this->m_UserData[nRegisterRelicsUserArray]->m_nRelicsInvenPos]->m_Number);

									gObjInventoryDeleteItem(Obj.m_Index, this->m_UserData[nRegisterRelicsUserArray].m_nRelicsInvenPos);
									GSProtocol.GCInventoryItemDeleteSend(Obj.m_Index, this->m_UserData[nRegisterRelicsUserArray].m_nRelicsInvenPos, 0);
									this->m_UserData[nRegisterRelicsUserArray].m_nRelicsInvenPos = -1;
								}
							}

							break;

						}
					}

					this->Set_ITLState(4);
					this->ResetUserFlag_DoingRelicsThing(lpObj);
				}

				else
				{
					this->SendRelicsError(lpNpc, Obj.m_Index, 20, -1);
					this->ResetUserFlag_DoingRelicsThing(lpObj);
				}
			}

			else
			{
				this->SendRelicsError(lpNpc, Obj.m_Index, 18, -1);
				this->ResetUserFlag_DoingRelicsThing(lpObj);
			}
		}
	}

	else
	{
		this->SendRelicsError(lpNpc, Obj.m_Index, 17, -1);
		this->ResetUserFlag_DoingRelicsThing(lpObj);
	}
}

void CIllusionTempleLeagueProcess::FirstRegen_OccupiedStone()
{
	for (int i = 0; i < 10; i++)
	{
		CGameObject lpObj = getGameObject(this->m_Occupied_StoneStatus[i]->m_nIndex);

		if (lpObj)
		{
			Obj.m_PosNum = this->m_Occupied_StoneStatus[i].m_wPosNum;
			Obj.Live = TRUE;
			Obj.DieRegen = 0;
			Obj.m_State = 1;
			this->m_Occupied_StoneStatus[i].m_byStatus = -1;
		}
	}

	this->m_bFirstRegen_OccStone = true;
}

void CIllusionTempleLeagueProcess::FirstRegen_CursedStone()
{
	CGameObject lpObj = getGameObject(this->m_Cursed_StoneStatus.m_nIndex);

	if (lpObj)
	{
		Obj.m_PosNum = this->m_Cursed_StoneStatus.m_wPosNum;
		Obj.Live = TRUE;
		Obj.DieRegen = 0;
		Obj.m_State = 1;
		this->m_Cursed_StoneStatus.m_byStatus = 0;
		this->m_bFirstRegen_CursedStone = true;
	}
}

void CIllusionTempleLeagueProcess::RegenCursedStoneStatus(BYTE btRound)
{
	BYTE byOccupiedStoneCnt = 0;

	for (int i = 0; i < 10; i++)
	{
		if (this->m_Occupied_StoneStatus[i].m_byStatus == 0 ||
			this->m_Occupied_StoneStatus[i].m_byStatus == 1)
		{
			byOccupiedStoneCnt++;
		}
	}

	CGameObject lpObj;

	if ((rand() % 100) > (100 - 10 * byOccupiedStoneCnt))
	{
		this->m_Cursed_StoneStatus.m_byStatus = -1;
		lpObj = getGameObject(this->m_Cursed_StoneStatus.m_nIndex);
		Obj.m_PosNum = this->m_Cursed_StoneStatus.m_wPosNum;
		Obj.DieRegen = 2;
		Obj.Live = 1;
		Obj.m_State = 16;
	}
	else
	{
		this->m_Cursed_StoneStatus.m_byStatus = 0;
		lpObj = getGameObject(this->m_Cursed_StoneStatus.m_nIndex);
		Obj.m_PosNum = this->m_Cursed_StoneStatus.m_wPosNum;
		Obj.Live = 1;
		Obj.DieRegen = 0;
		Obj.m_State = 1;
	}

	this->Send_ITL_StoneInfo(0, this->m_Cursed_StoneStatus.m_wType,
		this->m_Cursed_StoneStatus.m_byStatus,
		this->m_Cursed_StoneStatus.m_nIndex,
		this->m_Cursed_StoneStatus.byMapTagIndex);

	sLog->outBasic("[ ITL ][RegenCursedStoneStatus] Create NPC Status Map:%d, Round:%d,RegenStatus:%d, X:%d,Y:%d",
		this->m_nTempleNumber + 1, btRound, this->m_Cursed_StoneStatus.m_byStatus, Obj.X, Obj.Y);
}

void CIllusionTempleLeagueProcess::SetStatusRegenTime()
{
	this->m_StatusRegenTime = GetTickCount() + 5000;
}

void CIllusionTempleLeagueProcess::AddITLOccupiedStonePos(WORD wNpcType, int index, int Count, BYTE byMapTagIndex)
{
	if (wNpcType < 659 || wNpcType > 668)
	{
		return;
	}

	if (this->m_Occupied_StoneStatus[Count].m_nIndex == -1)
	{
		this->m_Occupied_StoneStatus[Count].m_nIndex = index;
		this->m_Occupied_StoneStatus[Count].m_wType = wNpcType;
		this->m_Occupied_StoneStatus[Count].m_wPosNum = Count;
		this->m_Occupied_StoneStatus[Count].m_byStatus = -1;
		this->m_Occupied_StoneStatus[Count].byMapTagIndex = byMapTagIndex;

		sLog->outBasic("[ ITL ][AddITLOccupiedStonePos] ArrayNUm :%d wNpcType:%d, index:%d ",
			Count, wNpcType, index);
	}
}

void CIllusionTempleLeagueProcess::AddITLCursedStonePos(WORD wNpcType, int index, BYTE byMapTagIndex)
{
	if (wNpcType != 658)
	{
		return;
	}

	this->m_Cursed_StoneStatus.m_wType = wNpcType;
	this->m_Cursed_StoneStatus.m_nIndex = index;
	this->m_Cursed_StoneStatus.m_wPosNum = 0;
	this->m_Cursed_StoneStatus.m_byStatus = 0;
	this->m_Cursed_StoneStatus.byMapTagIndex = byMapTagIndex;
	sLog->outBasic("[ ITL ][AddITLCursedStonePos] NpcType:%d, index :%d ", wNpcType, index);
}

void CIllusionTempleLeagueProcess::SetNpc()
{
	int nOccCount = 0;

	for (int i = 0; i < 11; i++)
	{
		int result = gObjAddMonster(this->m_ITLNpcInfo[i].byMapNumber);

		if (result > 0)
		{
			getGameObject(result)->m_PosNum = i;
			getGameObject(result)->X = this->m_ITLNpcInfo[i)->byX;
			getGameObject(result)->Y = this->m_ITLNpcInfo[i)->byY;
			getGameObject(result)->MapNumber = this->m_ITLNpcInfo[i)->byMapNumber;
			getGameObject(result)->TX = getGameObject(result)->X;
			getGameObject(result)->TY = getGameObject(result)->Y;
			getGameObject(result)->m_OldX = getGameObject(result)->X;
			getGameObject(result)->m_OldY = getGameObject(result)->Y;
			getGameObject(result)->Dir = this->m_ITLNpcInfo[i)->byDir;
			getGameObject(result)->StartX = getGameObject(result)->X;
			getGameObject(result)->StartY = getGameObject(result)->Y;
			gObjSetMonster(result, this->m_ITLNpcInfo[i].wNpcType);

			if (this->m_ITLNpcInfo[i].wNpcType == 658)
			{
				this->AddITLCursedStonePos(this->m_ITLNpcInfo[i].wNpcType, result, this->m_ITLNpcInfo[i].byMapTagIndex);
			}

			else if (this->m_ITLNpcInfo[i].wNpcType >= 659 && this->m_ITLNpcInfo[i].wNpcType <= 668)
			{
				this->AddITLOccupiedStonePos(this->m_ITLNpcInfo[i].wNpcType, result, nOccCount, this->m_ITLNpcInfo[i].byMapTagIndex);
				nOccCount++;
			}
		}
	}

	this->m_bIsSetNpc = true;
}

void CIllusionTempleLeagueProcess::RegenOccupiedStoneStatus()
{
	for (int i = 0; i < 10; i++)
	{
		if (this->m_Occupied_StoneStatus[i].m_nIndex != -1)
		{
			CGameObject lpObj = getGameObject(this->m_Occupied_StoneStatus[i]->m_nIndex);

			Obj.m_PosNum = this->m_Occupied_StoneStatus[i].m_wPosNum;
			Obj.Live = TRUE;
			Obj.DieRegen = FALSE;
			Obj.m_State = 1;

			if (this->m_Occupied_StoneStatus[i].m_byStatus == 10 ||
				this->m_Occupied_StoneStatus[i].m_byStatus == 11 ||
				this->m_Occupied_StoneStatus[i].m_byStatus == 100 ||
				this->m_Occupied_StoneStatus[i].m_byStatus == 101 ||
				this->m_Occupied_StoneStatus[i].m_byStatus == 254)
			{
				this->m_Occupied_StoneStatus[i].m_byStatus = -1;
				this->Send_ITL_StoneInfo(0, this->m_Occupied_StoneStatus[i].m_wType,
					this->m_Occupied_StoneStatus[i].m_byStatus, this->m_Occupied_StoneStatus[i].m_nIndex,
					this->m_Occupied_StoneStatus[i].byMapTagIndex);

				sLog->outBasic("[ ITL ][RegenOccupiedStoneStatus] Round [%d] NpcIndex:[%d],Type:[%d] MAKE NEUTRAL",
					this->m_byITLRound, this->m_Occupied_StoneStatus[i].m_nIndex, this->m_Occupied_StoneStatus[i].m_wType);
			}
		}
	}

	this->m_StatusRegenTime = -1;
}

void CIllusionTempleLeagueProcess::SendNoticeMessage(char *lpMsg)
{
	PMSG_NOTICE pNotice;
	TNotice::MakeNoticeMsg(&pNotice, 0, lpMsg);

	for (int i = 0; i < 10; i++)
	{
		if (this->m_UserData[i].m_nIndex != -1)
		{
			if (getGameObject(this->m_UserData[i]->m_nIndex)->Connected > PLAYER_LOGGED)
			{
				IOCP.DataSend(this->m_UserData[i].m_nIndex, (BYTE*)&pNotice, pNotice.h.size);
			}
		}
	}
}

void CIllusionTempleLeagueProcess::Send_ITL_StoneInfo(int userindex, WORD wNpcType, BYTE byState, int NpcIndex, BYTE byMapTagIndex)
{
	PMSG_ITL_STONEINFO pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xBF, 0x6C, sizeof(pMsg));

	pMsg.byMapTagIndex = byMapTagIndex;
	pMsg.wOccupiedStoneType = wNpcType;
	pMsg.byOccupiedStoneState = byState;
	pMsg.nNpcIndex = NpcIndex;

	for (int i = 0; i < 10; i++)
	{
		if (this->m_UserData[i].m_nIndex != -1)
		{
			if (ITL_MAP_RANGE(getGameObject(this->m_UserData[i]->m_nIndex)->MapNumber))
			{
				IOCP.DataSend(this->m_UserData[i].m_nIndex, (BYTE*)&pMsg, pMsg.h.size);
				sLog->outBasic("[ ITL ][Send_ITL_StoneInfo] userindex:%d, npcType:%d, npcstate:%d ,npcindex:%d,MAPTAG:%d",
					this->m_UserData[i].m_nIndex, wNpcType, byState, NpcIndex, byMapTagIndex);
			}
		}
	}
}

void CIllusionTempleLeagueProcess::Send_ITL_PlayState(BYTE btTeam)
{
	if (this->m_nRemainTimeMsec < 0)
	{
		this->m_nRemainTimeMsec = 0;
	}

	PMSG_ITL_PLAY_STATE pMsg;

	BYTE byAllied, byIllusion;
	this->GetOccupiedStoneCount(byAllied, byIllusion);

	pMsg.wRemainSec = this->m_nRemainTimeMsec / 1000;
	pMsg.byAlliedPoint = byAllied;
	pMsg.btIllusionPoint = byIllusion;
	pMsg.btMyTeam = btTeam;
	PHeadSubSetB((BYTE*)&pMsg, 0xBF, 0x62, sizeof(pMsg));

	for (int i = 0; i < 10; i++)
	{
		if (this->m_UserData[i].m_nIndex != -1 && getGameObject(this->m_UserData[i]->m_nIndex)->Connected > PLAYER_LOGGED)
		{
			if (this->m_UserData[i].m_btTeam == btTeam)
			{
				IOCP.DataSend(this->m_UserData[i].m_nIndex, (BYTE*)&pMsg, pMsg.h.size);
			}
		}
	}
}

void CIllusionTempleLeagueProcess::SendIllusionTempleLeagueState(BYTE btTempleState, int nIndex)
{
	PMSG_ITL_STATE pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xBF, 0x60, sizeof(pMsg));

	pMsg.btTempleNumber = this->m_nTempleNumber;
	pMsg.btITLState = btTempleState;

	if (nIndex == -1)
	{
		for (int i = 0; i < 10; i++)
		{
			if (this->m_UserData[i].m_nIndex != -1 && getGameObject(this->m_UserData[i]->m_nIndex)->Connected > PLAYER_LOGGED)
			{
				IOCP.DataSend(this->m_UserData[i].m_nIndex, (BYTE*)&pMsg, pMsg.h.size);
			}
		}
	}

	else
	{
		IOCP.DataSend(nIndex, (BYTE*)&pMsg, pMsg.h.size);
	}
}

BYTE CIllusionTempleLeagueProcess::GetUserTeam(int nIndex)
{
	if (!ObjectMaxRange(nIndex))
	{
		sLog->outBasic("return %s %d", __FILE__, __LINE__);
		return -1;
	}

	if (getGameObject(nIndex)->m_nITLIndex == -1 || this->m_UserData[getGameObject(nIndex)->m_nITLIndex)->m_nIndex != nIndex)
	{
		return -1;
	}

	if (getGameObject(nIndex)->Connected < PLAYER_PLAYING)
	{
		return -1;
	}

	return this->m_UserData[getGameObject(nIndex)->m_nITLIndex)->m_btTeam;
}

void CIllusionTempleLeagueProcess::IncUserKillCount(int nIndex)
{
	if (!ObjectMaxRange(nIndex))
	{
		sLog->outBasic("return %s %d", __FILE__, __LINE__);
		return;
	}

	if (getGameObject(nIndex)->m_nITLIndex == -1 || this->m_UserData[getGameObject(nIndex)->m_nITLIndex)->m_nIndex != nIndex)
	{
		return;
	}

	if (getGameObject(nIndex)->Connected < PLAYER_PLAYING)
	{
		return;
	}

	if (this->m_UserData[getGameObject(nIndex)->m_nITLIndex)->m_btUserKillCount >= 255)
	{
		return;
	}

	sLog->outBasic("[ ITL ][UserKill] map:%d, ACC:%s, NAME:%s ,KillCnt:%d -> %d",
		this->m_nTempleNumber + 1, getGameObject(nIndex)->AccountID, getGameObject(nIndex)->Name,
		this->m_UserData[getGameObject(nIndex)->m_nITLIndex]->m_btUserKillCount, this->m_UserData[getGameObject(nIndex)->m_nITLIndex)->m_btUserKillCount + 1);

	this->m_UserData[getGameObject(nIndex)->m_nITLIndex)->m_btUserKillCount++;
}

void CIllusionTempleLeagueProcess::ResetKillCount()
{
	for (int i = 0; i < 10; i++)
	{
		this->m_UserData[i].m_btUserKillCount = 0;
	}
}

void CIllusionTempleLeagueProcess::ITL_UseSkill(int nIndex, WORD wSkillNumber, int nTargetIndex, BYTE btDis)
{
	if (!ObjectMaxRange(nIndex))
	{
		sLog->outBasic("return %s %d", __FILE__, __LINE__);
		return;
	}

	if (!ObjectMaxRange(nTargetIndex))
	{
		sLog->outBasic("return %s %d", __FILE__, __LINE__);
		return;
	}

	CGameObject lpObj = getGameObject(nIndex);
	CGameObject lpTargetObj = getGameObject(nTargetIndex);

	if (this->GetIllusionTempleLeagueState() != 2)
	{
		sLog->outBasic("Process ITL_UseSkill Error #1 iIndex :%d Name:%s", nIndex, getGameObject(nIndex)->Name);
		this->SendUseSkillResult(nIndex, nTargetIndex, wSkillNumber, 0);

		return;
	}

	if (Obj.Connected < PLAYER_PLAYING || lpTargetObj.Connected < PLAYER_PLAYING)
	{
		sLog->outBasic("Process ITL_UseSkill Error #2 iIndex :%d Name:%s", nIndex, getGameObject(nIndex)->Name);
		this->SendUseSkillResult(nIndex, nTargetIndex, wSkillNumber, 0);

		return;
	}

	if (Obj.MapNumber != lpTargetObj.MapNumber)
	{
		sLog->outBasic("Process ITL_UseSkill Error #3 iIndex :%d Name:%s", nIndex, getGameObject(nIndex)->Name);
		this->SendUseSkillResult(nIndex, nTargetIndex, wSkillNumber, 0);

		return;
	}

	if (Obj.m_nITLIndex == -1 || lpTargetObj.m_nITLIndex == -1)
	{
		sLog->outBasic("Process ITL_UseSkill Error #4 iIndex :%d Name:%s", nIndex, getGameObject(nIndex)->Name);
		this->SendUseSkillResult(nIndex, nTargetIndex, wSkillNumber, 0);

		return;
	}

	int ret;

	switch (wSkillNumber)
	{
	case 618:
		ret = this->UseSkillProdection(lpObj);
		break;
	case 619:
		ret = this->UseSkillRestraint(lpObj, lpTargetObj);
		break;
	case 620:
		ret = this->UseSkillTeleport(lpObj);
		break;
	case 621:
		ret = this->UseSkillShieldBurn(lpObj, lpTargetObj);
		break;
	default:
		ret = 0;
		break;
	}

	if (ret == TRUE)
	{
		sLog->outBasic("[ ITL ] (%d) Use Skill (%d), (%s)(%s), (%d)",
			this->m_nTempleNumber + 1, wSkillNumber, Obj.AccountID, Obj.Name, -1);
	}

	if (wSkillNumber != 620)
	{
		if (ret == TRUE)
		{
			this->SendUseSkillResult(nIndex, nTargetIndex, wSkillNumber, TRUE);
		}

		else
		{
			sLog->outBasic("Process ITL_UseSkill Error #5 iIndex :%d Name:%s", nIndex, getGameObject(nIndex)->Name);
		}
	}

	if (ret == FALSE)
	{
		sLog->outBasic("Process ITL_UseSkill Error #6 iIndex :%d Name:%s", nIndex, getGameObject(nIndex)->Name);
		this->SendUseSkillResult(nIndex, nTargetIndex, wSkillNumber, FALSE);
	}
}

void CIllusionTempleLeagueProcess::ITLSkillProc(CGameObject &Obj)
{
	if (this->m_UserData[Obj.m_nITLIndex].m_nIndex != Obj.m_Index)
	{
		return;
	}

	if (this->m_UserData[Obj.m_nITLIndex].m_dwSkillProdectionTime != 0)
	{
		this->m_UserData[Obj.m_nITLIndex].m_dwSkillProdectionTime--;

		if (this->m_UserData[Obj.m_nITLIndex].m_dwSkillProdectionTime == 0)
		{
			this->m_UserData[Obj.m_nITLIndex].m_dwSkillProdectionTime = 0;
			gObjRemoveBuffEffect(lpObj, BUFFTYPE_TEMPLE_PROTECTION);
			this->Send_ITL_SkillEnd(lpObj, 618);
		}
	}

	if (this->m_UserData[Obj.m_nITLIndex].m_dwSkillRestraintTime != 0)
	{
		this->m_UserData[Obj.m_nITLIndex].m_dwSkillRestraintTime--;

		if (this->m_UserData[Obj.m_nITLIndex].m_dwSkillRestraintTime == 0)
		{
			this->m_UserData[Obj.m_nITLIndex].m_dwSkillRestraintTime = 0;
			gObjRemoveBuffEffect(lpObj, BUFFTYPE_RESTRICTION);
			this->Send_ITL_SkillEnd(lpObj, 619);
		}
	}
}

void CIllusionTempleLeagueProcess::Send_ITL_SkillEnd(CGameObject lpObj, WORD wSkillNumber)
{
	PMSG_ITL_SKILL_END pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xBF, 0x63, sizeof(pMsg));

	pMsg.MagicNumberH = HIBYTE(wSkillNumber);
	pMsg.MagicNumberL = LOBYTE(wSkillNumber);
	pMsg.wObjIndex = Obj.m_Index;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	GSProtocol.MsgSendV2(lpObj, (BYTE*)&pMsg, pMsg.h.size);
}

int CIllusionTempleLeagueProcess::UseSkillProdection(CGameObject &Obj)
{
	this->m_UserData[Obj.m_nITLIndex].m_dwSkillProdectionTime = 10;
	gObjAddBuffEffect(lpObj, BUFFTYPE_TEMPLE_PROTECTION, 0, 0, 0, 0, -10);
	return TRUE;
}

int CIllusionTempleLeagueProcess::UseSkillRestraint(CGameObject &Obj, CGameObject lpTargetObj)
{
	if (gCheckSkillDistance(Obj.m_Index, lpTargetObj.m_Index, 619) == FALSE)
	{
		return FALSE;
	}

	this->m_UserData[lpTargetObj.m_nITLIndex].m_dwSkillRestraintTime = 10;
	lpTargetObj.lpAttackObj = lpObj;
	gObjAddBuffEffect(lpObj, BUFFTYPE_RESTRICTION, 0, 0, 0, 0, -10);
	lpTargetObj.PathCount = 0;
	lpTargetObj.PathStartEnd = 0;
	gObjSetPosition(lpTargetObj.m_Index, lpTargetObj.X, lpTargetObj.Y);
	return TRUE;
}

int CIllusionTempleLeagueProcess::UseSkillTeleport(CGameObject &Obj)
{
	if (gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_STUN) == TRUE)
	{
		sLog->outBasic("UseSkillTeleport Error #1 iIndex :%d Name:%s", Obj.m_Index, Obj.Name);
		this->SendUseSkillResult(Obj.m_Index, Obj.m_Index, 620, 0);

		return FALSE;
	}

	if (gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_SLEEP) == TRUE)
	{
		sLog->outBasic("UseSkillTeleport Error #2 iIndex :%d Name:%s", Obj.m_Index, Obj.Name);
		this->SendUseSkillResult(Obj.m_Index, Obj.m_Index, 620, 0);

		return FALSE;
	}

	int tmpRelicsUserIndex = -1;
	int RelicsHaveUserCount = 0;

	for (int i = 0; i < 10; i++)
	{
		if (this->m_UserData[i].m_nRelicsInvenPos > 0)
		{
			RelicsHaveUserCount++;
		}
	}

	if (RelicsHaveUserCount > 0)
	{
		int nRand = rand() % RelicsHaveUserCount;

		if (nRand < 10)
		{
			int nRealCount = 0;

			for (int j = 0; j < 10; j++)
			{
				if (this->m_UserData[j].m_nRelicsInvenPos > 0)
				{
					if (nRealCount == nRand)
					{
						tmpRelicsUserIndex = this->m_UserData[j].m_nIndex;
					}

					nRealCount++;
				}
			}
		}
	}

	if (tmpRelicsUserIndex == -1)
	{
		sLog->outBasic("UseSkillTeleport Error #3 iIndex :%d Name:%s", Obj.m_Index, Obj.Name);
		this->SendUseSkillResult(Obj.m_Index, Obj.m_Index, 620, 0);

		return FALSE;
	}

	if (tmpRelicsUserIndex == Obj.m_Index)
	{
		sLog->outBasic("UseSkillTeleport Error #4 iIndex :%d Name:%s", Obj.m_Index, Obj.Name);
		this->SendUseSkillResult(Obj.m_Index, Obj.m_Index, 620, 0);

		return FALSE;
	}

	this->SendUseSkillResult(Obj.m_Index, Obj.m_Index, 620, TRUE);

	int addx = 0;
	int addy = 0;
	int count = 50;
	BOOL Find = FALSE;

	while (count-- != 0)
	{
		addx = getGameObject(tmpRelicsUserIndex)->X + rand() % 3 - 1;
		addy = getGameObject(tmpRelicsUserIndex)->Y + rand() % 3 - 1;

		if (gObjCheckTeleportArea(Obj.m_Index, addx, addy))
		{
			Find = TRUE;
			break;
		}
	}

	if (Find == FALSE)
	{
		addx = getGameObject(tmpRelicsUserIndex)->X;
		addy = getGameObject(tmpRelicsUserIndex)->Y;
	}

	gObjTeleportMagicUse(Obj.m_Index, addx, addy);
	return TRUE;
}

int CIllusionTempleLeagueProcess::UseSkillShieldBurn(CGameObject &Obj, CGameObject lpTargetObj)
{
	if (gCheckSkillDistance(Obj.m_Index, lpTargetObj.m_Index, 621) == FALSE)
	{
		return FALSE;
	}

	lpTargetObj.lpAttackObj = lpObj;

	if (lpTargetObj.iShield <= 1)
	{
		GSProtocol.GCDamageSend(Obj.m_Index, lpTargetObj.m_Index, 0, 0, 0, 0);
		return TRUE;
	}

	int nShieldDamage = lpTargetObj.iShield / 2;

	lpTargetObj.iShield = nShieldDamage;

	GSProtocol.GCDamageSend(Obj.m_Index, lpTargetObj.m_Index, 0, 0, 0, nShieldDamage);
	return TRUE;
}

void CIllusionTempleLeagueProcess::SendUseSkillResult(int nIndex, int nTargetIndex, WORD wSkillNumber, BYTE btResult)
{
	PMSG_ITL_USE_MAGIC_RESULT pResult;
	PHeadSubSetB((BYTE*)&pResult, 0xBF, 0x67, sizeof(pResult));

	pResult.btResult = btResult;
	pResult.MagicNumberH = HIBYTE(wSkillNumber);
	pResult.MagicNumberL = LOBYTE(wSkillNumber);
	pResult.wSourceObjIndex = nIndex;
	pResult.wTargetObjIndex = nTargetIndex;

	IOCP.DataSend(nIndex, (BYTE*)&pResult, pResult.h.size);

	if (btResult == TRUE)
	{
		GSProtocol.MsgSendV2(getGameObject(nIndex), (BYTE*)&pResult, pResult.h.size);
	}

	sLog->outBasic("[ ITL ] SendUseSkillResult Success:[%d], Index:[%d], Target:[%d], Skill:[%d] ",
		btResult, nIndex, nTargetIndex, wSkillNumber);
}

void CIllusionTempleLeagueProcess::ITLUserDie(CGameObject &Obj)
{
	if (Obj.m_nITLIndex == -1)
	{
		return;
	}

	if (this->m_UserData[Obj.m_nITLIndex].m_nIndex != Obj.m_Index)
	{
		return;
	}

	this->DropRelicsItem(Obj.m_Index, 1);

	if (this->m_UserData[Obj.m_nITLIndex].m_dwSkillProdectionTime != 0)
	{
		this->m_UserData[Obj.m_nITLIndex].m_dwSkillProdectionTime = 0;
		gObjRemoveBuffEffect(lpObj, BUFFTYPE_TEMPLE_PROTECTION);
		this->Send_ITL_SkillEnd(lpObj, 618);
	}

	if (this->m_UserData[Obj.m_nITLIndex].m_dwSkillRestraintTime != 0)
	{
		this->m_UserData[Obj.m_nITLIndex].m_dwSkillRestraintTime = 0;
		gObjRemoveBuffEffect(lpObj, BUFFTYPE_RESTRICTION);
		this->Send_ITL_SkillEnd(lpObj, 619);
	}
}

void CIllusionTempleLeagueProcess::GetOccupiedStoneCount(BYTE &byAllied, BYTE &byIllusion)
{
	byAllied = 0;
	byIllusion = 0;

	for (int i = 0; i < 10; i++)
	{
		if (this->m_Occupied_StoneStatus[i].m_byStatus == 0)
		{
			byAllied++;
		}

		else if (this->m_Occupied_StoneStatus[i].m_byStatus == 1)
		{
			byIllusion++;
		}
	}
}

void CIllusionTempleLeagueProcess::CalCursetTempleResult()
{
	this->CheckUsersOnConnect();

	int nAlliedExp = 0;
	int nIllusionExp = 0;
	BYTE byWinnerGuildIndex = -1;
	BYTE byLoseGuildIndex = -1;
	BYTE byWinGuildOccupiedCount = 0;
	BYTE byLoseGuildOccupiedCount = 0;
	BYTE byAlliedCnt = this->m_byAlliedTeamPoint;
	BYTE byIllusionCnt = this->m_byIllusionTeamPoint;

	if (byAlliedCnt <= byIllusionCnt)
	{
		if (byAlliedCnt >= byIllusionCnt)
		{
			if (byAlliedCnt == byIllusionCnt)
			{
				BYTE byAlliedTeamKillSum = 0;
				BYTE byIllusionTeamKillSum = 0;

				this->CalcKillCountByTeam(byAlliedTeamKillSum, byIllusionTeamKillSum);

				if (byAlliedTeamKillSum <= byIllusionTeamKillSum)
				{
					if (byAlliedTeamKillSum >= byIllusionTeamKillSum)
					{
						int nAlliedLevelSum = 0;
						int nIllusionLevelSum = 0;

						for (int i = 0; i < 10; i++)
						{
							if (this->m_UserData[i].m_btTeam == 0)
							{
								nAlliedLevelSum += (getGameObject(this->m_UserData[i]->m_nIndex]->Level + getGameObject(this->m_UserData[i)->m_nIndex)->m_PlayerData->MasterLevel);
							}

							else if (this->m_UserData[i].m_btTeam == 1)
							{
								nIllusionLevelSum += (getGameObject(this->m_UserData[i]->m_nIndex]->Level + getGameObject(this->m_UserData[i)->m_nIndex)->m_PlayerData->MasterLevel);
							}
						}

						if (nAlliedLevelSum <= nIllusionLevelSum)
						{
							if (nAlliedLevelSum < nIllusionLevelSum)
							{
								this->m_btWinTeam = 0;
								this->m_btLoseTeam = 1;
								byWinGuildOccupiedCount = byAlliedCnt;
								byLoseGuildOccupiedCount = byIllusionCnt;
								byWinnerGuildIndex = 0;
								byLoseGuildIndex = 1;
							}
						}

						else
						{
							this->m_btWinTeam = 1;
							this->m_btLoseTeam = 0;
							byWinGuildOccupiedCount = byIllusionCnt;
							byLoseGuildOccupiedCount = byAlliedCnt;
							byWinnerGuildIndex = 1;
							byLoseGuildIndex = 0;
						}
					}

					else
					{
						this->m_btWinTeam = 1;
						this->m_btLoseTeam = 0;
						byWinGuildOccupiedCount = byIllusionCnt;
						byLoseGuildOccupiedCount = byAlliedCnt;
						byWinnerGuildIndex = 1;
						byLoseGuildIndex = 0;
					}
				}

				else
				{
					this->m_btWinTeam = 0;
					this->m_btLoseTeam = 1;
					byWinGuildOccupiedCount = byAlliedCnt;
					byLoseGuildOccupiedCount = byIllusionCnt;
					byWinnerGuildIndex = 0;
					byLoseGuildIndex = 1;
				}
			}
		}

		else
		{
			this->m_btWinTeam = 1;
			this->m_btLoseTeam = 0;
			byWinGuildOccupiedCount = byIllusionCnt;
			byLoseGuildOccupiedCount = byAlliedCnt;
			byWinnerGuildIndex = 1;
			byLoseGuildIndex = 0;
		}
	}

	else
	{
		this->m_btWinTeam = 0;
		this->m_btLoseTeam = 1;
		byWinGuildOccupiedCount = byAlliedCnt;
		byLoseGuildOccupiedCount = byIllusionCnt;
		byWinnerGuildIndex = 0;
		byLoseGuildIndex = 1;
	}

	if (byWinnerGuildIndex != 255 && byLoseGuildIndex != 255)
	{
		EnterCriticalSection(&this->m_criGuildData);
		this->m_GuildDataSave[byWinnerGuildIndex].byWin = TRUE;
		this->m_GuildDataSave[byWinnerGuildIndex].byLose = FALSE;
		this->m_GuildDataSave[byWinnerGuildIndex].byOccupiedCount = byWinGuildOccupiedCount;
		this->m_GuildDataSave[byLoseGuildIndex].byWin = FALSE;
		this->m_GuildDataSave[byLoseGuildIndex].byLose = TRUE;
		this->m_GuildDataSave[byLoseGuildIndex].byOccupiedCount = byLoseGuildOccupiedCount;
		LeaveCriticalSection(&this->m_criGuildData);

		sLog->outBasic("[ ITL ] (%d) Result (WinTeam: %d)(Score:[%d][%d])", this->m_nTempleNumber + 1, this->m_btWinTeam, byAlliedCnt, byIllusionCnt);
	}

	else
	{
		sLog->outBasic("[ ITL ] (%d) Result (Team: %d)(Score:[%d][%d]) byWinnerGuildIndex :%d,byLoseGuildIndex : %d ", this->m_nTempleNumber + 1, this->m_btWinTeam, byAlliedCnt, byIllusionCnt, byWinnerGuildIndex, byLoseGuildIndex);
		this->m_bNobodyWinLose = true;
	}
}

void CIllusionTempleLeagueProcess::DropChaosGem(int nIndex)
{
	if (!ObjectMaxRange(nIndex))
	{
		return;
	}

	if (this->m_UserData[getGameObject(nIndex)->m_nITLIndex)->m_nIndex != nIndex)
	{
		return;
	}

	if (this->GetIllusionTempleLeagueState() != 3)
	{
		return;
	}

	if (this->m_UserData[getGameObject(nIndex)->m_nITLIndex)->m_btTeam == 0xFF)
	{
		return;
	}

	if (this->m_btWinTeam == 0xFF)
	{
		return;
	}

	if (this->m_UserData[getGameObject(nIndex)->m_nITLIndex)->m_btTeam != this->m_btWinTeam)
	{
		return;
	}

	if (this->m_UserData[getGameObject(nIndex)->m_nITLIndex)->m_bReward == true)
	{
		return;
	}

	this->m_UserData[getGameObject(nIndex)->m_nITLIndex)->m_bReward = true;
	ItemCreate(nIndex, getGameObject(nIndex)->MapNumber, getGameObject(nIndex)->X, getGameObject(nIndex)->Y, ItemGetNumberMake(12, 30), 0, 0, 0, 0, 0, nIndex, 0, 0, 0, 0, 0);

	sLog->outBasic("[ ITL ] (%d) [DropChaosGem] [ItemCreate] Index:[%d],Name:[%s],Acc:[%s]", this->m_nTempleNumber + 1, nIndex, getGameObject(nIndex)->Name, getGameObject(nIndex)->AccountID);
}

void CIllusionTempleLeagueProcess::SendAllUserAnyMsg(char *lpMsg)
{
	PMSG_NOTICE pNotice;
	TNotice::MakeNoticeMsg(&pNotice, 0, lpMsg);

	for each (std::pair<int,CGameObject*> ObjEntry in gGameObjects)
	{
		if (getGameObject(i)->Connected == PLAYER_PLAYING && getGameObject(i)->Type == OBJ_USER)
		{
			if (!DG_MAP_RANGE(getGameObject(i)->MapNumber))
			{
				IOCP.DataSend(i, (BYTE*)&pNotice, pNotice.h.size);
			}
		}
	}
}

int CIllusionTempleLeagueProcess::GetRealRemainTimeMin()
{
	if (this->GetIllusionTempleLeagueState() != 0)
	{
		return 0;
	}

	return this->m_nRemainTimeMin;
}

BOOL CIllusionTempleLeagueProcess::CheckSkillProdection(int iIllusionTempleIndex)
{
	if (iIllusionTempleIndex < 0 || iIllusionTempleIndex > 9)
	{
		return FALSE;
	}

	if (this->m_UserData[iIllusionTempleIndex].m_nIndex != -1 && this->m_UserData[iIllusionTempleIndex].m_dwSkillProdectionTime != 0)
	{
		return TRUE;
	}

	else
	{
		return FALSE;
	}
}

BOOL CIllusionTempleLeagueProcess::CheckSkillRestraint(int iIllusionTempleIndex)
{
	if (iIllusionTempleIndex < 0 || iIllusionTempleIndex > 9)
	{
		return FALSE;
	}

	if (this->m_UserData[iIllusionTempleIndex].m_nIndex != -1 && this->m_UserData[iIllusionTempleIndex].m_dwSkillRestraintTime != 0)
	{
		return TRUE;
	}

	else
	{
		return FALSE;
	}
}

void CIllusionTempleLeagueProcess::SendITLResult()
{
	PMSG_ITL_USER_ADD_EXP pAddExpMsg;
	PMSG_ITL_RESULT pMsg;

	char SendByte[256];
	int nOffset = 0;

	pMsg.btUserCount = this->m_nUserCount;

	for (int i = 0; i < 2; i++)
	{
		if (this->m_GuildDataSave[i].byTeamIndex == 0)
		{
			std::memcpy(pMsg.szAlliedGuildName, this->m_GuildDataSave[i].szGName, MAX_GUILD_LEN + 1);
			gParty.ClearPartyAuthITL(this->m_GuildDataSave[i].nPartyNo);
		}

		else if (this->m_GuildDataSave[i].byTeamIndex == 1)
		{
			std::memcpy(pMsg.szIllusionGuildName, this->m_GuildDataSave[i].szGName, MAX_GUILD_LEN + 1);
			gParty.ClearPartyAuthITL(this->m_GuildDataSave[i].nPartyNo);
		}

		if (this->m_GuildDataSave[i].byWin == TRUE)
		{
			pMsg.byWin = this->m_GuildDataSave[i].byTeamIndex;
		}
	}

	BYTE byWIN_TEAM_INDEX = pMsg.byWin;
	nOffset += sizeof(pMsg);

	for (int i = 0; i < 10; i++)
	{
		if (this->m_UserData[i].m_nIndex != -1 && getGameObject(this->m_UserData[i]->m_nIndex)->Connected > PLAYER_LOGGED)
		{
			std::memcpy(pAddExpMsg.GameID, getGameObject(this->m_UserData[i]->m_nIndex)->Name, MAX_ACCOUNT_LEN);
			pAddExpMsg.btMapNumber = this->m_nTempleNumber + 98;
			pAddExpMsg.btTeam = this->m_UserData[i].m_btTeam;
			pAddExpMsg.btClass = getGameObject(this->m_UserData[i]->m_nIndex)->Class;
			pAddExpMsg.btUserKillCount = this->m_UserData[i].m_btUserKillCount;
			
			std::memcpy(&SendByte[nOffset], &pAddExpMsg, sizeof(pAddExpMsg));
			nOffset += sizeof(pAddExpMsg);

			sLog->outBasic("[ ITL ] (%d) ITL Result  (Account:%s, Name:%s, G.name:%s , Team:%d, Class:%d, UserKillCount:%d,InvalidationCount:%d,OccupiedCount:%d)",
				this->m_nTempleNumber, getGameObject(this->m_UserData[i]->m_nIndex]->AccountID, getGameObject(this->m_UserData[i]->m_nIndex]->Name, getGameObject(this->m_UserData[i)->m_nIndex)->m_PlayerData->GuildName,
				this->m_UserData[i].m_btTeam, getGameObject(this->m_UserData[i]->m_nIndex]->m_PlayerData->DbClass, this->m_UserData[i]->m_btUserKillCount, this->m_UserData[i)->m_byInvalidationCount,
				this->m_UserData[i].m_byOccupiedCount);

			BYTE byWinTeam = byWIN_TEAM_INDEX == this->m_UserData[i].m_btTeam;

			if (g_IllusionTempleLeagueEvent.m_nITL_TYPE == 1)
			{
				getGameObject(this->m_UserData[i]->m_nIndex)->m_byEnterITLUserCount++;
			}

			this->GDSaveITLUserPoint(this->m_UserData[i].m_nIndex, getGameObject(this->m_UserData[i]->m_nIndex]->Name, getGameObject(this->m_UserData[i)->m_nIndex)->m_PlayerData->GuildName,
				this->m_UserData[i].m_byOccupiedCount, this->m_UserData[i].m_btUserKillCount, this->m_UserData[i].m_byInvalidationCount,
				g_IllusionTempleLeagueEvent.m_nITL_TYPE, 1, byWinTeam);
		}
	}

	if (this->m_bNobodyWinLose == false)
	{
		for (int i = 0; i < 2; i++)
		{
			if (this->m_GuildDataSave[i].bDBSave == false)
			{
				this->GDSaveITLGuildPoint(this->m_GuildDataSave[i].szGName, this->m_GuildDataSave[i].byITLType, this->m_GuildDataSave[i].byWin,
					this->m_GuildDataSave[i].byLose, this->m_GuildDataSave[i].byOccupiedCount, 1);

				EnterCriticalSection(&this->m_criGuildData);
				this->m_GuildDataSave[i].bDBSave = true;
				LeaveCriticalSection(&this->m_criGuildData);

			}
		}
	}

	PHeadSubSetB((BYTE*)&pMsg, 0xBF, 0x64, nOffset);
	std::memcpy(SendByte, (BYTE*)&pMsg, sizeof(pMsg));

	for (int i = 0; i < 10; i++)
	{
		if (this->m_UserData[i].m_btTeam != 0xFF && this->m_UserData[i].m_nIndex != -1 && getGameObject(this->m_UserData[i]->m_nIndex)->Connected > PLAYER_LOGGED)
		{
			IOCP.DataSend(this->m_UserData[i].m_nIndex, (BYTE*)SendByte, pMsg.h.size);
		}
	}
}

BYTE CIllusionTempleLeagueProcess::Find_SameGuild(int nGuildIdx, int &nUserIndex)
{
	int i;

	if (nGuildIdx < 0)
	{
		return -1;
	}

	for (i = 0;; i++)
	{
		if (i >= 2)
		{
			return 1;
		}

		if (this->m_GuildDataSave[i].nGuild > 0 && this->m_GuildDataSave[i].nGuild == nGuildIdx)
		{
			break;
		}
	}

	if (this->m_GuildDataSave[i].nRequestUserIndex == nUserIndex)
	{
		return 2;
	}

	else
	{
		return 0;
	}
}

BYTE CIllusionTempleLeagueProcess::Find_GuildSlot(int nGuildIdx, BYTE &bySlotNum)
{
	if (nGuildIdx < 0)
	{
		return -1;
	}

	for (int i = 0; i < 2; i++)
	{
		if (this->m_GuildDataSave[i].nGuild == nGuildIdx)
		{
			bySlotNum = i;
			return 0;
		}
	}

	return -2;
}

BYTE CIllusionTempleLeagueProcess::IsEmptySlot(BYTE bySlotNum)
{
	if (this->m_GuildDataSave[bySlotNum].nGuild != -1)
	{
		return -2;
	}

	return 0;
}

BYTE CIllusionTempleLeagueProcess::Find_EmptyGuildSlot(int nGuildNum, BYTE &byEmptySlot)
{
	if (nGuildNum < 0)
	{
		return -1;
	}

	for (int i = 0; i < 2; i++)
	{
		if (this->m_GuildDataSave[i].nGuild <= 0)
		{
			byEmptySlot = i;
			return 0;
		}
	}

	return -2;
}

BYTE CIllusionTempleLeagueProcess::DeleteGuild(int nGuildIdx)
{
	for (int i = 0; i < 2; i++)
	{
		if (this->m_GuildDataSave[i].nGuild == nGuildIdx)
		{
			EnterCriticalSection(&this->m_criGuildData);

			this->m_GuildDataSave[i].bEntered = false;
			this->m_GuildDataSave[i].bRequest = false;
			this->m_GuildDataSave[i].byITLType = 0;
			this->m_GuildDataSave[i].nGuild = -1;
			this->m_GuildDataSave[i].nPartyNo = -1;
			memset(this->m_GuildDataSave[i].szGName, 0, MAX_GUILD_LEN + 1);
			this->m_GuildDataSave[i].bDBSave = false;
			this->m_GuildDataSave[i].byTeamIndex = -1;

			LeaveCriticalSection(&this->m_criGuildData);

			return 0;
		}
	}

	return 1;
}

void CIllusionTempleLeagueProcess::ClearGuildDataSave()
{
	EnterCriticalSection(&this->m_criGuildData);

	this->m_GuildDataSave[0].bEntered = false;
	this->m_GuildDataSave[0].bRequest = false;
	this->m_GuildDataSave[0].byITLType = 0;
	this->m_GuildDataSave[0].nGuild = -1;
	this->m_GuildDataSave[0].nPartyNo = -1;
	this->m_GuildDataSave[0].bDBSave = false;
	this->m_GuildDataSave[0].byTeamIndex = -1;
	memset(this->m_GuildDataSave[0].szGName, 0, MAX_GUILD_LEN + 1);

	LeaveCriticalSection(&this->m_criGuildData);
}

BYTE CIllusionTempleLeagueProcess::SetGuild(int nGuildIndex, BYTE bySlotOrder, BYTE byITLType, int nPartyIdx, int nUserIndex)
{
	if (nGuildIndex <= 0)
	{
		return -1;
	}

	GUILD_INFO_STRUCT *lpG = Guild.SearchGuild_Number(nGuildIndex);

	if (!lpG)
	{
		return -2;
	}

	if (bySlotOrder > 2)
	{
		return -3;
	}

	if (this->m_GuildDataSave[bySlotOrder].nGuild != -1)
	{
		return -4;
	}

	EnterCriticalSection(&this->m_criGuildData);

	this->m_GuildDataSave[bySlotOrder].nGuild = nGuildIndex;
	this->m_GuildDataSave[bySlotOrder].bRequest = true;
	this->m_GuildDataSave[bySlotOrder].byITLType = byITLType;
	this->m_GuildDataSave[bySlotOrder].nPartyNo = nPartyIdx;
	this->m_GuildDataSave[bySlotOrder].nRequestUserIndex = nUserIndex;
	std::memcpy(this->m_GuildDataSave[bySlotOrder].szGName, lpG->Name, MAX_GUILD_LEN + 1);
	
	LeaveCriticalSection(&this->m_criGuildData);
}

void CIllusionTempleLeagueProcess::GDSaveITLGuildPoint(char* GuildName, int Type, BYTE btWin, BYTE btLose, BYTE byOccCnt, BYTE byEnterCnt)
{
	PMSG_REQ_SAVE_ITL_GUILDPOINT pMsg;

	std::memcpy(pMsg.GuildName, GuildName, MAX_GUILD_LEN + 1);
	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xF0;
	pMsg.h.subcode = 0xA1;
	pMsg.byWin = btWin;
	pMsg.byLose = btLose;
	pMsg.nType = Type;
	pMsg.byOccCnt = byOccCnt;
	pMsg.byEnterCnt = byEnterCnt;

	wsDataCli.DataSend((char *)&pMsg, sizeof(pMsg));
	sLog->outBasic("[ ITL ] (%d) [GDSaveITLGuildPoint]GName:[%s],Type:[%d],Win:[%d],Lose:[%d],OccCnt:[%d],EnterCnt:[%d]",
		this->m_nTempleNumber + 1, GuildName, Type, btWin, btLose, byOccCnt, byEnterCnt);
}

void CIllusionTempleLeagueProcess::GDSaveITLUserPoint(int index, char* UserName, char* GuildName, int nOccupiedCount, int KillPoint, int nInvalidationCount, int nType, int nEnterCount, int nWin)
{
	PMSG_REQ_SAVE_ITL_USERPOINT pMsg;

	std::memcpy(pMsg.UserName, UserName, MAX_ACCOUNT_LEN + 1);
	std::memcpy(pMsg.GName, GuildName, MAX_GUILD_LEN + 1);

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xF0;
	pMsg.h.subcode = 0xA2;
	pMsg.nOccupiedCount = nOccupiedCount;
	pMsg.nKillPoint = KillPoint;
	pMsg.nInvalidationCount = nInvalidationCount;
	pMsg.nType = nType;
	pMsg.nEnterCount = nEnterCount;
	pMsg.nWin = nWin;

	sLog->outBasic("[ ITL ] (%d) [GDSaveITLUserPoint] Name:%s, G.Name:%s, OccCnt:%d, KillCnt:%d, InvalidationCnt:%d,Type:%d, EnterCnt:%d, Win:%d",
		this->m_nTempleNumber + 1, UserName, GuildName, nOccupiedCount, KillPoint, nInvalidationCount, nType, nEnterCount, nWin);

	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
}

BYTE CIllusionTempleLeagueProcess::FindSameNameIndex(char *Name)
{
	for (int i = 0; i < 10; i++)
	{
		if (!strcmp(this->m_EnteredPlayerName[i].PlayerName, Name))
		{
			return i;
		}
	}

	return -1;
}

void CIllusionTempleLeagueProcess::DeleteAllName()
{
	for (int i = 0; i < 10; i++)
	{
		this->m_EnteredPlayerName[i].EnteredIndex = -1;
		memset(this->m_EnteredPlayerName[i].PlayerName, 0, MAX_ACCOUNT_LEN + 1);
	}
}

void CIllusionTempleLeagueProcess::MoveToStartPoint_WhenRoundBreak()
{
	for (int i = 0; i < 10; i++)
	{
		if (this->m_UserData[i].m_nIndex != -1 &&
			getGameObject(this->m_UserData[i]->m_nIndex)->Connected > PLAYER_LOGGED &&
			getGameObject(this->m_UserData[i]->m_nIndex)->MapNumber == this->m_nTempleNumber + 98)
		{
			if (this->m_UserData[i].m_btTeam == 0)
			{
				gObjMoveGate(this->m_UserData[i].m_nIndex, this->m_nTempleNumber + 446);
			}

			else if (this->m_UserData[i].m_btTeam == 1)
			{
				gObjMoveGate(this->m_UserData[i].m_nIndex, this->m_nTempleNumber + 448);
			}

			sLog->outBasic("[ ITL ] (%d) [RoundBreak_Move2WaittingRoom]  Round [%d], UserIndex :[%d], Account:[%s] , Name :[%s] ",
				this->m_nTempleNumber + 1, this->m_byITLRound, this->m_UserData[i].m_nIndex, getGameObject(this->m_UserData[i]->m_nIndex]->AccountID, getGameObject(this->m_UserData[i)->m_nIndex)->Name);
		}
	}
}

BYTE CIllusionTempleLeagueProcess::FindGuildInfo(int nGuildIdx, BYTE &byTeam, int &nPartyIndex)
{
	for (int i = 0; i < 2; i++)
	{
		if (this->m_GuildDataSave[i].nGuild == nGuildIdx)
		{
			byTeam = this->m_GuildDataSave[i].byTeamIndex;
			nPartyIndex = this->m_GuildDataSave[i].nPartyNo;
			return i;
		}
	}

	return -1;
}

BOOL CIllusionTempleLeagueProcess::ReEnterUserIllusionTempleLeague(CGameObject &Obj, BYTE btTempleIndex, int nGuildIdx, int nPartyIdx, int nUserArrayNo, BYTE TeamIdx)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		sLog->outBasic("return %s %d", __FILE__, __LINE__);
		return FALSE;
	}

	if (this->GetIllusionTempleLeagueState() == 0 ||
		this->GetIllusionTempleLeagueState() == 3)
	{
		return FALSE;
	}

	if (Obj.m_nITLIndex != -1)
	{
		sLog->outBasic("RE Enter Error: IllusionTempleIndex Fail %d ", Obj.m_nITLIndex);
		return FALSE;
	}

	int nItemPos = this->SearchRelicsItem(Obj.m_Index);

	if (nItemPos >= 0 && Obj.pntInventory[nItemPos]->m_Type == ITEMGET(14, 223))
	{
		gObjInventoryDeleteItem(Obj.m_Index, nItemPos);
		GSProtocol.GCInventoryItemDeleteSend(Obj.m_Index, nItemPos, 0);
	}

	BOOL bResult = 0;
	EnterCriticalSection(&this->m_critUserData);

	for (int i = 0; i < 10; i++)
	{
		if (this->m_UserData[i].m_nIndex == -1 && nUserArrayNo == i)
		{
			this->m_UserData[i].m_nIndex = Obj.m_Index;
			this->m_nUserCount++;
			Obj.m_nITLIndex = i;
			
			bResult = TRUE;
			break;
		}
	}

	LeaveCriticalSection(&this->m_critUserData);

	if (TeamIdx == 0)
	{
		Obj.m_Change = 404;
	}

	else if (TeamIdx == 1)
	{
		Obj.m_Change = 405;
	}

	if (this->m_btIllusionTempleLeagueState == 1 || this->m_btIllusionTempleLeagueState == 2)
	{
		if (TeamIdx == 0)
		{
			Obj.MapNumber = btTempleIndex + 98;
			gObjMoveGate(Obj.m_Index, btTempleIndex + 446);
		}

		else if (TeamIdx == 1)
		{
			Obj.MapNumber = btTempleIndex + 98;
			gObjMoveGate(Obj.m_Index, btTempleIndex + 448);
		}
	}

	gObjViewportListProtocolCreate(getGameObject(Obj.m_Index));
	return bResult;
}

void CIllusionTempleLeagueProcess::CalcKillCountByTeam(BYTE &byAlliedKillSum, BYTE &byIllusionKillSum)
{
	byAlliedKillSum = 0;
	byIllusionKillSum = 0;

	for (int i = 0; i < 10; i++)
	{
		if (this->m_UserData[i].m_btTeam == 0)
		{
			byAlliedKillSum += this->m_UserData[i].m_btUserKillCount;
		}

		else if (this->m_UserData[i].m_btTeam == 1)
		{
			byIllusionKillSum += this->m_UserData[i].m_btUserKillCount;
		}
	}
}

void CIllusionTempleLeagueProcess::ResetUserFlag_DoingRelicsThing(CGameObject &Obj)
{
	if (!lpObj)
	{
		return;
	}

	sLog->outBasic("[ ITL ][ResetUserFlag_DoingRelicsThing] NAME:%s, StoneState:%d, NPCTYPE:%d, GettingRelics:%d, RegisteringRelics:%d",
		Obj.Name, Obj.m_byStoneState, Obj.m_wITLNpcType, Obj.m_bGettingRelics, Obj.m_bRegisteringRelics);

	Obj.m_byStoneState = 99;
	Obj.m_wITLNpcType = 0;
	Obj.m_bGettingRelics = 0;
	Obj.m_bRegisteringRelics = 0;
	Obj.m_nITLRelicsTick = 0;

}

void CIllusionTempleLeagueProcess::CancleStoneStateProcess(WORD wNpcType, BYTE byState)
{
	int nNpcIndex = -1;
	BYTE SendState = 99;
	BYTE byMapTagIndex = -1;

	if (wNpcType == 658)
	{
		this->m_Cursed_StoneStatus.m_byStatus = 0;
		nNpcIndex = this->m_Cursed_StoneStatus.m_nIndex;
		SendState = this->m_Cursed_StoneStatus.m_byStatus;
		byMapTagIndex = this->m_Cursed_StoneStatus.byMapTagIndex;
	}

	else if (wNpcType >= 659 && wNpcType <= 668)
	{
		BYTE byGetRelicsCase = byState % 10;
		BYTE CASE = -1;

		if (byState == 10 || byState == 11)
		{
			CASE = 0;
		}

		for (int i = 0; i < 10; i++)
		{
			if (this->m_Occupied_StoneStatus[i].m_wType == wNpcType)
			{
				if (CASE != 0)
				{
					this->m_Occupied_StoneStatus[i].m_byStatus = -1;
					SendState = this->m_Occupied_StoneStatus[i].m_byStatus;
					nNpcIndex = this->m_Occupied_StoneStatus[i].m_nIndex;
					byMapTagIndex = this->m_Occupied_StoneStatus[i].byMapTagIndex;
					break;
				}

				if (byGetRelicsCase == 0 || byGetRelicsCase == 1)
				{
					if (byGetRelicsCase == 0)
					{
						byGetRelicsCase = 1;
					}

					else if (byGetRelicsCase == 1)
					{
						byGetRelicsCase = 0;
					}

					this->m_Occupied_StoneStatus[i].m_byStatus = byGetRelicsCase;
					SendState = this->m_Occupied_StoneStatus[i].m_byStatus;
					nNpcIndex = this->m_Occupied_StoneStatus[i].m_nIndex;
					byMapTagIndex = this->m_Occupied_StoneStatus[i].byMapTagIndex;
					break;
				}
			}
		}
	}

	this->Send_ITL_StoneInfo(0, wNpcType, SendState, nNpcIndex, byMapTagIndex);
}

int CIllusionTempleLeagueProcess::GetRelicsUserIndex(CGameObject &Obj)
{
	if (!lpObj)
	{
		return -1;
	}

	if (this->m_UserData[this->FindITLUser(Obj.m_Index)].m_nRelicsInvenPos <= 0)
	{
		return -1;
	}

	return Obj.m_Index;
}

void CIllusionTempleLeagueProcess::ResetTimeVariables()
{
	this->m_nRemainTimeMin = -1;
	this->m_nRemainTimeMsec = -1;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

