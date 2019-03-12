// CastleDeepEvent.cpp: implementation of the CCastleDeepEvent class.
// GS-CS	1.00.18	JPN	0x00593630	-	Completed
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CastleSiege.h"
#include "User/CUserData.h"
#include "Logging/Log.h"
#include "CastleDeepEvent.h"
#include "configread.h"
#include "Main.h"
#include "GOFunctions.h"

using namespace std;
CCastleDeepEvent g_CastleDeepEvent;

CCastleDeepEvent::CCastleDeepEvent()
{
	this->m_bDoEvent = FALSE;
	this->m_bHasData = FALSE;

	srand(time(NULL));

	InitializeCriticalSection(&this->m_critEventData);
}

CCastleDeepEvent::~CCastleDeepEvent()
{
	DeleteCriticalSection(&this->m_critEventData);
}

void CCastleDeepEvent::StartEvent()
{
	if(m_vtAssultType.size() <= 0)
	{
		sLog->outBasic("[CastleDeep Event] Start Fail - No Assult Data");
		SetState(CD_STATE_NONE);
		return;
	}

	int iCheckCount = 100;
	int iASSULT_RATE = 0;
	int bDoStart = FALSE;

	while(iCheckCount-- > 0)
	{
		int iRndIndex = rand()%m_vtAssultType.size();

		this->m_iCUR_ASSULT_TYPE = m_vtAssultType[iRndIndex];
		iASSULT_RATE = m_vtAssultTypeRate[iRndIndex];

		if(rand()%100 < iASSULT_RATE)
		{
			bDoStart = TRUE;
			break;
		}
	}

	if(bDoStart == FALSE)
	{
		sLog->outBasic("[CastleDeep Event] Start Fail - bDoStart == FALSE");
		SetState(CD_STATE_NONE);
		return;
	}

	this->m_iCUR_SPAWN_STEP = 0;

	std::map<int,std::vector<CASTLEDEEP_SPAWNTIME> >::iterator it = m_mapSpawnTime.find(m_iCUR_ASSULT_TYPE);

	if(it == m_mapSpawnTime.end())
	{
		SetState(CD_STATE_NONE);
		return;
	}

	m_mapRunCheck.clear();

	for(std::vector<CASTLEDEEP_SPAWNTIME>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
	{
		std::vector<CASTLEDEEP_SPAWNTIME>::iterator stSpawnTime = it2;

		m_mapRunCheck.insert( pair<int,int>(stSpawnTime->m_iMonsterGroup,stSpawnTime->m_iMonsterSpawnMinute) );
	}
}

void CCastleDeepEvent::StopEvent()
{
}

void CCastleDeepEvent::Init()
{
	if (g_ConfigRead.server.GetStateFromEventTable(g_ConfigRead.server.GetServerType(), EV_TABLE_DEEP) == false)
	{
		this->SetEventEnable(false);
	}

	if(this->m_bDoEvent != 0)
	{
		this->SetState(CD_STATE_CLOSED);
	}

	else
	{
		this->SetState(CD_STATE_NONE);
	}
}

int CCastleDeepEvent::Load(char * lpszFileName)
{
	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(lpszFileName);

	if(res.status != pugi::status_ok)
	{
		sLog->outError("[CastleDeep Event] Info file Load Fail [%s] [%s]", lpszFileName, res.description());
		return false;
	}

	EnterCriticalSection(&this->m_critEventData);
	this->m_vtAssultType.clear();
	this->m_vtAssultTypeRate.clear();
	this->m_mapSpawnTime.clear();
	this->m_mapMonsterInfo.clear();
	this->m_vtEventTime.clear();
	this->m_mapRunCheck.clear();
	LeaveCriticalSection(&this->m_critEventData);

	this->m_bHasData = FALSE;

	pugi::xml_node mainXML = file.child("LorenDeepEvent");
	this->m_bDoEvent = g_ConfigRead.server.GetStateFromEventTable(g_ConfigRead.server.GetServerType(), EV_TABLE_DEEP) == true ? mainXML.attribute("Enable").as_int() : false;

	pugi::xml_node schedule = mainXML.child("Schedule");

	for (pugi::xml_node start = schedule.child("Start"); start; start = start.next_sibling())
	{
		CASTLEDEEP_EVENTTIME pRET;

		pRET.m_iHour = start.attribute("Hour").as_int();
		pRET.m_iMinute = start.attribute("Minute").as_int();

		this->m_vtEventTime.push_back(pRET);
	}

	pugi::xml_node assaulttype = mainXML.child("AssaultTypeSettings");

	for (pugi::xml_node assault = assaulttype.child("Assault"); assault; assault = assault.next_sibling())
	{
		vector<CASTLEDEEP_MONSTERINFO> vtTempMonsterInfo;
		vector<CASTLEDEEP_SPAWNTIME> vtTempSpawnTime;
		int iAssultType;
		int iAssultRate;

		iAssultType = assault.attribute("Type").as_int();
		iAssultRate = assault.attribute("AppearanceRate").as_int();

		this->m_mapMonsterInfo.insert(pair <int, vector<CASTLEDEEP_MONSTERINFO > >(iAssultType,vtTempMonsterInfo));
		this->m_mapSpawnTime.insert(pair<int,vector<CASTLEDEEP_SPAWNTIME > >(iAssultType,vtTempSpawnTime));
		this->m_vtAssultType.push_back(iAssultType);
		this->m_vtAssultTypeRate.push_back(iAssultRate);
	}

	pugi::xml_node assaultgroup = mainXML.child("AssaultGroupSettings");

	for (pugi::xml_node assault = assaultgroup.child("Assault"); assault; assault = assault.next_sibling())
	{
		CASTLEDEEP_SPAWNTIME stSpawnTime;
		int iAssultType;

		iAssultType = assault.attribute("Type").as_int();
		stSpawnTime.m_iMonsterGroup = assault.attribute("MonsterGroup").as_int();
		stSpawnTime.m_iMonsterSpawnMinute = assault.attribute("SpawnTime").as_int();

		EnterCriticalSection(&this->m_critEventData);

		map<int,vector<CASTLEDEEP_SPAWNTIME>,less<int> >::iterator it = this->m_mapSpawnTime.find(iAssultType);
		
		if(it != this->m_mapSpawnTime.end())
		{
			it->second.push_back(stSpawnTime);
		}

		LeaveCriticalSection(&this->m_critEventData);
	}

	pugi::xml_node assaultmember = mainXML.child("AssaultGroupMember");

	for(pugi::xml_node assault = assaultmember.child("Assault"); assault; assault = assault.next_sibling())
	{
		CASTLEDEEP_MONSTERINFO stMonsterInfo;
		int iAssultType;

		iAssultType = assault.attribute("Type").as_int();
		stMonsterInfo.m_iMonsterGroup = assault.attribute("MonsterGroup").as_int();
		stMonsterInfo.m_iMonsterType = assault.attribute("MonsterIndex").as_int();
		stMonsterInfo.m_iMonsterNumber = assault.attribute("MonsterCount").as_int();
		stMonsterInfo.m_iSX = assault.attribute("StartX").as_int();
		stMonsterInfo.m_iSY = assault.attribute("StartY").as_int();
		stMonsterInfo.m_iDX = assault.attribute("EndX").as_int();
		stMonsterInfo.m_iDY = assault.attribute("EndY").as_int();

		EnterCriticalSection(&this->m_critEventData);
		
		map<int,vector<CASTLEDEEP_MONSTERINFO >,less<int> >::iterator it = this->m_mapMonsterInfo.find(iAssultType);

		if(it != this->m_mapMonsterInfo.end())
		{
			it->second.push_back(stMonsterInfo);
		}
		
		LeaveCriticalSection(&this->m_critEventData);
		
	}

	this->m_bHasData = TRUE;
	return true;
}

void CCastleDeepEvent::CheckSync()
{
	if(this->m_vtEventTime.empty())
	{
		sLog->outError( "[CastleDeep Event] No Schedule Time Data");
		this->SetState(CD_STATE_NONE);
		return;
	}

	_SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	int iMIN_HOUR = 24;
	int iMIN_MINUTE = 60;
	int bTIME_CHANGED = 0;
	std::vector<CASTLEDEEP_EVENTTIME>::iterator it;
	for(it = this->m_vtEventTime.begin(); it != this->m_vtEventTime.end();it++)
	{
		std::vector<CASTLEDEEP_EVENTTIME>::iterator pRET = it;

		if( (sysTime.wHour * 60 + sysTime.wMinute) < (pRET->m_iHour * 60 + pRET->m_iMinute) )
		{
			if( (iMIN_HOUR * 60 + iMIN_MINUTE) > (pRET->m_iHour * 60 + pRET->m_iMinute) )
			{
				bTIME_CHANGED = 1;
				iMIN_HOUR = pRET->m_iHour;
				iMIN_MINUTE = pRET->m_iMinute;
			}
		}
	}

	if(bTIME_CHANGED == 0)
	{
		for(it = this->m_vtEventTime.begin(),iMIN_HOUR = 24,iMIN_MINUTE = 60; it != this->m_vtEventTime.end();it++)
		{
			std::vector<CASTLEDEEP_EVENTTIME>::iterator pRET = it;

			if( (iMIN_HOUR * 60 + iMIN_MINUTE) > (pRET->m_iHour * 60 + pRET->m_iMinute) )
			{
				bTIME_CHANGED = 2;
				iMIN_HOUR = pRET->m_iHour;
				iMIN_MINUTE = pRET->m_iMinute;
			}
		}
	}

	switch(bTIME_CHANGED)
	{
	case 1:
		this->m_iTIME_MSEC_REMAIN = ( ( (iMIN_HOUR * 60) * 60 + iMIN_MINUTE * 60 ) - ( (sysTime.wHour * 60) * 60 + sysTime.wMinute * 60 + sysTime.wSecond) ) * 1000;
		break;
	case 2:
		this->m_iTIME_MSEC_REMAIN = ( ( ((iMIN_HOUR + 24) * 60) * 60 + iMIN_MINUTE * 60 ) - ( (sysTime.wHour * 60) * 60 + sysTime.wMinute * 60 + sysTime.wSecond ) ) * 1000;
		break;
	default:
		sLog->outError( "[CastleDeep Event] No Schedule Time Data");
		this->SetState(CD_STATE_NONE);
		return;
	}

	this->m_iTIME_TICK_COUNT = GetTickCount();
	this->m_iTIME_NOTIFY_COUNT = 5;
}

void CCastleDeepEvent::SetState(int iSTATE)
{
	if(m_iEVENT_STATE < 0 || m_iEVENT_STATE > 2)
		return;

	m_iEVENT_STATE = iSTATE;

	switch(m_iEVENT_STATE)
	{
		case 0: SetState_None(); break;
		case 1: SetState_Closed(); break;
		case 2: SetState_Playing(); break;
	}
}

void CCastleDeepEvent::SetState_None()
{
	this->StopEvent();
}

void CCastleDeepEvent::SetState_Closed()
{
	StopEvent();

	if(m_bDoEvent != 0)
	{
		CheckSync();
	}
	else
	{
		SetState(CD_STATE_NONE);
	}
}

void CCastleDeepEvent::SetState_Playing()
{
	m_iTIME_MSEC_REMAIN = 0;
	m_bCheckEnd = 0;
	m_uCheckEndTick = 0;
	ClearMonster();
	StartEvent();
}



void CCastleDeepEvent::Run()
{
	if(m_bDoEvent)
	{
		switch(m_iEVENT_STATE)
		{
			case 0: ProcState_None(); break;
			case 1: ProcState_Closed(); break;
			case 2: ProcState_Playing(); break;
		}
	}
}

void CCastleDeepEvent::ProcState_Closed()
{
	int iTICK_MSEC = GetTickCount() - m_iTIME_TICK_COUNT;

	if(iTICK_MSEC >= 1000)
	{
		m_iTIME_MSEC_REMAIN -= iTICK_MSEC;
		m_iTIME_TICK_COUNT = GetTickCount();

		if(m_bDoEvent != 0)
		{
			if(m_iTIME_MSEC_REMAIN <= 300000
				&& m_iTIME_MSEC_REMAIN > 0
				&& m_iTIME_MSEC_REMAIN / 60000 != m_iTIME_NOTIFY_COUNT)
			{
				m_iTIME_NOTIFY_COUNT = m_iTIME_MSEC_REMAIN / 60000;

				if(m_iTIME_NOTIFY_COUNT + 1 == 5)
				{
					char szTemp[256];
					wsprintf(szTemp, Lang.GetText(0,213));
					g_CastleSiege.SendMapServerGroupMsg(szTemp);
				}
			}
		}

		sLog->outBasic(" [CastleDeep Event] ProcState_Closed()	LEFT-MIN:%d", m_iTIME_MSEC_REMAIN/60000);
	}

	if(m_iTIME_MSEC_REMAIN <= 0)
	{
		char szTemp[256];
		wsprintf(szTemp, Lang.GetText(0,214));
		g_CastleSiege.SendMapServerGroupMsg(szTemp);

		if(m_bDoEvent != 0)
		{
			SetState(CD_STATE_PLAYING);
		}
		else
		{
			SetState(CD_STATE_NONE);
		}

		sLog->outBasic("[CastleDeep Event] - Event Started");
	}
}


void CCastleDeepEvent::ProcState_Playing()
{
using namespace std;
	int iTICK_MSEC = GetTickCount() - m_iTIME_TICK_COUNT;

	if(iTICK_MSEC >= 1000)
	{
		m_iTIME_MSEC_REMAIN +=  iTICK_MSEC;
		m_iTIME_TICK_COUNT = GetTickCount();

		for(map<int,int>::iterator it = m_mapRunCheck.begin();it != m_mapRunCheck.end();it++)
		{
			int iSpawnMin = it->second;

			if(iSpawnMin == m_iTIME_MSEC_REMAIN / 60000)
			{
				AddMonster(m_iCUR_ASSULT_TYPE,it->first);
				m_mapRunCheck.erase(it);
				break;
			}
		}

		sLog->outBasic("[CastleDeep Event] ProcState_Playing() LEFT-MSEC:%d",m_iTIME_MSEC_REMAIN/60000);
	}

	if(m_mapRunCheck.size() <= 0)
	{
		if(m_bCheckEnd == 0)
		{
			m_bCheckEnd = 1;
			m_uCheckEndTick = GetTickCount();
		}
		else
		{
			if(m_uCheckEndTick + 600000 < GetTickCount())
			{
				char szTemp[256];

				wsprintf(szTemp, Lang.GetText(0,215));
				GSProtocol.AllSendServerMsg(szTemp);

				if(m_bDoEvent != 0)
				{
					SetState(CD_STATE_CLOSED);
				}
				else
				{
					SetState(CD_STATE_NONE);
				}
			}
		}

		sLog->outBasic("[CastleDeep Event] - Event Ended");
	}
}

void CCastleDeepEvent::ClearMonster()
{
	for each (std::pair<int,CGameObject*> user in gGameObjects)
	{
		if(user.second->m_Attribute == 62)
		{
			gObjDel(*user.second);
		}
	}
}

void CCastleDeepEvent::AddMonster(int iAssultType,int iGroup)
{
	map<int,vector<CASTLEDEEP_MONSTERINFO >,less<int> >::iterator it = this->m_mapMonsterInfo.find(iAssultType);

	if(it == this->m_mapMonsterInfo.end())
	{
		return;
	}

	std::vector<CASTLEDEEP_MONSTERINFO>::iterator it2;

	for(it2 = it->second.begin(); it2 != it->second.end(); it2++)
	{
		std::vector<CASTLEDEEP_MONSTERINFO>::iterator stMonsterInfo = it2;

		if(stMonsterInfo->m_iMonsterGroup != iGroup)
			continue;

		BYTE cSX;
		BYTE cSY;

		for(int iMonCount = 0; iMonCount < stMonsterInfo->m_iMonsterNumber; iMonCount++)
		{
			if(gObjGetRandomFreeArea(MAP_INDEX_CASTLESIEGE,(BYTE &)cSX,(BYTE &)cSY,
				stMonsterInfo->m_iSX,stMonsterInfo->m_iSY,stMonsterInfo->m_iDX,stMonsterInfo->m_iDY,30) == TRUE)
			{
				int result = gObjAddMonster(MAP_INDEX_CASTLESIEGE);
				CGameObject* lpMonster = getGameObject(result);
				if(result >= 0)
				{
					lpMonster->m_PosNum = -1;
					lpMonster->X = cSX;
					lpMonster->Y = cSY;
					lpMonster->MapNumber = MAP_INDEX_CASTLESIEGE;
					lpMonster->TX = lpMonster->X;
					lpMonster->TY = lpMonster->Y;
					lpMonster->m_OldX = lpMonster->X;
					lpMonster->m_OldY = lpMonster->Y;
					lpMonster->Dir = 1;
					lpMonster->StartX = lpMonster->X;
					lpMonster->StartY = lpMonster->Y;

					gObjSetMonster(*lpMonster, stMonsterInfo->m_iMonsterType);

					lpMonster->m_Attribute = 62;
					lpMonster->MaxRegenTime = 0;
					lpMonster->Dir = rand()%8;
					lpMonster->DieRegen = 0;
					lpMonster->RegenTime = 1;
					lpMonster->MaxRegenTime = 1000;
					lpMonster->m_dwLastCheckTick = GetTickCount();
				}
			}
		}
	}
}

