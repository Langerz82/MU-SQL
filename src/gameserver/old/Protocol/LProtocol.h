#ifndef _MU_LPROTOCOL_H
#define _MU_LPROTOCOL_H

#include "StdAfx.h"

class CConServ
{
public:
	CConServ();
	~CConServ();

	void ConnectResultSend(int userIndex);
	void ServerListSend(int userIndex);
	void GetServerList(int userIndex);
	void GetServerInfo(int userIndex, USHORT ServerId);
	void LoadNewsFile(LPSTR szFile);
	void SendNews(int userIndex);
	void LoadServerList(LPSTR szFile);
	CONNECT_OBJECT cObj[1000];

}; 

extern CConServ g_ConnectServer;

void LProtocolCore(int userIndex, BYTE hCode, LPBYTE aRecv, int aLen);
void AddServer(PMSG_SERVERINFO * pMsg);
void LSProtocolCore(BYTE hCode, LPBYTE aRecv, int aLen);
//extern WzUdp g_CSSocket;

#endif

