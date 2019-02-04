////////////////////////////////////////////////////////////////////////////////
// ItemOptionTypeMng.h
#ifndef ITEMOPTIONTYPEMNG_H
#define ITEMOPTIONTYPEMNG_H

#include "zzzitem.h"
#include "user.h"

#define MAX_EXCELLENT_COMMON_OPTIONS 20
#define MAX_EXCELLENT_WING_OPTIONS 40
#define MAX_EXC_OPTION_EFFECT 7
#define MAX_ITEM_EXC_OPTION 9

enum COMMON_EXC_OPTIONS
{
	OPTION_INC_MANA_MONSTER_DIE = 0,
	OPTION_INC_LIFE_MONSTER_DIE = 1,
	OPTION_INC_ATTACK_SPEED = 2,
	OPTION_INC_ATTACK_PERCENT = 3,
	OPTION_INC_ATTACK_LEVEL = 4,
	OPTION_INC_EXCELLENT_DAMAGE = 5,
	OPTION_INC_ZEN_MONSTER_DIE = 6,
	OPTION_INC_DEFENSE_RATE = 7,
	OPTION_INC_DAMAGE_REFLECT = 8,
	OPTION_DEC_VICTIM_DAMAGE = 9,
	OPTION_INC_MAXIMUM_MANA = 10,
	OPTION_INC_MAXIMUM_LIFE = 11
};

enum EXC_OPTION_OPERATORS
{
	EXC_OPERATOR_NORMAL = 1,
	EXC_OPERATOR_PERCENT = 2,
	EXC_OPERATOR_DIVIDE_BY_LEVEL_20 = 3,
	EXC_OPERATOR_DIVIDE_BY_LIFE = 4,
	EXC_OPERATOR_DIVIDE_BY_MANA = 5,
	EXC_OPERATOR_INC_PER_LEVEL = 6,
	EXC_OPERATOR_FORMULA = 100
};

enum EXC_OPTION_EFFECTS
{
	EXC_EFFECT_ETC = 0,
	EXC_EFFECT_RATE = 1,
	EXC_EFFECT_ATTACK = 2,
	EXC_EFFECT_DEFENSE = 3,
	EXC_EFFECT_LIFE = 4,
	EXC_EFFECT_MANA = 5,
	EXC_EFFECT_SKILL = 6
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

class CItemOptionTypeMng
{
public:

	void Initialize();
	bool LoadScript(char *pchFileName);

	void Initialize_AccessoryItemOption();
	bool LoadAccessoryItemOptionScript(char *pchFileName);
	
	int CommonExcOptionRand(int ItemKindA, BYTE *btSocketExcOption);//[K2
	//int CommonExcOptionRand(int ItemKindA, BYTE *btSocketExcOption, int iOptionCount);//[K2
	int CommonExcOptionGetCount(int ItemKindA, BYTE *btSocketExcOption, int iOptionCount);
	int WingExcOptionRand(int ItemKindA, int ItemKindB, BYTE *btSocketExcOption);
	int WingExcOptionGetCount(int ItemKindA, int ItemKindB, BYTE *btSocketExcOption, int iOptionCount);

	void CalcExcOptionEffect(OBJECTSTRUCT *lpObj);
	void CalcWingOptionEffect(OBJECTSTRUCT *lpObj);

	int GetOptionEffectValue(OBJECTSTRUCT * lpObj, int iOptionType, int iOptionEffect);

	int _CalcEffectValue(OBJECTSTRUCT *lpObj, int iInputValue, int iOperator, int iBaseValue, int iFormulaID, CItem * Item);
	void _SetOptionEffect(OBJECTSTRUCT *lpObj, BYTE *OptionEffect, int iBaseValue, int iOperator, int iFormulaID, BYTE btItemPos);

	void _SetEffect_Etc(OBJECTSTRUCT *lpObj, BYTE OptionEffectID, int iBaseValue, int iOperator, int iFormulaID, BYTE btItemPos);
	void _SetEffect_Rate(OBJECTSTRUCT *lpObj, BYTE OptionEffectID, int iBaseValue, int iOperator, int iFormulaID, BYTE btItemPos);
	void _SetEffect_Attack(OBJECTSTRUCT *lpObj, BYTE OptionEffectID, int iBaseValue, int iOperator, int iFormulaID, BYTE btItemPos);
	void _SetEffect_Defense(OBJECTSTRUCT *lpObj, BYTE OptionEffectID, int iBaseValue, int iOperator, int iFormulaID, BYTE btItemPos);
	void _SetEffect_Life(OBJECTSTRUCT *lpObj, BYTE OptionEffectID, int iBaseValue, int iOperator, int iFormulaID, BYTE btItemPos);
	void _SetEffect_Mana(OBJECTSTRUCT *lpObj, BYTE OptionEffectID, int iBaseValue, int iOperator, int iFormulaID, BYTE btItemPos);
	void _SetEffect_Skill(OBJECTSTRUCT *lpObj, BYTE OptionEffectID, int iBaseValue, int iOperator, int iFormulaID, BYTE btItemPos);

	COMMON_EXT_OPTION_TYPE * GetCommonExcOption(int ExcOptionID);
	WING_EXT_OPTION_TYPE * GetWingOption(int ExcOptionID);

	COMMON_EXT_OPTION_TYPE m_CommonExtOptionType[MAX_EXCELLENT_COMMON_OPTIONS];
	WING_EXT_OPTION_TYPE m_WingExtOptionType[MAX_EXCELLENT_WING_OPTIONS];
	ACCESSORY_ITEM_OPTION_VALUE m_AccessoryItemOptionValue[6];
	EXC_OPTION_RATE m_ExcellentOptionRate;
}; 

extern CItemOptionTypeMng g_ItemOptionTypeMng;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

