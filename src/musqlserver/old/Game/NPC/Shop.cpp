////////////////////////////////////////////////////////////////////////////////
// Shop.cpp
//#include "StdAfx.h"
#include "Shop.h"
#include "Main.h"
#include "ItemSocketOptionSystem.h"
#include "PentagramSystem.h"
#include "GensSystem.h"
#include "GameProtocol.h"
//#include "Crywolf.h"
//#include "util.h"
#include "VipSys.h"
#include "SocketItemType.h"

// GS-N 0.99.60T 0x004EF0A0
//	GS-N	1.00.18	JPN	0x00518EA0	-	Completed

CShop::CShop()
{
	this->Init();
}

CShop::~CShop()
{
	return;
}

void CShop::Init()
{
	this->SendItemDataLen = 0;
	this->ItemCount = 0;
	memset(this->ShopInventoryMap, 0, sizeof(this->ShopInventoryMap));
}

BOOL CShop::InsertItem(int type, int index, int level, int dur, int op1, int op2, int op3, BYTE newop[], int setop, int socketcount, int elemental, int serial)
{
	int itemp;
	int width;
	int height;
	int x;
	int y;
	int blank;

	blank = -1;
	itemp = type * MAX_SUBTYPE_ITEMS + index;

	if (itemp < 0)
	{
		sLog->outError("Error in creating item at shop %s %d Item: %d %d", __FILE__, __LINE__, type, index);
		return false;
	}

	LPITEM_ATTRIBUTE p = &ItemAttribute[itemp];

	if (!p)
	{
		sLog->outError("error-L2: %s %d", __FILE__, __LINE__);
		return FALSE;
	}

	ItemGetSize(itemp, width, height);

	if (width < 0 || height < 0)
	{
		// Error in getting item size in shop %s %d
		sLog->outError("Error in getting item size in shop %s %d", __FILE__, __LINE__);
		return FALSE;
	}

	for (y = 0; y < 15; y++)
	{
		for (x = 0; x < 8; x++)
		{
			if (this->ShopInventoryMap[x + y * 8] == 0)
			{
				blank = this->InentoryMapCheck(x, y, width, height);

				if (blank >= 0)
				{
					goto skiploop;
				}
			}
		}
	}

	if (blank < 0)
	{
		sLog->outError("error-L2 : %s %d", __FILE__, __LINE__);
		return FALSE;
	}

skiploop:
	BYTE btExc = 0;
	bool bIsExc = false;

	this->m_item[blank].m_Level = level;

	if (dur == 0)
	{
		dur = ItemGetDurability(ITEMGET(type, index), level, 0, 0);
	}

	for (int k = 0; k < 9; k++)
	{
		if (newop[k] == -1)
		{
			continue;
		}

		if (newop[k] >= 6)
		{
			bool bOptionFound = false;

			for (int j = 0; j < 5; j++)
			{
				if (this->m_item[blank].m_SocketOption[j] == newop[k])
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
				if (this->m_item[blank].m_SocketOption[j] == 0xFF)
				{
					this->m_item[blank].m_SocketOption[j] = newop[k];
					bIsExc = true;
					break;
				}
			}
		}

		else
		{
			btExc |= (1 << (5 - newop[k]));
			bIsExc = true;
		}
	}

	this->m_item[blank].m_Durability = dur;
	this->m_item[blank].Convert(itemp, op1, op2, op3, btExc, setop, 0, NULL, 0xFF, 0, CURRENT_DB_VERSION);
	this->m_item[blank].Value();

	if (gSocketItemType.CheckSocketItemType(itemp) && socketcount > 0)
	{
		g_SocketOptionSystem.MakeSocketSlot(&this->m_item[blank], socketcount);
	}
	//	g_SocketOptionSystem.MakeSocketSlot(&this->m_item[blank], socketcount);

	if (g_PentagramSystem.IsPentagramItem(itemp) == true)
	{
		if (socketcount == 0)
		{
			g_PentagramSystem.GetMakePentagramSlotCountNKind(this->m_item[blank].m_SocketOption, ITEMGET(type, index));
		}

		else
		{
			BYTE Slot[5] = { 0, 0, 0, 0, 0 };

			for (int i = 0; i < socketcount; i++)
			{
				Slot[i] = 1;
			}

			g_PentagramSystem.MakePentagramSocketSlot(&this->m_item[blank], Slot[0], Slot[1], Slot[2], Slot[3], Slot[4]);
		}

		this->m_item[blank].m_BonusSocketOption = elemental;
	}

	this->m_item[blank].m_serial = serial;

	this->SendItemData[this->SendItemDataLen] = blank;
	this->SendItemDataLen++;
	ItemByteConvert((BYTE*)&this->SendItemData[this->SendItemDataLen], this->m_item[blank]);
	this->SendItemDataLen += MAX_ITEM_INFO;//Season 4.5 changed
	this->ItemCount++;

	return TRUE;

}
int CShop::InentoryMapCheck(int sx, int sy, int width, int height)
{
	int x;
	int y;
	int blank = 0;

	if ((sx + width) > 8)
		return -1;

	if ((sy + height) > 15)
		return -1;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			if (this->ShopInventoryMap[(sy + y) * 8 + (sx + x)])
			{
				blank++;
				break;
			}
		}
	}

	if (blank == 0)
	{
		for (y = 0; y < height; y++)
		{
			for (x = 0; x < width; x++)
			{
				this->ShopInventoryMap[(sy + y) * 8 + (sx + x)] = 1;
			}
		}

		return (sx + sy * 8);
	}

	return -1;
}

