////////////////////////////////////////////////////////////////////////////////
// VipSys.cpp
#include "StdAfx.h"
#include "VipSys.h"
#include "User/CUserData.h"
#include "protocol.h"
#include "Logging/Log.h"
#include "ChaosBox.h"
#include "MasterLevelSkillTreeSystem.h"
#include "configread.h"

CVipSystem g_VipSystem;

CVipSystem::CVipSystem()
{
	this->m_bFileLoad = false;
	this->m_mapVipInfo.clear();
	InitializeCriticalSection(&this->m_criti);
}

CVipSystem::~CVipSystem()
{
	DeleteCriticalSection(&this->m_criti);
}

void CVipSystem::ReadFile(char *filename)
{
	EnterCriticalSection(&this->m_criti);

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(filename);

	if (res.status != pugi::status_ok)
	{
		sLog->outError("Error - failed to load %s file (%s)", filename, res.description());

		LeaveCriticalSection(&this->m_criti);
		return;
	}

	this->m_bFileLoad = false;
	this->m_mapVipInfo.clear();

	pugi::xml_node vipsys = file.child("VipSystem");
	pugi::xml_node message = vipsys.child("Message");
	pugi::xml_node viptypes = vipsys.child("VipTypes");

	this->m_IsServerVIP = vipsys.attribute("LevelType").as_int(0);
	this->m_bIsSendRateChangeMessage = vipsys.attribute("SendRatesChangeMessage").as_bool(false);

	this->DayRateChangeMsg = message.attribute("Day").as_string();
	this->NightRateChangeMsg = message.attribute("Night").as_string();

	for (pugi::xml_node vip = viptypes.child("Vip"); vip; vip = vip.next_sibling())
	{
		VIP_INFO_DATA m_Vip;

		m_Vip.btType = vip.attribute("Type").as_int();
		m_Vip.szTypeName = vip.attribute("Name").as_string();
		m_Vip.wMLMonsterMinLevel = vip.attribute("ML_MonsterMinLevel").as_int();
		m_Vip.dwPointPerReset = vip.attribute("PointPerReset").as_int();
		m_Vip.wNightStartHour = vip.attribute("NightStartHour").as_int();
		m_Vip.wNightStartMinute = vip.attribute("NightStartMinute").as_int();
		m_Vip.wNightEndHour = vip.attribute("NightEndHour").as_int();
		m_Vip.wNightEndMinute = vip.attribute("NightEndMinute").as_int();

		pugi::xml_node day = vip.child("Day");
		pugi::xml_node night = vip.child("Night");

		m_Vip.m_VipEffect[VIP_EFFECT_DAY].fExpBonus = day.attribute("ExpBonus").as_float();
		m_Vip.m_VipEffect[VIP_EFFECT_DAY].iDropBonus = day.attribute("DropBonus").as_int();
		m_Vip.m_VipEffect[VIP_EFFECT_DAY].iExcDropBonus = day.attribute("ExcDropBonus").as_int();
		m_Vip.m_VipEffect[VIP_EFFECT_DAY].fMLExpBonus = day.attribute("MasterExpBonus").as_float();
		m_Vip.m_VipEffect[VIP_EFFECT_DAY].iMLDropBonus = day.attribute("MasterDropBonus").as_int();
		m_Vip.m_VipEffect[VIP_EFFECT_DAY].iMLExcDropBonus = day.attribute("MasterExcDropBonus").as_int();

		m_Vip.m_VipEffect[VIP_EFFECT_NIGHT].fExpBonus = night.attribute("ExpBonus").as_float();
		m_Vip.m_VipEffect[VIP_EFFECT_NIGHT].iDropBonus = night.attribute("DropBonus").as_int();
		m_Vip.m_VipEffect[VIP_EFFECT_NIGHT].iExcDropBonus = night.attribute("ExcDropBonus").as_int();
		m_Vip.m_VipEffect[VIP_EFFECT_NIGHT].fMLExpBonus = night.attribute("MasterExpBonus").as_float();
		m_Vip.m_VipEffect[VIP_EFFECT_NIGHT].iMLDropBonus = night.attribute("MasterDropBonus").as_int();
		m_Vip.m_VipEffect[VIP_EFFECT_NIGHT].iMLExcDropBonus = night.attribute("MasterExcDropBonus").as_int();

		pugi::xml_node chaosbox = vip.child("ChaosBoxMixRates");

		pugi::xml_node normal = chaosbox.child("Normal");

		m_Vip.m_ChaosBoxRates.PlusItem10Rate					= normal.attribute("Level10").as_int();
		m_Vip.m_ChaosBoxRates.PlusItem11Rate					= normal.attribute("Level11").as_int();
		m_Vip.m_ChaosBoxRates.PlusItem12Rate					= normal.attribute("Level12").as_int();
		m_Vip.m_ChaosBoxRates.PlusItem13Rate					= normal.attribute("Level13").as_int();
		m_Vip.m_ChaosBoxRates.PlusItem14Rate					= normal.attribute("Level14").as_int();
		m_Vip.m_ChaosBoxRates.PlusItem15Rate					= normal.attribute("Level15").as_int();
		m_Vip.m_ChaosBoxRates.PlusItemAddLuckRate			    = normal.attribute("AddLuck").as_int();

		pugi::xml_node wing = chaosbox.child("Wing");

		m_Vip.m_ChaosBoxRates.Wing2MaxRate						= wing.attribute("Second").as_int();
		m_Vip.m_ChaosBoxRates.Wing25MaxRate						= wing.attribute("Monster").as_int();
		m_Vip.m_ChaosBoxRates.Wing3MaxRate						= wing.attribute("Third").as_int();
		m_Vip.m_ChaosBoxRates.CapeLordMaxRate					= wing.attribute("Cape").as_int();
		m_Vip.m_ChaosBoxRates.FeatherCondorMaxRate				= wing.attribute("FeatherOfCondor").as_int();

		this->m_mapVipInfo.insert(std::pair<int, VIP_INFO_DATA>(m_Vip.btType, m_Vip));
	}

	this->m_bFileLoad = true;
	sLog->outBasic("[VIP System] Loaded %d VIP types", this->m_mapVipInfo.size());
	LeaveCriticalSection(&this->m_criti);
}

