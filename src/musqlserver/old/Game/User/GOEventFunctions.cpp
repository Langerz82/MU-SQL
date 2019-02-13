#include "GOEventFunctions.h"
#include "GOFunctions.h"

void gObjEventInventoryItemSet(CGameObject &lpObj, int itempos, BYTE set_byte)
{
	int width, height;

	if (itempos < 0 || itempos > EVENT_INVENTORY_SIZE - 1)
	{
		return;
	}
	if (lpObj.pEventInventory[itempos].GetSize(width, height) == 0)
	{
		sLog->outBasic("error %s %d", __FILE__, __LINE__);
		return;
	}

	gObjEventInventoryItemBoxSet(lpObj, itempos, width, height, set_byte);
}

void gObjEventInventoryItemBoxSet(CGameObject &lpObj, int itempos, int xl, int yl, BYTE set_byte)
{
	int itemposx = itempos % 8;
	int itemposy = itempos / 8;

	for (int y = 0; y < yl; y++)
	{
		for (int x = 0; x < xl; x++)
		{
			if (ExtentCheck(x + itemposx, y + itemposy, 8, 4) == TRUE)
			{
				*(BYTE*)(lpObj.pEventInventoryMap + (itemposy + y) * 8 + (itemposx + x)) = set_byte;
			}

			else
			{
				sLog->outBasic("error %s %d", __FILE__, __LINE__);
				return;
			}
		}
	}
}

bool gObjFixEventInventoryPointer(CGameObject &lpObj)
{
	if (gObjIsConnected(lpObj) == 0)
	{
		sLog->outBasic("[Fix Event Inv.Ptr] [%s][%s] - disconnected", lpObj.AccountID, lpObj.Name);
		return false;
	}

	if (lpObj.pEventInventory == lpObj.pEventInventory1)
	{
		return true;
	}

	if (lpObj.pEventInventory == lpObj.pEventInventory2)
	{
		if (lpObj.pTransaction == 1)
		{
			sLog->outBasic("[Fix Event Inv.Ptr] [%s][%s] - Transaction == 1, Do not change Pointer", lpObj.AccountID, lpObj.Name);
			return false;
		}
		else
		{
			sLog->outBasic("[Fix Event Inv.Ptr] [%s][%s] - Event Inventory Pointer was 2", lpObj.AccountID, lpObj.Name);

			for (int n = 0; n < INVENTORY_SIZE; n++)
			{
				lpObj.pEventInventory2[n].Clear();
			}
		}
	}
	else
	{
		sLog->outBasic("[Fix Event Inv.Ptr] [%s][%s] - Event Inventory Pointer was Wrong", lpObj.AccountID, lpObj.Name);
	}

	gObjSetEventInventory1Pointer(lpObj);

	return false;
}

bool gObjEventInventoryDeleteItem(CGameObject &lpObj, int itempos)
{
	gObjEventInventoryItemSet(lpObj, itempos, -1);
	lpObj.pEventInventory[itempos].Clear();

	return TRUE;
}

BYTE gObjEventInventoryInsertItemTemp(CGameObject &lpObj, CMapItem * Item)
{
	CItem item;
	int w, h, iwidth, iheight;
	BYTE blank = 0;

	if (Item->GetSize((int &)iwidth, (int &)iheight) == 0)
	{
		sLog->outBasic("error: Item does not exist %s %d", __FILE__, __LINE__);
		return -1;
	}

	for (h = 0; h < 4; h++)
	{
		for (w = 0; w < 8; w++)
		{
			if (*(BYTE*)(lpObj.pEventInventoryMap + h * 8 + w) == 255)
			{
				blank = gObjEventInventoryRectCheck(lpObj, w, h, iwidth, iheight);

				if (blank == 254)
				{
					goto GOTO_EndFunc;
				}

				if (blank != 255)
				{
					return blank;
				}
			}
		}
	}
GOTO_EndFunc:
	return -1;
}

