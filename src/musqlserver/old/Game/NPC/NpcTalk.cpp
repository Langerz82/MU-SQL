////////////////////////////////////////////////////////////////////////////////
// NpcTalk.cpp
#include "StdAfx.h"
#include "User/CUserData.h"
#include "NpcTalk.h"
#include "gObjMonster.h"
#include "util.h"
#include "GameProtocol.h"
#include "GameMain.h"
#include "Event/BloodCastle/BloodCastle.h"
#include "DevilSquare.h"
#include "QuestInfo.h"
#include "CastleSiegeSync.h"
#include "KanturuEntranceNPC.h"
#include "JewelOfHarmonySystem.h"
#include "Event.h"
#include "BuffEffect.h"
#include "BuffEffectSlot.h"
#include "DoppelGanger.h"

#include "LastManStanding.h"
#include "configread.h"
#include "GensSystem.h"
#include "NewPVP.h"
#include "User/CUserData.h"
#include "QuestExpProgMng.h"
#include "PentagramMixSystem.h"
#include "RuudStore.h"

// GS-N 0.99.60T 0x005211D0
//	GS-N	1.00.18	JPN	0x005527F0	-	Completed

BOOL NpcTalk(CGameObject &Npc, CGameObject lpObj)
{
	int npcnum = lpNpc->Class;
	//sLog->outError("[K2] %d", npcnum);
	if (npcnum < 0)
	{
		return FALSE;
	}

	if ((Obj.m_IfState->use) > 0)
	{
		return TRUE;
	}

	if (npcnum == 229) // Marlon
	{
		gQeustNpcTeleport.TalkRefAdd();
		Obj.TargetNpcNumber = lpNpc->m_Index;
	}

	if (NpcQuestCheck(lpNpc, lpObj) != FALSE)
	{
		return TRUE;
	}

	g_QuestExpProgMng.ChkUserQuestType(QUESTEXP_ASK_NPC_TALK, lpObj, lpNpc, 0);

	if (npcnum == 568) //-> Wandering Merchant Zyro
	{
		gQuestExpNpcTeleport.TalkRefAdd();
		Obj.TargetNpcNumber = lpNpc->m_Index;

		if (Npc_Dialog(lpNpc, lpObj))
		{
			return true;
		}
	}

	switch (npcnum)
	{
	case 367:
		if (NpcMainatenceMachine(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 259:
		if (NpcReira(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 254:
		if (NpcFasi(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 241:
		if (NpcGuildMasterTalk(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 239:
		if (NpcBattleAnnouncer(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 249:
		if (NpcRolensiaGuard(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 238:
		if (NpcChaosGoblelin(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 237:
		if (NpcRusipher(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 253:
		if (NpcPosionGirl(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 245:
		if (NpcDeviasWizard(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 247:
		if (NpcDeviasGuard(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;
	case 695:
	{

	}
	break;
	case 220:
		if (NpcCastleHuntZoneGuard(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 240:
		if (NpcWarehouse(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 226:
		if (NpcDarkSpiritTrainer(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 242:
		if (NpcNoriaRara(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 244:
		if (NpcDeviasMadam(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 236:
		if (NpcEventChipNPC(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 234:
		if (NpcServerDivision(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 232:
		if (NpcAngelKing(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 233:
		if (NpcAngelMessanger(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 255:
		if (NpcRolensiaMadam(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 250:
		if (NpcRoadMerchant(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 246:
		if (NpcDeviasWeapon(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 251:
		if (NpcRorensiaSmith(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 243:
		if (NpcNoriJangIn(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 256:
		if (NpcJewelMixDealer(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 221:
		if (NpcSiegeMachine_Attack(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 222:
		if (NpcSiegeMachine_Defense(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 223:
		if (NpcElderCircle(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 224:
		if (NpcCastleGuard(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 219:
		if (NpcCastleGateLever(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 216:
		if (NpcCastleCrown(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 217:
	case 218:
		if (NpcCastleSwitch(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 257:
		return Npc_Dialog(lpNpc, lpObj);

	case 368:
	case 369:
	case 370:
		if (g_kJewelOfHarmonySystem.NpcJewelOfHarmony(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;

	case 371:
	{
		if (NpcLeoHelper(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
	}
	break;

	case 375:
		if (NpcChaosCardMaster(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;
	case 407:
		if (NpcWerewolfQuarrel(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;
	case 408:
		if (NpcGateKeeper(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;
	case 450:
	case 478:
	{
		return FALSE;
	}
	break;
	case 452:
		if (NpcSeedMaster(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;
	case 453:
		if (NpcSeedResearcher(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;
	case 465:
		if (NpcSantaClause(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;
	case 467:
	{
		gGameProtocol.GCServerCmd(Obj.m_Index, 0x11, 0x01, 0x00);
	}
	break;
	case 468:
	{
		gObjAddBuffEffect(lpObj, BUFFTYPE_SANTA_POWER, EFFECTTYPE_POWER_UP, 30, 0, 0, 1800);
	}
	break;
	case 469:
	{
		gObjAddBuffEffect(lpObj, BUFFTYPE_SANTA_GUARD, EFFECTTYPE_GUARD_UP, 100, 0, 0, 1800);
	}
	break;
	case 470:
	{
		Obj.Life = Obj.MaxLife + Obj.AddLife;
		gGameProtocol.GCReFillSend(Obj.m_Index, Obj.Life, 0xFF, 0, Obj.iShield);
	}
	break;
	case 471:
	{
		Obj.Mana = Obj.MaxMana + Obj.AddMana;
		gGameProtocol.GCManaSend(Obj.m_Index, Obj.Mana, 0xFF, 0, Obj.BP);
	}
	break;
	case 472:
	{
		gObjAddBuffEffect(lpObj, BUFFTYPE_SANTA_HEAL, EFFECTTYPE_HP, 500, 0, 0, 1800);
	}
	break;
	case 473:
	{
		gObjAddBuffEffect(lpObj, BUFFTYPE_SANTA_PROTECT, EFFECTTYPE_MANA, 500, 0, 0, 1800);
	}
	break;
	case 474:
	{
		gObjAddBuffEffect(lpObj, BUFFTYPE_SANTA_SPEED, EFFECTTYPE_ATTACKSPEED, 15, 0, 0, 1800);
	}
	break;
	case 475:
	{
		gObjAddBuffEffect(lpObj, BUFFTYPE_SANTA_LUCKY, EFFECTTYPE_AG_RECOVERY_UP, 10, 0, 0, 1800);
	}
	break;
	case 479:
	{
		return NpcTitus(lpNpc, lpObj);
	}
	break;
	case 522:
	{
		return NpcJerint(lpNpc, lpObj);
	}
	break;
	case 540:
	{
		return NpcLugard(lpNpc, lpObj);
	}
	break;
	case 541:
	{
		return NpcSilverBox(lpNpc, lpObj);
	}
	break;
	case 542:
	{
		return NpcGoldBox(lpNpc, lpObj);
	}
	break;
	case 543:
	case 544:
		return Npc_Dialog(lpNpc, lpObj);
	case 546:
	{
		return NpcJewelMixDealer(lpNpc, lpObj);
	}
	break;
	case 547:
	{
		return NpcJulia(lpNpc, lpObj);
	}
	break;
	case 566:
	case 567:
		return Npc_Dialog(lpNpc, lpObj);
	case 581:
		return Npc_Dialog(lpNpc, lpObj);
	case 579:
		return NpcDavid(lpNpc, lpObj);
		break;
	case 580:
		return NpcAcheronEntrance(lpNpc, lpObj);
	case 582:
		return NpcElementalMaster(lpNpc, lpObj);
	case 584:
		return NpcArcaWar(lpNpc, lpObj);
	case 651:
		return NpcMarketWall(lpNpc, lpObj);
	case 682:
		return NpcMonicaTalk(lpNpc, lpObj);
	case 725:
		return g_RuudStore.NpcTalk(lpNpc, lpObj);
	case 720:
		if (NpcRusipher(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;
	case 719:
		if (NpcAngelMessanger(lpNpc, lpObj) == TRUE)
		{
			return TRUE;
		}
		break;
	case 723:
	{
		return NpcLugard(lpNpc, lpObj);
	}
	break;
	case 721:
	{
		return NpcJerint(lpNpc, lpObj);
	}
	break;
	case 258:
	{
		if (g_LastManStanding.m_Cfg.bEnable)
		{
			if (lpNpc->MapNumber == g_LastManStanding.m_Cfg.iRegNPCMap && lpNpc->X == g_LastManStanding.m_Cfg.iRegNPCX && lpNpc->Y == g_LastManStanding.m_Cfg.iRegNPCY)
			{
				if (g_LastManStanding.m_Rooms[0].bState != 1) // rooms[0] because the state at this point is same for all rooms
				{
					MsgOutput(Obj.m_Index, Lang.GetText(0, 395));
					return false;
				}
				if (Obj.m_PlayerData->RegisterdLMS == 1)
				{
					MsgOutput(Obj.m_Index, Lang.GetText(0, 396));
					return false;
				}
				if (Obj.PartyNumber >= 0)
				{
					MsgOutput(Obj.m_Index, Lang.GetText(0, 397));
					return false;
				}
				if (g_LastManStanding.m_Cfg.iReqItem)
				{
					int invitationpos = -1;
					int removedCount = 0;
					if (gObjGetItemCountInIventory(Obj.m_Index, ITEMGET(g_LastManStanding.m_Cfg.iReqItemCat, g_LastManStanding.m_Cfg.iReqItemIdx)) < g_LastManStanding.m_Cfg.iReqItemCount)
					{
						MsgOutput(Obj.m_Index, Lang.GetText(0, 398), g_LastManStanding.m_Cfg.iReqItemCount, ItemAttribute[ITEMGET(g_LastManStanding.m_Cfg.iReqItemCat, g_LastManStanding.m_Cfg.iReqItemIdx)].Name);
						return false;

					}
					for (short i = 0; i < MAIN_INVENTORY_SIZE; ++i)
					{
						if (Obj.pntInventory[i]->m_Type > 0 && Obj.pntInventory[i]->m_Type == ITEMGET(g_LastManStanding.m_Cfg.iReqItemCat, g_LastManStanding.m_Cfg.iReqItemIdx))
						{
							if (removedCount < g_LastManStanding.m_Cfg.iReqItemCount)
							{
								gObjInventoryDeleteItem(Obj.m_Index, i);
								gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, i, 1);
								removedCount++;
							}
							else
							{
								break;
							}
						}
					}
					/*	if(invitationpos == -1)
						{
							MsgOutput(Obj.m_Index,Lang.GetText(0,398),g_LastManStanding.m_Cfg.iReqItemCount,ItemAttribute[ITEMGET(g_LastManStanding.m_Cfg.iReqItemCat,g_LastManStanding.m_Cfg.iReqItemIdx)].Name);
							return false;
						}*/
					if (Obj.m_PlayerData->Money < g_LastManStanding.m_Cfg.iReqZen)
					{
						MsgOutput(Obj.m_Index, Lang.GetText(0, 399), g_LastManStanding.m_Cfg.iReqZen);
						return false;
					}
					Obj.m_PlayerData->Money -= g_LastManStanding.m_Cfg.iReqZen;
					gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

					if (g_LastManStanding.AddUser(Obj.m_Index)) {
						MsgOutput(Obj.m_Index, Lang.GetText(0, 400));
					}
				}
				else
				{
					if (Obj.m_PlayerData->Money < g_LastManStanding.m_Cfg.iReqZen)
					{
						MsgOutput(Obj.m_Index, Lang.GetText(0, 401), g_LastManStanding.m_Cfg.iReqZen);
						return false;
					}
					if (g_LastManStanding.AddUser(Obj.m_Index)) {
						MsgOutput(Obj.m_Index, Lang.GetText(0, 402));
					}
				}
			}
		}
	}
	break;
	}

	return FALSE;
}

BOOL NpcMainatenceMachine(CGameObject &Npc, CGameObject lpObj)
{
	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		return TRUE;
	}

	g_KanturuEntranceNPC.NotifyEntranceInfo(Obj.m_Index);
	return TRUE;
}

BOOL NpcReira(CGameObject &Npc, CGameObject lpObj)
{
	return FALSE;
}

BOOL NpcBattleAnnouncer(CGameObject &Npc, CGameObject lpObj)
{
	return 0;
}

BOOL NpcFasi(CGameObject &Npc, CGameObject lpObj)
{
	if (g_DevilSquare.IsEventEnable() != FALSE)
	{
		gGameProtocol.GCServerCmd(Obj.m_Index, 1, 6, 0);
	}

	BOOL bPlayerKiller = FALSE; //Season 2.5 add-on

	if (Obj.PartyNumber >= 0) //Season 2.5 add-on
	{
		if (gParty.GetPKPartyPenalty(Obj.PartyNumber) > 4)
		{
			bPlayerKiller = TRUE;
		}
	}
	else if (Obj.m_PK_Level > 4)
	{
		bPlayerKiller = TRUE;
	}

	if (bPlayerKiller == TRUE)
	{
		int hour = 0;
		int min = 0;

		if (lpNpc->TargetNumber != Obj.m_Index)
		{
			if (gObjGetPkTime(lpObj, hour, min) == TRUE)
			{
				DWORD dwNeedPoint = 0;

				switch (Obj.m_PK_Count)
				{
				case 1:
					dwNeedPoint = g_ConfigRead.pk.GetPKReqPoint(4);
					break;
				case 2:
					dwNeedPoint = g_ConfigRead.pk.GetPKReqPoint(5);
					break;
				case 3:
					dwNeedPoint = g_ConfigRead.pk.GetPKReqPoint(6);
					break;
				default:
					if (Obj.m_PK_Level > 3)
					{
						dwNeedPoint = g_ConfigRead.pk.iPKPointIncForMurderer * (Obj.m_PK_Count - 3) + g_ConfigRead.pk.GetPKReqPoint(6);
					}
					break;
				}

				if (dwNeedPoint != 0)
				{
					dwNeedPoint -= Obj.m_PK_Time;
				}

				char szTemp[100];

				wsprintf(szTemp, Lang.GetText(0, 16), dwNeedPoint);

				gGameProtocol.ChatTargetSend(lpNpc, szTemp, Obj.m_Index);

				lpNpc->TargetNumber = Obj.m_Index;

				return TRUE;
			}
		}
		else if (g_ConfigRead.pk.bPkPenaltyDisable == FALSE && Obj.m_PK_Level > 4)
		{
			int numbertext = rand() % 3;

			switch (numbertext)
			{
			case 0:
				gGameProtocol.ChatTargetSend(lpNpc, Lang.GetText(0, 34), Obj.m_Index);
				break;

			case 1:
				gGameProtocol.ChatTargetSend(lpNpc, Lang.GetText(0, 35), Obj.m_Index);
				break;

			case 2:
				gGameProtocol.ChatTargetSend(lpNpc, Lang.GetText(0, 36), Obj.m_Index);
				break;
			}

			lpNpc->TargetNumber = -1;	// Apply Deathway Fix, change 0 to -1;
			return TRUE;
		}
	}

	return FALSE;
}



BOOL NpcGuildMasterTalk(CGameObject &Npc, CGameObject lpObj)
{
	if (g_ConfigRead.data.common.GuildCreate == FALSE)
	{
		gGameProtocol.ChatTargetSend(lpNpc, Lang.GetText(0, 403), Obj.m_Index);
		return TRUE;
	}

	if (g_GensSystem.GetGensInfluence(lpObj) == FALSE)
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 404), Obj.m_Index, 1);
		return TRUE;
	}

	if (g_ArcaBattle.GetState() > 2 && g_ArcaBattle.GetState() < 9)
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 405), Obj.m_Index, 1);
		return TRUE;
	}

	if (Obj.m_PlayerData->GuildNumber > 0)
	{
		gGameProtocol.ChatTargetSend(lpNpc, Lang.GetText(0, 48), Obj.m_Index);
		return TRUE;
	}

	int capacity = gObjGuildMasterCapacityTest(lpObj);

	if (capacity == 0)
	{
		gGameProtocol.ChatTargetSend(lpNpc, Lang.GetText(0, 49), Obj.m_Index);
	}
	else if (capacity == 2)
	{
		gGameProtocol.ChatTargetSend(lpNpc, Lang.GetText(0, 50), Obj.m_Index);
	}
	else
	{
		gGameProtocol.GCGuildMasterQuestionSend(Obj.m_Index);
	}

	return TRUE;
}



BOOL NpcRolensiaGuard(CGameObject &Npc, CGameObject lpObj)
{
	if (gEnableEventNPCTalk != FALSE)
	{
		gGameProtocol.GCServerCmd(Obj.m_Index, 4, 0, 0);
	}
	return TRUE;
}



BOOL NpcChaosGoblelin(CGameObject &Npc, CGameObject lpObj)
{
	gUserFindDevilSquareKeyEyes(Obj.m_Index);

	return FALSE;
}


BOOL g_bUseLotterySystem = TRUE;

BOOL NpcChaosCardMaster(CGameObject &Npc, CGameObject lpObj)
{
	if (g_bUseLotterySystem == FALSE)
		return FALSE;

	if (bCanChaosBox == TRUE)
	{
		if (Obj.m_IfState->use > 0)
			return FALSE;

		if (Obj.m_bPShopOpen == true)
		{
			sLog->outBasic("[%s][%s] is Already Opening PShop, ChaosBox Failed",
				Obj.AccountID, Obj.Name);

			gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 112), Obj.m_Index, 1);
			return TRUE;
		}

		PMSG_TALKRESULT pMsg;

		pMsg.h.c = 0xC3;
		pMsg.h.headcode = 0x30;
		pMsg.h.size = sizeof(pMsg);
		pMsg.result = 0x15;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

		Obj.m_IfState->use = 1;
		Obj.m_IfState->type = 7;

		gObjItemTextSave(lpObj);
		gObjWarehouseTextSave(lpObj);
	}

	return TRUE;
}



BOOL NpcRusipher(CGameObject &Npc, CGameObject lpObj)
{
	if (g_DevilSquare.IsEventEnable() == FALSE)
	{
		return TRUE;
	}

	CGameObject &Obj = Obj.m_Index;

	if ((Obj.m_IfState->use) > 0)
	{
		return TRUE;
	}

	if (gUserFindDevilSquareInvitation(Obj.m_Index) == FALSE)
	{
		gGameProtocol.GCServerCmd(Obj.m_Index, 1, 2, 0);
	}
	else if (g_DevilSquare.GetState() == 1)
	{
		PMSG_TALKRESULT pMsg;

		pMsg.h.c = 0xC3;
		pMsg.h.headcode = 0x30;
		pMsg.h.size = sizeof(pMsg);
		pMsg.result = 0x04;

		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

		Obj.m_IfState->use = 1;
		Obj.m_IfState->type = 12;
	}
	else
	{
		gGameProtocol.GCServerCmd(Obj.m_Index, 1, 3, 0);
	}

	return TRUE;
}



BOOL NpcPosionGirl(CGameObject &Npc, CGameObject lpObj)
{
	return FALSE;
}

BOOL NpcDeviasWizard(CGameObject &Npc, CGameObject lpObj)
{
	return FALSE;
}


BOOL NpcDeviasGuard(CGameObject &Npc, CGameObject lpObj)
{
	return TRUE;
}


BOOL NpcDeviasWareHousemen(CGameObject &Npc, CGameObject lpObj)
{
	return FALSE;
}

BOOL NpcWarehouse(CGameObject &Npc, CGameObject lpObj)
{
	return FALSE;
}


BOOL NpcNoriaRara(CGameObject &Npc, CGameObject lpObj)
{
	if (gEnableEventNPCTalk != FALSE)
	{
		gGameProtocol.GCServerCmd(Obj.m_Index, 4, 2, 0);
	}

	return FALSE;
}

BOOL NpcDeviasMadam(CGameObject &Npc, CGameObject lpObj)
{
	if (gEnableEventNPCTalk != FALSE)
	{
		gGameProtocol.GCServerCmd(Obj.m_Index, 4, 1, 0);
	}

	return FALSE;
}

struct PMSG_REQ_VIEW_EC_MN
{
	PBMSG_HEAD2 h;	// C1:01
	int iINDEX;	// 4
	char szUID[11];	// 8
};

BOOL NpcEventChipNPC(CGameObject &Npc, CGameObject lpObj)
{
	if ((Obj.m_IfState->use) > 0)
	{
		return TRUE;
	}

	if (lpNpc->MapNumber == 0)
	{
		PMSG_REQ_VIEW_EC_MN pMsgTha;

		PHeadSubSetB((BYTE*)&pMsgTha, 0xBE, 0x01, sizeof(pMsgTha));
		pMsgTha.iINDEX = Obj.m_Index;
		strcpy(pMsgTha.szUID, Obj.AccountID);
		pEventObj = lpNpc;

		wsDataCli.DataSend((char*)&pMsgTha, sizeof(pMsgTha));

		Obj.m_IfState->type = 9;
		Obj.m_IfState->state = 0;
		Obj.m_IfState->use = 1;

		return TRUE;
	}

	if (lpNpc->MapNumber == 2)
	{
		PMSG_EVENTCHIPINFO Result;

		PHeadSetB((BYTE*)&Result, 0x94, sizeof(Result));
		Result.Type = 2;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&Result, Result.h.size);

		Obj.m_IfState->type = 9;
		Obj.m_IfState->state = 0;
		Obj.m_IfState->use = 1;

		return TRUE;
	}

	return FALSE;
}


BOOL NpcRorensiaSmith(CGameObject &Npc, CGameObject lpObj)
{
	return FALSE;
}



BOOL NpcNoriJangIn(CGameObject &Npc, CGameObject lpObj)
{
	return FALSE;
}



BOOL NpcQuestCheck(CGameObject &Npc, CGameObject lpObj)
{
	if (g_QuestInfo.NpcTalk(lpNpc, lpObj) != FALSE)
	{
		return TRUE;
	}

	return FALSE;
}



BOOL NpcServerDivision(CGameObject &Npc, CGameObject lpObj)
{
	if (gEnableServerDivision == FALSE)
	{
		return TRUE;
	}

	if (Obj.m_PlayerData->lpGuild != NULL)
	{
		gGameProtocol.GCServerCmd(Obj.m_Index, 6, 0, 0);
		return TRUE;
	}

	Obj.m_IfState->type = 11;
	Obj.m_IfState->state = 0;
	Obj.m_IfState->use = 1;

	return FALSE;
}


BOOL NpcRoadMerchant(CGameObject &Npc, CGameObject lpObj)
{
	return FALSE;
}



BOOL NpcAngelKing(CGameObject &Npc, CGameObject lpObj)
{
	return g_BloodCastle.NpcAngelKing(lpNpc, lpObj);
}


BOOL NpcAngelMessanger(CGameObject &Npc, CGameObject lpObj)
{
	if ((Obj.m_IfState->use) > 0)
	{
		return TRUE;
	}

	PMSG_TALKRESULT pMsg;

	pMsg.h.c = 0xC3;
	pMsg.h.headcode = 0x30;
	pMsg.h.size = sizeof(pMsg);
	pMsg.result = 0x06;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

	Obj.m_IfState->use = 1;
	Obj.m_IfState->type = 12;

	return FALSE;
}


BOOL NpcRolensiaMadam(CGameObject &Npc, CGameObject lpObj)
{
	return FALSE;
}



BOOL NpcDeviasWeapon(CGameObject &Npc, CGameObject lpObj)
{
	return FALSE;
}



BOOL NpcDarkSpiritTrainer(CGameObject &Npc, CGameObject lpObj)
{
	if (bCanChaosBox == TRUE)
	{
		PMSG_TALKRESULT pMsg;

		pMsg.h.c = 0xC3;
		pMsg.h.headcode = 0x30;
		pMsg.h.size = sizeof(pMsg);

		if (Obj.m_bPShopOpen == true)
		{
			sLog->outBasic("[%s][%s] is Already Opening PShop, ChaosBox Failed",
				Obj.AccountID, Obj.Name);
			gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 112), Obj.m_Index, 1);

			return TRUE;
		}

		Obj.m_IfState->type = 13;
		Obj.m_IfState->state = 0;
		Obj.m_IfState->use = 1;

		pMsg.result = 0x07;
		Obj.bIsChaosMixCompleted = false;
		pMsg.level1 = gDQChaosSuccessRateLevel1;
		pMsg.level2 = gDQChaosSuccessRateLevel2;
		pMsg.level3 = gDQChaosSuccessRateLevel3;
		pMsg.level4 = gDQChaosSuccessRateLevel4;
		pMsg.level5 = gDQChaosSuccessRateLevel5;
		pMsg.level6 = gDQChaosSuccessRateLevel6;
		pMsg.level7 = gDQChaosSuccessRateLevel7;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

		gGameProtocol.GCAnsCsMapSvrTaxInfo(Obj.m_Index, 1, g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index));
		gObjInventoryTrans(Obj.m_Index);

		sLog->outBasic("[%s][%s] Open Chaos Box", Obj.AccountID, Obj.Name);
		gObjItemTextSave(lpObj);
		gObjWarehouseTextSave(lpObj);
	}

	return TRUE;
}



BOOL NpcJewelMixDealer(CGameObject &Npc, CGameObject lpObj)
{
	if (bCanChaosBox == TRUE)
	{
		if ((Obj.m_IfState->use) > 0)
		{
			return TRUE;
		}

		PMSG_TALKRESULT pMsg;

		pMsg.h.c = 0xC3;
		pMsg.h.headcode = 0x30;
		pMsg.h.size = sizeof(pMsg);
		pMsg.result = 0x09;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

		Obj.m_IfState->use = 1;
		Obj.m_IfState->type = 12;

		gObjItemTextSave(lpObj);
		gObjWarehouseTextSave(lpObj);

	}

	return TRUE;
}


struct PMSG_ANS_USE_WEAPON_INTERFACE
{
	PBMSG_HEAD2 h;	// C1:B7:00
	BYTE btResult;	// 4
	BYTE btWeaponType;	// 5
	BYTE btObjIndexH;	// 6
	BYTE btObjIndexL;	// 7
};

BOOL NpcSiegeMachine_Attack(CGameObject &Npc, CGameObject lpObj)
{
	if ((Obj.m_IfState->use) > 0)
	{
		return TRUE;
	}

	PMSG_ANS_USE_WEAPON_INTERFACE pResult = { 0 };

	PHeadSubSetB((BYTE*)&pResult, 0xB7, 0x00, sizeof(pResult));

	pResult.btResult = 0;
	pResult.btWeaponType = 1;
	pResult.btObjIndexH = SET_NUMBERH(lpNpc->m_Index);
	pResult.btObjIndexL = SET_NUMBERL(lpNpc->m_Index);

	if (gObjIsConnectedGP(lpNpc->m_iWeaponUser) == FALSE)
	{
		lpNpc->m_iWeaponUser = 0;
		lpNpc->m_btWeaponState = 1;
	}

	lpNpc->m_btWeaponState = 1;

	if (lpNpc->m_btWeaponState == 1)
	{
		pResult.btResult = 1;
		lpNpc->m_btWeaponState = 2;
		lpNpc->m_iWeaponUser = Obj.m_Index;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		Obj.m_IfState->use = 1;
		Obj.m_IfState->type = 15;
	}
	else
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
	}

	return TRUE;
}


BOOL NpcSiegeMachine_Defense(CGameObject &Npc, CGameObject lpObj)
{
	if ((Obj.m_IfState->use) > 0)
	{
		return TRUE;
	}

	PMSG_ANS_USE_WEAPON_INTERFACE pResult = { 0 };

	PHeadSubSetB((BYTE*)&pResult, 0xB7, 0x00, sizeof(pResult));

	pResult.btResult = 0;
	pResult.btWeaponType = 2;
	pResult.btObjIndexH = SET_NUMBERH(lpNpc->m_Index);
	pResult.btObjIndexL = SET_NUMBERL(lpNpc->m_Index);

	if (gObjIsConnectedGP(lpNpc->m_iWeaponUser) == FALSE)
	{
		lpNpc->m_iWeaponUser = 0;
		lpNpc->m_btWeaponState = 1;
	}

	lpNpc->m_btWeaponState = 1;

	if (lpNpc->m_btWeaponState == 1)
	{
		pResult.btResult = 1;
		lpNpc->m_btWeaponState = 2;
		lpNpc->m_iWeaponUser = Obj.m_Index;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		Obj.m_IfState->use = 1;
		Obj.m_IfState->type = 16;
	}
	else
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
	}

	return TRUE;
}

BOOL NpcElderCircle(CGameObject &Npc, CGameObject lpObj) //GS-CS Decompiled 100%
{
	if ((Obj.m_IfState->use) > 0)
	{
		return TRUE;
	}

	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return TRUE;
	}

	if (g_CastleSiege.GetCastleState() == 7) //Good
	{
		gGameProtocol.ChatTargetSend(lpNpc, (Lang.GetText(0, 164)), Obj.m_Index);
		return TRUE;
	}

	if (strcmp(Obj.m_PlayerData->GuildName, "") == 0) //Good
	{
		gGameProtocol.ChatTargetSend(lpNpc, (Lang.GetText(0, 165)), Obj.m_Index);
		return TRUE;
	}

	if (g_CastleSiege.CheckGuildOwnCastle(Obj.m_PlayerData->GuildName)) //Good
	{
		if (Obj.m_PlayerData->GuildStatus != 128)
		{
			if (Obj.m_PlayerData->GuildStatus != 64)
			{
				gGameProtocol.ChatTargetSend(lpNpc, (Lang.GetText(0, 165)), Obj.m_Index);
				return TRUE;
			}
		}
	}
	else
	{
		gGameProtocol.ChatTargetSend(lpNpc, (Lang.GetText(0, 165)), Obj.m_Index);
		return TRUE;
	}

	PMSG_TALKRESULT pMsg;

	pMsg.h.c = 0xC3;
	pMsg.h.headcode = 0x30;
	pMsg.h.size = sizeof(pMsg);
	pMsg.result = 0x0C;

	Obj.m_IfState->use = 1;
	Obj.m_IfState->type = 12;

	pMsg.level1 = 0;
	pMsg.level2 = 0;
	pMsg.level3 = 0;
	pMsg.level4 = 0;
	pMsg.level5 = 0;
	pMsg.level6 = 0;
	pMsg.level7 = 0;

	if (bCanChaosBox == TRUE) //Good
	{
		Obj.m_IfState->type = 7;

		Obj.bIsChaosMixCompleted = false;
		Obj.m_bCsGuildInvolved = false; //Good

		pMsg.level1 = 1; //Good

		gGameProtocol.GCAnsCsMapSvrTaxInfo(Obj.m_Index, 1, g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index));
		gObjInventoryTrans(Obj.m_Index);

		sLog->outBasic("[%s][%s] Open Chaos Box", Obj.AccountID, Obj.Name);
		gObjItemTextSave(lpObj);
		gObjWarehouseTextSave(lpObj);
	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	return TRUE;
}



BOOL NpcCastleGuard(CGameObject &Npc, CGameObject lpObj) //GS-CS Decompiled 100%
{
	if ((Obj.m_IfState->use) > 0)
	{
		return TRUE;
	}

	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return TRUE;
	}

	if (g_CastleSiege.GetCastleState() == 7)
	{
		gGameProtocol.ChatTargetSend(lpNpc, (Lang.GetText(0, 166)), Obj.m_Index);
		return TRUE;
	}

	PMSG_TALKRESULT pMsg;

	pMsg.h.c = 0xC3;
	pMsg.h.headcode = 0x30;
	pMsg.h.size = sizeof(pMsg);
	pMsg.result = 0x0D;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

	Obj.m_IfState->use = 1;
	Obj.m_IfState->type = 12;

	return TRUE;
}



BOOL NpcCastleGateLever(CGameObject &Npc, CGameObject lpObj) //GS-CS Decompiled 100%
{
	if ((Obj.m_IfState->use) > 0)
	{
		return TRUE;
	}

	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return TRUE;
	}

	BOOL bControlEnable = 0;
	BYTE btResult = 0;
	int iGateIndex = -1;

	if (g_CastleSiege.GetCastleState() == 7)
	{
		if (Obj.m_btCsJoinSide != TRUE)
		{
			btResult = 4;
			bControlEnable = FALSE;
		}
		else
		{
			bControlEnable = TRUE;
		}
	}
	else
	{
		if (!g_CastleSiege.CheckCastleOwnerMember(Obj.m_Index))
		{
			if (!g_CastleSiege.CheckCastleOwnerUnionMember(Obj.m_Index))
			{
				btResult = 4;
				bControlEnable = FALSE;
			}
			else
			{
				bControlEnable = TRUE;
			}
		}
		else
		{
			bControlEnable = TRUE;
		}
	}

	if (bControlEnable != FALSE)
	{
		if (g_CastleSiege.CheckLeverAlive(lpNpc->m_Index) == TRUE)
		{
			if (g_CastleSiege.CheckCsGateAlive(lpNpc->m_iCsGateLeverLinkIndex) == TRUE)
			{
				btResult = 1;
				iGateIndex = lpNpc->m_iCsGateLeverLinkIndex;
			}
			else
			{
				btResult = 2;
			}
		}
	}
	else
	{
		btResult = 3;
	}

	gGameProtocol.GCAnsCsGateState(Obj.m_Index, btResult, iGateIndex);

	if (btResult != 1)
	{
		return TRUE;
	}

	Obj.m_IfState->use = 1;
	Obj.m_IfState->type = 12;

	return TRUE;
}



BOOL NpcCastleCrown(CGameObject &Npc, CGameObject lpObj) //GS-CS Decompiled 100%
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return TRUE;
	}

	if (gObjIsConnected(Obj.m_Index) == FALSE) //Good
	{
		return TRUE;
	}

	if (g_CastleSiege.GetCastleState() != 7) //Good
	{
		return TRUE;
	}

	if (Obj.m_btCsJoinSide < 2 || Obj.m_bCsGuildInvolved == FALSE)
	{
		return TRUE;
	}

	if ((Obj.Y - lpNpc->Y) > 3 || (Obj.X - lpNpc->X) > 3)
	{
		return TRUE;
	}

	if (g_CastleSiege.GetRegCrownAvailable() == FALSE)
	{
		return TRUE;
	}

	if (g_CastleSiege.CheckUnionGuildMaster(Obj.m_Index) == FALSE)
	{
		return TRUE;
	}

	CGameObject &Obj = g_CastleSiege.GetCrownUserIndex();

	if (gObjIsConnected(iUserIndex) == FALSE)
	{
		int iSwitchIndex1 = g_CastleSiege.GetCrownSwitchUserIndex(217);
		int iSwitchIndex2 = g_CastleSiege.GetCrownSwitchUserIndex(218);

		if (gObjIsConnected(iSwitchIndex1) == FALSE || gObjIsConnected(iSwitchIndex2) == FALSE)
		{
			sLog->outBasic("[CastleSiege] [%s][%s] Failed to Register Castle Crown (GUILD:%s)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName);
			return TRUE;
		}

		if (Obj.m_btCsJoinSide != getGameObject(iSwitchIndex1)->m_btCsJoinSide || Obj.m_btCsJoinSide != getGameObject(iSwitchIndex2)->m_btCsJoinSide)
		{
			gGameProtocol.GCAnsCsAccessCrownState(Obj.m_Index, 4);
			sLog->outBasic("[CastleSiege] [%s][%s] Failed to Register Castle Crown (GUILD:%s) (S1:%s/%s)(S2:%s/%s)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, getGameObject(iSwitchIndex1)->Name, getGameObject(iSwitchIndex1)->m_PlayerData->GuildName, getGameObject(iSwitchIndex2)->Name, getGameObject(iSwitchIndex2)->m_PlayerData->GuildName);
			return TRUE;
		}
		else
		{
			gGameProtocol.GCAnsCsAccessCrownState(Obj.m_Index, 0);
			g_CastleSiege.SetCrownUserIndex(Obj.m_Index);
			g_CastleSiege.SetCrownAccessUserX(Obj.X);
			g_CastleSiege.SetCrownAccessUserY(Obj.Y);
			g_CastleSiege.SetCrownAccessTickCount();
			g_CastleSiege.NotifyAllUserCsProgState(0, Obj.m_PlayerData->GuildName);
			sLog->outBasic("[CastleSiege] [%s][%s] Start to Register Castle Crown (GUILD:%s)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName);
		}
	}
	else if (Obj.m_Index != iUserIndex)
	{
		gGameProtocol.GCAnsCsAccessCrownState(Obj.m_Index, 3);
	}

	return TRUE;
}



BOOL NpcCastleSwitch(CGameObject &Npc, CGameObject lpObj) //GS-CS Decompiled 100%
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return TRUE;
	}

	if (gObjIsConnected(Obj.m_Index) == FALSE)
	{
		return TRUE;
	}

	if (g_CastleSiege.GetCastleState() != 7)
	{
		return TRUE;
	}

	if (Obj.m_btCsJoinSide < 2)
	{
		return TRUE;
	}

	if ((Obj.Y - lpNpc->Y) > 3 || (Obj.X - lpNpc->X) > 3)
	{
		return TRUE;
	}

	if (g_CastleSiege.CheckGuardianStatueExist())
	{
		MsgOutput(Obj.m_Index, Lang.GetText(0, 201));
		return TRUE;
	}

	CGameObject &Obj = g_CastleSiege.GetCrownSwitchUserIndex(lpNpc->Class);

	if (!gObjIsConnected(iUserIndex))
	{
		gGameProtocol.GCAnsCsAccessSwitchState(Obj.m_Index, lpNpc->m_Index, -1, 1);
		g_CastleSiege.SetCrownSwitchUserIndex(lpNpc->Class, Obj.m_Index);
		sLog->outBasic("[CastleSiege] [%s][%s] Start to Push Castle Crown Switch (GUILD:%s) - CS X:%d/Y:%d", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, lpNpc->X, lpNpc->Y);
	}
	else if (Obj.m_Index != iUserIndex)
	{
		gGameProtocol.GCAnsCsAccessSwitchState(Obj.m_Index, lpNpc->m_Index, iUserIndex, 2);
	}

	return TRUE;
}

BOOL NpcSeedMaster(CGameObject &Npc, CGameObject lpObj)
{
	PMSG_TALKRESULT pMsg;

	pMsg.h.c = 0xC3;
	pMsg.h.headcode = 0x30;
	pMsg.h.size = sizeof(pMsg);
	pMsg.result = 0x17;
	Obj.m_IfState->type = 7;
	Obj.m_IfState->state = 0;
	Obj.m_IfState->use = 1;
	Obj.bIsChaosMixCompleted = false;

	gObjInventoryTrans(Obj.m_Index);

	gObjItemTextSave(lpObj);
	gObjWarehouseTextSave(lpObj);
	gGameProtocol.GCAnsCsMapSvrTaxInfo(Obj.m_Index, 1, g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index));

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	return TRUE;
}
BOOL NpcSeedResearcher(CGameObject &Npc, CGameObject lpObj)
{
	PMSG_TALKRESULT pMsg;

	pMsg.h.c = 0xC3;
	pMsg.h.headcode = 0x30;
	pMsg.h.size = sizeof(pMsg);
	pMsg.result = 0x18;
	Obj.m_IfState->type = 7;
	Obj.m_IfState->state = 0;
	Obj.m_IfState->use = 1;
	Obj.bIsChaosMixCompleted = false;

	gObjInventoryTrans(Obj.m_Index);

	sLog->outBasic("[SeedResearcher][%s][%s] Open Chaos Box",
		Obj.AccountID, Obj.Name);

	gObjItemTextSave(lpObj);
	gObjWarehouseTextSave(lpObj);
	gGameProtocol.GCAnsCsMapSvrTaxInfo(Obj.m_Index, 1, g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index));

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	return TRUE;
}

bool NpcTitus(CGameObject &Npc, CGameObject lpObj)
{
	if (Obj.m_IfState->use > 0)
	{
		return TRUE;
	}

	g_NewPVP.SendChannelList(*lpNpc, *lpObj);

	PMSG_TALKRESULT pResult;

	pResult.h.c = 0xC3;
	pResult.h.headcode = 0x30;
	pResult.h.size = sizeof(pResult);
	pResult.result = 0x21;
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

	Obj.m_IfState->use = 1;
	Obj.m_IfState->type = 12;

	return TRUE;
}
/*
BOOL NpcTitus(CGameObject &Npc,CGameObject lpObj)
{
#if (ENABLETEST_NEWPVP == 1)
if (Obj.m_IfState->use > 0)
{
// ... wait return
}
else
{
g_NewPVP.SendChannelList(*lpNpc, *lpObj);
PMSG_TALKRESULT pResult;
pResult.h.c = 0xC3;
pResult.h.headcode = 0x30;
pResult.h.size = sizeof(pResult);
pResult.result = 33;
DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
Obj.m_IfState->type	= 20;
Obj.m_IfState->use	= 1;
}
return TRUE;
#else
if( //g_DuelManager.SendDuelStatus(lpObj) == TRUE)
{
return TRUE;
}
return FALSE;
#endif
}
*/

bool NpcLugard(CGameObject &Npc, CGameObject lpObj)
{
	if (gObjIsConnected(lpObj) == false)
	{
		return false;
	}

	if (g_DoppelGanger.IsEventEnable() == false)
	{
		return false;
	}

	g_DoppelGanger.PlatformLugardAct(lpNpc, lpObj);
	return true;
}

bool NpcSilverBox(CGameObject &Npc, CGameObject lpObj)
{
	if (gObjIsConnected(lpObj) == false)
	{
		return false;
	}

	g_DoppelGanger.MiddleTreasureAct(lpNpc, lpObj);
	return true;
}

bool NpcGoldBox(CGameObject &Npc, CGameObject lpObj)
{
	if (gObjIsConnected(lpObj) == false)
	{
		return false;
	}

	g_DoppelGanger.LastTreasureAct(lpNpc, lpObj);
	return true;
}

bool NpcJerint(CGameObject &Npc, CGameObject lpObj)
{
	PMSG_TALKRESULT pMsg;
	PHeadSetBE((BYTE*)&pMsg, 0x30, 5);
	pMsg.result = 0x24;
	pMsg.level1 = 0;
	Obj.TargetNpcNumber = lpNpc->m_Index;
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, 5);
	return true;
}

bool NpcJulia(CGameObject &Npc, CGameObject lpObj)
{
	PMSG_TALKRESULT pMsg;
	PHeadSetBE((BYTE*)&pMsg, 0x30, 5);
	pMsg.result = 0x25;
	pMsg.level1 = 0;
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, 5);
	return true;
}

struct PMSG_ANS_GUARD_IN_CASTLE_HUNTZONE {
	/*<thisrel this+0x0>*/ /*|0x4|*/ PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x1|*/ BYTE btResult;
	/*<thisrel this+0x5>*/ /*|0x1|*/ BYTE btUsable;
	/*<thisrel this+0x8>*/ /*|0x4|*/ int iCurrentPrice;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iMaxPrice;
	/*<thisrel this+0x10>*/ /*|0x4|*/ int iUnitOfPrice;
};
// <size 0x14>

BOOL NpcCastleHuntZoneGuard(CGameObject &Npc, CGameObject lpObj)
{
	if (gObjIsConnected(Obj.m_Index) == FALSE)
	{
		return TRUE;
	}

	if ((Obj.m_IfState->use) > 0)
	{
		return TRUE;
	}

	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return TRUE;
	}

	PMSG_ANS_GUARD_IN_CASTLE_HUNTZONE pResult = { 0 };

	PHeadSubSetB((BYTE*)&pResult, 0xB9, 0x03, sizeof(PMSG_ANS_GUARD_IN_CASTLE_HUNTZONE));
	pResult.btResult = TRUE;
	pResult.iMaxPrice = 300000;
	pResult.iUnitOfPrice = 10000;
	pResult.btUsable = 0;
	pResult.iCurrentPrice = 0;

	pResult.iCurrentPrice = g_CastleSiegeSync.GetTaxHuntZone(Obj.m_Index, FALSE);

	if (g_CastleSiege.GetHuntZoneEnter())
	{
		pResult.btUsable = TRUE;
	}

	if (Obj.m_PlayerData->lpGuild != NULL)
	{
		if (g_CastleSiege.CheckCastleOwnerUnionMember(Obj.m_Index))
		{
			pResult.btResult = 2;
		}

		if (g_CastleSiege.CheckCastleOwnerMember(Obj.m_Index))
		{
			if (Obj.m_PlayerData->GuildStatus == G_MASTER)
			{
				pResult.btResult = 3;
			}
		}
	}
	else
	{
		if (g_CastleSiege.GetHuntZoneEnter())
		{
			pResult.btResult = TRUE;
		}
	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

	return TRUE;
}

bool NpcDavid(CGameObject &Npc, CGameObject lpObj)
{
	PMSG_TALKRESULT pMsg;
	pMsg.h.c = 0xC3;
	pMsg.h.headcode = 0x30;
	pMsg.h.size = sizeof(pMsg);
	pMsg.result = 0x26;
	Obj.m_IfState->type = 7;
	Obj.m_IfState->state = 0;
	Obj.m_IfState->use = 1;
	Obj.bIsChaosMixCompleted = false;

	sLog->outBasic("[LuckyItem Master][%s][%s] Open Chaos Box",
		Obj.AccountID, Obj.Name);

	gObjItemTextSave(lpObj);
	gObjWarehouseTextSave(lpObj);
	gGameProtocol.GCAnsCsMapSvrTaxInfo(Obj.m_Index, 1, g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index));
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	return FALSE;
}

bool NpcLeoHelper(CGameObject &Npc, CGameObject lpObj)
{
	if (gObjIsConnected(Obj.m_Index) == FALSE)
	{
		return TRUE;
	}

	static int msg = 0x26;

	msg++;

	PMSG_TALKRESULT pMsg;
	pMsg.h.c = 0xC3;
	pMsg.h.headcode = 0x30;
	pMsg.h.size = sizeof(pMsg);
	pMsg.result = msg;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

	sLog->outBasic("%x", msg);

	return TRUE;
}

bool NpcSantaClause(CGameObject &Npc, CGameObject lpObj)
{
	if (gObjIsConnected(Obj.m_Index) == FALSE)
	{
		return TRUE;
	}

	if (Obj.m_PlayerData->m_iResets < g_ConfigRead.data.common.XMasSantaMinReset)
	{
		return TRUE;
	}

	PMSG_REQ_SANTACHECK pSanta;
	PHeadSubSetB((BYTE*)&pSanta, 0xBE, 0x20, sizeof(pSanta));

	std::memcpy(pSanta.AccountID, Obj.AccountID, 11);
	pSanta.Obj.m_Index = Obj.m_Index;
	pSanta.gGameServerCode = g_ConfigRead.server.GetGameServerCode();

	wsDataCli.DataSend((char *)&pSanta, sizeof(pSanta));

	return TRUE;
}

bool NpcAcheronEntrance(CGameObject &Npc, CGameObject lpObj)
{
	if (gObjIsConnected(Obj.m_Index) == FALSE)
	{
		return TRUE;
	}

	Obj.TargetNumber = lpNpc->m_Index;

	PMSG_TALKRESULT pMsg;
	PHeadSetBE((BYTE*)&pMsg, 0x30, sizeof(pMsg));
	pMsg.result = 0x27;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

	sLog->outBasic("[NPC] [%s][%s] Talk with Acheron Entrance NPC", Obj.AccountID, Obj.Name);

	return TRUE;
}

bool NpcArcaWar(CGameObject &Npc, CGameObject lpObj)
{
	if (gObjIsConnected(Obj.m_Index) == FALSE)
	{
		return TRUE;
	}

	Obj.TargetNumber = lpNpc->m_Index;

	PMSG_TALKRESULT pMsg;
	PHeadSetBE((BYTE*)&pMsg, 0x30, sizeof(pMsg));
	pMsg.result = 0x28;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

	sLog->outBasic("[NPC] [%s][%s] Talk with Arca War NPC", Obj.AccountID, Obj.Name);

	return TRUE;
}

bool NpcElementalMaster(CGameObject &Npc, CGameObject lpObj)
{
	if (Obj.m_IfState->use > 0)
		return FALSE;

	if (Obj.m_bPShopOpen == true)
	{
		sLog->outBasic("[%s][%s] is Already Opening PShop, PentagramMixBox Failed",
			Obj.AccountID, Obj.Name);

		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 112), Obj.m_Index, 1);
		return TRUE;
	}

	PMSG_TALKRESULT pMsg;

	pMsg.h.c = 0xC3;
	pMsg.h.headcode = 0x30;
	pMsg.h.size = sizeof(pMsg);
	pMsg.result = 0x29;
	Obj.m_IfState->type = 7;
	Obj.m_IfState->state = 0;
	Obj.m_IfState->use = 1;
	Obj.m_PlayerData->m_bIsPentagramMixCompleted = false;

	gObjInventoryTrans(Obj.m_Index);
	g_PentagramMixSystem.PentagramMixBoxInit(lpObj);

	sLog->outBasic("[PentagramMix] [%s][%s] Pentagram Mix Start",
		Obj.AccountID, Obj.Name);

	gObjItemTextSave(lpObj);
	gObjWarehouseTextSave(lpObj);

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	return TRUE;
}

//1.01.00	-> 617C20 -> 99% (Need fix state)
bool Npc_Dialog(CGameObject &Npc, CGameObject lpObj) //-> Complete
{
	PMSG_ANS_DIG_NPC_CLICK pResult;
	char * GensName;

	if (gObjIsConnected(Obj.m_Index))
	{
		Obj.TargetNpcNumber = lpNpc->m_Index;

		switch (lpNpc->Class) // Not original, but easier
		{
		case 543:
		{
			pResult.dwContributePoint = g_GensSystem.GetContributePoint(lpObj);
			GensName = g_GensSystem.GetGensInfluenceName(lpObj);

			sLog->outBasic("[Gens System] Gens_Duprian_NPC [%s][%s] GensName [%s] Total ContributePoint [%d]", Obj.AccountID, Obj.Name, GensName, pResult.dwContributePoint);
		}
		break;

		case 544:
		{
			pResult.dwContributePoint = g_GensSystem.GetContributePoint(lpObj);
			GensName = g_GensSystem.GetGensInfluenceName(lpObj);

			sLog->outBasic("[Gens System] Gens_Vanert_NPC [%s][%s] GensName [%s] Total ContributePoint [%d]", Obj.AccountID, Obj.Name, GensName, pResult.dwContributePoint);
		}
		break;

		case 257:
		{
			Obj.m_IfState->type = 33;
			Obj.m_IfState->use = 1;
			break;
		}
		case 566:
		case 567: case 568:
		case 581:
		{
			pResult.dwContributePoint = 0;
		}
		break;
		}

		if (lpNpc->Class == 568)
		{
			Obj.m_IfState->type = 20;
			Obj.m_IfState->state = 0;
			Obj.m_IfState->use = 1;
		}

		pResult.h.c = 0xC3;
		pResult.h.headcode = 0xF9;
		pResult.h.subcode = 1;
		pResult.h.size = sizeof(pResult);
		pResult.wNPCIndex = lpNpc->Class;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		return true;
	}

	return true;
}

bool NpcShadowPhantom(int iObjIndex) //-> Complete
{
	CGameObject lpObj = &getGameObject(iObjIndex);

	if (!gObjIsConnected(Obj.m_Index))
	{
		return true;
	}

	if (Obj.Type != OBJ_USER)
	{
		return true;
	}

	if (Obj.Level > g_iShadowPhantomMaxLevel || Obj.m_PlayerData->ChangeUP == 2)
	{
		gGameProtocol.GCServerCmd(Obj.m_Index, 13, 0, 0);
		return true;
	}

	int iIncleaseAttackPower = 0;
	int iIncleaseDefensePower = 0;

	if (Obj.Level <= 180)
	{
		iIncleaseAttackPower = Obj.Level / 3 + 45;
		iIncleaseDefensePower = Obj.Level / 5 + 50;
	}
	else
	{
		iIncleaseAttackPower = 105;
		iIncleaseDefensePower = 86;
	}

	gObjAddBuffEffect(lpObj, BUFFTYPE_ATTDEF_POWER_INC, EFFECTTYPE_IMPROVE_DAMAGE, iIncleaseAttackPower, EFFECTTYPE_IMPROVE_DEFENSE, iIncleaseDefensePower, (Obj.Level / 6 + 30) * 60);
	return true;
}

bool NpcMarketWall(CGameObject &Npc, CGameObject lpObj)
{
	if (!gObjIsConnected(Obj.m_Index))
	{
		return false;
	}

	PMSG_TALKRESULT pResult;

	pResult.h.c = 0xC3;
	pResult.h.headcode = 0x30;
	pResult.h.size = sizeof(pResult);
	pResult.result = 0x30;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

	return true;
}

bool NpcMonicaTalk(CGameObject &Npc, CGameObject lpObj)
{
	if (!gObjIsConnected(Obj.m_Index))
		return false;

	if (Obj.m_IfState->use > 0)
		return false;

	PMSG_TALKRESULT pResult;

	pResult.h.c = 0xC3;
	pResult.h.headcode = 0x30;
	pResult.h.size = sizeof(pResult);
	pResult.result = 0x33;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
	return true;
}
bool NpcWerewolfQuarrel(CGameObject &Npc, CGameObject lpObj)
{
	if (gObjIsConnected(Obj.m_Index))
	{
		Obj.TargetNpcNumber = lpNpc->m_Index;
	}

	return true;
}

bool NpcGateKeeper(CGameObject &Npc, CGameObject lpObj)
{
	if (gObjIsConnected(Obj.m_Index))
	{
		Obj.TargetNpcNumber = lpNpc->m_Index;
	}

	return true;
}



////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

