////////////////////////////////////////////////////////////////////////////////
// MonsterBag.h
#pragma once
#include "Bag.h"
class CMonsterBag :
	public CBag
{
public:
	CMonsterBag();
	virtual ~CMonsterBag();

	bool IsBag(int aIndex, int MonsterClass, int iParam2);
	bool UseBag(int aIndex, int iMonsterIndex);
	bool CheckCondition(int aIndex, int MonsterClass, int iParam2);
	bool UseBag_GremoryCase(int aIndex, int iMonsterIndex, BYTE btStorageType, BYTE btRewardSource, int iExpireDays);
	void SetBagInfo(int MonsterClass, int iParam2);

private:
	int		m_BagMonsterClass;
};



////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

