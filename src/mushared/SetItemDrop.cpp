////////////////////////////////////////////////////////////////////////////////
// SetItemDrop.cpp
#include "stdafx.h"
#include "SetItemDrop.h"
#include "user.h"
#include "SetItemOption.h"
#include "DSProtocol.h"
#include "ItemOptionTypeMng.h"
#include "configread.h"
#include "LargeRand.h"
#include "MasterLevelSkillTreeSystem.h"
#include "CustomMaxStats.h"

CSetItemDrop SetItemDrop;

CSetItemDrop::CSetItemDrop(void)
{
	this->m_dwDropUseRate = 0;
}

CSetItemDrop::~CSetItemDrop(void)
{
}

void CSetItemDrop::LoadFile(LPSTR lpFile)
{
	this->m_dwDropUseRate = 0;
	this->m_vtDropRules.clear();

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(lpFile);

	if (res.status != pugi::status_ok)
	{
		g_Log.MsgBox("%s file load fail (%s)", lpFile, res.description());
		return;
	}

	pugi::xml_node main = file.child("SetItemDropManager");

	this->m_dwDropUseRate = main.attribute("DropUseRate").as_int();

	for (pugi::xml_node rule = main.child("Rule"); rule; rule = rule.next_sibling())
	{
		SETITEM_DROP_RULE m_Rule;

		m_Rule.MonsterIndex = rule.attribute("MonsterIndex").as_int();
		m_Rule.MonsterMinLevel = rule.attribute("MonsterMinLevel").as_int();
		m_Rule.MonsterMaxLevel = rule.attribute("MonsterMaxLevel").as_int();
		m_Rule.PlayerMinLevel = rule.attribute("PlayerMinLevel").as_int();
		m_Rule.MapNumber = rule.attribute("MapNumber").as_int();

		if (strcmp(rule.attribute("PlayerMaxLevel").as_string(), "MAX") == 0)
		{
			m_Rule.PlayerMaxLevel = g_ConfigRead.data.common.UserMaxLevel + g_MaxStatsInfo.GetClass.MLUserMaxLevel;
		}
		else
		{
			m_Rule.PlayerMaxLevel = rule.attribute("PlayerMaxLevel").as_int(820);
		}

		for (pugi::xml_node itemdrop = rule.child("ItemDrop"); itemdrop; itemdrop = itemdrop.next_sibling())
		{
			SETITEM_DROP_ITEM m_Item;

			m_Item.ItemCat = itemdrop.attribute("Cat").as_int();
			m_Item.MinLevel = itemdrop.attribute("MinLevel").as_int();
			m_Item.MaxLevel = itemdrop.attribute("MaxLevel").as_int();
			m_Item.ItemCount = itemdrop.attribute("Count").as_int();
			m_Item.Skill = itemdrop.attribute("Skill").as_int();
			m_Item.Luck = itemdrop.attribute("Luck").as_int();
			m_Item.Option = itemdrop.attribute("Option").as_int();

			std::string szExcOptions = itemdrop.attribute("Exc").as_string();
			std::deque<int> exc_opt_list;
			strtk::parse(szExcOptions, ";", exc_opt_list);

			memset(m_Item.ExcOption, -1, sizeof(m_Item.ExcOption));

			for (int i = 0; i < exc_opt_list.size(); i++)
			{
				m_Item.ExcOption[i] = exc_opt_list.at(i);
			}

			m_Item.DropRate = itemdrop.attribute("DropRate").as_int();

			m_Rule.m_vtSetItems.push_back(m_Item);
		}

		this->m_vtDropRules.push_back(m_Rule);
	}
}

