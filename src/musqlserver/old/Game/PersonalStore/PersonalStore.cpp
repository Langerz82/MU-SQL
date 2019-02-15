////////////////////////////////////////////////////////////////////////////////
// PersonalStore.cpp
#include "StdAfx.h"
#include "PersonalStore.h"
#include "User/CUserData.h"
#include "GameMain.h"
#include "util.h"
#include "Logging/Log.h"
#include "configread.h"
#include "NewPVP.h"
#include "ChaosCastle.h"
#include "GensSystem.h"
#include "PentagramSystem.h"

#include "CashShop.h"
#include "MineSystem.h"
#include "BotSystem.h"

CPersonalStore g_PersonalStore;

CPersonalStore::CPersonalStore()
{

}

CPersonalStore::~CPersonalStore()
{

}

void CPersonalStore::CGPShopReqSetItemPrice(PMSG_REQ_PSHOP_SETITEMPRICE * lpMsg, CGameObject &Obj)
{

	if (gDoPShopOpen == FALSE)
	{
		CGPShopAnsSetItemPrice(Obj.m_Index, 0, lpMsg->btItemPos);
		return;
	}

	if (!gObjIsConnected(Obj.m_Index))
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.Level <= 5)
	{
		CGPShopAnsSetItemPrice(Obj.m_Index, 5, lpMsg->btItemPos);

		return;
	}

	if (INVENTORY_RANGE(lpMsg->btItemPos) == FALSE)
	{
		CGPShopAnsSetItemPrice(Obj.m_Index, 2, lpMsg->btItemPos);
		return;
	}

	if (Obj.pntInventory1[lpMsg->btItemPos].IsItem() == FALSE)
	{
		CGPShopAnsSetItemPrice(Obj.m_Index, 3, lpMsg->btItemPos);
		return;
	}

	CItemObject * sitem = &Obj.pntInventory1[lpMsg->btItemPos];
	int iItemPrice = MAKE_NUMBERDW(MAKE_NUMBERW(lpMsg->sItemPrice4, lpMsg->sItemPrice3), MAKE_NUMBERW(lpMsg->sItemPrice2, lpMsg->sItemPrice1));

	WORD wBlessPrice = MAKE_NUMBERW(lpMsg->sJewelOfBlessPrice2, lpMsg->sJewelOfBlessPrice1);
	WORD wSoulPrice = MAKE_NUMBERW(lpMsg->sJewelOfSoulPrice2, lpMsg->sJewelOfSoulPrice1);
	WORD wChaosPrice = MAKE_NUMBERW(lpMsg->sJewelOfChaosPrice2, lpMsg->sJewelOfChaosPrice1);

	if (gObjCheckSerial0ItemList(sitem) != FALSE)
	{
		MsgOutput(Obj.m_Index, Lang.GetText(0, 259));
		gGameProtocol.GCReFillSend(Obj.m_Index, Obj.Life, 0xFD, 1, Obj.iShield);

		return;
	}

	if (gObjInventorySearchSerialNumber(getGameObject(Obj.m_Index), sitem->GetNumber()) == FALSE)
	{
		gGameProtocol.GCReFillSend(Obj.m_Index, Obj.Life, 0xFD, 1, Obj.iShield);
		return;
	}

	if (iItemPrice <= 0 && wBlessPrice <= 0 && wSoulPrice <= 0 && wChaosPrice <= 0)
	{
		CGPShopAnsSetItemPrice(Obj.m_Index, 4, lpMsg->btItemPos);

		return;
	}

	if ((Obj.Penalty & 4) == 4 || (Obj.Penalty & 8) == 8)
	{
		CGPShopAnsSetItemPrice(Obj.m_Index, 6, lpMsg->btItemPos);

		return;
	}

	sitem->m_iPShopValue = iItemPrice;
	sitem->m_wPShopBlessValue = wBlessPrice;
	sitem->m_wPShopSoulValue = wSoulPrice;
	sitem->m_wPShopChaosValue = wChaosPrice;

	ItemAttribute[sitem->m_Type].Name[31] = 0;	// Zero String Terminated

	CGPShopAnsSetItemPrice(Obj.m_Index, 1, lpMsg->btItemPos);
}



void CPersonalStore::CGPShopAnsSetItemPrice(CGameObject &Obj, BYTE btResult, BYTE btItemPos)
{
	PMSG_ANS_PSHOP_SETITEMPRICE pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0x3F, 0x01, sizeof(pMsg));
	pMsg.btResult = btResult;
	pMsg.btItemPos = btItemPos;

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}


void CPersonalStore::CGPShopReqOpen(PMSG_REQ_PSHOP_OPEN * lpMsg, CGameObject &Obj)
{
	if (::gDoPShopOpen == FALSE)
		return;

	if (!gObjIsConnected(Obj.m_Index))
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.m_PlayerData->m_bSecurityCheck == false)
	{
		MsgOutput(Obj.m_Index, Lang.GetText(0, 394));
		this->CGPShopAnsOpen(Obj.m_Index, 3);
		return;
	}

	if (g_NewPVP.IsDuel(getGameObject(Obj.m_Index)))
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 313), Obj.m_Index, 1);
		return;
	}

	if (g_NewPVP.IsObserver(getGameObject(Obj.m_Index)))
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 314), Obj.m_Index, 1);
		return;
	}

	if (Obj.Level <= 5)
	{
		this->CGPShopAnsOpen(Obj.m_Index, 2);

		return;
	}

	if ((Obj.Penalty & 4) == 4 || (Obj.Penalty & 8) == 8)
	{
		this->CGPShopAnsOpen(Obj.m_Index, 3);

		return;
	}

	if (CC_MAP_RANGE(Obj.MapNumber))
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 115), Obj.m_Index, 1);
		this->CGPShopAnsOpen(Obj.m_Index, 0);
		return;
	}

	if (Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 563), Obj.m_Index, 1);
		this->CGPShopAnsOpen(Obj.m_Index, 0);
		return;
	}

	if (Obj.m_PlayerData->m_JoinUnityBattle == TRUE || g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 621), Obj.m_Index, 1);
		this->CGPShopAnsOpen(Obj.m_Index, 0);
		return;
	}

	if (Obj.m_PlayerData->m_bIsMining)
	{
		this->CGPShopAnsOpen(Obj.m_Index, 4);
		return;
	}

	if (g_MineSystem.IsEquipPickax(Obj.m_Index))
	{
		this->CGPShopAnsOpen(Obj.m_Index, 4);
		return;
	}

	if (g_GensSystem.IsMapBattleZone(Obj.MapNumber))
	{
		this->CGPShopAnsOpen(Obj.m_Index, 0);
		return;
	}

	bool bEnablePShopOpen = false;

	if (Obj.m_IfState->use == FALSE)
		bEnablePShopOpen = true;
	else
	{
		if (Obj.m_IfState->use == TRUE)
		{
			switch (Obj.m_IfState->type)
			{
			case 8:
				bEnablePShopOpen = true; break;
			}

			if (!bEnablePShopOpen)
			{
				sLog->outBasic("[PShop] [%s][%s] ERROR : m_IfState->type is Using : %d", Obj.AccountID, Obj.Name, Obj.m_IfState->type);
			}
		}
		else
		{
			return;
		}
	}

	if (!::gObjFixInventoryPointer(Obj.m_Index))
	{
		sLog->outBasic("[Fix Inv.Ptr] False Location - %s, %d", __FILE__, __LINE__);
	}

	if (Obj.pTransaction == 1)
	{
		bEnablePShopOpen = false;
	}

	if (bEnablePShopOpen)
	{
		if (Obj.m_bPShopOpen == false)
		{
			Obj.m_bPShopOpen = true;
			std::memcpy(Obj.m_szPShopText, lpMsg->szPShopText, sizeof(lpMsg->szPShopText));
			this->GDAllSavePShopItemValue(getGameObject(Obj.m_Index));
		}
		else
		{
			std::memcpy(Obj.m_szPShopText, lpMsg->szPShopText, sizeof(lpMsg->szPShopText));

			PMSG_ANS_PSHOP_TEXT_CHANGED pMsg;

			PHeadSubSetB((BYTE*)&pMsg, 0x3F, 0x10, sizeof(pMsg));
			pMsg.NumberH = SET_NUMBERH(Obj.m_Index);
			pMsg.NumberL = SET_NUMBERL(Obj.m_Index);
			std::memcpy(pMsg.btPShopText, lpMsg->szPShopText, sizeof(pMsg.btPShopText));
			std::memcpy(pMsg.btName, Obj.Name, sizeof(pMsg.btName));

			gGameProtocol.MsgSendV2(getGameObject(Obj.m_Index), (BYTE*)&pMsg, pMsg.h.size);

		}
	}
	else
	{
		CGPShopAnsOpen(Obj.m_Index, 0);
		return;
	}

	CGPShopAnsOpen(Obj.m_Index, 1);
}

