////////////////////////////////////////////////////////////////////////////////
// SkillSpecialize.cpp
#include "StdAfx.h"
#include "SkillSpecialize.h"
#include "configread.h"

CSkillSpecialize g_SkillSpec;

CSkillSpecialize::CSkillSpecialize(void) : m_Lua(true)
{
}

CSkillSpecialize::~CSkillSpecialize(void)
{
}

void CSkillSpecialize::LoadScript()
{
	this->m_Lua.DoFile(g_ConfigRead.GetPath("\\Scripts\\Specialization\\SkillSpec.lua"));
}

int CSkillSpecialize::CalcStatBonus(CGameObject lpObj, int iSkill)
{
	if (!&lpObj)
	{
		return 0;
	}

	if (Obj.Type != OBJ_USER)
	{
		return 0;
	}

	int iSkillBonus = 0;

	this->m_Lua.Generic_Call("SkillSpec_GetBonusValue", "iiiiii>i",
		(int)Obj.Class, 
		iSkill, 
		int(Obj.m_PlayerData->Strength + Obj.AddStrength),
		int(Obj.m_PlayerData->Dexterity + Obj.AddDexterity),
		int(Obj.m_PlayerData->Vitality + Obj.AddVitality),
		int(Obj.m_PlayerData->Energy + Obj.AddEnergy),
		&iSkillBonus);

	return iSkillBonus;
}


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

