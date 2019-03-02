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


#pragma pack (1)

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
	PBMSG_HEAD h;
	unsigned char subcode;	// 3
	unsigned char result;	// 4
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
	ACE_HANDLE handle;
	struct _PER_SOCKET_CONTEXT* PerSocketContext;
	bool News;
	int PacketCount;
	ULONGLONG i64PacketTime;
	const char* SocketKey;
	class ACE_SOCK_Stream* Socket;
	BYTE ServerPhase;

	// Merged from GameObject/UserData.
	DWORD ConnectCheckTime;	// 38
	int Connected;	// 4
	char LoginMsgSnd;	// 8
	char LoginMsgCount;	// 9
	char CloseCount;	// A
	char CloseType;	// B

	char  AccountID[11];	// 68
	char  Password[20];	// 68
	char HWID[100];
	BYTE  m_cAccountItemBlock; // 1D0

	//BYTE m_btDestX;	// 1524
	//BYTE m_btDestY;	// 1525
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
	BYTE Buffer2[MAX_IO_BUFFER_SIZE]; // 178C
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
	char Name[11];	// 1
	WORD Level;	// C
	BYTE CtlCode;	// E
	BYTE CharSet[CHAR_SET_SIZE];	// F
	BYTE btGuildStatus;	// 21
};

struct PMSG_CHARLIST_S9
{
	BYTE Index;	// 0
	char Name[11];	// 1
	WORD Level;	// C
	BYTE CtlCode;	// E
	BYTE CharSet[CHAR_SET_SIZE];	// F
	BYTE btGuildStatus;	// 21
	BYTE btPkLevel;
	BYTE unk1;
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

struct PMSG_SET_CHAT_COLOR
{
	PBMSG_HEAD2 h;
	BYTE btInfoMsg[3];
	BYTE btErrorMsg[3];
	BYTE btChatMsg[3];
	BYTE btWhisperMsg[3];
	BYTE btPartyMsg[3];
	BYTE btGuildMsg[3];
	BYTE btGensMsg[3];
	BYTE btAllianceMsg[3];
	BYTE btGMChatMsg[3];
};


struct PMSG_CHARCREATE
{
	PBMSG_HEAD h;
	BYTE subcode;	// 3
	char Name[10];	// 4
	BYTE ClassSkin;	// E
};


struct PMSG_CHARDELETE
{
	PBMSG_HEAD h;
	BYTE subcode;	// 3
	char Name[10];	// 4
	char LastJoominNumber[20];	// E
};

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Get Char List Result Packet
*	Direction : GameServer -> Client
*  Code     : 0xC1
*	HeadCode : 0xF3
*	SubCode  : 0x01
*/
struct PMSG_CHARCREATERESULT
{
	PBMSG_HEAD h;	// C1:F3:01
	BYTE subcode;	// 3
	unsigned char Result;	// 4
	unsigned char Name[10];	// 5
	BYTE pos;	// F
	WORD Level;	// 10
	BYTE Class;	// 12
	BYTE Equipment[24];	// 13
};


struct PMSG_DISABLE_RECONNECT // can use 1 for both
{
	PBMSG_HEAD h;
	BYTE subcode;
	BYTE Trash[100];
};


struct GUILD_INFO_STRUCT
{
	int Number;	//0
	char Name[9];	//4
	BYTE Mark[32];	//D
	BYTE Count;	//2D
	BYTE TotalCount;	//2E
	//std::vector<CGameObject*> Users; // TODO
	//char Names[MAX_USER_GUILD][11];	//2F
	short Index[MAX_USER_GUILD];	//3A0
	BYTE Use[MAX_USER_GUILD];	//440
	char pServer[MAX_USER_GUILD];	//490
	char TargetGuildName[9];	//4E0
	short TargetIndex[MAX_USER_GUILD];	//4EA
	GUILD_INFO_STRUCT *lpTargetGuildNode;	//58C
	BYTE WarDeclareState;	//590
	BYTE WarState;	//591
	BYTE WarType;	//592
	BYTE BattleGroundIndex;	//593
	BYTE BattleTeamCode;	//594
	BYTE PlayScore;	//595
	int TotalScore;	//598
	char Notice[60];	//59c
	int GuildStatus[MAX_USER_GUILD];	//5D8
	BYTE btGuildType;	//718
	int iGuildUnion;	//71C
	int iGuildRival;	//720
	int iTimeStamp;	//724
	char szGuildRivalName[9];	//728
	GUILD_INFO_STRUCT *back;	//734
	GUILD_INFO_STRUCT *next;	//738
	//CGameObject* lpLifeStone;
	int btLifeStoneCount;