void CPersonalStore::CGPShopAnsOpen(CGameObject &Obj, BYTE btResult)
{
	PMSG_ANS_PSHOP_OPEN pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0x3F, 0x02, sizeof(pMsg));
	pMsg.btResult = btResult;

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, sizeof(pMsg));
}



void CPersonalStore::CGPShopReqClose(CGameObject &Obj)
{
	if (!gObjIsConnected(Obj.m_Index))
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (g_NewPVP.IsDuel(getGameObject(Obj.m_Index)))
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 313), Obj.m_Index, 1);
		return;
	}

	if (g_NewPVP.IsObserver(getGameObject(Obj.m_Index)))
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 314), Obj.m_Index, 1);
		return;
	}

	Obj.m_bPShopOpen = false;
	memset(Obj.m_szPShopText, 0, sizeof(Obj.m_szPShopText));
	CGPShopAnsClose(Obj.m_Index, 1);
}

void CPersonalStore::CGPShopAnsClose(CGameObject &Obj, BYTE btResult)
{

	PMSG_ANS_PSHOP_CLOSE pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0x3F, 0x03, sizeof(pMsg));
	pMsg.btResult = btResult;
	pMsg.NumberH = SET_NUMBERH(Obj.m_Index);
	pMsg.NumberL = SET_NUMBERL(Obj.m_Index);

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, sizeof(pMsg));

	if (btResult == 1)
		gGameProtocol.MsgSendV2(getGameObject(Obj.m_Index), (BYTE*)&pMsg, pMsg.h.size);
}



void CPersonalStore::CGPShopReqBuyList(PMSG_REQ_BUYLIST_FROM_PSHOP * lpMsg, int aSourceIndex)
{
	if (::gDoPShopOpen == FALSE)
		return;

	if (!ObjectMaxRange(MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL)))
		return;

	if (gObjIsConnected(MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL)) == FALSE)
	{
		this->CGPShopAnsBuyList(aSourceIndex, -1, 2, 0);
		return;
	}

	CGameObject lpObj = getGameObject(MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL));


	if (Obj.m_bOffLevel == true)
	{
		this->CGPShopAnsBuyList(aSourceIndex, -1, 2, 0);
		return;
	}
	if (Obj.Type != OBJ_USER)
	{
		this->CGPShopAnsBuyList(aSourceIndex, -1, 2, 0);
		return;
	}

	if (getGameObject(aSourceIndex)->Type != OBJ_USER)
	{
		this->CGPShopAnsBuyList(aSourceIndex, -1, 2, 0);
		return;
	}

	if (Obj.m_PlayerData->ISBOT == true && g_BotSystem.GetBotType(Obj.m_Index) == BOT_SHOP)
	{
		gGameProtocol.GCAlterPShopVault(aSourceIndex, g_BotSystem.m_BotData[Obj.m_PlayerData->wBotIndex].m_Shop.iCoinType);
	}
	else if (Obj.m_bOff)
	{
		gGameProtocol.GCAlterPShopVault(aSourceIndex, g_ConfigRead.offtrade.CoinType);
	}
	else
	{
		gGameProtocol.GCAlterPShopVault(aSourceIndex, 0);
	}
	if (Obj.CloseCount >= 0)
	{
		this->CGPShopAnsBuyItem(aSourceIndex, -1, 0, 2);
		return;
	}

	if (getGameObject(aSourceIndex)->CloseCount >= 0)
	{
		this->CGPShopAnsBuyItem(aSourceIndex, -1, 0, 2);
		return;
	}

	if (g_NewPVP.IsDuel(getGameObject(aSourceIndex)))
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 313), aSourceIndex, 1);
		return;
	}

	if (g_NewPVP.IsObserver(getGameObject(aSourceIndex)))
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 314), aSourceIndex, 1);
		return;
	}

	if (getGameObject(aSourceIndex)->m_bMapSvrMoveQuit == true || getGameObject(aSourceIndex)->m_bMapSvrMoveReq == true || getGameObject(aSourceIndex)->m_bMapSvrMoveReq_2 == true ||
		Obj.m_bMapSvrMoveQuit == true || Obj.m_bMapSvrMoveReq == true || Obj.m_bMapSvrMoveReq_2 == true)
	{
		sLog->outBasic("[PShop][SuspectCheating] Map Of Move request was trying to Shop List. Buyer ID[%s]Name[%s] Seller Name[%s]",
			getGameObject(aSourceIndex)->AccountID, getGameObject(aSourceIndex)->Name, Obj.Name);
		return;
	}

	if (getGameObject(aSourceIndex)->m_PlayerData->m_JoinUnityBattle == TRUE ||
		Obj.m_PlayerData->m_JoinUnityBattle == TRUE ||
		g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		gGameProtocol.GCServerMsgStringSend("Not Available in Battle Core", aSourceIndex, 1);
		return;
	}

	if (Obj.m_Index == aSourceIndex)
	{
		this->CGPShopAnsBuyList(aSourceIndex, -1, 2, 0);
		return;
	}

	if (Obj.m_bPShopOpen == false)
	{
		this->CGPShopAnsBuyList(aSourceIndex, -1, 3, 0);
		return;
	}

	if ((Obj.Penalty & 4) == 4 || (Obj.Penalty & 8) == 8)
	{
		this->CGPShopAnsBuyList(aSourceIndex, -1, 4, 0);
		return;
	}

	if (!::gObjFixInventoryPointer(aSourceIndex))
	{
		sLog->outBasic("[Fix Inv.Ptr] False Location - %s, %d", __FILE__, __LINE__);
	}

	if (getGameObject(aSourceIndex)->pTransaction == 1)
	{
		return;
	}

	getGameObject(aSourceIndex)->m_bPShopWantDeal = true;
	getGameObject(aSourceIndex)->m_iPShopDealerIndex = Obj.m_Index;
	std::memcpy(getGameObject(aSourceIndex)->m_szPShopDealerName, Obj.Name, MAX_ACCOUNT_LEN);
	this->CGPShopAnsBuyList(aSourceIndex, Obj.m_Index, 1, false);


}

void CPersonalStore::CGPShopAnsBuyList(int aSourceIndex, int aTargetIndex, BYTE btResult, bool bResend)
{
	if (gDoPShopOpen == FALSE)
		return;

	PMSG_ANS_BUYLIST_FROM_PSHOP pMsgILC;	// Packet Message Item List Count
	PMSG_BUYLIST_FROM_PSHOP pMsgIL;
	BYTE sendBuf[1024];
	int sOfs = sizeof(pMsgILC);
	int pMsgILSize = sizeof(pMsgIL);

	if (btResult != 1)
	{
		CGameObject lpObj = getGameObject(aSourceIndex);

		pMsgILC.h.c = 0xC2;
		pMsgILC.h.headcode = 0x3F;

		if (bResend == false)
			pMsgILC.h.subcode = 0x05;
		else
			pMsgILC.h.subcode = 0x13;

		pMsgILC.h.sizeH = SET_NUMBERH(sOfs);
		pMsgILC.h.sizeL = SET_NUMBERL(sOfs);
		pMsgILC.btCount = 0;
		pMsgILC.Result = btResult;
		pMsgILC.NumberH = SET_NUMBERH(aTargetIndex);
		pMsgILC.NumberL = SET_NUMBERL(aTargetIndex);
		memset(pMsgILC.btName, 0, sizeof(pMsgILC.btName));
		memset(pMsgILC.szPShopText, 0, sizeof(pMsgILC.szPShopText));
		std::memcpy(sendBuf, &pMsgILC, sizeof(pMsgILC));

		IOCP.DataSend(aSourceIndex, sendBuf, sOfs);
	}
	else
	{
		CGameObject lpObjSource = getGameObject(aSourceIndex);
		CGameObject lpObjTarget = getGameObject(aTargetIndex);
		int iCount = 0;

		for (int n = PSHOP_START_RANGE; n < PSHOP_END_RANGE; n++)
		{
			if (lpObjTarget->pntInventory1[n]->IsItem() == TRUE)
			{
				pMsgIL.Pos = n;
				ItemByteConvert(pMsgIL.ItemInfo, lpObjTarget->pntInventory1[n]);
				pMsgIL.PShopItemValue = lpObjTarget->pntInventory1[n]->m_iPShopValue;
				pMsgIL.BlessValue = lpObjTarget->pntInventory1[n]->m_wPShopBlessValue;
				pMsgIL.ChaosValue = lpObjTarget->pntInventory1[n]->m_wPShopChaosValue;
				pMsgIL.SoulValue = lpObjTarget->pntInventory1[n]->m_wPShopSoulValue;
				std::memcpy(&sendBuf[sOfs], &pMsgIL, pMsgILSize);
				iCount++;
				sOfs += pMsgILSize;
			}
		}

		pMsgILC.h.c = 0xC2;
		pMsgILC.h.headcode = 0x3F;

		if (bResend == false)
			pMsgILC.h.subcode = 0x05;
		else
			pMsgILC.h.subcode = 0x13;

		pMsgILC.h.sizeH = SET_NUMBERH(sOfs);
		pMsgILC.h.sizeL = SET_NUMBERL(sOfs);
		pMsgILC.btCount = iCount;
		pMsgILC.Result = btResult;
		pMsgILC.NumberH = SET_NUMBERH(aTargetIndex);
		pMsgILC.NumberL = SET_NUMBERL(aTargetIndex);
		std::memcpy(pMsgILC.btName, lpObjTarget->Name, sizeof(pMsgILC.btName));
		std::memcpy(pMsgILC.szPShopText, lpObjTarget->m_szPShopText, sizeof(pMsgILC.szPShopText));
		std::memcpy(sendBuf, &pMsgILC, sizeof(pMsgILC));

		IOCP.DataSend(aSourceIndex, sendBuf, sOfs);

		g_PentagramSystem.GCPShopPentagramJewelViewInfo(aTargetIndex, aSourceIndex);

	}
}

