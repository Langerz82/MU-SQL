#ifndef _GENERALSTRUCTS_H
#define _GENERALSTRUCTS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "MuDefines.h"
#include "ServerEngine.h"

#include <string>
#include <map>

struct STR_ITEM_LEVEL_RATE;
struct STR_LUCKY_ITEM_EQUIPMENT;
struct STR_SMELT_RATE;
struct STR_LUCKY_ITEM_INFO;
struct STR_USER_SHOP_REBUY_ITEM;
struct STR_ITEMPERIOD_DATEINFO;
struct STR_GUILD_MEMBER;
struct STR_GUILD_INFO_STRUCT;
struct STR_UNION_MEMBER_DATA;
struct PARTYMATCHING_PARTYUSERDATA;
struct STR_sellItemPrice;
struct STR_PARTY_MEMBER_WAIT_LIST;
struct STR_PSHOP_ITEMVALUE_INFO_DS;
struct STR_STAT_BONUS;
struct STR_PARTYMATCHING_PARTYUSERDATA;


typedef struct STR_STRINGCOMPARE
{
	bool operator()(const std::string s1, const std::string s2) const
	{
		return strcmp(s1.data(), s2.data()) < 0;
	}
} strCmp;

typedef std::map<std::string, STR_GUILD_MEMBER*, strCmp> MAP_GUILD_MEMBER;
typedef std::map<std::string, STR_GUILD_INFO_STRUCT*, strCmp> MAP_GUILD_INFO;
typedef std::map<int, STR_UNION_MEMBER_DATA*> MAP_MEMBER_DATA;



struct STR_ITEM_LEVEL_RATE
{
	int iLevel;
	int	iRate;

	STR_ITEM_LEVEL_RATE::STR_ITEM_LEVEL_RATE()
	{
		this->iLevel		= 0;
		this->iRate			= 0;
	}
};

struct STR_LUCKY_ITEM_EQUIPMENT
{
	int	iItemNum;
	int	iSkill;
	int	iLuckOption;
	int	iAddOption;
	int	iClass;

	STR_LUCKY_ITEM_EQUIPMENT::STR_LUCKY_ITEM_EQUIPMENT()
	{
		this->iItemNum		= -1;
		this->iSkill		= 0;
		this->iLuckOption	= 0;
		this->iAddOption	= 0;
		this->iClass		= -1;
	}
};

struct STR_SMELT_RATE
{
	int	iMinDurab;
	int	iMaxDurab;
	int	iSmeltRate;

	STR_SMELT_RATE::STR_SMELT_RATE()
	{
		this->iMinDurab		= 0;
		this->iMaxDurab		= 0;
		this->iSmeltRate	= 0;
	}
};

struct STR_LUCKY_ITEM_INFO
{
	int						iItemNum;
	int						iItemKind;
	int						iAddOpt3Rate[7];
	int						iAddSetOptRate[2];
	STR_ITEM_LEVEL_RATE		ItemLevelRate[MAX_LUCKYINFO_LEVELRATE];
	STR_LUCKY_ITEM_EQUIPMENT	LuckyItemEquipment[MAX_LUCKYINFO_ITEMEQUIPMENT];
	STR_SMELT_RATE				SmeltRate[MAX_LUCKYINFO_SMELTRATE];

	STR_LUCKY_ITEM_INFO::STR_LUCKY_ITEM_INFO()
	{
		this->iItemNum	= -1;
		this->iItemKind	= -1;
		memset(this->iAddOpt3Rate, 0, sizeof(iAddOpt3Rate));
	}
};

struct STR_USER_SHOP_REBUY_ITEM
{
	STR_USER_SHOP_REBUY_ITEM()
	{
		this->Clear();
	}

	void Clear()
	{
		this->btUsed = FALSE;
		this->wItemCode = 0xFFFF;
		this->iLeftTime = 0;
		memset(this->btItemInfo, 0x00, MAX_ITEM_INFO);
		this->wItemCount = 0;
		this->dwItemPrice = 0;
		this->btItemNumber = 0;
	}

