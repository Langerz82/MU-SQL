////////////////////////////////////////////////////////////////////////////////
// PersonalStore.h
#ifndef PERSONALSTORE_H
#define PERSONALSTORE_H

#include "StdAfx.h"
#include "ItemManagement/Item.h"
#include "User/CUserData.h"
#include "MuDefines.h"


#pragma once

struct PMSG_PSHOP_VIEWPORT_NOTIFY 
{
	unsigned char btNumberH;
	unsigned char btNumberL;
	char szPShopText[36];
};

struct PMSG_BUYLIST_FROM_PSHOP
{
	BYTE Pos;	// 0
	BYTE ItemInfo[MAX_ITEM_INFO];	// 1
	int PShopItemValue;	// 8
	short BlessValue;
	short SoulValue;
	short ChaosValue;
};

struct PSHOP_ITEMVALUE_INFO
{
	int nPShopItemInvenNum;
	int Empty;
	int nMoney;
	short sBlessJewelValue;
	short sSoulJewelValue;
	short sChaosJewelValue;
};

struct PMSG_SEARCH_ITEM_PSHOP
{
	BYTE btNumberH;
	BYTE btNumberL;
	char szName[MAX_ACCOUNT_LEN + 1];
	char szPShopText[37];
};

class CPersonalStore
{
public:
	CPersonalStore();
	virtual ~CPersonalStore();

	void CGPShopReqSetItemPrice(PMSG_REQ_PSHOP_SETITEMPRICE* lpMsg, CGameObject &lpObj);
	void CGPShopAnsSetItemPrice(CGameObject &lpObj, BYTE btResult, BYTE btItemPos);
	void CGPShopReqOpen(PMSG_REQ_PSHOP_OPEN* lpMsg, CGameObject &lpObj);
	void CGPShopAnsOpen(CGameObject &lpObj, BYTE btResult);
	void CGPShopReqClose(CGameObject &lpObj);
	void CGPShopAnsClose(CGameObject &lpObj, BYTE btResult);
	void CGPShopReqBuyList(PMSG_REQ_BUYLIST_FROM_PSHOP* lpMsg, int aSourceIndex);

	void CGPShopAnsBuyList(int aSourceIndex, int aTargetIndex, BYTE btResult, bool bResend);
	void CGPShopReqBuyItem(LPBYTE lpRecv, int aSourceIndex);

	void CGPShopAnsBuyItem(int aSourceIndex, int aTargetIndex, int iItemPos, BYTE btResult);
	void CGPShopReqCloseDeal(PMSG_REQ_PSHOPDEAL_CLOSE* lpMsg, CGameObject &lpObj);
	void CGPShopAnsSoldItem(int aSourceIndex, int aTargetIndex, int iItemPos);
	void CGPShopAnsDealerClosedShop(int aSourceIndex, int aTargetIndex);

	void gObjInventoryItemSet_PShop(CGameObject &lpObj, int itempos, unsigned char set_byte);
	void gObjInventoryItemBoxSet_PShop(CGameObject &lpObj, int itempos, int xl, int yl, unsigned char set_byte);

	void PShop_ViewportListRegenarate(short aIndex);
	bool PShop_CheckInventoryEmpty(short aIndex);

	// Season 8
	void CGReqSearchItemInPShop(PMSG_REQ_SEARCH_ITEM_PSHOP *lpMsg, CGameObject &lpObj);
	void CGReqPShopLog(PMSG_REQ_PSHOP_LOG *lpMsg, CGameObject &lpObj);

	void GDRequestPShopItemValue(CGameObject* lpObj, char *szAccountID);
	void GDUpdatePShopItemValue(CGameObject* lpObj);
	void GDAllSavePShopItemValue(CGameObject* lpObj);
	void GDDelPShopItemValue(CGameObject* lpObj, int nPShopItemInvenNum);
	void GDMovePShopItem(CGameObject* lpObj, int nOldPShopItemInvenNum, int nNewPShopItemInvenNum);
	void GDAnsPShopItemValue(PMSG_ANS_PSHOPITEMVALUE_INFO *lpMsg);

	void GCPShopItemValueInfo(CGameObject &lpObj);

	void GCPShop_AllInfo(short aIndex, int iLastUserCount);
	void GCPShop_SearchItem(short aIndex, WORD sSearchItem, int iLastUserCount);

	bool PShop_CheckExistItemInInventory(short aIndex, WORD sItemType);
};

extern CPersonalStore g_PersonalStore;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

