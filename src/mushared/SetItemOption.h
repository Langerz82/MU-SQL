////////////////////////////////////////////////////////////////////////////////
// SetItemOption.h
// ------------------------------
// Decompiled by Deathway
// Date : 2007-05-09
// ------------------------------
#ifndef SETITEMOPTION_H
#define SETITEMOPTION_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "zzzitem.h"

#define MAX_SETITEM_OPTION			150
#define OPTION_TABLE_SIZE			6
#define SPECIAL_OPTION_TABLE_SIZE	2
#define FULL_OPTION_TABLE_SIZE		5
#define MAX_SETITEM_OPTION_INDEX	4 // Max Tiers

enum eOptionType
{
	AT_IMPROVE_DAMAGE = 0x50,
	AT_IMPROVE_MAGIC = 0x51,
	AT_IMPROVE_BLOCKING = 0x52,
	AT_IMPROVE_DEFENSE = 0x53,
	AT_LUCK = 0x54,
	AT_LIFE_REGENERATION = 0x55,
	AT_IMPROVE_LIFE = 0x56,
	AT_IMPROVE_MANA = 0x57,
	AT_DECREASE_DAMAGE = 0x58,
	AT_REFLECTION_DAMAGE = 0x59,
	AT_IMPROVE_BLOCKING_PERCENT = 0x5a,
	AT_IMPROVE_GAIN_GOLD = 0x5b,
	AT_EXCELLENT_DAMAGE = 0x5c,
	AT_IMPROVE_DAMAGE_LEVEL = 0x5d,
	AT_IMPROVE_DAMAGE_PERCENT = 0x5e,
	AT_IMPROVE_MAGIC_LEVEL = 0x5f,
	AT_IMPROVE_MAGIC_PERCENT = 0x60,
	AT_IMPROVE_ATTACK_SPEED = 0x61,
	AT_IMPROVE_GAIN_LIFE = 0x62,
	AT_IMPROVE_GAIN_MANA = 0x63,
	AT_PLUS_WING_MAXLIFE = 0x64,
	AT_PLUS_WING_MAXMANA = 0x65,
	AT_PLUS_ONE_PERCENT_DAMAGE = 0x66,
	AT_IMPROVE_AG_MAX = 0x67,
	AT_DAMAGE_ABSORB = 0x68,
	AT_PLUS_WING_ADD_LEADERSHIP = 0x69,
	AT_PLUS_FENRIR_INCREASE_LAST_DAMAGE = 0x6a,
	AT_PLUS_FENRIR_DECREASE_LAST_DAMAGE = 0x6b,
	AT_ATTACK1 = 0x78,
	AT_ATTACK2 = 0x79,
	AT_STAND1 = 0x7a,
	AT_STAND2 = 0x7b,
	AT_MOVE1 = 0x7c,
	AT_MOVE2 = 0x7d,
	AT_DAMAGE1 = 0x7e,
	AT_DIE1 = 0x7f,
	AT_SIT1 = 0x80,
	AT_POSE1 = 0x81,
	AT_HEALING1 = 0x82,
	AT_GREETING1 = 0x83,
	AT_GOODBYE1 = 0x84,
	AT_CLAP1 = 0x85,
	AT_GESTURE1 = 0x86,
	AT_DIRECTION1 = 0x87,
	AT_UNKNOWN1 = 0x88,
	AT_CRY1 = 0x89,
	AT_CHEER1 = 0x8a,
	AT_AWKWARD1 = 0x8b,
	AT_SEE1 = 0x8c,
	AT_WIN1 = 0x8d,
	AT_SMILE1 = 0x8e,
	AT_SLEEP1 = 0x8f,
	AT_COLD1 = 0x90,
	AT_AGAIN1 = 0x91,
	AT_RESPECT1 = 0x92,
	AT_SALUTE1 = 0x93,
	AT_RUSH1 = 0x94,
	AT_SET_RING_OPTION_ADD_SKILLDAMAGE = 0x95,
	AT_SET_OPTION_IMPROVE_STRENGTH = 0xa0,
	AT_SET_OPTION_IMPROVE_DEXTERITY = 0xa1,
	AT_SET_OPTION_IMPROVE_ENERGY = 0xa2,
	AT_SET_OPTION_IMPROVE_VITALITY = 0xa3,
	AT_SET_OPTION_IMPROVE_LEADERSHIP = 0xa4,
	AT_SET_OPTION_IMPROVE_MINATTACKDAMAGE = 0xa5,
	AT_SET_OPTION_IMPROVE_MAXATTACKDAMAGE = 0xa6,
	AT_SET_OPTION_IMPROVE_MAGICDAMAGE = 0xa7,
	AT_SET_OPTION_IMPROVE_DAMAGE = 0xa8,
	AT_SET_OPTION_IMPROVE_ATTACKRATE = 0xa9,
	AT_SET_OPTION_IMPROVE_DEFENCE = 0xaa,
	AT_SET_OPTION_IMPROVE_MAXLIFE = 0xab,
	AT_SET_OPTION_IMPROVE_MAXMANA = 0xac,
	AT_SET_OPTION_IMPROVE_MAXAG = 0xad,
	AT_SET_OPTION_IMPROVE_AG_VALUE = 0xae,
	AT_SET_OPTION_IMPROVE_CRITICALDAMAGE_SUCCESS = 0xaf,
	AT_SET_OPTION_IMPROVE_CRITICALDAMAGE = 0xb0,
	AT_SET_OPTION_IMPROVE_EX_DAMAGE_SUCCESS = 0xb1,
	AT_SET_OPTION_IMPROVE_EX_DAMAGE = 0xb2,
	AT_SET_OPTION_IMPROVE_SKILLDAMAGE = 0xb3,
	AT_SET_OPTION_DOUBLE_DAMAGE = 0xb4,
	AT_SET_OPTION_DEFENCE_IGNORE = 0xb5,
	AT_SET_OPTION_IMPROVE_SHIELD_DEFENCE = 0xb6,
	AT_SET_OPTION_TWO_HAND_SWORD_IMPROVE_DAMAGE = 0xb7,
	AT_SET_OPTION_IMPROVE_ALL_STATS = 0xb8,
	AT_SET_OPTION_IMPROVE_BASE_DEFENSE = 0xb9,
	AT_SET_OPTION_IMPROVE_TOTAL_ATTACKDAMAGE = 0xba,
	AT_SET_OPTION_IMPROVE_TOTAL_MAGICDAMAGE = 0xbb,
	AT_SET_OPTION_IMPROVE_MAGICDAMAGE_WITH_ENERGY = 0xbc,
	AT_SET_OPTION_ICE_MASTERY = 0xbd,
	AT_SET_OPTION_POSION_MASTERY = 0xbe,
	AT_SET_OPTION_THUNDER_MASTERY = 0xbf,
	AT_SET_OPTION_FIRE_MASTERY = 0xc0,
	AT_SET_OPTION_EARTH_MASTERY = 0xc1,
	AT_SET_OPTION_WIND_MASTERY = 0xc2,
	AT_SET_OPTION_WATER_MASTERY = 0xc3,
	AT_SET_IMPROVE_STRENGTH = 0xc4,
	AT_SET_IMPROVE_DEXTERITY = 0xc5,
	AT_SET_IMPROVE_ENERGY = 0xc6,
	AT_SET_IMPROVE_VITALITY = 0xc7,
};

