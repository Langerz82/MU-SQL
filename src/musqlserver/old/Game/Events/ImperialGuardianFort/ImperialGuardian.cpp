// Imperial Guardian Decompilation
// Decompiled by [IGC] Dudas
// Status: 100%
// Last update: 02/04/2014

#include "StdAfx.h"
#include "ImperialGuardian.h"
#include "User/CUserData.h"
#include "Main.h"
#include "prodef.h"
#include "util.h"
#include "BuffEffectSlot.h"
#include "MonsterStatCalc.h"
#include "gObjMonster.h"
#include "MapClass.h"
#include "BloodCastle.h"
#include "GameProtocol.h"
#include "CashShop.h"
#include "MasterLevelSkillTreeSystem.h"
#include "PersonalStore.h"
#include "CustomMaxStats.h"

CImperialGuardian g_ImperialGuardian;

CImperialGuardian::CImperialGuardian()
{
	this->Init();
}

CImperialGuardian::~CImperialGuardian()
{
	if (this->pEventDungeonItemBag)
	{
		delete pEventDungeonItemBag;
		pEventDungeonItemBag = NULL;
	}

	if (this->pEventDungeonItemBagStatue)
	{
		delete pEventDungeonItemBagStatue;
		pEventDungeonItemBagStatue = NULL;
	}

	if (this->pEventDungeonItemBagGaion)
	{
		delete pEventDungeonItemBagGaion;
		pEventDungeonItemBagGaion = NULL;
	}
}

void CImperialGuardian::Init()
{
	this->m_nDayOfWeeksForCheat = -1;
	this->m_nMixNeedZen = -1;
	this->m_nCheatMode = 0;
	this->m_nDay = -1;
	this->m_nWeather = 0;
	this->m_bLoadScript = 0;
	this->m_bEnableEvent = 0;
	InitializeCriticalSection(&this->m_criUserIndex);
}

void CImperialGuardian::InitEventDungeon()
{
	for(int i=0;i<MAX_ZONE;i++)
	{
		this->InitZone(i);
	}
}

void CImperialGuardian::LoadScript(LPSTR szFileName)
{
	CIniReader ReadIF(szFileName);

	if (g_ConfigRead.server.GetStateFromEventTable(g_ConfigRead.server.GetServerType(), EV_TABLE_IF) == false)
	{
		this->m_bEnableEvent = false;
	}

	else
	{
		this->m_bEnableEvent = ReadIF.ReadInt("ImperialGuardian", "Enable", 1);
	}

	this->m_dwDefaultLootTime = ReadIF.ReadInt("ImperialGuardian", "LootTime", 20);
	this->m_dwDefaultWaitPlayerTime = ReadIF.ReadInt("ImperialGuardian", "StandByTime", 180);
	this->m_dwDefaultTimeAttackTime = ReadIF.ReadInt("ImperialGuardian", "RoundDurationTime", 600);
	this->m_nMixNeedZen = ReadIF.ReadInt("ImperialGuardian", "CompleteSecromiconMixReqMoney", 1000000);
	this->m_nPaperScrapDropRate = ReadIF.ReadInt("ImperialGuardian", "SuspiciousScrapOfPaperDropRate", 0);
	this->m_nPaperScrapDropLevel = ReadIF.ReadInt("ImperialGuardian", "SuspiciousScrapOfPaperMonsterMinLevelDrop", 32);
	this->m_nMaxEnterCount = ReadIF.ReadInt("ImperialGuardian", "MaxEnterCount", 4);

	this->m_rewardExp.LoadScript(g_ConfigRead.GetPath("\\Events\\ImperialGuardian\\IGC_ImperialGuardian_Reward_Exp.xml"));
	this->LoadMonsterScript(g_ConfigRead.GetPath("\\Events\\ImperialGuardian\\IGC_ImperialGuardian_Monster_Spawn.xml"));
	this->LoadItemBag();

	this->m_bLoadScript = true;

	if ( this->m_bLoadScript )
	{
		this->InitEventDungeon();
	}
}

void CImperialGuardian::LoadMonsterScript(LPSTR Filename)
{
	this->m_vtMonsterPosInfo.clear();

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(Filename);

	if (res.status != pugi::status_ok)
	{
		sLog->outError("File load error: %s (%s)", Filename, res.description());
		return;
	}

	_stIFMonsterPos stMonsterPosInfo;

	pugi::xml_node mainXML = file.child("ImperialGuardian");

	for (pugi::xml_node map = mainXML.child("Map"); map; map = map.next_sibling())
	{
		BYTE btMap = map.attribute("Number").as_int();
		BYTE btDay = map.attribute("Day").as_int();

		for (pugi::xml_node spawn = map.child("Spawn"); spawn; spawn = spawn.next_sibling())
		{
			stMonsterPosInfo.m_Type = spawn.attribute("Index").as_int();
			stMonsterPosInfo.m_Dis = spawn.attribute("Distance").as_int();
			stMonsterPosInfo.m_X = spawn.attribute("StartX").as_int();
			stMonsterPosInfo.m_Y = spawn.attribute("StartY").as_int();
			stMonsterPosInfo.m_Dir = spawn.attribute("Dir").as_int();
			stMonsterPosInfo.m_Zone = spawn.attribute("Zone").as_int();
			stMonsterPosInfo.m_MapNumber = btMap;
			stMonsterPosInfo.m_Day = btDay;

			this->m_vtMonsterPosInfo.push_back(stMonsterPosInfo);
		}
	}
}

void CImperialGuardian::LoadItemBag()
{
	if ( this->pEventDungeonItemBag )
	{
		delete pEventDungeonItemBag;
		pEventDungeonItemBag = NULL;
	}

	if ( this->pEventDungeonItemBagStatue )
	{
		delete pEventDungeonItemBagStatue;
		pEventDungeonItemBagStatue = NULL;
	}

	if ( this->pEventDungeonItemBagGaion )
	{
		delete pEventDungeonItemBagGaion;
		pEventDungeonItemBagGaion = NULL;
	}

	pEventDungeonItemBag = new CEventDungeonItemBag;
	pEventDungeonItemBagStatue = new CEventDungeonItemBag;
	pEventDungeonItemBagGaion = new CEventDungeonItemBag;

	this->pEventDungeonItemBag->LoadScript(g_ConfigRead.GetPath("\\Events\\ImperialGuardian\\IGC_ImperialGuardian_Drop_General.xml"));
	this->pEventDungeonItemBagGaion->LoadScript(g_ConfigRead.GetPath("\\Events\\ImperialGuardian\\IGC_ImperialGuardian_Drop_Gaion.xml"));
	this->pEventDungeonItemBagStatue->LoadScript(g_ConfigRead.GetPath("\\Events\\ImperialGuardian\\IGC_ImperialGuardian_Drop_Statue.xml"));
}

void CImperialGuardian::ImperialGuardianProcessInit()
{
	if (g_ConfigRead.server.GetStateFromEventTable(g_ConfigRead.server.GetServerType(), EV_TABLE_IF) == false)
	{
		this->m_bEnableEvent = false;
	}
}

void CImperialGuardian::Run()
{
	if ( !this->m_bEnableEvent )
	{
		return;
	}

	for ( int i=0;i<MAX_ZONE;i++ )
	{
		switch ( this->m_stZoneInfo[i].m_nZoneState )
		{
			case 0:
				this->ProcReady(i);
				break;
			case 1:
				this->ProcBeginTimeAttack(i);
				break;
			case 2:
				this->ProcBeginWaitPlayer(i);
				break;
			case 3:
				this->ProcBeginLootTime(i);
				break;
			case 4:
				this->ProcAllWarpNextZone(i);
				break;
			case 5:
				this->ProcMissionFail(i);
				break;
			case 6:
				this->ProcAllKick(i);
				break;
			case 7:
				this->ProcMissionClear(i);
				break;
			default:
				break;
		}

		this->ProcCheckDungeon(i);
	}

	this->KickInvalidUser();
}

void CImperialGuardian::ProcReady(int nZoneIndex)
{
	if ( nZoneIndex < 0 || nZoneIndex > 3 )
	{
		return;
	}

	if ( this->GetPlayerCount(nZoneIndex) > 0 )
	{
		this->SetZoneState(nZoneIndex, 6);
	}
}

void CImperialGuardian::ProcBeginTimeAttack(int nZoneIndex)
{
	if ( nZoneIndex < 0 || nZoneIndex > 3 )
	{
		return;
	}

	if ( this->m_stZoneInfo[nZoneIndex].m_bGoTimeAttack ) 
	{
		DWORD iTickSec = GetTickCount() - this->m_stZoneInfo[nZoneIndex].m_dwTickCount[2];

		if ( !this->m_stZoneInfo[nZoneIndex].m_bSendWarningMsg && this->m_stZoneInfo[nZoneIndex].m_dwRemainTimeAttack <= 60000 )
		{
			char szMsg[255];
			memset(szMsg, 0x00, sizeof(szMsg));

			wsprintf(szMsg, Lang.GetText(0,340));
			this->GCSendServerMsgAll(nZoneIndex, szMsg);
			this->m_stZoneInfo[nZoneIndex].m_bSendWarningMsg = true;
		}

		if ( iTickSec >= 1000 )
		{
			if ( this->m_stZoneInfo[nZoneIndex].m_dwRemainTimeAttack > iTickSec )
				this->m_stZoneInfo[nZoneIndex].m_dwRemainTimeAttack -= iTickSec;
			else
				this->m_stZoneInfo[nZoneIndex].m_dwRemainTimeAttack = 0;

			this->m_stZoneInfo[nZoneIndex].m_dwTickCount[2] = GetTickCount();

			this->GCNotifyRemainTickCount(nZoneIndex, 2, this->m_stZoneInfo[nZoneIndex].m_dwRemainTimeAttack);
		}

		if ( this->IsLastZone(nZoneIndex) == true )
		{
			if ( this->GetLiveMonsterCount(nZoneIndex) < 1 )
			{
				this->m_stZoneInfo[nZoneIndex].m_dwRemainTimeAttack = 0;
			}
		}

		else if ( this->m_stZoneInfo[nZoneIndex].m_dwRemainTimeAttack > 60000 )
		{
			char szMsg[255];
			memset(szMsg, 0x00, sizeof(szMsg));

			DWORD nRemainMin = this->m_stZoneInfo[nZoneIndex].m_dwRemainTimeAttack / 60000 + 1;

			if ( nRemainMin != this->m_stZoneInfo[nZoneIndex].m_nSendRemainCreatePortal )
			{
				wsprintf(szMsg, Lang.GetText(0,341), nRemainMin);
				this->GCSendServerMsgAll(nZoneIndex, szMsg);

				this->m_stZoneInfo[nZoneIndex].m_nSendRemainCreatePortal = nRemainMin;
			}
		}

		if ( this->m_stZoneInfo[nZoneIndex].m_dwRemainTimeAttack == 0 )
		{
			this->m_stZoneInfo[nZoneIndex].m_bGoTimeAttack = false;

			if ( this->GetLiveMonsterCount(nZoneIndex) >= 1 )
			{
				this->SetZoneState(nZoneIndex, 5);
				this->DeleteMonster(nZoneIndex, this->m_stZoneInfo[nZoneIndex].m_nMonsterRegenTableIndex);
			}

			else
			{
				if ( this->IsLastZone(nZoneIndex) == true )
				{
					this->SetZoneState(nZoneIndex, 7);
				}

				else
				{
					this->SetZoneState(nZoneIndex, 3);
				}

				this->m_stZoneInfo[nZoneIndex].m_bAblePortal = true;
			}

		}
	}

	else
	{
		this->m_stZoneInfo[nZoneIndex].m_dwTickCount[2] = GetTickCount();
		this->m_stZoneInfo[nZoneIndex].m_bGoTimeAttack = true;

		char szMsg[255];
		memset(szMsg, 0x00, sizeof(szMsg));

		wsprintf(szMsg, Lang.GetText(0,342));

		this->GCSendServerMsgAll(nZoneIndex, szMsg);
		this->SetAtackAbleState(nZoneIndex, 525, true);
		this->SetAtackAbleState(nZoneIndex, 528, true);
		this->RegenMonster(nZoneIndex, this->m_stZoneInfo[nZoneIndex].m_nMonsterRegenTableIndex, this->m_stZoneInfo[nZoneIndex].m_nMaxUserLevel, false);
		this->GCNotifyRemainTickCount(nZoneIndex, 2, this->m_stZoneInfo[nZoneIndex].m_dwRemainTimeAttack);

	}
}

