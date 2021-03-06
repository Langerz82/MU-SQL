#include "StdAfx.h"
#include "GameProtocol.h"
#include "IOCP.h"
#include "GOFunctions.h"
#include "GOEventFunctions.h"
#include "ItemObject.h"
#include "Event.h"
#include "Main.h"
#include "util.h"
#include "TNotice.h"
#include "QuestInfo.h"
#include "ConMember.h"
#include "GameServer.h"
#include "GMMng.h"
//#include "BattleSoccerManager.h"
#include "BuffEffectSlot.h"
#include "ChaosBox.h"
//#include "CastleSiegeSync.h"
#include "TUnion.h"
#include "JewelMixSystem.h"
#include "Weapon.h"
#include "ImperialGuardian.h"
#include "MasterLevelSkillTreeSystem.h"
//#include "KalimaGate.h"
#include "ObjUseSkill.h"
#include "ObjCalCharacter.h"
#include "NpcTalk.h"
#include "Shop.h"
#include "PeriodItemEx.h"
//#include "BattleSoccer.h"
//#include "BattleSoccerManager.h"
#include "gObjMonster.h"
#include "ComboAttack.h"
#include "Gate.h"
#include "MoveCommand.h"
#include "MultiAttackHackCheck.h"
#include "ItemAddOption.h"
#include "JewelOfHarmonySystem.h"
#include "PacketCheckSum.h"
#include "DarkSpirit.h"
//#include "Crywolf.h"
//#include "CrywolfAltar.h"
//#include "CrywolfSync.h"
//#include "DevilSquare.h"
//#include "KanturuEntranceNPC.h"
#include "CashShop.h"
#include "PentagramSystem.h"
//#include "IllusionTempleEvent_Renewal.h"
#include "Sprotocol.h"
#include "DSProtocol.h"
//#include "EDSProtocol.h"
#include "Mercenary.h"
#include "ItemSocketOptionSystem.h"
#include "Guardian.h"
//#include "DoppelGanger.h"
#include "AntiSwear.h"
//#include "ChaosCastle.h"
//#include "ChaosCastleFinal.h"
#include "GamblingSystem.h"
//#include "bcore.h"
#include "GensSystem.h"
#include "GensSystemProtocol.h"
#include "EDSprotocol.h"
#include "NewPVP.h"
#include "MapServerManager.h"
#include "MineSystem.h"
#include "VipSys.h"
#include "LifeStone.h"
#include "QuestExpProtocol.h"
#include "QuestExpProgMng.h"
#include "QuestExpInfo.h"
#include "QuestExpLuaBind.h"
#include "LuaFun.h"
#include "BagManager.h"
#include "SetItemOption.h"
#include "LuckyItemManager.h"
#include "PentagramSystem.h"
#include "PentagramMixSystem.h"
#include "ProhibitedSymbols.h"
//#include "Authorization.h"
#include "PersonalStore.h"
#include "MuunSystem.h"
#include "MuRummyMng.h"
#include "KeyGenerater.h"
//#include "AcheronGuardianEvent.h"
#include "GremoryCase.h"
//#include "DevilSquareFinal.h"
#include "BotSystem.h"
#include "ExpManager.h"
#include "EvolutionMonsterMng.h"
#include "RuudStore.h"
#include "UserChatBlockMng.hpp"
#include "CustomMaxStats.h"
#include "ItemUpgradeJewel.h"
#include "SocketItemType.h"
#include "CustomMichi.h"
//#include "ArcaBattle.h"
//#include "UnityBattleField.h"
#include "ObjAttack.h"

GameProtocol GSProtocol;

GameProtocol::GameProtocol()
{
}

GameProtocol::~GameProtocol()
{
}

bool GameProtocol::PacketCheckTime(CGameObject* lpObj)
{
	if ((GetTickCount() - lpObj->m_PacketCheckTime) < 300)
	{
		return false;
	}

	lpObj->m_PacketCheckTime = GetTickCount();
	return true;
}

bool GameProtocol::DataEncryptCheck(CGameObject* lpObj, BYTE protoNum, BOOL Encrypt)
{
#ifndef EMU_NOCRYPT
	if (Encrypt == FALSE)
	{
		sLog->outBasic("Error-L1 : Not Encrypt %s %d", lpObj->m_PlayerData->ConnectUser->AccountID, protoNum);
		//this->GCSendDisableReconnect(lpObj);
		GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
		return false;
	}
#endif
	return true;
}

void GameProtocol::ProtocolCore(BYTE protoNum, unsigned char * aRecv, int aLen, CGameObject* lpObj, BOOL Encrypt)
{

	/*if (gStalkProtocol)
	{
		if (gStalkProtocolId[0] == lpObj->m_PlayerData->ConnectUser->AccountID[0])
		{
			if (gStalkProtocolId[1] == lpObj->m_PlayerData->ConnectUser->AccountID[1])
			{
				if (!strcmp(gStalkProtocolId, lpObj->m_PlayerData->ConnectUser->AccountID))
				{
					g_Log.AddHeadHex("DATA RECV", aRecv, aLen);
				}
			}
		}
	}*/

	//sLog->outError("K2(ProtocolCore) : %x %x %x", aRecv[0], aRecv[1], aRecv[2]);
	if (protoNum == 0xF1 || protoNum == 0xF3)
	{
		switch (protoNum)
		{
		case 0xF1:
		{
			PMSG_DEFAULT2 * lpMsg2 = (PMSG_DEFAULT2 *)aRecv;

			switch (lpMsg2->subcode)
			{
			case 0x02:
				if (this->DataEncryptCheck(lpObj, protoNum, Encrypt) != FALSE)
				{
					if (lpObj->m_PlayerData->m_bMapSvrMoveQuit == false)
						this->CGClientCloseMsg((PMSG_CLIENTCLOSE *)aRecv, lpObj);
					else
					{
						//this->GCSendDisableReconnect(lpObj);
						GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
					}
				}
				break;
			case 0x03:
				if (this->DataEncryptCheck(lpObj, protoNum, Encrypt) != FALSE)
				{
					this->CGClientMsg((PMSG_CLIENTMSG *)aRecv, lpObj);
				}
				break;
			default:
				sLog->outBasic("UNKNOWN F1 PACKET: %x %x %x %x %x %x %x %x %x %x", aRecv[0], aRecv[1], aRecv[2], aRecv[3], aRecv[4], aRecv[5], aRecv[6], aRecv[7], aRecv[8], aRecv[9]);
				break;

			}
		}
		break;
		case 0xF3:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x03:
				this->CGPCharacterMapJoinRequest((PMSG_CHARMAPJOIN *)aRecv, lpObj);
				break;
			case 0x06:
				this->CGLevelUpPointAdd((PMSG_LVPOINTADD *)aRecv, lpObj);
				break;
			case 0x10: // not used packet
				break;
			case 0x12:
				gObjMoveDataLoadingOK(*lpObj);
				break;
			case 0x15: // by drakelv & wizzy
			{
				struct PMSG_CHARCHECK
				{
					PBMSG_HEAD2 h; // C1:15
					char szName[MAX_ACCOUNT_LEN];
				};

				struct PMSG_CHARCHECK_RESULT
				{
					PBMSG_HEAD2 h; // C1:15
					char szName[MAX_ACCOUNT_LEN];
					BYTE bIsFromOtherAccount; // if i put 0x0e == 1 it pop's the selection screen
				};

				//C1 20 EC 29 00 00 00 00 00 00 00 00 1C 00 00 00 14 00 00 00 19 00 00 00 0A 00 00 00 FF 00 00 00 
				PMSG_CHARCHECK_RESULT pMsg;

				PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x15, sizeof(pMsg));
				memcpy(pMsg.szName, lpObj->Name, MAX_ACCOUNT_LEN);
				pMsg.bIsFromOtherAccount = 0; // if i put 0x0e == 1 it pop's the selection screen

				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);


			}
			break;
			case 0x21:
				this->CGReqUseTransformationRing((PMSG_REQ_USETRANSFORMATIONRING *)aRecv, lpObj);
				break;
			case 0x26:
				this->CGReqDoUsePopupType(lpObj);
				break;
			case 0x30:
				this->GCSkillKeyRecv((PMSG_SKILLKEY *)aRecv, lpObj);
				break;
			case 0x52:
				g_MasterLevelSkillTreeSystem.CGReqGetMasterLevelSkill((PMSG_REQ_MASTERLEVEL_SKILL *)aRecv, *lpObj);
				break;
			case 0x31:
				sLog->outBasic("WARNING: TRAP MESSAGE PACKET FROM MAIN: %x %x %x %x %x %x %x %x %x %x", Encrypt, aRecv[0], aRecv[1], aRecv[2], aRecv[3], aRecv[4], aRecv[5], aRecv[6], aRecv[7], aRecv[8], aRecv[9]);
				break;
			default:
				sLog->outBasic("UNKNOWN F3 PACKET: %x %x %x %x %x %x %x %x %x %x", aRecv[0], aRecv[1], aRecv[2], aRecv[3], aRecv[4], aRecv[5], aRecv[6], aRecv[7], aRecv[8], aRecv[9]);
				break;
			}
		}
		break;
		}
	}
	else
	{
		switch (protoNum)
		{
		case 0x00:
			this->PChatProc((PMSG_CHATDATA *)aRecv, lpObj);
			break;
		case 0x01:
			this->CGChatRecv((PMSG_CHATDATA_NUMBER *)aRecv, lpObj);
			break;
		case 0x02:
			this->CGChatWhisperRecv((PMSG_CHATDATA_WHISPER *)aRecv, lpObj);
			break;
		case 0x03:
			this->CGCheckMainRecv((PMSG_CHECK_MAINEXE_RESULT *)aRecv, lpObj);
			break;
		case 0x0E:
			this->CGLiveClient((PMSG_CLIENTTIME *)aRecv, lpObj);
			break;
		case 0xD7:
			this->PMoveProc((PMSG_MOVE *)aRecv, lpObj);
			break;
		case 0x10:
			this->RecvPositionSetProc((PMSG_POSISTION_SET *)aRecv, lpObj);
			break;
		case 0xDF:
			this->CGAttack((PMSG_ATTACK *)aRecv, lpObj);
			break;
		case 0x18:
			if (this->DataEncryptCheck(lpObj, protoNum, Encrypt) != FALSE)
			{
				this->CGActionRecv((PMSG_ACTION *)aRecv, lpObj);
			}
			break;
		case 0x19:
			if (this->DataEncryptCheck(lpObj, protoNum, Encrypt) != FALSE)
			{
				this->CGMagicAttack(aRecv, lpObj);
			}
			break;
		case 0x1B:
			this->CGMagicCancel((PMSG_MAGICCANCEL *)aRecv, lpObj);
			break;
		case 0x1C:
			if (this->DataEncryptCheck(lpObj, protoNum, Encrypt) != FALSE)
			{
				this->CGTeleportRecv((PMSG_TELEPORT *)aRecv, lpObj);
			}
			break;
		case 0x8E:
			if (aRecv[3] == 0x02)
			{
				this->CGReqMapMove((PMSG_REQ_MAPMOVE *)aRecv, lpObj);
			}
			break;
		case 0xB0:
			if (this->DataEncryptCheck(lpObj, protoNum, Encrypt) != FALSE)
			{
				this->CGTargetTeleportRecv((PMSG_TARGET_TELEPORT *)aRecv, lpObj);
			}
			break;
		case 0xD3:
			this->CGBeattackRecv(aRecv, lpObj, FALSE);
			break;
		case 0x1E:
			if (this->DataEncryptCheck(lpObj, protoNum, Encrypt) != FALSE)
			{
				this->CGDurationMagicRecv(aRecv, lpObj);
			}
			break;
		case 0x22:
			if (this->DataEncryptCheck(lpObj, protoNum, Encrypt) != FALSE)
			{
				this->CGItemGetRequest((PMSG_ITEMGETREQUEST *)aRecv, lpObj, aRecv[3], aRecv[4]);
			}
			break;
		case 0x23:
			this->CGItemDropRequest((PMSG_ITEMTHROW *)aRecv, lpObj, 0);
			break;
		case 0x24:
			this->CGInventoryItemMove((PMSG_INVENTORYITEMMOVE *)aRecv, lpObj);
			break;
		case 0x26:
			this->CGUseItemRecv((PMSG_USEITEM *)aRecv, lpObj);
			break;
		case 0x30:
			if (this->DataEncryptCheck(lpObj, protoNum, Encrypt) != FALSE)
			{
				this->CGTalkRequestRecv((PMSG_TALKREQUEST *)aRecv, lpObj);
			}
			break;
		case 0x31:
			this->CGCloseWindow(lpObj);
			break;
		case 0x32:
			if (this->DataEncryptCheck(lpObj, protoNum, Encrypt) != FALSE)
			{
				this->CGBuyRequestRecv((PMSG_BUYREQUEST *)aRecv, lpObj);
			}
			break;
		case 0x33:
			if (this->DataEncryptCheck(lpObj, protoNum, Encrypt) != FALSE)
			{
				CGSellRequestRecv((PMSG_SELLREQUEST *)aRecv, lpObj);
			}
			break;
		case 0x34:
			this->CGModifyRequestItem((PMSG_ITEMDURREPAIR *)aRecv, lpObj);
			break;
		case 0x36:
			if (this->DataEncryptCheck(lpObj, protoNum, Encrypt) != FALSE)
			{
				this->CGTradeRequestSend((PMSG_TRADE_REQUEST *)aRecv, lpObj);
			}
			break;
		case 0x37:
			this->CGTradeResponseRecv((PMSG_TRADE_RESPONSE *)aRecv, lpObj);
			break;
		case 0x3A:
			this->CGTradeMoneyRecv((PMSG_TRADE_GOLD *)aRecv, lpObj);
			break;
		case 0x3C:
			if (this->DataEncryptCheck(lpObj, protoNum, Encrypt) != FALSE)
			{
				this->CGTradeOkButtonRecv((PMSG_TRADE_OKBUTTON *)aRecv, lpObj);
			}
			break;
		case 0x3D:
			if (this->DataEncryptCheck(lpObj, protoNum, Encrypt) != FALSE)
			{
				this->CGTradeCancelButtonRecv(lpObj);
			}
			break;
		/*case 0x3F:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x01:
				g_PersonalStore.CGPShopReqSetItemPrice((PMSG_REQ_PSHOP_SETITEMPRICE *)aRecv, lpObj);
				break;
			case 0x02:
				g_PersonalStore.CGPShopReqOpen((PMSG_REQ_PSHOP_OPEN *)aRecv, lpObj);
				break;
			case 0x03:
				g_PersonalStore.CGPShopReqClose(lpObj);
				break;
			case 0x05:
				g_PersonalStore.CGPShopReqBuyList((PMSG_REQ_BUYLIST_FROM_PSHOP *)aRecv, lpObj);
				break;
			case 0x06:
				g_PersonalStore.CGPShopReqBuyItem(aRecv, lpObj);
				break;
			case 0x07:
				g_PersonalStore.CGPShopReqCloseDeal((PMSG_REQ_PSHOPDEAL_CLOSE *)aRecv, lpObj);
				break;
			}
		}
		break;*/
		/*case 0x40:
			if (this->DataEncryptCheck(lpObj, protoNum, Encrypt) != FALSE)
			{
				this->CGPartyRequestRecv((PMSG_PARTYREQUEST *)aRecv, lpObj);
			}
			break;
		case 0x41:
			if (this->DataEncryptCheck(lpObj, protoNum, Encrypt) != FALSE)
			{
				this->CGPartyRequestResultRecv((PMSG_PARTYREQUESTRESULT *)aRecv, lpObj);
			}
			break;
		case 0x42:
			this->CGPartyList(lpObj);
			break;
		case 0x43:
			if (this->PacketCheckTime(lpObj) == TRUE)
			{
				this->CGPartyDelUser((PMSG_PARTYDELUSER *)aRecv, lpObj, 1);
			}
			break;
		case 0x4A:
			this->CGRageAttack(aRecv, lpObj);
			break;
		case 0x4B:
			this->CGRageAttackRange((PMSG_RAGE_ATTACK_RANGE *)aRecv, lpObj);
			break;
		case 0x50:
			this->CGGuildRequestRecv((PMSG_GUILDJOINQ *)aRecv, lpObj);
			break;
		case 0x51:
			this->CGGuildRequestResultRecv((PMSG_GUILDQRESULT *)aRecv, lpObj);
			break;
		case 0x52:
			this->CGGuildListAll(lpObj);
			break;
		case 0x53:
			this->CGGuildDelUser((PMSG_GUILDDELUSER *)aRecv, lpObj);
			break;
		case 0x54:
			this->CGGuildMasterAnswerRecv((PMSG_GUILDMASTERANSWER *)aRecv, lpObj);
			break;
		case 0x55:
			this->CGGuildMasterInfoSave(lpObj, (PMSG_GUILDINFOSAVE *)aRecv);
			break;
		case 0x57:
			this->CGGuildMasterCreateCancel(lpObj);
			break;
		case 0x61:
			this->GCGuildWarRequestSendRecv((PMSG_GUILDWARSEND_RESULT *)aRecv, lpObj);
			break;
		case 0x66:
			this->GCGuildViewportInfo((PMSG_REQ_GUILDVIEWPORT *)aRecv, lpObj);
			break;*/
		/*case 0x6E: // chat black list
		{
			PMSG_DEFAULT2* tmpHeader = (PMSG_DEFAULT2*)aRecv;
			switch (tmpHeader->subcode)
			{
			case 1:
				UserChatBlockMng::CGRequestAddName(lpObj, (CGRequestUserChatBlockAdd*)aRecv);
				break;
			case 2:
				UserChatBlockMng::CGRequestDeleteName(lpObj, (CGRequestUserChatBlockDelete*)aRecv);
				break;
			default:
				sLog->outBasic("[DEBUG] (UserChatBlockMng) not handled case: %X", tmpHeader->subcode);
				break;
			}
		}
		break;*/
		case 0x6F:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x00:
				g_CancelItemSale.CGReqSoldItemList(*lpObj);
				break;
			case 0x01:
				g_CancelItemSale.CGReqEndCancelItemSale(*lpObj);
				break;
			case 0x02:
				g_CancelItemSale.CGReqReBuyItem((PMSG_REQ_REBUY_ITEM *)aRecv, *lpObj);
				break;
			}
		}
		break;
		case 0xB1:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x01:
				if (this->DataEncryptCheck(lpObj, protoNum, Encrypt) != FALSE)
				{
					this->CGReqMapSvrAuth((PMSG_REQ_MAPSERVERAUTH *)aRecv, lpObj);
				}
				break;
			}
		}
		break;
		case 0xB2:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x00:
				//this->CGReqCastleSiegeState((PMSG_REQ_CASTLESIEGESTATE *)aRecv, lpObj);
				break;
			case 0x01:
				//this->CGReqRegCastleSiege((PMSG_REQ_REGCASTLESIEGE *)aRecv, lpObj);
				break;
			case 0x02:
				//this->CGReqGiveUpCastleSiege((PMSG_REQ_GIVEUPCASTLESIEGE *)aRecv, lpObj);
				break;
			case 0x03:
				//this->CGReqGuildRegInfo((PMSG_REQ_GUILDREGINFO *)aRecv, lpObj);
				break;
			case 0x04:
				//this->CGReqRegGuildMark((PMSG_REQ_REGGUILDMARK *)aRecv, lpObj);
				break;
			case 0x05:
				this->CGReqNpcBuy((PMSG_REQ_NPCBUY *)aRecv, lpObj);
				break;
			case 0x06:
				this->CGReqNpcRepair((PMSG_REQ_NPCREPAIR *)aRecv, lpObj);
				break;
			case 0x07:
				this->CGReqNpcUpgrade((PMSG_REQ_NPCUPGRADE *)aRecv, lpObj);
				break;
			case 0x08:
				this->CGReqTaxMoneyInfo((PMSG_REQ_TAXMONEYINFO *)aRecv, lpObj);
				break;
			case 0x09:
				this->CGReqTaxRateChange((PMSG_REQ_TAXRATECHANGE *)aRecv, lpObj);
				break;
			case 0x10:
				this->CGReqMoneyDrawOut((PMSG_REQ_MONEYDRAWOUT *)aRecv, lpObj);
				break;
			case 0x12:
				this->CGReqCsGateOperate((PMSG_REQ_CSGATEOPERATE *)aRecv, lpObj);
				break;
			case 0x1B:
				this->CGReqCsMiniMapData((PMSG_REQ_MINIMAPDATA *)aRecv, lpObj);
				break;
			case 0x1C:
				this->CGReqStopCsMiniMapData((PMSG_REQ_STOPMINIMAPDATA *)aRecv, lpObj);
				break;
			case 0x1D:
				this->CGReqCsSendCommand((PMSG_REQ_CSCOMMAND *)aRecv, lpObj);
				break;
			case 0x1F:
				this->CGReqCsSetEnterHuntZone((PMSG_REQ_CSHUNTZONEENTER *)aRecv, lpObj);
				break;
			}
		}
		break;
		case 0xB3:
			this->CGReqNpcDbList((PMSG_REQ_NPCDBLIST *)aRecv, lpObj);
			break;
		case 0xB4:
			//this->CGReqCsRegGuildList((PMSG_REQ_CSREGGUILDLIST *)aRecv, lpObj);
			break;
		case 0xB5:
			//this->CGReqCsAttkGuildList((PMSG_REQ_CSATTKGUILDLIST *)aRecv, lpObj);
			break;
		case 0xB7:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x01:
				this->CGReqWeaponUse((PMSG_REQ_USEWEAPON *)aRecv, lpObj);
				break;
			case 0x04:
				this->CGReqWeaponDamageValue((PMSG_REQ_WEAPON_DAMAGE_VALUE *)aRecv, lpObj);
				break;
			}
		}
		break;
		case 0xB9:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x02:
				this->CGReqGuildMarkOfCastleOwner((PMSG_REQ_GUILDMARK_OF_CASTLEOWNER *)aRecv, lpObj);
				break;
			case 0x05:
				CGReqCastleHuntZoneEntrance((PMSG_REQ_MOVE_TO_CASTLE_HUNTZONE *)aRecv, lpObj);
				break;
			}
		}
		break;
		case 0xBC:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x00:
				this->CGReqJewelMix((PMSG_REQ_JEWEL_MIX *)aRecv, lpObj);
				break;
			case 0x01:
				this->CGReqJewelUnMix((PMSG_REQ_JEWEL_UNMIX *)aRecv, lpObj);
				break;
			}
		}
		break;
		case 0xBD:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x00:
				this->CGReqCrywolfInfo((PMSG_REQ_CRYWOLF_INFO *)aRecv, lpObj);
				break;
			case 0x03:
				this->CGReqAlatrContract((PMSG_REQ_CRYWOLF_ALTAR_CONTRACT *)aRecv, lpObj);
				break;
			case 0x09:
				this->CGReqPlusChaosRate((PMSG_REQ_CRYWOLF_BENEFIT_PLUS_CHAOSRATE *)aRecv, lpObj);
				break;
			}
		}
		break;
		case 0xBF:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x02:
				this->CGReqIllusionTempleUseSkill((PMSG_REQ_USEILLUSIONTEMPLESKILL *)aRecv, lpObj);
				break;
			case 0x05:
				this->GCIllusionTempleSendReward(lpObj);
				break;
			case 0x0B:
				this->CGReqLuckyCoinInfo(lpObj);
				break;
			case 0x0C:
				this->CGReqLuckyCoinRegister(reinterpret_cast<PMSG_REQ_LUCKYCOIN_REGISTER *>(aRecv), lpObj);
				break;
			case 0x0D:
				this->CGReqLuckyCoinTrade(reinterpret_cast<PMSG_REQ_LUCKYCOIN_TRADE *>(aRecv), lpObj);
				break;
			case 0x0E:
				this->CGReqEnterDoppelGanger(reinterpret_cast<PMSG_REQ_ENTER_DOPPELGANGER *>(aRecv), lpObj);
				break;
			case 0x17:
			{
				if (lpObj->MapNumber == MAP_INDEX_LORENMARKET)
				{
					gObjMoveGate(*lpObj, 17);
				}
				else
				{
					gObjMoveGate(*lpObj, 333);
				}
			}
			break;
			case 0x20:
				this->CGInventoryEquipment((PMSG_REQ_INVENTORY_EQUIPMENT_ITEM *)aRecv, lpObj);
				break;
			case 0x6A:
				this->CGReq_ITL_Relics((PMSG_REQ_ITL_RELATE_RELICS *)aRecv, lpObj);
				break;
			case 0x51:
				this->CGRequestStartMuBot((PMSG_MUBOT_REQ_START*)aRecv, lpObj);
				break;
			case 0x70:
				this->CG_Req_Enter_ITR((PMSG_REQ_ENTER_ITR *)aRecv, lpObj);
				break;
			case 0x71:
				this->CG_ReqAcceptEnterITR((PMSG_REQ_ACCEPTENTER_ITR *)aRecv, lpObj);
				break;
			}
		}
		break;
		case 0xCD:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x01:
				this->CGReqUBFMyCharacterInfo((PMSG_REQ_UBF_INFO *)aRecv, lpObj);
				break;
			case 0x02:
				this->CGReqUBFJoin((PMSG_REQ_UBF_JOIN *)aRecv, lpObj);
				break;
			case 0x06:
				this->CGReqUBFGetReward((PMSG_REQ_GET_REWARD *)aRecv, lpObj);
				break;
			case 0x07:
				this->CGReqUBFCancel((PMSG_REQ_UBF_CANCEL *)aRecv, lpObj);
				break;
			case 0x08:
				this->CGReqUBFGetRealName((PMSG_REQ_UBF_REAL_NAME *)aRecv, lpObj);
				break;
			}
		}
		break;
		case 0xE1:
			//this->CGGuildAssignStatus((PMSG_GUILD_ASSIGN_STATUS_REQ *)aRecv, lpObj);
			break;
		case 0xE2:
			//this->CGGuildAssignType((PMSG_GUILD_ASSIGN_TYPE_REQ *)aRecv, lpObj);
			break;
		case 0xE5:
			//CGRelationShipReqJoinBreakOff((PMSG_RELATIONSHIP_JOIN_BREAKOFF_REQ *)aRecv, lpObj);
			break;
		case 0xE6:
			//this->CGRelationShipAnsJoinBreakOff((PMSG_RELATIONSHIP_JOIN_BREAKOFF_ANS *)aRecv, lpObj);
			break;
		case 0xE7:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x01:
				this->CGReqSendMemberPosInfoStart(lpObj);
				break;
			case 0x02:
				this->CGReqSendMemberPosInfoStop(lpObj);
				break;
			case 0x03:
				this->GCReqSendNpcPosInfo((PMSG_REQ_NPC_POSITION *)aRecv, lpObj);
				break;
			}
		}
		break;
		case 0xE9:
			//this->CGUnionList((PMSG_UNIONLIST_REQ *)aRecv, lpObj);
			break;
		case 0xEB:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x01:
				//this->CGRelationShipReqKickOutUnionMember((PMSG_KICKOUT_UNIONMEMBER_REQ *)aRecv, lpObj);
				break;
			}
		}
		break;
		case 0x71:
			this->GCPingSendRecv((PMSG_PING_RESULT *)aRecv, lpObj);
			break;
		case 0x72:
			if (this->DataEncryptCheck(lpObj, protoNum, Encrypt) != FALSE)
			{
				this->GCPacketCheckSumRecv((PMSG_PACKETCHECKSUM *)aRecv, lpObj);
			}
			break;
		case 0x78:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x01:
			{
				g_MixSystem.SocketItemUpgradeMixS12(*lpObj, aRecv[4], aRecv[5], aRecv[14], aRecv[15], aRecv[16], aRecv[17], aRecv[18], aRecv[23], aRecv[24], aRecv[25], aRecv[26], aRecv[27], aRecv[32]);
				break;
			}
			case 0x02:
			{
				g_MixSystem.SeedExtractMixS12(*lpObj, aRecv[4], aRecv[5], aRecv[6], aRecv[15], aRecv[24]);
				break;
			}
			case 0x03:
			{
				g_MixSystem.SetSeedSphereMixS12(*lpObj, aRecv[4], aRecv[5], aRecv[6], aRecv[7], aRecv[16]);
				break;
			}
			case 0x04:
			{
				g_MixSystem.SeedSphereRemoveMixS12(*lpObj, aRecv[4], aRecv[5], aRecv[6], aRecv[15], aRecv[16], aRecv[17], aRecv[18], aRecv[19], aRecv[24], aRecv[25], aRecv[26], aRecv[27], aRecv[28]);
				break;
			}
			case 0x05: //K2 Seed
			{
				g_MixSystem.SeedSphereCompositeMixS12(*lpObj, aRecv[4], aRecv[5], aRecv[6], aRecv[7]);
				break;
			}
			case 0x06: //K2 Seed
			{
				g_MixSystem.SeedSphereEnhance(*lpObj, aRecv[4], aRecv[5], aRecv[6]);
				break;
			}
			default:
				sLog->outError("%X %X %X %X %X %X %X %X %X %X %X %X", aRecv[0], aRecv[1], aRecv[2], aRecv[3], aRecv[4], aRecv[5], aRecv[6], aRecv[7], aRecv[8], aRecv[9], aRecv[10], aRecv[11]);
			}
		}
		break;
		case 0x81:
			this->CGWarehouseMoneyInOut(lpObj, (PMSG_WAREHOUSEMONEYINOUT *)aRecv);
			break;
		case 0x82:
			this->CGWarehouseUseEnd(lpObj);
			break;
		case 0x83:
			this->GCWarehouseRecivePassword(lpObj, (PMSG_WAREHOUSEPASSSEND *)aRecv);
			break;
		case 0x86:
			this->CGChaosBoxItemMixButtonClick((PMSG_CHAOSMIX *)aRecv, lpObj);
			break;
		case 0x87:
			this->CGChaosBoxUseEnd(lpObj);
			break;
		case 0x88:
			g_MixSystem.CheckEmptySpace_MultiMix((PMSG_REQ_CHAOS_MULTIMIX_CHECK *)aRecv, *lpObj);
			break;
		case 0x90:
			this->GCReqmoveDevilSquare((PMSG_REQ_MOVEDEVILSQUARE *)aRecv, lpObj);
			break;
		case 0x91:
			this->GCReqDevilSquareRemainTime((PMSG_REQ_DEVILSQUARE_REMAINTIME *)aRecv, lpObj);
			break;
		case 0x95:
			this->GCRegEventChipRecv((PMSG_REGEVENTCHIP *)aRecv, lpObj);
			break;
		case 0x96:
			this->GCGetMutoNumRecv((PMSG_GETMUTONUMBER *)aRecv, lpObj);
			break;
		case 0x97:
			this->GCUseEndEventChipRescv(lpObj);
			break;
		case 0x98:
			this->GCUseRenaChangeZenRecv((PMSG_EXCHANGE_EVENTCHIP *)aRecv, lpObj);
			break;
		case 0x99:
			this->CGReqMoveOtherServer((PMSG_REQ_MOVE_OTHERSERVER *)aRecv, lpObj);
			break;
		case 0xA0:
			//this->CGRequestQuestInfo(lpObj);
			break;
		case 0xA2:
			//this->CGSetQuestState((PMSG_SETQUEST *)aRecv, lpObj);
			break;
		case 0xA7:
			this->CGRequestPetItemCommand((PMSG_REQUEST_PET_ITEM_COMMAND *)aRecv, lpObj);
			break;
		case 0xA9:
			this->CGRequestPetItemInfo((PMSG_REQUEST_PET_ITEMINFO *)aRecv, lpObj);
			break;
		case 0xAA:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			WORD wPacketType = MAKEWORD(lpDef->subcode, 0xAA);
			switch (wPacketType)
			{
			case MSG_DUEL_INVITE:
			{
				LPPMSG_REQ_DUEL_INVITE lpReq = (LPPMSG_REQ_DUEL_INVITE)aRecv;
				int nRet = OnCGInviteDuel(lpReq, lpObj);
				if (nRet != ENEWPVP::E_NO_ERROR)
				{
					PMSG_ANS_DUEL_INVITE res = { 0 };
					res.h.c = 0xC1;
					res.h.size = sizeof(PMSG_ANS_DUEL_INVITE);
					res.h.headcode = 0xAA;
					res.h.subcode = 0x01;
					res.nResult = nRet;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&res, res.h.size);
				}
			}
			break;

			case MSG_DUEL_ANSWER:
			{
				LPPMSG_ANS_DUEL_ANSWER lpReq = (LPPMSG_ANS_DUEL_ANSWER)aRecv;
				int nRet = OnCGAnswerDuel(lpReq, lpObj);
			}
			break;

			case MSG_DUEL_LEAVE:
			{
				LPPMSG_REQ_DUEL_EXIT lpReq = (LPPMSG_REQ_DUEL_EXIT)aRecv;
				int nRet = OnCGLeaveDuel(lpReq, lpObj);
				if (nRet != ENEWPVP::E_NO_ERROR)
				{
					PMSG_ANS_DUEL_EXIT res = { 0 };
					res.h.c = 0xC1;
					res.h.size = sizeof(PMSG_ANS_DUEL_EXIT);
					res.h.headcode = 0xAA;
					res.h.subcode = 0x03;
					res.nResult = nRet;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&res, res.h.size);
				}
			}
			break;

			case MSG_DUEL_JOINCNANNEL:
			{
				int nRet = OnDuelChannelJoin((LPPMSG_REQ_DUEL_JOINCNANNEL)aRecv, lpObj);
				if (nRet != ENEWPVP::E_NO_ERROR)
				{
					PMSG_ANS_DUEL_JOINCNANNEL res = { 0 };
					res.h.c = 0xC1;
					res.h.size = sizeof(PMSG_ANS_DUEL_JOINCNANNEL);
					res.h.headcode = 0xAA;
					res.h.subcode = 0x07;
					res.nResult = nRet;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&res, res.h.size);
				}
			}
			break;

			case MSG_DUEL_LEAVECNANNEL:
			{
				LPPMSG_REQ_DUEL_LEAVECNANNEL lpReq = (LPPMSG_REQ_DUEL_LEAVECNANNEL)aRecv;
				int nRet = OnDuelChannelLeave(lpReq, lpObj);
				if (nRet != ENEWPVP::E_NO_ERROR)
				{
					PMSG_ANS_DUEL_LEAVECNANNEL res = { 0 };
					res.h.c = 0xC1;
					res.h.size = sizeof(PMSG_ANS_DUEL_LEAVECNANNEL);
					res.h.headcode = 0xAA;
					res.h.subcode = 0x09;
					res.nResult = nRet;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&res, res.h.size);
				}
			}
			break;
			}
		}
		break;
		case 0x9A:
			this->CGRequestEnterBloodCastle((PMSG_REQ_MOVEBLOODCASTLE*)aRecv, lpObj);
			break;
		case 0x9F:
			this->CGRequestEventEnterCount((PMSG_REQ_CL_ENTERCOUNT *)aRecv, lpObj);
			break;
		case 0x9D:
			this->CGRequestLottoRegister((PMSG_REQ_2ANV_LOTTO_EVENT *)aRecv, lpObj);
			break;
		case 0xAE:
			CGSaveMuBotSettings((PMSG_MUBOT_SETTINGS_RECV*)aRecv, lpObj);
			break;
		case 0xAF:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x01:
				this->CGRequestEnterChaosCastle((PMSG_REQ_MOVECHAOSCASTLE *)aRecv, lpObj);
				break;
			case 0x02:
				this->CGRequestRepositionUserInChaosCastle((PMSG_REQ_REPOSUSER_IN_CC *)aRecv, lpObj);
				break;
			case 3:
				CGReqCCF_DayTime(lpObj);
				break;
			case 5:
				CGReqCCF_EnterCheck(lpObj);
				break;
			case 6:
				CGRequestRepositionUserInCCF((PMSG_REQ_REPOSUSER_IN_CCF *)aRecv, lpObj);
				break;
			case 7:
				CGReq_CCF_Ranking((PMSG_REQ_CCF_RANKING *)aRecv, lpObj);
				break;
			case 8:
				//CG_CCF_UI_OnOff((PMSG_CCF_UI_ONOFF *)aRecv, lpObj);
				break;
			}
		}
		break;
		/*
		case 0xC0:
			FriendListRequest(lpObj);
			break;
		case 0xC1:
			FriendAddRequest((PMSG_FRIEND_ADD_REQ *)aRecv, lpObj);
			break;
		case 0xC2:
			WaitFriendAddRequest((PMSG_FRIEND_ADD_SIN_RESULT *)aRecv, lpObj);
			break;
		case 0xC3:
			FriendDelRequest((PMSG_FRIEND_DEL_REQ *)aRecv, lpObj);
			break;
		case 0xC4:
			FriendStateClientRecv((PMSG_FRIEND_STATE_C *)aRecv, lpObj);
			break;
		case 0xC5:
			FriendMemoSend((PMSG_FRIEND_MEMO *)aRecv, lpObj);
			break;
		case 0xC7:
			FriendMemoReadReq((PMSG_FRIEND_READ_MEMO_REQ *)aRecv, lpObj);
			break;
		case 0xC8:
			FriendMemoDelReq((PMSG_FRIEND_MEMO_DEL_REQ *)aRecv, lpObj);
			break;
		case 0xC9:
			FriendMemoListReq(lpObj);
			break;
		case 0xCA:
			FriendChatRoomCreateReq((PMSG_FRIEND_ROOMCREATE_REQ *)aRecv, lpObj);
			break;
		case 0xCB:
			FriendRoomInvitationReq((PMSG_ROOM_INVITATION *)aRecv, lpObj);
			break;
		*/
		case 0xD0:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x07: //WereWolf
				this->WereWolfQuarrelCheck(lpObj);
				break;
			case 0x08: //GateKeeper
				this->GateKeeperCheck(lpObj);
				break;
			case 0x0A: // Exit from Santa Village
				if (gGateC.CheckGateLevel(*lpObj, 22) == true)
				{
					gObjMoveGate(*lpObj, 22);
				}
				else
				{
					char szTemp[256];
					sprintf(szTemp, Lang.GetText(0, 211), gGateC.GetLevel(22));

					this->GCServerMsgStringSend(szTemp, lpObj, 1);
				}
				break;
			case 0x10: // Santa Clause
				//EGReqSantaGift(lpObj);
				break;
			case 0xF0:
				g_RuudStore.CGReqBuyItem((PMSG_REQ_RUUD_STORE_BUYITEM *)aRecv, *lpObj);
				break;
			case 0xF2:
				this->CGReqUseBoxInInventory(lpObj, (PMSG_REQ_USE_BOX *)aRecv);
				break;
			}
		}
		break;
		case 0xD1:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x00:
				this->CGReqKanturuStateInfo((PMSG_REQ_KANTURU_STATE_INFO *)aRecv, lpObj);
				break;
			case 0x01:
				this->GCReqEnterKanturuBossMap((PMSG_REQ_ENTER_KANTURU_BOSS_MAP *)aRecv, lpObj);
				break;
			}
		}
		break;
		/*
		case 0xD2:
		{

			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x01:
				g_CashShop.CGCashPoint(lpObj);
				break;
			case 0x02:
				g_CashShop.CGCashShopOpen(lpObj, (PMSG_REQ_INGAMESHOPOPEN *)aRecv);
				break;
			case 0x03:
				g_CashShop.CGCashItemBuy(lpObj, (PMSG_REQ_INGAMESHOP_ITEMBUY *)aRecv);
				break;
			case 0x04:
				g_CashShop.CGCashItemGift(lpObj, (PMSG_REQ_CASHITEM_GIFT *)aRecv);
				break;
			case 0x05:
				g_CashShop.CGCashInventoryItemCount(lpObj, (PMSG_REQ_INGAMESHOP_INVENTORY *)aRecv);
				break;
			case 0x0B:
				g_CashShop.CGCashInventoryItemUse(lpObj, (PMSG_REQ_INGAMESHOP_ITEMUSE *)aRecv);
				break;
			case 0x0F:
				g_CashShop.CGCashInventoryItemUseInfo(lpObj, (PMSG_REQ_INGAMESHOP_ITEMUSEINFO *)aRecv);
				break;
			default:
				sLog->outError("InGameShop Unknown Packet: %X %X", lpDef->h.headcode, lpDef->subcode);
			}
		}
		break;
		*/
		case 0xEC:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{

			case 0:
				this->CGReqInJewelPentagramItem((PMSG_REQ_IN_PENTAGRAM_JEWEL *)aRecv, lpObj);
				break;

			case 1:
				this->CGReqOutJewelPentagramItem((PMSG_REQ_OUT_PENTAGRAM_JEWEL *)aRecv, lpObj);
				break;

			case 2:
				g_PentagramMixSystem.CGPentagramJewelRefineRecv((PMSG_PENTAGRAM_JEWEL_REFINE_RECV*)aRecv, *lpObj);
				break;
			case 3:
				this->CGReqUpgradePentagramJewel((PMSG_REQ_UPGRADE_PENTAGRAM_JEWEL *)aRecv, lpObj);
				break;
			case 5:
				int SlotRadiance;
				SlotRadiance = aRecv[8];
				sLog->outBasic("[K2] Radiance Slot # %d %d %d %d", aRecv[7], aRecv[8], aRecv[9], aRecv[10]);
				if (lpObj->pntInventory[SlotRadiance]->m_Type == ITEMGET(14, 316))	//ITEMGET(13, 47[5])) //Ticket BC
					sLog->outBasic("[K2] Add Radiance Slot", lpDef->h.headcode, lpDef->subcode);
				else
					sLog->outBasic("[K2] Wrong Radiance Expansion", lpDef->h.headcode, lpDef->subcode);
				//Add Radiance Slot [9] = Radiance Expansion item [4] = Pentagram Slot
				break;

			case 0x31:
				//g_PersonalStore.CGReqSearchItemInPShop((PMSG_REQ_SEARCH_ITEM_PSHOP *)aRecv, lpObj);
				break;

			case 0x33:
				//g_PersonalStore.CGReqPShopLog((PMSG_REQ_PSHOP_LOG *)aRecv, lpObj);
				break;

			default:
				sLog->outError("%X %X %X %X %X %X %X %X %X %X %X %X", aRecv[0], aRecv[1], aRecv[2], aRecv[3], aRecv[4], aRecv[5], aRecv[6], aRecv[7], aRecv[8], aRecv[9], aRecv[10], aRecv[11]);
				break;
			}
		}

		break;

		case 0xDB:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x00:
				this->CGReqDSFSchedule(lpObj);
				break;
			case 0x01:
				//GDReqDSFCanPartyEnter(lpObj);
				break;
			case 0x02:
				this->CGReqAcceptEnterDSF((PMSG_REQ_DSF_ACCEPTENTER *)aRecv, lpObj);
				break;
			case 0x03:
				this->CGReqDSFGoFinalParty((PMSG_REQ_DSF_GO_FINAL_PARTY_INFO *)aRecv, lpObj);
				break;
			case 0x09:
				this->CGReqDSFGetReward((PMSG_REQ_DSF_GET_REWARD *)aRecv, lpObj);
				break;
			}
		}
		break;

		case 0xED: // Guild Matching
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0:
				this->CGReqGuildMatchingList((PMSG_REQ_GUILDMATCHINGLIST *)aRecv, lpObj);
				break;
			case 1:
				this->CGReqGuildMatchingListSearchWord((PMSG_REQ_GUILDMATCHINGLISTSEARCHWORD *)aRecv, lpObj);
				break;
			case 2:
				this->CGReqRegGuildMatchingList((PMSG_REQ_REGGUILDMATCHINGDATA *)aRecv, lpObj);
				break;
			case 3:
				this->CGReqCancelGuildMatchingList((PMSG_REQ_CANCEL_GUILDMATCHINGLIST *)aRecv, lpObj);
				break;
			case 4:
				this->CGReqJoinGuildMatching((PMSG_REQ_JOIN_GUILDMATCHING *)aRecv, lpObj);
				break;
			case 5:
				this->CGReqCancelJoinGuildMatching(lpObj);
				break;
			case 6:
				this->CGReqAllowJoinGuildMatching((PMSG_REQ_ALLOW_JOIN_GUILDMATCHING *)aRecv, lpObj);
				break;
			case 7:
				this->CGReqGetAllowListJoinGuildMatching((PMSG_REQ_ALLOWLIST_GUILDMATCHING *)aRecv, lpObj);
				break;
			case 8:
				this->CGReqGetWaitStateListGuildMatching(lpObj);
				break;
			default:
				return;
			}
		}
		break;

		case 0xEF: // Party Matching
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0:
				this->CGReqRegWantedPartyMember((PMSG_REQ_REG_WANTED_PARTYMEMBER *)aRecv, lpObj);
				break;
			case 1:
				this->CGReqGetPartyMatchingList((PMSG_REQ_GET_PARTYMATCHINGLIST *)aRecv, lpObj);
				break;
			case 2:
				this->CGReqJoinMemberPartyMatching((PMSG_REQ_MEMBERJOIN_PARTYMATCHINGLIST *)aRecv, lpObj);
				break;
			case 3:
				this->CGReqMemberJoinStateList((PMSG_REQ_MEMBERJOIN_STATELIST_PARTYMATCHING *)aRecv, lpObj);
				break;
			case 4:
				this->CGReqMemberJoinStateListLeader((PMSG_REQ_WAITLIST_PARTYMATCHING *)aRecv, lpObj);
				break;
			case 5:
				this->CGReqAcceptJoinMemberPartyMatching((PMSG_REQ_ACCEPTMEMBER_PARTYMATCHING *)aRecv, lpObj);
				break;
			case 6:
				this->CGReqCancelPartyMatching((PMSG_REQ_CANCEL_JOIN_PARTYMATCHING *)aRecv, lpObj);
				break;
			default:
				return;
			}
		}
		break;

		case 0xF6:
		{
			switch (aRecv[3])
			{
			case 0x30:
				CGReqQuestExp((PMSG_REQ_NPC_QUESTEXP *)aRecv, *lpObj);
				break;

			case 0x31:
				CGReqAttDefPowerInc((PMSG_REQ_ATTDEF_POWER_INC *)aRecv, *lpObj);
				break;

			case 0x21:
				CGReqEventItemQuestList((PMSG_REQ_EVENT_ITEM_EP_LIST *)aRecv, *lpObj);
				break;

			case 0x0A:
				CGReqQuestSwitch((PMSG_REQ_QUESTEXP *)aRecv, *lpObj);
				break;

			case 0x0B:
				CGReqQuestProgress((PMSG_REQ_QUESTEXP *)aRecv, *lpObj);
				break;

			case 0x0D:
				CGReqQuestComplete((PMSG_REQ_QUESTEXP_COMPLETE *)aRecv, *lpObj);
				break;

			case 0x0F:
				CGReqQuestGiveUp((PMSG_REQ_QUESTEXP_GIVEUP *)aRecv, *lpObj);
				break;

			case 0x1A:
				CGReqProgressQuestList((PMSG_REQ_QUESTEXP_PROGRESS_LIST *)aRecv, *lpObj);
				break;

			case 0x1B:
				CGReqProgressQuestInfo((PMSG_REQ_QUESTEXP_PROGRESS_INFO *)aRecv, *lpObj);
				break;

			case 0x10:
				CGReqTutorialKeyComplete((PMSG_REQ_QUESTEXP_ASK_COMPLETE *)aRecv, *lpObj);
				break;

			default:
				sLog->outError("UNKNOWN QUEST PROTOCOL (%x/%x/%x)", aRecv[2], aRecv[3], aRecv[4]);
				break;
			}
		}
		break;

		case 0x70:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			//sLog->outError("rare Item ticket 0x01");

			//int Slot = aRecv[4];
			//CItemObject* lpItem = NULL;;
			//BOOL cont = false;
			//lpItem = *lpObj.pInventory[Slot];

			//::gObjInventoryItemSet(lpObj, Slot, -1);
			//lpObj.pInventory[Slot].Clear();
			//this->GCInventoryItemDeleteSend(lpObj, Slot, 1);

			//::ItemCreate(lpObj, 235, 0, 0, ITEMGET(14, 13), 0, 0, 0, 0, 0, lpObj, 0, 0, 0, 0, 0);
		}
		break;
		case 0xF7:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x01:
				this->CGReqEnterZone((PMSG_REQ_ENTER_ZONE *)aRecv, lpObj);
				break;
			}
		}
		break;
		case 0xF8:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x01:
				this->CGReqRegGensMember((PMSG_REQ_REG_GENS_MEMBER *)aRecv, lpObj);
				break;
			case 0x03:
				this->CGReqSecedeGensMember((PMSG_REQ_SEGEDE_GENS_MEMBER *)aRecv, lpObj);
				break;
			case 0x09:
				this->CGReqGensReward((PMSG_GENS_REWARD_CODE *)aRecv, lpObj);
				break;
			case 0x0B:
				this->CGReqGensMemberInfo((PMSG_REQ_GENS_INFO *)aRecv, lpObj);
				break;
			case 0x20:
				this->CGReqAcheronEnter((PMSG_REQ_ACHERON_ENTER *)aRecv, lpObj);
				break;
			case 0x30:
				this->CGReqArcaBattleGuildMasterJoin((PMSG_REQ_ARCA_BATTLE_JOIN *)aRecv, lpObj);
				break;
			case 0x32:
				this->CGReqArcaBattleGuildMemberJoin((PMSG_REQ_ARCA_BATTLE_JOIN *)aRecv, lpObj);
				break;
			case 0x34:
				this->CGReqArcaBattleEnter((PMSG_REQ_ARCA_BATTLE_ENTER *)aRecv, lpObj);
				break;
			case 0x36:
				this->CGReqArcaBattleBootyExchange(lpObj);
				break;
			case 0x3C:
				this->CGReqSpritemapExchange(lpObj);
				break;
			case 0x41:
				this->CGReqRegisteredMemberCnt(lpObj);
				break;
			case 0x43:
				g_ArcaBattle.CGReqMarkReg(*lpObj);
				break;
			case 0x45:
				g_ArcaBattle.CGReqMarkRank(*lpObj);
				break;
			case 0x4B:
				//g_AcheronGuardianEvent.CGReqAcheronEventEnter((PMSG_REQ_ACHERON_EVENT_ENTER *)aRecv, *lpObj);
				break;
			default:
				sLog->outBasic("UNKNOWN F8 PACKET: %x %x %x %x %x %x %x %x %x %x", aRecv[0], aRecv[1], aRecv[2], aRecv[3], aRecv[4], aRecv[5], aRecv[6], aRecv[7], aRecv[8], aRecv[9]);
				break;
			}
		}
		break;
		case 0xFA://why wpe dont catch it lol ehh
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x08:
			{
				this->CGReqAntiCheatRecv(lpObj, (PMSG_SEND_AH_INFO *)aRecv);
			}
			break;
			case 0x09:
			{
				gObjUseSkill.SkillChangeUse(*lpObj);
				DGGuildMemberInfoRequest(lpObj);
			}
			break;
			case 0x0A:
			{
				this->CGReqAntihackBreach(lpObj, (PMSG_ANTIHACK_BREACH *)aRecv);
			}
			break;
			case 0x0D:
			{
				this->CGReqFileCrcRecv(lpObj, (PMSG_REQ_CRCDATA *)aRecv);
			}
			break;
			case 0x11:
				this->CGReqAntihackCheck(lpObj, (PMSG_ANTIHACK_CHECK *)aRecv);
				break;
			case 0x15:
				this->CGRecvHitHackValues(lpObj, (PMSG_SEND_HITHACK_INFO*)aRecv);
				break;
			}
		}
		break;

		case 0x4C:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x00:
				g_MineSystem.MineTwinkle((PMSG_REQ_MINETWINKLE *)aRecv, *lpObj);
				break;
			case 0x01:
				g_MineSystem.RewardMineTwinkle((PMSG_REQ_MINETWINKLE_REWARD *)aRecv, *lpObj);
				break;
			case 0x03:
				g_MineSystem.FailMineTwinkle((PMSG_ANS_MINETWINKLE_END_ANIMATION *)aRecv, *lpObj, false);
				break;
			}
		}
		break;

		case 0x4D:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x00:
				if (DataEncryptCheck(lpObj, protoNum, Encrypt))
				{
					this->CGEventItemGetRequest((PMSG_EVENTITEM_GETREQUEST *)aRecv, lpObj);
				}
				break;
			case 0x01:
				this->CGEventItemDropRequest((PMSG_EVENTITEM_THROW *)aRecv, lpObj);
				break;
			case 0x0F:
				this->CGReqEventInvenOpen(aRecv, lpObj);
				break;
			case 0x10:
				g_CMuRummyMng.CGReqMuRummyStart((PMSG_REQ_MURUMMY_INFO *)aRecv, *lpObj);
				break;
			case 0x11:
				g_CMuRummyMng.CGReqCardReveal((PMSG_REQ_REVEAL_CARD *)aRecv, *lpObj);
				break;
			case 0x12:
				g_CMuRummyMng.CGReqCardMove((PMSG_REQ_CARD_MOVE *)aRecv, *lpObj);
				break;
			case 0x13:
				g_CMuRummyMng.CGReqCardReMove((PMSG_REQ_CARD_REMOVE *)aRecv, *lpObj);
				break;
			case 0x14:
				g_CMuRummyMng.CGReqCardMatch((PMSG_REQ_CARD_MATCH *)aRecv, *lpObj);
				break;
			case 0x15:
				g_CMuRummyMng.CGReqMuRummyEnd((PMSG_REQ_MURUMMY_END *)aRecv, *lpObj);
				break;
			}
		}
		break;
		case 0x4E:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x00:
				if (DataEncryptCheck(lpObj, protoNum, Encrypt))
				{
					this->CGMuunItemGetRequest((PMSG_MUUNITEM_GETREQUEST *)aRecv, lpObj);
				}
				break;
			case 0x08:
				g_CMuunSystem.CGMuunInventoryUseItemRecv((PMSG_USEITEM_MUUN_INVEN *)aRecv, *lpObj);
				break;
			case 0x09:
				if (DataEncryptCheck(lpObj, protoNum, Encrypt))
				{
					CGMuunItemSellRequestRecv((PMSG_MUUN_ITEM_SELLREQUEST *)aRecv, lpObj);
				}
				break;
			case 0x11:
				g_CMuunSystem.CGReqRideSelect((PMSG_MUUN_RIDE_SELECT*)aRecv, *lpObj);
				break;
			case 0x13:
				g_CMuunSystem.CGMuunExchangeItem((PMSG_REQ_MUUN_EXCHANGE *)aRecv, *lpObj);
				break;
			}
		}
		break;
		case 0x35:
		{
			int Slot = aRecv[3];
			CItemObject* lpItem = NULL;;
			BOOL cont = false;
			lpItem = lpObj->pntInventory[Slot];
			if (lpItem->IsSetItem() != FALSE)
			{
				//return;
			}

			::gObjInventoryItemSet(*lpObj, Slot, -1);
			lpObj->pntInventory[Slot]->Clear();
			this->GCInventoryItemDeleteSend(lpObj, Slot, 1);
			::ItemCreate(lpObj, 235, 0, 0, ITEMGET(14, 290), 0, 0, 0, 0, 0, lpObj->m_Index, 0, 0, 0, 0, 0);

		}

		case 0x4F:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x02:
				g_GremoryCase.CGReqGetItemFromGremoryCase((PMSG_ADD_GREMORYCASE_ITEM_TO_INVENTORY_REQ *)aRecv, *lpObj);
				break;
			case 0x05:
				g_GremoryCase.CGReqOpenGremoryCase(*lpObj);
				break;
			}
		}
		break;
		case 0x76:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x01:
				this->CGReqNixieBossEnter((PMSG_REQ_NIXIELAKE_ENTER *)aRecv, lpObj);
				break;
			}
		}
		break;
		case 0xFB:
			break;
		case 0xFC:
			break;
		case 0x59:
			break;
		default:
			//sLog->outError("error-L2 (ProtocolCore) : Account: %s Name: %s : Invalid packet received OP: %X (%s,%d) Connection State: %d",
			//	lpObj.AccountID, lpObj.Name, protoNum, __FILE__, __LINE__, lpObj.Connected);
			this->GCSendDisableReconnect(lpObj);
			//GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
		}
	}
}


void GameProtocol::CGLiveClient(PMSG_CLIENTTIME * lpClientTime, CGameObject* lpObj)
{
#ifdef GS_AUTHORITY
	BYTE Decrypted[200];
	memcpy(Decrypted, lpClientTime, sizeof(PMSG_CLIENTTIME));
	//StringNarrow
	WLBufferDecrypt(&Decrypted[3], lpClientTime->h.size - 3, "12094389340236896570890436753");
	lpClientTime = (PMSG_CLIENTTIME *)Decrypted;
#endif

	if (lpObj->m_PlayerData->ConnectUser->Connected > PLAYER_CONNECTED)
	{
		lpObj->m_PlayerData->ConnectUser->ConnectCheckTime = WorldTimer::getMSTime();
	}

#ifndef GS_AUTHORITY
	return;
#endif

	if (lpObj->m_PlayerData->CheckSpeedHack != false)
	{
		DWORD dwTime = MAKE_NUMBERDW(lpClientTime->TimeH, lpClientTime->TimeL);

		int systemtick = GetTickCount() - lpObj->m_PlayerData->CheckTick2;
		int usertick = dwTime - lpObj->m_PlayerData->CheckTick;
		int checktime = systemtick - usertick;

		lpObj->m_PlayerData->CheckTickCount++;

		if (lpObj->m_PlayerData->CheckTickCount > 45)
		{
			lpObj->m_PlayerData->CheckTick = dwTime;
			lpObj->m_PlayerData->CheckTickCount = 0;
		}
	}

	if (lpObj->m_PlayerData->ConnectUser->Connected == PLAYER_PLAYING && g_ConfigRead.m_CheckSpeedHack != 0 && lpClientTime->AttackSpeed > lpObj->m_AttackSpeed &&
		abs((int)(lpObj->m_AttackSpeed - lpClientTime->AttackSpeed)) > g_ConfigRead.m_CheckSpeedHackTolerance)
	{
		sLog->outError("[%s][%s] Client Speed Hack Detected [%d][%d]", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_AttackSpeed, lpClientTime->AttackSpeed);
		GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
		return;
	}

	if (lpObj->m_PlayerData->ConnectUser->Connected == PLAYER_PLAYING && g_ConfigRead.m_CheckSpeedHack != 0 && lpClientTime->MagicSpeed > lpObj->m_MagicSpeed &&
		abs((int)(lpObj->m_MagicSpeed - lpClientTime->MagicSpeed)) > g_ConfigRead.m_CheckSpeedHackTolerance)
	{
		sLog->outError("[%s][%s] Client Speed Hack Detected [%d][%d]", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_MagicSpeed, lpClientTime->MagicSpeed);
		GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
		return;
	}

	/*if (lpObj->Connected == PLAYER_PLAYING)
	{
	if (g_ConfigRead.m_CheckSpeedHack == 1)
	{
	if ((lpObj->m_AttackSpeed + g_ConfigRead.m_CheckSpeedHackTolerance) < lpClientTime->AttackSpeed || (lpObj->m_MagicSpeed + g_ConfigRead.m_CheckSpeedHackTolerance) < lpClientTime->MagicSpeed)
	{
	g_Log.AddC(TColor::Red, "[%s][%s][%s] ClientSpeedHack Detected : Editing AttackSpeed [%d][%d] [%d][%d]", lpObj->AccountID, lpObj->Name, lpObj->m_PlayerData->IP, lpObj->m_AttackSpeed, lpClientTime->AttackSpeed, lpObj->m_MagicSpeed, lpClientTime->MagicSpeed);

	lpObj->m_AttackSpeedHackDetectedCount++;

	if (lpObj->m_AttackSpeedHackDetectedCount >= g_ConfigRead.m_CheckSpeedHackLimit)
	{
	IOCP.CloseClient(aIndex);
	}
	}
	else
	{
	lpObj->m_AttackSpeedHackDetectedCount = 0;
	}
	}
	}*/
}



void GameProtocol::MsgSendV2(CGameObject* lpObj, unsigned char* Msg, int size)
{
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		CGameObject* vp2 = getGameObject(lpObj->VpPlayer2[n]->index);
		if (vp2->m_PlayerData != nullptr)
		{
			if (vp2->m_PlayerData->ConnectUser->ConnectionState != 0)
			{
				GIOCP.DataSend(vp2->m_PlayerData->ConnectUser->Index, Msg, size);
			}
		}
	}
}

struct PMSG_CHECK_MAINEXE
{
	PBMSG_HEAD h;	// C1:03
	WORD m_wKey;	// 4
};



WORD GameProtocol::EncryptCheckSumKey(WORD wSource)
{
	WORD wRandom = rand() % 64;
	WORD wAcc = ((wSource & 0x3F0) * 64) | (wRandom * 16) | (wSource & 0x0F);
	return wAcc ^ 0xB479;
}




void GameProtocol::CGCheckMainRecv(PMSG_CHECK_MAINEXE_RESULT * lpMsg, CGameObject* lpObj)
{
	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	if (!gCheckSumSwitch)
	{
		lpObj->CheckSumTime = 0;
		return;
	}

	if (lpObj->CheckSumTableNum < 0 || lpObj->CheckSumTableNum > MAX_CHECKSUM_KEY - 1)
	{
		sLog->outBasic("error-L1 : [%s][%s] CheckSum-Exe error", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
		this->GCSendDisableReconnect(lpObj);
		//GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
		return;
	}

	if (dwgCheckSum[lpObj->CheckSumTableNum] != lpMsg->m_dwKey)
	{
		sLog->outBasic("error-L1 : CheckSum-Exe error %d %d %d [%s]", dwgCheckSum[lpObj->CheckSumTableNum],
			lpMsg->m_dwKey, lpObj->CheckSumTableNum, lpObj->m_PlayerData->ConnectUser->AccountID);
		this->GCSendDisableReconnect(lpObj);
		//GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
		return;
	}

	lpObj->CheckSumTime = 0;
}





void GameProtocol::PEchoProc(unsigned char * aMsg, int aLen, short lpObj)
{
	for each (std::pair<int,CUserData*> user in gUserObjects)
	{
		if (user.second->ConnectUser->Connected >= PLAYER_CONNECTED)
		{
			GIOCP.DataSend(user.first, aMsg, aLen);
		}
	}
}







void GameProtocol::GCResultSend(CGameObject* lpObj, BYTE headcode, BYTE result)
{
	PMSG_DEFRESULT pResult;

	PHeadSetB((LPBYTE)&pResult, headcode, sizeof(pResult));
	pResult.result = result;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);
}



void GameProtocol::ChatSend(CGameObject* lpObj, char* szChat)
{
	int len = strlen(szChat);

	if (len < 1)
		return;

	if (len > MAX_CHAT_LEN - 1)
		return;

	PMSG_CHATDATA_NUMBER pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x01, len + 6);
	strcpy(pMsg.chatmsg, szChat);
	pMsg.NumberH = SET_NUMBERH(lpObj->m_Index);
	pMsg.NumberL = SET_NUMBERL(lpObj->m_Index);

	this->MsgSendV2(lpObj, (UCHAR*)&pMsg, pMsg.h.size);
}



void GameProtocol::vChatSend(CGameObject* lpObj, char* szChat, ...)
{
	char szBuffer[256] = "";
	va_list pArguments;

	va_start(pArguments, szChat);
	vsprintf(szBuffer, szChat, pArguments);
	va_end(pArguments);

	int len = strlen(szBuffer);

	if (len < 1)
		return;

	if (len > MAX_CHAT_LEN - 1)
		return;

	/*	char szTempMsg[4096] = {0};
		MultiByteToWideChar(1258, 0, szChat, len, (unsigned short*)szTempMsg, sizeof(szTempMsg));
		memcpy(&pMsg.szChatMsg, szTempMsg, sizeof(pMsg.szChatMsg));
		pMsg.szChatMsg[MAX_CHAT_LEN-2]=0;
		pMsg.szChatMsg[MAX_CHAT_LEN-1]=0;
		PHeadSetB((LPBYTE)&pMsg, 0x01, wcslen((unsigned short*)pMsg.szChatMsg)*2 + (sizeof(GC_CHATSEND)-sizeof(pMsg.szChatMsg)));
		pMsg.sIndex[0] = lpObj->m_Index >> 8;
		pMsg.sIndex[1] = lpObj->m_Index & 0xFF;*/

	PMSG_CHATDATA_NUMBER pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x01, len + 6);
	strcpy(pMsg.chatmsg, szBuffer);
	pMsg.NumberH = SET_NUMBERH(lpObj->m_Index);
	pMsg.NumberL = SET_NUMBERL(lpObj->m_Index);

	this->MsgSendV2(lpObj, (UCHAR*)&pMsg, pMsg.h.size);
}





void GameProtocol::AllSendServerMsg(char* chatmsg)
{
	PMSG_NOTICE pNotice;

	TNotice::MakeNoticeMsg((TNotice*)&pNotice, 0x00, chatmsg);

	for each (std::pair<int, CUserData*> user in gUserObjects)
	{
		if (user.second->ConnectUser->Connected == PLAYER_PLAYING)
		{
			GIOCP.DataSend(user.first, (unsigned char*)&pNotice, pNotice.h.size);
		}
	}
}



void GameProtocol::DataSendAll(unsigned char* lpMsg, int iMsgSize)
{
	for each (std::pair<int, CUserData*> user in gUserObjects)
	{
		if (user.second->ConnectUser->Connected == PLAYER_PLAYING)
		{
			GIOCP.DataSend(user.first, (unsigned char*)lpMsg, iMsgSize);
		}
	}
}




void GameProtocol::ChatTargetSend(CGameObject* lpObj, char * szChat, int senduser)
{
	int len = strlen(szChat);

	if (len < 1 || len > MAX_CHAT_LEN - 1)
		return;

	PMSG_CHATDATA_NUMBER pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x01, len + 6);
	strcpy(pMsg.chatmsg, szChat);
	pMsg.NumberH = SET_NUMBERH(lpObj->m_Index);
	pMsg.NumberL = SET_NUMBERL(lpObj->m_Index);
	pMsg.chatmsg[len + 1] = 0;

	GIOCP.DataSend(senduser, (UCHAR*)&pMsg, pMsg.h.size);
}



struct CHAT_LOG_DATA
{
	PBMSG_HEAD h;	// C1:02
	WORD wServer;	// 4
	char AccountID[11];	// 6
	char Name[11];	// 11
	BYTE btType;	// 1C
	char szChatMsg[90];	// 1D
	char szTargetName[5][11];	// 5A
};

void GameProtocol::PChatProc(PMSG_CHATDATA * lpChat, CGameObject* lpObj)
{
	int n;
	int number;
	int slen = strlen(lpChat->chatmsg);

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	if (slen < 1)
		return;

	if (slen > MAX_CHAT_LEN - 1)
	{
		sLog->outBasic("[Anti-HACK][PChatProc][%s][%s] Chat Message Len : %d", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, slen);
		return;
	}

	if (GetTickCount() - lpObj->m_ChatFloodTime < 1000)
	{
		lpObj->m_ChatFloodCount++;

		if (lpObj->m_ChatFloodCount > 6)
		{
			sLog->outError("lpObj->m_ChatFloodCOunt > 6");
			//this->GCSendDisableReconnect(lpObj);
			GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
			return;
		}
	}

	else
	{
		lpObj->m_ChatFloodCount = 0;
	}

	lpObj->m_ChatFloodTime = GetTickCount();

	char szId[MAX_ACCOUNT_LEN + 1];
	szId[MAX_ACCOUNT_LEN] = 0;
	CHAT_LOG_DATA pChatMsg;

	if (g_ConfigRead.server.GetServerType() != SERVER_BATTLECORE)
	{
		memcpy(szId, lpObj->Name, MAX_ACCOUNT_LEN);
	}

	else
	{
		memcpy(szId, lpObj->m_PlayerData->m_RealNameOfUBF, MAX_ACCOUNT_LEN);
	}
	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		char szBuff[100];
		memset(szBuff, 0x00, sizeof(szBuff));

		if (lpChat->chatmsg[0] != '!' &&
			lpChat->chatmsg[0] != '`' &&
			lpChat->chatmsg[0] != '@' &&
			lpChat->chatmsg[0] != '$' &&
			lpChat->chatmsg[0] != '~')
		{
			if (lpChat->chatmsg[0] == '/')
			{
				sprintf(szBuff, "%s", lpChat->chatmsg);
			}

			else
			{
				sprintf(szBuff, "[#%d]%s", lpObj->m_PlayerData->m_nServerCodeOfHomeWorld, lpChat->chatmsg);
			}
		}

		else
		{
			sprintf(szBuff, "%c[#%d]%s", lpChat->chatmsg[0], lpObj->m_PlayerData->m_nServerCodeOfHomeWorld, lpChat->chatmsg);
		}

		int nSize = strlen(szBuff) - slen;
		lpChat->h.size += nSize;
		memcpy(lpChat->chatmsg, szBuff, lpChat->h.size);
	}

	memcpy(lpChat->chatid, szId, MAX_ACCOUNT_LEN);

	int szTargetNameCount = 0;
	if (gWriteChatLog)
	{
		pChatMsg.h.c = 0xC1;
		pChatMsg.h.headcode = 0x02;
		memcpy(pChatMsg.AccountID, lpObj->m_PlayerData->ConnectUser->AccountID, MAX_ACCOUNT_LEN);
		memcpy(pChatMsg.Name, lpObj->Name, MAX_ACCOUNT_LEN);
		pChatMsg.AccountID[MAX_ACCOUNT_LEN] = 0;
		pChatMsg.Name[MAX_ACCOUNT_LEN] = 0;
		pChatMsg.wServer = g_ConfigRead.server.GetGameServerCode();
		pChatMsg.btType = 0xFF;
	}

	//sLog->outBasic("[%s][%s]: %s",lpObj->m_PlayerData->ConnectUser->AccountID,lpObj->Name,lpChat->chatmsg);
	if ((lpObj->Penalty & 2) == 2)
		return;

	if (lpObj->m_PlayerData->m_bSecurityCheck == false)
	{
		char seps[2] = " ";
		char string[256];
		char * szCmdToken;
		strcpy(string, lpChat->chatmsg);
		szCmdToken = strtok(string, seps);

		if (lpChat->chatmsg[0] == '/' && cManager.GetCmd(szCmdToken) == 431)
		{
		}

		else
		{
			return;
		}
	}
	switch (lpChat->chatmsg[0])
	{
	case '!':	// Global Announcement
		if (slen > 2)
		{
			if ((lpObj->AuthorityCode & 1) == 1)
			{
				if (lpChat->chatmsg[1] == '!')
				{
					char Msg[128];
					sprintf(Msg, "[%s][%s]: %s", g_ConfigRead.server.GetServerName(), lpObj->Name, &lpChat->chatmsg[2]);
					GS_GDReqMapSvrMsgMultiCast(g_MapServerManager.GetMapSvrGroup(), Msg);

					sLog->outBasic("Announcement: %s %s %s", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, &lpChat->chatmsg[2]);

					if (gWriteChatLog)
					{
						memcpy(pChatMsg.szChatMsg, &lpChat->chatmsg[2], MAX_CHAT_LEN - 1);
						pChatMsg.szChatMsg[MAX_CHAT_LEN] = 0;
						pChatMsg.btType = 0x03;
					}
					return;
				}
				else
				{

					PMSG_NOTICE pNotice;
					TNotice::MakeNoticeMsgEx(&pNotice, 0, "%s : %s", lpObj->Name, &lpChat->chatmsg[1]);
					TNotice::SendNoticeToAllUser(&pNotice);

					sLog->outBasic("Announcement: %s %s %s", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, &lpChat->chatmsg[1]);

					if (gWriteChatLog)
					{
						memcpy(pChatMsg.szChatMsg, &lpChat->chatmsg[1], MAX_CHAT_LEN - 1);
						pChatMsg.szChatMsg[MAX_CHAT_LEN] = 0;
						pChatMsg.btType = 0x03;
					}
					return;
				}
			}
		}
		break;
	case '/':	// Command
		if (slen > 2)
		{
			cManager.ManagementProc(*lpObj, lpChat->chatmsg, *lpObj); 
			return;
		}
		break;
	}

	// Other Commands
	if (lpObj->ChatLimitTime > 0)
	{
		MsgOutput(*lpObj, Lang.GetText(0, 139), lpObj->ChatLimitTime);
		return;
	}

	// Party Message
	if (lpChat->chatmsg[0] == '~' || lpChat->chatmsg[0] == ']')
	{
		if (lpObj->m_PlayerData->m_bUsePartyMatching == false || lpObj->m_PlayerData->m_nPartyMatchingIndex < 0)
		{
			if (lpObj->PartyNumber >= 0)
			{
				int partycount = gParty.GetPartyCount(lpObj->PartyNumber);

				if (partycount >= 0)
				{
					if (gWriteChatLog)
					{
						memcpy(pChatMsg.szChatMsg, &lpChat->chatmsg[1], MAX_CHAT_LEN - 1);
						pChatMsg.szChatMsg[MAX_CHAT_LEN] = 0;
						pChatMsg.btType = 0x01;
					}

					for (n = 0; n < MAX_USER_IN_PARTY; n++)
					{
						number = gParty.m_PartyS[lpObj->PartyNumber].Number[n];

						if (number >= 0)
						{
							GIOCP.DataSend(number, (LPBYTE)lpChat, lpChat->h.size);

							if (gWriteChatLog)
							{
								strcpy(pChatMsg.szTargetName[szTargetNameCount], getGameObject(number)->Name);
								szTargetNameCount++;
							}
						}
					}
				}
			}
		}

		else
		{
			if (gWriteChatLog)
			{
				memcpy(pChatMsg.szChatMsg, &lpChat->chatmsg[1], MAX_CHAT_LEN - 1);
				pChatMsg.szChatMsg[MAX_CHAT_LEN] = 0;
				pChatMsg.btType = 0x01;
			}

			GDSendChatMsgPartyMatching(lpChat->chatmsg, lpObj->Name, lpObj->m_PlayerData->m_nPartyMatchingIndex);
		}
	}

	else if (lpChat->chatmsg[0] == '$')
	{
		if (lpObj->m_PlayerData->m_GensInfluence == DUPRIAN_INFLUENCE || lpObj->m_PlayerData->m_GensInfluence == VANERT_INFLUENCE)
		{
			for each (std::pair<int, CUserData*> user in gUserObjects)
			{
				if (/*gObjIsConnected(user.second) &&*/ user.second->m_GensInfluence == user.second->m_GensInfluence)
				{
					GIOCP.DataSend(n, (LPBYTE)lpChat, lpChat->h.size);
				}
			}

			GDGensServerGroupChattingSend(lpObj->m_PlayerData->m_GensInfluence, lpChat);
		}
	}

	// Guild
	else if (lpChat->chatmsg[0] == '@')
	{
		if (lpObj->m_PlayerData->GuildNumber > 0)
		{
			// Notice
			if (lpChat->chatmsg[1] == '>')
			{
				if (lpObj->Name[0] == lpObj->m_PlayerData->lpGuild->Users[0]->Name[0])
				{
					if (!strcmp(lpObj->Name, lpObj->m_PlayerData->lpGuild->Users[0]->Name))
					{
						for (int i = 0; i < strlen(lpChat->chatmsg); i++)
						{
							if (lpChat->chatmsg[i] == '%')
							{
								MsgOutput(*lpObj, Lang.GetText(0, 514));
								return;
							}
						}
						GDGuildNoticeSave(lpObj->m_PlayerData->lpGuild->Name, &lpChat->chatmsg[2]);
						sLog->outBasic("Guild announcement %s %s", lpObj->m_PlayerData->lpGuild->Name, lpChat->chatmsg);

						if (gWriteChatLog)
						{
							memcpy(pChatMsg.szChatMsg, &lpChat->chatmsg[2], MAX_CHAT_LEN - 2);
							pChatMsg.szChatMsg[MAX_CHAT_LEN] = 0;
							pChatMsg.btType = 0x04;
						}
					}
				}
			}
			// Aliances
			else if (lpChat->chatmsg[1] == '@' && lpObj->m_PlayerData->lpGuild->iGuildUnion)
			{
				int iGuildCount = 0;
				int iGuildList[MAX_UNION_GUILD] = { 0 };

				if (UnionManager.GetGuildUnionMemberList(lpObj->m_PlayerData->lpGuild->iGuildUnion, iGuildCount, iGuildList) == TRUE)
				{
					for (int i = 0; i < iGuildCount; i++)
					{
						GUILD_INFO_STRUCT *lpGuildInfo = Guild.SearchGuild_Number(iGuildList[i]);

						if (!lpGuildInfo)
							continue;

						for (n = 0; n < MAX_USER_GUILD; n++)
						{
							if (lpGuildInfo->Use[n])
							{
								number = lpGuildInfo->Index[n];
								CGameObject* lpObjGuild = getGameObject(number);
								if (number >= 0)
								{
									if (lpGuildInfo->Users[n]->Name[0] == lpObjGuild->Name[0])
									{
										if (!strcmp(lpGuildInfo->Users[n]->Name, lpObjGuild->Name))
										{
											GIOCP.DataSend(number, (LPBYTE)lpChat, lpChat->h.size);
										}
									}
								}
							}
						}
					}
				}

				if (g_iServerGroupUnionChatting == TRUE)
					GDUnionServerGroupChattingSend(lpObj->m_PlayerData->lpGuild->iGuildUnion, lpChat);
			}
			// Just Guild
			else
			{
				int count = lpObj->m_PlayerData->lpGuild->Count;

				if (count >= 0)
				{
					for (n = 0; n < MAX_USER_GUILD; n++)
					{
						if (lpObj->m_PlayerData->lpGuild->Use[n])
						{
							number = lpObj->m_PlayerData->lpGuild->Index[n];
							CGameObject* lpObjGuild = getGameObject(number);
							if (number >= 0)
							{
								if (lpObj->m_PlayerData->lpGuild->Users[n]->Name[0] == lpObjGuild->Name[0])
								{
									if (!strcmp(lpObj->m_PlayerData->lpGuild->Users[n]->Name, lpObjGuild->Name))
									{
										GIOCP.DataSend(number, (LPBYTE)lpChat, lpChat->h.size);
									}
								}
							}
						}
					}
				}

				if (g_iServerGroupGuildChatting == TRUE)
				{
					if (lpObj->m_PlayerData->lpGuild->Count > 1)
					{
						GDGuildServerGroupChattingSend(lpObj->m_PlayerData->lpGuild->Number, lpChat);
					}
				}

				if (gWriteChatLog)
				{
					memcpy(pChatMsg.szChatMsg, &lpChat->chatmsg[1], MAX_CHAT_LEN - 1);
					pChatMsg.szChatMsg[MAX_CHAT_LEN] = 0;
					pChatMsg.btType = 0x02;
				}
			}

			if (gWriteChatLog)
			{
				strcpy(pChatMsg.szTargetName[szTargetNameCount], lpObj->m_PlayerData->GuildName);
				szTargetNameCount++;
			}
		}
	}
	else
	{
		if ((lpObj->Authority & 0x20) == 0x20 && gObjCheckUsedBuffEffect(*lpObj, BUFFTYPE_INVISABLE) == TRUE)
		{
			return;
		}

		if (!g_prohibitedSymbols.Validate(lpChat->chatmsg, strlen(lpChat->chatmsg), TYPE_CHAT))
		{
			MsgOutput(*lpObj, Lang.GetText(0, 545));
			return;
		}

		if (SwearFilter.CompareText(lpChat->chatmsg) == true && lpChat->chatmsg[0] != '@' && lpChat->chatmsg[0] != '~')
		{
			++lpObj->m_PlayerData->SwearWarning;

			if (lpObj->m_PlayerData->SwearWarning >= 3)
			{
				MsgOutput(*lpObj, Lang.GetText(0, 515));
				//this->GCSendDisableReconnect(lpObj);
				GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
			}
			else
			{
				MsgOutput(*lpObj, Lang.GetText(0, 516), lpObj->m_PlayerData->SwearWarning);
			}
			return;
		}


		if (g_NewPVP.IsDuel(*lpObj) || g_NewPVP.IsObserver(*lpObj))
		{
			g_NewPVP.ChatMsgSend(*lpObj, (LPBYTE)lpChat, lpChat->h.size);
		}

		else
		{
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)lpChat, lpChat->h.size);

			if (g_GensSystem.IsMapBattleZone(lpObj->MapNumber))
			{
				g_GensSystem.BattleZoneChatMsgSend(*lpObj, (LPBYTE)lpChat, lpChat->h.size);
			}

			else
			{
				this->MsgSendV2(lpObj, (LPBYTE)lpChat, lpChat->h.size);
			}
		}

		if (gWriteChatLog)
		{
			memcpy(pChatMsg.szChatMsg, lpChat->chatmsg, MAX_CHAT_LEN);
			pChatMsg.szChatMsg[MAX_CHAT_LEN] = 0;
			pChatMsg.btType = 0x00;

		}
	}

	if (gWriteChatLog)
	{
		if (pChatMsg.btType != 0xFF)
		{
			pChatMsg.btType |= szTargetNameCount << 4;
			pChatMsg.h.size = sizeof(pChatMsg) - (5 - szTargetNameCount) - 1;
		}
	}
}


void GameProtocol::CGChatRecv(PMSG_CHATDATA_NUMBER * lpMsg, CGameObject* lpObj)
{
	if (PacketCheckTime(lpObj) == FALSE)
	{
		return;
	}

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)lpMsg, lpMsg->h.size);
	MsgSendV2(lpObj, (LPBYTE)lpMsg, lpMsg->h.size);
}



struct PMSG_SERVERMSG
{
	PBMSG_HEAD h;	// C1:0C
	BYTE MsgNumber;	// 3
};

void GameProtocol::GCServerMsgSend(BYTE msg, CGameObject* lpObj)
{
	PMSG_SERVERMSG pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x0C, sizeof(pMsg));
	pMsg.MsgNumber = msg;
	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}




void GameProtocol::GCServerMsgStringSend(LPSTR  szMsg, CGameObject* lpObj, BYTE type)
{
	PMSG_NOTICE pNotice;

	TNotice::MakeNoticeMsg((TNotice*)&pNotice, type, szMsg);
	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pNotice, pNotice.h.size);
}

void GameProtocol::GCServerMsgStringSendAll(LPSTR  szMsg, BYTE type)
{
	PMSG_NOTICE pNotice;

	TNotice::MakeNoticeMsg((TNotice*)&pNotice, type, szMsg);
	this->DataSendAll((UCHAR*)&pNotice, pNotice.h.size);
}

void GameProtocol::GCServerMsgStringSendAllEx(BYTE type, LPSTR szMsg, ...)
{
	va_list pArguments;
	char szBuffer[512];

	va_start(pArguments, szMsg);
	vsprintf(szBuffer, szMsg, pArguments);

	this->GCServerMsgStringSendAll(szBuffer, type);
}

void GameProtocol::GCServerMsgStringSendEx(CGameObject* lpObj, BYTE type, LPSTR szMsg, ...)
{
	va_list pArguments;
	char szBuffer[512];

	va_start(pArguments, szMsg);
	vsprintf(szBuffer, szMsg, pArguments);

	this->GCServerMsgStringSend(szBuffer, lpObj, type);
}


void GameProtocol::GCServerMsgStringSendGuild(GUILD_INFO_STRUCT* lpNode, LPSTR szMsg, BYTE type)
{
	if (lpNode == NULL)
	{
		return;
	}

	for (int n = 0; n < MAX_USER_GUILD; n++)
	{
		if (lpNode->Use[n] > 0 && lpNode->Users[n] >= 0)
		{
			GCServerMsgStringSend((char*)szMsg, lpNode->Users[n], type);
		}
	}
}



struct PMSG_EVENT
{
	PBMSG_HEAD h;	// C1:0B
	BYTE State;	// 3
	BYTE Event;	// 4
};

void GameProtocol::GCEventStateSend(CGameObject* lpObj, BYTE state, BYTE event)
{
	PMSG_EVENT pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x0B, sizeof(pMsg));
	pMsg.Event = event;
	pMsg.State = state;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}


void GameProtocol::GCMapEventStateSend(int map, BYTE state, BYTE event)
{
	PMSG_EVENT pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x0B, sizeof(pMsg));
	pMsg.Event = event;
	pMsg.State = state;

	for each(std::pair<int, CGameObject*> user in gGameObjects)
	{
		if (user.second->Type == OBJ_USER && map == user.second->MapNumber)
		{
			GIOCP.DataSend(user.first, (UCHAR*)&pMsg, pMsg.h.size);
		}
	}
}



void GameProtocol::CGChatWhisperRecv(PMSG_CHATDATA_WHISPER* lpMsg, CGameObject* lpObj)
{
	char tid[11];
	PMSG_CHATDATA_WHISPER pWhisper;
	int len;
	int index;

	if (PacketCheckTime(lpObj) == FALSE)
	{
		return;
	}

	if (lpObj->ChatLimitTime > 0)
	{
		MsgOutput(*lpObj, Lang.GetText(0, 139), lpObj->ChatLimitTime);
		return;
	}

	if ((lpObj->Penalty & 2) == 2)
	{
		return;
	}

	if (g_ConfigRead.data.common.SavePrivateChat == true)
	{
		sLog->outBasic(Lang.GetText(0, 517), lpObj->m_PlayerData->ConnectUser->AccountID, lpMsg->id, lpMsg->chatmsg);
	}

	tid[10] = 0;
	memcpy(tid, lpMsg->id, sizeof(lpMsg->id));
	index = WhisperCash.AddCash(tid);
	CGameObject* lpObjCash = getGameObject(index);

	if (index < 0)
	{
		if (g_ConfigRead.data.common.joinmuMultiChannelWhisper == true)
		{
			//ifjoinmu
			lpObj->m_Whispering->iWhisperSent = true;
			lpObj->m_Whispering->wResponseCount = 0;
			PMSG_RECV_CHATDATA_WHISPER pDSMsg;
			pDSMsg.h.set((LPBYTE)&pDSMsg, 0xC2, sizeof(pDSMsg));
			memcpy(pDSMsg.fromId, lpObj->Name, sizeof(pDSMsg.fromId));
			pDSMsg.wMapSvrNum = g_MapServerManager.GetMapSvrGroup();
			memcpy(pDSMsg.id, lpMsg->id, sizeof(lpMsg->id));
			memcpy(pDSMsg.chatmsg, lpMsg->chatmsg, sizeof(lpMsg->chatmsg));
			pDSMsg.OriginPlayerIndex = lpObj->m_PlayerData->ConnectUser->Index;
			//wsDataCli.DataSend((char *)&pDSMsg, pDSMsg.h.size); // TODO
			return;
		}
		else
		{
			GCServerMsgSend(0, lpObj);
			return;
		}
	}

	if (!UserChatBlockMng::getInstance()->isCanReceiveChat(lpObj->m_Index, index))
	{
		return;
	}

	PHeadSetB((LPBYTE)&pWhisper, 0x02, sizeof(pWhisper));
	strcpy(pWhisper.id, lpObj->Name);
	memcpy(pWhisper.chatmsg, lpMsg->chatmsg, sizeof(lpMsg->chatmsg));

	//pWhisper.szChatMsg[MAX_CHAT_LEN-2] = 0;
	//pWhisper.chatmsg[MAX_CHAT_LEN-1] = 0;
	len = strlen(pWhisper.chatmsg);

	if (lpObjCash->m_PlayerData->bActiveSetParty == true)
	{
		if (len >= 4)
		{
			if (!strcmp(pWhisper.chatmsg, lpObjCash->m_PlayerData->PartyPassword))
			{
				gObjAutoPartySet(*lpObjCash, *lpObj);
			}
		}
	}
	if (len > 0)
	{
		pWhisper.h.size -= sizeof(pWhisper.chatmsg);
		pWhisper.h.size += len + 1;

		if (g_GensSystem.IsMapBattleZone(lpObj->MapNumber))
		{
			int OwnInfluence = g_GensSystem.GetGensInfluence(*lpObj);
			int TarInfluence = g_GensSystem.GetGensInfluence(*lpObjCash);

			if (OwnInfluence == TarInfluence || CheckAuthorityCondition(98, *lpObj) || CheckAuthorityCondition(98, *lpObjCash))
			{
				GIOCP.DataSend(index, (UCHAR*)&pWhisper, pWhisper.h.size);
			}
		}

		else
		{
			GIOCP.DataSend(index, (UCHAR*)&pWhisper, pWhisper.h.size);
		}
	}
}


void GameProtocol::SCPJoinResultSend(CGameObject* lpObj, BYTE result)
{
	PMSG_JOINRESULT pResult;

	memset(&pResult, 0, sizeof(pResult));

	pResult.h.size = sizeof(pResult);
	pResult.h.c = 0xC1;
	pResult.h.headcode = 0xF1;
	pResult.scode = 0x00;
	pResult.result = result;
	pResult.NumberH = SET_NUMBERH(lpObj);
	pResult.NumberL = SET_NUMBERL(lpObj);
	pResult.CliVersion[0] = szClientVersion[0];
	pResult.CliVersion[1] = szClientVersion[1];
	pResult.CliVersion[2] = szClientVersion[2];
	pResult.CliVersion[3] = szClientVersion[3];
	pResult.CliVersion[4] = szClientVersion[4];

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (unsigned char*)&pResult, pResult.h.size);
	lpObj->m_PlayerData->ConnectUser->ConnectCheckTime = WorldTimer::getMSTime();
}


void GameProtocol::GCJoinBillCheckSend(LPSTR AccountId, CGameObject* lpObj)
{
	SDHP_BILLSEARCH pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x06, sizeof(pMsg));
	std::memcpy(pMsg.Id, AccountId, sizeof(pMsg.Id));
	pMsg.Number = lpObj->m_Index;

	//wsJServerCli.DataSend((char*)&pMsg, pMsg.h.size); // TODO
}




void GameProtocol::GCJoinResult(BYTE result, CGameObject* lpObj)
{
	PMSG_RESULT  pResult;

	PHeadSubSetB((LPBYTE)&pResult, 0xF1, 0x01, sizeof(pResult));
	pResult.result = result;
	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);
}



void GameProtocol::GCJoinSocketResult(BYTE result, SOCKET Socket)
{
	PMSG_RESULT pResult;

	PHeadSubSetB((LPBYTE)&pResult, 0xF1, 0x01, sizeof(pResult));
	pResult.result = result;

	//wsGServer.DataSocketSend(Socket, (char*)&pResult, pResult.h.size); // TODO
}




void GameProtocol::CGClientCloseMsg(PMSG_CLIENTCLOSE * lpMsg, CGameObject* lpObj)
{
	switch (lpMsg->Flag)
	{
	case 0:	// Close Game
		lpObj->m_PlayerData->ConnectUser->m_bOff = 0;
		gObjCloseSet(*lpObj, 0);
		break;

	case 2:	// ServerList
		lpObj->m_PlayerData->ConnectUser->m_bOff = 0;
		gObjCloseSet(*lpObj, 2);
		break;

	case 1: // Select Character
		lpObj->m_PlayerData->ConnectUser->m_bOff = 0;
		gObjCloseSet(*lpObj, 1);
		break;
	}
}




void GameProtocol::GCCloseMsgSend(CGameObject* lpObj, BYTE result)
{
	PMSG_RESULT pMsg;

	PHeadSubSetBE((LPBYTE)&pMsg, 0xF1, 0x02, sizeof(pMsg));
	pMsg.result = result;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}



void GameProtocol::CGClientMsg(PMSG_CLIENTMSG* lpMsg, CGameObject* lpObj)
{
	if (lpMsg->Flag)
	{

	}
	else
	{
		if (lpObj->m_PlayerData->ConnectUser->m_ClientHackLogCount < 100)
		{

		}

		lpObj->m_PlayerData->ConnectUser->m_ClientHackLogCount++;

		if (lpObj->m_PlayerData->ConnectUser->m_ClientHackLogCount >= 100)
			lpObj->m_PlayerData->ConnectUser->m_ClientHackLogCount = 100;
	}

	char msg[255];

	wsprintf(msg, "Client HackCheck %d", lpMsg->Flag);
}


struct SDHP_CREATECHAR
{
	PBMSG_HEAD h;	// C1:04
	int UserNumber;	// 4
	int DBNumber;	// 8
	short Number;	// C
	char AccountId[11];	// E
	char Name[11];	// 18
	BYTE ClassSkin;	// 22
};



struct SDHP_CHARDELETE
{
	PBMSG_HEAD h;	// C1:05
	short Number;	// 4
	char AccountID[11];	// 6
	char Name[11];	// 10
	BYTE Guild;	// [0:NoGuild] [1:Master] [2:Member] 1A
	char GuildName[8];	// 1B
};



void GameProtocol::CGPCharacterMapJoinRequest(PMSG_CHARMAPJOIN * lpMsg, CGameObject* lpObj)
{
	if (!PacketCheckTime(lpObj))
	{
		//JGCharacterCreateFailSend(lpObj, lpMsg->Name);
		return;
	}

	if (lpObj->m_PlayerData->ConnectUser->Connected < PLAYER_CONNECTED)
		return;

	if (lpObj->m_PlayerData->ConnectUser->Connected == PLAYER_PLAYING)
		return;

	lpObj->bEnableDelCharacter = FALSE;

	char _name[MAX_ACCOUNT_LEN + 1];
	SDHP_DBCHARINFOREQUEST pCRequest;

	PHeadSetB((LPBYTE)&pCRequest, 0x06, sizeof(pCRequest));
	memset(_name, 0, MAX_ACCOUNT_LEN);

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		std::string tmpCmpName;
		tmpCmpName.insert(0, lpMsg->Name, MAX_ACCOUNT_LEN);

		if (lpObj->m_PlayerData->m_NameConvertOfUBF.IsDuplicationOfName() == TRUE)
		{
			memcpy(_name, lpObj->m_PlayerData->m_NameConvertOfUBF.ConvertSlotIndexToName(lpMsg->btSlotIndex), MAX_ACCOUNT_LEN);
		}

		else
		{
			memcpy(_name, lpObj->m_PlayerData->m_NameConvertOfUBF.ConvertRealNameToName(tmpCmpName.c_str()), MAX_ACCOUNT_LEN);
		}
	}

	else
	{
		memcpy(_name, lpMsg->Name, MAX_ACCOUNT_LEN);
	}

	memcpy(pCRequest.Name, _name, MAX_ACCOUNT_LEN);
	strcpy(pCRequest.AccountID, lpObj->m_PlayerData->ConnectUser->AccountID);
	pCRequest.Number = lpObj->m_PlayerData->ConnectUser->Index;

	//wsDataCli.DataSend((char*)&pCRequest, pCRequest.h.size); // TODO
}


struct PMSG_LEVELUP
{
	PBMSG_HEAD h;	// C1:F3:05
	BYTE subcode;	// 3
	WORD Level;	// 4
	WORD LevelUpPoint;	// 6
	WORD MaxLife;	// 8
	WORD MaxMana;	// A
	WORD wMaxShield;	// C
	WORD MaxBP;	// E
	short AddPoint;	// 10
	short MaxAddPoint;	// 12
	short MinusPoint;	// 14
	short MaxMinusPoint;	// 16
	int IGCMaxLife;
	int IGCMaxMana;
};


void GameProtocol::GCLevelUpMsgSend(CGameObject* lpObj, int iSendEffect)
{
	short AddPoint = 0;
	short MaxAddPoint = 0;
	short MinusPoint = 0;
	short MaxMinusPoint = 0;

	gObjGetStatPointState(*lpObj, AddPoint, MaxAddPoint, MinusPoint, MaxMinusPoint);

	PMSG_LEVELUP pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x05, sizeof(pMsg));
	pMsg.Level = lpObj->Level;
	pMsg.LevelUpPoint = lpObj->m_PlayerData->LevelUpPoint;
	pMsg.MaxLife = lpObj->MaxLife + lpObj->AddLife;
	pMsg.MaxMana = lpObj->MaxMana + lpObj->AddMana;
	pMsg.MaxBP = lpObj->MaxBP + lpObj->AddBP;
	pMsg.wMaxShield = lpObj->iMaxShield + lpObj->iAddShield;
	pMsg.AddPoint = AddPoint;
	pMsg.MaxAddPoint = MaxAddPoint;
	pMsg.MinusPoint = MinusPoint;
	pMsg.MaxMinusPoint = MaxMinusPoint;
	pMsg.IGCMaxLife = lpObj->MaxLife + lpObj->AddLife;
	pMsg.IGCMaxMana = lpObj->MaxMana + lpObj->AddMana;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);

	if (iSendEffect == 1)
		GCSendEffectInfo(lpObj, 0x10);
}

void GameProtocol::CGLevelUpPointAdd(PMSG_LVPOINTADD * lpMsg, CGameObject* lpObj)
{
	if (!gObjIsConnected(*lpObj))
	{
		this->GCSendDisableReconnect(lpObj);
		//GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
		return;
	}

	if (lpObj->Type != OBJ_USER)
	{
		this->GCSendDisableReconnect(lpObj);
		//GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
		return;
	}

	//EnterCriticalSection(&lpObj->m_PlayerData->AgiCheckCriti);

	lpObj->m_PlayerData->AgilityCheckDelay = GetTickCount();

	PMSG_LVPOINTADDRESULT pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x06, sizeof(pMsg));
	pMsg.ResultType = 0;

	if (::gObjLevelUpPointAdd(lpMsg->Type, *lpObj) == TRUE)
	{
		pMsg.ResultType = 0x10;
		pMsg.ResultType += lpMsg->Type;

		switch (lpMsg->Type)
		{
		case 2:	// Vit
			pMsg.MaxLifeAndMana = lpObj->MaxLife + lpObj->AddLife;
			pMsg.IGCMaxLifeAndMana = lpObj->MaxLife + lpObj->AddLife;
			break;

		case 3:	// Energy
			pMsg.MaxLifeAndMana = lpObj->MaxMana + lpObj->AddMana;
			pMsg.IGCMaxLifeAndMana = lpObj->MaxMana + lpObj->AddMana;
			break;
		}

		pMsg.wMaxShield = lpObj->iMaxShield + lpObj->iAddShield;
		gObjSetBP(*lpObj);
		pMsg.MaxBP = lpObj->MaxBP + lpObj->AddBP;
	}

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR *)&pMsg, pMsg.h.size);
	//LeaveCriticalSection(&lpObj->m_PlayerData->AgiCheckCriti);

	GSProtocol.GCPlayerStatsPanelNew(lpObj);
	GSProtocol.GCPlayerStatsPanelRates(lpObj);
}


struct PMSG_INVENTORYITEMMODIFY
{
	PBMSG_HEAD h;	// C1:F3:14
	BYTE subcode;	// 3
	BYTE Pos;	// 4
	BYTE ItemInfo[MAX_ITEM_INFO];	// 5

};

void GameProtocol::GCInventoryItemOneSend(CGameObject* lpObj, int pos)
{
	if (!lpObj->pntInventory[pos]->IsItem())
		return;

	PMSG_INVENTORYITEMMODIFY pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x14, sizeof(pMsg));
	pMsg.Pos = pos;
	ItemByteConvert(pMsg.ItemInfo, *lpObj->pntInventory[pos]);

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR *)&pMsg, pMsg.h.size);
}


struct PMSG_PKLEVEL
{
	PBMSG_HEAD h;	// C1:F3:08
	BYTE subcode;	// 3
	BYTE NumberH;	// 4
	BYTE NumberL;	// 5
	BYTE PkLevel;	// 6
};

void GameProtocol::GCPkLevelSend(CGameObject* lpObj, BYTE pklevel)
{
	PMSG_PKLEVEL pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x08, sizeof(pMsg));
	pMsg.NumberH = SET_NUMBERH(lpObj);
	pMsg.NumberL = SET_NUMBERL(lpObj);
	pMsg.PkLevel = pklevel;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
	MsgSendV2(lpObj, (UCHAR*)&pMsg, pMsg.h.size);
}



#pragma pack(1)
struct PMSG_MAGICLIST	//Revised to 1.01.03
{
	char Pos;	// 0
	WORD wSkillNum; //3
	BYTE btSkillLevel;	// 5
};
#pragma pack()

struct PMSG_MAGICLISTCOUNT
{
	PWMSG_HEAD h;
	BYTE subcode;
	BYTE Count;
	BYTE btListType;
};

void GameProtocol::GCMagicListOneSend(CGameObject* lpObj, char Pos, WORD type, BYTE level, WORD skill, BYTE btListType)
{
	PMSG_MAGICLISTCOUNT pCount;
	PMSG_MAGICLIST pList;
	BYTE sendbuf[1000];

	int lOfs = sizeof(pCount);
	PHeadSubSetW((LPBYTE)&pCount, 0xF3, 0x11, 0);

	pCount.Count = 0xFE;
	pCount.btListType = btListType;

	pList.Pos = Pos;
	pList.wSkillNum = type;
	pList.btSkillLevel = level << 3;
	pList.btSkillLevel |= (BYTE)skill & 0x07;

	memcpy(&sendbuf[lOfs], &pList, sizeof(pList));
	lOfs += sizeof(pList);

	pCount.h.sizeH = SET_NUMBERH(lOfs);
	pCount.h.sizeL = SET_NUMBERL(lOfs);
	memcpy(sendbuf, &pCount, sizeof(pCount));

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, sendbuf, lOfs);
}


void GameProtocol::GCMagicListOneDelSend(CGameObject* lpObj, char Pos, WORD type, BYTE level, WORD skill, BYTE btListType)
{
	PMSG_MAGICLISTCOUNT pCount;
	PMSG_MAGICLIST pList;
	BYTE sendbuf[1000];

	int lOfs = sizeof(pCount);
	PHeadSubSetW((LPBYTE)&pCount, 0xF3, 0x11, 0);

	pCount.h.sizeH = 0;
	pCount.h.sizeL = 0;
	pCount.Count = 0xFF;
	pCount.btListType = btListType;

	pList.Pos = Pos;
	pList.wSkillNum = type;
	pList.btSkillLevel = level << 3;
	pList.btSkillLevel |= (BYTE)skill & 0x07;

	memcpy(&sendbuf[lOfs], &pList, sizeof(pList));
	lOfs += sizeof(pList);

	pCount.h.sizeH = SET_NUMBERH(lOfs);
	pCount.h.sizeL = SET_NUMBERL(lOfs);
	memcpy(sendbuf, &pCount, sizeof(pCount));

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, sendbuf, lOfs);
}


void GameProtocol::GCMagicListMultiSend(CGameObject* lpObj, BYTE btListType)
{
	WORD BrandSkill;
	PMSG_MAGICLISTCOUNT pCount;
	PMSG_MAGICLIST pList;
	BYTE sendbuf[1000];

	int lOfs = sizeof(pCount);
	int Count = 0;
	pCount.btListType = btListType;

	PHeadSubSetW((LPBYTE)&pCount, 0xF3, 0x11, 0);

	for (int n = 0; n < MAX_MAGIC; n++)
	{
		if (lpObj->Magic[n].IsMagic() == TRUE)
		{
			if (MagicDamageC.GetSkillUseType(lpObj->Magic[n].m_Skill) == 3)
			{
				continue;
			}

			if (!g_MasterLevelSkillTreeSystem.CheckUsableWeaponSkill(*lpObj, n))
			{
				continue;
			}

			if (g_MasterLevelSkillTreeSystem.IsExistMLSBasedSkill(*lpObj, lpObj->Magic[n].m_Skill))
			{
				continue;
			}

			if (lpObj->Magic[n].m_Pass == true)
			{
				continue;
			}

			pList.Pos = n;
			pList.wSkillNum = (WORD)lpObj->Magic[n].m_Skill;
			pList.btSkillLevel = lpObj->Magic[n].m_Level << 3;
			pList.btSkillLevel |= lpObj->Magic[n].m_Skill & 0x07;

			memcpy(&sendbuf[lOfs], &pList, sizeof(pList));
			lOfs += sizeof(pList);

			Count++;

			if (MagicDamageC.IsItemAttachedSkill(lpObj->Magic[n].m_Skill))
			{
				BrandSkill = MagicDamageC.GetNormalBrandOfSkill(lpObj->Magic[n].m_Skill);

				if (!lpObj->pntInventory[1]->IsItem() || lpObj->pntInventory[1]->m_Special[0] != BrandSkill)
				{
					continue;
				}

				if (!lpObj->pntInventory[0]->IsItem() || lpObj->pntInventory[0]->m_Special[0] != BrandSkill)
				{
					continue;
				}

				pList.Pos = n + 1;
				pList.wSkillNum = LOWORD(lpObj->Magic[n].m_Skill);
				pList.btSkillLevel = lpObj->Magic[n].m_Level << 3;
				pList.btSkillLevel |= lpObj->Magic[n].m_Skill & 0x07;

				memcpy(&sendbuf[lOfs], &pList, sizeof(pList));
				lOfs += sizeof(pList);

				Count++;
			}
		}
	}

	pCount.Count = Count;
	pCount.h.sizeH = SET_NUMBERH(lOfs);
	pCount.h.sizeL = SET_NUMBERL(lOfs);
	memcpy(sendbuf, &pCount, sizeof(pCount));

	GIOCP.DataSend(lpObj->m_Index, sendbuf, lOfs);
}



struct PMSG_EQUIPMENTLIST
{
	PBMSG_HEAD h;	// C1:F3:13
	BYTE subcode;	// 3
	BYTE NumberH;	// 4
	BYTE NumberL;	// 5
	BYTE Equipment[CHAR_SET_SIZE];	// 6
};

void GameProtocol::GCEquipmentSend(CGameObject* lpObj)
{
	PMSG_EQUIPMENTLIST pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x13, sizeof(pMsg));
	pMsg.NumberH = SET_NUMBERH(lpObj);
	pMsg.NumberL = SET_NUMBERL(lpObj);
	gObjMakePreviewCharSet(*lpObj);
	memcpy(pMsg.Equipment, lpObj->CharSet, sizeof(lpObj->CharSet));

	MsgSendV2(lpObj, (LPBYTE)&pMsg, pMsg.h.size);
}


struct PMSG_RECALLMONLIFE
{
	PBMSG_HEAD h;	// C1:F3:20
	BYTE subcode;	// 3
	BYTE Life;	// 4
};

void GameProtocol::GCRecallMonLife(CGameObject* lpObj, int maxlife, int life)
{
	if (maxlife <= 0)
		return;

	PMSG_RECALLMONLIFE pMsg;
	BYTE per = life * 100 / maxlife;
	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x20, sizeof(pMsg));
	pMsg.Life = per;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}



struct PMSG_TIMEVIEW
{
	PBMSG_HEAD h;	// C1:F3:22
	BYTE subcode;	// 3
	WORD Second;	// 4
};

void GameProtocol::GCTimeViewSend(CGameObject* lpObj, int second)
{
	PMSG_TIMEVIEW pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x22, sizeof(pMsg));
	pMsg.Second = second;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR *)&pMsg, pMsg.h.size);
}



struct PMSG_GOALSEND
{
	PBMSG_HEAD h;	// C1:F3:23
	BYTE subcode;	// 3
	char RedTeamName[8];	// 4
	BYTE RedTeamScore;	// C
	char BlueTeamName[8];	// D
	BYTE BlueTeamScore;	// 15
};

void GameProtocol::GCGoalSend(CGameObject* lpObj, char* Name1, BYTE score1, char* Name2, BYTE score2)
{
	PMSG_GOALSEND pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x23, sizeof(pMsg));
	memcpy(pMsg.RedTeamName, Name1, sizeof(pMsg.RedTeamName));
	pMsg.RedTeamScore = score1;
	memcpy(pMsg.BlueTeamName, Name2, sizeof(pMsg.BlueTeamName));
	pMsg.BlueTeamScore = score2;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}



void GameProtocol::GCSkillKeyRecv(PMSG_SKILLKEY * lpMsg, CGameObject* lpObj)
{
	// Uneeded as if assigned a gameObject then there playing.
	//if (lpObj->m_PlayerData->ConnectUser->ConnectionState < PLAYER_PLAYING)
	//	return;

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	/*DGOptionDataSend(lpObj, lpObj->Name,
		lpMsg->SkillKey, lpMsg->GameOption,
		lpMsg->QkeyDefine, lpMsg->WkeyDefine, lpMsg->EkeyDefine, lpMsg->ChatWindow, lpMsg->RkeyDefine, ntohl(lpMsg->QWERLevel), lpObj->m_PlayerData->m_EnableUseChangeSkin);*/ // TODO
}





void GameProtocol::GCSkillKeySend(CGameObject* lpObj, LPBYTE keybuffer, BYTE GO, BYTE Qk, BYTE Wk, BYTE Ek, BYTE ChatWnd, BYTE Rk, int QWER)
{
	PMSG_SKILLKEY pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x30, sizeof(pMsg));
	memcpy(pMsg.SkillKey, keybuffer, sizeof(pMsg.SkillKey));
	pMsg.GameOption = GO;
	pMsg.QkeyDefine = Qk;
	pMsg.WkeyDefine = Wk;
	pMsg.EkeyDefine = Ek;
	pMsg.ChatWindow = ChatWnd;
	pMsg.RkeyDefine = Rk;
	pMsg.QWERLevel = QWER;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}


struct PMSG_ITEMGETRESULT
{
	PBMSG_HEAD h;	// C1:22
	BYTE result;	// [0xFE:Money] 3
	BYTE i;
	BYTE ii;
	BYTE Data[MAX_ITEM_INFO];	// 4
	BYTE x;
	BYTE xx;
};

void GameProtocol::GCMoneySend(CGameObject* lpObj, DWORD money)
{
	PMSG_ITEMGETRESULT pMsg;

	pMsg.h.c = 0xC3;
	pMsg.h.headcode = 0x22;
	pMsg.h.size = sizeof(pMsg);
	pMsg.result = 0xFE;

	WORD hiWord = SET_NUMBERHW(money);
	WORD loWord = SET_NUMBERLW(money);
	pMsg.Data[0] = SET_NUMBERH(hiWord);
	pMsg.Data[1] = SET_NUMBERL(hiWord);
	pMsg.Data[2] = SET_NUMBERH(loWord);
	pMsg.Data[3] = SET_NUMBERL(loWord);

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}


void GameProtocol::GCItemInventoryPutSend(CGameObject* lpObj, BYTE result, BYTE iteminfo1, BYTE iteminfo2)
{
	PMSG_ITEMGETRESULT pResult;

	pResult.h.c = 0xC3;
	pResult.h.headcode = 0x22;
	pResult.h.size = sizeof(pResult);
	pResult.result = result;
	pResult.Data[0] = iteminfo1;
	pResult.Data[1] = iteminfo2;
	pResult.h.size -= 2;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);
}



void GameProtocol::CGItemGetRequest(PMSG_ITEMGETREQUEST * lpMsg, CGameObject* lpObj, BYTE ID, BYTE ID1)
{
	int item_num, map_num;
	PMSG_ITEMGETRESULT pResult;
	int type;
	int level;
	int special;
	int NOption;
	char szItemName[50];
	CMapItem* lpItem;

	pResult.h.c = 0xC3;
	pResult.h.headcode = 0x22;
	pResult.h.size = sizeof(pResult);
	pResult.result = -1;

	if (!gObjIsConnected(*lpObj))
	{
		GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
		return;
	}

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	if (lpObj->m_PlayerData->ConnectUser->CloseType != -1)
		return;

	if (lpObj->m_PlayerData->m_bSecurityCheck == false)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	if (lpObj->DieRegen != 0)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	if (g_NewPVP.IsObserver(*lpObj) == TRUE)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	if (lpObj->m_IfState.use != 0)
	{
		if (lpObj->m_IfState.type != 3)
		{
			this->GCServerMsgStringSend(Lang.GetText(0, 31), lpObj, 1);
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

			return;
		}
	}

	if (!::gObjFixInventoryPointer(*lpObj))

		if (lpObj->pTransaction == TRUE)
		{
			return;

		}

	CMuRummyInfo * pMuRummyInfo = lpObj->m_PlayerData->m_pCMuRummyInfo;

	if (!pMuRummyInfo)
	{
		return;
	}

	if (pMuRummyInfo->IsWaitReward() == true)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

		this->GCServerMsgStringSend(Lang.GetText(0, 561), lpObj, 1);
		return;
	}

	item_num = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	if (MaxItemTypeRange(item_num) == FALSE)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	map_num = lpObj->MapNumber;

	if (MAX_MAP_RANGE(map_num) == FALSE)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	lpItem = &MapC[map_num].m_cItem[item_num];

	if (lpItem->IsItem() == TRUE && lpItem->Give == false && lpItem->live == true)
	{
		if (IT_MAP_RANGE(map_num) == TRUE)
		{
			if (lpItem->m_Type == ITEMGET(14, 223))
			{
				if (g_ConfigRead.server.GetServerType() == SERVER_NORMAL)
				{
					/*if (g_IT_Event.CheckRelics(lpObj) == TRUE)
					{
						pResult.result = -1;
						pResult.h.size -= sizeof(pResult.Data);
						GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

						return;
					}*/
				}
			}
		}

		if (lpItem->m_QuestItem != false)
		{
			bool bIsGetItem = g_QuestInfo.GetQuestItem(*lpObj, lpItem->m_Type, lpItem->m_Level);
			bool bIsQuestExpItem = g_QuestExpProgMng.IsQuestDropItem(*lpObj, lpItem->m_Type, lpItem->m_Level);

			if (bIsGetItem == false && bIsQuestExpItem == false)
			{
				pResult.result = -1;
				pResult.h.size -= sizeof(pResult.Data);
				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
				return;
			}
		}

		if (IsJumpingEventItem(lpItem->m_Type))
		{
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
			return;
		}

		LPITEM_ATTRIBUTE pItemAttribute = GetItemAttr(lpItem->m_Type);

		if (!pItemAttribute)
		{
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
			return;
		}

		if (pItemAttribute->ItemKindA == 12)
		{
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
			return;
		}

		if (lpItem->m_Type == ITEMGET(13, 20))	// Wizard Ring
		{
			switch (lpItem->m_Level)
			{
			case 0:
			{
				int iWRCount = gObjGetItemCountInIventory(*lpObj, lpItem->m_Type / MAX_SUBTYPE_ITEMS, lpItem->m_Type%MAX_SUBTYPE_ITEMS, lpItem->m_Level);

				if (iWRCount > 0)
				{
					pResult.result = -1;
					pResult.h.size -= sizeof(pResult.Data);

					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
					this->GCServerMsgStringSend(Lang.GetText(0, 105), lpObj, 1);

					return;
				}
				break;
			}
			case 1:

				pResult.result = -1;
				pResult.h.size -= sizeof(pResult.Data);

				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
				return;
				break;

			}
		}

		if (lpItem->m_Type == ITEMGET(13, 20) && lpItem->m_Level == 2)	// Wizard Ring
		{
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);

			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

			return;
		}

		if (lpItem->m_Type == ITEMGET(13, 38))	// MoonStonePendant
		{
			int count = gObjGetItemCountInIventory(*lpObj, lpItem->m_Type / MAX_SUBTYPE_ITEMS,
				lpItem->m_Type % MAX_SUBTYPE_ITEMS, lpItem->m_Level);

			if (count > 0)
			{
				pResult.result = -1;
				pResult.h.size -= sizeof(pResult.Data);

				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
				GCServerMsgStringSend(Lang.GetText(0, 285), lpObj, 1);

				return;
			}
		}

		if (lpItem->m_Type == ITEMGET(13, 39))	// ChangeRing
		{
			int count = gObjGetItemCountInIventory(*lpObj, lpItem->m_Type / MAX_SUBTYPE_ITEMS,
				lpItem->m_Type % MAX_SUBTYPE_ITEMS, lpItem->m_Level);

			if (count > 0)
			{
				pResult.result = -1;
				pResult.h.size -= sizeof(pResult.Data);

				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
				GCServerMsgStringSend(Lang.GetText(0, 287), lpObj, 1);

				return;
			}
		}

		type = lpItem->m_Type;
		level = lpItem->m_Level;
		special = lpItem->m_Special[0];
		NOption = lpItem->m_NewOption;
		strcpy(szItemName, lpItem->GetName());

		if (lpItem->m_Type == ITEMGET(14, 15)) // Zen
		{
			if (MapC[map_num].ItemGive(*lpObj, item_num, false) == TRUE)
			{
				if (!gObjCheckMaxZen(*lpObj, lpItem->m_BuyMoney))
				{
					if (lpObj->m_PlayerData->Money < MAX_ZEN)
					{
						lpObj->m_PlayerData->Money = MAX_ZEN;
						pResult.result = -2;
						WORD hiWord = SET_NUMBERHW(lpObj->m_PlayerData->Money);
						WORD loWord = SET_NUMBERLW(lpObj->m_PlayerData->Money);
						pResult.Data[0] = SET_NUMBERH(hiWord);
						pResult.Data[1] = SET_NUMBERL(hiWord);
						pResult.Data[2] = SET_NUMBERH(loWord);
						pResult.Data[3] = SET_NUMBERL(loWord);

						GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

						return;
					}

					pResult.result = -1;
					pResult.h.size -= sizeof(pResult.Data);

					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

					return;
				}

				lpObj->m_PlayerData->Money += lpItem->m_BuyMoney;
				pResult.result = -2;

				WORD hiWord = SET_NUMBERHW(lpObj->m_PlayerData->Money);
				WORD loWord = SET_NUMBERLW(lpObj->m_PlayerData->Money);
				pResult.Data[0] = SET_NUMBERH(hiWord);
				pResult.Data[1] = SET_NUMBERL(hiWord);
				pResult.Data[2] = SET_NUMBERH(loWord);
				pResult.Data[3] = SET_NUMBERL(loWord);
				pResult.h.size -= 3;
			}

			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

		}
		else
		{
			if (lpItem->m_Type == ITEMGET(13, 32))
			{
				int pos = gObjOverlapItemUsingDur(*lpItem, map_num, item_num, *lpObj, IsOverlapItem(lpItem->m_Type), ITEMGET(13, 32), 0);

				if (MAIN_INVENTORY_RANGE(pos) != FALSE)
				{
					pResult.result = -3;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
					lpObj->pntInventory[pos]->m_Durability += lpItem->m_Durability;
					GCItemDurSend(lpObj, pos, lpObj->pntInventory[pos]->m_Durability, 0);

					return;
				}
			}

			if (lpItem->m_Type == ITEMGET(13, 33))
			{
				int pos = gObjOverlapItemUsingDur(*lpItem, map_num, item_num, *lpObj, IsOverlapItem(lpItem->m_Type), ITEMGET(13, 33), 0);

				if (MAIN_INVENTORY_RANGE(pos) != FALSE)
				{
					pResult.result = -3;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
					lpObj->pntInventory[pos]->m_Durability += lpItem->m_Durability;
					GCItemDurSend(lpObj, pos, lpObj->pntInventory[pos]->m_Durability, 0);

					return;
				}
			}

			if (lpItem->m_Type == ITEMGET(13, 34))
			{
				int pos = gObjOverlapItemUsingDur(*lpItem, map_num, item_num, *lpObj, IsOverlapItem(lpItem->m_Type), ITEMGET(13, 34), 0);

				if (MAIN_INVENTORY_RANGE(pos) != FALSE)
				{
					pResult.result = -3;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
					lpObj->pntInventory[pos]->m_Durability += lpItem->m_Durability;
					GCItemDurSend(lpObj, pos, lpObj->pntInventory[pos]->m_Durability, 0);

					return;
				}
			}

			if (lpItem->m_Type == ITEMGET(14, 70) || lpItem->m_Type == ITEMGET(14, 71) || lpItem->m_Type == ITEMGET(14, 94)
				|| lpItem->m_Type == ITEMGET(14, 133)	//6.2
				)
			{
				int pos = gObjOverlapItemUsingDur(*lpItem, map_num, item_num, *lpObj, 50, lpItem->m_Type, 0);

				if (MAIN_INVENTORY_RANGE(pos) != FALSE)
				{
					pResult.result = -3;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
					lpObj->pntInventory[pos]->m_Durability += lpItem->m_Durability;
					GCItemDurSend(lpObj, pos, lpObj->pntInventory[pos]->m_Durability, 0);
					return;
				}
			}

			if (lpItem->m_Type == ITEMGET(13, 147)) // Trophy of Battle
			{
				int pos = gObjOverlapItemUsingDur(*lpItem, map_num, item_num, *lpObj, IsOverlapItem(lpItem->m_Type), ITEMGET(13, 147), 0);

				if (MAIN_INVENTORY_RANGE(pos) != FALSE)
				{
					pResult.result = -3;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
					lpObj->pntInventory[pos]->m_Durability += lpItem->m_Durability;
					GCItemDurSend(lpObj, pos, lpObj->pntInventory[pos]->m_Durability, 0);

					return;
				}
			}

			/*if (lpItem->m_Type == ITEMGET(14, 29)) // Symbol of Kundun
			{
				for (int n = 0; n < MAIN_INVENTORY_SIZE; n++)
				{
					int pos = ::g_KalimaGate.CheckOverlapKundunMark(*lpObj, lpItem->m_Level);

					if (MAIN_INVENTORY_RANGE(pos))
					{
						if (MapC[map_num].ItemGive(*lpObj, item_num, true) == TRUE)
						{
							BYTE NewOption[MAX_EXOPTION_SIZE];

							::ItemIsBufExOption(NewOption, lpItem);

							pResult.result = -3;

							GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

							lpObj->pntInventory[pos]->m_Durability += lpItem->m_Durability;

							if (lpObj->pntInventory[pos]->m_Durability >= IsOverlapItem(lpItem->m_Type))
							{
								int NewDur = lpObj->pntInventory[pos]->m_Durability - IsOverlapItem(lpItem->m_Type);
								::gObjInventoryItemSet(*lpObj, pos, -1);
								lpObj->pntInventory[pos]->Clear();
								this->GCInventoryItemDeleteSend(lpObj, pos, 1);
								::ItemCreate(lpObj, 235, lpObj->X, lpObj->Y, ItemGetNumberMake(14, 28),
									lpItem->m_Level, 0, 0, 0, 0, lpObj->m_Index, 0, 0, 0, 0, 0);

								if (NewDur > 0)
								{
									lpItem->m_State = 2;
									lpItem->Give = false;
									lpItem->live = true;
									lpItem->m_Durability = NewDur;

									continue;

								}
							}
							else
							{
								GCItemDurSend(lpObj, pos, lpObj->pntInventory[pos]->m_Durability, 0);

							}
							return;
						}
					}
					else
					{
						break;
					}
				}
			}*/

			if (lpItem->m_Type == ITEMGET(14, 110)) // Double Goer
			{
				int pos = gObjOverlapItemUsingDur(*lpItem, map_num, item_num, *lpObj, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 110), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
					lpObj->pntInventory[pos]->m_Durability += lpItem->m_Durability;

					if (lpObj->pntInventory[pos]->m_Durability >= IsOverlapItem(lpItem->m_Type))
					{
						int NewDur = lpObj->pntInventory[pos]->m_Durability - IsOverlapItem(lpItem->m_Type);
						::gObjInventoryItemSet(*lpObj, pos, -1);
						lpObj->pntInventory[pos]->Clear();
						this->GCInventoryItemDeleteSend(lpObj, pos, 1);
						::ItemCreate(lpObj, 235, lpObj->X, lpObj->Y, ItemGetNumberMake(14, 111),
							lpItem->m_Level, 0, 0, 0, 0, lpObj->m_Index, 0, 0, 0, 0, 0);

						if (NewDur > 0)
						{
							lpItem->m_State = 2;
							lpItem->Give = false;
							lpItem->live = true;
							lpItem->m_Durability = NewDur;

							//	continue;

						}
					}
					else
					{
						GCItemDurSend(lpObj, pos, lpObj->pntInventory[pos]->m_Durability, 0);

					}
					return;
				}
				else
				{

				}
			}
			if (lpItem->m_Type == ITEMGET(12, 144)) // Mithril
			{
				for (int j = 0; j < MAIN_INVENTORY_SIZE; j++)
				{
					BYTE btMainAtribute = lpItem->m_BonusSocketOption & 0x0F;
					int iInventoryPos = g_PentagramSystem.CheckOverlapMythrilPiece(*lpObj, lpItem->m_Type, btMainAtribute);

					if (!MAIN_INVENTORY_RANGE(iInventoryPos))
					{
						break;
					}

					int Dur = lpObj->pntInventory[iInventoryPos]->m_Durability + lpItem->m_Durability;

					if (Dur <= IsOverlapItem(lpItem->m_Type) && MapC[map_num].ItemGive(*lpObj, item_num, 1) == TRUE)
					{
						BYTE NewOption[MAX_EXOPTION_SIZE];

						::ItemIsBufExOption(NewOption, lpItem);

						pResult.result = -3;
						GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

						lpObj->pntInventory[iInventoryPos]->m_Durability += lpItem->m_Durability;

						if (lpObj->pntInventory[iInventoryPos]->m_Durability < IsOverlapItem(lpItem->m_Type))
						{
							this->GCItemDurSend(lpObj, iInventoryPos, lpObj->pntInventory[iInventoryPos]->m_Durability, 0);
							return;
						}

						int NewDur = lpObj->pntInventory[iInventoryPos]->m_Durability - IsOverlapItem(lpItem->m_Type);
						::gObjInventoryItemSet(*lpObj, iInventoryPos, -1);
						lpObj->pntInventory[iInventoryPos]->Clear();
						this->GCInventoryItemDeleteSend(lpObj, iInventoryPos, 1);

						if (lpItem->m_Type == ITEMGET(12, 144))
						{
							ItemCreate(lpObj, 233, lpObj->X, lpObj->Y, ItemGetNumberMake(12, 145), 0, 0, 0, 0, 0,
								lpObj->m_Index, 0, 0, 0, 0, lpItem->m_BonusSocketOption);

							sLog->outBasic("[Pentagram] [%s][%s] Make Mythril (Left:%d)", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, NewDur);
						}

						if (NewDur <= 0)
							return;

						lpItem->m_State = 2;
						lpItem->Give = 0;
						lpItem->live = 1;
						lpItem->m_Durability = NewDur;
					}
				}
			}

			//============
			if (lpItem->m_Type == ITEMGET(14, 290)) // Elemental Powder
			{
				int pos = gObjOverlapItemUsingDur(*lpItem, map_num, item_num, *lpObj, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 290), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
					lpObj->pntInventory[pos]->m_Durability += lpItem->m_Durability;

					if (lpObj->pntInventory[pos]->m_Durability >= IsOverlapItem(lpItem->m_Type))
					{
						int NewDur = lpObj->pntInventory[pos]->m_Durability - IsOverlapItem(lpItem->m_Type);
						::gObjInventoryItemSet(*lpObj, pos, -1);
						lpObj->pntInventory[pos]->Clear();
						this->GCInventoryItemDeleteSend(lpObj, pos, 1);
						::ItemCreate(lpObj, 235, lpObj->X, lpObj->Y, ItemGetNumberMake(14, 293), lpItem->m_Level, 0, 0, 0, 0, lpObj->m_Index, 0, 0, 0, 0, 0);

						if (NewDur > 0)
						{
							lpItem->m_State = 2;
							lpItem->Give = false;
							lpItem->live = true;
							lpItem->m_Durability = NewDur;

							//	continue;

						}
					}
					else
					{
						GCItemDurSend(lpObj, pos, lpObj->pntInventory[pos]->m_Durability, 0);

					}
					return;
				}
				else
				{

				}
			}

			if (lpItem->m_Type == ITEMGET(13, 145)) // Spirit Map
			{
				int pos = gObjOverlapItemUsingDur(*lpItem, map_num, item_num, *lpObj, IsOverlapItem(lpItem->m_Type), ITEMGET(13, 145), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
					lpObj->pntInventory[pos]->m_Durability += lpItem->m_Durability;

					if (lpObj->pntInventory[pos]->m_Durability >= IsOverlapItem(lpItem->m_Type))
					{
						int NewDur = lpObj->pntInventory[pos]->m_Durability - IsOverlapItem(lpItem->m_Type);
						::gObjInventoryItemSet(*lpObj, pos, -1);
						lpObj->pntInventory[pos]->Clear();
						this->GCInventoryItemDeleteSend(lpObj, pos, 1);
						::ItemCreate(lpObj, 235, lpObj->X, lpObj->Y, ItemGetNumberMake(13, 146),
							lpItem->m_Level, 0, 0, 0, 0, lpObj->m_Index, 0, 0, 0, 0, 0);

						if (NewDur > 0)
						{
							lpItem->m_State = 2;
							lpItem->Give = false;
							lpItem->live = true;
							lpItem->m_Durability = NewDur;

							//	continue;

						}
					}
					else
					{
						GCItemDurSend(lpObj, pos, lpObj->pntInventory[pos]->m_Durability, 0);

					}
					return;
				}
				else
				{

				}
			}
			if (lpItem->m_Type == ITEMGET(14, 101)) // Imperial Fort
			{
				int pos = gObjOverlapItemUsingDur(*lpItem, map_num, item_num, *lpObj, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 101), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
					lpObj->pntInventory[pos]->m_Durability += lpItem->m_Durability;

					if (lpObj->pntInventory[pos]->m_Durability >= IsOverlapItem(lpItem->m_Type))
					{
						int NewDur = lpObj->pntInventory[pos]->m_Durability - IsOverlapItem(lpItem->m_Type);
						::gObjInventoryItemSet(*lpObj, pos, -1);
						lpObj->pntInventory[pos]->Clear();
						this->GCInventoryItemDeleteSend(lpObj, pos, 1);
						::ItemCreate(lpObj, 235, lpObj->X, lpObj->Y, ItemGetNumberMake(14, 102),
							lpItem->m_Level, 0, 0, 0, 0, lpObj->m_Index, 0, 0, 0, 0, 0);

						if (NewDur > 0)
						{
							lpItem->m_State = 2;
							lpItem->Give = false;
							lpItem->live = true;
							lpItem->m_Durability = NewDur;

							//	continue;

						}
					}
					else
					{
						GCItemDurSend(lpObj, pos, lpObj->pntInventory[pos]->m_Durability, 0);

					}
					return;
				}
				else
				{

				}
			}

			if (lpItem->m_Type == ITEMGET(14, 100)) // Lucky Coin
			{
				int pos = gObjOverlapItemUsingDur(*lpItem, map_num, item_num, *lpObj, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 100), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
					lpObj->pntInventory[pos]->m_Durability += lpItem->m_Durability;

					GCItemDurSend(lpObj, pos, lpObj->pntInventory[pos]->m_Durability, 0);

					return;
				}
				else
				{

				}
			}

			/*if (lpItem->m_Type == ITEMGET(14, 21) && lpItem->m_Level == 3) // Sign of Lord
			{
				for (int n = 0; n < MAIN_INVENTORY_SIZE; n++)
				{
					int pos = ::g_CastleSiegeSync.CheckOverlapCsMarks(*lpObj);

					if (MAIN_INVENTORY_RANGE(pos))
					{
						int Dur = lpObj->pntInventory[pos]->m_Durability + lpItem->m_Durability;

						if (Dur <= IsOverlapItem(lpItem->m_Type))
						{
							if (MapC[map_num].ItemGive(*lpObj, item_num, true) == TRUE)
							{
								pResult.result = -3;
								GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
								lpObj->pntInventory[pos]->m_Durability += lpItem->m_Durability;
								GCItemDurSend(lpObj, pos, lpObj->pntInventory[pos]->m_Durability, 0);
								return;
							}
						}
						else
						{
							lpItem->m_Durability = Dur - IsOverlapItem(lpItem->m_Type);
							lpObj->pntInventory[pos]->m_Durability = IsOverlapItem(lpItem->m_Type);
							GCItemDurSend(lpObj, pos, lpObj->pntInventory[pos]->m_Durability, 0);
						}

					}
					else
					{
						break;
					}
				}
			}*/

			if (lpItem->m_Type == ITEMGET(14, 90)) // Golden cherry blossom
			{
				int pos = gObjOverlapItemUsingDur(*lpItem, map_num, item_num, *lpObj, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 90), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

					lpObj->pntInventory[pos]->m_Durability += lpItem->m_Durability;

					GCItemDurSend(lpObj, pos, lpObj->pntInventory[pos]->m_Durability, 0);

					return;
				}

				else
				{

				}
			}

			if (lpItem->m_Type == ITEMGET(14, 89)) // Red cherry blossom
			{
				int pos = gObjOverlapItemUsingDur(*lpItem, map_num, item_num, *lpObj, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 89), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

					lpObj->pntInventory[pos]->m_Durability += lpItem->m_Durability;

					GCItemDurSend(lpObj, pos, lpObj->pntInventory[pos]->m_Durability, 0);

					return;
				}

				else
				{

				}
			}

			if (lpItem->m_Type == ITEMGET(14, 88)) // White cherry blossom
			{
				int pos = gObjOverlapItemUsingDur(*lpItem, map_num, item_num, *lpObj, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 88), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

					lpObj->pntInventory[pos]->m_Durability += lpItem->m_Durability;

					GCItemDurSend(lpObj, pos, lpObj->pntInventory[pos]->m_Durability, 0);

					return;
				}

				else
				{

				}
			}

			if (lpItem->m_Type == ITEMGET(14, 153)) // White cherry blossom
			{
				int pos = gObjOverlapItemUsingDur(*lpItem, map_num, item_num, *lpObj, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 153), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

					lpObj->pntInventory[pos]->m_Durability += lpItem->m_Durability;

					GCItemDurSend(lpObj, pos, lpObj->pntInventory[pos]->m_Durability, 0);

					return;
				}

				else
				{

				}
			}

			if (lpItem->m_Type == ITEMGET(14, 154)) // White cherry blossom
			{
				int pos = gObjOverlapItemUsingDur(*lpItem, map_num, item_num, *lpObj, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 154), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

					lpObj->pntInventory[pos]->m_Durability += lpItem->m_Durability;

					GCItemDurSend(lpObj, pos, lpObj->pntInventory[pos]->m_Durability, 0);

					return;
				}

				else
				{

				}
			}

			if (lpItem->m_Type == ITEMGET(14, 155)) // White cherry blossom
			{
				int pos = gObjOverlapItemUsingDur(*lpItem, map_num, item_num, *lpObj, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 155), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

					lpObj->pntInventory[pos]->m_Durability += lpItem->m_Durability;

					GCItemDurSend(lpObj, pos, lpObj->pntInventory[pos]->m_Durability, 0);

					return;
				}

				else
				{

				}
			}

			if (lpItem->m_Type == ITEMGET(14, 156)) // White cherry blossom
			{
				int pos = gObjOverlapItemUsingDur(*lpItem, map_num, item_num, *lpObj, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 156), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

					lpObj->pntInventory[pos]->m_Durability += lpItem->m_Durability;

					GCItemDurSend(lpObj, pos, lpObj->pntInventory[pos]->m_Durability, 0);

					return;
				}

				else
				{

				}
			}

			if (g_LuckyItemManager.IsLuckyItemEquipment(lpItem->m_Type) && g_ConfigRead.data.common.AllowGetLuckyItemGround == false)
			{
				char szSetItemName[48] = { 0 };
				int tmpSetOption = 0;
				BYTE NewOption[8] = { 0 };

				ItemIsBufExOption(NewOption, lpItem);

				if (gSetItemOption.IsSetItem(lpItem->m_Type))
				{
					if (lpItem->m_SetOption & 1)
					{
						tmpSetOption = 1;
					}

					else
					{
						if (lpItem->m_SetOption & 2)
						{
							tmpSetOption = 2;
						}
					}

					strcpy(szSetItemName, gSetItemOption.GetSetOptionName(type, tmpSetOption));
				}
				pResult.result = -1;

				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
				this->GCServerMsgStringSend(Lang.GetText(0, 519), lpObj, 1);
				return;
			}

			if (lpItem->m_QuestItem == true && g_QuestExpManager.IsQuestItemAtt(lpItem->m_Type, 1) == true)
			{
				for (int n = 12; n < INVENTORY_SIZE; n++)
				{
					int pos = -1;

					if (lpObj->pntInventory[n]->m_Type == lpItem->m_Type &&
						lpObj->pntInventory[n]->m_Durability < IsOverlapItem(lpItem->m_Type))
					{
						pos = n;
					}

					if (INVENTORY_RANGE(pos) == true)
					{
						if (lpObj->pntInventory[pos]->m_Durability + lpItem->m_Durability > IsOverlapItem(lpItem->m_Type))
						{
							lpItem->m_Durability = lpObj->pntInventory[pos]->m_Durability + lpItem->m_Durability - IsOverlapItem(lpItem->m_Type);
							lpObj->pntInventory[pos]->m_Durability = IsOverlapItem(lpItem->m_Type);
							this->GCItemDurSend(lpObj, pos, lpObj->pntInventory[pos]->m_Durability, 0);
						}

						else
						{
							if (MapC[map_num].ItemGive(*lpObj, item_num, 1) == TRUE)
							{
								pResult.result = -3;
								GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

								lpObj->pntInventory[pos]->m_Durability += lpItem->m_Durability;
								GCItemDurSend(lpObj, pos, lpObj->pntInventory[pos]->m_Durability, 0);
								return;
							}
						}
					}
				}
			}

			pResult.result = ::gObjInventoryInsertItemTemp(*lpObj, *lpItem);

			if (pResult.result != 0xFF)
			{
				ItemByteConvert((LPBYTE)&pResult.Data[0], lpItem->m_Type, lpItem->m_Option1, lpItem->m_Option2,
					lpItem->m_Option3, lpItem->m_Level, lpItem->m_Durability, lpItem->m_NewOption,
					lpItem->m_SetOption, lpItem->m_JewelOfHarmonyOption, lpItem->m_ItemOptionEx, lpItem->m_SocketOption, lpItem->m_BonusSocketOption, 0);
				pResult.i = ID;
				pResult.ii = ID1;
				pResult.x = 0xFF;
				pResult.xx = 0x0C;
				if (MapC[map_num].ItemGive(*lpObj, item_num, false) == TRUE)
				{
					BYTE pos = ::gObjInventoryInsertItem(*lpObj, *lpItem);

					if (pos == 0xFF)
						pResult.result = -1;

					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

					if (pos == 0xFF)
					{
						::GCItemListSend(lpObj);
						g_PeriodItemEx.OnRequestPeriodItemList(*lpObj);
					}
					else
					{
						BYTE NewOption[MAX_EXOPTION_SIZE];

						::ItemIsBufExOption(NewOption, lpItem);

						this->GCSendGetItemInfoForParty(lpObj, lpItem);

						/*if (BC_MAP_RANGE(map_num))
						{
							int iBridgeIndex = g_BloodCastle.GetBridgeIndex(map_num);
							if (g_BloodCastle.CheckQuestItemSerial(iBridgeIndex, lpItem))
							{
								g_BloodCastle.CatchQuestItemByUser(iBridgeIndex, *lpObj, lpItem->m_Level);
							}
						}

						if (IT_MAP_RANGE(map_num) && lpItem->m_Type == ITEMGET(14, 223))
						{
							if (g_ConfigRead.server.GetServerType() == SERVER_NORMAL)
							{
								g_IT_Event.SetRelicsInventoryPos(lpObj->MapNumber, *lpObj, pos);
								g_IT_Event.SendRelicsUserInfo(lpObj);

							}
						}*/

						/*if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
						{
							if (lpObj->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
							{
								g_ChaosCastleFinal.SetUBFGetReward(*lpObj, lpItem->m_Type, lpItem->m_Number, pos);
							}

							else if (lpObj->MapNumber == MAP_INDEX_DEVILSQUARE_FINAL)
							{
								g_DevilSquareFinal.SetUBFGetReward(*lpObj, lpItem->m_Type, lpItem->m_Number, pos);
							}

							else if (CC_MAP_RANGE(lpObj->MapNumber) == TRUE)
							{
								g_ChaosCastle.SetUBFGetReward(*lpObj, lpItem->m_Type, lpItem->m_Number, pos);
							}
						}*/

						if (g_ArcaBattle.IsArcaBattleServer() == TRUE && lpItem->m_Type == ITEMGET(13, 147))
						{
							g_ArcaBattle.BootyItemGetCnt(*lpObj);
						}

						if (lpItem->m_PeriodItemDuration > 0)
						{
							g_PeriodItemEx.SetPeriodItemInfo(*lpObj, lpItem->m_Type, lpItem->m_Number, lpItem->m_PeriodItemDuration);

							this->GCInventoryItemOneSend(lpObj, pos);
							g_PeriodItemEx.SendPeriodItemInfoOnce(*lpObj, lpObj->pntInventory[pos]);
						}
					}
				}
				else
				{
					pResult.result = -1;
					pResult.h.size -= sizeof(pResult.Data);

					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
				}
			}
			else
			{
				pResult.result = -1;
				pResult.h.size -= sizeof(pResult.Data);

				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
			}
		}
	}
	else
	{
		pResult.h.size -= sizeof(pResult.Data);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);
	}
}

bool GameProtocol::CGItemDropRequest(PMSG_ITEMTHROW * lpMsg, CGameObject* lpObj, BOOL drop_type)
{
	BOOL CopyItem = FALSE;
	BYTE NewOption[MAX_EXOPTION_SIZE];

	if (!PacketCheckTime(lpObj))
		return FALSE;

	if (lpObj->Type != OBJ_USER)
	{
		return FALSE;
	}

	PMSG_ITEMTHROW_RESULT pResult;

	PHeadSetB((LPBYTE)&pResult, 0x23, sizeof(pResult));
	pResult.Result = true;
	pResult.Ipos = lpMsg->Ipos;

	if (lpObj->m_IfState.use != 0)
	{
		if (lpObj->m_IfState.type != 3)
		{
			pResult.Result = false;
			return FALSE;
		}
	}

	if (lpObj->m_PlayerData->m_bSecurityCheck == false)
	{
		MsgOutput(*lpObj, Lang.GetText(0, 822));
		pResult.Result = false;
	}

	if (lpObj->m_PlayerData->ConnectUser->CloseType != -1)
		pResult.Result = false;

	if (lpObj->m_PlayerData->m_bMapSvrMoveQuit == true)
	{
		pResult.Result = false;
	}

	if (!::gObjFixInventoryPointer(*lpObj))

		if (lpObj->pTransaction == 1)
		{
			pResult.Result = false;

			return false;
		}


	if (lpMsg->Ipos == 0xFF)	// Null Position
	{
		pResult.Result = false;
	}

	if (lpMsg->Ipos < 0 || lpMsg->Ipos > MAIN_INVENTORY_SIZE - 1)	// Out of Bounds
		pResult.Result = false;

	if (!gObjCanItemTouch(*lpObj, 0))
		pResult.Result = false;

	if (!IsDumpItem(lpObj->pntInventory[lpMsg->Ipos]->m_Type) && (lpObj->pntInventory[lpMsg->Ipos]->m_Type != ITEMGET(13, 20) || !lpObj->pntInventory[lpMsg->Ipos]->m_Level))
		pResult.Result = false;

	if (g_LuckyItemManager.IsLuckyItemEquipment(lpObj->pntInventory[lpMsg->Ipos]->m_Type) || g_LuckyItemManager.IsLuckyItemTicket(lpObj->pntInventory[lpMsg->Ipos]->m_Type))
	{
		pResult.Result = false;
	}

	// Icarus Preventions about Dinorant
	if (lpObj->MapNumber == MAP_INDEX_ICARUS)
	{
		if (lpMsg->Ipos == lpObj->m_btInvenPetPos)
		{
			if (lpObj->pntInventory[lpMsg->Ipos]->m_Type == ITEMGET(13, 3)) // Dinorant
			{
				if (lpObj->pntInventory[7]->IsItem() == FALSE)
				{
					pResult.Result = false;
				}
			}
		}
		else if (lpMsg->Ipos == 7)
		{
			if (lpObj->pntInventory[lpObj->m_btInvenPetPos]->m_Type != ITEMGET(13, 3)) // Dinorant 
			{
				pResult.Result = false;
			}
		}
	}

	if (lpObj->pntInventory[lpMsg->Ipos]->m_Type == ITEMGET(12, 30) ||
		lpObj->pntInventory[lpMsg->Ipos]->m_Type == ITEMGET(12, 31) ||
		lpObj->pntInventory[lpMsg->Ipos]->m_Type == ITEMGET(13, 36) ||
		lpObj->pntInventory[lpMsg->Ipos]->m_Type == ITEMGET(13, 37))
	{
		pResult.Result = false;
	}

	if (GetWingType(lpObj->pntInventory[lpMsg->Ipos]->m_Type, WCF_ITEMNUMBER) != NONE_WING && g_ConfigRead.pk.bPkPenaltyDisable == FALSE)
	{
		pResult.Result = false;
	}
	if (g_ConfigRead.data.common.joinmuRemoveItemSellDropRestriction == false)
	{
		if ((lpObj->pntInventory[lpMsg->Ipos]->m_Type >= ITEMGET(13, 0) && lpObj->pntInventory[lpMsg->Ipos]->m_Type <= ITEMGET(13, 3))
			|| lpObj->pntInventory[lpMsg->Ipos]->m_Type == ITEMGET(14, 13) ||
			lpObj->pntInventory[lpMsg->Ipos]->m_Type == ITEMGET(14, 14) ||
			lpObj->pntInventory[lpMsg->Ipos]->m_Type == ITEMGET(14, 16) ||
			(lpObj->pntInventory[lpMsg->Ipos]->m_Type >= ITEMGET(12, 0)
				&& lpObj->pntInventory[lpMsg->Ipos]->m_Type <= ITEMGET(12, 6)) ||
				(lpObj->pntInventory[lpMsg->Ipos]->m_Type >= ITEMGET(12, 36)
					&& lpObj->pntInventory[lpMsg->Ipos]->m_Type <= ITEMGET(12, 40))
			|| lpObj->pntInventory[lpMsg->Ipos]->m_Type == ITEMGET(12, 15)
			|| (lpObj->pntInventory[lpMsg->Ipos]->m_Level > g_ConfigRead.pk.iPkMaxLevelItemDrop
				&& lpObj->pntInventory[lpMsg->Ipos]->m_Type < ITEMGET(12, 0))
			|| (lpObj->pntInventory[lpMsg->Ipos]->IsSetItem() != FALSE && !g_ConfigRead.pk.bPkDropExpensiveItems)
			|| (lpObj->pntInventory[lpMsg->Ipos]->IsExtItem() != FALSE && !g_ConfigRead.pk.bPkDropExpensiveItems))
		{
			if (g_ConfigRead.pk.bPkPenaltyDisable == FALSE)
			{
				pResult.Result = false;
			}
		}
	}
	if (g_kJewelOfHarmonySystem.IsStrengthenByJewelOfHarmony(lpObj->pntInventory[lpMsg->Ipos]) == TRUE)
	{
		pResult.Result = false;
		this->GCServerMsgStringSend(Lang.GetText(0, 272), lpObj, 1);
	}

	if (g_kJewelOfHarmonySystem.IsJewelOfHarmonyOriginal(lpObj->pntInventory[lpMsg->Ipos]->m_Type) ||
		g_kJewelOfHarmonySystem.IsJewelOfHarmonyPurity(lpObj->pntInventory[lpMsg->Ipos]->m_Type) ||
		g_kJewelOfHarmonySystem.IsJewelOfHarmonySmeltingItems(lpObj->pntInventory[lpMsg->Ipos]->m_Type))
	{
		pResult.Result = false;
	}

	if (lpObj->pntInventory[lpMsg->Ipos]->m_Type == ITEMGET(13, 39))
	{
		pResult.Result = false;
	}

	if (lpObj->pntInventory[lpMsg->Ipos]->IsPeriodItem() == TRUE)
	{
		pResult.Result = false;
	}

	if (g_NewPVP.IsDuel(*lpObj))
	{
		pResult.Result = false;
	}

	if (IsJumpingEventItem(lpObj->pntInventory[lpMsg->Ipos]->m_Type) == TRUE)
	{
		pResult.Result = false;
	}

	if (pResult.Result == 1)
	{
		::ItemIsBufExOption(NewOption, lpObj->pntInventory[lpMsg->Ipos]);

		int map = lpObj->MapNumber;
		int type = lpObj->pntInventory[lpMsg->Ipos]->m_Type;
		int level = lpObj->pntInventory[lpMsg->Ipos]->m_Level;
		float dur = lpObj->pntInventory[lpMsg->Ipos]->m_Durability;
		BOOL bIsItem = lpObj->pntInventory[lpMsg->Ipos]->IsItem();
		BYTE Option1 = lpObj->pntInventory[lpMsg->Ipos]->m_Option1;
		BYTE Option2 = lpObj->pntInventory[lpMsg->Ipos]->m_Option2;
		BYTE Option3 = lpObj->pntInventory[lpMsg->Ipos]->m_Option3;
		BYTE NOption = lpObj->pntInventory[lpMsg->Ipos]->m_NewOption;
		UINT64 serial = lpObj->pntInventory[lpMsg->Ipos]->m_Number;
		int PetLevel = lpObj->pntInventory[lpMsg->Ipos]->m_PetItem_Level;
		UINT64 PetExp = lpObj->pntInventory[lpMsg->Ipos]->m_PetItem_Exp;
		BYTE SOption = lpObj->pntInventory[lpMsg->Ipos]->m_SetOption;
		BYTE btItemEffectEx = lpObj->pntInventory[lpMsg->Ipos]->m_ItemOptionEx;
		UINT64 serial2 = lpObj->pntInventory[lpMsg->Ipos]->m_Number;
		char szItemName[50] = "Item";
		BYTE JOHOption = g_kJewelOfHarmonySystem.GetItemStrengthenOption(lpObj->pntInventory[lpMsg->Ipos]);
		BYTE JOHOptionLevel = g_kJewelOfHarmonySystem.GetItemOptionLevel(lpObj->pntInventory[lpMsg->Ipos]);

		//Season 4 add-on
		BYTE SocketOption[5];
		BYTE SocketIndex;
		g_SocketOptionSystem.GetSocketOption(lpObj->pntInventory[lpMsg->Ipos], SocketOption, SocketIndex);

		for (int iOption = 0; iOption < 5; iOption++)
		{
			SocketOption[iOption] = lpObj->pntInventory[lpMsg->Ipos]->m_SocketOption[iOption];
		}

		if (gObjCheckSerial0ItemList(lpObj->pntInventory[lpMsg->Ipos]) != FALSE)
		{
			MsgOutput(*lpObj, Lang.GetText(0, 259));

			CopyItem = TRUE;
			pResult.Result = false;
			bIsItem = FALSE;
		}

		if (!::gObjInventorySearchSerialNumber(*lpObj, serial2))	// ANTI DUPE SYSTEM
		{
			CopyItem = TRUE;
			pResult.Result = false;
			bIsItem = FALSE;
		}

		if (bIsItem == TRUE)
		{
			strcpy(szItemName, lpObj->pntInventory[lpMsg->Ipos]->GetName());

			if (g_BagManager.SearchAndUseBag(*lpObj, BAG_COMMON, type, level) == true)
			{
				gObjInventoryDeleteItem(*lpObj, lpMsg->Ipos);
			}
			else if (type == ITEMGET(14, 51))
			{
				gObjInventoryDeleteItem(*lpObj, lpMsg->Ipos);
				FireworksOpenEven(*lpObj);
			}
			else if (type == ITEMGET(14, 63))
			{
				gObjInventoryDeleteItem(*lpObj, lpMsg->Ipos);
				FireworksOpenEven(*lpObj);

			}
			else if (type == ITEMGET(14, 210))
			{
				if (NewYearSummonMonster(*lpObj, 647) == FALSE)
				{
					pResult.Result = false;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

					return FALSE;
				}

				gObjInventoryDeleteItem(*lpObj, lpMsg->Ipos);
			}

			else if (type == ITEMGET(14, 211))
			{
				if (NewYearSummonMonster(*lpObj, 648) == FALSE)
				{
					pResult.Result = false;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

					return FALSE;
				}

				gObjInventoryDeleteItem(*lpObj, lpMsg->Ipos);
			}

			else if (type == ITEMGET(14, 212))
			{
				if (NewYearSummonMonster(*lpObj, 649) == FALSE)
				{
					pResult.Result = false;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

					return FALSE;
				}

				gObjInventoryDeleteItem(*lpObj, lpMsg->Ipos);
			}

			else if (type == ITEMGET(14, 213))
			{
				if (NewYearSummonMonster(*lpObj, 650) == FALSE)
				{
					pResult.Result = false;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

					return FALSE;
				}

				gObjInventoryDeleteItem(*lpObj, lpMsg->Ipos);
			}

			else if (type == ITEMGET(14, 217))
			{
				if (SummonGoldColossusMonster(*lpObj, 653, 1, 0) == FALSE)
				{
					pResult.Result = false;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

					return FALSE;
				}

				gObjInventoryDeleteItem(*lpObj, lpMsg->Ipos);
			}

			else if (type == ITEMGET(14, 218))
			{
				if (SummonGoldColossusMonster(*lpObj, 654, 1, 0) == FALSE)
				{
					pResult.Result = false;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

					return FALSE;
				}

				gObjInventoryDeleteItem(*lpObj, lpMsg->Ipos);
			}

			else if (type == ITEMGET(14, 219))
			{
				if (SummonGoldColossusMonster(*lpObj, 655, 1, 0) == FALSE)
				{
					pResult.Result = false;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

					return FALSE;
				}

				gObjInventoryDeleteItem(*lpObj, lpMsg->Ipos);
			}

			else if (type == ITEMGET(14, 220))
			{
				if (SummonGoldColossusMonster(*lpObj, 656, 1, 0) == FALSE)
				{
					pResult.Result = false;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

					return FALSE;
				}

				gObjInventoryDeleteItem(*lpObj, lpMsg->Ipos);
			}

			else if (type == ITEMGET(14, 221))
			{
				if (SummonGoldColossusMonster(*lpObj, 657, 1, 0) == FALSE)
				{
					pResult.Result = false;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

					return FALSE;
				}

				SummonGoldColossusMonster(*lpObj, 654, 5, 1);
				gObjInventoryDeleteItem(*lpObj, lpMsg->Ipos);
			}

			else if (type == ITEMGET(14, 234))
			{
				if (SummonPetEventMonster(*lpObj) == FALSE)
				{
					pResult.Result = false;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

					return FALSE;
				}

				gObjInventoryDeleteItem(*lpObj, lpMsg->Ipos);
			}
			else if (type == ITEMGET(14, 286))
			{
				gObjInventoryDeleteItem(*lpObj, lpMsg->Ipos);
				pResult.Result = true;

				BYTE SocketBonus = (lpObj->m_iPentagramMainAttribute == ELEMENT_NONE) ? (1 + rand() % 5) : lpObj->m_iPentagramMainAttribute;
				BYTE SocketOption[5];
				BYTE btSocketOption[5];

				SocketOption[0] = -1;
				SocketOption[1] = -1;
				SocketOption[2] = -1;
				SocketOption[3] = -1;
				SocketOption[4] = -1;

				int scount = 4;
				int count = 0;

				do
				{
					int slot = rand() % 4;

					if (SocketOption[slot] == (BYTE)-1)
					{
						SocketOption[slot] = 0xFE;
						count++;
					}
				} while (scount != count);

				for (int k = 0; k < 5; k++)
				{
					if (k)
					{
						btSocketOption[k] = -1;
					}

					else
					{
						btSocketOption[k] = 1;
					}
				}

				ItemCreate(lpObj, lpObj->MapNumber, lpObj->X, lpObj->Y, ITEMGET(12, 215), 0, 0, 0, 0, 0, lpObj->m_Index, 0, 0, 0, SocketOption, SocketBonus);
				ItemCreate(lpObj, lpObj->MapNumber, lpObj->X, lpObj->Y, ITEMGET(12, 222), 0, 0, 0, 0, 0, lpObj->m_Index, 0, 0, 0, btSocketOption, SocketBonus);
				ItemCreate(lpObj, lpObj->MapNumber, lpObj->X, lpObj->Y, ITEMGET(12, 232), 0, 0, 0, 0, 0, lpObj->m_Index, 0, 0, 0, btSocketOption, SocketBonus);
				ItemCreate(lpObj, lpObj->MapNumber, lpObj->X, lpObj->Y, ITEMGET(12, 242), 0, 0, 0, 0, 0, lpObj->m_Index, 0, 0, 0, btSocketOption, SocketBonus);
				ItemCreate(lpObj, lpObj->MapNumber, lpObj->X, lpObj->Y, ITEMGET(12, 252), 0, 0, 0, 0, 0, lpObj->m_Index, 0, 0, 0, btSocketOption, SocketBonus);
			}

			else if (type == ITEMGET(12, 26))
			{
				switch (level)
				{
				case 0:
				{
					gObjInventoryDeleteItem(*lpObj, lpMsg->Ipos);
					pResult.Result = true;
					int ltype = ITEMGET(12, 26);
					int randor = rand() % 100;
					int Level;

					if (randor < 10)
					{
						Level = 3;
					}
					else if (randor < 45)
					{
						Level = 1;
					}
					else
					{
						Level = 2;
					}

					ItemCreate(lpObj, lpObj->MapNumber, lpObj->X, lpObj->Y,
						ltype, Level, 1, 0, 0, 0, lpObj->m_Index, 0, 0, 0, 0, 0);

				}
				break;
				case 1: case 2: case 3:
				{
					gObjInventoryDeleteItem(*lpObj, lpMsg->Ipos);
					pResult.Result = true;

					PMSG_SERVERCMD ServerCmd;

					PHeadSubSetB((LPBYTE)&ServerCmd, 0xF3, 0x40, sizeof(ServerCmd));
					ServerCmd.CmdType = 0;
					ServerCmd.X = lpObj->X;
					ServerCmd.Y = lpObj->Y;

					MsgSendV2(lpObj, (LPBYTE)&ServerCmd, sizeof(ServerCmd));
					GIOCP.DataSend(lpObj->m_Index, (LPBYTE)&ServerCmd, sizeof(ServerCmd));

				}
				break;
				case 4:

					break;
				case 5:
					gObjInventoryDeleteItem(*lpObj, lpMsg->Ipos);
					pResult.Result = true;

					if ((rand() % 10000) < g_iHiddenTreasureBoxOfflineRate)
					{
						EGReqRegHTOfflineGift(*lpObj);
					}
					else
					{
						int money = 1000000;
						MapC[lpObj->MapNumber].MoneyItemDrop(money, lpObj->X, lpObj->Y);

					}
					break;
				}
			}
			else if (type == ITEMGET(13, 20) && level == 1) // Wizard Ring A
			{
				if (lpObj->Level >= 40)
				{
					::gObjInventoryDeleteItem(*lpObj, lpMsg->Ipos);
					g_BagManager.SearchAndUseBag(*lpObj, BAG_EVENT, EVENTBAG_WARRIORRING_1, lpObj->m_Index);
				}
				else
				{
					PMSG_NOTICE pNotice;

					TNotice::MakeNoticeMsg((TNotice *)&pNotice, 10, Lang.GetText(0, 106));
					TNotice::SetNoticeProperty((TNotice *)&pNotice, 10, _ARGB(255, 128, 149, 196), 1, 0, 20);
					TNotice::SendNoticeToUser(lpObj->m_PlayerData->ConnectUser->Index, (TNotice *)&pNotice);
					pResult.Result = false;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

					return FALSE;
				}
			}
			else if (type == ITEMGET(13, 20) && level == 2) // Wizard Ring B
			{
				if (lpObj->Level >= 80)
				{
					::gObjInventoryDeleteItem(*lpObj, lpMsg->Ipos);
					g_BagManager.SearchAndUseBag(*lpObj, BAG_EVENT, EVENTBAG_WARRIORRING_2, lpObj->m_Index);
				}
				else
				{
					PMSG_NOTICE pNotice;

					TNotice::MakeNoticeMsg((TNotice *)&pNotice, 10, Lang.GetText(0, 109));
					TNotice::SetNoticeProperty((TNotice *)&pNotice, 10, _ARGB(255, 128, 149, 196), 1, 0, 20);
					TNotice::SendNoticeToUser(lpObj->m_PlayerData->ConnectUser->Index, (TNotice *)&pNotice);
					pResult.Result = false;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

					return FALSE;
				}
			}
			/*else if (type == ITEMGET(14, 28)) // Lost Map
			{
				if (g_KalimaGate.CreateKalimaGate(*lpObj, level, lpMsg->px, lpMsg->py))
				{
					gObjInventoryDeleteItem(*lpObj, lpMsg->Ipos);
				}
				else
				{
					pResult.Result = false;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

					return FALSE;
				}
			}*/
			else if (type == ITEMGET(14, 64) && drop_type != 1)
			{
				pResult.Result = false;
				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
				return FALSE;
			}
			else if (type == ITEMGET(14, 20) && (((level - 1 < 0) ? FALSE : (level - 1 > 4) ? FALSE : TRUE) != FALSE)) // Remedy Of Love
			{
				gObjInventoryDeleteItem(*lpObj, lpMsg->Ipos);
			}
			else if (type == ITEMGET(13, 7)) // Contract (Summon)
			{
				int iMonsterID = 0;

				if (level == 0)
					iMonsterID = 286;
				else if (level == 1)
					iMonsterID = 287;

				if (g_CsNPC_Mercenary.CreateMercenary(*lpObj, iMonsterID, lpMsg->px, lpMsg->py))
				{
					::gObjInventoryDeleteItem(*lpObj, lpMsg->Ipos);
				}
				else
				{
					pResult.Result = false;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

					return FALSE;
				}
			}
			else if (type == ITEMGET(13, 11)) // Guardian / LifeStone
			{
				BOOL bResult = FALSE;

				if (level == 0)
					bResult = g_CsNPC_Guardian.CreateGuardian(*lpObj);

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && level == 1)
					bResult = g_CsNPC_LifeStone.CreateLifeStone(*lpObj);

				if (bResult == TRUE)
					::gObjInventoryDeleteItem(*lpObj, lpMsg->Ipos);
				else
				{
					pResult.Result = false;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

					return FALSE;
				}
			}

			else if ((type >= ITEMGET(14, 236) && type <= ITEMGET(14, 241)) && level == 0)
			{
				gObjInventoryDeleteItem(*lpObj, lpMsg->Ipos);

				switch (type)
				{
				case ITEMGET(14, 236):
					g_BagManager.SearchAndUseBag(*lpObj, BAG_EVENT, EVENTBAG_ITR_1, lpObj->m_Index);
					break;
				case ITEMGET(14, 237):
					g_BagManager.SearchAndUseBag(*lpObj, BAG_EVENT, EVENTBAG_ITR_2, lpObj->m_Index);
					break;
				case ITEMGET(14, 238):
					g_BagManager.SearchAndUseBag(*lpObj, BAG_EVENT, EVENTBAG_ITR_3, lpObj->m_Index);
					break;
				case ITEMGET(14, 239):
					g_BagManager.SearchAndUseBag(*lpObj, BAG_EVENT, EVENTBAG_ITR_4, lpObj->m_Index);
					break;
				case ITEMGET(14, 240):
					g_BagManager.SearchAndUseBag(*lpObj, BAG_EVENT, EVENTBAG_ITR_5, lpObj->m_Index);
					break;
				case ITEMGET(14, 241):
					g_BagManager.SearchAndUseBag(*lpObj, BAG_EVENT, EVENTBAG_ITR_6, lpObj->m_Index);
					break;
				}

			}

			else
			{

				int OwnerIndex = lpObj->m_PlayerData->ConnectUser->Index;

				if ((type >= ITEMGET(14, 0) && type <= ITEMGET(14, 10)) || (type == ITEMGET(4, 7)) || type == ITEMGET(4, 15) || type == ITEMGET(14, 64))
					OwnerIndex = -1;

				if (MapC[map].ItemDrop(type, level, dur, lpMsg->px, lpMsg->py, Option1, Option2, Option3,
					NOption, SOption, serial2, OwnerIndex, PetLevel, PetExp, btItemEffectEx, SocketOption, SocketIndex, 0) == TRUE)
				{
					gObjInventoryDeleteItem(*lpObj, lpMsg->Ipos);
					pResult.Result = true;

					if (drop_type == FALSE)
					{

					}
					else
					{

					}
				}
				else
				{
					pResult.Result = false;
				}
			}
		}
		else	// bIsItem
		{
			pResult.Result = false;
		}
	}

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

	if (CopyItem != FALSE)
	{
		::GCItemListSend(lpObj);
		g_PeriodItemEx.OnRequestPeriodItemList(*lpObj);
	}

	if (pResult.Result == 1)
	{
		if (lpMsg->Ipos < 12)
		{
			if (lpMsg->Ipos == 10 || lpMsg->Ipos == 11)
			{
				gObjUseSkill.SkillChangeUse(*lpObj);
			}

			::gObjMakePreviewCharSet(*lpObj);

			PMSG_USEREQUIPMENTCHANGED pChange;

			PHeadSetB((LPBYTE)&pChange, 0x25, sizeof(pChange));
			pChange.NumberH = SET_NUMBERH(lpObj);
			pChange.NumberL = SET_NUMBERL(lpObj);
			ItemByteConvert(pChange.ItemInfo, *lpObj->pntInventory[lpMsg->Ipos]);
			pChange.ItemInfo[I_OPTION] = lpMsg->Ipos << 4;
			pChange.ItemInfo[I_OPTION] |= LevelSmallConvert(*lpObj, lpMsg->Ipos) & 0x0F;
			pChange.Element = lpObj->m_iPentagramMainAttribute;
			if (pChange.ItemInfo[5] == 0xF0)
				pChange.ItemInfo[5] = 0xF1;
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pChange, pChange.h.size);
			MsgSendV2(lpObj, (LPBYTE)&pChange, pChange.h.size);
		}
	}

	return pResult.Result;
}






struct PMSG_INVENTORYITEMMOVE_RESULT
{
	PBMSG_HEAD h;	// C3:24
	BYTE result;	// 3
	BYTE Pos;	// 4
	BYTE ItemInfo[MAX_ITEM_INFO];	// 5
};


void GameProtocol::GCItemMoveResultSend(CGameObject* lpObj, BYTE result, BYTE pos, LPBYTE const ItemInfo)
{
	PMSG_INVENTORYITEMMOVE_RESULT pMsg;

	pMsg.h.c = 0xC3;
	pMsg.h.headcode = 0x24;
	pMsg.h.size = sizeof(pMsg);
	pMsg.result = result;
	pMsg.Pos = pos;
	pMsg.ItemInfo[I_TYPE] = ItemInfo[I_TYPE];
	pMsg.ItemInfo[I_OPTION] = ItemInfo[I_OPTION];
	pMsg.ItemInfo[I_DUR] = ItemInfo[I_DUR];
	pMsg.ItemInfo[I_NOPTION] = ItemInfo[I_NOPTION];
	pMsg.ItemInfo[I_SOPTION] = ItemInfo[I_SOPTION];
	pMsg.ItemInfo[I_380OPTION] = ItemInfo[I_380OPTION];
	pMsg.ItemInfo[I_JOHOPTION] = ItemInfo[I_JOHOPTION];
	pMsg.ItemInfo[I_SOCKET1] = ItemInfo[I_SOCKET1];
	pMsg.ItemInfo[I_SOCKET2] = ItemInfo[I_SOCKET2];
	pMsg.ItemInfo[I_SOCKET3] = ItemInfo[I_SOCKET3];
	pMsg.ItemInfo[I_SOCKET4] = ItemInfo[I_SOCKET4];
	pMsg.ItemInfo[I_SOCKET5] = ItemInfo[I_SOCKET5];

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
}




void GameProtocol::CGInventoryItemMove(PMSG_INVENTORYITEMMOVE * lpMsg, CGameObject* lpObj)
{
	BYTE result;
	BYTE ItemInfo[MAX_ITEM_INFO];

	if (!PacketCheckTime(lpObj))
	{
		this->GCItemMoveResultSend(lpObj, -1, 0, ItemInfo);
		return;
	}

	if (!::gObjIsConnectedGP(*lpObj))
	{
		this->GCItemMoveResultSend(lpObj, -1, 0, ItemInfo);
		return;
	}

	if (lpObj->m_PlayerData->m_bSecurityCheck == false)
	{
		MsgOutput(*lpObj, Lang.GetText(0, 822));
		this->GCItemMoveResultSend(lpObj, -1, 0, ItemInfo);
		return;
	}

	int it_type = 0;

	if (lpObj->m_PlayerData->m_ReqWarehouseOpen != false)
	{
		this->GCItemMoveResultSend(lpObj, -1, 0, (LPBYTE)&ItemInfo);

		return;
	}

	int source = lpMsg->source_item_num;
	int target = lpMsg->target_item_num;

	if (lpMsg->tFlag == 2 || lpMsg->sFlag == 2)
		it_type = 6;

	if (lpMsg->tFlag == 0 && lpMsg->sFlag == 0)
		it_type = 8;

	if ((lpMsg->tFlag == 0 && lpMsg->sFlag == 4) ||
		(lpMsg->tFlag == 4 && lpMsg->sFlag == 0) ||
		(lpMsg->tFlag == 4 && lpMsg->sFlag == 4))
		it_type = 8;

	if (gObjCanItemTouch(*lpObj, it_type) == FALSE)
	{
		if (lpMsg->sItemInfo[I_TYPE] == ITEMGET(4, 7) || lpMsg->sItemInfo[I_TYPE] == ITEMGET(4, 15))
		{
			if (target >= 12 || source < 12)
			{
				this->GCItemMoveResultSend(lpObj, -1, 0, (LPBYTE)&ItemInfo);
				return;
			}
		}
		else
		{
			this->GCItemMoveResultSend(lpObj, -1, 0, (LPBYTE)&ItemInfo);
			return;
		}
	}

	if (lpObj->DieRegen != FALSE)
	{
		this->GCItemMoveResultSend(lpObj, -1, 0, (LPBYTE)&ItemInfo);
		return;
	}

	if (lpObj->Teleport != FALSE)
	{
		this->GCItemMoveResultSend(lpObj, -1, 0, (LPBYTE)&ItemInfo);
		return;
	}

	ItemInfo[I_TYPE] = lpMsg->sItemInfo[I_TYPE];
	ItemInfo[I_OPTION] = lpMsg->sItemInfo[I_OPTION];
	ItemInfo[I_DUR] = lpMsg->sItemInfo[I_DUR];
	ItemInfo[I_NOPTION] = lpMsg->sItemInfo[I_NOPTION];
	ItemInfo[I_SOPTION] = lpMsg->sItemInfo[I_SOPTION];
	ItemInfo[I_380OPTION] = lpMsg->sItemInfo[I_380OPTION];
	ItemInfo[I_JOHOPTION] = lpMsg->sItemInfo[I_JOHOPTION];
	ItemInfo[I_SOCKET1] = lpMsg->sItemInfo[I_SOCKET1];
	ItemInfo[I_SOCKET2] = lpMsg->sItemInfo[I_SOCKET2];
	ItemInfo[I_SOCKET3] = lpMsg->sItemInfo[I_SOCKET3];
	ItemInfo[I_SOCKET4] = lpMsg->sItemInfo[I_SOCKET4];
	ItemInfo[I_SOCKET5] = lpMsg->sItemInfo[I_SOCKET5];

	if (lpMsg->sFlag == 3 || lpMsg->tFlag == 3)
	{
		if (lpObj->m_IfState.use < 1 || lpObj->m_IfState.type != 7)
		{
			this->GCItemMoveResultSend(lpObj, -1, 0, (LPBYTE)&ItemInfo);
			return;
		}

		if (lpObj->ChaosLock == TRUE)
		{
			this->GCItemMoveResultSend(lpObj, -1, 0, (LPBYTE)&ItemInfo);

			return;
		}
	}

	if (lpMsg->sFlag == 5 || lpMsg->tFlag == 5)
	{
		if (lpObj->m_IfState.use < 1 || lpObj->m_IfState.type != 13)
		{
			this->GCItemMoveResultSend(lpObj, -1, 0, (LPBYTE)&ItemInfo);
			return;
		}

		if (lpObj->ChaosLock == TRUE)
		{
			this->GCItemMoveResultSend(lpObj, -1, 0, (LPBYTE)&ItemInfo);

			return;
		}
	}

	if (lpMsg->sFlag == 6 || lpMsg->sFlag == 7 || lpMsg->sFlag == 8 ||
		lpMsg->tFlag == 6 || lpMsg->tFlag == 7 || lpMsg->tFlag == 8)
	{
		if (lpObj->m_IfState.use < 1 || lpObj->m_IfState.type != 7)
		{
			this->GCItemMoveResultSend(lpObj, -1, 0, (LPBYTE)&ItemInfo);
			return;
		}

		if (lpObj->ChaosLock == TRUE)
		{
			this->GCItemMoveResultSend(lpObj, -1, 0, (LPBYTE)&ItemInfo);

			return;
		}
	}
	if (lpMsg->sFlag == 9 || lpMsg->tFlag == 9)
	{
		if (lpObj->m_IfState.use < 1 || lpObj->m_IfState.type != 7)
		{
			this->GCItemMoveResultSend(lpObj, -1, 0, (LPBYTE)&ItemInfo);
			return;
		}

		if (lpObj->ChaosLock == TRUE)
		{
			this->GCItemMoveResultSend(lpObj, -1, 0, (LPBYTE)&ItemInfo);

			return;
		}
	}
	if (lpMsg->sFlag == 11 || lpMsg->sFlag == 12 || lpMsg->sFlag == 13 || lpMsg->sFlag == 14 || lpMsg->sFlag == 15 || lpMsg->sFlag == 16 || lpMsg->sFlag == 17 ||
		lpMsg->tFlag == 11 || lpMsg->tFlag == 12 || lpMsg->tFlag == 13 || lpMsg->tFlag == 14 || lpMsg->tFlag == 15 || lpMsg->tFlag == 16 || lpMsg->tFlag == 17 || lpMsg->tFlag == 19)
	{
		if (lpObj->m_IfState.use < 1 || lpObj->m_IfState.type != 7)
		{
			this->GCItemMoveResultSend(lpObj, -1, 0, (LPBYTE)&ItemInfo);
			return;
		}

		if (lpObj->ChaosLock == TRUE)
		{
			this->GCItemMoveResultSend(lpObj, -1, 0, (LPBYTE)&ItemInfo);

			return;
		}
	}


	if (lpMsg->sFlag == 2 || lpMsg->tFlag == 2)
	{
		if (lpObj->m_IfState.use < 1 || lpObj->m_IfState.type != 6)
		{
			this->GCItemMoveResultSend(lpObj, -1, 0, (LPBYTE)&ItemInfo);
			return;
		}
	}
	if (lpMsg->sFlag == 1 && lpMsg->tFlag == 0)
	{
		if (lpObj->m_IfState.use < 1 || lpObj->m_IfState.type != 1)
		{
			this->GCItemMoveResultSend(lpObj, -1, 0, (LPBYTE)&ItemInfo);
			return;
		}
	}

	if (lpMsg->sFlag == 2 && lpMsg->tFlag == 0)
	{
		int money = ::GetWarehouseUsedHowMuch(lpObj->Level, lpObj->WarehousePW);

		if ((lpObj->m_PlayerData->Money - money) < 1 && (lpObj->WarehouseMoney - money) < 1)
		{
			this->GCItemMoveResultSend(lpObj, -1, 0, (LPBYTE)&ItemInfo);
			char szTemp[256];
			wsprintf(szTemp, Lang.GetText(0, 154), money);
			this->GCServerMsgStringSend(szTemp, lpObj, 1);

			return;
		}

		if (::bCanWarehouseLock == TRUE)
		{
			if (lpObj->WarehouseLock == 1)
			{
				this->GCItemMoveResultSend(lpObj, -1, 0, (LPBYTE)&ItemInfo);
				this->GCServerMsgStringSend(Lang.GetText(0, 155), lpObj, 1);

				return;
			}
		}
	}
	int type = (ItemInfo[I_TYPE] + ((ItemInfo[I_NOPTION] & 0x80) * 2)) + ((ItemInfo[I_380OPTION] & 0xF0) << 5);

	if (it_type != 8)
	{
		LPITEM_ATTRIBUTE lpItemAttr = GetItemAttr(type);

		if (lpItemAttr == NULL)
		{
			this->GCItemMoveResultSend(lpObj, -1, 0, (LPBYTE)&ItemInfo);
			return;
		}

		if (lpItemAttr->QuestItem != false && !g_QuestExpManager.IsQuestItemAtt(type, 4))
		{
			this->GCItemMoveResultSend(lpObj, -1, 0, (LPBYTE)&ItemInfo);
			return;
		}

		if (type == ITEMGET(14, 64)) // cursed castle water
		{
			this->GCItemMoveResultSend(lpObj, -1, 0, (LPBYTE)&ItemInfo);
			return;
		}
	}

	if (lpMsg->sFlag == 21 && lpMsg->tFlag != 21 && lpMsg->tFlag != 1)
	{
		this->GCItemMoveResultSend(lpObj, -1, 0, ItemInfo);
		return;
	}

	if (lpMsg->sFlag == 1)
	{
		if (lpMsg->tFlag != 21)
		{
			if (lpMsg->tFlag != 1)
			{
				if (IsEventItem(*lpObj->pntTrade[source]) == TRUE)
				{
					GCItemMoveResultSend(lpObj, -1, 0, ItemInfo);
					return;
				}
			}
		}
	}
	if (lpMsg->tFlag != 21)
	{
		int type = (ItemInfo[I_TYPE] + ((ItemInfo[I_NOPTION] & 0x80) * 2)) + ((ItemInfo[I_380OPTION] & 0xF0) << 5);

		if (IsEventItem(*lpObj->pntTrade[type]) == TRUE) // TODO
		{
			this->GCItemMoveResultSend(lpObj, -1, 0, ItemInfo);
			return;
		}
	}

	if (lpMsg->sFlag == 22 && lpMsg->tFlag != 22)
	{
		this->GCItemMoveResultSend(lpObj, -1, 0, ItemInfo);
		return;
	}

	if ((lpMsg->sFlag == 0 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 2 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 2) ||
		(lpMsg->sFlag == 2 && lpMsg->tFlag == 2) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 3) ||

		(lpMsg->sFlag == 0 && lpMsg->tFlag == 5) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 6) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 7) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 8) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 9) ||
		(lpMsg->sFlag == 9 && lpMsg->tFlag == 9) ||
		(lpMsg->sFlag == 9 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 4) ||
		(lpMsg->sFlag == 4 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 4 && lpMsg->tFlag == 4) ||
		(lpMsg->sFlag == 5 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 5 && lpMsg->tFlag == 5) ||
		(lpMsg->sFlag == 6 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 6 && lpMsg->tFlag == 6) ||
		(lpMsg->sFlag == 7 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 7 && lpMsg->tFlag == 7) ||
		(lpMsg->sFlag == 8 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 8 && lpMsg->tFlag == 8) ||
		(lpMsg->sFlag == 3 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 3 && lpMsg->tFlag == 3) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 10) ||
		(lpMsg->sFlag == 10 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 11) ||
		(lpMsg->sFlag == 11 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 12) ||
		(lpMsg->sFlag == 12 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 13) ||
		(lpMsg->sFlag == 13 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 14) ||
		(lpMsg->sFlag == 14 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 15) ||
		(lpMsg->sFlag == 15 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 16) ||
		(lpMsg->sFlag == 16 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 17) ||
		(lpMsg->sFlag == 17 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 11 && lpMsg->tFlag == 11) ||
		(lpMsg->sFlag == 12 && lpMsg->tFlag == 12) ||
		(lpMsg->sFlag == 13 && lpMsg->tFlag == 13) ||
		(lpMsg->sFlag == 14 && lpMsg->tFlag == 14) ||
		(lpMsg->sFlag == 15 && lpMsg->tFlag == 15) ||
		(lpMsg->sFlag == 16 && lpMsg->tFlag == 16) ||
		(lpMsg->sFlag == 17 && lpMsg->tFlag == 17) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 19) ||
		(lpMsg->sFlag == 19 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 19 && lpMsg->tFlag == 19) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 20) ||
		(lpMsg->sFlag == 20 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 20 && lpMsg->tFlag == 20))
	{
		BOOL DurSSend;
		BOOL DurTSend;

		result = gObjInventoryMoveItem(*lpObj, source, target, DurSSend, DurTSend, lpMsg->sFlag, lpMsg->tFlag, (LPBYTE)&ItemInfo);

		if (lpObj->m_IfState.use == 1 && lpObj->m_IfState.type == 3) // Shop
		{
			this->GCPriceSend(lpObj, 1, 0);
		}

		this->GCItemMoveResultSend(lpObj, result, target, (LPBYTE)&ItemInfo);
		if (DurSSend != FALSE)
			this->GCItemDurSend(lpObj, source, lpObj->pntInventory[source]->m_Durability, FALSE);

		if (DurTSend != FALSE)
			this->GCItemDurSend(lpObj, target, lpObj->pntInventory[target]->m_Durability, FALSE);
		if (result != 0xFF && lpMsg->sFlag == 2 && lpMsg->tFlag == 0)
		{
			int money = ::GetWarehouseUsedHowMuch(lpObj->Level, lpObj->WarehousePW);

			if ((lpObj->m_PlayerData->Money - money) > 0)
			{
				int iZen = lpObj->m_PlayerData->Money;
				lpObj->m_PlayerData->Money -= money;
				this->GCMoneySend(lpObj, lpObj->m_PlayerData->Money);

				//sLog->outBasic("Pay WareHouse Money(In Inventory) : %d - %d = %d",iZen, money, lpObj->m_PlayerData->Money);
			}
			else if ((lpObj->WarehouseMoney - money) > 0)
			{
				int iZen = lpObj->WarehouseMoney;
				lpObj->WarehouseMoney -= money;

				//sLog->outBasic("Pay WareHouse Money(In WareHouse) : %d - %d = %d",iZen, money, lpObj->WarehouseMoney);

				this->GCWarehouseInventoryMoneySend(lpObj, 1, lpObj->m_PlayerData->Money, lpObj->WarehouseMoney);
			}
		}
		g_PeriodItemEx.SendPeriodItemInfoOnce(*lpObj, lpObj->pntInventory[target]->m_serial);

		return;
	}

	if (lpMsg->sFlag == 22 && lpMsg->tFlag == 22)
	{
		result = gObjMuunInvenMove(*lpObj, 0, 0, source, target, (LPBYTE)ItemInfo);
		GCItemMoveResultSend(lpObj, result, target, (LPBYTE)ItemInfo);
	}

	if (lpMsg->sFlag == 1 && lpMsg->tFlag == 21)
	{
		result = gObjTradeEventInventoryMove(*lpObj, source, target);

		if (result == 255)
		{
			this->GCItemMoveResultSend(lpObj, -1, target, ItemInfo);
			this->CGTradeCancelButtonRecv(lpObj);

		}

		else
		{
			this->GCItemMoveResultSend(lpObj, result, target, ItemInfo);
		}

		if (result == 255)
		{
			lpObj->m_IfState.state = 0;
			//this->GCEventInventoryItemListSend(lpObj); // TODO

		}

		else
		{
			lpObj->TradeOk = 0;
			getGameObject(lpObj->TargetNumber)->TradeOk = 0;
			GCTradeOkButtonSend(getGameObject(lpObj->TargetNumber), 2);
			GCTradeOkButtonSend(lpObj, 0);

		}

		return;
	}

	if (lpMsg->sFlag == 21 && lpMsg->tFlag == 1)
	{
		result = gObjEventInventoryTradeMove(*lpObj, source, target);
		GCItemMoveResultSend(lpObj, result, target, ItemInfo);

		return;
	}

	if (lpMsg->sFlag == 21 && lpMsg->tFlag == 21)
	{
		BOOL DurSSend;
		BOOL DurTSend;

		result = gObjEventInvenMove(*lpObj, &DurSSend, &DurTSend, source, target);
		this->GCItemMoveResultSend(lpObj, result, target, ItemInfo);

		if (result == 255)
		{
			return;
		}

		/*if (DurSSend)
		{
			this->GCEventItemDurSend(lpObj, source, (int)lpObj->pEventInventory[source].m_Durability);
		}

		if (DurTSend)
		{
			this->GCEventItemDurSend(lpObj, target, (int)lpObj->pEventInventory[target].m_Durability);
		}

		if (lpObj->pEventInventory[target].m_Durability == 0.0)
		{
			GCEventInventoryItemDeleteSend(lpObj, target, TRUE);
		}*/

		return;
	}

	if (lpMsg->sFlag == 1 && lpMsg->tFlag == 1)
	{
		result = gObjTradeTradeMove(*lpObj, source, target);
		this->GCItemMoveResultSend(lpObj, result, target, (LPBYTE)&ItemInfo);
		return;
	}

	if (lpMsg->sFlag == 1 && lpMsg->tFlag == 0)
	{
		result = gObjTradeInventoryMove(*lpObj, source, target);

		if (result == 0xFF)
		{
			this->GCItemMoveResultSend(lpObj, 0xFF, target, (LPBYTE)&ItemInfo);
			this->CGTradeCancelButtonRecv(lpObj);
		}
		else
		{
			this->GCItemMoveResultSend(lpObj, result, target, (LPBYTE)&ItemInfo);
		}

		if (result == 0xFF)
		{
			lpObj->m_IfState.state = 0;
			GCItemListSend(lpObj);
			g_PeriodItemEx.OnRequestPeriodItemList(*lpObj);
		}

		if (lpObj->TargetNumber >= 0)
		{
			CGameObject* lpTargetObj = getGameObject(lpObj->TargetNumber);
			lpObj->TradeOk = false;
			lpTargetObj->TradeOk = false;
			this->GCTradeOkButtonSend(lpTargetObj, 2);
			this->GCTradeOkButtonSend(lpObj, 0);
		}

		return;
	}

	if (lpMsg->sFlag == 0 && lpMsg->tFlag == 1)
	{
		result = gObjInventoryTradeMove(*lpObj, source, target);
		this->GCItemMoveResultSend(lpObj, result, target, (LPBYTE)&ItemInfo);
	}
}



void GameProtocol::GCEquipmentChange(CGameObject* lpObj, int pos)
{
	PMSG_USEREQUIPMENTCHANGED pChange;

	PHeadSetB((LPBYTE)&pChange, 0x25, sizeof(PMSG_USEREQUIPMENTCHANGED));
	pChange.NumberH = SET_NUMBERH(lpObj);
	pChange.NumberL = SET_NUMBERL(lpObj);
	ItemByteConvert(pChange.ItemInfo, *lpObj->pntInventory[pos]);
	pChange.ItemInfo[I_OPTION] = pos << 4;
	pChange.ItemInfo[I_OPTION] |= LevelSmallConvert(lpObj->pntInventory[pos]->m_Level) & MAX_ITEM_LEVEL;
	pChange.Element = lpObj->m_iPentagramMainAttribute;
	if (pChange.ItemInfo[5] == 0xF0)
		pChange.ItemInfo[5] = 0xF1;
	MsgSendV2(lpObj, (LPBYTE)&pChange, pChange.h.size);
}

void GameProtocol::CGTalkRequestRecv(PMSG_TALKREQUEST * lpMsg, CGameObject* lpObj)
{
	int DealerNumber;
	PMSG_TALKRESULT pResult;
	PMSG_SHOPITEMCOUNT pShopItemCount;

	if (!PacketCheckTime(lpObj))
		return;

	if (lpObj->m_PlayerData->ConnectUser->CloseType != -1)
		return;

	if (lpObj->m_PlayerData->m_bSecurityCheck == false)
	{
		MsgOutput(*lpObj, Lang.GetText(0, 822));
		return;
	}

	BYTE SendByte[4096];
	int lOfs = 0;

	if (lpObj->m_IfState.use > 0)
		return;

	DealerNumber = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
	CGameObject* ObjDealer = getGameObject(DealerNumber);

	if (DealerNumber < 0 || DealerNumber > g_ConfigRead.server.GetObjectMax() - 1)
		return;

	if (lpObj->MapNumber != ObjDealer->MapNumber)
		return;

	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		if (ObjDealer->Class == 367)
		{
			if ((lpObj->X < (ObjDealer->X - 5)) || (lpObj->X > (ObjDealer->X + 5)) || (lpObj->Y < (ObjDealer->Y - 10)) || (lpObj->Y > (ObjDealer->Y + 5)))
			{
				return;
			}
		}
		else if ((lpObj->X < (ObjDealer->X - 5)) || (lpObj->X > (ObjDealer->X + 5)) || (lpObj->Y < (ObjDealer->Y - 5)) || (lpObj->Y > (ObjDealer->Y + 5)))
		{
			return;
		}

	}
	else
	{
		if ((lpObj->X < (ObjDealer->X - 5)) || (lpObj->X > (ObjDealer->X + 5)) || (lpObj->Y < (ObjDealer->Y - 5)) || (lpObj->Y > (ObjDealer->Y + 5)))
		{
			return;
		}
	}

	if (ObjDealer->Type == OBJ_NPC)
	{
		if (NpcTalk(*ObjDealer, lpObj) == TRUE)
		{
			//this->GCAnsCsMapSvrTaxInfo(lpObj, 1, g_CastleSiegeSync.GetTaxRateChaos(*lpObj));
			this->GCAnsCsMapSvrTaxInfo(lpObj, 1, 0); // stub
			return;
		}
	}
	// ObjDealer->Type
	if (::gObjFixInventoryPointer(*lpObj) == false)
	{

	}

	if (lpObj->pTransaction == 1)
	{
		return;
	}

	if (ObjDealer->Class == 234)	// Npc Server Division
	{
		pResult.h.c = 0xC3;
		pResult.h.headcode = 0x30;
		pResult.h.size = sizeof(pResult);
		pResult.result = 5;

		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (ObjDealer->Type == OBJ_NPC && ObjDealer->m_btNpcType == NPC_TYPES::NPC_NONE)
		return;

	if (ObjDealer->m_btNpcType == NPC_TYPES::NPC_SHOP)
	{
		SHOP_DATA * lpShopData = g_ShopMng.GetShop(*lpObj, *ObjDealer);

		if (!lpShopData)
		{
			return;
		}
	}

	if (gMerryXMasNpcEvent == TRUE)
	{
		if ((rand() % 6) == 0)
		{
			ChatTargetSend(ObjDealer, Lang.GetText(0, 55), lpObj->m_Index);
		}
	}

	if (::gHappyNewYearNpcEvent == TRUE)
	{
		if ((rand() % 6) == 0)
		{
			ChatTargetSend(ObjDealer, Lang.GetText(0, 56), lpObj->m_Index);
		}
	}

	SHOP_DATA * lpShopData = g_ShopMng.GetShop(*lpObj, *ObjDealer);

	lpObj->TargetNpcNumber = DealerNumber;
	lpObj->m_IfState.use = 1;
	lpObj->m_IfState.type = 3;
	lpObj->m_ShopTime = 0;
	pResult.h.c = 0xC3;
	pResult.h.headcode = 0x30;
	pResult.h.size = sizeof(pResult);

	if (ObjDealer->m_btNpcType == NPC_TYPES::NPC_SHOP && lpShopData->btIsMossMerchant == TRUE)
	{
		pResult.result = 0x22;
	}

	else
	{
		pResult.result = 0x00;
	}

	if (ObjDealer->m_btNpcType == NPC_TYPES::NPC_SHOP && lpShopData->btIsMossMerchant == TRUE && !g_GamblingItemBag.IsEnabled())	return;

	if (ObjDealer->m_btNpcType == NPC_TYPES::NPC_WAREHOUSE)
	{
		if (lpObj->m_PlayerData->m_JoinUnityBattle == true || g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
		{
			this->GCServerMsgStringSend(Lang.GetText(0, 619), lpObj, 1);
			return;
		}

		if (lpObj->m_PlayerData->m_ReqWarehouseOpen != false)
		{
			return;
		}

		if (lpObj->m_PlayerData->m_bSecurityCheck == false)
		{
			return;
		}

		if (lpObj->m_btOpenWarehouse == TRUE)
		{
			return;
		}

		lpObj->m_PlayerData->m_ReqWarehouseOpen = true;
		lpObj->m_IfState.type = 6;
		lpObj->m_IfState.state = 0;
	}

	else if (ObjDealer->m_btNpcType == NPC_TYPES::NPC_CHAOSMIX)
	{
		if (bCanChaosBox == TRUE)
		{
			if (lpObj->m_bPShopOpen == true)
			{
				GCServerMsgStringSend(Lang.GetText(0, 112), lpObj, 1);

				return;
			}

			lpObj->m_IfState.type = 7;
			lpObj->m_IfState.state = 0;
			pResult.result = 3;
			if (ObjDealer->Class == 450)
				pResult.result = 22;
			else if (ObjDealer->Class == 478)
				pResult.result = 32;
			lpObj->bIsChaosMixCompleted = false;
			pResult.level1 = gDQChaosSuccessRateLevel1;
			pResult.level2 = gDQChaosSuccessRateLevel2;
			pResult.level3 = gDQChaosSuccessRateLevel3;
			pResult.level4 = gDQChaosSuccessRateLevel4;
			pResult.level5 = gDQChaosSuccessRateLevel5;
			pResult.level6 = gDQChaosSuccessRateLevel6;
			pResult.level7 = gDQChaosSuccessRateLevel7;

			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
			gObjInventoryTrans(*lpObj);

			gObjItemTextSave(*lpObj);
			gObjWarehouseTextSave(*lpObj);
			//GCAnsCsMapSvrTaxInfo(lpObj, 1, ::g_CastleSiegeSync.GetTaxRateChaos(*lpObj));
			GCAnsCsMapSvrTaxInfo(lpObj, 1, 0); // stub
		}
	}

	else if (lpShopData->btIsMossMerchant == TRUE)
	{
		pResult.result = 34;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
	}

	else
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
	}

	if (ObjDealer->m_btNpcType == NPC_TYPES::NPC_WAREHOUSE)
	{
		if (lpObj->m_PlayerData->m_bMapSvrMoveReq == true)
		{
			lpObj->m_PlayerData->m_ReqWarehouseOpen = false;
			return;
		}

		if (lpObj->m_State == 32)
		{
			lpObj->m_PlayerData->m_ReqWarehouseOpen = false;
			return;
		}

		if (lpObj->m_PlayerData->m_bMapSvrMoveQuit == true)
		{
			lpObj->m_PlayerData->m_ReqWarehouseOpen = false;
			return;
		}

		if (lpObj->m_IfState.use && lpObj->m_IfState.type == 19)
		{
			return;
		}

		if (lpObj->WarehouseSaveLock == TRUE)
		{
			lpObj->m_PlayerData->m_ReqWarehouseOpen = false;
			return;
		}

		lpObj->WarehouseCount = 0;
		GDGetWarehouseList(lpObj, lpObj->m_PlayerData->ConnectUser->AccountID);
		return;
	}

	if (ObjDealer->m_btNpcType == NPC_TYPES::NPC_CHAOSMIX)
		return;

	else if (ObjDealer->m_btNpcType == NPC_TYPES::NPC_SHOP)
	{
		lOfs = 0;

		lOfs += sizeof(pShopItemCount);

		int size = lOfs + lpShopData->m_ShopData.SendItemDataLen;
		PHeadSetW((LPBYTE)&pShopItemCount, 0x31, size);
		pShopItemCount.Type = 0;
		pShopItemCount.count = lpShopData->m_ShopData.ItemCount;
		memcpy(SendByte, &pShopItemCount, sizeof(pShopItemCount));
		memcpy(&SendByte[lOfs], lpShopData->m_ShopData.SendItemData, lpShopData->m_ShopData.SendItemDataLen);

		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, SendByte, size);
		//GCAnsCsMapSvrTaxInfo(lpObj, 2, ::g_CastleSiegeSync.GetTaxRateStore(*lpObj));
		GCAnsCsMapSvrTaxInfo(lpObj, 2, 0); // stub
	}
}




void GameProtocol::GCUserWarehouseSend(CGameObject* lpObj)
{
	if (lpObj->m_IfState.type != 6)
		return;

	if (lpObj->m_IfState.type == 6 && lpObj->m_IfState.state == 1)
		return;

	if (lpObj->m_btOpenWarehouse == TRUE)
	{
		return;
	}

	PMSG_SHOPITEMCOUNT pMsg;
	BYTE SendByte[4096];
	int lOfs = sizeof(pMsg);

	PHeadSetW((LPBYTE)&pMsg, 0x31, 0);
	pMsg.count = 0;
	pMsg.Type = 0;

	for (int n = 0; n < WAREHOUSE_SIZE; n++)
	{
		if (lpObj->pntWarehouse[n]->IsItem() == TRUE)
		{
			SendByte[lOfs] = n;
			lOfs++;
			ItemByteConvert(&SendByte[lOfs], *lpObj->pntWarehouse[n]);
			lOfs += MAX_ITEM_INFO;
			pMsg.count++;
		}
	}

	pMsg.h.sizeH = SET_NUMBERH(lOfs);
	pMsg.h.sizeL = SET_NUMBERL(lOfs);
	memcpy(SendByte, &pMsg, sizeof(pMsg));

	GIOCP.DataSend(lpObj->m_Index, SendByte, lOfs);
	this->GCWarehouseInventoryMoneySend(lpObj, 1, lpObj->m_PlayerData->Money, lpObj->WarehouseMoney);

	if (lpObj->m_IfState.type == 6)
	{
		lpObj->m_IfState.state = 1;
	}

	lpObj->m_btOpenWarehouse = TRUE;
}

void GameProtocol::CGBuyRequestRecv(PMSG_BUYREQUEST * lpMsg, CGameObject* lpObj)
{
	PMSG_BUYRESULT pResult;

	PHeadSetB((LPBYTE)&pResult, 0x32, sizeof(pResult));
	pResult.Result = -1;

	if (!PacketCheckTime(lpObj))
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	if (lpObj->m_PlayerData->ConnectUser->CloseType != -1)
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (lpObj->TargetNpcNumber == -1)
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	CGameObject* lpShopObj = getGameObject(lpObj->TargetNpcNumber);

	SHOP_DATA * lpShopData = g_ShopMng.GetShop(*lpObj, *lpShopObj);

	if (!lpShopData)
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (!::gObjFixInventoryPointer(*lpObj))
	{

	}

	if (lpObj->m_ShopTime == 0)
		lpObj->m_ShopTime = 1;

	if (lpShopData->btIsMossMerchant == TRUE)
	{
		if (lpObj->m_ShopTime > g_GamblingItemBag.GetGamblingShopTime())
		{
			if (lpObj->m_IfState.use && lpObj->m_IfState.type == 3)
			{
				lpObj->TargetNpcNumber = -1;
				pResult.Result = 0xFE;
				lpObj->m_IfState.use = 0;
				lpObj->m_IfState.type = 0;
			}

			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
			GCServerMsgStringSend(Lang.GetText(0, 520), lpObj, 1);
			return;
		}
	}

	if (lpObj->m_ShopTime > 60)
	{
		if (lpObj->m_IfState.use != 0 && lpObj->m_IfState.type == 3)
		{
			lpObj->TargetNpcNumber = -1;
			lpObj->m_IfState.use = 0;
			lpObj->m_IfState.type = 0;
		}

		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (lpObj->m_IfState.use > 0)
	{
		if (lpObj->m_IfState.type != 3)
		{
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
			return;
		}
	}

	if (lpMsg->Pos > MAX_ITEM_IN_SHOP - 1)
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (lpObj->m_PlayerData->m_bIsCancelItemSale == true)
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (lpShopData->m_ShopData.m_item[lpMsg->Pos]->IsItem() == TRUE)
	{
		//int iStoreTaxMoney = lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_BuyMoney + (int)((__int64)lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_BuyMoney * (__int64)g_CastleSiegeSync.GetTaxRateStore(*lpObj) / (__int64)100);
		int iStoreTaxMoney = lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_BuyMoney; // stub

		if (iStoreTaxMoney < 0)
			iStoreTaxMoney = 0;

		//int iStoreTaxMoney2 = (int)((__int64)lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_BuyMoney * (__int64)g_CastleSiegeSync.GetTaxRateStore(*lpObj) / (__int64)100);
		int iStoreTaxMoney2 = (int)(__int64)lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_BuyMoney; // stub

		if (iStoreTaxMoney2 < 0)
			iStoreTaxMoney2 = 0;

		if (g_ArcaBattle.IsArcaBattleServer() == 1 && iStoreTaxMoney > 0)
		{
			iStoreTaxMoney *= 3;
		}

		if (lpShopData->btIsMossMerchant == TRUE)
		{
			//int iTaxGambling = g_GamblingItemBag.GetGamblingValue() * g_CastleSiegeSync.GetTaxRateStore(*lpObj) / 100;
			//iStoreTaxMoney = g_GamblingItemBag.GetGamblingValue() + iTaxGambling;
			iStoreTaxMoney = g_GamblingItemBag.GetGamblingValue(); // stub
		}

		if (lpObj->m_PlayerData->Money < iStoreTaxMoney)
			pResult.Result = -1;
		else
		{
			BOOL bNoItem = TRUE;

			if ((lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type >= ITEMGET(14, 0) && lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type <= ITEMGET(14, 8)) ||
				(lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type >= ITEMGET(14, 35) && lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type <= ITEMGET(14, 40)))
			{
				int dur = lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Durability;

				if (dur == 0)
					dur = 1;

				if (::gObjSearchItem(*lpObj, lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type, dur, lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Level) == TRUE)
				{
					bNoItem = FALSE;
					lpObj->m_PlayerData->Money -= iStoreTaxMoney;
					//::g_CastleSiegeSync.AddTributeMoney(iStoreTaxMoney2);

					if (lpObj->m_PlayerData->Money < 0)
						lpObj->m_PlayerData->Money = 0;

					this->GCMoneySend(lpObj, lpObj->m_PlayerData->Money);
				}
			}

			if (lpShopData->btIsMossMerchant == TRUE)
			{
				if (!g_GamblingItemBag.IsEnabled())
				{
					pResult.Result = 0xFF;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
					return;
				}

				if (bNoItem)
				{
					int iCount = 0;

					while (iCount <= lpShopData->m_ShopData.ItemCount)
					{
						if (lpMsg->Pos == lpShopData->m_ShopData.SendItemData[iCount + (12 * iCount)])
						{
							pResult.Result = g_GamblingItemBag.GetGamblingSystemItem(*lpObj, 229, 0, 0, iCount);
							break;
						}
						iCount++;
					}

					pResult.Result = 0xFF;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
					return;
				}
			}

			if (bNoItem)
			{
				if (lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type == ITEMGET(13, 239) ||// Muun Energy Converter
					lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type >= ITEMGET(13, 212) && // Pets Muun
					lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type <= ITEMGET(13, 219) ||// Pets Muun
					lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type >= ITEMGET(13, 223) && // Pets Muun
					lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type <= ITEMGET(13, 275) ||// Pets Muun
					lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type >= ITEMGET(13, 278) && // Pets Muun
					lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type <= ITEMGET(13, 349) ||// Pets Muun
					lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type >= ITEMGET(13, 409) && // Pets Muun
					lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type <= ITEMGET(13, 464) ||// Pets Muun
					lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type >= ITEMGET(13, 480) && // Pets Muun
					lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type <= ITEMGET(13, 503)) // Pets Muun
				{
					BYTE btRet = gObjMuunInventoryInsertItem(*lpObj, *lpShopData->m_ShopData.m_item[lpMsg->Pos]);

					if (btRet == 0xFF)
					{
						pResult.Result = 0xFF;
						GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
						return;
					}

					ItemByteConvert(pResult.ItemInfo, *lpShopData->m_ShopData.m_item[lpMsg->Pos]);
					int buymoney = lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_BuyMoney;
					lpObj->m_PlayerData->Money -= iStoreTaxMoney;
					//g_CastleSiegeSync.AddTributeMoney(iStoreTaxMoney2);
					if (lpObj->m_PlayerData->Money < 0) lpObj->m_PlayerData->Money = 0;
					GCMoneySend(lpObj, lpObj->m_PlayerData->Money);
					//int iTaxRate = g_CastleSiegeSync.GetTaxRateStore(*lpObj);
					int iTaxRate = 0; // stub

					this->GCMuunInventoryItemOneSend(lpObj, btRet);

					pResult.Result = 0xFF;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
					return;
				}

				if (lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_serial == 0)
				{
					LPITEM_ATTRIBUTE p = GetItemAttr(lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type);

					if (!p)
					{
						pResult.Result = 0xFF;
						GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
						return;
					}

					pResult.Result = gObjShopBuyInventoryInsertItem(*lpObj, *lpShopData->m_ShopData.m_item[lpMsg->Pos]);

					if (pResult.Result != 0xFF)
					{

						ItemByteConvert(pResult.ItemInfo, *lpShopData->m_ShopData.m_item[lpMsg->Pos]);
						int buymoney = lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_BuyMoney;
						lpObj->m_PlayerData->Money -= iStoreTaxMoney;
						//g_CastleSiegeSync.AddTributeMoney(iStoreTaxMoney2);
						if (lpObj->m_PlayerData->Money < 0) lpObj->m_PlayerData->Money = 0;
						GCMoneySend(lpObj, lpObj->m_PlayerData->Money);
						//int iTaxRate = g_CastleSiegeSync.GetTaxRateStore(*lpObj);
					}
				}

				else
				{
					LPITEM_ATTRIBUTE p = GetItemAttr(lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type);

					if (!p)
					{
						pResult.Result = 0xFF;
						GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
						return;
					}

					if (CheckInventoryEmptySpace(*lpObj, p->Height, p->Width) == FALSE)
					{
						pResult.Result = 0xFF;
						GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
						return;
					}

					int buymoney = lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_BuyMoney;
					lpObj->m_PlayerData->Money -= iStoreTaxMoney;
					//g_CastleSiegeSync.AddTributeMoney(iStoreTaxMoney2);
					if (lpObj->m_PlayerData->Money < 0) lpObj->m_PlayerData->Money = 0;
					GCMoneySend(lpObj, lpObj->m_PlayerData->Money);
					//int iTaxRate = g_CastleSiegeSync.GetTaxRateStore(*lpObj);
					int iTaxRate = 0; // stub

					ItemCreate(lpObj, 222, 0, 0, lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type, lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Level,
						lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Durability, lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Option1,
						lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Option2, lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Option3,
						lpObj->m_Index, lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_NewOption, lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_SetOption,
						0, lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_SocketOption, lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_BonusSocketOption);

				}
			}
		}
	}

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
}



struct PMSG_SELLRESULT
{
	PBMSG_HEAD h;	// C1:33
	BYTE Result;	// 3
	DWORD Money;	// 4
};



void GameProtocol::CGSellRequestRecv(PMSG_SELLREQUEST * lpMsg, CGameObject* lpObj)
{
	PMSG_SELLRESULT pResult;

	PHeadSetB((LPBYTE)&pResult, 0x33, sizeof(pResult));
	pResult.Result = false;
	pResult.Money = 0;

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	if (lpObj->m_PlayerData->ConnectUser->CloseType != -1)
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (lpObj->m_IfState.use > 0 && lpObj->m_IfState.type != 3)
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (!PacketCheckTime(lpObj))
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (!::gObjFixInventoryPointer(*lpObj))
	{

	}

	if (lpObj->pTransaction == 1)
	{
		return;
	}

	if (lpObj->TargetNpcNumber == -1)
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	CGameObject* lpShopObj = getGameObject(lpObj->TargetNpcNumber);

	SHOP_DATA * lpShopData = g_ShopMng.GetShop(*lpObj, *lpShopObj);

	if (!lpShopData)
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (!gObjCanItemTouch(*lpObj, 3))
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (lpMsg->Pos > MAIN_INVENTORY_SIZE - 1 && lpMsg->Pos != 236)
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (gObjCheckSerial0ItemList(lpObj->pntInventory[lpMsg->Pos]) != FALSE)
	{
		MsgOutput(*lpObj, Lang.GetText(0, 259));
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

		return;
	}

	if (g_ConfigRead.data.common.ShopExCItemObjectLimit == FALSE)
	{
		if (lpObj->pntInventory[lpMsg->Pos]->m_NewOption == 63)
		{
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
			return;
		}
	}

	if (g_ConfigRead.data.common.EnableSellAllItems == false)
	{
		if (lpObj->pntInventory[lpMsg->Pos]->m_Type == ITEMGET(14, 11)) // Box of Luck
		{
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
			return;
		}

		if (lpObj->pntInventory[lpMsg->Pos]->m_QuestItem != false && !g_QuestExpManager.IsQuestItemAtt(lpObj->pntInventory[lpMsg->Pos]->m_Type, 8))
		{
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
			return;
		}

		if (lpObj->pntInventory[lpMsg->Pos]->m_Type == ITEMGET(13, 20) && lpObj->pntInventory[lpMsg->Pos]->m_Level == 1) // Wizards Ring +1
		{
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
			return;
		}

		if (lpObj->pntInventory[lpMsg->Pos]->m_Type == ITEMGET(13, 20) && lpObj->pntInventory[lpMsg->Pos]->m_Level == 2) // Wizards Ring +2
		{
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
			return;
		}

		if (lpObj->pntInventory[lpMsg->Pos]->m_Type == ITEMGET(12, 26) &&
			(lpObj->pntInventory[lpMsg->Pos]->m_Level == 1 ||
				lpObj->pntInventory[lpMsg->Pos]->m_Level == 2 ||
				lpObj->pntInventory[lpMsg->Pos]->m_Level == 3 ||
				lpObj->pntInventory[lpMsg->Pos]->m_Level == 4 ||
				lpObj->pntInventory[lpMsg->Pos]->m_Level == 5))
		{
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
			return;
		}

		if (g_kJewelOfHarmonySystem.IsStrengthenByJewelOfHarmony(lpObj->pntInventory[lpMsg->Pos]) == TRUE && g_ConfigRead.data.common.joinmuRemoveItemSellDropRestriction == false)
		{
			GCServerMsgStringSend(Lang.GetText(0, 267), lpObj, 1);
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
			return;
		}

		if (lpObj->pntInventory[lpMsg->Pos]->IsPeriodItem() == TRUE && lpObj->pntInventory[lpMsg->Pos]->IsPeriodItemExpire() == FALSE)
		{
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
			return;
		}

		if (lpObj->pntInventory[lpMsg->Pos]->IsPeriodItem() == TRUE)
		{
			if (lpObj->pntInventory[lpMsg->Pos]->m_Type == ITEMGET(12, 266) ||
				lpObj->pntInventory[lpMsg->Pos]->m_Type == ITEMGET(12, 267))
			{
				lpObj->pntInventory[lpMsg->Pos]->SetPeriodItemExpire();
				g_PeriodItemEx.RemovePeriodItemDataByForce(*lpObj, lpObj->pntInventory[lpMsg->Pos]->m_Type, lpObj->pntInventory[lpMsg->Pos]->m_Number);
			}

			else if (lpObj->pntInventory[lpMsg->Pos]->IsPeriodItemExpire() == FALSE)
			{
				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
				return;
			}
		}
	}

	if (lpObj->pntInventory[lpMsg->Pos]->m_Type == ITEMGET(14, 12) && lpObj->pntInventory[lpMsg->Pos]->m_Level == 1) // Heart+1
	{
		pResult.Result = true;
		pResult.Money = lpObj->m_PlayerData->Money;
		gObjInventoryItemSet(*lpObj, lpMsg->Pos, -1);
		lpObj->pntInventory[lpMsg->Pos]->Clear();
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

		return;
	}

	if (!IsSellToNPCItem(lpObj->pntInventory[lpMsg->Pos]->m_Type))
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (lpObj->pntInventory[lpMsg->Pos]->IsItem() == FALSE)
	{
		pResult.Result = false;
		pResult.Money = lpObj->m_PlayerData->Money;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

	}
	else
	{
		BYTE NewOption[MAX_EXOPTION_SIZE];
		int iAddZen = 0;
		int type = lpObj->pntInventory[lpMsg->Pos]->m_Type;

		if (type == ITEMGET(13, 4) || type == ITEMGET(13, 5)) // Pets
			lpObj->pntInventory[lpMsg->Pos]->PetValue();
		else
			lpObj->pntInventory[lpMsg->Pos]->Value();

		iAddZen = lpObj->pntInventory[lpMsg->Pos]->m_SellMoney;

		if (gObjCheckMaxZen(*lpObj, iAddZen) == FALSE)
		{
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
			return;
		}

		lpObj->m_PlayerData->Money += iAddZen;
		pResult.Result = true;
		pResult.Money = lpObj->m_PlayerData->Money;


		ItemIsBufExOption(NewOption, lpObj->pntInventory[lpMsg->Pos]);

		if (g_PentagramSystem.IsPentagramItem(*lpObj->pntInventory[lpMsg->Pos]))
		{
			g_PentagramSystem.DelPentagramJewelInfo(*lpObj, lpObj->pntInventory[lpMsg->Pos]);
		}

		bool bSendEquip = false;

		if ((lpObj->pntInventory[lpMsg->Pos]->m_Type == ITEMGET(13, 169) || lpObj->pntInventory[lpMsg->Pos]->m_Type == ITEMGET(13, 170)) && (lpMsg->Pos == 10 || lpMsg->Pos == 11))
		{
			bSendEquip = true;
		}

		if (IsExpensiveItem(lpObj->pntInventory[lpMsg->Pos]) == TRUE && lpObj->pntInventory[lpMsg->Pos]->IsPeriodItem() == FALSE)
		{
			g_CancelItemSale.GDReqAddItemToList(*lpObj, *lpObj->pntInventory[lpMsg->Pos], iAddZen);
		}

		::gObjInventoryDeleteItem(*lpObj, lpMsg->Pos);

		if (bSendEquip == true)
		{
			this->GCEquipmentSend(lpObj);
		}

		if (lpMsg->Pos == 10 || lpMsg->Pos == 11)
		{
			::gObjUseSkill.SkillChangeUse(*lpObj);
		}

		::gObjMakePreviewCharSet(*lpObj);

	}

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
}


int GameProtocol::GetNeedMoneyItemDurRepaire(/*CGameObject* lpObj, */CItemObject*  DurItem, BOOL RequestPos)
{
	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		return 1;
	}

	int iMoney = 0;
	int type = DurItem->m_Type;
	float ItemDurability = DurItem->m_Durability;

	if (type == ITEMGET(13, 10)) // Transformation Ring
		return 0;

	if (::HasItemDurability(type) != FALSE)
	{
		float lc4;
		float lc5 = 0;
		lc4 = DurItem->m_BaseDurability;

		if (ItemDurability == lc4)
			return 0;

		float lc6 = 1.0f - ItemDurability / lc4;
		int lc7;

		if (type == ITEMGET(13, 4) || type == ITEMGET(13, 5)) // Pets
		{
			lc7 = DurItem->m_BuyMoney;
		}
		else
		{
			lc7 = DurItem->m_BuyMoney / 3;

		}

		if (lc7 > 400000000) lc7 = 400000000;

		if (lc7 >= 1000)
			lc7 = lc7 / 100 * 100;
		else if (lc7 >= 100)
			lc7 = lc7 / 10 * 10;

		float lc8 = sqrt(static_cast<float>(lc7));
		float lc9 = sqrt(sqrt(static_cast<float>(lc7)));
		lc5 = 3.0f * lc8 * lc9;
		lc5 *= lc6;
		lc5 += 1.0f;

		if (ItemDurability <= 0.0f)
		{
			if (type == ITEMGET(13, 4) || type == ITEMGET(13, 5)) // Pets
				lc5 *= 2;
			else
				lc5 *= ::GetAllRepairItemRate(type);

		}

		if (RequestPos == TRUE)
		{
			lc5 += lc5 * 0.05f;
		}
		iMoney = lc5;

		if (iMoney >= 1000)
			iMoney = iMoney / 100 * 100;
		else if (iMoney >= 100)	// Deathway fix Here -> Must be 100 and >=
			iMoney = iMoney / 10 * 10;

	}
	else
	{
		iMoney = -1;
	}

	return iMoney;
}



struct PMSG_ITEMDURREPAIR_RESULT
{
	PBMSG_HEAD h;	// C1:34
	int Money;	// 4
};



void GameProtocol::ItemDurRepaire(CGameObject* lpObj, /*CGameObject* lpObj, */CItemObject* DurItem, int pos, int RequestPos)
{
	PMSG_ITEMDURREPAIR_RESULT pResult;
	int result = TRUE;

	PHeadSetB((LPBYTE)&pResult, 0x34, sizeof(pResult));
	int itemtype = DurItem->m_Type;

	if (lpObj->m_IfState.type != 13)
	{
		if (itemtype == ITEMGET(13, 4) || itemtype == ITEMGET(13, 5))
		{
			pResult.Money = 0;
			GIOCP.DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);

			return;
		}
	}

	if (DurItem->IsPeriodItem() == TRUE)
	{
		pResult.Money = 0;
		GIOCP.DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);

		return;
	}

	if (itemtype >= ITEMGET(14, 0) || (itemtype >= ITEMGET(13, 0) && itemtype < ITEMGET(13, 4)) || itemtype == ITEMGET(13, 10) || (itemtype >= ITEMGET(12, 7) && itemtype <= ITEMGET(12, 35)) || itemtype == ITEMGET(4, 7) || itemtype == ITEMGET(4, 15))
	{
		pResult.Money = 0;
		GIOCP.DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);

		return;
	}

	if (itemtype == ITEMGET(13, 38))
	{
		pResult.Money = 0;
		GIOCP.DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);

		return;
	}

	if (itemtype == ITEMGET(13, 39))
	{
		pResult.Money = 0;
		GIOCP.DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);

		return;
	}

	if (itemtype == ITEMGET(13, 40))
	{
		pResult.Money = 0;
		GIOCP.DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);

		return;
	}

	if (itemtype == ITEMGET(13, 41))
	{
		pResult.Money = 0;
		GIOCP.DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);

		return;
	}

	if (itemtype == ITEMGET(13, 42))
	{
		pResult.Money = 0;
		GIOCP.DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);

		return;
	}

	pResult.Money = GetNeedMoneyItemDurRepaire(DurItem, RequestPos);

	if (pResult.Money <= 0)
	{
		pResult.Money = 0;
		GIOCP.DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);

		return;
	}

	if ((lpObj->m_PlayerData->Money - pResult.Money) < 1)
		pResult.Money = 0;
	else
	{
		lpObj->m_PlayerData->Money -= pResult.Money;
		pResult.Money = lpObj->m_PlayerData->Money;
		DurItem->m_Durability = (float)((int)DurItem->m_BaseDurability);
		DurItem->Convert(DurItem->m_Type, DurItem->m_Option1, DurItem->m_Option2, DurItem->m_Option3,
			DurItem->m_NewOption, DurItem->m_SetOption, DurItem->m_ItemOptionEx, 0, -1, 0, CURRENT_DB_VERSION);
		GCItemDurSend(lpObj, pos, DurItem->m_Durability, FALSE);
	}

	GIOCP.DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);
}




void GameProtocol::CGModifyRequestItem(PMSG_ITEMDURREPAIR * lpMsg, CGameObject* lpObj)
{
	PMSG_ITEMDURREPAIR_RESULT pResult;

	PHeadSetB((LPBYTE)&pResult, 0x34, sizeof(pResult));
	pResult.Money = 0;

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	if (lpObj->m_PlayerData->ConnectUser->CloseType != -1)
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (!PacketCheckTime(lpObj))
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (lpMsg->Requestpos == 1 && lpObj->Level < 50)
	{
		pResult.Money = 0;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (lpMsg->Position == 0xFF)
	{
		for (int n = 0; n < MAIN_INVENTORY_SIZE; n++)
		{
			if (lpObj->pntInventory[n]->IsItem())
			{
				if (IsCashItem(lpObj->pntInventory[n]->m_Type) == TRUE)
					continue;

				if (IsPremiumItem(lpObj->pntInventory[n]->m_Type) == TRUE)
					continue;

				if (lpObj->pntInventory[n]->IsPeriodItem() == TRUE)
					continue;

				if (lpObj->pntInventory[n]->IsPeriodItemExpire() == TRUE)
					continue;

				if (g_LuckyItemManager.IsLuckyItemEquipment(lpObj->pntInventory[n]->m_Type))	//season 6.1 add-on
					continue;

				if (lpObj->pntInventory[n]->m_Type == ITEMGET(13, 20) && (lpObj->pntInventory[n]->m_Level == 0 || lpObj->pntInventory[n]->m_Level == 1))
					continue;

				if (lpObj->pntInventory[n]->m_Type == ITEMGET(13, 20) && lpObj->pntInventory[n]->m_Level == 2)
					continue;

				if (lpObj->pntInventory[n]->m_Type == ITEMGET(13, 4) || lpObj->pntInventory[n]->m_Type == ITEMGET(13, 5))
					continue;

				if (lpObj->pntInventory[n]->m_Type == ITEMGET(13, 37))
					continue;

				if (lpObj->pntInventory[n]->m_Type == ITEMGET(13, 38))
					continue;

				if (lpObj->pntInventory[n]->m_Type == ITEMGET(13, 40))
					continue;

				if (lpObj->pntInventory[n]->m_Type == ITEMGET(13, 41))
					continue;

				if (lpObj->pntInventory[n]->m_Type == ITEMGET(13, 42))
					continue;

				if (lpObj->pntInventory[n]->m_Type == ITEMGET(13, 51))
					continue;

				if (lpObj->pntInventory[n]->m_Type == ITEMGET(13, 64))
					continue;

				if (lpObj->pntInventory[n]->m_Type == ITEMGET(13, 65))
					continue;

				if (lpObj->pntInventory[n]->m_Type == ITEMGET(13, 67))
					continue;

				if (lpObj->pntInventory[n]->m_Type == ITEMGET(13, 70))
					continue;

				if (lpObj->pntInventory[n]->m_Type == ITEMGET(13, 76))
					continue;

				if (lpObj->pntInventory[n]->m_Type == ITEMGET(13, 77))
					continue;

				if (lpObj->pntInventory[n]->m_Type == ITEMGET(13, 78))
					continue;

				if (lpObj->pntInventory[n]->m_Type == ITEMGET(13, 80))
					continue;

				if (lpObj->pntInventory[n]->m_Type == ITEMGET(13, 106))
					continue;

				if (lpObj->pntInventory[n]->m_Type == ITEMGET(13, 123))
					continue;

				if (lpObj->pntInventory[n]->m_Type == ITEMGET(13, 39))
					continue;

				if (lpObj->pntInventory[n]->m_Type == ITEMGET(12, 26) &&
					(lpObj->pntInventory[n]->m_Level == 1 ||
						lpObj->pntInventory[n]->m_Level == 2 ||
						lpObj->pntInventory[n]->m_Level == 3 ||
						lpObj->pntInventory[n]->m_Level == 4 ||
						lpObj->pntInventory[n]->m_Level == 5))
					continue;

				if (!IsRepairItem(lpObj->pntInventory[n]->m_Type))
					continue;

				ItemDurRepaire(lpObj,/*lpObj,*/ lpObj->pntInventory[n], n, lpMsg->Requestpos);
			}
		}

		gObjCalCharacter.CalcCharacter(*lpObj);
		return;
	}

	if (lpMsg->Position > MAIN_INVENTORY_SIZE - 1)
	{
		pResult.Money = 0;
		return;
	}

	if (lpObj->pntInventory[lpMsg->Position]->IsPeriodItem() == TRUE)
		return;

	if (IsCashItem(lpObj->pntInventory[lpMsg->Position]->m_Type) == TRUE)
		return;

	if (IsPremiumItem(lpObj->pntInventory[lpMsg->Position]->m_Type) == TRUE)
		return;

	if (lpObj->pntInventory[lpMsg->Position]->IsPeriodItemExpire() == TRUE)
		return;

	if (g_LuckyItemManager.IsLuckyItemEquipment(lpObj->pntInventory[lpMsg->Position]->m_Type))	//season 6.1 add-on
		return;

	if (lpObj->pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 20) && (lpObj->pntInventory[lpMsg->Position]->m_Level == 0 || lpObj->pntInventory[lpMsg->Position]->m_Level == 1))
		return;

	if (lpObj->pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 20) && lpObj->pntInventory[lpMsg->Position]->m_Level == 2)
		return;

	if (lpObj->pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 37))
		return;

	if (lpObj->pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 38))
		return;

	if (lpObj->pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 40))
		return;

	if (lpObj->pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 41))
		return;

	if (lpObj->pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 42))
		return;

	if (lpObj->pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 51))
		return;

	if (lpObj->pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 70))
		return;

	if (lpObj->pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 66))
		return;

	if (lpObj->pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 76))
		return;

	if (lpObj->pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 77))
		return;

	if (lpObj->pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 78))
		return;

	if (lpObj->pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 80))
		return;

	if (lpObj->pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 106))
		return;

	if (lpObj->pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 123))
		return;

	if (lpObj->pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 39))
		return;

	if (!IsRepairItem(lpObj->pntInventory[lpMsg->Position]->m_Type))
		return;

	ItemDurRepaire(lpObj,/*lpObj,*/ lpObj->pntInventory[lpMsg->Position], lpMsg->Position, lpMsg->Requestpos);
	gObjCalCharacter.CalcCharacter(*lpObj);
}



struct PMSG_TRADE_REQUESTSEND
{
	PBMSG_HEAD h;	// C3:36
	char szId[10];	// 3
};


void GameProtocol::CGTradeRequestSend(PMSG_TRADE_REQUEST * lpMsg, CGameObject* lpObj)
{
	int number;

	if (bCanTrade == FALSE)
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 57), lpObj, 1);
		this->GCServerMsgStringSend(Lang.GetText(0, 58), lpObj, 1);
		return;
	}

	if (lpObj->m_PlayerData->ConnectUser->CloseType != -1)
		return;


	if (!PacketCheckTime(lpObj))
		return;

	number = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
	
	CGameObject* lpObjTarget = getGameObject(number);

	if (!gObjIsConnected(*lpObjTarget))
		return;

	if (lpObjTarget->Type != OBJ_USER || lpObj->Type != OBJ_USER) // ANTI_CRASH
	{
		return;
	}

	if (lpObj->m_PlayerData->m_bSecurityCheck == false)
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 521), lpObj, 1);
		return;
	}

	if (lpObjTarget->m_PlayerData->m_bSecurityCheck == false)
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 606), lpObj, 1);
		return;
	}

	if (lpObjTarget->m_PlayerData->m_bMapSvrMoveReq == true || lpObj->m_PlayerData->m_bMapSvrMoveReq == true)
	{
		return;
	}

	if (lpObjTarget->m_PlayerData->m_bMapSvrMoveQuit == true || lpObj->m_PlayerData->m_bMapSvrMoveQuit == true)
	{
		return;
	}

	if (lpObjTarget->m_PlayerData->ConnectUser->CloseCount >= 0)
		return;

	/*if (DS_MAP_RANGE(lpObjTarget->MapNumber) != FALSE)
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 12), lpObj, 1);
		return;
	}*/

	if (lpObj->m_bPShopOpen == true)
		return;

	if (lpObjTarget->m_bPShopOpen == true)
		return;

	/*if (CC_MAP_RANGE(lpObjTarget->MapNumber) != FALSE)
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 114), lpObj, 1);
		return;
	}*/

	if (lpObjTarget->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 572), lpObj, 1);
		return;
	}

	if (lpObj->m_PlayerData->m_JoinUnityBattle == true ||
		lpObjTarget->m_PlayerData->m_JoinUnityBattle == true ||
		g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 618), lpObj, 1);
		return;
	}

	if (IT_MAP_RANGE(lpObjTarget->MapNumber) != FALSE)
	{
		GCServerMsgStringSend(Lang.GetText(0, 290), lpObj, 1);
		return;
	}

	if (DG_MAP_RANGE(lpObjTarget->MapNumber) != FALSE)
	{
		GCServerMsgStringSend(Lang.GetText(0, 303), lpObj, 1);
		return;
	}

	if (IMPERIAL_MAP_RANGE(lpObjTarget->MapNumber) != FALSE)
	{
		GCServerMsgStringSend(Lang.GetText(0, 304), lpObj, 1);
		return;
	}

	/*if (BC_MAP_RANGE(lpObj->MapNumber) != FALSE)
	{
		int iBridgeIndex = g_BloodCastle.GetBridgeIndex(lpObj->MapNumber);
		if (g_BloodCastle.GetCurrentState(iBridgeIndex) != 1 || g_BloodCastle.CheckCanEnter(iBridgeIndex) == false)
		{
			this->GCServerMsgStringSend(Lang.GetText(0, 107), lpObj, 1);
			return;
		}
	}*/

	if (lpObjTarget->m_PlayerData->ISBOT == true && g_BotSystem.GetBotType(*getGameObject(number)) == BOT_TRADE)
	{
		g_BotSystem.AlchemistTradeOpen(*lpObj, number);
		return;
	}
	if (lpObjTarget->m_PlayerData->ISBOT == true && g_BotSystem.GetBotType(*getGameObject(number)) == BOT_BUFFER)
	{
		lpObjTarget->BuffPlayerIndex = lpObj->m_Index;
		g_BotSystem.BuffPlayer(lpObjTarget->BufferIndex, *lpObj);
		return;
	}

	if ((lpObjTarget->m_Option & 1) != 1)
	{
		this->GCTradeResponseSend(0, lpObj, lpObjTarget->Name, 0, 0);
		return;
	}

	if (lpObj->m_IfState.use > 0)
	{
		this->GCTradeResponseSend(3, lpObj, lpObjTarget->Name, 0, 0);
		return;
	}

	if (lpObjTarget->m_IfState.use > 0)
	{
		this->GCTradeResponseSend(2, lpObj, lpObjTarget->Name, 0, 0);
		return;
	}

	if (g_NewPVP.IsDuel(*lpObj) || g_NewPVP.IsDuel(*lpObjTarget))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 311), lpObj, 1);
		return;
	}

	if (g_NewPVP.IsObserver(*lpObj) || g_NewPVP.IsObserver(*lpObjTarget))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 312), lpObj, 1);
		return;
	}

	if (::gObjFixInventoryPointer(*lpObj) == false)
	{

	}

	if (lpObj->pTransaction == 1)
	{
		return;
	}

	lpObj->m_IfState.use = 1;
	lpObj->m_IfState.state = 0;
	lpObj->m_IfState.type = 1;
	lpObj->TargetNumber = number;
	lpObjTarget->m_IfState.use = 1;
	lpObjTarget->m_IfState.state = 0;
	lpObjTarget->m_IfState.type = 1;
	lpObjTarget->TargetNumber = lpObj->m_Index;
	lpObj->m_InterfaceTime = GetTickCount();
	lpObjTarget->m_InterfaceTime = GetTickCount();

	PMSG_TRADE_REQUESTSEND pMsg;

	pMsg.h.c = 0xC3;
	pMsg.h.headcode = 0x36;
	pMsg.h.size = sizeof(pMsg);
	memcpy(pMsg.szId, lpObj->Name, sizeof(pMsg.szId));

	GIOCP.DataSend(number, (LPBYTE)&pMsg, pMsg.h.size);

	sLog->outBasic("Request to trade [%s][%s][%s] to [%s][%s][%s]", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->ConnectUser->IP,
		lpObjTarget->m_PlayerData->ConnectUser->AccountID, lpObjTarget->Name, lpObjTarget->m_PlayerData->ConnectUser->IP);
}



void GameProtocol::CGTradeResponseRecv(PMSG_TRADE_RESPONSE * lpMsg, CGameObject* lpObj)
{
	BOOL Result = TRUE;
	int number = lpObj->TargetNumber;
	CGameObject* lpObjTarget = getGameObject(number);

	if (lpObj->m_IfState.use != FALSE)
	{
		if (lpObj->m_IfState.type == 1)
		{
			lpObj->m_IfState.use = FALSE;
			lpObj->TargetNumber = -1;
		}
		lpMsg->Response = false;
		GCTradeResponseSend(lpMsg->Response, lpObj, lpObj->Name, 0, 0);
		return;
	}

	if (gObjIsConnectedGP(*lpObj) == FALSE)
	{
		Result = FALSE;
		lpMsg->Response = false;
	}

	if (gObjIsConnectedGP(*lpObjTarget) == FALSE)
	{
		Result = FALSE;
		lpMsg->Response = false;
	}

	if (lpObj->m_PlayerData->ConnectUser->CloseType != -1)
	{
		Result = FALSE;
		lpMsg->Response = false;
	}

	if (lpObj->Type == OBJ_USER && lpObj->m_bPShopOpen == true)
	{
		Result = FALSE;
		lpMsg->Response = false;
	}

	if (lpObjTarget->Type == OBJ_USER && lpObjTarget->m_bPShopOpen == true)
	{
		Result = FALSE;
		lpMsg->Response = false;
	}

	if (lpObj->X < (lpObjTarget->X - 2) || lpObj->X >(lpObjTarget->X + 2) || lpObj->Y < (lpObjTarget->Y - 2) || lpObj->Y >(lpObjTarget->Y + 2))
	{
		Result = FALSE;
		lpMsg->Response = false;
	}

	if (lpObj->m_PlayerData->ISBOT == FALSE)
	{
		if (lpObj->m_IfState.use == FALSE || lpObj->m_IfState.type != 1 || lpObj->m_IfState.state != 0)
		{
			return;
		}
	}

	if (lpObjTarget->m_IfState.use == FALSE || lpObjTarget->m_IfState.type != 1 || lpObjTarget->m_IfState.state != 0)
	{
		return;
	}

	if (gObjFixInventoryPointer(*lpObj) == false)
	{

	}

	if (lpObj->pTransaction == 1 && lpObj->m_PlayerData->ISBOT == FALSE)
	{
		return;
	}

	if (lpMsg->Response == false)
	{
		GCTradeResponseSend(lpMsg->Response, lpObjTarget, lpObj->Name, 0, 0);
		Result = FALSE;
	}
	else
	{
		Result = TRUE;

		for (int i = 0; i < TRADE_BOX_SIZE; i++)
		{
			lpObj->pntTrade[i]->Clear();
			lpObjTarget->pntTrade[i]->Clear();
		}

		memset(lpObj->TradeMap, (BYTE)-1, TRADE_BOX_MAP_SIZE);
		memset(lpObjTarget->TradeMap, (BYTE)-1, TRADE_BOX_MAP_SIZE);
		lpObj->m_IfState.state = 1;
		lpObjTarget->m_IfState.state = 1;
		lpObj->TradeMoney = 0;
		lpObjTarget->TradeMoney = 0;

		if (::gObjInventoryTrans(*lpObj) == FALSE)
			Result = 2;

		if (::gObjInventoryTrans(*lpObjTarget) == FALSE)
			Result = 3;

		if (Result != TRUE)
		{
			lpMsg->Response = false;
			GCTradeResponseSend(lpMsg->Response, lpObjTarget, lpObj->Name, 0, 0);
			GCTradeResponseSend(lpMsg->Response, lpObj, lpObjTarget->Name, 0, 0);

			if (Result == 2)
			{

			}
			else if (Result == 3)
			{

			}

			Result = FALSE;
		}
		else
		{
			lpMsg->Response = true;
			GCTradeResponseSend(lpMsg->Response, lpObjTarget, lpObj->Name, lpObj->Level, lpObj->m_PlayerData->GuildNumber);
			GCTradeResponseSend(lpMsg->Response, lpObj, lpObjTarget->Name, lpObjTarget->Level, lpObjTarget->m_PlayerData->GuildNumber);

		}
	}

	if (Result == FALSE)
	{
		if (lpObj->m_IfState.use != FALSE && lpObj->m_IfState.type == 1)
		{
			lpObj->m_IfState.use = FALSE;
			lpObj->TargetNumber = -1;
		}

		if (lpObjTarget == lpObj)
		{
			if (lpObjTarget->m_IfState.use != FALSE && lpObjTarget->m_IfState.type == 1)
			{
				lpObjTarget->m_IfState.use = FALSE;
				lpObjTarget->TargetNumber = -1;
			}
		}
	}
}




bool GameProtocol::GCTradeResponseSend(BYTE response, CGameObject* lpObj, LPSTR id, WORD level, int GuildNumber)
{
	PMSG_TRADE_RESPONSE pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x37, sizeof(pMsg));
	pMsg.Response = response;
	pMsg.Level = level;
	pMsg.GuildNumber = GuildNumber;
	memcpy(pMsg.Id, id, sizeof(pMsg.Id));

	return GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
}

struct PMSG_TRADE_OTHER_DEL
{
	PBMSG_HEAD h;	// C1:38
	BYTE TradeItemIndex;	// 3
};

int GameProtocol::GCTradeOtherDel(CGameObject* lpObj, BYTE tradeindex)
{
	PMSG_TRADE_OTHER_DEL pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x38, sizeof(pMsg));
	pMsg.TradeItemIndex = tradeindex;

	return GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
}

struct PMSG_TRADE_OTHER_MOVE
{
	PBMSG_HEAD h;	// C1:39
	BYTE TradeItemIndex;	// 3
	BYTE ItemInfo[MAX_ITEM_INFO];	// 4
};


int GameProtocol::GCTradeOtherAdd(CGameObject* lpObj, BYTE tradeindex, LPBYTE iteminfo)
{
	PMSG_TRADE_OTHER_MOVE pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x39, sizeof(pMsg));
	pMsg.TradeItemIndex = tradeindex;
	pMsg.ItemInfo[I_TYPE] = iteminfo[I_TYPE];
	pMsg.ItemInfo[I_OPTION] = iteminfo[I_OPTION];
	pMsg.ItemInfo[I_DUR] = iteminfo[I_DUR];
	pMsg.ItemInfo[I_NOPTION] = iteminfo[I_NOPTION];
	pMsg.ItemInfo[I_SOPTION] = iteminfo[I_SOPTION];
	pMsg.ItemInfo[5] = iteminfo[5];
	pMsg.ItemInfo[6] = iteminfo[6];
	pMsg.ItemInfo[7] = iteminfo[7];
	pMsg.ItemInfo[8] = iteminfo[8];
	pMsg.ItemInfo[9] = iteminfo[9];
	pMsg.ItemInfo[10] = iteminfo[10];
	pMsg.ItemInfo[11] = iteminfo[11];

	return GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
}



void GameProtocol::CGTradeMoneyRecv(PMSG_TRADE_GOLD * lpMsg, CGameObject* lpObj)
{
	PMSG_DEFRESULT pMsg;
	int money = 0;
	int number;

	if (!PacketCheckTime(lpObj))
		return;

	if (lpObj->m_PlayerData->ConnectUser->CloseType != -1)
	{
		return;
	}

	number = lpObj->TargetNumber;
	CGameObject* lpObjTarget = getGameObject(number);

	if (lpObjTarget->m_PlayerData->ConnectUser->CloseType != -1)
	{
		return;
	}

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	if (lpObjTarget->Type != OBJ_USER)
	{
		return;
	}

	if (gObjIsConnectedGP(*lpObjTarget) == FALSE)
	{
		return;
	}

	if (lpObj->m_IfState.use == FALSE && lpObj->m_IfState.type != 1)
		return;

	if (gObjCanItemTouch(*lpObj, 1) == FALSE)
		return;

	if (lpMsg->Money > 1000000000)
		return;

	money = lpMsg->Money;

	if ((lpObj->m_PlayerData->Money - money) < 0)
		return;

	GCMoneySend(lpObj, lpObj->m_PlayerData->Money - money);

	PHeadSetB((LPBYTE)&pMsg, 0x3A, sizeof(pMsg));
	pMsg.result = 1;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);

	if (number >= 0)
	{
		lpObj->TradeMoney = money;
		lpObj->TradeOk = false;
		lpObjTarget->TradeOk = false;

		GCTradeOkButtonSend(lpObjTarget, 2);
		GCTradeOkButtonSend(lpObj, 0);
		GCTradeMoneyOther(lpObjTarget, lpObj->TradeMoney);
	}

	sLog->outBasic("[%s][%s][%s] -> [%s][%s][%s]: Trade Money %d", lpObj->m_PlayerData->ConnectUser->AccountID,
		lpObj->Name, lpObj->m_PlayerData->ConnectUser->Index, lpObjTarget->m_PlayerData->ConnectUser->AccountID,
		lpObjTarget->Name, lpObjTarget->m_PlayerData->ConnectUser->IP, money);
}

struct PMSG_TRADE_OTHER_GOLD
{
	PBMSG_HEAD h;	// C1:3B
	DWORD Money;	// 4
};


bool GameProtocol::GCTradeMoneyOther(CGameObject* lpObj, DWORD money)
{
	PMSG_TRADE_OTHER_GOLD pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x3B, sizeof(pMsg));
	pMsg.Money = money;

	return GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
}



bool GameProtocol::GCTradeOkButtonSend(CGameObject* lpObj, BYTE flag)
{
	PMSG_TRADE_OKBUTTON pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x3C, sizeof(pMsg));
	pMsg.Flag = flag;

	return GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
}


void GameProtocol::CGTradeOkButtonRecv(PMSG_TRADE_OKBUTTON * lpMsg, CGameObject* lpObj)
{
	int number = lpObj->TargetNumber;
	CGameObject* lpObjTarget = getGameObject(number);

	if (lpObjTarget->Type == OBJ_USER && lpObjTarget->m_PlayerData->ISBOT == 1)
	{
		if (lpMsg->Flag == 1)
		{
			g_BotSystem.AlchemistTradeOk(*lpObj, number);
			return;
		}
	}

	if (!gObjIsConnected(*lpObjTarget))
		return;

	if (lpObj->Type != OBJ_USER)
		return;

	if (lpObjTarget->Type != OBJ_USER)
		return;

	if (lpObj->m_IfState.use < 1 || lpObj->m_IfState.state != 1)
		return;

	if (!lpObjTarget->m_PlayerData->ISBOT)
	{
		if (lpObjTarget->m_IfState.use < 1 || lpObjTarget->m_IfState.state != 1)
			return;
	}
	if (lpObj->m_PlayerData->ConnectUser->CloseType != -1)
		return;

	if (lpObjTarget->m_PlayerData->ConnectUser->CloseType != -1)
		return;

	if (lpMsg->Flag == 1)
	{
		if (lpObj->TradeOk == false)
		{
			lpObj->TradeOk = true;
			GCTradeOkButtonSend(lpObjTarget, 1);
		}
	}
	else if (lpMsg->Flag == 0)
	{
		if (lpObj->TradeOk == true)
		{
			GCTradeOkButtonSend(lpObjTarget, 0);
			lpObj->TradeOk = false;
		}
	}

	gObjTradeOkButton(*lpObj);
	sLog->outBasic("[%s][%s][%s] -> [%s][%s][%s]: Trade Accept!", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->ConnectUser->IP,
		lpObjTarget->m_PlayerData->ConnectUser->AccountID, lpObjTarget->Name, lpObjTarget->m_PlayerData->ConnectUser->IP);
}


void GameProtocol::CGTradeCancelButtonRecv(CGameObject* lpObj)
{
	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	int number = lpObj->TargetNumber;
	CGameObject* lpObjTarget = getGameObject(number);

	gObjTradeCancel(*lpObj);
	CGTradeResult(lpObj, 0);

	if (number >= 0)
	{
		gObjTradeCancel(*lpObjTarget);
		CGTradeResult(lpObjTarget, 0);
	}

	sLog->outBasic("[%s][%s][%s]: Trade Cancel Button Press", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->ConnectUser->IP);
}


struct PMSG_TRADE_RESULT
{
	PBMSG_HEAD h;	// C1:3D
	BYTE Result;	// 3
};


void GameProtocol::CGTradeResult(CGameObject* lpObj, BYTE result)
{
	PMSG_TRADE_RESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x3D, sizeof(pMsg));
	pMsg.Result = result;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (unsigned char*)&pMsg, pMsg.h.size);
}

struct PMSG_PARTYREQUESTSEND
{
	PBMSG_HEAD h;	// C1:40
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
};


void GameProtocol::CGPartyRequestRecv(PMSG_PARTYREQUEST * lpMsg, CGameObject* lpObj)
{
	int number = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
	PMSG_PARTYREQUESTSEND pMsg;
	CGameObject* lpTargetObj = getGameObject(number);

	if (!PacketCheckTime(lpObj))
		return;

	if (lpObj->m_PlayerData->ConnectUser->CloseCount >= 0 || lpTargetObj->m_PlayerData->ConnectUser->CloseCount >= 0)
	{
		this->GCResultSend(lpObj, 0x41, 0x00);
		return;
	}

	if (lpTargetObj->m_PlayerData->m_bUsePartyMatching)
	{
		GCResultSend(lpObj, 0x41, 0x09);
		return;
	}

	if (lpTargetObj->Type != OBJ_USER)
	{
		this->GCSendDisableReconnect(lpObj);
		//GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
		return;
	}

	if (g_NewPVP.IsDuel(*lpObj) || g_NewPVP.IsDuel(*lpTargetObj))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 315), lpObj, 1);
		return;
	}

	if (g_NewPVP.IsObserver(*lpObj))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 316), lpObj, 1);
		return;
	}

	/*if (CC_MAP_RANGE(lpObj->MapNumber))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 116), lpObj, 1);
		return;
	}*/

	if (lpObj->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 573), lpObj, 1);
		return;
	}

	if (IT_MAP_RANGE(lpObj->MapNumber))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 291), lpObj, 1);
		return;
	}

	if (lpObj->m_IfState.use > 0)
	{
		this->GCResultSend(lpObj, 0x41, 0x00);
		return;
	}

	if (lpTargetObj->m_IfState.use > 0)
	{
		this->GCResultSend(lpObj, 0x41, 0x00);
		return;
	}

	if ((lpTargetObj->m_Option & 1) != TRUE)
	{
		this->GCResultSend(lpObj, 0x41, 0x01);
		return;
	}

	if (lpObj->PartyNumber >= 0)
	{
		int iUserIndex;
		int iDbNumber;
		gParty.GetIndexUser(lpObj->PartyNumber, 0, iUserIndex, iDbNumber);

		if (iUserIndex < 0 || (iUserIndex != lpObj->m_Index && lpObj->DBNumber != iDbNumber))
		{
			this->GCResultSend(lpObj, 0x41, 0x00);
			return;

		}
	}

	if (lpObj->PartyTargetUser >= 0)
		return;

	if (lpTargetObj->PartyNumber >= 0)
	{
		this->GCResultSend(lpObj, 0x41, 0x04);
		return;
	}

	if (lpTargetObj->PartyTargetUser >= 0)
	{
		this->GCResultSend(lpObj, 0x41, 0x00);
		return;
	}

	if (g_GensSystem.GetGensInfluence(*lpObj) != g_GensSystem.GetGensInfluence(*lpTargetObj) && g_GensSystem.CanGensJoinPartyWhileOppositeGens() == FALSE)
	{
		this->GCResultSend(lpObj, 0x41, 0x06);
		return;
	}

	if (g_GensSystem.IsMapBattleZone(lpObj->MapNumber) == TRUE && g_GensSystem.CanGensCreatePartyOnBattleZone() == FALSE)
	{
		this->GCResultSend(lpObj, 0x41, 0x07);
		return;
	}

	/*if (g_ArcaBattle.IsArcaBattleServer() && lpObj->m_PlayerData->GuildNumber != lpTargetObj->m_PlayerData->GuildNumber)
	{
		if (g_AcheronGuardianEvent.IsPlayStart() == false)
		{
			GCServerMsgStringSend(Lang.GetText(0, 522), lpObj, 1);
			return;
		}
	}*/

	if (lpObj->m_PlayerData->RegisterdLMS == 1)
	{
		MsgOutput(*lpObj, Lang.GetText(0, 523));
		return;
	}

	if (lpTargetObj->m_PlayerData->RegisterdLMS == 1)
	{
		MsgOutput(*lpObj, Lang.GetText(0, 524));
		return;
	}

	int maxlevel = 0;
	int minlevel = 0;

	if (lpTargetObj->PartyNumber >= 0)
	{

		if (gParty.Isleader(lpTargetObj->PartyNumber, *lpTargetObj, lpTargetObj->DBNumber) == FALSE)
		{
			char szTemp[256];
			wsprintf(szTemp, "%s is already in a party.", lpTargetObj->Name);
			GCServerMsgStringSend(szTemp, lpObj, 1);
			return;
		}

		if (gParty.GetLevel(lpTargetObj->PartyNumber, maxlevel, minlevel) == TRUE)
		{
			int limmaxlevel;
			int limmaxlevel2;

			if (maxlevel > lpTargetObj->Level)
				limmaxlevel = maxlevel;
			else
				limmaxlevel = lpTargetObj->Level;

			if (maxlevel < lpTargetObj->Level)
				limmaxlevel2 = maxlevel;
			else
				limmaxlevel2 = lpTargetObj->Level;

			if ((limmaxlevel - limmaxlevel2) > g_MaxStatsInfo.GetClass.Max_Party_Level_Dif)
			{
				this->GCResultSend(lpObj, 0x41, 0x05);
				// Restricted level for the party is %d
				return;
			}

			int limminlevel;
			int limminlevel2;

			if (maxlevel > lpTargetObj->Level)
				limminlevel = minlevel;
			else
				limminlevel = lpTargetObj->Level;

			if (maxlevel < lpTargetObj->Level)
				limminlevel2 = minlevel;
			else
				limminlevel2 = lpTargetObj->Level;

			if ((limminlevel - limminlevel2) > g_MaxStatsInfo.GetClass.Max_Party_Level_Dif)
			{
				this->GCResultSend(lpObj, 0x41, 0x05);
				// Restricted level for the party is %d
				return;
			}
		}
	}
	else
	{
		short sMaxMinLevel[2];

		if (lpObj->Level > lpTargetObj->Level)
			sMaxMinLevel[1] = lpObj->Level;
		else
			sMaxMinLevel[1] = lpTargetObj->Level;

		if (lpObj->Level < lpTargetObj->Level)
			sMaxMinLevel[0] = lpObj->Level;
		else
			sMaxMinLevel[0] = lpTargetObj->Level;

		if ((sMaxMinLevel[1] - sMaxMinLevel[0]) > g_MaxStatsInfo.GetClass.Max_Party_Level_Dif)
		{
			this->GCResultSend(lpObj, 0x41, 0x05);
			// Restricted level for the party is %d
			return;
		}
	}

	lpObj->m_IfState.use = TRUE;
	lpObj->m_IfState.type = 2;
	lpObj->m_IfState.state = 0;
	lpTargetObj->m_IfState.use = TRUE;
	lpTargetObj->m_IfState.type = 2;
	lpTargetObj->m_IfState.state = 0;
	lpObj->TargetNumber = number;
	lpTargetObj->TargetNumber = lpObj->m_Index;
	lpObj->m_InterfaceTime = GetTickCount();
	lpTargetObj->m_InterfaceTime = GetTickCount();
	lpObj->PartyTargetUser = number;

	PHeadSetB((LPBYTE)&pMsg, 0x40, sizeof(pMsg));
	pMsg.NumberH = SET_NUMBERH(lpObj);
	pMsg.NumberL = SET_NUMBERL(lpObj);

	GIOCP.DataSend(number, (LPBYTE)&pMsg, pMsg.h.size);
}



void GameProtocol::CGPartyRequestResultRecv(PMSG_PARTYREQUESTRESULT * lpMsg, CGameObject* lpObj)
{
	int number;
	BYTE result = false;
	char szTemp[256];
	int pnumber = -1;

	if (!PacketCheckTime(lpObj))
		return;

	number = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
	CGameObject* lpTargetObj = getGameObject(number);
	
	result = true;

	if (lpObj->m_PlayerData->m_bUsePartyMatching)
	{
		GCResultSend(lpObj, 0x41, 0x09);
		return;
	}

	if (lpTargetObj->MapNumber != lpObj->MapNumber)
	{
		result = false;
		GCResultSend(lpTargetObj, 0x41, 0x00);
	}

	if (lpObj->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 573), lpObj, 1);
		result = false;
		GCResultSend(lpTargetObj, 0x41, 0x00);
	}

	/*if (CC_MAP_RANGE(lpObj->MapNumber))
	{
		GCServerMsgStringSend(Lang.GetText(0, 116), lpObj, 1);
		result = false;
		this->GCResultSend(lpTargetObj, 0x41, 0x00);
	}*/

	if (lpMsg->Result == 0)
	{
		result = false;
		this->GCResultSend(lpTargetObj, 0x41, 0x01);
	}

	if (lpTargetObj->m_IfState.use == 0 || lpTargetObj->m_IfState.type != 2)
	{
		result = false;
		this->GCResultSend(lpTargetObj, 0x41, 0x00);
	}

	if (lpObj->m_IfState.use == 0 || lpObj->m_IfState.type != 2)
	{
		result = false;
		this->GCResultSend(lpTargetObj, 0x41, 0x00);
	}

	if (result == 1)
	{
		if (lpTargetObj->PartyNumber < 0)
		{
			lpTargetObj->PartyNumber = gParty.Create(*lpTargetObj, lpTargetObj->DBNumber, lpTargetObj->Level);
		}

		if (lpTargetObj->PartyNumber >= 0)
		{
			pnumber = lpTargetObj->PartyNumber;
			int iPartyPos = gParty.Add(lpTargetObj->PartyNumber, *lpObj, lpObj->DBNumber, lpObj->Level);

			if (iPartyPos >= 0)
			{
				lpObj->PartyNumber = lpTargetObj->PartyNumber;
				result = true;

				if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
				{
					wsprintf(szTemp, Lang.GetText(0, 20), lpObj->m_PlayerData->m_RealNameOfUBF);
					this->GCServerMsgStringSend(szTemp, lpTargetObj, 1);
					wsprintf(szTemp, Lang.GetText(0, 20), lpTargetObj->m_PlayerData->m_RealNameOfUBF);
					this->GCServerMsgStringSend(szTemp, lpObj, 1);
				}

				else
				{
					wsprintf(szTemp, Lang.GetText(0, 20), lpObj->Name);
					this->GCServerMsgStringSend(szTemp, lpTargetObj, 1);
					wsprintf(szTemp, Lang.GetText(0, 20), lpTargetObj->Name);
					this->GCServerMsgStringSend(szTemp, lpObj, 1);
				}

				gParty.Paint(pnumber);

				if (lpTargetObj->m_PlayerData->m_bUsePartyMatching == true)
				{
					char szLeaderName[MAX_ACCOUNT_LEN + 1];
					memcpy(szLeaderName, lpTargetObj->Name, MAX_ACCOUNT_LEN + 1);
					GDReqAcceptMemberJoin(lpTargetObj, 1, szLeaderName, lpObj->Name, 1);
				}
			}
			else if (iPartyPos == -1)
			{
				GCResultSend(lpTargetObj, 0x41, 2);
				GCResultSend(lpObj, 0x41, 2);
			}
			else if (iPartyPos == -2)
			{
				result = 2;
				wsprintf(szTemp, Lang.GetText(0, 21), lpObj->Name);
				GCServerMsgStringSend(szTemp, lpObj, 1);
			}
		}
	}

	if (lpObj->m_IfState.use != 0 && lpObj->m_IfState.type == 2)
	{
		lpObj->m_IfState.use = 0;
		lpObj->PartyTargetUser = -1;
	}

	if (lpTargetObj->m_IfState.use != 0 && lpTargetObj->m_IfState.type == 2)
	{
		lpTargetObj->m_IfState.use = 0;
		lpTargetObj->PartyTargetUser = -1;
	}

	if (pnumber >= 0)
	{
		CGPartyListAll(pnumber);
	}
}

void GameProtocol::CGPartyList(CGameObject* lpObj)
{
	PMSG_PARTYLISTCOUNT pCount;
	PMSG_PARTYLIST pList;
	BYTE sendbuf[512];
	int lOfs = 0;

	if (!this->PacketCheckTime(lpObj))
		return;

	PHeadSetB((LPBYTE)&pCount, 0x42, sizeof(pCount));
	pCount.Count = 0;
	pCount.Result = false;

	int number;
	int pnumber = lpObj->PartyNumber;

	if (pnumber >= 0)
		pCount.Count = gParty.GetCount(pnumber);

	lOfs += sizeof(pCount);

	if (pCount.Count != 0)
	{
		for (int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			number = gParty.m_PartyS[pnumber].Number[n];
			CGameObject* lpTargetObj = getGameObject(number);

			if (number >= 0)
			{
				if (gObjIsConnected(*lpTargetObj, gParty.m_PartyS[pnumber].DbNumber[n]) == TRUE)
				{
					memset(&pList, 0, sizeof(pList));
					memcpy(pList.szId, lpTargetObj->Name, MAX_ACCOUNT_LEN);
					pList.Number = n;
					pList.MapNumber = lpTargetObj->MapNumber;
					pList.X = lpTargetObj->X;
					pList.Y = lpTargetObj->Y;
					pList.Life = lpTargetObj->Life;
					pList.MaxLife = lpTargetObj->MaxLife + lpTargetObj->AddLife;
					pList.nServerChannel = lpTargetObj->m_PlayerData->m_nServerChannel + 1;
					pList.nMana = lpTargetObj->Mana;
					pList.nMaxMana = lpTargetObj->MaxMana;

					memcpy(&sendbuf[lOfs], &pList, sizeof(pList));
					lOfs += sizeof(pList);
				}
			}
		}

		pCount.Result = true;
	}

	pCount.h.size = lOfs;
	memcpy(sendbuf, &pCount, sizeof(pCount));

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&sendbuf, lOfs);
}



void GameProtocol::CGPartyListAll(int pnumber)
{
	PMSG_PARTYLISTCOUNT pCount;
	PMSG_PARTYLIST pList;
	BYTE sendbuf[512];
	int lOfs = 0;
	int number;

	if (pnumber < 0)
		return;

	if (gParty.IsParty(pnumber) == FALSE)
		return;

	PHeadSetB((LPBYTE)&pCount, 0x42, sizeof(pCount));
	pCount.Result = false;
	pCount.Count = gParty.GetCount(pnumber);

	if (pCount.Count == 0)
		return;

	pCount.Result = true;
	lOfs += sizeof(pCount);

	for (int n = 0; n < MAX_USER_IN_PARTY; n++)
	{
		number = gParty.m_PartyS[pnumber].Number[n];
		CGameObject* lpTargetObj = getGameObject(number);

		if (number >= 0)
		{
			if (gObjIsConnected(*lpTargetObj, gParty.m_PartyS[pnumber].DbNumber[n]) == TRUE)
			{
				memset(&pList, 0, sizeof(pList));

				if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
				{
					memcpy(pList.szId, lpTargetObj->m_PlayerData->m_RealNameOfUBF, MAX_ACCOUNT_LEN);
				}

				else
				{
					memcpy(pList.szId, lpTargetObj->Name, MAX_ACCOUNT_LEN);
				}

				pList.Number = n;
				pList.MapNumber = lpTargetObj->MapNumber;
				pList.X = lpTargetObj->X;
				pList.Y = lpTargetObj->Y;
				pList.Life = lpTargetObj->Life;
				pList.MaxLife = lpTargetObj->MaxLife;
				pList.nServerChannel = lpTargetObj->m_PlayerData->m_nServerChannel + 1;
				pList.nMana = lpTargetObj->Mana;
				pList.nMaxMana = lpTargetObj->MaxMana;

				memcpy(&sendbuf[lOfs], &pList, sizeof(pList));
				lOfs += sizeof(pList);
			}
		}
	}

	pCount.h.size = lOfs;
	memcpy(sendbuf, &pCount, sizeof(pCount));

	for (int n = 0; n < MAX_USER_IN_PARTY; n++)
	{
		number = gParty.m_PartyS[pnumber].Number[n];

		if (number >= 0)
		{
			GIOCP.DataSend(number, sendbuf, lOfs);
		}
	}

	for (int n = 0; n < MAX_USER_IN_PARTY; n++)
	{
		number = gParty.m_PartyS[pnumber].Number[n];

		if (number >= 0)
		{
			this->GCDisplayBuffeffectPartyMember(getGameObject(number));
		}
	}
}


void GameProtocol::CGPartyDelUser(PMSG_PARTYDELUSER * lpMsg, CGameObject* lpObj, int nType)
{
	int usernumber = -1;
	int dbnumber = -1;

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	if (lpObj->m_PlayerData->m_bUsePartyMatching == true)
	{
		char szTargetName[MAX_ACCOUNT_LEN + 1];
		memcpy(szTargetName, lpMsg->szTargetName, MAX_ACCOUNT_LEN + 1);

		if (nType == 0)
		{
			memcpy(szTargetName, lpObj->Name, MAX_ACCOUNT_LEN + 1);
		}

		if (nType == 1)
		{
			GDReqDeletePartyUser(lpObj, szTargetName);
		}

		if (lpObj->PartyNumber < 0)
		{
			return;
		}

		if (gParty.RevisionIndexUser(lpObj->PartyNumber, &lpMsg->Number, szTargetName) == FALSE)
		{
			return;
		}

	}

	if (lpObj->PartyNumber < 0)
	{
		return;
	}

	if (!gParty.GetIndexUser(lpObj->PartyNumber, lpMsg->Number, usernumber, dbnumber))
	{
		return;
	}

	if (usernumber < 0)
	{
		return;
	}

	int count;
	int number;
	int pnumber = lpObj->PartyNumber;

	if (pnumber < 0)
	{
		return;
	}

	count = gParty.GetCount(pnumber);

	if (count < 1)
	{
		return;
	}

	if (!DG_MAP_RANGE(getGameObject(usernumber)->MapNumber))
	{
		if (!lpMsg->Number)
		{
			BOOL bDPUser = FALSE;

			for (int n = 1; n < 5; n++)
			{
				CGameObject *lpObjParty = getGameObject(gParty.m_PartyS[pnumber].Number[n]);

				if (lpObjParty->m_Index != -1)
				{
					if (DG_MAP_RANGE(lpObjParty->MapNumber))
					{
						bDPUser = TRUE;
						break;
					}
				}
			}

			if (bDPUser == TRUE && count > 2)
			{
				gParty.Delete(pnumber, *getGameObject(pnumber), lpMsg->Number);
				count = gParty.GetCount(lpObj->PartyNumber);
				lpObj->PartyNumber = -1;
				lpObj->PartyTargetUser = -1;
				gParty.UpdatePKPartyPanalty(pnumber);
				gParty.ChangeLeader(pnumber);
				this->GCPartyDelUserSend(lpObj);
				this->CGPartyListAll(pnumber);
				return;
			}
		}

		if (!IMPERIAL_MAP_RANGE(lpObj->MapNumber))
		{
			if (!lpMsg->Number)
			{
				BOOL bIGUser = FALSE;

				for (int i = 1; i < 5; i++)
				{
					CGameObject* lpObjParty = getGameObject(gParty.m_PartyS[pnumber].Number[i]);
					if (IMPERIAL_MAP_RANGE(lpObjParty->MapNumber))
					{
						bIGUser = TRUE;
						break;
					}

				}

				if (bIGUser == TRUE && count > 2)
				{
					gParty.Delete(pnumber, *getGameObject(pnumber), lpMsg->Number);
					count = gParty.GetCount(lpObj->PartyNumber);
					lpObj->PartyNumber = -1;
					lpObj->PartyTargetUser = -1;
					gParty.UpdatePKPartyPanalty(pnumber);
					gParty.ChangeLeader(pnumber);
					this->GCPartyDelUserSend(lpObj);
					this->CGPartyListAll(pnumber);
					return;
				}
			}

			if (lpObj->m_PlayerData->lpGuild)
			{
				if (lpObj->m_PlayerData->lpGuild->WarState == 1 && lpObj->IsInBattleGround)
				{
					this->GCServerMsgStringSend(Lang.GetText(0, 574), lpObj, TRUE);
					return;
				}

				if (usernumber == gParty.m_PartyS[pnumber].Number[0])
				{
					CGameObject* lpObjParty = getGameObject(number);
					for (int j = 0; j < 5; j++)
					{
						int number = gParty.m_PartyS[pnumber].Number[j];
						if (number >= 0
							&& lpObjParty->m_PlayerData->lpGuild
							&& lpObjParty->m_PlayerData->lpGuild->WarState == TRUE
							&& lpObjParty->IsInBattleGround)
						{
							this->GCServerMsgStringSend(Lang.GetText(0, 574), lpObj, TRUE);
							return;
						}
					}
				}
			}

			if (lpMsg->Number && count > 2)
			{
				gParty.Delete(pnumber, *getGameObject(pnumber), lpMsg->Number);
				count = gParty.GetCount(lpObj->PartyNumber);
				lpObj->PartyNumber = -1;
				lpObj->PartyTargetUser = -1;
				gParty.UpdatePKPartyPanalty(pnumber);
				this->GCPartyDelUserSend(lpObj);
				CGPartyListAll(pnumber);
			}
			else
			{
				if (!IT_MAP_RANGE(lpObj->MapNumber) || lpMsg->Number || count <= 2)
				{
					if (IT_MAP_RANGE(lpObj->MapNumber) && count <= 2)
					{
						gParty.Delete(pnumber, *getGameObject(pnumber), lpMsg->Number);
						count = gParty.GetCount(lpObj->PartyNumber);
						lpObj->PartyNumber = -1;
						lpObj->PartyTargetUser = -1;
						gParty.UpdatePKPartyPanalty(pnumber);
						if (!lpMsg->Number)
							gParty.ChangeLeader(pnumber);
						this->GCPartyDelUserSend(lpObj);
						this->CGPartyListAll(pnumber);
					}
					else
					{
						if (!ITL_MAP_RANGE(lpObj->MapNumber) || lpMsg->Number || count <= 2)
						{
							if (ITL_MAP_RANGE(lpObj->MapNumber) && count <= 2)
							{
								gParty.Delete(pnumber, *getGameObject(lpMsg->Number));
								count = gParty.GetCount(lpObj->PartyNumber);
								lpObj->PartyNumber = -1;
								lpObj->PartyTargetUser = -1;
								gParty.UpdatePKPartyPanalty(pnumber);
								if (!lpMsg->Number)
									gParty.ChangeLeader(pnumber);
								this->GCPartyDelUserSend(lpObj);
								this->CGPartyListAll(pnumber);
							}
							else
							{
								for (int index = 0; index < 5; index++)
								{
									number = gParty.m_PartyS[pnumber].Number[index];
									CGameObject* lpObjParty = getGameObject(number);

									if (number >= 0)
									{
										gParty.Delete(pnumber, *getGameObject(pnumber), index);
										lpObjParty->PartyNumber = -1;
										lpObjParty->PartyTargetUser = -1;
										if (lpObjParty->m_PlayerData->m_bUsePartyMatching == true)
											this->GCPartyDelUserSendNoMessage(lpObjParty);
										else
											this->GCPartyDelUserSend(lpObjParty);
									}
								}

								gParty.Destroy(pnumber);
							}
						}
						else
						{
							gParty.Delete(pnumber, *getGameObject(pnumber), lpMsg->Number);
							count = gParty.GetCount(lpObj->PartyNumber);
							lpObj->PartyNumber = -1;
							lpObj->PartyTargetUser = -1;
							gParty.UpdatePKPartyPanalty(pnumber);
							gParty.ChangeLeader(pnumber);
							this->GCPartyDelUserSend(lpObj);
							this->CGPartyListAll(pnumber);
							if (!count)
								gParty.Destroy(pnumber);
						}
					}
				}
				else
				{
					gParty.Delete(number, *getGameObject(lpMsg->Number));
					count = gParty.GetCount(lpObj->PartyNumber);
					lpObj->PartyNumber = -1;
					lpObj->PartyTargetUser = -1;
					gParty.UpdatePKPartyPanalty(pnumber);
					gParty.ChangeLeader(pnumber);
					this->GCPartyDelUserSend(lpObj);
					this->CGPartyListAll(pnumber);
					if (!count)
						gParty.Destroy(pnumber);
				}
			}
		}
	}
}

struct PMSG_PARTYDELUSERSEND
{
	PBMSG_HEAD h;	// C1:43
	BYTE btSendMessage;
};


void GameProtocol::GCPartyDelUserSend(CGameObject* lpObj)
{
	PMSG_PARTYDELUSERSEND pMsg;

	pMsg.btSendMessage = FALSE;
	PHeadSetB((LPBYTE)&pMsg, 0x43, sizeof(pMsg));

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
}

void GameProtocol::GCPartyDelUserSendNoMessage(CGameObject* lpObj)
{
	PMSG_PARTYDELUSERSEND pMsg;

	pMsg.btSendMessage = TRUE;
	PHeadSetB((LPBYTE)&pMsg, 0x43, sizeof(pMsg));

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
}



struct PMSG_GUILDQUESTSEND
{
	PBMSG_HEAD h;	// C1:50
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
};

void GameProtocol::CGGuildRequestRecv(PMSG_GUILDJOINQ * lpMsg, CGameObject* lpObj)
{
	PMSG_GUILDQUESTSEND pMsg;
	int number = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
	CGameObject* lpTargetObj = getGameObject(number);

	if (!PacketCheckTime(lpObj))
		return;

	/*if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (g_CastleSiegeSync.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
		{
			this->GCServerMsgStringSend(Lang.GetText(0, 195), lpObj, 1);
			return;
		}
	}*/

	if (lpTargetObj->Type != OBJ_USER)
	{
		this->GCSendDisableReconnect(lpObj);
		//GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
		return;
	}

	if (lpObj->m_PlayerData->m_bSecurityCheck == false)
	{
		GCResultSend(lpObj, 0x51, 0x06);
		return;
	}

	if (lpObj->m_IfState.use > 0)
	{
		GCResultSend(lpObj, 0x51, 0x06);
		return;
	}

	if (lpTargetObj->m_IfState.use > 0)
	{
		GCResultSend(lpObj, 0x51, 0x06);
		return;
	}

	if (IT_MAP_RANGE(lpObj->MapNumber))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 292), lpObj, 1);
		return;
	}

	if (g_NewPVP.IsDuel(*lpObj) || g_NewPVP.IsDuel(*lpTargetObj))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 317), lpObj, 1);
		return;
	}

	if (g_NewPVP.IsObserver(*lpObj))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 318), lpObj, 1);
		return;
	}

	if (lpObj->Level < 6)
	{
		GCResultSend(lpObj, 0x51, 0x07);
		return;
	}

	if (lpObj->m_PlayerData->GuildNumber > 0)
	{
		GCResultSend(lpObj, 0x51, 0x05);
		return;
	}

	if ((lpTargetObj->m_Option & 1) != 1)
	{
		GCResultSend(lpObj, 0x51, 0x00);
		return;
	}

	if (lpTargetObj->m_PlayerData->GuildNumber > 0)
	{
		if (strcmp(lpTargetObj->m_PlayerData->lpGuild->Users[0]->Name, lpTargetObj->Name))
		{
			if (lpTargetObj->m_PlayerData->GuildStatus != 64)
			{
				GCResultSend(lpObj, 0x51, 0x04);
				return;
			}
		}

		if (lpTargetObj->m_PlayerData->lpGuild->WarState)
		{
			GCResultSend(lpObj, 0x51, 0x06);
			return;
		}

		int MaxGuildMember;

		if (lpTargetObj->Class == CLASS_DARKLORD)
			MaxGuildMember = (lpTargetObj->Level + lpTargetObj->m_PlayerData->MasterLevel) / 10 + lpTargetObj->Leadership / 10;
		else
			MaxGuildMember = (lpTargetObj->Level + lpTargetObj->m_PlayerData->MasterLevel) / 10;

		if (MaxGuildMember > g_ConfigRead.data.common.MaxGuildMember)
			MaxGuildMember = g_ConfigRead.data.common.MaxGuildMember;

		if (lpTargetObj->m_PlayerData->lpGuild->TotalCount >= MaxGuildMember)
		{
			GCResultSend(lpObj, 0x51, 0x02);
			return;
		}

		int OwnInfluence = g_GensSystem.GetGensInfluence(*lpObj);
		int TarInfluence = g_GensSystem.GetGensInfluence(*getGameObject(number));

		int iArcaBattleState = g_ArcaBattle.GetState();

		if (iArcaBattleState > 2 && iArcaBattleState < 9)
		{
			this->GCResultSend(lpObj, 0x51, 0xA4);
			return;
		}

		if (OwnInfluence <= NONE_INFLUENCE)
		{
			GCResultSend(lpObj, 0x51, 0xA3);
			return;
		}

		if (TarInfluence <= NONE_INFLUENCE)
		{
			GCResultSend(lpObj, 0x51, 0xA1);
			return;
		}

		if (OwnInfluence != TarInfluence && g_GensSystem.CanGensJoinGuildWhileOppositeGens() == FALSE)
		{
			GCResultSend(lpObj, 0x51, 0xA2);
			return;
		}
	}
	else
	{
		GCResultSend(lpObj, 0x51, 0x04);
		return;
	}

	lpObj->m_IfState.use = 1;
	lpObj->m_IfState.type = 4;
	lpObj->m_IfState.state = 0;

	lpTargetObj->m_IfState.use = 1;
	lpTargetObj->m_IfState.type = 4;
	lpTargetObj->m_IfState.state = 0;

	lpObj->TargetNumber = number;
	lpTargetObj->TargetNumber = lpObj->m_Index;
	lpObj->m_InterfaceTime = GetTickCount();
	lpTargetObj->m_InterfaceTime = GetTickCount();

	PHeadSetB((LPBYTE)&pMsg, 0x50, sizeof(pMsg));
	pMsg.NumberH = SET_NUMBERH(lpObj);
	pMsg.NumberL = SET_NUMBERL(lpObj);

	GIOCP.DataSend(number, (LPBYTE)&pMsg, pMsg.h.size);

}



void GameProtocol::CGGuildRequestResultRecv(PMSG_GUILDQRESULT * lpMsg, CGameObject* lpObj) //GS-CS Decompiled 100%
{
	int number;
	BYTE result = 0;
	int pnumber = -1;

	if (!PacketCheckTime(lpObj))
		return;

	/*if (g_CastleSiegeSync.GetCastleState() == 7) //Good
	{
		MsgOutput(*lpObj, (Lang.GetText(0, 195)));
		return;
	}*/

	number = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
	CGameObject* lpObjTarget = getGameObject(number);

	if (lpMsg->Result == 0)
	{
		result = 0;
		GCResultSend(lpObjTarget, 0x51, 0x00);
	}

	if (result == 1)
	{
		if (lpObj->m_PlayerData->GuildNumber > 0)
		{
			GDGuildMemberAdd(lpObjTarget, lpObj->m_PlayerData->lpGuild->Name, lpObjTarget->Name);
			GDReqCancelJoinGuildMatching(lpObjTarget, lpObj->Name, 1);
		}
	}

	if (lpObj->m_IfState.use && lpObj->m_IfState.type == 4)
		lpObj->m_IfState.use = 0;

	if (lpObjTarget->m_IfState.use && lpObjTarget->m_IfState.type == 4)
		lpObjTarget->m_IfState.use = 0;
}

struct PMSG_GUILDLIST
{
	char Name[10];	// 0
	BYTE Number;	// A
	BYTE ConnectAServer;	// B
	BYTE btGuildStatus;	// C
};

struct PMSG_GUILDLISTCOUNT_EX700
{
	PWMSG_HEAD h;	// C2:52
	BYTE Result;	// 4
	BYTE Count;	// 5
	int TotalScore;	// 8
	BYTE Score;	// C
	char szRivalGuild[5][8];	// D
};

struct PMSG_GUILDLISTCOUNT_S6
{
	PWMSG_HEAD h;	// C2:52
	BYTE Result;	// 4
	BYTE Count;	// 5
	int TotalScore;	// 8
	BYTE Score;	// C
	char szRivalGuild[8];
};

void GameProtocol::CGGuildListAll(int pnumber)
{
	CGameObject* lpObj = getGameObject(pnumber);

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	if (lpObj->m_PlayerData->lpGuild == NULL)
		return;

	PMSG_GUILDLISTCOUNT_EX700 pCount;
	PMSG_GUILDLIST pList;
	char sendbuf[1500];
	int lOfs = 0;
	int number;
	int n;
	int guildmaster = 0;

	PHeadSetW((LPBYTE)&pCount, 0x52, 0);
	pCount.Result = 0;
	pCount.Count = 0;
	lOfs += sizeof(pCount);
	pCount.h.sizeH = SET_NUMBERH(lOfs);
	pCount.h.sizeL = SET_NUMBERL(lOfs);

	if (lpObj->m_PlayerData->GuildNumber < 1)
	{
		GIOCP.DataSend(pnumber, (LPBYTE)&pCount, lOfs);
		return;
	}

	pCount.Count = lpObj->m_PlayerData->lpGuild->Count;
	pCount.TotalScore = lpObj->m_PlayerData->lpGuild->TotalScore;
	pCount.Score = lpObj->m_PlayerData->lpGuild->PlayScore;
	memset(pCount.szRivalGuild, 0, sizeof(pCount.szRivalGuild));

	if (lpObj->m_PlayerData->lpGuild->iGuildRival)
	{
		int iGuildCount = 0;
		int iGuildList[MAX_UNION_GUILD] = { 0 };

		if (UnionManager.GetGuildUnionMemberList(lpObj->m_PlayerData->lpGuild->iGuildRival, iGuildCount, iGuildList) == TRUE)
		{
			for (int i = 0; i < iGuildCount; i++)
			{
				GUILD_INFO_STRUCT *lpGuildInfo = Guild.SearchGuild_Number(iGuildList[i]);

				if (!lpGuildInfo)
					continue;

				memcpy(pCount.szRivalGuild[i], lpGuildInfo->Name, MAX_GUILD_LEN);
			}
		}

		else
		{
			memcpy(pCount.szRivalGuild[0], lpObj->m_PlayerData->lpGuild->szGuildRivalName, MAX_GUILD_LEN);
		}
	}

	pCount.Result = 1;
	guildmaster = 1;

	if (pCount.Count == 0)
		return;

	pCount.Count = 0;

	for (n = 0; n < MAX_USER_GUILD; n++)
	{
		number = lpObj->m_PlayerData->lpGuild->Use[n];
		CGameObject* lpGuildObj = getGameObject(number);

		if (number > 0)
		{
			memset(&pList, 0, sizeof(pList));
			memcpy(pList.Name, lpObj->m_PlayerData->lpGuild->Users[n]->Name, sizeof(pList.Name));
			pList.Number = lpObj->m_PlayerData->lpGuild->pServer[n];
			pList.ConnectAServer = (short)lpObj->m_PlayerData->lpGuild->pServer[n] & 0x7F;

			if (lpObj->m_PlayerData->lpGuild->pServer[n] >= 0)
				pList.ConnectAServer |= 0x80;

			pList.btGuildStatus = lpObj->m_PlayerData->lpGuild->GuildStatus[n];
			memcpy(&sendbuf[lOfs], &pList, sizeof(pList));
			lOfs += sizeof(pList);
			pCount.Count++;
		}
	}

	pCount.h.sizeH = SET_NUMBERH(lOfs);
	pCount.h.sizeL = SET_NUMBERL(lOfs);
	memcpy(sendbuf, (LPBYTE)&pCount, sizeof(pCount));

	GIOCP.DataSend(lpObj->m_Index, (LPBYTE)sendbuf, lOfs);
}

struct PMSG_GUILDDELUSER_RESULT
{
	PBMSG_HEAD h;	// C1:53
	BYTE Result;	// 3
};

void GameProtocol::CGGuildDelUser(PMSG_GUILDDELUSER * lpMsg, CGameObject* lpObj)
{
	if (!PacketCheckTime(lpObj))
		return;

	/*if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (g_CastleSiegeSync.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE) //Good
		{
			MsgOutput(*lpObj, Lang.GetText(0, 196));
			return;
		}
	}*/

	if (lpObj->m_PlayerData->GuildNumber < 1)
		return;

	char memberid[11];
	char guildname[11];
	char joomin[21];
	memset(joomin, 0, sizeof(joomin));
	PMSG_GUILDDELUSER_RESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x53, sizeof(pMsg));
	pMsg.Result = 3;

	if (!g_ConfigRead.data.common.GuildDestroy)
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	int iArcaBattleState = g_ArcaBattle.GetState();

	if (iArcaBattleState > 2 && iArcaBattleState < 9)
	{
		pMsg.Result = 6;

		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	if (lpObj->m_PlayerData->m_bSecurityCheck == false)
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	memset(memberid, 0, sizeof(memberid));
	memset(guildname, 0, sizeof(guildname));
	memcpy(memberid, lpMsg->Name, MAX_ACCOUNT_LEN);
	strcpy(guildname, lpObj->m_PlayerData->lpGuild->Name);

	if (!strcmp(lpObj->m_PlayerData->lpGuild->Users[0]->Name, lpObj->Name))
	{
		memset(joomin, 0, sizeof(joomin));
		memcpy(joomin, lpMsg->Password, sizeof(lpMsg->Password));

		if (gObjPasswordCheck(*lpObj, joomin) == FALSE)
		{
			pMsg.Result = 0;
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
			return;
		}

		if (!strcmp(memberid, lpObj->Name))
		{
			if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && g_ConfigRead.data.common.CastleGuildDestroyLimit == TRUE)
			{
				/*if (strcmp(lpObj->m_PlayerData->lpGuild->Name, g_CastleSiegeSync.GetCastleOwnerGuild()) == 0)
				{
					MsgOutput(*lpObj, Lang.GetText(0, 194));
					return;
				}*/
			}

			gObjGuildWarMasterClose(*lpObj);
			GDGuildDestroySend(lpObj, lpObj->m_PlayerData->lpGuild->Name, lpObj->Name);
			g_ArcaBattle.ReqRemoveRewardGuildBuff(lpObj->m_PlayerData->lpGuild->Name);
			g_ArcaBattle.GDReqMarkRegDel(*lpObj, lpObj->m_PlayerData->GuildNumber);
		}
		else
		{
			if (Guild.GetGuildMemberStatus(lpObj->m_PlayerData->lpGuild->Name, memberid) == -1)
			{
				pMsg.Result = 0;
				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
				return;
			}

			g_ArcaBattle.ReqGuildMemberDelBuffDel(memberid, guildname);
			GDGuildMemberDel(lpObj, guildname, memberid);
		}

	}
	else
	{
		if (strcmp(memberid, lpObj->Name))
		{
			pMsg.Result = 0;
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
			return;
		}

		memset(joomin, 0, sizeof(joomin));
		memcpy(joomin, lpMsg->Password, sizeof(lpMsg->Password));

		if (gObjPasswordCheck(*lpObj, joomin) == FALSE)
		{
			MsgOutput(*lpObj, Lang.GetText(0, 525));
		}

		else
		{
			g_ArcaBattle.ReqGuildMemberDelBuffDel(*lpObj, guildname);
			GDGuildMemberDel(lpObj, guildname, memberid);
		}

	}
}


void GameProtocol::GCGuildDelUserResult(CGameObject* lpObj, BYTE Result)
{
	PMSG_GUILDDELUSER_RESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x53, sizeof(pMsg));
	pMsg.Result = Result;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
}



struct PMSG_GUILDMASTERQUESTION
{
	PBMSG_HEAD h;	// C1:54
};

void GameProtocol::GCGuildMasterQuestionSend(CGameObject* lpObj)
{
	if (lpObj->m_IfState.use > 0)
	{
		return;
	}

	PMSG_GUILDMASTERQUESTION pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x54, sizeof(pMsg));
	lpObj->m_IfState.use = 1;
	lpObj->m_IfState.type = 5;
	lpObj->m_IfState.state = 0;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}



void GameProtocol::CGGuildMasterAnswerRecv(PMSG_GUILDMASTERANSWER * lpMsg, CGameObject* lpObj)
{
	if (lpMsg->Result == 1)
	{
		if (!gObjGuildMasterCapacityTest(*lpObj))
		{
			if (lpObj->m_IfState.use && lpObj->m_IfState.type == 5)
			{
				lpObj->m_IfState.use = 0;
			}

			return;
		}

		GCGuildMasterManagerRun(lpObj);
		return;
	}

	if (lpObj->m_IfState.use && lpObj->m_IfState.type == 5)
	{
		lpObj->m_IfState.use = 0;
	}
}


struct PMSG_GUILDMASTERMANAGER_RUN
{
	PBMSG_HEAD h;	// C1:55
};

void GameProtocol::GCGuildMasterManagerRun(CGameObject* lpObj)
{
	PMSG_GUILDMASTERMANAGER_RUN pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x55, sizeof(pMsg));

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
}



void GameProtocol::CGGuildMasterInfoSave(CGameObject* lpObj, PMSG_GUILDINFOSAVE * lpMsg)
{
	char GuildName[MAX_GUILD_LEN + 1];
	GuildName[MAX_GUILD_LEN] = '\0';
	memcpy(GuildName, lpMsg->GuildName, MAX_GUILD_LEN);
	int len = strlen(GuildName);

	if (len <= 2 || len > MAX_GUILD_LEN)
	{
		PMSG_GUILDCREATED_RESULT pMsg;

		PHeadSetB((LPBYTE)&pMsg, 0x56, sizeof(pMsg));
		pMsg.Result = 2;

		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);

		if (lpObj->m_IfState.use && lpObj->m_IfState.type == 5)
		{
			lpObj->m_IfState.use = 0;
		}

		return;
	}

	if (!g_prohibitedSymbols.Validate(GuildName, len, TYPE_GUILDNAME))
	{
		MsgOutput(*lpObj, Lang.GetText(0, 546));
		return;
	}

	GDReqCancelJoinGuildMatching(lpObj, lpObj->Name, 1);
	GDGuildCreateSend(lpObj, GuildName, lpObj->Name, lpMsg->Mark, lpMsg->btGuildType);

	if (lpObj->m_IfState.use && lpObj->m_IfState.type == 5)
	{
		lpObj->m_IfState.use = 0;
	}
}



void GameProtocol::CGGuildMasterCreateCancel(CGameObject* lpObj)
{
	if (lpObj->Type != OBJ_USER)
		return;

	if (lpObj->m_PlayerData->GuildNumber > 0)
		return;

	if (lpObj->m_IfState.use && lpObj->m_IfState.type == 5)
	{
		lpObj->m_IfState.use = 0;
	}
}


void GameProtocol::GCGuildViewportNowPaint(CGameObject* lpObj, char* guildname, BYTE* mark, BOOL isGuildMaster)
{
	GUILD_INFO_STRUCT * lpGuild = Guild.SearchGuild(guildname);

	if (lpGuild == NULL)
		return;

	BYTE _GuildInfoBuf[256] = { 0 };
	int _GuildInfoOfs = 5;

	if (lpObj->m_PlayerData->lpGuild == NULL)
		return;

	PMSG_SIMPLE_GUILDVIEWPORT pMsg;
	pMsg.GuildNumber = lpObj->m_PlayerData->lpGuild->Number;
	pMsg.NumberH = SET_NUMBERH(lpObj) & 0x7F;
	pMsg.NumberL = SET_NUMBERL(lpObj);

	if (isGuildMaster != FALSE)
		pMsg.NumberH |= 0x80;

	if (isGuildMaster != FALSE)
		pMsg.btGuildStatus = 0x80;
	else
		pMsg.btGuildStatus = lpObj->m_PlayerData->GuildStatus;

	pMsg.btGuildType = lpObj->m_PlayerData->lpGuild->btGuildType;
	pMsg.btGuildRelationShip = 0;
	lpObj->m_PlayerData->iGuildUnionTimeStamp = 0;
	memcpy(&_GuildInfoBuf[_GuildInfoOfs], &pMsg, sizeof(pMsg));
	_GuildInfoOfs += sizeof(pMsg);

	PMSG_SIMPLE_GUILDVIEWPORT_COUNT pGVCount;

	pGVCount.h.c = 0xC2;
	pGVCount.h.headcode = 0x65;
	pGVCount.h.sizeH = SET_NUMBERH(_GuildInfoOfs);
	pGVCount.h.sizeL = SET_NUMBERL(_GuildInfoOfs);
	pGVCount.Count = 1;
	memcpy(_GuildInfoBuf, &pGVCount, sizeof(pGVCount));

	//if (!CC_MAP_RANGE(lpObj->MapNumber) && lpObj->MapNumber != MAP_INDEX_CHAOSCASTLE_SURVIVAL) // temp
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, _GuildInfoBuf, _GuildInfoOfs);

	MsgSendV2(lpObj, _GuildInfoBuf, _GuildInfoOfs);
}



struct PMSG_GUILDDEL_VIEWPORT_NOW
{
	PBMSG_HEAD h;	// C1:5D
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
};



void GameProtocol::GCGuildViewportDelNow(CGameObject* lpObj, BOOL isGuildMaster)
{
	PMSG_GUILDDEL_VIEWPORT_NOW pMsg;


	PHeadSetB((LPBYTE)&pMsg, 0x5D, sizeof(pMsg));
	pMsg.NumberH = SET_NUMBERH(lpObj) & 0x7F;
	pMsg.NumberL = SET_NUMBERL(lpObj);

	if (isGuildMaster != FALSE)
	{
		pMsg.NumberH |= 0x80;
	}

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
	MsgSendV2(lpObj, (LPBYTE)&pMsg, pMsg.h.size);
}



void GameProtocol::GCManagerGuildWarEnd(char * GuildName)
{
	GUILD_INFO_STRUCT * lpNode = Guild.SearchGuild(GuildName);

	if (lpNode == NULL)
	{
		return;
	}

	int warmaster = lpNode->Users[0]->m_Index;

	if (warmaster >= 1)
	{
		BYTE Result1 = 0;
		BYTE Result2 = 0;

		if (lpNode != NULL)
		{
			if (lpNode->lpTargetGuildNode != NULL)
			{
				if (lpNode->PlayScore == 0 && lpNode->lpTargetGuildNode->PlayScore == 0)
				{
					Result1 = 6;
					Result2 = 6;
				}
				else
				{
					Result1 = 0;
					Result2 = 1;
				}
			}
		}

		/*if (lpNode->WarType == 1)
		{
			gBattleGroundEnable(lpNode->BattleGroundIndex, FALSE);
			gObjGuildWarEndSend(*lpNode, *lpNode->lpTargetGuildNode, Result1, Result2);
			gObjGuildWarEnd(*lpNode, *lpNode->lpTargetGuildNode);

			cManager.BattleInfoSend(::GetBattleTeamName(0, 0), -1, ::GetBattleTeamName(0, 1), -1);
		}
		else*/
		{
			gObjGuildWarEndSend(*lpNode, *lpNode->lpTargetGuildNode, Result1, Result2);
			gObjGuildWarEnd(*lpNode, *lpNode->lpTargetGuildNode);
		}
	}
}



void GameProtocol::GCManagerGuildWarSet(LPSTR GuildName1, LPSTR GuildName2, int type)
{
	GUILD_INFO_STRUCT * lpNode = Guild.SearchGuild(GuildName1);

	if (lpNode == NULL)
	{
		return;
	}

	this->GCGuildWarRequestResult(GuildName2, lpNode->Users[0], type);
}


struct PMSG_GUILDWARREQUEST_RESULT
{
	PBMSG_HEAD h;	// C1:60
	BYTE Result;	// 3
};

void GameProtocol::GCGuildWarRequestResult(LPSTR GuildName, CGameObject* lpObj, int type)
{
	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		return;
	}

	if (lpObj->m_IfState.use > 0 && lpObj->m_IfState.type != 12)
	{
		return;
	}

	PMSG_GUILDWARREQUEST_RESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x60, sizeof(pMsg));
	pMsg.Result = 3;

	if (lpObj->m_PlayerData->GuildNumber < 1)
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	GUILD_INFO_STRUCT * lpMyGuild = lpObj->m_PlayerData->lpGuild;

	if (!lpMyGuild)
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	if (lpMyGuild->WarState == 1 || lpMyGuild->WarDeclareState == 1)
	{
		pMsg.Result = 4;

		//Error on declaring war : %d %d %s
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);

		return;
	}

	if (g_ConfigRead.pk.bPkPenaltyDisable == FALSE && lpObj->m_PK_Level >= 6)
	{
		pMsg.Result = 4;

		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);

		return;
	}

	if (strcmp(lpMyGuild->Users[0]->Name, lpObj->Name))
	{
		pMsg.Result = 5;

		// Error on declaring war : 0x05 %s %s
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);

		return;
	}

	char _guildname[MAX_GUILD_LEN + 1];
	memset(_guildname, 0, sizeof(_guildname));
	memcpy(_guildname, GuildName, MAX_GUILD_LEN);

	if (!strncmp(lpMyGuild->Name, GuildName, MAX_GUILD_LEN))
		return;

	GUILD_INFO_STRUCT * lpNode = Guild.SearchGuild(_guildname);

	if (lpNode)
	{
		if (lpNode->WarState == 1 || lpNode->WarDeclareState == 1)
		{
			pMsg.Result = 4;

			//Error on declaring war : %d %d %s
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);

			return;
		}

		if (lpMyGuild->iGuildUnion != 0 && lpMyGuild->iGuildUnion == lpNode->iGuildUnion)
		{
			return;
		}


		if (g_ConfigRead.pk.bPkPenaltyDisable == FALSE && lpNode->Users[0] != nullptr && lpNode->Users[0]->m_PK_Level >= 6)
		{
			pMsg.Result = 4;

			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);

			return;
		}

		if (lpNode->Users[0] != nullptr)
		{
			int warmaster = lpNode->Users[0]->m_Index;
			if (warmaster >= 1)
			{
				/*if (CC_MAP_RANGE(lpObj->MapNumber) || CC_MAP_RANGE(lpNode->Users[0]->MapNumber))
				{
					GCServerMsgStringSend(Lang.GetText(0, 117), lpObj, 1);
					return;
				}*/

				if (lpObj->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL || lpNode->Users[0]->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
				{
					GCServerMsgStringSend(Lang.GetText(0, 575), lpObj, 1);
					return;
				}

				if ((lpNode->Users[0]->m_Option & 1) != 1)
				{
					pMsg.Result = 4;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);

					return;
				}

				pMsg.Result = 1;
				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
				GCGuildWarRequestSend(lpMyGuild->Name, lpNode->Users[0], type);
				lpMyGuild->WarDeclareState = 1;
				lpNode->WarDeclareState = 1;
				lpMyGuild->WarType = type;
				lpNode->WarType = type;

				// Declared guild war [%s][%s] sGuild:(%s) TargetGuild:(%s)
				strcpy(lpMyGuild->TargetGuildName, lpNode->Name);
				strcpy(lpNode->TargetGuildName, lpMyGuild->Name);
				lpMyGuild->lpTargetGuildNode = lpNode;
				lpNode->lpTargetGuildNode = lpMyGuild;
			}
			else
			{
				pMsg.Result = 2;
				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);

				//return;
			}
		}
	}
	else
	{
		pMsg.Result = 0;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);

		return;
	}
}


struct PMSG_GUILDWARSEND
{
	PBMSG_HEAD h;	// C1:61
	char GuildName[8];	// 3
	BYTE Type;	// B
};

void GameProtocol::GCGuildWarRequestSend(LPSTR GuildName, CGameObject* lpObj, int type) //GS-CS OK
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		PMSG_GUILDWARSEND pMsg;

		PHeadSetB((LPBYTE)&pMsg, 0x61, sizeof(pMsg));
		pMsg.Type = type;
		memcpy(pMsg.GuildName, GuildName, MAX_GUILD_LEN);

		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);

	}
}


struct PMSG_GUILDWAR_DECLARE
{
	PBMSG_HEAD h;	// C1:62
	char GuildName[8];	// 3
	BYTE Type;	// B
	BYTE TeamCode;	// C
};


void GameProtocol::GCGuildWarRequestSendRecv(PMSG_GUILDWARSEND_RESULT * lpMsg, CGameObject* lpObj)
{
	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		return;
	}

	PMSG_GUILDWAR_DECLARE pMsg;
	int count = 0;
	int g_call = 0;

	PHeadSetB((LPBYTE)&pMsg, 0x62, sizeof(pMsg));
	pMsg.Type = 0;

	GUILD_INFO_STRUCT * lpMyNode = lpObj->m_PlayerData->lpGuild;

	if (!lpMyNode)
	{
		PMSG_GUILDWARREQUEST_RESULT pResult;

		pResult.h.c = 0xC1;
		pResult.h.headcode = 0x60;
		pResult.h.size = sizeof(pResult);	// #error Change fro pResult
		pResult.Result = 0;

		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (lpMyNode->lpTargetGuildNode && lpMsg->Result)
	{
		int iTargetIndex = lpMyNode->lpTargetGuildNode->Index[0];
		CGameObject* lpTargetObj = getGameObject(iTargetIndex);
		if (//BC_MAP_RANGE(lpTargetObj->MapNumber) ||
			//CC_MAP_RANGE(lpTargetObj->MapNumber) ||
			//DS_MAP_RANGE(lpTargetObj->MapNumber) ||
			DG_MAP_RANGE(lpTargetObj->MapNumber) ||
			IT_MAP_RANGE(lpTargetObj->MapNumber) ||
			IMPERIAL_MAP_RANGE(lpTargetObj->MapNumber))
		{
			PMSG_NOTICE pNotice;

			TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 117));

			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pNotice, pNotice.h.size);

			lpMsg->Result = 0;
		}

		if (lpTargetObj->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
		{
			PMSG_NOTICE pNotice;

			TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 575));

			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pNotice, pNotice.h.size);

			lpMsg->Result = 0;
		}
	}

	pMsg.Type = lpMyNode->WarType;

	if (lpMsg->Result == 0)
	{
		if (lpMyNode->lpTargetGuildNode)
		{
			if (lpMyNode->lpTargetGuildNode->WarDeclareState == 1)
			{
				lpMyNode->lpTargetGuildNode->WarDeclareState = 0;
				lpMyNode->WarDeclareState = 0;

				PMSG_GUILDWARREQUEST_RESULT pResult;

				PHeadSetB((LPBYTE)&pResult, 0x60, sizeof(pResult));
				pResult.Result = 6;

				if (lpMyNode->lpTargetGuildNode->Index[0] >= 0)
				{
					GIOCP.DataSend(lpMyNode->lpTargetGuildNode->Index[0], (LPBYTE)&pResult, pResult.h.size);
				}
			}
		}
	}
	else //if ( lpMsg->Result == 1 )
	{
		if (lpMyNode->WarDeclareState == 1)
		{
			if (lpMyNode->lpTargetGuildNode)
			{
				if (lpMyNode->lpTargetGuildNode->WarDeclareState == 1)
				{
					PMSG_TELEPORT pTeleportMsg;

					/*if (lpMyNode->WarType == 1)
					{
						lpMyNode->BattleGroundIndex = gCheckBlankBattleGround();

						switch (lpMyNode->BattleGroundIndex)
						{
						case 0xFF:
							lpMyNode->WarDeclareState = 0;
							lpMyNode->WarState = 0;
							lpMyNode->lpTargetGuildNode->WarDeclareState = 0;
							lpMyNode->lpTargetGuildNode->WarState = 0;

							PMSG_GUILDWARREQUEST_RESULT pResult;

							PHeadSetB((LPBYTE)&pResult, 0x60, sizeof(pResult));
							pResult.Result = 4;

							GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
							return;
						}

						pTeleportMsg.MoveNumber = 51;
						lpMyNode->lpTargetGuildNode->BattleGroundIndex = lpMyNode->BattleGroundIndex;

						if (gBSGround[0]->m_BallIndex >= 0)
							gObjMonsterRegen(*getGameObject(gBSGround[0]->m_BallIndex));

						BattleSoccerGoalEnd(0);
						lpMyNode->PlayScore = 0;
						lpMyNode->lpTargetGuildNode->PlayScore = 0;
						gBattleGroundEnable(lpMyNode->BattleGroundIndex, TRUE);
						gSetBattleTeamMaster(lpMyNode->BattleGroundIndex, 0, lpMyNode->Name, lpMyNode);
						gSetBattleTeamMaster(lpMyNode->BattleGroundIndex, 1, lpMyNode->lpTargetGuildNode->Name, lpMyNode->lpTargetGuildNode);
					}*/

					lpMyNode->WarDeclareState = 2;
					lpMyNode->WarState = 1;
					lpMyNode->lpTargetGuildNode->WarDeclareState = 2;
					lpMyNode->lpTargetGuildNode->WarState = 1;
					lpMyNode->PlayScore = 0;
					lpMyNode->lpTargetGuildNode->PlayScore = 0;
					lpMyNode->BattleTeamCode = 0;
					lpMyNode->lpTargetGuildNode->BattleTeamCode = 1;
					memset(pMsg.GuildName, 0, sizeof(pMsg.GuildName));
					memcpy(pMsg.GuildName, lpMyNode->lpTargetGuildNode->Name, sizeof(pMsg.GuildName));
					pMsg.TeamCode = lpMyNode->BattleTeamCode;
					count = 0;

					CGameObject* lpObjMaster = getGameObject(lpMyNode->Index[0]);
					
					for (int n = 0; n < MAX_USER_GUILD; n++)
					{
						CGameObject* lpObjPartyUser = getGameObject(lpMyNode->Index[n]);
						if (lpMyNode->Use[n])
						{
							if (lpMyNode->Index[n] >= 0)
							{
								g_call = 0;

								if (n > 0)
								{
									if (lpMyNode->WarType == 1)
									{
										if (lpObjMaster->PartyNumber >= 0)
										{
											if (lpObjMaster->PartyNumber == lpObjPartyUser->PartyNumber)
											{
												g_call = 1;
											}
										}
									}
									else
									{
										g_call = 1;
									}
								}
								else if (n == 0)
								{
									g_call = 1;
								}

								if (g_call)
								{
									GIOCP.DataSend(lpObjPartyUser->m_Index, (LPBYTE)&pMsg, pMsg.h.size);

									// Declared guild war against (%s)
									GCGuildWarScore(lpObjPartyUser);
									int x = 60;

									if (lpMyNode->WarType == 1)
									{
										lpObjPartyUser->IsInBattleGround = true;
										pTeleportMsg.MoveNumber = 51;

										if (g_ConfigRead.pk.bPkPenaltyDisable || lpObjPartyUser->m_PK_Level < 6)
										{
											gObjTeleport(*lpObjPartyUser, 6, x++, 153);
											count++;
										}
									}
								}
							}
						}
					}

					std::memset(pMsg.GuildName, 0, sizeof(pMsg.GuildName));
					std::memcpy(pMsg.GuildName, lpMyNode->Name, sizeof(pMsg.GuildName));

					pMsg.TeamCode = lpMyNode->lpTargetGuildNode->BattleTeamCode;
					count = 0;

					CGameObject* lpObjTargetWar = getGameObject(lpMyNode->lpTargetGuildNode->Index[0]);
					for (int n = 0; n < MAX_USER_GUILD; n++)
					{
						CGameObject* lpObjTargetUser = getGameObject(lpMyNode->lpTargetGuildNode->Index[n]);
						if (lpMyNode->lpTargetGuildNode->Use[n])
						{
							if (lpMyNode->lpTargetGuildNode->Index[n] >= 0)
							{
								g_call = 0;

								if (n > 0)
								{
									if (lpMyNode->WarType == 1)
									{
										if (lpObjTargetWar->PartyNumber >= 0)
										{
											if (lpObjTargetWar->PartyNumber == lpObjTargetUser->PartyNumber)
											{
												g_call = 1;
											}
										}
									}
									else
									{
										g_call = 1;
									}
								}
								else if (n == 0)
								{
									g_call = 1;
								}

								if (g_call)
								{
									GIOCP.DataSend(lpMyNode->lpTargetGuildNode->Index[n], (LPBYTE)&pMsg, pMsg.h.size);

									// Declared guild war against (%s)
									GCGuildWarScore(lpMyNode->lpTargetGuildNode->Users[n]);
									int x = 59;

									if (lpMyNode->lpTargetGuildNode->WarType == 1)
									{

										if (g_ConfigRead.pk.bPkPenaltyDisable != 0 || lpMyNode->lpTargetGuildNode->Users[n]->m_PK_Level < 6)
										{
											lpMyNode->lpTargetGuildNode->Users[n]->IsInBattleGround = true;
											gObjTeleport(*lpMyNode->lpTargetGuildNode->Users[n], 6, x++, 164);
											count++;
										}
									}
								}
							}
						}
					}

					/*cManager.BattleInfoSend(GetBattleTeamName(0, 0),
						GetBattleTeamScore(0, 0),
						GetBattleTeamName(0, 1),
						GetBattleTeamScore(0, 1));*/

					if (lpMyNode->WarType == 1)
					{
						gObjAddMsgSendDelay(*lpObj, 5, *lpObj, 10000, 0);
						GCServerMsgStringSendGuild(lpMyNode->lpTargetGuildNode, Lang.GetText(0, 62), 1);
						GCServerMsgStringSendGuild(lpMyNode, Lang.GetText(0, 62), 1);
					}
				}
			}
		}
	}
}



void GameProtocol::GCGuildWarDeclare(CGameObject* lpObj, LPSTR _guildname)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		PMSG_GUILDWAR_DECLARE pMsg;

		PHeadSetB((LPBYTE)&pMsg, 0x62, sizeof(pMsg));
		memcpy(pMsg.GuildName, _guildname, sizeof(pMsg.GuildName));

		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
	}
}


struct PMSG_GUILDWAR_END
{
	PBMSG_HEAD h;	// C1:63
	BYTE Result;	// 3
	char GuildName[8];	// 4
};

void GameProtocol::GCGuildWarEnd(CGameObject* lpObj, BYTE result, char* _guildname)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		PMSG_GUILDWAR_END pMsg;

		PHeadSetB((LPBYTE)&pMsg, 0x63, sizeof(pMsg));
		pMsg.Result = result;
		memcpy(pMsg.GuildName, _guildname, sizeof(pMsg.GuildName));

		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
	}
}


struct PMSG_GUILDSCORE
{
	PBMSG_HEAD h;	// C1:64
	BYTE Score1;	// 3
	BYTE Score2;	// 4
	BYTE Type;	// 5
};


void GameProtocol::GCGuildWarScore(CGameObject* lpObj)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		if (lpObj->Type != OBJ_USER)
			return;

		if (lpObj->m_PlayerData->GuildNumber < 1)
			return;

		PMSG_GUILDSCORE pMsg;

		PHeadSetB((LPBYTE)&pMsg, 0x64, sizeof(pMsg));
		pMsg.Score1 = 0;
		pMsg.Score2 = 0;
		pMsg.Type = 0;

		if (lpObj->m_PlayerData->lpGuild != NULL)
		{
			pMsg.Score1 = lpObj->m_PlayerData->lpGuild->PlayScore;
		}

		if (lpObj->m_PlayerData->lpGuild->lpTargetGuildNode != NULL)
		{
			pMsg.Score2 = lpObj->m_PlayerData->lpGuild->lpTargetGuildNode->PlayScore;
		}

		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
	}
}




void GameProtocol::CGWarehouseMoneyInOut(CGameObject* lpObj, PMSG_WAREHOUSEMONEYINOUT* lpMsg)
{
	int money = 0;

	money = lpMsg->Money;

	if (lpObj->m_IfState.state == 0)
	{
		GCWarehouseInventoryMoneySend(lpObj, 0, 0, 0);
		return;
	}

	if (lpObj->m_IfState.use < 1 || lpObj->m_IfState.type != 6)
	{
		GCWarehouseInventoryMoneySend(lpObj, 0, 0, 0);

		return;
	}

	switch (lpMsg->Type)
	{
	case 0x00:
		if (money <= 0 || money > MAX_WAREHOUSE_ZEN)
		{
			GCWarehouseInventoryMoneySend(lpObj, 0, 0, 0);
			return;
		}

		if ((lpObj->WarehouseMoney + money) > MAX_WAREHOUSE_ZEN)
		{
			GCWarehouseInventoryMoneySend(lpObj, 0, 0, 0);
			return;
		}

		if (money > lpObj->m_PlayerData->Money)
		{
			GCWarehouseInventoryMoneySend(lpObj, 0, 0, 0);
			return;
		}

		lpObj->m_PlayerData->Money -= money;
		lpObj->WarehouseMoney += money;
		break;
	case 0x01:
	{
		if (bCanWarehouseLock == TRUE)
		{
			if (lpObj->WarehouseLock == 1)
			{
				GCWarehouseInventoryMoneySend(lpObj, 0, 0, 0);
				GCServerMsgStringSend(Lang.GetText(0, 155), lpObj, 1);
				return;
			}
		}

		if (money <= 0 || money > MAX_WAREHOUSE_ZEN)
		{
			GCWarehouseInventoryMoneySend(lpObj, 0, 0, 0);
			return;
		}

		if (money > lpObj->WarehouseMoney)
		{
			GCWarehouseInventoryMoneySend(lpObj, 0, 0, 0);
			return;
		}

		lpObj->WarehouseMoney -= money;
		lpObj->m_PlayerData->Money += money;
		int rZen = GetWarehouseUsedHowMuch(lpObj->Level, lpObj->WarehousePW);

		if ((lpObj->m_PlayerData->Money - rZen) > 0)
		{
			int oldmoney = lpObj->m_PlayerData->Money;
			lpObj->m_PlayerData->Money -= rZen;
			GCMoneySend(lpObj, lpObj->m_PlayerData->Money);

		}
		else if ((lpObj->WarehouseMoney - rZen) > 0)
		{
			int oldmoney = lpObj->WarehouseMoney;
			lpObj->WarehouseMoney -= rZen;

			GCWarehouseInventoryMoneySend(lpObj, 1, lpObj->m_PlayerData->Money, lpObj->WarehouseMoney);
		}
		else
		{
			lpObj->WarehouseMoney += money;
			lpObj->m_PlayerData->Money -= money;
			GCWarehouseInventoryMoneySend(lpObj, 0, 0, 0);
			char msg[255];

			wsprintf(msg, Lang.GetText(0, 154), rZen);
			GCServerMsgStringSend(msg, lpObj, 1);
			return;
		}
	}
	break;

	default:
		GCWarehouseInventoryMoneySend(lpObj, 0, 0, 0);
		return;
	}

	lpObj->WarehouseCount++;
	GCWarehouseInventoryMoneySend(lpObj, 1, lpObj->m_PlayerData->Money, lpObj->WarehouseMoney);
}

void GameProtocol::GCWarehouseInventoryMoneySend(CGameObject* lpObj, BYTE result, int money, int wmoney)
{
	PMSG_MONEY pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x81, sizeof(pMsg));
	pMsg.Result = result;
	pMsg.iMoney = money;
	pMsg.wMoney = wmoney;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
}



void GameProtocol::CGWarehouseUseEnd(CGameObject* lpObj)
{
	PMSG_DEFAULT pMsg;
	PHeadSetB((LPBYTE)&pMsg, 0x82, sizeof(pMsg));
	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
	bool bCheck = false;
	Check_SameSerialItem(*lpObj, 2, bCheck);

	if (lpObj->m_btOpenWarehouse == FALSE)
	{
		sLog->outBasic("[%s][%s] Attempt Multi-Ware dupe", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
		return;
	}

	gObjItemTextSave(*lpObj);
	GDSetWarehouseList(lpObj, TRUE);
	GDUserItemSave(lpObj);
	g_PentagramSystem.DBREQ_SetPentagramJewel(*lpObj, 0);
	g_PentagramSystem.DBREQ_SetPentagramJewel(*lpObj, 1);
}



struct PMSG_WAREHOUSESTATE
{
	PBMSG_HEAD h;	// C1:83
	BYTE State;	// 3
};


void GameProtocol::GCWarehouseStateSend(CGameObject* lpObj, BYTE state)
{
	PMSG_WAREHOUSESTATE pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x83, sizeof(pMsg));
	pMsg.State = state;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
}



void GameProtocol::GCWarehouseRecivePassword(CGameObject* lpObj, PMSG_WAREHOUSEPASSSEND* lpMsg)
{
	if (bCanWarehouseLock == FALSE)
		return;

	int pw = lpMsg->Pass;

	switch (lpMsg->Type)
	{
	case 0x00:
		if (lpObj->WarehouseLock)
		{
			if (lpObj->WarehousePW == pw)
			{
				lpObj->WarehouseLock = FALSE;
				GCWarehouseStateSend(lpObj, 0x0C);
			}
			else
			{
				GCWarehouseStateSend(lpObj, 0x0A);
			}
		}
		break;
	case 0x01:
		if (!lpObj->WarehouseLock || lpObj->WarehouseLock == 0xFF)
		{
			char szJoomin[21];

			memset(szJoomin, 0, sizeof(szJoomin));
			memcpy(szJoomin, lpMsg->LastJoominNumber, sizeof(lpMsg->LastJoominNumber));

			if (gObjPasswordCheck(*lpObj, szJoomin) == FALSE)
			{
				GCWarehouseStateSend(lpObj, 0x0D);
				return;
			}

			lpObj->WarehousePW = pw;
			lpObj->WarehouseLock = FALSE;
			GCWarehouseStateSend(lpObj, 0x0C);
		}
		else
		{
			GCWarehouseStateSend(lpObj, 0x0B);
		}
		break;
	case 0x02:
	{
		char szJoomin[21];

		memset(szJoomin, 0, sizeof(szJoomin));
		memcpy(szJoomin, lpMsg->LastJoominNumber, sizeof(lpMsg->LastJoominNumber));

		pw = atoi(szJoomin);

		if (gObjPasswordCheck(*lpObj, szJoomin) == FALSE)
		{
			GCWarehouseStateSend(lpObj, 0x0D);
			return;
		}

		lpObj->WarehouseLock = FALSE;
		lpObj->WarehousePW = 0;
		GCWarehouseStateSend(lpObj, lpObj->WarehouseLock);
	}
	break;
	}
}



void GameProtocol::GCUserChaosBoxSend(CGameObject* lpObj, int iChaosBoxType)
{
	if (iChaosBoxType == 1)
	{
		if (lpObj->m_IfState.type != 13)
		{
			return;
		}

		if (lpObj->m_IfState.type == 13 && lpObj->m_IfState.state == 1)
		{
			return;
		}
	}
	else
	{
		if (lpObj->m_IfState.type != 7)
		{
			return;
		}

		if (lpObj->m_IfState.type == 7 && lpObj->m_IfState.state == 1)
		{
			return;
		}
	}

	PMSG_SHOPITEMCOUNT pMsg;
	BYTE SendByte[2048];
	int lOfs = sizeof(pMsg);
	PHeadSetW((LPBYTE)&pMsg, 0x31, 0);
	pMsg.count = 0;

	if (iChaosBoxType == 1)
	{
		pMsg.Type = 5;
	}
	else
	{
		pMsg.Type = 3;
	}

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (lpObj->pntChaosBox[n]->IsItem() == TRUE)
		{
			SendByte[lOfs] = n;
			lOfs++;
			ItemByteConvert(&SendByte[lOfs], *lpObj->pntChaosBox[n]);
			lOfs += MAX_ITEM_INFO;
			pMsg.count++;
		}
	}

	pMsg.h.sizeH = SET_NUMBERH(lOfs);
	pMsg.h.sizeL = SET_NUMBERL(lOfs);
	memcpy(SendByte, &pMsg, sizeof(pMsg));

	GIOCP.DataSend(lpObj->m_Index, SendByte, lOfs);
}



void GameProtocol::CGChaosBoxItemMixButtonClick(PMSG_CHAOSMIX* aRecv, CGameObject* lpObj)
{

	if (lpObj->ChaosLock == TRUE)
	{
		GCServerMsgStringSend(Lang.GetText(0, 11), lpObj, 1);
		return;
	}

	if (lpObj->m_PlayerData->m_bSecurityCheck == false)
	{
		return;
	}

	if (lpObj->m_bPShopOpen == true)
	{
		GCServerMsgStringSend(Lang.GetText(0, 112), lpObj, 1);
		return;
	}

	//season4 add-on start
	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;
	if (aRecv->Type == 3 ||
		aRecv->Type == 4 ||
		aRecv->Type == 22 ||
		aRecv->Type == 23)
	{
		int pos = 0;
		int validitemcount = 0;

		for (int n = 0; n < CHAOS_BOX_SIZE; n++) //loc8
		{
			if (lpObj->pntChaosBox[n]->IsItem() == TRUE)
			{
				if (lpObj->pntChaosBox[n]->m_Level == 9)
				{
					pos = n;
					validitemcount++;
				}
				else if (lpObj->pntChaosBox[n]->m_Level == 10)
				{
					pos = n;
					validitemcount++;
				}
				else if (lpObj->pntChaosBox[n]->m_Level == 11)
				{
					pos = n;
					validitemcount++;
				}
				else if (lpObj->pntChaosBox[n]->m_Level == 12)
				{
					pos = n;
					validitemcount++;
				}
				else if (lpObj->pntChaosBox[n]->m_Level == 13) //new
				{
					pos = n;
					validitemcount++;
				}
				else if (lpObj->pntChaosBox[n]->m_Level == 14)
				{
					pos = n;
					validitemcount++;
				}
			}
		}

		if (validitemcount != 1)
		{
			pMsg.Result = 0;
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
			return;
		}

		int iItemWidth = 0;
		int iItemHeight = 0;

		lpObj->pntChaosBox[pos]->GetSize(iItemWidth, iItemHeight);

		if (!CheckInventoryEmptySpace(*lpObj, iItemHeight, iItemWidth))
		{
			pMsg.Result = 0;
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
			return;
		}

		if (g_LuckyItemManager.IsLuckyItemEquipment(lpObj->pntChaosBox[pos]->m_Type) == TRUE)
		{
			pMsg.Result = 7;
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
			return;
		}
	}
	//season4 add-on end

	lpObj->bIsChaosMixCompleted = true;
	int iMixType = aRecv->Type;

	//if (iMixType >= CHAOS_TYPE_BLOODANGEL_ITEM_MIX_START && iMixType <= CHAOS_TYPE_BLOODANGEL_ITEM_MIX_END)
	//{
	//	g_MixSystem.BloodAngelUnSealMix(lpObj, iMixType);
	//g_MixSystem.AncientSoulItemMix(lpObj);
	//}
	//else
	if (iMixType >= 0xC8 && iMixType <= 0xCF)
		g_MixSystem.DarkAngelWeaponMix(*lpObj, iMixType);
	else
		if (iMixType >= 0x88 && iMixType <= 0xA7)
			g_MixSystem.DarkAngelMix(*lpObj, iMixType);
		else
			if (iMixType >= 0xA8 && iMixType <= 0xC7)
				g_MixSystem.DarkAngelNextMix(*lpObj, iMixType);
			else
			{

				switch (iMixType)	// Season X Main ENG
				{
				case CHAOS_TYPE_DEVILSQUARE:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.DevilSquareItemChaosMix(*lpObj);
					}

					else
					{
						g_MixSystem.DevilSquareItemChaosMix_Multi(*lpObj, aRecv->MixCount);
					}
					break;
				case 0x86:
					g_MixSystem.ArchangelHammerMix(*lpObj);
					break;
				case 0x87:
					g_MixSystem.BlessedAAMix(*lpObj);
					break;
				case 0x40:
					g_MixSystem.DarkAngelWeaponMix(*lpObj, iMixType);
					break;
				case CHAOS_TYPE_UPGRADE_10:
					g_MixSystem.PlusItemLevelChaosMix(*lpObj, 3);
					break;
				case CHAOS_TYPE_UPGRADE_11:
					g_MixSystem.PlusItemLevelChaosMix(*lpObj, 4);
					break;
				case CHAOS_TYPE_UPGRADE_12:
					g_MixSystem.PlusItemLevelChaosMix(*lpObj, 22);
					break;
				case CHAOS_TYPE_UPGRADE_13:
					g_MixSystem.PlusItemLevelChaosMix(*lpObj, 23);
					break;
				case CHAOS_TYPE_UPGRADE_14:
					g_MixSystem.PlusItemLevelChaosMix(*lpObj, 49);
					break;
				case CHAOS_TYPE_UPGRADE_15:
					g_MixSystem.PlusItemLevelChaosMix(*lpObj, 50);
					break;
				case CHAOS_TYPE_DINORANT:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.PegasiaChaosMix(*lpObj);
					}

					else
					{
						g_MixSystem.PegasiaChaosMix_Multi(*lpObj, aRecv->MixCount);
					}
					break;
				case CHAOS_TYPE_FRUIT:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.CircleChaosMix(*lpObj);
					}

					else
					{
						g_MixSystem.CircleChaosMix_Multi(*lpObj, aRecv->MixCount);
					}
					break;
				case CHAOS_TYPE_SECOND_WING:
				case CHAOS_TYPE_CLOAK:
					g_MixSystem.WingChaosMix(*lpObj);
					break;
				case CHAOS_TYPE_BLOODCATLE:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.BloodCastleMix(*lpObj);
					}
					else
					{
						g_MixSystem.BloodCastleItemChaosMix_Multi(*lpObj, aRecv->MixCount);
					}
					break;
				case CHAOS_TYPE_DEFAULT:
				case CHAOS_TYPE_FIRST_WING:
					g_MixSystem.DefaultChaosMix(*lpObj);
					break;
				case CHAOS_TYPE_SETITEM:	// #warning mix domyslnie wylaczony w s2 i wyzej
					g_MixSystem.SetItemChaosMix(*lpObj);
					break;
				case CHAOS_TYPE_DARKHORSE:
					g_MixSystem.DarkHorseChaosMix(*lpObj);
					break;
				case CHAOS_TYPE_DARKSPIRIT:
					g_MixSystem.DarkSpiritChaosMix(*lpObj);
					break;
				case CHAOS_TYPE_BLESS_POTION:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.BlessPotionChaosMix(*lpObj);
					}

					else
					{
						g_MixSystem.BlessPotionChaosMix_Multi(*lpObj, aRecv->MixCount);
					}
					break;
				case CHAOS_TYPE_SOUL_POTION:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.SoulPotionChaosMix(*lpObj);
					}

					else
					{
						g_MixSystem.SoulPotionChaosMix_Multi(*lpObj, aRecv->MixCount);
					}
					break;

				case CHAOS_TYPE_LIFE_STONE:
					g_MixSystem.LifeStoneChaosMix(*lpObj);
					break;
				case CHAOS_TYPE_CASTLE_SPECIAL_ITEM_MIX:
					g_MixSystem.CastleSpecialItemMix(*lpObj);
					break;
				case CHAOS_TYPE_HT_BOX:
					g_MixSystem.HiddenTreasureBoxItemMix(*lpObj);
					break;
				case CHAOS_TYPE_FENRIR_01:
					g_MixSystem.Fenrir_01Level_Mix(*lpObj);
					break;
				case CHAOS_TYPE_FENRIR_02:
					g_MixSystem.Fenrir_02Level_Mix(*lpObj);
					break;
				case CHAOS_TYPE_FENRIR_03:
					g_MixSystem.Fenrir_03Level_Mix(*lpObj);
					break;
				case CHAOS_TYPE_FENRIR_04:
					g_MixSystem.Fenrir_04Upgrade_Mix(*lpObj);
					break;
				case CHAOS_TYPE_COMPOUNDPOTION_LV1:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.ShieldPotionLv1_Mix(*lpObj);
					}

					else
					{
						g_MixSystem.ShieldPotionLv1_Mix_Multi(*lpObj, aRecv->MixCount);
					}
					break;
				case CHAOS_TYPE_COMPOUNTPOTION_LV2:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.ShieldPotionLv2_Mix(*lpObj);
					}

					else
					{
						g_MixSystem.ShieldPotionLv2_Mix_Multi(*lpObj, aRecv->MixCount);
					}
					break;
				case CHAOS_TYPE_COMPOUNTPOTION_LV3:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.ShieldPotionLv3_Mix(*lpObj);
					}

					else
					{
						g_MixSystem.ShieldPotionLv3_Mix_Multi(*lpObj, aRecv->MixCount);
					}
					break;
				case CHAOS_TYPE_JEWELOFHARMONY_PURITY:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.JewelOfHarmonyItemPurityMix(*lpObj);
					}
					else
					{
						g_MixSystem.PurityJewelOfHarmony_MultiMix(*lpObj, aRecv->MixCount);
					}
					break;
				case CHAOS_TYPE_JEWELOFHARMONY_MIX_SMELTINGITEM:
					if (aRecv->MixCount == 1)
					{
						g_kJewelOfHarmonySystem.MakeSmeltingStoneItem(*lpObj);
					}
					else
					{
						g_kJewelOfHarmonySystem.MakeSmeltingStoneItem_MultiMix(*lpObj, aRecv->MixCount);
					}
					break;
				case CHAOS_TYPE_JEWELOFHARMONY_RESTORE_ITEM:
					g_kJewelOfHarmonySystem.RestoreStrengthenItem(*lpObj);
					break;
				case CHAOS_TYPE_380_OPTIONITEM:
					g_kItemSystemFor380.ChaosMix380ItemOption(*lpObj);
					break;
				case CHAOS_TYPE_LOTTERY_MIX:
					g_MixSystem.LotteryItemMix(*lpObj, 0);
					break;
				case CHAOS_TYPE_OLD_PAPER:
					g_MixSystem.IllusionTempleItemChaosMix(*lpObj);
					break;
				case CHAOS_TYPE_CONDOR_FEATHER:
					g_MixSystem.FeatherOfCondorMix(*lpObj);
					break;
				case CHAOS_TYPE_THIRD_WING:
					g_MixSystem.ThirdWingLevel2ChaosMix(*lpObj);
					break;
				case CHAOS_TYPE_BLOSSOM_MIX:
					g_MixSystem.CherryBlossomMix(*lpObj);
					break;
				case CHAOS_TYPE_SEED_EXTRACT:
					g_MixSystem.SeedExtractMix(*lpObj);
					break;
				case CHAOS_TYPE_SEED_SPHERE_COMPOSITE:
					g_MixSystem.SeedSphereCompositeMix(*lpObj);
					break;
				case CHAOS_TYPE_SET_SEED_SPHERE:
					g_MixSystem.SetSeedSphereMix(*lpObj, aRecv->Pos);
					break;
				case CHAOS_TYPE_SEED_SPHERE_REMOVE:
					g_MixSystem.SeedSphereRemoveMix(*lpObj, aRecv->Pos);
					break;
				case CHAOS_TYPE_SECROMICON:
					g_MixSystem.SecromiconMix(*lpObj);
					break;
				case CHAOS_TYPE_PREMIUM_BOX:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.PremiumBoxMix(*lpObj);
					}

					else
					{
						g_MixSystem.PremiumBoxMix_Multi(*lpObj, aRecv->MixCount);
					}
					break;
				case CHAOS_TYPE_SUMMONS_MIX:
					g_MixSystem.SummonsMix(*lpObj, 0);
					break;
				case CHAOS_TYPE_LUCKYITEM_EXCHANGE:
					g_LuckyItemManager.LuckyItemTicketExchange(*lpObj);
					break;
				case CHAOS_TYPE_LUCKYITEM_REFINE:
					g_LuckyItemManager.LuckyItemSmelting(*lpObj);
					break;
				case CHAOS_TYPE_SOCKETWEAPON:
					g_MixSystem.ItemRefineMix(*lpObj);
					break;
				case CHAOS_TYPE_ARCABATTLE_BOOTYMIX:
					g_ArcaBattle.BootyExchange(*lpObj);
					break;
				case CHAOS_TYPE_ARCABATTLE_SIGNOFLORD:
					g_ArcaBattle.CGReqMarkRegButtonClick(*lpObj);
					break;
				case CHAOS_TYPE_MONSTERWING:
					g_MixSystem.MonsterWingMix(*lpObj);
					break;
				case CHAOS_TYPE_CCF_REWARD:
					g_MixSystem.CCFRewardMix(*lpObj);
					break;
				case CHAOS_TYPE_DSF_REWARD:
					g_MixSystem.DSFRewardMix(*lpObj);
					break;

				case CHAOS_TYPE_ANCIENT_SOUL_ITEM:
					g_MixSystem.AncientSoulItemMix(*lpObj);
					break;
				case CHAOS_TYPE_ADVANCED_WING:
					g_MixSystem.AdvancedWingMix(*lpObj);
					break;
				case 78:
					g_MixSystem.bloodangelmix78(*lpObj);
					break;
				case 79:
					g_MixSystem.bloodangelmix79(*lpObj);
					break;
				case 80:
					g_MixSystem.bloodangelmix80(*lpObj);
					break;
				case 81:
					g_MixSystem.bloodangelmix81(*lpObj);
					break;
				case 86:
					g_MixSystem.bloodangelmix86(*lpObj);
					break;
				case 87:
					g_MixSystem.bloodangelmix87(*lpObj);
					break;
				case 88:
					g_MixSystem.bloodangelmix88(*lpObj);
					break;
				case 89:
					g_MixSystem.bloodangelmix89(*lpObj);
					break;
				case 90:
					g_MixSystem.bloodangelmix90(*lpObj);
					break;
				case 91:
					g_MixSystem.bloodangelmix91(*lpObj);
					break;
				case 92:
					g_MixSystem.bloodangelmix92(*lpObj);
					break;
				case 93:
					g_MixSystem.bloodangelmix93(*lpObj);
					break;
				case 98:
					g_MixSystem.bloodangelmix98(*lpObj);
					break;
				case 99:
					g_MixSystem.bloodangelmix99(*lpObj);
					break;
				case 100:
					g_MixSystem.bloodangelmix100(*lpObj);
					break;
				case 101:
					g_MixSystem.bloodangelmix101(*lpObj);
					break;
				case 106:
					g_MixSystem.bloodangelmix106(*lpObj);
					break;
				case 107:
					g_MixSystem.bloodangelmix107(*lpObj);
					break;
				case 108:
					g_MixSystem.bloodangelmix108(*lpObj);
					break;
				case 109:
					g_MixSystem.bloodangelmix109(*lpObj);
					break;
				case 114:
					g_MixSystem.bloodangelmix114(*lpObj);
					break;
				case 115:
					g_MixSystem.bloodangelmix115(*lpObj);
					break;
				case 116:
					g_MixSystem.bloodangelmix116(*lpObj);
					break;
				case 117:
					g_MixSystem.bloodangelmix117(*lpObj);
					break;
				case 118:
					g_MixSystem.bloodangelmix118(*lpObj);
					break;
				case 119:
					g_MixSystem.bloodangelmix119(*lpObj);
					break;
				case 120:
					g_MixSystem.bloodangelmix120(*lpObj);
					break;
				case 121:
					g_MixSystem.bloodangelmix121(*lpObj);
					break;
				case 126:
					g_MixSystem.bloodangelmix126(*lpObj);
					break;
				case 127:
					g_MixSystem.bloodangelmix127(*lpObj);
					break;
				case 128:
					g_MixSystem.bloodangelmix128(*lpObj);
					break;
				case 129:
					g_MixSystem.bloodangelmix129(*lpObj);
					break;
					//se 0x8F..0xA9: //Armor DA 
					//	g_MixSystem.DarkAngelMix(lpObj, iMixType);
					//	break;
				default:
					sLog->outBasic("[%s][%s] Undefine chaosmix type detect %d", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, iMixType);
					break;
				}

			}
}



void GameProtocol::CGChaosBoxUseEnd(CGameObject* lpObj)
{

	PMSG_DEFAULT pMsg;
	PHeadSetB((LPBYTE)&pMsg, 0x87, sizeof(pMsg));
	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);

	if (lpObj->m_IfState.use && lpObj->m_IfState.type == 7)
	{
		lpObj->m_IfState.use = 0;
		lpObj->m_IfState.state = 0;
	}

	g_MixSystem.ChaosBoxInit(*lpObj);
	g_PentagramMixSystem.PentagramMixBoxInit(lpObj);
	gObjInventoryCommit(*lpObj);
	lpObj->m_bIsCastleNPCUpgradeCompleted = false;
}





void GameProtocol::PMoveProc(PMSG_MOVE* lpMove, CGameObject* lpObj)
{

	if (lpObj->m_bOffLevel == true)
	{
		return;
	}

	PMSG_RECVMOVE pMove;
	short n;
	short pathtable;
	short ax;
	short ay;
	int sx;
	int sy;
	int tick = GetTickCount();

	if (lpObj->m_PlayerData->ConnectUser->Connected != PLAYER_PLAYING)
	{
		return; // for reconnect incase player is using MuBot
	}
	if (MAX_MAP_RANGE(lpObj->MapNumber) == FALSE)
	{
		return;
	}

	if (lpObj->Type == OBJ_USER && (g_ConfigRead.MovingHackCheck == TRUE || g_ConfigRead.MovingSpeedCheck == TRUE))
	{
		short ix = abs(lpObj->X - lpMove->X);
		short iy = abs(lpObj->Y - lpMove->Y);

		if (g_ConfigRead.MovingSpeedCheck == TRUE)
		{
			lpObj->m_PlayerData->MovingDistance += (ix + iy);
		}

		if (g_ConfigRead.MovingHackCheck == TRUE)
		{
			if (ix > g_ConfigRead.MovingHackDistance || iy > g_ConfigRead.MovingHackDistance)
			{
				gObjSetPosition(*lpObj, lpObj->X, lpObj->Y);
				return;
			}
		}
	}

	if (lpObj->RegenOk > 0)
		return;

	if (lpObj->m_PlayerData->m_bMapSvrMoveReq == true)
		return;

	if (lpObj->m_PlayerData->m_bMapSvrMoveReq_2 == true)
		return;

	if (lpObj->m_SkillInfo->LordSummonTime)
	{
		lpObj->m_SkillInfo->LordSummonTime = 0;
		char msg[255];
		wsprintf(msg, Lang.GetText(0, 146));
		GCServerMsgStringSend(msg, lpObj, 1);
	}

	if ((GetTickCount() - lpObj->m_LastMoveTime) < 100)
		return;

	if (lpObj->Teleport)
		return;

	if (gObjCheckUsedBuffEffect(*lpObj, BUFFTYPE_STONE) || gObjCheckUsedBuffEffect(*lpObj, BUFFTYPE_STUN) || gObjCheckUsedBuffEffect(*lpObj, BUFFTYPE_SLEEP)
		|| gObjCheckUsedBuffEffect(*lpObj, BUFFTYPE_FREEZE_2) || gObjCheckUsedBuffEffect(*lpObj, BUFFTYPE_EARTH_BINDS))
	{
		gObjSetPosition(*lpObj, lpObj->X, lpObj->Y);
		return;
	}

	if (lpObj->Type == OBJ_USER && gObjCheckMoveArea(*lpObj, lpMove->X, lpMove->Y) == false)
	{
		gObjSetPosition(*lpObj, lpObj->X, lpObj->Y);

		PMSG_TELEPORT_RESULT pTeleportResult;

		pTeleportResult.h.c = 0xC3;
		pTeleportResult.h.size = sizeof(pTeleportResult);
		pTeleportResult.h.headcode = 0x1C;
		pTeleportResult.Type = 0;
		pTeleportResult.Unk = 0;
		pTeleportResult.MapNumber = lpObj->MapNumber;
		pTeleportResult.Unk1 = 0;
		pTeleportResult.MapX = lpObj->X;
		pTeleportResult.MapY = lpObj->Y;
		pTeleportResult.Dir = lpObj->Dir;

		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pTeleportResult, pTeleportResult.h.size);
		return;
	}

	lpObj->m_LastMoveTime = GetTickCount();
	lpObj->m_Rest = 0;
	lpObj->PathCur = 0;
	lpObj->Dir = lpMove->Path[0] >> 4;
	lpObj->PathCount = lpMove->Path[0] & 0x0F;

	if (lpObj->PathCount > 15)
	{
		return;
	}

	for (n = 0; n < 15; n++)
	{
		lpObj->PathX[n] = 0;
		lpObj->PathY[n] = 0;
		lpObj->PathOri[n] = 0;
	}

	sx = lpMove->X;
	sy = lpMove->Y;

	/*if (DG_MAP_RANGE(lpObj->MapNumber) == TRUE)
	{
		if (g_DoppelGanger.GetDoppelgangerState() == 1 && g_DoppelGanger.CheckMapTile(lpObj->MapNumber, lpObj->X, lpObj->Y) == FALSE)
		{
			gObjSetPosition(*lpObj, lpObj->X, lpObj->Y);
			return;
		}
	}*/

	/*
	if ( gObjCheckXYMapTile(lpObj, 1) == TRUE )
	{
		lpObj->PathCount = 0;
		lpObj->PathCur = 0;
		lpObj->PathStartEnd = 0;
		gObjSetPosition(*lpObj, lpObj->X, lpObj->Y);
		return;
	}*/

	lpObj->PathX[0] = lpMove->X;
	ax = lpObj->PathX[0];
	lpObj->PathY[0] = lpMove->Y;
	ay = lpObj->PathY[0];
	lpObj->PathDir[0] = lpObj->Dir;
	lpObj->PathStartEnd = 1;

	if (lpObj->PathCount > 0)
	{
		lpObj->PathCur = 1;
		lpObj->PathCount++;
	}

	for (n = 1; n < lpObj->PathCount; n++)
	{
		if ((n % 2) == 1)
		{
			pathtable = lpMove->Path[(n + 1) / 2] >> 4;
		}
		else
		{
			pathtable = lpMove->Path[(n + 1) / 2] & 0x0F;
		}

		ax += RoadPathTable[pathtable * 2];
		ay += RoadPathTable[pathtable * 2 + 1];

		lpObj->PathOri[n - 1] = pathtable;
		lpObj->PathDir[n] = pathtable;
		lpObj->PathX[n] = ax;
		lpObj->PathY[n] = ay;
	}

	if (lpObj->PathCount > 0)
	{
		int nextX;
		int nextY;
		BYTE mapnumber;
		BYTE attr;
		nextX = lpObj->PathX[1];
		nextY = lpObj->PathY[1];
		mapnumber = lpObj->MapNumber;
		attr = MapC[mapnumber].GetAttr(nextX, nextY);

		if ((((BYTE)attr & (BYTE)4) == (BYTE)4) || (((BYTE)attr & (BYTE)8) == (BYTE)8))
		{
			for (n = 0; n < 15; n++)
			{
				lpObj->PathX[n] = 0;
				lpObj->PathY[n] = 0;
				lpObj->PathOri[n] = 0;
			}

			lpObj->PathCount = 0;
			lpObj->PathCur = 0;
			lpObj->PathStartEnd = 0;
			gObjSetPosition(*lpObj, lpObj->X, lpObj->Y);
			return;
		}
	}

	lpObj->TX = (BYTE)ax;
	lpObj->TY = (BYTE)ay;

	if (lpObj->Type == OBJ_USER && IMPERIAL_MAP_RANGE(lpObj->MapNumber))
	{
		int iCX = lpObj->X;
		int iCY = lpObj->Y;
		BOOL bSetStand = FALSE;
		WORD wMapAttr = 0;

		for (int iCOUNT = 0; iCOUNT < 256; iCOUNT++)
		{
			if (iCX > lpObj->TX)
				iCX--;
			if (iCX < lpObj->TX)
				iCX++;
			if (iCY > lpObj->TY)
				iCY--;
			if (iCY < lpObj->TY)
				iCY++;

			wMapAttr = MapC[lpObj->MapNumber].GetAttr(iCX, iCY);

			if (wMapAttr & 0x10)
			{
				bSetStand = TRUE;
				break;
			}

			if (iCX == lpObj->TX && iCY == lpObj->TY)
			{
				break;
			}

			if (bSetStand == 1)
			{
				lpObj->m_Rest = 1;
				lpObj->PathCur = 0;
				lpObj->PathCount = 0;
				lpObj->PathStartEnd = 0;
				gObjSetPosition(*lpObj, lpObj->X, lpObj->Y);

				PMSG_ACTIONRESULT pActionResult;

				PHeadSetB((LPBYTE)&pActionResult, 0x18, sizeof(pActionResult));
				pActionResult.NumberH = SET_NUMBERH(lpObj);
				pActionResult.NumberL = SET_NUMBERL(lpObj);
				pActionResult.ActionNumber = 0x7A;
				pActionResult.Dir = lpObj->Dir;
				pActionResult.TargetNumberH = 0;
				pActionResult.TargetNumberL = 0;

				GIOCP.DataSend(lpObj->m_Index, (LPBYTE)&pActionResult, sizeof(pActionResult));
				return;
			}
		}
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (lpObj->Type == OBJ_USER)
		{
			int nextX = lpObj->X;
			int nextY = lpObj->Y;
			int iResult = 0; //should be BOOL
			WORD attr = 0;

			for (int i = 0; i < 256; i++)
			{
				if (nextX > lpObj->TX)
				{
					nextX -= 1;
				}

				if (nextX < lpObj->TX)
				{
					nextX += 1;
				}

				if (nextY > lpObj->TY)
				{
					nextY -= 1;
				}

				if (nextY < lpObj->TY)
				{
					nextY += 1;
				}

				attr = MapC[lpObj->MapNumber].GetAttr(nextX, nextY);

				if ((attr & 16) == 16)
				{
					iResult = 1;
					break;
				}

				if (nextX == lpObj->TX)
				{
					if (nextY == lpObj->TY)
					{
						break;
					}
				}
			}

			if (iResult == 1)
			{
				lpObj->m_Rest = 1;
				lpObj->PathCur = 0;
				lpObj->PathCount = 0;
				lpObj->PathStartEnd = 0;

				gObjSetPosition(*lpObj, lpObj->X, lpObj->Y);

				PMSG_ACTIONRESULT pActionResult;

				PHeadSetB((LPBYTE)&pActionResult, 0x18, sizeof(pActionResult));
				pActionResult.NumberH = SET_NUMBERH(lpObj);
				pActionResult.NumberL = SET_NUMBERL(lpObj);
				pActionResult.ActionNumber = 0x7A;
				pActionResult.Dir = lpObj->Dir;
				pActionResult.TargetNumberH = 0;
				pActionResult.TargetNumberL = 0;

				GIOCP.DataSend(lpObj->m_Index, (LPBYTE)&pActionResult, sizeof(pActionResult));
				return;
			}
		}
	}

	/*if (BC_MAP_RANGE(lpObj->MapNumber) && lpObj->Type == OBJ_USER)
	{
		int iBridgeIndex = g_BloodCastle.GetBridgeIndex(lpObj->MapNumber);
		switch (g_BloodCastle.GetCurrentState(iBridgeIndex))
		{
		case 0x01:
			if (lpObj->TY > 15)
			{
				lpObj->m_Rest = 1;
				lpObj->PathCur = 0;
				lpObj->PathCount = 0;
				lpObj->PathStartEnd = 0;
				gObjSetPosition(*lpObj, lpObj->X, lpObj->Y);

				PMSG_ACTIONRESULT pActionResult;

				PHeadSetB((LPBYTE)&pActionResult, 0x18, sizeof(pActionResult));
				pActionResult.NumberH = SET_NUMBERH(lpObj);
				pActionResult.NumberL = SET_NUMBERL(lpObj);
				pActionResult.ActionNumber = 0x7A;
				pActionResult.Dir = lpObj->Dir;
				pActionResult.TargetNumberH = 0;
				pActionResult.TargetNumberL = 0;

				GIOCP.DataSend(lpObj->m_Index, (LPBYTE)&pActionResult, sizeof(pActionResult));
				return;
			}
			break;
		case 0x02:
			if (lpObj->Y > 17 && g_BloodCastle.CheckPlayStart(iBridgeIndex) == false)
			{
				gObjMoveGate(*lpObj, iBridgeIndex + 66);
				return;
			}

			if (lpObj->TY > 15 && g_BloodCastle.CheckPlayStart(iBridgeIndex) == false)
			{
				lpObj->m_Rest = 1;
				lpObj->PathCur = 0;
				lpObj->PathCount = 0;
				lpObj->PathStartEnd = 0;
				gObjSetPosition(*lpObj, lpObj->X, lpObj->Y);

				PMSG_ACTIONRESULT pActionResult;

				PHeadSetB((LPBYTE)&pActionResult, 0x18, sizeof(pActionResult));
				pActionResult.NumberH = SET_NUMBERH(lpObj);
				pActionResult.NumberL = SET_NUMBERL(lpObj);
				pActionResult.ActionNumber = 0x7A;
				pActionResult.Dir = lpObj->Dir;
				pActionResult.TargetNumberH = 0;
				pActionResult.TargetNumberL = 0;

				GIOCP.DataSend(lpObj->m_Index, (LPBYTE)&pActionResult, sizeof(pActionResult));
				return;
			}

			if (lpObj->TY > 76 && g_BloodCastle.CheckCastleDoorLive(iBridgeIndex) == true)
			{
				lpObj->m_Rest = 1;
				lpObj->PathCur = 0;
				lpObj->PathCount = 0;
				lpObj->PathStartEnd = 0;
				gObjSetPosition(*lpObj, lpObj->X, lpObj->Y);

				PMSG_ACTIONRESULT pActionResult;

				PHeadSetB((LPBYTE)&pActionResult, 0x18, sizeof(pActionResult));
				pActionResult.NumberH = SET_NUMBERH(lpObj);
				pActionResult.NumberL = SET_NUMBERL(lpObj);
				pActionResult.ActionNumber = 0x7A;
				pActionResult.Dir = lpObj->Dir;
				pActionResult.TargetNumberH = 0;
				pActionResult.TargetNumberL = 0;

				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pActionResult, sizeof(pActionResult));
				return;
			}
			break;
		}
	}*/

	PHeadSetB((LPBYTE)&pMove, 0xD7, sizeof(pMove));
	pMove.NumberH = SET_NUMBERH(lpObj);
	pMove.NumberL = SET_NUMBERL(lpObj);
	pMove.X = ax;
	pMove.Y = ay;
	pMove.Path = lpObj->Dir << 4;

	if (lpObj->Type == OBJ_USER)
	{
		if (lpObj->m_byCCF_CanEnter == 0xFF)
		{
			lpObj->m_byCCF_CanEnter = 0;

			if (lpObj->m_bCCF_UI_Using == TRUE)
			{
				lpObj->m_bCCF_UI_Using = FALSE;
			}
		}

		if (g_ConfigRead.server.GetServerType() == SERVER_NORMAL)
		{
			/*if (IT_MAP_RANGE(lpObj->MapNumber))
			{
				if (lpObj->m_nITR_RelicsTick > 0 || lpObj->m_wITR_NpcType > 0 || lpObj->m_byITR_StoneState != 99 || lpObj->m_bITR_RegisteringRelics == true || lpObj->m_bITR_GettingRelics == true)
				{
					g_IT_Event.CancleStoneState(lpObj->m_wITR_NpcType, lpObj->m_byITR_StoneState, lpObj->MapNumber);
					lpObj->m_nITR_RelicsTick = 0;
					lpObj->m_wITR_NpcType = 0;
					lpObj->m_byITR_StoneState = 99;
					lpObj->m_bITR_RegisteringRelics = false;
					lpObj->m_bITR_GettingRelics = false;
				}
			}*/
		}

		if (lpObj->m_IfState.use)
		{
			if (lpObj->m_IfState.type == 3)
			{
				lpObj->TargetNpcNumber = -1;
				lpObj->m_IfState.type = 0;
				lpObj->m_IfState.use = 0;
			}
		}

		if (!gObjPositionCheck(*lpObj))
		{
			lpObj->PathCur = 0;
			lpObj->PathCount = 0;
			ax = lpObj->X;
			ay = lpObj->Y;
			lpObj->TX = (BYTE)ax;
			lpObj->TY = (BYTE)ay;
			pMove.X = ax;
			pMove.Y = ay;
		}

		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMove, pMove.h.size);
	}

	int MVL = MAX_VIEWPORT;

	if (lpObj->Type == OBJ_MONSTER)
		MVL = MaxViewportMonster;

	for (n = 0; n < MVL; n++)
	{
		CGameObject* lpObjVP2 = getGameObject(lpObj->VpPlayer2[n]->number);
		if (lpObj->VpPlayer2[n]->state == TRUE)
		{
			if (lpObjVP2->m_PlayerData->ConnectUser->Connected > PLAYER_CONNECTED)
			{
				if (lpObjVP2->Live)
				{
					if (lpObjVP2->Type == OBJ_USER)
					{
						GIOCP.DataSend(lpObjVP2->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMove, pMove.h.size);
					}
				}
			}
			else
			{
				lpObj->VpPlayer2[n]->number = -1;
				lpObj->VpPlayer2[n]->state = 0;
				lpObj->VPCount2--;
			}
		}
	}

	MapC[lpObj->MapNumber].ClearStandAttr(lpObj->m_OldX, lpObj->m_OldY);
	MapC[lpObj->MapNumber].SetStandAttr(lpObj->TX, lpObj->TY);
	lpObj->m_OldX = lpObj->TX;
	lpObj->m_OldY = lpObj->TY;
	lpObj->X = sx;
	lpObj->Y = sy;
	lpObj->m_ViewState = 0;

	if (lpObj->Type == OBJ_USER) // Recv Check
	{
		if (rand() % 5 == 0)
		{
			this->GCSendRecvCheck(lpObj);
		}
	}
}



void GameProtocol::RecvPositionSetProc(PMSG_POSISTION_SET * lpMove, CGameObject* lpObj)
{
	short n;

	if (this->PacketCheckTime(lpObj) == FALSE)
	{
		return;
	}

	if (lpObj->Teleport != 0)
	{
		return;
	}

	if (MAX_MAP_RANGE(lpObj->MapNumber) == FALSE)
	{
		return;
	}

	//K2 AntiHack Teleport
	int XDif = 0;
	int YDif = 0;

	if (lpObj->X > lpMove->X)
		XDif = lpObj->X - lpMove->X;
	else
		XDif = lpMove->X - lpObj->X;

	if (lpObj->Y > lpMove->Y)
		YDif = lpObj->Y - lpMove->Y;
	else
		YDif = lpMove->Y - lpObj->Y;

	//sLog->outBasic("[K2] XDif %d YDif %d", XDif, YDif);
	if ((XDif > 6) || (YDif > 6))
	{
		PMSG_TELEPORT_RESULT pTeleportResult;

		pTeleportResult.h.c = 0xC3;
		pTeleportResult.h.size = sizeof(pTeleportResult);
		pTeleportResult.h.headcode = 0x1C;
		pTeleportResult.Type = 0;
		pTeleportResult.Unk = 0;
		pTeleportResult.MapNumber = lpObj->MapNumber;
		pTeleportResult.Unk1 = 0;
		pTeleportResult.MapX = lpObj->X;
		pTeleportResult.MapY = lpObj->Y;
		pTeleportResult.Dir = lpObj->Dir;

		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pTeleportResult, pTeleportResult.h.size);

		PMSG_NOTICE pNotice;

		TNotice::MakeNoticeMsg((TNotice *)&pNotice, 1, "[K2][Anti-Cheat] Teleport Hack Blocked.");
		sLog->outBasic("[K2][Anti-Cheat] Teleport Hack Blocked. [AccID=%s | CharName=%s]", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
		TNotice::SendNoticeToUser(lpObj->m_PlayerData->ConnectUser->Index, (TNotice *)&pNotice);

		return;
	}
	lpObj->X = lpMove->X;
	lpObj->Y = lpMove->Y;

	/*if (CC_MAP_RANGE(lpObj->MapNumber) != FALSE)
	{
		if ((GetTickCount() - lpObj->m_iChaosCastleBlowTime) < 1000)
		{
			return;
		}
	}*/

	if (lpObj->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		if ((GetTickCount() - lpObj->m_nCCF_BlowTime) < 1000)
		{
			return;
		}
	}

	if (gObjCheckXYMapTile(*lpObj, 2) == TRUE)
	{
		return;
	}

	PMSG_RECV_POSISTION_SET pMove;

	PHeadSetB((LPBYTE)&pMove, 0x10, sizeof(pMove));
	pMove.NumberH = SET_NUMBERH(lpObj);
	pMove.NumberL = SET_NUMBERL(lpObj);
	pMove.X = lpMove->X;
	pMove.Y = lpMove->Y;
	lpObj->TX = lpMove->X;
	lpObj->TY = lpMove->Y;

	if (::gObjPositionCheck(*lpObj) == FALSE)
	{
		return;
	}

	CreateFrustrum(lpObj->X, lpObj->Y, *lpObj);

	if (lpObj->Type == OBJ_USER)
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR *)&pMove, pMove.h.size);
	}

	int MVL = MAX_VIEWPORT;

	if (lpObj->Type == OBJ_MONSTER)
	{
		MVL = MaxViewportMonster;
	}

	for (n = 0; n < MVL; n++)
	{
		CGameObject* lpObjVP2 = getGameObject(lpObj->VpPlayer2[n]->number);

		if (lpObjVP2->m_PlayerData)
		{
			if (lpObj->VpPlayer2[n]->state == 1)
			{
				if (lpObjVP2->m_PlayerData->ConnectUser->Connected > PLAYER_CONNECTED && lpObjVP2->Live != FALSE)
				{
					GIOCP.DataSend(lpObjVP2->m_PlayerData->ConnectUser->Index, (UCHAR *)&pMove, pMove.h.size);
				}
				else
				{
					lpObj->VpPlayer2[n]->number = -1;
					lpObj->VpPlayer2[n]->state = FALSE;
					lpObj->VPCount2--;
				}
			}
		}
	}

	MapC[lpObj->MapNumber].ClearStandAttr(lpObj->m_OldX, lpObj->m_OldY);
	MapC[lpObj->MapNumber].SetStandAttr(lpObj->TX, lpObj->TY);

	lpObj->m_OldX = lpObj->TX;
	lpObj->m_OldY = lpObj->TY;

}



void GameProtocol::CGAttack(PMSG_ATTACK* lpMsg, CGameObject* lpObj)
{
	int usernumber = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
	CGameObject* lpTargetObj = getGameObject(usernumber);
	
	if (!lpTargetObj->Live)
		return;

	if (lpObj->Type == OBJ_USER  && lpObj->m_PlayerData->m_bSecurityCheck == false)
		return;
	int Allowed = 120;
	if (lpObj->m_AttackSpeed >= 300)
	{
		Allowed = 170; //6.26
	}
	else if ((lpObj->m_AttackSpeed > 150) && (lpObj->m_AttackSpeed < 300))
		Allowed = 190;
	else if ((lpObj->m_AttackSpeed > 0) && (lpObj->m_AttackSpeed < 150))
		Allowed = 190;
	if (lpObj->Type == OBJ_USER)
	{
		//10:50:00
		//10:51:00
		//1000 iTimeCalc < Allowed 300//
		int iTimeCalc = GetTickCount() - lpObj->m_LastAttackTime;
		lpObj->m_LastAttackTime = GetTickCount();
		if (iTimeCalc < Allowed) //1 1200
		{
			//g_Log.AddC(TColor::Green, "[K2 ANTIHACK] Speed Hack Detect %s %s %d %d", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, Allowed, lpObj->m_AttackSpeed);
			return;
		}
	}
	/*if (iTimeCalc < lpObj->m_DetectSpeedHackTime) //season4 changed (fix)
	{
		lpObj->m_DetectCount++;
		lpObj->m_SumLastAttackTime += iTimeCalc;

		if (lpObj->m_DetectCount > gHackCheckCount)
		{
			lpObj->m_DetectedHackKickCount++;
			lpObj->m_SpeedHackPenalty = gSpeedHackPenalty;

			if (gIsKickDetecHackCountLimit)
			{
				if (lpObj->m_DetectedHackKickCount > gDetectedHackKickCount)
				{
					sLog->outBasic("[%s][%s] Kick DetecHackCountLimit Over User (%d)", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_DetectedHackKickCount);
					this->GCSendDisableReconnect(lpObj);
					//GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
					return;
				}

				sLog->outBasic("[%s][%s] Attack Speed Is Wrong Normal (%d)(%d) Penalty %d", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_DetectSpeedHackTime, lpObj->m_SumLastAttackTime / lpObj->m_DetectCount, lpObj->m_SpeedHackPenalty);
			}

		}

		lpObj->m_LastAttackTime = GetTickCount();
	}
	else
	{
		lpObj->m_SumLastAttackTime = 0;
		lpObj->m_DetectCount = 0;
	}
	*/
	lpObj->m_LastAttackTime = GetTickCount();

	if (bIsIgnorePacketSpeedHackDetect)
	{
		if (lpObj->m_SpeedHackPenalty > 0)
		{
			lpObj->m_SpeedHackPenalty--;

			return;
		}
	}

	lpObj->Dir = lpMsg->DirDis;
	// (lpMsg->AttackAction == 6)
	GCActionSend(lpObj, lpMsg->AttackAction, lpObj, usernumber);
	gComboAttack.CheckCombo(*lpObj, 0);
	gObjAttack.Attack(*lpObj, *lpTargetObj, NULL, FALSE, FALSE, 0, FALSE, 0, 0, 0);

	lpObj->UseMagicNumber = 0;
}


void GameProtocol::GCDamageSend(CGameObject* lpObj, CGameObject* lpTargetObj, int AttackDamage, int MSBFlag, int MSBDamage, int iShieldDamage)
{
	PMSG_ATTACKRESULT pResult;

	PHeadSetB((LPBYTE)&pResult, 0xDF, sizeof(pResult));
	pResult.NumberH = SET_NUMBERH(lpTargetObj->m_Index);
	pResult.NumberL = SET_NUMBERL(lpTargetObj->m_Index);
	pResult.DamageH = SET_NUMBERH(AttackDamage);
	pResult.DamageL = SET_NUMBERL(AttackDamage);
	pResult.btShieldDamageH = SET_NUMBERH(iShieldDamage);
	pResult.btShieldDamageL = SET_NUMBERL(iShieldDamage);
	pResult.IGCDamage = AttackDamage;
	pResult.ElementalDmg = iShieldDamage;

	if (MSBFlag != FALSE)
	{
		pResult.NumberH &= 0x7F;
		pResult.NumberH |= 0x80;
	}

	pResult.DamageTypeH = HIBYTE(MSBDamage);
	pResult.DamageTypeL = LOBYTE(MSBDamage);

	if (lpTargetObj->Type == OBJ_USER)
	{
		GIOCP.DataSend(lpTargetObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
	}

	if (cManager.WatchTargetIndex == lpTargetObj->m_Index || cManager.WatchTargetIndex == lpObj->m_Index)
	{
		cManager.DataSend((LPBYTE)&pResult, pResult.h.size);
	}

	if (lpObj->Type == OBJ_USER)
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
	}
}


struct PMSG_DAMAGE
{
	PBMSG_HEAD h;	// C1:F3:07
	BYTE subcode;	// 3
	BYTE DamageH;	// 4
	BYTE DamageL;	// 5
	BYTE btShieldDamageH;	// 6
	BYTE btShieldDamageL;	// 7
};

void GameProtocol::GCDamageSendPoison(CGameObject* lpObj, int damage, int iShieldDamage)
{
	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	PMSG_DAMAGE pDamage;

	PHeadSubSetB((LPBYTE)&pDamage, 0xF3, 0x07, sizeof(pDamage));
	pDamage.DamageH = SET_NUMBERH(damage);
	pDamage.DamageL = SET_NUMBERL(damage);
	pDamage.btShieldDamageH = SET_NUMBERH(iShieldDamage);
	pDamage.btShieldDamageL = SET_NUMBERL(iShieldDamage);

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pDamage, pDamage.h.size);
}


struct PMSG_KILLPLAYER
{
	PBMSG_HEAD h;	// C1:16
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	BYTE ExpH;	// 5
	BYTE ExpL;	// 6
	BYTE DamageH;	// 7
	BYTE DamageL;	// 8
};


void GameProtocol::GCKillPlayerExpSend(CGameObject* lpObj, CGameObject* lpTargetObj, int exp, int AttackDamage, BOOL MSBFlag)
{
	PMSG_KILLPLAYER_EXT pkillMsg;

	PHeadSetBE((LPBYTE)&pkillMsg, 0x9c, sizeof(pkillMsg));
	pkillMsg.NumberH = SET_NUMBERH(lpTargetObj->m_Index);
	pkillMsg.NumberL = SET_NUMBERL(lpTargetObj->m_Index);
	pkillMsg.ExpH = SET_NUMBERHW(exp);
	pkillMsg.ExpL = SET_NUMBERLW(exp);
	pkillMsg.DamageH = SET_NUMBERH(AttackDamage);
	pkillMsg.DamageL = SET_NUMBERL(AttackDamage);

	if (MSBFlag != FALSE)
	{
		pkillMsg.NumberH &= 0x7F;
		pkillMsg.NumberH |= 0x80;
	}

	/*if (BC_MAP_RANGE(lpObj->MapNumber) != FALSE)
	{
		g_BloodCastle.AddExperience(*lpObj, exp);
	}*/

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pkillMsg, pkillMsg.h.size);
}

void GameProtocol::GCKillPlayerMasterExpSend(CGameObject* lpObj, CGameObject* lpTargetObj, int exp, int AttackDamage, BOOL MSBFlag)
{
	PMSG_KILLPLAYER_EXT pkillMsg;

	PHeadSetBE((LPBYTE)&pkillMsg, 0x9C, sizeof(pkillMsg));
	pkillMsg.NumberH = SET_NUMBERH(lpTargetObj->m_Index);
	pkillMsg.NumberL = SET_NUMBERL(lpTargetObj->m_Index);
	pkillMsg.ExpH = SET_NUMBERHW(exp);
	pkillMsg.ExpL = SET_NUMBERLW(exp);
	pkillMsg.DamageH = SET_NUMBERH(AttackDamage);
	pkillMsg.DamageL = SET_NUMBERL(AttackDamage);

	if (MSBFlag != FALSE)
	{
		pkillMsg.NumberH &= 0x7F;
		pkillMsg.NumberH |= 0x80;
	}

	/*if (BC_MAP_RANGE(lpObj->MapNumber) != FALSE)
	{
		g_BloodCastle.AddExperience(*lpObj, exp);
	}*/

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pkillMsg, pkillMsg.h.size);
}



struct PMSG_DIEPLAYER
{
	PBMSG_HEAD h;	// C1:17
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	BYTE Skill;	// 5
	BYTE KillerNumberH;	// 6
	BYTE KillerNumberL;	// 7
};


void GameProtocol::GCDiePlayerSend(CGameObject* lpObj, CGameObject* lpTargetObj, BYTE skill, int KillerIndex)
{
	PMSG_DIEPLAYER pDieMsg;

	PHeadSetB((LPBYTE)&pDieMsg, 0x17, sizeof(pDieMsg));
	pDieMsg.NumberH = SET_NUMBERH(lpTargetObj->m_Index);
	pDieMsg.NumberL = SET_NUMBERL(lpTargetObj->m_Index);
	pDieMsg.Skill = skill;
	pDieMsg.KillerNumberH = SET_NUMBERH(KillerIndex);
	pDieMsg.KillerNumberL = SET_NUMBERL(KillerIndex);
	int iSize = pDieMsg.h.size;

	MsgSendV2(lpObj, (LPBYTE)&pDieMsg, iSize);

	if (lpObj->Type == OBJ_USER)
	{
		GIOCP.DataSend(lpObj->m_Index, (LPBYTE)&pDieMsg, iSize);
	}
}



void GameProtocol::GCActionSend(CGameObject* lpObj, BYTE ActionNumber, CGameObject* lpObjTarget, int aTargetIndex)
{
	PMSG_ACTIONRESULT pActionResult;

	PHeadSetB((LPBYTE)&pActionResult, 0x18, sizeof(pActionResult));
	pActionResult.NumberH = SET_NUMBERH(lpObj);
	pActionResult.NumberL = SET_NUMBERL(lpObj);
	pActionResult.ActionNumber = ActionNumber;
	pActionResult.Dir = lpObj->Dir;
	pActionResult.TargetNumberH = SET_NUMBERH(aTargetIndex);
	pActionResult.TargetNumberL = SET_NUMBERL(aTargetIndex);

	MsgSendV2(lpObj, (LPBYTE)&pActionResult, pActionResult.h.size);
}



void GameProtocol::CGActionRecv(PMSG_ACTION * lpMsg, CGameObject* lpObj)
{
	if (lpMsg->Dir > 7 || lpMsg->h.size > 5)
	{
		//this->GCSendDisableReconnect(lpObj);
		GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
	}

	PMSG_ACTIONRESULT pResult;

	PHeadSetB((LPBYTE)&pResult, 0x18, sizeof(pResult));
	pResult.ActionNumber = lpMsg->ActionNumber;
	pResult.NumberH = SET_NUMBERH(lpObj);
	pResult.NumberL = SET_NUMBERL(lpObj);
	lpObj->m_ActionNumber = lpMsg->ActionNumber;
	lpObj->Dir = lpMsg->Dir;
	pResult.Dir = lpObj->Dir;
	pResult.TargetNumberH = lpMsg->iTargetIndexH;
	pResult.TargetNumberL = lpMsg->iTargetIndexL;

	if (lpObj->m_ActionNumber == 0x80)
	{
		lpObj->m_ViewState = 1;
		lpObj->m_Rest = lpObj->m_ActionNumber;
	}

	else if (lpObj->m_ActionNumber == 0x81)
	{
		lpObj->m_ViewState = 2;
		lpObj->m_Rest = lpObj->m_ActionNumber;
	}

	else if (lpObj->m_ActionNumber == 0x82)
	{
		lpObj->m_ViewState = 3;
		lpObj->m_Rest = lpObj->m_ActionNumber;
	}

	if (GetTickCount() - lpObj->m_ActionTime < 500)
	{
		lpObj->m_ActionCount++;

		if (lpObj->m_ActionCount > 9)
		{
			//this->GCSendDisableReconnect(lpObj);
			GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
			return;
		}
	}

	else
	{
		lpObj->m_ActionCount = 0;
	}

	lpObj->m_ActionTime = GetTickCount();

	int MVL = MAX_VIEWPORT;

	if (lpObj->Type == OBJ_MONSTER)
		MVL = MaxViewportMonster;

	for (int n = 0; n < MVL; n++)
	{
		if (lpObj->VpPlayer2[n]->type == OBJ_USER)
		{
			if (lpObj->VpPlayer2[n]->state != 0 && lpObj->VpPlayer2[n]->state != 0x10 && lpObj->VpPlayer2[n]->state != 0x08)
			{
				GIOCP.DataSend(lpObj->VpPlayer2[n]->number, (LPBYTE)&pResult, pResult.h.size);
			}
		}
	}
}



void GameProtocol::CGMagicAttack(LPBYTE lpRecv, CGameObject* lpObj)
{
	PMSG_MAGICATTACK * lpMsg = (PMSG_MAGICATTACK *)(lpRecv);

	
	WORD MagicNumber = MAKE_NUMBERW(lpMsg->MagicNumberH, lpMsg->MagicNumberL);
	int usernumber = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
	CGameObject* lpTargetObj = getGameObject(usernumber);

	if (lpTargetObj->Live == 0 || lpTargetObj->m_PlayerData->ConnectUser->Connected < PLAYER_LOGGED)
		return;

	if (lpObj->m_PlayerData->ConnectUser->CloseCount >= 0)
		return;

	if (lpObj->Type == OBJ_USER && lpObj->m_PlayerData->m_bSecurityCheck == false)
		return;

	CMagicInf * lpMagic;
	int attackret = gObjCheckAttackArea(*lpObj, usernumber);

	if (attackret)
	{

		if (lpObj->Class == 275)	// Kundun
			lpObj->TargetNumber = -1;

		if (bIsIgnorePacketSpeedHackDetect)
			return;
	}

	if (lpObj->Type == OBJ_USER)
	{

		if (MagicNumber == 58) // Nova cast
			usernumber = 58;

		if (MagicNumber == 395)
			usernumber = 395;

		lpMagic = gObjGetMagicSearch(*lpObj, MagicNumber);
		if (!lpMagic)
			return;

		if (lpMagic->m_Skill == AT_SKILL_RECALL_PARTY)
		{
			if (g_NewPVP.IsDuel(*lpObj) || g_NewPVP.IsDuel(*lpTargetObj))
			{
				return;
			}
		}
		if (MagicDamageC.SkillGetRequireClass(lpObj->Class, lpObj->m_PlayerData->ChangeUP, lpMagic->m_Skill) < 1)
			return;
		if (MagicDamageC.CheckStatus(lpMagic->m_Skill, lpObj->m_PlayerData->GuildStatus) == 0)
		{
			return;
		}
		if (MagicDamageC.CheckKillCount(lpMagic->m_Skill, lpObj->m_btKillCount) < 0)
		{
			return;
		}

		if (lpObj->Teleport && gObjUseSkill.m_SkillData.EnableWizardSkillWhileTeleport == false)
		{
			return;
		}
	}
	else
	{
		lpMagic = gObjGetMagic(*lpObj, MagicNumber);
		if (!lpMagic)
			return;
	}

	if (lpMagic->m_Skill != 24 && gObjUseSkill.SpeedHackCheck(*lpObj) == FALSE)
		return;

	if (!gCheckSkillDistance(*lpObj, getGameObject(usernumber), lpMagic->m_Skill))
	{
		DWORD dwTick = GetTickCount() - lpObj->m_dwSkillDistanceErrorTick;

		if (dwTick > (g_ConfigRead.SkillDistanceTimeDC * 1000))
		{
			lpObj->m_iSkillDistanceErrorCount = 0;
			lpObj->m_dwSkillDistanceErrorTick = GetTickCount();
		}

		lpObj->m_iSkillDistanceErrorCount++;

		if (lpObj->m_iSkillDistanceErrorCount > g_ConfigRead.SkillDistanceCountDC)
		{
			if (g_ConfigRead.SkillDistanceDC == 1)
			{
				//this->GCSendDisableReconnect(lpObj->m_Index);
				GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
				return;
			}

			lpObj->m_iSkillDistanceErrorCount = 0;
		}

		return;
	}

	gObjUseSkill.UseSkill(*lpObj, usernumber, lpMagic);
}

void GameProtocol::CGRageAttack(LPBYTE lpRecv, CGameObject* lpObj)
{
	PMSG_RAGEATTACK_REQ * lpMsg = (PMSG_RAGEATTACK_REQ *)(lpRecv);

	WORD MagicNumber = MAKE_NUMBERW(lpMsg->MagicNumberH, lpMsg->MagicNumberL);
	int usernumber = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
	CMagicInf * lpMagic;

	CGameObject* lpTargetObj = getGameObject(usernumber);

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	if (lpObj->Teleport && gObjUseSkill.m_SkillData.EnableWizardSkillWhileTeleport == false)
	{
		return;
	}

	if (lpObj->m_PlayerData->ConnectUser->CloseCount >= 0)
	{
		return;
	}

	if (MagicNumber == 263 || MagicNumber == 559 || MagicNumber == 563)
	{
		this->GCMonkMagicAttack(lpObj, MagicNumber, lpObj->m_PlayerData->DarkSideTarget[0], TRUE);
		return;
	}

	if (lpTargetObj->Live == 0 || lpTargetObj->m_PlayerData->ConnectUser->Connected < PLAYER_LOGGED)
		return;

	if (lpObj->Type == OBJ_USER)
	{
		lpMagic = gObjGetMagicSearch(*lpObj, MagicNumber);

		if (!lpMagic)
			return;

		if (lpMagic->m_Skill == AT_SKILL_RECALL_PARTY)
		{
			if (g_NewPVP.IsDuel(*lpObj) || g_NewPVP.IsDuel(*lpTargetObj))
			{
				return;
			}
		}

		if (MagicDamageC.SkillGetRequireClass(lpObj->Class, lpObj->m_PlayerData->ChangeUP, lpMagic->m_Skill) < 1)
			return;

		if (MagicDamageC.CheckStatus(lpMagic->m_Skill, lpObj->m_PlayerData->GuildStatus) == 0)
		{
			return;
		}

		if (MagicDamageC.CheckKillCount(lpMagic->m_Skill, lpObj->m_btKillCount) < 0)
		{
			return;
		}

		if (lpObj->m_PlayerData->m_bSecurityCheck == false)
		{
			return;
		}
	}

	if (!gCheckSkillDistance(*lpObj, lpTargetObj, lpMagic->m_Skill))
	{
		DWORD dwTick = GetTickCount() - lpObj->m_dwSkillDistanceErrorTick;

		if (dwTick > (g_ConfigRead.SkillDistanceTimeDC * 1000))
		{
			lpObj->m_iSkillDistanceErrorCount = 0;
			lpObj->m_dwSkillDistanceErrorTick = GetTickCount();
		}

		lpObj->m_iSkillDistanceErrorCount++;

		if (lpObj->m_iSkillDistanceErrorCount > g_ConfigRead.SkillDistanceCountDC)
		{
			if (g_ConfigRead.SkillDistanceDC == 1)
			{
				//this->GCSendDisableReconnect(lpObj->m_Index);
				GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
				return;
			}

			lpObj->m_iSkillDistanceErrorCount = 0;
		}

		return;
	}

	this->GCMonkMagicAttack(lpObj, MagicNumber, usernumber, TRUE);
}

void GameProtocol::CGRageAttackRange(PMSG_RAGE_ATTACK_RANGE *lpMsg, CGameObject* lpObj)
{
	if (lpObj->Teleport && gObjUseSkill.m_SkillData.EnableWizardSkillWhileTeleport == false)
	{
		return;
	}

	CMagicInf * lpMagic;

	WORD MagicNumber = MAKE_NUMBERW(lpMsg->MagicNumberH, lpMsg->MagicNumberL);
	WORD TargetIndex = MAKE_NUMBERW(lpMsg->TargetIndexH, lpMsg->TargetIndexL);
	WORD Target[5];

	for (int i = 0; i < 5; i++)
	{
		Target[i] = 10000;
	}

	lpMagic = gObjGetMagicSearch(*lpObj, MagicNumber);

	if (lpMagic)
	{
		if (gObjUseSkill.SkillMonkDarkSideGetTargetIndex(*lpObj, TargetIndex, lpMagic, Target) == TRUE)
		{
			PMSG_RAGE_ATTACK_RANGE_ANS pMsg;
			PHeadSetBE((LPBYTE)&pMsg, 0x4B, sizeof(pMsg));

			memcpy(&pMsg.Target, Target, sizeof(Target));
			pMsg.MagicNumber = lpMagic->m_Skill;;

			lpObj->m_PlayerData->DarkSideCount = 0;

			for (int i = 0; i < 5; i++)
			{
				if (Target[i] != 10000)
				{
					lpObj->m_PlayerData->DarkSideCount++;
				}
			}

			memcpy(lpObj->m_PlayerData->DarkSideTarget, Target, sizeof(Target));
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		}
	}
}

void GameProtocol::GCMagicAttackNumberSend(CGameObject* lpObj, WORD MagicNumber, int usernumber, unsigned char skillsuccess)
{
	PMSG_MAGICATTACK_RESULT pAttack;

	PHeadSetBE((LPBYTE)&pAttack, 0x19, sizeof(pAttack));
	pAttack.MagicNumberH = SET_NUMBERH(MagicNumber);
	pAttack.MagicNumberL = SET_NUMBERL(MagicNumber);
	pAttack.SourceNumberH = SET_NUMBERH(lpObj->m_Index);
	pAttack.SourceNumberL = SET_NUMBERL(lpObj->m_Index);
	pAttack.TargetNumberH = SET_NUMBERH(usernumber);
	pAttack.TargetNumberL = SET_NUMBERL(usernumber);
	pAttack.TargetNumberH &= 0x7F;

	if (skillsuccess == 1)
		pAttack.TargetNumberH |= 0x80;

	if (lpObj->Type == OBJ_USER)
		GIOCP.DataSend(lpObj->m_Index, (LPBYTE)&pAttack, pAttack.h.size);

	if (/*CC_MAP_RANGE(lpObj->MapNumber) ||*/ lpObj->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		if (g_MasterLevelSkillTreeSystem.CheckMasterLevelSkill(MagicNumber) == true)
		{
			switch (MagicNumber)
			{
			case 356:
			case 403:
			case 413:
			case 417:
			case 420:
				return;
			default:
				break;
			}
		}

		else
		{
			switch (MagicNumber)
			{
			case 16:
			case 26:
			case 27:
			case 28:
			case 48:
				return;
			default:
				break;
			}
		}
	}

	MsgSendV2(lpObj, (LPBYTE)&pAttack, pAttack.h.size);
}

void GameProtocol::GCMonkMagicAttack(CGameObject* lpObj, WORD MagicNumber, int usernumber, unsigned char skillsuccess)
{
	PMSG_RAGEATTACK pAttack;

	PHeadSetBE((LPBYTE)&pAttack, 0x4A, sizeof(pAttack));
	pAttack.MagicNumberH = SET_NUMBERH(MagicNumber);
	pAttack.MagicNumberL = SET_NUMBERL(MagicNumber);
	pAttack.SourceNumberH = SET_NUMBERH(lpObj->m_Index);
	pAttack.SourceNumberL = SET_NUMBERL(lpObj->m_Index);
	pAttack.TargetNumberH = SET_NUMBERH(usernumber);
	pAttack.TargetNumberL = SET_NUMBERL(usernumber);
	pAttack.TargetNumberH &= 0x7F;

	if (skillsuccess == 1)
		pAttack.TargetNumberH |= 0x80;

	if (lpObj->Type == OBJ_USER)
		GIOCP.DataSend(lpObj->m_Index, (LPBYTE)&pAttack, pAttack.h.size);

	MsgSendV2(lpObj, (LPBYTE)&pAttack, pAttack.h.size);
}

void GameProtocol::GCMagicCancelSend(CGameObject* lpObj, WORD MagicNumber)
{
	PMSG_MAGICCANCEL pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x1B, sizeof(pMsg));
	pMsg.MagicNumberL = SET_NUMBERL(MagicNumber);
	pMsg.MagicNumberH = SET_NUMBERH(MagicNumber);
	pMsg.NumberH = SET_NUMBERH(lpObj->m_Index);
	pMsg.NumberL = SET_NUMBERL(lpObj->m_Index);

	if (lpObj->Type == OBJ_USER)
		GIOCP.DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);

	MsgSendV2(lpObj, (UCHAR*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCUseEffectItem(CGameObject* lpObj, int iEffectUseOption, int iOptionType, int iEffectType, int iLeftTime)
{
	PMSG_USEEFFECTITEM pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x2D, sizeof(pMsg));
	pMsg.byEffectOption = iEffectUseOption;
	pMsg.wOptionType = iOptionType;
	pMsg.wEffectType = iEffectType;
	pMsg.iLeftTime = iLeftTime;
	pMsg.btBuffEffectType = 0;
	//	pMsg.dwEffectValue = 0;

	if (lpObj->Type == OBJ_USER)
		GIOCP.DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);
}

void GameProtocol::CGMagicCancel(PMSG_MAGICCANCEL * lpMsg, CGameObject* lpObj)
{
	WORD MagicNumber = MAKE_NUMBERW(lpMsg->MagicNumberL, lpMsg->MagicNumberH);

	switch (MagicNumber)
	{
	case 77:
		gObjRemoveBuffEffect(*lpObj, 6);
		break;
	case 233:
		gObjRemoveBuffEffect(*lpObj, 82);
		break;
	case 380:
		gObjRemoveBuffEffect(*lpObj, 138);
		break;
	case 383:
		gObjRemoveBuffEffect(*lpObj, 139);
		break;
	case 441:
		gObjRemoveBuffEffect(*lpObj, 143);
		break;
	}
}

struct PMSG_USE_MONSTERSKILL
{
	PBMSG_HEAD2 h;	// C1:69
	WORD btMonsterSkillNumber;	// 3
	WORD wObjIndex;	// 4
	WORD wTargetObjIndex;	// 6
};

void GameProtocol::GCUseMonsterSkillSend(CGameObject* lpObj, CGameObject* lpTargetObj, int iSkillNumber)
{
	PMSG_USE_MONSTERSKILL pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0x69, 0x00, sizeof(pMsg));
	pMsg.btMonsterSkillNumber = iSkillNumber;
	pMsg.wObjIndex = lpObj->m_Index;
	pMsg.wTargetObjIndex = lpTargetObj->m_Index;

	if (lpObj->Type == OBJ_USER)
		GIOCP.DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);

	MsgSendV2(lpObj, (UCHAR*)&pMsg, pMsg.h.size);
}

struct PMSG_VIEWSKILLSTATE
{
	PBMSG_HEAD h;	// C1:07
	BYTE State;	// 3
	BYTE NumberH;	// 4
	BYTE NumberL;	// 5
	BYTE BuffEffect; // 6
};

void GameProtocol::GCStateInfoSend(CGameObject* lpObj, BYTE state, int BuffViewport)
{
	PMSG_VIEWSKILLSTATE pMsg;

	BYTE BuffCount = 0;

	PHeadSetB((LPBYTE)&pMsg, 0x07, sizeof(pMsg));

	pMsg.State = state;

	pMsg.NumberH = HIBYTE(lpObj->m_Index);
	pMsg.NumberL = LOBYTE(lpObj->m_Index);

	pMsg.BuffEffect = BuffViewport;

	if (lpObj->Type == OBJ_USER)
		GIOCP.DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);

	//if (CC_MAP_RANGE(lpObj->MapNumber) || lpObj->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)	return;

	MsgSendV2(lpObj, (LPBYTE)&pMsg, pMsg.h.size);
}

void GameProtocol::CGTeleportRecv(PMSG_TELEPORT* lpMsg, CGameObject* lpObj)
{
	if (gObjCheckUsedBuffEffect(*lpObj, BUFFTYPE_STUN) == TRUE || gObjCheckUsedBuffEffect(*lpObj, BUFFTYPE_SLEEP) == TRUE
		|| gObjCheckUsedBuffEffect(*lpObj, BUFFTYPE_FREEZE_2) || gObjCheckUsedBuffEffect(*lpObj, BUFFTYPE_EARTH_BINDS))
	{
		PMSG_TELEPORT_RESULT pTeleportResult;

		pTeleportResult.h.c = 0xC3;
		pTeleportResult.h.size = sizeof(pTeleportResult);
		pTeleportResult.h.headcode = 0x1C;
		pTeleportResult.Type = 0;
		pTeleportResult.Unk = 0;
		pTeleportResult.MapNumber = lpObj->MapNumber;
		pTeleportResult.Unk1 = 0;
		pTeleportResult.MapX = lpObj->X;
		pTeleportResult.MapY = lpObj->Y;
		pTeleportResult.Dir = lpObj->Dir;

		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pTeleportResult, pTeleportResult.h.size);
		return;
	}

	if (lpObj->m_PlayerData->m_bMapSvrMoveReq == true || lpObj->m_PlayerData->m_bMapSvrMoveReq_2 == true || lpObj->m_PlayerData->m_bMapSvrMoveQuit == true || lpObj->m_State == 32)
	{
		return;
	}

	/*if (IT_MAP_RANGE(lpObj->MapNumber))
	{
		if (g_IT_Event.GetIllusionTempleState(lpObj->MapNumber) == 2)
		{
			if (g_IT_Event.CheckRelics(lpObj) != FALSE)
			{
				PMSG_TELEPORT_RESULT pTeleportResult;

				pTeleportResult.h.c = 0xC3;
				pTeleportResult.h.size = sizeof(pTeleportResult);
				pTeleportResult.h.headcode = 0x1C;
				pTeleportResult.Type = 0;
				pTeleportResult.Unk = 0;
				pTeleportResult.MapNumber = lpObj->MapNumber;
				pTeleportResult.Unk1 = 0;
				pTeleportResult.MapX = lpObj->X;
				pTeleportResult.MapY = lpObj->Y;
				pTeleportResult.Dir = lpObj->Dir;

				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pTeleportResult, pTeleportResult.h.size);
				return;
			}

			if (g_IT_Event.CheckSkillRestraint(lpObj->m_nITR_Index, lpObj->MapNumber) != FALSE)
			{
				PMSG_TELEPORT_RESULT pTeleportResult;

				pTeleportResult.h.c = 0xC3;
				pTeleportResult.h.size = sizeof(pTeleportResult);
				pTeleportResult.h.headcode = 0x1C;
				pTeleportResult.Type = 0;
				pTeleportResult.Unk = 0;
				pTeleportResult.MapNumber = lpObj->MapNumber;
				pTeleportResult.Unk1 = 0;
				pTeleportResult.MapX = lpObj->X;
				pTeleportResult.MapY = lpObj->Y;
				pTeleportResult.Dir = lpObj->Dir;

				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pTeleportResult, pTeleportResult.h.size);
				return;
			}
		}
	}*/

	/*if (IT_MAP_RANGE(lpObj->MapNumber))
	{
		if (g_IT_Event.GetIllusionTempleState(lpObj->MapNumber) == 1)
		{
			PMSG_TELEPORT_RESULT pTeleportResult;

			pTeleportResult.h.c = 0xC3;
			pTeleportResult.h.size = sizeof(pTeleportResult);
			pTeleportResult.h.headcode = 0x1C;
			pTeleportResult.Type = 0;
			pTeleportResult.Unk = 0;
			pTeleportResult.MapNumber = lpObj->MapNumber;
			pTeleportResult.Unk1 = 0;
			pTeleportResult.MapX = lpObj->X;
			pTeleportResult.MapY = lpObj->Y;
			pTeleportResult.Dir = lpObj->Dir;

			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pTeleportResult, pTeleportResult.h.size);
			return;
		}
	}*/

	/*if (CC_MAP_RANGE(lpObj->MapNumber) != FALSE) //season 4.0 add-on (fix)
	{
		if (g_ChaosCastle.GetCurrentState(lpObj->m_cChaosCastleIndex) == 3)
		{
			PMSG_TELEPORT_RESULT pTeleportResult;

			pTeleportResult.h.c = 0xC3;
			pTeleportResult.h.size = sizeof(pTeleportResult);
			pTeleportResult.h.headcode = 0x1C;
			pTeleportResult.Type = 0;
			pTeleportResult.Unk = 0;
			pTeleportResult.MapNumber = lpObj->MapNumber;
			pTeleportResult.Unk1 = 0;
			pTeleportResult.MapX = lpObj->X;
			pTeleportResult.MapY = lpObj->Y;
			pTeleportResult.Dir = lpObj->Dir;
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pTeleportResult, pTeleportResult.h.size);
			return;
		}
	}

	if (lpObj->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL && g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		if (g_ChaosCastleFinal.GetCurrentState() == 3)
		{
			PMSG_TELEPORT_RESULT pTeleportResult;

			pTeleportResult.h.c = 0xC3;
			pTeleportResult.h.size = sizeof(pTeleportResult);
			pTeleportResult.h.headcode = 0x1C;
			pTeleportResult.Type = 0;
			pTeleportResult.Unk = 0;
			pTeleportResult.MapNumber = lpObj->MapNumber;
			pTeleportResult.Unk1 = 0;
			pTeleportResult.MapX = lpObj->X;
			pTeleportResult.MapY = lpObj->Y;
			pTeleportResult.Dir = lpObj->Dir;
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pTeleportResult, pTeleportResult.h.size);
			return;
		}
	}*/

	/*if (DG_MAP_RANGE(lpObj->MapNumber) != FALSE)
	{
		if (g_DoppelGanger.GetDoppelgangerState() != 2)
		{
			PMSG_TELEPORT_RESULT pTeleportResult;

			pTeleportResult.h.c = 0xC3;
			pTeleportResult.h.size = sizeof(pTeleportResult);
			pTeleportResult.h.headcode = 0x1C;
			pTeleportResult.Type = 0;
			pTeleportResult.Unk = 0;
			pTeleportResult.MapNumber = lpObj->MapNumber;
			pTeleportResult.Unk1 = 0;
			pTeleportResult.MapX = lpObj->X;
			pTeleportResult.MapY = lpObj->Y;
			pTeleportResult.Dir = lpObj->Dir;
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pTeleportResult, pTeleportResult.h.size);
			return;
		}
	}*/

	if (lpMsg->MoveNumber == 0)
	{
		BYTE x = lpMsg->MapX;
		BYTE y = lpMsg->MapY;

		if (gObjCheckTeleportArea(*lpObj, x, y) == FALSE)
		{

			PMSG_TELEPORT_RESULT pTeleportResult;

			pTeleportResult.h.c = 0xC3;
			pTeleportResult.h.size = sizeof(pTeleportResult);
			pTeleportResult.h.headcode = 0x1C;
			pTeleportResult.Type = 0;
			pTeleportResult.Unk = 0;
			pTeleportResult.MapNumber = lpObj->MapNumber;
			pTeleportResult.Unk1 = 0;
			pTeleportResult.MapX = lpObj->X;
			pTeleportResult.MapY = lpObj->Y;
			pTeleportResult.Dir = lpObj->Dir;

			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pTeleportResult, pTeleportResult.h.size);
			return;
		}

		/*if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
		{
			if (lpObj->MapNumber == MAP_INDEX_CASTLESIEGE)
			{
				if (g_CastleSiege.CheckTeleportMagicAxisY(lpObj->Y, x, y) == FALSE)
				{
					y = lpObj->Y;
				}
			}
		}*/

		CMagicInf * lpMagic;
		PMSG_MAGICATTACK_RESULT pAttack;
		PHeadSetBE((LPBYTE)&pAttack, 0x19, sizeof(pAttack));
		pAttack.MagicNumberH = SET_NUMBERH(AT_SKILL_TELEPORT);
		pAttack.MagicNumberL = SET_NUMBERL(AT_SKILL_TELEPORT);
		pAttack.SourceNumberH = SET_NUMBERH(lpObj);
		pAttack.SourceNumberL = SET_NUMBERL(lpObj);
		pAttack.TargetNumberH = SET_NUMBERH(lpObj);
		pAttack.TargetNumberL = SET_NUMBERL(lpObj);

		lpMagic = gObjGetMagicSearch(*lpObj, AT_SKILL_TELEPORT);

		if (lpMagic)
		{
			int usemana = gObjMagicManaUse(*lpObj, lpMagic);

			if (usemana >= 0)
			{
				int usebp = gObjMagicBPUse(*lpObj, lpMagic);

				if (usebp >= 0)
				{
					if (lpObj->Type == OBJ_USER)
						GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pAttack, pAttack.h.size);

					MsgSendV2(lpObj, (LPBYTE)&pAttack, pAttack.h.size);
					gObjTeleportMagicUse(*lpObj, x, y);
					lpObj->Mana = usemana;
					lpObj->BP = usebp;
					GCManaSend(lpObj, lpObj->Mana, -1, 0, lpObj->BP);
				}
			}
		}
	}

	if (IMPERIAL_MAP_RANGE(lpObj->MapNumber) == TRUE)
	{
		int nCurrentZoneIndex = g_ImperialGuardian.GetCurrentZoneIndex(*lpObj);

		if (nCurrentZoneIndex >= 0)
		{
			g_ImperialGuardian.CGEnterPortal(*lpObj, nCurrentZoneIndex + 1);
		}
	}

	if (lpObj->m_IfState.use == 1 && lpObj->m_IfState.type == 6)
	{
		return;
	}

	if (gGateC.IsInGate(*lpObj, lpMsg->MoveNumber))
	{
		gObjMoveGate(*lpObj, lpMsg->MoveNumber);
		return;
	}

	gObjClearViewport(*lpObj);
	GCTeleportSend(lpObj, lpMsg->MoveNumber, lpObj->MapNumber, lpObj->X, lpObj->Y, lpObj->Dir);

	/*if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (lpObj->MapNumber == MAP_INDEX_CASTLESIEGE)
		{
			if (g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
			{
				g_CastleSiege.NotifySelfCsJoinSide(*lpObj);
				g_CastleSiege.NotifyCsSelfLeftTime(*lpObj);
			}
		}
	}*/
}

struct PMSG_RESULT_MOVEDEVILSQUARE
{
	PBMSG_HEAD h;	// C1:90
	BYTE Result;	// 3
};

void GameProtocol::CGReqMapMove(PMSG_REQ_MAPMOVE *lpMsg, CGameObject* lpObj)
{
	PMSG_ANS_MAPMOVE pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0x8E, 0x03, sizeof(pMsg));

	pMsg.btResult = 0;

	if (lpObj->Type != OBJ_USER)
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	if (g_KeyGenerater.CheckKeyValue(lpObj->m_PlayerData->m_dwMapMoveKeyValue, lpMsg->dwBlockKey) == FALSE)
	{
		pMsg.btResult = 12;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	if (lpObj->m_PlayerData->ConnectUser->Connected < PLAYER_PLAYING)
	{
		pMsg.btResult = 11;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	if (lpObj->Teleport != 0)
	{
		pMsg.btResult = 2;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	if (lpObj->m_PlayerData->m_bSecurityCheck == false)
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	if (lpObj->m_PlayerData->m_bMapSvrMoveReq == true || lpObj->m_PlayerData->m_bMapSvrMoveReq_2 == true || lpObj->m_PlayerData->m_bMapSvrMoveQuit == true || lpObj->m_State == 32)
	{
		return;
	}

	if (lpObj->m_bPShopOpen == true)
	{
		if (lpObj->MapNumber == MAP_INDEX_CASTLESIEGE && lpMsg->wMapIndex == MAP_INDEX_RORENCIA)
		{
			pMsg.btResult = 3;
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
			return;
		}

		if (lpObj->MapNumber == MAP_INDEX_RORENCIA && lpMsg->wMapIndex == MAP_INDEX_CASTLESIEGE)
		{
			pMsg.btResult = 3;
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
			return;
		}
	}

	if (lpObj->m_IfState.use == 1 && lpObj->m_IfState.type == 6)
	{
		pMsg.btResult = 2;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	if (lpObj->DieRegen == TRUE)
	{
		pMsg.btResult = 2;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	if (lpObj->RegenOk != FALSE)
	{
		pMsg.btResult = 2;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	if (pMsg.btResult)
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
	gMoveCommand.Move(*lpObj, lpMsg->wMapIndex);
}

void GameProtocol::GCSendMapMoveCheckSum(CGameObject* lpObj)
{
	PMSG_MAPMOVE_CHECKSUM pMsg;

	pMsg.dwKeyValue = lpObj->m_PlayerData->m_dwMapMoveKeyValue;
	PHeadSubSetB((LPBYTE)&pMsg, 0x8E, 0x01, sizeof(pMsg));

	GIOCP.DataSend(lpObj->m_Index, (LPBYTE)&pMsg, pMsg.h.size);
}

void GameProtocol::CGTargetTeleportRecv(PMSG_TARGET_TELEPORT * lpMsg, CGameObject* lpObj)
{
	if (gObjCheckUsedBuffEffect(*lpObj, BUFFTYPE_STONE) || gObjCheckUsedBuffEffect(*lpObj, BUFFTYPE_STUN) || gObjCheckUsedBuffEffect(*lpObj, BUFFTYPE_SLEEP) ||
		gObjCheckUsedBuffEffect(*lpObj, BUFFTYPE_FREEZE_2) || gObjCheckUsedBuffEffect(*lpObj, BUFFTYPE_EARTH_BINDS))
		return;

	CMagicInf * lpMagic;
	BYTE x;
	BYTE y;
	x = lpMsg->MapX;
	y = lpMsg->MapY;
	int iTargetIndex = MAKE_NUMBERW(lpMsg->NumberL, lpMsg->NumberH);	// #error? parameters inverse?
	CGameObject* lpTargetObj = getGameObject(iTargetIndex);
	if (!gObjIsConnectedGP(*lpTargetObj))
	{
		return;
	}

	/*if (IT_MAP_RANGE(lpTargetObj->MapNumber))
	{
		if (g_IT_Event.CheckRelics(lpTargetObj))
		{
			return;
		}
	}*/

	if (lpObj->PartyNumber != lpTargetObj->PartyNumber ||
		lpObj->PartyNumber == -1 ||
		lpTargetObj->PartyNumber == -1)
	{
		return;
	}

	if (!gObjCheckTeleportArea(*lpTargetObj, x, y))
	{
		return;
	}

	PMSG_MAGICATTACK_RESULT pAttack;

	PHeadSetBE((LPBYTE)&pAttack, 0x19, sizeof(pAttack));
	WORD MagicNumber = AT_SKILL_TARGET_TELEPORT;
	pAttack.MagicNumberH = SET_NUMBERH(MagicNumber);
	pAttack.MagicNumberL = SET_NUMBERL(MagicNumber);
	pAttack.SourceNumberH = SET_NUMBERH(lpObj);
	pAttack.SourceNumberL = SET_NUMBERL(lpObj);
	pAttack.TargetNumberH = SET_NUMBERH(iTargetIndex);	// #error? parameters inverse?
	pAttack.TargetNumberL = SET_NUMBERL(iTargetIndex);	// #error? parameters inverse?
	lpMagic = gObjGetMagicSearch(*lpObj, AT_SKILL_TARGET_TELEPORT);

	if (lpMagic)
	{
		int usemana = gObjMagicManaUse(*lpObj, lpMagic);

		if (usemana >= 0)
		{
			int usebp = gObjMagicBPUse(*lpObj, lpMagic);

			if (usebp >= 0)
			{
				if (lpObj->Type == OBJ_USER)
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pAttack, pAttack.h.size);

				MsgSendV2(lpObj, (LPBYTE)&pAttack, pAttack.h.size);
				gObjTeleportMagicUse(*lpTargetObj, x, y);
				lpObj->Mana = usemana;
				lpObj->BP = usebp;
				GCManaSend(lpObj, lpObj->Mana, -1, 0, lpObj->BP);

				return;
			}
		}
	}
}




void GameProtocol::GCTeleportSend(CGameObject* lpObj, WORD MoveNumber, BYTE MapNumber, BYTE MapX, BYTE MapY, BYTE Dir)
{
	PMSG_TELEPORT_RESULT pMsg;

	if (lpObj->Type != OBJ_USER)
		return;

	pMsg.h.c = 0xC3;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x1C;
	if (MoveNumber > 0)
	{
		pMsg.Type = 256; // Warp
	}
	else
	{
		pMsg.Type = 0; // Teleportacja
	}
	pMsg.MapNumber = MapNumber;
	pMsg.Unk1 = 0;
	pMsg.Unk = 0;
	pMsg.MapX = MapX;
	pMsg.MapY = MapY;
	pMsg.Dir = Dir;
	// mapserver

	if (MoveNumber == 0)
		GIOCP.DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);
	else
		GIOCP.DataSend(lpObj->m_Index, (UCHAR*)&pMsg, pMsg.h.size);
}

#include "SkillHitBox.h"

void GameProtocol::CGBeattackRecv(unsigned char* lpRecv, CGameObject* lpObj, int magic_send)
{
	// Set the lpRecv to a Predeterminated Packet
	PMSG_BEATTACK_COUNT * lpCount = (PMSG_BEATTACK_COUNT *)lpRecv;

	// Check the Protocol
	if (lpCount->h.headcode != 0xD3)
	{
		return;
	}

	// Check if the count is leess than 1
	if (lpCount->Count < 1)
	{
		return;
	}

	// Set the Max limitation to 5
	if (lpCount->Count > 5)
	{
		lpCount->Count = 5;
	}

	// Set the Start of the Offset
	int lOfs = sizeof(PMSG_BEATTACK_COUNT);
	CMagicInf * lpMagic;
	int tNumber;
	PMSG_BEATTACK * lpMsg;

	// Set lpMagic according
	// if lpObj is OBJ_USER
	// or if it is OBJ_MONSTER
	if (lpObj->Type == OBJ_USER)
	{
		BYTE attr = MapC[lpObj->MapNumber].GetAttr(lpObj->X, lpObj->Y);

		if ((attr & 1) == 1) //safezone
		{
			gObjSetPosition(*lpObj, lpObj->X, lpObj->Y);
			return;
		}
		//season 4.5 add-on end

		WORD MagicNumber = MAKE_NUMBERW(lpCount->MagicNumberH, lpCount->MagicNumberL);

		lpMagic = gObjGetMagicSearch(*lpObj, MagicNumber);
	}
	else
	{
		WORD MagicNumber = MAKE_NUMBERW(lpCount->MagicNumberH, lpCount->MagicNumberL);

		lpMagic = gObjGetMagic(*lpObj, MagicNumber);
	}

	if (lpObj->Type == OBJ_USER && lpObj->m_PlayerData->m_bSecurityCheck == false)
		return;

	if (lpObj->Teleport && gObjUseSkill.m_SkillData.EnableWizardSkillWhileTeleport == false)
		return;

	// Check if there is Magic
	if (lpMagic == NULL)
	{
		return;
	}

	// Avoid use Skill of the following types
	if (lpMagic->m_Skill == 42 ||
		lpMagic->m_Skill == 43 ||
		lpMagic->m_Skill == 41 ||
		g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == 336 ||
		g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == 331 ||
		lpMagic->m_Skill == 333 ||
		lpMagic->m_Skill == 339 ||
		lpMagic->m_Skill == 342 ||
		lpMagic->m_Skill == 332 ||
		lpMagic->m_Skill == 330 ||
		lpMagic->m_Skill == AT_SKILL_FIRE_BLOW)
	{
		return;
	}

	if (lpObj->Type == OBJ_USER)
	{
		// Check if Player can use Magic
		// Acording to its Class
		if (MagicDamageC.SkillGetRequireClass(lpObj->Class,
			lpObj->m_PlayerData->ChangeUP,
			lpMagic->m_Skill) < 1)
		{
			// It's not his magic [%s][%s] .
			gObjUserKill(*lpObj);
			return;
		}

		// Check the time cast Spell
		// No longer that 8 seconds
		if ((GetTickCount() - lpObj->UseMagicTime) > 8000)
		{
			sLog->outError("Too long time passed after casting magic [%s][%s] (%d)(%d)",
				lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpMagic->m_Skill, GetTickCount() - lpObj->UseMagicTime);

			return;
		}

		// Check if user cast to much Spells
		// Npo longer thant 4
		if (lpObj->UseMagicCount > 4)
		{
			return;
		}

		lpObj->UseMagicCount++;
	}

	// Avoid Attack out of the predeterminated
	// Magic need to be one of the followingla
	//	
	//	8	"Twister"
	//	9	"Evil Spirit"
	//	10	"Hellfire"
	//	13	"Blast"
	//	14	"Inferno"
	//	5	"Flame"
	//	24	"Triple Shot"
	//	50	"Flame of Evil(Monster)"
	//	12	"Aquaflash"
	//	41	"Twisting Slash"
	//	47	"Impale"
	//	43	"Death Stab"
	//	42	"Rageful Blow"
	//	52	"Penetration"
	//	55	"Fire Slash"
	//	78	"#Unknown"
	//  Master Triple Shot
	//  Master CometFall
	//  Master FireSlash
	//  235 "Five Shot"

	if (g_MasterLevelSkillTreeSystem.CheckMasterLevelSkill(lpMagic->m_Skill))
	{
		int iBaseMLS = g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill);

		if (iBaseMLS != 403
			&& iBaseMLS != 388
			&& iBaseMLS != 330
			&& iBaseMLS != 413
			&& iBaseMLS != 414
			&& iBaseMLS != 481
			&& iBaseMLS != 356
			&& iBaseMLS != 382
			&& iBaseMLS != 512
			&& iBaseMLS != 508
			&& iBaseMLS != 385
			&& iBaseMLS != 391
			&& iBaseMLS != 336
			&& iBaseMLS != 331
			&& iBaseMLS != 417
			&& iBaseMLS != 420
			&& iBaseMLS != 490
			&& iBaseMLS != 482
			&& iBaseMLS != 518
			&& iBaseMLS != 487
			&& iBaseMLS != 326
			&& iBaseMLS != 327
			&& iBaseMLS != 328
			&& iBaseMLS != AT_MSKILL_DK_LUNGE1
			&& iBaseMLS != 332
			&& iBaseMLS != 333
			&& iBaseMLS != 337
			&& iBaseMLS != 339
			&& iBaseMLS != 340
			&& iBaseMLS != 342
			&& iBaseMLS != 343
			&& iBaseMLS != 344
			&& iBaseMLS != 346
			&& iBaseMLS != 360
			&& iBaseMLS != 363
			&& iBaseMLS != 378
			&& iBaseMLS != 379
			&& iBaseMLS != AT_MSKILL_DW_EXPWIZARDRY1
			&& iBaseMLS != 381
			&& iBaseMLS != AT_MSKILL_DW_EXPWIZARDRY2
			&& iBaseMLS != 384
			&& iBaseMLS != 386
			&& iBaseMLS != 387
			&& iBaseMLS != 389
			&& iBaseMLS != 392
			&& iBaseMLS != 393
			&& iBaseMLS != 404
			&& iBaseMLS != 406
			&& iBaseMLS != 415
			&& iBaseMLS != 416
			&& iBaseMLS != 418
			&& iBaseMLS != 419
			&& iBaseMLS != 425
			&& iBaseMLS != 426
			&& iBaseMLS != 427
			&& iBaseMLS != 428
			&& iBaseMLS != 429
			&& iBaseMLS != 430
			&& iBaseMLS != 432
			&& iBaseMLS != 433
			&& iBaseMLS != 441
			&& iBaseMLS != 459
			&& iBaseMLS != 460
			&& iBaseMLS != 461
			&& iBaseMLS != 462
			&& iBaseMLS != 466
			&& iBaseMLS != 468
			&& iBaseMLS != AT_MSKILL_SUM_BERSERK1
			&& iBaseMLS != 470
			&& iBaseMLS != 472
			&& iBaseMLS != 479
			&& iBaseMLS != 480
			&& iBaseMLS != 483
			&& iBaseMLS != 484
			&& iBaseMLS != 486
			&& iBaseMLS != 488
			&& iBaseMLS != 489
			&& iBaseMLS != 492
			&& iBaseMLS != 493
			&& iBaseMLS != 494
			&& iBaseMLS != 495
			&& iBaseMLS != 496
			&& iBaseMLS != 497
			&& iBaseMLS != 509
			&& iBaseMLS != 510
			&& iBaseMLS != 514
			&& iBaseMLS != 515
			&& iBaseMLS != 516
			&& iBaseMLS != 519
			&& iBaseMLS != 520
			&& iBaseMLS != 523
			&& iBaseMLS != 539
			&& iBaseMLS != 551
			&& iBaseMLS != 552
			&& iBaseMLS != 554
			&& iBaseMLS != 555
			&& iBaseMLS != 556
			&& iBaseMLS != 558
			&& iBaseMLS != 559
			&& iBaseMLS != 560
			&& iBaseMLS != 561
			&& iBaseMLS != 562
			&& iBaseMLS != 563
			&& iBaseMLS != 564
			&& iBaseMLS != 565
			&& iBaseMLS != 566
			&& iBaseMLS != 567
			&& iBaseMLS != 575
			&& iBaseMLS != 577
			&& iBaseMLS != 411
			&& iBaseMLS != 424
			&& iBaseMLS != 431
			&& iBaseMLS != 463
			&& iBaseMLS != 491
			&& iBaseMLS != 522
			&& iBaseMLS != 521
			&& iBaseMLS != 524
			&& iBaseMLS != 551
			&& iBaseMLS != 552
			&& iBaseMLS != 554
			&& iBaseMLS != 555
			&& iBaseMLS != 556
			&& iBaseMLS != 558
			&& iBaseMLS != 559
			&& iBaseMLS != 560
			&& iBaseMLS != 561
			&& iBaseMLS != 562
			&& iBaseMLS != 563
			&& iBaseMLS != 564
			&& iBaseMLS != 565
			&& iBaseMLS != 566
			&& iBaseMLS != 567
			&& iBaseMLS != 575
			&& iBaseMLS != 577
			&& iBaseMLS != 631
			&& iBaseMLS != AT_MSKILL_OBSIDIAN_POWUP
			&& iBaseMLS != AT_MSKILL_CIRCLE_SHIELD_POWUP
			&& iBaseMLS != AT_MSKILL_CIRCLE_SHIELD_MASTERY
			&& iBaseMLS != AT_MSKILL_WRATH_POWUP
			&& iBaseMLS != AT_MSKILL_WRATH_PROFICIENCY
			&& iBaseMLS != AT_MSKILL_WRATH_MASTERY)
		{
			return;
		}
	}

	else
	{
		if (lpMagic->m_Skill != 8
			&& lpMagic->m_Skill != 9
			&& lpMagic->m_Skill != 10
			&& lpMagic->m_Skill != 13
			&& lpMagic->m_Skill != 14
			&& lpMagic->m_Skill != 5
			&& lpMagic->m_Skill != 24
			&& lpMagic->m_Skill != 50
			&& lpMagic->m_Skill != 12
			&& lpMagic->m_Skill != 41
			&& lpMagic->m_Skill != 47
			&& lpMagic->m_Skill != 43
			&& lpMagic->m_Skill != 42
			&& lpMagic->m_Skill != 52
			&& lpMagic->m_Skill != 55
			&& lpMagic->m_Skill != 78
			&& lpMagic->m_Skill != 277) // Penetration
		{
			return;
		}
	}

	if (lpMagic->m_Skill == 24 || lpMagic->m_Skill == 78 || g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == 414 || g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == 518
		|| lpMagic->m_Skill == 418 || lpMagic->m_Skill == 520)
	{
		int NSAttackSerial = lpCount->Serial;

		if (lpObj->OSAttackSerial >= 255 && (lpObj->OSAttackSerial - lpCount->Serial) > 50)
		{
			lpObj->OSAttackSerial = -1;
		}

		if (NSAttackSerial > lpObj->OSAttackSerial)
		{
			lpObj->OSAttackSerial = NSAttackSerial;
		}
	}

	if (lpMagic->m_Skill == AT_SKILL_EVIL ||
		g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == 385 ||
		g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == 487)
	{
		if (lpObj->Class != CLASS_WIZARD && lpObj->Class != CLASS_MAGUMSA)
		{
			return;
		}
	}

	if (bIsIgnorePacketSpeedHackDetect)
	{
		if (lpObj->m_SpeedHackPenalty > 0)
		{
			lpObj->m_SpeedHackPenalty--;
			return;
		}
	}

	int lOfs2 = lOfs;
	int pTargetNumber[128];

	for (int i = 0; i < lpCount->Count; i++)
	{
		lpMsg = (PMSG_BEATTACK *)&lpRecv[lOfs2];
		pTargetNumber[i] = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
		lOfs2 += sizeof(PMSG_BEATTACK);

		if (i >= 1)
		{
			int nTargetDistance = gObjCalDistanceTX(*getGameObject(pTargetNumber[0]), *getGameObject(pTargetNumber[i]));

			if (getGameObject(pTargetNumber[i])->Type == OBJ_MONSTER && nTargetDistance > 10)
			{
				sLog->outBasic("User Hack Detected [%s][%s] Skill: %d, Monster Distance: %d, Cnt:%d",
					lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpMagic->m_Skill, nTargetDistance, lpCount->Count);
			}
		}
	}

	for (int n = 0; n < lpCount->Count; n++)
	{
		lpMsg = (PMSG_BEATTACK *)&lpRecv[lOfs];
		tNumber = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

		if (!lpMsg->MagicKey && (lpMagic->m_Skill == AT_SKILL_EVIL || g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == 385 || g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == 487))	// "Evil Spirit"
		{
			return;
		}

		if (lpMsg->MagicKey && (lpMagic->m_Skill == AT_SKILL_EVIL || g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == 385 || g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == 487))	// "Evil Spirit"
		{
			if (lpObj->DurMagicKeyChecker->IsValidDurationTime(lpMsg->MagicKey) == FALSE)
			{
				lOfs += sizeof(PMSG_BEATTACK);

				continue;
			}

			if (lpObj->DurMagicKeyChecker->IsValidCount(lpMsg->MagicKey) == FALSE)
			{
				lOfs += sizeof(PMSG_BEATTACK);

				continue;
			}
		}

		if (gWriteSkillLog)
		{
			sLog->outBasic("Magic Attack3 : %d, serial = %d, Tgt =  %d, cnt = %d",
				lpMagic->m_Skill, lpCount->Serial, tNumber, lpCount->Count);
		}

		int DistanceCheck = gObjCalDistance(*lpObj, *getGameObject(tNumber));

		if (DistanceCheck > 13)
		{
			lOfs += sizeof(PMSG_BEATTACK);
			continue;
		}
		//season4 add-on end

		for (int i = 0; i < lpCount->Count; i++)
		{
			if (n != i)
			{
				if (pTargetNumber[i] == tNumber)
				{
					//this->GCSendDisableReconnect(lpObj->m_Index);
					GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
					return;
				}
			}
		}


		CGameObject* lpObjTarget = getGameObject(tNumber);
		if (lpMagic->m_Skill == AT_SKILL_DEFENSEDOWN)
		{
			gObjUseSkill.MaGumSkillDefenseDown(*lpObj, tNumber, lpMagic->m_Level);
			gObjAttack.Attack(*lpObj, *lpObjTarget, lpMagic, TRUE, 1, 0, FALSE, 0, 0, 0);
		}

		else if (lpMagic->m_Skill == AT_SKILL_FIRESCREAM)
		{
			gObjUseSkill.SkillFireScream(*lpObj, tNumber, lpMagic);
		}

		else if (g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == AT_MSKILL_MG_FIRESLASH1)
		{
			g_MasterLevelSkillTreeSystem.MLS_MaGumSkillDefenseDown(*lpObj, tNumber);
			gObjAttack.Attack(*lpObj, *lpObjTarget, lpMagic, TRUE, 1, 0, FALSE, 0, 0, 0);
		}

		else if (g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == AT_MSKILL_MG_FIRESLASH2)
		{
			g_MasterLevelSkillTreeSystem.MLS_SkillMasteryDefenseDown(*lpObj, tNumber, lpMagic);
			gObjAttack.Attack(*lpObj, *lpObjTarget, lpMagic, TRUE, 1, 0, FALSE, 0, 0, 0);
		}

		else if (lpMagic->m_Skill == AT_MSKILL_DL_FIRESCREAM1 || lpMagic->m_Skill == AT_MSKILL_DL_FIRESCREAM2)
		{
			g_MasterLevelSkillTreeSystem.MLS_FireScream(*lpObj, tNumber, lpMagic);
		}
		else if (lpMagic->m_Skill == AT_SKILL_SHINING_PEAK)
		{
			gObjUseSkill.SkillShiningPeak(*lpObj, lpMagic, tNumber);
		}
		else if (magic_send)
		{
			gObjAttack.Attack(*lpObj, *lpObjTarget, lpMagic, TRUE, 1, 0, FALSE, 0, 0, 0);
		}
		else
		{
			gObjAttack.Attack(*lpObj, *lpObjTarget, lpMagic, FALSE, 1, 0, FALSE, 0, 0, 0);
		}

		lOfs += sizeof(PMSG_BEATTACK);
	}
}

void GameProtocol::CGDurationMagicRecv(LPBYTE lpRecv, CGameObject* lpObj)
{

	PMSG_DURATION_MAGIC_RECV * lpMsg = (PMSG_DURATION_MAGIC_RECV *)(lpRecv);

	CMagicInf * lpMagic;

	if (lpObj->Teleport && gObjUseSkill.m_SkillData.EnableWizardSkillWhileTeleport == false)
	{
		return;
	}

	WORD MagicNumber = MAKE_NUMBERW(lpMsg->MagicNumberH, lpMsg->MagicNumberL);
	if (lpObj->Type == OBJ_USER)
	{
		lpMagic = gObjGetMagicSearch(*lpObj, MagicNumber);
	}
	else
	{
		lpMagic = gObjGetMagic(*lpObj, MagicNumber);
	}

	if (lpMagic == NULL)
	{
		return;
	}

	if (lpObj->Type == OBJ_USER)
	{
		if (MagicDamageC.CheckStatus(lpMagic->m_Skill, lpObj->m_PlayerData->GuildStatus) == FALSE)
		{
			return;
		}

		if (MagicDamageC.CheckKillCount(lpMagic->m_Skill, lpObj->m_btKillCount) < 0)
		{
			return;
		}

		if (lpObj->m_PlayerData->m_bSecurityCheck == false)
		{
			return;
		}
	}

	lpObj->UseMagicNumber = 0;

	if (lpMagic->m_Skill != 24)
	{
		if (!gObjUseSkill.SpeedHackCheck(*lpObj))
		{
			return;
		}
	}

	if (!gObjUseSkill.EnableSkill(lpMagic->m_Skill))
	{
		return;
	}

	int aTargetIndex = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	gObjUseSkill.UseSkill(*lpObj, lpMagic, lpMsg->X, lpMsg->Y, lpMsg->Dir, lpMsg->TargetPos, aTargetIndex);

	if (lpMsg->MagicKey)
		lpObj->DurMagicKeyChecker->SetDurationTime(lpMsg->MagicKey, GetTickCount());
}

void GameProtocol::CGUseItemRecv(PMSG_USEITEM* lpMsg, CGameObject* lpObj)
{
	int pos;
	CItemObject* citem;
	int iItemUseType = lpMsg->btItemUseType;

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	// Check User States
	if (lpObj->m_IfState.use && lpObj->m_IfState.type != 3)
	{
		GCReFillSend(lpObj, lpObj->Life, 0xFD, 1, lpObj->iShield);

		return;
	}

	if (lpObj->m_PlayerData->m_bSecurityCheck == false)
	{
		GCReFillSend(lpObj, lpObj->Life, 0xFD, 1, lpObj->iShield);
		MsgOutput(*lpObj, Lang.GetText(0, 822));
		return;
	}

	// Check if User is exiting
	if (lpObj->m_PlayerData->ConnectUser->CloseType != -1)
	{
		GCReFillSend(lpObj, lpObj->Life, 0xFD, 1, lpObj->iShield);
		return;
	}

	// Fix Inventory Pointer
	if (gObjFixInventoryPointer(*lpObj) == false)
	{
		sLog->outBasic("[Fix Inv.Ptr] False Location - %s, %d",
			__FILE__, __LINE__);
		//return;
	}

	// Check if there is a Transaction
	// Dupe
	if (lpObj->pTransaction == 1)
	{
		return;
	}

	if (g_NewPVP.IsObserver(*lpObj))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 319), lpObj, 1);
		return;
	}

	// Set Var Pos
	pos = lpMsg->inventoryPos;

	// Check if the pos is in bounds
	if (pos > MAIN_INVENTORY_SIZE - 1)
	{
		// error : Location is over the range. %s/%d
		GCReFillSend(lpObj, lpObj->Life, 0xFD, 1, lpObj->iShield);

		return;
	}

	// check if the item
	// have the same position
	// as the new position
	if (lpMsg->inventoryPos == lpMsg->invenrotyTarget)
	{
		GCReFillSend(lpObj, lpObj->Life, 0xFD, 1, lpObj->iShield);

		return;
	}

	// Set Pointer
	citem = lpObj->pntInventory[pos];
	citem->FixIt(); //K2
	//citem.FixIt;
	// The item is ITEM
	if (citem->IsItem())
	{
		// Check if item have 0 as Serial
		if (gObjCheckSerial0ItemList(citem))
		{
			MsgOutput(*lpObj, Lang.GetText(0, 259));

			GCReFillSend(lpObj, lpObj->Life, 0xFD, 1, lpObj->iShield);

			return;
		}

		// Check the item Serial
		if (gObjInventorySearchSerialNumber(*lpObj,
			citem->GetNumber()) == FALSE)
		{
			GCReFillSend(lpObj, lpObj->Life, 0xFD, 1, lpObj->iShield);
			return;
		}

		if (citem->m_serial && !gObjCanItemTouch(*lpObj, 1))
		{
			GCReFillSend(lpObj, lpObj->Life, 0xFD, 1, lpObj->iShield);
			return;
		}

		if (citem->m_Type == ITEMGET(14, 0) || citem->m_Type == ITEMGET(14, 1) || citem->m_Type == ITEMGET(14, 2) || citem->m_Type == ITEMGET(14, 3) || citem->m_Type == ITEMGET(14, 133))
		{
			int potion;
			for (potion = 0; potion < g_Michi.FixHackPotions.m_LifePotionMultiplyCount; potion++)
			{
				if (potion == 0 || (rand() % 100) < g_Michi.FixHackPotions.m_LifePotionMultiplyRate)
				{
					g_Michi.UseHealingPotion(citem, pos, *lpObj);
				}
			}
		}
		else if (citem->m_Type == ITEMGET(14, 4) ||	// Small Mana Potion
			citem->m_Type == ITEMGET(14, 5) || citem->m_Type == ITEMGET(14, 6))
		{
			int tMana = (citem->m_Value * 10) - (lpObj->Level);	// #formula

			if (tMana < 0)
			{
				tMana = 0;
			}

			switch (citem->m_Type)
			{
			case ITEMGET(14, 4):	// Small Mana Potion
				tMana += ((int)(lpObj->MaxMana + lpObj->AddMana)) * 20 / 100;
				break;
			case ITEMGET(14, 5):	// Mana Potion
				tMana += ((int)(lpObj->MaxMana + lpObj->AddMana)) * 30 / 100;
				break;
			case ITEMGET(14, 6):	// Large Mana Potion
				tMana += ((int)(lpObj->MaxMana + lpObj->AddMana)) * 40 / 100;
				break;
			}

			lpObj->Mana += tMana;

			if (lpObj->Mana > (lpObj->MaxMana + lpObj->AddMana - 1.0f))
				lpObj->Mana = lpObj->MaxMana + lpObj->AddMana;

			GCManaSend(lpObj, (WORD)lpObj->Mana, 0xFF, 0, lpObj->BP);

			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}
		}
		else if (citem->m_Type == ITEMGET(14, 36))
		{
			int iShieldGage = (lpObj->iMaxShield + lpObj->iAddShield) * 35 / 100;	// #formula

			if (lpObj->iShield < 0)
				lpObj->iShield = 0;

			if (lpObj->iFillShield > 0)
			{
				lpObj->iShield += lpObj->iFillShield;

				if (lpObj->iShield > (lpObj->iMaxShield + lpObj->iAddShield))
					lpObj->iShield = lpObj->iMaxShield + lpObj->iAddShield;
			}

			lpObj->iFillShieldCount = 2;
			lpObj->iFillShield = iShieldGage;
			lpObj->iFillShieldMax = iShieldGage;

			GCSendEffectInfo(lpObj, 3);

			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}
		}
		else if (citem->m_Type == ITEMGET(14, 35))
		{
			int iShieldGage = (lpObj->iMaxShield + lpObj->iAddShield) * 25 / 100;	// #formula

			if (lpObj->iShield < 0)
				lpObj->iShield = 0;

			if (lpObj->iFillShield > 0)
			{
				lpObj->iShield += lpObj->iFillShield;

				if (lpObj->iShield > (lpObj->iMaxShield + lpObj->iAddShield))
					lpObj->iShield = lpObj->iMaxShield + lpObj->iAddShield;
			}

			lpObj->iFillShieldCount = 2;
			lpObj->iFillShield = iShieldGage;
			lpObj->iFillShieldMax = iShieldGage;

			GCSendEffectInfo(lpObj, 3);

			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}
		}
		else if (citem->m_Type == ITEMGET(14, 37))
		{
			int iShieldGage = (lpObj->iMaxShield + lpObj->iAddShield) * 45 / 100; // #formula

			if (lpObj->iShield < 0)
				lpObj->iShield = 0;

			if (lpObj->iFillShield > 0)
			{
				lpObj->iShield += lpObj->iFillShield;

				if (lpObj->iShield > (lpObj->iMaxShield + lpObj->iAddShield))
					lpObj->iShield = lpObj->iMaxShield + lpObj->iAddShield;
			}

			lpObj->iFillShieldCount = 2;
			lpObj->iFillShield = iShieldGage;
			lpObj->iFillShieldMax = iShieldGage;

			GCSendEffectInfo(lpObj, 3);

			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}
		}
		else if (citem->m_Type == ITEMGET(14, 38))	// Small Compound Potion 
		{
			int iShieldGage = (lpObj->iMaxShield + lpObj->iAddShield) * 5 / 100;	// #formula
			int iHPGage = (int)((lpObj->MaxLife + lpObj->AddLife) * 10.0f / 100.0f);	// #formula

			if (lpObj->iShield < 0)
				lpObj->iShield = 0;

			if (lpObj->FillLife > 0.0f)
			{
				lpObj->Life += lpObj->FillLife;

				if (lpObj->Life > (lpObj->MaxLife + lpObj->AddLife))
					lpObj->Life = lpObj->MaxLife + lpObj->AddLife;
			}

			if (lpObj->iFillShield > 0)
			{
				lpObj->iShield += lpObj->iFillShield;

				if (lpObj->iShield > (lpObj->iMaxShield + lpObj->iAddShield))
					lpObj->iShield = lpObj->iMaxShield + lpObj->iAddShield;
			}

			lpObj->FillLife = (float)iHPGage;
			lpObj->FillLifeMax = (float)iHPGage;
			lpObj->FillLifeCount = 4;
			lpObj->iFillShield = iShieldGage;
			lpObj->iFillShieldMax = iShieldGage;
			lpObj->iFillShieldCount = 4;

			GCSendEffectInfo(lpObj, 3);

			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}

			sLog->outBasic("[%s][%s]Use Compound Potion Lv1 - SD[%d] HP[%f] -> SD[%d] HP[%f]",
				lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->iShield, lpObj->Life,
				lpObj->iShield + iShieldGage, (double)(lpObj->Life + (float)iHPGage));
		}
		else if (citem->m_Type == ITEMGET(14, 39))	// Compound Potion 
		{
			int iShieldGage = (lpObj->iMaxShield + lpObj->iAddShield) * 10 / 100;	// #formula
			int iHPGage = (int)((lpObj->MaxLife + lpObj->AddLife) * 25.0f / 100.0f);	// #formula

			if (lpObj->iShield < 0)
				lpObj->iShield = 0;

			if (lpObj->FillLife > 0.0f)
			{
				lpObj->Life += lpObj->FillLife;

				if (lpObj->Life > (lpObj->MaxLife + lpObj->AddLife))
					lpObj->Life = lpObj->MaxLife + lpObj->AddLife;
			}

			if (lpObj->iFillShield > 0)
			{
				lpObj->iShield += lpObj->iFillShield;

				if (lpObj->iShield > (lpObj->iMaxShield + lpObj->iAddShield))
					lpObj->iShield = lpObj->iMaxShield + lpObj->iAddShield;
			}

			lpObj->FillLife = (float)iHPGage;
			lpObj->FillLifeMax = (float)iHPGage;
			lpObj->FillLifeCount = 4;
			lpObj->iFillShield = iShieldGage;
			lpObj->iFillShieldMax = iShieldGage;
			lpObj->iFillShieldCount = 4;

			GCSendEffectInfo(lpObj, 3);

			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}

			sLog->outBasic("[%s][%s]Use Compound Potion Lv2 - SD[%d] HP[%f] -> SD[%d] HP[%f]",
				lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->iShield, lpObj->Life,
				lpObj->iShield + iShieldGage, (double)(lpObj->Life + (float)iHPGage));
		}
		else if (citem->m_Type == ITEMGET(14, 40))	// Large Compound Potion 
		{
			int iShieldGage = (lpObj->iMaxShield + lpObj->iAddShield) * 20 / 100;	// #formula
			int iHPGage = (int)((lpObj->MaxLife + lpObj->AddLife) * 45.0f / 100.0f);	// #formula

			if (lpObj->iShield < 0)
				lpObj->iShield = 0;

			if (lpObj->FillLife > 0.0f)
			{
				lpObj->Life += lpObj->FillLife;

				if (lpObj->Life > (lpObj->MaxLife + lpObj->AddLife))
					lpObj->Life = lpObj->MaxLife + lpObj->AddLife;
			}

			if (lpObj->iFillShield > 0)
			{
				lpObj->iShield += lpObj->iFillShield;

				if (lpObj->iShield > (lpObj->iMaxShield + lpObj->iAddShield))
					lpObj->iShield = lpObj->iMaxShield + lpObj->iAddShield;
			}

			lpObj->FillLife = (float)iHPGage;
			lpObj->FillLifeMax = (float)iHPGage;
			lpObj->FillLifeCount = 4;
			lpObj->iFillShield = iShieldGage;
			lpObj->iFillShieldMax = iShieldGage;
			lpObj->iFillShieldCount = 4;

			GCSendEffectInfo(lpObj, 3);

			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}

			sLog->outBasic("[%s][%s]Use Compound Potion Lv3 - SD[%d] HP[%f] -> SD[%d] HP[%f]",
				lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name,
				lpObj->iShield, lpObj->Life,
				lpObj->iShield + iShieldGage, (double)(lpObj->Life + (float)iHPGage));
		}
		else if (citem->m_Type == ITEMGET(14, 8)) // Antidote
		{
			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}

			if (gObjCheckUsedBuffEffect(*lpObj, BUFFTYPE_POISON) == TRUE)
			{
				gObjRemoveBuffEffect(*lpObj, BUFFTYPE_POISON);
				lpObj->lpAttackObj = 0;
				GCMagicCancelSend(lpObj, 1);
			}

			if (gObjCheckUsedBuffEffect(*lpObj, BUFFTYPE_FREEZE) == TRUE)
			{
				gObjRemoveBuffEffect(*lpObj, BUFFTYPE_FREEZE);
				lpObj->DelayActionTime = 0;
				lpObj->DelayLevel = 0;
				lpObj->lpAttackObj = 0;
				GCMagicCancelSend(lpObj, 7);
			}
		}
		else if (citem->m_Type == ITEMGET(14, 46))
		{
			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}

			gObjAddBuffEffect(*lpObj, BUFFTYPE_HALLOWEEN_BLESS);
		}
		else if (citem->m_Type == ITEMGET(14, 47))
		{
			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}

			gObjAddBuffEffect(*lpObj, BUFFTYPE_HALLOWEEN_ANGER);
		}
		else if (citem->m_Type == ITEMGET(14, 48))
		{
			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}

			gObjAddBuffEffect(*lpObj, BUFFTYPE_HALLOWEEN_CRY);
		}
		else if (citem->m_Type == ITEMGET(14, 49))
		{
			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}

			gObjAddBuffEffect(*lpObj, BUFFTYPE_HALLOWEEN_FOOD);
		}
		else if (citem->m_Type == ITEMGET(14, 50))
		{
			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}

			gObjAddBuffEffect(*lpObj, BUFFTYPE_HALLOWEEN_DRINK);
		}
		else if (citem->m_Type == ITEMGET(14, 85))
		{
			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}

			gObjAddBuffEffect(*lpObj, BUFFTYPE_CHERRYBLOSSOM_DRINK);
		}
		else if (citem->m_Type == ITEMGET(14, 86))
		{
			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}

			gObjAddBuffEffect(*lpObj, BUFFTYPE_CHERRYBLOSSOM_DUMPLING);
		}
		else if (citem->m_Type == ITEMGET(14, 87))
		{
			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}

			gObjAddBuffEffect(*lpObj, BUFFTYPE_CHERRYBLOSSOM_PETAL);
		}
		else if (citem->m_Type == ITEMGET(13, 81))
		{
			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}

			gObjAddBuffEffect(*lpObj, BUFFTYPE_CHARM_GUARDIAN);
		}
		else if (citem->m_Type == ITEMGET(13, 82))
		{
			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}

			gObjAddBuffEffect(*lpObj, BUFFTYPE_CHARM_PROTECTITEM);
		}

		else if (citem->m_Type == ITEMGET(13, 43))
		{
			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}

			gObjAddBuffEffect(*lpObj, BUFFTYPE_PCS_MARK1);
		}

		else if (citem->m_Type == ITEMGET(13, 44))
		{
			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}

			gObjAddBuffEffect(*lpObj, BUFFTYPE_PCS_MARK2);
		}

		else if (citem->m_Type == ITEMGET(13, 45))
		{
			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}

			gObjAddBuffEffect(*lpObj, BUFFTYPE_PCS_MARK3);
		}
		else if (g_kJewelOfHarmonySystem.IsJewelOfHarmonySmeltingItems(citem->m_Type) == TRUE)
		{
			if (g_kJewelOfHarmonySystem.SmeltItemBySmeltingStone(*lpObj, lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE)
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemOneSend(lpObj, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}
			else
			{
				GCReFillSend(lpObj, lpObj->Life, 0xFD, 1, lpObj->iShield);
			}
		}
		else if (citem->m_Type == ITEMGET(12, 30))
		{
			if (UseBundleOfBlessJewel(*lpObj, lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE)
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemOneSend(lpObj, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}
		}

#if( ENABLETEST_ITEMUPGRADEJEWELS == 1 )
		else if (g_ItemUpgradeJewels.IsJewel(citem->m_Type))
		{
			g_ItemUpgradeJewels.ProcInsert(*lpObj, lpMsg->inventoryPos, lpMsg->invenrotyTarget);
		}
#else
		else if (g_kJewelOfHarmonySystem.IsJewelOfHarmonyPurity(citem->m_Type) == TRUE && gSocketItemType.CheckSocketItemType(&lpObj->pntInventory[lpMsg->invenrotyTarget]) == FALSE) //season4 add-on)
		{
			if (g_kJewelOfHarmonySystem.StrengthenItemByJewelOfHarmony(*lpObj, lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE)
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemOneSend(lpObj, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}
			else
			{
				GCReFillSend(lpObj, lpObj->Life, 0xFD, 1, lpObj->iShield);
			}
		}
		else if (citem->m_Type == ITEMGET(14, 13)) // Jewel Of Bless
		{
			if (gObjItemLevelUp(*lpObj, lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE)
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemOneSend(lpObj, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}
		}
		else if (citem->m_Type == ITEMGET(14, 14)) // Jewel Of Soul
		{
			if (gObjItemRandomLevelUp(*lpObj, lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE)
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemOneSend(lpObj, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}
		}
		else if (citem->m_Type == ITEMGET(14, 16)) // Jewel Of Life
		{
			if (gObjItemRandomOption3Up(*lpObj, lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE)
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemOneSend(lpObj, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}
			else
			{
				GCReFillSend(lpObj, lpObj->Life, 0xFD, 1, lpObj->iShield);
			}
		}
#endif
		else if (citem->m_Type == ITEMGET(14, 160))	//1.01.00 update
		{
			if (g_LuckyItemManager.LuckyItemRepaire(*lpObj, lpMsg->inventoryPos, lpMsg->invenrotyTarget))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemOneSend(lpObj, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}
			else
			{
				GCReFillSend(lpObj, lpObj->Life, 0xFD, 1, lpObj->iShield);
			}
		}

		else if (citem->m_Type == ITEMGET(14, 161))	//1.01.00 update
		{
			if (g_kJewelOfHarmonySystem.StrengthenItemByJewelOfRise(*lpObj, lpMsg->inventoryPos, lpMsg->invenrotyTarget))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemOneSend(lpObj, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}
			else
			{
				GCReFillSend(lpObj, lpObj->Life, 0xFD, 1, lpObj->iShield);
			}
		}
		else if (citem->m_Type == ITEMGET(14, 209)) // Tradeable Seal Season 8
		{
			if (g_PentagramSystem.AddTradeCount(*lpObj, lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE)
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemOneSend(lpObj, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}

			else
			{
				GCReFillSend(lpObj, lpObj->Life, 0xFD, 1, lpObj->iShield);
			}
		}

		else if (citem->m_Type == ITEMGET(14, 224))
		{
			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}

			gObjAddBuffEffect(*lpObj, BUFFTYPE_LIGHT_BLESSING);
			//[K2]Light
		}

		else if (citem->m_Type == ITEMGET(14, 263))
		{
			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
				//	gObjInventoryDeleteItem(lpObj, pos);

			}
			gObjAddBuffEffect(*lpObj, BUFFTYPE_LIGHT_BLESSING_LOW);
		}

		else if (citem->m_Type == ITEMGET(14, 264))
		{
			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}

			gObjAddBuffEffect(*lpObj, BUFFTYPE_LIGHT_BLESSING_MIDDLE);
		}

		else if (citem->m_Type >= ITEMGET(15, 0) ||	// Group 15 - Scrolls
			citem->m_Type == ITEMGET(12, 7) || // Orb of Twisting Slash
			citem->m_Type == ITEMGET(12, 289) || // 
			citem->m_Type == ITEMGET(12, 290) || // 
			citem->m_Type == ITEMGET(12, 291) || // 
			citem->m_Type == ITEMGET(12, 292) || // 
			citem->m_Type == ITEMGET(12, 293) || // 
			citem->m_Type == ITEMGET(12, 295) || // 
			citem->m_Type == ITEMGET(12, 296) || // 
			citem->m_Type == ITEMGET(12, 297) || // 
			citem->m_Type == ITEMGET(12, 298) || // 
			citem->m_Type == ITEMGET(12, 299) || // 
			citem->m_Type == ITEMGET(12, 300) || // 
			citem->m_Type == ITEMGET(12, 303) || // 
			citem->m_Type == ITEMGET(12, 305) || // 
			(citem->m_Type >= ITEMGET(12, 8) && citem->m_Type <= ITEMGET(12, 24)) ||	// Orbs
			citem->m_Type == ITEMGET(12, 35) ||
			(citem->m_Type >= ITEMGET(12, 44) && citem->m_Type <= ITEMGET(12, 48)) ||	// Scroll of Fire Scream
			(citem->m_Type >= ITEMGET(12, 271) && citem->m_Type <= ITEMGET(12, 277))) // Grow Lancer Skills
		{

			if ((lpObj->m_PlayerData->Strength + lpObj->AddStrength) < citem->m_RequireStrength)
			{
				GCReFillSend(lpObj, lpObj->Life, 0xFD, 1, lpObj->iShield);
				return;
			}

			if ((lpObj->m_PlayerData->Dexterity + lpObj->AddDexterity) < citem->m_RequireDexterity)
			{
				GCReFillSend(lpObj, lpObj->Life, 0xFD, 1, lpObj->iShield);
				return;
			}

			if (citem->m_Type == ITEMGET(15, 18))	// Scroll of HellBurst
			{
				if (g_QuestInfo.GetQuestState(*lpObj, 2) != 2)
				{
					GCReFillSend(lpObj, lpObj->Life, 0xFD, 1, lpObj->iShield);
					return;
				}
			}

			if (citem->m_Type >= ITEMGET(12, 8) && citem->m_Type <= ITEMGET(12, 24))
			{
				if (lpObj->Level < citem->m_RequireLevel)	// Orbs
				{
					GCReFillSend(lpObj, lpObj->Life, 0xFD, 1, lpObj->iShield);
					return;
				}
			}

			unsigned short skillnumber;

			if (citem->IsClass(lpObj->Class, lpObj->m_PlayerData->ChangeUP) == FALSE)
			{
				GCInventoryItemDeleteSend(lpObj, -1, 1);

			}
			else
			{
				int addskill = gObjMagicAdd(*lpObj, ITEM_GET_TYPE(citem->m_Type), ITEM_GET_INDEX(citem->m_Type), citem->m_Level, skillnumber);

				if (addskill >= 0)
				{
					gObjInventoryItemSet(*lpObj, pos, -1);
					GCMagicListOneSend(lpObj, addskill, skillnumber, citem->m_Level, 0, 0);
					lpObj->pntInventory[pos]->Clear();
					GCInventoryItemDeleteSend(lpObj, pos, 1);
				}
				else
				{
					GCInventoryItemDeleteSend(lpObj, -1, 1);
				}
			}
		}
		else if (citem->m_Type == ITEMGET(14, 10)) // Town Portal Scroll
		{
			if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
			{
				sLog->outBasic("[UBF][Return Scroll] [%s][%s][%s] - No Available",
					lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->m_RealNameOfUBF);

				return;
			}

			sLog->outBasic("[Using Item] [Return Scroll] [%s][%s] - Current Map:[%d]",
				lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->MapNumber);

			/*if (BC_MAP_RANGE(lpObj->MapNumber))
			{
				int iBridgeIndex = g_BloodCastle.GetBridgeIndex(lpObj->MapNumber);
				if (g_BloodCastle.GetCurrentState(iBridgeIndex) == 2)
				{
					g_BloodCastle.SearchUserDropQuestItem(*lpObj);
				}
				else
				{
					g_BloodCastle.SearchUserDeleteQuestItem(*lpObj);
				}
			}

			if (IT_MAP_RANGE(lpObj->MapNumber))
			{
				g_IT_Event.DropRelicsItem(lpObj->MapNumber, *lpObj);
			}*/

			if (lpObj->m_IfState.use && lpObj->m_IfState.type == 3)
			{
				lpObj->TargetNpcNumber = -1;
				lpObj->m_IfState.type = 0;
				lpObj->m_IfState.use = 0;
			}

			gObjInventoryItemSet(*lpObj, pos, -1);
			lpObj->pntInventory[pos]->Clear();
			GCInventoryItemDeleteSend(lpObj, pos, 1);

			if (lpObj->MapNumber == MAP_INDEX_DEVIAS)
			{
				gObjMoveGate(*lpObj, 22);
			}
			else if (lpObj->MapNumber == MAP_INDEX_NORIA)
			{
				gObjMoveGate(*lpObj, 27);
			}
			else if (lpObj->MapNumber == MAP_INDEX_ELBELAND)
			{
				gObjMoveGate(*lpObj, 267);
			}
			else if (lpObj->MapNumber == MAP_INDEX_LOSTTOWER)
			{
				gObjMoveGate(*lpObj, 42);
			}
			else if (lpObj->MapNumber == MAP_INDEX_ATHLANSE)
			{
				gObjMoveGate(*lpObj, 49);
			}
			else if (lpObj->MapNumber == MAP_INDEX_TARKAN)
			{
				gObjMoveGate(*lpObj, 57);
			}
			/*else if (BC_MAP_RANGE(lpObj->MapNumber) != FALSE)
			{
				gObjMoveGate(*lpObj, 22);
			}
			else if (CC_MAP_RANGE(lpObj->MapNumber) != FALSE)
			{
				gObjMoveGate(*lpObj, 22);
			}*/
			else if (lpObj->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
			{
				gObjMoveGate(*lpObj, 333);
			}
			else if (KALIMA_MAP_RANGE(lpObj->MapNumber) != FALSE)
			{
				gObjMoveGate(*lpObj, 22);
			}
			else if (IT_MAP_RANGE(lpObj->MapNumber) != FALSE)
			{
				gObjMoveGate(*lpObj, 267);
			}
			else if (lpObj->MapNumber == MAP_INDEX_AIDA)
			{
				gObjMoveGate(*lpObj, 27);
			}
			else if (lpObj->MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE)
			{
				gObjMoveGate(*lpObj, 27);
			}
			else if (lpObj->MapNumber == MAP_INDEX_BARRACKS)
			{
				gObjMoveGate(*lpObj, 114);
			}
			else if (lpObj->MapNumber == MAP_INDEX_REFUGEE)
			{
				gObjMoveGate(*lpObj, 114);
			}
			else if (lpObj->MapNumber == MAP_INDEX_CALMNESS)
			{
				gObjMoveGate(*lpObj, 273);
			}
			else if (lpObj->MapNumber == MAP_INDEX_RAKLION || lpObj->MapNumber == MAP_INDEX_HATCHERY)
			{
				gObjMoveGate(*lpObj, 287);
			}
			else if (lpObj->MapNumber == MAP_INDEX_SANTAVILLAGE)
			{
				gObjMoveGate(*lpObj, 22);
			}
			else if (DG_MAP_RANGE(lpObj->MapNumber) != FALSE)
			{
				gObjMoveGate(*lpObj, 267);
			}
			else if (IMPERIAL_MAP_RANGE(lpObj->MapNumber) != FALSE)
			{
				gObjMoveGate(*lpObj, 22);
			}
			else if (lpObj->MapNumber == MAP_INDEX_LORENMARKET)
			{
				gObjMoveGate(*lpObj, 333);
			}
			else if (lpObj->MapNumber == MAP_INDEX_FEREA)
			{
				gObjMoveGate(*lpObj, 509);
			}
			else if (lpObj->MapNumber == MAP_INDEX_NIXIE_LAKE)
			{
				gObjMoveGate(*lpObj, 522);
			}

			/*else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && lpObj->MapNumber == MAP_INDEX_CASTLESIEGE)
			{
				if (g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					if (lpObj->m_btCsJoinSide == 1)
					{
						gObjMoveGate(*lpObj, 101);
					}
					else
					{
						gObjMoveGate(*lpObj, 100);
					}
				}
				else
				{
					if (g_CastleSiege.CheckCastleOwnerMember(*lpObj) == TRUE || g_CastleSiege.CheckCastleOwnerUnionMember(*lpObj) == TRUE)
					{
						gObjMoveGate(*lpObj, 101);
					}
					else
					{
						gObjMoveGate(*lpObj, 100);
					}
				}
			}*/

			else if (g_NewPVP.IsVulcanusMap(lpObj->MapNumber))
			{
				gObjMoveGate(*lpObj, 294);
			}

			else if (g_NewPVP.IsPKFieldMap(lpObj->MapNumber))
			{
				g_NewPVP.Reset(*lpObj);
				gObjMoveGate(*lpObj, 17);
			}

			else if (lpObj->MapNumber == MAP_INDEX_ARENA)
			{
			}

			else
			{
				gObjMoveGate(*lpObj, 17);
			}

		}
		/*else if (citem->m_Type == ITEMGET(13, 66))
		{
			if (DG_MAP_RANGE(lpObj->MapNumber) == TRUE)
			{
				g_DoppelGanger.LeaveDoppelganger(*lpObj);

				if (g_DoppelGanger.GetDoppelgangerState() == 2)
				{
					g_DoppelGanger.SendDoppelgangerResult(*lpObj, 1);
				}
			}

			gObjTeleport(*lpObj, MAP_INDEX_SANTAVILLAGE, 220, 20);

			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}
		}*/

		else if (citem->m_Type == ITEMGET(14, 162))
		{
			if (lpObj->m_PlayerData->m_InventoryExpansion < 2)
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
				lpObj->m_PlayerData->m_InventoryExpansion++;
				GDSetExpWare(lpObj, 1, lpObj->m_PlayerData->m_InventoryExpansion);

				PMSG_USEEXPANSIONITEM pMsg;
				PHeadSetB((LPBYTE)&pMsg, 0x2B, sizeof(pMsg));
				pMsg.Result = 1;
				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
			}
		}

		else if (citem->m_Type == ITEMGET(14, 163))
		{
			if (lpObj->m_PlayerData->m_WarehouseExpansion < 1)
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
				GDSetExpWare(lpObj, 2, 1);
				lpObj->m_PlayerData->m_WarehouseExpansion = 1;

				PMSG_USEEXPANSIONITEM pMsg;
				PHeadSetB((LPBYTE)&pMsg, 0x2B, sizeof(pMsg));
				pMsg.Result = 1;
				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
			}
		}

		else if (citem->m_Type == ITEMGET(13, 152) || citem->m_Type == ITEMGET(13, 156))
		{
			if (g_MasterLevelSkillTreeSystem.CheckMasterSkillPoint(*lpObj, 1) == TRUE)
			{
				g_MasterLevelSkillTreeSystem.ResetMasterSkill(*lpObj, 1);
				sLog->outBasic("[MasterLevelSkill Reset] - Passive (%d)", lpObj);
				gObjCloseSet(*lpObj, 1);
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}

			else
			{
				this->GCServerMsgStringSend(Lang.GetText(0, 526), lpObj, 1);
			}
		}

		else if (citem->m_Type == ITEMGET(13, 153) || citem->m_Type == ITEMGET(13, 157))
		{
			if (g_MasterLevelSkillTreeSystem.CheckMasterSkillPoint(*lpObj, 2) == TRUE)
			{
				g_MasterLevelSkillTreeSystem.ResetMasterSkill(*lpObj, 2);
				sLog->outBasic("[MasterLevelSkill Reset] - Strengthen Skill (%d)", lpObj);
				gObjCloseSet(*lpObj, 1);
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}

			else
			{
				this->GCServerMsgStringSend(Lang.GetText(0, 526), lpObj, 1);
			}
		}

		else if (citem->m_Type == ITEMGET(13, 154) || citem->m_Type == ITEMGET(13, 158))
		{
			if (g_MasterLevelSkillTreeSystem.CheckMasterSkillPoint(*lpObj, 3) == TRUE)
			{
				g_MasterLevelSkillTreeSystem.ResetMasterSkill(*lpObj, 3);
				sLog->outBasic("[MasterLevelSkill Reset] - Enhance Attack/Defense (%d)", lpObj);
				gObjCloseSet(*lpObj, 1);
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}

			else
			{
				this->GCServerMsgStringSend(Lang.GetText(0, 526), lpObj, 1);
			}
		}

		else if (citem->m_Type == ITEMGET(13, 155) || citem->m_Type == ITEMGET(13, 159))
		{
			if (g_MasterLevelSkillTreeSystem.CheckMasterSkillPoint(*lpObj, 0) == TRUE)
			{
				g_MasterLevelSkillTreeSystem.ResetMasterSkill(*lpObj, 0);
				sLog->outBasic("[MasterLevelSkill Reset] - All (%d)", lpObj);
				gObjCloseSet(*lpObj, 1);
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}

			else
			{
				this->GCServerMsgStringSend(Lang.GetText(0, 526), lpObj, 1);
			}
		}

		else if (citem->m_Type == ITEMGET(14, 9)) // Ale
		{
			int level = citem->m_Level;

			gObjInventoryItemSet(*lpObj, pos, -1);
			lpObj->pntInventory[pos]->Clear();
			GCInventoryItemDeleteSend(lpObj, pos, 1);
			gObjUseDrink(*lpObj, level);
		}
		else if (citem->m_Type == ITEMGET(14, 20)) // Remedy of Love
		{
			if (citem->m_Level == 0)
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
				gObjUseDrink(*lpObj, 2);
			}
		}
		else if (citem->m_Type == ITEMGET(13, 15)) // Fruits
		{
			if (iItemUseType == 0)
			{
				gObjUsePlusStatFruit(*lpObj, pos);
			}
			else if (iItemUseType == 1)
			{
				gObjUseMinusStatFruit(*lpObj, pos);
			}
		}
		else if (citem->m_Type == ITEMGET(13, 70))
		{
			if (lpObj->SaveMapNumber != -1)
			{
				bool bMove = gMoveCommand.CheckEquipmentToMove(*lpObj, lpObj->SaveMapNumber);

				if (bMove == true && MapNumberCheck(lpObj->SaveMapNumber))
				{
					gObjTeleport(*lpObj, lpObj->SaveMapNumber, lpObj->SaveX, lpObj->SaveY);
					gObjInventoryItemSet(*lpObj, pos, -1);
					lpObj->pntInventory[pos]->Clear();
					GCInventoryItemDeleteSend(lpObj, pos, 1);
					lpObj->SaveMapNumber = -1;
					GCServerCmd(lpObj, 61, 0, 0);
				}
			}
			else
			{
				if (gObjSearchItemMinus(*lpObj, pos, 1))
				{
					lpObj->SaveMapNumber = lpObj->MapNumber;
					lpObj->SaveX = lpObj->X;
					lpObj->SaveY = lpObj->Y;
				}
				else
				{
					gObjInventoryItemSet(*lpObj, pos, -1);
					lpObj->pntInventory[pos]->Clear();
					GCInventoryItemDeleteSend(lpObj, pos, 1);
				}
			}
		}
		else if (citem->m_Type == ITEMGET(13, 69))
		{
			if (lpObj->DieMapNumber != -1)
			{
				bool bMove = gMoveCommand.CheckEquipmentToMove(*lpObj, lpObj->DieMapNumber);

				bool bGens = g_GensSystem.IsMapBattleZone(lpObj->MapNumber);

				if (bMove == true && bGens == false && MapNumberCheck(lpObj->DieMapNumber))
				{
					gObjTeleport(*lpObj, lpObj->DieMapNumber, lpObj->DieX, lpObj->DieY);
					gObjInventoryItemSet(*lpObj, pos, -1);
					lpObj->pntInventory[pos]->Clear();
					GCInventoryItemDeleteSend(lpObj, pos, 1);
					lpObj->DieMapNumber = -1;
					GCServerCmd(lpObj, 60, 0, 0);
				}
			}
		}
		else if (citem->m_Type >= ITEMGET(13, 54) && citem->m_Type <= ITEMGET(13, 58))
		{
			CashShopExMinusStatFruit(*lpObj, pos);
		}
		else if (citem->m_Type == ITEMGET(14, 70)) // CSHOP HP Potions
		{
			int tLife = (citem->m_Value * 10) - (lpObj->Level);	// #formula

			if (tLife < 0)
			{
				tLife = 0;
			}

			tLife += ((int)(lpObj->MaxLife + lpObj->AddLife));	// #formula
			lpObj->Life += tLife;

			if (lpObj->Life > (lpObj->MaxLife + lpObj->AddLife - 1.0f))
				lpObj->Life = lpObj->MaxLife + lpObj->AddLife;

			GCReFillSend(lpObj, lpObj->Life, 0xFD, 1, lpObj->iShield);

			lpObj->FillLifeMax = tLife;
			lpObj->FillLife = tLife;

			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}
		}
		else if (citem->m_Type == ITEMGET(14, 71)) // CSHOP MP Potions
		{
			int tMana = (citem->m_Value * 10) - (lpObj->Level);	// #formula

			if (tMana < 0)
			{
				tMana = 0;
			}

			tMana += ((int)(lpObj->MaxMana + lpObj->AddMana));
			lpObj->Mana += tMana;

			if (lpObj->Mana > (lpObj->MaxMana + lpObj->AddMana - 1.0f))
				lpObj->Mana = lpObj->MaxMana + lpObj->AddMana;

			GCManaSend(lpObj, lpObj->Mana, 0xFF, 0, lpObj->BP);

			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}
		}
		else if (citem->m_Type == ITEMGET(14, 133))
		{
			gObjUseRecoveryPotion(*lpObj, pos, (double)0.65);
		}
		else if (citem->m_Type == ITEMGET(14, 78)) // CSHOP Arcanum For?a
		{
			LPITEMEFFECT lpEffect = g_ItemAddOption.GetItemEffectData(citem->m_Type);

			if (lpEffect == NULL)
			{
				GCReFillSend(lpObj, lpObj->Life, 0xFD, 1, lpObj->iShield);
				return;
			}

			g_PeriodItemEx.SetPeriodItemInfo(*lpObj, citem->m_Type, 0, lpEffect->iEffectValidTime);
			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}
		}
		else if (citem->m_Type == ITEMGET(14, 79)) // CSHOP Arcanum Agilidade
		{
			LPITEMEFFECT lpEffect = g_ItemAddOption.GetItemEffectData(citem->m_Type);

			if (lpEffect == NULL)
			{
				GCReFillSend(lpObj, lpObj->Life, 0xFD, 1, lpObj->iShield);
				return;
			}

			g_PeriodItemEx.SetPeriodItemInfo(*lpObj, citem->m_Type, 0, lpEffect->iEffectValidTime);
			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}
		}
		else if (citem->m_Type == ITEMGET(14, 80)) // CSHOP Arcanum Vitalidade
		{
			LPITEMEFFECT lpEffect = g_ItemAddOption.GetItemEffectData(citem->m_Type);

			if (lpEffect == NULL)
			{
				GCReFillSend(lpObj, lpObj->Life, 0xFD, 1, lpObj->iShield);
				return;
			}

			g_PeriodItemEx.SetPeriodItemInfo(*lpObj, citem->m_Type, 0, lpEffect->iEffectValidTime);
			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}
		}
		else if (citem->m_Type == ITEMGET(14, 81)) // CSHOP Arcanum Energia
		{
			LPITEMEFFECT lpEffect = g_ItemAddOption.GetItemEffectData(citem->m_Type);

			if (lpEffect == NULL)
			{
				GCReFillSend(lpObj, lpObj->Life, 0xFD, 1, lpObj->iShield);
				return;
			}

			g_PeriodItemEx.SetPeriodItemInfo(*lpObj, citem->m_Type, 0, lpEffect->iEffectValidTime);
			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}
		}
		else if (citem->m_Type == ITEMGET(14, 82)) // CSHOP Arcanum Comando
		{
			LPITEMEFFECT lpEffect = g_ItemAddOption.GetItemEffectData(citem->m_Type);

			if (lpEffect == NULL)
			{
				GCReFillSend(lpObj, lpObj->Life, 0xFD, 1, lpObj->iShield);
				return;
			}

			g_PeriodItemEx.SetPeriodItemInfo(*lpObj, citem->m_Type, 0, lpEffect->iEffectValidTime);
			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}
		}
		else if (citem->m_Type == ITEMGET(13, 60)) // CSHOP PK Free Ticket
		{
			if (lpObj->m_PK_Level == 6)
			{
				lpObj->m_PK_Level = 5;
			}
			else if (lpObj->m_PK_Level == 5)
			{
				lpObj->m_PK_Level = 4;
			}
			else if (lpObj->m_PK_Level == 4)
			{
				lpObj->m_PK_Level = 3;
			}
			else if (lpObj->m_PK_Level <= 3)
			{
				lpObj->m_PK_Level = lpObj->m_PK_Level;
			}

			GCPkLevelSend(lpObj, lpObj->m_PK_Level);

			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}
			sLog->outBasic("[PCS] [%s][%s] Use PK Clear Item", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
		}
		else if (citem->m_Type == ITEMGET(14, 7))	// Siege Potion
		{
			if (!gObjSearchItemMinus(*lpObj, pos, 1))
			{
				gObjInventoryItemSet(*lpObj, pos, -1);
				lpObj->pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(lpObj, pos, 1);
			}

			int iLevel = citem->m_Level;
			int iSerial = citem->GetNumber();
			int iDur = citem->m_Durability;

			if (lpObj->m_PlayerData->lpGuild)
			{
				sLog->outBasic("[CastleSiege] Using CastleSiege Potion Lv[%d]/Si[%d]/Dur[%d] - [%s][%s] - (Guild : %s)",
					iLevel, iSerial, iDur, lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->lpGuild->Name);
			}
			else
			{
				sLog->outBasic("[CastleSiege] Using CastleSiege Potion Lv[%d]/Si[%d]/Dur[%d] - [%s][%s]",
					iLevel, iSerial, iDur, lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
			}

			if (iLevel == 0)
			{
				gObjUseBlessAndSoulPotion(*lpObj, iLevel);
			}
			else if (iLevel == 1)
			{
				gObjUseBlessAndSoulPotion(*lpObj, iLevel);
			}
		}
		/*else if (citem->m_Type == ITEMGET(13, 48))
		{
			if ((lpObj->MapNumber < MAP_INDEX_KALIMA1 || lpObj->MapNumber > MAP_INDEX_KALIMA6) && lpObj->MapNumber != MAP_INDEX_KALIMA7)
			{
				int iLevel = g_KalimaGate.GetKalimaGateLevel2(*lpObj);
				int iSerial = citem->GetNumber();
				int iDuration = citem->m_Durability;
				BOOL bKalimaGateCreateResult = FALSE;
				int iKalimaGateX = 0;
				int iKalimaGateY = 0;

				iLevel++;
				iKalimaGateX = lpObj->X + rand() % 6 - 2;
				iKalimaGateY = lpObj->Y + rand() % 6 - 2;

				bKalimaGateCreateResult = g_KalimaGate.CreateKalimaGate(*lpObj, iLevel, iKalimaGateX, iKalimaGateY);

				if (bKalimaGateCreateResult == TRUE)
				{
					if (!gObjSearchItemMinus(*lpObj, pos, 1))
					{
						gObjInventoryItemSet(*lpObj, pos, -1);
						lpObj->pntInventory[pos]->Clear();
						GCInventoryItemDeleteSend(lpObj, pos, 1);
					}

					sLog->outBasic("[PCS] Use Free Kalima Ticket Success");
				}
				else
				{
					sLog->outBasic("[PCS] Use Free Kalima Ticket Failed");
				}
			}
		}*/

		else if (g_QuestExpManager.IsQuestItemAtt(citem->m_Type, 64) == true)
		{
			int Ep = g_QuestExpManager.GetQuestItemEp(citem->m_Type);
			int iResult = 0;
			lua_State *L = g_MuLuaQuestExp.GetLua();

			if (!L)
			{
				sLog->outBasic("[QuestExp] - Error - [%s] [%d]", __FILE__, __LINE__);
			}

			else
			{
				if (Ep)
				{
					g_Generic_Call(L, "ItemUseQuest", "ii>i", lpObj, Ep, &iResult);

					if (iResult)
					{
						gObjInventoryItemSet(*lpObj, pos, -1);
						lpObj->pntInventory[pos]->Clear();
						GCInventoryItemDeleteSend(lpObj, pos, 1);
					}
				}
			}
		}
		else
		{
			sLog->outBasic("error-L3 : %s return %s %d %d %s",
				lpObj->Name, __FILE__, __LINE__, pos, lpObj->pntInventory[pos]->GetName());

			GCReFillSend(lpObj, lpObj->Life, 0xFD, 1, lpObj->iShield);
		}
	}
	else
	{
		GCReFillSend(lpObj, lpObj->Life, 0xFD, 1, lpObj->iShield);
		sLog->outBasic("error-L3 : %s return %s %d %d",
			lpObj->Name, __FILE__, __LINE__, pos);
	}
}


struct PMSG_REFILL
{
	PBMSG_HEAD h;	// C1:26
	BYTE IPos;	// 3
	BYTE LifeH;	// 4
	BYTE LifeL;	// 5
	BYTE Flag;	// 6
	BYTE btShieldH;	// 7
	BYTE btShieldL;	// 8
	int IGCLife;
	int IGCPenta;
};



void GameProtocol::GCReFillSend(CGameObject* lpObj, int Life, BYTE Ipos, unsigned char flag, WORD wShield)
{
	PMSG_REFILL pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x26, sizeof(pMsg));
	pMsg.IPos = Ipos;
	pMsg.LifeH = SET_NUMBERH(Life);
	pMsg.LifeL = SET_NUMBERL(Life);
	pMsg.btShieldH = SET_NUMBERH(wShield);
	pMsg.btShieldL = SET_NUMBERL(wShield);
	pMsg.Flag = flag;	// #error Flag is Disabled
	pMsg.IGCLife = Life;
	pMsg.IGCPenta = wShield;
	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}


struct PMSG_MANASEND
{
	PBMSG_HEAD h;	// C1:27
	BYTE IPos;	// 3
	BYTE ManaH;	// 4
	BYTE ManaL;	// 5
	BYTE BPH;	// 6
	BYTE BPL;	// 7
	int IGCMana;
};

void GameProtocol::GCManaSend(CGameObject* lpObj, int Mana, BYTE Ipos, unsigned char flag, WORD BP)
{
	PMSG_MANASEND pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x27, sizeof(pMsg));
	pMsg.IPos = Ipos;
	pMsg.ManaH = SET_NUMBERH(Mana);
	pMsg.ManaL = SET_NUMBERL(Mana);
	pMsg.BPH = SET_NUMBERH(BP);
	pMsg.BPL = SET_NUMBERL(BP);
	pMsg.IGCMana = Mana;
	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}


struct PMSG_INVENTORYDELETE
{
	PBMSG_HEAD h;	// C1:28
	BYTE IPos;	// 3
	BYTE Flag;	// 4
};

void GameProtocol::GCInventoryItemDeleteSend(CGameObject* lpObj, BYTE pos, unsigned char flag)
{
	PMSG_INVENTORYDELETE pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x28, sizeof(pMsg));
	pMsg.IPos = pos;
	pMsg.Flag = flag;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}

struct PMSG_ITEMUSESPECIALTIME
{
	PBMSG_HEAD h;	// C1:29
	BYTE Number;	// 3
	WORD Time;	// 4
};

void GameProtocol::GCItemUseSpecialTimeSend(CGameObject* lpObj, unsigned char number, int time)
{
	if (time > 65535)
	{
		time = 65535;
	}

	PMSG_ITEMUSESPECIALTIME pMsg;

	PHeadSetBE((LPBYTE)&pMsg, 0x29, sizeof(pMsg));
	pMsg.Number = number;
	pMsg.Time = time;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}

struct PMSG_ITEMDUR
{
	PBMSG_HEAD h;	// C1:2A
	BYTE IPos;	// 3
	BYTE Dur;	// 4
	BYTE Flag;	// 5
};


void GameProtocol::GCItemDurSend(CGameObject* lpObj, BYTE pos, BYTE dur, unsigned char flag)
{
	PMSG_ITEMDUR pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x2A, sizeof(pMsg));
	pMsg.IPos = pos;
	pMsg.Dur = dur;
	pMsg.Flag = flag;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}



void GameProtocol::GCItemDurSend2(CGameObject* lpObj, unsigned char pos, unsigned char dur, unsigned char flag)
{
	PMSG_ITEMDUR pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x2A, sizeof(pMsg));
	pMsg.IPos = pos;
	pMsg.Dur = dur;
	pMsg.Flag = flag;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}


struct PMSG_WEATHER
{
	PBMSG_HEAD h;	// C1:0F
	BYTE Weather;	// 3
};

void GameProtocol::CGWeatherSend(CGameObject* lpObj, BYTE weather)
{
	PMSG_WEATHER pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0x0F, sizeof(pMsg));
	pMsg.Weather = weather;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}



void GameProtocol::GCServerCmd(CGameObject* lpObj, BYTE type, BYTE Cmd1, BYTE Cmd2)
{
	PMSG_SERVERCMD ServerCmd;

	PHeadSubSetB((LPBYTE)&ServerCmd, 0xF3, 0x40, sizeof(ServerCmd));
	ServerCmd.CmdType = type;
	ServerCmd.X = Cmd1;
	ServerCmd.Y = Cmd2;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&ServerCmd, ServerCmd.h.size);
}




void GameProtocol::GCReqmoveDevilSquare(PMSG_REQ_MOVEDEVILSQUARE* lpMsg, CGameObject* lpObj)
{

	BYTE cSquareNumber = lpMsg->SquareNumber;
	BYTE cInvitationItemPos = lpMsg->InvitationItemPos - INVENTORY_BAG_START;

	if (lpObj->m_IfState.use && lpObj->m_IfState.type != 12)
		return;

	PMSG_RESULT_MOVEDEVILSQUARE pResult;

	PHeadSetB((LPBYTE)&pResult, 0x90, sizeof(pResult));
	pResult.Result = 0;

	BOOL bPlayerKiller = FALSE; //season 2.5 add-on

	if (lpObj->PartyNumber >= 0) //season 2.5 add-on
	{
		if ((gParty.GetPKPartyPenalty(lpObj->PartyNumber)) > 4)
		{
			bPlayerKiller = TRUE;
		}
	}

	else
	{
		if (lpObj->m_PK_Level > 4)
		{
			bPlayerKiller = TRUE;
		}
	}

	if (g_ConfigRead.data.common.PKEnterEventAllow == true)
	{
		bPlayerKiller = FALSE;
	}

	if (bPlayerKiller == TRUE) //season 2.5 changed
	{
		pResult.Result = 6;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (MAIN_INVENTORY_RANGE(cInvitationItemPos) == FALSE)
	{
		pResult.Result = 1;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

		return;
	}

	/*if (DS_LEVEL_RANGE(cSquareNumber) == FALSE)
	{
		pResult.Result = 1;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

		return;
	}

	if (g_DevilSquare.GetState() != DevilSquare_OPEN)
	{
		pResult.Result = 2;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

		return;
	}

	if (g_DevilSquare.GetObjCount(cSquareNumber) >= g_DevilSquare.GetMaxUserInSquare())
	{
		pResult.Result = 5;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

		return;
	}*/

	CItemObject* sitem = lpObj->pntInventory[cInvitationItemPos];

	if (sitem->m_Type != ITEMGET(14, 19) && (sitem->m_Type != ITEMGET(13, 46) || sitem->m_Durability <= 0.0f))	// Devil's Invitation
	{
		pResult.Result = 1;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

		return;
	}

	int ItemLevel = sitem->m_Level;

	if (sitem->m_Type == ITEMGET(13, 46))
		ItemLevel = 10;

	if (ItemLevel != 0)
	{
		if (ItemLevel != 10)
		{
			if (ItemLevel != (cSquareNumber + 1))
			{
				pResult.Result = 1;
				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

				return;
			}
		}
	}

	BOOL bEnterCheck = FALSE;
	WORD movegateindex = 61;	// #gate
	/*int iENTER_LEVEL = g_DevilSquare.GetUserLevelToEnter(*lpObj, movegateindex);

	if (DS_LEVEL_RANGE(iENTER_LEVEL) == FALSE)
	{
		return;
	}*/
	int iENTER_LEVEL = 0; // stub

	if (ItemLevel == 10)
		cSquareNumber = iENTER_LEVEL;

	if (cSquareNumber != iENTER_LEVEL)
	{
		if (cSquareNumber > iENTER_LEVEL)
		{
			bEnterCheck = 2;
		}
		else
		{
			bEnterCheck = 1;
		}
	}

	if (bEnterCheck == 1)
	{
		pResult.Result = 3;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

		return;
	}

	if (bEnterCheck == 2)
	{
		pResult.Result = 4;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

		return;
	}

	if (sitem->m_Type == ITEMGET(14, 19) || (sitem->m_Type == ITEMGET(13, 46) && sitem->m_Durability == 1.0f))
	{
		gObjInventoryDeleteItem(*lpObj, cInvitationItemPos);
		GCInventoryItemDeleteSend(lpObj, cInvitationItemPos, 1);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
	}
	else if (sitem->m_Type == ITEMGET(13, 46) && sitem->m_Durability > 1.0f)
	{
		sitem->m_Durability -= 1.0f;
		GCItemDurSend2(lpObj, cInvitationItemPos, sitem->m_Durability, 0);
	}

	lpObj->m_nEventExp = 0;
	lpObj->m_nEventScore = 0;
	lpObj->m_nEventMoney = 0;
	lpObj->m_bDevilSquareIndex = 0; // stub //g_DevilSquare.GetDevilSquareIndex(movegateindex);
	lpObj->m_bDevilSquareAuth = true;
	lpObj->m_IfState.state = 0;
	lpObj->m_IfState.type = 0;
	lpObj->m_IfState.use = 0;

	gObjMoveGate(*lpObj, movegateindex);
	//g_DevilSquare.IncObjCount(cSquareNumber);
}



struct PMSG_RESULT_DEVILSQUARE_REMAINTIME
{
	PBMSG_HEAD h;	// C1:91
	BYTE hEventType;	// 3
	BYTE RemainTime;	// 4
	BYTE EnteredUser;	// 5
	BYTE RemainTime_LOW;	// 6
};


void GameProtocol::GCReqDevilSquareRemainTime(PMSG_REQ_DEVILSQUARE_REMAINTIME* lpMsg, CGameObject* lpObj)
{
	PMSG_RESULT_DEVILSQUARE_REMAINTIME pResult;

	PHeadSetB((LPBYTE)&pResult, 0x91, sizeof(pResult));
	pResult.RemainTime = 0;
	int iENTER_LEVEL;

	switch (lpMsg->hEventType)
	{
	case 1:
		if (lpObj->m_PK_Level >= 4 && g_ConfigRead.data.common.PKEnterEventAllow == false)
		{
			GCServerCmd(lpObj, 0x37, 0, 0);

			return;
		}

		pResult.hEventType = 1;

		/*if (g_DevilSquare.GetState() == DevilSquare_CLOSE)
		{
			pResult.RemainTime = g_DevilSquare.GetRemainTime();
		}
		else if (g_DevilSquare.GetState() == DevilSquare_OPEN)
		{
			pResult.RemainTime = 0;
		}
		else
		{
			pResult.RemainTime = g_DevilSquare.GetRemainTime() + g_DevilSquare.GetCloseTime();
		}*/
		break;

	case 2:
		if (lpObj->m_PK_Level >= 4 && g_ConfigRead.data.common.PKEnterEventAllow == false)
		{
			GCServerCmd(lpObj, 0x38, 0, 0);

			return;
		}

		pResult.hEventType = 2;

		/*if (g_BloodCastle.GetCurrentState(lpMsg->btItemLevel) == 1)
		{
			if (g_BloodCastle.CheckCanEnter(lpMsg->btItemLevel) != false)
			{
				pResult.RemainTime = 0;
			}
			else
			{
				pResult.RemainTime = g_BloodCastle.GetRemainTime(lpMsg->btItemLevel);
			}
		}
		else
		{
			pResult.RemainTime = g_BloodCastle.GetRemainTime(lpMsg->btItemLevel);
		}*/
		break;

	case 4:
		if (lpObj->m_PK_Level >= 4 && g_ConfigRead.data.common.PKEnterEventAllow == false)
		{
			GCServerCmd(lpObj, 0x39, 0, 0);

			return;
		}

		/*iENTER_LEVEL = g_ChaosCastle.GetUserLevelToEnter(*lpObj);

		if (iENTER_LEVEL == -1)
			return;

		pResult.hEventType = 4;

		if (g_ChaosCastle.GetCurrentState(iENTER_LEVEL) == 1)
		{
			if (g_ChaosCastle.CheckCanEnter(iENTER_LEVEL) != false)
			{
				pResult.RemainTime = 0;
				pResult.RemainTime_LOW = 0;
				pResult.EnteredUser = g_ChaosCastle.GetCurEnteredUser(iENTER_LEVEL);
			}
			else
			{
				WORD wREMAIN_TIME = g_ChaosCastle.GetRemainTime(iENTER_LEVEL);
				pResult.RemainTime = SET_NUMBERH(wREMAIN_TIME);
				pResult.RemainTime_LOW = SET_NUMBERL(wREMAIN_TIME);
				pResult.EnteredUser = g_ChaosCastle.GetCurEnteredUser(iENTER_LEVEL);
			}
		}
		else
		{
			WORD wREMAIN_TIME = g_ChaosCastle.GetRemainTime(iENTER_LEVEL);
			pResult.RemainTime = SET_NUMBERH(wREMAIN_TIME);
			pResult.RemainTime_LOW = SET_NUMBERL(wREMAIN_TIME);
			pResult.EnteredUser = g_ChaosCastle.GetCurEnteredUser(iENTER_LEVEL);
		}*/

		break;
	case 5:
		pResult.hEventType = 5;
		pResult.RemainTime = 0; // stub // g_IT_Event.GetRemainTime();
		pResult.RemainTime_LOW = 0;
		pResult.EnteredUser = 0;
		break;
	case 6:
		if (lpObj->m_PK_Level < 4)
		{
			pResult.hEventType = 6;
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		}
		else
		{
			GCServerCmd(lpObj, 0x39, 0, 0);
			return;
		}
	}

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
}



void GameProtocol::AllSendMsg(LPBYTE Msg, int size)
{
	for each(std::pair<int,CGameObject*> user in gGameObjects)
	{
		if (user.second->m_PlayerData->ConnectUser->Connected == PLAYER_PLAYING)
		{
			if (user.second->Type == OBJ_USER)
			{
				GIOCP.DataSend(user.second->m_PlayerData->ConnectUser->Index, Msg, size);
			}
		}
	}
}



void GameProtocol::AllSendSameMapMsg(UCHAR * Msg, int size, BYTE mapnumber)
{
	for each(std::pair<int, CGameObject*> user in gGameObjects)
	{
		if (user.second->m_PlayerData->ConnectUser->Connected == PLAYER_PLAYING)
		{
			if (user.second->Type == OBJ_USER)
			{
				if (user.second->MapNumber == mapnumber)
				{
					GIOCP.DataSend(user.second->m_PlayerData->ConnectUser->Index, Msg, size);
				}
			}
		}
	}
}


struct PMSG_PING
{
	PBMSG_HEAD h;	// C1:71
};


//#unused
void GameProtocol::GCSendPing(CGameObject* lpObj)
{
	PMSG_PING pMsgPing;

	PHeadSetB((LPBYTE)&pMsgPing, 0x71, sizeof(pMsgPing));
	lpObj->iPingTime = GetTickCount();

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsgPing, pMsgPing.h.size);
}


//#unused
void GameProtocol::GCPingSendRecv(PMSG_PING_RESULT* aRecv, CGameObject* lpObj)
{
	int PingTime = GetTickCount() - lpObj->iPingTime;

	MsgOutput(*lpObj, "Ping: %d ms", PingTime);
}



struct PMSG_REQ_REGISTER_EVENTCHIP
{
	PBMSG_HEAD2 h;	// C1:02
	int iIndex;	// 4
	BYTE Pos;	// 8
	char szUID[11];	// 9
};

struct PMSG_REQ_REGISTER_STONES
{
	PBMSG_HEAD2 h;	// C1:06
	int iIndex;	// 4
	BYTE iPosition;	// 8
	char szUID[11];	// 9
};


void GameProtocol::GCRegEventChipRecv(PMSG_REGEVENTCHIP* lpMsg, CGameObject* lpObj)
{
	if (!EVENCHIP_TYPE_RANGE(lpMsg->Type))
		return;


	if (lpObj->m_IfState.type == 1)
	{
		return;
	}

	if (lpObj->m_IfState.type == 7)
	{
		return;
	}

	if (lpObj->UseEventServer != FALSE)
	{
		PMSG_REGEVENTCHIP_RESULT Result;

		PHeadSetB((LPBYTE)&Result, 0x95, sizeof(Result));
		Result.ChipCount = -1;
		Result.Type = lpMsg->Type;

		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&Result, Result.h.size);

		return;
	}

	int Pos = lpMsg->ChipPos + INVENTORY_BAG_START;
	CItemObject* sitem = lpObj->pntInventory[Pos];

	if (!sitem->IsItem())
	{
		PMSG_REGEVENTCHIP_RESULT Result;

		PHeadSetB((LPBYTE)&Result, 0x95, sizeof(Result));
		Result.ChipCount = -1;
		Result.Type = lpMsg->Type;

		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&Result, Result.h.size);

		return;
	}

	lpObj->UseEventServer = TRUE;

	switch (lpMsg->Type)
	{
	case 0x00:
		if (sitem->m_Type == ITEMGET(14, 21) && sitem->m_Level == 0) // Rena
		{
			PMSG_REQ_REGISTER_EVENTCHIP pMsg;

			PHeadSubSetB((LPBYTE)&pMsg, 0xBE, 0x02, sizeof(pMsg));
			pMsg.iIndex = lpObj->m_Index;
			pMsg.Pos = Pos;
			strcpy(pMsg.szUID, lpObj->m_PlayerData->ConnectUser->AccountID);

			//wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg)); // TODO
		}
		else
		{
			PMSG_REGEVENTCHIP_RESULT Result;

			PHeadSetB((LPBYTE)&Result, 0x95, sizeof(Result));
			Result.ChipCount = -1;
			Result.Type = 0x00;

			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&Result, Result.h.size);
			lpObj->UseEventServer = FALSE;
		}
		break;
	case 0x01:
		if (sitem->m_Type == ITEMGET(14, 21) && sitem->m_Level == 1) // Stone
		{
			PMSG_REQ_REGISTER_STONES pMsg;

			PHeadSubSetB((LPBYTE)&pMsg, 0xBE, 0x06, sizeof(pMsg));
			pMsg.iIndex = lpObj->m_Index;
			pMsg.iPosition = Pos;
			strcpy(pMsg.szUID, lpObj->m_PlayerData->ConnectUser->AccountID);

			//wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg)); // TODO

		}
		else
		{
			PMSG_REGEVENTCHIP_RESULT Result;

			PHeadSetB((LPBYTE)&Result, 0x95, sizeof(Result));
			Result.ChipCount = -1;
			Result.Type = 0x01;

			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&Result, Result.h.size);
			lpObj->UseEventServer = FALSE;
		}
		break;
	default:
		lpObj->UseEventServer = FALSE;
		break;
	}
}

struct PMSG_REQ_REGISTER_MUTONUM
{
	PBMSG_HEAD2 h;	// C1:03
	int iIndex;	// 4
	char szUID[11];	// 8
};

void GameProtocol::GCGetMutoNumRecv(PMSG_GETMUTONUMBER* lpMsg, CGameObject* lpObj)
{
	if (lpObj->MutoNumber != 0)
	{
		char msg[255];
		wsprintf(msg, Lang.GetText(0, 527));
		GCServerMsgStringSend(msg, lpObj, 1);
		return;
	}

	if (lpObj->UseEventServer != FALSE)
		return;

	lpObj->UseEventServer = TRUE;

	if (!gObjFind10EventChip(*lpObj))
	{
		PMSG_GETMUTONUMBER_RESULT Result;

		PHeadSetB((LPBYTE)&Result, 0x96, sizeof(Result));
		Result.MutoNum[0] = -1;
		Result.MutoNum[1] = 0;
		Result.MutoNum[2] = 0;

		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&Result, Result.h.size);
		lpObj->UseEventServer = FALSE;

		return;
	}

	PMSG_REQ_REGISTER_MUTONUM pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xBE, 0x03, sizeof(pMsg));
	pMsg.iIndex = lpObj->m_Index;
	strcpy(pMsg.szUID, lpObj->m_PlayerData->ConnectUser->AccountID);

	//wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg)); // TODO

}



void GameProtocol::GCUseEndEventChipRescv(CGameObject* lpObj)
{
	
	if (lpObj->m_IfState.use && lpObj->m_IfState.type == 9)
	{
		lpObj->m_IfState.state = 0;
		lpObj->m_IfState.type = 0;
		lpObj->m_IfState.use = 0;
	}
}

struct PMSG_REQ_RESET_EVENTCHIP
{
	PBMSG_HEAD2 h;	// C1:[04:Rena] [09:Stone]
	int iIndex;	// 4
	char szUID[11];	// 8
};

void GameProtocol::GCUseRenaChangeZenRecv(PMSG_EXCHANGE_EVENTCHIP* lpMsg, CGameObject* lpObj)
{
	if (lpObj->UseEventServer)
		return;

	lpObj->UseEventServer = TRUE;

	PMSG_REQ_RESET_EVENTCHIP pMsg;

	if (lpMsg->btType == 1)	// Stone?
		PHeadSubSetB((LPBYTE)&pMsg, 0xBE, 0x09, sizeof(pMsg));
	else
		PHeadSubSetB((LPBYTE)&pMsg, 0xBE, 0x04, sizeof(pMsg));

	pMsg.iIndex = lpObj->m_Index;
	strcpy(pMsg.szUID, lpObj->m_PlayerData->ConnectUser->AccountID);

	//wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg)); // TODO

	if (lpMsg->btType == 0x01)
	{

	}
	else
	{

	}
}

struct PMSG_SEND_QEUSTINFO
{
	PBMSG_HEAD h;	// C1:A0
	BYTE Count;	// 3
	BYTE State[50];	// 4
};

/*void GameProtocol::CGRequestQuestInfo(CGameObject* lpObj)
{
	LPOBJ lpObj = *lpObj;

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	if (lpObj->m_PlayerData->m_SendQuestInfo != 0)
		return;

	int questcount = g_QuestInfo.GetQeustCount();
	int foundquest = 0;

	PMSG_SEND_QEUSTINFO pMsg;
	LPQUEST_INFO lpQuestInfo;

	memcpy(pMsg.State, lpObj->m_PlayerData->m_Quest, sizeof(pMsg.State));
	int i;
	for (i = 0; i < MAX_QUEST_INFO; i++)
	{
		lpQuestInfo = g_QuestInfo.GetQuestInfo(i);

		if (lpQuestInfo)
		{
			foundquest++;

			if (foundquest == questcount)
			{
				break;
			}
		}
	}

	int iSize = i / 4 + 5;

	PHeadSetB((LPBYTE)&pMsg, 0xA0, iSize);
	pMsg.Count = i;
	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, iSize);
	lpObj->m_PlayerData->m_SendQuestInfo = true;
}

void GameProtocol::GCSendQuestInfo(CGameObject* lpObj, int QuestIndex) // Third Quest
{
	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	CGRequestQuestInfo(lpObj);
	LPOBJ lpObj = *lpObj;
	PMSG_SETQUEST pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0xA1, sizeof(pMsg));
	pMsg.QuestIndex = QuestIndex;
	pMsg.State = g_QuestInfo.GetQuestStateBYTE(lpObj, QuestIndex);

	if (pMsg.State != 0)
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, sizeof(pMsg));
}


struct PMSG_SETQUEST_RESULT
{
	PBMSG_HEAD h;	// C1:A2
	BYTE QuestIndex;	// 3
	BYTE Result;	// 4
	BYTE State;	// 5
};


void GameProtocol::CGSetQuestState(PMSG_SETQUEST* lpMsg, CGameObject* lpObj)
{
	LPOBJ lpObj = *lpObj;
	PMSG_SETQUEST_RESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0xA2, sizeof(pMsg));
	pMsg.QuestIndex = lpMsg->QuestIndex;
	pMsg.Result = g_QuestInfo.SetQuestState(lpObj, lpMsg->QuestIndex, lpMsg->State);
	pMsg.State = g_QuestInfo.GetQuestStateBYTE(lpObj, lpMsg->QuestIndex);

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, sizeof(pMsg));
}


struct PMSG_SETQUEST_PRIZE
{
	PBMSG_HEAD h;	// C1:A3
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	BYTE Type;	// 5
	BYTE Count;	// 6
};


void GameProtocol::GCSendQuestPrize(CGameObject* lpObj, BYTE Type, BYTE Count)
{
	CGRequestQuestInfo(lpObj);
	LPOBJ lpObj = *lpObj;

	PMSG_SETQUEST_PRIZE pMsg;

	PHeadSetB((LPBYTE)&pMsg, 0xA3, sizeof(pMsg));
	pMsg.NumberH = SET_NUMBERH(lpObj->m_Index);
	pMsg.NumberL = SET_NUMBERL(lpObj->m_Index);
	pMsg.Type = Type;
	pMsg.Count = Count;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));
	MsgSendV2(lpObj, (LPBYTE)&pMsg, sizeof(pMsg));
}
*/


void GameProtocol::CGCloseWindow(CGameObject* lpObj)
{
	if (lpObj->m_IfState.use && lpObj->m_IfState.type)
	{
		if (lpObj->m_IfState.type == 7)
			return;

		if (lpObj->m_IfState.type == 10)	// Marlon
		{
			if (lpObj->TargetNpcNumber != -1 && getGameObject(lpObj->TargetNpcNumber)->Class == 229)
			{
				gQeustNpcTeleport.TalkRefDel();
				lpObj->TargetNpcNumber = -1;
			}
		}

		if (lpObj->TargetNpcNumber != -1 && getGameObject(lpObj->TargetNpcNumber)->Class == 568)
		{
			gQuestExpNpcTeleport.TalkRefDel();
			lpObj->TargetNpcNumber = -1;
		}

		if (lpObj->m_IfState.type == 3)
		{
			lpObj->TargetNpcNumber = -1;
		}

		if (lpObj->m_IfState.type == 1)
		{
			CGTradeCancelButtonRecv(lpObj);
		}

		if (lpObj->m_IfState.type == 6)
		{
			CGWarehouseUseEnd(lpObj);
		}

		if (lpObj->m_IfState.type == 13)
		{
			g_MixSystem.ChaosBoxInit(*lpObj);
			gObjInventoryCommit(*lpObj);
		}

		lpObj->m_IfState.state = 0;
		lpObj->m_IfState.type = 0;
		lpObj->m_IfState.use = 0;
	}
}



void GameProtocol::CGRequestEnterBloodCastle(PMSG_REQ_MOVEBLOODCASTLE* lpMsg, CGameObject* lpObj)
{
	/*
	BYTE btBridgeNumber = lpMsg->iBridgeNumber;
	BYTE btInvisibleCourtItemPos = lpMsg->iItemPos;
	BYTE btInvisibleCourtItemPos;
	int iITEM_LEVEL = 0;
	UINT64 dwITEM_SERIAL = 0;
	PMSG_RESULT_MOVEBLOODCASTLE pResult;

	PHeadSetB((LPBYTE)&pResult, 0x9A, sizeof(pResult));
	pResult.Result = 0;

	if (lpObj->Type != OBJ_USER)
		return;

	if (lpObj->m_IfState.use && lpObj->m_IfState.type != 12)
		return;

	BYTE btBridgeNumber;
	BYTE btGateNumber;

	int CurrLvl = lpObj->Level + lpObj->m_PlayerData->MasterLevel;

	if ((CurrLvl >= 1) && (CurrLvl <= 49))
	{
		pResult.Result = 4;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}
	if ((CurrLvl >= 50) && (CurrLvl <= 179))
	{
		btBridgeNumber = 0;
		btGateNumber = 66;
	}
	else if ((CurrLvl >= 180) && (CurrLvl <= 269))
	{
		btBridgeNumber = 1;
		btGateNumber = 67;
	}
	else if ((CurrLvl >= 270) && (CurrLvl <= 349))
	{
		btBridgeNumber = 2;
		btGateNumber = 68;
	}
	else if ((CurrLvl >= 350) && (CurrLvl <= 399))
	{
		btBridgeNumber = 3;
		btGateNumber = 69;
	}
	else if ((CurrLvl >= 400) && (CurrLvl <= 500))
	{
		btBridgeNumber = 4;
		btGateNumber = 70;
	}
	else if ((CurrLvl >= 501) && (CurrLvl <= 650))
	{
		btBridgeNumber = 5;
		btGateNumber = 71;
	}
	else if ((CurrLvl >= 651) && (CurrLvl <= 850))
	{
		btBridgeNumber = 6;
		btGateNumber = 80;
	}

	if (lpObj->Type != OBJ_USER)
		return;

	if (lpObj->m_IfState.use && lpObj->m_IfState.type != 12)
		return;

	BOOL bPlayerKiller = FALSE; //Season 2.5 add-on

	if (lpObj->PartyNumber >= 0) //Season 2.5 add-on
	{
		if (gParty.GetPKPartyPenalty(lpObj->PartyNumber) >= 5)
		{
			bPlayerKiller = TRUE;
		}
	}
	else if (lpObj->m_PK_Level >= 4)
	{
		bPlayerKiller = TRUE;
	}

	if (g_ConfigRead.data.common.PKEnterEventAllow == true)
	{
		bPlayerKiller = FALSE;
	}

	if (bPlayerKiller == TRUE)
	{
		pResult.Result = 7;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (g_BloodCastle.CheckCanEnter(btBridgeNumber) == false)
	{
		pResult.Result = 2;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	int Err = 0;

	for (int btInvisibleCourtItemPos = 12; btInvisibleCourtItemPos < 76; btInvisibleCourtItemPos++)
	{

		if (lpObj->pntInventory[btInvisibleCourtItemPos]->m_Type == ITEMGET(13, 18))	// Cloack's Invitation
		{
			if (lpObj->pntInventory[btInvisibleCourtItemPos]->m_Level == btBridgeNumber + 1)
			{

				gObjInventoryDeleteItem(*lpObj, btInvisibleCourtItemPos);
				GCInventoryItemDeleteSend(lpObj, btInvisibleCourtItemPos, 1);
				Err++;
				break;
			}

		}
		else if (lpObj->pntInventory[btInvisibleCourtItemPos]->m_Type == ITEMGET(13, 47)) //Ticket BC
		{
			if (lpObj->pntInventory[btInvisibleCourtItemPos]->m_Durability > 0.0f)
			{
				lpObj->pntInventory[btInvisibleCourtItemPos]->m_Durability -= 1.0f;
				if (lpObj->pntInventory[btInvisibleCourtItemPos]->m_Durability == 0.0f)
				{
					gObjInventoryDeleteItem(*lpObj, btInvisibleCourtItemPos);
					GCInventoryItemDeleteSend(lpObj, btInvisibleCourtItemPos, 1);
				}
				else
					GCItemDurSend2(lpObj, btInvisibleCourtItemPos, lpObj->pntInventory[btInvisibleCourtItemPos]->m_Durability, 0);
				Err++;
				break;
			}
		}

	}

	if (Err == 0)
	{
		pResult.Result = 1;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	int iBC_INDEX = g_BloodCastle.EnterUserBridge(btBridgeNumber, *lpObj);

	if (iBC_INDEX == -1)
	{
		pResult.Result = 5;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	lpObj->m_cBloodCastleIndex = btBridgeNumber;
	lpObj->m_cBloodCastleSubIndex = iBC_INDEX;
	lpObj->m_iBloodCastleEXP = 0;
	lpObj->m_IfState.state = 0;
	lpObj->m_IfState.type = 0;
	lpObj->m_IfState.use = 0;

	g_BloodCastle.SearchUserDeleteQuestItem(*lpObj);
	char szTemp[256];
	wsprintf(szTemp, Lang.GetText(0, 74), btBridgeNumber + 1);
	GCServerMsgStringSend(szTemp, lpObj, 1);

	gObjMoveGate(*lpObj, btGateNumber);
	*/
}

struct PMSG_RESULT_MOVECHAOSCASTLE
{
	PBMSG_HEAD2 h;	// C1:AF:01
	BYTE Result;	// 4
};

void GameProtocol::CGRequestEnterChaosCastle(PMSG_REQ_MOVECHAOSCASTLE* lpMsg, CGameObject* lpObj)
{
	/*
	BYTE btChaosCastleNumber = lpMsg->iChaosCastleNumber - 1;
	BYTE btGuardMailSetItemPos = lpMsg->iItemPos;
	int iITEM_LEVEL = 0;
	UINT64 dwITEM_SERIAL = 0;
	PMSG_RESULT_MOVECHAOSCASTLE pResult;

	PHeadSubSetB((LPBYTE)&pResult, 0xAF, 0x01, sizeof(pResult));
	pResult.Result = 0;

	if (lpObj->Type != OBJ_USER)
		return;

	if (BC_MAP_RANGE(lpObj->MapNumber) || CC_MAP_RANGE(lpObj->MapNumber) || DS_MAP_RANGE(lpObj->MapNumber) ||
		IT_MAP_RANGE(lpObj->MapNumber) || DG_MAP_RANGE(lpObj->MapNumber) || IMPERIAL_MAP_RANGE(lpObj->MapNumber) ||
		lpObj->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL || g_ConfigRead.server.GetServerType() == SERVER_ARCA)
	{
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 118));
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pNotice, pNotice.h.size);
		return;
	}

	if (lpObj->m_IfState.use && lpObj->m_IfState.type != 12)
		return;

	if (lpObj->m_PlayerData->m_bUsePartyMatching)
	{
		PMSG_ANS_PARTYMATCHING_ERROR pMsg;
		PHeadSubSetB((LPBYTE)&pMsg, 0xEF, 0x09, sizeof(pMsg));
		pMsg.iResult = -1;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
	}

	if (g_ConfigRead.pk.bPkPenaltyDisable == FALSE)
	{
		BOOL bPlayerKiller = FALSE; //Season 2.5 add-on

		if (lpObj->PartyNumber >= 0) //Season 2.5 add-on
		{
			if (gParty.GetPKPartyPenalty(lpObj->PartyNumber) >= 6)
			{
				bPlayerKiller = TRUE;
			}
		}
		else if (lpObj->m_PK_Level >= 6)
		{
			bPlayerKiller = TRUE;
		}

		if (bPlayerKiller == TRUE)
		{
			PMSG_NOTICE pNotice;
			TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 119));
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pNotice, pNotice.h.size);
			return;
		}
	}

	if (gObjDuelCheck(*lpObj) == TRUE)
	{
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 120));
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pNotice, pNotice.h.size);
		return;
	}

	if (lpObj->m_PlayerData->lpGuild)
	{
		if (lpObj->m_PlayerData->lpGuild->WarState == 1)
		{
			PMSG_NOTICE pNotice;
			TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 121));
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pNotice, pNotice.h.size);
			return;
		}
	}

	if (lpObj->pntInventory[lpObj->m_btInvenPetPos]->m_Type == ITEMGET(13, 3) ||	// Dino
		lpObj->pntInventory[lpObj->m_btInvenPetPos]->m_Type == ITEMGET(13, 2) ||	// Uniria
		lpObj->pntInventory[lpObj->m_btInvenPetPos]->m_Type == ITEMGET(13, 37))	// Fenrir
	{
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 122));
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pNotice, pNotice.h.size);
		return;
	}

	BOOL bPlayerKiller = FALSE; //Season 2.5 add-on

	if (lpObj->PartyNumber >= 0) //Season 2.5 add-on
	{
		if (gParty.GetPKPartyPenalty(lpObj->PartyNumber) >= 5)
		{
			bPlayerKiller = TRUE;
		}
	}
	else if (lpObj->m_PK_Level >= 4)
	{
		bPlayerKiller = TRUE;
	}

	if (bPlayerKiller == TRUE)
	{
		pResult.Result = 8;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	int iENTER_LEVEL = g_ChaosCastle.GetUserLevelToEnter(*lpObj);

	if (iENTER_LEVEL == -1)
	{
		pResult.Result = 1;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (g_ChaosCastle.CheckCanEnter(iENTER_LEVEL) == false)
	{
		pResult.Result = 2;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (lpObj->pntInventory[btGuardMailSetItemPos]->IsItem() == TRUE)
	{
		if (lpObj->pntInventory[btGuardMailSetItemPos]->m_Type == ITEMGET(13, 29)) // Armor of Guardman
		{
			iITEM_LEVEL = lpObj->pntInventory[btGuardMailSetItemPos]->m_Level;
			dwITEM_SERIAL = lpObj->pntInventory[btGuardMailSetItemPos]->m_Number;
		}
		else
		{
			pResult.Result = 1;
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
			return;
		}
	}
	else
	{
		pResult.Result = 1;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (g_ChaosCastle.CheckWearingMOPH(*lpObj) == TRUE)
	{
		GCServerMsgStringSend(Lang.GetText(0, 123), lpObj, 1);
		return;
	}

	int iBC_INDEX = -1;

	if (g_ChaosCastle.CheckUserEnterMoney(*lpObj, iENTER_LEVEL))
	{
		iBC_INDEX = g_ChaosCastle.EnterUserChaosCastle(iENTER_LEVEL, *lpObj);

		if (iBC_INDEX == -1)
		{
			pResult.Result = 5;
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
			return;
		}
	}
	else
	{
		pResult.Result = 7;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	BOOL bRES = g_ChaosCastle.PayUserEnterMoney(*lpObj, iENTER_LEVEL);

	if (bRES == FALSE)
	{
		g_ChaosCastle.LeaveUserChaosCastle(iENTER_LEVEL, *lpObj);
		pResult.Result = 5;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	gObjInventoryDeleteItem(*lpObj, btGuardMailSetItemPos);
	GCInventoryItemDeleteSend(lpObj, btGuardMailSetItemPos, 1);

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

	lpObj->m_cChaosCastleIndex = iENTER_LEVEL;
	lpObj->m_cChaosCastleSubIndex = iBC_INDEX;
	lpObj->m_IfState.state = 0;
	lpObj->m_IfState.type = 0;
	lpObj->m_IfState.use = 0;

	if (lpObj->m_bPShopOpen == true)
		g_PersonalStore.CGPShopReqClose(*lpObj);

	if (lpObj->PartyNumber >= 0)
	{
		PMSG_PARTYDELUSER pMsg;
		int pindex = gParty.GetIndex(lpObj->PartyNumber, *lpObj, lpObj->DBNumber);

		if (pindex >= 0)
		{
			pMsg.Number = pindex;
			CGPartyDelUser(&pMsg, lpObj, 0);
		}
	}

	if (lpObj->Class == CLASS_ELF && lpObj->m_RecallMon >= 0)
	{
		GCRecallMonLife(lpObj, 60, 0);
		gObjMonsterCallKill(*lpObj);
	}

	char szTemp[256];
	wsprintf(szTemp, Lang.GetText(0, 124), iENTER_LEVEL + 1);
	GCServerMsgStringSend(szTemp, lpObj, 1);

	switch (iENTER_LEVEL + 1)
	{
	case 1:
		gObjMoveGate(*lpObj, 82);
		break;
	case 2:
		gObjMoveGate(*lpObj, 83);
		break;
	case 3:
		gObjMoveGate(*lpObj, 84);
		break;
	case 4:
		gObjMoveGate(*lpObj, 85);
		break;
	case 5:
		gObjMoveGate(*lpObj, 86);
		break;
	case 6:
		gObjMoveGate(*lpObj, 87);
		break;
	case 7:
		gObjMoveGate(*lpObj, 272);
		break;
	}
	*/
}

void GameProtocol::CGRequestRepositionUserInChaosCastle(PMSG_REQ_REPOSUSER_IN_CC * lpMsg, CGameObject* lpObj)
{
	/*if (!CC_MAP_RANGE(lpObj->MapNumber))
		return;

	if (g_ChaosCastle.ObjSetPosition(*lpObj, lpObj->X, lpObj->Y) == TRUE)
		lpObj->m_iChaosCastleBlowTime = GetTickCount();*/
}



void GameProtocol::CGRequestEventEnterCount(PMSG_REQ_CL_ENTERCOUNT* lpMsg, CGameObject* lpObj)
{
	if (!lpMsg)
		return;

	switch (lpMsg->btEventType)
	{
	case 0x02:
		EGReqBloodCastleEnterCount(*lpObj);
		break;
	}
}

struct PMSG_REQ_2ANIV_SERIAL
{
	PBMSG_HEAD2 h;	// C1:08
	int iIndex;	// 8
	char szUID[MAX_ACCOUNT_LEN + 1];	// 8
	char SERIAL1[5];	// 13
	char SERIAL2[5];	// 18
	char SERIAL3[5];	// 1D
	int iMEMB_GUID;	// 24
};

void GameProtocol::CGRequestLottoRegister(PMSG_REQ_2ANV_LOTTO_EVENT* lpMsg, CGameObject* lpObj)
{
	PMSG_REQ_2ANIV_SERIAL pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xBE, 0x08, sizeof(pMsg));

	if (lpObj->UseEventServer)
		return;

	lpObj->UseEventServer = TRUE;
	pMsg.iIndex = lpObj->m_Index;
	pMsg.iMEMB_GUID = lpObj->DBNumber;
	memcpy(pMsg.szUID, lpObj->m_PlayerData->ConnectUser->AccountID, MAX_ACCOUNT_LEN);
	pMsg.szUID[MAX_ACCOUNT_LEN] = 0;
	memcpy(pMsg.SERIAL1, lpMsg->SERIAL1, 4);
	pMsg.SERIAL1[4] = 0;
	memcpy(pMsg.SERIAL2, lpMsg->SERIAL2, 4);
	pMsg.SERIAL2[4] = 0;
	memcpy(pMsg.SERIAL3, lpMsg->SERIAL3, 4);
	pMsg.SERIAL3[4] = 0;

	//wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg)); // TODO

	sLog->outBasic("[Mu_2Anv_Event] [%s][%s] Register Lotto Number (Serial: %s-%s-%s)",
		lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, pMsg.SERIAL1, pMsg.SERIAL2, pMsg.SERIAL3);
}

struct SDHP_CHARACTER_TRANSFER
{
	PBMSG_HEAD h;	// C1:CF
	char Account[10];	// 3
	short Number;	// E
};


void GameProtocol::CGReqMoveOtherServer(PMSG_REQ_MOVE_OTHERSERVER * lpMsg, CGameObject* lpObj)
{
	if (!gEnableServerDivision)
		return;

	if (lpObj->m_MoveOtherServer)
		return;

	lpObj->m_MoveOtherServer = true;

	PMSG_DEFRESULT pResult;
	PHeadSetB((LPBYTE)&pResult, 0x99, sizeof(pResult));

	if (!gObjPasswordCheck(*lpObj, lpMsg->LastJoominNumber))
	{
		pResult.result = 0;

		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		lpObj->m_MoveOtherServer = false;

		sLog->outBasic("[CharTrasfer] Fail (JoominNumber) [%s][%s]",
			lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);

		GCServerMsgStringSend(Lang.GetText(0, 528), lpObj, 1);

		return;
	}

	SDHP_CHARACTER_TRANSFER pCharTransfer;

	GJSetCharacterInfo(lpObj, FALSE);
	PHeadSetB((LPBYTE)&pCharTransfer, 0xCF, sizeof(pCharTransfer));
	pCharTransfer.Number = lpObj->m_Index;
	memcpy(pCharTransfer.Account, lpObj->m_PlayerData->ConnectUser->AccountID, sizeof(pCharTransfer.Account));

	//wsDataCli.DataSend((PCHAR)&pCharTransfer, sizeof(pCharTransfer)); // TODO

	sLog->outBasic("[CharTrasfer] Request Character Trasfer [%s][%s]",
		lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
}



void GameProtocol::GCPacketCheckSumRecv(PMSG_PACKETCHECKSUM * aRecv, CGameObject* lpObj)
{
	gPacketCheckSum.AddCheckSum(*lpObj, aRecv->funcindex, aRecv->CheckSum);
}

struct PMSG_GETITEMINFO_FOR_PARTY
{
	PBMSG_HEAD h;	// C1:47
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	UINT ItemInfo;	// 8
	BYTE Level;	// C
};

void GameProtocol::GCSendGetItemInfoForParty(CGameObject* lpObj, CMapItem * lpItem)
{
	if (!lpItem)
		return;

	if (lpObj->PartyNumber >= 0)
	{
		PMSG_GETITEMINFO_FOR_PARTY pMsg;

		pMsg.h.c = 0xC1;
		pMsg.h.headcode = 0x47;
		pMsg.h.size = sizeof(pMsg);
		pMsg.NumberH = SET_NUMBERH(lpObj);
		pMsg.NumberL = SET_NUMBERL(lpObj);
		pMsg.ItemInfo = lpItem->m_Type & (ITEMGET(MAX_TYPE_ITEMS - 1, MAX_SUBTYPE_ITEMS - 1));

		if (lpItem->m_Option1)
			pMsg.ItemInfo |= 0x2000;

		if (lpItem->m_Option2)
			pMsg.ItemInfo |= 0x4000;

		if (lpItem->m_Option3)
			pMsg.ItemInfo |= 0x8000;

		if (lpItem->m_NewOption)
			pMsg.ItemInfo |= 0x10000;

		if (lpItem->m_SetOption)
			pMsg.ItemInfo |= 0x20000;

		if (g_kItemSystemFor380.Is380OptionItem(lpItem))
			pMsg.ItemInfo |= 0x40000;

		pMsg.Level = lpItem->m_Level;

		int partycount = gParty.GetPartyCount(lpObj->PartyNumber);

		if (partycount >= 0)
		{
			for (int n = 0; n < MAX_USER_IN_PARTY; n++)
			{
				int number = gParty.m_PartyS[lpObj->PartyNumber].Number[n];

				if (number >= 0)
				{
					GIOCP.DataSend(number, (LPBYTE)&pMsg, pMsg.h.size);
				}
			}
		}
	}
}


struct PMSG_EFFECTINFO
{
	PBMSG_HEAD h;	// C1:48
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	BYTE btType;	// 5
};

void GameProtocol::GCSendEffectInfo(CGameObject* lpObj, BYTE btType)
{

	PMSG_EFFECTINFO pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x48;
	pMsg.h.size = sizeof(pMsg);
	pMsg.NumberH = SET_NUMBERH(lpObj);
	pMsg.NumberL = SET_NUMBERL(lpObj);
	pMsg.btType = btType;

	if (btType == 17)
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
	}

	MsgSendV2(lpObj, (LPBYTE)&pMsg, pMsg.h.size);
}



void GameProtocol::CGRequestPetItemCommand(PMSG_REQUEST_PET_ITEM_COMMAND * lpMsg, CGameObject* lpObj)
{

	if (lpMsg->PetType)
		return;

	int iTargetIndex = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
	//gDarkSpirit[lpObj->m_Index].ChangeCommand(lpMsg->Command, iTargetIndex); // TODO
}



void GameProtocol::CGRequestPetItemInfo(PMSG_REQUEST_PET_ITEMINFO * lpMsg, CGameObject* lpObj)
{

	if (lpMsg->PetType != 0 && lpMsg->PetType != 1)
		return;

	PMSG_SEND_PET_ITEMINFO pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0xA9, sizeof(pMsg));
	pMsg.PetType = lpMsg->PetType;
	pMsg.InvenType = lpMsg->InvenType;
	pMsg.nPos = lpMsg->nPos;

	if (lpMsg->InvenType == 0)	// Inventory
	{
		if (!INVENTORY_RANGE(lpMsg->nPos))
		{
			sLog->outBasic("[PetItem] [%s][%s] RequestPetItem Invalid Iventory nPos %d",
				lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpMsg->nPos);

			return;
		}

		if (lpObj->pntInventory[lpMsg->nPos]->IsItem())
		{
			pMsg.Level = lpObj->pntInventory[lpMsg->nPos]->m_PetItem_Level;
			pMsg.Exp = lpObj->pntInventory[lpMsg->nPos]->m_PetItem_Exp - gPetItemExp.m_DarkSpiritExpTable[pMsg.Level];
			pMsg.Dur = (BYTE)lpObj->pntInventory[lpMsg->nPos]->m_Durability;

			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));
		}
	}
	else if (lpMsg->InvenType == 1)	// Warehouse
	{
		if (lpObj->LoadWareHouseInfo != false)
		{
			if (!WAREHOUSE_RANGE(lpMsg->nPos))
			{
				sLog->outBasic("[PetItem] [%s][%s] RequestPetItem Invalid WareHouse nPos %d",
					lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpMsg->nPos);

				return;
			}

			if (lpObj->pntWarehouse[lpMsg->nPos]->IsItem())
			{
				pMsg.Level = lpObj->pntWarehouse[lpMsg->nPos]->m_PetItem_Level;
				pMsg.Exp = lpObj->pntWarehouse[lpMsg->nPos]->m_PetItem_Exp - gPetItemExp.m_DarkSpiritExpTable[pMsg.Level];
				pMsg.Dur = (BYTE)lpObj->pntInventory[lpMsg->nPos]->m_Durability;

				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));
			}
		}
	}
	else if (lpMsg->InvenType == 2)	// Trade
	{
		if (lpObj->m_IfState.use < 1 || lpObj->m_IfState.type != 1)
			return;

		if (!TRADE_BOX_RANGE(lpMsg->nPos))
		{
			sLog->outBasic("[PetItem] [%s][%s] RequestPetItem Invalid Trade nPos %d",
				lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpMsg->nPos);

			return;
		}

		if (lpObj->pntTrade[lpMsg->nPos]->IsItem())
		{
			pMsg.Level = lpObj->pntTrade[lpMsg->nPos]->m_PetItem_Level;
			pMsg.Exp = lpObj->pntTrade[lpMsg->nPos]->m_PetItem_Exp - gPetItemExp.m_DarkSpiritExpTable[pMsg.Level];
			pMsg.Dur = (BYTE)lpObj->pntInventory[lpMsg->nPos]->m_Durability;

			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));
		}
	}
	else if (lpMsg->InvenType == 3)	// Target Trade
	{
		if (lpObj->m_IfState.use < 1 || lpObj->m_IfState.type != 1)
			return;

		int iTargetIndex = lpObj->TargetNumber;
		CGameObject* lpObjTarget = getGameObject(iTargetIndex);
		

		if (lpObjTarget->m_IfState.use < 1 || lpObjTarget->m_IfState.type != 1)
			return;

		if (!TRADE_BOX_RANGE(lpMsg->nPos))
		{
			sLog->outBasic("[PetItem] [%s][%s] RequestPetItem Invalid TargetTrade nPos %d",
				lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpMsg->nPos);

			return;
		}

		if (lpObjTarget->pntTrade[lpMsg->nPos]->IsItem())
		{
			pMsg.Level = lpObjTarget->pntTrade[lpMsg->nPos]->m_PetItem_Level;
			pMsg.Exp = lpObjTarget->pntTrade[lpMsg->nPos]->m_PetItem_Exp - gPetItemExp.m_DarkSpiritExpTable[pMsg.Level];
			pMsg.Dur = (BYTE)lpObj->pntInventory[lpMsg->nPos]->m_Durability;

			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));
		}
	}
	else if (lpMsg->InvenType == 4)	// Chaos
	{
		if (!CHAOS_BOX_RANGE(lpMsg->nPos))
		{
			sLog->outBasic("[PetItem] [%s][%s] RequestPetItem Invalid Chaos nPos %d",
				lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpMsg->nPos);

			return;
		}

		if (lpObj->pntChaosBox[lpMsg->nPos]->IsItem())
		{
			pMsg.Level = lpObj->pntChaosBox[lpMsg->nPos]->m_PetItem_Level;
			pMsg.Exp = lpObj->pntChaosBox[lpMsg->nPos]->m_PetItem_Exp - gPetItemExp.m_DarkSpiritExpTable[pMsg.Level];
			pMsg.Dur = (BYTE)lpObj->pntInventory[lpMsg->nPos]->m_Durability;

			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));
		}
	}
	else if (lpMsg->InvenType == 5)	// Personal Shop
	{
		if (!INVENTORY_RANGE(lpMsg->nPos))
		{
			sLog->outBasic("[PetItem] [%s][%s] RequestPetItem Invalid Iventory nPos %d",
				lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpMsg->nPos);

			return;
		}

		if (lpObj->m_iPShopDealerIndex != -1)
		{
			CGameObject* lpObjDealer = getGameObject(lpObj->m_iPShopDealerIndex);
			if (lpObjDealer->pntInventory[lpMsg->nPos]->IsItem())
			{
				pMsg.Level = lpObjDealer->pntInventory[lpMsg->nPos]->m_PetItem_Level;
				pMsg.Exp = lpObjDealer->pntInventory[lpMsg->nPos]->m_PetItem_Exp - gPetItemExp.m_DarkSpiritExpTable[pMsg.Level];
				pMsg.Dur = (BYTE)lpObj->pntInventory[lpMsg->nPos]->m_Durability;

				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));
			}
		}
	}
}



struct PMSG_ANS_GUILDVIEWPORT
{
	PBMSG_HEAD h;	// C1:66
	int GuildNumber;	// 4
	BYTE btGuildType;	// 8
	BYTE UnionName[8];	// 9
	char GuildName[8];	// 11
	BYTE Mark[32];	// 19
};

/*
void GameProtocol::GCGuildViewportInfo(PMSG_REQ_GUILDVIEWPORT * aRecv, CGameObject* lpObj)
{
	if (!gObjIsConnectedGP(lpObj))
	{
		sLog->outBasic("error-L2 : Index %s %d", __FILE__, __LINE__);
		return;
	}

	if (!ObjectMaxRange(lpObj))
		return;

	LPOBJ lpObj = *lpObj;
	DWORD dwGuildNumber = aRecv->GuildNumber;
	_GUILD_INFO_STRUCT * lpGuildInfo = Guild.SearchGuild_Number(dwGuildNumber);

	if (lpGuildInfo)
	{
		PMSG_ANS_GUILDVIEWPORT pMsg;

		pMsg.h.c = 0xC1;
		pMsg.h.headcode = 0x66;
		pMsg.h.size = sizeof(pMsg);
		pMsg.GuildNumber = lpGuildInfo->Number;
		pMsg.btGuildType = lpGuildInfo->btGuildType;

		TUnionInfo * pUnionInfo = UnionManager.SearchUnion(lpGuildInfo->iGuildUnion);

		if (pUnionInfo)
			memcpy(pMsg.UnionName, pUnionInfo->m_szMasterGuild, MAX_GUILD_LEN);
		else
			pMsg.UnionName[0] = '\0';

		memcpy(pMsg.GuildName, lpGuildInfo->Name, MAX_GUILD_LEN);
		memcpy(pMsg.Mark, lpGuildInfo->Mark, sizeof(pMsg.Mark));

		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));
	}
	else
	{
		sLog->outBasic("GuildViewportInfo Fail : [%s] GuildNumber : %d",
			lpObj->Name, dwGuildNumber);
	}
}





void GameProtocol::CGGuildAssignStatus(PMSG_GUILD_ASSIGN_STATUS_REQ * aRecv, CGameObject* lpObj)
{
	if (!gObjIsConnectedGP(lpObj))
	{
		sLog->outBasic("error-L2 : Index %s %d", __FILE__, __LINE__);
		return;
	}

	if (!ObjectMaxRange(lpObj))
		return;

	LPOBJ lpObj = *lpObj;
	PMSG_GUILD_ASSIGN_STATUS_RESULT pMsg = { 0 };

	pMsg.h.set((LPBYTE)&pMsg, 0xE1, sizeof(pMsg));
	pMsg.btType = aRecv->btType;
	memcpy(pMsg.szTagetName, aRecv->szTagetName, sizeof(pMsg.szTagetName));

	if (lpObj->m_PlayerData->GuildNumber <= 0 || lpObj->m_PlayerData->lpGuild == NULL)
	{
		pMsg.btResult = GUILD_ANS_NOTEXIST_GUILD;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));
		return;
	}

	if (lpObj->m_PlayerData->GuildStatus != G_MASTER)
	{
		pMsg.btResult = GUILD_ANS_NOTEXIST_PERMISSION;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));
		return;
	}

	char szTargetName[MAX_ACCOUNT_LEN + 1] = { 0 };
	memcpy(szTargetName, aRecv->szTagetName, MAX_ACCOUNT_LEN);

	int iArcaBattleState = g_ArcaBattle.GetState();

	if (iArcaBattleState > 2 && iArcaBattleState < 9)
	{
		pMsg.btResult = GUILD_ANS_ARCA_BATTLE_ONGOING;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));
		return;
	}

	if (!strcmp(lpObj->Name, szTargetName))
	{
		pMsg.btResult = GUILD_ANS_NOTEXIST_PERMISSION;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));
		return;
	}

	if (lpObj->m_PlayerData->lpGuild == NULL)
		return;

	if (aRecv->btType == 1 || aRecv->btType == 2)
	{
		int iSubMasterCount = 0;
		int iBattleMasterCount = 0;

		for (int n = 0; n < MAX_USER_GUILD; n++)
		{
			if (lpObj->m_PlayerData->lpGuild->Use[n] > 0)
			{
				if (lpObj->m_PlayerData->lpGuild->GuildStatus[n] == G_SUB_MASTER)
				{
					iSubMasterCount++;
				}
				else if (lpObj->m_PlayerData->lpGuild->GuildStatus[n] == G_BATTLE_MASTER)
				{
					iBattleMasterCount++;
				}
			}
		}

		if (aRecv->btGuildStatus == G_SUB_MASTER)
		{
			if (iSubMasterCount == 0)
			{
				GDGuildReqAssignStatus(lpObj, aRecv->btType, aRecv->szTagetName, aRecv->btGuildStatus);
				return;
			}

			pMsg.btResult = GUILD_ANS_NOTEXIST_EXTRA_STATUS;
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));
			return;
		}
		else if (aRecv->btGuildStatus == G_BATTLE_MASTER)
		{
			if (iBattleMasterCount < (((lpObj->Level + lpObj->m_PlayerData->MasterLevel) / 200) + 1))
			{
				GDGuildReqAssignStatus(lpObj, aRecv->btType, aRecv->szTagetName, aRecv->btGuildStatus);
				return;
			}

			pMsg.btResult = GUILD_ANS_NOTEXIST_EXTRA_STATUS;
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));
			return;
		}
		else if (aRecv->btGuildStatus == G_PERSON)
		{
			GDGuildReqAssignStatus(lpObj, aRecv->btType, aRecv->szTagetName, aRecv->btGuildStatus);
			return;
		}
		else
		{
			return;
		}
	}

	if (aRecv->btType == 3)
	{
		GDGuildReqAssignStatus(lpObj, aRecv->btType, aRecv->szTagetName, aRecv->btGuildStatus);
	}
}



void GameProtocol::CGGuildAssignType(PMSG_GUILD_ASSIGN_TYPE_REQ * aRecv, CGameObject* lpObj)
{
	if (!gObjIsConnectedGP(lpObj))
	{
		sLog->outBasic("error-L2 : Index %s %d", __FILE__, __LINE__);
		return;
	}

	if (!ObjectMaxRange(lpObj))
		return;

	LPOBJ lpObj = *lpObj;
	PMSG_GUILD_ASSIGN_TYPE_RESULT pMsg = { 0 };

	pMsg.h.set((LPBYTE)&pMsg, 0xE2, sizeof(pMsg));
	pMsg.btGuildType = aRecv->btGuildType;

	if (lpObj->m_PlayerData->GuildNumber <= 0 || lpObj->m_PlayerData->lpGuild == NULL)
	{
		pMsg.btResult = GUILD_ANS_NOTEXIST_GUILD;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));
		return;
	}

	if (lpObj->m_PlayerData->GuildStatus != G_MASTER)
	{
		pMsg.btResult = GUILD_ANS_NOTEXIST_PERMISSION;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));
		return;
	}

	GDGuildReqAssignType(lpObj, aRecv->btGuildType);
}
*/

/*
void GameProtocol::CGRelationShipReqJoinBreakOff(PMSG_RELATIONSHIP_JOIN_BREAKOFF_REQ * aRecv, CGameObject* lpObj)
{
	if (g_CastleSiegeSync.GetCastleState() >= 5 && g_CastleSiegeSync.GetCastleState() <= 7)
	{
		MsgOutput(lpObj, Lang.GetText(0, 197));
		return;
	}

	int iTargetUserIndex = MAKE_NUMBERW(aRecv->btTargetUserIndexH, aRecv->btTargetUserIndexL);

	if (!ObjectMaxRange(lpObj) || !ObjectMaxRange(iTargetUserIndex))
		return;

	if (!PacketCheckTime(lpObj))
		return;

	if (gObjIsConnectedGP(iTargetUserIndex) == FALSE)
	{
		GCResultSend(lpObj, 0x51, 3);
		return;
	}

	int iArcaBattleState = g_ArcaBattle.GetState();

	if (iArcaBattleState > 2 && iArcaBattleState < 9)
	{
		//TRACE_LOG("lpObj, 0x51, 0xA5");
		GCResultSend(lpObj, 0x51, 0xA4);
		return;
	}

	if (g_NewPVP.IsDuel(lpObj) || g_NewPVP.IsDuel(gObj[iTargetUserIndex]))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 320), lpObj, 1);
		return;
	}

	if (g_NewPVP.IsObserver(lpObj) || g_NewPVP.IsObserver(gObj[iTargetUserIndex]))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 321), lpObj, 1);
		return;
	}

	if (!lpObj->m_PlayerData->lpGuild || !gObj[iTargetUserIndex].m_PlayerData->lpGuild)
	{
		GCResultSend(lpObj, 0x51, 5);
		return;
	}

	if (lpObj->m_PlayerData->GuildStatus != G_MASTER && gObj[iTargetUserIndex].m_PlayerData->GuildStatus != G_MASTER)
	{
		GCResultSend(lpObj, 0x51, 4);
		return;
	}

	if (lpObj->m_IfState.use > 0)
	{
		GCResultSend(lpObj, 0x51, 6);
		return;
	}

	if (gObj[iTargetUserIndex].m_IfState.use > 0)
	{
		GCResultSend(lpObj, 0x51, 6);
		return;
	}

	PMSG_RELATIONSHIP_JOIN_BREAKOFF_ANS ErrMsg = { 0 };

	PHeadSetB((LPBYTE)&ErrMsg, 0xE6, sizeof(ErrMsg));
	ErrMsg.btResult = 0;
	ErrMsg.btRequestType = aRecv->btRequestType;
	ErrMsg.btRelationShipType = aRecv->btRelationShipType;
	ErrMsg.btTargetUserIndexH = aRecv->btTargetUserIndexH;
	ErrMsg.btTargetUserIndexL = aRecv->btTargetUserIndexL;

	LPOBJ lpObj = *lpObj;
	LPOBJ lpTargetObj = &gObj[iTargetUserIndex];

	if (lpObj->m_PlayerData->GuildStatus != G_MASTER || lpTargetObj->m_PlayerData->GuildStatus != G_MASTER)
	{
		ErrMsg.btResult = GUILD_ANS_NOTEXIST_PERMISSION;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&ErrMsg, ErrMsg.h.size);
		return;
	}

	_GUILD_INFO_STRUCT * lpGuildInfo = lpObj->m_PlayerData->lpGuild;
	_GUILD_INFO_STRUCT * lpTargetGuildInfo = gObj[iTargetUserIndex].m_PlayerData->lpGuild;
	BYTE btRelationShip = gObjGetRelationShip(lpObj, lpTargetObj);

	if (aRecv->btRequestType == 1)
	{
		if (aRecv->btRelationShipType == 1)
		{
			if (lpTargetGuildInfo->Count < g_ConfigRead.data.common.AllianceMinGuildMember)
			{
				ErrMsg.btResult = GUILD_ANS_CANNOT_BE_UNION_MASTER;
				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&ErrMsg, ErrMsg.h.size);
				return;
			}

			if (lpTargetGuildInfo->iGuildUnion != 0 && lpTargetGuildInfo->iGuildUnion == lpTargetGuildInfo->Number)	// #error
			{
				if (UnionManager.GetGuildRelationShipCount(lpTargetGuildInfo->iGuildUnion, 1) >= g_ConfigRead.data.common.AllianceMaxGuilds)
				{
					ErrMsg.btResult = GUILD_ANS_EXCEED_MAX_UNION_MEMBER;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&ErrMsg, ErrMsg.h.size);
					return;
				}
			}

			char szCastleOwnerGuildName[MAX_GUILD_LEN + 1] = { 0 };
			memcpy(szCastleOwnerGuildName, g_CastleSiegeSync.GetCastleOwnerGuild(), MAX_GUILD_LEN);

			if (!strcmp(lpGuildInfo->Name, szCastleOwnerGuildName))
			{
				ErrMsg.btResult = GUILD_ANS_EXIST_UNION;
				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&ErrMsg, ErrMsg.h.size);
				return;
			}

			if (lpGuildInfo->iGuildUnion)
			{
				ErrMsg.btResult = GUILD_ANS_EXIST_UNION;
				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&ErrMsg, ErrMsg.h.size);
				return;
			}

			if (btRelationShip == 2)
			{
				ErrMsg.btResult = GUILD_ANS_EXIST_RELATIONSHIP_RIVAL;
				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&ErrMsg, ErrMsg.h.size);
				return;
			}

			if (lpTargetGuildInfo->iGuildUnion && lpTargetGuildInfo->Number != lpTargetGuildInfo->iGuildUnion)	// #error???
			{
				ErrMsg.btResult = GUIDL_ANS_NOT_UNION_MASTER;
				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&ErrMsg, ErrMsg.h.size);
				return;
			}

			int UnionMasterInfluence = g_GensSystem.GetGensInfluence(lpTargetObj);
			int GuildMasterInfluence = g_GensSystem.GetGensInfluence(lpObj);

			if (UnionMasterInfluence == NONE_INFLUENCE)
			{
				ErrMsg.btResult = GUILD_ANS_NOT_UNION_MASTER_GENS;
				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&ErrMsg, ErrMsg.h.size);
				return;
			}

			if (GuildMasterInfluence == NONE_INFLUENCE)
			{
				ErrMsg.btResult = GUILD_ANS_NOT_GENS;
				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&ErrMsg, ErrMsg.h.size);
				return;
			}

			if (UnionMasterInfluence != GuildMasterInfluence && g_GensSystem.CanGensJoinUnionWhileOppositeGens() == FALSE)
			{
				ErrMsg.btResult = GUILD_ANS_WRONG_GENS;
				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&ErrMsg, ErrMsg.h.size);
				return;
			}

		}
		else if (aRecv->btRelationShipType == 2)
		{
			
			//if ( lpGuildInfo->iGuildRival || lpTargetGuildInfo->iGuildRival )
			//{
			//	ErrMsg.btResult = GUILD_ANS_EXIST_RIVAL;
			//	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&ErrMsg, ErrMsg.h.size);
			//	return;
			//}

			if (btRelationShip == 1)
			{
				ErrMsg.btResult = GUILD_ANS_EXIST_RELATIONSHIP_UNION;
				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&ErrMsg, ErrMsg.h.size);
				return;
			}
		}
	}
	else if (aRecv->btRequestType == 2)
	{
		if (aRecv->btRelationShipType == 1)
		{
			if (lpGuildInfo->iGuildUnion == 0)
			{
				ErrMsg.btResult = GUILD_ANS_NOTEXIST_UNION;
				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&ErrMsg, ErrMsg.h.size);
				return;
			}
		}
		else if (aRecv->btRelationShipType == 2)
		{
			if (!lpGuildInfo->iGuildRival || !lpTargetGuildInfo->iGuildRival)
			{
				ErrMsg.btResult = GUILD_ANS_NOTEXIST_UNION;
				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&ErrMsg, ErrMsg.h.size);
				return;
			}

			if (lpGuildInfo->iGuildRival != lpTargetGuildInfo->Number || lpGuildInfo->Number != lpTargetGuildInfo->iGuildRival)
			{
				ErrMsg.btResult = GUILD_ANS_NOT_GUILD_RIVAL;
				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&ErrMsg, ErrMsg.h.size);
				return;
			}
		}
	}

	if (aRecv->btRequestType == 2)
	{
		if (aRecv->btRelationShipType == 1)
		{
			GDUnionBreakOff(lpObj, lpGuildInfo->iGuildUnion);
			return;
		}
	}

	lpObj->m_IfState.use = 1;
	lpObj->m_IfState.type = 14;
	lpObj->m_IfState.state = 0;
	lpObj->m_InterfaceTime = GetTickCount();

	lpTargetObj->m_IfState.use = 1;
	lpTargetObj->m_IfState.type = 14;
	lpTargetObj->m_IfState.state = 0;
	lpTargetObj->m_InterfaceTime = GetTickCount();

	PMSG_RELATIONSHIP_JOIN_BREAKOFF_REQ pMsg = { 0 };

	PHeadSetB((LPBYTE)&pMsg, 0xE5, sizeof(pMsg));
	pMsg.btTargetUserIndexL = SET_NUMBERL(lpObj);
	pMsg.btTargetUserIndexH = SET_NUMBERH(lpObj);
	pMsg.btRequestType = aRecv->btRequestType;
	pMsg.btRelationShipType = aRecv->btRelationShipType;

	GIOCP.DataSend(iTargetUserIndex, (LPBYTE)&pMsg, pMsg.h.size);
}
*/

/*
void GameProtocol::CGRelationShipAnsJoinBreakOff(PMSG_RELATIONSHIP_JOIN_BREAKOFF_ANS * aRecv, CGameObject* lpObj)
{
	int iTargetUserIndex = MAKE_NUMBERW(aRecv->btTargetUserIndexH, aRecv->btTargetUserIndexL);

	if (!ObjectMaxRange(lpObj) || !ObjectMaxRange(iTargetUserIndex))
		return;

	if (!PacketCheckTime(lpObj))
		return;

	if (gObjIsConnected(&gObj[iTargetUserIndex]) == FALSE)
	{
		GCResultSend(lpObj, 0x51, 3);
		return;
	}

	if (aRecv->btResult == 1)
	{
		if (aRecv->btRequestType == 1)
		{
			GDRelationShipReqJoin(iTargetUserIndex, lpObj, aRecv->btRelationShipType);
		}
		else if (aRecv->btRequestType == 2)
		{
			GDRelationShipReqBreakOff(lpObj, iTargetUserIndex, aRecv->btRelationShipType);
		}
	}
	else
	{
		PMSG_RELATIONSHIP_JOIN_BREAKOFF_ANS ErrMsg = { 0 };

		PHeadSetB((LPBYTE)&ErrMsg, 0xE6, sizeof(ErrMsg));
		ErrMsg.btResult = 0;
		ErrMsg.btRequestType = aRecv->btRequestType;
		ErrMsg.btRelationShipType = aRecv->btRelationShipType;
		ErrMsg.btTargetUserIndexH = aRecv->btTargetUserIndexH;
		ErrMsg.btTargetUserIndexL = aRecv->btTargetUserIndexL;
		ErrMsg.btResult = GUILD_ANS_CANCLE_REQUEST;

		GIOCP.DataSend(iTargetUserIndex, (LPBYTE)&ErrMsg, ErrMsg.h.size);
	}

	if (lpObj->m_IfState.use)
	{
		if (lpObj->m_IfState.type == 14)
		{
			lpObj->m_IfState.use = 0;
		}
	}

	if (gObj[iTargetUserIndex].m_IfState.use)
	{
		if (gObj[iTargetUserIndex].m_IfState.type == 14)
		{
			gObj[iTargetUserIndex].m_IfState.use = 0;
		}
	}
}
*/

/*
void GameProtocol::CGUnionList(PMSG_UNIONLIST_REQ * aRecv, CGameObject* lpObj)
{
	if (!ObjectMaxRange(lpObj))
		return;

	if (!PacketCheckTime(lpObj))
		return;

	LPOBJ lpObj = *lpObj;

	if (lpObj == NULL)
	{
		return;
	}

	if (gObjIsConnected(*lpObj) == FALSE)
	{
		GCResultSend(lpObj, 0x51, 3);
		return;
	}

	if (lpObj->m_PlayerData->lpGuild == NULL)
	{
		GCResultSend(lpObj, 0x51, 3);
		return;
	}

	if (lpObj->m_PlayerData->lpGuild->iGuildUnion == 0)
	{
		GCResultSend(lpObj, 0x51, 3);
		return;
	}

	GDUnionListSend(lpObj, lpObj->m_PlayerData->lpGuild->iGuildUnion);

}


void GameProtocol::CGRelationShipReqKickOutUnionMember(PMSG_KICKOUT_UNIONMEMBER_REQ* aRecv, CGameObject* lpObj)
{
	if (!ObjectMaxRange(lpObj))
		return;

	if (this->PacketCheckTime(lpObj) == FALSE)
		return;

	LPOBJ lpObj = *lpObj;

	if (lpObj == NULL)
		return;

	if (gObjIsConnected(*lpObj) == FALSE)
	{
		GCResultSend(lpObj, 0x51, 3);
		MsgOutput(lpObj, Lang.GetText(0, 529));
		return;
	}

	if (lpObj->m_PlayerData->lpGuild == NULL)
	{
		GCResultSend(lpObj, 0x51, 3);
		MsgOutput(lpObj, Lang.GetText(0, 530));
		return;
	}

	PMSG_RELATIONSHIP_JOIN_BREAKOFF_ANS ErrMsg = { 0 };

	PHeadSetB((LPBYTE)&ErrMsg, 0xE6, sizeof(ErrMsg));
	ErrMsg.btResult = 0;
	ErrMsg.btRequestType = 2;
	ErrMsg.btRelationShipType = 1;
	ErrMsg.btTargetUserIndexH = 0;
	ErrMsg.btTargetUserIndexL = 0;

	if (lpObj->m_PlayerData->lpGuild->iGuildUnion == 0)
	{
		ErrMsg.btResult = GUILD_ANS_NOTEXIST_UNION;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&ErrMsg, ErrMsg.h.size);

		return;
	}

	int iArcaBattleState = g_ArcaBattle.GetState();

	if (iArcaBattleState > 2 && iArcaBattleState < 9)
	{
		ErrMsg.btResult = GUILD_ANS_ARCA_BATTLE_ONGOING;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&ErrMsg, sizeof(ErrMsg));
		return;
	}

	if (lpObj->m_PlayerData->GuildStatus != GUILD_MASTER || lpObj->m_PlayerData->lpGuild->Number != lpObj->m_PlayerData->lpGuild->iGuildUnion)
	{
		ErrMsg.btResult = GUILD_ANS_NOTEXIST_PERMISSION;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&ErrMsg, ErrMsg.h.size);

		return;
	}

	char szUnionMemberGuildName[MAX_GUILD_LEN + 1] = { 0 };
	memcpy(szUnionMemberGuildName, aRecv->szTargetGuildName, MAX_GUILD_LEN);

	if (!strcmp(lpObj->m_PlayerData->lpGuild->Name, szUnionMemberGuildName))
	{
		ErrMsg.btResult = GUILD_ANS_NOTEXIST_PERMISSION;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&ErrMsg, ErrMsg.h.size);

		return;
	}

	GDRelationShipReqKickOutUnionMember(lpObj, szUnionMemberGuildName);
}
*/


void GameProtocol::CGReqMapSvrAuth(PMSG_REQ_MAPSERVERAUTH * lpMsg, CGameObject* lpObj)
{
	if (lpMsg->btCliVersion[0] != szClientVersion[0] ||
		lpMsg->btCliVersion[1] != szClientVersion[1] ||
		lpMsg->btCliVersion[2] != szClientVersion[2] ||
		lpMsg->btCliVersion[3] != szClientVersion[3] ||
		lpMsg->btCliVersion[4] != szClientVersion[4])
	{
		//GCAnsMapSvrAuth(lpObj, 6);
		//GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);

		//return;
	}

	char id[MAX_ACCOUNT_LEN + 1];
	id[MAX_ACCOUNT_LEN] = 0;
	memcpy(id, lpMsg->szAccountID, MAX_ACCOUNT_LEN);
	BuxConvert(id, MAX_ACCOUNT_LEN);

	/*
		BYTE btSerial[17];

		btSerial[16]=0;
		memcpy(btSerial, lpMsg->btCliSerial, sizeof(lpMsg->btCliSerial));

		if ( strcmp((char*)btSerial, szGameServerExeSerial) )
		{
			sLog->outError( "error-L1: Serial error [%s] [%s]", id, btSerial);
			GCAnsMapSvrAuth(lpObj, 6);
			GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);

			return;
		}*/

	if (g_ConfigRead.server.IsConnectMemberUse() == TRUE)
	{
		if (ConMember.IsMember(id) == FALSE)
		{
			GCAnsMapSvrAuth(lpObj, 2);

			return;
		}
	}


	if (PacketCheckTime(lpObj) == FALSE)
	{
		sLog->outBasic("error-L2: Login time is over [%d][%s]", lpObj->m_PlayerData->ConnectUser->Index, id);
		GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);

		return;
	}

	if (lpObj->m_PlayerData->ConnectUser->Connected != PLAYER_CONNECTED)
	{
		sLog->outBasic("error-L2: Authentication is allowed after connected [%d][%s]", lpObj->m_PlayerData->ConnectUser->Index, id);
		GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);

		return;
	}

	if (lpObj->m_PlayerData->ConnectUser->LoginMsgSnd != 0)
	{
		if (lpObj->m_PlayerData->ConnectUser->LoginMsgSnd == 1)
		{
			sLog->outBasic("error: It is after authentication information is sended [%d][%s][%s]", lpObj->m_PlayerData->ConnectUser->Index, id, lpObj->m_PlayerData->ConnectUser->IP);
		}
		else
		{
			sLog->outBasic("error: %d %s %d", lpObj->m_PlayerData->ConnectUser->LoginMsgSnd, __FILE__, __LINE__);
		}

		return;
	}

	lpObj->m_PlayerData->CheckTick = lpMsg->iTickCount;
	lpObj->m_PlayerData->CheckTick2 = WorldTimer::getMSTime();
	lpObj->m_PlayerData->ConnectUser->ConnectCheckTime = WorldTimer::getMSTime();
	lpObj->m_PlayerData->CheckSpeedHack = true;
	lpObj->m_PlayerData->ConnectUser->ConnectCheckTime = WorldTimer::getMSTime();
	lpObj->m_PlayerData->ConnectUser->LoginMsgSnd = 1;
	lpObj->m_PlayerData->ConnectUser->LoginMsgCount++;
	lpObj->m_PlayerData->ConnectUser->AccountID[MAX_ACCOUNT_LEN] = 0;
	memcpy(lpObj->m_PlayerData->ConnectUser->AccountID, id, MAX_ACCOUNT_LEN);
	memset(lpObj->Name, 0, MAX_ACCOUNT_LEN);
	memcpy(lpObj->m_PlayerData->m_MapServerAuthInfo->szCharName, lpMsg->szCharName, sizeof(lpMsg->szCharName));
	lpObj->m_PlayerData->m_MapServerAuthInfo->iJA1 = lpMsg->iJoinAuthCode1;
	lpObj->m_PlayerData->m_MapServerAuthInfo->iJA2 = lpMsg->iJoinAuthCode2;
	lpObj->m_PlayerData->m_MapServerAuthInfo->iJA3 = lpMsg->iJoinAuthCode3;
	lpObj->m_PlayerData->m_MapServerAuthInfo->iJA4 = lpMsg->iJoinAuthCode4;
	lpObj->m_PlayerData->ConnectUser->m_cAccountItemBlock = 0;
	lpObj->m_PlayerData->VipType = 0;
	lpObj->m_PlayerData->VipEffect = VIP_EFFECT_NONE;
	lpObj->m_PlayerData->AntiHackCheckTime = WorldTimer::getMSTime();
	lpObj->m_PlayerData->dwLastHitHackTick = WorldTimer::getMSTime();
	gObjAddMsgSendDelay(*lpObj, 1001, *lpObj, 5000, 0);
}

struct PMSG_ANS_MAPSERVERAUTH
{
	PBMSG_HEAD2 h;	// C1:B1:01
	BYTE iResult;	// 4
};

void GameProtocol::GCAnsMapSvrAuth(CGameObject *lpObj, int iResult)
{
	PMSG_ANS_MAPSERVERAUTH pMsgResult;

	PHeadSubSetB((LPBYTE)&pMsgResult, 0xB1, 0x01, sizeof(pMsgResult));
	pMsgResult.iResult = iResult;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsgResult, pMsgResult.h.size);
}

/*void GameProtocol::CGReqCastleSiegeState(PMSG_REQ_CASTLESIEGESTATE * lpMsg, CGameObject *lpObj)
{
	GS_GDReqOwnerGuildMaster(g_MapServerManager.GetMapSvrGroup(), lpObj);
}

void GameProtocol::GCAnsCastleSiegeState(CGameObject *lpObj, int iResult, LPSTR lpszGuildName, LPSTR lpszGuildMaster)
{
	if ((lpszGuildName == NULL) || (lpszGuildMaster == NULL))
	{
		return;
	}

	PMSG_ANS_CASTLESIEGESTATE pMsgResult;

	pMsgResult.h.set((LPBYTE)&pMsgResult.h, 0xB2, 0x00, sizeof(pMsgResult));

	pMsgResult.btResult = iResult;

	memcpy(pMsgResult.cOwnerGuild, lpszGuildName, sizeof(pMsgResult.cOwnerGuild));
	memcpy(pMsgResult.cOwnerGuildMaster, lpszGuildMaster, sizeof(pMsgResult.cOwnerGuildMaster));

	SYSTEMTIME tmStateStartDate = { 0 };
	SYSTEMTIME tmStateEndDate = { 0 };
	SYSTEMTIME tmSiegeStartDate = { 0 };

	int iCastleState = g_CastleSiege.GetCastleState();
	int bRETVAL = g_CastleSiege.GetCastleStateTerm(&tmStateStartDate, &tmStateEndDate);
	tmSiegeStartDate = g_CastleSiege.GetCastleLeftSiegeDate();

	if (bRETVAL == FALSE)
	{
		iCastleState = -1;
	}

	int iStateLeftSec = g_CastleSiege.GetCurRemainSec();

	// Create Packet
	pMsgResult.cCastleSiegeState = iCastleState;
	// Start Date
	pMsgResult.btStartYearH = SET_NUMBERH(tmStateStartDate.wYear);
	pMsgResult.btStartYearL = SET_NUMBERL(tmStateStartDate.wYear);
	pMsgResult.btStartMonth = tmStateStartDate.wMonth;
	pMsgResult.btStartDay = tmStateStartDate.wDay;
	pMsgResult.btStartHour = tmStateStartDate.wHour;
	pMsgResult.btStartMinute = tmStateStartDate.wMinute;
	// End Date
	pMsgResult.btEndYearH = SET_NUMBERH(tmStateEndDate.wYear);
	pMsgResult.btEndYearL = SET_NUMBERL(tmStateEndDate.wYear);
	pMsgResult.btEndMonth = tmStateEndDate.wMonth;
	pMsgResult.btEndDay = tmStateEndDate.wDay;
	pMsgResult.btEndHour = tmStateEndDate.wHour;
	pMsgResult.btEndMinute = tmStateEndDate.wMinute;
	// Siege Start Date
	pMsgResult.btSiegeStartYearH = SET_NUMBERH(tmSiegeStartDate.wYear);
	pMsgResult.btSiegeStartYearL = SET_NUMBERL(tmSiegeStartDate.wYear);
	pMsgResult.btSiegeStartMonth = tmSiegeStartDate.wMonth;
	pMsgResult.btSiegeStartDay = tmSiegeStartDate.wDay;
	pMsgResult.btSiegeStartHour = tmSiegeStartDate.wHour;
	pMsgResult.btSiegeStartMinute = tmSiegeStartDate.wMinute;
	// Time Left
	pMsgResult.btStateLeftSec1 = SET_NUMBERH(SET_NUMBERHW(iStateLeftSec));
	pMsgResult.btStateLeftSec2 = SET_NUMBERL(SET_NUMBERHW(iStateLeftSec));
	pMsgResult.btStateLeftSec3 = SET_NUMBERH(SET_NUMBERLW(iStateLeftSec));
	pMsgResult.btStateLeftSec4 = SET_NUMBERL(SET_NUMBERLW(iStateLeftSec));

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::CGReqRegCastleSiege(PMSG_REQ_REGCASTLESIEGE * lpMsg, CGameObject *lpObj)
{
	if (g_CastleSiege.GetCastleState() != CASTLESIEGE_STATE_REGSIEGE)
	{
		GCAnsRegCastleSiege(lpObj, 7, "");
		return;
	}
	if (strcmp(lpObj->m_PlayerData->GuildName, "") == 0)
	{
		GCAnsRegCastleSiege(lpObj, 6, "");
		return;
	}
	if (g_CastleSiege.CheckUnionGuildMaster(lpObj) == FALSE)
	{
		GCAnsRegCastleSiege(lpObj, 0, "");
	}
	else if ((g_CastleSiege.CheckCastleOwnerMember(lpObj) != FALSE) ||
		(g_CastleSiege.CheckCastleOwnerUnionMember(lpObj) != FALSE))
	{
		GCAnsRegCastleSiege(lpObj, 3, "");
	}
	else
	{
		GS_GDReqRegAttackGuild(g_MapServerManager.GetMapSvrGroup(), iIndex);
	}
}

void GameProtocol::GCAnsRegCastleSiege(CGameObject *lpObj, int iResult, LPSTR lpszGuildName)
{
	PMSG_ANS_REGCASTLESIEGE pMsgResult;

	if (lpszGuildName == NULL)
	{
		return;
	}

	if (ObjectMaxRange(lpObj) == FALSE)
	{
		return;
	}

	pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x01, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	memcpy(&pMsgResult.szGuildName, lpszGuildName, sizeof(pMsgResult.szGuildName));

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsgResult, pMsgResult.h.size);

	if (::gObjIsConnected(lpObj))
	{
		sLog->outBasic("[CastleSiege] [%s][%s] Registered Castle Siege (GUILD:%s) - Result:%d", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->GuildName, iResult);
	}
}

void GameProtocol::CGReqGiveUpCastleSiege(PMSG_REQ_GIVEUPCASTLESIEGE * lpMsg, CGameObject *lpObj)
{
	if ((g_CastleSiege.GetCastleState() < CASTLESIEGE_STATE_REGSIEGE) || (g_CastleSiege.GetCastleState() > CASTLESIEGE_STATE_REGMARK)) //Fixed
	{
		GCAnsGiveUpCastleSiege(lpObj, 3, 0, 0, "");
		return;
	}

	if (strcmp(lpObj->m_PlayerData->GuildName, "") == 0)
	{
		return;
	}

	if (g_CastleSiege.CheckUnionGuildMaster(lpObj) == FALSE)
	{
		return;
	}

	GS_GDReqGuildSetGiveUp(g_MapServerManager.GetMapSvrGroup(), iIndex, lpMsg->btGiveUp);
}

void GameProtocol::GCAnsGiveUpCastleSiege(CGameObject *lpObj, int iResult, int bGiveUp, int iMarkCount, LPSTR lpszGuildName)
{
	PMSG_ANS_GIVEUPCASTLESIEGE pMsgResult;

	if (lpszGuildName == NULL)
	{
		return;
	}

	if (ObjectMaxRange(lpObj) == FALSE)
	{
		return;
	}

	pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x02, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.btIsGiveUp = bGiveUp;

	memcpy(&pMsgResult.szGuildName, lpszGuildName, sizeof(pMsgResult.szGuildName));

	if ((pMsgResult.btResult == 1) && (iMarkCount > 0))
	{
		lpObj->m_PlayerData->Money += (iMarkCount * 3000);

		if (lpObj->m_PlayerData->Money > MAX_ZEN)
		{
			lpObj->m_PlayerData->Money = MAX_ZEN;
		}
		GCMoneySend(lpObj, lpObj->m_PlayerData->Money);
	}

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsgResult, pMsgResult.h.size);

	if (gObjIsConnected(lpObj))
	{
		sLog->outBasic("[CastleSiege] [%s][%s] GiveUp Castle Siege (GUILD:%s) - Result:%d", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->GuildName, iResult);
	}
}
*/

/*
void GameProtocol::CGReqGuildRegInfo(PMSG_REQ_GUILDREGINFO * lpMsg, CGameObject *lpObj)
{
	if (lpMsg == NULL)
	{
		return;
	}

	if (strcmp(lpObj->m_PlayerData->GuildName, "") == 0)
	{
		return;
	}

	GS_GDReqGuildMarkRegInfo(g_MapServerManager.GetMapSvrGroup(), iIndex);
}

void GameProtocol::GCAnsGuildRegInfo(CGameObject *lpObj, int iResult, CSP_ANS_GUILDREGINFO* lpMsgResult)
{
	PMSG_ANS_GUILDREGINFO pMsgResult;

	if (lpMsgResult == NULL)
	{
		return;
	}

	if (ObjectMaxRange(lpObj) == FALSE)
	{
		return;
	}

	pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 3, sizeof(pMsgResult));

	pMsgResult.btResult = iResult;
	pMsgResult.btGuildMark1 = SET_NUMBERH(SET_NUMBERHW(lpMsgResult->iRegMarkCount));
	pMsgResult.btGuildMark2 = SET_NUMBERL(SET_NUMBERHW(lpMsgResult->iRegMarkCount));
	pMsgResult.btGuildMark3 = SET_NUMBERH(SET_NUMBERLW(lpMsgResult->iRegMarkCount));
	pMsgResult.btGuildMark4 = SET_NUMBERL(SET_NUMBERLW(lpMsgResult->iRegMarkCount));
	pMsgResult.btRegRank = lpMsgResult->btRegRank;
	pMsgResult.btIsGiveUp = lpMsgResult->bIsGiveUp;
	memcpy(&pMsgResult.szGuildName, lpMsgResult->szGuildName, sizeof(pMsgResult.szGuildName));

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::CGReqRegGuildMark(PMSG_REQ_REGGUILDMARK * lpMsg, CGameObject *lpObj)
{
	if (lpMsg == NULL)
	{
		return;
	}

	if (ObjectMaxRange(lpObj) == FALSE)
	{
		return;
	}

	if (lpObj->UseEventServer == TRUE)
	{
		return;
	}

	lpObj->UseEventServer = TRUE;

	if (g_CastleSiege.GetCastleState() != CASTLESIEGE_STATE_REGMARK)
	{
		lpObj->UseEventServer = FALSE;
		return;
	}
	if (strcmp(lpObj->m_PlayerData->GuildName, "") == 0)
	{
		lpObj->UseEventServer = FALSE;
		return;
	}

	if (CHECK_LIMIT(lpMsg->btItemPos, 64) == FALSE)
	{
		PMSG_ANS_REGGUILDMARK pMsgResult = { 0 };
		pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x04, sizeof(pMsgResult));
		pMsgResult.btResult = 3;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsgResult, pMsgResult.h.size);
		lpObj->UseEventServer = FALSE;
	}
	else
	{
		int iItemPos = (lpMsg->btItemPos + 12);

		if (lpObj->pInventory[iItemPos].IsItem() == FALSE)
		{
			PMSG_ANS_REGGUILDMARK pMsgResult = { 0 };
			pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x04, sizeof(pMsgResult));
			pMsgResult.btResult = 3;
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsgResult, pMsgResult.h.size);
			lpObj->UseEventServer = FALSE;
		}
		else
		{
			if (lpObj->pInventory[iItemPos].m_Type != ITEMGET(14, 21) ||
				lpObj->pInventory[iItemPos].m_Level != 3 ||
				lpObj->pInventory[iItemPos].m_Durability <= 0.0f)
			{
				PMSG_ANS_REGGUILDMARK pMsgResult = { 0 };
				pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x04, sizeof(pMsgResult));
				pMsgResult.btResult = 3;
				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsgResult, pMsgResult.h.size);
				lpObj->UseEventServer = FALSE;
			}
			else
			{
				GS_GDReqRegGuildMark(g_MapServerManager.GetMapSvrGroup(), iIndex, iItemPos);
			}
		}
	}
}

void GameProtocol::GCAnsRegGuildMark(CGameObject *lpObj, int iResult, CSP_ANS_GUILDREGMARK* lpMsgResult)
{
	if (lpMsgResult == NULL)
	{
		return;
	}

	if (ObjectMaxRange(lpObj) == FALSE)
	{
		return;
	}

	if (lpObj->pInventory[lpMsgResult->iItemPos].IsItem() == FALSE)
	{
		PMSG_ANS_REGGUILDMARK pMsgResult = { 0 };
		pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x04, sizeof(pMsgResult));
		pMsgResult.btResult = 3;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsgResult, pMsgResult.h.size);
		lpObj->UseEventServer = 0;
	}
	else
	{
		if (lpObj->pInventory[lpMsgResult->iItemPos].m_Type != ITEMGET(14, 21) &&
			lpObj->pInventory[lpMsgResult->iItemPos].m_Level == 3 ||
			lpObj->pInventory[lpMsgResult->iItemPos].m_Durability <= 0.0f)
		{
			PMSG_ANS_REGGUILDMARK pMsgResult = { 0 };
			pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x04, sizeof(pMsgResult));
			pMsgResult.btResult = 3;
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsgResult, pMsgResult.h.size);
			lpObj->UseEventServer = FALSE;
		}
		else
		{
			if ((lpObj->pInventory[lpMsgResult->iItemPos].m_Durability -= 1.0) > 0.0f)
			{
				GCItemDurSend(lpObj, lpMsgResult->iItemPos, lpObj->pInventory[lpMsgResult->iItemPos].m_Durability, 0);
			}
			else
			{
				gObjInventoryDeleteItem(lpObj, lpMsgResult->iItemPos);
				GCInventoryItemDeleteSend(lpObj, lpMsgResult->iItemPos, 1);
			}

			PMSG_ANS_REGGUILDMARK pMsgResult;
			pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x04, sizeof(pMsgResult));

			pMsgResult.btResult = iResult;
			pMsgResult.btGuildMark1 = SET_NUMBERH(SET_NUMBERHW(lpMsgResult->iRegMarkCount));
			pMsgResult.btGuildMark2 = SET_NUMBERL(SET_NUMBERHW(lpMsgResult->iRegMarkCount));
			pMsgResult.btGuildMark3 = SET_NUMBERH(SET_NUMBERLW(lpMsgResult->iRegMarkCount));
			pMsgResult.btGuildMark4 = SET_NUMBERL(SET_NUMBERLW(lpMsgResult->iRegMarkCount));
			memcpy(&pMsgResult.szGuildName, lpMsgResult->szGuildName, sizeof(pMsgResult.szGuildName));
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsgResult, pMsgResult.h.size);
			lpObj->UseEventServer = 0;

		}
	}
}
*/

void GameProtocol::CGReqNpcBuy(PMSG_REQ_NPCBUY * lpMsg, CGameObject *lpObj)
{
	if (lpMsg == NULL)
	{
		return;
	}

	if (strcmp(lpObj->m_PlayerData->GuildName, "") == 0)
	{
		return;
	}

	/*if (g_CastleSiege.CheckGuildOwnCastle(lpObj->m_PlayerData->GuildName) == FALSE || (lpObj->m_PlayerData->GuildStatus != G_MASTER && lpObj->m_PlayerData->GuildStatus != G_SUB_MASTER))
	{
		sLog->outError("[CastleSiege] CGReqNpcBuy() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->GuildName, lpObj->m_PlayerData->GuildStatus, lpMsg->iNpcNumber, lpMsg->iNpcIndex);
	}
	else
	{
		BYTE btResult = 0;

		BOOL bRETVAL = g_CastleSiege.CheckAddDbNPC(*lpObj, lpMsg->iNpcNumber, lpMsg->iNpcIndex, btResult);

		if (bRETVAL == FALSE)
		{
			GCAnsNpcBuy(lpObj, btResult, lpMsg->iNpcNumber, lpMsg->iNpcIndex);
			sLog->outError("[CastleSiege] CGReqNpcBuy() ERROR - CCastleSiege::CheckAddDbNPC() [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->GuildName, lpObj->m_PlayerData->GuildStatus, lpMsg->iNpcNumber, lpMsg->iNpcIndex);
		}
		else
		{
			sLog->outError("[CastleSiege] CGReqNpcBuy() OK - [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->GuildName, lpObj->m_PlayerData->GuildStatus, lpMsg->iNpcNumber, lpMsg->iNpcIndex);
		}
	}*/
}

void GameProtocol::GCAnsNpcBuy(CGameObject *lpObj, int iResult, int iNpcNumber, int iNpcIndex)
{
	PMSG_ANS_NPCBUY pMsgResult;

	pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x05, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.iNpcNumber = iNpcNumber;
	pMsgResult.iNpcIndex = iNpcIndex;
	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::CGReqNpcRepair(PMSG_REQ_NPCREPAIR * lpMsg, CGameObject *lpObj)
{
	/*
	if (lpMsg == NULL)
	{
		return;
	}

	if (strcmp(lpObj->m_PlayerData->GuildName, "") == 0)
	{
		return;
	}

	if ((g_CastleSiege.CheckGuildOwnCastle(lpObj->m_PlayerData->GuildName) == FALSE) || (lpObj->m_PlayerData->GuildStatus != G_MASTER) && (lpObj->m_PlayerData->GuildStatus != G_SUB_MASTER))
	{
		sLog->outError("[CastleSiege] CGReqNpcRepair() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->GuildName, lpObj->m_PlayerData->GuildStatus);
		return;
	}

	_CS_NPC_DATA pNpcData;
	BOOL bRETVAL;
	int iNpcIndex;
	int iRepairCost;

	bRETVAL = g_CastleSiege.GetNpcData(lpMsg->iNpcNumber, lpMsg->iNpcIndex, pNpcData);

	if (bRETVAL == FALSE)
	{
		GCAnsNpcRepair(lpObj, 0, lpMsg->iNpcNumber, lpMsg->iNpcIndex, 0, 0);
		sLog->outError("[CastleSiege] CGReqNpcRepair() ERROR - CL Request Fail [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->GuildName, lpObj->m_PlayerData->GuildStatus, lpMsg->iNpcNumber, lpMsg->iNpcIndex);
		return;
	}
	else
	{
		iNpcIndex = pNpcData.m_iNPC_OBJINDEX;
		CGameObject* lpObjNPC = getGameObject(iNpcIndex);
		if (gObjIsConnected(*lpObjNPC) == FALSE)
		{
			GCAnsNpcRepair(lpObj, 0, lpMsg->iNpcNumber, lpMsg->iNpcIndex, 0, 0);
			sLog->outError("[CastleSiege] CGReqNpcRepair() ERROR - Npc Alive Data Mismatch [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->GuildName, lpObj->m_PlayerData->GuildStatus, pNpcData.m_iNPC_NUM, pNpcData.m_iNPC_INDEX);
			return;
		}
		else if (lpObjNPC->Live == 0)
		{
			GCAnsNpcRepair(lpObj, 0, lpMsg->iNpcNumber, lpMsg->iNpcIndex, 0, 0);
			sLog->outError("[CastleSiege] CGReqNpcRepair() ERROR - Npc Alive Data Mismatch [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->GuildName, lpObj->m_PlayerData->GuildStatus, pNpcData.m_iNPC_NUM, pNpcData.m_iNPC_INDEX);
			return;
		}
		else
		{
			if ((lpObjNPC->MaxLife == lpObjNPC->Life) || (lpObjNPC->MaxLife < lpObjNPC->Life))
			{
				lpObjNPC->Life = lpObjNPC->MaxLife;
				GCAnsNpcRepair(lpObj, 0, lpMsg->iNpcNumber, lpMsg->iNpcIndex, 0, 0);
				return;
			}
			else
			{
				iRepairCost = 0;

				switch (lpMsg->iNpcNumber)
				{
				case 277:
					iRepairCost = float((lpObjNPC->MaxLife - lpObjNPC->Life) * 0.5f) + float(lpObjNPC->m_btCsNpcDfLevel * 1000000);
					break;
				case 283:
					iRepairCost = (lpObjNPC->MaxLife - lpObjNPC->Life)* 0.3f + float((lpObjNPC->m_btCsNpcDfLevel + lpObjNPC->m_btCsNpcRgLevel) * 1000000);
					break;
				default:
					GCAnsNpcRepair(lpObj, 0, lpMsg->iNpcNumber, lpMsg->iNpcIndex, 0, 0);
					return;
				}

				if (lpObj->m_PlayerData->Money < iRepairCost)
				{
					GCAnsNpcRepair(lpObj, 3, lpMsg->iNpcNumber, lpMsg->iNpcIndex, 0, 0);
					sLog->outError("[CastleSiege] CGReqNpcRepair() ERROR - Money isn't enough [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->GuildName, lpObj->m_PlayerData->GuildStatus, pNpcData.m_iNPC_NUM, pNpcData.m_iNPC_INDEX);
					return;
				}
				else
				{
					sLog->outError("[CastleSiege] CGReqNpcRepair() OK - [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->GuildName, lpObj->m_PlayerData->GuildStatus, pNpcData.m_iNPC_NUM, pNpcData.m_iNPC_INDEX);
					GS_GDReqCastleNpcRepair(g_MapServerManager.GetMapSvrGroup(), lpObj, lpMsg->iNpcNumber, lpMsg->iNpcIndex, iRepairCost);
					//return;
				}
			}
		}
	}*/
}

void GameProtocol::GCAnsNpcRepair(CGameObject *lpObj, int iResult, int iNpcNumber, int iNpcIndex, int iNpcHP, int iNpcMaxHP)
{
	PMSG_ANS_NPCREPAIR pMsgResult;

	pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x06, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.iNpcNumber = iNpcNumber;
	pMsgResult.iNpcIndex = iNpcIndex;
	pMsgResult.iNpcHP = iNpcHP;
	pMsgResult.iNpcMaxHP = iNpcMaxHP;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsgResult, pMsgResult.h.size);
}



void GameProtocol::CGReqNpcUpgrade(PMSG_REQ_NPCUPGRADE * lpMsg, CGameObject *lpObj)
{
	/*
	if (lpMsg == NULL)
	{
		return;
	}

	int iNEED_GEMOFDEFEND = 0;
	int	iNEED_MONEY = 0;
	int	iCUR_UPLVL = 0;
	int	iNXT_UPLVL = 0;
	int	iNXT_UPVALUE = 0;
	int	bENABLE_UPGRADE = FALSE;

	if (strcmp(lpObj->m_PlayerData->GuildName, "") == 0)
		return;

	if ((g_CastleSiege.CheckGuildOwnCastle(lpObj->m_PlayerData->GuildName) == FALSE) || (lpObj->m_PlayerData->GuildStatus != G_MASTER && lpObj->m_PlayerData->GuildStatus != G_SUB_MASTER))
	{
		sLog->outBasic("[CastleSiege] CGReqNpcUpgrade() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->GuildName, lpObj->m_PlayerData->GuildStatus);
		return;
	}

	switch (lpMsg->iNpcNumber)
	{
	case 277://Castle Gate
	{
		_CS_NPC_DATA m_NpcData;
		BOOL bResult = g_CastleSiege.GetNpcData(lpMsg->iNpcNumber, lpMsg->iNpcIndex, m_NpcData);

		if (bResult)
		{
			switch (lpMsg->iNpcUpType)
			{
			case 1:
			{
				iCUR_UPLVL = m_NpcData.m_iNPC_DF_LEVEL;
				iNXT_UPLVL = lpMsg->iNpcUpValue;

				if ((iNXT_UPLVL >= 1) && (iNXT_UPLVL <= 3) && (iNXT_UPLVL == (iCUR_UPLVL + 1)))
				{
					bENABLE_UPGRADE = TRUE;
					iNXT_UPVALUE = iNXT_UPLVL;
					iNEED_GEMOFDEFEND = g_iNpcUpDfLevel_CGATE[(iNXT_UPLVL - 1)][1];
					iNEED_MONEY = g_iNpcUpDfLevel_CGATE[(iNXT_UPLVL - 1)][2];
				}
			}
			break;

			case 3:
			{
				iCUR_UPLVL = 0;

				for (int i = 0; i < 3; i++)
				{
					if (m_NpcData.m_iNPC_MAXHP >= *g_iNpcUpMaxHP_CGATE[i])
					{
						iCUR_UPLVL = (i + 1);
					}
				}
				iNXT_UPLVL = 0;

				for (int i = 0; i < 3; i++)
				{
					if (lpMsg->iNpcUpValue >= *g_iNpcUpMaxHP_CGATE[i])
					{
						iNXT_UPLVL = (i + 1);
					}
				}

				if ((iNXT_UPLVL >= 1) && (iNXT_UPLVL <= 3) && (iNXT_UPLVL == (iCUR_UPLVL + 1)))
				{
					bENABLE_UPGRADE = TRUE;
					iNXT_UPVALUE = g_iNpcUpMaxHP_CGATE[(iNXT_UPLVL - 1)][0];
					iNEED_GEMOFDEFEND = g_iNpcUpMaxHP_CGATE[(iNXT_UPLVL - 1)][1];
					iNEED_MONEY = g_iNpcUpMaxHP_CGATE[(iNXT_UPLVL - 1)][2];
				}
			}
			break;
			default:
			{
				GCAnsNpcUpgrade(lpObj, 5, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
				return;
			}
			break;
			}
		}
		else
		{
			GCAnsNpcUpgrade(lpObj, 7, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
			return;
		}
	}
	break;
	case 283://Statue
	{
		_CS_NPC_DATA m_NpcData;

		BOOL bResult;

		bResult = g_CastleSiege.GetNpcData(lpMsg->iNpcNumber, lpMsg->iNpcIndex, m_NpcData);

		if (bResult)
		{
			switch (lpMsg->iNpcUpType)
			{
			case 1:
			{
				iCUR_UPLVL = m_NpcData.m_iNPC_DF_LEVEL;
				iNXT_UPLVL = lpMsg->iNpcUpValue;

				if ((iNXT_UPLVL >= 1) && (iNXT_UPLVL <= 3) && (iNXT_UPLVL == (iCUR_UPLVL + 1))) //Fixed
				{
					bENABLE_UPGRADE = TRUE;
					iNXT_UPVALUE = iNXT_UPLVL;
					iNEED_GEMOFDEFEND = g_iNpcUpDfLevel_CSTATUE[(iNXT_UPLVL - 1)][1];
					iNEED_MONEY = g_iNpcUpDfLevel_CSTATUE[(iNXT_UPLVL - 1)][2];
				}
			}
			break;
			case 2:
			{
				iCUR_UPLVL = m_NpcData.m_iNPC_RG_LEVEL;
				iNXT_UPLVL = lpMsg->iNpcUpValue;

				if ((iNXT_UPLVL >= 1) && (iNXT_UPLVL <= 3) && (iNXT_UPLVL == (iCUR_UPLVL + 1)))
				{
					bENABLE_UPGRADE = TRUE;
					iNXT_UPVALUE = iNXT_UPLVL;
					iNEED_GEMOFDEFEND = g_iNpcUpRgLevel_CSTATUE[(iNXT_UPLVL - 1)][1];
					iNEED_MONEY = g_iNpcUpRgLevel_CSTATUE[(iNXT_UPLVL - 1)][2];
				}
			}
			break;
			case 3:
			{
				iCUR_UPLVL = 0;

				for (int i = 0; i < 3; i++)
				{
					if (m_NpcData.m_iNPC_MAXHP >= *g_iNpcUpMaxHP_CSTATUE[i])
					{
						iCUR_UPLVL = (i + 1);
					}
				}

				iNXT_UPLVL = 0;

				for (int i = 0; i < 3; i++)
				{
					if (lpMsg->iNpcUpValue >= *g_iNpcUpMaxHP_CSTATUE[i])
					{
						iNXT_UPLVL = (i + 1);
					}
				}

				if ((iNXT_UPLVL >= 1) && (iNXT_UPLVL <= 3) && (iNXT_UPLVL == (iCUR_UPLVL + 1)))
				{
					bENABLE_UPGRADE = TRUE;
					iNXT_UPVALUE = g_iNpcUpMaxHP_CSTATUE[(iNXT_UPLVL - 1)][0];
					iNEED_GEMOFDEFEND = g_iNpcUpMaxHP_CSTATUE[(iNXT_UPLVL - 1)][1];
					iNEED_MONEY = g_iNpcUpMaxHP_CSTATUE[(iNXT_UPLVL - 1)][2];
				}
			}
			break;
			default:
			{
				GCAnsNpcUpgrade(lpObj, 5, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
				sLog->outBasic("[CastleSiege] CGReqNpcUpgrade() ERROR - UpType doesn't Exist [%s][%s], Guild:(%s)(%d), NPC CLS:%d, IDX:%d, UPTYPE:%d, UPVAL:%d",
					lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->GuildName, lpObj->m_PlayerData->GuildNumber, lpMsg->iNpcNumber,
					lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
				return;
			}
			break;
			}
		}
		else
		{
			GCAnsNpcUpgrade(lpObj, 7, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
			return;
		}
	}
	break;
	}

	int iGEMOFDEFEND_COUNT;
	int x;
	int bRETVAL;

	if (bENABLE_UPGRADE == FALSE)
	{
		GCAnsNpcUpgrade(lpObj, 6, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
	}
	else
	{
		if (lpObj->m_PlayerData->Money < iNEED_MONEY)
		{
			GCAnsNpcUpgrade(lpObj, 3, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
			sLog->outBasic("[CastleSiege] CGReqNpcUpgrade() ERROR - Money is Low [%s][%s], Guild:(%s)(%d), NPC CLS:%d, IDX:%d, UPTYPE:%d, UPVAL:%d, NXTLV:%d, Money:%d, Need Money:%d",
				lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->GuildName, lpObj->m_PlayerData->GuildNumber, lpMsg->iNpcNumber, lpMsg->iNpcIndex,
				lpMsg->iNpcUpType, lpMsg->iNpcUpValue, iNXT_UPLVL, lpObj->m_PlayerData->Money, iNEED_MONEY);
		}
		else
		{
			for (iGEMOFDEFEND_COUNT = 0, x = 0; x < INVENTORY_SIZE; x++)
			{
				if (lpObj->pntInventory[x]->IsItem() == TRUE)
				{
					if (lpObj->pntInventory[x]->m_Type == ITEMGET(14, 31))
					{
						iGEMOFDEFEND_COUNT += 1;
					}
				}
			}

			if (iGEMOFDEFEND_COUNT < iNEED_GEMOFDEFEND)
			{
				GCAnsNpcUpgrade(lpObj, 4, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
				sLog->outBasic("[CastleSiege] CGReqNpcUpgrade() ERROR - Gem is Low [%s][%s], Guild:(%s)(%d), NPC CLS:%d, IDX:%d, UPTYPE:%d, UPVAL:%d, NXTLV:%d, Gem:%d, Need Gem:%d", 
					lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->GuildName, lpObj->m_PlayerData->GuildNumber, lpMsg->iNpcNumber, lpMsg->iNpcIndex,
					lpMsg->iNpcUpType, lpMsg->iNpcUpValue, iNXT_UPLVL, iGEMOFDEFEND_COUNT, iNEED_GEMOFDEFEND);
			}
			else
			{
				bRETVAL = g_CastleSiege.PayForUpgradeDbNpc(*lpObj, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue, (iNXT_UPLVL - 1));

				if (bRETVAL == FALSE)
				{
				}
				else
				{
					lpObj->m_bIsCastleNPCUpgradeCompleted = TRUE;
					sLog->outBasic("[CastleSiege] [0xB2][0x07] CGReqNpcUpgrade() - Pay For Npc Upgrade (CLS:%d, IDX:%d, UPTYPE:%d, UPVAL:%d)", lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, iNXT_UPVALUE);
					GS_GDReqCastleNpcUpgrade(g_MapServerManager.GetMapSvrGroup(), lpObj, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, iNXT_UPVALUE, (iNXT_UPLVL - 1));
					sLog->outBasic("[CastleSiege] CGReqNpcUpgrade() REQ OK - [%s][%s], Guild:(%s)(%d), NPC CLS:%d, IDX:%d, UPTYPE:%d, UPVAL:%d, NXTLV:%d", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->GuildName, lpObj->m_PlayerData->GuildNumber, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue, iNXT_UPLVL);
				}
			}
		}

	}
	*/
}

void GameProtocol::GCAnsNpcUpgrade(CGameObject *lpObj, int iResult, int iNpcNumber, int iNpcIndex, int iNpcUpType, int iNpcUpValue)
{
	PMSG_ANS_NPCUPGRADE pMsgResult;

	pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x07, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.iNpcNumber = iNpcNumber;
	pMsgResult.iNpcIndex = iNpcIndex;
	pMsgResult.iNpcUpType = iNpcUpType;
	pMsgResult.iNpcUpValue = iNpcUpValue;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::CGReqTaxMoneyInfo(PMSG_REQ_TAXMONEYINFO * lpMsg, CGameObject *lpObj)
{
	if (lpMsg == NULL)
	{
		return;
	}

	if (strcmp(lpObj->m_PlayerData->GuildName, "") == 0)
	{
		return;
	}

	/*if ((g_CastleSiege.CheckGuildOwnCastle(lpObj->m_PlayerData->GuildName) == FALSE) || (lpObj->m_PlayerData->GuildStatus != G_MASTER))
	{
		sLog->outError("[CastleSiege] CGReqTaxMoneyInfo() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->GuildName, lpObj->m_PlayerData->GuildStatus);
	}
	else
	{
		GS_GDReqTaxInfo(g_MapServerManager.GetMapSvrGroup(), lpObj);
		sLog->outBasic("[CastleSiege] CGReqTaxMoneyInfo() REQ OK - [%s][%s], Guild:(%s)(%d)", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->GuildName, lpObj->m_PlayerData->GuildStatus);
	}*/
}

void GameProtocol::GCAnsTaxMoneyInfo(CGameObject *lpObj, int iResult, BYTE btTaxRateChaos, BYTE btTaxRateStore, __int64 i64Money)
{
	PMSG_ANS_TAXMONEYINFO pMsgResult;

	pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x08, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.btTaxRateChaos = btTaxRateChaos;
	pMsgResult.btTaxRateStore = btTaxRateStore;
	pMsgResult.btMoney1 = SET_NUMBERH(SET_NUMBERHW(HIDWORD(i64Money)));
	pMsgResult.btMoney2 = SET_NUMBERL(SET_NUMBERHW(HIDWORD(i64Money)));
	pMsgResult.btMoney3 = SET_NUMBERH(SET_NUMBERLW(HIDWORD(i64Money)));
	pMsgResult.btMoney4 = SET_NUMBERL(SET_NUMBERLW(HIDWORD(i64Money)));
	pMsgResult.btMoney5 = SET_NUMBERH(SET_NUMBERHW(LODWORD(i64Money)));
	pMsgResult.btMoney6 = SET_NUMBERL(SET_NUMBERHW(LODWORD(i64Money)));
	pMsgResult.btMoney7 = SET_NUMBERH(SET_NUMBERLW(LODWORD(i64Money)));
	pMsgResult.btMoney8 = SET_NUMBERL(SET_NUMBERLW(LODWORD(i64Money)));

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::CGReqTaxRateChange(PMSG_REQ_TAXRATECHANGE * lpMsg, CGameObject *lpObj)
{
	int iMaxTaxRate;
	int iTaxRate;

	if (lpMsg == NULL)
	{
		return;
	}

	if (strcmp(lpObj->m_PlayerData->GuildName, "") == 0)
	{
		return;
	}

	/*if ((g_CastleSiege.CheckGuildOwnCastle(lpObj->m_PlayerData->GuildName) == FALSE) || (lpObj->m_PlayerData->GuildStatus != G_MASTER))
	{
		sLog->outError("[CastleSiege] CGReqTaxRateChange() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->GuildName, lpObj->m_PlayerData->GuildStatus);
	}
	else
	{
		iMaxTaxRate = 0;

		switch (lpMsg->btTaxType)
		{
		case 1:
			iMaxTaxRate = 3;
			break;
		case 2:
			iMaxTaxRate = 3;
			break;
		case 3:
			iMaxTaxRate = 300000;
			break;
		}

		iTaxRate = MAKE_NUMBERDW(MAKE_NUMBERW(lpMsg->btTaxRate1, lpMsg->btTaxRate2), MAKE_NUMBERW(lpMsg->btTaxRate3, lpMsg->btTaxRate4));

		if ((iTaxRate < 0) || (iTaxRate > iMaxTaxRate))
		{
			GCAnsTaxRateChange(lpObj, 0, 0, 0);
			sLog->outBasic("[CastleSiege] CGReqTaxRateChange() ERROR - Tax Rate Out of Range [%s][%s], Guild:(%s)(%d), TaxType:%d, TaxRate:%d", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->GuildName, lpObj->m_PlayerData->GuildStatus, lpMsg->btTaxType, iTaxRate);
		}
		else
		{
			GS_GDReqTaxRateChange(g_MapServerManager.GetMapSvrGroup(), lpObj, lpMsg->btTaxType, iTaxRate);
			sLog->outBasic("[CastleSiege] CGReqTaxRateChange() REQ OK - [%s][%s], Guild:(%s)(%d), TaxType:%d, TaxRate:%d", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->GuildName, lpObj->m_PlayerData->GuildStatus, lpMsg->btTaxType, iTaxRate);
		}
	}*/
}

void GameProtocol::GCAnsTaxRateChange(CGameObject *lpObj, int iResult, BYTE btTaxType, int iTaxRate)
{
	PMSG_ANS_TAXRATECHANGE pMsgResult;
	pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x09, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.btTaxType = btTaxType;
	pMsgResult.btTaxRate1 = SET_NUMBERH(SET_NUMBERHW(iTaxRate));
	pMsgResult.btTaxRate2 = SET_NUMBERL(SET_NUMBERHW(iTaxRate));
	pMsgResult.btTaxRate3 = SET_NUMBERH(SET_NUMBERLW(iTaxRate));
	pMsgResult.btTaxRate4 = SET_NUMBERL(SET_NUMBERLW(iTaxRate));
	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsgResult, pMsgResult.h.size);
}



void GameProtocol::CGReqMoneyDrawOut(PMSG_REQ_MONEYDRAWOUT * lpMsg, CGameObject *lpObj)
{
	int iMoneyChange;
	BOOL bRETVAL;

	if (lpMsg == NULL)
	{
		return;
	}

	if (strcmp(lpObj->m_PlayerData->GuildName, "") == 0)
	{
		return;
	}

	/*if ((g_CastleSiege.CheckGuildOwnCastle(lpObj->m_PlayerData->GuildName) == FALSE) || (lpObj->m_PlayerData->GuildStatus != G_MASTER))
	{
		sLog->outError("[CastleSiege] CGReqMoneyDrawOut() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->GuildName, lpObj->m_PlayerData->GuildStatus);
	}
	else
	{
		iMoneyChange = MAKE_NUMBERDW(MAKE_NUMBERW(lpMsg->btMoney4, lpMsg->btMoney3), MAKE_NUMBERW(lpMsg->btMoney2, lpMsg->btMoney1));

		if (iMoneyChange <= 0)
		{
			GCAnsMoneyDrawOut(lpObj, 0, 0);
			sLog->outBasic("[CastleSiege] CGReqMoneyDrawOut() ERROR - Req Money < 0 [%s][%s], Guild:(%s)(%d), Money:%d, TotMoney:%d", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->GuildName, lpObj->m_PlayerData->GuildStatus, iMoneyChange, lpObj->m_PlayerData->Money);
		}
		else
		{
			bRETVAL = g_CastleSiege.CheckCastleHasMoney(iMoneyChange);

			if (bRETVAL == FALSE)
			{
				GCAnsMoneyDrawOut(lpObj, 0, 0);
				sLog->outBasic("[CastleSiege] CGReqMoneyDrawOut() ERROR - Castle Money is Low [%s][%s], Guild:(%s)(%d), Money:%d, TotMoney:%d", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->GuildName, lpObj->m_PlayerData->GuildStatus, iMoneyChange, lpObj->m_PlayerData->Money);
			}
			else
			{
				if ((lpObj->m_PlayerData->Money + iMoneyChange) > MAX_ZEN)
				{
					GCAnsMoneyDrawOut(lpObj, 0, 0);
				}
				else
				{
					GS_GDReqCastleMoneyChange(g_MapServerManager.GetMapSvrGroup(), lpObj, -iMoneyChange);
					sLog->outBasic("[CastleSiege] CGReqMoneyDrawOut() REQ OK - [%s][%s], Guild:(%s)(%d), Money:%d, TotMoney:%d", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->GuildName, lpObj->m_PlayerData->GuildStatus, iMoneyChange, lpObj->m_PlayerData->Money);
				}
			}
		}
	}*/
}

void GameProtocol::GCAnsMoneyDrawOut(CGameObject *lpObj, int iResult, __int64 i64Money)
{
	PMSG_ANS_MONEYDRAWOUT pMsgResult;
	pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x10, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.btMoney1 = SET_NUMBERH(SET_NUMBERHW(HIDWORD(i64Money)));
	pMsgResult.btMoney2 = SET_NUMBERL(SET_NUMBERHW(HIDWORD(i64Money)));
	pMsgResult.btMoney3 = SET_NUMBERH(SET_NUMBERLW(HIDWORD(i64Money)));
	pMsgResult.btMoney4 = SET_NUMBERL(SET_NUMBERLW(HIDWORD(i64Money)));
	pMsgResult.btMoney5 = SET_NUMBERH(SET_NUMBERHW(LODWORD(i64Money)));
	pMsgResult.btMoney6 = SET_NUMBERL(SET_NUMBERHW(LODWORD(i64Money)));
	pMsgResult.btMoney7 = SET_NUMBERH(SET_NUMBERLW(LODWORD(i64Money)));
	pMsgResult.btMoney8 = SET_NUMBERL(SET_NUMBERLW(LODWORD(i64Money)));

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::GCAnsCsGateState(CGameObject *lpObj, int iResult, int iGateIndex)
{
	PMSG_ANS_CSGATESTATE pMsgResult;
	pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x11, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.btIndex1 = SET_NUMBERH((iGateIndex & 0xffff));
	pMsgResult.btIndex2 = SET_NUMBERL((iGateIndex & 0xffff));
	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::CGReqCsGateOperate(PMSG_REQ_CSGATEOPERATE * lpMsg, CGameObject *lpObj)
{
	/*int iGateIndex;

	if (strcmp(lpObj->m_PlayerData->GuildName, "") == 0)
	{
		return;
	}

	if (g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
	{
		if (lpObj->m_btCsJoinSide != 1)
		{
			sLog->outBasic("[CastleSiege] CGReqCsGateOperate() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name,
				lpObj->m_PlayerData->GuildName, lpObj->m_PlayerData->GuildStatus);
			return;
		}
	}
	else
	{
		if ((g_CastleSiege.CheckCastleOwnerMember(*lpObj) == FALSE) && (g_CastleSiege.CheckCastleOwnerUnionMember(*lpObj) == FALSE))
		{
			sLog->outBasic("[CastleSiege] CGReqCsGateOperate() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name,
				lpObj->m_PlayerData->GuildName, lpObj->m_PlayerData->GuildStatus);
			return;
		}
	}

	iGateIndex = MAKE_NUMBERW(lpMsg->btIndex1, lpMsg->btIndex2);

	if (g_CastleSiege.CheckCsGateAlive(iGateIndex) == TRUE)
	{
		int iOperateGate = g_CastleSiege.OperateGate(iGateIndex, getGameObject(iGateIndex)->m_iCsNpcExistVal, lpMsg->btOperate);

		if (iOperateGate == 0)
		{
			GCAnsCsGateOperate(lpObj, 0, -1, 0);
			sLog->outBasic("[CastleSiege] CGReqCsGateOperate() ERROR - Operate Gate Failed [%s][%s], Guild:(%s)(%d)", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name,
				lpObj->m_PlayerData->GuildName, lpObj->m_PlayerData->GuildStatus);
		}
		else
		{
			CGameObject* lpObjGate = getGameObject(iGateIndex);
			GCAnsCsGateOperate(lpObj, 1, iGateIndex, lpMsg->btOperate);
			g_CastleSiege.SendCsGateStateViewPort(iGateIndex, lpMsg->btOperate);
			sLog->outBasic("[CastleSiege] CGReqCsGateOperate() OK - [%s][%s], Guild:(%s)(%d), DOOR:(%d)(X:%d,Y:%d)(STATUE:%d)", lpObj->m_PlayerData->ConnectUser->AccountID,
				lpObj->Name, lpObj->m_PlayerData->GuildName, lpObj->m_PlayerData->GuildNumber, iGateIndex, lpObjGate->X, lpObjGate->Y, lpObjGate->m_btCsGateOpen);
		}
	}
	else
	{
		GCAnsCsGateOperate(lpObj, 2, -1, 0);
		sLog->outBasic("[CastleSiege] CGReqCsGateOperate() ERROR - Gate Doesn't Exist [%s][%s], Guild:(%s)(%d)", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name,
			lpObj->m_PlayerData->GuildName, lpObj->m_PlayerData->GuildStatus);
	}
	*/
}

void GameProtocol::GCAnsCsGateOperate(CGameObject *lpObj, int iResult, int iGateIndex, int iGateOperate)
{
	PMSG_ANS_CSGATEOPERATE pMsgResult;

	pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x12, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.btOperate = iGateOperate;
	pMsgResult.btIndex1 = SET_NUMBERH(iGateIndex & 0xffff); //??
	pMsgResult.btIndex2 = SET_NUMBERL(iGateIndex & 0xffff); //??
	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::GCAnsCsGateCurState(CGameObject *lpObj, int iGateIndex, int iGateOperate)
{
	PMSG_ANS_CSGATECURSTATE pMsgResult;
	pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x13, sizeof(pMsgResult));
	pMsgResult.btOperate = iGateOperate;
	pMsgResult.btIndex1 = SET_NUMBERH(iGateIndex & 0xffff); //??
	pMsgResult.btIndex2 = SET_NUMBERL(iGateIndex & 0xffff); //??
	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::GCAnsCsAccessSwitchState(CGameObject *lpObj, int iSwitchIndex, int iSwitchUserIndex, BYTE btSwitchState)
{
	PMSG_ANS_NOTIFYSWITCHPROC pMsgResult;
	pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x14, sizeof(pMsgResult));
	pMsgResult.btIndex1 = SET_NUMBERH(iSwitchIndex & 0xffff); //??
	pMsgResult.btIndex2 = SET_NUMBERL(iSwitchIndex & 0xffff); //??
	pMsgResult.btUserIndex1 = SET_NUMBERH(iSwitchUserIndex & 0xffff); //??
	pMsgResult.btUserIndex2 = SET_NUMBERL(iSwitchUserIndex & 0xffff); //??
	pMsgResult.btSwitchState = btSwitchState;
	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::GCAnsCsAccessCrownState(CGameObject *lpObj, BYTE btCrownState)
{
	PMSG_ANS_NOTIFYCROWNPROC pMsgResult;
	pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x15, sizeof(pMsgResult));
	pMsgResult.btCrownState = btCrownState;

	if (btCrownState == 0)
	{
		if (lpObj->m_iAccumulatedCrownAccessTime < 0 || lpObj->m_iAccumulatedCrownAccessTime > 61000) //season4.0 changed
		{
			lpObj->m_iAccumulatedCrownAccessTime = 0;
		}

		sLog->outBasic("[CastleSiege] [Reg. Attempt] Accumulated Crown AccessTime : %d [%s](%s)(%s)", lpObj->m_iAccumulatedCrownAccessTime, lpObj->m_PlayerData->GuildName,
			lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
	}

	if (btCrownState == 1)
	{
		sLog->outBasic("[CastleSiege] [Reg. Success] Accumulated Crown AccessTime : %d [%s](%s)(%s)", lpObj->m_iAccumulatedCrownAccessTime, lpObj->m_PlayerData->GuildName,
			lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
		lpObj->m_iAccumulatedCrownAccessTime = 0;
	}

	/*if (btCrownState == 2)
	{
		lpObj->m_iAccumulatedCrownAccessTime += GetTickCount() - g_CastleSiege.GetCrownAccessTickCount();

		if (lpObj->m_iAccumulatedCrownAccessTime < 0 || lpObj->m_iAccumulatedCrownAccessTime > 61000) //season4.0 changed
		{
			lpObj->m_iAccumulatedCrownAccessTime = 0;
		}

		if (lpObj->Live == FALSE)
		{
			if (g_CastleSiege.CastleSiegeResetAccumulationAfterKill == TRUE)
			{
				lpObj->m_iAccumulatedCrownAccessTime = 0;
			}
		}

		sLog->outBasic("[CastleSiege] [Reg. Fail] Accumulated Crown AccessTime : %d [%s](%s)(%s)", lpObj->m_iAccumulatedCrownAccessTime, lpObj->m_PlayerData->GuildName,
			lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
	}*/

	pMsgResult.dwAccumulatedCrownAccessTime = lpObj->m_iAccumulatedCrownAccessTime; //??

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsgResult, pMsgResult.h.size);
}
void GameProtocol::GCAnsCsNotifyStart(CGameObject *lpObj, BYTE btStartState)
{
	PMSG_ANS_NOTIFYCSSTART pMsgResult;
	pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x17, sizeof(pMsgResult));
	pMsgResult.btStartState = btStartState;
	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::GCAnsCsNotifyProgress(CGameObject *lpObj, BYTE btCastleSiegeState, LPCSTR lpszGuildName)
{
	if (lpszGuildName == NULL)
	{
		return;
	}

	PMSG_ANS_NOTIFYCSPROGRESS pMsgResult;

	pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x18, sizeof(pMsgResult));
	pMsgResult.btCastleSiegeState = btCastleSiegeState;
	memset(pMsgResult.szGuildName, 0, sizeof(pMsgResult.szGuildName));
	memcpy(pMsgResult.szGuildName, lpszGuildName, sizeof(pMsgResult.szGuildName));

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::GCAnsCsMapSvrTaxInfo(CGameObject *lpObj, BYTE btTaxType, BYTE btTaxRate)
{
	PMSG_ANS_MAPSVRTAXINFO pMsgResult;

	pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x1A, sizeof(pMsgResult));
	pMsgResult.btTaxType = btTaxType;
	pMsgResult.btTaxRate = btTaxRate;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsgResult, pMsgResult.h.size);

}

void GameProtocol::CGReqCsMiniMapData(PMSG_REQ_MINIMAPDATA * lpMsg, CGameObject *lpObj)
{
	/*if ((lpObj->m_btCsJoinSide <= 0) || (g_CastleSiege.CheckUnionGuildMaster(*lpObj) == FALSE))
	{
		GCAnsCsMiniMapData(lpObj, 3);
	}
	else if (g_CastleSiege.GetCastleState() != CASTLESIEGE_STATE_STARTSIEGE)
	{
		GCAnsCsMiniMapData(lpObj, 2);
	}
	else
	{
		g_CastleSiege.AddMiniMapDataReqUser(*lpObj);
		GCAnsCsMiniMapData(lpObj, 1);
	}*/
}

void GameProtocol::GCAnsCsMiniMapData(CGameObject *lpObj, BYTE btResult)
{
	PMSG_ANS_MINIMAPDATA pMsgResult;

	pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x1B, sizeof(pMsgResult));
	pMsgResult.btResult = btResult;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsgResult, pMsgResult.h.size);
}


void GameProtocol::CGReqStopCsMiniMapData(PMSG_REQ_STOPMINIMAPDATA * lpMsg, CGameObject *lpObj)
{

}


void GameProtocol::CGReqCsSendCommand(PMSG_REQ_CSCOMMAND * lpMsg, CGameObject *lpObj)
{
	/*if (lpObj->m_btCsJoinSide <= 0 || g_CastleSiege.CheckUnionGuildMaster(*lpObj) == FALSE)
	{
		return;
	}

	if (g_CastleSiege.GetCastleState() != CASTLESIEGE_STATE_STARTSIEGE)
	{
		return;
	}

	GCAnsCsSendCommand(lpObj->m_btCsJoinSide, lpMsg->btTeam, lpMsg->btX, lpMsg->btY, lpMsg->btCommand);
	*/
}

void GameProtocol::GCAnsCsSendCommand(int iCsJoinSize, BYTE btTeam, BYTE btX, BYTE btY, BYTE btCommand)
{
	PMSG_ANS_CSCOMMAND pMsgResult;

	pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x1D, sizeof(pMsgResult));

	pMsgResult.btTeam = btTeam;
	pMsgResult.btX = btX;
	pMsgResult.btY = btY;
	pMsgResult.btCommand = btCommand;

	for each (std::pair<int,CGameObject*> go in gGameObjects)
	{
		if ((go.second->m_btCsJoinSide == iCsJoinSize) && (go.second->MapNumber == MAP_INDEX_CASTLESIEGE))
		{
			GIOCP.DataSend(go.second->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsgResult, pMsgResult.h.size);
		}
	}
}

void GameProtocol::GCAnsCsLeftTimeAlarm(BYTE btHour, BYTE btMinute)
{
	PMSG_ANS_CSLEFTTIMEALARM pMsgResult;

	pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x1E, sizeof(pMsgResult));
	pMsgResult.btHour = btHour;
	pMsgResult.btMinute = btMinute;

	for each (std::pair<int, CGameObject*> go in gGameObjects)
	{
		GIOCP.DataSend(go.second->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsgResult, pMsgResult.h.size);
	}

}

void GameProtocol::GCAnsSelfCsLeftTimeAlarm(CGameObject *lpObj, BYTE btHour, BYTE btMinute)
{
	PMSG_ANS_CSLEFTTIMEALARM pMsgResult;

	pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x1E, sizeof(pMsgResult));
	pMsgResult.btHour = btHour;
	pMsgResult.btMinute = btMinute;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::CGReqCsSetEnterHuntZone(PMSG_REQ_CSHUNTZONEENTER * lpMsg, CGameObject *lpObj)
{

	/*if (g_CastleSiege.CheckCastleOwnerMember(*lpObj) == FALSE)
	{
		GCAnsCsSetEnterHuntZone(lpObj, 2, lpMsg->btHuntZoneEnter);
	}
	else if (g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
	{
		GCAnsCsSetEnterHuntZone(lpObj, 0, lpMsg->btHuntZoneEnter);
	}
	else
	{
		g_CastleSiege.SetHuntZoneEnter(lpMsg->btHuntZoneEnter);
		GCAnsCsSetEnterHuntZone(lpObj, 1, lpMsg->btHuntZoneEnter);
	}*/
}

void GameProtocol::GCAnsCsSetEnterHuntZone(CGameObject *lpObj, BYTE btResult, BYTE btEnterHuntZone)
{
	PMSG_ANS_CSHUNTZONEENTER pMsgResult;

	pMsgResult.h.set((LPBYTE)&pMsgResult, 0xB2, 0x1F, sizeof(pMsgResult));
	pMsgResult.btResult = btResult;
	pMsgResult.btHuntZoneEnter = btEnterHuntZone;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::CGReqNpcDbList(PMSG_REQ_NPCDBLIST * lpMsg, CGameObject *lpObj)
{
	/*PMSG_ANS_NPCDBLIST pResult;

	if (lpMsg == NULL)
	{
		return;
	}

	if (strcmp(lpObj->m_PlayerData->GuildName, "") == 0)
	{
		return;
	}

	if (g_CastleSiege.CheckGuildOwnCastle(lpObj->m_PlayerData->GuildName) == FALSE || ((lpObj->m_PlayerData->GuildStatus != 128) && (lpObj->m_PlayerData->GuildStatus != 64)))
	{
		sLog->outError("[CastleSiege] CGReqNpcDbList() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name,
			lpObj->m_PlayerData->GuildName, lpObj->m_PlayerData->GuildStatus);
		pResult.h.set((LPBYTE)&pResult, 0xB3, sizeof(pResult));
		pResult.iCount = 0;
		pResult.btResult = 2;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, sizeof(pResult));
	}
	else
	{
		switch (lpMsg->btMonsterCode)
		{
		case 1:
			g_CastleSiege.SendNpcStateList(*lpObj, 277);
			break;
		case 2:
			g_CastleSiege.SendNpcStateList(*lpObj, 283);
			break;
		}
	}*/
}

/*
void GameProtocol::CGReqCsRegGuildList(PMSG_REQ_CSREGGUILDLIST * lpMsg, CGameObject *lpObj)
{
	if (lpMsg == NULL) return;

	GS_GDReqAllGuildMarkRegInfo(g_MapServerManager.GetMapSvrGroup(), lpObj);
}

void GameProtocol::CGReqCsAttkGuildList(PMSG_REQ_CSATTKGUILDLIST * lpMsg, CGameObject *lpObj)
{
	char cBUFFER[1625]; //season 2.5 changed old -> 1022
	PMSG_ANS_CSATTKGUILDLIST* lpMsgSend;
	PMSG_CSATTKGUILDLIST* lpMsgSendBody;
	int iCount;

	if (lpMsg == NULL)	return;

	lpMsgSend = (PMSG_ANS_CSATTKGUILDLIST*)(cBUFFER);
	lpMsgSendBody = (PMSG_CSATTKGUILDLIST*)(cBUFFER + sizeof(PMSG_ANS_CSATTKGUILDLIST));

	iCount = 0;

	lpMsgSend->btResult = g_CastleSiege.GetCsAttkGuildList(lpMsgSendBody, iCount);
	lpMsgSend->iCount = iCount;

	if (lpMsgSend->iCount < 0)	 lpMsgSend->iCount = 0;
	if (lpMsgSend->iCount > 100)lpMsgSend->iCount = 100;

	lpMsgSend->h.set((LPBYTE)lpMsgSend, 0xB5, ((lpMsgSend->iCount * sizeof(PMSG_CSATTKGUILDLIST)) + sizeof(PMSG_ANS_CSATTKGUILDLIST)));
	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)cBUFFER, ((lpMsgSend->iCount * sizeof(PMSG_CSATTKGUILDLIST)) + sizeof(PMSG_ANS_CSATTKGUILDLIST)));
}
*/

void GameProtocol::CGReqWeaponUse(PMSG_REQ_USEWEAPON * aRecv, CGameObject *lpObj)
{
	WORD wObjIndex = MAKE_NUMBERW(aRecv->btObjIndexH, aRecv->btObjIndexL);

	CGameObject* lpOwnerObj = lpObj;
	CGameObject* lpWeaponObj = getGameObject(wObjIndex);

	if (lpOwnerObj->Type != OBJ_USER)
	{
		return;
	}

	if (lpWeaponObj->Type != OBJ_NPC)
	{
		return;
	}

	PMSG_ANS_USEWEAPON pMsg = { 0 };

	PHeadSubSetB((LPBYTE)&pMsg, 0xB7, 0x01, sizeof(pMsg));
	pMsg.btResult = 1;

	if (lpWeaponObj->Class == 221)	// Slingshot attack
	{
		pMsg.btWeaponType = 1;
		pMsg.btObjIndexH = SET_NUMBERH(wObjIndex);
		pMsg.btObjIndexL = SET_NUMBERL(wObjIndex);
		g_CsNPC_Weapon.GetTargetPointXY(lpWeaponObj->Class, aRecv->btTargetIndex - 1, pMsg.btPointX, pMsg.btPointY, TRUE);
	}
	else if (lpWeaponObj->Class == 222)	// Slingshot defense
	{
		pMsg.btWeaponType = 2;
		pMsg.btObjIndexH = SET_NUMBERH(wObjIndex);
		pMsg.btObjIndexL = SET_NUMBERL(wObjIndex);
		g_CsNPC_Weapon.GetTargetPointXY(lpWeaponObj->Class, aRecv->btTargetIndex - 1, pMsg.btPointX, pMsg.btPointY, TRUE);
	}

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));

	pMsg.btResult = 2;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpWeaponObj->VpPlayer2[n]->type == OBJ_USER)
		{
			if (lpWeaponObj->VpPlayer2[n]->state)
			{
				if (lpObj->m_Index != lpWeaponObj->VpPlayer2[n]->number)
				{
					GIOCP.DataSend(lpWeaponObj->VpPlayer2[n]->number, (LPBYTE)&pMsg, sizeof(pMsg));
				}
			}
		}
	}

	gObjNotifyUseWeaponV1(*lpOwnerObj, *lpWeaponObj, pMsg.btPointX, pMsg.btPointY);
	g_CsNPC_Weapon.SetWeaponCalDamageInfo(wObjIndex, pMsg.btPointX, pMsg.btPointY, 10000);
}



void GameProtocol::CGReqWeaponDamageValue(PMSG_REQ_WEAPON_DAMAGE_VALUE * aRecv, CGameObject *lpObj)
{

	BYTE btTargetX = 0;
	BYTE btTargetY = 0;
	WORD wObjIndex = MAKE_NUMBERW(aRecv->btObjIndexH, aRecv->btObjIndexL);

	if (g_CsNPC_Weapon.GetWeaponCalDamageInfo(wObjIndex, btTargetX, btTargetY) == TRUE)
	{
		gObjNotifyUseWeaponDamage(*getGameObject(wObjIndex), btTargetX, btTargetY);
	}
	else
	{
		MsgOutput(*lpObj, Lang.GetText(0, 181));
	}
}

void GameProtocol::GCSendObjectCreationState(CGameObject* lpObj)
{

	PMSG_NOTIFY_OBJECT_CREATION_STATE	pMsg = { 0 };

	if (lpObj->Class == 278)
	{
		PHeadSubSetB((LPBYTE)&pMsg, 0xB9, 0x01, sizeof(pMsg));
		pMsg.btObjIndexH = SET_NUMBERH(lpObj->m_Index);
		pMsg.btObjIndexL = SET_NUMBERL(lpObj->m_Index);
		pMsg.btCreationState = lpObj->m_btCreationState;
		MsgSendV2(lpObj, (LPBYTE)&pMsg, sizeof(pMsg));
	}
}

void GameProtocol::CGReqGuildMarkOfCastleOwner(PMSG_REQ_GUILDMARK_OF_CASTLEOWNER * aRecv, CGameObject *lpObj)
{
	/*
	char szGuildName[MAX_GUILD_LEN + 1] = { 0 };

	memcpy(szGuildName, g_CastleSiegeSync.GetCastleOwnerGuild(), MAX_GUILD_LEN);

	if (szGuildName)
	{
		GUILD_INFO_STRUCT * lpGuild = Guild.SearchGuild(szGuildName);

		if (lpGuild != NULL)
		{
			PMSG_ANS_GUILDMARK_OF_CASTLEOWNER pMsg = { 0 };

			PHeadSubSetB((LPBYTE)&pMsg, 0xB9, 0x02, sizeof(pMsg));
			memcpy(pMsg.GuildMarkOfCastleOwner, lpGuild->Mark, sizeof(pMsg.GuildMarkOfCastleOwner));

			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));
		}
	}*/
}

void GameProtocol::CGReqCastleHuntZoneEntrance(PMSG_REQ_MOVE_TO_CASTLE_HUNTZONE * aRecv, CGameObject *lpObj)
{
	/*PMSG_ANS_MOVE_TO_CASTLE_HUNTZONE pMsg = { 0 };

	PHeadSubSetB((LPBYTE)&pMsg, 0xB9, 0x05, sizeof(pMsg));

	pMsg.btResult = 0;

	int iEnterTaxMoney = 0;

	iEnterTaxMoney = g_CastleSiegeSync.GetTaxHuntZone(*lpObj, TRUE);

	if (iEnterTaxMoney < 0)
	{
		iEnterTaxMoney = 0;
	}

	BOOL bPermission = FALSE;

	if (g_CastleSiege.GetHuntZoneEnter())
	{
		bPermission = TRUE;
	}

	if (lpObj->m_PlayerData->lpGuild)
	{
		if (g_CastleSiege.CheckCastleOwnerMember(*lpObj))
		{
			bPermission = TRUE;
		}

		if (g_CastleSiege.CheckCastleOwnerUnionMember(*lpObj))
		{
			bPermission = TRUE;
		}
	}
	else
	{
		if (g_CastleSiege.GetHuntZoneEnter())
		{
			bPermission = TRUE;
		}
	}

	if (bPermission == TRUE)
	{
		if (lpObj->m_PlayerData->Money >= iEnterTaxMoney)
		{
			if (lpObj->MapNumber == MAP_INDEX_CASTLESIEGE)
			{
				if (gObjMoveGate(*lpObj, 95) == TRUE)
				{
					pMsg.btResult = 1;
					int iOldMoney = lpObj->m_PlayerData->Money;

					lpObj->m_PlayerData->Money -= iEnterTaxMoney;

					g_CastleSiegeSync.AddTributeMoney(iEnterTaxMoney);

					if (lpObj->m_PlayerData->Money < 0)
					{
						lpObj->m_PlayerData->Money = 0;
					}

					GCMoneySend(lpObj, lpObj->m_PlayerData->Money);

					sLog->outBasic("[Castle HuntZone] [%s][%s] - Entrance TAX : %d - %d = %d", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, iOldMoney, iEnterTaxMoney, 
						lpObj->m_PlayerData->Money);
				}
			}
		}
	}

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));
	*/
}

void GameProtocol::CGReqJewelMix(PMSG_REQ_JEWEL_MIX * lpMsg, CGameObject *lpObj)
{
	if (lpMsg == NULL)
		return;

	bool bCheck = false;
	Check_SameSerialItem(*lpObj, 3, bCheck);

	if (bCheck == true)
		return;
	CJewelMixSystem::MixJewel(*lpObj, lpMsg->btJewelType, lpMsg->btJewelMix);
}

void GameProtocol::GCAnsJewelMix(CGameObject *lpObj, int iResult)
{
	PMSG_ANS_JEWEL_UNMIX pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xBC, 0x00, sizeof(pMsg));
	pMsg.btResult = iResult;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));
}


void GameProtocol::CGReqJewelUnMix(PMSG_REQ_JEWEL_UNMIX * lpMsg, CGameObject *lpObj)
{
	if (lpMsg == NULL)
		return;

	bool bCheck = false;
	Check_SameSerialItem(*lpObj, 3, bCheck);

	if (bCheck == true)
		return;
	CJewelMixSystem::UnMixJewel(*lpObj, lpMsg->btJewelType, lpMsg->btJewelLevel, lpMsg->btJewelPos);
}

void GameProtocol::GCAnsJewelUnMix(CGameObject *lpObj, int iResult)
{
	PMSG_ANS_JEWEL_UNMIX pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xBC, 0x01, sizeof(pMsg));
	pMsg.btResult = iResult;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));
}

void GameProtocol::CGReqCrywolfInfo(PMSG_REQ_CRYWOLF_INFO* lpMsg, CGameObject *lpObj)
{

	/*if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		GCAnsCrywolfInfo(lpObj, g_Crywolf.GetOccupationState(), g_Crywolf.GetCrywolfState());
	}

	else
	{
		GCAnsCrywolfInfo(lpObj, g_CrywolfSync.GetOccupationState(), g_CrywolfSync.GetCrywolfState());
	}*/
}

void GameProtocol::GCAnsCrywolfInfo(CGameObject *lpObj, BYTE btOccupationState, BYTE btCrywolfState)
{
	/*PMSG_ANS_CRYWOLF_INFO pMsg = { 0 };

	PHeadSubSetB((LPBYTE)&pMsg, 0xBD, 0x00, sizeof(pMsg));
	pMsg.btOccupationState = btOccupationState;
	pMsg.btCrywolfState = btCrywolfState;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));
	*/
}


struct PMSG_ANS_CRYWOLF_ALTAR_CONTRACT
{
	PBMSG_HEAD2 h;	// C1:BD:03
	BYTE btResult;	// 4
	BYTE btAltarState;	// 5
	BYTE btObjIndexH;	// 6
	BYTE btObjIndexL;	// 7
};

void GameProtocol::CGReqAlatrContract(PMSG_REQ_CRYWOLF_ALTAR_CONTRACT* lpMsg, CGameObject *lpObj)
{
	/*int iAltarIndex = MAKE_NUMBERW(lpMsg->btObjIndexH, lpMsg->btObjIndexL);
	CGameObject* lpAltarObj = getGameObject(iAltarIndex);

	if (!CRYWOLF_ALTAR_CLASS_RANGE(lpAltarObj->Class))
		return;

	PMSG_ANS_CRYWOLF_ALTAR_CONTRACT pMsg = { 0 };

	PHeadSubSetB((LPBYTE)&pMsg, 0xBD, 0x03, sizeof(pMsg));
	pMsg.btResult = 0;
	pMsg.btAltarState = g_CrywolfNPC_Altar.GetAltarState(lpAltarObj->Class);
	pMsg.btObjIndexH = lpMsg->btObjIndexH;
	pMsg.btObjIndexL = lpMsg->btObjIndexL;

	if (pMsg.btAltarState == 0 &&
		lpObj->Type == OBJ_USER &&
		lpObj->Class == CLASS_ELF &&
		lpObj->Level >= MIN_ELF_LEVEL_ALTAR)
	{
		if (g_CrywolfNPC_Altar.SetAltarUserIndex(iAltarIndex, lpAltarObj->Class, *lpObj) != FALSE)
		{
			pMsg.btResult = 1;
		}
	}
	else
	{
		MsgOutput(*lpObj, Lang.GetText(0, 256), (int)MIN_ELF_LEVEL_ALTAR);
	}

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));
	*/
}

struct PMSG_ANS_CRYWOLF_BENEFIT_PLUS_CHAOSRATE
{
	PBMSG_HEAD2 h;	// C1:BD:09
	BYTE btPlusChaosRate;	// 4
};


void GameProtocol::CGReqPlusChaosRate(PMSG_REQ_CRYWOLF_BENEFIT_PLUS_CHAOSRATE* lpMsg, CGameObject *lpObj)
{
	/*
	PMSG_ANS_CRYWOLF_BENEFIT_PLUS_CHAOSRATE pMsg = { 0 };

	PHeadSubSetB((LPBYTE)&pMsg, 0xBD, 0x09, sizeof(pMsg));
	pMsg.btPlusChaosRate = g_CrywolfSync.GetPlusChaosRate();

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));
	*/
}

void GameProtocol::CGReqKanturuStateInfo(PMSG_REQ_KANTURU_STATE_INFO* lpMsg, CGameObject *lpObj)
{
	/*
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		g_KanturuEntranceNPC.NotifyEntranceInfo(*lpObj);
	}
	*/
}


void GameProtocol::GCReqEnterKanturuBossMap(PMSG_REQ_ENTER_KANTURU_BOSS_MAP* lpMsg, CGameObject *lpObj)
{
	/*
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		g_KanturuEntranceNPC.NotifyResultEnterKanturuBossMap(*lpObj);
	}
	*/
}

void GameProtocol::WereWolfQuarrelCheck(CGameObject* lpObj)
{

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	g_QuestInfo.CheckQuestMapEnterOnWerwolf(*lpObj);
}

void GameProtocol::GateKeeperCheck(CGameObject* lpObj)
{

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	g_QuestInfo.CheckQuestMapEnterOnGateKeeper(*lpObj);
}

struct PMSG_ILLUSIONTEMPLE_KILLPOINT
{
	PBMSG_HEAD2 h;
	BYTE btKillPoint;
};

void GameProtocol::GCSendIllusionTempleKillPoint(CGameObject* lpObj, BYTE KillPoint)
{
	PMSG_ILLUSIONTEMPLE_KILLPOINT pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xBF, 0x06, sizeof(pMsg));
	pMsg.btKillPoint = KillPoint;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
}

void GameProtocol::CGReqIllusionTempleUseSkill(PMSG_REQ_USEILLUSIONTEMPLESKILL * aRecv, CGameObject* lpObj)
{

	//g_IT_Event.ITR_USeSkill(*lpObj, MAKE_NUMBERW(aRecv->MagicNumberH, aRecv->MagicNumberL), MAKE_NUMBERW(aRecv->btTargetObjIndexH, aRecv->btTargetObjIndexL), aRecv->btDis);
}

void GameProtocol::GCIllusionTempleSendReward(CGameObject* lpObj)
{

	//g_IT_Event.ReqEventReward(lpObj);
}

void GameProtocol::GCMasterLevelUpMsgSend(CGameObject* lpObj)
{

	PMSG_MASTER_LEVEL_UP_SEND pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x51, sizeof(pMsg));

	pMsg.MasterLevel = lpObj->m_PlayerData->MasterLevel;
	pMsg.MLPoint = lpObj->m_PlayerData->MasterPoint;
	pMsg.GainPoint = g_MaxStatsInfo.GetClass.MLPointPerLevel;
	pMsg.MaxPoint = g_MaxStatsInfo.GetClass.MLUserMaxLevel;
	pMsg.MaxLife = lpObj->MaxLife + lpObj->AddLife;
	pMsg.MaxMana = lpObj->MaxMana + lpObj->AddMana;
	pMsg.MaxBP = lpObj->MaxBP + lpObj->AddBP;
	pMsg.MaxShield = lpObj->iMaxShield + lpObj->iAddShield;
	pMsg.IGCMaxLife = lpObj->MaxLife + lpObj->AddLife;
	pMsg.IGCMaxMana = lpObj->MaxMana + lpObj->AddMana;
	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);

	GCSendEffectInfo(lpObj, 16);
}

struct PMSG_REQ_REGISTER_LUCKYCOIN
{
	PBMSG_HEAD2 h;
	int iIndex;
	char szAccountID[11];
	BYTE btPos;
};

void GameProtocol::CGReqLuckyCoinInfo(CGameObject* lpObj)
{
	if (lpObj->UseEventServer == FALSE)
	{
		PMSG_REQ_LUCKYCOIN pMsg = { 0 };
		PHeadSubSetB((LPBYTE)&pMsg, 0xBE, 0x19, sizeof(pMsg));

		memcpy(pMsg.szUID, lpObj->m_PlayerData->ConnectUser->AccountID, 10);
		pMsg.szUID[10] = 0;
		pMsg.iIndex = lpObj->m_Index;

		//wsDataCli.DataSend((PCHAR)&pMsg, pMsg.h.size);

		lpObj->UseEventServer = TRUE;
	}
}

void GameProtocol::CGReqLuckyCoinRegister(PMSG_REQ_LUCKYCOIN_REGISTER *aRecv, CGameObject* lpObj)
{

	if (lpObj->m_IfState.type == 1)
	{
		return;
	}

	if (lpObj->m_IfState.type != 7)
	{
		return;
	}

	if (lpObj->UseEventServer == FALSE)
	{
		BYTE btPos = 0xFF;

		for (int i = INVENTORY_BAG_START; i < MAIN_INVENTORY_SIZE; i++)
		{
			if (lpObj->pntInventory[i]->IsItem() == TRUE)
			{
				if (lpObj->pntInventory[i]->m_Type == ITEMGET(14, 100))
				{
					btPos = i;
					break;
				}
			}
		}

		if (btPos == 0xFF)
		{
			PMSG_ANS_LUCKYCOIN_REGISTER pMsg;
			PHeadSubSetB((LPBYTE)&pMsg, 0xBF, 0x0C, sizeof(pMsg));

			pMsg.btResult = 0;
			pMsg.iLuckyCoin = 0;

			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);

			return;
		}

		lpObj->UseEventServer = TRUE;

		PMSG_REQ_REGISTER_LUCKYCOIN pMsg = { 0 };
		PHeadSubSetB((LPBYTE)&pMsg, 0xBE, 0x18, sizeof(pMsg));

		pMsg.btPos = btPos;
		pMsg.iIndex = lpObj->m_Index;
		memcpy(pMsg.szAccountID, lpObj->m_PlayerData->ConnectUser->AccountID, 10);

		//wsDataCli.DataSend((char *)&pMsg, pMsg.h.size); // TODO

	}
	return;
}

void GameProtocol::CGReqLuckyCoinTrade(PMSG_REQ_LUCKYCOIN_TRADE * aRecv, CGameObject* lpObj)
{
	//CItemBagEx * Bag = NULL;

	if (lpObj->m_IfState.type == 1)
	{
		return;
	}

	if (lpObj->m_IfState.type != 7)
	{
		return;
	}

	int Count = 0;
	CItemObject* Item;
	time_t Duration;
	int iBagResult = 0;

	for (int i = INVENTORY_BAG_START; i < MAIN_INVENTORY_SIZE; ++i)
	{
		if (lpObj->pntInventory[i]->IsItem() == TRUE && lpObj->pntInventory[i]->m_Type == ITEMGET(14, 100))
			Count += lpObj->pntInventory[i]->m_Durability;
	}

	if (Count < aRecv->iLuckyCoinTradeCount)
	{
		GCAnsLuckyCoinTrade(lpObj, 0);
		return;
	}

	if (!CheckInventoryEmptySpace(*lpObj, 4, 2))
	{
		GCAnsLuckyCoinTrade(lpObj, 2);
		return;
	}

	Count = aRecv->iLuckyCoinTradeCount;

	for (int i = INVENTORY_BAG_START; i < MAIN_INVENTORY_SIZE; ++i)
	{
		if (lpObj->pntInventory[i]->IsItem() == TRUE && lpObj->pntInventory[i]->m_Type == ITEMGET(14, 100))
		{
			if (lpObj->pntInventory[i]->m_Durability > Count)
			{
				lpObj->pntInventory[i]->m_Durability -= Count;
				GCItemDurSend2(lpObj, i, lpObj->pntInventory[i]->m_Durability, 0);
				Count = 0;
			}
			else
			{
				Count -= lpObj->pntInventory[i]->m_Durability;
				gObjInventoryDeleteItem(*lpObj, i);
				GCInventoryItemDeleteSend(lpObj, i, 1);
			}
			if (Count == 0)
				break;
		}
	}

	switch (aRecv->iLuckyCoinTradeCount)
	{
	case 10:
		iBagResult = g_BagManager.GetItemFromBag(*lpObj, BAG_EVENT, EVENTBAG_LUCKYCOIN10, lpObj->m_Index, *Item, Duration);
		break;
	case 20:
		iBagResult = g_BagManager.GetItemFromBag(*lpObj, BAG_EVENT, EVENTBAG_LUCKYCOIN20, lpObj->m_Index, *Item, Duration);
		break;
	case 30:
		iBagResult = g_BagManager.GetItemFromBag(*lpObj, BAG_EVENT, EVENTBAG_LUCKYCOIN30, lpObj->m_Index, *Item, Duration);
		break;
	default:
		GCAnsLuckyCoinTrade(lpObj, 4);
		return;
	}

	GJSetCharacterInfo(lpObj, FALSE);

	if (iBagResult == 0)
	{
		GCAnsLuckyCoinTrade(lpObj, 2);
		return;
	}

	else if (iBagResult == 2 || iBagResult == 3)
	{
		GCAnsLuckyCoinTrade(lpObj, 1);
		return;
	}

	ItemCreate(lpObj, 235, 0, 0, Item->m_Type, Item->m_Level, Item->m_Durability, Item->m_Option1, Item->m_Option2, Item->m_Option3, lpObj->m_Index, Item->m_NewOption, Item->m_SetOption, Duration, 0, 0);
	//sLog->outBasic("[Lucky Coin] [%s][%s] Trade Lucky Coin [%d]", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, aRecv->iLuckyCoinTradeCount);
	Item->Convert(Item->m_Type, Item->m_Option1, Item->m_Option2, Item->m_Option3, Item->m_NewOption, Item->m_SetOption, Item->m_ItemOptionEx, 0, 0, Duration, CURRENT_DB_VERSION);
}

void GameProtocol::GCAnsLuckyCoinTrade(CGameObject* lpObj, BYTE Result)
{
	PMSG_ANS_LUCKYCOIN_TRADE pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xBF, 0x0D, sizeof(pMsg));

	pMsg.btResult = Result;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
}

void GameProtocol::GCSendObjectHP(CGameObject* lpObj, CGameObject* lpTargetObj)
{
	PMSG_TARGET_HP pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xFA, 0x05, sizeof(pMsg));

	pMsg.m_Index = lpTargetObj->m_Index;
	pMsg.m_Hp = lpTargetObj->Life;
	pMsg.m_MaxHp = lpTargetObj->MaxLife + lpTargetObj->AddLife;
	pMsg.m_Level = lpTargetObj->Level;
	pMsg.m_bIceEffect = gObjCheckUsedBuffEffect(*lpTargetObj, BUFFTYPE_FREEZE);
	pMsg.m_bPoisonEffect = gObjCheckUsedBuffEffect(*lpTargetObj, BUFFTYPE_POISON);

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
	this->MsgSendV2(lpTargetObj, (LPBYTE)&pMsg, pMsg.h.size);
}

void GameProtocol::CGReqAntiCheatRecv(CGameObject* lpObj, PMSG_SEND_AH_INFO *aRecv)
{
	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	if (lpObj->m_PlayerData->LivePacketTime == 0 || g_ConfigRead.antihack.EnablePacketTimeCheck == FALSE)
	{
		lpObj->m_PlayerData->LivePacketTime = GetTickCount();
	}

	else
	{
		DWORD tick = GetTickCount() - lpObj->m_PlayerData->LivePacketTime;

		if (tick < g_ConfigRead.antihack.PacketTimeMinTime)
		{
			sLog->outError("[ANTI-HACK] (%s)(%s)(%s) (Map:%d)(X:%d)(Y:%d) SpeedHack detected -> Tick (Current:%d/Minimum:%d)",
				lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->ConnectUser->IP, lpObj->MapNumber, lpObj->X, lpObj->Y, tick,
				g_ConfigRead.antihack.PacketTimeMinTime);

			lpObj->m_PlayerData->LiveCheckDetectCount++;

			if (lpObj->m_PlayerData->LiveCheckDetectCount >= 3)
			{

				if (g_ConfigRead.antihack.EnableHackDetectMessage == TRUE)
				{
					MsgOutput(*lpObj, (char *)g_ConfigRead.antihack.HackDetectMessage.c_str());
				}

				if (g_ConfigRead.EnableAutoBanUserAccount == TRUE)
				{
					GDReqBanUser(lpObj, 1, 1);
				}

				//this->GCSendDisableReconnect(lpObj);
				GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);

				return;
			}
		}

		else
		{
			lpObj->m_PlayerData->LivePacketTime = WorldTimer::getMSTime();
		}
	}

	if (g_ConfigRead.antihack.EnabledAgiCheck == false)
	{
		lpObj->m_PlayerData->AgiCheckTime = WorldTimer::getMSTime();
		return;
	}

	if (lpObj->m_PlayerData->BlessSkillCast == true)
	{
		lpObj->m_PlayerData->AgiCheckTime = WorldTimer::getMSTime();
		return;
	}

	WORD Agility = lpObj->m_PlayerData->Dexterity + lpObj->AddDexterity;

	int AgilityDiff = (lpObj->m_PlayerData->Dexterity + lpObj->AddDexterity) - aRecv->Agi;

	if (aRecv->type == 1)
	{
		if (AgilityDiff <= -30 || AgilityDiff >= 30)
		{
			sLog->outError("[ANTI-HACK] (%s)(%s)(%s) Dexterity check error: %d/%d", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->ConnectUser->IP,
				Agility, aRecv->Agi);

			if (g_ConfigRead.antihack.EnableHackDetectMessage == TRUE)
			{
				MsgOutput(*lpObj, (char *)g_ConfigRead.antihack.HackDetectMessage.c_str());
			}

			if (g_ConfigRead.EnableAutoBanUserAccount == TRUE)
			{
				GDReqBanUser(lpObj, 1, 1);
			}

			//this->GCSendDisableReconnect(lpObj);
			GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
		}

		else
		{
			lpObj->m_PlayerData->AgiCheckTime = GetTickCount();
		}
	}
	else
	{
		sLog->outError("[ANTI-HACK] (%s)(%s)(%s) Cheat detected: %s", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->ConnectUser->IP,
			aRecv->Agi == 0 ? "SpeedHack" : "Proxy");

		if (g_ConfigRead.antihack.EnableHackDetectMessage == TRUE)
		{
			MsgOutput(*lpObj, (char *)g_ConfigRead.antihack.HackDetectMessage.c_str());
		}

		if (g_ConfigRead.EnableAutoBanUserAccount == TRUE)
		{
			GDReqBanUser(lpObj, 1, 1);
		}

		//this->GCSendDisableReconnect(lpObj);
		GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);

	}
}

void GameProtocol::CGReqFileCrcRecv(CGameObject* lpObj, PMSG_REQ_CRCDATA *aRecv)
{
	if (g_ConfigRead.MainCRC == 0)
	{
		lpObj->CrcCheckTime = 0;
		return;
	}

	if (aRecv->MainExe == g_ConfigRead.MainCRC &&
		aRecv->IGCDLL == g_ConfigRead.DLLCRC &&
		aRecv->PlayerBMD == g_ConfigRead.PlayerCRC &&
		aRecv->SkillCRC == g_ConfigRead.SkillCRC
		)
	{
		lpObj->CrcCheckTime = 0;
	}

	else
	{
		sLog->outBasic("[ANTI-HACK] (%s)(%s)(%s) Invalid Game-Client files (Recv/Valid) (Main:%X/%X) (IGC:%X/%X) (Player:%X/%X) (SkillCRC: %X/%X)", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->ConnectUser->IP, aRecv->MainExe, g_ConfigRead.MainCRC,
			aRecv->IGCDLL, g_ConfigRead.DLLCRC, aRecv->PlayerBMD, g_ConfigRead.PlayerCRC, aRecv->SkillCRC, g_ConfigRead.SkillCRC);

		if (g_ConfigRead.antihack.EnableHackDetectMessage == TRUE)
		{
			MsgOutput(*lpObj, (char *)g_ConfigRead.antihack.HackDetectMessage.c_str());
		}

		if (g_ConfigRead.EnableAutoBanUserAccount == TRUE)
		{
			GDReqBanUser(lpObj, 1, 1);
		}

		//this->GCSendDisableReconnect(lpObj);
		GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);

	}
}

void GameProtocol::GCElementalDamageSend(CGameObject* lpObj, int TargetIndex, int Damage, int Elemental)
{
	PMSG_ELEMENTALDAMAGE pResult;
	PHeadSetB((LPBYTE)&pResult, 0xD8, sizeof(pResult));

	pResult.NumberH = SET_NUMBERH(TargetIndex);
	pResult.NumberL = SET_NUMBERL(TargetIndex);
	pResult.Damage = Damage;
	pResult.btElement = Elemental;
	pResult.New = 0; ///NEW
	pResult.New1 = getGameObject(TargetIndex)->Life; ///[k2]
	pResult.New2 = 512; ///NEW
	pResult.btTargetH = SET_NUMBERH(lpObj);
	pResult.btTargetL = SET_NUMBERL(lpObj);

	if (getGameObject(TargetIndex)->Type == OBJ_USER)
	{
		GIOCP.DataSend(TargetIndex, (LPBYTE)&pResult, pResult.h.size);
	}

	if (cManager.WatchTargetIndex == TargetIndex || cManager.WatchTargetIndex == lpObj->m_Index)
	{
		cManager.DataSend((LPBYTE)&pResult, pResult.h.size);
	}

	if (lpObj->Type == OBJ_USER)
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
	}
}

void GameProtocol::CGReqEnterAcheron(CGameObject* lpObj)
{

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	int iTargetNumber = lpObj->TargetNumber;

	if (iTargetNumber == -1)
	{
		return;
	}

	if (getGameObject(iTargetNumber)->Class != 580)
	{
		return;
	}

	BYTE pos = 0xFF;

	for (int i = 12; i < MAIN_INVENTORY_SIZE; i++)
	{
		if (lpObj->pntInventory[i]->m_Type == ITEMGET(13, 146))
		{
			pos = i;
			break;
		}
	}

	if (pos == 0xFF)
	{
		PMSG_RESULT pMsg;
		PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x21, sizeof(pMsg));

		pMsg.result = 0x01;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	gObjMoveGate(*lpObj, 417);
	sLog->outBasic("[%s][%s] Entered to Acheron Map (Spirit Map serial: %I64d)", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name,
		lpObj->pntInventory[pos]->m_Number);

	gObjInventoryDeleteItem(*lpObj, pos);
	this->GCInventoryItemDeleteSend(lpObj, pos, 1);
}

void GameProtocol::GCObjectLifeInfo(CGameObject *lpObj, short sObjNum, int iMaxLife, int iCurLife)
{
	PMSG_OBJECT_LIFE_INFO pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xEC, 0x10, sizeof(pMsg));

	pMsg.ObjNumberH = HIBYTE(sObjNum);
	pMsg.ObjNumberL = LOBYTE(sObjNum);

	pMsg.MaxLifeHH = iMaxLife >> 24;
	pMsg.MaxLifeHL = iMaxLife >> 8;
	pMsg.MaxLifeLH = iMaxLife >> 16;
	pMsg.MaxLifeLL = iMaxLife;

	pMsg.CurLifeHH = iCurLife >> 24;
	pMsg.CurLifeHL = iCurLife >> 8;
	pMsg.CurLifeLH = iCurLife >> 16;
	pMsg.CurLifeLL = iCurLife;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));
}

void GameProtocol::GCChaosMachinePriceSend(CGameObject* lpObj)
{
	if (lpObj->Type != OBJ_USER)
	{
		sLog->outError("[ERROR] Index %d not USER", lpObj);
		return;
	}

	BYTE SendData[65535];
	PMSG_PRICE_INFO pMsg;
	PMSG_PRICE_ITEM_LIST pList;
	pMsg.h.c = 0xC2;
	pMsg.h.headcode = 0xFA;
	pMsg.subc = 0x96;
	int lOfs = 0;
	lOfs += sizeof(pMsg);

	pMsg.count = 0;
	pMsg.Type = 1;
	for (short i = 0; i < CHAOS_BOX_SIZE; ++i)
	{
		if (lpObj->pntChaosBox[i]->IsItem())
		{
			lpObj->pntChaosBox[i]->Value();
			pList.BuyPrice = lpObj->pntChaosBox[i]->m_BuyMoney;
			pList.SellPrice = lpObj->pntChaosBox[i]->m_SellMoney;
			pList.Pos = i;
			memcpy(&SendData[lOfs], &pList, sizeof(pList));
			lOfs += sizeof(pList);
			pMsg.count++;
		}
	}
	int size = lOfs + pMsg.count * sizeof(PMSG_PRICE_ITEM_LIST);
	pMsg.h.sizeL = SET_NUMBERL(size);
	pMsg.h.sizeH = SET_NUMBERH(size);
	memcpy(&SendData[0], &pMsg, sizeof(pMsg));
	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, SendData, size);

}
void GameProtocol::GCPriceSend(CGameObject* lpObj, BYTE type, SHOP_DATA *lpShopData)
{
	if (lpObj->Type != OBJ_USER)
	{
		sLog->outError("[ERROR] Index %d not USER", lpObj);
		return;
	}

	BYTE SendData[65535];
	PMSG_PRICE_INFO pMsg;
	PMSG_PRICE_ITEM_LIST pList;
	pMsg.h.c = 0xC2;
	pMsg.h.headcode = 0xFA;
	pMsg.subc = 0x95;
	int lOfs = 0;
	lOfs += sizeof(pMsg);

	if (type == 0) // shop
	{
		if (!lpShopData)
		{
			return;
		}

		pMsg.count = lpShopData->m_ShopData.ItemCount;
		int size = lOfs + pMsg.count * sizeof(PMSG_PRICE_ITEM_LIST);
		pMsg.h.sizeL = SET_NUMBERL(size);
		pMsg.h.sizeH = SET_NUMBERH(size);
		pMsg.Type = 0;
		memcpy(SendData, &pMsg, sizeof(pMsg));

		for (int i = 0; i < 120; i++)
		{
			if (lpShopData->m_ShopData.m_item[i]->IsItem())
			{
				lpShopData->m_ShopData.m_item[i]->Value();
				pList.BuyPrice = lpShopData->m_ShopData.m_item[i]->m_BuyMoney;
				pList.SellPrice = lpShopData->m_ShopData.m_item[i]->m_SellMoney;
				pList.Pos = i;
				memcpy(&SendData[lOfs], &pList, sizeof(pList));
				lOfs += sizeof(pList);
			}
		}

		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, SendData, size);
	}

	if (type == 1) // inventory
	{
		pMsg.count = 0;
		pMsg.Type = 1;

		for (int i = 0; i < INVENTORY_SIZE; ++i)
		{
			if (lpObj->pntInventory[i]->IsItem())
			{
				lpObj->pntInventory[i]->Value();
				pList.BuyPrice = lpObj->pntInventory[i]->m_BuyMoney;
				pList.SellPrice = lpObj->pntInventory[i]->m_SellMoney;
				pList.Pos = i;
				memcpy(&SendData[lOfs], &pList, sizeof(pList));
				lOfs += sizeof(pList);
				pMsg.count++;
			}
		}

		int size = lOfs + pMsg.count * sizeof(PMSG_PRICE_ITEM_LIST);
		pMsg.h.sizeL = SET_NUMBERL(size);
		pMsg.h.sizeH = SET_NUMBERH(size);
		memcpy(&SendData[0], &pMsg, sizeof(pMsg));
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, SendData, size);
	}
}

int GameProtocol::OnCGInviteDuel(LPPMSG_REQ_DUEL_INVITE lpMsg, CGameObject* lpObj)
{
	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		return ENEWPVP::E_NOT_DUEL_CSSERVER;
	}

	int nResponsor = MAKEWORD(lpMsg->NumberL, lpMsg->NumberH);

	CGameObject* lpRequester = lpObj;
	CGameObject* lpResponser = getGameObject(nResponsor);

	if (!PacketCheckTime(lpObj)) return ENEWPVP::E_LIMIT_PACKETTIME;
	if (lpResponser == lpObj)			return ENEWPVP::E_INVITE_MYSELF;
	if (g_MaxStatsInfo.GetClass.IsNonPvP[lpObj->MapNumber])
	{
		GCServerMsgStringSend(Lang.GetText(0, 97), lpObj, 1);
		return ENEWPVP::E_NON_PKSERVER;
	}
	if (lpObj->m_PlayerData->ConnectUser->CloseType != -1)	return ENEWPVP::E_CONNECTION_CLOSING;
	if (g_NewPVP.CheckPKPenalty(*lpRequester))
	{
		GCServerMsgStringSend(Lang.GetText(0, 98), lpObj, 1);
		return ENEWPVP::E_NOT_DUEL_MURDERER;
	}
	if (!gObjIsConnectedEx(*lpResponser))
	{
		sLog->outBasic("error: %s %d (%d)", __FILE__, __LINE__, nResponsor);
		return ENEWPVP::E_DISCONNECT;
	}

	if (g_NewPVP.CheckPKPenalty(*lpResponser))
	{
		GCServerMsgStringSend(Lang.GetText(0, 99), lpObj, 1);
		return ENEWPVP::E_NOT_DUEL_MURDERER;
	}

	char szDuelName[MAX_ACCOUNT_LEN + 1] = { 0 };
	char szDuelName2[MAX_ACCOUNT_LEN + 1] = { 0 };

	memcpy(szDuelName, lpResponser->Name, MAX_ACCOUNT_LEN);
	szDuelName[MAX_ACCOUNT_LEN] = '\0';

	memcpy(szDuelName2, lpMsg->szName, MAX_ACCOUNT_LEN);
	szDuelName2[MAX_ACCOUNT_LEN] = '\0';

	if (strcmp(szDuelName, szDuelName2) != 0)
	{
		GCServerMsgStringSend(Lang.GetText(0, 85), lpObj, 1);
		return ENEWPVP::E_FAILED_RESULT;
	}
	if (!IsDuelEnable(*lpResponser))
	{
		GCServerMsgStringSend(Lang.GetText(0, 86), lpObj, 1);
		return ENEWPVP::E_FAILED_RESULT;
	}
	if (g_NewPVP.IsGuildWar(*lpRequester))
	{
		GCServerMsgStringSend(Lang.GetText(0, 87), lpObj, 1);
		return ENEWPVP::E_GUILDWAR;
	}
	if (g_NewPVP.IsGuildWar(*lpResponser))
	{
		GCServerMsgStringSend(Lang.GetText(0, 88), lpObj, 1);
		return ENEWPVP::E_GUILDWAR;
	}
	if (g_NewPVP.IsSelfDefense(*lpRequester) || g_NewPVP.IsSelfDefense(*lpResponser))
	{
		GCServerMsgStringSend(Lang.GetText(0, 108), lpObj, 1);
		return ENEWPVP::E_SELFDEFENSE;
	}
	/*if (CC_MAP_RANGE(lpRequester->MapNumber) || CC_MAP_RANGE(lpResponser->MapNumber))
	{
		GCServerMsgStringSend(Lang.GetText(0, 125), lpObj, 1);
		return ENEWPVP::E_INVALID_MAP;
	}
	if (DS_MAP_RANGE(lpRequester->MapNumber) || DS_MAP_RANGE(lpResponser->MapNumber))
	{
		GCServerMsgStringSend(Lang.GetText(0, 100), lpObj, 1);
		return ENEWPVP::E_INVALID_MAP;
	}
	if (BC_MAP_RANGE(lpRequester->MapNumber) || BC_MAP_RANGE(lpResponser->MapNumber))
	{
		GCServerMsgStringSend(Lang.GetText(0, 89), lpObj, 1);
		return ENEWPVP::E_INVALID_MAP;
	}*/

	if (IT_MAP_RANGE(lpRequester->MapNumber) || IT_MAP_RANGE(lpResponser->MapNumber))
	{
		GCServerMsgStringSend(Lang.GetText(0, 300), lpObj, 1);
		return ENEWPVP::E_INVALID_MAP;
	}
	if (DG_MAP_RANGE(lpRequester->MapNumber) || DG_MAP_RANGE(lpResponser->MapNumber))
	{
		GCServerMsgStringSend(Lang.GetText(0, 301), lpObj, 1);
		return ENEWPVP::E_INVALID_MAP;
	}
	if (IMPERIAL_MAP_RANGE(lpRequester->MapNumber) || IMPERIAL_MAP_RANGE(lpResponser->MapNumber))
	{
		GCServerMsgStringSend(Lang.GetText(0, 302), lpObj, 1);
		return ENEWPVP::E_INVALID_MAP;
	}
	if (lpRequester->m_IfState.use > 0)
	{
		GCServerMsgStringSend(Lang.GetText(0, 90), lpObj, 1);
		return ENEWPVP::E_FAILED_RESULT;
	}
	if (lpResponser->m_IfState.use > 0)
	{
		GCServerMsgStringSendEx(lpObj, 1, Lang.GetText(0, 91), lpResponser->Name);
		return ENEWPVP::E_FAILED_RESULT;
	}
	int nRet = g_NewPVP.Reserve(*lpRequester, *lpResponser);
	if (nRet != ENEWPVP::E_NO_ERROR)
	{
		switch (nRet)
		{
		case ENEWPVP::E_ALREADY_DUELRESERVED:
			GCServerMsgStringSendEx(lpObj, 1, Lang.GetText(0, 93), lpResponser->Name);
			GCServerMsgStringSend(Lang.GetText(0, 84), lpObj, 1);
			break;
		case ENEWPVP::E_ALREADY_DUELLING:
			GCServerMsgStringSend(Lang.GetText(0, 83), lpObj, 1);
			GCServerMsgStringSend(Lang.GetText(0, 84), lpObj, 1);
			break;
		case ENEWPVP::E_ALREADY_DUELREQUESTED_1:
			GCServerMsgStringSendEx(lpObj, 1, Lang.GetText(0, 92), lpResponser->Name);
			break;
		case ENEWPVP::E_ALREADY_DUELRESERVED_1:
			GCServerMsgStringSendEx(lpObj, 1, Lang.GetText(0, 93), lpResponser->Name);
			break;
		case ENEWPVP::E_ALREADY_DUELLING_1:
			GCServerMsgStringSendEx(lpObj, 1, Lang.GetText(0, 94), lpResponser->Name);
			break;
		}
		return nRet;
	}

	GCServerMsgStringSendEx(lpObj, 1, Lang.GetText(0, 95), lpResponser->Name);
	sLog->outBasic("[NewPVP] [%s][%s] Requested to Start Duel to [%s][%s]", lpRequester->m_PlayerData->ConnectUser->AccountID, lpRequester->Name, lpResponser->m_PlayerData->ConnectUser->AccountID, lpResponser->Name);
	return ENEWPVP::E_NO_ERROR;
}

int GameProtocol::OnCGAnswerDuel(LPPMSG_ANS_DUEL_ANSWER lpMsg, CGameObject* lpObj)
{
	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		return ENEWPVP::E_NOT_DUEL_CSSERVER;
	}

	int nRequester = MAKEWORD(lpMsg->NumberL, lpMsg->NumberH);
	CGameObject* lpRequester = getGameObject(nRequester);
	CGameObject* lpResponser = lpObj;


	if (!gObjIsConnectedEx(*lpRequester))	return ENEWPVP::E_DISCONNECT;

	PMSG_ANS_DUEL_INVITE res = { 0 };
	res.h.c = 0xC1;
	res.h.size = sizeof(PMSG_ANS_DUEL_INVITE);
	res.h.headcode = 0xAA;
	res.h.subcode = 0x01;
	memcpy(res.szName, lpResponser->Name, MAX_ACCOUNT_LEN);
	if (lpResponser->m_PlayerData->ConnectUser->CloseType != -1)
	{
		g_NewPVP.Cancel(*lpRequester, *lpResponser, FALSE);
		res.nResult = ENEWPVP::E_CONNECTION_CLOSING;
		GIOCP.DataSend(lpResponser->m_PlayerData->ConnectUser->Index, (LPBYTE)&res, res.h.size);
		return ENEWPVP::E_CONNECTION_CLOSING;
	}
	if (!lpMsg->bDuelOK)
	{
		g_NewPVP.Cancel(*lpRequester, *lpResponser, FALSE);
		res.nResult = ENEWPVP::E_REFUSE_INVATE;
		GIOCP.DataSend(lpRequester->m_PlayerData->ConnectUser->Index, (LPBYTE)&res, res.h.size);
		return ENEWPVP::E_REFUSE_INVATE;
	}
	if (/*BC_MAP_RANGE(lpRequester->MapNumber) ||
		CC_MAP_RANGE(lpRequester->MapNumber) ||
		DS_MAP_RANGE(lpRequester->MapNumber) ||
		IT_MAP_RANGE(lpRequester->MapNumber) ||*/
		DG_MAP_RANGE(lpRequester->MapNumber) ||
		IMPERIAL_MAP_RANGE(lpRequester->MapNumber) ||
		lpRequester->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		g_NewPVP.Cancel(*lpRequester, *lpResponser, FALSE);
		GCServerMsgStringSend(Lang.GetText(0, 125), lpResponser, 1);
		GCServerMsgStringSend(Lang.GetText(0, 125), lpRequester, 1);
		res.nResult = ENEWPVP::E_INVALID_MAP;
		GIOCP.DataSend(lpResponser->m_Index, (LPBYTE)&res, res.h.size);
		GIOCP.DataSend(lpRequester->m_Index, (LPBYTE)&res, res.h.size);
		return ENEWPVP::E_INVALID_MAP;
	}
	int nRet = g_NewPVP.Join(*lpRequester, *lpResponser);
	if (nRet != ENEWPVP::E_NO_ERROR)
	{
		switch (nRet)
		{
		case ENEWPVP::E_ALREADY_DUELRESERVED_1:
			GCServerMsgStringSend(Lang.GetText(0, 96), lpObj, 1);
			break;
		case ENEWPVP::E_ALREADY_DUELLING_1:
			GCServerMsgStringSend(Lang.GetText(0, 83), lpObj, 1);
			GCServerMsgStringSend(Lang.GetText(0, 84), lpObj, 1);
			break;
		}
		res.nResult = nRet;
		GIOCP.DataSend(lpRequester->m_Index, (LPBYTE)&res, res.h.size);
		GIOCP.DataSend(lpResponser->m_Index, (LPBYTE)&res, res.h.size);
		g_NewPVP.Cancel(*lpRequester, *lpResponser, FALSE);
		return nRet;
	}
	return ENEWPVP::E_NO_ERROR;
}

int GameProtocol::OnCGLeaveDuel(LPPMSG_REQ_DUEL_EXIT lpMsg, CGameObject* lpObj)
{
	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		return ENEWPVP::E_NOT_DUEL_CSSERVER;
	}

	if (lpObj->m_PlayerData->ConnectUser->CloseType != -1)	return ENEWPVP::E_DISCONNECT;
	int nRet = g_NewPVP.Leave(*lpObj);
	if (nRet != ENEWPVP::E_NO_ERROR)	return nRet;
	return ENEWPVP::E_NO_ERROR;
}

int GameProtocol::OnDuelChannelJoin(LPPMSG_REQ_DUEL_JOINCNANNEL lpMsg, CGameObject* lpObj)
{
	if (lpObj->m_PlayerData->ConnectUser->CloseType != -1)	return ENEWPVP::E_DISCONNECT;
	int nRet = g_NewPVP.JoinChannel(lpMsg->nChannelId, *lpObj);
	if (nRet != ENEWPVP::E_NO_ERROR)	return nRet;
	return ENEWPVP::E_NO_ERROR;
}

int GameProtocol::OnDuelChannelLeave(LPPMSG_REQ_DUEL_LEAVECNANNEL lpMsg, CGameObject* lpObj)
{
	if (lpObj->m_PlayerData->ConnectUser->CloseType != -1)	return ENEWPVP::E_DISCONNECT;
	int nRet = g_NewPVP.LeaveChannel(lpMsg->nChannelId, *lpObj);
	if (nRet != ENEWPVP::E_NO_ERROR)	return nRet;
	return ENEWPVP::E_NO_ERROR;
}

/*
void GameProtocol::CGReqRegGensMember(PMSG_REQ_REG_GENS_MEMBER *lpMsg, CGameObject *lpObj)
{
	unsigned char bRet;

	LPOBJ lpObj = &gObj[iIndex];

	_GUILD_INFO_STRUCT *lpGuildInfo = lpObj->m_PlayerData->lpGuild;

	if (!ObjectMaxRange(lpObj)) return;

	if (gObjIsConnectedGP(lpObj))
	{
		PMSG_ANS_REG_GENS_MEMBER_EXDB pMsg;

		pMsg.bInfluence = lpMsg->bResult;

		//[GensSystem] [%s][%s] Gens Request - Join [%s]
		if (!g_GensSystem.GetGensInfluence(lpObj))
		{
			pMsg.bResult = 0; bRet = 1;
		}

		//GensSystem] [%s][%s] Gens Joining Failed to Gens be Same : [%s]
		if (g_GensSystem.GetGensInfluence(lpObj) > 0)
		{
			pMsg.bResult = 1; bRet = 0;
		}

		//[GensSystem] [%s][%s] Gens Joining Failed to Below Gens leaving Time
		//if (g_GensAbusingResetTime < 60)
		//{
		//	pMsg.bResult = 2; bRet = 1;
		//}

		//[GensSystem] [%s][%s] Gens Joining Failed to Below 50Lv
		if (lpObj->Level < 50)
		{
			pMsg.bResult = 3; bRet = 0;
		}

		//[GensSystem] [%s][%s] Gens Joining Failed to Gens be Different Guild Leader [%s]
		if (lpObj->m_PlayerData->GuildStatus >= 0)
		{
			pMsg.bResult = 4; bRet = 0;
		}

		//[GensSystem] [%s][%s] Gens Joining Failed to Not Register Guild Leader GuildName [%s]
		if (lpObj->m_PlayerData->GuildStatus == GUILD_MASTER)
		{
			pMsg.bResult = 5; bRet = 0;
		}

		//[GensSystem] [%s][%s] Gens Joining Failed to Already Partymember
		if (lpObj->PartyNumber >= 0)
		{
			pMsg.bResult = 6; bRet = 0;
		}

		//[GensSystem] [%s][%s] Gens Joining Failed to Union GuildMaster
		if (lpGuildInfo != NULL && lpGuildInfo->iGuildUnion)
		{
			pMsg.bResult = 7; bRet = 0;
		}

		if (lpObj->m_PlayerData->m_bUsePartyMatching)
		{
			pMsg.bResult = 8; bRet = 0;
		}

		if (bRet)
		{
			g_GensSystem.ReqRegGensMember(lpObj, lpMsg->bResult);
		}
		else
		{
			pMsg.bIndexH = SET_NUMBERH(lpObj);
			pMsg.bIndexL = SET_NUMBERL(lpObj);
			DGAnsRegGensMember((PMSG_ANS_REG_GENS_MEMBER_EXDB*)&pMsg);
		}
	}
}
*/
/*
//-> Completed
void GameProtocol::CGReqSecedeGensMember(PMSG_REQ_SEGEDE_GENS_MEMBER *lpMsg, CGameObject *lpObj)
{
	if (!ObjectMaxRange(lpObj))
		return;

	LPOBJ lpObj = &gObj[iIndex];

	unsigned char bRet = 1;

	if (gObjIsConnectedGP(lpObj))
	{
		PMSG_ANS_SECEDE_GENS_MEMBER_EXDB pMsg;

		if (!g_GensSystem.IsInfluenceNPC(lpObj))
		{
			pMsg.bResult = 3; bRet = 0;
		}

		else if (lpObj->m_PlayerData->GuildStatus == GUILD_MASTER)
		{
			pMsg.bResult = 2; bRet = 0;
		}

		//[GensSystem] [%s][%s] Gens Leaving Failed to Not Register Gens
		else if (!g_GensSystem.GetGensInfluence(lpObj))
		{
			pMsg.bResult = 1; bRet = 0;
		}

		else if (lpObj->m_PlayerData->m_bUsePartyMatching)
		{
			pMsg.bResult = 4; bRet = 0;
		}

		else
		{
			pMsg.bResult = 0; bRet = 1;
		}

		if (bRet)
		{
			g_GensSystem.ReqSecedeGensMember(lpObj);
		}

		else
		{
			pMsg.bIndexH = SET_NUMBERH(lpObj);
			pMsg.bIndexL = SET_NUMBERL(lpObj);
			DGAnsSecedeGensMember((PMSG_ANS_SECEDE_GENS_MEMBER_EXDB*)&pMsg);
		}
	}
}
*/
/*
//-> Completed
void GameProtocol::CGReqGensReward(PMSG_GENS_REWARD_CODE *lpMsg, CGameObject *lpObj)
{
	if (!ObjectMaxRange(lpObj))
		return;

	unsigned char bRet = 0; //Not Used

	LPOBJ lpObj = &gObj[iIndex];

	if (gObjIsConnectedGP(lpObj))
	{
		g_GensSystem.ReqExDBGensRewardCheck(lpObj, lpMsg->bReward);
	}
}

//-> Completed
void GameProtocol::CGReqGensMemberInfo(PMSG_REQ_GENS_INFO *lpMsg, CGameObject *lpObj)
{
	if (!ObjectMaxRange(lpObj)) return;

	LPOBJ lpObj = &gObj[iIndex];

	if (gObjIsConnectedGP(lpObj))
	{
		if (g_GensSystem.IsRegGensInfluence(lpObj))
		{
			g_GensSystem.ReqExDBGensInfo(lpObj);
		}
	}
}
*/

void GameProtocol::CGInventoryEquipment(PMSG_REQ_INVENTORY_EQUIPMENT_ITEM *lpMsg, CGameObject *lpObj)
{
	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	PMSG_ANS_INVENTORY_EQUIPMENT_ITEM pMsg = { 0 };

	pMsg.btResult = 0;
	PHeadSubSetB((LPBYTE)&pMsg, 0xBF, 0x20, sizeof(pMsg));

	pMsg.btItemPos = lpMsg->btItemPos;

	if (!IsInvenPet(lpObj->pntInventory[lpMsg->btItemPos]->m_Type) &&
		lpObj->pntInventory[lpMsg->btItemPos]->m_Durability == 0.0)
	{
		pMsg.btResult = -1;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	switch (lpObj->pntInventory[lpMsg->btItemPos]->m_Type)
	{
	case ITEMGET(13, 128):
	case ITEMGET(13, 129):
		if (lpObj->m_PlayerData->m_btSculptPos == 0)
		{
			if (lpMsg->btValue == (BYTE)-2 && lpObj->pntInventory[lpMsg->btItemPos]->m_Durability == 255.0)
			{
				lpObj->m_PlayerData->m_btSculptPos = lpMsg->btItemPos;
				lpObj->pntInventory[lpMsg->btItemPos]->m_Durability = 254.0;
				pMsg.btResult = -2;
			}

			goto DEF_SWITCH;
		}

		if (lpObj->m_PlayerData->m_btSculptPos < INVETORY_WEAR_SIZE || lpObj->m_PlayerData->m_btSculptPos >= MAIN_INVENTORY_SIZE)
		{
			goto DEF_SWITCH;
		}

		if (lpMsg->btValue == (BYTE)-1 && lpObj->m_PlayerData->m_btSculptPos == lpMsg->btItemPos)
		{
			if (lpObj->pntInventory[lpMsg->btItemPos]->m_Durability == 254.0)
			{
				lpObj->pntInventory[lpMsg->btItemPos]->m_Durability = 255.0;
				pMsg.btResult = -1;
			}

			goto DEF_SWITCH;
		}

		this->GCServerMsgStringSend(Lang.GetText(0, 548), lpObj, 1);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		break;
	case ITEMGET(13, 130):
	case ITEMGET(13, 131):
	case ITEMGET(13, 132):
	case ITEMGET(13, 133):
		if (lpObj->m_PlayerData->m_btCharmPos == 0)
		{
			if (lpMsg->btValue == (BYTE)-2 && lpObj->pntInventory[lpMsg->btItemPos]->m_Durability == 255.0)
			{
				lpObj->m_PlayerData->m_btCharmPos = lpMsg->btItemPos;
				lpObj->pntInventory[lpMsg->btItemPos]->m_Durability = 254.0;
				pMsg.btResult = -2;
			}

			goto DEF_SWITCH;
		}

		if (lpObj->m_PlayerData->m_btCharmPos < INVETORY_WEAR_SIZE || lpObj->m_PlayerData->m_btCharmPos >= MAIN_INVENTORY_SIZE)
		{
			goto DEF_SWITCH;
		}

		if (lpMsg->btValue == (BYTE)-1 && lpObj->m_PlayerData->m_btCharmPos == lpMsg->btItemPos)
		{
			if (lpObj->pntInventory[lpMsg->btItemPos]->m_Durability == 254.0)
			{
				lpObj->pntInventory[lpMsg->btItemPos]->m_Durability = 255.0;
				pMsg.btResult = -1;
			}

			goto DEF_SWITCH;
		}

		this->GCServerMsgStringSend(Lang.GetText(0, 548), lpObj, 1);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		break;
	case ITEMGET(13, 134):
		if (lpObj->m_PlayerData->m_btArtifactPos == 0)
		{
			if (lpMsg->btValue == (BYTE)-2 && lpObj->pntInventory[lpMsg->btItemPos]->m_Durability == 255.0)
			{
				lpObj->m_PlayerData->m_btArtifactPos = lpMsg->btItemPos;
				lpObj->pntInventory[lpMsg->btItemPos]->m_Durability = 254.0;
				pMsg.btResult = -2;
			}

			goto DEF_SWITCH;
		}

		if (lpObj->m_PlayerData->m_btArtifactPos < INVETORY_WEAR_SIZE || lpObj->m_PlayerData->m_btArtifactPos >= MAIN_INVENTORY_SIZE)
		{
			goto DEF_SWITCH;
		}

		if (lpMsg->btValue == (BYTE)-1 && lpObj->m_PlayerData->m_btArtifactPos == lpMsg->btItemPos)
		{
			if (lpObj->pntInventory[lpMsg->btItemPos]->m_Durability == 254.0)
			{
				lpObj->pntInventory[lpMsg->btItemPos]->m_Durability = 255.0;
				pMsg.btResult = -1;
			}

			goto DEF_SWITCH;
		}

		this->GCServerMsgStringSend(Lang.GetText(0, 548), lpObj, 1);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		break;
	case ITEMGET(13, 177):
	case ITEMGET(13, 178):
	case ITEMGET(13, 179):
		if (lpObj->m_PlayerData->m_btExpUpCharmPos == 0)
		{
			if (lpMsg->btValue == (BYTE)-2 && lpObj->pntInventory[lpMsg->btItemPos]->m_Durability == 255.0)
			{
				lpObj->m_PlayerData->m_btExpUpCharmPos = lpMsg->btItemPos;
				lpObj->pntInventory[lpMsg->btItemPos]->m_Durability = 254.0;
				pMsg.btResult = -2;
			}

			goto DEF_SWITCH;
		}

		if (lpObj->m_PlayerData->m_btExpUpCharmPos < INVETORY_WEAR_SIZE || lpObj->m_PlayerData->m_btExpUpCharmPos >= MAIN_INVENTORY_SIZE)
		{
			goto DEF_SWITCH;
		}

		if (lpMsg->btValue == (BYTE)-1 && lpObj->m_PlayerData->m_btExpUpCharmPos == lpMsg->btItemPos)
		{
			if (lpObj->pntInventory[lpMsg->btItemPos]->m_Durability == 254.0)
			{
				lpObj->pntInventory[lpMsg->btItemPos]->m_Durability = 255.0;
				pMsg.btResult = -1;
			}

			goto DEF_SWITCH;
		}

		this->GCServerMsgStringSend(Lang.GetText(0, 548), lpObj, 1);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		break;
	case ITEMGET(13, 2):
	case ITEMGET(13, 3):
	case ITEMGET(13, 4):
	case ITEMGET(13, 5):
	case ITEMGET(13, 37):
		if (lpObj->m_btInvenPetPos != 0)
		{
			if (lpObj->m_btInvenPetPos >= INVETORY_WEAR_SIZE && lpObj->m_btInvenPetPos < MAIN_INVENTORY_SIZE)
			{
				if (lpMsg->btValue != (BYTE)-1 || lpObj->m_btInvenPetPos != lpMsg->btItemPos)
				{
					this->GCServerMsgStringSend(Lang.GetText(0, 615), lpObj, 1);
					return;
				}

				if (gObjUseInvenPetCheck(*lpObj, lpObj->pntInventory[lpMsg->btItemPos], 0) == FALSE)
				{
					this->GCServerMsgStringSend(Lang.GetText(0, 616), lpObj, 1);
					return;
				}

				if (lpObj->pntInventory[lpMsg->btItemPos]->m_JewelOfHarmonyOption == 1)
				{

					lpObj->pntInventory[lpMsg->btItemPos]->m_JewelOfHarmonyOption = 0;
					lpObj->m_wInvenPet = -1;
					pMsg.btResult = -1;
				}
			}
		}

		else if (lpMsg->btValue == (BYTE)-2)
		{

			if (lpObj->m_wInvenPet != (WORD)-1 || lpObj->pntInventory[lpMsg->btItemPos]->m_JewelOfHarmonyOption == 1)
			{

				this->GCServerMsgStringSend(Lang.GetText(0, 615), lpObj, 1);
				return;
			}

			if (gObjUseInvenPetCheck(*lpObj, lpObj->pntInventory[lpMsg->btItemPos], 1) == FALSE)
			{
				this->GCServerMsgStringSend(Lang.GetText(0, 617), lpObj, 1);
				return;
			}

			lpObj->m_btInvenPetPos = lpMsg->btItemPos;
			lpObj->pntInventory[lpMsg->btItemPos]->m_JewelOfHarmonyOption = 1;
			pMsg.btResult = (BYTE)-2;

		}

		gObjMakePreviewCharSet(*lpObj);
		this->GCEquipmentChange(lpObj, lpMsg->btItemPos);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		break;
	default:
	DEF_SWITCH:
		gObjCalCharacter.CalcCharacter(*lpObj);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		break;
	}
}

void GameProtocol::CGReqUseTransformationRing(PMSG_REQ_USETRANSFORMATIONRING * lpMsg, CGameObject* lpObj)
{

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	if (lpMsg->btState == 1)
	{
		if (IsTransformCharacterSkin(lpObj->m_Change) == TRUE)
		{
			lpObj->m_PlayerData->m_EnableUseChangeSkin = 0;
		}
	}

	else
	{
		lpObj->m_PlayerData->m_EnableUseChangeSkin = 1;
	}

	PMSG_MOVE pMove;

	pMove.h.c = 0xC1;
	pMove.h.headcode = 0xD7;
	pMove.h.size = sizeof(pMove);
	pMove.X = lpObj->X;
	pMove.Y = lpObj->Y;

	PMoveProc(&pMove, lpObj);
	gObjViewportListProtocolCreate(*lpObj);
}

void GameProtocol::GCSendAttackSpeed(CGameObject* lpObj)
{
	PMSG_ATTACKSPEEDSEND pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0xEC, 0x30, sizeof(pMsg));

	pMsg.AttackSpeed = lpObj->m_AttackSpeed;

	pMsg.MagicSpeed = lpObj->m_MagicSpeed;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::CGReqArcaBattleGuildMasterJoin(PMSG_REQ_ARCA_BATTLE_JOIN *lpMsg, CGameObject *lpObj)
{

	if (gObjIsConnected(*lpObj) == false)
	{
		return;
	}

	GUILD_INFO_STRUCT * lpGuildInfo = lpObj->m_PlayerData->lpGuild;
	PMSG_ANS_ARCA_BATTLE_GUILD_JOIN_DS pMsg;

	pMsg.wNumber = lpObj->m_Index;

	if (!lpGuildInfo)
	{
		pMsg.btResult = 1;
		DGAnsArcaBattleGuildJoinSelect((PMSG_ANS_ARCA_BATTLE_GUILD_JOIN_DS *)&pMsg);
		return;
	}

	int iArcaBattleState = g_ArcaBattle.GetState();

	if (iArcaBattleState != 3)
	{
		pMsg.btResult = 5;
		DGAnsArcaBattleGuildJoinSelect((PMSG_ANS_ARCA_BATTLE_GUILD_JOIN_DS *)&pMsg);
		return;
	}

	if (lpObj->m_PlayerData->GuildStatus != GUILD_MASTER)
	{
		pMsg.btResult = 1;
		DGAnsArcaBattleGuildJoinSelect((PMSG_ANS_ARCA_BATTLE_GUILD_JOIN_DS *)&pMsg);
		return;
	}

	if (lpGuildInfo->Count < g_ArcaBattle.GetJoinMemberCnt())
	{
		pMsg.btResult = 2;
		DGAnsArcaBattleGuildJoinSelect((PMSG_ANS_ARCA_BATTLE_GUILD_JOIN_DS *)&pMsg);
		return;
	}

	g_ArcaBattle.GDReqArcaBattleIsTopRank(*lpObj, lpObj->m_PlayerData->GuildNumber);
}

void GameProtocol::CGReqArcaBattleGuildMemberJoin(PMSG_REQ_ARCA_BATTLE_JOIN *lpMsg, CGameObject *lpObj)
{
	PMSG_ANS_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS pMsg;

	pMsg.wNumber = lpObj->m_Index;

	if (lpObj->m_PlayerData->lpGuild == NULL)
	{
		pMsg.btResult = 7;
		DGAnsArcaBattleGuildMemberJoin((PMSG_ANS_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS *)&pMsg);
		return;
	}

	if (lpObj->m_PlayerData->GuildStatus == GUILD_MASTER)
	{
		pMsg.btResult = 13;
		DGAnsArcaBattleGuildMemberJoin((PMSG_ANS_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS *)&pMsg);
		return;
	}

	int iArcaBattleState = g_ArcaBattle.GetState();

	if (iArcaBattleState < 4)
	{
		pMsg.btResult = 14;
		DGAnsArcaBattleGuildMemberJoin((PMSG_ANS_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS *)&pMsg);
		return;
	}

	if (iArcaBattleState >= 7)
	{
		pMsg.btResult = 10;
		DGAnsArcaBattleGuildMemberJoin((PMSG_ANS_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS *)&pMsg);
		return;
	}

	GDReqArcaBattleGuildMemberJoin(lpObj);
}

void GameProtocol::CGReqArcaBattleEnter(PMSG_REQ_ARCA_BATTLE_ENTER *lpMsg, CGameObject *lpObj)
{
	PMSG_ANS_ARCA_BATTLE_ENTER_DS pMsg;

	if (gObjIsConnected(*lpObj))
	{
		int iArcaBattleState = g_ArcaBattle.GetState();

		if (iArcaBattleState == 6 || iArcaBattleState == 7)
		{
			if (lpObj->m_PlayerData->m_bUsePartyMatching)
			{
				PMSG_ANS_PARTYMATCHING_ERROR pMsg;
				PHeadSubSetB((LPBYTE)&pMsg, 0xEF, 0x09, sizeof(pMsg));
				pMsg.iResult = -1;

				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
				return;
			}

			if (lpObj->Teleport)
			{
				return;
			}

			BYTE btAttr = MapC[lpObj->MapNumber].GetAttr(lpObj->X, lpObj->Y);

			if (btAttr & 1)
				GDReqArcaBattleEnter(lpObj, lpMsg->btEnterSeq);
		}

		else
		{
			pMsg.wNumber = lpObj->m_Index;
			pMsg.btResult = 12;
			DGAnsArcaBattleEnter((PMSG_ANS_ARCA_BATTLE_ENTER_DS *)&pMsg);
		}
	}
}

void GameProtocol::CGReqAcheronEnter(PMSG_REQ_ACHERON_ENTER *lpMsg, CGameObject *lpObj)
{
	PMSG_ANS_ACHERON_ENTER pMsg;

	for (int pos = INVETORY_WEAR_SIZE; pos < MAIN_INVENTORY_SIZE; pos++)
	{
		if (lpObj->pntInventory[pos]->IsItem() && lpObj->pntInventory[pos]->m_Type == ITEMGET(13, 146))
		{
			gObjInventoryDeleteItem(*lpObj, pos);
			this->GCInventoryItemDeleteSend(lpObj, pos, 1);
			gObjMoveGate(*lpObj, 417);
			return;
		}
	}

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xF8;
	pMsg.h.subcode = 0x21;
	pMsg.btResult = 1;
	GIOCP.DataSend(lpObj->m_Index, (LPBYTE)&pMsg, sizeof(pMsg));
}

void GameProtocol::CGReqArcaBattleBootyExchange(CGameObject *lpObj)
{

	if (bCanChaosBox == TRUE && !lpObj->m_IfState.use)
	{
		if (lpObj->m_bPShopOpen == TRUE)
		{
			sLog->outBasic("[%s][%s] is Already Opening PShop, ChaosBox Failed", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
			this->GCServerMsgStringSend(Lang.GetText(0, 531), lpObj, 1);
			return;
		}

		lpObj->m_IfState.use = 1;
		lpObj->m_IfState.type = 7;
		gObjInventoryTrans(*lpObj);
		sLog->outBasic("[ArcaBattle][BootyItemMix] [%s][%s]  Booty Item Mix Start", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
		gObjItemTextSave(*lpObj);
		gObjWarehouseTextSave(*lpObj);
	}
}

void GameProtocol::CGReqSpritemapExchange(CGameObject *lpObj)
{

	if (gObjIsConnected(*lpObj))
	{
		if (bCanChaosBox == 1 && !(lpObj->m_IfState.type & 3))
		{
			if (lpObj->m_bPShopOpen == 1)
			{
				sLog->outBasic("[%s][%s] is Already Opening PShop, ChaosBox Failed", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
				this->GCServerMsgStringSend(Lang.GetText(0, 531), lpObj, 1);
			}

			else
			{
				lpObj->m_IfState.use = 1;
				lpObj->m_IfState.type = 7;

				gObjInventoryTrans(*lpObj);
				sLog->outBasic("[ArcaBattle][BootyItemMix] [%s][%s]  Booty Item Mix Start", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
				gObjItemTextSave(*lpObj);
				gObjWarehouseTextSave(*lpObj);
			}
		}
	}
}


void GameProtocol::CGReqRegisteredMemberCnt(CGameObject* lpObj)
{
	PMSG_REQ_AB_REG_MEMBER_CNT_DS pMsg;
	PMSG_ANS_AB_REG_MEMBER_CNT pAnsMsg;

	pAnsMsg.h.c = 0xC1;
	pAnsMsg.h.size = sizeof(pMsg);
	pAnsMsg.h.headcode = 0xF8;
	pAnsMsg.h.subcode = 0x42;
	pAnsMsg.btRegMemCnt = 0;

	if (lpObj->m_PlayerData->lpGuild)
	{
		pMsg.h.c = 0xC1;
		pMsg.h.size = sizeof(pMsg);
		pMsg.h.headcode = 0xF8;
		pMsg.h.subcode = 0x45;
		pMsg.iIndex = lpObj->m_Index;
		pMsg.iGuildNumber = lpObj->m_PlayerData->GuildNumber;
		//wsDataCli.DataSend((char*)&pMsg, sizeof(pMsg)); // TODO
	}

	else
	{
		pAnsMsg.btResult = 1;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pAnsMsg, sizeof(pMsg));
	}
}

void GameProtocol::CGReqEnterDoppelGanger(PMSG_REQ_ENTER_DOPPELGANGER *lpMsg, CGameObject *lpObj)
{
	//g_DoppelGanger.EnterDoppelgangerEvent(*lpObj, lpMsg->btPos);
}

void GameProtocol::CGReqEnterZone(PMSG_REQ_ENTER_ZONE * lpMsg, CGameObject *lpObj)
{
	if (lpMsg->btResult == TRUE)
	{
		g_ImperialGuardian.CGEnterPortal(*lpObj, 0);
	}
}


void GameProtocol::GCSendRecvCheck(CGameObject* lpObj)
{
	PMSG_RECV_TRAP pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xFA, 0x9F, sizeof(pMsg));

	pMsg.wUserIndex = lpObj->m_Index;
	pMsg.CurrTime = time(NULL);

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
}

void GameProtocol::CGReqAntihackBreach(CGameObject* lpObj, PMSG_ANTIHACK_BREACH *lpMsg)
{

	sLog->outError("[%s][%s][%s][%s] AntiHack breach -> Code:%d",
		lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->ConnectUser->IP, lpObj->m_PlayerData->ConnectUser->HWID, lpMsg->dwErrorCode);

	if (g_ConfigRead.antihack.AntiHackBreachDisconnectUser == true)
	{
		//this->GCSendDisableReconnect(lpObj);
		GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
	}
}

void GameProtocol::CGReqAntihackCheck(CGameObject* lpObj, PMSG_ANTIHACK_CHECK *lpMsg)
{

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	BYTE MainRecvHeader[] = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC };

	if (memcmp(lpMsg->checkdata, MainRecvHeader, 5) != 0 && g_ConfigRead.antihack.AntiHackRecvHookProtect == true) // data is wrong, recv is probably hooked
	{
		sLog->outError("[%s][%s][%s][%s] AntiHack breach -> Recv header data is wrong",
			lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_PlayerData->ConnectUser->IP, lpObj->m_PlayerData->ConnectUser->HWID);

		this->GCSendDisableReconnect(lpObj);
		//this->GCServerMsgStringSendEx(lpObj, 1, Lang.GetText(0, 547));
		gObjCloseSet(*lpObj, 0);
	}

	else
	{
		lpObj->m_PlayerData->AntiHackCheckTime = GetTickCount();
	}
}

void GameProtocol::CGReqInJewelPentagramItem(PMSG_REQ_IN_PENTAGRAM_JEWEL *lpMsg, CGameObject* lpObj)
{
	if (!lpMsg)
	{
		return;
	}

	if (!g_PentagramSystem.PentagramJewel_IN(*lpObj, lpMsg->iPentagramPos, lpMsg->iJewelPos))
		this->GCAnsInJewelPentagramItem(lpObj, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

void GameProtocol::GCAnsInJewelPentagramItem(CGameObject* lpObj, int iResult, BYTE btJewelPos, BYTE btJewelIndex, BYTE btItemType, WORD wItemIndex, BYTE btMainAttribute, BYTE btLevel, BYTE btRank1OptionNum, BYTE btRank1Level, BYTE btRank2OptionNum, BYTE btRank2Level, BYTE btRank3OptionNum, BYTE btRank3Level, BYTE btRank4OptionNum, BYTE btRank4Level, BYTE btRank5OptionNum, BYTE btRank5Level)
{
	PMSG_ANS_IN_PENTAGRAM_JEWEL pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xEC, 0x00, sizeof(pMsg));

	pMsg.btResult = iResult;
	pMsg.btJewelPos = btJewelPos;
	pMsg.btJewelIndex = btJewelIndex;
	pMsg.btItemType = btItemType;
	pMsg.wItemIndex = wItemIndex;
	pMsg.btMainAttribute = btMainAttribute;
	pMsg.btLevel = btLevel;
	pMsg.btRank1OptionNum = btRank1OptionNum;
	pMsg.btRank1Level = btRank1Level;
	pMsg.btRank2OptionNum = btRank2OptionNum;
	pMsg.btRank2Level = btRank2Level;
	pMsg.btRank3OptionNum = btRank3OptionNum;
	pMsg.btRank3Level = btRank3Level;
	pMsg.btRank4OptionNum = btRank4OptionNum;
	pMsg.btRank4Level = btRank4Level;
	pMsg.btRank5OptionNum = btRank5OptionNum;
	pMsg.btRank5Level = btRank5Level;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));
}

void GameProtocol::CGReqOutJewelPentagramItem(PMSG_REQ_OUT_PENTAGRAM_JEWEL *lpMsg, CGameObject* lpObj)
{
	if (!lpMsg)
	{
		return;
	}

	BYTE btJewelPos;
	BYTE btJewelDBIndex;

	int iResult = g_PentagramSystem.PentagramJewel_OUT(*lpObj, lpMsg->iPentagramPos, lpMsg->btSocketIndex, &btJewelPos, &btJewelDBIndex);
	this->GCAnsOutJewelPentagramItem(lpObj, iResult, btJewelPos, btJewelDBIndex);

	if (iResult != 1)
	{
		this->GCAnsPentagramJewelInOut(lpObj, iResult);
	}
}

void GameProtocol::GCAnsOutJewelPentagramItem(CGameObject *lpObj, int iResult, BYTE btJewelPos, BYTE btJewelDBIndex)
{
	PMSG_ANS_OUT_PENTAGRAM_JEWEL pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xEC, 0x01, sizeof(pMsg));

	pMsg.Result = iResult;
	pMsg.btJewelPos = btJewelPos;
	pMsg.btJewelDBIndex = btJewelDBIndex;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
}

void GameProtocol::CGReqRefinePentagramJewel(PMSG_REQ_REFINE_PENTAGRAM_JEWEL *lpMsg, CGameObject* lpObj)
{
	if (!lpMsg)
	{
		return;
	}

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	lpObj->m_PlayerData->m_bIsPentagramMixCompleted = true;
	BYTE btRefineResult = g_PentagramMixSystem.PentagramJewelRefine(*lpObj, lpMsg->btRefineKind);

	if (btRefineResult != 1)
	{
		this->GCAnsRefinePentagramJewel(lpObj, btRefineResult);
	}
}

void GameProtocol::GCAnsRefinePentagramJewel(CGameObject *lpObj, BYTE btResult)
{
	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((LPBYTE)&pMsg, 0x86, sizeof(pMsg));

	pMsg.Result = btResult;
	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
}

void GameProtocol::CGReqUpgradePentagramJewel(PMSG_REQ_UPGRADE_PENTAGRAM_JEWEL *lpMsg, CGameObject* lpObj)
{
	if (!lpMsg)
	{
		return;
	}

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	lpObj->m_PlayerData->m_bIsPentagramMixCompleted = true;
	BYTE btUpgradeResult = g_PentagramMixSystem.PentagramJewel_Upgrade(*lpObj, lpMsg->btUpgradeType, lpMsg->btTargetValue);

	if (btUpgradeResult != 1)
	{
		this->GCAnsUpgradePentagramJewel(lpObj, btUpgradeResult);
	}
}

void GameProtocol::GCAnsUpgradePentagramJewel(CGameObject *lpObj, BYTE btResult)
{
	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((LPBYTE)&pMsg, 0x86, sizeof(pMsg));

	pMsg.Result = btResult;
	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
}

void GameProtocol::GCAnsPentagramJewelInOut(CGameObject *lpObj, BYTE btResult)
{
	PMSG_PENTAGRAM_JEWEL_INOUT pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xEC, 0x04, sizeof(pMsg));

	pMsg.btResult = btResult;
	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
}

/*
void GameProtocol::CGReqGuildMatchingList(PMSG_REQ_GUILDMATCHINGLIST *lpMsg, CGameObject* lpObj)
{
	GDReqGuildMatchingList(lpObj->m_Index, lpMsg->nPage);
}

void GameProtocol::CGReqGuildMatchingListSearchWord(PMSG_REQ_GUILDMATCHINGLISTSEARCHWORD *lpMsg, CGameObject* lpObj)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	int nPage = lpMsg->nPage;
	char szSearchWord[11];

	memcpy(szSearchWord, lpMsg->szSearchWord, 11);
	GDReqGuildMatchingListSearchWord(nUserIndex, nPage, szSearchWord);
}

void GameProtocol::CGReqRegGuildMatchingList(PMSG_REQ_REGGUILDMATCHINGDATA *lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	LPOBJ lpObj = &gObj[nUserIndex];

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	int nGuildNumber = lpObj->m_PlayerData->GuildNumber;
	_GUILD_INFO_STRUCT * lpstGuildInfo = Guild.SearchGuild_Number(nGuildNumber);
	_stGuildMatchingList stGuildMatchingList;

	PMSG_ANS_REGGUILDMATCHINGDATA pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xED, 0x02, sizeof(pMsg));

	if (!lpstGuildInfo || Guild.GetGuildMemberStatus(lpstGuildInfo->Name, lpObj->Name) != GUILD_MASTER)
	{
		pMsg.nResult = -4;
		GIOCP.DataSend(nUserIndex, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	if (lpObj->m_PK_Level > 3)
	{
		pMsg.nResult = -2;
		GIOCP.DataSend(nUserIndex, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	memcpy(stGuildMatchingList.szMemo, lpMsg->szMemo, 41);
	stGuildMatchingList.btInterestType = lpMsg->btInterestType;
	stGuildMatchingList.btLevelRange = lpMsg->btLevelRange;
	stGuildMatchingList.btClassType = lpMsg->btClassType;
	stGuildMatchingList.btGuildMasterClass = lpObj->Class;

	if (lpObj->m_PlayerData->ChangeUP == 1)
	{
		stGuildMatchingList.btGuildMasterClass |= 0x08;
	}

	if (lpObj->m_PlayerData->ChangeUP == 2)
	{
		stGuildMatchingList.btGuildMasterClass |= CS_SET_CHANGEUP_SX;
		stGuildMatchingList.btGuildMasterClass |= CS_SET_CHANGEUP2_SX;
		//stAllowList.btApplicantClass |= 0x10;
	}

	memcpy(stGuildMatchingList.szGuildName, lpObj->m_PlayerData->GuildName, MAX_GUILD_LEN + 1);
	memcpy(stGuildMatchingList.szRegistrant, lpObj->Name, MAX_ACCOUNT_LEN + 1);
	stGuildMatchingList.btGuildMemberCnt = lpstGuildInfo->Count;
	stGuildMatchingList.nGuildNumber = lpstGuildInfo->Number;
	stGuildMatchingList.nGuildMasterLevel = lpObj->Level + lpObj->m_PlayerData->MasterLevel;
	stGuildMatchingList.btGensType = g_GensSystem.GetGensInfluence(lpObj);

	GDReqRegGuildMatchingList(nUserIndex, stGuildMatchingList);
}

void GameProtocol::CGReqCancelGuildMatchingList(PMSG_REQ_CANCEL_GUILDMATCHINGLIST * lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	LPOBJ lpObj = &gObj[nUserIndex];

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	int nGuildNumber = lpObj->m_PlayerData->GuildNumber;
	_GUILD_INFO_STRUCT * lpstGuildInfo = Guild.SearchGuild_Number(nGuildNumber);

	PMSG_ANS_CANCEL_GUILDMATCHINGLIST pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xED, 0x03, sizeof(pMsg));

	if (!lpstGuildInfo || Guild.GetGuildMemberStatus(lpstGuildInfo->Name, lpObj->Name) != GUILD_MASTER)
	{
		pMsg.nResult = -2;
		GIOCP.DataSend(nUserIndex, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	GDReqDelMatchingList(nUserIndex, nGuildNumber);
}

void GameProtocol::CGReqJoinGuildMatching(PMSG_REQ_JOIN_GUILDMATCHING *lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	LPOBJ lpObj = &gObj[nUserIndex];

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	int nGuildNumber = lpObj->m_PlayerData->GuildNumber;
	int nTargetGuildNumber = lpMsg->nGuildNumber;

	PMSG_ANS_JOIN_GUILDMATCHING pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xED, 0x04, sizeof(pMsg));

	if (nGuildNumber > 0)
	{
		pMsg.nResult = -3;
		GIOCP.DataSend(nUserIndex, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	if (gObj[nUserIndex].m_PK_Level > 3)
	{
		pMsg.nResult = -2;
		GIOCP.DataSend(nUserIndex, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	_stGuildMatchingAllowListDB stAllowList;

	stAllowList.nGuildNumber = nTargetGuildNumber;
	memcpy(stAllowList.szApplicantName, lpObj->Name, MAX_ACCOUNT_LEN + 1);
	stAllowList.btApplicantClass = lpObj->Class;
	stAllowList.btState = 0;
	stAllowList.nApplicantLevel = lpObj->Level + lpObj->m_PlayerData->MasterLevel;

	if (lpObj->m_PlayerData->ChangeUP == 1)
	{
		stAllowList.btApplicantClass |= 0x08;
	}

	else if (lpObj->m_PlayerData->ChangeUP == 2)
	{
		stAllowList.btApplicantClass |= CS_SET_CHANGEUP_SX;
		stAllowList.btApplicantClass |= CS_SET_CHANGEUP2_SX;
		//stAllowList.btApplicantClass |= 0x10;
	}

	GDReqJoinGuildMatchingList(nUserIndex, stAllowList);
}

void GameProtocol::CGReqCancelJoinGuildMatching(int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	LPOBJ lpObj = &gObj[nUserIndex];

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	GDReqCancelJoinGuildMatching(nUserIndex, lpObj->Name, 0);
}

void GameProtocol::CGReqAllowJoinGuildMatching(PMSG_REQ_ALLOW_JOIN_GUILDMATCHING *lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	LPOBJ lpObj = &gObj[nUserIndex];

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	char szMemberName[MAX_ACCOUNT_LEN + 1];
	memcpy(szMemberName, lpMsg->szName, MAX_ACCOUNT_LEN + 1);

	if (gObjIsConnected(lpObj))
	{
		GDReqAllowJoinGuildMatching(nUserIndex, lpMsg->nAllowType, szMemberName, lpObj->m_PlayerData->GuildName);
	}
}

void GameProtocol::CGReqGetAllowListJoinGuildMatching(PMSG_REQ_ALLOWLIST_GUILDMATCHING *lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	LPOBJ lpObj = &gObj[nUserIndex];

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	int nGuildNumber = lpObj->m_PlayerData->GuildNumber;
	_GUILD_INFO_STRUCT * lpstGuildInfo = Guild.SearchGuild_Number(nGuildNumber);

	PMSG_ANS_WAIT_GUILDMATCHING pMsg;
	PHeadSubSetW((LPBYTE)&pMsg, 0xED, 0x07, sizeof(pMsg));

	if (!lpstGuildInfo || Guild.GetGuildMemberStatus(lpstGuildInfo->Name, lpObj->Name) != GUILD_MASTER)
	{
		pMsg.nResult = -2;
		GIOCP.DataSend(nUserIndex, (LPBYTE)&pMsg, sizeof(pMsg));
		return;
	}

	GDReqWaitGuildMatchingList(nGuildNumber, nUserIndex);
}

void GameProtocol::CGReqGetWaitStateListGuildMatching(int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	LPOBJ lpObj = &gObj[nUserIndex];

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	char szName[MAX_ACCOUNT_LEN + 1];
	memcpy(szName, lpObj->Name, MAX_ACCOUNT_LEN + 1);

	GDReqGetWaitStateListGuildMatching(nUserIndex, szName);
}

void GameProtocol::CGReqRegWantedPartyMember(PMSG_REQ_REG_WANTED_PARTYMEMBER *lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	LPOBJ lpObj = &gObj[nUserIndex];

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	BOOL bAlreadyParty = FALSE;
	int nPartyNumber = -1;

	if (lpObj->PartyNumber != -1)
	{
		bAlreadyParty = TRUE;
		nPartyNumber = lpObj->PartyNumber;
	}

	_PARTY_INFO_LISTDB stList;

	if (bAlreadyParty)
	{
		stList.btCurPartyMemberCnt = gParty.GetCount(nPartyNumber);
	}

	else
	{
		stList.btCurPartyMemberCnt = 1;
	}

	stList.btUsePassword = lpMsg->btUsePassword;
	stList.btWantedClass = lpMsg->btWantedClass;
	stList.nHuntingGround = lpMsg->nHuntingGround;
	stList.nMaxLevel = lpMsg->nMaxLevel;
	stList.nMinLevel = lpMsg->nMinLevel;
	memcpy(stList.szLeaderName, lpObj->Name, MAX_ACCOUNT_LEN + 1);
	memcpy(stList.szPassword, lpMsg->szPassword, 5);
	memcpy(stList.szTitle, lpMsg->szTitle, 41);
	memcpy(stList.btWantedClassDetailInfo, lpMsg->btWantedClassDetailInfo, 7);
	stList.btGensType = g_GensSystem.GetGensInfluence(&gObj[nUserIndex]);
	stList.nLeaderLevel = gObj[nUserIndex].Level + gObj[nUserIndex].m_PlayerData->MasterLevel;
	stList.btLeaderClass = gObj[nUserIndex].Class;
	stList.btApprovalType = lpMsg->btApprovalType;

	if (gObj[nUserIndex].m_PlayerData->ChangeUP == 1)
	{
		stList.btLeaderClass |= 0x08;
	}

	else if (gObj[nUserIndex].m_PlayerData->ChangeUP == 2)
	{
		stList.btLeaderClass |= CS_SET_CHANGEUP_SX;
		stList.btLeaderClass |= CS_SET_CHANGEUP2_SX;
		//		stList.btLeaderClass |= 0x10;
	}

	if (gObj[nUserIndex].m_IfState.use != 0 && gObj[nUserIndex].m_IfState.type == 2)
	{
		PMSG_ANS_REG_WANTED_PARTYMEMBER pMsg;
		PHeadSubSetB((LPBYTE)&pMsg, 0xEF, 0x00, sizeof(pMsg));
		pMsg.nResult = -2;

		GIOCP.DataSend(nUserIndex, (LPBYTE)&pMsg, sizeof(pMsg));
	}

	else
	{
		GDReqRegWantedPartyMember(stList, nUserIndex);
	}
}
*/

/*
void GameProtocol::CGReqGetPartyMatchingList(PMSG_REQ_GET_PARTYMATCHINGLIST * lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	LPOBJ lpObj = &gObj[nUserIndex];

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	char szSearchWord[11];
	memset(szSearchWord, 0x00, sizeof(szSearchWord));

	if (lpMsg->btUseSearchWord == 0)
	{
		GDReqGetPartyMatchingList(lpMsg->nPage, szSearchWord, 0, nUserIndex);
	}

	else if (lpMsg->btUseSearchWord == 1)
	{
		memcpy(szSearchWord, lpMsg->szSearchWord, 10);
		GDReqGetPartyMatchingList(lpMsg->nPage, szSearchWord, 1, nUserIndex);
	}

	else if (lpMsg->btUseSearchWord == 2)
	{
		GDReqGetPartyMatchingList(lpMsg->nPage, szSearchWord, 2, nUserIndex);
	}
}

void GameProtocol::CGReqJoinMemberPartyMatching(PMSG_REQ_MEMBERJOIN_PARTYMATCHINGLIST *lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	LPOBJ lpObj = &gObj[nUserIndex];

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	char szMemberName[MAX_ACCOUNT_LEN + 1];
	char szLeaderName[MAX_ACCOUNT_LEN + 1];
	char szPassword[5];

	memset(szMemberName, 0x00, MAX_ACCOUNT_LEN + 1);
	memset(szLeaderName, 0x00, MAX_ACCOUNT_LEN + 1);
	memset(szPassword, 0x00, 5);

	memcpy(szMemberName, lpObj->Name, MAX_ACCOUNT_LEN);
	memcpy(szLeaderName, lpMsg->szLeaderName, MAX_ACCOUNT_LEN);
	memcpy(szPassword, lpMsg->szPassword, 4);

	int nTotalLevel = lpObj->Level + lpObj->m_PlayerData->MasterLevel;
	BYTE btChangeUpClass = lpObj->Class;

	if (lpObj->m_PlayerData->ChangeUP == 1)
	{
		btChangeUpClass |= 0x08;
	}

	if (lpObj->m_PlayerData->ChangeUP == 2)
	{
		btChangeUpClass |= CS_SET_CHANGEUP_SX;
		btChangeUpClass |= CS_SET_CHANGEUP2_SX;
		//btChangeUpClass |= 0x08;
		//btChangeUpClass |= 0x18;
	}

	BYTE btClass = lpObj->Class;

	switch (lpObj->Class)
	{
	case CLASS_WIZARD:
		btClass = 4;
		break;
	case CLASS_KNIGHT:
		btClass = 1;
		break;
	case CLASS_ELF:
		btClass = 2;
		break;
	case CLASS_MAGUMSA:
		btClass = 8;
		break;
	case CLASS_DARKLORD:
		btClass = 16;
		break;
	case CLASS_SUMMONER:
		btClass = 32;
		break;
	case CLASS_RAGEFIGHTER:
		btClass = 64;
		break;
	case CLASS_GROWLANCER:
		btClass = 128;
		break;
	default:
		btClass = 0;
		break;
	}

	if (gObj[nUserIndex].m_IfState.use != 0 && gObj[nUserIndex].m_IfState.type == 2)
	{
		PMSG_ANS_MEMBERJOIN_PARTYMATCHINGLIST pMsg;
		PHeadSubSetB((LPBYTE)&pMsg, 0xEF, 0x02, sizeof(pMsg));
		pMsg.nResult = -4;

		GIOCP.DataSend(nUserIndex, (LPBYTE)&pMsg, sizeof(pMsg));
	}

	else
	{
		BYTE btGensType = g_GensSystem.GetGensInfluence(&gObj[nUserIndex]);
		GDReqJoinMemberPartyMatching(szMemberName, szLeaderName,
			szPassword, nUserIndex, lpObj->DBNumber,
			nTotalLevel, btClass, lpMsg->btUseRandomParty,
			btGensType, btChangeUpClass);
	}
}
*/

/*
void GameProtocol::CGReqMemberJoinStateList(PMSG_REQ_MEMBERJOIN_STATELIST_PARTYMATCHING *lpMsg, CGameObject* lpObj)
{
	GDReqMemberJoinStateList(lpObj->Name, lpObj);
}

void GameProtocol::CGReqMemberJoinStateListLeader(PMSG_REQ_WAITLIST_PARTYMATCHING *lpMsg, CGameObject* lpObj)
{
	GDReqMemberJoinStateListLeader(lpObj->Name, lpObj);
}

void GameProtocol::CGReqAcceptJoinMemberPartyMatching(PMSG_REQ_ACCEPTMEMBER_PARTYMATCHING *lpMsg, CGameObject* lpObj)
{
	char szMemberName[MAX_ACCOUNT_LEN + 1];
	char szLeaderName[MAX_ACCOUNT_LEN + 1];

	memset(szMemberName, 0x00, MAX_ACCOUNT_LEN + 1);
	memset(szLeaderName, 0x00, MAX_ACCOUNT_LEN + 1);

	memcpy(szMemberName, lpMsg->szMemberName, MAX_ACCOUNT_LEN);
	memcpy(szLeaderName, lpObj->Name, MAX_ACCOUNT_LEN);

	GDReqAcceptMemberJoin(lpObj, lpMsg->btType, szLeaderName, szMemberName, 0);
}

void GameProtocol::CGReqCancelPartyMatching(PMSG_REQ_CANCEL_JOIN_PARTYMATCHING *lpMsg, CGameObject* lpObj)
{
	GDReqCancelPartyMatching(lpObj, lpMsg->btType);
}
*/

#pragma pack (1)
struct stBuffList
{
	stBuffList()
	{
		this->btBuffIndex = 0;
		this->nBuffDuration = 0;
	}

	BYTE btBuffIndex;
	int nBuffDuration;
};

struct PMSG_BUFFLIST_SEND_PARTY
{
	PBMSG_HEAD h;
	char Name[MAX_ACCOUNT_LEN + 1];
	BYTE btBuffCount;
	stBuffList stBuffList[32];
};
#pragma pack ()

void GameProtocol::GCDisplayBuffeffectPartyMember(CGameObject* lpObj)
{
	if (lpObj->PartyNumber < 0)
	{
		return;
	}

	PMSG_BUFFLIST_SEND_PARTY pMsg;
	PHeadSetB((LPBYTE)&pMsg, 0x2E, sizeof(pMsg));

	pMsg.btBuffCount = lpObj->m_BuffEffectCount;

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		memcpy(pMsg.Name, lpObj->m_PlayerData->m_RealNameOfUBF, MAX_ACCOUNT_LEN + 1);
	}

	else
	{
		memcpy(pMsg.Name, lpObj->Name, MAX_ACCOUNT_LEN + 1);
	}

	int nPartyIndex = lpObj->PartyNumber;

	if (pMsg.btBuffCount > MAX_BUFFEFFECT)
	{
		pMsg.btBuffCount = MAX_BUFFEFFECT;
	}

	for (int nBuffCnt = 0; nBuffCnt < pMsg.btBuffCount; nBuffCnt++)
	{
		pMsg.stBuffList[nBuffCnt].btBuffIndex = lpObj->pntBuffEffectList[nBuffCnt]->BuffIndex;
		pMsg.stBuffList[nBuffCnt].nBuffDuration = lpObj->pntBuffEffectList[nBuffCnt]->EffectDuration;
	}

	/*for (int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		if (gObjIsConnected(gParty.m_PartyS[nPartyIndex].Number[i]))
		{
			GIOCP.DataSend(gParty.m_PartyS[nPartyIndex].Number[i], (LPBYTE)&pMsg, pMsg.h.size);
		}
	}*/
}

/*
void GameProtocol::GCPartyMemberPosition(CGameObject* lpObj)
{

	if (gObj[nIndex].m_PlayerData->m_bSendMemberPos == FALSE)
	{
		return;
	}

	if (gObj[nIndex].PartyNumber < 0)
	{
		return;
	}

	PMSG_MEMBER_POS_INFO_SEND pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xE7, 0x01, sizeof(pMsg));

	int nPartyNumber = gObj[nIndex].PartyNumber;

	if (ObjectMaxRange(nPartyNumber))
	{
		pMsg.btCount = 0;

		for (int i = 0; i < 5; i++)
		{
			int nTargetIndex = gParty.m_PartyS[nPartyNumber].Number[i];

			if (ObjectMaxRange(nTargetIndex) == false)
			{
				continue;
			}

			if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
			{
				memcpy(pMsg.stPosInfo[pMsg.btCount].szName, gObj[nTargetIndex].m_PlayerData->m_RealNameOfUBF, MAX_ACCOUNT_LEN + 1);
			}

			else
			{
				memcpy(pMsg.stPosInfo[pMsg.btCount].szName, gObj[nTargetIndex].Name, MAX_ACCOUNT_LEN + 1);
			}

			pMsg.stPosInfo[pMsg.btCount].btMapNumber = gObj[nTargetIndex].MapNumber;
			pMsg.stPosInfo[pMsg.btCount].btPosX = gObj[nTargetIndex].X;
			pMsg.stPosInfo[pMsg.btCount].btPosY = gObj[nTargetIndex].Y;

			pMsg.btCount++;
		}

		GIOCP.DataSend(nIndex, (LPBYTE)&pMsg, pMsg.h.size);
	}
}
*/

void GameProtocol::CGReqSendMemberPosInfoStart(CGameObject* lpObj)
{
	if (lpObj->PartyNumber < 0)
	{
		return;
	}

	lpObj->m_PlayerData->m_bSendMemberPos = TRUE;
}

void GameProtocol::CGReqSendMemberPosInfoStop(CGameObject* lpObj)
{
	lpObj->m_PlayerData->m_bSendMemberPos = FALSE;
}

void GameProtocol::GCReqSendNpcPosInfo(PMSG_REQ_NPC_POSITION *lpMsg, CGameObject* lpObj)
{

	if (lpObj->PartyNumber < 0)
	{
		return;
	}

	if (lpMsg->btType)
	{
		lpObj->m_PlayerData->m_bSendNpcPos = FALSE;
	}

	else
	{
		lpObj->m_PlayerData->m_bSendNpcPos = TRUE;
	}
}

void GameProtocol::CGReqCCF_DayTime(CGameObject *lpObj)
{
	/*
	if (g_ConfigRead.server.GetServerType() != SERVER_BATTLECORE)
	{
		return;
	}

	lpObj->m_bCCF_UI_Using = true;
	g_ChaosCastleFinal.Send_CCF_Schedule(*lpObj);
	*/
}

void GameProtocol::CGReqCCF_EnterCheck(CGameObject *lpObj)
{
	/*
	if (g_ConfigRead.server.GetServerType() != SERVER_BATTLECORE)
	{
		return;
	}

	PMSG_RESULT_CCF_ENTERCHECK pResult;
	PHeadSubSetB((LPBYTE)&pResult, 0xAF, 0x05, sizeof(pResult));

	if (lpObj->MapNumber != MAP_INDEX_LORENMARKET)
	{
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 593));
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pNotice, pNotice.h.size);

		pResult.byResult = 11;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (!lpObj->m_bCCF_UI_Using)
	{
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 593));
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pNotice, pNotice.h.size);

		pResult.byResult = 6;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (lpObj->m_IfState.use && lpObj->m_IfState.type != 12)
	{
		pResult.byResult = 12;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, &pResult.h.c, pResult.h.size);
		return;
	}

	if (lpObj->m_byCCF_CanEnter)
	{
		sLog->outBasic("[Chaos Castle Survival] [ CGReqCCF_EnterCheck ] ACC:%s, NAME:%s m_byCCF_CanEnter :%d ", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name,
			lpObj->m_byCCF_CanEnter);
		lpObj->m_byCCF_CanEnter = 0;

		pResult.byResult = 13;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (!g_ConfigRead.pk.bPkPenaltyDisable)
	{
		BOOL bPlayerKiller = FALSE; //Season 2.5 add-on

		if (lpObj->PartyNumber >= 0) //Season 2.5 add-on
		{
			if (gParty.GetPKPartyPenalty(lpObj->PartyNumber) >= 6)
			{
				bPlayerKiller = TRUE;
			}
		}
		else if (lpObj->m_PK_Level >= 6)
		{
			bPlayerKiller = TRUE;
		}

		if (bPlayerKiller == TRUE)
		{
			PMSG_NOTICE pNotice;
			TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 593));
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pNotice, pNotice.h.size);

			pResult.byResult = 14;
			pResult.byRemainTime = 0;
			pResult.byRemainTime_LOW = 0;
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
			return;
		}
	}

	if (gObjDuelCheck(*lpObj) == TRUE)
	{
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 594));
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pNotice, pNotice.h.size);

		pResult.byResult = 15;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (lpObj->m_PlayerData->lpGuild && lpObj->m_PlayerData->lpGuild->WarState == 1)
	{
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 595));
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pNotice, pNotice.h.size);

		pResult.byResult = 16;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (lpObj->pntInventory[lpObj->m_btInvenPetPos]->m_Type == ITEMGET(13, 3) ||
		lpObj->pntInventory[lpObj->m_btInvenPetPos]->m_Type == ITEMGET(13, 2) ||
		lpObj->pntInventory[lpObj->m_btInvenPetPos]->m_Type == ITEMGET(13, 37))
	{
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 596));
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pNotice, pNotice.h.size);

		pResult.byResult = 17;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	BOOL bPlayerKiller = FALSE; //Season 2.5 add-on

	if (lpObj->PartyNumber >= 0) //Season 2.5 add-on
	{
		if (gParty.GetPKPartyPenalty(lpObj->PartyNumber) >= 6)
		{
			bPlayerKiller = TRUE;
		}
	}
	else if (lpObj->m_PK_Level >= 6)
	{
		bPlayerKiller = TRUE;
	}

	if (bPlayerKiller == TRUE)
	{
		pResult.byResult = 8;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, &pResult.h.c, pResult.h.size);
		return;
	}

	if (g_ChaosCastleFinal.m_nCCF_Level > lpObj->Level)
	{
		pResult.byResult = 1;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, &pResult.h.c, pResult.h.size);
		return;
	}

	if (g_ChaosCastleFinal.CheckWearingMOPH(*lpObj) == TRUE)
	{
		GCServerMsgStringSend(Lang.GetText(0, 576), lpObj, 1);

		pResult.byResult = 18;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, &pResult.h.c, pResult.h.size);
		return;
	}
	if ((MapC[lpObj->MapNumber].GetAttr(lpObj->X, lpObj->Y) & 1) != 1)
	{
		pResult.byResult = 19;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, &pResult.h.c, pResult.h.size);
		return;
	}

	int nBattleType = g_ChaosCastleFinal.Check_CCF_DayType();

	if (!nBattleType)
	{
		sLog->outBasic("[Chaos Castle Survival][CGReqCCF_EnterCheck] No OpenDay , Index:%d,Name:%s", lpObj->m_Index, lpObj->Name);

		pResult.byResult = 10;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, &pResult.h.c, pResult.h.size);
		return;
	}
	if (nBattleType == 3 && g_ChaosCastleFinal.CheckOpenOrNotOpenCCF_Final(3) == TRUE)
	{
		sLog->outBasic("[Chaos Castle Survival][CGReqCCF_EnterCheck] Not Final Time, Index:%d,Name:%s", lpObj->m_Index, lpObj->Name);

		pResult.byResult = 2;
		int nRemainTime = g_ChaosCastleFinal.GetFinalRemainTime(*lpObj);
		pResult.byRemainTime = HIBYTE(nRemainTime);
		pResult.byRemainTime_LOW = LOBYTE(nRemainTime);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, &pResult.h.c, pResult.h.size);
		return;
	}
	if (g_ChaosCastleFinal.CheckCanEnter() == FALSE)
	{
		pResult.byResult = 2;
		int nRemainTime = g_ChaosCastleFinal.GetRemainTime(*lpObj);
		pResult.byRemainTime = HIBYTE(nRemainTime);
		pResult.byRemainTime_LOW = LOBYTE(nRemainTime);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, &pResult.h.c, pResult.h.size);
		return;
	}

	int nCheckIndex = g_ChaosCastleFinal.EnterCheck_CCF_User(*lpObj);

	if (nCheckIndex == -1)
	{
		pResult.byResult = 5;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, &pResult.h.c, pResult.h.size);
		return;
	}

	if (nCheckIndex == -2)
	{
		pResult.byResult = 9;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, &pResult.h.c, pResult.h.size);
		return;
	}

	if (!nBattleType)
	{
		pResult.byResult = 10;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, &pResult.h.c, pResult.h.size);
		return;
	}

	if (nBattleType == 1)
	{
		if (!g_ChaosCastleFinal.CheckUserEnterMoney(*lpObj, 1))
		{
			pResult.byResult = 7;
			pResult.byRemainTime = 0;
			pResult.byRemainTime_LOW = 0;
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, &pResult.h.c, pResult.h.size);
			return;
		}

		lpObj->m_byCCF_CanEnter = 1;

		int nReturn = this->CGReqEnterChaosCastleFinal(lpObj);

		if (nReturn)
		{
			sLog->outBasic("[Chaos Castle Survival][gObj_EnterChaosCastleFinal] ACC :%s , NAME :%s return :%d ", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, nReturn);
		}

		return;
	}

	if (nBattleType == 2)
	{
		if (g_ChaosCastleFinal.CheckUserEnterMoney(*lpObj, 2))
		{
			lpObj->m_byCCF_CanEnter = -1;
			g_ChaosCastleFinal.GD_Req_Get_Permission(lpObj);
			lpObj->m_nCCF_CertiTick = GetTickCount();
		}
		else
		{
			pResult.byResult = 7;
			pResult.byRemainTime = 0;
			pResult.byRemainTime_LOW = 0;
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, &pResult.h.c, pResult.h.size);
		}
	}
	else
	{
		if (g_ChaosCastleFinal.CheckUserEnterMoney(*lpObj, 3))
		{
			lpObj->m_byCCF_CanEnter = -1;
			g_ChaosCastleFinal.GD_Req_Get_Permission(lpObj);
			lpObj->m_nCCF_CertiTick = GetTickCount();
		}
		else
		{
			pResult.byResult = 7;
			pResult.byRemainTime = 0;
			pResult.byRemainTime_LOW = 0;
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, &pResult.h.c, pResult.h.size);
		}
	}*/
}

int GameProtocol::CGReqEnterChaosCastleFinal(CGameObject *lpObj)
{
	/*
	PMSG_RESULT_CCF_ENTERCHECK pResult;
	PHeadSubSetB((LPBYTE)&pResult, 0xAF, 0x05, sizeof(pResult));

	if (g_ChaosCastleFinal.PayUserEnterMoney(*lpObj, lpObj->m_byCCF_CanEnter) == FALSE)
	{
		pResult.byResult = 7;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return -3;
	}

	int nCCFUserIndex = 0;
	int nEnterCheck = g_ChaosCastleFinal.EnterCCF(*lpObj, nCCFUserIndex);

	if (nEnterCheck)
	{
		sLog->outBasic("[Chaos Castle Survival] (ACC:%s,NAME:%s) EnterCCF return ERROR :%d ", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, nEnterCheck);

		pResult.byResult = 20;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return nEnterCheck;
	}

	else
	{
		pResult.byResult = 0;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

		lpObj->m_nCCF_UserIndex = nCCFUserIndex;
		lpObj->m_byCCF_CanEnter = 0;
		lpObj->m_IfState.state = 0;
		lpObj->m_IfState.type = 0;
		lpObj->m_IfState.use = 0;
		lpObj->m_byKillMobCount = 0;
		lpObj->m_byKillUserCount = 0;
		lpObj->m_bCCF_Quit_Msg = 0;

		if (lpObj->m_bPShopOpen == true)
			g_PersonalStore.CGPShopReqClose(*lpObj);

		if (lpObj->PartyNumber >= 0)
		{
			PMSG_PARTYDELUSER pMsg;

			int pindex = gParty.GetIndex(lpObj->PartyNumber, *lpObj, lpObj->DBNumber);

			if (pindex >= 0)
			{
				pMsg.Number = pindex;
				//CGPartyDelUser(&pMsg, lpObj, 0);
			}
		}

		if (lpObj->Class == CLASS_ELF)
		{
			if (lpObj->m_RecallMon >= 0)
			{
				GCRecallMonLife(lpObj, 60, 0);
				gObjMonsterCallKill(*lpObj);
			}
		}

		GCServerMsgStringSend(Lang.GetText(0, 577), lpObj, 1);
		lpObj->MapNumber = MAP_INDEX_CHAOSCASTLE_SURVIVAL;
		gObjMoveGate(*lpObj, 443);

		sLog->outBasic("[Chaos Castle Survival] (ACC:%s,NAME:%s) Enter CCF_Tryout", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
		return 0;
	}*/
}

void GameProtocol::CGRequestRepositionUserInCCF(PMSG_REQ_REPOSUSER_IN_CCF *lpMsg, CGameObject* lpObj)
{
	/*if (lpObj->MapNumber != MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		return;
	}

	if (g_ChaosCastleFinal.ObjSetPosition(*lpObj, lpObj->X, lpObj->Y) == TRUE)
	{
		lpObj->m_nCCF_BlowTime = GetTickCount();
	}*/
}

void GameProtocol::CG_CCF_UI_OnOff(PMSG_CCF_UI_ONOFF *lpMsg, CGameObject* lpObj)
{

	if (lpObj->m_byCCF_CanEnter == 0xFF)
		lpObj->m_byCCF_CanEnter = 0;

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
		lpObj->m_bCCF_UI_Using = 0;
}

void GameProtocol::CGReq_CCF_Ranking(PMSG_REQ_CCF_RANKING *lpMsg, CGameObject* lpObj)
{
	/*
	if (g_ConfigRead.server.GetServerType() != SERVER_BATTLECORE)
	{
		return;
	}

	if (lpObj->m_bCCF_UI_Using == false)
	{
		return;
	}

	if (lpObj->MapNumber != MAP_INDEX_LORENMARKET)
	{
		return;
	}

	if (g_ChaosCastleFinal.Check_CCF_DayType() == FALSE)
	{
		_stPMSG_CCF_RANK_INFO pMsg;

		PHeadSubSetW((LPBYTE)&pMsg, 0xAF, 0x07, sizeof(pMsg));
		pMsg.btResult = 2;
		pMsg.btCnt = 0;
		pMsg.h.sizeH = HIBYTE(sizeof(pMsg));
		pMsg.h.sizeL = LOBYTE(sizeof(pMsg));
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));
	}

	if (g_ChaosCastleFinal.m_FirstRankingInfoLoad)
	{
		g_ChaosCastleFinal.Send_RankingInfo(lpMsg->byRankingType, *lpObj);
	}

	else
	{
		g_ChaosCastleFinal.GD_Load_CCF_RankingList(g_ChaosCastleFinal.Check_CCF_DayType());

		PMSG_CCF_RANK_INFO pMsg;

		PHeadSubSetW((LPBYTE)&pMsg, 0xAF, 0x07, sizeof(pMsg));
		pMsg.btResult = 3;
		pMsg.btCnt = 0;
		pMsg.h.sizeH = HIBYTE(sizeof(pMsg));
		pMsg.h.sizeL = LOBYTE(sizeof(pMsg));
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));

		sLog->outBasic("[Chaos Castle Survival][CGReq_CCF_Ranking] GD_Load_CCF_RankingList Send ");
	}
	*/
}
void GameProtocol::GCMuunItemDurSend(CGameObject* lpObj, BYTE pos, BYTE dur)
{
	PMSG_MUUNITEM_DUR pMsg;

	PHeadSubSetB(&pMsg.h.c, 0x4E, 0x03, sizeof(pMsg));
	pMsg.IPos = pos;
	pMsg.Dur = dur;
	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
}

void GameProtocol::GCMuunEquipmentChange(CGameObject* lpObj, int pos)
{
	PMSG_USER_MUUN_EQUIPMENT_CHANGED pChange;

	PHeadSubSetB((LPBYTE)&pChange, 0x4E, 0x06, sizeof(pChange));

	pChange.NumberH = SET_NUMBERH(lpObj);
	pChange.NumberL = SET_NUMBERL(lpObj);
	ItemByteConvert(pChange.ItemInfo, *lpObj->pntMuunInventory[pos]);
	pChange.ItemInfo[1] = 16 * pos;
	pChange.ItemInfo[1] = lpObj->pntMuunInventory[pos]->m_Level & 0xF | 16 * pos;
	this->MsgSendV2(lpObj, (LPBYTE)&pChange, (unsigned __int8)pChange.h.size);
}

void GameProtocol::GCMuunInventoryItemDeleteSend(CGameObject* lpObj, BYTE pos, BYTE flag)
{
	PMSG_PETINVENTORY_DELETE pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0x4E, 0x04, sizeof(pMsg));
	pMsg.IPos = pos;
	pMsg.Flag = flag;
	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
}

void GameProtocol::GCMuunInventoryItemOneSend(CGameObject* lpObj, int pos)
{
	PMSG_MUUN_INVEN_ITEM_MOD pMsg;

	if (lpObj->pntMuunInventory[pos]->IsItem())
	{
		PHeadSubSetB((LPBYTE)&pMsg, 0x4E, 0x05, sizeof(pMsg));
		pMsg.Pos = pos;
		ItemByteConvert(pMsg.ItemInfo, *lpObj->pntMuunInventory[pos]);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
	}
}

void GameProtocol::CGMuunItemGetRequest(PMSG_MUUNITEM_GETREQUEST *lpMsg, CGameObject* lpObj)
{
	int item_num, map_num;
	PMSG_MUUNITEM_GETRESULT pResult;
	int type;
	int level;
	int special;
	int NOption;
	char szItemName[50];
	CMapItem * lpItem;

	pResult.h.c = 0xC3;
	pResult.h.headcode = 0x4E;
	pResult.h.subcode = 0x00;
	pResult.h.size = sizeof(pResult);
	pResult.result = -1;

	if (lpObj->m_PlayerData->ConnectUser->CloseType != -1)
		return;

	if (lpObj->m_PlayerData->m_bSecurityCheck == false)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	if (lpObj->DieRegen != 0)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, &pResult.h.c, (unsigned __int8)pResult.h.size);

		return;
	}

	if (g_NewPVP.IsObserver(*lpObj))
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

		return;
	}

	if (lpObj->m_IfState.use == 1)
	{
		GCServerMsgStringSend(Lang.GetText(0, 597), lpObj, 1);

		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, (pResult.h.size - 12));

		return;
	}

	if (!::gObjFixMuunInventoryPointer(*lpObj))

		if (lpObj->pTransaction == TRUE)
		{
			return;
		}

	item_num = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	if (MaxItemTypeRange(item_num) == FALSE)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	map_num = lpObj->MapNumber;

	if (MAX_MAP_RANGE(map_num) == FALSE)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	lpItem = &MapC[map_num].m_cItem[item_num];

	if (lpItem->IsItem() == TRUE && lpItem->Give == false && lpItem->live == true)
	{
		type = lpItem->m_Type;
		level = lpItem->m_Level;
		special = lpItem->m_Special[0];
		NOption = lpItem->m_NewOption;
		LPITEM_ATTRIBUTE pItemAttribute = GetItemAttr(type);

		if (!pItemAttribute)
		{
			return;
		}

		if (pItemAttribute->ItemKindA != 12)
		{
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

			return;
		}

		pResult.result = gObjChkMuunInventoryEmpty(*lpObj);

		if (pResult.result == 255)
		{
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

			return;
		}

		if (g_CMuunSystem.IsMuunExpireDate(lpItem->m_Type) == TRUE)
		{
			lpItem->SetMuunItemPeriodExpire();
		}

		ItemByteConvert((LPBYTE)&pResult.Data[0], lpItem->m_Type, lpItem->m_Option1, lpItem->m_Option2,
			lpItem->m_Option3, lpItem->m_Level, lpItem->m_Durability, lpItem->m_NewOption,
			lpItem->m_SetOption, lpItem->m_JewelOfHarmonyOption, lpItem->m_ItemOptionEx, lpItem->m_SocketOption, lpItem->m_BonusSocketOption, 0);

		if (MapC[map_num].ItemGive(*lpObj, item_num, 0) == 1)
		{
			BYTE Ret = ::gObjMuunInventoryInsertItem(*lpObj, *lpItem);

			if (Ret == 0xFF)
				pResult.result = -1;

			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

			if (Ret == 0xFF)
			{
				GCMuunInventoryItemListSend(lpObj);
			}

			else
			{
				BYTE NewOption[MAX_EXOPTION_SIZE];

				::ItemIsBufExOption(NewOption, (lpItem != NULL) ? (CItemObject*)&lpItem->m_Number : NULL);

				if (g_CMuunSystem.IsStoneofEvolution(lpItem))
				{
					WORD wPetItemNumber = (lpItem->m_SocketOption[0] << 8) | lpItem->m_SocketOption[1];
					LPITEM_ATTRIBUTE PetItemAttr = GetItemAttr(wPetItemNumber);

					if (!PetItemAttr)
					{
						this->GCSendGetItemInfoForParty(lpObj, lpItem);

						return;
					}

				}

				else
				{

				}

				this->GCSendGetItemInfoForParty(lpObj, lpItem);
			}
		}

		else
		{
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);

			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
			return;
		}
	}

	else
	{

		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);

		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}
}

void GameProtocol::CGMuunItemSellRequestRecv(PMSG_MUUN_ITEM_SELLREQUEST *lpMsg, CGameObject* lpObj)
{
	PMSG_MUUN_ITEM_SELLRESULT pResult;

	PHeadSubSetB((LPBYTE)&pResult, 0x4E, 0x09, sizeof(pResult));

	pResult.Result = 0;
	pResult.Money = 0;

	if (lpObj->m_PlayerData->ConnectUser->CloseType != -1)
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (lpObj->m_IfState.use > 0 && lpObj->m_IfState.type != 3)
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (!PacketCheckTime(lpObj))
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (!::gObjFixMuunInventoryPointer(*lpObj))
	{

	}

	if (lpObj->pTransaction == 1)
	{
		return;
	}

	if (lpObj->TargetNpcNumber == -1)
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	CGameObject* lpShopObj = getGameObject(lpObj->TargetNpcNumber);

	SHOP_DATA * lpShopData = g_ShopMng.GetShop(*lpObj, *lpShopObj);

	if (!lpShopData)
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (!gObjCanItemTouch(*lpObj, 3))
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (lpMsg->Pos > MUUN_INVENTORY_SIZE - 1)
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (gObjCheckSerial0ItemList(lpObj->pntMuunInventory[lpMsg->Pos]) != FALSE)
	{
		MsgOutput(*lpObj, Lang.GetText(0, 259));
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

		return;
	}

	if (!IsSellToNPCItem(lpObj->pntMuunInventory[lpMsg->Pos]->m_Type))
	{
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (lpObj->pntMuunInventory[lpMsg->Pos]->IsItem() == FALSE)
	{
		pResult.Result = false;
		pResult.Money = lpObj->m_PlayerData->Money;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

	}
	else
	{
		int sellmoney = 0;

		if (g_CMuunSystem.IsMuunItem(lpObj->pntMuunInventory[lpMsg->Pos]->m_Type) == TRUE)
		{
			sellmoney = 500000;
		}

		else if (g_CMuunSystem.IsStoneofEvolution(lpObj->pntMuunInventory[lpMsg->Pos]->m_Type) == TRUE)
		{
			sellmoney = 200000;
		}

		else if (lpObj->pntMuunInventory[lpMsg->Pos]->m_Type == ITEMGET(13, 239))
		{
			sellmoney = 300000;
		}

		else if (lpObj->pntMuunInventory[lpMsg->Pos]->m_Type == ITEMGET(13, 240))
		{
			sellmoney = 1000000;
		}

		else
		{
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
			return;
		}

		if (gObjCheckMaxZen(*lpObj, sellmoney) == FALSE)
		{
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
			return;
		}

		lpObj->m_PlayerData->Money += sellmoney;
		pResult.Result = 1;
		pResult.Money = lpObj->m_PlayerData->Money;

		BYTE NewOption[MAX_EXOPTION_SIZE];
		ItemIsBufExOption(NewOption, lpObj->pntMuunInventory[lpMsg->Pos]);

		if (lpMsg->Pos == 0 || lpMsg->Pos == 1)
		{
			WORD nItemCode = lpObj->pntMuunInventory[lpMsg->Pos]->m_Type;
			UINT64 nSerial = lpObj->pntMuunInventory[lpMsg->Pos]->m_Number;

			if (lpMsg->Pos == 0)
			{
				lpObj->m_wMuunItem = -1;
			}

			else if (lpMsg->Pos == 1)
			{
				lpObj->m_wMuunSubItem = -1;
			}

			g_CMuunSystem.RemoveUserMuunEffect(*lpObj, lpMsg->Pos);
			this->GCMuunEquipmentChange(lpObj, lpMsg->Pos);
		}

		gObjMuunInventoryDeleteItem(*lpObj, lpMsg->Pos);
		gObjMakePreviewCharSet(*lpObj);

		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
	}
}




void GameProtocol::CGReqEventInvenOpen(LPBYTE lpRecv, CGameObject* lpObj)
{
	/*
	if (g_CMuRummyMng.IsMuRummyEventOn() == true)
	{
		g_CMuRummyMng.CGReqMuRummyEventOpen((PMSG_REQ_MURUMMY_EVENT_OPEN *)lpRecv, *lpObj);
		return;
	}

	if (g_ConfigRead.data.common.EventInventoryOpen)
	{
		PMSG_ANS_MURUMMY_EVENT_OPEN pMsg;
		PHeadSubSetB((LPBYTE)&pMsg, 0x4D, 0x0F, sizeof(pMsg));

		pMsg.btResult = 3;

		time_t tmEndTime;
		DWORD dw32EndTime;
		SystemTimeToUnixTime(&g_ConfigRead.data.common.EventEndDate, &tmEndTime);
		dw32EndTime = (DWORD)tmEndTime;

		pMsg.btEventTime1 = SET_NUMBERH(SET_NUMBERHW(dw32EndTime));
		pMsg.btEventTime2 = SET_NUMBERL(SET_NUMBERHW(dw32EndTime));
		pMsg.btEventTime3 = SET_NUMBERH(SET_NUMBERLW(dw32EndTime));
		pMsg.btEventTime4 = SET_NUMBERL(SET_NUMBERLW(dw32EndTime));

		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
	}
	*/
}

void GameProtocol::CGEventItemGetRequest(PMSG_EVENTITEM_GETREQUEST *lpMsg, CGameObject* lpObj)
{

	int item_num, map_num;
	PMSG_EVENTITEM_GETRESULT pResult;
	int type;
	int level;
	int special;
	int NOption;
	char szItemName[50];
	CMapItem * lpItem;

	pResult.h.c = 0xC3;
	pResult.h.headcode = 0x4D;
	pResult.h.subcode = 0x00;
	pResult.h.size = sizeof(pResult);
	pResult.result = -1;

	pResult.index[0] = lpMsg->NumberH;
	pResult.index[1] = lpMsg->NumberL;

	if (!gObjIsConnected(*lpObj))
	{
		//this->GCSendDisableReconnect(lpObj);
		GIOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
		return;
	}

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	if (lpObj->m_PlayerData->ConnectUser->CloseType != -1)
		return;

	if (g_CMuRummyMng.IsMuRummyEventOn() == false && g_ConfigRead.data.common.EventInventoryOpen == false)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	if (lpObj->m_PlayerData->m_bSecurityCheck == false)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	if (lpObj->DieRegen != 0)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	if (g_NewPVP.IsObserver(*lpObj) == TRUE)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	if (!::gObjFixEventInventoryPointer(*lpObj))
		sLog->outBasic("[Fix EventInv.Ptr] False Location - %s, %d", __FILE__, __LINE__);

	if (lpObj->pTransaction == TRUE)
	{
		//sLog->outBasic("[%s][%s] CGEventItemGetRequest() Failed : Transaction == 1, IF_TYPE : %d",
		//	lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_IfState.type);
		return;

	}

	item_num = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	if (MaxItemTypeRange(item_num) == FALSE)
	{
		sLog->outBasic("error-L3 : %s %d", __FILE__, __LINE__);
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	map_num = lpObj->MapNumber;

	if (MAX_MAP_RANGE(map_num) == FALSE)
	{
		sLog->outBasic("error-L3 : %s %d", __FILE__, __LINE__);
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	lpItem = &MapC[map_num].m_cItem[item_num];

	if (lpItem->IsItem() == TRUE && lpItem->Give == false && lpItem->live == true)
	{
		type = lpItem->m_Type;
		level = lpItem->m_Level;
		special = lpItem->m_Special[0];
		NOption = lpItem->m_NewOption;
		LPITEM_ATTRIBUTE pItemAttribute = GetItemAttr(type);

		if (!pItemAttribute)
		{
			return;
		}

		if (pItemAttribute->ItemKindA != 11)
		{
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

			return;
		}

		if (lpItem->m_Type == ITEMGET(14, 278))
		{
			int iInventoryIndex;

			for (int iLoop = 0; iLoop < 32; iLoop++)
			{
				iInventoryIndex = -1;

				if (lpObj->pntEventInventory[iLoop]->m_Type == ITEMGET(14, 278) &&
					lpObj->pntEventInventory[iLoop]->m_Durability < 25.0)
				{
					iInventoryIndex = iLoop;
				}

				if (iInventoryIndex < 0 || iInventoryIndex > EVENT_INVENTORY_SIZE - 1)
				{
					continue;
				}

				if (MapC[map_num].ItemGive(*lpObj, item_num, 1) == TRUE)
				{
					BYTE NewOption[MAX_EXOPTION_SIZE];

					::ItemIsBufExOption(NewOption, (lpItem != NULL) ? (CItemObject*)&lpItem->m_Number : NULL);

					pResult.result = -3;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

					lpObj->pntEventInventory[iInventoryIndex]->m_Durability += lpItem->m_Durability;
					
					if (lpObj->pntEventInventory[iInventoryIndex]->m_Durability < 25.0)
					{
						this->GCEventItemDurSend(lpObj, iInventoryIndex, lpObj->pntEventInventory[iInventoryIndex]->m_Durability);
						return;
					}

					int iDurGap = lpObj->pntEventInventory[iInventoryIndex]->m_Durability - 25.0;
					BYTE btRet = gObjEventInventoryInsertItemTemp(*lpObj, lpItem);
					gObjEventInventoryItemSet(*lpObj, iInventoryIndex, -1);
					lpObj->pntEventInventory[iInventoryIndex]->Clear();
					this->GCEventInventoryItemDeleteSend(lpObj, iInventoryIndex, 1);

					ItemCreate(lpObj, 226, lpObj->X, lpObj->Y, ITEMGET(14, 244), lpItem->m_Level, 0, 0, 0, 0, lpObj->m_Index, 0, 0, 0, 0, 0);

					if (iDurGap <= 0)
					{
						return;
					}

					if (btRet == 255)
					{
						ItemCreate(lpObj, lpObj->MapNumber, lpObj->X, lpObj->Y, ItemGetNumberMake(14, 278), lpItem->m_Level, 0, 0, 0, 0, lpObj->m_Index, 0, 0, 0, 0, 0);
						return;
					}

					lpItem->m_State = 2;
					lpItem->Give = 0;
					lpItem->live = 1;
					lpItem->m_Durability = iDurGap;
				}
			}
		}

		if (lpItem->m_Type == ITEMGET(14, 215))
		{
			int iInventoryIndex;

			for (int iLoop = 0; iLoop < 32; iLoop++)
			{
				iInventoryIndex = -1;

				if (lpObj->pntEventInventory[iLoop]->m_Type == ITEMGET(14, 215) &&
					lpObj->pntEventInventory[iLoop]->m_Durability < 24.0)
				{
					iInventoryIndex = iLoop;
				}

				if (iInventoryIndex < 0 || iInventoryIndex > EVENT_INVENTORY_SIZE - 1)
				{
					continue;
				}

				if (MapC[map_num].ItemGive(*lpObj, item_num, 1) == TRUE)
				{
					BYTE NewOption[MAX_EXOPTION_SIZE];

					::ItemIsBufExOption(NewOption, (lpItem != NULL) ? (CItemObject*)&lpItem->m_Number : NULL);

					pResult.result = -3;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

					lpObj->pntEventInventory[iInventoryIndex]->m_Durability += lpItem->m_Durability;

					if (lpObj->pntEventInventory[iInventoryIndex]->m_Durability < 24.0)
					{
						this->GCEventItemDurSend(lpObj, iInventoryIndex, lpObj->pntEventInventory[iInventoryIndex]->m_Durability);
						return;
					}

					int iDurGap = lpObj->pntEventInventory[iInventoryIndex]->m_Durability - 24.0;
					BYTE btRet = gObjEventInventoryInsertItemTemp(*lpObj, lpItem);
					gObjEventInventoryItemSet(*lpObj, iInventoryIndex, -1);
					lpObj->pntEventInventory[iInventoryIndex]->Clear();
					this->GCEventInventoryItemDeleteSend(lpObj, iInventoryIndex, 1);

					ItemCreate(lpObj, 226, lpObj->X, lpObj->Y, ITEMGET(14, 216), lpItem->m_Level, 0, 0, 0, 0, lpObj->m_Index, 0, 0, 0, 0, 0);

					if (iDurGap <= 0)
					{
						return;
					}

					if (btRet == 255)
					{
						ItemCreate(lpObj, lpObj->MapNumber, lpObj->X, lpObj->Y, ItemGetNumberMake(14, 215), lpItem->m_Level, 0, 0, 0, 0, lpObj->m_Index, 0, 0, 0, 0, 0);
						return;
					}

					lpItem->m_State = 2;
					lpItem->Give = 0;
					lpItem->live = 1;
					lpItem->m_Durability = iDurGap;
				}
			}
		}

		pResult.result = gObjEventInventoryInsertItemTemp(*lpObj, lpItem);

		if (pResult.result == 255)
		{
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);

			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
			return;
		}

		ItemByteConvert((LPBYTE)&pResult.Data[0], lpItem->m_Type, lpItem->m_Option1, lpItem->m_Option2,
			lpItem->m_Option3, lpItem->m_Level, lpItem->m_Durability, lpItem->m_NewOption,
			lpItem->m_SetOption, lpItem->m_JewelOfHarmonyOption, lpItem->m_ItemOptionEx, lpItem->m_SocketOption, lpItem->m_BonusSocketOption, 0);

		if (MapC[map_num].ItemGive(*lpObj, item_num, false) == TRUE)
		{
			BYTE Ret = ::gObjEventInventoryInsertItem(*lpObj, *lpItem);

			if (Ret == 0xFF)
				pResult.result = -1;

			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

			if (Ret == 0xFF)
			{
				this->GCEventInventoryItemListSend(lpObj);
			}
			else
			{
				BYTE NewOption[MAX_EXOPTION_SIZE];

				::ItemIsBufExOption(NewOption, (lpItem != NULL) ? (CItemObject*)&lpItem->m_Number : NULL);

				this->GCSendGetItemInfoForParty(lpObj, lpItem);
			}
		}

		else
		{
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);

			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
			return;
		}
	}

	else
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);

		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}
}

bool GameProtocol::CGEventItemDropRequest(PMSG_EVENTITEM_THROW *lpMsg, CGameObject* lpObj)
{
	BOOL CopyItem = FALSE;
	BYTE NewOption[MAX_EXOPTION_SIZE];

	if (!PacketCheckTime(lpObj))
		return FALSE;

	if (lpObj->Type != OBJ_USER)
	{
		return FALSE;
	}

	PMSG_EVENTITEM_THROW_RESULT pResult;

	PHeadSubSetB((LPBYTE)&pResult, 0x4D, 0x01, sizeof(pResult));
	pResult.Result = true;
	pResult.Ipos = lpMsg->Ipos;

	if (lpObj->m_IfState.use != 0)
	{
		if (lpObj->m_IfState.type != 3)
		{
			pResult.Result = false;
			return FALSE;
		}
	}

	if (lpObj->m_PlayerData->m_bSecurityCheck == false)
	{
		MsgOutput(*lpObj, Lang.GetText(0, 394));
		pResult.Result = false;
	}

	if (lpObj->m_PlayerData->ConnectUser->CloseType != -1)
		pResult.Result = false;

	if (lpObj->m_PlayerData->m_bMapSvrMoveQuit == true)
	{
		sLog->outBasic("[ANTI-HACK][Item Duplication] - Item Drop during MapServer Move [%s][%s]",
			lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
		pResult.Result = false;
	}

	if (!::gObjFixEventInventoryPointer(*lpObj))
		sLog->outBasic("[Fix EventInv.Ptr] False Location - %s, %d", __FILE__, __LINE__);

	if (lpObj->pTransaction == 1)
	{
		//sLog->outBasic("[%s][%s] CGEventItemDropRequest() Failed : Transaction == 1, IF_TYPE : %d",
		//	lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->m_IfState.type);  // TODO - NO idea why error.
		pResult.Result = false;

		return false;
	}

	if (lpMsg->Ipos == 0xFF)
	{
		sLog->outBasic("error:%s %d", __FILE__, __LINE__);
		pResult.Result = false;
	}

	if (lpMsg->Ipos < 0 || lpMsg->Ipos > MAIN_INVENTORY_SIZE - 1)
		pResult.Result = false;

	if (!gObjCanItemTouch(*lpObj, 0))
		pResult.Result = false;

	if (!IsDumpItem(lpObj->pntEventInventory[lpMsg->Ipos]->m_Type))
		pResult.Result = false;

	LPITEM_ATTRIBUTE pItemAttribute = GetItemAttr(lpObj->pntEventInventory[lpMsg->Ipos]->m_Type);

	if (!pItemAttribute)
	{
		return false;
	}

	if (pItemAttribute->ItemKindA != 11)
		pResult.Result = false;

	if (pResult.Result == 1)
	{
		::ItemIsBufExOption(NewOption, lpObj->pntEventInventory[lpMsg->Ipos]);

		int map = lpObj->MapNumber;
		int type = lpObj->pntEventInventory[lpMsg->Ipos]->m_Type;
		int level = lpObj->pntEventInventory[lpMsg->Ipos]->m_Level;
		float dur = lpObj->pntEventInventory[lpMsg->Ipos]->m_Durability;
		BOOL bIsItem = lpObj->pntEventInventory[lpMsg->Ipos]->IsItem();
		BYTE Option1 = lpObj->pntEventInventory[lpMsg->Ipos]->m_Option1;
		BYTE Option2 = lpObj->pntEventInventory[lpMsg->Ipos]->m_Option2;
		BYTE Option3 = lpObj->pntEventInventory[lpMsg->Ipos]->m_Option3;
		BYTE NOption = lpObj->pntEventInventory[lpMsg->Ipos]->m_NewOption;
		UINT64 serial = lpObj->pntEventInventory[lpMsg->Ipos]->m_Number;
		int PetLevel = lpObj->pntEventInventory[lpMsg->Ipos]->m_PetItem_Level;
		UINT64 PetExp = lpObj->pntEventInventory[lpMsg->Ipos]->m_PetItem_Exp;
		BYTE SOption = lpObj->pntEventInventory[lpMsg->Ipos]->m_SetOption;
		BYTE btItemEffectEx = lpObj->pntEventInventory[lpMsg->Ipos]->m_ItemOptionEx;
		UINT64 serial2 = lpObj->pntEventInventory[lpMsg->Ipos]->m_Number;
		char szItemName[50] = "Item";
		BYTE JOHOption = g_kJewelOfHarmonySystem.GetItemStrengthenOption(lpObj->pntEventInventory[lpMsg->Ipos]);
		BYTE JOHOptionLevel = g_kJewelOfHarmonySystem.GetItemOptionLevel(lpObj->pntEventInventory[lpMsg->Ipos]);

		//Season 4 add-on
		BYTE SocketOption[5];
		BYTE SocketIndex;
		g_SocketOptionSystem.GetSocketOption(lpObj->pntEventInventory[lpMsg->Ipos], SocketOption, SocketIndex);

		for (int iOption = 0; iOption < 5; iOption++)
		{
			SocketOption[iOption] = lpObj->pntEventInventory[lpMsg->Ipos]->m_SocketOption[iOption];
		}

		if (gObjCheckSerial0ItemList(lpObj->pntEventInventory[lpMsg->Ipos]) != FALSE)
		{
			MsgOutput(*lpObj, Lang.GetText(0, 259));
			sLog->outBasic("[ANTI-HACK][Serial 0 Item] [ItemDrop] (%s)(%s) Item(%s) Pos(%d)",
				lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name, lpObj->pntEventInventory[lpMsg->Ipos]->GetName(),
				lpMsg->Ipos);

			CopyItem = TRUE;
			pResult.Result = false;
			bIsItem = FALSE;
		}

		if (!::gObjEventInventorySearchSerialNum(*lpObj, serial2))
		{
			CopyItem = TRUE;
			pResult.Result = false;
			bIsItem = FALSE;
		}

		if (bIsItem == TRUE)
		{
			strcpy(szItemName, lpObj->pntEventInventory[lpMsg->Ipos]->GetName());

			if (g_BagManager.SearchAndUseBag(*lpObj, BAG_COMMON, type, level) == true)
			{
				gObjEventInventoryDeleteItem(*lpObj, lpMsg->Ipos);
			}
			else if (type == ITEMGET(13, 210) && level == 0)
			{
				gObjEventInventoryDeleteItem(*lpObj, lpMsg->Ipos);
				g_BagManager.SearchAndUseBag(*lpObj, BAG_EVENT, EVENTBAG_EGG_PETITEM, lpObj->m_Index);
			}
			else if (type == ITEMGET(13, 220) && level == 0)
			{
				gObjEventInventoryDeleteItem(*lpObj, lpMsg->Ipos);
				g_BagManager.SearchAndUseBag(*lpObj, BAG_EVENT, EVENTBAG_EGG_LUCKYBAG, lpObj->m_Index);

			}

			else if (type == ITEMGET(13, 221) && level == 0)
			{
				gObjEventInventoryDeleteItem(*lpObj, lpMsg->Ipos);
				g_BagManager.SearchAndUseBag(*lpObj, BAG_EVENT, EVENTBAG_EGG_FIREGHOST, lpObj->m_Index);

			}

			else if (type == ITEMGET(13, 222) && level == 0)
			{
				gObjEventInventoryDeleteItem(*lpObj, lpMsg->Ipos);
				g_BagManager.SearchAndUseBag(*lpObj, BAG_EVENT, EVENTBAG_EGG_GOLDGOBLIN, lpObj->m_Index);

			}

			else if (type == ITEMGET(14, 244))
			{
				if (g_EvoMonMng.UseSummonScroll(*lpObj) == FALSE)
				{
					pResult.Result = false;
					GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

					return FALSE;
				}

				gObjEventInventoryDeleteItem(*lpObj, lpMsg->Ipos);
			}

			else
			{
				if (MapC[map].ItemDrop(type, level, dur, lpMsg->px, lpMsg->py, Option1, Option2, Option3,
					NOption, SOption, serial2, lpObj->m_Index, PetLevel, PetExp, btItemEffectEx, SocketOption, SocketIndex, 0) == TRUE)
				{
					gObjEventInventoryDeleteItem(*lpObj, lpMsg->Ipos);
					pResult.Result = true;

				}

				else
				{
					pResult.Result = false;
				}
			}
		}

		else
		{
			pResult.Result = false;
		}
	}

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

	if (CopyItem != FALSE)
	{
		this->GCEventInventoryItemListSend(lpObj);
	}
}

void GameProtocol::GCEventInventoryItemListSend(CGameObject* lpObj)
{
	PMSG_INVENTORYLISTCOUNT pMsgILC;
	PMSG_INVENTORYLIST pMsgIL;
	int sOfs = sizeof(PMSG_INVENTORYLISTCOUNT);
	int pMsgILSize = sizeof(PMSG_INVENTORYLIST);
	BYTE sendBuf[4096];
	int itemcount = 0;

	for (int n = 0; n < EVENT_INVENTORY_SIZE; n++)
	{
		if (lpObj->pntEventInventory[n]->IsItem() == TRUE)
		{
			if (lpObj->pntEventInventory[n]->m_bItemExist == false)
			{
				continue;
			}

			pMsgIL.Pos = n;
			ItemByteConvert(pMsgIL.ItemInfo, *lpObj->pntEventInventory[n]);
			memcpy(&sendBuf[sOfs], &pMsgIL, pMsgILSize);
			itemcount++;
			sOfs += pMsgILSize;
		}
	}

	pMsgILC.h.c = 0xC4;
	pMsgILC.h.headcode = 0x4D;
	pMsgILC.subcode = 0x02;
	pMsgILC.h.sizeH = SET_NUMBERH(sOfs);
	pMsgILC.h.sizeL = SET_NUMBERL(sOfs);
	pMsgILC.Count = itemcount;
	memcpy(sendBuf, &pMsgILC, sizeof(PMSG_INVENTORYLISTCOUNT));

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, sendBuf, sOfs);
}

void GameProtocol::GCEventItemDurSend(CGameObject* lpObj, BYTE pos, BYTE dur)
{
	PMSG_EVENTITEM_DUR pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0x4D, 0x03, sizeof(pMsg));
	pMsg.IPos = pos;
	pMsg.Dur = dur;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCEventInvenItemOneSend(CGameObject* lpObj, int pos)
{
	if (!lpObj->pntEventInventory[pos]->IsItem())
		return;

	PMSG_EVENT_INVEN_ITEM_MOD pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0x4D, 0x05, sizeof(pMsg));
	pMsg.Pos = pos;
	ItemByteConvert(pMsg.ItemInfo, *lpObj->pntEventInventory[pos]);

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR *)&pMsg, pMsg.h.size);
}

void GameProtocol::GCEventInventoryItemDeleteSend(CGameObject* lpObj, BYTE pos, unsigned char flag)
{
	PMSG_EVENTINVENTORY_DELETE pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0x4D, 0x04, sizeof(pMsg));
	pMsg.IPos = pos;
	pMsg.Flag = flag;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}


void GameProtocol::GCResSendExpEventInfo(CGameObject* lpObj)
{
	PMSG_RES_EXPEVENTINFO pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xBF, 0x52, sizeof(pMsg));

	pMsg.wPCBangRate = 0;
	pMsg.wExpRate = g_ExpManager.GetEventExp() / g_ExpManager.GetStaticExp() * 100.0;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
}


void GameProtocol::CGReq_ITL_Relics(PMSG_REQ_ITL_RELATE_RELICS *lpMsg, CGameObject *lpObj)
{
	/*if (!lpMsg)
	{
		return;
	}

	WORD NpcNumber = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
	CGameObject* lpObjNPC = getGameObject(NpcNumber);

	if (lpObj->MapNumber != lpObjNPC->MapNumber)
	{
		return;
	}

	sLog->outBasic("[ ITL ][ ITR ][ActRelicsGetOrRegister] NpcIndex:%d,byPrePacket:%d,byAct:%d,wNpcType:%d",
		NpcNumber, lpMsg->byPrePacket, lpMsg->byAct, lpMsg->wNpcType);

	if (IT_MAP_RANGE(lpObj->MapNumber))
	{
		g_IT_Event.ActRelicsGetOrRegister(*lpObjNPC, *lpObj, lpObj->MapNumber, lpMsg->byPrePacket, lpMsg->byAct, lpMsg->wNpcType);
	}*/
}

void GameProtocol::CG_Req_Enter_ITR(PMSG_REQ_ENTER_ITR *lpMsg, CGameObject *lpObj)
{
	/*
	_ITL_AnswerEnter pResult;
	PHeadSubSetB((LPBYTE)&pResult, 0xBF, 0x70, sizeof(pResult));
	memset(pResult.Name, 0x00, sizeof(pResult.Name));

	if (g_IT_Event.IsCanEnterTiming() == false)
	{
		pResult.bReason = 8;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (lpObj->PartyNumber < 0)
	{
		pResult.bReason = 1;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (gParty.Isleader(lpObj->PartyNumber, *lpObj, lpObj->DBNumber) == false)
	{
		pResult.bReason = 3;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (gParty.GetCount(lpObj->PartyNumber) < g_IT_Event.m_iITR_MinUserInParty)
	{
		pResult.bReason = 4;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (gParty.GetPKPartyPenalty(lpObj->PartyNumber) >= 5)
	{
		pResult.bReason = 12;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	int PartyUsersIndex = 0;
	BOOL bResult = this->EnterITRCheckUser(lpObj, PartyUsersIndex);

	if (bResult != 0)
	{
		pResult.bReason = bResult;
		memcpy(pResult.Name, getGameObject(PartyUsersIndex)->Name, MAX_ACCOUNT_LEN + 1);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

		if (lpObj->m_Index != PartyUsersIndex)
		{
			GIOCP.DataSend(PartyUsersIndex, (LPBYTE)&pResult, pResult.h.size);
		}

		return;
	}

	BYTE bySlotNum = -1;
	int nEnterIllusionIndex = g_IT_Event.CheckEnterLevel(lpObj);
	BYTE byMapNumber = g_IT_Event.Find_EmptySlot(lpObj->PartyNumber, bySlotNum, nEnterIllusionIndex);

	if (byMapNumber > 6)
	{
		pResult.bReason = 14;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	BOOL bUpdateOk = g_IT_Event.Update_PartyInfo(byMapNumber, bySlotNum, lpObj->PartyNumber, lpObj->m_Index);

	if (bUpdateOk != 0)
	{
		sLog->outBasic("[ITR][Update_PartyInfo] ERRORcode:%d,NAME:%s,partyindex:%d",
			bUpdateOk, lpObj->Name, lpObj->PartyNumber);

		pResult.bReason = -1;
		memcpy(pResult.Name, lpObj->Name, MAX_ACCOUNT_LEN + 1);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	int dbnumber = 0;
	int useridx;

	for (int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		if (gParty.GetIndexUser(lpObj->PartyNumber, i, useridx, dbnumber) == FALSE)
		{
			pResult.bReason = 13;
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);
			return;
		}

		if (useridx > 0)
		{
			pResult.bReason = 0;
			memcpy(pResult.Name, lpObj->Name, MAX_ACCOUNT_LEN + 1);
			GIOCP.DataSend(useridx, (LPBYTE)&pResult, pResult.h.size);
		}
	}
	*/
}

void GameProtocol::CG_ReqAcceptEnterITR(PMSG_REQ_ACCEPTENTER_ITR *lpMsg, CGameObject *lpObj)
{
	/*
	_ITR_AcceptEnter pResult;
	PHeadSubSetB((LPBYTE)&pResult, 0xBF, 0x71, sizeof(pResult));
	memset(pResult.Name, 0x00, sizeof(pResult.Name));

	int byOkCount = 0;
	int UserIndex;
	int dbnumber;
	BYTE byEnterOK = lpMsg->byEnterOK;

	if (gParty.IsParty(lpObj->PartyNumber) == FALSE)
	{
		sLog->outBasic("[ ITR ][CGReqAcceptEnterITR] reqman is not party Error Name :%s,", lpObj->Name);
		g_IT_Event.Delete_PartyInfo(lpObj->PartyNumber);
		gParty.ClearPartyAuthITR(lpObj->PartyNumber);
		return;
	}

	if (byEnterOK == TRUE)
	{
		sLog->outBasic("[ ITR ][CGReqAcceptEnterITR] reqman opposite Name :%s,", lpObj->Name);
		g_IT_Event.Delete_PartyInfo(lpObj->PartyNumber);
		gParty.ClearPartyAuthITR(lpObj->PartyNumber);

		pResult.byError = -1;
		memcpy(pResult.Name, lpObj->Name, MAX_ACCOUNT_LEN + 1);
		GIOCP.DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (gParty.EnterITR_PartyAuth(lpObj->PartyNumber, lpObj->m_Index) == FALSE)
	{
		sLog->outBasic("[ ITR ][CGReqAcceptEnterITR][EnterITR_PartyAuth] Internal Error Name :%s", lpObj->Name);
		g_IT_Event.Delete_PartyInfo(lpObj->PartyNumber);
		gParty.ClearPartyAuthITR(lpObj->PartyNumber);

		pResult.byError = -1;
		memcpy(pResult.Name, lpObj->Name, MAX_ACCOUNT_LEN + 1);
		GIOCP.DataSend(lpObj->m_Index, (LPBYTE)&pResult, pResult.h.size);
		return;
	}

	if (gParty.AllAgreeEnterITR(lpObj->PartyNumber) == FALSE)
	{
		sLog->outBasic("[ ITR ] [CGReqAcceptEnterITR] Enter Agree NAME:%s, G.NAME:%s,AgreeCnt:%d",
			lpObj->Name, lpObj->m_PlayerData->GuildName, gParty.ReturnOKCountITR(lpObj->PartyNumber));
		return;
	}

	int TroubleUserIndex = -1;
	BOOL bResult = EnterITRCheckUser(lpObj, TroubleUserIndex);

	if (bResult != 0)
	{
		pResult.byError = bResult;
		memcpy(pResult.Name, getGameObject(TroubleUserIndex)->Name, MAX_ACCOUNT_LEN + 1);
		int nAllPartyUserIndex;

		for (int j = 0; j < MAX_USER_IN_PARTY; j++)
		{
			gParty.GetIndexUser(lpObj->PartyNumber, j, nAllPartyUserIndex, dbnumber);
			GIOCP.DataSend(nAllPartyUserIndex, (LPBYTE)&pResult, pResult.h.size);
		}

		g_IT_Event.Delete_PartyInfo(lpObj->PartyNumber);
		gParty.ClearPartyAuthITR(lpObj->PartyNumber);
		return;
	}

	for (int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		gParty.GetIndexUser(lpObj->PartyNumber, i, UserIndex, dbnumber);

		if (UserIndex != -1)
		{
			if (gParty.m_PartyS[lpObj->PartyNumber].bITRWithYouOk[i] == FALSE)
			{
				continue;
			}

			BYTE byITRMapIndex = -1;
			BYTE bySlotNum = -1;
			BOOL bReturn = g_IT_Event.Find_ExistingPartyInfo(lpObj->PartyNumber, byITRMapIndex, bySlotNum);

			if (bReturn)
			{
				sLog->outBasic("[ ITR ] [Find_ExistingPartyInfo]ERROR return:%d Name:%s, mapnumber:%d, ITLMapIndex:%d,bySlotNum:%d ",
					bReturn, lpObj->Name, lpObj->MapNumber, byITRMapIndex, bySlotNum);

				return;
			}

			if (g_IT_Event.Enter_ITR(*lpObj, byITRMapIndex, bySlotNum) == TRUE)
			{
				CGameObject* lpObjUI = getGameObject(UserIndex);
				pResult.byError = 0;
				memcpy(pResult.Name, lpObjUI->Name, MAX_ACCOUNT_LEN + 1);
				GIOCP.DataSend(lpObjUI->m_PlayerData->ConnectUser->Index, (LPBYTE)&pResult, pResult.h.size);

				lpObjUI->MapNumber = byITRMapIndex + 45;
				gObjMoveGate(*lpObjUI, byITRMapIndex + 142);
			}
		}
	}*/
}

BYTE GameProtocol::EnterITRCheckUser(CGameObject *lpObj, int &TroubleUserIndex)
{
	/*int PartyLeaderITRIndex = g_IT_Event.CheckEnterLevel(lpObj);

	if (PartyLeaderITRIndex == -1)
	{
		return 5;
	}

	int useridx = 0;
	int dbnumber = 0;

	for (int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		BYTE bReturn = gParty.GetIndexUser(lpObj->PartyNumber, i, useridx, dbnumber);
		TroubleUserIndex = useridx;
		CGameObject* lpObjTrouble = getGameObject(useridx);

		if (bReturn == FALSE)
		{
			return 13;
		}

		if (useridx > 0)
		{
			if (lpObjTrouble->MapNumber != MAP_INDEX_LORENMARKET &&
				IT_MAP_RANGE(lpObjTrouble->MapNumber) == FALSE)
			{
				return 10;
			}

			if (lpObjTrouble->PartyNumber != lpObj->PartyNumber)
			{
				return 13;
			}

			bReturn = g_IT_Event.CheckEnterLevel(lpObjTrouble);

			if (PartyLeaderITRIndex != bReturn)
			{
				return 5;
			}

			if (g_IT_Event.CheckWearingMOPH(useridx) == TRUE)
			{
				return 7;
			}

			if (lpObjTrouble->m_IfState.use && lpObjTrouble->m_IfState.type != 12)
			{
				return 11;
			}

			if (lpObjTrouble->m_PK_Level >= 4)
			{
				return 12;
			}

			if (gObjDuelCheck(*lpObjTrouble) == TRUE)
			{
				return 15;
			}

			if (lpObjTrouble->m_PlayerData->lpGuild != NULL &&
				lpObjTrouble->m_PlayerData->lpGuild->WarState == TRUE)
			{
				return 16;
			}

			if (lpObjTrouble->pntInventory[lpObjTrouble->m_btInvenPetPos]->m_Type == ITEMGET(13, 3) ||
				lpObjTrouble->pntInventory[lpObjTrouble->m_btInvenPetPos]->m_Type == ITEMGET(13, 2) ||
				lpObjTrouble->pntInventory[lpObjTrouble->m_btInvenPetPos]->m_Type == ITEMGET(13, 37))
			{
				return 17;
			}
		}
	}
	*/
	return 0;
}

void GameProtocol::GCSendRareItemPriceInfo(CGameObject* lpObj)
{
	PMSG_RAREITEMPIRCESEND pMsg;
	pMsg.h.set((LPBYTE)&pMsg, 0xFA, 0xA4, sizeof(pMsg));

	pMsg.btItemSellPriceDivisor = g_ConfigRead.itemPrices.btItemSellPriceDivisor;
	pMsg.iJewelOfBlessPrice = g_ConfigRead.itemPrices.iJewelOfBlessPrice;
	pMsg.iJewelOfSoulPrice = g_ConfigRead.itemPrices.iJewelOfSoulPrice;;
	pMsg.iJewelOfChaosPrice = g_ConfigRead.itemPrices.iJewelOfChaosPrice;
	pMsg.iCrestOfMonarchPrice = g_ConfigRead.itemPrices.iCrestOfMonarchPrice;
	pMsg.iJewelOfCreationPrice = g_ConfigRead.itemPrices.iJewelOfCreationPrice;
	pMsg.iLochFeatherPrice = g_ConfigRead.itemPrices.iLochFeatherPrice;
	pMsg.iWereRabbitEggPrice = g_ConfigRead.itemPrices.iWereRabbitEggPrice;
	pMsg.iJewelOfGuardianPrice = g_ConfigRead.itemPrices.iJewelOfGuardianPrice;
	pMsg.iJewelOfLifePrice = g_ConfigRead.itemPrices.iJewelOfLifePrice;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));
}

void GameProtocol::GCSendDisableReconnect(CGameObject* lpObj)
{
	PMSG_DISABLE_RECONNECT pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0xFA, sizeof(pMsg));
	pMsg.subcode = 0xA5;

	for (int i = 0; i < 100; i++)
	{
		pMsg.Trash[i] = rand() % 0xFF;
	}

	lpObj->m_dwDCTimer = GetTickCount64();

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));

}
void GameProtocol::GCSendDisableReconnectSystem(CGameObject* lpObj)
{
	PMSG_DISABLE_RECONNECT pMsg;

	pMsg.h.set((LPBYTE)&pMsg, 0xFA, sizeof(pMsg));
	pMsg.subcode = 0xA7;

	for (int i = 0; i < 100; i++)
	{
		pMsg.Trash[i] = rand() % 0xFF;
	}

	//lpObj->m_dwDCTimer = GetTickCount64();

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));

}
void GameProtocol::CGReqUBFMyCharacterInfo(PMSG_REQ_UBF_INFO *lpMsg, CGameObject *lpObj)
{
	/*if (lpMsg->btUBFKind == 0)
	{
		g_UnityBattleField.GDReqCheckJoinedUnityBattleField(*lpObj, g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE, 0);
	}*/
}

void GameProtocol::CGReqUBFJoin(PMSG_REQ_UBF_JOIN *lpMsg, CGameObject *lpObj)
{
	/*if (lpMsg->btUBFKind == 0)
	{
		g_UnityBattleField.GDReqJoinUnityBattleField(*lpObj);
	}*/
}

void GameProtocol::CGReqUBFCancel(PMSG_REQ_UBF_CANCEL *lpMsg, CGameObject *lpObj)
{
	/*if (lpMsg->btCanceled == 1)
	{
		g_UnityBattleField.GDReqCancelUnityBattleField(*lpObj, 1);
	}*/
}

void GameProtocol::CGReqUBFGetRealName(PMSG_REQ_UBF_REAL_NAME *lpMsg, CGameObject *lpObj)
{
	/*if (lpMsg->btRequest == 1)
	{
		g_UnityBattleField.GDReqGetRealNameAndServerCode(*lpObj);
	}*/
}

void GameProtocol::CGReqUBFGetReward(PMSG_REQ_GET_REWARD *lpMsg, CGameObject *lpObj)
{
	
	//g_UnityBattleField.GDReqUBFGetReward(*lpObj, lpMsg->btBattleKind);
}

void GameProtocol::CGReqDSFSchedule(CGameObject *lpObj)
{

	if (g_ConfigRead.server.GetServerType() != SERVER_BATTLECORE)
	{
		return;
	}

	if (lpObj->MapNumber != MAP_INDEX_LORENMARKET)
	{
		return;
	}

	g_DevilSquareFinal.ReqDSFSchedule(*lpObj);
}

void GameProtocol::CGReqDSFEnter(CGameObject *lpObj)
{

	PMSG_ANS_DSF_ENTER pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xDB, 0x01, sizeof(pMsg));

	memset(pMsg.Name, 0x00, sizeof(pMsg.Name));
	pMsg.btDSFType = g_DevilSquareFinal.m_nDSF_TYPE;

	if (g_DevilSquareFinal.m_btDSFCurState != 0)
	{
		pMsg.btResult = 8;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	if (g_DevilSquareFinal.m_bEntrance == false)
	{
		pMsg.btResult = 8;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	if (gParty.GetLorenMarketUserCount(lpObj->PartyNumber) != 2)
	{
		sLog->outBasic("[DSF][CGReqDSFEnter][%s][%s][%s][%d] EnterError : Party User is not in RorenMarket",
			lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->m_PlayerData->m_RealNameOfUBF, lpObj->Name, lpObj->m_PlayerData->m_nServerCodeOfHomeWorld);

		pMsg.btResult = 10;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	if (gParty.GetPKPartyPenalty(lpObj->PartyNumber) >= 5)
	{
		pMsg.btResult = 12;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	int PartyUsersIndex = 0;
	BOOL bResult = this->EnterDSFCheckUser(lpObj, PartyUsersIndex);

	if (bResult != 0)
	{
		CGameObject* lpObjPartyUser = getGameObject(PartyUsersIndex);
		pMsg.btResult = bResult;
		memcpy(pMsg.Name, lpObjPartyUser->Name, MAX_ACCOUNT_LEN + 1);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);

		if (PartyUsersIndex != lpObj->m_Index)
		{
			GIOCP.DataSend(PartyUsersIndex, (LPBYTE)&pMsg, pMsg.h.size);
		}

		return;
	}

	BOOL bExistAlready = g_DevilSquareFinal.IsAlreadyExistUserInDSF(*lpObj);

	if (bExistAlready != FALSE)
	{
		g_DevilSquareFinal.DeletePartyByIndex(PartyUsersIndex);
	}

	if (g_DevilSquareFinal.m_nDSF_TYPE == 1)
	{
		BYTE btSlotNum = -1;
		BYTE btRet = g_DevilSquareFinal.Find_EmptyPartySlot(lpObj->PartyNumber, btSlotNum);

		if (btRet >= 0xFE)
		{
			pMsg.btResult = 14;
			memcpy(pMsg.Name, lpObj->Name, sizeof(pMsg.Name));
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);

			return;
		}

		BOOL bUpdateOk = g_DevilSquareFinal.SetParty(btSlotNum, 1, lpObj->PartyNumber, lpObj->m_Index);

		if (bUpdateOk != FALSE)
		{
			sLog->outBasic("[DSF][CGReqDSFEnter][%s][%s][%s][%d] ERROR code:%d",
				lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->m_PlayerData->m_RealNameOfUBF, lpObj->Name, lpObj->m_PlayerData->m_nServerCodeOfHomeWorld, bUpdateOk);

			pMsg.btResult = -1;
			memcpy(pMsg.Name, lpObj->Name, sizeof(pMsg.Name));
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);

			return;
		}

		int dbnumber = 0, useridx = -1;

		for (int i = 0; i < 2; i++)
		{
			BOOL bError = gParty.GetIndexUser(lpObj->PartyNumber, i, useridx, dbnumber);

			if (bError == FALSE)
			{
				pMsg.btResult = 13;
				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
				return;
			}

			if (useridx > 0)
			{
				pMsg.btResult = 0;
				memcpy(pMsg.Name, lpObj->Name, sizeof(pMsg.Name));
				GIOCP.DataSend(useridx, (LPBYTE)&pMsg, pMsg.h.size);

				sLog->outBasic("[DSF][CGReqDSFEnter][%s][%s][%s][%d] Request DSF Enter Success And Ask To Party Member, DSFType : %d",
					lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->m_PlayerData->m_RealNameOfUBF,
					lpObj->Name, lpObj->m_PlayerData->m_nServerCodeOfHomeWorld,
					g_DevilSquareFinal.m_nDSF_TYPE);
			}
		}
	}

	else if (g_DevilSquareFinal.m_nDSF_TYPE == 2)
	{
		int dbnumber = 0;
		int useridx = -1;
		WORD wServerCode1 = -1;
		WORD wServerCode2 = -1;
		char szUserName1[MAX_ACCOUNT_LEN + 1];
		char szUserName2[MAX_ACCOUNT_LEN + 1];

		for (int i = 0; i < 2; i++)
		{
			BOOL bError = gParty.GetIndexUser(lpObj->PartyNumber, i, useridx, dbnumber);
			CGameObject* lpObjUserIdx = getGameObject(useridx);

			if (bError == FALSE)
			{
				pMsg.btResult = 13;
				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
				return;
			}

			if (useridx > 0)
			{
				if (i == 0)
				{
					memcpy(szUserName1, lpObjUserIdx->m_PlayerData->m_NameConvertOfUBF.ConvertNameToRealName(lpObjUserIdx->Name), MAX_ACCOUNT_LEN + 1);
					wServerCode1 = lpObjUserIdx->m_PlayerData->m_NameConvertOfUBF.FindServerCodeAtCharacterName(lpObjUserIdx->Name);
				}

				else if (i == 1)
				{
					memcpy(szUserName2, lpObjUserIdx->m_PlayerData->m_NameConvertOfUBF.ConvertNameToRealName(lpObjUserIdx->Name), MAX_ACCOUNT_LEN + 1);
					wServerCode2 = lpObjUserIdx->m_PlayerData->m_NameConvertOfUBF.FindServerCodeAtCharacterName(lpObjUserIdx->Name);
				}
			}
		}

		BOOL bGoFinal = g_DevilSquareFinal.IsGoSemiFinalParty(szUserName1, szUserName2, wServerCode1, wServerCode2);

		if (bGoFinal == FALSE)
		{
			pMsg.btResult = 18;
			memcpy(pMsg.Name, lpObj->Name, sizeof(pMsg.Name));
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);

			return;
		}

		BYTE btSlotNum = -1;
		BYTE btRet = g_DevilSquareFinal.Find_EmptyPartySlot(lpObj->PartyNumber, btSlotNum);

		if (btRet >= 0xFE)
		{
			pMsg.btResult = 14;
			memcpy(pMsg.Name, lpObj->Name, sizeof(pMsg.Name));
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);

			return;
		}

		BOOL bUpdateOk = g_DevilSquareFinal.SetParty(btSlotNum, 2, lpObj->PartyNumber, lpObj->m_Index);

		if (bUpdateOk != FALSE)
		{
			sLog->outBasic("[DSF][CGReqDSFEnter][%s][%s][%s][%d] ERROR code:%d",
				lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->m_PlayerData->m_RealNameOfUBF, lpObj->Name, lpObj->m_PlayerData->m_nServerCodeOfHomeWorld, bUpdateOk);

			pMsg.btResult = -1;
			memcpy(pMsg.Name, lpObj->Name, sizeof(pMsg.Name));
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);

			return;
		}

		for (int i = 0; i < 2; i++)
		{
			BOOL bError = gParty.GetIndexUser(lpObj->PartyNumber, i, useridx, dbnumber);

			if (bError == FALSE)
			{
				pMsg.btResult = 13;
				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
				return;
			}

			if (useridx > 0)
			{
				pMsg.btResult = 0;
				memcpy(pMsg.Name, lpObj->Name, sizeof(pMsg.Name));
				GIOCP.DataSend(useridx, (LPBYTE)&pMsg, pMsg.h.size);

				sLog->outBasic("[DSF][CGReqDSFEnter][%s][%s][%s][%d] Request DSF Enter Success And Ask To Party Member, DSFType : %d",
					lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->m_PlayerData->m_RealNameOfUBF,
					lpObj->Name, lpObj->m_PlayerData->m_nServerCodeOfHomeWorld,
					g_DevilSquareFinal.m_nDSF_TYPE);
			}
		}
	}

	else if (g_DevilSquareFinal.m_nDSF_TYPE == 3)
	{
		int dbnumber = 0;
		int useridx = -1;
		WORD wServerCode1 = -1;
		WORD wServerCode2 = -1;
		char szUserName1[MAX_ACCOUNT_LEN + 1];
		char szUserName2[MAX_ACCOUNT_LEN + 1];

		for (int i = 0; i < 2; i++)
		{
			BOOL bError = gParty.GetIndexUser(lpObj->PartyNumber, i, useridx, dbnumber);
			CGameObject* lpObjUserIdx = getGameObject(useridx);

			if (bError == FALSE)
			{
				pMsg.btResult = 13;
				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
				return;
			}

			if (useridx > 0)
			{
				if (i == 0)
				{
					memcpy(szUserName1, lpObjUserIdx->m_PlayerData->m_NameConvertOfUBF.ConvertNameToRealName(lpObjUserIdx->Name), MAX_ACCOUNT_LEN + 1);
					wServerCode1 = lpObjUserIdx->m_PlayerData->m_NameConvertOfUBF.FindServerCodeAtCharacterName(lpObjUserIdx->Name);
				}

				else if (i == 1)
				{
					memcpy(szUserName2, lpObjUserIdx->m_PlayerData->m_NameConvertOfUBF.ConvertNameToRealName(lpObjUserIdx->Name), MAX_ACCOUNT_LEN + 1);
					wServerCode2 = lpObjUserIdx->m_PlayerData->m_NameConvertOfUBF.FindServerCodeAtCharacterName(lpObjUserIdx->Name);
				}
			}
		}

		BOOL bGoFinal = g_DevilSquareFinal.IsGoFinalParty(szUserName1, szUserName2, wServerCode1, wServerCode2);

		if (bGoFinal == FALSE)
		{
			pMsg.btResult = 18;
			memcpy(pMsg.Name, lpObj->Name, sizeof(pMsg.Name));
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);

			return;
		}

		BYTE btSlotNum = -1;
		BYTE btRet = g_DevilSquareFinal.Find_EmptyPartySlot(lpObj->PartyNumber, btSlotNum);

		if (btRet >= 0xFE)
		{
			pMsg.btResult = 14;
			memcpy(pMsg.Name, lpObj->Name, sizeof(pMsg.Name));
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);

			return;
		}

		BOOL bUpdateOk = g_DevilSquareFinal.SetParty(btSlotNum, 3, lpObj->PartyNumber, lpObj->m_Index);

		if (bUpdateOk != FALSE)
		{
			sLog->outBasic("[DSF][CGReqDSFEnter][%s][%s][%s][%d] ERROR code:%d",
				lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->m_PlayerData->m_RealNameOfUBF, lpObj->Name, lpObj->m_PlayerData->m_nServerCodeOfHomeWorld, bUpdateOk);

			pMsg.btResult = -1;
			memcpy(pMsg.Name, lpObj->Name, sizeof(pMsg.Name));
			GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);

			return;
		}

		for (int i = 0; i < 2; i++)
		{
			BOOL bError = gParty.GetIndexUser(lpObj->PartyNumber, i, useridx, dbnumber);

			if (bError == FALSE)
			{
				pMsg.btResult = 13;
				GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
				return;
			}

			if (useridx > 0)
			{
				pMsg.btResult = 0;
				memcpy(pMsg.Name, lpObj->Name, sizeof(pMsg.Name));
				GIOCP.DataSend(useridx, (LPBYTE)&pMsg, pMsg.h.size);

				sLog->outBasic("[DSF][CGReqDSFEnter][%s][%s][%s][%d] Request DSF Enter Success And Ask To Party Member, DSFType : %d",
					lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->m_PlayerData->m_RealNameOfUBF,
					lpObj->Name, lpObj->m_PlayerData->m_nServerCodeOfHomeWorld,
					g_DevilSquareFinal.m_nDSF_TYPE);
			}
		}
	}
}

int GameProtocol::EnterDSFCheckUser(CGameObject *lpObj, int & TroubleUserIndex)
{
	int useridx = -1;
	int dbnumber = 0;
	int nLorenMarketUserCount = 0;

	for (int i = 0; i < 2; i++)
	{
		BOOL bError = gParty.GetIndexUser(lpObj->PartyNumber, i, useridx, dbnumber);
		CGameObject* lpObjPartyUser = getGameObject(useridx);
		TroubleUserIndex = useridx;

		if (bError == FALSE)
		{
			return 13;
		}

		if (useridx > 0)
		{
			if (lpObjPartyUser->MapNumber == MAP_INDEX_LORENMARKET || lpObjPartyUser->MapNumber == MAP_INDEX_DEVILSQUARE_FINAL)
			{
				nLorenMarketUserCount++;
			}

			if (lpObjPartyUser->PartyNumber != lpObj->PartyNumber)
			{
				return 13;
			}

			if (lpObjPartyUser->Level < g_DevilSquareFinal.m_DSFUnitInfo.DSFMinLevel)
			{
				return 5;
			}

			if (g_DevilSquareFinal.CheckWearingMOPH(*lpObjPartyUser) == TRUE)
			{
				return 7;
			}

			if (lpObjPartyUser->m_IfState.use && lpObjPartyUser->m_IfState.type != 12)
			{
				return 11;
			}

			if (lpObjPartyUser->m_PK_Level >= 4)
			{
				return 12;
			}

			if (gObjDuelCheck(*lpObjPartyUser) == TRUE)
			{
				return 15;
			}

			if (lpObjPartyUser->m_PlayerData->lpGuild != NULL &&
				lpObjPartyUser->m_PlayerData->lpGuild->WarState == TRUE)
			{
				return 16;
			}
		}
	}

	return 0;
}

void GameProtocol::CGReqAcceptEnterDSF(PMSG_REQ_DSF_ACCEPTENTER *lpMsg, CGameObject *lpObj)
{
	PMSG_ANS_DSF_ACCEPTENTER pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xDB, 0x02, sizeof(pMsg));
	memset(pMsg.Name, 0x00, sizeof(pMsg.Name));

	int useridx = -1, dbnumber = 0;

	if (gParty.IsParty(lpObj->PartyNumber) == FALSE)
	{
		sLog->outBasic("[DSF][CGReqAcceptEnterDSF][%s][%s][%s][%d] reqman is not party Error",
			lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->m_PlayerData->m_RealNameOfUBF,
			lpObj->Name, lpObj->m_PlayerData->m_nServerCodeOfHomeWorld);

		g_DevilSquareFinal.DeleteParty(lpObj->PartyNumber);
		gParty.ClearPartyAuthDSF(lpObj->PartyNumber);
		return;
	}

	if (lpMsg->btEnterOK == TRUE)
	{
		sLog->outBasic("[DSF][CGReqAcceptEnterDSF][%s][%s][%s][%d] Enter Cancel...",
			lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->m_PlayerData->m_RealNameOfUBF,
			lpObj->Name, lpObj->m_PlayerData->m_nServerCodeOfHomeWorld);

		g_DevilSquareFinal.DeleteParty(lpObj->PartyNumber);
		gParty.ClearPartyAuthDSF(lpObj->PartyNumber);

		pMsg.btError = 24;
		memcpy(pMsg.Name, lpObj->Name, MAX_ACCOUNT_LEN + 1);

		for (int i = 0; i < 2; i++)
		{
			gParty.GetIndexUser(lpObj->PartyNumber, i, useridx, dbnumber);

			if (useridx > 0)
			{
				GIOCP.DataSend(useridx, (LPBYTE)&pMsg, pMsg.h.size);
			}
		}

		return;
	}

	sLog->outBasic("[DSF][CGReqAcceptEnterDSF][%s][%s][%s][%d] Enter OK...",
		lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->m_PlayerData->m_RealNameOfUBF,
		lpObj->Name, lpObj->m_PlayerData->m_nServerCodeOfHomeWorld);

	if (gParty.EnterDSF_PartyAuth(lpObj->PartyNumber, lpObj->m_Index) == FALSE)
	{
		sLog->outBasic("[DSF][CGReqAcceptEnterDSF][%s][%s][%s][%d] Internal Error!!!",
			lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->m_PlayerData->m_RealNameOfUBF,
			lpObj->Name, lpObj->m_PlayerData->m_nServerCodeOfHomeWorld);

		pMsg.btError = -1;
		memcpy(pMsg.Name, lpObj->Name, sizeof(pMsg.Name));
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	if (gParty.AllAgreeEnterDSF(lpObj->PartyNumber) == FALSE)
	{
		int AgreeCnt = gParty.DSFReturnOKCount(lpObj->PartyNumber);

		sLog->outBasic("[DSF[CGReqAcceptEnterDSF][%s][%s][%s][%d] Party Enter Agree AgreeCnt:%d",
			lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->m_PlayerData->m_RealNameOfUBF,
			lpObj->Name, lpObj->m_PlayerData->m_nServerCodeOfHomeWorld, AgreeCnt);

		return;
	}

	int TroubleUserIndex = -1;
	BOOL bResult = this->EnterDSFCheckUser(lpObj, TroubleUserIndex);

	if (bResult != FALSE)
	{
		pMsg.btError = bResult;
		memcpy(pMsg.Name, getGameObject(TroubleUserIndex)->Name, sizeof(pMsg.Name));

		int nAllPartyUserIndex;

		for (int i = 0; i < 2; i++)
		{
			gParty.GetIndexUser(lpObj->PartyNumber, i, nAllPartyUserIndex, dbnumber);
			GIOCP.DataSend(nAllPartyUserIndex, (LPBYTE)&pMsg, pMsg.h.size);
		}

		g_DevilSquareFinal.DeleteParty(lpObj->PartyNumber);
		gParty.ClearPartyAuthDSF(lpObj->PartyNumber);
		return;
	}

	for (int i = 0; i < 2; i++)
	{
		gParty.GetIndexUser(lpObj->PartyNumber, i, useridx, dbnumber);
		CGameObject* lpObjUserIDX = getGameObject(useridx);
		if (useridx != -1)
		{
			BYTE btSlotNum = -1;
			BOOL bReturn = g_DevilSquareFinal.Find_PartySlot(lpObj->PartyNumber, btSlotNum);

			if (bReturn != FALSE)
			{
				sLog->outBasic("[DSF][Find_ExistingPartyInfo][%s][%s][%s][%d] ERROR bReturn:%d, mapnumber:%d, btSlotNum:%d ",
					lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->m_PlayerData->m_RealNameOfUBF, lpObj->Name, lpObj->m_PlayerData->m_nServerCodeOfHomeWorld,
					bReturn, lpObj->MapNumber, btSlotNum);

				return;
			}

			BOOL bEnterOK = g_DevilSquareFinal.Enter_DSF(*lpObjUserIDX, btSlotNum);

			if (bEnterOK == FALSE)
			{
				g_DevilSquareFinal.DeleteParty(lpObj->PartyNumber);

				sLog->outBasic("[DSF][CGReqAcceptEnterDSF][%s][%s][%s][%d] Enter Error bReturn:%d, mapnumber:%d, btSlotNum:%d ",
					lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->m_PlayerData->m_RealNameOfUBF, lpObj->Name, lpObj->m_PlayerData->m_nServerCodeOfHomeWorld,
					bReturn, lpObj->MapNumber, btSlotNum);
			}

			else
			{
				pMsg.btError = 0;
				memcpy(pMsg.Name, lpObjUserIDX->Name, sizeof(pMsg.Name));
				GIOCP.DataSend(useridx, (LPBYTE)&pMsg, pMsg.h.size);

				lpObjUserIDX->MapNumber = MAP_INDEX_DEVILSQUARE_FINAL;
				gObjMoveGate(*lpObjUserIDX, 474);

				sLog->outBasic("[DSF][CGReqAcceptEnterDSF][%s][%s][%s][%d] Enter DSF - PartyNo:%d, btSlotNum:%d ",
					lpObjUserIDX->m_PlayerData->ConnectUser->AccountID, lpObjUserIDX->m_PlayerData->m_RealNameOfUBF,
					lpObjUserIDX->Name, lpObjUserIDX->m_PlayerData->m_nServerCodeOfHomeWorld,
					lpObjUserIDX->PartyNumber, btSlotNum);
			}
		}
	}
}

void GameProtocol::CGReqDSFGoFinalParty(PMSG_REQ_DSF_GO_FINAL_PARTY_INFO *lpMsg, CGameObject *lpObj)
{

	if (g_ConfigRead.server.GetServerType() != SERVER_BATTLECORE)
	{
		return;
	}

	BYTE sendBuf[4096];
	memset(sendBuf, 0x00, sizeof(sendBuf));

	PMSG_ANS_DSF_GO_FINAL_PARTY_INFO pMsg;
	DSF_GO_FINAL_PARTY pParty;

	int iSize = sizeof(PMSG_ANS_DSF_GO_FINAL_PARTY_INFO);
	int iCount = 0;

	if (lpMsg->btDSFType == 2)
	{
		for (int i = 0; i < 40; i++)
		{
			if (g_DevilSquareFinal.m_GoSemiFinalParty[i].btRank > 0)
			{
				memcpy(pParty.szUserName1, g_DevilSquareFinal.m_GoSemiFinalParty[i].szUserName1, MAX_ACCOUNT_LEN + 1);
				memcpy(pParty.szUserName2, g_DevilSquareFinal.m_GoSemiFinalParty[i].szUserName2, MAX_ACCOUNT_LEN + 1);
				pParty.btRank = g_DevilSquareFinal.m_GoSemiFinalParty[i].btRank;
				pParty.btDSFType = g_DevilSquareFinal.m_GoSemiFinalParty[i].btDSFType;
				pParty.iPoint = g_DevilSquareFinal.m_GoSemiFinalParty[i].iPoint;

				memcpy(&sendBuf[iSize], &pParty, sizeof(pParty));
				iSize += sizeof(pParty);
				iCount++;
			}
		}
	}

	if (lpMsg->btDSFType == 3)
	{
		for (int i = 0; i < 10; i++)
		{
			if (g_DevilSquareFinal.m_GoFinalParty[i].btRank > 0)
			{
				memcpy(pParty.szUserName1, g_DevilSquareFinal.m_GoFinalParty[i].szUserName1, MAX_ACCOUNT_LEN + 1);
				memcpy(pParty.szUserName2, g_DevilSquareFinal.m_GoFinalParty[i].szUserName2, MAX_ACCOUNT_LEN + 1);
				pParty.btRank = g_DevilSquareFinal.m_GoFinalParty[i].btRank;
				pParty.btDSFType = g_DevilSquareFinal.m_GoFinalParty[i].btDSFType;
				pParty.iPoint = g_DevilSquareFinal.m_GoFinalParty[i].iPoint;

				memcpy(&sendBuf[iSize], &pParty, sizeof(pParty));
				iSize += sizeof(pParty);
				iCount++;
			}
		}
	}

	if (lpMsg->btDSFType == 4)
	{
		if (g_DevilSquareFinal.m_WinnerParty.btRank > 0)
		{
			memcpy(pParty.szUserName1, g_DevilSquareFinal.m_WinnerParty.szUserName1, MAX_ACCOUNT_LEN + 1);
			memcpy(pParty.szUserName2, g_DevilSquareFinal.m_WinnerParty.szUserName2, MAX_ACCOUNT_LEN + 1);
			pParty.btRank = g_DevilSquareFinal.m_WinnerParty.btRank;
			pParty.btDSFType = g_DevilSquareFinal.m_WinnerParty.btDSFType;
			pParty.iPoint = g_DevilSquareFinal.m_WinnerParty.iPoint;

			memcpy(&sendBuf[iSize], &pParty, sizeof(pParty));
			iSize += sizeof(pParty);
			iCount++;
		}
	}

	PHeadSubSetW((LPBYTE)&pMsg, 0xDB, 0x03, iSize);
	pMsg.btCnt = iCount;
	pMsg.btDSFType = lpMsg->btDSFType;

	if (iCount == 0)
	{
		pMsg.btResult = 100;
	}

	else
	{
		pMsg.btResult = 0;
	}

	memcpy(&sendBuf, &pMsg, sizeof(pMsg));
	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, sendBuf, iSize);

	sLog->outBasic("[DSF][CGReqDSFGoFinalParty][%s][%s][%s][%d] ReqType:%d, ResultCount:%d",
		lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->m_PlayerData->m_RealNameOfUBF,
		lpObj->Name, lpObj->m_PlayerData->m_nServerCodeOfHomeWorld,
		lpMsg->btDSFType, iCount);
}

void GameProtocol::CGReqDSFGetReward(PMSG_REQ_DSF_GET_REWARD *lpMsg, CGameObject *lpObj)
{

	if (g_ConfigRead.server.GetServerType() != SERVER_BATTLECORE)
	{
		return;
	}

	if (lpMsg->btDSFType == 4)
	{
		g_DevilSquareFinal.GDReqGetReward(*lpObj);
	}
}

void GameProtocol::CGReqDoUsePopupType(CGameObject* lpObj)
{
	/*
	PMSG_ANS_USE_POPUP pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x26, sizeof(pMsg));

	pMsg.nPopupType = g_UnityBattleField.GetUBFPopupType();
	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);*/
}

void GameProtocol::GCSendEventBanner(CGameObject* lpObj, int iBannerType)
{
	PMSG_SEND_EVENT_BANNER pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0x4D, 0x18, sizeof(pMsg));

	pMsg.iBannerType = iBannerType;
	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
}

void GameProtocol::GCAlterPShopVault(CGameObject* lpObj, BYTE btValue)
{
	PMSG_ALTERPSHOPVAULT pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xFA, 0xA8, sizeof(pMsg));
	pMsg.type = btValue;
	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
}
void GameProtocol::GCSetCharColors(CGameObject* lpObj)
{
	PMSG_SET_CHAT_COLOR pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xFA, 0x12, sizeof(pMsg));

	memcpy(pMsg.btInfoMsg, g_ConfigRead.data.chatcolor.btInfoMsg, sizeof(pMsg.btInfoMsg));
	memcpy(pMsg.btErrorMsg, g_ConfigRead.data.chatcolor.btErrorMsg, sizeof(pMsg.btErrorMsg));
	memcpy(pMsg.btChatMsg, g_ConfigRead.data.chatcolor.btChatMsg, sizeof(pMsg.btChatMsg));
	memcpy(pMsg.btWhisperMsg, g_ConfigRead.data.chatcolor.btWhisperMsg, sizeof(pMsg.btWhisperMsg));
	memcpy(pMsg.btPartyMsg, g_ConfigRead.data.chatcolor.btPartyMsg, sizeof(pMsg.btPartyMsg));
	memcpy(pMsg.btGuildMsg, g_ConfigRead.data.chatcolor.btGuildMsg, sizeof(pMsg.btGuildMsg));
	memcpy(pMsg.btAllianceMsg, g_ConfigRead.data.chatcolor.btAllianceMsg, sizeof(pMsg.btAllianceMsg));
	memcpy(pMsg.btGensMsg, g_ConfigRead.data.chatcolor.btGensMsg, sizeof(pMsg.btGensMsg));
	memcpy(pMsg.btGMChatMsg, g_ConfigRead.data.chatcolor.btGMChatMsg, sizeof(pMsg.btGMChatMsg));

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
}

void GameProtocol::GCSendRuud(CGameObject* lpObj, int iRuud, int iObtainedRuud, bool bIsObtainedRuud)
{
	PMSG_RUUD pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xD0, 0xF1, sizeof(pMsg));

	pMsg.iTotalRuud = iRuud;
	pMsg.iObtainedRuud = iObtainedRuud;
	pMsg.btObtainNotiSend = bIsObtainedRuud;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
}

void GameProtocol::CGReqUseBoxInInventory(CGameObject* lpObj, PMSG_REQ_USE_BOX * aRecv)
{

	if (lpObj->m_IfState.use != 0)
	{
		return;
	}

	if (lpObj->m_PlayerData->m_bSecurityCheck == false)
	{
		return;
	}

	if (lpObj->m_PlayerData->ConnectUser->CloseType != -1)
	{
		return;
	}

	if (lpObj->m_PlayerData->m_bMapSvrMoveQuit == true)
	{
		sLog->outBasic("[ANTI-HACK][Item Duplication] - Use Box during MapServer Move [%s][%s]", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
		return;
	}

	PMSG_ANS_USE_BOX pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xD0, 0xF2, sizeof(pMsg));

	if (CheckInventoryEmptySpace(*lpObj, 4, 4) == FALSE)
	{
		pMsg.iResult = -2;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	if (lpObj->m_PlayerData->Money >= 1900000000)
	{
		pMsg.iResult = -3;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	CItemObject* Item;
	time_t Duration;
	int iResult = 0;
	int iBoxType = lpObj->pntInventory[aRecv->btPos]->m_Type;

	switch (iBoxType)
	{
	case ITEMGET(14, 282):
		iResult = g_BagManager.GetItemFromBag(*lpObj, BAG_EVENT, EVENTBAG_MASTERYBOX_MINOR, lpObj->m_Index, *Item, Duration);
		break;
	case ITEMGET(14, 283):
		iResult = g_BagManager.GetItemFromBag(*lpObj, BAG_EVENT, EVENTBAG_MASTERYBOX_STANDARD, lpObj->m_Index, *Item, Duration);
		break;
	case ITEMGET(14, 284):
		iResult = g_BagManager.GetItemFromBag(*lpObj, BAG_EVENT, EVENTBAG_MASTERYBOX_GREATER, lpObj->m_Index, *Item, Duration);
		break;
	case ITEMGET(14, 287):
		iResult = g_BagManager.GetItemFromBag(*lpObj, BAG_EVENT, EVENTBAG_MASTERYCHEST_2000, lpObj->m_Index, *Item, Duration);
		break;
	case ITEMGET(14, 288):
		iResult = g_BagManager.GetItemFromBag(*lpObj, BAG_EVENT, EVENTBAG_MASTERYCHEST_1000, lpObj->m_Index, *Item, Duration);
		break;
	case ITEMGET(14, 289):
		iResult = g_BagManager.GetItemFromBag(*lpObj, BAG_EVENT, EVENTBAG_MASTERYCHEST_500, lpObj->m_Index, *Item, Duration);
		break;
	case ITEMGET(14, 336):
		iResult = g_BagManager.GetItemFromBag(*lpObj, BAG_EVENT, EVENTBAG_MASTERYCHEST_100, lpObj->m_Index, *Item, Duration);
		break;
	case ITEMGET(14, 293):
		iResult = g_BagManager.GetItemFromBag(*lpObj, BAG_EVENT, EVENTBAG_ELEMENTALCAPSULE_NORMAL, lpObj->m_Index, *Item, Duration);
		break;
	case ITEMGET(14, 384):
		iResult = g_BagManager.GetItemFromBag(*lpObj, BAG_EVENT, EVENTBAG_RUNECAPSULE, lpObj->m_Index, *Item, Duration);
		break;/*
	case ITEMGET(14, 367):
		iResult = g_BagManager.GetItemFromBag(lpObj, BAG_EVENT, EVENTBAG_ROOMY_CARD_LOWER, lpObj->m_Index, Item, Duration);
		break;
	case ITEMGET(14, 368):
		iResult = g_BagManager.GetItemFromBag(lpObj, BAG_EVENT, EVENTBAG_ROOMY_CARD_MEDIUM, lpObj->m_Index, Item, Duration);
		break;
	case ITEMGET(14, 369):
		iResult = g_BagManager.GetItemFromBag(lpObj, BAG_EVENT, EVENTBAG_ROOMY_CARD_HIGHER, lpObj->m_Index, Item, Duration);
		break;
	case ITEMGET(14, 296):
		iResult = g_BagManager.GetItemFromBag(lpObj, BAG_EVENT, EVENTBAG_ELEMENTALCAPSULE_NORMAL, lpObj->m_Index, Item, Duration);
		break;
	case ITEMGET(14, 338):
		iResult = g_BagManager.GetItemFromBag(lpObj, BAG_EVENT, EVENTBAG_RUNECAPSULE, lpObj->m_Index, Item, Duration);
		break;
	case ITEMGET(14, 337):
		iResult = g_BagManager.GetItemFromBag(lpObj, BAG_EVENT, EVENTBAG_ROOMY_CARD_LOWER, lpObj->m_Index, Item, Duration);
		break;
	case ITEMGET(14, 339):
		iResult = g_BagManager.GetItemFromBag(lpObj, BAG_EVENT, EVENTBAG_ROOMY_CARD_MEDIUM, lpObj->m_Index, Item, Duration);
		break;
	case ITEMGET(14, 340):
		iResult = g_BagManager.GetItemFromBag(lpObj, BAG_EVENT, EVENTBAG_ROOMY_CARD_HIGHER, lpObj->m_Index, Item, Duration);
		break;*/
	case ITEMGET(13, 350):
		iResult = g_BagManager.GetItemFromBag(*lpObj, BAG_EVENT, EVENTBAG_MUUNEGG, lpObj->m_Index, *Item, Duration);
		break;		
	default:
		iResult = 0;
		break;
	}

	if (iResult == 0)
	{
		pMsg.iResult = -1;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	gObjInventoryDeleteItem(*lpObj, aRecv->btPos);
	GSProtocol.GCInventoryItemDeleteSend(lpObj, aRecv->btPos, 0);

	if (iResult == 2 || iResult == 3)
	{
		pMsg.iResult = 0;
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	pMsg.iResult = 2;
	pMsg.iItemType = Item->m_Type;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
	ItemCreate(lpObj, 235, 0, 0, Item->m_Type, Item->m_Level, Item->m_Durability, Item->m_Option1,
		Item->m_Option2, Item->m_Option3, lpObj->m_Index, Item->m_NewOption, Item->m_SetOption, Duration, Item->m_SocketOption, Item->m_BonusSocketOption);
}

void GameProtocol::CGRecvHitHackValues(CGameObject* lpObj, PMSG_SEND_HITHACK_INFO * lpMsg)
{

	bool bMismatchFound = false;
	bool bIsAllValues1Equal = false;
	bool bIsAllValues2Equal = false;

	if (lpObj->m_PlayerData->Dexterity > g_ConfigRead.data.common.iHitHackMaxAgility)
	{
		lpObj->m_PlayerData->dwLastHitHackTick = GetTickCount64();
		return;
	}

	DWORD dwCount = lpObj->m_PlayerData->dwHitHackCounter1;

	if ((dwCount + 1) % 7 != 0)
	{
		lpObj->m_PlayerData->dwLastHitHackValue1[dwCount] = lpMsg->dwHitHackVal1;
		lpObj->m_PlayerData->dwLastHitHackValue2[dwCount] = lpMsg->dwHitHackVal2;
		lpObj->m_PlayerData->dwHitHackCounter1++;
	}
	else
	{
		lpObj->m_PlayerData->dwHitHackCounter1 = 0;
		dwCount = 0;

		lpObj->m_PlayerData->dwLastHitHackValue1[6] = lpMsg->dwHitHackVal1;
		lpObj->m_PlayerData->dwLastHitHackValue2[6] = lpMsg->dwHitHackVal2;

	}

	for (int i = 0; i < 7 - 1; i++)
	{
		if (lpObj->m_PlayerData->dwLastHitHackValue1[i] != lpObj->m_PlayerData->dwLastHitHackValue1[i + 1])
		{
			bMismatchFound = true;
		}
	}

	if (!bMismatchFound)
	{
		if (lpObj->m_PlayerData->dwLastHitHackValue1[0] != 0)
		{
			bIsAllValues1Equal = true;
		}
	}

	bMismatchFound = false;

	for (int i = 0; i < 7 - 1; i++)
	{
		if (lpObj->m_PlayerData->dwLastHitHackValue2[i] != lpObj->m_PlayerData->dwLastHitHackValue2[i + 1])
		{
			bMismatchFound = true;
		}
	}

	if (!bMismatchFound)
	{
		if (lpObj->m_PlayerData->dwLastHitHackValue2[0] = !0)
		{
			bIsAllValues2Equal = true;
		}
	}

	if (bIsAllValues1Equal || bIsAllValues2Equal)
	{
		sLog->outError("[AntiHack][%s][%s][%s] Hit Hack detected", lpObj->m_PlayerData->ConnectUser->IP, lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);

		GSProtocol.GCSendDisableReconnect(lpObj);
	}

	lpObj->m_PlayerData->dwLastHitHackTick = GetTickCount64();

}

// event entry notify

void GameProtocol::CGEventEntryNotice(int iEventType, BYTE state)
{
	if (iEventType >= EVENT_NOTIFICATION_MAX)
	{
		return;
	}

	PMSG_EVENTENTRY_NOTICE pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF1, 0xFE, sizeof(PMSG_EVENTENTRY_NOTICE));

	pMsg.btEventType = iEventType;
	pMsg.btNoticeState = state;

	for each (std::pair<int,CGameObject*> user in gGameObjects)
	{

		if (user.second->Type == OBJ_USER && user.second->EventNotification[iEventType] != state)
		{
			user.second->EventNotification[iEventType] = state;
			GIOCP.DataSend(user.second->m_PlayerData->ConnectUser->Index, (unsigned char*)&pMsg, pMsg.h.size);
		}

	}
}

void GameProtocol::CGReqNixieBossEnter(PMSG_REQ_NIXIELAKE_ENTER *lpMsg, CGameObject *lpObj)
{
	if (g_ConfigRead.NixieLakeEntrance == false)
	{
		MsgOutput(*lpObj, "Nix Event is Closed");
		return;
	}

	PMSG_ANS_NIXIELAKE_ENTER pMsg;

	for (int pos = INVETORY_WEAR_SIZE; pos < MAIN_INVENTORY_SIZE; pos++)
	{
		if (lpObj->pntInventory[pos]->IsItem() && lpObj->pntInventory[pos]->m_Type == ITEMGET(14, 390) && lpObj->pntInventory[pos]->m_Durability == 5.0f)
		{
			gObjInventoryDeleteItem(*lpObj, pos);
			this->GCInventoryItemDeleteSend(lpObj, pos, 1);
			gObjMoveGate(*lpObj, 531);
			sLog->outBasic("[%s][%s] Entered to Nixie Boss Map", lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
			return;
		}
	}

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x76;
	pMsg.h.subcode = 0x02;
	pMsg.btResult = 1;
	GIOCP.DataSend(lpObj->m_Index, (LPBYTE)&pMsg, sizeof(pMsg));

}

void GameProtocol::GCPlayerStatsPanelNew(CGameObject* lpObj)
{
	PMSG_ADDSTATS_RESULT pStatsInfo;
	PHeadSubSetB((LPBYTE)&pStatsInfo, 0xEC, 0x25, sizeof(pStatsInfo));
	pStatsInfo.str = lpObj->m_PlayerData->Strength;
	pStatsInfo.stradd = lpObj->AddStrength;
	pStatsInfo.agi = lpObj->m_PlayerData->Dexterity;
	pStatsInfo.agiadd = lpObj->AddDexterity;
	pStatsInfo.vit = lpObj->m_PlayerData->Vitality;
	pStatsInfo.vitadd = lpObj->AddVitality;
	pStatsInfo.ene = lpObj->m_PlayerData->Energy;
	pStatsInfo.eneadd = lpObj->AddEnergy;
	pStatsInfo.cmd = lpObj->Leadership;
	pStatsInfo.cmdadd = lpObj->AddLeadership;
	pStatsInfo.mPrec = lpObj->m_PlayerData->m_MPSkillOpt->iMpsAutoRecoverMana + lpObj->m_PlayerData->m_WingExcOption->iWingOpRecoveryMana + lpObj->m_PlayerData->m_JewelOfHarmonyEffect->HJOpAddRefillMP + lpObj->m_PlayerData->m_btRefillMPSocketOption;

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pStatsInfo, sizeof(pStatsInfo));
}

// NUEVO STATS PANEL POR %
struct SendPlayerInfoNewPanel
{
	PBMSG_HEAD h;
	DWORD unk;
	DWORD unk1;
	WORD Strength;
	WORD AddSetAccStrength;
	WORD Agility;
	WORD AddSetAccAgility;
	WORD Stamina;
	WORD AddSetAccStamina;
	WORD Energy;
	WORD AddSetAccEnergy;
	WORD Command;
	WORD AddSetAccCommand;
	WORD CritDMGInc;
	WORD ExcDMBInc;
	WORD SkillAtkDMGInc;
	WORD BasicDef;
	WORD SDWhenAtckd;
	WORD SDIgnoreRate;
	WORD SDWhenAtk;
	WORD ZenObincRate;
	float DefIgnRate;
	float HPRecRate;
	float MPRecRate;
	float StunRate;
	float StunResRate;
	BYTE Desconosido;
	float ShieldAbsorb;
	float MobAtkHPRec;
	float MobAtkMPRec;
	float MobAtkSDRec;
	float SDRecRate;
	float ResAllMPRate;
	float ResAllHPRate;
	float RessAllSDRate;
	float AGRecRate;
	float DMGAbsorbRate;
	float ShieldBlockRate;
	float WeaponBlockRate;
	float HPAbsorb;
	float SDAbsorb;
	float ReturnDmgRate;
	float DmgRedcRate;
	float MPUseRedcRate;
	float CritDMGRate;
	float ExcDMGRate;
	float DoubleDmgRate;
	BYTE DMGRedcRate;
	BYTE AGUsageRedRate;
	BYTE DMGReflcRate;
};

void GameProtocol::GCPlayerStatsPanelRates(CGameObject* lpObj)
{

	PMSG_STATS_RESULT pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xEC, 0x29, sizeof(pMsg));
	pMsg.CriticalDMGInc = lpObj->m_PlayerData->m_MPSkillOpt->iMpsIncCriticalDamageRate + lpObj->m_PlayerData->SetOpAddCriticalDamageSuccessRate + lpObj->m_PlayerData->SetOpAddCriticalDamage + lpObj->m_PlayerData->m_JewelOfHarmonyEffect->HJOpAddCriticalDamage; // crit dmg inc
	pMsg.unk1 = lpObj->m_PlayerData->m_MPSkillOpt->iMpsAddPhysicDamage + lpObj->m_PlayerData->m_MPSkillOpt->iMpsElfAddPhysicDamage + lpObj->m_PlayerData->m_JewelOfHarmonyEffect->HJOpAddSkillAttack + lpObj->m_PlayerData->SetOpAddSkillAttack; // skill attack dmg inc
	pMsg.str = lpObj->m_PlayerData->Strength;
	pMsg.stradd = lpObj->AddStrength;
	pMsg.agi = lpObj->m_PlayerData->Dexterity;
	pMsg.agiadd = lpObj->AddDexterity;
	pMsg.vit = lpObj->m_PlayerData->Vitality;
	pMsg.vitadd = lpObj->AddVitality;
	pMsg.ene = lpObj->m_PlayerData->Energy;
	pMsg.eneadd = lpObj->AddEnergy;
	pMsg.cmd = lpObj->Leadership;
	pMsg.cmdadd = lpObj->AddLeadership;
	pMsg.ExcellentDMGInc = 27; // 
	pMsg.ExcellentDMGInc2 = 37; // 
	pMsg.SDRation = lpObj->m_PlayerData->m_JewelOfHarmonyEffect->HJOpAddSDRate; // sd(%) when attack
	pMsg.SDBypass = lpObj->m_PlayerData->m_JewelOfHarmonyEffect->HJOpAddIgnoreSDRate; // sd % when attack
	pMsg.unk4 = lpObj->m_PlayerData->m_MPSkillOpt->iMpsIncIgnoreEnemyBlock + lpObj->m_PlayerData->SetOpIgnoreDefense + lpObj->m_PlayerData->m_WingExcOption->iWingOpIgnoreEnemyDefense; // def ign rate
	pMsg.unk5 = lpObj->AutoHPRecovery + lpObj->m_PlayerData->m_MPSkillOpt->iMpsAutoRecoverLife + lpObj->m_PlayerData->m_btRefillHPSocketOption + lpObj->m_PlayerData->m_JewelOfHarmonyEffect->HJOpAddRefillHP + lpObj->m_PlayerData->m_WingExcOption->iWingOpRecoveryHP; // hp rec rate
	pMsg.ZenObincRate = 12;
	pMsg.unk7 = lpObj->m_PlayerData->m_MPSkillOpt->iMpsAddSturn; // stun rate
	pMsg.unk8 = lpObj->m_PlayerData->m_Resistance_Stun; // stun res rate
	pMsg.unk9 = 45678;
	pMsg.unk10 = 0; // shield anbsorb
	pMsg.unk11 = lpObj->m_PlayerData->m_MPSkillOpt->iMpsMonsterDieGetLife; // mob attack hp rec
	pMsg.unk12 = lpObj->m_PlayerData->m_MPSkillOpt->iMpsMonsterDieGetMana;; // mob attack mp rec 
	pMsg.unk13 = lpObj->m_PlayerData->m_MPSkillOpt->iMpsMonsterDieGetSD; // mob attack sd rec
	pMsg.unk14 = lpObj->m_PlayerData->m_MPSkillOpt->iMpsSDSpeed; // sd rec rate
	pMsg.unk15 = lpObj->m_PlayerData->m_MPSkillOpt->iMpsRecoverManaRate; // res all mp rate
	pMsg.unk16 = lpObj->m_PlayerData->m_MPSkillOpt->iMpsRecoverHPRate; // res all hp rate
	pMsg.unk17 = lpObj->m_PlayerData->m_MPSkillOpt->iMpsRecoverSDRate; // res all sd rate
	pMsg.unk18 = lpObj->m_PlayerData->m_MPSkillOpt->iMpsAutoRecoverAG + lpObj->m_PlayerData->SetOpIncAGValue; // ag rec rate
	pMsg.unk19 = 0; // dmg absorb rate
	pMsg.unk20 = lpObj->m_PlayerData->m_MPSkillOpt->iMpsShieldBlockRate + lpObj->m_PlayerData->m_MPSkillOpt->iMpsElfShieldBlockRate + lpObj->m_PlayerData->m_MPSkillOpt->iMpsAddShieldBlockingRage; // shield block rate
	pMsg.unk21 = 0; // weapon block rate
	pMsg.unk22 = 0; // hp absorb
	pMsg.unk23 = lpObj->m_PlayerData->m_JewelOfHarmonyEffect->HJOpDecreaseSDRate; // sd absorb
	pMsg.unk24 = lpObj->m_PlayerData->m_MPSkillOpt->iMpsReturnEnemyAttack + lpObj->m_PlayerData->m_WingExcOption->iWingOpReturnEnemyDamage; // return dmg rate
	pMsg.IncreaseDamageRate = 0; // dmg inc rate
	pMsg.unk26 = lpObj->m_SkillInfo->SoulBarrierManaRate; // MP use redc rate
	pMsg.CritDMGRate = lpObj->m_CriticalDamage + lpObj->m_PlayerData->SetOpAddCriticalDamage; // Critical Dmg Rate
	pMsg.ExcDMGRate = lpObj->m_ExcelentDamage + lpObj->m_PlayerData->SetOpAddExDamageSuccessRate; // Excellent Dmg Rate
	pMsg.DoubleDmgRate = lpObj->m_PlayerData->m_MPSkillOpt->iMpsIncDoubleDamageRate + lpObj->m_PlayerData->m_MPSkillOpt->iMpsIncDarkSpiritDoubleDamageRate + lpObj->m_PlayerData->m_MPSkillOpt->iMpsAddDoubleDamage + lpObj->m_PlayerData->SetOpDoubleDamage + lpObj->m_PlayerData->m_MPSkillOpt->iMpsMonkDoubleDamage + lpObj->m_PlayerData->m_WingExcOption->iWingOpAddDoubleDamage; // 
#if(CUSTOM_DAMAGES12 == 1)
	pMsg.unk28 = lpObj->DamageDecrease + lpObj->S12DamageDecrease + lpObj->m_PlayerData->m_JewelOfHarmonyEffect->HJOpAddDamageDecrease; // lpObj->DamageMinus
#else	
	pMsg.unk28 = lpObj->DamageDecrease + lpObj->m_PlayerData->m_JewelOfHarmonyEffect->HJOpAddDamageDecrease; // lpObj->DamageMinus
#endif
	pMsg.unk29 = lpObj->m_PlayerData->m_btAGReduceRate; // ag usage rec rate
	pMsg.unk30 = lpObj->DamageReflect + lpObj->m_PlayerData->SetOpReflectionDamage; // damage reflect

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}


void GameProtocol::CGReqHuntingLog(CGameObject *lpObj)
{
	PMSG_ANS_HUNTING_LOG pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xEC, 0x50, sizeof(pMsg));
	pMsg.unk1 = 1;
	pMsg.unk2 = 125;
	pMsg.unk3 = 300;
	pMsg.unk4 = 400;

	sLog->outBasic("Hunting Log in Test");

}

void GameProtocol::CGSaveMuBotSettings(PMSG_MUBOT_SETTINGS_RECV* lpMsg, CGameObject* lpObj)
{
	DGMuBotSettingsSend(lpObj, lpObj->Name, &lpMsg->btDATA[0]);

	sLog->outBasic("[MuHelper] Option Save [%s][%s]",lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
}

void GameProtocol::GCSendMuBotSettings(CGameObject* lpObj, BYTE* lpData)
{

	PMSG_MUBOT_SETTINGS_RECV pMsg;
	PHeadSetW((LPBYTE)&pMsg, 0xAE, sizeof(pMsg));

	pMsg.btType = 0;
	memcpy(pMsg.btDATA, lpData, sizeof(pMsg.btDATA));

	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, sizeof(pMsg));

	sLog->outBasic("[MuHelper] Option Send [%s][%s]",lpObj->m_PlayerData->ConnectUser->AccountID, lpObj->Name);
}

void GameProtocol::CGRequestStartMuBot(PMSG_MUBOT_REQ_START* lpMsg, CGameObject* lpObj)
{

	PMSG_MUBOT_USE pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xBF, 0x51, sizeof(pMsg));
	pMsg.Money = 0;
	pMsg.Time = 0;
	pMsg.Status = 1;

	if (lpObj->m_IfState.use == TRUE) 
	{
		return;
	}

	if (lpObj->Level < g_ConfigRead.data.mubot.minlevel) 
	{
		MsgOutput(*lpObj, "Only characters above %d level can run MU Helper", g_ConfigRead.data.mubot.minlevel);
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		return;
	}

	if (lpObj->MapNumber == MAP_INDEX_CASTLESIEGE || lpObj->MapNumber == MAP_INDEX_VULCANROOM)
	{
		MsgOutput(*lpObj, "MU Helper cant run on this location");
		GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
		lpObj->m_bMUBOT = FALSE;
		return;
	}

	if (lpMsg->btType == 0) 
	{
		if (lpObj->m_PlayerData->Money < 10000)
		{
			pMsg.Status = 2;
		}
		else 
		{
			lpObj->m_PlayerData->Money -= 10000;
			GCMoneySend(lpObj, lpObj->m_PlayerData->Money);
			pMsg.Status = 0;
			pMsg.Time = 0;
			pMsg.Money = 10000;
			lpObj->m_bMUBOT = TRUE;
		}
	}
	else 
	{
		lpObj->m_bMUBOT = FALSE;
	}

	lpObj->m_iMUBOT_TIME = 0;
	lpObj->m_iMUBOT_STAGE = 0;
	GIOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (LPBYTE)&pMsg, pMsg.h.size);
}