	BYTE btUsed;
	WORD wItemCode;
	int iLeftTime;
	BYTE btItemInfo[MAX_ITEM_INFO];
	WORD wItemCount;
	DWORD dwItemPrice;
	BYTE btItemNumber;
};

struct stArcaBattleMarkTopRankDS {
	BYTE btRank;
	char szGuildNames[MAX_GUILD_LEN + 1];
	DWORD dwMarkCnt;
};

struct stCancelGuildNames {
	char szGuildNames[9];
};

struct stCCFRankingInfo {
	stCCFRankingInfo() {
		memset(this->szCharName, 0x00, sizeof(this->szCharName));
		this->byRank = 0;
		this->nPoint = 0;
	}
	char szCharName[MAX_ACCOUNT_LEN + 1];
	BYTE byRank;
	int nPoint;
};

struct stGuildMatchingAllowList {
	char szName[MAX_ACCOUNT_LEN + 1];
	BYTE btClass;
	int nLevel;
};

struct stGuildMatchingAllowListDB {
	int nGuildNumber;
	char szApplicantName[MAX_ACCOUNT_LEN + 1];
	BYTE btApplicantClass;
	int nApplicantLevel;
	BYTE btState;
};

struct stGuildMatchingList {
	char szMemo[41];
	char szRegistrant[MAX_ACCOUNT_LEN + 1];
	char szGuildName[MAX_GUILD_LEN + 1];
	BYTE btGuildMemberCnt;
	BYTE btGuildMasterClass;
	BYTE btInterestType;
	BYTE btLevelRange;
	BYTE btClassType;
	int nGuildMasterLevel;
	int nBoardNumber;
	int nGuildNumber;
	BYTE btGensType;
};

struct stGuildUnderMember {
	BYTE btGuildMemberCnt;
	char szGuildNames[9];
};

struct ITL_REWARDLIST {
	BYTE byRank;
	BYTE byEnteredCnt;
	BYTE byPoint;
	bool bGotReward;
	char szCharName[MAX_ACCOUNT_LEN + 1];
	char szGuildName[MAX_GUILD_LEN + 1];
};

struct MuRummyCardInfoDS {
	MuRummyCardInfoDS() {
		this->btColor = -1;
		this->btNumber = -1;
		this->btSlotNum = -1;
		this->btSeq = -1;
		this->btStatus = -1;
	}
	BYTE btColor;
	BYTE btNumber;
	BYTE btSlotNum;
	BYTE btSeq;
	BYTE btStatus;
};

struct MuRummyCardUpdateDS {
	MuRummyCardUpdateDS() {
		this->btSlotNum = -1;
		this->btSeq = -1;
		this->btStatus = -1;
	}
	BYTE btSlotNum;
	BYTE btSeq;
	BYTE btStatus;
};

struct MuunRideViewPortInfo {
	BYTE NumberH;
	BYTE NumberL;
	BYTE MuunRideItemH;
	BYTE MuunRideItemL;
};

struct STR_QUESTEXP_INFO {
	STR_QUESTEXP_INFO::STR_QUESTEXP_INFO() {
		this->dwQuestIndexID = 0;
		this->wProgState = 0;
		this->lStartDate = 0;
		this->lEndDate = 0;
		memset(this->btAskIndex, 0, 5);
		memset(this->btAskValue, 0, 5);
		memset(this->btAskState, 0, 5);
	}
	DWORD dwQuestIndexID;
	WORD wProgState;
	BYTE btAskIndex[5];
	BYTE btAskValue[5];
	BYTE btAskState[5];
	time_t lStartDate;
	time_t lEndDate;
};

struct SEND_PARTYMEMBER_INFO {
	SEND_PARTYMEMBER_INFO() {
		this->bUse = FALSE;
	}
	int bUse;
	int bFlag;
	char Name[MAX_ACCOUNT_LEN + 1];
	int nUserIndex;
	int nServerChannel;
};

