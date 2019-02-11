////////////////////////////////////////////////////////////////////////////////
// Bag.h
#ifndef _MU_BAG_H
#define _MU_BAG_H

#pragma once

#include "StdAfx.h"
#include "ItemManagement/Item.h"
#include "ItemOptionTypeMng.h"

extern const char * szFileClassName[];

class CBag
{
public:
	CBag();
	virtual ~CBag();

	void InitBag();

	bool LoadBag(const char* lpFileName);
	virtual bool IsBag(LPGameObject &lpObj, int iParam1, int iParam2) = 0;
	virtual bool UseBag(LPGameObject &lpObj, int iParam) = 0;
	virtual bool UseBag_GremoryCase(LPGameObject &lpObj, int iParam, BYTE btStorageType, BYTE btRewardSource, int iExpireDays) = 0;
	virtual bool CheckCondition(LPGameObject &lpObj, int iParam1, int iParam2) = 0;
	virtual void SetBagInfo(int iParam1, int iParam2) = 0;

	int GetDropSection(LPGameObject &lpObj, BAG_SECTION_DROP &pRetDrop);
	int GetItemsSection(BAG_SECTION_DROP & pDropSec, BAG_SECTION_ITEMS &pRetItems);
	int GetItem(BAG_SECTION_ITEMS &pItemsSec, BAG_ITEM & pRetItem);
	int GetReadyItemToUse(LPGameObject &lpObj, CItem & pItem, time_t & DurationItem);
	static int GetItemLevel(int MinLevel, int MaxLevel);
	void MakeBagEffectUse(LPGameObject &lpObj, int MonsterIndex);
	void AddCoin(LPGameObject &lpObj);
	void DropSummonItem(LPGameObject &lpObj);

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
