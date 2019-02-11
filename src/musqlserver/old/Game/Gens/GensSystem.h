////////////////////////////////////////////////////////////////////////////////
// GensSystem.h
#ifndef GENSSYSTEM_H
#define GENSSYSTEM_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "GensSystemProtocol.h"
#include "MoveCommand.h"
#include "MuDefines.h"


#define MAX_GENS_CLASS 14

struct BATTLEZONE_MAP_DATA
{
	WORD m_wMapNumber;
	BYTE m_btEntryAllowType;
	float m_fExpBonus[MAX_INFLUENCE_TYPE - 1];
	float m_fMLExpBonus[MAX_INFLUENCE_TYPE - 1];
	int m_iDropBonus[MAX_INFLUENCE_TYPE - 1];
	int m_iMLDropBonus[MAX_INFLUENCE_TYPE - 1];
	int m_iExcDropBonus[MAX_INFLUENCE_TYPE - 1];
	int m_iMLExcDropBonus[MAX_INFLUENCE_TYPE - 1];
};

class GensSystem
{
public:
	GensSystem();
	virtual ~GensSystem();

	int GDReqAbusingInfo(CGameObject &lpObj); 
	int ReqExDBGensInfo(CGameObject &lpObj);
	void ReqRegGensMember(CGameObject &lpObj, BYTE btInfluence);
	int AnsRegGensMember(int iObjIndex, BYTE btResult);
	int ReqSecedeGensMember(CGameObject &lpObj);
	int AnsSecedeGensMember(int iObjIndex, BYTE btResult);

	bool IsInfluenceNPC(CGameObject &lpObj);

	int SendGensInfo(CGameObject &lpObj);
	int SendPkPenaltyMapMove(CGameObject &lpObj);
	int GDReqSaveContributePoint(CGameObject &lpObj);
	int DBSaveAbusingKillUserName(CGameObject &lpObj);
	void BattleZoneChatMsgSend(CGameObject &lpObj, LPBYTE Msg, int size);
	int IsMoveMapBattleZone(int iMapIndex);
	void GensViewportListProtocol(CGameObject &lpObj);
	int IsMapBattleZone(int iMapIndex);
	void SetGensInfluence(CGameObject &lpObj, int iInfluence);
	int IsPkEnable(CGameObject &lpObj, CGameObject lpTargetObj);
	int GetGensInfluence(CGameObject &lpObj);
	char *GetGensInfluenceName(CGameObject &lpObj);
	bool IsRegGensInfluence(CGameObject &lpObj);
	int SetUserBattleZoneEnable(CGameObject &lpObj, int bBattleZoneEnable);
	int IsUserBattleZoneEnable(CGameObject &lpObj);
	int SetContributePoint(CGameObject &lpObj, int iContributePoint);
	int AddContributePoint(CGameObject &lpObj, int iContributePoint);
	int SubContributePoint(CGameObject &lpObj, int iContributePoint);
	int GetContributePoint(CGameObject &lpObj);
	int PkPenaltyAddNeedZenMapMove(CGameObject &lpObj);
	void PkPenaltyDropInvenItem(CGameObject &lpObj);
	void PkPenaltyDropZen(CGameObject &lpObj);
	void SendPKPenaltyDebuff(CGameObject &lpObj);
	void CalcContributePoint(CGameObject &lpObj, CGameObject lpTargetObj);
	int ChkKillUserName(CGameObject &lpObj, char *szCharName);
	int AbusingPenalty(CGameObject &lpObj, int iKillUserIndex);
	int InsertKillUserName(CGameObject &lpObj, char *szCharName);
	void MoveInBattleZonePartySplit(CGameObject &lpObj);
	void AbusingInfoReset(CGameObject &lpObj);
	int CalGensClass(CGameObject &lpObj);
	int SetGensRanking(CGameObject &lpObj, int iGensRanking);
	int SetGensClass(CGameObject &lpObj, int iGensClass);
	int GetGensClass(CGameObject &lpObj);
	int GetNextContributePoint(CGameObject &lpObj);
	int SetGensMemberCount(int iInfluence, int iMemberCount);
	int GetGensMemberCount(int iInfluence);
	int ReqExDBGensRewardCheck(CGameObject &lpObj, int iInfluence);
	int ReqExDBGensRewardComplete(CGameObject &lpObj);
	int SendGensReward(CGameObject &lpObj, BYTE btResult);
	int SendGensRewardItem(CGameObject &lpObj, int iGensClass);
	bool GensRewardInventoryCheck(CGameObject &lpObj, int iItemCount);
	int ReqExDBGensMemberCount(CGameObject &lpObj);
	int ReqExDBSetGensRewardDay();
	int ReqExDBSetGensRanking();
	int LoadData(char *lpszFileName);
	int ReqGensRewardDay(CGameObject &lpObj);

	int GetPKPartyLevel(CGameObject &lpObj);

	void SendBattleZoneData(CGameObject &lpObj);

	bool CanGensSeeOppositeChat() { return this->m_bGensCanSeeOppositeGensChat; }
	bool CanGensJoinPartyWhileOppositeGens() { return this->m_bGensCanJoinPartyWhileOppositeGens; }
	bool CanGensJoinGuildWhileOppositeGens() { return this->m_bGensCanJoinGuildWhileOppositeGens; }
	bool CanGensJoinUnionWhileOppositeGens() { return this->m_bGensCanJoinUnionWhileOppositeGens; }
	bool CanGensCreatePartyOnBattleZone() { return this->m_bGensCanCreatePartyOnBattleZone; }

	bool IsGensMuBlueMode() { return this->m_bGensMapFromMuBlue; }

	int GetInitialContributePoint() { return this->m_iGensInitialContributePoint; }

	int GetAbusingResetTime() { return this->m_iGensAbusingResetTime; }
	int GetGensAttackWarpDelay() { return this->m_iGensAttackWarpDelay; }

	int GetDamageReduction() { return this->m_iGensDamageReduction; }

	float GetBattleZoneExpBonus(CGameObject &lpObj);
	int GetBattleZoneDropBonus(CGameObject &lpObj);
	int GetBattleZoneExcDropBonus(CGameObject &lpObj);

	bool GetEntryAllowType(CGameObject &lpObj, BYTE btMapNumber);

private:
	BOOL								m_bPkPenaltyDropZen;
	BOOL								m_bPkPenaltyDropInvenItem;									
	
	int									m_iGensAbusingPenaltyWarning;
	int									m_iGensAbusingPenaltyLimit;
	int									m_iGensAbusingResetTime;
	int									m_iGensInitialContributePoint;
	int									m_iGensPkPenaltyAddNeedZenMapMove;

	int									m_iGensDamageReduction;
										
	bool								m_bGensCanSeeOppositeGensChat;
	bool								m_bGensCanJoinPartyWhileOppositeGens;
	bool								m_bGensCanJoinGuildWhileOppositeGens;
	bool								m_bGensCanJoinUnionWhileOppositeGens;
	bool								m_bGensCanCreatePartyOnBattleZone;
	bool								m_bGensMapFromMuBlue;		
										
	int									m_iVanertMemberCount;
										
	int									m_iDuprianMemberCount;
										
	bool								m_bFileLoad;

	DWORD								m_iGensAttackWarpDelay; // custom for joinmu

	std::map<int, BATTLEZONE_MAP_DATA>	m_mapBattleZoneData;
	int									m_wBattleZoneWarp[MAX_MOVE_COMMAND];

	CRITICAL_SECTION					criti;
	STR_GENS_RANKING_DATA				m_GensRanking[MAX_GENS_CLASS];
};

extern GensSystem g_GensSystem;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

