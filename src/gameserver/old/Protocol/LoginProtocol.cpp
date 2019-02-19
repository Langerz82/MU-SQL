//////////////////////////////////////////////////////////////////////
// protocol.cpp
#include "StdAfx.h"
#include "LoginProtocol.h"
#include "IOCP.h"
#include "ServerData.h"
#include "Main.h"
#include "Utility/util.h"
#include "generalStructs.h"
#include "Query.h"

CLoginServerProtocol m_JSProtocol;

CLoginServerData::CLoginServerData()
{
	this->m_MuLoginServerDataCount = 0;
	memset(this->m_MuLoginServerData, 0x00, sizeof(this->m_MuLoginServerData));
}

CLoginServerData::~CLoginServerData()
{
}

void CLoginServerData::Init()
{
	for (int i = 0; i<MAX_LOGIN_SERVER; i++)
	{
		memset(this->m_MuLoginServerData[i].m_szServerName, 0, sizeof(this->m_MuLoginServerData[i].m_szServerName));
		this->m_MuLoginServerData[i].m_ServerIndex = -1;
		this->m_MuLoginServerData[i].m_wPort = 0;
		this->m_MuLoginServerData[i].m_wServerCode = -1;
		this->m_MuLoginServerData[i].m_VipServer = 0;
		this->m_MuLoginServerData[i].m_MaxHWIDUseCount = 0;
	}
}

int CLoginServerData::MuLoginFindServer(WORD ServerCode)
{
	for (int i = 0; i<MAX_LOGIN_SERVER; i++)
	{
		if (this->m_MuLoginServerData[i].m_wServerCode == ServerCode)
		{
			return i;
		}
	}

	return -1;
}

BOOL CLoginServerData::MuLoginAddServer(int ServerIndex, LPSTR ServerName, WORD ServerCode, WORD Port, BYTE ServerVIP, WORD MaxHWIDUseCount)
{
	if (MuLoginFindServer(ServerCode) != -1)
	{
		sLog->outError("[MeMuOnlineDB] GameServer %s trying to connect with a ServerCode %d already used",
			ServerName, ServerCode);

		return FALSE;
	}

	for (int i = 0; i<MAX_LOGIN_SERVER; i++)
	{
		if (this->m_MuLoginServerData[i].m_szServerName[0] == 0)
		{
			if (this->m_MuLoginServerData[i].m_szServerName[1] == 0)
			{
				this->m_MuLoginServerData[i].m_ServerIndex = ServerIndex;
				this->m_MuLoginServerData[i].m_wPort = Port;
				this->m_MuLoginServerData[i].m_wServerCode = ServerCode;
				this->m_MuLoginServerData[i].m_VipServer = ServerVIP;
				this->m_MuLoginServerData[i].m_MaxHWIDUseCount = MaxHWIDUseCount;
				std::memcpy(this->m_MuLoginServerData[i].m_szServerName, ServerName, 50);
				this->m_MuLoginServerDataCount++;

				return TRUE;
			}
		}
	}

	return FALSE;
}

void CLoginServerData::MuLoginDeleteServer(WORD ServerCode)
{
	for (int i = 0; i<MAX_LOGIN_SERVER; i++)
	{
		if (this->m_MuLoginServerData[i].m_wServerCode == ServerCode)
		{
			memset(this->m_MuLoginServerData[i].m_szServerName, 0, 50);
			this->m_MuLoginServerData[i].m_wPort = 0;
			this->m_MuLoginServerData[i].m_wServerCode = -1;
			this->m_MuLoginServerData[i].m_ServerIndex = -1;
			this->m_MuLoginServerDataCount--;
			break;
		}
	}
}

CLoginUserData::CLoginUserData()
{
	this->m_MuLoginUserDataCount = 0;
	this->m_iUserNumber = 0;
	this->m_vecMapMove.clear();

	memset(this->m_MuLoginUserData, 0x00, sizeof(this->m_MuLoginUserData));
	InitializeCriticalSection(&this->critUserData);
}

CLoginUserData::~CLoginUserData()
{
	DeleteCriticalSection(&this->critUserData);
}

void CLoginUserData::Init()
{
	for (int i = 0; i<MAX_LOGIN_USER; i++)
	{
		memset(this->m_MuLoginUserData[i].m_AccoundID, 0, sizeof(this->m_MuLoginUserData[i].m_AccoundID));
		memset(this->m_MuLoginUserData[i].m_Ip, 0, sizeof(this->m_MuLoginUserData[i].m_Ip));
		this->m_MuLoginUserData[i].iUserNumber = -1;
		this->m_MuLoginUserData[i].m_bRequestMapSvrMove = false;
		this->m_MuLoginUserData[i].m_bOffTrade = false;
		this->m_MuLoginUserData[i].m_ServerCode = -1;
	}
}

DWORD CLoginUserData::MakeAccountKey(LPSTR lpszAccountID)
{
	int len = (int)strlen(lpszAccountID);

	if (len <= 0)
		return 0;

	DWORD dwKey = 0;

	for (int i = 0; i<len; i++)
		dwKey += lpszAccountID[i] + 17;

	dwKey = (dwKey + (10 - len) * 17) % 256;
	return dwKey % 256;
}

BOOL CLoginUserData::MuLoginAddUser(WORD ServerCode, WORD ServerGroup, char * szAccountID, int UserIndex, char * szIp, char * szHWID)
{
	if (this->MuLoginFindUser(szAccountID) != -1)
	{
		sLog->outError("[JoinServer Error] - Account already exists (%s)", szAccountID);
		return 2;
	}

	for (int i = 0; i<MAX_LOGIN_USER; i++)
	{
		if (this->m_MuLoginUserData[i].m_AccoundID[0] == 0)
		{
			if (this->m_MuLoginUserData[i].m_AccoundID[1] == 0)
			{
				std::memcpy(this->m_MuLoginUserData[i].m_AccoundID, szAccountID, 10);
				std::memcpy(this->m_MuLoginUserData[i].m_Ip, szIp, 17);
				std::memcpy(this->m_MuLoginUserData[i].m_HWID, szHWID, 100);
				this->m_MuLoginUserData[i].m_AccoundID[10] = 0;
				this->m_MuLoginUserData[i].iUserNumber = this->m_iUserNumber;
				this->m_MuLoginUserData[i].m_ServerCode = ServerCode;
				this->m_MuLoginUserData[i].m_ServerGroup = ServerGroup;
				this->m_MuLoginUserData[i].m_bRequestMapSvrMove = false;
				this->m_MuLoginUserData[i].iIndex = UserIndex;
				this->m_MuLoginUserData[i].m_bOffTrade = false;
				this->m_iUserNumber++;
				this->m_MuLoginUserDataCount++;
				sLog->outBasic("[JoinServer] Add User (%s) (ServerCode:%d) (ServerGroup:%d) (Index:%d) (IP:%s) (HWID:%s)",
					szAccountID, ServerCode, ServerGroup, UserIndex, szIp, szHWID);
				return TRUE;
			}
		}
	}

	return FALSE;
}

