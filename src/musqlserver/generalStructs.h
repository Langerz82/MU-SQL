#ifndef _GENERALSTRUCTS_H
#define _GENERALSTRUCTS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "MuDefines.h"
#include "ServerEngine.h"
#include "MuunInfo.h"
#include "ItemManagement/Item.h"
#include "itemsocketoptiondefine.h"

#include <string>
#include <map>

struct STR_ITEM_LEVEL_RATE;
struct STR_LUCKY_ITEM_EQUIPMENT;
struct STR_SMELT_RATE;
struct STR_LUCKY_ITEM_INFO;
struct STR_USER_SHOP_REBUY_ITEM;
struct STR_ITEMPERIOD_DATEINFO;
struct STR_GUILD_MEMBER;
struct STR_GUILD_INFO_STRUCT;
struct STR_UNION_MEMBER_DATA;
struct PARTYMATCHING_PARTYUSERDATA;
struct STR_sellItemPrice;
struct STR_PARTY_MEMBER_WAIT_LIST;
struct STR_PSHOP_ITEMVALUE_INFO_DS;
struct STR_STAT_BONUS;
struct STR_PARTYMATCHING_PARTYUSERDATA;
struct DevilSquareScoreInfo;
struct DevilSquareBossMonsterInfo;
struct DevilSquareMonsterInfo;
struct CUSTOM_EVENT_DROP_INFO;
struct CUSTOM_EVENT_DROP_START_TIME;
struct CUSTOM_EVENT_DROP_ITEM_INFO;
struct CUSTOM_EVENT_DROP_RULE_INFO;
struct GREMORYCASE_ITEM_DATA;
struct PMSG_GREMORYCASE_ITEM;
struct JEWELOFHARMONY_ITEM_EFFECT;
struct JEWELOFHARMONY_ITEM_OPTION;
struct ITEMEFFECT;
struct STR_STRINGCOMPARE;
struct MU_WSAOVERLAPPED;
struct MU_WSABUF;
struct _PER_IO_CONTEXT;
struct _GUILD_INFO_STRUCT;



struct ComboSkillData
{

public:

	void Init()	// line : 19
	{
		this->dwTime = 0;
		this->Skill[0] = -1;
		this->Skill[1] = -1;
		this->Skill[2] = -1;
		this->ProgressIndex = -1;
	};	// line : 23

	DWORD dwTime;	// 0
	WORD Skill[3];	// 4
	int ProgressIndex;	// 8

};


struct PSHOP_ITEMVALUE_INFO_DS {
	int nPShopItemInvenNum;
	UINT64 ItemSerial;
	int nMoney;
	WORD sBlessJewelValue;
	WORD sSoulJewelValue;
	WORD sChaosJewelValue;
};


struct ITEMDROP_ITEM
{
	BYTE btItemType;
	WORD wItemIndex;
	BYTE btItemMinLevel;
	BYTE btItemMaxLevel;
	BYTE btItemDurability;
	BYTE btIsSkill;
	BYTE btIsLuck;
	BYTE btIsOption;
	BYTE btIsExc[9];
	BYTE btIsSetItem;
	BYTE btSocketCount;
	BYTE btIsElementalItem;
	BYTE btMuunEvoItemType;
	WORD wMuunEvoItemIndex;
	DWORD dwItemDropRate;
	DWORD dwPeriodDuration;
};

struct ITEMDROP_MONSTER
{
	ITEMDROP_MONSTER()
	{
		this->m_vtItems.clear();
	}

	WORD wMonsterID;
	WORD wMonsterMinLevel;
	WORD wMonsterMaxLevel;
	BYTE btMonsterMapNumber;
	DWORD dwItemDropRate;
	BYTE btItemCount;
	bool bIsCoinReward;
	BYTE btCoinType;
	DWORD dwCoinValue;
	WORD wPlayerMinLevel;
	WORD wPlayerMaxLevel;
	std::vector<boost::shared_ptr<ITEMDROP_ITEM>> m_vtItems;
};

struct ZEN_DROP
{
	BYTE btMapNumber;
	DWORD dwMinMoney;
	DWORD dwMaxMoney;
};


struct CUSTOM_MONSTER_ATTR
{
	float HPMul;
	float DmgMul;
	float DefMul;
	float ARateMul;
	float DRateMul;
};

typedef struct MONSTER_ATTRIBUTE
{
	int m_Index;	// 0
	int m_Rate;	// 4
	char m_Name[50];	// 8
	int m_Level;	// 1C
	int m_iAINumber;	// 20
	int m_iScriptHP;	// 24
	int m_Hp;	// 28
	int m_Mp;	// 2C
	int m_DamageMin;	// 30
	int m_DamageMax;	// 34
	int m_Defense;	// 38
	int m_MagicDefense;	// 3C
	int m_AttackRating;	// 40
	int m_Successfulblocking;	// 44
	int m_MoveRange;	// 48
	int m_AttackRange;	// 4C
	int m_AttackType;	// 50
	int m_ViewRange;	// 54
	int m_MoveSpeed;	// 58
	int m_AttackSpeed;	// 5C
	int m_RegenTime;	// 60
	int m_Attribute;	// 64
	int m_ItemRate;	// 68
	int m_MoneyRate;	// 6C
	int m_MaxItemLevel;	// 70
	BYTE m_Resistance[MAX_RESISTENCE_TYPE];	// 74
	DWORD m_MonsterSkill;	// 7C
	int m_PentagramMainAttribute;
	int m_PentagramAttributePattern;
	int m_PentagramDefense;
	int m_PentagramAttackMin;
	int m_PentagramAttackMax;
	int m_PentagramAttackRating;
	int m_PentagramDefenseRating;
	BYTE m_btExpType;

} MONSTER_ATTRIBUTE, *LPMONSTER_ATTRIBUTE;


struct _ST_MINESYSTEM_REPAIR_VALUE
{
	WORD sItemType;
	WORD sItemIndex;
	WORD sLevel;
	WORD sRepairValue;
};

struct _ST_MINESYSTEM_MINE_MIRACLE_SUCCESS_INFO
{
	WORD wTwinkleType;
	int iStage;
	int iSuccessRate;
};

struct _ST_MINESYSTEM_DROP_ITEM
{
	int iItemType;
	int iItemIndex;
	int iLevel;
	int iNumber;
};

struct _ST_MINESYSTEM_MINE_SUCCESS_INFO
{
	WORD wTwinkleType;
	int iDurabilityDecrement;
	int iSuccessRate[5];
};

struct _ST_MINESYSTEM_REWARD_VALUE
{
	_ST_MINESYSTEM_REWARD_VALUE()
	{
		this->wTwinkleType = 0;
		this->iStage = 0;
		this->wType = 0;
		this->wIndex = 0;
		this->iTotalJewelNumber = 0;
		this->vecDropItem.clear();
	}

	WORD wTwinkleType;
	int iStage;
	WORD wType;
	WORD wIndex;
	int iTotalJewelNumber;
	std::vector<_ST_MINESYSTEM_DROP_ITEM> vecDropItem;
};

struct _ST_MINESYSTEM_TWINKLE
{
	_ST_MINESYSTEM_TWINKLE()
	{
		this->wTwinkleIndex = 0;
		this->wUserIndex = 0;
		this->iCurrentStage = 0;
		this->wTwinkleType = 0;
		this->bIsDominated = 0;
		this->byMapNumber = 0;
	}

	WORD wTwinkleIndex;
	WORD wTwinkleType;
	WORD wUserIndex;
	int iCurrentStage;
	int bIsDominated;
	BYTE byMapNumber;
};

struct _ST_MINESYSTEM_TWINKLE_SPAWN
{
	WORD wType;
	BYTE byMapNumber;
	BYTE byDistance;
	BYTE byX;
	BYTE byY;
	BYTE byDir;
};


typedef struct BONUS_SOCKET_OPTION
{
	BYTE OptionIndex;
	BYTE EffectType;
	BYTE ItemTypeBegin;
	BYTE ItemTypeEnd;
	BYTE OptionValueType;
	WORD OptionValue;
	int ExtractRate;
	BYTE SocketSlotType[5];
	BYTE SocketSlotCount;

	void Clear()
	{
		OptionIndex = BONUS_OPTION_WEAPON_ATTACK_POWER;
		EffectType = SEED_EFFECT_TYPE_FIRE_BEGIN;
		ItemTypeBegin = 0;
		ItemTypeEnd = 0;
		OptionValueType = 0;
		OptionValue = 0;
		ExtractRate = 0;

		for (int i = 0; i<5; i++)
		{
			SocketSlotType[i] = 0;
		}

		SocketSlotCount = 0;
	}

}BONUS_SOCKET_OPTION, *LPBONUS_SOCKET_OPTION;

struct SET_SOCKET_OPTION
{
	BYTE OptionIndex;
	BYTE OptionValueType;
	WORD OptionValue;
	BYTE EffectType;
	int ExtractRate;
	BYTE SeedTypeTable[6];
	void Clear();
};

struct CUSTOM_SOCKET_LIST
{
	int iItemCode;
};