BOOL CShop::LoadShopItem(char* filename)
{
	int Token;
	int type;
	int index;
	int level;
	int dur;
	int op1;
	int op2;
	int op3;
	BYTE newop[MAX_ITEM_EXC_OPTION];
	int setop;
	int socketcount;
	int elemental;
	int serial;

	this->Init();

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(filename);

	if (res.status != pugi::status_ok)
	{
		sLog->outError("Failed to load %s file! (%s", filename, res.description());
		return FALSE;
	}

	pugi::xml_node mainXMLsection = file.child("Shop");

	for (pugi::xml_node item = mainsection.child("Item"); item; item = item.next_sibling())
	{
		type = item.attribute("Cat").as_int();
		index = item.attribute("Index").as_int();
		level = item.attribute("Level").as_int();
		dur = item.attribute("Durability").as_int();
		op1 = item.attribute("Skill").as_int();
		op2 = item.attribute("Luck").as_int();
		op3 = item.attribute("Option").as_int();

		std::string szExcOptions = item.attribute("Exc").as_string();
		std::deque<int> exc_opt_list;
		strtk::parse(szExcOptions, ";", exc_opt_list);

		memset(newop, -1, sizeof(newop));

		for (int i = 0; i < exc_opt_list.size(); i++)
		{
			newop[i] = exc_opt_list.at(i);
		}

		setop = item.attribute("SetItem").as_int();
		socketcount = item.attribute("SocketCount").as_int();
		elemental = item.attribute("Elemental").as_int();
		serial = item.attribute("Serial").as_int(0);

		if (this->InsertItem(type, index, level, dur, op1, op2, op3, newop, setop, socketcount, elemental, serial) == FALSE)
		{
			sLog->outError("error-L3 : %s %s %d", filename, __FILE__, __LINE__);
		}
	}

	return true;
}

CShopMng g_ShopMng;

CShopMng::CShopMng()
{
	this->m_vecShopData.clear();
}

CShopMng::~CShopMng()
{

}

void CShopMng::LoadShopList(char* filename)
{
	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(filename);

	if (res.status != pugi::status_ok)
	{
		sLog->outError("Failed to load %s file! (%s", filename, res.description());
		return;
	}

	this->m_vecShopData.clear();

	pugi::xml_node mainXMLsection = file.child("ShopList");
	SHOP_DATA ShopData;

	for (pugi::xml_node shop = mainsection.child("Shop"); shop; shop = shop.next_sibling())
	{
		ShopData.iNpcIndex = shop.attribute("NPCIndex").as_int();
		ShopData.btMapNumber = shop.attribute("MapNumber").as_int();
		ShopData.btPosX = shop.attribute("PosX").as_int();
		ShopData.btPosY = shop.attribute("PosY").as_int();
		ShopData.btDir = shop.attribute("Dir").as_int();
		ShopData.btReqVipLevel = shop.attribute("VipType").as_int(0);
		ShopData.btOnlyForGameMaster = shop.attribute("GMShop").as_int(0);
		ShopData.btIsMossMerchant = shop.attribute("MossMerchant").as_int(0);
		ShopData.btBattleCoreShop = shop.attribute("BattleCore").as_int(0);

		char szPath[512];
		sprintf(szPath, "..\\IGCData\\Shops\\%s", shop.attribute("FileName").as_string());

		ShopData.m_ShopData.LoadShopItem(szPath);

		this->m_vecShopData.push_back(ShopData);
	}

}

