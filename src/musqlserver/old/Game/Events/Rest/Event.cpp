
#include "Event.h"
#include "GameProtocol.h"
#include "GameServer.h"
#include "Main.h"
#include "Utility/util.h"
#include "BagManager.h"
#include "configread.h"

void EventChipEventProtocolCore(BYTE protoNum, LPBYTE aRecv, int aLen)
{
#if (TRACE_PROTOCOL==1)
	LogAddHeadHex("EVENT_SERVER", aRecv, aLen);
#endif

	switch ( protoNum )
	{
		case 0x01:
			EGRecvEventChipInfo((PMSG_ANS_VIEW_EC_MN *)aRecv);
			break;
		case 0x02:
			EGResultRegEventChip((PMSG_ANS_REGISTER_EVENTCHIP *)aRecv);
			break;
		case 0x03:
			EGRecvRegMutoNum((PMSG_ANS_REGISTER_MUTONUM *)aRecv);
			break;
		case 0x04:
			EGRecvChangeRena((PMSG_ANS_RESET_EVENTCHIP *)aRecv);
			break;
		case 0x05:
			EGRecvStoneInfo((PMSG_ANS_VIEW_STONES *)aRecv);
			break;
		case 0x06:
			EGRecvRegStone((PMSG_ANS_REGISTER_STONES *)aRecv);
			break;
		case 0x07:
			EGRecvDeleteStone((PMSG_ANS_DELETE_STONES *)aRecv);
			break;
		case 0x09:
			EGRecvChangeStones((PMSG_ANS_RESET_EVENTCHIP *)aRecv);
			break;
		case 0x08:
			EGRecv2AnvRegSerial((PMSG_ANS_2ANIV_SERIAL *)aRecv);
			break;
		case 0x10:
			EGRecvRegRingGift((PMSG_ANS_REG_RINGGIFT *)aRecv);
			break;
		case 0x15:
			EGAnsRegCCOfflineGift((PMSG_ANS_REG_CC_OFFLINE_GIFT *)aRecv);
			break;
		case 0x16:
			EGAnsRegDLOfflineGift((PMSG_ANS_REG_DL_OFFLINE_GIFT *)aRecv);
			break;
		case 0x17:
			EGAnsRegHTOfflineGift((PMSG_ANS_REG_HT_OFFLINE_GIFT *)aRecv);
			break;
		case 0x18:
			EGAnsRegLuckyCoin((PMSG_ANS_REG_LUCKYCOIN *)aRecv);
			break;
		case 0x19:
			EGAnsLuckyCoinInfo((PMSG_ANS_LUCKYCOIN *)aRecv);
			break;
		case 0x20:
			EGAnsSantaCheck(reinterpret_cast<PMSG_ANS_SANTACHECK *>(aRecv));
			break;
		case 0x21:
			EGAnsSantaGift(reinterpret_cast<PMSG_ANS_SANTAGIFT *>(aRecv));
			break;
	}
}

void FireworksOpenEven(CGameObject &lpObj)
{
	PMSG_SERVERCMD ServerCmd;

	PHeadSubSetB((LPBYTE)&ServerCmd, 0xF3, 0x40, sizeof(ServerCmd));
	ServerCmd.CmdType = 0;
	ServerCmd.X = lpObj.X;
	ServerCmd.Y = lpObj.Y;

	gGameProtocol.MsgSendV2(lpObj, (LPBYTE)&ServerCmd, sizeof(ServerCmd));
	IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (LPBYTE)&ServerCmd, sizeof(ServerCmd));
}

void ChristmasFireCrackDrop(CGameObject &lpObj) //season 4.5 add-on
{
	PMSG_SERVERCMD ServerCmd;

	PHeadSubSetB((LPBYTE)&ServerCmd, 0xF3, 0x40, sizeof(ServerCmd));
	ServerCmd.CmdType = 59;
	ServerCmd.X = lpObj.X;
	ServerCmd.Y = lpObj.Y;
	gGameProtocol.MsgSendV2(lpObj, (LPBYTE)&ServerCmd, sizeof(ServerCmd));
	IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (LPBYTE)&ServerCmd, sizeof(ServerCmd));
}

#pragma warning ( disable : 4101 )
void EGRecvEventChipInfo(PMSG_ANS_VIEW_EC_MN * aRecv)
{
	CGameObject lpObj = &gGameObjects[aRecv->iINDEX];

	PMSG_EVENTCHIPINFO eventchipeventinfo;
	char msg[255];

	PHeadSetB((LPBYTE)&eventchipeventinfo, 0x94, sizeof(eventchipeventinfo));
	eventchipeventinfo.Type = 0;
	eventchipeventinfo.ChipCount = aRecv->nEVENT_CHIPS;
	lpObj.EventChipCount = aRecv->nEVENT_CHIPS;
	lpObj.MutoNumber = aRecv->iMUTO_NUM;

	IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (LPBYTE)&eventchipeventinfo, eventchipeventinfo.h.size);

	lpObj.UseEventServer = FALSE;
}
#pragma warning ( default : 4101 )


