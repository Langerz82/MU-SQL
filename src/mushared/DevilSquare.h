////////////////////////////////////////////////////////////////////////////////
// DevilSquare.h
// ------------------------------
// Decompiled by Deathway
// Date : 2007-05-09
// ------------------------------
#ifndef DEVILSQUARE_H
#define DEVILSQUARE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DevilSquareGround.h"
#include "user.h"

// Original

enum eDevilSquareState
{
	DevilSquare_CLOSE = 0x0,
	DevilSquare_OPEN = 0x1,
	DevilSquare_PLAYING = 0x2,
	DevilSquare_NONE = 0x3
};

#define MAX_DEVILSQUARE_USER	10

#define DS_MAP_RANGE(map) ( ( ((map)) == MAP_INDEX_DEVILSQUARE  )?TRUE:( ((map)) == MAP_INDEX_DEVILSQUARE2  )?TRUE:FALSE  )
#define DS_LEVEL_RANGE(level) ( ( ((level)) < 0  )?FALSE:( ((level)) > MAX_DEVILSQUARE_GROUND-1  )?FALSE:TRUE)

struct DEVILSQUARE_TIME
{
	int m_iHOUR;
	int m_iMINUTE;
};

struct DEVILSQUARE_MONSTER_POSITION
{
	WORD m_Type;
	BYTE m_MapNumber;
	BYTE m_Dis;
	BYTE m_X;
	BYTE m_Y;
	BYTE m_W;
	BYTE m_H;
	BYTE m_Dir;
	BYTE m_Count;
};

class CDevilSquare
{

public:

	CDevilSquare();
	virtual ~CDevilSquare();

	void Init();
	void SetState(enum eDevilSquareState eState);
	enum eDevilSquareState GetState() { return this->m_eState; };	// line : 93
	void Run();
	int GetRemainTime() { return this->m_iRemainTime; }
	void SetMonster();
	void ClearMonstr();
	void gDevilSquareMonsterRegen(LPOBJ lpObj);
	void DieProcDevilSquare(LPOBJ lpObj);
	UINT64 gObjMonsterExpSingle(LPOBJ lpObj, LPOBJ lpTargetObj, int dmg, int tot_dmg);
	void gObjExpParty(LPOBJ lpObj, LPOBJ lpTargetObj, int AttackDamage, int MSBFlag);
	void gObjMonsterScoreDivision(LPOBJ lpMonObj, LPOBJ lpObj, int AttackDamage, int MSBFlag);
	void Load(LPSTR filename);
	void LoadMonster(LPSTR filename);
	void CalcScore();
	void SetClose();
	int GetDevilSquareIndex(int iGateNumber);
	int GetUserLevelToEnter(int iUserIndex, WORD& btMoveGate);
	BOOL AddUser(BYTE cSquareNumber, int iUserIndex);
	BOOL DelUser(BYTE cSquareNumber, int iUserIndex);
	BOOL ChangeUserIndex(int iOldIndex, int iNewIndex, BYTE cSquareNumber);
	int GetPlayUserCountRightNow(BYTE cSquareNumber);
	BOOL LeaveDevilSquare(BYTE cSquareNumber, int iUserIndex);
	void IncObjCount(BYTE cSquareNumber);
	int GetObjCount(BYTE cSquareNumber);

	bool IsEventEnable() { return this->m_bEventEnable; }
	void SetEventEnable(bool bEnable) { this->m_bEventEnable = bEnable; }
	int GetMaxUserInSquare() { return this->m_nMaxUserInSquare; }
	void SetMaxUserInSquare(int nVal) { this->m_nMaxUserInSquare = nVal; }
	int GetEyeDropRate() { return this->m_nEyeDropRate; }
	void SetEyeDropRate(int nVal) { this->m_nEyeDropRate = nVal; }
	int GetKeyDropRate() { return this->m_nKeyDropRate; }
	void SetKeyDropRate(int nVal) { this->m_nKeyDropRate = nVal; }
	int GetCloseTime() { return this->m_iCloseTime; }
	int GetOpenTime() { return this->m_iOpenTime; }
	int GetPlayTime() { return this->m_iPlaytime; }
	void SetMaxEnterCount(int nVal) { this->m_nMaxEnterCount = nVal; }
	int GetMaxEnterCount() { return this->m_nMaxEnterCount; }

	DEVILSQUARE_MONSTER_POSITION * GetMonsterPosData(int iPosNum, int iSquareNumber);

	void SetCheatOpenTime(int nMin)
	{
		this->m_iRemainTime = nMin;
		this->m_iremainTimeSec = nMin * 60;
	}

	BOOL GetPosition(int TableNum, int iEventLevel, short MapNumber, short & x, short & y);

private:

	void SetOpen();
	void SetPlaying();
	void SetNone();
	void ProcClose();
	void ProcOpen();
	void ProcPlaying();
	void ProcNone();
	void SendEventStartMsg();
	void ItemClear();
	void CheckSync();
	void gObjScoreClear();
	void CheckInvalidUser();

private:

	enum eDevilSquareState m_eState;	// 4
	ULONGLONG m_iTime;	// 8
	LONGLONG m_iRemainTime;	// C
	LONGLONG m_iremainTimeSec;	// 10
	BOOL m_bSendTimeCount;	// 14
	BOOL m_bQuit;	// 18
	BOOL m_bFlag;	// 1C
	int m_iCloseTime;	// 24
	int m_iOpenTime;	// 28
	int m_iPlaytime;	// 2C
	CDevilSquareGround m_DevilSquareGround[MAX_DEVILSQUARE_GROUND];	// 30
	int m_BonusScoreTable[MAX_TYPE_PLAYER][MAX_DEVILSQUARE_GROUND];	// 1BA8
	std::vector<DEVILSQUARE_TIME> m_vtDevilSquareTime;
	bool m_bEventEnable;
	int m_nMaxUserInSquare;
	int m_nEyeDropRate;
	int m_nKeyDropRate;
	int m_nMaxEnterCount;
	DEVILSQUARE_MONSTER_POSITION m_DSMP[MAX_DEVILSQUARE_GROUND][10];
	int m_DSMP_CNT[MAX_DEVILSQUARE_GROUND];
};

extern CDevilSquare g_DevilSquare;

struct EVENT_LEVEL_LIMITATION_EX
{
	int NormalCharacterMinLevel; // Dark Knight, Dark Wizard, Elf
	int NormalCharacterMaxLevel; // Dark Knight, Dark Wizard, Elf
	int MoveGate;	// Dest Gate
};

static EVENT_LEVEL_LIMITATION_EX g_sttDEVILSQUARE_LEVEL[MAX_DEVILSQUARE_GROUND] =
{
	// Devil Square 1
	50, 179, // DK, DW, Elf
	58,	// Gate

		// Devil Square 2
		180, 269, // DK, DW, Elf
		59,	// Gate

			// Devil Square 3
			270, 349, // DK, DW, Elf
			60,	// Gate

				// Devil Square 4
				350, 399, // DK, DW, Elf
				61,	// Gate

					// Devil Square 5
					400, 500, // DK, DW, Elf
					111,	// Gate

							// Devil Square 6
							501, 650, // DK, DW, Elf
							112,	// Gate

									// Devil Square 7
									651,	820,
									270
};

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

