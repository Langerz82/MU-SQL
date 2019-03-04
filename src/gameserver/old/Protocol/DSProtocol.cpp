////////////////////////////////////////////////////////////////////////////////
// DSProtocol.cpp
// ------------------------------
// Decompiled by Deathway
// Date : 2007-03-09
// ------------------------------
// GS-n 0.99.60T 0x0041D4D0
// GS-N	1.00.18	JNP	0x00424550	-	Completed

#include "StdAfx.h"
#include "DSProtocol.h"
#include "GameMain.h"
#include "GameServer.h"
#include "DevilSquare.h"
#include "MapServerManager.h"
#include "CastleSiegeSync.h"
#include "ChaosBox.h"
#include "QuestInfo.h"
#include "TNotice.h"
#include "SProtocol.h"
#include "EDSProtocol.h"
#include "GameProtocol.h"
#include "CUserData.h"
#include "CGameObject.h"
#include "GOFunctions.h"
#include "IOCP.h"
#include "ObjUseSkill.h"
#include "PeriodItemEx.h"
#include "BuffEffect.h"
#include "BloodCastle.h"
#include "Utility/util.h"
#include "ObjCalCharacter.h"
#include "CrywolfSync.h"
#include "CashShop.h"
#include "Event.h"
#include "configread.h"
#include "VipSys.h"
#include "GensSystem.h"
#include "ArcaBattle.h"
#include "MasterLevelSkillTreeSystem.h"
#include "Crywolf.h"
#include "CastleSiege.h"
#include "QuestExpUserMng.h"
#include "LuckyItemManager.h"
#include "PentagramSystem.h"
#include "ItemSocketOptionSystem.h"
#include "MuunSystem.h"
#include "SendNPCInfo.h"
#include "ChaosCastleFinal.h"
#include "MuRummyMng.h"
#include "MineSystem.h"
#include "PersonalStore.h"
#include "KeyGenerater.h"
#include "AcheronGuardianEvent.h"
#include "classdef.h"
#include "Shop.h"
#include "GamblingSystem.h"
#include "GremoryCase.h"
#include "UnityBattleField.h"
#include "ChaosCastle.h"
#include "StatSpecialize.h"
#include "EvolutionMonsterMng.h"
#include "ItemDropEx.h"
#include "SocketItemType.h"

int gMapSrvGroupOnlineServer = 0;

void(*ItemCreate)(CGameObject* lpObj, BYTE MapNumber, BYTE x, BYTE y, int type, BYTE level, BYTE dur, BYTE Op1, BYTE Op2, BYTE Op3, int LootIndex, BYTE NewOption, BYTE SetOption, time_t lDuration, BYTE *SocketOption, BYTE MainAttribute);

void DataServerProtocolCore(BYTE protoNum, unsigned char* aRecv, int aLen)
{

#if(TRACE_PROTOCOL==1)
	LogAddHeadHex("DATA_SERVER", aRecv, aLen);
#endif

	switch (protoNum)
	{
	case 0x00:
		DataServerLoginResult((SDHP_RESULT *)aRecv);
		break;

	case 0x02:
		DCInfo.DGAnsClassDefData((PMSG_ANS_CLASSDEF *)aRecv);
		break;

	case 0x03:
		DGAnsSwitchWare((PMSG_ANS_SWITCHWARE *)aRecv);
		break;

	case 0x04:
		JGCharacterCreateRequest((SDHP_CREATECHARRESULT *)aRecv);
		break;

	case 0x05:
		JGCharDelRequest((SDHP_CHARDELETERESULT *)aRecv);
		break;

	case 0x06:
		JGGetCharacterInfo((SDHP_DBCHAR_INFORESULT *)aRecv);
		break;

	case 0x08:
		DGGetWarehouseList((SDHP_GETWAREHOUSEDB_SAVE *)aRecv);
		break;

	case 0x09:
		DGSetWarehouseList((PMSG_ANS_WARESAVE *)aRecv);
		break;

	case 0x10:
		GDGetWarehouseNoItem((SDHP_GETWAREHOUSEDB_RESULT *)aRecv);
		break;

	case 0x4C:
		if (aRecv[3] == 0x00)
			g_MineSystem.GDAnsModifyMineSystemUPTUserInfo((SDHP_ANS_MINESYSTEM_UPT_USERINFO *)aRecv);
		else if (aRecv[3] == 0x01)
			g_MineSystem.GDAnsLoadMineSystemUPTUserInfo((SDHP_ANS_LOAD_MINESYSTEM_UPT_USERINFO *)aRecv);
		break;

	case 0x3E:
		if (aRecv[3] == 0x00)
			g_EvoMonMng.DGAnsEvoMonMaxScore(aRecv);
		break;

	case 0x4F:
	{
		PMSG_DEFAULT2 * lpDef1 = (PMSG_DEFAULT2*)aRecv;
		PWMSG_DEFAULT2 * lpDef2 = (PWMSG_DEFAULT2*)aRecv;

		if (aRecv[0] == 0xC1)
		{
			switch (lpDef1->subcode)
			{
			case 0x01:
				g_GremoryCase.DGAnsAddItemToGremoryCase((_stAnsAddItemToGremoryCase *)aRecv);
				break;
			case 0x02:
				g_GremoryCase.DGAnsCheckItemUseGremoryCase((_stAnsCheckUseItemGremoryCase *)aRecv);
				break;
			}
		}

		else if (aRecv[0] == 0xC2)
		{
			switch (lpDef2->subcode)
			{
			case 0x00:
				g_GremoryCase.DGAnsStorageItemList(aRecv);
				break;
			}
		}
	}
	break;

	case 0x52:
	{
		ItemSerialCreateRecv((SDHP_ITEMCREATERECV *)aRecv);
		break;
	}
	case 0x56:
		DGRecvPetItemInfo(aRecv);
		break;

	case 0x60:
		DGOptionDataRecv((SDHP_SKILLKEYDATA_SEND *)aRecv);
		break;

	case 0x6F:
	{
		PMSG_DEFAULT2 * lpDef1 = (PMSG_DEFAULT2*)aRecv;
		PWMSG_DEFAULT2 * lpDef2 = (PWMSG_DEFAULT2*)aRecv;

		if (aRecv[0] == 0xC1)
		{
			switch (lpDef1->subcode)
			{
			case 0x02:
				g_CancelItemSale.DGAnsGetReBuyItem((SDHP_ANS_SHOP_REBUY_GET_ITEM *)aRecv);
				break;
			}
		}

		else if (aRecv[0] == 0xC2)
		{
			switch (lpDef2->subcode)
			{
			case 0x00:
				g_CancelItemSale.DGAnsSoldItemList(aRecv);
				break;
			}
		}
	}
	break;

	case 0x80:
	{
		PMSG_DEFAULT2 * lpDef1 = (PMSG_DEFAULT2 *)aRecv;

		switch (lpDef1->subcode)
		{
		case 0x00:
			GS_DGAnsCastleTotalInfo(aRecv);
			break;

		case 0x01:
			GS_DGAnsOwnerGuildMaster(aRecv);
			break;

		case 0x03:
			GS_DGAnsCastleNpcBuy(aRecv);
			break;

		case 0x04:
			GS_DGAnsCastleNpcRepair(aRecv);
			break;

		case 0x05:
			GS_DGAnsCastleNpcUpgrade(aRecv);
			break;

		case 0x06:
			GS_DGAnsTaxInfo(aRecv);
			break;

		case 0x07:
			GS_DGAnsTaxRateChange(aRecv);
			break;

		case 0x08:
			GS_DGAnsCastleMoneyChange(aRecv);
			break;

		case 0x09:
			GS_DGAnsSiegeDateChange(aRecv);
			break;

		case 0x0A:
			GS_DGAnsGuildMarkRegInfo(aRecv);
			break;

		case 0x0B:
			GS_DGAnsSiegeEndedChange(aRecv);
			break;

		case 0x0C:
			GS_DGAnsCastleOwnerChange(aRecv);
			break;

		case 0x0D:
			GS_DGAnsRegAttackGuild(aRecv);
			break;

		case 0x0E:
			GS_DGAnsRestartCastleState(aRecv);
			break;

		case 0x0F:
			GS_DGAnsMapSvrMsgMultiCast(aRecv);
			break;

		case 0x10:
			GS_DGAnsRegGuildMark(aRecv);
			break;

		case 0x11:
			GS_DGAnsGuildMarkReset(aRecv);
			break;

		case 0x12:
			GS_DGAnsGuildSetGiveUp(aRecv);
			break;

		case 0x16:
			GS_DGAnsNpcRemove(aRecv);
			break;

		case 0x17:
			GS_DGAnsCastleStateSync(aRecv);
			break;

		case 0x18:
			GS_DGAnsCastleTributeMoney(aRecv);
			break;

		case 0x19:
			GS_DGAnsResetCastleTaxInfo(aRecv);
			break;

		case 0x1A:
			GS_DGAnsResetSiegeGuildInfo(aRecv);
			break;

		case 0x1B:
			GS_DGAnsResetRegSiegeInfo(aRecv);
			break;

		case 0x1F:
			GS_DGAnsGlobalPostMultiCast(aRecv);
			break;
		}
	}
	break;

	case 0x81:
		GS_DGAnsCastleInitData(aRecv);
		break;

	case 0x82:
		GS_DGAnsCastleNpcInfo(aRecv);
		break;

	case 0x83:
		GS_DGAnsAllGuildMarkRegInfo(aRecv);
		break;

	case 0x84:
		GS_DGAnsFirstCreateNPC(aRecv);
		break;

	case 0x85:
		GS_DGAnsCalcREgGuildList(aRecv);
		break;

	case 0x86:
		GS_DGAnsCsGulidUnionInfo(aRecv);
		break;

	case 0x87:
		GS_DGAnsCsSaveTotalGuildInfo(aRecv);
		break;

	case 0x88:
		GS_DGAnsCsLoadTotalGuildInfo(aRecv);
		break;

	case 0x89:
		GS_DGAnsCastleNpcUpdate(aRecv);
		break;

	case 0x61:
		DGMuBotOptionRecv((LPMUBOT_SETTINGS_SEND)aRecv);
		break;

	case 0xB0:
		DGAnsCrywolfSync(aRecv);
		break;

	case 0xB1:
		DGAnsCrywolfInfoLoad(aRecv);
		break;

	case 0xB2:
		DGAnsCrywolfInfoSave(aRecv);
		break;

	case 0xBD:
	{
		PMSG_DEFAULT2 * lpDef1 = (PMSG_DEFAULT2 *)aRecv;

		switch (lpDef1->subcode)
		{
		case 0x02:
			EGAnsBloodCastleEnterCount((PMSG_ANS_BLOODCASTLE_ENTERCOUNT *)aRecv);
			break;
		}
	}
	break;

	case 0xBE:
		EventChipEventProtocolCore(aRecv[3], aRecv, aLen);
		break;

	case 0xCF:
		DGMoveOtherServer((SDHP_CHARACTER_TRANSFER_RESULT *)aRecv);
		break;

	case 0xC0:
	{
		//PMSG_DEFAULT2 * lpDef1 = (PMSG_DEFAULT2 *)aRecv;

		//switch ( lpDef1->subcode )
		//{
		//case 0x01 :
			//STAT_REQ_ANS_SAVE_LOAD_DATA *lpMsg = reinterpret_cast<STAT_REQ_ANS_SAVE_LOAD_DATA *>(aRecv);
			//if(gObj[lpMsg->aIndex].UserStatistics != 0)
			//{
				//gObj[lpMsg->aIndex].UserStatistics->LoadData(&lpMsg->UserData);
			//}
			//break;
		//}
	}
	break;

	case 0xD0:
	{
		PMSG_DEFAULT2 * lpDef1 = (PMSG_DEFAULT2 *)aRecv;
		PWMSG_DEFAULT2 * lpDef2 = (PWMSG_DEFAULT2*)aRecv;

		if (aRecv[0] == 0xC1)
		{
			switch (lpDef1->subcode)
			{
			case 0x01:
				g_PeriodItemEx.DGPeriodItemExInsert((PMSG_ANS_PERIODITEMEX_INSERT *)aRecv);
				break;

			case 0x02:
				g_PeriodItemEx.DGPeriodItemExDelete((PMSG_ANS_PERIODITEMEX_DELETE *)aRecv);
				break;

			case 0x04:
				g_PeriodItemEx.DGPeriodItemExListCount((PMSG_ANS_PERIODITEMEX_LIST_COUNT *)aRecv);
				break;

			case 0x05:
				g_PeriodItemEx.DGPeriodItemExList((PMSG_ANS_PERIODITEMEX_LIST *)aRecv);
				break;
			}
		}

		else if (aRecv[0] == 0xC2)
		{
			switch (lpDef2->subcode)
			{
			case 0x03:
				g_PeriodItemEx.DGPeriodItemExExpiredItemList((PMSG_ANS_PERIODITEMEX_EXPIRED_ITEMLIST *)aRecv);
				break;
			}
		}
	}

	break;

	case 0xD1:
		DGAnsInGameShopPoint((ISHOP_ANS_POINT *)aRecv);
		break;
	case 0xD2:
		DGAnsInGameShopItemList(aRecv);
		break;
	case 0xD4:
		g_LuckyItemManager.DGAnsLuckyItemList((PMSG_ANS_LUCKYITEM_SELECT *)aRecv);
		break;
	case 0xD5:
		DGAnsInGameShopItemBuy(aRecv);
		break;
	case 0xD6:
		DGAnsInGameShopItemGift(aRecv);
		break;
	case 0xD9:
		DGAnsInGameShopItemUse((ISHOP_ITEM_USEANS *)aRecv);
		break;
	case 0xE0:
		g_PentagramSystem.DBANS_GetPentagramJewel(aRecv);
		break;

	case 0xE4:
	{
		PMSG_DEFAULT2 * lpDef1 = (PMSG_DEFAULT2 *)aRecv;

		switch (lpDef1->subcode)
		{
		case 0x03:
			g_BuffEffect.DGPeriodItemExSelect((PMSG_ANS_PERIODBUFF_SELECT *)aRecv);
			break;
		}
	}
	break;

	case 0xE5:
		g_ArcaBattle.DGAnsArcaBattleAllJoinUser((PMSG_ANS_AB_ALL_JOIN_USER_DS *)aRecv);
		break;

	case 0xE6:
		DGLoadEventInvenItem((_tagSDHP_ANS_DBEVENT_INVEN_LOAD *)aRecv);
		break;

	case 0xE8:
		g_CMuRummyMng.GDAnsCardInfo((PMSG_ANS_MURUMMY_SELECT_DS *)aRecv);
		break;

	case 0xE9:
		g_PersonalStore.GDAnsPShopItemValue((PMSG_ANS_PSHOPITEMVALUE_INFO *)aRecv);
		break;

	case 0xF5:
		g_ChaosCastleFinal.DG_Ans_CCF_Rank((PMSG_ANS_CCF_RANK *)aRecv);
		break;

	case 0xF9:
		if (aRecv[3] == 0x01)
		{
			DGAns_ChaosCastle_KillPoint_Result((SDHP_ANS_KILLPOINT_RESULT_CC_UBF *)aRecv);
		}
		else if (aRecv[3] == 0xA3)
		{
			DGAns_GetCCFPermission((SDHP_ANS_CCF_GETPERMISSION *)aRecv);
		}
		else if (aRecv[3] == 0xA4)
		{
			DGAns_CCF_Info_Notify((SDHP_ANS_CCF_INFO_NOTIFY *)aRecv);
		}
		break;

	case 0xFD:
		if (aRecv[0] == 0xC1)
		{
			if (aRecv[3] == 0x00)
			{
				DG_DSF_CanPartyEnter((PMSG_ANS_DSF_CAN_PARTY_ENTER *)aRecv);
			}

			else if (aRecv[3] == 0x03)
			{
				DG_DSF_PartyRankRenew((PMSG_ANS_DSF_PARTYRANKRENEW *)aRecv);
			}

			else if (aRecv[3] == 0x07)
			{
				DG_DSF_GetReward((PMSG_ANS_GET_DSF_REWARD *)aRecv);
			}
		}

		else if (aRecv[0] == 0xC2)
		{
			if (aRecv[4] == 0x05)
			{
				DG_DSF_GoFinalParty(aRecv);
			}
		}
		break;

	case 0xF6:
		g_QuestExpUserMng.UserQuestInfoLoad((PMSG_ANS_QUESTEXP_INFO *)aRecv);
		break;

	case 0xF8:
	{
		PMSG_DEFAULT2 * lpDef1 = (PMSG_DEFAULT2*)aRecv;
		PWMSG_DEFAULT2 * lpDef2 = (PWMSG_DEFAULT2*)aRecv;

		if (aRecv[0] == 0xC1)
		{
			switch (lpDef1->subcode)
			{
			case 0x2B:
				DGAnsArcaBattleGuildJoinSelect((PMSG_ANS_ARCA_BATTLE_GUILD_JOIN_DS *)aRecv);
				break;
			case 0x31:
				DGAnsArcaBattleGuildJoin((PMSG_ANS_ARCA_BATTLE_GUILD_JOIN_DS *)aRecv);
				break;
			case 0x33:
				DGAnsArcaBattleGuildMemberJoin((PMSG_ANS_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS *)aRecv);
				break;
			case 0x35:
				DGAnsArcaBattleEnter((PMSG_ANS_ARCA_BATTLE_ENTER_DS *)aRecv);
				break;
			case 0x3A:
				DGAnsArcaBattleWinGuildInfo((PMSG_ANS_AB_WIN_GUILD_INFO_DS *)aRecv);
				break;
			case 0x40:
				DGReqArcaBattleProcMultiCast((PMSG_ANS_AB_PROC_STATE_DS *)aRecv);
				break;
			case 0x42:
				GDAnsJoinMemberUnder((PMSG_ANS_AB_JOIN_MEMBER_UNDER_DS *)aRecv);
				break;
			case 0x44:
				g_ArcaBattle.DGAnsArcaBattleJoinMemberUnderReq((PMSG_ANS_AB_JOIN_CANCEL_DS *)aRecv);
				break;
			case 0x46:
				g_ArcaBattle.DGAnsRegisteredMemberCnt((PMSG_ANS_AB_REG_MEMBER_CNT_DS *)aRecv);
				break;
			case 0x48:
				g_ArcaBattle.DGAnsRemoveAllRewardBuff();
				break;
			case 0x4C:
				g_ArcaBattle.DGAnsRemoveRewardBuff((PMSG_ANS_REMOVE_GUILD_BUFF_DS *)aRecv);
				break;
			case 0x4E:
				g_ArcaBattle.DGAnsMarkCnt((PMSG_ANS_ARCA_BATTLE_MARK_CNT_DS *)aRecv);
				break;
			case 0x50:
				g_ArcaBattle.DGAnsMarkReg((PMSG_ANS_ARCA_BATTLE_MARK_REG_DS *)aRecv);
				break;
			case 0x52:
				g_ArcaBattle.DGAnsMarkRank((PMSG_ANS_ARCA_BATTLE_MARK_RANK_DS *)aRecv);
				break;
			case 0xF1:
				g_AcheronGuardianEvent.DGAnsAcheronEventProcMultiCast((PMSG_ANS_AE_PLAY_DS *)aRecv);
				break;
			default:
				if (aRecv[4] == 0xFC)
					g_ArcaBattle.DGAnsAllGuildMarkCnt((PMSG_ANS_ALL_GUILD_MARK_CNT_DS *)aRecv);
				break;
			}
		}
	}
	break;
	case 0xF3:
	{
		PMSG_DEFAULT2 * lpDef1 = (PMSG_DEFAULT2 *)aRecv;

		switch (lpDef1->subcode)
		{
		case 0x01:
			DGAnsUBFAccountUserInfo((PMSG_ANS_UBF_ACCOUNT_USERINFO *)aRecv);
			break;
		case 0x02:
			DGUBFRegisterAccountUserResult((PMSG_UBF_REGISTER_ACCOUNT_USER_RESULT *)aRecv);
			break;
		case 0x03:
			DGUBFAccountUserCopyResult((PMSG_UBF_ACCOUNT_USER_COPY_RESULT *)aRecv);
			break;
		case 0x05:
			DGAnsSetReceivedWinnerItem((PMSG_ANS_UBF_SET_RECEIVED_REWARD *)aRecv);
			break;
		case 0x06:
			DGAnsUBFGetReward((PMSG_ANS_UBF_GET_REWARD *)aRecv);
			break;
		case 0x07:
			DGUBFAnsCancelToJoinResult((PMSG_UBF_ANS_CANCEL_REGISTER_USER *)aRecv);
			break;
		case 0x08:
			DGUBFAnsGetRealNameAndServerCode((PMSG_ANS_GET_UBF_REAL_NAME *)aRecv);
			break;
		}
	}
	break;
	case 0xF1:
		g_CMuunSystem.DGLoadMuunInvenItem((_tagSDHP_ANS_DBMUUN_INVEN_LOAD *)aRecv);
		break;
	case 0xFF:	// Test Packet
	{
		PMSG_TEST * pMsg = (PMSG_TEST *)aRecv;

		sLog->outError("error-L2DataServer RECV : %d (%d)", aLen, pMsg->count);
	}
	break;
	case 0xC2:// JoinMu
		DGReqOtherChannelWishper((PMSG_RECV_CHATDATA_WHISPER*)aRecv);
		break;
	case 0xC3:
	{
		PMSG_DEFAULT2 * lpDef1 = (PMSG_DEFAULT2 *)aRecv;
		switch (lpDef1->subcode)
		{
		case 0x01:
		{
			DSMSG_ANS_SUBSERVER_COUNT * Ans = (DSMSG_ANS_SUBSERVER_COUNT*)aRecv;
			gMapSrvGroupOnlineServer = Ans->wSvrCount;
		}
		break;
		case 0x03:
		{
			DGWhisperResponseRecv((DSMSG_GS_WHISPER_RESULT*)aRecv);
		}
		break;
		case 0x04:
		{
			DGDisconnectOtherChannel((PMSG_RECV_DC_OTHER_CHANNEL*)aRecv);
		}
		break;
		}
	}
	break;
	}
}

void DataServerLogin()
{
	SDHP_SERVERINFO pInfo;

	pInfo.h.c = 0xC1;
	pInfo.h.size = sizeof(SDHP_SERVERINFO);	// Size
	pInfo.h.headcode = 0x00;
	pInfo.Port = g_ConfigRead.server.GetGameServerPort();
	pInfo.Type = 1;
	pInfo.ServerCode = g_ConfigRead.server.GetGameServerCode();
	strcpy(pInfo.ServerName, g_ConfigRead.server.GetServerName());

	//wsDataCli.DataSend((char *)&pInfo, sizeof(pInfo)); // TODO
}

void DataServerLoginResult(SDHP_RESULT * lpMsg)
{
	if (lpMsg->Result == false)
	{
		sLog->outError("Authentication error from authentication server");
		return;
	}

	DataServerConnected = TRUE;
	gServerReady++;
	SendMessage(ghWnd, WM_START_SERVER, 0, 0); // Review the Message WM_USER +5

	DCInfo.GDReqClassDefData();
}


/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Get Char List Request Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 0x01
 */
struct SDHP_GETCHARLIST
{
	PBMSG_HEAD h;	// C1:01
	char Id[10];	// 3
	short Number;	// E
	int IsUnityBattleFieldServer;
};


void JGGetCharacterInfo(SDHP_DBCHAR_INFORESULT * lpMsg)
{
	PMSG_CHARMAPJOINRESULT pjMsg;

	char szAccountId[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
	CGameObject* lpObj = getGameObject(lpMsg->Number);

	szAccountId[MAX_ACCOUNT_LEN] = 0;
	memcpy(szAccountId, lpMsg->AccountID, MAX_ACCOUNT_LEN);

	if (gObjIsAccontConnect(*lpObj, szAccountId) == FALSE)
	{
		sLog->outError("Request to receive character information doesn't match the user %s", szAccountId);
		GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);

		return;
	}

	szName[MAX_ACCOUNT_LEN] = 0;
	memcpy(szName, lpMsg->Name, MAX_ACCOUNT_LEN);

	if (lpMsg->result == false)
	{
		sLog->outError("error : %s Character data setting fail %s", szName, lpObj->m_PlayerData->ConnectUser->AccountID);
		GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);

		return;
	}

	if ((lpMsg->CtlCode & 1) != 0)
	{
		sLog->outError("error-L1 : Load Block Character infomation", szName, lpObj->m_PlayerData->ConnectUser->AccountID);	// Deathway Fix HERE
		GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);

		return;
	}

	for each(std::pair<int,CGameObject*> user in gGameObjects)
	{
		if (user.second->m_PlayerData->ConnectUser->Connected == PLAYER_PLAYING)
		{
			if (user.second->Type == OBJ_USER)
			{
				if (!strncmp(szName, user.second->Name, MAX_ACCOUNT_LEN) || !strncmp(szAccountId, user.second->m_PlayerData->ConnectUser->AccountID, MAX_ACCOUNT_LEN))
				{
					sLog->outBasic("[Anti-HACK][JGGetCharacterInfo] Attempted Character-Copy by double logging [%s][%s]",
						szName, lpObj->m_PlayerData->ConnectUser->AccountID);
					GSProtocol.GCSendDisableReconnect(lpObj);
					//GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
					return;
				}
			}
		}
	}


	if (gObjSetCharacter((UCHAR*)lpMsg, *lpObj) == false)
	{
		sLog->outError("error : %s Character data setting fail", szName);
		sLog->outError("CL:%d LV:%d MLV:%d IDX:%d CS:%d", lpMsg->Class, lpMsg->Level, lpMsg->mLevel, lpObj->m_Index, lpObj->m_PlayerData->ConnectUser->Connected);
		GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
		return;
	}

	lpObj->iShield = lpObj->iMaxShield + lpObj->iAddShield;

	if (lpObj->m_PlayerData->Money < 0)
	{
		sLog->outError("ERROR: MONEY < 0 [%s][%s] %d -> 0", szName, lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->m_PlayerData->Money);
		lpObj->m_PlayerData->Money = 2000000000;
	}

	if (lpObj->m_PlayerData->m_bMapSvrMoveReq == false)
	{
		if (MapNumberCheck(lpObj->MapNumber) == FALSE)
		{
			GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
			return;
		}

		if (lpObj->m_PK_Level > 4)
		{
			lpObj->MapNumber = MAP_INDEX_RORENCIA;
			lpObj->X = 143;
			lpObj->Y = 135;
			lpObj->TX = 143;
			lpObj->TY = 135;

			sLog->outBasic("[CRenewal][PKSetting][%s][%s] PkLevel:[%d],KillCount:[%d], PkPoint:[%d] PK Character move to LORENCIA",
				lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PK_Level, lpObj->m_PK_Count, lpObj->m_PK_Time);
		}

		short wGameServerCode = g_MapServerManager.CheckMoveMapSvr(*lpObj, lpObj->MapNumber, lpObj->m_PlayerData->m_sPrevMapSvrCode);

		if (wGameServerCode != g_ConfigRead.server.GetGameServerCode())
		{
			if (wGameServerCode == -1)
			{
				sLog->outError("[MapServerMng] Map Server Move Fail : CheckMoveMapSvr() == -1 [%s][%s] (%d)",
					lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_Index);
				GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);

				return;
			}

			GJReqMapSvrMove(lpObj, wGameServerCode, lpObj->MapNumber, lpObj->X, lpObj->Y);
			sLog->outBasic("[MapServerMng] Request to Move Map Server : (%d) - [%s][%s] (%d)",
				wGameServerCode, lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_Index);

			return;
		}
	}

	lpObj->m_PlayerData->m_bMapSvrMoveReq = false;
	lpObj->m_PlayerData->m_bMapSvrMoveReq_2 = false;

	pjMsg.h.c = 0xC3;
	pjMsg.h.headcode = 0xF3;
	pjMsg.h.size = sizeof(PMSG_CHARMAPJOINRESULT);
	pjMsg.subcode = 0x03;
	pjMsg.MapX = lpObj->X;
	pjMsg.MapY = lpObj->Y;
	pjMsg.MapNumber = lpObj->MapNumber;
	pjMsg.Dir = lpObj->Dir;

	UINT64 Experience = lpObj->m_PlayerData->Experience;

	if (g_MasterLevelSkillTreeSystem.IsMasterLevelUser(*lpObj) != FALSE)
	{
		Experience = lpObj->m_PlayerData->MasterExperience;
	}

	pjMsg.ExpHHH = SET_NUMBERH(SET_NUMBERHW(HIDWORD(Experience)));
	pjMsg.ExpHHL = SET_NUMBERL(SET_NUMBERHW(HIDWORD(Experience)));
	pjMsg.ExpHLH = SET_NUMBERH(SET_NUMBERLW(HIDWORD(Experience)));
	pjMsg.ExpHLL = SET_NUMBERL(SET_NUMBERLW(HIDWORD(Experience)));
	pjMsg.ExpLHH = SET_NUMBERH(SET_NUMBERHW(LODWORD(Experience)));
	pjMsg.ExpLHL = SET_NUMBERL(SET_NUMBERHW(LODWORD(Experience)));
	pjMsg.ExpLLH = SET_NUMBERH(SET_NUMBERLW(LODWORD(Experience)));
	pjMsg.ExpLLL = SET_NUMBERL(SET_NUMBERLW(LODWORD(Experience)));

	UINT64 NextExperience = lpObj->m_PlayerData->NextExp;

	if (g_MasterLevelSkillTreeSystem.IsMasterLevelUser(*lpObj) != FALSE)
	{
		NextExperience = lpObj->m_PlayerData->MasterNextExp;
	}

	pjMsg.NextExpHHH = SET_NUMBERH(SET_NUMBERHW(HIDWORD(NextExperience)));
	pjMsg.NextExpHHL = SET_NUMBERL(SET_NUMBERHW(HIDWORD(NextExperience)));
	pjMsg.NextExpHLH = SET_NUMBERH(SET_NUMBERLW(HIDWORD(NextExperience)));
	pjMsg.NextExpHLL = SET_NUMBERL(SET_NUMBERLW(HIDWORD(NextExperience)));
	pjMsg.NextExpLHH = SET_NUMBERH(SET_NUMBERHW(LODWORD(NextExperience)));
	pjMsg.NextExpLHL = SET_NUMBERL(SET_NUMBERHW(LODWORD(NextExperience)));
	pjMsg.NextExpLLH = SET_NUMBERH(SET_NUMBERLW(LODWORD(NextExperience)));
	pjMsg.NextExpLLL = SET_NUMBERL(SET_NUMBERLW(LODWORD(NextExperience)));

	pjMsg.LevelUpPoint = lpObj->m_PlayerData->LevelUpPoint;
	pjMsg.Str = lpObj->m_PlayerData->Strength;
	pjMsg.Dex = lpObj->m_PlayerData->Dexterity;
	pjMsg.Vit = lpObj->m_PlayerData->Vitality;
	pjMsg.Energy = lpObj->m_PlayerData->Energy;
	pjMsg.Money = lpObj->m_PlayerData->Money;
	pjMsg.PkLevel = lpObj->m_PK_Level;
	pjMsg.Life = lpObj->Life;
	pjMsg.MaxLife = lpObj->AddLife + lpObj->MaxLife;
	pjMsg.Mana = lpObj->Mana;
	pjMsg.MaxMana = lpObj->AddMana + lpObj->MaxMana;
	pjMsg.wShield = lpObj->iShield;
	pjMsg.wMaxShield = lpObj->iMaxShield + lpObj->iAddShield;
	pjMsg.CtlCode = lpMsg->CtlCode;
	pjMsg.BP = lpObj->BP;
	pjMsg.MaxBP = lpObj->MaxBP + lpObj->AddBP;
	pjMsg.Leadership = lpObj->Leadership;
	pjMsg.InvExpansion = lpObj->m_PlayerData->m_InventoryExpansion;
	pjMsg.IGCLife = lpObj->Life;
	pjMsg.IGCMaxLife = lpObj->AddLife + lpObj->MaxLife;
	pjMsg.IGCMana = lpObj->Mana;
	pjMsg.IGCMaxMana = lpObj->AddMana + lpObj->MaxMana;
	pjMsg.Ruud = lpObj->m_PlayerData->Ruud;
	pjMsg.GetTick = GetTickCount();
	short AddPoint = 0;
	short MaxAddPoint = 0;
	short MinusPoint = 0;
	short MaxMinusPoint = 0;

	gObjGetStatPointState(*lpObj, AddPoint, MaxAddPoint, MinusPoint, MaxMinusPoint);
	pjMsg.AddPoint = AddPoint;
	pjMsg.MaxAddPoint = MaxAddPoint;
	pjMsg.wMinusPoint = MinusPoint;
	pjMsg.wMaxMinusPoint = MaxMinusPoint;

	if (AddPoint < 0 || AddPoint > MaxAddPoint || MinusPoint < 0 || MinusPoint > MaxMinusPoint)
	{
	}

	if (g_ArcaBattle.IsArcaBattleServer() == TRUE && g_AcheronGuardianEvent.IsPlayStart() == FALSE)
	{
		g_ArcaBattle.SendArcaBattlePlayInfo(*lpObj, 1);
		g_ArcaBattle.SendArcaBattleCurrentState(*lpObj);
	}

	if (g_ArcaBattle.IsArcaBattleServer() == TRUE && g_AcheronGuardianEvent.IsPlayStart() == TRUE)
	{
		g_ArcaBattle.SendArcaBattleCurrentState(*lpObj);
	}

	if (g_ArcaBattle.IsArcaBattleServer() == TRUE && g_AcheronGuardianEvent.IsPlayStart() == FALSE)
	{
		g_ArcaBattle.EnterArcaBattleEvent(*lpObj);
	}

	g_QuestInfo.QuestInfoSave(*lpObj);

	PMSG_RESET_INGAME_SEND pRSMsg = { 0 };
	pRSMsg.h.c = 0xC1;
	pRSMsg.h.headcode = 0xFA;
	pRSMsg.h.size = sizeof(pRSMsg);
	pRSMsg.h.subcode = 0x0B;
	pRSMsg.Resets = lpObj->m_PlayerData->m_iResets;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pRSMsg, pRSMsg.h.size);
	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pjMsg, pjMsg.h.size);

	GCItemListSend(lpObj);
	FriendListRequest(lpObj);

	g_CMuRummyMng.GDReqCardInfo(*lpObj);
	g_CMuunSystem.GDReqLoadMuunInvenItem(*lpObj);
	g_MasterLevelSkillTreeSystem.SendMLData(*lpObj);
	g_MasterLevelSkillTreeSystem.CGReqGetMasterLevelSkillTree(*lpObj);
	GSProtocol.GCMagicListMultiSend(lpObj, 0);
	GDReqLoadEventInvenItem(lpObj);
	//GSProtocol.CGRequestQuestInfo(*lpObj); // TODO
	g_PeriodItemEx.RequestPeriodItemInfo(*lpObj);
	g_LuckyItemManager.GDReqLuckyItemSelect(*lpObj);
	g_GensSystem.ReqExDBGensInfo(*lpObj);
	g_GensSystem.GDReqAbusingInfo(*lpObj);
	g_GensSystem.ReqGensRewardDay(*lpObj);
	g_BuffEffect.RequestPeriodBuffSelect(*lpObj);
	g_PentagramSystem.DBREQ_GetPentagramJewel(*lpObj, lpMsg->AccountID, 0);
	g_PentagramSystem.DBREQ_GetPentagramJewel(*lpObj, lpMsg->AccountID, 1);
	g_PersonalStore.GDRequestPShopItemValue(*lpObj, lpMsg->AccountID);
	g_MineSystem.CheckIsUPTUserWhenConnect(*lpObj);
	g_GremoryCase.GDReqStorageItemList(*lpObj);
	g_SendNPCInfo.SendNpcCoordinate(lpObj, lpObj->MapNumber, 0);
	g_SendNPCInfo.SendPortalCoordinate(lpObj, lpObj->MapNumber);
	//[K2]
	g_UnityBattleField.GDReqCheckJoinedUnityBattleField(*lpObj, g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE ? TRUE : FALSE, 0);
	g_UnityBattleField.SendUBFNotice(lpObj);
	g_EvoMonMng.GCSendEvoMonNotice(lpObj);
	g_EvoMonMng.GDReqEvoMonMaxScore(lpObj);
	DGGuildMemberInfoRequest(lpObj);

	lpObj->m_PlayerData->m_dwMapMoveKeyValue = g_KeyGenerater.GenerateSeedValue();
	GSProtocol.GCSendMapMoveCheckSum(lpObj);

	GSProtocol.GCPlayerStatsPanelRates(lpObj);
	GSProtocol.GCPlayerStatsPanelNew(lpObj);

	if (g_ConfigRead.data.common.EnterGameMessage == 1)
	{
		PMSG_CHATDATA_WHISPER pMsg;
		PHeadSetB((LPBYTE)&pMsg, 0x02, sizeof(pMsg));

		memset(pMsg.chatmsg, 0x00, sizeof(pMsg.chatmsg));
		memcpy(pMsg.id, lpObj->Name, sizeof(pMsg.id));
		strcpy(pMsg.chatmsg, Lang.GetText(0, 544));

		pMsg.h.size -= sizeof(pMsg.chatmsg);
		pMsg.h.size += strlen(Lang.GetText(0, 544)) + 1;

		GSProtocol.DataSendAll((LPBYTE)&pMsg, pMsg.h.size);
	}

	if (lpObj->m_PlayerData->VipType > 0)
	{
		MsgOutput(*lpObj, Lang.GetText(0, 493), g_VipSystem.GetVipName(lpObj->m_PlayerData->VipType));
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_NORMAL)
	{
		GSProtocol.GCServerMsgStringSend((char *)g_ConfigRead.data.common.WelcomeMessage.c_str(), lpObj, 0);
	}

	gObjUseSkill.SkillChangeUse(*lpObj);

	GSProtocol.GCReFillSend(lpObj, lpObj->Life, 0xFF, 0, lpObj->iShield);
	GSProtocol.GCReFillSend(lpObj, lpObj->AddLife + lpObj->MaxLife, 0xFE, 0, lpObj->iAddShield + lpObj->iMaxShield);

	GSProtocol.GCManaSend(lpObj, lpObj->Mana, 0xFF, 0, lpObj->BP);
	GSProtocol.GCManaSend(lpObj, lpObj->MaxMana, 0xFE, 0, lpObj->MaxBP);

	//GSProtocol.GCResSendExpEventInfo(lpObj); // TODO
	g_QuestExpUserMng.DB_ReqUserQuestInfo(lpObj);

	PMSG_SETAGILITYBUG pAgilityBug;
	pAgilityBug.h.c = 0xC1;
	pAgilityBug.h.headcode = 0xFA;
	pAgilityBug.h.size = sizeof(pAgilityBug);
	pAgilityBug.subcode = 0xA2;

	if (lpObj->Class == CLASS_RAGEFIGHTER || lpObj->Class == CLASS_ELF)
	{
		pAgilityBug.value = 0x0F;
	}
	else
	{
		pAgilityBug.value = 0x02; ///[K2]
	}

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pAgilityBug, sizeof(pAgilityBug));

	//if (g_ConfigRead.data.common.LuckyItemAllowUseJewels == true)
	{
		BYTE dataenable[] = { 0xC1, 0x04, 0xFA, 0xA4 };
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, dataenable, sizeof(dataenable));
	}

	if (g_ConfigRead.data.common.joinmuRemoveItemSellDropRestriction == true)
	{
		BYTE dataenable[] = { 0xC1, 0x04, 0xFA, 0xB0 };
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, dataenable, sizeof(dataenable));
	}

	//BYTE TEST[] = { 0xC1, 0x06, 0x6D, 0x00, 0x01, 0x00 }; //Help Icon
	//GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, TEST, sizeof(TEST));

	GSProtocol.GCSendRareItemPriceInfo(lpObj);
	g_StatSpec.SendOptionList(*lpObj);

	if (g_ConfigRead.data.common.bReconnectSystem == 0)
	{
		GSProtocol.GCSendDisableReconnectSystem(lpObj);
	}

	for (int n = 0; n < INVENTORY_SIZE; n++)
	{
		if (lpObj->pntInventory[n]->IsPeriodItemExpire() == 1)
		{
			lpObj->pntInventory[n]->SetPeriodItemExpire();
			g_PeriodItemEx.RemovePeriodItemDataByForce(*lpObj, lpObj->pntInventory[n]->m_Type, lpObj->pntInventory[n]->m_Number);
			lpObj->pntInventory[n]->Clear();
		}
	}
}

