////////////////////////////////////////////////////////////////////////////////
// ObjCalCharacter.cpp
#include "StdAfx.h"
#include "ObjCalCharacter.h"
#include "Gamemain.h"
#include "DarkSpirit.h"
#include "SetItemOption.h"
#include "Logging/Log.h"
#include "ItemSocketOptionSystem.h"
#include "BuffEffect.h"
#include "BuffEffectSlot.h"
#include "PentagramSystem.h"
#include "configread.h"
#include "MasterLevelSkillTreeSystem.h"
#include "MuunSystem.h"
#include "ItemOptionTypeMng.h"
#include "StatSpecialize.h"
#include "util.h"
#include "ItemOption.h"
#include "PeriodItemEx.h"
#include "BuffEffectSlot.h"

// GS-N 0.99.60T 0x004A4740
//	GS-N	1.00.18	JPN	0x004C2650	-	Completed
// ADDONS by Dudi (FROM 1.00.90 CS + S5 ITEMS)

CObjCalCharacter gObjCalCharacter;

CObjCalCharacter::CObjCalCharacter() : m_Lua(true)
{

}

CObjCalCharacter::~CObjCalCharacter()
{

}

void CObjCalCharacter::Init()
{
	this->m_Lua.DoFile(g_ConfigRead.GetPath("\\Scripts\\Character\\CalcCharacter.lua"));
}

void CObjCalCharacter::CalcCharacter(CGameObject &Obj)
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	
	int Strength = 0;
	int Dexterity = 0;
	int Vitality = 0;
	int Energy = 0;
	int Leadership = 0;
	CItemObject* Right = Obj.pntInventory[0];
	CItemObject* Left = Obj.pntInventory[1];
	CItemObject* Helmet = Obj.pntInventory[2];
	CItemObject* Armor = Obj.pntInventory[3];
	CItemObject* Pants = Obj.pntInventory[4];
	CItemObject* Gloves = Obj.pntInventory[5];
	CItemObject* Boots = Obj.pntInventory[6];
	CItemObject* Amulet = Obj.pntInventory[9];
	CItemObject* Helper = Obj.pntInventory[8];
	CItemObject* Wing = Obj.pntInventory[7];
	CItemObject* RightRing = Obj.pntInventory[10];
	CItemObject* LeftRing = Obj.pntInventory[11];
	CItemObject* Pentagram = Obj.pntInventory[236];

	Obj.HaveWeaponInHand = 1;

	if (Right->IsItem() == 0 && Left->IsItem() == 0)
	{
		Obj.HaveWeaponInHand = 0;
	}
	else if (Right->IsItem() == 0)
	{
		if (Left->m_Type == ITEMGET(4, 7) && Left->m_Type == ITEMGET(4, 15) && Left->m_Type == ITEMGET(4, 32)
			&& Left->m_Type == ITEMGET(4, 33) && Left->m_Type == ITEMGET(4, 34) && Left->m_Type == ITEMGET(4, 35)) // Bolt
		{
			Obj.HaveWeaponInHand = 0;
		}
		else if (Left->m_Type >= ITEMGET(6, 0) && Left->m_Type < ITEMGET(7, 0)) // Shields
		{
			Obj.HaveWeaponInHand = 0;
		}
	}

	Obj.AddLife = 0;
	Obj.AddMana = 0;
	Obj.AddBP = 0;
	Obj.iAddShield = 0;
	Obj.MonsterDieGetMoney = 0;
	Obj.MonsterDieGetLife = 0;
	Obj.MonsterDieGetMana = 0;
	Obj.AutoHPRecovery = 0;
	Obj.DamageReflect = 0;
	Obj.DamageDecrease = 0;
#if(CUSTOM_DAMAGES12 == 1)
	Obj.S12DamageDecrease = 0;