struct SERVER_ATTRIBUTE_DEFINE
{
	void Clear()
	{
		this->ListIndex = 0;
		this->ServerIndex = 0;
		this->FireRate = 0;
		this->WaterRate = 0;
		this->WindRate = 0;
		this->EarthRate = 0;
		this->DarkRate = 0;
	}

	int ListIndex;
	int ServerIndex;
	int FireRate;
	int WaterRate;
	int WindRate;
	int EarthRate;
	int DarkRate;
};

struct PENTAGRAM_ITEM_OPEN_SOCKET_RATE
{
	void Clear()
	{
		this->SocketCount = 0;
		this->SocketOpenRate = 0;
		this->SocketOpenSet = 0;
		this->Slot_1 = 0;
		this->Slot_2 = 0;
		this->Slot_3 = 0;
		this->Slot_4 = 0;
		this->Slot_5 = 0;
	}

	int SocketCount;
	int SocketOpenRate;
	int SocketOpenSet;
	int Slot_1;
	int Slot_2;
	int Slot_3;
	int Slot_4;
	int Slot_5;
};

struct MONSTER_DROP_ITEM_RATE
{
	void Clear()
	{
		this->MonsterClass = 0;

		for (int i = 0; i<5; i++)
		{
			this->DropItemType[i] = 0;
			this->DropItemIndex[i] = 0;
			this->DropItemRate[i] = 0;
		}
	}

	int MonsterClass;
	int DropItemType[6];
	int DropItemIndex[6];
	int DropItemRate[6];
};

struct JEWEL_OUT_RATE
{
	void Clear()
	{
		this->JewelOutIndex = 0;
		this->JewelRank = 0;
		this->JewelLevel = 0;
		this->OutRate = 0;
	}

	int JewelOutIndex;
	int JewelRank;
	int JewelLevel;
	int OutRate;
};

struct PENTAGRAM_SET_EFFECT
{
	void Clear()
	{
		this->SetIndex = 0;

		for (int i = 0; i < 6; i++)
		{
			this->RON[i] = 0;
		}

		this->Value1 = 0;
		this->Value2 = 0;
		this->Value3 = 0;
		this->Value4 = 0;
	}

	int SetIndex;
	int RON[6];
	int Value1;
	int Value2;
	int Value3;
	int Value4;
};

struct PENTAGRAM_HAVE_SET_OPTION
{
	int BundleIndex;
	int ItemType;
	int ItemIndex;
	int SetOptionIndex[26];
};

struct PENTAGRAM_ITEM_OPTION
{
	void Clear()
	{
		this->ItemType = 0;
		this->ItemIndex = 0;
		memset(this->ItemName, 0, sizeof(this->ItemName));
		this->Grade = 0;

		for (int i = 0; i < 7; i++)
		{
			this->OptionNum[i] = -1;
		}
	}

	int ItemType;
	int ItemIndex;
	char ItemName[64];
	int Grade;
	int OptionNum[7];
};

struct PENTAGRAM_ITEM_OPTION_ENABLE_NEED
{
	void Clear()
	{
		this->OptionNum = 0;
		memset(this->OptionName, 0, sizeof(this->OptionName));

		for (int i = 0; i < 3; i++)
		{
			this->Need_ErrtelKind[i] = -1;
			this->Need_ErrtelRank[i] = -1;
			this->Need_ErrtelLevel[i] = -1;
		}
	}

	int OptionNum;
	char OptionName[64];
	int Need_ErrtelKind[3];
	int Need_ErrtelRank[3];
	int Need_ErrtelLevel[3];
};

struct PENTAGRAM_SOCKET_RATE_BY_GRADE
{
	void Clear()
	{
		this->Grade = 0;

		for (int i = 0; i < 5; i++)
		{
			this->SocketRate[i] = 0;
		}
	}

	int Grade;
	int SocketRate[5];
};


struct TEST_ITEMSDROP
{
	int ItemDropIndex;
	int ItemDropLevel;
	int ItemDropRate;
};


struct COMMON_EXT_OPTION_TYPE
{
	void Clear()
	{
		this->OptionID = -1;
		this->ItemKindA_1 = -1;
		this->ItemKindA_2 = -1;
		this->ItemKindA_3 = -1;
		this->OptionNumber = -1;
		this->Operator = 0;
		this->FormulaID = -1;
		this->OptionRate = 0;
		memset(this->OptionName, 0x00, 64);
		this->OptionValue = 0;
		memset(this->OptionEffect, 0, MAX_EXC_OPTION_EFFECT);
	}

	int OptionID;
	int ItemKindA_1;
	int ItemKindA_2;
	int ItemKindA_3;
	int OptionNumber;
	int Operator;
	int FormulaID;
	DWORD OptionRate;
	char OptionName[64];
	int OptionValue;
	BYTE OptionEffect[MAX_EXC_OPTION_EFFECT];
};

struct WING_EXT_OPTION_TYPE
{
	void Clear()
	{
		this->OptionID = -1;
		this->ItemKindA = -1;
		this->ItemKindB = -1;
		this->OptionNumber = -1;
		this->Operator = 0;
		this->FormulaID = -1;
		this->OptionRate = 0;
		memset(this->OptionName, 0, 64);
		this->OptionValue = 0;
		memset(this->OptionEffect, 0, MAX_EXC_OPTION_EFFECT);
	}

	int OptionID;
	int ItemKindA;
	int ItemKindB;
	int OptionNumber;
	int Operator;
	int FormulaID;
	DWORD OptionRate;
	char OptionName[64];
	int OptionValue;
	BYTE OptionEffect[MAX_EXC_OPTION_EFFECT];
};

struct ACCESSORY_ITEM_OPTION_VALUE
{
	void Clear()
	{
		this->ItemType = -1;
		this->ItemIndex = -1;
		for (int i = 0; i < 16; ++i)
			this->LevelValue[i] = -1;
	}
	int ItemType;
	int ItemIndex;
	int LevelValue[16];
};

struct WING_EXC_OPTION_RATE
{
	int ItemKindA;
	int ItemKindB;
	int WingOptionRate[9];
};

struct EXC_OPTION_RATE
{
	int m_CommonOptionRate[9];
	std::vector<WING_EXC_OPTION_RATE> m_vtWingOptionRate;
};


struct REFINE_OPTION
{
	void Clear()
	{
		this->RefineIndex = 0;
		this->RefineSuccessRate = 0;

		for (int i = 0; i < 4; i++)
		{
			this->NeedItem_Type[i] = 0;
			this->NeedItem_Index[i] = 0;
			this->NeedItem_Num[i] = 0;
		}

		this->NeedMoney = 0;
	}

	int RefineIndex;
	int RefineSuccessRate;

	int NeedItem_Type[4];
	int NeedItem_Index[4];
	int NeedItem_Num[4];

	int NeedMoney;
};

struct REFINE_SUCCESS_OPTION
{
	void Clear()
	{
		this->SuccessRefineIndex = 0;

		for (int i = 0; i < 5; i++)
		{
			this->Success_AcquisitionRate[i] = 0;
			this->Success_AcquisitionItem_Type[i] = 0;
			this->Success_AcquisitionItem_Index[i] = 0;
			this->Success_AcquisitionItem_Num[i] = 0;
		}
	}

	int SuccessRefineIndex;

	int Success_AcquisitionRate[5];
	int Success_AcquisitionItem_Type[5];
	int Success_AcquisitionItem_Index[5];
	int Success_AcquisitionItem_Num[5];
};

struct JEWEL_LEVEL_UPGRADE_NEED_ITEM
{
	void Clear()
	{
		this->TargetItemLevel = 0;
		this->MainSourceItemLevel = 0;
		this->MainSourceItemNum = 0;

		for (int i = 0; i < 4; i++)
		{
			this->NeedSourceItem_Type[i] = 0;
			this->NeedSourceItem_Index[i] = 0;
			this->NeedSourceItem_Num[i] = 0;
		}

		this->NeedMoney = 0;
	}

	int TargetItemLevel;
	int MainSourceItemLevel;
	int MainSourceItemNum;

	int NeedSourceItem_Type[4];
	int NeedSourceItem_Index[4];
	int NeedSourceItem_Num[4];

	int NeedMoney;
};

struct JEWEL_RANK_UPGRADE_NEED_ITEM
{
	void Clear()
	{
		this->TargetItemRank = 0;
		this->MainSourceItemRank = 0;
		this->MainSourceItemNum = 0;

		for (int i = 0; i < 4; i++)
		{
			this->NeedSourceItem_Type[i] = 0;
			this->NeedSourceItem_Index[i] = 0;
			this->NeedSourceItem_Num[i] = 0;
		}

		this->NeedMoney = 0;
	}

	int TargetItemRank;
	int MainSourceItemRank;
	int MainSourceItemNum;

	int NeedSourceItem_Type[4];
	int NeedSourceItem_Index[4];
	int NeedSourceItem_Num[4];

	int NeedMoney;
};

struct PENTAGRAM_JEWEL_ITEM_OPTION
{
	void Clear()
	{
		this->JewelIndex = 0;
		this->PentagramJewel_Type = 0;
		this->PentagramJewel_Index = 0;
		this->ItemRank = 0;
		this->RankOptionNum = 0;
		this->RankOptionNumRate = 0;
		this->RON = 0;

		for (int i = 0; i < 11; i++)
		{
			this->LevelValue[i] = 0;
			this->LevelUpgradeValue[i] = 0;
		}
	}

