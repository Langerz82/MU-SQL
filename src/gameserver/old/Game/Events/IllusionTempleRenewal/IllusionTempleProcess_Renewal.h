////////////////////////////////////////////////////////////////////////////////
// IllusionTempleProcess_Renewal.h
#ifndef ITR_PROCESS_H
#define ITR_PROCESS_H

#include "User/CUserData.h"

struct __ITR_CUserData
{
	__ITR_CUserData()
	{
		this->m_nIndex = -1;
	}

	int m_nIndex;
	BYTE m_btTeam;
	DWORD m_dwSkillProdectionTime;
	DWORD m_dwSkillRestraintTime;
	BYTE m_btUserKillCount;
	BYTE m_byOccupiedCount;
	BYTE m_byInvalidationCount;
	int m_bReward;
	bool m_bEnterOk;
	int m_nPartyIdx;
	int m_nRelicsInvenPos;
};

struct __ITR_PARTYTEAM
{
	int nPartyIndex;
	int nTeamIndex;
};

struct __ITR_OBJECT_DATA
{
	int m_nIndex;
	WORD m_wType;
	short m_wPosNum;
	BYTE m_byStatus;
	BYTE byMapTagIndex;
};

struct ITR_NPC_POSITIONINFO
{
	WORD wNpcType;
	BYTE byMapNumber;
	BYTE byDistance;
	BYTE byX;
	BYTE byY;
	BYTE byDir;
	BYTE byMapTagIndex;
};

#pragma once
class CIllusionTempleProcess_Renewal
{
public:
	CIllusionTempleProcess_Renewal();
	virtual ~CIllusionTempleProcess_Renewal();

	void ITR_ProcessInit(int nTempleNum);
	void ClearUserData(int nIndex);
	void CheckSync();

	void Proc_ITR(int nCurTime);
	void Proc_ITRState_RoundBreak(int nCurTime);
	void Proc_ITRState_None(int nCurTime);
	void Proc_ITRState_Ready(int nCurTime);
	void Proc_ITRState_Playing(int nCurTime);
	void Proc_ITRState_End(int nCurTime);

	void Set_ITRState(BYTE btState);
	BYTE GetIllusionTempleState();

	void Set_ITRState_None();
	void Set_ITRState_Ready();
	void Set_ITRState_RoundBreak();
	void Set_ITRState_Playing();
	void Set_ITRState_End();

	BOOL EnterUserIllusionTemple(CGameObject &Obj, BYTE btTempleIndex, int nPartyIdx);
	BOOL LeaveUserIllusionTemple(CGameObject &Obj);

	bool CanStartITRBattle(BYTE & ALLIED, BYTE & ILLUSION);
	void CheckUsersOnConnect();

	int GetRemainTimeMin();
	int GetEnterRemainTimeMin();

	int SearchRelicsItem(CGameObject &Obj);
	void DeleteAllRelicsItem();
	int FindITRUser(int index);
	void DropRelicsItem(CGameObject &Obj, BYTE byAct);
	char CheckHaveRelics(CGameObject &Obj);
	void SetRelicsInventoryPos(CGameObject &Obj, BYTE btPos);

	void SendRelicsUserInfo(CGameObject lpObj, BYTE byGet);

	void SetRemainTime();

	void AddITRNpcInfo(WORD wNpcType, BYTE byMapNumber, BYTE byDistance, BYTE byX, BYTE byY, BYTE byDir);
	void ResetITRNpcInfo();

	void ResetITRPosNum();
	void RemoveAllObj();

	void SendRelicsError(CGameObject lpObj, int index, BYTE byError, BYTE byAct);
	void Check_GetRelics(CGameObject &Npc, CGameObject lpObj, WORD wNpcType);
	void GetRelics(CGameObject &Npc, CGameObject lpObj, WORD wNpcType);
	void Check_RegisterRelics(CGameObject &Npc, CGameObject lpObj);
	void RegisterRelics(CGameObject &Npc, CGameObject lpObj);

	void FirstRegen_OccupiedStone();
	void FirstRegen_CursedStone();
	void RegenCursedStoneStatus(BYTE byRound);

	void SetStatusRegenTime();
	void AddITROccupiedStonePos(WORD wNpcType, int index, int Count, BYTE byMapTagIndex);
	void AddITRCursedStonePos(WORD wNpcType, int index, BYTE byMapTagIndex);
	void SetNpc();
	void RegenOccupiedStoneStatus();