#endif
	Obj.SkillLongSpearChange = false;

	int iItemIndex;
	BOOL bIsChangeItem;	// lc34

	for (iItemIndex = 0; iItemIndex < MAX_PLAYER_EQUIPMENT; iItemIndex++)
	{
		if (Obj.pntInventory[iItemIndex]->IsItem() != FALSE)
		{
			Obj.pntInventory[iItemIndex]->m_IsValidItem = true;
		}
	}

	if (Obj.pntInventory[236]->IsItem() != FALSE)
	{
		Obj.pntInventory[236]->m_IsValidItem = true;
	}

	do
	{

#ifdef ENABLE_CUSTOM_ITEMOPTION
		gItemOption.CalCItemObjectCommonOption(Obj. 1);
#endif

		Obj.m_PlayerData->SetOpAddMaxAttackDamage = 0;
		Obj.m_PlayerData->SetOpAddMinAttackDamage = 0;
		Obj.m_PlayerData->SetOpAddMinMagicDamage = 0;
		Obj.m_PlayerData->SetOpAddMaxMagicDamage = 0;
		Obj.m_PlayerData->SetOpAddDamage = 0;
		Obj.m_PlayerData->SetOpIncAGValue = 0;
		Obj.m_PlayerData->SetOpAddCriticalDamageSuccessRate = 0;
		Obj.m_PlayerData->SetOpAddCriticalDamage = 0;
		Obj.m_PlayerData->SetOpAddExDamageSuccessRate = 0;
		Obj.m_PlayerData->SetOpAddExDamage = 0;
		Obj.m_PlayerData->SetOpAddSkillAttack = 0;
		Obj.AddStrength = 0;
		Obj.AddDexterity = 0;
		Obj.AddVitality = 0;
		Obj.AddEnergy = 0;
		Obj.AddLeadership = 0;
		Obj.m_PlayerData->SetOpAddAttackDamage = 0;
		Obj.m_PlayerData->SetOpAddDefence = 0;
		Obj.m_PlayerData->SetOpAddMagicPower = 0;
		Obj.m_PlayerData->SetOpAddDefenceRate = 0;
		Obj.m_PlayerData->SetOpIgnoreDefense = 0;
		Obj.m_PlayerData->SetOpDoubleDamage = 0;
		Obj.m_PlayerData->SetOpTwoHandSwordImproveDamage = 0;
		Obj.m_PlayerData->SetOpImproveSuccessAttackRate = 0;
		Obj.m_PlayerData->SetOpReflectionDamage = 0;
		Obj.m_PlayerData->SetOpImproveSheldDefence = 0;
		Obj.m_PlayerData->SetOpDecreaseAG = 0;
		Obj.m_PlayerData->SetOpImproveItemDropRate = 0;
		Obj.m_PlayerData->IsFullSetItem = false;
		Obj.m_PlayerData->m_WingExcOption->Clear();
		Obj.m_BaseDefense = 0;
		memset(Obj.m_AddResistance, 0, sizeof(Obj.m_AddResistance));
		memset(&Obj.m_PlayerData->m_PentagramOptions, 0, sizeof(Obj.m_PlayerData->m_PentagramOptions));
		bIsChangeItem = 0;
		g_StatSpec.ClearUserOption(Obj);
		g_BuffEffect.SetPrevEffect(Obj);
		this->CalcSetItemStat(Obj);
		this->CalcSetItemOption(Obj);
		g_MasterLevelSkillTreeSystem.SetItemMLPassiveSkill(Obj. Right->GetDetailItemType());
		g_MasterLevelSkillTreeSystem.SetItemMLPassiveSkill(Obj. Left->GetDetailItemType());
		g_MasterLevelSkillTreeSystem.SetWingMLPassiveSkill(Obj. Wing->m_Type);
		g_MasterLevelSkillTreeSystem.SetPetItemMLPassiveSkill(Obj. Helper->m_Type);

		if (gObjCheckUsedBuffEffect(Obj. BUFFTYPE_MONK_INCREASE_HEALTH) == TRUE)
		{
			int iOption;
			gObjGetValueOfBuffIndex(Obj. BUFFTYPE_MONK_INCREASE_HEALTH, &iOption, 0);
			Obj.AddVitality += iOption;
		}

		if (gObjCheckUsedBuffEffect(Obj. BUFFTYPE_MONK_INCREASE_HEALTH_STR) == TRUE)
		{
			int iOption;
			gObjGetValueOfBuffIndex(Obj. BUFFTYPE_MONK_INCREASE_HEALTH_STR, &iOption, 0);
			Obj.AddVitality += iOption;
		}

		if (Obj.m_PlayerData->m_MPSkillOpt->iMpsIncEnergyStat > 0.0)
		{
			Obj.AddEnergy += Obj.m_PlayerData->m_MPSkillOpt->iMpsIncEnergyStat;
		}

		if (Obj.m_PlayerData->m_MPSkillOpt->iMpsIncVitalStat > 0.0)
		{
			Obj.AddVitality += Obj.m_PlayerData->m_MPSkillOpt->iMpsIncVitalStat;
		}

		if (Obj.m_PlayerData->m_MPSkillOpt->iMpsIncDexStat > 0.0)
		{
			EnterCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);
			Obj.m_PlayerData->AgilityCheckDelay = GetTickCount();
			Obj.AddDexterity += Obj.m_PlayerData->m_MPSkillOpt->iMpsIncDexStat;
			LeaveCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);
		}

		if (Obj.m_PlayerData->m_MPSkillOpt->iMpsIncPowerStat > 0.0)
		{
			Obj.AddStrength += Obj.m_PlayerData->m_MPSkillOpt->iMpsIncPowerStat;
		}

		if (Obj.m_PlayerData->m_MPSkillOpt->iMpsIncLeadershipStat > 0.0)
		{
			Obj.AddLeadership += Obj.m_PlayerData->m_MPSkillOpt->iMpsIncLeadershipStat;
		}

		if (Obj.m_PlayerData->m_MPSkillOpt->iMpsIncSteelArmor > 0.0)
		{
			Obj.m_BaseDefense += Obj.m_PlayerData->m_MPSkillOpt->iMpsIncSteelArmor;
		}

		if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_BLESS) == TRUE)
		{
			int iOption;
			gObjGetValueOfBuffIndex(Obj, BUFFTYPE_BLESS, &iOption, 0);

			EnterCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);
			Obj.m_PlayerData->AgilityCheckDelay = GetTickCount();
			Obj.AddVitality += iOption;
			Obj.AddStrength += iOption;
			Obj.AddDexterity += iOption;
			Obj.AddEnergy += iOption;

			if (Obj.Class == CLASS_DARKLORD)
				Obj.AddLeadership += iOption;

			LeaveCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);
		}

		for (iItemIndex = 0; iItemIndex < MAX_PLAYER_EQUIPMENT; iItemIndex++)
		{
			if (Obj.pntInventory[iItemIndex]->IsItem() != FALSE && Obj.pntInventory[iItemIndex]->m_IsValidItem != false)
			{
				if (this->ValidItem(Obj. &Obj.pntInventory[iItemIndex], iItemIndex) != FALSE)
				{
					Obj.pntInventory[iItemIndex]->m_IsValidItem = true;
				}
				else
				{
					Obj.pntInventory[iItemIndex]->m_IsValidItem = false;
					g_BuffEffect.ClearPrevEffect(Obj.;
					bIsChangeItem = TRUE;
				}
			}
		}
	} while (bIsChangeItem != FALSE);

	if (Obj.Class == CLASS_GROWLANCER)
	{
		Obj.m_PlayerData->RageDMG = 100 + Dexterity / 10;
	}
	else
	{
		Obj.m_PlayerData->RageDMG = 0;
	}

	if (Obj.pntInventory[236]->IsItem() == TRUE)
	{
		if (this->ValidItem(Obj. &Obj.pntInventory[236], 236))
		{
			Obj.pntInventory[236]->m_IsValidItem = true;
			g_PentagramSystem.CalcPentagramItem(Obj.m_Index, &Obj.pntInventory[236]);
		}

		else
		{
			Obj.pntInventory[236]->m_IsValidItem = false;
		}
	}

	else
	{
		g_PentagramSystem.ClearPentagramItem(Obj.m_Index);
	}

	Strength = Obj.m_PlayerData->Strength + Obj.AddStrength;
	Dexterity = Obj.m_PlayerData->Dexterity + Obj.AddDexterity;
	Vitality = Obj.m_PlayerData->Vitality + Obj.AddVitality;
	Energy = Obj.m_PlayerData->Energy + Obj.AddEnergy;
	Leadership = Obj.Leadership + Obj.AddLeadership;

	if (g_ConfigRead.UseDamageCalc == TRUE)
	{
		if (Obj.Class == CLASS_ELF) // Elf
		{
			if ((Right->m_Type >= ITEMGET(4, 8) && Right->m_Type < ITEMGET(4, 15)) ||
				(Right->m_Type >= ITEMGET(4, 0) && Right->m_Type < ITEMGET(4, 7)) ||
				Right->m_Type == ITEMGET(4, 16) || Right->m_Type == ITEMGET(4, 20) ||
				Right->m_Type == ITEMGET(4, 21) || Right->m_Type == ITEMGET(4, 22) ||
				Right->m_Type == ITEMGET(4, 23) || Right->m_Type == ITEMGET(4, 24) ||
				Right->m_Type == ITEMGET(4, 25) || Right->m_Type == ITEMGET(4, 26) ||
				Right->m_Type == ITEMGET(4, 27) || Right->m_Type == ITEMGET(4, 18) ||
				Right->m_Type == ITEMGET(4, 19) || Right->m_Type == ITEMGET(4, 17) ||
				Right->m_Type == ITEMGET(4, 28) || Right->m_Type == ITEMGET(4, 29) ||
				Right->m_Type == ITEMGET(4, 30) || Right->m_Type == ITEMGET(4, 31) ||
				Left->m_Type == ITEMGET(4, 32) || Left->m_Type == ITEMGET(4, 33) ||
				Left->m_Type == ITEMGET(4, 34) || Left->m_Type == ITEMGET(4, 35))
			{
				if ((Right->IsItem() != FALSE && Right->m_IsValidItem == false) || (Left->IsItem() != FALSE && Left->m_IsValidItem == false))
				{
					this->m_Lua.Generic_Call("ElfWithoutBowDamageCalc", "iiii>iiii", Strength, Dexterity, Vitality, Energy,
						&Obj.m_AttackDamageMinLeft, &Obj.m_AttackDamageMinRight, &Obj.m_AttackDamageMaxLeft, &Obj.m_AttackDamageMaxRight);
				}
				else
				{
					this->m_Lua.Generic_Call("ElfWithBowDamageCalc", "iiii>iiii", Strength, Dexterity, Vitality, Energy,
						&Obj.m_AttackDamageMinLeft, &Obj.m_AttackDamageMinRight, &Obj.m_AttackDamageMaxLeft, &Obj.m_AttackDamageMaxRight);
				}
			}
			else
			{
				this->m_Lua.Generic_Call("ElfWithoutBowDamageCalc", "iiii>iiii", Strength, Dexterity, Vitality, Energy,
					&Obj.m_AttackDamageMinLeft, &Obj.m_AttackDamageMinRight, &Obj.m_AttackDamageMaxLeft, &Obj.m_AttackDamageMaxRight);
			}
		}
		else if (Obj.Class == CLASS_KNIGHT) // Dark Knight
		{
			this->m_Lua.Generic_Call("KnightDamageCalc", "iiii>iiii", Strength, Dexterity, Vitality, Energy,
				&Obj.m_AttackDamageMinLeft, &Obj.m_AttackDamageMinRight, &Obj.m_AttackDamageMaxLeft, &Obj.m_AttackDamageMaxRight);
		}

		else if (Obj.Class == CLASS_MAGUMSA) // MAgic Gladiator
		{
			this->m_Lua.Generic_Call("GladiatorDamageCalc", "iiii>iiii", Strength, Dexterity, Vitality, Energy,
				&Obj.m_AttackDamageMinLeft, &Obj.m_AttackDamageMinRight, &Obj.m_AttackDamageMaxLeft, &Obj.m_AttackDamageMaxRight);
		}

		else if (Obj.Class == CLASS_DARKLORD) // Dark Lord
		{
			this->m_Lua.Generic_Call("LordDamageCalc", "iiiii>iiii", Strength, Dexterity, Vitality, Energy, Leadership,
				&Obj.m_AttackDamageMinLeft, &Obj.m_AttackDamageMinRight, &Obj.m_AttackDamageMaxLeft, &Obj.m_AttackDamageMaxRight);
		}

		else if (Obj.Class == CLASS_SUMMONER) // Summoner
		{
			this->m_Lua.Generic_Call("SummonerDamageCalc", "iiii>iiii", Strength, Dexterity, Vitality, Energy,
				&Obj.m_AttackDamageMinLeft, &Obj.m_AttackDamageMinRight, &Obj.m_AttackDamageMaxLeft, &Obj.m_AttackDamageMaxRight);
		}

		else if (Obj.Class == CLASS_RAGEFIGHTER)
		{
			this->m_Lua.Generic_Call("RageFighterDamageCalc", "iiii>iiii", Strength, Dexterity, Vitality, Energy,
				&Obj.m_AttackDamageMinLeft, &Obj.m_AttackDamageMinRight, &Obj.m_AttackDamageMaxLeft, &Obj.m_AttackDamageMaxRight);
		}

		else if (Obj.Class == CLASS_WIZARD)
		{
			this->m_Lua.Generic_Call("WizardDamageCalc", "iiii>iiii", Strength, Dexterity, Vitality, Energy,
				&Obj.m_AttackDamageMinLeft, &Obj.m_AttackDamageMinRight, &Obj.m_AttackDamageMaxLeft, &Obj.m_AttackDamageMaxRight);
		}

		else if (Obj.Class == CLASS_GROWLANCER)
		{
			this->m_Lua.Generic_Call("GrowLancerDamageCalc", "iiii>iiii", Strength, Dexterity, Vitality, Energy,
				&Obj.m_AttackDamageMinLeft, &Obj.m_AttackDamageMinRight, &Obj.m_AttackDamageMaxLeft, &Obj.m_AttackDamageMaxRight);
		}
	}
	else
	{
		if (Obj.Class == CLASS_ELF) // Elf
		{
			if ((Right->m_Type >= ITEMGET(4, 0) && Right->m_Type < ITEMGET(4, 7)) ||
				(Right->m_Type >= ITEMGET(4, 8) && Right->m_Type < ITEMGET(4, 15)) ||
				(Right->m_Type >= ITEMGET(4, 16) && Right->m_Type < ITEMGET(4, 32)))
			{
				if (Right->IsItem() != FALSE && Right->m_IsValidItem == false)
				{
					Obj.m_AttackDamageMinRight = (Dexterity + Strength) / 7;
					Obj.m_AttackDamageMaxRight = (Dexterity + Strength) / 4;
					Obj.m_AttackDamageMinLeft = (Dexterity + Strength) / 7;
					Obj.m_AttackDamageMaxLeft = (Dexterity + Strength) / 4;
				}
				else
				{
					Obj.m_AttackDamageMinRight = (Dexterity / 7) + (Strength / 14);
					Obj.m_AttackDamageMaxRight = (Dexterity / 4) + (Strength / 8);
					Obj.m_AttackDamageMinLeft = (Dexterity / 7) + (Strength / 14);
					Obj.m_AttackDamageMaxLeft = (Dexterity / 4) + (Strength / 8);
				}
			}
			else
			{
				Obj.m_AttackDamageMinRight = (Dexterity + Strength) / 7;
				Obj.m_AttackDamageMaxRight = (Dexterity + Strength) / 4;
				Obj.m_AttackDamageMinLeft = (Dexterity + Strength) / 7;
				Obj.m_AttackDamageMaxLeft = (Dexterity + Strength) / 4;
			}
		}
		else if (Obj.Class == CLASS_KNIGHT) // Dark Knight
		{
			Obj.m_AttackDamageMinRight = Strength / 6;
			Obj.m_AttackDamageMaxRight = Strength / 4;
			Obj.m_AttackDamageMinLeft = Strength / 6;
			Obj.m_AttackDamageMaxLeft = Strength / 4;
		}
		else if (Obj.Class == CLASS_MAGUMSA) // MAgic Gladiator
		{
			Obj.m_AttackDamageMinRight = (Strength / 6) + (Energy / 12);
			Obj.m_AttackDamageMaxRight = (Strength / 4) + (Energy / 8);
			Obj.m_AttackDamageMinLeft = (Strength / 6) + (Energy / 12);
			Obj.m_AttackDamageMaxLeft = (Strength / 4) + (Energy / 8);
		}
		else if (Obj.Class == CLASS_DARKLORD) // Dark Lord
		{
			Obj.m_AttackDamageMinRight = (Strength / 7) + (Energy / 14);
			Obj.m_AttackDamageMaxRight = (Strength / 5) + (Energy / 10);
			Obj.m_AttackDamageMinLeft = (Strength / 7) + (Energy / 14);
			Obj.m_AttackDamageMaxLeft = (Strength / 5) + (Energy / 10);
		}
		else if (Obj.Class == CLASS_SUMMONER)
		{
			Obj.m_AttackDamageMinRight = (Strength + Dexterity) / 7;
			Obj.m_AttackDamageMaxRight = (Strength + Dexterity) / 4;
			Obj.m_AttackDamageMinLeft = (Strength + Dexterity) / 7;
			Obj.m_AttackDamageMaxLeft = (Strength + Dexterity) / 4;
		}
		else if (Obj.Class == CLASS_RAGEFIGHTER)
		{
			Obj.m_AttackDamageMinRight = (Strength / 7) + (Vitality / 15);
			Obj.m_AttackDamageMaxRight = (Strength / 5) + (Vitality / 12);
			Obj.m_AttackDamageMinLeft = (Strength / 7) + (Vitality / 15);
			Obj.m_AttackDamageMaxLeft = (Strength / 5) + (Vitality / 12);
		}
		else if (Obj.Class == CLASS_GROWLANCER) 
		{
			Obj.m_AttackDamageMinRight = (Strength / 8) + (Dexterity / 10);
			Obj.m_AttackDamageMaxRight = (Strength / 4) + (Dexterity / 6);
			Obj.m_AttackDamageMinLeft = (Strength / 8) + (Dexterity / 10);
			Obj.m_AttackDamageMaxLeft = (Strength / 4) + (Dexterity / 6);
		}
		else
		{
			Obj.m_AttackDamageMinRight = (Strength / 8);
			Obj.m_AttackDamageMaxRight = (Strength / 4);
			Obj.m_AttackDamageMinLeft = (Strength / 8);
			Obj.m_AttackDamageMaxLeft = (Strength / 4);
		}

	}

	g_StatSpec.CalcStatOption(Obj. STAT_OPTION_INC_ATTACK_POWER);

	if (Obj.Class == CLASS_ELF)
	{
		Obj.m_AttackDamageMinRight += Obj.m_PlayerData->m_MPSkillOpt->iMpsAddMinAttack;
		Obj.m_AttackDamageMaxRight += Obj.m_PlayerData->m_MPSkillOpt->iMpsAddMaxAttack;
		Obj.m_AttackDamageMinLeft += Obj.m_PlayerData->m_MPSkillOpt->iMpsAddMinAttack;
		Obj.m_AttackDamageMaxLeft += Obj.m_PlayerData->m_MPSkillOpt->iMpsAddMaxAttack;
	}

	else if (Obj.Class == CLASS_RAGEFIGHTER)
	{
		Obj.m_AttackDamageMinRight += Obj.m_PlayerData->m_MPSkillOpt->iMpsAddMinAttack;
		Obj.m_AttackDamageMaxRight += Obj.m_PlayerData->m_MPSkillOpt->iMpsAddMaxAttack;
		Obj.m_AttackDamageMinLeft += Obj.m_PlayerData->m_MPSkillOpt->iMpsAddMinAttack;
		Obj.m_AttackDamageMaxLeft += Obj.m_PlayerData->m_MPSkillOpt->iMpsAddMaxAttack;
	}

	else if (Obj.Class == CLASS_KNIGHT || Obj.Class == CLASS_MAGUMSA || Obj.Class == CLASS_DARKLORD)
	{
		Obj.m_AttackDamageMinRight += Obj.m_PlayerData->m_MPSkillOpt->iMpsMinAttackDamage;
		Obj.m_AttackDamageMaxRight += Obj.m_PlayerData->m_MPSkillOpt->iMpsMaxAttackDamage;
		Obj.m_AttackDamageMinLeft += Obj.m_PlayerData->m_MPSkillOpt->iMpsMinAttackDamage;
		Obj.m_AttackDamageMaxLeft += Obj.m_PlayerData->m_MPSkillOpt->iMpsMaxAttackDamage;
	}

	Obj.pntInventory[7]->PlusSpecial(&Obj.m_AttackDamageMinRight, 80);
	Obj.pntInventory[7]->PlusSpecial(&Obj.m_AttackDamageMaxRight, 80);
	Obj.pntInventory[7]->PlusSpecial(&Obj.m_AttackDamageMinLeft, 80);
	Obj.pntInventory[7]->PlusSpecial(&Obj.m_AttackDamageMaxLeft, 80);

	int AddLeadership = 0;

	if (Obj.pntInventory[7]->IsItem() != FALSE && Obj.pntInventory[7]->m_IsValidItem != false)
	{
		AddLeadership += Obj.pntInventory[7]->m_Leadership;
	}

	if (Right->m_Type != -1)
	{
		if (Right->m_IsValidItem != false)
		{
			if (Right->m_Type >= ITEMGET(5, 0) && Right->m_Type <= ITEMGET(6, 0))
			{
				Obj.m_AttackDamageMinRight += Right->m_DamageMin / 2;
				Obj.m_AttackDamageMaxRight += Right->m_DamageMax / 2;
			}
			else
			{
				Obj.m_AttackDamageMinRight += Right->m_DamageMin;
				Obj.m_AttackDamageMaxRight += Right->m_DamageMax;
			}
		}

		if (Obj.pntInventory[0]->m_SkillChange != FALSE)
		{
			Obj.SkillLongSpearChange = true;
		}

		Obj.pntInventory[0]->PlusSpecial(&Obj.m_AttackDamageMinRight, 80);
		Obj.pntInventory[0]->PlusSpecial(&Obj.m_AttackDamageMaxRight, 80);
	}

	if (Left->m_Type != -1)
	{
		if (Left->m_IsValidItem != false)
		{
			Obj.m_AttackDamageMinLeft += Left->m_DamageMin;
			Obj.m_AttackDamageMaxLeft += Left->m_DamageMax;
		}

		Obj.pntInventory[1]->PlusSpecial(&Obj.m_AttackDamageMinLeft, 80);
		Obj.pntInventory[1]->PlusSpecial(&Obj.m_AttackDamageMaxLeft, 80);
	}


	Obj.m_CriticalDamage = 0;
	Obj.m_ExcelentDamage = 0;

	Obj.pntInventory[0]->PlusSpecial(&Obj.m_CriticalDamage, 84);
	Obj.pntInventory[1]->PlusSpecial(&Obj.m_CriticalDamage, 84);
	Obj.pntInventory[2]->PlusSpecial(&Obj.m_CriticalDamage, 84);
	Obj.pntInventory[3]->PlusSpecial(&Obj.m_CriticalDamage, 84);
	Obj.pntInventory[4]->PlusSpecial(&Obj.m_CriticalDamage, 84);
	Obj.pntInventory[5]->PlusSpecial(&Obj.m_CriticalDamage, 84);
	Obj.pntInventory[6]->PlusSpecial(&Obj.m_CriticalDamage, 84);
	Obj.pntInventory[7]->PlusSpecial(&Obj.m_CriticalDamage, 84);

	Obj.m_CriticalDamage += gObjGetTotalValueOfEffect(Obj. EFFECTTYPE_CRITICALDAMAGE);
	Obj.m_ExcelentDamage += gObjGetTotalValueOfEffect(Obj. EFFECTTYPE_EXCELLENTDAMAGE);

	if (g_ConfigRead.UseMagicDamageCalc == TRUE)
	{
		switch (Obj.Class)
		{
		case CLASS_ELF:
			this->m_Lua.Generic_Call("ElfMagicDamageCalc", "i>ii", Energy,
				&Obj.m_MagicDamageMin, &Obj.m_MagicDamageMax);
			break;
		case CLASS_KNIGHT:
			this->m_Lua.Generic_Call("KnightMagicDamageCalc", "i>ii", Energy,
				&Obj.m_MagicDamageMin, &Obj.m_MagicDamageMax);
			break;
		case CLASS_MAGUMSA:
			this->m_Lua.Generic_Call("GladiatorMagicDamageCalc", "i>ii", Energy,
				&Obj.m_MagicDamageMin, &Obj.m_MagicDamageMax);
			break;
		case CLASS_DARKLORD:
			this->m_Lua.Generic_Call("LordMagicDamageCalc", "i>ii", Energy,
				&Obj.m_MagicDamageMin, &Obj.m_MagicDamageMax);
			break;
		case CLASS_RAGEFIGHTER:
			this->m_Lua.Generic_Call("RageFighterMagicDamageCalc", "i>ii", Energy,
				&Obj.m_MagicDamageMin, &Obj.m_MagicDamageMax);
			break;
		case CLASS_SUMMONER:
			this->m_Lua.Generic_Call("SummonerMagicDamageCalc", "i>iiii", Energy,
				&Obj.m_MagicDamageMin, &Obj.m_MagicDamageMax, &Obj.m_CurseDamageMin, &Obj.m_CurseDamageMax);
			break;
		case CLASS_WIZARD:
			this->m_Lua.Generic_Call("WizardMagicDamageCalc", "i>ii", Energy,
				&Obj.m_MagicDamageMin, &Obj.m_MagicDamageMax);
			break;
		case CLASS_GROWLANCER:
			this->m_Lua.Generic_Call("GrowLancerMagicDamageCalc", "i>ii", Energy,
				&Obj.m_MagicDamageMin, &Obj.m_MagicDamageMax);
		}
	}
	else
	{
		Obj.m_MagicDamageMin = Energy / 9;
		Obj.m_MagicDamageMax = Energy / 4;

		if (Obj.Class == CLASS_SUMMONER)
		{
			Obj.m_MagicDamageMin = Energy / 9;
			Obj.m_MagicDamageMax = (int)(double(0.015) + Energy / 4);
			Obj.m_CurseDamageMin = Energy / 9;
			Obj.m_CurseDamageMax = (int)(double(0.015) + Energy / 4);
		}
	}

	g_StatSpec.CalcStatOption(Obj. STAT_OPTION_INC_MAGIC_DAMAGE);
	g_StatSpec.CalcStatOption(Obj. STAT_OPTION_INC_CURSE_DAMAGE);

	if (Obj.Class == CLASS_SUMMONER)
	{
		Obj.m_MagicDamageMin += Obj.m_PlayerData->m_MPSkillOpt->iMpsMinMagicDamage;
		Obj.m_MagicDamageMax += Obj.m_PlayerData->m_MPSkillOpt->iMpsMaxMagicDamage;
	}

	if (Obj.Class == CLASS_MAGUMSA)
	{
		Obj.m_MagicDamageMin += Obj.m_PlayerData->m_MPSkillOpt->iMpsMinAttackMagicDamage;
		Obj.m_MagicDamageMax += Obj.m_PlayerData->m_MPSkillOpt->iMpsMaxAttackMagicDamage;
		Obj.m_MagicDamageMin += Obj.m_PlayerData->m_MPSkillOpt->iMpsMinMagicDamage;
		Obj.m_MagicDamageMax += Obj.m_PlayerData->m_MPSkillOpt->iMpsMaxMagicDamage;
		Obj.m_MagicDamageMin += Obj.m_PlayerData->m_MPSkillOpt->iMpsAddMagicDamage;
		Obj.m_MagicDamageMax += Obj.m_PlayerData->m_MPSkillOpt->iMpsAddMagicDamage;
	}

	if (Obj.Class == CLASS_WIZARD)
	{
		Obj.m_MagicDamageMin += Obj.m_PlayerData->m_MPSkillOpt->iMpsMinMagicDamage;
		Obj.m_MagicDamageMax += Obj.m_PlayerData->m_MPSkillOpt->iMpsMaxMagicDamage;
		Obj.m_MagicDamageMin += Obj.m_PlayerData->m_MPSkillOpt->iMpsAddMagicDamage;
		Obj.m_MagicDamageMax += Obj.m_PlayerData->m_MPSkillOpt->iMpsAddMagicDamage;
	}

	Obj.pntInventory[7]->PlusSpecial(&Obj.m_MagicDamageMin, 81);
	Obj.pntInventory[7]->PlusSpecial(&Obj.m_MagicDamageMax, 81);
	Obj.pntInventory[7]->PlusSpecial(&Obj.m_CurseDamageMin, 113);
	Obj.pntInventory[7]->PlusSpecial(&Obj.m_CurseDamageMax, 113);

	if (Right->m_Type != -1)
	{
		if (Obj.pntInventory[0]->m_Type == ITEMGET(0, 31) ||
			Obj.pntInventory[0]->m_Type == ITEMGET(0, 21) ||
			Obj.pntInventory[0]->m_Type == ITEMGET(0, 23) ||
			Obj.pntInventory[0]->m_Type == ITEMGET(0, 25) ||
			Obj.pntInventory[0]->m_Type == ITEMGET(0, 28) ||
			Obj.pntInventory[0]->m_Type == ITEMGET(0, 30))
		{
			Obj.pntInventory[0]->PlusSpecial(&Obj.m_MagicDamageMin, 80);
			Obj.pntInventory[0]->PlusSpecial(&Obj.m_MagicDamageMax, 80);
		}
		else
		{
			Obj.pntInventory[0]->PlusSpecial(&Obj.m_MagicDamageMin, 81);
			Obj.pntInventory[0]->PlusSpecial(&Obj.m_MagicDamageMax, 81);
		}
	}

	if (Left->m_Type != -1)
	{
		Left->PlusSpecial(&Obj.m_CurseDamageMin, 113);
		Left->PlusSpecial(&Obj.m_CurseDamageMax, 113);
	}

	Obj.m_AttackRating = (Strength + Dexterity) / 2;
	Obj.m_AttackRating += Obj.pntInventory[5]->ItemDefense();

	if (g_ConfigRead.UseCalcAttackSpeed == TRUE)
	{
		this->m_Lua.Generic_Call("CalcAttackSpeed", "ii>ii", Obj.Class, Dexterity,
			&Obj.m_AttackSpeed, &Obj.m_MagicSpeed);
	}
	else
	{
		if (Obj.Class == CLASS_ELF) // Elf
		{
			Obj.m_AttackSpeed = Dexterity / 50;
			Obj.m_MagicSpeed = Dexterity / 50;
		}
		else if (Obj.Class == CLASS_KNIGHT || Obj.Class == CLASS_MAGUMSA) // Dark Knigh and MG
		{
			Obj.m_AttackSpeed = Dexterity / 15;
			Obj.m_MagicSpeed = Dexterity / 20;
		}
		else if (Obj.Class == CLASS_DARKLORD)
		{
			Obj.m_AttackSpeed = Dexterity / 10;
			Obj.m_MagicSpeed = Dexterity / 10;
		}
		else if (Obj.Class == CLASS_SUMMONER)
		{
			Obj.m_AttackSpeed = Dexterity / 20;
			Obj.m_MagicSpeed = Dexterity / 20;
		}
		else if (Obj.Class == CLASS_RAGEFIGHTER)
		{
			Obj.m_AttackSpeed = Dexterity / 9;
			Obj.m_MagicSpeed = Dexterity / 9;
		}
		else if (Obj.Class == CLASS_GROWLANCER)
		{
			Obj.m_AttackSpeed = Dexterity / 20;
			Obj.m_MagicSpeed = Dexterity / 20;
		}
		else
		{
			Obj.m_AttackSpeed = Dexterity / 20;
			Obj.m_MagicSpeed = Dexterity / 10;
		}
	}

	
	g_StatSpec.CalcStatOption(Obj. STAT_OPTION_INC_ATTACK_SPEED);

	bool RightItem = 0;

	if (Right->m_Type >= ITEMGET(0, 0) && Right->m_Type < ITEMGET(6, 0) && Right->m_Type != ITEMGET(4, 7) && Right->m_Type != ITEMGET(4, 15))
	{
		if (Right->m_IsValidItem != 0)
		{
			RightItem = 1;
		}
	}

	bool LeftItem = 0;

	if (Left->m_Type >= ITEMGET(0, 0) && Left->m_Type < ITEMGET(6, 0) && Left->m_Type != ITEMGET(4, 7) && Left->m_Type != ITEMGET(4, 15))
	{
		if (Left->m_IsValidItem != 0)
		{
			LeftItem = 1;
		}
	}

	if (RightItem != 0 && LeftItem != 0)
	{
		Obj.m_AttackSpeed += (Right->m_AttackSpeed + Left->m_AttackSpeed) / 2;
		Obj.m_MagicSpeed += (Right->m_AttackSpeed + Left->m_AttackSpeed) / 2;
	}
	else if (RightItem != 0)
	{
		Obj.m_AttackSpeed += Right->m_AttackSpeed;
		Obj.m_MagicSpeed += Right->m_AttackSpeed;
	}
	else if (LeftItem != 0)
	{
		Obj.m_AttackSpeed += Left->m_AttackSpeed;
		Obj.m_MagicSpeed += Left->m_AttackSpeed;
	}

	if (Gloves->IsItem() != 0 && Gloves->m_IsValidItem != 0)
	{
		Obj.m_AttackSpeed += Gloves->m_AttackSpeed;
		Obj.m_MagicSpeed += Gloves->m_AttackSpeed;
	}

	if (Helper->IsItem() != 0 && Helper->m_IsValidItem != 0)
	{
		Obj.m_AttackSpeed += Helper->m_AttackSpeed;
		Obj.m_MagicSpeed += Helper->m_AttackSpeed;
	}

	if (Amulet->IsItem() != 0 && Amulet->m_IsValidItem != 0)
	{
		Obj.m_AttackSpeed += Amulet->m_AttackSpeed;
		Obj.m_MagicSpeed += Amulet->m_AttackSpeed;
	}
	

	if (Wing->IsItem() == TRUE && Wing->m_IsValidItem == true && Wing->m_Type == ITEMGET(12, 268)) // additional bonus of s9 wings
	{
		Obj.m_AttackDamageMinRight += 100 + (Wing->m_Level * 5);
		Obj.m_AttackDamageMaxRight += 100 + (Wing->m_Level * 5);
		Obj.m_AttackDamageMinLeft += 100 + (Wing->m_Level * 5);
		Obj.m_AttackDamageMaxLeft += 100 + (Wing->m_Level * 5);
		Obj.m_MagicDamageMin += 100 + (Wing->m_Level * 5);
		Obj.m_MagicDamageMax += 100 + (Wing->m_Level * 5);
		Obj.m_CurseDamageMin += 100 + (Wing->m_Level * 5);
		Obj.m_CurseDamageMax += 100 + (Wing->m_Level * 5);
	}

	// Wizard Ring effect
	if ((Obj.pntInventory[10]->IsItem() == TRUE && Obj.pntInventory[10]->m_Type == ITEMGET(13, 20) && Obj.pntInventory[10]->m_Level == 0 && Obj.pntInventory[10]->m_Durability > 0.0f) || (Obj.pntInventory[11]->IsItem() == TRUE && Obj.pntInventory[11]->m_Type == ITEMGET(13, 20) && Obj.pntInventory[11]->m_Level == 0 && Obj.pntInventory[11]->m_Durability > 0.0f))
	{
		Obj.m_AttackDamageMinRight += Obj.m_AttackDamageMinRight * g_ConfigRead.pet.WizardRingAddDamage / 100;
		Obj.m_AttackDamageMaxRight += Obj.m_AttackDamageMaxRight * g_ConfigRead.pet.WizardRingAddDamage / 100;
		Obj.m_AttackDamageMinLeft += Obj.m_AttackDamageMinLeft * g_ConfigRead.pet.WizardRingAddDamage / 100;
		Obj.m_AttackDamageMaxLeft += Obj.m_AttackDamageMaxLeft * g_ConfigRead.pet.WizardRingAddDamage / 100;
		Obj.m_MagicDamageMin += Obj.m_MagicDamageMin * g_ConfigRead.pet.WizardRingAddMagicDamage / 100;
		Obj.m_MagicDamageMax += Obj.m_MagicDamageMax * g_ConfigRead.pet.WizardRingAddMagicDamage / 100;
		Obj.m_AttackSpeed += g_ConfigRead.pet.WizardRingAddAttackSpeed;
		Obj.m_MagicSpeed += g_ConfigRead.pet.WizardRingAddAttackSpeed;
	}

	if ((Obj.pntInventory[10]->IsItem() == TRUE && Obj.pntInventory[10]->m_Type == ITEMGET(13, 107) && Obj.pntInventory[10]->m_Level == 0 && Obj.pntInventory[10]->m_Durability > 0.0f) || (Obj.pntInventory[11]->IsItem() == TRUE && Obj.pntInventory[11]->m_Type == ITEMGET(13, 107) && Obj.pntInventory[11]->m_Level == 0 && Obj.pntInventory[11]->m_Durability > 0.0f))
	{
		Obj.m_AttackDamageMinRight += Obj.m_AttackDamageMinRight * g_ConfigRead.pet.LethalRingAddDamage / 100;
		Obj.m_AttackDamageMaxRight += Obj.m_AttackDamageMaxRight * g_ConfigRead.pet.LethalRingAddDamage / 100;
		Obj.m_AttackDamageMinLeft += Obj.m_AttackDamageMinLeft * g_ConfigRead.pet.LethalRingAddDamage / 100;
		Obj.m_AttackDamageMaxLeft += Obj.m_AttackDamageMaxLeft * g_ConfigRead.pet.LethalRingAddDamage / 100;
		Obj.m_MagicDamageMin += Obj.m_MagicDamageMin * g_ConfigRead.pet.LethalRingAddMagicDamage / 100;
		Obj.m_MagicDamageMax += Obj.m_MagicDamageMax * g_ConfigRead.pet.LethalRingAddMagicDamage / 100;
		Obj.m_AttackSpeed += g_ConfigRead.pet.LethalRingAddAttackSpeed;
		Obj.m_MagicSpeed += g_ConfigRead.pet.LethalRingAddAttackSpeed;
	}

	if (Helper->m_Type == ITEMGET(13, 106) && Helper->IsPeriodItemExpire() == FALSE)
	{
		Obj.m_Defense += g_ConfigRead.pet.UnicornAddDefense;
	}

	if (Helper->m_Type == ITEMGET(13, 123) && Helper->IsPeriodItemExpire() == FALSE)
	{
		Obj.m_AttackDamageMinRight += Obj.m_AttackDamageMinRight * g_ConfigRead.pet.EliteSkeletonPetAddDamage / 100;
		Obj.m_AttackDamageMaxRight += Obj.m_AttackDamageMaxRight * g_ConfigRead.pet.EliteSkeletonPetAddDamage / 100;
		Obj.m_AttackDamageMinLeft += Obj.m_AttackDamageMinLeft * g_ConfigRead.pet.EliteSkeletonPetAddDamage / 100;
		Obj.m_AttackDamageMaxLeft += Obj.m_AttackDamageMaxLeft * g_ConfigRead.pet.EliteSkeletonPetAddDamage / 100;
		Obj.m_MagicDamageMin += Obj.m_MagicDamageMin * g_ConfigRead.pet.EliteSkeletonPetAddDamage / 100;
		Obj.m_MagicDamageMax += Obj.m_MagicDamageMax * g_ConfigRead.pet.EliteSkeletonPetAddDamage / 100;
		Obj.m_AttackSpeed += g_ConfigRead.pet.EliteSkeletonPetAddAttackSpeed;
		Obj.m_MagicSpeed += g_ConfigRead.pet.EliteSkeletonPetAddAttackSpeed;
	}

	if (Obj.Class == CLASS_WIZARD) // Dark Wizard;
	{
		Obj.m_DetectSpeedHackTime = (int)(gAttackSpeedTimeLimit - (Obj.m_MagicSpeed * 2 * gDecTimePerAttackSpeed));
	}
	else
	{
		Obj.m_DetectSpeedHackTime = (int)(gAttackSpeedTimeLimit - (Obj.m_AttackSpeed * gDecTimePerAttackSpeed));
	}

	if (Obj.m_DetectSpeedHackTime < gMinimumAttackSpeedTime)
	{
		Obj.m_DetectSpeedHackTime = gMinimumAttackSpeedTime;
	}

	this->m_Lua.Generic_Call("CalcAttackSuccessRate_PvM", "iiiii>i", Obj.Class, Strength, Dexterity,
		Leadership, (Obj.Level + Obj.m_PlayerData->MasterLevel), &Obj.m_PlayerData->m_AttackRatePvM);

	this->m_Lua.Generic_Call("CalcAttackSuccessRate_PvP", "iii>d", Obj.Class, (Obj.m_PlayerData->Dexterity + Obj.AddDexterity), Obj.Level + Obj.m_PlayerData->MasterLevel, &Obj.m_PlayerData->m_AttackRatePvP);

	if (g_ConfigRead.UseCalcDefenseSuccessRate_PvM == TRUE)
	{
		this->m_Lua.Generic_Call("CalcDefenseSuccessRate_PvM", "ii>i", Obj.Class, Dexterity,	&Obj.m_SuccessfulBlocking);
	}
	else
	{
		if (Obj.Class == CLASS_ELF) // elf
		{
			Obj.m_SuccessfulBlocking = Dexterity / 4;
		}
		else if (Obj.Class == CLASS_DARKLORD) // DL
		{
			Obj.m_SuccessfulBlocking = Dexterity / 7;
		}
		else if (Obj.Class == CLASS_SUMMONER)
		{
			Obj.m_SuccessfulBlocking = Dexterity / 4;
		}
		else if (Obj.Class == CLASS_RAGEFIGHTER)
		{
			Obj.m_SuccessfulBlocking = Dexterity / 10;
		}
		else if (Obj.Class == CLASS_GROWLANCER) 
		{
			Obj.m_SuccessfulBlocking = Dexterity / 4;
		}
		else
		{
			Obj.m_SuccessfulBlocking = Dexterity / 3;
		}
	}

	if (g_ConfigRead.UseCalcDefenseSuccessRate_PvP == TRUE)
	{
		this->m_Lua.Generic_Call("CalcDefenseSuccessRate_PvP", "iii>d", Obj.Class, (Obj.m_PlayerData->Dexterity + Obj.AddDexterity), Obj.Level + Obj.m_PlayerData->MasterLevel, &Obj.m_PlayerData->m_DefenseRatePvP);
	}
	else
	{
		if (Obj.Class == CLASS_ELF) // elf
		{
			Obj.m_PlayerData->m_DefenseRatePvP = Dexterity / 4;
		}
		else if (Obj.Class == CLASS_DARKLORD) // DL
		{
			Obj.m_PlayerData->m_DefenseRatePvP = Dexterity / 7;
		}
		else if (Obj.Class == CLASS_SUMMONER)
		{
			Obj.m_PlayerData->m_DefenseRatePvP = Dexterity / 4;
		}
		else if (Obj.Class == CLASS_RAGEFIGHTER)
		{
			Obj.m_PlayerData->m_DefenseRatePvP = Dexterity / 10;
		}
		else if (Obj.Class == CLASS_GROWLANCER)
		{
			Obj.m_PlayerData->m_DefenseRatePvP = Dexterity / 4;
		}
		else
		{
			Obj.m_PlayerData->m_DefenseRatePvP = Dexterity / 3;
		}
	}

	g_StatSpec.CalcStatOption(Obj. STAT_OPTION_INC_ATTACK_RATE);
	g_StatSpec.CalcStatOption(Obj. STAT_OPTION_INC_ATTACK_RATE_PVP);
	g_StatSpec.CalcStatOption(Obj. STAT_OPTION_INC_DEFENSE_RATE);
	g_StatSpec.CalcStatOption(Obj. STAT_OPTION_INC_DEFENSE_RATE_PVP);

	if (Left->m_Type != -1)
	{
		if (Left->m_IsValidItem != false)
		{
			Obj.m_SuccessfulBlocking += Left->m_SuccessfulBlocking;
			Obj.pntInventory[1]->PlusSpecial(&Obj.m_SuccessfulBlocking, 82);
		}
	}

	bool Success = true;

	if (Obj.Class == CLASS_MAGUMSA)
	{
		for (int j = 3; j <= 6; j++)
		{
			if (Obj.pntInventory[j]->m_Type == -1)
			{
				Success = false;
				break;
			}

			if (Obj.pntInventory[j]->m_IsValidItem == false)
			{
				Success = false;
				break;
			}
		}
	}
	else if (Obj.Class == CLASS_RAGEFIGHTER)
	{
		for (int l = 2; l <= 6; l++)
		{
			if (l == 5)
			{
				continue;
			}

			if (Obj.pntInventory[l]->m_Type == -1)
			{
				Success = false;
				break;
			}

			if (Obj.pntInventory[l]->m_IsValidItem == false)
			{
				Success = false;
				break;
			}
		}
	}

	else
	{
		for (int k = 2; k <= 6; k++)
		{
			if (Obj.pntInventory[k]->m_Type == -1)
			{
				Success = false;
				break;
			}

			if (Obj.pntInventory[k]->m_IsValidItem == false)
			{
				Success = false;
				break;
			}
		}
	}

	int Level11Count = 0;
	int Level10Count = 0;
	int Level12Count = 0;
	int Level13Count = 0;
	int Level14Count = 0;
	int Level15Count = 0;

	if (Success != false)
	{
		int in;

		if (Obj.Class == CLASS_MAGUMSA)
		{
			in = Obj.pntInventory[3]->m_Type % MAX_SUBTYPE_ITEMS;

			if (in != ITEMGET(0, 15) &&
				in != ITEMGET(0, 20) &&
				in != ITEMGET(0, 23) &&
				in != ITEMGET(0, 33) &&
				in != ITEMGET(0, 32) &&
				in != ITEMGET(0, 37))
			{
				Success = false;
			}
			else
			{
				Level13Count++;

				for (int m = 3; m <= 6; m++)
				{
					if (in != (Obj.pntInventory[m]->m_Type % MAX_SUBTYPE_ITEMS))
					{
						Success = false;
					}
					if (Obj.pntInventory[m]->m_Level > 14)
					{
						Level15Count++;
					}
					else if (Obj.pntInventory[m]->m_Level > 13)
					{
						Level14Count++;
					}
					else if (Obj.pntInventory[m]->m_Level > 12)
					{
						Level13Count++;
					}
					else if (Obj.pntInventory[m]->m_Level > 11)
					{
						Level12Count++;
					}
					else if (Obj.pntInventory[m]->m_Level > 10)
					{
						Level11Count++;
					}
					else if (Obj.pntInventory[m]->m_Level > 9)
					{
						Level10Count++;
					}
				}
			}
		}
		else
		{
			in = Obj.pntInventory[2]->m_Type % MAX_SUBTYPE_ITEMS;

			for (int m = 2; m <= 6; m++)
			{
				if (in != (Obj.pntInventory[m]->m_Type % MAX_SUBTYPE_ITEMS))
				{
					Success = false;
				}
				if (Obj.pntInventory[m]->m_Level > 14)
				{
					Level15Count++;
				}
				else if (Obj.pntInventory[m]->m_Level > 13)
				{
					Level14Count++;
				}
				else if (Obj.pntInventory[m]->m_Level > 12)
				{
					Level13Count++;
				}
				else if (Obj.pntInventory[m]->m_Level > 11)
				{
					Level12Count++;
				}
				else if (Obj.pntInventory[m]->m_Level > 10)
				{
					Level11Count++;
				}
				else if (Obj.pntInventory[m]->m_Level > 9)
				{
					Level10Count++;
				}
			}
		}

		if (Success != false)	// #warning unuseful if
		{
			Obj.m_SuccessfulBlocking += Obj.m_SuccessfulBlocking / 10;
		}
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsIncDefenseSuccessRate > 0.0)
	{
		Obj.m_SuccessfulBlocking += Obj.m_PlayerData->m_MPSkillOpt->iMpsIncDefenseSuccessRate;
	}

	Obj.m_SuccessfulBlocking += Obj.m_PlayerData->m_MPSkillOpt->iMpsShieldBlock;
	Obj.m_SuccessfulBlocking += Obj.m_PlayerData->m_MPSkillOpt->iMpsShieldBlockRate;
	Obj.m_SuccessfulBlocking += Obj.m_PlayerData->m_MPSkillOpt->iMpsElfShieldBlockRate;
	Obj.m_SuccessfulBlocking += Obj.m_PlayerData->m_MPSkillOpt->iMpsAddShieldBlockingRage;
	Obj.m_SuccessfulBlocking += Obj.m_PlayerData->m_MPSkillOpt->iMpsAddBlockingRate;

	if (g_ConfigRead.UseCalcDefense == TRUE)
	{
		this->m_Lua.Generic_Call("CalcDefense", "ii>i", Obj.Class, Dexterity, &Obj.m_Defense);
	}
	else
	{
		if (Obj.Class == CLASS_ELF) // Elf
		{
			Obj.m_Defense = Dexterity / 10;
		}
		else if (Obj.Class == 1) // DK
		{
			Obj.m_Defense = Dexterity / 3;
		}
		else if (Obj.Class == 4) // DL
		{
			Obj.m_Defense = Dexterity / 7;
		}
		else if (Obj.Class == CLASS_SUMMONER)
		{
			Obj.m_Defense = Dexterity / 3;
		}
		else if (Obj.Class == CLASS_RAGEFIGHTER)
		{
			Obj.m_Defense = Dexterity / 8;
		}
		else if (Obj.Class == CLASS_GROWLANCER) 
		{
			Obj.m_Defense = Dexterity / 7;
		}
		else
		{
			Obj.m_Defense = Dexterity / 4;
		}
	}
	
	g_StatSpec.CalcStatOption(Obj. STAT_OPTION_INC_DEFENSE);

	Obj.m_Defense += Obj.pntInventory[2]->ItemDefense();
	Obj.m_Defense += Obj.pntInventory[3]->ItemDefense();
	Obj.m_Defense += Obj.pntInventory[4]->ItemDefense();
	Obj.m_Defense += Obj.pntInventory[5]->ItemDefense();
	Obj.m_Defense += Obj.pntInventory[6]->ItemDefense();
	Obj.m_Defense += Obj.pntInventory[1]->ItemDefense();
	Obj.m_Defense += Obj.pntInventory[7]->ItemDefense();

	if (Obj.m_btInvenPetPos != FALSE && Obj.pntInventory[Obj.m_btInvenPetPos]->m_JewelOfHarmonyOption == TRUE)
	{
		if (Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 4) && Obj.pntInventory[Obj.m_btInvenPetPos]->m_Durability > 0.0f)	// Dark Horse
		{
			int addDefense = 0;
			this->m_Lua.Generic_Call("CalcDarkHorseDefenseBonus", "ii>i", Dexterity, Obj.pntInventory[Obj.m_btInvenPetPos]->m_PetItem_Level, &addDefense);

			//int addDefense = Dexterity / 20 + 5 + Obj.pntInventory[Obj.m_btInvenPetPos]->m_PetItem_Level * 2;
			//addDefense = addDefense * g_ConfigRead.calc.DarkHorseDefenseMultiplier / 100.0f;

			Obj.m_Defense += addDefense;
		}
	}

	if ((Level15Count + Level14Count + Level13Count + Level12Count + Level11Count + Level10Count) >= 5)
	{
		if (Success != false)
		{
			if (Level15Count == 5)
			{
				Obj.m_Defense += Obj.m_Defense * 30 / 100;
			}
			else if (Level14Count == 5 || (Level14Count + Level15Count) == 5)
			{
				Obj.m_Defense += Obj.m_Defense * 25 / 100;
			}
			else if (Level13Count == 5 || (Level13Count + Level14Count + Level15Count) == 5)
			{
				Obj.m_Defense += Obj.m_Defense * 20 / 100;
			}
			else if (Level12Count == 5 || (Level12Count + Level13Count + Level14Count + Level15Count) == 5)
			{
				Obj.m_Defense += Obj.m_Defense * 15 / 100;
			}
			else if (Level11Count == 5 || (Level11Count + Level12Count + Level13Count + Level14Count + Level15Count) == 5)
			{
				Obj.m_Defense += Obj.m_Defense * 10 / 100;
			}
			else if (Level10Count == 5 || (Level10Count + Level11Count + Level12Count + Level13Count + Level14Count + Level15Count) == 5)
			{
				Obj.m_Defense += Obj.m_Defense * 5 / 100;
			}
		}
	}

	float fValue = Obj.m_Defense;

	fValue += Obj.m_PlayerData->m_MPSkillOpt->iMpsDefence;
	
	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsMonkAddVitalToDefense > 0.0)
	{
		fValue += Vitality / Obj.m_PlayerData->m_MPSkillOpt->iMpsMonkAddVitalToDefense;
	}

	BOOL nFullSet = TRUE;

	if (Obj.Class == CLASS_MAGUMSA)
	{
		if (Armor->IsItem() == FALSE)
			nFullSet = FALSE;
		if (Gloves->IsItem() == FALSE)
			nFullSet = FALSE;
		if (Pants->IsItem() == FALSE)
			nFullSet = FALSE;
		if (Boots->IsItem() == FALSE)
			nFullSet = FALSE;
	}

	if (Obj.Class == CLASS_RAGEFIGHTER)
	{
		if (Helmet->IsItem() == FALSE)
			nFullSet = FALSE;
		if (Armor->IsItem() == FALSE)
			nFullSet = FALSE;
		if (Pants->IsItem() == FALSE)
			nFullSet = FALSE;
		if (Boots->IsItem() == FALSE)
			nFullSet = FALSE;
	}

	else
	{
		if (Helmet->IsItem() == FALSE)
			nFullSet = FALSE;
		if (Armor->IsItem() == FALSE)
			nFullSet = FALSE;
		if (Gloves->IsItem() == FALSE)
			nFullSet = FALSE;
		if (Pants->IsItem() == FALSE)
			nFullSet = FALSE;
		if (Boots->IsItem() == FALSE)
			nFullSet = FALSE;
	}

	if (nFullSet == TRUE)
	{
		fValue += Obj.m_PlayerData->m_MPSkillOpt->iMpsIncSetItemDefense;
	}

	Obj.m_Defense = fValue;
	Obj.m_Defense = Obj.m_Defense * 10 / 20;

	if ((LeftRing->IsItem() == TRUE && LeftRing->m_Type == ITEMGET(13, 10) && LeftRing->m_Level == 5) || (RightRing->IsItem() == TRUE && RightRing->m_Type == ITEMGET(13, 10) && RightRing->m_Level == 5))
	{
		Obj.AddLife = ((int)(Obj.MaxLife * 20.0f)) / 100;
	}

	else if ((LeftRing->IsItem() == TRUE && LeftRing->m_Type == ITEMGET(13, 39)) || (RightRing->IsItem() == TRUE && RightRing->m_Type == ITEMGET(13, 39)))
	{
		Obj.m_Defense += (Obj.m_Defense * g_ConfigRead.pet.EliteSkeletonRingAddDefense / 100);
		Obj.AddLife += Obj.Level + Obj.m_PlayerData->MasterLevel;
	}

	else if ((LeftRing->IsItem() == TRUE && LeftRing->m_Type == ITEMGET(13, 41)) || (RightRing->IsItem() == TRUE && RightRing->m_Type == ITEMGET(13, 41)))
	{
		Obj.m_AttackDamageMinRight += Obj.m_AttackDamageMinRight * g_ConfigRead.pet.ChristmasRingAddDamage / 100;
		Obj.m_AttackDamageMaxRight += Obj.m_AttackDamageMaxRight * g_ConfigRead.pet.ChristmasRingAddDamage / 100;
		Obj.m_AttackDamageMinLeft += Obj.m_AttackDamageMinLeft * g_ConfigRead.pet.ChristmasRingAddDamage / 100;
		Obj.m_AttackDamageMaxLeft += Obj.m_AttackDamageMaxLeft * g_ConfigRead.pet.ChristmasRingAddDamage / 100;
		Obj.m_MagicDamageMin += Obj.m_MagicDamageMin * g_ConfigRead.pet.ChristmasRingAddMagicDamage / 100;
		Obj.m_MagicDamageMax += Obj.m_MagicDamageMax * g_ConfigRead.pet.ChristmasRingAddMagicDamage / 100;
	}
	else if ((LeftRing->IsItem() == TRUE && LeftRing->m_Type == ITEMGET(13, 42)) || (RightRing->IsItem() == TRUE && RightRing->m_Type == ITEMGET(13, 42)))
	{
		Obj.m_AddResistance[R_ICE] += (char)255;
		Obj.m_AddResistance[R_POISON] += (char)255;
		Obj.m_AddResistance[R_LIGHTNING] += (char)255;
		Obj.m_AddResistance[R_FIRE] += (char)255;
		Obj.m_AddResistance[R_EARTH] += (char)255;
		Obj.m_AddResistance[R_WIND] += (char)255;
		Obj.m_AddResistance[R_WATER] += (char)255;
	}

	else if ((LeftRing->IsItem() == TRUE && LeftRing->m_Type == ITEMGET(13, 76)) || (RightRing->IsItem() == TRUE && RightRing->m_Type == ITEMGET(13, 76)))
	{
		Obj.m_AttackDamageMinRight += g_ConfigRead.pet.PandaRingAddDamage;
		Obj.m_AttackDamageMaxRight += g_ConfigRead.pet.PandaRingAddDamage;
		Obj.m_AttackDamageMinLeft += g_ConfigRead.pet.PandaRingAddDamage;
		Obj.m_AttackDamageMaxLeft += g_ConfigRead.pet.PandaRingAddDamage;
		Obj.m_MagicDamageMin += g_ConfigRead.pet.PandaRingAddDamage;
		Obj.m_MagicDamageMax += g_ConfigRead.pet.PandaRingAddDamage;
		Obj.m_CurseDamageMin += g_ConfigRead.pet.PandaRingAddDamage;
		Obj.m_CurseDamageMax += g_ConfigRead.pet.PandaRingAddDamage;
	}

	else if ((LeftRing->IsItem() == TRUE && LeftRing->m_Type == ITEMGET(13, 77)) || (RightRing->IsItem() == TRUE && RightRing->m_Type == ITEMGET(13, 77)))
	{
		Obj.m_AttackDamageMinRight += g_ConfigRead.pet.BrownPandaRingAddDamage;
		Obj.m_AttackDamageMaxRight += g_ConfigRead.pet.BrownPandaRingAddDamage;
		Obj.m_AttackDamageMinLeft += g_ConfigRead.pet.BrownPandaRingAddDamage;
		Obj.m_AttackDamageMaxLeft += g_ConfigRead.pet.BrownPandaRingAddDamage;
		Obj.m_MagicDamageMin += g_ConfigRead.pet.BrownPandaRingAddDamage;
		Obj.m_MagicDamageMax += g_ConfigRead.pet.BrownPandaRingAddDamage;
		Obj.m_CurseDamageMin += g_ConfigRead.pet.BrownPandaRingAddDamage;
		Obj.m_CurseDamageMax += g_ConfigRead.pet.BrownPandaRingAddDamage;
	}

	else if ((LeftRing->IsItem() == TRUE && LeftRing->m_Type == ITEMGET(13, 78)) || (RightRing->IsItem() == TRUE && RightRing->m_Type == ITEMGET(13, 78)))
	{
		Obj.m_AttackDamageMinRight += g_ConfigRead.pet.PinkPandaRingAddDamage;
		Obj.m_AttackDamageMaxRight += g_ConfigRead.pet.PinkPandaRingAddDamage;
		Obj.m_AttackDamageMinLeft += g_ConfigRead.pet.PinkPandaRingAddDamage;
		Obj.m_AttackDamageMaxLeft += g_ConfigRead.pet.PinkPandaRingAddDamage;
		Obj.m_MagicDamageMin += g_ConfigRead.pet.PinkPandaRingAddDamage;
		Obj.m_MagicDamageMax += g_ConfigRead.pet.PinkPandaRingAddDamage;
		Obj.m_CurseDamageMin += g_ConfigRead.pet.PinkPandaRingAddDamage;
		Obj.m_CurseDamageMax += g_ConfigRead.pet.PinkPandaRingAddDamage;
	}

	else if ((LeftRing->IsItem() == TRUE && LeftRing->m_Type == ITEMGET(13, 122)) || (RightRing->IsItem() == TRUE && RightRing->m_Type == ITEMGET(13, 122)))
	{
		Obj.m_AttackDamageMinRight += g_ConfigRead.pet.SkeletonRingAddDamage;
		Obj.m_AttackDamageMaxRight += g_ConfigRead.pet.SkeletonRingAddDamage;
		Obj.m_AttackDamageMinLeft += g_ConfigRead.pet.SkeletonRingAddDamage;
		Obj.m_AttackDamageMaxLeft += g_ConfigRead.pet.SkeletonRingAddDamage;
		Obj.m_MagicDamageMin += g_ConfigRead.pet.SkeletonRingAddDamage;
		Obj.m_MagicDamageMax += g_ConfigRead.pet.SkeletonRingAddDamage;
		Obj.m_CurseDamageMin += g_ConfigRead.pet.SkeletonRingAddDamage;
		Obj.m_CurseDamageMax += g_ConfigRead.pet.SkeletonRingAddDamage;
	}

	else if ((LeftRing->IsItem() == TRUE && LeftRing->m_Type == ITEMGET(13, 163)) || (RightRing->IsItem() == TRUE && RightRing->m_Type == ITEMGET(13, 163)))
	{
		Obj.m_Defense += g_ConfigRead.pet.RobotKnightRingAddDefense;
		Obj.m_AttackDamageMinRight += g_ConfigRead.pet.RobotKnightRingAddDamage;
		Obj.m_AttackDamageMaxRight += g_ConfigRead.pet.RobotKnightRingAddDamage;
		Obj.m_AttackDamageMinLeft += g_ConfigRead.pet.RobotKnightRingAddDamage;
		Obj.m_AttackDamageMaxLeft += g_ConfigRead.pet.RobotKnightRingAddDamage;
		Obj.m_MagicDamageMin += g_ConfigRead.pet.RobotKnightRingAddDamage;
		Obj.m_MagicDamageMax += g_ConfigRead.pet.RobotKnightRingAddDamage;
		Obj.m_CurseDamageMin += g_ConfigRead.pet.RobotKnightRingAddDamage;
		Obj.m_CurseDamageMax += g_ConfigRead.pet.RobotKnightRingAddDamage;
	}

	else if ((LeftRing->IsItem() == TRUE && LeftRing->m_Type == ITEMGET(13, 164)) || (RightRing->IsItem() == TRUE && RightRing->m_Type == ITEMGET(13, 164)))
	{
		Obj.m_AttackSpeed += g_ConfigRead.pet.MiniRobotRingAddAttackSpeed;
		Obj.m_AttackDamageMinRight += g_ConfigRead.pet.MiniRobotRingAddDamage;
		Obj.m_AttackDamageMaxRight += g_ConfigRead.pet.MiniRobotRingAddDamage;
		Obj.m_AttackDamageMinLeft += g_ConfigRead.pet.MiniRobotRingAddDamage;
		Obj.m_AttackDamageMaxLeft += g_ConfigRead.pet.MiniRobotRingAddDamage;
		Obj.m_MagicDamageMin += g_ConfigRead.pet.MiniRobotRingAddDamage;
		Obj.m_MagicDamageMax += g_ConfigRead.pet.MiniRobotRingAddDamage;
		Obj.m_CurseDamageMin += g_ConfigRead.pet.MiniRobotRingAddDamage;
		Obj.m_CurseDamageMax += g_ConfigRead.pet.MiniRobotRingAddDamage;
	}

	else if ((LeftRing->IsItem() == TRUE && LeftRing->m_Type == ITEMGET(13, 165)) || (RightRing->IsItem() == TRUE && RightRing->m_Type == ITEMGET(13, 165)))
	{
		Obj.m_AttackDamageMinRight += g_ConfigRead.pet.MageRingAddDamage;
		Obj.m_AttackDamageMaxRight += g_ConfigRead.pet.MageRingAddDamage;
		Obj.m_AttackDamageMinLeft += g_ConfigRead.pet.MageRingAddDamage;
		Obj.m_AttackDamageMaxLeft += g_ConfigRead.pet.MageRingAddDamage;
		Obj.m_MagicDamageMin += g_ConfigRead.pet.MageRingAddDamage;
		Obj.m_MagicDamageMax += g_ConfigRead.pet.MageRingAddDamage;
		Obj.m_CurseDamageMin += g_ConfigRead.pet.MageRingAddDamage;
		Obj.m_CurseDamageMax += g_ConfigRead.pet.MageRingAddDamage;
	}

	else if ((LeftRing->IsItem() == TRUE && LeftRing->m_Type == ITEMGET(13, 169) && LeftRing->m_Durability > 0.0) || (RightRing->IsItem() == TRUE && RightRing->m_Type == ITEMGET(13, 169) && RightRing->m_Durability > 0.0))
	{
		Obj.m_AttackDamageMinRight += Obj.m_AttackDamageMinRight * g_ConfigRead.pet.DecorationRingAddDamage / 100;
		Obj.m_AttackDamageMaxRight += Obj.m_AttackDamageMaxRight * g_ConfigRead.pet.DecorationRingAddDamage / 100;
		Obj.m_AttackDamageMinLeft += Obj.m_AttackDamageMinLeft * g_ConfigRead.pet.DecorationRingAddDamage / 100;
		Obj.m_AttackDamageMaxLeft += Obj.m_AttackDamageMaxLeft * g_ConfigRead.pet.DecorationRingAddDamage / 100;
		Obj.m_MagicDamageMin += Obj.m_MagicDamageMin * g_ConfigRead.pet.DecorationRingAddDamage / 100;
		Obj.m_MagicDamageMax += Obj.m_MagicDamageMax * g_ConfigRead.pet.DecorationRingAddDamage / 100;
		Obj.m_AttackSpeed += g_ConfigRead.pet.DecorationRingAddAttackSpeed;
		Obj.m_MagicSpeed += g_ConfigRead.pet.DecorationRingAddAttackSpeed;
	}

	else if ((LeftRing->IsItem() == TRUE && LeftRing->m_Type == ITEMGET(13, 170) && LeftRing->m_Durability > 0.0) || (RightRing->IsItem() == TRUE && RightRing->m_Type == ITEMGET(13, 170) && RightRing->m_Durability > 0.0))
	{
		Obj.m_AttackDamageMinRight += Obj.m_AttackDamageMinRight * g_ConfigRead.pet.BlessedDecorationRingAddDamage / 100;
		Obj.m_AttackDamageMaxRight += Obj.m_AttackDamageMaxRight * g_ConfigRead.pet.BlessedDecorationRingAddDamage / 100;
		Obj.m_AttackDamageMinLeft += Obj.m_AttackDamageMinLeft * g_ConfigRead.pet.BlessedDecorationRingAddDamage / 100;
		Obj.m_AttackDamageMaxLeft += Obj.m_AttackDamageMaxLeft * g_ConfigRead.pet.BlessedDecorationRingAddDamage / 100;
		Obj.m_MagicDamageMin += Obj.m_MagicDamageMin * g_ConfigRead.pet.BlessedDecorationRingAddDamage / 100;
		Obj.m_MagicDamageMax += Obj.m_MagicDamageMax * g_ConfigRead.pet.BlessedDecorationRingAddDamage / 100;
		Obj.m_AttackSpeed += g_ConfigRead.pet.BlessedDecorationRingAddAttackSpeed;
		Obj.m_MagicSpeed += g_ConfigRead.pet.BlessedDecorationRingAddAttackSpeed;
	}

	else if ((LeftRing->IsItem() == TRUE && LeftRing->m_Type == ITEMGET(13, 268)) || (RightRing->IsItem() == TRUE && RightRing->m_Type == ITEMGET(13, 268)))
	{
		Obj.m_AttackDamageMinRight += g_ConfigRead.pet.DSFRingAddDamage;
		Obj.m_AttackDamageMaxRight += g_ConfigRead.pet.DSFRingAddDamage;
		Obj.m_AttackDamageMinLeft += g_ConfigRead.pet.DSFRingAddDamage;
		Obj.m_AttackDamageMaxLeft += g_ConfigRead.pet.DSFRingAddDamage;
		Obj.m_MagicDamageMin += g_ConfigRead.pet.DSFRingAddDamage;
		Obj.m_MagicDamageMax += g_ConfigRead.pet.DSFRingAddDamage;
		Obj.m_CurseDamageMin += g_ConfigRead.pet.DSFRingAddDamage;
		Obj.m_CurseDamageMax += g_ConfigRead.pet.DSFRingAddDamage;
		Obj.m_AttackSpeed += g_ConfigRead.pet.DSFRingAddAttackSpeed;
		Obj.m_MagicSpeed += g_ConfigRead.pet.DSFRingAddAttackSpeed;
		Obj.m_Defense += g_ConfigRead.pet.DSFRingAddDefense;
		Obj.AddLife += g_ConfigRead.pet.DSFRingAddHP;
	}

	if (Obj.pntInventory[8]->m_Type == ITEMGET(13, 0))
	{
		Obj.AddLife += g_ConfigRead.pet.AngelAddHP;
	}

	if (Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 37) && Obj.pntInventory[Obj.m_btInvenPetPos]->m_NewOption == 4 && Obj.pntInventory[Obj.m_btInvenPetPos]->m_Durability > 0.0f && Obj.pntInventory[Obj.m_btInvenPetPos]->m_JewelOfHarmonyOption == TRUE) //Golden Fenrir Attribute
	{
		Obj.AddLife += 200;	//Life +200
		Obj.AddMana += 200;	//Mana +200

		/*
		//Attack Power Already Coded on zzzitem.cpp
		Obj.m_AttackDamageMaxLeft += Obj.m_AttackDamageMaxLeft * 33 / 100; //Attack Power +33
		Obj.m_AttackDamageMinLeft += Obj.m_AttackDamageMinLeft * 33 / 100; //Attack Power +33
		Obj.m_AttackDamageMaxRight += Obj.m_AttackDamageMaxRight * 33 / 100; //Attack Power +33
		Obj.m_AttackDamageMinRight += Obj.m_AttackDamageMinRight * 33 / 100; //Attack Power +33
		*/

		Obj.m_MagicDamageMin += Obj.m_MagicDamageMin * 16 / 100; //Wizardry +16
		Obj.m_MagicDamageMax += Obj.m_MagicDamageMax * 16 / 100; //Wizardry +16
	}

	if (Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 3) && Obj.pntInventory[Obj.m_btInvenPetPos]->m_Durability > 0.0 && Obj.pntInventory[Obj.m_btInvenPetPos]->m_JewelOfHarmonyOption == TRUE)
	{
		Obj.pntInventory[Obj.m_btInvenPetPos]->PlusSpecial(&Obj.AddBP, 103);
	}

	if (Obj.Type == OBJ_USER && Obj.m_PlayerData->ISBOT == false)
	{
		gDarkSpirit[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].Set(Obj.m_Index, &Obj.pntInventory[Obj.m_btInvenPetPos]);
	}
	/*if ( Obj.Type == OBJ_USER && Obj.m_PlayerData->ISBOT == false )
	{
		gDarkSpirit[Obj.m_Index- g_ConfigRead.server.GetObjectStartUserIndex()].Set(Obj.m_Index, &Obj.pntInventory[1]);
	}*/

	if (Obj.Class == CLASS_SUMMONER && Left->IsItem())
	{
		Obj.m_CurseSpell = Left->IsCurseSpell();
	}

	int nMuunItemEffectValue = 0;

	if (g_CMuunSystem.GetMuunItemValueOfOptType(Obj. MUUN_INC_ATTACK_POWER, &nMuunItemEffectValue, 0) == 1)
	{
		Obj.m_AttackDamageMaxLeft += nMuunItemEffectValue;
		Obj.m_AttackDamageMinLeft += nMuunItemEffectValue;
		Obj.m_AttackDamageMaxRight += nMuunItemEffectValue;
		Obj.m_AttackDamageMinRight += nMuunItemEffectValue;
		Obj.m_MagicDamageMin += nMuunItemEffectValue;
		Obj.m_MagicDamageMax += nMuunItemEffectValue;
		Obj.m_CurseDamageMin += nMuunItemEffectValue;
		Obj.m_CurseDamageMax += nMuunItemEffectValue;
	}

	if (g_CMuunSystem.GetMuunItemValueOfOptType(Obj. MUUN_INC_MAX_ATTACK_POWER, &nMuunItemEffectValue, 0) == 1)
	{
		Obj.m_AttackDamageMaxLeft += nMuunItemEffectValue;
		Obj.m_AttackDamageMaxRight += nMuunItemEffectValue;
		Obj.m_MagicDamageMax += nMuunItemEffectValue;
		Obj.m_CurseDamageMax += nMuunItemEffectValue;
	}

	Obj.pntInventory[9]->PlusSpecialPercentEx(&Obj.AddBP, Obj.MaxBP, 173);
	Obj.pntInventory[10]->PlusSpecialPercentEx(&Obj.AddMana, Obj.MaxMana, 172);
	Obj.pntInventory[11]->PlusSpecialPercentEx(&Obj.AddMana, Obj.MaxMana, 172);

	CItemObject * rItem[3];
	int comparecount = 0;

	rItem[0] = &Obj.pntInventory[10];
	rItem[1] = &Obj.pntInventory[11];
	rItem[2] = &Obj.pntInventory[9];

