// EDSprotocol.h: interface for the EDSprotocol class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EDSPROTOCOL_H__675D1E8D_B9AC_45E3_8077_17EC82A9A5A5__INCLUDED_)
#define AFX_EDSPROTOCOL_H__675D1E8D_B9AC_45E3_8077_17EC82A9A5A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

using namespace std;


#include "StdAfx.h"
//#include "ProtocolStructs.h"
#include "Fprotocol.h"
#include "GuildClass.h"

#define MAX_MEMBER_GUILD	80

struct GENS_RANKING;
struct GUILD_INFO_STRUCT;
struct PARTY_MEMBER_WAIT_LIST;
struct _stRegWaitGuildMatching;
struct _stReqAddPartyMember;
struct _stReqAllowJoinGuildMatching;
struct _stReqCancelPartyMatching;
struct _stReqChattingPartyMatching;
struct _stReqDelGuildMatchingList;
struct _stReqDelPartyUserPartyMatching;
struct _stReqDelWaitGuildMatchingList;
struct _stReqGetPartyMatchingList;
struct _stReqGuildMatchingData;
struct _stReqGuildMatchingList;
struct _stReqGuildMatchingListSearchWord;
struct _stReqJoinMemberPartyMatching;
struct _stReqJoinMemberStateListPartyMatching;
struct _stReqRegWantedPartyMember;
struct _stReqSendPartyMemberList;
struct _stReqWaitGuildMatchingList;
struct _stReqWaitListPartyMatching;
struct _stReqWaitStateListGuildMatching;
struct EXSDHP_GUILD_ASSIGN_STATUS_REQ;
struct EXSDHP_GUILD_ASSIGN_STATUS_REQ;
struct EXSDHP_GUILD_ASSIGN_STATUS_RESULT;
struct EXSDHP_GUILD_ASSIGN_TYPE_REQ;
struct EXSDHP_GUILD_ASSIGN_TYPE_REQ;
struct EXSDHP_GUILD_ASSIGN_TYPE_RESULT;
struct EXSDHP_KICKOUT_UNIONMEMBER_REQ;
struct EXSDHP_KICKOUT_UNIONMEMBER_REQ;
struct EXSDHP_KICKOUT_UNIONMEMBER_RESULT;
struct EXSDHP_NOTIFICATION_RELATIONSHIP;
struct EXSDHP_RELATIONSHIP_BREAKOFF_REQ;
struct EXSDHP_RELATIONSHIP_BREAKOFF_REQ;
struct EXSDHP_RELATIONSHIP_BREAKOFF_RESULT;
struct EXSDHP_RELATIONSHIP_JOIN_REQ;
struct EXSDHP_RELATIONSHIP_JOIN_REQ;
struct EXSDHP_RELATIONSHIP_JOIN_RESULT;
struct EXSDHP_SERVERGROUP_GENS_CHATTING_RECV;
struct EXSDHP_SERVERGROUP_GENS_CHATTING_RECV;
struct EXSDHP_SERVERGROUP_GUILD_CHATTING_RECV;
struct EXSDHP_SERVERGROUP_GUILD_CHATTING_RECV;
struct EXSDHP_SERVERGROUP_UNION_CHATTING_RECV;
struct EXSDHP_SERVERGROUP_UNION_CHATTING_RECV;
struct EXSDHP_UNION_LIST;
struct EXSDHP_UNION_LIST_COUNT;
struct EXSDHP_UNION_LIST_REQ;
struct EXSDHP_UNION_LIST_REQ;
struct EXSDHP_UNION_RELATIONSHIP_LIST;
struct FCHP_CHATROOM_CREATE_REQ;
struct FCHP_CHATROOM_CREATE_RESULT;
struct FCHP_CHATROOM_CREATE_RESULT;
struct FCHP_CHATROOM_INVITATION_REQ;
struct FHP_FRIEND_ADD_REQ;
struct FHP_FRIEND_CHATROOM_CREATE_REQ;
struct FHP_FRIEND_CHATROOM_CREATE_REQ;
struct FHP_FRIEND_CHATROOM_CREATE_RESULT;
struct FHP_FRIEND_INVITATION_REQ;
struct FHP_FRIEND_INVITATION_REQ;
struct FHP_FRIEND_INVITATION_RET;
struct FHP_FRIEND_MEMO_DEL_REQ;
struct FHP_FRIEND_MEMO_LIST_REQ;
struct FHP_FRIEND_MEMO_RECV_REQ;
struct FHP_FRIEND_MEMO_SEND;
struct FHP_FRIEND_STATE_C;
struct FHP_FRIENDLIST_REQ;
struct FHP_WAITFRIEND_ADD_REQ;
struct FRIEND_MASTER;
struct GENS_USER;
struct NONE_INFLUENCE;
struct PARTYMATCHING_PARTYDATA;
struct PARTYMATCHING_PARTYDATA;
struct PARTYMATCHING_PARTYUSERDATA;
struct PMSG_ANS_ABUSING_INFO;
struct PMSG_ANS_GENS_INFO_EXDB;
struct PMSG_ANS_GENS_MEMBER_COUNT_EXDB;
struct PMSG_ANS_GENS_REWARD_DAY_CHECK_EXDB;
struct PMSG_ANS_GENS_REWARD_EXDB;
struct PMSG_ANS_REG_GENS_MEMBER_EXDB;
struct PMSG_ANS_SECEDE_GENS_MEMBER_EXDB;
struct PMSG_REQ_ABUSING_INFO;
struct PMSG_REQ_ABUSING_INFO;
struct PMSG_REQ_GENS_MEMBER_COUNT;
struct PMSG_REQ_GENS_MEMBER_COUNT;
struct PMSG_REQ_GENS_REWARD_CHECK_EXDB;
struct PMSG_REQ_GENS_REWARD_CHECK_EXDB;
struct PMSG_REQ_GENS_REWARD_COMPLETE_EXDB;
struct PMSG_REQ_GENS_REWARD_COMPLETE_EXDB;
struct PMSG_REQ_GENS_REWARD_DAY;
struct PMSG_REQ_GENS_REWARD_DAY;
struct PMSG_REQ_GUILD_PERIODBUFF_DELETE;
struct PMSG_REQ_GUILD_PERIODBUFF_INSERT;
struct PMSG_REQ_REG_GENS_MEMBER_EXDB;
struct PMSG_REQ_REG_GENS_MEMBER_EXDB;
struct PMSG_REQ_SAVE_ABUSING_KILLUSER_EXDB;
struct PMSG_REQ_SAVE_ABUSING_KILLUSER_EXDB;
struct PMSG_REQ_SAVE_CONTRIBUTE_POINT_EXDB;
struct PMSG_REQ_SAVE_CONTRIBUTE_POINT_EXDB;
struct PMSG_REQ_SECEDE_GENS_MEMBER_EXDB;
struct PMSG_REQ_SECEDE_GENS_MEMBER_EXDB;
struct PMSG_SET_GENS_REWARD_DAY_EXDB;
struct SDHP_GUILDALL;
struct SDHP_GUILDALL_COUNT;
struct SDHP_GUILDCREATE;
struct SDHP_GUILDCREATE;
struct SDHP_GUILDCREATE_RESULT;
struct SDHP_GUILDDESTROY;
struct SDHP_GUILDDESTROY;
struct SDHP_GUILDDESTROY_RESULT;
struct SDHP_GUILDMEMBER_INFO;
struct SDHP_GUILDMEMBER_INFO_GUILDNAME_REQUEST;
struct SDHP_GUILDMEMBER_INFO_GUILDNAME_REQUEST;
struct SDHP_GUILDMEMBER_INFO_REQUEST;
struct SDHP_GUILDMEMBER_INFO_REQUEST;
struct SDHP_GUILDMEMBERADD;
struct SDHP_GUILDMEMBERADD;
struct SDHP_GUILDMEMBERADD_RESULT;
struct SDHP_GUILDMEMBERADD_RESULT_WITHOUT_USERINDEX;
struct SDHP_GUILDMEMBERADD_WITHOUT_USERINDEX;
struct SDHP_GUILDMEMBERADD_WITHOUT_USERINDEX;
struct SDHP_GUILDMEMBERDEL;
struct SDHP_GUILDMEMBERDEL;
struct SDHP_GUILDMEMBERDEL_RESULT;
struct SDHP_GUILDNOTICE;
struct SDHP_GUILDNOTICE;
struct SDHP_GUILDSCOREUPDATE;
struct SDHP_GUILDSCOREUPDATE;
struct SDHP_RESULT;
struct SDHP_SERVERINFO;
struct SDHP_SERVERINFO;
struct SDHP_USERCLOSE;
struct SDHP_USERCLOSE;
struct SS_CHAT;
struct SS_CHAT;
struct SS_GUILD;
struct ST_EXDATASERVER;
struct STR_UNION_MEMBER_DATA;
struct _stAnsGuildMatchingList;
struct _stAnsGuildMatchingData;
struct _stAnsDelGuildMatchingList;
struct _stGuildMatchingAllowListDB;
struct _stAnsNotiGuildMatching;
struct _stAnsWaitGuildMatching;
struct _stAnsDelWaitGuildMatchingList;
struct _stAnsAllowJoinGuildMatching;
struct _stAnsWaitGuildMatchingList;
struct _stAnsWaitStateListGuildMatching;
struct _stAnsNotiGuildMatchingForGuildMaster;
struct _stAnsUseGuildMatchingGuild;
struct _stAnsJoinMemberPartyMatching;
struct _stAnsJoinMemberStateListPartyMatching;
struct _stAnsWaitListPartyMatching;
struct _stAnsAddPartyMember;
struct _stAnsCancelPartyMatching;
struct _stAnsDelPartyUserPartyMatching;
struct _stAnsAddRealPartyMember;
struct _stAnsSendPartyMemberList;
struct STR_GENS_RANKING_DATA;