void GCItemListSend(CGameObject* lpObj)
{
	PMSG_INVENTORYLISTCOUNT pMsgILC;	// Packet Message Item List Count
	PMSG_INVENTORYLIST pMsgIL;	// Packet Message Item List
	int sOfs = sizeof(PMSG_INVENTORYLISTCOUNT);
	int pMsgILSize = sizeof(PMSG_INVENTORYLIST);
	BYTE sendBuf[4096];
	int itemcount = 0;

	for (int n = 0; n < INVENTORY_SIZE; n++)
	{
		if (lpObj->pntInventory[n]->IsItem() == TRUE)
		{
			if (lpObj->pntInventory[n]->m_bItemExist == false)
			{
				continue;
			}

			pMsgIL.Pos = n;
			ItemByteConvert(pMsgIL.ItemInfo, *lpObj->pntInventory[n]);
			memcpy(&sendBuf[sOfs], &pMsgIL, pMsgILSize);
			itemcount++;
			sOfs += pMsgILSize;
		}
	}

	pMsgILC.h.c = 0xC4;
	pMsgILC.h.headcode = 0xF3;
	pMsgILC.subcode = 0x10;
	pMsgILC.h.sizeH = SET_NUMBERH(sOfs);
	pMsgILC.h.sizeL = SET_NUMBERL(sOfs);
	pMsgILC.Count = itemcount;
	memcpy(sendBuf, &pMsgILC, sizeof(PMSG_INVENTORYLISTCOUNT));

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, sendBuf, sOfs);
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Save Character Info Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC2
 *	HeadCode  : 0x07
 */
struct SDHP_DBCHAR_INFOSAVE
{
	PWMSG_HEAD h;	// C2:07
	char AccountID[11];
	char Name[11];	// 4
	short Level;	// 10
	short mLevel;
	BYTE Class;	// 12
	int LevelUpPoint;	// 14
	int mlPoint;
	UINT64 Exp;	// 18
	UINT64 mlExp;
	UINT64 NextExp;	// 1C
	UINT64 mlNextExp;
	int Money;	// 20
	WORD Str;	// 24
	WORD Dex;	// 26
	WORD Vit;	// 28
	WORD Energy;	// 2A
	DWORD Life;	// 2C
	DWORD MaxLife;	// 2E
	DWORD Mana;	// 30
	DWORD MaxMana;	// 32
	BYTE dbInventory[7584];	// 34
	BYTE dbMagicList[450];	// 6F4
	BYTE MapNumber;	// 7A8
	BYTE MapX;	// 7A9
	BYTE MapY;	// 7AA
	BYTE Dir;	// 7AB
	int PkCount;	// 7AC
	int PkLevel;	// 7B0
	int PkTime;	// 7B4
	BYTE dbQuest[100];	// 7B8
	BYTE CharInfoSave;	// 7EA
	WORD Leadership;	// 7EC
	WORD ChatLitmitTime;	// 7EE
	int iFruitPoint;	// 7F0
	int resets;
	char MarryName[11];
	char Married;
	int WinDuels;
	int LoseDuels;
	time_t ChatBlockTime;
	DWORD PenaltyMask;
	int Ruud;
	BYTE EventMap;
};

void GJSetCharacterInfo(CGameObject* lpObj, BOOL bMapServerMove)
{
	if (lpObj->m_PlayerData->m_bMapSvrMoveQuit == true)
	{
		sLog->outError("[MapServerMng] GJSetCharacterInfo() - Inventory Already Saved [%s][%s] (%d)",
			lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_Index);

		return;
	}

	if (bMapServerMove == TRUE)
	{
		lpObj->m_PlayerData->m_bMapSvrMoveQuit = TRUE;

		sLog->outBasic("[MapServerMng][%s][%s] GJSetCharacterInfo() - MapServerMove Character Save Start (%d)",
			lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_Index);
	}

	if (lpObj->Type == OBJ_USER && lpObj->m_btOpenWarehouse == TRUE)
	{
		GDSetWarehouseList(lpObj, FALSE);
	}

	SDHP_DBCHAR_INFOSAVE pCSave;	// Packet Character Save
	pCSave.h.c = 0xC2;
	pCSave.h.headcode = 0x07;
	pCSave.h.sizeH = sizeof(SDHP_DBCHAR_INFOSAVE) >> 8;
	pCSave.h.sizeL = sizeof(SDHP_DBCHAR_INFOSAVE) & 0xFF;
	pCSave.Name[MAX_ACCOUNT_LEN] = 0;
	memcpy(pCSave.AccountID, lpObj->m_PlayerData->ConnectUser->AccountID, sizeof(pCSave.AccountID) - 1);
	memcpy(pCSave.Name, lpObj->Name, sizeof(pCSave.Name) - 1);
	pCSave.Class = lpObj->m_PlayerData->DbClass;
	pCSave.Level = lpObj->Level;
	pCSave.mLevel = lpObj->m_PlayerData->MasterLevel;
	pCSave.LevelUpPoint = lpObj->m_PlayerData->LevelUpPoint;
	pCSave.mlPoint = lpObj->m_PlayerData->MasterPoint;
	pCSave.Exp = lpObj->m_PlayerData->Experience;
	pCSave.mlExp = lpObj->m_PlayerData->MasterExperience;
	pCSave.NextExp = lpObj->m_PlayerData->NextExp;
	pCSave.mlNextExp = lpObj->m_PlayerData->MasterNextExp;
	pCSave.Money = lpObj->m_PlayerData->Money;
	pCSave.Str = lpObj->m_PlayerData->Strength;
	pCSave.Dex = lpObj->m_PlayerData->Dexterity;
	pCSave.Vit = lpObj->m_PlayerData->Vitality;
	pCSave.Energy = lpObj->m_PlayerData->Energy;
	pCSave.Life = lpObj->Life;
	pCSave.MaxLife = lpObj->MaxLife;
	pCSave.Mana = lpObj->Mana;
	pCSave.MaxMana = lpObj->MaxMana;
	ItemByteConvert32((LPBYTE)pCSave.dbInventory, *lpObj->pntInventory, INVENTORY_SIZE);
	MagicByteConvert(pCSave.dbMagicList, lpObj->Magic, MAGIC_SIZE);
	pCSave.MapNumber = lpObj->MapNumber;
	pCSave.MapX = lpObj->X;
	pCSave.MapY = lpObj->Y;
	pCSave.Dir = lpObj->Dir;
	pCSave.PkCount = lpObj->m_PK_Count;
	pCSave.PkLevel = lpObj->m_PK_Level;
	pCSave.PkTime = lpObj->m_PK_Time;
	pCSave.Leadership = lpObj->Leadership;
	pCSave.ChatLitmitTime = lpObj->ChatLimitTime;
	pCSave.iFruitPoint = lpObj->m_PlayerData->iFruitPoint;
	pCSave.CharInfoSave = 0;
	pCSave.resets = lpObj->m_PlayerData->m_iResets;
	memcpy(pCSave.dbQuest, lpObj->m_PlayerData->m_Quest, sizeof(pCSave.dbQuest));
	memcpy(pCSave.MarryName, lpObj->MarryName, sizeof(pCSave.MarryName));
	pCSave.Married = lpObj->Married;
	pCSave.WinDuels = lpObj->WinDuels;
	pCSave.LoseDuels = lpObj->LoseDuels;
	pCSave.ChatBlockTime = lpObj->ChatBlockTime;
	pCSave.PenaltyMask = lpObj->PenaltyMask;
	//pCSave.Ruud = lpObj->EventMap;
	pCSave.Ruud = lpObj->m_PlayerData->Ruud;
	pCSave.EventMap = lpObj->EventMap;
	/*if (wsDataCli.DataSend((char*)&pCSave, sizeof(SDHP_DBCHAR_INFOSAVE)) == FALSE)
	{
		sLog->outError("%s Character data save fail", lpObj->Name);
		return;
	}*/ // TODO

	gObjItemTextSave(*lpObj);
	gObjStatTextSave(*lpObj);
	gObjSavePetItemInfo(lpObj, 0);
	g_LuckyItemManager.GDReqLuckyItemInsert2nd(lpObj);
	g_QuestExpUserMng.UserAllQuestInfoSave(lpObj);
	g_GensSystem.GDReqSaveContributePoint(*lpObj);
	g_GensSystem.DBSaveAbusingKillUserName(*lpObj);
	g_PentagramSystem.DBREQ_SetPentagramJewel(*lpObj, 0);
	g_PentagramSystem.DBREQ_SetPentagramJewel(*lpObj, 1);
	GDReqSaveEventInvenItem(lpObj);
	g_CMuunSystem.GDReqSaveMuunInvenItem(*lpObj);

}



/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Get Warehouse Info Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 0x08
 */
struct SDHP_GETWAREHOUSEDB
{
	PBMSG_HEAD h;	// C1:08
	char AccountID[11];	// 3
	short aIndex;	// E
};

void GDGetWarehouseList(CGameObject* lpObj, char* AccountID)
{
	if (strlen(AccountID) < 1)
	{
		return;
	}

	if (lpObj->m_PlayerData->m_bMapSvrMoveReq == true)
	{
		sLog->outBasic("[GetWarehouseList] MapServerMove User. Can't Open WhareHouse. return!! [%s], IP [%s] ",
			AccountID, lpObj->m_PlayerData->ConnectUser->IP);
		return;
	}

	if (lpObj->m_State == 32)
	{
		sLog->outBasic("[GetWarehouseList] MapServerMove User. Can't Open WhareHouse. return!! [%s], IP [%s] ",
			AccountID, lpObj->m_PlayerData->ConnectUser->IP);
		return;
	}

	if (lpObj->m_PlayerData->m_bMapSvrMoveQuit == true)
	{
		sLog->outBasic("[GetWarehouseList] MapServerMove User. Can't Open WhareHouse. return!! [%s], IP [%s] ",
			AccountID, lpObj->m_PlayerData->ConnectUser->IP);
		return;
	}

	SDHP_GETWAREHOUSEDB	pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x08;
	pMsg.h.size = sizeof(SDHP_GETWAREHOUSEDB);
	pMsg.aIndex = lpObj->m_Index;
	memcpy(pMsg.AccountID, AccountID, sizeof(pMsg.AccountID));

	//wsDataCli.DataSend((char*)&pMsg, pMsg.h.size); // TODO
}




void DGGetWarehouseList(SDHP_GETWAREHOUSEDB_SAVE * lpMsg)
{
	char szId[11];
	PMSG_TALKRESULT pResult;

	szId[MAX_ACCOUNT_LEN] = 0;
	memcpy(szId, lpMsg->AccountID, sizeof(lpMsg->AccountID));
	CGameObject* lpObj = getGameObject(lpMsg->aIndex);


	if (lpObj->m_PlayerData->m_bMapSvrMoveReq == true)
	{
		sLog->outBasic("[DGGetWarehouseList] MapServerMove User. Can't Open WhareHouse. return!! [%s], IP [%s] ",
			szId, lpObj->m_PlayerData->ConnectUser->IP);
		return;
	}

	if (lpObj->m_State == 32)
	{
		sLog->outBasic("[DGGetWarehouseList] MapServerMove User. Can't Open WhareHouse. return!! [%s], IP [%s] ",
			szId, lpObj->m_PlayerData->ConnectUser->IP);
		return;
	}

	if (lpObj->m_PlayerData->m_bMapSvrMoveQuit == true)
	{
		sLog->outBasic("[DGGetWarehouseList] MapServerMove User. Can't Open WhareHouse. return!! [%s], IP [%s] ",
			szId, lpObj->m_PlayerData->ConnectUser->IP);
		return;
	}

	if (!gObjIsAccontConnect(*lpObj, szId))
	{
		sLog->outError("Error-L1 : Request to receive Warehouse information doesn't match the user [%s][%d]", szId, aIndex);
		return;
	}

	int itype;
	int _type;
	BYTE OptionData;
	CItemObject* item;

	memset(lpObj->WarehouseMap, (BYTE)-1, WAREHOUSE_SIZE);
	lpObj->WarehouseMoney = lpMsg->Money;
	lpObj->WarehousePW = lpMsg->pw;

	int count = 0;
	for (int n = 0; n < WAREHOUSE_SIZE; n++)
	{
		itype = lpMsg->dbItems[n*MAX_DBITEM_INFO];

		itype |= (lpMsg->dbItems[n*MAX_DBITEM_INFO + 9] & 0xF0) << 5;
		itype |= (lpMsg->dbItems[n*MAX_DBITEM_INFO + 7] & 0x80) << 1;
		_type = itype;

		lpObj->pntWarehouse[n]->Clear();

		if (lpMsg->dbItems[n*MAX_DBITEM_INFO] == (BYTE)-1 && (lpMsg->dbItems[n*MAX_DBITEM_INFO + 9] & 0xF0) == 0xF0 && (lpMsg->dbItems[n*MAX_DBITEM_INFO + 7] & 0x80) == 0x80)
		{
			itype = -1;
		}

		if (IsItem(_type) == FALSE)
		{
			itype = -1;
		}

		if (_type == ITEMGET(14, 17) || _type == ITEMGET(14, 18))
		{
			if (lpMsg->dbItems[n*MAX_DBITEM_INFO + 2] == 0)	// Level 0
			{
				_type = -1;	// #error maybe this is itype
			}
		}

		if (_type == ITEMGET(14, 19))
		{
			if (lpMsg->dbItems[n*MAX_DBITEM_INFO + 2] == (BYTE)-1)
			{
				itype = -1;
			}
		}

		if (itype != -1)
		{
			item->m_Level = DBI_GET_LEVEL(lpMsg->dbItems[n*MAX_DBITEM_INFO + 1]);

			if (_type != ITEMGET(14, 11))	// Box Of Luck
			{
				if (item->m_Level > 15)
				{
					item->m_Level = 0;
				}
			}

			OptionData = lpMsg->dbItems[n*MAX_DBITEM_INFO + 1];
			item->m_Option1 = DBI_GET_SKILL(OptionData);
			item->m_Option2 = DBI_GET_LUCK(OptionData);
			item->m_Option3 = DBI_GET_OPTION(OptionData);
			item->m_Option3 |= DBI_GET_DINO_OPTION(lpMsg->dbItems[n*MAX_DBITEM_INFO + 7]);
			item->m_Durability = lpMsg->dbItems[n*MAX_DBITEM_INFO + 2];
			item->m_JewelOfHarmonyOption = lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_JOH_DATA];
			item->m_ItemOptionEx = DBI_GET_380OPTION(lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_OPTION380_DATA]);

			if (item->m_ItemOptionEx != 0)
			{
				item->m_Type = itype;
				if (g_kItemSystemFor380.Is380Item(item) == FALSE)
				{
					item->m_ItemOptionEx = 0;
				}
			}

			BYTE SocketOption[5];
			BYTE SocketOptionIndex = 0xFF;
			int i = 0;
			memset(&SocketOption, 0xFF, 5);

			if (gSocketItemType.CheckSocketItemType(itype) == TRUE)
			{
				SocketOptionIndex = lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_JOH_DATA];

				for (i = 0; i < 5; i++)
				{
					SocketOption[i] = lpMsg->dbItems[((n*MAX_DBITEM_INFO) + DBI_SOCKET_1) + i];
				}
			}

			else if (g_PentagramSystem.IsPentagramItem(itype) == TRUE ||
				g_PentagramSystem.IsPentagramJewel(itype) == TRUE)
			{
				SocketOptionIndex = lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_JOH_DATA];

				for (i = 0; i < 5; i++)
				{
					SocketOption[i] = lpMsg->dbItems[((n*MAX_DBITEM_INFO) + DBI_SOCKET_1) + i];
				}
			}

			else
			{
				SocketOptionIndex = 0;

				for (i = 0; i < 5; i++)
				{
					SocketOption[i] = 0xFF;
				}
			}

			item->m_PeriodItemOption = (lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_OPTION380_DATA] & 6) >> 1;

			item->Convert(itype, item->m_Option1, item->m_Option2, item->m_Option3, DBI_GET_NOPTION(lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_NOPTION_DATA]), lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SOPTION_DATA], item->m_ItemOptionEx, SocketOption, SocketOptionIndex, item->m_PeriodItemOption, CURRENT_DB_VERSION);

			if (_type == ITEMGET(14, 7)) // Siege Potion
			{

			}

			else if ((_type >= ITEMGET(14, 0) && _type <= ITEMGET(14, 8)) || (_type >= ITEMGET(14, 35) && _type <= ITEMGET(14, 40)))
			{
				if (item->m_Durability == 0.0f)
				{
					item->m_Durability = 1.0f;
				}
			}
			else if (_type != ITEMGET(13, 10) && _type != ITEMGET(14, 29) && _type != ITEMGET(14, 21) && item->m_Level == 3)
			{
				if (item->m_Durability > item->m_BaseDurability)
				{
					item->m_Durability = item->m_BaseDurability;
				}
			}

			int iMaxOverlap = IsOverlapItem(item->m_Type);

			if (iMaxOverlap != 0)
			{
				if (item->m_Durability > iMaxOverlap)
				{
					item->m_Durability = iMaxOverlap;
				}
			}

			lpObj->pntWarehouse[n]->m_Option1 = item->m_Option1;
			lpObj->pntWarehouse[n]->m_Option2 = item->m_Option2;
			lpObj->pntWarehouse[n]->m_Option3 = item->m_Option3;
			lpObj->pntWarehouse[n]->m_JewelOfHarmonyOption = item->m_JewelOfHarmonyOption;
			lpObj->pntWarehouse[n]->m_ItemOptionEx = item->m_ItemOptionEx;

			DWORD hidword = MAKE_NUMBERDW(MAKE_NUMBERW(lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL1], lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL2]), MAKE_NUMBERW(lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL3], lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL4]));
			DWORD lodword = MAKE_NUMBERDW(MAKE_NUMBERW(lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL5], lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL6]), MAKE_NUMBERW(lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL7], lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL8]));
			item->m_Number = MAKEQWORD(hidword, lodword);

			gObjWarehouseInsertItemPos(*lpObj, *item, n, -1);
			gObjWarehouseItemSet(*lpObj, n, 1);

			count++;
		}
	}

	pResult.h.c = 0xC3;
	pResult.h.headcode = 0x30;
	pResult.h.size = sizeof(PMSG_TALKRESULT);
	pResult.result = 2;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
	GSProtocol.GCUserWarehouseSend(*lpObj);

	if (bCanWarehouseLock == TRUE)
	{
		if (lpObj->WarehousePW > 0)
		{
			if (lpObj->WarehouseLock == 0)
			{
				GSProtocol.GCWarehouseStateSend(lpObj, 12);
			}
			else
			{
				GSProtocol.GCWarehouseStateSend(lpObj, 1);
			}
		}
		else
		{
			GSProtocol.GCWarehouseStateSend(lpObj, 0);
		}
	}

	lpObj->LoadWareHouseInfo = true;
	lpObj->m_PlayerData->m_ReqWarehouseOpen = false;
	gObjRequestPetItemInfo(lpObj, 1);
	bool bCheck = false;
	Check_SameSerialItem(*lpObj, 1, bCheck);
}

// UPTO

void GDGetWarehouseNoItem(SDHP_GETWAREHOUSEDB_RESULT * lpMsg)
{
	char szId[11];
	CGameObject* lpObj = lpMsg->aIndex;
	PMSG_TALKRESULT pResult;

	szId[MAX_ACCOUNT_LEN] = 0;
	memcpy(szId, lpMsg->AccountID, sizeof(lpMsg->AccountID));

	if (!gObjIsAccontConnect(lpObj, szId))
	{
		sLog->outError("Error-L1 : Request to receive Warehouse information doesn't match the user [%s][%d]", szId, aIndex);
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->m_IfState.type == 6)
	{
		if (lpObj->m_IfState.state == 0)
		{
			lpObj->m_IfState.state = 1;
			pResult.h.c = 0xC3;
			pResult.h.headcode = 0x30;
			pResult.h.size = sizeof(pResult);
			pResult.result = 2;

			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index,(LPBYTE)&pResult, pResult.h.size);
			sLog->outError("Warehouse item not found [%s]", lpObj->Name);

			PMSG_MONEY pMsg; // Season 4.5 fix

			PHeadSetB((LPBYTE)&pMsg, 0x81, sizeof(pMsg));
			pMsg.Result = 1;
			pMsg.iMoney = lpObj->m_PlayerData->Money;
			pMsg.wMoney = lpObj->WarehouseMoney;

			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		}
	}

	lpObj->LoadWareHouseInfo = true;
	lpObj->m_PlayerData->m_ReqWarehouseOpen = false;
	lpObj->m_btOpenWarehouse = TRUE;
}

void GDSetWarehouseList(CGameObject* lpObj, BOOL bCloseWindow)
{
	SDHP_GETWAREHOUSEDB_SAVE pMsg;

	if (lpObj->LoadWareHouseInfo == false)
	{
		sLog->outBasic("[%s][%s] WareHouse Save Fail : Not Open",
			lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);

		return;
	}

	memcpy(pMsg.AccountID, lpObj->m_PlayerData->ConnectUser->AccountID, sizeof(pMsg.AccountID));
	pMsg.aIndex = aIndex;
	pMsg.h.c = 0xC2;
	pMsg.h.headcode = 0x09;
	pMsg.h.sizeH = HIBYTE(sizeof(pMsg));
	pMsg.h.sizeL = LOBYTE(sizeof(pMsg));
	pMsg.Money = lpObj->WarehouseMoney;
	pMsg.pw = lpObj->WarehousePW;
	pMsg.WarehouseID = lpObj->WarehouseID;
	pMsg.CloseWindow = bCloseWindow;

	ItemByteConvert32(pMsg.dbItems, lpObj->pWarehouse, WAREHOUSE_SIZE);

	wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg));

	lpObj->WarehouseSaveLock = TRUE;

	gObjSavePetItemInfo(lpObj, 1);
	gObjWarehouseTextSave(&gObj[aIndex]);
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu DB Char Item Save Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC2
 *	HeadCode  : 0x11
 */
struct SDHP_DBCHAR_ITEMSAVE
{
	PWMSG_HEAD h;	// C2:11
	char Name[11];	// 4
	BYTE dbInventory[7584];	// 34
};

void GDUserItemSave(LPOBJ lpObj)
{
	SDHP_DBCHAR_ITEMSAVE pMsg;

	pMsg.h.c = 0xC2;
	pMsg.h.headcode = 0x11;
	pMsg.h.sizeH = HIBYTE(sizeof(pMsg));
	pMsg.h.sizeL = LOBYTE(sizeof(pMsg));
	memcpy(pMsg.Name, lpObj->Name, sizeof(pMsg.Name));
	ItemByteConvert32(pMsg.dbInventory, lpObj->pInventory, INVENTORY_SIZE);

	wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg));
}



/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Warehouse Money Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 0x12
 */
struct SDHP_WAREHOUSEMONEY_SAVE
{
	PBMSG_HEAD h;	// C1:12
	char AccountID[11];	// 3
	short aIndex;	// E
	int Money;	// 10
};

void GDSetWarehouseMoney(CGameObject* lpObj)
{
	SDHP_WAREHOUSEMONEY_SAVE pMsg;

	memcpy(pMsg.AccountID, lpObj->m_PlayerData->ConnectUser->AccountID, sizeof(pMsg.AccountID));
	pMsg.aIndex = aIndex;
	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x12;
	pMsg.h.size = sizeof(pMsg);
	pMsg.Money = lpObj->WarehouseMoney;

	wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg));
}

void GDGameServerInfoSave()
{
	return;		// GHere goes a MACRO
}