void CPersonalStore::CGPShopReqBuyItem(BYTE* lpRecv, int aSourceIndex)
{
	if (gDoPShopOpen == FALSE)
		return;

	PMSG_REQ_BUYITEM_FROM_PSHOP * lpMsg = (PMSG_REQ_BUYITEM_FROM_PSHOP *)lpRecv;

	if (gObjIsConnected(MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL)) == FALSE)
	{
		g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, -1, 0, 2);
		return;
	}

	CGameObject lpObj = getGameObject(MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL));

	if (Obj.Type != OBJ_USER)
	{
		g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, -1, 0, 2);
		return;
	}

	if (Obj.CloseCount >= 0)
	{
		g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, -1, 0, 2);
		return;
	}

	if (getGameObject(aSourceIndex)->CloseCount >= 0)
	{
		g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, -1, 0, 2);
		return;
	}

	if (g_NewPVP.IsDuel(getGameObject(aSourceIndex)))
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 313), aSourceIndex, 1);
		return;
	}

	if (g_NewPVP.IsObserver(getGameObject(aSourceIndex)))
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 314), aSourceIndex, 1);
		return;
	}

	if (Obj.m_bPShopOpen == false)
	{
		g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, Obj.m_Index, 0, 3);
		return;
	}

	char szName[MAX_ACCOUNT_LEN + 1] = { 0 };
	std::memcpy(szName, lpMsg->btName, sizeof(lpMsg->btName));
	szName[MAX_ACCOUNT_LEN] = 0;
	int iITEM_LOG_TYPE;
	int iITEM_LOG_LEVEL;
	int iITEM_LOG_DUR;
	int iITEM_LOG_SERIAL;
	int iITEM_LOG_BJPRICE;
	int iITEM_LOG_SJPRICE;
	int iITEM_LOG_CJPRICE;

	if (strcmp(szName, Obj.Name))
	{
		g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, Obj.m_Index, 0, 6);

		return;
	}

	if (Obj.CloseType != -1)
	{
		g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, Obj.m_Index, 0, 2);

		return;
	}

	if ((Obj.Penalty & 4) == 4 || (Obj.Penalty & 8) == 8)
	{
		g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, Obj.m_Index, 0, 9);

		return;
	}

	if ((getGameObject(aSourceIndex)->Penalty & 4) == 4 || (getGameObject(aSourceIndex)->Penalty & 8) == 8)
	{
		g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, Obj.m_Index, 0, 9);

		return;
	}

	EnterCriticalSection(&Obj.m_critPShopTrade);

	if (Obj.m_bPShopTransaction == true)
	{
		g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, Obj.m_Index, 0, 4);
		LeaveCriticalSection(&Obj.m_critPShopTrade);

	}
	else
	{
		Obj.m_bPShopTransaction = true;
		LeaveCriticalSection(&Obj.m_critPShopTrade);

		if (gObjCheckSerial0ItemList(&Obj.pntInventory1[lpMsg->btItemPos]) != FALSE)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 259));
			gGameProtocol.GCReFillSend(aSourceIndex, getGameObject(aSourceIndex)->Life, 0xFD, 1, getGameObject(aSourceIndex)->iShield);

			return;
		}

		if (::gObjInventorySearchSerialNumber(getGameObject(aSourceIndex), Obj.pntInventory1[lpMsg->btItemPos)->GetNumber()) == FALSE)
		{
			gGameProtocol.GCReFillSend(aSourceIndex, getGameObject(aSourceIndex)->Life, 0xFD, 1, getGameObject(aSourceIndex)->iShield);
			return;
		}

		if (::gObjInventorySearchSerialNumber(lpObj, Obj.pntInventory1[lpMsg->btItemPos].GetNumber()) == FALSE)
		{
			gGameProtocol.GCReFillSend(Obj.m_Index, Obj.Life, 0xFD, 1, Obj.iShield);
			return;
		}

		if (g_ConfigRead.data.common.BlockTradeFFFFFFFFItems == true)
		{
			if (Obj.pntInventory1[lpMsg->btItemPos].m_Number == (UINT64)-1)
			{
				gGameProtocol.GCReFillSend(Obj.m_Index, Obj.Life, 0xFD, 1, Obj.iShield);
				return;
			}
		}

		if (gObjFixInventoryPointer(aSourceIndex) == false)
		{
			sLog->outBasic("[Fix Inv.Ptr] False Location - %s, %d",
				__FILE__, __LINE__);
		}

		if (getGameObject(aSourceIndex)->pTransaction == 1)
		{
			return;
		}

		iITEM_LOG_TYPE = 0;
		iITEM_LOG_LEVEL = 0;
		iITEM_LOG_DUR = 0;
		iITEM_LOG_SERIAL = 0;

		__try
		{
			if (PSHOP_RANGE(lpMsg->btItemPos) == FALSE)
			{
				g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, Obj.m_Index, 0, 5);
				return;
			}

			if (Obj.m_bMapSvrMoveQuit == true)
			{
				g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, -1, 0, 2);
				return;
			}

			if (Obj.pntInventory1[lpMsg->btItemPos].IsItem() == TRUE)
			{
				if (Obj.pntInventory1[lpMsg->btItemPos].m_iPShopValue <= 0 && Obj.pntInventory1[lpMsg->btItemPos].m_wPShopBlessValue <= 0
					&& Obj.pntInventory1[lpMsg->btItemPos].m_wPShopSoulValue <= 0 && Obj.pntInventory1[lpMsg->btItemPos].m_wPShopChaosValue <= 0)
				{
					g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, Obj.m_Index, 0, 6);

					return;
				}

				if (Obj.m_bOff == false && Obj.m_PlayerData->ISBOT == false)
				{
					if (getGameObject(aSourceIndex)->m_PlayerData->Money < Obj.pntInventory1[lpMsg->btItemPos)->m_iPShopValue)
					{
						g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, Obj.m_Index, 0, 7);

						return;
					}
				}
				else
				{
					if (!Obj.m_bOff && Obj.m_PlayerData->ISBOT == false)
					{
						if (getGameObject(aSourceIndex)->m_PlayerData->Money < Obj.pntInventory1[lpMsg->btItemPos)->m_iPShopValue)
						{
							g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, Obj.m_Index, 0, 7);

							return;
						}
					}
					else if (Obj.m_bOff && Obj.m_PlayerData->ISBOT == false)
					{
						switch (g_ConfigRead.offtrade.CoinType)
						{
						case 0:
						{
							if (getGameObject(aSourceIndex)->m_PlayerData->Money < Obj.pntInventory1[lpMsg->btItemPos)->m_iPShopValue)
							{
								g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, Obj.m_Index, 0, 7);
								return;
							}
						}
						break;
						case 1:
						{
							if (getGameObject(aSourceIndex)->m_PlayerData->m_WCoinC < Obj.pntInventory1[lpMsg->btItemPos)->m_iPShopValue)
							{
								g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, Obj.m_Index, 0, 7);
								return;
							}
						}
						break;
						case 2:
						{
							if (getGameObject(aSourceIndex)->m_PlayerData->m_GoblinPoint < Obj.pntInventory1[lpMsg->btItemPos)->m_iPShopValue)
							{
								g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, Obj.m_Index, 0, 7);
								return;
							}
						}
						break;
						case 3:
						{
							if (getGameObject(aSourceIndex)->m_PlayerData->m_GoblinPoint < Obj.pntInventory1[lpMsg->btItemPos)->m_iPShopValue)
							{
								g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, Obj.m_Index, 0, 7);
								return;
							}
						}
						break;
						}
					}
					else
					{
						switch (g_BotSystem.m_BotData[Obj.m_PlayerData->wBotIndex].m_Shop.iCoinType)
						{
						case 0:
						{
							if (getGameObject(aSourceIndex)->m_PlayerData->Money < Obj.pntInventory1[lpMsg->btItemPos)->m_iPShopValue)
							{
								g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, Obj.m_Index, 0, 7);
								return;
							}
						}
						break;
						case 1:
						{
							if (getGameObject(aSourceIndex)->m_PlayerData->m_WCoinC < Obj.pntInventory1[lpMsg->btItemPos)->m_iPShopValue)
							{
								g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, Obj.m_Index, 0, 7);
								return;
							}
						}
						break;
						case 2:
						{
							if (getGameObject(aSourceIndex)->m_PlayerData->m_GoblinPoint < Obj.pntInventory1[lpMsg->btItemPos)->m_iPShopValue)
							{
								g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, Obj.m_Index, 0, 7);
								return;
							}
						}
						break;
						case 3:
						{
							if (getGameObject(aSourceIndex)->m_PlayerData->m_GoblinPoint < Obj.pntInventory1[lpMsg->btItemPos)->m_iPShopValue)
							{
								g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, Obj.m_Index, 0, 7);
								return;
							}
						}
						break;
						}
					}
				}

				if (Obj.pntInventory1[lpMsg->btItemPos].m_Type != lpMsg->iItemType
					|| Obj.pntInventory1[lpMsg->btItemPos].m_iPShopValue != lpMsg->PShopItemMoneyValue
					|| Obj.pntInventory1[lpMsg->btItemPos].m_wPShopBlessValue != lpMsg->PShopItemBlessJewelValue
					|| Obj.pntInventory1[lpMsg->btItemPos].m_wPShopSoulValue != lpMsg->PShopItemSoulJewelValue
					|| Obj.pntInventory1[lpMsg->btItemPos].m_wPShopChaosValue != lpMsg->PShopItemChaosJewelValue
					)
				{
					g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, Obj.m_Index, 0, 18);

					return;
				}

				short sBlessJewelCount = 0;
				short s10BlessJewelCount = 0;
				short s20BlessJewelCount = 0;
				short s30BlessJewelCount = 0;
				short sSoulJewelCount = 0;
				short s10SoulJewelCount = 0;
				short s20SoulJewelCount = 0;
				short s30SoulJewelCount = 0;
				short sChaosJewelCount = 0;
				short s10ChaosJewelCount = 0;
				short s20ChaosJewelCount = 0;
				short s30ChaosJewelCount = 0;
				short sBlessJewelTotalCount = 0;
				short sSoulJewelTotalCount = 0;
				short sChaosJewelTotalCount = 0;

				if (gCountBlessSoulChaosJewel(aSourceIndex, sBlessJewelCount, s10BlessJewelCount, s20BlessJewelCount, s30BlessJewelCount,
					sSoulJewelCount, s10SoulJewelCount, s20SoulJewelCount, s30SoulJewelCount,
					sChaosJewelCount, s10ChaosJewelCount, s20ChaosJewelCount, s30ChaosJewelCount))
				{
					sBlessJewelTotalCount = 30 * s30BlessJewelCount + 20 * s20BlessJewelCount + 10 * s10BlessJewelCount + sBlessJewelCount;
					sSoulJewelTotalCount = 30 * s30SoulJewelCount + 20 * s20SoulJewelCount + 10 * s10SoulJewelCount + sSoulJewelCount;
					sChaosJewelTotalCount = 30 * s30ChaosJewelCount + 20 * s20ChaosJewelCount + 10 * s10ChaosJewelCount + sChaosJewelCount;

					if (sBlessJewelTotalCount < Obj.pntInventory1[lpMsg->btItemPos].m_wPShopBlessValue)
					{
						g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, Obj.m_Index, 0, 11);

						return;
					}

					if (sSoulJewelTotalCount < Obj.pntInventory1[lpMsg->btItemPos].m_wPShopSoulValue)
					{
						g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, Obj.m_Index, 0, 12);

						return;
					}

					if (sChaosJewelTotalCount < Obj.pntInventory1[lpMsg->btItemPos].m_wPShopChaosValue)
					{
						g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, Obj.m_Index, 0, 13);

						return;
					}
				}

				short sBlessJewelNeedCount = 0;
				short s10BlessJewelNeedCount = 0;
				short s20BlessJewelNeedCount = 0;
				short s30BlessJewelNeedCount = 0;
				short sSoulJewelNeedCount = 0;
				short s10SoulJewelNeedCount = 0;
				short s20SoulJewelNeedCount = 0;
				short s30SoulJewelNeedCount = 0;
				short sChaosJewelNeedCount = 0;
				short s10ChaosJewelNeedCount = 0;
				short s20ChaosJewelNeedCount = 0;
				short s30ChaosJewelNeedCount = 0;

				if (Obj.pntInventory1[lpMsg->btItemPos].m_wPShopBlessValue > 0
					|| Obj.pntInventory1[lpMsg->btItemPos].m_wPShopSoulValue > 0
					|| Obj.pntInventory1[lpMsg->btItemPos].m_wPShopChaosValue > 0)
				{
					int iInvalidValueTestResult = gJewelBuyItemValueTest(aSourceIndex, Obj.m_Index,
						Obj.pntInventory1[lpMsg->btItemPos].m_wPShopBlessValue, Obj.pntInventory1[lpMsg->btItemPos].m_wPShopSoulValue, Obj.pntInventory1[lpMsg->btItemPos].m_wPShopChaosValue,
						sBlessJewelCount, s10BlessJewelCount, s20BlessJewelCount, s30BlessJewelCount,
						sSoulJewelCount, s10SoulJewelCount, s20SoulJewelCount, s30SoulJewelCount,
						sChaosJewelCount, s10ChaosJewelCount, s20ChaosJewelCount, s30ChaosJewelCount,
						sBlessJewelNeedCount, s10BlessJewelNeedCount, s20BlessJewelNeedCount, s30BlessJewelNeedCount,
						sSoulJewelNeedCount, s10SoulJewelNeedCount, s20SoulJewelNeedCount, s30SoulJewelNeedCount,
						sChaosJewelNeedCount, s10ChaosJewelNeedCount, s20ChaosJewelNeedCount, s30ChaosJewelNeedCount);

					if (iInvalidValueTestResult == 14)
					{
						g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, Obj.m_Index, 0, 14);

						return;
					}

					if (iInvalidValueTestResult == 15)
					{
						g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, Obj.m_Index, 0, 15);

						return;
					}

					if (iInvalidValueTestResult == 16)
					{
						g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, Obj.m_Index, 0, 16);
						return;
					}
				}

				DWORD dwCost = Obj.pntInventory1[lpMsg->btItemPos].m_iPShopValue;

				if (dwCost > 0 && gObjCheckMaxZen(Obj.m_Index, dwCost) == FALSE)
				{
					g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, Obj.m_Index, 0, 9);

					return;
				}

				if (g_PentagramSystem.IsPentagramItem(&Obj.pntInventory1[lpMsg->btItemPos]) == true)
				{
					if (Obj.pntInventory1[lpMsg->btItemPos].m_Durability < 1.0)
					{
						return;
					}

					if (!gTransPentagramJewelInfoTest(aSourceIndex, Obj.m_Index, Obj.pntInventory1[lpMsg->btItemPos]))
					{
						g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, Obj.m_Index, 0, 8);
						return;
					}

					Obj.pntInventory1[lpMsg->btItemPos].m_Durability -= 1.0;
				}

				if (Obj.pntInventory1[lpMsg->btItemPos].m_wPShopBlessValue > 0 ||	Obj.pntInventory1[lpMsg->btItemPos].m_wPShopSoulValue > 0 ||
					Obj.pntInventory1[lpMsg->btItemPos].m_wPShopChaosValue > 0)
				{
					int iJewelPutTest = gJewelInventoryPutTest(aSourceIndex, Obj.m_Index,
						Obj.pntInventory1[lpMsg->btItemPos].m_wPShopBlessValue,
						Obj.pntInventory1[lpMsg->btItemPos].m_wPShopSoulValue,
						Obj.pntInventory1[lpMsg->btItemPos].m_wPShopChaosValue);

					if (iJewelPutTest == 100)
					{
						if (g_PentagramSystem.IsPentagramItem(&Obj.pntInventory1[lpMsg->btItemPos]) == TRUE)
						{
							Obj.pntInventory1[lpMsg->btItemPos].m_Durability += 1.0;
						}

						g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, Obj.m_Index, 0, 17);

						return;
					}
				}

				BYTE btNewItemPos = 0;
				btNewItemPos = ::gObjOnlyInventoryInsertItem(aSourceIndex, Obj.pntInventory1[lpMsg->btItemPos]);

				if (btNewItemPos == 0xFF)
				{
					g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, Obj.m_Index, 0, 8);

					return;
				}

				if (g_PentagramSystem.IsPentagramItem(&Obj.pntInventory1[lpMsg->btItemPos]))
				{
					int nJewelCount = 0;
					gTransPentagramJewelInfo(aSourceIndex, btNewItemPos, Obj.m_Index, Obj.pntInventory1[lpMsg->btItemPos], &nJewelCount);
				}

				gJewelInventoryPut(aSourceIndex, Obj.m_Index,
					Obj.pntInventory1[lpMsg->btItemPos].m_wPShopBlessValue,
					Obj.pntInventory1[lpMsg->btItemPos].m_wPShopSoulValue,
					Obj.pntInventory1[lpMsg->btItemPos].m_wPShopChaosValue,
					sBlessJewelNeedCount, s10BlessJewelNeedCount, s20BlessJewelNeedCount, s30BlessJewelNeedCount,
					sSoulJewelNeedCount, s10SoulJewelNeedCount, s20SoulJewelNeedCount, s30SoulJewelNeedCount,
					sChaosJewelNeedCount, s10ChaosJewelNeedCount, s20ChaosJewelNeedCount, s30ChaosJewelNeedCount);

				iITEM_LOG_TYPE = Obj.pntInventory1[lpMsg->btItemPos].m_Type;
				iITEM_LOG_LEVEL = Obj.pntInventory1[lpMsg->btItemPos].m_Level;
				iITEM_LOG_DUR = Obj.pntInventory1[lpMsg->btItemPos].m_Durability;
				iITEM_LOG_SERIAL = Obj.pntInventory1[lpMsg->btItemPos].m_Number;
				iITEM_LOG_BJPRICE = Obj.pntInventory1[lpMsg->btItemPos].m_wPShopBlessValue;
				iITEM_LOG_SJPRICE = Obj.pntInventory1[lpMsg->btItemPos].m_wPShopSoulValue;
				iITEM_LOG_CJPRICE = Obj.pntInventory1[lpMsg->btItemPos].m_wPShopChaosValue;
				if (Obj.m_PlayerData->ISBOT == false)
				{
					g_PersonalStore.gObjInventoryItemSet_PShop(Obj.m_Index, lpMsg->btItemPos, -1);
					Obj.pntInventory1[lpMsg->btItemPos].Clear();
					gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, lpMsg->btItemPos, TRUE);
				}
				if (Obj.m_bOff && Obj.m_PlayerData->ISBOT == false)
				{
					switch (g_ConfigRead.offtrade.CoinType)
					{
					case 0:
					{
						getGameObject(aSourceIndex)->m_PlayerData->Money -= dwCost;

						Obj.m_PlayerData->Money += (dwCost - round(dwCost * 0.01));
					}
					break;
					case 1:
					{
						getGameObject(aSourceIndex)->m_PlayerData->m_WCoinC -= dwCost;
						::GDReqInGameShopPointAdd(aSourceIndex, 0, 0.00 - dwCost);
						::GDReqInGameShopPointAdd(Obj.m_Index, 0, dwCost);
					}
					break;
					case 2:
					{
						getGameObject(aSourceIndex)->m_PlayerData->m_GoblinPoint -= dwCost;
						::GDReqInGameShopPointAdd(aSourceIndex, 2, 0.00 - dwCost);
						::GDReqInGameShopPointAdd(Obj.m_Index, 2, dwCost);
					}
					break;
					case 3:
					{
						getGameObject(aSourceIndex)->m_PlayerData->m_GoblinPoint -= dwCost;
						::GDReqInGameShopPointAdd(aSourceIndex, 2, 0.00 - dwCost);
						::GDReqInGameShopPointAdd(Obj.m_Index, 2, dwCost);
					}
					break;
					}
				}
				else if (Obj.m_PlayerData->ISBOT == TRUE && g_BotSystem.GetBotType(Obj.m_Index) == BOT_SHOP)
				{
					switch (g_BotSystem.m_BotData[Obj.m_PlayerData->wBotIndex].m_Shop.iCoinType)
					{
					case 0:
					{
						getGameObject(aSourceIndex)->m_PlayerData->Money -= dwCost;
					}
					break;
					case 1:
					{
						getGameObject(aSourceIndex)->m_PlayerData->m_WCoinC -= dwCost;
						::GDReqInGameShopPointAdd(aSourceIndex, 0, 0.00 - dwCost);
					}
					break;
					case 2:
					{
						getGameObject(aSourceIndex)->m_PlayerData->m_GoblinPoint -= dwCost;
						::GDReqInGameShopPointAdd(aSourceIndex, 2, 0.00 - dwCost);
					}
					break;
					case 3:
					{
						getGameObject(aSourceIndex)->m_PlayerData->m_GoblinPoint -= dwCost;
						::GDReqInGameShopPointAdd(aSourceIndex, 2, 0.00 - dwCost);
					}
					break;
					}
				}
				else
				{
					getGameObject(aSourceIndex)->m_PlayerData->Money -= dwCost;
					Obj.m_PlayerData->Money += (dwCost - round(dwCost * 0.01)); // tax
				}

				gGameProtocol.GCMoneySend(aSourceIndex, getGameObject(aSourceIndex)->m_PlayerData->Money);
				gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

				if (Obj.m_PlayerData->ISBOT == false)
				{
					g_CashShop.GCCashPoint(getGameObject(Obj.m_Index));
				}

				g_CashShop.GCCashPoint(getGameObject(aSourceIndex));

				g_PersonalStore.CGPShopAnsBuyItem(aSourceIndex, Obj.m_Index, btNewItemPos, 1);

				if (Obj.m_PlayerData->ISBOT == false && g_BotSystem.GetBotType(Obj.m_Index) == BOT_SHOP)
				{
					g_PersonalStore.CGPShopAnsSoldItem(Obj.m_Index, aSourceIndex, lpMsg->btItemPos);
				}

				if (Obj.m_PlayerData->ISBOT == true && g_BotSystem.GetBotType(Obj.m_Index) == BOT_SHOP)
				{
					BotShopLog->Output("[BOT SHOP] [%s][%s][%s] Buy %s from %s for %d", getGameObject(aSourceIndex)->m_PlayerData->ConnectUser->IP, getGameObject(aSourceIndex)->AccountID, getGameObject(aSourceIndex)->Name, ItemAttribute[iITEM_LOG_TYPE)->Name, Obj.Name, dwCost);
				}
				if (g_PersonalStore.PShop_CheckInventoryEmpty(Obj.m_Index) == true)
				{
					gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 113), Obj.m_Index, 1);
					Obj.m_bPShopOpen = false;
					memset(Obj.m_szPShopText, 0, sizeof(Obj.m_szPShopText));
					g_PersonalStore.CGPShopAnsClose(Obj.m_Index, 1);
					Obj.m_bOff = false;
				}
				else
				{
					Obj.m_bPShopItemChange = true;
				}

			}
		}
		__finally
		{
			Obj.m_bPShopTransaction = false;
		}
	}
}