SHOP_DATA * CShopMng::GetShop(CGameObject &Obj, CGameObject &Npc)
{
	if (Obj.Type != OBJ_USER)
	{
		return NULL;
	}

	if (lpNpc->Type != OBJ_NPC)
	{
		return NULL;
	}

	std::vector<SHOP_DATA>::iterator ItShopData = this->m_vecShopData.begin();

	for (; ItShopData != this->m_vecShopData.end(); ItShopData++)
	{
		if (ItShopData->iNpcIndex == lpNpc->Class && ItShopData->btMapNumber == lpNpc->MapNumber && ItShopData->btPosX == lpNpc->X && ItShopData->btPosY == lpNpc->Y)
		{
			break;
		}
	}

	if (ItShopData == this->m_vecShopData.end())
	{
		return NULL;
	}

	if (Obj.m_PlayerData->VipType < ItShopData->btReqVipLevel && ItShopData->btOnlyForGameMaster == FALSE)
	{
		if (Obj.m_PlayerData->VipType == 0)
		{
			gGameProtocol.ChatTargetSend(lpNpc, Lang.GetText(0, 579), Obj.m_Index);
		}

		else
		{
			char szMsg[256];
			sprintf(szMsg, Lang.GetText(0, 605), g_VipSystem.GetVipName(Obj.m_PlayerData->VipType), g_VipSystem.GetVipName(ItShopData->btReqVipLevel));
			gGameProtocol.ChatTargetSend(lpNpc, szMsg, Obj.m_Index);
		}

		return NULL;
	}

	if (ItShopData->btOnlyForGameMaster == TRUE)
	{
		if ((Obj.Authority & 0x20) != 0x20 || (Obj.GameMaster & GM_USE_GMSHOP) != GM_USE_GMSHOP)
		{
			gGameProtocol.ChatTargetSend(lpNpc, Lang.GetText(0, 580), Obj.m_Index);
			return NULL;
		}
	}

	if (ItShopData->m_ShopData.ItemCount < 1)
	{
		if ((rand() % 2) != 0)
			gGameProtocol.ChatTargetSend(lpNpc, Lang.GetText(0, 51), Obj.m_Index);
		else
			gGameProtocol.ChatTargetSend(lpNpc, Lang.GetText(0, 52), Obj.m_Index);

		return NULL;
	}

	if (g_ConfigRead.pk.bPkPenaltyDisable == FALSE && g_GensSystem.IsMapBattleZone(Obj.MapNumber) == FALSE && g_ConfigRead.pk.bPkCanUseShops == false)
	{
		BOOL bPlayerKiller = FALSE; //season 2.5 add-on

		if (Obj.PartyNumber >= 0) //season 2.5 add-on
		{
			if ((gParty.GetPKPartyPenalty(Obj.PartyNumber)) > 4)
			{
				bPlayerKiller = TRUE;
			}
		}
		else
		{
			if (Obj.m_PK_Level > 4)
			{
				bPlayerKiller = TRUE;
			}
		}

		if (bPlayerKiller == TRUE) //season 2.5 changed
		{
			if ((rand() % 2) != 0)
				gGameProtocol.ChatTargetSend(lpNpc, Lang.GetText(0, 53), Obj.m_Index);
			else
				gGameProtocol.ChatTargetSend(lpNpc, Lang.GetText(0, 54), Obj.m_Index);

			return NULL;
		}
	}

	return &*ItShopData;
}

bool CShopMng::IsShopNpc(int iNpcIndex, BYTE btMapNumber, BYTE btPosX, BYTE btPosY)
{
	for (std::vector<SHOP_DATA>::iterator ItShopData = this->m_vecShopData.begin(); ItShopData != this->m_vecShopData.end(); ItShopData++)
	{
		if (ItShopData->iNpcIndex == iNpcIndex && ItShopData->btMapNumber == btMapNumber && ItShopData->btPosX == btPosX && ItShopData->btPosY == btPosY)
		{
			return true;
		}
	}

	return false;
}

