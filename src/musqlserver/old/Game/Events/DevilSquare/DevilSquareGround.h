////////////////////////////////////////////////////////////////////////////////
// DevilSquareGround.h
#ifndef DEVILSQUAREGROUND_H
#define DEVILSQUAREGROUND_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "User/CUserData.h"


class CDevilSquareGround	// size 0x494
{

public:

	CDevilSquareGround();
	virtual ~CDevilSquareGround();

	void Init(int Index);
	void Clear();
	void SetBonus(int index, int zen, int exp);
	void ApplyBonusRate(float fRewardExpRate);
	void Set(WORD type, int starttime, int endtime);
	void SetBoss(WORD type, int starttime, int x,  int y,  int tx, int ty);
	void IncObjCount();
	long GetObjCount();
	void RegenBossMonster(int currtime);
	int FindMonsterType(WORD type);
	WORD GetMonsterType(int currtime);
	void SendRankingInfo(LPGameObject &lpObj);
	void ClearScore();
	void SortScore();
	void InsertObj(LPGameObject &lpObj);
	void SendScore();
	BOOL AddUser(int aIndex);
	BOOL DelUser(int aIndex);
	BOOL ChangerUserIndex(int OldIndex, int NewIndex);
	int GetPlayUserCountRightNow();
	void DropReward(int iIndex);
private:

	DevilSquareMonsterInfo m_DevilSquareMonsterInfo[MAX_ST_DEVILSQUARE_MONSTER];	// 4
	DevilSquareBossMonsterInfo m_DevilSquareBossMonsterInfo[MAX_ST_DEVILSQUARE_MONSTER];	// F4
	std::vector<LPGameObject> m_DevilSquareRankList;	// 2D4
	PMSG_DEVILSQUARERESULT m_DevilSquareScoreInfoTOP10;	// 2E4
	int m_iIndex;	// 3F4
	int m_Bonus[4][2];	// 3F8
	long m_dwObjCount;	// 418
	int m_BonusScoreTable[MAX_TYPE_PLAYER][MAX_DEVILSQUARE_GROUND];	// 41C
	CRITICAL_SECTION m_criti;
	int m_iUserIndex[10];
};


#endif