	int JewelIndex;
	int PentagramJewel_Type;
	int PentagramJewel_Index;
	int ItemRank;
	int RankOptionNum;
	int RankOptionNumRate;
	int RON;

	int LevelValue[11];
	int LevelUpgradeValue[11];
};

struct JEWEL_RANK_UPGRADE_DETERMINE_RATE
{
	void Clear()
	{
		this->RankStep = 0;

		for (int i = 0; i < 4; i++)
		{
			this->RankRate[i] = 0;
		}

		this->RankUpgradeSuccessRate = 0;
	}

	int RankStep;
	int RankRate[4];
	int RankUpgradeSuccessRate;
};


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


typedef struct ITEMEFFECT
{
	WORD wOptionType;
	WORD wEffectType1;
	WORD wEffectType2;
	int iItemNumber;
	int iItemType;
	int iItemIndex;
	int iEffectValue1;
	int iEffectValue2;
	int iEffectValidTime;

} ITEMEFFECT, *LPITEMEFFECT;

struct JEWELOFHARMONY_ITEM_OPTION
{
	BOOL bValid;	// 0
	BYTE iRandomWeight;	// 4
	BYTE iRequireLevel;	// 5
	BYTE iItemEffectValue[MAX_JOH_ITEM_OPTION];	// 6
	DWORD iZenForRestore[MAX_JOH_ITEM_OPTION];	// 14
	char szOptionName[256];	// 4C
};

struct JEWELOFHARMONY_ITEM_EFFECT
{
	short HJOpAddMinAttackDamage;	// 0
	short HJOpAddMaxAttackDamage;	// 2
	short HJOpRequireStr;	// 4
	short HJOpRequireDex;	// 6
	short HJOpAddAttackDamage;	// 8
	short HJOpAddCriticalDamage;	// A
	short HJOpAddSkillAttack;	// C
	short HJOpAddAttackSuccessRatePVP;	// E
	short HJOpDecreaseSDRate;	// 10
	short HJOpAddIgnoreSDRate;	// 12
	short HJOpAddMagicPower;	// 14
	short HJOpAddDefense;	// 16
	short HJOpAddMaxAG;	// 18
	short HJOpAddMaxHP;	// 1A
	short HJOpAddRefillHP;	// 1C
	short HJOpAddRefillMP;	// 1E
	short HJOpAddDefenseSuccessRatePvP;	// 20
	short HJOpAddDamageDecrease;	// 22
	short HJOpAddSDRate;	// 24
};


struct _stGremoryCaseItem
{
	_stGremoryCaseItem()
	{
		this->btStorageType = 0;
		this->btRewardSource = 0;
		this->btItemGUID = 0;
		this->wItemID = 0;
		this->btItemLevel = 0;
		this->btItemDurability = 0;
		this->btItemSkill = 0;
		this->btItemLuck = 0;
		this->btItemOption = 0;
		this->btItemExcOption = 0;
		this->btItemSetOption = 0;
		memset(this->btItemSocketOption, -1, sizeof(this->btItemSocketOption));
		this->btItemMainAttribute = 0;
		this->wMuunEvoItemType = 0;
		this->btMuunEvoItemIndex = 0;
		this->dwAuthCode = 0;
		this->iReceiveDate = 0;
		this->iExpireDate = 0;
	}

	BYTE btStorageType;
	BYTE btRewardSource;
	BYTE btItemGUID;
	WORD wItemID;
	BYTE btItemLevel;
	BYTE btItemDurability;
	BYTE btItemSkill;
	BYTE btItemLuck;
	BYTE btItemOption;
	BYTE btItemExcOption;
	BYTE btItemSetOption;
	BYTE btItemSocketOption[5];
	BYTE btItemMainAttribute;
	WORD wMuunEvoItemType;
	BYTE btMuunEvoItemIndex;
	DWORD dwAuthCode;
	time_t iReceiveDate;
	time_t iExpireDate;
};


struct PMSG_GREMORYCASE_ITEM
{
	BYTE btRewardInventory;
	BYTE btRewardSource;
	DWORD dwItemGUID;
	BYTE btItemInfo[MAX_ITEM_INFO];
	DWORD dwAuthCode;
	DWORD dwExpireTime;
};


struct GREMORYCASE_ITEM_DATA
{
	GREMORYCASE_ITEM_DATA()
	{
		this->Clear();
	}

	void Clear()
	{
		this->btRewardInventory = 0;
		this->btRewardSource = 0;
		this->dwItemGUID = (DWORD)-1;
		this->ItemInfo.Clear();
		this->dwAuthCode = 0;
		this->dwReceiveDate = 0;
		this->dwExpireTime = 0;
	}

	BYTE btRewardInventory;
	BYTE btRewardSource;
	DWORD dwItemGUID;
	CItem ItemInfo;
	DWORD dwAuthCode;
	DWORD dwReceiveDate;
	DWORD dwExpireTime;
};

struct DSF_TIME_RULE_INFO
{
	void Clear()
	{
		this->BeforeEnterMinute = 0;
		this->AfterEnterMinute = 0;
		this->EachRoundMinute = 0;
		this->AfterRoundSecond = 0;
		this->AfterBossRoundSecond = 0;
		this->CommentTime = 0;
	}

	int BeforeEnterMinute;
	int AfterEnterMinute;
	int EachRoundMinute;
	int AfterRoundSecond;
	int AfterBossRoundSecond;
	int CommentTime;
};

struct DSF_UNIT_INFO
{
	void Clear()
	{
		this->MonsterUnitNumber = 0;
		this->PartyMinUser = 0;
		this->MinTeamCount = 0;
		this->MinUserCount = 0;
		this->DSFMinLevel = 0;
		this->MaxEnterCount = 0;
		memset(this->MonsterRewardRate, 0x00, sizeof(this->MonsterRewardRate));
	}

	int MonsterUnitNumber;
	int PartyMinUser;
	int MinTeamCount;
	int MinUserCount;
	int DSFMinLevel;
	int MaxEnterCount;
	int MonsterRewardRate[3];
};

struct DSF_START_DAY
{
	void Clear()
	{
		this->LeagueDay1 = 0;
		this->LeagueDay2 = 0;
		this->LeagueDay3 = 0;
		this->LeagueDay4 = 0;
		this->SemiFinalDay1 = 0;
		this->SemiFinalDay2 = 0;
		this->FinalDay = 0;
	}

	int LeagueDay1;
	int LeagueDay2;
	int LeagueDay3;
	int LeagueDay4;
	int SemiFinalDay1;
	int SemiFinalDay2;
	int FinalDay;
};

struct DSF_START_HOUR_TYPE
{
	void Clear()
	{
		this->DSFHour = -1;
		this->DSFMin = -1;
		this->DSFBattleType = 0;
	}

	int DSFHour;
	int DSFMin;
	int DSFBattleType;
};

struct DSF_REWARD_DAY
{
	void Clear()
	{
		this->RewardStartDay = 0;
		this->RewardEndDay = 0;
	}

	int RewardStartDay;
	int RewardEndDay;
};

struct DSF_EVENT_MONSTER_RATE
{
	void Clear()
	{
		this->RoundNum = 0;
		this->Rate = 0;
	}

	int RoundNum;
	int Rate;
};

struct DSF_AFTER_ROUND_ADD_POINT
{
	void Clear()
	{
		this->RoundNum = 0;
		this->Point = 0;
	}

	int RoundNum;
	int Point;
};

struct DSF_MONSTER_INFO
{
	void Clear()
	{
		this->RoundNum = -1;
		this->MonsterType = -1;
		this->MonsterMaxNumber = -1;
		this->MonsterKind = -1;
		this->MonsterPoint = -1;
	}

	int RoundNum;
	int MonsterType;
	int MonsterMaxNumber;
	int MonsterKind;
	int MonsterPoint;
};

struct DSF_MONSTER_REGEN_INFO
{
	void Clear()
	{
		this->MonsterNum = -1;
		this->MonsterMapNum = -1;
		this->MonsterDis = -1;
		this->RegenX = -1;
		this->RegenY = -1;
		this->MonsterDir = -1;
		this->MonsterKind = -1;
	}

	int MonsterNum;
	int MonsterMapNum;
	int MonsterDis;
	int RegenX;
	int RegenY;
	int MonsterDir;
	int MonsterKind;
};

struct DSF_EVENT_MONSTER_INFO
{
	void Clear()
	{
		this->RoundNum = 0;
		this->MonsterType = 0;
		this->MonsterMaxNumber = 0;
		this->MonsterKind = 0;
		this->MonsterPoint = 0;
	}

	int RoundNum;
	int MonsterType;
	int MonsterMaxNumber;
	int MonsterKind;
	int MonsterPoint;
};

