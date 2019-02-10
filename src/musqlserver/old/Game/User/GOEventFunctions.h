#ifndef _MU_GOFUNCTIONS_H_
#define _MU_GOFUNCTIONS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "MapItem.h"
#include "CGameObject.h"
#include "GOFunctions.h"

class CGameObject;

void gObjEventInventoryItemSet(LPGameObject &lpObj, int itempos, BYTE set_byte);
void gObjEventInventoryItemBoxSet(LPGameObject &lpObj, int itempos, int xl, int yl, BYTE set_byte);
bool gObjFixEventInventoryPointer(LPGameObject &lpObj);
bool gObjEventInventoryDeleteItem(LPGameObject &lpObj, int itempos);
BYTE gObjEventInventoryInsertItemTemp(LPGameObject &lpObj, CMapItem* Item);
BYTE gObjEventInventoryInsertItem(LPGameObject &lpObj, CMapItem* item);
BYTE gObjEventInventoryInsertItem(LPGameObject &lpObj, CItem item);
BYTE gObjEventInvenItemOverlap(LPGameObject &lpObj, int *durSsend, int *durTsend, BYTE source, BYTE target);
BYTE gObjEventInvenMove(LPGameObject &lpObj, int *durSsend, int *durTsend, BYTE source, BYTE target);
BYTE gObjEventInventoryTradeMove(LPGameObject &lpObj, BYTE source, BYTE target);
BYTE gObjTradeEventInventoryMove(LPGameObject &lpObj, BYTE source, BYTE target);
void gObjTempEventInventoryItemBoxSet(BYTE * TempMap, int itempos, int xl, int yl, BYTE set_byte);
BYTE gObjTempEventInventoryRectCheck(LPGameObject &lpObj, BYTE * TempMap, int sx, int sy, int width, int height);
BYTE gObjTempEventInventoryInsertItem(LPGameObject &lpObj, CItem item, BYTE * TempMap);
int CheckEventInventoryEmptySpace(LPGameObject &lpObj, int iItemHeight, int iItemWidth);
int IsEventItem(CItem item);
int IsEventItem(int iType);
BYTE gObjEventInventoryRectCheck(LPGameObject &lpObj, int sx, int sy, int width, int height);
BYTE gObjEventInventoryInsertItemPos(LPGameObject &lpObj, CItem item, int pos, BOOL RequestCheck);
int gObjEventInventorySearchSerialNum(LPGameObject &lpObj, UINT64 serial);
void gObjSetEventInventory1Pointer(LPGameObject &lpObj);
void gObjSetEventInventory2Pointer(LPGameObject &lpObj);

extern void gObjEventInventoryItemSet(LPGameObject &lpObj, int itempos, BYTE set_byte);
extern void gObjEventInventoryItemBoxSet(LPGameObject &lpObj, int itempos, int xl, int yl, BYTE set_byte);
extern bool gObjFixEventInventoryPointer(LPGameObject &lpObj);
extern bool gObjEventInventoryDeleteItem(LPGameObject &lpObj, int itempos);
extern BYTE gObjEventInventoryInsertItemTemp(LPGameObject &lpObj, CMapItem* Item);
extern BYTE gObjEventInventoryInsertItem(LPGameObject &lpObj, CMapItem* item);
extern BYTE gObjEventInventoryInsertItem(LPGameObject &lpObj, CItem item);
extern BYTE gObjEventInvenItemOverlap(LPGameObject &lpObj, int *durSsend, int *durTsend, BYTE source, BYTE target);
extern BYTE gObjEventInvenMove(LPGameObject &lpObj, int *durSsend, int *durTsend, BYTE source, BYTE target);
extern BYTE gObjEventInventoryTradeMove(LPGameObject &lpObj, BYTE source, BYTE target);
extern BYTE gObjTradeEventInventoryMove(LPGameObject &lpObj, BYTE source, BYTE target);
extern void gObjTempEventInventoryItemBoxSet(BYTE * TempMap, int itempos, int xl, int yl, BYTE set_byte);
extern BYTE gObjTempEventInventoryRectCheck(LPGameObject &lpObj, BYTE * TempMap, int sx, int sy, int width, int height);
extern BYTE gObjTempEventInventoryInsertItem(LPGameObject &lpObj, CItem item, BYTE * TempMap);
extern int CheckEventInventoryEmptySpace(LPGameObject &lpObj, int iItemHeight, int iItemWidth);
extern int IsEventItem(CItem item);
extern int IsEventItem(int iType);
extern BYTE gObjEventInventoryRectCheck(LPGameObject &lpObj, int sx, int sy, int width, int height);
extern BYTE gObjEventInventoryInsertItemPos(LPGameObject &lpObj, CItem item, int pos, BOOL RequestCheck);
extern int gObjEventInventorySearchSerialNum(LPGameObject &lpObj, UINT64 serial);
extern void gObjSetEventInventory1Pointer(LPGameObject &lpObj);
extern void gObjSetEventInventory2Pointer(LPGameObject &lpObj);

#endif
