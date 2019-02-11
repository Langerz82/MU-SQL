////////////////////////////////////////////////////////////////////////////////
// DoppelGangerEventInfo.h
#pragma once

structDOPPELGANGER_EVENT_INFO
{
	int nUserMaxLevel;
	float fMonsterLevel[5];
	float fMonsterHp[5];
	float fMonsterAttack[5];
	float fMonsterDefense[5];
	int nMiddleRewardItemDropCnt;
	int nMiddleItemBagIndex[3];
	int nMiddleItemBagDropRate[3];
	int nLastRewardItemDropCnt;
	int nLastItemBagIndex[6];
	int nLastItemBagDropRate[6];
};

class CDoppelGangerEventInfo
{
public:
	CDoppelGangerEventInfo(void);
	virtual ~CDoppelGangerEventInfo(void);
	void LoadDoppelgangerEventInfo(LPSTR lpFile);
	void ClearDoppelgangerEventInfo();
	DOPPELGANGER_EVENT_INFO * GetDoppelgangerEventInfo(int nUserMaxLevel);
	BOOL SetDoppelgangerEventInfo(int nUserCount, int nUserMaxLevel);

	float GetMonsterLevelWeight();
	float GetMonsterHpWeight();
	float GetMonsterAttackWeight();
	float GetMonsterDefenseWeight();
	int GetMiddleItemDropCnt();
	int GetMiddleItemBagIndex(int nRewardIndex);
	int GetMiddleItemDropRate(int nRewardIndex);
	int GetLastItemDropCnt();
	int GetLastItemBagIndex(int nRewardIndex);
	int GetLastItemDropRate(int nRewardIndex);

private:
	bool													m_bLoad;
	std::map<int,DOPPELGANGER_EVENT_INFO>				m_mapDoppelEvent_Info;
	DOPPELGANGER_EVENT_INFO								m_DoppelgangerInfo;
	int														m_nDoppelgangerUserCnt;

};



////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

