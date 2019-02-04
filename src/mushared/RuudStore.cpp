////////////////////////////////////////////////////////////////////////////////
// RuudStore.cpp
#include "stdafx.h"
#include "RuudStore.h"
#include "TLog.h"
#include "ItemSocketOptionSystem.h"
#include "PentagramSystem.h"
#include "MuunSystem.h"
#include "SetItemOption.h"
#include "winutil.h"
#include "SocketItemType.h"
#include "LogToFile.h"
#include "configread.h"

CRuudStore g_RuudStore;

CRuudStore::CRuudStore()
{
}

CRuudStore::~CRuudStore()
{
}

void CRuudStore::LoadFile(char * szFile)
{
	this->m_mapItemList.clear();

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(szFile);

	if (res.status != pugi::status_ok)
	{
		g_Log.MsgBox("Error loading %s file (%s)", szFile, res.description());
		return;
	}

	pugi::xml_node main = file.child("RuudShop");
	this->m_bEnable = main.attribute("Enable").as_bool();
	this->m_iShopTime = main.attribute("ShopTime").as_int();

	int iCount = 0;

	for (pugi::xml_node character = main.child("Character"); character; character = character.next_sibling())
	{
		BYTE btClass = character.attribute("Class").as_int();

		for (pugi::xml_node item = character.child("Item"); item; item = item.next_sibling())
		{
			RUUD_SHOP_ITEM m_Item;

			m_Item.btClass = btClass;
			m_Item.btCat = item.attribute("Cat").as_int();
			m_Item.wIndex = item.attribute("Index").as_int();
			m_Item.btLevel = item.attribute("Level").as_int();
			m_Item.btSkill = item.attribute("Skill").as_int();
			m_Item.btLuck = item.attribute("Luck").as_int();
			m_Item.btOption = item.attribute("Option").as_int();
			
			std::string szExcOptions = item.attribute("Exc").as_string();
			std::deque<int> exc_opt_list;
			strtk::parse(szExcOptions, ";", exc_opt_list);

			memset(m_Item.btExcOption, -1, sizeof(m_Item.btExcOption));

			for (int i = 0; i < exc_opt_list.size(); i++)
			{
				m_Item.btExcOption[i] = exc_opt_list.at(i);
			}

			m_Item.btSetOption = item.attribute("SetItem").as_int();
			m_Item.btSocketCount = item.attribute("SocketCount").as_int();
			m_Item.btElemental = item.attribute("Element").as_int();
			m_Item.dwRuudPrice = item.attribute("Price").as_int();

			this->AddItemToList(iCount, &m_Item);
			iCount++;
		}
	}
}

bool CRuudStore::NpcTalk(OBJECTSTRUCT * lpNpc, OBJECTSTRUCT * lpObj)
{
	if (!gObjIsConnected(lpObj->m_Index))
		return false;

	if (lpObj->m_IfState.use > 0)
		return false;

	lpObj->TargetNpcNumber = lpNpc->m_Index;
	lpObj->m_IfState.use = 1;
	lpObj->m_IfState.type = 3;
	lpObj->m_ShopTime = 0;

	PMSG_TALKRESULT pResult;

	pResult.h.c = 0xC3;
	pResult.h.headcode = 0x30;
	pResult.h.size = sizeof(pResult);
	pResult.result = 0x35;

	IOCP.DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);

	PMSG_SHOPITEMCOUNT pShopItemCount;
	BYTE SendByte[4096];
	int lOfs = sizeof(pShopItemCount);

	PHeadSetW((LPBYTE)&pShopItemCount, 0x31, 0);
	pShopItemCount.count = 0;

	for (std::map<int, RUUD_MAP_ITEM_DATA>::iterator It = this->m_mapItemList.begin(); It != this->m_mapItemList.end(); It++)
	{
		if (It->second.btClass == lpObj->Class)
		{
			SendByte[lOfs] = It->first;
			lOfs++;
			ItemByteConvert(&SendByte[lOfs], It->second.Item);
			lOfs += MAX_ITEM_INFO;
			pShopItemCount.count++;
		//	g_Log.Add("[Ruud Shop] Class - %d", It->second.btClass);
		//	break;
		}
	}

	pShopItemCount.Type = 23;
	pShopItemCount.h.sizeH = HIBYTE(lOfs);
	pShopItemCount.h.sizeL = LOBYTE(lOfs);
	memcpy(SendByte, &pShopItemCount, sizeof(pShopItemCount));

	IOCP.DataSend(lpObj->m_Index, SendByte, lOfs);
}

