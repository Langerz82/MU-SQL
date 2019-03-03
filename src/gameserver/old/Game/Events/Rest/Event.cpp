
#include "Event.h"
#include "GameProtocol.h"
#include "GameServer.h"
#include "Main.h"
#include "Utility/util.h"
#include "BagManager.h"
#include "configread.h"

void EventChipEventProtocolCore(BYTE protoNum, BYTE* aRecv, int aLen)
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

void FireworksOpenEven(CGameObject &Obj)
{
	PMSG_SERVERCMD ServerCmd;

	PHeadSubSetB((BYTE*)&ServerCmd, 0xF3, 0x40, sizeof(ServerCmd));
	ServerCmd.CmdType = 0;
	ServerCmd.X = Obj.X;
	ServerCmd.Y = Obj.Y;

	GSProtocol.MsgSendV2(lpObj, (BYTE*)&ServerCmd, sizeof(ServerCmd));
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&ServerCmd, sizeof(ServerCmd));
}

void ChristmasFireCrackDrop(CGameObject &Obj) //season 4.5 add-on
{
	PMSG_SERVERCMD ServerCmd;

	PHeadSubSetB((BYTE*)&ServerCmd, 0xF3, 0x40, sizeof(ServerCmd));
	ServerCmd.CmdType = 59;
	ServerCmd.X = Obj.X;
	ServerCmd.Y = Obj.Y;
	GSProtocol.MsgSendV2(lpObj, (BYTE*)&ServerCmd, sizeof(ServerCmd));
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&ServerCmd, sizeof(ServerCmd));
}

#pragma warning ( disable : 4101 )
void EGRecvEventChipInfo(PMSG_ANS_VIEW_EC_MN * aRecv)
{
	CGameObject lpObj = getGameObject(aRecv->iINDEX);

	PMSG_EVENTCHIPINFO eventchipeventinfo;
	char msg[255];

	PHeadSetB((BYTE*)&eventchipeventinfo, 0x94, sizeof(eventchipeventinfo));
	eventchipeventinfo.Type = 0;
	eventchipeventinfo.ChipCount = aRecv->nEVENT_CHIPS;
	Obj.EventChipCount = aRecv->nEVENT_CHIPS;
	Obj.MutoNumber = aRecv->iMUTO_NUM;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&eventchipeventinfo, eventchipeventinfo.h.size);

	Obj.UseEventServer = FALSE;
}
#pragma warning ( default : 4101 )


void EGResultRegEventChip(PMSG_ANS_REGISTER_EVENTCHIP * aRecv)
{
	PMSG_REGEVENTCHIP_RESULT Result;
	CGameObject lpObj;
	CGameObject &Obj;

	PHeadSetB((BYTE*)&Result, 0x95, sizeof(Result));
	lpObj = getGameObject(aRecv->iINDEX);
	Obj.m_Index = aRecv->iINDEX;

	if ( aRecv->bSUCCESS == FALSE )
	{
		Result.ChipCount = -1;
		sLog->outBasic("[EventChip] [%s][%s] RegEventServer Fail (RegEventchip) %d",
			Obj.AccountID, Obj.Name, aRecv->Pos);

	}
	else
	{
		Result.ChipCount = aRecv->nEVENT_CHIPS;
		gObjInventoryDeleteItem(Obj.m_Index, aRecv->Pos);
		GSProtocol.GCInventoryItemDeleteSend(Obj.m_Index, aRecv->Pos, 1);

		sLog->outBasic("[EventChip] [%s][%s] Delete EventChip (%d)",
			Obj.AccountID, Obj.Name, aRecv->Pos);
	}

	Result.Type = 0;

	IOCP.DataSend(Obj.m_Index, (BYTE*)&Result, Result.h.size);

	Obj.UseEventServer = FALSE;
}