class CPartyMatchingData
{
public:
	CPartyMatchingData();
	~CPartyMatchingData();

	void Init();

	void CreateParty(int Server, int ServerIndex, int nLeaderIndex, char *szLeaderName);
	void DeleteParty(char *szLeaderName);

	PARTYMATCHING_PARTYDATA * GetPartyDataByMaster(char *szLeaderName);
	PARTYMATCHING_PARTYDATA * GetPartyDataByMember(char *szMemberName);
	PARTYMATCHING_PARTYDATA * GetPartyDataByNumber(int nPartyMatchingIndex);

	PARTYMATCHING_PARTYUSERDATA * GetPartyMemberData(char *szMemberName);

	int AddPartyMember(char *szLeaderName, char *szMemberName, int nMemberIndex, int nMemberServer, int nMemberServerIndex);
	int DelPartyMember(char *szLeaderName, char *szMemberName);

	int ConnectMember(char *szMemberName, int nMemberIndex, int nChannelNumber, int nChannelIndex);
	int DisconnectMember(char *szMemberName);

private:
	CRITICAL_SECTION							m_criti;
	std::vector<PARTYMATCHING_PARTYDATA>		m_vtPartyData;

};

#define MAX_GENS_CLASS				14
#define GENS_REWARD_UNAVAILABLE		0
#define GENS_REWARD_WAITING			1
#define GENS_REWARD_ALREADY_GIVEN	2

