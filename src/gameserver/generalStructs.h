#ifndef _GENERALSTRUCTS_H
#define _GENERALSTRUCTS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "GensSystemProtocol.h"
//#include "CGameObject.h"
#include "ItemObject.h"

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

} STR_ACTION_STATE, *LPACTION_STATE;

typedef struct InterfaceState
{
	DWORD use : 2;
	DWORD state : 4;
	DWORD type : 10;

} STR_INTERFACE_STATE, *LPINTERFACE_STATE;

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
	class CItemObject* ItemInfo;
	DWORD dwAuthCode;
	DWORD dwReceiveDate;
	DWORD dwExpireTime;
};


struct PMSG_PENTAGRAM_JEWEL_REFINE_RECV
{
	PBMSG_HEAD2 header; // C1:EC:02
	BYTE type;
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


struct PWMSG_COUNT
{
	PWMSG_HEAD h;
	BYTE count;
};

struct PBMSG_COUNT
{
	PBMSG_HEAD h;
	BYTE count;
};

struct PBMSG_COUNT2
{
	PBMSG_HEAD2 h;
	BYTE count;
};

struct PWMSG_COUNT2
{
	PWMSG_HEAD2 h;
	BYTE count;
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


struct PMSG_ANS_REGISTERED_MEMBER_CNT
{
	PBMSG_HEAD2 h;
	BYTE unk1;
	BYTE unk2;
};


struct PMSG_CCF_RANK_INFO
{
	PWMSG_HEAD2 h;
	BYTE btResult;
	BYTE btCnt;
};

struct STR_EFFECTLIST
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

struct PMSG_ITEMVIEWPORTCREATE
{
	BYTE NumberH;
	BYTE NumberL;
	BYTE px;
	BYTE py;
	BYTE ItemInfo[MAX_ITEM_INFO];
};

typedef struct PMSG_VIEWPORTCREATE
{
	BYTE	NumberH;
	BYTE	NumberL;
	BYTE	X;
	BYTE	Y;
	BYTE	CharSet[18];
	char	Id[MAX_ACCOUNT_LEN];
	BYTE	TX;
	BYTE	TY;
	BYTE	DirAndPkLevel;
	BYTE	PentagramMainAttribute;
	BYTE	MuunItemH;
	BYTE	MuunItemL;
	BYTE	unk;
	BYTE	MuunSubItemH;
	BYTE	MuunSubItemL;
	BYTE    unk1;
	BYTE	MuunRideItemH;
	BYTE	MuunRideItemL;
	BYTE    unk2;
	BYTE	LevelH;
	BYTE	LevelL;
	BYTE	MaxLifeHH;
	BYTE	MaxLifeHL;
	BYTE	MaxLifeLH;
	BYTE	MaxLifeLL;
	BYTE	CurLifeHH;
	BYTE	CurLifeHL;
	BYTE	CurLifeLH;
	BYTE	CurLifeLL;
	WORD	nServerCodeOfHomeWorld;
	BYTE	BuffEffectCount;
	BYTE	BuffEffectList[MAX_BUFFEFFECT];
	BYTE    unk3;
} *LPPMSG_VIEWPORTCREATE;

typedef struct PMSG_VIEWPORTCREATE_CHANGE
{
	BYTE	NumberH;
	BYTE	NumberL;
	BYTE	X;
	BYTE	Y;
	BYTE	SkinH;
	BYTE	SkinL;
	char	Id[MAX_ACCOUNT_LEN];
	BYTE	TX;
	BYTE	TY;
	BYTE	DirAndPkLevel;
	BYTE	CharSet[18];
	BYTE	PentagramMainAttribute;
	BYTE	MuunItemH;
	BYTE	MuunItemL;
	BYTE	unk;
	BYTE	MuunSubItemH;
	BYTE	MuunSubItemL;
	BYTE    unk1;
	BYTE	MuunRideItemH;
	BYTE	MuunRideItemL;
	BYTE    unk2;
	BYTE	LevelH;
	BYTE	LevelL;
	BYTE	MaxLifeHH;
	BYTE	MaxLifeHL;
	BYTE	MaxLifeLH;
	BYTE	MaxLifeLL;
	BYTE	CurLifeHH;
	BYTE	CurLifeHL;
	BYTE	CurLifeLH;
	BYTE	CurLifeLL;
	WORD	nServerCodeOfHomeWorld;
	BYTE	BuffEffectCount;
	BYTE	BuffEffectList[MAX_BUFFEFFECT];
	BYTE    unk3;
} *LPPMSG_VIEWPORTCREATE_CHANGE;

typedef struct PMSG_VIEWPORTDESTROY
{
	BYTE	NumberH;
	BYTE	NumberL;
} *LPPMSG_VIEWPORTDESTROY;

struct PMSG_MONSTER_VIEWPORTCREATE
{
	BYTE	NumberH;
	BYTE	NumberL;
	BYTE	Type_HI;
	BYTE	Type_LO;
	BYTE	X;
	BYTE	Y;
	BYTE	TX;
	BYTE	TY;
	BYTE	Path;
	BYTE	PentagramMainAttribute;
	BYTE	LevelH;
	BYTE	LevelL;
	BYTE	MaxLifeHH;
	BYTE	MaxLifeHL;
	BYTE	MaxLifeLH;
	BYTE	MaxLifeLL;
	BYTE	CurLifeHH;
	BYTE	CurLifeHL;
	BYTE	CurLifeLH;
	BYTE	CurLifeLL;
	BYTE	BuffEffectCount;
	BYTE	BuffEffectList[MAX_BUFFEFFECT];
};

struct PMSG_CALLMONSTER_VIEWPORTCREATE
{
	BYTE	NumberH;
	BYTE	NumberL;
	BYTE	Type_HI;
	BYTE	Type_LO;
	BYTE	X;
	BYTE	Y;
	BYTE	TX;
	BYTE	TY;
	BYTE	Path;
	BYTE	Id[MAX_ACCOUNT_LEN];
	BYTE	PentagramMainAttribute;
	BYTE	LevelH;
	BYTE	LevelL;
	BYTE	MaxLifeHH;
	BYTE	MaxLifeHL;
	BYTE	MaxLifeLH;
	BYTE	MaxLifeLL;
	BYTE	CurLifeHH;
	BYTE	CurLifeHL;
	BYTE	CurLifeLH;
	BYTE	CurLifeLL;
	BYTE	CharSet[18];
	BYTE	SummonType;
	BYTE	BuffEffectCount;
	BYTE	BuffEffectList[MAX_BUFFEFFECT];
};

struct PMSG_GUILDVIEWPORT_USER
{
	BYTE NumberH;
	BYTE NumberL;
	BYTE GNumberH;
	BYTE GNumberL;
};

struct PMSG_GUILDVIEWPORT
{
	BYTE NumberH;
	BYTE NumberL;
	char GuildName[8];
	BYTE Mark[32];
};


struct SDHP_SUMMONER
{
	PBMSG_HEAD h;
	char AccountId[10];
	short Number;
	BYTE btSummoner;
};

struct SDHP_CREATECHARRESULT
{
	PBMSG_HEAD h;
	unsigned char Result;	// 3
	short Number;	// 4
	char AccountId[11];	// 6
	char Name[11];	// 10
	BYTE Pos;	// 1A
	BYTE ClassSkin;	// 1B
	BYTE Equipment[24];	// 1C
	WORD Level;	// 34
};


struct SDHP_CHARDELETERESULT
{
	PBMSG_HEAD h;
	unsigned char Result;	// 3
	short Number;	// 4
	char AccountID[11];	// 6
};


struct SDHP_DBCHAR_INFORESULT
{
	PWMSG_HEAD h;
	unsigned char result;	// 4
	short Number;	// 6
	char AccountID[11];	// 8
	char Name[11];	// 12
	BYTE Class;	// 1D
	short Level;	// 1E
	short mLevel;
	int LevelUpPoint;	// 20
	int mlPoint;
	UINT64 Exp;	// 24
	UINT64 NextExp;	// 28
	UINT64 mlExp;
	UINT64 mlNextExp;
	int Money;	// 2C
	WORD Str;	// 30
	WORD Dex;	// 32
	WORD Vit;	// 34
	WORD Energy;	// 36
	DWORD Life;	// 38
	DWORD MaxLife;	// 3A
	DWORD Mana;	// 3C
	DWORD MaxMana;	// 3E
	BYTE dbInventory[7584];	// 34
	BYTE dbMagicList[450];	// 700
	BYTE MapNumber;	// 7B4
	BYTE MapX;	// 7B5
	BYTE MapY;	// 7B6
	BYTE Dir;	// 7B7
	int PkCount;	// 7B8
	int PkLevel;	// 7BC
	int PkTime;	// 7C0
	BYTE CtlCode;	// 7C4
	DWORD GmCode;
	WORD GmExpDays;
	BYTE AccountCtlCode;	// 7C6
	BYTE dbQuest[100];	// 7C7
	WORD Leadership;	// 7FA
	WORD ChatLitmitTime;	// 7FC
	int iFruitPoint;	// 800
	int resets;
	char MarryName[11];
	char Married;
	char InventoryExpansion;
	char WarehouseExpansion;
	int WinDuels;
	int LoseDuels;
	int Ruud;
	time_t ChatBlockTime;
	DWORD PenaltyMask;
	BYTE EventMap;
};

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Get Character Info Packet
*	Direction : GameServer -> Client
*  Code     : 0xC3
*	HeadCode : 0xF3
*	SubCode  : 0x03
*/
struct PMSG_CHARMAPJOINRESULT
{
	PBMSG_HEAD h;	// C3:F3:03
	BYTE subcode;	// 3
	BYTE MapX;	// 4
	BYTE MapY;	// 5
	BYTE MapNumber;	// 6
	BYTE Dir;	// 7
	BYTE ExpHHH; //
	BYTE ExpHHL; //
	BYTE ExpHLH; //
	BYTE ExpHLL; //
	BYTE ExpLHH; //
	BYTE ExpLHL; //
	BYTE ExpLLH; //
	BYTE ExpLLL; //
	BYTE NextExpHHH; //
	BYTE NextExpHHL; //
	BYTE NextExpHLH; //
	BYTE NextExpHLL; //
	BYTE NextExpLHH; //
	BYTE NextExpLHL; //
	BYTE NextExpLLH; //
	BYTE NextExpLLL; //
	WORD Unknw;
	WORD LevelUpPoint;	// 10
	WORD Str;	// 12
	WORD Dex;	// 14
	WORD Vit;	// 16
	WORD Energy;	// 18
	WORD Life;	// 1A
	WORD MaxLife;	// 1C
	WORD Mana;	// 1E
	WORD MaxMana;	// 20
	WORD wShield;	// 22
	WORD wMaxShield;	// 24
	WORD BP;	// 26
	WORD MaxBP;	// 28
	int Money;	// 2C
	BYTE PkLevel;	// 30
	BYTE CtlCode;	// 31
	short AddPoint;	// 32
	short MaxAddPoint;	// 34
	WORD Leadership;	// 36
	WORD wMinusPoint;	// 38
	WORD wMaxMinusPoint;	// 3A
	WORD InvExpansion;
	int Ruud; // Season X Addon
	int IGCLife;
	int IGCMaxLife;
	int IGCMana;
	int IGCMaxMana;
	DWORD GetTick;
};

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Warehouse DB Save Packet
*	Direction : GameServer -> DataServer [0x09]
*	            DataServer -> GameServer [0x08]
*  Code      : 0xC2
*	HeadCode  : 0x08, 0x09
*/
struct SDHP_GETWAREHOUSEDB_SAVE
{
	PWMSG_HEAD h;
	char AccountID[11];	// 4
	short aIndex;	// E
	int Money;	// 10
	BYTE dbItems[7680];	// 14
	short pw;	// 796
	BYTE WarehouseID;
	BYTE CloseWindow;
};


struct SDHP_GETWAREHOUSEDB_RESULT
{
	PBMSG_HEAD h;
	char AccountID[11];	// 3
	short aIndex;	// E
	BYTE WarehouseID;
};


struct SDHP_ITEMCREATERECV
{
	PBMSG_HEAD h;
	BYTE x;	// 3
	BYTE y;	// 4
	BYTE MapNumber;	// 5 [235:Mu_2nd_Aniv], [236:CashShop]
	UINT64 m_Number;	// 8
	short Type;	// 6
	BYTE Level;	// 8
	BYTE Dur;	// 9
	BYTE Op1;	// A
	BYTE Op2;	// B
	BYTE Op3;	// C
	BYTE NewOption;	// D
	int aIndex;	// 10
	int lootindex;	// 14
	BYTE SetOption;	// 16
	time_t lDuration;
	BYTE SocketOption[5];
	BYTE MainAttribute;
};

struct SDHP_SKILLKEYDATA_SEND
{
	PBMSG_HEAD h;
	int aIndex;	// 4
	char Name[11];	// 8
	BYTE SkillKeyBuffer[20];	// 12
	BYTE GameOption;	// 1C
	BYTE QkeyDefine;	// 1D
	BYTE WkeyDefine;	// 1E
	BYTE EkeyDefine;	// 1F
	BYTE ChatWindow;	// 20
	BYTE RkeyDefine;
	int QWERLevelDefine;
	BYTE EnableTransformMode;
};

struct SDHP_CHARACTER_TRANSFER_RESULT
{
	PBMSG_HEAD h;
	char Account[10];	// 3
	short Number;	// E
	unsigned char Result;	// 10
};

struct SDHP_QMKMLS_LOAD
{
	PBMSG_HEAD h;
	int aIndex;
	char Name[11];
};

struct SDHP_MLS_DATA
{
	PBMSG_HEAD h;
	char Name[11];
	short ML_Level;
	long long ML_Experience;
	long long ML_NextExperience;
	short ML_Point;
};

struct SDHP_MLS_DATA_REQUEST
{
	PBMSG_HEAD h;
	BYTE SubCode;
	int Index;
	char Name[11];
	BYTE Unknow1;
	short ML_Level;
	WORD Unknow2;
	long long ML_Experience;
	long long ML_NextExperience;
	short ML_Point;
	short Unknow3;
	int Unknow4;
};

struct GSP_REQ_GENS_JOIN
{
	PBMSG_HEAD2		h;
	char			Name[11];
	short			aIndex;
	char			GensID;
};

struct GSP_ANS_GENS_JOIN
{
	PBMSG_HEAD2		h;
	char			Result;
	short			aIndex;
	char			Name[11];
	char			GensID;
	char			GensPos;
	int				GensExp;
	int				GensNextExp;
};

struct GSP_REQ_GENS_INFO
{
	PBMSG_HEAD2		h;
	short			aIndex;
	char			Name[11];
};

struct GSP_ANS_GENS_INFO
{
	PBMSG_HEAD2		h;
	short			aIndex;
	char			Name[11];
	char			GensID;
	char			GensPos;
	int				GensExp;
	int				GensNextExp;
	short			GensRank;
	bool			GensReward;
};

struct GSP_REQ_GENS_QUIT
{
	PBMSG_HEAD2		h;
	short			aIndex;
	char			Name[11];
};

struct GSP_ANS_GENS_QUIT
{
	PBMSG_HEAD2		h;
	short			aIndex;
	char			Name[11];
	char			Result;
};

struct GSP_REQ_GENS_RANK
{
	PBMSG_HEAD2		h;
	short			aIndex;
	char			Name[11];
	short			GensID;
};

struct GSP_ANS_GENS_RANK
{
	PBMSG_HEAD2		h;
	short			aIndex;
	char			Name[11];
	short			GensID;
	char			Result;
	short			Rank;
	char			LeadingGens;
};

struct ISHOP_REQ_ITEMLIST
{
	PBMSG_HEAD2		h;
	short			aIndex;
	BYTE			InvType;
	BYTE			InvNum;
	char			AccountID[11];
};

struct ISHOP_ANS_ITEMLIST
{
	PWMSG_HEAD2		h;
	short			aIndex;
	BYTE			InvType;
	BYTE			InvNum;
	char			AccountID[11];
	int				Result;
	int				Count;
};

struct ISHOP_ITEMLIST
{
	int				UniqueCode;
	int				AuthCode;
	int				UniqueID1;
	int				UniqueID2;
	int				UniqueID3;
	BYTE			InventoryType;
	char			GiftName[10];
	char			Message[200];
};

struct ISHOP_ITEM_BUY
{
	PBMSG_HEAD		h;
	short			aIndex;
	char			AccountID[11];
	int				ID1;
	int				ID2;
	int				ID3;
	int				Price;
	BYTE			CoinType;
};

struct ISHOP_ITEM_BUYANS
{
	PBMSG_HEAD		h;
	short			aIndex;
	char			AccountID[11];
	BYTE			Result;
	int				ID1;
	int				ID2;
	int				ID3;
};

struct ISHOP_ITEM_USE
{
	PBMSG_HEAD		h;
	short			aIndex;
	char			AccountID[11];
	int				UniqueCode;
	int				AuthCode;
};

struct ISHOP_ITEM_USEANS
{
	PBMSG_HEAD		h;
	short			aIndex;
	char			AccountID[11];
	BYTE			Result;
	int				UniqueCode;
	int				AuthCode;
	int				ID1;
	int				ID2;
	int				ID3;
};

struct ISHOP_REQ_POINT
{
	PBMSG_HEAD		h;
	short			aIndex;
	char			AccountID[11];
};

struct ISHOP_ANS_POINT
{
	PBMSG_HEAD		h;
	short			aIndex;
	char			AccountID[11];
	BYTE			Result;
	float			CoinP;
	float			CoinC;
	float			Goblin;
};

struct ISHOP_POINT_ADD
{
	PBMSG_HEAD		h;
	short			aIndex;
	char			AccountID[11];
	BYTE			Type;
	float			Coin;
};

struct ISHOP_ITEM_GIFT
{
	PWMSG_HEAD		h;
	short			aIndex;
	char			AccountID[11];
	int				ID1;
	int				ID2;
	int				ID3;
	int				Price;
	BYTE			CoinType;
	char			Name[11];
	char			TargetName[11];
	char			Message[200];
};

struct ISHOP_ITEM_GIFTANS
{
	PBMSG_HEAD		h;
	short			aIndex;
	char			AccountID[11];
	BYTE			Result;
	int				ID1;
	int				ID2;
	int				ID3;
};

struct ISHOP_ITEM_DELETE
{
	PBMSG_HEAD		h;
	char			AccountID[11];
	int				UniqueCode;
	int				AuthCode;
};

struct ISHOP_ITEM_ROLLBACK
{
	PBMSG_HEAD		h;
	char			AccountID[11];
	int				UniqueCode;
	int				AuthCode;
};

struct MUBOT_DATA
{
	PWMSG_HEAD		h;
	short			aIndex;
	char			Name[11];
	BYTE			MuBotData[512];
};

struct BAN_REQ_USER
{
	PBMSG_HEAD		h;
	BYTE			Type;
	BYTE			Ban;
	char			AccName[11];
};

struct PMSG_RESET_INGAME_SEND
{
	PBMSG_HEAD2 h;
	WORD Resets;
};


struct SDHP_VIPINFOSEND
{
	PBMSG_HEAD h;
	int iIndex;
	char AccountId[11];
	char Name[11];
};

struct SDHP_VIPINFORESULT
{
	PBMSG_HEAD h;
	int iIndex;
	char AccountId[11];
	char Name[11];
	BYTE VipType;
};

struct ISHOP_CHARCARD_BUY
{
	PBMSG_HEAD		h;
	char			AccountID[11];
	BYTE			Type;
};

struct ISHOP_ITEM_BUY_PACKAGE
{
	PWMSG_HEAD2		h;
	short			aIndex;
	char			AccountID[11];
	int				Price;
	BYTE			CoinType;
	WORD			Count;
};

struct ISHOP_ITEM_GIFT_PACKAGE
{
	PWMSG_HEAD2		h;
	short			aIndex;
	char			AccountID[11];
	int				Price;
	BYTE			CoinType;
	char			Name[11];
	char			TargetName[11];
	char			Message[200];
	WORD			Count;
};

struct ISHOP_ITEM_PACKAGE
{
	int				ID1;
	int				ID2;
	int				ID3;
};

struct PMSG_ANS_WARESAVE
{
	PBMSG_HEAD		h;
	int				iIndex;
	BYTE			Result;
	BYTE			CloseWindow;
};

struct SECLOCK_REQ_SAVE
{
	PBMSG_HEAD		h;
	char			AccountID[11];
	int				Code;
};

struct ATTRIBUTE_ADD_ERTEL
{
	PBMSG_HEAD2		h;
	char			AccountID[11];
	int				aIndex;
	BYTE			Elemental;
	BYTE			SocketID;
	BYTE			ItemIndex;
	WORD			ItemNumber;
	BYTE			PentagramPos;
	BYTE			ErtelPos;
	BYTE			OptionID[5];
	BYTE			OptionLevel[5];
};

struct ATTRIBUTE_ADD_ERTELANS
{
	PBMSG_HEAD2		h;
	BYTE			Result;
	int				aIndex;
	BYTE			Elemental;
	BYTE			SocketID;
	BYTE			ItemIndex;
	WORD			ItemNumber;
	BYTE			PentagramPos;
	BYTE			ErtelPos;
	BYTE			OptionID[5];
	BYTE			OptionLevel[5];
};

typedef struct
{
	PBMSG_HEAD h;
	unsigned short ServerCode;
	char CharName[MAX_ACCOUNT_LEN + 1];
}SDHP_DELETE_TEMPUSERINFO, *LPSDHP_DELETE_TEMPUSERINFO;

struct SDHP_DELETE_TEMPUSERINFO_RESULT
{
	PBMSG_HEAD h;
	char CharName[MAX_ACCOUNT_LEN + 1];
};
struct DS_GET_MONSTERCNT
{
	PBMSG_HEAD2		h;
	char character[11];
	int cnt;
	short aIndex;
};


struct DS_SAVE_MONSTERCNT
{
	PBMSG_HEAD2		h;
	char character[11];
	int type;
};

struct DS_SAVE_PLAYERKILLER
{
	PBMSG_HEAD2		h;
	char Victim[11];
	char Killer[11];
};


struct PMSG_ANS_ARCA_BATTLE_JOIN
{
	PBMSG_HEAD2 h;
	char btResult;
};

struct PMSG_REQ_ARCA_BATTLE_GUILD_JOIN_DS
{
	PBMSG_HEAD2 h;
	char szGuildMaster[11];
	char szGuildName[9];
	DWORD dwGuild;
	WORD wNumber;
};


struct PMSG_ANS_ARCA_BATTLE_ENTER
{
	PBMSG_HEAD2 h;
	char btResult;
};

struct PMSG_ANS_AB_PROC_STATE_DS
{
	PBMSG_HEAD2 h;
	BYTE btProcState;
};

struct _stGuildUnderMember
{
	BYTE btGuildMemberCnt;
	char szGuildNames[9];
};

/* 5188 */
struct PMSG_ANS_AB_JOIN_MEMBER_UNDER_DS
{
	PBMSG_HEAD2 h;
	BYTE btGuildCnt;
	_stGuildUnderMember GuildMemberCnt[6];
};

struct PMSG_ANS_AB_JOIN_MEMBER_UNDER
{
	PBMSG_HEAD2 h;
	char btMinGuildMember;
	char btGuildMemberCnt;
};

struct PMSG_REQ_AB_GUILD_JOIN_SELECT_DS
{
	PBMSG_HEAD2 h;
	char szGuildMaster[11];
	char szGuildName[9];
	WORD wNumber;
};

struct PMSG_REQ_AB_GUILD_JOIN_SELECT2ND_DS
{
	PBMSG_HEAD2 h;
	WORD wNumber;
	DWORD dwGuild;
};

struct PMSG_REQ_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS
{
	PBMSG_HEAD2 h;
	char szCharName[11];
	char szGuildName[9];
	unsigned int dwGuild;
	WORD wNumber;
};

struct PMSG_REQ_ARCA_BATTLE_ENTER_DS
{
	PBMSG_HEAD2 h;
	char szCharName[11];
	BYTE btEnterSeq;
	WORD wNumber;
};

struct SDHP_REQ_DBMUUN_INVEN_LOAD
{
	PBMSG_HEAD h;
	char AccountID[11];
	char Name[11];
	WORD aIndex;
};

struct SDHP_ANS_CCF_INFO_NOTIFY
{
	PBMSG_HEAD2 h;
	int nMin;
	int nType;
};

struct SDHP_ANS_CCF_GETPERMISSION
{
	PBMSG_HEAD2 h;
	int nResult;
	int nIndex;
};

struct SDHP_REQ_SET_CCF_WINNER_INFO
{
	PBMSG_HEAD2 h;
	char UBFName[MAX_ACCOUNT_LEN + 1];
	BYTE btCCFType;
	BYTE btRewardType;
};

struct PMSG_REQ_SAVE_CHAOSCASTLE_KILLPOINT_UBF
{
	PMSG_REQ_SAVE_CHAOSCASTLE_KILLPOINT_UBF()
	{
		memset(this->szCharName, 0x00, sizeof(this->szCharName));
		this->nPoint = 0;
	}