void EGRecvRegMutoNum( PMSG_ANS_REGISTER_MUTONUM* aRecv)
{
	CGameObject lpObj;
	CGameObject &Obj;
	

	lpObj = getGameObject(aRecv->iINDEX);
	Obj.m_Index = aRecv->iINDEX;

	PMSG_GETMUTONUMBER_RESULT Result;

	PHeadSetB((BYTE*)&Result, 0x96, sizeof(Result));
	
	if ( gObjFind10EventChip(Obj.m_Index) == FALSE )
	{
		Result.MutoNum[0] = -1;
		Result.MutoNum[1] = 0;
		Result.MutoNum[2] = 0;

		IOCP.DataSend(Obj.m_Index, (BYTE*)&Result, Result.h.size);

		Obj.UseEventServer = FALSE;

		return;
	}

	gObjDelete10EventChip(Obj.m_Index);
	Result.MutoNum[0] = aRecv->iMUTO_NUM / 1000000;
	Result.MutoNum[1] = aRecv->iMUTO_NUM / 1000 - aRecv->iMUTO_NUM / 1000000 * 1000;
	Result.MutoNum[2] = aRecv->iMUTO_NUM % 1000;
	Obj.MutoNumber = aRecv->iMUTO_NUM;
	
	sLog->outBasic("[EventChip] [%s][%s] Make MutoNumber %d,%d,%d",
		Obj.AccountID, Obj.Name, 
		Result.MutoNum[0], Result.MutoNum[1], Result.MutoNum[2]);

	IOCP.DataSend(Obj.m_Index, (BYTE*)&Result, Result.h.size);
	Obj.UseEventServer = FALSE;
}


void EGRecvChangeRena( PMSG_ANS_RESET_EVENTCHIP* aRecv)
{
	PMSG_REGEVENTCHIP_RESULT Result;
	CGameObject lpObj;
	CGameObject &Obj;

	PHeadSetB((BYTE*)&Result, 0x95, sizeof(Result));
	lpObj = getGameObject(aRecv->iINDEX);
	Obj.m_Index = aRecv->iINDEX;

	if ( aRecv->bSUCCESS != FALSE )
	{
		Obj.m_PlayerData->Money += Obj.EventChipCount * 3000;
		GSProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

		sLog->outBasic("[EventChip] [%s][%s] ChangeRena AddMoney(%d)",
			Obj.AccountID, Obj.Name, Obj.EventChipCount * 3000);
	}
	else
	{
		sLog->outBasic("[EventChip] [%s][%s] ChangeRena Fail",
			Obj.AccountID, Obj.Name);
	}

	Result.ChipCount = 0;
	Obj.EventChipCount = 0;

	IOCP.DataSend(Obj.m_Index, (BYTE*)&Result, Result.h.size);

	Obj.UseEventServer = FALSE;
}


CGameObject pEventObj;

void EGRecvStoneInfo( PMSG_ANS_VIEW_STONES* aRecv)
{
	CGameObject lpObj = getGameObject(aRecv->iINDEX);

	PMSG_EVENTCHIPINFO Result;
	
	PHeadSetB((BYTE*)&Result, 0x94, sizeof(Result));

	if ( aRecv->bSUCCESS )
		Obj.iStoneCount = aRecv->iStoneCount;
	else
		Obj.iStoneCount = 0;

	Obj.MutoNumber = 0;
	Result.Type = 3;
	Result.ChipCount = aRecv->iStoneCount;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&Result, Result.h.size);

	char msg[128];
	wsprintf(msg, Lang.GetText(0,78), Result.ChipCount);
	GSProtocol.ChatTargetSend(pEventObj, msg, Obj.m_Index);
}


void EGRecvRegStone( PMSG_ANS_REGISTER_STONES* aRecv)
{
	PMSG_REGEVENTCHIP_RESULT Result;
	CGameObject lpObj;
	CGameObject &Obj;
	

	PHeadSetB((BYTE*)&Result, 0x95, sizeof(Result));
	lpObj = getGameObject(aRecv->iINDEX);
	Obj.m_Index = aRecv->iINDEX;

	if ( aRecv->bSUCCESS != FALSE )
	{
		Result.ChipCount = aRecv->iStoneCount;
		gObjInventoryDeleteItem(Obj.m_Index, aRecv->iPosition);
		GSProtocol.GCInventoryItemDeleteSend(Obj.m_Index, aRecv->iPosition, 1);

		sLog->outBasic("[Stone] [%s][%s] Delete Stones",
			Obj.AccountID, Obj.Name);
	}
	else
	{
		Result.ChipCount = -1;
		
		sLog->outBasic("[Stone] [%s][%s] RegEventServer Fail (Stones : %d)",
			Obj.AccountID, Obj.Name, aRecv->iStoneCount);
	}

	IOCP.DataSend(Obj.m_Index, (BYTE*)&Result, Result.h.size);

	Obj.UseEventServer = FALSE;
}


