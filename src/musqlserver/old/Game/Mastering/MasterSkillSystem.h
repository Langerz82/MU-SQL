////////////////////////////////////////////////////////////////////////////////
// MasterSkillSystem.h
#ifndef MASTER_SKILL_SYSTEM
#define MASTER_SKILL_SYSTEM

#include "StdAfx.h"

struct MASTERLEVEL_PASSIVE_SKILL_OPTION;

class CMasterLevelSystem
{
public:
	CMasterLevelSystem(void);
	virtual ~CMasterLevelSystem(void);
	bool MasterLevelUp(CGameObject &lpObj, UINT64 addexp, int iMonsterType, const char* szEventType);
	UINT64 gObjNextMLExpCal(CGameObject &lpObj);
	bool IsMasterLevelUser(CGameObject &lpObj);
	bool CheckMLGetExp(CGameObject &lpObj, CGameObject &lpTargetObj);
	void SetExpTable();

	void SendMLData(CGameObject &lpObj);
	void InitData(CGameObject &lpObj);
	int GetDieDecExpRate(CGameObject &lpObj);
	int GetDieDecMoneyRate(CGameObject &lpObj);

	UINT64 * gMasterExperience;
};

extern CMasterLevelSystem g_MasterLevelSystem;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

