////////////////////////////////////////////////////////////////////////////////
// VipSys.h
#ifndef VIPSYS_H
#define VIPSYS_H

#include "user.h"

#define VIP_EFFECT_NONE -1
#define VIP_EFFECT_DAY 0
#define VIP_EFFECT_NIGHT 1

#define MAX_VIP_EFFECTS 2

struct VIP_DAYNIGHT_EFFECT
{
	float fExpBonus;
	int iDropBonus;
	int iExcDropBonus;
	float fMLExpBonus;
	int iMLDropBonus;
	int iMLExcDropBonus;
};

struct VIP_CHAOSBOX_RATES
{
	BYTE PlusItem10Rate;
	BYTE PlusItem11Rate;
	BYTE PlusItem12Rate;
	BYTE PlusItem13Rate;
	BYTE PlusItem14Rate;
	BYTE PlusItem15Rate;
	BYTE PlusItemAddLuckRate;

	BYTE SocketWeaponMaxRate;

	BYTE Wing2MaxRate;
	BYTE Wing25MaxRate;
	BYTE Wing3MaxRate;
	BYTE CapeLordMaxRate;
	BYTE FeatherCondorMaxRate;
};

struct VIP_INFO_DATA
{
	BYTE btType;
	std::string szTypeName;
	WORD wMLMonsterMinLevel;
	WORD wNightStartHour;
	WORD wNightStartMinute;
	WORD wNightEndHour;
	WORD wNightEndMinute;
	DWORD dwPointPerReset;
	VIP_DAYNIGHT_EFFECT m_VipEffect[MAX_VIP_EFFECTS];
	VIP_CHAOSBOX_RATES m_ChaosBoxRates;
};

class CVipSystem
{
public:

	CVipSystem();
	~CVipSystem();

	void ReadFile(char *filename);

	void Run();
	void SetVipForUser(OBJECTSTRUCT * lpObj, BYTE btVipType);

	float GetExpBonus(OBJECTSTRUCT * lpObj);
	int GetDropBonus(OBJECTSTRUCT * lpObj);
	int GetExcDropBonus(OBJECTSTRUCT * lpObj);
	WORD GetMLMonsterMinLevel(OBJECTSTRUCT * lpObj);
	DWORD GetPointPerReset(OBJECTSTRUCT * lpObj);
	int GetPlusItemMixRate(OBJECTSTRUCT * lpObj, int iMixType);
	int GetPlusItemAddLuckRate(OBJECTSTRUCT * lpObj);
	int GetWing2ndRate(OBJECTSTRUCT * lpObj);
	int GetWing25Rate(OBJECTSTRUCT * lpObj);
	int GetWing3rdRate(OBJECTSTRUCT * lpObj);
	int GetCapeOfLordRate(OBJECTSTRUCT * lpObj);
	int GetFeatherOfCondorRate(OBJECTSTRUCT * lpObj);

	int IsServerVIP() { return this->m_IsServerVIP; }
	char * GetVipName(BYTE btVipType);

private:
	bool m_bFileLoad;
	int m_IsServerVIP;

	bool m_bIsSendRateChangeMessage;
	std::string DayRateChangeMsg;
	std::string NightRateChangeMsg;

	std::map<int, VIP_INFO_DATA> m_mapVipInfo;
	CRITICAL_SECTION m_criti;

}; 

extern CVipSystem g_VipSystem;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

