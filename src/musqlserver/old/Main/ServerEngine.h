#ifndef _MU_SERVER_ENGINE_
#define _MU_SERVER_ENGINE_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "IOCP.h"

class CGameObject;

enum eSERVER_STATE
{
	SS_CLOSED,
	SS_CONNECTED,
	SS_LOGGED,
	SS_GUILD,
	SS_CHAT,
};

typedef void (*WsProtocolCore)(int, BYTE, LPBYTE, int);

#ifndef MAX_SERVER_TYPE
#define MAX_SERVER_TYPE (ST_EXDATASERVER+1)
#endif

struct STR_SERVER_ENGINE;
struct IPList;

void gObjServerInit();
int gObjServerAddSearch();
int gObjServerAdd(SOCKET Socket, char * Ip, int ServerIndex, eSERVER_TYPE eServerType);
void gObjServerDel(int aIndex);
void LoadAllowableIpList(LPSTR filename);

void gObjConnectStatSyncThread();

extern IPList g_IpList[25];
extern int ipcount;
extern int servercount;
extern STR_SERVER_ENGINE* g_Server;

#endif
