#ifndef _MU_CONNECTPROTOCOL_H_
#define _MU_CONNECTPROTOCOL_H_

#include "StdAfx.h"
#include "protocolStructs.h"
#include "IOCP.h"
#include "ServerData.h"
#include "ConnectServer.h"
#include "ConnectEngine.h"

struct PMSG_SERVER_SELECT;
struct PMSG_CLIENTVERSION;

void CSProtocolCore(BYTE protoNum, BYTE *aRecv, int aLen, int aIndex, bool Encrypt, int serial);
void UDPProtocolCore(BYTE hCode, LPBYTE aRecv, int aLen);
void UDPSetServerInfo(PMSG_SERVERINFO * aRecv);
void SCSendServerList(CGameObject &lpObj);
void SCSendServerInfo(CGameObject &lpObj, PMSG_SERVER_SELECT * aRecv);
void SCConnectResultSend(CGameObject &lpObj, BYTE btResult);
void SCSendNews(CGameObject &lpObj);
void SCSendAutoUpdateData(CGameObject &lpObj, PMSG_CLIENTVERSION * aRecv);
void UDPInit();

#endif