void CPersonalStore::CGPShopAnsBuyItem(int aSourceIndex, int aTargetIndex, int iItemPos, BYTE btResult)
{
	PMSG_ANS_BUYITEM_FROM_PSHOP pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0x3F, 0x06, sizeof(pMsg));
	pMsg.NumberH = SET_NUMBERH(aTargetIndex);
	pMsg.NumberL = SET_NUMBERL(aTargetIndex);
	pMsg.btItemPos = iItemPos;
	pMsg.Result = btResult;
	ItemByteConvert(pMsg.CItemObjectInfo, getGameObject(aSourceIndex)->pntInventory1[iItemPos));

	IOCP.DataSend(aSourceIndex, (BYTE*)&pMsg, pMsg.h.size);
}

void CPersonalStore::CGPShopReqCloseDeal(PMSG_REQ_PSHOPDEAL_CLOSE * lpMsg, CGameObject &Obj)
{
	if (!gObjIsConnected(Obj.m_Index))
		return;

	Obj.m_bPShopWantDeal = false;
	Obj.m_iPShopDealerIndex = -1;
	memset(Obj.m_szPShopDealerName, 0, MAX_ACCOUNT_LEN);
}



void CPersonalStore::CGPShopAnsSoldItem(int aSourceIndex, int aTargetIndex, int iItemPos)
{
	PMSG_ANS_SOLDITEM_FROM_PSHOP pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0x3F, 0x08, sizeof(pMsg));
	pMsg.btItemPos = iItemPos;
	std::memcpy(pMsg.btName, getGameObject(aTargetIndex)->Name, MAX_ACCOUNT_LEN);

	IOCP.DataSend(aSourceIndex, (BYTE*)&pMsg, pMsg.h.size);
}



