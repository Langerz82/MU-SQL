#ifndef _MU_GOEVENTFUNCTIONS_H_
#define _MU_GOEVENTFUNCTIONS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "MapItem.h"

void gObjEventInventoryItemSet(CGameObject &Obj, int itempos, BYTE set_byte);
void gObjEventInventoryItemBoxSet(CGameObject &Obj, int itempos, int xl, int yl, BYTE set_byte);
bool gObjFixEventInventoryPointer(CGameObject &Obj);
bool gObjEventInventoryDeleteItem(CGameObject &Obj, int itempos);
BYTE gObjEventInventoryInsertItemTemp(CGameObject &Obj, CMapItem* Item);
BYTE gObjEventInventoryInsertItem(CGameObject &Obj, CMapItem* item);
BYTE gObjEventInventoryInsertItem(CGameObject &Obj, CItemObject item);
BYTE gObjEventInvenItemOverlap(CGameObject &Obj, int *durSsend, int *durTsend, BYTE source, BYTE target);
BYTE gObjEventInvenMove(CGameObject &Obj, int *durSsend, int *durTsend, BYTE source, BYTE target);
BYTE gObjEventInventoryTradeMove(CGameObject &Obj, BYTE source, BYTE target);
BYTE gObjTradeEventInventoryMove(CGameObject &Obj, BYTE source, BYTE target);
void gObjTempEventInventoryItemBoxSet(BYTE * TempMap, int itempos, int xl, int yl, BYTE set_byte);
BYTE gObjTempEventInventoryRectCheck(CGameObject &Obj, BYTE * TempMap, int sx, int sy, int width, int height);
BYTE gObjTempEventInventoryInsertItem(CGameObject &Obj, CItemObject &item, BYTE * TempMap);
int CheckEventInventoryEmptySpace(CGameObject &Obj, int iItemHeight, int iItemWidth);
int IsEventItem(CItemObject &item);
BYTE gObjEventInventoryRectCheck(CGameObject &Obj, int sx, int sy, int width, int height);
BYTE gObjEventInventoryInsertItemPos(CGameObject &Obj, CItemObject item, int pos, BOOL RequestCheck);
int gObjEventInventorySearchSerialNum(CGameObject &Obj, UINT64 serial);
void gObjSetEventInventory1Pointer(CGameObject &Obj);
void gObjSetEventInventory2Pointer(CGameObject &Obj);

extern void gObjEventInventoryItemSet(CGameObject &Obj, int itempos, BYTE set_byte);
extern void gObjEventInventoryItemBoxSet(CGameObject &Obj, int itempos, int xl, int yl, BYTE set_byte);
extern bool gObjFixEventInventoryPointer(CGameObject &Obj);
extern bool gObjEventInventoryDeleteItem(CGameObject &Obj, int itempos);
extern BYTE gObjEventInventoryInsertItemTemp(CGameObject &Obj, CMapItem* Item);
extern BYTE gObjEventInventoryInsertItem(CGameObject &Obj, CMapItem* item);
extern BYTE gObjEventInventoryInsertItem(CGameObject &Obj, CItemObject item);
extern BYTE gObjEventInvenItemOverlap(CGameObject &Obj, int *durSsend, int *durTsend, BYTE source, BYTE target);
extern BYTE gObjEventInvenMove(CGameObject &Obj, int *durSsend, int *durTsend, BYTE source, BYTE target);
extern BYTE gObjEventInventoryTradeMove(CGameObject &Obj, BYTE source, BYTE target);
extern BYTE gObjTradeEventInventoryMove(CGameObject &Obj, BYTE source, BYTE target);
extern void gObjTempEventInventoryItemBoxSet(BYTE * TempMap, int itempos, int xl, int yl, BYTE set_byte);
extern BYTE gObjTempEventInventoryRectCheck(CGameObject &Obj, BYTE * TempMap, int sx, int sy, int width, int height);
extern BYTE gObjTempEventInventoryInsertItem(CGameObject &Obj, CItemObject &item, BYTE * TempMap);
extern int CheckEventInventoryEmptySpace(CGameObject &Obj, int iItemHeight, int iItemWidth);
extern int IsEventItem(CItemObject &item);
extern BYTE gObjEventInventoryRectCheck(CGameObject &Obj, int sx, int sy, int width, int height);
extern BYTE gObjEventInventoryInsertItemPos(CGameObject &Obj, CItemObject item, int pos, BOOL RequestCheck);
extern int gObjEventInventorySearchSerialNum(CGameObject &Obj, UINT64 serial);
extern void gObjSetEventInventory1Pointer(CGameObject &Obj);
extern void gObjSetEventInventory2Pointer(CGameObject &Obj);

#endif
