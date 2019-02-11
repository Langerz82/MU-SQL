
#if !defined(AFX_SPROTOCOL_H__5309DA38_D6AD_420B_896B_CB391FCD3624__INCLUDED_)
#define AFX_SPROTOCOL_H__5309DA38_D6AD_420B_896B_CB391FCD3624__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "ProtocolStructs.h"
#include "WzUdp.h"
#include "Database/Query.h"
#include "MapServerManager.h"

#define MAX_LOGIN_USER	(45000)
#define MAX_LOGIN_SERVER (MAX_MAP_GROUPS*MAX_MAP_SUBGROUPS+1)

enum PASSWORD_ENCRYPT_TYPES
{
	PWENC_NONE = 0,
	PWENC_WZMD5 = 1,
	PWENC_MD5 = 2,
	PWENC_SHA2 = 3
};

extern void MuLoginDeleteMapMove(char * szAccountID);
// EventControl
//-----------------------------
#define DRAGON_EVENT		0
#define XMAS_EVENT			1
#define FIRECRACKER_EVENT	2
#define HEARTOFLOVE_EVENT	3
#define MEDAL_EVENT			4

// OptionControl
//-----------------------------
#define OPTION_RELOAD		0
#define MONSTER_RELOAD		1
#define SHOP_RELOAD			2
#define ALLOPTION_RELOAD	3
#define EVENTINFO_RELOAD	4
#define ETCINFO_RELOAD		5
#define OPTION_CONTROL		6

struct JOIN_SERVER_SERVER_DATA;
struct JOIN_SERVER_CUserData;
struct SDHP_SERVERINFO;
struct SDHP_IDPASS;
struct SDHP_JOINFAIL;
struct SDHP_COMMAND_BLOCK;
struct SDHP_USERCLOSE_ID;
struct SDHP_BILLSEARCH;
struct SDHP_LOVEHEARTEVENT;
struct SDHP_LOVEHEARTCREATE;
struct PMSG_REQ_MAPSVRMOVE;
struct PMSG_REQ_MAPSVRAUTH;
struct PMSG_NOTIFY_MAXUSER;
struct PMSG_SET_OFFTRADE;
struct SDHP_USERCLOSE_ID;
struct ISHOP_VIP_BUY;
struct USER_CONNECT_DATA;
struct SDHP_IDPASSRESULT;
struct USER_CONNECT_DATA;


class CLoginServerData
{
public:

	CLoginServerData();
	~CLoginServerData();

	void Init();

	void MuLoginDeleteServer(WORD ServerCode);
	int MuLoginFindServer(WORD ServerCode);
	int MuLoginAddServer(int ServerIndex, LPSTR ServerName, WORD ServerCode, WORD Port, BYTE ServerVIP, WORD MaxHWIDUseCount);

	int GetVIPLevel(int Index)
	{
		return this->m_MuLoginServerData[Index].m_VipServer;
	}

	int GetHWIDUseCount(int Index)
	{
		return this->m_MuLoginServerData[Index].m_MaxHWIDUseCount;
	}

	LPSTR GetServerName(int Index)
	{
		return this->m_MuLoginServerData[Index].m_szServerName;
	}

	int GetServerIndex(int Index)
	{
		return this->m_MuLoginServerData[Index].m_ServerIndex;
	}

	int CheckServerUserLimit(int Index)
	{
		return this->m_MuLoginServerData[Index].m_wCurrentUser >= this->m_MuLoginServerData[Index].m_wMaxUser;
	}

	void GetUserLimitData(int Index, int & CurUser, int & MaxUser)
	{
		CurUser = this->m_MuLoginServerData[Index].m_wCurrentUser;
		MaxUser = this->m_MuLoginServerData[Index].m_wMaxUser;
	}

	void SetUserLimitData(int Index, int CurUser, int MaxUser)
	{
		this->m_MuLoginServerData[Index].m_wCurrentUser = CurUser;
		this->m_MuLoginServerData[Index].m_wMaxUser = MaxUser;
	}

private:

	JOIN_SERVER_SERVER_DATA m_MuLoginServerData[MAX_LOGIN_SERVER];
	int m_MuLoginServerDataCount;

};

class CLoginUserData
{
public:

	CLoginUserData();
	~CLoginUserData();
	friend class CLoginServerProtocol;

	void Init();

	int MuLoginAddUser(WORD ServerCode, WORD ServerGroup, char * szAccountID, int UserIndex, char * szIp, char * szHWID);
	int MuLoginFindUser(char * szAccountID);
	void MuLoginDeleteMapMove(char * szAccountID);
	void MuLoginDeleteUser(char * szAccountID);
	void MuLoginDeleteUser(WORD ServerCode, WORD ServerGroup);
	unsigned long MakeAccountKey(LPSTR lpszAccountID);
	int CheckMoveTimeOut(char * szAccountID);
	int CheckAccountID(LPSTR szAccountID);
	int SetVIPData(LPSTR szAccountID, int VIPType, LPSTR szDate, int Minutes);

	bool CheckHWIDLimit_Group(WORD ServerGroup, LPSTR szHWID);
	bool CheckHWIDLimit_Local(WORD ServerCode, LPSTR szHWID, DWORD HWIDMaxUse);