void CImperialGuardian::ProcBeginLootTime(int nZoneIndex)
{
	if ( nZoneIndex < 0 || nZoneIndex > 3 )
	{
		return;
	}

	if ( this->m_stZoneInfo[nZoneIndex].m_bGoLooting == true )
	{
		DWORD iTickSec = GetTickCount() - this->m_stZoneInfo[nZoneIndex].m_dwTickCount[1];

		if ( iTickSec >= 1000 )
		{
			if ( this->m_stZoneInfo[nZoneIndex].m_dwRemainLootTime > iTickSec )
				this->m_stZoneInfo[nZoneIndex].m_dwRemainLootTime -= iTickSec;
			else
				this->m_stZoneInfo[nZoneIndex].m_dwRemainLootTime = 0;
			
			this->m_stZoneInfo[nZoneIndex].m_dwTickCount[1] = GetTickCount();
			this->GCNotifyRemainTickCount(nZoneIndex, 0, this->m_stZoneInfo[nZoneIndex].m_dwRemainLootTime);
		}

		if ( this->m_stZoneInfo[nZoneIndex].m_dwRemainLootTime == 0 )
		{
			this->m_stZoneInfo[nZoneIndex].m_bGoLooting = false;
			
			if ( this->IsLastZone(nZoneIndex) == true )
				this->SetZoneState(nZoneIndex, 6);
			else
				this->SetZoneState(nZoneIndex, 5);

			this->DeleteMonster(nZoneIndex, this->m_stZoneInfo[nZoneIndex].m_nMonsterRegenTableIndex);
		}
	}

	else
	{
		this->m_stZoneInfo[nZoneIndex].m_dwTickCount[1] = GetTickCount();
		this->m_stZoneInfo[nZoneIndex].m_bGoLooting = true;
		this->GCNotifyRemainTickCount(nZoneIndex, 0, this->m_stZoneInfo[nZoneIndex].m_dwRemainLootTime);

		this->GCNotifyRemainTickCount(nZoneIndex, 0, this->m_stZoneInfo[nZoneIndex].m_dwRemainLootTime);

		if ( this->IsLastZone(nZoneIndex) == false )
		{
			char szText[255];
			memset(szText, 0, sizeof(szText));

			wsprintf(szText, Lang.GetText(0,343));
			this->GCSendServerMsgAll(nZoneIndex, szText);

			wsprintf(szText, Lang.GetText(0,344));
			this->GCSendServerMsgAll(nZoneIndex, szText);
		}
	}
}

void CImperialGuardian::ProcBeginWaitPlayer(int nZoneIndex)
{
	if ( nZoneIndex < 0 || nZoneIndex > 3 )
	{
		return;
	}

	if ( this->m_stZoneInfo[nZoneIndex].m_bGoWaitPlayer == true )
	{
		DWORD iTickSec = GetTickCount() - this->m_stZoneInfo[nZoneIndex].m_dwTickCount[0];

		if ( iTickSec >= 1000 )
		{
			if ( this->m_stZoneInfo[nZoneIndex].m_dwRemainWaitPlayerTime > iTickSec )
				this->m_stZoneInfo[nZoneIndex].m_dwRemainWaitPlayerTime -= iTickSec;
			else
				this->m_stZoneInfo[nZoneIndex].m_dwRemainWaitPlayerTime = 0;

			this->m_stZoneInfo[nZoneIndex].m_dwTickCount[0] = GetTickCount();
			this->GCNotifyRemainTickCount(nZoneIndex, 1, this->m_stZoneInfo[nZoneIndex].m_dwRemainWaitPlayerTime);
		}

		if ( !this->m_stZoneInfo[nZoneIndex].m_dwRemainWaitPlayerTime )
			this->SetZoneState(nZoneIndex, 1);
	}

	else
	{
		this->m_stZoneInfo[nZoneIndex].m_dwTickCount[0] = GetTickCount();
		this->m_stZoneInfo[nZoneIndex].m_bGoWaitPlayer = true;

		this->GCNotifyRemainTickCount(nZoneIndex, 1, this->m_stZoneInfo[nZoneIndex].m_dwRemainWaitPlayerTime);

		DWORD nRemainMin = this->m_stZoneInfo[nZoneIndex].m_dwRemainWaitPlayerTime / 60000;

		char szText[255];
		memset(szText, 0x00, sizeof(szText));

		wsprintf(szText, Lang.GetText(0,345), nRemainMin);
		this->GCSendServerMsgAll(nZoneIndex, szText);
	}
}

void CImperialGuardian::ProcAllWarpNextZone(int nZoneIndex)
{
	if ( nZoneIndex < 0 || nZoneIndex > 3 )
	{
		sLog->outBasic("error : %s %d", __FILE__, __LINE__);
		return;
	}

	int nPartyNumber = this->m_stZoneInfo[nZoneIndex].m_nPartyNumber;
	int nPartyCount = gParty.m_PartyS[nPartyNumber].Count;

	int nUserIndex[5];
	int nCount = 0;

	EnterCriticalSection(&this->m_criUserIndex);

	if ( (nZoneIndex+1) < 4 )
	{
		for(std::vector<int>::iterator it = this->m_stZoneInfo[nZoneIndex].m_vtUserIndex.begin(); it != this->m_stZoneInfo[nZoneIndex].m_vtUserIndex.end(); it++)
		{
			if ( gGameObjects[*it].Connected > 0)
			{
				nUserIndex[nCount] = *it;
				nCount++;
			}
		}
	}

	else
	{
		for(std::vector<int>::iterator it = this->m_stZoneInfo[nZoneIndex].m_vtUserIndex.begin(); it != this->m_stZoneInfo[nZoneIndex].m_vtUserIndex.end(); it++)
		{
			nUserIndex[nCount] = *it;
			nCount++;
		}
	}

	LeaveCriticalSection(&this->m_criUserIndex);

	for (int i = 0; i < nCount; i++)
	{
		gObjMoveGate(nUserIndex[i], 22);
	}
}

void CImperialGuardian::ProcAllKick(int nZoneIndex)
{
	if ( nZoneIndex < 0 || nZoneIndex > 3 )
	{
		sLog->outBasic("error : %s %d", __FILE__, __LINE__);
		return;
	}

	int nUserIndex[5];
	int nCount = 0;

	EnterCriticalSection(&this->m_criUserIndex);

	for(std::vector<int>::iterator it = this->m_stZoneInfo[nZoneIndex].m_vtUserIndex.begin(); it != this->m_stZoneInfo[nZoneIndex].m_vtUserIndex.end(); it++)
	{
		if ( gGameObjects[*it].Connected > 0 )
		{
			nUserIndex[nCount] = *it;
			nCount++;
		}
	}	
	
	LeaveCriticalSection(&this->m_criUserIndex);

	for (int i = 0; i < nCount; i++)
	{			
		gObjMoveGate(nUserIndex[i], 22);
		sLog->outBasic("[IMPERIALGUARDIAN] Leave Player Zone -> [ZONE]:%d [AccountID]:%s [Name]:%s", nZoneIndex+1,
			gGameObjects[nUserIndex[i]].AccountID, gGameObjects[nUserIndex[i]].Name);
	}

	this->InitZone(nZoneIndex);
	this->SetZoneState(nZoneIndex, 0);
	sLog->outBasic("[IMPERIALGUARDIAN] ALL KICK USER -> [ZONE]:%d", nZoneIndex+1);
}

void CImperialGuardian::ProcMissionFail(int nZoneIndex)
{
	if ( nZoneIndex < 0 || nZoneIndex > 3 )
	{
		sLog->outBasic("error : %s %d", __FILE__, __LINE__);
		return;
	}

	this->GCMissionFail(nZoneIndex);
	sLog->outBasic("[IMPERIALGUARDIAN] MISSION FAIL -> [ZONE]:%d", nZoneIndex+1);
	this->SetZoneState(nZoneIndex, 6);
}

void CImperialGuardian::ProcMissionClear(int nZoneIndex)
{
	this->GCNotifyAllZoneClear(nZoneIndex);
	sLog->outBasic("[IMPERIALGUARDIAN] SUCCESS MISSION CLEAR  -> [ZONE]:%d", nZoneIndex+1);

	EnterCriticalSection(&this->m_criUserIndex);

	for(std::vector<int>::iterator it = this->m_stZoneInfo[nZoneIndex].m_vtUserIndex.begin(); it != this->m_stZoneInfo[nZoneIndex].m_vtUserIndex.end(); it++)
	{
		if ( gGameObjects[*it].Connected > 0 )
		{
			if ( IMPERIAL_MAP_RANGE(gGameObjects[*it].MapNumber) == TRUE )
			{
				sLog->outBasic("[IMPERIALGUARDIAN] MISSION CLEAR USER ->[AccountID]:%s [Name]:%s", gGameObjects[*it].AccountID, gGameObjects[*it].Name);
			}
		}
	}

	LeaveCriticalSection(&this->m_criUserIndex);

	this->SetZoneState(nZoneIndex, 3);
}

bool CImperialGuardian::IsEmptyZone(int nZoneIndex)
{
	if ( nZoneIndex < 0 || nZoneIndex > 3 )
	{
		return false;
	}

	if ( this->m_stZoneInfo[nZoneIndex].m_nZoneState )
	{
		return false;
	}
	if ( this->GetPlayerCount(nZoneIndex) )
	{
		return false;
	}

	return true;
}

void CImperialGuardian::KickInvalidUser()
{
	for(int i=g_ConfigRead.server.GetObjectStartUserIndex();i<g_ConfigRead.server.GetObjectMax();i++)
	{
		if ( gGameObjects[i].Connected == PLAYER_PLAYING && gGameObjects[i].Type == OBJ_USER && IMPERIAL_MAP_RANGE(gGameObjects[i].MapNumber) )
		{
			if ( (gGameObjects[i].Authority & 2) != 2 && (gGameObjects[i].Authority & 0x20) != 0x20 )
			{
				if ( this->CheckValidUser(i) )
				{
					if ( this->m_nCheatMode == 0 && gGameObjects[i].PartyNumber < 0 )
					{
						gObjMoveGate(i, 22);
						sLog->outBasic("[IMPERIALGUARDIAN][ Invalid User ] User is not a party [%s][%s]", gGameObjects[i].AccountID, gGameObjects[i].Name);
					}
				}

				else if ( gGameObjects[i].RegenOk == FALSE && gGameObjects[i].m_State == 2 )
				{
					if ( gGameObjects[i].Live == TRUE )
					{
						gObjMoveGate(i, 22);
						sLog->outBasic("[IMPERIALGUARDIAN][ Invalid User ] [%s][%s]", gGameObjects[i].AccountID, gGameObjects[i].Name);
					}
				}
			}			
		}

		else
		{
			if ( this->CheckValidUser(i) == true )
			{
				this->RemoverUserInAllZone(i);
			}
		}
	}

}