	// #error Deathay Fix here
	GUILD_INFO_STRUCT() {
		return;
	};
	int  GetGuildUnion() { return this->iGuildUnion; };
	int  GetGuildRival() { return this->iGuildRival; };

	void SetGuildUnion(int iGuildNumber)	// line : 102
	{
		this->iGuildUnion = iGuildNumber;
		this->SetTimeStamp();
	};	// line : 105

	void SetGuildRival(int iGuildNumber)	// line : 108
	{
		this->iGuildRival = iGuildNumber;
		this->SetTimeStamp();
	};	// line : 111

	void SetTimeStamp()	// line : 117
	{
		this->iTimeStamp++;
	};	// line : 119

	int GetTimeStamp()	// line : 122
	{
		return this->iTimeStamp;
	};	// line : 124

	BOOL CheckTimeStamp(int iTime)	// line : 127
	{
		return (iTime == this->iTimeStamp) ? TRUE : FALSE;
	}	// line : 129
};



// EXTRACTED FROM USER.H 

typedef union _PATHTABLE
{
	short sPathTable[16];
	char cPathTable[32];
} PATHTABLE, *LPPATHTABLE;

typedef struct ActionState
{
	unsigned long Rest : 1;	// 0
	unsigned long Attack : 1;	// 1
	unsigned long Move : 1;	// 2
	unsigned long Escape : 1;	// 3
	unsigned long Emotion : 4;	// 4
	unsigned long EmotionCount : 8;	// 8

} ACTION_STATE, *LPACTION_STATE;

typedef struct InterfaceState
{
	DWORD use : 2;
	DWORD state : 4;
	DWORD type : 10;

} INTERFACE_STATE, *LPINTERFACE_STATE;

extern short RoadPathTable[MAX_ROAD_PATH_TABLE];
extern BOOL g_EnergyCheckOff;

struct HITDAMAGE_STRUCT
{
	short number;	// 0
	int HitDamage;	// 4
	ULONGLONG LastHitTime;	// 8
};

struct VIEWPORT_STRUCT
{
	char state;	// 0
	short number;	// 2
	unsigned char type;	// 4
	short index;	// 6
	int dis;	// 8
};

struct VIEWPORT_PLAYER_STRUCT
{
	char state;	// 0
	short number;	// 2
	unsigned char type;	// 4
	short index;	// 6
	int dis;	// 8

};

struct MessageStateMachine
{
	int MsgCode;	// 0
	int SendUser;	// 4
	ULONGLONG MsgTime;	// 8
	int SubCode;	// C

	MessageStateMachine()
	{
		this->MsgCode = -1;
	}

};

struct ExMessageStateMachine
{
	int MsgCode;	// 0
	int SendUser;	// 4
	ULONGLONG MsgTime;	// 8
	int SubCode;	// C
	int SubCode2;	// 10

	ExMessageStateMachine()
	{
		this->MsgCode = -1;
	}
};

struct WHISPER_STRUCT
{
	bool iWhisperSent;
	WORD wResponseCount;
};
struct SKILL_INFO
{
	void Clear()
	{
		this->GhostPhantomX = 0;
		this->GhostPhantomY = 0;
		this->RemedyOfLoveEffect = 0;
		this->RemedyOfLoveTime = 0;
		this->LordSummonTime = 0;
		this->LordSummonMapNumber = 0;
		this->LordSummonX = 0;
		this->LordSummonY = 0;
		this->SoulBarrierDefence = 0;
		this->SoulBarrierManaRate = 0;
		this->PoisonType = 0;
		this->IceType = 0;
		this->fInfinityArrowIncRate = 0.0;
		this->fCircleShieldRate = 0.0;
	}

	BYTE	GhostPhantomX;
	BYTE	GhostPhantomY;
	short	RemedyOfLoveEffect;
	short	RemedyOfLoveTime;
	WORD	LordSummonTime;
	BYTE	LordSummonMapNumber;
	BYTE	LordSummonX;
	BYTE	LordSummonY;
	int		SoulBarrierDefence;
	int		SoulBarrierManaRate;
	BYTE	PoisonType;
	BYTE	IceType;
	float	fInfinityArrowIncRate;
	float	fCircleShieldRate;
};

struct _MASTERLEVEL_PASSIVE_SKILL_OPTION
{
	_MASTERLEVEL_PASSIVE_SKILL_OPTION()
	{
		this->Clear();
	}

