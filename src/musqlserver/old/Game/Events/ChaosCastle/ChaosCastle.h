////////////////////////////////////////////////////////////////////////////////
// ChaosCastle.h
#ifndef CHAOSCASTLE_H
#define CHAOSCASTLE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BloodCastle.h"
#include "User/CUserData.h"
#include "GMMng.h"

#define MAX_CHAOSCASTLE_MONSTER	100
#define MAX_CHAOSCASTLE_USER	70
#define MAX_CHAOSCASTLE_LEVEL	7
#define MAX_CC_TRAP_STEP	4
#define MAX_CC_HOLLOW_ZONE	4
#define MAX_CC_MONSTERS	100
#define MAX_CC_MONSTER_DROP_ITEM	2
#define MIN_CC_USER_NEED_PLAY	1	// default is 2

#define CHAOS_CASTLE_ZEN_REWARD	0	// default is 0

#define CC_FLOOR_RANGE(floor) ( ((floor) < 0)?FALSE:((floor) >MAX_CHAOSCASTLE_LEVEL-1 )?FALSE:TRUE )
#define CC_SUB_FLOOR_RANGE(floor) ( ((floor) < 0)?FALSE:((floor) >MAX_CHAOSCASTLE_USER-1 )?FALSE:TRUE )
#define CC_TRAP_STEP_RANGE(floor) ( ((floor) < 0)?FALSE:((floor) >MAX_CC_TRAP_STEP-1 )?FALSE:TRUE )
#define CC_MAP_RANGE(map) (((map) < MAP_INDEX_CHAOSCASTLE1)?0:((map) > MAP_INDEX_CHAOSCASTLE6)?(((map) == MAP_INDEX_CHAOSCASTLE7)?1:0):1)

#define CC_STATE_NONE		0
#define CC_STATE_CLOSED		1
#define CC_STATE_PLAYING	2
#define CC_STATE_PLAYEND	3


static struct _CHAOSCASTLE_LEVEL
{
	int iLOWER_BOUND;	// 0
	int iUPPER_BOUND;	// 4

} g_sttCHAOSCASTLE_LEVEL[MAX_CHAOSCASTLE_LEVEL] =
{
	// Chaos Castle 1
	50, 179, // DK, DW, Elf

	// Chaos Castle 2
	180, 269, // DK, DW, Elf

	// Chaos Castle 3
	270, 349, // DK, DW, Elf

	// Chaos Castle 4
	350, 399, // DK, DW, Elf

	// Chaos Castle 5
	400, 500, // DK, DW, Elf

	// Chaos Castle 6
	501, 650, // DK, DW, Elf

	// Chaos Castle 7
	651, 820, // DK, DW, Elf
};

static int g_iChaosCastle_BlowOutDistance[4][2]={3,4,3,4,2,3,0,1};
static int g_iChaosCastle_BlowOutDamage[4]={15, 15, 10, 5};
static int g_iChaosCastle_EnterCost[MAX_CHAOSCASTLE_LEVEL]={25000, 80000, 150000, 250000, 400000, 650000, 850000};
static int g_iChaosCastle_GuardSetValue[MAX_CHAOSCASTLE_LEVEL]={ 2500, 2500, 2500, 2500, 2500, 2500, 2500};
static int g_iChaosCastle_MonsterItems[MAX_CHAOSCASTLE_LEVEL][MAX_CC_MONSTER_DROP_ITEM][2] =
{
	ITEMGET(14,13), 1, ITEMGET(14,14), 2,	// Chaos Castle 1
	ITEMGET(14,13), 1, ITEMGET(14,14), 3,	// Chaos Castle 2
	ITEMGET(14,13), 1, ITEMGET(14,14), 3,	// Chaos Castle 3
	ITEMGET(14,13), 2, ITEMGET(14,14), 2,	// Chaos Castle 4
	ITEMGET(14,13), 2, ITEMGET(14,14), 3,	// Chaos Castle 5
	ITEMGET(14,13), 2, ITEMGET(14,14), 3,	// Chaos Castle 6
	ITEMGET(14,13), 2, ITEMGET(14,14), 4	// Chaos Castle 7
};

static int g_iChaosCastle_MonsterItems_UBF[MAX_CHAOSCASTLE_LEVEL][MAX_CC_MONSTER_DROP_ITEM] =
{
	ITEMGET(14,261), 4,	// Chaos Castle 1
	ITEMGET(14,261), 4,	// Chaos Castle 2
	ITEMGET(14,261), 4,	// Chaos Castle 3
	ITEMGET(14,261), 4,	// Chaos Castle 4
	ITEMGET(14,261), 5,	// Chaos Castle 5
	ITEMGET(14,261), 5,	// Chaos Castle 6
	ITEMGET(14,261), 6	// Chaos Castle 7
};

