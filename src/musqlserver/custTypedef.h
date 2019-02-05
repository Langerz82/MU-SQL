
#ifndef __CUSTTYPEDEF_H
#define __CUSTTYPEDEF_H


typedef int BOOL;
typedef wchar_t WCHAR;

typedef unsigned char UCHAR;
typedef unsigned char* LPBYTE;
typedef unsigned char BYTE;
typedef unsigned short USHORT;
typedef unsigned short WORD;
typedef const WCHAR* LPCWSTR;
typedef unsigned long DWORD;
typedef void* HANDLE;
typedef unsigned long long UINT64;
typedef char* LPSTR;

typedef long long LONGLONG;
typedef long long time_t;

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


//typedef OBJECTSTRUCT* LPOBJECTSTRUCT;

#endif
