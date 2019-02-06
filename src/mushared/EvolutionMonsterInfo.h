////////////////////////////////////////////////////////////////////////////////
// EvolutionMonsterInfo.h
#ifndef EVOMON_INFO_H
#define EVOMON_INFO_H

enum EVOMON_USER_STATE
{
	EVOMON_NOT_ACTIVE = -1,
	EVOMON_ACTIVE = 0,
	EVOMON_KILLED = 1,
	EVOMON_AWAKENING = 2,
	EVOMON_AWAKE_FAILED = 3
};

#pragma once
class CEvolutionMonsterInfo
{
public:
	CEvolutionMonsterInfo();
	virtual ~CEvolutionMonsterInfo();

	void Init();
	void Clear();

	bool IsEvoMonActive() { return this->m_State != EVOMON_NOT_ACTIVE; }

	int GetState() { return this->m_State; }
	int GetEvoMonIndex() { return this->m_EvoMonIndex; }
	int GetEvoMonLevel() { return this->m_EvoMonLevel; }
	int GetScore() { return this->m_Score; }
	int GetMaxScore() { return this->m_nMaxScore; }
	int GetTotalDamage() { return this->m_nTotalDamage; }
	int GetFieldIndex() { return this->m_FieldIndex; }
	void SetState(int State) { this->m_State = State; }
	void SetEvoMonIndex(int Index) { this->m_EvoMonIndex = Index; }
	void SetEvoMonLevel(int Level) { this->m_EvoMonLevel = Level; }
	void SetScore(int nScore) { this->m_Score = nScore; }
	void SetMaxScore(int nMaxScore) { this->m_nMaxScore = nMaxScore; }
	void SetTotalDamage(int nDamage) { this->m_nTotalDamage = nDamage; }
	void SetFieldIndex(int Index) { this->m_FieldIndex = Index; }

private:
	int m_State;
	int m_Score;
	int m_nMaxScore;
	int m_nTotalDamage;
	int m_EvoMonLevel;
	int m_FieldIndex;
	int m_EvoMonIndex;
};

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

