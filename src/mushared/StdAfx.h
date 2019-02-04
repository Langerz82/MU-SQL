// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#pragma warning ( disable : 4786 )	// Disable Warning of Large Debuf Strings ( truncated to 255 len )

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _WIN32_WINNT 0x0601
#define WINVER 0x0601

//#define VLD_FORCE_ENABLE		// Visual Leak Detector
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#define NOMINMAX
#include <new.h>
#include <vector>
#include <set>
#include <stack>
#include <cmath>
#include <math.h>
#include <iostream>
#include <fstream>
#include <random>
#include <tchar.h>
#include <strsafe.h>
#include <time.h>
#include <Rpc.h>
#include <regex>
#include <atltime.h>
#include "strtk.hpp"
// Windows Header Files:
#include <Windows.h>
#include <WinSock2.h>
#include <MMSystem.h>
#include <CommCtrl.h>
#include <cstdlib>
#include <map>
#include <string>

//#include <vld.h>
// Local Header Files
#include "Lang.h"
#include "IniReader.h"

// Crypto++
#include <sha.h>
#include <md4.h>
#include <md5.h>
#include <ripemd.h>
#include <rsa.h>
#include <3way.h>
#include <cast.h>
#include <rc5.h>
#include <rc6.h>
#include <mars.h>
#include <idea.h>
#include <gost.h>
#include <tea.h>
#include <twofish.h>
#include <des.h>
#include <skipjack.h>
#include <safer.h>
#include <rsa.h>
#include <osrng.h>
#include <rijndael.h>
#include <integer.h>

// RSA
#include <mpir.h>
#include <gmp.h>
#include <RNG.h>
#include <SRSA.h>
#include <MuRSA.h>

// XML
#include <pugixml.hpp>

// LUA
#include <lua.hpp>

// muParser
#include <muParser.h>

// Boost
#include <boost/random.hpp>
#include <boost/generator_iterator.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/multiprecision/cpp_int.hpp>

//#pragma comment(lib, "pugixml.lib")
#pragma comment(lib, "cryptlib.lib")
#pragma comment(lib, "muparser.lib")
#pragma comment(lib, "lua52.lib")

#pragma comment(lib, "iniparser.lib")

#pragma comment(lib, "winmm.lib")

// TODO: reference additional headers your program requires here

//#define GS_AUTHORITY
#define CRYPTOPP_
//#define EMU_NOCRYPT
//#define C3C4_DISABLECRYPT

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Rpcrt4.lib")
#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "mpir64.lib")

#ifdef GS_AUTHORITY
#include <WinlicenseSDK.h>
#else
#define VM_TIGER_BLACK_START
#define VM_TIGER_BLACK_END
#define VM_TIGER_RED_START
#define VM_TIGER_RED_END
#define VM_TIGER_WHITE_START
#define VM_TIGER_WHITE_END
#define VM_TIGER_LONDON_START
#define VM_TIGER_LONDON_END
#define VM_DOLPHIN_RED_START
#define VM_DOLPHIN_RED_END
#define CHECK_CODE_INTEGRITY(x,y) (x = y);
#define CHECK_PROTECTION(x,y) (x = y);
#define ENCODE_START
#define ENCODE_END
#define CODEREPLACE_START
#define CODEREPLACE_END
#define MUTATE_START
#define MUTATE_END
#define CLEAR_START
#define CLEAR_END
#endif

#define TEST_FIX_MUUN 0

#define TEST_FIX_DARKRAVEN 1

#define ENABLE_SEASON12 1

#define HEADCODE_MOVE 0xD7
#define HEADCODE_POSITION_SET 0x10
#define HEADCODE_ATTACK_NORMAL 0xDF
#define HEADCODE_ATTACK_MULTI_TARGET 0xD3

/*
//== PROTOCOL KOREA ==//
#define PROTOCOL_CODE1 0xD3
#define PROTOCOL_CODE2 0xD7
#define PROTOCOL_CODE3 0xDF
#define PROTOCOL_CODE4 0x10

//==== PROTOCOL ENG  ===//
#define PROTOCOL_CODE1 0xD4
#define HEADCODE_ATTACK_NORMAL 0x11
#define PROTOCOL_CODE3 0x15
#define PROTOCOL_CODE4 0xDB
#endif*/

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

