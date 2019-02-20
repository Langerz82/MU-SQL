#if !defined(AFX_EMAIN_H__A7C14824_5DD7_49C9_BDFB_C8D813D08CB9__INCLUDED_)
#define AFX_EMAIN_H__A7C14824_5DD7_49C9_BDFB_C8D813D08CB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "resource.h"
#include "ServerEngine.h"
#include "Database/Database/DatabaseEnv.h"

class ConnectDatabaseConnection;
class MUDatabase;


#define SET_NUMBERH(x) ( (BYTE)((DWORD)(x)>>(DWORD)8) )
#define SET_NUMBERL(x) ( (BYTE)((DWORD)(x) & 0xFF) )
#define SET_NUMBERHW(x) ( (WORD)((DWORD)(x)>>(DWORD)16) )
#define SET_NUMBERLW(x) ( (WORD)((DWORD)(x) & 0xFFFF) )

extern WORD g_ConnectServerPort;
extern WORD g_ConnectServerUDP;
extern DWORD g_MaxConnectionsPerIP;
extern DWORD g_MaxPacketPerSec;
extern uint16 g_UDPPort;
extern WORD g_FTPPort;
extern std::string g_HostURL;
extern std::string g_FTPLogin;
extern std::string g_FTPPassword;
extern std::string g_VersionFile;
extern std::string g_ClientVersion;
extern std::string FTPLauncher;
extern std::string HashLauncher;
extern std::string g_WhiteListIP;

extern int g_dwMaxServerGroups;
extern WORD g_JoinServerListPort;

extern BOOL g_PwEncrypt;
extern BOOL g_UseJoinServer;
extern DWORD g_MachineIDConnectionLimitPerGroup;

extern std::string szWANIP;

extern std::string g_logsDir;
extern std::string g_logsEntryCount;
extern std::string g_logsEntry[10]; // up to 10 logs

extern BOOL gDisconnectHackUser;


#endif // !defined(AFX_EMAIN_H__A7C14824_5DD7_49C9_BDFB_C8D813D08CB9__INCLUDED_)
