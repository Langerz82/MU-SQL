#ifndef _MU_JEWELOFHARMONYSYSTEM_H_
#define _MU_JEWELOFHARMONYSYSTEM_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include <map>
#include "ItemObject.h"

#include "TRandomPoolMgr.h"

class CItemObject;

struct JEWELOFHARMONY_ITEM_EFFECT;
struct JEWELOFHARMONY_ITEM_OPTION;

class CJewelOfHarmonySystem
{

public:

	CJewelOfHarmonySystem();
	virtual ~CJewelOfHarmonySystem();

	BOOL LoadScript(LPSTR lpszFileName);
	BOOL LoadScriptOfSmelt(LPSTR lpszFileName);
	BOOL IsJewelOfHarmonyOriginal(short type);
	BOOL IsJewelOfHarmonyPurity(short type);
	BOOL IsJewelOfHarmonySmeltingItems(short type);
	void SetEnableToUsePuritySystem(int bEnable);
	BOOL IsEnableToUsePuritySystem();
	BYTE GetItemStrengthenOption(CItemObject* pItem);
	BYTE GetItemOptionLevel(CItemObject* pItem);
	BOOL IsActive(CItemObject* pItem);
	BOOL IsStrengthenByJewelOfHarmony(CItemObject* pItem);
	BOOL StrengthenItemByJewelOfHarmony(CGameObject &Obj, int source, int target);
	BOOL StrengthenItemByJewelOfRise(CGameObject &Obj, int source, int target);
	void StrengthenItemByMacro(CGameObject &Obj, BYTE invenrotyTargetPos, BYTE btOptionType,  BYTE btOptionLevel);
	void SetApplyStrengthenItem(CGameObject &Obj);
	void InitEffectValue(JEWELOFHARMONY_ITEM_EFFECT* pItemEffect);
	int GetItemEffectValue(CItemObject* pItem, int iOptionType);
	BOOL IsEnableToMakeSmeltingStoneItem(CItemObject* pItem);
	BOOL MakeSmeltingStoneItem(CGameObject &Obj);
	BOOL MakeSmeltingStoneItem_MultiMix(CGameObject &Obj, int iMixCount);
	BOOL SmeltItemBySmeltingStone(CGameObject &Obj, int source, int target);
	BOOL RestoreStrengthenItem(CGameObject &Obj);
	BYTE MakeCharSetData(CGameObject &Obj);
	BOOL NpcJewelOfHarmony(CGameObject &Npc, CGameObject &Obj);
	BYTE ShowStrengthenOption(CItemObject* pItem);
	BOOL IsEnableToTrade(CGameObject &Obj);

	BOOL m_bSystemSmeltingItem;	// 5328
	BOOL _MakeOption(CItemObject* pItem, BYTE btOptionType, BYTE btOptionLevel);
	int _GetSelectRandomOption(CItemObject* pItem, int iItemType);
	int _GetItemType(CItemObject* pItem);
	BOOL m_bSystemPrutiyJewel;	// 5318
	int m_iZenForPurity;	// 5330
	int m_iRatePuritySuccess;	// 532C
	JEWELOFHARMONY_ITEM_OPTION m_itemOption[MAX_JOH_ITEM_TYPE][MAX_JOH_ITEM_INDEX];	// 18
private:

	void _InitOption();
	BOOL _IsJewelOfHarmonySmeltingItemNor( short type);
	BOOL _IsJewelOfHarmonySmeltingItemExt(short type);
	BYTE _GetItemOptionLevel(CItemObject* pItem);
	BYTE _GetItemOptionRequireLevel(CItemObject* pItem);
	BOOL _CalcItemEffectValue(CItemObject* pItem, JEWELOFHARMONY_ITEM_EFFECT* pItemEffect);
	int _GetZenForRestoreItem(CItemObject* pItem);

private:

	BOOL m_bEnable;	// 4
	int JEWEL_OF_HARMONY_ITEMINDEX;	// 8
	int JEWEL_OF_HARMONY_PURITY_ITEMINDEX;	// C
	int JEWEL_OF_HARMONY_SMELT_NOR_ITEMINDEX;	// 10
	int JEWEL_OF_HARMONY_SMELT_EXT_ITEMINDEX;	// 14
	BOOL m_bSystemMixSmeltingStone;	// 531C
	BOOL m_bSystemRestoreStrengthen;	// 5320
	BOOL m_bSystemStrengthenItem;	// 5324
	int m_iRateMixSmeltingStoneNor;	// 5334
	int m_iRateMixSmeltingStoneExt;	// 5338
	int m_iZenForMixSmeltingStone;	// 533C
	int m_iRateStrengthenSuccess;	// 5340
	int m_iRateSmeltingSuccessNor;	// 5344
	int m_iRateSmeltingSuccessExt;	// 5348
	std::map<int,int> m_mapEnableMixList;	// 534C
	TRandomPoolMgr m_kRandomPool;	// 535C
};

extern CJewelOfHarmonySystem g_kJewelOfHarmonySystem;


#endif