	void Clear()
	{
		iMpsAttackSuccessRate = 0.0;
		iMpsIncDefenseSuccessRate = 0.0;
		iMpsIncreasePvPAttackRate = 0.0;
		iMpsIncreasePvPDefenseRate = 0.0;
		iMpsDurabilityReduction1 = 0.0;
		btMpsDownDur1Level = 0.0;
		iMpsDurabilityReduction2 = 0.0;
		iMpsDurabilityReduction3 = 0.0;
		iMpsResistancePoison = 0.0;
		iMpsResistanceThunder = 0.0;
		iMpsResistanceIce = 0.0;
		iMpsAutoRecoverLife = 0.0;
		iMpsAutoRecoverMana = 0.0;
		iMpsAutoRecoverAG = 0.0;
		iMpsAddPhysicDamage = 0.0;
		iMpsAddDamage = 0.0;
		iMpsTwoHandSwordAddDamage = 0.0;
		iMpsSwordAddDamage = 0.0;
		iMpsMaceStrength = 0.0;
		iMpsSpearAddDamage = 0.0;
		iMpsTwoHandsPvPAttack = 0.0;
		iMpsAddAttackSpeed = 0.0;
		iMpsIgnoreEnemyDefence = 0.0;
		iMpsAddDoubleDamage = 0.0;
		iMpsAddSturn = 0.0;
		iMpsMaxManaRate = 0.0;
		iMpsAddMagicDamage = 0.0;
		iMpsTwoHandsMagicPower = 0.0;
		iMpsStaffMagicPower = 0.0;
		iMpsIncShield = 0.0;
		iMpsStaffAttackSpeed = 0.0;
		iMpsTwoHandsStaffManAttack = 0.0;
		iMpsShieldBlockRate = 0.0;
		iMpsBowAddDamage = 0.0;
		iMpsBowAddAttackSpeed = 0.0;
		iMpsTwoHandsBowAddDamage = 0.0;
		iMpsTwoHandsBowManAttack = 0.0;
		iMpsElfAddShield = 0.0;
		iMpsElfShieldBlockRate = 0.0;
		iMpsElfAddPhysicDamage = 0.0;
		iMpsCallMonHp = 0.0;
		iMpsCallMonDefense = 0.0;
		iMpsCallMonAttack = 0.0;
		iMpsIncMagicPower = 0.0;
		iMpsIncDamageFireProper = 0.0;
		iMpsIncDamageWindProper = 0.0;
		iMpsIncDamageThunderProper = 0.0;
		iMpsIncDotDamage = 0.0;
		iMpsAddSturn2 = 0.0;
		iMpsAddSpring = 0.0;
		iMpsAddMagicPowerStick = 0.0;
		iMpsAddCurseMagicBook = 0.0;
		iMpsAddManAttackStick = 0.0;
		iMpsAddAttackSpeedMagicBook = 0.0;
		iMpsIncPercentBerserker1 = 0.0;
		iMpsIncPercentBerserker2 = 0.0;
		iMpsIncValueBerserker3 = 0.0;
		iMpsIncMinMagicDamage = 0.0;
		iMpsIncDarkHorseDefense = 0.0;
		iMpsAddForceWaveDamage = 0.0;
		iMpsAddDarkSpiritDamage = 0.0;
		iMpsAddCriticalDamageRate = 0.0;
		iMpsAddExcellentDamageRate = 0.0;
		iMpsAddCeptorDamage = 0.0;
		iMpsIncCeptorManAttack = 0.0;
		iMpsAddShieldDefense = 0.0;
		iMpsAddShieldBlockingRage = 0.0;
		iMpsAddPetAttack = 0.0;
		iMpsAddControlAttack = 0.0;
		iMpsAddMinAttack = 0.0;
		iMpsAddMaxAttack = 0.0;
		iMpsCriticalRateInc = 0.0;
		iMpsAddBlockingRate = 0.0;
		iMpsMonkWeaponDamage = 0.0;
		iMpsMonkDoubleDamage = 0.0;
		iMpsMonkAddVitalToDefense = 0.0;
		iMpsPlusZen = 0.0;
		iMpsDefence = 0.0;
		iMpsMaxHP = 0.0;
		iMpsMaxBP = 0.0;
		iMpsMaxMana = 0.0;
		iMpsMonsterDieGetMana = 0.0;
		iMpsMonsterDieGetLife = 0.0;
		iMpsMonsterDieGetSD = 0.0;
		iMpsPlusExp = 0.0;
		iMpsMaxSD = 0.0;
		iMpsSDSpeed = 0.0;
		iMpsMaxAttackDamage = 0.0;
		iMpsMinAttackDamage = 0.0;
		iMpsDecreaseMana = 0.0;
		iMpsMaxMagicDamage = 0.0;
		iMpsMinMagicDamage = 0.0;
		iMpsPetDurDownSpeed = 0.0;
		iMpsMaxAttackMagicDamage = 0.0;
		iMpsMinAttackMagicDamage = 0.0;
		iMpsImmuneRate = 0.0;
		iMpsIncSetItemDefense = 0.0;
		iMpsReturnEnemyAttack = 0.0;
		iMpsIncEnergyStat = 0.0;
		iMpsIncVitalStat = 0.0;
		iMpsIncDexStat = 0.0;
		iMpsIncPowerStat = 0.0;
		iMpsAddWingDamageBlock = 0.0;
		iMpsAddWingDefense = 0.0;
		iMpsAddWingDamage = 0.0;
		iMpsIncLeadershipStat = 0.0;
		iMpsIncCriticalDamageRate = 0.0;
		iMpsRecoverManaRate = 0.0;
		iMpsRecoverHPRate = 0.0;
		iMpsIncExcellentDamageRate = 0.0;
		iMpsIncDoubleDamageRate = 0.0;
		iMpsIncIgnoreEnemyBlock = 0.0;
		iMpsRecoverSDRate = 0.0;
		iMpsIncTripleDamageRate = 0.0;
		iMpsComboDamage = 0.0;
		iMpsIncMaxBP = 0.0;
		iMpsMasManaRate_Wizard = 0.0;
		iMpsIncMaxMagicDamage = 0.0;
		iMpsIncExcellentDamageRate_Darklord = 0.0;
		iMpsIncDamageBloodHowling = 0.0;
		iMpsIncDarkSpiritAttackSpeed = 0.0;
		iMpsIncDarkSpiritDoubleDamageRate = 0.0;
		iMpsDarkSpiritIgnoreEnemyDefense = 0.0;
		iMpsIncSteelArmor = 0.0;
		iMpsShieldBlock = 0.0;
		iMpsRageDamage = 0.0;
	}