/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Item Serial Create Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 0x52, 0x55 (in Case of PET )
 */
struct SDHP_ITEMCREATE
{
	PBMSG_HEAD h;	// C1:XX
	BYTE x;	// 3
	BYTE y;	// 4
	BYTE MapNumber;	// 5 [235:Mu_2nd_Aniv], [236:CashShop]
	short Type;	// 6
	BYTE Level;	// 8
	BYTE Dur;	// 9
	BYTE Op1;	// A
	BYTE Op2;	// B
	BYTE Op3;	// C
	BYTE NewOption;	// D
	CGameObject* lpObj;	// 10
	int lootindex;	// 14
	BYTE SetOption;	// 16
	time_t lDuration;
	BYTE SocketOption[5];
	BYTE MainAttribute;
};

// MapNumber [235:Mu_2nd_Aniv], [236:CashShop]
void ItemCreate(CGameObject* lpObj, BYTE MapNumber, BYTE x, BYTE y, int type, BYTE level, BYTE dur, BYTE Op1, BYTE Op2, BYTE Op3, int LootIndex, BYTE NewOption, BYTE SetOption, time_t lDuration, BYTE *SocketOption, BYTE MainAttribute)
{
	SDHP_ITEMCREATE  pMsg;
	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x52;
	pMsg.h.size = sizeof(pMsg);
	pMsg.MapNumber = MapNumber;
	pMsg.x = x;
	pMsg.y = y;
	pMsg.Type = type;
	pMsg.Level = level;

#if (ENABLE_CUSTOM_ITEMDROPEX == 1)
	if (g_DropEx.IsBlockItem(type))
	{
		if (MapNumber >= 0 && MapNumber <= MAX_NUMBER_MAP)
		{
			return;
		}
	}
#endif

	if (dur == 0)
	{
		dur = ItemGetDurability(type, level, NewOption, SetOption);
	}

	if (IsItem(type) == 0)
	{
		return;
	}

	pMsg.Dur = dur;
	pMsg.Op1 = Op1;
	pMsg.Op2 = Op2;
	pMsg.Op3 = Op3;
	pMsg.NewOption = NewOption;
	pMsg.aIndex = aIndex;
	pMsg.lootindex = LootIndex;
	pMsg.SetOption = SetOption;
	pMsg.lDuration = lDuration;

	if (SocketOption)
	{
		for (int i = 0; i < MAX_SOCKET_OPTION; i++)
		{
			pMsg.SocketOption[i] = SocketOption[i];
		}
	}

	else
	{
		for (int i = 0; i < MAX_SOCKET_OPTION; i++)
		{
			pMsg.SocketOption[i] = 0xFF;
		}
	}

	pMsg.MainAttribute = MainAttribute;

	if (LootIndex != -1 && gObj[LootIndex].Type == OBJ_USER)
	{
		if (g_ConfigRead.data.common.joinmuDropItemUnderCharacter[gObj[LootIndex].Class] == true)
		{
			pMsg.x = gObj[LootIndex].X;
			pMsg.y = gObj[LootIndex].Y;
		}
	}

	if (ObjectMaxRange(lpObj) != FALSE  && lpObj->Type != OBJ_USER)
	{
		BYTE NewOption[MAX_EXOPTION_SIZE];

		for (int i = 0; i < MAX_EXOPTION_SIZE; i++)
		{
			NewOption[i] = 0;
		}

		if ((pMsg.NewOption & 0x20) != 0)
		{
			NewOption[0] = TRUE;
		}
		if ((pMsg.NewOption & 0x10) != 0)
		{
			NewOption[1] = TRUE;
		}

		if ((pMsg.NewOption & 0x8) != 0)
		{
			NewOption[2] = TRUE;
		}
		if ((pMsg.NewOption & 0x4) != 0)
		{
			NewOption[3] = TRUE;
		}
		if ((pMsg.NewOption & 0x2) != 0)
		{
			NewOption[4] = TRUE;
		}
		if ((pMsg.NewOption & 0x1) != 0)
		{
			NewOption[5] = TRUE;
		}

	}

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

void PetItemSerialCreateSend(CGameObject* lpObj, BYTE MapNumber, BYTE x, BYTE y, int type, BYTE level, BYTE dur, BYTE Op1, BYTE Op2, BYTE Op3, int LootIndex, BYTE NewOption, BYTE SetOption)
{
	SDHP_ITEMCREATE  pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x55;
	pMsg.h.size = sizeof(pMsg);
	pMsg.MapNumber = MapNumber;
	pMsg.x = x;
	pMsg.y = y;
	pMsg.Type = type;
	pMsg.Level = level;

	if (dur == 0)
	{
		dur = ItemGetDurability(type, level, NewOption, SetOption);
	}

	pMsg.Dur = dur;
	pMsg.Op1 = Op1;
	pMsg.Op2 = Op2;
	pMsg.Op3 = Op3;
	pMsg.NewOption = NewOption;
	pMsg.aIndex = aIndex;
	pMsg.lootindex = LootIndex;
	pMsg.SetOption = SetOption;
	pMsg.lDuration = 0;
	pMsg.MainAttribute = 0;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

void ItemSerialCreateRecv(SDHP_ITEMCREATERECV * lpMsg)
{
	LPOBJ lpObj = &gObj[lpMsg->aIndex];
	int mapnumber = lpMsg->MapNumber;
	CGameObject* lpObj = lpMsg->aIndex;
	int lootindex = lpMsg->lootindex;

	if (mapnumber != (BYTE)-1)
	{
		if (mapnumber < 0 || (mapnumber > MAX_NUMBER_MAP - 1 && mapnumber < 217))
		{
			return;
		}
	}

	if (lpMsg->MapNumber == (BYTE)-1 || lpMsg->MapNumber == (BYTE)-2)
	{
		if (gObjIsConnectedGP(lpObj) == FALSE)
		{
			return;
		}

		lpObj = &gObj[aIndex];

		if (lpMsg->MapNumber == (BYTE)-2)
		{
			if (lpObj->m_IfState.type != 13)
			{
				return;
			}
		}
		else if (lpObj->m_IfState.type != 7)
		{
			return;
		}

		PMSG_CHAOSMIXRESULT pMsg;

		pMsg.h.c = 0xC1;
		pMsg.h.headcode = 0x86;
		pMsg.h.size = sizeof(PMSG_CHAOSMIXRESULT);

		if (lpMsg->MapNumber == (BYTE)-2)
		{
			pMsg.Result = 100;
		}
		else
		{
			pMsg.Result = 1;
		}

		CItem NewItem;

		NewItem.m_Type = lpMsg->Type;
		NewItem.m_Level = lpMsg->Level;
		NewItem.m_Durability = ItemGetDurability(lpMsg->Type, lpMsg->Level, lpMsg->NewOption, lpMsg->SetOption);

		if (lpMsg->Type == ITEMGET(14, 7)) // Siege Potion
		{
			NewItem.m_Durability = lpMsg->Dur;
		}

		if (lpMsg->Type == ITEMGET(13, 37))
		{
			NewItem.m_Durability = lpMsg->Dur;
		}

		BYTE SocketOption[5];
		int SocketSlotCount = 0;
		int SocketBonusOption = 0;

		if (gSocketItemType.CheckSocketItemType((int)&NewItem) == true)
		{
			SocketSlotCount = g_SocketOptionSystem.GetMakeSocketSlotCount(lpMsg->Type);
			SocketBonusOption = -1;

			for (int i = 0; i < 5; i++)
			{
				if (i < SocketSlotCount)
				{
					SocketOption[i] = 0xFE;
				}
				else
				{
					SocketOption[i] = 0xFF;
				}
			}
		}

		else
		{
			SocketBonusOption = 0;

			if (memcmp(lpMsg->SocketOption, SocketOption, 5) == 0)
			{
				for (int i = 0; i < 5; i++)
				{
					SocketOption[i] = 0xFF;
				}
			}

			else
			{
				for (int i = 0; i < 5; i++)
				{
					SocketOption[i] = 0xFF;
				}
			}
		}

		NewItem.Convert(lpMsg->Type, lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, SocketOption, SocketBonusOption, 0, CURRENT_DB_VERSION);
		ItemByteConvert(pMsg.ItemInfo, NewItem);
		NewItem.m_Number = lpMsg->m_Number;

		if (lpObj->ChaosMassMixCurrItem == 0)
		{
			g_MixSystem.ChaosBoxInit(lpObj);
		}

		BYTE btPos = gObjChaosBoxInsertItemTemp(lpObj, &NewItem);

		if (btPos == 0xFF)
		{
			pMsg.Result = CB_ERROR;

			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
			lpObj->ChaosLock = FALSE;

			return;
		}
		;
		gObjChaosBoxInsertItemPos(lpObj, NewItem, btPos, -1);
		gObjChaosItemSet(lpObj, btPos, 1);
		BYTE ExOption[MAX_EXOPTION_SIZE];
		ItemIsBufExOption(ExOption, &NewItem);

		pMsg.Pos = btPos;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);

		lpObj->ChaosMassMixCurrItem++;

		if (lpObj->ChaosMassMixCurrItem >= lpObj->ChaosMassMixSuccessCount)
		{
			lpObj->ChaosMassMixCurrItem = 0;
			lpObj->ChaosMassMixSuccessCount = 0;

			GJSetCharacterInfo(lpObj, aIndex, 0);
			lpObj->ChaosLock = FALSE;
		}

		return;
	}

	if (lpMsg->MapNumber == 230)
	{
		CItem Item;
		int iType = ITEM_GET_TYPE(lpMsg->Type);
		int iTypeIndex = ITEM_GET_INDEX(lpMsg->Type);
		Item.m_Level = lpMsg->Level;
		Item.m_Durability = lpMsg->Dur;
		Item.m_Number = lpMsg->m_Number;
		Item.m_Type = lpMsg->Type;

		Item.Convert(ITEMGET(iType, iTypeIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, lpMsg->MainAttribute, 0, CURRENT_DB_VERSION);

		PMSG_CHAOSMIXRESULT pMsg;
		PHeadSetB((LPBYTE)&pMsg, 0x86, sizeof(pMsg));
		pMsg.Result = CB_ERROR;
		ItemByteConvert(pMsg.ItemInfo, Item);

		BYTE btPos = gObjPentagramMixBoxInsertItem(lpObj, Item);

		if (btPos == 0xFF)
		{
			pMsg.Result = CB_ERROR;
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
			lpObj->m_PlayerData->m_PentagramMixLock = FALSE;
		}

		else
		{
			pMsg.Result = CB_SUCCESS;
			gObjPentagramMixItemSet(lpObj, btPos, 1);
			BYTE ExOption[MAX_EXOPTION_SIZE];
			ItemIsBufExOption(ExOption, &Item);

			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
			lpObj->m_PlayerData->m_PentagramMixLock = FALSE;
		}
	}

	else if (lpMsg->MapNumber == 231)
	{
		CItem Item;
		int iType = ITEM_GET_TYPE(lpMsg->Type);
		int iTypeIndex = ITEM_GET_INDEX(lpMsg->Type);
		Item.m_Level = lpMsg->Level;
		Item.m_Durability = lpMsg->Dur;
		Item.m_Number = lpMsg->m_Number;
		Item.m_Type = lpMsg->Type;

		Item.Convert(ITEMGET(iType, iTypeIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, lpMsg->MainAttribute, 0, CURRENT_DB_VERSION);
		BYTE btInOutResult = FALSE;

		BYTE btRes = gObjInventoryInsertItem(lpObj, Item);

		if (btRes == 0xFF)
		{
			GSProtocol.GCAnsPentagramJewelInOut(lpObj, btInOutResult);
		}
		else
		{
			GSProtocol.GCInventoryItemOneSend(lpObj, btRes);
			GSProtocol.GCAnsPentagramJewelInOut(lpObj, 1);
			BYTE ExOption[MAX_EXOPTION_SIZE];
			ItemIsBufExOption(ExOption, &Item);

		}
	}

	else if (lpMsg->MapNumber == 233)
	{
		if (gObj[lpMsg->aIndex].Connected > PLAYER_CONNECTED)
		{
			CItem Item;
			int iType = ITEM_GET_TYPE(lpMsg->Type);
			int iTypeIndex = ITEM_GET_INDEX(lpMsg->Type);
			Item.m_Level = lpMsg->Level;
			Item.m_Durability = lpMsg->Dur;
			Item.m_Number = lpMsg->m_Number;
			Item.m_Type = lpMsg->Type;

			Item.Convert(ITEMGET(iType, iTypeIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, lpMsg->MainAttribute, 0, CURRENT_DB_VERSION);

			BYTE iItemPos = gObjInventoryInsertItem(lpMsg->aIndex, Item);

			if (iItemPos == (BYTE)-1)
			{

			}
			else
			{
				::GSProtocol.GCInventoryItemOneSend(lpMsg->aIndex, iItemPos);
				GSProtocol.GCServerMsgStringSendEx(lpMsg->aIndex, 1, Lang.GetText(0, 535), ItemAttribute[Item.m_Type].Name);
			}
		}
	}

	else if (lpMsg->MapNumber == 234)
	{
		if (gObj[lpMsg->aIndex].Connected > PLAYER_CONNECTED)
		{
			CItem Item;
			int iType = ITEM_GET_TYPE(lpMsg->Type);
			int iTypeIndex = ITEM_GET_INDEX(lpMsg->Type);
			Item.m_Level = lpMsg->Level;
			Item.m_Number = lpMsg->m_Number;
			Item.m_Type = lpMsg->Type;

			Item.Convert(ITEMGET(iType, iTypeIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, lpMsg->MainAttribute, 0, CURRENT_DB_VERSION);

			Item.m_Durability = ItemGetDurability(ITEMGET(iType, iTypeIndex), Item.m_Level, Item.m_NewOption, Item.m_SetOption);

			BYTE iItemPos = gObjInventoryInsertItem(lpMsg->aIndex, Item);

			if (iItemPos == (BYTE)-1)
			{

				GSProtocol.GCAnsLuckyCoinTrade(lpMsg->aIndex, 2);
			}
			else
			{
				if (lpMsg->lDuration > 0)
				{
					g_PeriodItemEx.SetPeriodItemInfo(lpObj, lpMsg->Type, lpMsg->m_Number, lpMsg->lDuration);
				}

				GSProtocol.GCInventoryItemOneSend(lpMsg->aIndex, iItemPos);

				if (lpMsg->lDuration > 0)
				{
					g_PeriodItemEx.SendPeriodItemInfoOnce(lpObj, &Item);
				}

				GSProtocol.GCAnsLuckyCoinTrade(lpMsg->aIndex, 1);
			}

			GJSetCharacterInfo(lpObj, aIndex, FALSE);
		}
	}

	else if (lpMsg->MapNumber == 235)
	{
		if (gObj[lpMsg->aIndex].Connected > PLAYER_CONNECTED)
		{
			CItem Item;
			int iType = ITEM_GET_TYPE(lpMsg->Type);
			int iTypeIndex = ITEM_GET_INDEX(lpMsg->Type);
			Item.m_Level = lpMsg->Level;
			Item.m_Durability = lpMsg->Dur;
			Item.m_Number = lpMsg->m_Number;
			Item.m_Type = lpMsg->Type;

			Item.Convert(ITEMGET(iType, iTypeIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, lpMsg->MainAttribute, 0, CURRENT_DB_VERSION);

			BYTE iItemPos = gObjInventoryInsertItem(lpMsg->aIndex, Item);

			if (iItemPos == (BYTE)-1)
			{

			}
			else
			{
				if (lpMsg->lDuration > 0)
				{
					g_PeriodItemEx.SetPeriodItemInfo(lpObj, lpMsg->Type, lpMsg->m_Number, lpMsg->lDuration);
				}

				GSProtocol.GCInventoryItemOneSend(lpMsg->aIndex, iItemPos);

				if (lpMsg->lDuration > 0)
				{
					g_PeriodItemEx.SendPeriodItemInfoOnce(lpObj, &Item);
				}
			}

			GJSetCharacterInfo(lpObj, aIndex, FALSE);
		}
	}

	else if (lpMsg->MapNumber == 236)
	{
		if (gObj[lpMsg->aIndex].Connected > PLAYER_CONNECTED)
		{
			CItem pCreateItem;
			time_t curtime = time(NULL);
			int iItemType = ITEM_GET_TYPE(lpMsg->Type);
			int iItemIndex = ITEM_GET_INDEX(lpMsg->Type);

			pCreateItem.m_Type = lpMsg->Type;

			pCreateItem.Convert(ITEMGET(iItemType, iItemIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, lpMsg->MainAttribute, 0, CURRENT_DB_VERSION);
			pCreateItem.m_Level = lpMsg->Level;
			pCreateItem.m_Durability = 255.0;
			pCreateItem.m_Number = lpMsg->m_Number;

			BYTE btItemPos = gObjInventoryInsertItem(lpMsg->aIndex, pCreateItem);

			if (btItemPos == (BYTE)-1)
			{

			}
			else
			{
				if (lpMsg->lDuration > 0)
				{
					g_PeriodItemEx.SetPeriodItemInfo(lpObj, lpMsg->Type, lpMsg->m_Number, lpMsg->lDuration);
				}

				GSProtocol.GCInventoryItemOneSend(lpMsg->aIndex, btItemPos);

				if (lpMsg->lDuration > 0)
				{
					g_PeriodItemEx.SendPeriodItemInfoOnce(lpObj, &pCreateItem);
				}
			}
		}
	}

	else if (lpMsg->MapNumber == 229)
	{
		if (gObj[lpMsg->aIndex].Connected > PLAYER_LOGGED)
		{
			PMSG_BUYRESULT pResult;
			CItem pCreateItem;
			int iItemType = ITEM_GET_TYPE(lpMsg->Type);
			int iItemIndex = ITEM_GET_INDEX(lpMsg->Type);

			pCreateItem.m_Level = lpMsg->Level;
			pCreateItem.m_Durability = lpMsg->Dur;
			pCreateItem.m_Number = lpMsg->m_Number;
			pCreateItem.Convert(ITEMGET(iItemType, iItemIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, lpMsg->MainAttribute, 0, CURRENT_DB_VERSION);
			pCreateItem.m_Level = lpMsg->Level;

			BOOL bIsBlank = gObjGamblingInventoryCheck(lpMsg->aIndex, 2, 4);
			if (bIsBlank == TRUE)
			{
				BYTE btItemPos = gObjInventoryInsertItem(lpMsg->aIndex, pCreateItem);
				if (btItemPos == 0xFF)
				{
					PHeadSetB((LPBYTE)&pResult, 0x32, sizeof(pResult));
					pResult.Result = 0xFF;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
					return;
				}

				int iBuyTaxMoney = (g_GamblingItemBag.GetGamblingValue() * (__int64)g_CastleSiegeSync.GetTaxRateChaos(gObj[lpMsg->aIndex].m_Index) / (__int64)100);
				g_CastleSiegeSync.AddTributeMoney(iBuyTaxMoney);

				gObj[lpMsg->aIndex].m_PlayerData->Money -= iBuyTaxMoney + g_GamblingItemBag.GetGamblingValue();

				if (gObj[lpMsg->aIndex].m_PlayerData->Money < 0)
					gObj[lpMsg->aIndex].m_PlayerData->Money = 0;

				GSProtocol.GCMoneySend(lpObj, gObj[lpMsg->aIndex].m_PlayerData->Money);
				GSProtocol.GCInventoryItemOneSend(lpMsg->aIndex, btItemPos);

				BYTE ExOption[6];
				ItemIsBufExOption(ExOption, &pCreateItem);

				if (lpMsg->Type == ITEMGET(3, 11) ||
					lpMsg->Type == ITEMGET(5, 33) ||
					lpMsg->Type == ITEMGET(4, 24) ||
					lpMsg->Type == ITEMGET(2, 18) ||
					lpMsg->Type == ITEMGET(5, 34))
				{
					char szBuffer[256] = { 0 };
					sprintf(szBuffer, Lang.GetText(0, 589), pCreateItem.GetName());

					PMSG_NOTICE pNotice;
					TNotice::MakeNoticeMsg(&pNotice, 0, szBuffer);
					TNotice::SendNoticeToAllUser(&pNotice);

					PMSG_SERVERCMD ServerCmd;
					PHeadSubSetB((LPBYTE)&ServerCmd, 0xF3, 0x40, sizeof(ServerCmd));
					ServerCmd.CmdType = 0;
					ServerCmd.X = gObj[lpMsg->aIndex].X;
					ServerCmd.Y = gObj[lpMsg->aIndex].Y;
					GSProtocol.MsgSendV2(&gObj[lpMsg->aIndex], (LPBYTE)&ServerCmd, sizeof(ServerCmd));
					IOCP.DataSend(lpMsg->aIndex, (LPBYTE)&ServerCmd, sizeof(ServerCmd));

				}
				else
				{

				}
			}
			else
			{
				GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 536), lpMsg->aIndex, 1);

				PHeadSetB((LPBYTE)&pResult, 0x32, sizeof(pResult));
				pResult.Result = 0xFF;
				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
				lpObj->ChaosLock = FALSE;
				return;
			}
		}
	}

	else if (lpMsg->MapNumber == 228)
	{
		if (gObj[lpMsg->aIndex].Connected > PLAYER_CONNECTED)
		{
			CItem Item;
			int iType = ITEM_GET_TYPE(lpMsg->Type);
			int iTypeIndex = ITEM_GET_INDEX(lpMsg->Type);
			Item.m_Level = lpMsg->Level;
			Item.m_Number = lpMsg->m_Number;
			Item.m_Type = lpMsg->Type;

			Item.Convert(ITEMGET(iType, iTypeIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, lpMsg->MainAttribute, 0, CURRENT_DB_VERSION);

			Item.m_Durability = ItemGetDurability(ITEMGET(iType, iTypeIndex), Item.m_Level, Item.m_NewOption, Item.m_SetOption);

			BYTE iItemPos = gObjInventoryInsertItem(lpMsg->aIndex, Item);

			if (iItemPos == (BYTE)-1)
			{

			}
			else
			{
				GSProtocol.GCInventoryItemOneSend(lpMsg->aIndex, iItemPos);
			}
		}
	}

	else if (lpMsg->MapNumber == 227)
	{
		CItem Item;
		int iType = ITEM_GET_TYPE(lpMsg->Type);
		int iTypeIndex = ITEM_GET_INDEX(lpMsg->Type);
		Item.m_Level = lpMsg->Level;
		Item.m_Durability = lpMsg->Dur;
		Item.m_Number = lpMsg->m_Number;
		Item.m_Type = lpMsg->Type;
		if (g_CMuunSystem.IsMuunItem(lpMsg->Type) == TRUE)
		{
			int nMuunRank = g_CMuunSystem.GetMuunRankOfMuunInfo(lpMsg->Type);
			BYTE SocketOptionIndex = 0;
			SocketOptionIndex |= nMuunRank;

			Item.Convert(ITEMGET(iType, iTypeIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, SocketOptionIndex, 0, CURRENT_DB_VERSION);

		}
		else
		{
			Item.Convert(ITEMGET(iType, iTypeIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, lpMsg->MainAttribute, 0, CURRENT_DB_VERSION);

		}

		int iItemCount = 0;

		if (g_CMuunSystem.IsMuunItem(lpMsg->Type) == TRUE)
		{
			int nMuunRank = g_CMuunSystem.GetMuunRankOfMuunInfo(lpMsg->Type);
			int iItemCount = MapC[gObj[lpMsg->aIndex].MapNumber].MonsterItemDrop(lpMsg->Type, lpMsg->Level, lpMsg->Dur, lpMsg->x,
				lpMsg->y, lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption,
				lootindex, lpMsg->m_Number, 0, lpMsg->SocketOption, nMuunRank, lpMsg->lDuration);
		}
		else
		{
			int iItemCount = MapC[gObj[lpMsg->aIndex].MapNumber].MonsterItemDrop(lpMsg->Type, lpMsg->Level, lpMsg->Dur, lpMsg->x,
				lpMsg->y, lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption,
				lootindex, lpMsg->m_Number, 0, lpMsg->SocketOption, lpMsg->MainAttribute, lpMsg->lDuration);
		}

		BYTE NewOption[MAX_EXOPTION_SIZE];

		for (int i = 0; i < MAX_EXOPTION_SIZE; i++)
		{
			NewOption[i] = FALSE;
		}

		if ((lpMsg->NewOption & 0x20) != 0)
		{
			NewOption[0] = TRUE;
		}
		if ((lpMsg->NewOption & 0x10) != 0)
		{
			NewOption[1] = TRUE;
		}

		if ((lpMsg->NewOption & 0x8) != 0)
		{
			NewOption[2] = TRUE;
		}
		if ((lpMsg->NewOption & 0x4) != 0)
		{
			NewOption[3] = TRUE;
		}

		if ((lpMsg->NewOption & 0x2) != 0)
		{
			NewOption[4] = TRUE;
		}
		if ((lpMsg->NewOption & 0x1) != 0)
		{
			NewOption[5] = TRUE;
		}

	}

	else if (lpMsg->MapNumber == 226)
	{
		if (gObj[lpMsg->aIndex].Connected > 1)
		{
			CItem Item;
			int iType = ITEM_GET_TYPE(lpMsg->Type);
			int iTypeIndex = ITEM_GET_INDEX(lpMsg->Type);
			Item.m_Level = lpMsg->Level;
			Item.m_Durability = lpMsg->Dur;
			Item.m_Number = lpMsg->m_Number;
			Item.m_Type = lpMsg->Type;

			Item.Convert(ITEMGET(iType, iTypeIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, lpMsg->MainAttribute, 0, CURRENT_DB_VERSION);

			BYTE btItemPos = gObjEventInventoryInsertItem(lpMsg->aIndex, Item);

			if (btItemPos == (BYTE)-1)
			{

			}

			else
			{
				GSProtocol.GCEventInvenItemOneSend(lpMsg->aIndex, btItemPos);
			}
		}
	}

	else if (lpMsg->MapNumber == 225)
	{
		if (gObj[lpMsg->aIndex].Connected > 1)
		{
			LPOBJ lpObj = &gObj[lpMsg->aIndex];
			CMuRummyInfo * pCMuRummyInfo = lpObj->m_PlayerData->m_pCMuRummyInfo;

			if (!pCMuRummyInfo)
			{
				return;
			}

			CItem Item;
			int iType = ITEM_GET_TYPE(lpMsg->Type);
			int iTypeIndex = ITEM_GET_INDEX(lpMsg->Type);
			Item.m_Level = lpMsg->Level;
			Item.m_Durability = lpMsg->Dur;
			Item.m_Number = lpMsg->m_Number;
			Item.m_Type = lpMsg->Type;

			if (g_PentagramSystem.IsPentagramItem(lpMsg->Type) == TRUE)
			{
				BYTE btEnableSlot[5];
				BYTE btSocketSlotCount[5];
				BYTE btSlotCount = g_PentagramSystem.GetMakePentagramSlotCountNKind(btEnableSlot, lpMsg->Type);

				for (int i = 0; i < 5; i++)
				{
					if (btEnableSlot[i])
					{
						btSocketSlotCount[i] = 0xFE;
					}

					else
					{
						btSocketSlotCount[i] = 0xFF;
					}
				}

				Item.Convert(ITEMGET(iType, iTypeIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, btSocketSlotCount, lpMsg->MainAttribute, 0, CURRENT_DB_VERSION);
			}

			else
			{
				Item.Convert(ITEMGET(iType, iTypeIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, lpMsg->MainAttribute, 0, CURRENT_DB_VERSION);
			}

			BYTE btItemPos = gObjInventoryInsertItem(lpMsg->aIndex, Item);

			if (btItemPos == (BYTE)-1)
			{
				pCMuRummyInfo->SetWaitReward(0);
				g_CMuRummyMng.GCSendMsg(lpMsg->aIndex, 7, 0);

			}

			else
			{
				PMSG_ANS_MURUMMY_END pMsg;

				PHeadSubSetB((LPBYTE)&pMsg, 0x4D, 0x15, sizeof(pMsg));
				pMsg.btResult = TRUE;
				IOCP.DataSend(lpMsg->aIndex, (LPBYTE)&pMsg, pMsg.h.size);

				g_CMuRummyMng.GCSendMsg(lpMsg->aIndex, 9, ITEMGET(iType, iTypeIndex));

				if (lpMsg->lDuration > 0)
				{
					g_PeriodItemEx.SetPeriodItemInfo(lpObj, lpMsg->Type, lpMsg->m_Number, lpMsg->lDuration);
				}

				GSProtocol.GCInventoryItemOneSend(lpMsg->aIndex, btItemPos);

				if (lpMsg->lDuration > 0)
				{
					g_PeriodItemEx.SendPeriodItemInfoOnce(lpObj, &Item);
				}

				g_CMuRummyMng.GCSendMsg(lpMsg->aIndex, 8, 0);

				BYTE NewOption[MAX_EXOPTION_SIZE];
				ItemIsBufExOption(NewOption, &Item);

				g_CMuRummyMng.GDReqMuRummyDBLog(lpObj, pCMuRummyInfo->GetScore());
				pCMuRummyInfo->Clear();
				g_CMuRummyMng.GDReqScoreDelete(lpObj);
			}
		}
	}

	else if (lpMsg->MapNumber == 224)
	{
		if (gObj[lpMsg->aIndex].Connected > 1)
		{
			LPOBJ lpObj = &gObj[lpMsg->aIndex];

			CItem MuunItem;
			int ItemType = ITEM_GET_TYPE(lpMsg->Type);
			int ItemIndex = ITEM_GET_INDEX(lpMsg->Type);
			int nMuunRank = g_CMuunSystem.GetMuunRankOfMuunInfo(lpMsg->Type);
			BYTE SocketOption[5];
			int SocketSlotCount = 0;

			for (int k = 0; k < 5; k++)
			{
				SocketOption[k] = lpMsg->SocketOption[k];
			}

			MuunItem.m_Level = lpMsg->Level;
			MuunItem.m_Durability = lpMsg->Dur;
			MuunItem.m_Number = lpMsg->m_Number;
			MuunItem.Convert(lpMsg->Type, lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, SocketOption, nMuunRank, 0, 3);

			if (g_CMuunSystem.IsMuunExpireDate(lpMsg->Type) == TRUE)
			{
				MuunItem.SetMuunItemPeriodExpire();
			}

			BYTE btItemPos = gObjMuunInventoryInsertItem(lpMsg->aIndex, MuunItem);

			if (btItemPos == (BYTE)-1)
			{

			}
			else
			{
				GSProtocol.GCMuunInventoryItemOneSend(lpMsg->aIndex, btItemPos);
			}
		}
	}

	else if (lpMsg->MapNumber == 223)
	{
		if (gObj[lpMsg->aIndex].Connected > PLAYER_CONNECTED)
		{
			CItem Item;
			int iType = ITEM_GET_TYPE(lpMsg->Type);
			int iTypeIndex = ITEM_GET_INDEX(lpMsg->Type);
			Item.m_Level = lpMsg->Level;
			Item.m_Durability = lpMsg->Dur;
			Item.m_Number = lpMsg->m_Number;
			Item.m_Type = lpMsg->Type;

			Item.Convert(ITEMGET(iType, iTypeIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, 0, -1, 0, CURRENT_DB_VERSION);

			BYTE iItemPos = gObjInventoryInsertItem(lpMsg->aIndex, Item);

			if (iItemPos == (BYTE)-1)
			{

			}
			else
			{
				::GSProtocol.GCInventoryItemOneSend(lpMsg->aIndex, iItemPos);
			}

		}
	}

	else if (lpMsg->MapNumber == 222)
	{
		if (gObj[lpMsg->aIndex].Connected > PLAYER_CONNECTED)
		{
			CItem Item;
			int iType = ITEM_GET_TYPE(lpMsg->Type);
			int iTypeIndex = ITEM_GET_INDEX(lpMsg->Type);
			Item.m_Level = lpMsg->Level;
			Item.m_Durability = lpMsg->Dur;
			Item.m_Number = lpMsg->m_Number;
			Item.m_Type = lpMsg->Type;

			Item.Convert(ITEMGET(iType, iTypeIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, lpMsg->MainAttribute, 0, CURRENT_DB_VERSION);

			BYTE iItemPos = gObjInventoryInsertItem(lpMsg->aIndex, Item);

			PMSG_BUYRESULT pResult;
			PHeadSetB((LPBYTE)&pResult, 0x32, sizeof(pResult));

			if (iItemPos == (BYTE)-1)
			{
				pResult.Result = -1;
			}
			else
			{
				pResult.Result = iItemPos;
				ItemByteConvert(pResult.ItemInfo, Item);
			}

			IOCP.DataSend(lpMsg->aIndex, (LPBYTE)&pResult, sizeof(pResult));
		}
	}

	else if (lpMsg->MapNumber == 221)
	{
		if (gObj[lpMsg->aIndex].Connected > PLAYER_CONNECTED)
		{
			CItem Item;

			int iType = ITEM_GET_TYPE(lpMsg->Type);
			int iTypeIndex = ITEM_GET_INDEX(lpMsg->Type);
			Item.m_Level = lpMsg->Level;
			Item.m_Durability = lpMsg->Dur;
			Item.m_Number = lpMsg->m_Number;
			Item.m_Type = lpMsg->Type;

			Item.Convert(ITEMGET(iType, iTypeIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, lpMsg->MainAttribute, 0, CURRENT_DB_VERSION);

			BYTE iItemPos = gObjInventoryInsertItem(lpMsg->aIndex, Item);

			PMSG_BUYRESULT pResult;
			PHeadSetB((LPBYTE)&pResult, 0x32, sizeof(pResult));

			if (iItemPos == (BYTE)-1)
			{
				pResult.Result = -1;
			}
			else
			{
				pResult.Result = iItemPos;
				ItemByteConvert(pResult.ItemInfo, Item);
			}

			IOCP.DataSend(lpMsg->aIndex, (LPBYTE)&pResult, sizeof(pResult));
		}
	}

	else if (lpMsg->MapNumber == 220)
	{
		if (gObj[lpMsg->aIndex].Connected > 1)
		{
			LPOBJ lpObj = &gObj[lpMsg->aIndex];
			CMuRummyInfo * pCMuRummyInfo = lpObj->m_PlayerData->m_pCMuRummyInfo;

			if (!pCMuRummyInfo)
			{
				return;
			}

			CItem MuunItem;
			int ItemType = ITEM_GET_TYPE(lpMsg->Type);
			int ItemIndex = ITEM_GET_INDEX(lpMsg->Type);
			int nMuunRank = g_CMuunSystem.GetMuunRankOfMuunInfo(lpMsg->Type);
			BYTE SocketOption[5];
			int SocketSlotCount = 0;

			for (int k = 0; k < 5; k++)
			{
				SocketOption[k] = lpMsg->SocketOption[k];
			}

			MuunItem.m_Level = lpMsg->Level;
			MuunItem.m_Durability = lpMsg->Dur;
			MuunItem.m_Number = lpMsg->m_Number;
			MuunItem.Convert(lpMsg->Type, lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, SocketOption, nMuunRank, 0, 3);

			if (g_CMuunSystem.IsMuunExpireDate(lpMsg->Type) == TRUE)
			{
				MuunItem.SetMuunItemPeriodExpire();
			}

			BYTE btItemPos = gObjMuunInventoryInsertItem(lpMsg->aIndex, MuunItem);

			if (btItemPos == (BYTE)-1)
			{
				pCMuRummyInfo->SetWaitReward(0);
				g_CMuRummyMng.GCSendMsg(lpMsg->aIndex, 7, 0);

			}

			else
			{
				PMSG_ANS_MURUMMY_END pMsg;

				PHeadSubSetB((LPBYTE)&pMsg, 0x4D, 0x15, sizeof(pMsg));
				pMsg.btResult = TRUE;
				IOCP.DataSend(lpMsg->aIndex, (LPBYTE)&pMsg, pMsg.h.size);

				g_CMuRummyMng.GCSendMsg(lpMsg->aIndex, 9, ITEMGET(ItemType, ItemIndex));
				GSProtocol.GCMuunInventoryItemOneSend(lpMsg->aIndex, btItemPos);
				g_CMuRummyMng.GCSendMsg(lpMsg->aIndex, 8, 0);

				BYTE NewOption[MAX_EXOPTION_SIZE];
				ItemIsBufExOption(NewOption, &MuunItem);

				g_CMuRummyMng.GDReqMuRummyDBLog(lpObj, pCMuRummyInfo->GetScore());
				pCMuRummyInfo->Clear();
				g_CMuRummyMng.GDReqScoreDelete(lpObj);
			}
		}
	}

	else if (lpMsg->MapNumber == 219)
	{
		if (gObj[lpMsg->aIndex].Connected > 1)
		{
			CItem Item;
			int ItemType = ITEM_GET_TYPE(lpMsg->Type);
			int ItemIndex = ITEM_GET_INDEX(lpMsg->Type);
			Item.m_Level = lpMsg->Level;
			Item.m_Durability = lpMsg->Dur;
			Item.m_Number = lpMsg->m_Number;

			Item.Convert(lpMsg->Type, lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, 0, 0, 3);

			BYTE btItemPos = gObjInventoryInsertItem(lpMsg->aIndex, Item);

			if (btItemPos == (BYTE)-1)
			{

			}
			else
			{
				GSProtocol.GCInventoryItemOneSend(lpMsg->aIndex, btItemPos);

				BYTE NewOption[MAX_EXOPTION_SIZE];
				ItemIsBufExOption(NewOption, &Item);

				gObj[lpMsg->aIndex].ChaosLock = FALSE;
			}
		}

		else
		{
			gObj[lpMsg->aIndex].ChaosLock = FALSE;
		}
	}

	else if (lpMsg->MapNumber == 218)
	{
		if (gObj[lpMsg->aIndex].Connected > 1)
		{
			LPOBJ lpObj = &gObj[lpMsg->aIndex];

			int nRank = 0;
			BYTE SocketOption[5];

			if (g_CMuunSystem.IsMuunItem(lpMsg->Type) == TRUE)
			{
				nRank = g_CMuunSystem.GetMuunRankOfMuunInfo(lpMsg->Type);
			}

			else if (g_CMuunSystem.IsStoneofEvolution(lpMsg->Type) == TRUE)
			{
				nRank = 0;
			}

			for (int n = 0; n < 5; n++)
			{
				SocketOption[n] = lpMsg->SocketOption[n];
			}

			CItem Item;
			int ItemType = ITEM_GET_TYPE(lpMsg->Type);
			int ItemIndex = ITEM_GET_INDEX(lpMsg->Type);
			Item.m_Level = lpMsg->Level;
			Item.m_Durability = lpMsg->Dur;
			Item.m_Number = lpMsg->m_Number;

			Item.Convert(lpMsg->Type, lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, SocketOption, nRank, 0, 3);

			BYTE btRet = g_CMuunSystem.DGMuunExchangeInsertInven(lpObj, Item, lpMsg->x);

			if (btRet == 0xFF)
			{

			}
			else
			{
				BYTE NewOption[MAX_EXOPTION_SIZE];
				ItemIsBufExOption(NewOption, &Item);

				if (g_CMuunSystem.IsStoneofEvolution(lpMsg->Type) == TRUE)
				{
					WORD wPetItemNumber = (Item.m_SocketOption[0] << 8) | Item.m_SocketOption[1];
					LPITEM_ATTRIBUTE lpItemAttribute = GetItemAttr(wPetItemNumber);

					if (lpItemAttribute != NULL)
					{

					}
					else
					{

					}
				}
				else
				{

				}
			}
		}
	}

	else if (lpMsg->MapNumber == 217)
	{
		if (gObj[lpMsg->aIndex].Connected > PLAYER_CONNECTED)
		{
			CItem Item;

			int iType = ITEM_GET_TYPE(lpMsg->Type);
			int iTypeIndex = ITEM_GET_INDEX(lpMsg->Type);
			Item.m_Level = lpMsg->Level;
			Item.m_Durability = lpMsg->Dur;
			Item.m_Number = lpMsg->m_Number;
			Item.m_Type = lpMsg->Type;

			Item.Convert(ITEMGET(iType, iTypeIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, lpMsg->MainAttribute, 0, CURRENT_DB_VERSION);

			BYTE iItemPos = gObjInventoryInsertItem(lpMsg->aIndex, Item);

			PMSG_ANS_RUUD_STORE_BUYITEM pMsg;
			PHeadSubSetB((LPBYTE)&pMsg, 0xD0, 0xF0, sizeof(pMsg));

			if (iItemPos == (BYTE)-1)
			{
				pMsg.btResult = 0xFA;
			}
			else
			{
				pMsg.btResult = 0;
				pMsg.btPos = iItemPos;
				ItemByteConvert(pMsg.btItemInfo, Item);

			}
			GSProtocol.GCInventoryItemOneSend(lpObj, iItemPos);
			//IOCP.DataSend(lpMsg->aIndex, (LPBYTE)&pMsg, sizeof(pMsg));
		}
	}

	else
	{
		int iRetMapNumber = lpMsg->MapNumber;

		if (lpMsg->MapNumber >= 238 && lpMsg->MapNumber <= 245)
		{
			mapnumber = g_BloodCastle.GetItemMapNumberFirst(iRetMapNumber);
		}

		if (lpMsg->MapNumber >= 246 && lpMsg->MapNumber <= 253)
		{
			mapnumber = g_BloodCastle.GetItemMapNumberSecond(iRetMapNumber);
		}

		if (mapnumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL && g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
		{
			if (gObj[lpMsg->aIndex].MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
			{
				if (gObjIsConnected(lpMsg->aIndex))
				{
					lpMsg->x = gObj[lpMsg->aIndex].X;
					lpMsg->y = gObj[lpMsg->aIndex].Y;
				}
			}

			BYTE btAttr = MapC[MAP_INDEX_CHAOSCASTLE_SURVIVAL].GetAttr(lpMsg->x, lpMsg->y);

			if ((btAttr & 8) == 8)
			{
				g_ChaosCastleFinal.GetRandomDropPos(lpMsg->x, lpMsg->y);
			}
		}

		CItem Item;

		Item.m_Type = lpMsg->Type;

		BYTE SocketSlotCount = 0;
		BYTE SocketOption[5] = { 0, 0, 0, 0, 0 };
		BYTE SocketOptionIndex = 0;

		if (gSocketItemType.CheckSocketItemType(lpMsg->Type) == 1)
		{
			if (memcmp(lpMsg->SocketOption, SocketOption, 5) != 0)
			{
				for (int i = 0; i < 5; i++)
				{
					if (lpMsg->SocketOption[i] == 0xFE)
					{
						SocketSlotCount++;
					}
				}
			}

			else
			{
				SocketSlotCount = g_SocketOptionSystem.GetMakeSocketSlotCount(lpMsg->Type);
			}

			SocketOptionIndex = 0xFF;

			for (int i = 0; i < 5; i++)
			{
				if (i < SocketSlotCount)
				{
					SocketOption[i] = 0xFE;
				}
				else
				{
					SocketOption[i] = 0xFF;
				}
			}

		}

		else if (g_PentagramSystem.IsPentagramItem(lpMsg->Type) == TRUE)
		{
			BYTE btEnableSlot[5];

			if (memcmp(lpMsg->SocketOption, SocketOption, 5) != 0)
			{
				for (int i = 0; i < 5; i++)
				{
					if (lpMsg->SocketOption[i] == 0xFE)
					{
						btEnableSlot[i] = TRUE;
						SocketSlotCount++;
					}

					else
					{
						btEnableSlot[i] = FALSE;
					}
				}
			}

			else
			{
				SocketSlotCount = g_PentagramSystem.GetMakePentagramSlotCountNKind(btEnableSlot, lpMsg->Type);
			}

			SocketOptionIndex = lpMsg->MainAttribute;

			for (int k = 0; k < 5; k++)
			{
				if (btEnableSlot[k])
				{
					SocketOption[k] = 0xFE;
				}

				else
				{
					SocketOption[k] = 0xFF;
				}
			}
		}

		else if (g_CMuunSystem.IsMuunItem(lpMsg->Type) == TRUE)
		{
			int nMuunRank = g_CMuunSystem.GetMuunRankOfMuunInfo(lpMsg->Type);
			SocketOptionIndex |= nMuunRank;

			for (int k = 0; k < 5; k++)
			{
				SocketOption[k] = lpMsg->SocketOption[k];
			}

		}

		else if (g_CMuunSystem.IsStoneofEvolution(lpMsg->Type) == TRUE)
		{
			for (int k = 0; k < 5; k++)
			{
				SocketOption[k] = lpMsg->SocketOption[k];
			}

		}

		else if (g_PentagramSystem.IsPentagramJewel(lpMsg->Type) == TRUE)
		{
			SocketOptionIndex = lpMsg->MainAttribute;

			for (int l = 0; l < 5; l++)
			{
				SocketOption[l] = lpMsg->SocketOption[l];
			}
		}

		else
		{
			SocketOptionIndex = 0;

			if (memcmp(lpMsg->SocketOption, SocketOption, 5) == 0)
			{
				for (int i = 0; i < 5; i++)
				{
					SocketOption[i] = 0xFF;
				}
			}

			else
			{
				for (int i = 0; i < 5; i++)
				{
					SocketOption[i] = lpMsg->SocketOption[i];
				}
			}
		}

		int iItemCount = MapC[mapnumber].MonsterItemDrop(lpMsg->Type, lpMsg->Level, lpMsg->Dur, lpMsg->x,
			lpMsg->y, lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption,
			lootindex, lpMsg->m_Number, 0, SocketOption, SocketOptionIndex, lpMsg->lDuration);

		if (iItemCount != -1)
		{
			if (iRetMapNumber >= 238 && iRetMapNumber <= 245)
			{
				MapC[mapnumber].m_cItem[iItemCount].m_Time = GetTickCount() + 300000;
				MapC[mapnumber].m_cItem[iItemCount].m_LootTime = GetTickCount() + 20000;

			}

			if (iRetMapNumber >= 246 && iRetMapNumber <= 253)
			{
				MapC[mapnumber].m_cItem[iItemCount].m_Time = GetTickCount() + 900000;
				MapC[mapnumber].m_cItem[iItemCount].m_LootTime = GetTickCount() + 10000;

				int iBridgeIndex = g_BloodCastle.GetBridgeIndex(mapnumber); //s3 add-on (loc108)

				g_BloodCastle.SetQuestItemSerialNumber(iBridgeIndex, lpMsg->m_Number);
			}

			if (((mapnumber) < MAP_INDEX_ILLUSIONTEMPLE1) ? FALSE : ((mapnumber) > MAP_INDEX_ILLUSIONTEMPLE6) ? FALSE : TRUE) //Season 2.5 add-on
			{
				if (lpMsg->Type == ITEMGET(14, 64))
				{
					MapC[mapnumber].m_cItem[iItemCount].m_Time = GetTickCount() + 15000;
					MapC[mapnumber].m_cItem[iItemCount].m_LootTime = GetTickCount() + 5000;
				}

				if (lpMsg->Type == ITEMGET(12, 15))
				{
					MapC[mapnumber].m_cItem[iItemCount].m_Time = GetTickCount() + 300000;
					MapC[mapnumber].m_cItem[iItemCount].m_LootTime = GetTickCount() + 10000;
				}
			}
		}

		//int iItemCount;
		BYTE NewOption[MAX_EXOPTION_SIZE];

		for (int i = 0; i < MAX_EXOPTION_SIZE; i++)
		{
			NewOption[i] = FALSE;
		}

		if ((lpMsg->NewOption & 0x20) != 0)
		{
			NewOption[0] = TRUE;
		}
		if ((lpMsg->NewOption & 0x10) != 0)
		{
			NewOption[1] = TRUE;
		}

		if ((lpMsg->NewOption & 0x8) != 0)
		{
			NewOption[2] = TRUE;
		}
		if ((lpMsg->NewOption & 0x4) != 0)
		{
			NewOption[3] = TRUE;
		}

		if ((lpMsg->NewOption & 0x2) != 0)
		{
			NewOption[4] = TRUE;
		}
		if ((lpMsg->NewOption & 0x1) != 0)
		{
			NewOption[5] = TRUE;
		}
	}
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Item Move Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 0x53
 */
struct SDHP_ITEMMOVESAVE
{
	PBMSG_HEAD h;	// C1:53
	DWORD Serial;	// 4
	char ServerName[20];	// 8
	char Account[11];	// 1C
	char Name[11];	// 26
	char ItemName[21];	// 30
	BYTE X;	// 45
	BYTE Y;	// 46
	BYTE ItemLevel;	// 47
	BYTE ItemOp1;	// 48
	BYTE ItemOp2;	// 49
	BYTE ItemOp3;	// 4A
};


void ItemMovePathSave(char* ActID, char* Name, BYTE level, BYTE mapnumber, BYTE x, BYTE y, char* Item, BYTE op1, BYTE op2, BYTE op3, DWORD serial)
{
	SDHP_ITEMMOVESAVE pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x53;
	pMsg.h.size = sizeof(pMsg);
	pMsg.ItemLevel = level;
	pMsg.Serial = serial;
	pMsg.ItemOp1 = op1;
	pMsg.ItemOp2 = op2;
	pMsg.ItemOp3 = op3;
	pMsg.X = x;
	pMsg.Y = y;
	memcpy(pMsg.Account, ActID, sizeof(pMsg.Account));
	strcpy(pMsg.ItemName, Item);
	memcpy(pMsg.Name, Name, sizeof(pMsg.Name));
	strcpy(pMsg.ServerName, g_ConfigRead.server.GetServerName());

	wsDataCli.DataSend((PCHAR)&pMsg, pMsg.h.size);
}



/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Skill Key Data Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  : 0x60
 */
struct SDHP_SKILLKEYDATA
{
	PBMSG_HEAD h;	// C1:60
	char Name[11];	// 3
	BYTE SkillKeyBuffer[20];	// D
	BYTE GameOption;	// 17
	BYTE QkeyDefine;	// 18
	BYTE WkeyDefine;	// 19
	BYTE EkeyDefine;	// 1A
	BYTE ChatWindow;	// 1B
	BYTE RkeyDefine;
	int QWERLevelDefine;
	BYTE EnableTransformMode;
};


void DGOptionDataSend(CGameObject* lpObj, char* szName, BYTE * KeyBuffer, BYTE GO, BYTE Qk, BYTE Wk, BYTE Ek, BYTE ChatWnd, BYTE Rk, int QWER, BYTE Transform)
{
	SDHP_SKILLKEYDATA pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x60;
	pMsg.h.size = sizeof(pMsg);
	memcpy(pMsg.Name, szName, sizeof(pMsg.Name));
	memcpy(pMsg.SkillKeyBuffer, KeyBuffer, sizeof(pMsg.SkillKeyBuffer));
	pMsg.GameOption = GO;
	pMsg.QkeyDefine = Qk;
	pMsg.WkeyDefine = Wk;
	pMsg.EkeyDefine = Ek;
	pMsg.ChatWindow = ChatWnd;
	pMsg.RkeyDefine = Rk;
	pMsg.QWERLevelDefine = QWER;
	pMsg.EnableTransformMode = Transform;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}



void DGOptionDataRecv(SDHP_SKILLKEYDATA_SEND * lpMsg)
{
	CGameObject* lpObj = lpMsg->aIndex;
	char szName[MAX_ACCOUNT_LEN + 1];

	if (aIndex < 0 || aIndex >= g_ConfigRead.server.GetObjectMax())
	{
		return;
	}

	memset(szName, 0, sizeof(szName));
	memcpy(szName, lpMsg->Name, sizeof(lpMsg->Name));

	if (strcmp(lpObj->Name, szName) != 0)
	{
		return;
	}

	::GSProtocol.GCSkillKeySend(lpObj, lpMsg->SkillKeyBuffer, lpMsg->GameOption, lpMsg->QkeyDefine, lpMsg->WkeyDefine, lpMsg->EkeyDefine, lpMsg->ChatWindow, lpMsg->RkeyDefine, lpMsg->QWERLevelDefine);
	lpObj->m_PlayerData->m_EnableUseChangeSkin = lpMsg->EnableTransformMode;
	gObjViewportListProtocolCreate(&gObj[aIndex]);
}

void DGMoveOtherServer(SDHP_CHARACTER_TRANSFER_RESULT * lpMsg)
{
	if (!gObjIsConnectedGP(lpMsg->Number))
	{
		sLog->outError("error-L3 [%s][%d]", __FILE__, __LINE__);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->Number];

	if (lpMsg->Result != 0)
	{
		PMSG_DEFRESULT pResult;

		PHeadSetB((LPBYTE)&pResult, 0x99, sizeof(pResult));
		pResult.result = 1;

		IOCP.DataSend(lpMsg->Number, (LPBYTE)&pResult, pResult.h.size);

		sLog->outBasic("[CharTrasfer] Fail [%s][%s] (%d)",
			lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpMsg->Result);

		lpObj->m_MoveOtherServer = 0;

		GSProtocol.GCServerMsgStringSend("???? ????? change@webzen.co.kr?? ?????? ??�??????", lpObj->m_Index, 1);
		// Deathway translation here
		return;
	}

	sLog->outBasic("[CharTrasfer] Success [%s][%s] (%d)",
		lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpMsg->Result);

	GSProtocol.GCServerMsgStringSend("?????? ???????.", lpObj->m_Index, 1);// Deathway translation here
	GSProtocol.GCServerMsgStringSend("???? ?????? ????????�? ??????.", lpObj->m_Index, 1);// Deathway translation here
	GJSetCharacterInfo(lpObj, lpObj->m_Index, 0);
	lpObj->LoadWareHouseInfo = false;
	gObjCloseSet(lpMsg->Number, 2);
	lpObj->m_MoveOtherServer = 0;
}

struct Request_PetItem_Info
{
	BYTE nPos;	// 0
	UINT64 nSerial;	// 4
};


/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Request Pet Info Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC2
 *	HeadCode  : 0x56
 *	BASE PACKET - Complete with Request_PetItem_Info
 */
struct SDHP_REQUEST_PETITEM_INFO
{
	PWMSG_HEAD h;
	char AccountID[11];	// 4
	WORD Number;	// 10
	BYTE InvenType;	// 12
	BYTE nCount;	// 13
	WORD ServerType;
	WORD ServerCode;
};

void gObjRequestPetItemInfo(CGameObject* lpObj, int inventype)
{
	LPOBJ lpObj = &gObj[aIndex];
	char pbuffer[4096];
	int lofs = sizeof(SDHP_REQUEST_PETITEM_INFO);
	int founditemcount = 0;
	Request_PetItem_Info petIteminfo;

	if (inventype == 0)
	{
		for (int n = 0; n < INVENTORY_SIZE; n++)
		{
			if (lpObj->pntInventory[n]->IsItem())
			{
				if (lpObj->pntInventory[n]->m_Type == ITEMGET(13, 4) || lpObj->pntInventory[n]->m_Type == ITEMGET(13, 5))
				{
					founditemcount++;
					petIteminfo.nPos = n;
					petIteminfo.nSerial = lpObj->pntInventory[n]->m_Number;
					memcpy(&pbuffer[lofs], &petIteminfo, sizeof(petIteminfo));
					lofs += sizeof(petIteminfo);
				}
			}
		}
	}
	else if (inventype == 1)
	{
		for (int n = 0; n < WAREHOUSE_SIZE; n++)
		{
			if (lpObj->pntWarehouse[n]->IsItem())
			{
				if (lpObj->pntWarehouse[n]->m_Type == ITEMGET(13, 4) || lpObj->pntWarehouse[n]->m_Type == ITEMGET(13, 5))
				{
					founditemcount++;
					petIteminfo.nPos = n;
					petIteminfo.nSerial = lpObj->pntWarehouse[n]->m_Number;
					memcpy(&pbuffer[lofs], &petIteminfo, sizeof(petIteminfo));
					lofs += sizeof(petIteminfo);
				}
			}
		}
	}

	if (founditemcount)
	{
		SDHP_REQUEST_PETITEM_INFO pMsg;

		pMsg.h.set((LPBYTE)&pMsg, 0x56, lofs + sizeof(pMsg));
		memcpy(pMsg.AccountID, lpObj->m_PlayerData->ConnectUser->AccountID, MAX_ACCOUNT_LEN);
		pMsg.Number = aIndex;
		pMsg.InvenType = inventype;
		pMsg.nCount = founditemcount;
		pMsg.ServerType = g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE ? TRUE : FALSE;
		pMsg.ServerCode = g_ConfigRead.server.GetGameServerCode() / 20;

		memcpy(pbuffer, &pMsg, sizeof(pMsg));
		wsDataCli.DataSend((PCHAR)pbuffer, lofs + sizeof(pMsg));
	}
}

struct Recv_PetItem_Info
{
	BYTE nPos;	// 0
	UINT64 nSerial;	// 4
	BYTE Level;	// 8
	UINT64 Exp;	// C
};

struct SDHP_RECV_PETITEM_INFO
{
	PWMSG_HEAD h;
	char AccountID[11];	// 4
	WORD Number;	// 10
	BYTE InvenType;	// 12
	BYTE nCount;	// 13
};

void DGRecvPetItemInfo(LPBYTE lpData)
{
	SDHP_RECV_PETITEM_INFO* lpMsg = (SDHP_RECV_PETITEM_INFO *)lpData;
	Recv_PetItem_Info* pPetItemInfo = (Recv_PetItem_Info *)(lpData + sizeof(SDHP_RECV_PETITEM_INFO));
	CGameObject* lpObj = lpMsg->Number;
	char szAccountId[MAX_ACCOUNT_LEN + 1];
	szAccountId[MAX_ACCOUNT_LEN] = 0;

	memcpy(szAccountId, lpMsg->AccountID, MAX_ACCOUNT_LEN);

	if (!gObjIsAccontConnect(lpObj, szAccountId))
	{
		sLog->outError("Request to receive petitem infomation doesn't match the user %s", szAccountId);
		GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);

		return;
	}

	if (lpMsg->InvenType == 0)
	{
		BOOL ReCalc = 0;

		for (int n = 0; n < lpMsg->nCount; n++)
		{
			if (pPetItemInfo[n].nPos < INVETORY_WEAR_SIZE)
			{
				ReCalc = 1;
			}

			if (lpObj->pInventory[pPetItemInfo[n].nPos].IsItem())
			{
				if (lpObj->pInventory[pPetItemInfo[n].nPos].m_Number == pPetItemInfo[n].nSerial)
				{
					lpObj->pInventory[pPetItemInfo[n].nPos].SetPetItemInfo(pPetItemInfo[n].Level, pPetItemInfo[n].Exp);
				}
			}
		}

		if (ReCalc != FALSE)
		{
			gObjCalCharacter.CalcCharacter(lpObj);
		}
	}
	else if (lpMsg->InvenType == 1)
	{
		for (int n = 0; n < lpMsg->nCount; n++)
		{

			if (lpObj->pWarehouse[pPetItemInfo[n].nPos].IsItem())
			{
				if (lpObj->pWarehouse[pPetItemInfo[n].nPos].m_Number == pPetItemInfo[n].nSerial)
				{
					lpObj->pWarehouse[pPetItemInfo[n].nPos].SetPetItemInfo(pPetItemInfo[n].Level, pPetItemInfo[n].Exp);
				}
			}
		}

	}
}

struct Save_PetItem_Info
{
	UINT64 nSerial;	// 0
	BYTE Level;	// 3
	UINT64 Exp;	// 8
};


/* * * * * * * * * * * * * * * * * * * * *
 *	Mu PetInfo Save Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC2
 *	HeadCode  : 0x55
 *	BASE PACKET - Complete with Save_PetItem_Info
 */
struct SDHP_SAVE_PETITEM_INFO
{
	PWMSG_HEAD h;
	BYTE nCount;	// 4
	WORD ServerType;
	WORD ServerCode;
};

void gObjSavePetItemInfo(CGameObject* lpObj, int inventype)
{
	LPOBJ lpObj = &gObj[aIndex];
	char pbuffer[4096];
	int lofs = sizeof(SDHP_SAVE_PETITEM_INFO);
	int founditemcount = 0;
	Save_PetItem_Info petIteminfo;

	if (inventype == 0)
	{
		for (int n = 0; n < INVENTORY_SIZE; n++)
		{
			if (lpObj->pntInventory[n]->IsItem())
			{
				if (lpObj->pntInventory[n]->m_Type == ITEMGET(13, 4) || lpObj->pntInventory[n]->m_Type == ITEMGET(13, 5))
				{
					if (lpObj->pntInventory[n]->m_IsLoadPetItemInfo != FALSE)
					{
						founditemcount++;
						petIteminfo.nSerial = lpObj->pntInventory[n]->m_Number;
						petIteminfo.Level = lpObj->pntInventory[n]->m_PetItem_Level;
						petIteminfo.Exp = lpObj->pntInventory[n]->m_PetItem_Exp;

						memcpy(&pbuffer[lofs], &petIteminfo, sizeof(petIteminfo));
						lofs += sizeof(petIteminfo);
					}
				}
			}
		}
	}
	else if (inventype == 1)
	{
		for (int n = 0; n < WAREHOUSE_SIZE; n++)
		{
			if (lpObj->pntWarehouse[n]->IsItem())
			{
				if (lpObj->pntWarehouse[n]->m_Type == ITEMGET(13, 4) || lpObj->pntWarehouse[n]->m_Type == ITEMGET(13, 5))
				{
					if (lpObj->pntWarehouse[n]->m_IsLoadPetItemInfo != FALSE)
					{
						founditemcount++;
						petIteminfo.nSerial = lpObj->pntWarehouse[n]->m_Number;
						petIteminfo.Level = lpObj->pntWarehouse[n]->m_PetItem_Level;
						petIteminfo.Exp = lpObj->pntWarehouse[n]->m_PetItem_Exp;

						memcpy(&pbuffer[lofs], &petIteminfo, sizeof(petIteminfo));
						lofs += sizeof(petIteminfo);
					}
				}
			}
		}
	}

	if (founditemcount)
	{
		SDHP_SAVE_PETITEM_INFO pMsg;

		pMsg.h.set((LPBYTE)&pMsg, 0x57, lofs + sizeof(pMsg));
		pMsg.nCount = founditemcount;
		pMsg.ServerType = g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE ? TRUE : FALSE;

		if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
		{
			pMsg.ServerCode = lpObj->m_PlayerData->m_nServerCodeOfHomeWorld;
		}

		else
		{
			pMsg.ServerCode = g_ConfigRead.server.GetGameServerCode() / 20;
		}

		memcpy(pbuffer, &pMsg, sizeof(pMsg));

		wsDataCli.DataSend(pbuffer, lofs + sizeof(pMsg));
	}
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  :
 *	SubCode   :
 */
struct CSP_REQ_CASTLEDATA
{
	PBMSG_HEAD2 h;	// C1:80:00
	WORD wMapSvrNum;	// 4
	int iCastleEventCycle;	// 8
};

//004340C0 - identical
void GS_GDReqCastleTotalInfo(int iMapSvrGroup, int iCastleEventCycle)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	CSP_REQ_CASTLEDATA pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x00, sizeof(CSP_REQ_CASTLEDATA));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iCastleEventCycle = iCastleEventCycle;

	::wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  :
 *	SubCode   :
 */
struct CSP_REQ_OWNERGUILDMASTER
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
};

//00434180 - identical
void GS_GDReqOwnerGuildMaster(int iMapSvrGroup, CGameObject* lpObj)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (ObjectMaxRange(lpObj) == FALSE)
	{
		return;
	}

	CSP_REQ_OWNERGUILDMASTER pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x01, sizeof(CSP_REQ_OWNERGUILDMASTER));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  :
 *	SubCode   :
 */
struct CSP_REQ_NPCBUY
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	int iNpcNumber;	// C
	int iNpcIndex;	// 10
	int iNpcDfLevel;	// 14
	int iNpcRgLevel;	// 18
	int iNpcMaxHp;	// 1C
	int iNpcHp;	// 20
	BYTE btNpcX;	// 24
	BYTE btNpcY;	// 25
	BYTE btNpcDIR;	// 26
	int iBuyCost;	// 28
};

//00434220 - identical
void GS_GDReqCastleNpcBuy(int iMapSvrGroup, CGameObject* lpObj, int iNpcNumber, int iNpcIndex, int iNpcDfLevel, int iNpcRgLevel, int iNpcMaxHP, int iNpcHP, BYTE btNpcX, BYTE btNpcY, BYTE btNpcDIR, int iBuyCost)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (ObjectMaxRange(lpObj) == FALSE)
	{
		return;
	}

	CSP_REQ_NPCBUY pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x03, sizeof(CSP_REQ_NPCBUY));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;
	pMsg.iNpcNumber = iNpcNumber;
	pMsg.iNpcIndex = iNpcIndex;
	pMsg.iNpcDfLevel = iNpcDfLevel;
	pMsg.iNpcRgLevel = iNpcRgLevel;
	pMsg.iNpcMaxHp = iNpcMaxHP;
	pMsg.iNpcHp = iNpcHP;
	pMsg.btNpcX = btNpcX;
	pMsg.btNpcY = btNpcY;
	pMsg.btNpcDIR = btNpcDIR;
	pMsg.iBuyCost = iBuyCost;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  :
 *	SubCode   :
 */
struct CSP_REQ_NPCREPAIR
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	int iNpcNumber;	// C
	int iNpcIndex;	// 10
	int iRepairCost;	// 14
};

//00434310 - identical
void GS_GDReqCastleNpcRepair(int iMapSvrGroup, CGameObject* lpObj, int iNpcNumber, int iNpcIndex, int iRepairCost)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (ObjectMaxRange(lpObj) == FALSE)
	{
		return;
	}

	CSP_REQ_NPCREPAIR pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x04, sizeof(CSP_REQ_NPCREPAIR));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;
	pMsg.iNpcNumber = iNpcNumber;
	pMsg.iNpcIndex = iNpcIndex;
	pMsg.iRepairCost = iRepairCost;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  :
 *	SubCode   :
 */
struct CSP_REQ_NPCUPGRADE
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	int iNpcNumber;	// C
	int iNpcIndex;	// 10
	int iNpcUpType;	// 14
	int iNpcUpValue;	// 18
	int iNpcUpIndex;	// 1C
};

void GS_GDReqCastleNpcUpgrade(int iMapSvrGroup, CGameObject* lpObj, int iNpcNumber, int iNpcIndex, int iNpcUpType, int iNpcUpValue, int iNpcUpIndex)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (ObjectMaxRange(lpObj) == FALSE)
	{
		return;
	}

	CSP_REQ_NPCUPGRADE pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x05, sizeof(CSP_REQ_NPCUPGRADE));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;
	pMsg.iNpcNumber = iNpcNumber;
	pMsg.iNpcIndex = iNpcIndex;
	pMsg.iNpcUpType = iNpcUpType;
	pMsg.iNpcUpValue = iNpcUpValue;
	pMsg.iNpcUpIndex = iNpcUpIndex;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  :
 *	SubCode   :
 */
struct CSP_REQ_TAXINFO
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 3
	int iIndex;	// 8
};

void GS_GDReqTaxInfo(int iMapSvrGroup, CGameObject* lpObj)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (ObjectMaxRange(lpObj) == FALSE)
	{
		return;
	}

	CSP_REQ_TAXINFO pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x06, sizeof(CSP_REQ_TAXINFO));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}


/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  :
 *	SubCode   :
 */
struct CSP_REQ_TAXRATECHANGE
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	int iTaxKind;	// C
	int iTaxRate;	// 10
};

void GS_GDReqTaxRateChange(int iMapSvrGroup, CGameObject* lpObj, int iTaxType, int iTaxRate)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (ObjectMaxRange(lpObj) == FALSE)
	{
		return;
	}

	CSP_REQ_TAXRATECHANGE pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x07, sizeof(CSP_REQ_TAXRATECHANGE));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;
	pMsg.iTaxRate = iTaxRate;
	pMsg.iTaxKind = iTaxType;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  :
 *	SubCode   :
 */
struct CSP_REQ_MONEYCHANGE
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	int iMoneyChanged;	// C
};

void GS_GDReqCastleMoneyChange(int iMapSvrGroup, CGameObject* lpObj, int iMoneyChange)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (ObjectMaxRange(lpObj) == FALSE)
	{
		return;
	}

	CSP_REQ_MONEYCHANGE pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x08, sizeof(CSP_REQ_MONEYCHANGE));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;
	pMsg.iMoneyChanged = iMoneyChange;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  :
 *	SubCode   :
 */
struct CSP_REQ_SDEDCHANGE
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	WORD wStartYear;	// C
	BYTE btStartMonth;	// E
	BYTE btStartDay;	// F
	WORD wEndYear;	// 10
	BYTE btEndMonth;	// 12
	BYTE btEndDay;	// 13
};

void GS_GDReqSiegeDateChange(int iMapSvrGroup, CGameObject* lpObj, WORD wStartYear, BYTE btStartMonth, BYTE btStartDay, WORD wEndYear, BYTE btEndMonth, BYTE btEndDay)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	CSP_REQ_SDEDCHANGE pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x09, sizeof(CSP_REQ_SDEDCHANGE));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;
	pMsg.wStartYear = wStartYear;
	pMsg.btStartMonth = btStartMonth;
	pMsg.btStartDay = btStartDay;
	pMsg.wEndYear = wEndYear;
	pMsg.btEndMonth = btEndMonth;
	pMsg.btEndDay = btEndDay;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  :
 *	SubCode   :
 */
struct CSP_REQ_GUILDREGINFO
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	char szGuildName[8];	// C
};