bool CSetItemDrop::DropItem(LPOBJ lpMonster, LPOBJ lpUser)
{
	if (!ObjectMaxRange(lpUser->m_Index))
		return false;

	if (!ObjectMaxRange(lpMonster->m_Index))
		return false;

	if (GetLargeRand() % 1000000 >= this->m_dwDropUseRate)
		return false;

	VM_TIGER_WHITE_START
/*
	int checkval = 0;
	int checkval2 = 0;
*/
	int checkval = 0x8520571;
	int checkval2 = 0x194536;

	CHECK_PROTECTION(checkval, 0x8520571)
	CHECK_CODE_INTEGRITY(checkval2, 0x194536)

	SETITEM_DROP_RULE * lpDropRule = this->GetDropRule(lpUser, lpMonster);

	if (lpDropRule == NULL)
	{
		return false;
	}

	if (checkval != 0x8520571)
	{
		g_ConfigRead.data.common.gAddZen = GetLargeRand() % 1000000;
	}

	SETITEM_DROP_ITEM * lpDropItem = this->GetItemData(lpDropRule);

	if (lpDropItem == NULL)
	{
		return false;
	}

	if (checkval2 != 0x194536)
	{
		g_MaxStatsInfo.GetClass.MLPointPerLevel = rand() % 500;
	}

	VM_TIGER_WHITE_END

	for (int i = 0; i < lpDropItem->ItemCount; i++)
	{
		int iItemType = 0;
		int iSetItemOption = 0;

		if (lpDropItem->ItemCat == (BYTE)-1)
		{
			iItemType = gSetItemOption.GenRandomItemNum();
			iSetItemOption = gSetItemOption.GenSetOption(iItemType);
		}

		else
		{
			iItemType = gSetItemOption.GenRandomItemNumByCat(lpDropItem->ItemCat);
			iSetItemOption = gSetItemOption.GenSetOption(iItemType);
		}

		LPITEM_ATTRIBUTE p = &ItemAttribute[iItemType];

		if (!p)
		{
			g_Log.AddC(TColor::Red, "ERROR - Item not found (%d) CSetItemDrop::DropItem (%s)(%d)", iItemType, lpUser->Name, lpMonster->Class);
			return false;
		}

		if (iSetItemOption == 0)
		{
			g_Log.AddC(TColor::Red, "ERROR - Set Item Option not found (%d) (Cat:%d) CSetItemDrop::DropItem (%s)(%d)", iItemType, lpDropItem->ItemCat, lpUser->Name, lpMonster->Class);
			return false;
		}

		BYTE btSkill = 0;
		BYTE btLuck = 0;
		BYTE btOption = 0;
		BYTE btExc = 0;
		BYTE btItemLevel = 0;
		BYTE btItemDurability = 0;
		BYTE btNewExcOption[5];
		memset(btNewExcOption, -1, sizeof(btNewExcOption));

		if (lpDropItem->Skill == (BYTE)-1)
		{
			btSkill = rand() % 2;
		}

		else
		{
			btSkill = lpDropItem->Skill;
		}

		if (lpDropItem->Luck == (BYTE)-1)
		{
			btLuck = rand() % 2;
		}

		else
		{
			btLuck = lpDropItem->Luck;
		}

		if (lpDropItem->Option == (BYTE)-1)
		{
			btOption = rand() % (g_ConfigRead.data.common.Is28Opt == true ? 7 : 4);
		}

		else
		{
			btOption = lpDropItem->Option;
		}

		if (lpDropItem->ExcOption[0] == (BYTE)-2)
		{
			if (ItemAttribute[iItemType].ItemKindA != ITEM_KIND_A_WING)
			{
				btExc = g_ItemOptionTypeMng.CommonExcOptionRand(ItemAttribute[iItemType].ItemKindA, btNewExcOption);
			}

			else
			{
				btExc = g_ItemOptionTypeMng.WingExcOptionRand(ItemAttribute[iItemType].ItemKindA, ItemAttribute[iItemType].ItemKindB, btNewExcOption);
			}
		}

		else if (lpDropItem->ExcOption[0] == (BYTE)-3)
		{
			if (ItemAttribute[iItemType].ItemKindA != ITEM_KIND_A_WING)
			{
				btExc = g_ItemOptionTypeMng.CommonExcOptionGetCount(ItemAttribute[iItemType].ItemKindA, btNewExcOption, lpDropItem->ExcOption[1]);
			}

			else
			{
				btExc = g_ItemOptionTypeMng.WingExcOptionGetCount(ItemAttribute[iItemType].ItemKindA, ItemAttribute[iItemType].ItemKindB, btNewExcOption, lpDropItem->ExcOption[1]);
			}
		}

		else
		{
			for (int i = 0; i < 9; i++)
			{
				if (lpDropItem->ExcOption[i] == -1)
				{
					continue;
				}

				if (lpDropItem->ExcOption[i] >= 6)
				{
					bool bOptionFound = false;

					for (int j = 0; j < 5; j++)
					{
						if (btNewExcOption[j] == lpDropItem->ExcOption[i])
						{
							bOptionFound = true;
							break;
						}
					}

					if (bOptionFound == true)
					{
						continue;
					}
					
					for (int j = 0; j < 5; j++)
					{
						if (btNewExcOption[j] == 0xFF)
						{
							btNewExcOption[j] = lpDropItem->ExcOption[i];
							break;
						}
					}
				}

				else
				{
					btExc |= (1 << (5 - lpDropItem->ExcOption[i]));
				}
			}
		}

		btItemLevel = this->GetItemLevel(lpDropItem->MinLevel, lpDropItem->MaxLevel);
		btItemDurability = ItemGetDurability(iItemType, btItemLevel, btExc, iSetItemOption);

		ItemSerialCreateSend(lpMonster->m_Index, lpMonster->MapNumber, lpMonster->X, lpMonster->Y, iItemType, btItemLevel, 0, btSkill, btLuck, btOption, lpUser->m_Index, btExc, iSetItemOption, 0, btNewExcOption, 0);
	}

	return true;
}

