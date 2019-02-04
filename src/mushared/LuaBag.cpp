////////////////////////////////////////////////////////////////////////////////
// LuaBag.cpp
#include "stdafx.h"
#include "LuaBag.h"
#include "configread.h"
#include "MuunSystem.h"
#include "ItemSocketOptionSystem.h"
#include "SocketItemType.h"
#include "PentagramSystem.h"
#include "gObjMonster.h"
#include "CustomMichi.h"

CLuaBag gLuaBag;

CLuaBag::CLuaBag() : m_Lua(true)
{
}

CLuaBag::~CLuaBag()
{

}

void CLuaBag::Init()
{
	this->m_Lua.RegisterData(RegisterItemInfo, "RegisterItemInfo", "ItemInfo", sizeof(this->m_ItemInfo));
	this->m_Lua.DoFile(g_ConfigRead.GetPath(ITEMBAG_SCRIPT_FILE));
}

void CLuaBag::InsertItemMap(BAG_ITEM * m_Item)
{
	this->m_ItemInfo[0].Name = "ItemType";
	this->m_ItemInfo[0].Variable = m_Item->btItemType;

	this->m_ItemInfo[1].Name = "ItemIndex";
	this->m_ItemInfo[1].Variable = m_Item->wItemIndex;

	this->m_ItemInfo[2].Name = "ItemMinLevel";
	this->m_ItemInfo[2].Variable = m_Item->btItemMinLevel;

	this->m_ItemInfo[3].Name = "ItemMaxLevel";
	this->m_ItemInfo[3].Variable = m_Item->btItemMaxLevel;

	this->m_ItemInfo[4].Name = "Skill";
	this->m_ItemInfo[4].Variable = m_Item->btIsSkill;

	this->m_ItemInfo[5].Name = "Luck";
	this->m_ItemInfo[5].Variable = m_Item->btIsLuck;

	this->m_ItemInfo[6].Name = "Option";
	this->m_ItemInfo[6].Variable = m_Item->btIsOption;

	this->m_ItemInfo[7].Name = "Anc";
	this->m_ItemInfo[7].Variable = m_Item->btIsSetItem;

	this->m_ItemInfo[8].Name = "Socket";
	this->m_ItemInfo[8].Variable = m_Item->btIsSocketItem;

	this->m_ItemInfo[9].Name = "Elemental";
	this->m_ItemInfo[9].Variable = m_Item->btIsElementalItem;

	this->m_ItemInfo[10].Name = "MuunEvolutionItemType";
	this->m_ItemInfo[10].Variable = m_Item->btMuunEvolutionItemType;

	this->m_ItemInfo[11].Name = "MuunEvolutionItemIndex";
	this->m_ItemInfo[11].Variable = m_Item->wMuunEvolutionItemIndex;

	LPITEM_ATTRIBUTE p = GetItemAttr(ITEMGET(m_Item->btItemType, m_Item->wItemIndex));

	this->m_ItemInfo[12].Name = "KindA";
	this->m_ItemInfo[12].Variable = p->ItemKindA;

	this->m_ItemInfo[13].Name = "Durability";
	this->m_ItemInfo[13].Variable = m_Item->btItemDur;

	this->m_ItemInfo[14].Name = "Duration";
	this->m_ItemInfo[14].Variable = m_Item->dwDuration;

	for (int i = 0; i < 9; i++)
	{
		char szName[32];
		sprintf(szName, "Ex%d", i + 1);

		this->m_ItemInfo[15 + i].Name = szName;
		this->m_ItemInfo[15 + i].Variable = m_Item->btIsExItem[i];
	}
}

BOOL CLuaBag::GetVariableItem(lua_State * Lua)
{
	const char * VarName = luaL_checkstring(Lua, 2);

	for (int i = 0; i < (sizeof(gLuaBag.m_ItemInfo) / sizeof(gLuaBag.m_ItemInfo[0])); i++)
	{
		if (!strcmp(gLuaBag.m_ItemInfo[i].Name.c_str(), VarName))
		{
			lua_pushinteger(Lua, gLuaBag.m_ItemInfo[i].Variable);
			return true;
		}
	}

	return false;
}

BOOL CLuaBag::SetVariableItem(lua_State * Lua)
{
	const char * VarName = luaL_checkstring(Lua, 2);

	for (int i = 0; i < (sizeof(gLuaBag.m_ItemInfo) / sizeof(gLuaBag.m_ItemInfo[0])); i++)
	{
		if (!strcmp(gLuaBag.m_ItemInfo[i].Name.c_str(), VarName))
		{
			return true;
		}
	}

	return false;
}

