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

typedef struct _WSAOVERLAPPED {
	DWORD    Internal;
	DWORD    InternalHigh;
	DWORD    Offset;
	DWORD    OffsetHigh;
	DWORD	 hEvent;
} WSAOVERLAPPED, FAR * LPWSAOVERLAPPED;

typedef struct _WSABUF {
	ULONG len;     /* the length of the buffer */
	_Field_size_bytes_(len) CHAR FAR *buf; /* the pointer to the buffer */
} WSABUF, FAR * LPWSABUF;

/*
#ifndef _SYSTEMTIME
#define _SYSTEMTIME
typedef struct _SYSTEMTIME {
	WORD wYear;
	WORD wMonth;
	WORD wDayOfWeek;
	WORD wDay;
	WORD wHour;
	WORD wMinute;
	WORD wSecond;
	WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;
typedef SYSTEMTIME* LPSYSTEMTIME;
#endif
*/

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