#ifndef _CUSTTYPEDEF_H
#define _CUSTTYPEDEF_H

#include "CGameObject.h"

class CGameObject;

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

typedef CGameObject *LPGameObject;

#endif