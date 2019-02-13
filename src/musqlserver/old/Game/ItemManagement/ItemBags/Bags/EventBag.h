////////////////////////////////////////////////////////////////////////////////
// EventBag.h
#pragma once
#include "Bag.h"

class CEventBag : public CBag
{
public:
	CEventBag();
	virtual ~CEventBag();

	bool IsBag(CGameObject &Obj, int EventID, int iParam2);
	bool UseBag(CGameObject &Obj, int iMonsterIndex);
	bool CheckCondition(CGameObject &Obj, int w, int h);
	bool UseBag_GremoryCase(CGameObject &Obj, int iMonsterIndex, BYTE btStorageType, BYTE btRewardSource, int iExpireDays);
	void SetBagInfo(int EventID, int iParam2);

private:
	int EventID;
};



////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

