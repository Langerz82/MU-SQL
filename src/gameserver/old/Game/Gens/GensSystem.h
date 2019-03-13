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

struct GENS_RANKING_DATA;

class GensSystem
{
public:
	GensSystem();
	virtual ~GensSystem();

	int GDReqAbusingInfo(CGameObject &Obj); 
	int ReqExDBGensInfo(CGameObject &Obj);
	void ReqRegGensMember(CGameObject &Obj, BYTE btInfluence);
	int AnsRegGensMember(int iObjIndex, BYTE btResult);
	int ReqSecedeGensMember(CGameObject &Obj);
	int AnsSecedeGensMember(int iObjIndex, BYTE btResult);

	bool IsInfluenceNPC(CGameObject &Obj);

	int SendGensInfo(CGameObject &Obj);
	int SendPkPenaltyMapMove(CGameObject &Obj);
	int GDReqSaveContributePoint(CGameObject &Obj);
	int DBSaveAbusingKillUserName(CGameObject &Obj);
	void BattleZoneChatMsgSend(CGameObject &Obj, LPBYTE Msg, int size);
	int IsMoveMapBattleZone(int iMapIndex);
	void GensViewportListProtocol(CGameObject &Obj);
	int IsMapBattleZone(int iMapIndex);
	void SetGensInfluence(CGameObject &Obj, int iInfluence);
	int IsPkEnable(CGameObject &Obj, CGameObject lpTargetObj);
	int GetGensInfluence(CGameObject &Obj);
	char *GetGensInfluenceName(CGameObject &Obj);
	bool IsRegGensInfluence(CGameObject &Obj);
	int SetUserBattleZoneEnable(CGameObject &Obj, int bBattleZoneEnable);
	int IsUserBattleZoneEnable(CGameObject &Obj);
	int SetContributePoint(CGameObject &Obj, int iContributePoint);
	int AddContributePoint(CGameObject &Obj, int iContributePoint);
	int SubContributePoint(CGameObject &Obj, int iContributePoint);
	int GetContributePoint(CGameObject &Obj);
	int PkPenaltyAddNeedZenMapMove(CGameObject &Obj);
	void PkPenaltyDropInvenItem(CGameObject &Obj);
	void PkPenaltyDropZen(CGameObject &Obj);
	void SendPKPenaltyDebuff(CGameObject &Obj);
	void CalcContributePoint(CGameObject &Obj, CGameObject lpTargetObj);
	int ChkKillUserName(CGameObject &Obj, char *szCharName);
	int AbusingPenalty(CGameObject &Obj, int iKillUserIndex);
	int InsertKillUserName(CGameObject &Obj, char *szCharName);
	void MoveInBattleZonePartySplit(CGameObject &Obj);
	void AbusingInfoReset(CGameObject &Obj);
	int CalGensClass(CGameObject &Obj);
	int SetGensRanking(CGameObject &Obj, int iGensRanking);
	int SetGensClass(CGameObject &Obj, int iGensClass);
	int GetGensClass(CGameObject &Obj);
	int GetNextContributePoint(CGameObject &Obj);
	int SetGensMemberCount(int iInfluence, int iMemberCount);
	int GetGensMemberCount(int iInfluence);
	int ReqExDBGensRewardCheck(CGameObject &Obj, int iInfluence);
	int ReqExDBGensRewardComplete(CGameObject &Obj);
	int SendGensReward(CGameObject &Obj, BYTE btResult);
	int SendGensRewardItem(CGameObject &Obj, int iGensClass);
	bool GensRewardInventoryCheck(CGameObject &Obj, int iItemCount);
	int ReqExDBGensMemberCount(CGameObject &Obj);
	int ReqExDBSetGensRewardDay();
	int ReqExDBSetGensRanking();
	int LoadData(char *lpszFileName);
	int ReqGensRewardDay(CGameObject &Obj);

	int GetPKPartyLevel(CGameObject &Obj);

	void SendBattleZoneData(CGameObject &Obj);

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

	float GetBattleZoneExpBonus(CGameObject &Obj);
	int GetBattleZoneDropBonus(CGameObject &Obj);
	int GetBattleZoneExcDropBonus(CGameObject &Obj);

	bool GetEntryAllowType(CGameObject &Obj, BYTE btMapNumber);

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

	GENS_RANKING_DATA					m_GensRanking[MAX_GENS_CLASS];
	CRITICAL_SECTION					criti;
};

extern GensSystem g_GensSystem;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