void CPersonalStore::CGPShopAnsDealerClosedShop(int aSourceIndex, int aTargetIndex)
{
	PMSG_REQ_DEALER_CLOSED_SHOP pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0x3F, 0x12, sizeof(pMsg));
	pMsg.NumberH = SET_NUMBERH(aTargetIndex);
	pMsg.NumberL = SET_NUMBERL(aTargetIndex);

	IOCP.DataSend(aSourceIndex, (BYTE*)&pMsg, pMsg.h.size);
}

void CPersonalStore::PShop_ViewportListRegenarate(CGameObject &Obj)
{
	if (gObjIsConnected(Obj.m_Index) == 0)
	{
		return;
	}

	

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.RegenOk > 0)
	{
		return;
	}

	if (Obj.CloseCount > -1)
	{
		return;
	}

	BYTE cBUFFER[6000];
	int iVpOpenCount = 0;
	int iVpAddCount = 0;

	PMSG_ANS_PSHOP_VIEWPORT_NOTIFY * lpMsg = (PMSG_ANS_PSHOP_VIEWPORT_NOTIFY *)&cBUFFER;
	PMSG_PSHOP_VIEWPORT_NOTIFY * lpMsgBody = (PMSG_PSHOP_VIEWPORT_NOTIFY *)&cBUFFER[6];

	int iPShopOpenIndex[MAX_VIEWPORT];

	if (Obj.m_bPShopRedrawAbs != 0)
	{
		memset(Obj.m_iVpPShopPlayer, 0x00, sizeof(Obj.m_iVpPShopPlayer));
		Obj.m_wVpPShopPlayerCount = 0;
		Obj.m_bPShopRedrawAbs = 0;
	}

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		int tObjIndex = Obj.VpPlayer[n].number;

		if (!ObjectMaxRange(tObjIndex))
		{
			continue;
		}

		if (Obj.VpPlayer[n].state == 2 && Obj.VpPlayer[n].type == OBJ_USER && getGameObject(tObjIndex)->m_bPShopOpen == 1)
		{
			iPShopOpenIndex[iVpOpenCount] = tObjIndex;
			iVpOpenCount++;

			if (Obj.m_wVpPShopPlayerCount > 0 && Obj.m_wVpPShopPlayerCount <= MAX_VIEWPORT)
			{
				int found = 0;

				for (int i = 0; i < Obj.m_wVpPShopPlayerCount; i++)
				{
					if (Obj.m_iVpPShopPlayer[i] == tObjIndex)
					{
						found = 1;
						break;
					}
				}

				if (found == 0)
				{
					lpMsgBody[iVpAddCount].btNumberL = SET_NUMBERL(WORD(tObjIndex));
					lpMsgBody[iVpAddCount].btNumberH = SET_NUMBERH(WORD(tObjIndex));
					std::memcpy(lpMsgBody[iVpAddCount].szPShopText, getGameObject(tObjIndex)->m_szPShopText, sizeof(getGameObject(tObjIndex)->m_szPShopText));
					iVpAddCount++;
				}
			}
			else if (Obj.m_wVpPShopPlayerCount == 0)
			{
				lpMsgBody[iVpAddCount].btNumberL = SET_NUMBERL(WORD(tObjIndex));
				lpMsgBody[iVpAddCount].btNumberH = SET_NUMBERH(WORD(tObjIndex));
				std::memcpy(lpMsgBody[iVpAddCount].szPShopText, getGameObject(tObjIndex)->m_szPShopText, sizeof(getGameObject(tObjIndex)->m_szPShopText));
				iVpAddCount++;
			}
			else
			{
				sLog->outBasic("[PShop] ERROR : Obj.m_wVpPShopPlayerCount is OUT of BOUND : %d", Obj.m_wVpPShopPlayerCount);
			}
		}
	}

	memset(Obj.m_iVpPShopPlayer, 0x00, sizeof(Obj.m_iVpPShopPlayer));
	Obj.m_wVpPShopPlayerCount = 0;

	if (iVpOpenCount > 0 && iVpOpenCount <= MAX_VIEWPORT)
	{
		for (int n = 0; n < iVpOpenCount; n++)
		{
			Obj.m_iVpPShopPlayer[n] = iPShopOpenIndex[n];
		}
		Obj.m_wVpPShopPlayerCount = iVpOpenCount;
	}
	else if (iVpOpenCount != 0)
	{
		sLog->outBasic("[PShop] ERROR : iVpOpenCount is OUT of BOUND: %d", iVpOpenCount);
	}

	if (iVpAddCount > 0 && iVpAddCount <= MAX_VIEWPORT)
	{
		lpMsg->btCount = iVpAddCount;
		PHeadSubSetW((BYTE *)lpMsg, 0x3F, 0, sizeof(PMSG_ANS_PSHOP_VIEWPORT_NOTIFY) + sizeof(PMSG_PSHOP_VIEWPORT_NOTIFY)*iVpAddCount);
		IOCP.DataSend(Obj.m_Index, (BYTE *)lpMsg, ((lpMsg->h.sizeL & 0xFF) & 0xFF | ((lpMsg->h.sizeH & 0xFF) & 0xFF) << 8) & 0xFFFF);
	}
	else if (iVpAddCount != 0)
	{
		sLog->outBasic("[PShop] ERROR : iVpAddCount is OUT of BOUND: %d", iVpAddCount);
	}

	if (Obj.m_bPShopWantDeal == 1)
	{
		int iDealerIndex = Obj.m_iPShopDealerIndex;

		if (ObjectMaxRange(iDealerIndex) == true && gObjIsConnected(iDealerIndex) != 0)
		{
			if (getGameObject(iDealerIndex)->Type == OBJ_USER)
			{
				if (getGameObject(iDealerIndex)->m_bPShopOpen == 0)
				{
					Obj.m_bPShopWantDeal = 0;
					Obj.m_iPShopDealerIndex = -1;
					memset(Obj.m_szPShopDealerName, 0, sizeof(Obj.m_szPShopDealerName));

					this->CGPShopAnsDealerClosedShop(Obj.m_Index, iDealerIndex);
				}
				else if (getGameObject(iDealerIndex)->CloseCount >= 0 || Obj.CloseCount >= 0)
				{
					Obj.m_bPShopWantDeal = 0;
					Obj.m_iPShopDealerIndex = -1;
					memset(Obj.m_szPShopDealerName, 0, sizeof(Obj.m_szPShopDealerName));

					this->CGPShopAnsDealerClosedShop(Obj.m_Index, iDealerIndex);
				}
				else if (getGameObject(iDealerIndex)->m_bPShopItemChange == 1)
				{
					this->CGPShopAnsBuyList(Obj.m_Index, iDealerIndex, 1, 1);
				}
			}
			else
			{
				Obj.m_bPShopWantDeal = 0;
				Obj.m_iPShopDealerIndex = -1;
				memset(Obj.m_szPShopDealerName, 0, sizeof(Obj.m_szPShopDealerName));

				this->CGPShopAnsDealerClosedShop(Obj.m_Index, iDealerIndex);
			}
		}
		else
		{
			Obj.m_bPShopWantDeal = 0;
			Obj.m_iPShopDealerIndex = -1;
			memset(Obj.m_szPShopDealerName, 0, sizeof(Obj.m_szPShopDealerName));

			this->CGPShopAnsDealerClosedShop(Obj.m_Index, iDealerIndex);
		}
	}
}