void CShopMng::SetShopNpcs()
{
	for (std::vector<SHOP_DATA>::iterator it = this->m_vecShopData.begin(); it != this->m_vecShopData.end(); it++)
	{
		if (it->btBattleCoreShop == TRUE && g_ConfigRead.server.GetServerType() != SERVER_BATTLECORE)
		{
			continue;
		}

		if (it->btBattleCoreShop == FALSE && g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
		{
			continue;
		}

		int iResult = gObjAddMonster(it->btMapNumber);

		if (iResult == -1)
		{
			continue;
		}
		//sLog->outBasic("SetShopNpcs - Map %d %d %d", it->btMapNumber, it->btPosX, it->btPosY);	// Memory allocation error
		gObjSetMonster(iResult, it->iNpcIndex);
		getGameObject(iResult)->m_MoveRange = 0;
		getGameObject(iResult)->MapNumber = it->btMapNumber;
		getGameObject(iResult)->X = it->btPosX;
		getGameObject(iResult)->Y = it->btPosY;
		getGameObject(iResult)->TX = it->btPosX;
		getGameObject(iResult)->TY = it->btPosY;
		getGameObject(iResult)->m_OldX = it->btPosX;
		getGameObject(iResult)->m_OldY = it->btPosY;
		getGameObject(iResult)->StartX = it->btPosX;
		getGameObject(iResult)->StartY = it->btPosY;
		getGameObject(iResult)->Dir = it->btDir;
		getGameObject(iResult)->Live = TRUE;
		getGameObject(iResult)->DieRegen = FALSE;
		getGameObject(iResult)->m_State = TRUE;
		getGameObject(iResult)->m_btNpcType = NPC_TYPES::NPC_SHOP;

		//sLog->outBasic("[Shops] Spawn Shop NPC (Index:%d) [MAP:%d][X:%d][Y:%d][NPCID:%d][DIR:%d]",iResult, it->btMapNumber, it->btPosX, it->btPosY, it->iNpcIndex, it->btDir);

		if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
		{
			if (CRYWOLF_MAP_RANGE(it->btMapNumber))
			{
				g_Crywolf.m_ObjCommonNPC.AddObj(iResult);
			}
		}
	}
}

CCancelItemSale g_CancelItemSale;

CCancelItemSale::CCancelItemSale()
{

}

CCancelItemSale::~CCancelItemSale()
{

}

void CCancelItemSale::CGReqSoldItemList(CGameObject &Obj)
{
	if (ObjectMaxRange(Obj.m_Index) == false)
	{
		return;
	}

	if (gObjIsConnected(Obj.m_Index) == false)
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		MsgOutput(iIndex, Lang.GetText(0, 613));
		return;
	}

	if (g_ConfigRead.data.common.CancelItemSaleEnable == false)
	{
		MsgOutput(iIndex, Lang.GetText(0, 613));
		return;
	}

	this->GDReqSoldItemList(Obj.m_Index);
}

void CCancelItemSale::GCAnsSoldItemList(CGameObject &Obj)
{
	BYTE Buffer[2048];
	PMSG_SHOP_REBUY_LIST_ANS pMsg;
	SHOP_REBUY_LIST_ITEM pItem;

	int iSize = sizeof(PMSG_SHOP_REBUY_LIST_ANS);
	int iItemCount = 0;

	for (int i = 0; i < MAX_CANCEL_ITEMS_SALE; i++)
	{
		if (Obj.m_PlayerData->m_CancelItemSaleList[i]->btUsed == TRUE)
		{
			pItem.wItemCount = Obj.m_PlayerData->m_CancelItemSaleList[i]->wItemCount;
			pItem.dwItemPrice = Obj.m_PlayerData->m_CancelItemSaleList[i]->dwItemPrice;
			pItem.iLeftTime = Obj.m_PlayerData->m_CancelItemSaleList[i]->iLeftTime;
			pItem.btItemNumber = Obj.m_PlayerData->m_CancelItemSaleList[i]->btItemNumber;
			memcpy(&pItem.btItemInfo, Obj.m_PlayerData->m_CancelItemSaleList[i]->btItemInfo, MAX_ITEM_INFO);

			memcpy(&Buffer[iSize], &pItem, sizeof(pItem));
			iSize += sizeof(pItem);
			iItemCount++;
		}
	}

	PHeadSubSetW((BYTE*)&pMsg, 0x6F, 0x00, iSize);
	pMsg.btCount = iItemCount;
	pMsg.btResult = 0;
	memcpy(&Buffer, &pMsg, sizeof(pMsg));

	IOCP.DataSend(Obj.m_Index, Buffer, iSize);
}