void EGRecvDeleteStone( PMSG_ANS_DELETE_STONES* aRecv)
{
	return;
}


void EGRecvChangeStones( PMSG_ANS_RESET_EVENTCHIP* aRecv)
{
	PMSG_REGEVENTCHIP_RESULT Result;
	CGameObject lpObj;
	CGameObject &Obj;
	

	PHeadSetB((BYTE*)&Result, 0x95, sizeof(Result));
	lpObj = getGameObject(aRecv->iINDEX);
	Obj.m_Index = aRecv->iINDEX;

	if ( aRecv->bSUCCESS != FALSE )
	{
		Obj.m_PlayerData->Money += Obj.iStoneCount * 3000;
		GSProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

		sLog->outBasic("[Stones] [%s][%s] ChangeRena AddMoney(%d)",
			Obj.AccountID, Obj.Name, Obj.iStoneCount*3000);
	}
	else
	{
		sLog->outBasic("[Stones] [%s][%s] ChangeRena Fail",
			Obj.AccountID, Obj.Name);
	}

	Result.ChipCount = 0;
	Obj.iStoneCount = 0;

	IOCP.DataSend(Obj.m_Index, (BYTE*)&Result, Result.h.size);

	Obj.UseEventServer = FALSE;
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

	PHeadSetB((BYTE*)&Result, 0x9D, sizeof(Result));

	if ( !ObjectMaxRange(aRecv->iINDEX))
	{
		sLog->outBasic("[Mu_2Anv_Event] Error : Index is out of bound [%d]", aRecv->iINDEX);
		return;
	}

	if ( getGameObject(aRecv->iINDEX)->Connected <= PLAYER_LOGGED )
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

		if ( getGameObject(aRecv->iINDEX)->Connected > PLAYER_LOGGED )
		{
			PMSG_SERVERCMD ServerCmd;

			PHeadSubSetB((BYTE*)&ServerCmd, 0xF3, 0x40, sizeof(ServerCmd));
			ServerCmd.CmdType = 0;
			ServerCmd.X = getGameObject(aRecv->iINDEX)->X;
			ServerCmd.Y = getGameObject(aRecv->iINDEX)->Y;

			GSProtocol.MsgSendV2(getGameObject(aRecv->iINDEX), (BYTE*)&ServerCmd, sizeof(ServerCmd));
			IOCP.DataSend(aRecv->iINDEX, (BYTE*)&ServerCmd, sizeof(ServerCmd));
		}

		if ( g_bRingEventItemTextLoad )
		{
			strcpy(Result.szGIFT_NAME, g_sz2ANV_GIFT_NAME[aRecv->iGiftNumber-1]);
		}

		switch ( aRecv->iGiftNumber )
		{
			case 1:
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				getGameObject(aRecv->iINDEX)->m_PlayerData->Money += 500000;
				GSProtocol.GCMoneySend(aRecv->iINDEX, getGameObject(aRecv->iINDEX)->m_PlayerData->Money);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, "500,000 Zen");
				}
				break;
			case 7:
				getGameObject(aRecv->iINDEX)->m_PlayerData->Money += 50000;
				GSProtocol.GCMoneySend(aRecv->iINDEX, getGameObject(aRecv->iINDEX)->m_PlayerData->Money);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, "50,000 Zen");
				}
				break;
			case 8:	case 9:	case 10:
				getGameObject(aRecv->iINDEX)->m_PlayerData->Money += 30000;
				GSProtocol.GCMoneySend(aRecv->iINDEX, getGameObject(aRecv->iINDEX)->m_PlayerData->Money);

				if ( g_bRingEventItemTextLoad == FALSE )
				{
					strcpy(Result.szGIFT_NAME, "30,000 Zen");
				}
				break;
			case 11:
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
				ItemCreate(getGameObject(aRecv->iINDEX)->m_Index, GIFT_2ANV_MAP, 
					getGameObject(aRecv->iINDEX]->X, getGameObject(aRecv->iINDEX)->Y,
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
		aRecv->btIsRegistered, getGameObject(aRecv->iINDEX]->AccountID, getGameObject(aRecv->iINDEX)->Name);

	IOCP.DataSend(aRecv->iINDEX, (BYTE*)&Result, Result.h.size);

	getGameObject(aRecv->iINDEX)->UseEventServer = FALSE;
}