bool CPersonalStore::PShop_CheckInventoryEmpty(CGameObject &Obj)
{
	

	for (int i = PSHOP_START_RANGE; i < PSHOP_END_RANGE; i++)
	{
		if (Obj.pntInventory1[i]->IsItem() == 1)
		{
			return false;
		}
	}
	return true;
}

void CPersonalStore::gObjInventoryItemSet_PShop(CGameObject &Obj, int itempos, BYTE set_byte)
{
	int width;
	int height;
	if (itempos < INVETORY_WEAR_SIZE)
	{
		return;
	}
	else if (itempos > (INVENTORY_SIZE - 1))
	{
		return;
	}

	if (Obj.pntInventory1[itempos]->GetSize((int&)width, (int &)height) == 0)
	{
		sLog->outBasic("error: Item does not exist %s %d", __FILE__, __LINE__);
		return;
	}

	gObjInventoryItemBoxSet_PShop(Obj.m_Index, itempos, width, height, set_byte);
}



void CPersonalStore::gObjInventoryItemBoxSet_PShop(CGameObject &Obj, int itempos, int xl, int yl, BYTE set_byte)
{
	int itemposx = (itempos - INVETORY_WEAR_SIZE) % 8;
	int itemposy = (itempos - INVETORY_WEAR_SIZE) / 8;

	int xx, yy;

	if (!CheckOutOfInventory(Obj.m_Index, itemposy, yl))
	{
		return;
	}

	for (int y = 0; y < yl; y++)
	{
		yy = itemposy + y;

		for (int x = 0; x < xl; x++)
		{
			xx = itemposx + x;

			if (InventoryExtentCheck(xx, yy, 8, MAX_INVENTORY_H2))
			{
				*(BYTE*)(Obj.InventoryMap1 + (itemposy + y) * 8 + (itemposx + x)) = set_byte;
			}
			else
			{
				sLog->outBasic("error : %s %d", __FILE__, __LINE__);
				return;
			}
		}
	}
}