BYTE gObjEventInventoryInsertItem(CGameObject &lpObj, CMapItem * item)
{
	int w, h, iwidth, iheight;
	BYTE blank = 0;

	CItem copyitem;

	if (item->GetSize((int &)iwidth, (int &)iheight) == 0)
	{
		return -1;
	}

	for (h = 0; h < 4; h++)
	{
		for (w = 0; w < 8; w++)
		{
			if (*(BYTE*)(lpObj.pEventInventoryMap + h * 8 + w) == 255)
			{
				blank = gObjEventInventoryRectCheck(lpObj, w, h, iwidth, iheight);
				if (blank == 254)
				{
					goto GOTO_EndFunc;
				}

				if (blank != 255)
				{
					copyitem.m_Level = item->m_Level;
					copyitem.m_Durability = item->m_Durability;

					copyitem.Convert(item->m_Type, item->m_Option1, item->m_Option2, item->m_Option3, item->m_NewOption, item->m_SetOption, item->m_ItemOptionEx, item->m_SocketOption, item->m_BonusSocketOption, 0, CURRENT_DB_VERSION);
					copyitem.SetPetItemInfo(item->m_PetItem_Level, item->m_PetItem_Exp);

					copyitem.m_Number = item->m_Number;

					lpObj.pEventInventory[blank] = copyitem;

					gObjEventInventoryItemSet(lpObj, blank, lpObj.pEventInventory[blank].m_Type);
					return blank;
				}
			}
		}
	}

GOTO_EndFunc:
	return -1;
}

BYTE gObjEventInventoryInsertItem(CGameObject &lpObj, CItem item)
{
	int w, h, iwidth, iheight;
	BYTE blank = 0;

	if (item.GetSize((int&)iwidth, (int&)iheight) == 0)
	{
		sLog->outBasic("error: Item does not exist %s %d", __FILE__, __LINE__);
		return -1;
	}

	for (h = 0; h < 4; h++)
	{
		for (w = 0; w < 8; w++)
		{
			if (*(BYTE*)(lpObj.pEventInventoryMap + h * 8 + w) == 255)
			{
				blank = gObjEventInventoryRectCheck(lpObj, w, h, iwidth, iheight);

				if (blank == 254)
				{
					goto GOTO_EndFunc;
				}

				if (blank != 255)
				{
					if (gObjCheckSerial0ItemList(&item) != 0)
					{
						MsgOutput(lpObj, Lang.GetText(0, 259));
						return -1;
					}

					if (gObjEventInventorySearchSerialNum(lpObj, item.m_Number) == 0)
					{
						return -1;
					}

					lpObj.pEventInventory[blank] = item;

					gObjEventInventoryItemSet(lpObj, blank, lpObj.pEventInventory[blank].m_Type);
					return blank;
				}
			}
		}
	}

GOTO_EndFunc:
	return -1;
}

