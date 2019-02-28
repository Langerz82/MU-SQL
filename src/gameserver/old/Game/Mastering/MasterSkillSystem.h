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
	bool MasterLevelUp(CGameObject &Obj, UINT64 addexp, int iMonsterType, const char* szEventType);
	UINT64 gObjNextMLExpCal(CGameObject &Obj);
	bool IsMasterLevelUser(CGameObject &Obj);
	bool CheckMLGetExp(CGameObject &Obj, CGameObject &TargetObj);
	void SetExpTable();

	void SendMLData(CGameObject &Obj);
	void InitData(CGameObject &Obj);
	int GetDieDecExpRate(CGameObject &Obj);
	int GetDieDecMoneyRate(CGameObject &Obj);

	UINT64 * gMasterExperience;
};

extern CMasterLevelSystem g_MasterLevelSystem;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