bool CImperialGuardian::CheckValidUser(int nUserNumber)
{
	return this->GetCurrentZoneIndex(nUserNumber) > -1;
}

int CImperialGuardian::GetZoneState(int nZoneIndex)
{
	if ( nZoneIndex < 0 || nZoneIndex > 3 )
	{
		return -1;
	}

	return this->m_stZoneInfo[nZoneIndex].m_nZoneState;
}

bool CImperialGuardian::SetZoneState(int nZoneIndex, int nState)
{
	if ( nZoneIndex < 0 || nZoneIndex > 3 )
	{
		return false;
	}

	if ( nState > 8 )
	{
		return false;
	}

	this->m_stZoneInfo[nZoneIndex].m_nZoneState = nState;
	return true;
}

void CImperialGuardian::CGEnterPortal(int nUserIndex, int nDestZoneIndex)
{
	if ( !ObjectMaxRange(nUserIndex) )
	{
		return;
	}

	if ( nDestZoneIndex < 0 || nDestZoneIndex > 3 )
	{
		return;
	}

	CGameObject lpObj = &gGameObjects[nUserIndex];

	if ( lpObj.Type != OBJ_USER )
	{
		return;
	}

	if ( lpObj.Level < 150 )
	{
		PMSG_ENTER_ZONE_RESULT pMsg;

		pMsg.m_btDayOfWeek = 0;
		pMsg.m_dwRemainTime = 0;
		pMsg.m_btZoneIndex = 0;
		pMsg.m_btWeather = 0;
		
		pMsg.m_btResult = 6;
		PHeadSubSetB((LPBYTE)&pMsg, 0xF7, 0x02, sizeof(PMSG_ENTER_ZONE_RESULT));

		IOCP.DataSend(nUserIndex, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	if ( lpObj.m_bPShopOpen == true )
		g_PersonalStore.CGPShopReqClose(nUserIndex);

	int nEnterItemPos = -1;

	PMSG_ENTER_ZONE_RESULT pMsg;

	pMsg.m_btDayOfWeek = 0;
	pMsg.m_btWeather = 0;		
	pMsg.m_btResult = 0;
		
	PHeadSubSetB((LPBYTE)&pMsg, 0xF7, 0x02, sizeof(PMSG_ENTER_ZONE_RESULT));

	pMsg.m_btZoneIndex = nDestZoneIndex+1;
	pMsg.m_dwRemainTime = this->m_stZoneInfo[nDestZoneIndex].m_dwRemainWaitPlayerTime + this->m_stZoneInfo[nDestZoneIndex].m_dwRemainLootTime + this->m_stZoneInfo[nDestZoneIndex].m_dwRemainTimeAttack;

	if ( this->m_bEnableEvent == false )
	{
		return;
	}

	SYSTEMTIME time;
	GetLocalTime(&time);

	int nDayOfWeek;

	if ( this->m_nCheatMode && this->m_nDayOfWeeksForCheat > -1 )
	{
		nDayOfWeek = this->m_nDayOfWeeksForCheat;
	}

	else
	{
		nDayOfWeek = time.wDayOfWeek;
	}

	if ( lpObj.PartyNumber < 0 )
	{
		pMsg.m_btResult = 5;
		IOCP.DataSend(nUserIndex, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	if ( this->m_nCheatMode != TRUE )
	{
		if ( gParty.m_PartyS[lpObj.PartyNumber].Count < 1 )
		{
			pMsg.m_btResult = 5;
			IOCP.DataSend(nUserIndex, (LPBYTE)&pMsg, pMsg.h.size);
			return;
		}

		if ( !nDestZoneIndex )
		{
			if ( nDayOfWeek == 0 ) // sunday
			{
				nEnterItemPos = this->CheckFullSecromicon(nUserIndex);

				if ( nEnterItemPos < 0 && nEnterItemPos == -1 )
				{
					pMsg.m_btResult = 2;
					this->GCSendDataToUser(nUserIndex, (char*)&pMsg, pMsg.h.size);
					return;
				}
			}

			else
			{
				nEnterItemPos = this->CheckGaionOrderPaper(nUserIndex);

				if ( nEnterItemPos < 0 && nEnterItemPos == -1 )
				{
					pMsg.m_btResult = 2;
					this->GCSendDataToUser(nUserIndex, (char *)&pMsg, pMsg.h.size);
					return;
				}
			}
		}
	}

	if ( this->m_nCheatMode == TRUE || nDestZoneIndex <= 0 || this->GetCurrentZoneIndex(nUserIndex) == (nDestZoneIndex-1) )
	{
		if ( this->GetZoneState(nDestZoneIndex) && this->GetZoneState(nDestZoneIndex) != 2 )
		{
			char szMsg[256];
			wsprintf(szMsg, Lang.GetText(0,346));
			GSProtocol.GCServerMsgStringSend(szMsg, nUserIndex, 1);

			if ( nDestZoneIndex )
			{
				this->RollBackUserPos(nUserIndex);
			}

			else
			{
				pMsg.m_btResult = 1;
				this->GCSendDataToUser(nUserIndex, (char *)&pMsg, pMsg.h.size);
			}
		}

		else
		{
			if ( !nDestZoneIndex && !this->m_nCheatMode )
			{
				bool bFlag = false;

				for( int i=1;i<4 ;i++ )
				{
					if ( this->m_stZoneInfo[i].m_nPartyNumber == lpObj.PartyNumber )
					{
						bFlag = true;
						break;
					}
				}

				if ( bFlag )
				{
					char szMsg[256];
					wsprintf(szMsg, Lang.GetText(0,347));
					GSProtocol.GCServerMsgStringSend(szMsg, nUserIndex, 1);

					pMsg.m_btResult = 1;
					this->GCSendDataToUser(nUserIndex, (char *)&pMsg, pMsg.h.size);
					return;
				}
			}

			pMsg.m_btDayOfWeek = nDayOfWeek;

			if( nDayOfWeek == 0 )
			{
				pMsg.m_btDayOfWeek = 7;
			}

			if ( this->m_stZoneInfo[nDestZoneIndex].m_nPartyNumber >= 0 ||
				this->m_stZoneInfo[nDestZoneIndex].m_vtUserIndex.size() >= 1 )
			{
				if ( this->m_stZoneInfo[nDestZoneIndex].m_nPartyNumber != lpObj.PartyNumber )
				{
					char szMsg[256];
					wsprintf(szMsg, Lang.GetText(0,348));
					GSProtocol.GCServerMsgStringSend(szMsg, nUserIndex, 1);

					pMsg.m_btResult = 1;
					this->GCSendDataToUser(nUserIndex, (char *)&pMsg, pMsg.h.size);
					return;
				}

				if ( !this->m_nCheatMode && !nDestZoneIndex && !this->IsRegPartyUser(0, nUserIndex) )
				{
					char szMsg[256];
					wsprintf(szMsg, Lang.GetText(0,348));
					GSProtocol.GCServerMsgStringSend(szMsg, nUserIndex, 1);

					pMsg.m_btResult = 1;
					this->GCSendDataToUser(nUserIndex, (char *)&pMsg, pMsg.h.size);
					return;
				}

				if ( this->GetPlayerCount(nDestZoneIndex) > 5 )
				{
					char szMsg[256];
					wsprintf(szMsg, Lang.GetText(0,350));
					GSProtocol.GCServerMsgStringSend(szMsg, nUserIndex, 1);

					pMsg.m_btResult = 3;
					this->GCSendDataToUser(nUserIndex, (char *)&pMsg, pMsg.h.size);
					return;
				}
			}

			else
			{
				if ( nDestZoneIndex > 0 && this->m_nCheatMode != TRUE && !this->m_stZoneInfo[nDestZoneIndex-1].m_bAblePortal )
				{
					char szMsg[256];
					wsprintf(szMsg, Lang.GetText(0,351));
					GSProtocol.GCServerMsgStringSend(szMsg, nUserIndex, 1);

					this->RollBackUserPos(nUserIndex);
					return;
				}

				this->m_stZoneInfo[nDestZoneIndex].m_nPartyNumber = lpObj.PartyNumber;

				if ( nDestZoneIndex )
				{
					if ( this->m_stZoneInfo[nDestZoneIndex-1].m_nMonsterRegenTableIndex < 0 || this->m_nCheatMode )
					{
						this->m_stZoneInfo[nDestZoneIndex].m_nMonsterRegenTableIndex = nDayOfWeek;
					}

					else
					{
						this->m_stZoneInfo[nDestZoneIndex].m_nMonsterRegenTableIndex = this->m_stZoneInfo[nDestZoneIndex-1].m_nMonsterRegenTableIndex;
						this->m_stZoneInfo[nDestZoneIndex].m_nMaxUserLevel = this->m_stZoneInfo[nDestZoneIndex-1].m_nMaxUserLevel;
					}
				}

				else
				{
					this->m_stZoneInfo[0].m_nMonsterRegenTableIndex = nDayOfWeek;
				}

				int nMaxLevel;

				if ( gParty.m_PartyS[this->m_stZoneInfo[nDestZoneIndex].m_nPartyNumber].m_MaxLevel )
				{
					nMaxLevel = gGetPartyMaxLevel(this->m_stZoneInfo[nDestZoneIndex].m_nPartyNumber);
				}
				
				else
				{
					nMaxLevel = lpObj.Level + lpObj.m_PlayerData->MasterLevel;
				}

				this->m_stZoneInfo[nDestZoneIndex].m_nMaxUserLevel = nMaxLevel;

				this->RegAllPartyUser(nDestZoneIndex, nUserIndex);
				this->RegenMonster(nDestZoneIndex, this->m_stZoneInfo[nDestZoneIndex].m_nMonsterRegenTableIndex, nMaxLevel, true);
				this->SetZoneState(nDestZoneIndex, 2);
			}

			pMsg.m_btWeather = this->m_nWeather;

			if ( nDestZoneIndex > 0 )
			{
				this->RemoveUserInZone(nDestZoneIndex-1, nUserIndex);
			}
			
			int nGateIndex = this->GetGateNumber(nDestZoneIndex, this->m_stZoneInfo[nDestZoneIndex].m_nMonsterRegenTableIndex);

			if ( nGateIndex != -1 )
			{
				this->AddUserInZone(nDestZoneIndex, nUserIndex);

				pMsg.m_btResult = 0;
				this->GCSendDataToUser(nUserIndex, (char*)&pMsg, pMsg.h.size);

				if ( gObjCheckUsedBuffEffect(lpObj, 3) == TRUE )
				{
					gObjRemoveBuffEffect(lpObj, 3);
				}

				gObjMoveGate(nUserIndex, nGateIndex);

				if ( !this->m_nCheatMode && !nDestZoneIndex && nEnterItemPos >= 0 )
				{
					if ( lpObj.pInventory[nEnterItemPos].m_Type != ITEMGET(14,102) &&
						lpObj.pInventory[nEnterItemPos].m_Type != ITEMGET(14,109) )
					{
						if ( lpObj.pInventory[nEnterItemPos].m_Type == ITEMGET(13,126) ||
							lpObj.pInventory[nEnterItemPos].m_Type == ITEMGET(13,127) )
						{
							lpObj.pInventory[nEnterItemPos].m_Durability -= 1.0;

							if ( lpObj.pInventory[nEnterItemPos].m_Durability > 0.0 )
							{
								GSProtocol.GCItemDurSend2(nUserIndex, nEnterItemPos, lpObj.pInventory[nEnterItemPos].m_Durability, 0);
							}

							else
							{
								gObjInventoryDeleteItem(nUserIndex, nEnterItemPos);
								GSProtocol.GCInventoryItemDeleteSend(nUserIndex, nEnterItemPos, 1);
							}
						}
					}

					else
					{
						gObjInventoryDeleteItem(nUserIndex, nEnterItemPos);
						GSProtocol.GCInventoryItemDeleteSend(nUserIndex, nEnterItemPos, 1);
					}
				}

				sLog->outBasic("[IMPERIALGUARDIAN] Enter Zone User -> [AccountID]:%s [NAME]:%s [ZONE]:%d", lpObj.AccountID, lpObj.Name, nDestZoneIndex+1);
				sLog->outBasic("[IMPERIALGUARDIAN] [%d ZONE USER COUNT] : %d", nDestZoneIndex+1, this->GetPlayerCount(nDestZoneIndex));
			}
		}
	}
}

void CImperialGuardian::RegenMonster(int nZoneIndex, int nMonsterRegenTableIndex, int nMaxUserLevel, bool bOnlyCreateGate)
{
	if (nZoneIndex < 0 || nZoneIndex > 3)
	{
		sLog->outBasic("[IMPERIALGUARDIAN][RegenMonsterFunc] Invalid zone index => %d", nZoneIndex + 1);
		return;
	}

	if (nMonsterRegenTableIndex < 0 || nMonsterRegenTableIndex > 6)
	{
		sLog->outBasic("[IMPERIALGUARDIAN][RegenMonsterFunc] Invalid MonsterRegenTable Index => %d", nMonsterRegenTableIndex);
		return;
	}

	int nMaxLevel = gParty.m_PartyS[this->m_stZoneInfo[nZoneIndex].m_nPartyNumber].m_MaxLevel;

	if (nMaxUserLevel)
	{
		nMaxLevel = nMaxUserLevel;
	}

	for (std::vector<_stIFMonsterPos>::iterator It = this->m_vtMonsterPosInfo.begin(); It != this->m_vtMonsterPosInfo.end(); It++)
	{
		if (It->m_Zone == nZoneIndex && It->m_Day == nMonsterRegenTableIndex)
		{
			if (bOnlyCreateGate)
			{
				if (It->m_Type < 524 || It->m_Type > 528)
					continue;
			}

			else
			{
				if (It->m_Type >= 524 && It->m_Type <= 528)
					continue;
			}

			int nResult = gObjAddMonster(It->m_MapNumber);

			if (nResult < 0)
			{
				sLog->outBasic("[IMPERIALGUARDIAN][RegenMonsterFunc] gObjAddMonster fail (ErrorCode: %d, MonsterClass: %d)",
					nResult, It->m_Type);

				return;
			}

			if (this->SetPosMonster(nResult, It) == FALSE)
			{
				gObjDel(nResult);
				sLog->outBasic("error : %s %d", ".\\ImperialGuardian.cpp", __FILE__, __LINE__);
				return;
			}

			if (this->SetMonster(nResult, It->m_Type, nMaxLevel) == FALSE)
			{
				gObjDel(nResult);
				sLog->outBasic("error : %s %d", ".\\ImperialGuardian.cpp", __FILE__, __LINE__);
				return;
			}

			gGameObjects[nResult].m_nZoneIndex = nZoneIndex;

			_stMonsterIndexInfo stMonsterIndex;

			if (It->m_Type < 523 || It->m_Type > 528)
			{
				stMonsterIndex.m_bLive = true;
				stMonsterIndex.m_bAttackAble = true;
				stMonsterIndex.m_nIndex = nResult;

				this->m_stZoneInfo[nZoneIndex].m_bStartRegenMonster = true;

				sLog->outBasic("[IMPERIALGUARDIAN] AddMonster => %s, MapNumber => %d ,Zone => %d PosX => %d, PosY => %d, Connected => %d, STATE => %d",
					gGameObjects[nResult].Name, gGameObjects[nResult].MapNumber, gGameObjects[nResult].m_nZoneIndex, gGameObjects[nResult].X, gGameObjects[nResult].Y, gGameObjects[nResult].Connected, gGameObjects[nResult].m_State);
			}

			else
			{
				stMonsterIndex.m_bLive = false;
				stMonsterIndex.m_bAttackAble = false;
				stMonsterIndex.m_nIndex = nResult;

				if (It->m_Type >= 524 && It->m_Type <= 528 && It->m_Type != 526)
				{
					sLog->outBasic("[IMPERIALGUARDIAN] AddGate => %s, MapNumber => %d ,Zone => %d PosX => %d, PosY => %d",
						gGameObjects[nResult].Name, gGameObjects[nResult].MapNumber, gGameObjects[nResult].m_nZoneIndex, gGameObjects[nResult].X, gGameObjects[nResult].Y);
				}
			}

			this->m_stZoneInfo[nZoneIndex].m_vtMonsterIndexInfo.push_back(stMonsterIndex);

			if (It->m_Type >= 524 && It->m_Type <= 528 && It->m_Type != 526)
			{
				this->SetGateBlockState(It->m_MapNumber, nZoneIndex, It->m_X, It->m_Y, 1, It->m_Dir);
			}
		}
	}
}

void CImperialGuardian::DeleteMonster(int nZoneIndex, int nMonsterRegenTableIndex)
{
	std::vector<_stMonsterIndexInfo>::iterator It = this->m_stZoneInfo[nZoneIndex].m_vtMonsterIndexInfo.begin();
	int nCount = 0;

	while ( It != this->m_stZoneInfo[nZoneIndex].m_vtMonsterIndexInfo.end() )
	{
		int nIndex = It->m_nIndex;

		if ( gObjDel(nIndex) )
		{
			gGameObjects[nIndex].m_nZoneIndex = -1;
			nCount++;
		}

		It++;
	}

	sLog->outBasic("[IMPERIALGUARDIAN] DELETE ALL MONSTER -> [ZONE]:%d [COUNT]:%d", nZoneIndex+1, nCount);
}

void CImperialGuardian::InitZone(int nZoneIndex)
{
	if ( this->m_bLoadScript == false )
	{
		sLog->outError("[EVENTDUNGEON] script file not loaded");
		return;
	}

	this->m_stZoneInfo[nZoneIndex].m_bGoLooting = 0;
	this->m_stZoneInfo[nZoneIndex].m_bGoWaitPlayer = 0;
	this->m_stZoneInfo[nZoneIndex].m_bGoTimeAttack = 0;
	this->m_stZoneInfo[nZoneIndex].m_bAblePortal = 0;
	this->m_stZoneInfo[nZoneIndex].m_bStartRegenMonster = 0;
	this->m_stZoneInfo[nZoneIndex].m_bSendWarningMsg = 0;
	if ( nZoneIndex )
		this->m_stZoneInfo[nZoneIndex].m_dwRemainWaitPlayerTime = 1000 * this->m_dwDefaultWaitPlayerTime;
	else
		this->m_stZoneInfo[0].m_dwRemainWaitPlayerTime = 1000 * this->m_dwDefaultWaitPlayerTime;
	this->m_stZoneInfo[nZoneIndex].m_dwRemainLootTime = 1000 * this->m_dwDefaultLootTime;
	this->m_stZoneInfo[nZoneIndex].m_dwRemainTimeAttack = 1000 * this->m_dwDefaultTimeAttackTime;
	this->m_stZoneInfo[nZoneIndex].m_dwTickCount[0] = 0;
	this->m_stZoneInfo[nZoneIndex].m_dwTickCount[1] = 0;
	this->m_stZoneInfo[nZoneIndex].m_dwTickCount[2] = 0;
	this->m_stZoneInfo[nZoneIndex].m_nMonsterRegenTableIndex = -1;
	this->m_stZoneInfo[nZoneIndex].m_nPartyNumber = -1;
	this->m_stZoneInfo[nZoneIndex].m_nMaxUserLevel = 0;
	this->m_stZoneInfo[nZoneIndex].m_nZoneState = 0;
	this->m_stZoneInfo[nZoneIndex].m_nSendRemainCreatePortal = this->m_stZoneInfo[nZoneIndex].m_dwRemainTimeAttack / 60000 + 1;

	this->m_stZoneInfo[nZoneIndex].m_vtRegPartyUser.clear();
	this->m_stZoneInfo[nZoneIndex].m_vtUserIndex.clear();
	this->m_stZoneInfo[nZoneIndex].m_vtMonsterIndexInfo.clear();
	this->m_stZoneInfo[nZoneIndex].m_vtOpenedGate.clear();
}

bool CImperialGuardian::SetPosMonster(int nIndex, std::vector<_stIFMonsterPos>::iterator Iter)
{
	if (!ObjectMaxRange(nIndex))
	{
		sLog->outBasic("error : %s %d", __FILE__, __LINE__);
		return false;
	}

	CGameObject lpObj = &gGameObjects[nIndex];

	lpObj.m_PosNum = -1;
	lpObj.X = Iter->m_X;
	lpObj.Y = Iter->m_Y;
	lpObj.MapNumber = Iter->m_MapNumber;
	lpObj.TX = lpObj.X;
	lpObj.TY = lpObj.Y;
	lpObj.m_OldX = lpObj.X;
	lpObj.m_OldY = lpObj.Y;
	lpObj.Dir = Iter->m_Dir;
	lpObj.StartX = lpObj.X;
	lpObj.StartY = lpObj.Y;

	return true;
}

bool CImperialGuardian::SetMonster(int nIndex, int MonsterClass, int nMaxLevel)
{
	if ( !ObjectMaxRange(nIndex) )
	{
		sLog->outBasic("error : %s %d", __FILE__, __LINE__);
		return false;
	}

	CGameObject lpObj = &gGameObjects[nIndex];

	lpObj.ConnectCheckTime = GetTickCount();
	lpObj.TargetNumber = -1;
	lpObj.m_RecallMon = -1;
	lpObj.Connected = PLAYER_PLAYING;
	lpObj.Live = TRUE;
	lpObj.m_State = 1;
	lpObj.DieRegen = FALSE;
	lpObj.Type = OBJ_MONSTER;
	lpObj.Class = MonsterClass;
	lpObj.LastAttackerID = -1;
	g_MonsterStatCalc.ConvertMonsterStat(lpObj, nMaxLevel);
	lpObj.pInventoryCount[0] = 0;
	
	if ( lpObj.m_AttackType )
		gObjMonsterMagicAdd(lpObj, lpObj.m_AttackType, 1);

	gObjMonsterHitDamageInit(lpObj);
	gObjSetInventory1Pointer(lpObj);
	CreateFrustrum(lpObj.X, lpObj.Y, nIndex);
	MapC[lpObj.MapNumber].SetStandAttr(lpObj.X, lpObj.Y);
	lpObj.m_OldX = lpObj.X;
	lpObj.m_OldY = lpObj.Y;

	return true;
}

int CImperialGuardian::GetGateNumber(int nZoneIndex, int nDayOfWeek)
{
	int nGateNumber = -1;

	switch ( nZoneIndex )
	{
		case 0:
			if ( nDayOfWeek == 0 )
				nGateNumber = 322;
			else if ( nDayOfWeek == 1 || nDayOfWeek == 4 )
				nGateNumber = 307;
			else if ( nDayOfWeek == 2 || nDayOfWeek == 5 )
				nGateNumber = 312;
			else if ( nDayOfWeek == 3 || nDayOfWeek == 6 )
				nGateNumber = 317;
			break;
		case 1:
			if ( nDayOfWeek == 0 )
				nGateNumber = 324;
			else if ( nDayOfWeek == 1 || nDayOfWeek == 4 )
				nGateNumber = 309;
			else if ( nDayOfWeek == 2 || nDayOfWeek == 5 )
				nGateNumber = 314;
			else if ( nDayOfWeek == 3 || nDayOfWeek == 6 )
				nGateNumber = 319;
			break;
		case 2:
			if ( nDayOfWeek == 0 )
				nGateNumber = 326;
			else if ( nDayOfWeek == 1 || nDayOfWeek == 4 )
				nGateNumber = 311;
			else if ( nDayOfWeek == 2 || nDayOfWeek == 5 )
				nGateNumber = 316;
			else if ( nDayOfWeek == 3 || nDayOfWeek == 6 )
				nGateNumber = 321;
			break;
		case 3:
			if ( nDayOfWeek == 0 )
				nGateNumber = 328;
			break;
		default:
			sLog->outBasic("[IMPERIALGUARDIAN][GetGateNumberFunc] Invalid zoneIndex : %d", nZoneIndex+1);
			break;
	}

	return nGateNumber;
}

int CImperialGuardian::GetMapNumber(int nDayOfWeek)
{
	int nMapNumber = -1;

	if ( nDayOfWeek == 0 )
		nMapNumber = 72;
	else if ( nDayOfWeek == 1 || nDayOfWeek == 4 )
		nMapNumber = 69;
	else if ( nDayOfWeek == 2 || nDayOfWeek == 5 )
		nMapNumber = 70;
	else if ( nDayOfWeek == 3 || nDayOfWeek == 6 )
		nMapNumber = 71;

	return nMapNumber;
}

void CImperialGuardian::ProcCheckDungeon(int nZoneIndex)
{
	if ( nZoneIndex < 0 || nZoneIndex > 3 )
	{
		return;
	}

	SYSTEMTIME time;
	GetLocalTime(&time);

	if ( this->m_nDay != time.wDayOfWeek )
	{
		this->m_nDay = time.wDayOfWeek;
		this->m_nWeather = rand()%4;
	}

	this->CheckLiveMonster(nZoneIndex);

	if ( this->GetZoneState(nZoneIndex) )
	{
		if ( this->GetZoneState(nZoneIndex) == 1 && this->m_stZoneInfo[nZoneIndex].m_bStartRegenMonster == true && this->GetLiveMonsterCount(nZoneIndex) < 1 )
		{
			this->SetAtackAbleState(nZoneIndex, 524, true);
			this->SetAtackAbleState(nZoneIndex, 526, true);
			this->SetAtackAbleState(nZoneIndex, 527, true);
		}

		if ( this->GetPlayerCount(nZoneIndex) < 1 )
		{
			this->DeleteMonster(nZoneIndex, this->m_stZoneInfo[nZoneIndex].m_nMonsterRegenTableIndex);
			this->InitZone(nZoneIndex);
		}
	}
}

int CImperialGuardian::GetLiveMonsterCount(int nZoneIndex)
{
	std::vector<_stMonsterIndexInfo>::iterator it = this->m_stZoneInfo[nZoneIndex].m_vtMonsterIndexInfo.begin();

	int nCount = 0;

	while ( it != this->m_stZoneInfo[nZoneIndex].m_vtMonsterIndexInfo.end() )
	{
		if ( it->m_bLive == true )
		{
			nCount++;
		}

		it++;
	}

	return nCount;
}

void CImperialGuardian::CheckLiveMonster(int nZoneIndex)
{
	for ( std::vector<_stMonsterIndexInfo>::iterator it = this->m_stZoneInfo[nZoneIndex].m_vtMonsterIndexInfo.begin(); it != this->m_stZoneInfo[nZoneIndex].m_vtMonsterIndexInfo.end(); it++ )
	{
		if ( it->m_bLive == true )
		{
			if ( gGameObjects[it->m_nIndex].Live == FALSE || gGameObjects[it->m_nIndex].Connected != PLAYER_PLAYING )
			{
				it->m_bLive = false;
			}
		}

		if ( gGameObjects[it->m_nIndex].Live == TRUE )
		{
			if ( gGameObjects[it->m_nIndex].m_State != 1 && gGameObjects[it->m_nIndex].m_State != 2 )
			{
				sLog->outBasic("[IMPERIALGUARDIAN][RESTORE] [NAME]:%s [INDEX]:%d [TYPE]:%d [STATE]:%d", gGameObjects[it->m_nIndex].Name, it->m_nIndex,
					gGameObjects[it->m_nIndex].Class, gGameObjects[it->m_nIndex].m_State);

				gGameObjects[it->m_nIndex].m_State = 1;
			}
		}
	}
}

int CImperialGuardian::GetCurrentZoneIndex(int nUserNumber)
{
	EnterCriticalSection(&this->m_criUserIndex);
	int iZoneIndex = -1;

	for ( int i=0;i<MAX_ZONE;i++ )
	{	
		for ( std::vector<int>::iterator it = this->m_stZoneInfo[i].m_vtUserIndex.begin(); it != this->m_stZoneInfo[i].m_vtUserIndex.end(); it++ )
		{
			if ( *it == nUserNumber )
			{
				iZoneIndex = i;
				break;
			}
		}
	}

	LeaveCriticalSection(&this->m_criUserIndex);
	return iZoneIndex;
}

bool CImperialGuardian::AddUserInZone(int nZoneIndex, int nUserNumber)
{
	bool bAddUser = true;

	EnterCriticalSection(&this->m_criUserIndex);

	for ( std::vector<int>::iterator it = this->m_stZoneInfo[nZoneIndex].m_vtUserIndex.begin(); it != this->m_stZoneInfo[nZoneIndex].m_vtUserIndex.end(); it++ )
	{
		if ( *it == nUserNumber )
		{
			bAddUser = false;
			break;
		}
	}

	LeaveCriticalSection(&this->m_criUserIndex);

	if ( !bAddUser )
	{
		return false;
	}

	EnterCriticalSection(&this->m_criUserIndex);
	this->m_stZoneInfo[nZoneIndex].m_vtUserIndex.push_back(nUserNumber);
	LeaveCriticalSection(&this->m_criUserIndex);
	
	return true;
}

void CImperialGuardian::RemoveUserInZone(int nZoneIndex, int nUserNumber)
{
	EnterCriticalSection(&this->m_criUserIndex);

	for ( std::vector<int>::iterator it = this->m_stZoneInfo[nZoneIndex].m_vtUserIndex.begin(); it != this->m_stZoneInfo[nZoneIndex].m_vtUserIndex.end(); it++ )
	{
		if ( *it == nUserNumber )
		{
			this->m_stZoneInfo[nZoneIndex].m_vtUserIndex.erase(it);
			sLog->outBasic("[IMPERIALGUARDIAN] Leave Player Zone [ZONE]:%d, [AccountID]:%s, [Name]:%s", nZoneIndex, gGameObjects[nUserNumber].AccountID, gGameObjects[nUserNumber].Name);
			break;
		}
	}

	LeaveCriticalSection(&this->m_criUserIndex);
}

void CImperialGuardian::RemoverUserInAllZone(int nUserNumber)
{
	for(int i=0;i<MAX_ZONE;i++)
	{
		this->RemoveUserInZone(i, nUserNumber);
	}
}

int CImperialGuardian::GetPlayerCount(int nZoneIndex)
{
	if ( nZoneIndex < 0 || nZoneIndex > 3 )
	{
		return 0;
	}

	return this->m_stZoneInfo[nZoneIndex].m_vtUserIndex.size();
}

int CImperialGuardian::GetTotalPlayerCount()
{
	int nCount = 0;

	for (int i=0;i<MAX_ZONE;i++)
	{
		nCount += this->GetPlayerCount(i);
	}

	return nCount;
}

bool CImperialGuardian::IsLastZone(int nZoneIndex)
{
	if ( nZoneIndex < 0 || nZoneIndex > 3 )
	{
		return false;
	}

	if ( this->m_stZoneInfo[nZoneIndex].m_nMonsterRegenTableIndex != 0 && nZoneIndex == 2 )
	{
		return true;
	}

	else if ( this->m_stZoneInfo[nZoneIndex].m_nMonsterRegenTableIndex == 0 && nZoneIndex == 3 )
	{
		return true;
	}

	return false;
}

bool CImperialGuardian::GCNotifyZoneClear(int nZoneIndex) // function not used in 1.01.04
{
	return true;
}

bool CImperialGuardian::GCNotifyAllZoneClear(int nZoneIndex)
{
	if ( nZoneIndex < 0 || nZoneIndex > 3 )
	{
		return false;
	}

	PMSG_NOTIFY_ZONE_CLEAR pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xF7, 0x06, sizeof(pMsg));
	pMsg.m_nType = 2;

	int nUserIndex[5];
	int nCount = 0;
	
	EnterCriticalSection(&this->m_criUserIndex);

	for ( std::vector<int>::iterator it = this->m_stZoneInfo[nZoneIndex].m_vtUserIndex.begin(); it != this->m_stZoneInfo[nZoneIndex].m_vtUserIndex.end(); it++ )
	{
		if ( IMPERIAL_MAP_RANGE(gGameObjects[*it].MapNumber) == TRUE )
		{
			nUserIndex[nCount] = *it;
			nCount++;
		}
	}

	LeaveCriticalSection(&this->m_criUserIndex);

	for (int i = 0; i < nCount; i++)
	{
		pMsg.m_nRewardExp = this->m_rewardExp.GetRewardExp(nUserIndex[i]);

		if ( nZoneIndex == 3 )
			pMsg.m_nRewardExp *= 2;

		if ( gObjCheckUsedBuffEffect(&gGameObjects[nUserIndex[i]], 31) ||
			gObjCheckUsedBuffEffect(&gGameObjects[nUserIndex[i]], 42) )
		{
			pMsg.m_nRewardExp = 0;
		}

		this->ImperialGuardianLevelUp(nUserIndex[i], pMsg.m_nRewardExp);
			
		sLog->outBasic("[IMPERIALGUARDIAN] [ACCOUNTID]:%s, [NAME]:%s, [Reward Exp] : %d", gGameObjects[nUserIndex[i]].AccountID, gGameObjects[nUserIndex[i]].Name, pMsg.m_nRewardExp);

		if ( g_MasterLevelSkillTreeSystem.IsMasterLevelUser(&gGameObjects[nUserIndex[i]]) == false )
		{
			GSProtocol.GCKillPlayerExpSend(nUserIndex[i], -1, pMsg.m_nRewardExp, 0, 0);
		}

		this->GCSendDataToUser(nUserIndex[i], (char*)&pMsg, pMsg.h.size);

		g_CashShop.AddCoin(&gGameObjects[nUserIndex[i]], EVENT_DG);
		
		char szText[256];
		memset(szText, 0x00, sizeof(szText));

		wsprintf(szText, Lang.GetText(0,352));
		GSProtocol.GCServerMsgStringSend(szText, nUserIndex[i], 0);
	}

	return true;
}

void CImperialGuardian::GCMissionFail(int nZoneIndex)
{
	if ( nZoneIndex < 0 || nZoneIndex > 3 )
	{
		return;
	}

	PMSG_NOTIFY_ZONE_CLEAR pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xF7, 0x06, sizeof(pMsg));
	pMsg.m_nType = 0;
	pMsg.m_nRewardExp = 0;

	int nUserIndex[5];
	int nCount = 0;

	EnterCriticalSection(&this->m_criUserIndex);

	for ( std::vector<int>::iterator it = this->m_stZoneInfo[nZoneIndex].m_vtUserIndex.begin(); it != this->m_stZoneInfo[nZoneIndex].m_vtUserIndex.end(); it++ )
	{
		if ( IMPERIAL_MAP_RANGE(gGameObjects[*it].MapNumber) == TRUE )
		{
			nUserIndex[nCount] = *it;
			nCount++;
		}
	}

	LeaveCriticalSection(&this->m_criUserIndex);

	for (int i = 0; i < nCount; i++)
	{
		this->GCSendDataToUser(nUserIndex[i], (char*)&pMsg, pMsg.h.size);

		char szText[256];
		memset(szText, 0x00, sizeof(szText));

		wsprintf(szText, Lang.GetText(0,353));
		GSProtocol.GCServerMsgStringSend(szText, nUserIndex[i], 0);
	}
}

void CImperialGuardian::GCMissionFailUserDie(int nUserNumber)
{
	if ( !ObjectMaxRange(nUserNumber) )
	{
		return;
	}

	PMSG_NOTIFY_ZONE_CLEAR pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF7, 0x06, sizeof(pMsg));

	pMsg.m_nType = 0;
	pMsg.m_nRewardExp = 0;

	this->GCSendDataToUser(nUserNumber, (char*)&pMsg, pMsg.h.size);

	char szText[256];
	memset(szText, 0x00, sizeof(szText));
	
	wsprintf(szText, Lang.GetText(0,354));
	GSProtocol.GCServerMsgStringSend(szText, nUserNumber, 0);
}

void CImperialGuardian::GCSendDataToUser(int nIndex, char* lpMsg, int nSize)
{
	if ( !ObjectMaxRange(nIndex) )
	{
		return;
	}

	if ( gGameObjects[nIndex].Connected == PLAYER_PLAYING && gGameObjects[nIndex].Type == OBJ_USER )
	{
		IOCP.DataSend(nIndex, (LPBYTE)lpMsg, nSize);
	}
}

void CImperialGuardian::GCNotifyRemainTickCount(int nZoneIndex, char btMsgType, DWORD dwRemainTick)
{
	PMSG_NOTIFY_ZONE_TIME pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF7, 0x04, sizeof(pMsg));

	pMsg.m_btMsgType = btMsgType;
	pMsg.m_wZoneIndex = nZoneIndex;
	pMsg.m_dwRemainMonster = this->GetLiveMonsterCount(nZoneIndex);
	pMsg.m_btDayOfWeek = this->m_stZoneInfo[nZoneIndex].m_nMonsterRegenTableIndex;

	if ( pMsg.m_btDayOfWeek == 0 )
		pMsg.m_btDayOfWeek = 7;

	if ( btMsgType == 0 )
	{
		pMsg.m_dwRemainTime = this->m_stZoneInfo[nZoneIndex].m_dwRemainLootTime;
	}

	else if ( btMsgType == 1 )
	{
		pMsg.m_dwRemainTime = this->m_stZoneInfo[nZoneIndex].m_dwRemainWaitPlayerTime;
	}

	else if ( btMsgType == 2 )
	{
		pMsg.m_dwRemainTime = this->m_stZoneInfo[nZoneIndex].m_dwRemainTimeAttack;
	}

	else
	{
		sLog->outBasic("[IMPERIALGUARDIAN][GCNotifyRemainTickCountFunc] Invalid MsgType : %d", btMsgType);
		return;
	}

	int nUserIndex[5];
	int nCount = 0;

	EnterCriticalSection(&this->m_criUserIndex);

	for ( std::vector<int>::iterator it = this->m_stZoneInfo[nZoneIndex].m_vtUserIndex.begin(); it != this->m_stZoneInfo[nZoneIndex].m_vtUserIndex.end(); it++ )
	{
		if (ObjectMaxRange(*it) == false)
		{
			continue;
		}

		nUserIndex[nCount] = *it;
		nCount++;
	}

	LeaveCriticalSection(&this->m_criUserIndex);

	for (int i = 0; i < nCount; i++)
	{
		if ( IMPERIAL_MAP_RANGE(gGameObjects[nUserIndex[i]].MapNumber) == TRUE )
		{
			this->GCSendDataToUser(nUserIndex[i], (char*)&pMsg, pMsg.h.size);
		}
	}
}