void CPersonalStore::CGReqSearchItemInPShop(PMSG_REQ_SEARCH_ITEM_PSHOP *lpMsg, CGameObject &Obj)
{
	if (!lpMsg)
	{
		return;
	}

	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	if (lpMsg->sSearchItem == -1)
	{
		this->GCPShop_AllInfo(Obj.m_Index, lpMsg->iLastCount);
	}

	else
	{
		this->GCPShop_SearchItem(Obj.m_Index, lpMsg->sSearchItem, lpMsg->iLastCount);
	}
}

void CPersonalStore::CGReqPShopLog(PMSG_REQ_PSHOP_LOG *lpMsg, CGameObject &Obj)
{
	if (!lpMsg)
	{
		return;
	}

	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	if (!ObjectMaxRange(lpMsg->iTargetIndex))
	{
		return;
	}

	
	CGameObject lpTargetObj = getGameObject(lpMsg->iTargetIndex);

	if (lpMsg->btLogKind == 0)
	{
		sLog->outBasic("[PShopLog] [%s][%s] Whisper To [%s][%s]",
			Obj.AccountID, Obj.Name, lpTargetObj.AccountID, lpTargetObj.Name);
	}

	else if (lpMsg->btLogKind == 1)
	{
		sLog->outBasic("[PShopLog] [%s][%s] Mail To [%s][%s]",
			Obj.AccountID, Obj.Name, lpTargetObj.AccountID, lpTargetObj.Name);
	}
}

void CPersonalStore::GDRequestPShopItemValue(CGameObject &Obj, char *szAccountID)
{
	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		return;
	}

	PMSG_REQ_PSHOPITEMVALUE_INFO pMsg;
	std::memcpy(pMsg.AccountId, szAccountID, MAX_ACCOUNT_LEN + 1);
	std::memcpy(pMsg.szName, Obj.Name, MAX_ACCOUNT_LEN + 1);
	pMsg.iUserIndex = Obj.m_Index;

	PHeadSetB((BYTE*)&pMsg, 0xE9, sizeof(pMsg));
	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
}

void CPersonalStore::GDUpdatePShopItemValue(CGameObject &Obj)
{
	return;
}

void CPersonalStore::GDAllSavePShopItemValue(CGameObject &Obj)
{
	PMSG_UPDATE_PSHOPITEMVALUE_INFO pMsg;
	int iItemCnt = 0;
	bool bExistPShopItem = false;

	for (int i = PSHOP_START_RANGE; i < PSHOP_END_RANGE; i++)
	{
		if (Obj.pntInventory[i]->IsItem() == TRUE)
		{
			bExistPShopItem = true;
			pMsg.PShopItemValueInfo[iItemCnt].nPShopItemInvenNum = i;
			pMsg.PShopItemValueInfo[iItemCnt].ItemSerial = Obj.pntInventory[i]->m_Number;
			pMsg.PShopItemValueInfo[iItemCnt].nMoney = Obj.pntInventory[i]->m_iPShopValue;
			pMsg.PShopItemValueInfo[iItemCnt].sBlessJewelValue = Obj.pntInventory[i]->m_wPShopBlessValue;
			pMsg.PShopItemValueInfo[iItemCnt].sSoulJewelValue = Obj.pntInventory[i]->m_wPShopSoulValue;
			pMsg.PShopItemValueInfo[iItemCnt].sChaosJewelValue = Obj.pntInventory[i]->m_wPShopChaosValue;

			iItemCnt++;
		}
	}

	if (bExistPShopItem == true)
	{
		std::memcpy(pMsg.AccountId, Obj.AccountID, MAX_ACCOUNT_LEN + 1);
		std::memcpy(pMsg.szName, Obj.Name, MAX_ACCOUNT_LEN + 1);
		pMsg.btItemCnt = iItemCnt;

		PHeadSetW((BYTE*)&pMsg, 0xEB, sizeof(pMsg));

		wsDataCli.DataSend((char *)&pMsg, sizeof(pMsg));
	}
}

void CPersonalStore::GDDelPShopItemValue(CGameObject &Obj, int nPShopItemInvenNum)
{
	PMSG_DEL_PSHOPITEM pMsg;

	std::memcpy(pMsg.AccountId, Obj.AccountID, MAX_ACCOUNT_LEN + 1);
	std::memcpy(pMsg.szName, Obj.Name, MAX_ACCOUNT_LEN + 1);
	pMsg.nPShopItemInvenNum = nPShopItemInvenNum;

	PHeadSetB((BYTE*)&pMsg, 0xEC, sizeof(pMsg));
	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
}

void CPersonalStore::GDMovePShopItem(CGameObject &Obj, int nOldPShopItemInvenNum, int nNewPShopItemInvenNum)
{
	PMSG_MOVE_PSHOPITEM pMsg;

	std::memcpy(pMsg.AccountId, Obj.AccountID, MAX_ACCOUNT_LEN + 1);
	std::memcpy(pMsg.szName, Obj.Name, MAX_ACCOUNT_LEN + 1);

	pMsg.nOldPShopItemInvenNum = nOldPShopItemInvenNum;
	pMsg.nNewPShopItemInvenNum = nNewPShopItemInvenNum;

	PHeadSetB((BYTE*)&pMsg, 0xED, sizeof(pMsg));
	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
}

void CPersonalStore::GDAnsPShopItemValue(PMSG_ANS_PSHOPITEMVALUE_INFO *lpMsg)
{
	int iItemCnt = lpMsg->btItemCnt;
	int iObjIndex = lpMsg->iUserIndex;

	if (!ObjectMaxRange(iObjIndex))
	{
		return;
	}

	if (iItemCnt < 0 || iItemCnt > 31)
	{
		return;
	}

	CGameObject lpObj = getGameObject(iObjIndex);

	for (int i = 0; i < iItemCnt; i++)
	{
		if (lpMsg->PShopItemValueInfo[i].nPShopItemInvenNum >= 204 && Obj.pntInventory[lpMsg->PShopItemValueInfo[i]->nPShopItemInvenNum]->IsItem() == TRUE)
		{
			if (Obj.pntInventory[lpMsg->PShopItemValueInfo[i]->nPShopItemInvenNum]->m_Number == lpMsg->PShopItemValueInfo[i]->ItemSerial)
			{
				Obj.pntInventory[lpMsg->PShopItemValueInfo[i]->nPShopItemInvenNum]->m_iPShopValue = lpMsg->PShopItemValueInfo[i]->nMoney;
				Obj.pntInventory[lpMsg->PShopItemValueInfo[i]->nPShopItemInvenNum]->m_wPShopBlessValue = lpMsg->PShopItemValueInfo[i]->sBlessJewelValue;
				Obj.pntInventory[lpMsg->PShopItemValueInfo[i]->nPShopItemInvenNum]->m_wPShopSoulValue = lpMsg->PShopItemValueInfo[i]->sSoulJewelValue;
				Obj.pntInventory[lpMsg->PShopItemValueInfo[i]->nPShopItemInvenNum]->m_wPShopChaosValue = lpMsg->PShopItemValueInfo[i]->sChaosJewelValue;
			}
		}
	}

	this->GCPShopItemValueInfo(iObjIndex);
}

