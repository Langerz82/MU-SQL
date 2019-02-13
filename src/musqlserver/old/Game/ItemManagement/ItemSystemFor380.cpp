// ItemSystemFor380.cpp: implementation of the CItemSystemFor380 class.
//	GS-N	1.00.18	JPN	0x00570590	-	Completed
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ItemSystemFor380.h"
#include "CastleSiegeSync.h"
#include "ChaosBox.h"
#include "JewelOfHarmonySystem.h"


CItemSystemFor380 g_kItemSystemFor380;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemSystemFor380::CItemSystemFor380()
{
	//memset(this->m_itemOption, 0, sizeof(this->m_itemOption));
	this->m_itemOption = new ITEMOPTION_FOR380ITEM[MAX_ITEMS];
}

CItemSystemFor380::~CItemSystemFor380()
{
	delete[] this->m_itemOption;
	return;
}

void CItemSystemFor380::_InitOption()
{
	memset(this->m_itemOption, 0, sizeof(this->m_itemOption));

	this->m_iNeedJewelOfHarmonyCount = 1;
	this->m_iNeedJewelOfSuhoCount = 1;
	this->m_iNeedZenFor380Option = 10000000;
	this->m_iRateSuccessRateForMix1 = 50;
	this->m_iRateSuccessRateForMix2 = 60;
	this->m_iRateSuccessRateForMix3 = 70;
	this->m_iRateSuccessRateForMix4 = 80;
}



BOOL CItemSystemFor380::Load380ItemOptionInfo(LPSTR filename)
{
	this->_InitOption();

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(filename);

	if (res.status != pugi::status_ok)
	{
		sLog->outError("Failed to load %s file (%s)", filename, res.description());
		return FALSE;
	}

	pugi::xml_node mainXML = file.child("Item380Option");
	pugi::xml_node mix = mainXML.child("Mix");

	this->m_bSystemFor380ItemOption = mix.attribute("Enable").as_bool();
	this->m_iNeedJewelOfHarmonyCount = mix.attribute("JewelOfHarmonyCount").as_int();
	this->m_iNeedJewelOfSuhoCount = mix.attribute("JewelOfGuardianCount").as_int();
	this->m_iNeedZenFor380Option = mix.attribute("ReqMoney").as_int();

	pugi::xml_node successrate = mix.child("SuccessRate");

	this->m_iRateSuccessRateForMix1 = successrate.attribute("Grade1").as_int();
	this->m_iRateSuccessRateForMix2 = successrate.attribute("Grade2").as_int();
	this->m_iRateSuccessRateForMix3 = successrate.attribute("Grade3").as_int();
	this->m_iRateSuccessRateForMix4 = successrate.attribute("Grade4").as_int();

	pugi::xml_node item_option = mainXML.child("ItemOption");

	for (pugi::xml_node item = item_option.child("Item"); item; item = item.next_sibling())
	{
		int iItemType = item.attribute("Cat").as_int();
		int iItemIndex = item.attribute("Index").as_int();
		int iItemOption1 = item.attribute("Option1").as_int();
		int iItemValue1 = item.attribute("Value1").as_int();
		int iItemOption2 = item.attribute("Option2").as_int();
		int iItemValue2 = item.attribute("Value2").as_int();

		ITEMOPTION_FOR380ITEM * p = &this->m_itemOption[ITEMGET(iItemType, iItemIndex)];
		p->IsLoad = TRUE;
		p->m_Type = iItemType;
		p->m_Index = iItemIndex;
		p->m_Option1 = iItemOption1;
		p->m_Value1 = iItemValue1;
		p->m_Option2 = iItemOption2;
		p->m_Value2 = iItemValue2;
	}

	return TRUE;
}




BOOL CItemSystemFor380::Is380Item(CItemObject*  pItem)
{
	if (pItem == NULL)
	{
		return FALSE;
	}

	if (pItem->m_Type < 0 || pItem->m_Type > MAX_ITEMS - 1)
	{
		sLog->outBasic("[380Item] OptionItem Check error: (iItemNum:%d)", pItem->m_Type);
		return FALSE;
	}

	if (GetItemGroup(pItem->m_Type) != ITEMTYPE_380)
		return FALSE;

	if (this->m_itemOption[pItem->m_Type].IsLoad == FALSE)
		return FALSE;

	return TRUE;
}

BOOL CItemSystemFor380::Is380OptionItem(CItemObject*  pItem)
{
	if (pItem == NULL)
		return FALSE;

	if ( this->Is380Item(pItem) == FALSE )
		return FALSE;

	BYTE bResult = (pItem->m_ItemOptionEx & 0x80) >> 7;

	if ( bResult != 1 )
		return FALSE;

	return TRUE;
}

