////////////////////////////////////////////////////////////////////////////////
// RuudStore.h
#ifndef RUUDSTORE_H
#define RUUDSTORE_H

#include "ItemOptionTypeMng.h"

struct RUUD_SHOP_ITEM
{
	BYTE btClass;
	BYTE btCat;
	WORD wIndex;
	BYTE btLevel;
	BYTE btSkill;
	BYTE btLuck;
	BYTE btOption;
	BYTE btExcOption[MAX_ITEM_EXC_OPTION];
	BYTE btSetOption;
	BYTE btSocketCount;
	BYTE btElemental;
	DWORD dwRuudPrice;
};

struct RUUD_MAP_ITEM_DATA
{
	BYTE btClass;
	DWORD dwRuudPrice;
	CItem Item;
};

#pragma once
class CRuudStore
{
public:
	CRuudStore();
	virtual ~CRuudStore();

	void LoadFile(char *szFile);

	bool NpcTalk(OBJECTSTRUCT * lpNpc, OBJECTSTRUCT * lpObj);
	void CGReqBuyItem(PMSG_REQ_RUUD_STORE_BUYITEM * lpMsg, int iIndex);

private:
	bool m_bEnable;
	int m_iShopTime;
	void AddItemToList(int iPos, RUUD_SHOP_ITEM * lpItem);
	RUUD_SHOP_ITEM lpItemList;
	std::map<int, RUUD_MAP_ITEM_DATA> m_mapItemList;
};

extern CRuudStore g_RuudStore;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