	float	iMpsAttackSuccessRate; // done
	float	iMpsIncDefenseSuccessRate; // done
	float	iMpsIncreasePvPAttackRate; // done
	float	iMpsIncreasePvPDefenseRate; // done
	float	iMpsDurabilityReduction1; // done
	BYTE	btMpsDownDur1Level; // done
	float	iMpsDurabilityReduction2; // done
	float	iMpsDurabilityReduction3; // done
	float	iMpsResistancePoison; // done
	float	iMpsResistanceThunder; // done
	float	iMpsResistanceIce; // done
	float	iMpsAutoRecoverLife; // done
	float	iMpsAutoRecoverMana; // done
	float	iMpsAutoRecoverAG; // done
	float	iMpsAddPhysicDamage; // done
	float	iMpsAddDamage; // not used?
	float	iMpsTwoHandSwordAddDamage; // done
	float	iMpsSwordAddDamage; // done
	float	iMpsMaceStrength; // done
	float	iMpsSpearAddDamage; // done
	float	iMpsTwoHandsPvPAttack; // done
	float	iMpsAddAttackSpeed; // done
	float	iMpsIgnoreEnemyDefence; // done, but no reference in CMasterLevelSkillTreeSystem class
	float	iMpsAddDoubleDamage; // done
	float	iMpsAddSturn; // done
	float	iMpsMaxManaRate; // done
	float	iMpsAddMagicDamage; // done
	float	iMpsTwoHandsMagicPower; // done
	float	iMpsStaffMagicPower; // done
	float	iMpsIncShield; // done
	float	iMpsStaffAttackSpeed; // done
	float	iMpsTwoHandsStaffManAttack;  // done
	float	iMpsShieldBlockRate; // done
	float	iMpsBowAddDamage; // done
	float	iMpsBowAddAttackSpeed; // done
	float	iMpsTwoHandsBowAddDamage; // done
	float	iMpsTwoHandsBowManAttack; // done
	float	iMpsElfAddShield; // done
	float	iMpsElfShieldBlockRate; // done
	float	iMpsElfAddPhysicDamage; // done
	float	iMpsCallMonHp; // done
	float	iMpsCallMonDefense; // done
	float	iMpsCallMonAttack; // done
	float	iMpsIncMagicPower; // done
	float	iMpsIncDamageFireProper; // done
	float	iMpsIncDamageWindProper; // done
	float	iMpsIncDamageThunderProper; // done
	float	iMpsIncDotDamage; // done
	float	iMpsAddSturn2; // done
	float	iMpsAddSpring; // done
	float	iMpsAddMagicPowerStick; // done
	float	iMpsAddCurseMagicBook; // done
	float	iMpsAddManAttackStick; // done
	float	iMpsAddAttackSpeedMagicBook; // done
	float	iMpsIncPercentBerserker1; // done
	float	iMpsIncPercentBerserker2; // done
	float	iMpsIncValueBerserker3; // done
	float	iMpsIncMinMagicDamage; // done
	float	iMpsIncDarkHorseDefense; // done
	float	iMpsAddForceWaveDamage; // not used
	float	iMpsAddDarkSpiritDamage; // done
	float	iMpsAddCriticalDamageRate; // done
	float	iMpsAddExcellentDamageRate; // done
	float	iMpsAddCeptorDamage; // done
	float	iMpsIncCeptorManAttack; // done
	float	iMpsAddShieldDefense; // done
	float	iMpsAddShieldBlockingRage; // done
	float	iMpsAddPetAttack; // done
	float	iMpsAddControlAttack; // done
	float	iMpsAddMinAttack; // done
	float	iMpsAddMaxAttack; // done
	float	iMpsCriticalRateInc; // done
	float	iMpsAddBlockingRate; // done
	float	iMpsMonkWeaponDamage; // done
	float	iMpsMonkDoubleDamage; // done
	float	iMpsMonkAddVitalToDefense; // done
	float	iMpsPlusZen; // done, but no reference in CMasterLevelSkillTreeSystem class
	float	iMpsDefence; // done
	float	iMpsMaxHP; // done
	float	iMpsMaxBP; // done
	float	iMpsMaxMana; // done
	float	iMpsMonsterDieGetMana; // done
	float	iMpsMonsterDieGetLife; // done
	float	iMpsMonsterDieGetSD; // done
	float	iMpsPlusExp; // not used
	float	iMpsMaxSD; // done
	float	iMpsSDSpeed; // done
	float	iMpsMaxAttackDamage; // done
	float	iMpsMinAttackDamage; // done
	float	iMpsDecreaseMana; // done
	float	iMpsMaxMagicDamage; // done
	float	iMpsMinMagicDamage; // done
	float	iMpsPetDurDownSpeed; // done
	float	iMpsMaxAttackMagicDamage; // done, but no reference in CMasterLevelSkillTreeSystem class
	float	iMpsMinAttackMagicDamage; // done, but no reference in CMasterLevelSkillTreeSystem class
	float	iMpsImmuneRate; // done
	float	iMpsIncSetItemDefense; // done
	float	iMpsReturnEnemyAttack; // done
	float	iMpsIncEnergyStat; // done
	float	iMpsIncVitalStat; // done
	float	iMpsIncDexStat; // done
	float	iMpsIncPowerStat; // done
	float	iMpsAddWingDamageBlock; // done
	float	iMpsAddWingDefense; // done
	float	iMpsAddWingDamage; // done
	float	iMpsIncLeadershipStat; // done
	float	iMpsIncCriticalDamageRate; // done
	float	iMpsRecoverManaRate; // done
	float	iMpsRecoverHPRate; // done
	float	iMpsIncExcellentDamageRate; // done
	float	iMpsIncDoubleDamageRate; // done
	float	iMpsIncIgnoreEnemyBlock; // done
	float	iMpsRecoverSDRate; // done
	float	iMpsIncTripleDamageRate; // done
	float	iMpsComboDamage; // done
	float	iMpsIncMaxBP; // done
	float	iMpsMasManaRate_Wizard; // done
	float	iMpsIncMaxMagicDamage; // done
	float	iMpsIncExcellentDamageRate_Darklord; // done
	float	iMpsIncDamageBloodHowling; // done
	float	iMpsIncDarkSpiritAttackSpeed; // done
	float	iMpsIncDarkSpiritDoubleDamageRate; // done
	float	iMpsDarkSpiritIgnoreEnemyDefense; // done
	float	iMpsIncSteelArmor; // done
	float	iMpsShieldBlock; // done
	float	iMpsRageDamage; // done
};

struct EFFECTLIST
{
	BYTE BuffIndex;
	BYTE EffectCategory;
	BYTE EffectType1;
	BYTE EffectType2;
	int EffectValue1;
	int EffectValue2;
	ULONGLONG EffectSetTime;
	int EffectDuration;
};

struct PENTAGRAMJEWEL_INFO
{
	BYTE btJewelPos;
	BYTE btJewelIndex;
	BYTE btMainAttribute;
	BYTE btItemType;
	WORD wItemIndex;
	BYTE btLevel;
	BYTE btRank1OptionNum;
	BYTE btRank1Level;
	BYTE btRank2OptionNum;
	BYTE btRank2Level;
	BYTE btRank3OptionNum;
	BYTE btRank3Level;
	BYTE btRank4OptionNum;
	BYTE btRank4Level;
	BYTE btRank5OptionNum;
	BYTE btRank5Level;

