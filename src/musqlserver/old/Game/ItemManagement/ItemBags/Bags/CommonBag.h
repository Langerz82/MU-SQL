////////////////////////////////////////////////////////////////////////////////
// CommonBag.h
#pragma once
#include "Bag.h"

class CCommonBag : public CBag
{
public:
	CCommonBag();
	virtual ~CCommonBag();

	bool IsBag(LPGameObject &lpObj, int ItemID, int ItemLevel);
	bool UseBag(LPGameObject &lpObj, int iParam2);
	bool CheckCondition(LPGameObject &lpObj, int ItemID, int ItemLevel);
	bool UseBag_GremoryCase(LPGameObject &lpObj, int iMonsterIndex, BYTE btStorageType, BYTE btRewardSource, int iExpireDays);
	void SetBagInfo(int ItemID, int ItemLevel);

private:
	int		m_BagItemID;
	int		m_BagItemLevel;
};



////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