typedef struct SETITEM_TYPEINFO
{
	BOOL IsLoad;	// 0
	int OptionIndex[MAX_SETITEM_OPTION_INDEX];	// 4
	int ChaosMixLevel[2];	// C

} SETITEM_TYPEINFO, *LPSETITEM_TYPEINFO;


typedef struct SETITEM_OPTIONINFO	// 50
{
	BOOL IsLoad;	// 0
	char Name[255];	// 4
	int SetOptionCount; // 104
	int SetOptionTable[OPTION_TABLE_SIZE][2];	// 108
	int SetOptionTableValue[OPTION_TABLE_SIZE][2];	// 138
	int SetExPOptionTable[2];	// 168
	int SetExPOptionTableValue[2];	// 170
	int SetFullOtionTable[FULL_OPTION_TABLE_SIZE];	// 178
	int SetFullOtionTableValue[FULL_OPTION_TABLE_SIZE];	// 18C
	BYTE RequireClass[MAX_TYPE_PLAYER];	// 1A0

} SETITEM_OPTIONINFO, *LPSETITEM_OPTIONINFO;

typedef struct SETITEM_DROPINFO
{
	BYTE  IsItemFound[MAX_TYPE_ITEMS];
	DWORD dwSectionDropRate[MAX_TYPE_ITEMS];
	DWORD TotalDropRate;

} SETITEM_DROPINFO, *LPSETITEM_DROPINFO;

class CSetItemOption
{

public:

	CSetItemOption();
	virtual ~CSetItemOption();

	BOOL LoadTypeInfo(LPSTR filename);
	BOOL LoadOptionInfo(LPSTR filename);
	BOOL IsSetItem(int itemnum);
	int GetOptionTableIndex(int itemnum, int setoption);
	LPSTR GetSetOptionName(int itemnum, int settype);
	LPSTR GetSetOptionName(int optionindex);
	int GetSetOption(int optionindex, int optablenum, int& op1, int& op2, int& opvalue1, int& opvalue2, int Class, int ChangeUp);
	int GetMaxSetOptionCount(int optionindex);
	int GetGetFullSetOption(int optionindex, int& op1, int& op2, int& op3, int& op4, int& op5, int& opvalue1, int& opvalue2, int& opvalue3, int& opvalue4, int& opvalue5, int Class, int ChangeUp);
	int IsRequireClass(int optionindex, int Class, int ChangeUp);
	int CheckMixContition(int itemnum, int mixitemlevel);
	int MakeSetOption(int itemnum, int mixitemlevel);
	int GenRandomItemNum();
	int GenRandomItemNumByCat(int iCat);
	int GenSetOption(int itemnum);
	int	LuckyItemGenSetOption(int itemnum, int * piAddSetOptRate);

private:

	SETITEM_TYPEINFO m_SetItemTypeInfo[MAX_ITEMS];	// 4
	SETITEM_OPTIONINFO m_SetItemOptionInfo[MAX_SETITEM_OPTION];	// 28004
	SETITEM_DROPINFO m_SetItemDropInfo;

};

extern CSetItemOption gSetItemOption;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

