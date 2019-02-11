////////////////////////////////////////////////////////////////////////////////
// EvolutionMonsterMng.h
#ifndef EVOMON_MNG_H
#define EVOMON_MNG_H

#include "User/CUserData.h"

#define EVOMON_MONSTER_INDEX 681
#define EVOMON_SPECIAL_MONSTER_INDEX 690
#define EVOMON_FIELD_SUCCESS 704
#define EVOMON_FIELD_FAILED 705

struct EVOMON_MONSTER_DATA
{
	WORD m_wLevel;
	float m_fHp;
	float m_fDamage;
	float m_fDefense;
	float m_fAtkRate;
	float m_fDefRate;
	int m_iSuccessRate;
};

struct EVOMON_REWARD_RANGE
{
	WORD m_wMinLevel;
	WORD m_wMaxLevel;
	BYTE m_btRewardType;
};

struct EVOMON_SPECIAL_MONSTER
{
	WORD m_wMinLevel;
	WORD m_wMaxLevel;
	int m_iSuccessRate;
};

// Protocol
struct PMSG_EVOMON_SUMMON_RESULT
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btMapNumber;
	BYTE btX;
	BYTE btY;
	int iTimeLeft;
};

struct PMSG_EVOMON_SCORE
{
	PBMSG_HEAD2 h; // C1:3E:11
	WORD wScore;
};
#pragma once
class CEvolutionMonsterMng
{
public:
	CEvolutionMonsterMng(void);
	virtual ~CEvolutionMonsterMng(void);

	void Init();
	void LoadFile(char *szFile);

	bool UseSummonScroll(CGameObject &lpObj);

	int SummonEvoMon(CGameObject &lpObj);
	int SetField(int iEvoMonIndex, int nFieldIndex, int nOnwerIndex, int iFieldClass);

	void EvolutionMonsterDie(CGameObject & lpTargetObj, CGameObject &lpObj);
	void FieldProcess(CGameObject & lpFieldIndex);
	void EndTimeProcess(CGameObject & lpMonsterObj);

	void EvoluteMonster(CGameObject &lpObj);
	void SetMonsterStats(CGameObject & lpMonsterObj, EVOMON_MONSTER_DATA m_MonsterData);

	void EndEvoMon(CGameObject &lpObj);
	void UserQuit(CGameObject &lpObj);

	void GCSendUserScore(CGameObject &lpObj, int nScore);
	void GCSendEvoMonNotice(CGameObject &lpObj);

	void GiveReward(CGameObject &lpObj, bool bSpecialEvoMon);
	bool IsEventMap(int iMapNumber, int iX, int iY);
	int GetRewardItemType(BYTE btType);

	bool IsEvolutedEvoMon(int iMonsterIndex);
	int GetOwnerIndex(int iMonsterIndex);

	void GDReqEvoMonMaxScore(CGameObject &lpObj);
	void DGAnsEvoMonMaxScore(LPBYTE lpRecv);

	void GDReqSaveEvoMonScore(CGameObject &lpObj, int nScore, int nTotalDamage);

	bool IsEvoMonEnable() { return this->m_bEnable; }
	int GetEvoMonTimeLimit() { return this->m_iEvoMonTimeLimit; }

	int GetSummonScrollDropRate() { return this->m_iSummonScrollDropRate; }
	int GetSummonScrollDropLevel() { return this->m_iSummonScrollDropLevel; }

private:
	bool m_bLoadFile;

	bool m_bEnable;
	int m_iEvoMonTimeLimit;

	int m_iSummonScrollDropRate;
	int m_iSummonScrollDropLevel;

	BYTE m_btSpecialEvoMonRewardType;

	std::map<int, EVOMON_MONSTER_DATA> m_mapEvoMonData;
	std::vector<EVOMON_REWARD_RANGE> m_vtRewardRange;
	std::vector<EVOMON_SPECIAL_MONSTER> m_vtSpecialEvoMon;
};

extern CEvolutionMonsterMng g_EvoMonMng;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