class GensSystem_EDS
{
public:

	GensSystem_EDS();
	~GensSystem_EDS();

	BOOL InitGensSystem();
	void GensRankingProcess();
	void ManualRefreshRanking(BYTE Type);
	BOOL LoadGensData(LPSTR lpszFile);
	int GetGensInfluence(char *szUserName);

	void GDReqRegGensMember(CGameObject &Obj, PMSG_REQ_REG_GENS_MEMBER_EXDB *aRecv);
	void GDReqSecedeGensMember(CGameObject &Obj, PMSG_REQ_SECEDE_GENS_MEMBER_EXDB *aRecv);
	void GDReqAbusingInfo(CGameObject &Obj, PMSG_REQ_ABUSING_INFO *aRecv);
	void GDReqGensInfo(CGameObject &Obj, PMSG_REQ_ABUSING_INFO *aRecv);
	void GDReqSaveContributePoint(CGameObject &Obj, PMSG_REQ_SAVE_CONTRIBUTE_POINT_EXDB *aRecv);
	void GDReqSaveAbusingKillUserName(CGameObject &Obj, PMSG_REQ_SAVE_ABUSING_KILLUSER_EXDB *aRecv);
	void GDReqGensRewardCheck(CGameObject &Obj, PMSG_REQ_GENS_REWARD_CHECK_EXDB *aRecv);
	void GDReqGensRewardComplete(CGameObject &Obj, PMSG_REQ_GENS_REWARD_COMPLETE_EXDB *aRecv);
	void GDReqGensMemberCount(CGameObject &Obj, PMSG_REQ_GENS_MEMBER_COUNT *aRecv);
	void GDReqSetGensRewardDay(CGameObject &Obj, PMSG_SET_GENS_REWARD_DAY_EXDB *aRecv);
	void GDReqGensRewardDay(CGameObject &Obj, PMSG_REQ_GENS_REWARD_DAY *aRecv);

private:

	int CalcGensClass(int ContributePoint, int Ranking);
	void MakeRanking();
	void ReloadCurrentMonth();

	std::map<string, GENS_USER, STR_STRINGCOMPARE> m_mapGensRankDuprian;
	std::map<string, GENS_USER, STR_STRINGCOMPARE> m_mapGensRankVanert;

	STR_GENS_RANKING_DATA m_GensRankingData[14];

	CRITICAL_SECTION m_GensCriticalSection;
	DWORD m_GensRankingUpdateTimeTick;
	DWORD m_RewardMonth;



	//CLogToFile m_RankingLog;
	int m_DuprianUserCount;
	int m_VanertUserCount;
	int m_GensRankingMonth;
	int m_LeaveDateDayDiff;
	int m_GensRewardStartDay;
	int m_GensRewardEndDay;
	int m_MinRankToGainReward;
	TCHAR g_GensRankingPath[96];
};

class CExDataServerProtocol
{
public:

	CExDataServerProtocol();
	~CExDataServerProtocol();
	
	friend class CFriendSystem;
	static void ProtocolCore(CGameObject &Obj, BYTE HeadCode, LPBYTE aRecv, int iSize);
	BOOL Init();
	BOOL DBConnect();
	
	void GensManualRefreshRanking(BYTE Type);
	void GensRankingProcess();

	void GuildMatchingSendAcceptAndRejectInfo(CGameObject &Obj, char* szUserName, int nUserIndex, int nServerIndex);

private:

	void ExDataServerLogin(CGameObject &Obj, SDHP_SERVERINFO * lpMsg);	
	void GDCharCloseRecv(CGameObject &Obj, SDHP_USERCLOSE * aRecv);
	void GDGuildCreateSend(CGameObject &Obj, SDHP_GUILDCREATE * aRecv);
	void GDGuildDestroyRecv(CGameObject &Obj, SDHP_GUILDDESTROY * aRecv);
	void GDGuildMemberAdd(CGameObject &Obj, SDHP_GUILDMEMBERADD * aRecv);
	void GDGuildMemberAddWithoutUserIndex(CGameObject &Obj, SDHP_GUILDMEMBERADD_WITHOUT_USERINDEX * aRecv);
	void GDGuildMemberDel(CGameObject &Obj, SDHP_GUILDMEMBERDEL * aRecv);
	void DGGuildMemberInfoRequest(CGameObject &Obj, SDHP_GUILDMEMBER_INFO_REQUEST * aRecv);
	void DGGuildScoreUpdate(CGameObject &Obj, SDHP_GUILDSCOREUPDATE * aRecv);
	void GDGuildNoticeSave(CGameObject &Obj, SDHP_GUILDNOTICE * aRecv);
	void GDGuildReqAssignStatus(CGameObject &Obj, EXSDHP_GUILD_ASSIGN_STATUS_REQ * aRecv);
	void GDGuildReqAssignType(CGameObject &Obj, EXSDHP_GUILD_ASSIGN_TYPE_REQ * aRecv);
	void GDGuildServerGroupChattingSend(CGameObject &Obj,EXSDHP_SERVERGROUP_GUILD_CHATTING_RECV * aRecv);
	void GDUnionServerGroupChattingSend(CGameObject &Obj,EXSDHP_SERVERGROUP_UNION_CHATTING_RECV * aRecv);
	void GDGensServerGroupChattingSend(CGameObject &Obj,EXSDHP_SERVERGROUP_GENS_CHATTING_RECV * aRecv);
	void DGRelationShipAnsJoin(CGameObject &Obj, EXSDHP_RELATIONSHIP_JOIN_REQ * aRecv);
	void DGRelationShipAnsBreakOff(CGameObject &Obj, EXSDHP_RELATIONSHIP_BREAKOFF_REQ * aRecv);
	void DGUnionListRecv(CGameObject &Obj, EXSDHP_UNION_LIST_REQ * aRecv);
	void DGRelationShipAnsKickOutUnionMember(CGameObject &Obj, EXSDHP_KICKOUT_UNIONMEMBER_REQ *aRecv);
	void DGGuildMasterListSend(char *szGuild);
	void DGGuildInfoRequest(CGameObject &Obj, SDHP_GUILDMEMBER_INFO_GUILDNAME_REQUEST *aRecv);
	void DGRelationShipListSend(CGameObject &Obj, int iGuild, int relation_type, BOOL snd_all);
	void DGRelationShipNotificationSend(int iGuild, int iUpdateFlag);
	void FCHRoomCreateReq(CGameObject &Obj, char *szName, char *szFriendName, short Number, short ServerId, short FriendNumber, short FriendServerId);
	void FriendChatRoomCreateReq(CGameObject &Obj, FHP_FRIEND_CHATROOM_CREATE_REQ* lpMsg);
	void FCHChatRoomCreateReq(CGameObject &Obj, char *szName, char *szFriendName, short Number, short ServerId, short FriendNumber, short FriendServerId);
	void FriendChatRoomCreateAns(CGameObject &Obj, FCHP_CHATROOM_CREATE_RESULT* lpMsg);
	void FriendChatRoomInvitationReq(CGameObject &Obj, FHP_FRIEND_INVITATION_REQ * lpMsg);
	void FCHChatRoomInvitationReq(CGameObject &Obj, short RoomNumber,	char *szName, short Number,	short ServerId,	BYTE Type);
	void GDReqGuildPeriodBuffInsert(CGameObject &Obj, PMSG_REQ_GUILD_PERIODBUFF_INSERT *aRecv);
	void GDReqGuildPeriodBuffDelete(CGameObject &Obj, PMSG_REQ_GUILD_PERIODBUFF_DELETE *aRecv);