#define GET_MAX_RESISTANCE(x,y,z) ( ( ( ( (x) > (y) ) ? (x) : (y) ) > (z) ) ? ( ( (x) > (y) ) ? (x) : (y) ) : (z) )	

	Obj.m_Resistance[1] = GET_MAX_RESISTANCE(rItem[0]->m_Resistance[1], rItem[1]->m_Resistance[1], rItem[2]->m_Resistance[1]);
	Obj.m_Resistance[0] = GET_MAX_RESISTANCE(rItem[0]->m_Resistance[0], rItem[1]->m_Resistance[0], rItem[2]->m_Resistance[0]);
	Obj.m_Resistance[2] = GET_MAX_RESISTANCE(rItem[0]->m_Resistance[2], rItem[1]->m_Resistance[2], rItem[2]->m_Resistance[2]);
	Obj.m_Resistance[3] = GET_MAX_RESISTANCE(rItem[0]->m_Resistance[3], rItem[1]->m_Resistance[3], rItem[2]->m_Resistance[3]);
	Obj.m_Resistance[4] = GET_MAX_RESISTANCE(rItem[0]->m_Resistance[4], rItem[1]->m_Resistance[4], rItem[2]->m_Resistance[4]);
	Obj.m_Resistance[5] = GET_MAX_RESISTANCE(rItem[0]->m_Resistance[5], rItem[1]->m_Resistance[5], rItem[2]->m_Resistance[5]);
	Obj.m_Resistance[6] = GET_MAX_RESISTANCE(rItem[0]->m_Resistance[6], rItem[1]->m_Resistance[6], rItem[2]->m_Resistance[6]);

	Obj.m_Resistance[R_POISON] += Obj.m_PlayerData->m_MPSkillOpt->iMpsResistancePoison;
	Obj.m_Resistance[R_LIGHTNING] += Obj.m_PlayerData->m_MPSkillOpt->iMpsResistanceThunder;
	Obj.m_Resistance[R_ICE] += Obj.m_PlayerData->m_MPSkillOpt->iMpsResistanceIce;
	
	Obj.m_PlayerData->m_Resistance_Stun = 0;
	Obj.m_PlayerData->m_Resistance_Double = 0;
	Obj.m_PlayerData->m_Resistance_Perfect = 0;
	Obj.m_PlayerData->m_Resistance_SD = 0;
	Obj.m_PlayerData->m_Resistance_Excellent = 0;
	Obj.m_PlayerData->m_Resistance_Critical = 0;

	for (int i = 0; i < 3; i++)
	{
		switch (rItem[i]->m_Type)
		{
		case ITEMGET(13, 171):
			for (int j = 0; j < 6; j++)
			{
				if (rItem[i]->m_Type == ITEMGET(g_ItemOptionTypeMng.m_AccessoryItemOptionValue[j].ItemType, g_ItemOptionTypeMng.m_AccessoryItemOptionValue[j].ItemIndex))
				{
					Obj.m_PlayerData->m_Resistance_Perfect += g_ItemOptionTypeMng.m_AccessoryItemOptionValue[j].LevelValue[rItem[i]->m_Level];

					if (rItem[i]->IsExtItem() == TRUE)
					{
						Obj.m_PlayerData->m_Resistance_Perfect += 2;
					}
				}
			}
			break;
		case ITEMGET(13, 172):
			for (int j = 0; j < 6; j++)
			{
				if (rItem[i]->m_Type == ITEMGET(g_ItemOptionTypeMng.m_AccessoryItemOptionValue[j].ItemType, g_ItemOptionTypeMng.m_AccessoryItemOptionValue[j].ItemIndex))
				{
					Obj.m_PlayerData->m_Resistance_SD += g_ItemOptionTypeMng.m_AccessoryItemOptionValue[j].LevelValue[rItem[i]->m_Level];

					if (rItem[i]->IsExtItem() == TRUE)
					{
						Obj.m_PlayerData->m_Resistance_SD += 2;
					}
				}
			}
			break;
		case ITEMGET(13, 173):
			for (int j = 0; j < 6; j++)
			{
				if (rItem[i]->m_Type == ITEMGET(g_ItemOptionTypeMng.m_AccessoryItemOptionValue[j].ItemType, g_ItemOptionTypeMng.m_AccessoryItemOptionValue[j].ItemIndex))
				{
					Obj.m_PlayerData->m_Resistance_Double += g_ItemOptionTypeMng.m_AccessoryItemOptionValue[j].LevelValue[rItem[i]->m_Level];

					if (rItem[i]->IsExtItem() == TRUE)
					{
						Obj.m_PlayerData->m_Resistance_Double += 2;
					}
				}
			}
			break;
		case ITEMGET(13, 174):
			for (int j = 0; j < 6; j++)
			{
				if (rItem[i]->m_Type == ITEMGET(g_ItemOptionTypeMng.m_AccessoryItemOptionValue[j].ItemType, g_ItemOptionTypeMng.m_AccessoryItemOptionValue[j].ItemIndex))
				{
					Obj.m_PlayerData->m_Resistance_Stun += g_ItemOptionTypeMng.m_AccessoryItemOptionValue[j].LevelValue[rItem[i]->m_Level];

					if (rItem[i]->IsExtItem() == TRUE)
					{
						if (rItem[i]->m_Level)
						{
							Obj.m_PlayerData->m_Resistance_Stun += 2;
						}
						else
						{
							Obj.m_PlayerData->m_Resistance_Stun++;
						}
					}
				}
			}
			break;
		case ITEMGET(13, 175):
			for (int j = 0; j < 6; j++)
			{
				if (rItem[i]->m_Type == ITEMGET(g_ItemOptionTypeMng.m_AccessoryItemOptionValue[j].ItemType, g_ItemOptionTypeMng.m_AccessoryItemOptionValue[j].ItemIndex))
				{
					Obj.m_PlayerData->m_Resistance_Excellent += g_ItemOptionTypeMng.m_AccessoryItemOptionValue[j].LevelValue[rItem[i]->m_Level];

					if (rItem[i]->IsExtItem() == TRUE)
					{
						Obj.m_PlayerData->m_Resistance_Excellent += 2;
					}
				}
			}
			break;
		case ITEMGET(13, 176):
			for (int j = 0; j < 6; j++)
			{
				if (rItem[i]->m_Type == ITEMGET(g_ItemOptionTypeMng.m_AccessoryItemOptionValue[j].ItemType, g_ItemOptionTypeMng.m_AccessoryItemOptionValue[j].ItemIndex))
				{
					Obj.m_PlayerData->m_Resistance_Critical += g_ItemOptionTypeMng.m_AccessoryItemOptionValue[j].LevelValue[rItem[i]->m_Level];

					if (rItem[i]->IsExtItem() == TRUE)
					{
						Obj.m_PlayerData->m_Resistance_Critical += 2;
					}
				}
			}
			break;
		}
	}

	g_kItemSystemFor380.ApplyFor380Option(Obj.;
	g_kJewelOfHarmonySystem.SetApplyStrengthenItem(Obj.;
	g_SocketOptionSystem.SetApplySocketEffect(Obj.;
	gObjInventoryEquipment(Obj.;

	Obj.AddLife += Obj.m_PlayerData->m_MPSkillOpt->iMpsMaxHP;
	Obj.AddBP += Obj.m_PlayerData->m_MPSkillOpt->iMpsMaxBP;
	Obj.iAddShield += Obj.m_PlayerData->m_MPSkillOpt->iMpsMaxSD;
	Obj.AddMana += Obj.m_PlayerData->m_MPSkillOpt->iMpsMaxMana;
	
	Obj.AddMana += Obj.MaxMana * Obj.m_PlayerData->m_MPSkillOpt->iMpsMaxManaRate / 100.0;

	if (gObjCheckUsedBuffEffect(Obj. BUFFTYPE_HP_INC_MAS) == true)
	{
		Obj.AddBP += Obj.MaxBP * Obj.m_PlayerData->m_MPSkillOpt->iMpsIncMaxBP / 100.0;
	}

	if (gObjCheckUsedBuffEffect(Obj. BUFFTYPE_MELEE_DEFENSE_INC_STR) == true)
	{
		Obj.AddMana += Obj.MaxMana * Obj.m_PlayerData->m_MPSkillOpt->iMpsMasManaRate_Wizard / 100.0;
	}

	g_ItemOptionTypeMng.CalcExcOptionEffect(Obj.;
	g_ItemOptionTypeMng.CalcWingOptionEffect(Obj.;
	this->SetItemApply(Obj.;
	this->PremiumItemApply(Obj.;
	gObjNextExpCal(Obj.;

	if (gObjCheckUsedBuffEffect(Obj. BUFFTYPE_BERSERKER_PRO) == TRUE)
	{

		Obj.m_AttackDamageMinLeft += Obj.m_PlayerData->m_MPSkillOpt->iMpsIncValueBerserker3;
		Obj.m_AttackDamageMaxLeft += Obj.m_PlayerData->m_MPSkillOpt->iMpsIncValueBerserker3;
		Obj.m_AttackDamageMinRight += Obj.m_PlayerData->m_MPSkillOpt->iMpsIncValueBerserker3;
		Obj.m_AttackDamageMaxRight += Obj.m_PlayerData->m_MPSkillOpt->iMpsIncValueBerserker3;
		Obj.m_MagicDamageMin += Obj.m_PlayerData->m_MPSkillOpt->iMpsIncValueBerserker3;
		Obj.m_MagicDamageMax += Obj.m_PlayerData->m_MPSkillOpt->iMpsIncValueBerserker3;
		Obj.m_CurseDamageMin += Obj.m_PlayerData->m_MPSkillOpt->iMpsIncValueBerserker3;
		Obj.m_CurseDamageMax += Obj.m_PlayerData->m_MPSkillOpt->iMpsIncValueBerserker3;
	}
	
	if (Right->m_Type >= ITEMGET(4, 0) && Right->m_Type < ITEMGET(5, 0) && Right->m_Part == 0)
	{
		if (Left->m_Type == ITEMGET(4, 7) && Left->m_Level > 0)
		{
			Obj.m_AttackDamageMinLeft += ((Obj.m_AttackDamageMinLeft*((Left->m_Level * 2) + 1)) / 100) + 1;
			Obj.m_AttackDamageMaxLeft += ((Obj.m_AttackDamageMaxLeft*((Left->m_Level * 2) + 1)) / 100) + 1;
		}
	}
	else if (Right->m_Type >= ITEMGET(4, 0) && Right->m_Type < ITEMGET(5, 0) && Right->m_Part == 0)
	{
		if (Left->m_Type == ITEMGET(4, 15) && Left->m_Level > 0)
		{
			Obj.m_AttackDamageMinLeft += ((Obj.m_AttackDamageMinLeft*((Right->m_Level * 2) + 1)) / 100) + 1;
			Obj.m_AttackDamageMaxLeft += ((Obj.m_AttackDamageMinLeft*((Right->m_Level * 2) + 1)) / 100) + 1;
		}
	}

	if (Obj.Class == CLASS_KNIGHT || Obj.Class == CLASS_MAGUMSA || Obj.Class == CLASS_DARKLORD)
	{
		if (Right->m_Type != -1 && Left->m_Type != -1)
		{
			if (Right->m_Type >= ITEMGET(0, 0) && Right->m_Type < ITEMGET(4, 0) && Left->m_Type >= ITEMGET(0, 0) && Left->m_Type < ITEMGET(4, 0))
			{
				if (Right->m_Type == Left->m_Type && (Obj.Class == CLASS_KNIGHT || Obj.Class == CLASS_MAGUMSA))
				{
					this->m_Lua.Generic_Call("CalcTwoSameWeaponBonus", "iiii>iiii", Obj.m_AttackDamageMinLeft, Obj.m_AttackDamageMaxLeft,
						Obj.m_AttackDamageMinRight, Obj.m_AttackDamageMaxRight,
						&Obj.m_AttackDamageMinLeft, &Obj.m_AttackDamageMinRight,
						&Obj.m_AttackDamageMaxLeft, &Obj.m_AttackDamageMaxRight);
				}

				else
				{
					this->m_Lua.Generic_Call("CalcTwoDifferentWeaponBonus", "iiii>iiii", Obj.m_AttackDamageMinLeft, Obj.m_AttackDamageMaxLeft,
						Obj.m_AttackDamageMinRight, Obj.m_AttackDamageMaxRight,
						&Obj.m_AttackDamageMinLeft, &Obj.m_AttackDamageMinRight,
						&Obj.m_AttackDamageMaxLeft, &Obj.m_AttackDamageMaxRight);
				}
			}
		}
	}

	if (Obj.Class == CLASS_RAGEFIGHTER)
	{
		if (Right->m_Type != -1 && Left->m_Type != -1)
		{
			if (Right->m_Type >= ITEMGET(0, 0) && Right->m_Type < ITEMGET(4, 0) && Left->m_Type >= ITEMGET(0, 0) && Left->m_Type < ITEMGET(4, 0))
			{
				this->m_Lua.Generic_Call("CalcRageFighterTwoWeaponBonus", "iiii>iiii", Obj.m_AttackDamageMinLeft, Obj.m_AttackDamageMaxLeft,
					Obj.m_AttackDamageMinRight, Obj.m_AttackDamageMaxRight,
					&Obj.m_AttackDamageMinLeft, &Obj.m_AttackDamageMinRight,
					&Obj.m_AttackDamageMaxLeft, &Obj.m_AttackDamageMaxRight);
			}
		}
	}

	if (gObjDemonSprite(Obj. == TRUE)
	{
		Obj.m_AttackDamageMinRight += Obj.m_AttackDamageMinRight * g_ConfigRead.pet.DemonAddDamage / 100;
		Obj.m_AttackDamageMaxRight += Obj.m_AttackDamageMaxRight * g_ConfigRead.pet.DemonAddDamage / 100;
		Obj.m_AttackDamageMinLeft += Obj.m_AttackDamageMinLeft * g_ConfigRead.pet.DemonAddDamage / 100;
		Obj.m_AttackDamageMaxLeft += Obj.m_AttackDamageMaxLeft * g_ConfigRead.pet.DemonAddDamage / 100;
		Obj.m_MagicDamageMin += Obj.m_MagicDamageMin * g_ConfigRead.pet.DemonAddDamage / 100;
		Obj.m_MagicDamageMax += Obj.m_MagicDamageMax * g_ConfigRead.pet.DemonAddDamage / 100;
		Obj.m_CurseDamageMin += Obj.m_CurseDamageMin * g_ConfigRead.pet.DemonAddDamage / 100;
		Obj.m_CurseDamageMax += Obj.m_CurseDamageMax * g_ConfigRead.pet.DemonAddDamage / 100;
		Obj.m_AttackSpeed += g_ConfigRead.pet.DemonAddAttackSpeed;
		Obj.m_MagicSpeed += g_ConfigRead.pet.DemonAddAttackSpeed;
	}

	else if (gObjSafeGuardSprite(Obj. == TRUE)
	{
		Obj.AddLife += g_ConfigRead.pet.SafeGuardAddHP;
	}

	else if (Obj.pntInventory[8]->IsItem() == TRUE && Obj.pntInventory[8]->m_Type == ITEMGET(13, 80) &&
		Obj.pntInventory[8]->m_Durability > 0.0)
	{
		Obj.m_Defense += g_ConfigRead.pet.PandaPetAddDefense;
		Obj.m_MagicDefense += g_ConfigRead.pet.PandaPetAddDefense;
	}

	g_BuffEffect.SetNextEffect(Obj.;

	this->CalcMLSkillItemOption(Obj.;
	this->CalcShieldPoint(Obj.;

	if (Obj.iShield > (Obj.iMaxShield + Obj.iAddShield))
	{
		Obj.iShield = Obj.iMaxShield + Obj.iAddShield;
		gGameProtocol.GCReFillSend(Obj.m_Index, Obj.Life, 0xFF, 0, Obj.iShield);
	}

	int nEffectValue = 0;
	int iTotalMaxShield = Obj.iMaxShield + Obj.iAddShield;
	int iTotalMaxMana = Obj.MaxMana + Obj.AddMana;
	int iTotalMaxLife = Obj.MaxLife + Obj.AddLife;

	if (gObjGetValueOfBuffIndex(Obj. BUFFTYPE_ACHERON_FIRE, &nEffectValue, 0))
	{
		iTotalMaxShield /= 2.0;
		Obj.iAddShield -= iTotalMaxShield;

		if (Obj.iShield > (Obj.iMaxShield + Obj.iAddShield))
		{
			Obj.iShield = Obj.iMaxShield + Obj.iAddShield;
			gGameProtocol.GCReFillSend(Obj.m_Index, Obj.Life, 0xFF, 0, Obj.iShield);
		}
	}

	if (gObjGetValueOfBuffIndex(Obj. BUFFTYPE_ACHERON_FROST, &nEffectValue, 0))
	{
		iTotalMaxMana /= 2.0;
		Obj.AddMana -= iTotalMaxMana;

		if ((Obj.MaxMana + Obj.AddMana) < Obj.Mana)
		{
			Obj.Mana = Obj.MaxMana + Obj.AddMana;
			gGameProtocol.GCManaSend(Obj.m_Index, Obj.Mana, 0xFF, 0, Obj.BP);
		}
	}

	if (gObjGetValueOfBuffIndex(Obj. BUFFTYPE_ACHERON_BIND, &nEffectValue, 0))
	{
		iTotalMaxLife /= 2.0;
		Obj.AddLife -= iTotalMaxLife;

		if ((Obj.MaxLife + Obj.AddLife) < Obj.Life)
		{
			Obj.Life = Obj.MaxLife + Obj.AddLife;
			gGameProtocol.GCReFillSend(Obj.m_Index, Obj.Life, 0xFF, 0, Obj.iShield);
		}
	}

	gGameProtocol.GCReFillSend(Obj.m_Index, iTotalMaxLife, 0xFE, 0, iTotalMaxShield);
	gGameProtocol.GCManaSend(Obj.m_Index, iTotalMaxMana, 0xFE, 0, Obj.MaxBP + Obj.AddBP);
	g_StatSpec.SendOptionList(Obj.;
	gGameProtocol.GCSendAttackSpeed(Obj.m_Index);

#ifdef ENABLE_CUSTOM_ITEMOPTION
	gItemOption.CalCItemObjectCommonOption(Obj. 0);
#endif

	/*PMSG_ADDSTATS_RESULT pAddStats;
	PHeadSubSetB((BYTE*)&pAddStats, 0xEC, 0x25, sizeof(pAddStats));
	pAddStats.str = Obj.m_PlayerData->Strength;
	pAddStats.stradd = Obj.AddStrength;
	pAddStats.agi = Obj.m_PlayerData->Dexterity;
	pAddStats.agiadd = Obj.AddDexterity;
	pAddStats.vit = Obj.m_PlayerData->Vitality;
	pAddStats.vitadd = Obj.AddVitality;
	pAddStats.ene = Obj.m_PlayerData->Energy;
	pAddStats.eneadd = Obj.AddEnergy;
	pAddStats.cmd = Obj.Leadership;
	pAddStats.cmdadd = Obj.AddLeadership;
	IOCP.DataSend(Obj.m_Index, (BYTE*)&pAddStats, pAddStats.h.size); */

	gGameProtocol.GCPlayerStatsPanelNew(Obj.m_Index);
	gGameProtocol.GCPlayerStatsPanelRates(Obj.m_Index);

	for (int n = 0; n < INVENTORY_SIZE; n++)
	{
		if (Obj.pntInventory[n]->IsPeriodItemExpire() == 1)
		{
			Obj.pntInventory[n]->SetPeriodItemExpire();
			g_PeriodItemEx.RemovePeriodItemDataByForce(Obj. Obj.pntInventory[n]->m_Type, Obj.pntInventory[n]->m_Number);
			Obj.pntInventory[n]->Clear();
		}
	}
}

void CObjCalCharacter::CalcSetItemStat(CGameObject &Obj)
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	for (int i = 0; i < INVETORY_WEAR_SIZE; i++)
	{
		//if (Obj.pntInventory[i]->IsSetItem() && Obj.pntInventory[i]->m_IsValidItem && Obj.pntInventory[i]->m_Durability != 0.0f && IsBloodAngelItem(Obj.pntInventory[i]->m_Type))
		//{
			//if (Obj.pntInventory[i]->m_SetAddStat)
		//	sLog->outBasic("Obj.pntInventory[i]->m_SetAddStat %d Obj.pntInventory[i]->m_SetOption %d", Obj.pntInventory[i]->m_SetAddStat, Obj.pntInventory[i]->m_SetOption);
		//}
		if (Obj.pntInventory[i]->IsSetItem() && Obj.pntInventory[i]->m_IsValidItem && Obj.pntInventory[i]->m_Durability != 0.0f && IsBloodAngelItem(Obj.pntInventory[i]->m_Type))
		{
			int AddStatType = Obj.pntInventory[i]->GetAddStatType();

			switch (AddStatType)
			{
			case 1:
				Obj.pntInventory[i]->SetItemPlusSpecialStat(&Obj.AddStrength, 196);
				break;
			case 2:
				EnterCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);
				Obj.m_PlayerData->AgilityCheckDelay = GetTickCount();
				Obj.pntInventory[i]->SetItemPlusSpecialStat(&Obj.AddDexterity, 197);
				LeaveCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);
				break;
			case 3:
				Obj.pntInventory[i]->SetItemPlusSpecialStat(&Obj.AddEnergy, 198);
				break;
			case 4:
				Obj.pntInventory[i]->SetItemPlusSpecialStat(&Obj.AddVitality, 199);
				break;
			case 5:
				Obj.pntInventory[i]->SetItemPlusSpecialStat(&Obj.AddLeadership, 200);
				break;
			case 6:
				Obj.pntInventory[i]->SetItemPlusSpecialStat(&Obj.AddStrength, 201);

				EnterCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);
				Obj.m_PlayerData->AgilityCheckDelay = GetTickCount();
				Obj.pntInventory[i]->SetItemPlusSpecialStat(&Obj.AddDexterity, 201);
				LeaveCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);

				Obj.pntInventory[i]->SetItemPlusSpecialStat(&Obj.AddEnergy, 201);
				Obj.pntInventory[i]->SetItemPlusSpecialStat(&Obj.AddVitality, 201);

				if (Obj.Class == CLASS_DARKLORD)
				{
					Obj.pntInventory[i]->SetItemPlusSpecialStat(&Obj.AddLeadership, 201);
				}
				break;
			}

		}
	}
}



void CObjCalCharacter::GetSetItemOption(CGameObject &Obj, BYTE* pSetOptionTable, BYTE* pSetOptionCountTable, int * pSetOptionCount)
{
	*pSetOptionCount = 0;
	int RightSetOptionIndex = -1;
	int RightRingSetOptionIndex = -1;

	for (int i = 0; i < INVETORY_WEAR_SIZE; i++)
	{
		//1.01.00 fix
		if (Obj.pntInventory[i]->m_Durability <= 0.0)
		{
			continue;
		}

		int iSetItemType = Obj.pntInventory[i]->IsSetItem();

		if (iSetItemType)
		{
			if (Obj.pntInventory[i]->m_IsValidItem)
			{
				int iSetItemOption = gSetItemOption.GetOptionTableIndex(Obj.pntInventory[i]->m_Type, iSetItemType);

				if (iSetItemOption != -1)
				{
					if (i == 0)
					{
						RightSetOptionIndex = iSetItemOption;
					}
					else if (i == 1)
					{
						if (RightSetOptionIndex == iSetItemOption)
						{
							if (Obj.pntInventory[0]->GetWeaponType() == Obj.pntInventory[1]->GetWeaponType() && Obj.pntInventory[0]->IsSetItem())
							{
								continue;
							}
						}
					}

					if (i == 10)
					{
						RightRingSetOptionIndex = iSetItemOption;
					}
					else if (i == 11)
					{
						if (Obj.pntInventory[10]->m_Type == Obj.pntInventory[11]->m_Type && Obj.pntInventory[10]->IsSetItem())
						{
							continue;
						}
					}

					BOOL bFound = FALSE;

					for (int n = 0; n < *pSetOptionCount; n++)
					{
						if (pSetOptionTable[n] == iSetItemOption)
						{
							pSetOptionCountTable[n]++;
							bFound = TRUE;

							break;
						}
					}

					if (bFound == FALSE)
					{
						pSetOptionTable[*pSetOptionCount] = iSetItemOption;
						pSetOptionCountTable[*pSetOptionCount]++;

						*pSetOptionCount += 1;
					}
				}
			}
		}
	}
}
void CObjCalCharacter::CalcSetItemOption(CGameObject &Obj)
{
	BYTE SetOptionTable[29];
	BYTE SetOptionCountTable[29];
	int SetOptionCount = 0;
	int op1;
	int op2;
	int op3;
	int op4;
	int op5;
	int opvalue1;
	int opvalue2;
	int opvalue3;
	int opvalue4;
	int opvalue5;

	memset(SetOptionTable, 0, sizeof(SetOptionTable));
	memset(SetOptionCountTable, 0, sizeof(SetOptionCountTable));
	this->GetSetItemOption(lpObj, SetOptionTable, SetOptionCountTable, &SetOptionCount);

	for (int optioncount = 0; optioncount < SetOptionCount; optioncount++)
	{
		int OptionTableIndex = SetOptionTable[optioncount];
		int OptionTableCount = SetOptionCountTable[optioncount];

		if (OptionTableCount >= 2)
		{
			gSetItemOption.IsRequireClass(OptionTableIndex, Obj.Class, Obj.m_PlayerData->ChangeUP);

			for (int tablecnt = 0; tablecnt < (OptionTableCount - 1); tablecnt++)
			{
				if (gSetItemOption.GetSetOption(OptionTableIndex, tablecnt,
					op1, op2, opvalue1, opvalue2, Obj.Class, Obj.m_PlayerData->ChangeUP))
				{
					this->SetItemStatPlusSpecial(lpObj, op1, opvalue1);
					this->SetItemStatPlusSpecial(lpObj, op2, opvalue2);
				}
			}

			if (gSetItemOption.GetMaxSetOptionCount(OptionTableIndex) < OptionTableCount)
			{
				Obj.m_PlayerData->IsFullSetItem = true;

				gSetItemOption.GetGetFullSetOption(OptionTableIndex,
					op1, op2, op3, op4, op5,
					opvalue1, opvalue2, opvalue3, opvalue4, opvalue5,
					Obj.Class, Obj.m_PlayerData->ChangeUP);

				this->SetItemStatPlusSpecial(lpObj, op1, opvalue1);
				this->SetItemStatPlusSpecial(lpObj, op2, opvalue2);
				this->SetItemStatPlusSpecial(lpObj, op3, opvalue3);
				this->SetItemStatPlusSpecial(lpObj, op4, opvalue4);
				this->SetItemStatPlusSpecial(lpObj, op5, opvalue5);
			}
		}
	}

	for (int optioncount = 0; optioncount < SetOptionCount; optioncount++)
	{
		int OptionTableIndex = SetOptionTable[optioncount];
		int OptionTableCount = SetOptionCountTable[optioncount];

		if (OptionTableCount >= 2)
		{
			for (int tablecnt = 0; tablecnt < (OptionTableCount - 1); tablecnt++)
			{
				if (gSetItemOption.GetSetOption(OptionTableIndex, tablecnt,
					op1, op2, opvalue1, opvalue2, Obj.Class, Obj.m_PlayerData->ChangeUP))
				{
					this->SetItemPlusSpecial(lpObj, op1, opvalue1);
					this->SetItemPlusSpecial(lpObj, op2, opvalue2);
				}
			}

			if (gSetItemOption.GetMaxSetOptionCount(OptionTableIndex) < OptionTableCount)
			{
				Obj.m_PlayerData->IsFullSetItem = true;

				gSetItemOption.GetGetFullSetOption(OptionTableIndex,
					op1, op2, op3, op4, op5,
					opvalue1, opvalue2, opvalue3, opvalue4, opvalue5,
					Obj.Class, Obj.m_PlayerData->ChangeUP);

				this->SetItemPlusSpecial(lpObj, op1, opvalue1);
				this->SetItemPlusSpecial(lpObj, op2, opvalue2);
				this->SetItemPlusSpecial(lpObj, op3, opvalue3);
				this->SetItemPlusSpecial(lpObj, op4, opvalue4);
				this->SetItemPlusSpecial(lpObj, op5, opvalue5);
			}
		}
	}
}



void CObjCalCharacter::SetItemStatPlusSpecial(CGameObject &Obj, int option, int ivalue)
{
	if (Obj.Type != OBJ_USER)
		return;

	if (option == -1)
		return;

	switch (option)
	{
	case AT_SET_OPTION_IMPROVE_STRENGTH:
		Obj.AddStrength += ivalue;
		break;
	case AT_SET_OPTION_IMPROVE_DEXTERITY:
		EnterCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);
		Obj.m_PlayerData->AgilityCheckDelay = GetTickCount();
		Obj.AddDexterity += ivalue;
		LeaveCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);
		break;
	case AT_SET_OPTION_IMPROVE_ENERGY:
		Obj.AddEnergy += ivalue;
		break;
	case AT_SET_OPTION_IMPROVE_VITALITY:
		Obj.AddVitality += ivalue;
		break;
	case AT_SET_OPTION_IMPROVE_LEADERSHIP:
		Obj.AddLeadership += ivalue;
		break;
	case AT_SET_OPTION_IMPROVE_ALL_STATS:
		Obj.AddStrength += ivalue;
		EnterCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);
		Obj.m_PlayerData->AgilityCheckDelay = GetTickCount();
		Obj.AddDexterity += ivalue;
		LeaveCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);
		Obj.AddEnergy += ivalue;
		Obj.AddVitality += ivalue;

		if (Obj.Class == CLASS_DARKLORD)
		{
			Obj.AddLeadership += ivalue;
		}
		break;
	}
}


void CObjCalCharacter::SetItemPlusSpecial(CGameObject &Obj, int option, int ivalue)
{
	if (option == -1)
		return;
	if (ivalue == 0)
	{
		return;
	}

	switch (option)
	{
	case AT_SET_OPTION_DOUBLE_DAMAGE:
		Obj.m_PlayerData->SetOpDoubleDamage += ivalue;
		break;
	case AT_SET_OPTION_IMPROVE_SHIELD_DEFENCE:
		Obj.m_PlayerData->SetOpImproveSheldDefence += ivalue;
		break;
	case AT_SET_OPTION_TWO_HAND_SWORD_IMPROVE_DAMAGE:
		Obj.m_PlayerData->SetOpTwoHandSwordImproveDamage += ivalue;
		break;
	case AT_SET_OPTION_IMPROVE_MINATTACKDAMAGE:
		Obj.m_PlayerData->SetOpAddMinAttackDamage += ivalue;
		break;
	case AT_SET_OPTION_IMPROVE_MAXATTACKDAMAGE:
		Obj.m_PlayerData->SetOpAddMaxAttackDamage += ivalue;
		break;
	case AT_SET_OPTION_IMPROVE_MAGICDAMAGE:
		Obj.m_PlayerData->SetOpAddMagicPower += ivalue;
		break;
	case AT_SET_OPTION_IMPROVE_DAMAGE:
		Obj.m_PlayerData->SetOpAddDamage += ivalue;
		break;
	case AT_SET_OPTION_IMPROVE_ATTACKRATE:
		Obj.m_PlayerData->SetOpImproveSuccessAttackRate += ivalue;
		break;
	case AT_SET_OPTION_IMPROVE_DEFENCE:
		Obj.m_PlayerData->SetOpAddDefence += ivalue;
		break;
	case AT_SET_OPTION_DEFENCE_IGNORE:
		Obj.m_PlayerData->SetOpIgnoreDefense += ivalue;
		break;
	case AT_SET_OPTION_IMPROVE_MAXLIFE:
		Obj.AddLife += ivalue;
		break;
	case AT_SET_OPTION_IMPROVE_MAXMANA:
		Obj.AddMana += ivalue;
		break;
	case AT_SET_OPTION_IMPROVE_MAXAG:
		Obj.AddBP += ivalue;
		break;
	case AT_SET_OPTION_IMPROVE_AG_VALUE:
		Obj.m_PlayerData->SetOpIncAGValue += ivalue;
		break;
	case AT_SET_OPTION_IMPROVE_CRITICALDAMAGE:
		Obj.m_PlayerData->SetOpAddCriticalDamage += ivalue;
		break;
	case AT_SET_OPTION_IMPROVE_CRITICALDAMAGE_SUCCESS:
		Obj.m_PlayerData->SetOpAddCriticalDamageSuccessRate += ivalue;
		break;
	case AT_SET_OPTION_IMPROVE_EX_DAMAGE:
		Obj.m_PlayerData->SetOpAddExDamage += ivalue;
		break;
	case AT_SET_OPTION_IMPROVE_EX_DAMAGE_SUCCESS:
		Obj.m_PlayerData->SetOpAddExDamageSuccessRate += ivalue;
		break;
	case AT_SET_OPTION_IMPROVE_SKILLDAMAGE:
		Obj.m_PlayerData->SetOpAddSkillAttack += ivalue;
		break;
	case AT_SET_OPTION_IMPROVE_BASE_DEFENSE:
		Obj.m_BaseDefense += ivalue;
		break;
	case AT_SET_OPTION_IMPROVE_TOTAL_ATTACKDAMAGE:
		Obj.m_PlayerData->SetOpAddMinAttackDamage += ivalue;
		Obj.m_PlayerData->SetOpAddMaxAttackDamage += ivalue;
		break;
	case AT_SET_OPTION_IMPROVE_TOTAL_MAGICDAMAGE:
		Obj.m_PlayerData->SetOpAddMinMagicDamage += ivalue;
		Obj.m_PlayerData->SetOpAddMaxMagicDamage += ivalue;
		break;
	case AT_SET_OPTION_IMPROVE_MAGICDAMAGE_WITH_ENERGY:
		Obj.m_PlayerData->SetOpAddMagicPower += (Obj.m_PlayerData->Energy + Obj.AddEnergy) / ivalue;
		break;
	case AT_SET_OPTION_FIRE_MASTERY:
		Obj.m_AddResistance[R_FIRE] += ivalue;
		break;
	case AT_SET_OPTION_ICE_MASTERY:
		Obj.m_AddResistance[R_ICE] += ivalue;
		break;
	case AT_SET_OPTION_THUNDER_MASTERY:
		Obj.m_AddResistance[R_LIGHTNING] += ivalue;
		break;
	case AT_SET_OPTION_POSION_MASTERY:
		Obj.m_AddResistance[R_POISON] += ivalue;
		break;
	case AT_SET_OPTION_WATER_MASTERY:
		Obj.m_AddResistance[R_WATER] += ivalue;
		break;
	case AT_SET_OPTION_WIND_MASTERY:
		Obj.m_AddResistance[R_WIND] += ivalue;
		break;
	case AT_SET_OPTION_EARTH_MASTERY:
		Obj.m_AddResistance[R_EARTH] += ivalue;
		break;
	}
}



void CObjCalCharacter::SetItemApply(CGameObject &Obj)
{
	Obj.m_AttackDamageMinLeft += Obj.m_PlayerData->SetOpAddAttackDamage;
	Obj.m_AttackDamageMaxLeft += Obj.m_PlayerData->SetOpAddAttackDamage;
	Obj.m_AttackDamageMinRight += Obj.m_PlayerData->SetOpAddAttackDamage;
	Obj.m_AttackDamageMaxRight += Obj.m_PlayerData->SetOpAddAttackDamage;

	Obj.m_AttackDamageMinLeft += Obj.m_PlayerData->SetOpAddMinAttackDamage;
	Obj.m_AttackDamageMinRight += Obj.m_PlayerData->SetOpAddMinAttackDamage;

	Obj.m_AttackDamageMaxLeft += Obj.m_PlayerData->SetOpAddMaxAttackDamage;
	Obj.m_AttackDamageMaxRight += Obj.m_PlayerData->SetOpAddMaxAttackDamage;

	Obj.m_MagicDamageMin += Obj.m_MagicDamageMin * Obj.m_PlayerData->SetOpAddMagicPower / 100;
	Obj.m_MagicDamageMax += Obj.m_MagicDamageMax * Obj.m_PlayerData->SetOpAddMagicPower / 100;

	Obj.m_MagicDamageMin += Obj.m_PlayerData->SetOpAddMinMagicDamage;
	Obj.m_MagicDamageMax += Obj.m_PlayerData->SetOpAddMaxMagicDamage;

	Obj.AddLife += INT(Obj.AddVitality * DCInfo.DefClass[Obj.Class].VitalityToLife);
	Obj.AddMana += INT(Obj.AddEnergy * DCInfo.DefClass[Obj.Class].EnergyToMana);

	Obj.m_CriticalDamage += Obj.m_PlayerData->SetOpAddCriticalDamageSuccessRate;
	Obj.m_ExcelentDamage += Obj.m_PlayerData->SetOpAddExDamageSuccessRate;
	if (Obj.pntInventory[10]->IsSetItem())
	{
		Obj.pntInventory[10]->PlusSpecialSetRing((BYTE*)Obj.m_AddResistance);
	}

	if (Obj.pntInventory[11]->IsSetItem())
	{
		Obj.pntInventory[11]->PlusSpecialSetRing((BYTE*)Obj.m_AddResistance);
	}

	if (Obj.pntInventory[9]->IsSetItem())
	{
		Obj.pntInventory[9]->PlusSpecialSetRing((BYTE*)Obj.m_AddResistance);
	}

	if (Obj.m_PlayerData->SetOpTwoHandSwordImproveDamage)
	{
		if (!Obj.pntInventory[1]->IsItem() && !Obj.pntInventory[0]->IsItem())
		{
			Obj.m_PlayerData->SetOpTwoHandSwordImproveDamage = 0;
		}

		if (Obj.pntInventory[1]->IsItem() && Obj.pntInventory[1]->m_TwoHand == FALSE)
		{
			Obj.m_PlayerData->SetOpTwoHandSwordImproveDamage = 0;
		}

		if (Obj.pntInventory[0]->IsItem() && Obj.pntInventory[0]->m_TwoHand == FALSE)
		{
			Obj.m_PlayerData->SetOpTwoHandSwordImproveDamage = 0;
		}
	}

	if ((Obj.MaxLife + Obj.AddLife) < Obj.Life)
	{
		Obj.Life = Obj.MaxLife + Obj.AddLife;
		gGameProtocol.GCReFillSend(Obj.m_Index, Obj.Life, 0xFF, 0, Obj.iShield);
	}

	gObjSetBP(lpObj);

	if ((Obj.MaxMana + Obj.AddMana) < Obj.Mana)
	{
		Obj.Mana = Obj.MaxMana + Obj.AddMana;
		gGameProtocol.GCManaSend(Obj.m_Index, Obj.Mana, 0xFF, 0, Obj.BP);
	}

	Obj.m_Defense += Obj.m_PlayerData->SetOpAddDefence * 10 / 20;
	Obj.m_Defense += Obj.m_Defense * Obj.m_PlayerData->SetOpAddDefenceRate / 100;

	if (Obj.pntInventory[1]->m_Type >= ITEMGET(6, 0) && Obj.pntInventory[1]->m_Type < ITEMGET(7, 0))
	{
		Obj.m_Defense += Obj.m_Defense * Obj.m_PlayerData->SetOpImproveSheldDefence / 100;
	}
}

void CObjCalCharacter::CalcMLSkillItemOption(CGameObject &Obj)
{
	if (Obj.Type != OBJ_USER)
		return;

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsTwoHandSwordAddDamage > 0.0)
	{
		if (Obj.pntInventory[0]->GetDetailItemType() != ITEM_TWOHAND_SWORD && Obj.pntInventory[1]->GetDetailItemType() != ITEM_TWOHAND_SWORD)
		{
			Obj.m_PlayerData->m_MPSkillOpt->iMpsTwoHandSwordAddDamage = 0.0;
		}
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsTwoHandsPvPAttack > 0.0)
	{
		if (Obj.pntInventory[0]->GetDetailItemType() != ITEM_TWOHAND_SWORD && Obj.pntInventory[1]->GetDetailItemType() != ITEM_TWOHAND_SWORD)
		{
			Obj.m_PlayerData->m_MPSkillOpt->iMpsTwoHandsPvPAttack = 0.0;
		}
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsTwoHandsMagicPower > 0.0)
	{
		if (Obj.pntInventory[0]->GetDetailItemType() == ITEM_TWOHAND_STAFF || Obj.pntInventory[1]->GetDetailItemType() == ITEM_TWOHAND_STAFF)
		{
			Obj.m_MagicDamageMin += Obj.m_PlayerData->m_MPSkillOpt->iMpsTwoHandsMagicPower;
			Obj.m_MagicDamageMax += Obj.m_PlayerData->m_MPSkillOpt->iMpsTwoHandsMagicPower;
		}
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsStaffMagicPower > 0.0)
	{
		if (Obj.pntInventory[0]->GetDetailItemType() == ITEM_ONEHAND_STAFF || Obj.pntInventory[1]->GetDetailItemType() == ITEM_ONEHAND_STAFF)
		{
			Obj.m_MagicDamageMin += Obj.m_PlayerData->m_MPSkillOpt->iMpsStaffMagicPower;
			Obj.m_MagicDamageMax += Obj.m_PlayerData->m_MPSkillOpt->iMpsStaffMagicPower;
		}
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsAddMagicPowerStick > 0.0)
	{
		if (Obj.pntInventory[0]->GetDetailItemType() == ITEM_SUMMONER_STAFF || Obj.pntInventory[1]->GetDetailItemType() == ITEM_SUMMONER_STAFF)
		{
			Obj.m_MagicDamageMin += Obj.m_PlayerData->m_MPSkillOpt->iMpsAddMagicPowerStick;
			Obj.m_MagicDamageMax += Obj.m_PlayerData->m_MPSkillOpt->iMpsAddMagicPowerStick;
		}
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsIncShield > 0.0)
	{
		if (Obj.pntInventory[0]->GetDetailItemType() == ITEM_SHIELD || Obj.pntInventory[1]->GetDetailItemType() == ITEM_SHIELD)
		{
			Obj.m_Defense += Obj.m_PlayerData->m_MPSkillOpt->iMpsIncShield;
		}
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsElfAddShield > 0.0)
	{
		if (Obj.pntInventory[0]->GetDetailItemType() == ITEM_SHIELD || Obj.pntInventory[1]->GetDetailItemType() == ITEM_SHIELD)
		{
			Obj.m_Defense += Obj.m_PlayerData->m_MPSkillOpt->iMpsElfAddShield;
		}
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsAddShieldDefense > 0.0)
	{
		if (Obj.pntInventory[0]->GetDetailItemType() == ITEM_SHIELD || Obj.pntInventory[1]->GetDetailItemType() == ITEM_SHIELD)
		{
			Obj.m_Defense += Obj.m_PlayerData->m_MPSkillOpt->iMpsAddShieldDefense;
		}
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsStaffAttackSpeed > 0.0)
	{
		if (Obj.pntInventory[0]->GetDetailItemType() == ITEM_ONEHAND_STAFF || Obj.pntInventory[1]->GetDetailItemType() == ITEM_ONEHAND_STAFF)
		{
			Obj.m_AttackSpeed += Obj.m_PlayerData->m_MPSkillOpt->iMpsStaffAttackSpeed;
			Obj.m_MagicSpeed += Obj.m_PlayerData->m_MPSkillOpt->iMpsStaffAttackSpeed;
		}
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsAddAttackSpeedMagicBook > 0.0)
	{
		if (Obj.pntInventory[0]->GetDetailItemType() == ITEM_SUMMONER_BOOK || Obj.pntInventory[1]->GetDetailItemType() == ITEM_SUMMONER_BOOK)
		{
			Obj.m_AttackSpeed += Obj.m_PlayerData->m_MPSkillOpt->iMpsAddAttackSpeedMagicBook;
			Obj.m_MagicSpeed += Obj.m_PlayerData->m_MPSkillOpt->iMpsAddAttackSpeedMagicBook;
		}
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsAddCurseMagicBook > 0.0)
	{
		if (Obj.pntInventory[0]->GetDetailItemType() == ITEM_SUMMONER_BOOK || Obj.pntInventory[1]->GetDetailItemType() == ITEM_SUMMONER_BOOK)
		{
			Obj.m_CurseDamageMin += Obj.m_PlayerData->m_MPSkillOpt->iMpsAddCurseMagicBook;
			Obj.m_CurseDamageMax += Obj.m_PlayerData->m_MPSkillOpt->iMpsAddCurseMagicBook;
		}
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsTwoHandsStaffManAttack > 0.0)
	{
		if (Obj.pntInventory[0]->GetDetailItemType() != ITEM_TWOHAND_STAFF && Obj.pntInventory[1]->GetDetailItemType() != ITEM_TWOHAND_STAFF)
		{
			Obj.m_PlayerData->m_MPSkillOpt->iMpsTwoHandsStaffManAttack = 0.0;
		}
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsTwoHandsBowManAttack > 0.0)
	{
		if (Obj.pntInventory[0]->GetDetailItemType() != ITEM_CROSSBOW)
		{
			Obj.m_PlayerData->m_MPSkillOpt->iMpsTwoHandsBowManAttack = 0.0;
		}
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsAddManAttackStick > 0.0)
	{
		if (Obj.pntInventory[0]->GetDetailItemType() != ITEM_SUMMONER_STAFF && Obj.pntInventory[1]->GetDetailItemType() != ITEM_SUMMONER_STAFF)
		{
			Obj.m_PlayerData->m_MPSkillOpt->iMpsAddManAttackStick = 0.0;
		}
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsIncCeptorManAttack > 0.0)
	{
		if (Obj.pntInventory[0]->GetDetailItemType() != ITEM_DARKLORD_SCEPTER && Obj.pntInventory[1]->GetDetailItemType() != ITEM_DARKLORD_SCEPTER)
		{
			Obj.m_PlayerData->m_MPSkillOpt->iMpsIncCeptorManAttack = 0.0;
		}
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsShieldBlockRate > 0.0)
	{
		if (Obj.pntInventory[0]->GetDetailItemType() != ITEM_SHIELD && Obj.pntInventory[1]->GetDetailItemType() != ITEM_SHIELD)
		{
			Obj.m_PlayerData->m_MPSkillOpt->iMpsShieldBlockRate = 0.0;
		}
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsElfShieldBlockRate > 0.0)
	{
		if (Obj.pntInventory[0]->GetDetailItemType() != ITEM_SHIELD && Obj.pntInventory[1]->GetDetailItemType() != ITEM_SHIELD)
		{
			Obj.m_PlayerData->m_MPSkillOpt->iMpsElfShieldBlockRate = 0.0;
		}
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsAddShieldBlockingRage > 0.0)
	{
		if (Obj.pntInventory[0]->GetDetailItemType() != ITEM_SHIELD && Obj.pntInventory[1]->GetDetailItemType() != ITEM_SHIELD)
		{
			Obj.m_PlayerData->m_MPSkillOpt->iMpsAddShieldBlockingRage = 0.0;
		}
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsAddAttackSpeed > 0.0)
	{
		if (Obj.pntInventory[0]->GetDetailItemType() == ITEM_ONEHAND_SWORD && Obj.pntInventory[1]->GetDetailItemType() != ITEM_ONEHAND_SWORD)
		{
			Obj.m_AttackSpeed += Obj.m_PlayerData->m_MPSkillOpt->iMpsAddAttackSpeed;
			Obj.m_MagicSpeed += Obj.m_PlayerData->m_MPSkillOpt->iMpsAddAttackSpeed;
		}
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsSwordAddDamage > 0.0)
	{
		if (Obj.pntInventory[0]->GetDetailItemType() != ITEM_ONEHAND_SWORD && Obj.pntInventory[1]->GetDetailItemType() != ITEM_ONEHAND_SWORD)
		{
			Obj.m_PlayerData->m_MPSkillOpt->iMpsSwordAddDamage = 0.0;
		}
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsMaceStrength > 0.0)
	{
		if (Obj.pntInventory[0]->GetDetailItemType() != ITEM_MACE && Obj.pntInventory[1]->GetDetailItemType() != ITEM_MACE)
		{
			Obj.m_PlayerData->m_MPSkillOpt->iMpsMaceStrength = 0.0;
		}
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsAddSturn > 0.0)
	{
		if (Obj.pntInventory[0]->GetDetailItemType() != ITEM_MACE && Obj.pntInventory[1]->GetDetailItemType() != ITEM_MACE)
		{
			Obj.m_PlayerData->m_MPSkillOpt->iMpsAddSturn = 0.0;
		}
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsSpearAddDamage > 0.0)
	{
		if (Obj.pntInventory[0]->GetDetailItemType() != ITEM_SPEAR && Obj.pntInventory[1]->GetDetailItemType() != ITEM_SPEAR)
		{
			Obj.m_PlayerData->m_MPSkillOpt->iMpsSpearAddDamage = 0.0;
		}
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsAddDoubleDamage > 0.0)
	{
		if (Obj.pntInventory[0]->GetDetailItemType() != ITEM_SPEAR && Obj.pntInventory[1]->GetDetailItemType() != ITEM_SPEAR)
		{
			Obj.m_PlayerData->m_MPSkillOpt->iMpsAddDoubleDamage = 0.0;
		}
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsBowAddDamage > 0.0)
	{
		if (Obj.pntInventory[0]->GetDetailItemType() != ITEM_BOW)
		{
			Obj.m_PlayerData->m_MPSkillOpt->iMpsBowAddDamage = 0.0;
		}
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsAddCeptorDamage > 0.0)
	{
		if (Obj.pntInventory[0]->GetDetailItemType() != ITEM_DARKLORD_SCEPTER && Obj.pntInventory[1]->GetDetailItemType() != ITEM_DARKLORD_SCEPTER)
		{
			Obj.m_PlayerData->m_MPSkillOpt->iMpsAddCeptorDamage = 0.0;
		}
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsAddPetAttack > 0.0)
	{
		if (Obj.pntInventory[0]->GetDetailItemType() != ITEM_DARKLORD_SCEPTER && Obj.pntInventory[1]->GetDetailItemType() != ITEM_DARKLORD_SCEPTER)
		{
			Obj.m_PlayerData->m_MPSkillOpt->iMpsAddPetAttack = 0.0;
		}
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsAddControlAttack > 0.0)
	{
		if (Obj.pntInventory[0]->GetDetailItemType() != ITEM_DARKLORD_SCEPTER && Obj.pntInventory[1]->GetDetailItemType() != ITEM_DARKLORD_SCEPTER)
		{
			Obj.m_PlayerData->m_MPSkillOpt->iMpsAddControlAttack = 0.0;
		}
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsBowAddAttackSpeed > 0.0)
	{
		if (Obj.pntInventory[0]->GetDetailItemType() == ITEM_BOW)
		{
			Obj.m_AttackSpeed += Obj.m_PlayerData->m_MPSkillOpt->iMpsBowAddAttackSpeed;
			Obj.m_MagicSpeed += Obj.m_PlayerData->m_MPSkillOpt->iMpsBowAddAttackSpeed;
		}
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsTwoHandsBowAddDamage > 0.0)
	{
		if (Obj.pntInventory[0]->GetDetailItemType() != ITEM_CROSSBOW)
		{
			Obj.m_PlayerData->m_MPSkillOpt->iMpsTwoHandsBowAddDamage = 0.0;
		}
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsIncDarkHorseDefense > 0.0 && Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 4))
	{
		Obj.m_Defense += Obj.m_PlayerData->m_MPSkillOpt->iMpsIncDarkHorseDefense;
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsMonkWeaponDamage > 0.0)
	{
		if (Obj.pntInventory[0]->GetDetailItemType() != ITEM_RAGEFIGHTER_WEAPON && Obj.pntInventory[1]->GetDetailItemType() != ITEM_RAGEFIGHTER_WEAPON)
		{
			Obj.m_PlayerData->m_MPSkillOpt->iMpsMonkWeaponDamage = 0.0;
		}
	}

	if (Obj.pntInventory[7]->IsItem() == TRUE && Obj.pntInventory[7]->m_IsValidItem == true)
	{
		if (Obj.Class == CLASS_WIZARD || Obj.Class == CLASS_SUMMONER || Obj.Class == CLASS_MAGUMSA)
		{
			Obj.m_MagicDamageMin += Obj.m_PlayerData->m_MPSkillOpt->iMpsAddWingDamage;
			Obj.m_MagicDamageMax += Obj.m_PlayerData->m_MPSkillOpt->iMpsAddWingDamage;
		}

		if (Obj.Class == CLASS_SUMMONER)
		{
			Obj.m_CurseDamageMin += Obj.m_PlayerData->m_MPSkillOpt->iMpsAddWingDamage;
			Obj.m_CurseDamageMax += Obj.m_PlayerData->m_MPSkillOpt->iMpsAddWingDamage;
		}
	}

	if (Obj.Class == CLASS_KNIGHT || Obj.Class == CLASS_ELF || Obj.Class == CLASS_MAGUMSA || Obj.Class == CLASS_DARKLORD || Obj.Class == CLASS_RAGEFIGHTER || Obj.Class == CLASS_GROWLANCER)
	{
		if (Obj.m_PlayerData->m_MPSkillOpt->iMpsAddWingDamage > 0.0)
		{
			if (Obj.pntInventory[7]->IsItem() == FALSE || Obj.pntInventory[7]->m_IsValidItem == false)
			{
				Obj.m_PlayerData->m_MPSkillOpt->iMpsAddWingDamage = 0.0;
			}
		}
	}
}

BOOL CObjCalCharacter::ValidItem(CGameObject &Obj, CItemObject * lpItem, int pos)
{
	if (g_ConfigRead.data.common.CheckValidItem == false)
	{
		return TRUE;
	}

	LPITEM_ATTRIBUTE p = &ItemAttribute[lpItem->m_Type];

	if (p->RequireStrength != 0)
	{
		if ((Obj.m_PlayerData->Strength + Obj.AddStrength) < (lpItem->m_RequireStrength - lpItem->m_HJOpStrength))
		{
			return FALSE;
		}
	}

	if (p->RequireDexterity != 0)//
	{
		if ((Obj.m_PlayerData->Dexterity + Obj.AddDexterity) < (lpItem->m_RequireDexterity - lpItem->m_HJOpDexterity))
		{
			return FALSE;
		}
	}

	if (p->RequireEnergy != 0)
	{
		if ((Obj.m_PlayerData->Energy + Obj.AddEnergy) < lpItem->m_RequireEnergy)
		{
			return FALSE;
		}
	}

	if (p->RequireLevel != 0)
	{
		if ((Obj.Level) < lpItem->m_RequireLevel)
		{
			return FALSE;
		}
	}

	if (lpItem->m_Type >= ITEMGET(0, 0) && lpItem->m_Type <= ITEMGET(11, 0))
	{
		if (p->RequireVitality)
		{
			if ((Obj.m_PlayerData->Vitality + Obj.AddVitality) < lpItem->m_RequireVitality)
			{
				return FALSE;
			}
		}

		if (p->RequireLeadership)
		{
			if ((Obj.Leadership + Obj.AddLeadership) < lpItem->m_RequireLeaderShip)
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}


void CObjCalCharacter::CalcShieldPoint(CGameObject &Obj)
{
	int iMaxShieldPoint = 0;
	int iExpressionA = (Obj.m_PlayerData->Strength + Obj.AddStrength) + (Obj.m_PlayerData->Dexterity + Obj.AddDexterity) + (Obj.m_PlayerData->Vitality + Obj.AddVitality) + (Obj.m_PlayerData->Energy + Obj.AddEnergy);

	if (Obj.Class == CLASS_DARKLORD)
	{
		iExpressionA += Obj.Leadership + Obj.AddLeadership;
	}

	if (g_ConfigRead.g_iShieldGageConstB == 0)
		g_ConfigRead.g_iShieldGageConstB = 30;

	int Level = Obj.Level + Obj.m_PlayerData->MasterLevel;

	int iExpressionB = (Level * Level) / g_ConfigRead.g_iShieldGageConstB;

	if (g_ConfigRead.g_iShieldGageConstA == 0)
		g_ConfigRead.g_iShieldGageConstA = 12;

	iMaxShieldPoint = (iExpressionA * g_ConfigRead.g_iShieldGageConstA) / 10 + iExpressionB + Obj.m_Defense;
	Obj.iMaxShield = iMaxShieldPoint;
}

void CObjCalCharacter::PremiumItemApply(CGameObject &Obj)
{
	for (int i = 9; i < 12; ++i)
	{
		if (Obj.pntInventory[i]->m_Durability <= 0.0 || Obj.pntInventory[i]->IsPeriodItemExpire() == TRUE)
			continue;

		switch (Obj.pntInventory[i]->m_Type)
		{
		case ITEMGET(13, 109):
			Obj.AutoHPRecovery += 3;
			Obj.AddMana += (short)(Obj.MaxMana * 4 / 100.0f);
			break;
		case ITEMGET(13, 110):
			Obj.AutoHPRecovery += 3;
			Obj.AddLife += (short)(Obj.MaxLife * 4 / 100.0f);
			break;
		case ITEMGET(13, 111):
			Obj.AutoHPRecovery += 3;
			Obj.MonsterDieGetMoney += 30;
			break;
		case ITEMGET(13, 112):
			Obj.AutoHPRecovery += 3;
			Obj.DamageDecrease += 4;
			break;
		case ITEMGET(13, 113):
			Obj.AutoHPRecovery += 3;
			Obj.m_ExcelentDamage += 10;
			break;
		case ITEMGET(13, 114):
			Obj.AutoHPRecovery += 3;
			Obj.m_AttackSpeed += 7;
			Obj.m_MagicSpeed += 7;
			break;
		case ITEMGET(13, 115):
			Obj.AutoHPRecovery += 3;
			Obj.MonsterDieGetMana += 1;
			break;
		case ITEMGET(13, 128):
			Obj.m_CriticalDamage += 10;
			break;
		case ITEMGET(13, 129):
			Obj.m_ExcelentDamage += 10;
			break;
		case ITEMGET(13, 130):
			Obj.AddLife += 50;
			break;
		case ITEMGET(13, 131):
			Obj.AddMana += 50;
			break;
		case ITEMGET(13, 132):
			Obj.AddLife += 100;
			Obj.iAddShield += 500;
			break;
		case ITEMGET(13, 133):
			Obj.AddMana += 100;
			Obj.AddBP += 500;
			break;
		case ITEMGET(13, 134):
			break;
		}
	}
}



////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

