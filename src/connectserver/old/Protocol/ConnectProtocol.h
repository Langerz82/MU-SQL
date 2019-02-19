#ifndef _MU_CONNECTPROTOCOL_H_
#define _MU_CONNECTPROTOCOL_H_

#include "StdAfx.h"

#include "IOCP.h"
#include "ServerData.h"
#include "Main.h"
#include "ConnectEngine.h"
#include "generalStructs.h"

#define MAX_LOGIN_USER	(45000)
#define MAX_LOGIN_SERVER (10)

enum PASSWORD_ENCRYPT_TYPES
{
	PWENC_NONE = 0,
	PWENC_WZMD5 = 1,
	PWENC_MD5 = 2,
	PWENC_SHA2 = 3
};

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
	BOOL MuLoginAddServer(int ServerIndex, LPSTR ServerName, WORD ServerCode, WORD Port, BYTE ServerVIP, WORD MaxHWIDUseCount);

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

void CSProtocolCore(BYTE protoNum, BYTE *aRecv, int aLen, int uIndex, bool Encrypt, int serial);
void UDPProtocolCore(BYTE hCode, LPBYTE aRecv, int aLen);
void UDPSetServerInfo(PMSG_SERVERINFO * aRecv);
void SCSendServerList(int uIndex);
void SCSendServerInfo(int uIndex, PMSG_SERVER_SELECT * aRecv);
void SCConnectResultSend(STR_CS_USER &Obj, BYTE btResult);
void SCSendNews(STR_CS_USER &Obj);
void SCSendAutoUpdateData(int uIndex, PMSG_CLIENTVERSION * aRecv);

#endif