void CVipSystem::Run()
{
	SYSTEMTIME m_time;
	GetLocalTime(&m_time);

	for (int i = g_ConfigRead.server.GetObjectStartUserIndex(); i < g_ConfigRead.server.GetObjectMax(); i++)
	{
		if (gGameObjects[i].Connected < PLAYER_PLAYING)
		{
			continue;
		}

		if (gGameObjects[i].Type != OBJ_USER)
		{
			continue;
		}

		if (gGameObjects[i].m_PlayerData->VipType == 0)
		{
			continue;
		}

		EnterCriticalSection(&this->m_criti);

		std::map<int, VIP_INFO_DATA>::iterator it = this->m_mapVipInfo.find(gGameObjects[i].m_PlayerData->VipType);

		if (it == this->m_mapVipInfo.end())
		{
			LeaveCriticalSection(&this->m_criti);
			continue;
		}

		SYSTEMTIME m_time;
		int iVipEffect = -1;
		GetLocalTime(&m_time);

		if ((m_time.wHour * 60 + m_time.wMinute) >= (it->second.wNightStartHour * 60 + it->second.wNightStartMinute))
		{
			if ((m_time.wHour * 60 + m_time.wMinute) < (it->second.wNightEndHour * 60 + it->second.wNightEndMinute))
			{
				iVipEffect = VIP_EFFECT_NIGHT;
			}

			else
			{
				iVipEffect = VIP_EFFECT_DAY;
			}
		}

		else
		{
			iVipEffect = VIP_EFFECT_DAY;
		}

		LeaveCriticalSection(&this->m_criti);

		if (iVipEffect != gGameObjects[i].m_PlayerData->VipEffect)
		{
			gGameObjects[i].m_PlayerData->VipEffect = iVipEffect;

			if (this->m_bIsSendRateChangeMessage == true)
			{
				if (iVipEffect == VIP_EFFECT_DAY)
				{
					GSProtocol.GCServerMsgStringSend((char *)this->DayRateChangeMsg.c_str(), i, 1);
				}

				else if(iVipEffect == VIP_EFFECT_NIGHT)
				{
					GSProtocol.GCServerMsgStringSend((char *)this->NightRateChangeMsg.c_str(), i, 1);
				}
			}
		}
	}
}