	void Clear()
	{
		this->btJewelPos = -1;
		this->btJewelIndex = -1;
		this->btMainAttribute = -1;
		this->btItemType = -1;
		this->wItemIndex = -1;
		this->btLevel = 0;
		this->btRank1OptionNum = -1;
		this->btRank1Level = -1;
		this->btRank2OptionNum = -1;
		this->btRank2Level = -1;
		this->btRank3OptionNum = -1;
		this->btRank3Level = -1;
		this->btRank4OptionNum = -1;
		this->btRank4Level = -1;
		this->btRank5OptionNum = -1;
		this->btRank5Level = -1;
	}
};

struct PENTAGRAM_OPTION
{
	int m_iRuby_1RankAddDamage;
	int m_iRuby_2RankOptionNum;
	int m_iRuby_2RankAddAttackRelationshipRate;
	int m_iRuby_3RankOptionNum;
	int m_iRuby_3RankAddDamage;
	int m_iRuby_4RankOptionNum;
	int m_iRuby_4RankAddDamage;
	int m_iRuby_5RankOptionNum;
	int m_iRuby_5RankCriticalDamageRate;
	int m_iSapph_1RankAddDefense;
	int m_iSapph_2RankOptionNum;
	int m_iSapph_2RankAddDefenseRelationshipRate;
	int m_iSapph_3RankOptionNum;
	int m_iSapph_3RankAddDefense;
	int m_iSapph_4RankOptionNum;
	int m_iSapph_4RankAddDefense;
	int m_iSapph_5RankOptionNum;
	int m_iSapph_5RankMinusTargetDamageRate;
	int m_iEme_1RankAddAttackRate;
	int m_iEme_2RankOptionNum;
	int m_iEme_2RankAddAttackRelationshipRate;
	int m_iEme_3RankOptionNum;
	int m_iEme_3RankAddDamage;
	int m_iEme_4RankOptionNum;
	int m_iEme_4RankAddDamage;
	int m_iEme_5RankOptionNum;
	int m_iEme_5RankAddDamageRate;
	int m_iTopa_1RankOptionNum;
	int m_iTopa_1RankAddDefenseSuccessRate;
	int m_iTopa_2RankOptionNum;
	int m_iTopa_2RankAddDefenseRelationshipRate;
	int m_iTopa_3RankOptionNum;
	int m_iTopa_3RankAddDefense;
	int m_iTopa_4RankOptionNum;
	int m_iTopa_4RankAddDefense;
	int m_iTopa_5RankOptionNum;
	int m_iTopa_5RankDamageAbsorbRate;
	int m_iOnyx_1RankSlowMoveSkillRate;
	int m_iOnyx_2RankAddStrength;
	int m_iOnyx_2RankAddDexterity;
	int m_iOnyx_2RankAddEnergy;
	int m_iOnyx_2RankAddVitality;
	int m_iOnyx_3RankAddMaxLife;
	int m_iOnyx_3RankAddMaxMana;
	int m_iOnyx_3RankAddMaxAG;
	int m_iOnyx_3RankAddMaxSD;
	int m_iOnyx_4RankOptionNum;
	int m_iOnyx_4RankAddExllentDamageRate;
	int m_iOnyx_5RankHalfValueSkillRate;
	bool m_isAddPentaAttack;
	bool m_isAddPentaDefense;
	bool m_isAddCriPentaDamage;
	bool m_isChangePentaDefense;
	bool m_isChangePentaAttack;
	bool m_isAddResistByStrongRelationShip;
	bool m_isAddResistByPentaAttack;
};

struct _BOT_BUFF_LIST
{
	WORD wBuffId;
	WORD wDuration;
	int iEffect;
	WORD wEffectType;
};

struct EXC_WING_OPTION
{
	EXC_WING_OPTION()
	{
		this->Clear();
	}