struct DSF_PARTY
{
	int nPartyNo;
	int nRequestUserIndex;
	int n2ndPartyUserIndex;
	char szRequestUserID[MAX_ACCOUNT_LEN + 1];
	char sz2ndPartyUserID[MAX_ACCOUNT_LEN + 1];
	char szRequestUserName[MAX_ACCOUNT_LEN + 1];
	char sz2ndPartyUserName[MAX_ACCOUNT_LEN + 1];
	char szRequestUserRealName[MAX_ACCOUNT_LEN + 1];
	char sz2ndPartyUserRealName[MAX_ACCOUNT_LEN + 1];
	int nRequestUserServerCode;
	int n2ndPartyUserServerCode;
	int nRequestUserClass;
	int n2ndPartyUserClass;
	int nRequestUserLevel;
	int n2ndPartyUserLevel;
	bool bRequest;
	bool bEntered;
	BYTE btDSFType;
	BYTE btTeamIndex;
	int nPoint;
};

struct DSF_PARTY_RANK
{
	void Clear()
	{
		memset(this->szUserName1, 0x00, sizeof(this->szUserName1));
		memset(this->szUserName2, 0x00, sizeof(this->szUserName2));
		this->wServerCode1 = -1;
		this->wServerCode2 = -1;
		this->btRank = 0;
		this->btDSFType = 0;
		this->iPoint = 0;
	}

	char szUserName1[MAX_ACCOUNT_LEN + 1];
	char szUserName2[MAX_ACCOUNT_LEN + 1];
	WORD wServerCode1;
	WORD wServerCode2;
	BYTE btRank;
	BYTE btDSFType;
	int iPoint;
};

struct DSF_USER
{
	int m_nIndex;
	BYTE m_btTeam;
	BOOL m_bReward;
	bool m_bEnterOk;
	int m_nPartyIdx;
	int m_nPoint;
	char PlayerName[MAX_ACCOUNT_LEN + 1];
};

struct DSF_ENTER_PLAYER_NAME
{
	int EnteredIndex;
	char PlayerName[MAX_ACCOUNT_LEN + 1];
};


struct CUSTOM_EVENT_DROP_START_TIME
{
	int Year;
	int Month;
	int Day;
	int DayOfWeek;
	int Hour;
	int Minute;
	int Second;
};

struct CUSTOM_EVENT_DROP_ITEM_INFO
{
	int ItemIndex;
	int ItemLevel;
	BYTE Durability;
	BYTE Skill;
	BYTE Luck;
	BYTE Option;
	BYTE Excellent;
	BYTE Ancient;
	int DropCount;
	int DropDelay;
	int DropState;
};

struct CUSTOM_EVENT_DROP_RULE_INFO
{
	char Name[32];
	int Map;
	int X;
	int Y;
	int Range;
	int AlarmTime;
	int EventTime;
	std::vector<CUSTOM_EVENT_DROP_ITEM_INFO> DropItem;
};

struct CUSTOM_EVENT_DROP_INFO
{
	int Index;
	int State;
	int RemainTime;
	int TargetTime;
	int TickCount;
	int AlarmMinSave;
	int AlarmMinLeft;
	CUSTOM_EVENT_DROP_RULE_INFO RuleInfo;
	std::vector<CUSTOM_EVENT_DROP_START_TIME> StartTime;
};

struct DevilSquareMonsterInfo
{
	WORD m_Type;	// 0
	int m_RegenStartTime;	// 4
	int m_RegenEndTime;	// 8
	int m_iDevilSquareNumber;	// C
};

struct DevilSquareBossMonsterInfo
{
	WORD m_Type;	// 0
	int m_RegenStartTime;	// 4
	int m_RegenEndTime;	// 8
	int X;	// C
	int Y;	// 10
	int TX;	// 14
	int TY;	// 18
	int m_iDevilSquareNumber;	// 1C
};


struct DevilSquareScoreInfo
{
	char Name[10];	// 0
	int TotalScore;	// C
	int BonusExp;	// 10
	int BonusZen;	// 14
};

struct STR_STRINGCOMPARE
{
	bool operator()(const std::string s1, const std::string s2) const
	{
		return strcmp(s1.data(), s2.data()) < 0;
	}
} ;

typedef std::map<std::string, STR_GUILD_MEMBER*, STR_STRINGCOMPARE> MAP_GUILD_MEMBER;
typedef std::map<std::string, STR_GUILD_INFO_STRUCT*, STR_STRINGCOMPARE> MAP_GUILD_INFO;
typedef std::map<int, STR_UNION_MEMBER_DATA*> MAP_MEMBER_DATA;



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

struct stArcaBattleMarkTopRankDS {
	BYTE btRank;
	char szGuildNames[MAX_GUILD_LEN + 1];
	DWORD dwMarkCnt;
};

struct stCancelGuildNames {
	char szGuildNames[9];
};

struct stCCFRankingInfo {
	stCCFRankingInfo() {
		memset(this->szCharName, 0x00, sizeof(this->szCharName));
		this->byRank = 0;
		this->nPoint = 0;
	}
	char szCharName[MAX_ACCOUNT_LEN + 1];
	BYTE byRank;
	int nPoint;
};

struct stGuildMatchingAllowList {
	char szName[MAX_ACCOUNT_LEN + 1];
	BYTE btClass;
	int nLevel;
};

struct stGuildMatchingAllowListDB {
	int nGuildNumber;
	char szApplicantName[MAX_ACCOUNT_LEN + 1];
	BYTE btApplicantClass;
	int nApplicantLevel;
	BYTE btState;
};

struct stGuildMatchingList {
	char szMemo[41];
	char szRegistrant[MAX_ACCOUNT_LEN + 1];
	char szGuildName[MAX_GUILD_LEN + 1];
	BYTE btGuildMemberCnt;
	BYTE btGuildMasterClass;
	BYTE btInterestType;
	BYTE btLevelRange;
	BYTE btClassType;
	int nGuildMasterLevel;
	int nBoardNumber;
	int nGuildNumber;
	BYTE btGensType;
};

struct stGuildUnderMember {
	BYTE btGuildMemberCnt;
	char szGuildNames[9];
};

struct ITL_REWARDLIST {
	BYTE byRank;
	BYTE byEnteredCnt;
	BYTE byPoint;
	bool bGotReward;
	char szCharName[MAX_ACCOUNT_LEN + 1];
	char szGuildName[MAX_GUILD_LEN + 1];
};

struct MuRummyCardInfoDS {
	MuRummyCardInfoDS() {
		this->btColor = -1;
		this->btNumber = -1;
		this->btSlotNum = -1;
		this->btSeq = -1;
		this->btStatus = -1;
	}
	BYTE btColor;
	BYTE btNumber;
	BYTE btSlotNum;
	BYTE btSeq;
	BYTE btStatus;
};

struct MuRummyCardUpdateDS {
	MuRummyCardUpdateDS() {
		this->btSlotNum = -1;
		this->btSeq = -1;
		this->btStatus = -1;
	}
	BYTE btSlotNum;
	BYTE btSeq;
	BYTE btStatus;
};

struct MuunRideViewPortInfo {
	BYTE NumberH;
	BYTE NumberL;
	BYTE MuunRideItemH;
	BYTE MuunRideItemL;
};

struct STR_QUESTEXP_INFO {
	STR_QUESTEXP_INFO::STR_QUESTEXP_INFO() {
		this->dwQuestIndexID = 0;
		this->wProgState = 0;
		this->lStartDate = 0;
		this->lEndDate = 0;
		memset(this->btAskIndex, 0, 5);
		memset(this->btAskValue, 0, 5);
		memset(this->btAskState, 0, 5);
	}
	DWORD dwQuestIndexID;
	WORD wProgState;
	BYTE btAskIndex[5];
	BYTE btAskValue[5];
	BYTE btAskState[5];
	time_t lStartDate;
	time_t lEndDate;
};

struct SEND_PARTYMEMBER_INFO {
	SEND_PARTYMEMBER_INFO() {
		this->bUse = FALSE;
	}
	int bUse;
	int bFlag;
	char Name[MAX_ACCOUNT_LEN + 1];
	int nUserIndex;
	int nServerChannel;
};

struct stABAllGuildMark {
	char szGuildName[MAX_GUILD_LEN + 1];
	DWORD dwMarkCnt;
};

struct stABJoinUserInfoDS {
	char szGuildName[MAX_GUILD_LEN + 1];
	DWORD dwGuild;
	char szUserName[MAX_ACCOUNT_LEN + 1];
};

struct stABWinGuildInfoDS {
	char szGuildName[9];
	DWORD dwGuild;
	WORD wOccupyObelisk;
	WORD wObeliskGroup;
};

struct STR_GENS_RANKING_DATA
{
	int GensClass;
	int ContributePoint;
	int RankingPosition;
	BYTE RewardItemGroup;
	WORD RewardItemNumber;
	int RewardItemCount;
};

struct _ITL_GUILD_RANK {
	char szGuildName[MAX_GUILD_LEN + 1];
	BYTE byRank;
	BYTE byITLType;
	BYTE byWin;
	BYTE byLose;
};

struct STR_GUILD_MEMBER {
	char m_szMemberName[11];
	BYTE m_btStatus;
	BYTE m_btConnected;
	int m_iUserIndex;
	STR_GUILD_MEMBER() {
		ZeroMemory(m_szMemberName, sizeof(m_szMemberName));
		m_btStatus = 0;
		m_btConnected = -1;
		m_iUserIndex = -1;
	}
};