SETITEM_DROP_RULE * CSetItemDrop::GetDropRule(OBJECTSTRUCT * lpUser, OBJECTSTRUCT * lpMonster)
{
	TRandomPoolMgr RandomPoolSection;

	RandomPoolSection.InitPool();
	int SectionID = 1;

	for (std::vector<SETITEM_DROP_RULE>::iterator It = this->m_vtDropRules.begin(); It != this->m_vtDropRules.end(); It++, SectionID++)
	{
		if (It->MonsterIndex != lpMonster->Class && It->MonsterIndex != (WORD)-1)
		{
			continue;
		}

		if (It->MapNumber != lpMonster->MapNumber && It->MapNumber != (BYTE)-1)
		{
			continue;
		}

		if (lpMonster->Level < It->MonsterMinLevel && It->MonsterMinLevel != (WORD)-1)
		{
			continue;
		}

		if (lpMonster->Level > It->MonsterMaxLevel && It->MonsterMaxLevel != (WORD)-1)
		{
			continue;
		}

		if (lpUser->Level < It->PlayerMinLevel && It->PlayerMinLevel != (WORD)-1)
		{
			continue;
		}

		if (lpUser->Level > It->PlayerMaxLevel && It->PlayerMaxLevel != (WORD)-1)
		{
			continue;
		}

		RandomPoolSection.AddValue(SectionID, 0);
	}

	int RetSection = RandomPoolSection.GetRandomValue(eRANDOMPOOL_BY_RANDOM);

	if (RetSection == 0)
	{
		return NULL;
	}

	return &this->m_vtDropRules[RetSection - 1];
}

SETITEM_DROP_ITEM * CSetItemDrop::GetItemData(SETITEM_DROP_RULE *lpRule)
{
	int iSuccessRate = 0;
	int iSuccessMaxRate = 0;
	int itemDropRate = 0;

	for (std::vector<SETITEM_DROP_ITEM>::iterator It = lpRule->m_vtSetItems.begin(); It != lpRule->m_vtSetItems.end(); It++)
	{
		iSuccessMaxRate += It->DropRate;
	}

	if (iSuccessMaxRate < 1)
		return NULL;

	int randResult = rand() % iSuccessMaxRate;

	for (std::vector<SETITEM_DROP_ITEM>::iterator It = lpRule->m_vtSetItems.begin(); It != lpRule->m_vtSetItems.end(); It++)
	{
		SETITEM_DROP_ITEM * lpItemDrop = &*It;
		iSuccessRate += lpItemDrop->DropRate;

		itemDropRate = iSuccessRate;

		int iItemRateRangeLow = iSuccessRate - itemDropRate;
		int iItemRateRangeHigh = iSuccessRate;

		if (randResult <= iItemRateRangeHigh && randResult >= iItemRateRangeLow)
		{
			return &*It;
		}
	}

	return NULL;
}


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

