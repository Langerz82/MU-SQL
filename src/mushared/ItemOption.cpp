// ItemOption.cpp: implementation of the CItemOption class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ItemOption.h"
#include "ItemOptionTypeMng.h"
#include "MemScript.h"
#include "winutil.h"
#include "TLog.h"

CItemOption gItemOption;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemOption::CItemOption() // OK
{
	this->m_ItemOptionInfo.clear();
}

CItemOption::~CItemOption() // OK
{

}

void CItemOption::Load(char* path) // OK
{
	CMemScript* lpMemScript = new CMemScript;

	if (lpMemScript == 0)
	{
		g_Log.MsgBox(MEM_SCRIPT_ALLOC_ERROR, path);
		return;
	}

	if (lpMemScript->SetBuffer(path) == 0)
	{
		g_Log.MsgBox(lpMemScript->GetLastError());
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
		g_Log.MsgBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

bool CItemOption::GetItemOption(int index, CItem* lpItem) // OK
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

void CItemOption::CalcItemCommonOption(LPOBJ lpObj, bool flag) // OK
{
	for (int n = 0; n < INVETORY_WEAR_SIZE; n++)
	{
		if (lpObj->pInventory[n].IsItem() != 0 && lpObj->pInventory[n].m_IsValidItem != 0 && lpObj->pInventory[n].m_Durability != 0)
		{
			this->InsertOption(lpObj, &lpObj->pInventory[n], flag);
		}
	}
}

void CItemOption::InsertOption(LPOBJ lpObj, CItem* lpItem, bool flag) // OK
{
	for (int n = 0; n < MAX_SPECIAL; n++)
	{
		int index = lpItem->m_Special[n];

		int value = lpItem->m_SpecialValue[n];

		int Levels = lpObj->Level + lpObj->m_PlayerData->MasterLevel;

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
			lpObj->m_AttackDamageMinRight += value;
			lpObj->m_AttackDamageMaxRight += value;
			lpObj->m_AttackDamageMinLeft += value;
			lpObj->m_AttackDamageMaxLeft += value;
			break;
		case ITEM_OPTION_ADD_MAGIC_DAMAGE:
			lpObj->m_MagicDamageMin += value;
			lpObj->m_MagicDamageMax += value;
			break;
		case ITEM_OPTION_ADD_DEFENSE_SUCCESS_RATE:
			lpObj->m_PlayerData->m_MPSkillOpt.iMpsAddBlockingRate += value;
			break;
		case ITEM_OPTION_ADD_DEFENSE:
			lpObj->m_Defense += value;
			break;
		case ITEM_OPTION_ADD_CRITICAL_DAMAGE_RATE:
			lpObj->m_PlayerData->m_MPSkillOpt.iMpsIncCriticalDamageRate += value;
			break;
		case ITEM_OPTION_ADD_HP_RECOVERY_RATE:
			lpObj->AutoHPRecovery += value;
			break;
		case ITEM_OPTION_ADD_MONEY_AMOUNT_DROP_RATE:
			lpObj->MonsterDieGetMoney += value;
			break;
		case ITEM_OPTION_MUL_DEFENSE_SUCCESS_RATE:
			lpObj->m_PlayerData->m_MPSkillOpt.iMpsAddBlockingRate += (lpObj->m_PlayerData->m_MPSkillOpt.iMpsAddBlockingRate*value) / 100;
			break;
		case ITEM_OPTION_ADD_DAMAGE_REFLECT:
			lpObj->DamageReflect += value;
			break;
		case ITEM_OPTION_ADD_DAMAGE_REDUCTION:
			lpObj->DamageDecrease += value;
			break;
		case ITEM_OPTION_MUL_MP:
			lpObj->AddMana += (int)((lpObj->MaxMana*value) / 100);
			break;
		case ITEM_OPTION_MUL_HP:
			lpObj->AddLife += (int)((lpObj->MaxLife*value) / 100);
			break;
		case ITEM_OPTION_ADD_EXCELLENT_DAMAGE_RATE:
			lpObj->m_PlayerData->m_MPSkillOpt.iMpsIncExcellentDamageRate += value;
			break;
		case ITEM_OPTION_ADD_PHYSI_DAMAGE_BY_LEVEL:
			lpObj->m_AttackDamageMinRight += ((lpObj->Level + lpObj->m_PlayerData->MasterLevel) / value);
			lpObj->m_AttackDamageMaxRight += ((lpObj->Level + lpObj->m_PlayerData->MasterLevel) / value);
			lpObj->m_AttackDamageMinLeft += ((lpObj->Level + lpObj->m_PlayerData->MasterLevel) / value);
			lpObj->m_AttackDamageMaxLeft += ((lpObj->Level + lpObj->m_PlayerData->MasterLevel) / value);
			break;
		case ITEM_OPTION_MUL_PHYSI_DAMAGE:
			lpObj->m_AttackDamageMinRight += (lpObj->m_AttackDamageMinRight*value) / 100;
			lpObj->m_AttackDamageMaxRight += (lpObj->m_AttackDamageMaxRight*value) / 100;
			lpObj->m_AttackDamageMinLeft += (lpObj->m_AttackDamageMinLeft*value) / 100;
			lpObj->m_AttackDamageMaxLeft += (lpObj->m_AttackDamageMaxLeft*value) / 100;
			break;
		case ITEM_OPTION_ADD_MAGIC_DAMAGE_BY_LEVEL:
			lpObj->m_MagicDamageMin += ((lpObj->Level + lpObj->m_PlayerData->MasterLevel) / value);
			lpObj->m_MagicDamageMax += ((lpObj->Level + lpObj->m_PlayerData->MasterLevel) / value);
			break;
		case ITEM_OPTION_MUL_MAGIC_DAMAGE:
			lpObj->m_MagicDamageMin += (lpObj->m_MagicDamageMin*value) / 100;
			lpObj->m_MagicDamageMax += (lpObj->m_MagicDamageMax*value) / 100;
			break;
		case ITEM_OPTION_ADD_SPEED:
			lpObj->m_AttackSpeed += value;
			lpObj->m_MagicSpeed += value;
			break;
		case ITEM_OPTION_ADD_HUNT_HP:
			lpObj->m_PlayerData->m_MPSkillOpt.iMpsMonsterDieGetLife += value;
			break;
		case ITEM_OPTION_ADD_HUNT_MP:
			lpObj->m_PlayerData->m_MPSkillOpt.iMpsMonsterDieGetMana += value;
			break;
		case ITEM_OPTION_ADD_WING_HP:
			lpObj->AddLife += 50 + (value*lpItem->m_Level);
			break;
		case ITEM_OPTION_ADD_WING_MP:
			lpObj->AddMana += 50 + (value*lpItem->m_Level);
			break;
		case ITEM_OPTION_ADD_IGNORE_DEFENSE_RATE:
			lpObj->m_PlayerData->m_MPSkillOpt.iMpsIncIgnoreEnemyBlock += value;
			break;
		case ITEM_OPTION_ADD_BP:
			lpObj->AddBP += value;
			break;
		case ITEM_OPTION_MUL_BP:
			lpObj->AddBP += (int)((lpObj->MaxBP*value) / 100);
			break;
		case ITEM_OPTION_ADD_WING_LEADERSHIP:
			lpObj->AddLeadership += 10 + (value*lpItem->m_Level);
			break;
		case ITEM_OPTION_ADD_FENRIR1:
			break;
		case ITEM_OPTION_ADD_FENRIR2:
			break;
		case ITEM_OPTION_ADD_FENRIR3:
			break;
		case ITEM_OPTION_ADD_FULL_DAMAGE_REFLECT_RATE:
			lpObj->m_PlayerData->m_MPSkillOpt.iMpsReturnEnemyAttack += value;
			break;
		case ITEM_OPTION_ADD_DEFENSIVE_FULL_HP_RESTORE_RATE:
			lpObj->m_PlayerData->m_MPSkillOpt.iMpsRecoverHPRate += value;
			break;
		case ITEM_OPTION_ADD_DEFENSIVE_FULL_MP_RESTORE_RATE:
			lpObj->m_PlayerData->m_MPSkillOpt.iMpsRecoverManaRate += value;
			break;
		case ITEM_OPTION_ADD_DINORANT1:
			break;
		case ITEM_OPTION_ADD_CURSE_DAMAGE:
			lpObj->m_CurseDamageMin += value;
			lpObj->m_CurseDamageMax += value;
			break;
		case ITEM_OPTION_ADD_CURSE_DAMAGE_BY_LEVEL:
			lpObj->m_CurseDamageMin += ((lpObj->Level + lpObj->m_PlayerData->MasterLevel) / value);
			lpObj->m_CurseDamageMax += ((lpObj->Level + lpObj->m_PlayerData->MasterLevel) / value);
			break;
		case ITEM_OPTION_MUL_CURSE_DAMAGE:
			lpObj->m_CurseDamageMin += (lpObj->m_CurseDamageMin*value) / 100;
			lpObj->m_CurseDamageMax += (lpObj->m_CurseDamageMax*value) / 100;
			break;
		case ITEM_OPTION_ADD_DOUBLE_DAMAGE_RATE:
			lpObj->m_PlayerData->SetOpDoubleDamage += value;
			break;
		case ITEM_OPTION_ADD_EXPERIENCE_RATE:
			lpObj->m_PlayerData->Experience += value;
			lpObj->m_PlayerData->MasterExperience += value;
			break;
		case ITEM_OPTION_ADD_COMBO_EXPERIENCE_RATE:
			if (lpObj->pInventory[8].IsItem() != 0 && (lpObj->pInventory[8].m_Type == ITEMGET(13, 80) || lpObj->pInventory[8].m_Type== ITEMGET(13, 123)))
			{
				lpObj->m_PlayerData->Experience += value;
				lpObj->m_PlayerData->MasterExperience += value;
			}
			break;
		case ITEM_OPTION_ADD_HP:
			lpObj->AddLife += value;
			break;
		case ITEM_OPTION_ADD_MP:
			lpObj->AddMana += value;
			break;
		case ITEM_OPTION_ADD_HP_BY_LEVEL:
			lpObj->AddLife += ((lpObj->Level + lpObj->m_PlayerData->MasterLevel) / value);
			break;
		case ITEM_OPTION_ADD_MP_BY_LEVEL:
			lpObj->AddMana += ((lpObj->Level + lpObj->m_PlayerData->MasterLevel) / value);
			break;
		case ITEM_OPTION_ADD_DAMAGE:
			lpObj->m_AttackDamageMinRight += value;
			lpObj->m_AttackDamageMaxRight += value;
			lpObj->m_AttackDamageMinLeft += value;
			lpObj->m_AttackDamageMaxLeft += value;
			lpObj->m_MagicDamageMin += value;
			lpObj->m_MagicDamageMax += value;
			lpObj->m_CurseDamageMin += value;
			lpObj->m_CurseDamageMax += value;
			break;
		case ITEM_OPTION_ADD_DAMAGE_BY_LEVEL:
			lpObj->m_AttackDamageMinRight += ((lpObj->Level + lpObj->m_PlayerData->MasterLevel) / value);
			lpObj->m_AttackDamageMaxRight += ((lpObj->Level + lpObj->m_PlayerData->MasterLevel) / value);
			lpObj->m_AttackDamageMinLeft += ((lpObj->Level + lpObj->m_PlayerData->MasterLevel) / value);
			lpObj->m_AttackDamageMaxLeft += ((lpObj->Level + lpObj->m_PlayerData->MasterLevel) / value);
			lpObj->m_MagicDamageMin += ((lpObj->Level + lpObj->m_PlayerData->MasterLevel) / value);
			lpObj->m_MagicDamageMax += ((lpObj->Level + lpObj->m_PlayerData->MasterLevel) / value);
			lpObj->m_CurseDamageMin += ((lpObj->Level + lpObj->m_PlayerData->MasterLevel) / value);
			lpObj->m_CurseDamageMax += ((lpObj->Level + lpObj->m_PlayerData->MasterLevel) / value);
			break;
		case ITEM_OPTION_MUL_DAMAGE:
			lpObj->m_AttackDamageMinRight += (lpObj->m_AttackDamageMinRight*value) / 100;
			lpObj->m_AttackDamageMaxRight += (lpObj->m_AttackDamageMaxRight*value) / 100;
			lpObj->m_AttackDamageMinLeft += (lpObj->m_AttackDamageMinLeft*value) / 100;
			lpObj->m_AttackDamageMaxLeft += (lpObj->m_AttackDamageMaxLeft*value) / 100;
			lpObj->m_MagicDamageMin += (lpObj->m_MagicDamageMin*value) / 100;
			lpObj->m_MagicDamageMax += (lpObj->m_MagicDamageMax*value) / 100;
			lpObj->m_CurseDamageMin += (lpObj->m_CurseDamageMin*value) / 100;
			lpObj->m_CurseDamageMax += (lpObj->m_CurseDamageMax*value) / 100;
			break;
		case ITEM_OPTION_ADD_DEFENSE_BY_LEVEL:
			lpObj->m_Defense += ((lpObj->Level + lpObj->m_PlayerData->MasterLevel) / value);
			break;
		case ITEM_OPTION_MUL_DEFENSE:
			lpObj->m_Defense += (lpObj->m_Defense*value) / 100;
			break;
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