	void Clear()
	{
		this->iWingOpIgnoreEnemyDefense = 0;
		this->iWingOpReturnEnemyDamage = 0;
		this->iWingOpRecoveryHP = 0;
		this->iWingOpRecoveryMana = 0;
		this->iWingOpAddDoubleDamage = 0;
	}

	int iWingOpIgnoreEnemyDefense;
	int iWingOpReturnEnemyDamage;
	int iWingOpRecoveryHP;
	int iWingOpRecoveryMana;
	int iWingOpAddDoubleDamage;
};

struct STAT_USER_OPTION
{
	STAT_USER_OPTION()
	{
		this->Clear();
	}

	void Clear()
	{
		this->StatOptionID = 0;
		this->StatOptionValMin = 0;
		this->StatOptionValMax = 0;
	}

	WORD StatOptionID;
	WORD StatOptionValMin;
	WORD StatOptionValMax;
};

struct MUUN_EFFECT_LIST
{
	MUUN_EFFECT_LIST()
	{
		this->Clear();
	}

	void Clear()
	{
		this->nIndex = 0;
		this->nMuunItemNum = 0;
		this->nCategory = 0;
		this->bOptEnable = 0;
		this->nOptType = 0;
		this->nOptValue = 0;
		this->bAddOptEnable = 0;
		this->nAddOptType = 0;
		this->nAddOptValue = 0;
		this->nSetTime = 0;
		this->nDuration = 0;
		//this->pCMuunInfo = NULL;
		this->nTotalVal = 0;
		this->nSkillDelayTime = 0;
		this->bSkillUsed = false;
		this->nTargetIndex = -1;
		this->nTickTime = 0;
	}