void CVipSystem::SetVipForUser(CGameObject* lpObj, BYTE btVipType)
{
	if (btVipType == 0)
	{
		return;
	}

	EnterCriticalSection(&this->m_criti);

	std::map<int, VIP_INFO_DATA>::iterator it = this->m_mapVipInfo.find(btVipType);

	if (it == this->m_mapVipInfo.end())
	{
		LeaveCriticalSection(&this->m_criti);
		return;
	}

	lpObj->m_PlayerData->VipType = btVipType;

	SYSTEMTIME m_time;
	GetLocalTime(&m_time);

	if ((m_time.wHour * 60 + m_time.wMinute) >= (it->second.wNightStartHour * 60 + it->second.wNightStartMinute))
	{
		if ((m_time.wHour * 60 + m_time.wMinute) < (it->second.wNightEndHour * 60 + it->second.wNightEndMinute))
		{
			lpObj->m_PlayerData->VipEffect = VIP_EFFECT_NIGHT;
		}

		else
		{
			lpObj->m_PlayerData->VipEffect = VIP_EFFECT_DAY;
		}
	}

	else
	{
		lpObj->m_PlayerData->VipEffect = VIP_EFFECT_DAY;
	}

	LeaveCriticalSection(&this->m_criti);
}

float CVipSystem::GetExpBonus(CGameObject* lpObj)
{
	if (lpObj->Type != OBJ_USER)
	{
		return 0;
	}

	EnterCriticalSection(&this->m_criti);

	std::map<int, VIP_INFO_DATA>::iterator it = this->m_mapVipInfo.find(lpObj->m_PlayerData->VipType);

	if (it == this->m_mapVipInfo.end())
	{
		LeaveCriticalSection(&this->m_criti);
		return 0;
	}

	if (lpObj->m_PlayerData->VipEffect < 0 || lpObj->m_PlayerData->VipEffect > 1)
	{
		LeaveCriticalSection(&this->m_criti);
		return 0;
	}

	float fEffect = 0;

	if (g_MasterLevelSkillTreeSystem.IsMasterLevelUser(lpObj))
	{
		fEffect = it->second.m_VipEffect[lpObj->m_PlayerData->VipEffect].fMLExpBonus;
	}

	else
	{
		fEffect = it->second.m_VipEffect[lpObj->m_PlayerData->VipEffect].fExpBonus;
	}

	LeaveCriticalSection(&this->m_criti);
	return fEffect;
}

int CVipSystem::GetDropBonus(CGameObject* lpObj)
{
	if (lpObj->Type != OBJ_USER)
	{
		return 0;
	}

	EnterCriticalSection(&this->m_criti);

	std::map<int, VIP_INFO_DATA>::iterator it = this->m_mapVipInfo.find(lpObj->m_PlayerData->VipType);

	if (it == this->m_mapVipInfo.end())
	{
		LeaveCriticalSection(&this->m_criti);
		return 0;
	}

	if (lpObj->m_PlayerData->VipEffect < 0 || lpObj->m_PlayerData->VipEffect > 1)
	{
		LeaveCriticalSection(&this->m_criti);
		return 0;
	}

	int iEffect = 0;

	if (g_MasterLevelSkillTreeSystem.IsMasterLevelUser(lpObj))
	{
		iEffect = it->second.m_VipEffect[lpObj->m_PlayerData->VipEffect].iMLDropBonus;
	}

	else
	{
		iEffect = it->second.m_VipEffect[lpObj->m_PlayerData->VipEffect].iDropBonus;
	}

	LeaveCriticalSection(&this->m_criti);
	return iEffect;
}