static const char g_szRingEventOfflineGift[4][32] = { "100µ· ¹Â¹ÝÁö",
													  "10µ· ¹Â¹ÝÁö",
													  "5µ· ¹Â¹ÝÁö",
													  "2µ· ¹Â¹ÝÁö"};
															


void EGRecvRegRingGift( PMSG_ANS_REG_RINGGIFT* aRecv)
{
	getGameObject(aRecv->iINDEX)->UseEventServer = FALSE;

	if ( gObjIsConnected(aRecv->iINDEX) == FALSE )
		return;

	if ( strcmp(aRecv->szUID, getGameObject(aRecv->iINDEX)->AccountID))
		return;

	if ( aRecv->btIsRegistered == 1 )
	{
		if ( CHECK_LIMIT(aRecv->btGiftKind-1, 4) )
		{
			char szTemp[256];
			wsprintf(szTemp, "%s´Ô ²²¼­ %s¿¡ ´çÃ·µÇ¼Ì½À´Ï´Ù.",
				getGameObject(aRecv->iINDEX]->Name, g_szRingEventOfflineGift[aRecv->btGiftKind-1));

			GSProtocol.AllSendServerMsg(szTemp);

			sLog->outBasic("[Ring Event] [%s][%s] Register Succeeded Result:%d, Gift:%d",
				getGameObject(aRecv->iINDEX]->AccountID, getGameObject(aRecv->iINDEX)->Name,
				aRecv->btIsRegistered, aRecv->btGiftKind);
		}
		else
		{
			sLog->outBasic("[Ring Event] [%s][%s] Register Failed Result:%d, Gift:%d (out of bound, 1~4)",
				getGameObject(aRecv->iINDEX]->AccountID, getGameObject(aRecv->iINDEX)->Name,
				aRecv->btIsRegistered, aRecv->btGiftKind);
		}

		return;
	}

	sLog->outBasic("[Ring Event] [%s][%s] Register Failed Result : %d",
		getGameObject(aRecv->iINDEX]->AccountID, getGameObject(aRecv->iINDEX)->Name,
		aRecv->btIsRegistered);

	if ( gObjIsConnected(aRecv->iINDEX) == TRUE )
	{
		MapC[getGameObject(aRecv->iINDEX]->MapNumber]->MoneyItemDrop(100000, (BYTE)getGameObject(aRecv->iINDEX)->X, (BYTE)getGameObject(aRecv->iINDEX)->Y);
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


void EGReqBloodCastleEnterCount(CGameObject &Obj)
{
	if ( !gObjIsConnected(Obj.m_Index) )
		return;

	PMSG_REQ_BLOODCASTLE_ENTERCOUNT pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBD;
	pMsg.h.subcode = 0x02;
	pMsg.h.size = sizeof(pMsg);
	std::memcpy(pMsg.AccountID, Obj.AccountID, 10);
	std::memcpy(pMsg.GameID, Obj.Name, 10);
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
	std::memcpy(szAccountID, lpMsg->AccountID, 10);
	std::memcpy(szName, lpMsg->GameID, 10);

	if ( strcmp(getGameObject(lpMsg->iObjIndex)->AccountID, szAccountID) ||
		 strcmp(getGameObject(lpMsg->iObjIndex)->Name, szName) )
		 return;

	PMSG_ANS_CL_ENTERCOUNT pMsgSend;

	pMsgSend.h.c = 0xC1;
	pMsgSend.h.headcode = 0x9F;
	pMsgSend.h.size = sizeof(pMsgSend);
	pMsgSend.btEventType = 2;
	pMsgSend.btLeftEnterCount = lpMsg->iLeftCount;

	IOCP.DataSend(lpMsg->iObjIndex, (BYTE*)&pMsgSend, sizeof(pMsgSend));
}



struct PMSG_REQ_REG_CC_OFFLINE_GIFT
{
	PBMSG_HEAD2 h;	// C1:15
	int iINDEX;	// 4
	char szUID[11];	// 8
	WORD wServerCode;	// 14
	char szNAME[11];	// 16
};

void EGReqRegCCOfflineGift(CGameObject &Obj)
{
	if ( !gObjIsConnected(Obj.m_Index) )
		return;

	PMSG_REQ_REG_CC_OFFLINE_GIFT pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x15;
	pMsg.h.size = sizeof(pMsg);
	std::memcpy(pMsg.szUID, Obj.AccountID, 11);
	std::memcpy(pMsg.szNAME, Obj.Name, 11);
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
	std::memcpy(szAccountID, lpMsg->szUID, sizeof(szAccountID));
	std::memcpy(szName, lpMsg->szNAME, sizeof(szName));
	std::memcpy(szGIFT_NAME, lpMsg->szGIFT_NAME, sizeof(szGIFT_NAME));
	szAccountID[10] = 0;
	szName[10] = 0;
	szGIFT_NAME[49] = 0;
	char szText[256] = {0};

	wsprintf(szText, Lang.GetText(0,156), szName, szGIFT_NAME);
	GSProtocol.AllSendServerMsg(szText);

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

void EGReqRegDLOfflineGift(CGameObject &Obj)
{
	if ( !gObjIsConnected(Obj.m_Index) )
		return;

	PMSG_REQ_REG_DL_OFFLINE_GIFT pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x16;
	pMsg.h.size = sizeof(pMsg);
	std::memcpy(pMsg.szUID, Obj.AccountID, 11);
	std::memcpy(pMsg.szNAME, Obj.Name, 11);
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
	std::memcpy(szAccountID, lpMsg->szUID, sizeof(szAccountID));
	std::memcpy(szName, lpMsg->szNAME, sizeof(szName));
	std::memcpy(szGIFT_NAME, lpMsg->szGIFT_NAME, sizeof(szGIFT_NAME));
	szAccountID[10] = 0;
	szName[10] = 0;
	szGIFT_NAME[49] = 0;
	char szText[256] = {0};

	wsprintf(szText, "[´ÙÅ©·Îµå ±â³ä ÀÌº¥Æ®] %s ´Ô²²¼­ %s °æÇ°¿¡ ´çÃ·µÇ¼Ì½À´Ï´Ù.", szName, szGIFT_NAME);
	GSProtocol.AllSendServerMsg(szText);

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

void EGReqRegHTOfflineGift(CGameObject &Obj)
{
	if ( !gObjIsConnected(Obj.m_Index) )
		return;

	PMSG_REQ_REG_HT_OFFLINE_GIFT pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x17;
	pMsg.h.size = sizeof(pMsg);
	std::memcpy(pMsg.szUID, Obj.AccountID, 11);
	std::memcpy(pMsg.szNAME, Obj.Name, 11);
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
			CGameObject lpObj = getGameObject(lpMsg->iINDEX);

			MapC[Obj.MapNumber].MoneyItemDrop(1000000, Obj.X, Obj.Y);
		}
		return;
	}

	char szAccountID[11] = {0};
	char szName[11] = {0};
	char szGIFT_NAME[50] = {0};
	memset(szAccountID, 0, sizeof(szAccountID));
	memset(szName, 0, sizeof(szName));
	memset(szGIFT_NAME, 0, sizeof(szGIFT_NAME));
	std::memcpy(szAccountID, lpMsg->szUID, sizeof(szAccountID));
	std::memcpy(szName, lpMsg->szNAME, sizeof(szName));
	std::memcpy(szGIFT_NAME, lpMsg->szGIFT_NAME, sizeof(szGIFT_NAME));
	szAccountID[10] = 0;
	szName[10] = 0;
	szGIFT_NAME[49] = 0;
	char szText[256] = {0};

	wsprintf(szText, "[¼û°ÜÁø º¸¹°»óÀÚ ÀÌº¥Æ®] %s ´Ô²²¼­ %s °æÇ°¿¡ ´çÃ·µÇ¼Ì½À´Ï´Ù.", szName, szGIFT_NAME);
	GSProtocol.AllSendServerMsg(szText);

	sLog->outBasic("[Hidden TreasureBox Event] [%s][%s] Success to Register OffLine Gift (GIFT:%s)",
		szAccountID, szName, szGIFT_NAME);
}

void EGAnsRegLuckyCoin(PMSG_ANS_REG_LUCKYCOIN * lpMsg)
{
	if (!lpMsg)
		return;

	PMSG_ANS_LUCKYCOIN_REGISTER pMsg = {0};
	PHeadSubSetB((BYTE*)&pMsg, 0xBF, 0x0C, sizeof(pMsg));

	CGameObject lpObj = getGameObject(lpMsg->iIndex);

	pMsg.btResult = lpMsg->Result;
	
	if(pMsg.btResult == 1)
	{
		if(Obj.pntInventory[lpMsg->Pos]->IsItem() == TRUE && Obj.pntInventory[lpMsg->Pos]->m_Type == ITEMGET(14,100))
		{
			if(Obj.pntInventory[lpMsg->Pos]->m_Durability > 1.0)
			{
				Obj.pntInventory[lpMsg->Pos]->m_Durability -= 1.0f;
				GSProtocol.GCItemObjectDurSend2(Obj.m_Index, lpMsg->Pos, Obj.pntInventory[lpMsg->Pos]->m_Durability, 0);
			}
			else
			{
				gObjInventoryDeleteItem(Obj.m_Index, lpMsg->Pos);
				GSProtocol.GCInventoryItemDeleteSend(Obj.m_Index, lpMsg->Pos, 1);
			}
			pMsg.iLuckyCoin = lpMsg->LuckyCoins;
			Obj.LuckyCoinCount = lpMsg->LuckyCoins;
		}
		else
		{
			pMsg.btResult = 0;
			pMsg.iLuckyCoin = lpMsg->LuckyCoins;
		}
	}
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	Obj.UseEventServer = FALSE;
}

void EGAnsLuckyCoinInfo(PMSG_ANS_LUCKYCOIN *lpMsg)
{
	if(!lpMsg)
		return;

	PMSG_ANS_LUCKYCOININFO pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xBF, 0x0B, sizeof(pMsg));
	CGameObject lpObj = getGameObject(lpMsg->iIndex);

	pMsg.iLuckyCoin = lpMsg->LuckyCoins;
	Obj.LuckyCoinCount = lpMsg->LuckyCoins;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	Obj.UseEventServer = FALSE;
}

void EGAnsSantaCheck(PMSG_ANS_SANTACHECK *lpMsg)
{
	if(!lpMsg)
		return;

	CGameObject lpObj = getGameObject(lpMsg->Obj.m_Index);

	switch ( lpMsg->Result )
	{
		case 0:
			if(!lpMsg->UseCount)
				GSProtocol.GCServerCmd(Obj.m_Index, 16, 0, 0);
			else
				GSProtocol.GCServerCmd(Obj.m_Index, 16, 1, 0);
			break;
		case 1:
		case 3:
			GSProtocol.GCServerCmd(Obj.m_Index, 16, 3, 0);
			break;
		case 2:
			GSProtocol.GCServerCmd(Obj.m_Index, 16, 2, 0);
			break;
	}
}

void EGReqSantaGift(CGameObject &Obj)
{
	if(gObjIsConnected(Obj.m_Index) == false)
		return;

	PMSG_REQ_SANTAGIFT pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xBE, 0x21, sizeof(pMsg));

	std::memcpy(pMsg.AccountID, Obj.AccountID, 11);
	pMsg.gGameServerCode = g_ConfigRead.server.GetGameServerCode();
	pMsg.Obj.m_Index = Obj.m_Index;

	//wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
	// TODO
}