void CCancelItemSale::CGReqEndCancelItemSale(CGameObject &Obj)
{
	if (ObjectMaxRange(Obj.m_Index) == false)
	{
		return;
	}

	if (gObjIsConnected(Obj.m_Index) == false)
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (g_ConfigRead.data.common.CancelItemSaleEnable == false)
	{
		return;
	}

	Obj.m_PlayerData->m_bIsCancelItemSale = false;

	for (int i = 0; i < MAX_CANCEL_ITEMS_SALE; i++)
	{
		Obj.m_PlayerData->m_CancelItemSaleList[i]->Clear();
	}
}

void CCancelItemSale::CGReqReBuyItem(PMSG_REQ_REBUY_ITEM *lpMsg, CGameObject &Obj)
{
	if (ObjectMaxRange(Obj.m_Index) == false)
	{
		return;
	}

	if (gObjIsConnected(Obj.m_Index) == false)
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (g_ConfigRead.data.common.CancelItemSaleEnable == false)
	{
		return;
	}

	if (Obj.m_PlayerData->m_bIsCancelItemSale == false)
	{
		MsgOutput(iIndex, "Internal Error, Cancel Item Sale DB data doesn't exists.");
		return;
	}

	PMSG_ANS_REBUY_ITEM pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0x6F, 0x02, sizeof(pMsg));

	BYTE btItemNumber = 0xFF;

	for (int i = 0; i < MAX_CANCEL_ITEMS_SALE; i++)
	{
		if (Obj.m_PlayerData->m_CancelItemSaleList[i]->btUsed == TRUE)
		{
			if (Obj.m_PlayerData->m_CancelItemSaleList[i]->btItemNumber == lpMsg->btItemNumber &&
				Obj.m_PlayerData->m_CancelItemSaleList[i]->wItemCode == lpMsg->wItemCode &&
				Obj.m_PlayerData->m_CancelItemSaleList[i]->dwItemPrice == lpMsg->dwItemPrice &&
				Obj.m_PlayerData->m_CancelItemSaleList[i]->iLeftTime == lpMsg->dwLeftTime)
			{
				btItemNumber = i;
				break;
			}
		}
	}

	if (btItemNumber == 0xFF)
	{
		MsgOutput(iIndex, "Item doesn't exists in DB!");
		pMsg.btResult = 1;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	this->GDReqGetReBuyItem(iIndex, lpMsg->btItemNumber, lpMsg->dwItemPrice);
}

void CCancelItemSale::GDReqSoldItemList(CGameObject &Obj)
{
	SDHP_REQ_SHOP_REBUY_LIST pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0x6F, 0x00, sizeof(pMsg));

	pMsg.iIndex = iIndex;
	memcpy(pMsg.szAccountID, Obj.AccountID, MAX_ACCOUNT_LEN + 1);
	memcpy(pMsg.szName, Obj.Name, MAX_ACCOUNT_LEN + 1);
	pMsg.CurrTime = time(NULL);

	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
}

void CCancelItemSale::DGAnsSoldItemList(BYTE* lpRecv)
{
	SDHP_ANS_SHOP_REBUY_LIST * lpMsg = (SDHP_ANS_SHOP_REBUY_LIST *)lpRecv;
	SDHP_SHOP_REBUY_ITEM * lpItemList = (SDHP_SHOP_REBUY_ITEM *)(lpRecv + sizeof(SDHP_ANS_SHOP_REBUY_LIST));

	int iIndex = lpMsg->iIndex;

	if (ObjectMaxRange(Obj.m_Index) == false)
	{
		return;
	}

	if (gObjIsConnected(Obj.m_Index) == false)
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (lpMsg->btResult != 0)
	{
		if (lpMsg->btResult == 1)
		{
			gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 626), iIndex, 1);
		}

		else if (lpMsg->btResult == 2)
		{
			gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 625), iIndex, 1);
		}

		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 624), iIndex, 1);
	}

	else
	{
		Obj.m_PlayerData->m_bIsCancelItemSale = true;

		for (int i = 0; i < MAX_CANCEL_ITEMS_SALE; i++)
		{
			Obj.m_PlayerData->m_CancelItemSaleList[i]->Clear();
		}

		for (int i = 0; i < lpMsg->iCount; i++)
		{
			CItemObject Item;
			lpItemList = (SDHP_SHOP_REBUY_ITEM *)(lpRecv + sizeof(SDHP_ANS_SHOP_REBUY_LIST) + (i * sizeof(SDHP_SHOP_REBUY_ITEM)));

			this->MakeItem(lpItemList->ItemData, &Item);

			Obj.m_PlayerData->m_CancelItemSaleList[i]->btUsed = TRUE;
			Obj.m_PlayerData->m_CancelItemSaleList[i]->btItemNumber = lpItemList->btItemNumber;
			Obj.m_PlayerData->m_CancelItemSaleList[i]->wItemCode = Item.m_Type;
			Obj.m_PlayerData->m_CancelItemSaleList[i]->dwItemPrice = lpItemList->dwSellPrice;
			Obj.m_PlayerData->m_CancelItemSaleList[i]->iLeftTime = lpItemList->SellExpireDate - time(NULL);
			Obj.m_PlayerData->m_CancelItemSaleList[i]->wItemCount = lpItemList->wItemCount;

			ItemByteConvert(Obj.m_PlayerData->m_CancelItemSaleList[i]->btItemInfo, Item);
		}

		this->GCAnsSoldItemList(Obj.m_Index);
	}
}

