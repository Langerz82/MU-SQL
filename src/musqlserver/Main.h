#if !defined(AFX_EMAIN_H__A7C14824_5DD7_49C9_BDFB_C8D813D08CB9__INCLUDED_)
#define AFX_EMAIN_H__A7C14824_5DD7_49C9_BDFB_C8D813D08CB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "resource.h"
#include "Protocol/protocol.h"
#include "Protocol/Sprotocol.h"
#include "Protocol/DSprotocol.h"
#include "Protocol/protocol.h"
#include "Protocol/EDSprotocol.h"
#include "Protocol/Fprotocol.h"
#include "ServerEngine.h"

#define SET_NUMBERH(x) ( (BYTE)((DWORD)(x)>>(DWORD)8) )
#define SET_NUMBERL(x) ( (BYTE)((DWORD)(x) & 0xFF) )
#define SET_NUMBERHW(x) ( (WORD)((DWORD)(x)>>(DWORD)16) )
#define SET_NUMBERLW(x) ( (WORD)((DWORD)(x) & 0xFFFF) )

extern HWND ghWnd;
extern int g_dwMaxServerGroups;
extern WORD g_FSGateServerListPort;
extern WORD g_JoinServerListPort;
extern WORD g_DataServerListPort;
extern WORD g_ExDataServerListPort;
extern TCHAR g_MuOnlineDNS[64];
extern TCHAR g_MeMuOnlineDNS[64];
extern TCHAR g_EventServerDNS[64];
extern TCHAR g_RankingServerDNS[64];
extern TCHAR g_DBPort[8];
extern TCHAR g_UserID[64];
extern TCHAR g_Password[64];
extern BOOL g_PwEncrypt;
extern BOOL g_DSMode;
extern BOOL g_UseJoinServer;
extern BOOL g_UseDataServer;
extern BOOL g_UseExDataServer;
extern TCHAR g_ServerName[64];
extern DWORD g_GensRankingUpdateTime;
extern DWORD g_GensLeaveAfterDays;
extern DWORD g_MachineIDConnectionLimitPerGroup;
bool IniteDataServer();
void eDataServerClose();
extern CDataServerProtocol m_DSProtocol;
extern CLoginServerProtocol m_JSProtocol;
extern CExDataServerProtocol m_EXDSProtocol;
extern TCHAR szWANIP[150];
extern HANDLE hQueue1, hQueue2, hQueue3, hQueue4, hQueue5, hJSQueue, hEXDSQueue;
extern WORD g_MagumsaCreateMinLevel;
extern WORD g_DarkLordCreateMinLevel;
extern WORD g_GrowLancerCreateMinLevel;
extern int g_iShowAllQueriesInDS;
extern int g_iConnectStatSyncEnable;
extern int g_DSBattleCoreEnable;

#endif // !defined(AFX_EMAIN_H__A7C14824_5DD7_49C9_BDFB_C8D813D08CB9__INCLUDED_)