void GS_GDReqGuildMarkRegInfo(int iMapSvrGroup, CGameObject* lpObj)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (ObjectMaxRange(lpObj) == FALSE)
	{
		return;
	}

	CSP_REQ_GUILDREGINFO pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x0A, sizeof(CSP_REQ_GUILDREGINFO));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;
	memcpy(pMsg.szGuildName, lpObj->m_PlayerData->GuildName, 8);

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  :
 *	SubCode   :
 */
struct CSP_REQ_SIEGEENDCHANGE
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	BOOL bIsSiegeEnded;	// 8
};

void GS_GDReqSiegeEndedChange(int iMapSvrGroup, BOOL bIsSiegeEnded)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	CSP_REQ_SIEGEENDCHANGE pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x0B, sizeof(CSP_REQ_SIEGEENDCHANGE));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.bIsSiegeEnded = bIsSiegeEnded;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  :
 *	SubCode   :
 */
struct CSP_REQ_CASTLEOWNERCHANGE
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	BOOL bIsCastleOccupied;	// 8
	char szOwnerGuildName[8];	// C
};

void GS_GDReqCastleOwnerChange(int iMapSvrGroup, BOOL bIsCastleOccupied, char* lpszGuildName)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (lpszGuildName == NULL)
	{
		return;
	}

	CSP_REQ_CASTLEOWNERCHANGE pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x0C, sizeof(CSP_REQ_CASTLEOWNERCHANGE));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.bIsCastleOccupied = bIsCastleOccupied;
	memcpy(pMsg.szOwnerGuildName, lpszGuildName, 8);

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  :
 *	SubCode   :
 */
