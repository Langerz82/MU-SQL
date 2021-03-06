////////////////////////////////////////////////////////////////////////////////
// SkillUseTime.h
#ifndef SKILL_USE_TIME
#define SKILL_USE_TIME

#include "StdAfx.h"
#include "User/CUserData.h"

struct SKILL_TIME_INFO
{
	WORD iSkill;
	DWORD iMinMagicSpeed;
	DWORD iMaxMagicSpeed;
	ULONGLONG iUseTime;
};

#pragma once
class CSkillUseTime
{
public:
	CSkillUseTime(void);
	virtual ~CSkillUseTime(void);

	bool LoadFile(LPSTR lpFile);
	bool LoadConfig(LPSTR lpFile);
	bool CheckSkillTime(CGameObject &Obj, int Skill);

private:
	std::vector<SKILL_TIME_INFO> m_vtSkillTimeInfo;
	bool m_bFileLoad;
	bool m_bEnabled;
	bool m_bDebugMode;
	int m_iNumberOfBadSkillUseDC;
	bool m_iIsDC;

};

extern CSkillUseTime g_SkillUseTime;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

