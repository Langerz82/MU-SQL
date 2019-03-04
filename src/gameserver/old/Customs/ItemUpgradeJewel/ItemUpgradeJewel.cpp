////////////////////////////////////////////////////////////////////////////////
// ItemUpgradeJewel.cpp
#include "StdAfx.h"
#include "ItemUpgradeJewel.h"
#include "Logging/Log.h"
#include "Main.h"
#include "SetItemOption.h"
#include "configread.h"
#include "ItemSocketOptionSystem.h"
#include "LuckyItemManager.h"
#include "SocketItemType.h"
#include "GameProtocol.h"
#include "GOFunctions.h"

using namespace pugi;
ItemUpgradeJewels g_ItemUpgradeJewels;

ItemUpgradeJewels::ItemUpgradeJewels()
{

}

ItemUpgradeJewels::~ItemUpgradeJewels()
{

}

void ItemUpgradeJewels::Init()
{
	this->m_UpradeInfo.clear();
	if (this->m_UpradeInfo.capacity() > 0)
	{
		std::vector<ItemUpgradeJewelsInfo>().swap(this->m_UpradeInfo);
	}
}

void ItemUpgradeJewels::Load()
{
	this->Init();
	this->Read(g_ConfigRead.GetPath(FILE_ITEM_UPGRADE_JEWELS));
}

void ItemUpgradeJewels::Read(LPSTR File)
{
	xml_document Document;
	xml_parse_result Result = Document.load_file(File);
	// ----
	if (Result.status != status_ok)
	{
		sLog->outError("[IGC_ItemUpgradeJewel] File %s not found!", File);
		return;
	}
	// ----
	xml_node ChildCommon = Document.child("itemupgradejewels");
	xml_node Node;
	// ----
	for (Node = ChildCommon.child("jewel"); Node; Node = Node.next_sibling())
	{
		ItemUpgradeJewelsInfo lpItem;

		lpItem.ItemType = Node.attribute("type").as_int((int)-1);
		lpItem.ItemIndex = Node.attribute("index").as_int((int)-1);
		lpItem.DefaultRate = Node.attribute("rate").as_uint();
		lpItem.TargetLevelMin = Node.child("rules").child("level").attribute("min").as_int((BYTE)-1);
		lpItem.TargetLevelMax = Node.child("rules").child("level").attribute("max").as_int((BYTE)-1);
		lpItem.TargetRankNormal = Node.child("rules").child("rank").attribute("normal").as_int((BYTE)-1);
		lpItem.TargetRankExcellent = Node.child("rules").child("rank").attribute("excellent").as_int((BYTE)-1);
		lpItem.TargetRankAncient = Node.child("rules").child("rank").attribute("ancient").as_int((BYTE)-1);
		lpItem.TargetRankSocket = Node.child("rules").child("rank").attribute("socket").as_int((BYTE)-1);

		lpItem.m_RateChangeRules.clear();
		if (lpItem.m_RateChangeRules.capacity() > 0)
		{
			std::vector<ItemUpgradeJewelsRateChangeInfo>().swap(lpItem.m_RateChangeRules);
		}

		xml_node RateNode;

		for (RateNode = Node.child("ratechange"); RateNode; RateNode = RateNode.next_sibling())
		{
			ItemUpgradeJewelsRateChangeInfo lpRate;
			lpRate.Type = RateNode.attribute("type").as_int((BYTE)-1);
			lpRate.Level = RateNode.attribute("level").as_int((BYTE)-1);
			lpRate.Option = RateNode.attribute("option").as_int((BYTE)-1);
			lpRate.RateIncrease = RateNode.attribute("increase").as_uint(0);
			lpRate.RateDecrease = RateNode.attribute("decrease").as_uint(0);
			lpItem.m_RateChangeRules.push_back(lpRate);
		}

		this->m_UpradeInfo.push_back(lpItem);
	}
}