static int g_iChaosCastle_GroundAxis[4] = { 23, 75, 44, 108};
static int g_iChaosCastle_DamageAxis[MAX_CC_TRAP_STEP][4] = 
{
	0x17, 0x4B,	0x2C, 0x6C,
	0x19, 0x4D,	0x2A, 0x6A,
	0x1B, 0x4F,	0x28, 0x68,
	0x1D, 0x51,	0x26, 0x66
};


static int g_iChaosCastle_MapHollowZone[MAX_CC_TRAP_STEP][MAX_CC_HOLLOW_ZONE][4] =
{
	0x17, 0x4B,	0x2C, 0x4C,
	0x2B, 0x4D,	0x2C, 0x6C,
	0x17, 0x6B,	0x2A, 0x6C,
	0x17, 0x4D,	0x18, 0x6A,
	0x17, 0x4B,	0x2C, 0x4C,
	0x2B, 0x4D,	0x2C, 0x6C,
	0x17, 0x6B,	0x2A, 0x6C,
	0x17, 0x4D,	0x18, 0x6A,
	0x19, 0x4D,	0x2A, 0x4E,
	0x29, 0x4F,	0x2A, 0x6A,
	0x19, 0x69,	0x28, 0x6A,
	0x19, 0x4F,	0x1A, 0x68,
	0x1B, 0x4F,	0x28, 0x50,
	0x27, 0x51,	0x28, 0x68,
	0x1B, 0x67,	0x26, 0x68,
	0x1B, 0x51,	0x1C, 0x66
};

static RECT g_rtPOINT_FRAME[MAX_CC_TRAP_STEP];
static RECT g_rtPOINT_TRAP[MAX_CC_TRAP_STEP];











///////////////////////////////////////////////////////////////////////
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::://
///////////////////////////////////////////////////////////////////////

struct _MONSTER_ITEM_DROP
{
	int m_iIndex;	// 0
	int m_iItemKind;	// 3

	_MONSTER_ITEM_DROP()	// line : 229
	{
		this->m_iIndex = -1;
		this->m_iItemKind = -1;
	};	// line : 232
};

struct CHAOSCASTLE_START_TIME
{
	int m_iHour;	// 0
	int m_iMinute;	// 4
};

struct CHAOSCASTLE_MONSTER_POSITION
{
	WORD m_Type;
	BYTE m_MapNumber;
	BYTE m_Dis;
	BYTE m_X;
	BYTE m_Y;
	BYTE m_Dir;
};

struct _CHAOSCASTLE_USER
{
	_CHAOSCASTLE_USER()	// line : 338
	{
		this->Clear();
	};	// line : 340

	void Clear()	// line :342
	{
		this->m_iIndex = -1;
		this->m_iEXP = 0;
		this->m_iScore = 0;
		this->m_iUserState = 0;
		this->m_bSendQuitMsg = false;
	};	// line : 348

	int m_iIndex;	// 0
	int m_iEXP;	// 4
	int m_iScore;	// 8
	int m_iUserState;	// C
	bool m_bSendQuitMsg;	// 10
};

struct _CHAOSCASTLE_DATA
{
	int m_iCC_STATE;	// 0
	int m_iMapNumber;	// 4
	int m_iChaosCastleIndex;	// 8
	float m_iCastleExperience;
	int m_iMISSION_SUCCESS;	// C
	int m_iCC_REMAIN_MSEC;	// 10
	int m_iCC_TICK_COUNT;	// 14
	int m_iCC_NOTIFY_COUNT;	// 18
	bool m_bCC_CAN_ENTER;	// 1C
	bool m_bCC_CAN_PARTY;	// 1D
	bool m_bCC_PLAY_START;	// 1E
	bool m_bCC_MSG_BEFORE_ENTER;	// 1F
	bool m_bCC_MSG_BEFORE_PLAY;	// 20
	bool m_bCC_MSG_BEFORE_END;	// 21
	bool m_bCC_MSG_BEFORE_QUIT;	// 22
	int m_iCC_ENTERED_USER;	// 24
	int m_iCC_FIRST_USER_COUNT;	//28
	int m_iCC_FIRST_MONSTER_COUNT;	// 2C
	long m_lCC_CURRENT_MONSTER_COUNT;	// 30
	int m_iCC_TRAP_STATUS;	// 34
	int m_iCC_TRAP_OLD_STATUS;	// 38
	_CHAOSCASTLE_USER m_UserData[MAX_CHAOSCASTLE_USER];	// 3C
	long m_lCC_MONSTER_COUNT[MAX_CHAOSCASTLE_MONSTER];	// 5B4
	long m_lFallUser[MAX_CHAOSCASTLE_USER];	// 744
	std::map<int,_MONSTER_ITEM_DROP> m_mapMonsterItemList;	// 85C
	CRITICAL_SECTION m_TrapCriti;

