////////////////////////////////////////////////////////////////////////////////
// classdef.h
// ------------------------------
// Decompiled by Deathway
// Date : 2007-05-09
// ------------------------------
#ifndef CLASSDEF_H
#define CLASSDEF_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000.

//#include "zzzitem.h"
#include "prodef.h"

#define MAX_PLAYER_EQUIPMENT 12	// need to redo agin aññ with this macro

enum DBClassNumber
{
	CLASS_WIZARDEVO = 2,
	CLASS_KNIGHTEVO = 18,
	CLASS_ELFEVO = 34,
	CLASS_MAGICEVO = 50,
	CLASS_DARKLORDEVO = 66,
	CLASS_SUMMONEREVO = 82,
	CLASS_RAGEFIGHTEREVO = 98,
	CLASS_GROWLANCEREVO = 114
};

enum ClassNumber
{
	CLASS_WIZARD = 0x0,
	CLASS_KNIGHT = 0x1,
	CLASS_ELF = 0x2,
	CLASS_MAGUMSA = 0x3,
	CLASS_DARKLORD = 0x4,
	CLASS_SUMMONER = 0x5,
	CLASS_RAGEFIGHTER = 0x6,
	CLASS_GROWLANCER = 0x7
};

#define MAX_ITEM_SPECIAL_ATTRIBUTE 7
#define MAX_SOCKET_OPTION 5
struct CItem
{
public:
	CItem()
	{
		this.Clear();
	};
	void Clear();

	UINT64 m_Number; // 0
	char m_serial;	// 4
	short m_Type; // 6
	short m_Level; // 8
	BYTE m_Part;	// A
	BYTE m_Class;	// B
	BYTE m_TwoHand;	// C
	BYTE m_AttackSpeed;	// D
	BYTE m_WalkSpeed;	// E
	WORD m_DamageMin;	// 10
	WORD m_DamageMax;	// 12
	BYTE m_SuccessfulBlocking;	// 14
	WORD m_Defense;	// 16
	WORD m_MagicDefense;	// 18
	BYTE m_Speed;	// 1A
	WORD m_DamageMinOrigin;	// 1C
	WORD m_DefenseOrigin;	// 1E
	WORD m_Magic;	// 20
	WORD m_CurseSpell;
	float m_Durability; // 24
	WORD m_DurabilitySmall; // 28
	float m_BaseDurability;	// 2C
	BYTE m_SpecialNum;	// 30
	WORD m_Special[8];	// 31
	BYTE m_SpecialValue[8];	// 39
	WORD m_RequireStrength;	// 42
	WORD m_RequireDexterity;	// 44
	WORD m_RequireEnergy;	// 46
	WORD m_RequireLevel;	// 48
	WORD m_RequireVitality;	// 4A
	WORD m_RequireLeaderShip;	// 4C
	WORD m_Leadership;	// 4E
	BYTE m_RequireClass[MAX_TYPE_PLAYER];	// 50
	BYTE m_Resistance[MAX_ITEM_SPECIAL_ATTRIBUTE];	// 55
	int m_Value;	// 5C
	UINT64 m_SellMoney;	// 60
	UINT64 m_BuyMoney;	// 64
	int m_iPShopValue;	// 68
	bool m_bItemExist;	// 6C
	int m_OldSellMoney;	// 70
	int m_OldBuyMoney;	// 74
	BYTE m_Option1;	// 78  Skill
	BYTE m_Option2;	// 79  Luck
	BYTE m_Option3;	// 7A  Option
	BYTE m_NewOption;	// 7B ExcellentOption
	float m_DurabilityState[4];	// 7C
	float m_CurrentDurabilityState;	// 8C
	bool m_SkillChange;	// 90
	bool m_QuestItem;	// 91
	BYTE m_SetOption;	// 92
	BYTE m_SetAddStat;	// 93
	bool m_IsValidItem;	// 94
	BYTE m_SkillResistance[MAX_ITEM_SPECIAL_ATTRIBUTE];	// 95
	BOOL m_IsLoadPetItemInfo;	// 9C
	int  m_PetItem_Level;	// A0
	UINT64 m_PetItem_Exp; // A4
	BYTE m_JewelOfHarmonyOption;	// A8
	WORD m_HJOpStrength;	// AA
	WORD m_HJOpDexterity;	// AC
	BYTE m_ItemOptionEx;	// AE
	BYTE m_BonusSocketOption;
	BYTE m_SocketOption[MAX_SOCKET_OPTION];
	BYTE m_ImproveDurabilityRate;
	BYTE m_PeriodItemOption; // bit0 : period item flag, bit1 : expired item
	short m_wPShopBlessValue;
	short m_wPShopSoulValue;
	short m_wPShopChaosValue;
};

struct DEFAULTCLASSTYPE
{
	int Experience;	// 0
	WORD Strength;	// 4
	WORD Dexterity;	// 6
	WORD Vitality;	// 8
	WORD Energy;	// A
	float LevelLife;	// C
	float Life;	// 10
	float MaxLife;	// 14
	float LevelMana;	// 18
	float Mana;	// 1C
	float MaxMana;	// 20
	float VitalityToLife;	// 24
	float EnergyToMana;	// 28
	CItem Equipment[MAX_PLAYER_EQUIPMENT+2];	// 2C
	WORD Leadership;	// 9CC
};

struct PMSG_REQ_CLASSDEF
{
	PBMSG_HEAD h;
};

struct CLASSDEF_DATA
{
	int Class;
	int Str;
	int Dex;
	int Vit;
	int Energy;
	float Life;
	float Mana;
	float LevelLife;
	float LevelMana;
	float VitalityToLife;
	float EnergyToMana;
	int Leadership;
};

struct PMSG_ANS_CLASSDEF
{
	PWMSG_HEAD h;
	int iCount;
	CLASSDEF_DATA m_ClassData[MAX_TYPE_PLAYER];
};

class classdef
{
public:

	classdef();
	virtual ~classdef();

	void GDReqClassDefData();
	void DGAnsClassDefData(PMSG_ANS_CLASSDEF *lpMsg);

	void SetCharacter(int Class, int Str, int Dex, int Vit, int Energy, float Life, float Mana, float LevelLife, float LevelMana, float VitalityToLife, float EnergyToMana, int Leadership);
	void SetEquipment(int Class);
	int GetDefPoint(int char_class);

public:

	DEFAULTCLASSTYPE DefClass[MAX_TYPE_PLAYER];	// 4
	
};

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

