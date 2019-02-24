#ifndef _GENERALSTRUCTS_H
#define _GENERALSTRUCTS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "GensSystemProtocol.h"
//#include "ServerEngine.h"

#include <string>
#include <map>
#include <ace/SOCK_Stream.h>

class ACE_SOCK_Stream;

struct PER_IO_CONTEXT_L;
struct _PER_SOCKET_CONTEXT;

/* * * * * * * * * * * * * * * * * * * * *
*	Mu JoinServer Login
*	Direction : GameServer -> JoinServer
*				GameServer -> DataServer
*				GameServer -> ExDataServer
*  Code : 0xC1
*	HeadCode : 0x00
*/

// Fwd Declaration.
struct PMSG_CONNECT_INFO
{
	PBMSG_HEAD		h;
	UCHAR			SubHead;
	char			IP[16];
	USHORT			Port;
};


struct PMSG_SERVERSLIST_COUNT
{
	PWMSG_HEAD2		h;
	BYTE			CountH;
	BYTE			CountL;
};

struct PMSG_SERVERLIST_SERVER
{
	WORD			wServerCode;
	BYTE			btPercent;
	BYTE			btPlayType;
};

struct PMSG_SERVER_SELECT
{
	PBMSG_HEAD		h;
	WORD			ServerCode;
};

struct PMSG_SEND_NEWS
{
	PWMSG_HEAD2 h;
	BYTE Day;
	BYTE Month;
	WORD year;

	COLORREF dateColor;
	COLORREF titleColor;
	COLORREF textColor;
	char title[40];
	WORD textLen;
	//char Text[2048];
};


struct PMSG_SEND_TITLE
{
	PBMSG_HEAD h;
	char ServerName[12];
};



struct PMSG_RESULT
{
	PBMSG_HEAD		h;
	UCHAR			result;
};

struct PMSG_SERVERINFO
{
	PBMSG_HEAD h;	// C1:01
	short ServerCode;	// 4
	BYTE Percent;	// 6
	BYTE PlayType;
	short UserCount;	// 8
	short AccountCount;	// A
	short MaxUserCount;	// E
};


struct PMSG_AUTOUPDATE
{
	PBMSG_HEAD h;
	BYTE MainVersion;
	BYTE HeadVersion;
	BYTE SubVersion;
	char HostURL[100];
	WORD FTPPort;
	char FTPLogin[20];
	char FTPPassword[20];
	char VersionFile[20];
};


struct PMSG_CLIENTVERSION
{
	PBMSG_HEAD h;
	char ClientMainVersion;
	char ClientHeadVersion;
	char ClientSubVersion;
};

struct PMSG_VERSIONOK
{
	PBMSG_HEAD h;
	char VersionOK;
};

struct PMSG_LAUNCHER
{
	PWMSG_HEAD h;
	char FTP[200];
	char HASH[200];
};

struct PMSG_LAUNCHERNOLICENSE
{
	PBMSG_HEAD h;
};

struct PMSG_LAUNCHERERROR
{
	PBMSG_HEAD h;
	BYTE ERRORCODE;
};

struct STR_CS_USER;
struct STR_CS_USER
{
	STR_CS_USER() {}
	int Index;
	int ConnectionState;
	char IP[20];
	unsigned int Port;
	BYTE Type;
	HANDLE handle;
	struct _PER_SOCKET_CONTEXT* PerSocketContext;
	bool News;
	int PacketCount;
	ULONGLONG i64PacketTime;
	const char* SocketKey;
	class ACE_SOCK_Stream* Socket;
	BYTE ServerPhase;

	// Merged from GameObject/UserData.
	DWORD CheckTick;	// 3C
	DWORD CheckTick2;	// 44
	BYTE CheckTickCount;	// 48
	DWORD ConnectCheckTime;	// 38
	BYTE CheckSpeedHack;	// 40
	int Connected;	// 4
	char LoginMsgSnd;	// 8
	char LoginMsgCount;	// 9
	char CloseCount;	// A
	char CloseType;	// B
	DWORD AgiCheckTime;
	char  AccountID[11];	// 68
	char  Password[20];	// 68
	char HWID[100];
	BYTE  m_cAccountItemBlock; // 1D0
	BYTE VipType;
	char VipEffect;
	DWORD AntiHackCheckTime;
	DWORD64 dwLastHitHackTick;
	bool m_bMapSvrMoveQuit;	// 1518
	bool m_bMapSvrMoveReq;	// 1519
	bool m_bMapSvrMoveReq_2;
	DWORD m_dwMapSvrQuitTick;	// 151C
	short m_sPrevMapSvrCode;	// 1520
	short m_sDestMapNumber;	// 1522
	BYTE m_btDestX;	// 1524
	BYTE m_btDestY;	// 1525
	BYTE m_ClientHackLogCount;	// F43
	bool m_bOff;
	bool EnableCharacterCreate;

};

