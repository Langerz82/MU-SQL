#ifndef _MU_ITEMOPTIONTYPEMNG_H
#define _MU_ITEMOPTIONTYPEMNG_H

#include "StdAfx.h"
#include "ItemManagement/Item.h"
#include "User/CUserData.h"


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

	void CalcExcOptionEffect(LPGameObject &lpObj);
	void CalcWingOptionEffect(LPGameObject &lpObj);

	int GetOptionEffectValue(LPGameObject &lpObj, int iOptionType, int iOptionEffect);

	int _CalcEffectValue(LPGameObject lpObj, int iInputValue, int iOperator, int iBaseValue, int iFormulaID, CItem * Item);
	void _SetOptionEffect(LPGameObject lpObj, BYTE *OptionEffect, int iBaseValue, int iOperator, int iFormulaID, BYTE btItemPos);

	void _SetEffect_Etc(LPGameObject lpObj, BYTE OptionEffectID, int iBaseValue, int iOperator, int iFormulaID, BYTE btItemPos);
	void _SetEffect_Rate(LPGameObject lpObj, BYTE OptionEffectID, int iBaseValue, int iOperator, int iFormulaID, BYTE btItemPos);
	void _SetEffect_Attack(LPGameObject lpObj, BYTE OptionEffectID, int iBaseValue, int iOperator, int iFormulaID, BYTE btItemPos);
	void _SetEffect_Defense(LPGameObject lpObj, BYTE OptionEffectID, int iBaseValue, int iOperator, int iFormulaID, BYTE btItemPos);
	void _SetEffect_Life(LPGameObject lpObj, BYTE OptionEffectID, int iBaseValue, int iOperator, int iFormulaID, BYTE btItemPos);
	void _SetEffect_Mana(LPGameObject lpObj, BYTE OptionEffectID, int iBaseValue, int iOperator, int iFormulaID, BYTE btItemPos);
	void _SetEffect_Skill(LPGameObject lpObj, BYTE OptionEffectID, int iBaseValue, int iOperator, int iFormulaID, BYTE btItemPos);

	COMMON_EXT_OPTION_TYPE * GetCommonExcOption(int ExcOptionID);
	WING_EXT_OPTION_TYPE * GetWingOption(int ExcOptionID);

	COMMON_EXT_OPTION_TYPE m_CommonExtOptionType[MAX_EXCELLENT_COMMON_OPTIONS];
	WING_EXT_OPTION_TYPE m_WingExtOptionType[MAX_EXCELLENT_WING_OPTIONS];
	ACCESSORY_ITEM_OPTION_VALUE m_AccessoryItemOptionValue[6];
	EXC_OPTION_RATE m_ExcellentOptionRate;
}; 

extern CItemOptionTypeMng g_ItemOptionTypeMng;

#endif
