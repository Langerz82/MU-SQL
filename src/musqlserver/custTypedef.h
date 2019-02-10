#ifndef _CUSTTYPEDEF_H
#define _CUSTTYPEDEF_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000.

//#ifndef USER_H__
//#define USER_H__
//#include "User/user.h"
//#endif

typedef int BOOL;
typedef wchar_t WCHAR;

typedef unsigned char UCHAR;
typedef unsigned char* LPBYTE;
typedef unsigned char BYTE;
typedef unsigned short USHORT;
typedef unsigned short WORD;
typedef const WCHAR* LPCWSTR;
typedef unsigned long DWORD, ULONG;
typedef void* HANDLE;
typedef unsigned long long UINT64;
typedef char* LPSTR;

typedef long long LONGLONG;
typedef long long time_t;

typedef unsigned long long SOCKET;



typedef struct MU_WSAOVERLAPPED {
	DWORD    Internal;
	DWORD    InternalHigh;
	DWORD    Offset;
	DWORD    OffsetHigh;
	DWORD	 hEvent;
} FAR * LPMU_WSAOVERLAPPED;

typedef struct MU_WSABUF {
	ULONG len;
	CHAR* buf;
} *LPMU_WSABUF;


#ifndef NULL
	#define NULL 0
#endif

#ifndef TRUE
	#define TRUE 1
#endif
#ifndef FALSE
	#define FALSE 0
#endif

struct OBJECTSTRUCT;
typedef OBJECTSTRUCT* LPOBJ;

#endif