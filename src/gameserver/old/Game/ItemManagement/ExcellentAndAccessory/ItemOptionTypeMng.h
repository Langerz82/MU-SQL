#ifndef _MU_ITEMOPTIONTYPEMNG_H
#define _MU_ITEMOPTIONTYPEMNG_H

#include "StdAfx.h"
#include "ItemObject.h"
#include "User/CUserData.h"


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


class CItemObjectOptionTypeMng
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

	void CalcExcOptionEffect(CGameObject &Obj);
	void CalcWingOptionEffect(CGameObject &Obj);

	int GetOptionEffectValue(CGameObject &Obj, int iOptionType, int iOptionEffect);

	int _CalcEffectValue(CGameObject lpObj, int iInputValue, int iOperator, int iBaseValue, int iFormulaID, CItemObject * Item);
	void _SetOptionEffect(CGameObject lpObj, BYTE *OptionEffect, int iBaseValue, int iOperator, int iFormulaID, BYTE btItemPos);

	void _SetEffect_Etc(CGameObject lpObj, BYTE OptionEffectID, int iBaseValue, int iOperator, int iFormulaID, BYTE btItemPos);
	void _SetEffect_Rate(CGameObject lpObj, BYTE OptionEffectID, int iBaseValue, int iOperator, int iFormulaID, BYTE btItemPos);
	void _SetEffect_Attack(CGameObject lpObj, BYTE OptionEffectID, int iBaseValue, int iOperator, int iFormulaID, BYTE btItemPos);
	void _SetEffect_Defense(CGameObject lpObj, BYTE OptionEffectID, int iBaseValue, int iOperator, int iFormulaID, BYTE btItemPos);
	void _SetEffect_Life(CGameObject lpObj, BYTE OptionEffectID, int iBaseValue, int iOperator, int iFormulaID, BYTE btItemPos);
	void _SetEffect_Mana(CGameObject lpObj, BYTE OptionEffectID, int iBaseValue, int iOperator, int iFormulaID, BYTE btItemPos);
	void _SetEffect_Skill(CGameObject lpObj, BYTE OptionEffectID, int iBaseValue, int iOperator, int iFormulaID, BYTE btItemPos);

	COMMON_EXT_OPTION_TYPE * GetCommonExcOption(int ExcOptionID);
	WING_EXT_OPTION_TYPE * GetWingOption(int ExcOptionID);

	COMMON_EXT_OPTION_TYPE m_CommonExtOptionType[MAX_EXCELLENT_COMMON_OPTIONS];
	WING_EXT_OPTION_TYPE m_WingExtOptionType[MAX_EXCELLENT_WING_OPTIONS];
	ACCESSORY_ITEM_OPTION_VALUE m_AccessoryItemOptionValue[6];
	EXC_OPTION_RATE m_ExcellentOptionRate;
}; 

extern CItemObjectOptionTypeMng g_ItemOptionTypeMng;

#endif
