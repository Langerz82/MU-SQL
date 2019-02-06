////////////////////////////////////////////////////////////////////////////////
// CUserStatisticsData.cpp
#include "StdAfx.h"
#include "CUserStatisticsData.h"

void CUserStatisticsData::IncrementMonsterKillCount(int MonsterType)
{
	InterlockedIncrement(reinterpret_cast<long *>(&this->MonsterKillCount));

	switch(MonsterType)
	{
		case 0 : InterlockedIncrement(reinterpret_cast<long *>(&this->GoldenMonsterKillCount)); break;
		case 1 : InterlockedIncrement(reinterpret_cast<long *>(&this->BossKillCount)); break;
	}
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