int CLoginUserData::MuLoginFindUser(char * szAccountID)
{
	for (int i = 0; i<MAX_LOGIN_USER; i++)
	{
		if (tolower(this->m_MuLoginUserData[i].m_AccoundID[0]) == tolower(szAccountID[0]))
		{
			if (tolower(this->m_MuLoginUserData[i].m_AccoundID[1]) == tolower(szAccountID[1]))
			{
				if (lstrcmpi(this->m_MuLoginUserData[i].m_AccoundID, szAccountID) == 0)
				{
					return i;
				}
			}
		}
	}

	return -1;
}

void CLoginUserData::MuLoginDeleteMapMove(char * szAccountID)
{
	EnterCriticalSection(&this->critUserData);

	std::vector<USER_CONNECT_DATA*>::iterator it;

	if (!this->m_vecMapMove.empty())
	{
		for (it = this->m_vecMapMove.begin(); it != this->m_vecMapMove.end(); )
		{
			if (tolower((*it)->pMapServerMoveData.szAccountID[0]) == tolower(szAccountID[0]))
			{
				if (tolower((*it)->pMapServerMoveData.szAccountID[1]) == tolower(szAccountID[1]))
				{
					if (lstrcmpi((*it)->pMapServerMoveData.szAccountID, szAccountID) == 0)
					{
						this->m_vecMapMove.erase(it);
						break;
					}
				}
			}

			it++;
		}
	}

	LeaveCriticalSection(&this->critUserData);
}

void CLoginUserData::MuLoginDeleteUser(char * szAccountID)
{
	if (szAccountID == NULL)
		return;

	for (int i = 0; i<MAX_LOGIN_USER; i++)
	{
		if (tolower(this->m_MuLoginUserData[i].m_AccoundID[0]) == tolower(szAccountID[0]))
		{
			if (tolower(this->m_MuLoginUserData[i].m_AccoundID[1]) == tolower(szAccountID[1]))
			{
				if (lstrcmpi(this->m_MuLoginUserData[i].m_AccoundID, szAccountID) == 0)
				{
					MuLoginDeleteMapMove(szAccountID);
					memset(this->m_MuLoginUserData[i].m_AccoundID, 0, 10);
					memset(this->m_MuLoginUserData[i].m_Ip, 0, 17);
					memset(this->m_MuLoginUserData[i].m_HWID, 0, 100);
					this->m_MuLoginUserData[i].m_ServerCode = -1;
					this->m_MuLoginUserData[i].m_ServerGroup = -1;
					this->m_MuLoginUserData[i].m_bRequestMapSvrMove = false;
					this->m_MuLoginUserData[i].m_bOffTrade = false;
					this->m_MuLoginUserData[i].iUserNumber = -1;
					this->m_MuLoginUserDataCount--;
					sLog->outBasic("[JoinServer] Delete User (%s)", szAccountID);
					break;
				}
			}
		}
	}
}

void CLoginUserData::MuLoginDeleteUser(WORD ServerCode, WORD ServerGroup)
{
	for (int n = 0; n < MAX_LOGIN_USER; n++)
	{
		if (this->m_MuLoginUserData[n].m_ServerCode == ServerCode && this->m_MuLoginUserData[n].m_ServerGroup == ServerGroup)
		{
			memset(this->m_MuLoginUserData[n].m_AccoundID, 0x00, sizeof(this->m_MuLoginUserData[n].m_AccoundID));
			memset(this->m_MuLoginUserData[n].m_Ip, 0x00, sizeof(this->m_MuLoginUserData[n].m_Ip));
			this->m_MuLoginUserData[n].iUserNumber = -1;
			this->m_MuLoginUserData[n].m_bRequestMapSvrMove = FALSE;
			this->m_MuLoginUserData[n].m_ServerCode = -1;
			this->m_MuLoginUserData[n].m_ServerGroup = -1;
			sLog->outBasic("[JoinServer] Delete User From Server (Code:%d) (Group:%d)", ServerCode, ServerGroup);
		}
	}
}

BOOL CLoginUserData::CheckMoveTimeOut(char * szAccountID)
{
	int bDelete = FALSE;
	EnterCriticalSection(&this->critUserData);

	std::vector<USER_CONNECT_DATA*>::iterator it;

	if (!this->m_vecMapMove.empty())
	{
		for (it = this->m_vecMapMove.begin(); it != this->m_vecMapMove.end(); it++)
		{
			if (tolower((*it)->pMapServerMoveData.szAccountID[0]) != tolower(szAccountID[0]))
				continue;
			if (tolower((*it)->pMapServerMoveData.szAccountID[1]) != tolower(szAccountID[1]))
				continue;
			if (lstrcmpi((*it)->pMapServerMoveData.szAccountID, szAccountID) != 0)
				continue;

			if (GetTickCount() - (*it)->dwTick < 60000)
				break;

			bDelete = TRUE;
			break;
		}
	}

	LeaveCriticalSection(&this->critUserData);

	if (bDelete == TRUE)
	{
		sLog->outBasic("[JoinServer] Account has timeout for MapServerMove (%s)", szAccountID);
		return m_JSProtocol.DisconnectPlayer(szAccountID);
	}

	return FALSE;
}

