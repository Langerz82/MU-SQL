////////////////////////////////////////////////////////////////////////////////
// GameSecurity.cpp
#include "StdAfx.h"
#include "GameSecurity.h"
#include "Logging/Log.h"
#include "User/CUserData.h"
CGameSecurity gGameSecurity;

CGameSecurity::CGameSecurity()
{
	this->m_ASData.clear();
	InitializeCriticalSection(&lpCritiASData);
}

CGameSecurity::~CGameSecurity()
{

}

void CGameSecurity::DebugInfo(CGameObject &Obj)
{
	ATTACK_SPEED sAttackSpeed;
	EnterCriticalSection(&lpCritiASData);
	std::map<int,ATTACK_SPEED>::iterator iter = this->m_ASData.find(Obj.m_Index);
	
	if(iter == m_ASData.end())
	{
		sAttackSpeed.iPenaltyCount = 0;
		sAttackSpeed.LastAttackTime = GetTickCount64();
		this->m_ASData.insert(std::pair<int,ATTACK_SPEED>(Obj.m_Index, sAttackSpeed));
		LeaveCriticalSection(&lpCritiASData);
		return;
	}
	else
	{
		sAttackSpeed = iter->second;

		DWORD64 dwTimeNow = GetTickCount64();

		double realAttackSpeed = (double)((double)dwTimeNow - (double)sAttackSpeed.LastAttackTime) / 1000.00;

		sLog->outError("[Debug ANTI-HACK][%d][%s][%s] Attack Speed: %d Time Between last attack = %f",Obj.m_Index, Obj.m_PlayerData->ConnectUser->AccountID, Obj.Name, Obj.m_AttackSpeed, realAttackSpeed);

		iter->second.LastAttackTime = dwTimeNow;

	}

	LeaveCriticalSection(&lpCritiASData);

}



////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