struct PARTY_INFO_LIST {
	char szLeaderName[MAX_ACCOUNT_LEN + 1];
	char szTitle[41];
	WORD nMinLevel;
	WORD nMaxLevel;
	WORD nHuntingGround;
	int nLeaderLevel;
	BYTE btLeaderClass;
	BYTE btWantedClass;
	BYTE btCurPartyMemberCnt;
	BYTE btUsePassword;
	BYTE btWantedClassDetailInfo[7];
	int nServerChannel;
	BYTE btGensType;
};

struct PARTY_INFO_LISTDB {
	char szLeaderName[MAX_ACCOUNT_LEN + 1];
	char szTitle[41];
	char szPassword[5];
	WORD nMinLevel;
	WORD nMaxLevel;
	WORD nHuntingGround;
	int nLeaderLevel;
	BYTE btLeaderClass;
	BYTE btWantedClass;
	BYTE btCurPartyMemberCnt;
	BYTE btUsePassword;
	BYTE btWantedClassDetailInfo[7];
	BYTE btGensType;
	BYTE btApprovalType;
};

struct STR_PARTY_MEMBER_WAIT_LIST
{
	char szName[MAX_ACCOUNT_LEN + 1];
	BYTE btClass;
	int nLevel;
};

struct NAME {
	char m_szName[11];
	NAME() {
		std::ZeroMemory(this, sizeof(*this));
	}
	NAME(char * szName) {
		std::ZeroMemory(this, sizeof(*this));
		std::strncpy(m_szName, szName, 10);
	}
};

struct P_ADD_NEWS {
	BYTE day;
	BYTE month;
	WORD year;
	DWORD dateColor;
	DWORD titleColor;
	DWORD textColor;
	char title[40];
	WORD textLen;
	char Text[2048];
};

struct PARTYMATCHING_PARTYDATA {
	PARTYMATCHING_PARTYDATA() {
		this->m_PartyMatchingIndex = -1;
		this->m_UserCount = 0;
	}
	int m_PartyMatchingIndex;
	int m_UserCount;
	char szLeaderName[MAX_ACCOUNT_LEN + 1];
	//STR_PARTYMATCHING_PARTYUSERDATA m_UserData[5];
	STR_PARTYMATCHING_PARTYUSERDATA* m_UserData;
};

struct STR_PARTYMATCHING_PARTYUSERDATA {
	STR_PARTYMATCHING_PARTYUSERDATA() {
		this->iUsedInfo = FALSE;
		this->iUserConnected = FALSE;
		this->iUserIndex = -1;
		this->iServerNumber = -1;
		this->iServerIndex = -1;
	}
	int iUsedInfo;
	int iUserConnected;
	int iUserIndex;
	int iServerNumber;
	int iServerIndex;
	char szUserName[MAX_ACCOUNT_LEN + 1];
};

struct PERIOD_EXPIRED_ITEM {
	WORD wItemCode;
	UINT64 Serial;
};

struct PERIOD_EXPIRED_ITEMLIST {
	BYTE btItemCount;
	PERIOD_EXPIRED_ITEM ExpiredItem[90];
};

struct PERIOD_ITEMLIST {
	DWORD dwUserGuid;
	BYTE btItemCount;
	char chCharacterName[MAX_ACCOUNT_LEN + 1];
	//STR_ITEMPERIOD_DATEINFO ItemData[100];
	STR_ITEMPERIOD_DATEINFO* ItemData;
};

struct STR_sellItem {
	DWORD dwItemGuid;
	DWORD dwItemCODE;
	DWORD dwCategoryID;
	DWORD dwUseType;
	DWORD dwBuyType;
	DWORD dwCoolTime;
	double dbVarEndDate;
	DWORD dwLimitSellCount;
	DWORD dwState;
	char szItemName[64];
	char szItemDesc[128];
	DWORD dwPriceCount;
	//STR_sellItemPrice itemPrice[8];
	STR_sellItemPrice* itemPrice;
};

struct STR_sellItemPrice {
	DWORD dwPriceGuid;
	DWORD dwItemGuid;
	DWORD dwUseTime;
	DWORD dwAmount;
	DWORD dwPrice;
	DWORD dwSellRate;
};

struct sellPackage {
	DWORD dwPackageGuid;
	STR_sellItemPrice itemPrice;
	DWORD dwImageCode;
	DWORD dwCategory;
	DWORD dwBuyType;
	DWORD dwLimitSellCount;
	DWORD dwInItemCount;
	double dbVarEndDate;
	DWORD dwState;
	STR_sellItemPrice inItemPrices[16];
	char szPackageName[64];
	char szPackageDesc[128];
};

struct ServerInfo {
	WORD UserCount;
	WORD MaxUserCount;
	WORD Percent;
	DWORD TickCount;
};

struct ServerList {
	ServerList() {
		this->btServerCode = 0;
		this->wServerPort = 0;
		memset(this->cIp, 0, 16);
	}
	BYTE btServerCode;
	WORD wServerPort;
	char cIp[16];
};

struct STAT_BONUS {
	WORD BonusOptionID;
	WORD BonusOptionVal1;
	WORD BonusOptionVal2;
};

struct stMemberPosInfo {
	char szName[MAX_ACCOUNT_LEN + 1];
	BYTE btMapNumber;
	BYTE btPosX;
	BYTE btPosY;
};

struct _GUILD_INFO_STRUCT
{
	int Number;	//0
	char Name[9];	//4
	unsigned char Mark[32];	//D
	BYTE Count;	//2D
	BYTE TotalCount;	//2E
	char Names[MAX_USER_GUILD][11];	//2F
	short Index[MAX_USER_GUILD];	//3A0
	unsigned char Use[MAX_USER_GUILD];	//440
	char pServer[MAX_USER_GUILD];	//490
	char TargetGuildName[9];	//4E0
	short TargetIndex[MAX_USER_GUILD];	//4EA
	struct _GUILD_INFO_STRUCT* lpTargetGuildNode;	//58C
	unsigned char WarDeclareState;	//590
	unsigned char WarState;	//591
	unsigned char WarType;	//592
	unsigned char BattleGroundIndex;	//593
	unsigned char BattleTeamCode;	//594
	unsigned char PlayScore;	//595
	int TotalScore;	//598
	char Notice[60];	//59c
	int GuildStatus[MAX_USER_GUILD];	//5D8
	BYTE btGuildType;	//718
	int iGuildUnion;	//71C
	int iGuildRival;	//720
	int iTimeStamp;	//724
	char szGuildRivalName[9];	//728
	struct _GUILD_INFO_STRUCT* back;	//734
	struct _GUILD_INFO_STRUCT* next;	//738
	struct OBJECTSTRUCT* lpLifeStone;
	int btLifeStoneCount;

	// #error Deathay Fix here
	_GUILD_INFO_STRUCT() { return; };
	int  GetGuildUnion() { return this->iGuildUnion; };
	int  GetGuildRival() { return this->iGuildRival; };

	void SetGuildUnion(int iGuildNumber)	// line : 102
	{
		this->iGuildUnion = iGuildNumber;
		this->SetTimeStamp();
	};	// line : 105

	void SetGuildRival(int iGuildNumber)	// line : 108
	{
		this->iGuildRival = iGuildNumber;
		this->SetTimeStamp();
	};	// line : 111

	void SetTimeStamp()	// line : 117
	{
		this->iTimeStamp++;
	};	// line : 119

	int GetTimeStamp()	// line : 122
	{
		return this->iTimeStamp;
	};	// line : 124

	BOOL CheckTimeStamp(int iTime)	// line : 127
	{
		return (iTime == this->iTimeStamp) ? TRUE : FALSE;
	}	// line : 129
};

struct STR_GUILD_INFO_STRUCT {
	int m_iNumber;
	char m_szGuildName[9];
	char m_szGuildMaster[11];
	BYTE m_btGuildType;
	MAP_GUILD_MEMBER m_mapGuildMember;
	int m_iRivalGuild;
	int m_iUnionGuild;
	BYTE m_Mark[32];
	int m_Score;
	char m_Notice[128];
	BOOL m_bGSHasData;
	STR_GUILD_INFO_STRUCT() {
		m_iNumber = -1;
		memset(m_szGuildName, 0, sizeof(m_szGuildName));
		memset(m_szGuildMaster, 0, sizeof(m_szGuildMaster));
		m_btGuildType = -1;
		memset(m_Mark, 0, sizeof(m_Mark));
		memset(m_Notice, 0, sizeof(m_Notice));
		m_iRivalGuild = 0;
		m_iUnionGuild = 0;
		m_Score = 0;
		m_mapGuildMember.clear();
		m_bGSHasData = FALSE;
	}
};

struct JOIN_SERVER_SERVER_DATA {
	int m_ServerIndex;
	char m_szServerName[50];
	WORD m_wServerCode;
	WORD m_wPort;
	WORD m_wCurrentUser;
	WORD m_wMaxUser;
	BYTE m_VipServer;
	WORD m_MaxHWIDUseCount;
};