int CLoginUserData::CheckAccountID(LPSTR szAccountID)
{
	for (int i = 0; i<MAX_LOGIN_USER; i++)
	{
		if (tolower(this->m_MuLoginUserData[i].m_AccoundID[0]) == tolower(szAccountID[0]))
		{
			if (tolower(this->m_MuLoginUserData[i].m_AccoundID[1]) == tolower(szAccountID[1]))
			{
				if (lstrcmpi(this->m_MuLoginUserData[i].m_AccoundID, szAccountID) == 0)
				{
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

bool CLoginUserData::CheckHWIDLimit_Group(WORD ServerGroup, LPSTR szHWID)
{
	DWORD HWIDCount = 0;

	for (int i = 0; i < MAX_LOGIN_USER; i++)
	{
		if (this->m_MuLoginUserData[i].m_ServerGroup == ServerGroup)
		{
			if (lstrcmpi(this->m_MuLoginUserData[i].m_HWID, szHWID) == 0)
			{
				if (this->m_MuLoginUserData[i].m_bOffTrade == false)
				{
					HWIDCount++;
				}
			}
		}
	}

	if (HWIDCount >= g_MachineIDConnectionLimitPerGroup)
	{
		return FALSE;
	}

	return TRUE;
}

bool CLoginUserData::CheckHWIDLimit_Local(WORD ServerCode, LPSTR szHWID, DWORD HWIDMaxUse)
{
	DWORD HWIDCount = 0;

	for (int i = 0; i < MAX_LOGIN_USER; i++)
	{
		if (this->m_MuLoginUserData[i].m_ServerCode == ServerCode)
		{
			if (lstrcmpi(this->m_MuLoginUserData[i].m_HWID, szHWID) == 0)
			{
				if (this->m_MuLoginUserData[i].m_bOffTrade == false)
				{
					HWIDCount++;
				}
			}
		}
	}

	if (HWIDCount >= HWIDMaxUse)
	{
		return FALSE;
	}

	return TRUE;
}

int CLoginUserData::SetVIPData(LPSTR szAccountID, int VIPType, LPSTR szDate, int Minutes)
{
	int UserIndex = this->MuLoginFindUser(szAccountID);

	if (UserIndex == -1)
	{
		return FALSE;
	}

	if (VIPType == 0)
	{
		this->m_MuLoginUserData[UserIndex].iPayCode = 5;
		this->m_MuLoginUserData[UserIndex].dwPlayTime = 0;
		this->m_MuLoginUserData[UserIndex].iUsedTime = 0;
		this->m_MuLoginUserData[UserIndex].dwCheckTime = GetTickCount();
		memset(this->m_MuLoginUserData[UserIndex].szEndDays, 0x00, sizeof(this->m_MuLoginUserData[UserIndex].szEndDays));

		return TRUE;
	}

	else if (VIPType == 1)
	{
		this->m_MuLoginUserData[UserIndex].iPayCode = 4;
		this->m_MuLoginUserData[UserIndex].dwPlayTime = 0;
		this->m_MuLoginUserData[UserIndex].iUsedTime = 0;
		this->m_MuLoginUserData[UserIndex].dwCheckTime = GetTickCount();

		strncpy(this->m_MuLoginUserData[UserIndex].szEndDays, szDate, 4);
		strncpy(this->m_MuLoginUserData[UserIndex].szEndDays + 4, szDate + 5, 2);
		strncpy(this->m_MuLoginUserData[UserIndex].szEndDays + 6, szDate + 8, 2);

		return TRUE;
	}

	else if (VIPType == 2)
	{
		this->m_MuLoginUserData[UserIndex].iPayCode = 3;
		this->m_MuLoginUserData[UserIndex].dwPlayTime = Minutes;
		this->m_MuLoginUserData[UserIndex].iUsedTime = 0;
		this->m_MuLoginUserData[UserIndex].dwCheckTime = GetTickCount();

		memset(this->m_MuLoginUserData[UserIndex].szEndDays, 0x00, sizeof(this->m_MuLoginUserData[UserIndex].szEndDays));

		return TRUE;
	}

	return FALSE;
}


CLoginServerProtocol::CLoginServerProtocol(): m_AccountDB(), m_LogDB(), m_VIPDB()
{
	InitializeCriticalSection(&this->userCheck);
	memset(this->m_Salt, 0x00, sizeof(this->m_Salt));
}

CLoginServerProtocol::~CLoginServerProtocol()
{
	DeleteCriticalSection(&this->userCheck);
}

void CLoginServerProtocol::InsertDataMuLog(LPSTR ServerName, LPSTR Id, LPSTR Ip, LPSTR State, LPSTR HWID)
{
	this->m_LogDB.ExecQuery("INSERT INTO ConnectionHistory (AccountID, ServerName, IP, `Date`, `State`, HWID) Values('%s','%s','%s', GETDATE(), '%s', '%s')",
		Id, ServerName, Ip, State, HWID);
}

BOOL CLoginServerProtocol::Init()
{
	if (!g_UseJoinServer /*|| g_DSMode != FALSE*/)
	{
		return FALSE;
	}

	GetPrivateProfileString("SQL", "PasswordEncryptSalt", "1234567890", m_Salt, 30, ".\\DataServer.ini");

	this->m_ServerData.Init();
	this->m_UserData.Init();

	sLog->outBasic("[SUCCESS] - JOIN SERVER CONNECT MSSQL SUCCESS");

	return TRUE;
}


void CLoginServerProtocol::ProtocolCore(int userIndex, BYTE HeadCode, LPBYTE aRecv, int iSize)
{
#if (TRACE_PACKET == 1 )
	LogAddHeadHex("JOIN_SERVER", aRecv, iSize);
#endif

	switch (HeadCode)
	{
	case 0x00:
		m_JSProtocol.JoinServerLogin(userIndex, (SDHP_SERVERINFO *)aRecv);
		break;
	case 0x01:
		m_JSProtocol.JGPAccountRequest(userIndex, (SDHP_IDPASS *)aRecv);
		break;
	case 0x02:
		m_JSProtocol.GJPAccountFail(userIndex, (SDHP_JOINFAIL *)aRecv);
		break;
	case 0x04:
		m_JSProtocol.GJPAccountBlock(userIndex, (SDHP_COMMAND_BLOCK *)aRecv);
		break;
	case 0x05:
		m_JSProtocol.GJPUserClose(userIndex, (SDHP_USERCLOSE_ID *)aRecv);
		break;
	case 0x06:
		m_JSProtocol.GCJoinBillCheckSend(userIndex, (SDHP_BILLSEARCH *)aRecv);
		break;
	case 0x30:
		m_JSProtocol.LoveHeartEventRecv(userIndex, (SDHP_LOVEHEARTEVENT *)aRecv);
		break;
	case 0x31:
		m_JSProtocol.LoveHeartCreateSend(userIndex, (SDHP_LOVEHEARTCREATE *)aRecv);
		break;
	case 0x7A:
		//m_JSProtocol.GJReqMapSvrMove(userIndex, (PMSG_REQ_MAPSVRMOVE *)aRecv);
		break;
	case 0x7B:
		//m_JSProtocol.GJReqMapSvrAuth(userIndex, (PMSG_REQ_MAPSVRAUTH *)aRecv);
		break;
	case 0x7C:
		m_JSProtocol.GJNotifyMaxUserCount(userIndex, (PMSG_NOTIFY_MAXUSER *)aRecv);
		break;
	case 0x80:
		m_JSProtocol.GJReqSetOffTrade(userIndex, (PMSG_SET_OFFTRADE *)aRecv);
		break;
	case 0xD3:
		m_JSProtocol.GJReqVipAdd(userIndex, (ISHOP_VIP_BUY *)aRecv);
		break;
	case 0xFF:
		m_JSProtocol.DisconnectServer(userIndex);
		break;
	case 0xFE:
		m_JSProtocol.WJKillUser(userIndex, (SDHP_USERCLOSE_ID*)aRecv);
		break;
		/*case 0xA0:
		DisconnectSpecificUser	// No Reference in GS via packet 0x09
		break;*/
		/*case 0xA1:
		SentNotice to ALL Servers	// No Reference in GS via packet 0x40
		break;*/
		/*case 0xA2:
		RecvNotice Result	// No Reference in GS via packet 0x41
		break;*/
		/*case 0xA3:
		ExitCodeSendUSer // No Reference in GS via packet 0x20
		break;*/
		/*case 0xA4:
		Send Notice to Specific Server // No Reference in GS via packet 0x40
		break;*/
	}
}

void CLoginServerProtocol::JoinServerLogin(int userIndex, SDHP_SERVERINFO * lpMsg)
{
	SDHP_RESULT pResult = { 0 };

	PHeadSetB((BYTE*)&pResult, 0x00, sizeof(pResult));
	pResult.Result = 1;

	if (this->m_ServerData.MuLoginAddServer(userIndex, lpMsg->ServerName, lpMsg->ServerCode, lpMsg->Port, lpMsg->ServerVIP, lpMsg->MaxHWIDUseCount) == TRUE)
	{
		sLog->outBasic("[Join Server] GameServer connected %s PORT : %d CODE : %d VIP : %d HWIDCount : %d",
			lpMsg->ServerName, lpMsg->Port, lpMsg->ServerCode, lpMsg->ServerVIP, lpMsg->MaxHWIDUseCount);

		g_Server[userIndex].m_ServerCode = lpMsg->ServerCode;
	}
	else
	{
		pResult.Result = 0;
	}

	//IOCP.DataSend(userIndex, (BYTE*)&pResult, pResult.h.size, false); TODO - Send to game Server.
}

void CLoginServerProtocol::JGPAccountRequest(int userIndex, SDHP_IDPASS * aRecv)
{
	SDHP_IDPASSRESULT pResult = { 0 };

	// Copy Data
	PHeadSetB((BYTE*)&pResult, 0x01, sizeof(pResult));
	char szAccountID[11] = { 0 };
	char szPass[21] = { 0 };
	char szJoominNumber[21] = { 0 };
	char szIp[16] = { 0 };
	BuxConvert(aRecv->Id, 10);
	BuxConvert(aRecv->Pass, 20);
	std::memcpy(szAccountID, aRecv->Id, 10);
	std::memcpy(szPass, aRecv->Pass, 20);
	std::memcpy(pResult.Id, aRecv->Id, 10);
	std::memcpy(szIp, aRecv->IpAddress, 15);
	pResult.Number = aRecv->Number;

	// Prevent SQLInjection
	if (SQLSyntexCheck(szAccountID) == FALSE ||
		SQLSyntexCheck(szPass) == FALSE)
	{
		pResult.result = 2;
		IOCP.DataSend(userIndex, (BYTE*)&pResult, pResult.h.size, false);
		return;
	}

	if (SpaceSyntexCheck(szAccountID) == FALSE ||
		SpaceSyntexCheck(szPass) == FALSE)
	{
		pResult.result = 2;
		IOCP.DataSend(userIndex, (BYTE*)&pResult, pResult.h.size, false);
		return;
	}

	if (QuoteSpaceSyntaxCheck(szAccountID) == FALSE ||
		QuoteSpaceSyntaxCheck(szPass) == FALSE)
	{
		pResult.result = 2;
		IOCP.DataSend(userIndex, (BYTE*)&pResult, pResult.h.size, false);
		return;
	}

	if (PercentSyntaxCheck(szAccountID) == FALSE ||
		PercentSyntaxCheck(szPass) == FALSE)
	{
		pResult.result = 2;
		IOCP.DataSend(userIndex, (BYTE*)&pResult, pResult.h.size, false);
		return;
	}

	BOOL bErrorFlag = FALSE;
	int BlocCode = 0;

	EnterCriticalSection(&this->userCheck);

	if (SpaceSyntexCheck(szAccountID) == TRUE)
	{
		pResult.result = 1;

		// Add User to List
		if (bErrorFlag == FALSE)
		{
			QueryResult* res = this->m_AccountDB.Fetch("SELECT memb_guid, bloc_code, memb__pwd FROM MEMB_INFO WHERE memb___id='%s'", szAccountID);
			if (res != NULL)
			{
				if ((*res)->GetRowCount() == 0)
				{
					pResult.result = 2;
					bErrorFlag = TRUE;
					sLog->outBasic("[MeMuOnline] Account doesn't exist - ID : %s", szAccountID);
				}

				if (bErrorFlag == FALSE)
				{
					pResult.UserNumber = this->m_UserData.GetUserNumber();
					this->m_UserData.IncUserNumber();
					pResult.DBNumber = this->m_AccountDB.GetAsInteger(0);
					std::memcpy(szJoominNumber, aRecv->Pass, 20);
					std::memcpy(pResult.JoominNumber, szJoominNumber, sizeof(pResult.JoominNumber));;
					BlocCode = this->m_AccountDB.GetAsInteger(1);

					/*
					if (g_PwEncrypt != PWENC_NONE)
					{
						if (g_PwEncrypt == PWENC_WZMD5)
						{

							BYTE btBinaryPass[32] = { 0 };
							char szTemp[100];
							wsprintf(szTemp, "SELECT memb__pwd from MEMB_INFO where memb___id='%s'", szAccountID);

							if (this->m_AccountDB.GetAsBinary(szTemp, btBinaryPass, sizeof(btBinaryPass)) < 0)
							{
								bErrorFlag = TRUE;
								pResult.result = 0;
								sLog->outBasic("[MeMuOnlineDB] Password field is blank - ID : %s", szAccountID);
							}

							DWORD dwAccKey = this->m_UserData.MakeAccountKey(szAccountID);

							
							// TODO
							WZMD5 pMD5Hash;

							if (pMD5Hash.WZMD5_CheckValue(szPass, (char*)btBinaryPass, dwAccKey) == false)
							{
								pResult.result = 0;
								bErrorFlag = TRUE;
								sLog->outBasic("[MeMuOnline] Wrong Password - ID : %s", szAccountID);
							}
						}
						else if (g_PwEncrypt == PWENC_MD5)
						{
							char szTemp[100];
							char szPassword[33] = { 0 };

							wsprintf(szTemp, "SELECT memb__pwd from MEMB_INFO where memb___id='%s'", szAccountID);

							this->m_AccountDB.GetAsString("memb__pwd", szPassword, sizeof(szPassword) - 1);

							if (strlen(szPassword) == 0)
							{
								bErrorFlag = TRUE;
								pResult.result = 0;
								sLog->outBasic("[MeMuOnlineDB] Password field is blank - ID : %s", szAccountID);
							}

							if (md5(std::string(szPass)).compare(std::string(szPassword)) != 0)
							{
								pResult.result = 0;
								bErrorFlag = TRUE;
								sLog->outBasic("[MeMuOnline] Wrong Password - ID : %s", szAccountID);
							}
						}
					}*/
					//else
					//{
						TCHAR szPassword[21] = { 0 };
						this->m_AccountDB.GetAsString(2, szPassword, sizeof(szPassword) - 1);

						if (lstrcmp(szPass, szPassword))
						{
							pResult.result = 0;
							bErrorFlag = TRUE;
							sLog->outBasic("[MeMuOnline] Wrong Password - ID : %s", szAccountID);
						}
					//}
				}
				if (bErrorFlag == FALSE && this->m_ServerData.GetVIPLevel(this->m_ServerData.MuLoginFindServer(g_Server[userIndex].m_ServerCode)) != 0)
				{
					QueryResult* res = this->m_AccountDB.Fetch("EXEC VIPSystem_CheckAccount '%s'", szAccountID);

					int VIP = this->m_AccountDB.GetAsInteger(0);
					int Type = this->m_AccountDB.GetAsInteger(1);

					if (VIP == 0)
					{
						pResult.result = 64;
						bErrorFlag = TRUE;
						sLog->outBasic("[MeMuOnline] Account is not a VIP - ID : %s", szAccountID);
					}

					else if (Type < this->m_ServerData.GetVIPLevel(this->m_ServerData.MuLoginFindServer(g_Server[userIndex].m_ServerCode)))
					{
						pResult.result = 64;
						bErrorFlag = TRUE;
						sLog->outBasic("[MeMuOnline] Account has too low VIP level - ID : %s", szAccountID);
					}

				}

				if (bErrorFlag == FALSE)
				{
					QueryResult* res = this->m_AccountDB.Fetch("SELECT HWID FROM IGC_MachineID_Banned WHERE HWID = '%s'", aRecv->HWID);

					if ((*res)->GetRowCount() == 0)
					{
						pResult.result = 5;
						bErrorFlag = TRUE;
						sLog->outBasic("[MeMuOnline] MachineID banned: %s", aRecv->HWID);
					}

				}
			}

		}

		if (BlocCode > 0)
		{
			bErrorFlag = TRUE;

			if (BlocCode == 0x46)
				pResult.result = 15;
			else if (BlocCode == 0x41)
				pResult.result = 14;
			else if (BlocCode == 0x49)
				pResult.result = 17;
			else
				pResult.result = 5;
		}

		if (bErrorFlag == FALSE && pResult.result != 0)
		{
			int Index = this->m_UserData.MuLoginFindUser(szAccountID);

			if (Index != -1)
			{
				if (this->m_UserData.CheckOffTrade(Index) == true)
				{
					this->GCUserKillSend(Index, true);
				}
			}

			if (this->m_UserData.CheckAccountID(szAccountID) == TRUE && this->m_UserData.CheckMoveTimeOut(szAccountID) == FALSE)
			{
				sLog->outBasic("[MeMuOnline] Account already connected ID : %s", szAccountID);
				JGOtherJoin(userIndex, szAccountID);
				pResult.result = 3;
				bErrorFlag = TRUE;
			}

			if (this->m_UserData.CheckHWIDLimit_Local(g_Server[userIndex].m_ServerCode, aRecv->HWID, this->m_ServerData.GetHWIDUseCount(this->m_ServerData.MuLoginFindServer(g_Server[userIndex].m_ServerCode))) == FALSE)
			{
				sLog->outBasic("[MeMuOnline] Machine ID Limit Reached (Local) (%d) ServerCode (%d) : Account:%s HWID:%s", this->m_ServerData.GetHWIDUseCount(this->m_ServerData.MuLoginFindServer(g_Server[userIndex].m_ServerCode)), g_Server[userIndex].m_ServerCode, szAccountID, aRecv->HWID);
				JGOtherJoin(userIndex, szAccountID);
				pResult.result = 4;
				bErrorFlag = TRUE;
			}

			/*
			// TODO
			if (this->m_UserData.CheckHWIDLimit_Group(g_MapServerManager.GetMapSvrGroup(g_Server[userIndex].m_ServerCode), aRecv->HWID) == FALSE)
			{
				sLog->outBasic("[MeMuOnline] Machine ID Limit Reached (Group) (%d) MapSvrGroup (%d) : Account:%s HWID:%s", g_MachineIDConnectionLimitPerGroup, g_MapServerManager.GetMapSvrGroup(g_Server[userIndex].m_ServerCode), szAccountID, aRecv->HWID);
				JGOtherJoin(userIndex, szAccountID);
				pResult.result = 4;
				bErrorFlag = TRUE;
			}*/
		}

		if (bErrorFlag == FALSE)
		{
			BOOL bResult = this->m_UserData.MuLoginAddUser(g_Server[userIndex].m_ServerCode, /*g_MapServerManager.GetMapSvrGroup(g_Server[userIndex].m_ServerCode)*/ 0, szAccountID, aRecv->Number, aRecv->IpAddress, aRecv->HWID);

			if (bResult == FALSE)
			{
				pResult.result = 4;
			}

			else if (bResult == 2)
			{
				pResult.result = 3;
			}

			else
			{
				InsertDataMuLog(this->m_ServerData.GetServerName(this->m_ServerData.MuLoginFindServer(g_Server[userIndex].m_ServerCode)), szAccountID, aRecv->IpAddress, "Connect", aRecv->HWID);
				this->m_AccountDB.ExecQuery("EXEC WZ_CONNECT_MEMB '%s','%s','%s'",
					szAccountID, this->m_ServerData.GetServerName(this->m_ServerData.MuLoginFindServer(g_Server[userIndex].m_ServerCode)), szIp);
			}
		}
	}

	LeaveCriticalSection(&userCheck);

IOCP.DataSend(userIndex, (BYTE*)&pResult, pResult.h.size, __FUNCTION__);
}

void CLoginServerProtocol::GJPAccountFail(int userIndex, SDHP_JOINFAIL * aRecv)
{
	char szAccountID[11] = { 0 };
	std::memcpy(szAccountID, aRecv->Id, 10);
	int UserIndex = this->m_UserData.MuLoginFindUser(szAccountID);
	this->InsertDataMuLog(this->m_ServerData.GetServerName(this->m_ServerData.MuLoginFindServer(g_Server[userIndex].m_ServerCode)), szAccountID, this->m_UserData.GetIpAddr(UserIndex), "Login Fail", "N/A");
	this->m_UserData.MuLoginDeleteUser(szAccountID);
	this->m_AccountDB.ExecQuery("EXEC WZ_DISCONNECT_MEMB '%s'", szAccountID);
}


void CLoginServerProtocol::GJPAccountBlock(int userIndex, SDHP_COMMAND_BLOCK * aRecv)
{
	char szAccountID[11] = { 0 };
	std::memcpy(szAccountID, aRecv->Id, 10);
	this->m_UserData.MuLoginDeleteUser(szAccountID);
	this->m_AccountDB.ExecQuery("UPDATE MEMB_INFO SET bloc_code = 1 WHERE memb___id = '%s'", szAccountID);
}

void CLoginServerProtocol::GJPUserClose(int userIndex, SDHP_USERCLOSE_ID * aRecv)
{
	char szAccountID[11] = { 0 };
	std::memcpy(szAccountID, aRecv->szId, 10);
	this->InsertDataMuLog(this->m_ServerData.GetServerName(this->m_ServerData.MuLoginFindServer(g_Server[userIndex].m_ServerCode)), szAccountID, this->m_UserData.GetIpAddr(this->m_UserData.MuLoginFindUser(szAccountID)), "Disconnect", "N/A");
	this->m_UserData.MuLoginDeleteUser(szAccountID);
	this->m_AccountDB.ExecQuery("EXEC WZ_DISCONNECT_MEMB '%s'", szAccountID);
}

void CLoginServerProtocol::GCUserKillSend(int MuLoginIndex, bool IsForceDC)
{
	SDHP_BILLKILLUSER pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x07;
	pMsg.h.size = sizeof(pMsg);

	std::memcpy(pMsg.Id, this->m_UserData.GetAccountID(MuLoginIndex), 10);
	pMsg.Number = this->m_UserData.GetUserIndex(MuLoginIndex);

	int userIndex = this->m_ServerData.GetServerIndex(this->m_ServerData.MuLoginFindServer(this->m_UserData.GetServerCode(MuLoginIndex)));

	if (userIndex < 0 || userIndex > g_dwMaxServerGroups)
	{
		return;
	}

	pMsg.ForceDisconnect = IsForceDC;

IOCP.DataSend(userIndex, (BYTE*)&pMsg, pMsg.h.size, __FUNCTION__);

}

void CLoginServerProtocol::GCJoinBillCheckSend(int userIndex, SDHP_BILLSEARCH * aRecv)
{
	SDHP_BILLSEARCH_RESULT pResult;

	PHeadSetB((BYTE*)&pResult, 0x06, sizeof(pResult));
	char szAccountID[11] = { 0 };
	std::memcpy(szAccountID, aRecv->Id, 10);
	char szDate[20] = { 0 };

	int VIPType = 0;
	int UserIndex = -1;

	pResult.PayCode = 5;
	pResult.cCertifyType = 0;
	pResult.cVipType = 0;
	pResult.EndTime = 0;
	memset(pResult.EndsDays, 0x00, sizeof(pResult.EndsDays));

	QueryResult* res = this->m_AccountDB.Fetch("EXEC VIPSystem_CheckAccount '%s'", szAccountID);

	if ((*res)->GetRowCount() == 1)
	{
		VIPType = this->m_AccountDB.GetAsInteger(0);
		pResult.cVipType = this->m_AccountDB.GetAsInteger(1);
		UserIndex = this->m_UserData.MuLoginFindUser(szAccountID);
	}

	if (VIPType == 1)
	{
		pResult.PayCode = 4;
		pResult.cCertifyType = 0;

		this->m_AccountDB.GetAsString(3, szDate, 20);

		strncpy(pResult.EndsDays, szDate, 4);
		strncpy(pResult.EndsDays + 4, szDate + 5, 2);
		strncpy(pResult.EndsDays + 6, szDate + 8, 2);

		this->m_UserData.SetVIPData(szAccountID, VIPType, szDate, 0);
	}

	else if (VIPType == 2)
	{
		int Minute = this->m_AccountDB.GetAsInteger(4);

		pResult.PayCode = 3;
		pResult.cCertifyType = 0;
		pResult.EndTime = Minute;

		this->m_UserData.SetVIPData(szAccountID, VIPType, NULL, Minute);
	}

	else
	{
		pResult.PayCode = 5;
		pResult.cCertifyType = 0;
		pResult.cVipType = 0;

		this->m_UserData.SetVIPData(szAccountID, VIPType, NULL, 0);
	}

	pResult.Number = aRecv->Number;
	std::memcpy(pResult.Id, szAccountID, 10);

IOCP.DataSend(userIndex, (BYTE*)&pResult, pResult.h.size, __FUNCTION__);
}

void CLoginServerProtocol::JGOtherJoin(int userIndex, LPSTR szAccountID)
{
	SDHP_OTHERJOINMSG pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x08, sizeof(pMsg));
	std::memcpy(pMsg.AccountID, szAccountID, 10);

IOCP.DataSend(userIndex, (BYTE*)&pMsg, pMsg.h.size, __FUNCTION__);
}


void CLoginServerProtocol::LoveHeartEventRecv(int userIndex, SDHP_LOVEHEARTEVENT * aRecv)
{
	char szAccountID[11] = { 0 };
	char szName[11] = { 0 };
	std::memcpy(szAccountID, aRecv->Account, 10);
	std::memcpy(szName, aRecv->Name, 10);
	DWORD dwHeartCount = 0;

	SDHP_LOVEHEARTEVENT_RESULT pResult;

	PHeadSetB((BYTE*)&pResult, 0x30, sizeof(pResult));
	pResult.Result = 0;
	std::memcpy(pResult.Name, aRecv->Name, 10);

	if (this->m_AccountDB.ExecQuery("SELECT heart_count FROM LoveHeartCount WHERE Number=0") == TRUE)
	{
		if (this->m_AccountDB.Fetch() == 0)
		{
			this->m_AccountDB.ExecQuery("UPDATE LoveHeartCount SET heart_count=0 WHERE Number=0");
		}
		else
		{
			dwHeartCount = this->m_AccountDB.GetAsInteger(0);
		}
	}

	dwHeartCount++;

	if (dwHeartCount > 217000)
	{
		this->m_AccountDB.ExecQuery("UPDATE LoveHeartCount SET heart_count=0 WHERE Number=0");
	}

	this->m_AccountDB.ExecQuery("UPDATE LoveHeartCount SET heart_count= %d where Number=0", dwHeartCount);

	this->m_AccountDB.ExecQuery("INSERT INTO LoveHeartAll ( Number, Id, Server, Name ) Values (%d, '%s','%s','%s')",
		dwHeartCount, szAccountID, this->m_ServerData.GetServerName(this->m_ServerData.MuLoginFindServer(g_Server[userIndex].m_ServerCode)), szName);

	int iIndex = this->m_UserData.MuLoginFindUser(szAccountID);

	if (iIndex >= 0)	// if Is Connected
	{
		if ((rand() % 217000) == iIndex)	// if Wins
		{
			this->m_AccountDB.ExecQuery("INSERT INTO LoveHeart ( Number, Id, Server, Name ) Values (%d, '%s','%s','%s')",
				dwHeartCount, szAccountID, this->m_ServerData.GetServerName(this->m_ServerData.MuLoginFindServer(g_Server[userIndex].m_ServerCode)), szName);
			pResult.Result = 1;

			sLog->outError("[MeMuOnlineDB] [LOVE HEART] Event Winner : %s:%s - Server : %s",
				szAccountID, szName, this->m_ServerData.GetServerName(this->m_ServerData.MuLoginFindServer(g_Server[userIndex].m_ServerCode)));
		}
	}

	pResult.Number = dwHeartCount;
IOCP.DataSend(userIndex, (BYTE*)&pResult, pResult.h.size, __FUNCTION__);
}

void CLoginServerProtocol::LoveHeartCreateSend(int sIndex, SDHP_LOVEHEARTCREATE * aRecv)
{
	this->m_AccountDB.ExecQuery("UPDATE LoveHeartCount SET heart_count=heart_count+1  where Number=1");
}

/*
void CLoginServerProtocol::GJReqMapSvrAuth(int userIndex, PMSG_REQ_MAPSVRAUTH * aRecv)
{
	char szAccoundID[11] = { 0 };
	DWORD fResult = 0;
	std::memcpy(szAccoundID, aRecv->szAccountID, 10);

	if (lstrlen(szAccoundID) < 1)
		fResult = 1;

	PMSG_ANS_MAPSVRAUTH pResult = { 0 };

	PHeadSetB((BYTE*)&pResult, 0x7B, sizeof(pResult));
	std::memcpy(pResult.szAccountID, aRecv->szAccountID, 10);
	std::memcpy(pResult.szCharName, aRecv->szCharName, 10);
	pResult.iIndex = aRecv->iIndex;

	if (fResult == 0)
	{
		int userIndex = this->m_UserData.MuLoginFindUser(szAccoundID);

		// User Not Connected
		if (iUserIndex == -1)
		{
			fResult = 1;
		}

		else if (this->m_UserData.CheckMapServerMove(iUserIndex) == true)
		{
			tagSTR_CUserData pDataUser;
			BOOL bFlagFound = FALSE;

			EnterCriticalSection(&this->m_UserData.critUserData);

			for (std::vector<tagSTR_CUserData>::iterator _Iter = this->m_UserData.m_vecMapMove.begin(); _Iter != this->m_UserData.m_vecMapMove.end(); _Iter++)
			{
				pDataUser = tagSTR_CUserData(*_Iter);

				if (pDataUser.pMapServerMoveData.szAccountID[0] == aRecv->szAccountID[0])
				{
					if (pDataUser.pMapServerMoveData.szAccountID[1] == aRecv->szAccountID[1])
					{
						if (strncmp(pDataUser.pMapServerMoveData.szAccountID, aRecv->szAccountID, 10) == 0)
						{
							if (strncmp(pDataUser.pMapServerMoveData.szCharName, aRecv->szCharName, 10) == 0)
							{
								bFlagFound = TRUE;
								this->m_UserData.m_vecMapMove.erase(_Iter);
								break;
							}
						}
					}
				}
			}

			LeaveCriticalSection(&this->m_UserData.critUserData);

			if (bFlagFound == TRUE)
			{
				if (CHECK_LIMIT(pDataUser.pMapServerMoveData.wCurMapSvrCode, MAX_LOGIN_SERVER) &&
					CHECK_LIMIT(pDataUser.pMapServerMoveData.wDstMapSvrCode, MAX_LOGIN_SERVER))
				{
					if (aRecv->wDstMapSvrCode == pDataUser.pMapServerMoveData.wDstMapSvrCode)
					{
						if (aRecv->iJoinAuthCode1 == pDataUser.pMapServerMoveData.iJoinAuthCode1 &&
							aRecv->iJoinAuthCode2 == pDataUser.pMapServerMoveData.iJoinAuthCode2 &&
							aRecv->iJoinAuthCode3 == pDataUser.pMapServerMoveData.iJoinAuthCode3 &&
							aRecv->iJoinAuthCode4 == pDataUser.pMapServerMoveData.iJoinAuthCode4)
						{
							this->m_UserData.SetMapServerMove(iUserIndex, false);
							this->m_UserData.SetServerCode(iUserIndex, pDataUser.pMapServerMoveData.wDstMapSvrCode);
							this->m_UserData.SetUserIndex(iUserIndex, aRecv->iIndex);
							pResult.btX = pDataUser.pMapServerMoveData.btX;
							pResult.btY = pDataUser.pMapServerMoveData.btY;
							pResult.wPrevMapSvrCode = pDataUser.pMapServerMoveData.wCurMapSvrCode;
							pResult.wMapNumber = pDataUser.pMapServerMoveData.wMapNumber;
							pResult.btSecurityLock = pDataUser.btSecurityLock;
							pResult.dwSecurityCode = pDataUser.dwSecurityCode;

							this->m_AccountDB.ExecQuery("SELECT * FROM MEMB_INFO WHERE memb___id='%s'", szAccoundID);

							if (this->m_AccountDB.Fetch() != SQL_NO_DATA)
							{
								pResult.iUserNumber = pDataUser.iUserNumber;
								std::memcpy(pResult.szPassword, pDataUser.szPassword, sizeof(pResult.szPassword));
								pResult.iDBNumber = this->m_AccountDB.GetAsInteger("memb_guid");
								pResult.btBlockCode = (BYTE)this->m_AccountDB.GetAsInteger("bloc_code");
							}

							this->m_AccountDB.Close();
						}

						else
						{
							fResult = 2;
						}
					}

					else
					{
						fResult = 3;
					}
				}
				else
				{
					fResult = 3;
				}
			}
			else
			{
				fResult = 4;
			}
		}
		else
		{
			fResult = 4;
		}
	}

	if (fResult != 0)
		this->m_UserData.MuLoginDeleteUser(szAccoundID);

	pResult.iResult = fResult;

IOCP.DataSend(userIndex, (BYTE*)&pResult, pResult.h.size, __FUNCTION__);

	sLog->outBasic("[JoinServer] GJReqMapSvrAuth() -> User(%s) Character(%s) fResult(%d)",
		pResult.szAccountID, pResult.szCharName, pResult.iResult);
}
*/

void CLoginServerProtocol::GJNotifyMaxUserCount(int sIndex, PMSG_NOTIFY_MAXUSER * aRecv)
{
	int iIndex = this->m_ServerData.MuLoginFindServer(aRecv->iSvrCode);

	if (iIndex != -1)
	{
		this->m_ServerData.SetUserLimitData(iIndex, aRecv->iCurUserCount, aRecv->iMaxUserCount);
	}
}

BOOL CLoginServerProtocol::DisconnectPlayer(LPSTR szAccountID)
{
	int userIndex = this->m_UserData.MuLoginFindUser(szAccountID);

	if (userIndex == -1)
	{
		return FALSE;
	}

	this->GCUserKillSend(userIndex, true);
	this->m_UserData.MuLoginDeleteUser(szAccountID);

	return TRUE;
}

void CLoginServerProtocol::DisconnectServer(WORD ServerCode)
{
	this->m_ServerData.MuLoginDeleteServer(ServerCode);
	this->m_UserData.MuLoginDeleteUser(ServerCode, /*g_MapServerManager.GetMapSvrGroup(ServerCode)*/ 0);
}

void CLoginServerProtocol::WJKillUser(int sIndex, SDHP_USERCLOSE_ID* aRecv)
{
	char szAccountID[11] = { 0 };
	std::memcpy(szAccountID, aRecv->szId, 10);
	this->DisconnectPlayer(szAccountID);
	this->m_AccountDB.ExecQuery("EXEC WZ_DISCONNECT_MEMB '%s'", szAccountID);
}

void CLoginServerProtocol::CheckVIPTimeProc()
{
	int iResult = 0;

	for (int i = 0; i < MAX_LOGIN_USER; i++)
	{
		if (this->m_UserData.m_MuLoginUserData[i].m_AccoundID[0] != 0)
		{
			if (this->m_UserData.m_MuLoginUserData[i].m_AccoundID[1] != 0)
			{
				if (this->m_UserData.m_MuLoginUserData[i].iUserNumber != -1)
				{
					if (GetTickCount() - this->m_UserData.m_MuLoginUserData[i].dwCheckTime >= 300000)
					{
						switch (this->m_UserData.m_MuLoginUserData[i].iPayCode)
						{
						case 5: // Free
							this->m_UserData.m_MuLoginUserData[i].dwCheckTime = GetTickCount();
							break;
						case 3: // VIP by minutes
						case 4: // VIP by date
						{
							this->m_VIPDB.Fetch("EXEC VIPSystem_CheckAccount '%s'", this->m_UserData.m_MuLoginUserData[i].m_AccoundID);
							//this->m_VIPDB.Fetch();

							int VIP = this->m_VIPDB.GetAsInteger(0);
							int Type = this->m_VIPDB.GetAsInteger(1);

							int userIndex = this->m_ServerData.MuLoginFindServer(this->m_UserData.GetServerCode(i));

							if (VIP == 0)
							{
								this->GCUserKillSend(i, false);
								sLog->outBasic("[VIP] Account expired - ID : %s", this->m_UserData.m_MuLoginUserData[i].m_AccoundID);
							}

							else if (Type < this->m_ServerData.GetVIPLevel(this->m_ServerData.MuLoginFindServer(g_Server[userIndex].m_ServerCode)))
							{
								this->GCUserKillSend(i, false);
								sLog->outBasic("[VIP] Account expired - ID : %s", this->m_UserData.m_MuLoginUserData[i].m_AccoundID);
							}

							else
							{
								this->m_UserData.m_MuLoginUserData[i].dwCheckTime = GetTickCount();
							}
						}
						break;
						}
					}
				}
			}
		}
	}
}

void CLoginServerProtocol::GJReqSetOffTrade(int sIndex, PMSG_SET_OFFTRADE * aRecv)
{
	int userIndex = this->m_UserData.MuLoginFindUser(aRecv->szAccountID);

	if (userIndex == -1)
	{
		return;
	}

	this->m_UserData.SetOffTrade(userIndex, aRecv->m_bState);
	sLog->outBasic("[JoinServer] (%s)(%s) Set OffTrade State: %d", aRecv->szAccountID, aRecv->szName, aRecv->m_bState);
}

void CLoginServerProtocol::GJReqVipAdd(int sIndex, ISHOP_VIP_BUY *aRecv)
{
	this->m_AccountDB.ExecQuery("EXEC IGC_VipAdd '%s', %d, %d", aRecv->AccountID, aRecv->Days, aRecv->Type);
}
