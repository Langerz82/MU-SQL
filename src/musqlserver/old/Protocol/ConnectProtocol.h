#ifndef _MU_CONNECTPROTOCOL_H_
#define _MU_CONNECTPROTOCOL_H_

#include "StdAfx.h"
#include "IOCP.h"
#include "ServerData.h"
#include "ConnectServer.h"
#include "ConnectEngine.h"

void CSProtocolCore(BYTE protoNum, BYTE *aRecv, int aLen, int aIndex, bool Encrypt, int serial);
void UDPProtocolCore(BYTE hCode, LPBYTE aRecv, int aLen);
void UDPSetServerInfo(PMSG_SERVERINFO * aRecv);
void SCSendServerList(int aIndex);
void SCSendServerInfo(int aIndex, PMSG_SERVER_SELECT * aRecv);
void SCConnectResultSend(int aIndex, BYTE btResult);
void SCSendNews(int aIndex);
void SCSendAutoUpdateData(int aIndex, PMSG_CLIENTVERSION * aRecv);
void UDPInit();

#endif
