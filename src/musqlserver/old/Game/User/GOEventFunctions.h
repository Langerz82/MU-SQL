#ifndef _MU_GOEVENTFUNCTIONS_H_
#define _MU_GOEVENTFUNCTIONS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "MapItem.h"

void gObjEventInventoryItemSet(CGameObject &lpObj, int itempos, BYTE set_byte);
void gObjEventInventoryItemBoxSet(CGameObject &lpObj, int itempos, int xl, int yl, BYTE set_byte);
bool gObjFixEventInventoryPointer(CGameObject &lpObj);
bool gObjEventInventoryDeleteItem(CGameObject &lpObj, int itempos);
BYTE gObjEventInventoryInsertItemTemp(CGameObject &lpObj, CMapItem* Item);
BYTE gObjEventInventoryInsertItem(CGameObject &lpObj, CMapItem* item);
BYTE gObjEventInventoryInsertItem(CGameObject &lpObj, CItemObject item);
BYTE gObjEventInvenItemOverlap(CGameObject &lpObj, int *durSsend, int *durTsend, BYTE source, BYTE target);
BYTE gObjEventInvenMove(CGameObject &lpObj, int *durSsend, int *durTsend, BYTE source, BYTE target);
BYTE gObjEventInventoryTradeMove(CGameObject &lpObj, BYTE source, BYTE target);
BYTE gObjTradeEventInventoryMove(CGameObject &lpObj, BYTE source, BYTE target);
void gObjTempEventInventoryItemBoxSet(BYTE * TempMap, int itempos, int xl, int yl, BYTE set_byte);
BYTE gObjTempEventInventoryRectCheck(CGameObject &lpObj, BYTE * TempMap, int sx, int sy, int width, int height);
BYTE gObjTempEventInventoryInsertItem(CGameObject &lpObj, CItemObject item, BYTE * TempMap);
int CheckEventInventoryEmptySpace(CGameObject &lpObj, int iItemHeight, int iItemWidth);
int IsEventItem(CItemObject item);
int IsEventItem(int iType);
BYTE gObjEventInventoryRectCheck(CGameObject &lpObj, int sx, int sy, int width, int height);
BYTE gObjEventInventoryInsertItemPos(CGameObject &lpObj, CItemObject item, int pos, BOOL RequestCheck);
int gObjEventInventorySearchSerialNum(CGameObject &lpObj, UINT64 serial);
void gObjSetEventInventory1Pointer(CGameObject &lpObj);
void gObjSetEventInventory2Pointer(CGameObject &lpObj);

extern void gObjEventInventoryItemSet(CGameObject &lpObj, int itempos, BYTE set_byte);
extern void gObjEventInventoryItemBoxSet(CGameObject &lpObj, int itempos, int xl, int yl, BYTE set_byte);
extern bool gObjFixEventInventoryPointer(CGameObject &lpObj);
extern bool gObjEventInventoryDeleteItem(CGameObject &lpObj, int itempos);
extern BYTE gObjEventInventoryInsertItemTemp(CGameObject &lpObj, CMapItem* Item);
extern BYTE gObjEventInventoryInsertItem(CGameObject &lpObj, CMapItem* item);
extern BYTE gObjEventInventoryInsertItem(CGameObject &lpObj, CItemObject item);
extern BYTE gObjEventInvenItemOverlap(CGameObject &lpObj, int *durSsend, int *durTsend, BYTE source, BYTE target);
extern BYTE gObjEventInvenMove(CGameObject &lpObj, int *durSsend, int *durTsend, BYTE source, BYTE target);
extern BYTE gObjEventInventoryTradeMove(CGameObject &lpObj, BYTE source, BYTE target);
extern BYTE gObjTradeEventInventoryMove(CGameObject &lpObj, BYTE source, BYTE target);
extern void gObjTempEventInventoryItemBoxSet(BYTE * TempMap, int itempos, int xl, int yl, BYTE set_byte);
extern BYTE gObjTempEventInventoryRectCheck(CGameObject &lpObj, BYTE * TempMap, int sx, int sy, int width, int height);
extern BYTE gObjTempEventInventoryInsertItem(CGameObject &lpObj, CItemObject item, BYTE * TempMap);
extern int CheckEventInventoryEmptySpace(CGameObject &lpObj, int iItemHeight, int iItemWidth);
extern int IsEventItem(CItemObject item);
extern int IsEventItem(int iType);
extern BYTE gObjEventInventoryRectCheck(CGameObject &lpObj, int sx, int sy, int width, int height);
extern BYTE gObjEventInventoryInsertItemPos(CGameObject &lpObj, CItemObject item, int pos, BOOL RequestCheck);
extern int gObjEventInventorySearchSerialNum(CGameObject &lpObj, UINT64 serial);
extern void gObjSetEventInventory1Pointer(CGameObject &lpObj);
extern void gObjSetEventInventory2Pointer(CGameObject &lpObj);

#endif