	PBMSG_HEAD2 h;
	char szCharName[MAX_ACCOUNT_LEN + 1];
	int nPoint;
	int nIndex;
	int nCastleIndex;
};

struct SDHP_ANS_KILLPOINT_RESULT_CC_UBF
{
	PBMSG_HEAD2 h;
	int nIndex;
	int nResult;
	int nCurrentPoint;
	int nTotalPoint;
};

struct _tagSDHP_ANS_DBMUUN_INVEN_LOAD
{
	PWMSG_HEAD h;
	BYTE dbItems[1984];
	char SubEquip;
	WORD aIndex;
};

struct _tagSDHP_REQ_DBMUUN_INVEN_SAVE
{
	PWMSG_HEAD h;
	char AccountID[11];
	char Name[11];
	BYTE dbInventory[1984];
};

struct _tagSDHP_REQ_DBEVENT_INVEN_SAVE
{
	PWMSG_HEAD h;
	char AccountID[11];
	char Name[11];
	BYTE dbInventory[1024];
};

struct _tagSDHP_REQ_DBEVENT_INVEN_LOAD
{
	PBMSG_HEAD h;
	char AccountID[11];
	char Name[11];
	WORD aIndex;
};

struct _tagSDHP_ANS_DBEVENT_INVEN_LOAD
{
	PWMSG_HEAD h;
	BYTE dbItems[1024];
	WORD aIndex;
};

struct PMSG_REQ_SWITCHWARE
{
	PBMSG_HEAD h;
	int iIndex;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	int WarehouseID;
};

struct PMSG_ANS_SWITCHWARE
{
	PBMSG_HEAD h;
	int iIndex;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	int WarehouseID;
	int Result;
};

// UBF (Unity Battle Field aka Battle Core)

struct PMSG_UBF_REGISTER_ACCOUNT_USER
{
	PBMSG_HEAD2 h;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
	char szBattleFieldName[MAX_ACCOUNT_LEN + 1];
	int iUserIndex;
	WORD ServerCode;
	BYTE btRegisterState;
	BYTE btRegisterMonth;
	BYTE btRegisterDay;
};

struct PMSG_UBF_REGISTER_ACCOUNT_USER_RESULT
{
	PBMSG_HEAD2 h;
	int iUserIndex;
	BYTE btResult;
	WORD nLeftSec;
};

struct PMSG_UBF_ACCOUNT_USER_COPY
{
	PBMSG_HEAD2 h;
	int iUserIndex;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
	WORD ServerCode;
	BYTE btPromotionCode;
};

struct PMSG_UBF_ACCOUNT_USER_COPY_RESULT
{
	PBMSG_HEAD2 h;
	int iUserIndex;
	BYTE btResult;
	BYTE btSubResult;
};

struct PMSG_UBF_ACCOUNT_USER_COPY_PETITEM
{
	PWMSG_HEAD h;
	char szAccountID[MAX_ACCOUNT_LEN];
	int iUserIndex;
	BYTE btCount;
	int IsUnityBattleFieldServer;
	int ServerCode;
};

struct PMSG_UBF_COPY_PETITEM
{
	BYTE btItemPos;
	UINT64 i64ItemSerial;
};

struct PMSG_REQ_UBF_ACCOUNT_USERINFO
{
	PBMSG_HEAD2 h;
	int iUserIndex;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
	int iServerCode;
	int IsUnityBattleFieldServer;
	BYTE btObserverMode;
};

struct PMSG_ANS_UBF_ACCOUNT_USERINFO
{
	PBMSG_HEAD2 h;
	int iUserIndex;
	BYTE btResult;
	BYTE btRegisterState;
	BYTE btRegisterMonth;
	BYTE btRegisterDay;
	BYTE btObserverMode;
};

struct PMSG_UBF_REQ_CANCEL_REGISTER_USER
{
	PBMSG_HEAD2 h;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
	int iUserIndex;
	WORD ServerCode;
	BYTE btCanceled;
};

struct PMSG_UBF_ANS_CANCEL_REGISTER_USER
{
	PBMSG_HEAD2 h;
	int iUserIndex;
	BYTE btCanceledResult;
	BYTE btDeletedResult;
};

struct PMSG_REQ_GET_UBF_REAL_NAME
{
	PBMSG_HEAD2 h;
	int iUserIndex;
	char szUBFName[MAX_ACCOUNT_LEN + 1];
};

struct PMSG_ANS_GET_UBF_REAL_NAME
{
	PBMSG_HEAD2 h;
	int iUserIndex;
	char szUBFName[MAX_ACCOUNT_LEN + 1];
	char szRealName[MAX_ACCOUNT_LEN + 1];
	int iServerCode;
	BYTE btReturn;
};

struct PMSG_REQ_UBF_GET_REWARD
{
	PBMSG_HEAD2 h;
	int iUserIndex;
	int iServerCode;
	BYTE btBattleKind;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
};

struct PMSG_ANS_UBF_GET_REWARD
{
	PBMSG_HEAD2 h;
	int iUserIndex;
	BYTE btBattleKind;
	BYTE btReturn;
	BYTE btStage1;
	BYTE btStage2;
	BYTE btStage3;
	BYTE btRewardCount1[2];
	BYTE btRewardCount2[2];
	BYTE btRewardCount3[2];
};

struct PMSG_REQ_UBF_SET_RECEIVED_REWARD
{
	PBMSG_HEAD2 h;
	int iUserIndex;
	int iServerCode;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
	BYTE btReceivedReward;
	BYTE btBattleKind;
};

struct PMSG_ANS_UBF_SET_RECEIVED_REWARD
{
	PBMSG_HEAD2 h;
	int iUserIndex;
	BYTE btReturn;
};

struct PMSG_RECV_CHATDATA_WHISPER
{
	PBMSG_HEAD h;	// C1:02
	WORD wMapSvrNum;
	int OriginGSIndex;
	int OriginPlayerIndex;
	char id[10];
	char fromId[10];
	char chatmsg[90];
};

struct DSMSG_REQ_SUBSERVER_COUNT
{
	PBMSG_HEAD2 h;	// C1:C3:00
	WORD wMapSvrGroup;
};

struct DSMSG_ANS_SUBSERVER_COUNT
{
	PBMSG_HEAD2 h;	// C1:C3:01
	WORD wSvrCount;
};

struct DSMSG_ANS_WHISPER_RESULT
{
	PBMSG_HEAD2 h;	// C1:C3:02
	int OriginGSIndex; // sent back as is
	int OriginPlayerIndex; // sent back as is
	BYTE btResult; // Result 1 - sucess , 0 - fail

};

struct DSMSG_GS_WHISPER_RESULT
{
	PBMSG_HEAD2 h;	// C1:C3:03
	int OriginPlayerIndex; // sent back as is
	BYTE btResult; // Result 1 - sucess , 0 - fail
};

struct DSMSG_CHAOSMACHINE_LOSTITEMS
{
	PWMSG_HEAD h;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
	BYTE btLostItems[CHAOS_BOX_SIZE * MAX_DBITEM_INFO];
};

struct PMSG_RECV_DC_OTHER_CHANNEL
{
	PBMSG_HEAD2 h; // C1:C3:04
	char szName[MAX_ACCOUNT_LEN + 1];
	WORD wMapSrvGroup;
};// Season X

struct PMSG_DSREQ_BLOCKCHAT_LIST
{
	PBMSG_HEAD2 h;
	int iIndex;
	char szName[MAX_ACCOUNT_LEN + 1];
};

struct PMSG_DSANS_BLOCKCHAT_LIST
{
	PWMSG_HEAD2 h;
	int iIndex;
	char szName[MAX_ACCOUNT_LEN + 1];
};

struct PMSG_REQ_EVOMON_MAXSCORE
{
	PBMSG_HEAD2 h;
	int nUserIndex;
	char szName[MAX_ACCOUNT_LEN + 1];
};

struct PMSG_ANS_EVOMON_MAXSCORE
{
	PBMSG_HEAD2 h;
	int nUserIndex;
	int nMaxScore;
};

struct PMSG_REQ_SAVE_EVOMON_RESULT
{
	PBMSG_HEAD2 h;
	int nUserIndex;
	char szName[MAX_ACCOUNT_LEN + 1];
	int nScore;
	int nTotalDamage;
};

typedef struct
{
	PWMSG_HEAD h;
	int aIndex;
	char szName[11];
	BYTE btDATA[512];
}MUBOT_SETTINGS_SEND, *LPMUBOT_SETTINGS_SEND;

struct PMSG_ANTIHACK_BREACH
{
	PBMSG_HEAD h;
	BYTE subcode;
	DWORD dwErrorCode;
};

struct PMSG_ANTIHACK_CHECK
{
	PBMSG_HEAD h;
	BYTE subcode;
	BYTE checkdata[5];
};

struct PMSG_ATTACKSPEEDSEND
{
	PBMSG_HEAD2		h;
	DWORD			AttackSpeed;
	DWORD			MagicSpeed;
};

struct PMSG_PROPERTYITEMCOUNT
{
	PBMSG_HEAD2		h;
	BYTE			btCount;
};

struct PMSG_REQ_USETRANSFORMATIONRING
{
	PBMSG_HEAD2		h;
	BYTE			btState;
};

struct PMSG_PROPERTYITEMINFO
{
	PBMSG_HEAD2		h;
	short			ItemID;
	short			Pos;
	int				Time;
};

struct PMSG_EX_GAMESERVER_MOVE_SEND
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btGameServerCode;
};

struct PMSG_EX_GAMESERVER_MOVE_RESULT
{
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_AGILITYSEND
{
	PBMSG_HEAD h;
	BYTE subcode;
	WORD Agility;
};

struct PMSG_ANS_MAPSVRTAXINFO
{
	PBMSG_HEAD2 h;	// C1:B2:1A
	BYTE btTaxType;	// 4
	BYTE btTaxRate;	// 5
};


struct PMSG_ADDSTATS
{
	PBMSG_HEAD h;
	BYTE subcode;
	BYTE statstype;
	WORD Points;
	WORD LUP;
	int MaxLifeAndMana;
};



struct PMSG_MONEY
{
	PBMSG_HEAD h;	// C1:81
	BYTE Result;	// 3
	int wMoney;	// 4
	int iMoney;	// 8
};

struct PMSG_ATTACK
{
	PBMSG_HEAD h;
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	BYTE AttackAction;	// 5
	BYTE DirDis;	// 6
};

struct PMSG_MAGICATTACK
{
	PBMSG_HEAD h;
	BYTE NumberH;
	BYTE MagicNumberH;
	BYTE NumberL;
	BYTE MagicNumberL;
	BYTE Dis;
};

struct PMSG_DURATION_MAGIC_RECV
{
	PBMSG_HEAD h;
	BYTE X;
	BYTE MagicNumberH;
	BYTE Y;
	BYTE MagicNumberL;
	BYTE Dir;
	BYTE NumberH;
	BYTE Dis;
	BYTE NumberL;
	BYTE TargetPos;
	BYTE MagicKey;
};

struct PMSG_BEATTACK_COUNT
{
	PBMSG_HEAD h;	// C1:D7
	BYTE MagicNumberH;
	BYTE Count;
	BYTE MagicNumberL;
	BYTE X;
	BYTE Serial;
	BYTE Y;
};

struct PMSG_BEATTACK
{
	BYTE NumberH;	// 0
	BYTE MagicKey;	// 1
	BYTE NumberL;	// 2
};

struct PMSG_ACTIONRESULT
{
	PBMSG_HEAD h;
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	BYTE Dir;	// 5
	BYTE ActionNumber;	// 6
	BYTE TargetNumberH;	// 7
	BYTE TargetNumberL;	// 8
};


struct PMSG_RAGEATTACK_REQ
{
	PBMSG_HEAD h;
	BYTE NumberH;
	BYTE MagicNumberH;
	BYTE NumberL;
	BYTE MagicNumberL;
	BYTE Empty;
};


struct PMSG_KILLPLAYER_EXT
{
	PBMSG_HEAD h;	// C3:9C
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	WORD ExpH;	// 6
	WORD ExpL;	// 8
	BYTE DamageH;	// A
	BYTE DamageL;	// B
};

struct PMSG_ANS_END_DUEL
{
	PBMSG_HEAD h;	// C1:AB
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	char szName[10];	// 5
};

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Send Warehouse Info Packet
*	Direction : GameServer -> Client
*  Code      : 0xC3
*	HeadCode  : 0x30
*/
struct PMSG_TALKRESULT
{
	PBMSG_HEAD h;
	unsigned char result;	// 3
	BYTE level1;	// 4
	BYTE level2;	// 5
	BYTE level3;	// 6
	BYTE level4;	// 7
	BYTE level5;	// 8
	BYTE level6;	// 9
	BYTE level7;
};


struct PMSG_SUMMONER_INFO
{
	PBMSG_HEAD2 h;
	BYTE Result;
};

/* * * * * * * * * * * * * * * * * * * * *
*	Packet to Send Blod Castle State
*	Direction : GameServer -> Client
*	Code      : 0xC1
*	HeadCode  : 0x9B
*/
struct PMSG_STATEBLOODCASTLE
{
	PBMSG_HEAD h;	// C1:9B
	BYTE btPlayState;		// 3
	WORD wRemainSec;	// 4
	WORD wMaxKillMonster;	// 6
	WORD wCurKillMonster;	// 8
	WORD wUserHaveWeapon;	// A
	BYTE btWeaponNum;	// C
};

struct PMSG_KILLCOUNT
{
	PBMSG_HEAD2 h;
	BYTE btKillCount;
};

struct SDHP_DBCHARINFOREQUEST
{
	PBMSG_HEAD h;	// C1:06
	char AccountID[11];	// 3
	char Name[11];	// E
	short Number;	// 1A
};

struct PMSG_ATTACKRESULT
{
	PBMSG_HEAD h;	// C1:DC
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	BYTE DamageH;	// 5
	BYTE DamageL;	// 6
	BYTE DamageTypeH;
	BYTE DamageTypeL;	// 7
	BYTE btShieldDamageH;	// 8
	BYTE btShieldDamageL;	// 9
	int IGCDamage;
	int ElementalDmg;
};

struct PMSG_ELEMENTALDAMAGE
{
	PBMSG_HEAD h;
	BYTE NumberH;
	BYTE NumberL;
	BYTE btElement;
	BYTE btTargetH;
	BYTE btTargetL;
	int Damage;
	int New;
	int New1;
	int New2;
};


struct PMSG_PENTAGRAMJEWEL_INFO
{
	PWMSG_HEAD2 h;
	BYTE btResult;
	BYTE btJewelCnt;
	BYTE btJewelPos;
};

struct PMSG_REQ_IN_PENTAGRAM_JEWEL
{
	PBMSG_HEAD2 h;
	int iPentagramPos;
	int iJewelPos;
};

struct PMSG_ANS_IN_PENTAGRAM_JEWEL
{
	PBMSG_HEAD2 h;
	BYTE btResult;
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
};

struct PMSG_PENTAGRAM_JEWEL_INOUT
{
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_REQ_OUT_PENTAGRAM_JEWEL
{
	PBMSG_HEAD2 h;
	int iPentagramPos;
	BYTE btSocketIndex;
};

struct PMSG_ANS_OUT_PENTAGRAM_JEWEL
{
	PBMSG_HEAD2 h;
	BYTE Result;
	BYTE btJewelPos;
	BYTE btJewelDBIndex;
};


struct PMSG_RECV_POSISTION_SET
{
	PBMSG_HEAD h;	// C1:D6
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	BYTE X;	// 5
	BYTE Y;	// 6
};

struct PMSG_USEREQUIPMENTCHANGED
{
	PBMSG_HEAD h;
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	BYTE ItemInfo[MAX_ITEM_INFO];	// 5
	BYTE Element;
};

struct PMSG_SERVERCMD
{
	PBMSG_HEAD h;
	BYTE subcode;	// 3
	BYTE CmdType;	// 4
	BYTE X;	// 5
	BYTE Y;	// 6
};

struct PMSG_ITEMTHROW_RESULT
{
	PBMSG_HEAD h;	// C1:23
	unsigned char Result;	// 3
	BYTE Ipos;	// 4
};

struct PMSG_INVENTORYLISTCOUNT
{
	PWMSG_HEAD h;	// C4:F3:10
	BYTE subcode;	// 4
	BYTE Count;	// 5
};

struct PMSG_INVENTORYLIST
{
	BYTE Pos;	// 0
	BYTE ItemInfo[MAX_ITEM_INFO];	// 1
};

struct PMSG_GUILDCREATED_RESULT
{
	PBMSG_HEAD h;	// C1:56
	BYTE Result;	// 3
	BYTE btGuildType;	// 4
};

struct PMSG_SIMPLE_GUILDVIEWPORT_COUNT
{
	PWMSG_HEAD h;	// C2:65
	BYTE Count;	// 4
};

struct PMSG_SIMPLE_GUILDVIEWPORT
{
	int GuildNumber;	// 0
	BYTE btGuildStatus;	// 4
	BYTE btGuildType;	// 5
	BYTE btGuildRelationShip;	// 6
	BYTE NumberH;	// 7
	BYTE NumberL;	// 8
	BYTE btOwnerStatus; //9 season4 add-on
};

struct PMSG_RECVMOVE
{
	PBMSG_HEAD h;
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	BYTE X;	// 5
	BYTE Y;	// 6
	BYTE Path;	// 7
};

struct PMSG_MAGICATTACK_RESULT
{
	PBMSG_HEAD h;
	BYTE SourceNumberH;
	BYTE SourceNumberL;
	BYTE MagicNumberH;
	BYTE MagicNumberL;
	BYTE TargetNumberH;
	BYTE TargetNumberL;
};

struct PMSG_SET_DEVILSQUARE
{
	PBMSG_HEAD h;	// C1:92
	BYTE Type;	// 3
};

struct PMSG_GUILD_ASSIGN_STATUS_RESULT
{
	PBMSG_HEAD h;	// C1:E1
	BYTE btType;	// 3
	BYTE btResult;	// 4
	char szTagetName[10];	// 5
};

struct PMSG_GUILD_ASSIGN_TYPE_RESULT
{
	PBMSG_HEAD h;	// C1:E2
	BYTE btGuildType;	// 3
	BYTE btResult;	// 4
};

//////////////////////////////////////////////////////////////////////////////
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//////////////////////////////////////////////////////////////////////////////




struct PMSG_AHINFO
{
	PBMSG_HEAD h;
	BYTE SUB;
	DWORD Time;	// 4
	WORD AttackSpeed;	// 8
	WORD MagicSpeed;	// A
	char dump1[15];
	BYTE IsModifiedGameSpeed;
	char dump2[15];
	BYTE IsModifiedAttackSpeed;
	char dump3[15];
	BYTE IsModifiedMagicSpeed;
	char dump4[15];
};

struct PMSG_CHECK_MAINEXE_RESULT
{
	PBMSG_HEAD2 h;
	DWORD m_dwKey;	// 4
};




struct PMSG_CHATDATA
{
	PBMSG_HEAD h;	//	
	char chatid[10];	//	3
	char chatmsg[90];	//	D
};


struct PMSG_CHATDATA_NUMBER
{
	PBMSG_HEAD h;
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	char chatmsg[90];	// 5
};

struct PMSG_CHATDATA_WHISPER
{
	PBMSG_HEAD h;	// C1:02
	char id[10];	// 3
	char chatmsg[90];	// D  
};


struct PMSG_CHARMAPJOIN
{
	PBMSG_HEAD h;
	BYTE subcode;	// 3
	char Name[10];	// 4
	BYTE btSlotIndex;
};


struct PMSG_LVPOINTADD
{
	PBMSG_HEAD h;
	BYTE subcode;	// 3
	BYTE Type;	// 4
};

struct PMSG_LVPOINTADDRESULT
{
	PBMSG_HEAD h;	// C1:F3:06
	BYTE subcode;	// 3
	BYTE ResultType;	// 4
	WORD MaxLifeAndMana;	// 6
	WORD wMaxShield;	// 8
	WORD MaxBP;	// A
	int IGCMaxLifeAndMana;
};

struct PMSG_SKILLKEY
{
	PBMSG_HEAD h;	// C1:F3:30
	BYTE subcode;	// 3
	BYTE SkillKey[20];	// 4
	BYTE GameOption;	// E
	BYTE QkeyDefine;	// F
	BYTE WkeyDefine;	// 10
	BYTE EkeyDefine;	// 11
	BYTE ChatWindow;	// 13
	BYTE RkeyDefine;
	int QWERLevel;
};

struct PMSG_EX_SKILL_COUNT
{
	PBMSG_HEAD h;	// C1:BA
	BYTE NumberH;
	BYTE NumberL;
	WORD Type;
	BYTE Count;
};

struct PMSG_ITEMGETREQUEST
{
	PBMSG_HEAD h;
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
};


struct PMSG_ITEMTHROW
{
	PBMSG_HEAD h;
	BYTE px;	// 3
	BYTE py;	// 4
	BYTE Ipos;	// 5
};


struct PMSG_INVENTORYITEMMOVE
{
	PBMSG_HEAD h;
	BYTE sFlag;	// 3
	BYTE source_item_num;	// 4
	BYTE sItemInfo[MAX_ITEM_INFO];	// 5
	BYTE tFlag;	// C
	BYTE target_item_num;	// D
};


struct PMSG_TALKREQUEST
{
	PBMSG_HEAD h;
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
};


struct PMSG_BUYREQUEST
{
	PBMSG_HEAD h;
	BYTE Pos;	// 3
};


struct PMSG_SELLREQUEST
{
	PBMSG_HEAD h;
	BYTE Pos;	// 3
};


struct PMSG_ITEMDURREPAIR
{
	PBMSG_HEAD h;
	BYTE Position;	// [0xFF:Repair ALL] 3
	BYTE Requestpos;	// 4
};


struct PMSG_TRADE_REQUEST
{
	PBMSG_HEAD h;
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
};

struct PMSG_USE_STAT_FRUIT
{
	PBMSG_HEAD h;
	BYTE result;
	WORD btStatValue;
	BYTE btFruitType;
};

struct PMSG_TRADE_RESPONSE
{
	PBMSG_HEAD h;	// C1:37
	BYTE Response;	// 3
	BYTE Id[10];	// 4
	WORD Level;	// E
	int GuildNumber;	// 10
};


struct PMSG_TRADE_GOLD
{
	PBMSG_HEAD h;
	DWORD Money;	// 4
};


struct PMSG_TRADE_OKBUTTON
{
	PBMSG_HEAD h;	// C1:3C
	BYTE Flag;	// 3
};

struct PMSG_PARTYREQUEST
{
	PBMSG_HEAD h;
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
};


struct PMSG_PARTYREQUESTRESULT
{
	PBMSG_HEAD h;
	BYTE Result;	// 3
	BYTE NumberH;	// 4
	BYTE NumberL;	// 5
};


struct PMSG_PARTYDELUSER
{
	PBMSG_HEAD h;
	BYTE Number;	// 3
	char szTargetName[MAX_ACCOUNT_LEN + 1];
};

struct PMSG_PARTYLIST
{
	char szId[10];	// 0
	BYTE Number;	// A
	BYTE MapNumber;	// B
	BYTE X;	// C
	BYTE Y;	// D
	int Life;	// 10
	int MaxLife;	// 14
	int nServerChannel;
	int nMana;
	int nMaxMana;
	int Unk;
};

struct PMSG_PARTYLISTCOUNT
{
	PBMSG_HEAD h;	// C1:42
	BYTE Result;	// 3
	BYTE Count;	// 4
};


struct PMSG_GUILDJOINQ
{
	PBMSG_HEAD h;
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
};


struct PMSG_GUILDQRESULT
{
	PBMSG_HEAD h;
	BYTE Result;	// 3
	BYTE NumberH;	// 4
	BYTE NumberL;	// 5
};


struct PMSG_GUILDDELUSER
{
	PBMSG_HEAD h;
	char Name[10];	// 3
	char Password[20];	// D
};


struct PMSG_GUILDMASTERANSWER
{
	PBMSG_HEAD h;
	BYTE Result;	// 3
};


struct PMSG_GUILDINFOSAVE
{
	PBMSG_HEAD h;
	BYTE btGuildType;	// 3
	char GuildName[8];	// 4
	BYTE Mark[32];	// C
};


struct PMSG_GUILDWARSEND_RESULT
{
	PBMSG_HEAD h;
	BYTE Result;	// 3
};


struct PMSG_WAREHOUSEMONEYINOUT
{
	PBMSG_HEAD h;
	BYTE Type;	// [0x01:Deposit] [0x02:Withdraw] 3
	int Money;	// 4
};


struct PMSG_WAREHOUSEPASSSEND
{
	PBMSG_HEAD h;
	BYTE Type;	// [0x00:Open Warehouse] [0x01:Set Pass] [0x02:Delete Pass] 3
	short Pass;	// 4
	char LastJoominNumber[20];	// 6
};

struct PMSG_CHAOSMIX
{
	PBMSG_HEAD h;
	BYTE Type;	// 3
	BYTE Pos;	// 4
	BYTE unk1;
	BYTE MixCount;
};

struct PMSG_MOVE
{
	PBMSG_HEAD h;	// C1:1D
	BYTE X;	// 3
	BYTE Y;	// 4
	BYTE Path[8];	// 5
};

struct PMSG_POSISTION_SET
{
	PBMSG_HEAD h;
	BYTE X;	// 3
	BYTE Y;	// 4
};

struct PMSG_ACTION
{
	PBMSG_HEAD h;
	BYTE Dir;	// 3
	BYTE ActionNumber;	// 4
	BYTE iTargetIndexH;	// 5
	BYTE iTargetIndexL;	// 6
};

struct PMSG_MAGICCANCEL
{
	PBMSG_HEAD h;
	BYTE MagicNumberL;
	BYTE MagicNumberH;	// 3
	BYTE NumberH;	// 4
	BYTE NumberL;	// 5
};


struct PMSG_TELEPORT
{
	PBMSG_HEAD h;
	WORD MoveNumber;	// 3
	BYTE MapX;	// 4
	BYTE MapY;	// 5
};

struct PMSG_MAPMOVE_CHECKSUM
{
	PBMSG_HEAD2 h;
	DWORD dwKeyValue;
};


struct PMSG_REQ_MAPMOVE
{
	PBMSG_HEAD2 h;
	DWORD dwBlockKey;
	WORD wMapIndex;
	BYTE EventByte;
};


struct PMSG_ANS_MAPMOVE
{
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_TARGET_TELEPORT
{
	PBMSG_HEAD h;
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	BYTE MapX;	// 5
	BYTE MapY;	// 6
};

struct PMSG_DURATION_MAGIC_SEND
{
	PBMSG_HEAD h;
	BYTE X;
	BYTE Y;
	BYTE Dir;
	BYTE MagicNumberH;
	BYTE NumberH;
	BYTE MagicNumberL;
	BYTE NumberL;

};

struct PMSG_USEITEM
{
	PBMSG_HEAD h;
	BYTE inventoryPos;	// 3
	BYTE invenrotyTarget;	// 4
	BYTE btItemUseType;	// 5
};

struct PMSG_USEEXPANSIONITEM
{
	PBMSG_HEAD h;
	BYTE Result;
};

struct PMSG_REQ_MOVEDEVILSQUARE
{
	PBMSG_HEAD h;
	BYTE SquareNumber;	// 3
	BYTE InvitationItemPos;	// 4
};


struct PMSG_REQ_DEVILSQUARE_REMAINTIME
{
	PBMSG_HEAD h;
	BYTE hEventType;	// 3
	BYTE btItemLevel;	// 4
};


struct PMSG_PING_RESULT
{
	PBMSG_HEAD h;
};


struct PMSG_REGEVENTCHIP
{
	PBMSG_HEAD h;
	BYTE Type;	// 3
	BYTE ChipPos;	// 4
};


struct PMSG_GETMUTONUMBER
{
	PBMSG_HEAD h;
};


struct PMSG_EXCHANGE_EVENTCHIP
{
	PBMSG_HEAD h;
	BYTE btType;	// [0x00:Change Renas] [0x01:Change Stones]	3
};

struct PMSG_SETQUEST
{
	PBMSG_HEAD h;	// C1:A1
	BYTE QuestIndex;	// 3
	BYTE State;	// 4
};

struct PMSG_RESULT_MOVEBLOODCASTLE
{
	PBMSG_HEAD h;	// C1:9A
	BYTE Result;	// 3
};

struct PMSG_REQ_MOVEBLOODCASTLE
{
	PBMSG_HEAD h;
	BYTE iBridgeNumber;	// 3
	BYTE iItemPos;	// 4
};

struct PMSG_ANS_ENTER_ON_QUESTNPC
{
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_REQ_MOVECHAOSCASTLE
{
	PBMSG_HEAD2 h;
	BYTE iChaosCastleNumber;	// 4
	BYTE iItemPos;	// 5
};
struct PMSG_REQ_MOVEILLUSIONTEMPLE
{
	PBMSG_HEAD2 h;
	BYTE ID;
	BYTE iItemPos;
};
struct PMSG_REQ_USEILLUSIONTEMPLESKILL
{
	PBMSG_HEAD2 h;
	BYTE MagicNumberH;
	BYTE MagicNumberL;
	BYTE btTargetObjIndexH;
	BYTE btTargetObjIndexL;
	BYTE btDis;
};

struct PMSG_REQ_REPOSUSER_IN_CC
{
	PBMSG_HEAD2 h;
	BYTE btPX;	// 4
	BYTE btPY;	// 5
};


struct PMSG_REQ_CL_ENTERCOUNT
{
	PBMSG_HEAD h;
	BYTE btEventType;	// 3
};


struct PMSG_REQ_2ANV_LOTTO_EVENT
{
	PBMSG_HEAD h;
	char SERIAL1[5];	// 3
	char SERIAL2[5];	// 8
	char SERIAL3[5];	// D
};


struct PMSG_REQ_MOVE_OTHERSERVER
{
	PBMSG_HEAD h;
	char LastJoominNumber[20];	// 3
};


struct PMSG_PACKETCHECKSUM
{
	PBMSG_HEAD h;
	BYTE funcindex;	// 3
	DWORD CheckSum;	// 4
};

struct PMSG_REQ_START_DUEL
{
	PBMSG_HEAD h;
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	char szName[10];	// 5
};


struct PMSG_REQ_END_DUEL
{
	PBMSG_HEAD h;
};



struct PMSG_ANS_DUEL_OK
{
	PBMSG_HEAD h;
	bool bDuelOK;	// 3
	BYTE NumberH;	// 4
	BYTE NumberL;	// 5
	char szName[10];	// 6
};


struct PMSG_REQUEST_PET_ITEM_COMMAND
{
	PBMSG_HEAD h;
	BYTE PetType;	// 3
	BYTE Command;	// 4
	BYTE NumberH;	// 5
	BYTE NumberL;	// 6
};


struct PMSG_REQUEST_PET_ITEMINFO
{
	PBMSG_HEAD h;
	BYTE PetType;	// 3
	BYTE InvenType;	// 4
	BYTE nPos;	// 5
};

struct PMSG_SEND_PCPOINT
{
	PBMSG_HEAD2 h;
	WORD PCPoint;
	WORD MaxPCPoint;
};

struct PMSG_REQ_OPEN_PCPOINTSHOP
{
	PBMSG_HEAD2 h;
	BYTE Result;
};

struct PMSG_REQ_BUY_PCPOINTSHOP
{
	PBMSG_HEAD2 h;
	BYTE Pos;
};

struct PMSG_BUYRESULT
{
	PBMSG_HEAD h;	// C1:32
	BYTE Result;	// 3
	BYTE ItemInfo[MAX_ITEM_INFO];	// 4
};

struct PMSG_REQ_GUILDVIEWPORT
{
	PBMSG_HEAD h;
	int GuildNumber;	// 4
};

struct PMSG_SHOPITEMCOUNT
{
	PWMSG_HEAD h;	// C2:31
	BYTE Type;	// 4
	BYTE count;	// 5
};

struct PMSG_GUILD_ASSIGN_STATUS_REQ
{
	PBMSG_HEAD h;
	BYTE btType;	// 3
	BYTE btGuildStatus;	// 4
	char szTagetName[10];	// 5
};


struct PMSG_GUILD_ASSIGN_TYPE_REQ
{
	PBMSG_HEAD h;
	BYTE btGuildType;	// 3
};


struct PMSG_RELATIONSHIP_JOIN_BREAKOFF_REQ
{
	PBMSG_HEAD h;	// C1:E5
	BYTE btRelationShipType;	// 3
	BYTE btRequestType;	// 4
	BYTE btTargetUserIndexH;	// 5
	BYTE btTargetUserIndexL;	// 6
};


struct PMSG_RELATIONSHIP_JOIN_BREAKOFF_ANS
{
	PBMSG_HEAD h;	// C1:E6
	BYTE btRelationShipType;	// 3
	BYTE btRequestType;	// 4
	BYTE btResult;	// 5
	BYTE btTargetUserIndexH;	// 6
	BYTE btTargetUserIndexL;	// 7
};


struct PMSG_UNIONLIST_REQ {