int CVipSystem::GetExcDropBonus(CGameObject* lpObj)
{
	if (lpObj->Type != OBJ_USER)
	{
		return 0;
	}

	EnterCriticalSection(&this->m_criti);

	std::map<int, VIP_INFO_DATA>::iterator it = this->m_mapVipInfo.find(lpObj->m_PlayerData->VipType);

	if (it == this->m_mapVipInfo.end())
	{
		LeaveCriticalSection(&this->m_criti);
		return 0;
	}

	if (lpObj->m_PlayerData->VipEffect < 0 || lpObj->m_PlayerData->VipEffect > 1)
	{
		LeaveCriticalSection(&this->m_criti);
		return 0;
	}

	int iEffect = 0;

	if (g_MasterLevelSkillTreeSystem.IsMasterLevelUser(lpObj))
	{
		iEffect = it->second.m_VipEffect[lpObj->m_PlayerData->VipEffect].iMLExcDropBonus;
	}

	else
	{
		iEffect = it->second.m_VipEffect[lpObj->m_PlayerData->VipEffect].iExcDropBonus;
	}

	LeaveCriticalSection(&this->m_criti);
	return iEffect;
}

WORD CVipSystem::GetMLMonsterMinLevel(OBJECTSTRUCT * lpObj)
{
	if (lpObj->Type != OBJ_USER)
	{
		return 0;
	}

	EnterCriticalSection(&this->m_criti);

	std::map<int, VIP_INFO_DATA>::iterator it = this->m_mapVipInfo.find(lpObj->m_PlayerData->VipType);

	if (it == this->m_mapVipInfo.end())
	{
		LeaveCriticalSection(&this->m_criti);
		return 0;
	}

	WORD wEffect = it->second.wMLMonsterMinLevel;

	LeaveCriticalSection(&this->m_criti);
	return wEffect;
}

char * CVipSystem::GetVipName(BYTE btVipType)
{
	EnterCriticalSection(&this->m_criti);

	std::map<int, VIP_INFO_DATA>::iterator it = this->m_mapVipInfo.find(btVipType);

	if (it == this->m_mapVipInfo.end())
	{
		LeaveCriticalSection(&this->m_criti);
		return "Error";
	}

	char * szVipName = (char *)it->second.szTypeName.c_str();

	LeaveCriticalSection(&this->m_criti);
	return szVipName;
}

DWORD CVipSystem::GetPointPerReset(OBJECTSTRUCT *lpObj)
{
	if (lpObj->Type != OBJ_USER)
	{
		return 0;
	}

	EnterCriticalSection(&this->m_criti);

	std::map<int, VIP_INFO_DATA>::iterator it = this->m_mapVipInfo.find(lpObj->m_PlayerData->VipType);

	if (it == this->m_mapVipInfo.end())
	{
		LeaveCriticalSection(&this->m_criti);
		return 0;
	}

	DWORD dwEffect = it->second.dwPointPerReset;

	LeaveCriticalSection(&this->m_criti);
	return dwEffect;
}

int CVipSystem::GetPlusItemMixRate(CGameObject* lpObj, int iMixType)
{
	if (lpObj->Type != OBJ_USER)
	{
		return 0;
	}

	EnterCriticalSection(&this->m_criti);

	std::map<int, VIP_INFO_DATA>::iterator it = this->m_mapVipInfo.find(lpObj->m_PlayerData->VipType);

	if (it == this->m_mapVipInfo.end())
	{
		LeaveCriticalSection(&this->m_criti);
		return 0;
	}
	
	int iEffect = 0;

	if (iMixType == CHAOS_TYPE_UPGRADE_10)
	{
		iEffect = it->second.m_ChaosBoxRates.PlusItem10Rate;
	}

	else if (iMixType == CHAOS_TYPE_UPGRADE_11)
	{
		iEffect = it->second.m_ChaosBoxRates.PlusItem11Rate;
	}

	else if (iMixType == CHAOS_TYPE_UPGRADE_12)
	{
		iEffect = it->second.m_ChaosBoxRates.PlusItem12Rate;
	}

	else if (iMixType == CHAOS_TYPE_UPGRADE_13)
	{
		iEffect = it->second.m_ChaosBoxRates.PlusItem13Rate;
	}

	else if (iMixType == CHAOS_TYPE_UPGRADE_14)
	{
		iEffect = it->second.m_ChaosBoxRates.PlusItem14Rate;
	}

	else if (iMixType == CHAOS_TYPE_UPGRADE_15)
	{
		iEffect = it->second.m_ChaosBoxRates.PlusItem15Rate;
	}

	else
	{
		iEffect = 0;
	}

	LeaveCriticalSection(&this->m_criti);
	return iEffect;
}