void EGAnsSantaGift(PMSG_ANS_SANTAGIFT *lpMsg)
{
	if(!lpMsg)
		return;

	CGameObject lpObj = getGameObject(lpMsg->Obj.m_Index);

	switch ( lpMsg->Result )
	{
		case 0:
			if (Obj.m_PlayerData->SantaCount < g_ConfigRead.data.common.XMasSantaFirstPrizeMaxCount)
			{
				g_BagManager.SearchAndUseBag(Obj.m_Index, BAG_EVENT, EVENTBAG_SANTAFIRST, Obj.m_Index);
			}

			else if (Obj.m_PlayerData->SantaCount < g_ConfigRead.data.common.XMasSantaSecondPrizeMaxCount)
			{
				g_BagManager.SearchAndUseBag(Obj.m_Index, BAG_EVENT, EVENTBAG_SANTASECOND, Obj.m_Index);
			}

			else
			{
				g_BagManager.SearchAndUseBag(Obj.m_Index, BAG_EVENT, EVENTBAG_SANTATHIRD, Obj.m_Index);
			}

			Obj.m_PlayerData->SantaCount++;
			sLog->outBasic("[X-MAS Event] [AccountID]: %s, [VISITCOUNT]:%d", Obj.AccountID, Obj.m_PlayerData->SantaCount);
			break;
		case 2:
			GSProtocol.GCServerCmd(Obj.m_Index, 16, 2, 0);
			break;
		case 1:
		case 3:
			GSProtocol.GCServerCmd(Obj.m_Index, 16, 3, 0);
			break;
	}
}
