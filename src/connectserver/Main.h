#if !defined(AFX_EMAIN_H__A7C14824_5DD7_49C9_BDFB_C8D813D08CB9__INCLUDED_)
#define AFX_EMAIN_H__A7C14824_5DD7_49C9_BDFB_C8D813D08CB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "resource.h"
#include "ServerEngine.h"

#define SET_NUMBERH(x) ( (BYTE)((DWORD)(x)>>(DWORD)8) )
#define SET_NUMBERL(x) ( (BYTE)((DWORD)(x) & 0xFF) )
#define SET_NUMBERHW(x) ( (WORD)((DWORD)(x)>>(DWORD)16) )
#define SET_NUMBERLW(x) ( (WORD)((DWORD)(x) & 0xFFFF) )


extern int g_dwMaxServerGroups;
extern WORD g_JoinServerListPort;
extern TCHAR g_ServerAddress[64];
extern TCHAR g_DBPort[8];
extern TCHAR g_UserID[64];
extern TCHAR g_Password[64];
extern TCHAR g_MuOnlineDB[64];


extern BOOL g_PwEncrypt;
extern BOOL g_UseJoinServer;
extern DWORD g_MachineIDConnectionLimitPerGroup;

extern TCHAR szWANIP[150];

extern TCHAR g_logsDir[64];
extern TCHAR g_logsEntryCount[2];
extern std::string g_logsEntry[10]; // up to 10 logs

extern int g_MaxPacketPerSec;
extern int g_MaxConnectionsPerIP;

#endif // !defined(AFX_EMAIN_H__A7C14824_5DD7_49C9_BDFB_C8D813D08CB9__INCLUDED_)