	CRITICAL_SECTION critUserData;
	std::vector<USER_CONNECT_DATA*> m_vecMapMove;

	void IncUserNumber()
	{
		this->m_iUserNumber++;
	}

	void DecUserNumber()
	{
		this->m_iUserNumber--;
	}

	long GetUserNumber()
	{
		return this->m_iUserNumber;
	}

	LPSTR GetAccountID(int Index)
	{
		return this->m_MuLoginUserData[Index].m_AccoundID;
	}

	LPSTR GetHWID(int Index)
	{
		return this->m_MuLoginUserData[Index].m_HWID;
	}

	LPSTR GetIpAddr(int Index)
	{
		return this->m_MuLoginUserData[Index].IpAddr;
	}

	int GetUserIndex(int Index)
	{
		return this->m_MuLoginUserData[Index].iIndex;
	}

	int GetUserNumber(int Index)
	{
		return this->m_MuLoginUserData[Index].iUserNumber;
	}

	int GetServerCode(int Index)
	{
		return this->m_MuLoginUserData[Index].m_ServerCode;
	}

	bool CheckMapServerMove(int Index)
	{
		return this->m_MuLoginUserData[Index].m_bRequestMapSvrMove;
	}

	void SetMapServerMove(int Index, bool Move)
	{
		this->m_MuLoginUserData[Index].m_bRequestMapSvrMove = Move;
	}

	bool CheckOffTrade(int Index)
	{
		return this->m_MuLoginUserData[Index].m_bOffTrade;
	}

	void SetOffTrade(int Index, bool OffTrade)
	{
		this->m_MuLoginUserData[Index].m_bOffTrade = OffTrade;
	}

	void SetServerCode(int Index, WORD ServerCode)
	{
		this->m_MuLoginUserData[Index].m_ServerCode = ServerCode;
	}

	void SetUserIndex(int Index, int iUserIndex)
	{
		this->m_MuLoginUserData[Index].iIndex = iUserIndex;
	}
	int m_MuLoginUserDataCount;
private:
	JOIN_SERVER_CUserData m_MuLoginUserData[MAX_LOGIN_USER];
	int m_iUserNumber;
};

class CLoginServerProtocol
{
public:

	CLoginServerProtocol();
	~CLoginServerProtocol();
	BOOL Init();
	static void ProtocolCore(CGameObject &lpObj, BYTE HeadCode, LPBYTE aRecv, int iSize);
	BOOL DisconnectPlayer(LPSTR szAccountID);
	void DisconnectServer(WORD ServerCode);
	void CheckVIPTimeProc();

private:

	void JoinServerLogin(CGameObject &lpObj, SDHP_SERVERINFO * lpMsg);
	void JGPAccountRequest(CGameObject &lpObj, SDHP_IDPASS * aRecv);
	void GJPAccountFail(CGameObject &lpObj, SDHP_JOINFAIL * aRecv);
	void GJPAccountBlock(CGameObject &lpObj, SDHP_COMMAND_BLOCK * aRecv);
	void GJPUserClose(CGameObject &lpObj, SDHP_USERCLOSE_ID * aRecv );
	void JGOtherJoin(CGameObject &lpObj, LPSTR szAccountID);
	void GCUserKillSend(int MuLoginIndex, bool IsForceDC);
	void GCJoinBillCheckSend(CGameObject &lpObj, SDHP_BILLSEARCH * aRecv);
	void LoveHeartEventRecv(CGameObject &lpObj, SDHP_LOVEHEARTEVENT * aRecv);
	void LoveHeartCreateSend(CGameObject &lpObj, SDHP_LOVEHEARTCREATE * aRecv);
	void GJReqMapSvrMove(CGameObject &lpObj, PMSG_REQ_MAPSVRMOVE * aRecv);
	void GJReqMapSvrAuth(CGameObject &lpObj, PMSG_REQ_MAPSVRAUTH * aRecv);
	void GJNotifyMaxUserCount(CGameObject &lpObj, PMSG_NOTIFY_MAXUSER * aRecv);
	void GJReqSetOffTrade(CGameObject &lpObj, PMSG_SET_OFFTRADE * aRecv);
	void WJKillUser(CGameObject &lpObj,SDHP_USERCLOSE_ID * aRecv);
	void GJReqVipAdd(CGameObject &lpObj, ISHOP_VIP_BUY *aRecv);
	static unsigned long __stdcall ConnectServerThread(LPVOID);
	void InsertDataMuLog(LPSTR ServerName, LPSTR Id, LPSTR Ip, LPSTR State, LPSTR HWID);

	CRITICAL_SECTION userCheck;
	char m_Salt[30];
	WzUdp m_ConnecServerUDP;

	CQuery m_AccountDB;
	CQuery m_LogDB;
	CQuery m_VIPDB;

	CLoginServerData m_ServerData;
public:
	CLoginUserData   m_UserData;

};

#endif // !defined(AFX_SPROTOCOL_H__5309DA38_D6AD_420B_896B_CB391FCD3624__INCLUDED_)


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