struct SERVER_INFO
{
	char Name[50];
	char IP[16];
	WORD Port;
	WORD Code;
	bool Visible;
	WORD UserCount;
	WORD MaxUserCount;
	WORD Percent;
	DWORD TickCount;
	BYTE PlayType;
	bool Launcher;
	char FTPAddress[200];
	char HashCode[200];

	SERVER_INFO()
	{
		this->Port = 0;
		this->Code = 0;
		memset(IP, 0x00, sizeof(IP));
		memset(Name, 0x00, sizeof(Name));
		this->Visible = false;
		this->UserCount = 0;
		this->MaxUserCount = 0;
		this->Percent = 0;
		this->TickCount = 0;
		this->PlayType = 0;
	}
};

struct NEWS_INFO
{
	BYTE day;
	BYTE month;
	WORD year;

	// text color etc
	COLORREF dateColor;
	COLORREF titleColor;
	COLORREF textColor;

	char title[40];

	// text lenght
	WORD textLen;
	char Text[2048];
	bool Used;

	NEWS_INFO()
	{
		Used = false;
	}
	// rest of packet is text
	// lenght of packet is: header + structure + text
};

struct MU_WSAOVERLAPPED {
	DWORD    Internal;
	DWORD    InternalHigh;
	DWORD    Offset;
	DWORD    OffsetHigh;
	DWORD	 hEvent;
};

struct MU_WSABUF {
	ULONG len;
	CHAR* buf;
};

struct _PER_IO_CONTEXT
{
	//MU_WSAOVERLAPPED m_Overlapped; // 0
	//MU_WSABUF m_wsabuf;
	BYTE Buffer[MAX_IO_BUFFER_SIZE]; // 1C
	//BYTE BufferOut[MAX_IO_BUFFER_SIZE]; // 178C
	int nSecondOfs; // 2EFC
	int nTotalBytes;	// 2F00
	int nbBytes; // 2F04
	int IOOperation; // 2F08
	int nWaitIO; // 2F0C

};

struct _PER_SOCKET_CONTEXT
{
	SOCKET m_socket;	// 0
	int nIndex;	// 4
	_PER_IO_CONTEXT IOContext[2];	// 8
	int dwIOCount;	// 5E28
};

struct PMSG_ANS_MAPSVRMOVE {
	PBMSG_HEAD h;
	int iIndex;
	char szAccountID[11];
	char szCharName[11];
	WORD wCurMapSvrCode;
	WORD wDstMapSvrCode;
	WORD wMapNumber;
	BYTE btX;
	BYTE btY;
	int iResult;
	int iJoinAuthCode1;
	int iJoinAuthCode2;
	int iJoinAuthCode3;
	int iJoinAuthCode4;
};

struct USER_CONNECT_DATA
{
	PMSG_ANS_MAPSVRMOVE pMapServerMoveData;
	int iUserNumber;
	DWORD dwTick;
	BYTE btSecurityLock;
	DWORD dwSecurityCode;
	char szPassword[20];
};

