#include "GOEventFunctions.h"
#include "GOFunctions.h"

void gObjEventInventoryItemSet(CGameObject &Obj, int itempos, BYTE set_byte)
{
	int width, height;

	if (itempos < 0 || itempos > EVENT_INVENTORY_SIZE - 1)
	{
		return;
	}
	if (Obj.pntEventInventory[itempos]->GetSize(width, height) == 0)
	{
		sLog->outBasic("error %s %d", __FILE__, __LINE__);
		return;
	}

	gObjEventInventoryItemBoxSet(Obj, itempos, width, height, set_byte);
}

void gObjEventInventoryItemBoxSet(CGameObject &Obj, int itempos, int xl, int yl, BYTE set_byte)
{
	int itemposx = itempos % 8;
	int itemposy = itempos / 8;

	for (int y = 0; y < yl; y++)
	{
		for (int x = 0; x < xl; x++)
		{
			if (ExtentCheck(x + itemposx, y + itemposy, 8, 4) == TRUE)
			{
				*(BYTE*)(Obj.EventInventoryMap + (itemposy + y) * 8 + (itemposx + x)) = set_byte;
			}

			else
			{
				sLog->outBasic("error %s %d", __FILE__, __LINE__);
				return;
			}
		}
	}
}

bool gObjFixEventInventoryPointer(CGameObject &Obj)
{
	if (gObjIsConnected(Obj) == 0)
	{
		sLog->outBasic("[Fix Event Inv.Ptr] [%s][%s] - disconnected", Obj.AccountID, Obj.Name);
		return false;
	}

	if (Obj.pntEventInventory == Obj.pntEventInventory1)
	{
		return true;
	}

	if (Obj.pntEventInventory == Obj.pntEventInventory2)
	{
		if (Obj.pTransaction == 1)
		{
			sLog->outBasic("[Fix Event Inv.Ptr] [%s][%s] - Transaction == 1, Do not change Pointer", Obj.AccountID, Obj.Name);
			return false;
		}
		else
		{
			sLog->outBasic("[Fix Event Inv.Ptr] [%s][%s] - Event Inventory Pointer was 2", Obj.AccountID, Obj.Name);

			for (int n = 0; n < INVENTORY_SIZE; n++)
			{
				Obj.pntEventInventory2[n]->Clear();
			}
		}
	}
	else
	{
		sLog->outBasic("[Fix Event Inv.Ptr] [%s][%s] - Event Inventory Pointer was Wrong", Obj.AccountID, Obj.Name);
	}

	gObjSetEventInventory1Pointer(Obj);

	return false;
}

bool gObjEventInventoryDeleteItem(CGameObject &Obj, int itempos)
{
	gObjEventInventoryItemSet(Obj, itempos, -1);
	Obj.pntEventInventory[itempos]->Clear();

	return TRUE;
}

BYTE gObjEventInventoryInsertItemTemp(CGameObject &Obj, CMapItem * Item)
{
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
			if (*(BYTE*)(Obj.EventInventoryMap + h * 8 + w) == 255)
			{
				blank = gObjEventInventoryRectCheck(Obj, w, h, iwidth, iheight);

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

BYTE gObjEventInventoryInsertItem(CGameObject &Obj, CMapItem * item)
{
	int w, h, iwidth, iheight;
	BYTE blank = 0;

	CItemObject* copyitem = new CItemObject;

	if (item->GetSize((int &)iwidth, (int &)iheight) == 0)
	{
		return -1;
	}

	for (h = 0; h < 4; h++)
	{
		for (w = 0; w < 8; w++)
		{
			if (*(BYTE*)(Obj.EventInventoryMap + h * 8 + w) == 255)
			{
				blank = gObjEventInventoryRectCheck(Obj, w, h, iwidth, iheight);
				if (blank == 254)
				{
					goto GOTO_EndFunc;
				}

				if (blank != 255)
				{
					copyitem->m_Level = item->m_Level;
					copyitem->m_Durability = item->m_Durability;

					copyitem->Convert(item->m_Type, item->m_Option1, item->m_Option2, item->m_Option3, item->m_NewOption, item->m_SetOption, item->m_ItemOptionEx, item->m_SocketOption, item->m_BonusSocketOption, 0, CURRENT_DB_VERSION);
					copyitem->SetPetItemInfo(item->m_PetItem_Level, item->m_PetItem_Exp);

					copyitem->m_Number = item->m_Number;

					Obj.pntEventInventory[blank] = copyitem;

					gObjEventInventoryItemSet(Obj, blank, Obj.pntEventInventory[blank]->m_Type);
					return blank;
				}
			}
		}
	}

GOTO_EndFunc:
	return -1;
}

BYTE gObjEventInventoryInsertItem(CGameObject &Obj, CItemObject &item)
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
			if (*(BYTE*)(Obj.EventInventoryMap + h * 8 + w) == 255)
			{
				blank = gObjEventInventoryRectCheck(Obj, w, h, iwidth, iheight);

				if (blank == 254)
				{
					goto GOTO_EndFunc;
				}

				if (blank != 255)
				{
					if (gObjCheckSerial0ItemList(&item) != 0)
					{
						MsgOutput(Obj, Lang.GetText(0, 259));
						return -1;
					}

					if (gObjEventInventorySearchSerialNum(Obj, item.m_Number) == 0)
					{
						return -1;
					}

					Obj.pntEventInventory[blank] = &item;

					gObjEventInventoryItemSet(Obj, blank, Obj.pntEventInventory[blank]->m_Type);
					return blank;
				}
			}
		}
	}