void EGResultRegEventChip(PMSG_ANS_REGISTER_EVENTCHIP * aRecv)
{
	PMSG_REGEVENTCHIP_RESULT Result;
	CGameObject lpObj;
	int aIndex;

	PHeadSetB((LPBYTE)&Result, 0x95, sizeof(Result));
	lpObj = &gGameObjects[aRecv->iINDEX];
	aIndex = aRecv->iINDEX;

	if ( aRecv->bSUCCESS == FALSE )
	{
		Result.ChipCount = -1;
		sLog->outBasic("[EventChip] [%s][%s] RegEventServer Fail (RegEventchip) %d",
			lpObj.AccountID, lpObj.Name, aRecv->Pos);

	}
	else
	{
		Result.ChipCount = aRecv->nEVENT_CHIPS;
		gObjInventoryDeleteItem(aIndex, aRecv->Pos);
		gGameProtocol.GCInventoryItemDeleteSend(aIndex, aRecv->Pos, 1);

		sLog->outBasic("[EventChip] [%s][%s] Delete EventChip (%d)",
			lpObj.AccountID, lpObj.Name, aRecv->Pos);
	}

	Result.Type = 0;

	IOCP.DataSend(lpObj.m_Index, (LPBYTE)&Result, Result.h.size);

	lpObj.UseEventServer = FALSE;
}



void EGRecvRegMutoNum( PMSG_ANS_REGISTER_MUTONUM* aRecv)
{
	CGameObject lpObj;
	int aIndex;
	

	lpObj = &gGameObjects[aRecv->iINDEX];
	aIndex = aRecv->iINDEX;

	PMSG_GETMUTONUMBER_RESULT Result;

	PHeadSetB((LPBYTE)&Result, 0x96, sizeof(Result));
	
	if ( gObjFind10EventChip(aIndex) == FALSE )
	{
		Result.MutoNum[0] = -1;
		Result.MutoNum[1] = 0;
		Result.MutoNum[2] = 0;

		IOCP.DataSend(lpObj.m_Index, (LPBYTE)&Result, Result.h.size);

		lpObj.UseEventServer = FALSE;

		return;
	}

	gObjDelete10EventChip(aIndex);
	Result.MutoNum[0] = aRecv->iMUTO_NUM / 1000000;
	Result.MutoNum[1] = aRecv->iMUTO_NUM / 1000 - aRecv->iMUTO_NUM / 1000000 * 1000;
	Result.MutoNum[2] = aRecv->iMUTO_NUM % 1000;
	lpObj.MutoNumber = aRecv->iMUTO_NUM;
	
	sLog->outBasic("[EventChip] [%s][%s] Make MutoNumber %d,%d,%d",
		lpObj.AccountID, lpObj.Name, 
		Result.MutoNum[0], Result.MutoNum[1], Result.MutoNum[2]);

	IOCP.DataSend(lpObj.m_Index, (LPBYTE)&Result, Result.h.size);
	lpObj.UseEventServer = FALSE;
}


void EGRecvChangeRena( PMSG_ANS_RESET_EVENTCHIP* aRecv)
{
	PMSG_REGEVENTCHIP_RESULT Result;
	CGameObject lpObj;
	int aIndex;

	PHeadSetB((LPBYTE)&Result, 0x95, sizeof(Result));
	lpObj = &gGameObjects[aRecv->iINDEX];
	aIndex = aRecv->iINDEX;

	if ( aRecv->bSUCCESS != FALSE )
	{
		lpObj.m_PlayerData->Money += lpObj.EventChipCount * 3000;
		gGameProtocol.GCMoneySend(aIndex, lpObj.m_PlayerData->Money);

		sLog->outBasic("[EventChip] [%s][%s] ChangeRena AddMoney(%d)",
			lpObj.AccountID, lpObj.Name, lpObj.EventChipCount * 3000);
	}
	else
	{
		sLog->outBasic("[EventChip] [%s][%s] ChangeRena Fail",
			lpObj.AccountID, lpObj.Name);
	}

	Result.ChipCount = 0;
	lpObj.EventChipCount = 0;

	IOCP.DataSend(lpObj.m_Index, (LPBYTE)&Result, Result.h.size);

	lpObj.UseEventServer = FALSE;
}


CGameObject pEventObj;

void EGRecvStoneInfo( PMSG_ANS_VIEW_STONES* aRecv)
{
	CGameObject lpObj = &gGameObjects[aRecv->iINDEX];

	PMSG_EVENTCHIPINFO Result;
	
	PHeadSetB((LPBYTE)&Result, 0x94, sizeof(Result));

	if ( aRecv->bSUCCESS )
		lpObj.iStoneCount = aRecv->iStoneCount;
	else
		lpObj.iStoneCount = 0;

	lpObj.MutoNumber = 0;
	Result.Type = 3;
	Result.ChipCount = aRecv->iStoneCount;

	IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (LPBYTE)&Result, Result.h.size);

	char msg[128];
	wsprintf(msg, Lang.GetText(0,78), Result.ChipCount);
	gGameProtocol.ChatTargetSend(pEventObj, msg, lpObj.m_Index);
}


