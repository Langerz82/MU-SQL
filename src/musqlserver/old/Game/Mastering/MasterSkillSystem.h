////////////////////////////////////////////////////////////////////////////////
// MasterSkillSystem.h
#ifndef MASTER_SKILL_SYSTEM
#define MASTER_SKILL_SYSTEM

#include "StdAfx.h"


class CMasterLevelSystem
{
public:
	CMasterLevelSystem(void);
	virtual ~CMasterLevelSystem(void);
	bool MasterLevelUp(LPGameObject &lpObj, UINT64 addexp, int iMonsterType, const char* szEventType);
	UINT64 gObjNextMLExpCal(LPGameObject &lpObj);
	bool IsMasterLevelUser(LPGameObject &lpObj);
	bool CheckMLGetExp(LPGameObject &lpObj, LPGameObject &lpTargetObj);
	void SetExpTable();

	void SendMLData(LPGameObject &lpObj);
	void InitData(LPGameObject &lpObj);
	int GetDieDecExpRate(LPGameObject &lpObj);
	int GetDieDecMoneyRate(LPGameObject &lpObj);

	UINT64 * gMasterExperience;
};
//extern CMasterSkillSystem g_MasterLevelSystem;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

