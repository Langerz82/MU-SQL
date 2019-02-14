
#include "GameProtocol.h"
#include "Event.h"
#include "Main.h"
#include "TNotice.h"
#include "QuestInfo.h"
#include "ConMember.h"
#include "GameServer.h"
#include "GMMng.h"
#include "BattleSoccerManager.h"
#include "BuffEffectSlot.h"
#include "ChaosBox.h"
#include "CastleSiegeSync.h"
#include "TUnion.h"
#include "JewelMixSystem.h"
#include "Weapon.h"
#include "ImperialGuardian.h"
#include "MasterLevelSkillTreeSystem.h"
#include "KalimaGate.h"
#include "ObjUseSkill.h"
#include "ObjCalCharacter.h"
#include "NpcTalk.h"
#include "Shop.h"
#include "PeriodItemEx.h"
#include "BattleSoccer.h"
#include "BattleSoccerManager.h"
#include "gObjMonster.h"
#include "ComboAttack.h"
#include "Gate.h"
#include "MoveCommand.h"
#include "MultiAttackHackCheck.h"
#include "ItemAddOption.h"
#include "JewelOfHarmonySystem.h"
#include "PacketCheckSum.h"
#include "DarkSpirit.h"
#include "Crywolf.h"
#include "CrywolfAltar.h"
#include "CrywolfSync.h"
#include "DevilSquare.h"
#include "KanturuEntranceNPC.h"
#include "CashShop.h"
#include "PentagramSystem.h"
#include "IllusionTempleEvent_Renewal.h"
#include "Sprotocol.h"
#include "Mercenary.h"
#include "ItemSocketOptionSystem.h"
#include "Guardian.h"
#include "DoppelGanger.h"
#include "AntiSwear.h"
#include "ChaosCastle.h"
#include "ChaosCastleFinal.h"
#include "GamblingSystem.h"
#include "configread.h"
//#include "bcore.h"
#include "GensSystem.h"
#include "GensSystemProtocol.h"
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
#include "Authorization.h"
#include "PersonalStore.h"
#include "MuunSystem.h"
#include "MuRummyMng.h"
#include "KeyGenerater.h"
#include "AcheronGuardianEvent.h"
#include "GremoryCase.h"
#include "DevilSquareFinal.h"
#include "BotSystem.h"
#include "ExpManager.h"
#include "EvolutionMonsterMng.h"
#include "RuudStore.h"
#include "UserChatBlockMng.hpp"
#include "CustomMaxStats.h"
#include "ItemUpgradeJewel.h"
#include "SocketItemType.h"
#include "CustomMichi.h"

GameProtocol gGameProtocol;

GameProtocol::GameProtocol()
{
}

GameProtocol::~GameProtocol()
{
}

bool GameProtocol::PacketCheckTime(CGameObject &Obj)
{
	if ((GetTickCount() - lpObj.m_PacketCheckTime) < 300)
	{
		return false;
	}

	lpObj.m_PacketCheckTime = GetTickCount();
	return true;
}

bool GameProtocol::DataEncryptCheck(CGameObject &Obj, BYTE protoNum, BOOL Encrypt)
{
#ifndef EMU_NOCRYPT
	if (Encrypt == FALSE)
	{
		sLog->outBasic("Error-L1 : Not Encrypt %s %d", lpObj.AccountID, protoNum);
		//this->GCSendDisableReconnect(aIndex);
		IOCP.CloseClient(lppObj);
		return false;
	}
#endif
	return true;
}

void GameProtocol::ProtocolCore(BYTE protoNum, BYTE* aRecv, int aLen, CGameObject &Obj, BOOL Encrypt)
{

	if (gStalkProtocol)
	{
		if (gStalkProtocolId[0] == lpObj.AccountID[0])
		{
			if (gStalkProtocolId[1] == lpObj.AccountID[1])
			{
				if (!strcmp(gStalkProtocolId, lpObj.AccountID))
				{
					//g_Log.AddHeadHex("DATA RECV", aRecv, aLen); // TODO find equivalent
				}
			}
		}
	}
	//sLog->outError( "K2(ProtocolCore) : %x %x %x", aRecv[0], aRecv[1], aRecv[2]);
	if (protoNum == 0xF1 || protoNum == 0xF3)
	{
		switch (protoNum)
		{
		case 0x0E:
			if (this->DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE)
			{
				this->CGLiveClient((PMSG_CLIENTTIME *)aRecv, aIndex);
			}
			break;
		case 0xF1:
		{
			PMSG_DEFAULT2 * lpMsg2 = (PMSG_DEFAULT2 *)aRecv;

			switch (lpMsg2->subcode)
			{
			case 0x01:
				if (this->DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE)
				{

					this->CSPJoinIdPassRequest((PMSG_IDPASS *)aRecv, aIndex);
				}
				break;
			case 0x02:
				if (this->DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE)
				{
					if (lpObj.m_bMapSvrMoveQuit == false)
						this->CGClientCloseMsg((PMSG_CLIENTCLOSE *)aRecv, aIndex);
					else
					{
						//this->GCSendDisableReconnect(aIndex);
						IOCP.CloseClient(aIndex);
					}
				}
				break;
			case 0x03:
				if (this->DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE)
				{
					this->CGClientMsg((PMSG_CLIENTMSG *)aRecv, aIndex);
				}
				break;
			default:
				sLog->outError("UNKNOWN F1 PACKET: %x %x %x %x %x %x %x %x %x %x", aRecv[0], aRecv[1], aRecv[2], aRecv[3], aRecv[4], aRecv[5], aRecv[6], aRecv[7], aRecv[8], aRecv[9]);
				break;

			}
		}
		break;
		case 0xF3:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x00:
				DataServerGetCharListRequest(aIndex);
				break;
			case 0x01:
				this->CGPCharacterCreate((PMSG_CHARCREATE *)aRecv, aIndex);
				break;
			case 0x02:
				this->CGPCharDel((PMSG_CHARDELETE *)aRecv, aIndex);
				break;
			case 0x03:
				this->CGPCharacterMapJoinRequest((PMSG_CHARMAPJOIN *)aRecv, aIndex);
				break;
			case 0x06:
				this->CGLevelUpPointAdd((PMSG_LVPOINTADD *)aRecv, aIndex);
				break;
			case 0x10: // not used packet
				break;
			case 0x12:
				gObjMoveDataLoadingOK(*getGameObject(aIndex));
				break;
			case 0x15: // by drakelv & wizzy
			{
				//C1 20 EC 29 00 00 00 00 00 00 00 00 1C 00 00 00 14 00 00 00 19 00 00 00 0A 00 00 00 FF 00 00 00 
				PMSG_CHARCHECK_RESULT pMsg;

				PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x15, sizeof(pMsg));
				memcpy(pMsg.szName, lpObj.Name, MAX_ACCOUNT_LEN);
				pMsg.bIsFromOtherAccount = 0; // if i put 0x0e == 1 it pop's the selection screen

				IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);


			}
			break;
			case 0x21:
				this->CGReqUseTransformationRing((PMSG_REQ_USETRANSFORMATIONRING *)aRecv, aIndex);
				break;
			case 0x26:
				this->CGReqDoUsePopupType(aIndex);
				break;
			case 0x30:
				this->GCSkillKeyRecv((PMSG_SKILLKEY *)aRecv, aIndex);
				break;
			case 0x52:
				g_MasterLevelSkillTreeSystem.CGReqGetMasterLevelSkill((PMSG_REQ_MASTERLEVEL_SKILL *)aRecv, aIndex);
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
			this->PChatProc((PMSG_CHATDATA *)aRecv, aIndex);
			break;
		case 0x01:
			this->CGChatRecv((PMSG_CHATDATA_NUMBER *)aRecv, aIndex);
			break;
		case 0x02:
			this->CGChatWhisperRecv((PMSG_CHATDATA_WHISPER *)aRecv, aIndex);
			break;
		case 0x03:
			this->CGCheckMainRecv((PMSG_CHECK_MAINEXE_RESULT *)aRecv, aIndex);
			break;
		case 0x0E:
			this->CGLiveClient((PMSG_CLIENTTIME *)aRecv, aIndex);
			break;
		case 0xD7:
			this->PMoveProc((PMSG_MOVE *)aRecv, aIndex);
			break;
		case 0x10:
			this->RecvPositionSetProc((PMSG_POSISTION_SET *)aRecv, aIndex);
			break;
		case 0xDF:
			this->CGAttack((PMSG_ATTACK *)aRecv, aIndex);
			break;
		case 0x18:
			if (this->DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE)
			{
				this->CGActionRecv((PMSG_ACTION *)aRecv, aIndex);
			}
			break;
		case 0x19:
			if (this->DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE)
			{
				this->CGMagicAttack(aRecv, aIndex);
			}
			break;
		case 0x1B:
			this->CGMagicCancel((PMSG_MAGICCANCEL *)aRecv, aIndex);
			break;
		case 0x1C:
			if (this->DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE)
			{
				this->CGTeleportRecv((PMSG_TELEPORT *)aRecv, aIndex);
			}
			break;
		case 0x8E:
			if (aRecv[3] == 0x02)
			{
				this->CGReqMapMove((PMSG_REQ_MAPMOVE *)aRecv, aIndex);
			}
			break;
		case 0xB0:
			if (this->DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE)
			{
				this->CGTargetTeleportRecv((PMSG_TARGET_TELEPORT *)aRecv, aIndex);
			}
			break;
		case 0xD3:
			this->CGBeattackRecv(aRecv, aIndex, FALSE);
			break;
		case 0x1E:
			if (this->DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE)
			{
				this->CGDurationMagicRecv(aRecv, aIndex);
			}
			break;
		case 0x22:
			if (this->DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE)
			{
				this->CGItemGetRequest((PMSG_ITEMGETREQUEST *)aRecv, aIndex, aRecv[3], aRecv[4]);
			}
			break;
		case 0x23:
			this->CGItemDropRequest((PMSG_ITEMTHROW *)aRecv, aIndex, 0);
			break;
		case 0x24:
			this->CGInventoryItemMove((PMSG_INVENTORYITEMMOVE *)aRecv, aIndex);
			break;
		case 0x26:
			this->CGUseItemRecv((PMSG_USEITEM *)aRecv, aIndex);
			break;
		case 0x30:
			if (this->DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE)
			{
				this->CGTalkRequestRecv((PMSG_TALKREQUEST *)aRecv, aIndex);
			}
			break;
		case 0x31:
			this->CGCloseWindow(aIndex);
			break;
		case 0x32:
			if (this->DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE)
			{
				this->CGBuyRequestRecv((PMSG_BUYREQUEST *)aRecv, aIndex);
			}
			break;
		case 0x33:
			if (this->DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE)
			{
				CGSellRequestRecv((PMSG_SELLREQUEST *)aRecv, aIndex);
			}
			break;
		case 0x34:
			this->CGModifyRequestItem((PMSG_ITEMDURREPAIR *)aRecv, aIndex);
			break;
		case 0x36:
			if (this->DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE)
			{
				this->CGTradeRequestSend((PMSG_TRADE_REQUEST *)aRecv, aIndex);
			}
			break;
		case 0x37:
			this->CGTradeResponseRecv((PMSG_TRADE_RESPONSE *)aRecv, aIndex);
			break;
		case 0x3A:
			this->CGTradeMoneyRecv((PMSG_TRADE_GOLD *)aRecv, aIndex);
			break;
		case 0x3C:
			if (this->DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE)
			{
				this->CGTradeOkButtonRecv((PMSG_TRADE_OKBUTTON *)aRecv, aIndex);
			}
			break;
		case 0x3D:
			if (this->DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE)
			{
				this->CGTradeCancelButtonRecv(aIndex);
			}
			break;
		case 0x3F:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x01:
				g_PersonalStore.CGPShopReqSetItemPrice((PMSG_REQ_PSHOP_SETITEMPRICE *)aRecv, aIndex);
				break;
			case 0x02:
				g_PersonalStore.CGPShopReqOpen((PMSG_REQ_PSHOP_OPEN *)aRecv, aIndex);
				break;
			case 0x03:
				g_PersonalStore.CGPShopReqClose(aIndex);
				break;
			case 0x05:
				g_PersonalStore.CGPShopReqBuyList((PMSG_REQ_BUYLIST_FROM_PSHOP *)aRecv, aIndex);
				break;
			case 0x06:
				g_PersonalStore.CGPShopReqBuyItem(aRecv, aIndex);
				break;
			case 0x07:
				g_PersonalStore.CGPShopReqCloseDeal((PMSG_REQ_PSHOPDEAL_CLOSE *)aRecv, aIndex);
				break;
			}
		}
		break;
		case 0x40:
			if (this->DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE)
			{
				this->CGPartyRequestRecv((PMSG_PARTYREQUEST *)aRecv, aIndex);
			}
			break;
		case 0x41:
			if (this->DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE)
			{
				this->CGPartyRequestResultRecv((PMSG_PARTYREQUESTRESULT *)aRecv, aIndex);
			}
			break;
		case 0x42:
			this->CGPartyList(aIndex);
			break;
		case 0x43:
			if (this->PacketCheckTime(aIndex) == TRUE)
			{
				this->CGPartyDelUser((PMSG_PARTYDELUSER *)aRecv, aIndex, 1);
			}
			break;
		case 0x4A:
			this->CGRageAttack(aRecv, aIndex);
			break;
		case 0x4B:
			this->CGRageAttackRange((PMSG_RAGE_ATTACK_RANGE *)aRecv, aIndex);
			break;
		case 0x50:
			this->CGGuildRequestRecv((PMSG_GUILDJOINQ *)aRecv, aIndex);
			break;
		case 0x51:
			this->CGGuildRequestResultRecv((PMSG_GUILDQRESULT *)aRecv, aIndex);
			break;
		case 0x52:
			this->CGGuildListAll(aIndex);
			break;
		case 0x53:
			this->CGGuildDelUser((PMSG_GUILDDELUSER *)aRecv, aIndex);
			break;
		case 0x54:
			this->CGGuildMasterAnswerRecv((PMSG_GUILDMASTERANSWER *)aRecv, aIndex);
			break;
		case 0x55:
			this->CGGuildMasterInfoSave(aIndex, (PMSG_GUILDINFOSAVE *)aRecv);
			break;
		case 0x57:
			this->CGGuildMasterCreateCancel(aIndex);
			break;
		case 0x61:
			this->GCGuildWarRequestSendRecv((PMSG_GUILDWARSEND_RESULT *)aRecv, aIndex);
			break;
		case 0x66:
			this->GCGuildViewportInfo((PMSG_REQ_GUILDVIEWPORT *)aRecv, aIndex);
			break;
		case 0x6E: // chat black list
		{
			PMSG_DEFAULT2* tmpHeader = (PMSG_DEFAULT2*)aRecv;
			switch (tmpHeader->subcode)
			{
			case 1:
				UserChatBlockMng::CGRequestAddName(aIndex, (CGRequestUserChatBlockAdd*)aRecv);
				break;
			case 2:
				UserChatBlockMng::CGRequestDeleteName(aIndex, (CGRequestUserChatBlockDelete*)aRecv);
				break;
			default:
				sLog->outBasic("[DEBUG] (UserChatBlockMng) not handled case: %X", tmpHeader->subcode);
				break;
			}
		}
		break;
		case 0x6F:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x00:
				g_CancelItemSale.CGReqSoldItemList(aIndex);
				break;
			case 0x01:
				g_CancelItemSale.CGReqEndCancelItemSale(aIndex);
				break;
			case 0x02:
				g_CancelItemSale.CGReqReBuyItem((PMSG_REQ_REBUY_ITEM *)aRecv, aIndex);
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
				if (this->DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE)
				{
					this->CGReqMapSvrAuth((PMSG_REQ_MAPSERVERAUTH *)aRecv, aIndex);
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
				this->CGReqCastleSiegeState((PMSG_REQ_CASTLESIEGESTATE *)aRecv, aIndex);
				break;
			case 0x01:
				this->CGReqRegCastleSiege((PMSG_REQ_REGCASTLESIEGE *)aRecv, aIndex);
				break;
			case 0x02:
				this->CGReqGiveUpCastleSiege((PMSG_REQ_GIVEUPCASTLESIEGE *)aRecv, aIndex);
				break;
			case 0x03:
				this->CGReqGuildRegInfo((PMSG_REQ_GUILDREGINFO *)aRecv, aIndex);
				break;
			case 0x04:
				this->CGReqRegGuildMark((PMSG_REQ_REGGUILDMARK *)aRecv, aIndex);
				break;
			case 0x05:
				this->CGReqNpcBuy((PMSG_REQ_NPCBUY *)aRecv, aIndex);
				break;
			case 0x06:
				this->CGReqNpcRepair((PMSG_REQ_NPCREPAIR *)aRecv, aIndex);
				break;
			case 0x07:
				this->CGReqNpcUpgrade((PMSG_REQ_NPCUPGRADE *)aRecv, aIndex);
				break;
			case 0x08:
				this->CGReqTaxMoneyInfo((PMSG_REQ_TAXMONEYINFO *)aRecv, aIndex);
				break;
			case 0x09:
				this->CGReqTaxRateChange((PMSG_REQ_TAXRATECHANGE *)aRecv, aIndex);
				break;
			case 0x10:
				this->CGReqMoneyDrawOut((PMSG_REQ_MONEYDRAWOUT *)aRecv, aIndex);
				break;
			case 0x12:
				this->CGReqCsGateOperate((PMSG_REQ_CSGATEOPERATE *)aRecv, aIndex);
				break;
			case 0x1B:
				this->CGReqCsMiniMapData((PMSG_REQ_MINIMAPDATA *)aRecv, aIndex);
				break;
			case 0x1C:
				this->CGReqStopCsMiniMapData((PMSG_REQ_STOPMINIMAPDATA *)aRecv, aIndex);
				break;
			case 0x1D:
				this->CGReqCsSendCommand((PMSG_REQ_CSCOMMAND *)aRecv, aIndex);
				break;
			case 0x1F:
				this->CGReqCsSetEnterHuntZone((PMSG_REQ_CSHUNTZONEENTER *)aRecv, aIndex);
				break;
			}
		}
		break;
		case 0xB3:
			this->CGReqNpcDbList((PMSG_REQ_NPCDBLIST *)aRecv, aIndex);
			break;
		case 0xB4:
			this->CGReqCsRegGuildList((PMSG_REQ_CSREGGUILDLIST *)aRecv, aIndex);
			break;
		case 0xB5:
			this->CGReqCsAttkGuildList((PMSG_REQ_CSATTKGUILDLIST *)aRecv, aIndex);
			break;
		case 0xB7:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x01:
				this->CGReqWeaponUse((PMSG_REQ_USEWEAPON *)aRecv, aIndex);
				break;
			case 0x04:
				this->CGReqWeaponDamageValue((PMSG_REQ_WEAPON_DAMAGE_VALUE *)aRecv, aIndex);
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
				this->CGReqGuildMarkOfCastleOwner((PMSG_REQ_GUILDMARK_OF_CASTLEOWNER *)aRecv, aIndex);
				break;
			case 0x05:
				CGReqCastleHuntZoneEntrance((PMSG_REQ_MOVE_TO_CASTLE_HUNTZONE *)aRecv, aIndex);
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
				this->CGReqJewelMix((PMSG_REQ_JEWEL_MIX *)aRecv, aIndex);
				break;
			case 0x01:
				this->CGReqJewelUnMix((PMSG_REQ_JEWEL_UNMIX *)aRecv, aIndex);
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
				this->CGReqCrywolfInfo((PMSG_REQ_CRYWOLF_INFO *)aRecv, aIndex);
				break;
			case 0x03:
				this->CGReqAlatrContract((PMSG_REQ_CRYWOLF_ALTAR_CONTRACT *)aRecv, aIndex);
				break;
			case 0x09:
				this->CGReqPlusChaosRate((PMSG_REQ_CRYWOLF_BENEFIT_PLUS_CHAOSRATE *)aRecv, aIndex);
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
				this->CGReqIllusionTempleUseSkill((PMSG_REQ_USEILLUSIONTEMPLESKILL *)aRecv, aIndex);
				break;
			case 0x05:
				this->GCIllusionTempleSendReward(aIndex);
				break;
			case 0x0B:
				this->CGReqLuckyCoinInfo(aIndex);
				break;
			case 0x0C:
				this->CGReqLuckyCoinRegister(reinterpret_cast<PMSG_REQ_LUCKYCOIN_REGISTER *>(aRecv), aIndex);
				break;
			case 0x0D:
				this->CGReqLuckyCoinTrade(reinterpret_cast<PMSG_REQ_LUCKYCOIN_TRADE *>(aRecv), aIndex);
				break;
			case 0x0E:
				this->CGReqEnterDoppelGanger(reinterpret_cast<PMSG_REQ_ENTER_DOPPELGANGER *>(aRecv), aIndex);
				break;
			case 0x17:
			{
				if (lpObj.MapNumber == MAP_INDEX_LORENMARKET)
				{
					gObjMoveGate(aIndex, 17);
				}
				else
				{
					gObjMoveGate(aIndex, 333);
				}
			}
			break;
			case 0x20:
				this->CGInventoryEquipment((PMSG_REQ_INVENTORY_EQUIPMENT_ITEM *)aRecv, aIndex);
				break;
			case 0x6A:
				this->CGReq_ITL_Relics((PMSG_REQ_ITL_RELATE_RELICS *)aRecv, aIndex);
				break;
			case 0x51:
				this->CGRequestStartMuBot((PMSG_MUBOT_REQ_START*)aRecv, aIndex);
				break;
			case 0x70:
				this->CG_Req_Enter_ITR((PMSG_REQ_ENTER_ITR *)aRecv, aIndex);
				break;
			case 0x71:
				this->CG_ReqAcceptEnterITR((PMSG_REQ_ACCEPTENTER_ITR *)aRecv, aIndex);
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
				this->CGReqUBFMyCharacterInfo((PMSG_REQ_UBF_INFO *)aRecv, aIndex);
				break;
			case 0x02:
				this->CGReqUBFJoin((PMSG_REQ_UBF_JOIN *)aRecv, aIndex);
				break;
			case 0x06:
				this->CGReqUBFGetReward((PMSG_REQ_GET_REWARD *)aRecv, aIndex);
				break;
			case 0x07:
				this->CGReqUBFCancel((PMSG_REQ_UBF_CANCEL *)aRecv, aIndex);
				break;
			case 0x08:
				this->CGReqUBFGetRealName((PMSG_REQ_UBF_REAL_NAME *)aRecv, aIndex);
				break;
			}
		}
		break;
		case 0xE1:
			this->CGGuildAssignStatus((PMSG_GUILD_ASSIGN_STATUS_REQ *)aRecv, aIndex);
			break;
		case 0xE2:
			this->CGGuildAssignType((PMSG_GUILD_ASSIGN_TYPE_REQ *)aRecv, aIndex);
			break;
		case 0xE5:
			CGRelationShipReqJoinBreakOff((PMSG_RELATIONSHIP_JOIN_BREAKOFF_REQ *)aRecv, aIndex);
			break;
		case 0xE6:
			this->CGRelationShipAnsJoinBreakOff((PMSG_RELATIONSHIP_JOIN_BREAKOFF_ANS *)aRecv, aIndex);
			break;
		case 0xE7:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x01:
				this->CGReqSendMemberPosInfoStart(aIndex);
				break;
			case 0x02:
				this->CGReqSendMemberPosInfoStop(aIndex);
				break;
			case 0x03:
				this->GCReqSendNpcPosInfo((PMSG_REQ_NPC_POSITION *)aRecv, aIndex);
				break;
			}
		}
		break;
		case 0xE9:
			this->CGUnionList((PMSG_UNIONLIST_REQ *)aRecv, aIndex);
			break;
		case 0xEB:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x01:
				this->CGRelationShipReqKickOutUnionMember((PMSG_KICKOUT_UNIONMEMBER_REQ *)aRecv, aIndex);
				break;
			}
		}
		break;
		case 0x71:
			this->GCPingSendRecv((PMSG_PING_RESULT *)aRecv, aIndex);
			break;
		case 0x72:
			if (this->DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE)
			{
				this->GCPacketCheckSumRecv((PMSG_PACKETCHECKSUM *)aRecv, aIndex);
			}
			break;
		case 0x78:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x01:
			{
				g_MixSystem.SocketItemUpgradeMixS12(aIndex, aRecv[4], aRecv[5], aRecv[14], aRecv[15], aRecv[16], aRecv[17], aRecv[18], aRecv[23], aRecv[24], aRecv[25], aRecv[26], aRecv[27], aRecv[32]);
				break;
			}
			case 0x02:
			{
				g_MixSystem.SeedExtractMixS12(aIndex, aRecv[4], aRecv[5], aRecv[6], aRecv[15], aRecv[24]);
				break;
			}
			case 0x03:
			{
				g_MixSystem.SetSeedSphereMixS12(aIndex, aRecv[4], aRecv[5], aRecv[6], aRecv[7], aRecv[16]);
				break;
			}
			case 0x04:
			{
				g_MixSystem.SeedSphereRemoveMixS12(aIndex, aRecv[4], aRecv[5], aRecv[6], aRecv[15], aRecv[16], aRecv[17], aRecv[18], aRecv[19], aRecv[24], aRecv[25], aRecv[26], aRecv[27], aRecv[28]);
				break;
			}
			case 0x05: //K2 Seed
			{
				g_MixSystem.SeedSphereCompositeMixS12(aIndex, aRecv[4], aRecv[5], aRecv[6], aRecv[7]);
				break;
			}
			case 0x06: //K2 Seed
			{
				g_MixSystem.SeedSphereEnhance(aIndex, aRecv[4], aRecv[5], aRecv[6]);
				break;
			}
			default:
				sLog->outError( "%X %X %X %X %X %X %X %X %X %X %X %X", aRecv[0], aRecv[1], aRecv[2], aRecv[3], aRecv[4], aRecv[5], aRecv[6], aRecv[7], aRecv[8], aRecv[9], aRecv[10], aRecv[11]);
			}
		}
		break;
		case 0x81:
			this->CGWarehouseMoneyInOut(aIndex, (PMSG_WAREHOUSEMONEYINOUT *)aRecv);
			break;
		case 0x82:
			this->CGWarehouseUseEnd(aIndex);
			break;
		case 0x83:
			this->GCWarehouseRecivePassword(aIndex, (PMSG_WAREHOUSEPASSSEND *)aRecv);
			break;
		case 0x86:
			this->CGChaosBoxItemMixButtonClick((PMSG_CHAOSMIX *)aRecv, aIndex);
			break;
		case 0x87:
			this->CGChaosBoxUseEnd(aIndex);
			break;
		case 0x88:
			g_MixSystem.CheckEmptySpace_MultiMix((PMSG_REQ_CHAOS_MULTIMIX_CHECK *)aRecv, aIndex);
			break;
		case 0x90:
			this->GCReqmoveDevilSquare((PMSG_REQ_MOVEDEVILSQUARE *)aRecv, aIndex);
			break;
		case 0x91:
			this->GCReqDevilSquareRemainTime((PMSG_REQ_DEVILSQUARE_REMAINTIME *)aRecv, aIndex);
			break;
		case 0x95:
			this->GCRegEventChipRecv((PMSG_REGEVENTCHIP *)aRecv, aIndex);
			break;
		case 0x96:
			this->GCGetMutoNumRecv((PMSG_GETMUTONUMBER *)aRecv, aIndex);
			break;
		case 0x97:
			this->GCUseEndEventChipRescv(aIndex);
			break;
		case 0x98:
			this->GCUseRenaChangeZenRecv((PMSG_EXCHANGE_EVENTCHIP *)aRecv, aIndex);
			break;
		case 0x99:
			this->CGReqMoveOtherServer((PMSG_REQ_MOVE_OTHERSERVER *)aRecv, aIndex);
			break;
		case 0xA0:
			this->CGRequestQuestInfo(aIndex);
			break;
		case 0xA2:
			this->CGSetQuestState((PMSG_SETQUEST *)aRecv, aIndex);
			break;
		case 0xA7:
			this->CGRequestPetItemCommand((PMSG_REQUEST_PET_ITEM_COMMAND *)aRecv, aIndex);
			break;
		//case 0xA9:
		//	this->CGRequestPetItemInfo((PMSG_REQUEST_PET_ITEMINFO *)aRecv, *getGameObject(aIndex));
		//	break;
		case 0xAA:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			WORD wPacketType = MAKEWORD(lpDef->subcode, 0xAA);
			switch (wPacketType)
			{
			case MSG_DUEL_INVITE:
			{
				PMSG_REQ_DUEL_INVITE* lpReq = (PMSG_REQ_DUEL_INVITE*)aRecv;
				int nRet = OnCGInviteDuel(lpReq, aIndex);
				if (nRet != ENEWPVP::E_NO_ERROR)
				{
					PMSG_ANS_DUEL_INVITE res = { 0 };
					res.h.c = 0xC1;
					res.h.size = sizeof(PMSG_ANS_DUEL_INVITE);
					res.h.headcode = 0xAA;
					res.h.subcode = 0x01;
					res.nResult = nRet;
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&res, res.h.size);
				}
			}
			break;

			case MSG_DUEL_ANSWER:
			{
				PMSG_ANS_DUEL_ANSWER* lpReq = (PMSG_ANS_DUEL_ANSWER*)aRecv;
				int nRet = OnCGAnswerDuel(lpReq, aIndex);
			}
			break;

			case MSG_DUEL_LEAVE:
			{
				PMSG_REQ_DUEL_EXIT* lpReq = (PMSG_REQ_DUEL_EXIT*)aRecv;
				int nRet = OnCGLeaveDuel(lpReq, aIndex);
				if (nRet != ENEWPVP::E_NO_ERROR)
				{
					PMSG_ANS_DUEL_EXIT res = { 0 };
					res.h.c = 0xC1;
					res.h.size = sizeof(PMSG_ANS_DUEL_EXIT);
					res.h.headcode = 0xAA;
					res.h.subcode = 0x03;
					res.nResult = nRet;
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&res, res.h.size);
				}
			}
			break;

			case MSG_DUEL_JOINCNANNEL:
			{
				int nRet = OnDuelChannelJoin((PMSG_REQ_DUEL_JOINCNANNEL*)aRecv, aIndex);
				if (nRet != ENEWPVP::E_NO_ERROR)
				{
					PMSG_ANS_DUEL_JOINCNANNEL res = { 0 };
					res.h.c = 0xC1;
					res.h.size = sizeof(PMSG_ANS_DUEL_JOINCNANNEL);
					res.h.headcode = 0xAA;
					res.h.subcode = 0x07;
					res.nResult = nRet;
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&res, res.h.size);
				}
			}
			break;

			case MSG_DUEL_LEAVECNANNEL:
			{
				PMSG_REQ_DUEL_LEAVECNANNEL* lpReq = (PMSG_REQ_DUEL_LEAVECNANNEL*)aRecv;
				int nRet = OnDuelChannelLeave(lpReq, aIndex);
				if (nRet != ENEWPVP::E_NO_ERROR)
				{
					PMSG_ANS_DUEL_LEAVECNANNEL res = { 0 };
					res.h.c = 0xC1;
					res.h.size = sizeof(PMSG_ANS_DUEL_LEAVECNANNEL);
					res.h.headcode = 0xAA;
					res.h.subcode = 0x09;
					res.nResult = nRet;
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&res, res.h.size);
				}
			}
			break;
			}
		}
		break;
		case 0x9A:
			this->CGRequestEnterBloodCastle((PMSG_REQ_MOVEBLOODCASTLE*)aRecv, aIndex);
			break;
		case 0x9F:
			this->CGRequestEventEnterCount((PMSG_REQ_CL_ENTERCOUNT *)aRecv, aIndex);
			break;
		case 0x9D:
			this->CGRequestLottoRegister((PMSG_REQ_2ANV_LOTTO_EVENT *)aRecv, aIndex);
			break;
		case 0xAE:
			CGSaveMuBotSettings((PMSG_MUBOT_SETTINGS_RECV*)aRecv, aIndex);
			break;
		case 0xAF:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x01:
				this->CGRequestEnterChaosCastle((PMSG_REQ_MOVECHAOSCASTLE *)aRecv, aIndex);
				break;
			case 0x02:
				this->CGRequestRepositionUserInChaosCastle((PMSG_REQ_REPOSUSER_IN_CC *)aRecv, aIndex);
				break;
			case 3:
				CGReqCCF_DayTime(aIndex);
				break;
			case 5:
				CGReqCCF_EnterCheck(aIndex);
				break;
			case 6:
				CGRequestRepositionUserInCCF((PMSG_REQ_REPOSUSER_IN_CCF *)aRecv, aIndex);
				break;
			case 7:
				CGReq_CCF_Ranking((PMSG_REQ_CCF_RANKING *)aRecv, aIndex);
				break;
			case 8:
				CG_CCF_UI_OnOff((PMSG_CCF_UI_ONOFF *)aRecv, aIndex);
				break;
			}
		}
		break;
		case 0xC0:
			FriendListRequest(aIndex);
			break;
		case 0xC1:
			FriendAddRequest((PMSG_FRIEND_ADD_REQ *)aRecv, aIndex);
			break;
		case 0xC2:
			WaitFriendAddRequest((PMSG_FRIEND_ADD_SIN_RESULT *)aRecv, aIndex);
			break;
		case 0xC3:
			FriendDelRequest((PMSG_FRIEND_DEL_REQ *)aRecv, aIndex);
			break;
		case 0xC4:
			FriendStateClientRecv((PMSG_FRIEND_STATE_C *)aRecv, aIndex);
			break;
		case 0xC5:
			FriendMemoSend((PMSG_FRIEND_MEMO *)aRecv, aIndex);
			break;
		case 0xC7:
			FriendMemoReadReq((PMSG_FRIEND_READ_MEMO_REQ *)aRecv, aIndex);
			break;
		case 0xC8:
			FriendMemoDelReq((PMSG_FRIEND_MEMO_DEL_REQ *)aRecv, aIndex);
			break;
		case 0xC9:
			FriendMemoListReq(aIndex);
			break;
		case 0xCA:
			FriendChatRoomCreateReq((PMSG_FRIEND_ROOMCREATE_REQ *)aRecv, aIndex);
			break;
		case 0xCB:
			FriendRoomInvitationReq((PMSG_ROOM_INVITATION *)aRecv, aIndex);
			break;
		case 0xD0:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x07: //WereWolf
				this->WereWolfQuarrelCheck(aIndex);
				break;
			case 0x08: //GateKeeper
				this->GateKeeperCheck(aIndex);
				break;
			case 0x0A: // Exit from Santa Village
				if (gGateC.CheckGateLevel(aIndex, 22) == true)
				{
					gObjMoveGate(aIndex, 22);
				}
				else
				{
					char szTemp[256];
					sprintf(szTemp, Lang.GetText(0, 211), gGateC.GetLevel(22));

					this->GCServerMsgStringSend(szTemp, aIndex, 1);
				}
				break;
			case 0x10: // Santa Clause
				EGReqSantaGift(aIndex);
				break;
			case 0xF0:
				g_RuudStore.CGReqBuyItem((PMSG_REQ_RUUD_STORE_BUYITEM *)aRecv, aIndex);
				break;
			case 0xF2:
				this->CGReqUseBoxInInventory(aIndex, (PMSG_REQ_USE_BOX *)aRecv);
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
				this->CGReqKanturuStateInfo((PMSG_REQ_KANTURU_STATE_INFO *)aRecv, aIndex);
				break;
			case 0x01:
				this->GCReqEnterKanturuBossMap((PMSG_REQ_ENTER_KANTURU_BOSS_MAP *)aRecv, aIndex);
				break;
			}
		}
		break;

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
				sLog->outError( "InGameShop Unknown Packet: %X %X", lpDef->h.headcode, lpDef->subcode);
			}
		}
		break;

		case 0xEC:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{

			case 0:
				this->CGReqInJewelPentagramItem((PMSG_REQ_IN_PENTAGRAM_JEWEL *)aRecv, aIndex);
				break;

			case 1:
				this->CGReqOutJewelPentagramItem((PMSG_REQ_OUT_PENTAGRAM_JEWEL *)aRecv, aIndex);
				break;

			case 2:
				g_PentagramMixSystem.CGPentagramJewelRefineRecv((PMSG_PENTAGRAM_JEWEL_REFINE_RECV*)aRecv, aIndex);
				break;
			case 3:
				this->CGReqUpgradePentagramJewel((PMSG_REQ_UPGRADE_PENTAGRAM_JEWEL *)aRecv, aIndex);
				break;
			case 5:
				int SlotRadiance;
				SlotRadiance = aRecv[8];
				g_Log.AddC(TColor::Yellow, "[K2] Radiance Slot # %d %d %d %d", aRecv[7], aRecv[8], aRecv[9], aRecv[10]);
				if (lpObj.pInventory[SlotRadiance].m_Type == ITEMGET(14, 316))	//ITEMGET(13, 47[5])) //Ticket BC
					g_Log.AddC(TColor::Yellow, "[K2] Add Radiance Slot", lpDef->h.headcode, lpDef->subcode);
				else
					g_Log.AddC(TColor::Yellow, "[K2] Wrong Radiance Expansion", lpDef->h.headcode, lpDef->subcode);
				//Add Radiance Slot [9] = Radiance Expansion item [4] = Pentagram Slot
				break;

			case 0x31:
				g_PersonalStore.CGReqSearchItemInPShop((PMSG_REQ_SEARCH_ITEM_PSHOP *)aRecv, aIndex);
				break;

			case 0x33:
				g_PersonalStore.CGReqPShopLog((PMSG_REQ_PSHOP_LOG *)aRecv, aIndex);
				break;

			default:
				sLog->outError( "%X %X %X %X %X %X %X %X %X %X %X %X", aRecv[0], aRecv[1], aRecv[2], aRecv[3], aRecv[4], aRecv[5], aRecv[6], aRecv[7], aRecv[8], aRecv[9], aRecv[10], aRecv[11]);
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
				this->CGReqDSFSchedule(aIndex);
				break;
			case 0x01:
				GDReqDSFCanPartyEnter(aIndex);
				break;
			case 0x02:
				this->CGReqAcceptEnterDSF((PMSG_REQ_DSF_ACCEPTENTER *)aRecv, aIndex);
				break;
			case 0x03:
				this->CGReqDSFGoFinalParty((PMSG_REQ_DSF_GO_FINAL_PARTY_INFO *)aRecv, aIndex);
				break;
			case 0x09:
				this->CGReqDSFGetReward((PMSG_REQ_DSF_GET_REWARD *)aRecv, aIndex);
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
				this->CGReqGuildMatchingList((PMSG_REQ_GUILDMATCHINGLIST *)aRecv, aIndex);
				break;
			case 1:
				this->CGReqGuildMatchingListSearchWord((PMSG_REQ_GUILDMATCHINGLISTSEARCHWORD *)aRecv, aIndex);
				break;
			case 2:
				this->CGReqRegGuildMatchingList((PMSG_REQ_REGGUILDMATCHINGDATA *)aRecv, aIndex);
				break;
			case 3:
				this->CGReqCancelGuildMatchingList((PMSG_REQ_CANCEL_GUILDMATCHINGLIST *)aRecv, aIndex);
				break;
			case 4:
				this->CGReqJoinGuildMatching((PMSG_REQ_JOIN_GUILDMATCHING *)aRecv, aIndex);
				break;
			case 5:
				this->CGReqCancelJoinGuildMatching(aIndex);
				break;
			case 6:
				this->CGReqAllowJoinGuildMatching((PMSG_REQ_ALLOW_JOIN_GUILDMATCHING *)aRecv, aIndex);
				break;
			case 7:
				this->CGReqGetAllowListJoinGuildMatching((PMSG_REQ_ALLOWLIST_GUILDMATCHING *)aRecv, aIndex);
				break;
			case 8:
				this->CGReqGetWaitStateListGuildMatching(aIndex);
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
				this->CGReqRegWantedPartyMember((PMSG_REQ_REG_WANTED_PARTYMEMBER *)aRecv, aIndex);
				break;
			case 1:
				this->CGReqGetPartyMatchingList((PMSG_REQ_GET_PARTYMATCHINGLIST *)aRecv, aIndex);
				break;
			case 2:
				this->CGReqJoinMemberPartyMatching((PMSG_REQ_MEMBERJOIN_PARTYMATCHINGLIST *)aRecv, aIndex);
				break;
			case 3:
				this->CGReqMemberJoinStateList((PMSG_REQ_MEMBERJOIN_STATELIST_PARTYMATCHING *)aRecv, aIndex);
				break;
			case 4:
				this->CGReqMemberJoinStateListLeader((PMSG_REQ_WAITLIST_PARTYMATCHING *)aRecv, aIndex);
				break;
			case 5:
				this->CGReqAcceptJoinMemberPartyMatching((PMSG_REQ_ACCEPTMEMBER_PARTYMATCHING *)aRecv, aIndex);
				break;
			case 6:
				this->CGReqCancelPartyMatching((PMSG_REQ_CANCEL_JOIN_PARTYMATCHING *)aRecv, aIndex);
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
				CGReqQuestExp((PMSG_REQ_NPC_QUESTEXP *)aRecv, aIndex);
				break;

			case 0x31:
				CGReqAttDefPowerInc((PMSG_REQ_ATTDEF_POWER_INC *)aRecv, aIndex);
				break;

			case 0x21:
				CGReqEventItemQuestList((PMSG_REQ_EVENT_ITEM_EP_LIST *)aRecv, aIndex);
				break;

			case 0x0A:
				CGReqQuestSwitch((PMSG_REQ_QUESTEXP *)aRecv, aIndex);
				break;

			case 0x0B:
				CGReqQuestProgress((PMSG_REQ_QUESTEXP *)aRecv, aIndex);
				break;

			case 0x0D:
				CGReqQuestComplete((PMSG_REQ_QUESTEXP_COMPLETE *)aRecv, aIndex);
				break;

			case 0x0F:
				CGReqQuestGiveUp((PMSG_REQ_QUESTEXP_GIVEUP *)aRecv, aIndex);
				break;

			case 0x1A:
				CGReqProgressQuestList((PMSG_REQ_QUESTEXP_PROGRESS_LIST *)aRecv, aIndex);
				break;

			case 0x1B:
				CGReqProgressQuestInfo((PMSG_REQ_QUESTEXP_PROGRESS_INFO *)aRecv, aIndex);
				break;

			case 0x10:
				CGReqTutorialKeyComplete((PMSG_REQ_QUESTEXP_ASK_COMPLETE *)aRecv, aIndex);
				break;

			default:
				sLog->outError( "UNKNOWN QUEST PROTOCOL (%x/%x/%x)", aRecv[2], aRecv[3], aRecv[4]);
				break;
			}
		}
		break;
		case 0x70:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			//sLog->outError( "rare Item ticket 0x01");

			//int Slot = aRecv[4];
			//CItemObject * lpItem = NULL;;
			//BOOL cont = false;
			//lpItem = &lpObj.pInventory[Slot];

			//::gObjInventoryItemSet(aIndex, Slot, -1);
			//lpObj.pInventory[Slot].Clear();
			//this->GCInventoryItemDeleteSend(aIndex, Slot, 1);

			//::ItemSerialCreateSend(aIndex, 235, 0, 0, ITEMGET(14, 13), 0, 0, 0, 0, 0, aIndex, 0, 0, 0, 0, 0);
		}
		break;
		case 0xF7:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x01:
				this->CGReqEnterZone((PMSG_REQ_ENTER_ZONE *)aRecv, aIndex);
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
				this->CGReqRegGensMember((PMSG_REQ_REG_GENS_MEMBER *)aRecv, aIndex);
				break;
			case 0x03:
				this->CGReqSecedeGensMember((PMSG_REQ_SEGEDE_GENS_MEMBER *)aRecv, aIndex);
				break;
			case 0x09:
				this->CGReqGensReward((PMSG_GENS_REWARD_CODE *)aRecv, aIndex);
				break;
			case 0x0B:
				this->CGReqGensMemberInfo((PMSG_REQ_GENS_INFO *)aRecv, aIndex);
				break;
			case 0x20:
				this->CGReqAcheronEnter((PMSG_REQ_ACHERON_ENTER *)aRecv, aIndex);
				break;
			case 0x30:
				this->CGReqArcaBattleGuildMasterJoin((PMSG_REQ_ARCA_BATTLE_JOIN *)aRecv, aIndex);
				break;
			case 0x32:
				this->CGReqArcaBattleGuildMemberJoin((PMSG_REQ_ARCA_BATTLE_JOIN *)aRecv, aIndex);
				break;
			case 0x34:
				this->CGReqArcaBattleEnter((PMSG_REQ_ARCA_BATTLE_ENTER *)aRecv, aIndex);
				break;
			case 0x36:
				this->CGReqArcaBattleBootyExchange(aIndex);
				break;
			case 0x3C:
				this->CGReqSpritemapExchange(aIndex);
				break;
			case 0x41:
				this->CGReqRegisteredMemberCnt(aIndex);
				break;
			case 0x43:
				g_ArcaBattle.CGReqMarkReg(aIndex);
				break;
			case 0x45:
				g_ArcaBattle.CGReqMarkRank(aIndex);
				break;
			case 0x4B:
				g_AcheronGuardianEvent.CGReqAcheronEventEnter((PMSG_REQ_ACHERON_EVENT_ENTER *)aRecv, aIndex);
				break;
			default:
				g_Log.AddC(TColor::Blue, "UNKNOWN F8 PACKET: %x %x %x %x %x %x %x %x %x %x", aRecv[0], aRecv[1], aRecv[2], aRecv[3], aRecv[4], aRecv[5], aRecv[6], aRecv[7], aRecv[8], aRecv[9]);
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
				this->CGReqAntiCheatRecv(aIndex, (PMSG_SEND_AH_INFO *)aRecv);
			}
			break;
			case 0x09:
			{
				gObjUseSkill.SkillChangeUse(lpObj.m_Index);
				DGGuildMemberInfoRequest(aIndex);
			}
			break;
			case 0x0A:
			{
				this->CGReqAntihackBreach(aIndex, (PMSG_ANTIHACK_BREACH *)aRecv);
			}
			break;
			case 0x0D:
			{
				this->CGReqFileCrcRecv(aIndex, (PMSG_REQ_CRCDATA *)aRecv);
			}
			break;
			case 0x11:
				this->CGReqAntihackCheck(aIndex, (PMSG_ANTIHACK_CHECK *)aRecv);
				break;
			case 0x15:
				this->CGRecvHitHackValues(aIndex, (PMSG_SEND_HITHACK_INFO*)aRecv);
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
				g_MineSystem.MineTwinkle((PMSG_REQ_MINETWINKLE *)aRecv, aIndex);
				break;
			case 0x01:
				g_MineSystem.RewardMineTwinkle((PMSG_REQ_MINETWINKLE_REWARD *)aRecv, aIndex);
				break;
			case 0x03:
				g_MineSystem.FailMineTwinkle((PMSG_ANS_MINETWINKLE_END_ANIMATION *)aRecv, aIndex, false);
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
				if (DataEncryptCheck(aIndex, protoNum, Encrypt))
				{
					this->CGEventItemGetRequest((PMSG_EVENTITEM_GETREQUEST *)aRecv, aIndex);
				}
				break;
			case 0x01:
				this->CGEventItemDropRequest((PMSG_EVENTITEM_THROW *)aRecv, aIndex);
				break;
			case 0x0F:
				this->CGReqEventInvenOpen(aRecv, aIndex);
				break;
			case 0x10:
				g_CMuRummyMng.CGReqMuRummyStart((PMSG_REQ_MURUMMY_INFO *)aRecv, aIndex);
				break;
			case 0x11:
				g_CMuRummyMng.CGReqCardReveal((PMSG_REQ_REVEAL_CARD *)aRecv, aIndex);
				break;
			case 0x12:
				g_CMuRummyMng.CGReqCardMove((PMSG_REQ_CARD_MOVE *)aRecv, aIndex);
				break;
			case 0x13:
				g_CMuRummyMng.CGReqCardReMove((PMSG_REQ_CARD_REMOVE *)aRecv, aIndex);
				break;
			case 0x14:
				g_CMuRummyMng.CGReqCardMatch((PMSG_REQ_CARD_MATCH *)aRecv, aIndex);
				break;
			case 0x15:
				g_CMuRummyMng.CGReqMuRummyEnd((PMSG_REQ_MURUMMY_END *)aRecv, aIndex);
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
				if (DataEncryptCheck(aIndex, protoNum, Encrypt))
				{
					this->CGMuunItemGetRequest((PMSG_MUUNITEM_GETREQUEST *)aRecv, aIndex);
				}
				break;
			case 0x08:
				g_CMuunSystem.CGMuunInventoryUseItemRecv((PMSG_USEITEM_MUUN_INVEN *)aRecv, aIndex);
				break;
			case 0x09:
				if (DataEncryptCheck(aIndex, protoNum, Encrypt))
				{
					CGMuunItemSellRequestRecv((PMSG_MUUN_ITEM_SELLREQUEST *)aRecv, aIndex);
				}
				break;
			case 0x11:
				g_CMuunSystem.CGReqRideSelect((PMSG_MUUN_RIDE_SELECT*)aRecv, aIndex);
				break;
			case 0x13:
				g_CMuunSystem.CGMuunExchangeItem((PMSG_REQ_MUUN_EXCHANGE *)aRecv, aIndex);
				break;
			}
		}
		break;
		case 0x35:
		{
			int Slot = aRecv[3];
			CItemObject * lpItem = NULL;;
			BOOL cont = false;
			lpItem = &lpObj.pInventory[Slot];
			if (lpItem->IsSetItem() != FALSE)
			{
				//return;
			}

			::gObjInventoryItemSet(aIndex, Slot, -1);
			lpObj.pInventory[Slot].Clear();
			this->GCInventoryItemDeleteSend(aIndex, Slot, 1);
			::ItemSerialCreateSend(aIndex, 235, 0, 0, ITEMGET(14, 290), 0, 0, 0, 0, 0, aIndex, 0, 0, 0, 0, 0);

		}

		case 0x4F:
		{
			PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

			switch (lpDef->subcode)
			{
			case 0x02:
				g_GremoryCase.CGReqGetItemFromGremoryCase((PMSG_ADD_GREMORYCASE_ITEM_TO_INVENTORY_REQ *)aRecv, aIndex);
				break;
			case 0x05:
				g_GremoryCase.CGReqOpenGremoryCase(aIndex);
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
				this->CGReqNixieBossEnter((PMSG_REQ_NIXIELAKE_ENTER *)aRecv, aIndex);
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
			//sLog->outError( "error-L2 (ProtocolCore) : Account: %s Name: %s : Invalid packet received OP: %X (%s,%d) Connection State: %d",
			//	lpObj.AccountID, lpObj.Name, protoNum, __FILE__, __LINE__, lpObj.Connected);
			this->GCSendDisableReconnect(aIndex);
			//IOCP.CloseClient(aIndex);
		}
	}
}

void GameProtocol::MsgSendV2(CGameObject &Obj, BYTE* Msg, int size)
{
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj.VpPlayer2[n].type == OBJ_USER)
		{
			if (lpObj.VpPlayer2[n].state != 0)
			{
				IOCP.DataSend(lpObj.VpPlayer2[n].number, Msg, size);
			}
		}
	}
}

void GameProtocol::CGLiveClient(PMSG_CLIENTTIME * lpClientTime, short aIndex)
{
#ifdef GS_AUTHORITY
	BYTE Decrypted[200];
	memcpy(Decrypted, lpClientTime, sizeof(PMSG_CLIENTTIME));
	//StringNarrow
	WLBufferDecrypt(&Decrypted[3], lpClientTime->h.size - 3, "12094389340236896570890436753");
	lpClientTime = (PMSG_CLIENTTIME *)Decrypted;
#endif

	if (lpObj.Connected > PLAYER_CONNECTED)
	{
		lpObj.ConnectCheckTime = GetTickCount();
	}

#ifndef GS_AUTHORITY
	return;
#endif

	if (lpObj.CheckSpeedHack != false)
	{
		DWORD dwTime = MAKE_NUMBERDW(lpClientTime->TimeH, lpClientTime->TimeL);

		int systemtick = GetTickCount() - lpObj.CheckTick2;
		int usertick = dwTime - lpObj.CheckTick;
		int checktime = systemtick - usertick;

		lpObj.CheckTickCount++;

		if (lpObj.CheckTickCount > 45)
		{
			lpObj.CheckTick = dwTime;
			lpObj.CheckTickCount = 0;
		}
	}

	if (lpObj.Connected == PLAYER_PLAYING && g_ConfigRead.m_CheckSpeedHack != 0 && lpClientTime->AttackSpeed > lpObj.m_AttackSpeed && abs((int)(lpObj.m_AttackSpeed - lpClientTime->AttackSpeed)) > g_ConfigRead.m_CheckSpeedHackTolerance)
	{
		sLog->outError( "[%s][%s] Client Speed Hack Detected [%d][%d]", lpObj.AccountID, lpObj.Name, lpObj.m_AttackSpeed, lpClientTime->AttackSpeed);
		IOCP.CloseClient(aIndex);
		return;
	}

	if (lpObj.Connected == PLAYER_PLAYING && g_ConfigRead.m_CheckSpeedHack != 0 && lpClientTime->MagicSpeed > lpObj.m_MagicSpeed && abs((int)(lpObj.m_MagicSpeed - lpClientTime->MagicSpeed)) > g_ConfigRead.m_CheckSpeedHackTolerance)
	{
		sLog->outError( "[%s][%s] Client Speed Hack Detected [%d][%d]", lpObj.AccountID, lpObj.Name, lpObj.m_MagicSpeed, lpClientTime->MagicSpeed);
		IOCP.CloseClient(aIndex);
		return;
	}

	/*if (lpObj.Connected == PLAYER_PLAYING)
	{
		if (g_ConfigRead.m_CheckSpeedHack == 1)
		{
			if ((lpObj.m_AttackSpeed + g_ConfigRead.m_CheckSpeedHackTolerance) < lpClientTime->AttackSpeed || (lpObj.m_MagicSpeed + g_ConfigRead.m_CheckSpeedHackTolerance) < lpClientTime->MagicSpeed)
			{
				sLog->outError( "[%s][%s][%s] ClientSpeedHack Detected : Editing AttackSpeed [%d][%d] [%d][%d]", lpObj.AccountID, lpObj.Name, lpObj.m_PlayerData->Ip_addr, lpObj.m_AttackSpeed, lpClientTime->AttackSpeed, lpObj.m_MagicSpeed, lpClientTime->MagicSpeed);

				lpObj.m_AttackSpeedHackDetectedCount++;

				if (lpObj.m_AttackSpeedHackDetectedCount >= g_ConfigRead.m_CheckSpeedHackLimit)
				{
					IOCP.CloseClient(aIndex);
				}
			}
			else
			{
				lpObj.m_AttackSpeedHackDetectedCount = 0;
			}
		}
	}*/
}




void GameProtocol::GCCheckMainExeKeySend(CGameObject &Obj)
{
	PMSG_CHECK_MAINEXE pMsg;

	lpObj.CheckSumTableNum = rand() % MAX_CHECKSUM_KEY;
	lpObj.CheckSumTime = GetTickCount();
	PHeadSetB((BYTE*)&pMsg, 0x03, sizeof(pMsg));
	pMsg.m_wKey = EncryptCheckSumKey(lpObj.CheckSumTableNum);

	//IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);
}





WORD GameProtocol::EncryptCheckSumKey(WORD wSource)
{
	WORD wRandom = rand() % 64;
	WORD wAcc = ((wSource & 0x3F0) * 64) | (wRandom * 16) | (wSource & 0x0F);
	return wAcc ^ 0xB479;
}




void GameProtocol::CGCheckMainRecv(PMSG_CHECK_MAINEXE_RESULT * lpMsg, CGameObject &Obj)
{
	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	if (!gCheckSumSwitch)
	{
		lpObj.CheckSumTime = 0;
		return;
	}

	if (lpObj.CheckSumTableNum < 0 || lpObj.CheckSumTableNum > MAX_CHECKSUM_KEY - 1)
	{
		sLog->outBasic("error-L1 : [%s][%s] CheckSum-Exe error", lpObj.AccountID, lpObj.Name);
		this->GCSendDisableReconnect(aIndex);
		//IOCP.CloseClient(aIndex);
		return;
	}

	if (dwgCheckSum[lpObj.CheckSumTableNum] != lpMsg->m_dwKey)
	{
		sLog->outBasic("error-L1 : CheckSum-Exe error %d %d %d [%s]", dwgCheckSum[lpObj.CheckSumTableNum],
			lpMsg->m_dwKey, lpObj.CheckSumTableNum, lpObj.AccountID);
		this->GCSendDisableReconnect(aIndex);
		//IOCP.CloseClient(aIndex);
		return;
	}

	lpObj.CheckSumTime = 0;
}





void GameProtocol::PEchoProc(BYTE * aMsg, int aLen, short aIndex)
{
	for (int n = 0; n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		if (getGameObject(n)->Connected >= PLAYER_CONNECTED)
		{
			IOCP.DataSend(n, aMsg, aLen);
		}
	}
}







void GameProtocol::GCResultSend(CGameObject &Obj, BYTE headcode, BYTE result)
{
	PMSG_DEFRESULT pResult;

	PHeadSetB((BYTE*)&pResult, headcode, sizeof(pResult));
	pResult.result = result;

	IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pResult, pResult.h.size);
}



void GameProtocol::ChatSend(CGameObject &Obj, char* szChat)
{
	int len = strlen(szChat);

	if (len < 1)
		return;

	if (len > MAX_CHAT_LEN - 1)
		return;

	PMSG_CHATDATA_NUMBER pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x01, len + 6);
	strcpy(pMsg.chatmsg, szChat);
	pMsg.NumberH = SET_NUMBERH(lpObj.m_Index);
	pMsg.NumberL = SET_NUMBERL(lpObj.m_Index);

	this->MsgSendV2(lpObj, (UCHAR*)&pMsg, pMsg.h.size);
}



void GameProtocol::vChatSend(CGameObject &Obj, char* szChat, ...)
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
		MultiByteToWideChar(1258, 0, szChat, len, (WORD*)szTempMsg, sizeof(szTempMsg));
		memcpy(&pMsg.szChatMsg, szTempMsg, sizeof(pMsg.szChatMsg));
		pMsg.szChatMsg[MAX_CHAT_LEN-2]=0;
		pMsg.szChatMsg[MAX_CHAT_LEN-1]=0;
		PHeadSetB((BYTE*)&pMsg, 0x01, wcslen((WORD*)pMsg.szChatMsg)*2 + (sizeof(GC_CHATSEND)-sizeof(pMsg.szChatMsg)));
		pMsg.sIndex[0] = lpObj.m_Index >> 8;
		pMsg.sIndex[1] = lpObj.m_Index & 0xFF;*/

	PMSG_CHATDATA_NUMBER pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x01, len + 6);
	strcpy(pMsg.chatmsg, szBuffer);
	pMsg.NumberH = SET_NUMBERH(lpObj.m_Index);
	pMsg.NumberL = SET_NUMBERL(lpObj.m_Index);

	this->MsgSendV2(lpObj, (UCHAR*)&pMsg, pMsg.h.size);
}





void GameProtocol::AllSendServerMsg(char* chatmsg)
{
	PMSG_NOTICE pNotice;

	TNotice::MakeNoticeMsg((TNotice*)&pNotice, 0x00, chatmsg);

	for (int n = g_ConfigRead.server.GetObjectStartUserIndex(); n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		if (getGameObject(n)->Connected == PLAYER_PLAYING)
		{
			if (getGameObject(n)->Type == OBJ_USER)
			{
				IOCP.DataSend(n, (BYTE*)&pNotice, pNotice.h.size);
			}
		}
	}
}



void GameProtocol::DataSendAll(BYTE* lpMsg, int iMsgSize)
{
	for (int n = g_ConfigRead.server.GetObjectStartUserIndex(); n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		if (getGameObject(n)->Connected == PLAYER_PLAYING)
		{
			if (getGameObject(n)->Type == OBJ_USER)
			{
				IOCP.DataSend(n, (BYTE*)lpMsg, iMsgSize);
			}
		}
	}
}




void GameProtocol::ChatTargetSend(CGameObject &Obj, char * szChat, int senduser)
{
	int len = strlen(szChat);

	if (len < 1 || len > MAX_CHAT_LEN - 1)
		return;

	PMSG_CHATDATA_NUMBER pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x01, len + 6);
	strcpy(pMsg.chatmsg, szChat);
	pMsg.NumberH = SET_NUMBERH(lpObj.m_Index);
	pMsg.NumberL = SET_NUMBERL(lpObj.m_Index);
	pMsg.chatmsg[len + 1] = 0;

	IOCP.DataSend(senduser, (UCHAR*)&pMsg, pMsg.h.size);
}

void GameProtocol::PChatProc(PMSG_CHATDATA * lpChat, short aIndex)
{
	if (!ObjectMaxRange(aIndex))
		return;

	int n;
	
	int number;
	int slen = strlen(lpChat->chatmsg);

	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	if (slen < 1)
		return;

	if (slen > MAX_CHAT_LEN - 1)
	{
		sLog->outBasic("[Anti-HACK][PChatProc][%s][%s] Chat Message Len : %d", lpObj.AccountID, lpObj.Name, slen);
		return;
	}

	if (GetTickCount() - lpObj.m_ChatFloodTime < 1000)
	{
		lpObj.m_ChatFloodCount++;

		if (lpObj.m_ChatFloodCount > 6)
		{
			sLog->outError( "lpObj.m_ChatFloodCOunt > 6");
			//this->GCSendDisableReconnect(aIndex);
			IOCP.CloseClient(aIndex);
			return;
		}
	}

	else
	{
		lpObj.m_ChatFloodCount = 0;
	}

	lpObj.m_ChatFloodTime = GetTickCount();

	char szId[MAX_ACCOUNT_LEN + 1];
	szId[MAX_ACCOUNT_LEN] = 0;
	CHAT_LOG_DATA pChatMsg;

	if (g_ConfigRead.server.GetServerType() != SERVER_BATTLECORE)
	{
		memcpy(szId, lpObj.Name, MAX_ACCOUNT_LEN);
	}

	else
	{
		memcpy(szId, lpObj.m_PlayerData->m_RealNameOfUBF, MAX_ACCOUNT_LEN);
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
				sprintf(szBuff, "[#%d]%s", lpObj.m_PlayerData->m_nServerCodeOfHomeWorld, lpChat->chatmsg);
			}
		}

		else
		{
			sprintf(szBuff, "%c[#%d]%s", lpChat->chatmsg[0], lpObj.m_PlayerData->m_nServerCodeOfHomeWorld, lpChat->chatmsg);
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
		memcpy(pChatMsg.AccountID, lpObj.AccountID, MAX_ACCOUNT_LEN);
		memcpy(pChatMsg.Name, lpObj.Name, MAX_ACCOUNT_LEN);
		pChatMsg.AccountID[MAX_ACCOUNT_LEN] = 0;
		pChatMsg.Name[MAX_ACCOUNT_LEN] = 0;
		pChatMsg.wServer = g_ConfigRead.server.GetGameServerCode();
		pChatMsg.btType = 0xFF;
	}

	//MsgLog->Output("[%s][%s]: %s",lpObj.AccountID,lpObj.Name,lpChat->chatmsg);
	if ((lpObj.Penalty & 2) == 2)
		return;

	if (lpObj.m_PlayerData->m_bSecurityCheck == false)
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
			if ((lpObj.AuthorityCode & 1) == 1)
			{
				if (lpChat->chatmsg[1] == '!')
				{
					char Msg[128];
					sprintf(Msg, "[%s][%s]: %s", g_ConfigRead.server.GetServerName(), lpObj.Name, &lpChat->chatmsg[2]);
					GS_GDReqMapSvrMsgMultiCast(g_MapServerManager.GetMapSvrGroup(), Msg);

					sLog->outBasic("Announcement: %s %s %s", lpObj.AccountID, lpObj.Name, &lpChat->chatmsg[2]);

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
					TNotice::MakeNoticeMsgEx(&pNotice, 0, "%s : %s", lpObj.Name, &lpChat->chatmsg[1]);
					TNotice::SendNoticeToAllUser(&pNotice);

					sLog->outBasic("Announcement: %s %s %s", lpObj.AccountID, lpObj.Name, &lpChat->chatmsg[1]);

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

			cManager.ManagementProc(lpObj, lpChat->chatmsg, aIndex);
			return;
		}
		break;
	}

	// Other Commands
	if (lpObj.ChatLimitTime > 0)
	{
		MsgOutput(lpObj.m_Index, Lang.GetText(0, 139), lpObj.ChatLimitTime);
		return;
	}

	// Party Message
	if (lpChat->chatmsg[0] == '~' || lpChat->chatmsg[0] == ']')
	{
		if (lpObj.m_PlayerData->m_bUsePartyMatching == false || lpObj.m_PlayerData->m_nPartyMatchingIndex < 0)
		{
			if (lpObj.PartyNumber >= 0)
			{
				int partycount = gParty.GetPartyCount(lpObj.PartyNumber);

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
						number = gParty.m_PartyS[lpObj.PartyNumber].Number[n];

						if (number >= 0)
						{
							IOCP.DataSend(number, (BYTE*)lpChat, lpChat->h.size);

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

			GDSendChatMsgPartyMatching(lpChat->chatmsg, lpObj.Name, lpObj.m_PlayerData->m_nPartyMatchingIndex);
		}
	}

	else if (lpChat->chatmsg[0] == '$')
	{
		if (lpObj.m_PlayerData->m_GensInfluence == DUPRIAN_INFLUENCE || lpObj.m_PlayerData->m_GensInfluence == VANERT_INFLUENCE)
		{
			for (n = g_ConfigRead.server.GetObjectStartUserIndex(); n < g_ConfigRead.server.GetObjectMax(); n++)
			{
				if (gObjIsConnected(n) && getGameObject(n)->m_PlayerData->m_GensInfluence == lpObj.m_PlayerData->m_GensInfluence)
				{
					IOCP.DataSend(n, (BYTE*)lpChat, lpChat->h.size);
				}
			}

			GDGensServerGroupChattingSend(lpObj.m_PlayerData->m_GensInfluence, lpChat);
		}
	}

	// Guild
	else if (lpChat->chatmsg[0] == '@')
	{
		if (lpObj.m_PlayerData->GuildNumber > 0)
		{
			// Notice
			if (lpChat->chatmsg[1] == '>')
			{
				if (lpObj.Name[0] == lpObj.m_PlayerData->lpGuild->Names[0][0])
				{
					if (!strcmp(lpObj.Name, lpObj.m_PlayerData->lpGuild->Names[0]))
					{
						for (int i = 0; i < strlen(lpChat->chatmsg); i++)
						{
							if (lpChat->chatmsg[i] == '%')
							{
								MsgOutput(aIndex, Lang.GetText(0, 514));
								return;
							}
						}
						GDGuildNoticeSave(lpObj.m_PlayerData->lpGuild->Name, &lpChat->chatmsg[2]);
						sLog->outBasic("Guild announcement %s %s", lpObj.m_PlayerData->lpGuild->Name, lpChat->chatmsg);

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
			else if (lpChat->chatmsg[1] == '@' && lpObj.m_PlayerData->lpGuild->iGuildUnion)
			{
				int iGuildCount = 0;
				int iGuildList[MAX_UNION_GUILD] = { 0 };

				if (UnionManager.GetGuildUnionMemberList(lpObj.m_PlayerData->lpGuild->iGuildUnion, iGuildCount, iGuildList) == TRUE)
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

								if (number >= 0)
								{
									if (lpGuildInfo->Names[n][0] == getGameObject(number]->Name[0))
									{
										if (!strcmp(lpGuildInfo->Names[n], getGameObject(number)->Name))
										{
											IOCP.DataSend(number, (BYTE*)lpChat, lpChat->h.size);
										}
									}
								}
							}
						}
					}
				}

				if (g_iServerGroupUnionChatting == TRUE)
					GDUnionServerGroupChattingSend(lpObj.m_PlayerData->lpGuild->iGuildUnion, lpChat);
			}
			// Just Guild
			else
			{
				int count = lpObj.m_PlayerData->lpGuild->Count;

				if (count >= 0)
				{
					for (n = 0; n < MAX_USER_GUILD; n++)
					{
						if (lpObj.m_PlayerData->lpGuild->Use[n])
						{
							number = lpObj.m_PlayerData->lpGuild->Index[n];

							if (number >= 0)
							{
								if (lpObj.m_PlayerData->lpGuild->Names[n][0] == getGameObject(number]->Name[0))
								{
									if (!strcmp(lpObj.m_PlayerData->lpGuild->Names[n], getGameObject(number)->Name))
									{
										IOCP.DataSend(number, (BYTE*)lpChat, lpChat->h.size);
									}
								}
							}
						}
					}
				}

				if (g_iServerGroupGuildChatting == TRUE)
				{
					if (lpObj.m_PlayerData->lpGuild->Count > 1)
					{
						GDGuildServerGroupChattingSend(lpObj.m_PlayerData->lpGuild->Number, lpChat);
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
				strcpy(pChatMsg.szTargetName[szTargetNameCount], lpObj.m_PlayerData->GuildName);
				szTargetNameCount++;
			}
		}
	}
	else
	{
		if ((lpObj.Authority & 0x20) == 0x20 && gObjCheckUsedBuffEffect(&getGameObject(aIndex), BUFFTYPE_INVISABLE) == TRUE)
		{
			return;
		}

		if (!g_prohibitedSymbols.Validate(lpChat->chatmsg, strlen(lpChat->chatmsg), TYPE_CHAT))
		{
			MsgOutput(aIndex, Lang.GetText(0, 545));
			return;
		}

		if (SwearFilter.CompareText(lpChat->chatmsg) == true && lpChat->chatmsg[0] != '@' && lpChat->chatmsg[0] != '~')
		{
			++lpObj.m_PlayerData->SwearWarning;

			if (lpObj.m_PlayerData->SwearWarning >= 3)
			{
				MsgOutput(aIndex, Lang.GetText(0, 515));
				//this->GCSendDisableReconnect(aIndex);
				IOCP.CloseClient(aIndex);
			}
			else
			{
				MsgOutput(aIndex, Lang.GetText(0, 516), lpObj.m_PlayerData->SwearWarning);
			}
			return;
		}


		if (g_NewPVP.IsDuel(*lpObj) || g_NewPVP.IsObserver(*lpObj))
		{
			g_NewPVP.ChatMsgSend(*lpObj, (BYTE*)lpChat, lpChat->h.size);
		}

		else
		{
			IOCP.DataSend(lpObj.m_Index, (BYTE*)lpChat, lpChat->h.size);

			if (g_GensSystem.IsMapBattleZone(lpObj.MapNumber))
			{
				g_GensSystem.BattleZoneChatMsgSend(lpObj, (BYTE*)lpChat, lpChat->h.size);
			}

			else
			{
				this->MsgSendV2(lpObj, (BYTE*)lpChat, lpChat->h.size);
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


void GameProtocol::CGChatRecv(PMSG_CHATDATA_NUMBER * lpMsg, CGameObject &Obj)
{
	

	if (PacketCheckTime(aIndex) == FALSE)
	{
		return;
	}

	IOCP.DataSend(lpObj.m_Index, (BYTE*)lpMsg, lpMsg->h.size);
	MsgSendV2(lpObj, (BYTE*)lpMsg, lpMsg->h.size);
}




void GameProtocol::GCServerMsgSend(BYTE msg, CGameObject &Obj)
{
	PMSG_SERVERMSG pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x0C, sizeof(pMsg));
	pMsg.MsgNumber = msg;
	IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);
}




void GameProtocol::GCServerMsgStringSend(LPSTR  szMsg, CGameObject &Obj, BYTE type)
{
	PMSG_NOTICE pNotice;

	TNotice::MakeNoticeMsg((TNotice*)&pNotice, type, szMsg);
	IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pNotice, pNotice.h.size);
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

void GameProtocol::GCServerMsgStringSendEx(CGameObject &Obj, BYTE type, LPSTR szMsg, ...)
{
	va_list pArguments;
	char szBuffer[512];

	va_start(pArguments, szMsg);
	vsprintf(szBuffer, szMsg, pArguments);

	this->GCServerMsgStringSend(szBuffer, aIndex, type);
}


void GameProtocol::GCServerMsgStringSendGuild(GUILD_INFO_STRUCT &lpNode, LPSTR szMsg, BYTE type)
{
	if (lpNode == NULL)
	{
		return;
	}

	for (int n = 0; n < MAX_USER_GUILD; n++)
	{
		if (lpNode.Use[n] > 0 && lpNode.Index[n] >= 0)
		{
			GCServerMsgStringSend((char*)szMsg, lpNode.Index[n], type);
		}
	}
}

void GameProtocol::GCEventStateSend(CGameObject &Obj, BYTE state, BYTE event)
{
	PMSG_EVENT pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x0B, sizeof(pMsg));
	pMsg.Event = event;
	pMsg.State = state;

	IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);
}



void GameProtocol::GCMapEventStateSend(int map, BYTE state, BYTE event)
{
	PMSG_EVENT pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x0B, sizeof(pMsg));
	pMsg.Event = event;
	pMsg.State = state;

	for (int n = 0; n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		if (getGameObject(n]->Connected == PLAYER_PLAYING && getGameObject(n)->Type == OBJ_USER && map == getGameObject(n)->MapNumber)
		{
			IOCP.DataSend(n, (UCHAR*)&pMsg, pMsg.h.size);
		}
	}
}



void GameProtocol::CGChatWhisperRecv(PMSG_CHATDATA_WHISPER* lpMsg, CGameObject &Obj)
{
	char tid[11];
	PMSG_CHATDATA_WHISPER pWhisper;
	int len;
	
	int index;

	if (PacketCheckTime(aIndex) == FALSE)
	{
		return;
	}

	if (lpObj.ChatLimitTime > 0)
	{
		MsgOutput(lpObj.m_Index, Lang.GetText(0, 139), lpObj.ChatLimitTime);
		return;
	}

	if ((lpObj.Penalty & 2) == 2)
	{
		return;
	}

	if (g_ConfigRead.data.common.SavePrivateChat == true)
	{
		MsgLog->Output(Lang.GetText(0, 517), lpObj.AccountID, lpMsg->id, lpMsg->chatmsg);
	}

	tid[10] = 0;
	memcpy(tid, lpMsg->id, sizeof(lpMsg->id));
	index = WhisperCash.AddCash(tid);

	if (index < 0)
	{
		if (g_ConfigRead.data.common.joinmuMultiChannelWhisper == true)
		{
			//ifjoinmu
			lpObj.m_Whispering.iWhisperSent = true;
			lpObj.m_Whispering.wResponseCount = 0;
			PMSG_RECV_CHATDATA_WHISPER pDSMsg;
			pDSMsg.h.set((BYTE*)&pDSMsg, 0xC2, sizeof(pDSMsg));
			memcpy(pDSMsg.fromId, lpObj.Name, sizeof(pDSMsg.fromId));
			pDSMsg.wMapSvrNum = g_MapServerManager.GetMapSvrGroup();
			memcpy(pDSMsg.id, lpMsg->id, sizeof(lpMsg->id));
			memcpy(pDSMsg.chatmsg, lpMsg->chatmsg, sizeof(lpMsg->chatmsg));
			pDSMsg.OriginPlayerIndex = aIndex;
			wsDataCli.DataSend((char *)&pDSMsg, pDSMsg.h.size);
			return;
		}
		else
		{
			GCServerMsgSend(0, aIndex);
			return;
		}
	}

	if (!UserChatBlockMng::getInstance()->isCanReceiveChat(aIndex, index))
	{
		return;
	}

	PHeadSetB((BYTE*)&pWhisper, 0x02, sizeof(pWhisper));
	strcpy(pWhisper.id, lpObj.Name);
	memcpy(pWhisper.chatmsg, lpMsg->chatmsg, sizeof(lpMsg->chatmsg));

	//pWhisper.szChatMsg[MAX_CHAT_LEN-2] = 0;
	//pWhisper.chatmsg[MAX_CHAT_LEN-1] = 0;
	len = strlen(pWhisper.chatmsg);

	if (getGameObject(index)->m_PlayerData->bActiveSetParty == true)
	{
		if (len >= 4)
		{
			if (!strcmp(pWhisper.chatmsg, getGameObject(index)->m_PlayerData->PartyPassword))
			{
				gObjAutoPartySet(&getGameObject(index], &getGameObject(aIndex));
			}
		}
	}
	if (len > 0)
	{
		pWhisper.h.size -= sizeof(pWhisper.chatmsg);
		pWhisper.h.size += len + 1;

		if (g_GensSystem.IsMapBattleZone(lpObj.MapNumber))
		{
			int OwnInfluence = g_GensSystem.GetGensInfluence(lpObj);
			int TarInfluence = g_GensSystem.GetGensInfluence(&getGameObject(index));

			if (OwnInfluence == TarInfluence || CheckAuthorityCondition(98, lpObj) || CheckAuthorityCondition(98, &getGameObject(index)))
			{
				IOCP.DataSend(index, (UCHAR*)&pWhisper, pWhisper.h.size);
			}
		}

		else
		{
			IOCP.DataSend(index, (UCHAR*)&pWhisper, pWhisper.h.size);
		}
	}
}

void GameProtocol::SCPJoinResultSend(CGameObject &Obj, BYTE result)
{
	PMSG_JOINRESULT pResult;

	memset(&pResult, 0, sizeof(pResult));

	pResult.h.size = sizeof(pResult);
	pResult.h.c = 0xC1;
	pResult.h.headcode = 0xF1;
	pResult.scode = 0x00;
	pResult.result = result;
	pResult.NumberH = SET_NUMBERH(aIndex);
	pResult.NumberL = SET_NUMBERL(aIndex);
	pResult.CliVersion[0] = szClientVersion[0];
	pResult.CliVersion[1] = szClientVersion[1];
	pResult.CliVersion[2] = szClientVersion[2];
	pResult.CliVersion[3] = szClientVersion[3];
	pResult.CliVersion[4] = szClientVersion[4];

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
	lpObj.ConnectCheckTime = GetTickCount();
}




void GameProtocol::CSPJoinIdPassRequest(PMSG_IDPASS* lpMsg, CGameObject &Obj)
{
	if (aIndex < g_ConfigRead.server.GetObjectStartUserIndex() || aIndex > g_ConfigRead.server.GetObjectMax())
	{
		return;
	}

	char serial[17];
	char id[11];
	char hwid[25];

	if (lpMsg->CliVersion[0] != szClientVersion[0] ||
		lpMsg->CliVersion[1] != szClientVersion[1] ||
		lpMsg->CliVersion[2] != szClientVersion[2] ||
		lpMsg->CliVersion[3] != szClientVersion[3] ||
		lpMsg->CliVersion[4] != szClientVersion[4])
	{
		GCJoinResult(JS_BAD_CLIENT_VERSION, aIndex);
		IOCP.CloseClient(aIndex);
		return;
	}

	if (lpMsg->ServerSeason != g_AuthSys.GetSeason())
	{
		sLog->outError( "[IP: %s][aIndex: %d] connecting with DLL for different Game Season, review and correct (if required) %d", lpObj.m_PlayerData->Ip_addr, aIndex, lpMsg->ServerSeason);

		//this->GCServerMsgStringSend(Lang.GetText(0,547), aIndex, 0);
		//GCJoinResult(JS_BAD_CLIENT_VERSION, aIndex);
		//IOCP.CloseClient(aIndex);
		//return;
	}

#ifdef EMU_NOCRYPT
	BuxConvert(lpMsg->Id, MAX_ACCOUNT_LEN);
	BuxConvert(lpMsg->Pass, 20);
#endif

	id[10] = 0;
	memcpy(id, lpMsg->Id, sizeof(lpMsg->Id));
	BuxConvert(id, MAX_ACCOUNT_LEN);
	ProcessClientHWID(lpMsg->HWID, hwid);

	if (PercentSyntaxCheck(id) == FALSE)
	{
		this->GCJoinResult(JS_BAD_CLIENT_VERSION, aIndex);
		IOCP.CloseClient(aIndex);
		return;
	}

	serial[16] = 0;
	memcpy(serial, lpMsg->CliSerial, sizeof(lpMsg->CliSerial));

	if (strcmp(serial, szGameServerExeSerial) != 0)
	{
		this->GCJoinResult(JS_BAD_CLIENT_VERSION, aIndex);
		IOCP.CloseClient(aIndex);
		return;
	}

	if (lpMsg->HWID == NULL)
	{
		IOCP.CloseClient(aIndex);
		return;
	}

	if (g_ConfigRead.server.IsConnectMemberUse() == TRUE)
	{
		if (ConMember.IsMember(id) == FALSE)
		{
			this->GCJoinResult(JS_ONLY_VIP_MEMBERS, aIndex);
			return;
		}
	}

	if (g_ConfigRead.server.IsTestServer() == TRUE)
	{
		if (ConMember.IsMember(id) == FALSE)
		{
			this->GCJoinResult(2, aIndex);
			this->GCServerMsgStringSend(Lang.GetText(0, 518), aIndex, 0);
			return;
		}
	}

	

	if (PacketCheckTime(aIndex) == FALSE)
	{
		IOCP.CloseClient(aIndex);
		return;
	}

	if (lpObj.Connected != PLAYER_CONNECTED)
	{
		IOCP.CloseClient(aIndex);
		return;
	}

	if (lpObj.LoginMsgSnd != FALSE)
	{
		if (lpObj.LoginMsgSnd == TRUE)
		{

		}
		else
		{

		}

		return;
	}

	SDHP_IDPASS spMsg;

	PHeadSetB((BYTE*)&spMsg, 0x01, sizeof(spMsg));
	spMsg.Number = aIndex;
	memcpy(spMsg.Id, lpMsg->Id, sizeof(spMsg.Id));
	memcpy(spMsg.Pass, lpMsg->Pass, sizeof(spMsg.Pass));
	memcpy(spMsg.HWID, hwid, sizeof(hwid));
	strcpy(spMsg.IpAddress, lpObj.m_PlayerData->Ip_addr);
	lpObj.CheckTick = lpMsg->TickCount;
	lpObj.CheckTick2 = GetTickCount();
	lpObj.ConnectCheckTime = GetTickCount();
	lpObj.CheckSpeedHack = true;
	lpObj.ConnectCheckTime = GetTickCount();
	lpObj.AgiCheckTime = GetTickCount();
	lpObj.LoginMsgSnd = 1;
	lpObj.LoginMsgCount++;
	lpObj.AccountID[MAX_ACCOUNT_LEN] = 0;
	memcpy(lpObj.AccountID, id, MAX_ACCOUNT_LEN);
	memcpy(lpObj.m_PlayerData->HWID, hwid, sizeof(hwid));
	lpObj.m_cAccountItemBlock = 0;
	lpObj.m_PlayerData->VipType = 0;
	lpObj.m_PlayerData->VipEffect = VIP_EFFECT_NONE;
	lpObj.m_PlayerData->AntiHackCheckTime = GetTickCount();
	lpObj.m_PlayerData->dwLastHitHackTick = GetTickCount64();
	wsJServerCli.DataSend((char*)&spMsg, spMsg.h.size);
	sLog->outBasic("Connect Account: %s / MachineID: %s", lpObj.AccountID, lpMsg->HWID);
	lpObj.m_bMapSvrMoveReq = false;
	lpObj.m_bMapSvrMoveReq_2 = false;
	lpObj.m_sPrevMapSvrCode = -1;
	lpObj.m_sDestMapNumber = -1;
	lpObj.m_btDestX = 0;
	lpObj.m_btDestY = 0;
}

void GameProtocol::GCJoinBillCheckSend(LPSTR AccountId, CGameObject &Obj)
{
	SDHP_BILLSEARCH pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x06, sizeof(pMsg));
	memcpy(pMsg.Id, AccountId, sizeof(pMsg.Id));
	pMsg.Number = aIndex;

	wsJServerCli.DataSend((char*)&pMsg, pMsg.h.size);
}




void GameProtocol::GCJoinResult(BYTE result, CGameObject &Obj)
{
	PMSG_RESULT  pResult;

	PHeadSubSetB((BYTE*)&pResult, 0xF1, 0x01, sizeof(pResult));
	pResult.result = result;
	IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pResult, pResult.h.size);
}



void GameProtocol::GCJoinSocketResult(BYTE result, SOCKET Socket)
{
	PMSG_RESULT pResult;

	PHeadSubSetB((BYTE*)&pResult, 0xF1, 0x01, sizeof(pResult));
	pResult.result = result;

	wsGServer.DataSocketSend(Socket, (char*)&pResult, pResult.h.size);
}




void GameProtocol::CGClientCloseMsg(PMSG_CLIENTCLOSE * lpMsg, CGameObject &Obj)
{
	if (aIndex < 0 || aIndex > g_ConfigRead.server.GetObjectMax() - 1)
	{
		return;
	}

	switch (lpMsg->Flag)
	{
	case 0:	// Close Game
		lpObj.m_bOff = 0;
		gObjCloseSet(aIndex, 0);
		break;

	case 2:	// ServerList
		lpObj.m_bOff = 0;
		gObjCloseSet(aIndex, 2);
		break;

	case 1: // Select Character
		lpObj.m_bOff = 0;
		gObjCloseSet(aIndex, 1);
		break;
	}
}




void GameProtocol::GCCloseMsgSend(CGameObject &Obj, BYTE result)
{
	PMSG_RESULT pMsg;

	PHeadSubSetBE((BYTE*)&pMsg, 0xF1, 0x02, sizeof(pMsg));
	pMsg.result = result;

	IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);
}



void GameProtocol::CGClientMsg(PMSG_CLIENTMSG* lpMsg, CGameObject &Obj)
{
	if (lpMsg->Flag)
	{

	}
	else
	{
		if (lpObj.m_ClientHackLogCount < 100)
		{

		}

		lpObj.m_ClientHackLogCount++;

		if (lpObj.m_ClientHackLogCount >= 100)
			lpObj.m_ClientHackLogCount = 100;
	}

	char msg[255];

	wsprintf(msg, "Client HackCheck %d", lpMsg->Flag);
}

void GameProtocol::CGPCharacterCreate(PMSG_CHARCREATE * lpMsg, CGameObject &Obj)
{
	if (!PacketCheckTime(aIndex))
	{
		JGCharacterCreateFailSend(aIndex, lpMsg->Name);
		return;
	}

	if (aIndex < g_ConfigRead.server.GetObjectStartUserIndex() || aIndex > g_ConfigRead.server.GetObjectMax())
	{
		return;
	}

	if (lpObj.Connected < PLAYER_LOGGED)
	{
		IOCP.CloseClient(aIndex);
		return;
	}

	if (lpObj.Connected == PLAYER_PLAYING)
	{
		//this->GCSendDisableReconnect(aIndex);
		IOCP.CloseClient(aIndex);

		return;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 620), aIndex, 1);
		JGCharacterCreateFailSend(aIndex, lpMsg->Name);
		return;
	}

	if (lpObj.m_PlayerData->m_bSecurityCheck == false)
	{
		GCServerMsgStringSend(Lang.GetText(0, 512), aIndex, 1);
		JGCharacterCreateFailSend(aIndex, lpMsg->Name);
		return;
	}

	if (!gCreateCharacter)
	{
		GCServerMsgStringSend(Lang.GetText(0, 513), aIndex, 1);
		JGCharacterCreateFailSend(aIndex, lpMsg->Name);
		return;
	}

	if (lpMsg->ClassSkin == 0x00 || lpMsg->ClassSkin == 0x10 || lpMsg->ClassSkin == 0x20 || lpMsg->ClassSkin == 0x30 || lpMsg->ClassSkin == 0x40 || lpMsg->ClassSkin == 0x50 || lpMsg->ClassSkin == 0x60 || lpMsg->ClassSkin == 0x70)
	{

	}
	else
	{
		JGCharacterCreateFailSend(aIndex, lpMsg->Name);
		return;
	}

	if (lpMsg->ClassSkin == 0x30) // MG
	{
		if ((lpObj.EnableCharacterCreate & 4) != 4)
		{
			JGCharacterCreateFailSend(aIndex, lpMsg->Name);
			return;
		}
	}

	if (lpMsg->ClassSkin == 0x40) // DL
	{
		if ((lpObj.EnableCharacterCreate & 2) != 2)
		{
			JGCharacterCreateFailSend(aIndex, lpMsg->Name);
			return;
		}
	}

	if (lpMsg->ClassSkin == 0x50)
	{
		if ((lpObj.EnableCharacterCreate & 1) != 1)
		{
			JGCharacterCreateFailSend(aIndex, lpMsg->Name);
			return;
		}
	}

	if (lpMsg->ClassSkin == 0x60)
	{
		if ((lpObj.EnableCharacterCreate & 8) != 8)
		{
			JGCharacterCreateFailSend(aIndex, lpMsg->Name);
			return;
		}
	}

	if (lpMsg->ClassSkin == 0x70)
	{
		if ((lpObj.EnableCharacterCreate & 0x10) != 0x10)
		{
			JGCharacterCreateFailSend(aIndex, lpMsg->Name);
			return;
		}
	}

	char szName[11];
	szName[10] = 0;
	memcpy(szName, lpMsg->Name, MAX_ACCOUNT_LEN);
	if (!g_prohibitedSymbols.Validate(szName, strlen(szName), TYPE_NAME))
	{
		JGCharacterCreateFailSend(aIndex, lpMsg->Name);
		return;
	}

	if (SwearFilter.CompareText(lpMsg->Name))
	{
		JGCharacterCreateFailSend(aIndex, lpMsg->Name);
		return;
	}

	if (strstr(lpMsg->Name, "[A]") != 0)
	{
		JGCharacterCreateFailSend(aIndex, lpMsg->Name);
		return;
	}

	SDHP_CREATECHAR sCreate;

	memset(&sCreate, 0, sizeof(sCreate));
	PHeadSetB((BYTE*)&sCreate, 0x04, sizeof(sCreate));
	sCreate.ClassSkin = lpMsg->ClassSkin;
	sCreate.Number = aIndex;
	memcpy(sCreate.AccountId, lpObj.AccountID, MAX_ACCOUNT_LEN);
	memcpy(sCreate.Name, lpMsg->Name, MAX_ACCOUNT_LEN);
	char szTemp[11];
	char szTemp2[11];
	szTemp[10] = 0;
	szTemp2[10] = 0;
	memcpy(szTemp, sCreate.Name, MAX_ACCOUNT_LEN);
	memcpy(szTemp2, sCreate.AccountId, MAX_ACCOUNT_LEN);

	//sLog->outBasic("Character Created : %s,%s", szTemp2, szTemp);

	wsDataCli.DataSend((char*)&sCreate, sCreate.h.size);
	for (int n = 0; n < 5; n++)
	{

		if (strcmp(lpObj.Name1[n], " ") == 0)
		{

			memcpy(lpObj.Name1[n], szTemp, MAX_ACCOUNT_LEN);
			break;
		}
		if (strcmp(lpObj.Name1[n], "") == 0)
		{

			memcpy(lpObj.Name1[n], szTemp, MAX_ACCOUNT_LEN);
			break;
		}
	}
}

void GameProtocol::CGPCharDel(PMSG_CHARDELETE * lpMsg, CGameObject &Obj)
{

	if (!PacketCheckTime(aIndex))
		return;

	if (lpObj.Connected < PLAYER_LOGGED)
	{
		IOCP.CloseClient(aIndex);
		return;
	}

	if (lpObj.Connected == PLAYER_PLAYING)
	{
		this->GCSendDisableReconnect(aIndex);
		//IOCP.CloseClient(aIndex);
		return;
	}

	if (lpObj.bEnableDelCharacter == FALSE)
	{
		this->GCSendDisableReconnect(aIndex);
		//	IOCP.CloseClient(aIndex);
		return;
	}

	if (lpObj.m_PlayerData->GuildNumber > 0 && lpObj.m_PlayerData->lpGuild != NULL)
	{
		this->GCSendDisableReconnect(aIndex);
		//IOCP.CloseClient(aIndex);
		return;
	}

	SDHP_CHARDELETE pCDel;
	PMSG_RESULT pResult;

	PHeadSubSetB((BYTE*)&pResult, 0xF3, 0x02, sizeof(pResult));
	pResult.result = 0;

	if (lpObj.m_cAccountItemBlock)
	{
		pResult.result = 3;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (!g_ConfigRead.data.common.GuildDestroy)
	{
		pResult.result = 0;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		pResult.result = 0;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (lpObj.m_PlayerData->m_bSecurityCheck == false)
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 513), aIndex, 1);
		pResult.result = 3;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	char szJoomin[21];
	char szTemp[20];
	char szTemp2[20];
	bool Go = false;
	memset(szJoomin, 0, sizeof(szJoomin));
	memcpy(szJoomin, lpMsg->LastJoominNumber, sizeof(lpMsg->LastJoominNumber));

	if (gObjPasswordCheck(aIndex, szJoomin) == FALSE)
	{
		pResult.result = 2;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	PHeadSetB((BYTE*)&pCDel, 0x05, sizeof(pCDel));
	pCDel.Number = aIndex;
	memcpy(pCDel.AccountID, lpObj.AccountID, sizeof(pCDel.AccountID));
	memcpy(pCDel.Name, lpMsg->Name, sizeof(lpMsg->Name));
	pCDel.Guild = 0;

	if (lpObj.m_PlayerData->GuildNumber > 0 && lpObj.m_PlayerData->lpGuild != NULL)
	{
		if (!strcmp(lpObj.Name, lpObj.m_PlayerData->lpGuild->Names[0]))
		{
			pCDel.Guild = 1;	// Master
		}
		else
		{
			pCDel.Guild = 2;	// Member
		}

		memcpy(pCDel.GuildName, lpObj.m_PlayerData->lpGuild->Name, MAX_GUILD_LEN);
	}

	g_UnityBattleField.GDReqCancelUnityBattleField(aIndex, 2, lpMsg->Name);

	szTemp[10] = 0;
	szTemp2[10] = 0;
	memcpy(szTemp, pCDel.Name, MAX_ACCOUNT_LEN);
	memcpy(szTemp2, pCDel.AccountID, MAX_ACCOUNT_LEN);

	wsDataCli.DataSend((PCHAR)&pCDel, pCDel.h.size);
	lpObj.Level = 0;
}



void GameProtocol::CGPCharacterMapJoinRequest(PMSG_CHARMAPJOIN * lpMsg, CGameObject &Obj)
{
	if (!PacketCheckTime(aIndex))
	{
		//JGCharacterCreateFailSend(aIndex, lpMsg->Name);
		return;
	}

	if (!gObjIsAccontConnect(aIndex, lpObj.AccountID))
		return;

	if (lpObj.Connected == PLAYER_PLAYING)
		return;

	lpObj.bEnableDelCharacter = FALSE;

	char _name[MAX_ACCOUNT_LEN + 1];
	SDHP_DBCHARINFOREQUEST pCRequest;

	PHeadSetB((BYTE*)&pCRequest, 0x06, sizeof(pCRequest));
	memset(_name, 0, MAX_ACCOUNT_LEN);

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		std::string tmpCmpName;
		tmpCmpName.insert(0, lpMsg->Name, MAX_ACCOUNT_LEN);

		if (lpObj.m_PlayerData->m_NameConvertOfUBF.IsDuplicationOfName() == TRUE)
		{
			memcpy(_name, lpObj.m_PlayerData->m_NameConvertOfUBF.ConvertSlotIndexToName(lpMsg->btSlotIndex), MAX_ACCOUNT_LEN);
		}

		else
		{
			memcpy(_name, lpObj.m_PlayerData->m_NameConvertOfUBF.ConvertRealNameToName(tmpCmpName.c_str()), MAX_ACCOUNT_LEN);
		}
	}

	else
	{
		memcpy(_name, lpMsg->Name, MAX_ACCOUNT_LEN);
	}

	memcpy(pCRequest.Name, _name, MAX_ACCOUNT_LEN);
	strcpy(pCRequest.AccountID, lpObj.AccountID);
	pCRequest.Number = aIndex;

	wsDataCli.DataSend((char*)&pCRequest, pCRequest.h.size);
}

void GameProtocol::GCLevelUpMsgSend(CGameObject &Obj, int iSendEffect)
{
	if (!ObjectMaxRange(aIndex))
		return;

	short AddPoint = 0;
	short MaxAddPoint = 0;
	short MinusPoint = 0;
	short MaxMinusPoint = 0;

	gObjGetStatPointState(lpObj.m_Index, AddPoint, MaxAddPoint, MinusPoint, MaxMinusPoint);

	PMSG_LEVELUP pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x05, sizeof(pMsg));
	pMsg.Level = lpObj.Level;
	pMsg.LevelUpPoint = lpObj.m_PlayerData->LevelUpPoint;
	pMsg.MaxLife = lpObj.MaxLife + lpObj.AddLife;
	pMsg.MaxMana = lpObj.MaxMana + lpObj.AddMana;
	pMsg.MaxBP = lpObj.MaxBP + lpObj.AddBP;
	pMsg.wMaxShield = lpObj.iMaxShield + lpObj.iAddShield;
	pMsg.AddPoint = AddPoint;
	pMsg.MaxAddPoint = MaxAddPoint;
	pMsg.MinusPoint = MinusPoint;
	pMsg.MaxMinusPoint = MaxMinusPoint;
	pMsg.IGCMaxLife = lpObj.MaxLife + lpObj.AddLife;
	pMsg.IGCMaxMana = lpObj.MaxMana + lpObj.AddMana;

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

	if (iSendEffect == 1)
		GCSendEffectInfo(aIndex, 0x10);
}

void GameProtocol::CGLevelUpPointAdd(PMSG_LVPOINTADD * lpMsg, CGameObject &Obj)
{
	if (!gObjIsConnected(aIndex))
	{
		this->GCSendDisableReconnect(aIndex);
		//IOCP.CloseClient(aIndex);
		return;
	}

	if (lpObj.Type != OBJ_USER)
	{
		this->GCSendDisableReconnect(aIndex);
		//IOCP.CloseClient(aIndex);
		return;
	}

	EnterCriticalSection(&lpObj.m_PlayerData->AgiCheckCriti);

	lpObj.m_PlayerData->AgilityCheckDelay = GetTickCount();

	PMSG_LVPOINTADDRESULT pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x06, sizeof(pMsg));
	pMsg.ResultType = 0;

	if (::gObjLevelUpPointAdd(lpMsg->Type, &getGameObject(aIndex), aIndex) == TRUE)
	{
		pMsg.ResultType = 0x10;
		pMsg.ResultType += lpMsg->Type;

		switch (lpMsg->Type)
		{
		case 2:	// Vit
			pMsg.MaxLifeAndMana = lpObj.MaxLife + lpObj.AddLife;
			pMsg.IGCMaxLifeAndMana = lpObj.MaxLife + lpObj.AddLife;
			break;

		case 3:	// Energy
			pMsg.MaxLifeAndMana = lpObj.MaxMana + lpObj.AddMana;
			pMsg.IGCMaxLifeAndMana = lpObj.MaxMana + lpObj.AddMana;
			break;
		}

		pMsg.wMaxShield = lpObj.iMaxShield + lpObj.iAddShield;
		gObjSetBP(aIndex);
		pMsg.MaxBP = lpObj.MaxBP + lpObj.AddBP;
	}

	IOCP.DataSend(lpObj.m_Index, (UCHAR *)&pMsg, pMsg.h.size);
	LeaveCriticalSection(&lpObj.m_PlayerData->AgiCheckCriti);

	/*PMSG_ADDSTATS_RESULT pAddStats;
	PHeadSubSetB((BYTE*)&pAddStats, 0xEC, 0x25, sizeof(pAddStats));
	pAddStats.str = lpObj.m_PlayerData->Strength;
	pAddStats.stradd = lpObj.AddStrength;
	pAddStats.agi = lpObj.m_PlayerData->Dexterity;
	pAddStats.agiadd = lpObj.AddDexterity;
	pAddStats.vit = lpObj.m_PlayerData->Vitality;
	pAddStats.vitadd = lpObj.AddVitality;
	pAddStats.ene = lpObj.m_PlayerData->Energy;
	pAddStats.eneadd = lpObj.AddEnergy;
	pAddStats.cmd = lpObj.Leadership;
	pAddStats.cmdadd = lpObj.AddLeadership;
	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pAddStats, pAddStats.h.size); */

	GameProtocol.GCPlayerStatsPanelNew(aIndex);
	GameProtocol.GCPlayerStatsPanelRates(aIndex);
}

void GameProtocol::GCInventoryItemOneSend(CGameObject &Obj, int pos)
{
	if (!lpObj.pInventory[pos].IsItem())
		return;

	PMSG_INVENTORYITEMMODIFY pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x14, sizeof(pMsg));
	pMsg.Pos = pos;
	ItemByteConvert(pMsg.ItemInfo, lpObj.pInventory[pos]);

	IOCP.DataSend(lpObj.m_Index, (UCHAR *)&pMsg, pMsg.h.size);
}

void GameProtocol::GCPkLevelSend(CGameObject &Obj, BYTE pklevel)
{
	PMSG_PKLEVEL pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x08, sizeof(pMsg));
	pMsg.NumberH = SET_NUMBERH(aIndex);
	pMsg.NumberL = SET_NUMBERL(aIndex);
	pMsg.PkLevel = pklevel;

	IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);
	MsgSendV2(&getGameObject(aIndex), (UCHAR*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCMagicListOneSend(CGameObject &Obj, char Pos, WORD type, BYTE level, WORD skill, BYTE btListType)
{
	PMSG_MAGICLISTCOUNT pCount;
	PMSG_MAGICLIST pList;
	BYTE sendbuf[1000];

	int lOfs = sizeof(pCount);
	PHeadSubSetW((BYTE*)&pCount, 0xF3, 0x11, 0);

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

	IOCP.DataSend(lpObj.m_Index, sendbuf, lOfs);
}


void GameProtocol::GCMagicListOneDelSend(CGameObject &Obj, char Pos, WORD type, BYTE level, WORD skill, BYTE btListType)
{
	PMSG_MAGICLISTCOUNT pCount;
	PMSG_MAGICLIST pList;
	BYTE sendbuf[1000];

	int lOfs = sizeof(pCount);
	PHeadSubSetW((BYTE*)&pCount, 0xF3, 0x11, 0);

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

	IOCP.DataSend(lpObj.m_Index, sendbuf, lOfs);
}


void GameProtocol::GCMagicListMultiSend(CGameObject &Obj, BYTE btListType)
{
	WORD BrandSkill;
	PMSG_MAGICLISTCOUNT pCount;
	PMSG_MAGICLIST pList;
	BYTE sendbuf[1000];

	int lOfs = sizeof(pCount);
	int Count = 0;
	pCount.btListType = btListType;

	PHeadSubSetW((BYTE*)&pCount, 0xF3, 0x11, 0);

	for (int n = 0; n < MAX_MAGIC; n++)
	{
		if (lpObj.Magic[n].IsMagic() == TRUE)
		{
			if (MagicDamageC.GetSkillUseType(lpObj.Magic[n].m_Skill) == 3)
			{
				continue;
			}

			if (!g_MasterLevelSkillTreeSystem.CheckUsableWeaponSkill(lpObj, n))
			{
				continue;
			}

			if (g_MasterLevelSkillTreeSystem.IsExistMLSBasedSkill(lpObj, lpObj.Magic[n].m_Skill))
			{
				continue;
			}

			if (lpObj.Magic[n].m_Pass == true)
			{
				continue;
			}

			pList.Pos = n;
			pList.wSkillNum = (WORD)lpObj.Magic[n].m_Skill;
			pList.btSkillLevel = lpObj.Magic[n].m_Level << 3;
			pList.btSkillLevel |= lpObj.Magic[n].m_Skill & 0x07;

			memcpy(&sendbuf[lOfs], &pList, sizeof(pList));
			lOfs += sizeof(pList);

			Count++;

			if (MagicDamageC.IsItemAttachedSkill(lpObj.Magic[n].m_Skill))
			{
				BrandSkill = MagicDamageC.GetNormalBrandOfSkill(lpObj.Magic[n].m_Skill);

				if (!lpObj.pInventory[1].IsItem() || lpObj.pInventory[1].m_Special[0] != BrandSkill)
				{
					continue;
				}

				if (!lpObj.pInventory[0].IsItem() || lpObj.pInventory[0].m_Special[0] != BrandSkill)
				{
					continue;
				}

				pList.Pos = n + 1;
				pList.wSkillNum = LOWORD(lpObj.Magic[n].m_Skill);
				pList.btSkillLevel = lpObj.Magic[n].m_Level << 3;
				pList.btSkillLevel |= lpObj.Magic[n].m_Skill & 0x07;

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

	IOCP.DataSend(lpObj.m_PlayerData->IDNumber, sendbuf, lOfs);
}

void GameProtocol::GCEquipmentSend(CGameObject &Obj)
{
	PMSG_EQUIPMENTLIST pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x13, sizeof(pMsg));
	pMsg.NumberH = SET_NUMBERH(aIndex);
	pMsg.NumberL = SET_NUMBERL(aIndex);
	gObjMakePreviewCharSet(aIndex);
	
	memcpy(pMsg.Equipment, lpObj.CharSet, sizeof(lpObj.CharSet));

	MsgSendV2(lpObj, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCRecallMonLife(CGameObject &Obj, int maxlife, int life)
{
	if (maxlife <= 0)
		return;

	PMSG_RECALLMONLIFE pMsg;
	BYTE per = life * 100 / maxlife;
	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x20, sizeof(pMsg));
	pMsg.Life = per;

	IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCTimeViewSend(CGameObject &Obj, int second)
{
	PMSG_TIMEVIEW pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x22, sizeof(pMsg));
	pMsg.Second = second;

	IOCP.DataSend(Obj.m_Index, (UCHAR *)&pMsg, pMsg.h.size);
}

void GameProtocol::GCGoalSend(CGameObject &Obj, char* Name1, BYTE score1, char* Name2, BYTE score2)
{
	PMSG_GOALSEND pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x23, sizeof(pMsg));
	memcpy(pMsg.RedTeamName, Name1, sizeof(pMsg.RedTeamName));
	pMsg.RedTeamScore = score1;
	memcpy(pMsg.BlueTeamName, Name2, sizeof(pMsg.BlueTeamName));
	pMsg.BlueTeamScore = score2;

	IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCSkillKeyRecv(PMSG_SKILLKEY * lpMsg, CGameObject &Obj)
{
	if (!gObjIsGamePlaing(&getGameObject(aIndex)))
		return;

	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	DGOptionDataSend(lpObj.m_Index, lpObj.Name,
		lpMsg->SkillKey, lpMsg->GameOption,
		lpMsg->QkeyDefine, lpMsg->WkeyDefine, lpMsg->EkeyDefine, lpMsg->ChatWindow, lpMsg->RkeyDefine, ntohl(lpMsg->QWERLevel), lpObj.m_PlayerData->m_EnableUseChangeSkin);
}

void GameProtocol::GCSkillKeySend(CGameObject &Obj, BYTE* keybuffer, BYTE GO, BYTE Qk, BYTE Wk, BYTE Ek, BYTE ChatWnd, BYTE Rk, int QWER)
{
	PMSG_SKILLKEY pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x30, sizeof(pMsg));
	memcpy(pMsg.SkillKey, keybuffer, sizeof(pMsg.SkillKey));
	pMsg.GameOption = GO;
	pMsg.QkeyDefine = Qk;
	pMsg.WkeyDefine = Wk;
	pMsg.EkeyDefine = Ek;
	pMsg.ChatWindow = ChatWnd;
	pMsg.RkeyDefine = Rk;
	pMsg.QWERLevel = QWER;

	IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCMoneySend(CGameObject &Obj, DWORD money)
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

	IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);
}


void GameProtocol::GCItemObjectInventoryPutSend(CGameObject &Obj, BYTE result, BYTE iteminfo1, BYTE iteminfo2)
{
	PMSG_ITEMGETRESULT pResult;

	pResult.h.c = 0xC3;
	pResult.h.headcode = 0x22;
	pResult.h.size = sizeof(pResult);
	pResult.result = result;
	pResult.Data[0] = iteminfo1;
	pResult.Data[1] = iteminfo2;
	pResult.h.size -= 2;

	IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pResult, pResult.h.size);
}



void GameProtocol::CGItemGetRequest(PMSG_ITEMGETREQUEST * lpMsg, CGameObject &Obj, BYTE ID, BYTE ID1)
{
	int item_num, map_num;
	PMSG_ITEMGETRESULT pResult;
	int type;
	int level;
	int special;
	int NOption;
	char szItemName[50];
	CMapItem * lpItem;

	pResult.h.c = 0xC3;
	pResult.h.headcode = 0x22;
	pResult.h.size = sizeof(pResult);
	pResult.result = -1;

	if (!gObjIsConnected(aIndex))
	{
		IOCP.CloseClient(aIndex);
		return;
	}

	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	if (lpObj.CloseType != -1)
		return;

	if (lpObj.m_PlayerData->m_bSecurityCheck == false)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	if (lpObj.DieRegen != 0)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	if (g_NewPVP.IsObserver(getGameObject(aIndex)) == TRUE)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	if (lpObj.m_IfState.use != 0)
	{
		if (lpObj.m_IfState.type != 3)
		{
			this->GCServerMsgStringSend(Lang.GetText(0, 31), aIndex, 1);
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);
			IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pResult, pResult.h.size);

			return;
		}
	}

	if (!::gObjFixInventoryPointer(aIndex))

		if (lpObj.pTransaction == TRUE)
		{
			return;

		}

	CMuRummyInfo * pMuRummyInfo = lpObj.m_PlayerData->m_pCMuRummyInfo;

	if (!pMuRummyInfo)
	{
		return;
	}

	if (pMuRummyInfo->IsWaitReward() == true)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pResult, pResult.h.size);

		this->GCServerMsgStringSend(Lang.GetText(0, 561), aIndex, 1);
		return;
	}

	item_num = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	if (MaxItemTypeRange(item_num) == FALSE)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	map_num = lpObj.MapNumber;

	if (MAX_MAP_RANGE(map_num) == FALSE)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	lpItem = &MapC[map_num].m_CItemObject[item_num];

	if (lpItem->IsItem() == TRUE && lpItem->Give == false && lpItem->live == true)
	{
		if (IT_MAP_RANGE(map_num) == TRUE)
		{
			if (lpItem->m_Type == ITEMGET(14, 223))
			{
				if (g_ConfigRead.server.GetServerType() == SERVER_NORMAL)
				{
					if (g_IT_Event.CheckRelics(aIndex) == TRUE)
					{
						pResult.result = -1;
						pResult.h.size -= sizeof(pResult.Data);
						IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pResult, pResult.h.size);

						return;
					}
				}
			}
		}

		if (lpItem->m_QuestItem != false)
		{
			bool bIsGetItem = g_QuestInfo.GetQuestItem(aIndex, lpItem->m_Type, lpItem->m_Level);
			bool bIsQuestExpItem = g_QuestExpProgMng.IsQuestDropItem(aIndex, lpItem->m_Type, lpItem->m_Level);

			if (bIsGetItem == false && bIsQuestExpItem == false)
			{
				pResult.result = -1;
				pResult.h.size -= sizeof(pResult.Data);
				IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
				return;
			}
		}

		if (IsJumpingEventItem(lpItem->m_Type))
		{
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}

		LPITEM_ATTRIBUTE pItemAttribute = GetItemAttr(lpItem->m_Type);

		if (!pItemAttribute)
		{
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}

		if (pItemAttribute->ItemKindA == 12)
		{
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}

		if (lpItem->m_Type == ITEMGET(13, 20))	// Wizard Ring
		{
			switch (lpItem->m_Level)
			{
			case 0:
			{
				int iWRCount = gObjGetItemCountInIventory(aIndex, lpItem->m_Type / MAX_SUBTYPE_ITEMS, lpItem->m_Type%MAX_SUBTYPE_ITEMS, lpItem->m_Level);

				if (iWRCount > 0)
				{
					pResult.result = -1;
					pResult.h.size -= sizeof(pResult.Data);

					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
					this->GCServerMsgStringSend(Lang.GetText(0, 105), aIndex, 1);

					return;
				}
				break;
			}
			case 1:

				pResult.result = -1;
				pResult.h.size -= sizeof(pResult.Data);

				IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
				return;
				break;

			}
		}

		if (lpItem->m_Type == ITEMGET(13, 20) && lpItem->m_Level == 2)	// Wizard Ring
		{
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);

			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

			return;
		}

		if (lpItem->m_Type == ITEMGET(13, 38))	// MoonStonePendant
		{
			int count = gObjGetItemCountInIventory(aIndex, lpItem->m_Type / MAX_SUBTYPE_ITEMS,
				lpItem->m_Type % MAX_SUBTYPE_ITEMS, lpItem->m_Level);

			if (count > 0)
			{
				pResult.result = -1;
				pResult.h.size -= sizeof(pResult.Data);

				IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
				GCServerMsgStringSend(Lang.GetText(0, 285), aIndex, 1);

				return;
			}
		}

		if (lpItem->m_Type == ITEMGET(13, 39))	// ChangeRing
		{
			int count = gObjGetItemCountInIventory(aIndex, lpItem->m_Type / MAX_SUBTYPE_ITEMS,
				lpItem->m_Type % MAX_SUBTYPE_ITEMS, lpItem->m_Level);

			if (count > 0)
			{
				pResult.result = -1;
				pResult.h.size -= sizeof(pResult.Data);

				IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
				GCServerMsgStringSend(Lang.GetText(0, 287), aIndex, 1);

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
			if (MapC[map_num].ItemGive(aIndex, item_num, false) == TRUE)
			{
				if (!gObjCheckMaxZen(aIndex, lpItem->m_BuyMoney))
				{
					if (lpObj.m_PlayerData->Money < MAX_ZEN)
					{
						lpObj.m_PlayerData->Money = MAX_ZEN;
						pResult.result = -2;
						WORD hiWord = SET_NUMBERHW(lpObj.m_PlayerData->Money);
						WORD loWord = SET_NUMBERLW(lpObj.m_PlayerData->Money);
						pResult.Data[0] = SET_NUMBERH(hiWord);
						pResult.Data[1] = SET_NUMBERL(hiWord);
						pResult.Data[2] = SET_NUMBERH(loWord);
						pResult.Data[3] = SET_NUMBERL(loWord);

						IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

						return;
					}

					pResult.result = -1;
					pResult.h.size -= sizeof(pResult.Data);

					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

					return;
				}

				lpObj.m_PlayerData->Money += lpItem->m_BuyMoney;
				pResult.result = -2;

				WORD hiWord = SET_NUMBERHW(lpObj.m_PlayerData->Money);
				WORD loWord = SET_NUMBERLW(lpObj.m_PlayerData->Money);
				pResult.Data[0] = SET_NUMBERH(hiWord);
				pResult.Data[1] = SET_NUMBERL(hiWord);
				pResult.Data[2] = SET_NUMBERH(loWord);
				pResult.Data[3] = SET_NUMBERL(loWord);
				pResult.h.size -= 3;
			}

			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

		}
		else
		{
			if (lpItem->m_Type == ITEMGET(13, 32))
			{
				int pos = gObjOverlapItemUsingDur((lpItem) ? ((CItemObject *)&lpItem->m_Number) : NULL, map_num, item_num, aIndex, IsOverlapItem(lpItem->m_Type), ITEMGET(13, 32), 0);

				if (MAIN_INVENTORY_RANGE(pos) != FALSE)
				{
					pResult.result = -3;
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
					lpObj.pInventory[pos].m_Durability += lpItem->m_Durability;
					GCItemObjectDurSend(aIndex, pos, lpObj.pInventory[pos].m_Durability, 0);

					return;
				}
			}

			if (lpItem->m_Type == ITEMGET(13, 33))
			{
				int pos = gObjOverlapItemUsingDur((lpItem) ? ((CItemObject *)&lpItem->m_Number) : NULL, map_num, item_num, aIndex, IsOverlapItem(lpItem->m_Type), ITEMGET(13, 33), 0);

				if (MAIN_INVENTORY_RANGE(pos) != FALSE)
				{
					pResult.result = -3;
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
					lpObj.pInventory[pos].m_Durability += lpItem->m_Durability;
					GCItemObjectDurSend(aIndex, pos, lpObj.pInventory[pos].m_Durability, 0);

					return;
				}
			}

			if (lpItem->m_Type == ITEMGET(13, 34))
			{
				int pos = gObjOverlapItemUsingDur((lpItem) ? ((CItemObject *)&lpItem->m_Number) : NULL, map_num, item_num, aIndex, IsOverlapItem(lpItem->m_Type), ITEMGET(13, 34), 0);

				if (MAIN_INVENTORY_RANGE(pos) != FALSE)
				{
					pResult.result = -3;
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
					lpObj.pInventory[pos].m_Durability += lpItem->m_Durability;
					GCItemObjectDurSend(aIndex, pos, lpObj.pInventory[pos].m_Durability, 0);

					return;
				}
			}

			if (lpItem->m_Type == ITEMGET(14, 70) || lpItem->m_Type == ITEMGET(14, 71) || lpItem->m_Type == ITEMGET(14, 94)
				|| lpItem->m_Type == ITEMGET(14, 133)	//6.2
				)
			{
				int pos = gObjOverlapItemUsingDur((lpItem) ? ((CItemObject *)&lpItem->m_Number) : NULL, map_num, item_num, aIndex, 50, lpItem->m_Type, 0);

				if (MAIN_INVENTORY_RANGE(pos) != FALSE)
				{
					pResult.result = -3;
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
					lpObj.pInventory[pos].m_Durability += lpItem->m_Durability;
					GCItemObjectDurSend(aIndex, pos, lpObj.pInventory[pos].m_Durability, 0);
					return;
				}
			}

			if (lpItem->m_Type == ITEMGET(13, 147)) // Trophy of Battle
			{
				int pos = gObjOverlapItemUsingDur((lpItem) ? ((CItemObject *)&lpItem->m_Number) : NULL, map_num, item_num, aIndex, IsOverlapItem(lpItem->m_Type), ITEMGET(13, 147), 0);

				if (MAIN_INVENTORY_RANGE(pos) != FALSE)
				{
					pResult.result = -3;
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
					lpObj.pInventory[pos].m_Durability += lpItem->m_Durability;
					GCItemObjectDurSend(aIndex, pos, lpObj.pInventory[pos].m_Durability, 0);

					return;
				}
			}

			if (lpItem->m_Type == ITEMGET(14, 29)) // Symbol of Kundun
			{
				for (int n = 0; n < MAIN_INVENTORY_SIZE; n++)
				{
					int pos = ::g_KalimaGate.CheckOverlapKundunMark(aIndex, lpItem->m_Level);

					if (MAIN_INVENTORY_RANGE(pos))
					{
						if (MapC[map_num].ItemGive(aIndex, item_num, true) == TRUE)
						{
							BYTE NewOption[MAX_EXOPTION_SIZE];

							::ItemIsBufExOption(NewOption, (lpItem != NULL) ? (CItemObject*)&lpItem->m_Number : NULL);

							pResult.result = -3;

							IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

							lpObj.pInventory[pos].m_Durability += lpItem->m_Durability;

							if (lpObj.pInventory[pos].m_Durability >= IsOverlapItem(lpItem->m_Type))
							{
								int NewDur = lpObj.pInventory[pos].m_Durability - IsOverlapItem(lpItem->m_Type);
								::gObjInventoryItemSet(aIndex, pos, -1);
								lpObj.pInventory[pos].Clear();
								this->GCInventoryItemDeleteSend(aIndex, pos, 1);
								::ItemSerialCreateSend(aIndex, 235, lpObj.X, lpObj.Y, ItemGetNumberMake(14, 28),
									lpItem->m_Level, 0, 0, 0, 0, aIndex, 0, 0, 0, 0, 0);

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
								GCItemObjectDurSend(aIndex, pos, lpObj.pInventory[pos].m_Durability, 0);

							}
							return;
						}
					}
					else
					{
						break;
					}
				}
			}
			if (lpItem->m_Type == ITEMGET(14, 110)) // Double Goer
			{
				int pos = gObjOverlapItemUsingDur((lpItem) ? ((CItemObject *)&lpItem->m_Number) : NULL, map_num, item_num, aIndex, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 110), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
					lpObj.pInventory[pos].m_Durability += lpItem->m_Durability;

					if (lpObj.pInventory[pos].m_Durability >= IsOverlapItem(lpItem->m_Type))
					{
						int NewDur = lpObj.pInventory[pos].m_Durability - IsOverlapItem(lpItem->m_Type);
						::gObjInventoryItemSet(aIndex, pos, -1);
						lpObj.pInventory[pos].Clear();
						this->GCInventoryItemDeleteSend(aIndex, pos, 1);
						::ItemSerialCreateSend(aIndex, 235, lpObj.X, lpObj.Y, ItemGetNumberMake(14, 111),
							lpItem->m_Level, 0, 0, 0, 0, aIndex, 0, 0, 0, 0, 0);

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
						GCItemObjectDurSend(aIndex, pos, lpObj.pInventory[pos].m_Durability, 0);

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
					int iInventoryPos = g_PentagramSystem.CheckOverlapMythrilPiece(aIndex, lpItem->m_Type, btMainAtribute);

					if (!MAIN_INVENTORY_RANGE(iInventoryPos))
					{
						break;
					}

					int Dur = lpObj.pInventory[iInventoryPos].m_Durability + lpItem->m_Durability;

					if (Dur <= IsOverlapItem(lpItem->m_Type) && MapC[map_num].ItemGive(aIndex, item_num, 1) == TRUE)
					{
						BYTE NewOption[MAX_EXOPTION_SIZE];

						::ItemIsBufExOption(NewOption, (lpItem != NULL) ? (CItemObject*)&lpItem->m_Number : NULL);

						pResult.result = -3;
						IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

						lpObj.pInventory[iInventoryPos].m_Durability += lpItem->m_Durability;

						if (lpObj.pInventory[iInventoryPos].m_Durability < IsOverlapItem(lpItem->m_Type))
						{
							this->GCItemObjectDurSend(aIndex, iInventoryPos, lpObj.pInventory[iInventoryPos].m_Durability, 0);
							return;
						}

						int NewDur = lpObj.pInventory[iInventoryPos].m_Durability - IsOverlapItem(lpItem->m_Type);
						::gObjInventoryItemSet(aIndex, iInventoryPos, -1);
						lpObj.pInventory[iInventoryPos].Clear();
						this->GCInventoryItemDeleteSend(aIndex, iInventoryPos, 1);

						if (lpItem->m_Type == ITEMGET(12, 144))
						{
							ItemSerialCreateSend(aIndex, 233, lpObj.X, lpObj.Y, ItemGetNumberMake(12, 145), 0, 0, 0, 0, 0,
								aIndex, 0, 0, 0, 0, lpItem->m_BonusSocketOption);

							sLog->outBasic("[Pentagram] [%s][%s] Make Mythril (Left:%d)", lpObj.AccountID, lpObj.Name, NewDur);
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
				int pos = gObjOverlapItemUsingDur((lpItem) ? ((CItemObject *)&lpItem->m_Number) : NULL, map_num, item_num, aIndex, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 290), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
					lpObj.pInventory[pos].m_Durability += lpItem->m_Durability;

					if (lpObj.pInventory[pos].m_Durability >= IsOverlapItem(lpItem->m_Type))
					{
						int NewDur = lpObj.pInventory[pos].m_Durability - IsOverlapItem(lpItem->m_Type);
						::gObjInventoryItemSet(aIndex, pos, -1);
						lpObj.pInventory[pos].Clear();
						this->GCInventoryItemDeleteSend(aIndex, pos, 1);
						::ItemSerialCreateSend(aIndex, 235, lpObj.X, lpObj.Y, ItemGetNumberMake(14, 293), lpItem->m_Level, 0, 0, 0, 0, aIndex, 0, 0, 0, 0, 0);

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
						GCItemObjectDurSend(aIndex, pos, lpObj.pInventory[pos].m_Durability, 0);

					}
					return;
				}
				else
				{

				}
			}

			if (lpItem->m_Type == ITEMGET(13, 145)) // Spirit Map
			{
				int pos = gObjOverlapItemUsingDur((lpItem) ? ((CItemObject *)&lpItem->m_Number) : NULL, map_num, item_num, aIndex, IsOverlapItem(lpItem->m_Type), ITEMGET(13, 145), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
					lpObj.pInventory[pos].m_Durability += lpItem->m_Durability;

					if (lpObj.pInventory[pos].m_Durability >= IsOverlapItem(lpItem->m_Type))
					{
						int NewDur = lpObj.pInventory[pos].m_Durability - IsOverlapItem(lpItem->m_Type);
						::gObjInventoryItemSet(aIndex, pos, -1);
						lpObj.pInventory[pos].Clear();
						this->GCInventoryItemDeleteSend(aIndex, pos, 1);
						::ItemSerialCreateSend(aIndex, 235, lpObj.X, lpObj.Y, ItemGetNumberMake(13, 146),
							lpItem->m_Level, 0, 0, 0, 0, aIndex, 0, 0, 0, 0, 0);

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
						GCItemObjectDurSend(aIndex, pos, lpObj.pInventory[pos].m_Durability, 0);

					}
					return;
				}
				else
				{

				}
			}
			if (lpItem->m_Type == ITEMGET(14, 101)) // Imperial Fort
			{
				int pos = gObjOverlapItemUsingDur((lpItem) ? ((CItemObject *)&lpItem->m_Number) : NULL, map_num, item_num, aIndex, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 101), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
					lpObj.pInventory[pos].m_Durability += lpItem->m_Durability;

					if (lpObj.pInventory[pos].m_Durability >= IsOverlapItem(lpItem->m_Type))
					{
						int NewDur = lpObj.pInventory[pos].m_Durability - IsOverlapItem(lpItem->m_Type);
						::gObjInventoryItemSet(aIndex, pos, -1);
						lpObj.pInventory[pos].Clear();
						this->GCInventoryItemDeleteSend(aIndex, pos, 1);
						::ItemSerialCreateSend(aIndex, 235, lpObj.X, lpObj.Y, ItemGetNumberMake(14, 102),
							lpItem->m_Level, 0, 0, 0, 0, aIndex, 0, 0, 0, 0, 0);

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
						GCItemObjectDurSend(aIndex, pos, lpObj.pInventory[pos].m_Durability, 0);

					}
					return;
				}
				else
				{

				}
			}

			if (lpItem->m_Type == ITEMGET(14, 100)) // Lucky Coin
			{
				int pos = gObjOverlapItemUsingDur((lpItem) ? ((CItemObject *)&lpItem->m_Number) : NULL, map_num, item_num, aIndex, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 100), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
					lpObj.pInventory[pos].m_Durability += lpItem->m_Durability;

					GCItemObjectDurSend(aIndex, pos, lpObj.pInventory[pos].m_Durability, 0);

					return;
				}
				else
				{

				}
			}

			if (lpItem->m_Type == ITEMGET(14, 21) && lpItem->m_Level == 3) // Sign of Lord
			{
				for (int n = 0; n < MAIN_INVENTORY_SIZE; n++)
				{
					int pos = ::g_CastleSiegeSync.CheckOverlapCsMarks(aIndex);

					if (MAIN_INVENTORY_RANGE(pos))
					{
						int Dur = lpObj.pInventory[pos].m_Durability + lpItem->m_Durability;

						if (Dur <= IsOverlapItem(lpItem->m_Type))
						{
							if (MapC[map_num].ItemGive(aIndex, item_num, true) == TRUE)
							{
								pResult.result = -3;
								IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
								lpObj.pInventory[pos].m_Durability += lpItem->m_Durability;
								GCItemObjectDurSend(aIndex, pos, lpObj.pInventory[pos].m_Durability, 0);
								return;
							}
						}
						else
						{
							lpItem->m_Durability = Dur - IsOverlapItem(lpItem->m_Type);
							lpObj.pInventory[pos].m_Durability = IsOverlapItem(lpItem->m_Type);
							GCItemObjectDurSend(aIndex, pos, lpObj.pInventory[pos].m_Durability, 0);
						}

					}
					else
					{
						break;
					}
				}
			}

			if (lpItem->m_Type == ITEMGET(14, 90)) // Golden cherry blossom
			{
				int pos = gObjOverlapItemUsingDur((lpItem) ? ((CItemObject *)&lpItem->m_Number) : NULL, map_num, item_num, aIndex, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 90), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

					lpObj.pInventory[pos].m_Durability += lpItem->m_Durability;

					GCItemObjectDurSend(aIndex, pos, lpObj.pInventory[pos].m_Durability, 0);

					return;
				}

				else
				{

				}
			}

			if (lpItem->m_Type == ITEMGET(14, 89)) // Red cherry blossom
			{
				int pos = gObjOverlapItemUsingDur((lpItem) ? ((CItemObject *)&lpItem->m_Number) : NULL, map_num, item_num, aIndex, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 89), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

					lpObj.pInventory[pos].m_Durability += lpItem->m_Durability;

					GCItemObjectDurSend(aIndex, pos, lpObj.pInventory[pos].m_Durability, 0);

					return;
				}

				else
				{

				}
			}

			if (lpItem->m_Type == ITEMGET(14, 88)) // White cherry blossom
			{
				int pos = gObjOverlapItemUsingDur((lpItem) ? ((CItemObject *)&lpItem->m_Number) : NULL, map_num, item_num, aIndex, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 88), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

					lpObj.pInventory[pos].m_Durability += lpItem->m_Durability;

					GCItemObjectDurSend(aIndex, pos, lpObj.pInventory[pos].m_Durability, 0);

					return;
				}

				else
				{

				}
			}

			if (lpItem->m_Type == ITEMGET(14, 153)) // White cherry blossom
			{
				int pos = gObjOverlapItemUsingDur((lpItem) ? ((CItemObject *)&lpItem->m_Number) : NULL, map_num, item_num, aIndex, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 153), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

					lpObj.pInventory[pos].m_Durability += lpItem->m_Durability;

					GCItemObjectDurSend(aIndex, pos, lpObj.pInventory[pos].m_Durability, 0);

					return;
				}

				else
				{

				}
			}

			if (lpItem->m_Type == ITEMGET(14, 154)) // White cherry blossom
			{
				int pos = gObjOverlapItemUsingDur((lpItem) ? ((CItemObject *)&lpItem->m_Number) : NULL, map_num, item_num, aIndex, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 154), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

					lpObj.pInventory[pos].m_Durability += lpItem->m_Durability;

					GCItemObjectDurSend(aIndex, pos, lpObj.pInventory[pos].m_Durability, 0);

					return;
				}

				else
				{

				}
			}

			if (lpItem->m_Type == ITEMGET(14, 155)) // White cherry blossom
			{
				int pos = gObjOverlapItemUsingDur((lpItem) ? ((CItemObject *)&lpItem->m_Number) : NULL, map_num, item_num, aIndex, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 155), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

					lpObj.pInventory[pos].m_Durability += lpItem->m_Durability;

					GCItemObjectDurSend(aIndex, pos, lpObj.pInventory[pos].m_Durability, 0);

					return;
				}

				else
				{

				}
			}

			if (lpItem->m_Type == ITEMGET(14, 156)) // White cherry blossom
			{
				int pos = gObjOverlapItemUsingDur((lpItem) ? ((CItemObject *)&lpItem->m_Number) : NULL, map_num, item_num, aIndex, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 156), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

					lpObj.pInventory[pos].m_Durability += lpItem->m_Durability;

					GCItemObjectDurSend(aIndex, pos, lpObj.pInventory[pos].m_Durability, 0);

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

				ItemIsBufExOption(NewOption, (lpItem != NULL) ? (CItemObject*)&lpItem->m_Number : NULL);

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

				IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
				this->GCServerMsgStringSend(Lang.GetText(0, 519), aIndex, 1);
				return;
			}

			if (lpItem->m_QuestItem == true && g_QuestExpManager.IsQuestItemAtt(lpItem->m_Type, 1) == true)
			{
				for (int n = 12; n < INVENTORY_SIZE; n++)
				{
					int pos = -1;

					if (lpObj.pInventory[n].m_Type == lpItem->m_Type &&
						lpObj.pInventory[n].m_Durability < IsOverlapItem(lpItem->m_Type))
					{
						pos = n;
					}

					if (INVENTORY_RANGE(pos) == true)
					{
						if (lpObj.pInventory[pos].m_Durability + lpItem->m_Durability > IsOverlapItem(lpItem->m_Type))
						{
							lpItem->m_Durability = lpObj.pInventory[pos].m_Durability + lpItem->m_Durability - IsOverlapItem(lpItem->m_Type);
							lpObj.pInventory[pos].m_Durability = IsOverlapItem(lpItem->m_Type);
							this->GCItemObjectDurSend(aIndex, pos, lpObj.pInventory[pos].m_Durability, 0);
						}

						else
						{
							if (MapC[map_num].ItemGive(aIndex, item_num, 1) == TRUE)
							{
								pResult.result = -3;
								IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

								lpObj.pInventory[pos].m_Durability += lpItem->m_Durability;
								GCItemObjectDurSend(aIndex, pos, lpObj.pInventory[pos].m_Durability, 0);
								return;
							}
						}
					}
				}
			}

			pResult.result = ::gObjInventoryInsertItemTemp(&getGameObject(aIndex), lpItem);

			if (pResult.result != 0xFF)
			{
				ItemByteConvert((BYTE*)&pResult.Data[0], lpItem->m_Type, lpItem->m_Option1, lpItem->m_Option2,
					lpItem->m_Option3, lpItem->m_Level, lpItem->m_Durability, lpItem->m_NewOption,
					lpItem->m_SetOption, lpItem->m_JewelOfHarmonyOption, lpItem->m_ItemOptionEx, lpItem->m_SocketOption, lpItem->m_BonusSocketOption, 0);
				pResult.i = ID;
				pResult.ii = ID1;
				pResult.x = 0xFF;
				pResult.xx = 0x0C;
				if (MapC[map_num].ItemGive(aIndex, item_num, false) == TRUE)
				{
					BYTE pos = ::gObjInventoryInsertItem(aIndex, lpItem);

					if (pos == 0xFF)
						pResult.result = -1;

					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

					if (pos == 0xFF)
					{
						::GCItemObjectListSend(aIndex);
						g_PeriodItemEx.OnRequestPeriodItemList(&getGameObject(aIndex));
					}
					else
					{
						BYTE NewOption[MAX_EXOPTION_SIZE];

						::ItemIsBufExOption(NewOption, (lpItem != NULL) ? (CItemObject*)&lpItem->m_Number : NULL);

						this->GCSendGetItemInfoForParty(aIndex, lpItem);

						if (BC_MAP_RANGE(map_num))
						{
							int iBridgeIndex = g_BloodCastle.GetBridgeIndex(map_num);
							if (g_BloodCastle.CheckQuestItemSerial(iBridgeIndex, lpItem))
							{
								g_BloodCastle.CatchQuestItemByUser(iBridgeIndex, aIndex, lpItem->m_Level);
							}
						}

						if (IT_MAP_RANGE(map_num) && lpItem->m_Type == ITEMGET(14, 223))
						{
							if (g_ConfigRead.server.GetServerType() == SERVER_NORMAL)
							{
								g_IT_Event.SetRelicsInventoryPos(lpObj.MapNumber, aIndex, pos);
								g_IT_Event.SendRelicsUserInfo(&getGameObject(aIndex));

							}
						}

						if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
						{
							if (lpObj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
							{
								g_ChaosCastleFinal.SetUBFGetReward(aIndex, lpItem->m_Type, lpItem->m_Number, pos);
							}

							else if (lpObj.MapNumber == MAP_INDEX_DEVILSQUARE_FINAL)
							{
								g_DevilSquareFinal.SetUBFGetReward(aIndex, lpItem->m_Type, lpItem->m_Number, pos);
							}

							else if (CC_MAP_RANGE(lpObj.MapNumber) == TRUE)
							{
								g_ChaosCastle.SetUBFGetReward(aIndex, lpItem->m_Type, lpItem->m_Number, pos);
							}
						}

						if (g_ArcaBattle.IsArcaBattleServer() == TRUE && lpItem->m_Type == ITEMGET(13, 147))
						{
							g_ArcaBattle.BootyItemGetCnt(&getGameObject(aIndex));
						}

						if (lpItem->m_PeriodItemDuration > 0)
						{
							g_PeriodItemEx.SetPeriodItemInfo(&getGameObject(aIndex), lpItem->m_Type, lpItem->m_Number, lpItem->m_PeriodItemDuration);

							this->GCInventoryItemOneSend(aIndex, pos);
							g_PeriodItemEx.SendPeriodItemInfoOnce(&getGameObject(aIndex], &lpObj.pInventory[pos));
						}
					}
				}
				else
				{
					pResult.result = -1;
					pResult.h.size -= sizeof(pResult.Data);

					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
				}
			}
			else
			{
				pResult.result = -1;
				pResult.h.size -= sizeof(pResult.Data);

				IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
			}
		}
	}
	else
	{
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pResult, pResult.h.size);
	}
}

bool GameProtocol::CGItemDropRequest(PMSG_ITEMTHROW * lpMsg, CGameObject &Obj, BOOL drop_type)
{
	BOOL CopyItem = FALSE;
	BYTE NewOption[MAX_EXOPTION_SIZE];

	if (!PacketCheckTime(aIndex))
		return FALSE;

	if (lpObj.Type != OBJ_USER)
	{
		return FALSE;
	}

	PMSG_ITEMTHROW_RESULT pResult;

	PHeadSetB((BYTE*)&pResult, 0x23, sizeof(pResult));
	pResult.Result = true;
	pResult.Ipos = lpMsg->Ipos;

	if (lpObj.m_IfState.use != 0)
	{
		if (lpObj.m_IfState.type != 3)
		{
			pResult.Result = false;
			return FALSE;
		}
	}

	if (lpObj.m_PlayerData->m_bSecurityCheck == false)
	{
		MsgOutput(aIndex, Lang.GetText(0, 822));
		pResult.Result = false;
	}

	if (lpObj.CloseType != -1)
		pResult.Result = false;

	if (lpObj.m_bMapSvrMoveQuit == true)
	{
		pResult.Result = false;
	}

	if (!::gObjFixInventoryPointer(aIndex))

		if (lpObj.pTransaction == 1)
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

	if (!gObjCanItemTouch(lpObj, 0))
		pResult.Result = false;

	if (!IsDumpItem(lpObj.pInventory[lpMsg->Ipos].m_Type) && (lpObj.pInventory[lpMsg->Ipos].m_Type != ITEMGET(13, 20) || !lpObj.pInventory[lpMsg->Ipos].m_Level))
		pResult.Result = false;

	if (g_LuckyItemManager.IsLuckyItemEquipment(lpObj.pInventory[lpMsg->Ipos].m_Type) || g_LuckyItemManager.IsLuckyItemTicket(lpObj.pInventory[lpMsg->Ipos].m_Type))
	{
		pResult.Result = false;
	}

	// Icarus Preventions about Dinorant
	if (lpObj.MapNumber == MAP_INDEX_ICARUS)
	{
		if (lpMsg->Ipos == lpObj.m_btInvenPetPos)
		{
			if (lpObj.pInventory[lpMsg->Ipos].m_Type == ITEMGET(13, 3)) // Dinorant
			{
				if (lpObj.pInventory[7].IsItem() == FALSE)
				{
					pResult.Result = false;
				}
			}
		}
		else if (lpMsg->Ipos == 7)
		{
			if (lpObj.pInventory[lpObj.m_btInvenPetPos].m_Type != ITEMGET(13, 3)) // Dinorant 
			{
				pResult.Result = false;
			}
		}
	}

	if (lpObj.pInventory[lpMsg->Ipos].m_Type == ITEMGET(12, 30) ||
		lpObj.pInventory[lpMsg->Ipos].m_Type == ITEMGET(12, 31) ||
		lpObj.pInventory[lpMsg->Ipos].m_Type == ITEMGET(13, 36) ||
		lpObj.pInventory[lpMsg->Ipos].m_Type == ITEMGET(13, 37))
	{
		pResult.Result = false;
	}

	if (GetWingType(lpObj.pInventory[lpMsg->Ipos].m_Type, WCF_ITEMNUMBER) != NONE_WING && g_ConfigRead.pk.bPkPenaltyDisable == FALSE)
	{
		pResult.Result = false;
	}
	if (g_ConfigRead.data.common.joinmuRemoveItemSellDropRestriction == false)
	{
		if ((lpObj.pInventory[lpMsg->Ipos].m_Type >= ITEMGET(13, 0) && lpObj.pInventory[lpMsg->Ipos].m_Type <= ITEMGET(13, 3))
			|| lpObj.pInventory[lpMsg->Ipos].m_Type == ITEMGET(14, 13) ||
			lpObj.pInventory[lpMsg->Ipos].m_Type == ITEMGET(14, 14) ||
			lpObj.pInventory[lpMsg->Ipos].m_Type == ITEMGET(14, 16) ||
			(lpObj.pInventory[lpMsg->Ipos].m_Type >= ITEMGET(12, 0)
				&& lpObj.pInventory[lpMsg->Ipos].m_Type <= ITEMGET(12, 6)) ||
				(lpObj.pInventory[lpMsg->Ipos].m_Type >= ITEMGET(12, 36)
					&& lpObj.pInventory[lpMsg->Ipos].m_Type <= ITEMGET(12, 40))
			|| lpObj.pInventory[lpMsg->Ipos].m_Type == ITEMGET(12, 15)
			|| (lpObj.pInventory[lpMsg->Ipos].m_Level > g_ConfigRead.pk.iPkMaxLevelItemDrop
				&& lpObj.pInventory[lpMsg->Ipos].m_Type < ITEMGET(12, 0))
			|| (lpObj.pInventory[lpMsg->Ipos].IsSetItem() != FALSE && !g_ConfigRead.pk.bPkDropExpensiveItems)
			|| (lpObj.pInventory[lpMsg->Ipos].IsExtItem() != FALSE && !g_ConfigRead.pk.bPkDropExpensiveItems))
		{
			if (g_ConfigRead.pk.bPkPenaltyDisable == FALSE)
			{
				pResult.Result = false;
			}
		}
	}
	if (g_kJewelOfHarmonySystem.IsStrengthenByJewelOfHarmony(&lpObj.pInventory[lpMsg->Ipos]) == TRUE)
	{
		pResult.Result = false;
		this->GCServerMsgStringSend(Lang.GetText(0, 272), lpObj.m_Index, 1);
	}

	if (g_kJewelOfHarmonySystem.IsJewelOfHarmonyOriginal(lpObj.pInventory[lpMsg->Ipos].m_Type) ||
		g_kJewelOfHarmonySystem.IsJewelOfHarmonyPurity(lpObj.pInventory[lpMsg->Ipos].m_Type) ||
		g_kJewelOfHarmonySystem.IsJewelOfHarmonySmeltingItems(lpObj.pInventory[lpMsg->Ipos].m_Type))
	{
		pResult.Result = false;
	}

	if (lpObj.pInventory[lpMsg->Ipos].m_Type == ITEMGET(13, 39))
	{
		pResult.Result = false;
	}

	if (lpObj.pInventory[lpMsg->Ipos].IsPeriodItem() == TRUE)
	{
		pResult.Result = false;
	}

	if (g_NewPVP.IsDuel(*lpObj))
	{
		pResult.Result = false;
	}

	if (IsJumpingEventItem(lpObj.pInventory[lpMsg->Ipos].m_Type) == TRUE)
	{
		pResult.Result = false;
	}

	if (pResult.Result == 1)
	{
		::ItemIsBufExOption(NewOption, &lpObj.pInventory[lpMsg->Ipos]);

		int map = lpObj.MapNumber;
		int type = lpObj.pInventory[lpMsg->Ipos].m_Type;
		int level = lpObj.pInventory[lpMsg->Ipos].m_Level;
		float dur = lpObj.pInventory[lpMsg->Ipos].m_Durability;
		BOOL bIsItem = lpObj.pInventory[lpMsg->Ipos].IsItem();
		BYTE Option1 = lpObj.pInventory[lpMsg->Ipos].m_Option1;
		BYTE Option2 = lpObj.pInventory[lpMsg->Ipos].m_Option2;
		BYTE Option3 = lpObj.pInventory[lpMsg->Ipos].m_Option3;
		BYTE NOption = lpObj.pInventory[lpMsg->Ipos].m_NewOption;
		UINT64 serial = lpObj.pInventory[lpMsg->Ipos].m_Number;
		int PetLevel = lpObj.pInventory[lpMsg->Ipos].m_PetItem_Level;
		UINT64 PetExp = lpObj.pInventory[lpMsg->Ipos].m_PetItem_Exp;
		BYTE SOption = lpObj.pInventory[lpMsg->Ipos].m_SetOption;
		BYTE btItemEffectEx = lpObj.pInventory[lpMsg->Ipos].m_ItemOptionEx;
		UINT64 serial2 = lpObj.pInventory[lpMsg->Ipos].m_Number;
		char szItemName[50] = "Item";
		BYTE JOHOption = g_kJewelOfHarmonySystem.GetItemStrengthenOption(&lpObj.pInventory[lpMsg->Ipos]);
		BYTE JOHOptionLevel = g_kJewelOfHarmonySystem.GetItemOptionLevel(&lpObj.pInventory[lpMsg->Ipos]);

		//Season 4 add-on
		BYTE SocketOption[5];
		BYTE SocketIndex;
		g_SocketOptionSystem.GetSocketOption(&lpObj.pInventory[lpMsg->Ipos], SocketOption, SocketIndex);

		for (int iOption = 0; iOption < 5; iOption++)
		{
			SocketOption[iOption] = lpObj.pInventory[lpMsg->Ipos].m_SocketOption[iOption];
		}

		if (gObjCheckSerial0ItemList(&lpObj.pInventory[lpMsg->Ipos]) != FALSE)
		{
			MsgOutput(lpObj.m_Index, Lang.GetText(0, 259));

			CopyItem = TRUE;
			pResult.Result = false;
			bIsItem = FALSE;
		}

		if (!::gObjInventorySearchSerialNumber(lpObj, serial2))	// ANTI DUPE SYSTEM
		{
			CopyItem = TRUE;
			pResult.Result = false;
			bIsItem = FALSE;
		}

		if (bIsItem == TRUE)
		{
			strcpy(szItemName, lpObj.pInventory[lpMsg->Ipos].GetName());

			if (g_BagManager.SearchAndUseBag(lpObj.m_Index, BAG_COMMON, type, level) == true)
			{
				gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
			}
			else if (type == ITEMGET(14, 51))
			{
				gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
				FireworksOpenEven(&getGameObject(aIndex));
			}
			else if (type == ITEMGET(14, 63))
			{
				gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
				FireworksOpenEven(&getGameObject(aIndex));

			}
			else if (type == ITEMGET(14, 210))
			{
				if (NewYearSummonMonster(lpObj, 647) == FALSE)
				{
					pResult.Result = false;
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}

				gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
			}

			else if (type == ITEMGET(14, 211))
			{
				if (NewYearSummonMonster(lpObj, 648) == FALSE)
				{
					pResult.Result = false;
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}

				gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
			}

			else if (type == ITEMGET(14, 212))
			{
				if (NewYearSummonMonster(lpObj, 649) == FALSE)
				{
					pResult.Result = false;
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}

				gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
			}

			else if (type == ITEMGET(14, 213))
			{
				if (NewYearSummonMonster(lpObj, 650) == FALSE)
				{
					pResult.Result = false;
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}

				gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
			}

			else if (type == ITEMGET(14, 217))
			{
				if (SummonGoldColossusMonster(lpObj, 653, 1, 0) == FALSE)
				{
					pResult.Result = false;
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}

				gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
			}

			else if (type == ITEMGET(14, 218))
			{
				if (SummonGoldColossusMonster(lpObj, 654, 1, 0) == FALSE)
				{
					pResult.Result = false;
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}

				gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
			}

			else if (type == ITEMGET(14, 219))
			{
				if (SummonGoldColossusMonster(lpObj, 655, 1, 0) == FALSE)
				{
					pResult.Result = false;
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}

				gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
			}

			else if (type == ITEMGET(14, 220))
			{
				if (SummonGoldColossusMonster(lpObj, 656, 1, 0) == FALSE)
				{
					pResult.Result = false;
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}

				gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
			}

			else if (type == ITEMGET(14, 221))
			{
				if (SummonGoldColossusMonster(lpObj, 657, 1, 0) == FALSE)
				{
					pResult.Result = false;
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}

				SummonGoldColossusMonster(lpObj, 654, 5, 1);
				gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
			}

			else if (type == ITEMGET(14, 234))
			{
				if (SummonPetEventMonster(lpObj) == FALSE)
				{
					pResult.Result = false;
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}

				gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
			}
			else if (type == ITEMGET(14, 286))
			{
				gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
				pResult.Result = true;

				BYTE SocketBonus = (lpObj.m_iPentagramMainAttribute == ELEMENT_NONE) ? (1 + rand() % 5) : lpObj.m_iPentagramMainAttribute;
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

				ItemSerialCreateSend(aIndex, lpObj.MapNumber, lpObj.X, lpObj.Y, ITEMGET(12, 215), 0, 0, 0, 0, 0, lpObj.m_Index, 0, 0, 0, SocketOption, SocketBonus);
				ItemSerialCreateSend(aIndex, lpObj.MapNumber, lpObj.X, lpObj.Y, ITEMGET(12, 222), 0, 0, 0, 0, 0, lpObj.m_Index, 0, 0, 0, btSocketOption, SocketBonus);
				ItemSerialCreateSend(aIndex, lpObj.MapNumber, lpObj.X, lpObj.Y, ITEMGET(12, 232), 0, 0, 0, 0, 0, lpObj.m_Index, 0, 0, 0, btSocketOption, SocketBonus);
				ItemSerialCreateSend(aIndex, lpObj.MapNumber, lpObj.X, lpObj.Y, ITEMGET(12, 242), 0, 0, 0, 0, 0, lpObj.m_Index, 0, 0, 0, btSocketOption, SocketBonus);
				ItemSerialCreateSend(aIndex, lpObj.MapNumber, lpObj.X, lpObj.Y, ITEMGET(12, 252), 0, 0, 0, 0, 0, lpObj.m_Index, 0, 0, 0, btSocketOption, SocketBonus);
			}

			else if (type == ITEMGET(12, 26))
			{
				switch (level)
				{
				case 0:
				{
					gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
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

					ItemSerialCreateSend(aIndex, lpObj.MapNumber, lpObj.X, lpObj.Y,
						ltype, Level, 1, 0, 0, 0, lpObj.m_Index, 0, 0, 0, 0, 0);

				}
				break;
				case 1: case 2: case 3:
				{
					gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
					pResult.Result = true;

					PMSG_SERVERCMD ServerCmd;

					PHeadSubSetB((BYTE*)&ServerCmd, 0xF3, 0x40, sizeof(ServerCmd));
					ServerCmd.CmdType = 0;
					ServerCmd.X = lpObj.X;
					ServerCmd.Y = lpObj.Y;

					MsgSendV2(lpObj, (BYTE*)&ServerCmd, sizeof(ServerCmd));
					IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (BYTE*)&ServerCmd, sizeof(ServerCmd));

				}
				break;
				case 4:

					break;
				case 5:
					gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
					pResult.Result = true;

					if ((rand() % 10000) < g_iHiddenTreasureBoxOfflineRate)
					{
						EGReqRegHTOfflineGift(lpObj.m_Index);
					}
					else
					{
						int money = 1000000;
						MapC[lpObj.MapNumber].MoneyItemDrop(money, lpObj.X, lpObj.Y);

					}
					break;
				}
			}
			else if (type == ITEMGET(13, 20) && level == 1) // Wizard Ring A
			{
				if (lpObj.Level >= 40)
				{
					::gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
					g_BagManager.SearchAndUseBag(aIndex, BAG_EVENT, EVENTBAG_WARRIORRING_1, aIndex);
				}
				else
				{
					PMSG_NOTICE pNotice;

					TNotice::MakeNoticeMsg((TNotice *)&pNotice, 10, Lang.GetText(0, 106));
					TNotice::SetNoticeProperty((TNotice *)&pNotice, 10, _ARGB(255, 128, 149, 196), 1, 0, 20);
					TNotice::SendNoticeToUser(aIndex, (TNotice *)&pNotice);
					pResult.Result = false;
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}
			}
			else if (type == ITEMGET(13, 20) && level == 2) // Wizard Ring B
			{
				if (lpObj.Level >= 80)
				{
					::gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
					g_BagManager.SearchAndUseBag(aIndex, BAG_EVENT, EVENTBAG_WARRIORRING_2, aIndex);
				}
				else
				{
					PMSG_NOTICE pNotice;

					TNotice::MakeNoticeMsg((TNotice *)&pNotice, 10, Lang.GetText(0, 109));
					TNotice::SetNoticeProperty((TNotice *)&pNotice, 10, _ARGB(255, 128, 149, 196), 1, 0, 20);
					TNotice::SendNoticeToUser(aIndex, (TNotice *)&pNotice);
					pResult.Result = false;
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}
			}
			else if (type == ITEMGET(14, 28)) // Lost Map
			{
				if (g_KalimaGate.CreateKalimaGate(aIndex, level, lpMsg->px, lpMsg->py))
				{
					gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
				}
				else
				{
					pResult.Result = false;
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}
			}
			else if (type == ITEMGET(14, 64) && drop_type != 1)
			{
				pResult.Result = false;
				IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
				return FALSE;
			}
			else if (type == ITEMGET(14, 20) && (((level - 1 < 0) ? FALSE : (level - 1 > 4) ? FALSE : TRUE) != FALSE)) // Remedy Of Love
			{
				gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
			}
			else if (type == ITEMGET(13, 7)) // Contract (Summon)
			{
				int iMonsterID = 0;

				if (level == 0)
					iMonsterID = 286;
				else if (level == 1)
					iMonsterID = 287;

				if (g_CsNPC_Mercenary.CreateMercenary(aIndex, iMonsterID, lpMsg->px, lpMsg->py))
				{
					::gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
				}
				else
				{
					pResult.Result = false;
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}
			}
			else if (type == ITEMGET(13, 11)) // Guardian / LifeStone
			{
				BOOL bResult = FALSE;

				if (level == 0)
					bResult = g_CsNPC_Guardian.CreateGuardian(aIndex);

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && level == 1)
					bResult = g_CsNPC_LifeStone.CreateLifeStone(aIndex);

				if (bResult == TRUE)
					::gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
				else
				{
					pResult.Result = false;
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}
			}

			else if ((type >= ITEMGET(14, 236) && type <= ITEMGET(14, 241)) && level == 0)
			{
				gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);

				switch (type)
				{
				case ITEMGET(14, 236):
					g_BagManager.SearchAndUseBag(aIndex, BAG_EVENT, EVENTBAG_ITR_1, aIndex);
					break;
				case ITEMGET(14, 237):
					g_BagManager.SearchAndUseBag(aIndex, BAG_EVENT, EVENTBAG_ITR_2, aIndex);
					break;
				case ITEMGET(14, 238):
					g_BagManager.SearchAndUseBag(aIndex, BAG_EVENT, EVENTBAG_ITR_3, aIndex);
					break;
				case ITEMGET(14, 239):
					g_BagManager.SearchAndUseBag(aIndex, BAG_EVENT, EVENTBAG_ITR_4, aIndex);
					break;
				case ITEMGET(14, 240):
					g_BagManager.SearchAndUseBag(aIndex, BAG_EVENT, EVENTBAG_ITR_5, aIndex);
					break;
				case ITEMGET(14, 241):
					g_BagManager.SearchAndUseBag(aIndex, BAG_EVENT, EVENTBAG_ITR_6, aIndex);
					break;
				}

			}

			else
			{

				int OwnerIndex = aIndex;

				if ((type >= ITEMGET(14, 0) && type <= ITEMGET(14, 10)) || (type == ITEMGET(4, 7)) || type == ITEMGET(4, 15) || type == ITEMGET(14, 64))
					OwnerIndex = -1;

				if (MapC[map].ItemDrop(type, level, dur, lpMsg->px, lpMsg->py, Option1, Option2, Option3,
					NOption, SOption, serial2, OwnerIndex, PetLevel, PetExp, btItemEffectEx, SocketOption, SocketIndex, 0) == TRUE)
				{
					gObjInventoryDeleteItem(aIndex, lpMsg->Ipos);
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

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

	if (CopyItem != FALSE)
	{
		::GCItemObjectListSend(aIndex);
		g_PeriodItemEx.OnRequestPeriodItemList(lpObj);
	}

	if (pResult.Result == 1)
	{
		if (lpMsg->Ipos < 12)
		{
			if (lpMsg->Ipos == 10 || lpMsg->Ipos == 11)
			{
				gObjUseSkill.SkillChangeUse(aIndex);
			}

			::gObjMakePreviewCharSet(aIndex);

			PMSG_USEREQUIPMENTCHANGED pChange;

			PHeadSetB((BYTE*)&pChange, 0x25, sizeof(pChange));
			pChange.NumberH = SET_NUMBERH(aIndex);
			pChange.NumberL = SET_NUMBERL(aIndex);
			ItemByteConvert(pChange.ItemInfo, lpObj.pInventory[lpMsg->Ipos]);
			pChange.ItemInfo[I_OPTION] = lpMsg->Ipos << 4;
			pChange.ItemInfo[I_OPTION] |= LevelSmallConvert(aIndex, lpMsg->Ipos) & 0x0F;
			pChange.Element = lpObj.m_iPentagramMainAttribute;
			if (pChange.ItemInfo[5] == 0xF0)
				pChange.ItemInfo[5] = 0xF1;
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pChange, pChange.h.size);
			MsgSendV2(lpObj, (BYTE*)&pChange, pChange.h.size);
		}
	}

	return pResult.Result;
}

void GameProtocol::GCItemObjectMoveResultSend(CGameObject &Obj, BYTE result, BYTE pos, BYTE* const ItemInfo)
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

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::CGInventoryItemMove(PMSG_INVENTORYITEMMOVE * lpMsg, CGameObject &Obj)
{
	BYTE result;
	BYTE ItemInfo[MAX_ITEM_INFO];

	if (!PacketCheckTime(aIndex))
	{
		this->GCItemObjectMoveResultSend(aIndex, -1, 0, ItemInfo);
		return;
	}

	if (!::gObjIsConnectedGP(aIndex))
	{
		this->GCItemObjectMoveResultSend(aIndex, -1, 0, ItemInfo);
		return;
	}

	if (lpObj.m_PlayerData->m_bSecurityCheck == false)
	{
		MsgOutput(aIndex, Lang.GetText(0, 822));
		this->GCItemObjectMoveResultSend(aIndex, -1, 0, ItemInfo);
		return;
	}

	
	int it_type = 0;

	if (lpObj.m_PlayerData->m_ReqWarehouseOpen != false)
	{
		this->GCItemObjectMoveResultSend(aIndex, -1, 0, (BYTE*)&ItemInfo);

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

	if (gObjCanItemTouch(lpObj, it_type) == FALSE)
	{
		if (lpMsg->sItemInfo[I_TYPE] == ITEMGET(4, 7) || lpMsg->sItemInfo[I_TYPE] == ITEMGET(4, 15))
		{
			if (target >= 12 || source < 12)
			{
				this->GCItemObjectMoveResultSend(aIndex, -1, 0, (BYTE*)&ItemInfo);
				return;
			}
		}
		else
		{
			this->GCItemObjectMoveResultSend(aIndex, -1, 0, (BYTE*)&ItemInfo);
			return;
		}
	}

	if (lpObj.DieRegen != FALSE)
	{
		this->GCItemObjectMoveResultSend(aIndex, -1, 0, (BYTE*)&ItemInfo);
		return;
	}

	if (lpObj.Teleport != FALSE)
	{
		this->GCItemObjectMoveResultSend(aIndex, -1, 0, (BYTE*)&ItemInfo);
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
		if (lpObj.m_IfState.use < 1 || lpObj.m_IfState.type != 7)
		{
			this->GCItemObjectMoveResultSend(aIndex, -1, 0, (BYTE*)&ItemInfo);
			return;
		}

		if (lpObj.ChaosLock == TRUE)
		{
			this->GCItemObjectMoveResultSend(aIndex, -1, 0, (BYTE*)&ItemInfo);

			return;
		}
	}

	if (lpMsg->sFlag == 5 || lpMsg->tFlag == 5)
	{
		if (lpObj.m_IfState.use < 1 || lpObj.m_IfState.type != 13)
		{
			this->GCItemObjectMoveResultSend(aIndex, -1, 0, (BYTE*)&ItemInfo);
			return;
		}

		if (lpObj.ChaosLock == TRUE)
		{
			this->GCItemObjectMoveResultSend(aIndex, -1, 0, (BYTE*)&ItemInfo);

			return;
		}
	}

	if (lpMsg->sFlag == 6 || lpMsg->sFlag == 7 || lpMsg->sFlag == 8 ||
		lpMsg->tFlag == 6 || lpMsg->tFlag == 7 || lpMsg->tFlag == 8)
	{
		if (lpObj.m_IfState.use < 1 || lpObj.m_IfState.type != 7)
		{
			this->GCItemObjectMoveResultSend(aIndex, -1, 0, (BYTE*)&ItemInfo);
			return;
		}

		if (lpObj.ChaosLock == TRUE)
		{
			this->GCItemObjectMoveResultSend(aIndex, -1, 0, (BYTE*)&ItemInfo);

			return;
		}
	}
	if (lpMsg->sFlag == 9 || lpMsg->tFlag == 9)
	{
		if (lpObj.m_IfState.use < 1 || lpObj.m_IfState.type != 7)
		{
			this->GCItemObjectMoveResultSend(aIndex, -1, 0, (BYTE*)&ItemInfo);
			return;
		}

		if (lpObj.ChaosLock == TRUE)
		{
			this->GCItemObjectMoveResultSend(aIndex, -1, 0, (BYTE*)&ItemInfo);

			return;
		}
	}
	if (lpMsg->sFlag == 11 || lpMsg->sFlag == 12 || lpMsg->sFlag == 13 || lpMsg->sFlag == 14 || lpMsg->sFlag == 15 || lpMsg->sFlag == 16 || lpMsg->sFlag == 17 ||
		lpMsg->tFlag == 11 || lpMsg->tFlag == 12 || lpMsg->tFlag == 13 || lpMsg->tFlag == 14 || lpMsg->tFlag == 15 || lpMsg->tFlag == 16 || lpMsg->tFlag == 17 || lpMsg->tFlag == 19)
	{
		if (lpObj.m_IfState.use < 1 || lpObj.m_IfState.type != 7)
		{
			this->GCItemObjectMoveResultSend(aIndex, -1, 0, (BYTE*)&ItemInfo);
			return;
		}

		if (lpObj.ChaosLock == TRUE)
		{
			this->GCItemObjectMoveResultSend(aIndex, -1, 0, (BYTE*)&ItemInfo);

			return;
		}
	}


	if (lpMsg->sFlag == 2 || lpMsg->tFlag == 2)
	{
		if (lpObj.m_IfState.use < 1 || lpObj.m_IfState.type != 6)
		{
			this->GCItemObjectMoveResultSend(aIndex, -1, 0, (BYTE*)&ItemInfo);
			return;
		}
	}
	if (lpMsg->sFlag == 1 && lpMsg->tFlag == 0)
	{
		if (lpObj.m_IfState.use < 1 || lpObj.m_IfState.type != 1)
		{
			this->GCItemObjectMoveResultSend(aIndex, -1, 0, (BYTE*)&ItemInfo);
			return;
		}
	}

	if (lpMsg->sFlag == 2 && lpMsg->tFlag == 0)
	{
		int money = ::GetWarehouseUsedHowMuch(lpObj.Level, lpObj.WarehousePW);

		if ((lpObj.m_PlayerData->Money - money) < 1 && (lpObj.WarehouseMoney - money) < 1)
		{
			this->GCItemObjectMoveResultSend(aIndex, -1, 0, (BYTE*)&ItemInfo);
			char szTemp[256];
			wsprintf(szTemp, Lang.GetText(0, 154), money);
			this->GCServerMsgStringSend(szTemp, lpObj.m_Index, 1);

			return;
		}

		if (::bCanWarehouseLock == TRUE)
		{
			if (lpObj.WarehouseLock == 1)
			{
				this->GCItemObjectMoveResultSend(aIndex, -1, 0, (BYTE*)&ItemInfo);
				this->GCServerMsgStringSend(Lang.GetText(0, 155), lpObj.m_Index, 1);

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
			this->GCItemObjectMoveResultSend(aIndex, -1, 0, (BYTE*)&ItemInfo);
			return;
		}

		if (lpItemAttr->QuestItem != false && !g_QuestExpManager.IsQuestItemAtt(type, 4))
		{
			this->GCItemObjectMoveResultSend(aIndex, -1, 0, (BYTE*)&ItemInfo);
			return;
		}

		if (type == ITEMGET(14, 64)) // cursed castle water
		{
			this->GCItemObjectMoveResultSend(aIndex, -1, 0, (BYTE*)&ItemInfo);
			return;
		}
	}

	if (lpMsg->sFlag == 21 && lpMsg->tFlag != 21 && lpMsg->tFlag != 1)
	{
		this->GCItemObjectMoveResultSend(aIndex, -1, 0, ItemInfo);
		return;
	}

	if (lpMsg->sFlag == 1)
	{
		if (lpMsg->tFlag != 21)
		{
			if (lpMsg->tFlag != 1)
			{
				if (IsEventItem(lpObj.Trade[source]) == TRUE)
				{
					GCItemObjectMoveResultSend(aIndex, -1, 0, ItemInfo);
					return;
				}
			}
		}
	}
	if (lpMsg->tFlag != 21)
	{
		int type = (ItemInfo[I_TYPE] + ((ItemInfo[I_NOPTION] & 0x80) * 2)) + ((ItemInfo[I_380OPTION] & 0xF0) << 5);

		if (IsEventItem(type) == TRUE)
		{
			this->GCItemObjectMoveResultSend(aIndex, -1, 0, ItemInfo);
			return;
		}
	}

	if (lpMsg->sFlag == 22 && lpMsg->tFlag != 22)
	{
		this->GCItemObjectMoveResultSend(aIndex, -1, 0, ItemInfo);
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

		result = gObjInventoryMoveItem(aIndex, source, target, DurSSend, DurTSend, lpMsg->sFlag, lpMsg->tFlag, (BYTE*)&ItemInfo);

		if (lpObj.m_IfState.use == 1 && lpObj.m_IfState.type == 3) // Shop
		{
			this->GCPriceSend(lpObj.m_Index, 1, 0);
		}

		this->GCItemObjectMoveResultSend(aIndex, result, target, (BYTE*)&ItemInfo);
		if (DurSSend != FALSE)
			this->GCItemObjectDurSend(aIndex, source, lpObj.pInventory[source].m_Durability, FALSE);

		if (DurTSend != FALSE)
			this->GCItemObjectDurSend(aIndex, target, lpObj.pInventory[target].m_Durability, FALSE);
		if (result != 0xFF && lpMsg->sFlag == 2 && lpMsg->tFlag == 0)
		{
			int money = ::GetWarehouseUsedHowMuch(lpObj.Level, lpObj.WarehousePW);

			if ((lpObj.m_PlayerData->Money - money) > 0)
			{
				int iZen = lpObj.m_PlayerData->Money;
				lpObj.m_PlayerData->Money -= money;
				this->GCMoneySend(lpObj.m_Index, lpObj.m_PlayerData->Money);

				//sLog->outBasic("Pay WareHouse Money(In Inventory) : %d - %d = %d",iZen, money, lpObj.m_PlayerData->Money);
			}
			else if ((lpObj.WarehouseMoney - money) > 0)
			{
				int iZen = lpObj.WarehouseMoney;
				lpObj.WarehouseMoney -= money;

				//sLog->outBasic("Pay WareHouse Money(In WareHouse) : %d - %d = %d",iZen, money, lpObj.WarehouseMoney);

				this->GCWarehouseInventoryMoneySend(aIndex, 1, lpObj.m_PlayerData->Money, lpObj.WarehouseMoney);
			}
		}
		g_PeriodItemEx.SendPeriodItemInfoOnce(lpObj, &lpObj.pInventory[target]);

		return;
	}

	if (lpMsg->sFlag == 22 && lpMsg->tFlag == 22)
	{
		result = gObjMuunInvenMove(lpObj, 0, 0, source, target, (BYTE*)ItemInfo);
		GCItemObjectMoveResultSend(aIndex, result, target, (BYTE*)ItemInfo);
	}

	if (lpMsg->sFlag == 1 && lpMsg->tFlag == 21)
	{
		result = gObjTradeEventInventoryMove(lpObj, source, target);

		if (result == 255)
		{
			this->GCItemObjectMoveResultSend(aIndex, -1, target, ItemInfo);
			this->CGTradeCancelButtonRecv(aIndex);

		}

		else
		{
			this->GCItemObjectMoveResultSend(aIndex, result, target, ItemInfo);
		}

		if (result == 255)
		{
			lpObj.m_IfState.state = 0;
			this->GCEventInventoryItemListSend(aIndex);

		}

		else
		{
			lpObj.TradeOk = 0;
			getGameObject(lpObj.TargetNumber)->TradeOk = 0;
			GCTradeOkButtonSend(lpObj.TargetNumber, 2);
			GCTradeOkButtonSend(aIndex, 0);

		}

		return;
	}

	if (lpMsg->sFlag == 21 && lpMsg->tFlag == 1)
	{
		result = gObjEventInventoryTradeMove(lpObj, source, target);
		GCItemObjectMoveResultSend(aIndex, result, target, ItemInfo);

		return;
	}

	if (lpMsg->sFlag == 21 && lpMsg->tFlag == 21)
	{
		BOOL DurSSend;
		BOOL DurTSend;

		result = gObjEventInvenMove(lpObj, &DurSSend, &DurTSend, source, target);
		this->GCItemObjectMoveResultSend(aIndex, result, target, ItemInfo);

		if (result == 255)
		{
			return;
		}

		if (DurSSend)
		{
			this->GCEventItemDurSend(aIndex, source, (int)lpObj.pEventInventory[source].m_Durability);
		}

		if (DurTSend)
		{
			this->GCEventItemDurSend(aIndex, target, (int)lpObj.pEventInventory[target].m_Durability);
		}

		if (lpObj.pEventInventory[target].m_Durability == 0.0)
		{
			GCEventInventoryItemDeleteSend(aIndex, target, TRUE);
		}

		return;
	}

	if (lpMsg->sFlag == 1 && lpMsg->tFlag == 1)
	{
		result = gObjTradeTradeMove(lpObj, source, target);
		this->GCItemObjectMoveResultSend(aIndex, result, target, (BYTE*)&ItemInfo);
		return;
	}

	if (lpMsg->sFlag == 1 && lpMsg->tFlag == 0)
	{
		result = gObjTradeInventoryMove(lpObj, source, target);

		if (result == 0xFF)
		{
			this->GCItemObjectMoveResultSend(aIndex, 0xFF, target, (BYTE*)&ItemInfo);
			this->CGTradeCancelButtonRecv(aIndex);
		}
		else
		{
			this->GCItemObjectMoveResultSend(aIndex, result, target, (BYTE*)&ItemInfo);
		}

		if (result == 0xFF)
		{
			lpObj.m_IfState.state = 0;
			GCItemObjectListSend(aIndex);
			g_PeriodItemEx.OnRequestPeriodItemList(lpObj);
		}

		if (lpObj.TargetNumber >= 0)
		{
			lpObj.TradeOk = false;
			getGameObject(lpObj.TargetNumber)->TradeOk = false;
			this->GCTradeOkButtonSend(lpObj.TargetNumber, 2);
			this->GCTradeOkButtonSend(aIndex, 0);
		}

		return;
	}

	if (lpMsg->sFlag == 0 && lpMsg->tFlag == 1)
	{
		result = gObjInventoryTradeMove(lpObj, source, target);
		this->GCItemObjectMoveResultSend(aIndex, result, target, (BYTE*)&ItemInfo);
	}
}



void GameProtocol::GCEquipmentChange(CGameObject &Obj, int pos)
{
	PMSG_USEREQUIPMENTCHANGED pChange;
	

	PHeadSetB((BYTE*)&pChange, 0x25, sizeof(PMSG_USEREQUIPMENTCHANGED));
	pChange.NumberH = SET_NUMBERH(aIndex);
	pChange.NumberL = SET_NUMBERL(aIndex);
	ItemByteConvert(pChange.ItemInfo, lpObj.pInventory[pos]);
	pChange.ItemInfo[I_OPTION] = pos << 4;
	pChange.ItemInfo[I_OPTION] |= LevelSmallConvert(aIndex, pos) & MAX_ITEM_LEVEL;
	pChange.Element = lpObj.m_iPentagramMainAttribute;
	if (pChange.ItemInfo[5] == 0xF0)
		pChange.ItemInfo[5] = 0xF1;
	MsgSendV2(lpObj, (BYTE*)&pChange, pChange.h.size);
}

void GameProtocol::CGTalkRequestRecv(PMSG_TALKREQUEST * lpMsg, CGameObject &Obj)
{
	
	int DealerNumber;
	PMSG_TALKRESULT pResult;
	PMSG_SHOPITEMCOUNT pShopItemCount;

	if (!PacketCheckTime(aIndex))
		return;

	if (!gObjIsConnectedGP(aIndex))
	{
		return;
	}

	if (lpObj.CloseType != -1)
		return;

	if (lpObj.m_PlayerData->m_bSecurityCheck == false)
	{
		MsgOutput(aIndex, Lang.GetText(0, 822));
		return;
	}

	BYTE SendByte[4096];
	int lOfs = 0;

	if (lpObj.m_IfState.use > 0)
		return;

	DealerNumber = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	if (DealerNumber < 0 || DealerNumber > g_ConfigRead.server.GetObjectMax() - 1)
		return;

	if (lpObj.MapNumber != getGameObject(DealerNumber)->MapNumber)
		return;

	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		if (getGameObject(DealerNumber)->Class == 367)
		{
			if ((lpObj.X < (getGameObject(DealerNumber]->X - 5)) || (lpObj.X > (getGameObject(DealerNumber]->X + 5)) || (lpObj.Y < (getGameObject(DealerNumber)->Y - 10)) || (lpObj.Y > (getGameObject(DealerNumber)->Y + 5)))
			{
				return;
			}
		}
		else if ((lpObj.X < (getGameObject(DealerNumber]->X - 5)) || (lpObj.X > (getGameObject(DealerNumber]->X + 5)) || (lpObj.Y < (getGameObject(DealerNumber)->Y - 5)) || (lpObj.Y > (getGameObject(DealerNumber)->Y + 5)))
		{
			return;
		}

	}
	else
	{
		if ((lpObj.X < (getGameObject(DealerNumber]->X - 5)) || (lpObj.X > (getGameObject(DealerNumber]->X + 5)) || (lpObj.Y < (getGameObject(DealerNumber)->Y - 5)) || (lpObj.Y > (getGameObject(DealerNumber)->Y + 5)))
		{
			return;
		}
	}

	if (getGameObject(DealerNumber)->Type == OBJ_NPC)
	{
		if (NpcTalk(&getGameObject(DealerNumber), lpObj) == TRUE)
		{
			this->GCAnsCsMapSvrTaxInfo(lpObj.m_Index, 1, g_CastleSiegeSync.GetTaxRateChaos(lpObj.m_Index));
			return;
		}
	}
	// getGameObject(DealerNumber)->Type
	if (::gObjFixInventoryPointer(aIndex) == false)
	{

	}

	if (lpObj.pTransaction == 1)
	{
		return;
	}

	if (getGameObject(DealerNumber)->Class == 234)	// Npc Server Division
	{
		pResult.h.c = 0xC3;
		pResult.h.headcode = 0x30;
		pResult.h.size = sizeof(pResult);
		pResult.result = 5;

		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (getGameObject(DealerNumber]->Type == OBJ_NPC && getGameObject(DealerNumber)->m_btNpcType == NPC_TYPES::NPC_NONE)
		return;

	if (getGameObject(DealerNumber)->m_btNpcType == NPC_TYPES::NPC_SHOP)
	{
		SHOP_DATA * lpShopData = g_ShopMng.GetShop(lpObj, &getGameObject(DealerNumber));

		if (!lpShopData)
		{
			return;
		}
	}

	if (gMerryXMasNpcEvent == TRUE)
	{
		if ((rand() % 6) == 0)
		{
			ChatTargetSend(&getGameObject(DealerNumber), Lang.GetText(0, 55), aIndex);
		}
	}

	if (::gHappyNewYearNpcEvent == TRUE)
	{
		if ((rand() % 6) == 0)
		{
			ChatTargetSend(&getGameObject(DealerNumber), Lang.GetText(0, 56), aIndex);
		}
	}

	SHOP_DATA * lpShopData = g_ShopMng.GetShop(lpObj, &getGameObject(DealerNumber));

	lpObj.TargetNpcNumber = DealerNumber;
	lpObj.m_IfState.use = 1;
	lpObj.m_IfState.type = 3;
	lpObj.m_ShopTime = 0;
	pResult.h.c = 0xC3;
	pResult.h.headcode = 0x30;
	pResult.h.size = sizeof(pResult);

	if (getGameObject(DealerNumber)->m_btNpcType == NPC_TYPES::NPC_SHOP && lpShopData->btIsMossMerchant == TRUE)
	{
		pResult.result = 0x22;
	}

	else
	{
		pResult.result = 0x00;
	}

	if (getGameObject(DealerNumber)->m_btNpcType == NPC_TYPES::NPC_SHOP && lpShopData->btIsMossMerchant == TRUE && !g_GamblingItemBag.IsEnabled())	return;

	if (getGameObject(DealerNumber)->m_btNpcType == NPC_TYPES::NPC_WAREHOUSE)
	{
		if (lpObj.m_PlayerData->m_JoinUnityBattle == true || g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
		{
			this->GCServerMsgStringSend(Lang.GetText(0, 619), aIndex, 1);
			return;
		}

		if (lpObj.m_PlayerData->m_ReqWarehouseOpen != false)
		{
			return;
		}

		if (lpObj.m_PlayerData->m_bSecurityCheck == false)
		{
			return;
		}

		if (lpObj.m_btOpenWarehouse == TRUE)
		{
			return;
		}

		lpObj.m_PlayerData->m_ReqWarehouseOpen = true;
		lpObj.m_IfState.type = 6;
		lpObj.m_IfState.state = 0;
	}

	else if (getGameObject(DealerNumber)->m_btNpcType == NPC_TYPES::NPC_CHAOSMIX)
	{
		if (bCanChaosBox == TRUE)
		{
			if (lpObj.m_bPShopOpen == true)
			{
				GCServerMsgStringSend(Lang.GetText(0, 112), lpObj.m_Index, 1);

				return;
			}

			lpObj.m_IfState.type = 7;
			lpObj.m_IfState.state = 0;
			pResult.result = 3;
			if (getGameObject(DealerNumber)->Class == 450)
				pResult.result = 22;
			else if (getGameObject(DealerNumber)->Class == 478)
				pResult.result = 32;
			lpObj.bIsChaosMixCompleted = false;
			pResult.level1 = gDQChaosSuccessRateLevel1;
			pResult.level2 = gDQChaosSuccessRateLevel2;
			pResult.level3 = gDQChaosSuccessRateLevel3;
			pResult.level4 = gDQChaosSuccessRateLevel4;
			pResult.level5 = gDQChaosSuccessRateLevel5;
			pResult.level6 = gDQChaosSuccessRateLevel6;
			pResult.level7 = gDQChaosSuccessRateLevel7;

			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
			gObjInventoryTrans(lpObj.m_Index);

			gObjItemTextSave(lpObj);
			gObjWarehouseTextSave(lpObj);
			GCAnsCsMapSvrTaxInfo(lpObj.m_Index, 1, ::g_CastleSiegeSync.GetTaxRateChaos(lpObj.m_Index));
		}
	}

	else if (lpShopData->btIsMossMerchant == TRUE)
	{
		pResult.result = 34;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
	}

	else
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
	}

	if (getGameObject(DealerNumber)->m_btNpcType == NPC_TYPES::NPC_WAREHOUSE)
	{
		if (lpObj.m_bMapSvrMoveReq == true)
		{
			lpObj.m_PlayerData->m_ReqWarehouseOpen = false;
			return;
		}

		if (lpObj.m_State == 32)
		{
			lpObj.m_PlayerData->m_ReqWarehouseOpen = false;
			return;
		}

		if (lpObj.m_bMapSvrMoveQuit == true)
		{
			lpObj.m_PlayerData->m_ReqWarehouseOpen = false;
			return;
		}

		if (lpObj.m_IfState.use && lpObj.m_IfState.type == 19)
		{
			return;
		}

		if (lpObj.WarehouseSaveLock == TRUE)
		{
			lpObj.m_PlayerData->m_ReqWarehouseOpen = false;
			return;
		}

		lpObj.WarehouseCount = 0;
		GDGetWarehouseList(aIndex, lpObj.AccountID);
		return;
	}

	if (getGameObject(DealerNumber)->m_btNpcType == NPC_TYPES::NPC_CHAOSMIX)
		return;

	else if (getGameObject(DealerNumber)->m_btNpcType == NPC_TYPES::NPC_SHOP)
	{
		lOfs = 0;

		lOfs += sizeof(pShopItemCount);

		int size = lOfs + lpShopData->m_ShopData.SendItemDataLen;
		PHeadSetW((BYTE*)&pShopItemCount, 0x31, size);
		pShopItemCount.Type = 0;
		pShopItemCount.count = lpShopData->m_ShopData.ItemCount;
		memcpy(SendByte, &pShopItemCount, sizeof(pShopItemCount));
		memcpy(&SendByte[lOfs], lpShopData->m_ShopData.SendItemData, lpShopData->m_ShopData.SendItemDataLen);

		IOCP.DataSend(lpObj.m_Index, SendByte, size);
		GCAnsCsMapSvrTaxInfo(lpObj.m_Index, 2, ::g_CastleSiegeSync.GetTaxRateStore(lpObj.m_Index));
	}
}




void GameProtocol::GCUserWarehouseSend(CGameObject &Obj)
{
	if (lpObj.m_IfState.type != 6)
		return;

	if (lpObj.m_IfState.type == 6 && lpObj.m_IfState.state == 1)
		return;

	if (lpObj.m_btOpenWarehouse == TRUE)
	{
		return;
	}

	PMSG_SHOPITEMCOUNT pMsg;
	BYTE SendByte[4096];
	int lOfs = sizeof(pMsg);

	PHeadSetW((BYTE*)&pMsg, 0x31, 0);
	pMsg.count = 0;
	pMsg.Type = 0;

	for (int n = 0; n < WAREHOUSE_SIZE; n++)
	{
		if (lpObj.pWarehouse[n].IsItem() == TRUE)
		{
			SendByte[lOfs] = n;
			lOfs++;
			ItemByteConvert(&SendByte[lOfs], lpObj.pWarehouse[n]);
			lOfs += MAX_ITEM_INFO;
			pMsg.count++;
		}
	}

	pMsg.h.sizeH = SET_NUMBERH(lOfs);
	pMsg.h.sizeL = SET_NUMBERL(lOfs);
	memcpy(SendByte, &pMsg, sizeof(pMsg));

	IOCP.DataSend(lpObj.m_PlayerData->IDNumber, SendByte, lOfs);
	this->GCWarehouseInventoryMoneySend(lpObj.m_Index, 1, lpObj.m_PlayerData->Money, lpObj.WarehouseMoney);

	if (lpObj.m_IfState.type == 6)
	{
		lpObj.m_IfState.state = 1;
	}

	lpObj.m_btOpenWarehouse = TRUE;
}

void GameProtocol::CGBuyRequestRecv(PMSG_BUYREQUEST * lpMsg, CGameObject &Obj)
{
	
	PMSG_BUYRESULT pResult;

	PHeadSetB((BYTE*)&pResult, 0x32, sizeof(pResult));
	pResult.Result = -1;

	if (!PacketCheckTime(aIndex))
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	if (lpObj.CloseType != -1)
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (lpObj.TargetNpcNumber == -1)
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	CGameObject lpShopObj = &getGameObject(lpObj.TargetNpcNumber);

	SHOP_DATA * lpShopData = g_ShopMng.GetShop(lpObj, lpShopObj);

	if (!lpShopData)
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (!::gObjFixInventoryPointer(aIndex))
	{

	}

	if (lpObj.m_ShopTime == 0)
		lpObj.m_ShopTime = 1;

	if (lpShopData->btIsMossMerchant == TRUE)
	{
		if (lpObj.m_ShopTime > g_GamblingItemBag.GetGamblingShopTime())
		{
			if (lpObj.m_IfState.use && lpObj.m_IfState.type == 3)
			{
				lpObj.TargetNpcNumber = -1;
				pResult.Result = 0xFE;
				lpObj.m_IfState.use = 0;
				lpObj.m_IfState.type = 0;
			}

			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
			GCServerMsgStringSend(Lang.GetText(0, 520), aIndex, 1);
			return;
		}
	}

	if (lpObj.m_ShopTime > 60)
	{
		if (lpObj.m_IfState.use != 0 && lpObj.m_IfState.type == 3)
		{
			lpObj.TargetNpcNumber = -1;
			lpObj.m_IfState.use = 0;
			lpObj.m_IfState.type = 0;
		}

		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (lpObj.m_IfState.use > 0)
	{
		if (lpObj.m_IfState.type != 3)
		{
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}
	}

	if (lpMsg->Pos > MAX_ITEM_IN_SHOP - 1)
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (lpObj.m_PlayerData->m_bIsCancelItemSale == true)
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (lpShopData->m_ShopData.m_item[lpMsg->Pos].IsItem() == TRUE)
	{
		int iStoreTaxMoney = lpShopData->m_ShopData.m_item[lpMsg->Pos].m_BuyMoney + (int)((__int64)lpShopData->m_ShopData.m_item[lpMsg->Pos].m_BuyMoney * (__int64)g_CastleSiegeSync.GetTaxRateStore(lpObj.m_Index) / (__int64)100);

		if (iStoreTaxMoney < 0)
			iStoreTaxMoney = 0;

		int iStoreTaxMoney2 = (int)((__int64)lpShopData->m_ShopData.m_item[lpMsg->Pos].m_BuyMoney * (__int64)g_CastleSiegeSync.GetTaxRateStore(lpObj.m_Index) / (__int64)100);

		if (iStoreTaxMoney2 < 0)
			iStoreTaxMoney2 = 0;

		if (g_ArcaBattle.IsArcaBattleServer() == 1 && iStoreTaxMoney > 0)
		{
			iStoreTaxMoney *= 3;
		}

		if (lpShopData->btIsMossMerchant == TRUE)
		{
			int iTaxGambling = g_GamblingItemBag.GetGamblingValue() * g_CastleSiegeSync.GetTaxRateStore(lpObj.m_Index) / 100;
			iStoreTaxMoney = g_GamblingItemBag.GetGamblingValue() + iTaxGambling;
		}

		if (lpObj.m_PlayerData->Money < iStoreTaxMoney)
			pResult.Result = -1;
		else
		{
			BOOL bNoItem = TRUE;

			if ((lpShopData->m_ShopData.m_item[lpMsg->Pos].m_Type >= ITEMGET(14, 0) && lpShopData->m_ShopData.m_item[lpMsg->Pos].m_Type <= ITEMGET(14, 8)) ||
				(lpShopData->m_ShopData.m_item[lpMsg->Pos].m_Type >= ITEMGET(14, 35) && lpShopData->m_ShopData.m_item[lpMsg->Pos].m_Type <= ITEMGET(14, 40)))
			{
				int dur = lpShopData->m_ShopData.m_item[lpMsg->Pos].m_Durability;

				if (dur == 0)
					dur = 1;

				if (::gObjSearchItem(&getGameObject(aIndex], lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type, dur, lpShopData->m_ShopData.m_item[lpMsg->Pos)->m_Level) == TRUE)
				{
					bNoItem = FALSE;
					lpObj.m_PlayerData->Money -= iStoreTaxMoney;
					::g_CastleSiegeSync.AddTributeMoney(iStoreTaxMoney2);

					if (lpObj.m_PlayerData->Money < 0)
						lpObj.m_PlayerData->Money = 0;

					this->GCMoneySend(aIndex, lpObj.m_PlayerData->Money);
				}
			}

			if (lpShopData->btIsMossMerchant == TRUE)
			{
				if (!g_GamblingItemBag.IsEnabled())
				{
					pResult.Result = 0xFF;
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
					return;
				}

				if (bNoItem)
				{
					int iCount = 0;

					while (iCount <= lpShopData->m_ShopData.ItemCount)
					{
						if (lpMsg->Pos == lpShopData->m_ShopData.SendItemData[iCount + (12 * iCount)])
						{
							pResult.Result = g_GamblingItemBag.GetGamblingSystemItem(aIndex, 229, 0, 0, iCount);
							break;
						}
						iCount++;
					}

					pResult.Result = 0xFF;
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
					return;
				}
			}

			if (bNoItem)
			{
				if (lpShopData->m_ShopData.m_item[lpMsg->Pos].m_Type == ITEMGET(13, 239) ||// Muun Energy Converter
					lpShopData->m_ShopData.m_item[lpMsg->Pos].m_Type >= ITEMGET(13, 212) && // Pets Muun
					lpShopData->m_ShopData.m_item[lpMsg->Pos].m_Type <= ITEMGET(13, 219) ||// Pets Muun
					lpShopData->m_ShopData.m_item[lpMsg->Pos].m_Type >= ITEMGET(13, 223) && // Pets Muun
					lpShopData->m_ShopData.m_item[lpMsg->Pos].m_Type <= ITEMGET(13, 275) ||// Pets Muun
					lpShopData->m_ShopData.m_item[lpMsg->Pos].m_Type >= ITEMGET(13, 278) && // Pets Muun
					lpShopData->m_ShopData.m_item[lpMsg->Pos].m_Type <= ITEMGET(13, 349) ||// Pets Muun
					lpShopData->m_ShopData.m_item[lpMsg->Pos].m_Type >= ITEMGET(13, 409) && // Pets Muun
					lpShopData->m_ShopData.m_item[lpMsg->Pos].m_Type <= ITEMGET(13, 464) ||// Pets Muun
					lpShopData->m_ShopData.m_item[lpMsg->Pos].m_Type >= ITEMGET(13, 480) && // Pets Muun
					lpShopData->m_ShopData.m_item[lpMsg->Pos].m_Type <= ITEMGET(13, 503)) // Pets Muun
				{
					BYTE btRet = gObjMuunInventoryInsertItem(aIndex, lpShopData->m_ShopData.m_item[lpMsg->Pos]);

					if (btRet == 0xFF)
					{
						pResult.Result = 0xFF;
						IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
						return;
					}

					ItemByteConvert(pResult.ItemInfo, lpShopData->m_ShopData.m_item[lpMsg->Pos]);
					int buymoney = lpShopData->m_ShopData.m_item[lpMsg->Pos].m_BuyMoney;
					lpObj.m_PlayerData->Money -= iStoreTaxMoney;
					g_CastleSiegeSync.AddTributeMoney(iStoreTaxMoney2);
					if (lpObj.m_PlayerData->Money < 0) lpObj.m_PlayerData->Money = 0;
					GCMoneySend(aIndex, lpObj.m_PlayerData->Money);
					int iTaxRate = g_CastleSiegeSync.GetTaxRateStore(lpObj.m_Index);

					this->GCMuunInventoryItemOneSend(aIndex, btRet);

					pResult.Result = 0xFF;
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
					return;
				}

				if (lpShopData->m_ShopData.m_item[lpMsg->Pos].m_serial == 0)
				{
					LPITEM_ATTRIBUTE p = GetItemAttr(lpShopData->m_ShopData.m_item[lpMsg->Pos].m_Type);

					if (!p)
					{
						pResult.Result = 0xFF;
						IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
						return;
					}

					pResult.Result = gObjShopBuyInventoryInsertItem(aIndex, lpShopData->m_ShopData.m_item[lpMsg->Pos]);

					if (pResult.Result != 0xFF)
					{

						ItemByteConvert(pResult.ItemInfo, lpShopData->m_ShopData.m_item[lpMsg->Pos]);
						int buymoney = lpShopData->m_ShopData.m_item[lpMsg->Pos].m_BuyMoney;
						lpObj.m_PlayerData->Money -= iStoreTaxMoney;
						g_CastleSiegeSync.AddTributeMoney(iStoreTaxMoney2);
						if (lpObj.m_PlayerData->Money < 0) lpObj.m_PlayerData->Money = 0;
						GCMoneySend(aIndex, lpObj.m_PlayerData->Money);
						int iTaxRate = g_CastleSiegeSync.GetTaxRateStore(lpObj.m_Index);
					}
				}

				else
				{
					LPITEM_ATTRIBUTE p = GetItemAttr(lpShopData->m_ShopData.m_item[lpMsg->Pos].m_Type);

					if (!p)
					{
						pResult.Result = 0xFF;
						IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
						return;
					}

					if (CheckInventoryEmptySpace(&getGameObject(aIndex), p->Height, p->Width) == FALSE)
					{
						pResult.Result = 0xFF;
						IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
						return;
					}

					int buymoney = lpShopData->m_ShopData.m_item[lpMsg->Pos].m_BuyMoney;
					lpObj.m_PlayerData->Money -= iStoreTaxMoney;
					g_CastleSiegeSync.AddTributeMoney(iStoreTaxMoney2);
					if (lpObj.m_PlayerData->Money < 0) lpObj.m_PlayerData->Money = 0;
					GCMoneySend(aIndex, lpObj.m_PlayerData->Money);
					int iTaxRate = g_CastleSiegeSync.GetTaxRateStore(lpObj.m_Index);

					ItemSerialCreateSend(aIndex, 222, 0, 0, lpShopData->m_ShopData.m_item[lpMsg->Pos].m_Type, lpShopData->m_ShopData.m_item[lpMsg->Pos].m_Level,
						lpShopData->m_ShopData.m_item[lpMsg->Pos].m_Durability, lpShopData->m_ShopData.m_item[lpMsg->Pos].m_Option1,
						lpShopData->m_ShopData.m_item[lpMsg->Pos].m_Option2, lpShopData->m_ShopData.m_item[lpMsg->Pos].m_Option3,
						aIndex, lpShopData->m_ShopData.m_item[lpMsg->Pos].m_NewOption, lpShopData->m_ShopData.m_item[lpMsg->Pos].m_SetOption,
						0, lpShopData->m_ShopData.m_item[lpMsg->Pos].m_SocketOption, lpShopData->m_ShopData.m_item[lpMsg->Pos].m_BonusSocketOption);

				}
			}
		}
	}

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
}



struct PMSG_SELLRESULT
{
	PBMSG_HEAD2 h;	// C1:33
	BYTE Result;	// 3
	DWORD Money;	// 4
};



void GameProtocol::CGSellRequestRecv(PMSG_SELLREQUEST * lpMsg, CGameObject &Obj)
{
	PMSG_SELLRESULT pResult;
	

	PHeadSetB((BYTE*)&pResult, 0x33, sizeof(pResult));
	pResult.Result = false;
	pResult.Money = 0;

	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	if (lpObj.CloseType != -1)
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (lpObj.m_IfState.use > 0 && lpObj.m_IfState.type != 3)
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (!PacketCheckTime(aIndex))
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (!::gObjFixInventoryPointer(aIndex))
	{

	}

	if (lpObj.pTransaction == 1)
	{
		return;
	}

	if (lpObj.TargetNpcNumber == -1)
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	CGameObject lpShopObj = &getGameObject(lpObj.TargetNpcNumber);

	SHOP_DATA * lpShopData = g_ShopMng.GetShop(lpObj, lpShopObj);

	if (!lpShopData)
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (!gObjCanItemTouch(lpObj, 3))
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (lpMsg->Pos > MAIN_INVENTORY_SIZE - 1 && lpMsg->Pos != 236)
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (gObjCheckSerial0ItemList(&lpObj.pInventory[lpMsg->Pos]) != FALSE)
	{
		MsgOutput(aIndex, Lang.GetText(0, 259));
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if (g_ConfigRead.data.common.ShopExCItemObjectLimit == FALSE)
	{
		if (lpObj.pInventory[lpMsg->Pos].m_NewOption == 63)
		{
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}
	}

	if (g_ConfigRead.data.common.EnableSellAllItems == false)
	{
		if (lpObj.pInventory[lpMsg->Pos].m_Type == ITEMGET(14, 11)) // Box of Luck
		{
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}

		if (lpObj.pInventory[lpMsg->Pos].m_QuestItem != false && !g_QuestExpManager.IsQuestItemAtt(lpObj.pInventory[lpMsg->Pos].m_Type, 8))
		{
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}

		if (lpObj.pInventory[lpMsg->Pos].m_Type == ITEMGET(13, 20) && lpObj.pInventory[lpMsg->Pos].m_Level == 1) // Wizards Ring +1
		{
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}

		if (lpObj.pInventory[lpMsg->Pos].m_Type == ITEMGET(13, 20) && lpObj.pInventory[lpMsg->Pos].m_Level == 2) // Wizards Ring +2
		{
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}

		if (lpObj.pInventory[lpMsg->Pos].m_Type == ITEMGET(12, 26) &&
			(lpObj.pInventory[lpMsg->Pos].m_Level == 1 ||
				lpObj.pInventory[lpMsg->Pos].m_Level == 2 ||
				lpObj.pInventory[lpMsg->Pos].m_Level == 3 ||
				lpObj.pInventory[lpMsg->Pos].m_Level == 4 ||
				lpObj.pInventory[lpMsg->Pos].m_Level == 5))
		{
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}

		if (g_kJewelOfHarmonySystem.IsStrengthenByJewelOfHarmony(&lpObj.pInventory[lpMsg->Pos]) == TRUE && g_ConfigRead.data.common.joinmuRemoveItemSellDropRestriction == false)
		{
			GCServerMsgStringSend(Lang.GetText(0, 267), lpObj.m_Index, 1);
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}

		if (lpObj.pInventory[lpMsg->Pos].IsPeriodItem() == TRUE && lpObj.pInventory[lpMsg->Pos].IsPeriodItemExpire() == FALSE)
		{
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}

		if (lpObj.pInventory[lpMsg->Pos].IsPeriodItem() == TRUE)
		{
			if (lpObj.pInventory[lpMsg->Pos].m_Type == ITEMGET(12, 266) ||
				lpObj.pInventory[lpMsg->Pos].m_Type == ITEMGET(12, 267))
			{
				lpObj.pInventory[lpMsg->Pos].SetPeriodItemExpire();
				g_PeriodItemEx.RemovePeriodItemDataByForce(lpObj, lpObj.pInventory[lpMsg->Pos].m_Type, lpObj.pInventory[lpMsg->Pos].m_Number);
			}

			else if (lpObj.pInventory[lpMsg->Pos].IsPeriodItemExpire() == FALSE)
			{
				IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
				return;
			}
		}
	}

	if (lpObj.pInventory[lpMsg->Pos].m_Type == ITEMGET(14, 12) && lpObj.pInventory[lpMsg->Pos].m_Level == 1) // Heart+1
	{
		pResult.Result = true;
		pResult.Money = lpObj.m_PlayerData->Money;
		gObjInventoryItemSet(aIndex, lpMsg->Pos, -1);
		lpObj.pInventory[lpMsg->Pos].Clear();
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if (!IsSellToNPCItemObject(lpObj.pInventory[lpMsg->Pos].m_Type))
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (lpObj.pInventory[lpMsg->Pos].IsItem() == FALSE)
	{
		pResult.Result = false;
		pResult.Money = lpObj.m_PlayerData->Money;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

	}
	else
	{
		BYTE NewOption[MAX_EXOPTION_SIZE];
		int iAddZen = 0;
		int type = lpObj.pInventory[lpMsg->Pos].m_Type;

		if (type == ITEMGET(13, 4) || type == ITEMGET(13, 5)) // Pets
			lpObj.pInventory[lpMsg->Pos].PetValue();
		else
			lpObj.pInventory[lpMsg->Pos].Value();

		iAddZen = lpObj.pInventory[lpMsg->Pos].m_SellMoney;

		if (gObjCheckMaxZen(aIndex, iAddZen) == FALSE)
		{
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}

		lpObj.m_PlayerData->Money += iAddZen;
		pResult.Result = true;
		pResult.Money = lpObj.m_PlayerData->Money;


		ItemIsBufExOption(NewOption, &lpObj.pInventory[lpMsg->Pos]);

		if (g_PentagramSystem.IsPentagramItem(&lpObj.pInventory[lpMsg->Pos]))
		{
			g_PentagramSystem.DelPentagramJewelInfo(aIndex, &lpObj.pInventory[lpMsg->Pos]);
		}

		bool bSendEquip = false;

		if ((lpObj.pInventory[lpMsg->Pos].m_Type == ITEMGET(13, 169) || lpObj.pInventory[lpMsg->Pos].m_Type == ITEMGET(13, 170)) && (lpMsg->Pos == 10 || lpMsg->Pos == 11))
		{
			bSendEquip = true;
		}

		if (IsExpensiveItem(&lpObj.pInventory[lpMsg->Pos]) == TRUE && lpObj.pInventory[lpMsg->Pos].IsPeriodItem() == FALSE)
		{
			g_CancelItemSale.GDReqAddItemToList(aIndex, lpObj.pInventory[lpMsg->Pos], iAddZen);
		}

		::gObjInventoryDeleteItem(aIndex, lpMsg->Pos);

		if (bSendEquip == true)
		{
			this->GCEquipmentSend(aIndex);
		}

		if (lpMsg->Pos == 10 || lpMsg->Pos == 11)
		{
			::gObjUseSkill.SkillChangeUse(aIndex);
		}

		::gObjMakePreviewCharSet(aIndex);

	}

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
}


int GameProtocol::GetNeedMoneyItemDurRepaire(/*CGameObject &Obj, */CItemObject *  DurItem, BOOL RequestPos)
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
	PBMSG_HEAD2 h;	// C1:34
	int Money;	// 4
};



void GameProtocol::ItemDurRepaire(CGameObject &Obj, /*CGameObject &Obj, */CItemObject * DurItem, int pos, int RequestPos)
{
	PMSG_ITEMDURREPAIR_RESULT pResult;
	int result = TRUE;

	PHeadSetB((BYTE*)&pResult, 0x34, sizeof(pResult));
	int itemtype = DurItem->m_Type;

	if (lpObj.m_IfState.type != 13)
	{
		if (itemtype == ITEMGET(13, 4) || itemtype == ITEMGET(13, 5))
		{
			pResult.Money = 0;
			IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (BYTE*)&pResult, pResult.h.size);

			return;
		}
	}

	if (DurItem->IsPeriodItem() == TRUE)
	{
		pResult.Money = 0;
		IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if (itemtype >= ITEMGET(14, 0) || (itemtype >= ITEMGET(13, 0) && itemtype < ITEMGET(13, 4)) || itemtype == ITEMGET(13, 10) || (itemtype >= ITEMGET(12, 7) && itemtype <= ITEMGET(12, 35)) || itemtype == ITEMGET(4, 7) || itemtype == ITEMGET(4, 15))
	{
		pResult.Money = 0;
		IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if (itemtype == ITEMGET(13, 38))
	{
		pResult.Money = 0;
		IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if (itemtype == ITEMGET(13, 39))
	{
		pResult.Money = 0;
		IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if (itemtype == ITEMGET(13, 40))
	{
		pResult.Money = 0;
		IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if (itemtype == ITEMGET(13, 41))
	{
		pResult.Money = 0;
		IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if (itemtype == ITEMGET(13, 42))
	{
		pResult.Money = 0;
		IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	pResult.Money = GetNeedMoneyItemDurRepaire(/*aIndex, */DurItem, RequestPos);

	if (pResult.Money <= 0)
	{
		pResult.Money = 0;
		IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if ((lpObj.m_PlayerData->Money - pResult.Money) < 1)
		pResult.Money = 0;
	else
	{
		lpObj.m_PlayerData->Money -= pResult.Money;
		pResult.Money = lpObj.m_PlayerData->Money;
		DurItem->m_Durability = (float)((int)DurItem->m_BaseDurability);
		DurItem->Convert(DurItem->m_Type, DurItem->m_Option1, DurItem->m_Option2, DurItem->m_Option3,
			DurItem->m_NewOption, DurItem->m_SetOption, DurItem->m_ItemOptionEx, 0, -1, 0, CURRENT_DB_VERSION);
		GCItemObjectDurSend(lpObj.m_Index, pos, DurItem->m_Durability, FALSE);
	}

	IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (BYTE*)&pResult, pResult.h.size);
}




void GameProtocol::CGModifyRequestItem(PMSG_ITEMDURREPAIR * lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(aIndex))
		return;

	
	PMSG_ITEMDURREPAIR_RESULT pResult;

	PHeadSetB((BYTE*)&pResult, 0x34, sizeof(pResult));
	pResult.Money = 0;

	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	if (lpObj.CloseType != -1)
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (!PacketCheckTime(aIndex))
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (lpMsg->Requestpos == 1 && lpObj.Level < 50)
	{
		pResult.Money = 0;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (lpMsg->Position == 0xFF)
	{
		for (int n = 0; n < MAIN_INVENTORY_SIZE; n++)
		{
			if (lpObj.pInventory[n].IsItem())
			{
				if (IsCashItem(lpObj.pInventory[n].m_Type) == TRUE)
					continue;

				if (IsPremiumItem(lpObj.pInventory[n].m_Type) == TRUE)
					continue;

				if (lpObj.pInventory[n].IsPeriodItem() == TRUE)
					continue;

				if (lpObj.pInventory[n].IsPeriodItemExpire() == TRUE)
					continue;

				if (g_LuckyItemManager.IsLuckyItemEquipment(lpObj.pInventory[n].m_Type))	//season 6.1 add-on
					continue;

				if (lpObj.pInventory[n].m_Type == ITEMGET(13, 20) && (lpObj.pInventory[n].m_Level == 0 || lpObj.pInventory[n].m_Level == 1))
					continue;

				if (lpObj.pInventory[n].m_Type == ITEMGET(13, 20) && lpObj.pInventory[n].m_Level == 2)
					continue;

				if (lpObj.pInventory[n].m_Type == ITEMGET(13, 4) || lpObj.pInventory[n].m_Type == ITEMGET(13, 5))
					continue;

				if (lpObj.pInventory[n].m_Type == ITEMGET(13, 37))
					continue;

				if (lpObj.pInventory[n].m_Type == ITEMGET(13, 38))
					continue;

				if (lpObj.pInventory[n].m_Type == ITEMGET(13, 40))
					continue;

				if (lpObj.pInventory[n].m_Type == ITEMGET(13, 41))
					continue;

				if (lpObj.pInventory[n].m_Type == ITEMGET(13, 42))
					continue;

				if (lpObj.pInventory[n].m_Type == ITEMGET(13, 51))
					continue;

				if (lpObj.pInventory[n].m_Type == ITEMGET(13, 64))
					continue;

				if (lpObj.pInventory[n].m_Type == ITEMGET(13, 65))
					continue;

				if (lpObj.pInventory[n].m_Type == ITEMGET(13, 67))
					continue;

				if (lpObj.pInventory[n].m_Type == ITEMGET(13, 70))
					continue;

				if (lpObj.pInventory[n].m_Type == ITEMGET(13, 76))
					continue;

				if (lpObj.pInventory[n].m_Type == ITEMGET(13, 77))
					continue;

				if (lpObj.pInventory[n].m_Type == ITEMGET(13, 78))
					continue;

				if (lpObj.pInventory[n].m_Type == ITEMGET(13, 80))
					continue;

				if (lpObj.pInventory[n].m_Type == ITEMGET(13, 106))
					continue;

				if (lpObj.pInventory[n].m_Type == ITEMGET(13, 123))
					continue;

				if (lpObj.pInventory[n].m_Type == ITEMGET(13, 39))
					continue;

				if (lpObj.pInventory[n].m_Type == ITEMGET(12, 26) &&
					(lpObj.pInventory[n].m_Level == 1 ||
						lpObj.pInventory[n].m_Level == 2 ||
						lpObj.pInventory[n].m_Level == 3 ||
						lpObj.pInventory[n].m_Level == 4 ||
						lpObj.pInventory[n].m_Level == 5))
					continue;

				if (!IsRepairItem(lpObj.pInventory[n].m_Type))
					continue;

				ItemDurRepaire(lpObj,/*aIndex,*/ &lpObj.pInventory[n], n, lpMsg->Requestpos);
			}
		}

		gObjCalCharacter.CalcCharacter(lpObj.m_Index);
		return;
	}

	if (lpMsg->Position > MAIN_INVENTORY_SIZE - 1)
	{
		pResult.Money = 0;
		return;
	}

	if (lpObj.pInventory[lpMsg->Position].IsPeriodItem() == TRUE)
		return;

	if (IsCashItem(lpObj.pInventory[lpMsg->Position].m_Type) == TRUE)
		return;

	if (IsPremiumItem(lpObj.pInventory[lpMsg->Position].m_Type) == TRUE)
		return;

	if (lpObj.pInventory[lpMsg->Position].IsPeriodItemExpire() == TRUE)
		return;

	if (g_LuckyItemManager.IsLuckyItemEquipment(lpObj.pInventory[lpMsg->Position].m_Type))	//season 6.1 add-on
		return;

	if (lpObj.pInventory[lpMsg->Position].m_Type == ITEMGET(13, 20) && (lpObj.pInventory[lpMsg->Position].m_Level == 0 || lpObj.pInventory[lpMsg->Position].m_Level == 1))
		return;

	if (lpObj.pInventory[lpMsg->Position].m_Type == ITEMGET(13, 20) && lpObj.pInventory[lpMsg->Position].m_Level == 2)
		return;

	if (lpObj.pInventory[lpMsg->Position].m_Type == ITEMGET(13, 37))
		return;

	if (lpObj.pInventory[lpMsg->Position].m_Type == ITEMGET(13, 38))
		return;

	if (lpObj.pInventory[lpMsg->Position].m_Type == ITEMGET(13, 40))
		return;

	if (lpObj.pInventory[lpMsg->Position].m_Type == ITEMGET(13, 41))
		return;

	if (lpObj.pInventory[lpMsg->Position].m_Type == ITEMGET(13, 42))
		return;

	if (lpObj.pInventory[lpMsg->Position].m_Type == ITEMGET(13, 51))
		return;

	if (lpObj.pInventory[lpMsg->Position].m_Type == ITEMGET(13, 70))
		return;

	if (lpObj.pInventory[lpMsg->Position].m_Type == ITEMGET(13, 66))
		return;

	if (lpObj.pInventory[lpMsg->Position].m_Type == ITEMGET(13, 76))
		return;

	if (lpObj.pInventory[lpMsg->Position].m_Type == ITEMGET(13, 77))
		return;

	if (lpObj.pInventory[lpMsg->Position].m_Type == ITEMGET(13, 78))
		return;

	if (lpObj.pInventory[lpMsg->Position].m_Type == ITEMGET(13, 80))
		return;

	if (lpObj.pInventory[lpMsg->Position].m_Type == ITEMGET(13, 106))
		return;

	if (lpObj.pInventory[lpMsg->Position].m_Type == ITEMGET(13, 123))
		return;

	if (lpObj.pInventory[lpMsg->Position].m_Type == ITEMGET(13, 39))
		return;

	if (!IsRepairItem(lpObj.pInventory[lpMsg->Position].m_Type))
		return;

	ItemDurRepaire(lpObj,/*aIndex,*/ &lpObj.pInventory[lpMsg->Position], lpMsg->Position, lpMsg->Requestpos);
	gObjCalCharacter.CalcCharacter(lpObj.m_Index);
}



struct PMSG_TRADE_REQUESTSEND
{
	PBMSG_HEAD2 h;	// C3:36
	char szId[10];	// 3
};


void GameProtocol::CGTradeRequestSend(PMSG_TRADE_REQUEST * lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(aIndex))
		return;

	int number;

	if (bCanTrade == FALSE)
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 57), aIndex, 1);
		this->GCServerMsgStringSend(Lang.GetText(0, 58), aIndex, 1);
		return;
	}

	if (lpObj.CloseType != -1)
		return;

	

	if (!PacketCheckTime(aIndex))
		return;

	number = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	if (ObjectMaxRange(number) == FALSE)
	{
		return;
	}

	if (number == aIndex)
		return;

	if (!gObjIsConnected(number))
		return;

	if (getGameObject(number)->Type != OBJ_USER || lpObj.Type != OBJ_USER) // ANTI_CRASH
	{
		return;
	}

	if (lpObj.m_PlayerData->m_bSecurityCheck == false)
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 521), aIndex, 1);
		return;
	}

	if (getGameObject(number)->m_PlayerData->m_bSecurityCheck == false)
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 606), aIndex, 1);
		return;
	}

	if (getGameObject(number)->m_bMapSvrMoveReq == true || lpObj.m_bMapSvrMoveReq == true)
	{
		return;
	}

	if (getGameObject(number)->m_bMapSvrMoveQuit == true || lpObj.m_bMapSvrMoveQuit == true)
	{
		return;
	}

	if (getGameObject(number)->CloseCount >= 0)
		return;

	if (DS_MAP_RANGE(getGameObject(number)->MapNumber) != FALSE)
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 12), aIndex, 1);
		return;
	}

	if (lpObj.m_bPShopOpen == true)
		return;

	if (getGameObject(number)->m_bPShopOpen == true)
		return;

	if (CC_MAP_RANGE(getGameObject(number)->MapNumber) != FALSE)
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 114), aIndex, 1);
		return;
	}

	if (getGameObject(number)->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 572), aIndex, 1);
		return;
	}

	if (lpObj.m_PlayerData->m_JoinUnityBattle == true ||
		getGameObject(number)->m_PlayerData->m_JoinUnityBattle == true ||
		g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 618), aIndex, 1);
		return;
	}

	if (IT_MAP_RANGE(getGameObject(number)->MapNumber) != FALSE)
	{
		GCServerMsgStringSend(Lang.GetText(0, 290), aIndex, 1);
		return;
	}

	if (DG_MAP_RANGE(getGameObject(number)->MapNumber) != FALSE)
	{
		GCServerMsgStringSend(Lang.GetText(0, 303), aIndex, 1);
		return;
	}

	if (IMPERIAL_MAP_RANGE(getGameObject(number)->MapNumber) != FALSE)
	{
		GCServerMsgStringSend(Lang.GetText(0, 304), aIndex, 1);
		return;
	}

	if (BC_MAP_RANGE(lpObj.MapNumber) != FALSE)
	{
		int iBridgeIndex = g_BloodCastle.GetBridgeIndex(lpObj.MapNumber);
		if (g_BloodCastle.GetCurrentState(iBridgeIndex) != 1 || g_BloodCastle.CheckCanEnter(iBridgeIndex) == false)
		{
			this->GCServerMsgStringSend(Lang.GetText(0, 107), aIndex, 1);
			return;
		}
	}
	if (getGameObject(number)->m_PlayerData->ISBOT == true && g_BotSystem.GetBotType(number) == BOT_TRADE)
	{
		g_BotSystem.AlchemistTradeOpen(aIndex, number);
		return;
	}
	if (getGameObject(number)->m_PlayerData->ISBOT == true && g_BotSystem.GetBotType(number) == BOT_BUFFER)
	{
		getGameObject(number)->BuffPlayerIndex = aIndex;
		g_BotSystem.BuffPlayer(getGameObject(number)->BufferIndex, aIndex);
		return;
	}

	if ((getGameObject(number)->m_Option & 1) != 1)
	{
		this->GCTradeResponseSend(0, aIndex, getGameObject(number)->Name, 0, 0);
		return;
	}

	if (lpObj.m_IfState.use > 0)
	{
		this->GCTradeResponseSend(3, aIndex, getGameObject(number)->Name, 0, 0);
		return;
	}

	if (getGameObject(number)->m_IfState.use > 0)
	{
		this->GCTradeResponseSend(2, aIndex, getGameObject(number)->Name, 0, 0);
		return;
	}

	if (g_NewPVP.IsDuel(getGameObject(aIndex]) || g_NewPVP.IsDuel(getGameObject(number)))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 311), aIndex, 1);
		return;
	}

	if (g_NewPVP.IsObserver(getGameObject(aIndex]) || g_NewPVP.IsObserver(getGameObject(number)))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 312), aIndex, 1);
		return;
	}

	if (::gObjFixInventoryPointer(aIndex) == false)
	{

	}

	if (lpObj.pTransaction == 1)
	{
		return;
	}

	lpObj.m_IfState.use = 1;
	lpObj.m_IfState.state = 0;
	lpObj.m_IfState.type = 1;
	lpObj.TargetNumber = number;
	getGameObject(number)->m_IfState.use = 1;
	getGameObject(number)->m_IfState.state = 0;
	getGameObject(number)->m_IfState.type = 1;
	getGameObject(number)->TargetNumber = aIndex;
	lpObj.m_InterfaceTime = GetTickCount();
	getGameObject(number)->m_InterfaceTime = GetTickCount();

	PMSG_TRADE_REQUESTSEND pMsg;

	pMsg.h.c = 0xC3;
	pMsg.h.headcode = 0x36;
	pMsg.h.size = sizeof(pMsg);
	memcpy(pMsg.szId, lpObj.Name, sizeof(pMsg.szId));

	IOCP.DataSend(number, (BYTE*)&pMsg, pMsg.h.size);

	TradeLog->Output("Request to trade [%s][%s][%s] to [%s][%s][%s]", lpObj.AccountID, lpObj.Name, lpObj.m_PlayerData->Ip_addr,
		getGameObject(number]->AccountID, getGameObject(number)->Name, getGameObject(number)->m_PlayerData->Ip_addr);
}



void GameProtocol::CGTradeResponseRecv(PMSG_TRADE_RESPONSE * lpMsg, CGameObject &Obj)
{
	if (ObjectMaxRange(aIndex) == FALSE)
	{
		return;
	}

	BOOL Result = TRUE;
	int number = lpObj.TargetNumber;

	if (ObjectMaxRange(number) == FALSE)
	{
		if (lpObj.m_IfState.use != FALSE)
		{
			if (lpObj.m_IfState.type == 1)
			{
				lpObj.m_IfState.use = FALSE;
				lpObj.TargetNumber = -1;
			}
		}

		lpMsg->Response = false;
		GCTradeResponseSend(lpMsg->Response, aIndex, lpObj.Name, 0, 0);
		return;
	}

	if (gObjIsConnectedGP(aIndex) == FALSE)
	{
		Result = FALSE;
		lpMsg->Response = false;
	}

	if (gObjIsConnectedGP(number) == FALSE)
	{
		Result = FALSE;
		lpMsg->Response = false;
	}

	if (lpObj.CloseType != -1)
	{
		Result = FALSE;
		lpMsg->Response = false;
	}

	if (lpObj.Type == OBJ_USER && lpObj.m_bPShopOpen == true)
	{
		Result = FALSE;
		lpMsg->Response = false;
	}

	if (getGameObject(number]->Type == OBJ_USER && getGameObject(number)->m_bPShopOpen == true)
	{
		Result = FALSE;
		lpMsg->Response = false;
	}

	if (lpObj.X < (getGameObject(number]->X - 2) || lpObj.X >(getGameObject(number]->X + 2) || lpObj.Y < (getGameObject(number)->Y - 2) || lpObj.Y >(getGameObject(number)->Y + 2))
	{
		Result = FALSE;
		lpMsg->Response = false;
	}

	if (lpObj.m_PlayerData->ISBOT == FALSE)
	{
		if (lpObj.m_IfState.use == FALSE || lpObj.m_IfState.type != 1 || lpObj.m_IfState.state != 0)
		{
			return;
		}
	}

	if (getGameObject(number]->m_IfState.use == FALSE || getGameObject(number)->m_IfState.type != 1 || getGameObject(number)->m_IfState.state != 0)
	{
		return;
	}

	if (gObjFixInventoryPointer(aIndex) == false)
	{

	}

	if (lpObj.pTransaction == 1 && lpObj.m_PlayerData->ISBOT == FALSE)
	{
		return;
	}

	if (lpMsg->Response == false)
	{
		GCTradeResponseSend(lpMsg->Response, number, lpObj.Name, 0, 0);
		Result = FALSE;
	}
	else
	{
		Result = TRUE;

		for (int i = 0; i < TRADE_BOX_SIZE; i++)
		{
			lpObj.Trade[i].Clear();
			getGameObject(number]->Trade[i)->Clear();
		}

		memset(lpObj.TradeMap, (BYTE)-1, TRADE_BOX_MAP_SIZE);
		memset(getGameObject(number)->TradeMap, (BYTE)-1, TRADE_BOX_MAP_SIZE);
		lpObj.m_IfState.state = 1;
		getGameObject(number)->m_IfState.state = 1;
		lpObj.TradeMoney = 0;
		getGameObject(number)->TradeMoney = 0;

		if (::gObjInventoryTrans(aIndex) == FALSE)
			Result = 2;

		if (::gObjInventoryTrans(number) == FALSE)
			Result = 3;

		if (Result != TRUE)
		{
			lpMsg->Response = false;
			GCTradeResponseSend(lpMsg->Response, number, lpObj.Name, 0, 0);
			GCTradeResponseSend(lpMsg->Response, aIndex, getGameObject(number)->Name, 0, 0);

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
			GCTradeResponseSend(lpMsg->Response, number, lpObj.Name, lpObj.Level, lpObj.m_PlayerData->GuildNumber);
			GCTradeResponseSend(lpMsg->Response, aIndex, getGameObject(number]->Name, getGameObject(number)->Level, getGameObject(number)->m_PlayerData->GuildNumber);

		}
	}

	if (Result == FALSE)
	{
		if (lpObj.m_IfState.use != FALSE && lpObj.m_IfState.type == 1)
		{
			lpObj.m_IfState.use = FALSE;
			lpObj.TargetNumber = -1;
		}

		if (getGameObject(number)->TargetNumber == aIndex)
		{
			if (getGameObject(number]->m_IfState.use != FALSE && getGameObject(number)->m_IfState.type == 1)
			{
				getGameObject(number)->m_IfState.use = FALSE;
				getGameObject(number)->TargetNumber = -1;
			}
		}
	}
}




bool GameProtocol::GCTradeResponseSend(BYTE response, CGameObject &Obj, LPSTR id, WORD level, int GuildNumber)
{
	PMSG_TRADE_RESPONSE pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x37, sizeof(pMsg));
	pMsg.Response = response;
	pMsg.Level = level;
	pMsg.GuildNumber = GuildNumber;
	memcpy(pMsg.Id, id, sizeof(pMsg.Id));

	return IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}

struct PMSG_TRADE_OTHER_DEL
{
	PBMSG_HEAD2 h;	// C1:38
	BYTE TradeItemIndex;	// 3
};

int GameProtocol::GCTradeOtherDel(CGameObject &Obj, BYTE tradeindex)
{
	PMSG_TRADE_OTHER_DEL pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x38, sizeof(pMsg));
	pMsg.TradeItemIndex = tradeindex;

	return IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}

struct PMSG_TRADE_OTHER_MOVE
{
	PBMSG_HEAD2 h;	// C1:39
	BYTE TradeItemIndex;	// 3
	BYTE ItemInfo[MAX_ITEM_INFO];	// 4
};


int GameProtocol::GCTradeOtherAdd(CGameObject &Obj, BYTE tradeindex, BYTE* iteminfo)
{
	PMSG_TRADE_OTHER_MOVE pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x39, sizeof(pMsg));
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

	return IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}



void GameProtocol::CGTradeMoneyRecv(PMSG_TRADE_GOLD * lpMsg, CGameObject &Obj)
{
	PMSG_DEFRESULT pMsg;
	int money = 0;
	int number;

	if (!PacketCheckTime(aIndex))
		return;

	if (!ObjectMaxRange(aIndex))
	{
		return;
	}

	if (lpObj.CloseType != -1)
	{
		return;
	}

	number = lpObj.TargetNumber;

	if (!ObjectMaxRange(number))
	{
		return;
	}

	if (getGameObject(number)->CloseType != -1)
	{
		return;
	}

	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	if (getGameObject(number)->Type != OBJ_USER)
	{
		return;
	}

	if (gObjIsConnectedGP(number) == FALSE)
	{
		return;
	}

	if (lpObj.m_IfState.use == FALSE && lpObj.m_IfState.type != 1)
		return;

	if (gObjCanItemTouch(&getGameObject(aIndex), 1) == FALSE)
		return;

	if (lpMsg->Money > 1000000000)
		return;

	money = lpMsg->Money;

	if ((lpObj.m_PlayerData->Money - money) < 0)
		return;

	GCMoneySend(aIndex, lpObj.m_PlayerData->Money - money);

	PHeadSetB((BYTE*)&pMsg, 0x3A, sizeof(pMsg));
	pMsg.result = 1;

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

	if (number >= 0)
	{
		lpObj.TradeMoney = money;
		lpObj.TradeOk = false;
		getGameObject(number)->TradeOk = false;

		GCTradeOkButtonSend(number, 2);
		GCTradeOkButtonSend(aIndex, 0);
		GCTradeMoneyOther(number, lpObj.TradeMoney);
	}

	TradeLog->Output("[%s][%s][%s] -> [%s][%s][%s]: Trade Money %d", lpObj.AccountID, lpObj.Name, lpObj.m_PlayerData->Ip_addr, getGameObject(number]->AccountID, getGameObject(number)->Name, getGameObject(number)->m_PlayerData->Ip_addr, money);
}

struct PMSG_TRADE_OTHER_GOLD
{
	PBMSG_HEAD2 h;	// C1:3B
	DWORD Money;	// 4
};


bool GameProtocol::GCTradeMoneyOther(CGameObject &Obj, DWORD money)
{
	PMSG_TRADE_OTHER_GOLD pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x3B, sizeof(pMsg));
	pMsg.Money = money;

	return IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}



bool GameProtocol::GCTradeOkButtonSend(CGameObject &Obj, BYTE flag)
{
	PMSG_TRADE_OKBUTTON pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x3C, sizeof(pMsg));
	pMsg.Flag = flag;

	return IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}


void GameProtocol::CGTradeOkButtonRecv(PMSG_TRADE_OKBUTTON * lpMsg, CGameObject &Obj)
{
	int number = lpObj.TargetNumber;

	if (number < 0 || number > g_ConfigRead.server.GetObjectMax() - 1)
		return;

	if (getGameObject(number]->Type == OBJ_USER && getGameObject(number)->m_PlayerData->ISBOT == 1)
	{
		if (lpMsg->Flag == 1)
		{
			g_BotSystem.AlchemistTradeOk(aIndex, number);
			return;
		}
	}

	if (!gObjIsConnected(&getGameObject(number)))
		return;

	if (lpObj.Type != OBJ_USER)
		return;

	if (getGameObject(number)->Type != OBJ_USER)
		return;

	if (lpObj.m_IfState.use < 1 || lpObj.m_IfState.state != 1)
		return;

	if (!getGameObject(number)->m_PlayerData->ISBOT)
	{
		if (getGameObject(number]->m_IfState.use < 1 || getGameObject(number)->m_IfState.state != 1)
			return;
	}
	if (lpObj.CloseType != -1)
		return;

	if (getGameObject(number)->CloseType != -1)
		return;

	if (lpMsg->Flag == 1)
	{
		if (lpObj.TradeOk == false)
		{
			lpObj.TradeOk = true;
			GCTradeOkButtonSend(number, 1);
		}
	}
	else if (lpMsg->Flag == 0)
	{
		if (lpObj.TradeOk == true)
		{
			GCTradeOkButtonSend(number, 0);
			lpObj.TradeOk = false;
		}
	}

	gObjTradeOkButton(aIndex);
	TradeLog->Output("[%s][%s][%s] -> [%s][%s][%s]: Trade Accept!", lpObj.AccountID, lpObj.Name, lpObj.m_PlayerData->Ip_addr, getGameObject(number]->AccountID, getGameObject(number)->Name, getGameObject(number)->m_PlayerData->Ip_addr);
}


void GameProtocol::CGTradeCancelButtonRecv(CGameObject &Obj)
{
	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	int number = lpObj.TargetNumber;
	gObjTradeCancel(aIndex);
	CGTradeResult(aIndex, 0);

	if (number >= 0)
	{
		gObjTradeCancel(number);
		CGTradeResult(number, 0);
	}

	TradeLog->Output("[%s][%s][%s]: Trade Cancel Button Press", lpObj.AccountID, lpObj.Name, lpObj.m_PlayerData->Ip_addr);
}


struct PMSG_TRADE_RESULT
{
	PBMSG_HEAD2 h;	// C1:3D
	BYTE Result;	// 3
};


void GameProtocol::CGTradeResult(CGameObject &Obj, BYTE result)
{
	PMSG_TRADE_RESULT pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x3D, sizeof(pMsg));
	pMsg.Result = result;

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}

struct PMSG_PARTYREQUESTSEND
{
	PBMSG_HEAD2 h;	// C1:40
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
};

void GameProtocol::CGPartyRequestRecv(PMSG_PARTYREQUEST * lpMsg, CGameObject &Obj)
{
	int number = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
	PMSG_PARTYREQUESTSEND pMsg;

	if (number < 0 || number > g_ConfigRead.server.GetObjectMax() - 1)
		return;

	if (!PacketCheckTime(aIndex))
		return;

	if (!gObjIsConnected(&getGameObject(number)))
	{
		this->GCResultSend(aIndex, 0x41, 0x03);
		return;
	}

	if (lpObj.CloseCount >= 0 || getGameObject(number)->CloseCount >= 0)
	{
		this->GCResultSend(aIndex, 0x41, 0x00);
		return;
	}

	if (getGameObject(number)->m_PlayerData->m_bUsePartyMatching)
	{
		GCResultSend(aIndex, 0x41, 0x09);
		return;
	}

	if (getGameObject(number)->Type != OBJ_USER)
	{
		this->GCSendDisableReconnect(aIndex);
		//IOCP.CloseClient(aIndex);
		return;
	}

	if (g_NewPVP.IsDuel(getGameObject(aIndex]) || g_NewPVP.IsDuel(getGameObject(number)))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 315), aIndex, 1);
		return;
	}

	if (g_NewPVP.IsObserver(getGameObject(aIndex)))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 316), aIndex, 1);
		return;
	}

	if (CC_MAP_RANGE(lpObj.MapNumber))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 116), aIndex, 1);
		return;
	}

	if (lpObj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 573), aIndex, 1);
		return;
	}

	if (IT_MAP_RANGE(lpObj.MapNumber))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 291), aIndex, 1);
		return;
	}

	if (lpObj.m_IfState.use > 0)
	{
		this->GCResultSend(aIndex, 0x41, 0x00);
		return;
	}

	if (getGameObject(number)->m_IfState.use > 0)
	{
		this->GCResultSend(aIndex, 0x41, 0x00);
		return;
	}

	if ((getGameObject(number)->m_Option & 1) != TRUE)
	{
		this->GCResultSend(aIndex, 0x41, 0x01);
		return;
	}

	if (lpObj.PartyNumber >= 0)
	{
		CGameObject &Obj;
		int iDbNumber;
		gParty.GetIndexUser(lpObj.PartyNumber, 0, iUserIndex, iDbNumber);

		if (iUserIndex < 0 || (iUserIndex != aIndex && lpObj.DBNumber != iDbNumber))
		{
			this->GCResultSend(aIndex, 0x41, 0x00);
			return;

		}
	}

	if (lpObj.PartyTargetUser >= 0)
		return;

	if (getGameObject(number)->PartyNumber >= 0)
	{
		this->GCResultSend(aIndex, 0x41, 0x04);
		return;
	}

	if (getGameObject(number)->PartyTargetUser >= 0)
	{
		this->GCResultSend(aIndex, 0x41, 0x00);
		return;
	}

	if (g_GensSystem.GetGensInfluence(&getGameObject(aIndex]) != g_GensSystem.GetGensInfluence(&getGameObject(number)) && g_GensSystem.CanGensJoinPartyWhileOppositeGens() == FALSE)
	{
		this->GCResultSend(aIndex, 0x41, 0x06);
		return;
	}

	if (g_GensSystem.IsMapBattleZone(lpObj.MapNumber) == TRUE && g_GensSystem.CanGensCreatePartyOnBattleZone() == FALSE)
	{
		this->GCResultSend(aIndex, 0x41, 0x07);
		return;
	}

	if (g_ArcaBattle.IsArcaBattleServer() && lpObj.m_PlayerData->GuildNumber != getGameObject(number)->m_PlayerData->GuildNumber)
	{
		if (g_AcheronGuardianEvent.IsPlayStart() == false)
		{
			GCServerMsgStringSend(Lang.GetText(0, 522), aIndex, 1);
			return;
		}
	}

	if (lpObj.m_PlayerData->RegisterdLMS == 1)
	{
		MsgOutput(aIndex, Lang.GetText(0, 523));
		return;
	}

	if (getGameObject(number)->m_PlayerData->RegisterdLMS == 1)
	{
		MsgOutput(aIndex, Lang.GetText(0, 524));
		return;
	}

	int maxlevel = 0;
	int minlevel = 0;

	if (getGameObject(number)->PartyNumber >= 0)
	{

		if (gParty.Isleader(getGameObject(number]->PartyNumber, number, getGameObject(number)->DBNumber) == FALSE)
		{
			char szTemp[256];
			wsprintf(szTemp, "%s is already in a party.", getGameObject(number)->Name);
			GCServerMsgStringSend(szTemp, aIndex, 1);
			return;
		}

		if (gParty.GetLevel(getGameObject(number)->PartyNumber, maxlevel, minlevel) == TRUE)
		{
			int limmaxlevel;
			int limmaxlevel2;

			if (maxlevel > getGameObject(number)->Level)
				limmaxlevel = maxlevel;
			else
				limmaxlevel = getGameObject(number)->Level;

			if (maxlevel < getGameObject(number)->Level)
				limmaxlevel2 = maxlevel;
			else
				limmaxlevel2 = getGameObject(number)->Level;

			if ((limmaxlevel - limmaxlevel2) > g_MaxStatsInfo.GetClass.Max_Party_Level_Dif)
			{
				this->GCResultSend(aIndex, 0x41, 0x05);
				// Restricted level for the party is %d
				return;
			}

			int limminlevel;
			int limminlevel2;

			if (maxlevel > getGameObject(number)->Level)
				limminlevel = minlevel;
			else
				limminlevel = getGameObject(number)->Level;

			if (maxlevel < getGameObject(number)->Level)
				limminlevel2 = minlevel;
			else
				limminlevel2 = getGameObject(number)->Level;

			if ((limminlevel - limminlevel2) > g_MaxStatsInfo.GetClass.Max_Party_Level_Dif)
			{
				this->GCResultSend(aIndex, 0x41, 0x05);
				// Restricted level for the party is %d
				return;
			}
		}
	}
	else
	{
		short sMaxMinLevel[2];

		if (lpObj.Level > getGameObject(number)->Level)
			sMaxMinLevel[1] = lpObj.Level;
		else
			sMaxMinLevel[1] = getGameObject(number)->Level;

		if (lpObj.Level < getGameObject(number)->Level)
			sMaxMinLevel[0] = lpObj.Level;
		else
			sMaxMinLevel[0] = getGameObject(number)->Level;

		if ((sMaxMinLevel[1] - sMaxMinLevel[0]) > g_MaxStatsInfo.GetClass.Max_Party_Level_Dif)
		{
			this->GCResultSend(aIndex, 0x41, 0x05);
			// Restricted level for the party is %d
			return;
		}
	}

	lpObj.m_IfState.use = TRUE;
	lpObj.m_IfState.type = 2;
	lpObj.m_IfState.state = 0;
	getGameObject(number)->m_IfState.use = TRUE;
	getGameObject(number)->m_IfState.type = 2;
	getGameObject(number)->m_IfState.state = 0;
	lpObj.TargetNumber = number;
	getGameObject(number)->TargetNumber = aIndex;
	lpObj.m_InterfaceTime = GetTickCount();
	getGameObject(number)->m_InterfaceTime = GetTickCount();
	lpObj.PartyTargetUser = number;

	PHeadSetB((BYTE*)&pMsg, 0x40, sizeof(pMsg));
	pMsg.NumberH = SET_NUMBERH(aIndex);
	pMsg.NumberL = SET_NUMBERL(aIndex);

	IOCP.DataSend(number, (BYTE*)&pMsg, pMsg.h.size);
}



void GameProtocol::CGPartyRequestResultRecv(PMSG_PARTYREQUESTRESULT * lpMsg, CGameObject &Obj)
{
	int number;
	BYTE result = false;
	char szTemp[256];
	int pnumber = -1;

	if (!PacketCheckTime(aIndex))
		return;

	number = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	if (number < 0 || number > g_ConfigRead.server.GetObjectMax() - 1)
		return;

	if (!gObjIsConnected(&getGameObject(aIndex)))
		return;

	if (!gObjIsConnected(&getGameObject(number)))
	{
		GCResultSend(aIndex, 0x41, 0x04);
	}
	else
	{
		result = true;
	}

	if (lpObj.m_PlayerData->m_bUsePartyMatching)
	{
		GCResultSend(aIndex, 0x41, 0x09);
		return;
	}

	if (getGameObject(number)->MapNumber != lpObj.MapNumber)
	{
		result = false;
		GCResultSend(number, 0x41, 0x00);
	}

	if (lpObj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 573), aIndex, 1);
		result = false;
		GCResultSend(number, 0x41, 0x00);
	}

	if (CC_MAP_RANGE(lpObj.MapNumber))
	{
		GCServerMsgStringSend(Lang.GetText(0, 116), aIndex, 1);
		result = false;
		this->GCResultSend(number, 0x41, 0x00);
	}

	if (lpMsg->Result == 0)
	{
		result = false;
		this->GCResultSend(number, 0x41, 0x01);
	}

	if (getGameObject(number]->m_IfState.use == 0 || getGameObject(number)->m_IfState.type != 2)
	{
		result = false;
		this->GCResultSend(number, 0x41, 0x00);
	}

	if (lpObj.m_IfState.use == 0 || lpObj.m_IfState.type != 2)
	{
		result = false;
		this->GCResultSend(number, 0x41, 0x00);
	}

	if (result == 1)
	{
		if (getGameObject(number)->PartyNumber < 0)
		{
			getGameObject(number]->PartyNumber = gParty.Create(number, getGameObject(number)->DBNumber, getGameObject(number)->Level);
		}

		if (getGameObject(number)->PartyNumber >= 0)
		{
			pnumber = getGameObject(number)->PartyNumber;
			int iPartyPos = gParty.Add(getGameObject(number)->PartyNumber, aIndex, lpObj.DBNumber, lpObj.Level);

			if (iPartyPos >= 0)
			{
				lpObj.PartyNumber = getGameObject(number)->PartyNumber;
				result = true;

				if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
				{
					wsprintf(szTemp, Lang.GetText(0, 20), lpObj.m_PlayerData->m_RealNameOfUBF);
					this->GCServerMsgStringSend(szTemp, number, 1);
					wsprintf(szTemp, Lang.GetText(0, 20), getGameObject(number)->m_PlayerData->m_RealNameOfUBF);
					this->GCServerMsgStringSend(szTemp, aIndex, 1);
				}

				else
				{
					wsprintf(szTemp, Lang.GetText(0, 20), lpObj.Name);
					this->GCServerMsgStringSend(szTemp, number, 1);
					wsprintf(szTemp, Lang.GetText(0, 20), getGameObject(number)->Name);
					this->GCServerMsgStringSend(szTemp, aIndex, 1);
				}

				gParty.Paint(pnumber);

				if (getGameObject(number)->m_PlayerData->m_bUsePartyMatching == true)
				{
					char szLeaderName[MAX_ACCOUNT_LEN + 1];
					memcpy(szLeaderName, getGameObject(number)->Name, MAX_ACCOUNT_LEN + 1);
					GDReqAcceptMemberJoin(number, 1, szLeaderName, lpObj.Name, 1);
				}
			}
			else if (iPartyPos == -1)
			{
				GCResultSend(number, 0x41, 2);
				GCResultSend(aIndex, 0x41, 2);
			}
			else if (iPartyPos == -2)
			{
				result = 2;
				wsprintf(szTemp, Lang.GetText(0, 21), lpObj.Name);
				GCServerMsgStringSend(szTemp, aIndex, 1);
			}
		}
	}

	if (lpObj.m_IfState.use != 0 && lpObj.m_IfState.type == 2)
	{
		lpObj.m_IfState.use = 0;
		lpObj.PartyTargetUser = -1;
	}

	if (getGameObject(number]->m_IfState.use != 0 && getGameObject(number)->m_IfState.type == 2)
	{
		getGameObject(number)->m_IfState.use = 0;
		getGameObject(number)->PartyTargetUser = -1;
	}

	if (pnumber >= 0)
	{
		CGPartyListAll(pnumber);
	}
}

void GameProtocol::CGPartyList(CGameObject &Obj)
{
	PMSG_PARTYLISTCOUNT pCount;
	PMSG_PARTYLIST pList;
	BYTE sendbuf[512];
	int lOfs = 0;

	if (!this->PacketCheckTime(aIndex))
		return;

	PHeadSetB((BYTE*)&pCount, 0x42, sizeof(pCount));
	pCount.Count = 0;
	pCount.Result = false;

	int number;
	int pnumber = lpObj.PartyNumber;

	if (pnumber >= 0)
		pCount.Count = gParty.GetCount(pnumber);

	lOfs += sizeof(pCount);

	if (pCount.Count != 0)
	{
		for (int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			number = gParty.m_PartyS[pnumber].Number[n];

			if (number >= 0)
			{
				if (gObjIsConnected(&getGameObject(number], gParty.m_PartyS[pnumber]->DbNumber[n)) == TRUE)
				{
					memset(&pList, 0, sizeof(pList));
					memcpy(pList.szId, getGameObject(number)->Name, MAX_ACCOUNT_LEN);
					pList.Number = n;
					pList.MapNumber = getGameObject(number)->MapNumber;
					pList.X = getGameObject(number)->X;
					pList.Y = getGameObject(number)->Y;
					pList.Life = getGameObject(number)->Life;
					pList.MaxLife = getGameObject(number]->MaxLife + getGameObject(number)->AddLife;
					pList.nServerChannel = getGameObject(number)->m_PlayerData->m_nServerChannel + 1;
					pList.nMana = getGameObject(number)->Mana;
					pList.nMaxMana = getGameObject(number)->MaxMana;

					memcpy(&sendbuf[lOfs], &pList, sizeof(pList));
					lOfs += sizeof(pList);
				}
			}
		}

		pCount.Result = true;
	}

	pCount.h.size = lOfs;
	memcpy(sendbuf, &pCount, sizeof(pCount));

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&sendbuf, lOfs);
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

	PHeadSetB((BYTE*)&pCount, 0x42, sizeof(pCount));
	pCount.Result = false;
	pCount.Count = gParty.GetCount(pnumber);

	if (pCount.Count == 0)
		return;

	pCount.Result = true;
	lOfs += sizeof(pCount);

	for (int n = 0; n < MAX_USER_IN_PARTY; n++)
	{
		number = gParty.m_PartyS[pnumber].Number[n];

		if (number >= 0)
		{
			if (gObjIsConnected(&getGameObject(number], gParty.m_PartyS[pnumber]->DbNumber[n)) == TRUE)
			{
				memset(&pList, 0, sizeof(pList));

				if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
				{
					memcpy(pList.szId, getGameObject(number)->m_PlayerData->m_RealNameOfUBF, MAX_ACCOUNT_LEN);
				}

				else
				{
					memcpy(pList.szId, getGameObject(number)->Name, MAX_ACCOUNT_LEN);
				}

				pList.Number = n;
				pList.MapNumber = getGameObject(number)->MapNumber;
				pList.X = getGameObject(number)->X;
				pList.Y = getGameObject(number)->Y;
				pList.Life = getGameObject(number)->Life;
				pList.MaxLife = getGameObject(number)->MaxLife;
				pList.nServerChannel = getGameObject(number)->m_PlayerData->m_nServerChannel + 1;
				pList.nMana = getGameObject(number)->Mana;
				pList.nMaxMana = getGameObject(number)->MaxMana;

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
			IOCP.DataSend(number, sendbuf, lOfs);
		}
	}

	for (int n = 0; n < MAX_USER_IN_PARTY; n++)
	{
		number = gParty.m_PartyS[pnumber].Number[n];

		if (number >= 0)
		{
			this->GCDisplayBuffeffectPartyMember(number);
		}
	}
}


void GameProtocol::CGPartyDelUser(PMSG_PARTYDELUSER * lpMsg, CGameObject &Obj, int nType)
{
	int usernumber = -1;
	int dbnumber = -1;

	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	if (lpObj.m_PlayerData->m_bUsePartyMatching == true)
	{
		char szTargetName[MAX_ACCOUNT_LEN + 1];
		memcpy(szTargetName, lpMsg->szTargetName, MAX_ACCOUNT_LEN + 1);

		if (nType == 0)
		{
			memcpy(szTargetName, lpObj.Name, MAX_ACCOUNT_LEN + 1);
		}

		if (nType == 1)
		{
			GDReqDeletePartyUser(aIndex, szTargetName);
		}

		if (lpObj.PartyNumber < 0)
		{
			return;
		}

		if (gParty.RevisionIndexUser(lpObj.PartyNumber, &lpMsg->Number, szTargetName) == FALSE)
		{
			return;
		}

	}

	if (lpObj.PartyNumber < 0)
	{
		return;
	}

	if (!gParty.GetIndexUser(lpObj.PartyNumber, lpMsg->Number, usernumber, dbnumber))
	{
		return;
	}

	if (usernumber < 0)
	{
		return;
	}

	int count;
	int number;
	int pnumber = lpObj.PartyNumber;

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
				int iIndex = gParty.m_PartyS[pnumber].Number[n];

				if (iIndex != -1)
				{
					if (DG_MAP_RANGE(Obj.MapNumber))
					{
						bDPUser = TRUE;
						break;
					}
				}
			}

			if (bDPUser == TRUE && count > 2)
			{
				gParty.Delete(pnumber, lpMsg->Number);
				count = gParty.GetCount(lpObj.PartyNumber);
				getGameObject(usernumber)->PartyNumber = -1;
				getGameObject(usernumber)->PartyTargetUser = -1;
				gParty.UpdatePKPartyPanalty(pnumber);
				gParty.ChangeLeader(pnumber);
				this->GCPartyDelUserSend(usernumber);
				this->CGPartyListAll(pnumber);
				return;
			}
		}

		if (!IMPERIAL_MAP_RANGE(getGameObject(usernumber)->MapNumber))
		{
			if (!lpMsg->Number)
			{
				BOOL bIGUser = FALSE;

				for (int i = 1; i < 5; i++)
				{
					int iIndex = gParty.m_PartyS[pnumber].Number[i];
					if (iIndex != -1)
					{
						if (IMPERIAL_MAP_RANGE(Obj.MapNumber))
						{
							bIGUser = TRUE;
							break;
						}
					}
				}

				if (bIGUser == TRUE && count > 2)
				{
					gParty.Delete(pnumber, lpMsg->Number);
					count = gParty.GetCount(lpObj.PartyNumber);
					getGameObject(usernumber)->PartyNumber = -1;
					getGameObject(usernumber)->PartyTargetUser = -1;
					gParty.UpdatePKPartyPanalty(pnumber);
					gParty.ChangeLeader(pnumber);
					this->GCPartyDelUserSend(usernumber);
					this->CGPartyListAll(pnumber);
					return;
				}
			}

			if (getGameObject(usernumber)->m_PlayerData->lpGuild)
			{
				if (getGameObject(usernumber]->m_PlayerData->lpGuild->WarState == 1 && getGameObject(usernumber)->IsInBattleGround)
				{
					this->GCServerMsgStringSend(Lang.GetText(0, 574), aIndex, TRUE);
					return;
				}

				if (usernumber == gParty.m_PartyS[pnumber].Number[0])
				{
					for (int j = 0; j < 5; j++)
					{
						int number = gParty.m_PartyS[pnumber].Number[j];
						if (number >= 0
							&& getGameObject(number)->m_PlayerData->lpGuild
							&& getGameObject(number)->m_PlayerData->lpGuild->WarState == TRUE
							&& getGameObject(number)->IsInBattleGround)
						{
							this->GCServerMsgStringSend(Lang.GetText(0, 574), aIndex, TRUE);
							return;
						}
					}
				}
			}

			if (lpMsg->Number && count > 2)
			{
				gParty.Delete(pnumber, lpMsg->Number);
				count = gParty.GetCount(lpObj.PartyNumber);
				getGameObject(usernumber)->PartyNumber = -1;
				getGameObject(usernumber)->PartyTargetUser = -1;
				gParty.UpdatePKPartyPanalty(pnumber);
				this->GCPartyDelUserSend(usernumber);
				CGPartyListAll(pnumber);
			}
			else
			{
				if (!IT_MAP_RANGE(getGameObject(usernumber)->MapNumber) || lpMsg->Number || count <= 2)
				{
					if (IT_MAP_RANGE(getGameObject(usernumber)->MapNumber) && count <= 2)
					{
						gParty.Delete(pnumber, lpMsg->Number);
						count = gParty.GetCount(lpObj.PartyNumber);
						getGameObject(usernumber)->PartyNumber = -1;
						getGameObject(usernumber)->PartyTargetUser = -1;
						gParty.UpdatePKPartyPanalty(pnumber);
						if (!lpMsg->Number)
							gParty.ChangeLeader(pnumber);
						this->GCPartyDelUserSend(usernumber);
						this->CGPartyListAll(pnumber);
					}
					else
					{
						if (!ITL_MAP_RANGE(getGameObject(usernumber)->MapNumber) || lpMsg->Number || count <= 2)
						{
							if (ITL_MAP_RANGE(getGameObject(usernumber)->MapNumber) && count <= 2)
							{
								gParty.Delete(pnumber, lpMsg->Number);
								count = gParty.GetCount(lpObj.PartyNumber);
								getGameObject(usernumber)->PartyNumber = -1;
								getGameObject(usernumber)->PartyTargetUser = -1;
								gParty.UpdatePKPartyPanalty(pnumber);
								if (!lpMsg->Number)
									gParty.ChangeLeader(pnumber);
								this->GCPartyDelUserSend(usernumber);
								this->CGPartyListAll(pnumber);
							}
							else
							{
								for (int index = 0; index < 5; index++)
								{
									number = gParty.m_PartyS[pnumber].Number[index];

									if (number >= 0)
									{
										gParty.Delete(pnumber, index);
										getGameObject(number)->PartyNumber = -1;
										getGameObject(number)->PartyTargetUser = -1;
										if (getGameObject(number)->m_PlayerData->m_bUsePartyMatching == true)
											this->GCPartyDelUserSendNoMessage(number);
										else
											this->GCPartyDelUserSend(number);
									}
								}

								gParty.Destroy(pnumber);
							}
						}
						else
						{
							gParty.Delete(pnumber, lpMsg->Number);
							count = gParty.GetCount(lpObj.PartyNumber);
							getGameObject(usernumber)->PartyNumber = -1;
							getGameObject(usernumber)->PartyTargetUser = -1;
							gParty.UpdatePKPartyPanalty(pnumber);
							gParty.ChangeLeader(pnumber);
							this->GCPartyDelUserSend(usernumber);
							this->CGPartyListAll(pnumber);
							if (!count)
								gParty.Destroy(pnumber);
						}
					}
				}
				else
				{
					gParty.Delete(number, lpMsg->Number);
					count = gParty.GetCount(lpObj.PartyNumber);
					getGameObject(usernumber)->PartyNumber = -1;
					getGameObject(usernumber)->PartyTargetUser = -1;
					gParty.UpdatePKPartyPanalty(pnumber);
					gParty.ChangeLeader(pnumber);
					this->GCPartyDelUserSend(usernumber);
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
	PBMSG_HEAD2 h;	// C1:43
	BYTE btSendMessage;
};


void GameProtocol::GCPartyDelUserSend(CGameObject &Obj)
{
	PMSG_PARTYDELUSERSEND pMsg;

	pMsg.btSendMessage = FALSE;
	PHeadSetB((BYTE*)&pMsg, 0x43, sizeof(pMsg));

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCPartyDelUserSendNoMessage(CGameObject &Obj)
{
	PMSG_PARTYDELUSERSEND pMsg;

	pMsg.btSendMessage = TRUE;
	PHeadSetB((BYTE*)&pMsg, 0x43, sizeof(pMsg));

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}

struct PMSG_GUILDQUESTSEND
{
	PBMSG_HEAD2 h;	// C1:50
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
};

void GameProtocol::CGGuildRequestRecv(PMSG_GUILDJOINQ * lpMsg, CGameObject &Obj)
{
	PMSG_GUILDQUESTSEND pMsg;
	int number = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	if (number < g_ConfigRead.server.GetObjectStartUserIndex() || number > g_ConfigRead.server.GetObjectMax() - 1)
		return;

	if (!PacketCheckTime(aIndex))
		return;

	if (!gObjIsConnected(&getGameObject(number)))
	{
		GCResultSend(aIndex, 0x51, 0x03);
		return;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (g_CastleSiegeSync.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
		{
			this->GCServerMsgStringSend(Lang.GetText(0, 195), aIndex, 1);
			return;
		}
	}

	if (getGameObject(number)->Type != OBJ_USER)
	{
		this->GCSendDisableReconnect(aIndex);
		//IOCP.CloseClient(aIndex);
		return;
	}

	if (lpObj.m_PlayerData->m_bSecurityCheck == false)
	{
		GCResultSend(aIndex, 0x51, 0x06);
		return;
	}

	if (lpObj.m_IfState.use > 0)
	{
		GCResultSend(aIndex, 0x51, 0x06);
		return;
	}

	if (getGameObject(number)->m_IfState.use > 0)
	{
		GCResultSend(aIndex, 0x51, 0x06);
		return;
	}

	if (IT_MAP_RANGE(lpObj.MapNumber))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 292), aIndex, 1);
		return;
	}

	if (g_NewPVP.IsDuel(getGameObject(aIndex]) || g_NewPVP.IsDuel(getGameObject(number)))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 317), aIndex, 1);
		return;
	}

	if (g_NewPVP.IsObserver(getGameObject(aIndex)))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 318), aIndex, 1);
		return;
	}

	if (lpObj.Level < 6)
	{
		GCResultSend(aIndex, 0x51, 0x07);
		return;
	}

	if (lpObj.m_PlayerData->GuildNumber > 0)
	{
		GCResultSend(aIndex, 0x51, 0x05);
		return;
	}

	if ((getGameObject(number)->m_Option & 1) != 1)
	{
		GCResultSend(aIndex, 0x51, 0x00);
		return;
	}

	if (getGameObject(number)->m_PlayerData->GuildNumber > 0)
	{
		if (strcmp(getGameObject(number]->m_PlayerData->lpGuild->Names[0], getGameObject(number)->Name))
		{
			if (getGameObject(number)->m_PlayerData->GuildStatus != 64)
			{
				GCResultSend(aIndex, 0x51, 0x04);
				return;
			}
		}

		if (getGameObject(number)->m_PlayerData->lpGuild->WarState)
		{
			GCResultSend(aIndex, 0x51, 0x06);
			return;
		}

		int MaxGuildMember;

		if (getGameObject(number)->Class == CLASS_DARKLORD)
			MaxGuildMember = (getGameObject(number]->Level + getGameObject(number)->m_PlayerData->MasterLevel) / 10 + getGameObject(number)->Leadership / 10;
		else
			MaxGuildMember = (getGameObject(number]->Level + getGameObject(number)->m_PlayerData->MasterLevel) / 10;

		if (MaxGuildMember > g_ConfigRead.data.common.MaxGuildMember)
			MaxGuildMember = g_ConfigRead.data.common.MaxGuildMember;

		if (getGameObject(number)->m_PlayerData->lpGuild->TotalCount >= MaxGuildMember)
		{
			GCResultSend(aIndex, 0x51, 0x02);
			return;
		}

		int OwnInfluence = g_GensSystem.GetGensInfluence(&getGameObject(aIndex));
		int TarInfluence = g_GensSystem.GetGensInfluence(&getGameObject(number));

		int iArcaBattleState = g_ArcaBattle.GetState();

		if (iArcaBattleState > 2 && iArcaBattleState < 9)
		{
			this->GCResultSend(aIndex, 0x51, 0xA4);
			return;
		}

		if (OwnInfluence <= NONE_INFLUENCE)
		{
			GCResultSend(aIndex, 0x51, 0xA3);
			return;
		}

		if (TarInfluence <= NONE_INFLUENCE)
		{
			GCResultSend(aIndex, 0x51, 0xA1);
			return;
		}

		if (OwnInfluence != TarInfluence && g_GensSystem.CanGensJoinGuildWhileOppositeGens() == FALSE)
		{
			GCResultSend(aIndex, 0x51, 0xA2);
			return;
		}
	}
	else
	{
		GCResultSend(aIndex, 0x51, 0x04);
		return;
	}

	lpObj.m_IfState.use = 1;
	lpObj.m_IfState.type = 4;
	lpObj.m_IfState.state = 0;

	getGameObject(number)->m_IfState.use = 1;
	getGameObject(number)->m_IfState.type = 4;
	getGameObject(number)->m_IfState.state = 0;

	lpObj.TargetNumber = number;
	getGameObject(number)->TargetNumber = aIndex;
	lpObj.m_InterfaceTime = GetTickCount();
	getGameObject(number)->m_InterfaceTime = GetTickCount();

	PHeadSetB((BYTE*)&pMsg, 0x50, sizeof(pMsg));
	pMsg.NumberH = SET_NUMBERH(aIndex);
	pMsg.NumberL = SET_NUMBERL(aIndex);

	IOCP.DataSend(number, (BYTE*)&pMsg, pMsg.h.size);

}



void GameProtocol::CGGuildRequestResultRecv(PMSG_GUILDQRESULT * lpMsg, CGameObject &Obj) //GS-CS Decompiled 100%
{
	int number;
	BYTE result = 0;
	int pnumber = -1;

	if (!PacketCheckTime(aIndex))
		return;

	if (g_CastleSiegeSync.GetCastleState() == 7) //Good
	{
		MsgOutput(aIndex, (Lang.GetText(0, 195)));
		return;
	}

	number = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	if (number < 0 || number > g_ConfigRead.server.GetObjectMax() - 1)
		return;

	if (!gObjIsConnected(&getGameObject(number)))
	{
		GCResultSend(aIndex, 0x51, 0x02);
	}
	else
	{
		result = 1;
	}

	if (lpMsg->Result == 0)
	{
		result = 0;
		GCResultSend(number, 0x51, 0x00);
	}

	if (result == 1)
	{
		if (lpObj.m_PlayerData->GuildNumber > 0)
		{
			GDGuildMemberAdd(number, lpObj.m_PlayerData->lpGuild->Name, getGameObject(number)->Name);
			GDReqCancelJoinGuildMatching(number, lpObj.Name, 1);
		}
	}

	if (lpObj.m_IfState.use && lpObj.m_IfState.type == 4)
		lpObj.m_IfState.use = 0;

	if (getGameObject(number]->m_IfState.use && getGameObject(number)->m_IfState.type == 4)
		getGameObject(number)->m_IfState.use = 0;
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
	if (getGameObject(pnumber)->Type != OBJ_USER)
	{
		return;
	}

	if (getGameObject(pnumber)->m_PlayerData->lpGuild == NULL)
		return;

	PMSG_GUILDLISTCOUNT_EX700 pCount;
	PMSG_GUILDLIST pList;
	char sendbuf[1500];
	int lOfs = 0;
	int number;
	int n;
	int guildmaster = 0;

	PHeadSetW((BYTE*)&pCount, 0x52, 0);
	pCount.Result = 0;
	pCount.Count = 0;
	lOfs += sizeof(pCount);
	pCount.h.sizeH = SET_NUMBERH(lOfs);
	pCount.h.sizeL = SET_NUMBERL(lOfs);

	if (getGameObject(pnumber)->m_PlayerData->GuildNumber < 1)
	{
		IOCP.DataSend(pnumber, (BYTE*)&pCount, lOfs);
		return;
	}

	pCount.Count = getGameObject(pnumber)->m_PlayerData->lpGuild->Count;
	pCount.TotalScore = getGameObject(pnumber)->m_PlayerData->lpGuild->TotalScore;
	pCount.Score = getGameObject(pnumber)->m_PlayerData->lpGuild->PlayScore;
	memset(pCount.szRivalGuild, 0, sizeof(pCount.szRivalGuild));

	if (getGameObject(pnumber)->m_PlayerData->lpGuild->iGuildRival)
	{
		int iGuildCount = 0;
		int iGuildList[MAX_UNION_GUILD] = { 0 };

		if (UnionManager.GetGuildUnionMemberList(getGameObject(pnumber)->m_PlayerData->lpGuild->iGuildRival, iGuildCount, iGuildList) == TRUE)
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
			memcpy(pCount.szRivalGuild[0], getGameObject(pnumber)->m_PlayerData->lpGuild->szGuildRivalName, MAX_GUILD_LEN);
		}
	}

	pCount.Result = 1;
	guildmaster = 1;

	if (pCount.Count == 0)
		return;

	pCount.Count = 0;

	for (n = 0; n < MAX_USER_GUILD; n++)
	{
		number = getGameObject(pnumber]->m_PlayerData->lpGuild->Use[n);

		if (number > 0)
		{
			memset(&pList, 0, sizeof(pList));
			memcpy(pList.Name, getGameObject(pnumber]->m_PlayerData->lpGuild->Names[n), sizeof(pList.Name));
			pList.Number = getGameObject(pnumber]->m_PlayerData->lpGuild->pServer[n);
			pList.ConnectAServer = (short)getGameObject(pnumber]->m_PlayerData->lpGuild->pServer[n) & 0x7F;

			if (getGameObject(pnumber]->m_PlayerData->lpGuild->pServer[n) >= 0)
				pList.ConnectAServer |= 0x80;

			pList.btGuildStatus = getGameObject(pnumber]->m_PlayerData->lpGuild->GuildStatus[n);
			memcpy(&sendbuf[lOfs], &pList, sizeof(pList));
			lOfs += sizeof(pList);
			pCount.Count++;
		}
	}

	pCount.h.sizeH = SET_NUMBERH(lOfs);
	pCount.h.sizeL = SET_NUMBERL(lOfs);
	memcpy(sendbuf, (BYTE*)&pCount, sizeof(pCount));

	IOCP.DataSend(pnumber, (BYTE*)sendbuf, lOfs);
}

struct PMSG_GUILDDELUSER_RESULT
{
	PBMSG_HEAD2 h;	// C1:53
	BYTE Result;	// 3
};

void GameProtocol::CGGuildDelUser(PMSG_GUILDDELUSER * lpMsg, CGameObject &Obj)
{
	if (!PacketCheckTime(aIndex))
		return;

	if (aIndex < g_ConfigRead.server.GetObjectStartUserIndex() || aIndex > g_ConfigRead.server.GetObjectMax() - 1)
		return;

	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (g_CastleSiegeSync.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE) //Good
		{
			MsgOutput(aIndex, Lang.GetText(0, 196));
			return;
		}
	}

	if (lpObj.m_PlayerData->GuildNumber < 1)
		return;

	char memberid[11];
	char guildname[11];
	char joomin[21];
	memset(joomin, 0, sizeof(joomin));
	PMSG_GUILDDELUSER_RESULT pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x53, sizeof(pMsg));
	pMsg.Result = 3;

	if (!g_ConfigRead.data.common.GuildDestroy)
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	int iArcaBattleState = g_ArcaBattle.GetState();

	if (iArcaBattleState > 2 && iArcaBattleState < 9)
	{
		pMsg.Result = 6;

		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (lpObj.m_PlayerData->m_bSecurityCheck == false)
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	memset(memberid, 0, sizeof(memberid));
	memset(guildname, 0, sizeof(guildname));
	memcpy(memberid, lpMsg->Name, MAX_ACCOUNT_LEN);
	strcpy(guildname, lpObj.m_PlayerData->lpGuild->Name);

	if (!strcmp(lpObj.m_PlayerData->lpGuild->Names[0], lpObj.Name))
	{
		memset(joomin, 0, sizeof(joomin));
		memcpy(joomin, lpMsg->Password, sizeof(lpMsg->Password));

		if (gObjPasswordCheck(aIndex, joomin) == FALSE)
		{
			pMsg.Result = 0;
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
			return;
		}

		if (!strcmp(memberid, lpObj.Name))
		{
			if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && g_ConfigRead.data.common.CastleGuildDestroyLimit == TRUE)
			{
				if (strcmp(lpObj.m_PlayerData->lpGuild->Name, g_CastleSiegeSync.GetCastleOwnerGuild()) == 0)
				{
					MsgOutput(aIndex, Lang.GetText(0, 194));
					return;
				}
			}

			gObjGuildWarMasterClose(&getGameObject(aIndex));
			GDGuildDestroySend(aIndex, lpObj.m_PlayerData->lpGuild->Name, lpObj.Name);
			g_ArcaBattle.ReqRemoveRewardGuildBuff(lpObj.m_PlayerData->lpGuild->Name);
			g_ArcaBattle.GDReqMarkRegDel(aIndex, lpObj.m_PlayerData->GuildNumber);
		}
		else
		{
			if (Guild.GetGuildMemberStatus(lpObj.m_PlayerData->lpGuild->Name, memberid) == -1)
			{
				pMsg.Result = 0;
				IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
				return;
			}

			g_ArcaBattle.ReqGuildMemberDelBuffDel(memberid, guildname);
			GDGuildMemberDel(aIndex, guildname, memberid);
		}

	}
	else
	{
		if (strcmp(memberid, lpObj.Name))
		{
			pMsg.Result = 0;
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
			return;
		}

		memset(joomin, 0, sizeof(joomin));
		memcpy(joomin, lpMsg->Password, sizeof(lpMsg->Password));

		if (gObjPasswordCheck(aIndex, joomin) == FALSE)
		{
			MsgOutput(aIndex, Lang.GetText(0, 525));
		}

		else
		{
			g_ArcaBattle.ReqGuildMemberDelBuffDel(&getGameObject(aIndex), guildname);
			GDGuildMemberDel(aIndex, guildname, memberid);
		}

	}
}



void GameProtocol::GCGuildDelUserResult(CGameObject &Obj, BYTE Result)
{
	PMSG_GUILDDELUSER_RESULT pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x53, sizeof(pMsg));
	pMsg.Result = Result;

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}



struct PMSG_GUILDMASTERQUESTION
{
	PBMSG_HEAD2 h;	// C1:54
};

void GameProtocol::GCGuildMasterQuestionSend(CGameObject &Obj)
{
	if (lpObj.m_IfState.use > 0)
	{
		return;
	}

	PMSG_GUILDMASTERQUESTION pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x54, sizeof(pMsg));
	lpObj.m_IfState.use = 1;
	lpObj.m_IfState.type = 5;
	lpObj.m_IfState.state = 0;

	IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);
}



void GameProtocol::CGGuildMasterAnswerRecv(PMSG_GUILDMASTERANSWER * lpMsg, CGameObject &Obj)
{
	if (lpMsg->Result == 1)
	{
		if (!gObjGuildMasterCapacityTest(&getGameObject(aIndex)))
		{
			if (lpObj.m_IfState.use && lpObj.m_IfState.type == 5)
			{
				lpObj.m_IfState.use = 0;
			}

			return;
		}

		GCGuildMasterManagerRun(aIndex);
		return;
	}

	if (lpObj.m_IfState.use && lpObj.m_IfState.type == 5)
	{
		lpObj.m_IfState.use = 0;
	}
}


struct PMSG_GUILDMASTERMANAGER_RUN
{
	PBMSG_HEAD2 h;	// C1:55
};

void GameProtocol::GCGuildMasterManagerRun(CGameObject &Obj)
{
	PMSG_GUILDMASTERMANAGER_RUN pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x55, sizeof(pMsg));

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}



void GameProtocol::CGGuildMasterInfoSave(CGameObject &Obj, PMSG_GUILDINFOSAVE * lpMsg)
{
	char GuildName[MAX_GUILD_LEN + 1];
	GuildName[MAX_GUILD_LEN] = '\0';
	memcpy(GuildName, lpMsg->GuildName, MAX_GUILD_LEN);
	int len = strlen(GuildName);

	if (len <= 2 || len > MAX_GUILD_LEN)
	{
		PMSG_GUILDCREATED_RESULT pMsg;

		PHeadSetB((BYTE*)&pMsg, 0x56, sizeof(pMsg));
		pMsg.Result = 2;

		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

		if (lpObj.m_IfState.use && lpObj.m_IfState.type == 5)
		{
			lpObj.m_IfState.use = 0;
		}

		return;
	}

	if (!g_prohibitedSymbols.Validate(GuildName, len, TYPE_GUILDNAME))
	{
		MsgOutput(aIndex, Lang.GetText(0, 546));
		return;
	}

	GDReqCancelJoinGuildMatching(aIndex, lpObj.Name, 1);
	GDGuildCreateSend(aIndex, GuildName, lpObj.Name, lpMsg->Mark, lpMsg->btGuildType);

	if (lpObj.m_IfState.use && lpObj.m_IfState.type == 5)
	{
		lpObj.m_IfState.use = 0;
	}
}



void GameProtocol::CGGuildMasterCreateCancel(CGameObject &Obj)
{
	if (lpObj.Type != OBJ_USER)
		return;

	if (lpObj.m_PlayerData->GuildNumber > 0)
		return;

	if (lpObj.m_IfState.use && lpObj.m_IfState.type == 5)
	{
		lpObj.m_IfState.use = 0;
	}
}


void GameProtocol::GCGuildViewportNowPaint(CGameObject &Obj, char* guildname, BYTE* mark, BOOL isGuildMaster)
{
	GUILD_INFO_STRUCT * lpGuild = Guild.SearchGuild(guildname);

	if (lpGuild == NULL)
		return;

	
	BYTE _GuildInfoBuf[256] = { 0 };
	int _GuildInfoOfs = 5;

	if (lpObj.m_PlayerData->lpGuild == NULL)
		return;

	PMSG_SIMPLE_GUILDVIEWPORT pMsg;
	pMsg.GuildNumber = lpObj.m_PlayerData->lpGuild->Number;
	pMsg.NumberH = SET_NUMBERH(aIndex) & 0x7F;
	pMsg.NumberL = SET_NUMBERL(aIndex);

	if (isGuildMaster != FALSE)
		pMsg.NumberH |= 0x80;

	if (isGuildMaster != FALSE)
		pMsg.btGuildStatus = 0x80;
	else
		pMsg.btGuildStatus = lpObj.m_PlayerData->GuildStatus;

	pMsg.btGuildType = lpObj.m_PlayerData->lpGuild->btGuildType;
	pMsg.btGuildRelationShip = 0;
	lpObj.m_PlayerData->iGuildUnionTimeStamp = 0;
	memcpy(&_GuildInfoBuf[_GuildInfoOfs], &pMsg, sizeof(pMsg));
	_GuildInfoOfs += sizeof(pMsg);

	PMSG_SIMPLE_GUILDVIEWPORT_COUNT pGVCount;

	pGVCount.h.c = 0xC2;
	pGVCount.h.headcode = 0x65;
	pGVCount.h.sizeH = SET_NUMBERH(_GuildInfoOfs);
	pGVCount.h.sizeL = SET_NUMBERL(_GuildInfoOfs);
	pGVCount.Count = 1;
	memcpy(_GuildInfoBuf, &pGVCount, sizeof(pGVCount));

	if (!CC_MAP_RANGE(lpObj.MapNumber) && lpObj.MapNumber != MAP_INDEX_CHAOSCASTLE_SURVIVAL)
		IOCP.DataSend(lpObj.m_Index, _GuildInfoBuf, _GuildInfoOfs);

	MsgSendV2(lpObj, _GuildInfoBuf, _GuildInfoOfs);
}



struct PMSG_GUILDDEL_VIEWPORT_NOW
{
	PBMSG_HEAD2 h;	// C1:5D
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
};



void GameProtocol::GCGuildViewportDelNow(CGameObject &Obj, BOOL isGuildMaster)
{
	PMSG_GUILDDEL_VIEWPORT_NOW pMsg;

	

	PHeadSetB((BYTE*)&pMsg, 0x5D, sizeof(pMsg));
	pMsg.NumberH = SET_NUMBERH(aIndex) & 0x7F;
	pMsg.NumberL = SET_NUMBERL(aIndex);

	if (isGuildMaster != FALSE)
	{
		pMsg.NumberH |= 0x80;
	}

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
	MsgSendV2(lpObj, (BYTE*)&pMsg, pMsg.h.size);
}



void GameProtocol::GCManagerGuildWarEnd(char * GuildName)
{
	GUILD_INFO_STRUCT * lpNode = Guild.SearchGuild(GuildName);

	if (lpNode == NULL)
	{
		return;
	}

	int n = 0;
	int warmaster = -1;

	while (true)
	{
		if (getGameObject(n)->Type == OBJ_USER)
		{
			if (getGameObject(n)->Connected > PLAYER_LOGGED)
			{
				if (getGameObject(n]->Name[0] == lpNode->Names[0][0))
				{
					if (strcmp(getGameObject(n]->Name, lpNode->Names[0)) == 0)
					{
						warmaster = n;
						break;
					}
				}
			}
		}

		if (n < g_ConfigRead.server.GetObjectMax() - 1)
		{
			n++;
		}
		else
		{
			break;
		}
	}

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

		if (lpNode->WarType == 1)
		{
			gBattleGroundEnable(lpNode->BattleGroundIndex, FALSE);
			gObjGuildWarEndSend(lpNode, lpNode->lpTargetGuildNode, Result1, Result2);
			gObjGuildWarEnd(lpNode, lpNode->lpTargetGuildNode);

			cManager.BattleInfoSend(::GetBattleTeamName(0, 0), -1, ::GetBattleTeamName(0, 1), -1);
		}
		else
		{
			gObjGuildWarEndSend(lpNode, lpNode->lpTargetGuildNode, Result1, Result2);
			gObjGuildWarEnd(lpNode, lpNode->lpTargetGuildNode);
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

	int n = 0;
	int warmaster = -1;

	while (true)
	{
		if (getGameObject(n)->Type == OBJ_USER)
		{
			if (getGameObject(n)->Connected > PLAYER_LOGGED)
			{
				if (getGameObject(n]->Name[0] == lpNode->Names[0][0))
				{
					if (strcmp(getGameObject(n]->Name, lpNode->Names[0)) == 0)
					{
						warmaster = n;
						break;
					}
				}
			}
		}

		if (n < g_ConfigRead.server.GetObjectMax() - 1)
		{
			n++;
		}
		else
		{
			break;
		}
	}

	if (warmaster >= 1)
	{
		this->GCGuildWarRequestResult(GuildName2, warmaster, type);
	}
}


struct PMSG_GUILDWARREQUEST_RESULT
{
	PBMSG_HEAD2 h;	// C1:60
	BYTE Result;	// 3
};

void GameProtocol::GCGuildWarRequestResult(LPSTR GuildName, CGameObject &Obj, int type)
{
	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		return;
	}

	if (lpObj.m_IfState.use > 0 && lpObj.m_IfState.type != 12)
	{
		return;
	}

	PMSG_GUILDWARREQUEST_RESULT pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x60, sizeof(pMsg));
	pMsg.Result = 3;

	if (lpObj.m_PlayerData->GuildNumber < 1)
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	GUILD_INFO_STRUCT * lpMyGuild = lpObj.m_PlayerData->lpGuild;

	if (!lpMyGuild)
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (lpMyGuild->WarState == 1 || lpMyGuild->WarDeclareState == 1)
	{
		pMsg.Result = 4;

		//Error on declaring war : %d %d %s
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

		return;
	}

	if (g_ConfigRead.pk.bPkPenaltyDisable == FALSE && lpObj.m_PK_Level >= 6)
	{
		pMsg.Result = 4;

		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

		return;
	}

	if (strcmp(lpMyGuild->Names[0], lpObj.Name))
	{
		pMsg.Result = 5;

		// Error on declaring war : 0x05 %s %s
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

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
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

			return;
		}

		if (lpMyGuild->iGuildUnion != 0 && lpMyGuild->iGuildUnion == lpNode->iGuildUnion)
		{
			return;
		}

		int n = g_ConfigRead.server.GetObjectStartUserIndex();	// #warning Change this 0 to ObjectStartUserIndex
		int warmaster = -1;

		while (true)
		{
			if (getGameObject(n)->Type == OBJ_USER)
			{
				if (getGameObject(n)->Connected > PLAYER_LOGGED)
				{
					if (getGameObject(n]->Name[0] == lpNode->Names[0][0))
					{
						if (!strcmp(getGameObject(n]->Name, lpNode->Names[0)))
						{
							if (g_ConfigRead.pk.bPkPenaltyDisable == FALSE && getGameObject(n)->m_PK_Level >= 6)
							{
								pMsg.Result = 4;

								IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

								return;
							}

							warmaster = n;
							break;
						}
					}
				}
			}

			if (n < g_ConfigRead.server.GetObjectMax() - 1)
				n++;
			else
				break;
		}

		if (warmaster >= 1)
		{
			if (CC_MAP_RANGE(lpObj.MapNumber) || CC_MAP_RANGE(getGameObject(warmaster)->MapNumber))
			{
				GCServerMsgStringSend(Lang.GetText(0, 117), aIndex, 1);
				return;
			}

			if (lpObj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL || getGameObject(warmaster)->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
			{
				GCServerMsgStringSend(Lang.GetText(0, 575), aIndex, 1);
				return;
			}

			if ((getGameObject(warmaster)->m_Option & 1) != 1)
			{
				pMsg.Result = 4;
				IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

				return;
			}

			pMsg.Result = 1;
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
			GCGuildWarRequestSend(lpMyGuild->Name, warmaster, type);
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
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

			//return;
		}
	}
	else
	{
		pMsg.Result = 0;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

		return;
	}
}


struct PMSG_GUILDWARSEND
{
	PBMSG_HEAD2 h;	// C1:61
	char GuildName[8];	// 3
	BYTE Type;	// B
};

void GameProtocol::GCGuildWarRequestSend(LPSTR GuildName, CGameObject &Obj, int type) //GS-CS OK
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		PMSG_GUILDWARSEND pMsg;

		PHeadSetB((BYTE*)&pMsg, 0x61, sizeof(pMsg));
		pMsg.Type = type;
		memcpy(pMsg.GuildName, GuildName, MAX_GUILD_LEN);

		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

	}
}


struct PMSG_GUILDWAR_DECLARE
{
	PBMSG_HEAD2 h;	// C1:62
	char GuildName[8];	// 3
	BYTE Type;	// B
	BYTE TeamCode;	// C
};


void GameProtocol::GCGuildWarRequestSendRecv(PMSG_GUILDWARSEND_RESULT * lpMsg, CGameObject &Obj)
{
	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		return;
	}

	if (lpObj.Connected != 3)
		return;

	PMSG_GUILDWAR_DECLARE pMsg;
	int count = 0;
	int g_call = 0;

	PHeadSetB((BYTE*)&pMsg, 0x62, sizeof(pMsg));
	pMsg.Type = 0;

	GUILD_INFO_STRUCT * lpMyNode = lpObj.m_PlayerData->lpGuild;

	if (!lpMyNode)
	{
		PMSG_GUILDWARREQUEST_RESULT pResult;

		pResult.h.c = 0xC1;
		pResult.h.headcode = 0x60;
		pResult.h.size = sizeof(pResult);	// #error Change fro pResult
		pResult.Result = 0;

		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (lpMyNode->lpTargetGuildNode && lpMsg->Result)
	{
		int iTarGetIndex = lpMyNode->lpTargetGuildNode->Index[0];

		if (!ObjectMaxRange(iTarGetIndex))
		{
			return;
		}

		if (BC_MAP_RANGE(getGameObject(iTarGetIndex)->MapNumber) ||
			CC_MAP_RANGE(getGameObject(iTarGetIndex)->MapNumber) ||
			DS_MAP_RANGE(getGameObject(iTarGetIndex)->MapNumber) ||
			DG_MAP_RANGE(getGameObject(iTarGetIndex)->MapNumber) ||
			IT_MAP_RANGE(getGameObject(iTarGetIndex)->MapNumber) ||
			IMPERIAL_MAP_RANGE(getGameObject(iTarGetIndex)->MapNumber))
		{
			PMSG_NOTICE pNotice;

			TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 117));

			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pNotice, pNotice.h.size);

			lpMsg->Result = 0;
		}

		if (getGameObject(iTarGetIndex)->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
		{
			PMSG_NOTICE pNotice;

			TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 575));

			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pNotice, pNotice.h.size);

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

				PHeadSetB((BYTE*)&pResult, 0x60, sizeof(pResult));
				pResult.Result = 6;

				if (lpMyNode->lpTargetGuildNode->Index[0] >= 0)
				{
					IOCP.DataSend(lpMyNode->lpTargetGuildNode->Index[0], (BYTE*)&pResult, pResult.h.size);
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

					if (lpMyNode->WarType == 1)
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

							PHeadSetB((BYTE*)&pResult, 0x60, sizeof(pResult));
							pResult.Result = 4;

							IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
							return;
						}

						pTeleportMsg.MoveNumber = 51;
						lpMyNode->lpTargetGuildNode->BattleGroundIndex = lpMyNode->BattleGroundIndex;

						if (gBSGround[0]->m_BallIndex >= 0)
							gObjMonsterRegen(&getGameObject(gBSGround[0]->m_BallIndex));

						BattleSoccerGoalEnd(0);
						lpMyNode->PlayScore = 0;
						lpMyNode->lpTargetGuildNode->PlayScore = 0;
						gBattleGroundEnable(lpMyNode->BattleGroundIndex, TRUE);
						gSetBattleTeamMaster(lpMyNode->BattleGroundIndex, 0, lpMyNode->Name, lpMyNode);
						gSetBattleTeamMaster(lpMyNode->BattleGroundIndex, 1, lpMyNode->lpTargetGuildNode->Name, lpMyNode->lpTargetGuildNode);
					}

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

					for (int n = 0; n < MAX_USER_GUILD; n++)
					{
						if (lpMyNode->Use[n])
						{
							if (lpMyNode->Index[n] >= 0)
							{
								g_call = 0;

								if (n > 0)
								{
									if (lpMyNode->WarType == 1)
									{
										if (getGameObject(lpMyNode->Index[0])->PartyNumber >= 0)
										{
											if (getGameObject(lpMyNode->Index[0]]->PartyNumber == getGameObject(lpMyNode->Index[n))->PartyNumber)
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
									IOCP.DataSend(lpMyNode->Index[n], (BYTE*)&pMsg, pMsg.h.size);

									// Declared guild war against (%s)
									GCGuildWarScore(lpMyNode->Index[n]);
									int x = 60;

									if (lpMyNode->WarType == 1)
									{
										getGameObject(lpMyNode->Index[n])->IsInBattleGround = true;
										pTeleportMsg.MoveNumber = 51;

										if (g_ConfigRead.pk.bPkPenaltyDisable || getGameObject(lpMyNode->Index[n])->m_PK_Level < 6)
										{
											gObjTeleport(lpMyNode->Index[n], 6, x++, 153);
											count++;
										}
									}
								}
							}
						}
					}

					memset(pMsg.GuildName, 0, sizeof(pMsg.GuildName));
					memcpy(pMsg.GuildName, lpMyNode->Name, sizeof(pMsg.GuildName));

					pMsg.TeamCode = lpMyNode->lpTargetGuildNode->BattleTeamCode;
					count = 0;

					for (int n = 0; n < MAX_USER_GUILD; n++)
					{
						if (lpMyNode->lpTargetGuildNode->Use[n])
						{
							if (lpMyNode->lpTargetGuildNode->Index[n] >= 0)
							{
								g_call = 0;

								if (n > 0)
								{
									if (lpMyNode->WarType == 1)
									{
										if (getGameObject(lpMyNode->lpTargetGuildNode->Index[0])->PartyNumber >= 0)
										{
											if (getGameObject(lpMyNode->lpTargetGuildNode->Index[0]]->PartyNumber == getGameObject(lpMyNode->lpTargetGuildNode->Index[n))->PartyNumber)
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
									IOCP.DataSend(lpMyNode->lpTargetGuildNode->Index[n], (BYTE*)&pMsg, pMsg.h.size);

									// Declared guild war against (%s)
									GCGuildWarScore(lpMyNode->lpTargetGuildNode->Index[n]);
									int x = 59;

									if (lpMyNode->lpTargetGuildNode->WarType == 1)
									{

										if (g_ConfigRead.pk.bPkPenaltyDisable != 0 || getGameObject(lpMyNode->lpTargetGuildNode->Index[n])->m_PK_Level < 6)
										{
											getGameObject(lpMyNode->lpTargetGuildNode->Index[n])->IsInBattleGround = true;
											gObjTeleport(lpMyNode->lpTargetGuildNode->Index[n], 6, x++, 164);
											count++;
										}
									}
								}
							}
						}
					}

					cManager.BattleInfoSend(GetBattleTeamName(0, 0),
						GetBattleTeamScore(0, 0),
						GetBattleTeamName(0, 1),
						GetBattleTeamScore(0, 1));

					if (lpMyNode->WarType == 1)
					{
						gObjAddMsgSendDelay(&getGameObject(aIndex), 5, aIndex, 10000, 0);
						GCServerMsgStringSendGuild(lpMyNode->lpTargetGuildNode, Lang.GetText(0, 62), 1);
						GCServerMsgStringSendGuild(lpMyNode, Lang.GetText(0, 62), 1);
					}
				}
			}
		}
	}
}



void GameProtocol::GCGuildWarDeclare(CGameObject &Obj, LPSTR _guildname)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		PMSG_GUILDWAR_DECLARE pMsg;

		PHeadSetB((BYTE*)&pMsg, 0x62, sizeof(pMsg));
		memcpy(pMsg.GuildName, _guildname, sizeof(pMsg.GuildName));

		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
	}
}


struct PMSG_GUILDWAR_END
{
	PBMSG_HEAD2 h;	// C1:63
	BYTE Result;	// 3
	char GuildName[8];	// 4
};

void GameProtocol::GCGuildWarEnd(CGameObject &Obj, BYTE result, char* _guildname)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		PMSG_GUILDWAR_END pMsg;

		PHeadSetB((BYTE*)&pMsg, 0x63, sizeof(pMsg));
		pMsg.Result = result;
		memcpy(pMsg.GuildName, _guildname, sizeof(pMsg.GuildName));

		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
	}
}


struct PMSG_GUILDSCORE
{
	PBMSG_HEAD2 h;	// C1:64
	BYTE Score1;	// 3
	BYTE Score2;	// 4
	BYTE Type;	// 5
};


void GameProtocol::GCGuildWarScore(CGameObject &Obj)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		if (lpObj.Type != OBJ_USER)
			return;

		if (lpObj.m_PlayerData->GuildNumber < 1)
			return;

		PMSG_GUILDSCORE pMsg;

		PHeadSetB((BYTE*)&pMsg, 0x64, sizeof(pMsg));
		pMsg.Score1 = 0;
		pMsg.Score2 = 0;
		pMsg.Type = 0;

		if (lpObj.m_PlayerData->lpGuild != NULL)
		{
			pMsg.Score1 = lpObj.m_PlayerData->lpGuild->PlayScore;
		}

		if (lpObj.m_PlayerData->lpGuild->lpTargetGuildNode != NULL)
		{
			pMsg.Score2 = lpObj.m_PlayerData->lpGuild->lpTargetGuildNode->PlayScore;
		}

		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
	}
}



void GameProtocol::CGWarehouseMoneyInOut(CGameObject &Obj, PMSG_WAREHOUSEMONEYINOUT* lpMsg)
{
	int money = 0;

	if (!gObjIsConnected(aIndex))
	{
		GCWarehouseInventoryMoneySend(aIndex, 0, 0, 0);
		return;
	}

	
	money = lpMsg->Money;

	if (lpObj.m_IfState.state == 0)
	{
		GCWarehouseInventoryMoneySend(aIndex, 0, 0, 0);
		return;
	}

	if (lpObj.m_IfState.use < 1 || lpObj.m_IfState.type != 6)
	{
		GCWarehouseInventoryMoneySend(aIndex, 0, 0, 0);

		return;
	}

	switch (lpMsg->Type)
	{
	case 0x00:
		if (money <= 0 || money > MAX_WAREHOUSE_ZEN)
		{
			GCWarehouseInventoryMoneySend(aIndex, 0, 0, 0);
			return;
		}

		if ((lpObj.WarehouseMoney + money) > MAX_WAREHOUSE_ZEN)
		{
			GCWarehouseInventoryMoneySend(aIndex, 0, 0, 0);
			return;
		}

		if (money > lpObj.m_PlayerData->Money)
		{
			GCWarehouseInventoryMoneySend(aIndex, 0, 0, 0);
			return;
		}

		lpObj.m_PlayerData->Money -= money;
		lpObj.WarehouseMoney += money;
		break;
	case 0x01:
	{
		if (bCanWarehouseLock == TRUE)
		{
			if (lpObj.WarehouseLock == 1)
			{
				GCWarehouseInventoryMoneySend(aIndex, 0, 0, 0);
				GCServerMsgStringSend(Lang.GetText(0, 155), lpObj.m_Index, 1);
				return;
			}
		}

		if (money <= 0 || money > MAX_WAREHOUSE_ZEN)
		{
			GCWarehouseInventoryMoneySend(aIndex, 0, 0, 0);
			return;
		}

		if (money > lpObj.WarehouseMoney)
		{
			GCWarehouseInventoryMoneySend(aIndex, 0, 0, 0);
			return;
		}

		lpObj.WarehouseMoney -= money;
		lpObj.m_PlayerData->Money += money;
		int rZen = GetWarehouseUsedHowMuch(lpObj.Level, lpObj.WarehousePW);

		if ((lpObj.m_PlayerData->Money - rZen) > 0)
		{
			int oldmoney = lpObj.m_PlayerData->Money;
			lpObj.m_PlayerData->Money -= rZen;
			GCMoneySend(lpObj.m_Index, lpObj.m_PlayerData->Money);

		}
		else if ((lpObj.WarehouseMoney - rZen) > 0)
		{
			int oldmoney = lpObj.WarehouseMoney;
			lpObj.WarehouseMoney -= rZen;

			GCWarehouseInventoryMoneySend(aIndex, 1, lpObj.m_PlayerData->Money, lpObj.WarehouseMoney);
		}
		else
		{
			lpObj.WarehouseMoney += money;
			lpObj.m_PlayerData->Money -= money;
			GCWarehouseInventoryMoneySend(aIndex, 0, 0, 0);
			char msg[255];

			wsprintf(msg, Lang.GetText(0, 154), rZen);
			GCServerMsgStringSend(msg, lpObj.m_Index, 1);
			return;
		}
	}
	break;

	default:
		GCWarehouseInventoryMoneySend(aIndex, 0, 0, 0);
		return;
	}

	lpObj.WarehouseCount++;
	GCWarehouseInventoryMoneySend(aIndex, 1, lpObj.m_PlayerData->Money, lpObj.WarehouseMoney);
}

void GameProtocol::GCWarehouseInventoryMoneySend(CGameObject &Obj, BYTE result, int money, int wmoney)
{
	PMSG_MONEY pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x81, sizeof(pMsg));
	pMsg.Result = result;
	pMsg.iMoney = money;
	pMsg.wMoney = wmoney;

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}



void GameProtocol::CGWarehouseUseEnd(CGameObject &Obj)
{
	if (!gObjIsConnectedGP(aIndex))
	{
		return;
	}

	

	PMSG_DEFAULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x82, sizeof(pMsg));
	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
	bool bCheck = false;
	Check_SameSerialItem(aIndex, 2, bCheck);

	if (lpObj.m_btOpenWarehouse == FALSE)
	{
		AntiHackLog->Output("[%s][%s] Attempt Multi-Ware dupe", lpObj.AccountID, lpObj.Name);
		return;
	}

	gObjItemTextSave(lpObj);
	GDSetWarehouseList(aIndex, TRUE);
	GDUserItemSave(lpObj);
	g_PentagramSystem.DBREQ_SetPentagramJewel(aIndex, 0);
	g_PentagramSystem.DBREQ_SetPentagramJewel(aIndex, 1);
}



struct PMSG_WAREHOUSESTATE
{
	PBMSG_HEAD2 h;	// C1:83
	BYTE State;	// 3
};


void GameProtocol::GCWarehouseStateSend(CGameObject &Obj, BYTE state)
{
	PMSG_WAREHOUSESTATE pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x83, sizeof(pMsg));
	pMsg.State = state;

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}



void GameProtocol::GCWarehouseRecivePassword(CGameObject &Obj, PMSG_WAREHOUSEPASSSEND* lpMsg)
{
	if (bCanWarehouseLock == FALSE)
		return;

	int pw = lpMsg->Pass;

	switch (lpMsg->Type)
	{
	case 0x00:
		if (lpObj.WarehouseLock)
		{
			if (lpObj.WarehousePW == pw)
			{
				lpObj.WarehouseLock = FALSE;
				GCWarehouseStateSend(aIndex, 0x0C);
			}
			else
			{
				GCWarehouseStateSend(aIndex, 0x0A);
			}
		}
		break;
	case 0x01:
		if (!lpObj.WarehouseLock || lpObj.WarehouseLock == 0xFF)
		{
			char szJoomin[21];

			memset(szJoomin, 0, sizeof(szJoomin));
			memcpy(szJoomin, lpMsg->LastJoominNumber, sizeof(lpMsg->LastJoominNumber));

			if (gObjPasswordCheck(aIndex, szJoomin) == FALSE)
			{
				GCWarehouseStateSend(aIndex, 0x0D);
				return;
			}

			lpObj.WarehousePW = pw;
			lpObj.WarehouseLock = FALSE;
			GCWarehouseStateSend(aIndex, 0x0C);
		}
		else
		{
			GCWarehouseStateSend(aIndex, 0x0B);
		}
		break;
	case 0x02:
	{
		char szJoomin[21];

		memset(szJoomin, 0, sizeof(szJoomin));
		memcpy(szJoomin, lpMsg->LastJoominNumber, sizeof(lpMsg->LastJoominNumber));

		pw = atoi(szJoomin);

		if (gObjPasswordCheck(aIndex, szJoomin) == FALSE)
		{
			GCWarehouseStateSend(aIndex, 0x0D);
			return;
		}

		lpObj.WarehouseLock = FALSE;
		lpObj.WarehousePW = 0;
		GCWarehouseStateSend(aIndex, lpObj.WarehouseLock);
	}
	break;
	}
}



void GameProtocol::GCUserChaosBoxSend(CGameObject &Obj, int iChaosBoxType)
{
	if (iChaosBoxType == 1)
	{
		if (lpObj.m_IfState.type != 13)
		{
			return;
		}

		if (lpObj.m_IfState.type == 13 && lpObj.m_IfState.state == 1)
		{
			return;
		}
	}
	else
	{
		if (lpObj.m_IfState.type != 7)
		{
			return;
		}

		if (lpObj.m_IfState.type == 7 && lpObj.m_IfState.state == 1)
		{
			return;
		}
	}

	PMSG_SHOPITEMCOUNT pMsg;
	BYTE SendByte[2048];
	int lOfs = sizeof(pMsg);
	PHeadSetW((BYTE*)&pMsg, 0x31, 0);
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
		if (lpObj.pChaosBox[n].IsItem() == TRUE)
		{
			SendByte[lOfs] = n;
			lOfs++;
			ItemByteConvert(&SendByte[lOfs], lpObj.pChaosBox[n]);
			lOfs += MAX_ITEM_INFO;
			pMsg.count++;
		}
	}

	pMsg.h.sizeH = SET_NUMBERH(lOfs);
	pMsg.h.sizeL = SET_NUMBERL(lOfs);
	memcpy(SendByte, &pMsg, sizeof(pMsg));

	IOCP.DataSend(lpObj.m_PlayerData->IDNumber, SendByte, lOfs);
}



void GameProtocol::CGChaosBoxItemMixButtonClick(PMSG_CHAOSMIX* aRecv, CGameObject &Obj)
{

	if (ObjectMaxRange(aIndex) == false)
	{
		return;
	}

	if (!gObjIsConnectedGP(aIndex))
	{
		return;
	}

	

	if (lpObj.ChaosLock == TRUE)
	{
		GCServerMsgStringSend(Lang.GetText(0, 11), lpObj.m_Index, 1);
		return;
	}

	if (lpObj.m_PlayerData->m_bSecurityCheck == false)
	{
		return;
	}

	if (lpObj.m_bPShopOpen == true)
	{
		GCServerMsgStringSend(Lang.GetText(0, 112), lpObj.m_Index, 1);
		return;
	}

	//season4 add-on start
	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
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
			if (lpObj.pChaosBox[n].IsItem() == TRUE)
			{
				if (lpObj.pChaosBox[n].m_Level == 9)
				{
					pos = n;
					validitemcount++;
				}
				else if (lpObj.pChaosBox[n].m_Level == 10)
				{
					pos = n;
					validitemcount++;
				}
				else if (lpObj.pChaosBox[n].m_Level == 11)
				{
					pos = n;
					validitemcount++;
				}
				else if (lpObj.pChaosBox[n].m_Level == 12)
				{
					pos = n;
					validitemcount++;
				}
				else if (lpObj.pChaosBox[n].m_Level == 13) //new
				{
					pos = n;
					validitemcount++;
				}
				else if (lpObj.pChaosBox[n].m_Level == 14)
				{
					pos = n;
					validitemcount++;
				}
			}
		}

		if (validitemcount != 1)
		{
			pMsg.Result = 0;
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
			return;
		}

		int iItemWidth = 0;
		int iItemHeight = 0;

		lpObj.pChaosBox[pos].GetSize(iItemWidth, iItemHeight);

		if (!CheckInventoryEmptySpace(lpObj, iItemHeight, iItemWidth))
		{
			pMsg.Result = 0;
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
			return;
		}

		if (g_LuckyItemManager.IsLuckyItemEquipment(lpObj.pChaosBox[pos].m_Type) == TRUE)
		{
			pMsg.Result = 7;
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
			return;
		}
	}
	//season4 add-on end

	lpObj.bIsChaosMixCompleted = true;
	int iMixType = aRecv->Type;

	//if (iMixType >= CHAOS_TYPE_BLOODANGEL_ITEM_MIX_START && iMixType <= CHAOS_TYPE_BLOODANGEL_ITEM_MIX_END)
	//{
	//	g_MixSystem.BloodAngelUnSealMix(lpObj, iMixType);
	//g_MixSystem.AncientSoulItemMix(lpObj);
	//}
	//else
	if (iMixType >= 0xC8 && iMixType <= 0xCF)
		g_MixSystem.DarkAngelWeaponMix(lpObj, iMixType);
	else
		if (iMixType >= 0x88 && iMixType <= 0xA7)
			g_MixSystem.DarkAngelMix(lpObj, iMixType);
		else
			if (iMixType >= 0xA8 && iMixType <= 0xC7)
				g_MixSystem.DarkAngelNextMix(lpObj, iMixType);
			else
			{

				switch (iMixType)	// Season X Main ENG
				{
				case CHAOS_TYPE_DEVILSQUARE:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.DevilSquareItemChaosMix(lpObj);
					}

					else
					{
						g_MixSystem.DevilSquareItemChaosMix_Multi(lpObj, aRecv->MixCount);
					}
					break;
				case 0x86:
					g_MixSystem.ArchangelHammerMix(lpObj);
					break;
				case 0x87:
					g_MixSystem.BlessedAAMix(lpObj);
					break;
				case 0x40:
					g_MixSystem.DarkAngelWeaponMix(lpObj, iMixType);
					break;
				case CHAOS_TYPE_UPGRADE_10:
					g_MixSystem.PlusItemLevelChaosMix(lpObj, 3);
					break;
				case CHAOS_TYPE_UPGRADE_11:
					g_MixSystem.PlusItemLevelChaosMix(lpObj, 4);
					break;
				case CHAOS_TYPE_UPGRADE_12:
					g_MixSystem.PlusItemLevelChaosMix(lpObj, 22);
					break;
				case CHAOS_TYPE_UPGRADE_13:
					g_MixSystem.PlusItemLevelChaosMix(lpObj, 23);
					break;
				case CHAOS_TYPE_UPGRADE_14:
					g_MixSystem.PlusItemLevelChaosMix(lpObj, 49);
					break;
				case CHAOS_TYPE_UPGRADE_15:
					g_MixSystem.PlusItemLevelChaosMix(lpObj, 50);
					break;
				case CHAOS_TYPE_DINORANT:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.PegasiaChaosMix(lpObj);
					}

					else
					{
						g_MixSystem.PegasiaChaosMix_Multi(lpObj, aRecv->MixCount);
					}
					break;
				case CHAOS_TYPE_FRUIT:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.CircleChaosMix(lpObj);
					}

					else
					{
						g_MixSystem.CircleChaosMix_Multi(lpObj, aRecv->MixCount);
					}
					break;
				case CHAOS_TYPE_SECOND_WING:
				case CHAOS_TYPE_CLOAK:
					g_MixSystem.WingChaosMix(lpObj);
					break;
				case CHAOS_TYPE_BLOODCATLE:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.BloodCastleMix(lpObj);
					}
					else
					{
						g_MixSystem.BloodCastleItemChaosMix_Multi(lpObj, aRecv->MixCount);
					}
					break;
				case CHAOS_TYPE_DEFAULT:
				case CHAOS_TYPE_FIRST_WING:
					g_MixSystem.DefaultChaosMix(lpObj);
					break;
				case CHAOS_TYPE_SETITEM:	// #warning mix domyslnie wylaczony w s2 i wyzej
					g_MixSystem.SetItemChaosMix(lpObj);
					break;
				case CHAOS_TYPE_DARKHORSE:
					g_MixSystem.DarkHorseChaosMix(lpObj);
					break;
				case CHAOS_TYPE_DARKSPIRIT:
					g_MixSystem.DarkSpiritChaosMix(lpObj);
					break;
				case CHAOS_TYPE_BLESS_POTION:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.BlessPotionChaosMix(lpObj);
					}

					else
					{
						g_MixSystem.BlessPotionChaosMix_Multi(lpObj, aRecv->MixCount);
					}
					break;
				case CHAOS_TYPE_SOUL_POTION:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.SoulPotionChaosMix(lpObj);
					}

					else
					{
						g_MixSystem.SoulPotionChaosMix_Multi(lpObj, aRecv->MixCount);
					}
					break;

				case CHAOS_TYPE_LIFE_STONE:
					g_MixSystem.LifeStoneChaosMix(lpObj);
					break;
				case CHAOS_TYPE_CASTLE_SPECIAL_ITEM_MIX:
					g_MixSystem.CastleSpecialItemMix(lpObj);
					break;
				case CHAOS_TYPE_HT_BOX:
					g_MixSystem.HiddenTreasureBoxItemMix(lpObj);
					break;
				case CHAOS_TYPE_FENRIR_01:
					g_MixSystem.Fenrir_01Level_Mix(lpObj);
					break;
				case CHAOS_TYPE_FENRIR_02:
					g_MixSystem.Fenrir_02Level_Mix(lpObj);
					break;
				case CHAOS_TYPE_FENRIR_03:
					g_MixSystem.Fenrir_03Level_Mix(lpObj);
					break;
				case CHAOS_TYPE_FENRIR_04:
					g_MixSystem.Fenrir_04Upgrade_Mix(lpObj);
					break;
				case CHAOS_TYPE_COMPOUNDPOTION_LV1:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.ShieldPotionLv1_Mix(lpObj);
					}

					else
					{
						g_MixSystem.ShieldPotionLv1_Mix_Multi(lpObj, aRecv->MixCount);
					}
					break;
				case CHAOS_TYPE_COMPOUNTPOTION_LV2:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.ShieldPotionLv2_Mix(lpObj);
					}

					else
					{
						g_MixSystem.ShieldPotionLv2_Mix_Multi(lpObj, aRecv->MixCount);
					}
					break;
				case CHAOS_TYPE_COMPOUNTPOTION_LV3:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.ShieldPotionLv3_Mix(lpObj);
					}

					else
					{
						g_MixSystem.ShieldPotionLv3_Mix_Multi(lpObj, aRecv->MixCount);
					}
					break;
				case CHAOS_TYPE_JEWELOFHARMONY_PURITY:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.JewelOfHarmonyItemPurityMix(lpObj);
					}
					else
					{
						g_MixSystem.PurityJewelOfHarmony_MultiMix(lpObj, aRecv->MixCount);
					}
					break;
				case CHAOS_TYPE_JEWELOFHARMONY_MIX_SMELTINGITEM:
					if (aRecv->MixCount == 1)
					{
						g_kJewelOfHarmonySystem.MakeSmeltingStoneItem(lpObj);
					}
					else
					{
						g_kJewelOfHarmonySystem.MakeSmeltingStoneItem_MultiMix(lpObj, aRecv->MixCount);
					}
					break;
				case CHAOS_TYPE_JEWELOFHARMONY_RESTORE_ITEM:
					g_kJewelOfHarmonySystem.RestoreStrengthenItem(lpObj);
					break;
				case CHAOS_TYPE_380_OPTIONITEM:
					g_kItemSystemFor380.ChaosMix380ItemOption(lpObj);
					break;
				case CHAOS_TYPE_LOTTERY_MIX:
					g_MixSystem.LotteryItemMix(lpObj, 0);
					break;
				case CHAOS_TYPE_OLD_PAPER:
					g_MixSystem.IllusionTempleItemChaosMix(lpObj);
					break;
				case CHAOS_TYPE_CONDOR_FEATHER:
					g_MixSystem.FeatherOfCondorMix(lpObj);
					break;
				case CHAOS_TYPE_THIRD_WING:
					g_MixSystem.ThirdWingLevel2ChaosMix(lpObj);
					break;
				case CHAOS_TYPE_BLOSSOM_MIX:
					g_MixSystem.CherryBlossomMix(lpObj);
					break;
				case CHAOS_TYPE_SEED_EXTRACT:
					g_MixSystem.SeedExtractMix(lpObj);
					break;
				case CHAOS_TYPE_SEED_SPHERE_COMPOSITE:
					g_MixSystem.SeedSphereCompositeMix(lpObj);
					break;
				case CHAOS_TYPE_SET_SEED_SPHERE:
					g_MixSystem.SetSeedSphereMix(lpObj, aRecv->Pos);
					break;
				case CHAOS_TYPE_SEED_SPHERE_REMOVE:
					g_MixSystem.SeedSphereRemoveMix(lpObj, aRecv->Pos);
					break;
				case CHAOS_TYPE_SECROMICON:
					g_MixSystem.SecromiconMix(lpObj);
					break;
				case CHAOS_TYPE_PREMIUM_BOX:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.PremiumBoxMix(lpObj);
					}

					else
					{
						g_MixSystem.PremiumBoxMix_Multi(lpObj, aRecv->MixCount);
					}
					break;
				case CHAOS_TYPE_SUMMONS_MIX:
					g_MixSystem.SummonsMix(lpObj, 0);
					break;
				case CHAOS_TYPE_LUCKYITEM_EXCHANGE:
					g_LuckyItemManager.LuckyItemTicketExchange(lpObj);
					break;
				case CHAOS_TYPE_LUCKYITEM_REFINE:
					g_LuckyItemManager.LuckyItemSmelting(lpObj);
					break;
				case CHAOS_TYPE_SOCKETWEAPON:
					g_MixSystem.ItemRefineMix(lpObj);
					break;
				case CHAOS_TYPE_ARCABATTLE_BOOTYMIX:
					g_ArcaBattle.BootyExchange(lpObj);
					break;
				case CHAOS_TYPE_ARCABATTLE_SIGNOFLORD:
					g_ArcaBattle.CGReqMarkRegButtonClick(lpObj);
					break;
				case CHAOS_TYPE_MONSTERWING:
					g_MixSystem.MonsterWingMix(lpObj);
					break;
				case CHAOS_TYPE_CCF_REWARD:
					g_MixSystem.CCFRewardMix(lpObj);
					break;
				case CHAOS_TYPE_DSF_REWARD:
					g_MixSystem.DSFRewardMix(lpObj);
					break;

				case CHAOS_TYPE_ANCIENT_SOUL_ITEM:
					g_MixSystem.AncientSoulItemMix(lpObj);
					break;
				case CHAOS_TYPE_ADVANCED_WING:
					g_MixSystem.AdvancedWingMix(lpObj);
					break;
				case 78:
					g_MixSystem.bloodangelmix78(lpObj);
					break;
				case 79:
					g_MixSystem.bloodangelmix79(lpObj);
					break;
				case 80:
					g_MixSystem.bloodangelmix80(lpObj);
					break;
				case 81:
					g_MixSystem.bloodangelmix81(lpObj);
					break;
				case 86:
					g_MixSystem.bloodangelmix86(lpObj);
					break;
				case 87:
					g_MixSystem.bloodangelmix87(lpObj);
					break;
				case 88:
					g_MixSystem.bloodangelmix88(lpObj);
					break;
				case 89:
					g_MixSystem.bloodangelmix89(lpObj);
					break;
				case 90:
					g_MixSystem.bloodangelmix90(lpObj);
					break;
				case 91:
					g_MixSystem.bloodangelmix91(lpObj);
					break;
				case 92:
					g_MixSystem.bloodangelmix92(lpObj);
					break;
				case 93:
					g_MixSystem.bloodangelmix93(lpObj);
					break;
				case 98:
					g_MixSystem.bloodangelmix98(lpObj);
					break;
				case 99:
					g_MixSystem.bloodangelmix99(lpObj);
					break;
				case 100:
					g_MixSystem.bloodangelmix100(lpObj);
					break;
				case 101:
					g_MixSystem.bloodangelmix101(lpObj);
					break;
				case 106:
					g_MixSystem.bloodangelmix106(lpObj);
					break;
				case 107:
					g_MixSystem.bloodangelmix107(lpObj);
					break;
				case 108:
					g_MixSystem.bloodangelmix108(lpObj);
					break;
				case 109:
					g_MixSystem.bloodangelmix109(lpObj);
					break;
				case 114:
					g_MixSystem.bloodangelmix114(lpObj);
					break;
				case 115:
					g_MixSystem.bloodangelmix115(lpObj);
					break;
				case 116:
					g_MixSystem.bloodangelmix116(lpObj);
					break;
				case 117:
					g_MixSystem.bloodangelmix117(lpObj);
					break;
				case 118:
					g_MixSystem.bloodangelmix118(lpObj);
					break;
				case 119:
					g_MixSystem.bloodangelmix119(lpObj);
					break;
				case 120:
					g_MixSystem.bloodangelmix120(lpObj);
					break;
				case 121:
					g_MixSystem.bloodangelmix121(lpObj);
					break;
				case 126:
					g_MixSystem.bloodangelmix126(lpObj);
					break;
				case 127:
					g_MixSystem.bloodangelmix127(lpObj);
					break;
				case 128:
					g_MixSystem.bloodangelmix128(lpObj);
					break;
				case 129:
					g_MixSystem.bloodangelmix129(lpObj);
					break;
					//se 0x8F..0xA9: //Armor DA 
					//	g_MixSystem.DarkAngelMix(lpObj, iMixType);
					//	break;
				default:
					sLog->outBasic("[%s][%s] Undefine chaosmix type detect %d", lpObj.AccountID, lpObj.Name, iMixType);
					break;
				}

			}
}



void GameProtocol::CGChaosBoxUseEnd(CGameObject &Obj)
{
	if (!gObjIsConnectedGP(aIndex))
	{
		return;
	}

	

	PMSG_DEFAULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x87, sizeof(pMsg));
	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

	if (lpObj.m_IfState.use && lpObj.m_IfState.type == 7)
	{
		lpObj.m_IfState.use = 0;
		lpObj.m_IfState.state = 0;
	}

	g_MixSystem.ChaosBoxInit(lpObj);
	g_PentagramMixSystem.PentagramMixBoxInit(lpObj);
	gObjInventoryCommit(lpObj.m_Index);
	lpObj.m_bIsCastleNPCUpgradeCompleted = false;
}





void GameProtocol::PMoveProc(PMSG_MOVE* lpMove, CGameObject &Obj)
{
	if (!ObjectMaxRange(aIndex))
	{
		return;
	}

	if (lpObj.m_bOffLevel == true)
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
	

	if (lpObj.Connected != PLAYER_PLAYING)
	{
		return; // for reconnect incase player is using MuBot
	}
	if (MAX_MAP_RANGE(lpObj.MapNumber) == FALSE)
	{
		return;
	}

	if (lpObj.Type == OBJ_USER && (g_ConfigRead.MovingHackCheck == TRUE || g_ConfigRead.MovingSpeedCheck == TRUE))
	{
		short ix = abs(lpObj.X - lpMove->X);
		short iy = abs(lpObj.Y - lpMove->Y);

		if (g_ConfigRead.MovingSpeedCheck == TRUE)
		{
			lpObj.m_PlayerData->MovingDistance += (ix + iy);
		}

		if (g_ConfigRead.MovingHackCheck == TRUE)
		{
			if (ix > g_ConfigRead.MovingHackDistance || iy > g_ConfigRead.MovingHackDistance)
			{
				gObjSetPosition(aIndex, lpObj.X, lpObj.Y);
				return;
			}
		}
	}

	if (lpObj.RegenOk > 0)
		return;

	if (lpObj.m_bMapSvrMoveReq == true)
		return;

	if (lpObj.m_bMapSvrMoveReq_2 == true)
		return;

	if (lpObj.m_SkillInfo.LordSummonTime)
	{
		lpObj.m_SkillInfo.LordSummonTime = 0;
		char msg[255];
		wsprintf(msg, Lang.GetText(0, 146));
		GCServerMsgStringSend(msg, lpObj.m_Index, 1);
	}

	if ((GetTickCount() - lpObj.m_LastMoveTime) < 100)
		return;

	if (lpObj.Teleport)
		return;

	if (gObjCheckUsedBuffEffect(&getGameObject(aIndex], BUFFTYPE_STONE) || gObjCheckUsedBuffEffect(&getGameObject(aIndex), BUFFTYPE_STUN) || gObjCheckUsedBuffEffect(&getGameObject(aIndex), BUFFTYPE_SLEEP)
		|| gObjCheckUsedBuffEffect(&getGameObject(aIndex], BUFFTYPE_FREEZE_2) || gObjCheckUsedBuffEffect(&getGameObject(aIndex), BUFFTYPE_EARTH_BINDS))
	{
		gObjSetPosition(lpObj.m_Index, lpObj.X, lpObj.Y);
		return;
	}

	if (lpObj.Type == OBJ_USER && gObjCheckMoveArea(aIndex, lpMove->X, lpMove->Y) == false)
	{
		gObjSetPosition(lpObj.m_Index, lpObj.X, lpObj.Y);

		PMSG_TELEPORT_RESULT pTeleportResult;

		pTeleportResult.h.c = 0xC3;
		pTeleportResult.h.size = sizeof(pTeleportResult);
		pTeleportResult.h.headcode = 0x1C;
		pTeleportResult.Type = 0;
		pTeleportResult.Unk = 0;
		pTeleportResult.MapNumber = lpObj.MapNumber;
		pTeleportResult.Unk1 = 0;
		pTeleportResult.MapX = lpObj.X;
		pTeleportResult.MapY = lpObj.Y;
		pTeleportResult.Dir = lpObj.Dir;

		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pTeleportResult, pTeleportResult.h.size);
		return;
	}

	lpObj.m_LastMoveTime = GetTickCount();
	lpObj.m_Rest = 0;
	lpObj.PathCur = 0;
	lpObj.Dir = lpMove->Path[0] >> 4;
	lpObj.PathCount = lpMove->Path[0] & 0x0F;

	if (lpObj.PathCount > 15)
	{
		return;
	}

	for (n = 0; n < 15; n++)
	{
		lpObj.PathX[n] = 0;
		lpObj.PathY[n] = 0;
		lpObj.PathOri[n] = 0;
	}

	sx = lpMove->X;
	sy = lpMove->Y;

	if (DG_MAP_RANGE(lpObj.MapNumber) == TRUE)
	{
		if (g_DoppelGanger.GetDoppelgangerState() == 1 && g_DoppelGanger.CheckMapTile(lpObj.MapNumber, lpObj.X, lpObj.Y) == FALSE)
		{
			gObjSetPosition(lpObj.m_Index, lpObj.X, lpObj.Y);
			return;
		}
	}

	/*
	if ( gObjCheckXYMapTile(lpObj, 1) == TRUE )
	{
		lpObj.PathCount = 0;
		lpObj.PathCur = 0;
		lpObj.PathStartEnd = 0;
		gObjSetPosition(lpObj.m_Index, lpObj.X, lpObj.Y);
		return;
	}*/

	lpObj.PathX[0] = lpMove->X;
	ax = lpObj.PathX[0];
	lpObj.PathY[0] = lpMove->Y;
	ay = lpObj.PathY[0];
	lpObj.PathDir[0] = lpObj.Dir;
	lpObj.PathStartEnd = 1;

	if (lpObj.PathCount > 0)
	{
		lpObj.PathCur = 1;
		lpObj.PathCount++;
	}

	for (n = 1; n < lpObj.PathCount; n++)
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

		lpObj.PathOri[n - 1] = pathtable;
		lpObj.PathDir[n] = pathtable;
		lpObj.PathX[n] = ax;
		lpObj.PathY[n] = ay;
	}

	if (lpObj.PathCount > 0)
	{
		int nextX;
		int nextY;
		BYTE mapnumber;
		BYTE attr;
		nextX = lpObj.PathX[1];
		nextY = lpObj.PathY[1];
		mapnumber = lpObj.MapNumber;
		attr = MapC[mapnumber].GetAttr(nextX, nextY);

		if ((((BYTE)attr & (BYTE)4) == (BYTE)4) || (((BYTE)attr & (BYTE)8) == (BYTE)8))
		{
			for (n = 0; n < 15; n++)
			{
				lpObj.PathX[n] = 0;
				lpObj.PathY[n] = 0;
				lpObj.PathOri[n] = 0;
			}

			lpObj.PathCount = 0;
			lpObj.PathCur = 0;
			lpObj.PathStartEnd = 0;
			gObjSetPosition(lpObj.m_Index, lpObj.X, lpObj.Y);
			return;
		}
	}

	lpObj.TX = (BYTE)ax;
	lpObj.TY = (BYTE)ay;

	if (lpObj.Type == OBJ_USER && IMPERIAL_MAP_RANGE(lpObj.MapNumber))
	{
		int iCX = lpObj.X;
		int iCY = lpObj.Y;
		BOOL bSetStand = FALSE;
		WORD wMapAttr = 0;

		for (int iCOUNT = 0; iCOUNT < 256; iCOUNT++)
		{
			if (iCX > lpObj.TX)
				iCX--;
			if (iCX < lpObj.TX)
				iCX++;
			if (iCY > lpObj.TY)
				iCY--;
			if (iCY < lpObj.TY)
				iCY++;

			wMapAttr = MapC[lpObj.MapNumber].GetAttr(iCX, iCY);

			if (wMapAttr & 0x10)
			{
				bSetStand = TRUE;
				break;
			}

			if (iCX == lpObj.TX && iCY == lpObj.TY)
			{
				break;
			}

			if (bSetStand == 1)
			{
				lpObj.m_Rest = 1;
				lpObj.PathCur = 0;
				lpObj.PathCount = 0;
				lpObj.PathStartEnd = 0;
				gObjSetPosition(lpObj.m_Index, lpObj.X, lpObj.Y);

				PMSG_ACTIONRESULT pActionResult;

				PHeadSetB((BYTE*)&pActionResult, 0x18, sizeof(pActionResult));
				pActionResult.NumberH = SET_NUMBERH(aIndex);
				pActionResult.NumberL = SET_NUMBERL(aIndex);
				pActionResult.ActionNumber = 0x7A;
				pActionResult.Dir = lpObj.Dir;
				pActionResult.TargetNumberH = 0;
				pActionResult.TargetNumberL = 0;

				IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (BYTE*)&pActionResult, sizeof(pActionResult));
				return;
			}
		}
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (lpObj.Type == OBJ_USER)
		{
			int nextX = lpObj.X;
			int nextY = lpObj.Y;
			int iResult = 0; //should be BOOL
			WORD attr = 0;

			for (int i = 0; i < 256; i++)
			{
				if (nextX > lpObj.TX)
				{
					nextX -= 1;
				}

				if (nextX < lpObj.TX)
				{
					nextX += 1;
				}

				if (nextY > lpObj.TY)
				{
					nextY -= 1;
				}

				if (nextY < lpObj.TY)
				{
					nextY += 1;
				}

				attr = MapC[lpObj.MapNumber].GetAttr(nextX, nextY);

				if ((attr & 16) == 16)
				{
					iResult = 1;
					break;
				}

				if (nextX == lpObj.TX)
				{
					if (nextY == lpObj.TY)
					{
						break;
					}
				}
			}

			if (iResult == 1)
			{
				lpObj.m_Rest = 1;
				lpObj.PathCur = 0;
				lpObj.PathCount = 0;
				lpObj.PathStartEnd = 0;

				gObjSetPosition(lpObj.m_Index, lpObj.X, lpObj.Y);

				PMSG_ACTIONRESULT pActionResult;

				PHeadSetB((BYTE*)&pActionResult, 0x18, sizeof(pActionResult));
				pActionResult.NumberH = SET_NUMBERH(aIndex);
				pActionResult.NumberL = SET_NUMBERL(aIndex);
				pActionResult.ActionNumber = 0x7A;
				pActionResult.Dir = lpObj.Dir;
				pActionResult.TargetNumberH = 0;
				pActionResult.TargetNumberL = 0;

				IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (BYTE*)&pActionResult, sizeof(pActionResult));
				return;
			}
		}
	}

	if (BC_MAP_RANGE(lpObj.MapNumber) && lpObj.Type == OBJ_USER)
	{
		int iBridgeIndex = g_BloodCastle.GetBridgeIndex(lpObj.MapNumber);
		switch (g_BloodCastle.GetCurrentState(iBridgeIndex))
		{
		case 0x01:
			if (lpObj.TY > 15)
			{
				lpObj.m_Rest = 1;
				lpObj.PathCur = 0;
				lpObj.PathCount = 0;
				lpObj.PathStartEnd = 0;
				gObjSetPosition(lpObj.m_Index, lpObj.X, lpObj.Y);

				PMSG_ACTIONRESULT pActionResult;

				PHeadSetB((BYTE*)&pActionResult, 0x18, sizeof(pActionResult));
				pActionResult.NumberH = SET_NUMBERH(aIndex);
				pActionResult.NumberL = SET_NUMBERL(aIndex);
				pActionResult.ActionNumber = 0x7A;
				pActionResult.Dir = lpObj.Dir;
				pActionResult.TargetNumberH = 0;
				pActionResult.TargetNumberL = 0;

				IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (BYTE*)&pActionResult, sizeof(pActionResult));
				return;
			}
			break;
		case 0x02:
			if (lpObj.Y > 17 && g_BloodCastle.CheckPlayStart(iBridgeIndex) == false)
			{
				gObjMoveGate(lpObj.m_Index, iBridgeIndex + 66);
				return;
			}

			if (lpObj.TY > 15 && g_BloodCastle.CheckPlayStart(iBridgeIndex) == false)
			{
				lpObj.m_Rest = 1;
				lpObj.PathCur = 0;
				lpObj.PathCount = 0;
				lpObj.PathStartEnd = 0;
				gObjSetPosition(lpObj.m_Index, lpObj.X, lpObj.Y);

				PMSG_ACTIONRESULT pActionResult;

				PHeadSetB((BYTE*)&pActionResult, 0x18, sizeof(pActionResult));
				pActionResult.NumberH = SET_NUMBERH(aIndex);
				pActionResult.NumberL = SET_NUMBERL(aIndex);
				pActionResult.ActionNumber = 0x7A;
				pActionResult.Dir = lpObj.Dir;
				pActionResult.TargetNumberH = 0;
				pActionResult.TargetNumberL = 0;

				IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (BYTE*)&pActionResult, sizeof(pActionResult));
				return;
			}

			if (lpObj.TY > 76 && g_BloodCastle.CheckCastleDoorLive(iBridgeIndex) == true)
			{
				lpObj.m_Rest = 1;
				lpObj.PathCur = 0;
				lpObj.PathCount = 0;
				lpObj.PathStartEnd = 0;
				gObjSetPosition(lpObj.m_Index, lpObj.X, lpObj.Y);

				PMSG_ACTIONRESULT pActionResult;

				PHeadSetB((BYTE*)&pActionResult, 0x18, sizeof(pActionResult));
				pActionResult.NumberH = SET_NUMBERH(aIndex);
				pActionResult.NumberL = SET_NUMBERL(aIndex);
				pActionResult.ActionNumber = 0x7A;
				pActionResult.Dir = lpObj.Dir;
				pActionResult.TargetNumberH = 0;
				pActionResult.TargetNumberL = 0;

				IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (BYTE*)&pActionResult, sizeof(pActionResult));
				return;
			}
			break;
		}
	}

	PHeadSetB((BYTE*)&pMove, 0xD7, sizeof(pMove));
	pMove.NumberH = SET_NUMBERH(aIndex);
	pMove.NumberL = SET_NUMBERL(aIndex);
	pMove.X = ax;
	pMove.Y = ay;
	pMove.Path = lpObj.Dir << 4;

	if (lpObj.Type == OBJ_USER)
	{
		if (lpObj.m_byCCF_CanEnter == 0xFF)
		{
			lpObj.m_byCCF_CanEnter = 0;

			if (lpObj.m_bCCF_UI_Using == TRUE)
			{
				lpObj.m_bCCF_UI_Using = FALSE;
			}
		}

		if (g_ConfigRead.server.GetServerType() == SERVER_NORMAL)
		{
			if (IT_MAP_RANGE(lpObj.MapNumber))
			{
				if (lpObj.m_nITR_RelicsTick > 0 || lpObj.m_wITR_NpcType > 0 || lpObj.m_byITR_StoneState != 99 || lpObj.m_bITR_RegisteringRelics == true || lpObj.m_bITR_GettingRelics == true)
				{
					g_IT_Event.CancleStoneState(lpObj.m_wITR_NpcType, lpObj.m_byITR_StoneState, lpObj.MapNumber);
					lpObj.m_nITR_RelicsTick = 0;
					lpObj.m_wITR_NpcType = 0;
					lpObj.m_byITR_StoneState = 99;
					lpObj.m_bITR_RegisteringRelics = false;
					lpObj.m_bITR_GettingRelics = false;
				}
			}
		}

		if (lpObj.m_IfState.use)
		{
			if (lpObj.m_IfState.type == 3)
			{
				lpObj.TargetNpcNumber = -1;
				lpObj.m_IfState.type = 0;
				lpObj.m_IfState.use = 0;
			}
		}

		if (!gObjPositionCheck(lpObj))
		{
			lpObj.PathCur = 0;
			lpObj.PathCount = 0;
			ax = lpObj.X;
			ay = lpObj.Y;
			lpObj.TX = (BYTE)ax;
			lpObj.TY = (BYTE)ay;
			pMove.X = ax;
			pMove.Y = ay;
		}

		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMove, pMove.h.size);
	}

	int MVL = MAX_VIEWPORT;

	if (lpObj.Type == OBJ_MONSTER)
		MVL = MaxViewportMonster;

	for (n = 0; n < MVL; n++)
	{
		if (lpObj.VpPlayer2[n].state == TRUE)
		{
			int number = lpObj.VpPlayer2[n].number;

			if (number >= 0)
			{
				if (getGameObject(number)->Connected > PLAYER_CONNECTED)
				{
					if (getGameObject(number)->Live)
					{
						if (getGameObject(number)->Type == OBJ_USER)
						{
							IOCP.DataSend(lpObj.VpPlayer2[n].number, (BYTE*)&pMove, pMove.h.size);
						}
					}
				}
				else
				{
					lpObj.VpPlayer2[n].number = -1;
					lpObj.VpPlayer2[n].state = 0;
					lpObj.VPCount2--;
				}
			}
		}
	}

	MapC[lpObj.MapNumber].ClearStandAttr(lpObj.m_OldX, lpObj.m_OldY);
	MapC[lpObj.MapNumber].SetStandAttr(lpObj.TX, lpObj.TY);
	lpObj.m_OldX = lpObj.TX;
	lpObj.m_OldY = lpObj.TY;
	lpObj.X = sx;
	lpObj.Y = sy;
	lpObj.m_ViewState = 0;

	if (lpObj.Type == OBJ_USER) // Recv Check
	{
		if (rand() % 5 == 0)
		{
			this->GCSendRecvCheck(lpObj.m_Index);
		}
	}
}



void GameProtocol::RecvPositionSetProc(PMSG_POSISTION_SET * lpMove, CGameObject &Obj)
{
	short n;

	if (ObjectMaxRange(aIndex) == FALSE)
	{
		return;
	}

	

	if (this->PacketCheckTime(aIndex) == FALSE)
	{
		return;
	}

	if (lpObj.Teleport != 0)
	{
		return;
	}

	if (MAX_MAP_RANGE(lpObj.MapNumber) == FALSE)
	{
		return;
	}

	//K2 AntiHack Teleport
	int XDif = 0;
	int YDif = 0;

	if (lpObj.X > lpMove->X)
		XDif = lpObj.X - lpMove->X;
	else
		XDif = lpMove->X - lpObj.X;

	if (lpObj.Y > lpMove->Y)
		YDif = lpObj.Y - lpMove->Y;
	else
		YDif = lpMove->Y - lpObj.Y;

	//sLog->outBasic("[K2] XDif %d YDif %d", XDif, YDif);
	if ((XDif > 6) || (YDif > 6))
	{
		PMSG_TELEPORT_RESULT pTeleportResult;

		pTeleportResult.h.c = 0xC3;
		pTeleportResult.h.size = sizeof(pTeleportResult);
		pTeleportResult.h.headcode = 0x1C;
		pTeleportResult.Type = 0;
		pTeleportResult.Unk = 0;
		pTeleportResult.MapNumber = lpObj.MapNumber;
		pTeleportResult.Unk1 = 0;
		pTeleportResult.MapX = lpObj.X;
		pTeleportResult.MapY = lpObj.Y;
		pTeleportResult.Dir = lpObj.Dir;

		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pTeleportResult, pTeleportResult.h.size);

		PMSG_NOTICE pNotice;

		TNotice::MakeNoticeMsg((TNotice *)&pNotice, 1, "[K2][Anti-Cheat] Teleport Hack Blocked.");
		sLog->outBasic("[K2][Anti-Cheat] Teleport Hack Blocked. [AccID=%s | CharName=%s]", lpObj.AccountID, lpObj.Name);
		TNotice::SendNoticeToUser(aIndex, (TNotice *)&pNotice);

		return;
	}
	lpObj.X = lpMove->X;
	lpObj.Y = lpMove->Y;

	if (CC_MAP_RANGE(lpObj.MapNumber) != FALSE)
	{
		if ((GetTickCount() - lpObj.m_iChaosCastleBlowTime) < 1000)
		{
			return;
		}
	}

	if (lpObj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		if ((GetTickCount() - lpObj.m_nCCF_BlowTime) < 1000)
		{
			return;
		}
	}

	if (gObjCheckXYMapTile(lpObj, 2) == TRUE)
	{
		return;
	}

	PMSG_RECV_POSISTION_SET pMove;

	PHeadSetB((BYTE*)&pMove, 0x10, sizeof(pMove));
	pMove.NumberH = SET_NUMBERH(aIndex);
	pMove.NumberL = SET_NUMBERL(aIndex);
	pMove.X = lpMove->X;
	pMove.Y = lpMove->Y;
	lpObj.TX = lpMove->X;
	lpObj.TY = lpMove->Y;

	if (::gObjPositionCheck(lpObj) == FALSE)
	{
		return;
	}

	CreateFrustrum(lpObj.X, lpObj.Y, aIndex);

	if (lpObj.Type == OBJ_USER)
	{
		IOCP.DataSend(lpObj.m_Index, (UCHAR *)&pMove, pMove.h.size);
	}

	int MVL = MAX_VIEWPORT;

	if (lpObj.Type == OBJ_MONSTER)
	{
		MVL = MaxViewportMonster;
	}

	for (n = 0; n < MVL; n++)
	{
		if (lpObj.VpPlayer2[n].type == OBJ_USER)
		{
			if (lpObj.VpPlayer2[n].state == 1)
			{
				int Index = lpObj.VpPlayer2[n].number;

				if (ObjectMaxRange(Index) == false)
				{
					continue;
				}

				if (getGameObject(Index]->Connected > PLAYER_CONNECTED && getGameObject(Index)->Live != FALSE)
				{
					IOCP.DataSend(lpObj.VpPlayer2[n].number, (UCHAR *)&pMove, pMove.h.size);
				}
				else
				{
					lpObj.VpPlayer2[n].number = -1;
					lpObj.VpPlayer2[n].state = FALSE;
					lpObj.VPCount2--;
				}
			}
		}
	}

	MapC[lpObj.MapNumber].ClearStandAttr(lpObj.m_OldX, lpObj.m_OldY);
	MapC[lpObj.MapNumber].SetStandAttr(lpObj.TX, lpObj.TY);

	lpObj.m_OldX = lpObj.TX;
	lpObj.m_OldY = lpObj.TY;

}



void GameProtocol::CGAttack(PMSG_ATTACK* lpMsg, CGameObject &Obj)
{
	if (aIndex < 0 || aIndex > g_ConfigRead.server.GetObjectMax() - 1)
	{
		return;
	}

	CGameObject lpObj;
	CGameObject lpTargetObj;
	int usernumber = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	if (usernumber < 0 || usernumber > g_ConfigRead.server.GetObjectMax() - 1)
	{
		return;
	}

	if (!getGameObject(usernumber)->Live)
		return;

	if (lpObj.Type == OBJ_USER  && lpObj.m_PlayerData->m_bSecurityCheck == false)
		return;
	int Allowed = 120;
	lpObj = &getGameObject(aIndex);
	if (lpObj.m_AttackSpeed >= 300)
	{
		Allowed = 170; //6.26
	}
	else if ((lpObj.m_AttackSpeed > 150) && (lpObj.m_AttackSpeed < 300))
		Allowed = 190;
	else if ((lpObj.m_AttackSpeed > 0) && (lpObj.m_AttackSpeed < 150))
		Allowed = 190;
	if (lpObj.Type == OBJ_USER)
	{
		//10:50:00
		//10:51:00
		//1000 iTimeCalc < Allowed 300//
		int iTimeCalc = GetTickCount() - lpObj.m_LastAttackTime;
		lpObj.m_LastAttackTime = GetTickCount();
		if (iTimeCalc < Allowed) //1 1200
		{
			//sLog->outBasic("[K2 ANTIHACK] Speed Hack Detect %s %s %d %d", lpObj.AccountID, lpObj.Name, Allowed, lpObj.m_AttackSpeed);
			return;
		}
	}
	/*if (iTimeCalc < lpObj.m_DetectSpeedHackTime) //season4 changed (fix)
	{
		lpObj.m_DetectCount++;
		lpObj.m_SumLastAttackTime += iTimeCalc;

		if (lpObj.m_DetectCount > gHackCheckCount)
		{
			lpObj.m_DetectedHackKickCount++;
			lpObj.m_SpeedHackPenalty = gSpeedHackPenalty;

			if (gIsKickDetecHackCountLimit)
			{
				if (lpObj.m_DetectedHackKickCount > gDetectedHackKickCount)
				{
					sLog->outBasic("[%s][%s] Kick DetecHackCountLimit Over User (%d)", lpObj.AccountID, lpObj.Name, lpObj.m_DetectedHackKickCount);
					this->GCSendDisableReconnect(aIndex);
					//IOCP.CloseClient(aIndex);
					return;
				}

				sLog->outBasic("[%s][%s] Attack Speed Is Wrong Normal (%d)(%d) Penalty %d", lpObj.AccountID, lpObj.Name, lpObj.m_DetectSpeedHackTime, lpObj.m_SumLastAttackTime / lpObj.m_DetectCount, lpObj.m_SpeedHackPenalty);
			}

		}

		lpObj.m_LastAttackTime = GetTickCount();
	}
	else
	{
		lpObj.m_SumLastAttackTime = 0;
		lpObj.m_DetectCount = 0;
	}
	*/
	lpObj.m_LastAttackTime = GetTickCount();

	if (bIsIgnorePacketSpeedHackDetect)
	{
		if (lpObj.m_SpeedHackPenalty > 0)
		{
			lpObj.m_SpeedHackPenalty--;

			return;
		}
	}

	lpTargetObj = &getGameObject(usernumber);
	lpObj.Dir = lpMsg->DirDis;
	// (lpMsg->AttackAction == 6)
	GCActionSend(lpObj, lpMsg->AttackAction, aIndex, usernumber);
	gComboAttack.CheckCombo(aIndex, 0);
	gObjAttack(lpObj, lpTargetObj, NULL, FALSE, FALSE, 0, FALSE, 0, 0);

	lpObj.UseMagicNumber = 0;
}

void GameProtocol::GCDamageSend(CGameObject &Obj, CGameObject &ObjTarget, int AttackDamage, int MSBFlag, int MSBDamage, int iShieldDamage)
{
	PMSG_ATTACKRESULT pResult;

	PHeadSetB((BYTE*)&pResult, 0xDF, sizeof(pResult));
	pResult.NumberH = SET_NUMBERH(TargetIndex);
	pResult.NumberL = SET_NUMBERL(TargetIndex);
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

	if (getGameObject(TargetIndex)->Type == OBJ_USER)
	{
		IOCP.DataSend(TargetIndex, (BYTE*)&pResult, pResult.h.size);
	}

	if (cManager.WatchTargetIndex == TargetIndex || cManager.WatchTargetIndex == aIndex)
	{
		cManager.DataSend((BYTE*)&pResult, pResult.h.size);
	}

	if (lpObj.Type == OBJ_USER)
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
	}
}


struct PMSG_DAMAGE
{
	PBMSG_HEAD2 h;	// C1:F3:07
	BYTE subcode;	// 3
	BYTE DamageH;	// 4
	BYTE DamageL;	// 5
	BYTE btShieldDamageH;	// 6
	BYTE btShieldDamageL;	// 7
};

void GameProtocol::GCDamageSendPoison(CGameObject &Obj, int damage, int iShieldDamage)
{
	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	PMSG_DAMAGE pDamage;

	PHeadSubSetB((BYTE*)&pDamage, 0xF3, 0x07, sizeof(pDamage));
	pDamage.DamageH = SET_NUMBERH(damage);
	pDamage.DamageL = SET_NUMBERL(damage);
	pDamage.btShieldDamageH = SET_NUMBERH(iShieldDamage);
	pDamage.btShieldDamageL = SET_NUMBERL(iShieldDamage);

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pDamage, pDamage.h.size);
}


struct PMSG_KILLPLAYER
{
	PBMSG_HEAD2 h;	// C1:16
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	BYTE ExpH;	// 5
	BYTE ExpL;	// 6
	BYTE DamageH;	// 7
	BYTE DamageL;	// 8
};


void GameProtocol::GCKillPlayerExpSend(CGameObject &Obj, CGameObject &ObjTarget, int exp, int AttackDamage, BOOL MSBFlag)
{
	PMSG_KILLPLAYER_EXT pkillMsg;

	PHeadSetBE((BYTE*)&pkillMsg, 0x9c, sizeof(pkillMsg));
	pkillMsg.NumberH = SET_NUMBERH(TargetIndex);
	pkillMsg.NumberL = SET_NUMBERL(TargetIndex);
	pkillMsg.ExpH = SET_NUMBERHW(exp);
	pkillMsg.ExpL = SET_NUMBERLW(exp);
	pkillMsg.DamageH = SET_NUMBERH(AttackDamage);
	pkillMsg.DamageL = SET_NUMBERL(AttackDamage);

	if (MSBFlag != FALSE)
	{
		pkillMsg.NumberH &= 0x7F;
		pkillMsg.NumberH |= 0x80;
	}

	if (BC_MAP_RANGE(lpObj.MapNumber) != FALSE)
	{
		g_BloodCastle.AddExperience(aIndex, exp);
	}

	IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pkillMsg, pkillMsg.h.size);
}

void GameProtocol::GCKillPlayerMasterExpSend(CGameObject &Obj, CGameObject &ObjTarget, int exp, int AttackDamage, BOOL MSBFlag)
{
	PMSG_KILLPLAYER_EXT pkillMsg;

	PHeadSetBE((BYTE*)&pkillMsg, 0x9C, sizeof(pkillMsg));
	pkillMsg.NumberH = SET_NUMBERH(TargetIndex);
	pkillMsg.NumberL = SET_NUMBERL(TargetIndex);
	pkillMsg.ExpH = SET_NUMBERHW(exp);
	pkillMsg.ExpL = SET_NUMBERLW(exp);
	pkillMsg.DamageH = SET_NUMBERH(AttackDamage);
	pkillMsg.DamageL = SET_NUMBERL(AttackDamage);

	if (MSBFlag != FALSE)
	{
		pkillMsg.NumberH &= 0x7F;
		pkillMsg.NumberH |= 0x80;
	}

	if (BC_MAP_RANGE(lpObj.MapNumber) != FALSE)
	{
		g_BloodCastle.AddExperience(aIndex, exp);
	}

	IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pkillMsg, pkillMsg.h.size);
}



struct PMSG_DIEPLAYER
{
	PBMSG_HEAD2 h;	// C1:17
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	BYTE Skill;	// 5
	BYTE KillerNumberH;	// 6
	BYTE KillerNumberL;	// 7
};


void GameProtocol::GCDiePlayerSend(CGameObject &Obj, int TargetIndex, BYTE skill, int KillerIndex)
{
	PMSG_DIEPLAYER pDieMsg;

	PHeadSetB((BYTE*)&pDieMsg, 0x17, sizeof(pDieMsg));
	pDieMsg.NumberH = SET_NUMBERH(TargetIndex);
	pDieMsg.NumberL = SET_NUMBERL(TargetIndex);
	pDieMsg.Skill = skill;
	pDieMsg.KillerNumberH = SET_NUMBERH(KillerIndex);
	pDieMsg.KillerNumberL = SET_NUMBERL(KillerIndex);
	int iSize = pDieMsg.h.size;

	MsgSendV2(lpObj, (BYTE*)&pDieMsg, iSize);

	if (lpObj.Type == OBJ_USER)
	{
		IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (BYTE*)&pDieMsg, iSize);
	}
}



void GameProtocol::GCActionSend(CGameObject &Obj, BYTE ActionNumber, CGameObject &ObjTarget)
{
	PMSG_ACTIONRESULT pActionResult;

	PHeadSetB((BYTE*)&pActionResult, 0x18, sizeof(pActionResult));
	pActionResult.NumberH = SET_NUMBERH(aIndex);
	pActionResult.NumberL = SET_NUMBERL(aIndex);
	pActionResult.ActionNumber = ActionNumber;
	pActionResult.Dir = lpObj.Dir;
	pActionResult.TargetNumberH = SET_NUMBERH(aTargetIndex);
	pActionResult.TargetNumberL = SET_NUMBERL(aTargetIndex);

	MsgSendV2(lpObj, (BYTE*)&pActionResult, pActionResult.h.size);
}



void GameProtocol::CGActionRecv(PMSG_ACTION * lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(aIndex))
	{
		return;
	}

	if (lpMsg->Dir > 7 || lpMsg->h.size > 5)
	{
		//this->GCSendDisableReconnect(aIndex);
		IOCP.CloseClient(aIndex);
	}

	PMSG_ACTIONRESULT pResult;
	

	PHeadSetB((BYTE*)&pResult, 0x18, sizeof(pResult));
	pResult.ActionNumber = lpMsg->ActionNumber;
	pResult.NumberH = SET_NUMBERH(aIndex);
	pResult.NumberL = SET_NUMBERL(aIndex);
	lpObj.m_ActionNumber = lpMsg->ActionNumber;
	lpObj.Dir = lpMsg->Dir;
	pResult.Dir = lpObj.Dir;
	pResult.TargetNumberH = lpMsg->iTargetIndexH;
	pResult.TargetNumberL = lpMsg->iTargetIndexL;

	if (lpObj.m_ActionNumber == 0x80)
	{
		lpObj.m_ViewState = 1;
		lpObj.m_Rest = lpObj.m_ActionNumber;
	}

	else if (lpObj.m_ActionNumber == 0x81)
	{
		lpObj.m_ViewState = 2;
		lpObj.m_Rest = lpObj.m_ActionNumber;
	}

	else if (lpObj.m_ActionNumber == 0x82)
	{
		lpObj.m_ViewState = 3;
		lpObj.m_Rest = lpObj.m_ActionNumber;
	}

	if (GetTickCount() - lpObj.m_ActionTime < 500)
	{
		lpObj.m_ActionCount++;

		if (lpObj.m_ActionCount > 9)
		{
			//this->GCSendDisableReconnect(aIndex);
			IOCP.CloseClient(aIndex);
			return;
		}
	}

	else
	{
		lpObj.m_ActionCount = 0;
	}

	lpObj.m_ActionTime = GetTickCount();

	int MVL = MAX_VIEWPORT;

	if (lpObj.Type == OBJ_MONSTER)
		MVL = MaxViewportMonster;

	for (int n = 0; n < MVL; n++)
	{
		if (lpObj.VpPlayer2[n].type == OBJ_USER)
		{
			if (lpObj.VpPlayer2[n].state != 0 && lpObj.VpPlayer2[n].state != 0x10 && lpObj.VpPlayer2[n].state != 0x08)
			{
				IOCP.DataSend(lpObj.VpPlayer2[n].number, (BYTE*)&pResult, pResult.h.size);
			}
		}
	}
}



void GameProtocol::CGMagicAttack(PMSG_MAGICATTACK* lpMsg, CGameObject &Obj)
{
	if (aIndex < 0 || aIndex > g_ConfigRead.server.GetObjectMax() - 1)
	{
		return;
	}

	//PMSG_MAGICATTACK * lpMsg = (PMSG_MAGICATTACK *)(lpRecv);

	CGameObject lpObj;
	CGameObject lpTargetObj;
	WORD MagicNumber = MAKE_NUMBERW(lpMsg->MagicNumberH, lpMsg->MagicNumberL);
	int usernumber = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	if (usernumber < 0 || usernumber > g_ConfigRead.server.GetObjectMax() - 1)
	{
		return;
	}

	if (getGameObject(usernumber]->Live == 0 || getGameObject(usernumber)->Connected < PLAYER_LOGGED)
		return;

	if (lpObj.CloseCount >= 0)
		return;

	if (lpObj.Type == OBJ_USER && lpObj.m_PlayerData->m_bSecurityCheck == false)
		return;

	lpObj = &getGameObject(aIndex);
	lpTargetObj = &getGameObject(usernumber);

	CMagicInf * lpMagic;
	int attackret = gObjCheckAttackArea(aIndex, usernumber);

	if (attackret)
	{

		if (lpObj.Class == 275)	// Kundun
			lpObj.TargetNumber = -1;

		if (bIsIgnorePacketSpeedHackDetect)
			return;
	}

	if (lpObj.Type == OBJ_USER)
	{

		if (MagicNumber == 58) // Nova cast
			usernumber = 58;

		if (MagicNumber == 395)
			usernumber = 395;

		lpMagic = gObjGetMagicSearch(lpObj, MagicNumber);
		if (!lpMagic)
			return;

		if (lpMagic->m_Skill == AT_SKILL_RECALL_PARTY)
		{
			if (g_NewPVP.IsDuel(*lpObj) || g_NewPVP.IsDuel(*lpTargetObj))
			{
				return;
			}
		}
		if (MagicDamageC.SkillGetRequireClass(lpObj.Class, lpObj.m_PlayerData->ChangeUP, lpMagic->m_Skill) < 1)
			return;
		if (MagicDamageC.CheckStatus(lpMagic->m_Skill, lpObj.m_PlayerData->GuildStatus) == 0)
		{
			return;
		}
		if (MagicDamageC.CheckKillCount(lpMagic->m_Skill, lpObj.m_btKillCount) < 0)
		{
			return;
		}

		if (lpObj.Teleport && gObjUseSkill.m_SkillData.EnableWizardSkillWhileTeleport == false)
		{
			return;
		}
	}
	else
	{
		lpMagic = gObjGetMagic(lpObj, MagicNumber);
		if (!lpMagic)
			return;
	}

	if (lpMagic->m_Skill != 24 && gObjUseSkill.SpeedHackCheck(aIndex) == FALSE)
		return;

	if (!gCheckSkillDistance(aIndex, usernumber, lpMagic->m_Skill))
	{
		DWORD dwTick = GetTickCount() - lpObj.m_dwSkillDistanceErrorTick;

		if (dwTick > (g_ConfigRead.SkillDistanceTimeDC * 1000))
		{
			lpObj.m_iSkillDistanceErrorCount = 0;
			lpObj.m_dwSkillDistanceErrorTick = GetTickCount();
		}

		lpObj.m_iSkillDistanceErrorCount++;

		if (lpObj.m_iSkillDistanceErrorCount > g_ConfigRead.SkillDistanceCountDC)
		{
			if (g_ConfigRead.SkillDistanceDC == 1)
			{
				//this->GCSendDisableReconnect(lpObj.m_Index);
				IOCP.CloseClient(lpObj.m_Index);
				return;
			}

			lpObj.m_iSkillDistanceErrorCount = 0;
		}

		return;
	}

	gObjUseSkill.UseSkill(aIndex, usernumber, lpMagic);
}

void GameProtocol::CGRageAttack(PMSG_RAGEATTACK_REQ* lpRecv, CGameObject &Obj)
{
	CGameObject lpObj;
	CGameObject lpTargetObj;

	PMSG_RAGEATTACK_REQ * lpMsg = (PMSG_RAGEATTACK_REQ *)(lpRecv);

	WORD MagicNumber = MAKE_NUMBERW(lpMsg->MagicNumberH, lpMsg->MagicNumberL);
	int usernumber = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
	CMagicInf * lpMagic;

	lpObj = &getGameObject(aIndex);
	lpTargetObj = &getGameObject(usernumber);

	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	if (lpObj.Teleport && gObjUseSkill.m_SkillData.EnableWizardSkillWhileTeleport == false)
	{
		return;
	}

	if (lpObj.CloseCount >= 0)
	{
		return;
	}

	if (MagicNumber == 263 || MagicNumber == 559 || MagicNumber == 563)
	{
		this->GCMonkMagicAttack(&getGameObject(aIndex], MagicNumber, lpObj.m_PlayerData->DarkSideTarget[0), TRUE);
		return;
	}

	if (usernumber < 0 || usernumber > g_ConfigRead.server.GetObjectMax() - 1)
	{
		return;
	}

	if (getGameObject(usernumber]->Live == 0 || getGameObject(usernumber)->Connected < PLAYER_LOGGED)
		return;

	if (lpObj.Type == OBJ_USER)
	{
		lpMagic = gObjGetMagicSearch(lpObj, MagicNumber);

		if (!lpMagic)
			return;

		if (lpMagic->m_Skill == AT_SKILL_RECALL_PARTY)
		{
			if (g_NewPVP.IsDuel(*lpObj) || g_NewPVP.IsDuel(*lpTargetObj))
			{
				return;
			}
		}

		if (MagicDamageC.SkillGetRequireClass(lpObj.Class, lpObj.m_PlayerData->ChangeUP, lpMagic->m_Skill) < 1)
			return;

		if (MagicDamageC.CheckStatus(lpMagic->m_Skill, lpObj.m_PlayerData->GuildStatus) == 0)
		{
			return;
		}

		if (MagicDamageC.CheckKillCount(lpMagic->m_Skill, lpObj.m_btKillCount) < 0)
		{
			return;
		}

		if (lpObj.m_PlayerData->m_bSecurityCheck == false)
		{
			return;
		}
	}

	if (!gCheckSkillDistance(aIndex, usernumber, lpMagic->m_Skill))
	{
		DWORD dwTick = GetTickCount() - lpObj.m_dwSkillDistanceErrorTick;

		if (dwTick > (g_ConfigRead.SkillDistanceTimeDC * 1000))
		{
			lpObj.m_iSkillDistanceErrorCount = 0;
			lpObj.m_dwSkillDistanceErrorTick = GetTickCount();
		}

		lpObj.m_iSkillDistanceErrorCount++;

		if (lpObj.m_iSkillDistanceErrorCount > g_ConfigRead.SkillDistanceCountDC)
		{
			if (g_ConfigRead.SkillDistanceDC == 1)
			{
				//this->GCSendDisableReconnect(lpObj.m_Index);
				IOCP.CloseClient(lpObj.m_Index);
				return;
			}

			lpObj.m_iSkillDistanceErrorCount = 0;
		}

		return;
	}

	this->GCMonkMagicAttack(lpObj, MagicNumber, usernumber, TRUE);
}

void GameProtocol::CGRageAttackRange(PMSG_RAGE_ATTACK_RANGE *lpMsg, CGameObject &Obj)
{
	if (ObjectMaxRange(aIndex) == FALSE)
	{
		return;
	}

	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	if (lpObj.Teleport && gObjUseSkill.m_SkillData.EnableWizardSkillWhileTeleport == false)
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

	lpMagic = gObjGetMagicSearch(lpObj, MagicNumber);

	if (lpMagic)
	{
		if (gObjUseSkill.SkillMonkDarkSideGetTargetIndex(aIndex, TargetIndex, lpMagic, Target) == TRUE)
		{
			PMSG_RAGE_ATTACK_RANGE_ANS pMsg;
			PHeadSetBE((BYTE*)&pMsg, 0x4B, sizeof(pMsg));

			memcpy(&pMsg.Target, Target, sizeof(Target));
			pMsg.MagicNumber = lpMagic->m_Skill;;

			lpObj.m_PlayerData->DarkSideCount = 0;

			for (int i = 0; i < 5; i++)
			{
				if (Target[i] != 10000)
				{
					lpObj.m_PlayerData->DarkSideCount++;
				}
			}

			memcpy(lpObj.m_PlayerData->DarkSideTarget, Target, sizeof(Target));
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		}
	}
}

void GameProtocol::GCMagicAttackNumberSend(CGameObject &Obj, WORD MagicNumber, CGameObject &Obj, BYTE skillsuccess)
{
	PMSG_MAGICATTACK_RESULT pAttack;

	PHeadSetBE((BYTE*)&pAttack, 0x19, sizeof(pAttack));
	pAttack.MagicNumberH = SET_NUMBERH(MagicNumber);
	pAttack.MagicNumberL = SET_NUMBERL(MagicNumber);
	pAttack.SourceNumberH = SET_NUMBERH(lpObj.m_Index);
	pAttack.SourceNumberL = SET_NUMBERL(lpObj.m_Index);
	pAttack.TargetNumberH = SET_NUMBERH(usernumber);
	pAttack.TargetNumberL = SET_NUMBERL(usernumber);
	pAttack.TargetNumberH &= 0x7F;

	if (skillsuccess == 1)
		pAttack.TargetNumberH |= 0x80;

	if (lpObj.Type == OBJ_USER)
		IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (BYTE*)&pAttack, pAttack.h.size);

	if (CC_MAP_RANGE(lpObj.MapNumber) || lpObj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
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

	MsgSendV2(lpObj, (BYTE*)&pAttack, pAttack.h.size);
}

void GameProtocol::GCMonkMagicAttack(CGameObject &Obj, WORD MagicNumber, CGameObject &Obj, BYTE skillsuccess)
{
	PMSG_RAGEATTACK pAttack;

	PHeadSetBE((BYTE*)&pAttack, 0x4A, sizeof(pAttack));
	pAttack.MagicNumberH = SET_NUMBERH(MagicNumber);
	pAttack.MagicNumberL = SET_NUMBERL(MagicNumber);
	pAttack.SourceNumberH = SET_NUMBERH(lpObj.m_Index);
	pAttack.SourceNumberL = SET_NUMBERL(lpObj.m_Index);
	pAttack.TargetNumberH = SET_NUMBERH(usernumber);
	pAttack.TargetNumberL = SET_NUMBERL(usernumber);
	pAttack.TargetNumberH &= 0x7F;

	if (skillsuccess == 1)
		pAttack.TargetNumberH |= 0x80;

	if (lpObj.Type == OBJ_USER)
		IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (BYTE*)&pAttack, pAttack.h.size);

	MsgSendV2(lpObj, (BYTE*)&pAttack, pAttack.h.size);
}

void GameProtocol::GCMagicCancelSend(CGameObject &Obj, WORD MagicNumber)
{
	PMSG_MAGICCANCEL pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x1B, sizeof(pMsg));
	pMsg.MagicNumberL = SET_NUMBERL(MagicNumber);
	pMsg.MagicNumberH = SET_NUMBERH(MagicNumber);
	pMsg.NumberH = SET_NUMBERH(lpObj.m_Index);
	pMsg.NumberL = SET_NUMBERL(lpObj.m_Index);

	if (lpObj.Type == OBJ_USER)
		IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (UCHAR*)&pMsg, pMsg.h.size);

	MsgSendV2(lpObj, (UCHAR*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCUseEffectItem(CGameObject &Obj, int iEffectUseOption, int iOptionType, int iEffectType, int iLeftTime)
{
	PMSG_USEEFFECTITEM pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x2D, sizeof(pMsg));
	pMsg.byEffectOption = iEffectUseOption;
	pMsg.wOptionType = iOptionType;
	pMsg.wEffectType = iEffectType;
	pMsg.iLeftTime = iLeftTime;
	pMsg.btBuffEffectType = 0;
	//	pMsg.dwEffectValue = 0;

	if (lpObj.Type == OBJ_USER)
		IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (UCHAR*)&pMsg, pMsg.h.size);
}

void GameProtocol::CGMagicCancel(PMSG_MAGICCANCEL * lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(aIndex))
		return;

	

	WORD MagicNumber = MAKE_NUMBERW(lpMsg->MagicNumberL, lpMsg->MagicNumberH);

	switch (MagicNumber)
	{
	case 77:
		gObjRemoveBuffEffect(lpObj, 6);
		break;
	case 233:
		gObjRemoveBuffEffect(lpObj, 82);
		break;
	case 380:
		gObjRemoveBuffEffect(lpObj, 138);
		break;
	case 383:
		gObjRemoveBuffEffect(lpObj, 139);
		break;
	case 441:
		gObjRemoveBuffEffect(lpObj, 143);
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

void GameProtocol::GCUseMonsterSkillSend(CGameObject &Obj, CGameObject lpTargetObj, int iSkillNumber)
{
	PMSG_USE_MONSTERSKILL pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0x69, 0x00, sizeof(pMsg));
	pMsg.btMonsterSkillNumber = iSkillNumber;
	pMsg.wObjIndex = lpObj.m_Index;
	pMsg.wTargetObjIndex = lpTargetObj.m_Index;

	if (lpObj.Type == OBJ_USER)
		IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (UCHAR*)&pMsg, pMsg.h.size);

	MsgSendV2(lpObj, (UCHAR*)&pMsg, pMsg.h.size);
}

struct PMSG_VIEWSKILLSTATE
{
	PBMSG_HEAD2 h;	// C1:07
	BYTE State;	// 3
	BYTE NumberH;	// 4
	BYTE NumberL;	// 5
	BYTE BuffEffect; // 6
};

void GameProtocol::GCStateInfoSend(CGameObject &Obj, BYTE state, int BuffViewport)
{
	PMSG_VIEWSKILLSTATE pMsg;

	BYTE BuffCount = 0;

	PHeadSetB((BYTE*)&pMsg, 0x07, sizeof(pMsg));

	pMsg.State = state;

	pMsg.NumberH = HIBYTE(lpObj.m_Index);
	pMsg.NumberL = LOBYTE(lpObj.m_Index);

	pMsg.BuffEffect = BuffViewport;

	if (lpObj.Type == OBJ_USER)
		IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (BYTE*)&pMsg, pMsg.h.size);

	if (CC_MAP_RANGE(lpObj.MapNumber) || lpObj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)	return;

	MsgSendV2(lpObj, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::CGTeleportRecv(PMSG_TELEPORT* lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(aIndex))
		return;

	if (gObjCheckUsedBuffEffect(&getGameObject(aIndex], BUFFTYPE_STUN) == TRUE || gObjCheckUsedBuffEffect(&getGameObject(aIndex), BUFFTYPE_SLEEP) == TRUE
		|| gObjCheckUsedBuffEffect(&getGameObject(aIndex], BUFFTYPE_FREEZE_2) || gObjCheckUsedBuffEffect(&getGameObject(aIndex), BUFFTYPE_EARTH_BINDS))
	{
		PMSG_TELEPORT_RESULT pTeleportResult;

		pTeleportResult.h.c = 0xC3;
		pTeleportResult.h.size = sizeof(pTeleportResult);
		pTeleportResult.h.headcode = 0x1C;
		pTeleportResult.Type = 0;
		pTeleportResult.Unk = 0;
		pTeleportResult.MapNumber = lpObj.MapNumber;
		pTeleportResult.Unk1 = 0;
		pTeleportResult.MapX = lpObj.X;
		pTeleportResult.MapY = lpObj.Y;
		pTeleportResult.Dir = lpObj.Dir;

		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pTeleportResult, pTeleportResult.h.size);
		return;
	}

	

	if (lpObj.m_bMapSvrMoveReq == true || lpObj.m_bMapSvrMoveReq_2 == true || lpObj.m_bMapSvrMoveQuit == true || lpObj.m_State == 32)
	{
		return;
	}

	if (IT_MAP_RANGE(lpObj.MapNumber))
	{
		if (g_IT_Event.GetIllusionTempleState(lpObj.MapNumber) == 2)
		{
			if (g_IT_Event.CheckRelics(aIndex) != FALSE)
			{
				PMSG_TELEPORT_RESULT pTeleportResult;

				pTeleportResult.h.c = 0xC3;
				pTeleportResult.h.size = sizeof(pTeleportResult);
				pTeleportResult.h.headcode = 0x1C;
				pTeleportResult.Type = 0;
				pTeleportResult.Unk = 0;
				pTeleportResult.MapNumber = lpObj.MapNumber;
				pTeleportResult.Unk1 = 0;
				pTeleportResult.MapX = lpObj.X;
				pTeleportResult.MapY = lpObj.Y;
				pTeleportResult.Dir = lpObj.Dir;

				IOCP.DataSend(lpObj.m_Index, (BYTE*)&pTeleportResult, pTeleportResult.h.size);
				return;
			}

			if (g_IT_Event.CheckSkillRestraint(lpObj.m_nITR_Index, lpObj.MapNumber) != FALSE)
			{
				PMSG_TELEPORT_RESULT pTeleportResult;

				pTeleportResult.h.c = 0xC3;
				pTeleportResult.h.size = sizeof(pTeleportResult);
				pTeleportResult.h.headcode = 0x1C;
				pTeleportResult.Type = 0;
				pTeleportResult.Unk = 0;
				pTeleportResult.MapNumber = lpObj.MapNumber;
				pTeleportResult.Unk1 = 0;
				pTeleportResult.MapX = lpObj.X;
				pTeleportResult.MapY = lpObj.Y;
				pTeleportResult.Dir = lpObj.Dir;

				IOCP.DataSend(lpObj.m_Index, (BYTE*)&pTeleportResult, pTeleportResult.h.size);
				return;
			}
		}
	}

	if (IT_MAP_RANGE(lpObj.MapNumber))
	{
		if (g_IT_Event.GetIllusionTempleState(lpObj.MapNumber) == 1)
		{
			PMSG_TELEPORT_RESULT pTeleportResult;

			pTeleportResult.h.c = 0xC3;
			pTeleportResult.h.size = sizeof(pTeleportResult);
			pTeleportResult.h.headcode = 0x1C;
			pTeleportResult.Type = 0;
			pTeleportResult.Unk = 0;
			pTeleportResult.MapNumber = lpObj.MapNumber;
			pTeleportResult.Unk1 = 0;
			pTeleportResult.MapX = lpObj.X;
			pTeleportResult.MapY = lpObj.Y;
			pTeleportResult.Dir = lpObj.Dir;

			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pTeleportResult, pTeleportResult.h.size);
			return;
		}
	}

	if (CC_MAP_RANGE(lpObj.MapNumber) != FALSE) //season 4.0 add-on (fix)
	{
		if (g_ChaosCastle.GetCurrentState(lpObj.m_cChaosCastleIndex) == 3)
		{
			PMSG_TELEPORT_RESULT pTeleportResult;

			pTeleportResult.h.c = 0xC3;
			pTeleportResult.h.size = sizeof(pTeleportResult);
			pTeleportResult.h.headcode = 0x1C;
			pTeleportResult.Type = 0;
			pTeleportResult.Unk = 0;
			pTeleportResult.MapNumber = lpObj.MapNumber;
			pTeleportResult.Unk1 = 0;
			pTeleportResult.MapX = lpObj.X;
			pTeleportResult.MapY = lpObj.Y;
			pTeleportResult.Dir = lpObj.Dir;
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pTeleportResult, pTeleportResult.h.size);
			return;
		}
	}

	if (lpObj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL && g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		if (g_ChaosCastleFinal.GetCurrentState() == 3)
		{
			PMSG_TELEPORT_RESULT pTeleportResult;

			pTeleportResult.h.c = 0xC3;
			pTeleportResult.h.size = sizeof(pTeleportResult);
			pTeleportResult.h.headcode = 0x1C;
			pTeleportResult.Type = 0;
			pTeleportResult.Unk = 0;
			pTeleportResult.MapNumber = lpObj.MapNumber;
			pTeleportResult.Unk1 = 0;
			pTeleportResult.MapX = lpObj.X;
			pTeleportResult.MapY = lpObj.Y;
			pTeleportResult.Dir = lpObj.Dir;
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pTeleportResult, pTeleportResult.h.size);
			return;
		}
	}

	if (DG_MAP_RANGE(lpObj.MapNumber) != FALSE)
	{
		if (g_DoppelGanger.GetDoppelgangerState() != 2)
		{
			PMSG_TELEPORT_RESULT pTeleportResult;

			pTeleportResult.h.c = 0xC3;
			pTeleportResult.h.size = sizeof(pTeleportResult);
			pTeleportResult.h.headcode = 0x1C;
			pTeleportResult.Type = 0;
			pTeleportResult.Unk = 0;
			pTeleportResult.MapNumber = lpObj.MapNumber;
			pTeleportResult.Unk1 = 0;
			pTeleportResult.MapX = lpObj.X;
			pTeleportResult.MapY = lpObj.Y;
			pTeleportResult.Dir = lpObj.Dir;
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pTeleportResult, pTeleportResult.h.size);
			return;
		}
	}

	if (lpMsg->MoveNumber == 0)
	{
		BYTE x = lpMsg->MapX;
		BYTE y = lpMsg->MapY;

		if (gObjCheckTeleportArea(aIndex, x, y) == FALSE)
		{

			PMSG_TELEPORT_RESULT pTeleportResult;

			pTeleportResult.h.c = 0xC3;
			pTeleportResult.h.size = sizeof(pTeleportResult);
			pTeleportResult.h.headcode = 0x1C;
			pTeleportResult.Type = 0;
			pTeleportResult.Unk = 0;
			pTeleportResult.MapNumber = lpObj.MapNumber;
			pTeleportResult.Unk1 = 0;
			pTeleportResult.MapX = lpObj.X;
			pTeleportResult.MapY = lpObj.Y;
			pTeleportResult.Dir = lpObj.Dir;

			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pTeleportResult, pTeleportResult.h.size);
			return;
		}

		if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
		{
			if (lpObj.MapNumber == MAP_INDEX_CASTLESIEGE)
			{
				if (g_CastleSiege.CheckTeleportMagicAxisY(lpObj.Y, x, y) == FALSE)
				{
					y = lpObj.Y;
				}
			}
		}

		CMagicInf * lpMagic;
		PMSG_MAGICATTACK_RESULT pAttack;
		PHeadSetBE((BYTE*)&pAttack, 0x19, sizeof(pAttack));
		pAttack.MagicNumberH = SET_NUMBERH(AT_SKILL_TELEPORT);
		pAttack.MagicNumberL = SET_NUMBERL(AT_SKILL_TELEPORT);
		pAttack.SourceNumberH = SET_NUMBERH(aIndex);
		pAttack.SourceNumberL = SET_NUMBERL(aIndex);
		pAttack.TargetNumberH = SET_NUMBERH(aIndex);
		pAttack.TargetNumberL = SET_NUMBERL(aIndex);

		lpMagic = gObjGetMagicSearch(&getGameObject(aIndex), AT_SKILL_TELEPORT);

		if (lpMagic)
		{
			int usemana = gObjMagicManaUse(&getGameObject(aIndex), lpMagic);

			if (usemana >= 0)
			{
				int usebp = gObjMagicBPUse(&getGameObject(aIndex), lpMagic);

				if (usebp >= 0)
				{
					if (lpObj.Type == OBJ_USER)
						IOCP.DataSend(lpObj.m_Index, (BYTE*)&pAttack, pAttack.h.size);

					MsgSendV2(&getGameObject(aIndex), (BYTE*)&pAttack, pAttack.h.size);
					gObjTeleportMagicUse(aIndex, x, y);
					lpObj.Mana = usemana;
					lpObj.BP = usebp;
					GCManaSend(lpObj.m_Index, lpObj.Mana, -1, 0, lpObj.BP);
				}
			}
		}
	}

	if (IMPERIAL_MAP_RANGE(lpObj.MapNumber) == TRUE)
	{
		int nCurrentZoneIndex = g_ImperialGuardian.GetCurrentZoneIndex(aIndex);

		if (nCurrentZoneIndex >= 0)
		{
			g_ImperialGuardian.CGEnterPortal(aIndex, nCurrentZoneIndex + 1);
		}
	}

	if (lpObj.m_IfState.use == 1 && lpObj.m_IfState.type == 6)
	{
		return;
	}

	if (gGateC.IsInGate(aIndex, lpMsg->MoveNumber))
	{
		gObjMoveGate(aIndex, lpMsg->MoveNumber);
		return;
	}

	gObjClearViewport(&getGameObject(aIndex));
	GCTeleportSend(&getGameObject(aIndex), lpMsg->MoveNumber, lpObj.MapNumber, lpObj.X, lpObj.Y, lpObj.Dir);

	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (lpObj.MapNumber == MAP_INDEX_CASTLESIEGE)
		{
			if (g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
			{
				g_CastleSiege.NotifySelfCsJoinSide(aIndex);
				g_CastleSiege.NotifyCsSelfLeftTime(aIndex);
			}
		}
	}
}
struct PMSG_RESULT_MOVEDEVILSQUARE
{
	PBMSG_HEAD2 h;	// C1:90
	BYTE Result;	// 3
};

void GameProtocol::CGReqMapMove(PMSG_REQ_MAPMOVE *lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
		return;

	CGameObject lpObj = Obj;

	PMSG_ANS_MAPMOVE pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0x8E, 0x03, sizeof(pMsg));

	pMsg.btResult = 0;

	if (lpObj.Type != OBJ_USER)
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (g_KeyGenerater.CheckKeyValue(lpObj.m_PlayerData->m_dwMapMoveKeyValue, lpMsg->dwBlockKey) == FALSE)
	{
		pMsg.btResult = 12;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (lpObj.Connected < PLAYER_PLAYING)
	{
		pMsg.btResult = 11;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (lpObj.Teleport != 0)
	{
		pMsg.btResult = 2;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (lpObj.m_PlayerData->m_bSecurityCheck == false)
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (lpObj.m_bMapSvrMoveReq == true || lpObj.m_bMapSvrMoveReq_2 == true || lpObj.m_bMapSvrMoveQuit == true || lpObj.m_State == 32)
	{
		return;
	}

	if (lpObj.m_bPShopOpen == true)
	{
		if (lpObj.MapNumber == MAP_INDEX_CASTLESIEGE && lpMsg->wMapIndex == MAP_INDEX_RORENCIA)
		{
			pMsg.btResult = 3;
			IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
			return;
		}

		if (lpObj.MapNumber == MAP_INDEX_RORENCIA && lpMsg->wMapIndex == MAP_INDEX_CASTLESIEGE)
		{
			pMsg.btResult = 3;
			IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
			return;
		}
	}

	if (lpObj.m_IfState.use == 1 && lpObj.m_IfState.type == 6)
	{
		pMsg.btResult = 2;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (lpObj.DieRegen == TRUE)
	{
		pMsg.btResult = 2;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (lpObj.RegenOk != FALSE)
	{
		pMsg.btResult = 2;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (pMsg.btResult)
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
	gMoveCommand.Move(lpObj, lpMsg->wMapIndex);
}

void GameProtocol::GCSendMapMoveCheckSum(CGameObject &Obj)
{
	PMSG_MAPMOVE_CHECKSUM pMsg;

	pMsg.dwKeyValue = lpObj.m_PlayerData->m_dwMapMoveKeyValue;
	PHeadSubSetB((BYTE*)&pMsg, 0x8E, 0x01, sizeof(pMsg));

	IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::CGTargetTeleportRecv(PMSG_TARGET_TELEPORT * lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(aIndex))
		return;

	if (gObjCheckUsedBuffEffect(&getGameObject(aIndex], BUFFTYPE_STONE) || gObjCheckUsedBuffEffect(&getGameObject(aIndex), BUFFTYPE_STUN) || gObjCheckUsedBuffEffect(&getGameObject(aIndex), BUFFTYPE_SLEEP) ||
		gObjCheckUsedBuffEffect(&getGameObject(aIndex], BUFFTYPE_FREEZE_2) || gObjCheckUsedBuffEffect(&getGameObject(aIndex), BUFFTYPE_EARTH_BINDS))
		return;

	CMagicInf * lpMagic;
	BYTE x;
	BYTE y;
	x = lpMsg->MapX;
	y = lpMsg->MapY;
	int iTargetIndex = MAKE_NUMBERW(lpMsg->NumberL, lpMsg->NumberH);	// #error? parameters inverse?

	if (!gObjIsConnectedGP(iTargetIndex))
	{
		return;
	}

	if (IT_MAP_RANGE(getGameObject(iTargetIndex)->MapNumber))
	{
		if (g_IT_Event.CheckRelics(iTargetIndex))
		{
			return;
		}
	}

	if (lpObj.PartyNumber != getGameObject(iTargetIndex)->PartyNumber ||
		lpObj.PartyNumber == -1 ||
		getGameObject(iTargetIndex)->PartyNumber == -1)
	{
		return;
	}

	if (!gObjCheckTeleportArea(iTargetIndex, x, y))
	{
		return;
	}

	PMSG_MAGICATTACK_RESULT pAttack;

	PHeadSetBE((BYTE*)&pAttack, 0x19, sizeof(pAttack));
	WORD MagicNumber = AT_SKILL_TARGET_TELEPORT;
	pAttack.MagicNumberH = SET_NUMBERH(MagicNumber);
	pAttack.MagicNumberL = SET_NUMBERL(MagicNumber);
	pAttack.SourceNumberH = SET_NUMBERH(aIndex);
	pAttack.SourceNumberL = SET_NUMBERL(aIndex);
	pAttack.TargetNumberH = SET_NUMBERH(iTargetIndex);	// #error? parameters inverse?
	pAttack.TargetNumberL = SET_NUMBERL(iTargetIndex);	// #error? parameters inverse?
	lpMagic = gObjGetMagicSearch(&getGameObject(aIndex), AT_SKILL_TARGET_TELEPORT);

	if (lpMagic)
	{
		int usemana = gObjMagicManaUse(&getGameObject(aIndex), lpMagic);

		if (usemana >= 0)
		{
			int usebp = gObjMagicBPUse(&getGameObject(aIndex), lpMagic);

			if (usebp >= 0)
			{
				if (lpObj.Type == OBJ_USER)
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pAttack, pAttack.h.size);

				MsgSendV2(&getGameObject(aIndex), (BYTE*)&pAttack, pAttack.h.size);
				gObjTeleportMagicUse(iTargetIndex, x, y);
				lpObj.Mana = usemana;
				lpObj.BP = usebp;
				GCManaSend(lpObj.m_Index, lpObj.Mana, -1, 0, lpObj.BP);

				return;
			}
		}
	}
}




void GameProtocol::GCTeleportSend(CGameObject &Obj, WORD MoveNumber, BYTE MapNumber, BYTE MapX, BYTE MapY, BYTE Dir)
{
	PMSG_TELEPORT_RESULT pMsg;

	if (lpObj.Type != OBJ_USER)
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
		IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (UCHAR*)&pMsg, pMsg.h.size);
	else
		IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (UCHAR*)&pMsg, pMsg.h.size);
}

#include "SkillHitBox.h"

void GameProtocol::CGBeattackRecv(BYTE* lpRecv, CGameObject &Obj, int magic_send)
{
	if (aIndex < 0 || aIndex > g_ConfigRead.server.GetObjectMax() - 1)
	{
		return;
	}

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
	// if aIndex is OBJ_USER
	// or if it is OBJ_MONSTER
	if (lpObj.Type == OBJ_USER)
	{
		BYTE attr = MapC[lpObj.MapNumber].GetAttr(lpObj.X, lpObj.Y);

		if ((attr & 1) == 1) //safezone
		{
			gObjSetPosition(aIndex, lpObj.X, lpObj.Y);
			return;
		}
		//season 4.5 add-on end

		WORD MagicNumber = MAKE_NUMBERW(lpCount->MagicNumberH, lpCount->MagicNumberL);

		lpMagic = gObjGetMagicSearch(&getGameObject(aIndex), MagicNumber);
	}
	else
	{
		WORD MagicNumber = MAKE_NUMBERW(lpCount->MagicNumberH, lpCount->MagicNumberL);

		lpMagic = gObjGetMagic(&getGameObject(aIndex), MagicNumber);
	}

	if (lpObj.Type == OBJ_USER && lpObj.m_PlayerData->m_bSecurityCheck == false)
		return;

	if (lpObj.Teleport && gObjUseSkill.m_SkillData.EnableWizardSkillWhileTeleport == false)
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

	if (lpObj.Type == OBJ_USER)
	{
		// Check if Player can use Magic
		// Acording to its Class
		if (MagicDamageC.SkillGetRequireClass(lpObj.Class,
			lpObj.m_PlayerData->ChangeUP,
			lpMagic->m_Skill) < 1)
		{
			// It's not his magic [%s][%s] .
			gObjUserKill(aIndex);
			return;
		}

		// Check the time cast Spell
		// No longer that 8 seconds
		if ((GetTickCount() - lpObj.UseMagicTime) > 8000)
		{
			sLog->outError( "Too long time passed after casting magic [%s][%s] (%d)(%d)",
				lpObj.AccountID, lpObj.Name, lpMagic->m_Skill, GetTickCount() - lpObj.UseMagicTime);

			return;
		}

		// Check if user cast to much Spells
		// Npo longer thant 4
		if (lpObj.UseMagicCount > 4)
		{
			return;
		}

		lpObj.UseMagicCount++;
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

		if (lpObj.OSAttackSerial >= 255 && (lpObj.OSAttackSerial - lpCount->Serial) > 50)
		{
			lpObj.OSAttackSerial = -1;
		}

		if (NSAttackSerial > lpObj.OSAttackSerial)
		{
			lpObj.OSAttackSerial = NSAttackSerial;
			
		}
	}

	if (lpMagic->m_Skill == AT_SKILL_EVIL ||
		g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == 385 ||
		g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == 487)
	{
		if (lpObj.Class != CLASS_WIZARD && lpObj.Class != CLASS_MAGUMSA)
		{
			return;
		}
	}

	if (bIsIgnorePacketSpeedHackDetect)
	{
		

		if (lpObj.m_SpeedHackPenalty > 0)
		{
			lpObj.m_SpeedHackPenalty--;
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
			int nTargetDistance = gObjCalDistanceTX(&getGameObject(pTargetNumber[0]], &getGameObject(pTargetNumber[i)));

			if (getGameObject(pTargetNumber[i])->Type == OBJ_MONSTER && nTargetDistance > 10)
			{
				sLog->outBasic("User Hack Detected [%s][%s] Skill: %d, Monster Distance: %d, Cnt:%d",
					lpObj.AccountID, lpObj.Name, lpMagic->m_Skill, nTargetDistance, lpCount->Count);
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
			if (lpObj.DurMagicKeyChecker->IsValidDurationTime(lpMsg->MagicKey) == FALSE)
			{
				lOfs += sizeof(PMSG_BEATTACK);

				continue;
			}

			if (lpObj.DurMagicKeyChecker->IsValidCount(lpMsg->MagicKey) == FALSE)
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

		//season4 add-on start
		if (ObjectMaxRange(tNumber) == false)
		{
			continue;
		}

		int DistanceCheck = gObjCalDistance(lpObj, &getGameObject(tNumber));

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
					//this->GCSendDisableReconnect(lpObj.m_Index);
					IOCP.CloseClient(lpObj.m_Index);
					return;
				}
			}
		}

		if (tNumber >= 0 && tNumber < g_ConfigRead.server.GetObjectMax() - 1)
		{
			if (lpMagic->m_Skill == AT_SKILL_DEFENSEDOWN)
			{
				gObjUseSkill.MaGumSkillDefenseDown(aIndex, tNumber, lpMagic->m_Level);
				gObjAttack(&getGameObject(aIndex], &getGameObject(tNumber), lpMagic, TRUE, 1, 0, FALSE, 0, 0);
			}

			else if (lpMagic->m_Skill == AT_SKILL_FIRESCREAM)
			{
				gObjUseSkill.SkillFireScream(aIndex, tNumber, lpMagic);
			}

			else if (g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == AT_MSKILL_MG_FIRESLASH1)
			{
				g_MasterLevelSkillTreeSystem.MLS_MaGumSkillDefenseDown(aIndex, tNumber);
				gObjAttack(&getGameObject(aIndex], &getGameObject(tNumber), lpMagic, TRUE, 1, 0, FALSE, 0, 0);
			}

			else if (g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == AT_MSKILL_MG_FIRESLASH2)
			{
				g_MasterLevelSkillTreeSystem.MLS_SkillMasteryDefenseDown(aIndex, tNumber, lpMagic);
				gObjAttack(&getGameObject(aIndex], &getGameObject(tNumber), lpMagic, TRUE, 1, 0, FALSE, 0, 0);
			}

			else if (lpMagic->m_Skill == AT_MSKILL_DL_FIRESCREAM1 || lpMagic->m_Skill == AT_MSKILL_DL_FIRESCREAM2)
			{
				g_MasterLevelSkillTreeSystem.MLS_FireScream(aIndex, tNumber, lpMagic);
			}
			else if (lpMagic->m_Skill == AT_SKILL_SHINING_PEAK)
			{
				gObjUseSkill.SkillShiningPeak(aIndex, lpMagic, tNumber);
			}
			else if (magic_send)
			{
				gObjAttack(&getGameObject(aIndex], &getGameObject(tNumber), lpMagic, TRUE, 1, 0, FALSE, 0, 0);
			}
			else
			{
				gObjAttack(&getGameObject(aIndex], &getGameObject(tNumber), lpMagic, FALSE, 1, 0, FALSE, 0, 0);
			}

		}

		lOfs += sizeof(PMSG_BEATTACK);
	}
}

void GameProtocol::CGDurationMagicRecv(PMSG_DURATION_MAGIC_RECV* lpMsg, CGameObject &Obj)
{
	if (aIndex < 0 || aIndex > g_ConfigRead.server.GetObjectMax() - 1)
	{
		return;
	}

	//PMSG_DURATION_MAGIC_RECV * lpMsg = (PMSG_DURATION_MAGIC_RECV *)(lpRecv);

	CMagicInf * lpMagic;
	

	if (lpObj.Teleport && gObjUseSkill.m_SkillData.EnableWizardSkillWhileTeleport == false)
	{
		return;
	}

	WORD MagicNumber = MAKE_NUMBERW(lpMsg->MagicNumberH, lpMsg->MagicNumberL);
	if (lpObj.Type == OBJ_USER)
	{
		lpMagic = gObjGetMagicSearch(lpObj, MagicNumber);
	}
	else
	{
		lpMagic = gObjGetMagic(lpObj, MagicNumber);
	}

	if (lpMagic == NULL)
	{
		return;
	}

	if (lpObj.Type == OBJ_USER)
	{
		if (MagicDamageC.CheckStatus(lpMagic->m_Skill, lpObj.m_PlayerData->GuildStatus) == FALSE)
		{
			return;
		}

		if (MagicDamageC.CheckKillCount(lpMagic->m_Skill, lpObj.m_btKillCount) < 0)
		{
			return;
		}

		if (lpObj.m_PlayerData->m_bSecurityCheck == false)
		{
			return;
		}
	}

	lpObj.UseMagicNumber = 0;

	if (lpMagic->m_Skill != 24)
	{
		if (!gObjUseSkill.SpeedHackCheck(aIndex))
		{
			return;
		}
	}

	if (!gObjUseSkill.EnableSkill(lpMagic->m_Skill))
	{
		return;
	}

	int aTargetIndex = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	gObjUseSkill.UseSkill(aIndex, lpMagic, lpMsg->X, lpMsg->Y, lpMsg->Dir, lpMsg->TargetPos, aTargetIndex);

	if (lpMsg->MagicKey)
		lpObj.DurMagicKeyChecker->SetDurationTime(lpMsg->MagicKey, GetTickCount());
}

void GameProtocol::CGUseItemRecv(PMSG_USEITEM* lpMsg, CGameObject &Obj)
{
	int pos;
	CItemObject * CItemObject;
	int iItemUseType = lpMsg->btItemUseType;

	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	// Check User States
	if (lpObj.m_IfState.use && lpObj.m_IfState.type != 3)
	{
		GCReFillSend(aIndex, lpObj.Life, 0xFD, 1, lpObj.iShield);

		return;
	}

	if (lpObj.m_PlayerData->m_bSecurityCheck == false)
	{
		GCReFillSend(aIndex, lpObj.Life, 0xFD, 1, lpObj.iShield);
		MsgOutput(aIndex, Lang.GetText(0, 822));
		return;
	}

	// Check if User is exiting
	if (lpObj.CloseType != -1)
	{
		GCReFillSend(aIndex, lpObj.Life, 0xFD, 1, lpObj.iShield);
		return;
	}

	// Fix Inventory Pointer
	if (gObjFixInventoryPointer(aIndex) == false)
	{
		sLog->outBasic("[Fix Inv.Ptr] False Location - %s, %d",
			__FILE__, __LINE__);
		//return;
	}

	// Check if there is a Transaction
	// Dupe
	if (lpObj.pTransaction == 1)
	{
		return;
	}

	if (g_NewPVP.IsObserver(getGameObject(aIndex)))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 319), aIndex, 1);
		return;
	}

	// Set Var Pos
	pos = lpMsg->inventoryPos;

	// Check if the pos is in bounds
	if (pos > MAIN_INVENTORY_SIZE - 1)
	{
		// error : Location is over the range. %s/%d
		GCReFillSend(aIndex, lpObj.Life, 0xFD, 1, lpObj.iShield);

		return;
	}

	// check if the item
	// have the same position
	// as the new position
	if (lpMsg->inventoryPos == lpMsg->invenrotyTarget)
	{
		GCReFillSend(aIndex, lpObj.Life, 0xFD, 1, lpObj.iShield);

		return;
	}

	// Set Pointer
	CItemObject = &lpObj.pInventory[pos];
	CItemObject->FixIt(); //K2
	//CItemObject.FixIt;
	// The item is ITEM
	if (CItemObject->IsItem())
	{
		// Check if item have 0 as Serial
		if (gObjCheckSerial0ItemList(CItemObject))
		{
			MsgOutput(aIndex, Lang.GetText(0, 259));

			GCReFillSend(aIndex, lpObj.Life, 0xFD, 1, lpObj.iShield);

			return;
		}

		// Check the item Serial
		if (gObjInventorySearchSerialNumber(&getGameObject(aIndex),
			CItemObject->GetNumber()) == FALSE)
		{
			GCReFillSend(aIndex, lpObj.Life, 0xFD, 1, lpObj.iShield);
			return;
		}

		if (CItemObject->m_serial && !gObjCanItemTouch(&getGameObject(aIndex), 1))
		{
			GCReFillSend(aIndex, lpObj.Life, 0xFD, 1, lpObj.iShield);
			return;
		}

		if (CItemObject->m_Type == ITEMGET(14, 0) || CItemObject->m_Type == ITEMGET(14, 1) || CItemObject->m_Type == ITEMGET(14, 2) || CItemObject->m_Type == ITEMGET(14, 3) || CItemObject->m_Type == ITEMGET(14, 133))
		{
			int potion;
			for (potion = 0; potion < g_Michi.FixHackPotions.m_LifePotionMultiplyCount; potion++)
			{
				if (potion == 0 || (rand() % 100) < g_Michi.FixHackPotions.m_LifePotionMultiplyRate)
				{
					g_Michi.UseHealingPotion(CItemObject, pos, aIndex);
				}
			}
		}
		else if (CItemObject->m_Type == ITEMGET(14, 4) ||	// Small Mana Potion
			CItemObject->m_Type == ITEMGET(14, 5) || CItemObject->m_Type == ITEMGET(14, 6))
		{
			int tMana = (CItemObject->m_Value * 10) - (lpObj.Level);	// #formula

			if (tMana < 0)
			{
				tMana = 0;
			}

			switch (CItemObject->m_Type)
			{
			case ITEMGET(14, 4):	// Small Mana Potion
				tMana += ((int)(lpObj.MaxMana + lpObj.AddMana)) * 20 / 100;
				break;
			case ITEMGET(14, 5):	// Mana Potion
				tMana += ((int)(lpObj.MaxMana + lpObj.AddMana)) * 30 / 100;
				break;
			case ITEMGET(14, 6):	// Large Mana Potion
				tMana += ((int)(lpObj.MaxMana + lpObj.AddMana)) * 40 / 100;
				break;
			}

			lpObj.Mana += tMana;

			if (lpObj.Mana > (lpObj.MaxMana + lpObj.AddMana - 1.0f))
				lpObj.Mana = lpObj.MaxMana + lpObj.AddMana;

			GCManaSend(aIndex, (WORD)lpObj.Mana, 0xFF, 0, lpObj.BP);

			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
		}
		else if (CItemObject->m_Type == ITEMGET(14, 36))
		{
			int iShieldGage = (lpObj.iMaxShield + lpObj.iAddShield) * 35 / 100;	// #formula

			if (lpObj.iShield < 0)
				lpObj.iShield = 0;

			if (lpObj.iFillShield > 0)
			{
				lpObj.iShield += lpObj.iFillShield;

				if (lpObj.iShield > (lpObj.iMaxShield + lpObj.iAddShield))
					lpObj.iShield = lpObj.iMaxShield + lpObj.iAddShield;
			}

			lpObj.iFillShieldCount = 2;
			lpObj.iFillShield = iShieldGage;
			lpObj.iFillShieldMax = iShieldGage;

			GCSendEffectInfo(aIndex, 3);

			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
		}
		else if (CItemObject->m_Type == ITEMGET(14, 35))
		{
			int iShieldGage = (lpObj.iMaxShield + lpObj.iAddShield) * 25 / 100;	// #formula

			if (lpObj.iShield < 0)
				lpObj.iShield = 0;

			if (lpObj.iFillShield > 0)
			{
				lpObj.iShield += lpObj.iFillShield;

				if (lpObj.iShield > (lpObj.iMaxShield + lpObj.iAddShield))
					lpObj.iShield = lpObj.iMaxShield + lpObj.iAddShield;
			}

			lpObj.iFillShieldCount = 2;
			lpObj.iFillShield = iShieldGage;
			lpObj.iFillShieldMax = iShieldGage;

			GCSendEffectInfo(aIndex, 3);

			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
		}
		else if (CItemObject->m_Type == ITEMGET(14, 37))
		{
			int iShieldGage = (lpObj.iMaxShield + lpObj.iAddShield) * 45 / 100; // #formula

			if (lpObj.iShield < 0)
				lpObj.iShield = 0;

			if (lpObj.iFillShield > 0)
			{
				lpObj.iShield += lpObj.iFillShield;

				if (lpObj.iShield > (lpObj.iMaxShield + lpObj.iAddShield))
					lpObj.iShield = lpObj.iMaxShield + lpObj.iAddShield;
			}

			lpObj.iFillShieldCount = 2;
			lpObj.iFillShield = iShieldGage;
			lpObj.iFillShieldMax = iShieldGage;

			GCSendEffectInfo(aIndex, 3);

			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
		}
		else if (CItemObject->m_Type == ITEMGET(14, 38))	// Small Compound Potion 
		{
			int iShieldGage = (lpObj.iMaxShield + lpObj.iAddShield) * 5 / 100;	// #formula
			int iHPGage = (int)((lpObj.MaxLife + lpObj.AddLife) * 10.0f / 100.0f);	// #formula

			if (lpObj.iShield < 0)
				lpObj.iShield = 0;

			if (lpObj.FillLife > 0.0f)
			{
				lpObj.Life += lpObj.FillLife;

				if (lpObj.Life > (lpObj.MaxLife + lpObj.AddLife))
					lpObj.Life = lpObj.MaxLife + lpObj.AddLife;
			}

			if (lpObj.iFillShield > 0)
			{
				lpObj.iShield += lpObj.iFillShield;

				if (lpObj.iShield > (lpObj.iMaxShield + lpObj.iAddShield))
					lpObj.iShield = lpObj.iMaxShield + lpObj.iAddShield;
			}

			lpObj.FillLife = (float)iHPGage;
			lpObj.FillLifeMax = (float)iHPGage;
			lpObj.FillLifeCount = 4;
			lpObj.iFillShield = iShieldGage;
			lpObj.iFillShieldMax = iShieldGage;
			lpObj.iFillShieldCount = 4;

			GCSendEffectInfo(aIndex, 3);

			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			sLog->outBasic("[%s][%s]Use Compound Potion Lv1 - SD[%d] HP[%f] -> SD[%d] HP[%f]",
				lpObj.AccountID, lpObj.Name, lpObj.iShield, lpObj.Life,
				lpObj.iShield + iShieldGage, (double)(lpObj.Life + (float)iHPGage));
		}
		else if (CItemObject->m_Type == ITEMGET(14, 39))	// Compound Potion 
		{
			int iShieldGage = (lpObj.iMaxShield + lpObj.iAddShield) * 10 / 100;	// #formula
			int iHPGage = (int)((lpObj.MaxLife + lpObj.AddLife) * 25.0f / 100.0f);	// #formula

			if (lpObj.iShield < 0)
				lpObj.iShield = 0;

			if (lpObj.FillLife > 0.0f)
			{
				lpObj.Life += lpObj.FillLife;

				if (lpObj.Life > (lpObj.MaxLife + lpObj.AddLife))
					lpObj.Life = lpObj.MaxLife + lpObj.AddLife;
			}

			if (lpObj.iFillShield > 0)
			{
				lpObj.iShield += lpObj.iFillShield;

				if (lpObj.iShield > (lpObj.iMaxShield + lpObj.iAddShield))
					lpObj.iShield = lpObj.iMaxShield + lpObj.iAddShield;
			}

			lpObj.FillLife = (float)iHPGage;
			lpObj.FillLifeMax = (float)iHPGage;
			lpObj.FillLifeCount = 4;
			lpObj.iFillShield = iShieldGage;
			lpObj.iFillShieldMax = iShieldGage;
			lpObj.iFillShieldCount = 4;

			GCSendEffectInfo(aIndex, 3);

			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			sLog->outBasic("[%s][%s]Use Compound Potion Lv2 - SD[%d] HP[%f] -> SD[%d] HP[%f]",
				lpObj.AccountID, lpObj.Name, lpObj.iShield, lpObj.Life,
				lpObj.iShield + iShieldGage, (double)(lpObj.Life + (float)iHPGage));
		}
		else if (CItemObject->m_Type == ITEMGET(14, 40))	// Large Compound Potion 
		{
			int iShieldGage = (lpObj.iMaxShield + lpObj.iAddShield) * 20 / 100;	// #formula
			int iHPGage = (int)((lpObj.MaxLife + lpObj.AddLife) * 45.0f / 100.0f);	// #formula

			if (lpObj.iShield < 0)
				lpObj.iShield = 0;

			if (lpObj.FillLife > 0.0f)
			{
				lpObj.Life += lpObj.FillLife;

				if (lpObj.Life > (lpObj.MaxLife + lpObj.AddLife))
					lpObj.Life = lpObj.MaxLife + lpObj.AddLife;
			}

			if (lpObj.iFillShield > 0)
			{
				lpObj.iShield += lpObj.iFillShield;

				if (lpObj.iShield > (lpObj.iMaxShield + lpObj.iAddShield))
					lpObj.iShield = lpObj.iMaxShield + lpObj.iAddShield;
			}

			lpObj.FillLife = (float)iHPGage;
			lpObj.FillLifeMax = (float)iHPGage;
			lpObj.FillLifeCount = 4;
			lpObj.iFillShield = iShieldGage;
			lpObj.iFillShieldMax = iShieldGage;
			lpObj.iFillShieldCount = 4;

			GCSendEffectInfo(aIndex, 3);

			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			sLog->outBasic("[%s][%s]Use Compound Potion Lv3 - SD[%d] HP[%f] -> SD[%d] HP[%f]",
				lpObj.AccountID, lpObj.Name,
				lpObj.iShield, lpObj.Life,
				lpObj.iShield + iShieldGage, (double)(lpObj.Life + (float)iHPGage));
		}
		else if (CItemObject->m_Type == ITEMGET(14, 8)) // Antidote
		{
			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			if (gObjCheckUsedBuffEffect(&getGameObject(aIndex), BUFFTYPE_POISON) == TRUE)
			{
				gObjRemoveBuffEffect(&getGameObject(aIndex), BUFFTYPE_POISON);
				lpObj.lpAttackObj = 0;
				GCMagicCancelSend(&getGameObject(aIndex), 1);
			}

			if (gObjCheckUsedBuffEffect(&getGameObject(aIndex), BUFFTYPE_FREEZE) == TRUE)
			{
				gObjRemoveBuffEffect(&getGameObject(aIndex), BUFFTYPE_FREEZE);
				lpObj.DelayActionTime = 0;
				lpObj.DelayLevel = 0;
				lpObj.lpAttackObj = 0;
				GCMagicCancelSend(&getGameObject(aIndex), 7);
			}
		}
		else if (CItemObject->m_Type == ITEMGET(14, 46))
		{
			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			gObjAddBuffEffect(&getGameObject(aIndex), BUFFTYPE_HALLOWEEN_BLESS);
		}
		else if (CItemObject->m_Type == ITEMGET(14, 47))
		{
			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			gObjAddBuffEffect(&getGameObject(aIndex), BUFFTYPE_HALLOWEEN_ANGER);
		}
		else if (CItemObject->m_Type == ITEMGET(14, 48))
		{
			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			gObjAddBuffEffect(&getGameObject(aIndex), BUFFTYPE_HALLOWEEN_CRY);
		}
		else if (CItemObject->m_Type == ITEMGET(14, 49))
		{
			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			gObjAddBuffEffect(&getGameObject(aIndex), BUFFTYPE_HALLOWEEN_FOOD);
		}
		else if (CItemObject->m_Type == ITEMGET(14, 50))
		{
			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			gObjAddBuffEffect(&getGameObject(aIndex), BUFFTYPE_HALLOWEEN_DRINK);
		}
		else if (CItemObject->m_Type == ITEMGET(14, 85))
		{
			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			gObjAddBuffEffect(&getGameObject(aIndex), BUFFTYPE_CHERRYBLOSSOM_DRINK);
		}
		else if (CItemObject->m_Type == ITEMGET(14, 86))
		{
			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			gObjAddBuffEffect(&getGameObject(aIndex), BUFFTYPE_CHERRYBLOSSOM_DUMPLING);
		}
		else if (CItemObject->m_Type == ITEMGET(14, 87))
		{
			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			gObjAddBuffEffect(&getGameObject(aIndex), BUFFTYPE_CHERRYBLOSSOM_PETAL);
		}
		else if (CItemObject->m_Type == ITEMGET(13, 81))
		{
			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			gObjAddBuffEffect(&getGameObject(aIndex), BUFFTYPE_CHARM_GUARDIAN);
		}
		else if (CItemObject->m_Type == ITEMGET(13, 82))
		{
			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			gObjAddBuffEffect(&getGameObject(aIndex), BUFFTYPE_CHARM_PROTECTITEM);
		}

		else if (CItemObject->m_Type == ITEMGET(13, 43))
		{
			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			gObjAddBuffEffect(&getGameObject(aIndex), BUFFTYPE_PCS_MARK1);
		}

		else if (CItemObject->m_Type == ITEMGET(13, 44))
		{
			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			gObjAddBuffEffect(&getGameObject(aIndex), BUFFTYPE_PCS_MARK2);
		}

		else if (CItemObject->m_Type == ITEMGET(13, 45))
		{
			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			gObjAddBuffEffect(&getGameObject(aIndex), BUFFTYPE_PCS_MARK3);
		}
		else if (g_kJewelOfHarmonySystem.IsJewelOfHarmonySmeltingItems(CItemObject->m_Type) == TRUE)
		{
			if (g_kJewelOfHarmonySystem.SmeltItemBySmeltingStone(&getGameObject(aIndex), lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE)
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemOneSend(aIndex, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
			else
			{
				GCReFillSend(aIndex, lpObj.Life, 0xFD, 1, lpObj.iShield);
			}
		}
		else if (CItemObject->m_Type == ITEMGET(12, 30))
		{
			if (UseBundleOfBlessJewel(&getGameObject(aIndex), lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE)
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemOneSend(aIndex, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
		}

#if( ENABLETEST_ITEMUPGRADEJEWELS == 1 )
		else if (g_ItemUpgradeJewels.IsJewel(CItemObject->m_Type))
		{
			g_ItemUpgradeJewels.ProcInsert(&getGameObject(aIndex), lpMsg->inventoryPos, lpMsg->invenrotyTarget);
		}
#else
		else if (g_kJewelOfHarmonySystem.IsJewelOfHarmonyPurity(CItemObject->m_Type) == TRUE && gSocketItemType.CheckSocketItemType(&lpObj.pInventory[lpMsg->invenrotyTarget]) == FALSE) //season4 add-on)
		{
			if (g_kJewelOfHarmonySystem.StrengthenItemByJewelOfHarmony(&getGameObject(aIndex), lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE)
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemOneSend(aIndex, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
			else
			{
				GCReFillSend(aIndex, lpObj.Life, 0xFD, 1, lpObj.iShield);
			}
		}
		else if (CItemObject->m_Type == ITEMGET(14, 13)) // Jewel Of Bless
		{
			if (gObjItemLevelUp(&getGameObject(aIndex), lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE)
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemOneSend(aIndex, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
		}
		else if (CItemObject->m_Type == ITEMGET(14, 14)) // Jewel Of Soul
		{
			if (gObjItemRandomLevelUp(&getGameObject(aIndex), lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE)
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemOneSend(aIndex, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
		}
		else if (CItemObject->m_Type == ITEMGET(14, 16)) // Jewel Of Life
		{
			if (gObjItemRandomOption3Up(&getGameObject(aIndex), lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE)
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemOneSend(aIndex, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
			else
			{
				GCReFillSend(aIndex, lpObj.Life, 0xFD, 1, lpObj.iShield);
			}
		}
#endif
		else if (CItemObject->m_Type == ITEMGET(14, 160))	//1.01.00 update
		{
			if (g_LuckyItemManager.LuckyItemRepaire(&getGameObject(aIndex), lpMsg->inventoryPos, lpMsg->invenrotyTarget))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemOneSend(aIndex, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
			else
			{
				GCReFillSend(aIndex, lpObj.Life, 0xFD, 1, lpObj.iShield);
			}
		}

		else if (CItemObject->m_Type == ITEMGET(14, 161))	//1.01.00 update
		{
			if (g_kJewelOfHarmonySystem.StrengthenItemByJewelOfRise(&getGameObject(aIndex), lpMsg->inventoryPos, lpMsg->invenrotyTarget))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemOneSend(aIndex, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
			else
			{
				GCReFillSend(aIndex, lpObj.Life, 0xFD, 1, lpObj.iShield);
			}
		}
		else if (CItemObject->m_Type == ITEMGET(14, 209)) // Tradeable Seal Season 8
		{
			if (g_PentagramSystem.AddTradeCount(&getGameObject(aIndex), lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE)
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemOneSend(aIndex, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			else
			{
				GCReFillSend(aIndex, lpObj.Life, 0xFD, 1, lpObj.iShield);
			}
		}

		else if (CItemObject->m_Type == ITEMGET(14, 224))
		{
			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			gObjAddBuffEffect(&getGameObject(aIndex), BUFFTYPE_LIGHT_BLESSING);
			//[K2]Light
		}

		else if (CItemObject->m_Type == ITEMGET(14, 263))
		{
			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
				//	gObjInventoryDeleteItem(aIndex, pos);

			}
			gObjAddBuffEffect(&getGameObject(aIndex), BUFFTYPE_LIGHT_BLESSING_LOW);
		}

		else if (CItemObject->m_Type == ITEMGET(14, 264))
		{
			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			gObjAddBuffEffect(&getGameObject(aIndex), BUFFTYPE_LIGHT_BLESSING_MIDDLE);
		}

		else if (CItemObject->m_Type >= ITEMGET(15, 0) ||	// Group 15 - Scrolls
			CItemObject->m_Type == ITEMGET(12, 7) || // Orb of Twisting Slash
			CItemObject->m_Type == ITEMGET(12, 289) || // 
			CItemObject->m_Type == ITEMGET(12, 290) || // 
			CItemObject->m_Type == ITEMGET(12, 291) || // 
			CItemObject->m_Type == ITEMGET(12, 292) || // 
			CItemObject->m_Type == ITEMGET(12, 293) || // 
			CItemObject->m_Type == ITEMGET(12, 295) || // 
			CItemObject->m_Type == ITEMGET(12, 296) || // 
			CItemObject->m_Type == ITEMGET(12, 297) || // 
			CItemObject->m_Type == ITEMGET(12, 298) || // 
			CItemObject->m_Type == ITEMGET(12, 299) || // 
			CItemObject->m_Type == ITEMGET(12, 300) || // 
			CItemObject->m_Type == ITEMGET(12, 303) || // 
			CItemObject->m_Type == ITEMGET(12, 305) || // 
			(CItemObject->m_Type >= ITEMGET(12, 8) && CItemObject->m_Type <= ITEMGET(12, 24)) ||	// Orbs
			CItemObject->m_Type == ITEMGET(12, 35) ||
			(CItemObject->m_Type >= ITEMGET(12, 44) && CItemObject->m_Type <= ITEMGET(12, 48)) ||	// Scroll of Fire Scream
			(CItemObject->m_Type >= ITEMGET(12, 271) && CItemObject->m_Type <= ITEMGET(12, 277))) // Grow Lancer Skills
		{

			if ((lpObj.m_PlayerData->Strength + lpObj.AddStrength) < CItemObject->m_RequireStrength)
			{
				GCReFillSend(aIndex, lpObj.Life, 0xFD, 1, lpObj.iShield);
				return;
			}

			if ((lpObj.m_PlayerData->Dexterity + lpObj.AddDexterity) < CItemObject->m_RequireDexterity)
			{
				GCReFillSend(aIndex, lpObj.Life, 0xFD, 1, lpObj.iShield);
				return;
			}

			if (CItemObject->m_Type == ITEMGET(15, 18))	// Scroll of HellBurst
			{
				if (g_QuestInfo.GetQuestState(&getGameObject(aIndex), 2) != 2)
				{
					GCReFillSend(aIndex, lpObj.Life, 0xFD, 1, lpObj.iShield);
					return;
				}
			}

			if (CItemObject->m_Type >= ITEMGET(12, 8) && CItemObject->m_Type <= ITEMGET(12, 24))
			{
				if (lpObj.Level < CItemObject->m_RequireLevel)	// Orbs
				{
					GCReFillSend(aIndex, lpObj.Life, 0xFD, 1, lpObj.iShield);
					return;
				}
			}

			WORD skillnumber;

			if (CItemObject->IsClass(lpObj.Class, lpObj.m_PlayerData->ChangeUP) == FALSE)
			{
				GCInventoryItemDeleteSend(aIndex, -1, 1);

			}
			else
			{
				int addskill = gObjMagicAdd(&getGameObject(aIndex), ITEM_GET_TYPE(CItemObject->m_Type), ITEM_GET_INDEX(CItemObject->m_Type), CItemObject->m_Level, skillnumber);

				if (addskill >= 0)
				{
					gObjInventoryItemSet(aIndex, pos, -1);
					GCMagicListOneSend(aIndex, addskill, skillnumber, CItemObject->m_Level, 0, 0);
					lpObj.pInventory[pos].Clear();
					GCInventoryItemDeleteSend(aIndex, pos, 1);
				}
				else
				{
					GCInventoryItemDeleteSend(aIndex, -1, 1);
				}
			}
		}
		else if (CItemObject->m_Type == ITEMGET(14, 10)) // Town Portal Scroll
		{
			if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
			{
				sLog->outBasic("[UBF][Return Scroll] [%s][%s][%s] - No Available",
					lpObj.AccountID, lpObj.Name, lpObj.m_PlayerData->m_RealNameOfUBF);

				return;
			}

			sLog->outBasic("[Using Item] [Return Scroll] [%s][%s] - Current Map:[%d]",
				lpObj.AccountID, lpObj.Name, lpObj.MapNumber);

			if (BC_MAP_RANGE(lpObj.MapNumber))
			{
				int iBridgeIndex = g_BloodCastle.GetBridgeIndex(lpObj.MapNumber);
				if (g_BloodCastle.GetCurrentState(iBridgeIndex) == 2)
				{
					g_BloodCastle.SearchUserDropQuestItem(aIndex);
				}
				else
				{
					g_BloodCastle.SearchUserDeleteQuestItem(aIndex);
				}
			}

			if (IT_MAP_RANGE(lpObj.MapNumber))
			{
				g_IT_Event.DropRelicsItem(lpObj.MapNumber, aIndex);
			}

			if (lpObj.m_IfState.use && lpObj.m_IfState.type == 3)
			{
				lpObj.TargetNpcNumber = -1;
				lpObj.m_IfState.type = 0;
				lpObj.m_IfState.use = 0;
			}

			gObjInventoryItemSet(aIndex, pos, -1);
			lpObj.pInventory[pos].Clear();
			GCInventoryItemDeleteSend(aIndex, pos, 1);

			if (lpObj.MapNumber == MAP_INDEX_DEVIAS)
			{
				gObjMoveGate(aIndex, 22);
			}
			else if (lpObj.MapNumber == MAP_INDEX_NORIA)
			{
				gObjMoveGate(aIndex, 27);
			}
			else if (lpObj.MapNumber == MAP_INDEX_ELBELAND)
			{
				gObjMoveGate(aIndex, 267);
			}
			else if (lpObj.MapNumber == MAP_INDEX_LOSTTOWER)
			{
				gObjMoveGate(aIndex, 42);
			}
			else if (lpObj.MapNumber == MAP_INDEX_ATHLANSE)
			{
				gObjMoveGate(aIndex, 49);
			}
			else if (lpObj.MapNumber == MAP_INDEX_TARKAN)
			{
				gObjMoveGate(aIndex, 57);
			}
			else if (BC_MAP_RANGE(lpObj.MapNumber) != FALSE)
			{
				gObjMoveGate(aIndex, 22);
			}
			else if (CC_MAP_RANGE(lpObj.MapNumber) != FALSE)
			{
				gObjMoveGate(aIndex, 22);
			}
			else if (lpObj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
			{
				gObjMoveGate(aIndex, 333);
			}
			else if (KALIMA_MAP_RANGE(lpObj.MapNumber) != FALSE)
			{
				gObjMoveGate(aIndex, 22);
			}
			else if (IT_MAP_RANGE(lpObj.MapNumber) != FALSE)
			{
				gObjMoveGate(aIndex, 267);
			}
			else if (lpObj.MapNumber == MAP_INDEX_AIDA)
			{
				gObjMoveGate(aIndex, 27);
			}
			else if (lpObj.MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE)
			{
				gObjMoveGate(aIndex, 27);
			}
			else if (lpObj.MapNumber == MAP_INDEX_BARRACKS)
			{
				gObjMoveGate(aIndex, 114);
			}
			else if (lpObj.MapNumber == MAP_INDEX_REFUGEE)
			{
				gObjMoveGate(aIndex, 114);
			}
			else if (lpObj.MapNumber == MAP_INDEX_CALMNESS)
			{
				gObjMoveGate(aIndex, 273);
			}
			else if (lpObj.MapNumber == MAP_INDEX_RAKLION || lpObj.MapNumber == MAP_INDEX_HATCHERY)
			{
				gObjMoveGate(aIndex, 287);
			}
			else if (lpObj.MapNumber == MAP_INDEX_SANTAVILLAGE)
			{
				gObjMoveGate(aIndex, 22);
			}
			else if (DG_MAP_RANGE(lpObj.MapNumber) != FALSE)
			{
				gObjMoveGate(aIndex, 267);
			}
			else if (IMPERIAL_MAP_RANGE(lpObj.MapNumber) != FALSE)
			{
				gObjMoveGate(aIndex, 22);
			}
			else if (lpObj.MapNumber == MAP_INDEX_LORENMARKET)
			{
				gObjMoveGate(aIndex, 333);
			}
			else if (lpObj.MapNumber == MAP_INDEX_FEREA)
			{
				gObjMoveGate(aIndex, 509);
			}
			else if (lpObj.MapNumber == MAP_INDEX_NIXIE_LAKE)
			{
				gObjMoveGate(aIndex, 522);
			}

			else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && lpObj.MapNumber == MAP_INDEX_CASTLESIEGE)
			{
				if (g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					if (lpObj.m_btCsJoinSide == 1)
					{
						gObjMoveGate(aIndex, 101);
					}
					else
					{
						gObjMoveGate(aIndex, 100);
					}
				}
				else
				{
					if (g_CastleSiege.CheckCastleOwnerMember(aIndex) == TRUE || g_CastleSiege.CheckCastleOwnerUnionMember(aIndex) == TRUE)
					{
						gObjMoveGate(aIndex, 101);
					}
					else
					{
						gObjMoveGate(aIndex, 100);
					}
				}
			}

			else if (g_NewPVP.IsVulcanusMap(lpObj.MapNumber))
			{
				gObjMoveGate(aIndex, 294);
			}

			else if (g_NewPVP.IsPKFieldMap(lpObj.MapNumber))
			{
				g_NewPVP.Reset(getGameObject(aIndex));
				gObjMoveGate(aIndex, 17);
			}

			else if (lpObj.MapNumber == MAP_INDEX_ARENA)
			{
			}

			else
			{
				gObjMoveGate(aIndex, 17);
			}

		}
		else if (CItemObject->m_Type == ITEMGET(13, 66))
		{
			if (DG_MAP_RANGE(lpObj.MapNumber) == TRUE)
			{
				g_DoppelGanger.LeaveDoppelganger(aIndex);

				if (g_DoppelGanger.GetDoppelgangerState() == 2)
				{
					g_DoppelGanger.SendDoppelgangerResult(&getGameObject(aIndex), 1);
				}
			}

			gObjTeleport(aIndex, MAP_INDEX_SANTAVILLAGE, 220, 20);

			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
		}

		else if (CItemObject->m_Type == ITEMGET(14, 162))
		{
			if (lpObj.m_PlayerData->m_InventoryExpansion < 2)
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
				lpObj.m_PlayerData->m_InventoryExpansion++;
				GDSetExpWare(aIndex, 1, lpObj.m_PlayerData->m_InventoryExpansion);

				PMSG_USEEXPANSIONITEM pMsg;
				PHeadSetB((BYTE*)&pMsg, 0x2B, sizeof(pMsg));
				pMsg.Result = 1;
				IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
			}
		}

		else if (CItemObject->m_Type == ITEMGET(14, 163))
		{
			if (lpObj.m_PlayerData->m_WarehouseExpansion < 1)
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
				GDSetExpWare(aIndex, 2, 1);
				lpObj.m_PlayerData->m_WarehouseExpansion = 1;

				PMSG_USEEXPANSIONITEM pMsg;
				PHeadSetB((BYTE*)&pMsg, 0x2B, sizeof(pMsg));
				pMsg.Result = 1;
				IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
			}
		}

		else if (CItemObject->m_Type == ITEMGET(13, 152) || CItemObject->m_Type == ITEMGET(13, 156))
		{
			if (g_MasterLevelSkillTreeSystem.CheckMasterSkillPoint(aIndex, 1) == TRUE)
			{
				g_MasterLevelSkillTreeSystem.ResetMasterSkill(aIndex, 1);
				sLog->outBasic("[MasterLevelSkill Reset] - Passive (%d)", aIndex);
				gObjCloseSet(aIndex, 1);
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			else
			{
				this->GCServerMsgStringSend(Lang.GetText(0, 526), aIndex, 1);
			}
		}

		else if (CItemObject->m_Type == ITEMGET(13, 153) || CItemObject->m_Type == ITEMGET(13, 157))
		{
			if (g_MasterLevelSkillTreeSystem.CheckMasterSkillPoint(aIndex, 2) == TRUE)
			{
				g_MasterLevelSkillTreeSystem.ResetMasterSkill(aIndex, 2);
				sLog->outBasic("[MasterLevelSkill Reset] - Strengthen Skill (%d)", aIndex);
				gObjCloseSet(aIndex, 1);
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			else
			{
				this->GCServerMsgStringSend(Lang.GetText(0, 526), aIndex, 1);
			}
		}

		else if (CItemObject->m_Type == ITEMGET(13, 154) || CItemObject->m_Type == ITEMGET(13, 158))
		{
			if (g_MasterLevelSkillTreeSystem.CheckMasterSkillPoint(aIndex, 3) == TRUE)
			{
				g_MasterLevelSkillTreeSystem.ResetMasterSkill(aIndex, 3);
				sLog->outBasic("[MasterLevelSkill Reset] - Enhance Attack/Defense (%d)", aIndex);
				gObjCloseSet(aIndex, 1);
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			else
			{
				this->GCServerMsgStringSend(Lang.GetText(0, 526), aIndex, 1);
			}
		}

		else if (CItemObject->m_Type == ITEMGET(13, 155) || CItemObject->m_Type == ITEMGET(13, 159))
		{
			if (g_MasterLevelSkillTreeSystem.CheckMasterSkillPoint(aIndex, 0) == TRUE)
			{
				g_MasterLevelSkillTreeSystem.ResetMasterSkill(aIndex, 0);
				sLog->outBasic("[MasterLevelSkill Reset] - All (%d)", aIndex);
				gObjCloseSet(aIndex, 1);
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			else
			{
				this->GCServerMsgStringSend(Lang.GetText(0, 526), aIndex, 1);
			}
		}

		else if (CItemObject->m_Type == ITEMGET(14, 9)) // Ale
		{
			int level = CItemObject->m_Level;

			gObjInventoryItemSet(aIndex, pos, -1);
			lpObj.pInventory[pos].Clear();
			GCInventoryItemDeleteSend(aIndex, pos, 1);
			gObjUseDrink(&getGameObject(aIndex), level);
		}
		else if (CItemObject->m_Type == ITEMGET(14, 20)) // Remedy of Love
		{
			if (CItemObject->m_Level == 0)
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
				gObjUseDrink(&getGameObject(aIndex), 2);
			}
		}
		else if (CItemObject->m_Type == ITEMGET(13, 15)) // Fruits
		{
			if (iItemUseType == 0)
			{
				gObjUsePlusStatFruit(aIndex, pos);
			}
			else if (iItemUseType == 1)
			{
				gObjUseMinusStatFruit(aIndex, pos);
			}
		}
		else if (CItemObject->m_Type == ITEMGET(13, 70))
		{
			if (lpObj.SaveMapNumber != -1)
			{
				bool bMove = gMoveCommand.CheckEquipmentToMove(&getGameObject(aIndex), lpObj.SaveMapNumber);

				if (bMove == true && MapNumberCheck(lpObj.SaveMapNumber))
				{
					gObjTeleport(aIndex, lpObj.SaveMapNumber, lpObj.SaveX, lpObj.SaveY);
					gObjInventoryItemSet(aIndex, pos, -1);
					lpObj.pInventory[pos].Clear();
					GCInventoryItemDeleteSend(aIndex, pos, 1);
					lpObj.SaveMapNumber = -1;
					GCServerCmd(aIndex, 61, 0, 0);
				}
			}
			else
			{
				if (gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
				{
					lpObj.SaveMapNumber = lpObj.MapNumber;
					lpObj.SaveX = lpObj.X;
					lpObj.SaveY = lpObj.Y;
				}
				else
				{
					gObjInventoryItemSet(aIndex, pos, -1);
					lpObj.pInventory[pos].Clear();
					GCInventoryItemDeleteSend(aIndex, pos, 1);
				}
			}
		}
		else if (CItemObject->m_Type == ITEMGET(13, 69))
		{
			if (lpObj.DieMapNumber != -1)
			{
				bool bMove = gMoveCommand.CheckEquipmentToMove(&getGameObject(aIndex), lpObj.DieMapNumber);

				bool bGens = g_GensSystem.IsMapBattleZone(lpObj.MapNumber);

				if (bMove == true && bGens == false && MapNumberCheck(lpObj.DieMapNumber))
				{
					gObjTeleport(aIndex, lpObj.DieMapNumber, lpObj.DieX, lpObj.DieY);
					gObjInventoryItemSet(aIndex, pos, -1);
					lpObj.pInventory[pos].Clear();
					GCInventoryItemDeleteSend(aIndex, pos, 1);
					lpObj.DieMapNumber = -1;
					GCServerCmd(aIndex, 60, 0, 0);
				}
			}
		}
		else if (CItemObject->m_Type >= ITEMGET(13, 54) && CItemObject->m_Type <= ITEMGET(13, 58))
		{
			CashShopExMinusStatFruit(aIndex, pos);
		}
		else if (CItemObject->m_Type == ITEMGET(14, 70)) // CSHOP HP Potions
		{
			int tLife = (CItemObject->m_Value * 10) - (lpObj.Level);	// #formula

			if (tLife < 0)
			{
				tLife = 0;
			}

			tLife += ((int)(lpObj.MaxLife + lpObj.AddLife));	// #formula
			lpObj.Life += tLife;

			if (lpObj.Life > (lpObj.MaxLife + lpObj.AddLife - 1.0f))
				lpObj.Life = lpObj.MaxLife + lpObj.AddLife;

			GCReFillSend(aIndex, lpObj.Life, 0xFD, 1, lpObj.iShield);

			lpObj.FillLifeMax = tLife;
			lpObj.FillLife = tLife;

			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
		}
		else if (CItemObject->m_Type == ITEMGET(14, 71)) // CSHOP MP Potions
		{
			int tMana = (CItemObject->m_Value * 10) - (lpObj.Level);	// #formula

			if (tMana < 0)
			{
				tMana = 0;
			}

			tMana += ((int)(lpObj.MaxMana + lpObj.AddMana));
			lpObj.Mana += tMana;

			if (lpObj.Mana > (lpObj.MaxMana + lpObj.AddMana - 1.0f))
				lpObj.Mana = lpObj.MaxMana + lpObj.AddMana;

			GCManaSend(aIndex, lpObj.Mana, 0xFF, 0, lpObj.BP);

			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
		}
		else if (CItemObject->m_Type == ITEMGET(14, 133))
		{
			gObjUseRecoveryPotion(&getGameObject(aIndex), pos, (double)0.65);
		}
		else if (CItemObject->m_Type == ITEMGET(14, 78)) // CSHOP Arcanum For?a
		{
			LPITEMEFFECT lpEffect = g_ItemAddOption.GetItemEffectData(CItemObject->m_Type);

			if (lpEffect == NULL)
			{
				GCReFillSend(aIndex, lpObj.Life, 0xFD, 1, lpObj.iShield);
				return;
			}

			g_PeriodItemEx.SetPeriodItemInfo(&getGameObject(aIndex), CItemObject->m_Type, 0, lpEffect->iEffectValidTime);
			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
		}
		else if (CItemObject->m_Type == ITEMGET(14, 79)) // CSHOP Arcanum Agilidade
		{
			LPITEMEFFECT lpEffect = g_ItemAddOption.GetItemEffectData(CItemObject->m_Type);

			if (lpEffect == NULL)
			{
				GCReFillSend(aIndex, lpObj.Life, 0xFD, 1, lpObj.iShield);
				return;
			}

			g_PeriodItemEx.SetPeriodItemInfo(&getGameObject(aIndex), CItemObject->m_Type, 0, lpEffect->iEffectValidTime);
			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
		}
		else if (CItemObject->m_Type == ITEMGET(14, 80)) // CSHOP Arcanum Vitalidade
		{
			LPITEMEFFECT lpEffect = g_ItemAddOption.GetItemEffectData(CItemObject->m_Type);

			if (lpEffect == NULL)
			{
				GCReFillSend(aIndex, lpObj.Life, 0xFD, 1, lpObj.iShield);
				return;
			}

			g_PeriodItemEx.SetPeriodItemInfo(&getGameObject(aIndex), CItemObject->m_Type, 0, lpEffect->iEffectValidTime);
			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
		}
		else if (CItemObject->m_Type == ITEMGET(14, 81)) // CSHOP Arcanum Energia
		{
			LPITEMEFFECT lpEffect = g_ItemAddOption.GetItemEffectData(CItemObject->m_Type);

			if (lpEffect == NULL)
			{
				GCReFillSend(aIndex, lpObj.Life, 0xFD, 1, lpObj.iShield);
				return;
			}

			g_PeriodItemEx.SetPeriodItemInfo(&getGameObject(aIndex), CItemObject->m_Type, 0, lpEffect->iEffectValidTime);
			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
		}
		else if (CItemObject->m_Type == ITEMGET(14, 82)) // CSHOP Arcanum Comando
		{
			LPITEMEFFECT lpEffect = g_ItemAddOption.GetItemEffectData(CItemObject->m_Type);

			if (lpEffect == NULL)
			{
				GCReFillSend(aIndex, lpObj.Life, 0xFD, 1, lpObj.iShield);
				return;
			}

			g_PeriodItemEx.SetPeriodItemInfo(&getGameObject(aIndex), CItemObject->m_Type, 0, lpEffect->iEffectValidTime);
			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
		}
		else if (CItemObject->m_Type == ITEMGET(13, 60)) // CSHOP PK Free Ticket
		{
			if (lpObj.m_PK_Level == 6)
			{
				lpObj.m_PK_Level = 5;
			}
			else if (lpObj.m_PK_Level == 5)
			{
				lpObj.m_PK_Level = 4;
			}
			else if (lpObj.m_PK_Level == 4)
			{
				lpObj.m_PK_Level = 3;
			}
			else if (lpObj.m_PK_Level <= 3)
			{
				lpObj.m_PK_Level = lpObj.m_PK_Level;
			}

			GCPkLevelSend(aIndex, lpObj.m_PK_Level);

			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}
			sLog->outBasic("[PCS] [%s][%s] Use PK Clear Item", lpObj.AccountID, lpObj.Name);
		}
		else if (CItemObject->m_Type == ITEMGET(14, 7))	// Siege Potion
		{
			if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
			{
				gObjInventoryItemSet(aIndex, pos, -1);
				lpObj.pInventory[pos].Clear();
				GCInventoryItemDeleteSend(aIndex, pos, 1);
			}

			int iLevel = CItemObject->m_Level;
			int iSerial = CItemObject->GetNumber();
			int iDur = CItemObject->m_Durability;

			if (lpObj.m_PlayerData->lpGuild)
			{
				sLog->outBasic("[CastleSiege] Using CastleSiege Potion Lv[%d]/Si[%d]/Dur[%d] - [%s][%s] - (Guild : %s)",
					iLevel, iSerial, iDur, lpObj.AccountID, lpObj.Name, lpObj.m_PlayerData->lpGuild->Name);
			}
			else
			{
				sLog->outBasic("[CastleSiege] Using CastleSiege Potion Lv[%d]/Si[%d]/Dur[%d] - [%s][%s]",
					iLevel, iSerial, iDur, lpObj.AccountID, lpObj.Name);
			}

			if (iLevel == 0)
			{
				gObjUseBlessAndSoulPotion(aIndex, iLevel);
			}
			else if (iLevel == 1)
			{
				gObjUseBlessAndSoulPotion(aIndex, iLevel);
			}
		}
		else if (CItemObject->m_Type == ITEMGET(13, 48))
		{
			if ((lpObj.MapNumber < MAP_INDEX_KALIMA1 || lpObj.MapNumber > MAP_INDEX_KALIMA6) && lpObj.MapNumber != MAP_INDEX_KALIMA7)
			{
				int iLevel = g_KalimaGate.GetKalimaGateLevel2(aIndex);
				int iSerial = CItemObject->GetNumber();
				int iDuration = CItemObject->m_Durability;
				BOOL bKalimaGateCreateResult = FALSE;
				int iKalimaGateX = 0;
				int iKalimaGateY = 0;

				iLevel++;
				iKalimaGateX = lpObj.X + rand() % 6 - 2;
				iKalimaGateY = lpObj.Y + rand() % 6 - 2;

				bKalimaGateCreateResult = g_KalimaGate.CreateKalimaGate(aIndex, iLevel, iKalimaGateX, iKalimaGateY);

				if (bKalimaGateCreateResult == TRUE)
				{
					if (!gObjSearchItemMinus(&getGameObject(aIndex), pos, 1))
					{
						gObjInventoryItemSet(aIndex, pos, -1);
						lpObj.pInventory[pos].Clear();
						GCInventoryItemDeleteSend(aIndex, pos, 1);
					}

					sLog->outBasic("[PCS] Use Free Kalima Ticket Success");
				}
				else
				{
					sLog->outBasic("[PCS] Use Free Kalima Ticket Failed");
				}
			}
		}

		else if (g_QuestExpManager.IsQuestItemAtt(CItemObject->m_Type, 64) == true)
		{
			int Ep = g_QuestExpManager.GetQuestItemEp(CItemObject->m_Type);
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
					g_Generic_Call(L, "ItemUseQuest", "ii>i", aIndex, Ep, &iResult);

					if (iResult)
					{
						gObjInventoryItemSet(aIndex, pos, -1);
						lpObj.pInventory[pos].Clear();
						GCInventoryItemDeleteSend(aIndex, pos, 1);
					}
				}
			}
		}
		else
		{
			sLog->outBasic("error-L3 : %s return %s %d %d %s",
				lpObj.Name, __FILE__, __LINE__, pos, lpObj.pInventory[pos].GetName());

			GCReFillSend(aIndex, lpObj.Life, 0xFD, 1, lpObj.iShield);
		}
	}
	else
	{
		GCReFillSend(aIndex, lpObj.Life, 0xFD, 1, lpObj.iShield);
		sLog->outBasic("error-L3 : %s return %s %d %d",
			lpObj.Name, __FILE__, __LINE__, pos);
	}
}


struct PMSG_REFILL
{
	PBMSG_HEAD2 h;	// C1:26
	BYTE IPos;	// 3
	BYTE LifeH;	// 4
	BYTE LifeL;	// 5
	BYTE Flag;	// 6
	BYTE btShieldH;	// 7
	BYTE btShieldL;	// 8
	int IGCLife;
	int IGCPenta;
};



void GameProtocol::GCReFillSend(CGameObject &Obj, int Life, BYTE Ipos, BYTE flag, WORD wShield)
{
	PMSG_REFILL pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x26, sizeof(pMsg));
	pMsg.IPos = Ipos;
	pMsg.LifeH = SET_NUMBERH(Life);
	pMsg.LifeL = SET_NUMBERL(Life);
	pMsg.btShieldH = SET_NUMBERH(wShield);
	pMsg.btShieldL = SET_NUMBERL(wShield);
	pMsg.Flag = flag;	// #error Flag is Disabled
	pMsg.IGCLife = Life;
	pMsg.IGCPenta = wShield;
	IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);
}


struct PMSG_MANASEND
{
	PBMSG_HEAD2 h;	// C1:27
	BYTE IPos;	// 3
	BYTE ManaH;	// 4
	BYTE ManaL;	// 5
	BYTE BPH;	// 6
	BYTE BPL;	// 7
	int IGCMana;
};

void GameProtocol::GCManaSend(CGameObject &Obj, int Mana, BYTE Ipos, BYTE flag, WORD BP)
{
	if (aIndex < 0 || aIndex > g_ConfigRead.server.GetObjectMax() - 1)
	{
		return;
	}

	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	PMSG_MANASEND pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x27, sizeof(pMsg));
	pMsg.IPos = Ipos;
	pMsg.ManaH = SET_NUMBERH(Mana);
	pMsg.ManaL = SET_NUMBERL(Mana);
	pMsg.BPH = SET_NUMBERH(BP);
	pMsg.BPL = SET_NUMBERL(BP);
	pMsg.IGCMana = Mana;
	IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);
}


struct PMSG_INVENTORYDELETE
{
	PBMSG_HEAD2 h;	// C1:28
	BYTE IPos;	// 3
	BYTE Flag;	// 4
};

void GameProtocol::GCInventoryItemDeleteSend(CGameObject &Obj, BYTE pos, BYTE flag)
{
	PMSG_INVENTORYDELETE pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x28, sizeof(pMsg));
	pMsg.IPos = pos;
	pMsg.Flag = flag;

	IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);
}

struct PMSG_ITEMUSESPECIALTIME
{
	PBMSG_HEAD2 h;	// C1:29
	BYTE Number;	// 3
	WORD Time;	// 4
};

void GameProtocol::GCItemObjectUseSpecialTimeSend(CGameObject &Obj, BYTE number, int time)
{
	if (time > 65535)
	{
		time = 65535;
	}

	PMSG_ITEMUSESPECIALTIME pMsg;

	PHeadSetBE((BYTE*)&pMsg, 0x29, sizeof(pMsg));
	pMsg.Number = number;
	pMsg.Time = time;

	IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);
}

struct PMSG_ITEMDUR
{
	PBMSG_HEAD2 h;	// C1:2A
	BYTE IPos;	// 3
	BYTE Dur;	// 4
	BYTE Flag;	// 5
};


void GameProtocol::GCItemObjectDurSend(CGameObject &Obj, BYTE pos, BYTE dur, BYTE flag)
{
	PMSG_ITEMDUR pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x2A, sizeof(pMsg));
	pMsg.IPos = pos;
	pMsg.Dur = dur;
	pMsg.Flag = flag;

	IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);
}



void GameProtocol::GCItemObjectDurSend2(CGameObject &Obj, BYTE pos, BYTE dur, BYTE flag)
{
	PMSG_ITEMDUR pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x2A, sizeof(pMsg));
	pMsg.IPos = pos;
	pMsg.Dur = dur;
	pMsg.Flag = flag;

	IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);
}


struct PMSG_WEATHER
{
	PBMSG_HEAD2 h;	// C1:0F
	BYTE Weather;	// 3
};

void GameProtocol::CGWeatherSend(CGameObject &Obj, BYTE weather)
{
	PMSG_WEATHER pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x0F, sizeof(pMsg));
	pMsg.Weather = weather;

	IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);
}



void GameProtocol::GCServerCmd(CGameObject &Obj, BYTE type, BYTE Cmd1, BYTE Cmd2)
{
	PMSG_SERVERCMD ServerCmd;

	PHeadSubSetB((BYTE*)&ServerCmd, 0xF3, 0x40, sizeof(ServerCmd));
	ServerCmd.CmdType = type;
	ServerCmd.X = Cmd1;
	ServerCmd.Y = Cmd2;

	IOCP.DataSend(lpObj.m_Index, (UCHAR*)&ServerCmd, ServerCmd.h.size);
}




void GameProtocol::GCReqmoveDevilSquare(PMSG_REQ_MOVEDEVILSQUARE* lpMsg, CGameObject &Obj)
{
	if (aIndex < 0 || aIndex > g_ConfigRead.server.GetObjectMax() - 1)
	{
		return;
	}

	BYTE cSquareNumber = lpMsg->SquareNumber;
	BYTE cInvitationItemPos = lpMsg->InvitationItemPos - INVENTORY_BAG_START;

	if (lpObj.m_IfState.use && lpObj.m_IfState.type != 12)
		return;

	PMSG_RESULT_MOVEDEVILSQUARE pResult;

	PHeadSetB((BYTE*)&pResult, 0x90, sizeof(pResult));
	pResult.Result = 0;

	

	BOOL bPlayerKiller = FALSE; //season 2.5 add-on

	if (lpObj.PartyNumber >= 0) //season 2.5 add-on
	{
		if ((gParty.GetPKPartyPenalty(lpObj.PartyNumber)) > 4)
		{
			bPlayerKiller = TRUE;
		}
	}

	else
	{
		if (lpObj.m_PK_Level > 4)
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
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (MAIN_INVENTORY_RANGE(cInvitationItemPos) == FALSE)
	{
		pResult.Result = 1;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if (DS_LEVEL_RANGE(cSquareNumber) == FALSE)
	{
		pResult.Result = 1;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if (g_DevilSquare.GetState() != DevilSquare_OPEN)
	{
		pResult.Result = 2;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if (g_DevilSquare.GetObjCount(cSquareNumber) >= g_DevilSquare.GetMaxUserInSquare())
	{
		pResult.Result = 5;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	CItemObject * sitem = &lpObj.pInventory[cInvitationItemPos];

	if (sitem->m_Type != ITEMGET(14, 19) && (sitem->m_Type != ITEMGET(13, 46) || sitem->m_Durability <= 0.0f))	// Devil's Invitation
	{
		pResult.Result = 1;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

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
				IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

				return;
			}
		}
	}

	BOOL bEnterCheck = FALSE;
	WORD movegateindex = 61;	// #gate
	int iENTER_LEVEL = g_DevilSquare.GetUserLevelToEnter(aIndex, movegateindex);

	if (DS_LEVEL_RANGE(iENTER_LEVEL) == FALSE)
	{
		return;
	}

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
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if (bEnterCheck == 2)
	{
		pResult.Result = 4;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if (sitem->m_Type == ITEMGET(14, 19) || (sitem->m_Type == ITEMGET(13, 46) && sitem->m_Durability == 1.0f))
	{
		gObjInventoryDeleteItem(aIndex, cInvitationItemPos);
		GCInventoryItemDeleteSend(aIndex, cInvitationItemPos, 1);
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
	}
	else if (sitem->m_Type == ITEMGET(13, 46) && sitem->m_Durability > 1.0f)
	{
		sitem->m_Durability -= 1.0f;
		GCItemObjectDurSend2(lpObj.m_Index, cInvitationItemPos, sitem->m_Durability, 0);
	}

	lpObj.m_nEventExp = 0;
	lpObj.m_nEventScore = 0;
	lpObj.m_nEventMoney = 0;
	lpObj.m_bDevilSquareIndex = g_DevilSquare.GetDevilSquareIndex(movegateindex);
	lpObj.m_bDevilSquareAuth = true;
	lpObj.m_IfState.state = 0;
	lpObj.m_IfState.type = 0;
	lpObj.m_IfState.use = 0;

	gObjMoveGate(lpObj.m_Index, movegateindex);
	g_DevilSquare.IncObjCount(cSquareNumber);
}



struct PMSG_RESULT_DEVILSQUARE_REMAINTIME
{
	PBMSG_HEAD2 h;	// C1:91
	BYTE hEventType;	// 3
	BYTE RemainTime;	// 4
	BYTE EnteredUser;	// 5
	BYTE RemainTime_LOW;	// 6
};


void GameProtocol::GCReqDevilSquareRemainTime(PMSG_REQ_DEVILSQUARE_REMAINTIME* lpMsg, CGameObject &Obj)
{
	PMSG_RESULT_DEVILSQUARE_REMAINTIME pResult;

	PHeadSetB((BYTE*)&pResult, 0x91, sizeof(pResult));
	pResult.RemainTime = 0;
	int iENTER_LEVEL;

	switch (lpMsg->hEventType)
	{
	case 1:
		if (lpObj.m_PK_Level >= 4 && g_ConfigRead.data.common.PKEnterEventAllow == false)
		{
			GCServerCmd(aIndex, 0x37, 0, 0);

			return;
		}

		pResult.hEventType = 1;

		if (g_DevilSquare.GetState() == DevilSquare_CLOSE)
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
		}
		break;

	case 2:
		if (lpObj.m_PK_Level >= 4 && g_ConfigRead.data.common.PKEnterEventAllow == false)
		{
			GCServerCmd(aIndex, 0x38, 0, 0);

			return;
		}

		pResult.hEventType = 2;

		if (g_BloodCastle.GetCurrentState(lpMsg->btItemLevel) == 1)
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
		}
		break;

	case 4:
		if (lpObj.m_PK_Level >= 4 && g_ConfigRead.data.common.PKEnterEventAllow == false)
		{
			GCServerCmd(aIndex, 0x39, 0, 0);

			return;
		}

		iENTER_LEVEL = g_ChaosCastle.GetUserLevelToEnter(aIndex);

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
		}
		break;
	case 5:
		pResult.hEventType = 5;
		pResult.RemainTime = g_IT_Event.GetRemainTime();
		pResult.RemainTime_LOW = 0;
		pResult.EnteredUser = 0;
		break;
	case 6:
		if (lpObj.m_PK_Level < 4)
		{
			pResult.hEventType = 6;
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		}
		else
		{
			GCServerCmd(aIndex, 0x39, 0, 0);
			return;
		}
	}

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
}



void GameProtocol::AllSendMsg(BYTE* Msg, int size)
{
	for (int n = g_ConfigRead.server.GetObjectStartUserIndex(); n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		if (getGameObject(n)->Connected == PLAYER_PLAYING)
		{
			if (getGameObject(n)->Type == OBJ_USER)
			{
				IOCP.DataSend(n, Msg, size);
			}
		}
	}
}



void GameProtocol::AllSendSameMapMsg(UCHAR * Msg, int size, BYTE mapnumber)
{
	for (int n = 0; n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		if (getGameObject(n)->Connected == PLAYER_PLAYING)
		{
			if (getGameObject(n)->Type == OBJ_USER)
			{
				if (getGameObject(n)->MapNumber == mapnumber)
				{
					IOCP.DataSend(n, Msg, size);
				}
			}
		}
	}
}


struct PMSG_PING
{
	PBMSG_HEAD2 h;	// C1:71
};


//#unused
void GameProtocol::GCSendPing(CGameObject &Obj)
{
	PMSG_PING pMsgPing;

	PHeadSetB((BYTE*)&pMsgPing, 0x71, sizeof(pMsgPing));
	lpObj.iPingTime = GetTickCount();

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsgPing, pMsgPing.h.size);
}


//#unused
void GameProtocol::GCPingSendRecv(PMSG_PING_RESULT* aRecv, CGameObject &Obj)
{
	int PingTime = GetTickCount() - lpObj.iPingTime;

	MsgOutput(aIndex, "Ping: %d ms", PingTime);
}



struct PMSG_REQ_REGISTER_EVENTCHIP
{
	PBMSG_HEAD2 h;	// C1:02
	int iINDEX;	// 4
	BYTE Pos;	// 8
	char szUID[11];	// 9
};

struct PMSG_REQ_REGISTER_STONES
{
	PBMSG_HEAD2 h;	// C1:06
	int iINDEX;	// 4
	BYTE iPosition;	// 8
	char szUID[11];	// 9
};


void GameProtocol::GCRegEventChipRecv(PMSG_REGEVENTCHIP* lpMsg, CGameObject &Obj)
{
	if (!EVENCHIP_TYPE_RANGE(lpMsg->Type))
		return;

	

	if (lpObj.m_IfState.type == 1)
	{
		return;
	}

	if (lpObj.m_IfState.type == 7)
	{
		return;
	}

	if (lpObj.UseEventServer != FALSE)
	{
		PMSG_REGEVENTCHIP_RESULT Result;

		PHeadSetB((BYTE*)&Result, 0x95, sizeof(Result));
		Result.ChipCount = -1;
		Result.Type = lpMsg->Type;

		IOCP.DataSend(lpObj.m_Index, (BYTE*)&Result, Result.h.size);

		return;
	}

	int Pos = lpMsg->ChipPos + INVENTORY_BAG_START;
	CItemObject * sitem = &lpObj.pInventory[Pos];

	if (!sitem->IsItem())
	{
		PMSG_REGEVENTCHIP_RESULT Result;

		PHeadSetB((BYTE*)&Result, 0x95, sizeof(Result));
		Result.ChipCount = -1;
		Result.Type = lpMsg->Type;

		IOCP.DataSend(lpObj.m_Index, (BYTE*)&Result, Result.h.size);

		return;
	}

	lpObj.UseEventServer = TRUE;

	switch (lpMsg->Type)
	{
	case 0x00:
		if (sitem->m_Type == ITEMGET(14, 21) && sitem->m_Level == 0) // Rena
		{
			PMSG_REQ_REGISTER_EVENTCHIP pMsg;

			PHeadSubSetB((BYTE*)&pMsg, 0xBE, 0x02, sizeof(pMsg));
			pMsg.iINDEX = aIndex;
			pMsg.Pos = Pos;
			strcpy(pMsg.szUID, lpObj.AccountID);

			wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg));
		}
		else
		{
			PMSG_REGEVENTCHIP_RESULT Result;

			PHeadSetB((BYTE*)&Result, 0x95, sizeof(Result));
			Result.ChipCount = -1;
			Result.Type = 0x00;

			IOCP.DataSend(lpObj.m_Index, (BYTE*)&Result, Result.h.size);
			lpObj.UseEventServer = FALSE;
		}
		break;
	case 0x01:
		if (sitem->m_Type == ITEMGET(14, 21) && sitem->m_Level == 1) // Stone
		{
			PMSG_REQ_REGISTER_STONES pMsg;

			PHeadSubSetB((BYTE*)&pMsg, 0xBE, 0x06, sizeof(pMsg));
			pMsg.iINDEX = aIndex;
			pMsg.iPosition = Pos;
			strcpy(pMsg.szUID, lpObj.AccountID);

			wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg));

		}
		else
		{
			PMSG_REGEVENTCHIP_RESULT Result;

			PHeadSetB((BYTE*)&Result, 0x95, sizeof(Result));
			Result.ChipCount = -1;
			Result.Type = 0x01;

			IOCP.DataSend(lpObj.m_Index, (BYTE*)&Result, Result.h.size);
			lpObj.UseEventServer = FALSE;
		}
		break;
	default:
		lpObj.UseEventServer = FALSE;
		break;
	}
}

struct PMSG_REQ_REGISTER_MUTONUM
{
	PBMSG_HEAD2 h;	// C1:03
	int iINDEX;	// 4
	char szUID[11];	// 8
};

void GameProtocol::GCGetMutoNumRecv(PMSG_GETMUTONUMBER* lpMsg, CGameObject &Obj)
{
	if (lpObj.MutoNumber != 0)
	{
		char msg[255];
		wsprintf(msg, Lang.GetText(0, 527));
		GCServerMsgStringSend(msg, aIndex, 1);
		return;
	}

	if (lpObj.UseEventServer != FALSE)
		return;

	lpObj.UseEventServer = TRUE;

	if (!gObjFind10EventChip(aIndex))
	{
		PMSG_GETMUTONUMBER_RESULT Result;

		PHeadSetB((BYTE*)&Result, 0x96, sizeof(Result));
		Result.MutoNum[0] = -1;
		Result.MutoNum[1] = 0;
		Result.MutoNum[2] = 0;

		IOCP.DataSend(lpObj.m_Index, (BYTE*)&Result, Result.h.size);
		lpObj.UseEventServer = FALSE;

		return;
	}

	PMSG_REQ_REGISTER_MUTONUM pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xBE, 0x03, sizeof(pMsg));
	pMsg.iINDEX = aIndex;
	strcpy(pMsg.szUID, lpObj.AccountID);

	wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg));

}



void GameProtocol::GCUseEndEventChipRescv(CGameObject &Obj)
{
	if (!gObjIsConnectedGP(aIndex))
	{
		return;
	}

	if (lpObj.m_IfState.use && lpObj.m_IfState.type == 9)
	{
		lpObj.m_IfState.state = 0;
		lpObj.m_IfState.type = 0;
		lpObj.m_IfState.use = 0;
	}
}

struct PMSG_REQ_RESET_EVENTCHIP
{
	PBMSG_HEAD2 h;	// C1:[04:Rena] [09:Stone]
	int iINDEX;	// 4
	char szUID[11];	// 8
};

void GameProtocol::GCUseRenaChangeZenRecv(PMSG_EXCHANGE_EVENTCHIP* lpMsg, CGameObject &Obj)
{
	if (lpObj.UseEventServer)
		return;

	lpObj.UseEventServer = TRUE;

	PMSG_REQ_RESET_EVENTCHIP pMsg;

	if (lpMsg->btType == 1)	// Stone?
		PHeadSubSetB((BYTE*)&pMsg, 0xBE, 0x09, sizeof(pMsg));
	else
		PHeadSubSetB((BYTE*)&pMsg, 0xBE, 0x04, sizeof(pMsg));

	pMsg.iINDEX = aIndex;
	strcpy(pMsg.szUID, lpObj.AccountID);

	wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg));

	if (lpMsg->btType == 0x01)
	{

	}
	else
	{

	}
}

struct PMSG_SEND_QEUSTINFO
{
	PBMSG_HEAD2 h;	// C1:A0
	BYTE Count;	// 3
	BYTE State[50];	// 4
};

void GameProtocol::CGRequestQuestInfo(CGameObject &Obj)
{
	

	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	if (lpObj.m_PlayerData->m_SendQuestInfo != 0)
		return;

	int questcount = g_QuestInfo.GetQeustCount();
	int foundquest = 0;

	PMSG_SEND_QEUSTINFO pMsg;
	LPQUEST_INFO lpQuestInfo;

	memcpy(pMsg.State, lpObj.m_PlayerData->m_Quest, sizeof(pMsg.State));
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

	PHeadSetB((BYTE*)&pMsg, 0xA0, iSize);
	pMsg.Count = i;
	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, iSize);
	lpObj.m_PlayerData->m_SendQuestInfo = true;
}

void GameProtocol::GCSendQuestInfo(CGameObject &Obj, int QuestIndex) // Third Quest
{
	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	CGRequestQuestInfo(aIndex);
	
	PMSG_SETQUEST pMsg;

	PHeadSetB((BYTE*)&pMsg, 0xA1, sizeof(pMsg));
	pMsg.QuestIndex = QuestIndex;
	pMsg.State = g_QuestInfo.GetQuestStateBYTE(lpObj, QuestIndex);

	if (pMsg.State != 0)
		IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pMsg, sizeof(pMsg));
}


struct PMSG_SETQUEST_RESULT
{
	PBMSG_HEAD2 h;	// C1:A2
	BYTE QuestIndex;	// 3
	BYTE Result;	// 4
	BYTE State;	// 5
};


void GameProtocol::CGSetQuestState(PMSG_SETQUEST* lpMsg, CGameObject &Obj)
{
	
	PMSG_SETQUEST_RESULT pMsg;

	PHeadSetB((BYTE*)&pMsg, 0xA2, sizeof(pMsg));
	pMsg.QuestIndex = lpMsg->QuestIndex;
	pMsg.Result = g_QuestInfo.SetQuestState(lpObj, lpMsg->QuestIndex, lpMsg->State);
	pMsg.State = g_QuestInfo.GetQuestStateBYTE(lpObj, lpMsg->QuestIndex);

	IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pMsg, sizeof(pMsg));
}


struct PMSG_SETQUEST_PRIZE
{
	PBMSG_HEAD2 h;	// C1:A3
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	BYTE Type;	// 5
	BYTE Count;	// 6
};


void GameProtocol::GCSendQuestPrize(CGameObject &Obj, BYTE Type, BYTE Count)
{
	CGRequestQuestInfo(aIndex);
	

	PMSG_SETQUEST_PRIZE pMsg;

	PHeadSetB((BYTE*)&pMsg, 0xA3, sizeof(pMsg));
	pMsg.NumberH = SET_NUMBERH(lpObj.m_Index);
	pMsg.NumberL = SET_NUMBERL(lpObj.m_Index);
	pMsg.Type = Type;
	pMsg.Count = Count;

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, sizeof(pMsg));
	MsgSendV2(lpObj, (BYTE*)&pMsg, sizeof(pMsg));
}



void GameProtocol::CGCloseWindow(CGameObject &Obj)
{
	if (lpObj.m_IfState.use && lpObj.m_IfState.type)
	{
		if (lpObj.m_IfState.type == 7)
			return;

		if (lpObj.m_IfState.type == 10)	// Marlon
		{
			if (lpObj.TargetNpcNumber != -1 && getGameObject(lpObj.TargetNpcNumber)->Class == 229)
			{
				gQeustNpcTeleport.TalkRefDel();
				lpObj.TargetNpcNumber = -1;
			}
		}

		if (lpObj.TargetNpcNumber != -1 && getGameObject(lpObj.TargetNpcNumber)->Class == 568)
		{
			gQuestExpNpcTeleport.TalkRefDel();
			lpObj.TargetNpcNumber = -1;
		}

		if (lpObj.m_IfState.type == 3)
		{
			lpObj.TargetNpcNumber = -1;
		}

		if (lpObj.m_IfState.type == 1)
		{
			CGTradeCancelButtonRecv(aIndex);
		}

		if (lpObj.m_IfState.type == 6)
		{
			CGWarehouseUseEnd(aIndex);
		}

		if (lpObj.m_IfState.type == 13)
		{
			g_MixSystem.ChaosBoxInit(&getGameObject(aIndex));
			gObjInventoryCommit(aIndex);
		}

		lpObj.m_IfState.state = 0;
		lpObj.m_IfState.type = 0;
		lpObj.m_IfState.use = 0;
	}
}



void GameProtocol::CGRequestEnterBloodCastle(PMSG_REQ_MOVEBLOODCASTLE* lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	//BYTE btBridgeNumber = lpMsg->iBridgeNumber;
	//BYTE btInvisibleCourtItemPos = lpMsg->iItemPos;
	BYTE btInvisibleCourtItemPos;
	int iITEM_LEVEL = 0;
	UINT64 dwITEM_SERIAL = 0;
	PMSG_RESULT_MOVEBLOODCASTLE pResult;

	PHeadSetB((BYTE*)&pResult, 0x9A, sizeof(pResult));
	pResult.Result = 0;

	if (Obj.Type != OBJ_USER || Obj.Connected <= PLAYER_LOGGED)
		return;

	if (Obj.m_IfState.use && Obj.m_IfState.type != 12)
		return;

	BYTE btBridgeNumber;
	BYTE btGateNumber;

	int CurrLvl = Obj.Level + Obj.m_PlayerData->MasterLevel;

	if ((CurrLvl >= 1) && (CurrLvl <= 49))
	{
		pResult.Result = 4;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
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

	if (Obj.Type != OBJ_USER || Obj.Connected <= PLAYER_LOGGED)
		return;

	if (Obj.m_IfState.use && Obj.m_IfState.type != 12)
		return;

	BOOL bPlayerKiller = FALSE; //Season 2.5 add-on

	if (Obj.PartyNumber >= 0) //Season 2.5 add-on
	{
		if (gParty.GetPKPartyPenalty(Obj.PartyNumber) >= 5)
		{
			bPlayerKiller = TRUE;
		}
	}
	else if (Obj.m_PK_Level >= 4)
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
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (g_BloodCastle.CheckCanEnter(btBridgeNumber) == false)
	{
		pResult.Result = 2;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	int Err = 0;

	for (int btInvisibleCourtItemPos = 12; btInvisibleCourtItemPos < 76; btInvisibleCourtItemPos++)
	{

		if (Obj.pInventory[btInvisibleCourtItemPos]->m_Type == ITEMGET(13, 18))	// Cloack's Invitation
		{
			if (Obj.pInventory[btInvisibleCourtItemPos]->m_Level == btBridgeNumber + 1)
			{

				gObjInventoryDeleteItem(iIndex, btInvisibleCourtItemPos);
				GCInventoryItemDeleteSend(iIndex, btInvisibleCourtItemPos, 1);
				Err++;
				break;
			}

		}
		else if (Obj.pInventory[btInvisibleCourtItemPos]->m_Type == ITEMGET(13, 47)) //Ticket BC
		{
			if (Obj.pInventory[btInvisibleCourtItemPos]->m_Durability > 0.0f)
			{
				Obj.pInventory[btInvisibleCourtItemPos]->m_Durability -= 1.0f;
				if (Obj.pInventory[btInvisibleCourtItemPos]->m_Durability == 0.0f)
				{
					gObjInventoryDeleteItem(iIndex, btInvisibleCourtItemPos);
					GCInventoryItemDeleteSend(iIndex, btInvisibleCourtItemPos, 1);
				}
				else
					GCItemObjectDurSend2(Obj.m_Index, btInvisibleCourtItemPos, Obj.pInventory[btInvisibleCourtItemPos]->m_Durability, 0);
				Err++;
				break;
			}
		}

	}

	if (Err == 0)
	{
		pResult.Result = 1;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	int iBC_INDEX = g_BloodCastle.EnterUserBridge(btBridgeNumber, iIndex);

	if (iBC_INDEX == -1)
	{
		pResult.Result = 5;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	Obj.m_cBloodCastleIndex = btBridgeNumber;
	Obj.m_cBloodCastleSubIndex = iBC_INDEX;
	Obj.m_iBloodCastleEXP = 0;
	Obj.m_IfState.state = 0;
	Obj.m_IfState.type = 0;
	Obj.m_IfState.use = 0;

	g_BloodCastle.SearchUserDeleteQuestItem(Obj.m_Index);
	char szTemp[256];
	wsprintf(szTemp, Lang.GetText(0, 74), btBridgeNumber + 1);
	GCServerMsgStringSend(szTemp, iIndex, 1);

	gObjMoveGate(iIndex, btGateNumber);
}

struct PMSG_RESULT_MOVECHAOSCASTLE
{
	PBMSG_HEAD2 h;	// C1:AF:01
	BYTE Result;	// 4
};

void GameProtocol::CGRequestEnterChaosCastle(PMSG_REQ_MOVECHAOSCASTLE* lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	BYTE btChaosCastleNumber = lpMsg->iChaosCastleNumber - 1;
	BYTE btGuardMailSetItemPos = lpMsg->iItemPos;
	int iITEM_LEVEL = 0;
	UINT64 dwITEM_SERIAL = 0;
	PMSG_RESULT_MOVECHAOSCASTLE pResult;

	PHeadSubSetB((BYTE*)&pResult, 0xAF, 0x01, sizeof(pResult));
	pResult.Result = 0;

	if (Obj.Type != OBJ_USER || Obj.Connected <= PLAYER_LOGGED)
		return;

	if (BC_MAP_RANGE(Obj.MapNumber) || CC_MAP_RANGE(Obj.MapNumber) || DS_MAP_RANGE(Obj.MapNumber) ||
		IT_MAP_RANGE(Obj.MapNumber) || DG_MAP_RANGE(Obj.MapNumber) || IMPERIAL_MAP_RANGE(Obj.MapNumber) ||
		Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL || g_ConfigRead.server.GetServerType() == SERVER_ARCA)
	{
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 118));
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pNotice, pNotice.h.size);
		return;
	}

	if (Obj.m_IfState.use && Obj.m_IfState.type != 12)
		return;

	if (Obj.m_PlayerData->m_bUsePartyMatching)
	{
		PMSG_ANS_PARTYMATCHING_ERROR pMsg;
		PHeadSubSetB((BYTE*)&pMsg, 0xEF, 0x09, sizeof(pMsg));
		pMsg.iResult = -1;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	if (g_ConfigRead.pk.bPkPenaltyDisable == FALSE)
	{
		BOOL bPlayerKiller = FALSE; //Season 2.5 add-on

		if (Obj.PartyNumber >= 0) //Season 2.5 add-on
		{
			if (gParty.GetPKPartyPenalty(Obj.PartyNumber) >= 6)
			{
				bPlayerKiller = TRUE;
			}
		}
		else if (Obj.m_PK_Level >= 6)
		{
			bPlayerKiller = TRUE;
		}

		if (bPlayerKiller == TRUE)
		{
			PMSG_NOTICE pNotice;
			TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 119));
			IOCP.DataSend(Obj.m_Index, (BYTE*)&pNotice, pNotice.h.size);
			return;
		}
	}

	if (gObjDuelCheck(Obj) == TRUE)
	{
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 120));
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pNotice, pNotice.h.size);
		return;
	}

	if (Obj.m_PlayerData->lpGuild)
	{
		if (Obj.m_PlayerData->lpGuild->WarState == 1)
		{
			PMSG_NOTICE pNotice;
			TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 121));
			IOCP.DataSend(Obj.m_Index, (BYTE*)&pNotice, pNotice.h.size);
			return;
		}
	}

	if (Obj.pInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 3) ||	// Dino
		Obj.pInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 2) ||	// Uniria
		Obj.pInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 37))	// Fenrir
	{
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 122));
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pNotice, pNotice.h.size);
		return;
	}

	BOOL bPlayerKiller = FALSE; //Season 2.5 add-on

	if (Obj.PartyNumber >= 0) //Season 2.5 add-on
	{
		if (gParty.GetPKPartyPenalty(Obj.PartyNumber) >= 5)
		{
			bPlayerKiller = TRUE;
		}
	}
	else if (Obj.m_PK_Level >= 4)
	{
		bPlayerKiller = TRUE;
	}

	if (bPlayerKiller == TRUE)
	{
		pResult.Result = 8;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	int iENTER_LEVEL = g_ChaosCastle.GetUserLevelToEnter(Obj.m_Index);

	if (iENTER_LEVEL == -1)
	{
		pResult.Result = 1;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (g_ChaosCastle.CheckCanEnter(iENTER_LEVEL) == false)
	{
		pResult.Result = 2;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (Obj.pInventory[btGuardMailSetItemPos]->IsItem() == TRUE)
	{
		if (Obj.pInventory[btGuardMailSetItemPos]->m_Type == ITEMGET(13, 29)) // Armor of Guardman
		{
			iITEM_LEVEL = Obj.pInventory[btGuardMailSetItemPos]->m_Level;
			dwITEM_SERIAL = Obj.pInventory[btGuardMailSetItemPos]->m_Number;
		}
		else
		{
			pResult.Result = 1;
			IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}
	}
	else
	{
		pResult.Result = 1;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (g_ChaosCastle.CheckWearingMOPH(Obj.m_Index) == TRUE)
	{
		GCServerMsgStringSend(Lang.GetText(0, 123), iIndex, 1);
		return;
	}

	int iBC_INDEX = -1;

	if (g_ChaosCastle.CheckUserEnterMoney(iIndex, iENTER_LEVEL))
	{
		iBC_INDEX = g_ChaosCastle.EnterUserChaosCastle(iENTER_LEVEL, iIndex);

		if (iBC_INDEX == -1)
		{
			pResult.Result = 5;
			IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}
	}
	else
	{
		pResult.Result = 7;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	BOOL bRES = g_ChaosCastle.PayUserEnterMoney(iIndex, iENTER_LEVEL);

	if (bRES == FALSE)
	{
		g_ChaosCastle.LeaveUserChaosCastle(iENTER_LEVEL, iIndex);
		pResult.Result = 5;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	gObjInventoryDeleteItem(iIndex, btGuardMailSetItemPos);
	GCInventoryItemDeleteSend(iIndex, btGuardMailSetItemPos, 1);

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);

	Obj.m_cChaosCastleIndex = iENTER_LEVEL;
	Obj.m_cChaosCastleSubIndex = iBC_INDEX;
	Obj.m_IfState.state = 0;
	Obj.m_IfState.type = 0;
	Obj.m_IfState.use = 0;

	if (Obj.m_bPShopOpen == true)
		g_PersonalStore.CGPShopReqClose(Obj.m_Index);

	if (Obj.PartyNumber >= 0)
	{
		PMSG_PARTYDELUSER pMsg;
		int pindex = gParty.GetIndex(Obj.PartyNumber, iIndex, Obj.DBNumber);

		if (pindex >= 0)
		{
			pMsg.Number = pindex;
			CGPartyDelUser(&pMsg, iIndex, 0);
		}
	}

	if (Obj.Class == CLASS_ELF && Obj.m_RecallMon >= 0)
	{
		GCRecallMonLife(iIndex, 60, 0);
		gObjMonsterCallKill(Obj.m_Index);
	}

	char szTemp[256];
	wsprintf(szTemp, Lang.GetText(0, 124), iENTER_LEVEL + 1);
	GCServerMsgStringSend(szTemp, iIndex, 1);

	switch (iENTER_LEVEL + 1)
	{
	case 1:
		gObjMoveGate(iIndex, 82);
		break;
	case 2:
		gObjMoveGate(iIndex, 83);
		break;
	case 3:
		gObjMoveGate(iIndex, 84);
		break;
	case 4:
		gObjMoveGate(iIndex, 85);
		break;
	case 5:
		gObjMoveGate(iIndex, 86);
		break;
	case 6:
		gObjMoveGate(iIndex, 87);
		break;
	case 7:
		gObjMoveGate(iIndex, 272);
		break;
	}

}

void GameProtocol::CGRequestRepositionUserInChaosCastle(PMSG_REQ_REPOSUSER_IN_CC * lpMsg, CGameObject &Obj)
{
	if (!gObjIsConnected(aIndex))
		return;

	if (!CC_MAP_RANGE(lpObj.MapNumber))
		return;

	if (g_ChaosCastle.ObjSetPosition(aIndex, lpObj.X, lpObj.Y) == TRUE)
		lpObj.m_iChaosCastleBlowTime = GetTickCount();
}



void GameProtocol::CGRequestEventEnterCount(PMSG_REQ_CL_ENTERCOUNT* lpMsg, CGameObject &Obj)
{
	if (!lpMsg)
		return;

	switch (lpMsg->btEventType)
	{
	case 0x02:
		EGReqBloodCastleEnterCount(aIndex);
		break;
	}
}

struct PMSG_REQ_2ANIV_SERIAL
{
	PBMSG_HEAD2 h;	// C1:08
	int iINDEX;	// 8
	char szUID[MAX_ACCOUNT_LEN + 1];	// 8
	char SERIAL1[5];	// 13
	char SERIAL2[5];	// 18
	char SERIAL3[5];	// 1D
	int iMEMB_GUID;	// 24
};

void GameProtocol::CGRequestLottoRegister(PMSG_REQ_2ANV_LOTTO_EVENT* lpMsg, CGameObject &Obj)
{
	PMSG_REQ_2ANIV_SERIAL pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xBE, 0x08, sizeof(pMsg));

	if (lpObj.UseEventServer)
		return;

	lpObj.UseEventServer = TRUE;
	pMsg.iINDEX = aIndex;
	pMsg.iMEMB_GUID = lpObj.DBNumber;
	memcpy(pMsg.szUID, lpObj.AccountID, MAX_ACCOUNT_LEN);
	pMsg.szUID[MAX_ACCOUNT_LEN] = 0;
	memcpy(pMsg.SERIAL1, lpMsg->SERIAL1, 4);
	pMsg.SERIAL1[4] = 0;
	memcpy(pMsg.SERIAL2, lpMsg->SERIAL2, 4);
	pMsg.SERIAL2[4] = 0;
	memcpy(pMsg.SERIAL3, lpMsg->SERIAL3, 4);
	pMsg.SERIAL3[4] = 0;

	wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg));

	sLog->outBasic("[Mu_2Anv_Event] [%s][%s] Register Lotto Number (Serial: %s-%s-%s)",
		lpObj.AccountID, lpObj.Name, pMsg.SERIAL1, pMsg.SERIAL2, pMsg.SERIAL3);
}

struct SDHP_CHARACTER_TRANSFER
{
	PBMSG_HEAD2 h;	// C1:CF
	char Account[10];	// 3
	short Number;	// E
};


void GameProtocol::CGReqMoveOtherServer(PMSG_REQ_MOVE_OTHERSERVER * lpMsg, CGameObject &Obj)
{
	if (!gEnableServerDivision)
		return;

	if (!gObjIsConnectedGP(aIndex))
	{
		sLog->outError( "error-L3 [%s][%d]", __FILE__, __LINE__);
		return;
	}

	

	if (lpObj.m_MoveOtherServer)
		return;

	lpObj.m_MoveOtherServer = true;

	PMSG_DEFRESULT pResult;
	PHeadSetB((BYTE*)&pResult, 0x99, sizeof(pResult));

	if (!gObjPasswordCheck(aIndex, lpMsg->LastJoominNumber))
	{
		pResult.result = 0;

		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		lpObj.m_MoveOtherServer = false;

		sLog->outBasic("[CharTrasfer] Fail (JoominNumber) [%s][%s]",
			lpObj.AccountID, lpObj.Name);

		GCServerMsgStringSend(Lang.GetText(0, 528), lpObj.m_Index, 1);

		return;
	}

	SDHP_CHARACTER_TRANSFER pCharTransfer;

	GJSetCharacterInfo(lpObj, lpObj.m_Index, FALSE);
	PHeadSetB((BYTE*)&pCharTransfer, 0xCF, sizeof(pCharTransfer));
	pCharTransfer.Number = aIndex;
	memcpy(pCharTransfer.Account, lpObj.AccountID, sizeof(pCharTransfer.Account));

	wsDataCli.DataSend((PCHAR)&pCharTransfer, sizeof(pCharTransfer));

	sLog->outBasic("[CharTrasfer] Request Character Trasfer [%s][%s]",
		lpObj.AccountID, lpObj.Name);
}



void GameProtocol::GCPacketCheckSumRecv(PMSG_PACKETCHECKSUM * aRecv, CGameObject &Obj)
{
	gPacketCheckSum.AddCheckSum(aIndex, aRecv->funcindex, aRecv->CheckSum);
}

struct PMSG_GETITEMINFO_FOR_PARTY
{
	PBMSG_HEAD2 h;	// C1:47
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	UINT ItemInfo;	// 8
	BYTE Level;	// C
};

void GameProtocol::GCSendGetItemInfoForParty(CGameObject &Obj, CMapItem * lpItem)
{
	

	if (!lpItem)
		return;

	if (lpObj.PartyNumber >= 0)
	{
		PMSG_GETITEMINFO_FOR_PARTY pMsg;

		pMsg.h.c = 0xC1;
		pMsg.h.headcode = 0x47;
		pMsg.h.size = sizeof(pMsg);
		pMsg.NumberH = SET_NUMBERH(aIndex);
		pMsg.NumberL = SET_NUMBERL(aIndex);
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

		if (g_kItemSystemFor380.Is380OptionItem((lpItem != NULL) ? (CItemObject*)&lpItem->m_Number : NULL))
			pMsg.ItemInfo |= 0x40000;

		pMsg.Level = lpItem->m_Level;

		int partycount = gParty.GetPartyCount(lpObj.PartyNumber);

		if (partycount >= 0)
		{
			for (int n = 0; n < MAX_USER_IN_PARTY; n++)
			{
				int number = gParty.m_PartyS[lpObj.PartyNumber].Number[n];

				if (number >= 0)
				{
					IOCP.DataSend(number, (BYTE*)&pMsg, pMsg.h.size);
				}
			}
		}
	}
}


struct PMSG_EFFECTINFO
{
	PBMSG_HEAD2 h;	// C1:48
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	BYTE btType;	// 5
};

void GameProtocol::GCSendEffectInfo(CGameObject &Obj, BYTE btType)
{
	if (ObjectMaxRange(aIndex) == FALSE)
	{
		return;
	}

	
	PMSG_EFFECTINFO pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x48;
	pMsg.h.size = sizeof(pMsg);
	pMsg.NumberH = SET_NUMBERH(aIndex);
	pMsg.NumberL = SET_NUMBERL(aIndex);
	pMsg.btType = btType;

	if (btType == 17)
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	MsgSendV2(lpObj, (BYTE*)&pMsg, pMsg.h.size);
}



void GameProtocol::CGRequestPetItemCommand(PMSG_REQUEST_PET_ITEM_COMMAND * lpMsg, CGameObject &Obj)
{
	if (!gObjIsConnectedGP(aIndex))
	{
		sLog->outBasic("error-L2 : Index %s %d", __FILE__, __LINE__);
		return;
	}

	if (!ObjectMaxRange(aIndex))
		return;

	

	if (lpMsg->PetType)
		return;

	int iTargetIndex = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
	gDarkSpirit[aIndex - g_ConfigRead.server.GetObjectStartUserIndex()].ChangeCommand(lpMsg->Command, iTargetIndex);
}


// DONE YAYA!!!
void GameProtocol::CGRequestPetItemInfo(CGameObject &Obj, PMSG_SEND_PET_ITEMINFO &pMsg)
{
	if (!gObjIsConnectedGP(lpObj))
	{
		sLog->outBasic("error-L2 : Index %s %d", __FILE__, __LINE__);
		return;
	}

	if (pMsg.PetType != 0 && pMsg.PetType != 1)
		return;

	if (pMsg.InvenType == 0)	// Inventory
	{
		if (!INVENTORY_RANGE(pMsg.nPos))
		{
			sLog->outError("[PetItem] [%s][%s] RequestPetItem Invalid Iventory nPos %d",
				lpObj.AccountID, lpObj.Name, pMsg.nPos);
			return;
		}

		if (lpObj.pInventory[pMsg.nPos].IsItem())
		{
			pMsg.Level = lpObj.pInventory[pMsg.nPos].m_PetItem_Level;
			pMsg.Exp = lpObj.pInventory[pMsg.nPos].m_PetItem_Exp - gPetItemExp.m_DarkSpiritExpTable[pMsg.Level];
			pMsg.Dur = (BYTE)lpObj.pInventory[pMsg.nPos].m_Durability;

			IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (BYTE*)&pMsg, sizeof(pMsg));
		}
	}
	else if (pMsg.InvenType == 1)	// Warehouse
	{
		if (lpObj.LoadWareHouseInfo != false)
		{
			if (!WAREHOUSE_RANGE(pMsg.nPos))
			{
				sLog->outError("[PetItem] [%s][%s] RequestPetItem Invalid WareHouse nPos %d",
					lpObj.AccountID, lpObj.Name, pMsg.nPos);
				return;
			}

			if (lpObj.pWarehouse[pMsg.nPos].IsItem())
			{
				pMsg.Level = lpObj.pWarehouse[pMsg.nPos].m_PetItem_Level;
				pMsg.Exp = lpObj.pWarehouse[pMsg.nPos].m_PetItem_Exp - gPetItemExp.m_DarkSpiritExpTable[pMsg.Level];
				pMsg.Dur = (BYTE)lpObj.pInventory[pMsg.nPos].m_Durability;

				IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (BYTE*)&pMsg, sizeof(pMsg));
			}
		}
	}
	else if (pMsg.InvenType == 2)	// Trade
	{
		if (lpObj.m_IfState.use < 1 || lpObj.m_IfState.type != 1)
			return;

		if (!TRADE_BOX_RANGE(pMsg.nPos))
		{
			sLog->outError("[PetItem] [%s][%s] RequestPetItem Invalid Trade nPos %d",
				lpObj.AccountID, lpObj.Name, pMsg.nPos);
			return;
		}

		if (lpObj.Trade[pMsg.nPos].IsItem())
		{
			pMsg.Level = lpObj.Trade[pMsg.nPos].m_PetItem_Level;
			pMsg.Exp = lpObj.Trade[pMsg.nPos].m_PetItem_Exp - gPetItemExp.m_DarkSpiritExpTable[pMsg.Level];
			pMsg.Dur = (BYTE)lpObj.pInventory[pMsg.nPos].m_Durability;

			IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (BYTE*)&pMsg, sizeof(pMsg));
		}
	}
	else if (pMsg.InvenType == 3)	// Target Trade
	{
		if (lpObj.m_IfState.use < 1 || lpObj.m_IfState.type != 1)
			return;

		int iTargetIndex = lpObj.TargetNumber;

		if (iTargetIndex < 0)
			return;

		if (!ObjectMaxRange(iTargetIndex))
			return;

		if (getGameObject(iTargetIndex]->m_IfState.use < 1 || getGameObject(iTargetIndex)->m_IfState.type != 1)
			return;

		if (!TRADE_BOX_RANGE(pMsg.nPos))
		{
			sLog->outError("[PetItem] [%s][%s] RequestPetItem Invalid TargetTrade nPos %d",
				lpObj.AccountID, lpObj.Name, pMsg.nPos);
			return;
		}

		if (getGameObject(iTargetIndex]->Trade[pMsg.nPos).IsItem())
		{
			pMsg.Level = getGameObject(iTargetIndex]->Trade[pMsg.nPos).m_PetItem_Level;
			pMsg.Exp = getGameObject(iTargetIndex]->Trade[pMsg.nPos].m_PetItem_Exp - gPetItemExp.m_DarkSpiritExpTable[pMsg.Level);
			pMsg.Dur = (BYTE)lpObj.pInventory[pMsg.nPos].m_Durability;

			IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (BYTE*)&pMsg, sizeof(pMsg));
		}
	}
	else if (pMsg.InvenType == 4)	// Chaos
	{
		if (!CHAOS_BOX_RANGE(pMsg.nPos))
		{
			sLog->outError("[PetItem] [%s][%s] RequestPetItem Invalid Chaos nPos %d",
				lpObj.AccountID, lpObj.Name, pMsg.nPos);
			return;
		}

		if (lpObj.pChaosBox[pMsg.nPos].IsItem())
		{
			pMsg.Level = lpObj.pChaosBox[pMsg.nPos].m_PetItem_Level;
			pMsg.Exp = lpObj.pChaosBox[pMsg.nPos].m_PetItem_Exp - gPetItemExp.m_DarkSpiritExpTable[pMsg.Level];
			pMsg.Dur = (BYTE)lpObj.pInventory[pMsg.nPos].m_Durability;

			IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (BYTE*)&pMsg, sizeof(pMsg));
		}
	}
	else if (pMsg.InvenType == 5)	// Personal Shop
	{
		if (!INVENTORY_RANGE(pMsg.nPos))
		{
			sLog->outError("[PetItem] [%s][%s] RequestPetItem Invalid Iventory nPos %d",
				lpObj.AccountID, lpObj.Name, pMsg.nPos);
			return;
		}

		if (lpObj.m_iPShopDealerIndex != -1)
		{
			if (ObjectMaxRange(lpObj.m_iPShopDealerIndex))
			{
				if (getGameObject(lpObj.m_iPShopDealerIndex]->pInventory[pMsg.nPos).IsItem())
				{
					pMsg.Level = getGameObject(lpObj.m_iPShopDealerIndex]->pInventory[pMsg.nPos).m_PetItem_Level;
					pMsg.Exp = getGameObject(lpObj.m_iPShopDealerIndex]->pInventory[pMsg.nPos].m_PetItem_Exp - gPetItemExp.m_DarkSpiritExpTable[pMsg.Level);
					pMsg.Dur = (BYTE)lpObj.pInventory[pMsg.nPos].m_Durability;

					IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (BYTE*)&pMsg, sizeof(pMsg));
				}
			}
		}
	}
}





struct PMSG_ANS_GUILDVIEWPORT
{
	PBMSG_HEAD2 h;	// C1:66
	int GuildNumber;	// 4
	BYTE btGuildType;	// 8
	BYTE UnionName[8];	// 9
	char GuildName[8];	// 11
	BYTE Mark[32];	// 19
};

void GameProtocol::GCGuildViewportInfo(PMSG_REQ_GUILDVIEWPORT * aRecv, CGameObject &Obj)
{
	if (!gObjIsConnectedGP(aIndex))
	{
		sLog->outBasic("error-L2 : Index %s %d", __FILE__, __LINE__);
		return;
	}

	if (!ObjectMaxRange(aIndex))
		return;

	
	DWORD dwGuildNumber = aRecv->GuildNumber;
	GUILD_INFO_STRUCT * lpGuildInfo = Guild.SearchGuild_Number(dwGuildNumber);

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

		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, sizeof(pMsg));
	}
	else
	{
		sLog->outBasic("GuildViewportInfo Fail : [%s] GuildNumber : %d",
			lpObj.Name, dwGuildNumber);
	}
}





void GameProtocol::CGGuildAssignStatus(PMSG_GUILD_ASSIGN_STATUS_REQ * aRecv, CGameObject &Obj)
{
	if (!gObjIsConnectedGP(aIndex))
	{
		sLog->outBasic("error-L2 : Index %s %d", __FILE__, __LINE__);
		return;
	}

	if (!ObjectMaxRange(aIndex))
		return;

	
	PMSG_GUILD_ASSIGN_STATUS_RESULT pMsg = { 0 };

	pMsg.h.set((BYTE*)&pMsg, 0xE1, sizeof(pMsg));
	pMsg.btType = aRecv->btType;
	memcpy(pMsg.szTagetName, aRecv->szTagetName, sizeof(pMsg.szTagetName));

	if (lpObj.m_PlayerData->GuildNumber <= 0 || lpObj.m_PlayerData->lpGuild == NULL)
	{
		pMsg.btResult = GUILD_ANS_NOTEXIST_GUILD;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, sizeof(pMsg));
		return;
	}

	if (lpObj.m_PlayerData->GuildStatus != G_MASTER)
	{
		pMsg.btResult = GUILD_ANS_NOTEXIST_PERMISSION;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, sizeof(pMsg));
		return;
	}

	char szTargetName[MAX_ACCOUNT_LEN + 1] = { 0 };
	memcpy(szTargetName, aRecv->szTagetName, MAX_ACCOUNT_LEN);

	int iArcaBattleState = g_ArcaBattle.GetState();

	if (iArcaBattleState > 2 && iArcaBattleState < 9)
	{
		pMsg.btResult = GUILD_ANS_ARCA_BATTLE_ONGOING;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, sizeof(pMsg));
		return;
	}

	if (!strcmp(lpObj.Name, szTargetName))
	{
		pMsg.btResult = GUILD_ANS_NOTEXIST_PERMISSION;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, sizeof(pMsg));
		return;
	}

	if (lpObj.m_PlayerData->lpGuild == NULL)
		return;

	if (aRecv->btType == 1 || aRecv->btType == 2)
	{
		int iSubMasterCount = 0;
		int iBattleMasterCount = 0;

		for (int n = 0; n < MAX_USER_GUILD; n++)
		{
			if (lpObj.m_PlayerData->lpGuild->Use[n] > 0)
			{
				if (lpObj.m_PlayerData->lpGuild->GuildStatus[n] == G_SUB_MASTER)
				{
					iSubMasterCount++;
				}
				else if (lpObj.m_PlayerData->lpGuild->GuildStatus[n] == G_BATTLE_MASTER)
				{
					iBattleMasterCount++;
				}
			}
		}

		if (aRecv->btGuildStatus == G_SUB_MASTER)
		{
			if (iSubMasterCount == 0)
			{
				GDGuildReqAssignStatus(aIndex, aRecv->btType, aRecv->szTagetName, aRecv->btGuildStatus);
				return;
			}

			pMsg.btResult = GUILD_ANS_NOTEXIST_EXTRA_STATUS;
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, sizeof(pMsg));
			return;
		}
		else if (aRecv->btGuildStatus == G_BATTLE_MASTER)
		{
			if (iBattleMasterCount < (((lpObj.Level + lpObj.m_PlayerData->MasterLevel) / 200) + 1))
			{
				GDGuildReqAssignStatus(aIndex, aRecv->btType, aRecv->szTagetName, aRecv->btGuildStatus);
				return;
			}

			pMsg.btResult = GUILD_ANS_NOTEXIST_EXTRA_STATUS;
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, sizeof(pMsg));
			return;
		}
		else if (aRecv->btGuildStatus == G_PERSON)
		{
			GDGuildReqAssignStatus(aIndex, aRecv->btType, aRecv->szTagetName, aRecv->btGuildStatus);
			return;
		}
		else
		{
			return;
		}
	}

	if (aRecv->btType == 3)
	{
		GDGuildReqAssignStatus(aIndex, aRecv->btType, aRecv->szTagetName, aRecv->btGuildStatus);
	}
}



void GameProtocol::CGGuildAssignType(PMSG_GUILD_ASSIGN_TYPE_REQ * aRecv, CGameObject &Obj)
{
	if (!gObjIsConnectedGP(aIndex))
	{
		sLog->outBasic("error-L2 : Index %s %d", __FILE__, __LINE__);
		return;
	}

	if (!ObjectMaxRange(aIndex))
		return;

	
	PMSG_GUILD_ASSIGN_TYPE_RESULT pMsg = { 0 };

	pMsg.h.set((BYTE*)&pMsg, 0xE2, sizeof(pMsg));
	pMsg.btGuildType = aRecv->btGuildType;

	if (lpObj.m_PlayerData->GuildNumber <= 0 || lpObj.m_PlayerData->lpGuild == NULL)
	{
		pMsg.btResult = GUILD_ANS_NOTEXIST_GUILD;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, sizeof(pMsg));
		return;
	}

	if (lpObj.m_PlayerData->GuildStatus != G_MASTER)
	{
		pMsg.btResult = GUILD_ANS_NOTEXIST_PERMISSION;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, sizeof(pMsg));
		return;
	}

	GDGuildReqAssignType(aIndex, aRecv->btGuildType);
}



void GameProtocol::CGRelationShipReqJoinBreakOff(PMSG_RELATIONSHIP_JOIN_BREAKOFF_REQ * aRecv, CGameObject &Obj)
{
	if (g_CastleSiegeSync.GetCastleState() >= 5 && g_CastleSiegeSync.GetCastleState() <= 7)
	{
		MsgOutput(aIndex, Lang.GetText(0, 197));
		return;
	}

	int iTargetUserIndex = MAKE_NUMBERW(aRecv->btTargetUserIndexH, aRecv->btTargetUserIndexL);

	if (!ObjectMaxRange(aIndex) || !ObjectMaxRange(iTargetUserIndex))
		return;

	if (!PacketCheckTime(aIndex))
		return;

	if (gObjIsConnectedGP(iTargetUserIndex) == FALSE)
	{
		GCResultSend(aIndex, 0x51, 3);
		return;
	}

	int iArcaBattleState = g_ArcaBattle.GetState();

	if (iArcaBattleState > 2 && iArcaBattleState < 9)
	{
		//TRACE_LOG("aIndex, 0x51, 0xA5");
		GCResultSend(aIndex, 0x51, 0xA4);
		return;
	}

	if (g_NewPVP.IsDuel(getGameObject(aIndex]) || g_NewPVP.IsDuel(getGameObject(iTargetUserIndex)))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 320), aIndex, 1);
		return;
	}

	if (g_NewPVP.IsObserver(getGameObject(aIndex]) || g_NewPVP.IsObserver(getGameObject(iTargetUserIndex)))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 321), aIndex, 1);
		return;
	}

	if (!lpObj.m_PlayerData->lpGuild || !getGameObject(iTargetUserIndex)->m_PlayerData->lpGuild)
	{
		GCResultSend(aIndex, 0x51, 5);
		return;
	}

	if (lpObj.m_PlayerData->GuildStatus != G_MASTER && getGameObject(iTargetUserIndex)->m_PlayerData->GuildStatus != G_MASTER)
	{
		GCResultSend(aIndex, 0x51, 4);
		return;
	}

	if (lpObj.m_IfState.use > 0)
	{
		GCResultSend(aIndex, 0x51, 6);
		return;
	}

	if (getGameObject(iTargetUserIndex)->m_IfState.use > 0)
	{
		GCResultSend(aIndex, 0x51, 6);
		return;
	}

	PMSG_RELATIONSHIP_JOIN_BREAKOFF_ANS ErrMsg = { 0 };

	PHeadSetB((BYTE*)&ErrMsg, 0xE6, sizeof(ErrMsg));
	ErrMsg.btResult = 0;
	ErrMsg.btRequestType = aRecv->btRequestType;
	ErrMsg.btRelationShipType = aRecv->btRelationShipType;
	ErrMsg.btTargetUserIndexH = aRecv->btTargetUserIndexH;
	ErrMsg.btTargetUserIndexL = aRecv->btTargetUserIndexL;

	
	CGameObject lpTargetObj = &getGameObject(iTargetUserIndex);

	if (lpObj.m_PlayerData->GuildStatus != G_MASTER || lpTargetObj.m_PlayerData->GuildStatus != G_MASTER)
	{
		ErrMsg.btResult = GUILD_ANS_NOTEXIST_PERMISSION;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&ErrMsg, ErrMsg.h.size);
		return;
	}

	GUILD_INFO_STRUCT * lpGuildInfo = lpObj.m_PlayerData->lpGuild;
	GUILD_INFO_STRUCT * lpTargetGuildInfo = getGameObject(iTargetUserIndex)->m_PlayerData->lpGuild;
	BYTE btRelationShip = gObjGetRelationShip(lpObj, lpTargetObj);

	if (aRecv->btRequestType == 1)
	{
		if (aRecv->btRelationShipType == 1)
		{
			if (lpTargetGuildInfo->Count < g_ConfigRead.data.common.AllianceMinGuildMember)
			{
				ErrMsg.btResult = GUILD_ANS_CANNOT_BE_UNION_MASTER;
				IOCP.DataSend(lpObj.m_Index, (BYTE*)&ErrMsg, ErrMsg.h.size);
				return;
			}

			if (lpTargetGuildInfo->iGuildUnion != 0 && lpTargetGuildInfo->iGuildUnion == lpTargetGuildInfo->Number)	// #error
			{
				if (UnionManager.GetGuildRelationShipCount(lpTargetGuildInfo->iGuildUnion, 1) >= g_ConfigRead.data.common.AllianceMaxGuilds)
				{
					ErrMsg.btResult = GUILD_ANS_EXCEED_MAX_UNION_MEMBER;
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&ErrMsg, ErrMsg.h.size);
					return;
				}
			}

			char szCastleOwnerGuildName[MAX_GUILD_LEN + 1] = { 0 };
			memcpy(szCastleOwnerGuildName, g_CastleSiegeSync.GetCastleOwnerGuild(), MAX_GUILD_LEN);

			if (!strcmp(lpGuildInfo->Name, szCastleOwnerGuildName))
			{
				ErrMsg.btResult = GUILD_ANS_EXIST_UNION;
				IOCP.DataSend(lpObj.m_Index, (BYTE*)&ErrMsg, ErrMsg.h.size);
				return;
			}

			if (lpGuildInfo->iGuildUnion)
			{
				ErrMsg.btResult = GUILD_ANS_EXIST_UNION;
				IOCP.DataSend(lpObj.m_Index, (BYTE*)&ErrMsg, ErrMsg.h.size);
				return;
			}

			if (btRelationShip == 2)
			{
				ErrMsg.btResult = GUILD_ANS_EXIST_RELATIONSHIP_RIVAL;
				IOCP.DataSend(lpObj.m_Index, (BYTE*)&ErrMsg, ErrMsg.h.size);
				return;
			}

			if (lpTargetGuildInfo->iGuildUnion && lpTargetGuildInfo->Number != lpTargetGuildInfo->iGuildUnion)	// #error???
			{
				ErrMsg.btResult = GUIDL_ANS_NOT_UNION_MASTER;
				IOCP.DataSend(lpObj.m_Index, (BYTE*)&ErrMsg, ErrMsg.h.size);
				return;
			}

			int UnionMasterInfluence = g_GensSystem.GetGensInfluence(lpTargetObj);
			int GuildMasterInfluence = g_GensSystem.GetGensInfluence(lpObj);

			if (UnionMasterInfluence == NONE_INFLUENCE)
			{
				ErrMsg.btResult = GUILD_ANS_NOT_UNION_MASTER_GENS;
				IOCP.DataSend(lpObj.m_Index, (BYTE*)&ErrMsg, ErrMsg.h.size);
				return;
			}

			if (GuildMasterInfluence == NONE_INFLUENCE)
			{
				ErrMsg.btResult = GUILD_ANS_NOT_GENS;
				IOCP.DataSend(lpObj.m_Index, (BYTE*)&ErrMsg, ErrMsg.h.size);
				return;
			}

			if (UnionMasterInfluence != GuildMasterInfluence && g_GensSystem.CanGensJoinUnionWhileOppositeGens() == FALSE)
			{
				ErrMsg.btResult = GUILD_ANS_WRONG_GENS;
				IOCP.DataSend(lpObj.m_Index, (BYTE*)&ErrMsg, ErrMsg.h.size);
				return;
			}

		}
		else if (aRecv->btRelationShipType == 2)
		{
			/*
			if ( lpGuildInfo->iGuildRival || lpTargetGuildInfo->iGuildRival )
			{
				ErrMsg.btResult = GUILD_ANS_EXIST_RIVAL;
				IOCP.DataSend(lpObj.m_Index, (BYTE*)&ErrMsg, ErrMsg.h.size);
				return;
			}*/

			if (btRelationShip == 1)
			{
				ErrMsg.btResult = GUILD_ANS_EXIST_RELATIONSHIP_UNION;
				IOCP.DataSend(lpObj.m_Index, (BYTE*)&ErrMsg, ErrMsg.h.size);
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
				IOCP.DataSend(lpObj.m_Index, (BYTE*)&ErrMsg, ErrMsg.h.size);
				return;
			}
		}
		else if (aRecv->btRelationShipType == 2)
		{
			if (!lpGuildInfo->iGuildRival || !lpTargetGuildInfo->iGuildRival)
			{
				ErrMsg.btResult = GUILD_ANS_NOTEXIST_UNION;
				IOCP.DataSend(lpObj.m_Index, (BYTE*)&ErrMsg, ErrMsg.h.size);
				return;
			}

			if (lpGuildInfo->iGuildRival != lpTargetGuildInfo->Number || lpGuildInfo->Number != lpTargetGuildInfo->iGuildRival)
			{
				ErrMsg.btResult = GUILD_ANS_NOT_GUILD_RIVAL;
				IOCP.DataSend(lpObj.m_Index, (BYTE*)&ErrMsg, ErrMsg.h.size);
				return;
			}
		}
	}

	if (aRecv->btRequestType == 2)
	{
		if (aRecv->btRelationShipType == 1)
		{
			GDUnionBreakOff(aIndex, lpGuildInfo->iGuildUnion);
			return;
		}
	}

	lpObj.m_IfState.use = 1;
	lpObj.m_IfState.type = 14;
	lpObj.m_IfState.state = 0;
	lpObj.m_InterfaceTime = GetTickCount();

	lpTargetObj.m_IfState.use = 1;
	lpTargetObj.m_IfState.type = 14;
	lpTargetObj.m_IfState.state = 0;
	lpTargetObj.m_InterfaceTime = GetTickCount();

	PMSG_RELATIONSHIP_JOIN_BREAKOFF_REQ pMsg = { 0 };

	PHeadSetB((BYTE*)&pMsg, 0xE5, sizeof(pMsg));
	pMsg.btTargetUserIndexL = SET_NUMBERL(aIndex);
	pMsg.btTargetUserIndexH = SET_NUMBERH(aIndex);
	pMsg.btRequestType = aRecv->btRequestType;
	pMsg.btRelationShipType = aRecv->btRelationShipType;

	IOCP.DataSend(iTargetUserIndex, (BYTE*)&pMsg, pMsg.h.size);
}



void GameProtocol::CGRelationShipAnsJoinBreakOff(PMSG_RELATIONSHIP_JOIN_BREAKOFF_ANS * aRecv, CGameObject &Obj)
{
	int iTargetUserIndex = MAKE_NUMBERW(aRecv->btTargetUserIndexH, aRecv->btTargetUserIndexL);

	if (!ObjectMaxRange(aIndex) || !ObjectMaxRange(iTargetUserIndex))
		return;

	if (!PacketCheckTime(aIndex))
		return;

	if (gObjIsConnected(&getGameObject(iTargetUserIndex)) == FALSE)
	{
		GCResultSend(aIndex, 0x51, 3);
		return;
	}

	if (aRecv->btResult == 1)
	{
		if (aRecv->btRequestType == 1)
		{
			GDRelationShipReqJoin(iTargetUserIndex, aIndex, aRecv->btRelationShipType);
		}
		else if (aRecv->btRequestType == 2)
		{
			GDRelationShipReqBreakOff(aIndex, iTargetUserIndex, aRecv->btRelationShipType);
		}
	}
	else
	{
		PMSG_RELATIONSHIP_JOIN_BREAKOFF_ANS ErrMsg = { 0 };

		PHeadSetB((BYTE*)&ErrMsg, 0xE6, sizeof(ErrMsg));
		ErrMsg.btResult = 0;
		ErrMsg.btRequestType = aRecv->btRequestType;
		ErrMsg.btRelationShipType = aRecv->btRelationShipType;
		ErrMsg.btTargetUserIndexH = aRecv->btTargetUserIndexH;
		ErrMsg.btTargetUserIndexL = aRecv->btTargetUserIndexL;
		ErrMsg.btResult = GUILD_ANS_CANCLE_REQUEST;

		IOCP.DataSend(iTargetUserIndex, (BYTE*)&ErrMsg, ErrMsg.h.size);
	}

	if (lpObj.m_IfState.use)
	{
		if (lpObj.m_IfState.type == 14)
		{
			lpObj.m_IfState.use = 0;
		}
	}

	if (getGameObject(iTargetUserIndex)->m_IfState.use)
	{
		if (getGameObject(iTargetUserIndex)->m_IfState.type == 14)
		{
			getGameObject(iTargetUserIndex)->m_IfState.use = 0;
		}
	}
}



void GameProtocol::CGUnionList(PMSG_UNIONLIST_REQ * aRecv, CGameObject &Obj)
{
	if (!ObjectMaxRange(aIndex))
		return;

	if (!PacketCheckTime(aIndex))
		return;

	

	if (lpObj == NULL)
	{
		return;
	}

	if (gObjIsConnected(&getGameObject(aIndex)) == FALSE)
	{
		GCResultSend(aIndex, 0x51, 3);
		return;
	}

	if (lpObj.m_PlayerData->lpGuild == NULL)
	{
		GCResultSend(aIndex, 0x51, 3);
		return;
	}

	if (lpObj.m_PlayerData->lpGuild->iGuildUnion == 0)
	{
		GCResultSend(aIndex, 0x51, 3);
		return;
	}

	GDUnionListSend(aIndex, lpObj.m_PlayerData->lpGuild->iGuildUnion);

}


void GameProtocol::CGRelationShipReqKickOutUnionMember(PMSG_KICKOUT_UNIONMEMBER_REQ* aRecv, CGameObject &Obj)
{
	if (this->PacketCheckTime(lpObj) == FALSE)
		return;

	if (lpObj == NULL)
		return;

	if (gObjIsConnected(lpObj) == FALSE)
	{
		GCResultSend(lpObj, 0x51, 3);
		MsgOutput(lpObj, Lang.GetText(0, 529));
		return;
	}

	if (lpObj.m_PlayerData->lpGuild == NULL)
	{
		GCResultSend(lpObj, 0x51, 3);
		MsgOutput(aIndex, Lang.GetText(0, 530));
		return;
	}

	PMSG_RELATIONSHIP_JOIN_BREAKOFF_ANS ErrMsg = { 0 };

	PHeadSetB((BYTE*)&ErrMsg, 0xE6, sizeof(ErrMsg));
	ErrMsg.btResult = 0;
	ErrMsg.btRequestType = 2;
	ErrMsg.btRelationShipType = 1;
	ErrMsg.btTargetUserIndexH = 0;
	ErrMsg.btTargetUserIndexL = 0;

	if (lpObj.m_PlayerData->lpGuild->iGuildUnion == 0)
	{
		ErrMsg.btResult = GUILD_ANS_NOTEXIST_UNION;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&ErrMsg, ErrMsg.h.size);

		return;
	}

	int iArcaBattleState = g_ArcaBattle.GetState();

	if (iArcaBattleState > 2 && iArcaBattleState < 9)
	{
		ErrMsg.btResult = GUILD_ANS_ARCA_BATTLE_ONGOING;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&ErrMsg, sizeof(ErrMsg));
		return;
	}

	if (lpObj.m_PlayerData->GuildStatus != GUILD_MASTER || lpObj.m_PlayerData->lpGuild->Number != lpObj.m_PlayerData->lpGuild->iGuildUnion)
	{
		ErrMsg.btResult = GUILD_ANS_NOTEXIST_PERMISSION;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&ErrMsg, ErrMsg.h.size);

		return;
	}

	char szUnionMemberGuildName[MAX_GUILD_LEN + 1] = { 0 };
	memcpy(szUnionMemberGuildName, aRecv->szTargetGuildName, MAX_GUILD_LEN);

	if (!strcmp(lpObj.m_PlayerData->lpGuild->Name, szUnionMemberGuildName))
	{
		ErrMsg.btResult = GUILD_ANS_NOTEXIST_PERMISSION;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&ErrMsg, ErrMsg.h.size);

		return;
	}

	GDRelationShipReqKickOutUnionMember(aIndex, szUnionMemberGuildName);
}



void GameProtocol::CGReqMapSvrAuth(PMSG_REQ_MAPSERVERAUTH * lpMsg, CGameObject &Obj)
{
	if (lpMsg->btCliVersion[0] != szClientVersion[0] ||
		lpMsg->btCliVersion[1] != szClientVersion[1] ||
		lpMsg->btCliVersion[2] != szClientVersion[2] ||
		lpMsg->btCliVersion[3] != szClientVersion[3] ||
		lpMsg->btCliVersion[4] != szClientVersion[4])
	{
		//GCAnsMapSvrAuth(iIndex, 6);
		//IOCP.CloseClient(Obj.m_Index);

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
			sLog->outError(  "error-L1: Serial error [%s] [%s]", id, btSerial);
			GCAnsMapSvrAuth(iIndex, 6);
			IOCP.CloseClient(Obj.m_Index);

			return;
		}*/

	if (g_ConfigRead.server.IsConnectMemberUse() == TRUE)
	{
		if (ConMember.IsMember(id) == FALSE)
		{
			GCAnsMapSvrAuth(iIndex, 2);

			return;
		}
	}

	CGameObject lpObj = Obj;

	if (PacketCheckTime(Obj.m_Index) == FALSE)
	{
		sLog->outBasic("error-L2: Login time is over [%d][%s]", iIndex, id);
		IOCP.CloseClient(Obj.m_Index);

		return;
	}

	if (Obj.Connected != PLAYER_CONNECTED)
	{
		sLog->outBasic("error-L2: Authentication is allowed after connected [%d][%s]", iIndex, id);
		IOCP.CloseClient(Obj.m_Index);

		return;
	}

	if (Obj.LoginMsgSnd != 0)
	{
		if (Obj.LoginMsgSnd == 1)
		{
			sLog->outBasic("error: It is after authentication information is sended [%d][%s][%s]", iIndex, id, Obj.m_PlayerData->Ip_addr);
		}
		else
		{
			sLog->outBasic("error: %d %s %d", Obj.LoginMsgSnd, __FILE__, __LINE__);
		}

		return;
	}

	Obj.CheckTick = lpMsg->iTickCount;
	Obj.CheckTick2 = GetTickCount();
	Obj.ConnectCheckTime = GetTickCount();
	Obj.CheckSpeedHack = true;
	Obj.ConnectCheckTime = GetTickCount();
	Obj.LoginMsgSnd = 1;
	Obj.LoginMsgCount++;
	Obj.AccountID[MAX_ACCOUNT_LEN] = 0;
	memcpy(Obj.AccountID, id, MAX_ACCOUNT_LEN);
	memset(Obj.Name, 0, MAX_ACCOUNT_LEN);
	memcpy(Obj.m_MapServerAuthInfo.szCharName, lpMsg->szCharName, sizeof(lpMsg->szCharName));
	Obj.m_MapServerAuthInfo.iJA1 = lpMsg->iJoinAuthCode1;
	Obj.m_MapServerAuthInfo.iJA2 = lpMsg->iJoinAuthCode2;
	Obj.m_MapServerAuthInfo.iJA3 = lpMsg->iJoinAuthCode3;
	Obj.m_MapServerAuthInfo.iJA4 = lpMsg->iJoinAuthCode4;
	Obj.m_cAccountItemBlock = 0;
	Obj.m_PlayerData->VipType = 0;
	Obj.m_PlayerData->VipEffect = VIP_EFFECT_NONE;
	Obj.m_PlayerData->AntiHackCheckTime = GetTickCount();
	Obj.m_PlayerData->dwLastHitHackTick = GetTickCount64();
	gObjAddMsgSendDelay(lpObj, 1001, iIndex, 5000, 0);
}

struct PMSG_ANS_MAPSERVERAUTH
{
	PBMSG_HEAD2 h;	// C1:B1:01
	BYTE iResult;	// 4
};

void GameProtocol::GCAnsMapSvrAuth(CGameObject &Obj, int iResult)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		sLog->outError( "[MapServerMng] Packet Error JG [0x7B] - Index out of bound : %d", iIndex);
		return;
	}

	PMSG_ANS_MAPSERVERAUTH pMsgResult;

	PHeadSubSetB((BYTE*)&pMsgResult, 0xB1, 0x01, sizeof(pMsgResult));
	pMsgResult.iResult = iResult;

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::CGReqCastleSiegeState(PMSG_REQ_CASTLESIEGESTATE * lpMsg, CGameObject &Obj)
{
	GS_GDReqOwnerGuildMaster(g_MapServerManager.GetMapSvrGroup(), iIndex);
}

void GameProtocol::GCAnsCastleSiegeState(CGameObject &Obj, int iResult, LPSTR lpszGuildName, LPSTR lpszGuildMaster)
{
	if ((lpszGuildName == NULL) || (lpszGuildMaster == NULL))
	{
		return;
	}

	PMSG_ANS_CASTLESIEGESTATE pMsgResult;

	pMsgResult.h.set((BYTE*)&pMsgResult.h, 0xB2, 0x00, sizeof(pMsgResult));

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

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::CGReqRegCastleSiege(PMSG_REQ_REGCASTLESIEGE * lpMsg, CGameObject &Obj)
{
	if (g_CastleSiege.GetCastleState() != CASTLESIEGE_STATE_REGSIEGE)
	{
		GCAnsRegCastleSiege(iIndex, 7, "");
		return;
	}
	if (strcmp(Obj.m_PlayerData->GuildName, "") == 0)
	{
		GCAnsRegCastleSiege(iIndex, 6, "");
		return;
	}
	if (g_CastleSiege.CheckUnionGuildMaster(Obj.m_Index) == FALSE)
	{
		GCAnsRegCastleSiege(iIndex, 0, "");
	}
	else if ((g_CastleSiege.CheckCastleOwnerMember(Obj.m_Index) != FALSE) ||
		(g_CastleSiege.CheckCastleOwnerUnionMember(Obj.m_Index) != FALSE))
	{
		GCAnsRegCastleSiege(iIndex, 3, "");
	}
	else
	{
		GS_GDReqRegAttackGuild(g_MapServerManager.GetMapSvrGroup(), iIndex);
	}
}

void GameProtocol::GCAnsRegCastleSiege(CGameObject &Obj, int iResult, LPSTR lpszGuildName)
{
	PMSG_ANS_REGCASTLESIEGE pMsgResult;

	if (lpszGuildName == NULL)
	{
		return;
	}

	if (ObjectMaxRange(Obj.m_Index) == FALSE)
	{
		return;
	}

	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x01, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	memcpy(&pMsgResult.szGuildName, lpszGuildName, sizeof(pMsgResult.szGuildName));

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsgResult, pMsgResult.h.size);

	if (::gObjIsConnected(Obj.m_Index))
	{
		sLog->outBasic("[CastleSiege] [%s][%s] Registered Castle Siege (GUILD:%s) - Result:%d", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, iResult);
	}
}

void GameProtocol::CGReqGiveUpCastleSiege(PMSG_REQ_GIVEUPCASTLESIEGE * lpMsg, CGameObject &Obj)
{
	if ((g_CastleSiege.GetCastleState() < CASTLESIEGE_STATE_REGSIEGE) || (g_CastleSiege.GetCastleState() > CASTLESIEGE_STATE_REGMARK)) //Fixed
	{
		GCAnsGiveUpCastleSiege(iIndex, 3, 0, 0, "");
		return;
	}

	if (strcmp(Obj.m_PlayerData->GuildName, "") == 0)
	{
		return;
	}

	if (g_CastleSiege.CheckUnionGuildMaster(Obj.m_Index) == FALSE)
	{
		return;
	}

	GS_GDReqGuildSetGiveUp(g_MapServerManager.GetMapSvrGroup(), iIndex, lpMsg->btGiveUp);
}

void GameProtocol::GCAnsGiveUpCastleSiege(CGameObject &Obj, int iResult, int bGiveUp, int iMarkCount, LPSTR lpszGuildName)
{
	PMSG_ANS_GIVEUPCASTLESIEGE pMsgResult;

	if (lpszGuildName == NULL)
	{
		return;
	}

	if (ObjectMaxRange(Obj.m_Index) == FALSE)
	{
		return;
	}

	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x02, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.btIsGiveUp = bGiveUp;

	memcpy(&pMsgResult.szGuildName, lpszGuildName, sizeof(pMsgResult.szGuildName));

	if ((pMsgResult.btResult == 1) && (iMarkCount > 0))
	{
		Obj.m_PlayerData->Money += (iMarkCount * 3000);

		if (Obj.m_PlayerData->Money > MAX_ZEN)
		{
			Obj.m_PlayerData->Money = MAX_ZEN;
		}
		GCMoneySend(iIndex, Obj.m_PlayerData->Money);
	}

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsgResult, pMsgResult.h.size);

	if (gObjIsConnected(Obj.m_Index))
	{
		sLog->outBasic("[CastleSiege] [%s][%s] GiveUp Castle Siege (GUILD:%s) - Result:%d", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, iResult);
	}
}

void GameProtocol::CGReqGuildRegInfo(PMSG_REQ_GUILDREGINFO * lpMsg, CGameObject &Obj)
{
	if (lpMsg == NULL)
	{
		return;
	}

	if (strcmp(Obj.m_PlayerData->GuildName, "") == 0)
	{
		return;
	}

	GS_GDReqGuildMarkRegInfo(g_MapServerManager.GetMapSvrGroup(), iIndex);
}

void GameProtocol::GCAnsGuildRegInfo(CGameObject &Obj, int iResult, CSP_ANS_GUILDREGINFO* lpMsgResult)
{
	PMSG_ANS_GUILDREGINFO pMsgResult;

	if (lpMsgResult == NULL)
	{
		return;
	}

	if (ObjectMaxRange(Obj.m_Index) == FALSE)
	{
		return;
	}

	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 3, sizeof(pMsgResult));

	pMsgResult.btResult = iResult;
	pMsgResult.btGuildMark1 = SET_NUMBERH(SET_NUMBERHW(lpMsgResult->iRegMarkCount));
	pMsgResult.btGuildMark2 = SET_NUMBERL(SET_NUMBERHW(lpMsgResult->iRegMarkCount));
	pMsgResult.btGuildMark3 = SET_NUMBERH(SET_NUMBERLW(lpMsgResult->iRegMarkCount));
	pMsgResult.btGuildMark4 = SET_NUMBERL(SET_NUMBERLW(lpMsgResult->iRegMarkCount));
	pMsgResult.btRegRank = lpMsgResult->btRegRank;
	pMsgResult.btIsGiveUp = lpMsgResult->bIsGiveUp;
	memcpy(&pMsgResult.szGuildName, lpMsgResult->szGuildName, sizeof(pMsgResult.szGuildName));

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::CGReqRegGuildMark(PMSG_REQ_REGGUILDMARK * lpMsg, CGameObject &Obj)
{
	if (lpMsg == NULL)
	{
		return;
	}

	if (ObjectMaxRange(Obj.m_Index) == FALSE)
	{
		return;
	}

	if (Obj.UseEventServer == TRUE)
	{
		return;
	}

	Obj.UseEventServer = TRUE;

	if (g_CastleSiege.GetCastleState() != CASTLESIEGE_STATE_REGMARK)
	{
		Obj.UseEventServer = FALSE;
		return;
	}
	if (strcmp(Obj.m_PlayerData->GuildName, "") == 0)
	{
		Obj.UseEventServer = FALSE;
		return;
	}

	if (CHECK_LIMIT(lpMsg->btItemPos, 64) == FALSE)
	{
		PMSG_ANS_REGGUILDMARK pMsgResult = { 0 };
		pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x04, sizeof(pMsgResult));
		pMsgResult.btResult = 3;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
		Obj.UseEventServer = FALSE;
	}
	else
	{
		int iItemPos = (lpMsg->btItemPos + 12);

		if (Obj.pInventory[iItemPos]->IsItem() == FALSE)
		{
			PMSG_ANS_REGGUILDMARK pMsgResult = { 0 };
			pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x04, sizeof(pMsgResult));
			pMsgResult.btResult = 3;
			IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
			Obj.UseEventServer = FALSE;
		}
		else
		{
			if (Obj.pInventory[iItemPos]->m_Type != ITEMGET(14, 21) ||
				Obj.pInventory[iItemPos]->m_Level != 3 ||
				Obj.pInventory[iItemPos]->m_Durability <= 0.0f)
			{
				PMSG_ANS_REGGUILDMARK pMsgResult = { 0 };
				pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x04, sizeof(pMsgResult));
				pMsgResult.btResult = 3;
				IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
				Obj.UseEventServer = FALSE;
			}
			else
			{
				GS_GDReqRegGuildMark(g_MapServerManager.GetMapSvrGroup(), iIndex, iItemPos);
			}
		}
	}
}

void GameProtocol::GCAnsRegGuildMark(CGameObject &Obj, int iResult, CSP_ANS_GUILDREGMARK* lpMsgResult)
{
	if (lpMsgResult == NULL)
	{
		return;
	}

	if (ObjectMaxRange(Obj.m_Index) == FALSE)
	{
		return;
	}

	if (Obj.pInventory[lpMsgResult->iItemPos]->IsItem() == FALSE)
	{
		PMSG_ANS_REGGUILDMARK pMsgResult = { 0 };
		pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x04, sizeof(pMsgResult));
		pMsgResult.btResult = 3;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
		Obj.UseEventServer = 0;
	}
	else
	{
		if (Obj.pInventory[lpMsgResult->iItemPos]->m_Type != ITEMGET(14, 21) &&
			Obj.pInventory[lpMsgResult->iItemPos]->m_Level == 3 ||
			Obj.pInventory[lpMsgResult->iItemPos]->m_Durability <= 0.0f)
		{
			PMSG_ANS_REGGUILDMARK pMsgResult = { 0 };
			pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x04, sizeof(pMsgResult));
			pMsgResult.btResult = 3;
			IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
			Obj.UseEventServer = FALSE;
		}
		else
		{
			if ((Obj.pInventory[lpMsgResult->iItemPos]->m_Durability -= 1.0) > 0.0f)
			{
				GCItemObjectDurSend(iIndex, lpMsgResult->iItemPos, Obj.pInventory[lpMsgResult->iItemPos]->m_Durability, 0);
			}
			else
			{
				gObjInventoryDeleteItem(iIndex, lpMsgResult->iItemPos);
				GCInventoryItemDeleteSend(iIndex, lpMsgResult->iItemPos, 1);
			}

			PMSG_ANS_REGGUILDMARK pMsgResult;
			pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x04, sizeof(pMsgResult));

			pMsgResult.btResult = iResult;
			pMsgResult.btGuildMark1 = SET_NUMBERH(SET_NUMBERHW(lpMsgResult->iRegMarkCount));
			pMsgResult.btGuildMark2 = SET_NUMBERL(SET_NUMBERHW(lpMsgResult->iRegMarkCount));
			pMsgResult.btGuildMark3 = SET_NUMBERH(SET_NUMBERLW(lpMsgResult->iRegMarkCount));
			pMsgResult.btGuildMark4 = SET_NUMBERL(SET_NUMBERLW(lpMsgResult->iRegMarkCount));
			memcpy(&pMsgResult.szGuildName, lpMsgResult->szGuildName, sizeof(pMsgResult.szGuildName));
			IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
			Obj.UseEventServer = 0;

		}
	}
}

void GameProtocol::CGReqNpcBuy(PMSG_REQ_NPCBUY * lpMsg, CGameObject &Obj)
{
	if (lpMsg == NULL)
	{
		return;
	}

	if (::gObjIsConnected(Obj.m_Index) == FALSE)
	{
		return;
	}

	if (strcmp(Obj.m_PlayerData->GuildName, "") == 0)
	{
		return;
	}

	if (g_CastleSiege.CheckGuildOwnCastle(Obj.m_PlayerData->GuildName) == FALSE || (Obj.m_PlayerData->GuildStatus != G_MASTER && Obj.m_PlayerData->GuildStatus != G_SUB_MASTER))
	{
		sLog->outError( "[CastleSiege] CGReqNpcBuy() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus, lpMsg->iNpcNumber, lpMsg->iNpcIndex);
	}
	else
	{
		BYTE btResult = 0;

		BOOL bRETVAL = g_CastleSiege.CheckAddDbNPC(iIndex, lpMsg->iNpcNumber, lpMsg->iNpcIndex, btResult);

		if (bRETVAL == FALSE)
		{
			GCAnsNpcBuy(iIndex, btResult, lpMsg->iNpcNumber, lpMsg->iNpcIndex);
			sLog->outError( "[CastleSiege] CGReqNpcBuy() ERROR - CCastleSiege::CheckAddDbNPC() [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus, lpMsg->iNpcNumber, lpMsg->iNpcIndex);
		}
		else
		{
			sLog->outError( "[CastleSiege] CGReqNpcBuy() OK - [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus, lpMsg->iNpcNumber, lpMsg->iNpcIndex);
		}
	}
}

void GameProtocol::GCAnsNpcBuy(CGameObject &Obj, int iResult, int iNpcNumber, int iNpcIndex)
{
	PMSG_ANS_NPCBUY pMsgResult;

	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x05, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.iNpcNumber = iNpcNumber;
	pMsgResult.iNpcIndex = iNpcIndex;
	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::CGReqNpcRepair(PMSG_REQ_NPCREPAIR * lpMsg, CGameObject &Obj)
{
	if (lpMsg == NULL)
	{
		return;
	}

	if (::gObjIsConnected(Obj.m_Index) == FALSE)
	{
		return;
	}

	if (strcmp(Obj.m_PlayerData->GuildName, "") == 0)
	{
		return;
	}

	if ((g_CastleSiege.CheckGuildOwnCastle(Obj.m_PlayerData->GuildName) == FALSE) || (Obj.m_PlayerData->GuildStatus != G_MASTER) && (Obj.m_PlayerData->GuildStatus != G_SUB_MASTER))
	{
		sLog->outError( "[CastleSiege] CGReqNpcRepair() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus);
		return;
	}

	_CS_NPC_DATA pNpcData;
	BOOL bRETVAL;
	int iNpcIndex;
	int iRepairCost;

	bRETVAL = g_CastleSiege.GetNpcData(lpMsg->iNpcNumber, lpMsg->iNpcIndex, pNpcData);

	if (bRETVAL == FALSE)
	{
		GCAnsNpcRepair(iIndex, 0, lpMsg->iNpcNumber, lpMsg->iNpcIndex, 0, 0);
		sLog->outError( "[CastleSiege] CGReqNpcRepair() ERROR - CL Request Fail [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus, lpMsg->iNpcNumber, lpMsg->iNpcIndex);
		return;
	}
	else
	{
		iNpcIndex = pNpcData.m_iNPC_OBJINDEX;

		if (gObjIsConnected(iNpcIndex) == FALSE)
		{
			GCAnsNpcRepair(iIndex, 0, lpMsg->iNpcNumber, lpMsg->iNpcIndex, 0, 0);
			sLog->outError( "[CastleSiege] CGReqNpcRepair() ERROR - Npc Alive Data Mismatch [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus, pNpcData.m_iNPC_NUM, pNpcData.m_iNPC_INDEX);
			return;
		}
		else if (getGameObject(iNpcIndex)->Live == 0)
		{
			GCAnsNpcRepair(iIndex, 0, lpMsg->iNpcNumber, lpMsg->iNpcIndex, 0, 0);
			sLog->outError( "[CastleSiege] CGReqNpcRepair() ERROR - Npc Alive Data Mismatch [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus, pNpcData.m_iNPC_NUM, pNpcData.m_iNPC_INDEX);
			return;
		}
		else
		{
			if ((getGameObject(iNpcIndex]->MaxLife == getGameObject(iNpcIndex]->Life) || (getGameObject(iNpcIndex)->MaxLife < getGameObject(iNpcIndex)->Life))
			{
				getGameObject(iNpcIndex]->Life = getGameObject(iNpcIndex)->MaxLife;
				GCAnsNpcRepair(iIndex, 0, lpMsg->iNpcNumber, lpMsg->iNpcIndex, 0, 0);
				return;
			}
			else
			{
				iRepairCost = 0;

				switch (lpMsg->iNpcNumber)
				{
				case 277:
					iRepairCost = float((getGameObject(iNpcIndex]->MaxLife - getGameObject(iNpcIndex)->Life) * 0.5f) + float(getGameObject(iNpcIndex)->m_btCsNpcDfLevel * 1000000);
					break;
				case 283:
					iRepairCost = (getGameObject(iNpcIndex]->MaxLife - getGameObject(iNpcIndex]->Life)* 0.3f + float((getGameObject(iNpcIndex)->m_btCsNpcDfLevel + getGameObject(iNpcIndex)->m_btCsNpcRgLevel) * 1000000);
					break;
				default:
					GCAnsNpcRepair(iIndex, 0, lpMsg->iNpcNumber, lpMsg->iNpcIndex, 0, 0);
					return;
				}

				if (Obj.m_PlayerData->Money < iRepairCost)
				{
					GCAnsNpcRepair(iIndex, 3, lpMsg->iNpcNumber, lpMsg->iNpcIndex, 0, 0);
					sLog->outError( "[CastleSiege] CGReqNpcRepair() ERROR - Money isn't enough [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus, pNpcData.m_iNPC_NUM, pNpcData.m_iNPC_INDEX);
					return;
				}
				else
				{
					sLog->outError( "[CastleSiege] CGReqNpcRepair() OK - [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus, pNpcData.m_iNPC_NUM, pNpcData.m_iNPC_INDEX);
					GS_GDReqCastleNpcRepair(g_MapServerManager.GetMapSvrGroup(), iIndex, lpMsg->iNpcNumber, lpMsg->iNpcIndex, iRepairCost);
					//return;
				}
			}
		}
	}
}

void GameProtocol::GCAnsNpcRepair(CGameObject &Obj, int iResult, int iNpcNumber, int iNpcIndex, int iNpcHP, int iNpcMaxHP)
{
	PMSG_ANS_NPCREPAIR pMsgResult;

	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x06, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.iNpcNumber = iNpcNumber;
	pMsgResult.iNpcIndex = iNpcIndex;
	pMsgResult.iNpcHP = iNpcHP;
	pMsgResult.iNpcMaxHP = iNpcMaxHP;

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}



void GameProtocol::CGReqNpcUpgrade(PMSG_REQ_NPCUPGRADE * lpMsg, CGameObject &Obj)
{
	if (lpMsg == NULL)
	{
		return;
	}

	if (::gObjIsConnected(Obj.m_Index) == FALSE)
	{
		return;
	}

	int iNEED_GEMOFDEFEND = 0;
	int	iNEED_MONEY = 0;
	int	iCUR_UPLVL = 0;
	int	iNXT_UPLVL = 0;
	int	iNXT_UPVALUE = 0;
	int	bENABLE_UPGRADE = FALSE;

	if (strcmp(Obj.m_PlayerData->GuildName, "") == 0)
		return;

	if ((g_CastleSiege.CheckGuildOwnCastle(Obj.m_PlayerData->GuildName) == FALSE) || (Obj.m_PlayerData->GuildStatus != G_MASTER && Obj.m_PlayerData->GuildStatus != G_SUB_MASTER))
	{
		sLog->outBasic("[CastleSiege] CGReqNpcUpgrade() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus);
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
				GCAnsNpcUpgrade(iIndex, 5, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
				return;
			}
			break;
			}
		}
		else
		{
			GCAnsNpcUpgrade(iIndex, 7, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
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
				GCAnsNpcUpgrade(iIndex, 5, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
				sLog->outBasic("[CastleSiege] CGReqNpcUpgrade() ERROR - UpType doesn't Exist [%s][%s], Guild:(%s)(%d), NPC CLS:%d, IDX:%d, UPTYPE:%d, UPVAL:%d", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildNumber, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
				return;
			}
			break;
			}
		}
		else
		{
			GCAnsNpcUpgrade(iIndex, 7, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
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
		GCAnsNpcUpgrade(iIndex, 6, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
	}
	else
	{
		if (Obj.m_PlayerData->Money < iNEED_MONEY)
		{
			GCAnsNpcUpgrade(iIndex, 3, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
			sLog->outBasic("[CastleSiege] CGReqNpcUpgrade() ERROR - Money is Low [%s][%s], Guild:(%s)(%d), NPC CLS:%d, IDX:%d, UPTYPE:%d, UPVAL:%d, NXTLV:%d, Money:%d, Need Money:%d", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildNumber, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue, iNXT_UPLVL, Obj.m_PlayerData->Money, iNEED_MONEY);
		}
		else
		{
			for (iGEMOFDEFEND_COUNT = 0, x = 0; x < INVENTORY_SIZE; x++)
			{
				if (Obj.pInventory[x]->IsItem() == TRUE)
				{
					if (Obj.pInventory[x]->m_Type == ITEMGET(14, 31))
					{
						iGEMOFDEFEND_COUNT += 1;
					}
				}
			}

			if (iGEMOFDEFEND_COUNT < iNEED_GEMOFDEFEND)
			{
				GCAnsNpcUpgrade(iIndex, 4, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
				sLog->outBasic("[CastleSiege] CGReqNpcUpgrade() ERROR - Gem is Low [%s][%s], Guild:(%s)(%d), NPC CLS:%d, IDX:%d, UPTYPE:%d, UPVAL:%d, NXTLV:%d, Gem:%d, Need Gem:%d", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildNumber, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue, iNXT_UPLVL, iGEMOFDEFEND_COUNT, iNEED_GEMOFDEFEND);
			}
			else
			{
				bRETVAL = g_CastleSiege.PayForUpgradeDbNpc(iIndex, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue, (iNXT_UPLVL - 1));

				if (bRETVAL == FALSE)
				{
				}
				else
				{
					Obj.m_bIsCastleNPCUpgradeCompleted = TRUE;
					sLog->outBasic("[CastleSiege] [0xB2][0x07] CGReqNpcUpgrade() - Pay For Npc Upgrade (CLS:%d, IDX:%d, UPTYPE:%d, UPVAL:%d)", lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, iNXT_UPVALUE);
					GS_GDReqCastleNpcUpgrade(g_MapServerManager.GetMapSvrGroup(), iIndex, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, iNXT_UPVALUE, (iNXT_UPLVL - 1));
					sLog->outBasic("[CastleSiege] CGReqNpcUpgrade() REQ OK - [%s][%s], Guild:(%s)(%d), NPC CLS:%d, IDX:%d, UPTYPE:%d, UPVAL:%d, NXTLV:%d", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildNumber, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue, iNXT_UPLVL);
				}
			}
		}

	}
}

void GameProtocol::GCAnsNpcUpgrade(CGameObject &Obj, int iResult, int iNpcNumber, int iNpcIndex, int iNpcUpType, int iNpcUpValue)
{
	PMSG_ANS_NPCUPGRADE pMsgResult;

	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x07, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.iNpcNumber = iNpcNumber;
	pMsgResult.iNpcIndex = iNpcIndex;
	pMsgResult.iNpcUpType = iNpcUpType;
	pMsgResult.iNpcUpValue = iNpcUpValue;

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::CGReqTaxMoneyInfo(PMSG_REQ_TAXMONEYINFO * lpMsg, CGameObject &Obj)
{
	if (lpMsg == NULL)
	{
		return;
	}

	if (strcmp(Obj.m_PlayerData->GuildName, "") == 0)
	{
		return;
	}

	if ((g_CastleSiege.CheckGuildOwnCastle(Obj.m_PlayerData->GuildName) == FALSE) || (Obj.m_PlayerData->GuildStatus != G_MASTER))
	{
		sLog->outError( "[CastleSiege] CGReqTaxMoneyInfo() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus);
	}
	else
	{
		GS_GDReqTaxInfo(g_MapServerManager.GetMapSvrGroup(), iIndex);
		sLog->outBasic("[CastleSiege] CGReqTaxMoneyInfo() REQ OK - [%s][%s], Guild:(%s)(%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus);
	}
}

void GameProtocol::GCAnsTaxMoneyInfo(CGameObject &Obj, int iResult, BYTE btTaxRateChaos, BYTE btTaxRateStore, __int64 i64Money)
{
	PMSG_ANS_TAXMONEYINFO pMsgResult;

	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x08, sizeof(pMsgResult));
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

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::CGReqTaxRateChange(PMSG_REQ_TAXRATECHANGE * lpMsg, CGameObject &Obj)
{
	int iMaxTaxRate;
	int iTaxRate;

	if (lpMsg == NULL)
	{
		return;
	}

	if (strcmp(Obj.m_PlayerData->GuildName, "") == 0)
	{
		return;
	}

	if ((g_CastleSiege.CheckGuildOwnCastle(Obj.m_PlayerData->GuildName) == FALSE) || (Obj.m_PlayerData->GuildStatus != G_MASTER))
	{
		sLog->outError( "[CastleSiege] CGReqTaxRateChange() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus);
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
			GCAnsTaxRateChange(iIndex, 0, 0, 0);
			sLog->outBasic("[CastleSiege] CGReqTaxRateChange() ERROR - Tax Rate Out of Range [%s][%s], Guild:(%s)(%d), TaxType:%d, TaxRate:%d", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus, lpMsg->btTaxType, iTaxRate);
		}
		else
		{
			GS_GDReqTaxRateChange(g_MapServerManager.GetMapSvrGroup(), iIndex, lpMsg->btTaxType, iTaxRate);
			sLog->outBasic("[CastleSiege] CGReqTaxRateChange() REQ OK - [%s][%s], Guild:(%s)(%d), TaxType:%d, TaxRate:%d", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus, lpMsg->btTaxType, iTaxRate);
		}
	}
}

void GameProtocol::GCAnsTaxRateChange(CGameObject &Obj, int iResult, BYTE btTaxType, int iTaxRate)
{
	PMSG_ANS_TAXRATECHANGE pMsgResult;
	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x09, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.btTaxType = btTaxType;
	pMsgResult.btTaxRate1 = SET_NUMBERH(SET_NUMBERHW(iTaxRate));
	pMsgResult.btTaxRate2 = SET_NUMBERL(SET_NUMBERHW(iTaxRate));
	pMsgResult.btTaxRate3 = SET_NUMBERH(SET_NUMBERLW(iTaxRate));
	pMsgResult.btTaxRate4 = SET_NUMBERL(SET_NUMBERLW(iTaxRate));
	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}



void GameProtocol::CGReqMoneyDrawOut(PMSG_REQ_MONEYDRAWOUT * lpMsg, CGameObject &Obj)
{
	int iMoneyChange;
	BOOL bRETVAL;

	if (lpMsg == NULL)
	{
		return;
	}

	if (strcmp(Obj.m_PlayerData->GuildName, "") == 0)
	{
		return;
	}

	if ((g_CastleSiege.CheckGuildOwnCastle(Obj.m_PlayerData->GuildName) == FALSE) || (Obj.m_PlayerData->GuildStatus != G_MASTER))
	{
		sLog->outError( "[CastleSiege] CGReqMoneyDrawOut() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus);
	}
	else
	{
		iMoneyChange = MAKE_NUMBERDW(MAKE_NUMBERW(lpMsg->btMoney4, lpMsg->btMoney3), MAKE_NUMBERW(lpMsg->btMoney2, lpMsg->btMoney1));

		if (iMoneyChange <= 0)
		{
			GCAnsMoneyDrawOut(iIndex, 0, 0);
			sLog->outBasic("[CastleSiege] CGReqMoneyDrawOut() ERROR - Req Money < 0 [%s][%s], Guild:(%s)(%d), Money:%d, TotMoney:%d", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus, iMoneyChange, Obj.m_PlayerData->Money);
		}
		else
		{
			bRETVAL = g_CastleSiege.CheckCastleHasMoney(iMoneyChange);

			if (bRETVAL == FALSE)
			{
				GCAnsMoneyDrawOut(iIndex, 0, 0);
				sLog->outBasic("[CastleSiege] CGReqMoneyDrawOut() ERROR - Castle Money is Low [%s][%s], Guild:(%s)(%d), Money:%d, TotMoney:%d", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus, iMoneyChange, Obj.m_PlayerData->Money);
			}
			else
			{
				if ((Obj.m_PlayerData->Money + iMoneyChange) > MAX_ZEN)
				{
					GCAnsMoneyDrawOut(iIndex, 0, 0);
				}
				else
				{
					GS_GDReqCastleMoneyChange(g_MapServerManager.GetMapSvrGroup(), iIndex, -iMoneyChange);
					sLog->outBasic("[CastleSiege] CGReqMoneyDrawOut() REQ OK - [%s][%s], Guild:(%s)(%d), Money:%d, TotMoney:%d", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus, iMoneyChange, Obj.m_PlayerData->Money);
				}
			}
		}
	}
}

void GameProtocol::GCAnsMoneyDrawOut(CGameObject &Obj, int iResult, __int64 i64Money)
{
	PMSG_ANS_MONEYDRAWOUT pMsgResult;
	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x10, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.btMoney1 = SET_NUMBERH(SET_NUMBERHW(HIDWORD(i64Money)));
	pMsgResult.btMoney2 = SET_NUMBERL(SET_NUMBERHW(HIDWORD(i64Money)));
	pMsgResult.btMoney3 = SET_NUMBERH(SET_NUMBERLW(HIDWORD(i64Money)));
	pMsgResult.btMoney4 = SET_NUMBERL(SET_NUMBERLW(HIDWORD(i64Money)));
	pMsgResult.btMoney5 = SET_NUMBERH(SET_NUMBERHW(LODWORD(i64Money)));
	pMsgResult.btMoney6 = SET_NUMBERL(SET_NUMBERHW(LODWORD(i64Money)));
	pMsgResult.btMoney7 = SET_NUMBERH(SET_NUMBERLW(LODWORD(i64Money)));
	pMsgResult.btMoney8 = SET_NUMBERL(SET_NUMBERLW(LODWORD(i64Money)));

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::GCAnsCsGateState(CGameObject &Obj, int iResult, int iGateIndex)
{
	PMSG_ANS_CSGATESTATE pMsgResult;
	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x11, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.btIndex1 = SET_NUMBERH((iGateIndex & 0xffff));
	pMsgResult.btIndex2 = SET_NUMBERL((iGateIndex & 0xffff));
	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::CGReqCsGateOperate(PMSG_REQ_CSGATEOPERATE * lpMsg, CGameObject &Obj)
{
	int iGateIndex;

	if (strcmp(Obj.m_PlayerData->GuildName, "") == 0)
	{
		return;
	}

	if (g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
	{
		if (Obj.m_btCsJoinSide != 1)
		{
			sLog->outBasic("[CastleSiege] CGReqCsGateOperate() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus);
			return;
		}
	}
	else
	{
		if ((g_CastleSiege.CheckCastleOwnerMember(Obj.m_Index) == FALSE) && (g_CastleSiege.CheckCastleOwnerUnionMember(Obj.m_Index) == FALSE))
		{
			sLog->outBasic("[CastleSiege] CGReqCsGateOperate() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus);
			return;
		}
	}

	iGateIndex = MAKE_NUMBERW(lpMsg->btIndex1, lpMsg->btIndex2);

	if (g_CastleSiege.CheckCsGateAlive(iGateIndex) == TRUE)
	{
		int iOperateGate = g_CastleSiege.OperateGate(iGateIndex, getGameObject(iGateIndex)->m_iCsNpcExistVal, lpMsg->btOperate);

		if (iOperateGate == 0)
		{
			GCAnsCsGateOperate(iIndex, 0, -1, 0);
			sLog->outBasic("[CastleSiege] CGReqCsGateOperate() ERROR - Operate Gate Failed [%s][%s], Guild:(%s)(%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus);
		}
		else
		{
			GCAnsCsGateOperate(iIndex, 1, iGateIndex, lpMsg->btOperate);
			g_CastleSiege.SendCsGateStateViewPort(iGateIndex, lpMsg->btOperate);
			sLog->outBasic("[CastleSiege] CGReqCsGateOperate() OK - [%s][%s], Guild:(%s)(%d), DOOR:(%d)(X:%d,Y:%d)(STATUE:%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildNumber, iGateIndex, getGameObject(iGateIndex]->X, getGameObject(iGateIndex)->Y, getGameObject(iGateIndex)->m_btCsGateOpen);
		}
	}
	else
	{
		GCAnsCsGateOperate(iIndex, 2, -1, 0);
		sLog->outBasic("[CastleSiege] CGReqCsGateOperate() ERROR - Gate Doesn't Exist [%s][%s], Guild:(%s)(%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus);
	}
}

void GameProtocol::GCAnsCsGateOperate(CGameObject &Obj, int iResult, int iGateIndex, int iGateOperate)
{
	PMSG_ANS_CSGATEOPERATE pMsgResult;

	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x12, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.btOperate = iGateOperate;
	pMsgResult.btIndex1 = SET_NUMBERH(iGateIndex & 0xffff); //??
	pMsgResult.btIndex2 = SET_NUMBERL(iGateIndex & 0xffff); //??
	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::GCAnsCsGateCurState(CGameObject &Obj, int iGateIndex, int iGateOperate)
{
	PMSG_ANS_CSGATECURSTATE pMsgResult;
	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x13, sizeof(pMsgResult));
	pMsgResult.btOperate = iGateOperate;
	pMsgResult.btIndex1 = SET_NUMBERH(iGateIndex & 0xffff); //??
	pMsgResult.btIndex2 = SET_NUMBERL(iGateIndex & 0xffff); //??
	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::GCAnsCsAccessSwitchState(CGameObject &Obj, int iSwitchIndex, int iSwitchUserIndex, BYTE btSwitchState)
{
	PMSG_ANS_NOTIFYSWITCHPROC pMsgResult;
	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x14, sizeof(pMsgResult));
	pMsgResult.btIndex1 = SET_NUMBERH(iSwitchIndex & 0xffff); //??
	pMsgResult.btIndex2 = SET_NUMBERL(iSwitchIndex & 0xffff); //??
	pMsgResult.btUserIndex1 = SET_NUMBERH(iSwitchUserIndex & 0xffff); //??
	pMsgResult.btUserIndex2 = SET_NUMBERL(iSwitchUserIndex & 0xffff); //??
	pMsgResult.btSwitchState = btSwitchState;
	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::GCAnsCsAccessCrownState(CGameObject &Obj, BYTE btCrownState)
{
	PMSG_ANS_NOTIFYCROWNPROC pMsgResult;
	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x15, sizeof(pMsgResult));
	pMsgResult.btCrownState = btCrownState;

	if (btCrownState == 0)
	{
		if (Obj.m_iAccumulatedCrownAccessTime < 0 || Obj.m_iAccumulatedCrownAccessTime > 61000) //season4.0 changed
		{
			Obj.m_iAccumulatedCrownAccessTime = 0;
		}

		sLog->outBasic("[CastleSiege] [Reg. Attempt] Accumulated Crown AccessTime : %d [%s](%s)(%s)", Obj.m_iAccumulatedCrownAccessTime, Obj.m_PlayerData->GuildName, Obj.AccountID, Obj.Name);
	}

	if (btCrownState == 1)
	{
		sLog->outBasic("[CastleSiege] [Reg. Success] Accumulated Crown AccessTime : %d [%s](%s)(%s)", Obj.m_iAccumulatedCrownAccessTime, Obj.m_PlayerData->GuildName, Obj.AccountID, Obj.Name);
		Obj.m_iAccumulatedCrownAccessTime = 0;
	}

	if (btCrownState == 2)
	{
		Obj.m_iAccumulatedCrownAccessTime += GetTickCount() - g_CastleSiege.GetCrownAccessTickCount();

		if (Obj.m_iAccumulatedCrownAccessTime < 0 || Obj.m_iAccumulatedCrownAccessTime > 61000) //season4.0 changed
		{
			Obj.m_iAccumulatedCrownAccessTime = 0;
		}

		if (Obj.Live == FALSE)
		{
			if (g_CastleSiege.CastleSiegeResetAccumulationAfterKill == TRUE)
			{
				Obj.m_iAccumulatedCrownAccessTime = 0;
			}
		}

		sLog->outBasic("[CastleSiege] [Reg. Fail] Accumulated Crown AccessTime : %d [%s](%s)(%s)", Obj.m_iAccumulatedCrownAccessTime, Obj.m_PlayerData->GuildName, Obj.AccountID, Obj.Name);
	}

	pMsgResult.dwAccumulatedCrownAccessTime = Obj.m_iAccumulatedCrownAccessTime; //??

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}
void GameProtocol::GCAnsCsNotifyStart(CGameObject &Obj, BYTE btStartState)
{
	PMSG_ANS_NOTIFYCSSTART pMsgResult;
	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x17, sizeof(pMsgResult));
	pMsgResult.btStartState = btStartState;
	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::GCAnsCsNotifyProgress(CGameObject &Obj, BYTE btCastleSiegeState, LPCSTR lpszGuildName)
{
	if (lpszGuildName == NULL)
	{
		return;
	}

	PMSG_ANS_NOTIFYCSPROGRESS pMsgResult;

	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x18, sizeof(pMsgResult));
	pMsgResult.btCastleSiegeState = btCastleSiegeState;
	memset(pMsgResult.szGuildName, 0, sizeof(pMsgResult.szGuildName));
	memcpy(pMsgResult.szGuildName, lpszGuildName, sizeof(pMsgResult.szGuildName));

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::GCAnsCsMapSvrTaxInfo(CGameObject &Obj, BYTE btTaxType, BYTE btTaxRate)
{
	PMSG_ANS_MAPSVRTAXINFO pMsgResult;

	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x1A, sizeof(pMsgResult));
	pMsgResult.btTaxType = btTaxType;
	pMsgResult.btTaxRate = btTaxRate;

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsgResult, pMsgResult.h.size);

}

void GameProtocol::CGReqCsMiniMapData(PMSG_REQ_MINIMAPDATA * lpMsg, CGameObject &Obj)
{
	if (gObjIsConnected(Obj.m_Index) == FALSE)
	{
		return;
	}

	if ((Obj.m_btCsJoinSide <= 0) || (g_CastleSiege.CheckUnionGuildMaster(Obj.m_Index) == FALSE))
	{
		GCAnsCsMiniMapData(iIndex, 3);
	}
	else if (g_CastleSiege.GetCastleState() != CASTLESIEGE_STATE_STARTSIEGE)
	{
		GCAnsCsMiniMapData(iIndex, 2);
	}
	else
	{
		g_CastleSiege.AddMiniMapDataReqUser(Obj.m_Index);
		GCAnsCsMiniMapData(iIndex, 1);
	}
}

void GameProtocol::GCAnsCsMiniMapData(CGameObject &Obj, BYTE btResult)
{
	PMSG_ANS_MINIMAPDATA pMsgResult;

	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x1B, sizeof(pMsgResult));
	pMsgResult.btResult = btResult;

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}


void GameProtocol::CGReqStopCsMiniMapData(PMSG_REQ_STOPMINIMAPDATA * lpMsg, CGameObject &Obj)
{
	if (gObjIsConnected(Obj.m_Index) == FALSE)
	{
		return;
	}

}


void GameProtocol::CGReqCsSendCommand(PMSG_REQ_CSCOMMAND * lpMsg, CGameObject &Obj)
{
	if (gObjIsConnected(Obj.m_Index) == FALSE)
	{
		return;
	}

	if (Obj.m_btCsJoinSide <= 0 || g_CastleSiege.CheckUnionGuildMaster(Obj.m_Index) == FALSE)
	{
		return;
	}

	if (g_CastleSiege.GetCastleState() != CASTLESIEGE_STATE_STARTSIEGE)
	{
		return;
	}

	GCAnsCsSendCommand(Obj.m_btCsJoinSide, lpMsg->btTeam, lpMsg->btX, lpMsg->btY, lpMsg->btCommand);
}

void GameProtocol::GCAnsCsSendCommand(int iCsJoinSize, BYTE btTeam, BYTE btX, BYTE btY, BYTE btCommand)
{
	PMSG_ANS_CSCOMMAND pMsgResult;

	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x1D, sizeof(pMsgResult));

	pMsgResult.btTeam = btTeam;
	pMsgResult.btX = btX;
	pMsgResult.btY = btY;
	pMsgResult.btCommand = btCommand;

	for (int iIndex = g_ConfigRead.server.GetObjectStartUserIndex(); iIndex < g_ConfigRead.server.GetObjectMax(); iIndex++)
	{
		if (gObjIsConnected(Obj.m_Index) == FALSE) continue;

		if ((Obj.m_btCsJoinSide == iCsJoinSize) && (Obj.MapNumber == MAP_INDEX_CASTLESIEGE))
		{
			IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
		}
	}
}

void GameProtocol::GCAnsCsLeftTimeAlarm(BYTE btHour, BYTE btMinute)
{
	PMSG_ANS_CSLEFTTIMEALARM pMsgResult;

	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x1E, sizeof(pMsgResult));
	pMsgResult.btHour = btHour;
	pMsgResult.btMinute = btMinute;

	for (int iIndex = g_ConfigRead.server.GetObjectStartUserIndex(); iIndex < g_ConfigRead.server.GetObjectMax(); iIndex++)
	{
		if (!gObjIsConnected(Obj.m_Index))
		{
			continue;
		}

		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
	}

}

void GameProtocol::GCAnsSelfCsLeftTimeAlarm(CGameObject &Obj, BYTE btHour, BYTE btMinute)
{
	PMSG_ANS_CSLEFTTIMEALARM pMsgResult;

	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x1E, sizeof(pMsgResult));
	pMsgResult.btHour = btHour;
	pMsgResult.btMinute = btMinute;

	if (!gObjIsConnected(Obj.m_Index))
	{
		return;
	}

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::CGReqCsSetEnterHuntZone(PMSG_REQ_CSHUNTZONEENTER * lpMsg, CGameObject &Obj)
{
	if (gObjIsConnected(Obj.m_Index) == FALSE)
	{
		return;
	}

	if (g_CastleSiege.CheckCastleOwnerMember(Obj.m_Index) == FALSE)
	{
		GCAnsCsSetEnterHuntZone(iIndex, 2, lpMsg->btHuntZoneEnter);
	}
	else if (g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
	{
		GCAnsCsSetEnterHuntZone(iIndex, 0, lpMsg->btHuntZoneEnter);
	}
	else
	{
		g_CastleSiege.SetHuntZoneEnter(lpMsg->btHuntZoneEnter);
		GCAnsCsSetEnterHuntZone(iIndex, 1, lpMsg->btHuntZoneEnter);
	}
}

void GameProtocol::GCAnsCsSetEnterHuntZone(CGameObject &Obj, BYTE btResult, BYTE btEnterHuntZone)
{
	PMSG_ANS_CSHUNTZONEENTER pMsgResult;

	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x1F, sizeof(pMsgResult));
	pMsgResult.btResult = btResult;
	pMsgResult.btHuntZoneEnter = btEnterHuntZone;

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::CGReqNpcDbList(PMSG_REQ_NPCDBLIST * lpMsg, CGameObject &Obj)
{
	PMSG_ANS_NPCDBLIST pResult;

	if (lpMsg == NULL)
	{
		return;
	}

	if (strcmp(Obj.m_PlayerData->GuildName, "") == 0)
	{
		return;
	}

	if (g_CastleSiege.CheckGuildOwnCastle(Obj.m_PlayerData->GuildName) == FALSE || ((Obj.m_PlayerData->GuildStatus != 128) && (Obj.m_PlayerData->GuildStatus != 64)))
	{
		sLog->outError( "[CastleSiege] CGReqNpcDbList() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus);
		pResult.h.set((BYTE*)&pResult, 0xB3, sizeof(pResult));
		pResult.iCount = 0;
		pResult.btResult = 2;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, sizeof(pResult));
	}
	else
	{
		switch (lpMsg->btMonsterCode)
		{
		case 1:
			g_CastleSiege.SendNpcStateList(iIndex, 277);
			break;
		case 2:
			g_CastleSiege.SendNpcStateList(iIndex, 283);
			break;
		}
	}
}

void GameProtocol::CGReqCsRegGuildList(PMSG_REQ_CSREGGUILDLIST * lpMsg, CGameObject &Obj)
{
	if (lpMsg == NULL) return;

	GS_GDReqAllGuildMarkRegInfo(g_MapServerManager.GetMapSvrGroup(), iIndex);
}

void GameProtocol::CGReqCsAttkGuildList(PMSG_REQ_CSATTKGUILDLIST * lpMsg, CGameObject &Obj)
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

	lpMsgSend->h.set((BYTE*)lpMsgSend, 0xB5, ((lpMsgSend->iCount * sizeof(PMSG_CSATTKGUILDLIST)) + sizeof(PMSG_ANS_CSATTKGUILDLIST)));
	IOCP.DataSend(Obj.m_Index, (BYTE*)cBUFFER, ((lpMsgSend->iCount * sizeof(PMSG_CSATTKGUILDLIST)) + sizeof(PMSG_ANS_CSATTKGUILDLIST)));
}

void GameProtocol::CGReqWeaponUse(PMSG_REQ_USEWEAPON * aRecv, CGameObject &Obj)
{
	WORD wObjIndex = MAKE_NUMBERW(aRecv->btObjIndexH, aRecv->btObjIndexL);

	if (!ObjectMaxRange(wObjIndex))
	{
		return;
	}

	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	CGameObject lpOwnerObj = Obj;
	CGameObject lpWeaponObj = &getGameObject(wObjIndex);

	if (lpOwnerObj.Type != OBJ_USER)
	{
		return;
	}

	if (lpWeaponObj.Type != OBJ_NPC)
	{
		return;
	}

	PMSG_ANS_USEWEAPON pMsg = { 0 };

	PHeadSubSetB((BYTE*)&pMsg, 0xB7, 0x01, sizeof(pMsg));
	pMsg.btResult = 1;

	if (lpWeaponObj.Class == 221)	// Slingshot attack
	{
		pMsg.btWeaponType = 1;
		pMsg.btObjIndexH = SET_NUMBERH(wObjIndex);
		pMsg.btObjIndexL = SET_NUMBERL(wObjIndex);
		g_CsNPC_Weapon.GetTargetPointXY(lpWeaponObj.Class, aRecv->btTargetIndex - 1, pMsg.btPointX, pMsg.btPointY, TRUE);
	}
	else if (lpWeaponObj.Class == 222)	// Slingshot defense
	{
		pMsg.btWeaponType = 2;
		pMsg.btObjIndexH = SET_NUMBERH(wObjIndex);
		pMsg.btObjIndexL = SET_NUMBERL(wObjIndex);
		g_CsNPC_Weapon.GetTargetPointXY(lpWeaponObj.Class, aRecv->btTargetIndex - 1, pMsg.btPointX, pMsg.btPointY, TRUE);
	}

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, sizeof(pMsg));

	pMsg.btResult = 2;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpWeaponObj.VpPlayer2[n].type == OBJ_USER)
		{
			if (lpWeaponObj.VpPlayer2[n].state)
			{
				if (iIndex != lpWeaponObj.VpPlayer2[n].number)
				{
					IOCP.DataSend(lpWeaponObj.VpPlayer2[n].number, (BYTE*)&pMsg, sizeof(pMsg));
				}
			}
		}
	}

	gObjNotifyUseWeaponV1(lpOwnerObj, lpWeaponObj, pMsg.btPointX, pMsg.btPointY);
	g_CsNPC_Weapon.SetWeaponCalDamageInfo(wObjIndex, pMsg.btPointX, pMsg.btPointY, 10000);
}

void GameProtocol::CGReqWeaponDamageValue(PMSG_REQ_WEAPON_DAMAGE_VALUE * aRecv, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	BYTE btTargetX = 0;
	BYTE btTargetY = 0;
	WORD wObjIndex = MAKE_NUMBERW(aRecv->btObjIndexH, aRecv->btObjIndexL);

	if (g_CsNPC_Weapon.GetWeaponCalDamageInfo(wObjIndex, btTargetX, btTargetY) == TRUE)
	{
		gObjNotifyUseWeaponDamage(&getGameObject(wObjIndex), btTargetX, btTargetY);
	}
	else
	{
		MsgOutput(iIndex, Lang.GetText(0, 181));
	}
}

void GameProtocol::GCSendObjectCreationState(int iObjectIndex)
{
	if (!ObjectMaxRange(iObjectIndex))
	{
		return;
	}

	CGameObject lpObj = &getGameObject(iObjectIndex);
	PMSG_NOTIFY_OBJECT_CREATION_STATE	pMsg = { 0 };

	if (lpObj.Class == 278)
	{
		PHeadSubSetB((BYTE*)&pMsg, 0xB9, 0x01, sizeof(pMsg));
		pMsg.btObjIndexH = SET_NUMBERH(iObjectIndex);
		pMsg.btObjIndexL = SET_NUMBERL(iObjectIndex);
		pMsg.btCreationState = lpObj.m_btCreationState;
		MsgSendV2(lpObj, (BYTE*)&pMsg, sizeof(pMsg));
	}
}

void GameProtocol::CGReqGuildMarkOfCastleOwner(PMSG_REQ_GUILDMARK_OF_CASTLEOWNER * aRecv, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	CGameObject lpObj = Obj;
	char szGuildName[MAX_GUILD_LEN + 1] = { 0 };

	memcpy(szGuildName, g_CastleSiegeSync.GetCastleOwnerGuild(), MAX_GUILD_LEN);

	if (szGuildName)
	{
		GUILD_INFO_STRUCT * lpGuild = Guild.SearchGuild(szGuildName);

		if (lpGuild != NULL)
		{
			PMSG_ANS_GUILDMARK_OF_CASTLEOWNER pMsg = { 0 };

			PHeadSubSetB((BYTE*)&pMsg, 0xB9, 0x02, sizeof(pMsg));
			memcpy(pMsg.GuildMarkOfCastleOwner, lpGuild->Mark, sizeof(pMsg.GuildMarkOfCastleOwner));

			IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, sizeof(pMsg));
		}
	}
}

void GameProtocol::CGReqCastleHuntZoneEntrance(PMSG_REQ_MOVE_TO_CASTLE_HUNTZONE * aRecv, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	CGameObject lpObj = Obj;

	PMSG_ANS_MOVE_TO_CASTLE_HUNTZONE pMsg = { 0 };

	PHeadSubSetB((BYTE*)&pMsg, 0xB9, 0x05, sizeof(pMsg));

	pMsg.btResult = 0;

	int iEnterTaxMoney = 0;

	iEnterTaxMoney = g_CastleSiegeSync.GetTaxHuntZone(lpObj.m_Index, TRUE);

	if (iEnterTaxMoney < 0)
	{
		iEnterTaxMoney = 0;
	}

	BOOL bPermission = FALSE;

	if (g_CastleSiege.GetHuntZoneEnter())
	{
		bPermission = TRUE;
	}

	if (lpObj.m_PlayerData->lpGuild)
	{
		if (g_CastleSiege.CheckCastleOwnerMember(lpObj.m_Index))
		{
			bPermission = TRUE;
		}

		if (g_CastleSiege.CheckCastleOwnerUnionMember(lpObj.m_Index))
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
		if (lpObj.m_PlayerData->Money >= iEnterTaxMoney)
		{
			if (lpObj.MapNumber == MAP_INDEX_CASTLESIEGE)
			{
				if (gObjMoveGate(lpObj.m_Index, 95) == TRUE)
				{
					pMsg.btResult = 1;
					int iOldMoney = lpObj.m_PlayerData->Money;

					lpObj.m_PlayerData->Money -= iEnterTaxMoney;

					g_CastleSiegeSync.AddTributeMoney(iEnterTaxMoney);

					if (lpObj.m_PlayerData->Money < 0)
					{
						lpObj.m_PlayerData->Money = 0;
					}

					GCMoneySend(lpObj.m_Index, lpObj.m_PlayerData->Money);

					sLog->outBasic("[Castle HuntZone] [%s][%s] - Entrance TAX : %d - %d = %d", lpObj.AccountID, lpObj.Name, iOldMoney, iEnterTaxMoney, lpObj.m_PlayerData->Money);
				}
			}
		}
	}

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, sizeof(pMsg));
}

void GameProtocol::CGReqJewelMix(PMSG_REQ_JEWEL_MIX * lpMsg, CGameObject &Obj)
{
	if (lpMsg == NULL)
		return;

	bool bCheck = false;
	Check_SameSerialItem(iIndex, 3, bCheck);

	if (bCheck == true)
		return;
	CJewelMixSystem::MixJewel(iIndex, lpMsg->btJewelType, lpMsg->btJewelMix);
}

void GameProtocol::GCAnsJewelMix(CGameObject &Obj, int iResult)
{
	PMSG_ANS_JEWEL_UNMIX pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xBC, 0x00, sizeof(pMsg));
	pMsg.btResult = iResult;

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, sizeof(pMsg));
}


void GameProtocol::CGReqJewelUnMix(PMSG_REQ_JEWEL_UNMIX * lpMsg, CGameObject &Obj)
{
	if (lpMsg == NULL)
		return;

	bool bCheck = false;
	Check_SameSerialItem(iIndex, 3, bCheck);

	if (bCheck == true)
		return;
	CJewelMixSystem::UnMixJewel(iIndex, lpMsg->btJewelType, lpMsg->btJewelLevel, lpMsg->btJewelPos);
}

void GameProtocol::GCAnsJewelUnMix(CGameObject &Obj, int iResult)
{
	PMSG_ANS_JEWEL_UNMIX pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xBC, 0x01, sizeof(pMsg));
	pMsg.btResult = iResult;

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, sizeof(pMsg));
}

void GameProtocol::CGReqCrywolfInfo(PMSG_REQ_CRYWOLF_INFO* lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
		return;

	CGameObject lpObj = Obj;

	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		GCAnsCrywolfInfo(iIndex, g_Crywolf.GetOccupationState(), g_Crywolf.GetCrywolfState());
	}

	else
	{
		GCAnsCrywolfInfo(iIndex, g_CrywolfSync.GetOccupationState(), g_CrywolfSync.GetCrywolfState());
	}
}

void GameProtocol::GCAnsCrywolfInfo(CGameObject &Obj, BYTE btOccupationState, BYTE btCrywolfState)
{
	PMSG_ANS_CRYWOLF_INFO pMsg = { 0 };

	PHeadSubSetB((BYTE*)&pMsg, 0xBD, 0x00, sizeof(pMsg));
	pMsg.btOccupationState = btOccupationState;
	pMsg.btCrywolfState = btCrywolfState;

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, sizeof(pMsg));
}


struct PMSG_ANS_CRYWOLF_ALTAR_CONTRACT
{
	PBMSG_HEAD2 h;	// C1:BD:03
	BYTE btResult;	// 4
	BYTE btAltarState;	// 5
	BYTE btObjIndexH;	// 6
	BYTE btObjIndexL;	// 7
};

void GameProtocol::CGReqAlatrContract(PMSG_REQ_CRYWOLF_ALTAR_CONTRACT* lpMsg, CGameObject &Obj)
{
	int iAltarIndex = MAKE_NUMBERW(lpMsg->btObjIndexH, lpMsg->btObjIndexL);

	if (!ObjectMaxRange(Obj.m_Index))
		return;

	if (!ObjectMaxRange(iAltarIndex))
		return;

	CGameObject lpObj = Obj;
	CGameObject lpAltarObj = &getGameObject(iAltarIndex);

	if (!CRYWOLF_ALTAR_CLASS_RANGE(lpAltarObj.Class))
		return;

	if (!gObjIsConnected(lpObj))
		return;

	PMSG_ANS_CRYWOLF_ALTAR_CONTRACT pMsg = { 0 };

	PHeadSubSetB((BYTE*)&pMsg, 0xBD, 0x03, sizeof(pMsg));
	pMsg.btResult = 0;
	pMsg.btAltarState = g_CrywolfNPC_Altar.GetAltarState(lpAltarObj.Class);
	pMsg.btObjIndexH = lpMsg->btObjIndexH;
	pMsg.btObjIndexL = lpMsg->btObjIndexL;

	if (pMsg.btAltarState == 0 &&
		lpObj.Type == OBJ_USER &&
		lpObj.Class == CLASS_ELF &&
		lpObj.Level >= MIN_ELF_LEVEL_ALTAR)
	{
		if (g_CrywolfNPC_Altar.SetAltarUserIndex(iAltarIndex, lpAltarObj.Class, iIndex) != FALSE)
		{
			pMsg.btResult = 1;
		}
	}
	else
	{
		MsgOutput(iIndex, Lang.GetText(0, 256), (int)MIN_ELF_LEVEL_ALTAR);
	}

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, sizeof(pMsg));
}

struct PMSG_ANS_CRYWOLF_BENEFIT_PLUS_CHAOSRATE
{
	PBMSG_HEAD2 h;	// C1:BD:09
	BYTE btPlusChaosRate;	// 4
};


void GameProtocol::CGReqPlusChaosRate(PMSG_REQ_CRYWOLF_BENEFIT_PLUS_CHAOSRATE* lpMsg, CGameObject &Obj)
{
	if (ObjectMaxRange(Obj.m_Index) == FALSE)
		return;

	PMSG_ANS_CRYWOLF_BENEFIT_PLUS_CHAOSRATE pMsg = { 0 };

	PHeadSubSetB((BYTE*)&pMsg, 0xBD, 0x09, sizeof(pMsg));
	pMsg.btPlusChaosRate = g_CrywolfSync.GetPlusChaosRate();

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, sizeof(pMsg));
}

void GameProtocol::CGReqKanturuStateInfo(PMSG_REQ_KANTURU_STATE_INFO* lpMsg, CGameObject &Obj)
{
	if (ObjectMaxRange(Obj.m_Index) == FALSE)
		return;

	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		g_KanturuEntranceNPC.NotifyEntranceInfo(Obj.m_Index);
	}
}


void GameProtocol::GCReqEnterKanturuBossMap(PMSG_REQ_ENTER_KANTURU_BOSS_MAP* lpMsg, CGameObject &Obj)
{
	if (ObjectMaxRange(Obj.m_Index) == FALSE)
		return;

	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		g_KanturuEntranceNPC.NotifyResultEnterKanturuBossMap(Obj.m_Index);
	}
}

void GameProtocol::WereWolfQuarrelCheck(CGameObject &Obj)
{
	if (ObjectMaxRange(aIndex) == FALSE)
	{
		return;
	}

	if (lpObj.Type != OBJ_USER || lpObj.Connected <= PLAYER_LOGGED)
	{
		return;
	}

	g_QuestInfo.CheckQuestMapEnterOnWerwolf(aIndex);
}

void GameProtocol::GateKeeperCheck(CGameObject &Obj)
{
	if (ObjectMaxRange(aIndex) == FALSE)
	{
		return;
	}

	if (lpObj.Type != OBJ_USER || lpObj.Connected <= PLAYER_LOGGED)
	{
		return;
	}

	g_QuestInfo.CheckQuestMapEnterOnGateKeeper(aIndex);
}

struct PMSG_ILLUSIONTEMPLE_KILLPOINT
{
	PBMSG_HEAD2 h;
	BYTE btKillPoint;
};

void GameProtocol::GCSendIllusionTempleKillPoint(CGameObject &Obj, BYTE KillPoint)
{
	PMSG_ILLUSIONTEMPLE_KILLPOINT pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xBF, 0x06, sizeof(pMsg));
	pMsg.btKillPoint = KillPoint;

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::CGReqIllusionTempleUseSkill(PMSG_REQ_USEILLUSIONTEMPLESKILL * aRecv, CGameObject &Obj)
{
	if (ObjectMaxRange(aIndex) == false)
	{
		return;
	}

	g_IT_Event.ITR_USeSkill(aIndex, MAKE_NUMBERW(aRecv->MagicNumberH, aRecv->MagicNumberL), MAKE_NUMBERW(aRecv->btTargetObjIndexH, aRecv->btTargetObjIndexL), aRecv->btDis);
}

void GameProtocol::GCIllusionTempleSendReward(CGameObject &Obj)
{
	if (ObjectMaxRange(aIndex) == false)
	{
		return;
	}

	g_IT_Event.ReqEventReward(aIndex);
}

void GameProtocol::GCMasterLevelUpMsgSend(CGameObject &Obj)
{
	if (!ObjectMaxRange(aIndex))
		return;

	PMSG_MASTER_LEVEL_UP_SEND pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x51, sizeof(pMsg));

	pMsg.MasterLevel = lpObj.m_PlayerData->MasterLevel;
	pMsg.MLPoint = lpObj.m_PlayerData->MasterPoint;
	pMsg.GainPoint = g_MaxStatsInfo.GetClass.MLPointPerLevel;
	pMsg.MaxPoint = g_MaxStatsInfo.GetClass.MLUserMaxLevel;
	pMsg.MaxLife = lpObj.MaxLife + lpObj.AddLife;
	pMsg.MaxMana = lpObj.MaxMana + lpObj.AddMana;
	pMsg.MaxBP = lpObj.MaxBP + lpObj.AddBP;
	pMsg.MaxShield = lpObj.iMaxShield + lpObj.iAddShield;
	pMsg.IGCMaxLife = lpObj.MaxLife + lpObj.AddLife;
	pMsg.IGCMaxMana = lpObj.MaxMana + lpObj.AddMana;
	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

	GCSendEffectInfo(aIndex, 16);
}

struct PMSG_REQ_REGISTER_LUCKYCOIN
{
	PBMSG_HEAD2 h;
	int iIndex;
	char szAccountID[11];
	BYTE btPos;
};

void GameProtocol::CGReqLuckyCoinInfo(CGameObject &Obj)
{
	if (lpObj.UseEventServer == FALSE)
	{
		PMSG_REQ_LUCKYCOIN pMsg = { 0 };
		PHeadSubSetB((BYTE*)&pMsg, 0xBE, 0x19, sizeof(pMsg));

		memcpy(pMsg.szUID, lpObj.AccountID, 10);
		pMsg.szUID[10] = 0;
		pMsg.iIndex = aIndex;

		wsDataCli.DataSend((PCHAR)&pMsg, pMsg.h.size);

		lpObj.UseEventServer = TRUE;
	}
}

void GameProtocol::CGReqLuckyCoinRegister(PMSG_REQ_LUCKYCOIN_REGISTER *aRecv, CGameObject &Obj)
{
	

	if (gObjIsConnectedGP(aIndex) == false)
	{
		return;
	}

	if (lpObj.m_IfState.type == 1)
	{
		return;
	}

	if (lpObj.m_IfState.type != 7)
	{
		return;
	}

	if (lpObj.UseEventServer == FALSE)
	{
		BYTE btPos = 0xFF;

		for (int i = INVENTORY_BAG_START; i < MAIN_INVENTORY_SIZE; i++)
		{
			if (lpObj.pInventory[i].IsItem() == TRUE)
			{
				if (lpObj.pInventory[i].m_Type == ITEMGET(14, 100))
				{
					btPos = i;
					break;
				}
			}
		}

		if (btPos == 0xFF)
		{
			PMSG_ANS_LUCKYCOIN_REGISTER pMsg;
			PHeadSubSetB((BYTE*)&pMsg, 0xBF, 0x0C, sizeof(pMsg));

			pMsg.btResult = 0;
			pMsg.iLuckyCoin = 0;

			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

			return;
		}

		lpObj.UseEventServer = TRUE;

		PMSG_REQ_REGISTER_LUCKYCOIN pMsg = { 0 };
		PHeadSubSetB((BYTE*)&pMsg, 0xBE, 0x18, sizeof(pMsg));

		pMsg.btPos = btPos;
		pMsg.iIndex = aIndex;
		memcpy(pMsg.szAccountID, lpObj.AccountID, 10);

		wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);

	}
	return;
}

void GameProtocol::CGReqLuckyCoinTrade(PMSG_REQ_LUCKYCOIN_TRADE * aRecv, CGameObject &Obj)
{
	//CItemObjectBagEx * Bag = NULL;
	

	if (gObjIsConnectedGP(aIndex) == false)
	{
		return;
	}

	if (lpObj.m_IfState.type == 1)
	{
		return;
	}

	if (lpObj.m_IfState.type != 7)
	{
		return;
	}

	int Count = 0;
	CItemObject Item;
	time_t Duration;
	int iBagResult = 0;

	for (int i = INVENTORY_BAG_START; i < MAIN_INVENTORY_SIZE; ++i)
	{
		if (lpObj.pInventory[i].IsItem() == TRUE && lpObj.pInventory[i].m_Type == ITEMGET(14, 100))
			Count += lpObj.pInventory[i].m_Durability;
	}

	if (Count < aRecv->iLuckyCoinTradeCount)
	{
		GCAnsLuckyCoinTrade(aIndex, 0);
		return;
	}

	if (!CheckInventoryEmptySpace(lpObj, 4, 2))
	{
		GCAnsLuckyCoinTrade(aIndex, 2);
		return;
	}

	Count = aRecv->iLuckyCoinTradeCount;

	for (int i = INVENTORY_BAG_START; i < MAIN_INVENTORY_SIZE; ++i)
	{
		if (lpObj.pInventory[i].IsItem() == TRUE && lpObj.pInventory[i].m_Type == ITEMGET(14, 100))
		{
			if (lpObj.pInventory[i].m_Durability > Count)
			{
				lpObj.pInventory[i].m_Durability -= Count;
				GCItemObjectDurSend2(aIndex, i, lpObj.pInventory[i].m_Durability, 0);
				Count = 0;
			}
			else
			{
				Count -= lpObj.pInventory[i].m_Durability;
				gObjInventoryDeleteItem(aIndex, i);
				GCInventoryItemDeleteSend(aIndex, i, 1);
			}
			if (Count == 0)
				break;
		}
	}

	switch (aRecv->iLuckyCoinTradeCount)
	{
	case 10:
		iBagResult = g_BagManager.GetItemFromBag(aIndex, BAG_EVENT, EVENTBAG_LUCKYCOIN10, aIndex, Item, Duration);
		break;
	case 20:
		iBagResult = g_BagManager.GetItemFromBag(aIndex, BAG_EVENT, EVENTBAG_LUCKYCOIN20, aIndex, Item, Duration);
		break;
	case 30:
		iBagResult = g_BagManager.GetItemFromBag(aIndex, BAG_EVENT, EVENTBAG_LUCKYCOIN30, aIndex, Item, Duration);
		break;
	default:
		GCAnsLuckyCoinTrade(aIndex, 4);
		return;
	}

	GJSetCharacterInfo(lpObj, aIndex, FALSE);

	if (iBagResult == 0)
	{
		GCAnsLuckyCoinTrade(aIndex, 2);
		return;
	}

	else if (iBagResult == 2 || iBagResult == 3)
	{
		GCAnsLuckyCoinTrade(aIndex, 1);
		return;
	}

	ItemSerialCreateSend(aIndex, 235, 0, 0, Item.m_Type, Item.m_Level, Item.m_Durability, Item.m_Option1, Item.m_Option2, Item.m_Option3, aIndex, Item.m_NewOption, Item.m_SetOption, Duration, 0, 0);
	//sLog->outBasic("[Lucky Coin] [%s][%s] Trade Lucky Coin [%d]", lpObj.AccountID, lpObj.Name, aRecv->iLuckyCoinTradeCount);
	Item.Convert(Item.m_Type, Item.m_Option1, Item.m_Option2, Item.m_Option3, Item.m_NewOption, Item.m_SetOption, Item.m_ItemOptionEx, 0, 0, Duration, CURRENT_DB_VERSION);
}

void GameProtocol::GCAnsLuckyCoinTrade(CGameObject &Obj, BYTE Result)
{
	PMSG_ANS_LUCKYCOIN_TRADE pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xBF, 0x0D, sizeof(pMsg));

	pMsg.btResult = Result;

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCSendObjectHP(CGameObject &Obj, int aTargetIndex)
{
	PMSG_TARGET_HP pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xFA, 0x05, sizeof(pMsg));

	pMsg.m_Index = aTargetIndex;
	pMsg.m_Hp = getGameObject(aTargetIndex)->Life;
	pMsg.m_MaxHp = getGameObject(aTargetIndex]->MaxLife + getGameObject(aTargetIndex)->AddLife;
	pMsg.m_Level = getGameObject(aTargetIndex)->Level;
	pMsg.m_bIceEffect = gObjCheckUsedBuffEffect(&getGameObject(aTargetIndex), BUFFTYPE_FREEZE);
	pMsg.m_bPoisonEffect = gObjCheckUsedBuffEffect(&getGameObject(aTargetIndex), BUFFTYPE_POISON);

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
	this->MsgSendV2(&getGameObject(aTargetIndex), (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::CGReqAntiCheatRecv(CGameObject &Obj, PMSG_SEND_AH_INFO *aRecv)
{
	if (!ObjectMaxRange(aIndex))
	{
		return;
	}

	if (!gObjIsConnected(aIndex))
	{
		return;
	}

	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	if (lpObj.Connected != PLAYER_PLAYING)
	{
		return;
	}

	if (lpObj.m_PlayerData->LivePacketTime == 0 || g_ConfigRead.antihack.EnablePacketTimeCheck == FALSE)
	{
		lpObj.m_PlayerData->LivePacketTime = GetTickCount();
	}

	else
	{
		DWORD tick = GetTickCount() - lpObj.m_PlayerData->LivePacketTime;

		if (tick < g_ConfigRead.antihack.PacketTimeMinTime)
		{
			sLog->outError( "[ANTI-HACK] (%s)(%s)(%s) (Map:%d)(X:%d)(Y:%d) SpeedHack detected -> Tick (Current:%d/Minimum:%d)",
				lpObj.AccountID, lpObj.Name, lpObj.m_PlayerData->Ip_addr, lpObj.MapNumber, lpObj.X, lpObj.Y, tick, g_ConfigRead.antihack.PacketTimeMinTime);

			AntiHackLog->Output("[ANTI-HACK] (%s)(%s)(%s) (Map:%d)(X:%d)(Y:%d) SpeedHack detected -> Tick (Current:%d/Minimum:%d)",
				lpObj.AccountID, lpObj.Name, lpObj.m_PlayerData->Ip_addr, lpObj.MapNumber, lpObj.X, lpObj.Y, tick, g_ConfigRead.antihack.PacketTimeMinTime);

			lpObj.m_PlayerData->LiveCheckDetectCount++;

			if (lpObj.m_PlayerData->LiveCheckDetectCount >= 3)
			{

				if (g_ConfigRead.antihack.EnableHackDetectMessage == TRUE)
				{
					MsgOutput(aIndex, (char *)g_ConfigRead.antihack.HackDetectMessage.c_str());
				}

				if (g_ConfigRead.EnableAutoBanUserAccount == TRUE)
				{
					GDReqBanUser(aIndex, 1, 1);
				}

				//this->GCSendDisableReconnect(aIndex);
				IOCP.CloseClient(aIndex);

				return;
			}
		}

		else
		{
			lpObj.m_PlayerData->LivePacketTime = GetTickCount();
		}
	}

	if (g_ConfigRead.antihack.EnabledAgiCheck == false)
	{
		lpObj.AgiCheckTime = GetTickCount();
		return;
	}

	if (lpObj.m_PlayerData->BlessSkillCast == true)
	{
		lpObj.AgiCheckTime = GetTickCount();
		return;
	}

	WORD Agility = lpObj.m_PlayerData->Dexterity + lpObj.AddDexterity;

	int AgilityDiff = (lpObj.m_PlayerData->Dexterity + lpObj.AddDexterity) - aRecv->Agi;

	if (aRecv->type == 1)
	{
		if (AgilityDiff <= -30 || AgilityDiff >= 30)
		{
			sLog->outError( "[ANTI-HACK] (%s)(%s)(%s) Dexterity check error: %d/%d", lpObj.AccountID, lpObj.Name, lpObj.m_PlayerData->Ip_addr,
				Agility, aRecv->Agi);

			AntiHackLog->Output("[ANTI-HACK] (%s)(%s)(%s) Dexterity check error: %d/%d", lpObj.AccountID, lpObj.Name, lpObj.m_PlayerData->Ip_addr,
				Agility, aRecv->Agi);

			if (g_ConfigRead.antihack.EnableHackDetectMessage == TRUE)
			{
				MsgOutput(aIndex, (char *)g_ConfigRead.antihack.HackDetectMessage.c_str());
			}

			if (g_ConfigRead.EnableAutoBanUserAccount == TRUE)
			{
				GDReqBanUser(aIndex, 1, 1);
			}

			//this->GCSendDisableReconnect(aIndex);
			IOCP.CloseClient(aIndex);
		}

		else
		{
			lpObj.AgiCheckTime = GetTickCount();
		}
	}
	else
	{
		sLog->outError( "[ANTI-HACK] (%s)(%s)(%s) Cheat detected: %s", lpObj.AccountID, lpObj.Name, lpObj.m_PlayerData->Ip_addr,
			aRecv->Agi == 0 ? "SpeedHack" : "Proxy");

		AntiHackLog->Output("[ANTI-HACK] (%s)(%s)(%s) Cheat detected: %s", lpObj.AccountID, lpObj.Name, lpObj.m_PlayerData->Ip_addr,
			aRecv->Agi == 0 ? "SpeedHack" : "Proxy");

		if (g_ConfigRead.antihack.EnableHackDetectMessage == TRUE)
		{
			MsgOutput(aIndex, (char *)g_ConfigRead.antihack.HackDetectMessage.c_str());
		}

		if (g_ConfigRead.EnableAutoBanUserAccount == TRUE)
		{
			GDReqBanUser(aIndex, 1, 1);
		}

		//this->GCSendDisableReconnect(aIndex);
		IOCP.CloseClient(aIndex);

	}
}

void GameProtocol::CGReqFileCrcRecv(CGameObject &Obj, PMSG_REQ_CRCDATA *aRecv)
{
	if (!ObjectMaxRange(aIndex))
	{
		return;
	}

	if (!gObjIsConnectedGP(aIndex))
	{
		return;
	}

	

	if (g_ConfigRead.MainCRC == 0)
	{
		lpObj.CrcCheckTime = 0;
		return;
	}

	if (aRecv->MainExe == g_ConfigRead.MainCRC &&
		aRecv->IGCDLL == g_ConfigRead.DLLCRC &&
		aRecv->PlayerBMD == g_ConfigRead.PlayerCRC &&
		aRecv->SkillCRC == g_ConfigRead.SkillCRC
		)
	{
		lpObj.CrcCheckTime = 0;
	}

	else
	{
		sLog->outBasic("[ANTI-HACK] (%s)(%s)(%s) Invalid Game-Client files (Recv/Valid) (Main:%X/%X) (IGC:%X/%X) (Player:%X/%X) (SkillCRC: %X/%X)", lpObj.AccountID, lpObj.Name, lpObj.m_PlayerData->Ip_addr, aRecv->MainExe, g_ConfigRead.MainCRC,
			aRecv->IGCDLL, g_ConfigRead.DLLCRC, aRecv->PlayerBMD, g_ConfigRead.PlayerCRC, aRecv->SkillCRC, g_ConfigRead.SkillCRC);

		AntiHackLog->Output("[ANTI-HACK] (%s)(%s)(%s) Invalid Game-Client files (Recv/Valid) (Main:%X/%X) (IGC:%X/%X) (Player:%X/%X) (SkillCRC: %X/%X)", lpObj.AccountID, lpObj.Name, lpObj.m_PlayerData->Ip_addr, aRecv->MainExe, g_ConfigRead.MainCRC,
			aRecv->IGCDLL, g_ConfigRead.DLLCRC, aRecv->PlayerBMD, g_ConfigRead.PlayerCRC, aRecv->SkillCRC, g_ConfigRead.SkillCRC);

		if (g_ConfigRead.antihack.EnableHackDetectMessage == TRUE)
		{
			MsgOutput(aIndex, (char *)g_ConfigRead.antihack.HackDetectMessage.c_str());
		}

		if (g_ConfigRead.EnableAutoBanUserAccount == TRUE)
		{
			GDReqBanUser(aIndex, 1, 1);
		}

		//this->GCSendDisableReconnect(aIndex);
		IOCP.CloseClient(aIndex);

	}
}

void GameProtocol::GCElementalDamageSend(CGameObject &Obj, int TargetIndex, int Damage, int Elemental)
{
	PMSG_ELEMENTALDAMAGE pResult;
	PHeadSetB((BYTE*)&pResult, 0xD8, sizeof(pResult));

	pResult.NumberH = SET_NUMBERH(TargetIndex);
	pResult.NumberL = SET_NUMBERL(TargetIndex);
	pResult.Damage = Damage;
	pResult.btElement = Elemental;
	pResult.New = 0; ///NEW
	pResult.New1 = getGameObject(TargetIndex]->Life; ///[k2)
	pResult.New2 = 512; ///NEW
	pResult.btTargetH = SET_NUMBERH(aIndex);
	pResult.btTargetL = SET_NUMBERL(aIndex);

	if (getGameObject(TargetIndex)->Type == OBJ_USER)
	{
		IOCP.DataSend(TargetIndex, (BYTE*)&pResult, pResult.h.size);
	}

	if (cManager.WatchTargetIndex == TargetIndex || cManager.WatchTargetIndex == aIndex)
	{
		cManager.DataSend((BYTE*)&pResult, pResult.h.size);
	}

	if (lpObj.Type == OBJ_USER)
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
	}
}

void GameProtocol::CGReqEnterAcheron(CGameObject &Obj)
{
	if (!ObjectMaxRange(aIndex))
	{
		return;
	}

	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	int iTargetNumber = lpObj.TargetNumber;

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
		if (lpObj.pInventory[i].m_Type == ITEMGET(13, 146))
		{
			pos = i;
			break;
		}
	}

	if (pos == 0xFF)
	{
		PMSG_RESULT pMsg;
		PHeadSubSetB((BYTE*)&pMsg, 0xF8, 0x21, sizeof(pMsg));

		pMsg.result = 0x01;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	gObjMoveGate(aIndex, 417);
	g_Log.AddC(TColor::Blue, "[%s][%s] Entered to Acheron Map (Spirit Map serial: %I64d)", lpObj.AccountID, lpObj.Name,
		lpObj.pInventory[pos].m_Number);

	gObjInventoryDeleteItem(aIndex, pos);
	this->GCInventoryItemDeleteSend(aIndex, pos, 1);
}

void GameProtocol::GCObjectLifeInfo(CGameObject &Obj, int sObjNum, int iMaxLife, int iCurLife)
{
	PMSG_OBJECT_LIFE_INFO pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xEC, 0x10, sizeof(pMsg));

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

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, sizeof(pMsg));
}

void GameProtocol::GCChaosMachinePriceSend(CGameObject &Obj)
{
	if (lpObj.Type != OBJ_USER)
	{
		sLog->outError( "[ERROR] Index %d not USER", aIndex);
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
		if (lpObj.pChaosBox[i].IsItem())
		{
			lpObj.pChaosBox[i].Value();
			pList.BuyPrice = lpObj.pChaosBox[i].m_BuyMoney;
			pList.SellPrice = lpObj.pChaosBox[i].m_SellMoney;
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
	IOCP.DataSend(lpObj.m_Index, SendData, size);

}
void GameProtocol::GCPriceSend(CGameObject &Obj, BYTE type, SHOP_DATA *lpShopData)
{
	if (lpObj.Type != OBJ_USER)
	{
		sLog->outError( "[ERROR] Index %d not USER", aIndex);
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
			if (lpShopData->m_ShopData.m_item[i].IsItem())
			{
				lpShopData->m_ShopData.m_item[i].Value();
				pList.BuyPrice = lpShopData->m_ShopData.m_item[i].m_BuyMoney;
				pList.SellPrice = lpShopData->m_ShopData.m_item[i].m_SellMoney;
				pList.Pos = i;
				memcpy(&SendData[lOfs], &pList, sizeof(pList));
				lOfs += sizeof(pList);
			}
		}

		IOCP.DataSend(lpObj.m_Index, SendData, size);
	}

	if (type == 1) // inventory
	{
		pMsg.count = 0;
		pMsg.Type = 1;

		for (int i = 0; i < INVENTORY_SIZE; ++i)
		{
			if (lpObj.pInventory[i].IsItem())
			{
				lpObj.pInventory[i].Value();
				pList.BuyPrice = lpObj.pInventory[i].m_BuyMoney;
				pList.SellPrice = lpObj.pInventory[i].m_SellMoney;
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
		IOCP.DataSend(lpObj.m_Index, SendData, size);
	}
}

int GameProtocol::OnCGInviteDuel(PMSG_REQ_DUEL_INVITE* lpMsg, CGameObject &Obj)
{
	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		return ENEWPVP::E_NOT_DUEL_CSSERVER;
	}

	int nResponsor = MAKEWORD(lpMsg->NumberL, lpMsg->NumberH);

	CGameObjectECTSTRUCT lpRequester = (CGameObjectECTSTRUCT)&getGameObject(aIndex);

	if (!PacketCheckTime(aIndex)) return ENEWPVP::E_LIMIT_PACKETTIME;
	if (nResponsor == aIndex)			return ENEWPVP::E_INVITE_MYSELF;
	if (g_MaxStatsInfo.GetClass.IsNonPvP[lpObj.MapNumber])
	{
		GCServerMsgStringSend(Lang.GetText(0, 97), aIndex, 1);
		return ENEWPVP::E_NON_PKSERVER;
	}
	if (lpObj.CloseType != -1)	return ENEWPVP::E_CONNECTION_CLOSING;
	if (g_NewPVP.CheckPKPenalty(lpRequester))
	{
		GCServerMsgStringSend(Lang.GetText(0, 98), aIndex, 1);
		return ENEWPVP::E_NOT_DUEL_MURDERER;
	}
	if (!gObjIsConnectedEx(nResponsor))
	{
		sLog->outBasic("error: %s %d (%d)", __FILE__, __LINE__, nResponsor);
		return ENEWPVP::E_DISCONNECT;
	}
	CGameObjectECTSTRUCT lpResponsor = (CGameObjectECTSTRUCT)&getGameObject(nResponsor);
	if (g_NewPVP.CheckPKPenalty(lpResponsor))
	{
		GCServerMsgStringSend(Lang.GetText(0, 99), aIndex, 1);
		return ENEWPVP::E_NOT_DUEL_MURDERER;
	}

	char szDuelName[MAX_ACCOUNT_LEN + 1] = { 0 };
	char szDuelName2[MAX_ACCOUNT_LEN + 1] = { 0 };

	memcpy(szDuelName, lpResponsor->Name, MAX_ACCOUNT_LEN);
	szDuelName[MAX_ACCOUNT_LEN] = '\0';

	memcpy(szDuelName2, lpMsg->szName, MAX_ACCOUNT_LEN);
	szDuelName2[MAX_ACCOUNT_LEN] = '\0';

	if (strcmp(szDuelName, szDuelName2) != 0)
	{
		GCServerMsgStringSend(Lang.GetText(0, 85), aIndex, 1);
		return ENEWPVP::E_FAILED_RESULT;
	}
	if (!IsDuelEnable(lpResponsor->m_Index))
	{
		GCServerMsgStringSend(Lang.GetText(0, 86), aIndex, 1);
		return ENEWPVP::E_FAILED_RESULT;
	}
	if (g_NewPVP.IsGuildWar(lpRequester))
	{
		GCServerMsgStringSend(Lang.GetText(0, 87), aIndex, 1);
		return ENEWPVP::E_GUILDWAR;
	}
	if (g_NewPVP.IsGuildWar(lpResponsor))
	{
		GCServerMsgStringSend(Lang.GetText(0, 88), aIndex, 1);
		return ENEWPVP::E_GUILDWAR;
	}
	if (g_NewPVP.IsSelfDefense(lpRequester) || g_NewPVP.IsSelfDefense(lpResponsor))
	{
		GCServerMsgStringSend(Lang.GetText(0, 108), aIndex, 1);
		return ENEWPVP::E_SELFDEFENSE;
	}
	if (CC_MAP_RANGE(lpRequester->MapNumber) || CC_MAP_RANGE(lpResponsor->MapNumber))
	{
		GCServerMsgStringSend(Lang.GetText(0, 125), aIndex, 1);
		return ENEWPVP::E_INVALID_MAP;
	}
	if (DS_MAP_RANGE(lpRequester->MapNumber) || DS_MAP_RANGE(lpResponsor->MapNumber))
	{
		GCServerMsgStringSend(Lang.GetText(0, 100), aIndex, 1);
		return ENEWPVP::E_INVALID_MAP;
	}
	if (BC_MAP_RANGE(lpRequester->MapNumber) || BC_MAP_RANGE(lpResponsor->MapNumber))
	{
		GCServerMsgStringSend(Lang.GetText(0, 89), aIndex, 1);
		return ENEWPVP::E_INVALID_MAP;
	}
	if (IT_MAP_RANGE(lpRequester->MapNumber) || IT_MAP_RANGE(lpResponsor->MapNumber))
	{
		GCServerMsgStringSend(Lang.GetText(0, 300), aIndex, 1);
		return ENEWPVP::E_INVALID_MAP;
	}
	if (DG_MAP_RANGE(lpRequester->MapNumber) || DG_MAP_RANGE(lpResponsor->MapNumber))
	{
		GCServerMsgStringSend(Lang.GetText(0, 301), aIndex, 1);
		return ENEWPVP::E_INVALID_MAP;
	}
	if (IMPERIAL_MAP_RANGE(lpRequester->MapNumber) || IMPERIAL_MAP_RANGE(lpResponsor->MapNumber))
	{
		GCServerMsgStringSend(Lang.GetText(0, 302), aIndex, 1);
		return ENEWPVP::E_INVALID_MAP;
	}
	if (lpRequester->m_IfState.use > 0)
	{
		GCServerMsgStringSend(Lang.GetText(0, 90), aIndex, 1);
		return ENEWPVP::E_FAILED_RESULT;
	}
	if (lpResponsor->m_IfState.use > 0)
	{
		GCServerMsgStringSendEx(aIndex, 1, Lang.GetText(0, 91), lpResponsor->Name);
		return ENEWPVP::E_FAILED_RESULT;
	}
	int nRet = g_NewPVP.Reserve(*lpRequester, *lpResponsor);
	if (nRet != ENEWPVP::E_NO_ERROR)
	{
		switch (nRet)
		{
		case ENEWPVP::E_ALREADY_DUELRESERVED:
			GCServerMsgStringSendEx(aIndex, 1, Lang.GetText(0, 93), lpResponsor->Name);
			GCServerMsgStringSend(Lang.GetText(0, 84), aIndex, 1);
			break;
		case ENEWPVP::E_ALREADY_DUELLING:
			GCServerMsgStringSend(Lang.GetText(0, 83), aIndex, 1);
			GCServerMsgStringSend(Lang.GetText(0, 84), aIndex, 1);
			break;
		case ENEWPVP::E_ALREADY_DUELREQUESTED_1:
			GCServerMsgStringSendEx(aIndex, 1, Lang.GetText(0, 92), lpResponsor->Name);
			break;
		case ENEWPVP::E_ALREADY_DUELRESERVED_1:
			GCServerMsgStringSendEx(aIndex, 1, Lang.GetText(0, 93), lpResponsor->Name);
			break;
		case ENEWPVP::E_ALREADY_DUELLING_1:
			GCServerMsgStringSendEx(aIndex, 1, Lang.GetText(0, 94), lpResponsor->Name);
			break;
		}
		return nRet;
	}

	GCServerMsgStringSendEx(aIndex, 1, Lang.GetText(0, 95), lpResponsor->Name);
	sLog->outBasic("[NewPVP] [%s][%s] Requested to Start Duel to [%s][%s]", lpRequester->AccountID, lpRequester->Name, lpResponsor->AccountID, lpResponsor->Name);
	return ENEWPVP::E_NO_ERROR;
}

int GameProtocol::OnCGAnswerDuel(PMSG_ANS_DUEL_ANSWER* lpMsg, CGameObject &Obj)
{
	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		return ENEWPVP::E_NOT_DUEL_CSSERVER;
	}

	CGameObjectECTSTRUCT lpRequester = NULL;
	CGameObjectECTSTRUCT lpResponsor = (CGameObjectECTSTRUCT)&getGameObject(aIndex);
	int nRequester = MAKEWORD(lpMsg->NumberL, lpMsg->NumberH);

	if (!gObjIsConnectedEx(nRequester))	return ENEWPVP::E_DISCONNECT;
	lpRequester = (CGameObjectECTSTRUCT)&getGameObject(nRequester);

	PMSG_ANS_DUEL_INVITE res = { 0 };
	res.h.c = 0xC1;
	res.h.size = sizeof(PMSG_ANS_DUEL_INVITE);
	res.h.headcode = 0xAA;
	res.h.subcode = 0x01;
	memcpy(res.szName, lpResponsor->Name, MAX_ACCOUNT_LEN);
	if (lpResponsor->CloseType != -1)
	{
		g_NewPVP.Cancel(*lpRequester, *lpResponsor, FALSE);
		res.nResult = ENEWPVP::E_CONNECTION_CLOSING;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&res, res.h.size);
		return ENEWPVP::E_CONNECTION_CLOSING;
	}
	if (!lpMsg->bDuelOK)
	{
		g_NewPVP.Cancel(*lpRequester, *lpResponsor, FALSE);
		res.nResult = ENEWPVP::E_REFUSE_INVATE;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&res, res.h.size);
		return ENEWPVP::E_REFUSE_INVATE;
	}
	if (BC_MAP_RANGE(lpRequester->MapNumber) ||
		CC_MAP_RANGE(lpRequester->MapNumber) ||
		DS_MAP_RANGE(lpRequester->MapNumber) ||
		IT_MAP_RANGE(lpRequester->MapNumber) ||
		DG_MAP_RANGE(lpRequester->MapNumber) ||
		IMPERIAL_MAP_RANGE(lpRequester->MapNumber) ||
		lpRequester->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		g_NewPVP.Cancel(*lpRequester, *lpResponsor, FALSE);
		GCServerMsgStringSend(Lang.GetText(0, 125), lpResponsor->m_Index, 1);
		GCServerMsgStringSend(Lang.GetText(0, 125), lpRequester->m_Index, 1);
		res.nResult = ENEWPVP::E_INVALID_MAP;
		IOCP.DataSend(lpResponsor->m_Index, (BYTE*)&res, res.h.size);
		IOCP.DataSend(lpRequester->m_Index, (BYTE*)&res, res.h.size);
		return ENEWPVP::E_INVALID_MAP;
	}
	int nRet = g_NewPVP.Join(*lpRequester, *lpResponsor);
	if (nRet != ENEWPVP::E_NO_ERROR)
	{
		switch (nRet)
		{
		case ENEWPVP::E_ALREADY_DUELRESERVED_1:
			GCServerMsgStringSend(Lang.GetText(0, 96), aIndex, 1);
			break;
		case ENEWPVP::E_ALREADY_DUELLING_1:
			GCServerMsgStringSend(Lang.GetText(0, 83), aIndex, 1);
			GCServerMsgStringSend(Lang.GetText(0, 84), aIndex, 1);
			break;
		}
		res.nResult = nRet;
		IOCP.DataSend(lpRequester->m_Index, (BYTE*)&res, res.h.size);
		IOCP.DataSend(lpResponsor->m_Index, (BYTE*)&res, res.h.size);
		g_NewPVP.Cancel(*lpRequester, *lpResponsor, FALSE);
		return nRet;
	}
	return ENEWPVP::E_NO_ERROR;
}

int GameProtocol::OnCGLeaveDuel(PMSG_REQ_DUEL_EXIT* lpMsg, CGameObject &Obj)
{
	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		return ENEWPVP::E_NOT_DUEL_CSSERVER;
	}

	if (lpObj.CloseType != -1)	return ENEWPVP::E_DISCONNECT;
	int nRet = g_NewPVP.Leave(getGameObject(aIndex));
	if (nRet != ENEWPVP::E_NO_ERROR)	return nRet;
	return ENEWPVP::E_NO_ERROR;
}

int GameProtocol::OnDuelChannelJoin(PMSG_REQ_DUEL_JOINCNANNEL* lpMsg, CGameObject &Obj)
{
	if (lpObj.CloseType != -1)	return ENEWPVP::E_DISCONNECT;
	int nRet = g_NewPVP.JoinChannel(lpMsg->nChannelId, getGameObject(aIndex));
	if (nRet != ENEWPVP::E_NO_ERROR)	return nRet;
	return ENEWPVP::E_NO_ERROR;
}

int GameProtocol::OnDuelChannelLeave(PMSG_REQ_DUEL_LEAVECNANNEL* lpMsg, CGameObject &Obj)
{
	if (lpObj.CloseType != -1)	return ENEWPVP::E_DISCONNECT;
	int nRet = g_NewPVP.LeaveChannel(lpMsg->nChannelId, getGameObject(aIndex));
	if (nRet != ENEWPVP::E_NO_ERROR)	return nRet;
	return ENEWPVP::E_NO_ERROR;
}

void GameProtocol::CGReqRegGensMember(PMSG_REQ_REG_GENS_MEMBER *lpMsg, CGameObject &Obj)
{
	BYTE bRet;

	CGameObject lpObj = Obj;

	GUILD_INFO_STRUCT *lpGuildInfo = Obj.m_PlayerData->lpGuild;

	if (!ObjectMaxRange(Obj.m_Index)) return;

	if (gObjIsConnectedGP(Obj.m_Index))
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
		if (lpObj.Level < 50)
		{
			pMsg.bResult = 3; bRet = 0;
		}

		//[GensSystem] [%s][%s] Gens Joining Failed to Gens be Different Guild Leader [%s]
		if (lpObj.m_PlayerData->GuildStatus >= 0)
		{
			pMsg.bResult = 4; bRet = 0;
		}

		//[GensSystem] [%s][%s] Gens Joining Failed to Not Register Guild Leader GuildName [%s]
		if (lpObj.m_PlayerData->GuildStatus == GUILD_MASTER)
		{
			pMsg.bResult = 5; bRet = 0;
		}

		//[GensSystem] [%s][%s] Gens Joining Failed to Already Partymember
		if (lpObj.PartyNumber >= 0)
		{
			pMsg.bResult = 6; bRet = 0;
		}

		//[GensSystem] [%s][%s] Gens Joining Failed to Union GuildMaster
		if (lpGuildInfo != NULL && lpGuildInfo->iGuildUnion)
		{
			pMsg.bResult = 7; bRet = 0;
		}

		if (lpObj.m_PlayerData->m_bUsePartyMatching)
		{
			pMsg.bResult = 8; bRet = 0;
		}

		if (bRet)
		{
			g_GensSystem.ReqRegGensMember(lpObj, lpMsg->bResult);
		}
		else
		{
			pMsg.bIndexH = SET_NUMBERH(Obj.m_Index);
			pMsg.bIndexL = SET_NUMBERL(Obj.m_Index);
			DGAnsRegGensMember((PMSG_ANS_REG_GENS_MEMBER_EXDB*)&pMsg);
		}
	}
}

//-> Completed
void GameProtocol::CGReqSecedeGensMember(PMSG_REQ_SEGEDE_GENS_MEMBER *lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
		return;

	CGameObject lpObj = Obj;

	BYTE bRet = 1;

	if (gObjIsConnectedGP(Obj.m_Index))
	{
		PMSG_ANS_SECEDE_GENS_MEMBER_EXDB pMsg;

		if (!g_GensSystem.IsInfluenceNPC(lpObj))
		{
			pMsg.bResult = 3; bRet = 0;
		}

		else if (lpObj.m_PlayerData->GuildStatus == GUILD_MASTER)
		{
			pMsg.bResult = 2; bRet = 0;
		}

		//[GensSystem] [%s][%s] Gens Leaving Failed to Not Register Gens
		else if (!g_GensSystem.GetGensInfluence(lpObj))
		{
			pMsg.bResult = 1; bRet = 0;
		}

		else if (lpObj.m_PlayerData->m_bUsePartyMatching)
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
			pMsg.bIndexH = SET_NUMBERH(Obj.m_Index);
			pMsg.bIndexL = SET_NUMBERL(Obj.m_Index);
			DGAnsSecedeGensMember((PMSG_ANS_SECEDE_GENS_MEMBER_EXDB*)&pMsg);
		}
	}
}

//-> Completed
void GameProtocol::CGReqGensReward(PMSG_GENS_REWARD_CODE *lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
		return;

	BYTE bRet = 0; //Not Used

	CGameObject lpObj = Obj;

	if (gObjIsConnectedGP(Obj.m_Index))
	{
		g_GensSystem.ReqExDBGensRewardCheck(lpObj, lpMsg->bReward);
	}
}

//-> Completed
void GameProtocol::CGReqGensMemberInfo(PMSG_REQ_GENS_INFO *lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index)) return;

	CGameObject lpObj = Obj;

	if (gObjIsConnectedGP(Obj.m_Index))
	{
		if (g_GensSystem.IsRegGensInfluence(lpObj))
		{
			g_GensSystem.ReqExDBGensInfo(lpObj);
		}
	}
}
void GameProtocol::CGInventoryEquipment(PMSG_REQ_INVENTORY_EQUIPMENT_ITEM *lpMsg, CGameObject &Obj)
{
	if (ObjectMaxRange(Obj.m_Index) == FALSE)
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (!gObjIsConnected(Obj.m_Index))
	{
		return;
	}

	CGameObject lpObj = Obj;

	PMSG_ANS_INVENTORY_EQUIPMENT_ITEM pMsg = { 0 };

	pMsg.btResult = 0;
	PHeadSubSetB((BYTE*)&pMsg, 0xBF, 0x20, sizeof(pMsg));

	pMsg.btItemPos = lpMsg->btItemPos;

	if (!IsInvenPet(lpObj.pInventory[lpMsg->btItemPos].m_Type) &&
		lpObj.pInventory[lpMsg->btItemPos].m_Durability == 0.0)
	{
		pMsg.btResult = -1;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	switch (lpObj.pInventory[lpMsg->btItemPos].m_Type)
	{
	case ITEMGET(13, 128):
	case ITEMGET(13, 129):
		if (lpObj.m_PlayerData->m_btSculptPos == 0)
		{
			if (lpMsg->btValue == (BYTE)-2 && lpObj.pInventory[lpMsg->btItemPos].m_Durability == 255.0)
			{
				lpObj.m_PlayerData->m_btSculptPos = lpMsg->btItemPos;
				lpObj.pInventory[lpMsg->btItemPos].m_Durability = 254.0;
				pMsg.btResult = -2;
			}

			goto DEF_SWITCH;
		}

		if (lpObj.m_PlayerData->m_btSculptPos < INVETORY_WEAR_SIZE || lpObj.m_PlayerData->m_btSculptPos >= MAIN_INVENTORY_SIZE)
		{
			goto DEF_SWITCH;
		}

		if (lpMsg->btValue == (BYTE)-1 && lpObj.m_PlayerData->m_btSculptPos == lpMsg->btItemPos)
		{
			if (lpObj.pInventory[lpMsg->btItemPos].m_Durability == 254.0)
			{
				lpObj.pInventory[lpMsg->btItemPos].m_Durability = 255.0;
				pMsg.btResult = -1;
			}

			goto DEF_SWITCH;
		}

		this->GCServerMsgStringSend(Lang.GetText(0, 548), iIndex, 1);
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		break;
	case ITEMGET(13, 130):
	case ITEMGET(13, 131):
	case ITEMGET(13, 132):
	case ITEMGET(13, 133):
		if (lpObj.m_PlayerData->m_btCharmPos == 0)
		{
			if (lpMsg->btValue == (BYTE)-2 && lpObj.pInventory[lpMsg->btItemPos].m_Durability == 255.0)
			{
				lpObj.m_PlayerData->m_btCharmPos = lpMsg->btItemPos;
				lpObj.pInventory[lpMsg->btItemPos].m_Durability = 254.0;
				pMsg.btResult = -2;
			}

			goto DEF_SWITCH;
		}

		if (lpObj.m_PlayerData->m_btCharmPos < INVETORY_WEAR_SIZE || lpObj.m_PlayerData->m_btCharmPos >= MAIN_INVENTORY_SIZE)
		{
			goto DEF_SWITCH;
		}

		if (lpMsg->btValue == (BYTE)-1 && lpObj.m_PlayerData->m_btCharmPos == lpMsg->btItemPos)
		{
			if (lpObj.pInventory[lpMsg->btItemPos].m_Durability == 254.0)
			{
				lpObj.pInventory[lpMsg->btItemPos].m_Durability = 255.0;
				pMsg.btResult = -1;
			}

			goto DEF_SWITCH;
		}

		this->GCServerMsgStringSend(Lang.GetText(0, 548), iIndex, 1);
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		break;
	case ITEMGET(13, 134):
		if (lpObj.m_PlayerData->m_btArtifactPos == 0)
		{
			if (lpMsg->btValue == (BYTE)-2 && lpObj.pInventory[lpMsg->btItemPos].m_Durability == 255.0)
			{
				lpObj.m_PlayerData->m_btArtifactPos = lpMsg->btItemPos;
				lpObj.pInventory[lpMsg->btItemPos].m_Durability = 254.0;
				pMsg.btResult = -2;
			}

			goto DEF_SWITCH;
		}

		if (lpObj.m_PlayerData->m_btArtifactPos < INVETORY_WEAR_SIZE || lpObj.m_PlayerData->m_btArtifactPos >= MAIN_INVENTORY_SIZE)
		{
			goto DEF_SWITCH;
		}

		if (lpMsg->btValue == (BYTE)-1 && lpObj.m_PlayerData->m_btArtifactPos == lpMsg->btItemPos)
		{
			if (lpObj.pInventory[lpMsg->btItemPos].m_Durability == 254.0)
			{
				lpObj.pInventory[lpMsg->btItemPos].m_Durability = 255.0;
				pMsg.btResult = -1;
			}

			goto DEF_SWITCH;
		}

		this->GCServerMsgStringSend(Lang.GetText(0, 548), iIndex, 1);
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		break;
	case ITEMGET(13, 177):
	case ITEMGET(13, 178):
	case ITEMGET(13, 179):
		if (lpObj.m_PlayerData->m_btExpUpCharmPos == 0)
		{
			if (lpMsg->btValue == (BYTE)-2 && lpObj.pInventory[lpMsg->btItemPos].m_Durability == 255.0)
			{
				lpObj.m_PlayerData->m_btExpUpCharmPos = lpMsg->btItemPos;
				lpObj.pInventory[lpMsg->btItemPos].m_Durability = 254.0;
				pMsg.btResult = -2;
			}

			goto DEF_SWITCH;
		}

		if (lpObj.m_PlayerData->m_btExpUpCharmPos < INVETORY_WEAR_SIZE || lpObj.m_PlayerData->m_btExpUpCharmPos >= MAIN_INVENTORY_SIZE)
		{
			goto DEF_SWITCH;
		}

		if (lpMsg->btValue == (BYTE)-1 && lpObj.m_PlayerData->m_btExpUpCharmPos == lpMsg->btItemPos)
		{
			if (lpObj.pInventory[lpMsg->btItemPos].m_Durability == 254.0)
			{
				lpObj.pInventory[lpMsg->btItemPos].m_Durability = 255.0;
				pMsg.btResult = -1;
			}

			goto DEF_SWITCH;
		}

		this->GCServerMsgStringSend(Lang.GetText(0, 548), iIndex, 1);
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		break;
	case ITEMGET(13, 2):
	case ITEMGET(13, 3):
	case ITEMGET(13, 4):
	case ITEMGET(13, 5):
	case ITEMGET(13, 37):
		if (lpObj.m_btInvenPetPos != 0)
		{
			if (lpObj.m_btInvenPetPos >= INVETORY_WEAR_SIZE && lpObj.m_btInvenPetPos < MAIN_INVENTORY_SIZE)
			{
				if (lpMsg->btValue != (BYTE)-1 || lpObj.m_btInvenPetPos != lpMsg->btItemPos)
				{
					this->GCServerMsgStringSend(Lang.GetText(0, 615), iIndex, 1);
					return;
				}

				if (gObjUseInvenPetCheck(lpObj, &lpObj.pInventory[lpMsg->btItemPos], 0) == FALSE)
				{
					this->GCServerMsgStringSend(Lang.GetText(0, 616), iIndex, 1);
					return;
				}

				if (lpObj.pInventory[lpMsg->btItemPos].m_JewelOfHarmonyOption == 1)
				{

					lpObj.pInventory[lpMsg->btItemPos].m_JewelOfHarmonyOption = 0;
					lpObj.m_wInvenPet = -1;
					pMsg.btResult = -1;
				}
			}
		}

		else if (lpMsg->btValue == (BYTE)-2)
		{

			if (lpObj.m_wInvenPet != (WORD)-1 || lpObj.pInventory[lpMsg->btItemPos].m_JewelOfHarmonyOption == 1)
			{

				this->GCServerMsgStringSend(Lang.GetText(0, 615), iIndex, 1);
				return;
			}

			if (gObjUseInvenPetCheck(lpObj, &lpObj.pInventory[lpMsg->btItemPos], 1) == FALSE)
			{
				this->GCServerMsgStringSend(Lang.GetText(0, 617), iIndex, 1);
				return;
			}

			lpObj.m_btInvenPetPos = lpMsg->btItemPos;
			lpObj.pInventory[lpMsg->btItemPos].m_JewelOfHarmonyOption = 1;
			pMsg.btResult = (BYTE)-2;

		}

		gObjMakePreviewCharSet(Obj.m_Index);
		this->GCEquipmentChange(iIndex, lpMsg->btItemPos);
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		break;
	default:
	DEF_SWITCH:
		gObjCalCharacter.CalcCharacter(Obj.m_Index);
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		break;
	}
}

void GameProtocol::CGReqUseTransformationRing(PMSG_REQ_USETRANSFORMATIONRING * lpMsg, CGameObject &Obj)
{
	if (ObjectMaxRange(aIndex) == false)
	{
		return;
	}

	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	if (lpMsg->btState == 1)
	{
		if (IsTransformCharacterSkin(lpObj.m_Change) == TRUE)
		{
			lpObj.m_PlayerData->m_EnableUseChangeSkin = 0;
		}
	}

	else
	{
		lpObj.m_PlayerData->m_EnableUseChangeSkin = 1;
	}

	PMSG_MOVE pMove;

	pMove.h.c = 0xC1;
	pMove.h.headcode = 0xD7;
	pMove.h.size = sizeof(pMove);
	pMove.X = lpObj.X;
	pMove.Y = lpObj.Y;

	PMoveProc(&pMove, aIndex);
	gObjViewportListProtocolCreate(&getGameObject(aIndex));
}

void GameProtocol::GCSendAttackSpeed(CGameObject &Obj)
{
	PMSG_ATTACKSPEEDSEND pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0xEC, 0x30, sizeof(pMsg));

	pMsg.AttackSpeed = lpObj.m_AttackSpeed;

	pMsg.MagicSpeed = lpObj.m_MagicSpeed;

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::CGReqArcaBattleGuildMasterJoin(PMSG_REQ_ARCA_BATTLE_JOIN *lpMsg, CGameObject &Obj)
{
	if (ObjectMaxRange(Obj.m_Index) == false)
	{
		return;
	}

	CGameObject lpObj = Obj;

	if (gObjIsConnected(Obj.m_Index) == false)
	{
		return;
	}

	GUILD_INFO_STRUCT * lpGuildInfo = lpObj.m_PlayerData->lpGuild;
	PMSG_ANS_ARCA_BATTLE_GUILD_JOIN_DS pMsg;

	pMsg.wNumber = iIndex;

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

	if (lpObj.m_PlayerData->GuildStatus != GUILD_MASTER)
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

	g_ArcaBattle.GDReqArcaBattleIsTopRank(lpObj.m_Index, lpObj.m_PlayerData->GuildNumber);
}

void GameProtocol::CGReqArcaBattleGuildMemberJoin(PMSG_REQ_ARCA_BATTLE_JOIN *lpMsg, CGameObject &Obj)
{
	PMSG_ANS_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS pMsg;

	if (ObjectMaxRange(Obj.m_Index) == false)
	{
		return;
	}

	CGameObject lpObj = Obj;

	if (gObjIsConnected(Obj.m_Index) == false)
	{
		return;
	}

	pMsg.wNumber = iIndex;

	if (lpObj.m_PlayerData->lpGuild == NULL)
	{
		pMsg.btResult = 7;
		DGAnsArcaBattleGuildMemberJoin((PMSG_ANS_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS *)&pMsg);
		return;
	}

	if (lpObj.m_PlayerData->GuildStatus == GUILD_MASTER)
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

void GameProtocol::CGReqArcaBattleEnter(PMSG_REQ_ARCA_BATTLE_ENTER *lpMsg, CGameObject &Obj)
{
	PMSG_ANS_ARCA_BATTLE_ENTER_DS pMsg;

	if (ObjectMaxRange(Obj.m_Index) == false)
	{
		return;
	}

	CGameObject lpObj = Obj;

	if (gObjIsConnected(Obj.m_Index))
	{
		int iArcaBattleState = g_ArcaBattle.GetState();

		if (iArcaBattleState == 6 || iArcaBattleState == 7)
		{
			if (lpObj.m_PlayerData->m_bUsePartyMatching)
			{
				PMSG_ANS_PARTYMATCHING_ERROR pMsg;
				PHeadSubSetB((BYTE*)&pMsg, 0xEF, 0x09, sizeof(pMsg));
				pMsg.iResult = -1;

				IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
				return;
			}

			if (lpObj.Teleport)
			{
				return;
			}

			BYTE btAttr = MapC[lpObj.MapNumber].GetAttr(lpObj.X, lpObj.Y);

			if (btAttr & 1)
				GDReqArcaBattleEnter(lpObj, lpMsg->btEnterSeq);
		}

		else
		{
			pMsg.wNumber = iIndex;
			pMsg.btResult = 12;
			DGAnsArcaBattleEnter((PMSG_ANS_ARCA_BATTLE_ENTER_DS *)&pMsg);
		}
	}
}

void GameProtocol::CGReqAcheronEnter(PMSG_REQ_ACHERON_ENTER *lpMsg, CGameObject &Obj)
{
	PMSG_ANS_ACHERON_ENTER pMsg;

	if (ObjectMaxRange(Obj.m_Index) == false)
	{
		return;
	}

	CGameObject lpObj = Obj;

	if (gObjIsConnected(Obj.m_Index) == false)
	{
		return;
	}

	for (int pos = INVETORY_WEAR_SIZE; pos < MAIN_INVENTORY_SIZE; pos++)
	{
		if (lpObj.pInventory[pos].IsItem() && lpObj.pInventory[pos].m_Type == ITEMGET(13, 146))
		{
			gObjInventoryDeleteItem(iIndex, pos);
			this->GCInventoryItemDeleteSend(iIndex, pos, 1);
			gObjMoveGate(iIndex, 417);
			return;
		}
	}

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xF8;
	pMsg.h.subcode = 0x21;
	pMsg.btResult = 1;
	IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (BYTE*)&pMsg, sizeof(pMsg));
}

void GameProtocol::CGReqArcaBattleBootyExchange(CGameObject &Obj)
{
	if (ObjectMaxRange(Obj.m_Index) == false)
	{
		return;
	}
	CGameObject lpObj = Obj;

	if (gObjIsConnected(Obj.m_Index) == false)
	{
		return;
	}

	int bCanChaosBox = 1; // stub.
	if (bCanChaosBox == TRUE && !lpObj.m_IfState.use)
	{
		if (lpObj.m_bPShopOpen == TRUE)
		{
			sLog->outBasic("[%s][%s] is Already Opening PShop, ChaosBox Failed", lpObj.AccountID, lpObj.Name);
			this->GCServerMsgStringSend(Lang.GetText(0, 531), iIndex, 1);
			return;
		}

		lpObj.m_IfState.use = 1;
		lpObj.m_IfState.type = 7;
		gObjInventoryTrans(lpObj.m_Index);
		sLog->outBasic("[ArcaBattle][BootyItemMix] [%s][%s]  Booty Item Mix Start", lpObj.AccountID, lpObj.Name);
		gObjItemTextSave(lpObj);
		gObjWarehouseTextSave(lpObj);
	}
}

void GameProtocol::CGReqSpritemapExchange(CGameObject &Obj)
{
	if (ObjectMaxRange(Obj.m_Index) == false)
	{
		return;
	}

	CGameObject lpObj = Obj;

	if (gObjIsConnected(Obj.m_Index))
	{
		int bCanChaosBox = 1; // stub;
		if (bCanChaosBox == 1 && !(lpObj.m_IfState.type & 3))
		{
			if (lpObj.m_bPShopOpen == 1)
			{
				sLog->outBasic("[%s][%s] is Already Opening PShop, ChaosBox Failed", lpObj.AccountID, lpObj.Name);
				this->GCServerMsgStringSend(Lang.GetText(0, 531), iIndex, 1);
			}

			else
			{
				lpObj.m_IfState.use = 1;
				lpObj.m_IfState.type = 7;

				gObjInventoryTrans(lpObj.m_Index);
				sLog->outBasic("[ArcaBattle][BootyItemMix] [%s][%s]  Booty Item Mix Start", lpObj.AccountID, lpObj.Name);
				gObjItemTextSave((CGameObject &)lpObj);
				gObjWarehouseTextSave((CGameObject &)lpObj);
			}
		}
	}
}

void GameProtocol::CGReqRegisteredMemberCnt(CGameObject &Obj)
{
	PMSG_REQ_AB_REG_MEMBER_CNT_DS pMsg;
	PMSG_ANS_AB_REG_MEMBER_CNT pAnsMsg;

	if (ObjectMaxRange(aIndex))
	{
		

		if (gObjIsConnected(aIndex))
		{
			pAnsMsg.h.c = 0xC1;
			pAnsMsg.h.size = sizeof(pMsg);
			pAnsMsg.h.headcode = 0xF8;
			pAnsMsg.h.subcode = 0x42;
			pAnsMsg.btRegMemCnt = 0;

			if (lpObj.m_PlayerData->lpGuild)
			{
				pMsg.h.c = 0xC1;
				pMsg.h.size = sizeof(pMsg);
				pMsg.h.headcode = 0xF8;
				pMsg.h.subcode = 0x45;
				pMsg.iIndex = aIndex;
				pMsg.iGuildNumber = lpObj.m_PlayerData->GuildNumber;
				wsDataCli.DataSend((char*)&pMsg, sizeof(pMsg));
			}

			else
			{
				pAnsMsg.btResult = 1;
				IOCP.DataSend(lpObj.m_Index, (BYTE*)&pAnsMsg, sizeof(pMsg));
			}
		}
	}
}

void GameProtocol::CGReqEnterDoppelGanger(PMSG_REQ_ENTER_DOPPELGANGER *lpMsg, CGameObject &Obj)
{
	if (ObjectMaxRange(Obj.m_Index) == false)
	{
		sLog->outBasic("return %s %d", __FILE__, __LINE__);
		return;
	}

	g_DoppelGanger.EnterDoppelgangerEvent(iIndex, lpMsg->btPos);
}

void GameProtocol::CGReqEnterZone(PMSG_REQ_ENTER_ZONE * lpMsg, CGameObject &Obj)
{
	if (ObjectMaxRange(Obj.m_Index) == false)
	{
		sLog->outBasic("return %s %d", __FILE__, __LINE__);
		return;
	}

	if (lpMsg->btResult == TRUE)
	{
		g_ImperialGuardian.CGEnterPortal(iIndex, 0);
	}
}

void GameProtocol::GCSendRecvCheck(CGameObject &Obj)
{
	PMSG_RECV_TRAP pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xFA, 0x9F, sizeof(pMsg));

	pMsg.wUserIndex = aIndex;
	pMsg.CurrTime = time(NULL);

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::CGReqAntihackBreach(CGameObject &Obj, PMSG_ANTIHACK_BREACH *lpMsg)
{
	

	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	sLog->outError( "[%s][%s][%s][%s] AntiHack breach -> Code:%d",
		lpObj.AccountID, lpObj.Name, lpObj.m_PlayerData->Ip_addr, lpObj.m_PlayerData->HWID, lpMsg->dwErrorCode);

	AntiHackLog->Output("[%s][%s][%s][%s] AntiHack breach -> Code: %d",
		lpObj.AccountID, lpObj.Name, lpObj.m_PlayerData->Ip_addr, lpObj.m_PlayerData->HWID, lpMsg->dwErrorCode);

	if (g_ConfigRead.antihack.AntiHackBreachDisconnectUser == true)
	{
		//this->GCSendDisableReconnect(aIndex);
		IOCP.CloseClient(aIndex);
	}
}

void GameProtocol::CGReqAntihackCheck(CGameObject &Obj, PMSG_ANTIHACK_CHECK *lpMsg)
{
	

	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	BYTE MainRecvHeader[] = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC };

	if (memcmp(lpMsg->checkdata, MainRecvHeader, 5) != 0 && g_ConfigRead.antihack.AntiHackRecvHookProtect == true) // data is wrong, recv is probably hooked
	{
		sLog->outError( "[%s][%s][%s][%s] AntiHack breach -> Recv header data is wrong",
			lpObj.AccountID, lpObj.Name, lpObj.m_PlayerData->Ip_addr, lpObj.m_PlayerData->HWID);

		AntiHackLog->Output("[%s][%s][%s][%s] AntiHack breach -> Recv header data is wrong",
			lpObj.AccountID, lpObj.Name, lpObj.m_PlayerData->Ip_addr, lpObj.m_PlayerData->HWID);
		this->GCSendDisableReconnect(aIndex);
		//this->GCServerMsgStringSendEx(aIndex, 1, Lang.GetText(0, 547));
		gObjCloseSet(aIndex, 0);
	}

	else
	{
		lpObj.m_PlayerData->AntiHackCheckTime = GetTickCount();
	}
}

void GameProtocol::CGReqInJewelPentagramItem(PMSG_REQ_IN_PENTAGRAM_JEWEL *lpMsg, CGameObject &Obj)
{
	if (!lpMsg)
	{
		return;
	}

	if (!g_PentagramSystem.PentagramJewel_IN(aIndex, lpMsg->iPentagramPos, lpMsg->iJewelPos))
		this->GCAnsInJewelPentagramItem(aIndex, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

void GameProtocol::GCAnsInJewelPentagramItem(CGameObject &Obj, int iResult, BYTE btJewelPos, BYTE btJewelIndex, BYTE btItemType, WORD wItemIndex, BYTE btMainAttribute, BYTE btLevel, BYTE btRank1OptionNum, BYTE btRank1Level, BYTE btRank2OptionNum, BYTE btRank2Level, BYTE btRank3OptionNum, BYTE btRank3Level, BYTE btRank4OptionNum, BYTE btRank4Level, BYTE btRank5OptionNum, BYTE btRank5Level)
{
	PMSG_ANS_IN_PENTAGRAM_JEWEL pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xEC, 0x00, sizeof(pMsg));

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

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, sizeof(pMsg));
}

void GameProtocol::CGReqOutJewelPentagramItem(PMSG_REQ_OUT_PENTAGRAM_JEWEL *lpMsg, CGameObject &Obj)
{
	if (!lpMsg)
	{
		return;
	}

	BYTE btJewelPos;
	BYTE btJewelDBIndex;

	int iResult = g_PentagramSystem.PentagramJewel_OUT(aIndex, lpMsg->iPentagramPos, lpMsg->btSocketIndex, &btJewelPos, &btJewelDBIndex);
	this->GCAnsOutJewelPentagramItem(aIndex, iResult, btJewelPos, btJewelDBIndex);

	if (iResult != 1)
	{
		this->GCAnsPentagramJewelInOut(aIndex, iResult);
	}
}

void GameProtocol::GCAnsOutJewelPentagramItem(CGameObject &Obj, int iResult, BYTE btJewelPos, BYTE btJewelDBIndex)
{
	PMSG_ANS_OUT_PENTAGRAM_JEWEL pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xEC, 0x01, sizeof(pMsg));

	pMsg.Result = iResult;
	pMsg.btJewelPos = btJewelPos;
	pMsg.btJewelDBIndex = btJewelDBIndex;

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::CGReqRefinePentagramJewel(PMSG_REQ_REFINE_PENTAGRAM_JEWEL *lpMsg, CGameObject &Obj)
{
	if (!lpMsg)
	{
		return;
	}

	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	lpObj.m_PlayerData->m_bIsPentagramMixCompleted = true;
	BYTE btRefineResult = g_PentagramMixSystem.PentagramJewelRefine(aIndex, lpMsg->btRefineKind);

	if (btRefineResult != 1)
	{
		this->GCAnsRefinePentagramJewel(aIndex, btRefineResult);
	}
}

void GameProtocol::GCAnsRefinePentagramJewel(CGameObject &Obj, BYTE btResult)
{
	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));

	pMsg.Result = btResult;
	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::CGReqUpgradePentagramJewel(PMSG_REQ_UPGRADE_PENTAGRAM_JEWEL *lpMsg, CGameObject &Obj)
{
	if (!lpMsg)
	{
		return;
	}

	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	lpObj.m_PlayerData->m_bIsPentagramMixCompleted = true;
	BYTE btUpgradeResult = g_PentagramMixSystem.PentagramJewel_Upgrade(aIndex, lpMsg->btUpgradeType, lpMsg->btTargetValue);

	if (btUpgradeResult != 1)
	{
		this->GCAnsUpgradePentagramJewel(aIndex, btUpgradeResult);
	}
}

void GameProtocol::GCAnsUpgradePentagramJewel(CGameObject &Obj, BYTE btResult)
{
	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));

	pMsg.Result = btResult;
	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCAnsPentagramJewelInOut(CGameObject &Obj, BYTE btResult)
{
	PMSG_PENTAGRAM_JEWEL_INOUT pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xEC, 0x04, sizeof(pMsg));

	pMsg.btResult = btResult;
	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::CGReqGuildMatchingList(PMSG_REQ_GUILDMATCHINGLIST *lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	GDReqGuildMatchingList(nUserIndex, lpMsg->nPage);
}

void GameProtocol::CGReqGuildMatchingListSearchWord(PMSG_REQ_GUILDMATCHINGLISTSEARCHWORD *lpMsg, int nUserIndex)
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

	CGameObject lpObj = &getGameObject(nUserIndex);

	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	int nGuildNumber = lpObj.m_PlayerData->GuildNumber;
	GUILD_INFO_STRUCT * lpstGuildInfo = Guild.SearchGuild_Number(nGuildNumber);
	_stGuildMatchingList stGuildMatchingList;

	PMSG_ANS_REGGUILDMATCHINGDATA pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xED, 0x02, sizeof(pMsg));

	if (!lpstGuildInfo || Guild.GetGuildMemberStatus(lpstGuildInfo->Name, lpObj.Name) != GUILD_MASTER)
	{
		pMsg.nResult = -4;
		IOCP.DataSend(nUserIndex, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (lpObj.m_PK_Level > 3)
	{
		pMsg.nResult = -2;
		IOCP.DataSend(nUserIndex, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	memcpy(stGuildMatchingList.szMemo, lpMsg->szMemo, 41);
	stGuildMatchingList.btInterestType = lpMsg->btInterestType;
	stGuildMatchingList.btLevelRange = lpMsg->btLevelRange;
	stGuildMatchingList.btClassType = lpMsg->btClassType;
	stGuildMatchingList.btGuildMasterClass = lpObj.Class;

	if (lpObj.m_PlayerData->ChangeUP == 1)
	{
		stGuildMatchingList.btGuildMasterClass |= 0x08;
	}

	if (lpObj.m_PlayerData->ChangeUP == 2)
	{
		stGuildMatchingList.btGuildMasterClass |= CS_SET_CHANGEUP_SX;
		stGuildMatchingList.btGuildMasterClass |= CS_SET_CHANGEUP2_SX;
		//stAllowList.btApplicantClass |= 0x10;
	}

	memcpy(stGuildMatchingList.szGuildName, lpObj.m_PlayerData->GuildName, MAX_GUILD_LEN + 1);
	memcpy(stGuildMatchingList.szRegistrant, lpObj.Name, MAX_ACCOUNT_LEN + 1);
	stGuildMatchingList.btGuildMemberCnt = lpstGuildInfo->Count;
	stGuildMatchingList.nGuildNumber = lpstGuildInfo->Number;
	stGuildMatchingList.nGuildMasterLevel = lpObj.Level + lpObj.m_PlayerData->MasterLevel;
	stGuildMatchingList.btGensType = g_GensSystem.GetGensInfluence(lpObj);

	GDReqRegGuildMatchingList(nUserIndex, stGuildMatchingList);
}

void GameProtocol::CGReqCancelGuildMatchingList(PMSG_REQ_CANCEL_GUILDMATCHINGLIST * lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	CGameObject lpObj = &getGameObject(nUserIndex);

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	int nGuildNumber = lpObj.m_PlayerData->GuildNumber;
	GUILD_INFO_STRUCT * lpstGuildInfo = Guild.SearchGuild_Number(nGuildNumber);

	PMSG_ANS_CANCEL_GUILDMATCHINGLIST pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xED, 0x03, sizeof(pMsg));

	if (!lpstGuildInfo || Guild.GetGuildMemberStatus(lpstGuildInfo->Name, lpObj.Name) != GUILD_MASTER)
	{
		pMsg.nResult = -2;
		IOCP.DataSend(nUserIndex, (BYTE*)&pMsg, pMsg.h.size);
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

	CGameObject lpObj = &getGameObject(nUserIndex);

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	int nGuildNumber = lpObj.m_PlayerData->GuildNumber;
	int nTargetGuildNumber = lpMsg->nGuildNumber;

	PMSG_ANS_JOIN_GUILDMATCHING pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xED, 0x04, sizeof(pMsg));

	if (nGuildNumber > 0)
	{
		pMsg.nResult = -3;
		IOCP.DataSend(nUserIndex, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (getGameObject(nUserIndex)->m_PK_Level > 3)
	{
		pMsg.nResult = -2;
		IOCP.DataSend(nUserIndex, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	_stGuildMatchingAllowListDB stAllowList;

	stAllowList.nGuildNumber = nTargetGuildNumber;
	memcpy(stAllowList.szApplicantName, lpObj.Name, MAX_ACCOUNT_LEN + 1);
	stAllowList.btApplicantClass = lpObj.Class;
	stAllowList.btState = 0;
	stAllowList.nApplicantLevel = lpObj.Level + lpObj.m_PlayerData->MasterLevel;

	if (lpObj.m_PlayerData->ChangeUP == 1)
	{
		stAllowList.btApplicantClass |= 0x08;
	}

	else if (lpObj.m_PlayerData->ChangeUP == 2)
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

	CGameObject lpObj = &getGameObject(nUserIndex);

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	GDReqCancelJoinGuildMatching(nUserIndex, lpObj.Name, 0);
}

void GameProtocol::CGReqAllowJoinGuildMatching(PMSG_REQ_ALLOW_JOIN_GUILDMATCHING *lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	CGameObject lpObj = &getGameObject(nUserIndex);

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	char szMemberName[MAX_ACCOUNT_LEN + 1];
	memcpy(szMemberName, lpMsg->szName, MAX_ACCOUNT_LEN + 1);

	if (gObjIsConnected(lpObj))
	{
		GDReqAllowJoinGuildMatching(nUserIndex, lpMsg->nAllowType, szMemberName, lpObj.m_PlayerData->GuildName);
	}
}

void GameProtocol::CGReqGetAllowListJoinGuildMatching(PMSG_REQ_ALLOWLIST_GUILDMATCHING *lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	CGameObject lpObj = &getGameObject(nUserIndex);

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	int nGuildNumber = lpObj.m_PlayerData->GuildNumber;
	GUILD_INFO_STRUCT * lpstGuildInfo = Guild.SearchGuild_Number(nGuildNumber);

	PMSG_ANS_WAIT_GUILDMATCHING pMsg;
	PHeadSubSetW((BYTE*)&pMsg, 0xED, 0x07, sizeof(pMsg));

	if (!lpstGuildInfo || Guild.GetGuildMemberStatus(lpstGuildInfo->Name, lpObj.Name) != GUILD_MASTER)
	{
		pMsg.nResult = -2;
		IOCP.DataSend(nUserIndex, (BYTE*)&pMsg, sizeof(pMsg));
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

	CGameObject lpObj = &getGameObject(nUserIndex);

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	char szName[MAX_ACCOUNT_LEN + 1];
	memcpy(szName, lpObj.Name, MAX_ACCOUNT_LEN + 1);

	GDReqGetWaitStateListGuildMatching(nUserIndex, szName);
}

void GameProtocol::CGReqRegWantedPartyMember(PMSG_REQ_REG_WANTED_PARTYMEMBER *lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	CGameObject lpObj = &getGameObject(nUserIndex);

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	BOOL bAlreadyParty = FALSE;
	int nPartyNumber = -1;

	if (lpObj.PartyNumber != -1)
	{
		bAlreadyParty = TRUE;
		nPartyNumber = lpObj.PartyNumber;
	}

	PARTY_INFO_LISTDB stList;

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
	memcpy(stList.szLeaderName, lpObj.Name, MAX_ACCOUNT_LEN + 1);
	memcpy(stList.szPassword, lpMsg->szPassword, 5);
	memcpy(stList.szTitle, lpMsg->szTitle, 41);
	memcpy(stList.btWantedClassDetailInfo, lpMsg->btWantedClassDetailInfo, 7);
	stList.btGensType = g_GensSystem.GetGensInfluence(&getGameObject(nUserIndex));
	stList.nLeaderLevel = getGameObject(nUserIndex]->Level + getGameObject(nUserIndex)->m_PlayerData->MasterLevel;
	stList.btLeaderClass = getGameObject(nUserIndex)->Class;
	stList.btApprovalType = lpMsg->btApprovalType;

	if (getGameObject(nUserIndex)->m_PlayerData->ChangeUP == 1)
	{
		stList.btLeaderClass |= 0x08;
	}

	else if (getGameObject(nUserIndex)->m_PlayerData->ChangeUP == 2)
	{
		stList.btLeaderClass |= CS_SET_CHANGEUP_SX;
		stList.btLeaderClass |= CS_SET_CHANGEUP2_SX;
		//		stList.btLeaderClass |= 0x10;
	}

	if (getGameObject(nUserIndex]->m_IfState.use != 0 && getGameObject(nUserIndex)->m_IfState.type == 2)
	{
		PMSG_ANS_REG_WANTED_PARTYMEMBER pMsg;
		PHeadSubSetB((BYTE*)&pMsg, 0xEF, 0x00, sizeof(pMsg));
		pMsg.nResult = -2;

		IOCP.DataSend(nUserIndex, (BYTE*)&pMsg, sizeof(pMsg));
	}

	else
	{
		GDReqRegWantedPartyMember(stList, nUserIndex);
	}
}

void GameProtocol::CGReqGetPartyMatchingList(PMSG_REQ_GET_PARTYMATCHINGLIST * lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	CGameObject lpObj = &getGameObject(nUserIndex);

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

	CGameObject lpObj = &getGameObject(nUserIndex);

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

	memcpy(szMemberName, lpObj.Name, MAX_ACCOUNT_LEN);
	memcpy(szLeaderName, lpMsg->szLeaderName, MAX_ACCOUNT_LEN);
	memcpy(szPassword, lpMsg->szPassword, 4);

	int nTotalLevel = lpObj.Level + lpObj.m_PlayerData->MasterLevel;
	BYTE btChangeUpClass = lpObj.Class;

	if (lpObj.m_PlayerData->ChangeUP == 1)
	{
		btChangeUpClass |= 0x08;
	}

	if (lpObj.m_PlayerData->ChangeUP == 2)
	{
		btChangeUpClass |= CS_SET_CHANGEUP_SX;
		btChangeUpClass |= CS_SET_CHANGEUP2_SX;
		//btChangeUpClass |= 0x08;
		//btChangeUpClass |= 0x18;
	}

	BYTE btClass = lpObj.Class;

	switch (lpObj.Class)
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

	if (getGameObject(nUserIndex]->m_IfState.use != 0 && getGameObject(nUserIndex)->m_IfState.type == 2)
	{
		PMSG_ANS_MEMBERJOIN_PARTYMATCHINGLIST pMsg;
		PHeadSubSetB((BYTE*)&pMsg, 0xEF, 0x02, sizeof(pMsg));
		pMsg.nResult = -4;

		IOCP.DataSend(nUserIndex, (BYTE*)&pMsg, sizeof(pMsg));
	}

	else
	{
		BYTE btGensType = g_GensSystem.GetGensInfluence(&getGameObject(nUserIndex));
		GDReqJoinMemberPartyMatching(szMemberName, szLeaderName,
			szPassword, nUserIndex, lpObj.DBNumber,
			nTotalLevel, btClass, lpMsg->btUseRandomParty,
			btGensType, btChangeUpClass);
	}
}

void GameProtocol::CGReqMemberJoinStateList(PMSG_REQ_MEMBERJOIN_STATELIST_PARTYMATCHING *lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	CGameObject lpObj = &getGameObject(nUserIndex);

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	GDReqMemberJoinStateList(lpObj.Name, nUserIndex);
}

void GameProtocol::CGReqMemberJoinStateListLeader(PMSG_REQ_WAITLIST_PARTYMATCHING *lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	CGameObject lpObj = &getGameObject(nUserIndex);

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	GDReqMemberJoinStateListLeader(lpObj.Name, nUserIndex);
}

void GameProtocol::CGReqAcceptJoinMemberPartyMatching(PMSG_REQ_ACCEPTMEMBER_PARTYMATCHING *lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	CGameObject lpObj = &getGameObject(nUserIndex);

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	char szMemberName[MAX_ACCOUNT_LEN + 1];
	char szLeaderName[MAX_ACCOUNT_LEN + 1];

	memset(szMemberName, 0x00, MAX_ACCOUNT_LEN + 1);
	memset(szLeaderName, 0x00, MAX_ACCOUNT_LEN + 1);

	memcpy(szMemberName, lpMsg->szMemberName, MAX_ACCOUNT_LEN);
	memcpy(szLeaderName, lpObj.Name, MAX_ACCOUNT_LEN);

	GDReqAcceptMemberJoin(nUserIndex, lpMsg->btType, szLeaderName, szMemberName, 0);
}

void GameProtocol::CGReqCancelPartyMatching(PMSG_REQ_CANCEL_JOIN_PARTYMATCHING *lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	CGameObject lpObj = &getGameObject(nUserIndex);

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	GDReqCancelPartyMatching(nUserIndex, lpMsg->btType);
}

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
	PBMSG_HEAD2 h;
	char Name[MAX_ACCOUNT_LEN + 1];
	BYTE btBuffCount;
	stBuffList stBuffList[32];
};
#pragma pack ()

void GameProtocol::GCDisplayBuffeffectPartyMember(CGameObject &Obj)
{
	if (ObjectMaxRange(aIndex) == false)
	{
		return;
	}

	

	if (gObjIsConnected(aIndex) == false)
	{
		return;
	}

	if (lpObj.PartyNumber < 0)
	{
		return;
	}

	PMSG_BUFFLIST_SEND_PARTY pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x2E, sizeof(pMsg));

	pMsg.btBuffCount = lpObj.m_BuffEffectCount;

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		memcpy(pMsg.Name, lpObj.m_PlayerData->m_RealNameOfUBF, MAX_ACCOUNT_LEN + 1);
	}

	else
	{
		memcpy(pMsg.Name, lpObj.Name, MAX_ACCOUNT_LEN + 1);
	}

	int nPartyIndex = lpObj.PartyNumber;

	if (pMsg.btBuffCount > MAX_BUFFEFFECT)
	{
		pMsg.btBuffCount = MAX_BUFFEFFECT;
	}

	for (int nBuffCnt = 0; nBuffCnt < pMsg.btBuffCount; nBuffCnt++)
	{
		pMsg.stBuffList[nBuffCnt].btBuffIndex = lpObj.m_BuffEffectList[nBuffCnt].BuffIndex;
		pMsg.stBuffList[nBuffCnt].nBuffDuration = lpObj.m_BuffEffectList[nBuffCnt].EffectDuration;
	}

	for (int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		if (gObjIsConnected(gParty.m_PartyS[nPartyIndex].Number[i]))
		{
			IOCP.DataSend(gParty.m_PartyS[nPartyIndex].Number[i], (BYTE*)&pMsg, pMsg.h.size);
		}
	}
}

void GameProtocol::GCPartyMemberPosition(int nIndex)
{
	if (ObjectMaxRange(nIndex) == false)
	{
		return;
	}

	if (!gObjIsConnected(nIndex))
	{
		return;
	}

	if (getGameObject(nIndex)->m_PlayerData->m_bSendMemberPos == FALSE)
	{
		return;
	}

	if (getGameObject(nIndex)->PartyNumber < 0)
	{
		return;
	}

	PMSG_MEMBER_POS_INFO_SEND pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xE7, 0x01, sizeof(pMsg));

	int nPartyNumber = getGameObject(nIndex)->PartyNumber;

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
				memcpy(pMsg.stPosInfo[pMsg.btCount].szName, getGameObject(nTargetIndex)->m_PlayerData->m_RealNameOfUBF, MAX_ACCOUNT_LEN + 1);
			}

			else
			{
				memcpy(pMsg.stPosInfo[pMsg.btCount].szName, getGameObject(nTargetIndex)->Name, MAX_ACCOUNT_LEN + 1);
			}

			pMsg.stPosInfo[pMsg.btCount].btMapNumber = getGameObject(nTargetIndex)->MapNumber;
			pMsg.stPosInfo[pMsg.btCount].btPosX = getGameObject(nTargetIndex)->X;
			pMsg.stPosInfo[pMsg.btCount].btPosY = getGameObject(nTargetIndex)->Y;

			pMsg.btCount++;
		}

		IOCP.DataSend(nIndex, (BYTE*)&pMsg, pMsg.h.size);
	}
}

void GameProtocol::CGReqSendMemberPosInfoStart(int nIndex)
{
	if (ObjectMaxRange(nIndex) == false)
	{
		return;
	}

	if (!gObjIsConnected(nIndex))
	{
		return;
	}

	if (getGameObject(nIndex)->Type != OBJ_USER)
	{
		return;
	}

	if (getGameObject(nIndex)->PartyNumber < 0)
	{
		return;
	}

	getGameObject(nIndex)->m_PlayerData->m_bSendMemberPos = TRUE;
}

void GameProtocol::CGReqSendMemberPosInfoStop(int nIndex)
{
	if (ObjectMaxRange(nIndex) == false)
	{
		return;
	}

	if (!gObjIsConnected(nIndex))
	{
		return;
	}

	if (getGameObject(nIndex)->Type != OBJ_USER)
	{
		return;
	}

	if (getGameObject(nIndex)->PartyNumber < 0)
	{
		return;
	}

	getGameObject(nIndex)->m_PlayerData->m_bSendMemberPos = FALSE;
}

void GameProtocol::GCReqSendNpcPosInfo(PMSG_REQ_NPC_POSITION *lpMsg, int nIndex)
{
	if (ObjectMaxRange(nIndex) == false)
	{
		return;
	}

	if (!gObjIsConnected(nIndex))
	{
		return;
	}

	if (getGameObject(nIndex)->Type != OBJ_USER)
	{
		return;
	}

	if (getGameObject(nIndex)->PartyNumber < 0)
	{
		return;
	}

	if (lpMsg->btType)
	{
		getGameObject(nIndex)->m_PlayerData->m_bSendNpcPos = FALSE;
	}

	else
	{
		getGameObject(nIndex)->m_PlayerData->m_bSendNpcPos = TRUE;
	}
}

void GameProtocol::CGReqCCF_DayTime(CGameObject &Obj)
{
	if (ObjectMaxRange(Obj.m_Index) == false)
	{
		return;
	}

	if (gObjIsConnected(Obj.m_Index) == false)
	{
		return;
	}

	if (g_ConfigRead.server.GetServerType() != SERVER_BATTLECORE)
	{
		return;
	}

	Obj.m_bCCF_UI_Using = true;
	g_ChaosCastleFinal.Send_CCF_Schedule(Obj.m_Index);
}

void GameProtocol::CGReqCCF_EnterCheck(CGameObject &Obj)
{
	if (ObjectMaxRange(Obj.m_Index) == false)
	{
		return;
	}

	if (Obj.Type != OBJ_USER || Obj.Connected <= PLAYER_LOGGED || g_ConfigRead.server.GetServerType() != SERVER_BATTLECORE)
	{
		return;
	}

	PMSG_RESULT_CCF_ENTERCHECK pResult;
	PHeadSubSetB((BYTE*)&pResult, 0xAF, 0x05, sizeof(pResult));

	if (Obj.MapNumber != MAP_INDEX_LORENMARKET)
	{
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 593));
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pNotice, pNotice.h.size);

		pResult.byResult = 11;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (!Obj.m_bCCF_UI_Using)
	{
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 593));
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pNotice, pNotice.h.size);

		pResult.byResult = 6;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (Obj.m_IfState.use && Obj.m_IfState.type != 12)
	{
		pResult.byResult = 12;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_Index, &pResult.h.c, pResult.h.size);
		return;
	}

	if (Obj.m_byCCF_CanEnter)
	{
		sLog->outBasic("[Chaos Castle Survival] [ CGReqCCF_EnterCheck ] ACC:%s, NAME:%s m_byCCF_CanEnter :%d ", Obj.AccountID, Obj.Name, Obj.m_byCCF_CanEnter);
		Obj.m_byCCF_CanEnter = 0;

		pResult.byResult = 13;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (!g_ConfigRead.pk.bPkPenaltyDisable)
	{
		BOOL bPlayerKiller = FALSE; //Season 2.5 add-on

		if (Obj.PartyNumber >= 0) //Season 2.5 add-on
		{
			if (gParty.GetPKPartyPenalty(Obj.PartyNumber) >= 6)
			{
				bPlayerKiller = TRUE;
			}
		}
		else if (Obj.m_PK_Level >= 6)
		{
			bPlayerKiller = TRUE;
		}

		if (bPlayerKiller == TRUE)
		{
			PMSG_NOTICE pNotice;
			TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 593));
			IOCP.DataSend(Obj.m_Index, (BYTE*)&pNotice, pNotice.h.size);

			pResult.byResult = 14;
			pResult.byRemainTime = 0;
			pResult.byRemainTime_LOW = 0;
			IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}
	}

	if (gObjDuelCheck(Obj) == TRUE)
	{
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 594));
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pNotice, pNotice.h.size);

		pResult.byResult = 15;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (Obj.m_PlayerData->lpGuild && Obj.m_PlayerData->lpGuild->WarState == 1)
	{
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 595));
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pNotice, pNotice.h.size);

		pResult.byResult = 16;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (Obj.pInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 3) ||
		Obj.pInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 2) ||
		Obj.pInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 37))
	{
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 596));
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pNotice, pNotice.h.size);

		pResult.byResult = 17;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	BOOL bPlayerKiller = FALSE; //Season 2.5 add-on

	if (Obj.PartyNumber >= 0) //Season 2.5 add-on
	{
		if (gParty.GetPKPartyPenalty(Obj.PartyNumber) >= 6)
		{
			bPlayerKiller = TRUE;
		}
	}
	else if (Obj.m_PK_Level >= 6)
	{
		bPlayerKiller = TRUE;
	}

	if (bPlayerKiller == TRUE)
	{
		pResult.byResult = 8;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_Index, &pResult.h.c, pResult.h.size);
		return;
	}

	if (g_ChaosCastleFinal.m_nCCF_Level > Obj.Level)
	{
		pResult.byResult = 1;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_Index, &pResult.h.c, pResult.h.size);
		return;
	}

	if (g_ChaosCastleFinal.CheckWearingMOPH(Obj.m_Index) == TRUE)
	{
		GCServerMsgStringSend(Lang.GetText(0, 576), iIndex, 1);

		pResult.byResult = 18;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_Index, &pResult.h.c, pResult.h.size);
		return;
	}
	if ((MapC[Obj.MapNumber].GetAttr(Obj.X, Obj.Y) & 1) != 1)
	{
		pResult.byResult = 19;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_Index, &pResult.h.c, pResult.h.size);
		return;
	}

	int nBattleType = g_ChaosCastleFinal.Check_CCF_DayType();

	if (!nBattleType)
	{
		sLog->outBasic("[Chaos Castle Survival][CGReqCCF_EnterCheck] No OpenDay , Index:%d,Name:%s", iIndex, Obj.Name);

		pResult.byResult = 10;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_Index, &pResult.h.c, pResult.h.size);
		return;
	}
	if (nBattleType == 3 && g_ChaosCastleFinal.CheckOpenOrNotOpenCCF_Final(3) == TRUE)
	{
		sLog->outBasic("[Chaos Castle Survival][CGReqCCF_EnterCheck] Not Final Time, Index:%d,Name:%s", iIndex, Obj.Name);

		pResult.byResult = 2;
		int nRemainTime = g_ChaosCastleFinal.GetFinalRemainTime(Obj.m_Index);
		pResult.byRemainTime = HIBYTE(nRemainTime);
		pResult.byRemainTime_LOW = LOBYTE(nRemainTime);
		IOCP.DataSend(Obj.m_Index, &pResult.h.c, pResult.h.size);
		return;
	}
	if (g_ChaosCastleFinal.CheckCanEnter() == FALSE)
	{
		pResult.byResult = 2;
		int nRemainTime = g_ChaosCastleFinal.GetRemainTime(Obj.m_Index);
		pResult.byRemainTime = HIBYTE(nRemainTime);
		pResult.byRemainTime_LOW = LOBYTE(nRemainTime);
		IOCP.DataSend(Obj.m_Index, &pResult.h.c, pResult.h.size);
		return;
	}

	int nCheckIndex = g_ChaosCastleFinal.EnterCheck_CCF_User(Obj.m_Index);

	if (nCheckIndex == -1)
	{
		pResult.byResult = 5;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_Index, &pResult.h.c, pResult.h.size);
		return;
	}

	if (nCheckIndex == -2)
	{
		pResult.byResult = 9;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_Index, &pResult.h.c, pResult.h.size);
		return;
	}

	if (!nBattleType)
	{
		pResult.byResult = 10;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_Index, &pResult.h.c, pResult.h.size);
		return;
	}

	if (nBattleType == 1)
	{
		if (!g_ChaosCastleFinal.CheckUserEnterMoney(iIndex, 1))
		{
			pResult.byResult = 7;
			pResult.byRemainTime = 0;
			pResult.byRemainTime_LOW = 0;
			IOCP.DataSend(Obj.m_Index, &pResult.h.c, pResult.h.size);
			return;
		}

		Obj.m_byCCF_CanEnter = 1;

		int nReturn = this->CGReqEnterChaosCastleFinal(Obj.m_Index);

		if (nReturn)
		{
			sLog->outBasic("[Chaos Castle Survival][gObj_EnterChaosCastleFinal] ACC :%s , NAME :%s return :%d ", Obj.AccountID, Obj.Name, nReturn);
		}

		return;
	}

	if (nBattleType == 2)
	{
		if (g_ChaosCastleFinal.CheckUserEnterMoney(iIndex, 2))
		{
			Obj.m_byCCF_CanEnter = -1;
			g_ChaosCastleFinal.GD_Req_Get_Permission(Obj);
			Obj.m_nCCF_CertiTick = GetTickCount();
		}
		else
		{
			pResult.byResult = 7;
			pResult.byRemainTime = 0;
			pResult.byRemainTime_LOW = 0;
			IOCP.DataSend(Obj.m_Index, &pResult.h.c, pResult.h.size);
		}
	}
	else
	{
		if (g_ChaosCastleFinal.CheckUserEnterMoney(iIndex, 3))
		{
			Obj.m_byCCF_CanEnter = -1;
			g_ChaosCastleFinal.GD_Req_Get_Permission(Obj);
			Obj.m_nCCF_CertiTick = GetTickCount();
		}
		else
		{
			pResult.byResult = 7;
			pResult.byRemainTime = 0;
			pResult.byRemainTime_LOW = 0;
			IOCP.DataSend(Obj.m_Index, &pResult.h.c, pResult.h.size);
		}
	}
}

int GameProtocol::CGReqEnterChaosCastleFinal(CGameObject &Obj)
{
	if (ObjectMaxRange(Obj.m_Index) == false)
	{
		return -4;
	}

	if (Obj.Type == OBJ_USER && Obj.Connected <= PLAYER_LOGGED)
	{
		return -5;
	}

	PMSG_RESULT_CCF_ENTERCHECK pResult;
	PHeadSubSetB((BYTE*)&pResult, 0xAF, 0x05, sizeof(pResult));

	if (g_ChaosCastleFinal.PayUserEnterMoney(iIndex, Obj.m_byCCF_CanEnter) == FALSE)
	{
		pResult.byResult = 7;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return -3;
	}

	int nCCFUserIndex = 0;
	int nEnterCheck = g_ChaosCastleFinal.EnterCCF(iIndex, nCCFUserIndex);

	if (nEnterCheck)
	{
		sLog->outBasic("[Chaos Castle Survival] (ACC:%s,NAME:%s) EnterCCF return ERROR :%d ", Obj.AccountID, Obj.Name, nEnterCheck);

		pResult.byResult = 20;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return nEnterCheck;
	}

	else
	{
		pResult.byResult = 0;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);

		Obj.m_nCCF_UserIndex = nCCFUserIndex;
		Obj.m_byCCF_CanEnter = 0;
		Obj.m_IfState.state = 0;
		Obj.m_IfState.type = 0;
		Obj.m_IfState.use = 0;
		Obj.m_byKillMobCount = 0;
		Obj.m_byKillUserCount = 0;
		Obj.m_bCCF_Quit_Msg = 0;

		if (Obj.m_bPShopOpen == true)
			g_PersonalStore.CGPShopReqClose(Obj.m_Index);

		if (Obj.PartyNumber >= 0)
		{
			PMSG_PARTYDELUSER pMsg;

			int pindex = gParty.GetIndex(Obj.PartyNumber, iIndex, Obj.DBNumber);

			if (pindex >= 0)
			{
				pMsg.Number = pindex;
				CGPartyDelUser(&pMsg, iIndex, 0);
			}
		}

		if (Obj.Class == CLASS_ELF)
		{
			if (Obj.m_RecallMon >= 0)
			{
				GCRecallMonLife(iIndex, 60, 0);
				gObjMonsterCallKill(Obj.m_Index);
			}
		}

		GCServerMsgStringSend(Lang.GetText(0, 577), iIndex, 1);
		Obj.MapNumber = MAP_INDEX_CHAOSCASTLE_SURVIVAL;
		gObjMoveGate(iIndex, 443);

		sLog->outBasic("[Chaos Castle Survival] (ACC:%s,NAME:%s) Enter CCF_Tryout", Obj.AccountID, Obj.Name);
		return 0;
	}
}

void GameProtocol::CGRequestRepositionUserInCCF(PMSG_REQ_REPOSUSER_IN_CCF *lpMsg, CGameObject &Obj)
{
	if (lpObj.MapNumber != MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		return;
	}

	if (!gObjIsConnected(aIndex))
	{
		return;
	}

	if (g_ChaosCastleFinal.ObjSetPosition(aIndex, lpObj.X, lpObj.Y) == TRUE)
	{
		lpObj.m_nCCF_BlowTime = GetTickCount();
	}
}

void GameProtocol::CG_CCF_UI_OnOff(PMSG_CCF_UI_ONOFF *lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	if (getGameObject(nUserIndex]->Type != OBJ_USER || getGameObject(nUserIndex)->Connected <= PLAYER_LOGGED)
	{
		return;
	}

	if (getGameObject(nUserIndex)->m_byCCF_CanEnter == 0xFF)
		getGameObject(nUserIndex)->m_byCCF_CanEnter = 0;

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
		getGameObject(nUserIndex)->m_bCCF_UI_Using = 0;
}

void GameProtocol::CGReq_CCF_Ranking(PMSG_REQ_CCF_RANKING *lpMsg, CGameObject &Obj)
{
	if (ObjectMaxRange(aIndex) == false)
	{
		return;
	}

	if (g_ConfigRead.server.GetServerType() != SERVER_BATTLECORE)
	{
		return;
	}

	

	if (lpObj.m_bCCF_UI_Using == false)
	{
		return;
	}

	if (lpObj.MapNumber != MAP_INDEX_LORENMARKET)
	{
		return;
	}

	if (g_ChaosCastleFinal.Check_CCF_DayType() == FALSE)
	{
		_stPMSG_CCF_RANK_INFO pMsg;

		PHeadSubSetW((BYTE*)&pMsg, 0xAF, 0x07, sizeof(pMsg));
		pMsg.btResult = 2;
		pMsg.btCnt = 0;
		pMsg.h.sizeH = HIBYTE(sizeof(pMsg));
		pMsg.h.sizeL = LOBYTE(sizeof(pMsg));
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, sizeof(pMsg));
	}

	if (g_ChaosCastleFinal.m_FirstRankingInfoLoad)
	{
		g_ChaosCastleFinal.Send_RankingInfo(lpMsg->byRankingType, aIndex);
	}

	else
	{
		g_ChaosCastleFinal.GD_Load_CCF_RankingList(g_ChaosCastleFinal.Check_CCF_DayType());

		_stPMSG_CCF_RANK_INFO pMsg;

		PHeadSubSetW((BYTE*)&pMsg, 0xAF, 0x07, sizeof(pMsg));
		pMsg.btResult = 3;
		pMsg.btCnt = 0;
		pMsg.h.sizeH = HIBYTE(sizeof(pMsg));
		pMsg.h.sizeL = LOBYTE(sizeof(pMsg));
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, sizeof(pMsg));

		sLog->outBasic("[Chaos Castle Survival][CGReq_CCF_Ranking] GD_Load_CCF_RankingList Send ");
	}
}
void GameProtocol::GCMuunItemDurSend(CGameObject &Obj, BYTE pos, BYTE dur)
{
	PMSG_MUUNITEM_DUR pMsg;

	PHeadSubSetB(&pMsg.h.c, 0x4E, 0x03, sizeof(pMsg));
	pMsg.IPos = pos;
	pMsg.Dur = dur;
	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCMuunEquipmentChange(CGameObject &Obj, int pos)
{
	CGameObject lpObj;
	PMSG_USER_MUUN_EQUIPMENT_CHANGED pChange;

	lpObj = &getGameObject(aIndex);
	PHeadSubSetB((BYTE*)&pChange, 0x4E, 0x06, sizeof(pChange));

	pChange.NumberH = SET_NUMBERH(aIndex);
	pChange.NumberL = SET_NUMBERL(aIndex);
	ItemByteConvert(pChange.ItemInfo, lpObj.pMuunInventory[pos]);
	pChange.ItemInfo[1] = 16 * pos;
	pChange.ItemInfo[1] = lpObj.pMuunInventory[pos].m_Level & 0xF | 16 * pos;
	this->MsgSendV2(lpObj, (BYTE*)&pChange, (unsigned __int8)pChange.h.size);
}

void GameProtocol::GCMuunInventoryItemDeleteSend(CGameObject &Obj, BYTE pos, BYTE flag)
{
	PMSG_PETINVENTORY_DELETE pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0x4E, 0x04, sizeof(pMsg));
	pMsg.IPos = pos;
	pMsg.Flag = flag;
	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCMuunInventoryItemOneSend(CGameObject &Obj, int pos)
{
	PMSG_MUUN_INVEN_ITEM_MOD pMsg;

	if (lpObj.pMuunInventory[pos].IsItem())
	{
		PHeadSubSetB((BYTE*)&pMsg, 0x4E, 0x05, sizeof(pMsg));
		pMsg.Pos = pos;
		ItemByteConvert(pMsg.ItemInfo, lpObj.pMuunInventory[pos]);
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
	}
}

void GameProtocol::CGMuunItemGetRequest(PMSG_MUUNITEM_GETREQUEST *lpMsg, CGameObject &Obj)
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

	if (!gObjIsConnected(aIndex))
	{
		//this->GCSendDisableReconnect(aIndex);
		IOCP.CloseClient(aIndex);
		return;
	}

	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	if (lpObj.CloseType != -1)
		return;

	if (lpObj.m_PlayerData->m_bSecurityCheck == false)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	if (lpObj.DieRegen != 0)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(lpObj.m_Index, &pResult.h.c, (unsigned __int8)pResult.h.size);

		return;
	}

	if (g_NewPVP.IsObserver(getGameObject(aIndex)))
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if (lpObj.m_IfState.use == 1)
	{
		GCServerMsgStringSend(Lang.GetText(0, 597), aIndex, 1);

		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, (pResult.h.size - 12));

		return;
	}

	if (!::gObjFixMuunInventoryPointer(aIndex))

		if (lpObj.pTransaction == TRUE)
		{
			return;
		}

	item_num = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	if (MaxItemTypeRange(item_num) == FALSE)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	map_num = lpObj.MapNumber;

	if (MAX_MAP_RANGE(map_num) == FALSE)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	lpItem = &MapC[map_num].m_CItemObject[item_num];

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
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

			return;
		}

		pResult.result = gObjChkMuunInventoryEmpty(&getGameObject(aIndex));

		if (pResult.result == 255)
		{
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

			return;
		}

		if (g_CMuunSystem.IsMuunExpireDate(lpItem->m_Type) == TRUE)
		{
			lpItem->SetMuunItemPeriodExpire();
		}

		ItemByteConvert((BYTE*)&pResult.Data[0], lpItem->m_Type, lpItem->m_Option1, lpItem->m_Option2,
			lpItem->m_Option3, lpItem->m_Level, lpItem->m_Durability, lpItem->m_NewOption,
			lpItem->m_SetOption, lpItem->m_JewelOfHarmonyOption, lpItem->m_ItemOptionEx, lpItem->m_SocketOption, lpItem->m_BonusSocketOption, 0);

		if (MapC[map_num].ItemGive(aIndex, item_num, 0) == 1)
		{
			BYTE Ret = ::gObjMuunInventoryInsertItem(aIndex, lpItem);

			if (Ret == 0xFF)
				pResult.result = -1;

			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

			if (Ret == 0xFF)
			{
				GCMuunInventoryItemListSend(aIndex);
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
						this->GCSendGetItemInfoForParty(aIndex, lpItem);

						return;
					}

				}

				else
				{

				}

				this->GCSendGetItemInfoForParty(aIndex, lpItem);
			}
		}

		else
		{
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);

			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}
	}

	else
	{

		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);

		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}
}

void GameProtocol::CGMuunItemSellRequestRecv(PMSG_MUUN_ITEM_SELLREQUEST *lpMsg, CGameObject &Obj)
{
	PMSG_MUUN_ITEM_SELLRESULT pResult;
	

	PHeadSubSetB((BYTE*)&pResult, 0x4E, 0x09, sizeof(pResult));

	pResult.Result = 0;
	pResult.Money = 0;

	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	if (lpObj.CloseType != -1)
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (lpObj.m_IfState.use > 0 && lpObj.m_IfState.type != 3)
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (!PacketCheckTime(aIndex))
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (!::gObjFixMuunInventoryPointer(aIndex))
	{

	}

	if (lpObj.pTransaction == 1)
	{
		return;
	}

	if (lpObj.TargetNpcNumber == -1)
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	CGameObject lpShopObj = &getGameObject(lpObj.TargetNpcNumber);

	SHOP_DATA * lpShopData = g_ShopMng.GetShop(lpObj, lpShopObj);

	if (!lpShopData)
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (!gObjCanItemTouch(lpObj, 3))
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (lpMsg->Pos > MUUN_INVENTORY_SIZE - 1)
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (gObjCheckSerial0ItemList(&lpObj.pMuunInventory[lpMsg->Pos]) != FALSE)
	{
		MsgOutput(aIndex, Lang.GetText(0, 259));
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if (!IsSellToNPCItemObject(lpObj.pMuunInventory[lpMsg->Pos].m_Type))
	{
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (lpObj.pMuunInventory[lpMsg->Pos].IsItem() == FALSE)
	{
		pResult.Result = false;
		pResult.Money = lpObj.m_PlayerData->Money;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

	}
	else
	{
		int sellmoney = 0;

		if (g_CMuunSystem.IsMuunItem(lpObj.pMuunInventory[lpMsg->Pos].m_Type) == TRUE)
		{
			sellmoney = 500000;
		}

		else if (g_CMuunSystem.IsStoneofEvolution(lpObj.pMuunInventory[lpMsg->Pos].m_Type) == TRUE)
		{
			sellmoney = 200000;
		}

		else if (lpObj.pMuunInventory[lpMsg->Pos].m_Type == ITEMGET(13, 239))
		{
			sellmoney = 300000;
		}

		else if (lpObj.pMuunInventory[lpMsg->Pos].m_Type == ITEMGET(13, 240))
		{
			sellmoney = 1000000;
		}

		else
		{
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}

		if (gObjCheckMaxZen(aIndex, sellmoney) == FALSE)
		{
			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}

		lpObj.m_PlayerData->Money += sellmoney;
		pResult.Result = 1;
		pResult.Money = lpObj.m_PlayerData->Money;

		BYTE NewOption[MAX_EXOPTION_SIZE];
		ItemIsBufExOption(NewOption, &lpObj.pMuunInventory[lpMsg->Pos]);

		if (lpMsg->Pos == 0 || lpMsg->Pos == 1)
		{
			WORD nItemCode = lpObj.pMuunInventory[lpMsg->Pos].m_Type;
			UINT64 nSerial = lpObj.pMuunInventory[lpMsg->Pos].m_Number;

			if (lpMsg->Pos == 0)
			{
				lpObj.m_wMuunItem = -1;
			}

			else if (lpMsg->Pos == 1)
			{
				lpObj.m_wMuunSubItem = -1;
			}

			g_CMuunSystem.RemoveUserMuunEffect(lpObj, lpMsg->Pos);
			this->GCMuunEquipmentChange(aIndex, lpMsg->Pos);
		}

		gObjMuunInventoryDeleteItem(aIndex, lpMsg->Pos);
		gObjMakePreviewCharSet(aIndex);

		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
	}
}

void GameProtocol::CGReqEventInvenOpen(BYTE* lpRecv, CGameObject &Obj)
{
	if (g_CMuRummyMng.IsMuRummyEventOn() == true)
	{
		g_CMuRummyMng.CGReqMuRummyEventOpen((PMSG_REQ_MURUMMY_EVENT_OPEN *)lpRecv, aIndex);
		return;
	}

	if (g_ConfigRead.data.common.EventInventoryOpen)
	{
		PMSG_ANS_MURUMMY_EVENT_OPEN pMsg;
		PHeadSubSetB((BYTE*)&pMsg, 0x4D, 0x0F, sizeof(pMsg));

		pMsg.btResult = 3;

		time_t tmEndTime;
		DWORD dw32EndTime;
		SystemTimeToUnixTime(&g_ConfigRead.data.common.EventEndDate, &tmEndTime);
		dw32EndTime = (DWORD)tmEndTime;

		pMsg.btEventTime1 = SET_NUMBERH(SET_NUMBERHW(dw32EndTime));
		pMsg.btEventTime2 = SET_NUMBERL(SET_NUMBERHW(dw32EndTime));
		pMsg.btEventTime3 = SET_NUMBERH(SET_NUMBERLW(dw32EndTime));
		pMsg.btEventTime4 = SET_NUMBERL(SET_NUMBERLW(dw32EndTime));

		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
	}
}

void GameProtocol::CGEventItemGetRequest(PMSG_EVENTITEM_GETREQUEST *lpMsg, CGameObject &Obj)
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

	if (!gObjIsConnected(aIndex))
	{
		//this->GCSendDisableReconnect(aIndex);
		IOCP.CloseClient(aIndex);
		return;
	}

	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	if (lpObj.CloseType != -1)
		return;

	if (g_CMuRummyMng.IsMuRummyEventOn() == false && g_ConfigRead.data.common.EventInventoryOpen == false)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	if (lpObj.m_PlayerData->m_bSecurityCheck == false)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	if (lpObj.DieRegen != 0)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	if (g_NewPVP.IsObserver(getGameObject(aIndex)) == TRUE)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	if (!::gObjFixEventInventoryPointer(aIndex))
		sLog->outBasic("[Fix EventInv.Ptr] False Location - %s, %d", __FILE__, __LINE__);

	if (lpObj.pTransaction == TRUE)
	{
		sLog->outBasic("[%s][%s] CGEventItemGetRequest() Failed : Transaction == 1, IF_TYPE : %d",
			lpObj.AccountID, lpObj.Name, lpObj.m_IfState.type);
		return;

	}

	item_num = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	if (MaxItemTypeRange(item_num) == FALSE)
	{
		sLog->outBasic("error-L3 : %s %d", __FILE__, __LINE__);
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	map_num = lpObj.MapNumber;

	if (MAX_MAP_RANGE(map_num) == FALSE)
	{
		sLog->outBasic("error-L3 : %s %d", __FILE__, __LINE__);
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	lpItem = &MapC[map_num].m_CItemObject[item_num];

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
			IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pResult, pResult.h.size);

			return;
		}

		if (lpItem->m_Type == ITEMGET(14, 278))
		{
			int iInventoryIndex;

			for (int iLoop = 0; iLoop < 32; iLoop++)
			{
				iInventoryIndex = -1;

				if (lpObj.pEventInventory[iLoop].m_Type == ITEMGET(14, 278) &&
					lpObj.pEventInventory[iLoop].m_Durability < 25.0)
				{
					iInventoryIndex = iLoop;
				}

				if (iInventoryIndex < 0 || iInventoryIndex > EVENT_INVENTORY_SIZE - 1)
				{
					continue;
				}

				if (MapC[map_num].ItemGive(aIndex, item_num, 1) == TRUE)
				{
					BYTE NewOption[MAX_EXOPTION_SIZE];

					::ItemIsBufExOption(NewOption, (lpItem != NULL) ? (CItemObject*)&lpItem->m_Number : NULL);

					pResult.result = -3;
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

					lpObj.pEventInventory[iInventoryIndex].m_Durability += lpItem->m_Durability;

					if (lpObj.pEventInventory[iInventoryIndex].m_Durability < 25.0)
					{
						this->GCEventItemDurSend(aIndex, iInventoryIndex, lpObj.pEventInventory[iInventoryIndex].m_Durability);
						return;
					}

					int iDurGap = lpObj.pEventInventory[iInventoryIndex].m_Durability - 25.0;
					BYTE btRet = gObjEventInventoryInsertItemTemp(&getGameObject(aIndex), lpItem);
					gObjEventInventoryItemSet(aIndex, iInventoryIndex, -1);
					lpObj.pEventInventory[iInventoryIndex].Clear();
					this->GCEventInventoryItemDeleteSend(aIndex, iInventoryIndex, 1);

					ItemSerialCreateSend(aIndex, 226, lpObj.X, lpObj.Y, ITEMGET(14, 244), lpItem->m_Level, 0, 0, 0, 0, aIndex, 0, 0, 0, 0, 0);

					if (iDurGap <= 0)
					{
						return;
					}

					if (btRet == 255)
					{
						ItemSerialCreateSend(aIndex, lpObj.MapNumber, lpObj.X, lpObj.Y, ItemGetNumberMake(14, 278), lpItem->m_Level, 0, 0, 0, 0, aIndex, 0, 0, 0, 0, 0);
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

				if (lpObj.pEventInventory[iLoop].m_Type == ITEMGET(14, 215) &&
					lpObj.pEventInventory[iLoop].m_Durability < 24.0)
				{
					iInventoryIndex = iLoop;
				}

				if (iInventoryIndex < 0 || iInventoryIndex > EVENT_INVENTORY_SIZE - 1)
				{
					continue;
				}

				if (MapC[map_num].ItemGive(aIndex, item_num, 1) == TRUE)
				{
					BYTE NewOption[MAX_EXOPTION_SIZE];

					::ItemIsBufExOption(NewOption, (lpItem != NULL) ? (CItemObject*)&lpItem->m_Number : NULL);

					pResult.result = -3;
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

					lpObj.pEventInventory[iInventoryIndex].m_Durability += lpItem->m_Durability;

					if (lpObj.pEventInventory[iInventoryIndex].m_Durability < 24.0)
					{
						this->GCEventItemDurSend(aIndex, iInventoryIndex, lpObj.pEventInventory[iInventoryIndex].m_Durability);
						return;
					}

					int iDurGap = lpObj.pEventInventory[iInventoryIndex].m_Durability - 24.0;
					BYTE btRet = gObjEventInventoryInsertItemTemp(&getGameObject(aIndex), lpItem);
					gObjEventInventoryItemSet(aIndex, iInventoryIndex, -1);
					lpObj.pEventInventory[iInventoryIndex].Clear();
					this->GCEventInventoryItemDeleteSend(aIndex, iInventoryIndex, 1);

					ItemSerialCreateSend(aIndex, 226, lpObj.X, lpObj.Y, ITEMGET(14, 216), lpItem->m_Level, 0, 0, 0, 0, aIndex, 0, 0, 0, 0, 0);

					if (iDurGap <= 0)
					{
						return;
					}

					if (btRet == 255)
					{
						ItemSerialCreateSend(aIndex, lpObj.MapNumber, lpObj.X, lpObj.Y, ItemGetNumberMake(14, 215), lpItem->m_Level, 0, 0, 0, 0, aIndex, 0, 0, 0, 0, 0);
						return;
					}

					lpItem->m_State = 2;
					lpItem->Give = 0;
					lpItem->live = 1;
					lpItem->m_Durability = iDurGap;
				}
			}
		}

		pResult.result = gObjEventInventoryInsertItemTemp(&getGameObject(aIndex), lpItem);

		if (pResult.result == 255)
		{
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);

			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}

		ItemByteConvert((BYTE*)&pResult.Data[0], lpItem->m_Type, lpItem->m_Option1, lpItem->m_Option2,
			lpItem->m_Option3, lpItem->m_Level, lpItem->m_Durability, lpItem->m_NewOption,
			lpItem->m_SetOption, lpItem->m_JewelOfHarmonyOption, lpItem->m_ItemOptionEx, lpItem->m_SocketOption, lpItem->m_BonusSocketOption, 0);

		if (MapC[map_num].ItemGive(aIndex, item_num, false) == TRUE)
		{
			BYTE Ret = ::gObjEventInventoryInsertItem(aIndex, lpItem);

			if (Ret == 0xFF)
				pResult.result = -1;

			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

			if (Ret == 0xFF)
			{
				this->GCEventInventoryItemListSend(aIndex);
			}
			else
			{
				BYTE NewOption[MAX_EXOPTION_SIZE];

				::ItemIsBufExOption(NewOption, (lpItem != NULL) ? (CItemObject*)&lpItem->m_Number : NULL);

				this->GCSendGetItemInfoForParty(aIndex, lpItem);
			}
		}

		else
		{
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);

			IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}
	}

	else
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);

		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}
}

bool GameProtocol::CGEventItemDropRequest(PMSG_EVENTITEM_THROW *lpMsg, CGameObject &Obj)
{
	BOOL CopyItem = FALSE;
	BYTE NewOption[MAX_EXOPTION_SIZE];

	if (!PacketCheckTime(aIndex))
		return FALSE;

	if (lpObj.Type != OBJ_USER)
	{
		return FALSE;
	}

	PMSG_EVENTITEM_THROW_RESULT pResult;

	PHeadSubSetB((BYTE*)&pResult, 0x4D, 0x01, sizeof(pResult));
	pResult.Result = true;
	pResult.Ipos = lpMsg->Ipos;

	if (lpObj.m_IfState.use != 0)
	{
		if (lpObj.m_IfState.type != 3)
		{
			pResult.Result = false;
			return FALSE;
		}
	}

	if (lpObj.m_PlayerData->m_bSecurityCheck == false)
	{
		MsgOutput(aIndex, Lang.GetText(0, 394));
		pResult.Result = false;
	}

	if (lpObj.CloseType != -1)
		pResult.Result = false;

	if (lpObj.m_bMapSvrMoveQuit == true)
	{
		sLog->outBasic("[ANTI-HACK][Item Duplication] - Item Drop during MapServer Move [%s][%s]",
			lpObj.AccountID, lpObj.Name);
		pResult.Result = false;
	}

	if (!::gObjFixEventInventoryPointer(aIndex))
		sLog->outBasic("[Fix EventInv.Ptr] False Location - %s, %d", __FILE__, __LINE__);

	if (lpObj.pTransaction == 1)
	{
		sLog->outBasic("[%s][%s] CGEventItemDropRequest() Failed : Transaction == 1, IF_TYPE : %d",
			lpObj.AccountID, lpObj.Name, lpObj.m_IfState.type);
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

	if (!gObjCanItemTouch(lpObj, 0))
		pResult.Result = false;

	if (!IsDumpItem(lpObj.pEventInventory[lpMsg->Ipos].m_Type))
		pResult.Result = false;

	LPITEM_ATTRIBUTE pItemAttribute = GetItemAttr(lpObj.pEventInventory[lpMsg->Ipos].m_Type);

	if (!pItemAttribute)
	{
		return false;
	}

	if (pItemAttribute->ItemKindA != 11)
		pResult.Result = false;

	if (pResult.Result == 1)
	{
		::ItemIsBufExOption(NewOption, &lpObj.pEventInventory[lpMsg->Ipos]);

		int map = lpObj.MapNumber;
		int type = lpObj.pEventInventory[lpMsg->Ipos].m_Type;
		int level = lpObj.pEventInventory[lpMsg->Ipos].m_Level;
		float dur = lpObj.pEventInventory[lpMsg->Ipos].m_Durability;
		BOOL bIsItem = lpObj.pEventInventory[lpMsg->Ipos].IsItem();
		BYTE Option1 = lpObj.pEventInventory[lpMsg->Ipos].m_Option1;
		BYTE Option2 = lpObj.pEventInventory[lpMsg->Ipos].m_Option2;
		BYTE Option3 = lpObj.pEventInventory[lpMsg->Ipos].m_Option3;
		BYTE NOption = lpObj.pEventInventory[lpMsg->Ipos].m_NewOption;
		UINT64 serial = lpObj.pEventInventory[lpMsg->Ipos].m_Number;
		int PetLevel = lpObj.pEventInventory[lpMsg->Ipos].m_PetItem_Level;
		UINT64 PetExp = lpObj.pEventInventory[lpMsg->Ipos].m_PetItem_Exp;
		BYTE SOption = lpObj.pEventInventory[lpMsg->Ipos].m_SetOption;
		BYTE btItemEffectEx = lpObj.pEventInventory[lpMsg->Ipos].m_ItemOptionEx;
		UINT64 serial2 = lpObj.pEventInventory[lpMsg->Ipos].m_Number;
		char szItemName[50] = "Item";
		BYTE JOHOption = g_kJewelOfHarmonySystem.GetItemStrengthenOption(&lpObj.pEventInventory[lpMsg->Ipos]);
		BYTE JOHOptionLevel = g_kJewelOfHarmonySystem.GetItemOptionLevel(&lpObj.pEventInventory[lpMsg->Ipos]);

		//Season 4 add-on
		BYTE SocketOption[5];
		BYTE SocketIndex;
		g_SocketOptionSystem.GetSocketOption(&lpObj.pEventInventory[lpMsg->Ipos], SocketOption, SocketIndex);

		for (int iOption = 0; iOption < 5; iOption++)
		{
			SocketOption[iOption] = lpObj.pEventInventory[lpMsg->Ipos].m_SocketOption[iOption];
		}

		if (gObjCheckSerial0ItemList(&lpObj.pEventInventory[lpMsg->Ipos]) != FALSE)
		{
			MsgOutput(lpObj.m_Index, Lang.GetText(0, 259));
			sLog->outBasic("[ANTI-HACK][Serial 0 Item] [ItemDrop] (%s)(%s) Item(%s) Pos(%d)",
				lpObj.AccountID, lpObj.Name, lpObj.pEventInventory[lpMsg->Ipos].GetName(),
				lpMsg->Ipos);

			CopyItem = TRUE;
			pResult.Result = false;
			bIsItem = FALSE;
		}

		if (!::gObjEventInventorySearchSerialNum(lpObj, serial2))
		{
			CopyItem = TRUE;
			pResult.Result = false;
			bIsItem = FALSE;
		}

		if (bIsItem == TRUE)
		{
			strcpy(szItemName, lpObj.pEventInventory[lpMsg->Ipos].GetName());

			if (g_BagManager.SearchAndUseBag(lpObj.m_Index, BAG_COMMON, type, level) == true)
			{
				gObjEventInventoryDeleteItem(aIndex, lpMsg->Ipos);
			}
			else if (type == ITEMGET(13, 210) && level == 0)
			{
				gObjEventInventoryDeleteItem(aIndex, lpMsg->Ipos);
				g_BagManager.SearchAndUseBag(aIndex, BAG_EVENT, EVENTBAG_EGG_PETITEM, aIndex);
			}
			else if (type == ITEMGET(13, 220) && level == 0)
			{
				gObjEventInventoryDeleteItem(aIndex, lpMsg->Ipos);
				g_BagManager.SearchAndUseBag(aIndex, BAG_EVENT, EVENTBAG_EGG_LUCKYBAG, aIndex);

			}

			else if (type == ITEMGET(13, 221) && level == 0)
			{
				gObjEventInventoryDeleteItem(aIndex, lpMsg->Ipos);
				g_BagManager.SearchAndUseBag(aIndex, BAG_EVENT, EVENTBAG_EGG_FIREGHOST, aIndex);

			}

			else if (type == ITEMGET(13, 222) && level == 0)
			{
				gObjEventInventoryDeleteItem(aIndex, lpMsg->Ipos);
				g_BagManager.SearchAndUseBag(aIndex, BAG_EVENT, EVENTBAG_EGG_GOLDGOBLIN, aIndex);

			}

			else if (type == ITEMGET(14, 244))
			{
				if (g_EvoMonMng.UseSummonScroll(aIndex) == FALSE)
				{
					pResult.Result = false;
					IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}

				gObjEventInventoryDeleteItem(aIndex, lpMsg->Ipos);
			}

			else
			{
				if (MapC[map].ItemDrop(type, level, dur, lpMsg->px, lpMsg->py, Option1, Option2, Option3,
					NOption, SOption, serial2, aIndex, PetLevel, PetExp, btItemEffectEx, SocketOption, SocketIndex, 0) == TRUE)
				{
					gObjEventInventoryDeleteItem(aIndex, lpMsg->Ipos);
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

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pResult, pResult.h.size);

	if (CopyItem != FALSE)
	{
		this->GCEventInventoryItemListSend(aIndex);
	}
}

void GameProtocol::GCEventInventoryItemListSend(CGameObject &Obj)
{
	PMSG_INVENTORYLISTCOUNT pMsgILC;
	PMSG_INVENTORYLIST pMsgIL;
	int sOfs = sizeof(PMSG_INVENTORYLISTCOUNT);
	int pMsgILSize = sizeof(PMSG_INVENTORYLIST);
	BYTE sendBuf[4096];
	
	int itemcount = 0;

	for (int n = 0; n < EVENT_INVENTORY_SIZE; n++)
	{
		if (lpObj.pEventInventory[n].IsItem() == TRUE)
		{
			if (lpObj.pEventInventory[n].m_bItemExist == false)
			{
				continue;
			}

			pMsgIL.Pos = n;
			ItemByteConvert(pMsgIL.ItemInfo, lpObj.pEventInventory[n]);
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

	IOCP.DataSend(lpObj.m_Index, sendBuf, sOfs);
}

void GameProtocol::GCEventItemDurSend(CGameObject &Obj, BYTE pos, BYTE dur)
{
	PMSG_EVENTITEM_DUR pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0x4D, 0x03, sizeof(pMsg));
	pMsg.IPos = pos;
	pMsg.Dur = dur;

	IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCEventInvenItemOneSend(CGameObject &Obj, int pos)
{
	if (!lpObj.pEventInventory[pos].IsItem())
		return;

	PMSG_EVENT_INVEN_ITEM_MOD pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0x4D, 0x05, sizeof(pMsg));
	pMsg.Pos = pos;
	ItemByteConvert(pMsg.ItemInfo, lpObj.pEventInventory[pos]);

	IOCP.DataSend(lpObj.m_Index, (UCHAR *)&pMsg, pMsg.h.size);
}

void GameProtocol::GCEventInventoryItemDeleteSend(CGameObject &Obj, BYTE pos, BYTE flag)
{
	PMSG_EVENTINVENTORY_DELETE pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0x4D, 0x04, sizeof(pMsg));
	pMsg.IPos = pos;
	pMsg.Flag = flag;

	IOCP.DataSend(lpObj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCResSendExpEventInfo(CGameObject &Obj)
{
	PMSG_RES_EXPEVENTINFO pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xBF, 0x52, sizeof(pMsg));

	pMsg.wPCBangRate = 0;
	pMsg.wExpRate = g_ExpManager.GetEventExp() / g_ExpManager.GetStaticExp() * 100.0;

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::CGReq_ITL_Relics(PMSG_REQ_ITL_RELATE_RELICS *lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	if (!gObjIsConnected(Obj.m_Index))
	{
		return;
	}

	if (!lpMsg)
	{
		return;
	}

	WORD NpcNumber = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	if (NpcNumber >= g_ConfigRead.server.GetObjectMax())
	{
		return;
	}

	CGameObject lpObj = Obj;

	if (!lpObj)
	{
		return;
	}

	if (lpObj.MapNumber != getGameObject(NpcNumber)->MapNumber)
	{
		return;
	}

	sLog->outBasic("[ ITL ][ ITR ][ActRelicsGetOrRegister] NpcIndex:%d,byPrePacket:%d,byAct:%d,wNpcType:%d",
		NpcNumber, lpMsg->byPrePacket, lpMsg->byAct, lpMsg->wNpcType);

	if (IT_MAP_RANGE(lpObj.MapNumber))
	{
		g_IT_Event.ActRelicsGetOrRegister(&getGameObject(NpcNumber), lpObj, lpObj.MapNumber, lpMsg->byPrePacket, lpMsg->byAct, lpMsg->wNpcType);
	}
}

void GameProtocol::CG_Req_Enter_ITR(PMSG_REQ_ENTER_ITR *lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	_ITL_AnswerEnter pResult;
	PHeadSubSetB((BYTE*)&pResult, 0xBF, 0x70, sizeof(pResult));
	memset(pResult.Name, 0x00, sizeof(pResult.Name));

	if (g_IT_Event.IsCanEnterTiming() == false)
	{
		pResult.bReason = 8;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (Obj.PartyNumber < 0)
	{
		pResult.bReason = 1;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (gParty.Isleader(Obj.PartyNumber, iIndex, Obj.DBNumber) == false)
	{
		pResult.bReason = 3;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (gParty.GetCount(Obj.PartyNumber) < g_IT_Event.m_iITR_MinUserInParty)
	{
		pResult.bReason = 4;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (gParty.GetPKPartyPenalty(Obj.PartyNumber) >= 5)
	{
		pResult.bReason = 12;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	int PartyUsersIndex = 0;
	BOOL bResult = this->EnterITRCheckUser(iIndex, PartyUsersIndex);

	if (bResult != 0)
	{
		pResult.bReason = bResult;
		memcpy(pResult.Name, getGameObject(PartyUsersIndex)->Name, MAX_ACCOUNT_LEN + 1);
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);

		if (iIndex != PartyUsersIndex)
		{
			IOCP.DataSend(PartyUsersIndex, (BYTE*)&pResult, pResult.h.size);
		}

		return;
	}

	BYTE bySlotNum = -1;
	int nEnterIllusionIndex = g_IT_Event.CheckEnterLevel(Obj.m_Index);
	BYTE byMapNumber = g_IT_Event.Find_EmptySlot(Obj.PartyNumber, bySlotNum, nEnterIllusionIndex);

	if (byMapNumber > 6)
	{
		pResult.bReason = 14;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	BOOL bUpdateOk = g_IT_Event.Update_PartyInfo(byMapNumber, bySlotNum, Obj.PartyNumber, iIndex);

	if (bUpdateOk != 0)
	{
		sLog->outBasic("[ITR][Update_PartyInfo] ERRORcode:%d,NAME:%s,partyindex:%d",
			bUpdateOk, Obj.Name, Obj.PartyNumber);

		pResult.bReason = -1;
		memcpy(pResult.Name, Obj.Name, MAX_ACCOUNT_LEN + 1);
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	int dbnumber = 0;
	int useridx;

	for (int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		if (gParty.GetIndexUser(Obj.PartyNumber, i, useridx, dbnumber) == FALSE)
		{
			pResult.bReason = 13;
			IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}

		if (useridx > 0)
		{
			pResult.bReason = 0;
			memcpy(pResult.Name, Obj.Name, MAX_ACCOUNT_LEN + 1);
			IOCP.DataSend(useridx, (BYTE*)&pResult, pResult.h.size);
		}
	}
}

void GameProtocol::CG_ReqAcceptEnterITR(PMSG_REQ_ACCEPTENTER_ITR *lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	CGameObject lpObj = Obj;

	if (!lpObj)
	{
		return;
	}

	if (!gObjIsConnected(lpObj))
	{
		return;
	}

	_ITR_AcceptEnter pResult;
	PHeadSubSetB((BYTE*)&pResult, 0xBF, 0x71, sizeof(pResult));
	memset(pResult.Name, 0x00, sizeof(pResult.Name));

	int byOkCount = 0;
	int UserIndex;
	int dbnumber;
	BYTE byEnterOK = lpMsg->byEnterOK;

	if (gParty.IsParty(lpObj.PartyNumber) == FALSE)
	{
		sLog->outBasic("[ ITR ][CGReqAcceptEnterITR] reqman is not party Error Name :%s,", lpObj.Name);
		g_IT_Event.Delete_PartyInfo(lpObj.PartyNumber);
		gParty.ClearPartyAuthITR(lpObj.PartyNumber);
		return;
	}

	if (byEnterOK == TRUE)
	{
		sLog->outBasic("[ ITR ][CGReqAcceptEnterITR] reqman opposite Name :%s,", lpObj.Name);
		g_IT_Event.Delete_PartyInfo(lpObj.PartyNumber);
		gParty.ClearPartyAuthITR(lpObj.PartyNumber);

		pResult.byError = -1;
		memcpy(pResult.Name, lpObj.Name, MAX_ACCOUNT_LEN + 1);
		IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (gParty.EnterITR_PartyAuth(lpObj.PartyNumber, lpObj.m_Index) == FALSE)
	{
		sLog->outBasic("[ ITR ][CGReqAcceptEnterITR][EnterITR_PartyAuth] Internal Error Name :%s", lpObj.Name);
		g_IT_Event.Delete_PartyInfo(lpObj.PartyNumber);
		gParty.ClearPartyAuthITR(lpObj.PartyNumber);

		pResult.byError = -1;
		memcpy(pResult.Name, lpObj.Name, MAX_ACCOUNT_LEN + 1);
		IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (gParty.AllAgreeEnterITR(lpObj.PartyNumber) == FALSE)
	{
		sLog->outBasic("[ ITR ] [CGReqAcceptEnterITR] Enter Agree NAME:%s, G.NAME:%s,AgreeCnt:%d",
			lpObj.Name, lpObj.m_PlayerData->GuildName, gParty.ReturnOKCountITR(lpObj.PartyNumber));
		return;
	}

	int TroubleUserIndex = -1;
	BOOL bResult = EnterITRCheckUser(iIndex, TroubleUserIndex);

	if (bResult != 0)
	{
		pResult.byError = bResult;
		memcpy(pResult.Name, getGameObject(TroubleUserIndex)->Name, MAX_ACCOUNT_LEN + 1);
		int nAllPartyUserIndex;

		for (int j = 0; j < MAX_USER_IN_PARTY; j++)
		{
			gParty.GetIndexUser(lpObj.PartyNumber, j, nAllPartyUserIndex, dbnumber);
			IOCP.DataSend(nAllPartyUserIndex, (BYTE*)&pResult, pResult.h.size);
		}

		g_IT_Event.Delete_PartyInfo(lpObj.PartyNumber);
		gParty.ClearPartyAuthITR(lpObj.PartyNumber);
		return;
	}

	for (int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		gParty.GetIndexUser(lpObj.PartyNumber, i, UserIndex, dbnumber);

		if (UserIndex != -1)
		{
			if (gParty.m_PartyS[lpObj.PartyNumber].bITRWithYouOk[i] == FALSE)
			{
				continue;
			}

			BYTE byITRMapIndex = -1;
			BYTE bySlotNum = -1;
			BOOL bReturn = g_IT_Event.Find_ExistingPartyInfo(lpObj.PartyNumber, byITRMapIndex, bySlotNum);

			if (bReturn)
			{
				sLog->outBasic("[ ITR ] [Find_ExistingPartyInfo]ERROR return:%d Name:%s, mapnumber:%d, ITLMapIndex:%d,bySlotNum:%d ",
					bReturn, lpObj.Name, lpObj.MapNumber, byITRMapIndex, bySlotNum);

				return;
			}

			if (g_IT_Event.Enter_ITR(UserIndex, byITRMapIndex, bySlotNum) == TRUE)
			{
				pResult.byError = 0;
				memcpy(pResult.Name, getGameObject(UserIndex)->Name, MAX_ACCOUNT_LEN + 1);
				IOCP.DataSend(UserIndex, (BYTE*)&pResult, pResult.h.size);

				getGameObject(UserIndex)->MapNumber = byITRMapIndex + 45;
				gObjMoveGate(UserIndex, byITRMapIndex + 142);
			}
		}
	}
}

BYTE GameProtocol::EnterITRCheckUser(CGameObject &Obj, int &TroubleUserIndex)
{
	int PartyLeaderITRIndex = g_IT_Event.CheckEnterLevel(Obj.m_Index);

	if (PartyLeaderITRIndex == -1)
	{
		return 5;
	}

	int useridx = 0;
	int dbnumber = 0;

	for (int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		BYTE bReturn = gParty.GetIndexUser(Obj.PartyNumber, i, useridx, dbnumber);
		TroubleUserIndex = useridx;

		if (bReturn == FALSE)
		{
			return 13;
		}

		if (useridx > 0)
		{
			if (getGameObject(useridx)->MapNumber != MAP_INDEX_LORENMARKET &&
				IT_MAP_RANGE(getGameObject(useridx)->MapNumber) == FALSE)
			{
				return 10;
			}

			if (getGameObject(useridx)->PartyNumber != Obj.PartyNumber)
			{
				return 13;
			}

			bReturn = g_IT_Event.CheckEnterLevel(useridx);

			if (PartyLeaderITRIndex != bReturn)
			{
				return 5;
			}

			if (g_IT_Event.CheckWearingMOPH(useridx) == TRUE)
			{
				return 7;
			}

			if (getGameObject(useridx]->m_IfState.use && getGameObject(useridx)->m_IfState.type != 12)
			{
				return 11;
			}

			if (getGameObject(useridx)->m_PK_Level >= 4)
			{
				return 12;
			}

			if (gObjDuelCheck(&getGameObject(useridx)) == TRUE)
			{
				return 15;
			}

			if (getGameObject(useridx)->m_PlayerData->lpGuild != NULL &&
				getGameObject(useridx)->m_PlayerData->lpGuild->WarState == TRUE)
			{
				return 16;
			}

			if (getGameObject(useridx]->pInventory[getGameObject(useridx)->m_btInvenPetPos)->m_Type == ITEMGET(13, 3) ||
				getGameObject(useridx]->pInventory[getGameObject(useridx)->m_btInvenPetPos)->m_Type == ITEMGET(13, 2) ||
				getGameObject(useridx]->pInventory[getGameObject(useridx)->m_btInvenPetPos)->m_Type == ITEMGET(13, 37))
			{
				return 17;
			}
		}
	}

	return 0;
}

void GameProtocol::GCSendRareItemPriceInfo(CGameObject &Obj)
{
	PMSG_RAREITEMPIRCESEND pMsg;
	pMsg.h.set((BYTE*)&pMsg, 0xFA, 0xA4, sizeof(pMsg));

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

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, sizeof(pMsg));
}

void GameProtocol::GCSendDisableReconnect(CGameObject &Obj)
{
	PMSG_DISABLE_RECONNECT pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0xFA, sizeof(pMsg));
	pMsg.subcode = 0xA5;

	for (int i = 0; i < 100; i++)
	{
		pMsg.Trash[i] = rand() % 0xFF;
	}

	lpObj.m_dwDCTimer = GetTickCount64();

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, sizeof(pMsg));

}
void GameProtocol::GCSendDisableReconnectSystem(CGameObject &Obj)
{
	PMSG_DISABLE_RECONNECT pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0xFA, sizeof(pMsg));
	pMsg.subcode = 0xA7;

	for (int i = 0; i < 100; i++)
	{
		pMsg.Trash[i] = rand() % 0xFF;
	}

	//lpObj.m_dwDCTimer = GetTickCount64();

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, sizeof(pMsg));

}
void GameProtocol::CGReqUBFMyCharacterInfo(PMSG_REQ_UBF_INFO *lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (lpMsg->btUBFKind == 0)
	{
		g_UnityBattleField.GDReqCheckJoinedUnityBattleField(iIndex, g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE, 0);
	}
}

void GameProtocol::CGReqUBFJoin(PMSG_REQ_UBF_JOIN *lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (lpMsg->btUBFKind == 0)
	{
		g_UnityBattleField.GDReqJoinUnityBattleField(Obj.m_Index);
	}
}

void GameProtocol::CGReqUBFCancel(PMSG_REQ_UBF_CANCEL *lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (lpMsg->btCanceled == 1)
	{
		g_UnityBattleField.GDReqCancelUnityBattleField(iIndex, 1);
	}
}

void GameProtocol::CGReqUBFGetRealName(PMSG_REQ_UBF_REAL_NAME *lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (lpMsg->btRequest == 1)
	{
		g_UnityBattleField.GDReqGetRealNameAndServerCode(Obj.m_Index);
	}
}

void GameProtocol::CGReqUBFGetReward(PMSG_REQ_GET_REWARD *lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	g_UnityBattleField.GDReqUBFGetReward(iIndex, lpMsg->btBattleKind);
}

void GameProtocol::CGReqDSFSchedule(CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (!gObjIsConnected(Obj.m_Index))
	{
		return;
	}

	if (g_ConfigRead.server.GetServerType() != SERVER_BATTLECORE)
	{
		return;
	}

	if (Obj.MapNumber != MAP_INDEX_LORENMARKET)
	{
		return;
	}

	g_DevilSquareFinal.ReqDSFSchedule(Obj.m_Index);
}

void GameProtocol::CGReqDSFEnter(CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	PMSG_ANS_DSF_ENTER pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xDB, 0x01, sizeof(pMsg));

	memset(pMsg.Name, 0x00, sizeof(pMsg.Name));
	pMsg.btDSFType = g_DevilSquareFinal.m_nDSF_TYPE;

	if (g_DevilSquareFinal.m_btDSFCurState != 0)
	{
		pMsg.btResult = 8;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (g_DevilSquareFinal.m_bEntrance == false)
	{
		pMsg.btResult = 8;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (gParty.GetLorenMarketUserCount(Obj.PartyNumber) != 2)
	{
		sLog->outBasic("[DSF][CGReqDSFEnter][%s][%s][%s][%d] EnterError : Party User is not in RorenMarket",
			Obj.AccountID, Obj.m_PlayerData->m_RealNameOfUBF, Obj.Name, Obj.m_PlayerData->m_nServerCodeOfHomeWorld);

		pMsg.btResult = 10;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (gParty.GetPKPartyPenalty(Obj.PartyNumber) >= 5)
	{
		pMsg.btResult = 12;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	int PartyUsersIndex = 0;
	BOOL bResult = this->EnterDSFCheckUser(iIndex, PartyUsersIndex);

	if (bResult != 0)
	{
		pMsg.btResult = bResult;
		memcpy(pMsg.Name, getGameObject(PartyUsersIndex)->Name, MAX_ACCOUNT_LEN + 1);
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

		if (PartyUsersIndex != iIndex)
		{
			IOCP.DataSend(PartyUsersIndex, (BYTE*)&pMsg, pMsg.h.size);
		}

		return;
	}

	BOOL bExistAlready = g_DevilSquareFinal.IsAlreadyExistUserInDSF(Obj.m_Index);

	if (bExistAlready != FALSE)
	{
		g_DevilSquareFinal.DeletePartyByIndex(Obj.m_Index);
	}

	if (g_DevilSquareFinal.m_nDSF_TYPE == 1)
	{
		BYTE btSlotNum = -1;
		BYTE btRet = g_DevilSquareFinal.Find_EmptyPartySlot(Obj.PartyNumber, btSlotNum);

		if (btRet >= 0xFE)
		{
			pMsg.btResult = 14;
			memcpy(pMsg.Name, Obj.Name, sizeof(pMsg.Name));
			IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

			return;
		}

		BOOL bUpdateOk = g_DevilSquareFinal.SetParty(btSlotNum, 1, Obj.PartyNumber, iIndex);

		if (bUpdateOk != FALSE)
		{
			sLog->outBasic("[DSF][CGReqDSFEnter][%s][%s][%s][%d] ERROR code:%d",
				Obj.AccountID, Obj.m_PlayerData->m_RealNameOfUBF, Obj.Name, Obj.m_PlayerData->m_nServerCodeOfHomeWorld, bUpdateOk);

			pMsg.btResult = -1;
			memcpy(pMsg.Name, Obj.Name, sizeof(pMsg.Name));
			IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

			return;
		}

		int dbnumber = 0, useridx = -1;

		for (int i = 0; i < 2; i++)
		{
			BOOL bError = gParty.GetIndexUser(Obj.PartyNumber, i, useridx, dbnumber);

			if (bError == FALSE)
			{
				pMsg.btResult = 13;
				IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
				return;
			}

			if (useridx > 0)
			{
				pMsg.btResult = 0;
				memcpy(pMsg.Name, Obj.Name, sizeof(pMsg.Name));
				IOCP.DataSend(useridx, (BYTE*)&pMsg, pMsg.h.size);

				sLog->outBasic("[DSF][CGReqDSFEnter][%s][%s][%s][%d] Request DSF Enter Success And Ask To Party Member, DSFType : %d",
					Obj.AccountID, Obj.m_PlayerData->m_RealNameOfUBF,
					Obj.Name, Obj.m_PlayerData->m_nServerCodeOfHomeWorld,
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
			BOOL bError = gParty.GetIndexUser(Obj.PartyNumber, i, useridx, dbnumber);

			if (bError == FALSE)
			{
				pMsg.btResult = 13;
				IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
				return;
			}

			if (useridx > 0)
			{
				if (i == 0)
				{
					memcpy(szUserName1, getGameObject(useridx]->m_PlayerData->m_NameConvertOfUBF.ConvertNameToRealName(getGameObject(useridx)->Name), MAX_ACCOUNT_LEN + 1);
					wServerCode1 = getGameObject(useridx]->m_PlayerData->m_NameConvertOfUBF.FindServerCodeAtCharacterName(getGameObject(useridx)->Name);
				}

				else if (i == 1)
				{
					memcpy(szUserName2, getGameObject(useridx]->m_PlayerData->m_NameConvertOfUBF.ConvertNameToRealName(getGameObject(useridx)->Name), MAX_ACCOUNT_LEN + 1);
					wServerCode2 = getGameObject(useridx]->m_PlayerData->m_NameConvertOfUBF.FindServerCodeAtCharacterName(getGameObject(useridx)->Name);
				}
			}
		}

		BOOL bGoFinal = g_DevilSquareFinal.IsGoSemiFinalParty(szUserName1, szUserName2, wServerCode1, wServerCode2);

		if (bGoFinal == FALSE)
		{
			pMsg.btResult = 18;
			memcpy(pMsg.Name, Obj.Name, sizeof(pMsg.Name));
			IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

			return;
		}

		BYTE btSlotNum = -1;
		BYTE btRet = g_DevilSquareFinal.Find_EmptyPartySlot(Obj.PartyNumber, btSlotNum);

		if (btRet >= 0xFE)
		{
			pMsg.btResult = 14;
			memcpy(pMsg.Name, Obj.Name, sizeof(pMsg.Name));
			IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

			return;
		}

		BOOL bUpdateOk = g_DevilSquareFinal.SetParty(btSlotNum, 2, Obj.PartyNumber, iIndex);

		if (bUpdateOk != FALSE)
		{
			sLog->outBasic("[DSF][CGReqDSFEnter][%s][%s][%s][%d] ERROR code:%d",
				Obj.AccountID, Obj.m_PlayerData->m_RealNameOfUBF, Obj.Name, Obj.m_PlayerData->m_nServerCodeOfHomeWorld, bUpdateOk);

			pMsg.btResult = -1;
			memcpy(pMsg.Name, Obj.Name, sizeof(pMsg.Name));
			IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

			return;
		}

		for (int i = 0; i < 2; i++)
		{
			BOOL bError = gParty.GetIndexUser(Obj.PartyNumber, i, useridx, dbnumber);

			if (bError == FALSE)
			{
				pMsg.btResult = 13;
				IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
				return;
			}

			if (useridx > 0)
			{
				pMsg.btResult = 0;
				memcpy(pMsg.Name, Obj.Name, sizeof(pMsg.Name));
				IOCP.DataSend(useridx, (BYTE*)&pMsg, pMsg.h.size);

				sLog->outBasic("[DSF][CGReqDSFEnter][%s][%s][%s][%d] Request DSF Enter Success And Ask To Party Member, DSFType : %d",
					Obj.AccountID, Obj.m_PlayerData->m_RealNameOfUBF,
					Obj.Name, Obj.m_PlayerData->m_nServerCodeOfHomeWorld,
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
			BOOL bError = gParty.GetIndexUser(Obj.PartyNumber, i, useridx, dbnumber);

			if (bError == FALSE)
			{
				pMsg.btResult = 13;
				IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
				return;
			}

			if (useridx > 0)
			{
				if (i == 0)
				{
					memcpy(szUserName1, getGameObject(useridx]->m_PlayerData->m_NameConvertOfUBF.ConvertNameToRealName(getGameObject(useridx)->Name), MAX_ACCOUNT_LEN + 1);
					wServerCode1 = getGameObject(useridx]->m_PlayerData->m_NameConvertOfUBF.FindServerCodeAtCharacterName(getGameObject(useridx)->Name);
				}

				else if (i == 1)
				{
					memcpy(szUserName2, getGameObject(useridx]->m_PlayerData->m_NameConvertOfUBF.ConvertNameToRealName(getGameObject(useridx)->Name), MAX_ACCOUNT_LEN + 1);
					wServerCode2 = getGameObject(useridx]->m_PlayerData->m_NameConvertOfUBF.FindServerCodeAtCharacterName(getGameObject(useridx)->Name);
				}
			}
		}

		BOOL bGoFinal = g_DevilSquareFinal.IsGoFinalParty(szUserName1, szUserName2, wServerCode1, wServerCode2);

		if (bGoFinal == FALSE)
		{
			pMsg.btResult = 18;
			memcpy(pMsg.Name, Obj.Name, sizeof(pMsg.Name));
			IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

			return;
		}

		BYTE btSlotNum = -1;
		BYTE btRet = g_DevilSquareFinal.Find_EmptyPartySlot(Obj.PartyNumber, btSlotNum);

		if (btRet >= 0xFE)
		{
			pMsg.btResult = 14;
			memcpy(pMsg.Name, Obj.Name, sizeof(pMsg.Name));
			IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

			return;
		}

		BOOL bUpdateOk = g_DevilSquareFinal.SetParty(btSlotNum, 3, Obj.PartyNumber, iIndex);

		if (bUpdateOk != FALSE)
		{
			sLog->outBasic("[DSF][CGReqDSFEnter][%s][%s][%s][%d] ERROR code:%d",
				Obj.AccountID, Obj.m_PlayerData->m_RealNameOfUBF, Obj.Name, Obj.m_PlayerData->m_nServerCodeOfHomeWorld, bUpdateOk);

			pMsg.btResult = -1;
			memcpy(pMsg.Name, Obj.Name, sizeof(pMsg.Name));
			IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

			return;
		}

		for (int i = 0; i < 2; i++)
		{
			BOOL bError = gParty.GetIndexUser(Obj.PartyNumber, i, useridx, dbnumber);

			if (bError == FALSE)
			{
				pMsg.btResult = 13;
				IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
				return;
			}

			if (useridx > 0)
			{
				pMsg.btResult = 0;
				memcpy(pMsg.Name, Obj.Name, sizeof(pMsg.Name));
				IOCP.DataSend(useridx, (BYTE*)&pMsg, pMsg.h.size);

				sLog->outBasic("[DSF][CGReqDSFEnter][%s][%s][%s][%d] Request DSF Enter Success And Ask To Party Member, DSFType : %d",
					Obj.AccountID, Obj.m_PlayerData->m_RealNameOfUBF,
					Obj.Name, Obj.m_PlayerData->m_nServerCodeOfHomeWorld,
					g_DevilSquareFinal.m_nDSF_TYPE);
			}
		}
	}
}

int GameProtocol::EnterDSFCheckUser(CGameObject &Obj, int & TroubleUserIndex)
{
	int useridx = -1;
	int dbnumber = 0;
	int nLorenMarketUserCount = 0;

	for (int i = 0; i < 2; i++)
	{
		BOOL bError = gParty.GetIndexUser(Obj.PartyNumber, i, useridx, dbnumber);
		TroubleUserIndex = useridx;

		if (bError == FALSE)
		{
			return 13;
		}

		if (useridx > 0)
		{
			if (getGameObject(useridx]->MapNumber == MAP_INDEX_LORENMARKET || getGameObject(useridx)->MapNumber == MAP_INDEX_DEVILSQUARE_FINAL)
			{
				nLorenMarketUserCount++;
			}

			if (getGameObject(useridx)->PartyNumber != Obj.PartyNumber)
			{
				return 13;
			}

			if (getGameObject(useridx)->Level < g_DevilSquareFinal.m_DSFUnitInfo.DSFMinLevel)
			{
				return 5;
			}

			if (g_DevilSquareFinal.CheckWearingMOPH(useridx) == TRUE)
			{
				return 7;
			}

			if (getGameObject(useridx]->m_IfState.use && getGameObject(useridx)->m_IfState.type != 12)
			{
				return 11;
			}

			if (getGameObject(useridx)->m_PK_Level >= 4)
			{
				return 12;
			}

			if (gObjDuelCheck(&getGameObject(useridx)) == TRUE)
			{
				return 15;
			}

			if (getGameObject(useridx)->m_PlayerData->lpGuild != NULL &&
				getGameObject(useridx)->m_PlayerData->lpGuild->WarState == TRUE)
			{
				return 16;
			}
		}
	}

	return 0;
}

void GameProtocol::CGReqAcceptEnterDSF(PMSG_REQ_DSF_ACCEPTENTER *lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (!gObjIsConnected(Obj))
	{
		return;
	}

	CGameObject lpObj = Obj;

	PMSG_ANS_DSF_ACCEPTENTER pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xDB, 0x02, sizeof(pMsg));
	memset(pMsg.Name, 0x00, sizeof(pMsg.Name));

	int useridx = -1, dbnumber = 0;

	if (gParty.IsParty(lpObj.PartyNumber) == FALSE)
	{
		sLog->outBasic("[DSF][CGReqAcceptEnterDSF][%s][%s][%s][%d] reqman is not party Error",
			lpObj.AccountID, lpObj.m_PlayerData->m_RealNameOfUBF,
			lpObj.Name, lpObj.m_PlayerData->m_nServerCodeOfHomeWorld);

		g_DevilSquareFinal.DeleteParty(lpObj.PartyNumber);
		gParty.ClearPartyAuthDSF(lpObj.PartyNumber);
		return;
	}

	if (lpMsg->btEnterOK == TRUE)
	{
		sLog->outBasic("[DSF][CGReqAcceptEnterDSF][%s][%s][%s][%d] Enter Cancel...",
			lpObj.AccountID, lpObj.m_PlayerData->m_RealNameOfUBF,
			lpObj.Name, lpObj.m_PlayerData->m_nServerCodeOfHomeWorld);

		g_DevilSquareFinal.DeleteParty(lpObj.PartyNumber);
		gParty.ClearPartyAuthDSF(lpObj.PartyNumber);

		pMsg.btError = 24;
		memcpy(pMsg.Name, lpObj.Name, MAX_ACCOUNT_LEN + 1);

		for (int i = 0; i < 2; i++)
		{
			gParty.GetIndexUser(lpObj.PartyNumber, i, useridx, dbnumber);

			if (useridx > 0)
			{
				IOCP.DataSend(useridx, (BYTE*)&pMsg, pMsg.h.size);
			}
		}

		return;
	}

	sLog->outBasic("[DSF][CGReqAcceptEnterDSF][%s][%s][%s][%d] Enter OK...",
		lpObj.AccountID, lpObj.m_PlayerData->m_RealNameOfUBF,
		lpObj.Name, lpObj.m_PlayerData->m_nServerCodeOfHomeWorld);

	if (gParty.EnterDSF_PartyAuth(lpObj.PartyNumber, iIndex) == FALSE)
	{
		sLog->outBasic("[DSF][CGReqAcceptEnterDSF][%s][%s][%s][%d] Internal Error!!!",
			lpObj.AccountID, lpObj.m_PlayerData->m_RealNameOfUBF,
			lpObj.Name, lpObj.m_PlayerData->m_nServerCodeOfHomeWorld);

		pMsg.btError = -1;
		memcpy(pMsg.Name, lpObj.Name, sizeof(pMsg.Name));
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (gParty.AllAgreeEnterDSF(lpObj.PartyNumber) == FALSE)
	{
		int AgreeCnt = gParty.DSFReturnOKCount(lpObj.PartyNumber);

		sLog->outBasic("[DSF[CGReqAcceptEnterDSF][%s][%s][%s][%d] Party Enter Agree AgreeCnt:%d",
			lpObj.AccountID, lpObj.m_PlayerData->m_RealNameOfUBF,
			lpObj.Name, lpObj.m_PlayerData->m_nServerCodeOfHomeWorld, AgreeCnt);

		return;
	}

	int TroubleUserIndex = -1;
	BOOL bResult = this->EnterDSFCheckUser(iIndex, TroubleUserIndex);

	if (bResult != FALSE)
	{
		pMsg.btError = bResult;
		memcpy(pMsg.Name, getGameObject(TroubleUserIndex)->Name, sizeof(pMsg.Name));

		int nAllPartyUserIndex;

		for (int i = 0; i < 2; i++)
		{
			gParty.GetIndexUser(lpObj.PartyNumber, i, nAllPartyUserIndex, dbnumber);
			IOCP.DataSend(nAllPartyUserIndex, (BYTE*)&pMsg, pMsg.h.size);
		}

		g_DevilSquareFinal.DeleteParty(lpObj.PartyNumber);
		gParty.ClearPartyAuthDSF(lpObj.PartyNumber);
		return;
	}

	for (int i = 0; i < 2; i++)
	{
		gParty.GetIndexUser(lpObj.PartyNumber, i, useridx, dbnumber);

		if (useridx != -1)
		{
			BYTE btSlotNum = -1;
			BOOL bReturn = g_DevilSquareFinal.Find_PartySlot(lpObj.PartyNumber, btSlotNum);

			if (bReturn != FALSE)
			{
				sLog->outBasic("[DSF][Find_ExistingPartyInfo][%s][%s][%s][%d] ERROR bReturn:%d, mapnumber:%d, btSlotNum:%d ",
					lpObj.AccountID, lpObj.m_PlayerData->m_RealNameOfUBF, lpObj.Name, lpObj.m_PlayerData->m_nServerCodeOfHomeWorld,
					bReturn, lpObj.MapNumber, btSlotNum);

				return;
			}

			BOOL bEnterOK = g_DevilSquareFinal.Enter_DSF(useridx, btSlotNum);

			if (bEnterOK == FALSE)
			{
				g_DevilSquareFinal.DeleteParty(lpObj.PartyNumber);

				sLog->outBasic("[DSF][CGReqAcceptEnterDSF][%s][%s][%s][%d] Enter Error bReturn:%d, mapnumber:%d, btSlotNum:%d ",
					lpObj.AccountID, lpObj.m_PlayerData->m_RealNameOfUBF, lpObj.Name, lpObj.m_PlayerData->m_nServerCodeOfHomeWorld,
					bReturn, lpObj.MapNumber, btSlotNum);
			}

			else
			{
				pMsg.btError = 0;
				memcpy(pMsg.Name, getGameObject(useridx)->Name, sizeof(pMsg.Name));
				IOCP.DataSend(useridx, (BYTE*)&pMsg, pMsg.h.size);

				getGameObject(useridx)->MapNumber = MAP_INDEX_DEVILSQUARE_FINAL;
				gObjMoveGate(useridx, 474);

				sLog->outBasic("[DSF][CGReqAcceptEnterDSF][%s][%s][%s][%d] Enter DSF - PartyNo:%d, btSlotNum:%d ",
					getGameObject(useridx]->AccountID, getGameObject(useridx)->m_PlayerData->m_RealNameOfUBF,
					getGameObject(useridx]->Name, getGameObject(useridx)->m_PlayerData->m_nServerCodeOfHomeWorld,
					getGameObject(useridx)->PartyNumber, btSlotNum);
			}
		}
	}
}

void GameProtocol::CGReqDSFGoFinalParty(PMSG_REQ_DSF_GO_FINAL_PARTY_INFO *lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (!gObjIsConnected(Obj))
	{
		return;
	}

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

	PHeadSubSetW((BYTE*)&pMsg, 0xDB, 0x03, iSize);
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
	IOCP.DataSend(Obj.m_Index, sendBuf, iSize);

	sLog->outBasic("[DSF][CGReqDSFGoFinalParty][%s][%s][%s][%d] ReqType:%d, ResultCount:%d",
		Obj.AccountID, Obj.m_PlayerData->m_RealNameOfUBF,
		Obj.Name, Obj.m_PlayerData->m_nServerCodeOfHomeWorld,
		lpMsg->btDSFType, iCount);
}

void GameProtocol::CGReqDSFGetReward(PMSG_REQ_DSF_GET_REWARD *lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (!gObjIsConnected(Obj))
	{
		return;
	}

	if (g_ConfigRead.server.GetServerType() != SERVER_BATTLECORE)
	{
		return;
	}

	if (lpMsg->btDSFType == 4)
	{
		g_DevilSquareFinal.GDReqGetReward(Obj.m_Index);
	}
}

void GameProtocol::CGReqDoUsePopupType(CGameObject &Obj)
{
	if (!ObjectMaxRange(aIndex))
	{
		return;
	}

	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	if (!gObjIsConnected(&getGameObject(aIndex)))
	{
		return;
	}

	PMSG_ANS_USE_POPUP pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x26, sizeof(pMsg));

	pMsg.nPopupType = g_UnityBattleField.GetUBFPopupType();
	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCSendEventBanner(CGameObject &Obj, int iBannerType)
{
	PMSG_SEND_EVENT_BANNER pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0x4D, 0x18, sizeof(pMsg));

	pMsg.iBannerType = iBannerType;
	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCAlterPShopVault(CGameObject &Obj, BYTE btValue)
{
	PMSG_ALTERPSHOPVAULT pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xFA, 0xA8, sizeof(pMsg));
	pMsg.type = btValue;
	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}
void GameProtocol::GCSetCharColors(CGameObject &Obj)
{
	PMSG_SET_CHAT_COLOR pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xFA, 0x12, sizeof(pMsg));

	memcpy(pMsg.btInfoMsg, g_ConfigRead.data.chatcolor.btInfoMsg, sizeof(pMsg.btInfoMsg));
	memcpy(pMsg.btErrorMsg, g_ConfigRead.data.chatcolor.btErrorMsg, sizeof(pMsg.btErrorMsg));
	memcpy(pMsg.btChatMsg, g_ConfigRead.data.chatcolor.btChatMsg, sizeof(pMsg.btChatMsg));
	memcpy(pMsg.btWhisperMsg, g_ConfigRead.data.chatcolor.btWhisperMsg, sizeof(pMsg.btWhisperMsg));
	memcpy(pMsg.btPartyMsg, g_ConfigRead.data.chatcolor.btPartyMsg, sizeof(pMsg.btPartyMsg));
	memcpy(pMsg.btGuildMsg, g_ConfigRead.data.chatcolor.btGuildMsg, sizeof(pMsg.btGuildMsg));
	memcpy(pMsg.btAllianceMsg, g_ConfigRead.data.chatcolor.btAllianceMsg, sizeof(pMsg.btAllianceMsg));
	memcpy(pMsg.btGensMsg, g_ConfigRead.data.chatcolor.btGensMsg, sizeof(pMsg.btGensMsg));
	memcpy(pMsg.btGMChatMsg, g_ConfigRead.data.chatcolor.btGMChatMsg, sizeof(pMsg.btGMChatMsg));

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCSendRuud(CGameObject &Obj, int iRuud, int iObtainedRuud, bool bIsObtainedRuud)
{
	PMSG_RUUD pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xD0, 0xF1, sizeof(pMsg));

	pMsg.iTotalRuud = iRuud;
	pMsg.iObtainedRuud = iObtainedRuud;
	pMsg.btObtainNotiSend = bIsObtainedRuud;

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::CGReqUseBoxInInventory(CGameObject &Obj, PMSG_REQ_USE_BOX * aRecv)
{
	if (!ObjectMaxRange(aIndex))
	{
		return;
	}

	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	if (!gObjIsConnected(&getGameObject(aIndex)))
	{
		return;
	}

	if (lpObj.m_IfState.use != 0)
	{
		return;
	}

	if (lpObj.m_PlayerData->m_bSecurityCheck == false)
	{
		return;
	}

	if (lpObj.CloseType != -1)
	{
		return;
	}

	if (lpObj.m_bMapSvrMoveQuit == true)
	{
		sLog->outBasic("[ANTI-HACK][Item Duplication] - Use Box during MapServer Move [%s][%s]", lpObj.AccountID, lpObj.Name);
		return;
	}

	PMSG_ANS_USE_BOX pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xD0, 0xF2, sizeof(pMsg));

	if (CheckInventoryEmptySpace(&getGameObject(aIndex), 4, 4) == FALSE)
	{
		pMsg.iResult = -2;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (lpObj.m_PlayerData->Money >= 1900000000)
	{
		pMsg.iResult = -3;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	CItemObject Item;
	time_t Duration;
	int iResult = 0;
	int iBoxType = lpObj.pInventory[aRecv->btPos].m_Type;

	switch (iBoxType)
	{
	case ITEMGET(14, 282):
		iResult = g_BagManager.GetItemFromBag(aIndex, BAG_EVENT, EVENTBAG_MASTERYBOX_MINOR, aIndex, Item, Duration);
		break;
	case ITEMGET(14, 283):
		iResult = g_BagManager.GetItemFromBag(aIndex, BAG_EVENT, EVENTBAG_MASTERYBOX_STANDARD, aIndex, Item, Duration);
		break;
	case ITEMGET(14, 284):
		iResult = g_BagManager.GetItemFromBag(aIndex, BAG_EVENT, EVENTBAG_MASTERYBOX_GREATER, aIndex, Item, Duration);
		break;
	case ITEMGET(14, 287):
		iResult = g_BagManager.GetItemFromBag(aIndex, BAG_EVENT, EVENTBAG_MASTERYCHEST_2000, aIndex, Item, Duration);
		break;
	case ITEMGET(14, 288):
		iResult = g_BagManager.GetItemFromBag(aIndex, BAG_EVENT, EVENTBAG_MASTERYCHEST_1000, aIndex, Item, Duration);
		break;
	case ITEMGET(14, 289):
		iResult = g_BagManager.GetItemFromBag(aIndex, BAG_EVENT, EVENTBAG_MASTERYCHEST_500, aIndex, Item, Duration);
		break;
	case ITEMGET(14, 336):
		iResult = g_BagManager.GetItemFromBag(aIndex, BAG_EVENT, EVENTBAG_MASTERYCHEST_100, aIndex, Item, Duration);
		break;
	case ITEMGET(14, 293):
		iResult = g_BagManager.GetItemFromBag(aIndex, BAG_EVENT, EVENTBAG_ELEMENTALCAPSULE_NORMAL, aIndex, Item, Duration);
		break;
	case ITEMGET(14, 384):
		iResult = g_BagManager.GetItemFromBag(aIndex, BAG_EVENT, EVENTBAG_RUNECAPSULE, aIndex, Item, Duration);
		break;/*
	case ITEMGET(14, 367):
		iResult = g_BagManager.GetItemFromBag(aIndex, BAG_EVENT, EVENTBAG_ROOMY_CARD_LOWER, aIndex, Item, Duration);
		break;
	case ITEMGET(14, 368):
		iResult = g_BagManager.GetItemFromBag(aIndex, BAG_EVENT, EVENTBAG_ROOMY_CARD_MEDIUM, aIndex, Item, Duration);
		break;
	case ITEMGET(14, 369):
		iResult = g_BagManager.GetItemFromBag(aIndex, BAG_EVENT, EVENTBAG_ROOMY_CARD_HIGHER, aIndex, Item, Duration);
		break;
	case ITEMGET(14, 296):
		iResult = g_BagManager.GetItemFromBag(aIndex, BAG_EVENT, EVENTBAG_ELEMENTALCAPSULE_NORMAL, aIndex, Item, Duration);
		break;
	case ITEMGET(14, 338):
		iResult = g_BagManager.GetItemFromBag(aIndex, BAG_EVENT, EVENTBAG_RUNECAPSULE, aIndex, Item, Duration);
		break;
	case ITEMGET(14, 337):
		iResult = g_BagManager.GetItemFromBag(aIndex, BAG_EVENT, EVENTBAG_ROOMY_CARD_LOWER, aIndex, Item, Duration);
		break;
	case ITEMGET(14, 339):
		iResult = g_BagManager.GetItemFromBag(aIndex, BAG_EVENT, EVENTBAG_ROOMY_CARD_MEDIUM, aIndex, Item, Duration);
		break;
	case ITEMGET(14, 340):
		iResult = g_BagManager.GetItemFromBag(aIndex, BAG_EVENT, EVENTBAG_ROOMY_CARD_HIGHER, aIndex, Item, Duration);
		break;*/
	case ITEMGET(13, 350):
		iResult = g_BagManager.GetItemFromBag(aIndex, BAG_EVENT, EVENTBAG_MUUNEGG, aIndex, Item, Duration);
		break;		
	default:
		iResult = 0;
		break;
	}

	if (iResult == 0)
	{
		pMsg.iResult = -1;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	gObjInventoryDeleteItem(aIndex, aRecv->btPos);
	GameProtocol.GCInventoryItemDeleteSend(aIndex, aRecv->btPos, 0);

	if (iResult == 2 || iResult == 3)
	{
		pMsg.iResult = 0;
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	pMsg.iResult = 2;
	pMsg.iItemType = Item.m_Type;

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
	ItemSerialCreateSend(aIndex, 235, 0, 0, Item.m_Type, Item.m_Level, Item.m_Durability, Item.m_Option1, Item.m_Option2, Item.m_Option3, aIndex, Item.m_NewOption, Item.m_SetOption, Duration, Item.m_SocketOption, Item.m_BonusSocketOption);
}

void GameProtocol::CGRecvHitHackValues(CGameObject &Obj, PMSG_SEND_HITHACK_INFO * lpMsg)
{
	if (!g_ConfigRead.data.common.bHitHackDetection)
	{
		return;
	}

	if (!ObjectMaxRange(aIndex))
	{
		return;
	}

	if (!gObjIsConnectedGP(aIndex))
	{
		return;
	}

	bool bMismatchFound = false;
	bool bIsAllValues1Equal = false;;
	bool bIsAllValues2Equal = false;;
	

	if (lpObj.m_PlayerData->Dexterity > g_ConfigRead.data.common.iHitHackMaxAgility)
	{
		lpObj.m_PlayerData->dwLastHitHackTick = GetTickCount64();
		return;
	}

	DWORD dwCount = lpObj.m_PlayerData->dwHitHackCounter1;

	if ((dwCount + 1) % 7 != 0)
	{
		lpObj.m_PlayerData->dwLastHitHackValue1[dwCount] = lpMsg->dwHitHackVal1;
		lpObj.m_PlayerData->dwLastHitHackValue2[dwCount] = lpMsg->dwHitHackVal2;
		lpObj.m_PlayerData->dwHitHackCounter1++;
	}
	else
	{
		lpObj.m_PlayerData->dwHitHackCounter1 = 0;
		dwCount = 0;

		lpObj.m_PlayerData->dwLastHitHackValue1[6] = lpMsg->dwHitHackVal1;
		lpObj.m_PlayerData->dwLastHitHackValue2[6] = lpMsg->dwHitHackVal2;

	}

	for (int i = 0; i < 7 - 1; i++)
	{
		if (lpObj.m_PlayerData->dwLastHitHackValue1[i] != lpObj.m_PlayerData->dwLastHitHackValue1[i + 1])
		{
			bMismatchFound = true;
		}
	}

	if (!bMismatchFound)
	{
		if (lpObj.m_PlayerData->dwLastHitHackValue1[0] != 0)
		{
			bIsAllValues1Equal = true;
		}
	}

	bMismatchFound = false;

	for (int i = 0; i < 7 - 1; i++)
	{
		if (lpObj.m_PlayerData->dwLastHitHackValue2[i] != lpObj.m_PlayerData->dwLastHitHackValue2[i + 1])
		{
			bMismatchFound = true;
		}
	}

	if (!bMismatchFound)
	{
		if (lpObj.m_PlayerData->dwLastHitHackValue2[0] = !0)
		{
			bIsAllValues2Equal = true;
		}
	}

	if (bIsAllValues1Equal || bIsAllValues2Equal)
	{
		sLog->outError( "[AntiHack][%s][%s][%s] Hit Hack detected", lpObj.m_PlayerData->Ip_addr, lpObj.AccountID, lpObj.Name);
		AntiHackLog->Output("[AntiHack][%s][%s][%s] Hit Hack detected", lpObj.m_PlayerData->Ip_addr, lpObj.AccountID, lpObj.Name);
		GameProtocol.GCSendDisableReconnect(aIndex);
	}

	lpObj.m_PlayerData->dwLastHitHackTick = GetTickCount64();

}

// event entry notify

void GameProtocol::CGEventEntryNotice(int iEventType, BYTE state)
{
	if (iEventType >= EVENT_NOTIFICATION::EVENT_NOTIFICATION_MAX)
	{
		return;
	}

	PMSG_EVENTENTRY_NOTICE pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xF1, 0xFE, sizeof(PMSG_EVENTENTRY_NOTICE));

	pMsg.btEventType = iEventType;
	pMsg.btNoticeState = state;

	for (int n = g_ConfigRead.server.GetObjectStartUserIndex(); n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		if (getGameObject(n)->Connected == PLAYER_PLAYING)
		{
			if (getGameObject(n]->Type == OBJ_USER && getGameObject(n)->EventNotification[iEventType) != state)
			{
				getGameObject(n]->EventNotification[iEventType) = state;
				IOCP.DataSend(n, (BYTE*)&pMsg, pMsg.h.size);
			}
		}
	}
}

void GameProtocol::CGReqNixieBossEnter(PMSG_REQ_NIXIELAKE_ENTER *lpMsg, CGameObject &Obj)
{
	CGameObject lpObj = Obj;

	if (ObjectMaxRange(Obj.m_Index) == false)
	{
		return;
	}

	if (g_ConfigRead.NixieLakeEntrance == false)
	{
		MsgOutput(lpObj.m_Index, "Nix Event is Closed");
		return;
	}

	if (gObjIsConnected(Obj.m_Index) == false)
	{
		return;
	}

	PMSG_ANS_NIXIELAKE_ENTER pMsg;

	for (int pos = INVETORY_WEAR_SIZE; pos < MAIN_INVENTORY_SIZE; pos++)
	{
		if (lpObj.pInventory[pos].IsItem() && lpObj.pInventory[pos].m_Type == ITEMGET(14, 390) && lpObj.pInventory[pos].m_Durability == 5.0f)
		{
			gObjInventoryDeleteItem(iIndex, pos);
			this->GCInventoryItemDeleteSend(iIndex, pos, 1);
			gObjMoveGate(iIndex, 531);
			sLog->outBasic("[%s][%s] Entered to Nixie Boss Map", lpObj.AccountID, lpObj.Name);
			return;
		}
	}

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x76;
	pMsg.h.subcode = 0x02;
	pMsg.btResult = 1;
	IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (BYTE*)&pMsg, sizeof(pMsg));

}

void GameProtocol::GCPlayerStatsPanelNew(CGameObject &Obj)
{
	PMSG_ADDSTATS_RESULT pStatsInfo;
	PHeadSubSetB((BYTE*)&pStatsInfo, 0xEC, 0x25, sizeof(pStatsInfo));
	pStatsInfo.str = lpObj.m_PlayerData->Strength;
	pStatsInfo.stradd = lpObj.AddStrength;
	pStatsInfo.agi = lpObj.m_PlayerData->Dexterity;
	pStatsInfo.agiadd = lpObj.AddDexterity;
	pStatsInfo.vit = lpObj.m_PlayerData->Vitality;
	pStatsInfo.vitadd = lpObj.AddVitality;
	pStatsInfo.ene = lpObj.m_PlayerData->Energy;
	pStatsInfo.eneadd = lpObj.AddEnergy;
	pStatsInfo.cmd = lpObj.Leadership;
	pStatsInfo.cmdadd = lpObj.AddLeadership;
	pStatsInfo.mPrec = lpObj.m_PlayerData->m_MPSkillOpt.iMpsAutoRecoverMana + lpObj.m_PlayerData->m_WingExcOption.iWingOpRecoveryMana + lpObj.m_PlayerData->m_JewelOfHarmonyEffect.HJOpAddRefillMP + lpObj.m_PlayerData->m_btRefillMPSocketOption;

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pStatsInfo, sizeof(pStatsInfo));
}

// NUEVO STATS PANEL POR %
struct SendPlayerInfoNewPanel
{
	PBMSG_HEAD2 h;
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

void GameProtocol::GCPlayerStatsPanelRates(CGameObject &Obj)
{

	PMSG_STATS_RESULT pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xEC, 0x29, sizeof(pMsg));
	pMsg.CriticalDMGInc = lpObj.m_PlayerData->m_MPSkillOpt.iMpsIncCriticalDamageRate + lpObj.m_PlayerData->SetOpAddCriticalDamageSuccessRate + lpObj.m_PlayerData->SetOpAddCriticalDamage + lpObj.m_PlayerData->m_JewelOfHarmonyEffect.HJOpAddCriticalDamage; // crit dmg inc
	pMsg.unk1 = lpObj.m_PlayerData->m_MPSkillOpt.iMpsAddPhysicDamage + lpObj.m_PlayerData->m_MPSkillOpt.iMpsElfAddPhysicDamage + lpObj.m_PlayerData->m_JewelOfHarmonyEffect.HJOpAddSkillAttack + lpObj.m_PlayerData->SetOpAddSkillAttack; // skill attack dmg inc
	pMsg.str = lpObj.m_PlayerData->Strength;
	pMsg.stradd = lpObj.AddStrength;
	pMsg.agi = lpObj.m_PlayerData->Dexterity;
	pMsg.agiadd = lpObj.AddDexterity;
	pMsg.vit = lpObj.m_PlayerData->Vitality;
	pMsg.vitadd = lpObj.AddVitality;
	pMsg.ene = lpObj.m_PlayerData->Energy;
	pMsg.eneadd = lpObj.AddEnergy;
	pMsg.cmd = lpObj.Leadership;
	pMsg.cmdadd = lpObj.AddLeadership;
	pMsg.ExcellentDMGInc = 27; // 
	pMsg.ExcellentDMGInc2 = 37; // 
	pMsg.SDRation = lpObj.m_PlayerData->m_JewelOfHarmonyEffect.HJOpAddSDRate; // sd(%) when attack
	pMsg.SDBypass = lpObj.m_PlayerData->m_JewelOfHarmonyEffect.HJOpAddIgnoreSDRate; // sd % when attack
	pMsg.unk4 = lpObj.m_PlayerData->m_MPSkillOpt.iMpsIncIgnoreEnemyBlock + lpObj.m_PlayerData->SetOpIgnoreDefense + lpObj.m_PlayerData->m_WingExcOption.iWingOpIgnoreEnemyDefense; // def ign rate
	pMsg.unk5 = lpObj.AutoHPRecovery + lpObj.m_PlayerData->m_MPSkillOpt.iMpsAutoRecoverLife + lpObj.m_PlayerData->m_btRefillHPSocketOption + lpObj.m_PlayerData->m_JewelOfHarmonyEffect.HJOpAddRefillHP + lpObj.m_PlayerData->m_WingExcOption.iWingOpRecoveryHP; // hp rec rate
	pMsg.ZenObincRate = 12;
	pMsg.unk7 = lpObj.m_PlayerData->m_MPSkillOpt.iMpsAddSturn; // stun rate
	pMsg.unk8 = lpObj.m_PlayerData->m_Resistance_Stun; // stun res rate
	pMsg.unk9 = 45678;
	pMsg.unk10 = 0; // shield anbsorb
	pMsg.unk11 = lpObj.m_PlayerData->m_MPSkillOpt.iMpsMonsterDieGetLife; // mob attack hp rec
	pMsg.unk12 = lpObj.m_PlayerData->m_MPSkillOpt.iMpsMonsterDieGetMana;; // mob attack mp rec 
	pMsg.unk13 = lpObj.m_PlayerData->m_MPSkillOpt.iMpsMonsterDieGetSD; // mob attack sd rec
	pMsg.unk14 = lpObj.m_PlayerData->m_MPSkillOpt.iMpsSDSpeed; // sd rec rate
	pMsg.unk15 = lpObj.m_PlayerData->m_MPSkillOpt.iMpsRecoverManaRate; // res all mp rate
	pMsg.unk16 = lpObj.m_PlayerData->m_MPSkillOpt.iMpsRecoverHPRate; // res all hp rate
	pMsg.unk17 = lpObj.m_PlayerData->m_MPSkillOpt.iMpsRecoverSDRate; // res all sd rate
	pMsg.unk18 = lpObj.m_PlayerData->m_MPSkillOpt.iMpsAutoRecoverAG + lpObj.m_PlayerData->SetOpIncAGValue; // ag rec rate
	pMsg.unk19 = 0; // dmg absorb rate
	pMsg.unk20 = lpObj.m_PlayerData->m_MPSkillOpt.iMpsShieldBlockRate + lpObj.m_PlayerData->m_MPSkillOpt.iMpsElfShieldBlockRate + lpObj.m_PlayerData->m_MPSkillOpt.iMpsAddShieldBlockingRage; // shield block rate
	pMsg.unk21 = 0; // weapon block rate
	pMsg.unk22 = 0; // hp absorb
	pMsg.unk23 = lpObj.m_PlayerData->m_JewelOfHarmonyEffect.HJOpDecreaseSDRate; // sd absorb
	pMsg.unk24 = lpObj.m_PlayerData->m_MPSkillOpt.iMpsReturnEnemyAttack + lpObj.m_PlayerData->m_WingExcOption.iWingOpReturnEnemyDamage; // return dmg rate
	pMsg.IncreaseDamageRate = 0; // dmg inc rate
	pMsg.unk26 = lpObj.m_SkillInfo.SoulBarrierManaRate; // MP use redc rate
	pMsg.CritDMGRate = lpObj.m_CriticalDamage + lpObj.m_PlayerData->SetOpAddCriticalDamage; // Critical Dmg Rate
	pMsg.ExcDMGRate = lpObj.m_ExcelentDamage + lpObj.m_PlayerData->SetOpAddExDamageSuccessRate; // Excellent Dmg Rate
	pMsg.DoubleDmgRate = lpObj.m_PlayerData->m_MPSkillOpt.iMpsIncDoubleDamageRate + lpObj.m_PlayerData->m_MPSkillOpt.iMpsIncDarkSpiritDoubleDamageRate + lpObj.m_PlayerData->m_MPSkillOpt.iMpsAddDoubleDamage + lpObj.m_PlayerData->SetOpDoubleDamage + lpObj.m_PlayerData->m_MPSkillOpt.iMpsMonkDoubleDamage + lpObj.m_PlayerData->m_WingExcOption.iWingOpAddDoubleDamage; // 
#if(CUSTOM_DAMAGES12 == 1)
	pMsg.unk28 = lpObj.DamageDecrease + lpObj.S12DamageDecrease + lpObj.m_PlayerData->m_JewelOfHarmonyEffect.HJOpAddDamageDecrease; // lpObj.DamageMinus
#else	
	pMsg.unk28 = lpObj.DamageDecrease + lpObj.m_PlayerData->m_JewelOfHarmonyEffect.HJOpAddDamageDecrease; // lpObj.DamageMinus
#endif
	pMsg.unk29 = lpObj.m_PlayerData->m_btAGReduceRate; // ag usage rec rate
	pMsg.unk30 = lpObj.DamageReflect + lpObj.m_PlayerData->SetOpReflectionDamage; // damage reflect

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}


void GameProtocol::CGReqHuntingLog(CGameObject &Obj)
{
	PMSG_ANS_HUNTING_LOG pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xEC, 0x50, sizeof(pMsg));
	pMsg.unk1 = 1;
	pMsg.unk2 = 125;
	pMsg.unk3 = 300;
	pMsg.unk4 = 400;

	g_Log.AddC(TColor::Blue, "Hunting Log in Test");

}

void GameProtocol::CGSaveMuBotSettings(PMSG_MUBOT_SETTINGS_RECV* lpMsg, CGameObject &Obj)
{
	if (!gObjIsConnectedGP(aIndex))
		return;

	DGMuBotSettingsSend(aIndex, lpObj.Name, &lpMsg->btDATA[0]);

	g_Log.AddC(TColor::Aquamarine, "[MuHelper] Option Save [%s][%s]",lpObj.AccountID, lpObj.Name);
}

void GameProtocol::GCSendMuBotSettings(CGameObject &Obj, BYTE* lpData)
{
	if (!gObjIsConnected(aIndex))
		return;

	PMSG_MUBOT_SETTINGS_RECV pMsg;
	PHeadSetW((BYTE*)&pMsg, 0xAE, sizeof(pMsg));

	pMsg.btType = 0;
	memcpy(pMsg.btDATA, lpData, sizeof(pMsg.btDATA));

	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, sizeof(pMsg));

	g_Log.AddC(TColor::Green,"[MuHelper] Option Send [%s][%s]",lpObj.AccountID, lpObj.Name);
}

void GameProtocol::CGRequestStartMuBot(PMSG_MUBOT_REQ_START* lpMsg, CGameObject &Obj)
{
	if (g_ConfigRead.data.mubot.enable == FALSE) 
	{
		return;
	}

	if (!gObjIsConnectedGP(aIndex)) 
	{
		return;
	}

	PMSG_MUBOT_USE pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xBF, 0x51, sizeof(pMsg));
	pMsg.Money = 0;
	pMsg.Time = 0;
	pMsg.Status = 1;

	

	if (lpObj.m_IfState.use == TRUE) 
	{
		return;
	}

	if (lpObj.Level < g_ConfigRead.data.mubot.minlevel) 
	{
		MsgOutput(aIndex, "Only characters above %d level can run MU Helper", g_ConfigRead.data.mubot.minlevel);
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (lpObj.MapNumber == MAP_INDEX_CASTLESIEGE || lpObj.MapNumber == MAP_INDEX_VULCANROOM)
	{
		MsgOutput(aIndex, "MU Helper cant run on this location");
		IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		lpObj.m_bMUBOT = FALSE;
		return;
	}

	if (lpMsg->btType == 0) 
	{
		if (lpObj.m_PlayerData->Money < 10000)
		{
			pMsg.Status = 2;
		}
		else 
		{
			lpObj.m_PlayerData->Money -= 10000;
			GCMoneySend(lpObj.m_Index, lpObj.m_PlayerData->Money);
			pMsg.Status = 0;
			pMsg.Time = 0;
			pMsg.Money = 10000;
			lpObj.m_bMUBOT = TRUE;
		}
	}
	else 
	{
		lpObj.m_bMUBOT = FALSE;
	}

	lpObj.m_iMUBOT_TIME = 0;
	lpObj.m_iMUBOT_STAGE = 0;
	IOCP.DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}

std::vector<Recv_PetItem_Info> GameProtocol::gObjRequestPetItemInfoDS(CGameObject &Obj, int inventype, std::vector<int> serials)
{
	int serverCode = g_ConfigRead.server.GetGameServerCode() / 20;
	int founditemcount = 0;
	int serverType = g_ConfigRead.server.GetServerType(); // stub server type.

	std::vector<Recv_PetItem_Info> tData;
	if (serverType == 0) // normal GS
	{
		for (int i = 0; i < serials.size(); i++)
		{
			Recv_PetItem_Info pRecvPetInfo;

			QueryResult* res = this->m_PetDB->Fetch("SELECT Pet_Level, Pet_Exp FROM T_PetItem_Info WHERE ItemSerial=%I64d",
				serials[i]);

			if (res == NULL)
			{
				this->m_PetDB->ExecQuery("INSERT INTO T_PetItem_Info (ItemSerial, Pet_Level, Pet_Exp) VALUES (%I64d, %d, %I64d)",
					serials[i], 1, 0);

				pRecvPetInfo.Level = 1;
				pRecvPetInfo.Exp = 0;
				pRecvPetInfo.nPos = i;
				pRecvPetInfo.nSerial = serials[i];

			}
			else
			{
				pRecvPetInfo.Level = this->m_PetDB->GetAsInteger(0);
				pRecvPetInfo.Exp = this->m_PetDB->GetAsInteger64(1);
				pRecvPetInfo.nPos = i;
				pRecvPetInfo.nSerial = serials[i];
			}
			tData.push_back(pRecvPetInfo);
		}
	}
	else // battlecore GS
	{
		for (int i = 0; i < serials.size(); i++)
		{
			Recv_PetItem_Info pRecvPetInfo;

			QueryResult* res = this->m_PetDB->Fetch("CALL IGC_BattleCore_LoadPetItemInfo (%I64d, %d);",
				serials[i], serverCode);

			if (res == NULL)
			{
				this->m_PetDB->ExecQuery("CALL IGC_BattleCore_SavePetItemInfo (%I64d, %d, %d, %I64d);",
					serials[i], serverCode, i, 0);

				pRecvPetInfo.Level = 1);
				pRecvPetInfo.Exp = 0;
				pRecvPetInfo.nPos = i;
				pRecvPetInfo.nSerial = serials[i];
			}
			else
			{
				pRecvPetInfo.Level = this->m_PetDB->GetAsInteger(0);
				pRecvPetInfo.Exp = this->m_PetDB->GetAsInteger64(1);
				pRecvPetInfo.nPos = this->m_PetDB->GetAsInteger64(2);
				pRecvPetInfo.nSerial = this->m_PetDB->GetAsInteger64(3);
			}
			tData.push_back(pRecvPetInfo);
		}
	}
	return tData;
}


void GameProtocol::gObjRequestPetItemInfo(CGameObject &Obj, int inventype)
{
	std::vector<int> serials;
	int founditemcount = 0;
	if (inventype == 0)
	{
		for (int n = 0; n < INVENTORY_SIZE; n++)
		{
			if (lpObj.pInventory[n].IsItem())
			{
				if (lpObj.pInventory[n].m_Type == ITEMGET(13, 4) || lpObj.pInventory[n].m_Type == ITEMGET(13, 5))
				{
					founditemcount++;
					serials.push_back(lpObj.pInventory[n].m_Number);
				}
			}
		}
	}
	else if (inventype == 1)
	{
		for (int n = 0; n < WAREHOUSE_SIZE; n++)
		{
			if (lpObj.pWarehouse[n].IsItem())
			{
				if (lpObj.pWarehouse[n].m_Type == ITEMGET(13, 4) || lpObj.pWarehouse[n].m_Type == ITEMGET(13, 5))
				{
					founditemcount++;
					serials.push_back(lpObj.pWarehouse[n].m_Number);
				}
			}
		}
	}

	std::vector<Recv_PetItem_Info> data = gObjRequestPetItemInfoDS(lpObj, inventype, serials);

	PMSG_SEND_PET_ITEMINFO pMsg;
	pMsg.h.set((BYTE*)&pMsg, 0xA9, sizeof(pMsg));
	pMsg.InvenType = inventype;
	pMsg.PetType = g_ConfigRead.server.GetServerType();

	if (inventype == 0)
	{
		BOOL ReCalc = 0;

		for (int n = 0; n < data.size(); n++)
		{
			if (data[n].nPos < INVETORY_WEAR_SIZE)
			{
				ReCalc = 1;
			}

			if (lpObj.pInventory[data[n].nPos].IsItem())
			{
				if (lpObj.pInventory[data[n].nPos].m_Number == data[n].nSerial)
				{
					lpObj.pInventory[data[n].nPos].SetPetItemInfo(data[n].Level, data[n].Exp);
				}
			}
			this->CGRequestPetItemInfo(lpObj, pMsg);
		}

		if (ReCalc != FALSE)
		{
			gObjCalCharacter.CalcCharacter(lpObj);
		}
	}
	else if (inventype == 1)
	{
		for (int n = 0; n < data.size(); n++)
		{

			if (lpObj.pWarehouse[data[n].nPos].IsItem())
			{
				if (lpObj.pWarehouse[data[n].nPos].m_Number == data[n].nSerial)
				{
					lpObj.pWarehouse[data[n].nPos].SetPetItemInfo(data[n].Level, data[n].Exp);
				}
			}
			this->CGRequestPetItemInfo(lpObj, pMsg);
		}
	}
}

void GameProtocol::GJPUserClose(LPSTR szAccountID)
{
	SDHP_USERCLOSE_ID pClose;

	pClose.h.c = 0xC1;
	pClose.h.size = sizeof(SDHP_USERCLOSE_ID);
	pClose.h.headcode = 0x05;
	memcpy(pClose.szId, szAccountID, 10);

	wsJServerCli.DataSend((char*)&pClose, pClose.h.size);
}

void GameProtocol::GJPUserClose(CGameObject &Obj)
{
	SDHP_USERCLOSE_ID pClose;

	pClose.h.c = 0xC1;
	pClose.h.size = sizeof(SDHP_USERCLOSE_ID);
	pClose.h.headcode = 0x05;
	memcpy(pClose.szId, lpObj.AccountID, 10);
	memcpy(pClose.szName, lpObj.BackName, 10);
	pClose.Level = lpObj.Level;
	pClose.DbClass = lpObj.m_PlayerData->DbClass;

	wsJServerCli.DataSend((char*)&pClose, pClose.h.size);
}