void CCancelItemSale::GDReqAddItemToList(CGameObject &Obj, CItemObject Item, DWORD dwSellPrice)
{
	if (g_ConfigRead.data.common.CancelItemSaleEnable == false)
	{
		return;
	}

	SDHP_REQ_SHOP_REBUY_ADD_ITEM pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0x6F, 0x01, sizeof(pMsg));

	memcpy(pMsg.szAccountID, Obj.AccountID, MAX_ACCOUNT_LEN + 1);
	memcpy(pMsg.szName, Obj.Name, MAX_ACCOUNT_LEN + 1);
	pMsg.SellDate = time(NULL);
	pMsg.SellExpireDate = time(NULL) + g_ConfigRead.data.common.CancelItemSaleExpiryTime;
	pMsg.dwSellPrice = dwSellPrice * g_ConfigRead.data.common.CancelItemSaleMultipler / 100;
	ItemByteConvert32(pMsg.ItemData, &Item, 1);

	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
}

void CCancelItemSale::GDReqGetReBuyItem(CGameObject &Obj, BYTE btItemNumber, DWORD dwItemPrice)
{
	SDHP_REQ_SHOP_REBUY_GET_ITEM pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0x6F, 0x02, sizeof(pMsg));

	pMsg.iIndex = iIndex;
	memcpy(pMsg.szAccountID, Obj.AccountID, MAX_ACCOUNT_LEN + 1);
	memcpy(pMsg.szName, Obj.Name, MAX_ACCOUNT_LEN + 1);
	pMsg.btItemNumber = btItemNumber;
	pMsg.dwItemPrice = dwItemPrice;

	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
}

void CCancelItemSale::DGAnsGetReBuyItem(SDHP_ANS_SHOP_REBUY_GET_ITEM *lpMsg)
{
	int iIndex = lpMsg->iIndex;

	if (ObjectMaxRange(Obj.m_Index) == false)
	{
		return;
	}

	if (gObjIsConnected(Obj.m_Index) == false)
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	PMSG_ANS_REBUY_ITEM pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0x6F, 0x02, sizeof(pMsg));

	if (lpMsg->btResult != 0)
	{
		if (lpMsg->btResult == 1)
		{
			gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 626), iIndex, 1);
		}

		else if (lpMsg->btResult == 2)
		{
			gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 625), iIndex, 1);
		}

		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 624), iIndex, 1);
		pMsg.btResult = lpMsg->btResult;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	else
	{
		if (Obj.m_PlayerData->Money < lpMsg->dwSellPrice)
		{
			gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 623), iIndex, 1);
			pMsg.btResult = 1;
			IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
			return;
		}

		CItemObject Item;
		this->MakeItem(lpMsg->ItemData, &Item);

		LPITEM_ATTRIBUTE p = &ItemAttribute[Item.m_Type];

		if (CheckInventoryEmptySpace(Obj, p->Height * lpMsg->wItemCount, p->Width * lpMsg->wItemCount) == FALSE)
		{
			gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 622), iIndex, 1);
			pMsg.btResult = 1;
			IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
			return;
		}

		for (int i = 0; i < lpMsg->wItemCount; i++)
		{
			BYTE pos = gObjInventoryInsertItem(iIndex, Item);
			gGameProtocol.GCInventoryItemOneSend(iIndex, pos);
		}

		Obj.m_PlayerData->Money -= lpMsg->dwSellPrice;
		gGameProtocol.GCMoneySend(iIndex, Obj.m_PlayerData->Money);

		pMsg.btResult = 0;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		this->GDReqDeleteSoldItem(iIndex, lpMsg->btItemNumber, Item.m_Type, lpMsg->dwSellPrice);
	}

	this->GDReqSoldItemList(Obj.m_Index);
}