int CVipSystem::GetPlusItemAddLuckRate(CGameObject* lpObj)
{
	if (lpObj->Type != OBJ_USER)
	{
		return 0;
	}

	EnterCriticalSection(&this->m_criti);

	std::map<int, VIP_INFO_DATA>::iterator it = this->m_mapVipInfo.find(lpObj->m_PlayerData->VipType);

	if (it == this->m_mapVipInfo.end())
	{
		LeaveCriticalSection(&this->m_criti);
		return 0;
	}
	
	int iEffect = it->second.m_ChaosBoxRates.PlusItemAddLuckRate;

	LeaveCriticalSection(&this->m_criti);
	return iEffect;
}

int CVipSystem::GetWing2ndRate(OBJECTSTRUCT * lpObj)
{
	if (lpObj->Type != OBJ_USER)
	{
		return 0;
	}

	EnterCriticalSection(&this->m_criti);

	std::map<int, VIP_INFO_DATA>::iterator it = this->m_mapVipInfo.find(lpObj->m_PlayerData->VipType);

	if (it == this->m_mapVipInfo.end())
	{
		LeaveCriticalSection(&this->m_criti);
		return 0;
	}

	int iEffect = it->second.m_ChaosBoxRates.Wing2MaxRate;

	LeaveCriticalSection(&this->m_criti);
	return iEffect;
}

int CVipSystem::GetWing25Rate(OBJECTSTRUCT * lpObj)
{
	if (lpObj->Type != OBJ_USER)
	{
		return 0;
	}

	EnterCriticalSection(&this->m_criti);

	std::map<int, VIP_INFO_DATA>::iterator it = this->m_mapVipInfo.find(lpObj->m_PlayerData->VipType);

	if (it == this->m_mapVipInfo.end())
	{
		LeaveCriticalSection(&this->m_criti);
		return 0;
	}

	int iEffect = it->second.m_ChaosBoxRates.Wing25MaxRate;

	LeaveCriticalSection(&this->m_criti);
	return iEffect;
}

int CVipSystem::GetWing3rdRate(OBJECTSTRUCT * lpObj)
{
	if (lpObj->Type != OBJ_USER)
	{
		return 0;
	}

	EnterCriticalSection(&this->m_criti);

	std::map<int, VIP_INFO_DATA>::iterator it = this->m_mapVipInfo.find(lpObj->m_PlayerData->VipType);

	if (it == this->m_mapVipInfo.end())
	{
		LeaveCriticalSection(&this->m_criti);
		return 0;
	}

	int iEffect = it->second.m_ChaosBoxRates.Wing3MaxRate;

	LeaveCriticalSection(&this->m_criti);
	return iEffect;
}

int CVipSystem::GetCapeOfLordRate(OBJECTSTRUCT * lpObj)
{
	if (lpObj->Type != OBJ_USER)
	{
		return 0;
	}

	EnterCriticalSection(&this->m_criti);

	std::map<int, VIP_INFO_DATA>::iterator it = this->m_mapVipInfo.find(lpObj->m_PlayerData->VipType);

	if (it == this->m_mapVipInfo.end())
	{
		LeaveCriticalSection(&this->m_criti);
		return 0;
	}

	int iEffect = it->second.m_ChaosBoxRates.CapeLordMaxRate;

	LeaveCriticalSection(&this->m_criti);
	return iEffect;
}

int CVipSystem::GetFeatherOfCondorRate(OBJECTSTRUCT * lpObj)
{
	if (lpObj->Type != OBJ_USER)
	{
		return 0;
	}

	EnterCriticalSection(&this->m_criti);

	std::map<int, VIP_INFO_DATA>::iterator it = this->m_mapVipInfo.find(lpObj->m_PlayerData->VipType);

	if (it == this->m_mapVipInfo.end())
	{
		LeaveCriticalSection(&this->m_criti);
		return 0;
	}

	int iEffect = it->second.m_ChaosBoxRates.FeatherCondorMaxRate;

	LeaveCriticalSection(&this->m_criti);
	return iEffect;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

