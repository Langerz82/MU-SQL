// Eprotocol.cpp: implementation of the Eprotocol class.
//
//////////////////////////////////////////////////////////////////////

#include "Eprotocol.h"
#include "IOCP.h"
#include "Main.h"
#include "ServerEngine.h"

#define szModule "EProtocol"

//CQuery g_EventDB;

void InitEventDB()
{

	if(GetPrivateProfileInt("SETTINGS","UseEventServer",1,".\\IGC.ini") != TRUE)
	{
		return;
	}

	if ( TRUE )
	{
		sLog->outBasic("[EventDB] Connection Successfull!");
	}
	else
	{
		sLog->outError("[EventDB] Error On Connection!");
	}
}


void EProtocolCore(CGameObject &lpObj, BYTE HeadCode, LPBYTE aRecv, int iSize)
{
#if (TRACE_PACKET == 1 )
	LogAddHeadHex("EVENT_SERVER", aRecv, iSize);
#endif
	//g_ServerInfoDisplayer.CheckEVDSConnection(TRUE);
	switch ( HeadCode )
	{
	case 0x01:
		EGAnsEventChipInfo(aIndex,(PMSG_REQ_VIEW_EC_MN *)aRecv);
		break;
	case 0x02:
		EGAnsRegEventChipInfo(aIndex,(PMSG_REQ_REGISTER_EVENTCHIP *)aRecv);
		break;
	case 0x04:
		EGAnsResetEventChip(aIndex,(PMSG_REQ_RESET_EVENTCHIP *)aRecv);
		break;
	case 0x05:
		EGAnsEventStoneInfo(aIndex,(PMSG_REQ_VIEW_EC_MN *)aRecv);
		break;
	case 0x06:
		EGAnsRegEventStoneInfo(aIndex,(PMSG_REQ_REGISTER_STONES *)aRecv);
		break;
	case 0x07:
		EGAnsDeleteStones(aIndex,(PMSG_REQ_DELETE_STONES *)aRecv);
		break;
	case 0x08:
		EGAns2AnivRegSerial(aIndex,(PMSG_REQ_2ANIV_SERIAL *)aRecv);
		break;
	case 0x09:
		EGAnsResetStoneInfo(aIndex,(PMSG_REQ_RESET_EVENTCHIP *)aRecv);
		break;
	case 0x15:
		EGAnsRegCCOfflineGift(aIndex,(PMSG_REQ_REG_CC_OFFLINE_GIFT *)aRecv);
		break;
	case 0x16:
		EGAnsRegDLOfflineGift(aIndex,(PMSG_REQ_REG_DL_OFFLINE_GIFT *)aRecv);
		break;
	case 0x17:
		EGAnsRegHTOfflineGift(aIndex,(PMSG_REQ_REG_HT_OFFLINE_GIFT *)aRecv);
		break;
	case 0x18:
		EGAnsRegLuckyCoin(aIndex,(PMSG_REQ_REGISTER_LUCKYCOIN *)aRecv);
		break;
	case 0x19:
		EGAnsLuckyCoinInfo(aIndex,(PMSG_REQ_LUCKYCOIN *)aRecv);
		break;
	}
	//g_ServerInfoDisplayer.CheckEVDSConnection(FALSE);
}

