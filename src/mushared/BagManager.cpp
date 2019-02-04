////////////////////////////////////////////////////////////////////////////////
// BagManager.cpp
#include "stdafx.h"
#include "BagManager.h"
#include "TLog.h"
#include "configread.h"

CBagManager g_BagManager;

CBagManager::CBagManager()
{
	this->m_multimapBags.clear();
	InitializeCriticalSection(&this->m_criti);
}

CBagManager::~CBagManager()
{
	this->DeleteItemBags();
	DeleteCriticalSection(&this->m_criti);
}

void CBagManager::InitBagManager()
{
	this->DeleteItemBags();
	gLuaBag.Init();
	gLuaBag.LoadItemBag();
}

bool CBagManager::AddItemBag(int BagType, int iParam1, int iParam2, const char* lpFileName)
{
	CBag * Bag = nullptr;
	switch (BagType)
	{
		case BAG_COMMON:
			Bag = new CCommonBag;

			if (Bag == nullptr)
			{
				g_Log.AddC(TColor::Red, "Memory allocation failed!");
				return false;
			}

			break;

		case BAG_MONSTER:
			Bag = new CMonsterBag;

			if (Bag == nullptr)
			{
				g_Log.AddC(TColor::Red, "Memory allocation failed!");
				return false;
			}

			break;

		case BAG_EVENT:
			Bag = new CEventBag;

			if (Bag == nullptr)
			{
				g_Log.AddC(TColor::Red, "Memory allocation failed!");
				return false;
			}

			break;
	}

	Bag->SetBagInfo(iParam1, iParam2);

	char szPath[MAX_PATH];
	sprintf(szPath, "\\ItemBags\\%s.xml", lpFileName);
	if (Bag->LoadBag(g_ConfigRead.GetPath(szPath)) == false)
	{
		delete Bag;
		Bag = nullptr;
		return false;
	}

	EnterCriticalSection(&this->m_criti);
	this->m_multimapBags.insert(std::pair<int, CBag *>(BagType, Bag));
	LeaveCriticalSection(&this->m_criti);

	return true;
}

void CBagManager::DeleteItemBags()
{
	EnterCriticalSection(&this->m_criti);

	for (std::multimap<int, CBag *>::iterator It = this->m_multimapBags.begin(); It != this->m_multimapBags.end(); It++)
	{
		delete It->second;
		
		It->second = nullptr;
	}

	this->m_multimapBags.clear();

	LeaveCriticalSection(&this->m_criti);
}

bool CBagManager::IsBag(int aIndex, int BagType, int iParam1, int iParam2)
{
	EnterCriticalSection(&this->m_criti);

	std::pair <std::multimap<int, CBag *>::iterator, std::multimap<int, CBag *>::iterator> ret = this->m_multimapBags.equal_range(BagType);

	for (std::multimap<int, CBag *>::iterator it = ret.first; it != ret.second; it++)
	{
		if (it->second->IsBag(aIndex, iParam1, iParam2) == false)
		{
			LeaveCriticalSection(&this->m_criti);
			return true;
		}
	}
	
	LeaveCriticalSection(&this->m_criti);
	return false;
}

bool CBagManager::SearchAndUseBag(int aIndex, int BagType, int iParam1, int iParam2)
{
	EnterCriticalSection(&this->m_criti);

	std::pair <std::multimap<int, CBag *>::iterator, std::multimap<int, CBag *>::iterator> ret = this->m_multimapBags.equal_range(BagType);

	for (std::multimap<int, CBag *>::iterator it = ret.first; it != ret.second; it++)
	{
		if (it->second->CheckCondition(aIndex, iParam1, iParam2) == false)
		{
			continue;
		}

		if (it->second->UseBag(aIndex, iParam2) == false)
		{
			LeaveCriticalSection(&this->m_criti);
			return false;
		}

		else
		{
			if (it->second->IsBagUseEffect() >= 0)
			{
				it->second->MakeBagEffectUse(aIndex, aIndex);
			}

			it->second->AddCoin(aIndex);
			it->second->DropSummonItem(aIndex);
			LeaveCriticalSection(&this->m_criti);
			return true;
		}
	}

	LeaveCriticalSection(&this->m_criti);
	return false;
}

int CBagManager::GetItemFromBag(int aIndex, int BagType, int iParam1, int iParam2, CItem & Item, time_t & DurationItem) // 0 - fail, 1 - item, 2 - zen, 3 - ruud
{
	EnterCriticalSection(&this->m_criti);

	std::pair <std::multimap<int, CBag *>::iterator, std::multimap<int, CBag *>::iterator> ret = this->m_multimapBags.equal_range(BagType);

	for (std::multimap<int, CBag *>::iterator it = ret.first; it != ret.second; it++)
	{
		if (it->second->CheckCondition(aIndex, iParam1, iParam2) == false)
		{
			continue;
		}

		int iResult = it->second->GetReadyItemToUse(aIndex, Item, DurationItem);

		if (iResult == 0)
		{
			LeaveCriticalSection(&this->m_criti);
			return 0;
		}

		else
		{
			if (it->second->IsBagUseEffect() >= 0)
			{
				it->second->MakeBagEffectUse(aIndex, aIndex);
			}

			it->second->AddCoin(aIndex);
			it->second->DropSummonItem(aIndex);
			LeaveCriticalSection(&this->m_criti);
			return iResult;
		}
	}

	LeaveCriticalSection(&this->m_criti);
	return 0;
}

bool CBagManager::UseBag_GremoryCase(int aIndex, int BagType, int iParam1, int iParam2, BYTE btStorageType, BYTE btRewardSource, int iExpireDays)
{
	EnterCriticalSection(&this->m_criti);

	std::pair <std::multimap<int, CBag *>::iterator, std::multimap<int, CBag *>::iterator> ret = this->m_multimapBags.equal_range(BagType);

	for (std::multimap<int, CBag *>::iterator it = ret.first; it != ret.second; it++)
	{
		if (it->second->CheckCondition(aIndex, iParam1, iParam2) == false)
		{
			continue;
		}

		if (it->second->UseBag_GremoryCase(aIndex, iParam2, btStorageType, btRewardSource, iExpireDays) == false)
		{
			LeaveCriticalSection(&this->m_criti);
			return false;
		}

		else
		{
			if (it->second->IsBagUseEffect() >= 0)
			{
				it->second->MakeBagEffectUse(aIndex, aIndex);
			}

			it->second->AddCoin(aIndex);

			LeaveCriticalSection(&this->m_criti);
			return true;
		}
	}

	LeaveCriticalSection(&this->m_criti);
	return false;
}

void CBagManager::AddCashCoin(int aIndex, int BagType, int iParam1, int iParam2)
{
	EnterCriticalSection(&this->m_criti);

	std::pair <std::multimap<int, CBag *>::iterator, std::multimap<int, CBag *>::iterator> ret = this->m_multimapBags.equal_range(BagType);

	for (std::multimap<int, CBag *>::iterator it = ret.first; it != ret.second; it++)
	{
		if (it->second->CheckCondition(aIndex, iParam1, iParam2) == false)
		{
			continue;
		}

		it->second->AddCoin(aIndex);
	}

	LeaveCriticalSection(&this->m_criti);
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