void CImperialGuardian::SetGateBlockState(int nMapNumber, int nZoneIndex, int nX, int nY, int iGateState, int nDir)
{
	if ( nX < 0 || nX > 255 )
	{
		return;
	}

	if ( nY < 0 || nY > 255 )
	{
		return;
	}

	if ( iGateState == 1 )
	{
		switch ( nDir )
		{
			case 1:
				for (int i = nX - 2; i <= nX + 2; i++ )
				{
					for (int j = nY; j <= nY + 3; j++ )
					{
						if ( i != nX || j != nY )
						{
							MapC[nMapNumber].m_attrbuf[j * 256 + i] |= 0x10;
						}
					}
				}
				break;
			case 3:
				for (int i = nX - 3; i <= nX; i++ )
				{
					for (int j = nY - 2; j <= nY + 2; j++ )
					{
						if ( i != nX || j != nY )
						{
							MapC[nMapNumber].m_attrbuf[j * 256 + i] |= 0x10;
						}
					}
				}
				break;
			case 5:
				for (int i = nX - 2; i <= nX + 2; i++ )
				{
					for (int j = nY; j <= nY - 3; j++ )
					{
						if ( i != nX || j != nY )
						{
							MapC[nMapNumber].m_attrbuf[j * 256 + i] |= 0x10;
						}
					}
				}
				break;
			case 7:
				for (int i = nX; i <= nX + 3; i++ )
				{
					for (int j = nY - 2; j <= nY + 2; j++ )
					{
						if ( i != nX || j != nY )
						{
							MapC[nMapNumber].m_attrbuf[j * 256 + i] |= 0x10;
						}
					}
				}
				break;
		}
	}

	else
	{
		switch ( nDir )
		{
			case 1:
				for (int i = nX - 2; i <= nX + 2; i++ )
				{
					for (int j = nY; j <= nY + 3; j++ )
					{
						if ( i != nX || j != nY )
						{
							MapC[nMapNumber].m_attrbuf[j * 256 + i] &= 0xEF;
						}
					}
				}
				break;
			case 3:
				for (int i = nX - 3; i <= nX; i++ )
				{
					for (int j = nY - 2; j <= nY + 2; j++ )
					{
						if ( i != nX || j != nY )
						{
							MapC[nMapNumber].m_attrbuf[j * 256 + i] &= 0xEF;
						}
					}
				}
				break;
			case 5:
				for (int i = nX - 2; i <= nX + 2; i++ )
				{
					for (int j = nY; j <= nY - 3; j++ )
					{
						if ( i != nX || j != nY )
						{
							MapC[nMapNumber].m_attrbuf[j * 256 + i] &= 0xEF;
						}
					}
				}
				break;
			case 7:
				for (int i = nX; i <= nX + 3; i++ )
				{
					for (int j = nY - 2; j <= nY + 2; j++ )
					{
						if ( i != nX || j != nY )
						{
							MapC[nMapNumber].m_attrbuf[j * 256 + i] &= 0xEF;
						}
					}
				}
				break;
		}
	}
}