void ItemUpgradeJewels::ProcInsert(CGameObject &Obj, int JewelPos, int TargetPos)
{
	ItemUpgradeJewelsInfo* lpJewel = NULL;

	for (int i = 0; i < this->m_UpradeInfo.size(); i++)
	{
		if (ITEMGET(this->m_UpradeInfo[i].ItemType,
			this->m_UpradeInfo[i].ItemIndex) == Obj.pntInventory[JewelPos]->m_Type)
		{
			lpJewel = &this->m_UpradeInfo[i];
			break;
		}
	}

	if (lpJewel == NULL)
	{
		GSProtocol.GCReFillSend(&Obj, (WORD)Obj.Life, 0xFD, 1, Obj.iShield);
		return;
	}

	if (this->ProcUpgrade(Obj, JewelPos, TargetPos, lpJewel))
	{
		gObjInventoryItemSet(Obj, JewelPos, -1);
		Obj.pntInventory[JewelPos]->Clear();
		GSProtocol.GCInventoryItemOneSend(&Obj, TargetPos);
		GSProtocol.GCInventoryItemDeleteSend(&Obj, JewelPos, 1);
	}
	else
	{
		GSProtocol.GCReFillSend(&Obj, (WORD)Obj.Life, 0xFD, 1, Obj.iShield);
	}
}