BYTE gObjEventInvenItemOverlap(CGameObject &lpObj, int *durSsend, int *durTsend, BYTE source, BYTE target)
{
	CItemObject* sitem = &lpObj.pEventInventory[source];
	CItemObject* titem = &lpObj.pEventInventory[target];

	int max_count = 0;

	if (titem->IsItem() == FALSE)
	{
		return 0;
	}

	LPITEM_ATTRIBUTE pSItemAttribute = GetItemAttr(sitem->m_Type);
	LPITEM_ATTRIBUTE pTItemAttribute = GetItemAttr(titem->m_Type);

	if (!pSItemAttribute || !pTItemAttribute)
	{
		return 1;
	}

	if (pSItemAttribute->ItemKindA != 11 || pTItemAttribute->ItemKindA != 11)
	{
		return 1;
	}

	max_count = IsOverlapItem(sitem->m_Type);

	if (max_count && titem->m_Type == sitem->m_Type && titem->m_Level == sitem->m_Level && titem->m_Durability < max_count)
	{
		int sdur = max_count - titem->m_Durability;

		if (sitem->m_Durability < titem->m_Durability)
		{
			sdur = titem->m_Durability;
		}

		titem->m_Durability = sdur + titem->m_Durability;
		sitem->m_Durability -= sdur;

		if (titem->m_Type == ITEMGET(14, 215) && titem->m_Durability >= max_count)
		{
			titem->m_Durability -= max_count;

			if (titem->m_Durability == 0.0)
			{
				gObjEventInventoryItemSet(lpObj, target, -1);
				lpObj.pEventInventory[target].Clear();
				gGameProtocol.GCEventInventoryItemDeleteSend(lpObj, target, TRUE);
				// TODO - Send to player

				*durTsend = FALSE;
			}

			else
			{
				*durTsend = TRUE;
			}

			//ItemSerialCreateSend(lpObj, 226, lpObj.X, lpObj.Y, ITEMGET(14, 216), sitem->m_Level, 0, 0, 0, 0, lpObj.m_Index, 0, 0, 0, 0, 0);
			// TODO - Add to DB.

		}

		if (titem->m_Type == ITEMGET(14, 278) && titem->m_Durability >= max_count)
		{
			titem->m_Durability -= max_count;

			if (titem->m_Durability == 0.0)
			{
				gObjEventInventoryItemSet(lpObj, target, -1);
				lpObj.pEventInventory[target].Clear();
				gGameProtocol.GCEventInventoryItemDeleteSend(lpObj, target, TRUE); // TODO Send Player.

				*durTsend = FALSE;
			}

			else
			{
				*durTsend = TRUE;
			}

			ItemSerialCreateSend(lpObj, 226, lpObj.X, lpObj.Y, ITEMGET(14, 244), sitem->m_Level, 0, 0, 0, 0, lpObj.m_Index, 0, 0, 0, 0, 0);
			// TODO Add DB.
		}

		if (sitem->m_Durability <= 0.0)
		{
			gObjEventInventoryItemSet(lpObj, source, -1);
			sitem->Clear();
			gGameProtocol.GCEventInventoryItemDeleteSend(lpObj, source, FALSE); // TODO - Send Player.
			*durSsend = TRUE;
			return 21;
		}

		else
		{
			*durSsend = TRUE;
		}
	}

	return -1;
}
BYTE gObjEventInvenMove(CGameObject &lpObj, int *durSsend, int *durTsend, BYTE source, BYTE target)
{
	BYTE TempEventInventoryMap[32];

	if (source > EVENT_INVENTORY_SIZE - 1)
	{
		return -1;
	}

	if (target > EVENT_INVENTORY_SIZE - 1)
	{
		return -1;
	}

	if (source == target)
	{
		return -1;
	}

	BYTE btRet = gObjEventInvenItemOverlap(lpObj, durSsend, durTsend, source, target);

	if (btRet == 1)
	{
		return -1;
	}

	else if (btRet == 21 || btRet == 0xFF)
	{
		return btRet;
	}

	if (lpObj.pEventInventory[source].IsItem() == FALSE || lpObj.pEventInventory[target].IsItem() == TRUE)
	{
		return -1;
	}

	int width, height;

	lpObj.pEventInventory[source].GetSize(width, height);
	memcpy(&TempEventInventoryMap, lpObj.pEventInventoryMap, EVENT_INVENTORY_MAP_SIZE);
	gObjEventInventoryItemBoxSet(lpObj, source, width, height, -1);

	int w = target % 8;
	int h = target / 8;

	if (ExtentCheck(w, h, 8, 4) == FALSE)
	{
		memcpy(lpObj.pEventInventory, TempEventInventoryMap, EVENT_INVENTORY_MAP_SIZE);
		return -1;
	}

	if (*(BYTE*)(lpObj.pEventInventoryMap + h * 8 + w) != 0xFF)
	{
		memcpy(lpObj.pEventInventoryMap, TempEventInventoryMap, EVENT_INVENTORY_MAP_SIZE);
		return -1;
	}

	BYTE blank = gObjEventInventoryRectCheck(lpObj, w, h, width, height);

	if (blank == 0xFF)
	{
		memcpy(lpObj.pEventInventoryMap, TempEventInventoryMap, EVENT_INVENTORY_MAP_SIZE);
		return -1;
	}

	memcpy(&lpObj.pEventInventory[blank], &lpObj.pEventInventory[source], sizeof(lpObj.pEventInventory[blank]));
	lpObj.pEventInventory[source].Clear();
	gObjEventInventoryItemBoxSet(lpObj, blank, width, height, lpObj.pEventInventory[blank].m_Type);

	return 21;
}

