#ifndef _CUSTTYPEDEF_H
#define _CUSTTYPEDEF_H

typedef int BOOL;
typedef wchar_t WCHAR;

typedef unsigned char BYTE, *LPBYTE, UCHAR;
typedef unsigned short USHORT, WORD;
typedef const WCHAR* LPCWSTR;
typedef unsigned long ULONG;
typedef void* HANDLE;
typedef unsigned long long UINT64;
typedef char* LPSTR;

typedef long long LONGLONG;


typedef unsigned long long SOCKET;

#ifdef _WIN32
typedef unsigned long DWORD;
typedef long long time_t;

#endif


#endif
