////////////////////////////////////////////////////////////////////////////////
// SkillSpecialize.cpp
#include "stdafx.h"
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

int CSkillSpecialize::CalcStatBonus(OBJECTSTRUCT *lpObj, int iSkill)
{
	if (!lpObj)
	{
		return 0;
	}

	if (lpObj->Type != OBJ_USER)
	{
		return 0;
	}

	int iSkillBonus = 0;

	this->m_Lua.Generic_Call("SkillSpec_GetBonusValue", "iiiiii>i",
		(int)lpObj->Class, 
		iSkill, 
		int(lpObj->m_PlayerData->Strength + lpObj->AddStrength),
		int(lpObj->m_PlayerData->Dexterity + lpObj->AddDexterity),
		int(lpObj->m_PlayerData->Vitality + lpObj->AddVitality),
		int(lpObj->m_PlayerData->Energy + lpObj->AddEnergy),
		&iSkillBonus);

	return iSkillBonus;
}


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