BYTE gObjEventInventoryTradeMove(CGameObject &lpObj, BYTE source, BYTE target)
{
	int h, w, iwidth, iheight, s_num, blank;

	if (lpObj.m_bMapSvrMoveReq == true || lpObj.m_bMapSvrMoveQuit == true || lpObj.m_State == 32)
	{
		sLog->outError("[gObjEventInventoryTradeMove] Can't move item in inventory - MapServerMove");
		return -1;
	}

	if (source > EVENT_INVENTORY_SIZE)
	{
		sLog->outBasic("error : %s %d", __FILE__, __LINE__);
		return -1;
	}

	if (lpObj.pEventInventory[source].IsItem() == 0)
	{
		return -1;
	}

	if (lpObj.TargetNumber < 0)
	{
		return -1;
	}

	if (lpObj.m_IfState->use == 0 || lpObj.m_IfState->type != 1)
	{
		return -1;
	}

	LPITEM_ATTRIBUTE pItemAttribute = GetItemAttr(lpObj.pEventInventory[source].m_Type);

	if (pItemAttribute == NULL)
	{
		return -1;
	}

	if (pItemAttribute->ItemKindA != 11)
	{
		return -1;
	}

	if (!IsTransactionItem(lpObj.pEventInventory[source].m_Type))
	{
		return -1;
	}

	lpObj.pEventInventory[source].GetSize((int &)iwidth, (int &)iheight);
	s_num = lpObj.pEventInventory[source].GetNumber();

	if (gObjCheckSerial0ItemList(&lpObj.pEventInventory[source]) != 0)
	{
		MsgOutput(lpObj, Lang.GetText(0, 259));
		sLog->outBasic("[ANTI-HACK][Serial 0 Item] [Trade] (%s)(%s) Item(%s) Pos(%d)", lpObj.AccountID, lpObj.Name, lpObj.pEventInventory[source].GetName(), source);
		return -1;
	}

	if (gObjEventInventorySearchSerialNum(lpObj, s_num) == 0)
	{
		return -1;
	}

	w = target % 8;
	h = target / 8;

	if (ExtentCheck(w, h, 8, 4) == 0)
	{
		return -1;
	}

	if (*(BYTE*)(lpObj.TradeMap + h * 8 + w) == 255)
	{
		BYTE itembuf[13];
		blank = gObjTradeRectCheck(lpObj, w, h, iwidth, iheight);

		if (blank == 255)
		{
			return -1;
		}

		lpObj.Trade[blank] = lpObj.pEventInventory[source];
		gObjEventInventoryDeleteItem(lpObj, source);
		gObjTradeItemBoxSet(lpObj, blank, iwidth, iheight, lpObj.Trade[blank].m_Type);
		ItemByteConvert(itembuf, lpObj.Trade[blank]);
		gGameProtocol.GCTradeOtherAdd(lpObj, blank, itembuf); // TODO Send Player.
		return true;
	}

	return -1;
}

BYTE gObjTradeEventInventoryMove(CGameObject &lpObj, BYTE source, BYTE target)
{
	int iwidth, iheight;
	UINT64 s_num;
	BYTE itembuf[13];

	if (source > TRADE_BOX_SIZE)
	{
		sLog->outBasic("error : %s %d", __FILE__, __LINE__);
		return -1;
	}

	if (target > EVENT_INVENTORY_SIZE)
	{
		sLog->outBasic("error : %s %d", __FILE__, __LINE__);
		return -1;
	}

	if (lpObj.TargetNumber < 0)
	{
		return -1;
	}

	if (lpObj.Trade[source].IsItem() == 0)
	{
		return -1;
	}

	if (IsEventItem(lpObj.Trade[source].m_Type) == FALSE)
	{
		return -1;
	}

	if (lpObj.m_IfState->use == 0 || lpObj.m_IfState->type != 1)
	{
		return -1;
	}

	if (gObjEventInventoryInsertItemPos(lpObj, lpObj.Trade[source], target, 1) == 255)
	{
		return -1;
	}

	s_num = 0;
	s_num = lpObj.Trade[source].m_Number;

	lpObj.Trade[source].GetSize((int &)iwidth, (int &)iheight);

	gObjTradeItemBoxSet(lpObj, source, iwidth, iheight, 255);
	lpObj.Trade[source].Clear();

	ItemByteConvert(itembuf, lpObj.Trade[source]);

	gGameProtocol.GCTradeOtherDel(lpObj, source); // TODO Send Player

	if (gObjCheckSerial0ItemList(&lpObj.Trade[source]) != 0)
	{
		MsgOutput(lpObj, Lang.GetText(0, 259));
		sLog->outBasic("[ANTI-HACK][Serial 0 Item] [Trade] (%s)(%s) Item(%s) Pos(%d)", lpObj.AccountID, lpObj.Name, lpObj.Trade[source].GetName(), source);
		return -1;
	}

	if (gObjEventInventorySearchSerialNum(lpObj, s_num) == 0)
	{
		return -1;
	}

	return 21;
}