struct CSP_REQ_REGATTACKGUILD
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	char szEnemyGuildName[8];	// C
};

void GS_GDReqRegAttackGuild(int iMapSvrGroup, CGameObject* lpObj)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (ObjectMaxRange(lpObj) == FALSE)
	{
		return;
	}

	CSP_REQ_REGATTACKGUILD pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x0D, sizeof(CSP_REQ_REGATTACKGUILD));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;
	memcpy(pMsg.szEnemyGuildName, lpObj->m_PlayerData->GuildName, 8);

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  :
 *	SubCode   :
 */
struct CSP_REQ_CASTLESIEGEEND
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
};

void GS_GDReqRestartCastleState(int iMapSvrGroup)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	CSP_REQ_CASTLESIEGEEND pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x0E, sizeof(CSP_REQ_CASTLESIEGEEND));
	pMsg.wMapSvrNum = iMapSvrGroup;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  :
 *	SubCode   :
 */
struct CSP_REQ_MAPSVRMULTICAST
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	char szMsgText[128];	// 6
};

void GS_GDReqMapSvrMsgMultiCast(int iMapSvrGroup, char * lpszMsgText)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (lpszMsgText == NULL)
	{
		return;
	}

	CSP_REQ_MAPSVRMULTICAST pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x0F, sizeof(CSP_REQ_MAPSVRMULTICAST));
	pMsg.wMapSvrNum = iMapSvrGroup;
	strcpy(pMsg.szMsgText, lpszMsgText);
	pMsg.szMsgText[127] = 0;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

struct CSP_GLOBALPOST_MULTICAST
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	char szSender[MAX_ACCOUNT_LEN];
	char szMessage[MAX_CHAT_LEN];
	char szServerName[50];
	BYTE btColorRGB[3];
};

void GS_GDReqGlobalPostMultiCast(int iMapSvrGroup, CGameObject* lpObj, char * szMessage)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (!ObjectMaxRange(lpObj))
	{
		return;
	}

	if (szMessage == NULL)
	{
		return;
	}

	CSP_GLOBALPOST_MULTICAST pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x1F, sizeof(CSP_GLOBALPOST_MULTICAST));
	pMsg.wMapSvrNum = iMapSvrGroup;
	memcpy(pMsg.szSender, lpObj->Name, MAX_ACCOUNT_LEN);
	strcpy(pMsg.szMessage, szMessage);
	memcpy(pMsg.szServerName, g_ConfigRead.server.GetServerName(), 50);

	pMsg.btColorRGB[0] = g_ConfigRead.data.post.btGPostColorRGB[0];
	pMsg.btColorRGB[1] = g_ConfigRead.data.post.btGPostColorRGB[1];
	pMsg.btColorRGB[2] = g_ConfigRead.data.post.btGPostColorRGB[2];

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  :
 *	SubCode   :
 */
struct CSP_REQ_GUILDREGMARK
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	char szGuildName[8];	// C
	int iItemPos;	// 14
};

void GS_GDReqRegGuildMark(int iMapSvrGroup, CGameObject* lpObj, int iItemPos)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (ObjectMaxRange(lpObj) == FALSE)
	{
		return;
	}

	CSP_REQ_GUILDREGMARK pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x10, sizeof(CSP_REQ_GUILDREGMARK));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;
	pMsg.iItemPos = iItemPos;
	memcpy(&pMsg.szGuildName, lpObj->m_PlayerData->GuildName, sizeof(pMsg.szGuildName));

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  :
 *	SubCode   :
 */
struct CSP_REQ_GUILDRESETMARK
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	char szGuildName[8];	// C
};

void GS_GDReqGuildMarkReset(int iMapSvrGroup, CGameObject* lpObj, char* lpszGuildName)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (ObjectMaxRange(lpObj) == FALSE)
	{
		return;
	}

	if (lpszGuildName == NULL)
	{
		return;
	}

	CSP_REQ_GUILDRESETMARK pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x11, sizeof(CSP_REQ_GUILDRESETMARK));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;
	memcpy(pMsg.szGuildName, lpszGuildName, 8);

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  :
 *	SubCode   :
 */
struct CSP_REQ_GUILDSETGIVEUP
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	char szGuildName[8];	// C
	BOOL bIsGiveUp;	// 14
};

void GS_GDReqGuildSetGiveUp(int iMapSvrGroup, CGameObject* lpObj, BOOL bIsGiveUp)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (ObjectMaxRange(lpObj) == FALSE)
	{
		return;
	}

	CSP_REQ_GUILDSETGIVEUP pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x12, sizeof(CSP_REQ_GUILDSETGIVEUP));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;
	pMsg.bIsGiveUp = bIsGiveUp;
	memcpy(pMsg.szGuildName, lpObj->m_PlayerData->GuildName, 8);

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  :
 *	SubCode   :
 */
struct CSP_REQ_NPCREMOVE
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iNpcNumber;	// 8
	int iNpcIndex;	// C
};

void GS_GDReqNpcRemove(int iMapSvrGroup, int iNpcNumber, int iNpcIndex)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	CSP_REQ_NPCREMOVE pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x16, sizeof(CSP_REQ_NPCREMOVE));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iNpcNumber = iNpcNumber;
	pMsg.iNpcIndex = iNpcIndex;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  :
 *	SubCode   :
 */
struct CSP_REQ_CASTLESTATESYNC
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iCastleState;	// 8
	int iTaxRateChaos;	// C
	int iTaxRateStore;	// 10
	int iTaxHuntZone;	// 14
	char szOwnerGuildName[8];	// 18
};

void GS_GDReqCastleStateSync(int iMapSvrGroup, int iCastleState, int iTaxRateChaos, int iTaxRateStore, int iTaxHuntZone, char* lpszOwnerGuild)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	CSP_REQ_CASTLESTATESYNC pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x17, sizeof(CSP_REQ_CASTLESTATESYNC));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iCastleState = iCastleState;
	pMsg.iTaxRateChaos = iTaxRateChaos;
	pMsg.iTaxRateStore = iTaxRateStore;
	pMsg.iTaxHuntZone = iTaxHuntZone;
	memcpy(pMsg.szOwnerGuildName, lpszOwnerGuild, 8);

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  :
 *	SubCode   :
 */
struct CSP_REQ_CASTLETRIBUTEMONEY
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iCastleTributeMoney;	// 8
};

void GS_GDReqCastleTributeMoney(int iMapSvrGroup, int iCastleTributeMoney)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (iCastleTributeMoney < 0)
	{
		return;
	}

	CSP_REQ_CASTLETRIBUTEMONEY pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x18, sizeof(CSP_REQ_CASTLETRIBUTEMONEY));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iCastleTributeMoney = iCastleTributeMoney;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  :
 *	SubCode   :
 */
struct CSP_REQ_RESETCASTLETAXINFO
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
};

void GS_GDReqResetCastleTaxInfo(int iMapSvrGroup)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	CSP_REQ_RESETCASTLETAXINFO pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x19, sizeof(CSP_REQ_RESETCASTLETAXINFO));
	pMsg.wMapSvrNum = iMapSvrGroup;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  :
 *	SubCode   :
 */
struct CSP_REQ_RESETSIEGEGUILDINFO
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
};

void GS_GDReqResetSiegeGuildInfo(int iMapSvrGroup)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	CSP_REQ_RESETSIEGEGUILDINFO pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x1A, sizeof(CSP_REQ_RESETSIEGEGUILDINFO));
	pMsg.wMapSvrNum = iMapSvrGroup;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  :
 *	SubCode   :
 */
struct CSP_REQ_RESETREGSIEGEINFO
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 2
};

void GS_GDReqResetRegSiegeInfo(int iMapSvrGroup)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	CSP_REQ_RESETREGSIEGEINFO pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x80, 0x1B, sizeof(CSP_REQ_RESETREGSIEGEINFO));
	pMsg.wMapSvrNum = iMapSvrGroup;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  :
 */
struct CSP_REQ_CSINITDATA
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
	int iCastleEventCycle;	// 8
};

void GS_GDReqCastleInitData(int iMapSvrGroup, int iCastleEventCycle)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	CSP_REQ_CSINITDATA pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x81, sizeof(CSP_REQ_CSINITDATA));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iCastleEventCycle = iCastleEventCycle;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  :
 */
struct CSP_REQ_NPCDATA
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
};

void GS_GDReqCastleNpcInfo(int iMapSvrGroup, CGameObject* lpObj)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	CSP_REQ_NPCDATA pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x82, sizeof(CSP_REQ_NPCDATA));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  :
 */
struct CSP_REQ_ALLGUILDREGINFO
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
};

void GS_GDReqAllGuildMarkRegInfo(int iMapSvrGroup, CGameObject* lpObj)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	CSP_REQ_ALLGUILDREGINFO pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x83, sizeof(CSP_REQ_ALLGUILDREGINFO));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = aIndex;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  :
 */
struct CSP_REQ_CALCREGGUILDLIST
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
};

void GS_GDReqCalcRegGuildList(int iMapSvrGroup)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	CSP_REQ_CALCREGGUILDLIST pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x85, sizeof(CSP_REQ_CALCREGGUILDLIST));
	pMsg.wMapSvrNum = iMapSvrGroup;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : GameServer -> DataServer
 *  Code      : 0xC1
 *	HeadCode  :
 */
struct CSP_REQ_CSLOADTOTALGUILDINFO
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
};

void GS_GDReqCsLoadTotalGuildInfo(int iMapSvrGroup)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	CSP_REQ_CSLOADTOTALGUILDINFO pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0x88, sizeof(CSP_REQ_CSLOADTOTALGUILDINFO));
	pMsg.wMapSvrNum = iMapSvrGroup;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

struct CSP_ANS_CASTLEDATA
{

	// static data ------------------------------------

	// non-static data --------------------------------
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xa>*/ /*|0x2|*/ WORD wStartYear;
	/*<thisrel this+0xc>*/ /*|0x1|*/ BYTE btStartMonth;
	/*<thisrel this+0xd>*/ /*|0x1|*/ BYTE btStartDay;
	/*<thisrel this+0xe>*/ /*|0x2|*/ WORD wEndYear;
	/*<thisrel this+0x10>*/ /*|0x1|*/ BYTE btEndMonth;
	/*<thisrel this+0x11>*/ /*|0x1|*/ BYTE btEndDay;
	/*<thisrel this+0x12>*/ /*|0x1|*/ BYTE btIsSiegeGuildList;
	/*<thisrel this+0x13>*/ /*|0x1|*/ BYTE btIsSiegeEnded;
	/*<thisrel this+0x14>*/ /*|0x1|*/ BYTE btIsCastleOccupied;
	/*<thisrel this+0x15>*/ /*|0x8|*/ char szCastleOwnGuild[8];
	/*<thisrel this+0x20>*/ /*|0x8|*/ __int64 i64CastleMoney;
	/*<thisrel this+0x28>*/ /*|0x4|*/ int iTaxRateChaos;
	/*<thisrel this+0x2c>*/ /*|0x4|*/ int iTaxRateStore;
	/*<thisrel this+0x30>*/ /*|0x4|*/ int iTaxHuntZone;
	/*<thisrel this+0x34>*/ /*|0x4|*/ int iFirstCreate;
};

void GS_DGAnsCastleTotalInfo(LPBYTE lpRecv)
{
	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		CSP_ANS_CASTLEDATA* lpMsg = (CSP_ANS_CASTLEDATA*)lpRecv;

		if (lpMsg == NULL)
			return;

		if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
			sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x00] GS_DGAnsCastleTotalInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
	}

	else
	{
		return;
	}
}

struct CSP_ANS_OWNERGUILDMASTER {

	// static data ------------------------------------

	// non-static data --------------------------------
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iIndex;
	/*<thisrel this+0x10>*/ /*|0x8|*/ char szCastleOwnGuild[8];
	/*<thisrel this+0x18>*/ /*|0xa|*/ char szCastleOwnGuildMaster[10];

};

void GS_DGAnsOwnerGuildMaster(LPBYTE lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_OWNERGUILDMASTER* lpMsg = (CSP_ANS_OWNERGUILDMASTER*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x00] GS_DGAnsCastleTotalInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	GSProtocol.GCAnsCastleSiegeState(lpMsg->iIndex, lpMsg->iResult, lpMsg->szCastleOwnGuild, lpMsg->szCastleOwnGuildMaster);
}

struct CSP_ANS_NPCBUY {

	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iIndex;
	/*<thisrel this+0x10>*/ /*|0x4|*/ int iNpcNumber;
	/*<thisrel this+0x14>*/ /*|0x4|*/ int iNpcIndex;
	/*<thisrel this+0x18>*/ /*|0x4|*/ int iBuyCost;
};

void GS_DGAnsCastleNpcBuy(LPBYTE lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_NPCBUY* lpMsg = (CSP_ANS_NPCBUY*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x03] GS_DGAnsCastleNpcBuy() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if (lpMsg->iResult == 1)
	{
		BOOL bAddResult = g_CastleSiege.AddDbNPC(lpMsg->iNpcNumber, lpMsg->iNpcIndex);

		if (bAddResult == TRUE)
		{
			if (gObjIsConnected(lpMsg->iIndex))
			{
				gObj[lpMsg->iIndex].m_PlayerData->Money -= lpMsg->iBuyCost;

				if (gObj[lpMsg->iIndex].m_PlayerData->Money < 0)
				{
					gObj[lpMsg->iIndex].m_PlayerData->Money = 0;
				}

				GSProtocol.GCMoneySend(lpMsg->iIndex, gObj[lpMsg->iIndex].m_PlayerData->Money);
			}

			sLog->outBasic("[CastleSiege] GS_DGAnsCastleNpcBuy() - CCastleSiege::AddDbNPC() OK - Npc:(CLS:%d, IDX:%d)", lpMsg->iNpcNumber, lpMsg->iNpcIndex);
		}
		else
		{
			sLog->outBasic("[CastleSiege] GS_DGAnsCastleNpcBuy() - CCastleSiege::AddDbNPC() FAILED - Npc:(CLS:%d, IDX:%d)", lpMsg->iNpcNumber, lpMsg->iNpcIndex);
		}
	}
	else
	{
		sLog->outBasic("[CastleSiege] GS_DGAnsCastleNpcBuy() FAILED - Result:(%d), Npc:(CLS:%d, IDX:%d)", lpMsg->iResult, lpMsg->iNpcNumber, lpMsg->iNpcIndex);
	}

	GSProtocol.GCAnsNpcBuy(lpMsg->iIndex, lpMsg->iResult, lpMsg->iNpcNumber, lpMsg->iNpcIndex);
}

struct CSP_ANS_NPCREPAIR {
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iIndex;
	/*<thisrel this+0x10>*/ /*|0x4|*/ int iNpcNumber;
	/*<thisrel this+0x14>*/ /*|0x4|*/ int iNpcIndex;
	/*<thisrel this+0x18>*/ /*|0x4|*/ int iNpcMaxHp;
	/*<thisrel this+0x1c>*/ /*|0x4|*/ int iNpcHp;
	/*<thisrel this+0x20>*/ /*|0x4|*/ int iRepairCost;
};

void GS_DGAnsCastleNpcRepair(LPBYTE lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_NPCREPAIR* lpMsg = (CSP_ANS_NPCREPAIR*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x04] GS_DGAnsCastleNpcRepair() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if (lpMsg->iResult == 1)
	{
		BOOL bRepairResult = g_CastleSiege.RepairDbNPC(lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcHp, lpMsg->iNpcMaxHp);
		if (bRepairResult == TRUE)
		{
			if (gObjIsConnected(lpMsg->iIndex))
			{
				gObj[lpMsg->iIndex].m_PlayerData->Money -= lpMsg->iRepairCost;

				if (gObj[lpMsg->iIndex].m_PlayerData->Money < 0)
				{
					gObj[lpMsg->iIndex].m_PlayerData->Money = 0;
				}

				GSProtocol.GCMoneySend(lpMsg->iIndex, gObj[lpMsg->iIndex].m_PlayerData->Money);
			}

			sLog->outBasic("[CastleSiege] GS_DGAnsCastleNpcRepair() - CCastleSiege::RepairDbNPC() OK - Npc:(CLS:%d, IDX:%d)", lpMsg->iNpcNumber, lpMsg->iNpcIndex);
		}
		else
		{
			sLog->outBasic("[CastleSiege] GS_DGAnsCastleNpcRepair() - CCastleSiege::RepairDbNPC() FAILED - Npc:(CLS:%d, IDX:%d)", lpMsg->iNpcNumber, lpMsg->iNpcIndex);
		}
	}
	else
	{
		sLog->outBasic("[CastleSiege] GS_DGAnsCastleNpcRepair() FAILED - Result:(%d), Npc:(CLS:%d, IDX:%d)", lpMsg->iResult, lpMsg->iNpcNumber, lpMsg->iNpcIndex);
	}

	GSProtocol.GCAnsNpcRepair(lpMsg->iIndex, lpMsg->iResult, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcHp, lpMsg->iNpcMaxHp);
}

struct CSP_ANS_NPCUPGRADE {
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iIndex;
	/*<thisrel this+0x10>*/ /*|0x4|*/ int iNpcNumber;
	/*<thisrel this+0x14>*/ /*|0x4|*/ int iNpcIndex;
	/*<thisrel this+0x18>*/ /*|0x4|*/ int iNpcUpType;
	/*<thisrel this+0x1c>*/ /*|0x4|*/ int iNpcUpValue;
	/*<thisrel this+0x20>*/ /*|0x4|*/ int iNpcUpIndex;
};

void GS_DGAnsCastleNpcUpgrade(LPBYTE lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_NPCUPGRADE* lpMsg = (CSP_ANS_NPCUPGRADE*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x05] GS_DGAnsCastleNpcUpgrade() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if (lpMsg->iResult == 0)
	{
		sLog->outError("[CastleSiege] ERROR - Castle NPC Upgrade Fail() (CLS:%d, IDX:%d, UPTYPE:%d, UPVAL:%d)", lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
	}
	else
	{
		g_CastleSiege.UpgradeDbNPC(lpMsg->iIndex, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue, lpMsg->iNpcUpIndex);
		sLog->outBasic("[CastleSiege] [0x80][0x06] GS_DGAnsTaxInfo() - Npc Upgrade OK (CLS:%d, IDX:%d, UPTYPE:%d, UPVAL:%d)", lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
	}

	GSProtocol.GCAnsNpcUpgrade(lpMsg->iIndex, lpMsg->iResult, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
}

struct CSP_ANS_TAXINFO {
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iIndex;
	/*<thisrel this+0x10>*/ /*|0x8|*/ __int64 i64CastleMoney;
	/*<thisrel this+0x18>*/ /*|0x4|*/ int iTaxRateChaos;
	/*<thisrel this+0x1c>*/ /*|0x4|*/ int iTaxRateStore;
	/*<thisrel this+0x20>*/ /*|0x4|*/ int iTaxHuntZone;
};

void GS_DGAnsTaxInfo(LPBYTE lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_TAXINFO* lpMsg = (CSP_ANS_TAXINFO*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x06] GS_DGAnsTaxInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if (lpMsg->iResult == TRUE)
	{
		GSProtocol.GCAnsTaxMoneyInfo(lpMsg->iIndex, lpMsg->iResult, lpMsg->iTaxRateChaos, lpMsg->iTaxRateStore, lpMsg->i64CastleMoney);
		g_CastleSiege.SetCastleMoney(lpMsg->i64CastleMoney);
	}
}

struct CSP_ANS_TAXRATECHANGE {
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iIndex;
	/*<thisrel this+0x10>*/ /*|0x4|*/ int iTaxKind;
	/*<thisrel this+0x14>*/ /*|0x4|*/ int iTaxRate;
};

void GS_DGAnsTaxRateChange(LPBYTE lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_TAXRATECHANGE* lpMsg = (CSP_ANS_TAXRATECHANGE*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x07] GS_DGAnsTaxRateChange() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if (lpMsg->iResult == 1)
	{
		g_CastleSiege.SetTaxRate(lpMsg->iTaxKind, lpMsg->iTaxRate);
		GSProtocol.GCAnsTaxRateChange(lpMsg->iIndex, lpMsg->iResult, lpMsg->iTaxKind, lpMsg->iTaxRate);
	}
}

struct CSP_ANS_MONEYCHANGE {
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iIndex;
	/*<thisrel this+0x10>*/ /*|0x4|*/ int iMoneyChanged;
	/*<thisrel this+0x18>*/ /*|0x8|*/ __int64 i64CastleMoney;
};

void GS_DGAnsCastleMoneyChange(LPBYTE lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_MONEYCHANGE* lpMsg = (CSP_ANS_MONEYCHANGE*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x08] GS_DGAnsCastleMoneyChange() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if (lpMsg->iResult == TRUE)
	{
		if (gObjIsConnected(lpMsg->iIndex))
		{
			if (lpMsg->iMoneyChanged < 0)
			{
				gObj[lpMsg->iIndex].m_PlayerData->Money -= lpMsg->iMoneyChanged;

				if (gObj[lpMsg->iIndex].m_PlayerData->Money > MAX_ZEN)
				{
					gObj[lpMsg->iIndex].m_PlayerData->Money = MAX_ZEN;
				}

				GSProtocol.GCMoneySend(lpMsg->iIndex, gObj[lpMsg->iIndex].m_PlayerData->Money);

				sLog->outBasic("[CastleSiege] [0x80][0x08] GS_DGAnsCastleMoneyChange() - Withdraw Request OK [%s][%s] (ReqMoney:%d, TotMoney:%I64d)", gObj[lpMsg->iIndex].AccountID,
					gObj[lpMsg->iIndex].Name, lpMsg->iMoneyChanged, lpMsg->i64CastleMoney);
			}
		}
		g_CastleSiege.SetCastleMoney(lpMsg->i64CastleMoney);
	}

	GSProtocol.GCAnsMoneyDrawOut(lpMsg->iIndex, lpMsg->iResult, lpMsg->i64CastleMoney);
}

struct CSP_ANS_SDEDCHANGE {
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iIndex;
	/*<thisrel this+0x10>*/ /*|0x2|*/ WORD wStartYear;
	/*<thisrel this+0x12>*/ /*|0x1|*/ BYTE btStartMonth;
	/*<thisrel this+0x13>*/ /*|0x1|*/ BYTE btStartDay;
	/*<thisrel this+0x14>*/ /*|0x2|*/ WORD wEndYear;
	/*<thisrel this+0x16>*/ /*|0x1|*/ BYTE btEndMonth;
	/*<thisrel this+0x17>*/ /*|0x1|*/ BYTE btEndDay;
};

void GS_DGAnsSiegeDateChange(LPBYTE lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_SDEDCHANGE* lpMsg = (CSP_ANS_SDEDCHANGE*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x09] GS_DGAnsSiegeDateChange() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}
}

void GS_DGAnsGuildMarkRegInfo(LPBYTE lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_GUILDREGINFO* lpMsg = (CSP_ANS_GUILDREGINFO*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x0A] GS_DGAnsGuildMarkRegInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	GSProtocol.GCAnsGuildRegInfo(lpMsg->iIndex, lpMsg->iResult, lpMsg);
}

struct CSP_ANS_SIEGEENDCHANGE {
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int bIsSiegeEnded;
};

void GS_DGAnsSiegeEndedChange(LPBYTE lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_SIEGEENDCHANGE* lpMsg = (CSP_ANS_SIEGEENDCHANGE*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x0B] GS_DGAnsSiegeEndedChange() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}
}

struct CSP_ANS_CASTLEOWNERCHANGE
{
	PBMSG_HEAD2 h;	// C1:80:0C
	int iResult;	// 4
	unsigned short wMapSvrNum;	// 8
	int bIsCastleOccupied;	// C
	char szOwnerGuildName[0x8];	// 10
};

void GS_DGAnsCastleOwnerChange(LPBYTE lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_CASTLEOWNERCHANGE* lpMsg = (CSP_ANS_CASTLEOWNERCHANGE*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x0C] GS_DGAnsCastleOwnerChange() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}
}

struct CSP_ANS_REGATTACKGUILD {

	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iIndex;
	/*<thisrel this+0x10>*/ /*|0x8|*/ char szEnemyGuildName[8];
};

void GS_DGAnsRegAttackGuild(LPBYTE lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_REGATTACKGUILD* lpMsg = (CSP_ANS_REGATTACKGUILD*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x0D] GS_DGAnsRegAttackGuild() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	GSProtocol.GCAnsRegCastleSiege(lpMsg->iIndex, lpMsg->iResult, lpMsg->szEnemyGuildName);
}

struct CSP_ANS_CASTLESIEGEEND {
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
};

void GS_DGAnsRestartCastleState(LPBYTE lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_CASTLESIEGEEND* lpMsg = (CSP_ANS_CASTLESIEGEEND*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x0E] GS_DGAnsRestartCastleState() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}
	else
	{
		g_CastleSiege.ResetCastleCycle();
	}
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : DataServer -> GameServer
 *  Code      : 0xC1
 *	HeadCode  :
 *	SubCode	  :
 */
