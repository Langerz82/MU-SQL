////////////////////////////////////////////////////////////////////////////////
// SkillSpecialize.h
#ifndef SKILLSPECIALIZE_H
#define SKILLSPECIALIZE_H

#include "User/CUserData.h"
#include "MuLua.h"

#pragma once
class CSkillSpecialize
{
public:
	CSkillSpecialize(void);
	virtual ~CSkillSpecialize(void);

	void LoadScript();
	int CalcStatBonus(CGameObject &Obj, int iSkill);

private:
	MULua m_Lua;
};

extern CSkillSpecialize g_SkillSpec;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