void CItemSystemFor380::InitEffectValue(ITEMOPTION_FOR380ITEM_EFFECT * pItemEffect)
{
	pItemEffect->OpAddAttackSuccessRatePVP = 0;
	pItemEffect->OpAddDamage = 0;
	pItemEffect->OpAddDefenseSuccessRatePvP = 0;
	pItemEffect->OpAddDefense = 0;
	pItemEffect->OpAddMaxHP = 0;
	pItemEffect->OpAddMaxSD = 0;
	pItemEffect->OpRefillOn  = 0;
	pItemEffect->OpAddRefillSD = 0;
}


BOOL CItemSystemFor380::ApplyFor380Option(CGameObject &Obj)
{
	ITEMOPTION_FOR380ITEM_EFFECT * pItemEffect = Obj.m_PlayerData->m_ItemOptionExFor380;
	this->InitEffectValue(pItemEffect);
	int iItemIndex = 0;

	for ( iItemIndex = 0; iItemIndex < INVETORY_WEAR_SIZE ; iItemIndex++)
	{
		CItemObject * pItem = &Obj.pInventory[iItemIndex];

		if ( pItem->IsItem() &&
			 pItem->m_IsValidItem &&
			 this->Is380OptionItem(pItem) == TRUE )
		{
			BYTE iOption1, iOption2;
			WORD iValue1, iValue2;

			iOption1 = this->m_itemOption[pItem->m_Type ].m_Option1;
			iValue1 = this->m_itemOption[pItem->m_Type ].m_Value1;
			iOption2 = this->m_itemOption[pItem->m_Type ].m_Option2;
			iValue2 = this->m_itemOption[pItem->m_Type ].m_Value2;
			BOOL bResult = FALSE;
			bResult = this->_CalcItemEffectValue(iOption1, iValue1, pItemEffect);
			bResult |= this->_CalcItemEffectValue(iOption2, iValue2, pItemEffect);
		}
	}

	Obj.AddLife += pItemEffect->OpAddMaxHP;
	Obj.iAddShield += pItemEffect->OpAddMaxSD;

	return TRUE;
}

BOOL CItemSystemFor380::_CalcItemEffectValue(int iItemOptionType, int iItemEffectValue, ITEMOPTION_FOR380ITEM_EFFECT * pItemEffect)
{
	if ( iItemOptionType == 0 )
		return FALSE;

	BOOL bResult = TRUE;

	switch ( iItemOptionType )
	{
		//case ITEMOPTION_FOR380ITEM_EFFECT_NONE:
		case ITEMOPTION_FOR380ITEM_EFFECT_OPADDATTACKSUCCESSRATEPVP:
			pItemEffect->OpAddAttackSuccessRatePVP += iItemEffectValue;
			break;
		case ITEMOPTION_FOR380ITEM_EFFECT_OPADDDAMAGE:
			pItemEffect->OpAddDamage += iItemEffectValue;
			break;
		case ITEMOPTION_FOR380ITEM_EFFECT_OPADDDEFENSESUCCESSRATEPVP:
			pItemEffect->OpAddDefenseSuccessRatePvP += iItemEffectValue;
			break;
		case ITEMOPTION_FOR380ITEM_EFFECT_OPADDDEFENSE:
			pItemEffect->OpAddDefense += iItemEffectValue;
			break;
		case ITEMOPTION_FOR380ITEM_EFFECT_OPADDMAXHP:
			pItemEffect->OpAddMaxHP += iItemEffectValue;
			break;
		case ITEMOPTION_FOR380ITEM_EFFECT_OPADDMAXSD:
			pItemEffect->OpAddMaxSD += iItemEffectValue;
			break;
		case ITEMOPTION_FOR380ITEM_EFFECT_OPREFILLON:
			pItemEffect->OpRefillOn += 1;
			break;
		case ITEMOPTION_FOR380ITEM_EFFECT_OPADDREFILLSD:
			pItemEffect->OpAddRefillSD += iItemEffectValue;
			break;
		default:
			bResult = FALSE;
			break;
	}

	return bResult;
}

BOOL CItemSystemFor380::_SetOption(CItemObject * pItem, BOOL bOption)
{
	if (pItem == NULL)
		return FALSE;

	if ( !this->Is380Item(pItem) )
		return FALSE;

	if ( bOption  == TRUE )
		pItem->m_ItemOptionEx |= 0x80;
	else
		pItem->m_ItemOptionEx &= ~0x80;

	return TRUE;
}


void CItemSystemFor380::SetOptionItemByMacro(CGameObject &Obj, BYTE invenrotyTargetPos, int bOption)
{
	// HERE GOES A MACRO
	return;
	CItemObject * pItem=NULL;
}