struct CSP_ANS_MAPSVRMULTICAST
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	char szMsgText[128];	// 6
};

void GS_DGAnsMapSvrMsgMultiCast(LPBYTE lpRecv)
{
	CSP_ANS_MAPSVRMULTICAST * lpMsg = (CSP_ANS_MAPSVRMULTICAST *)lpRecv;

	if (lpMsg == NULL)
	{
		return;
	}

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x0F] GS_DGAnsMapSvrMsgMultiCast() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	char szNotice[144] = { 0 };
	memcpy(szNotice, lpMsg->szMsgText, 128);

	if (strlen(szNotice) > 1)
	{
		PMSG_NOTICE pNotice;

		TNotice::MakeNoticeMsg((TNotice *)&pNotice, 0, szNotice);
		TNotice::SetNoticeProperty((TNotice *)&pNotice, 0, _ARGB(255, 255, 200, 80), 1, 0, 20);
		TNotice::SendNoticeToAllUser((TNotice *)&pNotice);
	}

}

void GS_DGAnsGlobalPostMultiCast(LPBYTE lpRecv)
{
	CSP_GLOBALPOST_MULTICAST * lpMsg = (CSP_GLOBALPOST_MULTICAST *)lpRecv;

	if (lpMsg == NULL)
	{
		return;
	}

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("GS_DGAnsGlobalPostMultiCast() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	PMSG_POST_DATA pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xFA, 0x11, sizeof(pMsg));

	pMsg.btColorRGB[0] = lpMsg->btColorRGB[0];
	pMsg.btColorRGB[1] = lpMsg->btColorRGB[1];
	pMsg.btColorRGB[2] = lpMsg->btColorRGB[2];

	memcpy(pMsg.szName, lpMsg->szSender, MAX_ACCOUNT_LEN);
	memcpy(pMsg.szServerName, lpMsg->szServerName, 50);

	strcpy(pMsg.szMessage, lpMsg->szMessage);
	strcpy(pMsg.szTag, Lang.GetText(0, 433));

	GSProtocol.DataSendAll((LPBYTE)&pMsg, sizeof(pMsg));
}

void GS_DGAnsRegGuildMark(LPBYTE lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_GUILDREGMARK* lpMsg = (CSP_ANS_GUILDREGMARK*)lpRecv;

	if (lpMsg == NULL)
	{
		return;
	}

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x10] GS_DGAnsRegGuildMark() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}
	else
	{
		GSProtocol.GCAnsRegGuildMark(lpMsg->iIndex, lpMsg->iResult, lpMsg);
	}
}

struct CSP_ANS_GUILDRESETMARK {
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iIndex;
	/*<thisrel this+0x10>*/ /*|0x8|*/ char szGuildName[8];
	/*<thisrel this+0x18>*/ /*|0x4|*/ int iRegMarkCount;
};

void GS_DGAnsGuildMarkReset(LPBYTE lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_GUILDRESETMARK* lpMsg = (CSP_ANS_GUILDRESETMARK*)lpRecv;

	if (lpMsg == NULL)
	{
		return;
	}

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x11] GS_DGAnsGuildMarkReset() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}
}

struct CSP_ANS_GUILDSETGIVEUP {
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iIndex;
	/*<thisrel this+0x10>*/ /*|0x8|*/ char szGuildName[8];
	/*<thisrel this+0x18>*/ /*|0x4|*/ int bIsGiveUp;
	/*<thisrel this+0x1c>*/ /*|0x4|*/ int iRegMarkCount;
};

void GS_DGAnsGuildSetGiveUp(LPBYTE lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_GUILDSETGIVEUP* lpMsg = (CSP_ANS_GUILDSETGIVEUP*)lpRecv;

	if (lpMsg == NULL)
	{
		return;
	}

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x12] GS_DGAnsGuildSetGiveUp() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	GSProtocol.GCAnsGiveUpCastleSiege(lpMsg->iIndex, lpMsg->iResult, lpMsg->bIsGiveUp, lpMsg->iRegMarkCount, lpMsg->szGuildName);
}

struct CSP_ANS_NPCREMOVE {
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iNpcNumber;
	/*<thisrel this+0x10>*/ /*|0x4|*/ int iNpcIndex;
};

void GS_DGAnsNpcRemove(LPBYTE lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_NPCREMOVE* lpMsg = (CSP_ANS_NPCREMOVE*)lpRecv;

	if (lpMsg == NULL)
	{
		return;
	}

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x16] GS_DGAnsNpcRemove() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : DataServer -> GameServer
 *  Code      : 0xC1
 *	HeadCode  :
 *	SubCode	  :
 */
struct CSP_ANS_CASTLESTATESYNC
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iCastleState;	// 8
	int iTaxRateChaos;	// C
	int iTaxRateStore;	// 10
	int iTaxHuntZone;	// 14
	char szOwnerGuildName[8];	// 18
};

void GS_DGAnsCastleStateSync(LPBYTE lpRecv) //Identical gs-cs 56
{
	CSP_ANS_CASTLESTATESYNC * lpMsg = (CSP_ANS_CASTLESTATESYNC *)lpRecv;

	if (lpMsg == NULL)
	{
		return;
	}

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		return;
	}

	g_CastleSiegeSync.SetCastleState(lpMsg->iCastleState);
	g_CastleSiegeSync.SetTaxRateChaos(lpMsg->iTaxRateChaos);
	g_CastleSiegeSync.SetTaxRateStore(lpMsg->iTaxRateStore);
	g_CastleSiegeSync.SetTaxHuntZone(lpMsg->iTaxHuntZone);
	g_CastleSiegeSync.SetCastleOwnerGuild(lpMsg->szOwnerGuildName);
}

/* * * * * * * * * * * * * * * * * * * * *
 *	Mu Castle Siege          Packet
 *	Direction : DataServer -> GameServer
 *  Code      : 0xC1
 *	HeadCode  :
 *	SubCode	  :
 */
struct CSP_ANS_CASTLETRIBUTEMONEY
{
	PBMSG_HEAD2 h;
	int iResult;	// 4
	WORD wMapSvrNum;	// 8
};

void GS_DGAnsCastleTributeMoney(LPBYTE lpRecv)
{
	CSP_ANS_CASTLETRIBUTEMONEY * lpMsg = (CSP_ANS_CASTLETRIBUTEMONEY *)lpRecv;

	if (lpMsg == NULL)
	{
		return;
	}

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x18] GS_DGAnsCastleTributeMoney() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	sLog->outBasic("[CastleSiege] [0x80][0x18] GS_DGAnsCastleTributeMoney() - Money Tribute OK (%d)", g_CastleSiegeSync.GetTributeMoney());
	g_CastleSiegeSync.ResetTributeMoney();

}

struct CSP_ANS_RESETCASTLETAXINFO {
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;

};

void GS_DGAnsResetCastleTaxInfo(LPBYTE lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_RESETCASTLETAXINFO* lpMsg = (CSP_ANS_RESETCASTLETAXINFO*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x19] GS_DGAnsResetCastleTaxInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}
	else
	{
		if (lpMsg->iResult == 1)
		{
			g_CastleSiege.ResetCastleTaxInfo();
		}
	}
}

struct CSP_ANS_RESETSIEGEGUILDINFO {
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
};

void GS_DGAnsResetSiegeGuildInfo(LPBYTE lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_RESETSIEGEGUILDINFO* lpMsg = (CSP_ANS_RESETSIEGEGUILDINFO*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x1A] GS_DGAnsResetSiegeGuildInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}
}

struct CSP_ANS_RESETREGSIEGEINFO {
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
};

void GS_DGAnsResetRegSiegeInfo(LPBYTE lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_RESETREGSIEGEINFO* lpMsg = (CSP_ANS_RESETREGSIEGEINFO*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x1B] GS_DGAnsResetRegSiegeInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}
}

void GS_DGAnsCastleInitData(LPBYTE lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_CSINITDATA* lpMsg = (CSP_ANS_CSINITDATA*)lpRecv;
	CSP_CSINITDATA* lpMsgBody = (CSP_CSINITDATA*)(lpRecv + sizeof(CSP_ANS_CSINITDATA));

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x81] GS_DGAnsCastleInitData() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if (g_CastleSiege.GetDataLoadState() != 2)
	{
		sLog->outError("[CastleSiege] CASTLE SIEGE DATA SETTING FAILED [0x81] - m_iCastleDataLoadState != CASTLESIEGE_DATALOAD_2 (%d)", g_CastleSiege.GetDataLoadState());
		return;
	}

	g_CastleSiege.SetDataLoadState(3);

	if (lpMsg->iResult == FALSE)
	{
		sLog->outError("[CastleSiege] CASTLE SIEGE DATA SETTING FAILED [0x81] - lpMsg->iResult == 0");
		sLog->outError("[CastleSiege] CASTLE SIEGE DATA SETTING FAILED [0x81] - lpMsg->iResult == 0");
		return;
	}

	BOOL bRET_VAL = FALSE;

	bRET_VAL = g_CastleSiege.SetCastleInitData(lpMsg);

	if (bRET_VAL == FALSE)
	{
		sLog->outError("[CastleSiege] CASTLE SIEGE DATA SETTING FAILED [0x81] - g_CastleSiege.SetCastleInitData() == FALSE");
		return;
	}

	bRET_VAL = g_CastleSiege.SetCastleNpcData(lpMsgBody, lpMsg->iCount);

	if (bRET_VAL == FALSE)
	{
		sLog->outError("[CastleSiege] CASTLE SIEGE DATA SETTING FAILED [0x81] - g_CastleSiege.SetCastleNpcData() == FALSE");
		return;
	}

	g_CastleSiege.SetDbDataLoadOK(TRUE);

	g_CastleSiege.SetDataLoadState(4);

	if (lpMsg->iFirstCreate == 1)
	{
		g_CastleSiege.FirstCreateDbNPC();
	}

	g_CastleSiege.Init();
}

struct CSP_NPCDATA {
	/*<thisrel this+0x0>*/ /*|0x4|*/ int iNpcNumber;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iNpcIndex;
	/*<thisrel this+0x8>*/ /*|0x4|*/ int iNpcDfLevel;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iNpcRgLevel;
	/*<thisrel this+0x10>*/ /*|0x4|*/ int iNpcMaxHp;
	/*<thisrel this+0x14>*/ /*|0x4|*/ int iNpcHp;
	/*<thisrel this+0x18>*/ /*|0x1|*/ BYTE btNpcX;
	/*<thisrel this+0x19>*/ /*|0x1|*/ BYTE btNpcY;
	/*<thisrel this+0x1a>*/ /*|0x1|*/ BYTE btNpcDIR;
};

void GS_DGAnsCastleNpcInfo(LPBYTE lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_NPCDATA* lpMsg = (CSP_ANS_NPCDATA*)lpRecv;
	CSP_NPCDATA* lpMsgBody = (CSP_NPCDATA*)(lpRecv + sizeof(CSP_ANS_NPCDATA));

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x82] GS_DGAnsCastleNpcInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}
}

struct CSP_ANS_ALLGUILDREGINFO { // <size 0x14>
	/*<thisrel this+0x0>*/ /*|0x4|*/ PWMSG_HEAD h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iIndex;
	/*<thisrel this+0x10>*/ /*|0x4|*/ int iCount;
};
struct CSP_GUILDREGINFO {// <size 0x10>
	/*<thisrel this+0x0>*/ /*|0x8|*/ char szGuildName[8];
	/*<thisrel this+0x8>*/ /*|0x4|*/ int iRegMarkCount;
	/*<thisrel this+0xc>*/ /*|0x1|*/ BYTE bIsGiveUp;
	/*<thisrel this+0xd>*/ /*|0x1|*/ BYTE btRegRank;
};
struct PMSG_ANS_CSREGGUILDLIST {// <size 0xc>
	/*<thisrel this+0x0>*/ /*|0x4|*/ PWMSG_HEAD h;
	/*<thisrel this+0x4>*/ /*|0x1|*/ BYTE btResult;
	/*<thisrel this+0x8>*/ /*|0x4|*/ int iCount;
};
struct PMSG_CSREGGUILDLIST {// <size 0xe>
	/*<thisrel this+0x0>*/ /*|0x8|*/ char szGuildName[8];
	/*<thisrel this+0x8>*/ /*|0x1|*/ BYTE btRegMarks1;
	/*<thisrel this+0x9>*/ /*|0x1|*/ BYTE btRegMarks2;
	/*<thisrel this+0xa>*/ /*|0x1|*/ BYTE btRegMarks3;
	/*<thisrel this+0xb>*/ /*|0x1|*/ BYTE btRegMarks4;
	/*<thisrel this+0xc>*/ /*|0x1|*/ BYTE btIsGiveUp;
	/*<thisrel this+0xd>*/ /*|0x1|*/ BYTE btSeqNum;
};

void GS_DGAnsAllGuildMarkRegInfo(LPBYTE lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_ALLGUILDREGINFO* lpMsg = (CSP_ANS_ALLGUILDREGINFO*)lpRecv;
	CSP_GUILDREGINFO* lpMsgBody = (CSP_GUILDREGINFO*)(lpRecv + sizeof(CSP_ANS_ALLGUILDREGINFO));
	char cBUFFER[1668];
	PMSG_ANS_CSREGGUILDLIST* lpMsgSend;
	PMSG_CSREGGUILDLIST* lpMsgSendBody;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x83] GS_DGAnsAllGuildMarkRegInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if (!gObjIsConnected(lpMsg->iIndex))
		return;

	lpMsgSend = (PMSG_ANS_CSREGGUILDLIST*)cBUFFER;
	lpMsgSendBody = (PMSG_CSREGGUILDLIST*)&cBUFFER[12];

	lpMsgSend->iCount = 0;
	lpMsgSend->btResult = lpMsg->iResult;

	if (lpMsg->iResult == TRUE)
	{
		lpMsgSend->iCount = lpMsg->iCount;

		for (int i = 0; i < lpMsg->iCount; i++)
		{
			lpMsgSendBody[i].btSeqNum = lpMsgBody[i].btRegRank;
			lpMsgSendBody[i].btIsGiveUp = lpMsgBody[i].bIsGiveUp;
			lpMsgSendBody[i].btRegMarks1 = SET_NUMBERH(SET_NUMBERHW(lpMsgBody[i].iRegMarkCount));
			lpMsgSendBody[i].btRegMarks2 = SET_NUMBERL(SET_NUMBERHW(lpMsgBody[i].iRegMarkCount));
			lpMsgSendBody[i].btRegMarks3 = SET_NUMBERH(SET_NUMBERLW(lpMsgBody[i].iRegMarkCount));
			lpMsgSendBody[i].btRegMarks4 = SET_NUMBERL(SET_NUMBERLW(lpMsgBody[i].iRegMarkCount));
			memcpy(&lpMsgSendBody[i].szGuildName, lpMsgBody[i].szGuildName, sizeof(lpMsgSendBody[i].szGuildName));
		}
	}

	lpMsgSend->h.set((LPBYTE)&cBUFFER, 0xB4, (lpMsgSend->iCount * sizeof(PMSG_CSREGGUILDLIST) + sizeof(PMSG_ANS_CSREGGUILDLIST)));

	IOCP.DataSend(lpMsg->iIndex, (LPBYTE)&cBUFFER, (lpMsgSend->iCount * sizeof(PMSG_CSREGGUILDLIST) + sizeof(PMSG_ANS_CSREGGUILDLIST)));
}

struct CSP_ANS_NPCSAVEDATA {

	// static data ------------------------------------

	// non-static data --------------------------------
	/*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
};

void GS_DGAnsFirstCreateNPC(LPBYTE lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_NPCSAVEDATA* lpMsg = (CSP_ANS_NPCSAVEDATA*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x84] GS_DGAnsFirstCreateNPC() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if (lpMsg->iResult == TRUE)
	{
		if (g_CastleSiege.GetDbNpcCreated() == FALSE)
		{
			g_CastleSiege.CreateDbNPC();
			g_CastleSiege.SetDbNpcCreated(1);
		}
	}
}
struct CSP_ANS_CALCREGGUILDLIST {// <size 0x10>
	/*<thisrel this+0x0>*/ /*|0x4|*/ PWMSG_HEAD h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iCount;
};

void GS_DGAnsCalcREgGuildList(LPBYTE lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_CALCREGGUILDLIST* lpMsg = (CSP_ANS_CALCREGGUILDLIST*)lpRecv;
	CSP_CALCREGGUILDLIST* lpMsgBody = (CSP_CALCREGGUILDLIST*)(lpRecv + sizeof(CSP_ANS_CALCREGGUILDLIST));

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x85] GS_DGAnsCalcREgGuildList() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if (lpMsg->iResult == TRUE)
	{
		g_CastleSiege.SetCalcRegGuildList(lpMsgBody, lpMsg->iCount);
		return;
	}

	sLog->outError("[CastleSiege] PACKET-ERROR [0x85] GS_DGAnsCalcREgGuildList() - lpMsg->iResult != 1 (%d)", lpMsg->iResult);
}

struct CSP_ANS_CSGUILDUNIONINFO {// <size 0x10>
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PWMSG_HEAD h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iCount;
};

void GS_DGAnsCsGulidUnionInfo(LPBYTE lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_CSGUILDUNIONINFO* lpMsg = (CSP_ANS_CSGUILDUNIONINFO*)lpRecv;
	CSP_CSGUILDUNIONINFO* lpMsgBody = (CSP_CSGUILDUNIONINFO*)(lpRecv + sizeof(CSP_ANS_CSGUILDUNIONINFO));

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x86] GS_DGAnsCsGulidUnionInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if (lpMsg->iResult == TRUE)
	{
		g_CastleSiege.MakeCsTotalGuildInfo(lpMsgBody, lpMsg->iCount);
		return;
	}
}

struct CSP_ANS_CSSAVETOTALGUILDINFO {// <size 0xc>
	/*<thisrel this+0x0>*/ /*|0x3|*/ PBMSG_HEAD h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
};

void GS_DGAnsCsSaveTotalGuildInfo(LPBYTE lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_CSSAVETOTALGUILDINFO* lpMsg = (CSP_ANS_CSSAVETOTALGUILDINFO*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x87] GS_DGAnsCsSaveTotalGuildInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if (lpMsg->iResult == TRUE)
	{
		g_CastleSiege.SetIsSiegeGuildList(TRUE);
		return;
	}
	g_CastleSiege.SetIsSiegeGuildList(FALSE);
}

struct CSP_ANS_CSLOADTOTALGUILDINFO {// <size 0x10>
	/*<thisrel this+0x0>*/ /*|0x4|*/ PWMSG_HEAD h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iCount;
};

void GS_DGAnsCsLoadTotalGuildInfo(LPBYTE lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_CSLOADTOTALGUILDINFO* lpMsg = (CSP_ANS_CSLOADTOTALGUILDINFO*)lpRecv;
	CSP_CSLOADTOTALGUILDINFO* lpMsgBody = (CSP_CSLOADTOTALGUILDINFO*)(lpRecv + sizeof(CSP_ANS_CSLOADTOTALGUILDINFO));

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x88] GS_DGAnsCsLoadTotalGuildInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if (lpMsg->iResult == TRUE)
	{
		g_CastleSiege.SetCsTotalGuildInfo(lpMsgBody, lpMsg->iCount);
		return;
	}
}

struct CSP_ANS_NPCUPDATEDATA {// <size 0xc>
	/*<thisrel this+0x0>*/ /*|0x3|*/ PBMSG_HEAD h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
};

void GS_DGAnsCastleNpcUpdate(LPBYTE lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_NPCUPDATEDATA* lpMsg = (CSP_ANS_NPCUPDATEDATA*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x89] GS_DGAnsCastleNpcUpdate() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	char* szResult;

	if (lpMsg->iResult != FALSE)
	{
		szResult = "SUCCEED";
	}
	else
	{
		szResult = "FAIL";
	}

	sLog->outBasic("[CastleSiege] [0x89] GS_DGAnsCastleNpcUpdate() - Npc Data Update Result : '%s'", szResult);
}



struct CWP_REQ_CRYWOLFSYNC
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
	int iCrywolfState;	// 8
	int iOccupationState;	// C
	int iApplyBenefit;
	int iApplyPenalty;
	int iPlusChaosRate;
	int iGemDropPenaltyRate;
	int iGettingExpPenaltyRate;
	int iMinusMonHPBenefitRate;
	int iKundunHPRefillState;
};

void GDReqCrywolfSync(int iMapSvrGroup, int iCrywolfState, int iOccupationState)
{
	if (iMapSvrGroup < 0)
		return;

	CWP_REQ_CRYWOLFSYNC pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0xB0, sizeof(CWP_REQ_CRYWOLFSYNC));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iCrywolfState = iCrywolfState;
	pMsg.iOccupationState = iOccupationState;
	pMsg.iApplyBenefit = g_CrywolfSync.GetApplyBenefit();
	pMsg.iApplyPenalty = g_CrywolfSync.GetApplyPenalty();
	pMsg.iPlusChaosRate = g_CrywolfSync.GetPlusChaosRate();
	pMsg.iGemDropPenaltyRate = g_CrywolfSync.GetGemDropPenaltiyRate(); // zapisz, bo mi dzis prad wylaczaja czesto k
	pMsg.iGettingExpPenaltyRate = g_CrywolfSync.GetGettingExpPenaltyRate();
	pMsg.iMinusMonHPBenefitRate = g_CrywolfSync.GetMonHPBenefitRate();
	pMsg.iKundunHPRefillState = g_CrywolfSync.GetKundunHPRefillState();

	wsDataCli.DataSend((PCHAR)&pMsg, pMsg.h.size);
}



struct CWP_ANS_CRYWOLFSYNC
{
	PBMSG_HEAD h;
	BYTE btResult;	// 3
	int iCrywolfState;	// 4
	int iOccupationState;	// 8
	int iApplyBenefit;
	int iApplyPenalty;
	int iPlusChaosRate;
	int iGemDropPenaltyRate;
	int iGettingExpPenaltyRate;
	int iMinusMonHPBenefitRate;
	int iKundunHPRefillState;
};


DWORD dwCrywolfSyncLogTickCount = 0;

void DGAnsCrywolfSync(LPBYTE lpRecv)
{
	CWP_ANS_CRYWOLFSYNC * lpMsg = (CWP_ANS_CRYWOLFSYNC *)lpRecv;

	if (lpMsg == NULL)
		return;

	g_CrywolfSync.SetApplyBenefit(lpMsg->iApplyBenefit);
	g_CrywolfSync.SetApplyPenalty(lpMsg->iApplyPenalty);
	g_CrywolfSync.SetPlusChaosRate(lpMsg->iPlusChaosRate);
	g_CrywolfSync.SetGemDropPenaltiyRate(lpMsg->iGemDropPenaltyRate);
	g_CrywolfSync.SetGettingExpPenaltyRate(lpMsg->iGettingExpPenaltyRate);
	g_CrywolfSync.SetMonHPBenefitRate(lpMsg->iMinusMonHPBenefitRate);
	g_CrywolfSync.SetKundunHPRefillState(lpMsg->iKundunHPRefillState);

	int iOldOccupationState = g_CrywolfSync.GetOccupationState();
	g_CrywolfSync.SetCrywolfState(lpMsg->iCrywolfState);
	g_CrywolfSync.SetOccupationState(lpMsg->iOccupationState);

	if (GetTickCount() > (dwCrywolfSyncLogTickCount + 60000))
	{
		g_Log.AddC(TColor::Blue, "[Crywolf Sync] Occupation: %d, State: %d, Benefit: %d, Penalty: %d, PlusChaosRate: %d, GemDropPenaltyRate: %d, ExpPenaltyRate: %d, MinusMonHPBenefitRate: %d, KundunHPRefillState: %d",
			lpMsg->iOccupationState, lpMsg->iCrywolfState, lpMsg->iApplyBenefit, lpMsg->iApplyPenalty, lpMsg->iPlusChaosRate, lpMsg->iGemDropPenaltyRate, lpMsg->iGettingExpPenaltyRate, lpMsg->iMinusMonHPBenefitRate, lpMsg->iKundunHPRefillState);
		dwCrywolfSyncLogTickCount = GetTickCount();
	}
}

struct CWP_REQ_CRYWOLFINFOLOAD
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
};

void GDReqCrywolfInfoLoad(int iMapSvrGroup)
{
	if (iMapSvrGroup < 0)
		return;

	CWP_REQ_CRYWOLFINFOLOAD pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0xB1, sizeof(CWP_REQ_CRYWOLFINFOLOAD));
	pMsg.wMapSvrNum = iMapSvrGroup;

	wsDataCli.DataSend((PCHAR)&pMsg, pMsg.h.size);
}


struct CWP_ANS_CRYWOLFINFOLOAD
{
	PBMSG_HEAD h;
	BYTE btResult;	// 3
	int iCrywolfState;	// 4
	int iOccupationState;	// 8
};

void DGAnsCrywolfInfoLoad(LPBYTE lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CWP_ANS_CRYWOLFINFOLOAD * lpMsg = (CWP_ANS_CRYWOLFINFOLOAD *)lpRecv;

	if (lpMsg == NULL)
	{
		return;
	}

	int iResult;

	if (true != FALSE && lpMsg->iOccupationState >= 0 && lpMsg->iOccupationState <= 2)
	{
		iResult = 1;
	}
	else
	{
		iResult = 0;
	}

	lpMsg->btResult = iResult;

	if ((iResult & 0xFF) != 0)
	{
		g_Crywolf.ApplyCrywolfDBInfo(lpMsg->iCrywolfState, lpMsg->iOccupationState);
		return;
	}
}

struct CWP_REQ_CRYWOLFINFOSAVE
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
	int iCrywolfState;	// 8
	int iOccupationState;	// C
};


void GDReqCrywolfInfoSave(int iMapSvrGroup, int iCrywolfState, int iOccupationState)
{
	if (iMapSvrGroup < 0)
		return;

	CWP_REQ_CRYWOLFINFOSAVE pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0xB2, sizeof(CWP_REQ_CRYWOLFINFOSAVE));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iCrywolfState = iCrywolfState;
	pMsg.iOccupationState = iOccupationState;

	wsDataCli.DataSend((PCHAR)&pMsg, pMsg.h.size);
}



struct CWP_ANS_CRYWOLFINFOSAVE
{
	PBMSG_HEAD h;
	BYTE btResult;	// 3
};

void DGAnsCrywolfInfoSave(LPBYTE lpRecv)
{
	CWP_ANS_CRYWOLFINFOSAVE * lpMsg = (CWP_ANS_CRYWOLFINFOSAVE *)lpRecv;
}

void GDReqInGameShopItemList(CGameObject* lpObj, BYTE InventoryType, BYTE InventoryNum)
{
	ISHOP_REQ_ITEMLIST pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xD2, 0x02, sizeof(pMsg));

	memcpy(pMsg.AccountID, lpObj->m_PlayerData->ConnectUser->AccountID, 11);
	pMsg.aIndex = aIndex;
	pMsg.InvType = InventoryType;
	pMsg.InvNum = InventoryNum;

	wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg));
}

void DGAnsInGameShopItemList(LPBYTE lpRecv)
{
	ISHOP_ANS_ITEMLIST * lpMsg = (ISHOP_ANS_ITEMLIST *)(lpRecv);
	ISHOP_ITEMLIST * lpMsgBody = (ISHOP_ITEMLIST *)(lpRecv + sizeof(ISHOP_ANS_ITEMLIST));

	LPOBJ lpObj = &gObj[lpMsg->aIndex];

	g_CashShop.GCCashInventoryItemCount(lpObj, lpRecv);
}

void GDReqInGameShopPoint(CGameObject* lpObj)
{
	ISHOP_REQ_POINT pMsg = { 0 };

	PHeadSetB((LPBYTE)&pMsg, 0xD1, sizeof(pMsg));
	memcpy(pMsg.AccountID, lpObj->m_PlayerData->ConnectUser->AccountID, 11);
	pMsg.aIndex = aIndex;

	wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg));
}

void DGAnsInGameShopPoint(ISHOP_ANS_POINT *lpMsg)
{
	if (lpMsg->aIndex < g_ConfigRead.server.GetObjectStartUserIndex() || lpMsg->aIndex > g_ConfigRead.server.GetObjectMax())
		return;

	gObj[lpMsg->aIndex].m_PlayerData->m_WCoinC = lpMsg->CoinC;
	gObj[lpMsg->aIndex].m_PlayerData->m_WCoinP = lpMsg->CoinP;
	gObj[lpMsg->aIndex].m_PlayerData->m_GoblinPoint = lpMsg->Goblin;

	g_CashShop.GCCashPoint(&gObj[lpMsg->aIndex]);
}

void GDReqInGameShopPointAdd(CGameObject* lpObj, BYTE Type, float Coin)
{
	ISHOP_POINT_ADD pMsg = { 0 };

	PHeadSetB((LPBYTE)&pMsg, 0xD7, sizeof(pMsg));

	pMsg.aIndex = aIndex;
	memcpy(pMsg.AccountID, lpObj->m_PlayerData->ConnectUser->AccountID, 11);
	pMsg.Type = Type;
	pMsg.Coin = Coin;

	wsDataCli.DataSend((PCHAR)&pMsg, pMsg.h.size);
}

void DGAnsInGameShopItemBuy(LPBYTE lpRecv)
{
	ISHOP_ITEM_BUYANS * pItem = (ISHOP_ITEM_BUYANS *)(lpRecv);

	g_CashShop.GCCashItemBuy(&gObj[pItem->aIndex], pItem->ID1, pItem->ID2, pItem->ID3, pItem->Result);
}

void GDReqInGameShopItemBuy(CGameObject* lpObj, int ID1, int ID2, int ID3, int Price, BYTE CoinType)
{
	ISHOP_ITEM_BUY pMsg;
	PHeadSetB((LPBYTE)&pMsg, 0xD5, sizeof(pMsg));

	pMsg.aIndex = aIndex;
	memcpy(pMsg.AccountID, lpObj->m_PlayerData->ConnectUser->AccountID, 11);
	pMsg.ID1 = ID1;
	pMsg.ID2 = ID2;
	pMsg.ID3 = ID3;
	pMsg.Price = Price;
	pMsg.CoinType = CoinType;

	wsDataCli.DataSend((PCHAR)&pMsg, pMsg.h.size);
}

void GDReqInGameShopItemUse(CGameObject* lpObj, int UniqueCode, int AuthCode)
{
	ISHOP_ITEM_USE pMsg;
	PHeadSetB((LPBYTE)&pMsg, 0xD9, sizeof(pMsg));

	pMsg.aIndex = aIndex;
	memcpy(pMsg.AccountID, lpObj->m_PlayerData->ConnectUser->AccountID, 11);
	pMsg.UniqueCode = UniqueCode;
	pMsg.AuthCode = AuthCode;

	wsDataCli.DataSend((PCHAR)&pMsg, pMsg.h.size);
}

void DGAnsInGameShopItemUse(ISHOP_ITEM_USEANS * lpMsg)
{
	if (!ObjectMaxRange(lpMsg->aIndex))
		return;

	g_CashShop.GCCashInventoryItemUse(&gObj[lpMsg->aIndex], lpMsg->Result, lpMsg->UniqueCode, lpMsg->AuthCode, lpMsg->ID1, lpMsg->ID2, lpMsg->ID3);
}

void GDReqInGameShopItemGift(CGameObject* lpObj, int ID1, int ID2, int ID3, int Price, BYTE CoinType, char Target[11], char Message[200])
{
	ISHOP_ITEM_GIFT pMsg;
	PHeadSetW((LPBYTE)&pMsg, 0xD6, sizeof(pMsg));

	pMsg.aIndex = aIndex;
	memcpy(pMsg.AccountID, lpObj->m_PlayerData->ConnectUser->AccountID, 11);
	pMsg.ID1 = ID1;
	pMsg.ID2 = ID2;
	pMsg.ID3 = ID3;
	pMsg.Price = Price;
	pMsg.CoinType = CoinType;
	memcpy(pMsg.TargetName, Target, 11);
	memcpy(pMsg.Name, lpObj->Name, 11);
	memcpy(pMsg.Message, Message, 200);

	wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg));
}