void CRuudStore::CGReqBuyItem(PMSG_REQ_RUUD_STORE_BUYITEM * lpMsg, int iIndex)
{
	CLogToFile g_RuudLog("RUUD_BUYITEM", ".\\RUUD_BUYITEM", 1);

	if (!ObjectMaxRange(iIndex))
	{
		return;
	}

	if (!gObjIsConnected(iIndex))
	{
		return;
	}

	if (gObj[iIndex].Type != OBJ_USER)
	{
		return;
	}

	LPOBJ lpObj = &gObj[iIndex];
	int Option2 = 0;

	PMSG_ANS_RUUD_STORE_BUYITEM pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xD0, 0xF0, sizeof(pMsg));

	if (lpObj->CloseType != -1)
	{
		pMsg.btResult = 0xFF;
		IOCP.DataSend(iIndex, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	if (lpObj->TargetNpcNumber == -1)
	{
		pMsg.btResult = 0xFF;
		IOCP.DataSend(iIndex, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	if (!::gObjFixInventoryPointer(iIndex))
	{
		g_Log.Add("[Fix Inv.Ptr] False Location - %s, %d", __FILE__, __LINE__);
	}

	if (lpObj->m_ShopTime == 0)
		lpObj->m_ShopTime = 1;

	if (lpObj->m_ShopTime > this->m_iShopTime)
	{
		if (lpObj->m_IfState.use != 0 && lpObj->m_IfState.type == 3)
		{
			lpObj->TargetNpcNumber = -1;
			lpObj->m_IfState.use = 0;
			lpObj->m_IfState.type = 0;
		}

		pMsg.btResult = 0xFE;
		IOCP.DataSend(iIndex, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	if (lpObj->m_IfState.use > 0)
	{
		if (lpObj->m_IfState.type != 3)
		{
			pMsg.btResult = 0xFF;
			IOCP.DataSend(iIndex, (LPBYTE)&pMsg, pMsg.h.size);
			return;
		}
	}

	std::map<int, RUUD_MAP_ITEM_DATA>::iterator It = this->m_mapItemList.find(lpMsg->btPos);

	if (It == this->m_mapItemList.end())
	{
		pMsg.btResult = 0xFD;
		IOCP.DataSend(iIndex, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	if (lpObj->m_PlayerData->Ruud < It->second.dwRuudPrice)
	{
		pMsg.btResult = 0xFC;
		IOCP.DataSend(iIndex, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	int w, h;
	It->second.Item.GetSize(w, h);

	if (CheckInventoryEmptySpace(lpObj, h, w) == FALSE)
	{
		pMsg.btResult = 0xFB;
		IOCP.DataSend(iIndex, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	lpObj->m_PlayerData->Ruud -= It->second.dwRuudPrice;

	GSProtocol.GCSendRuud(iIndex, lpObj->m_PlayerData->Ruud, 0, false);

	if (It->second.Item.m_Type == ITEMGET(5, 41) || It->second.Item.m_Type == ITEMGET(3, 19) || It->second.Item.m_Type == ITEMGET(0, 46) || It->second.Item.m_Type == ITEMGET(5, 43) || It->second.Item.m_Type == ITEMGET(2, 22) || It->second.Item.m_Type == ITEMGET(0, 42) || It->second.Item.m_Type == ITEMGET(4, 28) || It->second.Item.m_Type == ITEMGET(4, 34) || It->second.Item.m_Type == ITEMGET(5, 41) || It->second.Item.m_Type == ITEMGET(0, 44) )
	{
		BYTE randLuck = rand() % 100 < g_ConfigRead.RandomBuyRuudLuck;
		BYTE randd = rand() % 25;
		BYTE newopt = 0x20 + randd;

		if (newopt == 0x27) //
			newopt++;
		if (newopt == 0x2B)
			newopt++;
		if (newopt >= 0x2D && newopt <= 0x2F)
			newopt = 0x30;
		if (newopt >= 0x31 && newopt <= 0x37)
			newopt = 0x38;
		BYTE ran = rand() % 1;
		It->second.Item.m_NewOption = newopt;
		It->second.Item.m_Option2 = randLuck;
		It->second.Item.m_Option1 = 1;
	}
	if (It->second.Item.m_Type == ITEMGET(14, 390))
	{
		It->second.Item.m_Durability = 5;
	}

	ItemSerialCreateSend(iIndex, 217, 0, 0, It->second.Item.m_Type, It->second.Item.m_Level, It->second.Item.m_Durability, It->second.Item.m_Option1, It->second.Item.m_Option2, It->second.Item.m_Option3, iIndex,
		It->second.Item.m_NewOption, It->second.Item.m_SetOption, 0, It->second.Item.m_SocketOption, It->second.Item.m_BonusSocketOption);
	g_RuudLog.Output("Character:%s ,Type:%d ,Level:%d ,Dur:%d ,Skill:%d ,Luck:%d ,Option:%d ,Exc:%d ,SetOpt:%d ", lpObj->Name, It->second.Item.m_Type, It->second.Item.m_Level, It->second.Item.m_Durability, It->second.Item.m_Option1, lpItemList.btLuck, It->second.Item.m_Option3,It->second.Item.m_NewOption, It->second.Item.m_SetOption);
}

void CRuudStore::AddItemToList(int iPos, RUUD_SHOP_ITEM * lpItem)
{
	if (this->m_mapItemList.find(iPos) != this->m_mapItemList.end())
	{
		g_Log.MsgBox("[Ruud Shop] Error: Item already exists (Guid:%d)", iPos);
		return;
	}

	RUUD_MAP_ITEM_DATA m_Data;
	m_Data.Item.Clear();

	ITEM_ATTRIBUTE * p = GetItemAttr(ITEMGET(lpItem->btCat, lpItem->wIndex));

	if (p == NULL)
	{
		g_Log.MsgBox("[Ruud Shop] Error: Item Not Found (Guid:%d) (ItemID:%d)", iPos, ITEMGET(lpItem->btCat, lpItem->wIndex));
		return;
	}

	m_Data.Item.m_Type = ITEMGET(lpItem->btCat, lpItem->wIndex);
	m_Data.Item.m_Level = lpItem->btLevel;
	m_Data.Item.m_Option1 = lpItem->btSkill;
	m_Data.Item.m_Option2 = lpItem->btLuck;
	m_Data.Item.m_Option3 = lpItem->btOption;

	if (lpItem->btSetOption != 0)
	{
		m_Data.Item.m_SetOption = gSetItemOption.GenSetOption(m_Data.Item.m_Type);
	}

	if (lpItem->btExcOption[0] == (BYTE)-2)
	{
		if (p->ItemKindA == ITEM_KIND_A_WING)
		{
			m_Data.Item.m_NewOption = g_ItemOptionTypeMng.WingExcOptionRand(p->ItemKindA, p->ItemKindB, m_Data.Item.m_SocketOption);
		}

		else
		{
			m_Data.Item.m_NewOption = g_ItemOptionTypeMng.CommonExcOptionRand(p->ItemKindA, m_Data.Item.m_SocketOption);
		}
	}

	else if (lpItem->btExcOption[0] == (BYTE)-3)
	{
		if (p->ItemKindA == ITEM_KIND_A_WING)
		{
			m_Data.Item.m_NewOption = g_ItemOptionTypeMng.WingExcOptionGetCount(p->ItemKindA, p->ItemKindB, m_Data.Item.m_SocketOption, lpItem->btExcOption[1]);
		}

		else
		{
			m_Data.Item.m_NewOption = g_ItemOptionTypeMng.CommonExcOptionGetCount(p->ItemKindA, m_Data.Item.m_SocketOption, lpItem->btExcOption[1]);
		}
	}

	else
	{
		for (int i = 0; i < 9; i++)
		{
			if (lpItem->btExcOption[i] == -1)
			{
				continue;
			}

			if (lpItem->btExcOption[i] >= 6)
			{
				bool bOptionFound = false;

				for (int j = 0; j < 5; j++)
				{
					if (m_Data.Item.m_SocketOption[j] == lpItem->btExcOption[i])
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
					if (m_Data.Item.m_SocketOption[j] == 0xFF)
					{
						m_Data.Item.m_SocketOption[j] = lpItem->btExcOption[i];
						break;
					}
				}
			}

			else
			{
				m_Data.Item.m_NewOption |= (1 << (5 - lpItem->btExcOption[i]));
			}
		}
	}

	if (gSocketItemType.CheckSocketItemType((int)&m_Data.Item) == true)
	{
		if (lpItem->btSocketCount == 0)
		{
			g_SocketOptionSystem.MakeSocketSlot(&m_Data.Item, rand() % 5 + 1);
		}

		else
		{
			g_SocketOptionSystem.MakeSocketSlot(&m_Data.Item, lpItem->btSocketCount);
		}

		m_Data.Item.m_BonusSocketOption = 0xFF;
	}

	else if (g_PentagramSystem.IsPentagramItem(m_Data.Item.m_Type) == true)
	{
		if (lpItem->btSocketCount == 0)
		{
			g_PentagramSystem.GetMakePentagramSlotCountNKind(m_Data.Item.m_SocketOption, m_Data.Item.m_Type);
		}

		else
		{
			BYTE Slot[5] = { 0, 0, 0, 0, 0 };

			for (int i = 0; i < lpItem->btSocketCount; i++)
			{
				Slot[i] = 1;
			}

			g_PentagramSystem.MakePentagramSocketSlot(&m_Data.Item, Slot[0], Slot[1], Slot[2], Slot[3], Slot[4]);
		}

		m_Data.Item.m_BonusSocketOption = lpItem->btElemental | 0x10;
	}

	m_Data.Item.Convert(m_Data.Item.m_Type, m_Data.Item.m_Option1, m_Data.Item.m_Option2, m_Data.Item.m_Option3, m_Data.Item.m_NewOption, m_Data.Item.m_SetOption, 0, m_Data.Item.m_SocketOption, m_Data.Item.m_BonusSocketOption, 0, CURRENT_DB_VERSION);
	m_Data.btClass = lpItem->btClass;

	m_Data.dwRuudPrice = lpItem->dwRuudPrice;
	//g_Log.Add("[Ruud Shop] Class #1 - %d", m_Data.btClass);
	this->m_mapItemList.insert(std::make_pair(iPos, m_Data));
}


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