	void CloseExDataServer();

	GUILD_INFO_STRUCT &GetGuild(char *szGuild);
	GUILD_INFO_STRUCT &GetGuild(int iNumber);
	STR_GUILD_MEMBER* GetGuildMember(char *szGuild, char *szName);
	STR_GUILD_MEMBER* GetGuildMember(int nGuildNumber, char *szName);
	STR_UNION_MEMBER_DATA* GetUnionData(int iGuild);

	int GetGuildMemberCount(char *szGuild);
	int GetGuildMemberCount(int nGuildNumber);

	int CreateDBGuild(char* szGuild, char* szMaster, LPBYTE pMark);
	int GetDBGuildNumber(char *szGuild);

	BOOL GuildExists(char *szGuild);
	BOOL AddGuild(char *szGuild, char *szMaster, int iNumber, char *lpNotice, int iScore, LPBYTE pMark, int iType);
	BOOL DelGuild(char *szGuild);
	BOOL MasterDBExists(char *szMaster);
	BOOL DelDBGuild(char *szGuild);

	BOOL GuildMemberExists(char *szGuild, char *szName);
	BOOL AddGuildMember(char *szGuild, char *szName, BYTE btStatus = 0, BYTE btConnected = -1);
	BOOL DelAllDBGuildMember(char *szGuild);
	BOOL DBGuildMemberExists(char *szName);
	BOOL AddDBGuildMember(char *szGuild, char *szName);
	BOOL DelDBGuildMember(char *szName);
	BOOL DelGuildMember(char *szGuild, char *szName);

	BOOL UpdateGuildMemberStatus(char *szGuild, char *szName, BYTE btStatus);
	BOOL UpdateGuildType(char *szGuild, BYTE btType);

	void SendGuildMemberInfo(CGameObject &Obj, char *szName, CGameObject &ObjTarget);
	BOOL GetGuildMemberInfo(char *szName, OUT char *szGuild, OUT int& riLevel, OUT int& riStatus);

	BOOL IsCSGuild(char *szGuild);

	BOOL UpdateScore(char *szGuild, int iScore);
	BOOL SaveNotice(char *szGuild, char *szNotice);

	BOOL CreateRelationShipData(int iGuild);
	BOOL DestroyRelationShipData(int iGuild);

	int RelationShipOperation(int iReqGuild, int iTargGuild, int relation_type, int operation);

	int RelationShipJoin(GUILD_INFO_STRUCT &lpReqGuild, GUILD_INFO_STRUCT &lpTargGuild, int type);
	int UnionJoin(GUILD_INFO_STRUCT &lpReqGuild, GUILD_INFO_STRUCT &lpTargGuild);
	int RivalJoin(GUILD_INFO_STRUCT &lpReqGuild, GUILD_INFO_STRUCT &lpTargGuild);