void CImperialGuardian::GCSendCastleGateInfo(int nGateIndex, int nZoneIndex, int nUserIndex)
{
	char cTEMP_BUF[256];

	PMSG_SETMAPATTR_COUNT * lpMsg = (PMSG_SETMAPATTR_COUNT *)(cTEMP_BUF);
	PMSG_SETMAPATTR * lpMsgBody = (PMSG_SETMAPATTR *)(cTEMP_BUF + sizeof(PMSG_SETMAPATTR_COUNT));

	PHeadSetB((LPBYTE)lpMsg, 0x46, sizeof(PMSG_SETMAPATTR_COUNT)+sizeof(PMSG_SETMAPATTR)*6);

	lpMsg->btType = 0;
	lpMsg->btMapAttr = 16;
	lpMsg->btCount = 1;

	int iGateState;

	if ( gGameObjects[nGateIndex].Live == TRUE )
	{
		iGateState = TRUE;
	}

	else
	{
		iGateState = FALSE;
	}

	int nDir = gGameObjects[nGateIndex].Dir;
	int nX = gGameObjects[nGateIndex].X;
	int nY = gGameObjects[nGateIndex].Y;

	switch ( nDir )
	{
		case 1:
			lpMsgBody[0].btX = nX - 2;
			lpMsgBody[0].btY = nY;
			lpMsgBody[1].btX = nX + 2;
			lpMsgBody[1].btY = nY + 3;
			break;
		case 3:
			lpMsgBody[0].btX = nX - 3;
			lpMsgBody[0].btY = nY - 2;
			lpMsgBody[1].btX = nX;
			lpMsgBody[1].btY = nY + 2;
			break;
		case 5:
			lpMsgBody[0].btX = nX - 2;
			lpMsgBody[0].btY = nY - 3;
			lpMsgBody[1].btX = nX + 2;
			lpMsgBody[1].btY = nY;
			break;
		case 7:
			lpMsgBody[0].btX = nX;
			lpMsgBody[0].btY = nY - 2;
			lpMsgBody[1].btX = nX + 3;
			lpMsgBody[1].btY = nY + 2;
			break;
	}

	if ( iGateState == TRUE )
	{
		lpMsg->btMapSetType = 0;
	}

	else
	{
		lpMsg->btMapSetType = 1;
	}

	if ( gGameObjects[nUserIndex].Connected == PLAYER_PLAYING )
	{
		IOCP.DataSend(nUserIndex, (LPBYTE)&cTEMP_BUF, (sizeof(PMSG_SETMAPATTR_COUNT)+6*sizeof(PMSG_SETMAPATTR)));

		sLog->outBasic("[IMPERIALGUARDIAN] SEND GATE STATE -> [ZONE]:%d [AccountID]:%s, [NAME]:%s [STATE]:%d",
			nZoneIndex+1, gGameObjects[nUserIndex].AccountID, gGameObjects[nUserIndex].Name, iGateState);

		sLog->outBasic("[IMPERIALGUARDIAN] beginX : %d, beginY : %d , endX :%d , endY : %d",
			lpMsgBody[0].btX, lpMsgBody[0].btY, lpMsgBody[1].btX, lpMsgBody[1].btY);
	}
}