	void Clear()	// line : 387
	{
		this->m_iMISSION_SUCCESS = -1;
		this->m_iCC_NOTIFY_COUNT = -1;
		this->m_bCC_MSG_BEFORE_ENTER = false;
		this->m_bCC_MSG_BEFORE_PLAY = false;
		this->m_bCC_MSG_BEFORE_END = false;
		this->m_bCC_MSG_BEFORE_QUIT = false;
		this->m_bCC_CAN_ENTER = false;
		this->m_bCC_CAN_PARTY = false;
		this->m_bCC_PLAY_START = false;
		this->m_iCC_ENTERED_USER = 0;
		this->m_iCC_FIRST_USER_COUNT = 0;
		this->m_iCC_FIRST_MONSTER_COUNT = 0;
		this->m_lCC_CURRENT_MONSTER_COUNT = 0;
		this->m_iCC_TRAP_STATUS = 0;
		this->m_iCC_TRAP_OLD_STATUS = 0;

		for ( int iMON = 0;iMON < MAX_CHAOSCASTLE_MONSTER ; iMON++)
		{
			this->m_lCC_MONSTER_COUNT[iMON] = -1;
		}

		for ( int iFALL = 0; iFALL < MAX_CHAOSCASTLE_USER ; iFALL ++)
		{
			this->m_lFallUser[iFALL] = -1;
		}

		this->m_mapMonsterItemList.clear();
	};	// line : 408
};



class CChaosCastle
{

public:

	CChaosCastle();
	virtual ~CChaosCastle();

	friend int CGMMng::ManagementProc(CGameObject &lpObj, char* szCmd, CGameObject &lpObj);

