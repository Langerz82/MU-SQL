////////////////////////////////////////////////////////////////////////////////
// GameSecurity.h
#ifndef GAMESECURITY_H
#define GAMESECURITY_H

struct ATTACK_SPEED
{
	int iPenaltyCount;
	DWORD64 LastAttackTime;
};

class CGameSecurity
{
public:
	CGameSecurity();
	~CGameSecurity();

	void DebugInfo(int aIndex);
	
	std::map<int,ATTACK_SPEED> m_ASData;
	CRITICAL_SECTION lpCritiASData;
}; extern CGameSecurity gGameSecurity;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