	int nIndex;
	int nMuunItemNum;
	char nCategory;
	bool bOptEnable;
	char nOptType;
	int nOptValue;
	bool bAddOptEnable;
	char nAddOptType;
	int nAddOptValue;
	DWORD nSetTime;
	int nDuration;
	//CMuunInfo *pCMuunInfo;
	int nTotalVal;
	int nSkillDelayTime;
	int nTickTime;
	bool bSkillUsed;
	int nTargetIndex;
};

struct MOVE_MAPSERVER_AUTHINFO
{
	char szCharName[MAX_ACCOUNT_LEN + 2];
	int iJA1;
	int iJA2;
	int iJA3;
	int iJA4;
};

typedef struct
{
	BYTE m_SocketOptionIndex;
	BYTE m_SocketOptionValueType;
	WORD m_SocketOptionValue;
	BYTE m_SocketOptionCount;

	void Clear()
	{
		this->m_SocketOptionIndex = 0xFF;
		this->m_SocketOptionValue = 0;
		this->m_SocketOptionValueType = 0;
		this->m_SocketOptionCount = 0;
	}
} SOCKET_OPTION_LIST, *LPSOCKET_OPTION_LIST;

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

struct GREMORYCASE_ITEM_DATA
{
	GREMORYCASE_ITEM_DATA()
	{
		this->Clear();
	}

	void Clear()
	{
		this->btRewardInventory = 0;
		this->btRewardSource = 0;
		this->dwItemGUID = (DWORD)-1;
		this->ItemInfo->Clear();
		this->dwAuthCode = 0;
		this->dwReceiveDate = 0;
		this->dwExpireTime = 0;
	}

	BYTE btRewardInventory;
	BYTE btRewardSource;
	DWORD dwItemGUID;
	CItemObject* ItemInfo;
	DWORD dwAuthCode;
	DWORD dwReceiveDate;
	DWORD dwExpireTime;
};


struct PMSG_PENTAGRAM_JEWEL_REFINE_RECV
{
	PBMSG_HEAD2 header; // C1:EC:02
	BYTE type;
};

struct PMSG_REGEVENTCHIP_RESULT
{
	PBMSG_HEAD h;	// C1:95
	BYTE Type;	// 3
	int ChipCount;	// 4
};


struct PMSG_GETMUTONUMBER_RESULT
{
	PBMSG_HEAD h;	// C1:96
	short MutoNum[3];	// 4
};

struct _CS_NPC_DATA
{
	_CS_NPC_DATA();
	virtual ~_CS_NPC_DATA();
	void Clear();
	void SetBaseValue();

