////////////////////////////////////////////////////////////////////////////////
// CommonBag.cpp
#include "StdAfx.h"
#include "CommonBag.h"
#include "Logging/Log.h"
#include "User/CUserData.h"
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

bool CCommonBag::CheckCondition(CGameObject &Obj, int ItemID, int ItemLevel)
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

bool CCommonBag::IsBag(CGameObject &Obj, int ItemID, int ItemLevel)
{
	if (this->m_BagItemID == ItemID && this->m_BagItemLevel == ItemLevel)
	{
		return true;
	}

	return false;
}

bool CCommonBag::UseBag(CGameObject &Obj, int iParam2)
{
	

	if (rand() % 10000 >= this->m_BagData.dwItemDropRate)
	{
		MapC[Obj.MapNumber].MoneyItemDrop(this->m_BagData.dwDropMoney, Obj.X, Obj.Y);

		return true;
	}

	if (rand() % 10000 < this->m_BagData.dwGainRuudRate)
	{
		int iRuudValue = this->GetValueMinMax(this->m_BagData.dwMinGainRuud, this->m_BagData.dwMaxGainRuud);
		Obj.m_PlayerData->Ruud += iRuudValue;

		gGameProtocol.GCSendRuud(Obj.m_Index, Obj.m_PlayerData->Ruud, iRuudValue, true);
		return true;
	}

	BAG_ITEM m_Item;
	BAG_SECTION_ITEMS m_ItemSection;
	BAG_SECTION_DROP  m_DropSection;

	int iResult = this->GetDropSection(Obj.m_Index, m_DropSection);

	if (iResult == FALSE)
	{
		MsgOutput(Obj.m_Index, Lang.GetText(0,539));
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
			MakeRewardSetItem(Obj.m_Index, Obj.X, Obj.Y, 1, Obj.MapNumber);
			return true;
		}

		if (this->GetItem(m_ItemSection, m_Item) == FALSE)
		{
			return false;
		}
		bool bResult = gLuaBag.DropCommonBag(Obj.m_Index, Obj.MapNumber, Obj.X, Obj.Y, &m_Item);

		if (bResult == false)
		{
			return false;
		}

		return true;
	}

	for (int i = 0; i < m_ItemSection.btItemDropCount; i++)
	{
		BYTE cDropX = Obj.X;
		BYTE cDropY = Obj.Y;

		if (!gObjGetRandomItemDropLocation(Obj.MapNumber, cDropX, cDropY, 4, 4, 10))
		{
			cDropX = Obj.X;
			cDropY = Obj.Y;
		}

		if (rand()%10000 < this->m_BagData.dwRandomSetItemDropRate)
		{
			MakeRewardSetItem(Obj.m_Index, cDropX, cDropY, 1, Obj.MapNumber);
			continue;
		}
	
		if (this->GetItem(m_ItemSection, m_Item) == FALSE)
		{
			return false;
		}

		bool bResult = gLuaBag.DropCommonBag(Obj.m_Index, Obj.MapNumber, cDropX, cDropY, &m_Item);

		if (bResult == false)
		{
			return false;
		}

	}

	return true;
}

bool CCommonBag::UseBag_GremoryCase(CGameObject &Obj, int iMonsterIndex, BYTE btStorageType, BYTE btRewardSource, int iExpireDays)
{
	return false;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