void CImperialGuardian::DropItem(int nIndex, int nMonsterIndex)
{
	if ( gGameObjects[nIndex].Type != OBJ_USER )
	{
		return;
	}

	int nZoneIndex = this->GetCurrentZoneIndex(nIndex);

	if ( nZoneIndex < 0 || nZoneIndex > 3 )
	{
		return;
	}

	int nMaxLevel = this->m_stZoneInfo[nZoneIndex].m_nMaxUserLevel;
	int nType = 0;

	if ( !nMaxLevel )
	{
		nMaxLevel = gGameObjects[nIndex].Level + gGameObjects[nIndex].m_PlayerData->MasterLevel;
	}

	if ( gGameObjects[nMonsterIndex].Class == 504 ) // Gaion
	{
		sLog->outBasic("[IMPERIALGUARDIAN] Kill the boss monster => %s [ZONE]:%d [AccountID]:%s [Name]:%s",
			gGameObjects[nMonsterIndex].Name, nZoneIndex+1, gGameObjects[nIndex].AccountID, gGameObjects[nIndex].Name);

		this->pEventDungeonItemBagGaion->DropEventItem(nIndex, nMaxLevel);
	}

	else if ( gGameObjects[nMonsterIndex].Class == 526 ) // Statue
	{
		sLog->outBasic("[IMPERIALGUARDIAN] Broken Statue => %s [ZONE]:%d [AccountID]:%s [Name]:%s",
			gGameObjects[nMonsterIndex].Name, nZoneIndex+1, gGameObjects[nIndex].AccountID, gGameObjects[nIndex].Name);

		this->pEventDungeonItemBagStatue->DropEventItem(nIndex, nMaxLevel);
	}

	else
	{
		sLog->outBasic("[IMPERIALGUARDIAN] Kill the boss monster => %s [ZONE]:%d [AccountID]:%s [Name]:%s",
			gGameObjects[nMonsterIndex].Name, nZoneIndex+1, gGameObjects[nIndex].AccountID, gGameObjects[nIndex].Name);

		this->pEventDungeonItemBag->DropEventItem(nIndex, nMaxLevel);
	}

	if ( this->m_stZoneInfo[nZoneIndex].m_nMonsterRegenTableIndex != 0 )
	{
		switch ( gGameObjects[nMonsterIndex].Class )
		{
			case 506:
				nType = ItemGetNumberMake(14, 108);
				break;
			case 508:
				nType = ItemGetNumberMake(14, 103);
				break;
			case 509:
				nType = ItemGetNumberMake(14, 104);
				break;
			case 510:
				nType = ItemGetNumberMake(14, 105);
				break;
			case 511:
				nType = ItemGetNumberMake(14, 106);
				break;
			case 507:
				nType = ItemGetNumberMake(14, 107);
				break;
			default:
				return;
		}

		int nCount = 0;
		int nRand = rand()%100;

		if ( nRand < 0 || nRand >= 50 )
		{
			if ( nRand < 50 || nRand >= 79 )
				nCount = 3;
			else
				nCount = 2;
		}

		else
		{
			nCount = 1;
		}

		for ( int i=0; i<nCount; i++ )
		{
			BYTE btDropX = gGameObjects[nIndex].X;
			BYTE btDropY = gGameObjects[nIndex].Y;

			if ( !gObjGetRandomItemDropLocation(gGameObjects[nIndex].MapNumber, btDropX, btDropY, 2, 2, 10) )
			{
				btDropX = gGameObjects[nIndex].X;
				btDropY = gGameObjects[nIndex].Y;
			}

			ItemSerialCreateSend(nIndex, gGameObjects[nIndex].MapNumber, btDropX, btDropY, nType, 0, 0, 0, 0, 0, nIndex, 0, 0, 0, 0, 0);
			sLog->outBasic("[IMPERIALGUARDIAN] Drop Item : (%d)(%d/%d) Item:(%s)%d Level:%d op1:%d op2:%d op3:%d",
				gGameObjects[nIndex].MapNumber, btDropX, btDropY, ItemAttribute[nType].Name, nType, 0, 0, 0, 0);
		}
	}
}

