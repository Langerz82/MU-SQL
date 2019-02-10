#ifndef _MU_LOGINPROTOCOL_H_
#define _MU_LOGINPROTOCOL_H_

#include "stdafx.h"
#include "IOCP.h"
#include "ServerData.h"
#include "ConnectServer.h"
#include "Logging/Log.h"
#include "LoginEngine.h"
#include "Utility/Utils.h"

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