bool ItemUpgradeJewels::ProcUpgrade(CGameObject &Obj, int JewelPos, int TargetPos, ItemUpgradeJewelsInfo* lpJewel)
{
	if (JewelPos < 0 || JewelPos > MAIN_INVENTORY_SIZE - 1)
	{
		return false;
	}

	if (TargetPos < 0 || TargetPos > MAIN_INVENTORY_SIZE - 1)
	{
		return false;
	}

	if (!Obj.pntInventory[JewelPos]->IsItem()
		|| !Obj.pntInventory[TargetPos]->IsItem())
	{
		return false;
	}

	int JewelCode = Obj.pntInventory[JewelPos]->m_Type;
	int TargetCode = Obj.pntInventory[TargetPos]->m_Type;

	if (JewelCode == ITEMGET(14, 13) && TargetCode == ITEMGET(0, 41))
	{
		if (UseBundleOfBlessJewel(Obj, JewelPos, TargetPos) == TRUE)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	if (JewelCode == ITEMGET(14, 13) && TargetCode == ITEMGET(13, 37))
	{
		CItemObject* ItemFenrir = Obj.pntInventory[TargetPos];

		if (ItemFenrir->m_Durability >= 255)
		{
			return false;
		}

		if (rand() % 10000 < g_iFenrirRepairRate)
		{
			int iAddDur = rand() % 150 + 50;

			if ((ItemFenrir->m_Durability + iAddDur) > 255)
			{
				ItemFenrir->m_Durability = 255.0f;
			}
			else
			{
				ItemFenrir->m_Durability += iAddDur;
			}

			MsgOutput(Obj, "[FENRIR REPAIR] Success - %d/255", int(ItemFenrir->m_Durability));
		}
		else
		{
			MsgOutput(Obj, "[FENRIR REPAIR] Failed.");
		}

		return true;
	}

	CItemObject* ItemTarget = Obj.pntInventory[TargetPos];

	if (g_LuckyItemManager.IsLuckyItemEquipment(ItemTarget->m_Type) == TRUE)
	{
		return false;
	}

	if (lpJewel->TargetLevelMax != (BYTE)-1
		&& ItemTarget->m_Level >= lpJewel->TargetLevelMax)
	{
		return false;
	}

	if (lpJewel->TargetLevelMin != (BYTE)-1
		&& ItemTarget->m_Level < lpJewel->TargetLevelMin)
	{
		return false;
	}

	if (ItemTarget->m_Type >= ITEMGET(12, 36) && ItemTarget->m_Type <= ITEMGET(12, 40))
	{

	}
	else if (ItemTarget->m_Type == ITEMGET(12, 270))
	{

	}
	else if (ItemTarget->m_Type == ITEMGET(12, 269))
	{

	}
#if (CUSTOM_WINGS == 1)
	else if (ItemTarget->m_Type >= ITEMGET(12, 440) && ItemTarget->m_Type <= ITEMGET(12, 445))
	{

	}
#endif
	else if (ItemTarget->m_Type >= ITEMGET(12, 41) &&	ItemTarget->m_Type <= ITEMGET(12, 43))
	{

	}
	else if (ItemTarget->m_Type == ITEMGET(12, 49) || ItemTarget->m_Type == ITEMGET(12, 50))
	{

	}
	else if (ItemTarget->m_Type >= ITEMGET(12, 200) && ItemTarget->m_Type <= ITEMGET(12, 214))
	{

	}
	else if (ItemTarget->m_Type >= ITEMGET(12, 262) &&	ItemTarget->m_Type <= ITEMGET(12, 265))
	{

	}
	else if (!(ItemTarget->m_Type < ITEMGET(12, 7)	|| ItemTarget->m_Type == ITEMGET(13, 30))
		|| ItemTarget->m_Type == ITEMGET(4, 7)	|| ItemTarget->m_Type == ITEMGET(4, 15))
	{
		return false;
	}

	if (ItemTarget->m_NewOption && lpJewel->TargetRankExcellent != 1)
	{
		return false;
	}

	if (gSetItemOption.IsSetItem(ItemTarget->m_Type) && ItemTarget->m_SetOption > 0
		&& lpJewel->TargetRankAncient != 1)
	{
		return false;
	}

	if (gSocketItemType.CheckSocketItemType(ItemTarget->m_Type) && lpJewel->TargetRankSocket != 1)
	{
		return false;
	}

	DWORD Rate = lpJewel->DefaultRate;

	for (int i = 0; i < lpJewel->m_RateChangeRules.size(); i++)
	{
		bool Bonus = false;

		if (lpJewel->m_RateChangeRules[i].Type == ItemUpgradeJewelsRateType::Luck
			&&	ItemTarget->m_Option2)
		{
			Bonus = true;
		}
		else if (lpJewel->m_RateChangeRules[i].Type == ItemUpgradeJewelsRateType::Excellent
			&&	ItemTarget->m_NewOption)
		{
			Bonus = true;
		}
		else if (lpJewel->m_RateChangeRules[i].Type == ItemUpgradeJewelsRateType::Ancient
			&&	gSetItemOption.IsSetItem(ItemTarget->m_Type) && ItemTarget->m_SetOption > 0)
		{
			Bonus = true;
		}
		else if (lpJewel->m_RateChangeRules[i].Type == ItemUpgradeJewelsRateType::Socket
			&&	gSocketItemType.CheckSocketItemType(ItemTarget->m_Type))
		{
			Bonus = true;
		}
		else if (lpJewel->m_RateChangeRules[i].Type == ItemUpgradeJewelsRateType::Level
			&&	lpJewel->m_RateChangeRules[i].Level != (BYTE)-1 && lpJewel->m_RateChangeRules[i].Level == ItemTarget->m_Level)
		{
			Bonus = true;
		}
		else if (lpJewel->m_RateChangeRules[i].Type == ItemUpgradeJewelsRateType::Option
			&&	lpJewel->m_RateChangeRules[i].Option != (BYTE)-1 && lpJewel->m_RateChangeRules[i].Option == ItemTarget->m_Option3)
		{
			Bonus = true;
		}

		if (Bonus)
		{
			Rate -= lpJewel->m_RateChangeRules[i].RateDecrease;
			Rate += lpJewel->m_RateChangeRules[i].RateIncrease;
		}
	}

	if (JewelCode == ITEMGET(14, 13))
	{
		if (rand() % 10000 < Rate)
		{
			ItemTarget->m_Level++;

			if (ItemTarget->m_Level > lpJewel->TargetLevelMax)
			{
				ItemTarget->m_Level = lpJewel->TargetLevelMax;
			}
		}
	}
	else if (JewelCode == ITEMGET(14, 14))
	{
		if (rand() % 10000 < Rate)
		{
			ItemTarget->m_Level++;

			if (ItemTarget->m_Level > lpJewel->TargetLevelMax)
			{
				ItemTarget->m_Level = lpJewel->TargetLevelMax;
			}
		}
		else
		{
			if (ItemTarget->m_Level >= 7)
			{
				ItemTarget->m_Level = 0;
			}
			else
			{
				ItemTarget->m_Level--;

				if (ItemTarget->m_Level < 0)
				{
					ItemTarget->m_Level = 0;
				}
			}
		}
	}
	else if (JewelCode == ITEMGET(14, 16))
	{
		if (ItemTarget->m_Option3 == 0)
		{
			if (ItemTarget->m_Type >= ITEMGET(12, 3) && ItemTarget->m_Type <= ITEMGET(12, 6) ||
				ItemTarget->m_Type == ITEMGET(12, 49) ||
				ItemTarget->m_Type == ITEMGET(12, 269) ||
				ItemTarget->m_Type == ITEMGET(12, 42))
			{
				ItemTarget->m_NewOption &= 0xDF;

				if (rand() % 2)
				{
					ItemTarget->m_NewOption |= 0x20;
				}
			}

			if (ItemTarget->m_Type >= ITEMGET(12, 36) &&
				ItemTarget->m_Type <= ITEMGET(12, 40) ||
				ItemTarget->m_Type == ITEMGET(12, 270) ||
				ItemTarget->m_Type == ITEMGET(12, 43) ||
				ItemTarget->m_Type == ITEMGET(12, 50)
#if (CUSTOM_WINGS == 1)
				||
				ItemTarget->m_Type >= ITEMGET(12, 440) &&
				ItemTarget->m_Type <= ITEMGET(12, 445)
#endif
				)
			{
				ItemTarget->m_NewOption &= 0xDF;

				if (rand() % 3)
				{
					ItemTarget->m_NewOption |= 0x20;
				}
				else if (rand() % 3)
				{
					ItemTarget->m_NewOption |= 0x10;
				}
			}

			if (ItemTarget->m_Type >= ITEMGET(12, 262) &&
				ItemTarget->m_Type <= ITEMGET(12, 265))
			{
				ItemTarget->m_NewOption &= 0xDF;

				if (rand() % 2)
				{
					ItemTarget->m_NewOption |= 0x10;
				}
			}
		}

		if (ItemTarget->m_Option3 >= 7)
		{
			return false;
		}

		if (rand() % 10000 < Rate)
		{
			ItemTarget->m_Option3++;
		}
		else
		{
			ItemTarget->m_Option3 = 0;
		}
	}
	else if (JewelCode == ITEMGET(14, 42))
	{
		if (g_kJewelOfHarmonySystem.m_bSystemSmeltingItem == false)
		{
			return false;
		}

		if (g_kJewelOfHarmonySystem.IsStrengthenByJewelOfHarmony(ItemTarget))
		{
			return false;
		}

		int iItemType = g_kJewelOfHarmonySystem._GetItemType(ItemTarget);

		if (iItemType == JEWELOFHARMONY_ITEM_TYPE_NULL)
		{
			return false;
		}

		int iItemOption = g_kJewelOfHarmonySystem._GetSelectRandomOption(ItemTarget, iItemType);

		if (iItemOption == AT_JEWELOFHARMONY_NOT_STRENGTHEN_ITEM)
		{
			return false;
		}

		int iItemOptionLevel = g_kJewelOfHarmonySystem.m_itemOption[iItemType][iItemOption].iRequireLevel;

		if (rand() % 10000 < Rate)
		{
			g_kJewelOfHarmonySystem._MakeOption(ItemTarget, iItemOption, iItemOptionLevel);
			GSProtocol.GCServerMsgStringSend("Adding Jewel Of Harmony Succes!", &Obj, 1);
		}
		else
		{
			GSProtocol.GCServerMsgStringSend("Adding Jewel Of Harmony Failed", &Obj, 1);
		}
	}

	gObjMakePreviewCharSet(Obj);

	float levelitemdur = (float)ItemGetDurability(Obj.pntInventory[TargetPos]->m_Type,
		Obj.pntInventory[TargetPos]->m_Level,Obj.pntInventory[TargetPos]->IsExtItem(),Obj.pntInventory[TargetPos]->IsSetItem());

	Obj.pntInventory[TargetPos]->m_Durability = levelitemdur * Obj.pntInventory[TargetPos]->m_Durability / Obj.pntInventory[TargetPos]->m_BaseDurability;

	Obj.pntInventory[TargetPos]->Convert(Obj.pntInventory[TargetPos]->m_Type,Obj.pntInventory[TargetPos]->m_Option1,
		Obj.pntInventory[TargetPos]->m_Option2,Obj.pntInventory[TargetPos]->m_Option3,Obj.pntInventory[TargetPos]->m_NewOption,
		Obj.pntInventory[TargetPos]->m_SetOption,Obj.pntInventory[TargetPos]->m_ItemOptionEx, 0, -1, 0,CURRENT_DB_VERSION);

	return true;
}

bool ItemUpgradeJewels::IsJewel(int ItemCode)
{
	for (int i = 0; i < m_UpradeInfo.size(); i++)
	{
		if (ITEMGET(m_UpradeInfo[i].ItemType,
			m_UpradeInfo[i].ItemIndex) == ItemCode)
		{
			return true;
		}
	}
	return false;
}

ItemUpgradeJewelsRateChangeInfo* ItemUpgradeJewels::GetRateChangeRule(ItemUpgradeJewelsInfo* lpJewel, BYTE Type)
{
	for (int i = 0; i < lpJewel->m_RateChangeRules.size(); i++)
	{
		if (lpJewel->m_RateChangeRules[i].Type == Type)
		{
			return &lpJewel->m_RateChangeRules[i];
		}
	}
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

