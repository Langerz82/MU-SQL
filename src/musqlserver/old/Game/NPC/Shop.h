////////////////////////////////////////////////////////////////////////////////
// Shop.h
#ifndef ___SHOP_H
#define ___SHOP_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "ItemObject.h"
#include "ItemOptionTypeMng.h"

struct PMSG_REQ_REBUY_ITEM;
struct SDHP_ANS_SHOP_REBUY_GET_ITEM;

class CItemObject;

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
	CItemObject* m_item[MAX_ITEM_IN_SHOP];	// 80
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
	SHOP_DATA * GetShop(CGameObject &lpObj, CGameObject &lpNpc);
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

	void CGReqSoldItemList(CGameObject &Obj);
	void GCAnsSoldItemList(CGameObject &Obj);
	void CGReqEndCancelItemSale(CGameObject &Obj);
	void CGReqReBuyItem(PMSG_REQ_REBUY_ITEM *lpMsg, CGameObject &Obj);

	void GDReqSoldItemList(CGameObject &Obj);
	void DGAnsSoldItemList(LPBYTE lpRecv);
	void GDReqAddItemToList(CGameObject &Obj, CItemObject Item, DWORD dwSellPrice);
	void GDReqGetReBuyItem(CGameObject &Obj, BYTE btItemNumber, DWORD dwItemPrice);
	void DGAnsGetReBuyItem(SDHP_ANS_SHOP_REBUY_GET_ITEM *lpMsg);
	void GDReqDeleteSoldItem(CGameObject &Obj, BYTE btItemNumber, WORD wItemCode, DWORD dwItemPrice);

	void MakeItem(LPBYTE lpData, CItemObject * lpItem);
};

extern CCancelItemSale g_CancelItemSale;

#endif



////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