void gObjTempEventInventoryItemBoxSet(BYTE * TempMap, int itempos, int xl, int yl, BYTE set_byte)
{
	int itemposx = itempos % 8;
	int itemposy = itempos / 8;

	for (int y = 0; y < yl; y++)
	{
		for (int x = 0; x < xl; x++)
		{
			*(BYTE*)(TempMap + (itemposy + y) * 8 + (itemposx + x)) = set_byte;
		}
	}
}

BYTE gObjTempEventInventoryRectCheck(CGameObject &lpObj, BYTE * TempMap, int sx, int sy, int width, int height)
{
	int x, y, blank = 0;

	if (sx + width > 8)
	{
		return -1;
	}

	if (sy + height > 4)
	{
		return -2;
	}

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			if (ExtentCheck(sx + x, sy + y, 8, 4) && *(BYTE*)(TempMap + (sy + y) * 8 + (sx + x)) != 255)
			{
				return -1;
			}
		}
	}

	if (blank == 0)
	{
		return sx + sy * 8;
	}

	return -1;
}

BYTE gObjTempEventInventoryInsertItem(CGameObject &lpObj, CItem item, BYTE * TempMap)
{
	int w, h, iwidth, iheight;
	BYTE blank = 0;

	if (item.GetSize(iwidth, iheight) == 0)
	{
		sLog->outBasic("Item does not exist [%s][%d] %d", __FILE__, __LINE__, item.m_Type);
		return -1;
	}

	for (h = 0; h < 4; h++)
	{
		for (w = 0; w < 8; w++)
		{
			if (*(BYTE*)(TempMap + h * 8 + w) == 255)
			{
				blank = gObjTempEventInventoryRectCheck(lpObj, TempMap, w, h, iwidth, iheight);

				if (blank == 254)
				{
					goto GOTO_EndFunc;
				}
				if (blank != 255)
				{
					gObjTempEventInventoryItemBoxSet(TempMap, blank, iwidth, iheight, item.m_Type);
					return blank;
				}
			}
		}
	}
GOTO_EndFunc:
	return -1;
}