void EGRecvRegStone( PMSG_ANS_REGISTER_STONES* aRecv)
{
	PMSG_REGEVENTCHIP_RESULT Result;
	CGameObject lpObj;
	int aIndex;
	

	PHeadSetB((LPBYTE)&Result, 0x95, sizeof(Result));
	lpObj = &gGameObjects[aRecv->iINDEX];
	aIndex = aRecv->iINDEX;

	if ( aRecv->bSUCCESS != FALSE )
	{
		Result.ChipCount = aRecv->iStoneCount;
		gObjInventoryDeleteItem(aIndex, aRecv->iPosition);
		gGameProtocol.GCInventoryItemDeleteSend(aIndex, aRecv->iPosition, 1);

		sLog->outBasic("[Stone] [%s][%s] Delete Stones",
			lpObj.AccountID, lpObj.Name);
	}
	else
	{
		Result.ChipCount = -1;
		
		sLog->outBasic("[Stone] [%s][%s] RegEventServer Fail (Stones : %d)",
			lpObj.AccountID, lpObj.Name, aRecv->iStoneCount);
	}

	IOCP.DataSend(lpObj.m_Index, (LPBYTE)&Result, Result.h.size);

	lpObj.UseEventServer = FALSE;
}


void EGRecvDeleteStone( PMSG_ANS_DELETE_STONES* aRecv)
{
	return;
}


void EGRecvChangeStones( PMSG_ANS_RESET_EVENTCHIP* aRecv)
{
	PMSG_REGEVENTCHIP_RESULT Result;
	CGameObject lpObj;
	int aIndex;
	

	PHeadSetB((LPBYTE)&Result, 0x95, sizeof(Result));
	lpObj = &gGameObjects[aRecv->iINDEX];
	aIndex = aRecv->iINDEX;

	if ( aRecv->bSUCCESS != FALSE )
	{
		lpObj.m_PlayerData->Money += lpObj.iStoneCount * 3000;
		gGameProtocol.GCMoneySend(aIndex, lpObj.m_PlayerData->Money);

		sLog->outBasic("[Stones] [%s][%s] ChangeRena AddMoney(%d)",
			lpObj.AccountID, lpObj.Name, lpObj.iStoneCount*3000);
	}
	else
	{
		sLog->outBasic("[Stones] [%s][%s] ChangeRena Fail",
			lpObj.AccountID, lpObj.Name);
	}

	Result.ChipCount = 0;
	lpObj.iStoneCount = 0;

	IOCP.DataSend(lpObj.m_Index, (LPBYTE)&Result, Result.h.size);

	lpObj.UseEventServer = FALSE;
}

struct PMSG_ANS_2ANV_LOTTO_EVENT
{
	PBMSG_HEAD2 h;	// C1:9D
	BYTE btIsRegistered;	// 3
	char szGIFT_NAME[64];	// 4
};


#define GIFT_2ANV_MAP	235
#define MAX_GIFT_2ANV	50
#define GIFT_2ANV_RANGE(x)  ( (((x))<0)?FALSE:(((x))>MAX_GIFT_2ANV-1)?FALSE:TRUE )

BOOL g_bRingEventItemTextLoad = FALSE;
char g_sz2ANV_GIFT_NAME[MAX_GIFT_2ANV][64];