bool CLuaBag::DropCommonBag(int aIndex, BYTE MapNumber, BYTE DropX, BYTE DropY, BAG_ITEM *m_Item)
{

	this->InsertItemMap(m_Item);

	int iResult;
	this->m_Lua.Generic_Call("CommonBagItemDrop", "iiii>i", aIndex, MapNumber, DropX, DropY, &iResult);

	return iResult;
}

bool CLuaBag::DropMonsterBag(int aIndex, int iMonsterIndex, BYTE MapNumber, BYTE DropX, BYTE DropY, BAG_ITEM *m_Item)
{
	this->InsertItemMap(m_Item);

	int iResult;
	this->m_Lua.Generic_Call("MonsterBagItemDrop", "iiiii>i", iMonsterIndex, MapNumber, DropX, DropY, aIndex, &iResult);

	return iResult;
}

bool CLuaBag::DropEventBag(int aIndex, int iMonsterIndex, BYTE MapNumber, BYTE DropX, BYTE DropY, BAG_ITEM *m_Item)
{
	this->InsertItemMap(m_Item);

	int iResult;
	this->m_Lua.Generic_Call("EventBagItemDrop", "iiiii>i", iMonsterIndex, MapNumber, DropX, DropY, aIndex, &iResult);

	return iResult;
}

void CLuaBag::LoadItemBag()
{
	this->m_Lua.Generic_Call("LoadItemBag", ">");
}

bool CLuaBag::MakeItemFromBag(int aIndex, BAG_ITEM * m_Item, CItem & Item, time_t & DurationItem)
{
	this->InsertItemMap(m_Item);

	int SocketCount = 0;
	int MuunEvolutionItemID = -1;
	int Durability = 0;
	int Duration = 0;
	int iNewExcOption[9];
	int ExOption = 0;

	this->m_Lua.Generic_Call("EventBagMakeItem", ">iiiiiiiiiii",
		&Item.m_Level, &Durability, &Item.m_Option1, &Item.m_Option2, &Item.m_Option3,
		&Item.m_SetOption, &Item.m_BonusSocketOption, &SocketCount, &MuunEvolutionItemID, &Duration,
		&iNewExcOption[0], &iNewExcOption[1], &iNewExcOption[2], &iNewExcOption[3], &iNewExcOption[4], &iNewExcOption[5],
		&iNewExcOption[6], &iNewExcOption[7], &iNewExcOption[8]);

	Item.m_Type = ITEMGET(m_Item->btItemType, m_Item->wItemIndex);
	Item.m_Durability = Durability;
	DurationItem = Duration;

	BYTE SocketOption[5] = { -1, -1, -1, -1, -1 };

	if (g_CMuunSystem.IsStoneofEvolution(m_Item->btItemType) == true)
	{
		SocketOption[0] = MuunEvolutionItemID >> 8;
		SocketOption[1] = ITEM_GET_INDEX(MuunEvolutionItemID);
	}
	else
	{
		for (int i = 0; i < SocketCount; i++)
		{
			SocketOption[i] = 0xFE;
		}
	}

	if (gSocketItemType.CheckSocketItemType(m_Item->btItemType) == true)
	{
		for (int i = 0; i < SocketCount; i++)
		{
			SocketOption[i] = 0xFE;
		}
	}

	if (g_PentagramSystem.IsPentagramItem(m_Item->btItemType) == TRUE)
	{
		for (int i = 0; i < SocketCount; i++)
		{
			SocketOption[i] = 0xFE;
		}
	}

	if (iNewExcOption[0] == -2)
	{
		if (ItemAttribute[Item.m_Type].ItemKindA == ITEM_KIND_A_WING)
		{
			Item.m_NewOption = g_ItemOptionTypeMng.WingExcOptionRand(ItemAttribute[Item.m_Type].ItemKindA, ItemAttribute[Item.m_Type].ItemKindB, Item.m_SocketOption);
		}
		else
		{
			Item.m_NewOption = g_ItemOptionTypeMng.CommonExcOptionRand(ItemAttribute[Item.m_Type].ItemKindA, Item.m_SocketOption);
		}
	}

	else if (iNewExcOption[0] == -3)
	{
		if (ItemAttribute[Item.m_Type].ItemKindA == ITEM_KIND_A_WING)
		{
			Item.m_NewOption = g_ItemOptionTypeMng.WingExcOptionGetCount(ItemAttribute[Item.m_Type].ItemKindA, ItemAttribute[Item.m_Type].ItemKindB, Item.m_SocketOption, iNewExcOption[1]);
		}
		else
		{
			Item.m_NewOption = g_ItemOptionTypeMng.CommonExcOptionGetCount(ItemAttribute[Item.m_Type].ItemKindA, Item.m_SocketOption, iNewExcOption[1]);
		}
	}

	else if (iNewExcOption[0] == -4)
	{
		Item.m_NewOption = NewOptionRand2(g_ConfigRead.MinOptionCount, g_ConfigRead.MaxOptionCount);
		Item.m_Option1 = 1;
	}
	else
	{
		for (int i = 0; i < 9; i++)
		{
			if (iNewExcOption[i] == -1)
			{
				continue;
			}

			if (iNewExcOption[i] >= 6)
			{
				bool bOptionFound = false;

				for (int j = 0; j < 5; j++)
				{
					if (Item.m_SocketOption[j] == iNewExcOption[i])
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
					if (Item.m_SocketOption[j] == 0xFF)
					{	//[K2]
						//g_Log.AddC(TColor::Yellow, "[K2] Socket2 %d  ", iNewExcOption[i]);
						Item.m_SocketOption[j] = iNewExcOption[i];
						break;
					}
				}

			}

			else
			{
				Item.m_NewOption |= (1 << (5 - iNewExcOption[i]));
			}
		}
	}

	return true;
}