void EGAnsEventChipInfo(CGameObject &lpObj,PMSG_REQ_VIEW_EC_MN * lpMsg)
{
	int test = false;
	PMSG_ANS_VIEW_EC_MN pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x01;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	std::memcpy(pMsg.szUID, lpMsg->szUID,sizeof(pMsg.szUID));
	pMsg.szUID[10] = 0;

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void EGAnsRegEventChipInfo(CGameObject &lpObj,PMSG_REQ_REGISTER_EVENTCHIP * lpMsg)
{
	PMSG_ANS_REGISTER_EVENTCHIP pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x02;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	pMsg.Pos = lpMsg->Pos;

	strcpy(pMsg.szUID,lpMsg->szUID);
	pMsg.szUID[10] = 0x00;
	pMsg.bSUCCESS = FALSE;

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void EGAnsResetEventChip(CGameObject &lpObj,PMSG_REQ_RESET_EVENTCHIP * lpMsg)
{
	PMSG_ANS_RESET_EVENTCHIP pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x04;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	strcpy(pMsg.szUID,lpMsg->szUID);
	pMsg.szUID[10] = 0;

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void EGAnsEventStoneInfo(CGameObject &lpObj,PMSG_REQ_VIEW_EC_MN * lpMsg)
{
	PMSG_ANS_VIEW_STONES pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x05;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	strcpy(pMsg.szUID,lpMsg->szUID);
	pMsg.szUID[10] = 0x00;

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void EGAnsRegEventStoneInfo(CGameObject &lpObj,PMSG_REQ_REGISTER_STONES * lpMsg)
{
	PMSG_ANS_REGISTER_STONES pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x06;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	pMsg.iPosition = lpMsg->iPosition;
	strcpy(pMsg.szUID,lpMsg->szUID);
	pMsg.szUID[10] = 0x00;

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void EGAnsDeleteStones(CGameObject &lpObj,PMSG_REQ_DELETE_STONES * lpMsg)
{
	PMSG_ANS_DELETE_STONES pMsg = {0};

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x07;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	strcpy(pMsg.szUID,lpMsg->szUID);
	pMsg.szUID[10] = 0x00;

	pMsg.bSUCCESS = FALSE;

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void EGAns2AnivRegSerial(CGameObject &lpObj,PMSG_REQ_2ANIV_SERIAL * lpMsg)
{
	PMSG_ANS_2ANIV_SERIAL pMsg = {0};

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x08;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	char szName[11] = {0};
	memcpy(szName, lpMsg->szUID, 10);
	pMsg.iINDEX = lpMsg->iINDEX;
	memcpy(pMsg.szUID, szName, 10);

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void EGAnsResetStoneInfo(CGameObject &lpObj,PMSG_REQ_RESET_EVENTCHIP * lpMsg)
{
	PMSG_ANS_RESET_EVENTCHIP pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x09;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	char szName[11] = {0};
	memcpy(szName, lpMsg->szUID, 10);
	pMsg.iINDEX = lpMsg->iINDEX;
	memcpy(pMsg.szUID, szName, 10);
	pMsg.szUID[10] = 0;

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void EGAnsRegCCOfflineGift(CGameObject &lpObj,PMSG_REQ_REG_CC_OFFLINE_GIFT * lpMsg)
{
	PMSG_ANS_REG_CC_OFFLINE_GIFT pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x15;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	pMsg.wServerCode = lpMsg->wServerCode;
	strcpy(pMsg.szUID,lpMsg->szUID);
	strcpy(pMsg.szNAME,lpMsg->szNAME);
	pMsg.szUID[10] = 0x00;
	pMsg.szNAME[10] = 0x00;
	
	pMsg.iResultCode = 0;

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void EGAnsRegDLOfflineGift(CGameObject &lpObj,PMSG_REQ_REG_DL_OFFLINE_GIFT * lpMsg)
{
	PMSG_ANS_REG_DL_OFFLINE_GIFT pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x16;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	pMsg.wServerCode = lpMsg->wServerCode;
	strcpy(pMsg.szUID,lpMsg->szUID);
	strcpy(pMsg.szNAME,lpMsg->szNAME);
	pMsg.szUID[10] = 0x00;
	pMsg.szNAME[10] = 0x00;
	
	pMsg.iResultCode = 0;

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void EGAnsRegHTOfflineGift(CGameObject &lpObj,PMSG_REQ_REG_HT_OFFLINE_GIFT * lpMsg)
{
	PMSG_ANS_REG_HT_OFFLINE_GIFT pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x17;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	pMsg.wServerCode = lpMsg->wServerCode;
	strcpy(pMsg.szUID,lpMsg->szUID);
	strcpy(pMsg.szNAME,lpMsg->szNAME);
	pMsg.szUID[10] = 0x00;
	pMsg.szNAME[10] = 0x00;
	
	pMsg.iResultCode = 0;

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void EGAnsLuckyCoinInfo(CGameObject &lpObj,PMSG_REQ_LUCKYCOIN * lpMsg)
{
	int test = false;
	PMSG_ANS_LUCKYCOIN pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x19;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iIndex= lpMsg->iIndex;
	memcpy(pMsg.szUID,lpMsg->szUID, sizeof(pMsg.szUID));
	pMsg.szUID[10] = 0;

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

void EGAnsRegLuckyCoin(CGameObject &lpObj,PMSG_REQ_REGISTER_LUCKYCOIN * lpMsg)
{
	PMSG_ANS_REG_LUCKYCOIN pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x18;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iIndex= lpMsg->iIndex;
	pMsg.Pos = lpMsg->btPos;

	strcpy(pMsg.szUID,lpMsg->szAccountID);
	pMsg.szUID[10] = 0x00;
	pMsg.Result = FALSE;

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg));
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