void EGRecv2AnvRegSerial( PMSG_ANS_2ANIV_SERIAL* aRecv)
{
	PMSG_ANS_2ANV_LOTTO_EVENT Result;

	PHeadSetB((LPBYTE)&Result, 0x9D, sizeof(Result));

	if ( !ObjectMaxRange(aRecv->iINDEX))
	{
		sLog->outBasic("[Mu_2Anv_Event] Error : Index is out of bound [%d]", aRecv->iINDEX);
		return;
	}

	if ( gGameObjects[aRecv->iINDEX]->Connected <= PLAYER_LOGGED )
	{
		sLog->outBasic("[Mu_2Anv_Event] Error : Index is out of bound [%d]", aRecv->iINDEX);
		return;
	}

	Result.szGIFT_NAME[0] = 0;

	if ( aRecv->btIsRegistered == FALSE )
	{
		Result.btIsRegistered = FALSE;
		
		if ( !GIFT_2ANV_RANGE(aRecv->iGiftNumber-1) )
		{
			sLog->outBasic("[Mu_2Anv_Event] Error : Gift Index is out of bound [%d]", aRecv->iGiftNumber);
			Result.btIsRegistered = 2;
		}

		if ( gGameObjects[aRecv->iINDEX]->Connected > PLAYER_LOGGED )
		{
			PMSG_SERVERCMD ServerCmd;

			PHeadSubSetB((LPBYTE)&ServerCmd, 0xF3, 0x40, sizeof(ServerCmd));
			ServerCmd.CmdType = 0;
			ServerCmd.X = gGameObjects[aRecv->iINDEX]->X;
			ServerCmd.Y = gGameObjects[aRecv->iINDEX]->Y;

			gGameProtocol.MsgSendV2(&gGameObjects[aRecv->iINDEX], (LPBYTE)&ServerCmd, sizeof(ServerCmd));
			IOCP.DataSend(aRecv->iINDEX, (LPBYTE)&ServerCmd, sizeof(ServerCmd));
		}

		if ( g_bRingEventItemTextLoad )
		{
			strcpy(Result.szGIFT_NAME, g_sz2ANV_GIFT_NAME[aRecv->iGiftNumber-1]);
		}

		switch ( aRecv->iGiftNumber )
		{
			case 1:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(14, 13),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(14, 13)].Name);
				}
				break;
			case 2:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(14, 14),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(14, 14)].Name);
				}
				break;
			case 3:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(12, 15),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(12, 15)].Name);
				}
				break;
			case 4:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(14, 11),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(14, 11)].Name);
				}
				break;
			case 5:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(14, 11),
					3, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, "Heart of Love");
				}
				break;
			case 6:
				gGameObjects[aRecv->iINDEX]->m_PlayerData->Money += 500000;
				gGameProtocol.GCMoneySend(aRecv->iINDEX, gGameObjects[aRecv->iINDEX]->m_PlayerData->Money);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, "500,000 Zen");
				}
				break;
			case 7:
				gGameObjects[aRecv->iINDEX]->m_PlayerData->Money += 50000;
				gGameProtocol.GCMoneySend(aRecv->iINDEX, gGameObjects[aRecv->iINDEX]->m_PlayerData->Money);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, "50,000 Zen");
				}
				break;
			case 8:	case 9:	case 10:
				gGameObjects[aRecv->iINDEX]->m_PlayerData->Money += 30000;
				gGameProtocol.GCMoneySend(aRecv->iINDEX, gGameObjects[aRecv->iINDEX]->m_PlayerData->Money);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, "30,000 Zen");
				}
				break;
			case 11:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(12, 19),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(12, 11)].Name);	// #error Change 11 to 19
				}
				break;
			case 12:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(12, 18),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(12, 18)].Name);
				}
				break;
			case 13:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(12, 17),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(12, 17)].Name);
				}
				break;
			case 14:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(12, 16),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(12, 16)].Name);
				}
				break;
			case 15:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(12, 14),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(12, 14)].Name);
				}
				break;
			case 16:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(12, 13),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(12, 13)].Name);
				}
				break;
			case 17:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(12, 12),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(12, 12)].Name);
				}
				break;
			case 18:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(13, 2),
					0, 255, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(13, 2)].Name);
				}
				break;
			case 19:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(13, 3),
					0, 255, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(13, 3)].Name);	// #error Change 2 to 3
				}
				break;
			case 20:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(14, 16),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(14, 16)].Name);
				}
				break;
			case 21:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(13, 0),
					0, 255, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(13, 0)].Name);
				}
				break;
			case 22:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(13, 1),
					0, 255, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(13, 1)].Name);
				}
				break;
			case 23:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(14, 11),
					1, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(14, 11)].Name);
				}
				break;
			case 24:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(14, 11),
					9, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(14, 11)].Name);
				}
				break;
			case 25:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(14, 11),
					10, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(14, 11)].Name);
				}
				break;
			case 26:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(14, 19),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(14, 19)].Name);
				}
				break;
			case 27:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(14, 19),
					1, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(14, 19)].Name);
				}
				break;
			case 28:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(14, 19),
					2, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(14, 19)].Name);
				}
				break;
			case 29:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(14, 19),
					3, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(14, 19)].Name);
				}
				break;
			case 30:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(14, 19),
					4, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(14, 19)].Name);
				}
				break;
			case 31:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(14, 11),
					2, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(14, 11)].Name);
				}
				break;
			case 32:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(14, 20),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(14, 20)].Name);
				}
				break;
			case 33:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(14, 22),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(14, 22)].Name);
				}
				break;
			case 34:	case 35:	case 36:	case 37:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(13, 15),
					aRecv->iGiftNumber - 34, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(13, 15)].Name);
				}
				break;
			case 38:	case 39:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(14, 11),
					aRecv->iGiftNumber - 27, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(14, 11)].Name);
				}
				break;
			case 40:	case 41:	case 42:	case 43:	case 44:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(14, 20),
					aRecv->iGiftNumber - 39, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(14, 20)].Name);
				}
				break;
			case 45:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(14, 11),
					8, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(14, 11)].Name);
				}
				break;
			case 46:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(14, 41),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(14, 41)].Name);
				}
				break;
			case 47:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(14, 42),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(14, 42)].Name);
				}
				break;
			case 48:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(14, 44),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(14, 44)].Name);
				}
				break;
			case 49:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(14, 43),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(14, 43)].Name);
				}
				break;
			case 50:
				ItemSerialCreateSend(gGameObjects[aRecv->iINDEX]->m_Index, GIFT_2ANV_MAP, 
					gGameObjects[aRecv->iINDEX]->X, gGameObjects[aRecv->iINDEX]->Y,
					ItemGetNumberMake(14, 31),
					0, 0, 0, 0, 0,
					aRecv->iINDEX,
					0, 0, 0, 0, 0);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, ItemAttribute[ITEMGET(14, 31)].Name);
				}
				break;
			default:
				sLog->outBasic("[Mu_2Anv_Event] Error : iGiftNumber is Out of Boud [%d]", aRecv->iGiftNumber);
				break;
		}
	}
	else if ( aRecv->btIsRegistered == 1 ||
			  aRecv->btIsRegistered == 2 ||
			  aRecv->btIsRegistered == 3 ||
			  aRecv->btIsRegistered == 4 ||
			  aRecv->btIsRegistered == 5 )
	{
		Result.btIsRegistered = aRecv->btIsRegistered;
	}
	else
	{
		Result.btIsRegistered = 4;
		sLog->outBasic("[Mu_2Anv_Event] Error : Result Value is Wrong [%d]", aRecv->btIsRegistered);
	}

	sLog->outBasic("[Mu_2Anv_Event] Register Serial Result : %d [%s][%s]",
		aRecv->btIsRegistered, gGameObjects[aRecv->iINDEX]->AccountID, gGameObjects[aRecv->iINDEX]->Name);

	IOCP.DataSend(aRecv->iINDEX, (LPBYTE)&Result, Result.h.size);

	gGameObjects[aRecv->iINDEX]->UseEventServer = FALSE;
}