struct stABAllGuildMark {
	char szGuildName[MAX_GUILD_LEN + 1];
	DWORD dwMarkCnt;
};

struct stABJoinUserInfoDS {
	char szGuildName[MAX_GUILD_LEN + 1];
	DWORD dwGuild;
	char szUserName[MAX_ACCOUNT_LEN + 1];
};

struct stABWinGuildInfoDS {
	char szGuildName[9];
	DWORD dwGuild;
	WORD wOccupyObelisk;
	WORD wObeliskGroup;
};

struct STR_GENS_RANKING_DATA
{
	int GensClass;
	int ContributePoint;
	int RankingPosition;
	BYTE RewardItemGroup;
	WORD RewardItemNumber;
	int RewardItemCount;
};

struct _ITL_GUILD_RANK {
	char szGuildName[MAX_GUILD_LEN + 1];
	BYTE byRank;
	BYTE byITLType;
	BYTE byWin;
	BYTE byLose;
};

struct STR_GUILD_MEMBER {
	char m_szMemberName[11];
	BYTE m_btStatus;
	BYTE m_btConnected;
	int m_iUserIndex;
	STR_GUILD_MEMBER() {
		ZeroMemory(m_szMemberName, sizeof(m_szMemberName));
		m_btStatus = 0;
		m_btConnected = -1;
		m_iUserIndex = -1;
	}
};


struct PARTY_INFO_LIST {
	char szLeaderName[MAX_ACCOUNT_LEN + 1];
	char szTitle[41];
	WORD nMinLevel;
	WORD nMaxLevel;
	WORD nHuntingGround;
	int nLeaderLevel;
	BYTE btLeaderClass;
	BYTE btWantedClass;
	BYTE btCurPartyMemberCnt;
	BYTE btUsePassword;
	BYTE btWantedClassDetailInfo[7];
	int nServerChannel;
	BYTE btGensType;
};

struct PARTY_INFO_LISTDB {
	char szLeaderName[MAX_ACCOUNT_LEN + 1];
	char szTitle[41];
	char szPassword[5];
	WORD nMinLevel;
	WORD nMaxLevel;
	WORD nHuntingGround;
	int nLeaderLevel;
	BYTE btLeaderClass;
	BYTE btWantedClass;
	BYTE btCurPartyMemberCnt;
	BYTE btUsePassword;
	BYTE btWantedClassDetailInfo[7];
	BYTE btGensType;
	BYTE btApprovalType;
};

struct STR_PARTY_MEMBER_WAIT_LIST
{
	char szName[MAX_ACCOUNT_LEN + 1];
	BYTE btClass;
	int nLevel;
};

struct NAME {
	char m_szName[11];
	NAME() {
		std::ZeroMemory(this, sizeof(*this));
	}
	NAME(char * szName) {
		std::ZeroMemory(this, sizeof(*this));
		std::strncpy(m_szName, szName, 10);
	}
};

struct P_ADD_NEWS {
	BYTE day;
	BYTE month;
	WORD year;
	DWORD dateColor;
	DWORD titleColor;
	DWORD textColor;
	char title[40];
	WORD textLen;
	char Text[2048];
};

struct PARTYMATCHING_PARTYDATA {
	PARTYMATCHING_PARTYDATA() {
		this->m_PartyMatchingIndex = -1;
		this->m_UserCount = 0;
	}
	int m_PartyMatchingIndex;
	int m_UserCount;
	char szLeaderName[MAX_ACCOUNT_LEN + 1];
	//STR_PARTYMATCHING_PARTYUSERDATA m_UserData[5];
	STR_PARTYMATCHING_PARTYUSERDATA* m_UserData;
};

