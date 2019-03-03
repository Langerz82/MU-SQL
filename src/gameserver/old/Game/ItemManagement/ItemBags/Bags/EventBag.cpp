////////////////////////////////////////////////////////////////////////////////
// EventBag.cpp
#include "StdAfx.h"
#include "EventBag.h"
#include "Logging/Log.h"
#include "User/CUserData.h"
#include "LuaBag.h"
#include "MapClass.h"
#include "GameMain.h"

CEventBag::CEventBag()
{
}

CEventBag::~CEventBag()
{
}

void CEventBag::SetBagInfo(int EventID, int iParam2)
{
	this->EventID = EventID;
}

bool CEventBag::CheckCondition(CGameObject &Obj, int EventID, int iParam2)
{
	if (rand() % 10000 >= this->m_BagData.dwBagUseRate)
	{
		return false;
	}

	if (this->EventID != EventID)
	{
		return false;
	}

	return true;
}

bool CEventBag::IsBag(CGameObject &Obj, int EventID, int iParam2)
{
	if (this->EventID != EventID)
	{
		return false;
	}

	return true;
}

bool CEventBag::UseBag(CGameObject &Obj, int iMonsterIndex)
{
	
	CGameObject lpMonsterObj = getGameObject(iMonsterIndex);

	if (rand() % 10000 >= this->m_BagData.dwItemDropRate)
	{
		MapC[Obj.MapNumber].MoneyItemDrop(this->m_BagData.dwDropMoney, Obj.X, Obj.Y);

		return true;
	}

	if (rand() % 10000 < this->m_BagData.dwGainRuudRate)
	{
		int iRuudValue = this->GetValueMinMax(this->m_BagData.dwMinGainRuud, this->m_BagData.dwMaxGainRuud);
		Obj.m_PlayerData->Ruud += iRuudValue;

		GSProtocol.GCSendRuud(Obj.m_Index, Obj.m_PlayerData->Ruud, iRuudValue, true);
		return true;
	}

	BAG_ITEM m_Item;
	BAG_SECTION_ITEMS m_ItemSection;
	BAG_SECTION_DROP m_DropSection;

	int iResult = this->GetDropSection(Obj.m_Index, m_DropSection);

	if (iResult == FALSE)
	{
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
			MakeRewardSetItem(Obj.m_Index, lpMonsterObj.X, lpMonsterObj.Y, 1, Obj.MapNumber);
			return true;
		}

		if (this->GetItem(m_ItemSection, m_Item) == FALSE)
		{
			return false;
		}

		bool bResult = gLuaBag.DropCommonBag(Obj.m_Index, lpMonsterObj.MapNumber, lpMonsterObj.X, lpMonsterObj.Y, &m_Item);

		if (bResult == false)
		{
			return false;
		}

		return true;
	}

	for (int i = 0; i < m_ItemSection.btItemDropCount; i++)
	{		
		BYTE cDropX = lpMonsterObj.X;
		BYTE cDropY = lpMonsterObj.Y;

		if (!gObjGetRandomItemDropLocation(lpMonsterObj.MapNumber, cDropX, cDropY, 4, 4, 10))
		{
			cDropX = lpMonsterObj.X;
			cDropY = lpMonsterObj.Y;
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

		bool bResult = gLuaBag.DropMonsterBag(Obj.m_Index, iMonsterIndex, lpMonsterObj.MapNumber, cDropX, cDropY, &m_Item);

		if (bResult == false)
		{
			return false;
		}

	}
	return true;
}

bool CEventBag::UseBag_GremoryCase(CGameObject &Obj, int iMonsterIndex, BYTE btStorageType, BYTE btRewardSource, int iExpireDays)
{
	
	CGameObject lpMonsterObj = getGameObject(iMonsterIndex);

	if (rand() % 10000 >= this->m_BagData.dwItemDropRate)
	{
		MapC[Obj.MapNumber].MoneyItemDrop(this->m_BagData.dwDropMoney, Obj.X, Obj.Y);

		return true;
	}

	if (rand() % 10000 < this->m_BagData.dwGainRuudRate)
	{
		int iRuudValue = this->GetValueMinMax(this->m_BagData.dwMinGainRuud, this->m_BagData.dwMaxGainRuud);
		Obj.m_PlayerData->Ruud += iRuudValue;

		GSProtocol.GCSendRuud(Obj.m_Index, Obj.m_PlayerData->Ruud, iRuudValue, true);
		return true;
	}

	BAG_ITEM m_Item;
	BAG_SECTION_ITEMS m_ItemSection;
	BAG_SECTION_DROP m_DropSection;

	int iResult = this->GetDropSection(Obj.m_Index, m_DropSection);

	if (iResult == FALSE)
	{
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
			_stGremoryCaseItem stItem;
			MakeRewardSetItem(Obj, lpMonsterObj.X, lpMonsterObj.Y, 1, Obj.MapNumber, stItem);

			stItem.btStorageType = btStorageType;
			stItem.btRewardSource = btRewardSource;
			g_GremoryCase.GDReqAddItemToGremoryCase(Obj, stItem, iExpireDays);

			return true;
		}

		if (this->GetItem(m_ItemSection, m_Item) == FALSE)
		{
			return false;
		}

		_stGremoryCaseItem stItem;

		bool bResult = gLuaBag.MakeItemFromBagForGremoryCase(Obj, &m_Item, stItem);

		if (bResult == false)
		{
			return false;
		}

		stItem.btStorageType = btStorageType;
		stItem.btRewardSource = btRewardSource;
		g_GremoryCase.GDReqAddItemToGremoryCase(Obj, stItem, iExpireDays);

		return true;
	}

	for (int i = 0; i < m_ItemSection.btItemDropCount; i++)
	{		
		BYTE cDropX = lpMonsterObj.X;
		BYTE cDropY = lpMonsterObj.Y;

		if (!gObjGetRandomItemDropLocation(*getGameObject(lpMonsterObj.MapNumber), cDropX, cDropY, 4, 4, 10))
		{
			cDropX = lpMonsterObj.X;
			cDropY = lpMonsterObj.Y;
		}
	
		if (rand()%10000 < this->m_BagData.dwRandomSetItemDropRate)
		{
			_stGremoryCaseItem stItem;
			MakeRewardSetItem(Obj, cDropX, cDropY, 1, Obj.MapNumber, stItem);

			stItem.btStorageType = btStorageType;
			stItem.btRewardSource = btRewardSource;
			g_GremoryCase.GDReqAddItemToGremoryCase(Obj, stItem, iExpireDays);

			continue;
		}

		if (this->GetItem(m_ItemSection, m_Item) == FALSE)
		{
			return false;
		}

		_stGremoryCaseItem stItem;

		bool bResult = gLuaBag.MakeItemFromBagForGremoryCase(Obj.m_Index, &m_Item, stItem);

		if (bResult == false)
		{
			return false;
		}

		stItem.btStorageType = btStorageType;
		stItem.btRewardSource = btRewardSource;
		g_GremoryCase.GDReqAddItemToGremoryCase(Obj, stItem, iExpireDays);

	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

