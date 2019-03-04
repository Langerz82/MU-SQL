// ItemOption.cpp: implementation of the CItemObjectOption class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ItemOption.h"
#include "ItemOptionTypeMng.h"
#include "MemScript.h"
#include <map>
#include <vector>
#include <string>

CItemObjectOption gItemOption;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemObjectOption::CItemObjectOption() // OK
{
	this->m_ItemOptionInfo.clear();
}

CItemObjectOption::~CItemObjectOption() // OK
{

}

void CItemObjectOption::Load(char* path) // OK
{
	CMemScript* lpMemScript = new CMemScript;

	if (lpMemScript == 0)
	{
		sLog->outError(MEM_SCRIPT_ALLOC_ERROR, path);
		return;
	}

	if (lpMemScript->SetBuffer(path) == 0)
	{
		sLog->outError(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	this->m_ItemOptionInfo.clear();

	try
	{
		while (true)
		{
			if (lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			if (strcmp("end", lpMemScript->GetString()) == 0)
			{
				break;
			}

			ITEM_OPTION_INFO info;

			info.Index = lpMemScript->GetNumber();

			info.OptionIndex = lpMemScript->GetAsNumber();

			info.OptionValue = lpMemScript->GetAsNumber();

			info.ItemMinIndex = lpMemScript->GetAsNumber();

			info.ItemMaxIndex = lpMemScript->GetAsNumber();

			info.ItemOption1 = lpMemScript->GetAsNumber();

			info.ItemOption2 = lpMemScript->GetAsNumber();

			info.ItemOption3 = lpMemScript->GetAsNumber();

			info.ItemNewOption = lpMemScript->GetAsNumber();

			std::map<int, std::vector<ITEM_OPTION_INFO>>::iterator it = this->m_ItemOptionInfo.find(info.Index);

			if (it == this->m_ItemOptionInfo.end())
			{
				this->m_ItemOptionInfo.insert(std::pair<int, std::vector<ITEM_OPTION_INFO>>(info.Index, std::vector<ITEM_OPTION_INFO>(1, info)));
			}
			else
			{
				it->second.push_back(info);
			}
		}
	}
	catch (...)
	{
		sLog->outError(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

bool CItemObjectOption::GetItemOption(int index, CItemObject* lpItem) // OK
{
	bool result = 0;

	std::map<int, std::vector<ITEM_OPTION_INFO>>::iterator ItemOptionInfo = this->m_ItemOptionInfo.find(index);

	if (ItemOptionInfo != this->m_ItemOptionInfo.end())
	{
		for (std::vector<ITEM_OPTION_INFO>::iterator it = ItemOptionInfo->second.begin(); it != ItemOptionInfo->second.end(); it++)
		{
			if (it->Index != index)
			{
				continue;
			}

			if (it->ItemMinIndex != -1 && it->ItemMinIndex > lpItem->m_Type)
			{
				continue;
			}

			if (it->ItemMaxIndex != -1 && it->ItemMaxIndex < lpItem->m_Type)
			{
				continue;
			}

			if (it->ItemOption1 != -1 && it->ItemOption1 > lpItem->m_Option1)
			{
				continue;
			}

			if (it->ItemOption2 != -1 && it->ItemOption2 > lpItem->m_Option2)
			{
				continue;
			}

			if (it->ItemOption3 != -1 && it->ItemOption3 > lpItem->m_Option3)
			{
				continue;
			}

			if (it->ItemNewOption != -1 && (lpItem->m_NewOption & it->ItemNewOption) == 0)
			{
				continue;
			}

			result = 1;

			lpItem->m_Special[index] = it->OptionIndex;

			lpItem->m_SpecialValue[index] = it->OptionValue;
		}
	}

	return result;
}

void CItemObjectOption::CalCItemObjectCommonOption(CGameObject &Obj, bool flag) // OK
{
	for (int n = 0; n < INVETORY_WEAR_SIZE; n++)
	{
		if (Obj.pntInventory[n]->IsItem() != 0 && Obj.pntInventory[n]->m_IsValidItem != 0 && Obj.pntInventory[n]->m_Durability != 0)
		{
			this->InsertOption(Obj, Obj.pntInventory[n], flag);
		}
	}
}

void CItemObjectOption::InsertOption(CGameObject &Obj, CItemObject* lpItem, bool flag) // OK
{
	for (int n = 0; n < MAX_SPECIAL; n++)
	{
		int index = lpItem->m_Special[n];

		int value = lpItem->m_SpecialValue[n];

		int Levels = Obj.Level + Obj.m_PlayerData->MasterLevel;

		if (flag == 0)
		{
			if (index == ITEM_OPTION_ADD_WING_LEADERSHIP)
			{
				continue;
			}
		}
		else
		{
			if (index != ITEM_OPTION_ADD_WING_LEADERSHIP)
			{
				continue;
			}
		}

		switch (index)
		{
		case ITEM_OPTION_ADD_PHYSI_DAMAGE:
			Obj.m_AttackDamageMinRight += value;
			Obj.m_AttackDamageMaxRight += value;
			Obj.m_AttackDamageMinLeft += value;
			Obj.m_AttackDamageMaxLeft += value;
			break;
		case ITEM_OPTION_ADD_MAGIC_DAMAGE:
			Obj.m_MagicDamageMin += value;
			Obj.m_MagicDamageMax += value;
			break;
		case ITEM_OPTION_ADD_DEFENSE_SUCCESS_RATE:
			Obj.m_PlayerData->m_MPSkillOpt->iMpsAddBlockingRate += value;
			break;
		case ITEM_OPTION_ADD_DEFENSE:
			Obj.m_Defense += value;
			break;
		case ITEM_OPTION_ADD_CRITICAL_DAMAGE_RATE:
			Obj.m_PlayerData->m_MPSkillOpt->iMpsIncCriticalDamageRate += value;
			break;
		case ITEM_OPTION_ADD_HP_RECOVERY_RATE:
			Obj.AutoHPRecovery += value;
			break;
		case ITEM_OPTION_ADD_MONEY_AMOUNT_DROP_RATE:
			Obj.MonsterDieGetMoney += value;
			break;
		case ITEM_OPTION_MUL_DEFENSE_SUCCESS_RATE:
			Obj.m_PlayerData->m_MPSkillOpt->iMpsAddBlockingRate += (Obj.m_PlayerData->m_MPSkillOpt->iMpsAddBlockingRate*value) / 100;
			break;
		case ITEM_OPTION_ADD_DAMAGE_REFLECT:
			Obj.DamageReflect += value;
			break;
		case ITEM_OPTION_ADD_DAMAGE_REDUCTION:
			Obj.DamageDecrease += value;
			break;
		case ITEM_OPTION_MUL_MP:
			Obj.AddMana += (int)((Obj.MaxMana*value) / 100);
			break;
		case ITEM_OPTION_MUL_HP:
			Obj.AddLife += (int)((Obj.MaxLife*value) / 100);
			break;
		case ITEM_OPTION_ADD_EXCELLENT_DAMAGE_RATE:
			Obj.m_PlayerData->m_MPSkillOpt->iMpsIncExcellentDamageRate += value;
			break;
		case ITEM_OPTION_ADD_PHYSI_DAMAGE_BY_LEVEL:
			Obj.m_AttackDamageMinRight += ((Obj.Level + Obj.m_PlayerData->MasterLevel) / value);
			Obj.m_AttackDamageMaxRight += ((Obj.Level + Obj.m_PlayerData->MasterLevel) / value);
			Obj.m_AttackDamageMinLeft += ((Obj.Level + Obj.m_PlayerData->MasterLevel) / value);
			Obj.m_AttackDamageMaxLeft += ((Obj.Level + Obj.m_PlayerData->MasterLevel) / value);
			break;
		case ITEM_OPTION_MUL_PHYSI_DAMAGE:
			Obj.m_AttackDamageMinRight += (Obj.m_AttackDamageMinRight*value) / 100;
			Obj.m_AttackDamageMaxRight += (Obj.m_AttackDamageMaxRight*value) / 100;
			Obj.m_AttackDamageMinLeft += (Obj.m_AttackDamageMinLeft*value) / 100;
			Obj.m_AttackDamageMaxLeft += (Obj.m_AttackDamageMaxLeft*value) / 100;
			break;
		case ITEM_OPTION_ADD_MAGIC_DAMAGE_BY_LEVEL:
			Obj.m_MagicDamageMin += ((Obj.Level + Obj.m_PlayerData->MasterLevel) / value);
			Obj.m_MagicDamageMax += ((Obj.Level + Obj.m_PlayerData->MasterLevel) / value);
			break;
		case ITEM_OPTION_MUL_MAGIC_DAMAGE:
			Obj.m_MagicDamageMin += (Obj.m_MagicDamageMin*value) / 100;
			Obj.m_MagicDamageMax += (Obj.m_MagicDamageMax*value) / 100;
			break;
		case ITEM_OPTION_ADD_SPEED:
			Obj.m_AttackSpeed += value;
			Obj.m_MagicSpeed += value;
			break;
		case ITEM_OPTION_ADD_HUNT_HP:
			Obj.m_PlayerData->m_MPSkillOpt->iMpsMonsterDieGetLife += value;
			break;
		case ITEM_OPTION_ADD_HUNT_MP:
			Obj.m_PlayerData->m_MPSkillOpt->iMpsMonsterDieGetMana += value;
			break;
		case ITEM_OPTION_ADD_WING_HP:
			Obj.AddLife += 50 + (value*lpItem->m_Level);
			break;
		case ITEM_OPTION_ADD_WING_MP:
			Obj.AddMana += 50 + (value*lpItem->m_Level);
			break;
		case ITEM_OPTION_ADD_IGNORE_DEFENSE_RATE:
			Obj.m_PlayerData->m_MPSkillOpt->iMpsIncIgnoreEnemyBlock += value;
			break;
		case ITEM_OPTION_ADD_BP:
			Obj.AddBP += value;
			break;
		case ITEM_OPTION_MUL_BP:
			Obj.AddBP += (int)((Obj.MaxBP*value) / 100);
			break;
		case ITEM_OPTION_ADD_WING_LEADERSHIP:
			Obj.AddLeadership += 10 + (value*lpItem->m_Level);
			break;
		case ITEM_OPTION_ADD_FENRIR1:
			break;
		case ITEM_OPTION_ADD_FENRIR2:
			break;
		case ITEM_OPTION_ADD_FENRIR3:
			break;
		case ITEM_OPTION_ADD_FULL_DAMAGE_REFLECT_RATE:
			Obj.m_PlayerData->m_MPSkillOpt->iMpsReturnEnemyAttack += value;
			break;
		case ITEM_OPTION_ADD_DEFENSIVE_FULL_HP_RESTORE_RATE:
			Obj.m_PlayerData->m_MPSkillOpt->iMpsRecoverHPRate += value;
			break;
		case ITEM_OPTION_ADD_DEFENSIVE_FULL_MP_RESTORE_RATE:
			Obj.m_PlayerData->m_MPSkillOpt->iMpsRecoverManaRate += value;
			break;
		case ITEM_OPTION_ADD_DINORANT1:
			break;
		case ITEM_OPTION_ADD_CURSE_DAMAGE:
			Obj.m_CurseDamageMin += value;
			Obj.m_CurseDamageMax += value;
			break;
		case ITEM_OPTION_ADD_CURSE_DAMAGE_BY_LEVEL:
			Obj.m_CurseDamageMin += ((Obj.Level + Obj.m_PlayerData->MasterLevel) / value);
			Obj.m_CurseDamageMax += ((Obj.Level + Obj.m_PlayerData->MasterLevel) / value);
			break;
		case ITEM_OPTION_MUL_CURSE_DAMAGE:
			Obj.m_CurseDamageMin += (Obj.m_CurseDamageMin*value) / 100;
			Obj.m_CurseDamageMax += (Obj.m_CurseDamageMax*value) / 100;
			break;
		case ITEM_OPTION_ADD_DOUBLE_DAMAGE_RATE:
			Obj.m_PlayerData->SetOpDoubleDamage += value;
			break;
		case ITEM_OPTION_ADD_EXPERIENCE_RATE:
			Obj.m_PlayerData->Experience += value;
			Obj.m_PlayerData->MasterExperience += value;
			break;
		case ITEM_OPTION_ADD_COMBO_EXPERIENCE_RATE:
			if (Obj.pntInventory[8]->IsItem() != 0 && (Obj.pntInventory[8]->m_Type == ITEMGET(13, 80) || Obj.pntInventory[8]->m_Type== ITEMGET(13, 123)))
			{
				Obj.m_PlayerData->Experience += value;
				Obj.m_PlayerData->MasterExperience += value;
			}
			break;
		case ITEM_OPTION_ADD_HP:
			Obj.AddLife += value;
			break;
		case ITEM_OPTION_ADD_MP:
			Obj.AddMana += value;
			break;
		case ITEM_OPTION_ADD_HP_BY_LEVEL:
			Obj.AddLife += ((Obj.Level + Obj.m_PlayerData->MasterLevel) / value);
			break;
		case ITEM_OPTION_ADD_MP_BY_LEVEL:
			Obj.AddMana += ((Obj.Level + Obj.m_PlayerData->MasterLevel) / value);
			break;
		case ITEM_OPTION_ADD_DAMAGE:
			Obj.m_AttackDamageMinRight += value;
			Obj.m_AttackDamageMaxRight += value;
			Obj.m_AttackDamageMinLeft += value;
			Obj.m_AttackDamageMaxLeft += value;
			Obj.m_MagicDamageMin += value;
			Obj.m_MagicDamageMax += value;
			Obj.m_CurseDamageMin += value;
			Obj.m_CurseDamageMax += value;
			break;
		case ITEM_OPTION_ADD_DAMAGE_BY_LEVEL:
			Obj.m_AttackDamageMinRight += ((Obj.Level + Obj.m_PlayerData->MasterLevel) / value);
			Obj.m_AttackDamageMaxRight += ((Obj.Level + Obj.m_PlayerData->MasterLevel) / value);
			Obj.m_AttackDamageMinLeft += ((Obj.Level + Obj.m_PlayerData->MasterLevel) / value);
			Obj.m_AttackDamageMaxLeft += ((Obj.Level + Obj.m_PlayerData->MasterLevel) / value);
			Obj.m_MagicDamageMin += ((Obj.Level + Obj.m_PlayerData->MasterLevel) / value);
			Obj.m_MagicDamageMax += ((Obj.Level + Obj.m_PlayerData->MasterLevel) / value);
			Obj.m_CurseDamageMin += ((Obj.Level + Obj.m_PlayerData->MasterLevel) / value);
			Obj.m_CurseDamageMax += ((Obj.Level + Obj.m_PlayerData->MasterLevel) / value);
			break;
		case ITEM_OPTION_MUL_DAMAGE:
			Obj.m_AttackDamageMinRight += (Obj.m_AttackDamageMinRight*value) / 100;
			Obj.m_AttackDamageMaxRight += (Obj.m_AttackDamageMaxRight*value) / 100;
			Obj.m_AttackDamageMinLeft += (Obj.m_AttackDamageMinLeft*value) / 100;
			Obj.m_AttackDamageMaxLeft += (Obj.m_AttackDamageMaxLeft*value) / 100;
			Obj.m_MagicDamageMin += (Obj.m_MagicDamageMin*value) / 100;
			Obj.m_MagicDamageMax += (Obj.m_MagicDamageMax*value) / 100;
			Obj.m_CurseDamageMin += (Obj.m_CurseDamageMin*value) / 100;
			Obj.m_CurseDamageMax += (Obj.m_CurseDamageMax*value) / 100;
			break;
		case ITEM_OPTION_ADD_DEFENSE_BY_LEVEL:
			Obj.m_Defense += ((Obj.Level + Obj.m_PlayerData->MasterLevel) / value);
			break;
		case ITEM_OPTION_MUL_DEFENSE:
			Obj.m_Defense += (Obj.m_Defense*value) / 100;
			break;
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