int CImperialGuardian::CheckOverlapMysteriousPaper(int nIndex, int nItemLevel) // not used
{
	return -1;
}

void CImperialGuardian::RollBackUserPos(int nUserNumber)
{
	if ( gGameObjects[nUserNumber].Type == OBJ_USER && gGameObjects[nUserNumber].Connected > PLAYER_LOGGED )
	{
		gGameObjects[nUserNumber].RegenMapNumber = gGameObjects[nUserNumber].MapNumber;
		gGameObjects[nUserNumber].RegenMapX = gGameObjects[nUserNumber].X;
		gGameObjects[nUserNumber].RegenMapY = gGameObjects[nUserNumber].Y;

		gObjClearViewport(&gGameObjects[nUserNumber]);
		GSProtocol.GCTeleportSend(&gGameObjects[nUserNumber], -1, gGameObjects[nUserNumber].MapNumber, gGameObjects[nUserNumber].X, gGameObjects[nUserNumber].Y, gGameObjects[nUserNumber].Dir);
		
		if ( gGameObjects[nUserNumber].m_Change >= 0 )
			gObjViewportListProtocolCreate(&gGameObjects[nUserNumber]);

		gGameObjects[nUserNumber].RegenOk = TRUE;
	}
}

int CImperialGuardian::CheckGaionOrderPaper(int nIndex)
{
	if ( ObjectMaxRange(nIndex) == false )
	{
		sLog->outBasic("error : %s %d", __FILE__, __LINE__);
		return -2;
	}

	CGameObject lpObj = &gGameObjects[nIndex];

	if ( lpObj.m_IfState.use > 0 )
	{
		return -3;
	}

	if ( lpObj.Type != OBJ_USER || lpObj.Connected <= PLAYER_LOGGED )
	{
		return -2;
	}

	for ( int i=INVETORY_WEAR_SIZE;i<INVENTORY_SIZE;i++ )
	{
		if ( lpObj.pInventory[i].IsItem() == TRUE && lpObj.pInventory[i].m_Type == ITEMGET(14,102) )
		{
			return i;
		}
	}

	for ( int i=INVETORY_WEAR_SIZE;i<INVENTORY_SIZE;i++ )
	{
		if ( lpObj.pInventory[i].m_Type == ITEMGET(13,126) && lpObj.pInventory[i].m_Durability > 0.0 )
		{
			return i;
		}
	}

	return -1;
}

int CImperialGuardian::CheckFullSecromicon(int nIndex)
{
	if ( ObjectMaxRange(nIndex) == false )
	{
		sLog->outBasic("error : %s %d", __FILE__, __LINE__);
		return -2;
	}

	CGameObject lpObj = &gGameObjects[nIndex];

	if ( lpObj.m_IfState.use > 0 )
	{
		return -3;
	}

	if ( lpObj.Type != OBJ_USER || lpObj.Connected <= PLAYER_LOGGED )
	{
		return -2;
	}

	for ( int i=INVETORY_WEAR_SIZE;i<INVENTORY_SIZE;i++ )
	{
		if ( lpObj.pInventory[i].IsItem() == TRUE && lpObj.pInventory[i].m_Type == ITEMGET(14,109) )
		{
			return i;
		}
	}

	for ( int i=INVETORY_WEAR_SIZE;i<INVENTORY_SIZE;i++ )
	{
		if ( lpObj.pInventory[i].m_Type == ITEMGET(13,127) && lpObj.pInventory[i].m_Durability > 0.0 )
		{
			return i;
		}
	}

	return -1;
}

bool CImperialGuardian::IsAttackAbleMonster(int nMonsterIndex)
{
	for(int i=0;i<MAX_ZONE;i++)
	{
		for (std::vector<_stMonsterIndexInfo>::iterator it = this->m_stZoneInfo[i].m_vtMonsterIndexInfo.begin(); it != this->m_stZoneInfo[i].m_vtMonsterIndexInfo.end(); it++)
		{
			if ( it->m_nIndex == nMonsterIndex )
			{
				return it->m_bAttackAble;
			}
		}
	}
	
	return false;
}

void CImperialGuardian::SetAtackAbleState(int nZoneIndex, int nMonsterClass, bool bState)
{
	if ( nZoneIndex < 0 || nZoneIndex > 3 )
	{
		sLog->outBasic("error : %s %d", __FILE__, __LINE__);
		return;
	}

	if ( this->m_stZoneInfo[nZoneIndex].m_vtMonsterIndexInfo.size() == 0 )
	{
		sLog->outBasic("error : %s %d", __FILE__, __LINE__);
		return;
	}

	for (std::vector<_stMonsterIndexInfo>::iterator it = this->m_stZoneInfo[nZoneIndex].m_vtMonsterIndexInfo.begin(); it != this->m_stZoneInfo[nZoneIndex].m_vtMonsterIndexInfo.end(); it++ )
	{
		CGameObject lpObj = &gGameObjects[it->m_nIndex];

		if ( lpObj.Class == nMonsterClass )
		{
			it->m_bAttackAble = bState;
		}
	}
}

void CImperialGuardian::SetDayOfWeekGM(int nDayOfWeeks)
{
	this->m_nDayOfWeeksForCheat = nDayOfWeeks;
}

void CImperialGuardian::SetCheatModeGM(WORD nCheatMode)
{
	this->m_nCheatMode = nCheatMode;
}

void CImperialGuardian::WarpZoneGM(int nUserIndex, int nZoneIndex)
{
	if ( this->m_nCheatMode )
	{
		this->CGEnterPortal(nUserIndex, nZoneIndex);
	}
}

_stZoneInfo * CImperialGuardian::GetZoneInfo(_stZoneInfo *result, int nZoneIndex)
{
	result = &this->m_stZoneInfo[nZoneIndex];
	return result;
}

void CImperialGuardian::SetTargetMoveAllMonster(int nZoneIndex, int nTargetNumber)
{
	if ( ObjectMaxRange(nTargetNumber) == false )
	{
		sLog->outBasic("error : %s %d", __FILE__, __LINE__);
		return;
	}

	if ( gGameObjects[nTargetNumber].Connected < PLAYER_CONNECTED )
	{
		sLog->outBasic("error : %s %d", __FILE__, __LINE__);
		return;
	}

	for(std::vector<_stMonsterIndexInfo>::iterator it = this->m_stZoneInfo[nZoneIndex].m_vtMonsterIndexInfo.begin(); it != this->m_stZoneInfo[nZoneIndex].m_vtMonsterIndexInfo.end(); it++)
	{
		if ( it->m_bLive == true )
		{
			CGameObject lpObj = &gGameObjects[it->m_nIndex];
			
			if ( lpObj.TargetNumber != -1 || lpObj.m_ActState.Emotion != 0 )
			{
				if ( lpObj.TargetNumber != nTargetNumber )
				{
					if ( rand()%100 < 30 )
					{
						lpObj.TargetNumber = nTargetNumber;
						lpObj.m_ActState.Emotion = 1;
						lpObj.m_ActState.EmotionCount = 50;
					}
				}
			}

			else
			{
				lpObj.TargetNumber = nTargetNumber;
				lpObj.m_ActState.Emotion = 1;
				lpObj.m_ActState.EmotionCount = 50;
			}
		}
	}
}

void CImperialGuardian::DestroyGate(int nZoneIndex, int nIndex, int nTargetIndex)
{
	if (nZoneIndex < 0 || nZoneIndex > 3)
	{
		return;
	}

	this->SetGateBlockState(gGameObjects[nIndex].MapNumber, nZoneIndex, gGameObjects[nIndex].X, gGameObjects[nIndex].Y, 0, gGameObjects[nIndex].Dir);

	EnterCriticalSection(&this->m_criUserIndex);

	int nUserIndex[5];
	int nCount = 0;

	for(std::vector<int>::iterator it = this->m_stZoneInfo[nZoneIndex].m_vtUserIndex.begin(); it != this->m_stZoneInfo[nZoneIndex].m_vtUserIndex.end(); it++)
	{
		nUserIndex[nCount] = *it;
		nCount++;
	}

	LeaveCriticalSection(&this->m_criUserIndex);

	for (int i = 0; i < nCount; i++)
	{
		this->GCSendCastleGateInfo(nIndex, nZoneIndex, nUserIndex[i]);
	}

	this->m_stZoneInfo[nZoneIndex].m_vtOpenedGate.push_back(nIndex);

	sLog->outBasic("[IMPERIALGUARDIAN] DestroyGate -> [ZONE]:%d, [AccountID]:%s, [NAME]:%s, [GATE INDEX]:%d, [USER COUNT]:%d",
		nZoneIndex, gGameObjects[nTargetIndex].AccountID, gGameObjects[nTargetIndex].Name, nIndex, this->m_stZoneInfo[nZoneIndex].m_vtUserIndex.size());
}

void CImperialGuardian::GCSendServerMsgAll(int nZoneIndex, char* szMsg)
{
	EnterCriticalSection(&this->m_criUserIndex);

	int nUserIndex[5];
	int nCount = 0;

	for(std::vector<int>::iterator it = this->m_stZoneInfo[nZoneIndex].m_vtUserIndex.begin(); it != this->m_stZoneInfo[nZoneIndex].m_vtUserIndex.end(); it++)
	{
		if ( gGameObjects[*it].Connected > PLAYER_EMPTY )
		{
			nUserIndex[nCount] = *it;
			nCount++;
		}
	}

	LeaveCriticalSection(&this->m_criUserIndex);

	for (int i = 0; i < nCount; i++)
	{
		GSProtocol.GCServerMsgStringSend(szMsg, nUserIndex[i], 0);
	}
}

