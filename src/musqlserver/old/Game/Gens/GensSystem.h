////////////////////////////////////////////////////////////////////////////////
// GensSystem.h
#ifndef GENSSYSTEM_H
#define GENSSYSTEM_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GensSystemProtocol.h"
#include "MoveCommand.h"

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

	int GDReqAbusingInfo(LPGameObject &lpObj); 
	int ReqExDBGensInfo(LPGameObject &lpObj);
	void ReqRegGensMember(LPGameObject &lpObj, BYTE btInfluence);
	int AnsRegGensMember(int iObjIndex, BYTE btResult);
	int ReqSecedeGensMember(LPGameObject &lpObj);
	int AnsSecedeGensMember(int iObjIndex, BYTE btResult);

	bool IsInfluenceNPC(LPGameObject &lpObj);

	int SendGensInfo(LPGameObject &lpObj);
	int SendPkPenaltyMapMove(LPGameObject &lpObj);
	int GDReqSaveContributePoint(LPGameObject &lpObj);
	int DBSaveAbusingKillUserName(LPGameObject &lpObj);
	void BattleZoneChatMsgSend(LPGameObject &lpObj, LPBYTE Msg, int size);
	int IsMoveMapBattleZone(int iMapIndex);
	void GensViewportListProtocol(LPGameObject &lpObj);
	int IsMapBattleZone(int iMapIndex);
	void SetGensInfluence(LPGameObject &lpObj, int iInfluence);
	int IsPkEnable(LPGameObject &lpObj, CGameObject* lpTargetObj);
	int GetGensInfluence(LPGameObject &lpObj);
	char *GetGensInfluenceName(LPGameObject &lpObj);
	bool IsRegGensInfluence(LPGameObject &lpObj);
	int SetUserBattleZoneEnable(LPGameObject &lpObj, int bBattleZoneEnable);
	int IsUserBattleZoneEnable(LPGameObject &lpObj);
	int SetContributePoint(LPGameObject &lpObj, int iContributePoint);
	int AddContributePoint(LPGameObject &lpObj, int iContributePoint);
	int SubContributePoint(LPGameObject &lpObj, int iContributePoint);
	int GetContributePoint(LPGameObject &lpObj);
	int PkPenaltyAddNeedZenMapMove(LPGameObject &lpObj);
	void PkPenaltyDropInvenItem(LPGameObject &lpObj);
	void PkPenaltyDropZen(LPGameObject &lpObj);
	void SendPKPenaltyDebuff(LPGameObject &lpObj);
	void CalcContributePoint(LPGameObject &lpObj, CGameObject* lpTargetObj);
	int ChkKillUserName(LPGameObject &lpObj, char *szCharName);
	int AbusingPenalty(LPGameObject &lpObj, int iKillUserIndex);
	int InsertKillUserName(LPGameObject &lpObj, char *szCharName);
	void MoveInBattleZonePartySplit(LPGameObject &lpObj);
	void AbusingInfoReset(LPGameObject &lpObj);
	int CalGensClass(LPGameObject &lpObj);
	int SetGensRanking(LPGameObject &lpObj, int iGensRanking);
	int SetGensClass(LPGameObject &lpObj, int iGensClass);
	int GetGensClass(LPGameObject &lpObj);
	int GetNextContributePoint(LPGameObject &lpObj);
	int SetGensMemberCount(int iInfluence, int iMemberCount);
	int GetGensMemberCount(int iInfluence);
	int ReqExDBGensRewardCheck(LPGameObject &lpObj, int iInfluence);
	int ReqExDBGensRewardComplete(LPGameObject &lpObj);
	int SendGensReward(LPGameObject &lpObj, BYTE btResult);
	int SendGensRewardItem(LPGameObject &lpObj, int iGensClass);
	bool GensRewardInventoryCheck(LPGameObject &lpObj, int iItemCount);
	int ReqExDBGensMemberCount(LPGameObject &lpObj);
	int ReqExDBSetGensRewardDay();
	int ReqExDBSetGensRanking();
	int LoadData(char *lpszFileName);
	int ReqGensRewardDay(LPGameObject &lpObj);

	int GetPKPartyLevel(LPGameObject &lpObj);

	void SendBattleZoneData(LPGameObject &lpObj);

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

	float GetBattleZoneExpBonus(LPGameObject &lpObj);
	int GetBattleZoneDropBonus(LPGameObject &lpObj);
	int GetBattleZoneExcDropBonus(LPGameObject &lpObj);

	bool GetEntryAllowType(LPGameObject &lpObj, BYTE btMapNumber);

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
	GENS_RANKING_DATA					m_GensRanking[MAX_GENS_CLASS];
};

extern GensSystem g_GensSystem;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

