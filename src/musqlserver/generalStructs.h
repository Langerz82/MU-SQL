#ifndef _GENERALSTRUCTS_H
#define _GENERALSTRUCTS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"

#include <string>
#include <map>

struct STR_ITEM_LEVEL_RATE;
struct STR_LUCKY_ITEM_EQUIPMENT;
struct STR_SMELT_RATE;
struct STR_LUCKY_ITEM_INFO;
struct STR_USER_SHOP_REBUY_ITEM;


struct STR_ITEM_LEVEL_RATE
{
	int iLevel;
	int	iRate;

	STR_ITEM_LEVEL_RATE::STR_ITEM_LEVEL_RATE()
	{
		this->iLevel		= 0;
		this->iRate			= 0;
	}
};

struct STR_LUCKY_ITEM_EQUIPMENT
{
	int	iItemNum;
	int	iSkill;
	int	iLuckOption;
	int	iAddOption;
	int	iClass;

	STR_LUCKY_ITEM_EQUIPMENT::STR_LUCKY_ITEM_EQUIPMENT()
	{
		this->iItemNum		= -1;
		this->iSkill		= 0;
		this->iLuckOption	= 0;
		this->iAddOption	= 0;
		this->iClass		= -1;
	}
};

struct STR_SMELT_RATE
{
	int	iMinDurab;
	int	iMaxDurab;
	int	iSmeltRate;

	STR_SMELT_RATE::STR_SMELT_RATE()
	{
		this->iMinDurab		= 0;
		this->iMaxDurab		= 0;
		this->iSmeltRate	= 0;
	}
};

struct STR_LUCKY_ITEM_INFO
{
	int						iItemNum;
	int						iItemKind;
	int						iAddOpt3Rate[7];
	int						iAddSetOptRate[2];
	STR_ITEM_LEVEL_RATE		ItemLevelRate[MAX_LUCKYINFO_LEVELRATE];
	STR_LUCKY_ITEM_EQUIPMENT	LuckyItemEquipment[MAX_LUCKYINFO_ITEMEQUIPMENT];
	STR_SMELT_RATE				SmeltRate[MAX_LUCKYINFO_SMELTRATE];

	STR_LUCKY_ITEM_INFO::STR_LUCKY_ITEM_INFO()
	{
		this->iItemNum	= -1;
		this->iItemKind	= -1;
		memset(this->iAddOpt3Rate, 0, sizeof(iAddOpt3Rate));
	}
};

struct STR_USER_SHOP_REBUY_ITEM
{
	STR_USER_SHOP_REBUY_ITEM()
	{
		this->Clear();
	}

	void Clear()
	{
		this->btUsed = FALSE;
		this->wItemCode = 0xFFFF;
		this->iLeftTime = 0;
		memset(this->btItemInfo, 0x00, MAX_ITEM_INFO);
		this->wItemCount = 0;
		this->dwItemPrice = 0;
		this->btItemNumber = 0;
	}

	BYTE btUsed;
	WORD wItemCode;
	int iLeftTime;
	BYTE btItemInfo[MAX_ITEM_INFO];
	WORD wItemCount;
	DWORD dwItemPrice;
	BYTE btItemNumber;
};


#endif