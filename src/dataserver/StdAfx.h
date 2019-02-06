// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#pragma warning ( disable : 4786 )

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_




#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define TRACE_PACKET 0	
#define CURRENT_DB_VERSION 3
#define MAX_TYPE_PLAYER 8
#define MAX_NUMBER_MAP 101
#define CHECK_LIMIT(value, limit) (((value)< 0)?FALSE:((value)> limit-1)?FALSE:TRUE)
#define AUTO_REG 0
#define DATASERVER_VERSION "12.0.1.0"

// WARNING - IF YOU WANT TO RELEASE DS FOR PUBLIC - UNCOMMENT THIS LINE
//#define DS_AUTHORITY 

// Windows Header Files:
#include <windows.h>
#include <winsock2.h>
#include <commctrl.h>
#include <sql.h>
#include <sqltypes.h>
#include <sqlext.h>
#include "dbghelp.h"

#pragma comment ( lib , "comctl32.lib" )
#pragma comment ( lib , "WS2_32.lib" )
//#pragma comment ( lib , "cryptlib.lib" )
#pragma warning (disable : 4244)
#pragma warning (disable : 4819)
#pragma warning (disable : 4996)
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <time.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <queue>
#include "pugixml.hpp"
#include <boost/shared_ptr.hpp>

// Local Header Files
#include "prodef.h"
#include "DataServer.h"
#include "Query.h"
#include "TServerInfoDisplayer.h"
#include "MapServerManager.h"
#include "CrashDump.h"

#ifdef DS_AUTHORITY
#include <WinlicenseSDK.h>
#else
#define VM_TIGER_BLACK_START
#define VM_TIGER_BLACK_END
#define VM_TIGER_LONDON_START
#define VM_TIGER_LONDON_END
#define VM_TIGER_RED_START
#define VM_TIGER_RED_END
#define VM_DOLPHIN_RED_START
#define VM_DOLPHIN_RED_END
#define VM_DOLPHIN_WHITE_START
#define VM_DOLPHIN_WHITE_END
#define ENCODE_START
#define ENCODE_END
#define CODEREPLACE_START
#define CODEREPLACE_END
#define MUTATE_START
#define MUTATE_END
#define CHECK_PROTECTION
#define CLEAR_START
#define CLEAR_END
#endif

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