	void Run();
	void Init();
	void Load(char* filename);
	void LoadMonster(char* filename);
	void SetState(int iChaosCastleIndex, int iCC_STATE);
	int EnterUserChaosCastle(int iChaosCastleIndex, int iUserIndex);
	int LeaveUserChaosCastle(int iChaosCastleIndex, int iUserIndex);
	void AddFallUser(int iChaosCastleIndex, int iUserIndex);
	void ProcessFallUser(int iChaosCastleIndex);
	//int __thiscall GetCurrentAliveUser(int);
	void SetUserState(int iUserIndex, int iState);
	int GetCurrentWinUser(int iChaosCastleIndex);
	int CalcSendRewardEXP(int iUserIndex, int iEXP, int iKILLCOUNT_USER, int iKILLCOUNT_MONSTER);
	int LevelUp(int iUserIndex, int iAddExp);
	int GetUserLevelToEnter(int iUserIndex);
	int CheckUserEnterMoney(int iUserIndex, int iEnterLevel);
	int PayUserEnterMoney(int iUserIndex, int iEnterLevel);
	void GiveUserDamage(int iUserIndex, int iDamage);
	void ClearMonster(int iChaosCastleIndex);
	int SetMonster(int iChaosCastleIndex);
	void AddMonsterList(int iChaosCastleIndex, int iMonsterIndex);
	void DelMonsterList(int iChaosCastleIndex, int iMonsterIndex);
	int GetMonsterListCount(int iChaosCastleIndex);
	void SetItemsToMonster(int iChaosCastleIndex);
	void SearchNDropMonsterItem(int iChaosCastleIndex, int iMonsterIndex, int iMaxHitUserIndex);
	void SendAllUserAnyMsg(LPBYTE lpMsg, int iSize);
	void SendChaosCastleAnyMsg(LPBYTE lpMsg, int iSize, int iChaosCastleIndex);
	void SendNoticeMessage(int iChaosCastleIndex, LPSTR lpszMSG);
	void SendNoticeState(int iChaosCastleIndex, int iPlayState);
	void SendWinMessage(int iChaosCastleIndex, int iWinnerIndex);
	void SendFailMessage(int iChaosCastleIndex, int iLoserIndex);
	void SendAllLoserFailMessage(int iChaosCastleIndex, int iWinnerIndex);
	int GetCurrentState(int iChaosCastleIndex);
	int GetCurEnteredUser(int iChaosCastleIndex);
	int GetCurPlayUser(int iChaosCastleIndex);
	int GetRemainTime(int iChaosCastleIndex);
	int GetCurrentRemainSec(int iChaosCastleIndex);
	float GetExperienceBonus(int iChaosCastleIndex);
	//int __thiscall GetExcelItemDropRate();
	//int __thiscall GetNormalItemDropRate();
	int CheckEnterLevel(int iIndex, int iLevel);
	int CheckEnterItem(int iIndex);
	int CheckPlayEnded(int iChaosCastleIndex);
	bool CheckCanEnter(int iChaosCastleIndex);
	bool CheckPlayStart(int iChaosCastleIndex);
	bool CheckCanStartPlay(int iChaosCastleIndex);
	int ObjSetPosition(int iIndex, int iX, int iY);
	void SearchNBlowObjs(int iMapNumber, int iX, int iY);
	int BlowObjsFromPoint(int iIndex, int iMapNumber, int& iX, int& iY);
	int BlowObjsToPoint(int iIndex, int iMapNumber, int iX, int iY);
	int CalDistance(int iX1, int iY1, int iX2, int iY2);
	void SafetyCastleZone(int iChaosCastleIndex);
	void UnSafetyCastleZone(int iChaosCastleIndex);
	void SendCastleZoneSafetyInfo(int iChaosCastleIndex, bool bDoSet);
	void CheckUserInDieTile(int iChaosCastleIndex);
	void CheckMonsterInDieTile(int iChaosCastleIndex);
	void PlayFailedRollBack(int iChaosCastleIndex);
	int ReCalcUserCount(int iChaosCastleIndex);
	void RewardUserEXP(int iChaosCastleIndex, int iChaosCastleSubIndex, BOOL bWinner);
	void ProcessTrapStatus(int iChaosCastleIndex);
	void CalUsersInTrap(int iChaosCastleIndex);
	int CheckWearingMOPH(int iUserIndex);
	void GiveWinnerItem(int iChaosCastleIndex, int iWinnerIndex);
	void PullObjInnerPlace(int iChaosCastleIndex, int iTRAP_STEP);
	void SetMapAttrHollow(int iChaosCastleIndex, int iTRAP_STEP);
	void SetMapAttrFill(int iChaosCastleIndex);
	void ChaosCastleRank(int iUserIndex, int iPlayer, int iMonster, __int64 iExp, bool bWinner);
	int GetChaosCastleMapNumber(int iChaosCastleIndex);
	int GetChaosCastleIndex(int iMAP_NUM);
	bool IsEventEnabled() { return this->m_bCC_EVENT_ENABLE; }
	void SetEventEnable(bool bEnable) { this->m_bCC_EVENT_ENABLE = bEnable; }
	CHAOSCASTLE_MONSTER_POSITION *GetMonsterPosData(int iPosNum, int iChaosCastleIndex);
	void GD_Req_Save_KillPoint_UBF(int index, char *Name, int KillPoint, int CastleIndex);
	void RewardUserKillPointUBF(int iChaosCastleIndex, int iChaosCastleSubIndex, bool bWinner);
	void GDReqSetUBFReward_CCBattle(int iUserIndex, BYTE byRewardType);
	void SetUBFGetReward(int iUserIndex, WORD wItemCode, UINT64 ItemSerial, BYTE btItemPos);
	void GiveUBFReward(int iUserIndex, BYTE byRewardType);

	int GetMaxEnterCount() { return this->m_iCC_MAX_ENTER_COUNT; }

protected:

	void CheckSync(int iChaosCastleIndex);
	void ClearChaosCastleData(int iChaosCastleIndex);
	void ProcState_None(int iChaosCastleIndex);
	void ProcState_Closed(int iChaosCastleIndex);
	void ProcState_Playing(int iChaosCastleIndex);
	void ProcState_PlayEnd(int iChaosCastleIndex);
	void SetState_None(int iChaosCastleIndex);
	void SetState_Closed(int iChaosCastleIndex);
	void SetState_Playing(int iChaosCastleIndex);
	void SetState_PlayEnd(int iChaosCastleIndex);

private:

	BOOL m_bCC_EVENT_ENABLE;	// 4
	int m_iCC_TIME_MIN_OPEN;	// 8
	int m_iCC_TIME_MIN_PLAY;	// C
	int m_iCC_TIME_MIN_REST;	// 10
	int m_iCC_MIN_USER_START;
	int m_iCC_MAX_ENTER_COUNT;
	int m_iCC_MOB_CNT_SCRIPT[MAX_CHAOSCASTLE_LEVEL];

	CHAOSCASTLE_MONSTER_POSITION m_CCMP[MAX_CHAOSCASTLE_LEVEL][100];
	std::vector<CHAOSCASTLE_START_TIME> m_vtChaosCastleOpenTime;	// 20
	_CHAOSCASTLE_DATA m_stChaosCastleData[MAX_CHAOSCASTLE_LEVEL];	// 30

};





extern CChaosCastle g_ChaosCastle;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