struct JOIN_SERVER_CUserData {
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

struct PER_IO_CONTEXT_L
{
	MU_WSAOVERLAPPED			m_Overlapped;
	MU_WSABUF					m_wsabuf;
	CHAR						Buffer[MAX_BUFF_SIZE];
	int							nTotalBytes;
	int							nSentBytes;
	int							IOOperation;
	void*						ST_LISTNODE;
};

struct ST_LISTNODE
{
	ST_LISTNODE*	        pUpLink;
	ST_LISTNODE*	        pDownLink;
	BYTE *		pObject;
	unsigned int		nSize;
	BOOL				bSending;
	int					nOfs;
	BYTE				headcode;
	int					uindex;
	int					iSessionId; //new
	PER_IO_CONTEXT_L	IoCtxt; //re-enabled old one :)
};

struct SDHP_IDPASS {
	PBMSG_HEAD2 h;
	char Id[10];
	char Pass[20];
	short Number;
	char HWID[100];
	char IpAddress[17];
};

struct SDHP_IDPASSRESULT {
	PBMSG_HEAD2 h;
	BYTE result;
	short Number;
	char Id[10];
	int UserNumber;
	int DBNumber;
	char JoominNumber[20];
};

struct IPList
{
	char m_IP[16];
};

struct STR_SERVER_ENGINE
{
	int m_Index;
	SOCKET m_Socket;
	eSERVER_STATE m_State;
	eSERVER_TYPE m_Type;
	char m_ServerIp[16];
	struct _PER_SOCKET_CONTEXT * PerSocketContext;
	WORD m_ServerCode;
	WORD m_ServerGroup;
	WORD m_ConnectPort;
	BOOL newsSent;
};

struct SDHP_JOINFAIL {
	PBMSG_HEAD2 h;
	short Number;
	char Id[11];
	int UserNumber;
	int DBNumber;
};

struct SDHP_BILLSEARCH_RESULT {
	PBMSG_HEAD2 h;
	char Id[10];
	short Number;
	BYTE cCertifyType;
	BYTE PayCode;
	char EndsDays[12];
	int EndTime;
	BYTE cVipType;
};

struct SDHP_BILLSEARCH {
	PBMSG_HEAD2 h;
	char Id[10];
	short Number;
};




/* * * * * * * * * * * * * * * * * * * * *
*	Packet User Close by Id
*	Direction : GameServer -> JoinServer
*  Code : 0xC1
*	HeadCode : 0x05
*/
struct SDHP_USERCLOSE_ID
{
	PBMSG_HEAD h;
	char szId[10];
	char szName[10];
	unsigned short Level;
	unsigned char DbClass;

};











/* * * * * * * * * * * * * * * * * * * * *
*	Packet Join Fail
*	Direction : GameServer -> JoinServer
*  Code : 0xC1
*	HeadCode : 0x04
*/
struct SDHP_COMMAND_BLOCK
{
	PBMSG_HEAD h;
	short Number;
	char Id[11];
	int UserNumber;
	int DBNumber;
	unsigned char BlockCode;
};







/* * * * * * * * * * * * * * * * * * * * *
*	Packet Bill Kill User
*	Direction : JoinServer -> GameServer
*  Code : 0xC1
*	HeadCode : 0x07, 0x09
*/
struct SDHP_BILLKILLUSER
{
	struct PBMSG_HEAD h;
	char Id[10];
	short Number;
	bool ForceDisconnect;
};





/* * * * * * * * * * * * * * * * * * * * *
*	Packet Other Join Message
*	Direction : JoinServer -> GameServer
*  Code : 0xC1
*	HeadCode : 0x08
*/
struct SDHP_OTHERJOINMSG
{
	struct PBMSG_HEAD h;
	char AccountID[10];
};


/* * * * * * * * * * * * * * * * * * * * *
*	Packet Other Join Message
*	Direction : JoinServer -> GameServer
*  Code : 0xC1
*	HeadCode : 0x20
*/
struct SDHP_EXITMSG
{
	PBMSG_HEAD h;
	unsigned char ExitCode[3];
};


/* * * * * * * * * * * * * * * * * * * * *
*	Packet Other Join Message
*	Direction : JoinServer -> GameServer
*  Code : 0xC1
*	HeadCode : 0x30
*/
struct SDHP_EVENTSTART
{
	PBMSG_HEAD h;
	unsigned char Event;
};

/* * * * * * * * * * * * * * * * * * * * *
*	Packet Other Join Message
*	Direction : JoinServer -> GameServer
*  Code : 0xC1
*	HeadCode : 0x40
*/
struct SDHP_EVENTCONTROL
{
	PBMSG_HEAD h;
	unsigned char Event;
	unsigned char Stat;
};

/* * * * * * * * * * * * * * * * * * * * *
*	Packet Other Join Message
*	Direction : JoinServer -> GameServer
*  Code : 0xC1
*	HeadCode : 0x41
*/
struct SDHP_OPTIONCONTROL
{
	PBMSG_HEAD h;
	unsigned char Option;
};



/* * * * * * * * * * * * * * * * * * * * *
*	Packet Join Fail
*	Direction : GameServer -> JoinServer
*  Code : 0xC1
*	HeadCode : 0x30
*/
struct SDHP_LOVEHEARTEVENT
{
	PBMSG_HEAD h;
	char Account[10];
	char Name[10];
};




/* * * * * * * * * * * * * * * * * * * * *
*	Packet Other Join Message
*	Direction : JoinServer -> GameServer
*  Code : 0xC1
*	HeadCode : 0x61
*/
struct SDHP_LOVEHEARTEVENT_RESULT
{
	PBMSG_HEAD h;
	unsigned char Result;
	char Name[10];
	int Number;
};




/* * * * * * * * * * * * * * * * * * * * *
*	Packet Join Fail
*	Direction : GameServer -> JoinServer
*  Code : 0xC1
*	HeadCode : 0x31
*/
struct SDHP_LOVEHEARTCREATE
{
	PBMSG_HEAD h;
	unsigned char x;
	unsigned char y;
	unsigned char MapNumber;
};



/* * * * * * * * * * * * * * * * * * * * *
*	Packet Other Join Message
*	Direction : JoinServer -> GameServer
*  Code : 0xC1
*	HeadCode : 0x62
*/
struct SDHP_NOTICE
{
	PBMSG_HEAD h;
	char Notice[61];
};



/* * * * * * * * * * * * * * * * * * * * *
*	Packet Other Join Message
*	Direction : JoinServer -> GameServer
*  Code : 0xC1
*	HeadCode : 0x63
*/
struct SDHP_USER_NOTICE
{
	PBMSG_HEAD h;
	char szId[10];
	char Notice[61];
};



/* * * * * * * * * * * * * * * * * * * * *
*	Packet Join Fail
*	Direction : GameServer -> JoinServer
*  Code : 0xC1
*	HeadCode : 0x7A
*/
struct PMSG_REQ_MAPSVRMOVE
{
	PBMSG_HEAD h;
	int iIndex;
	char szAccountID[11];
	char szCharName[11];
	char szPassword[20];
	unsigned char btSecurityLock;
	unsigned int dwSecurityCode;
	unsigned short wCurMapSvrCode;
	unsigned short wDstMapSvrCode;
	unsigned short wMapNumber;
	unsigned char btX;
	unsigned char btY;
};






/* * * * * * * * * * * * * * * * * * * * *
*	Packet Map Server Auth Answer
*	Direction : JoinServer -> GameServer
*  Code : 0xC1
*	HeadCode : 0x7A ?????????????????????????????? #error
*/
struct PMSG_ANS_MAPSVRAUTH
{
	PBMSG_HEAD h;
	int iIndex;
	char szAccountID[11];
	char szCharName[11];
	char szPassword[20];
	BYTE btSecurityLock;
	DWORD dwSecurityCode;
	WORD wPrevMapSvrCode;
	WORD wMapNumber;
	BYTE btX;
	BYTE btY;
	int iResult;
	int iUserNumber;
	int iDBNumber;
	unsigned char btBlockCode;
};



/* * * * * * * * * * * * * * * * * * * * *
*	Packet Join Fail
*	Direction : GameServer -> JoinServer
*  Code : 0xC1
*	HeadCode : 0x7B
*/
struct PMSG_REQ_MAPSVRAUTH
{
	PBMSG_HEAD h;
	int iIndex;
	char szAccountID[11];
	char szCharName[11];
	unsigned short wDstMapSvrCode;
	int iJoinAuthCode1;
	int iJoinAuthCode2;
	int iJoinAuthCode3;
	int iJoinAuthCode4;
};

/* * * * * * * * * * * * * * * * * * * * *
*	Packet Notify Max User
*	Direction : GameServer -> JoinServer
*  Code : 0xC1
*	HeadCode : 0x7C
*/
struct PMSG_NOTIFY_MAXUSER
{
	PBMSG_HEAD h;
	int iSvrCode;	// 4
	int iMaxUserCount;	// 8
	int iCurUserCount;	//	C
};

struct PMSG_SET_OFFTRADE
{
	PBMSG_HEAD h;
	char szAccountID[11];
	char szName[11];
	bool m_bState;	// 4
};

/* * * * * * * * * * * * * * * * * * * * *
*	Packet Join Fail
*	Direction : GameServer ->
*  Code : 0xC1
*	HeadCode : 0x60
*/
/*struct PMSG_JG_MEMO_SEND
{
PWMSG_HEAD h;
char Name[10];
char TargetName[10];
char Subject[32];
short MemoSize;
char Memo[1000];
};*/



struct USER_DATA
{
	PMSG_ANS_MAPSVRMOVE pMapServerMoveData;
	int iUserNumber;
	DWORD dwTick;
	BYTE btSecurityLock;
	DWORD dwSecurityCode;
	char szPassword[20];
};



struct PMSG_LOGINSEVER_LIVE
{
	PBMSG_HEAD h;	// C1:02
	DWORD RQueue;
};

struct ISHOP_VIP_BUY
{
	PBMSG_HEAD		h;
	char			AccountID[11];
	BYTE			Type;
	int				Days;
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

struct CONNECT_OBJECT {
	DWORD lPacketTime;
	int COUNT;
	char ip[16];
};

struct ServerInfo {
	WORD UserCount;
	WORD MaxUserCount;
	WORD Percent;
	DWORD TickCount;
};

struct GS_CONNECT_INFO {
	PBMSG_HEAD h;
	UCHAR SubHead;
	char IP[16];
	USHORT Port;
};

struct PMSG_HELLO {
	PBMSG_HEAD h;
	UCHAR result;
};

struct PMSG_JOINRESULT
{
	PBMSG_HEAD h;	// C1:F1
	BYTE scode;	// 3
	BYTE result;	// 4
	BYTE NumberH;	// 5
	BYTE NumberL;	// 6
	BYTE CliVersion[5];	// 7
};

struct PMSG_CLIENTTIME
{
	PBMSG_HEAD h;
	WORD TimeH;
	WORD TimeL;
	WORD AttackSpeed;	// 8
	WORD Agility;
	WORD MagicSpeed;	// A
	char Version[10];
	DWORD ServerSeason;
};

#pragma pack (1)
struct PMSG_IDPASS
{
	PBMSG_HEAD h;
	BYTE subcode;	// 3
	char Id[10];	// 4
	char Pass[20];	// E
	char HWID[100]; // NEW
	unsigned long TickCount;	// 18
	BYTE CliVersion[5];	// 1C
	BYTE CliSerial[16];	// 21  
	DWORD ServerSeason;
};
#pragma pack ()

struct PMSG_CLIENTCLOSE
{
	PBMSG_HEAD h;
	BYTE SubCode;	// 3
	BYTE Flag;	// 4
};

struct PMSG_CLIENTMSG
{
	PBMSG_HEAD h;
	BYTE SubCode;	// 3
	BYTE Flag;	// 4
	BYTE subFlag;	// 5
};

struct STR_USER_CHARACTERS
{
	CHAR Name[11];
	WORD Level;
	BYTE Class;
	BYTE CtlCode;
	BYTE Resets;
	BYTE Inventory[INVENTORY_BINARY_SIZE];
	BYTE dbInventory[48];
};

struct SDHP_CHARLISTCOUNT
{
	PWMSG_HEAD h;
	short Number;	// 4
	BYTE Count;	// 6
	int DbNumber;	// 8
	int SecurityCode;
	BYTE EnableCharacterCreate;
	char AccountId[11];	// D
	BYTE MoveCnt;	// 18
	BYTE WhExpansion;
};

struct PMSG_CHARLISTCOUNT
{
	PBMSG_HEAD h;	// C1:F3:00
	BYTE subcode;	// 3
	BYTE MaxClass;	// 4
	BYTE MoveCnt;	// 5
	BYTE Count;	// 6
	BYTE CharacterSlotCount;
	BYTE WhExpansion;
};
struct SDHP_CHARLIST
{
	BYTE Index;	// 0
	char Name[11];	// 1
	char UnityBFRealName[11];
	WORD ServerCodeOfHomeWorld;
	WORD Level;	// C
	BYTE Class;	// E
	BYTE CtlCode;	// F
	BYTE dbInventory[48];	// 10
	BYTE btGuildStatus;	// 41
	WORD Resets;
	BYTE PK_Level;
};

struct PMSG_CHARLIST
{
	BYTE Index;	// 0
	char Name[10];	// 1
	WORD Level;	// C
	BYTE CtlCode;	// E
	BYTE CharSet[CHAR_SET_SIZE];	// F
	BYTE btGuildStatus;	// 21
};

struct PMSG_CHARLIST_S9
{
	BYTE Index;	// 0
	char Name[10];	// 1
	WORD Level;	// C
	BYTE CtlCode;	// E
	BYTE CharSet[CHAR_SET_SIZE];	// F
	BYTE btGuildStatus;	// 21
	BYTE btPkLevel;
};

struct PMSG_CHARLIST_ENABLE_CREATION
{
	PBMSG_HEAD2 h; // C1:DE:00
	BYTE EnableClass;// 4
};

struct PMSG_RESET_INFO_CHARLIST
{
	PBMSG_HEAD2 h;
	WORD Reset[5];
};

struct PMSG_NOTICE
{
	PBMSG_HEAD h;
	BYTE type;	// 3
	BYTE btCount;	// 4
	WORD wDelay;	// 6	
	int dwColor;	// 8
	BYTE btSpeed;	// C
	char Notice[256];	// D

};





#endif
