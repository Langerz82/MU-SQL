////////////////////////////////////////////////////////////////////////////////
// KeyGenerater.cpp
#include "stdafx.h"
#include "KeyGenerater.h"
#include "LargeRand.h"

CKeyGenerater g_KeyGenerater;

CKeyGenerater::CKeyGenerater()
{
}

CKeyGenerater::~CKeyGenerater()
{
}

DWORD CKeyGenerater::GenerateSeedValue()
{
	return GetLargeRand();
}

DWORD CKeyGenerater::GenerateKeyValue(DWORD dwKeyValue)
{
	return KEY_GENERATE_FILTER[dwKeyValue % 0xA][1] + KEY_GENERATE_FILTER[dwKeyValue % 0xA][0] * dwKeyValue - KEY_GENERATE_FILTER[dwKeyValue % 0xA][2] / KEY_GENERATE_FILTER[dwKeyValue % 0xA][3];
}

bool CKeyGenerater::CheckKeyValue(DWORD & dwOldKeyValue, DWORD dwReceiveKeyValue)
{
	DWORD dwGeneratedKeyValue = this->GenerateKeyValue(dwOldKeyValue);

	if (dwReceiveKeyValue == dwGeneratedKeyValue)
	{
		dwOldKeyValue = dwGeneratedKeyValue;
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