	BOOL m_bIN_USE;

	int m_iNPC_NUM;
	int m_iNPC_INDEX;

	BOOL m_bNPC_DBSAVE;
	int m_iNPC_SIDE;
	int m_iNPC_OBJINDEX;
	int m_iNPC_DF_LEVEL;
	int m_iNPC_RG_LEVEL;
	int m_iNPC_MAXHP;
	int m_iNPC_HP;
	int m_iNPC_SX;
	int m_iNPC_SY;
	int m_iNPC_DX;
	int m_iNPC_DY;
	int m_iNPC_DIR;
	int m_iNPC_BASE_DF_LEVEL;
	int m_iNPC_BASE_RG_LEVEL;
	int m_iNPC_BASE_MAXHP;
	int m_iNPC_BASE_HP;
	int m_iNPC_BASE_SX;
	int m_iNPC_BASE_SY;
	int m_iNPC_BASE_DX;
	int m_iNPC_BASE_DY;
	int m_iNPC_BASE_DIR;
	int m_iCS_GATE_LEVER_INDEX;
	int m_iNPC_LIVE;
	int m_iNPC_AUTH;

	union
	{
		struct
		{
			BYTE m_btCsNpcExistVal1;
			BYTE m_btCsNpcExistVal2;
			BYTE m_btCsNpcExistVal3;
			BYTE m_btCsNpcExistVal4;
		};
		int m_iCsNpcExistVal;
	};
};


struct PMSG_MASTER_LEVEL_UP_SEND
{
	PBMSG_HEAD2 h;
	WORD MasterLevel;
	WORD GainPoint;
	WORD MLPoint;
	WORD MaxPoint;
	WORD MaxLife;
	WORD MaxMana;
	WORD MaxShield;
	WORD MaxBP;
	int IGCMaxLife;
	int IGCMaxMana;
};


struct PMSG_REQ_LUCKYCOIN
{
	PBMSG_HEAD2 h;
	int iIndex;
	char szUID[11];
};

struct PMSG_ANS_ARCA_BATTLE_GUILD_JOIN_DS
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	WORD wNumber;
};

struct PMSG_ANS_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS
{
	PBMSG_HEAD2 h;
	char btResult;
	WORD wNumber;
};

struct PMSG_ANS_ARCA_BATTLE_ENTER_DS
{
	PBMSG_HEAD2 h;
	char btResult;
	BYTE btEnterSeq;
	WORD wNumber;
};

struct _stPMSG_CCF_RANK_INFO
{
	PWMSG_HEAD2 h;
	BYTE btResult;
	BYTE btCnt;
};


struct _stPMSG_CCF_RANK_INFO
{
	PWMSG_HEAD2 h;
	BYTE btResult;
	BYTE btCnt;
};


struct JEWELOFHARMONY_ITEM_EFFECT
{
	short HJOpAddMinAttackDamage;	// 0
	short HJOpAddMaxAttackDamage;	// 2
	short HJOpRequireStr;	// 4
	short HJOpRequireDex;	// 6
	short HJOpAddAttackDamage;	// 8
	short HJOpAddCriticalDamage;	// A
	short HJOpAddSkillAttack;	// C
	short HJOpAddAttackSuccessRatePVP;	// E
	short HJOpDecreaseSDRate;	// 10
	short HJOpAddIgnoreSDRate;	// 12
	short HJOpAddMagicPower;	// 14
	short HJOpAddDefense;	// 16
	short HJOpAddMaxAG;	// 18
	short HJOpAddMaxHP;	// 1A
	short HJOpAddRefillHP;	// 1C
	short HJOpAddRefillMP;	// 1E
	short HJOpAddDefenseSuccessRatePvP;	// 20
	short HJOpAddDamageDecrease;	// 22
	short HJOpAddSDRate;	// 24
};

typedef struct ITEMEFFECT
{
	WORD wOptionType;
	WORD wEffectType1;
	WORD wEffectType2;
	int iItemNumber;
	int iItemType;
	int iItemIndex;
	int iEffectValue1;
	int iEffectValue2;
	int iEffectValidTime;

} ITEMEFFECT, *LPITEMEFFECT;






#pragma pack ()

#endif
