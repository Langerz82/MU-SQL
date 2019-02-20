#ifndef MU_SERVERDEFINES_H
#define MU_SERVERDEFINES_H

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

#else

typedef char* LPTSTR;

#endif


#endif