int CImperialGuardian::ImperialGuardianLevelUp(int iIndex, int iAddExp)
{
	if ( ObjectMaxRange(iIndex) == FALSE )
	{
		return 0;
	}

	if ( g_MasterLevelSkillTreeSystem.IsMasterLevelUser(&gGameObjects[iIndex]) == true)
	{
		g_MasterLevelSkillTreeSystem.MasterLevelUp(&gGameObjects[iIndex], iAddExp, 0, "Imperial Guardian");
		return 0;
	}

	int iLEFT_EXP = 0;

	sLog->outBasic("[Imperial Guardian] Experience : [%s][%s](%d) Experience: %d + %d",	
		gGameObjects[iIndex].AccountID,	gGameObjects[iIndex].Name,
		gGameObjects[iIndex].Level, gGameObjects[iIndex].m_PlayerData->Experience,
		iAddExp);

	::gObjSetExpPetItem(iIndex, iAddExp);

	if ( gGameObjects[iIndex].Level >= g_ConfigRead.data.common.UserMaxLevel)
	{
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0,45), gGameObjects[iIndex].m_Index, 1);
		return 0;
	}

	if ( (gGameObjects[iIndex].m_PlayerData->Experience + iAddExp) < gGameObjects[iIndex].m_PlayerData->NextExp )
	{
		gGameObjects[iIndex].m_PlayerData->Experience += iAddExp;
	}
	else
	{
		iLEFT_EXP = gGameObjects[iIndex].m_PlayerData->Experience + iAddExp - gGameObjects[iIndex].m_PlayerData->NextExp;
		gGameObjects[iIndex].m_PlayerData->Experience = gGameObjects[iIndex].m_PlayerData->NextExp;
		gGameObjects[iIndex].Level++;

		if ( g_ConfigRead.data.reset.iBlockLevelUpPointAfterResets == -1 || gGameObjects[iIndex].m_PlayerData->m_iResets < g_ConfigRead.data.reset.iBlockLevelUpPointAfterResets )
		{
			if (gGameObjects[iIndex].Class == CLASS_DARKLORD || gGameObjects[iIndex].Class == CLASS_MAGUMSA || gGameObjects[iIndex].Class == CLASS_RAGEFIGHTER || gGameObjects[iIndex].Class == CLASS_GROWLANCER)
			{
				gGameObjects[iIndex].m_PlayerData->LevelUpPoint += g_MaxStatsInfo.GetClass.LevelUpPointMGDL;
			}

			else
			{
				gGameObjects[iIndex].m_PlayerData->LevelUpPoint += g_MaxStatsInfo.GetClass.LevelUpPointNormal;
			}

			if ( gGameObjects[iIndex].m_PlayerData->PlusStatQuestClear != false )
			{
				gGameObjects[iIndex].m_PlayerData->LevelUpPoint++;

				//sLog->outBasic("[%s][%s] LevelUp PlusStatQuest Clear AddStat %d",gGameObjects[iIndex].AccountID, gGameObjects[iIndex].Name, gGameObjects[iIndex].m_PlayerData->LevelUpPoint);
			}
		}

		gGameObjects[iIndex].MaxLife += DCInfo.DefClass[gGameObjects[iIndex].Class].LevelLife;
		gGameObjects[iIndex].MaxMana += DCInfo.DefClass[gGameObjects[iIndex].Class].LevelMana;
		gGameObjects[iIndex].Life = gGameObjects[iIndex].MaxLife;
		gGameObjects[iIndex].Mana = gGameObjects[iIndex].MaxMana;
		gObjNextExpCal(&gGameObjects[iIndex]);
		gObjSetBP(iIndex);
		GSProtocol.GCLevelUpMsgSend(gGameObjects[iIndex].m_Index, 1);
		gObjCalcMaxLifePower(gGameObjects[iIndex].m_Index);
		sLog->outBasic("Level Up [%s][%s][%d]", gGameObjects[iIndex].AccountID, gGameObjects[iIndex].Name, gGameObjects[iIndex].Level);
		return 0;
	}

	return iLEFT_EXP;
}

void CImperialGuardian::RegAllPartyUser(int nZoneIndex, int nFirstEnterUserIndex)
{
	if ( gGameObjects[nFirstEnterUserIndex].Connected < PLAYER_CONNECTED )
	{
		return;
	}

	int nPartyNumber = gGameObjects[nFirstEnterUserIndex].PartyNumber;
	int nPartyCount = gParty.GetPartyCount(nPartyNumber);

	for ( int i=0; i < nPartyCount; i++ )
	{
		int nUserNumber = gParty.m_PartyS[nPartyNumber].Number[i];
		this->m_stZoneInfo[nZoneIndex].m_vtRegPartyUser.push_back(nUserNumber);
	}
}

bool CImperialGuardian::IsRegPartyUser(int nZoneIndex, int nUserIndex)
{
	bool bFlag = false;

	for(std::vector<int>::iterator it = this->m_stZoneInfo[nZoneIndex].m_vtRegPartyUser.begin(); it != this->m_stZoneInfo[nZoneIndex].m_vtRegPartyUser.end(); it++)
	{
		if ( *it == nUserIndex )
		{
			bFlag = true;
		}
	}

	return bFlag;
}

void CImperialGuardian::UserMonsterCountCheck()
{
	// empty
}

void CImperialGuardian::MonsterBaseAct(CGameObject &lpObj)
{
	CGameObject lpTargetObj = NULL;

	if ( lpObj.TargetNumber >= 0 )
		lpTargetObj = &gGameObjects[lpObj.TargetNumber];
	else
		lpObj.m_ActState.Emotion = 0;

	if ( lpObj.m_ActState.Emotion == 1 )
	{
		if ( lpObj.m_ActState.EmotionCount > 0 )
		{
			lpObj.m_ActState.EmotionCount--;
		}
		else
		{
			lpObj.m_ActState.Emotion = 0;
		}

		if ( lpObj.TargetNumber >= 0 && lpObj.PathStartEnd == 0)
		{
			int dis = gObjCalDistance(lpObj, lpTargetObj);
			int attackrange;

			if ( lpObj.m_AttackType >= 100 )	// #warning Change this To Level
				attackrange = lpObj.m_AttackRange+2;
			else
				attackrange = lpObj.m_AttackRange;

			if ( dis <= attackrange )
			{
				int tuser = lpObj.TargetNumber;

				if (!ObjectMaxRange(tuser))
				{
					return;
				}

				int map = gGameObjects[tuser].MapNumber;
				BYTE attr;

				if ( MAX_MAP_RANGE(map) == FALSE )
				{
					sLog->outError( "[ERROR] MAX_MAP_RANGE (gObjMonster) == FALSE (%d)", map);
					return;
				}

				if ( MapC[map].CheckWall(lpObj.X, lpObj.Y, gGameObjects[tuser].X, gGameObjects[tuser].Y) == TRUE )
				{
					attr = MapC[map].GetAttr(gGameObjects[tuser].X, gGameObjects[tuser].Y);

					if ( (attr&1) != 1 )
					{
						lpObj.m_ActState.Attack = 1;
						lpObj.m_ActState.EmotionCount = rand()%30;
					}
					else
					{
						lpObj.TargetNumber = -1;
						lpObj.m_ActState.EmotionCount = 30;
						lpObj.m_ActState.Emotion = 1;
					}

					lpObj.Dir = GetPathPacketDirPos(lpTargetObj.X-lpObj.X, lpTargetObj.Y-lpObj.Y);
					lpObj.NextActionTime = lpObj.m_AttackSpeed;
				}
			}
			
			else
			{
				if ( gObjMonsterGetTargetPos(lpObj) == TRUE )
				{
					if ( MAX_MAP_RANGE(lpObj.MapNumber) == FALSE )
					{
						sLog->outError( "[ERROR] MAX_MAP_RANGE (gObjMonster) == FALSE (%d)", lpObj.MapNumber);
						return;
					}

					if ( MapC[lpObj.MapNumber].CheckWall(lpObj.X, lpObj.Y, lpObj.MTX, lpObj.MTY) == TRUE )
					{
						lpObj.m_ActState.Move = 1;
						lpObj.NextActionTime = 400;
						lpObj.Dir = GetPathPacketDirPos(lpTargetObj.X-lpObj.X, lpTargetObj.Y-lpObj.Y);
					}
					else
					{
						gObjMonsterMoveAction(lpObj);
						lpObj.m_ActState.Emotion = 3;
						lpObj.m_ActState.EmotionCount = 10;

					}
				}

				else
				{
					int tx = 0;
					int ty = 0;

					if ( lpTargetObj.Connected > PLAYER_LOGGED && gObjGetTargetPos(lpObj, lpTargetObj.X, lpTargetObj.Y, tx, ty) == TRUE )
					{
						if ( MAX_MAP_RANGE(lpObj.MapNumber) == FALSE )
						{
							sLog->outError( "[ERROR] MAX_MAP_RANGE (gObjMonster) == FALSE (%d)", lpObj.MapNumber);
							return;
						}

							if ( MapC[lpObj.MapNumber].CheckWall(lpObj.X, lpObj.Y, lpObj.MTX, lpObj.MTY) == TRUE )
							{
								lpObj.MTX = tx;
								lpObj.MTY = ty;
								lpObj.m_ActState.Move = 1;
								lpObj.NextActionTime = 400;
								lpObj.Dir = GetPathPacketDirPos(lpTargetObj.X-lpObj.X, lpTargetObj.Y-lpObj.Y);
							}

							else
							{
								gObjMonsterMoveAction(lpObj);
								lpObj.m_ActState.Emotion = 3;
								lpObj.m_ActState.EmotionCount = 10;
							}
					}
				}
			}
		}
	}

	else if ( lpObj.m_ActState.Emotion == 3 )
	{
		if ( lpObj.m_ActState.EmotionCount > 0 )
		{
			lpObj.m_ActState.EmotionCount--;
		}
		else
		{
			lpObj.m_ActState.Emotion = 0;
		}

		lpObj.m_ActState.Move = 0;
		lpObj.m_ActState.Attack = 0;
		lpObj.NextActionTime = 400;
	}

	else if ( lpObj.m_ActState.Emotion == 0 )
	{
		if ( lpObj.m_ActState.Attack )
		{
			lpObj.m_ActState.Attack = 0;
			lpObj.TargetNumber = -1;
			lpObj.NextActionTime = 500;
		}

		int actcode = rand()%2;

		if ( lpObj.m_MoveRange > 0 && !gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_STONE) && !gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_STUN) && !gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_SLEEP) &&
			!gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_FREEZE_2) && !gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_EARTH_BINDS))
		{
			gObjMonsterMoveAction(lpObj);
		}

		if ( lpObj.m_bIsMonsterAttackFirst == true )
		{
			lpObj.TargetNumber = gObjMonsterSearchEnemy(lpObj, OBJ_USER);

			if ( lpObj.TargetNumber >= 0 )
			{
				lpObj.m_ActState.Emotion = 1;
				lpObj.m_ActState.EmotionCount = 100;
				this->SetTargetMoveAllMonster(lpObj.m_nZoneIndex, lpObj.TargetNumber);
			}
		}
	}
}

int CImperialGuardian::GetMixNeedZen()
{
	return this->m_nMixNeedZen;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

