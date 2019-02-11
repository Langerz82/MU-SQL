////////////////////////////////////////////////////////////////////////////////
// Shop.h
#ifndef ___SHOP_H
#define ___SHOP_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "ItemManagement/Item.h"
#include "ItemOptionTypeMng.h"

struct PMSG_REQ_REBUY_ITEM;
struct SDHP_ANS_SHOP_REBUY_GET_ITEM;

class CShop
{

public:

	CShop();
	virtual ~CShop();

	void Init();
	BOOL LoadShopItem(char* filename);
	int InsertItem(int type, int index, int level, int dur, int op1, int op2, int op3, BYTE newop[], int setop, int socketcount, int elemental, int serial);
	int InentoryMapCheck(int sx, int sy, int width,  int height);

public:

	BYTE ShopInventoryMap[MAX_ITEM_IN_SHOP];	// 4
	int ItemCount;	// 7C
	CItem m_item[MAX_ITEM_IN_SHOP];	// 80
	BYTE SendItemData[MAX_ITEM_IN_SHOP*(MAX_ITEM_INFO+1)];	// 5300
	int SendItemDataLen;	// 56C0
};

struct SHOP_DATA
{
	int		iNpcIndex;
	BYTE	btMapNumber;
	BYTE	btPosX;
	BYTE	btPosY;
	BYTE	btDir;
	BYTE	btOnlyForGameMaster;
	BYTE	btIsMossMerchant;
	BYTE	btReqVipLevel;
	BYTE	btBattleCoreShop;
	BYTE	btClass[MAX_TYPE_PLAYER];
	CShop	m_ShopData;
};

class CShopMng
{
	
public:

	CShopMng();
	virtual ~CShopMng();

	void LoadShopList(char* filename);
	SHOP_DATA * GetShop(LPGameObject &lpObj, LPGameObject &lpNpc);
	bool IsShopNpc(int iNpcIndex, BYTE btMapNumber, BYTE btPosX, BYTE btPosY);
	void SetShopNpcs();

private:
	
	std::vector<SHOP_DATA> m_vecShopData;
};

extern CShopMng g_ShopMng;



class CCancelItemSale
{

public:

	CCancelItemSale();
	virtual ~CCancelItemSale();

	void CGReqSoldItemList(int iIndex);
	void GCAnsSoldItemList(int iIndex);
	void CGReqEndCancelItemSale(int iIndex);
	void CGReqReBuyItem(PMSG_REQ_REBUY_ITEM *lpMsg, int iIndex);

	void GDReqSoldItemList(int iIndex);
	void DGAnsSoldItemList(LPBYTE lpRecv);
	void GDReqAddItemToList(int iIndex, CItem Item, DWORD dwSellPrice);
	void GDReqGetReBuyItem(int iIndex, BYTE btItemNumber, DWORD dwItemPrice);
	void DGAnsGetReBuyItem(SDHP_ANS_SHOP_REBUY_GET_ITEM *lpMsg);
	void GDReqDeleteSoldItem(int iIndex, BYTE btItemNumber, WORD wItemCode, DWORD dwItemPrice);

	void MakeItem(LPBYTE lpData, CItem * lpItem);
};

extern CCancelItemSale g_CancelItemSale;

#endif



////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