static const char g_szRingEventOfflineGift[4][32] = { "100µ· ¹Â¹ÝÁö",
													  "10µ· ¹Â¹ÝÁö",
													  "5µ· ¹Â¹ÝÁö",
													  "2µ· ¹Â¹ÝÁö"};
															


void EGRecvRegRingGift( PMSG_ANS_REG_RINGGIFT* aRecv)
{
	gGameObjects[aRecv->iINDEX]->UseEventServer = FALSE;

	if ( gObjIsConnected(aRecv->iINDEX) == FALSE )
		return;

	if ( strcmp(aRecv->szUID, gGameObjects[aRecv->iINDEX]->AccountID))
		return;

	if ( aRecv->btIsRegistered == 1 )
	{
		if ( CHECK_LIMIT(aRecv->btGiftKind-1, 4) )
		{
			char szTemp[256];
			wsprintf(szTemp, "%s´Ô ²²¼­ %s¿¡ ´çÃ·µÇ¼Ì½À´Ï´Ù.",
				gGameObjects[aRecv->iINDEX]->Name, g_szRingEventOfflineGift[aRecv->btGiftKind-1]);

			gGameProtocol.AllSendServerMsg(szTemp);

			sLog->outBasic("[Ring Event] [%s][%s] Register Succeeded Result:%d, Gift:%d",
				gGameObjects[aRecv->iINDEX]->AccountID, gGameObjects[aRecv->iINDEX]->Name,
				aRecv->btIsRegistered, aRecv->btGiftKind);
		}
		else
		{
			sLog->outBasic("[Ring Event] [%s][%s] Register Failed Result:%d, Gift:%d (out of bound, 1~4)",
				gGameObjects[aRecv->iINDEX]->AccountID, gGameObjects[aRecv->iINDEX]->Name,
				aRecv->btIsRegistered, aRecv->btGiftKind);
		}

		return;
	}

	sLog->outBasic("[Ring Event] [%s][%s] Register Failed Result : %d",
		gGameObjects[aRecv->iINDEX]->AccountID, gGameObjects[aRecv->iINDEX]->Name,
		aRecv->btIsRegistered);

	if ( gObjIsConnected(aRecv->iINDEX) == TRUE )
	{
		MapC[gGameObjects[aRecv->iINDEX]->MapNumber]->MoneyItemDrop(100000, (BYTE)gGameObjects[aRecv->iINDEX]->X, (BYTE)gGameObjects[aRecv->iINDEX]->Y);
	}
}


struct PMSG_REQ_BLOODCASTLE_ENTERCOUNT
{
	PBMSG_HEAD2 h;	// C1:0B
	char AccountID[10];	// 3
	char GameID[10];	// D
	int ServerCode;	// 18
	int iObjIndex;	// 1C
};


