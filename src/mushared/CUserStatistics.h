////////////////////////////////////////////////////////////////////////////////
// CUserStatistics.h
#pragma once
#include "CUserStatisticsData.h"

class CUserStatistics
{
private:
	char Name[11];
	int aIndex;
public:
	CUserStatistics(char Name[11], int aIndex);
	virtual ~CUserStatistics();

	bool LoadData(CUserStatisticsData *LoadUserData);
	void SaveData();

	CUserStatisticsData *UserData;
};

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