struct STR_PARTYMATCHING_PARTYUSERDATA {
	STR_PARTYMATCHING_PARTYUSERDATA() {
		this->iUsedInfo = FALSE;
		this->iUserConnected = FALSE;
		this->iUserIndex = -1;
		this->iServerNumber = -1;
		this->iServerIndex = -1;
	}
	int iUsedInfo;
	int iUserConnected;
	int iUserIndex;
	int iServerNumber;
	int iServerIndex;
	char szUserName[MAX_ACCOUNT_LEN + 1];
};

struct PERIOD_EXPIRED_ITEM {
	WORD wItemCode;
	UINT64 Serial;
};

struct PERIOD_EXPIRED_ITEMLIST {
	BYTE btItemCount;
	PERIOD_EXPIRED_ITEM ExpiredItem[90];
};

struct PERIOD_ITEMLIST {
	DWORD dwUserGuid;
	BYTE btItemCount;
	char chCharacterName[MAX_ACCOUNT_LEN + 1];
	//STR_ITEMPERIOD_DATEINFO ItemData[100];
	STR_ITEMPERIOD_DATEINFO* ItemData;
};

struct STR_sellItem {
	DWORD dwItemGuid;
	DWORD dwItemCODE;
	DWORD dwCategoryID;
	DWORD dwUseType;
	DWORD dwBuyType;
	DWORD dwCoolTime;
	double dbVarEndDate;
	DWORD dwLimitSellCount;
	DWORD dwState;
	char szItemName[64];
	char szItemDesc[128];
	DWORD dwPriceCount;
	//STR_sellItemPrice itemPrice[8];
	STR_sellItemPrice* itemPrice;
};

struct STR_sellItemPrice {
	DWORD dwPriceGuid;
	DWORD dwItemGuid;
	DWORD dwUseTime;
	DWORD dwAmount;
	DWORD dwPrice;
	DWORD dwSellRate;
};

struct sellPackage {
	DWORD dwPackageGuid;
	STR_sellItemPrice itemPrice;
	DWORD dwImageCode;
	DWORD dwCategory;
	DWORD dwBuyType;
	DWORD dwLimitSellCount;
	DWORD dwInItemCount;
	double dbVarEndDate;
	DWORD dwState;
	STR_sellItemPrice inItemPrices[16];
	char szPackageName[64];
	char szPackageDesc[128];
};

struct ServerInfo {
	WORD UserCount;
	WORD MaxUserCount;
	WORD Percent;
	DWORD TickCount;
};

struct ServerList {
	ServerList() {
		this->btServerCode = 0;
		this->wServerPort = 0;
		memset(this->cIp, 0, 16);
	}
	BYTE btServerCode;
	WORD wServerPort;
	char cIp[16];
};

struct STAT_BONUS {
	WORD BonusOptionID;
	WORD BonusOptionVal1;
	WORD BonusOptionVal2;
};

struct stMemberPosInfo {
	char szName[MAX_ACCOUNT_LEN + 1];
	BYTE btMapNumber;
	BYTE btPosX;
	BYTE btPosY;
};

struct STR_GUILD_INFO_STRUCT {
	int m_iNumber;
	char m_szGuildName[9];
	char m_szGuildMaster[11];
	BYTE m_btGuildType;
	MAP_GUILD_MEMBER m_mapGuildMember;
	int m_iRivalGuild;
	int m_iUnionGuild;
	BYTE m_Mark[32];
	int m_Score;
	char m_Notice[128];
	BOOL m_bGSHasData;
	STR_GUILD_INFO_STRUCT() {
		m_iNumber = -1;
		memset(m_szGuildName, 0, sizeof(m_szGuildName));
		memset(m_szGuildMaster, 0, sizeof(m_szGuildMaster));
		m_btGuildType = -1;
		memset(m_Mark, 0, sizeof(m_Mark));
		memset(m_Notice, 0, sizeof(m_Notice));
		m_iRivalGuild = 0;
		m_iUnionGuild = 0;
		m_Score = 0;
		m_mapGuildMember.clear();
		m_bGSHasData = FALSE;
	}
};

