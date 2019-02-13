#ifndef _MU_ITEMOPTIONTYPEMNG_H
#define _MU_ITEMOPTIONTYPEMNG_H

#include "StdAfx.h"
#include "ItemObject.h"
#include "User/CUserData.h"


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

	void CalcExcOptionEffect(CGameObject &lpObj);
	void CalcWingOptionEffect(CGameObject &lpObj);

	int GetOptionEffectValue(CGameObject &lpObj, int iOptionType, int iOptionEffect);

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
