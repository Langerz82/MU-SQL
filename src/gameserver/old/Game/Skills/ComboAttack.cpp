////////////////////////////////////////////////////////////////////////////////
// ComboAttack.cpp
#include "ComboAttack.h"
#include "MasterLevelSkillTreeSystem.h"
// GS-N 0.99.60T 0x004A42C0 : Completed
//	GS-N	1.00.18	JPN	0x004C2270	-	Completed

CComboAttack gComboAttack;

CComboAttack::CComboAttack()
{
	return;
}

CComboAttack::~CComboAttack()
{
	return;
}

int CComboAttack::GetSkillPos(int skillnum)
{
	if (g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(skillnum) == 326	//-> Cyclone
		|| g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(skillnum) == 327	//-> Slash
		|| g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(skillnum) == 328	//-> Falling Slash
		|| g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(skillnum) == 329)	//-> Lunge	
	{
		return 0;
	}
	else if (g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(skillnum) == 330
		|| g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(skillnum) == 332)	//-> Twisting
	{
		return 1;
	}
	else if (g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(skillnum) == 333
		|| g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(skillnum) == 331)	//-> Rageful Blow
	{
		return 1;
	}
	else if (g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(skillnum) == 336
		|| g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(skillnum) == 339
		|| g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(skillnum) == 342)	//-> Slash *
	{
		return 1;
	}
	else if (g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(skillnum) == 337
		|| g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(skillnum) == 340
		|| g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(skillnum) == 343)	//-> Strike Of Destruction
	{
		return 1;
	}

	else if (g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(skillnum) == 344
		|| g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(skillnum) == 346) //-> Blood Storm
	{
		return 1;
	}
	// ----
	switch (skillnum)
	{
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
	{
		return 0;
	}
		break;
		// --
	case 41:
	case 42:
	case 43:
	case 232:
	{
		return 1;
	}
		break;
		// --
	default:
	{
		return -1;
	}
		break;
	}
}

BOOL CComboAttack::CheckCombo(CGameObject &Obj, int skillnum)
{
	if ( Obj.Type != OBJ_USER )
	{
		return 0;
	}
	
	if ( Obj.m_PlayerData->ComboSkillquestClear  != false )
	{
		int sp = this->GetSkillPos(skillnum);

		if ( sp == 0 )
		{
			Obj.m_PlayerData->comboSkill->ProgressIndex  = 0;
			Obj.m_PlayerData->comboSkill->dwTime = GetTickCount() + 3000;
			Obj.m_PlayerData->comboSkill->Skill[0]  = skillnum;
		}
		else if ( sp == 1 )
		{
			if ( Obj.m_PlayerData->comboSkill->Skill[0]  == 0xFFFF )
			{
				Obj.m_PlayerData->comboSkill->Init();
				return 0;
			}

			int Time =GetTickCount();

			if (Obj.m_PlayerData->comboSkill->dwTime < GetTickCount() )
			{
				Obj.m_PlayerData->comboSkill->Init();
				return 0;
			}

			if ( Obj.m_PlayerData->comboSkill->ProgressIndex == 0 )
			{
				Obj.m_PlayerData->comboSkill->ProgressIndex = 1;
				Obj.m_PlayerData->comboSkill->dwTime = GetTickCount() + 3000;
				Obj.m_PlayerData->comboSkill->Skill[1] = skillnum;
			}
			else if ( Obj.m_PlayerData->comboSkill->Skill[1] != skillnum )
			{
				Obj.m_PlayerData->comboSkill->Init();

				if((GetTickCount() - Obj.m_PlayerData->comboSkill->dwTime) < 1000)
				{
					return 0;
				}

				return 1;
			}
			else
			{
				Obj.m_PlayerData->comboSkill->Init();
			}
		}
		else
		{
			Obj.m_PlayerData->comboSkill->ProgressIndex= -1;
			Obj.m_PlayerData->comboSkill->dwTime = 0;
			Obj.m_PlayerData->comboSkill->Skill[0] = -1;
		}
	}

	return 0;
}
			



////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

