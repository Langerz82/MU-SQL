#ifndef _MU_DEVILSQUAREFINAL_H
#define _MU_DEVILSQUAREFINAL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "User/CUserData.h"

#include "generalStructs.h"

struct PMSG_REQ_DSF_CAN_PARTY_ENTER;
struct PMSG_REQ_SAVE_DSF_PARTYPOINT;
struct PMSG_REQ_DSF_PARTYRANKRENEW;
struct PMSG_REQ_DSF_GO_FINAL_PARTY;
struct PMSG_REQ_SAVE_DSF_REWARD_USER;
struct PMSG_REQ_GET_DSF_REWARD;
struct PMSG_ANS_DSF_CAN_PARTY_ENTER;
struct PMSG_ANS_DSF_PARTYRANKRENEW;
struct PMSG_ANS_DSF_GO_FINAL_PARTY;
struct PMSG_ANS_GET_DSF_REWARD;
struct PMSG_REQ_SET_DSF_WINNER_INFO;


#pragma once
class CDevilSquareFinal
{
public:
	CDevilSquareFinal(void);
	~CDevilSquareFinal(void);

	void Initialize_DSFSchedule();
	void Load_DSFScheduleScript(char *pchFileName);

	void Initalize_DSFMonster();
	void Load_DSFMonsterScript(char *pchFileName);

	void ReqDSFSchedule(CGameObject &Obj);

	void DSFEventInit();

	BOOL CheckChangeRing(int nType);
	BOOL CheckWearingMOPH(CGameObject &Obj);
	
	void DSF_Run();

	BYTE WhatIsTodayTypeOfDSF();

	void Proc_DSF(int nCurTime);
	void Proc_DSFState_None(int nCurTime);
	void Proc_DSFState_Ready(int nCurTime);
	void Proc_DSFState_Playing(int nCurTime);
	void Proc_DSFState_RoundBreak(int nCurTime);
	void Proc_DSFState_End(int nCurTime);

	void SendAllUserAnyMsg(LPBYTE lpMsg, int iSize);

	void Set_DSFState(BYTE btState);
	void Set_DSFState_None();
	void Set_DSFState_Ready();
	void Set_DSFState_Playing();
	void Set_DSFState_RoundBreak();
	void Set_DSFState_End();

	BOOL IsAlreadyExistUserInDSF(CGameObject &Obj);

	char Find_PartySlot(int nPartyNumber, BYTE & btSlotNum);
	char Find_EmptyPartySlot(int nPartyNumber, BYTE & btSlotNum);
	char SetParty(BYTE btSlotOrder, BYTE btDSFType, int nPartyIdx, int nUserIndex);

	BOOL IsGoSemiFinalParty(char *szUserName1, char *szUserName2, WORD wServerCode1, WORD wServerCode2);
	BOOL IsGoFinalParty(char *szUserName1, char *szUserName2, WORD wServerCode1, WORD wServerCode2);

	void DeleteParty(int nPartyNumber);
	void DeletePartyByIndex(int nIndex);

	BOOL Enter_DSF(CGameObject &Obj, BYTE btSlotNum);
	BOOL Leave_DSF(CGameObject &Obj);

	void ClearUserData(int nIndex);

	void SendDSFState(BYTE btDSFState, int nIndex);
	void Send_DSF_PlayState(BYTE btTeam);

	void CheckUsersOnConnect();
	bool CanStartDSFBattle();

	void SendDSFResult(CGameObject &Obj);
	void SaveWinnerPartyPoint();
	
	void GDSaveDSFPartyPoint(char *szAccountID1, char *szUserName1, int nUser1Level, int nClass1, char *szAccountID2, char *szUserName2, int nUser2Level, int nClass2, int iDSFType, int iPoint, BYTE btEnterCnt);
	void GDReqDSFPartyRankRenew(BYTE btDSFType);
	void GDReqDSFGoFinalParty(int nDSFType);
	void GDInsertRewardUser(char *szAccountID, char *szUserName, int iClass, int iDSFType);
	void GDReqGetReward(CGameObject &Obj);

	void DSF_ProcessInit();

	int SetMonster();
	void DSFMonsterRegen(CGameObject &Obj);
	void ClearMonster();

	BYTE GetUserTeam(CGameObject &Obj);

	int MonsterKillPoint(int nMonsterType);
	void MonsterKillPointCalc(CGameObject &Obj, int nPoint);

	void DSFUserDie(CGameObject &Obj);
	void EndUserMove();
	
	void DSFDropReward(CGameObject & lpUser, CGameObject &Monster);
	void SetUBFGetReward(CGameObject &Obj, WORD wItemCode, UINT64 ItemSerial, BYTE btItemPos);
	void GDReqSetDSFReward_UBF(CGameObject &Obj, BYTE btDSFType, BYTE btRewardType);

	void GiveDSFWinnerItem(int iWinnerIndex, BYTE btDSFType, BYTE btRewardType);

public:
	bool m_bEVENT_ENABLE;
	int m_nDSF_TYPE;
	BYTE m_btDSFCurState;

	bool m_bGetGoFinalUser;

	int m_nTickCount;
	int m_nNextTimeMin;
	int m_nCurBattleStartTime;
	int m_nRemainTimeMin;
	int m_nRemainTimeMsec;

	bool m_bEntrance;
	bool m_bSendMsgBeforeReady;
	bool m_bRoundBreak;

	int m_nEventEndRemainTime;

	BYTE m_btDSFRound;

	bool m_bResultPointSend;
	bool m_bWinnerResultMsgSend;
	bool m_bGameStart;

	int m_nUserCount;
	int m_nTeamUserCount[10];
	int m_nTeamPoint[10];
	BYTE m_btWinTeam;

	int m_nDSFTestType;

	DSF_TIME_RULE_INFO m_DSFTimeRuleInfo;
	DSF_UNIT_INFO m_DSFUnitInfo;
	DSF_START_DAY m_DSFStartDay;
	DSF_START_HOUR_TYPE m_DSFStartHourType[25];
	DSF_REWARD_DAY m_DSFRewardDay;

	DSF_EVENT_MONSTER_RATE m_DSFEventMonsterRate[15];
	DSF_AFTER_ROUND_ADD_POINT m_DSFAfterRoundAddPoint[15];
	DSF_EVENT_MONSTER_INFO m_DSFEventMonsterInfo[15];
	DSF_MONSTER_INFO m_DSFMonsterInfo[15];
	DSF_MONSTER_REGEN_INFO m_DSFMonsterRegenInfo[200];

	CRITICAL_SECTION m_criPartyData;
	DSF_PARTY m_PartyDataSave[10];

	CRITICAL_SECTION m_critUserData;
	DSF_USER m_UserData[20];
	DSF_ENTER_PLAYER_NAME m_EnteredPlayerName[20];

	DSF_PARTY_RANK m_GoSemiFinalParty[40];
	DSF_PARTY_RANK m_GoFinalParty[10];
	DSF_PARTY_RANK m_WinnerParty;
};

extern CDevilSquareFinal g_DevilSquareFinal;

#endif