BOOL CheckEventInventoryEmptySpace(CGameObject &lpObj, int iItemHeight, int iItemWidth)
{
	int h = 0;
	int w = 0;
	BYTE blank = 0;

	for (h = 0; h < 4; h++)
	{
		for (w = 0; w < 8; w++)
		{
			if (*(BYTE*)(lpObj.pEventInventoryMap + (h) * 8 + (w)) == 255)
			{
				blank = gObjEventInventoryRectCheck(lpObj, w, h, iItemWidth, iItemHeight);

				if (blank == 0xFE)
					return FALSE;

				if (blank != 0xFF)
					return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL IsEventItem(CItem item)
{
	LPITEM_ATTRIBUTE pItemAttribute = GetItemAttr(item.m_Type);

	if (!pItemAttribute)
	{
		return FALSE;
	}

	return pItemAttribute->ItemKindA == 11;
}

BOOL IsEventItem(int iType)
{
	LPITEM_ATTRIBUTE pItemAttribute = GetItemAttr(iType);

	if (!pItemAttribute)
	{
		return FALSE;
	}

	return pItemAttribute->ItemKindA == 11;
}

BYTE gObjEventInventoryRectCheck(CGameObject &lpObj, int sx, int sy, int width, int height)
{
	int x, y;
	int blank = 0;

	if (sx + width > 8)
	{
		return -1;
	}

	if (sy + height > 4)
	{
		return -2;
	}

	int xx, yy;

	for (y = 0; y < height; y++)
	{
		yy = sy + y;

		for (x = 0; x < width; x++)
		{
			xx = sx + x;

			if (ExtentCheck(xx, yy, 8, 4))
			{
				if (*(BYTE*)(lpObj.pEventInventoryMap + (sy + y) * 8 + (sx + x)) != 255)
				{
					return -1;
				}
			}
			else
			{
				sLog->outBasic("error : %s %d", __FILE__, __LINE__);
				return -1;
			}
		}
	}

	if (blank == 0)
	{
		return sx + sy * 8;
	}

	return  -1;
}

BYTE gObjEventInventoryInsertItemPos(CGameObject &lpObj, CItem item, int pos, BOOL RequestCheck)
{
	if (pos < 0 || pos > EVENT_INVENTORY_SIZE)
	{
		return -1;
	}

	BYTE TempInventoryMap[EVENT_INVENTORY_MAP_SIZE];

	int w, h, iwidth, iheight;

	int blank, useClass = 0;

	if (lpObj.pEventInventory[pos].IsItem() == 1)
	{
		return -1;
	}

	if (item.IsItem() == 0)
	{
		return -1;
	}

	w = pos % 8;
	h = pos / 8;

	item.GetSize((int &)iwidth, (int &)iheight);

	if (!ExtentCheck(w, h, 8, 4))
	{
		return -1;
	}

	memcpy(TempInventoryMap, lpObj.pEventInventoryMap, EVENT_INVENTORY_MAP_SIZE);

	if (*(BYTE*)(lpObj.pEventInventoryMap + h * 8 + w) != 255)
	{
		memcpy(lpObj.pEventInventoryMap, TempInventoryMap, EVENT_INVENTORY_MAP_SIZE);
		return -1;
	}

	blank = gObjEventInventoryRectCheck(lpObj, w, h, iwidth, iheight);

	if (blank >= 254)
	{
		memcpy(lpObj.pEventInventoryMap, TempInventoryMap, EVENT_INVENTORY_MAP_SIZE);
		return false;
	}

	lpObj.pEventInventory[pos] = item;
	gObjEventInventoryItemSet(lpObj, pos, 1);

	return pos;
}

BOOL gObjEventInventorySearchSerialNum(CGameObject &lpObj, UINT64 serial)
{
	UINT64 s_num;
	int count = 0;

	if (g_ConfigRead.ItemSerialCheck == 0)
	{
		return true;
	}

	if (serial == 0 || serial == (UINT64)-1)
	{
		return true;
	}

	for (int n = 0; n < EVENT_INVENTORY_SIZE; n++)
	{
		s_num = lpObj.pEventInventory[n].GetNumber();

		if (s_num != 0 && s_num == serial && s_num != (UINT64)-1)
		{
			count++;
		}
	}

	if (count <= 1)
	{
		return true;
	}

	for (int n = 0; n < EVENT_INVENTORY_SIZE; n++)
	{
		s_num = lpObj.pEventInventory[n].GetNumber();

		if (s_num != 0 && s_num == serial && s_num != (UINT64)-1)
		{
			sLog->outBasic("error-L1: CopyItem Id[%s] Char[%s] Item[%s] InventoryPos[%d] serial[%I64d]", lpObj.AccountID, lpObj.Name, lpObj.pEventInventory[n].GetName(), n, s_num);
			gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 15), lpObj, 1); // TODO Send to game server.
			gObjUserKill(lpObj);
		}
	}

	return false;
}

void gObjSetEventInventory1Pointer(CGameObject &lpObj)
{
	lpObj.pEventInventory = lpObj.pEventInventory1;
	lpObj.pEventInventoryMap = lpObj.pEventInventoryMap1;
}

void gObjSetEventInventory2Pointer(CGameObject &lpObj)
{
	lpObj.pEventInventory = lpObj.pEventInventory2;
	lpObj.pEventInventoryMap = lpObj.pEventInventoryMap2;
}