void EGReqBloodCastleEnterCount(int iIndex)
{
	if ( !gObjIsConnected(iIndex) )
		return;

	PMSG_REQ_BLOODCASTLE_ENTERCOUNT pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBD;
	pMsg.h.subcode = 0x02;
	pMsg.h.size = sizeof(pMsg);
	memcpy(pMsg.AccountID, gGameObjects[iIndex]->AccountID, 10);
	memcpy(pMsg.GameID, gGameObjects[iIndex]->Name, 10);
	pMsg.ServerCode = g_ConfigRead.server.GetGameServerCode() / 20;
	pMsg.iObjIndex = iIndex;

	//wsDataCli.DataSend(reinterpret_cast<char *>(&pMsg), pMsg.h.size);
	// TODO.
}

struct PMSG_ANS_CL_ENTERCOUNT
{
	PBMSG_HEAD2 h;	// C1:9F
	BYTE btEventType;	// 3
	BYTE btLeftEnterCount;	// 4
};

void EGAnsBloodCastleEnterCount( PMSG_ANS_BLOODCASTLE_ENTERCOUNT* lpMsg)
{
	if ( !lpMsg)
		return;

	if ( !gObjIsConnected(lpMsg->iObjIndex))
		return;

	char szAccountID[11] = {0};
	char szName[11] = {0};
	memcpy(szAccountID, lpMsg->AccountID, 10);
	memcpy(szName, lpMsg->GameID, 10);

	if ( strcmp(gGameObjects[lpMsg->iObjIndex]->AccountID, szAccountID) ||
		 strcmp(gGameObjects[lpMsg->iObjIndex]->Name, szName) )
		 return;

	PMSG_ANS_CL_ENTERCOUNT pMsgSend;

	pMsgSend.h.c = 0xC1;
	pMsgSend.h.headcode = 0x9F;
	pMsgSend.h.size = sizeof(pMsgSend);
	pMsgSend.btEventType = 2;
	pMsgSend.btLeftEnterCount = lpMsg->iLeftCount;

	IOCP.DataSend(lpMsg->iObjIndex, (LPBYTE)&pMsgSend, sizeof(pMsgSend));
}



struct PMSG_REQ_REG_CC_OFFLINE_GIFT
{
	PBMSG_HEAD2 h;	// C1:15
	int iINDEX;	// 4
	char szUID[11];	// 8
	WORD wServerCode;	// 14
	char szNAME[11];	// 16
};

void EGReqRegCCOfflineGift(int iIndex)
{
	if ( !gObjIsConnected(iIndex) )
		return;

	PMSG_REQ_REG_CC_OFFLINE_GIFT pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x15;
	pMsg.h.size = sizeof(pMsg);
	memcpy(pMsg.szUID, gGameObjects[iIndex]->AccountID, 11);
	memcpy(pMsg.szNAME, gGameObjects[iIndex]->Name, 11);
	pMsg.wServerCode = g_ConfigRead.server.GetGameServerCode() / 20;
	pMsg.iINDEX = iIndex;
	pMsg.szUID[10] = 0;
	pMsg.szNAME[10] = 0;

	//wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg));
	// TODO
}



void EGAnsRegCCOfflineGift( PMSG_ANS_REG_CC_OFFLINE_GIFT* lpMsg)
{
	if ( !lpMsg)
		return;

	if ( !lpMsg->iResultCode )
		return;

	char szAccountID[11] = {0};
	char szName[11] = {0};
	char szGIFT_NAME[50] = {0};
	memset(szAccountID, 0, sizeof(szAccountID));
	memset(szName, 0, sizeof(szName));
	memset(szGIFT_NAME, 0, sizeof(szGIFT_NAME));
	memcpy(szAccountID, lpMsg->szUID, sizeof(szAccountID));
	memcpy(szName, lpMsg->szNAME, sizeof(szName));
	memcpy(szGIFT_NAME, lpMsg->szGIFT_NAME, sizeof(szGIFT_NAME));
	szAccountID[10] = 0;
	szName[10] = 0;
	szGIFT_NAME[49] = 0;
	char szText[256] = {0};

	wsprintf(szText, Lang.GetText(0,156), szName, szGIFT_NAME);
	gGameProtocol.AllSendServerMsg(szText);

	sLog->outBasic("[Chaos Castle] [%s][%s] Success to Register OffLine Gift (GIFT:%s)",
		szAccountID, szName, szGIFT_NAME);
}



struct PMSG_REQ_REG_DL_OFFLINE_GIFT
{
	PBMSG_HEAD2 h;	// C1:16
	int iINDEX;	// 4
	char szUID[11];	// 8
	WORD wServerCode;	// 14
	char szNAME[11];	// 16
};

void EGReqRegDLOfflineGift(int iIndex)
{
	if ( !gObjIsConnected(iIndex) )
		return;

	PMSG_REQ_REG_DL_OFFLINE_GIFT pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x16;
	pMsg.h.size = sizeof(pMsg);
	memcpy(pMsg.szUID, gGameObjects[iIndex]->AccountID, 11);
	memcpy(pMsg.szNAME, gGameObjects[iIndex]->Name, 11);
	pMsg.wServerCode = g_ConfigRead.server.GetGameServerCode() / 20;
	pMsg.iINDEX = iIndex;
	pMsg.szUID[10] = 0;
	pMsg.szNAME[10] = 0;

	//wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg));
	// TOOD
}



