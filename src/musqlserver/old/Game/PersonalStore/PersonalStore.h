#ifndef PERSONALSTORE_H
#define PERSONALSTORE_H

#include "StdAfx.h"
#include "ItemObject.h"
#include "User/CUserData.h"
#include "MuDefines.h"


#pragma once

class CPersonalStore
{
public:
	CPersonalStore();
	virtual ~CPersonalStore();

	void CGPShopReqSetItemPrice(PMSG_REQ_PSHOP_SETITEMPRICE* lpMsg, CGameObject &Obj);
	void CGPShopAnsSetItemPrice(CGameObject &Obj, BYTE btResult, BYTE btItemPos);
	void CGPShopReqOpen(PMSG_REQ_PSHOP_OPEN* lpMsg, CGameObject &Obj);
	void CGPShopAnsOpen(CGameObject &Obj, BYTE btResult);
	void CGPShopReqClose(CGameObject &Obj);
	void CGPShopAnsClose(CGameObject &Obj, BYTE btResult);
	void CGPShopReqBuyList(PMSG_REQ_BUYLIST_FROM_PSHOP* lpMsg, int aSourceIndex);

	void CGPShopAnsBuyList(int aSourceIndex, int aTargetIndex, BYTE btResult, bool bResend);
	void CGPShopReqBuyItem(LPBYTE lpRecv, int aSourceIndex);

	void CGPShopAnsBuyItem(int aSourceIndex, int aTargetIndex, int iItemPos, BYTE btResult);
	void CGPShopReqCloseDeal(PMSG_REQ_PSHOPDEAL_CLOSE* lpMsg, CGameObject &Obj);
	void CGPShopAnsSoldItem(int aSourceIndex, int aTargetIndex, int iItemPos);
	void CGPShopAnsDealerClosedShop(int aSourceIndex, int aTargetIndex);

	void gObjInventoryItemSet_PShop(CGameObject &Obj, int itempos, BYTE set_byte);
	void gObjInventoryItemBoxSet_PShop(CGameObject &Obj, int itempos, int xl, int yl, BYTE set_byte);

	void PShop_ViewportListRegenarate(short aIndex);
	bool PShop_CheckInventoryEmpty(short aIndex);

	// Season 8
	void CGReqSearchItemInPShop(PMSG_REQ_SEARCH_ITEM_PSHOP *lpMsg, CGameObject &Obj);
	void CGReqPShopLog(PMSG_REQ_PSHOP_LOG *lpMsg, CGameObject &Obj);

	void GDRequestPShopItemValue(CGameObject &Obj, char *szAccountID);
	void GDUpdatePShopItemValue(CGameObject &Obj);
	void GDAllSavePShopItemValue(CGameObject &Obj);
	void GDDelPShopItemValue(CGameObject &Obj, int nPShopItemInvenNum);
	void GDMovePShopItem(CGameObject &Obj, int nOldPShopItemInvenNum, int nNewPShopItemInvenNum);
	void GDAnsPShopItemValue(PMSG_ANS_PSHOPITEMVALUE_INFO *lpMsg);

	void GCPShopItemValueInfo(CGameObject &Obj);

	void GCPShop_AllInfo(short aIndex, int iLastUserCount);
	void GCPShop_SearchItem(short aIndex, WORD sSearchItem, int iLastUserCount);

	bool PShop_CheckExistItemInInventory(short aIndex, WORD sItemType);
};

extern CPersonalStore g_PersonalStore;

#endif