	void SendNoticeMessage(char *lpMsg);
	void Send_ITR_StoneInfo(int userindex, WORD wNpcType, BYTE byState, int NpcIndex, BYTE byMapTagIndex);
	void Send_ITR_PlayState(BYTE btTeam);
	void SendIllusionTempleState(BYTE btTempleState, int nIndex);

	BYTE GetUserTeam(int nIndex);
	void IncUserKillCount(int nIndex);
	void ResetKillCount();

	void ITR_UseSkill(int nIndex, WORD wSkillNumber, int nTargetIndex, BYTE btDis);
	void ITRSkillProc(CGameObject &Obj);
	void Send_ITR_SkillEnd(CGameObject lpObj, WORD wSkillNumber);
	int UseSkillProdection(CGameObject &Obj);
	int UseSkillRestraint(CGameObject lpObj, CGameObject &TargetObj);
	int UseSkillTeleport(CGameObject &Obj);
	int UseSkillShieldBurn(CGameObject lpObj, CGameObject &TargetObj);
	void SendUseSkillResult(int nIndex, int nTargetIndex, WORD wSkillNumber, BYTE btResult);

	void ITRUserDie(CGameObject* lpObj);
	void GetOccupiedStoneCount(BYTE &byAllied, BYTE &byIllusion);

	void CalCursetTempleResult();
	void DropITR_RewardBox(int nIndex);

	void SendAllUserAnyMsg(char *lpMsg);

	int GetRealRemainTimeMin();

	BOOL CheckSkillProdection(int iIllusionTempleIndex);
	BOOL CheckSkillRestraint(int iIllusionTempleIndex);

	void SendITRResult();
	void MoveToStartPoint_WhenRoundBreak();
	void CalcKillCountByTeam(BYTE &byAlliedKillSum, BYTE &byIllusionKillSum);

	void ResetUserFlag_DoingRelicsThing(CGameObject &Obj);
	void CancleStoneStateProcess(WORD wNpcType, BYTE byState);
	int GetRelicsUserIndex(CGameObject &Obj);

	BYTE FindEmptyPartySlot(int partyindex, BYTE &byEmptySlot);
	BYTE SetPartyTeam(BYTE bySlotOrder, int nPartyIdx, int nUserIndex);
	BYTE DeletePartyInfo(int partyindex);
	BYTE Find_SameParty(int partyindex, int &nUserIndex);
	BYTE Find_PartySlot(int partyindex, BYTE &bySlotNum);

	bool GetEntrance() { return this->m_bEntrance; }
	void SetAllCheckSync() { this->m_bAllCheckSync = true; }
	bool GetAllCheckSync() { return this->m_bAllCheckSync; }

private:
	CRITICAL_SECTION m_critUserData;

	__ITR_CUserData m_UserData[10];
	__ITR_PARTYTEAM m_PartyTeam[2];

	int m_nUserCount;
	char m_nTempleNumber;
	BYTE m_btIllusionTempleState;

	__ITR_OBJECT_DATA m_Cursed_StoneStatus;
	__ITR_OBJECT_DATA m_Occupied_StoneStatus[10];

	int m_bEntrance;
	BYTE m_byITRRound;
	bool m_bITRRoundStartFail;
	bool m_bNobodyWinLose;

	int m_StatusRegenTime;
	int m_nRemainTimeMin;
	int m_nRemainTimeMsec;
	int m_nTickCount;
	int m_nNextTimeMin;
	int m_nRoundBreakCount;
	bool m_bRoundBreak;

	int m_nEventEndRemainTime;
	bool m_bGameEndPosMove;
	bool m_bSendMsgBeforeReady;
	bool m_bSendMsgBeforePlay;

	int m_nAlliedUserCount;
	int m_nIllusionUserCount;

	BYTE m_btWinTeam;
	BYTE m_btLoseTeam;

	int m_bAllCheckSync;

	BYTE m_byIllusionTeamPoint;
	BYTE m_byAlliedTeamPoint;

	ITR_NPC_POSITIONINFO m_ITRNpcInfo[11];
	BYTE m_byNpcCount;
	bool m_bIsSetNpc;
	bool m_bFirstRegen_CursedStone;
	bool m_bFirstRegen_OccStone;

	int m_bSendResult;
};

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

