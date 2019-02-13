#ifndef _MU_CONNECTPROTOCOL_H_
#define _MU_CONNECTPROTOCOL_H_

#include "StdAfx.h"
#include "protocolStructs.h"
#include "IOCP.h"
#include "ServerData.h"
#include "ConnectServer.h"
#include "ConnectEngine.h"

class CUserData;

void CSProtocolCore(BYTE protoNum, BYTE *aRecv, int aLen, CUserData &Obj, bool Encrypt, int serial);
void UDPProtocolCore(BYTE hCode, LPBYTE aRecv, int aLen);
void UDPSetServerInfo(PMSG_SERVERINFO * aRecv);
void SCSendServerList(CUserData &Obj);
void SCSendServerInfo(CUserData &Obj, PMSG_SERVER_SELECT * aRecv);
void SCConnectResultSend(CUserData &Obj, BYTE btResult);
void SCSendNews(CUserData &Obj);
void SCSendAutoUpdateData(CUserData &Obj, PMSG_CLIENTVERSION * aRecv);
void UDPInit();

#endif
