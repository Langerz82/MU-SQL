////////////////////////////////////////////////////////////////////////////////
// DevilSquareGround.h
#ifndef DEVILSQUAREGROUND_H
#define DEVILSQUAREGROUND_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "User/CUserData.h"

struct PMSG_ANS_EVENTUSERSCORE
{
	PBMSG_HEAD2 h;	// C1:01
	char AccountID[10];	// 3
	char GameID[10];	// D
	int ServerCode;	// 18
	int Score;	// 1C
	int Class;	// 20
	int SquareNum;	// 24
};


struct DevilSquareMonsterInfo
{
	WORD m_Type;	// 0
	int m_RegenStartTime;	// 4
	int m_RegenEndTime;	// 8
	int m_iDevilSquareNumber;	// C
};

struct DevilSquareBossMonsterInfo
{
	WORD m_Type;	// 0
	int m_RegenStartTime;	// 4
	int m_RegenEndTime;	// 8
	int X;	// C
	int Y;	// 10
	int TX;	// 14
	int TY;	// 18
	int m_iDevilSquareNumber;	// 1C
};


struct DevilSquareScoreInfo
{
	char Name[10];	// 0
	int TotalScore;	// C
	int BonusExp;	// 10
	int BonusZen;	// 14
};

#pragma pack(1)

struct PMSG_DEVILSQUARERESULT
{
	PBMSG_HEAD h;	// C1:93
	BYTE MyRank;	// 3
	BYTE Count;	// 4
	DevilSquareScoreInfo Score[11];	// 5
};

#pragma pack()

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
	void SendRankingInfo(CGameObject &Obj);
	void ClearScore();
	void SortScore();
	void InsertObj(CGameObject &Obj);
	void SendScore();
	BOOL AddUser(CGameObject &Obj);
	BOOL DelUser(CGameObject &Obj);
	BOOL ChangerUserIndex(int OldIndex, int NewIndex);
	int GetPlayUserCountRightNow();
	void DropReward(CGameObject &Obj);
private:

	DevilSquareMonsterInfo m_DevilSquareMonsterInfo[MAX_ST_DEVILSQUARE_MONSTER];	// 4
	DevilSquareBossMonsterInfo m_DevilSquareBossMonsterInfo[MAX_ST_DEVILSQUARE_MONSTER];	// F4
	std::vector<CGameObject*> m_DevilSquareRankList;	// 2D4
	PMSG_DEVILSQUARERESULT m_DevilSquareScoreInfoTOP10;	// 2E4
	int m_iIndex;	// 3F4
	int m_Bonus[4][2];	// 3F8
	long m_dwObjCount;	// 418
	int m_BonusScoreTable[MAX_TYPE_PLAYER][MAX_DEVILSQUARE_GROUND];	// 41C
	CRITICAL_SECTION m_criti;
	int m_iUserIndex[10];
};


#endif

