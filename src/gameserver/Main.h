#ifndef _MU_MAIN_GS_H_
#define _MU_MAIN_GS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "StdAfx.h"
#include "resource.h"
//#include "ServerEngine.h"
#include "Database/Database/DatabaseEnv.h"

class ConnectDatabaseConnection;
class MUDatabase;


#define SET_NUMBERH(x) ( (BYTE)((DWORD)(x)>>(DWORD)8) )
#define SET_NUMBERL(x) ( (BYTE)((DWORD)(x) & 0xFF) )
#define SET_NUMBERHW(x) ( (WORD)((DWORD)(x)>>(DWORD)16) )
#define SET_NUMBERLW(x) ( (WORD)((DWORD)(x) & 0xFFFF) )

extern WORD g_GameServerPort;
extern DWORD g_MaxConnectionsPerIP;
extern DWORD g_MaxPacketPerSec;
extern WORD g_ServerCode;

extern std::string g_HostURL;
extern std::string g_WhiteListIP;

extern std::string g_ServerAddress;
extern std::string g_DBPort;
extern std::string g_UserID;
extern std::string g_Password;
extern std::string g_MuOnlineDB;

extern DWORD g_MachineIDConnectionLimitPerGroup;

extern std::string szWANIP;
extern WORD g_GameServerListPort;

extern std::string g_logsDir;
extern std::string g_logsEntryCount;
extern std::string g_logsEntry[10]; // up to 10 logs

extern BOOL gDisconnectHackUser;

extern char szClientVersion[8];

extern WORD g_MagumsaCreateMinLevel;
extern WORD g_DarkLordCreateMinLevel;
extern WORD g_GrowLancerCreateMinLevel;

#endif