BOOL CItemSystemFor380::ChaosMix380ItemOption(CGameObject &Obj)
{
	if (this->m_bSystemFor380ItemOption != TRUE)
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0,284), Obj, 1);
		Obj.bIsChaosMixCompleted = false;

		return FALSE;
	}

	Obj.ChaosLock = TRUE;

	int iValidItemCount = 0;
	int iJewelOfHarmony = 0;
	int iJewelOfSuho = 0;
	int iInvalidItemCount = 0;
	int iMixPrice = 0;
	int iCharmOfLuckCount = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = 0;
	CItemObject * pTargetItem = NULL;
	int iPosOfJewelOfHarmony = -1;
	int iPosOfJewelOfSuho = -1;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (this->Is380Item(&Obj.pChaosBox[n]) == TRUE &&
				this->Is380OptionItem(&Obj.pChaosBox[n]) == FALSE &&
				Obj.pChaosBox[n].m_Level > 3 &&
				(Obj.pChaosBox[n].m_Option3 << 2) > 3)
			{
				iValidItemCount++;
				pTargetItem = &Obj.pChaosBox[n];
			}
			else if (g_kJewelOfHarmonySystem.IsJewelOfHarmonyPurity(Obj.pChaosBox[n].m_Type) == TRUE)
			{
				iJewelOfHarmony++;
				iPosOfJewelOfHarmony = n;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 31))
			{
				iJewelOfSuho++;
				iPosOfJewelOfSuho = n;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 53))
			{
				iCharmOfLuckCount += Obj.pChaosBox[n].m_Durability;
			}
			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if (iInvalidItemCount ||
		iValidItemCount != 1 ||
		iJewelOfHarmony != this->m_iNeedJewelOfHarmonyCount ||
		iJewelOfSuho != this->m_iNeedJewelOfSuhoCount ||
		pTargetItem == NULL ||
		iPosOfJewelOfHarmony == -1 ||
		iPosOfJewelOfSuho == -1 ||
		iCharmOfLuckCount > 10)
	{
		IOCP.DataSend(Obj.m_PlayerData->IDNumber, (LPBYTE)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return FALSE;
	}

	iMixPrice = this->m_iNeedZenFor380Option;
	int iChaosTaxMoney = iMixPrice * g_CastleSiegeSync.GetTaxRateChaos(Obj) / 100;

	if (iChaosTaxMoney < 0)
		iChaosTaxMoney = 0;

	iMixPrice += iChaosTaxMoney;

	if (iMixPrice < 0)
		iMixPrice = 0;

	if (Obj.m_PlayerData->Money < iMixPrice)
	{
		pMsg.Result = 2;
		IOCP.DataSend(Obj.m_PlayerData->IDNumber, (LPBYTE)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return FALSE;
	}

	Obj.m_PlayerData->Money -= iMixPrice;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);

	gGameProtocol.GCMoneySend(Obj, Obj.m_PlayerData->Money);
	g_MixSystem.LogChaosItem(Obj, "[380Item][Item Mix");
	sLog->outBasic("[380Item][Item Mix] - Mix Start");

	int iRate = rand() % 100;
	int iRateSuccess = this->m_iRateSuccessRateForMix1;

	if (pTargetItem->m_Level < 7)
		iRateSuccess = this->m_iRateSuccessRateForMix1;
	else if (pTargetItem->m_Level < 10)
		iRateSuccess = this->m_iRateSuccessRateForMix2;
	else if (pTargetItem->m_Level <= 13)
		iRateSuccess = this->m_iRateSuccessRateForMix3;
	else
		iRateSuccess = this->m_iRateSuccessRateForMix4;

	iRateSuccess += iCharmOfLuckCount;

	for (int i = 0; i < CHAOS_BOX_SIZE;i++)
	{
		if (&Obj.pChaosBox[i] != pTargetItem)
		{
			Obj.pChaosBox[i].Clear();
			Obj.pChaosBoxMap[i] = -1;
		}
	}

	if (iRate < iRateSuccess)
	{
		this->_SetOption(pTargetItem, TRUE);
		gGameProtocol.GCUserChaosBoxSend(Obj, 0);

		sLog->outBasic("[380Item][ItemMix] Mix Success [%s][%s], Money(%d-%d) Rate(%d/%d) Option(%d,%d) OptionValue(%d,%d)",
			Obj.AccountID, Obj.Name, Obj.m_PlayerData->Money,
			iMixPrice, iRate, iRateSuccess,
			this->m_itemOption[(pTargetItem->m_Type)].m_Option1,
			this->m_itemOption[(pTargetItem->m_Type)].m_Option2,
			this->m_itemOption[(pTargetItem->m_Type)].m_Value1,
			this->m_itemOption[(pTargetItem->m_Type)].m_Value2);
	}
	else
	{
		gGameProtocol.GCUserChaosBoxSend(Obj, 0);
		IOCP.DataSend(Obj.m_PlayerData->IDNumber, (LPBYTE)&pMsg, pMsg.h.size);

		sLog->outBasic("[380Item][ItemMix] Mix Fail [%s][%s], Money(%d-%d) Rate(%d/%d)",
			Obj.AccountID, Obj.Name, Obj.m_PlayerData->Money,
			iMixPrice, iRate, iRateSuccess);

	}

	Obj.ChaosLock = FALSE;
	return TRUE;
}