struct JOIN_SERVER_CUserData {
	char m_AccoundID[11];
	char m_HWID[100];
	WORD m_ServerCode;
	WORD m_ServerGroup;
	bool m_bRequestMapSvrMove;
	int iUserNumber;
	DWORD dwPlayTime;
	char m_Ip[17];
	int iPayCode;
	int iUsedTime;
	char szEndDays[9];
	DWORD dwCheckTime;
	int iIndex;
	bool m_bOffTrade;
	char IpAddr[17];
};

typedef struct STR_UNION_MEMBER_DATA {
	std::vector<int> m_vecUnionMember;
} STR_UNION_MEMBER_DATA, *LP_STR_UNION_MEMBER_DATA;

struct USERWAREHOUSE_DATA {
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	int WarehouseID;
	DWORD LastChangeTick;
	int ChangeIDEnableState;
	bool WarehouseOpenState;
};

struct STR_ITEMPERIOD_DATEINFO
{
	BYTE btUsedInfo;
	BYTE btItemType;
	WORD wItemCode;
	BYTE btEffectCategory;
	BYTE btEffectType1;
	BYTE btEffectType2;
	UINT64 Serial;
	time_t lItemBuyDate;
	time_t lItemExpireDate;
};

struct STR_STAT_BONUS
{
	WORD BonusOptionID;
	WORD BonusOptionVal1;
	WORD BonusOptionVal2;
};

struct STR_SERVER_ENGINE
{
	int m_Index;
	SOCKET m_Socket;
	eSERVER_STATE m_State;
	eSERVER_TYPE m_Type;
	char m_ServerIp[16];
	struct _PER_SOCKET_CONTEXT * PerSocketContext;
	WsProtocolCore m_ProtocolCore;
	WORD m_ServerCode;
	WORD m_ServerGroup;
	unsigned short m_ConnectPort;
};

struct IPList
{
	char m_IP[16];
};

struct STR_CS_USER
{
	STR_CS_USER() {}
	int Index;
	int ConnectionState;
	char IP[20];
	unsigned int Port;
	unsigned char Type;
	SOCKET socket;
	_PER_SOCKET_CONTEXT* PerSocketContext;
	bool News;
	int PacketCount;
	ULONGLONG i64PacketTime;
};

typedef struct tagIocpServerParameter
{
	DWORD dwServerCount;
	eSERVER_TYPE eServerType;

} IocpServerParameter;

struct _PER_SOCKET_CONTEXT
{
	SOCKET m_socket;	// 0
	int nIndex;	// 4
	_PER_IO_CONTEXT IOContext[2];	// 8
	int dwIOCount;	// 5E28
};

struct PacketQueue
{
	PacketQueue()
	{
		this->nSize = 0;
		this->headcode = 0;
		this->uindex = -1;
		this->iSessionId = -1;
	}

	boost::shared_ptr<unsigned char[]> pData;
	unsigned short nSize;
	unsigned char headcode;
	int uindex;
	int iSessionId;
};


// USER STRUCTS

typedef union tagPATHTABLE
{
	short sPathTable[16];
	char cPathTable[32];
} PATHTABLE, *LPPATHTABLE;

typedef struct tagActionState
{
	unsigned long Rest : 1;	// 0
	unsigned long Attack : 1;	// 1
	unsigned long Move : 1;	// 2
	unsigned long Escape : 1;	// 3
	unsigned long Emotion : 4;	// 4
	unsigned long EmotionCount : 8;	// 8

} ACTION_STATE, *LPACTION_STATE;

typedef struct tagInterfaceState
{
	DWORD use : 2;
	DWORD state : 4;
	DWORD type : 10;

} INTERFACE_STATE, *LPINTERFACE_STATE;

extern short RoadPathTable[MAX_ROAD_PATH_TABLE];
extern BOOL g_EnergyCheckOff;

struct HITDAMAGE_STRUCT
{
	short number;	// 0
	int HitDamage;	// 4
	ULONGLONG LastHitTime;	// 8
};

struct VIEWPORT_STRUCT
{
	char state;	// 0
	short number;	// 2
	unsigned char type;	// 4
	short index;	// 6
	int dis;	// 8
};

struct VIEWPORT_PLAYER_STRUCT
{
	char state;	// 0
	short number;	// 2
	unsigned char type;	// 4
	short index;	// 6
	int dis;	// 8

};

struct MessageStateMachine
{
	int MsgCode;	// 0
	int SendUser;	// 4
	ULONGLONG MsgTime;	// 8
	int SubCode;	// C

	MessageStateMachine()
	{
		this->MsgCode = -1;
	}

};

struct ExMessageStateMachine
{
	int MsgCode;	// 0
	int SendUser;	// 4
	ULONGLONG MsgTime;	// 8
	int SubCode;	// C
	int SubCode2;	// 10

	ExMessageStateMachine()
	{
		this->MsgCode = -1;
	}
};

struct WHISPER_STRUCT
{
	bool iWhisperSent;
	WORD wResponseCount;
};
struct SKILL_INFO
{
	void Clear()
	{
		this->GhostPhantomX = 0;
		this->GhostPhantomY = 0;
		this->RemedyOfLoveEffect = 0;
		this->RemedyOfLoveTime = 0;
		this->LordSummonTime = 0;
		this->LordSummonMapNumber = 0;
		this->LordSummonX = 0;
		this->LordSummonY = 0;
		this->SoulBarrierDefence = 0;
		this->SoulBarrierManaRate = 0;
		this->PoisonType = 0;
		this->IceType = 0;
		this->fInfinityArrowIncRate = 0.0;
		this->fCircleShieldRate = 0.0;
	}

	BYTE	GhostPhantomX;
	BYTE	GhostPhantomY;
	short	RemedyOfLoveEffect;
	short	RemedyOfLoveTime;
	WORD	LordSummonTime;
	BYTE	LordSummonMapNumber;
	BYTE	LordSummonX;
	BYTE	LordSummonY;
	int		SoulBarrierDefence;
	int		SoulBarrierManaRate;
	BYTE	PoisonType;
	BYTE	IceType;
	float	fInfinityArrowIncRate;
	float	fCircleShieldRate;
};

struct _MASTERLEVEL_PASSIVE_SKILL_OPTION
{
	_MASTERLEVEL_PASSIVE_SKILL_OPTION()
	{
		this->Clear();
	}