void EGAnsRegDLOfflineGift( PMSG_ANS_REG_DL_OFFLINE_GIFT* lpMsg)
{
	if ( !lpMsg)
		return;

	if ( !lpMsg->iResultCode )
		return;

	char szAccountID[11] = {0};
	char szName[11] = {0};
	char szGIFT_NAME[50] = {0};
	memset(szAccountID, 0, sizeof(szAccountID));
	memset(szName, 0, sizeof(szName));
	memset(szGIFT_NAME, 0, sizeof(szGIFT_NAME));
	memcpy(szAccountID, lpMsg->szUID, sizeof(szAccountID));
	memcpy(szName, lpMsg->szNAME, sizeof(szName));
	memcpy(szGIFT_NAME, lpMsg->szGIFT_NAME, sizeof(szGIFT_NAME));
	szAccountID[10] = 0;
	szName[10] = 0;
	szGIFT_NAME[49] = 0;
	char szText[256] = {0};

	wsprintf(szText, "[´ÙÅ©·Îµå ±â³ä ÀÌº¥Æ®] %s ´Ô²²¼­ %s °æÇ°¿¡ ´çÃ·µÇ¼Ì½À´Ï´Ù.", szName, szGIFT_NAME);
	gGameProtocol.AllSendServerMsg(szText);

	sLog->outBasic("[DarkLord Heart Event] [%s][%s] Success to Register OffLine Gift (GIFT:%s)",
		szAccountID, szName, szGIFT_NAME);
}


struct PMSG_REQ_REG_HT_OFFLINE_GIFT
{
	PBMSG_HEAD2 h;	// C1:17
	int iINDEX;	// 4
	char szUID[11];	// 8
	WORD wServerCode;	// 14
	char szNAME[11];	// 16
};

void EGReqRegHTOfflineGift(int iIndex)
{
	if ( !gObjIsConnected(iIndex) )
		return;

	PMSG_REQ_REG_HT_OFFLINE_GIFT pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x17;
	pMsg.h.size = sizeof(pMsg);
	memcpy(pMsg.szUID, gGameObjects[iIndex]->AccountID, 11);
	memcpy(pMsg.szNAME, gGameObjects[iIndex]->Name, 11);
	pMsg.wServerCode = g_ConfigRead.server.GetGameServerCode() / 20;
	pMsg.iINDEX = iIndex;
	pMsg.szUID[10] = 0;
	pMsg.szNAME[10] = 0;

	//wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg));
	// TODO
}



void EGAnsRegHTOfflineGift( PMSG_ANS_REG_HT_OFFLINE_GIFT* lpMsg)
{
	if ( !lpMsg)
		return;

	if ( !lpMsg->iResultCode )
	{
		if ( gObjIsConnected(lpMsg->iINDEX))
		{
			CGameObject lpObj = &gGameObjects[lpMsg->iINDEX];

			MapC[lpObj.MapNumber].MoneyItemDrop(1000000, lpObj.X, lpObj.Y);
		}
		return;
	}

	char szAccountID[11] = {0};
	char szName[11] = {0};
	char szGIFT_NAME[50] = {0};
	memset(szAccountID, 0, sizeof(szAccountID));
	memset(szName, 0, sizeof(szName));
	memset(szGIFT_NAME, 0, sizeof(szGIFT_NAME));
	memcpy(szAccountID, lpMsg->szUID, sizeof(szAccountID));
	memcpy(szName, lpMsg->szNAME, sizeof(szName));
	memcpy(szGIFT_NAME, lpMsg->szGIFT_NAME, sizeof(szGIFT_NAME));
	szAccountID[10] = 0;
	szName[10] = 0;
	szGIFT_NAME[49] = 0;
	char szText[256] = {0};

	wsprintf(szText, "[¼û°ÜÁø º¸¹°»óÀÚ ÀÌº¥Æ®] %s ´Ô²²¼­ %s °æÇ°¿¡ ´çÃ·µÇ¼Ì½À´Ï´Ù.", szName, szGIFT_NAME);
	gGameProtocol.AllSendServerMsg(szText);

	sLog->outBasic("[Hidden TreasureBox Event] [%s][%s] Success to Register OffLine Gift (GIFT:%s)",
		szAccountID, szName, szGIFT_NAME);
}