bool CLuaBag::MakeItemFromBagForGremoryCase(int aIndex, BAG_ITEM * m_Item, _stGremoryCaseItem & Item)
{
	this->InsertItemMap(m_Item);

	int SocketCount = 0;
	int MuunEvolutionItemID = -1;
	int Durability = 0;
	int Duration = 0;
	int iNewExcOption[9];
	int iSocketCount = 0;

	this->m_Lua.Generic_Call("EventBagMakeItem", ">iiiiiiiiiiiiiiiii",
		&Item.btItemLevel, &Durability, &Item.btItemSkill, &Item.btItemLuck, &Item.btItemOption,
		&Item.btItemSetOption, &Item.btItemMainAttribute, &iSocketCount, &MuunEvolutionItemID, &Duration,
		&iNewExcOption[0], &iNewExcOption[1], &iNewExcOption[2], &iNewExcOption[3], &iNewExcOption[4], &iNewExcOption[5],
		&iNewExcOption[6], &iNewExcOption[7], &iNewExcOption[8]);

	Item.wItemID = ITEMGET(m_Item->btItemType, m_Item->wItemIndex);

	if (Durability == 0)
	{
		Item.btItemDurability = ItemGetDurability(Item.wItemID, Item.btItemLevel, Item.btItemExcOption, Item.btItemSetOption);
	}

	else
	{
		Item.btItemDurability = Durability;
	}

	if (iNewExcOption[0] == -2)
	{
		if (ItemAttribute[Item.wItemID].ItemKindA == ITEM_KIND_A_WING)
		{
			Item.btItemExcOption = g_ItemOptionTypeMng.WingExcOptionRand(ItemAttribute[Item.wItemID].ItemKindA, ItemAttribute[Item.wItemID].ItemKindB, Item.btItemSocketOption);
		}

		else
		{
			Item.btItemExcOption = g_ItemOptionTypeMng.CommonExcOptionRand(ItemAttribute[Item.wItemID].ItemKindA, Item.btItemSocketOption);
		}
	}

	else if (iNewExcOption[0] == -3)
	{
		if (ItemAttribute[Item.wItemID].ItemKindA == ITEM_KIND_A_WING)
		{
			Item.btItemExcOption = g_ItemOptionTypeMng.WingExcOptionGetCount(ItemAttribute[Item.wItemID].ItemKindA, ItemAttribute[Item.wItemID].ItemKindB, Item.btItemSocketOption, iNewExcOption[1]);
		}

		else
		{
			Item.btItemExcOption = g_ItemOptionTypeMng.CommonExcOptionGetCount(ItemAttribute[Item.wItemID].ItemKindA, Item.btItemSocketOption, iNewExcOption[1]);
		}
	}

	else
	{

		for (int i = 0; i < 9; i++)
		{
			if (iNewExcOption[i] == -1)
			{
				continue;
			}

			if (iNewExcOption[i] >= 6)
			{
				bool bOptionFound = false;

				for (int j = 0; j < 5; j++)
				{
					if (Item.btItemSocketOption[j] == iNewExcOption[i])
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
					if (Item.btItemSocketOption[j] == 0xFF)
					{
						Item.btItemSocketOption[j] = 0xFF;
						//break;

					}
					//[K2]
					//g_Log.AddC(TColor::Yellow, "[K2] Socket4 [%d]  ", Item.btItemSocketOption[j]);

					//}
				}
			}

			else
			{
				Item.btItemExcOption |= (1 << (5 - iNewExcOption[i]));
			}
		}
	}

	Item.wMuunEvoItemType = ITEM_GET_TYPE(MuunEvolutionItemID);
	Item.btMuunEvoItemIndex = ITEM_GET_INDEX(MuunEvolutionItemID);

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

