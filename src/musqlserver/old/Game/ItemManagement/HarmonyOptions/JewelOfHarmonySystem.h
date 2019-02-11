#ifndef _MU_JEWELOFHARMONYSYSTEM_H_
#define _MU_JEWELOFHARMONYSYSTEM_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include <map>
#include "ItemManagement/Item.h"

#include "TRandomPoolMgr.h"

class CItem;

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
	BYTE GetItemStrengthenOption(CItem* pItem);
	BYTE GetItemOptionLevel(CItem* pItem);
	BOOL IsActive(CItem* pItem);
	BOOL IsStrengthenByJewelOfHarmony(CItem* pItem);
	BOOL StrengthenItemByJewelOfHarmony(struct LPGameObject &lpObj, int source, int target);
	BOOL StrengthenItemByJewelOfRise(struct LPGameObject &lpObj, int source, int target);
	void StrengthenItemByMacro(struct LPGameObject &lpObj, BYTE invenrotyTargetPos, BYTE btOptionType,  BYTE btOptionLevel);
	void SetApplyStrengthenItem(struct LPGameObject &lpObj);
	void InitEffectValue(JEWELOFHARMONY_ITEM_EFFECT* pItemEffect);
	int GetItemEffectValue(CItem* pItem, int iOptionType);
	BOOL IsEnableToMakeSmeltingStoneItem(CItem* pItem);
	BOOL MakeSmeltingStoneItem(struct LPGameObject &lpObj);
	BOOL MakeSmeltingStoneItem_MultiMix(struct LPGameObject &lpObj, int iMixCount);
	BOOL SmeltItemBySmeltingStone(struct LPGameObject &lpObj, int source, int target);
	BOOL RestoreStrengthenItem(struct LPGameObject &lpObj);
	BYTE MakeCharSetData(struct LPGameObject &lpObj);
	BOOL NpcJewelOfHarmony(struct OBJECTSTRUCT * lpNpc, struct LPGameObject &lpObj);
	BYTE ShowStrengthenOption(CItem* pItem);
	BOOL IsEnableToTrade(struct LPGameObject &lpObj);

	BOOL m_bSystemSmeltingItem;	// 5328
	BOOL _MakeOption(CItem* pItem, BYTE btOptionType, BYTE btOptionLevel);
	int _GetSelectRandomOption(CItem* pItem, int iItemType);
	int _GetItemType(CItem* pItem);
	BOOL m_bSystemPrutiyJewel;	// 5318
	int m_iZenForPurity;	// 5330
	int m_iRatePuritySuccess;	// 532C
	JEWELOFHARMONY_ITEM_OPTION m_itemOption[MAX_JOH_ITEM_TYPE][MAX_JOH_ITEM_INDEX];	// 18
private:

	void _InitOption();
	BOOL _IsJewelOfHarmonySmeltingItemNor( short type);
	BOOL _IsJewelOfHarmonySmeltingItemExt(short type);
	BYTE _GetItemOptionLevel(CItem* pItem);
	BYTE _GetItemOptionRequireLevel(CItem* pItem);
	BOOL _CalcItemEffectValue(CItem* pItem, JEWELOFHARMONY_ITEM_EFFECT* pItemEffect);
	int _GetZenForRestoreItem(CItem* pItem);

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
	//TRandomPoolMgr m_kRandomPool;	// 535C
};

extern CJewelOfHarmonySystem g_kJewelOfHarmonySystem;


#endif