void CCancelItemSale::GDReqDeleteSoldItem(CGameObject &Obj, BYTE btItemNumber, WORD wItemCode, DWORD dwItemPrice)
{
	SDHP_REQ_SHOP_REBUY_DELETE_ITEM pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0x6F, 0x03, sizeof(pMsg));

	memcpy(pMsg.szAccountID, Obj.AccountID, MAX_ACCOUNT_LEN + 1);
	memcpy(pMsg.szName, Obj.Name, MAX_ACCOUNT_LEN + 1);
	pMsg.btItemNumber = btItemNumber;
	pMsg.wItemCode = wItemCode;
	pMsg.dwItemPrice = dwItemPrice;

	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
}

void CCancelItemSale::MakeItem(BYTE* lpData, CItemObject * lpItem)
{
	int itype;
	int _type;
	CItemObject &item;
	BYTE OptionData;
	WORD hiWord;
	WORD loWord;
	int check;
	bool bIsItemExist = true;

	bIsItemExist = true;
	lpItem->Clear();
	itype = lpData[DBI_TYPE];

	if (lpData[DBI_TYPE] == 0xFF
		&& (lpData[DBI_NOPTION_DATA] & 0x80) == 0x80
		&& (lpData[DBI_OPTION380_DATA] & 0xF0) == 0xF0)
	{
		return;
	}

	itype |= (lpData[DBI_OPTION380_DATA] & 0xF0) * 32;
	itype |= (lpData[DBI_NOPTION_DATA] & 0x80) * 2;
	_type = itype;

	if (lpData[DBI_TYPE] == 0xFF
		&& (lpData[DBI_NOPTION_DATA] & 0x80) == 0x80
		&& (lpData[DBI_OPTION380_DATA] & 0xF0) == 0xF0)
	{
		itype = -1;
	}

	if (_type == -1)
	{
		return;
	}

	if (IsItem(_type) == false)
	{
		bIsItemExist = false;
	}

	if (bIsItemExist == false)
	{
		lpItem->m_bItemExist = false;
		lpItem->Clear();
		return;
	}

	if (_type == ITEMGET(14, 17) || _type == ITEMGET(14, 18))
	{
		if (lpData[DBI_DUR] == 0)
		{
			itype = -1;
		}
	}

	if (_type == ITEMGET(13, 49) || _type == ITEMGET(13, 50))
	{
		if (lpData[DBI_DUR] == 0)
		{
			itype = -1;
		}
	}

	if (_type == ITEMGET(14, 19))
	{
		if (lpData[DBI_DUR] == (BYTE)-1)
		{
			itype = -1;
		}
	}

	if (itype != -1)
	{
		item.m_Level = DBI_GET_LEVEL(lpData[DBI_OPTION_DATA]);

		if (_type != ITEMGET(14, 11))
		{
			if (item.m_Level > 15)
			{
				item.m_Level = 0;
			}
		}

		OptionData = lpData[DBI_OPTION_DATA];
		item.m_Option1 = DBI_GET_SKILL(OptionData);
		item.m_Option2 = DBI_GET_LUCK(OptionData);
		item.m_Option3 = DBI_GET_OPTION(OptionData);
		item.m_Option3 |= DBI_GET_OPTION16(lpData[DBI_NOPTION_DATA]) >> 4;

		item.m_Durability = lpData[DBI_DUR];
		item.m_JewelOfHarmonyOption = lpData[DBI_JOH_DATA];
		item.m_ItemOptionEx = DBI_GET_380OPTION(lpData[DBI_OPTION380_DATA]);

		if (item.m_ItemOptionEx)
		{
			item.m_Type = itype;

			if (g_kItemSystemFor380.Is380Item(&item) == false)
			{
				item.m_ItemOptionEx = 0;
			}
		}

		BYTE SocketOption[5];
		BYTE SocketOptionIndex = 0xFF;
		memset(&SocketOption, 0xFF, 5);

		if (gSocketItemType.CheckSocketItemType(_type) == 1 || g_PentagramSystem.IsPentagramItem(_type) || g_PentagramSystem.IsPentagramJewel(_type))
		{
			for (int i = 0; i < 5; i++)
			{
				SocketOption[i] = lpData[(DBI_SOCKET_1)+i];
			}

			SocketOptionIndex = lpData[DBI_JOH_DATA];
		}

		else
		{
			for (int i = 0; i < 5; i++)
			{
				SocketOption[i] = lpData[(DBI_SOCKET_1)+i];

				if (SocketOption[i] == 0)
				{
					SocketOption[i] = 0xFF;
				}
			}

			SocketOptionIndex = 0;
		}

		item.m_PeriodItemOption = (lpData[DBI_OPTION380_DATA] & 6) >> 1;

		item.Convert(itype, item.m_Option1, item.m_Option2, item.m_Option3, DBI_GET_NOPTION(lpData[DBI_NOPTION_DATA]), lpData[DBI_SOPTION_DATA], item.m_ItemOptionEx, SocketOption, SocketOptionIndex, item.m_PeriodItemOption, CURRENT_DB_VERSION);

		if (_type == ITEMGET(14, 7))
		{

		}

		else
		{
			if (_type >= ITEMGET(14, 0) && _type <= ITEMGET(14, 8)
				|| _type >= ITEMGET(14, 38) && _type <= ITEMGET(14, 40))
			{
				if (item.m_Durability == 0.0f)
				{
					item.m_Durability = 1.0f;
				}
			}
			//Begin CashShop Items
			else if (_type == ITEMGET(14, 70) || _type == ITEMGET(14, 71))//cashshop
			{
				if (item.m_Durability == 0.0f)
				{
					item.m_Durability = 1.0f;
				}
			}

			if (_type == ITEMGET(14, 78) || _type == ITEMGET(14, 79)
				|| _type == ITEMGET(14, 80) || _type == ITEMGET(14, 81)
				|| _type == ITEMGET(14, 82))
			{
				if (item.m_Durability == 0.0f)
				{
					item.m_Durability = 1.0f;
				}
			}

			if (_type == ITEMGET(14, 63) || _type == ITEMGET(14, 64))
			{
				if (item.m_Durability == 0.0f)
				{
					item.m_Durability = 1.0f;
				}
			}
			//End CashShop Fucntions
			else if (_type >= ITEMGET(14, 46) && _type <= ITEMGET(14, 50))
			{
				if (item.m_Durability == 0.0f)
				{
					item.m_Durability = 1.0f;
				}
			}
			else if (_type >= ITEMGET(14, 35) && _type <= ITEMGET(14, 37))
			{
				if (item.m_Durability == 0.0f)
				{
					item.m_Durability = 1.0f;
				}
			}

			else if (_type == ITEMGET(13, 51))
			{
				if (item.m_Durability == 0.0f)
				{
					item.m_Durability = 1.0f;
				}
			}

			else if (_type != ITEMGET(13, 10) && _type != ITEMGET(14, 29) && _type != ITEMGET(14, 21))
			{
				if (item.m_Level == 3)
				{
					if (item.m_Durability > item.m_BaseDurability && bIsItemExist == 1)
					{
						item.m_Durability = item.m_BaseDurability;
					}
				}
			}
		}

		int iMaxOverlap = IsOverlapItem(item.m_Type);

		if (iMaxOverlap != 0)
		{
			if (item.m_Durability > iMaxOverlap)
			{
				item.m_Durability = iMaxOverlap;
			}
		}

		DWORD hidword = MAKE_NUMBERDW(MAKE_NUMBERW(lpData[DBI_SERIAL1], lpData[DBI_SERIAL2]), MAKE_NUMBERW(lpData[DBI_SERIAL3], lpData[DBI_SERIAL4]));
		DWORD lodword = MAKE_NUMBERDW(MAKE_NUMBERW(lpData[DBI_SERIAL5], lpData[DBI_SERIAL6]), MAKE_NUMBERW(lpData[DBI_SERIAL7], lpData[DBI_SERIAL8]));
		item.m_Number = MAKEQWORD(hidword, lodword);

		*lpItem = item;
	}
}


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

