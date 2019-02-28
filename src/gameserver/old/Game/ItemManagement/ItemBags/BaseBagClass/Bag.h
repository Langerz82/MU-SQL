////////////////////////////////////////////////////////////////////////////////
// Bag.h
#ifndef _MU_BAG_H
#define _MU_BAG_H

#pragma once

#include "StdAfx.h"
#include "ItemObject.h"
#include "ItemOptionTypeMng.h"

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
