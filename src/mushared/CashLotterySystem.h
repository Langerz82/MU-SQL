////////////////////////////////////////////////////////////////////////////////
// CashLotterySystem.h
#ifndef CASHLOTTERYSYSTEM_H__
#define CASHLOTTERYSYSTEM_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TRandomPoolMgr.h"
#include "zzzitem.h"

#define MAX_LOTTERY_ITEM_CATEGORY	23
#define MAX_LOTTERY_ITEM_COUNT	130

struct LOTTERY_ITEM_OPTION_RATE
{
	int iSkillOptionRate;	// 0
	int iLuckOptionRate;	// 4
	int iAddOptionRate;	// 8
	int iExOptionRate;	// C
};

struct LOTTERY_ITEMLIST
{
	WORD wItemIndex;	// 0
	BYTE btItemType;	// 2
	BYTE btItemLevelMin;	// 3
	BYTE btItemLevelMax;	// 4
	BYTE btSkillOption;	// 5
	BYTE btLuckOption;	// 6
	BYTE btAddOption;	// 7
	BYTE btExOption;	// 8
};


class CCashLotterySystem
{

public:

	CCashLotterySystem();
	virtual ~CCashLotterySystem();

	void Initialize();
	void Load(LPSTR pchFilename);
	void SetitemDropRate(int iCategory, int iDropRate, int iSkillOptionRate, int iLuckOptionRate, int iAddOptionRate, int iExOptionRate);
	BOOL InsertItem(int iItemCategory, int iItemType, int iItemIndex, int iItemLevelMin, int iItemLevelMax, int iSkillOption, int iLuckOption, int iAddOption, int iExOption);
	int GetItem(CItem* lpItem);
	void GetRequireInvenSize(int* piHeight, int* piWidth);

private:

	TRandomPoolMgr CategoryRandomPool;	// 4
	TRandomPoolMgr AddOptionRandomPool;	// 1C
	TRandomPoolMgr ExOptionRandomPool;	// 34
	int iItemMaxHeight;	// 4C
	int iItemMaxWidth;	// 50
	int LotteryItemListCount[MAX_LOTTERY_ITEM_CATEGORY];	// 54
	LOTTERY_ITEM_OPTION_RATE LotteryItemOptionRate[MAX_LOTTERY_ITEM_CATEGORY];	// A4
	LOTTERY_ITEMLIST LotteryItemList[MAX_LOTTERY_ITEM_CATEGORY][MAX_LOTTERY_ITEM_COUNT];	// 1E4
};

//extern CCashLotterySystem g_CashLotterySystem;

#define MAX_LOTTERY	5

struct LOTTERY
{
	BYTE InUse;
	char Name[20];
	CCashLotterySystem* pLottery;
};

class CCashLottery
{
public:
	CCashLottery();
	virtual ~CCashLottery();

	void Init();
	BYTE LoadLottery(const char* LotteryName, char* szFileName);
	CCashLotterySystem* GetLottery(const char* szName);
	int GetItem(const char* szName, CItem* lpItem);
protected:
	LOTTERY Lottery[MAX_LOTTERY];
};

extern CCashLottery g_Lottery;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