	PBMSG_HEAD h;










};

struct PMSG_KICKOUT_UNIONMEMBER_REQ
{
	PBMSG_HEAD2 h;
	char szTargetGuildName[8];	// 4
};


struct PMSG_REQ_MAPSERVERAUTH
{
	PBMSG_HEAD2 h;
	char szAccountID[12];	// 4
	char szCharName[12];	// 10
	int iJoinAuthCode1;	// 1C
	int iJoinAuthCode2;	// 20
	int iJoinAuthCode3;	// 24
	int iJoinAuthCode4;	// 28
	int iTickCount;	// 2C
	BYTE btCliVersion[5];	// 30
	BYTE btCliSerial[16];	// 35
	BYTE EventMap;
};

struct PMSG_REQ_CASTLESIEGESTATE
{
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_REGCASTLESIEGE
{
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_GIVEUPCASTLESIEGE
{
	PBMSG_HEAD2 h;
	BYTE btGiveUp;
};

struct PMSG_REQ_GUILDREGINFO
{
	PBMSG_HEAD2 h;
};

struct CSP_ANS_GUILDREGINFO
{
	PBMSG_HEAD2 h;
	int iResult;	// 4
	WORD wMapSvrNum;	// 8
	class CGameObject *lpObj;	// C
	char szGuildName[8];	// 10
	int iRegMarkCount;	// 18
	BYTE bIsGiveUp;	// 1C
	BYTE btRegRank;	// 1D
};

struct PMSG_REQ_REGGUILDMARK
{
	PBMSG_HEAD2 h;
	BYTE btItemPos;
};

struct CSP_ANS_GUILDREGMARK
{
	struct PBMSG_HEAD2 h;
	int iResult;
	WORD wMapSvrNum;
	class CGameObject *lpObj;
	char szGuildName[8];
	int iItemPos;
	int iRegMarkCount;
};

struct PMSG_REQ_NPCBUY
{
	PBMSG_HEAD2 h;
	int iNpcNumber;
	int iNpcIndex;
};

struct PMSG_REQ_NPCREPAIR
{
	PBMSG_HEAD2 h;
	int iNpcNumber;
	int iNpcIndex;
};

struct PMSG_REQ_NPCUPGRADE
{
	PBMSG_HEAD2 h;
	int iNpcNumber;
	int iNpcIndex;
	int iNpcUpType;
	int iNpcUpValue;
};

struct PMSG_REQ_TAXMONEYINFO
{
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_TAXRATECHANGE
{
	PBMSG_HEAD2 h;
	BYTE btTaxType;
	BYTE btTaxRate1;
	BYTE btTaxRate2;
	BYTE btTaxRate3;
	BYTE btTaxRate4;
};

struct PMSG_REQ_MONEYDRAWOUT
{
	PBMSG_HEAD2 h;
	BYTE btMoney1;
	BYTE btMoney2;
	BYTE btMoney3;
	BYTE btMoney4;
};

struct PMSG_REQ_CSGATEOPERATE
{
	PBMSG_HEAD2 h;
	BYTE btOperate;
	BYTE btIndex1;
	BYTE btIndex2;
};

struct PMSG_REQ_MINIMAPDATA
{
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_STOPMINIMAPDATA
{
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_CSCOMMAND
{
	PBMSG_HEAD2 h;
	BYTE btTeam;
	BYTE btX;
	BYTE btY;
	BYTE btCommand;
};

struct PMSG_REQ_CSHUNTZONEENTER
{
	PBMSG_HEAD2 h;
	BYTE btHuntZoneEnter;
};

struct PMSG_REQ_NPCDBLIST
{
	PBMSG_HEAD h;
	BYTE btMonsterCode;
};

struct PMSG_REQ_CSREGGUILDLIST
{
	PBMSG_HEAD h;
};

struct PMSG_REQ_CSATTKGUILDLIST
{
	PBMSG_HEAD h;
};

struct PMSG_REQ_USEWEAPON
{
	PBMSG_HEAD2 h;
	BYTE btObjIndexH;
	BYTE btObjIndexL;
	BYTE btTargetIndex;
};

struct PMSG_REQ_WEAPON_DAMAGE_VALUE
{
	PBMSG_HEAD2 h;
	BYTE btObjIndexH;
	BYTE btObjIndexL;
};

struct PMSG_REQ_GUILDMARK_OF_CASTLEOWNER
{
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_JEWEL_MIX
{
	PBMSG_HEAD2 h;
	BYTE btJewelType;
	BYTE btJewelMix;
};

struct PMSG_REQ_JEWEL_UNMIX
{
	PBMSG_HEAD2 h;
	BYTE btJewelType;
	BYTE btJewelLevel;
	BYTE btJewelPos;
};

struct PMSG_REQ_CRYWOLF_INFO
{
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_CRYWOLF_ALTAR_CONTRACT
{
	PBMSG_HEAD2 h;
	BYTE btObjIndexH;
	BYTE btObjIndexL;
};

struct PMSG_REQ_CRYWOLF_BENEFIT_PLUS_CHAOSRATE
{
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_KANTURU_STATE_INFO
{
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_ENTER_KANTURU_BOSS_MAP
{
	PBMSG_HEAD2 h;
};

struct CSP_ANS_CSINITDATA
{
	PWMSG_HEAD h;	// 0
	int iResult;		// 4
	WORD wMapSvrNum;	// 8
	WORD wStartYear;	// a
	BYTE btStartMonth;	// c
	BYTE btStartDay;	// d
	WORD wEndYear; // e
	BYTE btEndMonth; // 10
	BYTE btEndDay;	// 11
	BYTE btIsSiegeGuildList;	// 12
	BYTE btIsSiegeEnded;	// 13
	BYTE btIsCastleOccupied;	// 14
	char szCastleOwnGuild[8]; // 15
	__int64 i64CastleMoney; // 20
	int iTaxRateChaos; // 28
	int iTaxRateStore; // 2c
	int iTaxHuntZone; // 30
	int iFirstCreate; // 34
	int iCount; // 38
};

struct CSP_CSINITDATA
{
	int iNpcNumber; // 0
	int iNpcIndex; // 4
	int iNpcDfLevel; // 8
	int iNpcRgLevel; // c
	int iNpcMaxHp; // 10
	int iNpcHp; // 14
	BYTE btNpcX; // 18
	BYTE btNpcY; // 19
	BYTE btNpcDIR; // 1a
};

struct CSP_CALCREGGUILDLIST
{
	char szGuildName[8]; // 0
	int iRegMarkCount; // 8
	int iGuildMemberCount; // c
	int iGuildMasterLevel; // 10
	int iSeqNum; // 14
};

struct CSP_CSGUILDUNIONINFO
{
	char szGuildName[8]; // 0
	int iCsGuildID; // 8
};

struct CSP_CSLOADTOTALGUILDINFO
{
	char szGuildName[8]; // 0
	int iCsGuildID; // 8
	int iGuildInvolved; // c
	int iGuildScore; //10 season 2.5 add-on
};

struct PMSG_CSATTKGUILDLIST
{
	BYTE btCsJoinSide; // 0
	BYTE btGuildInvolved; // 1
	char szGuildName[8]; // 2
	int iGuildScore; //A season 2.5 add-on
};

struct CSP_ANS_NPCDATA
{
	PWMSG_HEAD h; // 0
	int iResult; // 4
	WORD wMapSvrNum; // 8
	class CGameObject *lpObj; // c
	int iCount; // 10
};

struct PMSG_ANS_NOTIFYCSSTART
{
	PBMSG_HEAD2 h; // 0
	BYTE btStartState; // 4
};

struct PMSG_REQ_MOVE_TO_CASTLE_HUNTZONE
{
	PBMSG_HEAD2 h;
	int iPrice;
};

struct PMSG_ANS_CASTLESIEGESTATE
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	char cCastleSiegeState;
	BYTE btStartYearH;
	BYTE btStartYearL;
	BYTE btStartMonth;
	BYTE btStartDay;
	BYTE btStartHour;
	BYTE btStartMinute;
	BYTE btEndYearH;
	BYTE btEndYearL;
	BYTE btEndMonth;
	BYTE btEndDay;
	BYTE btEndHour;
	BYTE btEndMinute;
	BYTE btSiegeStartYearH;
	BYTE btSiegeStartYearL;
	BYTE btSiegeStartMonth;
	BYTE btSiegeStartDay;
	BYTE btSiegeStartHour;
	BYTE btSiegeStartMinute;
	char cOwnerGuild[8];
	char cOwnerGuildMaster[10];
	BYTE btStateLeftSec1;
	BYTE btStateLeftSec2;
	BYTE btStateLeftSec3;
	BYTE btStateLeftSec4;
};

struct PMSG_ANS_REGCASTLESIEGE
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	char szGuildName[8];
};

struct PMSG_ANS_GIVEUPCASTLESIEGE
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btIsGiveUp;
	char szGuildName[8];
};

struct PMSG_ANS_GUILDREGINFO
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	char szGuildName[8];
	BYTE btGuildMark1;
	BYTE btGuildMark2;
	BYTE btGuildMark3;
	BYTE btGuildMark4;
	BYTE btIsGiveUp;
	BYTE btRegRank;
};

struct PMSG_ANS_REGGUILDMARK
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	char szGuildName[8];
	BYTE btGuildMark1;
	BYTE btGuildMark2;
	BYTE btGuildMark3;
	BYTE btGuildMark4;
};

struct PMSG_ANS_NPCBUY
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	int iNpcNumber;
	int iNpcIndex;
};

struct PMSG_ANS_NPCREPAIR
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	int iNpcNumber;
	int iNpcIndex;
	int iNpcHP;
	int iNpcMaxHP;
};

struct PMSG_ANS_NPCUPGRADE
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	int iNpcNumber;
	int iNpcIndex;
	int iNpcUpType;
	int iNpcUpValue;
};

struct PMSG_ANS_TAXMONEYINFO
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btTaxRateChaos;
	BYTE btTaxRateStore;
	BYTE btMoney1;
	BYTE btMoney2;
	BYTE btMoney3;
	BYTE btMoney4;
	BYTE btMoney5;
	BYTE btMoney6;
	BYTE btMoney7;
	BYTE btMoney8;
};

struct PMSG_ANS_TAXRATECHANGE
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btTaxType;
	BYTE btTaxRate1;
	BYTE btTaxRate2;
	BYTE btTaxRate3;
	BYTE btTaxRate4;
};

struct PMSG_ANS_MONEYDRAWOUT
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btMoney1;
	BYTE btMoney2;
	BYTE btMoney3;
	BYTE btMoney4;
	BYTE btMoney5;
	BYTE btMoney6;
	BYTE btMoney7;
	BYTE btMoney8;
};

struct PMSG_ANS_CSGATESTATE
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btIndex1;
	BYTE btIndex2;
};

struct PMSG_ANS_CSGATEOPERATE
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btOperate;
	BYTE btIndex1;
	BYTE btIndex2;
};

struct PMSG_ANS_CSGATECURSTATE
{
	PBMSG_HEAD2 h;
	BYTE btOperate;
	BYTE btIndex1;
	BYTE btIndex2;
};

struct PMSG_ANS_NOTIFYSWITCHPROC
{
	PBMSG_HEAD2 h;
	BYTE btIndex1;
	BYTE btIndex2;
	BYTE btUserIndex1;
	BYTE btUserIndex2;
	BYTE btSwitchState;
};

struct PMSG_ANS_NOTIFYCROWNPROC
{
	PBMSG_HEAD2 h;
	BYTE btCrownState;
	DWORD dwAccumulatedCrownAccessTime;
};

struct PMSG_ANS_CSCOMMAND
{
	PBMSG_HEAD2 h;
	BYTE btTeam;
	BYTE btX;
	BYTE btY;
	BYTE btCommand;
};

struct PMSG_ANS_NPCDBLIST
{
	PWMSG_HEAD h;
	BYTE btResult;
	int iCount;
};

struct PMSG_ANS_CSATTKGUILDLIST
{
	PWMSG_HEAD h;
	BYTE btResult;
	int iCount;
};

struct PMSG_NOTIFY_OBJECT_CREATION_STATE
{
	struct PBMSG_HEAD2 h;
	BYTE btObjIndexH;
	BYTE btObjIndexL;
	BYTE btCreationState;
};

struct PMSG_ANS_MOVE_TO_CASTLE_HUNTZONE
{
	PBMSG_HEAD2 h; // +0x0(0x4)
	BYTE btResult; // +0x4(0x1)
};

struct PMSG_ANS_GUILDMARK_OF_CASTLEOWNER
{
	PBMSG_HEAD2 h;	// C1:B9:02
	BYTE GuildMarkOfCastleOwner[32];	// 4
};

struct PMSG_ANS_USEWEAPON
{
	PBMSG_HEAD2 h;	// C1:B7:01
	BYTE btResult;	// [1:Attacker] [2:Targets]	4
	BYTE btObjIndexH;	// 5
	BYTE btObjIndexL;	// 6
	BYTE btWeaponType;	// [1:Slingshot attack] [2:Slingshot defense]	7
	BYTE btPointX;	// 8
	BYTE btPointY;	// 9
};

struct PMSG_ANS_CSHUNTZONEENTER
{
	PBMSG_HEAD2 h;	// C1:B2:1F
	BYTE btResult;	// 4
	BYTE btHuntZoneEnter;	// 5
};

struct PMSG_ANS_NOTIFYCSPROGRESS
{
	PBMSG_HEAD2 h;	// C1:B2:18
	BYTE btCastleSiegeState;	// 4
	BYTE szGuildName[8];	// 5
};

struct PMSG_ANS_JEWEL_UNMIX
{
	PBMSG_HEAD2 h;	// C1:BC:[00:01]
	BYTE btResult;	// 4
};

struct PMSG_ANS_MINIMAPDATA
{
	PBMSG_HEAD2 h;
	BYTE btResult;	// 4
};

struct PMSG_ANS_CSLEFTTIMEALARM
{
	PBMSG_HEAD2 h;	// C1:B2:1E
	BYTE btHour;	// 4
	BYTE btMinute;	// 5
};

struct PMSG_REQ_LUCKYCOIN_REGISTER
{
	PBMSG_HEAD2 h;
};

struct PMSG_ANS_LUCKYCOIN_REGISTER
{
	PBMSG_HEAD2 h;
	unsigned char btResult;
	int iLuckyCoin;
};

struct PMSG_ANS_LUCKYCOININFO
{
	PBMSG_HEAD2 h;
	int iLuckyCoin;
};

struct PMSG_REQ_LUCKYCOIN_TRADE
{
	PBMSG_HEAD2 h;
	int iLuckyCoinTradeCount;
};

struct PMSG_REQ_INVENTORY_EQUIPMENT_ITEM
{
	PBMSG_HEAD2 h;
	BYTE btItemPos;
	BYTE btValue;
};

struct PMSG_ANS_INVENTORY_EQUIPMENT_ITEM
{
	PBMSG_HEAD2 h;
	BYTE btItemPos;
	BYTE btResult;
};

struct PMSG_ANS_LUCKYCOIN_TRADE
{
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_TELEPORT_RESULT
{
	PBMSG_HEAD h;	// C3:1C
	BYTE Unk;
	WORD Type;
	BYTE MapNumber;	// 4
	BYTE Unk1;
	BYTE MapX;	// 5
	BYTE MapY;	// 6
	BYTE Dir;	// 7
};

struct PMSG_REQ_ENTER_DOPPELGANGER
{
	PBMSG_HEAD2 h;
	BYTE btPos;
};

struct PMSG_REQ_ENTER_ZONE
{
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_NOTIFY_ZONE_TIME
{
	PBMSG_HEAD2 h;
	BYTE m_btMsgType;
	BYTE m_btDayOfWeek;
	WORD m_wZoneIndex;
	DWORD m_dwRemainTime;
	DWORD m_dwRemainMonster;
};

//BYTE Packet[13] = {0xC1, 0x0D, 0xF7, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
struct PMSG_NOTIFY_ZONE_CLEAR
{
	PBMSG_HEAD2 h;
	DWORD m_nType;
	DWORD m_nRewardExp;
};
struct PMSG_ENTER_ZONE_RESULT
{
	PBMSG_HEAD2 h;
	BYTE m_btResult;
	BYTE m_btDayOfWeek;
	BYTE m_btZoneIndex;
	BYTE m_btWeather;
	DWORD m_dwRemainTime;
};

struct PMSG_RAGE_ATTACK_RANGE
{
	PBMSG_HEAD h;
	BYTE MagicNumberH;
	BYTE MagicNumberL;
	BYTE TargetIndexH;
	BYTE TargetIndexL;
};

struct PMSG_RAGEATTACK
{
	PBMSG_HEAD h;
	BYTE SourceNumberH;
	BYTE SourceNumberL;
	BYTE MagicNumberH;
	BYTE MagicNumberL;
	BYTE TargetNumberH;
	BYTE TargetNumberL;
};


struct PMSG_RAGE_ATTACK_RANGE_ANS
{
	PBMSG_HEAD2 h;
	WORD MagicNumber;
	WORD Target[5];
};

struct PMSG_TARGET_HP
{
	PBMSG_HEAD2 h; // C1:FA:05
	WORD m_Index;
	DWORD m_Hp;
	DWORD m_MaxHp;
	WORD m_Level;
	bool m_bIceEffect;
	bool m_bPoisonEffect;
};

struct PMSG_ADDON_REQUEST
{
	BYTE HEAD;
	BYTE SIZE;
	BYTE OPCODE;
	BYTE OPCODE2;
	WORD M_ADDON;
	DWORD M_CRC1;
	DWORD M_CRC2;
};

struct PMSG_TARGET_HP_INFO
{
	WORD m_Index;
	DWORD m_Hp;
	DWORD m_MaxHp;
	WORD m_Level;
	bool m_bIceEffect;
	bool m_bPoisonEffect;
};


struct PMSG_TARGET_HP_DELETE
{
	WORD m_Index;
};

struct PMSG_SEND_AH_INFO {
	PBMSG_HEAD2 h;
	BYTE type; // 0 - hack detected; 1 - Agi
	DWORD Agi;
	DWORD MainTick;
};

struct PMSG_REQ_CRCDATA
{
	PBMSG_HEAD2 h;
	DWORD MainExe;
	DWORD IGCDLL;
	DWORD PlayerBMD;
	DWORD HackDB;
	DWORD SkillCRC;
};


struct PMSG_REQ_REFINE_PENTAGRAM_JEWEL
{
	PBMSG_HEAD2 h;
	BYTE btRefineKind;
};

struct PMSG_REQ_UPGRADE_PENTAGRAM_JEWEL
{
	PBMSG_HEAD2 h;
	BYTE btUpgradeType;
	BYTE btTargetValue;
};

struct PMSG_RSA_KEY
{
	PWMSG_HEAD2 h;
	DWORD m_ESize;
	DWORD m_NSize;
};

struct PMSG_OBJECT_LIFE_INFO
{
	PBMSG_HEAD2 h;
	BYTE ObjNumberH;
	BYTE ObjNumberL;
	BYTE MaxLifeHH;
	BYTE MaxLifeHL;
	BYTE MaxLifeLH;
	BYTE MaxLifeLL;
	BYTE CurLifeHH;
	BYTE CurLifeHL;
	BYTE CurLifeLH;
	BYTE CurLifeLL;
};


struct PMSG_PRICE_INFO
{
	PWMSG_HEAD h;	// C2:95
	BYTE subc;
	BYTE Type;	// 4
	BYTE count;	// 5
};

struct PMSG_PRICE_ITEM_LIST
{
	BYTE Pos;
	UINT64 SellPrice;
	UINT64 BuyPrice;
};


// NewPVP Protocol
/************************************************************************/
/* STRUCTS BELOW FOR PROTOCOL CLIENT ONLY                               */
/************************************************************************/
typedef struct PMSG_REQ_DUEL_INVITE
{
	PBMSG_HEAD2 h;
	BYTE NumberH;
	BYTE NumberL;
	char szName[MAX_ACCOUNT_LEN];
} PMSG_REQ_DUEL_INVITE, *LPPMSG_REQ_DUEL_INVITE;

typedef struct PMSG_ANS_DUEL_ANSWER
{
	PBMSG_HEAD2 h;
	BYTE bDuelOK;
	BYTE NumberH;
	BYTE NumberL;
} PMSG_ANS_DUEL_ANSWER, *LPPMSG_ANS_DUEL_ANSWER;

typedef struct PMSG_REQ_DUEL_EXIT
{
	PBMSG_HEAD2 h;
} PMSG_REQ_DUEL_EXIT, *LPPMSG_REQ_DUEL_EXIT;

typedef struct PMSG_REQ_DUEL_JOINCNANNEL
{
	PBMSG_HEAD2 h;
	BYTE nChannelId;
} PMSG_REQ_DUEL_JOINCNANNEL, *LPPMSG_REQ_DUEL_JOINCNANNEL;

typedef struct PMSG_REQ_DUEL_LEAVECNANNEL
{
	PBMSG_HEAD2 h;
	BYTE nChannelId;
} PMSG_REQ_DUEL_LEAVECNANNEL, *LPPMSG_REQ_DUEL_LEAVECNANNEL;

struct PMSG_REQ_ARCA_BATTLE_JOIN
{
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_ARCA_BATTLE_ENTER
{
	PBMSG_HEAD2 h;
	char btEnterSeq;
};

struct PMSG_REQ_ACHERON_ENTER
{
	PBMSG_HEAD2 h;
};

struct PMSG_ANS_ACHERON_ENTER
{
	PBMSG_HEAD2 h;
	char btResult;
};

struct PMSG_REQ_MASTERLEVEL_SKILL
{
	PBMSG_HEAD2 h; //C1:F3:52
	int iMasterLevelSkill;
};

struct PMSG_SETCHARSET
{
	PBMSG_HEAD h;
	BYTE subcode;
	WORD charset;
};

struct PMSG_SETAGILITYBUG
{
	PBMSG_HEAD h;
	BYTE subcode;
	BYTE value;
};
//C1 1C EC 25 1D 00 00 00 14 00 00 00 19 00 00 00 0A 00 00 00 00 00 00 00 CD CC 6C 3F
struct PMSG_ADDSTATS_RESULT
{
	PBMSG_HEAD2 h; // C1:EC
	WORD str;
	WORD stradd;
	WORD agi;
	WORD agiadd;
	WORD vit;
	WORD vitadd;
	WORD ene;
	WORD eneadd;
	WORD cmd;
	WORD cmdadd;
	float mPrec;
};
struct PMSG_STATS_RESULT
{
	PBMSG_HEAD2 h; // C1:EC
	DWORD CriticalDMGInc;
	DWORD unk1;
	WORD str;
	WORD stradd;
	WORD agi;
	WORD agiadd;
	WORD vit;
	WORD vitadd;
	WORD ene;
	WORD eneadd;
	WORD cmd;
	WORD cmdadd;
	DWORD SDRation;// unk2;
	DWORD SDBypass;
	float unk4;
	float unk5;
	WORD ZenObincRate;
	float unk7;
	float unk8;
	DWORD unk9;
	float unk10;
	float unk11;
	float unk12;
	float unk13;
	float unk14;
	float unk15;
	float unk16;
	float unk17;
	float unk18;
	float unk19;
	float unk20;
	float unk21;
	float unk22;
	float unk23;
	float unk24;
	float IncreaseDamageRate;
	float unk26;
	float CritDMGRate;
	float ExcDMGRate;
	float DoubleDmgRate;
	BYTE unk28;
	BYTE unk29;
	BYTE unk30;
	float ExcellentDMGInc;
	float ExcellentDMGInc2;
};

struct PMSG_REQ_NIXIELAKE_ENTER
{
	PBMSG_HEAD2 h;
};

struct PMSG_ANS_NIXIELAKE_ENTER
{
	PBMSG_HEAD2 h;
	char btResult;
};

struct PMSG_CHAIN_MAGIC
{
	PBMSG_HEAD2 h;
	BYTE MagicNumberH;
	BYTE MagicNumberL;
	WORD wUserIndex;
	BYTE nCount;
};

struct PMSG_CHAIN_MAGIC_OBJECT
{
	WORD nTargetIndex;
};

struct PMSG_RECV_TRAP // it must similar to any "correct" packet
{
	PBMSG_HEAD2 h;
	WORD wUserIndex;
	time_t CurrTime;
};

// Season 8 Matching System
// Guild Matching Packets

struct _stGuildMatchingList
{
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

struct PMSG_REQ_GUILDMATCHINGLIST
{
	PBMSG_HEAD2 h;
	int nPage;
};

struct PMSG_REQ_GUILDMATCHINGLISTSEARCHWORD
{
	PBMSG_HEAD2 h;
	int nPage;
	char szSearchWord[11];
};

struct PMSG_REQ_REGGUILDMATCHINGDATA
{
	PBMSG_HEAD2 h;
	char szMemo[41];
	BYTE btInterestType;
	BYTE btLevelRange;
	BYTE btClassType;
};

struct PMSG_REQ_CANCEL_GUILDMATCHINGLIST
{
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_JOIN_GUILDMATCHING
{
	PBMSG_HEAD2 h;
	int nGuildNumber;
};

struct PMSG_REQ_ALLOW_JOIN_GUILDMATCHING
{
	PBMSG_HEAD2 h;
	int nAllowType;
	char szName[11];
};

struct PMSG_REQ_ALLOWLIST_GUILDMATCHING
{
	PBMSG_HEAD2 h;
};

struct PMSG_ANS_GUILDMATCHINGLIST
{
	PWMSG_HEAD2 h;
	int nPage;
	int nTotalPage;
	int nListCount;
	int nResult;
	_stGuildMatchingList stGuildMatchingList;
};

struct PMSG_ANS_REGGUILDMATCHINGDATA
{
	PBMSG_HEAD2 h;
	int nResult;
};

struct PMSG_ANS_CANCEL_JOIN_GUILDMATCHING
{
	PBMSG_HEAD2 h;
	int nResult;
};

struct PMSG_ANS_CANCEL_GUILDMATCHINGLIST
{
	PBMSG_HEAD2 h;
	int nResult;
};

struct PMSG_ANS_JOIN_GUILDMATCHING
{
	PBMSG_HEAD2 h;
	int nResult;
};

struct PMSG_ANS_ALLOW_JOIN_GUILDMATCHING
{
	PBMSG_HEAD2 h;
	char szMemberName[MAX_ACCOUNT_LEN + 1];
	int nAllowType;
	int nResult;
};

struct PMSG_ANS_WAIT_GUILDMATCHING
{
	PWMSG_HEAD2 h;
	int nListCount;
	int nResult;
};

struct PMSG_ANS_ALLOWLIST_STATE_GUILDMATCHING
{
	PBMSG_HEAD2 h;
	char szGuildMasterName[MAX_ACCOUNT_LEN + 1];
	char szGuildName[MAX_GUILD_LEN + 1];
	int nResult;
};

struct PMSG_ANS_NOTI_GUILDMATCHING
{
	PBMSG_HEAD2 h;
	int nResult;
};

struct PMSG_ANS_NOTI_GUILDMATCHING_FORGUILDMASTER
{
	PBMSG_HEAD2 h;
	int nResult;
};

// Party Matching

struct _PARTY_INFO_LISTDB
{
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

struct _PARTY_INFO_LIST
{
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

struct PMSG_ANS_REG_WANTED_PARTYMEMBER
{
	PBMSG_HEAD2 h;
	int nResult;
};

struct PMSG_ANS_GET_PARTYMATCHINGLIST
{
	PWMSG_HEAD2 h;
	int nListCount;
	int nPage;
	int nTotalPage;
	int nResult;
};

struct PMSG_ANS_MEMBERJOIN_PARTYMATCHINGLIST
{
	PBMSG_HEAD2 h;
	int nResult;
};

struct PMSG_ANS_MEMBERJOIN_STATELIST_PARTYMATCHING
{
	PBMSG_HEAD2 h;
	int nResult;
	int nLeaderChannel;
	char szLeaderName[MAX_ACCOUNT_LEN + 1];
};

struct PMSG_ANS_WAITLIST_PARTYMATCHING
{
	PWMSG_HEAD2 h;
	int nListCnt;
	int nResult;
};

struct PMSG_ANS_CANCEL_JOIN_PARTYMATCHING
{
	PBMSG_HEAD2 h;
	BYTE btType;
	int nResult;
};

struct PMSG_ANS_ACCEPTMEMBER_PARTYMATCHING
{
	PBMSG_HEAD2 h;
	char szMemberName[MAX_ACCOUNT_LEN + 1];
	BYTE btType;
	BYTE btFlag;
	int nResult;
};

struct PMSG_ANS_NOTI_JOINPARTYMATCHING
{
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_REG_WANTED_PARTYMEMBER
{
	PBMSG_HEAD2 h;
	char szTitle[41];
	char szPassword[5];
	WORD nMinLevel;
	WORD nMaxLevel;
	WORD nHuntingGround;
	BYTE btWantedClass;
	BYTE btWantedClassDetailInfo[7];
	BYTE btUsePassword;
	BYTE btApprovalType;
};

struct PMSG_REQ_GET_PARTYMATCHINGLIST
{
	PBMSG_HEAD2 h;
	int nPage;
	char szSearchWord[11];
	BYTE btUseSearchWord;
};

struct PMSG_REQ_MEMBERJOIN_PARTYMATCHINGLIST
{
	PBMSG_HEAD2 h;
	char szLeaderName[MAX_ACCOUNT_LEN + 1];
	char szPassword[5];
	BYTE btUsePassword;
	BYTE btUseRandomParty;
};

struct PMSG_REQ_MEMBERJOIN_STATELIST_PARTYMATCHING
{
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_WAITLIST_PARTYMATCHING
{
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_ACCEPTMEMBER_PARTYMATCHING
{
	PBMSG_HEAD2 h;
	char szMemberName[MAX_ACCOUNT_LEN + 1];
	BYTE btType;
};

struct PMSG_REQ_CANCEL_JOIN_PARTYMATCHING
{
	PBMSG_HEAD2 h;
	BYTE btType;
};

struct PMSG_ANS_PARTYMATCHING_ERROR
{
	PBMSG_HEAD2 h;
	int iResult;
};

// Chaos Castle Survival of the Fittest
struct PMSG_CCF_UI_ONOFF
{
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_REPOSUSER_IN_CCF
{
	PBMSG_HEAD2 h;
	BYTE btPX;	// 4
	BYTE btPY;	// 5
};

struct PMSG_REQ_CCF_RANKING
{
	PBMSG_HEAD2 h;
	BYTE byRankingType;
};


struct PMSG_RESULT_CCF_ENTERCHECK
{
	PBMSG_HEAD2 h;
	BYTE byResult;
	BYTE byRemainTime;
	BYTE byRemainTime_LOW;
};


// Muun
struct PMSG_MUUNITEM_DUR
{
	PBMSG_HEAD2 h;
	BYTE IPos;
	BYTE Dur;
};
struct PMSG_MUUN_INVEN_SUB_EQUIP_OPEN
{
	PBMSG_HEAD2 h;
	BYTE Result;
};

struct PMSG_MUUNITEM_CONDITION_STATUS
{
	PBMSG_HEAD2 h;
	BYTE btIPos;
	BYTE btStatus;
};

struct PMSG_USEITEM_MUUN_INVEN
{
	PBMSG_HEAD2 h;
	BYTE inventoryPos;
	BYTE invenrotyTarget;
	BYTE btItemUseType;
};
struct PMSG_USEITEM_MUUN_INVEN_RESULT
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btItemUseType;
};

struct PMSG_USER_MUUN_EQUIPMENT_CHANGED
{
	PBMSG_HEAD2 h;
	BYTE NumberH;
	BYTE NumberL;
	BYTE ItemInfo[12];
};

struct PMSG_PETINVENTORY_DELETE
{
	PBMSG_HEAD2 h;
	BYTE IPos;
	BYTE Flag;
};

struct PMSG_MUUN_INVEN_ITEM_MOD
{
	PBMSG_HEAD2 h;
	BYTE subcode;
	BYTE Pos;
	BYTE ItemInfo[12];
};

struct PMSG_MUUNITEM_GETREQUEST
{
	PBMSG_HEAD2 h;
	BYTE NumberH;
	BYTE NumberL;
};

struct PMSG_MUUNITEM_GETRESULT
{
	PBMSG_HEAD2 h;
	BYTE result;
	BYTE Data[12];
};

struct PMSG_MUUN_ITEM_SELLRESULT
{
	PBMSG_HEAD2 h;
	BYTE Result;
	DWORD Money;
};

struct PMSG_MUUN_ITEM_SELLREQUEST
{
	PBMSG_HEAD2 h;
	BYTE Pos;
};

// Minimap Party member pos

struct stMemberPosInfo
{
	char szName[MAX_ACCOUNT_LEN + 1];
	BYTE btMapNumber;
	BYTE btPosX;
	BYTE btPosY;
};

struct PMSG_MEMBER_POS_INFO_SEND
{
	PBMSG_HEAD2 h;
	BYTE btCount;
	stMemberPosInfo stPosInfo[5];
};

struct PMSG_REQ_NPC_POSITION
{
	PBMSG_HEAD2 h;
	BYTE btType;
};

// Event Inventory
struct PMSG_EVENTITEM_GETREQUEST
{
	PBMSG_HEAD2 h;
	BYTE NumberH;
	BYTE NumberL;
};

struct PMSG_EVENTITEM_GETRESULT
{
	PBMSG_HEAD2 h;
	BYTE result;
	BYTE index[2]; // New Packet
	BYTE Data[12];
};

struct PMSG_EVENTITEM_THROW
{
	PBMSG_HEAD2 h;
	BYTE px;
	BYTE py;
	BYTE Ipos;
};

struct PMSG_EVENTITEM_THROW_RESULT
{
	PBMSG_HEAD2 h;
	unsigned char Result;
	BYTE Ipos;
};

struct PMSG_EVENTINVENTORY_DELETE
{
	PBMSG_HEAD2 h;
	BYTE IPos;
	BYTE Flag;
};

struct PMSG_EVENTITEM_DUR
{
	PBMSG_HEAD2 h;
	BYTE IPos;
	BYTE Dur;
};

struct PMSG_EVENT_INVEN_ITEM_MOD
{
	PBMSG_HEAD2 h;
	BYTE Pos;
	BYTE ItemInfo[12];
};

// Exp info
struct PMSG_RES_EXPEVENTINFO
{
	PBMSG_HEAD2 h;
	WORD wPCBangRate;
	WORD wExpRate;
};

// Mining System

struct PMSG_REQ_MINETWINKLE
{
	PBMSG_HEAD2 h;
	WORD wUserIndex;
	WORD wTwinkleIndex;
	WORD wTwinkleType;
	BYTE byMapNumber;
};

struct PMSG_ANS_MINETWINKLE
{
	PBMSG_HEAD2 h;
	WORD wUserIndex;
	WORD wTwinkleIndex;
	WORD wTwinkleType;
	BYTE byMapNumber;
	BYTE byCurrentStage;
	int iRewardJewelNumber;
	int iResult;
};

struct PMSG_REQ_MINETWINKLE_REWARD
{
	PBMSG_HEAD2 h;
	WORD wUserIndex;
	WORD wTwinkleIndex;
	WORD wTwinkleType;
	BYTE byMapNumber;
	BYTE byIsStopMine;
};

struct PMSG_ANS_MINETWINKLE_REWARD
{
	PBMSG_HEAD2 h;
	WORD wUserIndex;
	WORD wTwinkleIndex;
	WORD wTwinkleType;
	BYTE byMapNumber;
	BYTE byCurrentStage;
	int iResult;
};

struct PMSG_ANS_MINETWINKLE_END_ANIMATION
{
	PBMSG_HEAD2 h;
	WORD wUserIndex;
	WORD wTwinkleIndex;
	BYTE byMapNumber;
};

struct PMSG_ANS_MINETWINKLE_CANCEL
{
	PBMSG_HEAD2 h;
	WORD wUserIndex;
	WORD wTwinkleIndex;
	WORD wTwinkleType;
	BYTE byMapNumber;
	BYTE byCurrentStage;
};

// Illusion Temple Renewal
struct PMSG_ITL_RELICS_GET_USER
{
	PBMSG_HEAD2 h;
	WORD wUserIndex;
	char Name[MAX_ACCOUNT_LEN];
	BYTE byGet;
};

struct PMSG_ANS_ITL_RELATE_RELICS
{
	PBMSG_HEAD2 h;
	BYTE byError;
	BYTE byNumberH;
	BYTE byNumberL;
	WORD wNpcType;
	BYTE byAct;
};

struct PMSG_ITL_STONEINFO
{
	PBMSG_HEAD2 h;
	BYTE byMapTagIndex;
	WORD wOccupiedStoneType;
	BYTE byOccupiedStoneState;
	int nNpcIndex;
};

struct PMSG_ITL_PLAY_STATE
{
	PBMSG_HEAD2 h;
	WORD wRemainSec;
	BYTE byAlliedPoint;
	BYTE btIllusionPoint;
	BYTE btMyTeam;
};

struct PMSG_ITL_STATE
{
	PBMSG_HEAD2 h;
	BYTE btTempleNumber;
	BYTE btITLState;
};

struct PMSG_ITL_SKILL_END
{
	PBMSG_HEAD2 h;
	BYTE MagicNumberH;
	BYTE MagicNumberL;
	WORD wObjIndex;
};

struct PMSG_ILLUSION_TEMPLE_USE_MAGIC_RESULT
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE MagicNumberH;
	BYTE MagicNumberL;
	WORD wSourceObjIndex;
	WORD wTargetObjIndex;
};

struct PMSG_ITR_USER_ADD_EXP
{
	char GameID[MAX_ACCOUNT_LEN];
	BYTE btMapNumber;
	BYTE btTeam;
	BYTE btClass;
	BYTE btUserKillCount;
};

struct PMSG_ITR_RESULT
{
	PBMSG_HEAD2 h;
	BYTE btAlliedPoint;
	BYTE btIllusionPoint;
	BYTE btUserCount;
	BYTE byWinTeamIndex;
};

struct _ITL_AnswerEnter
{
	PBMSG_HEAD2 h;
	BYTE bReason;
	char Name[MAX_ACCOUNT_LEN + 1];
};

struct _ITR_AcceptEnter
{
	PBMSG_HEAD2 h;
	BYTE byError;
	char Name[MAX_ACCOUNT_LEN + 1];
};

struct PMSG_REQ_ENTER_ITR
{
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_ACCEPTENTER_ITR
{
	PBMSG_HEAD2 h;
	BYTE byEnterOK;
};

struct PMSG_REQ_ITL_RELATE_RELICS
{
	PBMSG_HEAD2 h;
	BYTE NumberH;
	BYTE NumberL;
	WORD wNpcType;
	BYTE byPrePacket;
	BYTE byAct;
};

struct PMSG_ITL_USE_MAGIC_RESULT
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE MagicNumberH;
	BYTE MagicNumberL;
	WORD wSourceObjIndex;
	WORD wTargetObjIndex;
};

struct PMSG_ITL_RESULT
{
	PBMSG_HEAD2 h;
	BYTE btUserCount;
	char szAlliedGuildName[MAX_GUILD_LEN + 1];
	char szIllusionGuildName[MAX_GUILD_LEN + 1];
	BYTE byWin;
};

struct PMSG_ITL_USER_ADD_EXP
{
	char GameID[MAX_ACCOUNT_LEN];
	BYTE btMapNumber;
	BYTE btTeam;
	BYTE btClass;
	BYTE btUserKillCount;
};

struct PMSG_ANS_ITL_SCHEDULE
{
	PWMSG_HEAD2 h;
	BYTE byCount;
	BYTE LeagueDay1;
	BYTE LeagueDay2;
	BYTE LeagueDay3;
	BYTE LeagueDay4;
	BYTE LeagueDay5;
	BYTE LeagueDay6;
	BYTE LeagueDay7;
	BYTE SemiFinal;
	BYTE Final;
	BYTE ITL_STATE;
};

struct __ITL_GUILD_RANK
{
	char szGuildName[MAX_GUILD_LEN + 1];
	BYTE byRank;
	BYTE byITLType;
	BYTE byWin;
	BYTE byLose;
};

struct PMSG_ITL_GUILD_RANKINFO
{
	PWMSG_HEAD2 h;
	BYTE byResult;
	BYTE byCnt;
};

struct PMSG_ANS_REWARDLIST
{
	PWMSG_HEAD2 h;
	BYTE byResult;
	BYTE byCount;
};

struct _tagITL_REWARDLIST
{
	BYTE byRank;
	BYTE byEnteredCnt;
	BYTE byPoint;
	bool bGotReward;
	char szCharName[MAX_ACCOUNT_LEN + 1];
	char szGuildName[MAX_GUILD_LEN + 1];
};

struct PMSG_REQ_ACHERON_EVENT_ENTER
{
	PBMSG_HEAD2 h;
};

struct PMSG_ANS_ACHERON_EVENT_ENTER
{
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_SEND_WINDOW_CLOSE
{
	PBMSG_HEAD2 h;
};

struct PMSG_RAREITEMPIRCESEND
{
	PWMSG_HEAD2 h;
	BYTE btItemSellPriceDivisor;
	int iJewelOfBlessPrice;
	int	iJewelOfSoulPrice;
	int iJewelOfChaosPrice;
	int iJewelOfLifePrice;
	int iJewelOfCreationPrice;
	int iCrestOfMonarchPrice;
	int iLochFeatherPrice;
	int iJewelOfGuardianPrice;
	int iWereRabbitEggPrice;
};

struct STAT_BONUS
{
	WORD BonusOptionID;
	WORD BonusOptionVal1;
	WORD BonusOptionVal2;
};

struct PMSG_STAT_BONUS
{
	PBMSG_HEAD2 h;
	STAT_BONUS m_BonusOption[5];
};

struct PMSG_MUUN_RIDE_SELECT
{
	PBMSG_HEAD2 h;
	WORD wItemNum;
};

struct PMSG_REQ_MUUN_EXCHANGE
{
	PBMSG_HEAD2 h;
	BYTE btSelect;
};

struct PMSG_SEND_MUUN_RIDE_VIEWPORT_INFO
{
	PWMSG_HEAD2 h;
	BYTE Count;
};
struct _tagMuunRideViewPortInfo
{
	BYTE NumberH;
	BYTE NumberL;
	BYTE MuunRideItemH;
	BYTE MuunRideItemL;
};

struct PMSG_ANS_MUUN_EXCHANGE
{
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_MUUN_ATTACK_COMMAND
{
	PBMSG_HEAD2 h;
	BYTE Slot;
	BYTE SkillType;
	BYTE NumberH;
	BYTE NumberL;
	BYTE TargetNumberH;
	BYTE TargetNumberL;
};

// UBF

struct PMSG_REQ_UBF_INFO
{
	PBMSG_HEAD2 h;
	BYTE btUBFKind;
};

struct PMSG_REQ_UBF_JOIN
{
	PBMSG_HEAD2 h;
	BYTE btUBFKind;
};

struct PMSG_REQ_UBF_CANCEL
{
	PBMSG_HEAD2 h;
	BYTE btCanceled;
};

struct PMSG_REQ_UBF_REAL_NAME
{
	PBMSG_HEAD2 h;
	BYTE btRequest;
};

struct PMSG_REQ_GET_REWARD
{
	PBMSG_HEAD2 h;
	BYTE btBattleKind;
};

struct PMSG_ANS_UBF_JOIN
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	WORD nLeftSec;
};

struct PMSG_ANS_UBF_INFO
{
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_ANS_UBF_CANCEL
{
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_ANS_GET_REWARD
{
	PBMSG_HEAD2 h;
	BYTE btResult;
};

// DSF

struct PMSG_REQ_DSF_ACCEPTENTER
{
	PBMSG_HEAD2 h;
	BYTE btEnterOK;
};

struct PMSG_REQ_DSF_GO_FINAL_PARTY_INFO
{
	PBMSG_HEAD2 h;
	BYTE btDSFType;
};

struct PMSG_REQ_DSF_GET_REWARD
{
	PBMSG_HEAD2 h;
	BYTE btDSFType;
};

struct PMSG_ANS_DSF_ENTER
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	char Name[MAX_ACCOUNT_LEN + 1];
	BYTE btDSFType;
};

struct PMSG_ANS_DSF_ACCEPTENTER
{
	PBMSG_HEAD2 h;
	BYTE btError;
	char Name[MAX_ACCOUNT_LEN + 1];
};

struct PMSG_ANS_DSF_GO_FINAL_PARTY_INFO
{
	PWMSG_HEAD2 h;
	BYTE btResult;
	BYTE btCnt;
	BYTE btDSFType;
};

struct DSF_GO_FINAL_PARTY
{
	DSF_GO_FINAL_PARTY()
	{
		this->Clear();
	}