void EGAnsRegLuckyCoin(PMSG_ANS_REG_LUCKYCOIN * lpMsg)
{
	if (!lpMsg)
		return;

	PMSG_ANS_LUCKYCOIN_REGISTER pMsg = {0};
	PHeadSubSetB((LPBYTE)&pMsg, 0xBF, 0x0C, sizeof(pMsg));

	CGameObject lpObj = &gGameObjects[lpMsg->iIndex];

	pMsg.btResult = lpMsg->Result;
	
	if(pMsg.btResult == 1)
	{
		if(lpObj.pInventory[lpMsg->Pos].IsItem() == TRUE && lpObj.pInventory[lpMsg->Pos].m_Type == ITEMGET(14,100))
		{
			if(lpObj.pInventory[lpMsg->Pos].m_Durability > 1.0)
			{
				lpObj.pInventory[lpMsg->Pos].m_Durability -= 1.0f;
				gGameProtocol.GCItemDurSend2(lpObj.m_Index, lpMsg->Pos, lpObj.pInventory[lpMsg->Pos].m_Durability, 0);
			}
			else
			{
				gObjInventoryDeleteItem(lpObj.m_Index, lpMsg->Pos);
				gGameProtocol.GCInventoryItemDeleteSend(lpObj.m_Index, lpMsg->Pos, 1);
			}
			pMsg.iLuckyCoin = lpMsg->LuckyCoins;
			lpObj.LuckyCoinCount = lpMsg->LuckyCoins;
		}
		else
		{
			pMsg.btResult = 0;
			pMsg.iLuckyCoin = lpMsg->LuckyCoins;
		}
	}
	IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (LPBYTE)&pMsg, pMsg.h.size);
	lpObj.UseEventServer = FALSE;
}

void EGAnsLuckyCoinInfo(PMSG_ANS_LUCKYCOIN *lpMsg)
{
	if(!lpMsg)
		return;

	PMSG_ANS_LUCKYCOININFO pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xBF, 0x0B, sizeof(pMsg));
	CGameObject lpObj = &gGameObjects[lpMsg->iIndex];

	pMsg.iLuckyCoin = lpMsg->LuckyCoins;
	lpObj.LuckyCoinCount = lpMsg->LuckyCoins;

	IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (LPBYTE)&pMsg, pMsg.h.size);
	lpObj.UseEventServer = FALSE;
}

void EGAnsSantaCheck(PMSG_ANS_SANTACHECK *lpMsg)
{
	if(!lpMsg)
		return;

	CGameObject lpObj = &gGameObjects[lpMsg->aIndex];

	switch ( lpMsg->Result )
	{
		case 0:
			if(!lpMsg->UseCount)
				gGameProtocol.GCServerCmd(lpObj.m_Index, 16, 0, 0);
			else
				gGameProtocol.GCServerCmd(lpObj.m_Index, 16, 1, 0);
			break;
		case 1:
		case 3:
			gGameProtocol.GCServerCmd(lpObj.m_Index, 16, 3, 0);
			break;
		case 2:
			gGameProtocol.GCServerCmd(lpObj.m_Index, 16, 2, 0);
			break;
	}
}

void EGReqSantaGift(CGameObject &lpObj)
{
	if(gObjIsConnected(aIndex) == false)
		return;

	PMSG_REQ_SANTAGIFT pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xBE, 0x21, sizeof(pMsg));

	memcpy(pMsg.AccountID, lpObj.AccountID, 11);
	pMsg.gGameServerCode = g_ConfigRead.server.GetGameServerCode();
	pMsg.aIndex = aIndex;

	//wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
	// TODO
}

void EGAnsSantaGift(PMSG_ANS_SANTAGIFT *lpMsg)
{
	if(!lpMsg)
		return;

	CGameObject lpObj = &gGameObjects[lpMsg->aIndex];

	switch ( lpMsg->Result )
	{
		case 0:
			if (lpObj.m_PlayerData->SantaCount < g_ConfigRead.data.common.XMasSantaFirstPrizeMaxCount)
			{
				g_BagManager.SearchAndUseBag(lpObj.m_Index, BAG_EVENT, EVENTBAG_SANTAFIRST, lpObj.m_Index);
			}

			else if (lpObj.m_PlayerData->SantaCount < g_ConfigRead.data.common.XMasSantaSecondPrizeMaxCount)
			{
				g_BagManager.SearchAndUseBag(lpObj.m_Index, BAG_EVENT, EVENTBAG_SANTASECOND, lpObj.m_Index);
			}

			else
			{
				g_BagManager.SearchAndUseBag(lpObj.m_Index, BAG_EVENT, EVENTBAG_SANTATHIRD, lpObj.m_Index);
			}

			lpObj.m_PlayerData->SantaCount++;
			sLog->outBasic("[X-MAS Event] [AccountID]: %s, [VISITCOUNT]:%d", lpObj.AccountID, lpObj.m_PlayerData->SantaCount);
			break;
		case 2:
			gGameProtocol.GCServerCmd(lpObj.m_Index, 16, 2, 0);
			break;
		case 1:
		case 3:
			gGameProtocol.GCServerCmd(lpObj.m_Index, 16, 3, 0);
			break;
	}
}