void DGAnsInGameShopItemGift(LPBYTE lpRecv)
{
	ISHOP_ITEM_GIFTANS * pItem = (ISHOP_ITEM_GIFTANS *)(lpRecv);

	g_CashShop.GCCashItemGift(&gObj[pItem->aIndex], pItem->ID1, pItem->ID2, pItem->ID3, pItem->Result);
}

void GDReqInGameShopItemDelete(CGameObject* lpObj, int UniqueCode, int AuthCode)
{
	ISHOP_ITEM_DELETE pMsg;
	PHeadSetB((LPBYTE)&pMsg, 0xDB, sizeof(pMsg));

	memcpy(pMsg.AccountID, lpObj->m_PlayerData->ConnectUser->AccountID, MAX_ACCOUNT_LEN + 1);
	pMsg.UniqueCode = UniqueCode;
	pMsg.AuthCode = AuthCode;

	wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg));
}

void GDReqInGameShopItemRollbackUse(CGameObject* lpObj, int UniqueCode, int AuthCode)
{
	ISHOP_ITEM_ROLLBACK pMsg;
	PHeadSetB((LPBYTE)&pMsg, 0xDC, sizeof(pMsg));

	memcpy(pMsg.AccountID, lpObj->m_PlayerData->ConnectUser->AccountID, MAX_ACCOUNT_LEN + 1);
	pMsg.UniqueCode = UniqueCode;
	pMsg.AuthCode = AuthCode;

	wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg));
}

void GDReqBanUser(CGameObject* lpObj, BYTE Type, BYTE Ban)
{
	BAN_REQ_USER pBan;

	PHeadSetB((LPBYTE)&pBan, 0xFA, sizeof(pBan));

	pBan.Type = Type;
	pBan.Ban = Ban;

	if (pBan.Type == 1)
	{
		memcpy(pBan.AccName, lpObj->m_PlayerData->ConnectUser->AccountID, 11);
	}
	else
	{
		memcpy(pBan.AccName, lpObj->Name, 11);
	}

	wsDataCli.DataSend((PCHAR)&pBan, pBan.h.size);
}

struct SDHP_EXPANDEDWAREHOUSE_SET
{
	PBMSG_HEAD h;
	char AccountID[11];
	BYTE ExpansionType;
	BYTE ExpansionLevel;
};

void GDSetExpWare(CGameObject* lpObj, BYTE ExpansionType, BYTE ExpansionLevel)
{
	SDHP_EXPANDEDWAREHOUSE_SET pMsg;
	PHeadSetB((LPBYTE)&pMsg, 0xDA, sizeof(pMsg));

	pMsg.ExpansionType = ExpansionType;

	if (pMsg.ExpansionType == 2)
	{
		memcpy(pMsg.AccountID, lpObj->m_PlayerData->ConnectUser->AccountID, 11);
	}
	else
	{
		memcpy(pMsg.AccountID, lpObj->Name, 11);
	}

	pMsg.ExpansionLevel = ExpansionLevel;

	wsDataCli.DataSend((PCHAR)&pMsg, pMsg.h.size);
}

void GDCharCardBuy(char AccountID[11], BYTE Type)
{
	ISHOP_CHARCARD_BUY pCard;

	PHeadSetB((LPBYTE)&pCard, 0xD4, sizeof(pCard));

	memcpy(pCard.AccountID, AccountID, 11);

	pCard.Type = Type;

	wsDataCli.DataSend((PCHAR)&pCard, pCard.h.size);
}

void DGSetWarehouseList(PMSG_ANS_WARESAVE *lpMsg)
{
	if (!ObjectMaxRange(lpMsg->iIndex))
	{
		return;
	}

	if (gObj[lpMsg->iIndex].Type != OBJ_USER)
	{
		return;
	}

	gObj[lpMsg->iIndex].WarehouseSaveLock = FALSE;

	if (lpMsg->CloseWindow == TRUE)
	{
		gObj[lpMsg->iIndex].m_btOpenWarehouse = FALSE;
		gObj[lpMsg->iIndex].WarehouseTick = GetTickCount();

		if (gObj[lpMsg->iIndex].m_IfState.use == 1 && gObj[lpMsg->iIndex].m_IfState.type == 6)
		{
			gObj[lpMsg->iIndex].m_IfState.use = 0;
			gObj[lpMsg->iIndex].m_IfState.type = 0;
			gObj[lpMsg->iIndex].m_IfState.state = 0;
		}
	}

}

void GDReqSecLock(CGameObject* lpObj, int Code)
{
	SECLOCK_REQ_SAVE pLock;

	PHeadSetB((LPBYTE)&pLock, 0xFB, sizeof(pLock));

	memcpy(pLock.AccountID, lpObj->m_PlayerData->ConnectUser->AccountID, 11);

	pLock.Code = Code;

	wsDataCli.DataSend((char *)&pLock, pLock.h.size);
}

void ReqMonsterCount(CGameObject* lpObj)
{
	DS_GET_MONSTERCNT pMsg;
	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xFD;
	pMsg.h.size = sizeof(pMsg);
	pMsg.aIndex = aIndex;
	memcpy(pMsg.character, lpObj->Name, 10);
	pMsg.character[10] = 0;
	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);

}

void ReqMonsterCountAns(DS_GET_MONSTERCNT * aRecv)
{
	if (gObj[aRecv->aIndex].Connected == PLAYER_PLAYING && gObj[aRecv->aIndex].Type == OBJ_USER)
	{
		gObj[aRecv->aIndex].m_PlayerData->IGC_KillCount = aRecv->cnt;
	}
}

void ReqSaveMonsterCount(short aIndex, int monsterid)
{
	DS_SAVE_MONSTERCNT pMsg;
	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xFC;
	pMsg.h.size = sizeof(pMsg);
	pMsg.type = monsterid;
	memcpy(pMsg.character, lpObj->Name, 10);
	pMsg.character[10] = 0;
	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
}

void ReqSavePlayerKiller(short kIndex, short vIndex)
{
	DS_SAVE_PLAYERKILLER pMsg;
	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xFE;
	pMsg.h.size = sizeof(pMsg);
	memcpy(pMsg.Victim, gObj[vIndex].Name, 10);
	pMsg.Victim[10] = 0;
	memcpy(pMsg.Killer, gObj[kIndex].Name, 10);
	pMsg.Killer[10] = 0;
	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
}

void GDReqArcaBattleGuildJoin(CGameObject* lpObj)
{
	PMSG_REQ_ARCA_BATTLE_GUILD_JOIN_DS pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xF8;
	pMsg.h.subcode = 0x30;
	memcpy(pMsg.szGuildMaster, obj->Name, MAX_ACCOUNT_LEN + 1);
	memcpy(pMsg.szGuildName, obj->m_PlayerData->lpGuild->Name, MAX_GUILD_LEN + 1);
	pMsg.wNumber = obj->m_Index;
	pMsg.dwGuild = obj->m_PlayerData->GuildNumber;
	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

void DGAnsArcaBattleGuildJoin(PMSG_ANS_ARCA_BATTLE_GUILD_JOIN_DS *lpMsg)
{
	PMSG_ANS_ARCA_BATTLE_JOIN pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xF8;
	pMsg.h.subcode = 0x31;
	pMsg.btResult = lpMsg->btResult;
	IOCP.DataSend(lpMsg->wNumber, (LPBYTE)&pMsg, sizeof(pMsg));
}

void DGAnsArcaBattleGuildJoinSelect(PMSG_ANS_ARCA_BATTLE_GUILD_JOIN_DS *lpMsg)
{
	PMSG_ANS_ARCA_BATTLE_JOIN pMsg;
	CGameObject* lpObj = lpMsg->wNumber;

	if (ObjectMaxRange(lpObj) == false)
		return;

	if (lpObj->Type != OBJ_USER)
		return;

	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnected(lpObj))
	{
		switch (lpMsg->btResult)
		{
		case 0:
			sLog->outBasic("[ArcaBattle] [%s][%s][%s] ArcaBattle Guild Join", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->GuildName);
			GDReqArcaBattleGuildJoin(lpObj);
			return;

		case 1:
			sLog->outBasic("[ArcaBattle] [%s][%s] ArcaBattle Join Fail to Not Guild Master", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
			break;

		case 2:
			sLog->outBasic("[ArcaBattle] [%s][%s] ArcaBattle Join Fail to Below Guild Member", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
			break;

		case 3:
			sLog->outBasic("[ArcaBattle] [%s][%s] ArcaBattle Join Fail to Guild Over", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
			break;

		case 4:
			sLog->outBasic("[ArcaBattle] [%s][%s] ArcaBattle Join Fail to be Guild Name Same", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
			break;

		case 5:
			sLog->outBasic("[ArcaBattle] [%s][%s] (Guild Master) ArcaBattle Join Fail to Time Over", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
			break;

		case 6:
			sLog->outBasic("[ArcaBattle] [%s][%s] ArcaBattle Join Fail to not enough Guild Rank", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
			break;

		default:
			sLog->outBasic("[ArcaBattle] [%s][%s] GuildJoin Error Result : %d", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpMsg->btResult);
			break;
		}

		pMsg.h.c = 0xC1;
		pMsg.h.size = sizeof(pMsg);
		pMsg.h.headcode = 0xF8;
		pMsg.h.subcode = 0x31;
		pMsg.btResult = lpMsg->btResult;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));
	}
}

void GDReqArcaBattleGuildMemberJoin(CGameObject* lpObj)
{
	PMSG_REQ_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xF8;
	pMsg.h.subcode = 0x32;
	memcpy(pMsg.szCharName, obj->Name, MAX_ACCOUNT_LEN + 1);
	memcpy(pMsg.szGuildName, obj->m_PlayerData->lpGuild->Name, MAX_GUILD_LEN + 1);
	pMsg.dwGuild = obj->m_PlayerData->GuildNumber;
	pMsg.wNumber = obj->m_Index;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

void DGAnsArcaBattleGuildMemberJoin(PMSG_ANS_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS *lpMsg)
{
	PMSG_ANS_ARCA_BATTLE_JOIN pMsg;
	CGameObject* lpObj = lpMsg->wNumber;

	if (ObjectMaxRange(lpObj) == false)
		return;

	if (lpObj->Type != OBJ_USER)
		return;

	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnected(lpObj))
	{
		switch (lpMsg->btResult)
		{
		case 0:
			sLog->outBasic("[ArcaBattle] [%s][%s][%s] ArcaBattle Guild Member Join", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->GuildName);
			return;

		case 7:
			sLog->outBasic("[ArcaBattle] [%s][%s] ArcaBattle Join Fail to Not Guild Member", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
			break;

		case 8:
			sLog->outBasic("[ArcaBattle] [%s][%s] ArcaBattle Join Fail to be Name Same", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
			break;

		case 9:
			sLog->outBasic("[ArcaBattle] [%s][%s] (Guild Member) ArcaBattle Join Fail to Guild Member Over", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
			break;

		case 10:
			sLog->outBasic("[ArcaBattle] [%s][%s] (Guild Member) ArcaBattle Join Fail to Time Over", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
			break;

		case 13:
			sLog->outBasic("[ArcaBattle] [%s][%s] (Guild Master) ArcaBattle Join Fail GuildMaster Will be Registered Automatically", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
			break;

		case 14:
			sLog->outBasic("[ArcaBattle] [%s][%s] ArcaBattle Join Fail to Guild Master Join Time ", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
			break;

		default:
			sLog->outBasic("[ArcaBattle] [%s][%s] GuildMemberJoin Error Result : %d", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpMsg->btResult);
			break;
		}

		pMsg.h.c = 0xC1;
		pMsg.h.size = sizeof(pMsg);
		pMsg.h.headcode = 0xF8;
		pMsg.h.subcode = 0x33;
		pMsg.btResult = lpMsg->btResult;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));
	}
}

void GDReqArcaBattleEnter(CGameObject* lpObj, BYTE btEnterSeq)
{
	PMSG_REQ_ARCA_BATTLE_ENTER_DS pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xF8;
	pMsg.h.subcode = 0x34;
	pMsg.h.size = sizeof(pMsg);
	memcpy(pMsg.szCharName, obj->Name, MAX_ACCOUNT_LEN + 1);
	pMsg.wNumber = obj->m_Index;
	pMsg.btEnterSeq = btEnterSeq;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

void DGAnsArcaBattleEnter(PMSG_ANS_ARCA_BATTLE_ENTER_DS *lpMsg)
{
	CGameObject* lpObj = lpMsg->wNumber;

	if (gObjIsConnected(lpObj) == FALSE)
	{
		return;
	}

	if (lpObj->Teleport != FALSE)
	{
		return;
	}

	if (lpMsg->btResult == 11)
	{
		sLog->outBasic("[ArcaBattle] [%s][%s] ArcaBattle Enter Fail to Not Join Guild Master", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
	}

	else if (lpMsg->btResult == 12)
	{
		sLog->outBasic("[ArcaBattle] [%s][%s] ArcaBattle Enter Fail to Not Play Time", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
	}

	else if (lpMsg->btResult == 0)
	{
		if (lpMsg->btEnterSeq == TRUE)
		{
			gObjMoveGate(lpObj, 426);
			sLog->outBasic("[ArcaBattle] [%s][%s][%s] ArcaBattle Enter", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->GuildName);
			return;
		}
	}

	else
	{
		sLog->outBasic("[ArcaBattle] [%s][%s] Enter Error Result : %d", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpMsg->btResult);
		return;
	}

	PMSG_ANS_ARCA_BATTLE_JOIN pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xF8;
	pMsg.h.subcode = 0x35;
	pMsg.btResult = lpMsg->btResult;
	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));
}

void DGAnsArcaBattleWinGuildInfo(PMSG_ANS_AB_WIN_GUILD_INFO_DS *lpMsg)
{
	g_ArcaBattle.AddArcaBattleWinGuildInfo(lpMsg->m_stABWinGuildInfoDS, lpMsg->btGuildCnt);
}

void DGReqArcaBattleProcMultiCast(PMSG_ANS_AB_PROC_STATE_DS *lpMsg)
{
	if (!g_ArcaBattle.IsArcaBattleServer())
	{
		g_ArcaBattle.SetState(lpMsg->btProcState);
	}

	sLog->outBasic("[ArcaBattle] ArcaBattle Proc State [%d]", lpMsg->btProcState);
}

void GDAnsJoinMemberUnder(PMSG_ANS_AB_JOIN_MEMBER_UNDER_DS *lpMsg)
{
	PMSG_ANS_AB_JOIN_MEMBER_UNDER pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xF8;
	pMsg.h.subcode = 0x3E;
	pMsg.btMinGuildMember = g_ArcaBattle.GetJoinMemberCnt();

	for (int i = 0; i < lpMsg->btGuildCnt; ++i)
	{
		_GUILD_INFO_STRUCT * lpGuild = Guild.SearchGuild(lpMsg->GuildMemberCnt[i].szGuildNames);

		if (lpMsg->GuildMemberCnt[i].btGuildMemberCnt < g_ArcaBattle.GetJoinMemberCnt() && lpGuild != NULL)
		{
			for (int j = 0; j < 80; j++)
			{
				if (lpGuild->GuildStatus[j] == GUILD_MASTER)
				{
					if (gObjIsConnected(lpGuild->Index[j]) == 1)
					{
						pMsg.btGuildMemberCnt = lpMsg->GuildMemberCnt[i].btGuildMemberCnt;
						IOCP.DataSend(lpGuild->Index[j], (LPBYTE)&pMsg, pMsg.h.size);
					}

					break;
				}
			}
		}
	}
}

void DGAns_CCF_Info_Notify(SDHP_ANS_CCF_INFO_NOTIFY *lpMsg)
{
	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		return;
	}

	char *szTemp;

	switch (lpMsg->nType)
	{
	case 1:
		szTemp = "Preliminary Battle";
		break;
	case 2:
		szTemp = "Semi Final Battle";
		break;
	case 3:
		szTemp = "The final battle";
		break;
	}

	char szMsg[256];

	for (int nUser = g_ConfigRead.server.GetObjectStartUserIndex(); nUser < g_ConfigRead.server.GetObjectMax(); nUser++)
	{
		LPOBJ lpObj = &gObj[nUser];

		if (gObjIsConnected(nUser) && lpObj->Type == OBJ_USER)
		{
			memset(szMsg, 0x00, sizeof(szMsg));
			sprintf(szMsg, Lang.GetText(0, 567), szTemp, lpMsg->nMin);

			GSProtocol.GCServerMsgStringSend(szMsg, nUser, 0);
		}
	}
}

void DGAns_GetCCFPermission(SDHP_ANS_CCF_GETPERMISSION *lpMsg)
{
	if (gObjIsConnected(lpMsg->nIndex) == false)
	{
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->nIndex];

	BYTE byCCF_Enter_Result = -1;

	if (lpMsg->nResult)
	{
		byCCF_Enter_Result = 3;
	}

	else
	{
		if (lpObj->m_byCCF_CanEnter != 0xFF || lpObj->m_bCCF_UI_Using != TRUE)
		{
			byCCF_Enter_Result = 6;
		}

		else
		{
			DWORD nNowTick = GetTickCount();
			DWORD nUserTick = lpObj->m_nCCF_CertiTick;
			DWORD nTickGap = nNowTick - nUserTick;
			if (nTickGap >= 60000)
				byCCF_Enter_Result = 4;
			else
				byCCF_Enter_Result = 0;
		}
	}

	PMSG_RESULT_CCF_ENTERCHECK pResult;

	PHeadSubSetB((LPBYTE)&pResult, 0xAF, 0x05, sizeof(pResult));

	if (byCCF_Enter_Result)
	{
		pResult.byResult = 3;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;

		IOCP.DataSend(lpMsg->nIndex, (LPBYTE)&pResult, pResult.h.size);
		lpObj->m_byCCF_CanEnter = 0;
		lpObj->m_bCCF_UI_Using = false;
	}

	else
	{
		int nResult = GSProtocol.CGReqEnterChaosCastleFinal(lpMsg->nIndex);

		if (nResult)
		{
			sLog->outBasic("[CGReqCCF_EnterCheck][CGReqEnterChaosCastleFinal] ACC :%s , NAME :%s return :%d ", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, nResult);
		}
	}
}

void GCMuunInventoryItemListSend(CGameObject* lpObj)
{
	int itemcount;
	CGameObject* lpObj;
	BYTE sendBuf[4096];
	unsigned int pMsgILSize;
	unsigned int sOfs;
	PMSG_INVENTORYLIST pMsgIL;
	PMSG_INVENTORYLISTCOUNT pMsgILC;

	sOfs = 6;
	pMsgILSize = 13;
	lpObj = &gObj[aIndex];
	itemcount = 0;
	for (int n = 0; n < MUUN_INVENTORY_SIZE; ++n)
	{
		if (lpObj->pMuunInventory[n].IsItem() == 1)
		{
			if (lpObj->pMuunInventory[n].m_bItemExist)
			{
				/*			pMsgIL.Pos = n;
			ItemByteConvert(pMsgIL.ItemInfo,lpObj->pInventory[n]);
			memcpy(&sendBuf[sOfs], &pMsgIL, pMsgILSize);
			itemcount++;
			sOfs += pMsgILSize;*/
				pMsgIL.Pos = n;
				//memcpy(&sendBuf[3920], &lpObj->pMuunInventory[n], 0xC8u);
				ItemByteConvert(pMsgIL.ItemInfo, lpObj->pMuunInventory[n]);
				memcpy(&sendBuf[sOfs], &pMsgIL, pMsgILSize);
				++itemcount;
				sOfs += pMsgILSize;
			}
		}
	}

	pMsgILC.h.c = 0xC4;
	pMsgILC.h.headcode = 0x4E;
	pMsgILC.subcode = 0x02;
	pMsgILC.h.sizeH = SET_NUMBERH(sOfs);
	pMsgILC.h.sizeL = SET_NUMBERL(sOfs);
	pMsgILC.Count = itemcount;
	memcpy(sendBuf, &pMsgILC, sizeof(PMSG_INVENTORYLISTCOUNT));
	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, sendBuf, sOfs);
}

void GDReqSaveEventInvenItem(CGameObject* lpObj)
{
	if (g_CMuRummyMng.IsMuRummyEventOn() == false && g_ConfigRead.data.common.EventInventoryOpen == false)
	{
		return;
	}

	if (lpObj->EventInventoryLoad == false)
	{
		return;
	}

	if (ObjectMaxRange(lpObj->m_Index) == false)
	{
		return;
	}

	if (!gObjIsConnected(lpObj->m_Index))
	{
		return;
	}

	BYTE NewOption[MAX_EXOPTION_SIZE];

	for (int n = 0; n < EVENT_INVENTORY_SIZE; n++)
	{
		if (lpObj->pEventInventory[n].IsItem() == TRUE)
		{
			ItemIsBufExOption(NewOption, &lpObj->pEventInventory[n]);

		}
	}

	_tagSDHP_REQ_DBEVENT_INVEN_SAVE pMsg;
	memcpy(pMsg.AccountID, lpObj->m_PlayerData->ConnectUser->AccountID, MAX_ACCOUNT_LEN + 1);
	memcpy(pMsg.Name, lpObj->Name, MAX_ACCOUNT_LEN + 1);

	pMsg.h.c = 0xC2;
	pMsg.h.sizeH = HIBYTE(sizeof(pMsg));
	pMsg.h.sizeL = LOBYTE(sizeof(pMsg));
	pMsg.h.headcode = 0xE7;
	ItemByteConvert32(pMsg.dbInventory, lpObj->pEventInventory, 32);

	wsDataCli.DataSend((char *)&pMsg, sizeof(pMsg));
}

void GDReqLoadEventInvenItem(CGameObject* lpObj)
{
	if (g_CMuRummyMng.IsMuRummyEventOn() == false && g_ConfigRead.data.common.EventInventoryOpen == false)
	{
		return;
	}

	if (ObjectMaxRange(lpObj->m_Index) == false)
	{
		return;
	}

	if (!gObjIsConnected(lpObj->m_Index))
	{
		return;
	}

	_tagSDHP_REQ_DBEVENT_INVEN_LOAD pMsg;
	memcpy(pMsg.AccountID, lpObj->m_PlayerData->ConnectUser->AccountID, MAX_ACCOUNT_LEN + 1);
	memcpy(pMsg.Name, lpObj->Name, MAX_ACCOUNT_LEN + 1);

	pMsg.aIndex = lpObj->m_Index;
	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xE6;

	wsDataCli.DataSend((char *)&pMsg, sizeof(pMsg));
}

void DGLoadEventInvenItem(_tagSDHP_ANS_DBEVENT_INVEN_LOAD *lpMsg)
{
	if (g_CMuRummyMng.IsMuRummyEventOn() == false && g_ConfigRead.data.common.EventInventoryOpen == false)
	{
		return;
	}

	char szId[11];
	CGameObject* lpObj = lpMsg->aIndex;

	szId[MAX_ACCOUNT_LEN] = 0;
	memcpy(szId, lpObj->m_PlayerData->ConnectUser->AccountID, MAX_ACCOUNT_LEN);

	if (ObjectMaxRange(lpObj) == FALSE)
	{
		return;
	}

	if (lpObj->m_PlayerData->m_bMapSvrMoveReq == true)
	{
		return;
	}

	if (lpObj->m_State == 32)
	{
		return;
	}

	if (lpObj->m_PlayerData->m_bMapSvrMoveQuit == true)
	{
		return;
	}

	if (!gObjIsAccontConnect(lpObj, szId))
	{
		return;
	}

	lpObj->EventInventoryLoad = true;
	LPOBJ lpObj = &gObj[aIndex];
	int itype;
	int _type;
	BYTE OptionData;
	CItem item;

	memset(lpObj->pEventInventoryMap, (BYTE)-1, EVENT_INVENTORY_MAP_SIZE);

	int count = 0;
	for (int n = 0; n < EVENT_INVENTORY_SIZE; n++)
	{
		itype = lpMsg->dbItems[n*MAX_DBITEM_INFO];

		itype |= (lpMsg->dbItems[n*MAX_DBITEM_INFO + 9] & 0xF0) << 5;
		itype |= (lpMsg->dbItems[n*MAX_DBITEM_INFO + 7] & 0x80) << 1;
		_type = itype;

		lpObj->pEventInventory[n].Clear();

		if (lpMsg->dbItems[n*MAX_DBITEM_INFO] == (BYTE)-1 && (lpMsg->dbItems[n*MAX_DBITEM_INFO + 9] & 0xF0) == 0xF0 && (lpMsg->dbItems[n*MAX_DBITEM_INFO + 7] & 0x80) == 0x80)
		{
			itype = -1;
		}

		if (IsItem(_type) == FALSE)
		{
			itype = -1;
		}

		if (_type == ITEMGET(14, 17) || _type == ITEMGET(14, 18))
		{
			if (lpMsg->dbItems[n*MAX_DBITEM_INFO + 2] == 0)
			{
				_type = -1;
			}
		}

		if (_type == ITEMGET(14, 19))
		{
			if (lpMsg->dbItems[n*MAX_DBITEM_INFO + 2] == (BYTE)-1)
			{
				itype = -1;
			}
		}

		if (itype != -1)
		{
			item.m_Level = DBI_GET_LEVEL(lpMsg->dbItems[n*MAX_DBITEM_INFO + 1]);

			if (_type != ITEMGET(14, 11))	// Box Of Luck
			{
				if (item.m_Level > 15)
				{
					item.m_Level = 0;
				}
			}

			OptionData = lpMsg->dbItems[n*MAX_DBITEM_INFO + 1];
			item.m_Option1 = DBI_GET_SKILL(OptionData);
			item.m_Option2 = DBI_GET_LUCK(OptionData);
			item.m_Option3 = DBI_GET_OPTION(OptionData);
			item.m_Option3 |= DBI_GET_DINO_OPTION(lpMsg->dbItems[n*MAX_DBITEM_INFO + 7]);
			item.m_Durability = lpMsg->dbItems[n*MAX_DBITEM_INFO + 2];
			item.m_JewelOfHarmonyOption = lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_JOH_DATA];
			item.m_ItemOptionEx = DBI_GET_380OPTION(lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_OPTION380_DATA]);

			if (item.m_ItemOptionEx != 0)
			{
				item.m_Type = itype;
				if (g_kItemSystemFor380.Is380Item(&item) == FALSE)
				{
					item.m_ItemOptionEx = 0;

				}
			}

			BYTE SocketOption[5];
			BYTE SocketOptionIndex = 0xFF;
			int i = 0;
			memset(&SocketOption, 0xFF, 5);

			if (gSocketItemType.CheckSocketItemType(itype) == TRUE)
			{
				SocketOptionIndex = lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_JOH_DATA];

				for (i = 0; i < 5; i++)
				{
					SocketOption[i] = lpMsg->dbItems[((n*MAX_DBITEM_INFO) + DBI_SOCKET_1) + i];
				}
			}

			else if (g_PentagramSystem.IsPentagramItem(itype) == TRUE ||
				g_PentagramSystem.IsPentagramJewel(itype) == TRUE)
			{
				SocketOptionIndex = lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_JOH_DATA];

				for (i = 0; i < 5; i++)
				{
					SocketOption[i] = lpMsg->dbItems[((n*MAX_DBITEM_INFO) + DBI_SOCKET_1) + i];
				}
			}

			else
			{
				SocketOptionIndex = 0;

				for (i = 0; i < 5; i++)
				{
					SocketOption[i] = 0xFF;
				}
			}

			item.m_PeriodItemOption = (lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_OPTION380_DATA] & 6) >> 1;

			item.Convert(itype, item.m_Option1, item.m_Option2, item.m_Option3, DBI_GET_NOPTION(lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_NOPTION_DATA]), lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SOPTION_DATA], item.m_ItemOptionEx, SocketOption, SocketOptionIndex, item.m_PeriodItemOption, CURRENT_DB_VERSION);

			if (_type == ITEMGET(14, 7)) // Siege Potion
			{

			}

			else if ((_type >= ITEMGET(14, 0) && _type <= ITEMGET(14, 8)) || (_type >= ITEMGET(14, 35) && _type <= ITEMGET(14, 40)))
			{
				if (item.m_Durability == 0.0f)
				{
					item.m_Durability = 1.0f;
				}
			}

			else if (_type != ITEMGET(13, 10) && _type != ITEMGET(14, 29) && _type != ITEMGET(14, 21) && item.m_Level == 3)
			{
				if (item.m_Durability > item.m_BaseDurability)
				{
					item.m_Durability = item.m_BaseDurability;
				}
			}

			lpObj->pEventInventory[n].m_Option1 = item.m_Option1;
			lpObj->pEventInventory[n].m_Option2 = item.m_Option2;
			lpObj->pEventInventory[n].m_Option3 = item.m_Option3;
			lpObj->pEventInventory[n].m_JewelOfHarmonyOption = item.m_JewelOfHarmonyOption;
			lpObj->pEventInventory[n].m_ItemOptionEx = item.m_ItemOptionEx;

			DWORD hidword = MAKE_NUMBERDW(MAKE_NUMBERW(lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL1], lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL2]), MAKE_NUMBERW(lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL3], lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL4]));
			DWORD lodword = MAKE_NUMBERDW(MAKE_NUMBERW(lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL5], lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL6]), MAKE_NUMBERW(lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL7], lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL8]));
			item.m_Number = MAKEQWORD(hidword, lodword);

			gObjEventInventoryInsertItemPos(lpObj->m_Index, item, n, -1);
			gObjEventInventoryItemSet(lpObj->m_Index, n, 1);
			count++;
		}
	}

	BYTE NewOption[MAX_EXOPTION_SIZE];

	for (int n = 0; n < EVENT_INVENTORY_SIZE; n++)
	{
		if (lpObj->pEventInventory[n].IsItem() == TRUE)
		{
			ItemIsBufExOption(NewOption, &lpObj->pEventInventory[n]);

		}
	}

	GSProtocol.GCEventInventoryItemListSend(lpObj);

	if (g_ConfigRead.data.common.EventInventoryOpen == true)
	{
		GSProtocol.GCSendEventBanner(lpObj, 1); //[K2]
	}

	GSProtocol.GCPlayerStatsPanelRates(lpObj->m_Index);
	GSProtocol.GCPlayerStatsPanelNew(lpObj->m_Index);

	for (int n = 0; n < INVENTORY_SIZE; n++)
	{
		if (lpObj->pntInventory[n]->IsPeriodItemExpire() == 1)
		{
			lpObj->pntInventory[n]->SetPeriodItemExpire();
			g_PeriodItemEx.RemovePeriodItemDataByForce(lpObj, lpObj->pntInventory[n]->m_Type, lpObj->pntInventory[n]->m_Number);
			lpObj->pntInventory[n]->Clear();
		}
	}
}

void GDReqSwitchWare(CGameObject* lpObj, int WareID)
{
	PMSG_REQ_SWITCHWARE pMsg;
	PHeadSetB((LPBYTE)&pMsg, 0x03, sizeof(pMsg));

	pMsg.iIndex = aIndex;
	memcpy(pMsg.szAccountID, lpObj->m_PlayerData->ConnectUser->AccountID, MAX_ACCOUNT_LEN + 1);
	pMsg.WarehouseID = WareID;

	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
}

void DGAnsSwitchWare(PMSG_ANS_SWITCHWARE *lpMsg)
{
	int iIndex = lpMsg->iIndex;

	if (ObjectMaxRange(iIndex) == false)
	{
		return;
	}

	if (!gObjIsAccontConnect(iIndex, lpMsg->szAccountID))
	{
		return;
	}

	if (lpMsg->Result == 0)
	{
		MsgOutput(iIndex, Lang.GetText(0, 452), lpMsg->WarehouseID);
	}

	else if (lpMsg->Result == 1)
	{
		MsgOutput(iIndex, "Internal error");
	}

	else if (lpMsg->Result == 2)
	{
		MsgOutput(iIndex, "You cannot switch warehouse now, please wait.");
	}

	else if (lpMsg->Result == 3)
	{
		MsgOutput(iIndex, "You cannot switch warehouse now, warehouse is open or still not saved");
	}

	else if (lpMsg->Result == 4)
	{
		MsgOutput(iIndex, "This is already used warehouse (%d)", lpMsg->WarehouseID);
	}
}

