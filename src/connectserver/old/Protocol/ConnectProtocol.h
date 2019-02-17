#ifndef _MU_CONNECTPROTOCOL_H_
#define _MU_CONNECTPROTOCOL_H_

#include "StdAfx.h"

#include "IOCP.h"
#include "ServerData.h"
#include "Main.h"
#include "ConnectEngine.h"


void CSProtocolCore(BYTE protoNum, BYTE *aRecv, int aLen, STR_CS_USER &Obj, bool Encrypt, int serial);
void UDPProtocolCore(BYTE hCode, LPBYTE aRecv, int aLen);
void UDPSetServerInfo(PMSG_SERVERINFO * aRecv);
void SCSendServerList(STR_CS_USER &Obj);
void SCSendServerInfo(STR_CS_USER &Obj, PMSG_SERVER_SELECT * aRecv);
void SCConnectResultSend(STR_CS_USER &Obj, BYTE btResult);
void SCSendNews(STR_CS_USER &Obj);
void SCSendAutoUpdateData(STR_CS_USER &Obj, PMSG_CLIENTVERSION * aRecv);
void UDPInit();

#endif