	int RelationShipBreak(GUILD_INFO_STRUCT &lpMasterGuild, GUILD_INFO_STRUCT &lpKickGuild, int type);
	int UnionBreak(GUILD_INFO_STRUCT &lpMasterGuild, GUILD_INFO_STRUCT &lpKickGuild);
	int RivalBreak(GUILD_INFO_STRUCT &lpMasterGuild, GUILD_INFO_STRUCT &lpKickGuild);

	BOOL AddUnion(GUILD_INFO_STRUCT &lpReqGuild, GUILD_INFO_STRUCT &lpTargGuild);
	BOOL AddUnion(int iReqGuild, int iTargGuild);
	BOOL KickUnion(GUILD_INFO_STRUCT &lpMasterGuild, GUILD_INFO_STRUCT &lpKickGuild);

	BOOL UpdateDBUnion(int iGuild, int iUnion);
	BOOL UpdateDBRival(int iGuild, int iRival);

	void RelationShipBreakSend(int iGuild, int iType);
	void SendListToAllRivals(GUILD_INFO_STRUCT &lpGuild);

	int GetChatServer();

	void GDReqGuildMatchingList(CGameObject &Obj, _stReqGuildMatchingList * lpMsg);
	void GDReqGuildMatchingListSearchWord(CGameObject &Obj, _stReqGuildMatchingListSearchWord * lpMsg);
	void GDReqRegGuildMatchingList(CGameObject &Obj, _stReqGuildMatchingData * lpMsg);
	void GDReqDelMatchingList(CGameObject &Obj, _stReqDelGuildMatchingList * lpMsg);
	void GDReqJoinGuildMatchingList(CGameObject &Obj, _stRegWaitGuildMatching * lpMsg);
	void GDReqCancelJoinGuildMatching(CGameObject &Obj, _stReqDelWaitGuildMatchingList * lpMsg);
	void GDReqAllowJoinGuildMatching(CGameObject &Obj, _stReqAllowJoinGuildMatching * lpMsg);
	void GDReqWaitGuildMatchingList(CGameObject &Obj, _stReqWaitGuildMatchingList * lpMsg);
	void GDReqGetWaitStateListGuildMatching(CGameObject &Obj, _stReqWaitStateListGuildMatching * lpMsg);

	void SendNotiGuildMatchingForGuildMaster(int nGuildNumber);
	void SendUseGuildMatchingGuild(char *szName, int nGuildNumber);
	void UpdateGuildMatchingMemberCount(char *szGuildName, int nGuildMemberCnt);

	void GDReqRegWantedPartyMember(CGameObject &Obj, _stReqRegWantedPartyMember * lpMsg);
	void GDReqGetPartyMatchingList(CGameObject &Obj, _stReqGetPartyMatchingList * lpMsg);
	void GDReqJoinMemberPartyMatching(CGameObject &Obj, _stReqJoinMemberPartyMatching * lpMsg);
	void GDReqJoinMemberStateList(CGameObject &Obj, _stReqJoinMemberStateListPartyMatching *lpMsg);
	void GDReqJoinMemberStateListLeader(CGameObject &Obj, _stReqWaitListPartyMatching *lpMsg);
	void GDReqAcceptMemberJoin(CGameObject &Obj, _stReqAddPartyMember *lpMsg);
	void GDReqCancelPartyMatching(CGameObject &Obj, _stReqCancelPartyMatching *lpMsg);
	void GDReqDeletePartyUser(CGameObject &Obj, _stReqDelPartyUserPartyMatching *lpMsg);
	void GDReqSendPartyMemberList(CGameObject &Obj, _stReqSendPartyMemberList * lpMsg);
	void GDSendChatMsgPartyMatching(CGameObject &Obj, _stReqChattingPartyMatching * lpMsg);

	void FixAddPartyMember(int nLeaderIndex, int nMemberIndex, int nServerIndex);
	void SendPartyMatchingMemberList(char *szLeaderName, BYTE btType);
	void SendPartyMatchingOneList(char *szMemberName);
	void AutoAddPartyMember(char *szLeaderName, char *szMemberName, int nUserIndex, int nServerIndex);

	MAP_GUILD_INFO m_MapGuildManager;
	MAP_MEMBER_DATA m_MapUnionManager;

	GensSystem_EDS				m_GensSystemEDS;
	CFriendSystem				m_FriendSystemEDS;
	CPartyMatchingData			m_PartyMatchingEDS;

};

#endif // !defined(AFX_EDSPROTOCOL_H__675D1E8D_B9AC_45E3_8077_17EC82A9A5A5__INCLUDED_)