void DGAnsUBFAccountUserInfo(PMSG_ANS_UBF_ACCOUNT_USERINFO *lpMsg)
{
	if (!ObjectMaxRange(lpMsg->iUserIndex))
	{
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->iUserIndex];

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	if (lpMsg->btResult == TRUE)
	{
		g_UnityBattleField.DGAnsCheckJoinedUnityBattleField(lpObj->m_Index, lpMsg->btRegisterState);
	}

	else
	{
		g_UnityBattleField.DGAnsCheckJoinedUnityBattleField(lpObj->m_Index, 0);
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE && lpMsg->btObserverMode == TRUE && (lpMsg->btRegisterState == 0 || lpMsg->btResult == 0))
	{
		g_UnityBattleField.DGAnsCheckJoinedUnityBattleField(lpObj->m_Index, lpMsg->btRegisterState);

		sLog->outBasic("[DGAnsUBFAccountUserInfo][%s][%s] LogOut, Because Not Resister State or Request To Cancel", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
		gObjCloseSet(lpObj->m_Index, 0);
	}

	if (lpMsg->btObserverMode == FALSE)
	{
		PMSG_ANS_UBF_INFO pResult;
		PHeadSubSetB((LPBYTE)&pResult, 0xDC, 0x01, sizeof(pResult));
		pResult.btResult = lpMsg->btRegisterState;
		IOCP.DataSend(lpMsg->iUserIndex, (LPBYTE)&pResult, pResult.h.size);
	}
}

void DGUBFRegisterAccountUserResult(PMSG_UBF_REGISTER_ACCOUNT_USER_RESULT *lpMsg)
{
	if (!ObjectMaxRange(lpMsg->iUserIndex))
	{
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->iUserIndex];

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	if (lpMsg->btResult != TRUE)
	{
		sLog->outBasic("[UBFRegisterAccountUserResult][%s][%s] UBFRegisterAccountUserResult Error :%d (0:DB fail/3: Unexpired Day)",
			lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpMsg->btResult);
	}

	g_UnityBattleField.DGAnsJoinUnityBattleField(lpMsg->iUserIndex, lpMsg->btResult, lpMsg->nLeftSec);
}

void DGUBFAccountUserCopyResult(PMSG_UBF_ACCOUNT_USER_COPY_RESULT *lpMsg)
{
	if (!ObjectMaxRange(lpMsg->iUserIndex))
	{
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->iUserIndex];

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	g_UnityBattleField.DGAnsCopyCharacterInfo(lpMsg->iUserIndex, lpMsg->btResult, lpMsg->btSubResult);
}

void DGAnsUBFGetReward(PMSG_ANS_UBF_GET_REWARD *lpMsg)
{
	if (!ObjectMaxRange(lpMsg->iUserIndex))
	{
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->iUserIndex];

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	PMSG_ANS_GET_REWARD pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xCD, 0x06, sizeof(pMsg));

	pMsg.btResult = lpMsg->btReturn;
	IOCP.DataSend(lpMsg->iUserIndex, (LPBYTE)&pMsg, pMsg.h.size);

	if (lpMsg->btReturn == 0)
	{
		sLog->outBasic("[UBF][DGAnsUBFGetReward][%d][%s][%s] is not eligible for WinnerItem. return : %d",
			lpMsg->iUserIndex, lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpMsg->btReturn);
		return;
	}

	sLog->outBasic("[UBF][DGAnsUBFGetReward][%d][%s][%s] Received Reward Of UnityBattleField - ChaosCastleFinal : %d",
		lpMsg->iUserIndex, lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpMsg->btReturn);

	if (lpMsg->btBattleKind == 1) // CCF
	{
		if (lpMsg->btStage1 == TRUE)
		{
			for (int i = 0; i < lpMsg->btRewardCount1[0]; i++)
			{
				g_ChaosCastleFinal.GiveCCFWinnerItem(lpObj->m_Index, 1, 0);
			}

			for (int i = 0; i < lpMsg->btRewardCount1[1]; i++)
			{
				g_ChaosCastleFinal.GiveCCFWinnerItem(lpObj->m_Index, 1, 1);
			}
		}

		if (lpMsg->btStage2 == TRUE)
		{
			for (int i = 0; i < lpMsg->btRewardCount2[0]; i++)
			{
				g_ChaosCastleFinal.GiveCCFWinnerItem(lpObj->m_Index, 2, 0);
			}

			for (int i = 0; i < lpMsg->btRewardCount2[1]; i++)
			{
				g_ChaosCastleFinal.GiveCCFWinnerItem(lpObj->m_Index, 2, 1);
			}
		}

		if (lpMsg->btStage3 == TRUE)
		{
			for (int i = 0; i < lpMsg->btRewardCount3[0]; i++)
			{
				g_ChaosCastleFinal.GiveCCFWinnerItem(lpObj->m_Index, 3, 0);
			}

			for (int i = 0; i < lpMsg->btRewardCount3[1]; i++)
			{
				g_ChaosCastleFinal.GiveCCFWinnerItem(lpObj->m_Index, 3, 1);
			}
		}
	}

	else if (lpMsg->btBattleKind == 2) // Tormented Square
	{
		if (lpMsg->btStage1 == TRUE)
		{
			for (int i = 0; i < lpMsg->btRewardCount1[0]; i++)
			{
				g_DevilSquareFinal.GiveDSFWinnerItem(lpObj->m_Index, 1, 0);
			}

			for (int i = 0; i < lpMsg->btRewardCount1[1]; i++)
			{
				g_DevilSquareFinal.GiveDSFWinnerItem(lpObj->m_Index, 1, 1);
			}
		}

		if (lpMsg->btStage2 == TRUE)
		{
			for (int i = 0; i < lpMsg->btRewardCount2[0]; i++)
			{
				g_DevilSquareFinal.GiveDSFWinnerItem(lpObj->m_Index, 2, 0);
			}

			for (int i = 0; i < lpMsg->btRewardCount2[1]; i++)
			{
				g_DevilSquareFinal.GiveDSFWinnerItem(lpObj->m_Index, 2, 1);
			}
		}

		if (lpMsg->btStage3 == TRUE)
		{
			for (int i = 0; i < lpMsg->btRewardCount3[0]; i++)
			{
				g_DevilSquareFinal.GiveDSFWinnerItem(lpObj->m_Index, 3, 0);
			}

			for (int i = 0; i < lpMsg->btRewardCount3[1]; i++)
			{
				g_DevilSquareFinal.GiveDSFWinnerItem(lpObj->m_Index, 3, 1);
			}
		}
	}

	else if (lpMsg->btBattleKind == 3) // CC Battle
	{
		if (lpMsg->btStage1 == TRUE)
		{
			for (int i = 0; i < lpMsg->btRewardCount1[0]; i++)
			{
				g_ChaosCastle.GiveUBFReward(lpObj->m_Index, 0);
			}

			for (int i = 0; i < lpMsg->btRewardCount1[1]; i++)
			{
				g_ChaosCastle.GiveUBFReward(lpObj->m_Index, 1);
			}
		}
	}

	g_UnityBattleField.GDReqSetReceivedWinnerItem(lpMsg->iUserIndex, lpMsg->btBattleKind);
}

void DGAnsSetReceivedWinnerItem(PMSG_ANS_UBF_SET_RECEIVED_REWARD *lpMsg)
{
	if (!ObjectMaxRange(lpMsg->iUserIndex))
	{
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->iUserIndex];

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	g_UnityBattleField.DGAnsSetReceivedWinnerItem(lpMsg->iUserIndex, lpMsg->btReturn);
}

void DGUBFAnsCancelToJoinResult(PMSG_UBF_ANS_CANCEL_REGISTER_USER *lpMsg)
{
	g_UnityBattleField.DGAnsCancelUnityBattleField(lpMsg->iUserIndex, lpMsg->btCanceledResult, lpMsg->btDeletedResult);
}

void DGUBFAnsGetRealNameAndServerCode(PMSG_ANS_GET_UBF_REAL_NAME *lpMsg)
{
	if (!ObjectMaxRange(lpMsg->iUserIndex))
	{
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->iUserIndex];

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	g_UnityBattleField.DGAnsGetRealNameAndServerCode(lpMsg->iUserIndex, lpMsg->iServerCode);
}

void GDReqCopyPetItemInfo(CGameObject* lpObj)
{
	if (!ObjectMaxRange(lpObj))
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	if (!gObjIsConnected(lpObj))
	{
		return;
	}

	char Buffer[4096];
	int lOfs = sizeof(PMSG_UBF_ACCOUNT_USER_COPY_PETITEM);

	int Count = 0;
	PMSG_UBF_ACCOUNT_USER_COPY_PETITEM pMsg;
	PMSG_UBF_COPY_PETITEM pPetItem;

	for (int i = 0; i < INVENTORY_SIZE; i++)
	{
		if (lpObj->pInventory[i].IsItem() == TRUE &&
			(lpObj->pInventory[i].m_Type == ITEMGET(13, 4) || lpObj->pInventory[i].m_Type == ITEMGET(13, 5)))
		{
			pPetItem.btItemPos = i;
			pPetItem.i64ItemSerial = lpObj->pInventory[i].m_Number;

			memcpy(&Buffer[lOfs], &pPetItem, sizeof(pPetItem));
			lOfs += sizeof(pPetItem);
			Count++;
		}
	}

	if (Count > 0)
	{
		pMsg.h.set((LPBYTE)&pMsg, 0xF4, lOfs);
		memcpy(pMsg.szAccountID, lpObj->m_PlayerData->ConnectUser->AccountID, MAX_ACCOUNT_LEN + 1);

		pMsg.iUserIndex = aIndex;
		pMsg.btCount = Count;
		pMsg.IsUnityBattleFieldServer = g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE ? TRUE : FALSE;
		pMsg.ServerCode = g_ConfigRead.server.GetGameServerCode() / 20;
		memcpy(&Buffer, &pMsg, sizeof(pMsg));

		wsDataCli.DataSend(Buffer, lOfs);
	}

	sLog->outBasic("[UBF][GDReqCopyPetItemInfo][%s][%s] Move(Copy) the PetInfo Into UnityBattleField",
		lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
}

void DGAns_ChaosCastle_KillPoint_Result(SDHP_ANS_KILLPOINT_RESULT_CC_UBF *lpMsg)
{
	CGameObject* lpObj = lpMsg->nIndex;

	if (!ObjectMaxRange(lpObj))
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	if (!gObjIsConnected(lpObj))
	{
		return;
	}

	sLog->outBasic("[CChaosCastle][DGAns_ChaosCastle_KillPoint_Result][%s][%s][%s] Ans Result-Return:%d / KillPoint:%d/ TotalPoint:%d",
		lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->m_RealNameOfUBF, lpMsg->nResult, lpMsg->nCurrentPoint, lpMsg->nTotalPoint);
}

void GDReqDSFCanPartyEnter(CGameObject* lpObj)
{
	if (!ObjectMaxRange(lpObj))
	{
		return;
	}

	if (!gObjIsConnected(lpObj))
	{
		return;
	}

	sLog->outBasic("[DSF][GDReqCanPartyEnter][%s][%s][%s][%d] DSF Enter Request",
		lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->m_PlayerData->m_RealNameOfUBF,
		lpObj->Name, lpObj->m_PlayerData->m_nServerCodeOfHomeWorld);

	PMSG_ANS_DSF_ENTER pResult;
	PHeadSubSetB((LPBYTE)&pResult, 0xDB, 0x01, sizeof(pResult));

	memset(pResult.Name, 0x00, sizeof(pResult.Name));

	if (lpObj->PartyNumber < 0)
	{
		pResult.btResult = 1;

		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (gParty.Isleader(lpObj->PartyNumber, aIndex, lpObj->DBNumber) == FALSE)
	{
		pResult.btResult = 3;

		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (gParty.GetCount(lpObj->PartyNumber) != 2)
	{
		pResult.btResult = 4;

		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	int dbnumber = 0, useridx = -1;

	PMSG_REQ_DSF_CAN_PARTY_ENTER pMsg;

	for (int i = 0; i < 2; i++)
	{
		if (gParty.GetIndexUser(lpObj->PartyNumber, i, useridx, dbnumber) == FALSE)
		{
			pResult.btResult = 13;

			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
			return;
		}

		if (useridx > 0)
		{
			if (i == 0)
			{
				memcpy(pMsg.szAccountID1, gObj[useridx].AccountID, MAX_ACCOUNT_LEN + 1);
				memcpy(pMsg.szUserName1, gObj[useridx].Name, MAX_ACCOUNT_LEN + 1);
			}

			else if (i == 1)
			{
				memcpy(pMsg.szAccountID2, gObj[useridx].AccountID, MAX_ACCOUNT_LEN + 1);
				memcpy(pMsg.szUserName2, gObj[useridx].Name, MAX_ACCOUNT_LEN + 1);
			}
		}
	}

	pMsg.iUserIndex = aIndex;
	pMsg.btDSFType = g_DevilSquareFinal.WhatIsTodayTypeOfDSF();
	pMsg.iEnterYear = sysTime.wYear;
	pMsg.btEnterMonth = sysTime.wMonth;
	pMsg.btEnterDay = sysTime.wDay;

	PHeadSubSetB((LPBYTE)&pMsg, 0xFD, 0x00, sizeof(pMsg));
	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
}

void DG_DSF_CanPartyEnter(PMSG_ANS_DSF_CAN_PARTY_ENTER *lpMsg)
{
	CGameObject* lpObj = lpMsg->iUserIndex;

	if (!gObjIsConnected(lpObj))
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (!lpObj)
	{
		return;
	}

	PMSG_ANS_DSF_ENTER pResult;
	PHeadSubSetB((LPBYTE)&pResult, 0xDB, 0x01, sizeof(pResult));

	memset(pResult.Name, 0x00, sizeof(pResult.Name));

	if (lpMsg->btResult == 3)
	{
		if (g_DevilSquareFinal.WhatIsTodayTypeOfDSF() == 1)
		{
			pResult.btResult = 22;
		}

		else if (g_DevilSquareFinal.WhatIsTodayTypeOfDSF() == 2)
		{
			pResult.btResult = 23;
		}

		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

		sLog->outBasic("[DSF][DG_DSF_CanPartyEnter] [%s][%s][%s][%d] Can't Enter : Already Pass Party",
			lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->m_PlayerData->m_RealNameOfUBF,
			lpObj->Name, lpObj->m_PlayerData->m_nServerCodeOfHomeWorld);
	}

	else if (lpMsg->btResult == 0 || lpMsg->btResult == 4)
	{
		GSProtocol.CGReqDSFEnter(lpObj);

		sLog->outBasic("[DSF][DG_DSF_CanPartyEnter] [%s][%s][%s][%d] Can Enter : EnterCnt:%d",
			lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->m_PlayerData->m_RealNameOfUBF,
			lpObj->Name, lpObj->m_PlayerData->m_nServerCodeOfHomeWorld,
			lpMsg->btCount);
	}

	else if (lpMsg->btResult == 1)
	{
		if (g_DevilSquareFinal.WhatIsTodayTypeOfDSF() == 2)
		{
			pResult.btResult = 18;
		}

		else if (g_DevilSquareFinal.WhatIsTodayTypeOfDSF() == 3)
		{
			pResult.btResult = 19;
		}

		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

		sLog->outBasic("[DSF][DG_DSF_CanPartyEnter] [%s][%s][%s][%d] Can't Enter : Not Tournament User",
			lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->m_PlayerData->m_RealNameOfUBF,
			lpObj->Name, lpObj->m_PlayerData->m_nServerCodeOfHomeWorld);
	}

	else if (lpMsg->btResult == 2 && g_DevilSquareFinal.WhatIsTodayTypeOfDSF() == 1)
	{
		pResult.btResult = 22;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

		sLog->outBasic("[DSF][DG_DSF_CanPartyEnter] [%s][%s][%s][%d] Can't Enter : Already Exist Pass User",
			lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->m_PlayerData->m_RealNameOfUBF,
			lpObj->Name, lpObj->m_PlayerData->m_nServerCodeOfHomeWorld);
	}

	else if (lpMsg->btResult == 5)
	{
		pResult.btResult = 21;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

		sLog->outBasic("[DSF][DG_DSF_CanPartyEnter] [%s][%s][%s][%d] Can't Enter : Enter With Different User",
			lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->m_PlayerData->m_RealNameOfUBF,
			lpObj->Name, lpObj->m_PlayerData->m_nServerCodeOfHomeWorld);
	}
}

void DG_DSF_PartyRankRenew(PMSG_ANS_DSF_PARTYRANKRENEW *lpMsg)
{
	if (!lpMsg)
	{
		return;
	}

	if (g_ConfigRead.server.GetServerType() != SERVER_BATTLECORE)
	{
		return;
	}

	if (lpMsg->btResult == 1)
	{
		g_DevilSquareFinal.GDReqDSFGoFinalParty(2);
		g_DevilSquareFinal.GDReqDSFGoFinalParty(3);
		g_DevilSquareFinal.GDReqDSFGoFinalParty(4);
	}
}

void DG_DSF_GoFinalParty(LPBYTE lpRecv)
{
	PMSG_ANS_DSF_GO_FINAL_PARTY * lpMsg = (PMSG_ANS_DSF_GO_FINAL_PARTY *)(lpRecv);

	if (!lpMsg)
	{
		return;
	}

	if (g_ConfigRead.server.GetServerType() != SERVER_BATTLECORE)
	{
		return;
	}

	int nPartyCnt = lpMsg->btPartyCnt;

	switch (lpMsg->btDSFType)
	{
	case 2:
		if (nPartyCnt > 40)
		{
			nPartyCnt = 40;
		}

		for (int i = 0; i < 40; i++)
		{
			g_DevilSquareFinal.m_GoSemiFinalParty[i].Clear();
		}

		for (int j = 0; j < nPartyCnt; j++)
		{
			DSF_GO_FINAL_PARTY * lpData = (DSF_GO_FINAL_PARTY *)(lpRecv + sizeof(PMSG_ANS_DSF_GO_FINAL_PARTY) + (j * sizeof(DSF_GO_FINAL_PARTY)));

			if (lpData->btRank <= 0)
			{
				memset(g_DevilSquareFinal.m_GoSemiFinalParty[j].szUserName1, 0x00, MAX_ACCOUNT_LEN + 1);
				memset(g_DevilSquareFinal.m_GoSemiFinalParty[j].szUserName2, 0x00, MAX_ACCOUNT_LEN + 1);
				g_DevilSquareFinal.m_GoSemiFinalParty[j].wServerCode1 = -1;
				g_DevilSquareFinal.m_GoSemiFinalParty[j].wServerCode2 = -1;
				g_DevilSquareFinal.m_GoSemiFinalParty[j].btRank = 0;
				g_DevilSquareFinal.m_GoSemiFinalParty[j].btDSFType = 0;
				g_DevilSquareFinal.m_GoSemiFinalParty[j].iPoint = 0;
			}

			else
			{
				memcpy(g_DevilSquareFinal.m_GoSemiFinalParty[j].szUserName1, lpData->szUserName1, MAX_ACCOUNT_LEN + 1);
				memcpy(g_DevilSquareFinal.m_GoSemiFinalParty[j].szUserName2, lpData->szUserName2, MAX_ACCOUNT_LEN + 1);
				g_DevilSquareFinal.m_GoSemiFinalParty[j].wServerCode1 = lpData->wServerCode1;
				g_DevilSquareFinal.m_GoSemiFinalParty[j].wServerCode2 = lpData->wServerCode2;
				g_DevilSquareFinal.m_GoSemiFinalParty[j].btRank = lpData->btRank;
				g_DevilSquareFinal.m_GoSemiFinalParty[j].btDSFType = lpData->btDSFType;
				g_DevilSquareFinal.m_GoSemiFinalParty[j].iPoint = lpData->iPoint;
			}
		}
		break;
	case 3:
		if (nPartyCnt > 10)
		{
			nPartyCnt = 10;
		}

		for (int i = 0; i < 10; i++)
		{
			g_DevilSquareFinal.m_GoFinalParty[i].Clear();
		}

		for (int j = 0; j < nPartyCnt; j++)
		{
			DSF_GO_FINAL_PARTY * lpData = (DSF_GO_FINAL_PARTY *)(lpRecv + sizeof(PMSG_ANS_DSF_GO_FINAL_PARTY) + (j * sizeof(DSF_GO_FINAL_PARTY)));

			if (lpData->btRank <= 0)
			{
				memset(g_DevilSquareFinal.m_GoFinalParty[j].szUserName1, 0x00, MAX_ACCOUNT_LEN + 1);
				memset(g_DevilSquareFinal.m_GoFinalParty[j].szUserName2, 0x00, MAX_ACCOUNT_LEN + 1);
				g_DevilSquareFinal.m_GoFinalParty[j].wServerCode1 = -1;
				g_DevilSquareFinal.m_GoFinalParty[j].wServerCode2 = -1;
				g_DevilSquareFinal.m_GoFinalParty[j].btRank = 0;
				g_DevilSquareFinal.m_GoFinalParty[j].btDSFType = 0;
				g_DevilSquareFinal.m_GoFinalParty[j].iPoint = 0;
			}

			else
			{
				memcpy(g_DevilSquareFinal.m_GoFinalParty[j].szUserName1, lpData->szUserName1, MAX_ACCOUNT_LEN + 1);
				memcpy(g_DevilSquareFinal.m_GoFinalParty[j].szUserName2, lpData->szUserName2, MAX_ACCOUNT_LEN + 1);
				g_DevilSquareFinal.m_GoFinalParty[j].wServerCode1 = lpData->wServerCode1;
				g_DevilSquareFinal.m_GoFinalParty[j].wServerCode2 = lpData->wServerCode2;
				g_DevilSquareFinal.m_GoFinalParty[j].btRank = lpData->btRank;
				g_DevilSquareFinal.m_GoFinalParty[j].btDSFType = lpData->btDSFType;
				g_DevilSquareFinal.m_GoFinalParty[j].iPoint = lpData->iPoint;
			}
		}
		break;
	case 4:

		if (nPartyCnt <= 0)
		{
			break;
		}

		g_DevilSquareFinal.m_WinnerParty.Clear();

		DSF_GO_FINAL_PARTY * lpData = (DSF_GO_FINAL_PARTY *)(lpRecv + sizeof(PMSG_ANS_DSF_GO_FINAL_PARTY));

		if (lpData->btRank <= 0)
		{
			memset(g_DevilSquareFinal.m_WinnerParty.szUserName1, 0x00, MAX_ACCOUNT_LEN + 1);
			memset(g_DevilSquareFinal.m_WinnerParty.szUserName2, 0x00, MAX_ACCOUNT_LEN + 1);
			g_DevilSquareFinal.m_WinnerParty.wServerCode1 = -1;
			g_DevilSquareFinal.m_WinnerParty.wServerCode2 = -1;
			g_DevilSquareFinal.m_WinnerParty.btRank = 0;
			g_DevilSquareFinal.m_WinnerParty.btDSFType = 0;
			g_DevilSquareFinal.m_WinnerParty.iPoint = 0;
		}

		else
		{
			memcpy(g_DevilSquareFinal.m_WinnerParty.szUserName1, lpData->szUserName1, MAX_ACCOUNT_LEN + 1);
			memcpy(g_DevilSquareFinal.m_WinnerParty.szUserName2, lpData->szUserName2, MAX_ACCOUNT_LEN + 1);
			g_DevilSquareFinal.m_WinnerParty.wServerCode1 = lpData->wServerCode1;
			g_DevilSquareFinal.m_WinnerParty.wServerCode2 = lpData->wServerCode2;
			g_DevilSquareFinal.m_WinnerParty.btRank = lpData->btRank;
			g_DevilSquareFinal.m_WinnerParty.btDSFType = lpData->btDSFType;
			g_DevilSquareFinal.m_WinnerParty.iPoint = lpData->iPoint;
		}
		break;

	}
}

void DG_DSF_GetReward(PMSG_ANS_GET_DSF_REWARD *lpMsg)
{
	if (!lpMsg)
	{
		return;
	}

	PMSG_ANS_DSF_GET_REWARD pResult;
	PHeadSubSetB((LPBYTE)&pResult, 0xDB, 0x09, sizeof(pResult));

	pResult.btResult = lpMsg->btResult;
	IOCP.DataSend(lpMsg->nUserIndex, (LPBYTE)&pResult, pResult.h.size);

	if (lpMsg->btResult == 1)
	{
		sLog->outBasic("[DSF][DG_DSF_GetReward] [%s][%s][%s][%d] Can't Get Reward : Not League Winner!",
			gObj[lpMsg->nUserIndex].AccountID, gObj[lpMsg->nUserIndex].m_PlayerData->m_RealNameOfUBF,
			gObj[lpMsg->nUserIndex].Name, gObj[lpMsg->nUserIndex].m_PlayerData->m_nServerCodeOfHomeWorld);
	}

	else if (lpMsg->btResult == 2)
	{
		sLog->outBasic("[DSF][DG_DSF_GetReward] [%s][%s][%s][%d] Can't Get Reward : Get Reward Already!",
			gObj[lpMsg->nUserIndex].AccountID, gObj[lpMsg->nUserIndex].m_PlayerData->m_RealNameOfUBF,
			gObj[lpMsg->nUserIndex].Name, gObj[lpMsg->nUserIndex].m_PlayerData->m_nServerCodeOfHomeWorld);
	}

	else
	{
		sLog->outBasic("[DSF][DG_DSF_GetReward] [%s][%s][%s][%d] Can Get Reward Success!!! ",
			gObj[lpMsg->nUserIndex].AccountID, gObj[lpMsg->nUserIndex].m_PlayerData->m_RealNameOfUBF,
			gObj[lpMsg->nUserIndex].Name, gObj[lpMsg->nUserIndex].m_PlayerData->m_nServerCodeOfHomeWorld);
	}
}

void DGReqOtherChannelWishper(PMSG_RECV_CHATDATA_WHISPER *lpMsg)
{
	int userIndex = gObjGetIndex(lpMsg->id);

	if (userIndex == -1)
	{
		GDWhisperResultSend(0, lpMsg->OriginPlayerIndex, lpMsg->OriginGSIndex);
		return;
	}
	else
	{
		GDWhisperResultSend(1, lpMsg->OriginPlayerIndex, lpMsg->OriginGSIndex);
	}
	char tid[11];
	PMSG_CHATDATA_WHISPER pWhisper;
	int len;
	int index;


	if (g_ConfigRead.data.common.SavePrivateChat == true)
	{
		sLog->outBasic(Lang.GetText(0, 517), lpMsg->fromId, lpMsg->id, lpMsg->chatmsg);
	}

	tid[10] = 0;
	memcpy(tid, lpMsg->id, sizeof(lpMsg->id));
	index = WhisperCash.AddCash(tid);

	PHeadSetB((LPBYTE)&pWhisper, 0x02, sizeof(pWhisper));
	strcpy(pWhisper.id, lpMsg->fromId);
	memcpy(pWhisper.chatmsg, lpMsg->chatmsg, sizeof(lpMsg->chatmsg));

	//pWhisper.szChatMsg[MAX_CHAT_LEN-2] = 0;
	//pWhisper.chatmsg[MAX_CHAT_LEN-1] = 0;
	len = strlen(pWhisper.chatmsg);

	if (len > 0)
	{
		pWhisper.h.size -= sizeof(pWhisper.chatmsg);
		pWhisper.h.size += len + 1;
		IOCP.DataSend(index, (UCHAR*)&pWhisper, pWhisper.h.size);
	}

}

void GDReqMapSrvGroupServerCount()
{
	DSMSG_REQ_SUBSERVER_COUNT pMsg;
	pMsg.h.set((LPBYTE)&pMsg, 0xC3, 0x00, sizeof(pMsg));
	pMsg.wMapSvrGroup = g_MapServerManager.GetMapSvrGroup();
	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

void DGWhisperResponseRecv(DSMSG_GS_WHISPER_RESULT * aRecv)
{
	LPOBJ lpObj = &gObj[aRecv->OriginPlayerIndex];

	if (!lpObj)
	{
		sLog->outBasic("[%d] Is no longer online LINE %d", aRecv->OriginPlayerIndex, __LINE__);
		return;
	}

	if (lpObj->Connected != PLAYER_PLAYING)
	{
		sLog->outBasic("[%d] Is no longer online LINE %d", aRecv->OriginPlayerIndex, __LINE__);
		return;
	}

	if (lpObj->m_Whispering.iWhisperSent == true)
	{
		if (aRecv->btResult == 1)
			lpObj->m_Whispering.iWhisperSent = false;

		lpObj->m_Whispering.wResponseCount++;
		if (lpObj->m_Whispering.wResponseCount >= gMapSrvGroupOnlineServer && lpObj->m_Whispering.iWhisperSent == true) // if all gs return false , player is not found
		{
			GSProtocol.GCServerMsgSend(0, lpObj->m_Index);
		}
	}
}

void GDWhisperResultSend(BYTE Result, int OriginPlayerIndex, int OriginGS)
{
	DSMSG_ANS_WHISPER_RESULT pMsg;
	pMsg.h.set((LPBYTE)&pMsg, 0xC3, 0x02, sizeof(pMsg));

	pMsg.btResult = Result;
	pMsg.OriginGSIndex = OriginGS;
	pMsg.OriginPlayerIndex = OriginPlayerIndex;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}
void GDReqDisconnectOtherChannel(char * charname)
{
	PMSG_RECV_DC_OTHER_CHANNEL pMsg;
	pMsg.h.set((LPBYTE)&pMsg, 0xC3, 0x04, sizeof(pMsg));

	pMsg.wMapSrvGroup = g_MapServerManager.GetMapSvrGroup();
	memcpy(pMsg.szName, charname, MAX_ACCOUNT_LEN + 1);

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);

}
void DGDisconnectOtherChannel(PMSG_RECV_DC_OTHER_CHANNEL * aRecv)
{
	int iTargetIndex = gObjGetIndex(aRecv->szName);

	if (iTargetIndex >= 0)
	{
		LPOBJ lpTargetObj = &gObj[iTargetIndex];

		if (lpTargetObj == NULL)
		{
			return;
		}

		if (lpTargetObj->m_bOff != false)
		{
			GJSetCharacterInfo(lpTargetObj, iTargetIndex, FALSE);
			gObj[iTargetIndex].m_bOff = false;
			gObjDel(iTargetIndex);

		}

		GSProtocol.GCSendDisableReconnect(iTargetIndex);
		sLog->outBasic("Multi-Cast Disconnect: %s", aRecv->szName);
	}
}

struct MUBOT_SETTINGS_REQ_SAVE
{
	PWMSG_HEAD h;
	char szName[11];
	BYTE btDATA[512];
};

void DGMuBotSettingsSend(CGameObject* lpObj, char* szName, BYTE* lpSettings)
{
	MUBOT_SETTINGS_REQ_SAVE pMsg = { 0 };
	PHeadSetW((LPBYTE)&pMsg, 0x61, sizeof(pMsg));

	memcpy(pMsg.szName, szName, 10);
	memcpy(pMsg.btDATA, lpSettings, sizeof(pMsg.btDATA));

	wsDataCli.DataSend((char*)&pMsg, sizeof(pMsg));
}

void DGMuBotOptionRecv(LPMUBOT_SETTINGS_SEND lpMsg)
{
	CGameObject* lpObj = lpMsg->aIndex;
	char szName[MAX_ACCOUNT_LEN + 1];

	if (aIndex < 0 || aIndex >= g_ConfigRead.server.GetObjectMax())
	{
		return;
	}

	memset(szName, 0, sizeof(szName));
	memcpy(szName, lpMsg->szName, MAX_ACCOUNT_LEN);

	if (strcmp(lpObj->Name, szName) != 0)
	{
		return;
	}

	GSProtocol.GCSendMuBotSettings(lpMsg->aIndex, lpMsg->btDATA);
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