	void Clear()
	{
		iMpsAttackSuccessRate = 0.0;
		iMpsIncDefenseSuccessRate = 0.0;
		iMpsIncreasePvPAttackRate = 0.0;
		iMpsIncreasePvPDefenseRate = 0.0;
		iMpsDurabilityReduction1 = 0.0;
		btMpsDownDur1Level = 0.0;
		iMpsDurabilityReduction2 = 0.0;
		iMpsDurabilityReduction3 = 0.0;
		iMpsResistancePoison = 0.0;
		iMpsResistanceThunder = 0.0;
		iMpsResistanceIce = 0.0;
		iMpsAutoRecoverLife = 0.0;
		iMpsAutoRecoverMana = 0.0;
		iMpsAutoRecoverAG = 0.0;
		iMpsAddPhysicDamage = 0.0;
		iMpsAddDamage = 0.0;
		iMpsTwoHandSwordAddDamage = 0.0;
		iMpsSwordAddDamage = 0.0;
		iMpsMaceStrength = 0.0;
		iMpsSpearAddDamage = 0.0;
		iMpsTwoHandsPvPAttack = 0.0;
		iMpsAddAttackSpeed = 0.0;
		iMpsIgnoreEnemyDefence = 0.0;
		iMpsAddDoubleDamage = 0.0;
		iMpsAddSturn = 0.0;
		iMpsMaxManaRate = 0.0;
		iMpsAddMagicDamage = 0.0;
		iMpsTwoHandsMagicPower = 0.0;
		iMpsStaffMagicPower = 0.0;
		iMpsIncShield = 0.0;
		iMpsStaffAttackSpeed = 0.0;
		iMpsTwoHandsStaffManAttack = 0.0;
		iMpsShieldBlockRate = 0.0;
		iMpsBowAddDamage = 0.0;
		iMpsBowAddAttackSpeed = 0.0;
		iMpsTwoHandsBowAddDamage = 0.0;
		iMpsTwoHandsBowManAttack = 0.0;
		iMpsElfAddShield = 0.0;
		iMpsElfShieldBlockRate = 0.0;
		iMpsElfAddPhysicDamage = 0.0;
		iMpsCallMonHp = 0.0;
		iMpsCallMonDefense = 0.0;
		iMpsCallMonAttack = 0.0;
		iMpsIncMagicPower = 0.0;
		iMpsIncDamageFireProper = 0.0;
		iMpsIncDamageWindProper = 0.0;
		iMpsIncDamageThunderProper = 0.0;
		iMpsIncDotDamage = 0.0;
		iMpsAddSturn2 = 0.0;
		iMpsAddSpring = 0.0;
		iMpsAddMagicPowerStick = 0.0;
		iMpsAddCurseMagicBook = 0.0;
		iMpsAddManAttackStick = 0.0;
		iMpsAddAttackSpeedMagicBook = 0.0;
		iMpsIncPercentBerserker1 = 0.0;
		iMpsIncPercentBerserker2 = 0.0;
		iMpsIncValueBerserker3 = 0.0;
		iMpsIncMinMagicDamage = 0.0;
		iMpsIncDarkHorseDefense = 0.0;
		iMpsAddForceWaveDamage = 0.0;
		iMpsAddDarkSpiritDamage = 0.0;
		iMpsAddCriticalDamageRate = 0.0;
		iMpsAddExcellentDamageRate = 0.0;
		iMpsAddCeptorDamage = 0.0;
		iMpsIncCeptorManAttack = 0.0;
		iMpsAddShieldDefense = 0.0;
		iMpsAddShieldBlockingRage = 0.0;
		iMpsAddPetAttack = 0.0;
		iMpsAddControlAttack = 0.0;
		iMpsAddMinAttack = 0.0;
		iMpsAddMaxAttack = 0.0;
		iMpsCriticalRateInc = 0.0;
		iMpsAddBlockingRate = 0.0;
		iMpsMonkWeaponDamage = 0.0;
		iMpsMonkDoubleDamage = 0.0;
		iMpsMonkAddVitalToDefense = 0.0;
		iMpsPlusZen = 0.0;
		iMpsDefence = 0.0;
		iMpsMaxHP = 0.0;
		iMpsMaxBP = 0.0;
		iMpsMaxMana = 0.0;
		iMpsMonsterDieGetMana = 0.0;
		iMpsMonsterDieGetLife = 0.0;
		iMpsMonsterDieGetSD = 0.0;
		iMpsPlusExp = 0.0;
		iMpsMaxSD = 0.0;
		iMpsSDSpeed = 0.0;
		iMpsMaxAttackDamage = 0.0;
		iMpsMinAttackDamage = 0.0;
		iMpsDecreaseMana = 0.0;
		iMpsMaxMagicDamage = 0.0;
		iMpsMinMagicDamage = 0.0;
		iMpsPetDurDownSpeed = 0.0;
		iMpsMaxAttackMagicDamage = 0.0;
		iMpsMinAttackMagicDamage = 0.0;
		iMpsImmuneRate = 0.0;
		iMpsIncSetItemDefense = 0.0;
		iMpsReturnEnemyAttack = 0.0;
		iMpsIncEnergyStat = 0.0;
		iMpsIncVitalStat = 0.0;
		iMpsIncDexStat = 0.0;
		iMpsIncPowerStat = 0.0;
		iMpsAddWingDamageBlock = 0.0;
		iMpsAddWingDefense = 0.0;
		iMpsAddWingDamage = 0.0;
		iMpsIncLeadershipStat = 0.0;
		iMpsIncCriticalDamageRate = 0.0;
		iMpsRecoverManaRate = 0.0;
		iMpsRecoverHPRate = 0.0;
		iMpsIncExcellentDamageRate = 0.0;
		iMpsIncDoubleDamageRate = 0.0;
		iMpsIncIgnoreEnemyBlock = 0.0;
		iMpsRecoverSDRate = 0.0;
		iMpsIncTripleDamageRate = 0.0;
		iMpsComboDamage = 0.0;
		iMpsIncMaxBP = 0.0;
		iMpsMasManaRate_Wizard = 0.0;
		iMpsIncMaxMagicDamage = 0.0;
		iMpsIncExcellentDamageRate_Darklord = 0.0;
		iMpsIncDamageBloodHowling = 0.0;
		iMpsIncDarkSpiritAttackSpeed = 0.0;
		iMpsIncDarkSpiritDoubleDamageRate = 0.0;
		iMpsDarkSpiritIgnoreEnemyDefense = 0.0;
		iMpsIncSteelArmor = 0.0;
		iMpsShieldBlock = 0.0;
		iMpsRageDamage = 0.0;
	}

	float	iMpsAttackSuccessRate; // done
	float	iMpsIncDefenseSuccessRate; // done
	float	iMpsIncreasePvPAttackRate; // done
	float	iMpsIncreasePvPDefenseRate; // done
	float	iMpsDurabilityReduction1; // done
	BYTE	btMpsDownDur1Level; // done
	float	iMpsDurabilityReduction2; // done
	float	iMpsDurabilityReduction3; // done
	float	iMpsResistancePoison; // done
	float	iMpsResistanceThunder; // done
	float	iMpsResistanceIce; // done
	float	iMpsAutoRecoverLife; // done
	float	iMpsAutoRecoverMana; // done
	float	iMpsAutoRecoverAG; // done
	float	iMpsAddPhysicDamage; // done
	float	iMpsAddDamage; // not used?
	float	iMpsTwoHandSwordAddDamage; // done
	float	iMpsSwordAddDamage; // done
	float	iMpsMaceStrength; // done
	float	iMpsSpearAddDamage; // done
	float	iMpsTwoHandsPvPAttack; // done
	float	iMpsAddAttackSpeed; // done
	float	iMpsIgnoreEnemyDefence; // done, but no reference in CMasterLevelSkillTreeSystem class
	float	iMpsAddDoubleDamage; // done
	float	iMpsAddSturn; // done
	float	iMpsMaxManaRate; // done
	float	iMpsAddMagicDamage; // done
	float	iMpsTwoHandsMagicPower; // done
	float	iMpsStaffMagicPower; // done
	float	iMpsIncShield; // done
	float	iMpsStaffAttackSpeed; // done
	float	iMpsTwoHandsStaffManAttack;  // done
	float	iMpsShieldBlockRate; // done
	float	iMpsBowAddDamage; // done
	float	iMpsBowAddAttackSpeed; // done
	float	iMpsTwoHandsBowAddDamage; // done
	float	iMpsTwoHandsBowManAttack; // done
	float	iMpsElfAddShield; // done
	float	iMpsElfShieldBlockRate; // done
	float	iMpsElfAddPhysicDamage; // done
	float	iMpsCallMonHp; // done
	float	iMpsCallMonDefense; // done
	float	iMpsCallMonAttack; // done
	float	iMpsIncMagicPower; // done
	float	iMpsIncDamageFireProper; // done
	float	iMpsIncDamageWindProper; // done
	float	iMpsIncDamageThunderProper; // done
	float	iMpsIncDotDamage; // done
	float	iMpsAddSturn2; // done
	float	iMpsAddSpring; // done
	float	iMpsAddMagicPowerStick; // done
	float	iMpsAddCurseMagicBook; // done
	float	iMpsAddManAttackStick; // done
	float	iMpsAddAttackSpeedMagicBook; // done
	float	iMpsIncPercentBerserker1; // done
	float	iMpsIncPercentBerserker2; // done
	float	iMpsIncValueBerserker3; // done
	float	iMpsIncMinMagicDamage; // done
	float	iMpsIncDarkHorseDefense; // done
	float	iMpsAddForceWaveDamage; // not used
	float	iMpsAddDarkSpiritDamage; // done
	float	iMpsAddCriticalDamageRate; // done
	float	iMpsAddExcellentDamageRate; // done
	float	iMpsAddCeptorDamage; // done
	float	iMpsIncCeptorManAttack; // done
	float	iMpsAddShieldDefense; // done
	float	iMpsAddShieldBlockingRage; // done
	float	iMpsAddPetAttack; // done
	float	iMpsAddControlAttack; // done
	float	iMpsAddMinAttack; // done
	float	iMpsAddMaxAttack; // done
	float	iMpsCriticalRateInc; // done
	float	iMpsAddBlockingRate; // done
	float	iMpsMonkWeaponDamage; // done
	float	iMpsMonkDoubleDamage; // done
	float	iMpsMonkAddVitalToDefense; // done
	float	iMpsPlusZen; // done, but no reference in CMasterLevelSkillTreeSystem class
	float	iMpsDefence; // done
	float	iMpsMaxHP; // done
	float	iMpsMaxBP; // done
	float	iMpsMaxMana; // done
	float	iMpsMonsterDieGetMana; // done
	float	iMpsMonsterDieGetLife; // done
	float	iMpsMonsterDieGetSD; // done
	float	iMpsPlusExp; // not used
	float	iMpsMaxSD; // done
	float	iMpsSDSpeed; // done
	float	iMpsMaxAttackDamage; // done
	float	iMpsMinAttackDamage; // done
	float	iMpsDecreaseMana; // done
	float	iMpsMaxMagicDamage; // done
	float	iMpsMinMagicDamage; // done
	float	iMpsPetDurDownSpeed; // done
	float	iMpsMaxAttackMagicDamage; // done, but no reference in CMasterLevelSkillTreeSystem class
	float	iMpsMinAttackMagicDamage; // done, but no reference in CMasterLevelSkillTreeSystem class
	float	iMpsImmuneRate; // done
	float	iMpsIncSetItemDefense; // done
	float	iMpsReturnEnemyAttack; // done
	float	iMpsIncEnergyStat; // done
	float	iMpsIncVitalStat; // done
	float	iMpsIncDexStat; // done
	float	iMpsIncPowerStat; // done
	float	iMpsAddWingDamageBlock; // done
	float	iMpsAddWingDefense; // done
	float	iMpsAddWingDamage; // done
	float	iMpsIncLeadershipStat; // done
	float	iMpsIncCriticalDamageRate; // done
	float	iMpsRecoverManaRate; // done
	float	iMpsRecoverHPRate; // done
	float	iMpsIncExcellentDamageRate; // done
	float	iMpsIncDoubleDamageRate; // done
	float	iMpsIncIgnoreEnemyBlock; // done
	float	iMpsRecoverSDRate; // done
	float	iMpsIncTripleDamageRate; // done
	float	iMpsComboDamage; // done
	float	iMpsIncMaxBP; // done
	float	iMpsMasManaRate_Wizard; // done
	float	iMpsIncMaxMagicDamage; // done
	float	iMpsIncExcellentDamageRate_Darklord; // done
	float	iMpsIncDamageBloodHowling; // done
	float	iMpsIncDarkSpiritAttackSpeed; // done
	float	iMpsIncDarkSpiritDoubleDamageRate; // done
	float	iMpsDarkSpiritIgnoreEnemyDefense; // done
	float	iMpsIncSteelArmor; // done
	float	iMpsShieldBlock; // done
	float	iMpsRageDamage; // done
};