struct JOIN_SERVER_SERVER_DATA {
	int m_ServerIndex;
	char m_szServerName[50];
	WORD m_wServerCode;
	WORD m_wPort;
	WORD m_wCurrentUser;
	WORD m_wMaxUser;
	BYTE m_VipServer;
	WORD m_MaxHWIDUseCount;
};

struct JOIN_SERVER_USER_DATA {
	char m_AccoundID[11];
	char m_HWID[100];
	WORD m_ServerCode;
	WORD m_ServerGroup;
	bool m_bRequestMapSvrMove;
	int iUserNumber;
	DWORD dwPlayTime;
	char m_Ip[17];
	int iPayCode;
	int iUsedTime;
	char szEndDays[9];
	DWORD dwCheckTime;
	int iIndex;
	bool m_bOffTrade;
	char IpAddr[17];
};

typedef struct STR_UNION_MEMBER_DATA {
	std::vector<int> m_vecUnionMember;
} STR_UNION_MEMBER_DATA, *LP_STR_UNION_MEMBER_DATA;

struct USERWAREHOUSE_DATA {
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	int WarehouseID;
	DWORD LastChangeTick;
	int ChangeIDEnableState;
	bool WarehouseOpenState;
};

struct STR_ITEMPERIOD_DATEINFO
{
	BYTE btUsedInfo;
	BYTE btItemType;
	WORD wItemCode;
	BYTE btEffectCategory;
	BYTE btEffectType1;
	BYTE btEffectType2;
	UINT64 Serial;
	time_t lItemBuyDate;
	time_t lItemExpireDate;
};

struct STR_STAT_BONUS
{
	WORD BonusOptionID;
	WORD BonusOptionVal1;
	WORD BonusOptionVal2;
};

struct STR_SERVER_ENGINE
{
	int m_Index;
	SOCKET m_Socket;
	eSERVER_STATE m_State;
	eSERVER_TYPE m_Type;
	char m_ServerIp[16];
	struct _PER_SOCKET_CONTEXT * PerSocketContext;
	WsProtocolCore m_ProtocolCore;
	WORD m_ServerCode;
	WORD m_ServerGroup;
	unsigned short m_ConnectPort;
};

struct IPList
{
	char m_IP[16];
};

struct STR_CS_USER
{
	STR_CS_USER() {}
	int Index;
	int ConnectionState;
	char IP[20];
	unsigned int Port;
	unsigned char Type;
	SOCKET socket;
	_PER_SOCKET_CONTEXT * PerSocketContext;
	bool News;
	int PacketCount;
	ULONGLONG i64PacketTime;
};

struct _PER_IO_CONTEXT
{
	MU_WSAOVERLAPPED Overlapped; // 0
	MU_WSABUF wsabuf;
	unsigned char Buffer[MAX_IO_BUFFER_SIZE]; // 1C
	unsigned char BufferSecond[MAX_IO_BUFFER_SIZE]; // 178C
	int nSecondOfs; // 2EFC
	int nTotalBytes;	// 2F00
	int nSentBytes; // 2F04
	int IOOperation; // 2F08
	int nWaitIO; // 2F0C

};

typedef struct tagIocpServerParameter
{
	DWORD dwServerCount;
	eSERVER_TYPE eServerType;

} IocpServerParameter;

struct _PER_SOCKET_CONTEXT
{
	SOCKET m_socket;	// 0
	int nIndex;	// 4
	_PER_IO_CONTEXT IOContext[2];	// 8
	int dwIOCount;	// 5E28
};

struct PacketQueue
{
	PacketQueue()
	{
		this->nSize = 0;
		this->headcode = 0;
		this->uindex = -1;
		this->iSessionId = -1;
	}

	boost::shared_ptr<unsigned char[]> pData;
	unsigned short nSize;
	unsigned char headcode;
	int uindex;
	int iSessionId;
};

#endif
