////////////////////////////////////////////////////////////////////////////////
// Bag.h
#ifndef _MU_BAG_H
#define _MU_BAG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "ItemObject.h"
#include "ItemOptionTypeMng.h"


struct BAG_ITEM
{
	BYTE btItemType;
	WORD wItemIndex;
	BYTE btItemMinLevel;
	BYTE btItemMaxLevel;
	BYTE btItemDur;
	char btIsSkill;
	char btIsLuck;
	char btIsOption;
	char btIsExItem[MAX_ITEM_EXC_OPTION];
	char btIsSetItem;
	char btIsSocketItem;
	char btIsElementalItem;
	BYTE btMuunEvolutionItemType;
	WORD wMuunEvolutionItemIndex;
	DWORD dwDuration;
};

struct BAG_SECTION_ITEMS
{
	BYTE btSectionID;
	BYTE btItemDropCount;
	DWORD dwSectionUseRate;
	std::vector<BAG_ITEM> m_vtItems;
};

struct BAG_SECTION_DROP
{
	WORD wUseSectionMinLevel;
	WORD wUseSectionMaxLevel;
	BYTE btUseSectionAllowedClass[MAX_TYPE_PLAYER];
	BYTE btUseSectionAllowedMapNum;
	std::map<int, BAG_SECTION_ITEMS> m_mapSectionItems;
};

struct BAG_DATA
{
	bool m_bLoadBag;
	char szBagName[255];
	DWORD dwItemDropRate;
	DWORD dwRandomSetItemDropRate;
	DWORD dwDropMoney;
	DWORD dwMinGainRuud;
	DWORD dwMaxGainRuud;
	DWORD dwGainRuudRate;
	int iBagUseEffect;
	DWORD dwBagUseRate;
	bool bAddCoinEnable;
	BYTE btAddCoinType;
	DWORD dwAddCoinValue;
	bool bSummonBookEnable;
	DWORD dwSummonBookDropRate;
	BYTE btSummonBookItemType;
	WORD wSummonBookItemIndex;
	std::vector<BAG_SECTION_DROP> m_vtSectionDrop;
};


extern const char * szFileClassName[];

class CBag
{
public:
	CBag();
	virtual ~CBag();

	void InitBag();

	bool LoadBag(const char* lpFileName);
	virtual bool IsBag(CGameObject &Obj, int iParam1, int iParam2) = 0;
	virtual bool UseBag(CGameObject &Obj, int iParam) = 0;
	virtual bool UseBag_GremoryCase(CGameObject &Obj, int iParam, BYTE btStorageType, BYTE btRewardSource, int iExpireDays) = 0;
	virtual bool CheckCondition(CGameObject &Obj, int iParam1, int iParam2) = 0;
	virtual void SetBagInfo(int iParam1, int iParam2) = 0;

	int GetDropSection(CGameObject &Obj, BAG_SECTION_DROP &pRetDrop);
	int GetItemsSection(BAG_SECTION_DROP & pDropSec, BAG_SECTION_ITEMS &pRetItems);
	int GetItem(BAG_SECTION_ITEMS &pItemsSec, BAG_ITEM & pRetItem);
	int GetReadyItemToUse(CGameObject &Obj, CItemObject & pItem, time_t & DurationItem);
	static int GetItemLevel(int MinLevel, int MaxLevel);
	void MakeBagEffectUse(CGameObject &Obj, int MonsterIndex);
	void AddCoin(CGameObject &Obj);
	void DropSummonItem(CGameObject &Obj);

	int IsBagUseEffect() { return this->m_BagData.iBagUseEffect; }

	inline int GetValueMinMax(int min, int max)
	{
		int diff;
		__try
		{
			diff = max - min;
			int level = rand() % (diff + 1);

			return level += min;
		}

		__except (diff = 1, 1)
		{
			return min;
		}
	}

protected:
	BAG_DATA m_BagData;
};

#endif