	void Clear()
	{
		memset(szUserName1, 0x00, sizeof(szUserName1));
		memset(szUserName2, 0x00, sizeof(szUserName2));
		this->wServerCode1 = -1;
		this->wServerCode2 = -1;
		this->btRank = 0;
		this->btDSFType = 0;
		this->iPoint = 0;
		this->iEnterYear = 0;
		this->btEnterMonth = 0;
		this->btEnterDay = 0;
	}

	char szUserName1[MAX_ACCOUNT_LEN + 1];
	char szUserName2[MAX_ACCOUNT_LEN + 1];
	WORD wServerCode1;
	WORD wServerCode2;
	BYTE btRank;
	BYTE btDSFType;
	int iPoint;
	int iEnterYear;
	BYTE btEnterMonth;
	BYTE btEnterDay;
};

struct PMSG_ANS_DSF_SCHEDULE
{
	PBMSG_HEAD2 h;
	BYTE LeagueDay1;
	BYTE LeagueDay2;
	BYTE LeagueDay3;
	BYTE LeagueDay4;
	BYTE SemiFinalDay1;
	BYTE SemiFinalDay2;
	BYTE Final;
	BYTE CurDay;
	BYTE LeagueHour[10];
	BYTE LeagueMin[10];
	BYTE SemiFinalHour[10];
	BYTE SemiFinalMin[10];
	BYTE FinalHour;
	BYTE FinalMin;
	BYTE RewardStartDay;
	BYTE RewardEndDay;
};

struct PMSG_DSF_TEAM_USERNAME
{
	PWMSG_HEAD2 h;
	int nPartyCount;
	BYTE btResult;
};

struct DSF_TEAM_PARTY_USER_NAME
{
	int iTeamIndex;
	char szUserName1[MAX_ACCOUNT_LEN + 1];
	char szUserName2[MAX_ACCOUNT_LEN + 1];
	int iTeamLevel;
};

struct PMSG_DSF_STATE
{
	PBMSG_HEAD2 h;
	BYTE btDSFState;
};

struct PMSG_DSF_PLAY_STATE
{
	PBMSG_HEAD2 h;
	WORD wRemainSec;
	int iTeamPoint[20];
	BYTE btMyTeam;
};

struct PMSG_DSF_RESULT
{
	PBMSG_HEAD2 h;
	int iTeamPoint[20];
};

struct PMSG_ANS_DSF_GET_REWARD
{
	PBMSG_HEAD2 h;
	BYTE btResult;
};

// Rest of S9
struct PMSG_ANS_USE_POPUP
{
	PBMSG_HEAD2 h;
	int nPopupType;
};

struct PMSG_SEND_EVENT_BANNER
{
	PBMSG_HEAD2 h;
	int iBannerType;
};

struct PMSG_ALTERPSHOPVAULT
{
	PBMSG_HEAD h;
	BYTE subcode;
	BYTE type;
};

struct PMSG_POST_DATA
{
	PBMSG_HEAD2 h;
	BYTE btColorRGB[3];
	char szServerName[50];
	char szTag[20];
	char szName[MAX_ACCOUNT_LEN];
	char szMessage[MAX_CHAT_LEN];
};

struct PMSG_SEND_HITHACK_INFO // C1:FA:14
{
	PBMSG_HEAD2 h;
	DWORD dwHitHackVal1;
	DWORD dwHitHackVal2;
};

// Season X
struct BLOCKCHAT_DATA
{
	BYTE btBlockEnable;
	BYTE btIndex;
	char szName[MAX_ACCOUNT_LEN + 1];
};

struct PMSG_BLOCKCHAT_LIST // C2:6E:00
{
	PWMSG_HEAD2 h;
	BYTE btType;
	BYTE btSubType;
	BYTE Count;
	BLOCKCHAT_DATA m_BlockData[20];
};

struct PMSG_EVENTENTRY_NOTICE
{
	PBMSG_HEAD2 h; // C1:F1:FE
	BYTE btEventType;
	BYTE btNoticeState;
};

struct PMSG_RUUD
{
	PBMSG_HEAD2 h; // C1:D0:F1
	int iTotalRuud;
	int iObtainedRuud;
	BYTE btObtainNotiSend;
};

struct PMSG_REQ_CHAOS_MULTIMIX_CHECK
{
	PBMSG_HEAD h;
	BYTE btMixType;
	BYTE btMixCount;
};

struct PMSG_ANS_CHAOS_MULTIMIX_CHECK
{
	PBMSG_HEAD h;
	BYTE btResult;
};

struct PMSG_RANKING_SET
{
	PBMSG_HEAD h;
	BYTE btSetType;
	int iRankingType;
	DWORD ValueH;
	DWORD ValueL;
};

struct PMSG_REQ_RUUD_STORE_BUYITEM
{
	PBMSG_HEAD2 h;
	BYTE btPos;
};

struct PMSG_ANS_RUUD_STORE_BUYITEM
{
	PBMSG_HEAD2 h;
	BYTE btResult; // -1,-3 = general error, -2 = timeout, -4 = not enough ruud, -5 = not enough space
	BYTE btPos;
	BYTE btItemInfo[MAX_ITEM_INFO];
};

struct PMSG_REQ_USE_BOX
{
	PBMSG_HEAD2 h; // C1:D0:F2
	BYTE btPos;
};

struct PMSG_ANS_USE_BOX
{
	PBMSG_HEAD2 h;
	int iResult;
	int iItemType;
};

// test hunting log
struct PMSG_REQ_HUNTING_LOG
{
	PBMSG_HEAD2 h;
	WORD unk1;
	WORD unk2;
	WORD unk3;
	WORD unk4;
};

struct PMSG_ANS_HUNTING_LOG
{
	PBMSG_HEAD2 h;
	WORD unk1;
	WORD unk2;
	WORD unk3;
	WORD unk4;
};

struct PMSG_MUBOT_REQ_START
{
	PBMSG_HEAD2 h;
	BYTE btType;
};

struct PMSG_MUBOT_USE
{
	PBMSG_HEAD2 h;
	WORD Time;
	DWORD Money;
	BYTE Status;
};

struct PMSG_MUBOT_SETTINGS_RECV
{
	PWMSG_HEAD h;
	BYTE btType;
	BYTE btDATA[512];
};



#pragma pack ()

#endif