GOTO_EndFunc:
	return -1;
}

BYTE gObjEventInvenItemOverlap(CGameObject &Obj, int *durSsend, int *durTsend, BYTE source, BYTE target)
{
	CItemObject* sitem = Obj.pntEventInventory[source];
	CItemObject* titem = Obj.pntEventInventory[target];

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
				gObjEventInventoryItemSet(Obj, target, -1);
				Obj.pntEventInventory[target]->Clear();
				gGameProtocol.GCEventInventoryItemDeleteSend(Obj, target, TRUE);
				// TODO - Send to player

				*durTsend = FALSE;
			}

			else
			{
				*durTsend = TRUE;
			}

			//ItemCreate(Obj, 226, Obj.X, Obj.Y, ITEMGET(14, 216), sitem->m_Level, 0, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
			// TODO - Add to DB.

		}

		if (titem->m_Type == ITEMGET(14, 278) && titem->m_Durability >= max_count)
		{
			titem->m_Durability -= max_count;

			if (titem->m_Durability == 0.0)
			{
				gObjEventInventoryItemSet(Obj, target, -1);
				Obj.pntEventInventory[target]->Clear();
				gGameProtocol.GCEventInventoryItemDeleteSend(Obj, target, TRUE); // TODO Send Player.

				*durTsend = FALSE;
			}

			else
			{
				*durTsend = TRUE;
			}

			ItemCreate(Obj, 226, Obj.X, Obj.Y, ITEMGET(14, 244), sitem->m_Level, 0, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
			// TODO Add DB.
		}

		if (sitem->m_Durability <= 0.0)
		{
			gObjEventInventoryItemSet(Obj, source, -1);
			sitem->Clear();
			gGameProtocol.GCEventInventoryItemDeleteSend(Obj, source, FALSE); // TODO - Send Player.
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
BYTE gObjEventInvenMove(CGameObject &Obj, int *durSsend, int *durTsend, BYTE source, BYTE target)
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

	BYTE btRet = gObjEventInvenItemOverlap(Obj, durSsend, durTsend, source, target);

	if (btRet == 1)
	{
		return -1;
	}

	else if (btRet == 21 || btRet == 0xFF)
	{
		return btRet;
	}

	if (Obj.pntEventInventory[source]->IsItem() == FALSE || Obj.pntEventInventory[target]->IsItem() == TRUE)
	{
		return -1;
	}

	int width, height;

	Obj.pntEventInventory[source]->GetSize(width, height);
	std::memcpy(&TempEventInventoryMap, Obj.EventInventoryMap, EVENT_INVENTORY_MAP_SIZE);
	gObjEventInventoryItemBoxSet(Obj, source, width, height, -1);

	int w = target % 8;
	int h = target / 8;

	if (ExtentCheck(w, h, 8, 4) == FALSE)
	{
		std::memcpy(Obj.pntEventInventory, TempEventInventoryMap, EVENT_INVENTORY_MAP_SIZE);
		return -1;
	}

	if (*(BYTE*)(Obj.EventInventoryMap + h * 8 + w) != 0xFF)
	{
		std::memcpy(Obj.EventInventoryMap, TempEventInventoryMap, EVENT_INVENTORY_MAP_SIZE);
		return -1;
	}

	BYTE blank = gObjEventInventoryRectCheck(Obj, w, h, width, height);

	if (blank == 0xFF)
	{
		std::memcpy(Obj.EventInventoryMap, TempEventInventoryMap, EVENT_INVENTORY_MAP_SIZE);
		return -1;
	}

	std::memcpy(&Obj.pntEventInventory[blank], &Obj.pntEventInventory[source], sizeof(Obj.pntEventInventory[blank]));
	Obj.pntEventInventory[source]->Clear();
	gObjEventInventoryItemBoxSet(Obj, blank, width, height, Obj.pntEventInventory[blank]->m_Type);

	return 21;
}

BYTE gObjEventInventoryTradeMove(CGameObject &Obj, BYTE source, BYTE target)
{
	int h, w, iwidth, iheight, s_num, blank;

	if (Obj.m_bMapSvrMoveReq == true || Obj.m_bMapSvrMoveQuit == true || Obj.m_State == 32)
	{
		sLog->outError("[gObjEventInventoryTradeMove] Can't move item in inventory - MapServerMove");
		return -1;
	}

	if (source > EVENT_INVENTORY_SIZE)
	{
		sLog->outBasic("error : %s %d", __FILE__, __LINE__);
		return -1;
	}

	if (Obj.pntEventInventory[source]->IsItem() == 0)
	{
		return -1;
	}

	if (Obj.TargetNumber < 0)
	{
		return -1;
	}

	if (Obj.m_IfState->use == 0 || Obj.m_IfState->type != 1)
	{
		return -1;
	}

	LPITEM_ATTRIBUTE pItemAttribute = GetItemAttr(Obj.pntEventInventory[source]->m_Type);

	if (pItemAttribute == NULL)
	{
		return -1;
	}

	if (pItemAttribute->ItemKindA != 11)
	{
		return -1;
	}

	if (!IsTransactionItem(Obj.pntEventInventory[source]->m_Type))
	{
		return -1;
	}

	Obj.pntEventInventory[source]->GetSize((int &)iwidth, (int &)iheight);
	s_num = Obj.pntEventInventory[source]->GetNumber();

	if (gObjCheckSerial0ItemList(Obj.pntEventInventory[source]) != 0)
	{
		MsgOutput(Obj, Lang.GetText(0, 259));
		sLog->outBasic("[ANTI-HACK][Serial 0 Item] [Trade] (%s)(%s) Item(%s) Pos(%d)", Obj.AccountID, Obj.Name, Obj.pntEventInventory[source]->GetName(), source);
		return -1;
	}

	if (gObjEventInventorySearchSerialNum(Obj, s_num) == 0)
	{
		return -1;
	}

	w = target % 8;
	h = target / 8;

	if (ExtentCheck(w, h, 8, 4) == 0)
	{
		return -1;
	}

	if (*(BYTE*)(Obj.TradeMap + h * 8 + w) == 255)
	{
		BYTE itembuf[13];
		blank = gObjTradeRectCheck(Obj, w, h, iwidth, iheight);

		if (blank == 255)
		{
			return -1;
		}

		Obj.pntTrade[blank] = Obj.pntEventInventory[source];
		gObjEventInventoryDeleteItem(Obj, source);
		gObjTradeItemBoxSet(Obj, blank, iwidth, iheight, Obj.pntTrade[blank]->m_Type);
		ItemByteConvert(itembuf, *Obj.pntTrade[blank]);
		gGameProtocol.GCTradeOtherAdd(Obj, blank, itembuf); // TODO Send Player.
		return true;
	}

	return -1;
}

BYTE gObjTradeEventInventoryMove(CGameObject &Obj, BYTE source, BYTE target)
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

	if (Obj.TargetNumber < 0)
	{
		return -1;
	}

	if (Obj.pntTrade[source]->IsItem() == 0)
	{
		return -1;
	}

	if (IsEventItem(*Obj.pntTrade[source]) == FALSE)
	{
		return -1;
	}

	if (Obj.m_IfState->use == 0 || Obj.m_IfState->type != 1)
	{
		return -1;
	}

	if (gObjEventInventoryInsertItemPos(Obj, *Obj.pntTrade[source], target, 1) == 255)
	{
		return -1;
	}

	s_num = 0;
	s_num = Obj.pntTrade[source]->m_Number;

	Obj.pntTrade[source]->GetSize((int &)iwidth, (int &)iheight);

	gObjTradeItemBoxSet(Obj, source, iwidth, iheight, 255);
	Obj.pntTrade[source]->Clear();

	ItemByteConvert(itembuf, *Obj.pntTrade[source]);

	gGameProtocol.GCTradeOtherDel(Obj, source); // TODO Send Player

	if (gObjCheckSerial0ItemList(Obj.pntTrade[source]) != 0)
	{
		MsgOutput(Obj, Lang.GetText(0, 259));
		sLog->outBasic("[ANTI-HACK][Serial 0 Item] [Trade] (%s)(%s) Item(%s) Pos(%d)", Obj.AccountID, Obj.Name, Obj.pntTrade[source]->GetName(), source);
		return -1;
	}

	if (gObjEventInventorySearchSerialNum(Obj, s_num) == 0)
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

BYTE gObjTempEventInventoryRectCheck(CGameObject &Obj, BYTE * TempMap, int sx, int sy, int width, int height)
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

BYTE gObjTempEventInventoryInsertItem(CGameObject &Obj, CItemObject &item, BYTE * TempMap)
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
				blank = gObjTempEventInventoryRectCheck(Obj, TempMap, w, h, iwidth, iheight);

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

BOOL CheckEventInventoryEmptySpace(CGameObject &Obj, int iItemHeight, int iItemWidth)
{
	int h = 0;
	int w = 0;
	BYTE blank = 0;

	for (h = 0; h < 4; h++)
	{
		for (w = 0; w < 8; w++)
		{
			if (*(BYTE*)(Obj.EventInventoryMap + (h) * 8 + (w)) == 255)
			{
				blank = gObjEventInventoryRectCheck(Obj, w, h, iItemWidth, iItemHeight);

				if (blank == 0xFE)
					return FALSE;

				if (blank != 0xFF)
					return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL IsEventItem(CItemObject &item, int )
{
	LPITEM_ATTRIBUTE pItemAttribute = GetItemAttr(item.m_Type);

	if (!pItemAttribute)
	{
		return FALSE;
	}

	return pItemAttribute->ItemKindA == 11;
}

BYTE gObjEventInventoryRectCheck(CGameObject &Obj, int sx, int sy, int width, int height)
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
				if (*(BYTE*)(Obj.EventInventoryMap + (sy + y) * 8 + (sx + x)) != 255)
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

BYTE gObjEventInventoryInsertItemPos(CGameObject &Obj, CItemObject &item, int pos, BOOL RequestCheck)
{
	if (pos < 0 || pos > EVENT_INVENTORY_SIZE)
	{
		return -1;
	}

	BYTE TempInventoryMap[EVENT_INVENTORY_MAP_SIZE];

	int w, h, iwidth, iheight;

	int blank, useClass = 0;

	if (Obj.pntEventInventory[pos]->IsItem() == 1)
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

	std::memcpy(TempInventoryMap, Obj.EventInventoryMap, EVENT_INVENTORY_MAP_SIZE);

	if (*(BYTE*)(Obj.EventInventoryMap + h * 8 + w) != 255)
	{
		std::memcpy(Obj.EventInventoryMap, TempInventoryMap, EVENT_INVENTORY_MAP_SIZE);
		return -1;
	}

	blank = gObjEventInventoryRectCheck(Obj, w, h, iwidth, iheight);

	if (blank >= 254)
	{
		std::memcpy(Obj.EventInventoryMap, TempInventoryMap, EVENT_INVENTORY_MAP_SIZE);
		return false;
	}

	Obj.pntEventInventory[pos] = &item;
	gObjEventInventoryItemSet(Obj, pos, 1);

	return pos;
}

BOOL gObjEventInventorySearchSerialNum(CGameObject &Obj, UINT64 serial)
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
		s_num = Obj.pntEventInventory[n]->GetNumber();

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
		s_num = Obj.pntEventInventory[n]->GetNumber();

		if (s_num != 0 && s_num == serial && s_num != (UINT64)-1)
		{
			sLog->outBasic("error-L1: CopyItem Id[%s] Char[%s] Item[%s] InventoryPos[%d] serial[%I64d]", Obj.AccountID, Obj.Name, Obj.pntEventInventory[n]->GetName(), n, s_num);
			gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 15), Obj, 1); // TODO Send to game server.
			gObjUserKill(Obj);
		}
	}

	return false;
}

void gObjSetEventInventory1Pointer(CGameObject &Obj)
{
	Obj.pntEventInventory = Obj.pntEventInventory1;
	Obj.EventInventoryMap = Obj.EventInventoryMap1;
}

void gObjSetEventInventory2Pointer(CGameObject &Obj)
{
	Obj.pntEventInventory = Obj.pntEventInventory2;
	Obj.EventInventoryMap = Obj.EventInventoryMap2;
}