struct EFFECTLIST
{
	BYTE BuffIndex;
	BYTE EffectCategory;
	BYTE EffectType1;
	BYTE EffectType2;
	int EffectValue1;
	int EffectValue2;
	ULONGLONG EffectSetTime;
	int EffectDuration;
};

#pragma pack (1)
struct PENTAGRAMJEWEL_INFO
{
	BYTE btJewelPos;
	BYTE btJewelIndex;
	BYTE btMainAttribute;
	BYTE btItemType;
	WORD wItemIndex;
	BYTE btLevel;
	BYTE btRank1OptionNum;
	BYTE btRank1Level;
	BYTE btRank2OptionNum;
	BYTE btRank2Level;
	BYTE btRank3OptionNum;
	BYTE btRank3Level;
	BYTE btRank4OptionNum;
	BYTE btRank4Level;
	BYTE btRank5OptionNum;
	BYTE btRank5Level;

	void Clear()
	{
		this->btJewelPos = -1;
		this->btJewelIndex = -1;
		this->btMainAttribute = -1;
		this->btItemType = -1;
		this->wItemIndex = -1;
		this->btLevel = 0;
		this->btRank1OptionNum = -1;
		this->btRank1Level = -1;
		this->btRank2OptionNum = -1;
		this->btRank2Level = -1;
		this->btRank3OptionNum = -1;
		this->btRank3Level = -1;
		this->btRank4OptionNum = -1;
		this->btRank4Level = -1;
		this->btRank5OptionNum = -1;
		this->btRank5Level = -1;
	}
};
#pragma pack ()

struct PENTAGRAM_OPTION
{
	int m_iRuby_1RankAddDamage;
	int m_iRuby_2RankOptionNum;
	int m_iRuby_2RankAddAttackRelationshipRate;
	int m_iRuby_3RankOptionNum;
	int m_iRuby_3RankAddDamage;
	int m_iRuby_4RankOptionNum;
	int m_iRuby_4RankAddDamage;
	int m_iRuby_5RankOptionNum;
	int m_iRuby_5RankCriticalDamageRate;
	int m_iSapph_1RankAddDefense;
	int m_iSapph_2RankOptionNum;
	int m_iSapph_2RankAddDefenseRelationshipRate;
	int m_iSapph_3RankOptionNum;
	int m_iSapph_3RankAddDefense;
	int m_iSapph_4RankOptionNum;
	int m_iSapph_4RankAddDefense;
	int m_iSapph_5RankOptionNum;
	int m_iSapph_5RankMinusTargetDamageRate;
	int m_iEme_1RankAddAttackRate;
	int m_iEme_2RankOptionNum;
	int m_iEme_2RankAddAttackRelationshipRate;
	int m_iEme_3RankOptionNum;
	int m_iEme_3RankAddDamage;
	int m_iEme_4RankOptionNum;
	int m_iEme_4RankAddDamage;
	int m_iEme_5RankOptionNum;
	int m_iEme_5RankAddDamageRate;
	int m_iTopa_1RankOptionNum;
	int m_iTopa_1RankAddDefenseSuccessRate;
	int m_iTopa_2RankOptionNum;
	int m_iTopa_2RankAddDefenseRelationshipRate;
	int m_iTopa_3RankOptionNum;
	int m_iTopa_3RankAddDefense;
	int m_iTopa_4RankOptionNum;
	int m_iTopa_4RankAddDefense;
	int m_iTopa_5RankOptionNum;
	int m_iTopa_5RankDamageAbsorbRate;
	int m_iOnyx_1RankSlowMoveSkillRate;
	int m_iOnyx_2RankAddStrength;
	int m_iOnyx_2RankAddDexterity;
	int m_iOnyx_2RankAddEnergy;
	int m_iOnyx_2RankAddVitality;
	int m_iOnyx_3RankAddMaxLife;
	int m_iOnyx_3RankAddMaxMana;
	int m_iOnyx_3RankAddMaxAG;
	int m_iOnyx_3RankAddMaxSD;
	int m_iOnyx_4RankOptionNum;
	int m_iOnyx_4RankAddExllentDamageRate;
	int m_iOnyx_5RankHalfValueSkillRate;
	bool m_isAddPentaAttack;
	bool m_isAddPentaDefense;
	bool m_isAddCriPentaDamage;
	bool m_isChangePentaDefense;
	bool m_isChangePentaAttack;
	bool m_isAddResistByStrongRelationShip;
	bool m_isAddResistByPentaAttack;
};

struct _BOT_BUFF_LIST
{
	WORD wBuffId;
	WORD wDuration;
	int iEffect;
	WORD wEffectType;
};

struct EXC_WING_OPTION
{
	EXC_WING_OPTION()
	{
		this->Clear();
	}

	void Clear()
	{
		this->iWingOpIgnoreEnemyDefense = 0;
		this->iWingOpReturnEnemyDamage = 0;
		this->iWingOpRecoveryHP = 0;
		this->iWingOpRecoveryMana = 0;
		this->iWingOpAddDoubleDamage = 0;
	}

	int iWingOpIgnoreEnemyDefense;
	int iWingOpReturnEnemyDamage;
	int iWingOpRecoveryHP;
	int iWingOpRecoveryMana;
	int iWingOpAddDoubleDamage;
};

struct STAT_USER_OPTION
{
	STAT_USER_OPTION()
	{
		this->Clear();
	}

	void Clear()
	{
		this->StatOptionID = 0;
		this->StatOptionValMin = 0;
		this->StatOptionValMax = 0;
	}

	WORD StatOptionID;
	WORD StatOptionValMin;
	WORD StatOptionValMax;
};

struct MUUN_EFFECT_LIST
{
	MUUN_EFFECT_LIST()
	{
		this->Clear();
	}

	void Clear()
	{
		this->nIndex = 0;
		this->nMuunItemNum = 0;
		this->nCategory = 0;
		this->bOptEnable = 0;
		this->nOptType = 0;
		this->nOptValue = 0;
		this->bAddOptEnable = 0;
		this->nAddOptType = 0;
		this->nAddOptValue = 0;
		this->nSetTime = 0;
		this->nDuration = 0;
		this->pCMuunInfo = NULL;
		this->nTotalVal = 0;
		this->nSkillDelayTime = 0;
		this->bSkillUsed = false;
		this->nTargetIndex = -1;
		this->nTickTime = 0;
	}

	int nIndex;
	int nMuunItemNum;
	char nCategory;
	bool bOptEnable;
	char nOptType;
	int nOptValue;
	bool bAddOptEnable;
	char nAddOptType;
	int nAddOptValue;
	DWORD nSetTime;
	int nDuration;
	CMuunInfo *pCMuunInfo;
	int nTotalVal;
	int nSkillDelayTime;
	int nTickTime;
	bool bSkillUsed;
	int nTargetIndex;
};

struct MOVE_MAPSERVER_AUTHINFO
{
	char szCharName[MAX_ACCOUNT_LEN + 2];
	int iJA1;
	int iJA2;
	int iJA3;
	int iJA4;
};

struct CharacterNameOfUBF
{
	void Clear()
	{
		memset(this->szCharacterName, 0, MAX_ACCOUNT_LEN + 1);
		memset(this->szCharacterNameOfUBF, 0, MAX_ACCOUNT_LEN + 1);
		this->nServerCodeOfHomeWorld = 0;
	}

	char szCharacterName[MAX_ACCOUNT_LEN + 1];
	char szCharacterNameOfUBF[MAX_ACCOUNT_LEN + 1];
	WORD nServerCodeOfHomeWorld;
};

struct MU_WSAOVERLAPPED {
	DWORD    Internal;
	DWORD    InternalHigh;
	DWORD    Offset;
	DWORD    OffsetHigh;
	DWORD	 hEvent;
};

struct MU_WSABUF {
	ULONG len;
	CHAR* buf;
};

// END USER STRUCTS

struct _PER_IO_CONTEXT
{
	MU_WSAOVERLAPPED m_Overlapped; // 0
	MU_WSABUF m_wsabuf;
	unsigned char Buffer[MAX_IO_BUFFER_SIZE]; // 1C
	unsigned char BufferSecond[MAX_IO_BUFFER_SIZE]; // 178C
	int nSecondOfs; // 2EFC
	int nTotalBytes;	// 2F00
	int nSentBytes; // 2F04
	int IOOperation; // 2F08
	int nWaitIO; // 2F0C

};

#endif