void CPersonalStore::GCPShopItemValueInfo(CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	if (!gObjIsConnected(Obj.m_Index))
	{
		return;
	}

	BYTE sendbuf[4096];
	memset(sendbuf, 0x00, sizeof(sendbuf));

	PMSG_PSHOPITEMVALUE_INFO_COUNT pMsg;
	PSHOP_ITEMVALUE_INFO PShopItemValueInfo[32];

	int lOfs = sizeof(pMsg);
	int iItemCnt = 0;

	

	for (int i = PSHOP_START_RANGE; i < PSHOP_END_RANGE; i++)
	{
		if (Obj.pntInventory[i]->IsItem() == TRUE)
		{
			PShopItemValueInfo[iItemCnt].nPShopItemInvenNum = i;
			PShopItemValueInfo[iItemCnt].Empty = 0;
			PShopItemValueInfo[iItemCnt].nMoney = Obj.pntInventory[i]->m_iPShopValue;
			PShopItemValueInfo[iItemCnt].sBlessJewelValue = Obj.pntInventory[i]->m_wPShopBlessValue;
			PShopItemValueInfo[iItemCnt].sSoulJewelValue = Obj.pntInventory[i]->m_wPShopSoulValue;
			PShopItemValueInfo[iItemCnt].sChaosJewelValue = Obj.pntInventory[i]->m_wPShopChaosValue;

			std::memcpy(&sendbuf[lOfs], &PShopItemValueInfo[iItemCnt], sizeof(PSHOP_ITEMVALUE_INFO));
			lOfs += sizeof(PSHOP_ITEMVALUE_INFO);
			iItemCnt++;
		}
	}

	pMsg.Result = iItemCnt > 0;
	pMsg.btMoneyCommisionRate = 1;
	pMsg.btJewelCommisionRate = 3;
	pMsg.btItemCnt = iItemCnt;

	PHeadSubSetW((BYTE*)&pMsg, 0xEC, 0x32, lOfs);
	std::memcpy(sendbuf, &pMsg, sizeof(pMsg));

	IOCP.DataSend(Obj.m_Index, sendbuf, lOfs);

}

void CPersonalStore::GCPShop_AllInfo(CGameObject &Obj, int iLastUserCount)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	if (!gObjIsConnected(Obj.m_Index))
	{
		return;
	}

	BYTE sendbuf[4096];
	memset(sendbuf, 0x00, sizeof(sendbuf));

	PMSG_SEARCH_ITEM_PSHOP_COUNT pMsg;
	PMSG_SEARCH_ITEM_PSHOP SearchItemPShop[1000];

	int lOfs = sizeof(PMSG_SEARCH_ITEM_PSHOP_COUNT);
	int iAddCnt = 0;

	for (int n = g_ConfigRead.server.GetObjectStartUserIndex(); n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		CGameObject lpObj = getGameObject(n);

		if (getGameObject(n)->Connected == PLAYER_PLAYING && Obj.Type == OBJ_USER &&
			(Obj.MapNumber == MAP_INDEX_RORENCIA || Obj.MapNumber == MAP_INDEX_DEVIAS || Obj.MapNumber == MAP_INDEX_NORIA ||
				Obj.MapNumber == MAP_INDEX_ELBELAND))
		{
			if (Obj.m_Index != Obj.m_Index)
			{
				if ((MapC[Obj.MapNumber].GetAttr(Obj.X, Obj.Y) & 1) == 1)
				{
					if (Obj.m_bPShopOpen == true)
					{
						if (iAddCnt >= iLastUserCount)
						{
							SearchItemPShop[iAddCnt].btNumberL = LOBYTE(Obj.m_Index);
							SearchItemPShop[iAddCnt].btNumberH = HIBYTE(Obj.m_Index);
							std::memcpy(SearchItemPShop[iAddCnt].szName, Obj.Name, MAX_ACCOUNT_LEN + 1);
							std::memcpy(SearchItemPShop[iAddCnt].szPShopText, Obj.m_szPShopText, 37);

							std::memcpy(&sendbuf[lOfs], &SearchItemPShop[iAddCnt], sizeof(PMSG_SEARCH_ITEM_PSHOP));
							lOfs += sizeof(PMSG_SEARCH_ITEM_PSHOP);
						}

						iAddCnt++;

						if (iLastUserCount < iAddCnt)
						{
							if (!(iAddCnt % 50))
							{
								pMsg.iPShopCnt = iAddCnt;
								pMsg.btContinueFlag = TRUE;

								PHeadSubSetW((BYTE*)&pMsg, 0xEC, 0x31, lOfs);
								std::memcpy(sendbuf, &pMsg, sizeof(pMsg));

								IOCP.DataSend(Obj.m_Index, sendbuf, lOfs);
								return;
							}
						}
					}
				}
			}
		}
	}

	if (iAddCnt)
	{
		pMsg.iPShopCnt = iAddCnt;
		pMsg.btContinueFlag = 0;
	}

	else
	{
		pMsg.iPShopCnt = -1;
		pMsg.btContinueFlag = 0;
	}

	PHeadSubSetW((BYTE*)&pMsg, 0xEC, 0x31, lOfs);
	std::memcpy(sendbuf, &pMsg, sizeof(pMsg));

	IOCP.DataSend(Obj.m_Index, sendbuf, lOfs);
}

void CPersonalStore::GCPShop_SearchItem(CGameObject &Obj, WORD sSearchItem, int iLastUserCount)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	if (!gObjIsConnected(Obj.m_Index))
	{
		return;
	}

	BYTE sendbuf[4096];
	memset(sendbuf, 0x00, sizeof(sendbuf));

	PMSG_SEARCH_ITEM_PSHOP_COUNT pMsg;
	PMSG_SEARCH_ITEM_PSHOP SearchItemPShop[1000];

	int lOfs = sizeof(PMSG_SEARCH_ITEM_PSHOP_COUNT);
	int iAddCnt = 0;

	for (int n = g_ConfigRead.server.GetObjectStartUserIndex(); n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		CGameObject lpObj = getGameObject(n);

		if (getGameObject(n)->Connected == PLAYER_PLAYING && Obj.Type == OBJ_USER &&
			(Obj.MapNumber == MAP_INDEX_RORENCIA || Obj.MapNumber == MAP_INDEX_DEVIAS || Obj.MapNumber == MAP_INDEX_NORIA ||
				Obj.MapNumber == MAP_INDEX_ELBELAND))
		{
			if (Obj.m_Index != Obj.m_Index)
			{
				if ((MapC[Obj.MapNumber].GetAttr(Obj.X, Obj.Y) & 1) == 1)
				{
					if (Obj.m_bPShopOpen == true)
					{
						if (this->PShop_CheckExistItemInInventory(n, sSearchItem) == TRUE)
						{
							if (iAddCnt >= iLastUserCount)
							{
								SearchItemPShop[iAddCnt].btNumberL = LOBYTE(Obj.m_Index);
								SearchItemPShop[iAddCnt].btNumberH = HIBYTE(Obj.m_Index);
								std::memcpy(SearchItemPShop[iAddCnt].szName, Obj.Name, MAX_ACCOUNT_LEN + 1);
								std::memcpy(SearchItemPShop[iAddCnt].szPShopText, Obj.m_szPShopText, 37);

								std::memcpy(&sendbuf[lOfs], &SearchItemPShop[iAddCnt], sizeof(PMSG_SEARCH_ITEM_PSHOP));
								lOfs += sizeof(PMSG_SEARCH_ITEM_PSHOP);
							}

							iAddCnt++;

							if (iLastUserCount < iAddCnt)
							{
								if (!(iAddCnt % 50))
								{
									pMsg.iPShopCnt = iAddCnt;
									pMsg.btContinueFlag = TRUE;

									PHeadSubSetW((BYTE*)&pMsg, 0xEC, 0x31, lOfs);
									std::memcpy(sendbuf, &pMsg, sizeof(pMsg));

									IOCP.DataSend(Obj.m_Index, sendbuf, lOfs);
									return;
								}
							}
						}
					}
				}
			}
		}
	}

	if (iAddCnt)
	{
		pMsg.iPShopCnt = iAddCnt;
		pMsg.btContinueFlag = 0;
	}

	else
	{
		pMsg.iPShopCnt = -1;
		pMsg.btContinueFlag = 0;
	}

	PHeadSubSetW((BYTE*)&pMsg, 0xEC, 0x31, lOfs);
	std::memcpy(sendbuf, &pMsg, sizeof(pMsg));

	IOCP.DataSend(Obj.m_Index, sendbuf, lOfs);
}

bool CPersonalStore::PShop_CheckExistItemInInventory(CGameObject &Obj, WORD sItemType)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return false;
	}

	if (!gObjIsConnected(Obj.m_Index))
	{
		return false;
	}

	for (int i = PSHOP_START_RANGE; i < PSHOP_END_RANGE; i++)
	{
		if (Obj.pntInventory1[i]->m_Type == sItemType)
		{
			return true;
		}
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

