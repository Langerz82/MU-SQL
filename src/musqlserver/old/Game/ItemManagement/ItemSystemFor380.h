// ItemSystemFor380.h: interface for the CItemSystemFor380 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMSYSTEMFOR380_H__F545D1C3_2C57_497F_961A_77B689252FFE__INCLUDED_)
#define AFX_ITEMSYSTEMFOR380_H__F545D1C3_2C57_497F_961A_77B689252FFE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "generalStructs.h"
#include "CGameObject.h"
#include "ItemManagement/Item.h"

#define ITEMOPTION_FOR380ITEM_EFFECT_NONE							0
#define ITEMOPTION_FOR380ITEM_EFFECT_OPADDATTACKSUCCESSRATEPVP		1
#define ITEMOPTION_FOR380ITEM_EFFECT_OPADDDAMAGE					2
#define ITEMOPTION_FOR380ITEM_EFFECT_OPADDDEFENSESUCCESSRATEPVP		3
#define ITEMOPTION_FOR380ITEM_EFFECT_OPADDDEFENSE					4
#define ITEMOPTION_FOR380ITEM_EFFECT_OPADDMAXHP						5
#define ITEMOPTION_FOR380ITEM_EFFECT_OPADDMAXSD						6
#define ITEMOPTION_FOR380ITEM_EFFECT_OPREFILLON						7
#define ITEMOPTION_FOR380ITEM_EFFECT_OPADDREFILLSD					8

class CGameObject;
//class Citem;

struct ITEMOPTION_FOR380ITEM_EFFECT;
struct ITEMOPTION_FOR380ITEM;

class CItemSystemFor380
{

public:

	CItemSystemFor380();
	virtual ~CItemSystemFor380();

	BOOL Load380ItemOptionInfo(LPSTR filename);
	BOOL Is380Item( CItem*  pItem);
	BOOL Is380OptionItem( CItem* pItem);
	void InitEffectValue( ITEMOPTION_FOR380ITEM_EFFECT* pItemEffect);
	int ApplyFor380Option(CGameObject &Obj);
	void SetOptionItemByMacro(CGameObject &Obj, BYTE invenrotyTargetPos, int bOption);
	int ChaosMix380ItemOption(CGameObject &Obj);

private:

	void _InitOption();
	int _CalcItemEffectValue(int iItemOptionType, int iItemEffectValue, ITEMOPTION_FOR380ITEM_EFFECT* pItemEffect);
	int _SetOption(CItem* pItem, int bOption);

private:

	ITEMOPTION_FOR380ITEM* m_itemOption;	// 4
	BOOL m_bSystemFor380ItemOption;	// 20004
	int m_iNeedJewelOfHarmonyCount;	// 20008
	int m_iNeedJewelOfSuhoCount;	// 2000C
	int m_iNeedZenFor380Option;	// 20010
	int m_iRateSuccessRateForMix1;	// 20014
	int m_iRateSuccessRateForMix2;	// 20018
	int m_iRateSuccessRateForMix3;	// 2001C
	int m_iRateSuccessRateForMix4;

};

extern CItemSystemFor380 g_kItemSystemFor380;

#endif // !defined(AFX_ITEMSYSTEMFOR380_H__F545D1C3_2C57_497F_961A_77B689252FFE__INCLUDED_)


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

