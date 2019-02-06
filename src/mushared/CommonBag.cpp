////////////////////////////////////////////////////////////////////////////////
// CommonBag.cpp
#include "stdafx.h"
#include "CommonBag.h"
#include "TLog.h"
#include "user.h"
#include "LuaBag.h"
#include "MapClass.h"
#include "GameMain.h"

CCommonBag::CCommonBag()
{
}

CCommonBag::~CCommonBag()
{
}

void CCommonBag::SetBagInfo(int ItemID, int ItemLevel)
{
	this->m_BagItemID = ItemID;
	this->m_BagItemLevel = ItemLevel;
}

bool CCommonBag::CheckCondition(int aIndex, int ItemID, int ItemLevel)
{
	if (rand() % 10000 >= this->m_BagData.dwBagUseRate)
	{
		return false;
	}

	if (this->m_BagItemID == ItemID && this->m_BagItemLevel == ItemLevel)
	{
		return true;
	}

	return false;
}

bool CCommonBag::IsBag(int aIndex, int ItemID, int ItemLevel)
{
	if (this->m_BagItemID == ItemID && this->m_BagItemLevel == ItemLevel)
	{
		return true;
	}

	return false;
}

bool CCommonBag::UseBag(int aIndex, int iParam2)
{
	LPOBJ lpObj = &gObj[aIndex];

	if (rand() % 10000 >= this->m_BagData.dwItemDropRate)
	{
		MapC[gObj[aIndex].MapNumber].MoneyItemDrop(this->m_BagData.dwDropMoney, lpObj->X, lpObj->Y);

		return true;
	}

	if (rand() % 10000 < this->m_BagData.dwGainRuudRate)
	{
		int iRuudValue = this->GetValueMinMax(this->m_BagData.dwMinGainRuud, this->m_BagData.dwMaxGainRuud);
		lpObj->m_PlayerData->Ruud += iRuudValue;

		GSProtocol.GCSendRuud(aIndex, lpObj->m_PlayerData->Ruud, iRuudValue, true);
		return true;
	}

	BAG_ITEM m_Item;
	BAG_SECTION_ITEMS m_ItemSection;
	BAG_SECTION_DROP  m_DropSection;

	int iResult = this->GetDropSection(aIndex, m_DropSection);

	if (iResult == FALSE)
	{
		MsgOutput(aIndex, Lang.GetText(0,539));
		return false;
	}

	iResult = this->GetItemsSection(m_DropSection, m_ItemSection);

	if (iResult == FALSE)
	{
		return false;
	}

	if (m_ItemSection.btItemDropCount <= 0)
	{
		return false;
	}

	if (m_ItemSection.btItemDropCount == 1)
	{
		if (rand()%10000 < this->m_BagData.dwRandomSetItemDropRate)
		{
			MakeRewardSetItem(aIndex, lpObj->X, lpObj->Y, 1, lpObj->MapNumber);
			return true;
		}

		if (this->GetItem(m_ItemSection, m_Item) == FALSE)
		{
			return false;
		}
		bool bResult = gLuaBag.DropCommonBag(aIndex, lpObj->MapNumber, lpObj->X, lpObj->Y, &m_Item);

		if (bResult == false)
		{
			return false;
		}

		return true;
	}

	for (int i = 0; i < m_ItemSection.btItemDropCount; i++)
	{
		BYTE cDropX = lpObj->X;
		BYTE cDropY = lpObj->Y;

		if (!gObjGetRandomItemDropLocation(lpObj->MapNumber, cDropX, cDropY, 4, 4, 10))
		{
			cDropX = lpObj->X;
			cDropY = lpObj->Y;
		}

		if (rand()%10000 < this->m_BagData.dwRandomSetItemDropRate)
		{
			MakeRewardSetItem(aIndex, cDropX, cDropY, 1, lpObj->MapNumber);
			continue;
		}
	
		if (this->GetItem(m_ItemSection, m_Item) == FALSE)
		{
			return false;
		}

		bool bResult = gLuaBag.DropCommonBag(aIndex, lpObj->MapNumber, cDropX, cDropY, &m_Item);

		if (bResult == false)
		{
			return false;
		}

	}

	return true;
}

bool CCommonBag::UseBag_GremoryCase(int aIndex, int iMonsterIndex, BYTE btStorageType, BYTE btRewardSource, int iExpireDays)
{
	return false;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

