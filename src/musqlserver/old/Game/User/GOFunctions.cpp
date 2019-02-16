#include "GOFunctions.h"
#include "CGameObject.h"
#include "GameServer.h"
#include "PrecompiledHeader/gameGlobals.h"
#include "DoppelGanger.h"
#include "MapServerManager.h"
#include "gObjMonster.h"
#include "ObjCalCharacter.h"
#include "ObjUseSkill.h"
#include "DarkSpirit.h"
#include "DevilSquare.h"
#include "ImperialGuardian.h"
#include "MonsterAttr.h"
#include "SendNPCInfo.h"
#include "Authorization.h"
#include "ChaosBox.h"
#include "PacketChecksum.h"
#include "QuestInfo.h"
#include "EledoradoEvent.h"
#include "TNotice.h"
#include "GMMng.h"
#include "zzzmathlib.h"
#include "Gate.h"
#include "ObjAttack.h"
#include "SProtocol.h"
#include "MultiAttackHAckCheck.h"
#include "BattleSoccer.h"
#include "BattleSoccerManager.h"
#include "TUnion.h"
#include "TMonsterAIElement.h"
#include "Weapon.h"
#include "CrywolfSync.h"
#include "Kanturu.h"
#include "CashShop.h"
#include "Mercenary.h"
#include "KalimaGate.h"
#include "KanturuBattleUserMng.h"
#include "Guardian.h"
#include "MoveCommand.h"
#include "StatMng.h"
#include "SetItemOption.h"
#include "SkillAdditionInfo.h"
#include "PeriodItemEx.h"
#include "TMonsterSkillManager.h"
#include "IllusionTempleEvent_Renewal.h"
#include "BuffEffect.h"
#include "BuffEffectSlot.h"
#include "MasterSkillSystem.h"
#include "PentagramMixSystem.h"
#include "MasterLevelSkillTreeSystem.h"
#include "Crywolf.h"
#include "CrywolfStatue.h"
#include "CrywolfAltar.h"
#include "LifeStone.h"
#include "GuardianStatue.h"
#include "CannonTower.h"
#include "CastleCrown.h"
#include "CastleCrownSwitch.h"
#include "CastleSiegeSync.h"
#include "Raklion.h"
#include "RaklionUtil.h"
#include "RaklionBattleUserMng.h"
#include "RaklionMonsterMng.h"
#include "ItemSocketOptionSystem.h"
#include "ChaosCastle.h"
#include "LastManStanding.h"
#include "MapAttribute.h"
#include "GensSystem.h"
#include "NewPVP.h"
#include "VipSys.h"
#include "ExpManager.h"
#include "QuestExpProgMng.h"
#include "QuestExpInfo.h"
#include "LuckyItemManager.h"
#include "PentagramSystem.h"
#include "MuunSystem.h"
#include "ChaosCastleFinal.h"
#include "MuRummyMng.h"
#include "MineSystem.h"
#include "Shop.h"
#include "PersonalStore.h"
#include "LargeRand.h"
#include "AcheronGuardianEvent.h"
#include "ItemOptionTypeMng.h"
#include "BonusEvent.h"
#include "CItemDrop.h"
#include "DevilSquareFinal.h"
#include "BotSystem.h"
#include "OfflineLevelling.h"
#include "EvolutionMonsterMng.h"
#include "UserDieItemDrop.h"
#include "UserChatBlockMng.hpp"
#include "CustomMaxStats.h"
#include "SocketItemType.h"
#include "GOEventFunctions.h"
#include "ArcaBattle.h"
#include "JewelOfHarmonySystem.h"
#include "CastleSiege.h"
#include "MonsterSetBase.h"
#include "EDSprotocol.h"
#include "Sprotocol.h"
#include "gameGlobals.h"

CItemObject* pTempInventory;
BYTE* pTempInventoryMap;

void gObjSkillUseProc(CGameObject &Obj)
{
	if (Obj.m_SkillInfo->RemedyOfLoveTime > 0)
	{
		Obj.m_SkillInfo->RemedyOfLoveTime--;

		if (Obj.m_SkillInfo->RemedyOfLoveTime <= 0)
		{
			Obj.m_SkillInfo->RemedyOfLoveEffect = 0;
			Obj.m_SkillInfo->RemedyOfLoveTime = 0;
		}
	}

	if (Obj.m_SkillInfo->LordSummonTime != 0 && Obj.Type == OBJ_USER)
	{
		Obj.m_SkillInfo->LordSummonTime--;

		if (Obj.m_SkillInfo->LordSummonTime <= 0)
		{
			Obj.m_SkillInfo->LordSummonTime = 0;

			int m_check1 = gMoveCommand.CheckMainToMove(Obj);
			int m_check2 = gMoveCommand.CheckEquipmentToMove(Obj, Obj.m_SkillInfo->LordSummonMapNumber);
			int m_check3 = gMoveCommand.CheckInterfaceToMove(Obj);

			if (m_check1 == false || m_check2 == false || m_check3 == false)
			{
				char szMsg[256];
				wsprintf(szMsg, Lang.GetText(0, 146));

				gGameProtocol.GCServerMsgStringSend(szMsg, Obj, 1);
			}
			else
			{
				gObjRecall(Obj, Obj.m_SkillInfo->LordSummonMapNumber, Obj.m_SkillInfo->LordSummonX, Obj.m_SkillInfo->LordSummonY);
			}
			//season4 add-on (fix)
			Obj.m_SkillInfo->LordSummonMapNumber = 0;
			Obj.m_SkillInfo->LordSummonTime = 0;
			Obj.m_SkillInfo->LordSummonX = 0;
			Obj.m_SkillInfo->LordSummonY = 0;
		}
		else if (Obj.m_SkillInfo->LordSummonTime <= 5 && Obj.m_SkillInfo->LordSummonTime != 0)
		{
			char szMsg[256];
			wsprintf(szMsg, Lang.GetText(0, 135), Obj.m_SkillInfo->LordSummonTime);

			gGameProtocol.GCServerMsgStringSend(szMsg, Obj, 1);
		}
	}

	TMonsterSkillElementInfo::CheckSkillElementInfoProc(Obj);

	if (IT_MAP_RANGE(Obj.MapNumber)) //season 2.5 add-on
	{
		if (Obj.m_nITR_Index != -1)
		{
			g_IT_Event.EventSkillProc(Obj);
		}
	}

	if (Obj.m_SkillNumber < 1)
	{
		return;
	}

	if (Obj.m_SkillNumber == 18)
	{
		if (GetTickCount() > Obj.m_SkillTime)
		{
			Obj.m_SkillNumber = 0;
		}
	}
}

void gObjSkillUseProcTime500(CGameObject &Obj)
{
	if (Obj.Type != OBJ_USER)
		return;

	if (Obj.m_PlayerData->SkillHellFire2State != 0)
	{
		unsigned long dwCurrentTick = GetTickCount();
		Obj.m_PlayerData->SkillHellFire2Count = (dwCurrentTick - Obj.m_PlayerData->SkillHellFire2Time) / 500;

		if (Obj.m_PlayerData->SkillHellFire2Count > 12)
		{
			Obj.m_PlayerData->SkillHellFire2Count = 12;
		}

		if (dwCurrentTick >= (Obj.m_PlayerData->SkillHellFire2Time + 6000))
		{
			CMagicInf * lpMagic = gObjGetMagicSearch(Obj, 40);

			if (lpMagic)
			{
				gObjUseSkill.RunningSkill(Obj, 0, lpMagic, 0);
			}
		}
		else
		{
			int mana = MagicDamageC.SkillGetMana(40);
			mana = mana * 20 / 100;
			BOOL bSuccess = TRUE;

			if (mana > Obj.Mana)
			{
				bSuccess = FALSE;
				int count = 0;

				while (true)
				{
					count++;

					if (gObjIsConnected(Obj) == FALSE)
					{
						break;
					}

					if (count > 100)
					{
						break;
					}

					int pos = gObjGetManaItemPos(Obj);

					if (pos == -1)
					{
						break;
					}

					PMSG_USEITEM pMsg;
					pMsg.inventoryPos = pos;
					pMsg.invenrotyTarget = 0;
					gGameProtocol.CGUseItemRecv(&pMsg, Obj);

					if (mana <= Obj.Mana)
					{
						bSuccess = TRUE;
						break;
					}
				}
			}

			if (bSuccess == FALSE)
			{
				CMagicInf * lpMagic = gObjGetMagicSearch(Obj, 40);

				if (lpMagic)
				{
					gObjUseSkill.RunningSkill(Obj, 0, lpMagic, 0);
				}
			}
			else
			{
				Obj.Mana -= mana;

				if (Obj.Mana < 0.0f)
				{
					Obj.Mana = 0.0f;
				}

				gGameProtocol.GCManaSend(Obj, Obj.Mana, -1, 0, Obj.BP);
				PMSG_EX_SKILL_COUNT pMsg;
				PHeadSetB((BYTE*)&pMsg, 0xBA, sizeof(pMsg));
				pMsg.Type = 40;
				pMsg.Count = Obj.m_PlayerData->SkillHellFire2Count;
				pMsg.NumberH = SET_NUMBERH(Obj.m_Index);
				pMsg.NumberL = SET_NUMBERL(Obj.m_Index);

				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR *)&pMsg, pMsg.h.size);
				gGameProtocol.MsgSendV2(Obj, (UCHAR *)&pMsg, pMsg.h.size);
			}
		}
	}

	if (Obj.m_PlayerData->SkillStrengthenHellFire2State != 0)
	{
		unsigned long dwCurrentTick = GetTickCount();
		Obj.m_PlayerData->SkillStrengthenHellFire2Count = (dwCurrentTick - Obj.m_PlayerData->SkillStrengthenHellFire2Time) / 500;

		if (Obj.m_PlayerData->SkillStrengthenHellFire2Count > 12)
		{
			Obj.m_PlayerData->SkillStrengthenHellFire2Count = 12;
		}

		if (dwCurrentTick >= (Obj.m_PlayerData->SkillStrengthenHellFire2Time + 6000))
		{
			CMagicInf * lpMagic = gObjGetMagicSearch(Obj, 392);

			if (lpMagic)
			{
				gObjUseSkill.RunningSkill(Obj, 0, lpMagic, 0);
			}
		}
		else
		{
			int mana = MagicDamageC.SkillGetMana(392);
			mana = mana * 20 / 100;
			BOOL bSuccess = TRUE;

			if (mana > Obj.Mana)
			{
				bSuccess = FALSE;
				int count = 0;

				while (true)
				{
					count++;

					if (gObjIsConnected(Obj) == FALSE)
					{
						break;
					}

					if (count > 100)
					{
						break;
					}

					int pos = gObjGetManaItemPos(Obj);

					if (pos == -1)
					{
						break;
					}

					PMSG_USEITEM pMsg;
					pMsg.inventoryPos = pos;
					pMsg.invenrotyTarget = 0;
					gGameProtocol.CGUseItemRecv(&pMsg, Obj);

					if (mana <= Obj.Mana)
					{
						bSuccess = TRUE;
						break;
					}
				}
			}

			if (bSuccess == FALSE)
			{
				CMagicInf * lpMagic = gObjGetMagicSearch(Obj, 392);

				if (lpMagic)
				{
					gObjUseSkill.RunningSkill(Obj, 0, lpMagic, 0);
				}
			}
			else
			{
				Obj.Mana -= mana;

				if (Obj.Mana < 0.0f)
				{
					Obj.Mana = 0.0f;
				}

				gGameProtocol.GCManaSend(Obj, Obj.Mana, -1, 0, Obj.BP);
				PMSG_EX_SKILL_COUNT pMsg;
				PHeadSetB((BYTE*)&pMsg, 0xBA, sizeof(pMsg));
				pMsg.Type = 392;
				pMsg.Count = Obj.m_PlayerData->SkillStrengthenHellFire2Count;
				pMsg.NumberH = SET_NUMBERH(Obj.m_Index);
				pMsg.NumberL = SET_NUMBERL(Obj.m_Index);

				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR *)&pMsg, pMsg.h.size);
				gGameProtocol.MsgSendV2(Obj, (UCHAR *)&pMsg, pMsg.h.size);
			}
		}
	}
}

void MonsterAndMsgProc()
{
	CGameObject* lpObj;
	int n;
	int a_index;

	for each (std::pair<int, CGameObject*> ObjEntry in gGameObjects)
	{
		CGameObject* lpObj = ObjEntry.second;

		if (lpObj->Connected == PLAYER_PLAYING)
		{
			if (lpObj->Type == OBJ_MONSTER || lpObj->Type == OBJ_NPC)
			{
				if (lpObj->m_iCurrentAI != 0)
				{
					continue;
				}
				gObjMonsterProcess(*lpObj);
			}

			else if (lpObj->Type == OBJ_USER)
			{
				gObjSkillUseProcTime500(*lpObj);
				gObjMsgProc(*lpObj);
				CreateFrustrum(lpObj->X, lpObj->Y, *lpObj);
			}

			if (lpObj->Type == OBJ_USER && lpObj->m_PlayerData->ISBOT == false && n >= g_ConfigRead.server.GetObjectStartUserIndex())
			{
				gDarkSpirit[n - g_ConfigRead.server.GetObjectStartUserIndex()].Run();
			}
		}
		else if (lpObj->Connected >= PLAYER_LOGGED)
		{
			if (lpObj->Type == OBJ_USER)
			{
				gObjMsgProc(*lpObj);

				if (lpObj->Connected == PLAYER_PLAYING)
				{
					CreateFrustrum(lpObj->X, lpObj->Y, *lpObj);
				}
			}
		}

		else
		{
			gObjMsgProc(*lpObj);
		}
	}

	for each (std::pair<int, CGameObject*> ObjEntry in gGameObjects)
	{
		CGameObject* lpObj = ObjEntry.second;

		if (lpObj->Connected == PLAYER_PLAYING)
		{
			for (int i = 0; i < MAX_MONSTER_SEND_ATTACK_MSG; i++)
			{
				if (gSMAttackProcMsg[n][i].MsgCode >= 0)
				{
					if (GetTickCount64() > gSMAttackProcMsg[n][i].MsgTime)
					{
						gObjStateAttackProc(*lpObj, gSMAttackProcMsg[n][i].MsgCode, *getGameObject(gSMAttackProcMsg[n][i].SendUser),
							gSMAttackProcMsg[n][i].SubCode, gSMAttackProcMsg[n][i].SubCode2);
						gSMAttackProcMsg[n][i].MsgCode = -1;
					}
				}
			}
		}
	}
	TMonsterSkillManager::MonsterSkillProc();
}

void MoveMonsterProc()
{
	DWORD MoveTime;
	int DelayTime;
	CGameObject* lpObj;

	for each (std::pair<int, CGameObject*> ObjEntry in gGameObjects)
	{
		CGameObject* lpObj = ObjEntry.second;

		if (lpObj->m_iCurrentAI != 0)
		{
			if (lpObj->Type == OBJ_MONSTER || lpObj->Type == OBJ_NPC)
			{
				continue;
			}
		}

		if (DG_MAP_RANGE(lpObj->MapNumber))
		{
			continue;
		}

		if (lpObj->Connected == PLAYER_PLAYING)
		{
			if (lpObj->Type == OBJ_MONSTER)
			{
				if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
				{
					if (ATTRIBUTE_RANGE(lpObj->m_Attribute) != FALSE)
					{
						CreateFrustrum(lpObj->X, lpObj->Y, *lpObj);
						continue;
					}
				}

				else
				{
					if (ATTRIBUTE_RANGE(lpObj->m_Attribute) != FALSE || lpObj->Class == 277 || lpObj->Class == 283 || lpObj->Class == 288 || lpObj->Class == 278 || lpObj->Class == 216 || lpObj->Class == 217 || lpObj->Class == 218 || lpObj->Class == 219 || (CRYWOLF_ALTAR_CLASS_RANGE(lpObj->Class) != FALSE || CRYWOLF_STATUE_CHECK(lpObj->Class) != FALSE))
					{
						CreateFrustrum(lpObj->X, lpObj->Y, *lpObj);
						continue;
					}
				}
			}

			if (lpObj->m_State == 2)
			{
				if (lpObj->PathCount != 0)
				{
					if (lpObj->DelayLevel != 0)
					{
						DelayTime = 300;
					}
					else
					{
						DelayTime = 0;
					}

					if (lpObj->Type == OBJ_MONSTER && lpObj->m_RecallMon >= 100)
					{
						lpObj->m_MoveSpeed = 200;
					}
					else
					{
						lpObj->m_MoveSpeed = 400;
					}

					if (lpObj->PathDir[lpObj->PathCur] % 2 == 0)
					{
						MoveTime = (lpObj->m_MoveSpeed + DelayTime)*(double)1.3;
					}
					else
					{
						MoveTime = lpObj->m_MoveSpeed + DelayTime;
					}

					if ((GetTickCount() - lpObj->PathTime) > MoveTime && lpObj->PathCur < 14)
					{
						int nextX = lpObj->PathX[lpObj->PathCur];
						int nextY = lpObj->PathY[lpObj->PathCur];
						BYTE mapnumber = lpObj->MapNumber;
						BYTE attr = MapC[mapnumber].GetAttr(nextX, nextY);

						if (lpObj->Type == OBJ_USER &&
							((attr & 4) == 4 || (attr & 8) == 8))
						{
							sLog->outBasic("[ CHECK POSITION ] MoveMosterProc [%s][%s] Map[%d]-(%d,%d) User(%d,%d) Can not Move Position Attr[%d]",
								lpObj->AccountID, lpObj->Name, lpObj->MapNumber, nextX, nextY, lpObj->X, lpObj->Y, attr);
							for (int n = 0; n < 15; n++)
							{
								lpObj->PathX[n] = 0;
								lpObj->PathY[n] = 0;
								lpObj->PathOri[n] = 0;
							}

							lpObj->PathCount = 0;
							lpObj->PathCur = 0;
							lpObj->PathTime = GetTickCount();

							if (lpObj->Type == OBJ_MONSTER || lpObj->Type == OBJ_NPC)
							{
								lpObj->PathStartEnd = 0;
							}

							if (lpObj->Type == OBJ_USER)
							{
								gObjSetPosition(*lpObj, lpObj->X, lpObj->Y);
							}
						}
						else
						{
							lpObj->X = lpObj->PathX[lpObj->PathCur];
							lpObj->Y = lpObj->PathY[lpObj->PathCur];
							lpObj->Dir = lpObj->PathDir[lpObj->PathCur];

							lpObj->PathTime = GetTickCount();
							lpObj->PathCur++;

							if (lpObj->PathCur >= lpObj->PathCount)
							{
								lpObj->PathCur = 0;
								lpObj->PathCount = 0;

								if (lpObj->Type == OBJ_MONSTER || lpObj->Type == OBJ_NPC)
								{
									lpObj->PathStartEnd = 0;
								}
							}
						}
					}
				}
				CreateFrustrum(lpObj->X, lpObj->Y, *lpObj);
			}
		}
	}
}


int gObjInitGameObject(CGameObject* gameObject)
{
	gameObject->Type = (BYTE)OBJ_EMPTY;
	gameObject->m_socket = INVALID_SOCKET;
	gameObject->PathTime = GetTickCount();
	gameObject->m_RecallMon = -1;
	gameObject->m_lpMagicBack = new CMagicInf[MAGIC_SIZE];
	gameObject->Magic = new CMagicInf[MAGIC_SIZE];
	gameObject->VpPlayer = new VIEWPORT_STRUCT[MAX_VIEWPORT];
	gameObject->VpPlayer2 = new VIEWPORT_PLAYER_STRUCT[MAX_VIEWPORT];
	//gameObject->DurMagicKeyChecker = new TDurMagicKeyChecker();
	gameObject->sHD = new HITDAMAGE_STRUCT[MAX_ST_HIT_DAMAGE];
	//gameObject->m_Agro = new TMonsterAIAgro();
	gameObject->MapNumber = MAP_INDEX_RORENCIA;

	if (gameObject->m_Index >= g_ConfigRead.server.GetObjectStartUserIndex())
	{
		gameObject->m_PlayerData = new CUserData();
		gameObject->pntInventory1 = new CItemObject*[INVENTORY_SIZE];
		gameObject->pntInventory2 = new CItemObject*[INVENTORY_SIZE];
		gameObject->InventoryMap1 = new LPBYTE[INVENTORY_MAP_SIZE];
		gameObject->InventoryMap2 = new LPBYTE[INVENTORY_MAP_SIZE];
		gameObject->pntMuunInventory1 = new CItemObject*[MUUN_INVENTORY_SIZE];
		gameObject->pntMuunInventory2 = new CItemObject*[MUUN_INVENTORY_SIZE];
		gameObject->pntEventInventory1 = new CItemObject*[EVENT_INVENTORY_SIZE];
		gameObject->pntEventInventory2 = new CItemObject*[EVENT_INVENTORY_SIZE];
		gameObject->EventInventoryMap1 = new LPBYTE[EVENT_INVENTORY_MAP_SIZE];
		gameObject->EventInventoryMap2 = new LPBYTE[EVENT_INVENTORY_MAP_SIZE];
	}

	else
	{
		gameObject->pntInventory1 = &pTempInventory;
		gameObject->pntInventory2 = &pTempInventory;
		gameObject->pntMuunInventory1 = &pTempInventory;
		gameObject->pntMuunInventory2 = &pTempInventory;
		gameObject->pntEventInventory1 = &pTempInventory;
		gameObject->pntEventInventory2 = &pTempInventory;
		gameObject->InventoryMap1 = &pTempInventoryMap;
		gameObject->InventoryMap2 = &pTempInventoryMap;
		gameObject->EventInventoryMap1 = &pTempInventoryMap;
		gameObject->EventInventoryMap2 = &pTempInventoryMap;
	}

	if (gameObject->m_Index >= g_ConfigRead.server.GetObjectStartUserIndex())
	{
		gameObject->pntTrade = new CItemObject*[TRADE_BOX_SIZE];
		gameObject->TradeMap = new BYTE[TRADE_BOX_MAP_SIZE];
		gameObject->pntWarehouse = new CItemObject*[WAREHOUSE_SIZE];
		gameObject->WarehouseMap = new BYTE[WAREHOUSE_SIZE];
		gameObject->pntChaosBox = new CItemObject*[CHAOS_BOX_SIZE];
		gameObject->ChaosBoxMap = new BYTE[CHAOS_BOX_MAP_SIZE];
	}

	gameObject->WarehouseCount = 0;
	gameObject->m_bGMSummon = false;
	gameObject->m_btOpenWarehouse = FALSE;

	if (gameObject->m_Index >= g_ConfigRead.server.GetObjectStartUserIndex())
	{
		gameObject->m_PlayerData->ConnectUser->PerSocketContext = new _PER_SOCKET_CONTEXT;
	}
}


int gObjInit()
{
	try
	{
		if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
		{
			MaxViewportMonster = 75;
		}


		CGameObject* gameObject = new CGameObject[g_ConfigRead.server.GetObjectMax()];
		m_ObjBill = new BILL_CLASS[g_ConfigRead.server.GetObjectMaxUser()];
		gMultiAttackHackCheck = new CMultiAttackHackCheck[g_ConfigRead.server.GetObjectMax()];

		gDarkSpirit = new CDarkSpirit[g_ConfigRead.server.GetObjectMaxUser()];
		//gGameObjects.size() = g_ConfigRead.server.GetObjectStartUserIndex();
		//gObjMonCount = 0;
		//gObjCallMonCount = g_ConfigRead.server.GetObjectMaxMonster();

		//gSMMsg = new MessageStateMachine*[g_ConfigRead.server.GetObjectMax()];
		//gSMAttackProcMsg = new ExMessageStateMachine*[g_ConfigRead.server.GetObjectMax()];

		for each (std::pair<int, CGameObject*> ObjEntry in gGameObjects)
		{
			CGameObject* lpObj = ObjEntry.second;

			gSMMsg[lpObj->m_Index] = new MessageStateMachine[MAX_MONSTER_SEND_MSG];
			gSMAttackProcMsg[lpObj->m_Index] = new ExMessageStateMachine[MAX_MONSTER_SEND_ATTACK_MSG];
		}

		CItemObject* pTempInventory = new CItemObject[INVENTORY_SIZE];
		LPBYTE pTempInventoryMap = new BYTE[INVENTORY_MAP_SIZE];

		for each (std::pair<int, CGameObject*> ObjEntry in gGameObjects)
		{
			gObjInitGameObject(ObjEntry.second);

			InitializeCriticalSection(&gameObject->m_critPShopTrade);
		}

		if (gameObject->Type == OBJ_USER)
		{
			gObjAdd(gameObject->m_PlayerData->ConnectUser->socket, gameObject->m_PlayerData->ConnectUser->IP, *gameObject);
		}
	}

	catch (std::exception e)
	{
		sLog->outError("%s", e.what());
		return -1;
	}

	InitFrustrumEx();
	return 1;
}

void gObjEnd()
{
	gObjAllDisconnect();
	gObjAllLogOut();

	for each (std::pair<int, CGameObject*> ObjEntry in gGameObjects)
	{
		CGameObject* lpObj = ObjEntry.second;
		delete[] gSMMsg[lpObj->m_Index];
		delete[] gSMAttackProcMsg[lpObj->m_Index];	
	}

	delete[] m_ObjBill;
	delete[] gDarkSpirit;
	delete[] gMultiAttackHackCheck;
	delete[] pTempInventory;
	delete[] pTempInventoryMap;
	//delete CharSaveLog;

	for each (std::pair<int, CGameObject*> ObjEntry in gGameObjects)
	{
		CGameObject* lpObj = ObjEntry.second;
		delete[] lpObj->m_lpMagicBack;
		delete[] lpObj->Magic;
		delete[] lpObj->VpPlayer;
		delete[] lpObj->VpPlayer2;
		delete[] lpObj->sHD;

		if (lpObj->m_PlayerData != NULL)
		{
			delete lpObj->m_PlayerData;
			lpObj->m_PlayerData = NULL;
		}

		delete[] lpObj->pntChaosBox;
		delete[] lpObj->ChaosBoxMap;
		delete[] lpObj->pntInventory1;
		delete[] lpObj->pntInventory2;
		delete[] lpObj->InventoryMap1;
		delete[] lpObj->InventoryMap2;
		delete lpObj->m_PlayerData->ConnectUser->PerSocketContext;
		delete[] lpObj->pntTrade;
		delete[] lpObj->TradeMap;
		delete[] lpObj->pntWarehouse;
		delete[] lpObj->WarehouseMap;
		delete[] lpObj->pntMuunInventory1;
		delete[] lpObj->pntMuunInventory2;
		delete[] lpObj->pntEventInventory1;
		delete[] lpObj->pntEventInventory2;
		delete[] lpObj->EventInventoryMap1;
		delete[] lpObj->EventInventoryMap2;
	}
}

void gObjClearViewportOfMine(CGameObject &Obj)
{
	for (int i = 0; i < MAX_VIEWPORT; i++)
	{
		Obj.VpPlayer[i].state = 0;
		Obj.VpPlayer[i].number = -1;
	}

	Obj.VPCount = 0;
}

void gObjClearViewport(CGameObject &Obj)
{
	for (int i = 0; i < MAX_VIEWPORT; i++)
	{
		Obj.VpPlayer[i].state = 0;
		Obj.VpPlayer[i].number = -1;
		Obj.VpPlayer2[i].state = 0;
		Obj.VpPlayer2[i].number = -1;
	}

	Obj.VPCount = 0;
	Obj.VPCount2 = 0;
}
void gObjCloseSet(CGameObject &Obj, int Flag)
{
	if (Obj.CloseCount > 0)
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.Connected == PLAYER_PLAYING)
	{

		if (BC_MAP_RANGE(Obj.MapNumber))
		{
			g_BloodCastle.SearchUserDropQuestItem(Obj);
		}

		if (IT_MAP_RANGE(Obj.MapNumber))
		{
			g_IT_Event.DropRelicsItem(Obj.MapNumber, Obj);
			g_IT_Event.Leave_ITR(Obj, Obj.MapNumber);

			if (Obj.m_nITR_RelicsTick > 0 || Obj.m_wITR_NpcType > 0 || Obj.m_byITR_StoneState != 99 || Obj.m_bITR_RegisteringRelics == true || Obj.m_bITR_GettingRelics == true)
			{
				g_IT_Event.CancleStoneState(Obj.m_wITR_NpcType, Obj.m_byITR_StoneState, Obj.MapNumber);
				Obj.m_nITR_RelicsTick = 0;
				Obj.m_wITR_NpcType = 0;
				Obj.m_byITR_StoneState = 99;
				Obj.m_bITR_RegisteringRelics = false;
				Obj.m_bITR_GettingRelics = false;
			}
		}

		if (Obj.MapNumber == MAP_INDEX_DEVILSQUARE_FINAL)
		{
			g_DevilSquareFinal.Leave_DSF(Obj);
		}

		if (DG_MAP_RANGE(Obj.MapNumber))
		{
			g_DoppelGanger.LeaveDoppelganger(Obj);

			if (g_DoppelGanger.GetDoppelgangerState() == 2)
			{
				g_DoppelGanger.SendDoppelgangerResult(Obj, 1);
			}
		}

		if ((GetTickCount() - Obj.MySelfDefenseTime) < 60000)
		{
			gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 42), Obj, 1);
			gGameProtocol.GCCloseMsgSend(Obj, -1);
			return;
		}

		if ((Obj.m_IfState->use != 0) && ((Obj.m_IfState->type == 1) || (Obj.m_IfState->type == 6) || (Obj.m_IfState->type == 13) || (Obj.m_IfState->type == 7)))
		{
			gObjSaveChaosBoxItemList(Obj);
			switch (Obj.m_IfState->type)
			{
			case 1:
				gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 43), Obj, 1);
				break;

			case 6:
				gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 44), Obj, 1);
				break;

			case 7:
				gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 59), Obj, 1);
				break;

			case 13:
				gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 145), Obj, 1);
				break;
			}
			return;
		}
	}

	Obj.CloseCount = 6;
	Obj.CloseType = Flag;
	Obj.bEnableDelCharacter = 1;

}

void gObjCharTradeClear(CGameObject &Obj)
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.m_PlayerData->ISBOT == true && Obj.m_PlayerData->bt_BotType != 1)
	{
		return;
	}

	memset(Obj.TradeMap, 0xFF, TRADE_BOX_SIZE);

	for (int i = 0; i < TRADE_BOX_SIZE; i++)
	{
		Obj.pntTrade[i]->Clear();
	}

	Obj.TradeMoney = 0;
	Obj.TradeOk = false;
}

void gObjCharZeroSet(CGameObject &Obj)
{
	memset(Obj.Name, 0, MAX_ACCOUNT_LEN);

	if (Obj.Type == OBJ_USER)
	{
		g_SocketOptionSystem.ClearUserData(Obj);
		g_MasterLevelSkillTreeSystem.InitPassiveSkillData(Obj);
	}

	Obj.m_Option = 3;
	Obj.m_iScriptMaxLife = 0;
	Obj.Level = 0;

	memset(Obj.BackName, 0, sizeof(Obj.BackName));

	Obj.Life = 0;
	Obj.MaxLife = 0;
	Obj.AddLife = 0;
	Obj.AddMana = 0;
	Obj.Live = FALSE;
	Obj.Dir = 0;
	Obj.X = 0;
	Obj.Y = 0;
	Obj.TX = 0;
	Obj.TY = 0;
	Obj.MTX = 0;
	Obj.MTY = 0;
	Obj.MapNumber = MAP_INDEX_RORENCIA;
	Obj.RegenTime = 0;
	Obj.RegenOk = 0;
	Obj.lpAttackObj = 0;
	Obj.PartyTargetUser = -1;
	Obj.CloseCount = -1;
	Obj.CloseType = -1;
	Obj.m_IfState->state = -1;
	Obj.m_IfState->type = -1;
	Obj.m_IfState->use = 0;
	Obj.m_InterfaceTime = 0;
	Obj.m_ActionNumber = 0;
	Obj.m_State = 0;
	Obj.m_Rest = 0;
	Obj.m_ViewState = 0;
	Obj.DieRegen = 0;
	Obj.iMaxShield = 0;
	Obj.iShield = 0;
	Obj.iAddShield = 0;
	Obj.iFillShield = 0;
	Obj.iFillShieldMax = 0;
	Obj.iFillShieldCount = 0;
	Obj.dwShieldAutoRefillTimer = 0;
	Obj.WinDuels = 0;
	Obj.LoseDuels = 0;
	Obj.m_SkillInfo->Clear();
	Obj.m_Attribute = 0;
	Obj.m_iMonsterBattleDelay = 0;
	Obj.m_cKalimaGateExist = FALSE;
	Obj.m_iKalimaGateIndex = -1;
	Obj.m_cKalimaGateEnterCount = 0;
	Obj.TradeOk = false;
	Obj.sHDCount = 0;
	Obj.PathCount = 0;
	Obj.PathCur = 0;
	Obj.PathStartEnd = 0;
	Obj.VPCount = 0;
	Obj.VPCount2 = 0;
	Obj.m_ShopTime = 0;
	Obj.m_ManaFillCount = 0;
	Obj.FillLife = 0;
	Obj.iObjectSecTimer = 0;
	Obj.m_FriendServerOnline = FALSE;
	Obj.AddStrength = 0;
	Obj.AddDexterity = 0;
	Obj.AddVitality = 0;
	Obj.AddEnergy = 0;

	Obj.m_CurseSpell = 0;
	memset(Obj.m_AddResistance, 0, sizeof(Obj.m_AddResistance));
	Obj.DamageDecrease = 0;

#if(CUSTOM_DAMAGES12 == 1)
	Obj.S12DamageDecrease = 0;
#endif
	// muhelper
	Obj.m_bMUBOT = FALSE;
	Obj.m_iMUBOT_TIME = 0;
	Obj.m_iMUBOT_STAGE = 0;

	Obj.DamageReflect = 0;
	Obj.MonsterDieGetMoney = 0;
	Obj.MonsterDieGetLife = 0;
	Obj.MonsterDieGetMana = 0;
	Obj.m_CriticalDamage = 0;
	Obj.m_ExcelentDamage = 0;
	Obj.m_BaseDefense = 0;
	Obj.UseMagicNumber = 0;
	Obj.UseMagicTime = 0;
	Obj.UseMagicCount = 0;
	Obj.OSAttackSerial = -1;
	Obj.SASCount = 0;
	Obj.SkillAttackTime = GetTickCount();
	Obj.m_LastAttackTick = GetTickCount();
	Obj.m_LastAttackNumber = 0;
	Obj.m_AttackSpeed = 0;
	Obj.m_MagicSpeed = 0;
	Obj.m_TotalAttackTime = GetTickCount();
	Obj.m_TotalAttackCount = 0;
	Obj.CheckSumTime = GetTickCount() + 10000;
	Obj.CheckSumTableNum = -1;

	if (g_ConfigRead.MainCRC != 0)
	{
		Obj.CrcCheckTime = GetTickCount();
	}
	else
	{
		Obj.CrcCheckTime = 0;
	}

	Obj.m_SumLastAttackTime = 0;
	Obj.m_DetectCount = 0;
	Obj.m_DetectSpeedHackTime = 0.00;
	Obj.m_SpeedHackPenalty = 0;
	Obj.m_CurseDamageMin = 0;
	Obj.m_CurseDamageMax = 0;
	Obj.m_DetectedHackKickCount = 0;
	Obj.IsInBattleGround = false;
	Obj.m_dwPKTimer = GetTickCount();
	Obj.CheckTickCount = 0;
	Obj.m_AttackSpeedHackDetectedCount = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		Obj.VpPlayer[n].state = 0;
		Obj.VpPlayer2[n].state = 0;
		Obj.VpPlayer[n].number = -1;
		Obj.VpPlayer2[n].number = -1;
	}

	Obj.PartyNumber = -1;
	Obj.PartyTargetUser = -1;
	Obj.Authority = 0;
	Obj.AuthorityCode = 0;
	Obj.Penalty = 0;
	Obj.m_RecallMon = -1;
	Obj.m_Change = -1;

	Obj.LastAttackerID = -1;
	Obj.m_cChaosCastleIndex = -1;
	Obj.m_cChaosCastleSubIndex = -1;
	Obj.m_iChaosCastleBlowTime = 0;
	Obj.m_cKillUserCount = 0;
	Obj.m_cKillMonsterCount = 0;
	memset(Obj.CharSet, 0, sizeof(Obj.CharSet));

	for (int i = 0; i < MAX_SELF_DEFENSE; i++)
	{
		Obj.SelfDefense[i] = -1;
		Obj.SelfDefenseTime[i] = 0;
	}

	Obj.MySelfDefenseTime = 0;
	Obj.m_MagicDefense = 0;
	Obj.LoadWareHouseInfo = false;
	Obj.m_bAttackerKilled = false;
	Obj.m_MoveOtherServer = 0;
	Obj.m_BossGoldDerconMapNumber = -1;
	Obj.TargetNumber = 0;
	Obj.TargetNpcNumber = -1;
	Obj.WarehouseMoney = 0;
	Obj.WarehouseLock = -1;
	Obj.WarehouseCount = 0;
	Obj.WarehouseSaveLock = 0;
	Obj.WarehousePW = 0;
	Obj.WarehouseID = 0;
	Obj.WarehouseTick = 0;
	Obj.WarehouseUnfailLock = 0;
	Obj.m_bDevilSquareAuth = false;
	Obj.m_TimeCount = 0;
	Obj.EventChipCount = 0;
	Obj.MutoNumber = 0;
	Obj.HaveWeaponInHand = false;

	Obj.iStoneCount = 0;
	Obj.m_iDuelUserReserved = -1;
	Obj.m_iDuelUserRequested = -1;
	Obj.m_iDuelUser = -1;
	Obj.m_btDuelScore = 0;
	Obj.m_iDuelTickCount = 0;
	Obj.m_MaxLifePower = 0;
	Obj.AddBP = 0;
	Obj.m_CheckLifeTime = 0;
	gPacketCheckSum.ClearCheckSum(Obj);
	Obj.m_LastTeleportTime = 0;
	Obj.m_ClientHackLogCount = 0;
	Obj.UseEventServer = FALSE;
	Obj.SaveX = 0;
	Obj.SaveY = 0;
	Obj.SaveMapNumber = -1;
	Obj.DieX = 0;
	Obj.DieY = 0;
	Obj.DieMapNumber = -1;
	Obj.m_wMineCount = 0;
	Obj.m_btMiningLevel = 0;
	Obj.ChatBlockTime = 0;
	Obj.m_PostCMDLastUseTick = GetTickCount() - 10000; // so you can post as soon as you enter game instead waiting 10 sec
	gObjClearBuffEffect(Obj, CLEAR_TYPE_LOGOUT);

	if (Obj.Type == OBJ_USER && (Obj.m_PlayerData->ISBOT == false && Obj.m_PlayerData->bt_BotType != 1))
	{
		memset(Obj.WarehouseMap, (BYTE)-1, WAREHOUSE_SIZE);
		memset(Obj.TradeMap, (BYTE)-1, TRADE_BOX_SIZE);
		memset(Obj.ChaosBoxMap, (BYTE)-1, CHAOS_BOX_MAP_SIZE);
	}

	for (int i = 0; i < MAX_MAGIC; i++)
	{
		Obj.Magic[i].Clear();
	}

	Obj.MagicCount = 0;

	for (int i = 0; i < MAX_MONSTER_SEND_MSG; i++)
	{
		gSMMsg[Obj.m_Index][i].MsgCode = -1;
	}

	Obj.m_bIsInMonsterHerd = false;
	Obj.m_bIsMonsterAttackFirst = true;
	Obj.m_lpMonsterHerd = NULL;

	if ((Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()) >= 0)
	{
		for (int i = 0; i < MAX_MONSTER_SEND_ATTACK_MSG; i++)
		{
			gSMAttackProcMsg[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()][i].MsgCode = -1;
		}
	}

	Obj.Leadership = 0;
	Obj.AddLeadership = 0;
	Obj.ChatLimitTime = 0;
	Obj.ChatLimitTimeSec = 0;
	Obj.SkillLongSpearChange = false;

	Obj.bEnableDelCharacter = 1;

	if (Obj.Type == OBJ_USER && (Obj.m_PlayerData->ISBOT == false && Obj.m_PlayerData->bt_BotType != 1))
	{
		for (int i = 0; i < WAREHOUSE_SIZE; i++)
		{
			Obj.pntWarehouse[i]->Clear();
		}

		for (int i = 0; i < CHAOS_BOX_SIZE; i++)
		{
			Obj.pntChaosBox[i]->Clear();
		}
	}

	::gMultiAttackHackCheck[Obj.m_Index].Init();
	Obj.pTransaction = 0;
	::gObjMonsterHitDamageInit(Obj);
	::gObjSetInventory1Pointer(Obj);
	::gObjSetEventInventory1Pointer(Obj);
	::gObjSetMuunInventory1Pointer(Obj);

	if (Obj.Type == OBJ_USER && (Obj.m_PlayerData->ISBOT == false && Obj.m_PlayerData->bt_BotType != 1))
	{
		memset(Obj.pInventoryMap, (BYTE)-1, INVENTORY_MAP_SIZE);
	}

	gObjCharTradeClear(Obj);

	if (Obj.Type == OBJ_USER && (Obj.m_PlayerData->ISBOT == false && Obj.m_PlayerData->bt_BotType != 1))
	{
		for (int i = 0; i < WAREHOUSE_SIZE; i++)
		{
			Obj.pntWarehouse[i]->Clear();
		}

		for (int i = 0; i < INVENTORY_SIZE; i++)
		{
			Obj.pntInventory1[i]->Clear();
		}

		for (int i = 0; i < INVENTORY_SIZE; i++)
		{
			Obj.pntInventory2[i]->Clear();
		}

		for (int i = 0; i < TRADE_BOX_SIZE; i++)
		{
			Obj.pntTrade[i]->Clear();
		}

		for (int i = 0; i < MUUN_INVENTORY_SIZE; i++)
		{
			Obj.pntMuunInventory1[i]->Clear();
			Obj.pntMuunInventory2[i]->Clear();
		}

		for (int i = 0; i < 2; i++)
		{
			Obj.pntMuunEffectList[i]->Clear();
		}

		Obj.bMuunInventoryLoad = false;
		Obj.dwCheckMuunItemTime = 0;
		memset(Obj.EventInventoryMap1, -1, EVENT_INVENTORY_MAP_SIZE);
		memset(Obj.EventInventoryMap2, -1, EVENT_INVENTORY_MAP_SIZE);

		for (int i = 0; i < EVENT_INVENTORY_SIZE; i++)
		{
			Obj.pntEventInventory1[i]->Clear();
			Obj.pntEventInventory2[i]->Clear();
		}

		Obj.EventInventoryLoad = false;

		CMuRummyInfo * pCMuRummyInfo = Obj.m_PlayerData->m_pCMuRummyInfo;

		if (!pCMuRummyInfo)
		{
			sLog->outBasic("[MuRummy][Error] pCMuRummyInfo is NULL [%s][%s] [%s, %d]", Obj.AccountID, Obj.Name, __FILE__, __LINE__);
		}

		else
		{
			pCMuRummyInfo->Clear();
		}

		CEvolutionMonsterInfo * pCEvoMonInfo = Obj.m_PlayerData->m_pCEvoMonInfo;

		if (!pCEvoMonInfo)
		{
			sLog->outBasic("[EvoMon][Error] pCEvoMonInfo is NULL [%s][%s] [%s, %d]", Obj.AccountID, Obj.Name, __FILE__, __LINE__);
		}

		else
		{
			pCEvoMonInfo.Init();
		}

		Obj.m_wMuunItem = -1;
		Obj.m_wMuunSubItem = -1;
		Obj.m_wMuunRideItem = -1;
		Obj.m_wInvenPet = -1;
		Obj.m_btInvenPetPos = 0;
		Obj.bSubEquip = 0;
		Obj.m_nDSFIndex = -1;
		Obj.m_PlayerData->Init(false);
	}

	Obj.bIsChaosMixCompleted = false;
	Obj.m_bMapSvrMoveQuit = false;
	Obj.m_iCsNpcExistVal = 0;
	Obj.m_btCsNpcType = 0;
	Obj.m_btCsGateOpen = 0;
	Obj.m_iCsGateLeverLinkIndex = -1;
	Obj.m_btCsNpcDfLevel = 0;
	Obj.m_btCsNpcRgLevel = 0;
	Obj.m_btCsJoinSide = 0;
	Obj.m_bCsGuildInvolved = false;
	Obj.m_bIsCastleNPCUpgradeCompleted = 0;
	Obj.m_btWeaponState = 0;
	Obj.m_bCsGuildInvolved = false;
	Obj.m_iAccumulatedDamage = 0;
	Obj.m_iAccumulatedCrownAccessTime = 0;
	Obj.m_MonsterSkillElementInfo.Reset();
	Obj.m_iCrywolfMVPScore = 0;
	Obj.m_iSkillDistanceErrorCount = 0;
	Obj.m_dwSkillDistanceErrorTick = 0;
	memset(Obj.MarryName, 0x00, sizeof(Obj.MarryName));
	Obj.Married = 0;
	Obj.MarryRequested = 0;
	Obj.MarryRequestIndex = -1;
	Obj.MarryRequestTime = 0;
	Obj.BufferIndex = -1;
	Obj.BuffId = -1;
	Obj.BuffPlayerIndex = -1;

	Obj.m_ActionTime = 0;
	Obj.m_ActionCount = 0;
	Obj.m_ChatFloodCount = 0;
	Obj.m_ChatFloodTime = 0;
	Obj.m_bOff = false;
	Obj.m_bOffLevel = false;
	Obj.m_iPentagramAttributePattern = 0;
	Obj.m_iPentagramDefense = 0;
	Obj.m_iPentagramDamageMax = 0;
	Obj.m_iPentagramDamageMin = 0;
	Obj.m_iPentagramDamageOrigin = 1;

	Obj.m_bBlind = false;

	Obj.m_iCreatedActivationTime = 0;
	Obj.m_btLifeStoneCount = 0;
	Obj.m_btNpcType = 0;
	Obj.m_bGMSummon = false;
	Obj.m_Disappear_Monster = 0;
	Obj.m_Disappear_NewYearMonster = 0;
	Obj.m_SummonCharDBNum = 0;
	Obj.m_bIsHelpMon = false;

	Obj.m_nITR_Index = -1;
	Obj.m_nITR_RelicsTick = 0;
	Obj.m_bITR_GettingRelics = 0;
	Obj.m_bITR_RegisteringRelics = 0;
	Obj.m_wITR_NpcType = 0;
	Obj.m_byITR_StoneState = 99;
	Obj.m_dwDCTimer = 0;
	Obj.m_dwOffLevelTime = 0;
	memset(&Obj.pntBotBuffs, 0, sizeof(Obj.pntBotBuffs));
	Obj.m_nPhaseLordSilvester = 0;
	Obj.m_btOpenWarehouse = FALSE;
	Obj.m_Whispering->iWhisperSent = false;
	Obj.m_Whispering->wResponseCount = 0;
	Obj.m_nEvoMonOwnerIndex = -1;
	for (int k = 0; k < 3; k++)
		Obj.m_nRecallMonIndex[k] = -1;//

	for (int i = 0; i < EVENT_NOTIFICATION::EVENT_NOTIFICATION_MAX; ++i)
	{
		Obj.EventNotification[i] = 0;
	}

	gObjClearViewport(Obj);
}

int gObjGetSocket(SOCKET socket)
{
	for (int n = g_ConfigRead.server.GetObjectStartUserIndex(); n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		if (getGameObject(n)->Connected != PLAYER_EMPTY)
		{
			if (getGameObject(n)->m_socket == socket)
			{
				return n;
			}
		}
	}
	return -1;
}

int gObjGetHWIDUseCount(LPSTR HWID)
{
	int count = 0;

	for (int n = g_ConfigRead.server.GetObjectStartUserIndex(); n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		if (getGameObject(n)->Connected != PLAYER_EMPTY)
		{
			if (strcmp(getGameObject(n)->m_PlayerData->HWID, HWID) == 0)
			{
				if (getGameObject(n)->m_bOff == false)
				{
					count++;
				}
			}
		}
	}

	return count;
}

void gObjSetTradeOption(CGameObject &Obj, int option)
{
	if (gObjIsConnected(Obj) == TRUE)
	{
		if (option == 0)
		{
			Obj.m_Option = 0;
			gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 33), Obj, 1);
		}
		else
		{
			Obj.m_Option |= 1;
			gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 32), Obj, 1);
		}
	}
}

bool IsDuelEnable(CGameObject &Obj)
{
	if (gObjIsConnected(Obj) == TRUE)
	{
		if ((Obj.m_Option & 2) == 2)
		{
			return true;
		}
	}

	return false;
}

bool IsOnDuel(CGameObject &Obj1, CGameObject &Obj2)
{
	if (Obj1.Type != OBJ_USER || Obj2.Type != OBJ_USER) return false;
	if (Obj1.m_iDuelUser == Obj2.m_iDuelUser) return true;
	return false;
}

void gObjSetDuelOption(CGameObject &Obj, int option)
{
	if (gObjIsConnected(Obj) == TRUE)
	{
		if (option == 0)
		{
			Obj.m_Option = 0;
		}
		else
		{
			Obj.m_Option |= 2;
		}
	}
}

void DbItemSetInByte(CGameObject &Obj, struct SDHP_DBCHAR_INFORESULT* lpMsg, bool* bAllItemExist)
{
	int itype;
	int _type;
	CItemObject* item = new CItemObject();
	BYTE OptionData;
	WORD hiWord;
	WORD loWord;
	int check;
	bool bIsItemExist = true;

	for (int n = 0; n < INVENTORY_SIZE; n++)
	{
		bIsItemExist = true;
		Obj.pntInventory[n]->Clear();
		itype = lpMsg->dbInventory[n*MAX_DBITEM_INFO];

		if (lpMsg->dbInventory[n*MAX_DBITEM_INFO] == 0xFF
			&& (lpMsg->dbInventory[n*MAX_DBITEM_INFO + DBI_NOPTION_DATA] & 0x80) == 0x80
			&& (lpMsg->dbInventory[n*MAX_DBITEM_INFO + DBI_OPTION380_DATA] & 0xF0) == 0xF0)
		{
			continue;
		}

		itype |= (lpMsg->dbInventory[n*MAX_DBITEM_INFO + DBI_OPTION380_DATA] & 0xF0) * 32;
		itype |= (lpMsg->dbInventory[n*MAX_DBITEM_INFO + DBI_NOPTION_DATA] & 0x80) * 2;
		_type = itype;

		if (lpMsg->dbInventory[n*MAX_DBITEM_INFO] == 0xFF
			&& (lpMsg->dbInventory[n*MAX_DBITEM_INFO + DBI_NOPTION_DATA] & 0x80) == 0x80
			&& (lpMsg->dbInventory[n*MAX_DBITEM_INFO + DBI_OPTION380_DATA] & 0xF0) == 0xF0)
		{
			itype = -1;
		}

		if (_type == -1)
		{
			continue;
		}

		if (IsItem(_type) == false)
		{
			bIsItemExist = false;

			if (bAllItemExist != 0)
			{
				*bAllItemExist = 0;
			}
		}

		if (bIsItemExist == false)
		{
			Obj.pntInventory[n]->m_bItemExist = 0;

			Obj.pntInventory[n]->Clear();
			continue;
		}

		if (_type == ITEMGET(14, 17) || _type == ITEMGET(14, 18))
		{
			if (lpMsg->dbInventory[n*MAX_DBITEM_INFO + DBI_DUR] == 0)
			{
				itype = -1;
			}
		}

		if (_type == ITEMGET(13, 49) || _type == ITEMGET(13, 50))
		{
			if (lpMsg->dbInventory[n*MAX_DBITEM_INFO + DBI_DUR] == 0)
			{
				itype = -1;
			}
		}

		if (_type == ITEMGET(14, 19))
		{
			if (lpMsg->dbInventory[n*MAX_DBITEM_INFO + DBI_DUR] == (BYTE)-1)
			{
				itype = -1;
			}
		}

		if (itype != -1)
		{
			item->m_Level = DBI_GET_LEVEL(lpMsg->dbInventory[n*MAX_DBITEM_INFO + DBI_OPTION_DATA]);

			if (_type != ITEMGET(14, 11))
			{
				if (item->m_Level > 15)
				{
					item->m_Level = 0;
				}
			}

			OptionData = lpMsg->dbInventory[n*MAX_DBITEM_INFO + DBI_OPTION_DATA];
			item->m_Option1 = DBI_GET_SKILL(OptionData);
			item->m_Option2 = DBI_GET_LUCK(OptionData);
			item->m_Option3 = DBI_GET_OPTION(OptionData);
			item->m_Option3 |= DBI_GET_OPTION16(lpMsg->dbInventory[n*MAX_DBITEM_INFO + DBI_NOPTION_DATA]) >> 4;

			item->m_Durability = lpMsg->dbInventory[n*MAX_DBITEM_INFO + DBI_DUR];
			item->m_JewelOfHarmonyOption = lpMsg->dbInventory[n*MAX_DBITEM_INFO + DBI_JOH_DATA];
			item->m_ItemOptionEx = DBI_GET_380OPTION(lpMsg->dbInventory[n*MAX_DBITEM_INFO + DBI_OPTION380_DATA]);

			if (item->m_ItemOptionEx)
			{
				item->m_Type = itype;

				if (g_kItemSystemFor380.Is380Item(item) == false)
				{
					item->m_ItemOptionEx = 0;
					sLog->outBasic("[380Item][%s][%s] Invalid 380 Item Option in Inventory pos[%d]",
						Obj.AccountID, Obj.Name, n);
				}
			}

			BYTE SocketOption[5];
			BYTE SocketOptionIndex = 0xFF;
			memset(&SocketOption, 0xFF, 5);

			if (gSocketItemType.CheckSocketItemType(_type) == 1 || g_PentagramSystem.IsPentagramItem(_type) || g_PentagramSystem.IsPentagramJewel(_type))
			{
				for (int i = 0; i < 5; i++)
				{
					SocketOption[i] = lpMsg->dbInventory[((n*MAX_DBITEM_INFO) + DBI_SOCKET_1) + i];
				}

				SocketOptionIndex = lpMsg->dbInventory[n*MAX_DBITEM_INFO + DBI_JOH_DATA];
			}

			else
			{
				for (int i = 0; i < 5; i++)
				{
					SocketOption[i] = 0xFF;
				}

				SocketOptionIndex = 0;
			}

			item->m_PeriodItemOption = (lpMsg->dbInventory[n*MAX_DBITEM_INFO + DBI_OPTION380_DATA] & 6) >> 1;

			item->Convert(itype, item->m_Option1, item->m_Option2, item->m_Option3, DBI_GET_NOPTION(lpMsg->dbInventory[n*MAX_DBITEM_INFO + DBI_NOPTION_DATA]), lpMsg->dbInventory[n*MAX_DBITEM_INFO + DBI_SOPTION_DATA], item->m_ItemOptionEx, SocketOption, SocketOptionIndex, item->m_PeriodItemOption, CURRENT_DB_VERSION);

			if (_type == ITEMGET(14, 7))
			{

			}

			else
			{
				if (_type >= ITEMGET(14, 0) && _type <= ITEMGET(14, 8)
					|| _type >= ITEMGET(14, 38) && _type <= ITEMGET(14, 40))
				{
					if (item->m_Durability == 0.0f)
					{
						item->m_Durability = 1.0f;
					}

					if (item->m_Durability > 255.0f)
					{
						item->m_Durability = 255.0f;
					}
				}
				//Begin CashShop Items
				else if (_type == ITEMGET(14, 70) || _type == ITEMGET(14, 71))//cashshop
				{
					if (item->m_Durability == 0.0f)
					{
						item->m_Durability = 1.0f;
					}

					if (item->m_Durability > 255.0f)
					{
						item->m_Durability = 255.0f;
					}
				}

				if (_type == ITEMGET(14, 94))
				{
					if (item->m_Durability == 0.0f)
					{
						item->m_Durability = 1.0f;
					}

					if (item->m_Durability > 100.0f)
					{
						item->m_Durability = 100.0f;
					}
				}
				//new
				if (_type == ITEMGET(14, 133)) //OK
				{
					if (item->m_Durability == 0.0f)
					{
						item->m_Durability = 1.0f;
					}

					if (item->m_Durability > 50.0f)
					{
						item->m_Durability = 50.0f;
					}
				}

				if (_type == ITEMGET(14, 78) || _type == ITEMGET(14, 79)
					|| _type == ITEMGET(14, 80) || _type == ITEMGET(14, 81)
					|| _type == ITEMGET(14, 82))
				{
					if (item->m_Durability == 0.0f)
					{
						item->m_Durability = 1.0f;
					}

					if (item->m_Durability > 2.0f)
					{
						item->m_Durability = 2.0f;
					}
				}

				if (_type == ITEMGET(14, 63) || _type == ITEMGET(14, 64))
				{
					if (item->m_Durability == 0.0f)
					{
						item->m_Durability = 1.0f;
					}
				}
				//End CashShop Fucntions
				else if (_type >= ITEMGET(14, 46) && _type <= ITEMGET(14, 50))
				{
					if (item->m_Durability == 0.0f)
					{
						item->m_Durability = 1.0f;
					}

					if (item->m_Durability > 255.0f)
					{
						item->m_Durability = 255.0f;
					}
				}
				else if (_type >= ITEMGET(14, 35) && _type <= ITEMGET(14, 37))
				{
					if (item->m_Durability == 0.0f)
					{
						item->m_Durability = 1.0f;
					}

					if (item->m_Durability > 255.0f)
					{
						item->m_Durability = 255.0f;
					}
				}

				else if (_type == ITEMGET(13, 51))
				{
					if (item->m_Durability == 0.0f)
					{
						item->m_Durability = 1.0f;
					}
				}

				else if (_type != ITEMGET(13, 10) && _type != ITEMGET(14, 29) && _type != ITEMGET(14, 21))
				{
					if (item->m_Level == 3)
					{
						if (item->m_Durability > item->m_BaseDurability && bIsItemExist == 1)
						{
							item->m_Durability = item->m_BaseDurability;
						}
					}
				}
				else if (_type == ITEMGET(13, 70))
				{
					if (item->m_Durability == 1)
					{
						item->m_Durability = item->m_BaseDurability;
					}
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

			Obj.pntInventory[n]->m_Option1 = item->m_Option1;
			Obj.pntInventory[n]->m_Option2 = item->m_Option2;
			Obj.pntInventory[n]->m_Option3 = item->m_Option3;
			Obj.pntInventory[n]->m_JewelOfHarmonyOption = item->m_JewelOfHarmonyOption;
			Obj.pntInventory[n]->m_ItemOptionEx = item->m_ItemOptionEx;

			DWORD hidword = MAKE_NUMBERDW(MAKE_NUMBERW(lpMsg->dbInventory[n*MAX_DBITEM_INFO + DBI_SERIAL1], lpMsg->dbInventory[n*MAX_DBITEM_INFO + DBI_SERIAL2]), MAKE_NUMBERW(lpMsg->dbInventory[n*MAX_DBITEM_INFO + DBI_SERIAL3], lpMsg->dbInventory[n*MAX_DBITEM_INFO + DBI_SERIAL4]));
			DWORD lodword = MAKE_NUMBERDW(MAKE_NUMBERW(lpMsg->dbInventory[n*MAX_DBITEM_INFO + DBI_SERIAL5], lpMsg->dbInventory[n*MAX_DBITEM_INFO + DBI_SERIAL6]), MAKE_NUMBERW(lpMsg->dbInventory[n*MAX_DBITEM_INFO + DBI_SERIAL7], lpMsg->dbInventory[n*MAX_DBITEM_INFO + DBI_SERIAL8]));
			item->m_Number = MAKEQWORD(hidword, lodword);

			gObjInventoryInsertItemPos(Obj, *item, n, 0);

			if (item->m_Type == ITEMGET(14, 64))
			{
				gObjInventoryDeleteItem(Obj, n);
			}
			if (item->m_Type == ITEMGET(13, 107) && item->m_Durability <= 0.0f)
			{
				gObjInventoryDeleteItem(Obj, n);
			}
		}
	}
	gGameProtocol.gObjRequestPetItemInfo(Obj, 0);
}

void gObjSetBP(CGameObject &Obj)
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	int Strength = Obj.m_PlayerData->Strength + Obj.AddStrength;
	int Dexterity = Obj.m_PlayerData->Dexterity + Obj.AddDexterity;
	int Vitality = Obj.m_PlayerData->Vitality + Obj.AddVitality;
	int Energy = Obj.m_PlayerData->Energy + Obj.AddEnergy;
	int Leadership = Obj.Leadership + Obj.AddLeadership;

	switch (Obj.Class)
	{
	case CLASS_WIZARD:
		Obj.MaxBP = (Strength * 0.2) + (Dexterity * 0.4) + (Vitality * 0.3) + (Energy * 0.2);
		break;

	case CLASS_KNIGHT:
		Obj.MaxBP = (Strength * 0.15) + (Dexterity * 0.2) + (Vitality * 0.3) + (Energy * 1.0);
		break;

	case CLASS_ELF:
		Obj.MaxBP = (Strength * 0.3) + (Dexterity * 0.2) + (Vitality * 0.3) + (Energy * 0.2);
		break;

	case CLASS_MAGUMSA:
		Obj.MaxBP = (Strength * 0.2) + (Dexterity * 0.25) + (Vitality * 0.3) + (Energy * 0.15);
		break;

	case CLASS_DARKLORD:
		Obj.MaxBP = (Strength * 0.3) + (Dexterity * 0.2) + (Vitality * 0.1) + (Energy * 0.15) + (Leadership * 0.3);
		break;

	case CLASS_SUMMONER:
		Obj.MaxBP = (Strength * 0.2) + (Dexterity * 0.25) + (Vitality * 0.3) + (Energy * 0.15);
		break;

	case CLASS_RAGEFIGHTER:
		Obj.MaxBP = (Strength * 0.15) + (Dexterity * 0.2) + (Vitality * 0.3) + (Energy * 1.0);
		break;

	case CLASS_GROWLANCER:
		Obj.MaxBP = (Strength * 0.15) + (Dexterity * 0.2) + (Vitality * 0.3) + (Energy * 1.0);
		break;
	}
}

bool gObjSetCharacter(LPBYTE lpdata, CGameObject &Obj)
{
	WORD itype;
	int n;
	char szCharName[MAX_ACCOUNT_LEN + 1];
	SDHP_DBCHAR_INFORESULT * lpMsg = (SDHP_DBCHAR_INFORESULT *)lpdata;

	CGameObject* lpObj = new CGameObject();

	if (lpObj->Connected < PLAYER_LOGGED)
	{
		return FALSE;
	}

	if (lpObj->Connected == PLAYER_PLAYING)
	{
		return FALSE;
	}

	if (lpMsg->Class == 0 || lpMsg->Class == 1 || lpMsg->Class == 2 || lpMsg->Class == 16 || lpMsg->Class == 17 || lpMsg->Class == 18 || lpMsg->Class == 32 || lpMsg->Class == 33 || lpMsg->Class == 34 || lpMsg->Class == 48 || lpMsg->Class == 50 || lpMsg->Class == 64 || lpMsg->Class == 66 || lpMsg->Class == 80 || lpMsg->Class == 81 || lpMsg->Class == 82 || lpMsg->Class == 96 || lpMsg->Class == 98 || lpMsg->Class == 112 || lpMsg->Class == 114)
	{

	}
	else
	{
		return FALSE;
	}

	gObjCharZeroSet(*lpObj);

	if (g_MixSystem.ChaosBoxInit(*lpObj) == FALSE)
	{

	}

	if (g_PentagramMixSystem.PentagramMixBoxInit(lpObj) == FALSE)
	{

	}

	lpObj->m_PosNum = 0;
	lpObj->Level = lpMsg->Level;
	lpObj->m_PlayerData->LevelUpPoint = lpMsg->LevelUpPoint;
	lpObj->m_PlayerData->DbClass = lpMsg->Class;
	lpObj->Class = lpMsg->Class >> 4;
	lpObj->m_PlayerData->ChangeUP = lpMsg->Class & 0x07;	// Set Second Type of Character

	szCharName[MAX_ACCOUNT_LEN] = 0;
	std::memcpy(szCharName, lpMsg->Name, MAX_ACCOUNT_LEN);
	strcpy(lpObj->Name, szCharName);

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		std::string tmpCmpName;
		tmpCmpName.insert(0, lpObj->Name, MAX_ACCOUNT_LEN);

		strncpy(lpObj->m_PlayerData->m_RealNameOfUBF, lpObj->m_PlayerData->m_NameConvertOfUBF.ConvertNameToRealName(tmpCmpName.c_str()), MAX_ACCOUNT_LEN);
		lpObj->m_PlayerData->m_nServerCodeOfHomeWorld = lpObj->m_PlayerData->m_NameConvertOfUBF.FindServerCodeAtCharacterName(tmpCmpName.c_str());
	}

	if (lpObj->Level < 1)
	{
		return FALSE;
	}

	lpObj->X = lpMsg->MapX;
	lpObj->Y = lpMsg->MapY;
	lpObj->MapNumber = lpMsg->MapNumber;
	lpObj->StartX = lpObj->X;
	lpObj->StartY = lpObj->Y;

	if (MAX_MAP_RANGE(lpObj->MapNumber) == FALSE)
	{
		lpObj->MapNumber = MAP_INDEX_RORENCIA;
		MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, lpObj->X, lpObj->Y);
	}

	if (lpObj->Level < 6)
	{
		if (lpObj->Class == 2)
		{
			lpObj->MapNumber = MAP_INDEX_NORIA;
			MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, lpObj->X, lpObj->Y);
		}
		else if (lpObj->Class == 5)
		{
			short sX;
			short sY;
			short sLVL;
			BYTE btMAPNUM = lpObj->MapNumber;
			BYTE btDIR;

			int iMapNumber = gGateC.GetGate(267, (short &)sX, (short &)sY, (BYTE &)btMAPNUM, (BYTE &)btDIR, (short &)sLVL);

			if (iMapNumber >= 0)
			{
				lpObj->MapNumber = btMAPNUM;
				lpObj->X = sX;
				lpObj->Y = sY;
			}
		}
		else
		{
			lpObj->MapNumber = MAP_INDEX_RORENCIA;
			MapC[lpObj->MapNumber].GetLevelPos(lpObj->Level, lpObj->X, lpObj->Y);
		}
	}

	if (DS_MAP_RANGE(lpObj->MapNumber) != FALSE)
	{
		lpObj->MapNumber = MAP_INDEX_RORENCIA;
		MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, lpObj->X, lpObj->Y);
	}

	if (BC_MAP_RANGE(lpObj->MapNumber) != FALSE)
	{
		lpObj->MapNumber = MAP_INDEX_DEVIAS;
		MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, lpObj->X, lpObj->Y);
		lpObj->m_cBloodCastleIndex = -1;
		lpObj->m_cBloodCastleSubIndex = -1;
		lpObj->m_iBloodCastleEXP = 0;
		lpObj->m_bBloodCastleComplete = 0;
	}

	else
	{
		lpObj->m_cBloodCastleIndex = -1;
		lpObj->m_cBloodCastleSubIndex = -1;
		lpObj->m_iBloodCastleEXP = 0;
		lpObj->m_bBloodCastleComplete = 0;
	}

	if (lpObj->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
			lpObj->MapNumber = MAP_INDEX_LORENMARKET;
		else
			lpObj->MapNumber = MAP_INDEX_DEVIAS;

		MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, lpObj->X, lpObj->Y);
	}

	lpObj->m_bCCF_UI_Using = 0;
	lpObj->m_nCCF_BlowTime = 0;
	lpObj->m_byCCF_CanEnter = 0;
	lpObj->m_nCCF_CertiTick = 0;
	lpObj->m_nCCF_UserIndex = -1;
	lpObj->m_nCCF_BlowTime = 0;
	lpObj->m_byKillUserCount = 0;
	lpObj->m_byKillMobCount = 0;
	lpObj->m_bCCF_Quit_Msg = 0;

	if (lpObj->MapNumber == MAP_INDEX_REFUGEE)
	{
		lpObj->MapNumber = MAP_INDEX_BARRACKS;
		MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, lpObj->X, lpObj->Y);
	}

	if (lpObj->MapNumber == MAP_INDEX_ACHERON || lpObj->MapNumber == MAP_INDEX_DEBENTER || lpObj->MapNumber == MAP_INDEX_NARS || lpObj->MapNumber == MAP_INDEX_URUK_MOUNTAIN)
	{
		short sX;
		short sY;
		short sLVL;
		BYTE btMAPNUM = MAP_INDEX_ACHERON;
		BYTE btDIR;

		int iMapNumber = gGateC.GetGate(417, (short &)sX, (short &)sY, (BYTE &)btMAPNUM, (BYTE &)btDIR, (short &)sLVL);

		if (iMapNumber >= 0)
		{
			lpObj->MapNumber = btMAPNUM;
			lpObj->X = sX;
			lpObj->Y = sY;
		}
	}

	if (lpObj->MapNumber == MAP_INDEX_SANTAVILLAGE && gGateC.CheckGateLevel(*lpObj, 22) == false) //season4 add-on
	{
		if (lpObj->Class == CLASS_ELF)
		{
			lpObj->MapNumber = MAP_INDEX_NORIA;
			MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, lpObj->X, lpObj->Y);
		}
		else if (lpObj->Class == CLASS_SUMMONER)
		{
			lpObj->MapNumber = MAP_INDEX_ELBELAND;
			MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, lpObj->X, lpObj->Y);
		}
		else
		{
			lpObj->MapNumber = MAP_INDEX_RORENCIA;
			MapC[lpObj->MapNumber].GetLevelPos(lpObj->Level, lpObj->X, lpObj->Y);
		}
	}

	if (lpObj->MapNumber == MAP_INDEX_ARCA_WAR || lpObj->MapNumber == MAP_INDEX_DEBENTER_ARCA_WAR || lpObj->MapNumber == MAP_INDEX_URUK_MOUNTAIN_ARCA_WAR)
	{
		short sX;
		short sY;
		short sLVL;
		BYTE btMAPNUM = MAP_INDEX_NORIA;
		BYTE btDIR;

		int iMapNumber = gGateC.GetGate(27, (short &)sX, (short &)sY, (BYTE &)btMAPNUM, (BYTE &)btDIR, (short &)sLVL);

		if (iMapNumber >= 0)
		{
			lpObj->MapNumber = btMAPNUM;
			lpObj->X = sX;
			lpObj->Y = sY;
		}
	}

	if (IMPERIAL_MAP_RANGE(lpObj->MapNumber) != FALSE)
	{
		lpObj->MapNumber = MAP_INDEX_DEVIAS;
		MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, lpObj->X, lpObj->Y);
	}

	if (CC_MAP_RANGE(lpObj->MapNumber) != FALSE)
	{
		lpObj->MapNumber = MAP_INDEX_DEVIAS;
		MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, lpObj->X, lpObj->Y);
	}

	lpObj->m_cChaosCastleIndex = -1;
	lpObj->m_cChaosCastleSubIndex = -1;
	lpObj->m_iChaosCastleBlowTime = 0;
	lpObj->m_cKillUserCount = 0;
	lpObj->m_cKillMonsterCount = 0;

	if (IT_MAP_RANGE(lpObj->MapNumber) != FALSE)
	{
		lpObj->MapNumber = MAP_INDEX_ELBELAND;
		MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, lpObj->X, lpObj->Y);
		lpObj->m_cIllusionTempleIndex = -1;
	}

	else
	{
		lpObj->m_cIllusionTempleIndex = -1;
	}

	if (DG_MAP_RANGE(lpObj->MapNumber) != FALSE)
	{
		lpObj->MapNumber = MAP_INDEX_ELBELAND;
		MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, lpObj->X, lpObj->Y);
	}

	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		if (lpObj->MapNumber == MAP_INDEX_KANTURU_BOSS)
		{
			short sX;
			short sY;
			short sLVL;
			BYTE btMAPNUM = lpObj->MapNumber;
			BYTE btDIR;

			int iMapNumber = gGateC.GetGate(137, (short &)sX, (short &)sY, (BYTE &)btMAPNUM, (BYTE &)btDIR, (short &)sLVL);

			if (iMapNumber >= 0)
			{
				lpObj->MapNumber = btMAPNUM;
				lpObj->X = sX;
				lpObj->Y = sY;
			}
		}
	}

	else
	{
		if (lpObj->MapNumber == MAP_INDEX_CASTLESIEGE)
		{
			if (g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
			{
				short sX;
				short sY;
				short sLVL;
				BYTE btMAPNUM = lpObj->MapNumber;
				BYTE btDIR;

				int iMapNumber = gGateC.GetGate(100, (short &)sX, (short &)sY, (BYTE &)btMAPNUM, (BYTE &)btDIR, (short &)sLVL);

				if (iMapNumber >= 0)
				{
					lpObj->MapNumber = btMAPNUM;
					lpObj->X = sX;
					lpObj->Y = sY;
					lpObj->m_sDestMapNumber = btMAPNUM;
					lpObj->m_btDestX = sX;
					lpObj->m_btDestY = sY;
				}
			}
		}
	}

	if (lpObj->MapNumber == MAP_INDEX_CASTLEHUNTZONE)
	{
		short sX;
		short sY;
		short sLVL;
		BYTE btMAPNUM = lpObj->MapNumber;
		BYTE btDIR;

		int iMapNumber = gGateC.GetGate(106, (short &)sX, (short &)sY, (BYTE &)btMAPNUM, (BYTE &)btDIR, (short &)sLVL);

		if (iMapNumber >= 0)
		{
			lpObj->MapNumber = btMAPNUM;
			lpObj->X = sX;
			lpObj->Y = sY;
			lpObj->m_sDestMapNumber = btMAPNUM;
			lpObj->m_btDestX = sX;
			lpObj->m_btDestY = sY;
		}
	}

	if (lpObj->m_bMapSvrMoveReq == true)
	{
		short sSVR_CODE = g_MapServerManager.CheckMoveMapSvr(*lpObj, lpObj->m_sDestMapNumber, lpObj->m_sPrevMapSvrCode);

		if (sSVR_CODE == g_ConfigRead.server.GetGameServerCode())
		{
			lpObj->MapNumber = lpObj->m_sDestMapNumber;
			lpObj->X = lpObj->m_btDestX;
			lpObj->Y = lpObj->m_btDestY;
			lpObj->StartX = lpObj->X;
			lpObj->StartY = lpObj->Y;
		}

		else
		{
			gObjDel(*lpObj);

			return FALSE;
		}
	}

	if (lpObj->MapNumber == MAP_INDEX_LORENMARKET)
	{
		BYTE btAttr = MapC[lpObj->MapNumber].GetAttr(lpObj->X, lpObj->Y);

		if ((btAttr & 1) != 1)
		{
			MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, lpObj->X, lpObj->Y);
			lpObj->StartX = lpObj->X;
			lpObj->StartY = lpObj->Y;
		}
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		lpObj->MapNumber = MAP_INDEX_LORENMARKET;
		BYTE btAttr = MapC[lpObj->MapNumber].GetAttr(lpObj->X, lpObj->Y);

		if ((btAttr & 1) != 1)
		{
			MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, lpObj->X, lpObj->Y);
			lpObj->StartX = lpObj->X;
			lpObj->StartY = lpObj->Y;
		}
	}

	if (g_MapAttr.CheckVIP(lpObj->MapNumber) > lpObj->m_PlayerData->VipType)
	{
		lpObj->MapNumber = MAP_INDEX_RORENCIA;
		MapC[lpObj->MapNumber].GetLevelPos(lpObj->Level, lpObj->X, lpObj->Y);
	}

	BYTE attr = MapC[lpObj->MapNumber].GetAttr(lpObj->X, lpObj->Y);
	int result;
	short x;
	short y;
	BYTE mapNumber;
	BYTE dir;
	short level;
	int gt;
	int userlevel;

	if ((attr & 4) == 4 || (attr & 8) == 8)
	{
		MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, lpObj->X, lpObj->Y);
	}

	mapNumber = MAP_INDEX_RORENCIA;
	userlevel = lpObj->Level;
	x = lpObj->X;
	y = lpObj->Y;
	dir = lpObj->Dir;
	level = lpObj->Level;
	mapNumber = lpObj->MapNumber;
	gt = 0;

	if (g_MapAttr.CheckBlockEntry(lpObj->MapNumber) == true)
	{
		lpObj->MapNumber = MAP_INDEX_RORENCIA;
		MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, (short &)lpObj->X, (short &)lpObj->Y);
	}

	if (g_MaxStatsInfo.GetClass.IsNonPvP[lpObj->MapNumber] == TRUE && (g_NewPVP.IsVulcanusMap(lpObj->MapNumber) || g_NewPVP.IsPKFieldMap(lpObj->MapNumber)))
	{
		short sX;
		short sY;
		short sLVL;
		BYTE btMAPNUM = lpObj->MapNumber;
		BYTE btDIR;

		int iMapNumber = gGateC.GetGate(17, (short &)sX, (short &)sY, (BYTE &)btMAPNUM, (BYTE &)btDIR, (short &)sLVL);

		if (iMapNumber >= 0)
		{
			lpObj->MapNumber = btMAPNUM;
			lpObj->X = sX;
			lpObj->Y = sY;
		}
	}

	if (g_MaxStatsInfo.GetClass.IsNonPvP[lpObj->MapNumber] == FALSE && g_NewPVP.IsPKFieldMap(lpObj->MapNumber))
	{
		short sX;
		short sY;
		short sLVL;
		BYTE btMAPNUM = lpObj->MapNumber;
		BYTE btDIR;

		int iMapNumber = gGateC.GetGate(17, (short &)sX, (short &)sY, (BYTE &)btMAPNUM, (BYTE &)btDIR, (short &)sLVL);

		if (iMapNumber >= 0)
		{
			lpObj->MapNumber = btMAPNUM;
			lpObj->X = sX;
			lpObj->Y = sY;
		}
	}

	if (lpObj->MapNumber == MAP_INDEX_HATCHERY &&
		(g_Raklion.GetRaklionState() == RAKLION_STATE_CLOSE_DOOR ||
			g_Raklion.GetRaklionState() == RAKLION_STATE_ALL_USER_DIE ||
			g_Raklion.GetRaklionState() == RAKLION_STATE_NOTIFY_4 ||
			g_Raklion.GetRaklionState() == RAKLION_STATE_END)) //season4 add-on
	{
		gt = 286;
	}

	if (lpObj->MapNumber == MAP_INDEX_RAKLION) //season4 add-on
	{
		g_RaklionUtil.NotifyRaklionCurrentState(*lpObj, g_Raklion.GetRaklionState(), g_Raklion.GetRaklionStateDetail());
	}

	if (lpObj->MapNumber == MAP_INDEX_DEVIAS)
	{
		gt = 22;
	}
	else if (lpObj->MapNumber == MAP_INDEX_NORIA)
	{
		gt = 27;
	}
	else if (lpObj->MapNumber == MAP_INDEX_LOSTTOWER)
	{
		gt = 42;
	}

	if (gt > 0)
	{
		result = gGateC.GetGate(gt, x, y, mapNumber, dir, level);

		if (result >= 0)
		{
			if (lpObj->Class == CLASS_DARKLORD || lpObj->Class == CLASS_MAGUMSA ||
				lpObj->Class == CLASS_RAGEFIGHTER || lpObj->Class == CLASS_GROWLANCER)
			{
				if (level > 0)
				{
					level = level / 3 * 2;
				}
			}

			if (level != 0 && lpObj->Level < level)
			{
				if (lpObj->Class == CLASS_ELF) // Elf
				{
					lpObj->MapNumber = MAP_INDEX_NORIA;
					MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, lpObj->X, lpObj->Y);
				}
				else if (lpObj->Class == CLASS_SUMMONER)
				{
					lpObj->MapNumber = MAP_INDEX_ELBELAND;
					MapC[lpObj->MapNumber].GetLevelPos(lpObj->Level, lpObj->X, lpObj->Y);
				}
				else
				{
					lpObj->MapNumber = MAP_INDEX_RORENCIA;
					MapC[lpObj->MapNumber].GetLevelPos(lpObj->Level, lpObj->X, lpObj->Y);
				}
			}
		}
		else
		{
			sLog->outBasic("error : %d (%s %d)", result, __FILE__, __LINE__);
		}
	}

	lpMsg->MapX = lpObj->X;
	lpMsg->MapY = lpObj->Y;
	lpObj->TX = lpMsg->MapX;
	lpObj->TY = lpMsg->MapY;
	lpObj->m_OldX = lpObj->X;
	lpObj->m_OldY = lpObj->Y;
	lpObj->m_PlayerData->Experience = lpMsg->Exp;

	if (lpMsg->mlNextExp == NULL && lpMsg->mLevel == NULL && lpMsg->mlExp == NULL)
	{
		g_MasterLevelSkillTreeSystem.InitData(*lpObj);
	}

	else
	{
		lpObj->m_PlayerData->MasterLevel = lpMsg->mLevel;
		lpObj->m_PlayerData->MasterExperience = lpMsg->mlExp;
		lpObj->m_PlayerData->MasterNextExp = lpMsg->mlNextExp;
		lpObj->m_PlayerData->MasterPoint = lpMsg->mlPoint;
	}

	lpObj->m_PlayerData->Strength = lpMsg->Str;
	lpObj->m_PlayerData->Dexterity = lpMsg->Dex;
	lpObj->m_PlayerData->Vitality = lpMsg->Vit;
	lpObj->m_PlayerData->Energy = lpMsg->Energy;
	lpObj->Life = lpMsg->Life;
	//lpObj->Life /= 10.0f;
	lpObj->MaxLife = lpMsg->MaxLife;
	//lpObj->MaxLife /= 10.0f;
	lpObj->Mana = lpMsg->Mana;
	//lpObj->Mana /= 10.0f;
	lpObj->MaxMana = lpMsg->MaxMana;
	//lpObj->MaxMana /= 10.0f;
	lpObj->m_PlayerData->Money = lpMsg->Money;
	lpObj->Leadership = lpMsg->Leadership;
	lpObj->ChatLimitTime = lpMsg->ChatLitmitTime;
	lpObj->m_PlayerData->iFruitPoint = lpMsg->iFruitPoint;
	lpObj->m_PlayerData->m_iResets = lpMsg->resets;
	lpObj->WinDuels = lpMsg->WinDuels;
	lpObj->LoseDuels = lpMsg->LoseDuels;
	lpObj->GameMaster = lpMsg->GmCode;
	lpObj->PenaltyMask = lpMsg->PenaltyMask;
	lpObj->m_PlayerData->AgilityCheckDelay = GetTickCount();
	lpObj->m_PlayerData->AgilityCheckCount = 0;

	if (lpObj->m_PlayerData->Strength > g_MaxStatsInfo.GetClass.MaxStats[lpObj->Class][0])
	{
		lpObj->m_PlayerData->Strength = g_MaxStatsInfo.GetClass.MaxStats[lpObj->Class][0];
		lpMsg->Str = g_MaxStatsInfo.GetClass.MaxStats[lpObj->Class][0];
	}

	if (lpObj->m_PlayerData->Dexterity > g_MaxStatsInfo.GetClass.MaxStats[lpObj->Class][1])
	{
		lpObj->m_PlayerData->Dexterity = g_MaxStatsInfo.GetClass.MaxStats[lpObj->Class][1];
		lpMsg->Dex = g_MaxStatsInfo.GetClass.MaxStats[lpObj->Class][1];
	}

	if (lpObj->m_PlayerData->Vitality > g_MaxStatsInfo.GetClass.MaxStats[lpObj->Class][2])
	{
		lpObj->m_PlayerData->Vitality = g_MaxStatsInfo.GetClass.MaxStats[lpObj->Class][2];
		lpMsg->Vit = g_MaxStatsInfo.GetClass.MaxStats[lpObj->Class][2];
	}

	if (lpObj->m_PlayerData->Energy > g_MaxStatsInfo.GetClass.MaxStats[lpObj->Class][3])
	{
		lpObj->m_PlayerData->Energy = g_MaxStatsInfo.GetClass.MaxStats[lpObj->Class][3];
		lpMsg->Energy = g_MaxStatsInfo.GetClass.MaxStats[lpObj->Class][3];
	}

	if (lpObj->Leadership > g_MaxStatsInfo.GetClass.MaxStats[lpObj->Class][4])
	{
		lpObj->Leadership = g_MaxStatsInfo.GetClass.MaxStats[lpObj->Class][4];
		lpMsg->Leadership = g_MaxStatsInfo.GetClass.MaxStats[lpObj->Class][4];
	}

	if (lpObj->Mana < 1.0f)
	{
		lpObj->Mana = 1.0f;
	}

	if (lpObj->Life < 1.0f)
	{
		lpObj->Life = 1.0f;
	}

	lpObj->MaxLife = DCInfo.DefClass[lpObj->Class].Life + ((lpObj->Level + lpObj->m_PlayerData->MasterLevel) - 1) * DCInfo.DefClass[lpObj->Class].LevelLife + ((lpObj->m_PlayerData->Vitality - DCInfo.DefClass[lpObj->Class].Vitality) * DCInfo.DefClass[lpObj->Class].VitalityToLife);
	lpObj->MaxMana = DCInfo.DefClass[lpObj->Class].Mana + ((lpObj->Level + lpObj->m_PlayerData->MasterLevel) - 1) * DCInfo.DefClass[lpObj->Class].LevelMana + ((lpObj->m_PlayerData->Energy - DCInfo.DefClass[lpObj->Class].Energy) * DCInfo.DefClass[lpObj->Class].EnergyToMana);

	if (lpObj->Life > lpObj->MaxLife)
	{
		lpObj->Life = lpObj->MaxLife;
	}

	if (lpObj->Mana > lpObj->MaxMana)
	{
		lpObj->Mana = lpObj->MaxMana;
	}

	lpObj->VitalityToLife = DCInfo.DefClass[lpObj->Class].VitalityToLife;
	lpObj->EnergyToMana = DCInfo.DefClass[lpObj->Class].EnergyToMana;

	gObjSetBP(*lpObj);
	lpObj->BP = lpObj->MaxBP / 2;
	::gObjCalcMaxLifePower(*lpObj);
	lpObj->m_PK_Count = lpMsg->PkCount;
	lpObj->m_PK_Level = lpMsg->PkLevel;
	lpObj->m_PK_Time = lpMsg->PkTime;

	lpObj->m_PlayerData->m_InventoryExpansion = lpMsg->InventoryExpansion;
	lpObj->m_PlayerData->m_WarehouseExpansion = lpMsg->WarehouseExpansion;

	if (lpObj->m_PK_Level == 3)
	{
		lpObj->m_PK_Count = 0;
		lpObj->m_PK_Time = 0;
	}

	lpObj->MaxRegenTime = 4000;	// Resurrection of the player if die
	lpObj->m_MoveSpeed = 1000;

	std::memcpy(lpObj->m_PlayerData->m_Quest, lpMsg->dbQuest, sizeof(lpObj->m_PlayerData->m_Quest));

	if (lpObj->m_PlayerData->m_Quest[0] == 0)
	{
		memset(lpObj->m_PlayerData->m_Quest, (BYTE)-1, sizeof(lpObj->m_PlayerData->m_Quest));
	}

	if (::g_QuestInfo.GetQuestState(*lpObj, 2) == 2)	// Ring of Honor
	{
		if (lpObj->Level < /*QUEST_MINLEVEL_PLUSSTAT*/1)	// Repair Bug of Marlon Before
		{
			::g_QuestInfo.ReSetQuestState(*lpObj, 2);
		}
		else
		{
			lpObj->m_PlayerData->PlusStatQuestClear = true;
		}
	}

	if (::g_QuestInfo.GetQuestState(*lpObj, 3) == 2)	// Dark Stone
	{
		if (lpObj->Level < /*QUEST_MINLEVEL_PLUSSTAT*/1)	// Repair Bug of Marlon After
		{
			::g_QuestInfo.ReSetQuestState(*lpObj, 3);
		}
		else
		{
			lpObj->m_PlayerData->ComboSkillquestClear = true;
		}
	}

	if (lpObj->m_PlayerData->m_Quest[21] > 20)
	{
		lpObj->m_PlayerData->m_Quest[21] = 0;
	}

	if (lpObj->m_PlayerData->m_Quest[22] > 20)
	{
		lpObj->m_PlayerData->m_Quest[22] = 0;
	}

	if (lpObj->m_PlayerData->m_Quest[24] > 20)
	{
		lpObj->m_PlayerData->m_Quest[24] = 0;
	}

	::gObjSetInventory1Pointer(*lpObj);
	lpObj->m_PlayerData->GuildNumber = 0;
	lpObj->m_PlayerData->lpGuild = NULL;

	for (int i = 0; i < MAX_MAGIC; i++)
	{
		lpObj->Magic[i].Clear();
	}

	if (lpObj->Class == CLASS_WIZARD || lpObj->Class == CLASS_ELF ||
		lpObj->Class == CLASS_KNIGHT || lpObj->Class == CLASS_DARKLORD ||
		lpObj->Class == CLASS_MAGUMSA || lpObj->Class == CLASS_SUMMONER ||
		lpObj->Class == CLASS_RAGEFIGHTER || lpObj->Class == CLASS_GROWLANCER)
	{
		BOOL bEnergyBall = TRUE;
		BOOL bForceSkill = TRUE;
		BOOL IsGetInitinityArrowSkill = FALSE;

		gObjMagicAddEnergyCheckOnOff(0);

		for (n = 0; n < MAX_MAGIC; n++)
		{
			itype = lpMsg->dbMagicList[n * MAX_MAGIC_INFO];

			if ((lpMsg->dbMagicList[n * MAX_MAGIC_INFO + DBM_LEVEL] & 0x7) > 0) //Season3 add-on
			{
				itype = itype * (lpMsg->dbMagicList[n * MAX_MAGIC_INFO + DBM_LEVEL] & 0x7) + lpMsg->dbMagicList[n * MAX_MAGIC_INFO + DBM_EXTRA];
			}

			if (itype != 0xFF)
			{
				if (g_MasterLevelSkillTreeSystem.CheckMasterLevelSkill(itype))
				{
					BYTE level = lpMsg->dbMagicList[n * MAX_MAGIC_INFO + DBM_LEVEL] >> 3;

					if (gObjMagicAdd(*lpObj, itype, level) > -1)
					{
						lpObj->m_PlayerData->MasterPointUsed += level;
						g_MasterLevelSkillTreeSystem.CalcPassiveSkillData(*lpObj, itype, level);
					}
				}

				else
				{

					if (itype == 17)
					{
						bEnergyBall = FALSE;
					}

					if (itype == 60)
					{
						bForceSkill = FALSE;
					}

					if (itype == 77)
					{
						IsGetInitinityArrowSkill = 1;
						gObjMagicAdd(*lpObj, itype, lpMsg->dbMagicList[n * MAX_MAGIC_INFO + DBM_LEVEL] & 0xF);
					}

					else if (itype != 18 && itype != 19 && itype != 20 && itype != 21 && itype != 22 && itype != 23 && itype != 24 && itype != 25 && itype != 49 && itype != 56 && itype != 62 && itype != 76 && itype != 54 && itype != AT_SKILL_SAHAMUTT && itype != 224 && itype != 225 && itype != 260 && itype != 261 && itype != 270 && itype != 740)
					{
						gObjMagicAdd(*lpObj, itype, lpMsg->dbMagicList[n * MAX_MAGIC_INFO + DBM_LEVEL] & 0xF);
					}
				}
			}
		}

		if (lpObj->Class == CLASS_WIZARD)	// Wizard
		{
			if (bEnergyBall == TRUE)
			{
				gObjMagicAdd(*lpObj, 17, 0);
				bEnergyBall = FALSE;
			}
		}

		if (lpObj->Class == CLASS_DARKLORD)	// Dark Lord
		{
			if (bForceSkill == TRUE)
			{
				gObjMagicAdd(*lpObj, 60, 0);
				bForceSkill = FALSE;
			}
		}

		if (IsGetInitinityArrowSkill == 0 && lpObj->Class == CLASS_ELF)
		{
			if (lpObj->Level >= g_SkillAdditionInfo.GetInfinityArrowUseLevel())
			{
				if (lpObj->m_PlayerData->ChangeUP >= 1)
				{
					int iAddSkillPosition = gObjMagicAdd(*lpObj, 77, 0);

					if (iAddSkillPosition > 0)
					{
						//sLog->outBasic("[%s][%s] Add Infinity Arrow Skill (Character Level : %d)(ChangeUp: %d)",
						//	lpObj->AccountID, lpObj->Name, lpObj->Level, lpObj->m_PlayerData->ChangeUP);
					}
				}
			}
		}

		if (lpObj->Class == CLASS_GROWLANCER)
		{
			gObjMagicAdd(*lpObj, 275, 0);
		}
		else if (lpObj->Class == CLASS_WIZARD)
		{
			gObjMagicAdd(*lpObj, 45, 0);
			gObjMagicAdd(*lpObj, 67, 0);
			gObjMagicAdd(*lpObj, 68, 0);
			gObjMagicAdd(*lpObj, 69, 0);
			gObjMagicAdd(*lpObj, 70, 0);
			gObjMagicAdd(*lpObj, 71, 0);
		}
		else if (lpObj->Class == CLASS_KNIGHT)
		{
			gObjMagicAdd(*lpObj, 44, 0);
			gObjMagicAdd(*lpObj, 67, 0);
			gObjMagicAdd(*lpObj, 68, 0);
			gObjMagicAdd(*lpObj, 69, 0);
			gObjMagicAdd(*lpObj, 70, 0);
			gObjMagicAdd(*lpObj, 71, 0);
		}
		else if (lpObj->Class == CLASS_RAGEFIGHTER)
		{
			gObjMagicAdd(*lpObj, 269, 0);
		}
		else if (lpObj->Class == CLASS_ELF)
		{
			gObjMagicAdd(*lpObj, 46, 0);
			gObjMagicAdd(*lpObj, 67, 0);
			gObjMagicAdd(*lpObj, 68, 0);
			gObjMagicAdd(*lpObj, 69, 0);
			gObjMagicAdd(*lpObj, 70, 0);
			gObjMagicAdd(*lpObj, 71, 0);
		}
		else if (lpObj->Class == CLASS_MAGUMSA)
		{
			gObjMagicAdd(*lpObj, 57, 0);
			gObjMagicAdd(*lpObj, 67, 0);
			gObjMagicAdd(*lpObj, 68, 0);
			gObjMagicAdd(*lpObj, 69, 0);
			gObjMagicAdd(*lpObj, 70, 0);
			gObjMagicAdd(*lpObj, 71, 0);
		}
		else if (lpObj->Class == CLASS_DARKLORD)	// Dark Lord
		{
			gObjMagicAdd(*lpObj, 74, 0);	// Fire Blast
										//gObjMagicAdd(*lpObj, 75, 0);	// Sacrifice - BUG Apply Fix in Skill.vtm coz is not created
		}
		else if (lpObj->Class == CLASS_SUMMONER)
		{
			gObjMagicAdd(*lpObj, 45, 0);
			gObjMagicAdd(*lpObj, 57, 0);
			gObjMagicAdd(*lpObj, 67, 0);
			gObjMagicAdd(*lpObj, 68, 0);
			gObjMagicAdd(*lpObj, 69, 0);
			gObjMagicAdd(*lpObj, 70, 0);
			gObjMagicAdd(*lpObj, 71, 0);
		}

		::gObjMagicAddEnergyCheckOnOff(TRUE);
	}

	bool bAllItemExist = true;

	DbItemSetInByte(*lpObj, lpMsg, &bAllItemExist);

	lpObj->Live = TRUE;
	lpObj->Type = OBJ_USER;
	lpObj->TargetNumber = -1;
	lpObj->Connected = PLAYER_PLAYING;
	lpObj->m_PlayerData->AgilityCheckDelay = GetTickCount();
	lpObj->m_PlayerData->AgilityCheckCount = 0;
	lpObj->AgiCheckTime = GetTickCount();
	gObjFindInventoryEquipment(*lpObj);
	::gObjMakePreviewCharSet(*lpObj);
	CreateFrustrum(lpObj->X, lpObj->Y, *lpObj);
	MapC[lpObj->MapNumber].SetStandAttr(lpObj->X, lpObj->Y);
	lpObj->m_OldX = lpObj->X;
	lpObj->m_OldY = lpObj->Y;
	lpMsg->NextExp = lpObj->m_PlayerData->NextExp;
	lpObj->m_PlayerData->LivePacketTime = 0;
	lpObj->m_PlayerData->LiveCheckDetectCount = 0;
	lpObj->m_PlayerData->Ruud = lpMsg->Ruud;
	lpObj->EventMap = lpMsg->EventMap;
	if (lpObj->Life == 0.0f)
	{
		lpObj->Live = TRUE;
		lpObj->m_State = 4;
		lpObj->DieRegen = TRUE;
		lpObj->RegenTime = GetTickCount();
	}
	else
	{
		lpObj->m_State = 1;
	}

	if ((lpMsg->CtlCode & 8) == 8 && lpMsg->GmCode != 0)
	{
		lpObj->Authority = 2;
		gObjAddBuffEffect(*lpObj, BUFFTYPE_GM_MARK, 0, 0, 0, 0, -10);
		cManager.ManagerAdd(lpObj->Name, *lpObj);
		MsgOutput(*lpObj, Lang.GetText(0, 408), lpMsg->GmExpDays);
	}

	else
	{
		lpObj->Authority = 1;
	}

	if ((lpMsg->CtlCode & 0x20) == 0x20 && lpMsg->GmCode != 0)
	{
		lpObj->Authority = 0x20;
		gObjAddBuffEffect(*lpObj, BUFFTYPE_GM_MARK, 0, 0, 0, 0, -10);
		MsgOutput(*lpObj, Lang.GetText(0, 409), lpMsg->GmExpDays);
	}

	lpObj->Penalty = 0;

	if (lpObj->m_cAccountItemBlock != 0)
	{
		lpObj->Penalty |= 8;
	}

	if ((lpMsg->CtlCode & 2) == 2)
	{
		lpObj->Penalty |= 4;
	}

	if ((lpObj->PenaltyMask & 1) == 1)
	{
		lpObj->Penalty |= 2;
		lpObj->ChatBlockTime = lpMsg->ChatBlockTime;
		MsgOutput(*lpObj, Lang.GetText(0, 410));
	}

	gObjAuthorityCodeSet(*lpObj);

	BYTE weather = MapC[lpObj->MapNumber].GetWeather();

	if ((weather >> 4) > 0)
	{
		gGameProtocol.CGWeatherSend(*lpObj, weather);
	}

	if (DragonEvent->GetState() > 0)
	{
		if (DragonEvent->GetMapNumber() == lpObj->MapNumber)
		{
			::gGameProtocol.GCMapEventStateSend(lpObj->MapNumber, 1, 1);
		}
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && lpObj->MapNumber == MAP_INDEX_CASTLESIEGE)
	{
		gGameProtocol.GCAnsCsNotifyStart(*lpObj, CASTLESIEGEREADY(g_CastleSiege.GetCastleState()));

		if (g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
		{
			g_CastleSiege.NotifyCsSelfLeftTime(*lpObj);
		}
	}

	::gEledoradoEvent.CheckGoldDercon(lpObj->MapNumber);
	//::gGameProtocol.GCCheckMainExeKeySend(lpObj->m_Index);

	if (lpMsg->Married == 1)
	{
		std::memcpy(lpObj->MarryName, lpMsg->MarryName, 10);
		lpObj->Married = 1;
	}
	else
	{
		lpObj->Married = 0;
	}

	if (m_ObjBill[lpObj->m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].GetCertify() >= 0 && m_ObjBill[lpObj->m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].GetCertify() < 3)
	{
		gLCount[m_ObjBill[lpObj->m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].GetCertify()].Add();
	}

	if (bAllItemExist == false)
	{
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx((TNotice *)&pNotice, 1, Lang.GetText(0, 411));
		IOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (UCHAR *)&pNotice, pNotice.h.size);
	}

	if (lpObj->Level <= g_iCharacterRecuperationMaxLevel)
	{
		lpObj->m_iAutoRecuperationTime = GetTickCount();
	}
	else
	{
		lpObj->m_iAutoRecuperationTime = 0;
	}

	lpObj->dwShieldAutoRefillTimer = GetTickCount();

	g_CashShop.AddUser(*lpObj);
	g_PeriodItemEx.AddPeriodInfo(*lpObj);
	g_CMuunSystem.AddMuunItemPeriodInfo(*lpObj);

	if (lpObj->Class == CLASS_DARKLORD && lpObj->m_Index >= g_ConfigRead.server.GetObjectStartUserIndex())
	{
		gDarkSpirit[lpObj->m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].SetMode(CDarkSpirit::ePetItem_Mode::PetItem_Mode_Normal, -1);
	}

	UserChatBlockMng::getInstance()->addSlot(lpObj->m_Index);

	return TRUE;
}

int gObjCanItemTouch(CGameObject &Obj, int type)
{
	if ((Obj.Penalty & 4) == 4)
	{
		return false;
	}

	if ((Obj.Penalty & 8) == 8)
	{
		if (type == 6 || type == 8)
		{
			return true;
		}
		return false;
	}
	return true;
}

void gObjMagicTextSave(CGameObject &Obj)
{
	int total = 0;

	for (int n = 0; n < MAGIC_SIZE; n++)
	{
		if (Obj.Magic[n].IsMagic() == TRUE)
		{
			/*CharSaveLog->Output("[%s][%s] Skill Save Complete %d %d", Obj.AccountID, Obj.Name, n, Obj.Magic[n].m_Skill);*/
			total++;
		}
	}
}

void ItemIsBufExOption(LPBYTE buf, CItemObject *lpItem)
{
	for (int i = 0; i < MAX_EXOPTION_SIZE; i++)
	{
		buf[i] = 0;
	}

	if ((lpItem->m_NewOption & 0x20) == 0x20)
	{
		buf[0] = TRUE;
	}

	if ((lpItem->m_NewOption & 0x10) == 0x10)
	{
		buf[1] = TRUE;
	}

	if ((lpItem->m_NewOption & 8) == 8)
	{
		buf[2] = TRUE;
	}

	if ((lpItem->m_NewOption & 4) == 4)
	{
		buf[3] = TRUE;
	}

	if ((lpItem->m_NewOption & 2) == 2)
	{
		buf[4] = TRUE;
	}

	if ((lpItem->m_NewOption & 1) == 1)
	{
		buf[5] = TRUE;
	}

}

void gObjStatTextSave(CGameObject &Obj)
{
	/*CharSaveLog->Output("[%s][%s] CharInfoSave : Class=%d Level=%d LVPoint=%d Exp=%d Str=%d Dex=%d Vit=%d Energy=%d Leadership:%d Map=%d Pk=%d",
		Obj.AccountID, Obj.Name, Obj.m_PlayerData->DbClass, Obj.Level, Obj.m_PlayerData->LevelUpPoint, Obj.m_PlayerData->Experience,
		Obj.m_PlayerData->Strength, Obj.m_PlayerData->Dexterity, Obj.m_PlayerData->Vitality, Obj.m_PlayerData->Energy, Obj.Leadership, Obj.MapNumber,
		Obj.m_PK_Level);*/
}

void gObjItemTextSave(CGameObject &Obj)
{
	BYTE NewOption[MAX_EXOPTION_SIZE];
	int n;

	for (n = 0; n < INVENTORY_SIZE; n++)
	{
		if (Obj.pntInventory[n]->IsItem() == TRUE)
		{
			if (Obj.pntInventory[n]->m_serial != 0)
			{
				ItemIsBufExOption(NewOption, Obj.pntInventory[n]);
				/*CharSaveLog->Output("[%s][%s] SI[%d,%s,%d,%d,%d,%d]serial:[%I64d] dur:[%d] Ex:[%d,%d,%d,%d,%d,%d,%d] Set[%d] 380:[%d] HO:[%d,%d] E:[%d]", Obj.AccountID, Obj.Name, n, Obj.pntInventory[n]->GetName(),
					Obj.pntInventory[n]->m_Level, Obj.pntInventory[n]->m_Option1,
					Obj.pntInventory[n]->m_Option2, Obj.pntInventory[n]->m_Option3,
					Obj.pntInventory[n]->m_Number, (BYTE)Obj.pntInventory[n]->m_Durability,
					NewOption[0], NewOption[1], NewOption[2], NewOption[3], NewOption[4], NewOption[5], NewOption[6],
					Obj.pntInventory[n]->m_SetOption, Obj.pntInventory[n]->m_ItemOptionEx >> 7, g_kJewelOfHarmonySystem.GetItemStrengthenOption(&Obj.pntInventory[n]), g_kJewelOfHarmonySystem.GetItemOptionLevel(&Obj.pntInventory[n]),
					Obj.pntInventory[n]->m_BonusSocketOption);*/
			}
		}
	}

}

void gObjWarehouseTextSave(CGameObject &Obj)
{
	BYTE NewOption[MAX_EXOPTION_SIZE];
	int n;

	for (n = 0; n < WAREHOUSE_SIZE; n++)
	{
		if (Obj.pntWarehouse[n]->IsItem() == TRUE)
		{
			if (Obj.pntWarehouse[n]->m_serial != 0)
			{
				ItemIsBufExOption(NewOption, Obj.pntWarehouse[n]);
				/*CharSaveLog->Output("[%s][%s] SW [%d,%s,%d,%d,%d,%d]serial:[%I64d] dur:[%d] Ex:[%d,%d,%d,%d,%d,%d,%d] Set:[%d] 380:[%d] HO:[%d,%d] E:[%d]", Obj.AccountID, Obj.Name, n, Obj.pntWarehouse[n]->GetName(),
					Obj.pntWarehouse[n]->m_Level, Obj.pntWarehouse[n]->m_Option1,
					Obj.pntWarehouse[n]->m_Option2, Obj.pntWarehouse[n]->m_Option3,
					Obj.pntWarehouse[n]->m_Number, (BYTE)Obj.pntWarehouse[n]->m_Durability,
					NewOption[0], NewOption[1], NewOption[2], NewOption[3], NewOption[4], NewOption[5], NewOption[6],
					Obj.pntWarehouse[n]->m_SetOption, Obj.pntWarehouse[n]->m_ItemOptionEx >> 7,
					g_kJewelOfHarmonySystem.GetItemStrengthenOption(&Obj.pntWarehouse[n]),
					g_kJewelOfHarmonySystem.GetItemOptionLevel(&Obj.pntWarehouse[n]),
					Obj.pntWarehouse[n]->m_BonusSocketOption);*/
			}
		}
	}

}

void gObjAuthorityCodeSet(CGameObject &Obj)
{
	if ((Obj.Authority & 1) == 1)
	{
		return;
	}

	if ((Obj.Authority & 2) == 2)
	{
		Obj.AuthorityCode = -1;
		return;
	}

	if ((Obj.Authority & 4) == 4)
	{
		return;
	}

	if ((Obj.Authority & 8) == 8)
	{
		Obj.AuthorityCode |= 1;
		Obj.AuthorityCode |= 2;
		Obj.AuthorityCode |= 4;
		Obj.AuthorityCode |= 8;
		Obj.AuthorityCode |= 16;
		Obj.AuthorityCode |= 32;

		return;
	}

	if ((Obj.Authority & 16) == 16)
	{
		return;
	}

	if ((Obj.Authority & 32) == 32)
	{
		Obj.AuthorityCode |= 1;
		Obj.AuthorityCode |= 2;
		Obj.AuthorityCode |= 4;
		Obj.AuthorityCode |= 8;
		Obj.AuthorityCode |= 16;
		Obj.AuthorityCode |= 32;

		return;
	}

	if ((Obj.PenaltyMask & 1) == 1)
	{
		Obj.Penalty |= 2;
	}

}

int gObjSetPosMonster(CGameObject &Obj, int PosTableNum)
{
	Obj.m_PosNum = PosTableNum;
	Obj.X = gMSetBase.m_Mp[PosTableNum].m_X;
	Obj.Y = gMSetBase.m_Mp[PosTableNum].m_Y;
	Obj.MapNumber = gMSetBase.m_Mp[PosTableNum].m_MapNumber;
	Obj.TX = Obj.X;
	Obj.TY = Obj.Y;
	Obj.m_OldX = Obj.X;
	Obj.m_OldY = Obj.Y;
	Obj.Dir = gMSetBase.m_Mp[PosTableNum].m_Dir;
	Obj.StartX = Obj.X;
	Obj.StartY = Obj.Y;

	if (gMSetBase.GetPosition(PosTableNum, Obj.MapNumber, Obj.X, Obj.Y) == FALSE)
	{
		return FALSE;
	}

	Obj.TX = Obj.X;
	Obj.TY = Obj.Y;
	Obj.m_OldX = Obj.X;
	Obj.m_OldY = Obj.Y;
	Obj.Dir = gMSetBase.m_Mp[PosTableNum].m_Dir;
	Obj.StartX = Obj.X;
	Obj.StartY = Obj.Y;
	int iPentagramMainAttribute = 0;
	gMSetBase.GetPentagramMainAttribute(PosTableNum, &iPentagramMainAttribute);

	if (iPentagramMainAttribute)
	{
		Obj.m_iPentagramMainAttribute = iPentagramMainAttribute;
	}

	return true;
}

int gObjSetMonster(CGameObject &Obj, int MonsterClass)
{
	int Level;

	Obj.ConnectCheckTime = GetTickCount();

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		if (MonsterClass > 547)
		{
			if (MonsterClass == 651 || MonsterClass == 669)
			{
				return FALSE;
			}
		}

		else if (MonsterClass >= 545 || (MonsterClass == 240 || MonsterClass == 385 || MonsterClass == 540))
		{
			return FALSE;
		}
	}

	else
	{
		if (MonsterClass >= 683 && MonsterClass <= 688)
		{
			return FALSE;
		}
	}

	Obj.TargetNumber = -1;
	Obj.m_RecallMon = -1;

	if (MonsterClass == 200)
	{
		gBSGround[0]->m_BallIndex = Obj.m_Index;
	}

	if (MonsterClass == 240)
	{
		Obj.m_btNpcType = NPC_TYPES::NPC_WAREHOUSE;
	}

	else if (MonsterClass == 238 || MonsterClass == 450 || MonsterClass == 478 || MonsterClass == 368 || MonsterClass == 369 || MonsterClass == 370 || MonsterClass == 452 || MonsterClass == 453)
	{
		Obj.m_btNpcType = NPC_TYPES::NPC_CHAOSMIX;
	}

	else if (MonsterClass == 236)
	{
		Obj.m_btNpcType == NPC_TYPES::NPC_GOLDARCHER;
	}

	else if (MonsterClass == 582)
	{
		Obj.m_btNpcType = NPC_TYPES::NPC_PENTAGRAMMIX;
	}

	else
	{
		Obj.m_btNpcType = NPC_TYPES::NPC_NONE;
	}

	Obj.Connected = PLAYER_PLAYING;
	Obj.Live = TRUE;
	Obj.m_State = 1;

	if ((MonsterClass >= 204 && MonsterClass <= 259)
		|| (MonsterClass >= 368 && MonsterClass <= 370) ||
		MonsterClass == 367 ||
		MonsterClass == 375 || MonsterClass == 376 || MonsterClass == 377 || MonsterClass == 379 || MonsterClass == 406 || MonsterClass == 407 || MonsterClass == 408 ||
		MonsterClass == 415 || MonsterClass == 416 || MonsterClass == 417 || MonsterClass == 465 ||
		MonsterClass == 467 || MonsterClass == 468 || MonsterClass == 469 || MonsterClass == 470 ||
		MonsterClass == 471 || MonsterClass == 472 || MonsterClass == 473 || MonsterClass == 474 ||
		MonsterClass == 475 || MonsterClass == 478 || MonsterClass == 371 || MonsterClass == 479 || MonsterClass == 540 || MonsterClass == 492 ||
		MonsterClass == 522 || MonsterClass == 464 || MonsterClass == 450 || MonsterClass == 451 || MonsterClass == 452 || MonsterClass == 453 ||
		MonsterClass == 543 || MonsterClass == 544 || MonsterClass == 385 || MonsterClass == 381 || MonsterClass == 382 || MonsterClass == 380 || MonsterClass == 383 || MonsterClass == 384 || MonsterClass == 541 || MonsterClass == 542 || MonsterClass == 546 || MonsterClass == 545 || MonsterClass == 547 || MonsterClass == 566 || MonsterClass == 567 || MonsterClass == 568 || MonsterClass == 577 || MonsterClass == 578 || MonsterClass == 579 ||
		MonsterClass == 583 || MonsterClass == 580 || MonsterClass == 581 || MonsterClass == 582 || MonsterClass == 584 || MonsterClass == 603 || MonsterClass == 604 || MonsterClass == 643 || MonsterClass == 651 || MonsterClass == 658 || MonsterClass == 659 || MonsterClass == 660 || MonsterClass == 661 || MonsterClass == 662 || MonsterClass == 663 || MonsterClass == 664 || MonsterClass == 665 || MonsterClass == 666 || MonsterClass == 667 ||
		MonsterClass == 668 || MonsterClass == 682 || MonsterClass == 683 || MonsterClass == 684 || MonsterClass == 685 || MonsterClass == 686 || MonsterClass == 687 || MonsterClass == 688 || MonsterClass == 695 || MonsterClass == 725 || MonsterClass == 720 || MonsterClass == 719 || MonsterClass == 723 || MonsterClass == 721 || MonsterClass == 722 || MonsterClass == 748)
	{
		Obj.Type = OBJ_NPC;
	}
	else
	{
		Obj.Type = OBJ_MONSTER;
	}

	if (MonsterClass == 385)
	{
		g_IT_Event.SetMirageIndex(Obj.m_Index);
	}

	if (MonsterClass >= 523 && MonsterClass <= 528)
		Obj.Type = OBJ_MONSTER;

	Obj.Class = MonsterClass;
	Level = Obj.Level;
	LPMONSTER_ATTRIBUTE lpm = gMAttr.GetAttr(Obj.Class);

	if (lpm == NULL)
	{
		sLog->outError("Warning! Cannot find monster attribute %d (%s %d)", Obj.Class, __FILE__, __LINE__);
		return false;
	}

	strncpy(Obj.Name, lpm->m_Name, 10);

	if (lpm->m_Level == 0)
	{
		sLog->outBasic("Monster level can't be 0, Class: %d, %s %d", Obj.Class, __FILE__, __LINE__);
	}

	Obj.Level = lpm->m_Level;
	Obj.m_AttackSpeed = lpm->m_AttackSpeed;
	Obj.m_AttackDamageMin = lpm->m_DamageMin;
	Obj.m_AttackDamageMax = lpm->m_DamageMax;
	Obj.m_Defense = lpm->m_Defense;
	Obj.m_MagicDefense = lpm->m_MagicDefense;
	Obj.m_AttackRating = lpm->m_AttackRating;
	Obj.m_SuccessfulBlocking = lpm->m_Successfulblocking;
	Obj.Life = lpm->m_Hp;
	Obj.MaxLife = lpm->m_Hp;
	Obj.Mana = lpm->m_Mp;
	Obj.MaxMana = lpm->m_Mp;
	Obj.m_MoveRange = lpm->m_MoveRange;
	Obj.m_AttackSpeed = lpm->m_AttackSpeed;
	Obj.m_MoveSpeed = lpm->m_MoveSpeed;
	Obj.MaxRegenTime = lpm->m_RegenTime * 1000;
	Obj.m_AttackRange = lpm->m_AttackRange;
	Obj.m_ViewRange = lpm->m_ViewRange;
	Obj.m_Attribute = lpm->m_Attribute;
	Obj.m_AttackType = lpm->m_AttackType;
	Obj.m_ItemRate = lpm->m_ItemRate;
	Obj.m_MoneyRate = lpm->m_MoneyRate;

	Obj.m_iScriptMaxLife = lpm->m_iScriptHP;
	Obj.m_iBasicAI = lpm->m_iAINumber;
	Obj.m_iCurrentAI = lpm->m_iAINumber;
	Obj.m_iCurrentAIState = 0;
	Obj.m_iLastAIRunTime = 0;
	Obj.m_iGroupNumber = 0;
	Obj.m_iSubGroupNumber = 0;
	Obj.m_iGroupMemberGuid = -1;
	Obj.m_iRegenType = 0;

	std::map<int, CUSTOM_MONSTER_ATTR*>::iterator _It = gCMAttr.m_MonsterData.find(MonsterClass);

	if (_It == gCMAttr.m_MonsterData.end())
	{
		Obj.Life *= gCMAttr.m_DefaultData.HPMul;
		Obj.MaxLife *= gCMAttr.m_DefaultData.HPMul;

		Obj.m_AttackDamageMin = (int)(Obj.m_AttackDamageMin*gCMAttr.m_DefaultData.DmgMul);
		Obj.m_AttackDamageMax = (int)(Obj.m_AttackDamageMax*gCMAttr.m_DefaultData.DmgMul);
		//Obj.m_ElementalDamageMin *= gCMAttr.m_DefaultData.DmgMul;
		//Obj.m_ElementalDamageMax *= gCMAttr.m_DefaultData.DmgMul;

		Obj.m_Defense = (int)(Obj.m_Defense*gCMAttr.m_DefaultData.DefMul);
		Obj.m_MagicDefense = (int)(Obj.m_MagicDefense*gCMAttr.m_DefaultData.DefMul);
		//Obj.m_ElementalDefense *= gCMAttr.m_DefaultData.DefMul;

		Obj.m_AttackRating = (int)(Obj.m_AttackRating*gCMAttr.m_DefaultData.ARateMul);
		//Obj.m_ElementalAttackRate *= gCMAttr.m_DefaultData.ARateMul;
		Obj.m_SuccessfulBlocking = (int)(Obj.m_SuccessfulBlocking*gCMAttr.m_DefaultData.DRateMul);
		//Obj.m_ElementalDefenseRate *= gCMAttr.m_DefaultData.DRateMul;
	}
	else
	{
		CUSTOM_MONSTER_ATTR* pMonInfo = _It->second;

		Obj.Life *= pMonInfo->HPMul;
		Obj.MaxLife *= pMonInfo->HPMul;

		Obj.m_AttackDamageMin = (int)(Obj.m_AttackDamageMin*pMonInfo->DmgMul);
		Obj.m_AttackDamageMax = (int)(Obj.m_AttackDamageMax*pMonInfo->DmgMul);
		//Obj.m_ElementalDamageMin *= pMonInfo->DmgMul;
		//Obj.m_ElementalDamageMax *= pMonInfo->DmgMul;

		Obj.m_Defense = (int)(Obj.m_Defense*pMonInfo->DefMul);
		Obj.m_MagicDefense = (int)(Obj.m_MagicDefense*pMonInfo->DefMul);
		//Obj.m_ElementalDefense *= pMonInfo->DefMul;

		Obj.m_AttackRating = (int)(Obj.m_AttackRating*pMonInfo->ARateMul);
		//Obj.m_ElementalAttackRate *= pMonInfo->ARateMul;
		Obj.m_SuccessfulBlocking = (int)(Obj.m_SuccessfulBlocking*pMonInfo->DRateMul);
		//Obj.m_ElementalDefenseRate *= pMonInfo->DRateMul;
	}

	Obj.m_Agro.ResetAll();

	Obj.m_iLastAutomataRuntime = 0;
	Obj.m_iLastAutomataDelay = 0;

	Obj.m_btExpType = lpm->m_btExpType;

	if (lpm->m_PentagramMainAttribute == 6)
	{
		switch (rand() % 5)
		{
		case 0:
			Obj.m_iPentagramMainAttribute = EL_FIRE;
			break;
		case 1:
			Obj.m_iPentagramMainAttribute = EL_WATER;
			break;
		case 2:
			Obj.m_iPentagramMainAttribute = EL_EARTH;
			break;
		case 3:
			Obj.m_iPentagramMainAttribute = EL_WIND;
			break;
		case 4:
			Obj.m_iPentagramMainAttribute = EL_DARKNESS;
			break;
		}
	}

	else if (lpm->m_PentagramMainAttribute > 0)
	{
		Obj.m_iPentagramMainAttribute = lpm->m_PentagramMainAttribute;
	}

	Obj.m_iPentagramAttributePattern = lpm->m_PentagramAttributePattern;
	Obj.m_iPentagramDefense = lpm->m_PentagramDefense;
	Obj.m_iPentagramAttackMin = lpm->m_PentagramAttackMin;
	Obj.m_iPentagramAttackMax = lpm->m_PentagramAttackMax;
	Obj.m_iPentagramAttackRating = lpm->m_PentagramAttackRating;
	Obj.m_iPentagramDefenseRating = lpm->m_PentagramDefenseRating;

	Obj.pInventoryCount[0] = 0;
	std::memcpy(Obj.m_Resistance, lpm->m_Resistance, MAX_RESISTENCE_TYPE);
	gObjSetInventory1Pointer(Obj);

	if (Obj.m_AttackType != 0)
	{
		if (DG_MAP_RANGE(Obj.MapNumber) == TRUE && Obj.Type == OBJ_MONSTER)
		{
			gObjMonsterMagicAdd(Obj, Obj.m_AttackType, 1);
		}

		else if (Obj.m_AttackType >= 100)
		{
			gObjMonsterMagicAdd(Obj, Obj.m_AttackType - 100, 1);
		}
		else
		{
			gObjMonsterMagicAdd(Obj, Obj.m_AttackType, 1);
		}

		if (Obj.Class == 144 || Obj.Class == 174 || Obj.Class == 182 || Obj.Class == 190 || Obj.Class == 260 || Obj.Class == 268 || Obj.Class == 334)
		{
			gObjMonsterMagicAdd(Obj, 17, 1);
		}

		if (Obj.Class == 145
			|| Obj.Class == 175
			|| Obj.Class == 183
			|| Obj.Class == 191
			|| Obj.Class == 261
			|| Obj.Class == 269
			|| Obj.Class == 146
			|| Obj.Class == 176
			|| Obj.Class == 184
			|| Obj.Class == 192
			|| Obj.Class == 262
			|| Obj.Class == 270
			|| Obj.Class == 147
			|| Obj.Class == 177
			|| Obj.Class == 185
			|| Obj.Class == 193
			|| Obj.Class == 263
			|| Obj.Class == 271
			|| Obj.Class == 148
			|| Obj.Class == 178
			|| Obj.Class == 186
			|| Obj.Class == 194
			|| Obj.Class == 264
			|| Obj.Class == 272
			|| Obj.Class == 160
			|| Obj.Class == 180
			|| Obj.Class == 188
			|| Obj.Class == 196
			|| Obj.Class == 266
			|| Obj.Class == 274
			|| Obj.Class == 332
			|| Obj.Class == 333
			|| Obj.Class == 336
			|| Obj.Class == 337)
		{
			gObjMonsterMagicAdd(Obj, 17, 1);
		}

		if (Obj.Class == 161 || Obj.Class == 181 || Obj.Class == 189 || Obj.Class == 197 || Obj.Class == 267 || Obj.Class == 275)
		{
			gObjMonsterMagicAdd(Obj, 200, 1);
			gObjMonsterMagicAdd(Obj, 17, 1);
			gObjMonsterMagicAdd(Obj, 1, 1);
			gObjMonsterMagicAdd(Obj, 201, 1);
			gObjMonsterMagicAdd(Obj, 202, 1);
			gObjMonsterMagicAdd(Obj, 55, 1);
		}

		if (Obj.Class == 149 || Obj.Class == 179 || Obj.Class == 187 || Obj.Class == 195 || Obj.Class == 265 || Obj.Class == 273 || Obj.Class == 335)
		{
			gObjMonsterMagicAdd(Obj, 17, 1);
			gObjMonsterMagicAdd(Obj, 1, 1);
		}

		if (Obj.Class == 66 || Obj.Class == 73 || Obj.Class == 77 || Obj.Class == 89 || Obj.Class == 95 || Obj.Class == 112 || Obj.Class == 118 || Obj.Class == 124 || Obj.Class == 130 || Obj.Class == 143 || Obj.Class == 163 || Obj.Class == 165 || Obj.Class == 167 || Obj.Class == 169 || Obj.Class == 171 || Obj.Class == 173 || Obj.Class == 427)
		{
			gObjMonsterMagicAdd(Obj, 17, 1);
		}

		if (Obj.Class == 89 || Obj.Class == 95 || Obj.Class == 112 || Obj.Class == 118 || Obj.Class == 124 || Obj.Class == 130 || Obj.Class == 143 || Obj.Class == 433)
		{
			gObjMonsterMagicAdd(Obj, 3, 1);
		}

		if (Obj.Class == 561)
		{
			gObjMonsterMagicAdd(Obj, 238, 1);
			gObjMonsterMagicAdd(Obj, 237, 1);
			gObjMonsterMagicAdd(Obj, 38, 1);
			gObjMonsterMagicAdd(Obj, 9, 1);
		}

		if (Obj.Class == 673)
		{
			gObjMonsterMagicAdd(Obj, 622, 1);
		}
	}

	CreateFrustrum(Obj.X, Obj.Y, Obj);

	MapC[Obj.MapNumber].SetStandAttr(Obj.X, Obj.Y);
	Obj.m_OldX = Obj.X;
	Obj.m_OldY = Obj.Y;
	Obj.LastAttackerID = -1;

	return true;
}

short gObjAddSearch(SOCKET aSocket, char* ip)
{
	int count;
	int totalcount = 0;

	if (gDisconnect == 1)
	{
		return -1;
	}

	if (g_AuthSys.IsLongDisconnect() == true)
	{
		return -1;
	}

	if (gObjTotalUser > g_ConfigRead.server.GetObjectMaxUser())
	{
		gGameProtocol.GCJoinSocketResult(4, aSocket);
		return -1;
	}

	count = gGameObjects.size();

	while (true)
	{
		if (getGameObject(count)->Connected == PLAYER_EMPTY)
		{
			return count;
		}

		count++;

		if (count >= g_ConfigRead.server.GetObjectMax())
		{
			count = g_ConfigRead.server.GetObjectStartUserIndex();
		}
		totalcount++;
		if (totalcount >= g_ConfigRead.server.GetObjectMaxUser())
		{
			break;
		}
	}
	return -1;
}

short gObjAdd(SOCKET aSocket, char* ip, CGameObject &Obj)
{
	CGameObject* lpObj = new CGameObject();
	if (lpObj->Connected != PLAYER_EMPTY)
	{
		return -1;
	}

	gObjCharZeroSet((*lpObj));
	lpObj->LoginMsgSnd = FALSE;
	lpObj->LoginMsgCount = 0;
	memset(lpObj->AccountID, 0, sizeof(getGameObject(0)->AccountID) - 1);
	lpObj->m_Index = gGameObjects.size();
	lpObj->m_socket = aSocket;
	lpObj->ConnectCheckTime = GetTickCount();
	lpObj->AutoSaveTime = lpObj->ConnectCheckTime;
	lpObj->Connected = PLAYER_CONNECTED;
	lpObj->CheckSpeedHack = false;
	lpObj->LoginMsgCount = 0;
	lpObj->EnableCharacterCreate = 0;
	lpObj->Type = OBJ_USER;
	m_ObjBill[gGameObjects.size() - g_ConfigRead.server.GetObjectStartUserIndex()].Init();
	sLog->outBasic("Connection Accept : [%d][%s]", lpObj->m_PlayerData->ConnectUser->Index, ip);

	insertGameObject(lpObj);

	return lpObj->m_Index;
}

short gObjMonsterRecall(int iMapNumber)
{
	int number = gObjAddMonster(iMapNumber);

	if (number < 0)
	{
		return -1;
	}
	return -1;
}

short gObjAddMonster(int iMapNumber)
{
	bool bCheckCanMove = true;

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE
		|| g_ConfigRead.server.GetServerType() == SERVER_NORMAL)
	{
		if (IT_MAP_RANGE(iMapNumber) || ITL_MAP_RANGE(iMapNumber) || iMapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL || iMapNumber == MAP_INDEX_DEVILSQUARE_FINAL || CC_MAP_RANGE(iMapNumber))
		{
			bCheckCanMove = false;
		}
	}

	if (bCheckCanMove == true)
	{
		if (g_MapServerManager.CheckMapCanMove(iMapNumber) == FALSE)
		{
			return -1;
		}
	}

	int count;
	int totalcount = 0;

	count = gObjMonCount;

	while (true)
	{
		if (getGameObject(count)->Connected == PLAYER_EMPTY)
		{
			gObjCharZeroSet(*getGameObject(count));
			getGameObject(count)->m_Index = count;
			getGameObject(count)->Connected = PLAYER_CONNECTED;
			getGameObject(count)->MapNumber = iMapNumber;
			gObjMonCount++;

			if (gObjMonCount >= g_ConfigRead.server.GetObjectMaxMonster())
			{
				gObjMonCount = 0;
			}

			return count;

		}

		count++;

		if (count >= g_ConfigRead.server.GetObjectMaxMonster())
		{
			count = 0;
		}

		totalcount++;

		if (totalcount >= g_ConfigRead.server.GetObjectMaxMonster())
		{
			sLog->outBasic("error: Cannnot add any more. %s %d", __FILE__, __LINE__);
			return -1;
		}
	}
	return -1;
}


short gObjAddCallMon()
{
	int count;
	int totalcount = 0;

	count = gObjCallMonCount;

	while (true)
	{
		if (getGameObject(count)->Connected == PLAYER_EMPTY)
		{
			gObjCharZeroSet(*getGameObject(count));
			getGameObject(count)->m_Index = count;
			getGameObject(count)->Connected = PLAYER_CONNECTED;
			gObjCallMonCount++;

			if (gObjCallMonCount >= g_ConfigRead.server.GetObjectStartUserIndex())
			{
				gObjCallMonCount = g_ConfigRead.server.GetObjectMaxMonster();

			}
			return count;
		}

		count++;

		if (count >= g_ConfigRead.server.GetObjectStartUserIndex())
		{
			count = g_ConfigRead.server.GetObjectMaxMonster();
		}

		totalcount++;

		if (totalcount >= g_ConfigRead.server.GetObjectMaxCallMonster())
		{
			sLog->outBasic("error: Cannnot add any more %s %d", __FILE__, __LINE__);
			return -1;
		}
	}

	return -1;

}


void gObjUserKill(CGameObject &Obj)
{
	

	if (Obj.CloseCount < 1)
	{
		Obj.CloseCount = 6;
		Obj.CloseType = 0;
	}
}




void gObjAllLogOut()
{
	gDisconnect = 1;
	gObjAllDisconnect();
	Sleep(1000);
	sLog->outError("Whole connection closed");
}




void gObjAllDisconnect()
{
	for (int n = g_ConfigRead.server.GetObjectStartUserIndex(); n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		if (getGameObject(n)->Connected > PLAYER_EMPTY)
		{
			if (getGameObject(n)->Type == OBJ_USER)
			{
				if (getGameObject(n)->m_bOff != false)
				{
					// TODO
					//gGameProtocol.SetCharacterInfo(*getGameObject(n), n, FALSE);
					getGameObject(n)->m_bOff = false;
					gObjDel(*getGameObject(n));

				}

				IOCP.CloseClient(n);
			}
		}
	}
}

void gObjTradeSave(CGameObject &Obj)
{
	if (Obj.m_IfState->use != 0 && Obj.m_IfState->type == 1)
	{
		int tObjNum = Obj.TargetNumber;

		if (tObjNum > 0)
		{
			char szTemp[256];

			gGameProtocol.CGTradeResult(*getGameObject(tObjNum), 0);
			wsprintf(szTemp, Lang.GetText(0, 14), Obj.Name);
			gGameProtocol.GCServerMsgStringSend((char*)szTemp, *getGameObject(tObjNum), 1);
			gObjTradeCancel(*getGameObject(tObjNum));
			sLog->outBasic("Opposite canceled trade: [%s][%s]", getGameObject(tObjNum)->AccountID, getGameObject(tObjNum)->Name);
		}

		gObjTradeCancel(Obj);
		sLog->outBasic("Cancel trade: [%s][%s]", Obj.AccountID, Obj.Name);
	}
}




void gObjBillRequest(CGameObject &Obj)
{
	char szMsg[128];

	if (m_ObjBill[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].GetPayCode() == 0)
	{
		if (m_ObjBill[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].GetCertify() == 0)
		{
			wsprintf(szMsg, Lang.GetText(0, 22));
		}

		else if (m_ObjBill[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].GetCertify() == 1)
		{
			if (m_ObjBill[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].GetEndTime() != 0)
			{
				wsprintf(szMsg, Lang.GetText(0, 23), m_ObjBill[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].GetEndTime(), g_VipSystem.GetVipName(Obj.m_PlayerData->VipType));
			}
		}
	}
	else if (m_ObjBill[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].GetPayCode() == 1)
	{
		char szTemp[20];
		szTemp[4] = 0;
		strncpy(szTemp, m_ObjBill[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].GetEndsDays(), 4);
		int Day = atoi(szTemp);

		strncpy(szTemp, m_ObjBill[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].GetEndsDays() + 4, 2);
		szTemp[2] = 0;
		int Month = atoi(szTemp);

		strncpy(szTemp, m_ObjBill[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].GetEndsDays() + 6, 2);
		szTemp[2] = 0;
		int Year = atoi(szTemp);

		if (m_ObjBill[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].GetCertify() == 0)	// Account Based
		{
			wsprintf(szMsg, Lang.GetText(0, 24), Day, Month, Year, g_VipSystem.GetVipName(Obj.m_PlayerData->VipType));
		}
		else if (m_ObjBill[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].GetCertify() == 1)	// IP Based
		{
			wsprintf(szMsg, Lang.GetText(0, 25), Day, Month, Year, g_VipSystem.GetVipName(Obj.m_PlayerData->VipType));
		}
	}
	else if (m_ObjBill[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].GetPayCode() == 3)
	{
		char szYear[5] = "";
		char szMonth[3] = "";
		char szDay[3] = "";
		char szHour[3] = "";
		char szMin[3] = "";

		strncpy(szYear, m_ObjBill[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].GetEndsDays(), 4);
		strncpy(szMonth, m_ObjBill[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].GetEndsDays() + 4, 2);
		strncpy(szDay, m_ObjBill[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].GetEndsDays() + 6, 2);
		strncpy(szHour, m_ObjBill[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].GetEndsDays() + 8, 2);
		strncpy(szMin, m_ObjBill[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].GetEndsDays() + 10, 2);

		wsprintf(szMsg, Lang.GetText(0, 148), m_ObjBill[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].GetEndTime(), szYear, szMonth,
			szDay, szHour, szMin);

		sLog->outBasic("[%s][%s] BillType : (Time) RemainPoint : (%d)",
			Obj.AccountID, Obj.Name, m_ObjBill[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].GetEndTime(), g_VipSystem.GetVipName(Obj.m_PlayerData->VipType));
	}
	else if (m_ObjBill[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].GetPayCode() == 4)
	{
		char szYear[5] = "";
		char szMonth[3] = "";
		char szDay[3] = "";

		strncpy(szYear, m_ObjBill[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].GetEndsDays(), 4);
		strncpy(szMonth, m_ObjBill[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].GetEndsDays() + 4, 2);
		strncpy(szDay, m_ObjBill[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].GetEndsDays() + 6, 2);

		wsprintf(szMsg, Lang.GetText(0, 149), szYear, szMonth, szDay, g_VipSystem.GetVipName(Obj.m_PlayerData->VipType));

		sLog->outBasic("[%s][%s] BillType : (Date) RemainDate : (%s-%s-%s)",
			Obj.AccountID, Obj.Name, szYear, szMonth, szDay);
	}
	else if (m_ObjBill[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].GetPayCode() == 5)		// FREE
	{
		wsprintf(szMsg, Lang.GetText(0, 150));
		sLog->outBasic("[%s][%s] BillType : (NoCharge)", Obj.AccountID, Obj.Name);
	}
	else	// Pospaid Account
	{
		wsprintf(szMsg, Lang.GetText(0, 26), g_VipSystem.GetVipName(Obj.m_PlayerData->VipType));
	}

	sLog->outBasic(szMsg);
	gGameProtocol.GCServerMsgStringSend(szMsg, Obj, 1);
}




short gObjMemFree(CGameObject &Obj)
{
	int i = 0;

	if (gDisconnect == TRUE)
	{
		return -1;
	}

	if (Obj.Connected < PLAYER_CONNECTED)
	{
		return false;
	}

	gObjTradeSave(Obj);

	if (Obj.Connected >= PLAYER_LOGGED)
	{
		if (Obj.Type == OBJ_USER)
		{
			if (Obj.Connected == PLAYER_PLAYING)
			{
				if (BC_MAP_RANGE(Obj.MapNumber))
				{
					g_BloodCastle.SearchUserDropQuestItem(Obj);
				}

				if (IT_MAP_RANGE(Obj.MapNumber))
				{
					g_IT_Event.DropRelicsItem(Obj.MapNumber, Obj);
					g_IT_Event.Leave_ITR(Obj, Obj.MapNumber);

					if (Obj.m_nITR_RelicsTick > 0 || Obj.m_wITR_NpcType > 0 || Obj.m_byITR_StoneState != 99 || Obj.m_bITR_RegisteringRelics == true || Obj.m_bITR_GettingRelics == true)
					{
						g_IT_Event.CancleStoneState(Obj.m_wITR_NpcType, Obj.m_byITR_StoneState, Obj.MapNumber);
						Obj.m_nITR_RelicsTick = 0;
						Obj.m_wITR_NpcType = 0;
						Obj.m_byITR_StoneState = 99;
						Obj.m_bITR_RegisteringRelics = false;
						Obj.m_bITR_GettingRelics = false;
					}
				}

				if (Obj.MapNumber == MAP_INDEX_DEVILSQUARE_FINAL)
				{
					g_DevilSquareFinal.Leave_DSF(Obj);
				}

				if (DG_MAP_RANGE(Obj.MapNumber))
				{
					g_DoppelGanger.LeaveDoppelganger(Obj);

					if (g_DoppelGanger.GetDoppelgangerState() == 2)
					{
						g_DoppelGanger.SendDoppelgangerResult(Obj, 1);
					}
				}

				// gGameProtocol.SetCharacterInfo(Obj, index, 0);  // TODO
			}

			// GJPUserClose(Obj); // TODO
		}
	}

	gObjViewportClose(Obj);
	Obj.Connected = PLAYER_EMPTY;
	return 1;
}





BOOL gObjGameClose(CGameObject &Obj)
{
	if (Obj.Connected != PLAYER_PLAYING)
	{
		return FALSE;
	}

	if (Obj.Type != OBJ_USER)
	{
		sLog->outBasic("Invalid character type causes failure to close (%s/%s)", Obj.AccountID, Obj.Name);
		return FALSE;
	}

	if (Obj.m_RecallMon >= 0)
	{
		gObjMonsterCallKill(Obj);
	}

	gObjMagicTextSave(Obj);

	if (((Obj.m_IfState->use != 0)
		&& (Obj.m_IfState->type == 7))
		|| ((Obj.m_IfState->use != 0)
			&& (Obj.m_IfState->type == 13)))
	{
		if (Obj.bIsChaosMixCompleted == true || Obj.m_bIsCastleNPCUpgradeCompleted == true)
		{
			gObjInventoryCommit(Obj);
		}
		else
		{
			gObjInventoryRollback(Obj);
		}
	}

	gObjTradeSave(Obj);

	if ((Obj.m_IfState->use != 0) && (Obj.m_IfState->type == 10))
	{
		if (Obj.TargetNpcNumber != -1 && getGameObject(Obj.TargetNpcNumber)->Class == 229)
		{
			gQeustNpcTeleport.TalkRefDel();
		}
	}

	if (Obj.m_IfState->use != 0)
	{
		if (Obj.TargetNpcNumber != -1 && getGameObject(Obj.TargetNpcNumber)->Class == 568)
		{
			gQuestExpNpcTeleport.TalkRefDel();
		}
	}

	if ((Obj.Authority & 2) == 2)
	{
		cManager.ManagerDel(Obj.Name);
	}

	g_NewPVP.Reset(Obj);
	gObjResetDuel(Obj);

	g_EvoMonMng.UserQuit(Obj);

	if (Obj.PartyNumber >= 0)
	{
		int iPartyIndex = gParty.GetIndex(Obj.PartyNumber, Obj, Obj.DBNumber);

		if (iPartyIndex >= 0)
		{
			PMSG_PARTYDELUSER pMsg;

			pMsg.Number = iPartyIndex;
			gGameProtocol.CGPartyDelUser(&pMsg, Obj, 0);
		}
	}

	if (Obj.m_PlayerData->GuildNumber > 0)
	{
		if (Obj.m_PlayerData->lpGuild->WarState == 1)
		{
			gObjGuildWarMasterClose(Obj);
		}

		GDCharClose(0, Obj.m_PlayerData->lpGuild->Name, Obj.Name);
	}
	else
	{
		GDCharClose(1, NULL, Obj.Name);
	}

	if (Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		Obj.m_bCCF_UI_Using = false;
		Obj.m_byCCF_CanEnter = 0;
		Obj.m_nCCF_CertiTick = 0;
		Obj.m_nCCF_UserIndex = -1;
		Obj.m_nCCF_BlowTime = 0;
		Obj.m_byKillUserCount = 0;
		Obj.m_byKillMobCount = 0;
		Obj.m_bCCF_Quit_Msg = false;
	}

	if (DS_MAP_RANGE(Obj.MapNumber))
	{
		g_DevilSquare.DelUser(Obj.m_bDevilSquareIndex, Obj);
	}

	if (BC_MAP_RANGE(Obj.MapNumber))
	{
		g_BloodCastle.SearchUserDropQuestItem(Obj);
	}

	if (IT_MAP_RANGE(Obj.MapNumber))
	{
		g_IT_Event.DropRelicsItem(Obj.MapNumber, Obj);
		g_IT_Event.Leave_ITR(Obj, Obj.MapNumber);

		if (Obj.m_nITR_RelicsTick > 0 || Obj.m_wITR_NpcType > 0 || Obj.m_byITR_StoneState != 99 || Obj.m_bITR_RegisteringRelics == true || Obj.m_bITR_GettingRelics == true)
		{
			g_IT_Event.CancleStoneState(Obj.m_wITR_NpcType, Obj.m_byITR_StoneState, Obj.MapNumber);
			Obj.m_nITR_RelicsTick = 0;
			Obj.m_wITR_NpcType = 0;
			Obj.m_byITR_StoneState = 99;
			Obj.m_bITR_RegisteringRelics = false;
			Obj.m_bITR_GettingRelics = false;
		}
	}

	if (Obj.MapNumber == MAP_INDEX_DEVILSQUARE_FINAL)
	{
		g_DevilSquareFinal.Leave_DSF(Obj);
	}

	if (DG_MAP_RANGE(Obj.MapNumber))
	{
		g_DoppelGanger.LeaveDoppelganger(Obj);

		if (g_DoppelGanger.GetDoppelgangerState() == 2)
		{
			g_DoppelGanger.SendDoppelgangerResult(Obj, 1);
		}
	}

	if (Obj.Type == OBJ_USER && Obj.MapNumber == g_LastManStanding.m_Cfg.iPVPMap && Obj.m_PlayerData->RegisterdLMS == 1)
	{
		g_LastManStanding.UserQuit(Obj);
	}

	gObjSaveChaosBoxItemList(Obj);

	if (gObjFixInventoryPointer(Obj) == false)
	{
		sLog->outBasic("[Fix Inv.Ptr] False Location - %s, %d", __FILE__, __LINE__);
	}

	g_PeriodItemEx.RemovePeriodInfo(Obj);
	g_CMuunSystem.RemoveMuunItemPeriodInfo(Obj);
	g_CMuRummyMng.GDReqMuRummyInfoUpdate(Obj);

	if (Obj.m_PlayerData->m_bIsMining)
	{
		g_MineSystem.CheckIsUPTUserWhenDisconnected(Obj);
	}

	UserChatBlockMng::getInstance()->deleteSlot(Obj.m_Index);

	GJSetCharacterInfo(Obj, 0);
	gObjViewportClose(Obj);
	sLog->outBasic("(%d)(%s)(%s) Character closed", Obj, Obj.AccountID, Obj.Name);

	if (Obj.Name)
	{
		WhisperCash.DelCash(Obj.Name);
	}

	gObjClearBuffEffect(Obj, CLEAR_TYPE_LOGOUT);

	std::memcpy(Obj.BackName, Obj.Name, sizeof(Obj.Name) - 1);
	Obj.BackName[10] = 0;

	Obj.Connected = PLAYER_LOGGED;
	Obj.m_PlayerData->m_bUsePartyMatching = false;
	Obj.m_PlayerData->m_bPartyMatchingLeader = false;
	Obj.m_PlayerData->m_bUseGuildMatching = false;
	Obj.m_PlayerData->m_bSendNpcPos = FALSE;

	if (m_ObjBill[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].GetCertify() >= 0 && m_ObjBill[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].GetCertify() < 3)
	{
		gLCount[m_ObjBill[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].GetCertify()].Delete();
	}

	Obj.m_nPhaseLordSilvester = 0;

	for (int k = 0; k < 3; k++)
		Obj.m_nRecallMonIndex[k] = -1;

	return TRUE;
}





short gObjDel(CGameObject &Obj)
{
	int i = 0;

	if (Obj.Connected < PLAYER_CONNECTED)
	{
		return 0;
	}

	if (MAX_MAP_RANGE(Obj.MapNumber))
	{
		MapC[Obj.MapNumber].ClearStandAttr(Obj.m_OldX, Obj.m_OldY);
		MapC[Obj.MapNumber].ClearStandAttr(Obj.X, Obj.Y);
	}

	if (IMPERIAL_MAP_RANGE(Obj.MapNumber) == TRUE)
	{
		sLog->outBasic("[IMPERIALGUARDIAN_TEST] gObjDel() => Class:%d, Index:%d", Obj.Class, Obj.m_Index);
	}

	int UserConnectState = Obj.Connected;

	if (Obj.Type == OBJ_USER && Obj.m_PlayerData->ISBOT == false && Obj.m_bOff == false)
	{
		gObjGameClose(Obj);

		if (UserConnectState >= PLAYER_LOGGED)
		{
			if (Obj.m_bMapSvrMoveQuit == false && Obj.m_bOff == false)
			{
				GJPUserClose(Obj);
			}
			else
			{

			}

			memset(Obj.AccountID, 0, sizeof(Obj.AccountID) - 1);
			memset(Obj.m_PlayerData->Password, 0, sizeof(Obj.m_PlayerData->Password));
			memset(Obj.m_PlayerData->HWID, 0, sizeof(Obj.m_PlayerData->HWID));
		}
	}

	if (Obj.Type == OBJ_USER && Obj.m_PlayerData->ISBOT == false && Obj.m_bOff == false)
	{
		Obj.Connected = PLAYER_EMPTY;
	}

	else
	{
		Obj.Connected = PLAYER_EMPTY;
	}

	if (Obj.Type == OBJ_USER)
	{
		Obj.m_PlayerData->m_bUsePartyMatching = false;
		Obj.m_PlayerData->m_bPartyMatchingLeader = false;
		Obj.m_PlayerData->m_nServerChannel = -1;
		Obj.m_PlayerData->m_nPartyMatchingIndex = -1;
		Obj.m_PlayerData->m_bSendNpcPos = FALSE;
		Obj.m_PlayerData->VipType = 0;
		Obj.m_bPShopOpen = false;
		if (g_OffLevel.FindUser(Obj) != -1)
		{
			g_OffLevel.DeleteUser(Obj);
		}
	}

	Obj.m_bGMSummon = false;
	Obj.m_nPhaseLordSilvester = 0;

	for (int k = 0; k < 3; k++)
		Obj.m_nRecallMonIndex[k] = -1;

	eraseGameObject(&Obj);

	return 1;
}




short gObjSometimeClose(CGameObject &Obj)
{
	if (gDisconnect == 1)
	{
		return -1;
	}

	gObjTradeSave(Obj);

	if (Obj.Connected > PLAYER_CONNECTED)
	{
		if (Obj.Type == OBJ_USER)
		{
			if (Obj.Connected == PLAYER_PLAYING)
			{
				if (Obj.PartyNumber >= 0)
				{
					PMSG_PARTYDELUSER pMsg;
					int iPartyIndex = gParty.GetIndex(Obj.PartyNumber, Obj, Obj.DBNumber);

					if (iPartyIndex >= 0)
					{
						pMsg.Number = iPartyIndex;
						gGameProtocol.CGPartyDelUser(&pMsg, Obj, 0);
					}
				}
			}
		}
	}

	return 1;
}





BOOL gObjIsGamePlaing(CGameObject &Obj)
{
	if (Obj.Connected != PLAYER_PLAYING)
	{
		return false;
	}

	return TRUE;
}



BOOL gObjIsConnectedGP(CGameObject &Obj)
{
	if (Obj.Type != OBJ_USER)
	{
		return FALSE;
	}

	if (Obj.Connected != PLAYER_PLAYING)
	{
		return FALSE;
	}

	if (Obj.CloseCount > 0)
	{
		return FALSE;
	}

	return TRUE;
}



BOOL gObjIsConnectedGP(CGameObject &Obj, char * CharName)
{
	if (Obj.Type != OBJ_USER)
	{
		return FALSE;
	}

	if (Obj.Connected != PLAYER_PLAYING)
	{
		return FALSE;
	}

	if (Obj.CloseCount > 0)
	{
		return FALSE;
	}

	if (strcmp(Obj.Name, CharName) != 0)
	{
		return FALSE;
	}

	return TRUE;
}


bool gObjIsAccountValid(CGameObject &Obj, char* account) // OK
{
	if (Obj.Connected == PLAYER_EMPTY)
	{
		return 0;
	}

	if (strcmp(account, "") == 0)
	{
		return 0;
	}

	if (strcmp(Obj.AccountID, account) != 0)
	{
		return 0;
	}

	return 1;
}


BOOL gObjIsConnected(CGameObject &Obj, int dbnumber)
{
	if (Obj.Connected < PLAYER_PLAYING)
	{
		return 0;
	}

	if (dbnumber > -1 && Obj.DBNumber != dbnumber)
	{
		return 0;
	}

	return 1;
}

BOOL gObjIsConnectedEx(CGameObject &Obj)
{
	if (Obj.Type != OBJ_USER)	return FALSE;
	if (Obj.Connected != PLAYER_PLAYING) return FALSE;
	if (Obj.CloseCount >= 0)	return FALSE;
	return TRUE;
}

BOOL gObjIsAccontConnect(CGameObject &Obj, char* accountid)
{
	if (Obj.Connected < PLAYER_CONNECTED)
	{
		return 0;
	}

	if (strlen(&Obj.AccountID[0]) < 1)
	{
		return 0;
	}

	if (strcmp(accountid, &Obj.AccountID[0]) != 0)
	{
		return 0;
	}
	return 1;
}






BOOL gObjPasswordCheck(CGameObject &Obj, char* szInPassword)
{
	if (Obj.Type != OBJ_USER)
	{
		return FALSE;
	}

	if (strcmp(Obj.m_PlayerData->Password, szInPassword) == 0)
	{
		return TRUE;
	}

	else
	{
		return FALSE;
	}
}

BOOL gObjCheckXYMapTile(CGameObject &Obj, int iDbgName)
{
	if (Obj.Type != OBJ_USER)
	{
		return false;
	}

	if ((Obj.Authority & 2) == 2)
	{
		return FALSE;
	}

	if (CC_MAP_RANGE(Obj.MapNumber))
	{
		if (g_ChaosCastle.GetCurrentState(g_ChaosCastle.GetChaosCastleIndex(Obj.MapNumber)) == 2)
		{
			return FALSE;
		}
	}

	if (Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
		{
			if (g_ChaosCastleFinal.GetCurrentState() == 2)
			{
				return FALSE;
			}
		}
	}

	int x = Obj.X;
	int y = Obj.Y;
	int mapnumber = Obj.MapNumber;

	if (mapnumber > MAX_NUMBER_MAP - 1)
	{
		mapnumber = MAP_INDEX_RORENCIA;
	}

	if (Obj.MapNumber != MAP_INDEX_LORENMARKET && (Obj.MapNumber != MAP_INDEX_ARCA_WAR || Obj.MapNumber != MAP_INDEX_DEBENTER_ARCA_WAR || g_ArcaBattle.IsArcaBattleServer() == FALSE) && Obj.MapNumber != MAP_INDEX_ACHERON && Obj.MapNumber != MAP_INDEX_DEBENTER)
	{
		if (Obj.Class == 2 && Obj.Level < 10)
		{
			if (Obj.MapNumber != MAP_INDEX_NORIA)
			{
				sLog->outBasic("%s %d %d %d Invalid location causes to force to move", Obj.Name, Obj.MapNumber, Obj.X, Obj.Y);
				PMSG_TELEPORT pMsg;
				pMsg.MoveNumber = 27;
				gObjMoveGate(Obj, pMsg.MoveNumber);
			}
		}

		else
		{
			if (Obj.Level < 0)
			{
				sLog->outBasic("%s %d %d %d Invalid location causes to force to move", Obj.Name, Obj.MapNumber, Obj.X, Obj.Y);
				PMSG_TELEPORT pMsg;
				pMsg.MoveNumber = 17;
				gObjMoveGate(Obj, pMsg.MoveNumber);
			}
		}
	}

	BYTE attr = MapC[mapnumber].GetAttr(x, y);

	if ((attr & 4) == 4 || (attr & 8) == 8)
	{
		BYTE attr2 = MapC[mapnumber].GetAttr(x + 2, y);
		BYTE attr3 = MapC[mapnumber].GetAttr(x, y + 2);
		BYTE attr4 = MapC[mapnumber].GetAttr(x - 2, y);
		BYTE attr5 = MapC[mapnumber].GetAttr(x, y - 2);

		if (attr2 > 1 && attr3 > 1 && attr4 > 1 && attr5 > 1)
		{
			sLog->outBasic("[ CHECK POSITION ] DbgName[%d] [%s][%s] Map[%d]-(%d,%d) Invalid location causes to force to move", iDbgName, Obj.AccountID, Obj.Name, Obj.MapNumber, Obj.X, Obj.Y);

			PMSG_TELEPORT pMsg;

			if (IT_MAP_RANGE(Obj.MapNumber))
			{
				if (g_IT_Event.GetIllusionTempleState(Obj.MapNumber) == 2)
				{
					BYTE btTeam = g_IT_Event.GetUserTeam(Obj.MapNumber, Obj.m_Index);

					if (btTeam == 0xFF)
					{
						pMsg.MoveNumber = 17;
					}

					else if (btTeam == 1)
					{
						pMsg.MoveNumber = Obj.MapNumber + 109;
					}

					else if (btTeam == 0)
					{
						pMsg.MoveNumber = Obj.MapNumber + 103;
					}

					else
					{
						pMsg.MoveNumber = 17;
					}
				}

				else
				{
					pMsg.MoveNumber = 17;
				}

				gObjMoveGate(Obj, pMsg.MoveNumber);
				return TRUE;
			}

			else if (Obj.MapNumber == MAP_INDEX_DEVILSQUARE_FINAL && g_DevilSquareFinal.m_btDSFCurState == 2)
			{
				if (g_DevilSquareFinal.GetUserTeam(Obj) == -1)
				{
					pMsg.MoveNumber = 17;
				}

				else
				{
					pMsg.MoveNumber = 474;
				}

				gObjMoveGate(Obj, pMsg.MoveNumber);
				return TRUE;
			}

			else
			{
				gObjMoveGate(Obj, 17);
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL gObjSetAccountLogin(CGameObject &Obj, char * szId, int aUserNumber, int aDBNumber, char* Password)
{
	if (Obj.Connected != PLAYER_CONNECTED)
	{
		sLog->outBasic("error-L2: [%d] User(%s) is not connected. DBNumber: %d %s %d", Obj.m_Index, szId, aDBNumber, __FILE__, __LINE__);
		return 0;
	}

	if (strcmp(Obj.AccountID, szId) != 0)
	{
		sLog->outBasic("error: %s did not request to authenticate.", szId);
		return 0;
	}

	for each (std::pair<int,CGameObject*> ObjEntry in gGameObjects)
	{
		CGameObject* lpObj = ObjEntry.second;
		if (!strcmpi(szId, lpObj->AccountID))
		{
			if (lpObj->m_bOff)
			{
				lpObj->m_bOff = false;
				gObjDel(*lpObj);
				break;
			}
		}
	}

	Obj.UserNumber = aUserNumber;
	Obj.DBNumber = aDBNumber;
	Obj.Connected = PLAYER_LOGGED;
	strcpy(Obj.m_PlayerData->Password, Password);

	gGameProtocol.GCJoinBillCheckSend(Obj.AccountID, Obj);
	return TRUE;

}





BOOL gObjGetUserNumber(CGameObject &Obj)
{
	int usernumber;

	usernumber = Obj.UserNumber;
	return usernumber;
}





BOOL gObjGetNumber(CGameObject &Obj, int& UN, int& DBN)
{
	UN = Obj.UserNumber;
	DBN = Obj.DBNumber;
	return 1;
}





// gObj Address 653DBF8
char* gObjGetAccountId(CGameObject &Obj)
{
	if (Obj.Connected == 2)
	{
		return &Obj.AccountID[0];
	}
	return 0;
}


BOOL gObjUserIdConnectCheck(char* szId, int index)
{
	if (index < 0 || index > g_ConfigRead.server.GetObjectMax())	// Deathway Fix Here
	{
		sLog->outBasic("error : index error !! %s %d", __FILE__, __LINE__);
		return 0;
	}

	if (getGameObject(index)->Connected != PLAYER_PLAYING)
	{
		return 0;
	}

	if (getGameObject(index)->Name[0] != *szId)
	{
		return 0;
	}

	if (strcmp(getGameObject(index)->Name, szId) == 0)
	{
		return 1;
	}
	return 0;
}





BYTE GetPathPacketDirPos(int px, int py)
{
	BYTE pos = 0;

	if (px <= -1 && py <= -1)
	{
		pos = 0;
	}
	else if (px <= -1 && py == 0)
	{
		pos = 7;
	}
	else if (px <= -1 && py >= 1)
	{
		pos = 6;
	}
	else if (px == 0 && py <= -1)
	{
		pos = 1;
	}
	else if (px == 0 && py >= 1)
	{
		pos = 5;
	}
	else if (px >= 1 && py <= -1)
	{
		pos = 2;
	}
	else if (px >= 1 && py == 0)
	{
		pos = 3;
	}
	else if (px >= 1 && py >= 1)
	{
		pos = 4;
	}

	return pos;
}





int  gObjCalDistance(CGameObject &Obj1, CGameObject &Obj2)
{
	if (&Obj1 == NULL || &Obj2 == NULL)
	{
		return 0;
	}

	if (Obj1.X == Obj2.X && Obj1.Y == Obj2.Y)
	{
		return 0;
	}

	float tx = Obj1.X - Obj2.X;
	float ty = Obj1.Y - Obj2.Y;

	return sqrt((tx*tx) + (ty*ty));
}





BOOL gObjPositionCheck(CGameObject &Obj)
{
	int ix = Obj.m_OldX - Obj.TX;
	int iy = Obj.m_OldY - Obj.TY;

	if (ix <= -15 || ix >= 15)
	{
		return FALSE;
	}

	if (iy <= -15 || iy >= 15)
	{
		return FALSE;
	}

	return TRUE;
}





BOOL gObjCheckTileArea(CGameObject &Obj, int x, int y, int dis)
{
	

	if (x < (Obj.X - dis) || x >(Obj.X + dis) || y < (Obj.Y - dis) || y >(Obj.Y + dis))
	{
		return FALSE;
	}
	return TRUE;
}





BOOL ExtentCheck(int x, int y, int w, int h)
{
	if ((x >= 0) && (x < w) && (y >= 0) && (y < h))
	{
		return 1;
	}
	return 0;
}

//00566240	-> 100% (1.01.00) [NEW]
BOOL InventoryExtentCheck(int x, int y, int w, int h)
{
	int nEndOfRange, nStartOfRange;
	// ----
	if (y < 0)
	{
		return false;
	}
	// ----
	if (y >= 8)	//-> Need rewrite too)
	{
		if (y >= 12)
		{
			if (y >= 16)
			{
				if (y >= 20)
				{
					if (y >= 24)
					{
						if (y >= 28)
						{
							return false;
						}
						// ----
						nStartOfRange = 24;
						nEndOfRange = 28;
					}
					else
					{
						nStartOfRange = 20;
						nEndOfRange = 24;
					}
				}
				else
				{
					nStartOfRange = 16;
					nEndOfRange = 20;
				}
			}
			else
			{
				nStartOfRange = 12;
				nEndOfRange = 16;
			}
		}
		else
		{
			nStartOfRange = 8;
			nEndOfRange = 12;
		}
	}
	else
	{
		nStartOfRange = 0;
		nEndOfRange = 8;
	}
	// ----
	if (nEndOfRange > h)
	{
		nEndOfRange = h;
	}
	// ----
	return x >= 0 && x < w && y >= nStartOfRange && y < nEndOfRange;
}

//00566370	-> 100% (1.01.00) [NEW]
BOOL CheckOutOfInventory(CGameObject &Obj, int sy, int height)
{
	int nEndofItemY = sy + height - 1;
	// ----

	if (sy >= 4 * Obj.m_PlayerData->m_InventoryExpansion + 8 && (sy < 24 || sy > 27))
	{
		return -1;
	}

	if (sy < 0)
	{
		return false;
	}
	// ----
	if (sy >= 8) //-> Need rewrite xD
	{
		if (sy >= 12)
		{
			if (sy >= 16)
			{
				if (sy >= 20)
				{
					if (sy >= 24)
					{
						if (sy >= 28)
						{
							return false;
						}
						// ----
						if (nEndofItemY >= 28)
						{
							return false;
						}
					}
					else
					{
						if (nEndofItemY >= 24)
						{
							return false;
						}
					}
				}
				else
				{
					if (nEndofItemY >= 20)
					{
						return false;
					}
				}
			}
			else
			{
				if (nEndofItemY >= 16)
				{
					return false;
				}
			}
		}
		else
		{
			if (nEndofItemY >= 12)
			{
				return false;
			}
		}
	}
	else
	{
		if (nEndofItemY >= 8)
		{
			return false;
		}
	}
	// ----
	return true;
}

//005664f0	-> 100% (1.01.00) [NEW]
BOOL CheckOutOfWarehouse(CGameObject &Obj, int sy, int height)
{
	if (sy >= 15 * Obj.m_PlayerData->m_WarehouseExpansion + 15)
	{
		return false;
	}
	// ----
	int nEndofItemY = sy + height - 1;
	// ----
	if (sy < 0)
	{
		return false;
	}
	// ----
	if (sy >= 15)
	{
		if (sy >= 30)
		{
			return false;
		}
		// ----
		if (nEndofItemY >= 30)
		{
			return false;
		}
	}
	else
	{
		if (nEndofItemY >= 15)
		{
			return false;
		}
	}
	// ----
	return true;
}

//00566440	-> 100% (1.01.00) [NEW]
BOOL WarehouseExtentCheck(int x, int y, int w, int h)
{
	int nEndOfRange, nStartOfRange;
	// ----
	if (y < 0)
	{
		return false;
	}
	// ----
	if (y >= 15)
	{
		if (y >= 30)
		{
			return false;
		}
		// ----
		nStartOfRange = 15;
		nEndOfRange = 30;
	}
	else
	{
		nStartOfRange = 0;
		nEndOfRange = 15;
	}
	// ----
	if (nEndOfRange > h)
	{
		nEndOfRange = h;
	}
	// ----
	return x >= 0 && x < w && y >= nStartOfRange && y < nEndOfRange;
}



void gObjSetInventory1Pointer(CGameObject &Obj)
{
	Obj.pntInventory = Obj.pntInventory1;
	Obj.pInventoryMap = *Obj.InventoryMap1;
	Obj.pInventoryCount = &Obj.InventoryCount1;
}



void gObjSetInventory2Pointer(CGameObject &Obj)
{
	Obj.pntInventory = Obj.pntInventory2;
	Obj.pInventoryMap = *Obj.InventoryMap2;
	Obj.pInventoryCount = &Obj.InventoryCount2;
}

void gObjAddMsgSend(CGameObject &Obj, int aMsgCode, CGameObject &ObjTarget, int  SubCode)
{
	for (int n = 0; n < MAX_MONSTER_SEND_MSG; n++)
	{
		if (gSMMsg[Obj.m_Index][n].MsgCode < 0)
		{
			gSMMsg[Obj.m_Index][n].MsgCode = aMsgCode;
			gSMMsg[Obj.m_Index][n].MsgTime = GetTickCount64();
			gSMMsg[Obj.m_Index][n].SendUser = Obj.m_Index;
			gSMMsg[Obj.m_Index][n].SubCode = SubCode;
			return;
		}
	}

	for (int n = 0; n < MAX_MONSTER_SEND_MSG; n++)
	{
		if (gSMMsg[Obj.m_Index][n].MsgCode == 2 || gSMMsg[Obj.m_Index][n].MsgCode == 6 || gSMMsg[Obj.m_Index][n].MsgCode == 7)
		{
			gSMMsg[Obj.m_Index][n].MsgCode = aMsgCode;
			gSMMsg[Obj.m_Index][n].MsgTime = GetTickCount64();
			gSMMsg[Obj.m_Index][n].SendUser = Obj.m_Index;
			gSMMsg[Obj.m_Index][n].SubCode = SubCode;

			return;
		}
	}
}




void gObjAddMsgSendDelay(CGameObject &Obj, int aMsgCode, CGameObject &ObjTarget, int delay, int SubCode)
{
	for (int n = 0; n < MAX_MONSTER_SEND_MSG; n++)
	{
		if (gSMMsg[Obj.m_Index][n].MsgCode < 0)
		{
			gSMMsg[Obj.m_Index][n].MsgCode = aMsgCode;
			gSMMsg[Obj.m_Index][n].MsgTime = GetTickCount64() + delay;
			gSMMsg[Obj.m_Index][n].SendUser = ObjTarget.m_Index;
			gSMMsg[Obj.m_Index][n].SubCode = SubCode;

			return;
		}
	}

	for (int n = 0; n < MAX_MONSTER_SEND_MSG; n++)
	{
		if (gSMMsg[Obj.m_Index][n].MsgCode == 2 || gSMMsg[Obj.m_Index][n].MsgCode == 6 || gSMMsg[Obj.m_Index][n].MsgCode == 7)
		{
			gSMMsg[Obj.m_Index][n].MsgCode = aMsgCode;
			gSMMsg[Obj.m_Index][n].MsgTime = GetTickCount64();
			gSMMsg[Obj.m_Index][n].SendUser = ObjTarget.m_Index;
			gSMMsg[Obj.m_Index][n].SubCode = SubCode;

			return;
		}
	}

}



void gObjAddMsgSendDelayInSpecificQPos(CGameObject &Obj, int aMsgCode, CGameObject &ObjTarget, int delay, int SubCode, int iQPosition)
{
	if (iQPosition >= 0 && iQPosition < MAX_MONSTER_SEND_MSG)
	{
		gSMMsg[Obj.m_Index][iQPosition].MsgCode = aMsgCode;
		gSMMsg[Obj.m_Index][iQPosition].MsgTime = GetTickCount64();
		gSMMsg[Obj.m_Index][iQPosition].SendUser = Obj.m_Index;
		gSMMsg[Obj.m_Index][iQPosition].SubCode = SubCode;
	}

}




void gObjAddAttackProcMsgSendDelay(CGameObject &Obj, int aMsgCode, CGameObject &ObjTarget, int delay, int SubCode, int SubCode2)
{
	for (int n = 0; n < MAX_MONSTER_SEND_ATTACK_MSG; n++)
	{
		int iIndex = Obj.m_Index;

		if (gSMAttackProcMsg[iIndex][n].MsgCode < 0)
		{
			gSMAttackProcMsg[iIndex][n].MsgCode = aMsgCode;
			gSMAttackProcMsg[iIndex][n].MsgTime = GetTickCount64() + delay;
			gSMAttackProcMsg[iIndex][n].SendUser = Obj.m_Index;
			gSMAttackProcMsg[iIndex][n].SubCode = SubCode;
			gSMAttackProcMsg[iIndex][n].SubCode2 = SubCode2;

			return;
		}
	}

}



void gObjMsgProc(CGameObject &Obj)
{
	for (int n = 0; n < MAX_MONSTER_SEND_MSG; n++)
	{
		CGameObject* lpObjTarget = getGameObject(gSMMsg[Obj.m_Index][n].SendUser);
		if (gSMMsg[Obj.m_Index][n].MsgCode >= 0)
		{
			if (GetTickCount64() > gSMMsg[Obj.m_Index][n].MsgTime)
			{
				if (Obj.Type == OBJ_MONSTER || Obj.Type == OBJ_NPC)
				{
					gObjMonsterStateProc(Obj, gSMMsg[Obj.m_Index][n].MsgCode, *lpObjTarget, gSMMsg[Obj.m_Index][n].SubCode);
				}
				else
				{
					gObjStateProc(Obj, gSMMsg[Obj.m_Index][n].MsgCode, *lpObjTarget, gSMMsg[Obj.m_Index][n].SubCode);
				}

				gSMMsg[Obj.m_Index][n].MsgCode = -1;
			}
		}
	}
}




void gObjStateProc(CGameObject &Obj, int aMsgCode, CGameObject &ObjTarget, int SubCode)
{
	if (&Obj == NULL)
	{
		sLog->outBasic("error : %s %d", __FILE__, __LINE__);
		return;
	}
	CGameObject* lpObjTarget = getGameObject(Obj.TargetNumber);

	switch (aMsgCode)
	{
	case  2:
		gObjBackSpring(Obj, *lpObjTarget);
		break;

	case  3:
		gObjMonsterDieLifePlus(Obj, *lpObjTarget);
		break;

	case 1000:
		gObjBillRequest(Obj);
		break;

	case 1001:
		gObjReqMapSvrAuth(Obj);
		break;

	case  4:
		if (Obj.Type == OBJ_USER && Obj.Connected == 3)
		{
			if (Obj.m_PlayerData->lpGuild != NULL && Obj.m_PlayerData->lpGuild->lpTargetGuildNode != NULL)
			{
				gObjGuildWarEnd(*Obj.m_PlayerData->lpGuild, *Obj.m_PlayerData->lpGuild->lpTargetGuildNode);
			}
		}
		break;

	case  5:
		if (Obj.Type == OBJ_USER && Obj.Connected == 3)
		{
			if (Obj.m_PlayerData->lpGuild != NULL && Obj.m_PlayerData->lpGuild->WarState != NULL && Obj.m_PlayerData->lpGuild->WarType == 1)
			{
				BattleSoccerGoalStart(0);
			}
		}
		break;

	case  7:
		if (Obj.Type == OBJ_USER && Obj.Connected == 3)
		{
			if (Obj.m_PlayerData->lpGuild != NULL && Obj.m_PlayerData->lpGuild->WarState != NULL && Obj.m_PlayerData->lpGuild->WarType == 1)
			{
				if (Obj.m_PlayerData->lpGuild->BattleGroundIndex >= 0)
				{
					gGameProtocol.GCManagerGuildWarEnd(Obj.m_PlayerData->lpGuild->Name);
				}
			}
		}
		break;

	case 10:
		if (Obj.DamageReflect != 0)
		{
			gObjAttack(Obj, *lpObjTarget, 0, 0, 0, SubCode, 0, 0, 1);
		}
		break;
	case 12:
		gObjAttack(Obj, *lpObjTarget, 0, 0, 0, SubCode, 0, 0, 0);
		break;
	case 13:
		Obj.Life = Obj.MaxLife + Obj.AddLife;
		gGameProtocol.GCReFillSend(Obj, Obj.Life, -1, 0, Obj.iShield);
		break;
	case 14:
		Obj.Mana = Obj.MaxMana + Obj.AddMana;
		gGameProtocol.GCManaSend(Obj, Obj.Mana, -1, 0, Obj.BP);
		break;
	case 15:
		Obj.iShield = Obj.iMaxShield + Obj.iAddShield;
		gGameProtocol.GCReFillSend(Obj, Obj.Life, -1, 0, Obj.iShield);
		break;
	case 16:
		Obj.Life += SubCode;
		if (Obj.Life > Obj.AddLife + Obj.MaxLife)
		{
			Obj.Life = Obj.AddLife + Obj.MaxLife;
		}
		gGameProtocol.GCReFillSend(Obj, Obj.Life, 0xFF, 0, Obj.iShield);
		break;
	case 54:
		if (Obj.Live)
		{
			CMagicInf * lpMagic = &DefMagicInf[79];
			gObjAttack(Obj, Obj, lpMagic, 1, 0, SubCode, 0, 0, 0);
		}
	case 58: //Eletric Surge
		if (Obj.Live)
		{
			CMagicInf * lpMagic = &DefMagicInf[216]; //Eletrict Surge
			gObjAttack(Obj, Obj, lpMagic, 1, 0, SubCode, 0, 0, 0);
		}
		break;
	case 59:
		if (Obj.Live)
		{
			CMagicInf * lpMagic = gObjGetMagicSearch(Obj, 225);
			if (lpMagic)
			{
				BYTE PosX = LOBYTE(LOWORD(SubCode));
				BYTE PosY = HIBYTE(LOWORD(SubCode));
				BYTE Msg = HIWORD(SubCode);
				gObjUseSkill.NewSkillProc(Obj, lpMagic, PosX, PosY, *lpObjTarget, Msg + 1, 0);
			}
		}
		break;
	case 65:
		if (Obj.Live)
		{
			CMagicInf * lpMagic = gObjGetMagicSearch(Obj, 339);
			if (lpMagic)
			{
				gObjUseSkill.NewSkillProc(Obj, lpMagic, 0, 0, *lpObjTarget, SubCode + 1, 0);
			}
		}
		break;
	}
}





void gObjStateAttackProc(CGameObject &Obj, int aMsgCode, CGameObject &ObjTarget, int SubCode, int SubCode2)
{
	switch (aMsgCode)
	{
	case 50:
		if (Obj.Live != FALSE)
		{
			CMagicInf * lpMagic = gObjGetMagicSearch(Obj, SubCode);

			if (!lpMagic)
				return;

			gObjAttack(Obj, ObjTarget, lpMagic, 0, 1, 0, SubCode2, 0, 0);

			if (lpMagic->m_Skill == AT_SKILL_SUICIDE)
			{
				Obj.m_PosNum = -1;
				Obj.Life = 0.0;

				gObjLifeCheck(Obj, Obj, 0, 1, 0, 0, 0, 0, 0);
			}
		}
		break;
	case 53:
		if (Obj.Live)
		{
			CMagicInf * lpMagic = gObjGetMagicSearch(Obj, SubCode);

			if (lpMagic != NULL)
			{
				gObjAttack(Obj, ObjTarget, lpMagic, 1, 1, 0, 0, 0, 0);
			}
		}
		break;
	case 51:
		if (Obj.Type == OBJ_USER && Obj.m_PlayerData->ISBOT == false && Obj.m_Index >= g_ConfigRead.server.GetObjectStartUserIndex())
		{
			gDarkSpirit[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].Attack(Obj, Obj, NULL, SubCode, SubCode2);
		}
		break;
	case 60:
		if (Obj.Live != FALSE)
		{
			CMagicInf * lpMagic = gObjGetMagicSearch(Obj, SubCode);

			if (lpMagic != NULL)
			{
				gObjAttack(Obj, ObjTarget, lpMagic, 1, 1, 0, SubCode2, 0, 0);
			}
		}
		break;
	case 61:
	{
		CMagicInf * lpMagic = gObjGetMagicSearch(Obj, SubCode);

		if (lpMagic != NULL)
		{
			if (lpMagic->m_Skill == 263)
			{
				gObjUseSkill.SkillMonkBarrageJustOneTarget(Obj, lpMagic, SubCode2);
			}

			else if (lpMagic->m_Skill == 559 || lpMagic->m_Skill == 563)
			{
				g_MasterLevelSkillTreeSystem.MLS_SkillMonkBarrageJustOneTarget(Obj, lpMagic, SubCode2);
			}
		}
	}
	break;
	case 63:
		g_CMuunSystem.Attack(Obj, Obj, 0, SubCode, SubCode2);
		break;
	}

}




BOOL gObjBackSpring(CGameObject &Obj, CGameObject &TargetObj)
{
	int tdir;
	int x;
	int y;
	BYTE attr;
	PMSG_POSISTION_SET pMove;

	if (MAX_MAP_RANGE(Obj.MapNumber) == FALSE)
	{
		sLog->outBasic("error : %s %d", __FILE__, __LINE__);
		return FALSE;
	}

	if (Obj.Type == OBJ_USER)
	{
		if (Obj.m_LastTeleportTime > 0)
		{
			return FALSE;
		}
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (Obj.Type == OBJ_USER)
		{
			if ((Obj.Authority & 0x02) == 2)
			{
				return FALSE;
			}
		}
	}

	if (Obj.Class >= 131 && Obj.Class <= 134)
	{
		return FALSE;
	}

	if (Obj.Class >= 523 && Obj.Class <= 528)
	{
		return FALSE;
	}

	if (Obj.Class >= 598 && Obj.Class <= 602)
	{
		return FALSE;
	}

	if (Obj.Class >= 627 && Obj.Class <= 631)
	{
		return FALSE;
	}

	if (Obj.Class == 275 || Obj.Class == 657 || Obj.Class == 673)
	{
		return FALSE;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (Obj.Class == 277 || //castle siege ??
			Obj.Class == 283 ||
			Obj.Class == 288 ||
			Obj.Class == 278 ||
			Obj.Class == 215 ||
			Obj.Class == 216 ||
			Obj.Class == 217 ||
			Obj.Class == 218 ||
			Obj.Class == 219)
		{
			return FALSE;
		}

		if (g_CastleSiege.GetCrownUserIndex() == Obj.m_Index)
		{
			if (Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 4))
			{
				return FALSE;
			}
		}

		if (g_Crywolf.GetCrywolfState() == 4 || g_Crywolf.GetCrywolfState() == 3)
		{
			if (Obj.Type == OBJ_USER)
			{
				for (int i = 205; i <= 209; i++)//crywolf altar
				{
					int iAltarIndex = g_CrywolfNPC_Altar.GetAltarUserIndex(i);

					if (iAltarIndex != -1)
					{
						if (iAltarIndex == Obj.m_Index)
						{
							return FALSE;
						}
					}
				}
			}
		}

		if (CRYWOLF_ALTAR_CLASS_RANGE(Obj.Class) || CHECK_CLASS(204, Obj.Class) != FALSE) //HermeX Fix
		{
			return FALSE;
		}
		if (Obj.Class == 348)//balista
		{
			return FALSE;
		}
	}

	if (Obj.Class == 204)
	{
		return FALSE;
	}

	//Season 4.0 add-on
	if (Obj.Class == 459 ||
		Obj.Class == 460 ||
		Obj.Class == 461 ||
		Obj.Class == 462)
	{
		return FALSE;
	}

	//Season 4.0 add-on
	if (Obj.Class == 459 ||
		Obj.Class == 460 ||
		Obj.Class == 461 ||
		Obj.Class == 462)
	{
		return FALSE;
	}

	int EffectID = -1;
	for (int i = 0; i < MAX_BUFFEFFECT; i++) //season4 add-on
	{
		EffectID = Obj.pntBuffEffectList[i]->BuffIndex;

		if (EffectID == BUFFTYPE_STONE ||
			EffectID == BUFFTYPE_STUN ||
			EffectID == BUFFTYPE_RESTRICTION ||
			EffectID == BUFFTYPE_FREEZE_2 ||
			EffectID == BUFFTYPE_EARTH_BINDS)
		{
			return FALSE;
		}
	}

	if ((rand() % 3) == 0)
	{
		if (Obj.GetTarget()->Dir < 4)
		{
			tdir = Obj.GetTarget()->Dir + 4;
		}
		else
		{
			tdir = Obj.GetTarget()->Dir - 4;
		}

		tdir *= 2;
	}
	else
	{
		tdir = Obj.GetTarget()->Dir * 2;
	}

	x = Obj.X;
	y = Obj.Y;
	x += RoadPathTable[tdir];
	y += RoadPathTable[1 + tdir];
	attr = MapC[Obj.MapNumber].GetAttr(x, y);

	if ((attr & 1) == 1 ||
		(attr & 2) == 2 ||
		(attr & 4) == 4 ||
		(attr & 8) == 8 ||
		(attr & 16) == 16
		)
	{
		return FALSE;
	}

	pMove.h.c = 0xC1;
	pMove.h.headcode = 0x11;
	pMove.h.size = sizeof(pMove);
	pMove.X = x;
	pMove.Y = y;

	Obj.m_Rest = 0;
	MapC[Obj.MapNumber].ClearStandAttr(Obj.m_OldX, Obj.m_OldY);
	MapC[Obj.MapNumber].SetStandAttr(x, y);
	Obj.m_OldX = x;
	Obj.m_OldY = y;
	gGameProtocol.RecvPositionSetProc(&pMove, Obj);

	if (Obj.Type == OBJ_USER)
	{
		Obj.PathCount = 0;
	}

	return TRUE;
}





BOOL BackSpringCheck(int & x, int & y, int & dir, BYTE map)
{
	BYTE attr;
	int tx = x;
	int ty = y;
	int tdir = dir / 2;

	tx = tx + RoadPathTable[dir];
	ty = ty + RoadPathTable[1 + dir];

	attr = MapC[map].GetAttr(tx, ty);

	if ((attr & 1) == 1 ||
		(attr & 4) == 4 ||
		(attr & 8) == 8 ||
		(attr & 16) == 16
		)
	{
		tdir += 4;

		if (tdir > 7)
		{
			tdir -= 8;
		}

		dir = tdir * 2;
		return FALSE;
	}

	x = tx;
	y = ty;

	return TRUE;
}

BOOL gObjBackSpring2(CGameObject &Obj, CGameObject &TargetObj, int count)
{
	int tdir;

	if (MAX_MAP_RANGE(Obj.MapNumber) == FALSE)
	{
		sLog->outBasic("error : %s %d", __FILE__, __LINE__);
		return FALSE;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (Obj.Type == OBJ_USER)
		{
			if ((Obj.Authority & 0x02) == 2)
			{
				return FALSE;
			}
		}
	}

	if (Obj.Class == 287 || Obj.Class == 286)
	{
		return FALSE;
	}

	if (Obj.Class >= 131 && Obj.Class <= 134)
	{
		return FALSE;
	}

	if (Obj.Class >= 523 && Obj.Class <= 528)
	{
		return FALSE;
	}

	if (Obj.Class >= 598 && Obj.Class <= 602)
	{
		return FALSE;
	}

	if (Obj.Class >= 627 && Obj.Class <= 631)
	{
		return FALSE;
	}

	if (Obj.Class == 275 || Obj.Class == 657 || Obj.Class == 673)
	{
		return FALSE;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (Obj.Class == 278)
		{
			return FALSE;
		}

		if (Obj.Class == 277 ||//castle siege ??
			Obj.Class == 283 ||
			Obj.Class == 288 ||
			Obj.Class == 278 ||
			Obj.Class == 215 ||
			Obj.Class == 216 ||
			Obj.Class == 217 ||
			Obj.Class == 218 ||
			Obj.Class == 219)
		{
			return FALSE;
		}

		if (g_CastleSiege.GetCrownUserIndex() == Obj.m_Index)
		{
			if (Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 4))
			{
				return FALSE;
			}
		}

		if (g_Crywolf.GetCrywolfState() == 4 || g_Crywolf.GetCrywolfState() == 3)
		{
			if (Obj.Type == OBJ_USER)
			{
				for (int i = 205; i <= 209; i++)//crywolf altar
				{
					int iAltarIndex = g_CrywolfNPC_Altar.GetAltarUserIndex(i);

					if (iAltarIndex != -1)
					{
						if (iAltarIndex == Obj.m_Index)
						{
							return FALSE;
						}
					}
				}
			}
		}

		if (CRYWOLF_ALTAR_CLASS_RANGE(Obj.Class) || CHECK_CLASS(204, Obj.Class) != FALSE) //HermeX Fix
		{
			return FALSE;
		}
		if (Obj.Class == 348)//balista
		{
			return FALSE;
		}
	}

	if (Obj.Class == 275) //Season 2.5 add-on (Kundun Fix)
	{
		return FALSE;
	}

	if (Obj.Class == 204)
	{
		return FALSE;
	}

	//Season 4.0 add-on
	if (Obj.Class == 459 ||
		Obj.Class == 460 ||
		Obj.Class == 461 ||
		Obj.Class == 462)
	{
		return FALSE;
	}

	int EffectID = -1;

	for (int i = 0; i < MAX_BUFFEFFECT; i++) //season4 add-on
	{
		EffectID = Obj.pntBuffEffectList[i]->BuffIndex;

		if (EffectID == BUFFTYPE_STONE ||
			EffectID == BUFFTYPE_STUN ||
			EffectID == BUFFTYPE_RESTRICTION ||
			EffectID == BUFFTYPE_FREEZE_2 ||
			EffectID == BUFFTYPE_EARTH_BINDS)
		{
			return FALSE;
		}
	}

	tdir = GetPathPacketDirPos(Obj.X - TargetObj.X, Obj.Y - TargetObj.Y) * 2;

	int x = Obj.X;
	int y = Obj.Y;

	for (int n = 0; n < count; n++)
	{
		if (n >= 2)
		{
			if (Obj.Class == 4 || Obj.Class == 3 || Obj.Class == 1)
			{
				BackSpringCheck(x, y, tdir, Obj.MapNumber);
			}
		}
		else
		{
			BackSpringCheck(x, y, tdir, Obj.MapNumber);
		}
	}

	PMSG_POSISTION_SET pMove;

	pMove.h.c = 0xC1;
	pMove.h.headcode = 0x11;
	pMove.h.size = sizeof(pMove);
	pMove.X = x;
	pMove.Y = y;

	Obj.m_Rest = 0;
	MapC[Obj.MapNumber].ClearStandAttr(Obj.m_OldX, Obj.m_OldY);
	MapC[Obj.MapNumber].SetStandAttr(x, y);
	Obj.m_OldX = x;
	Obj.m_OldY = y;
	gGameProtocol.RecvPositionSetProc(&pMove, Obj);

	return TRUE;
}





bool gObjLevelUp(CGameObject &Obj, UINT64 addexp, int iMonsterType, const char * szEventType)
{
	if (g_MasterLevelSkillTreeSystem.IsMasterLevelUser(Obj) == true)
	{
		return g_MasterLevelSkillTreeSystem.MasterLevelUp(Obj, addexp, iMonsterType, szEventType);
	}

	//sLog->outBasic("[%s] Experience : Map[%s]-(%d,%d) [%s][%s](%d) Experience: %I64d + %I64d  MonsterClass : %d",szEventType, Lang.GetMap(0, Obj.MapNumber),Obj.X,Obj.Y,Obj.AccountID,Obj.Name,Obj.Level,Obj.m_PlayerData->Experience - addexp,addexp,iMonsterType);

	if (Obj.Level >= g_ConfigRead.data.common.UserMaxLevel && Obj.m_PlayerData->Experience >= gLevelExperience[Obj.Level])
	{
		Obj.m_PlayerData->Experience = gLevelExperience[Obj.Level];
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 45), Obj, 1);
		return false;
	}

	gObjSetExpPetItem(Obj, addexp);
	if (Obj.m_PlayerData->Experience < Obj.m_PlayerData->NextExp)
	{
		return true;
	}

	Obj.m_PlayerData->Experience = Obj.m_PlayerData->NextExp;
	Obj.Level++;

	if (g_ConfigRead.data.reset.iBlockLevelUpPointAfterResets == -1 || Obj.m_PlayerData->m_iResets < g_ConfigRead.data.reset.iBlockLevelUpPointAfterResets)
	{

		if (Obj.Class == 4)
		{
			Obj.m_PlayerData->LevelUpPoint += g_MaxStatsInfo.GetClass.LevelUpPointMGDL;
		}

		else if (Obj.Class == 3)
		{
			Obj.m_PlayerData->LevelUpPoint += g_MaxStatsInfo.GetClass.LevelUpPointMGDL;
		}

		else if (Obj.Class == 6)
		{
			Obj.m_PlayerData->LevelUpPoint += g_MaxStatsInfo.GetClass.LevelUpPointMGDL;
		}

		else if (Obj.Class == 7)
		{
			Obj.m_PlayerData->LevelUpPoint += g_MaxStatsInfo.GetClass.LevelUpPointMGDL;
		}

		else
		{
			Obj.m_PlayerData->LevelUpPoint += g_MaxStatsInfo.GetClass.LevelUpPointNormal;
		}

		if (Obj.m_PlayerData->PlusStatQuestClear != false)
		{

			Obj.m_PlayerData->LevelUpPoint += 1;
			//sLog->outBasic("[%s][%s] LevelUp PlusStatQuest Clear AddStat %d",Obj.AccountID, Obj.Name, Obj.m_PlayerData->LevelUpPoint);

		}
	}

	gObjCalCharacter.CalcCharacter(Obj);
	g_CMuunSystem.CheckMuunItemConditionLevelUp(Obj);
	Obj.MaxLife += DCInfo.DefClass[Obj.Class].LevelLife;
	Obj.MaxMana += DCInfo.DefClass[Obj.Class].LevelMana;
	Obj.Life = Obj.MaxLife;
	Obj.Mana = Obj.MaxMana;

	Obj.Life = Obj.MaxLife + Obj.AddLife;
	Obj.Mana = Obj.MaxMana + Obj.AddMana;

	gObjCalCharacter.CalcShieldPoint(Obj);
	Obj.iShield = Obj.iMaxShield + Obj.iAddShield;
	gGameProtocol.GCReFillSend(Obj, Obj.Life, -1, 0, Obj.iShield);

	gObjNextExpCal(Obj);
	//sLog->outBasic("Level Up [%s][%s][%d]", Obj.AccountID, Obj.Name, Obj.Level);

	if (Obj.Class == CLASS_ELF)
	{
		if (Obj.Level >= g_SkillAdditionInfo.GetInfinityArrowUseLevel() && Obj.m_PlayerData->ChangeUP >= 1)
		{
			int iAddSkillPosition = gObjMagicAdd(Obj, 0x4D, 0);

			if (iAddSkillPosition >= 0)
			{
				gGameProtocol.GCMagicListOneSend(Obj, iAddSkillPosition, 0x4D, 0xDC, 0, 0);
				//sLog->outBasic("[%s][%s] Add Infinity Arrow Skill (Character Level : %d)(ChangeUp: %d)",	Obj.AccountID,Obj.Name,Obj.Level,Obj.m_PlayerData->ChangeUP);
			}
			else
			{
				//sLog->outBasic("[%s][%s] Fail - Add Infinity Arrow Skill (Character Level : %d)(ChangeUp: %d)",Obj.AccountID,Obj.Name,Obj.Level,Obj.m_PlayerData->ChangeUP);
			}
		}
	}

	//GJSetCharacterInfo(Obj, Obj, 0);
	gObjSetBP(Obj);
	gGameProtocol.GCLevelUpMsgSend(Obj, 1);
	gObjCalcMaxLifePower(Obj);

	return false;
}





BOOL gObjLevelUpPointAdd(BYTE type, CGameObject &Obj, CGameObject &ObjTarget)	// type : [1:Strength][2:Agility][3:Vitality][4:Energy][5:Leadership]
{
	if (Obj.Type != OBJ_USER)
	{
		sLog->outError("error-L1: LevelUp point value error %s %s %s %d", Obj.AccountID, Obj.Name, __FILE__, __LINE__);
		return FALSE;
	}

	if (Obj.m_PlayerData->LevelUpPoint < 1)
	{
		return FALSE;
	}

	switch (type)
	{
	case 0:
		if (Obj.m_PlayerData->Strength < g_MaxStatsInfo.GetClass.MaxStats[Obj.Class][0])
		{
			Obj.m_PlayerData->Strength++;
		}
		else
			return false;
		break;

	case 1:
		if (Obj.m_PlayerData->Dexterity < g_MaxStatsInfo.GetClass.MaxStats[Obj.Class][1])
		{
			Obj.m_PlayerData->Dexterity++;
		}
		else
			return false;
		break;

	case 2:
		if (Obj.m_PlayerData->Vitality < g_MaxStatsInfo.GetClass.MaxStats[Obj.Class][2])
		{
			Obj.m_PlayerData->Vitality++;

			Obj.MaxLife += Obj.VitalityToLife;
			gObjCalcMaxLifePower(Obj);
		}
		else
			return false;
		break;

	case 3:
		if (Obj.m_PlayerData->Energy < g_MaxStatsInfo.GetClass.MaxStats[Obj.Class][3])
		{
			Obj.m_PlayerData->Energy++;
			Obj.MaxMana += Obj.EnergyToMana;
		}
		else
			return false;
		break;

	case 4:
		if (Obj.Leadership < g_MaxStatsInfo.GetClass.MaxStats[Obj.Class][4])
		{
			Obj.Leadership++;
		}
		else
			return false;
		break;
	}

	Obj.m_PlayerData->LevelUpPoint--;
	gObjCalCharacter.CalcCharacter(Obj);


	return TRUE;
}

BOOL gObjGuildMasterCapacityTest(CGameObject &Obj)
{
	if (Obj.Level < g_ConfigRead.data.common.GuildCreateLevel)
	{
		return 0;
	}
	return 1;
}

void gObjNextExpCal(CGameObject &Obj)
{
	if (Obj.Type == OBJ_USER)
	{
		Obj.m_PlayerData->NextExp = gLevelExperience[Obj.Level];
	}
}

int  retResistance(CGameObject &Obj, int Resistance_Type)
{
	BYTE r = Obj.m_Resistance[Resistance_Type];

	if (r == 0xFF)
	{
		return 1;
	}

	if (r > 0 && gObjCheckUsedBuffEffect(Obj, BUFFTYPE_SOUL_POTION) != NULL && (Resistance_Type == 2 || Resistance_Type == 0))
	{
		r += r * 50 / 100;
	}

	if (r == 0)
	{
		return 0;
	}

	if ((rand() % (r + 1)) == 0)
	{
		return 0;
	}

	return 1;
}




BOOL retCalcSkillResistance(CGameObject &TargetObj, BOOL isDouble)
{
	if (isDouble != FALSE)
	{
		if ((rand() % 100) < 45)
		{
			return TRUE;
		}
	}
	else
	{
		if ((rand() % 100) < 40)
		{
			return TRUE;
		}
	}

	return FALSE;
}




BOOL gObjAttackQ(CGameObject &Obj)
{
	if (ATTRIBUTE_RANGE(Obj.m_Attribute))
	{
		return FALSE;
	}

	if (Obj.Class == 221 || Obj.Class == 222)
	{
		return FALSE;
	}

	if (g_MineSystem.IsTwinkle(Obj.Class) == TRUE)
	{
		return FALSE;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (Obj.Class == 277 || Obj.Class == 283)
		{
			if (g_CastleSiege.GetCastleState() != CASTLESIEGE_STATE_STARTSIEGE)
			{
				return FALSE;
			}
		}

		if (Obj.Class == 221 || Obj.Class == 222) //HermeX Decompilation
		{
			return FALSE;
		}

		if (Obj.Class == 277)
		{
			if (Obj.m_btCsGateOpen == 1)
			{
				return FALSE;
			}
		}
	}

	if (BC_MAP_RANGE(Obj.MapNumber))
	{
		if (Obj.Type >= OBJ_MONSTER)
		{
			int iBridgeIndex = g_BloodCastle.GetBridgeIndex(Obj.MapNumber);

			if (Obj.Class == 131 && g_BloodCastle.CheckMonsterKillSuccess(iBridgeIndex) == false)
			{
				return FALSE;
			}

			if ((((Obj.Class - 132) < 0) ? FALSE : ((Obj.Class - 132) > 2) ? FALSE : TRUE) != FALSE && g_BloodCastle.CheckBossKillSuccess(iBridgeIndex) == false)
			{
				return FALSE;
			}

		}
	}

	if ((Obj.Class >= 100 && Obj.Class <= 110) || Obj.Class == 523 || Obj.Class == 247 || Obj.Class == 249)
	{
		return FALSE;
	}

	if (Obj.Class == 689)
	{
		return FALSE;
	}

	if (Obj.m_State != 2 && Obj.m_bOffLevel == false)
	{
		return FALSE;
	}

	if (Obj.Live == false)
	{
		return false;
	}

	if (Obj.Teleport == 1)
	{
		return FALSE;
	}

	if (g_ConfigRead.EnableAttackBlockInSafeZone == TRUE)
	{
		BYTE btAttr = MapC[Obj.MapNumber].GetAttr(Obj.X, Obj.Y);

		if ((btAttr & 1) == 1)
		{
			return FALSE;
		}
	}

	return TRUE;
}

void gObjReady4Relife(CGameObject &Obj)
{
	gObjCalCharacter.CalcCharacter(Obj);
	gGameProtocol.GCReFillSend(Obj, Obj.MaxLife + Obj.AddLife, 0xFE, 0, Obj.iMaxShield + Obj.iAddShield);
}

#include "PVPZone.h"

void gObjPlayerKiller(CGameObject &Obj, CGameObject &TargetObj, int MSBDamage)
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	ReqSavePlayerKiller(Obj.m_Index, TargetObj.m_Index);

	if (TargetObj.Type == OBJ_USER)
	{
		for (int i = 0; i < MAX_PVPZONE; i++)
		{
			if ((TargetObj.MapNumber == g_PVPZone.m_Data[i].MapNumber) && ((TargetObj.X >= g_PVPZone.m_Data[i].PlayerKiller_PKX1)
				&& (TargetObj.X <= g_PVPZone.m_Data[i].PlayerKiller_PKX2)) && ((TargetObj.Y >= g_PVPZone.m_Data[i].PlayerKiller_PKY1)
					&& (TargetObj.Y <= g_PVPZone.m_Data[i].PlayerKiller_PKY2)))
			{
				return;
			}
		}
	}

	for (int r = 0; r < LMS_MAX_ROOMS; r++)
	{
		if (g_LastManStanding.m_Rooms[r].bState == 3 || g_LastManStanding.m_Rooms[r].bState == 4)
		{
			for (int i = 0; i < LMS_MAX_USER_PER_ROOM; i++)
			{
				if (g_LastManStanding.m_Rooms[r].m_Data[i].iIndex == Obj.m_Index)
				{
					return;
				}
			}
		}
	}

	if (gObjGetRelationShip(Obj, TargetObj) == 2)
	{
		if (Obj.m_PlayerData->lpGuild != NULL && TargetObj.m_PlayerData->lpGuild != NULL)
		{

		}
		else
		{

		}
		return;
	}

	if (CC_MAP_RANGE(TargetObj.MapNumber))
	{
		if (g_ChaosCastle.GetCurrentState(g_ChaosCastle.GetChaosCastleIndex(TargetObj.MapNumber)) == 2)
		{
			return;
		}
	}

	if (TargetObj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		if (g_ChaosCastleFinal.GetCurrentState() == 2)
		{
			return;
		}
	}

	if (Obj.MapNumber == MAP_INDEX_DEVILSQUARE_FINAL || TargetObj.MapNumber == MAP_INDEX_DEVILSQUARE_FINAL)
	{
		return;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (TargetObj.MapNumber == MAP_INDEX_CASTLESIEGE)
		{
			if (g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
			{
				if (Obj.m_btCsJoinSide != 0)
				{
					return;
				}
			}

			if (TargetObj.m_PK_Level >= 6)
			{
				return;
			}
		}
	}

	if ((Obj.Authority & 16) || (TargetObj.Authority & 16))
	{
		return;
	}

	if (IT_MAP_RANGE(Obj.MapNumber) || IT_MAP_RANGE(TargetObj.MapNumber))
	{
		return;
	}

	if (g_GensSystem.IsMapBattleZone(Obj.MapNumber) && g_GensSystem.IsMapBattleZone(TargetObj.MapNumber))
	{
		return;
	}

	if (g_ArcaBattle.IsArcaBattleServer() == TRUE)
	{
		return;
	}

	if (g_MapAttr.CheckPkInc(Obj.MapNumber) == FALSE)
	{
		MsgOutput(Obj, Lang.GetText(0, 412));
		return;
	}

	g_GensSystem.SendPKPenaltyDebuff(Obj);

	for (int n = 0; n < MAX_SELF_DEFENSE; n++)
	{
		if (TargetObj.SelfDefense[n] >= 0)
		{
			if (TargetObj.SelfDefense[n] == Obj.m_Index)
			{
				return;
			}
		}
	}

	if (TargetObj.m_PK_Level >= 5)
	{
		return;
	}

	int iOriginPkLevel = Obj.m_PK_Level;
	int iOriginPkCount = Obj.m_PK_Count;

	if (MSBDamage == 4) //season3 add-on
	{
		return;
	}

	if (g_ConfigRead.pk.bPkLevelIncreaseOff == true)
	{
		return;
	}

	BYTE btDefaultPartyPkLevel = 3; //Season 2.5 add-on

	if (TargetObj.PartyNumber >= 0) //Season 2.5 add-on
	{
		if (gParty.GetPKPartyPenalty(TargetObj.PartyNumber) >= 5)
		{
			btDefaultPartyPkLevel = gParty.GetPKPartyPenalty(TargetObj.PartyNumber);
		}
		else
		{
			btDefaultPartyPkLevel = TargetObj.m_PK_Level;
		}
	}
	else
	{
		btDefaultPartyPkLevel = TargetObj.m_PK_Level;
	}

	if (Obj.m_PK_Level == 3)
	{
		if (((char)btDefaultPartyPkLevel) == 3) //Season 2.5 changed
		{
			Obj.m_PK_Count = 1;
		}
		else if (((char)btDefaultPartyPkLevel) < 3) //Season 2.5 changed
		{
			Obj.m_PK_Count = 1;
		}
		else if (((char)btDefaultPartyPkLevel) == 4) //Season 2.5 changed
		{
			Obj.m_PK_Count = 1;
		}
		else
		{
			if (((char)btDefaultPartyPkLevel) >= 6 && TargetObj.Level > 20) //Season 2.5 changed
			{
				Obj.m_PK_Count--;

				if (Obj.m_PK_Count < -3)
				{
					Obj.m_PK_Count = (BYTE)-3;
					return;
				}
			}
			else
			{
				return;
			}
		}
	}
	else if (Obj.m_PK_Level > 3)
	{
		if (((char)btDefaultPartyPkLevel) <= 4) //Season 2.5 changed
		{
			Obj.m_PK_Count++;
			//	if(Obj.m_PK_Count > 100)
			//	{
			//	Obj.m_PK_Count = 100;
			//	}
		}
		else
		{
			return;
		}
	}
	else if (Obj.m_PK_Level < 3)
	{
		if (((char)btDefaultPartyPkLevel) == 3) //Season 2.5 changed
		{
			Obj.m_PK_Count = 1;
		}
		else if (((char)btDefaultPartyPkLevel) < 3) //Season 2.5 changed
		{
			Obj.m_PK_Count = 1;
		}
		else if (((char)btDefaultPartyPkLevel) == 4) //Season 2.5 changed
		{
			Obj.m_PK_Count = 1;
		}
		else
		{
			if (((char)btDefaultPartyPkLevel) >= 6 && TargetObj.Level > 20) //Season 2.5 changed
			{
				Obj.m_PK_Count--;

				if (Obj.m_PK_Count < -3)
				{
					Obj.m_PK_Count = -3;
				}
			}
			else
			{
				return;
			}
		}
	}

	if (Obj.m_PK_Count == 0)
	{
		Obj.m_PK_Level = 3;
		Obj.m_PK_Time = 0;
	}
	else if (Obj.m_PK_Count == 1)
	{
		Obj.m_PK_Level = 4;
		Obj.m_PK_Time = 0;
	}
	else if (Obj.m_PK_Count == 2)
	{
		Obj.m_PK_Level = 5;
		Obj.m_PK_Time = 0;

		if (Obj.PartyNumber >= 0) //Season 2.5 add-on
		{
			gParty.UpdatePKUserInfo(Obj.PartyNumber, Obj, Obj.DBNumber, Obj.m_PK_Level);
			gParty.UpdatePKPartyPanalty(Obj.PartyNumber);
		}
	}
	else if (Obj.m_PK_Count >= 3)
	{
		Obj.m_PK_Level = 6;
		Obj.m_PK_Time = 0;

		if (Obj.PartyNumber >= 0) //Season 2.5 add-on
		{
			gParty.UpdatePKUserInfo(Obj.PartyNumber, Obj, Obj.DBNumber, Obj.m_PK_Level);
			gParty.UpdatePKPartyPanalty(Obj.PartyNumber);
		}
	}
	else if (Obj.m_PK_Count <= -3)
	{
		Obj.m_PK_Level = 1;
		Obj.m_PK_Time = 0;
	}

	gGameProtocol.GCPkLevelSend(Obj, Obj.m_PK_Level);

	if (Obj.m_PK_Level == 4)
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 46), Obj, 1);
	}

	if (Obj.m_PlayerData->m_bUseGuildMatching)
	{
		if (Obj.m_PlayerData->GuildNumber > 0)
			GDReqDelMatchingList(Obj, Obj.m_PlayerData->GuildNumber);
		else
			GDReqCancelJoinGuildMatching(Obj, Obj.Name, 0);
	}

	if (Obj.m_PlayerData->m_bUsePartyMatching)
	{
		if (Obj.m_PlayerData->m_bPartyMatchingLeader)
		{
			GDReqCancelPartyMatching(Obj, 0);
		}

		else
		{
			if (Obj.PartyNumber >= 0)
			{
				gParty.Delete(Obj.PartyNumber, Obj, Obj.DBNumber);
				Obj.PartyNumber = -1;
			}

			GDReqCancelPartyMatching(Obj, 1);
		}
	}

}

void gObjUserDie(CGameObject &Obj, CGameObject& lpTargetObj)
{
	int count = 76;
	int itemdrop = 0;
	int number = 0;
	int dropresult = 0;

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	gObjSetKillCount(Obj, 0);

	gObjRemoveBuffEffect(Obj, CLEAR_TYPE_NON_PCS_ITEM_EFFECT);

	gObjUseSkill.RemoveAllCharacterInvalidMagicAndSkillState(Obj);

	if (gObjTargetGuildWarCheck(Obj, lpTargetObj) == 1)
	{
		return;
	}

	if (Obj.m_bOffLevel == true)
	{
		Obj.m_bOff = false;
		Obj.m_bOffLevel = false;
	}

	if (DS_MAP_RANGE(Obj.MapNumber))
	{
		if (lpTargetObj.Type == OBJ_MONSTER)
		{

			LPMONSTER_ATTRIBUTE mAttr = gMAttr.GetAttr(lpTargetObj.Class);

			if (mAttr != 0)
			{
				sLog->outBasic("[DevilSquare] Dead In DevilSquare [%s][%s][%s]", Obj.AccountID, Obj.Name, mAttr->m_Name);
			}
		}

		return;
	}

	if (Obj.MapNumber == MAP_INDEX_DEVILSQUARE_FINAL)
	{
		g_DevilSquareFinal.DSFUserDie(Obj);
	}

	if (BC_MAP_RANGE(Obj.MapNumber))
	{
		g_BloodCastle.SetUserState(Obj, 1);

		int iBridgeIndex = g_BloodCastle.GetBridgeIndex(Obj.MapNumber);

		if (g_BloodCastle.GetCurrentState(iBridgeIndex + 1) == BC_STATE_PLAYING)
		{
			sLog->outBasic("[Blood Castle] (%d) Try to drop Ultimate Weapon [%s][%s]", iBridgeIndex + 1, Obj.AccountID, Obj.Name);
			g_BloodCastle.SearchUserDropQuestItem(Obj);
		}
		else
		{
			sLog->outBasic("[Blood Castle] (%d) Try to delete Ultimate Weapon [%s][%s]", iBridgeIndex + 1, Obj.AccountID, Obj.Name);
			g_BloodCastle.SearchUserDeleteQuestItem(Obj);
		}

		if (lpTargetObj.Type == OBJ_MONSTER)
		{
			LPMONSTER_ATTRIBUTE mAttr = gMAttr.GetAttr(lpTargetObj.Class);

			if (mAttr != 0)
			{
				sLog->outBasic("[Blood Castle] (%d) Dead In Blood Castle, Killed by Monster [%s][%s][%s]", iBridgeIndex + 1, Obj.AccountID, Obj.Name, mAttr->m_Name);
				g_BloodCastle.SearchUserDropQuestItem(Obj);
			}
		}
		else if (lpTargetObj.Type == OBJ_USER)
		{
			sLog->outBasic("[Blood Castle] (%d) Dead In Blood Castle, Killed by Other User [%s][%s][%s]", iBridgeIndex + 1, Obj.AccountID, Obj.Name, lpTargetObj.Name);
			g_BloodCastle.SearchUserDropQuestItem(Obj);
		}
		return;
	}

	if (Obj.Type == OBJ_USER && lpTargetObj.Type == OBJ_USER)
	{
		if (Obj.MapNumber == g_LastManStanding.m_Cfg.iPVPMap)
		{
			g_LastManStanding.UserDie(Obj, lpTargetObj.m_Index);
		}
	}

	if (g_AcheronGuardianEvent.IsPlayStart() == true)
	{
		return;
	}

	if (g_ArcaBattle.IsArcaBattleServer() == TRUE && g_ArcaBattle.GetState() == 7)
	{
		g_ArcaBattle.AddContributePoint(lpTargetObj, Obj);
		g_ArcaBattle.AddKillPoint(lpTargetObj, Obj);
		return;
	}

	if (IT_MAP_RANGE(Obj.MapNumber))
	{
		g_IT_Event.IllusionTempleUserDie(Obj);
	}

	else if (CC_MAP_RANGE(Obj.MapNumber))
	{
		return;
	}

	else if (Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		return;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE && Obj.MapNumber == MAP_INDEX_CASTLESIEGE)
		{
			if (g_CastleSiege.CastleSiegeResetAccumulationAfterKill == TRUE)
			{
				Obj.m_iAccumulatedCrownAccessTime = 0;
			}

			return;
		}
	}

	if ((Obj.Authority & 0x10) || (lpTargetObj.Authority & 0x10))
	{
		return;
	}

	if (lpTargetObj.Type == OBJ_MONSTER)
	{
		gGameProtocol.ChatTargetSend(lpTargetObj, Lang.GetText(0, 335), Obj.m_Index);
	}
	else
	{

	}

	g_UserDieItemDrop.Drop(Obj, lpTargetObj);

	g_GensSystem.CalcContributePoint(lpTargetObj, Obj);

	Obj.DieMapNumber = Obj.MapNumber;
	Obj.DieX = Obj.X;
	Obj.DieY = Obj.Y;

	int iItemDropRate = g_ConfigRead.pk.GetItemDropRate(Obj.m_PK_Level, (lpTargetObj.Type == 1 && Obj.Type == 1));

	if (rand() % 100 < iItemDropRate)
	{
		itemdrop = TRUE;
	}

	if (gObjCanItemTouch(Obj, 0) == 0)
	{
		itemdrop = FALSE;
	}

	if (Obj.Type == OBJ_USER && lpTargetObj.Type == OBJ_USER)
	{
		if (IsOnDuel(Obj, lpTargetObj))
		{
			itemdrop = FALSE;
		}
	}

	if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_CHARM_PROTECTITEM) != 0)
	{
		itemdrop = FALSE;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (g_Crywolf.GetCrywolfState() == 4)
		{
			if (CRYWOLF_MAP_RANGE(Obj.MapNumber))
			{
				itemdrop = FALSE;
			}
		}
	}

	if (Obj.Type == OBJ_USER && Obj.m_PlayerData->RegisterdLMS == TRUE) // port to x9
	{
		itemdrop = FALSE;
	}

	if (lpTargetObj.Type == OBJ_USER)
	{
		if (g_GensSystem.IsMapBattleZone(lpTargetObj.MapNumber) &&
			g_GensSystem.IsMapBattleZone(Obj.MapNumber))
		{
			return;
		}

		else
		{
			g_GensSystem.PkPenaltyDropInvenItem(Obj);
		}
	}

	if (itemdrop == TRUE)
	{
		if (Obj.m_PK_Level >= 4)
		{
			count = 24;

			while (count-- != 0)
			{
				number = rand() % 12;

				if (Obj.pntInventory[number]->IsItem() == 1 && Obj.pntInventory[number]->m_Level < g_ConfigRead.pk.iPkMaxLevelItemDrop)
				{
					if (g_kJewelOfHarmonySystem.IsStrengthenByJewelOfHarmony(Obj.pntInventory[number]) == 0 && IsCashItem(Obj.pntInventory[number]->m_Type) == 0)
					{
						PMSG_ITEMTHROW lpMsg;
						lpMsg.Ipos = number;
						lpMsg.px = Obj.X;
						lpMsg.py = Obj.Y;
						if (gGameProtocol.CGItemDropRequest(&lpMsg, Obj, 1) == 1)
						{
							dropresult = 1;
							break;
						}
					}
				}
			}
		}

		if (dropresult == 0)
		{
			count = INVENTORY_MAP_SIZE;

			while (count-- != 0)
			{
				number = rand() % INVENTORY_MAP_SIZE + 12;

				if (Obj.pntInventory[number]->IsItem() == 1 && Obj.pntInventory[number]->m_Level <= g_ConfigRead.pk.iPkMaxLevelItemDrop)
				{
					if (g_kJewelOfHarmonySystem.IsStrengthenByJewelOfHarmony(Obj.pntInventory[number]) == 0 && IsCashItem(Obj.pntInventory[number]->m_Type) == 0)
					{
						if (Obj.pntInventory[number]->m_Type == ITEMGET(13, 20) && (Obj.pntInventory[number]->m_Level >= 1 && Obj.pntInventory[number]->m_Level <= 2))
						{
							continue;
						}

						if (g_PentagramSystem.IsPentagramItem(Obj.pntInventory[number]->m_Type) == true ||
							g_PentagramSystem.IsPentagramJewel(Obj.pntInventory[number]->m_Type) == true)
						{
							continue;
						}

						PMSG_ITEMTHROW lpMsg;
						lpMsg.Ipos = number;
						lpMsg.px = Obj.X;
						lpMsg.py = Obj.Y;

						if (gGameProtocol.CGItemDropRequest(&lpMsg, Obj, 1) == 1)
						{
							dropresult = 1;
							sLog->outBasic("[%s][%s] User Die Item Drop, ItemName:%s", Obj.AccountID, Obj.Name, Obj.pntInventory[number]->GetName());
							break;
						}
					}
				}
			}
		}
	}

	g_GensSystem.PkPenaltyDropZen(Obj);

	if (Obj.Type == OBJ_USER && lpTargetObj.Type == OBJ_USER)
	{
		if (IsOnDuel(Obj, lpTargetObj))
		{
			g_NewPVP.SetScore(lpTargetObj);
		}

		return;
	}

	UINT64 minexp = gLevelExperience[Obj.Level - 1];
	UINT64 maxexp = gLevelExperience[Obj.Level];

	if (g_MasterLevelSkillTreeSystem.IsMasterLevelUser(Obj) == TRUE)
	{
		minexp = g_MasterLevelSkillTreeSystem.gMasterExperience[Obj.m_PlayerData->MasterLevel];
		maxexp = g_MasterLevelSkillTreeSystem.gMasterExperience[Obj.m_PlayerData->MasterLevel + 1];
	}

	boost::multiprecision::int128_t subexp = 0;
	boost::multiprecision::int128_t decexp = 0;
	DWORD decexprate = 0;

	BYTE btDefaultPartyPkLevel = 3; //Season 2.5 add-on

	if (Obj.PartyNumber >= 0) //Season 2.5 add-on
	{
		if ((gParty.GetPKPartyPenalty(Obj.PartyNumber)) >= 5)
		{
			btDefaultPartyPkLevel = gParty.GetPKPartyPenalty(Obj.PartyNumber);
		}
		else
		{
			btDefaultPartyPkLevel = Obj.m_PK_Level;
		}
	}
	else
	{
		btDefaultPartyPkLevel = Obj.m_PK_Level;
	}

	if (Obj.Level > 10)
	{
		decexprate = g_ConfigRead.pk.GetExpDecreaseRate(btDefaultPartyPkLevel, Obj.Level + Obj.m_PlayerData->MasterLevel);
	}

	decexp = 0;

	if (decexprate > 0)
	{
		decexp = ((maxexp - minexp)*decexprate) / 1000;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (g_Crywolf.GetCrywolfState() == 4)
		{
			if (Obj.MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE)
			{
				decexp = 0;
			}
		}
	}

	if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_CHARM_GUARDIAN) == 0)
	{
		if (g_MasterLevelSkillTreeSystem.IsMasterLevelUser(Obj) != FALSE) //Season3 add-on
		{
			subexp = Obj.m_PlayerData->MasterExperience - decexp;
		}
		else
		{
			subexp = Obj.m_PlayerData->Experience - decexp;
		}

		if (subexp < minexp)
		{
			subexp = minexp;
		}
	}

	DWORD submoney = 0;

	if (Obj.m_PlayerData->Money > 0 && gObjCheckUsedBuffEffect(Obj, BUFFTYPE_CHARM_GUARDIAN) == 0)
	{
		int iMoneyLevel = 0;

		if (Obj.Level > 10)
		{
			iMoneyLevel = g_ConfigRead.pk.GetZenLossRate(Obj.m_PK_Level, (Obj.Type == OBJ_USER && lpTargetObj.Type == OBJ_USER));
		}

		if (iMoneyLevel > 0)
		{
			submoney = (DWORD)(Obj.m_PlayerData->Money * iMoneyLevel) / 100;
		}

		Obj.m_PlayerData->Money = Obj.m_PlayerData->Money - submoney;

		if (Obj.m_PlayerData->Money < 0)
		{
			Obj.m_PlayerData->Money = 0;
		}

	}

	if (Obj.WarehouseMoney > 0 && gObjCheckUsedBuffEffect(Obj, BUFFTYPE_CHARM_GUARDIAN) == 0)
	{
		submoney = 0;

		int iMoneyLevel = 0;

		if (Obj.Level > 10)
		{
			iMoneyLevel = g_ConfigRead.pk.GetZenLossRate(Obj.m_PK_Level, (Obj.Type == OBJ_USER && lpTargetObj.Type == OBJ_USER));
		}

		if (iMoneyLevel > 0)
		{
			submoney = (DWORD)(Obj.WarehouseMoney * iMoneyLevel) / 100;
		}

		Obj.WarehouseMoney = Obj.WarehouseMoney - submoney;

		if (Obj.WarehouseMoney < 0)
		{
			Obj.WarehouseMoney = 0;
		}

		GDSetWarehouseMoney(Obj);

	}

	if (g_MasterLevelSkillTreeSystem.IsMasterLevelUser(Obj) != FALSE) //Season3 add-on
	{
		Obj.m_PlayerData->MasterExperience = subexp.convert_to<UINT64>();
	}
	else
	{
		Obj.m_PlayerData->Experience = subexp.convert_to<UINT64>();
	}
}




void gObjInterfaceTimeCheck(CGameObject &Obj)
{
	int tObjNum;
	BOOL TimeCheck = FALSE;

	if (Obj.m_IfState->use == 0)
	{
		return;
	}

	if ((GetTickCount() - Obj.m_InterfaceTime) < 5000)
	{
		return;
	}

	if (Obj.m_IfState->state == 1)
	{
		return;
	}

	tObjNum = Obj.TargetNumber;
	CGameObject* target = getGameObject(tObjNum);
	if (tObjNum < 0)
	{
		return;
	}

	if (Obj.m_IfState->type == 1)
	{
		gGameProtocol.CGTradeResult(Obj, 3);
		gGameProtocol.CGTradeResult(*target, 3);

		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 47), *target, 1);
		TimeCheck = TRUE;
	}
	else
	{
		if (Obj.m_IfState->type == 2)
		{
			Obj.PartyTargetUser = -1;
			target->PartyTargetUser = -1;
			gGameProtocol.GCResultSend(*target, 0x41, FALSE);
			TimeCheck = TRUE;
		}
	}

	if (TimeCheck != FALSE)
	{
		Obj.m_IfState->use = 0;
		Obj.TargetNumber = -1;
		target->m_IfState->use = 0;
		target->TargetNumber = -1;
	}

}




void gObjPkDownTimeCheck(CGameObject &Obj, int TargetLevel)
{
	if (Obj.m_PK_Level == 3)
	{
		return;
	}

	if (TargetLevel >= 2)
	{
		Obj.m_PK_Time += TargetLevel / g_ConfigRead.pk.iPKPointDeductionDivisor;
	}

	else
	{
		Obj.m_PK_Time++;
	}

	if (Obj.m_PK_Level >= 3)
	{
		if (Obj.m_PK_Level > 3)
		{
			if (Obj.m_PK_Count <= 3)
			{
				if (Obj.m_PK_Count == 3)
				{
					if (Obj.m_PK_Time > g_ConfigRead.pk.GetPKReqPoint(5))
					{
						Obj.m_PK_Time = 0;
						Obj.m_PK_Count--;

						if (Obj.m_PK_Count > 0)
						{
							if (Obj.m_PK_Count == 2)
							{
								Obj.m_PK_Level = 5;
							}

							else if (Obj.m_PK_Count == 1)
							{
								Obj.m_PK_Level = 4;
							}
						}

						else
						{
							Obj.m_PK_Count = 0;
							Obj.m_PK_Level = 3;
						}

						if (Obj.m_PK_Level <= 5)
						{
							gGameProtocol.GCPkLevelSend(Obj, Obj.m_PK_Level);
						}

						if (Obj.m_PK_Level == 4)
						{
							gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 46), Obj, 1);

							if (Obj.PartyNumber >= 0)
							{
								gParty.UpdatePKUserInfo(Obj.PartyNumber, Obj, Obj.DBNumber, Obj.m_PK_Level);
								gParty.UpdatePKPartyPanalty(Obj.PartyNumber);
								gGameProtocol.CGPartyListAll(Obj.PartyNumber);
							}
						}
					}
				}

				else if (Obj.m_PK_Time > g_ConfigRead.pk.GetPKReqPoint(4))
				{
					Obj.m_PK_Time = 0;
					Obj.m_PK_Count--;

					if (Obj.m_PK_Count > 0)
					{
						if (Obj.m_PK_Count == 2)
						{
							Obj.m_PK_Level = 5;
						}

						else if (Obj.m_PK_Count == 1)
						{
							Obj.m_PK_Level = 4;
						}
					}

					else
					{
						Obj.m_PK_Count = 0;
						Obj.m_PK_Level = 3;
					}

					if (Obj.m_PK_Level <= 5)
					{
						gGameProtocol.GCPkLevelSend(Obj, Obj.m_PK_Level);
					}

					if (Obj.m_PK_Level == 4)
					{
						gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 46), Obj, 1);

						if (Obj.PartyNumber >= 0)
						{
							gParty.UpdatePKUserInfo(Obj.PartyNumber, Obj, Obj.DBNumber, Obj.m_PK_Level);
							gParty.UpdatePKPartyPanalty(Obj.PartyNumber);
							gGameProtocol.CGPartyListAll(Obj.PartyNumber);
						}
					}
				}
			}

			else if (Obj.m_PK_Time > g_ConfigRead.pk.iPKPointIncForMurderer)
			{
				Obj.m_PK_Time = 0;
				Obj.m_PK_Count--;

				if (Obj.m_PK_Count > 0)
				{
					if (Obj.m_PK_Count == 2)
					{
						Obj.m_PK_Level = 5;
					}

					else if (Obj.m_PK_Count == 1)
					{
						Obj.m_PK_Level = 4;
					}
				}

				else
				{
					Obj.m_PK_Count = 0;
					Obj.m_PK_Level = 3;
				}

				if (Obj.m_PK_Level <= 5)
				{
					gGameProtocol.GCPkLevelSend(Obj, Obj.m_PK_Level);
				}

				if (Obj.m_PK_Level == 4)
				{
					gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 46), Obj, 1);

					if (Obj.PartyNumber >= 0)
					{
						gParty.UpdatePKUserInfo(Obj.PartyNumber, Obj, Obj.DBNumber, Obj.m_PK_Level);
						gParty.UpdatePKPartyPanalty(Obj.PartyNumber);
						gGameProtocol.CGPartyListAll(Obj.PartyNumber);
					}
				}
			}
		}
	}

	else if (Obj.m_PK_Time > g_ConfigRead.pk.iPKPointIncForMurderer)
	{
		Obj.m_PK_Time = 0;
		Obj.m_PK_Count++;
		Obj.m_PK_Level++;

		gGameProtocol.GCPkLevelSend(Obj, Obj.m_PK_Level);
	}
}




BOOL gObjAngelSprite(CGameObject &Obj)
{
	if (Obj.Type != OBJ_USER)
	{
		return FALSE;
	}

	if (Obj.pntInventory[8]->IsItem() == FALSE)
	{
		return FALSE;
	}

	CItemObject * Angel = Obj.pntInventory[8];

	if (Angel->m_Type == ITEMGET(13, 0) && Angel->m_Durability > 0.0f)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL gObjSatanSprite(CGameObject &Obj)
{
	if (Obj.Type != OBJ_USER)
	{
		return FALSE;
	}

	if (Obj.pntInventory[8]->IsItem() == FALSE)
	{
		return FALSE;
	}

	CItemObject * Satan = Obj.pntInventory[8];

	if (Satan->m_Type == ITEMGET(13, 1) && Satan->m_Durability > 0.0f)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL gObjDemonSprite(CGameObject &Obj)
{
	if (Obj.Type != OBJ_USER)
	{
		return FALSE;
	}

	if (Obj.pntInventory[8]->IsItem() == FALSE)
	{
		return FALSE;
	}

	CItemObject * Demon = Obj.pntInventory[8];

	if (Demon->m_Type == ITEMGET(13, 64) && Demon->m_Durability > 0.0f)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL gObjSafeGuardSprite(CGameObject &Obj)
{
	if (Obj.Type != OBJ_USER)
	{
		return FALSE;
	}

	if (Obj.pntInventory[8]->IsItem() == FALSE)
	{
		return FALSE;
	}

	CItemObject * SafeGuard = Obj.pntInventory[8];

	if (SafeGuard->m_Type == ITEMGET(13, 65) && SafeGuard->m_Durability > 0.0f)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL gObjFindInventoryEquipment(CGameObject &Obj)
{
	

	if (Obj.Type != OBJ_USER)
	{
		return FALSE;
	}

	for (int i = INVETORY_WEAR_SIZE; i < MAIN_INVENTORY_SIZE; i++)
	{
		gObjSetInventoryEquipment(Obj, i);
	}

	return TRUE;
}

BOOL gObjSetInventoryEquipment(CGameObject &Obj, BYTE btPos)
{
	if (Obj.Type != OBJ_USER)
	{
		return FALSE;
	}

	if (Obj.pntInventory[btPos]->IsItem() == FALSE)
	{
		return FALSE;
	}

	switch (Obj.pntInventory[btPos]->m_Type)
	{
	case ITEMGET(13, 128):
	case ITEMGET(13, 129):
		if (Obj.pntInventory[btPos]->m_Durability == 254.0 && (Obj.m_PlayerData->m_btSculptPos < INVETORY_WEAR_SIZE || Obj.m_PlayerData->m_btSculptPos >= MAIN_INVENTORY_SIZE))
			Obj.m_PlayerData->m_btSculptPos = btPos;
		break;
	case ITEMGET(13, 130):
	case ITEMGET(13, 131):
	case ITEMGET(13, 132):
	case ITEMGET(13, 133):
		if (Obj.pntInventory[btPos]->m_Durability == 254.0 && (Obj.m_PlayerData->m_btCharmPos < INVETORY_WEAR_SIZE || Obj.m_PlayerData->m_btCharmPos >= MAIN_INVENTORY_SIZE))
			Obj.m_PlayerData->m_btCharmPos = btPos;
		break;
	case ITEMGET(13, 134):
		if (Obj.pntInventory[btPos]->m_Durability == 254.0 && (Obj.m_PlayerData->m_btArtifactPos < INVETORY_WEAR_SIZE || Obj.m_PlayerData->m_btArtifactPos >= MAIN_INVENTORY_SIZE))
			Obj.m_PlayerData->m_btArtifactPos = btPos;
		break;
	case ITEMGET(13, 177):
	case ITEMGET(13, 178):
	case ITEMGET(13, 179):
		if (Obj.pntInventory[btPos]->m_Durability == 254.0 && (Obj.m_PlayerData->m_btExpUpCharmPos < INVETORY_WEAR_SIZE || Obj.m_PlayerData->m_btExpUpCharmPos >= MAIN_INVENTORY_SIZE))
			Obj.m_PlayerData->m_btExpUpCharmPos = btPos;
		break;
	case ITEMGET(13, 2):
	case ITEMGET(13, 3):
	case ITEMGET(13, 4):
	case ITEMGET(13, 5):
	case ITEMGET(13, 37):
		if (Obj.pntInventory[btPos]->m_JewelOfHarmonyOption == 1 && (Obj.m_btInvenPetPos < 12 || Obj.m_btInvenPetPos >= 204))
		{
			Obj.m_btInvenPetPos = btPos;
		}
		else
		{

		}
		break;
	}

	return TRUE;
}

BOOL gObjInventoryEquipment(CGameObject &Obj)
{
	if (Obj.Type != OBJ_USER)
	{
		return FALSE;
	}

	if (Obj.m_PlayerData->m_btSculptPos >= INVETORY_WEAR_SIZE && Obj.m_PlayerData->m_btSculptPos < MAIN_INVENTORY_SIZE)
	{
		LPBUFF_EFFECT_DATE lpEffect = g_BuffEffectSlot.GetEffectDataFromItemCode(Obj.pntInventory[Obj.m_PlayerData->m_btSculptPos]->m_Type);

		if (lpEffect == NULL) return FALSE;

		if (Obj.pntInventory[Obj.m_PlayerData->m_btSculptPos]->m_Durability == 254.0)
		{
			gObjAddBuffEffect(Obj, lpEffect->BuffIndex);
		}

		else
		{
			Obj.m_PlayerData->m_btSculptPos = 0;
			gObjRemoveBuffEffect(Obj, lpEffect->BuffIndex);
		}
	}

	if (Obj.m_PlayerData->m_btCharmPos >= INVETORY_WEAR_SIZE && Obj.m_PlayerData->m_btCharmPos < MAIN_INVENTORY_SIZE)
	{
		LPBUFF_EFFECT_DATE lpEffect = g_BuffEffectSlot.GetEffectDataFromItemCode(Obj.pntInventory[Obj.m_PlayerData->m_btCharmPos]->m_Type);

		if (lpEffect == NULL) return FALSE;

		if (Obj.pntInventory[Obj.m_PlayerData->m_btCharmPos]->m_Durability == 254.0)
		{
			gObjAddBuffEffect(Obj, lpEffect->BuffIndex);
		}

		else
		{
			Obj.m_PlayerData->m_btCharmPos = 0;
			gObjRemoveBuffEffect(Obj, lpEffect->BuffIndex);
		}
	}

	if (Obj.m_PlayerData->m_btArtifactPos >= INVETORY_WEAR_SIZE && Obj.m_PlayerData->m_btArtifactPos < MAIN_INVENTORY_SIZE)
	{
		LPBUFF_EFFECT_DATE lpEffect = g_BuffEffectSlot.GetEffectDataFromItemCode(Obj.pntInventory[Obj.m_PlayerData->m_btArtifactPos]->m_Type);

		if (lpEffect == NULL) return FALSE;

		if (Obj.pntInventory[Obj.m_PlayerData->m_btArtifactPos]->m_Durability == 254.0)
		{
			gObjAddBuffEffect(Obj, lpEffect->BuffIndex);
		}

		else
		{
			Obj.m_PlayerData->m_btArtifactPos = 0;
			gObjRemoveBuffEffect(Obj, lpEffect->BuffIndex);
		}
	}

	if (Obj.m_PlayerData->m_btExpUpCharmPos >= INVETORY_WEAR_SIZE && Obj.m_PlayerData->m_btExpUpCharmPos < MAIN_INVENTORY_SIZE)
	{
		LPBUFF_EFFECT_DATE lpEffect = g_BuffEffectSlot.GetEffectDataFromItemCode(Obj.pntInventory[Obj.m_PlayerData->m_btExpUpCharmPos]->m_Type);

		if (lpEffect == NULL) return FALSE;

		if (Obj.pntInventory[Obj.m_PlayerData->m_btExpUpCharmPos]->m_Durability == 254.0)
		{
			gObjAddBuffEffect(Obj, lpEffect->BuffIndex);
		}

		else
		{
			Obj.m_PlayerData->m_btExpUpCharmPos = 0;
			gObjRemoveBuffEffect(Obj, lpEffect->BuffIndex);
		}
	}

	if (Obj.m_btInvenPetPos >= INVETORY_WEAR_SIZE && Obj.m_btInvenPetPos < MAIN_INVENTORY_SIZE)
	{
		LPBUFF_EFFECT_DATE lpEffect = g_BuffEffectSlot.GetEffectDataFromItemCode(Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type);

		//sLog->outBasic("[CRenewal][InvenPet][InvenPetBuff][%s][%s] PetIndex:[%d], Pos:[%d]",Obj.AccountID, Obj.Name, Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type, Obj.m_btInvenPetPos);

		if (lpEffect == NULL)
		{
			//sLog->outBasic("[CRenewal][InvenPet][InvenPetBuff][%s][%s] PetIndex:[%d], Pos:[%d] [Fail] lpBuffData is Null",Obj.AccountID, Obj.Name, Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type, Obj.m_btInvenPetPos);
			return FALSE;
		}

		if (Obj.pntInventory[Obj.m_btInvenPetPos]->m_JewelOfHarmonyOption == 1)
		{
			if (Obj.m_wInvenPet != (WORD)-1)
			{
				return FALSE;
			}

			gObjAddBuffEffect(Obj, lpEffect->BuffIndex);

			//sLog->outBasic("[CRenewal][InvenPet][ReqInvenPetBuff][%s][%s] PetIndex:[%d], Pos:[%d] [Success] Register InvenPet Buff",	Obj.AccountID, Obj.Name, Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type, Obj.m_btInvenPetPos);

			CItemObject * lpItem = Obj.pntInventory[Obj.m_btInvenPetPos];

			if (lpItem->m_Option1)
			{
				int addskill = gObjWeaponMagicAdd(Obj, lpItem->m_Special[0], lpItem->m_Level);

				if (addskill >= 0)
				{
					gGameProtocol.GCMagicListOneSend(Obj, addskill, lpItem->m_Special[0], lpItem->m_Level, 0, 0);
				}
			}

			Obj.m_wInvenPet = Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type;
		}

		else
		{
			gObjRemoveBuffEffect(Obj, lpEffect->BuffIndex);

			//sLog->outBasic("[CRenewal][InvenPet][RemoveInvenPetBuff][%s][%s] PetIndex:[%d], Pos:[%d] [Success] Remove InvenPet Buff",Obj.AccountID, Obj.Name, Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type, Obj.m_btInvenPetPos);

			int result = gObjMagicDel(Obj, Obj.pntInventory[Obj.m_btInvenPetPos]->m_Special[0], Obj.pntInventory[Obj.m_btInvenPetPos]->m_Level);

			if (result >= 0)
			{
				gGameProtocol.GCMagicListOneDelSend(Obj, result, Obj.pntInventory[Obj.m_btInvenPetPos]->m_Special[0], Obj.pntInventory[Obj.m_btInvenPetPos]->m_Level, 0, 0);
			}

			Obj.m_btInvenPetPos = 0;
		}
	}

	return TRUE;
}

BOOL gObjWingSprite(CGameObject &Obj)
{
	if (Obj.Type != OBJ_USER)
	{
		return FALSE;
	}

	if (Obj.pntInventory[7]->IsItem() == FALSE)
	{
		return FALSE;
	}

	CItemObject * Wing = Obj.pntInventory[7];

	if (GetWingType(Obj.pntInventory[7]->m_Type, WCF_ITEMNUMBER) != NONE_WING)
	{
		if (Wing->m_Durability > 0.0f)
		{
			return TRUE;
		}
	}

	return FALSE;
}




BOOL gObjUniriaSprite(CGameObject &Obj)
{
	if (Obj.m_btInvenPetPos == 0 || Obj.m_wInvenPet == (WORD)-1)
	{
		return FALSE;
	}

	CItemObject * Uniria = Obj.pntInventory[Obj.m_btInvenPetPos];

	if (Uniria->m_Type == ITEMGET(13, 2) && Uniria->m_Durability > 0.0f)
	{
		return TRUE;
	}

	return FALSE;
}

void gObjSpriteDamage(CGameObject &Obj, int damage)
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	int send_dur = 0;

	if (Obj.pntInventory[8]->IsItem() == FALSE)
	{
		return;
	}

	if (Obj.pntInventory[8]->IsPeriodItem() == TRUE)
	{
		return;
	}

	float fN = 10.0f;
	CItemObject * sprite = Obj.pntInventory[8];
	float fdamage = damage;

	if (sprite->m_Type == ITEMGET(13, 64) && g_ConfigRead.pet.DamageDisableForPet[DAMAGE_OFF_DEMON] == false)
	{
		fdamage = (damage * 3) / 10.0f;
		fdamage /= fN;
		sprite->m_Durability -= fdamage;
		send_dur = 1;
	}

	if (sprite->m_Type == ITEMGET(13, 106) && g_ConfigRead.pet.DamageDisableForPet[DAMAGE_OFF_UNICORN] == false)
	{
		fdamage = (damage * 3) / 10.0f;
		fdamage /= fN;
		sprite->m_Durability -= fdamage;
		send_dur = 1;
	}

	if (sprite->m_Type == ITEMGET(13, 123) && g_ConfigRead.pet.DamageDisableForPet[DAMAGE_OFF_SKELETON] == false)
	{
		fdamage = (damage * 3) / 10.0f;
		fdamage /= fN;
		sprite->m_Durability -= fdamage;
		send_dur = 1;
	}

	if (sprite->m_Type == ITEMGET(13, 65) && g_ConfigRead.pet.DamageDisableForPet[DAMAGE_OFF_SPIRIT_ANGEL] == false)
	{
		fdamage = (damage * 2) / 10.0f;
		fdamage /= fN;
		sprite->m_Durability -= fdamage;
		send_dur = 1;
	}

	if (sprite->m_Type == ITEMGET(13, 67) && g_ConfigRead.pet.DamageDisableForPet[DAMAGE_OFF_RUDOLF] == false)
	{
		if (Obj.m_PlayerData->m_MPSkillOpt->iMpsDurabilityReduction3 > 0.0)
		{
			fdamage = damage / Obj.m_PlayerData->m_MPSkillOpt->iMpsDurabilityReduction3;
		}

		fdamage = (fdamage * 3) / 10.0f;
		fdamage /= fN;
		sprite->m_Durability -= fdamage;
		send_dur = 1;
	}

	if (sprite->m_Type == ITEMGET(13, 80) && g_ConfigRead.pet.DamageDisableForPet[DAMAGE_OFF_PANDA] == false)
	{
		fdamage = (damage * 2) / 10.0f;
		fdamage /= fN;
		sprite->m_Durability -= fdamage;
		send_dur = 1;
	}

	if (sprite->m_Type == ITEMGET(13, 0) && g_ConfigRead.pet.DamageDisableForPet[DAMAGE_OFF_ANGEL] == false) // angel
	{
		if (Obj.m_PlayerData->m_MPSkillOpt->iMpsDurabilityReduction3 > 0.0)
		{
			fdamage = damage / Obj.m_PlayerData->m_MPSkillOpt->iMpsDurabilityReduction3;
		}

		fdamage = (damage * 3) / 10.0f;
		fdamage /= fN;
		sprite->m_Durability -= fdamage;
		send_dur = 1;
	}
	else if (sprite->m_Type == ITEMGET(13, 1) && g_ConfigRead.pet.DamageDisableForPet[DAMAGE_OFF_SATAN] == false)
	{
		if (Obj.m_PlayerData->m_MPSkillOpt->iMpsDurabilityReduction3 > 0.0)
		{
			fdamage = damage / Obj.m_PlayerData->m_MPSkillOpt->iMpsDurabilityReduction3;
		}

		fdamage = (fdamage * 2) / 10.0f;
		fdamage /= fN;
		sprite->m_Durability -= fdamage;
		send_dur = 1;
	}

	if (sprite->m_Durability < 0.0f)
	{
		sprite->m_Durability = 0.0f; // fixed durability error (previously gs was sending negative values)
	}

	if (send_dur != FALSE)
	{
		gGameProtocol.GCItemObjectDurSend(Obj, 8, sprite->m_Durability, 0);

		if (sprite->m_Durability < 1.0f)
		{
			int itemnum = sprite->m_Type;

			sLog->outBasic("[%s][%s] %s+%d Item is broken because durability is exhausted", Obj.AccountID, Obj.Name,
				Obj.pntInventory[8]->GetName(), Obj.pntInventory[8]->m_Level);
			gObjInventoryDeleteItem(Obj, 8);
			gGameProtocol.GCInventoryItemDeleteSend(Obj, 8, 1); // flag changed ( 0 -> 1 )
			gObjMakePreviewCharSet(Obj);
			gGameProtocol.GCEquipmentChange(Obj, 8);

			if (BC_MAP_RANGE(Obj.MapNumber) || CC_MAP_RANGE(Obj.MapNumber) || Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
			{
				if (itemnum == ITEMGET(13, 0) || itemnum == ITEMGET(13, 1))
				{
					int pos = -1;

					for (int i = MAX_PLAYER_EQUIPMENT; i < MAIN_INVENTORY_SIZE; i++)
					{
						if (Obj.pntInventory[i]->IsItem() == TRUE)
						{
							if (Obj.pntInventory[i]->m_Type == itemnum)
							{
								pos = i;
								break;
							}
						}
					}

					if (pos != -1)
					{
						BYTE ItemInfo[MAX_ITEM_INFO];
						BOOL DurSend;
						BOOL DurTargetSend;

						ItemByteConvert(ItemInfo, *Obj.pntInventory[pos]);
						gObjInventoryMoveItem(Obj, pos, 8, DurSend, DurTargetSend, 0, 0, ItemInfo);
						gGameProtocol.GCItemObjectMoveResultSend(Obj, 0, 8, ItemInfo);
						gGameProtocol.GCInventoryItemDeleteSend(Obj, pos, 0);
						gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 82), Obj, 1);
					}
				}
			}

			if (Obj.MapNumber == MAP_INDEX_ICARUS && itemnum == ITEMGET(13, 3) || Obj.MapNumber == MAP_INDEX_ICARUS && itemnum == ITEMGET(13, 37))
			{
				int pos = -1;

				for (int i = MAX_PLAYER_EQUIPMENT; i < MAIN_INVENTORY_SIZE; i++)
				{
					if (Obj.pntInventory[i]->IsItem() == TRUE)
					{
						if (Obj.pntInventory[i]->m_Type == ITEMGET(13, 3))
						{
							pos = i;
							break;
						}
					}
				}

				if (pos == -1)
				{
					if (Obj.pntInventory[7]->IsItem() == FALSE)
					{
						gObjMoveGate(Obj, 22);
					}
				}
				else
				{
					BYTE ItemInfo[MAX_ITEM_INFO];
					BOOL DurSend;
					BOOL DurTargetSend;

					ItemByteConvert(ItemInfo, *Obj.pntInventory[pos]);
					gObjInventoryMoveItem(Obj, pos, 8, DurSend, DurTargetSend, 0, 0, ItemInfo);
					gGameProtocol.GCItemObjectMoveResultSend(Obj, 0, 8, ItemInfo);
					gGameProtocol.GCInventoryItemDeleteSend(Obj, pos, 0);
					gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 82), Obj, 1);
				}
			}

			GCItemObjectListSend(Obj); // with this, user CAN NOT bug uniria
			g_PeriodItemEx.OnRequestPeriodItemList(Obj);
		}
	}
}




void gObjSecondDurDown(CGameObject &Obj)
{
	Obj.m_TimeCount += 1;

	int ret = 0;
	int reCalCharacter = 0;
	int	item_num[5];
	item_num[0] = 2;
	item_num[1] = 3;
	item_num[2] = 4;
	item_num[3] = 5;
	item_num[4] = 6;
	if (Obj.m_Change >= 0)
	{
		gObjChangeDurProc(Obj);
	}

	BYTE attr = MapC[Obj.MapNumber].GetAttr(Obj.X, Obj.Y);

	for (int i = 0; i < 5; i++)
	{
		CItemObject * DurItem = Obj.pntInventory[item_num[i]];
		ret = 0;
		// ----
		if (g_LuckyItemManager.IsLuckyItemEquipment(DurItem->m_Type))
		{
			ret = DurItem->LuckyItemArmorDurabilityDown(1, Obj);
			// ----
			if (ret != 0)
			{
				gGameProtocol.GCItemObjectDurSend2(Obj, (BYTE)item_num[i], (int)DurItem->m_Durability, 0);
			}
			// ----
			if (ret == 2)
			{
				reCalCharacter = 1;
			}
		}
	}

	if (Obj.m_TimeCount % 10 != 0)
	{
		return;
	}

	if (Obj.pntInventory[7]->IsItem() == 1)
	{
		ret = Obj.pntInventory[7]->DurabilityDown(1, Obj);

		if (ret != 0)
		{
			gGameProtocol.GCItemObjectDurSend2(Obj, 7, Obj.pntInventory[7]->m_Durability, 0);
		}

		if (ret == 2)
		{
			reCalCharacter = 1;
		}
	}

	if ((attr & 1) == 0)
	{
		if (Obj.pntInventory[10]->IsItem() == 1 && Obj.pntInventory[10]->m_PeriodItemOption != 1)
		{
			if ((Obj.pntInventory[10]->m_Type == ITEMGET(13, 20) && Obj.pntInventory[10]->m_Level == 0) ||
				(Obj.pntInventory[10]->m_Type == ITEMGET(13, 107) && Obj.pntInventory[10]->m_Level == 0))
			{
				ret = Obj.pntInventory[10]->DurabilityDown(70, Obj);
				if (ret != 0)
				{
					gGameProtocol.GCItemObjectDurSend2(Obj, 10, Obj.pntInventory[10]->m_Durability, 0);
				}

				if (ret == 2)
				{
					reCalCharacter = 1;
				}
			}

			else if (Obj.pntInventory[10]->m_Type == ITEMGET(13, 38))
			{
				ret = Obj.pntInventory[10]->DurabilityDown(63, Obj);
				if (ret != 0)
				{
					gGameProtocol.GCItemObjectDurSend2(Obj, 10, Obj.pntInventory[10]->m_Durability, 0);
				}
			}

			else if (Obj.pntInventory[10]->m_Type == ITEMGET(13, 169))
			{
				ret = Obj.pntInventory[10]->DurabilityDown(70, Obj);
				if (ret != 0)
				{
					gGameProtocol.GCItemObjectDurSend2(Obj, 10, Obj.pntInventory[10]->m_Durability, 0);
				}

				if (ret == 2)
				{
					reCalCharacter = 1;
				}
			}

			else if (Obj.pntInventory[10]->m_Type == ITEMGET(13, 170))
			{
				ret = Obj.pntInventory[10]->DurabilityDown(70, Obj);
				if (ret != 0)
				{
					gGameProtocol.GCItemObjectDurSend2(Obj, 10, Obj.pntInventory[10]->m_Durability, 0);
				}

				if (ret == 2)
				{
					reCalCharacter = 1;
				}
			}

			else if (Obj.pntInventory[10]->m_Type == ITEMGET(13, 268))
			{
				ret = Obj.pntInventory[10]->DurabilityDown(70, Obj);
				if (ret != 0)
				{
					gGameProtocol.GCItemObjectDurSend2(Obj, 10, Obj.pntInventory[10]->m_Durability, 0);
				}

				if (ret == 2)
				{
					reCalCharacter = 1;
				}
			}

			else if (Obj.pntInventory[10]->m_Type != ITEMGET(13, 10))
			{
				ret = Obj.pntInventory[10]->DurabilityDown(1, Obj);
				if (ret != 0)
				{
					gGameProtocol.GCItemObjectDurSend2(Obj, 10, Obj.pntInventory[10]->m_Durability, 0);
				}

				if (ret == 2)
				{
					reCalCharacter = 1;
				}
			}
		}

		if (Obj.pntInventory[11]->IsItem() == 1 && Obj.pntInventory[11]->m_PeriodItemOption != 1)
		{
			if ((Obj.pntInventory[11]->m_Type == ITEMGET(13, 20) && Obj.pntInventory[11]->m_Level == 0) ||
				(Obj.pntInventory[11]->m_Type == ITEMGET(13, 107) && Obj.pntInventory[11]->m_Level == 0))
			{
				ret = Obj.pntInventory[11]->DurabilityDown(70, Obj);
				if (ret != 0)
				{
					gGameProtocol.GCItemObjectDurSend2(Obj, 11, Obj.pntInventory[11]->m_Durability, 0);
				}

				if (ret == 2)
				{
					reCalCharacter = 1;
				}
			}
			else if (Obj.pntInventory[11]->m_Type == ITEMGET(13, 38))
			{
				ret = Obj.pntInventory[11]->DurabilityDown(63, Obj);
				if (ret != 0)
				{
					gGameProtocol.GCItemObjectDurSend2(Obj, 11, Obj.pntInventory[11]->m_Durability, 0);
				}
			}

			else if (Obj.pntInventory[11]->m_Type == ITEMGET(13, 169))
			{
				ret = Obj.pntInventory[11]->DurabilityDown(70, Obj);
				if (ret != 0)
				{
					gGameProtocol.GCItemObjectDurSend2(Obj, 11, Obj.pntInventory[11]->m_Durability, 0);
				}

				if (ret == 2)
				{
					reCalCharacter = 1;
				}
			}

			else if (Obj.pntInventory[11]->m_Type == ITEMGET(13, 170))
			{
				ret = Obj.pntInventory[11]->DurabilityDown(70, Obj);
				if (ret != 0)
				{
					gGameProtocol.GCItemObjectDurSend2(Obj, 11, Obj.pntInventory[11]->m_Durability, 0);
				}

				if (ret == 2)
				{
					reCalCharacter = 1;
				}
			}

			else if (Obj.pntInventory[11]->m_Type == ITEMGET(13, 268))
			{
				ret = Obj.pntInventory[11]->DurabilityDown(70, Obj);
				if (ret != 0)
				{
					gGameProtocol.GCItemObjectDurSend2(Obj, 11, Obj.pntInventory[11]->m_Durability, 0);
				}

				if (ret == 2)
				{
					reCalCharacter = 1;
				}
			}

			else if (Obj.pntInventory[11]->m_Type != ITEMGET(13, 10))
			{
				ret = Obj.pntInventory[11]->DurabilityDown(1, Obj);
				if (ret != 0)
				{
					gGameProtocol.GCItemObjectDurSend2(Obj, 11, Obj.pntInventory[11]->m_Durability, 0);
				}

				if (ret == 2)
				{
					reCalCharacter = 1;
				}
			}
		}

		if (Obj.pntInventory[9]->IsItem() == 1 && Obj.pntInventory[9]->m_PeriodItemOption != 1)
		{
			if (Obj.pntInventory[9]->m_Type != ITEMGET(13, 10))
			{
				ret = Obj.pntInventory[9]->DurabilityDown(1, Obj);

				if (ret != 0)
				{
					gGameProtocol.GCItemObjectDurSend2(Obj, 9, Obj.pntInventory[9]->m_Durability, 0);
				}

				if (ret == 2)
				{
					reCalCharacter = 1;
				}
			}
		}
	}

	if (reCalCharacter != 0)
	{
		gObjCalCharacter.CalcCharacter(Obj);
	}
}

void gObjChangeDurProc(CGameObject &Obj) //season 4.5 add-on
{
	if (Obj.pntInventory[10]->IsItem() == 1 && Obj.pntInventory[10]->IsPeriodItem() == FALSE)
	{
		if (Obj.pntInventory[10]->m_Type == ITEMGET(13, 10))
		{
			int m_Durability = Obj.pntInventory[10]->m_Durability;

			Obj.pntInventory[10]->m_Durability -= (float)0.02;

			if (m_Durability > (int)Obj.pntInventory[10]->m_Durability)
			{
				if (Obj.pntInventory[10]->m_Durability < 1)
				{
					Obj.pntInventory[10]->m_Durability = 0;
				}

				BYTE dur = Obj.pntInventory[10]->m_Durability;

				gGameProtocol.GCItemObjectDurSend(Obj, 10, dur, 0);

				if (dur == 0)
				{
					gObjUseSkill.SkillChangeUse(Obj);
					sLog->outBasic("[%s][%s] %s+%d %d Item broken because durability exhausted", Obj.AccountID, Obj.Name, Obj.pntInventory[10]->GetName(), Obj.pntInventory[10]->m_Level, dur);
					Obj.pntInventory[10]->Clear();
					gGameProtocol.GCInventoryItemDeleteSend(Obj, 10, 0);
				}
			}
		}
		else if (Obj.pntInventory[10]->m_Type == ITEMGET(13, 39))
		{
			int m_Durability = Obj.pntInventory[10]->m_Durability;

			Obj.pntInventory[10]->m_Durability -= (float)0.02;

			if (m_Durability > (int)Obj.pntInventory[10]->m_Durability)
			{
				if (Obj.pntInventory[10]->m_Durability < 1)
				{
					Obj.pntInventory[10]->m_Durability = 0;
				}

				BYTE dur = Obj.pntInventory[10]->m_Durability;

				gGameProtocol.GCItemObjectDurSend(Obj, 10, dur, 0);

				if (dur == 0)
				{
					gObjUseSkill.SkillChangeUse(Obj);
					sLog->outBasic("[%s][%s] %s+%d %d Item broken because durability exhausted", Obj.AccountID, Obj.Name, Obj.pntInventory[10]->GetName(), Obj.pntInventory[10]->m_Level, dur);
					Obj.pntInventory[10]->Clear();
					gGameProtocol.GCInventoryItemDeleteSend(Obj, 10, 0);
				}
			}
		}
		//Second Edition
		else if (Obj.pntInventory[10]->m_Type == ITEMGET(13, 40))
		{
			int m_Durability = Obj.pntInventory[10]->m_Durability;

			Obj.pntInventory[10]->m_Durability -= (float)0.02;

			if (m_Durability > (int)Obj.pntInventory[10]->m_Durability)
			{
				if (Obj.pntInventory[10]->m_Durability < 1)
				{
					Obj.pntInventory[10]->m_Durability = 0;
				}

				BYTE dur = Obj.pntInventory[10]->m_Durability;

				gGameProtocol.GCItemObjectDurSend(Obj, 10, dur, 0);

				if (dur == 0)
				{
					gObjUseSkill.SkillChangeUse(Obj);
					sLog->outBasic("[%s][%s] %s+%d %d Item broken because durability exhausted", Obj.AccountID, Obj.Name, Obj.pntInventory[10]->GetName(), Obj.pntInventory[10]->m_Level, dur);
					Obj.pntInventory[10]->Clear();
					gGameProtocol.GCInventoryItemDeleteSend(Obj, 10, 0);
				}
			}
		}
		//Season 2.5 add-on
		else if (Obj.pntInventory[10]->m_Type == ITEMGET(13, 41)) //Season 2.5 Santa Girl Ring
		{
			int m_Durability = Obj.pntInventory[10]->m_Durability;

			Obj.pntInventory[10]->m_Durability -= (float)0.02;

			if (m_Durability > (int)Obj.pntInventory[10]->m_Durability)
			{
				if (Obj.pntInventory[10]->m_Durability < 1)
				{
					Obj.pntInventory[10]->m_Durability = 0;
				}

				BYTE dur = Obj.pntInventory[10]->m_Durability;

				gGameProtocol.GCItemObjectDurSend(Obj, 10, dur, 0);

				if (dur == 0)
				{
					gObjUseSkill.SkillChangeUse(Obj);
					sLog->outBasic("[%s][%s] %s+%d %d Item broken because durability exhausted", Obj.AccountID, Obj.Name, Obj.pntInventory[10]->GetName(), Obj.pntInventory[10]->m_Level, dur);
					Obj.pntInventory[10]->Clear();
					gGameProtocol.GCInventoryItemDeleteSend(Obj, 10, 0);
				}
			}
		}
		else if (Obj.pntInventory[10]->m_Type == ITEMGET(13, 68)) //Season 4.0 XMas Ring
		{
			int m_Durability = Obj.pntInventory[10]->m_Durability;

			Obj.pntInventory[10]->m_Durability -= (float)0.02;

			if (m_Durability > (int)Obj.pntInventory[10]->m_Durability)
			{
				if (Obj.pntInventory[10]->m_Durability < 1)
				{
					Obj.pntInventory[10]->m_Durability = 0;
				}

				BYTE dur = Obj.pntInventory[10]->m_Durability;

				gGameProtocol.GCItemObjectDurSend(Obj, 10, dur, 0);

				if (dur == 0)
				{
					gObjUseSkill.SkillChangeUse(Obj);
					sLog->outBasic("[%s][%s] %s+%d %d Item broken because durability exhausted", Obj.AccountID, Obj.Name, Obj.pntInventory[10]->GetName(), Obj.pntInventory[10]->m_Level, dur);
					Obj.pntInventory[10]->Clear();
					gGameProtocol.GCInventoryItemDeleteSend(Obj, 10, 0);
				}
			}
		}
		else if ((Obj.pntInventory[10]->m_Type == ITEMGET(13, 76) || Obj.pntInventory[10]->m_Type == ITEMGET(13, 77) || Obj.pntInventory[10]->m_Type == ITEMGET(13, 78))
			&& Obj.pntInventory[10]->m_PeriodItemOption == 0) //Season 4.6 Panda Ring
		{
			int m_Durability = Obj.pntInventory[10]->m_Durability;

			Obj.pntInventory[10]->m_Durability -= (float)0.02;

			if (m_Durability > (int)Obj.pntInventory[10]->m_Durability)
			{
				if (Obj.pntInventory[10]->m_Durability < 1)
				{
					Obj.pntInventory[10]->m_Durability = 0;
				}

				BYTE dur = Obj.pntInventory[10]->m_Durability;

				gGameProtocol.GCItemObjectDurSend(Obj, 10, dur, 0);

				if (dur == 0)
				{
					gObjUseSkill.SkillChangeUse(Obj);
					sLog->outBasic("[%s][%s] %s+%d %d Item broken because durability exhausted", Obj.AccountID, Obj.Name, Obj.pntInventory[10]->GetName(), Obj.pntInventory[10]->m_Level, dur);
					Obj.pntInventory[10]->Clear();
					gGameProtocol.GCInventoryItemDeleteSend(Obj, 10, 0);
				}
			}
		}
		else if (Obj.pntInventory[10]->m_Type == ITEMGET(13, 122) && Obj.pntInventory[10]->m_PeriodItemOption == 0) //Season 5.0 Skeleton Ring
		{
			int m_Durability = Obj.pntInventory[10]->m_Durability;

			Obj.pntInventory[10]->m_Durability -= (float)0.02;

			if (m_Durability > (int)Obj.pntInventory[10]->m_Durability)
			{
				if (Obj.pntInventory[10]->m_Durability < 1)
				{
					Obj.pntInventory[10]->m_Durability = 0;
				}

				BYTE dur = Obj.pntInventory[10]->m_Durability;

				gGameProtocol.GCItemObjectDurSend(Obj, 10, dur, 0);

				if (dur == 0)
				{
					gObjUseSkill.SkillChangeUse(Obj);
					sLog->outBasic("[%s][%s] %s+%d %d Item broken because durability exhausted", Obj.AccountID, Obj.Name, Obj.pntInventory[10]->GetName(), Obj.pntInventory[10]->m_Level, dur);
					Obj.pntInventory[10]->Clear();
					gGameProtocol.GCInventoryItemDeleteSend(Obj, 10, 0);
				}
			}
		}
	}

	if (Obj.pntInventory[11]->IsItem() == 1 && Obj.pntInventory[11]->IsPeriodItem() == FALSE)
	{
		if (Obj.pntInventory[11]->m_Type == ITEMGET(13, 10))
		{
			int m_Durability = Obj.pntInventory[11]->m_Durability;

			Obj.pntInventory[11]->m_Durability -= (float)0.02;

			if (m_Durability > (int)Obj.pntInventory[11]->m_Durability)
			{
				if (Obj.pntInventory[11]->m_Durability < 1)
				{
					Obj.pntInventory[11]->m_Durability = 0;
				}

				BYTE dur = Obj.pntInventory[11]->m_Durability;

				gGameProtocol.GCItemObjectDurSend(Obj, 11, dur, 0);

				if (dur == 0)
				{
					gObjUseSkill.SkillChangeUse(Obj);
					sLog->outBasic("[%s][%s] %s+%d %d Item broken because durability exhausted", Obj.AccountID, Obj.Name, Obj.pntInventory[10]->GetName(), Obj.pntInventory[11]->m_Level, dur); //wz bug
					Obj.pntInventory[11]->Clear();
					gGameProtocol.GCInventoryItemDeleteSend(Obj, 11, 0);
				}
			}
		}
		else if (Obj.pntInventory[11]->m_Type == ITEMGET(13, 39))
		{
			int m_Durability = Obj.pntInventory[11]->m_Durability;

			Obj.pntInventory[11]->m_Durability -= (float)0.02;

			if (m_Durability > (int)Obj.pntInventory[11]->m_Durability)
			{
				if (Obj.pntInventory[11]->m_Durability < 1)
				{
					Obj.pntInventory[11]->m_Durability = 0;
				}

				BYTE dur = Obj.pntInventory[11]->m_Durability;

				gGameProtocol.GCItemObjectDurSend(Obj, 11, dur, 0);

				if (dur == 0)
				{
					gObjUseSkill.SkillChangeUse(Obj);
					sLog->outBasic("[%s][%s] %s+%d %d Item broken because durability exhausted", Obj.AccountID, Obj.Name, Obj.pntInventory[11]->GetName(), Obj.pntInventory[11]->m_Level, dur);
					Obj.pntInventory[11]->Clear();
					gGameProtocol.GCInventoryItemDeleteSend(Obj, 11, 0);
				}
			}
		}
		//Second Edition
		else if (Obj.pntInventory[11]->m_Type == ITEMGET(13, 40))
		{
			int m_Durability = Obj.pntInventory[11]->m_Durability;

			Obj.pntInventory[11]->m_Durability -= (float)0.02;

			if (m_Durability > (int)Obj.pntInventory[11]->m_Durability)
			{
				if (Obj.pntInventory[11]->m_Durability < 1)
				{
					Obj.pntInventory[11]->m_Durability = 0;
				}

				BYTE dur = Obj.pntInventory[11]->m_Durability;

				gGameProtocol.GCItemObjectDurSend(Obj, 11, dur, 0);

				if (dur == 0)
				{
					gObjUseSkill.SkillChangeUse(Obj);
					sLog->outBasic("[%s][%s] %s+%d %d Item broken because durability exhausted", Obj.AccountID, Obj.Name, Obj.pntInventory[11]->GetName(), Obj.pntInventory[11]->m_Level, dur);
					Obj.pntInventory[11]->Clear();
					gGameProtocol.GCInventoryItemDeleteSend(Obj, 11, 0);
				}
			}
		}
		//Season 2.5 add-on
		else if (Obj.pntInventory[11]->m_Type == ITEMGET(13, 41)) //Season 2.5 Santa Girl Ring
		{
			int m_Durability = Obj.pntInventory[11]->m_Durability;

			Obj.pntInventory[11]->m_Durability -= (float)0.02;

			if (m_Durability > (int)Obj.pntInventory[11]->m_Durability)
			{
				if (Obj.pntInventory[11]->m_Durability < 1)
				{
					Obj.pntInventory[11]->m_Durability = 0;
				}

				BYTE dur = Obj.pntInventory[11]->m_Durability;

				gGameProtocol.GCItemObjectDurSend(Obj, 11, dur, 0);

				if (dur == 0)
				{
					gObjUseSkill.SkillChangeUse(Obj);
					sLog->outBasic("[%s][%s] %s+%d %d Item broken because durability exhausted", Obj.AccountID, Obj.Name, Obj.pntInventory[11]->GetName(), Obj.pntInventory[11]->m_Level, dur);
					Obj.pntInventory[11]->Clear();
					gGameProtocol.GCInventoryItemDeleteSend(Obj, 11, 0);
				}
			}
		}

		else if (Obj.pntInventory[11]->m_Type == ITEMGET(13, 68)) //Season 4.0 XMas Ring
		{
			int m_Durability = Obj.pntInventory[11]->m_Durability;

			Obj.pntInventory[11]->m_Durability -= (float)0.02;

			if (m_Durability > (int)Obj.pntInventory[11]->m_Durability)
			{
				if (Obj.pntInventory[11]->m_Durability < 1)
				{
					Obj.pntInventory[11]->m_Durability = 0;
				}

				BYTE dur = Obj.pntInventory[11]->m_Durability;

				gGameProtocol.GCItemObjectDurSend(Obj, 11, dur, 0);

				if (dur == 0)
				{
					gObjUseSkill.SkillChangeUse(Obj);
					sLog->outBasic("[%s][%s] %s+%d %d Item broken because durability exhausted", Obj.AccountID, Obj.Name, Obj.pntInventory[11]->GetName(), Obj.pntInventory[11]->m_Level, dur);
					Obj.pntInventory[11]->Clear();
					gGameProtocol.GCInventoryItemDeleteSend(Obj, 11, 0);
				}
			}
		}

		else if ((Obj.pntInventory[11]->m_Type == ITEMGET(13, 76) || Obj.pntInventory[11]->m_Type == ITEMGET(13, 77) || Obj.pntInventory[11]->m_Type == ITEMGET(13, 78))
			&& Obj.pntInventory[11]->m_PeriodItemOption == 0) //Season 4.6 Panda Ring
		{
			int m_Durability = Obj.pntInventory[11]->m_Durability;

			Obj.pntInventory[11]->m_Durability -= (float)0.02;

			if (m_Durability > (int)Obj.pntInventory[11]->m_Durability)
			{
				if (Obj.pntInventory[11]->m_Durability < 1)
				{
					Obj.pntInventory[11]->m_Durability = 0;
				}

				BYTE dur = Obj.pntInventory[11]->m_Durability;

				gGameProtocol.GCItemObjectDurSend(Obj, 11, dur, 0);

				if (dur == 0)
				{
					gObjUseSkill.SkillChangeUse(Obj);
					sLog->outBasic("[%s][%s] %s+%d %d Item broken because durability exhausted", Obj.AccountID, Obj.Name, Obj.pntInventory[11]->GetName(), Obj.pntInventory[11]->m_Level, dur);
					Obj.pntInventory[11]->Clear();
					gGameProtocol.GCInventoryItemDeleteSend(Obj, 11, 0);
				}
			}
		}

		else if (Obj.pntInventory[11]->m_Type == ITEMGET(13, 122) && Obj.pntInventory[11]->m_PeriodItemOption == 0) //Season 5.0 Skeleton Ring
		{
			int m_Durability = Obj.pntInventory[11]->m_Durability;

			Obj.pntInventory[11]->m_Durability -= (float)0.02;

			if (m_Durability > (int)Obj.pntInventory[11]->m_Durability)
			{
				if (Obj.pntInventory[11]->m_Durability < 1)
				{
					Obj.pntInventory[11]->m_Durability = 0;
				}

				BYTE dur = Obj.pntInventory[11]->m_Durability;

				gGameProtocol.GCItemObjectDurSend(Obj, 11, dur, 0);

				if (dur == 0)
				{
					gObjUseSkill.SkillChangeUse(Obj);
					sLog->outBasic("[%s][%s] %s+%d %d Item broken because durability exhausted", Obj.AccountID, Obj.Name, Obj.pntInventory[11]->GetName(), Obj.pntInventory[11]->m_Level, dur);
					Obj.pntInventory[11]->Clear();
					gGameProtocol.GCInventoryItemDeleteSend(Obj, 11, 0);
				}
			}
		}
	}
}

void gObjWingDurProc(CGameObject &Obj)
{
	BYTE send_dur = 0;

	// ??? ?????????? ?????? ?? ???..
	return;

	if (Obj.pntInventory[7]->IsItem() == TRUE)
	{
		int dur = (int)Obj.pntInventory[7]->m_Durability;
		Obj.pntInventory[7]->m_Durability -= (float)0.0002;

		if (dur > (int)Obj.pntInventory[7]->m_Durability)
		{
			if (Obj.pntInventory[7]->m_Durability < 1)
				Obj.pntInventory[7]->m_Durability = 0;

			send_dur = (BYTE)Obj.pntInventory[7]->m_Durability;
			gGameProtocol.GCItemObjectDurSend(Obj, 7, send_dur, 0);
			if (send_dur == 0)
			{
				sLog->outBasic("[%s][%s] %s+%d %d Item broken because durability exhausted", Obj.AccountID, Obj.Name, Obj.pntInventory[7]->GetName(), Obj.pntInventory[7]->m_Level, send_dur);
				Obj.pntInventory[7]->Clear();
				gGameProtocol.GCInventoryItemDeleteSend(Obj, 7, 0);
			}
		}
		//sLog->outBasic("???? ?????? %f",Obj.pntInventory[7]->m_Durability);
		return;
	}
}


void gObjPenaltyDurDown(CGameObject &Obj, CGameObject &TargetObj)	// Functio n Not USed
{
	int ret = 0;
	int reCalCharacter = 0;

	int irand = rand() % 2;

	CItemObject * Weapon = Obj.pntInventory[irand];
	int decdur = 1;

	switch (Obj.Class)
	{
	case 0:
		decdur = Obj.Level - (TargetObj.Level * 38) / 20;
		break;
	case 1:
		decdur = Obj.Level - (TargetObj.Level * 37) / 20;
		break;
	case 2:
		decdur = Obj.Level - (TargetObj.Level * 36) / 20;
		break;
	case 3:
	case 4:
		decdur = Obj.Level - (TargetObj.Level * 37) / 20;
		break;
	default: break;
	}

	decdur /= 10;

	if (Weapon->m_Type != ITEMGET(4, 7) && Weapon->m_Type != ITEMGET(4, 15))
	{
		if (Weapon->m_Type < ITEMGET(6, 0) && Weapon->m_Type >= ITEMGET(7, 0))
		{
			if (Weapon->IsItem())
			{
				ret = Weapon->DurabilityDown2(decdur, Obj);
			}
		}
		else
		{
			CItemObject * Weapon = Obj.pntInventory[irand];

			if (Weapon->IsItem())
			{
				ret = Weapon->DurabilityDown2(decdur, Obj);
			}
		}
	}

	if (ret != 0)
	{
		gGameProtocol.GCItemObjectDurSend2(Obj, irand, Obj.pntInventory[irand]->m_Durability, 0);
	}

	if (ret == 2)
	{
		reCalCharacter = 1;
	}

	int item_num[7] = { 0,1,2,3,4,5,6 };
	int item_pos = rand() % 7;

	CItemObject * DurItem = Obj.pntInventory[item_num[item_pos]];

	if (DurItem->m_Type != ITEMGET(4, 7) && DurItem->m_Type != ITEMGET(4, 15))
	{
		if (item_pos < 2)
		{
			if (DurItem->m_Type >= ITEMGET(6, 0) && DurItem->m_Type < ITEMGET(7, 0))
			{
				ret = DurItem->DurabilityDown2(decdur, Obj);

				if (ret != 0)
				{
					gGameProtocol.GCItemObjectDurSend2(Obj, item_num[item_pos], DurItem->m_Durability, 0);

					if (ret == 2)
					{
						reCalCharacter = 1;
					}
				}
			}
		}
		else
		{
			ret = DurItem->DurabilityDown2(decdur, Obj);

			if (ret != 0)
			{
				gGameProtocol.GCItemObjectDurSend2(Obj, item_num[item_pos], DurItem->m_Durability, 0);

				if (ret == 2)
				{
					reCalCharacter = 1;
				}
			}
		}
	}

	CItemObject * Wing = Obj.pntInventory[7];

	if (Wing->IsItem())
	{
		ret = DurItem->DurabilityDown2(decdur, Obj);

		if (ret != 0)
		{
			gGameProtocol.GCItemObjectDurSend2(Obj, item_num[item_pos], DurItem->m_Durability, 0);
			if (ret == 2)
			{
				reCalCharacter = 1;
			}
		}
	}

	if (reCalCharacter != 0)
	{
		gObjCalCharacter.CalcCharacter(Obj);
	}
}




void gObjWeaponDurDown(CGameObject &Obj, CGameObject &TargetObj, int type)
{
	if (Obj.Type == OBJ_MONSTER || Obj.Type == OBJ_NPC)
	{
		return;
	}

	if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_CHARM_PROTECTITEM) != 0)
	{
		return;
	}

	if (g_MineSystem.IsEquipPickax(Obj))
	{
		return;
	}

	int itargetdefence = 0;
	CItemObject * Right = Obj.pntInventory[0];
	CItemObject * Left = Obj.pntInventory[1];
	int ret = 0;
	int iOption1, idefense = 0;
	if (type == 0)
	{
		itargetdefence = TargetObj.m_Defense;

		if ((Obj.Class == 1 || Obj.Class == 3 || Obj.Class == 4)
			&& (Right->m_Type >= 0 && Right->m_Type < ITEMGET(4, 0)
				&& Left->m_Type >= 0 && Left->m_Type < ITEMGET(4, 0)))
		{
			if (rand() % 2 == 0)
			{
				ret = Left->NormalWeaponDurabilityDown(itargetdefence, Obj);

				if (ret != 0)
				{
					gGameProtocol.GCItemObjectDurSend(Obj, 1, Left->m_Durability, 0);
					if (ret == 2)
					{
						gObjCalCharacter.CalcCharacter(Obj);
					}
				}
			}
			else
			{
				ret = Right->NormalWeaponDurabilityDown(itargetdefence, Obj);
				if (ret != 0)
				{
					gGameProtocol.GCItemObjectDurSend(Obj, 0, Right->m_Durability, 0);
					if (ret == 2)
					{
						gObjCalCharacter.CalcCharacter(Obj);
					}
				}
			}

			return;
		}
		else if (Right->m_Type >= ITEMGET(4, 0) && Right->m_Type <= ITEMGET(4, 6) || Right->m_Type >= ITEMGET(4, 8) && Right->m_Type <= ITEMGET(4, 14) || Right->m_Type >= ITEMGET(4, 16) && Right->m_Type <= ITEMGET(4, 31)) //season12 add-on
		{
			ret = Right->BowWeaponDurabilityDown(itargetdefence, Obj);

			if (ret != 0)
			{
				gGameProtocol.GCItemObjectDurSend(Obj, 0, Right->m_Durability, 0);
				if (ret == 2)
				{
					gObjCalCharacter.CalcCharacter(Obj);
				}
			}
		}
		else if (Right->m_Type >= ITEMGET(0, 0) && Right->m_Type < ITEMGET(4, 0))
		{
			ret = Right->NormalWeaponDurabilityDown(itargetdefence, Obj);

			if (ret != 0)
			{
				gGameProtocol.GCItemObjectDurSend(Obj, 0, Right->m_Durability, 0);
				if (ret == 2)
				{
					gObjCalCharacter.CalcCharacter(Obj);
				}
			}
		}
		else if (Right->m_Type >= ITEMGET(5, 0) && Right->m_Type < ITEMGET(6, 0))
		{
			ret = Right->StaffWeaponDurabilityDown(itargetdefence, Obj);

			if (ret != 0)
			{
				gGameProtocol.GCItemObjectDurSend(Obj, 0, Right->m_Durability, 0);
				if (ret == 2)
				{
					gObjCalCharacter.CalcCharacter(Obj);
				}
			}
		}
		return;
	}

	if (TargetObj.Type == OBJ_MONSTER || TargetObj.Type == OBJ_NPC)
	{
		itargetdefence = TargetObj.m_MagicDefense;
	}
	else
	{
		itargetdefence = TargetObj.m_Defense;
	}

	if (Right->m_Type >= ITEMGET(5, 0) && Right->m_Type < ITEMGET(6, 0))
	{
		ret = Right->StaffWeaponDurabilityDown(itargetdefence, Obj);
		if (ret != 0)
		{
			gGameProtocol.GCItemObjectDurSend(Obj, 0, Right->m_Durability, 0);
			if (ret == 2)
			{
				gObjCalCharacter.CalcCharacter(Obj);
			}
		}
	}
}




void gObjArmorRandomDurDown(CGameObject &Obj, CGameObject &AttackObj)
{
	int item_num[7] = { 0,1,2,3,4,5,6 };
	int item_pos = rand() % 7;

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_CHARM_PROTECTITEM) != 0)
	{
		return;
	}

	CItemObject * DurItem = Obj.pntInventory[item_num[item_pos]];
	int damagemin = AttackObj.m_AttackDamageMin;
	int ret;

	if (item_pos < 2)
	{
		if (DurItem->m_Type >= ITEMGET(6, 0) && DurItem->m_Type < ITEMGET(7, 0))
		{
			ret = DurItem->ArmorDurabilityDown(damagemin, Obj);

			if (ret != 0)
			{
				gGameProtocol.GCItemObjectDurSend(Obj, item_num[item_pos], DurItem->m_Durability, 0);
				if (ret == 2)
				{
					gObjCalCharacter.CalcCharacter(Obj);
				}
			}
			return;
		}
	}
	else
	{
		if (DurItem->m_Type != ITEMGET(4, 7) && DurItem->m_Type != ITEMGET(4, 15))
		{
			ret = DurItem->ArmorDurabilityDown(damagemin, Obj);

			if (ret != 0)
			{
				gGameProtocol.GCItemObjectDurSend(Obj, item_num[item_pos], DurItem->m_Durability, 0);
				if (ret == 2)
				{
					gObjCalCharacter.CalcCharacter(Obj);
				}
			}
		}
	}
}




bool gObjIsSelfDefense(CGameObject &Obj, int aTargetIndex)
{
	if (!ObjectMaxRange(aTargetIndex))
	{
		return false;
	}

	for (int n = 0; n < MAX_SELF_DEFENSE; n++)
	{
		if (Obj.SelfDefense[n] >= 0)
		{
			if (Obj.SelfDefense[n] == aTargetIndex)
			{
				return true;
			}
		}
	}

	return false;
}




void gObjCheckSelfDefense(CGameObject &Obj, int aTargetIndex)
{
	if (!ObjectMaxRange(aTargetIndex))
	{
		return;
	}

	if (getGameObject(aTargetIndex)->m_PK_Level > 4)
	{
		return;
	}

	for (int n = 0; n < MAX_SELF_DEFENSE; n++)
	{
		if (getGameObject(aTargetIndex)->SelfDefense[n] >= 0)
		{
			if (getGameObject(aTargetIndex)->SelfDefense[n] == Obj.m_Index)
			{
				return;
			}
		}
	}

	int blank = -1;

	for (int n = 0; n < MAX_SELF_DEFENSE; n++)
	{
		if (Obj.SelfDefense[n] >= 0)
		{
			if (Obj.SelfDefense[n] == aTargetIndex)
			{
				Obj.SelfDefenseTime[n] = GetTickCount() + 60000;
				return;
			}
		}
		else
		{
			blank = n;
		}
	}

	if (blank < 0)
	{
		sLog->outBasic("Self-defense count exceeds");
		return;
	}

	Obj.MySelfDefenseTime = GetTickCount();
	Obj.SelfDefense[blank] = aTargetIndex;
	Obj.SelfDefenseTime[blank] = GetTickCount() + 60000;

	char szTemp[64];

	wsprintf(szTemp, Lang.GetText(0, 27), Obj.Name, getGameObject(aTargetIndex)->Name);

	gGameProtocol.GCServerMsgStringSend(szTemp, Obj, 1);
	gGameProtocol.GCServerMsgStringSend(szTemp, *getGameObject(aTargetIndex), 1);

}




void gObjTimeCheckSelfDefense(CGameObject &Obj)
{
	char szTemp[64];

	for (int n = 0; n < MAX_SELF_DEFENSE; n++)
	{
		if (Obj.SelfDefense[n] >= 0)
		{
			if (GetTickCount() > Obj.SelfDefenseTime[n])
			{
				wsprintf(szTemp, Lang.GetText(0, 28), Obj.Name);
				gGameProtocol.GCServerMsgStringSend(szTemp, Obj, 1);
				gGameProtocol.GCServerMsgStringSend(szTemp, *getGameObject( Obj.SelfDefense[n] ), 1);

				Obj.SelfDefense[n] = -1;

			}
		}
	}
}


BOOL gObjAttack(CGameObject &Obj, CGameObject &TargetObj, CMagicInf * lpMagic, BOOL magicsend, BYTE MSBFlag, int AttackDamage, BOOL bCombo, BYTE RFAttack, BYTE byReflect, BYTE byPentagramAttack)
{
	return gclassObjAttack.Attack(Obj, TargetObj, lpMagic, magicsend, MSBFlag, AttackDamage, bCombo, RFAttack, byReflect, byPentagramAttack);
}


void gObjMonsterExpDivision(CGameObject &MonObj, CGameObject &Obj, int AttackDamage, int MSBFlag)
{
	gObjMonsterHitDamageUserDel(MonObj);
	MonObj.MonsterMoneyDrop = 0;

	int tObjNum;
	UINT64 exp;
	UINT64 nDropZen = 0;
	CGameObject* lpTargetObj;

	for (int n = 0; n < MAX_ST_HIT_DAMAGE; n++)
	{
		if (MonObj.sHD[n].number >= 0)
		{
			tObjNum = MonObj.sHD[n].number;
			lpTargetObj = getGameObject(tObjNum);

			if (MonObj.sHD[n].HitDamage > 0)
			{
				bool bSendExp = 1;

				exp = gObjMonsterExpSingle(Obj, MonObj, MonObj.sHD[n].HitDamage, MonObj.MaxLife, bSendExp, nDropZen);

				if (exp > 0)
				{
					if (bSendExp == 1)
					{
						if (lpTargetObj->m_Index == MonObj.m_Index)
						{
							if (g_MasterLevelSkillTreeSystem.IsMasterLevelUser(Obj))
							{
								gGameProtocol.GCKillPlayerMasterExpSend(Obj, MonObj, exp, AttackDamage, MSBFlag);
							}

							else
							{
								gGameProtocol.GCKillPlayerExpSend(Obj, MonObj, exp, AttackDamage, MSBFlag);
							}
						}
					}
				}

				if (ItemDrop.IsZenDropActive(MonObj.MapNumber) == true)
				{
					DWORD dwZenDropMoney = ItemDrop.GetZenAmount(MonObj.MapNumber, MonObj.Level);
					dwZenDropMoney = exp / 2 + dwZenDropMoney;
					//(BaseExp / 2 * 1+Zen%
					//sLog->outBasic("[K2] 1# dwZenDropMoney %d Exp %d  MonObj.Level %d", dwZenDropMoney, exp, MonObj.Level);
					if (dwZenDropMoney > 0)
					{
						MonObj.MonsterMoneyDrop = dwZenDropMoney;
					}
				}

				else
				{
					MonObj.MonsterMoneyDrop = nDropZen;

				}

				if ((Obj.pntInventory[10]->IsItem() == TRUE && Obj.pntInventory[10]->m_Type == ITEMGET(13, 76) && Obj.pntInventory[10]->IsPeriodItemExpire() == FALSE) ||
					Obj.pntInventory[11]->IsItem() == TRUE && Obj.pntInventory[11]->m_Type == ITEMGET(13, 76) && Obj.pntInventory[11]->IsPeriodItemExpire() == FALSE)
				{
					MonObj.MonsterMoneyDrop *= 1.5;
				}

				else if ((Obj.pntInventory[10]->IsItem() == TRUE && Obj.pntInventory[10]->m_Type == ITEMGET(13, 77) && Obj.pntInventory[10]->IsPeriodItemExpire() == FALSE) ||
					Obj.pntInventory[11]->IsItem() == TRUE && Obj.pntInventory[11]->m_Type == ITEMGET(13, 77) && Obj.pntInventory[11]->IsPeriodItemExpire() == FALSE)
				{
					MonObj.MonsterMoneyDrop *= 1.5;
				}

				else if ((Obj.pntInventory[10]->IsItem() == TRUE && Obj.pntInventory[10]->m_Type == ITEMGET(13, 78) && Obj.pntInventory[10]->IsPeriodItemExpire() == FALSE) ||
					Obj.pntInventory[11]->IsItem() == TRUE && Obj.pntInventory[11]->m_Type == ITEMGET(13, 78) && Obj.pntInventory[11]->IsPeriodItemExpire() == FALSE)
				{
					MonObj.MonsterMoneyDrop *= 1.5;
				}

				else if ((Obj.pntInventory[10]->IsItem() == TRUE && Obj.pntInventory[10]->m_Type == ITEMGET(13, 166) && Obj.pntInventory[10]->IsPeriodItemExpire() == FALSE) ||
					Obj.pntInventory[11]->IsItem() == TRUE && Obj.pntInventory[11]->m_Type == ITEMGET(13, 166) && Obj.pntInventory[11]->IsPeriodItemExpire() == FALSE)
				{
					MonObj.MonsterMoneyDrop *= 1.5;
				}

				else if (Obj.pntInventory[8]->IsItem() == TRUE && Obj.pntInventory[8]->m_Type == ITEMGET(13, 106) && Obj.pntInventory[8]->m_Durability > 0.0)
				{
					MonObj.MonsterMoneyDrop *= 1.5;
				}
			}
		}
	}
}

struct _stHitPartyInfo
{
	_stHitPartyInfo()
	{
		this->nPartyNumber = -1;
		this->nHitDamage = 0;
	}

	int nPartyNumber;
	int nHitDamage;
};

void gObjMonsterExpDivisionRenewal(CGameObject &MonObj, CGameObject &Obj, int AttackDamage, int MSBFlag)
{
	_stHitPartyInfo stHitParty[40];

	bool bSendExp;
	bool bFindPartySlot = false;
	int nZeroIndex = -1;
	bool bIsFindZeroIndex = false;
	bool bIsPartyDamage = false;
	UINT64 nDropZen = 0;
	for (int n = 0; n < MAX_ST_HIT_DAMAGE; n++)
	{
		if (MonObj.sHD[n].number >= 0)
		{
			int nTargetNumber = MonObj.sHD[n].number;
			CGameObject* lpTargetObj = getGameObject(nTargetNumber);

			if (getGameObject(nTargetNumber)->PartyNumber < 0)
			{
				bSendExp = true;
				UINT64 nTempDropZen = gObjMonsterExpSingleRenewal(*lpTargetObj, MonObj, MonObj.sHD[n].HitDamage, MonObj.MaxLife, bSendExp);

				if (Obj.m_Index == MonObj.TargetNumber)
				{
					nDropZen = nTempDropZen;
				}
			}

			else
			{
				for (int k = 0; k < 40; k++)
				{
					if (stHitParty[k].nPartyNumber == lpTargetObj->PartyNumber)
					{
						stHitParty[k].nHitDamage += MonObj.sHD[n].HitDamage;
						stHitParty[k].nPartyNumber = lpTargetObj->PartyNumber;
						bFindPartySlot = true;
					}

					if (stHitParty[k].nPartyNumber == -1)
					{
						if (bIsFindZeroIndex == false)
						{
							nZeroIndex = k;
							bIsFindZeroIndex = true;
						}
					}
				}

				if (bFindPartySlot == false)
				{
					if (nZeroIndex >= 0 && nZeroIndex < MAX_ST_HIT_DAMAGE)
					{
						stHitParty[nZeroIndex].nHitDamage = MonObj.sHD[n].HitDamage;
						stHitParty[nZeroIndex].nPartyNumber = lpTargetObj->PartyNumber;
					}
				}

				bIsPartyDamage = true;
			}
		}
	}

	if (bIsPartyDamage)
	{
		for (int i = 0; i < 40; i++)
		{
			if (stHitParty[i].nPartyNumber >= 0)
			{
				UINT64 nTempDropZen = gObjExpPartyRenewal(stHitParty[i].nPartyNumber, Obj, MonObj, stHitParty[i].nHitDamage, MonObj.MaxLife, 1);

				if (nTempDropZen > 0)
					nDropZen = nTempDropZen;
			}
		}
	}

	if (MonObj.Type == OBJ_MONSTER)
	{

		if (ItemDrop.IsZenDropActive(MonObj.MapNumber) == true)
		{
			DWORD dwZenDropMoney = ItemDrop.GetZenAmount(MonObj.MapNumber, MonObj.Level);

			if (dwZenDropMoney > 0)
			{
				MonObj.MonsterMoneyDrop = dwZenDropMoney;
			}
		}

		else
		{
			MonObj.MonsterMoneyDrop = nDropZen;
		}

		if ((Obj.pntInventory[10]->IsItem() == TRUE && Obj.pntInventory[10]->m_Type == ITEMGET(13, 76) && Obj.pntInventory[10]->IsPeriodItemExpire() == FALSE) ||
			(Obj.pntInventory[11]->IsItem() == TRUE && Obj.pntInventory[11]->m_Type == ITEMGET(13, 76) && Obj.pntInventory[11]->IsPeriodItemExpire() == FALSE))
		{
			MonObj.MonsterMoneyDrop *= 1.5;
		}

		if ((Obj.pntInventory[10]->IsItem() == TRUE && Obj.pntInventory[10]->m_Type == ITEMGET(13, 77) && Obj.pntInventory[10]->IsPeriodItemExpire() == FALSE) ||
			(Obj.pntInventory[11]->IsItem() == TRUE && Obj.pntInventory[11]->m_Type == ITEMGET(13, 77) && Obj.pntInventory[11]->IsPeriodItemExpire() == FALSE))
		{
			MonObj.MonsterMoneyDrop *= 1.5;
		}

		if ((Obj.pntInventory[10]->IsItem() == TRUE && Obj.pntInventory[10]->m_Type == ITEMGET(13, 78) && Obj.pntInventory[10]->IsPeriodItemExpire() == FALSE) ||
			(Obj.pntInventory[11]->IsItem() == TRUE && Obj.pntInventory[11]->m_Type == ITEMGET(13, 78) && Obj.pntInventory[11]->IsPeriodItemExpire() == FALSE))
		{
			MonObj.MonsterMoneyDrop *= 1.5;
		}
	}
}



UINT64 gObjMonsterExpSingle(CGameObject &Obj, CGameObject &TargetObj, int dmg, int tot_dmg, bool& bSendExp, UINT64& nDropZen)
{
	if (Obj.Type != OBJ_USER)
	{
		return 0;
	}

	if (g_MasterLevelSkillTreeSystem.CheckMLGetExp(Obj, TargetObj) == FALSE)
	{
		bSendExp = FALSE;
		return FALSE;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE) // exp is disabled on BattleCore
	{
		return 0;
	}

	if (DS_MAP_RANGE(Obj.MapNumber))
	{
		nDropZen = g_DevilSquare.gObjMonsterExpSingle(Obj, TargetObj, dmg, tot_dmg);
		return nDropZen;
	}

	UINT64 exp;
	UINT64 maxexp = 0;

	int level = (TargetObj.Level + 25)*TargetObj.Level / 3; //  formula 3
	int userlevel = Obj.Level;

	if (g_MasterLevelSkillTreeSystem.IsMasterLevelUser(Obj) == TRUE)
	{
		userlevel += Obj.m_PlayerData->MasterLevel;
	}

	if ((TargetObj.Level + 10) < userlevel)
	{
		level = level * (TargetObj.Level + 10) / userlevel; // formula 3
	}

	if (TargetObj.Level >= 65)
	{
		level += (TargetObj.Level - 64)*(TargetObj.Level / 4); // formula 4
	}

	if (level > 0)
	{
		maxexp = level / 2;
	}

	else
	{
		level = 0;
	}

	if (maxexp < 1)
	{
		exp = level;
	}
	else
	{
		exp = maxexp / 2 + level;
	}

	exp = dmg * exp / tot_dmg;

	if (BC_MAP_RANGE(Obj.MapNumber))
	{
		exp = exp * 50 / 100;
	}

	if (CC_MAP_RANGE(Obj.MapNumber))
	{
		exp *= g_ChaosCastle.GetExperienceBonus(Obj.m_cChaosCastleIndex);
	}

	UINT64 dwDefaultExp = exp;
	nDropZen = exp;
	float fVipBonus = g_VipSystem.GetExpBonus(Obj);
	float fMapBonus = 0.0;
	float fEventBonus = 0.0;
	float fBaseExp = 0.0;

	if (!g_MasterLevelSkillTreeSystem.IsMasterLevelUser(Obj))
	{
		fEventBonus = g_BonusEvent.GetAddExp();
		fMapBonus = g_MapAttr.GetExpBonus(Obj.MapNumber);
		fBaseExp = g_ExpManager.GetExpMultiplier(Obj.Level, Obj.m_PlayerData->m_iResets, false);
	}

	else
	{
		fEventBonus = g_BonusEvent.GetAddMLExp();
		fMapBonus = g_MapAttr.GetMasterExpBonus(Obj.MapNumber);
		fBaseExp = g_ExpManager.GetExpMultiplier(Obj.Level + Obj.m_PlayerData->MasterLevel, Obj.m_PlayerData->m_iResets, true);
	}

	exp *= (fBaseExp + fVipBonus + fEventBonus + fMapBonus);

	if (g_CrywolfSync.GetOccupationState() == 1 && g_CrywolfSync.GetApplyPenalty() == TRUE)
	{
		exp = (exp * g_CrywolfSync.GetGettingExpPenaltyRate()) / 100;
	}

	if (DG_MAP_RANGE(Obj.MapNumber))
	{
		if (g_MasterLevelSkillTreeSystem.IsMasterLevelUser(Obj))
		{
			exp *= g_DoppelGanger.GetDoppelGangerMLExpRate();
		}

		else
		{
			exp *= g_DoppelGanger.GetDoppelGangerExpRate();
		}
	}

	if (exp > 0)
	{
		CheckItemOptForGetExpExRenewal(Obj, exp, dwDefaultExp, false, TargetObj);

		if (g_MasterLevelSkillTreeSystem.IsMasterLevelUser(Obj))
		{
			Obj.m_PlayerData->MasterExperience += exp;
		}
		else
		{
			Obj.m_PlayerData->Experience += exp;
		}

		if (gObjLevelUp(Obj, exp, TargetObj.Class, "Single") == 0)
		{
			bSendExp = 0;
		}
	}

	return exp;
}

UINT64 gObjMonsterExpSingleRenewal(CGameObject &Obj, CGameObject &lpTargetObj, int dmg, int tot_dmg, bool &bSendExp)
{
	if (Obj.Type != OBJ_USER)
	{
		sLog->outError("[ERROR] Obj.Type != OBJ_USER (%s)(%d)", __FILE__, __LINE__);
		return 0;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE) // exp is disabled on BattleCore
	{
		return 0;
	}

	UINT64 nExp = 0;
	int nTotalLevel = Obj.Level + Obj.m_PlayerData->MasterLevel;
	int nMonsterLevel = lpTargetObj.Level;

	if (g_MasterLevelSkillTreeSystem.CheckMLGetExp(Obj, lpTargetObj) == FALSE)
	{
		bSendExp = false;
		return 0;
	}

	nMonsterLevel *= 4;

	if (nTotalLevel > nMonsterLevel)
	{
		nExp = ((15 * nMonsterLevel) + (15 * nTotalLevel) * 0.3 / 200.0 * (nMonsterLevel - nTotalLevel));
	}

	else
	{
		nExp = ((15 * nTotalLevel) + (15 * nTotalLevel) * 0.3 / 200.0 * (nMonsterLevel - nTotalLevel));
	}

	if (nExp <= 0)
	{
		nExp = 1;
	}

	if (dmg > tot_dmg)
	{
		dmg = tot_dmg;
	}

	nExp = dmg * nExp / tot_dmg;

	if (BC_MAP_RANGE(Obj.MapNumber))
	{
		nExp = nExp * 50 / 100;
	}

	UINT64 dwDefaultExp = nExp;
	UINT64 nDropZen = nExp;
	float fVipBonus = g_VipSystem.GetExpBonus(Obj);
	float fMapBonus = 0.0;
	float fEventBonus = 0.0;
	float fBaseExp = 0.0;

	if (!g_MasterLevelSkillTreeSystem.IsMasterLevelUser(Obj))
	{
		fEventBonus = g_BonusEvent.GetAddExp();
		fMapBonus = g_MapAttr.GetExpBonus(Obj.MapNumber);
		fBaseExp = g_ExpManager.GetExpMultiplier(Obj.Level, Obj.m_PlayerData->m_iResets, false);
	}

	else
	{
		fEventBonus = g_BonusEvent.GetAddMLExp();
		fMapBonus = g_MapAttr.GetMasterExpBonus(Obj.MapNumber);
		fBaseExp = g_ExpManager.GetExpMultiplier(Obj.Level + Obj.m_PlayerData->MasterLevel, Obj.m_PlayerData->m_iResets, true);
	}

	nExp *= (fBaseExp + fVipBonus + fEventBonus + fMapBonus);

	if (g_CrywolfSync.GetOccupationState() == 1 && g_CrywolfSync.GetApplyPenalty() == TRUE)
	{
		nExp = (nExp * g_CrywolfSync.GetGettingExpPenaltyRate()) / 100;
	}

	if (nExp > 0)
	{
		CheckItemOptForGetExpExRenewal(Obj, nExp, dwDefaultExp, false, lpTargetObj);

		if (g_MasterLevelSkillTreeSystem.IsMasterLevelUser(Obj))
		{
			Obj.m_PlayerData->MasterExperience += nExp;
		}
		else
		{
			Obj.m_PlayerData->Experience += nExp;
		}

		if (gObjLevelUp(Obj, nExp, lpTargetObj.Class, "Single") == 0)
		{
			bSendExp = 0;
		}
	}

	if (bSendExp)
	{
		gGameProtocol.GCKillPlayerMasterExpSend(Obj, lpTargetObj, nExp, dmg, bSendExp);
	}

	return nDropZen;
}

void gObjExpParty(CGameObject &Obj, CGameObject &TargetObj, int AttackDamage, int MSBFlag)
{
	if (DS_MAP_RANGE(Obj.MapNumber))
	{
		g_DevilSquare.gObjExpParty(Obj, TargetObj, AttackDamage, MSBFlag);
		return;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE) // exp is disabled on BattleCore
	{
		return;
	}

	UINT64 exp;
	UINT64 maxexp = 0;
	UINT64 totalexp;
	int level = (TargetObj.Level + 25)*TargetObj.Level / 3;
	int number;
	int partynum = Obj.PartyNumber;
	int totallevel = 0;
	int partylevel;
	int partycount;
	int dis[5];
	int viewplayer = 0;
	int viewpercent = 100;
	int bApplaySetParty = 0;
	BYTE bCheckSetParty[MAX_TYPE_PLAYER];
	int toplevel = 0;

	for (int n = 0; n < MAX_USER_IN_PARTY; n++)
	{
		number = gParty.m_PartyS[partynum].Number[n];

		if (number >= 0)
		{
			CGameObject* lpPartyObj = getGameObject(number);

			int dis = gObjCalDistance(TargetObj, *lpPartyObj); //Season 2.5 add-on

			if (dis < 10) //Season 2.5 add-on
			{
				if (lpPartyObj->Level > toplevel)
				{
					toplevel = lpPartyObj->Level;
				}
			}

			if (dis < 10) //Season 3.0 add-on
			{
				if (lpPartyObj->Level + lpPartyObj->m_PlayerData->MasterLevel > toplevel)
				{
					toplevel = lpPartyObj->Level + lpPartyObj->m_PlayerData->MasterLevel;
				}
			}
		}
	}

	if (!ObjectMaxRange(partynum))
	{
		sLog->outBasic("error : %s %d", __FILE__, __LINE__);
		return;
	}

	memset(bCheckSetParty, 0, sizeof(bCheckSetParty));

	partycount = gParty.m_PartyS[partynum].Count;

	for (int n = 0; n < MAX_USER_IN_PARTY; n++)
	{
		number = gParty.m_PartyS[partynum].Number[n];

		if (number >= 0)
		{
			CGameObject* lpPartyObj = getGameObject(number);

			if (TargetObj.MapNumber == lpPartyObj->MapNumber)
			{
				dis[n] = gObjCalDistance(TargetObj, *getGameObject(number));

				if (dis[n] < 10)
				{
					lpPartyObj = getGameObject(number);

					if (toplevel >= (lpPartyObj->Level + lpPartyObj->m_PlayerData->MasterLevel + 200))
					{
						totallevel += lpPartyObj->Level + lpPartyObj->m_PlayerData->MasterLevel + 200;
					}
					else
					{
						totallevel += lpPartyObj->Level + lpPartyObj->m_PlayerData->MasterLevel;
					}
					viewplayer += 1;
					bCheckSetParty[lpPartyObj->Class] = 1;
				}
			}
		}
	}

	viewpercent += gParty.GetExpBonus(Obj, TargetObj, partynum);

	if (viewplayer > 1)
	{
		partylevel = totallevel / viewplayer;
	}
	else
	{
		partylevel = totallevel;
	}

	if ((TargetObj.Level + 10) < partylevel)
	{
		level = level * (TargetObj.Level + 10) / partylevel;
	}

	if (TargetObj.Level >= 65)
	{
		level = level + (TargetObj.Level - 64)*(TargetObj.Level / 4);
	}

	if (level > 0)
	{
		maxexp = level / 2;
	}
	else
	{
		level = 0;
	}

	if (maxexp < 1)
	{
		totalexp = level;
	}
	else
	{
		totalexp = maxexp / 2 + level;
	}

	if (TargetObj.Type == OBJ_MONSTER)
	{
		//exp = (totalexp * viewpercent * (Obj.Level + Obj.m_PlayerData->MasterLevel)) / totallevel / 100;
		//if (ItemDrop.IsZenDropActive(lpTargetObj.MapNumber) == true)
		//{
		//	UINT64 dwZenDropMoney = ItemDrop.GetZenAmount(lpTargetObj.MapNumber, lpTargetObj.Level);
		//dwZenDropMoney = exp / 2 + dwZenDropMoney * 2;
		//sLog->outBasic("[K2] 2# dwZenDropMoney %d Exp %d", dwZenDropMoney, exp);
		//if (dwZenDropMoney > 0)
		//	{
		//		lpTargetObj.MonsterMoneyDrop = dwZenDropMoney;
		//	}
		//}
		if (ItemDrop.IsZenDropActive(TargetObj.MapNumber) == true)
		{
			DWORD dwZenDropMoney = ItemDrop.GetZenAmount(TargetObj.MapNumber, TargetObj.Level);

			if (dwZenDropMoney > 0)
			{
				TargetObj.MonsterMoneyDrop = dwZenDropMoney;
			}
		}
		else
		{
			TargetObj.MonsterMoneyDrop = totalexp;
		}

		if ((Obj.pntInventory[10]->IsItem() == TRUE && Obj.pntInventory[10]->m_Type == ITEMGET(13, 76) && Obj.pntInventory[10]->IsPeriodItemExpire() == FALSE) ||
			Obj.pntInventory[11]->IsItem() == TRUE && Obj.pntInventory[11]->m_Type == ITEMGET(13, 76) && Obj.pntInventory[11]->IsPeriodItemExpire() == FALSE)
		{
			TargetObj.MonsterMoneyDrop *= 1.5;
		}

		else if ((Obj.pntInventory[10]->IsItem() == TRUE && Obj.pntInventory[10]->m_Type == ITEMGET(13, 77) && Obj.pntInventory[10]->IsPeriodItemExpire() == FALSE) ||
			Obj.pntInventory[11]->IsItem() == TRUE && Obj.pntInventory[11]->m_Type == ITEMGET(13, 77) && Obj.pntInventory[11]->IsPeriodItemExpire() == FALSE)
		{
			TargetObj.MonsterMoneyDrop *= 1.5;
		}

		else if ((Obj.pntInventory[10]->IsItem() == TRUE && Obj.pntInventory[10]->m_Type == ITEMGET(13, 78) && Obj.pntInventory[10]->IsPeriodItemExpire() == FALSE) ||
			Obj.pntInventory[11]->IsItem() == TRUE && Obj.pntInventory[11]->m_Type == ITEMGET(13, 78) && Obj.pntInventory[11]->IsPeriodItemExpire() == FALSE)
		{
			TargetObj.MonsterMoneyDrop *= 1.5;
		}

		else if ((Obj.pntInventory[10]->IsItem() == TRUE && Obj.pntInventory[10]->m_Type == ITEMGET(13, 166) && Obj.pntInventory[10]->IsPeriodItemExpire() == FALSE) ||
			Obj.pntInventory[11]->IsItem() == TRUE && Obj.pntInventory[11]->m_Type == ITEMGET(13, 166) && Obj.pntInventory[11]->IsPeriodItemExpire() == FALSE)
		{
			TargetObj.MonsterMoneyDrop *= 1.5;
		}
	}

	for (int n = 0; n < MAX_USER_IN_PARTY; n++)
	{
		number = gParty.m_PartyS[partynum].Number[n];

		if (number >= 0)
		{
			CGameObject* lpPartyObj = getGameObject(number);

			if (TargetObj.MapNumber == lpPartyObj->MapNumber && dis[n] < 10)
			{
				UINT64 mymaxexp;

				if (!g_MasterLevelSkillTreeSystem.IsMasterLevelUser(*lpPartyObj))
				{
					mymaxexp = gLevelExperience[lpPartyObj->Level];
				}

				else
				{
					mymaxexp = lpPartyObj->m_PlayerData->MasterNextExp;
				}

				exp = (totalexp * viewpercent * (lpPartyObj->Level + lpPartyObj->m_PlayerData->MasterLevel)) / totallevel / 100;

				if (g_MasterLevelSkillTreeSystem.CheckMLGetExp(*lpPartyObj, TargetObj) == FALSE)
				{
					exp = 0;
				}

				if (BC_MAP_RANGE(lpPartyObj->MapNumber))
				{
					exp = exp * 50 / 100;
				}

				if (exp > mymaxexp)
				{
					exp = mymaxexp;
				}

				if (lpPartyObj->Type == OBJ_USER && TargetObj.Type == OBJ_USER)
				{
					exp = 0;
				}

				UINT64 dwDefaultExp = exp;
				float fVipBonus = g_VipSystem.GetExpBonus(*lpPartyObj);
				float fMapBonus = 0.0;
				float fEventBonus = 0.0;
				float fBaseExp = 0.0;

				if (!g_MasterLevelSkillTreeSystem.IsMasterLevelUser(*lpPartyObj))
				{
					fEventBonus = g_BonusEvent.GetAddExp();
					fMapBonus = g_MapAttr.GetExpBonus(lpPartyObj->MapNumber);
					fBaseExp = g_ExpManager.GetExpMultiplier(lpPartyObj->Level, lpPartyObj->m_PlayerData->m_iResets, false);
				}

				else
				{
					fEventBonus = g_BonusEvent.GetAddMLExp();
					fMapBonus = g_MapAttr.GetMasterExpBonus(lpPartyObj->MapNumber);
					fBaseExp = g_ExpManager.GetExpMultiplier(lpPartyObj->Level + lpPartyObj->m_PlayerData->MasterLevel, lpPartyObj->m_PlayerData->m_iResets, true);
				}

				exp *= (fBaseExp + fVipBonus + fEventBonus + fMapBonus);

				if (g_CrywolfSync.GetOccupationState() == 1 && g_CrywolfSync.GetApplyPenalty() == TRUE)
				{
					exp = (exp * g_CrywolfSync.GetGettingExpPenaltyRate()) / 100;
				}

				if (DG_MAP_RANGE(lpPartyObj->MapNumber))
				{
					if (g_MasterLevelSkillTreeSystem.IsMasterLevelUser(*lpPartyObj))
					{
						exp *= g_DoppelGanger.GetDoppelGangerMLExpRate();
					}

					else
					{
						exp *= g_DoppelGanger.GetDoppelGangerExpRate();
					}
				}

				if (exp > 0)
				{
					CheckItemOptForGetExpExRenewal(*lpPartyObj, exp, dwDefaultExp, false, TargetObj);

					if (g_MasterLevelSkillTreeSystem.IsMasterLevelUser(*lpPartyObj))
					{
						lpPartyObj->m_PlayerData->MasterExperience += exp;
					}

					else
					{
						lpPartyObj->m_PlayerData->Experience += exp;
					}

					if (gObjLevelUp(*lpPartyObj, exp, TargetObj.Class, "Party") == 0)
					{
						continue;
					}

					gGameProtocol.GCKillPlayerMasterExpSend(*lpPartyObj, TargetObj, exp, AttackDamage, MSBFlag);
				}
			}
		}
	}
}

UINT64 gObjExpPartyRenewal(int nPartyNumber, CGameObject &Obj, CGameObject &TargetObj, int AttackDamage, int nTotalDamage, int MSBFlag)
{
	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE) // exp is disabled on BattleCore
	{
		return 0;
	}

	int nMonsterLevel = TargetObj.Level;
	bool bApplaySetParty = 0;
	int nNumber = 0;
	int nTotalLevel = 0;
	int nTopLevel = 0;
	int nPartyCount = 0;
	int nViewPlayer = 0;
	UINT64 nDropZen = 0;
	UINT64 nTempExp = 0;
	BYTE bCheckSetParty[MAX_TYPE_PLAYER];
	int dis[5];

	if (&TargetObj == NULL)
	{
		return 0;
	}

	if (!ObjectMaxRange(nPartyNumber))
	{
		return 0;
	}

	memset(bCheckSetParty, 0, sizeof(bCheckSetParty));

	for (int n = 0; n < MAX_USER_IN_PARTY; n++)
	{
		nNumber = gParty.m_PartyS[nPartyNumber].Number[n];

		if (nNumber >= 0)
		{
			if (getGameObject(nNumber)->Connected == PLAYER_PLAYING)
			{
				if (TargetObj.MapNumber == getGameObject(nNumber)->MapNumber)
				{
					dis[n] = gObjCalDistance(TargetObj, *getGameObject(nNumber));

					if (dis[n] < 10)
					{
						CGameObject* lpPartyObj = Obj.GetParty();

						nViewPlayer++;
						bCheckSetParty[getGameObject(nNumber)->Class] = 1;
						nTotalLevel += lpPartyObj->m_PlayerData->MasterLevel + lpPartyObj->Level;

						if (lpPartyObj->m_PlayerData->MasterLevel + lpPartyObj->Level > nTopLevel)
						{
							nTopLevel = lpPartyObj->m_PlayerData->MasterLevel + lpPartyObj->Level;
						}
					}
				}
			}
		}
	}

	nMonsterLevel *= 4;

	if (nTopLevel > nMonsterLevel)
	{
		nTempExp = ((15 * nMonsterLevel) + (15 * nTopLevel) * 0.3 / 200.0 * (nMonsterLevel - nTopLevel));
	}

	else
	{
		nTempExp = ((15 * nTopLevel) + (15 * nTopLevel) * 0.3 / 200.0 * (nMonsterLevel - nTopLevel));
	}

	if (nTempExp <= 0)
	{
		nTempExp = 1;
	}

	if (AttackDamage > nTotalDamage)
	{
		AttackDamage = nTotalDamage;
	}

	nTempExp = AttackDamage * nTempExp / nTotalDamage;

	nPartyCount = gParty.m_PartyS[nPartyNumber].Count;
	float fPartyCntAddExp;

	if ((bCheckSetParty[0] != 0 && bCheckSetParty[1] != 0 && bCheckSetParty[2] != 0) ||
		(bCheckSetParty[3] != 0 && bCheckSetParty[4] != 0 && bCheckSetParty[5] != 0) ||
		(bCheckSetParty[4] != 0 && bCheckSetParty[5] != 0 && bCheckSetParty[6] != 0) ||
		(bCheckSetParty[0] != 0 && bCheckSetParty[2] != 0 && bCheckSetParty[7] != 0))
	{
		bApplaySetParty = true;
	}

	if (bApplaySetParty)
	{
		switch (nViewPlayer)
		{
		case 3:
			fPartyCntAddExp = 1.05;
			break;
		case 4:
			fPartyCntAddExp = 1.06;
			break;
		case 5:
			fPartyCntAddExp = 1.07;
			break;
		default:
			fPartyCntAddExp = 1.00;
			break;
		}
	}

	else
	{
		switch (nViewPlayer)
		{
		case 2:
			fPartyCntAddExp = 1.02;
			break;
		case 3:
			fPartyCntAddExp = 1.03;
			break;
		case 4:
			fPartyCntAddExp = 1.04;
			break;
		case 5:
			fPartyCntAddExp = 1.05;
			break;
		default:
			fPartyCntAddExp = 1.00;
			break;
		}
	}

	for (int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		nNumber = gParty.m_PartyS[nPartyNumber].Number[i];

		if (nNumber >= 0)
		{
			if (getGameObject(nNumber)->Connected == PLAYER_PLAYING)
			{
				CGameObject* lpPartyObj = getGameObject(nNumber);
				int nMemberLevel = lpPartyObj->m_PlayerData->MasterLevel + lpPartyObj->Level;

				if (TargetObj.MapNumber == lpPartyObj->MapNumber)
				{
					if (dis[i] < 10)
					{
						UINT64 nExp;

						if (g_MasterLevelSkillTreeSystem.CheckMLGetExp(*lpPartyObj, TargetObj) == FALSE)
						{
							nExp = 0;
						}

						else
						{
							UINT64 i64MyMaxExp;

							if (!g_MasterLevelSkillTreeSystem.IsMasterLevelUser(*lpPartyObj))
							{
								i64MyMaxExp = gLevelExperience[lpPartyObj->Level];
							}

							else
							{
								i64MyMaxExp = lpPartyObj->m_PlayerData->MasterNextExp;
							}

							nExp = nTempExp * fPartyCntAddExp * nMemberLevel / nTotalLevel;

							if (BC_MAP_RANGE(lpPartyObj->MapNumber))
							{
								nExp = nExp * 50 / 100;
							}

							if (lpPartyObj->Type == OBJ_USER && TargetObj.Type == OBJ_USER)
							{
								nExp = 0;
							}

							UINT64 dwDefaultExp = nExp;
							float fVipBonus = g_VipSystem.GetExpBonus(*lpPartyObj);
							float fMapBonus = 0.0;
							float fEventBonus = 0.0;
							float fBaseExp = 0.0;

							// TODO
							if (Obj.m_Index == lpPartyObj->m_Index)
							{
								nDropZen = nExp;
							}

							if (!g_MasterLevelSkillTreeSystem.IsMasterLevelUser(*lpPartyObj))
							{
								fEventBonus = g_BonusEvent.GetAddExp();
								fMapBonus = g_MapAttr.GetExpBonus(lpPartyObj->MapNumber);
								fBaseExp = g_ExpManager.GetExpMultiplier(lpPartyObj->Level, lpPartyObj->m_PlayerData->m_iResets, false);
							}

							else
							{
								fEventBonus = g_BonusEvent.GetAddMLExp();
								fMapBonus = g_MapAttr.GetMasterExpBonus(lpPartyObj->MapNumber);
								fBaseExp = g_ExpManager.GetExpMultiplier(lpPartyObj->Level + lpPartyObj->m_PlayerData->MasterLevel, lpPartyObj->m_PlayerData->m_iResets, true);
							}

							nExp *= (fBaseExp + fVipBonus + fMapBonus + fEventBonus);

							if (g_CrywolfSync.GetOccupationState() == 1 && g_CrywolfSync.GetApplyPenalty() == TRUE)
							{
								nExp = (nExp * g_CrywolfSync.GetGettingExpPenaltyRate()) / 100;
							}

							if (DG_MAP_RANGE(lpPartyObj->MapNumber))
							{
								if (g_MasterLevelSkillTreeSystem.IsMasterLevelUser(*lpPartyObj))
								{
									nExp *= g_DoppelGanger.GetDoppelGangerMLExpRate();
								}

								else
								{
									nExp *= g_DoppelGanger.GetDoppelGangerExpRate();
								}
							}

							if (nExp > 0)
							{
								CheckItemOptForGetExpExRenewal(*lpPartyObj, nExp, dwDefaultExp, false, TargetObj);

								if (g_MasterLevelSkillTreeSystem.IsMasterLevelUser(*lpPartyObj))
								{
									lpPartyObj->m_PlayerData->MasterExperience += nExp;
								}

								else
								{
									lpPartyObj->m_PlayerData->Experience += nExp;
								}

								if (gObjLevelUp(*lpPartyObj, nExp, TargetObj.Class, "Party") == 0)
								{
									continue;
								}
							}

							gGameProtocol.GCKillPlayerMasterExpSend(*lpPartyObj, TargetObj, nExp, AttackDamage, MSBFlag);
						}
					}
				}
			}
		}
	}

	return nDropZen;
}

void gObjMonsterDieLifePlus(CGameObject &Obj, CGameObject &ObjTarget)
{
	if (ObjTarget.Level < 0 || ObjTarget.Level > 255)
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	Obj.Life += ObjTarget.Level;

	COMMON_EXT_OPTION_TYPE * lpLife = g_ItemOptionTypeMng.GetCommonExcOption(4);
	COMMON_EXT_OPTION_TYPE * lpMana = g_ItemOptionTypeMng.GetCommonExcOption(5);

	if (Obj.MonsterDieGetLife != 0 && lpLife != NULL)
	{
		int iEffectValue = g_ItemOptionTypeMng._CalcEffectValue(Obj, 1, lpLife->Operator, Obj.MonsterDieGetLife * lpLife->OptionValue, lpLife->FormulaID, NULL);
		Obj.Life += iEffectValue;
	}

	if (Obj.MonsterDieGetMana != 0 && lpMana != NULL)
	{
		int iEffectValue = g_ItemOptionTypeMng._CalcEffectValue(Obj, 1, lpMana->Operator, Obj.MonsterDieGetLife * lpMana->OptionValue, lpMana->FormulaID, NULL);
		Obj.Mana += Obj.MonsterDieGetMana;
	}

	if (Obj.m_PlayerData->m_wSocketOptionMonsterDieGetHp > 0) //season4 add-on
	{
		Obj.Life += Obj.m_PlayerData->m_wSocketOptionMonsterDieGetHp;
	}

	if (Obj.m_PlayerData->m_wSocketOptionMonsterDieGetMana > 0) //season4 add-on
	{
		Obj.Mana += Obj.m_PlayerData->m_wSocketOptionMonsterDieGetMana;
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsMonsterDieGetLife > 0.0)
	{
		Obj.Life = Obj.Life + ((Obj.MaxLife + Obj.AddLife) / Obj.m_PlayerData->m_MPSkillOpt->iMpsMonsterDieGetLife);
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsMonsterDieGetMana > 0.0)
	{
		Obj.Mana = Obj.Mana + ((Obj.MaxMana + Obj.AddMana) / Obj.m_PlayerData->m_MPSkillOpt->iMpsMonsterDieGetMana);
	}

	if (Obj.m_PlayerData->m_MPSkillOpt->iMpsMonsterDieGetSD > 0.0)
	{
		Obj.iShield = Obj.iShield + ((Obj.iMaxShield + Obj.iAddShield) / Obj.m_PlayerData->m_MPSkillOpt->iMpsMonsterDieGetSD);
	}

	if (Obj.Life > (Obj.MaxLife + Obj.AddLife))
	{
		Obj.Life = Obj.MaxLife + Obj.AddLife;
	}

	if (Obj.iShield > (Obj.iMaxShield + Obj.iAddShield))
	{
		Obj.iShield = Obj.iMaxShield + Obj.iAddShield;
	}

	gGameProtocol.GCReFillSend(Obj, Obj.Life, -1, 0, Obj.iShield);

	if (Obj.MonsterDieGetMana != 0 || Obj.m_PlayerData->m_wSocketOptionMonsterDieGetMana != 0 || Obj.m_PlayerData->m_MPSkillOpt->iMpsMonsterDieGetMana != 0.0)
	{
		if (Obj.Mana > (Obj.MaxMana + Obj.AddMana))
		{
			Obj.Mana = Obj.MaxMana + Obj.AddMana;
		}

		gGameProtocol.GCManaSend(Obj, Obj.Mana, 255, 0, Obj.BP);
	}
}

void gObjLifeCheck(CGameObject &TargetObj, CGameObject& Obj, int AttackDamage, int DamageSendType, int MSBFlag, int MSBDamage, WORD Skill, int iShieldDamage, int iElementalDamage)
{
	if (Obj.Connected < PLAYER_PLAYING)
	{
		return;
	}

	if (ATTRIBUTE_RANGE(Obj.m_Attribute) || ATTRIBUTE_RANGE(TargetObj.m_Attribute))
	{
		return;
	}

	if (Obj.Class == 689) // Uruk Mountain Traps
	{
		return;
	}

	CGameObject* lpCallObj = &Obj;

	if (Obj.Type == OBJ_MONSTER && Obj.m_RecallMon >= 0)
	{
		lpCallObj = getGameObject(Obj.m_RecallMon);
	}

	if (AttackDamage != 0 && TargetObj.Type == OBJ_MONSTER)
	{
		if (TargetObj.m_RecallMon >= 0)
		{
			gGameProtocol.GCRecallMonLife(*lpCallObj, TargetObj.MaxLife, TargetObj.Life);
		}

		gObjMonsterSetHitDamage(TargetObj, lpCallObj->m_Index, AttackDamage);
	}

	if (TargetObj.Life <= 0 && TargetObj.Live != 0)
	{
		if (DamageSendType == 0)
		{
			gGameProtocol.GCDamageSend(*lpCallObj, TargetObj, AttackDamage, MSBFlag, MSBDamage, iShieldDamage);
		}
		else if (DamageSendType == 1)
		{
			gGameProtocol.GCDamageSendPoison(TargetObj, AttackDamage, iShieldDamage);
		}
		else if (DamageSendType == 2)
		{
			gGameProtocol.GCDamageSend(*lpCallObj, TargetObj, AttackDamage, MSBFlag, 5, iShieldDamage);
		}

		if (iElementalDamage != 0)
		{
			gGameProtocol.GCElementalDamageSend(*lpCallObj, TargetObj.m_Index, iElementalDamage, lpCallObj->m_iPentagramMainAttribute);
		}
		if (lpCallObj->Type == OBJ_MONSTER)
		{
			gObjAddMsgSend(*lpCallObj, 3, TargetObj, 0);
			CreateFrustrum(lpCallObj->X, lpCallObj->Y, *lpCallObj);
			TargetObj.KillerType = 1;
		}

		if (lpCallObj->Type == OBJ_USER && lpCallObj->m_PlayerData->ISBOT == false && Obj.m_Index >= g_ConfigRead.server.GetObjectStartUserIndex())
		{
			gDarkSpirit[lpCallObj->m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].ReSetTarget(TargetObj);
			g_CMuunSystem.ReSetTarget(Obj, TargetObj);
		}

		if (lpCallObj->Type == OBJ_USER && TargetObj.Type == OBJ_USER)
		{
			gObjSetKillCount(*lpCallObj, 1);
		}

		if (IT_MAP_RANGE(lpCallObj->MapNumber))
		{
			if (g_IT_Event.GetIllusionTempleState(lpCallObj->MapNumber) == 2)
			{
				g_IT_Event.IncreaseKillCount(lpCallObj->m_Index, lpCallObj->MapNumber, 1);

				sLog->outBasic("[ ITR ] [IncreaseKillCount] Map:%d, (%s)(%s) Kill User TargetInfo (%s)(%s)",
					lpCallObj->MapNumber - 44, lpCallObj->AccountID, lpCallObj->Name, TargetObj.AccountID, TargetObj.Name);
			}
		}

		if (TargetObj.Type == OBJ_MONSTER)
		{
			if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
			{
				if (TargetObj.Class == 275 || TargetObj.Class == 295 || TargetObj.MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE
					|| TargetObj.MapNumber == MAP_INDEX_HATCHERY || TargetObj.Class == 561 || TargetObj.Class == 43
					|| TargetObj.Class == 44 || TargetObj.Class == 53 || TargetObj.Class == 54 || TargetObj.Class == 55
					|| TargetObj.Class == 56 || TargetObj.Class == 78 || TargetObj.Class == 79 || TargetObj.Class == 80
					|| TargetObj.Class == 81 || TargetObj.Class == 82 || TargetObj.Class == 83 || TargetObj.Class == 493
					|| TargetObj.Class == 494 || TargetObj.Class == 495 || TargetObj.Class == 496 || TargetObj.Class == 497
					|| TargetObj.Class == 498 || TargetObj.Class == 499 || TargetObj.Class == 500 || TargetObj.Class == 501
					|| TargetObj.Class == 502)
				{
					gObjAddMsgSendDelayInSpecificQPos(TargetObj, 1, *lpCallObj, 800, 0, 0);
				}
				else
				{
					gObjAddMsgSendDelay(TargetObj, 1, *lpCallObj, 800, 0);
				}

				g_Crywolf.CrywolfMonsterDieProc(TargetObj.m_Index, lpCallObj->m_Index);
			}

			else
			{
				if (TargetObj.Class == 275 || TargetObj.Class == 295 || TargetObj.Class == 561 || TargetObj.Class == 43
					|| TargetObj.Class == 44 || TargetObj.Class == 53 || TargetObj.Class == 54 || TargetObj.Class == 55
					|| TargetObj.Class == 56 || TargetObj.Class == 78 || TargetObj.Class == 79 || TargetObj.Class == 80
					|| TargetObj.Class == 81 || TargetObj.Class == 82 || TargetObj.Class == 83 || TargetObj.Class == 493
					|| TargetObj.Class == 494 || TargetObj.Class == 495 || TargetObj.Class == 496 || TargetObj.Class == 497
					|| TargetObj.Class == 498 || TargetObj.Class == 499 || TargetObj.Class == 500 || TargetObj.Class == 501
					|| TargetObj.Class == 502 || TargetObj.Class == 627 || TargetObj.Class == 628 || TargetObj.Class == 629
					|| TargetObj.Class == 630 || TargetObj.Class == 631 || TargetObj.Class == 627 || TargetObj.Class == 628
					|| TargetObj.Class == 629 || TargetObj.Class == 630 || TargetObj.Class == 631 || TargetObj.Class == 459
					|| TargetObj.Class == 673) // Boss monsters, fixed for bug which boss sometimes doesn't drop item if he is killed by one hit
				{
					gObjAddMsgSendDelayInSpecificQPos(TargetObj, 1, *lpCallObj, 800, 0, 0);

					if (g_ConfigRead.data.common.BossKillMessage == true)
					{
						PMSG_NOTICE pNotice;
						char szMsg[256];
						LPMONSTER_ATTRIBUTE lpm = gMAttr.GetAttr(TargetObj.Class);

						int iMaxHitUser = gObjMonsterTopHitDamageUser(TargetObj);

						if (iMaxHitUser == -1)
						{
							iMaxHitUser = lpCallObj->m_Index;
						}

						wsprintf(szMsg, Lang.GetText(0, 581), lpm->m_Name, getGameObject(iMaxHitUser)->Name);
						TNotice::MakeNoticeMsg(&pNotice, 0, szMsg);
						TNotice::SendNoticeToAllUser(&pNotice);
					}
				}
				else
				{
					gObjAddMsgSendDelay(TargetObj, 1, *lpCallObj, 800, 0);
				}
			}

			if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
			{
				if (TargetObj.m_Attribute == 62)
				{
					char szMsg[256];

					switch (TargetObj.Class)
					{
					case 300: // Hero Mobs
					case 301:
					case 302:
					case 303:
					{
						PMSG_NOTICE pNotice;

						ItemCreate(*lpCallObj, lpCallObj->MapNumber, lpCallObj->X, lpCallObj->Y, ItemGetNumberMake(14, 13), 0, 0, 0, 0, 0, lpCallObj->m_Index, 0, 0, 0, 0, 0);

						if (gObjIsConnected(*lpCallObj))
						{
							wsprintf(szMsg, Lang.GetText(0, 216), TargetObj.Name, lpCallObj->Name);
						}
						else
						{
							wsprintf(szMsg, Lang.GetText(0, 217), TargetObj.Name);
						}

						TNotice::MakeNoticeMsg(&pNotice, 0, szMsg);

						for each (std::pair<int,CGameObject*> ObjEntry in gGameObjects)
						{
							CGameObject* lpObj = ObjEntry.second;
							if (lpObj->Connected == PLAYER_PLAYING && lpObj->Type == OBJ_USER)
							{
								IOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (BYTE*)&pNotice, pNotice.h.size);
							}
						}
					}
					break;

					case 295: // Erohim
					{
						PMSG_NOTICE pNotice;

						if (gObjIsConnected(*lpCallObj))
						{
							wsprintf(szMsg, Lang.GetText(0, 218), lpCallObj->Name);
						}
						else
						{
							wsprintf(szMsg, Lang.GetText(0, 219));
						}

						TNotice::MakeNoticeMsg(&pNotice, 0, szMsg);

						for each (std::pair<int,CGameObject*> ObjEntry in gGameObjects)
						{
							CGameObject* lpObj = ObjEntry.second;
							if (lpObj->Connected == PLAYER_PLAYING && lpObj->Type == OBJ_USER)
							{
								IOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (BYTE*)&pNotice, pNotice.h.size);
							}
						}
					}
					break;
					}
				}
			}

			else
			{
				g_Kanturu.KanturuMonsterDieProc(TargetObj.m_Index, lpCallObj->m_Index);
			}

			if (TargetObj.Class >= 678 && TargetObj.Class <= 680)
			{
				KillLordSilvesterRecallMon(TargetObj.m_RecallMon, TargetObj.m_Index);
			}

			if (BC_MAP_RANGE(TargetObj.MapNumber))
			{
				int iBridgeIndex = g_BloodCastle.GetBridgeIndex(TargetObj.MapNumber);

				if (TargetObj.Class == 131)
				{
					g_BloodCastle.DestroyCastleDoor(iBridgeIndex, TargetObj);
				}

				if (BC_STATUE_RANGE(TargetObj.Class - 132))
				{
					g_BloodCastle.DestroySaintStatue(iBridgeIndex, TargetObj);
				}
			}

			if (g_ArcaBattle.IsArcaBattleServer() == TRUE && TargetObj.Type >= OBJ_MONSTER && g_ArcaBattle.GetState() == 7)
			{
				if (Obj.m_Attribute == 100)
				{
					CGameObject* lpCallObj = getGameObject(Obj.m_RecallMon);

					if (lpCallObj->Type != OBJ_USER)
					{
						return;
					}
				}

				g_ArcaBattle.IsOccupyObelisk(Obj, TargetObj.Class);

				if (TargetObj.Class == 592 || TargetObj.Class == 593 || TargetObj.Class == 594 || TargetObj.Class == 595 || TargetObj.Class == 596
					|| TargetObj.Class == 611 || TargetObj.Class == 612 || TargetObj.Class == 613 || TargetObj.Class == 614 || TargetObj.Class == 615)
				{
					g_ArcaBattle.AddContributePoint(Obj, TargetObj);
				}

				g_ArcaBattle.AddKillPoint(Obj, TargetObj);
			}

			if (CC_MAP_RANGE(TargetObj.MapNumber))
			{
				if (rand() % 100 < 50)
				{
					g_ChaosCastle.SearchNBlowObjs(TargetObj.MapNumber, TargetObj.X, TargetObj.Y);
				}

				g_ChaosCastle.DelMonsterList(g_ChaosCastle.GetChaosCastleIndex(TargetObj.MapNumber), TargetObj.m_Index);

				int TopHitUser = gObjMonsterTopHitDamageUser(TargetObj);
				CGameObject* lpObjTopHit = getGameObject(TopHitUser);
				if (TopHitUser != -1)
				{
					if (CC_MAP_RANGE(getGameObject(TopHitUser)->MapNumber))
					{
						if (gObjIsConnected(*lpObjTopHit))
						{
							getGameObject(TopHitUser)->m_cKillMonsterCount += 1;

							int KillMonsterCount = getGameObject(TopHitUser)->m_cKillMonsterCount;
							int CCIndex = g_ChaosCastle.GetChaosCastleIndex(getGameObject(TopHitUser)->MapNumber);
							g_QuestExpProgMng.ChkUserQuestTypeEventMap(QUESTEXP_ASK_CHAOSCASTLE_MON_KILL, Obj, CCIndex, KillMonsterCount);
						}
					}
				}
			}

			if (TargetObj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
			{
				if (rand() % 100 < 50)
				{
					g_ChaosCastleFinal.SearchNBlowObjs(TargetObj.MapNumber, TargetObj.X, TargetObj.Y);
				}

				g_ChaosCastleFinal.DelMonsterList(TargetObj.m_Index);

				int nMaxHitterIndex = gObjMonsterTopHitDamageUser(TargetObj);
				CGameObject* lpObjMaxHitter = getGameObject(nMaxHitterIndex);
				if (nMaxHitterIndex != -1 && lpObjMaxHitter->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL && gObjIsConnected(*lpObjMaxHitter))
					lpObjMaxHitter->m_byKillMobCount++;
			}

			if (DG_MAP_RANGE(TargetObj.MapNumber))
			{
				if (TargetObj.Class == 531)
				{
					g_DoppelGanger.DelIceWorkerIndex(TargetObj.m_Index);
				}

				if (TargetObj.Class == 530)
				{
					g_DoppelGanger.SetKillerState(4);
					g_DoppelGanger.SetMiddleBossIndex(530, TargetObj.m_Index);
					g_DoppelGanger.AddMiddleTreasureBoxAll(TargetObj.X, TargetObj.Y);
				}

				if (TargetObj.Class == 529)
				{
					g_DoppelGanger.SetAngerKillerState(4);
					g_DoppelGanger.SetMiddleBossIndex(529, TargetObj.m_Index);
					g_DoppelGanger.AddMiddleTreasureBoxAll(TargetObj.X, TargetObj.Y);
				}
			}

			if (TargetObj.MapNumber == MAP_INDEX_DEVILSQUARE_FINAL && g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE && Obj.Type == OBJ_USER)
			{
				int nPoint = g_DevilSquareFinal.MonsterKillPoint(TargetObj.Class);
				g_DevilSquareFinal.MonsterKillPointCalc(Obj, nPoint);
				g_DevilSquareFinal.DSFDropReward(Obj, TargetObj);

				sLog->outBasic("[DSF][MonsterKill][%s][%s][%s][%d] MonsterType:%d, KillPoint:%d",
					Obj.AccountID, Obj.m_PlayerData->m_RealNameOfUBF,
					Obj.Name, Obj.m_PlayerData->m_nServerCodeOfHomeWorld,
					TargetObj.Class, nPoint);
			}

			gObjPkDownTimeCheck(*lpCallObj, TargetObj.Level);

			if (lpCallObj->Type == OBJ_USER)
			{
				gObjAddMsgSendDelay(*lpCallObj, 3, TargetObj, 2000, 0);
			}
		}
		else if (TargetObj.Type == OBJ_USER)
		{
			if (BC_MAP_RANGE(TargetObj.MapNumber))
			{
				g_BloodCastle.SetUserState(TargetObj, 1);
				g_BloodCastle.SearchUserDropQuestItem(TargetObj);
			}

			if (IT_MAP_RANGE(TargetObj.MapNumber))
			{
				g_IT_Event.DropRelicsItem(TargetObj.MapNumber, TargetObj);
			}

			if (CC_MAP_RANGE(TargetObj.MapNumber))
			{
				g_ChaosCastle.SetUserState(TargetObj, 1);

				if (CC_MAP_RANGE(lpCallObj->MapNumber))
				{
					if (gObjIsConnected(*lpCallObj))
					{
						lpCallObj->m_cKillUserCount += 1;

						int KillUserCount = Obj.m_cKillUserCount;
						int CCIndex = g_ChaosCastle.GetChaosCastleIndex(Obj.MapNumber);
						g_QuestExpProgMng.ChkUserQuestTypeEventMap(QUESTEXP_ASK_CHAOSCASTLE_USER_KILL, Obj, CCIndex, KillUserCount);
					}
				}

				g_ChaosCastle.SendFailMessage(g_ChaosCastle.GetChaosCastleIndex(TargetObj.MapNumber), TargetObj.m_Index);

				sLog->outBasic("[Chaos Castle] (%d) [%s][%s] User Dead In Chaos Castle : Killed by [%s][%s]", g_ChaosCastle.GetChaosCastleIndex(TargetObj.MapNumber) + 1, TargetObj.AccountID, TargetObj.Name, lpCallObj->AccountID, lpCallObj->Name);
			}

			if (TargetObj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
			{
				g_ChaosCastleFinal.SetUserState(TargetObj, 1);

				if (lpCallObj->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL && gObjIsConnected(Obj) == TRUE)
				{
					lpCallObj->m_byKillUserCount++;
				}

				g_ChaosCastleFinal.SendFailMessage(TargetObj.m_Index);

				sLog->outBasic("[Chaos Castle Survival] [%s][%s] User Dead In Chaos Castle Survival : Killed by [%s][%s]", TargetObj.AccountID, TargetObj.Name, lpCallObj->AccountID, lpCallObj->Name);
				sLog->outBasic("[Chaos Castle Survival] [gObjLifeCheck] NAME :%s, Index :%d ", TargetObj.Name, TargetObj.m_Index);
			}

			TargetObj.KillerType = 0;

			gObjUserDie(TargetObj, *lpCallObj);

			int iGuildWar = gObjGuildWarCheck(*lpCallObj, TargetObj);
			int iDuel = gObjDuelCheck(*lpCallObj, TargetObj);

			if (iGuildWar == 0 && iDuel == 0)
			{
				if (TargetObj.m_bAttackerKilled == 0)
				{
					gObjPlayerKiller(*lpCallObj, TargetObj, MSBDamage);
				}
			}

			TargetObj.m_bAttackerKilled = 0;

			if (TargetObj.m_RecallMon >= 0)
			{
				gObjMonsterCallKill(TargetObj);
			}

			g_QuestExpProgMng.ChkUserQuestTypeEventMap(QUESTEXP_ASK_CONTRIBUTION_POINT, Obj, Obj.MapNumber, 0);
		}

		TargetObj.Live = 0;
		TargetObj.m_State = 4;
		TargetObj.RegenTime = GetTickCount();
		TargetObj.DieRegen = 1;
		TargetObj.PathCount = 0;

		if (gObjTargetGuildWarCheck(TargetObj, *lpCallObj) == 1)
		{
			TargetObj.KillerType = 2;
		}

		if (gObjDuelCheck(*lpCallObj, TargetObj) == TRUE)
		{
			TargetObj.KillerType = 3;
			g_NewPVP.CheckScore(*lpCallObj, TargetObj);
		}

		if (TargetObj.Teleport == 1)
		{
			TargetObj.Teleport = 0;
		}

		gGameProtocol.GCDiePlayerSend(TargetObj, TargetObj.m_Index, Skill, lpCallObj->m_Index);

		if (TargetObj.Class >= 524 && TargetObj.Class <= 528 && TargetObj.Class != 526)
		{
			g_ImperialGuardian.DestroyGate(TargetObj.m_nZoneIndex, TargetObj, Obj);
		}

		if (Obj.Class == 523)
		{
			sLog->outBasic("[IMPERIALGUARDIAN] Player Killed by Trap [AccountID]:%s, [NAME]:%s, [ZONE]:%d",
				TargetObj.AccountID, TargetObj.Name, Obj.m_nZoneIndex + 1);
		}

		if (Obj.Class >= 504 && Obj.Class <= 521)
		{
			sLog->outBasic("[IMPERIALGUARDIAN] Player Dead [Zone]:%d [AccountID]:%s, [NAME]:%s by Monster => %s(%d)",
				Obj.m_nZoneIndex + 1, TargetObj.AccountID, TargetObj.Name, Obj.Name, Obj.m_Index);
		}

		if (TargetObj.Class >= 504 && TargetObj.Class <= 521)
		{
			sLog->outBasic("[IMPERIALGUARDIAN] Monster Dead [TYPE]: %d,[NAME]: %s, [ZONE]:%d",
				TargetObj.Class, TargetObj.Name, TargetObj.m_nZoneIndex + 1);
		}

		if ((TargetObj.Class >= 504 && TargetObj.Class <= 511) || TargetObj.Class == 526)
		{
			g_ImperialGuardian.DropItem(Obj, TargetObj);
		}

		if (TargetObj.MapNumber == MAP_INDEX_HATCHERY) //season4 add-on
		{
			if (TargetObj.Type >= OBJ_MONSTER)
			{
				if (TargetObj.Class == 460 ||
					TargetObj.Class == 461 ||
					TargetObj.Class == 462)
				{
					g_Raklion.BossEggDieDecrease();
					g_RaklionUtil.NotifyRaklionUserMonsterCount(g_Raklion.GetBossEggDieCount(), 0);
				}
			}
		}

		if (TargetObj.Class == 79)
		{
			gEledoradoEvent.m_BossGoldDerconMapNumber[TargetObj.m_BossGoldDerconMapNumber] = -1;
			gEledoradoEvent.CheckGoldDercon(lpCallObj->MapNumber);
		}

		if (TargetObj.m_RecallMon >= 0)
		{
			return;
		}

		if (TargetObj.Class == 340 || TargetObj.Class == 348 || TargetObj.Class == 349)
		{
			return;
		}

		if (TargetObj.Class == 287 || TargetObj.Class == 286)
		{
			return;
		}

		if (TargetObj.Class == 681 || TargetObj.Class == 690)
		{
			g_EvoMonMng.EvolutionMonsterDie(TargetObj, Obj);
		}

		if (TargetObj.Class == 275)
		{
			return;
		}

		if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
		{
			if (TargetObj.Class == 288)
			{
				return;
			}
		}

		if (TargetObj.Class == 295)
		{
			return;
		}

		if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
		{
			if (TargetObj.Class == 362 || TargetObj.Class == 363 || TargetObj.Class == 361)
			{
				return;
			}
		}

		if (TargetObj.Class == 412) //Season 2.5 Dark Elf Hero
		{
			return;
		}

		if (TargetObj.Class >= 524 && TargetObj.Class <= 528 && TargetObj.Class != 526)
		{
			return;
		}

		if (TargetObj.MapNumber == MAP_INDEX_DEVILSQUARE_FINAL)
		{
			return;
		}

		if (lpCallObj->Type == OBJ_USER)
		{
			//sLog->outBasic("[K2] m_btExpType %d", TargetObj.m_btExpType);
			//	TargetObj.m_btExpType = 1;
			if (TargetObj.m_btExpType == 1)
			{
				gObjMonsterExpDivisionRenewal(TargetObj, *lpCallObj, AttackDamage, MSBFlag);
			}

			if (lpCallObj->PartyNumber >= 0)
			{
				gObjExpParty(*lpCallObj, TargetObj, AttackDamage, MSBFlag);
			}
			else if (TargetObj.Type == OBJ_MONSTER)
			{
				gObjMonsterExpDivision(TargetObj, *lpCallObj, AttackDamage, MSBFlag);
			}

			if (TargetObj.Type == OBJ_MONSTER && DS_MAP_RANGE(TargetObj.MapNumber))
			{
				g_DevilSquare.gObjMonsterScoreDivision(TargetObj, *lpCallObj, AttackDamage, MSBFlag);
			}
		}

		if (TargetObj.Type == OBJ_MONSTER)
		{
			if (TargetObj.m_bIsInMonsterHerd != 0 && TargetObj.m_lpMonsterHerd != 0)
			{
				TargetObj.m_lpMonsterHerd->MonsterDieAction(TargetObj);
			}
		}
	}
	else
	{
		if (TargetObj.Class == 673)
		{
			if (!TargetObj.m_nPhaseLordSilvester && TargetObj.MaxLife * 0.7 >= TargetObj.Life)
				gObjMonsterSummonSkillForLordSilvester(TargetObj);
			if (TargetObj.m_nPhaseLordSilvester == 1 && TargetObj.MaxLife * 0.5 >= TargetObj.Life)
				gObjMonsterSummonSkillForLordSilvester(TargetObj);
			if (TargetObj.m_nPhaseLordSilvester == 2 && TargetObj.MaxLife * 0.2 >= TargetObj.Life)
				gObjMonsterSummonSkillForLordSilvester(TargetObj);
		}
		//MSBDamage 0x03 -> Luck

		if (iElementalDamage == TRUE)
		{
			gGameProtocol.GCElementalDamageSend(*lpCallObj, TargetObj.m_Index, AttackDamage, MSBDamage);
		}

		else if (DamageSendType == 0)
		{
			gGameProtocol.GCDamageSend(*lpCallObj, TargetObj, AttackDamage, MSBFlag, MSBDamage, iShieldDamage);
		}
		else if (DamageSendType == 1)
		{
			gGameProtocol.GCDamageSendPoison(TargetObj, AttackDamage, iShieldDamage);
		}
		else if (DamageSendType == 2)
		{
			gGameProtocol.GCDamageSend(*lpCallObj, TargetObj, AttackDamage, MSBFlag, 5, iShieldDamage);
		}
		else if (DamageSendType == 3)
		{
			gGameProtocol.GCDamageSend(*lpCallObj, TargetObj, AttackDamage, MSBFlag, 6, iShieldDamage);
		}
	}

	if (TargetObj.Type == OBJ_MONSTER)
	{
		gGameProtocol.GCSendObjectHP(*lpCallObj, TargetObj.m_Index);
	}

	if (TargetObj.Type == OBJ_MONSTER)
	{
		for (int i = 0; i < MAX_VIEWPORT; i++)
		{
			if (TargetObj.VpPlayer[i].type == OBJ_USER)
			{
				gGameProtocol.GCObjectLifeInfo(*getGameObject(TargetObj.VpPlayer[i].number), TargetObj.m_Index, TargetObj.MaxLife + TargetObj.AddLife, TargetObj.Life);
			}
		}
	}
}



BOOL gObjInventoryTrans(CGameObject &Obj)
{
	if (Obj.pTransaction == 1 && Obj.m_PlayerData->ISBOT == FALSE)
	{
		return FALSE;
	}

	for (int n = 0; n < MAX_MAGIC; n++)
	{
		Obj.m_lpMagicBack[n] = Obj.Magic[n];
	}

	for (int n = 0; n < INVENTORY_SIZE; n++)
	{
		Obj.pntInventory2[n] = Obj.pntInventory1[n];
	}

	for (int n = 0; n < EVENT_INVENTORY_SIZE; n++)
	{
		Obj.pntEventInventory2[n] = Obj.pntEventInventory1[n];
	}

	std::memcpy(Obj.InventoryMap2, Obj.InventoryMap1, INVENTORY_MAP_SIZE);
	std::memcpy(Obj.EventInventoryMap2, Obj.EventInventoryMap1, EVENT_INVENTORY_MAP_SIZE);

	Obj.InventoryCount2 = Obj.InventoryCount1;
	gObjSetInventory2Pointer(Obj);
	gObjSetEventInventory2Pointer(Obj);
	Obj.pTransaction = 1;


	return TRUE;
}

// CMagicInf::operator= 4c4d90




int gObjInventoryCommit(CGameObject &Obj)
{
	if (Obj.pTransaction != 1)
	{
		return false;
	}

	for (int n = 0; n < INVENTORY_SIZE; n++)
	{
		Obj.pntInventory1[n] = Obj.pntInventory2[n];
	}

	std::memcpy(Obj.InventoryMap1, Obj.InventoryMap2, INVENTORY_MAP_SIZE);

	Obj.InventoryCount1 = Obj.InventoryCount2;

	gObjSetInventory1Pointer(Obj);

	for (int n = 0; n < EVENT_INVENTORY_SIZE; n++)
	{
		Obj.pntEventInventory1[n] = Obj.pntEventInventory2[n];
	}

	std::memcpy(Obj.EventInventoryMap1, Obj.EventInventoryMap2, EVENT_INVENTORY_MAP_SIZE);

	gObjSetEventInventory1Pointer(Obj);

	Obj.pTransaction = 2;

	return true;
}




int gObjInventoryRollback(CGameObject &Obj)
{
	if (Obj.pTransaction != 1)
	{
		return false;
	}

	for (int n = 0; n < MAX_MAGIC; n++)
	{
		Obj.Magic[n] = Obj.m_lpMagicBack[n];
	}

	for (int n = 0; n < INVENTORY_SIZE; n++)
	{
		Obj.pntInventory2[n]->Clear();
	}

	for (int n = 0; n < EVENT_INVENTORY_SIZE; n++)
	{
		Obj.pntEventInventory2[n]->Clear();
	}

	gObjSetInventory1Pointer(Obj);
	gObjSetEventInventory1Pointer(Obj);

	Obj.pTransaction = 3;

	return true;
}




void gObjInventoryItemSet(CGameObject &Obj, int itempos, BYTE set_byte)
{
	int width;
	int height;
	if (itempos < INVETORY_WEAR_SIZE || itempos == 236)
	{
		return;
	}
	else if (itempos >(INVENTORY_SIZE - 1))
	{
		return;
	}

	if (Obj.pntInventory[itempos]->GetSize((int&)width, (int &)height) == 0)
	{
		return;
	}

	gObjInventoryItemBoxSet(Obj, itempos, width, height, set_byte);
}


void gObjInventoryItemBoxSet(CGameObject &Obj, int itempos, int xl, int yl, BYTE set_byte)
{
	int itemposx = (itempos - INVETORY_WEAR_SIZE) % 8;
	int itemposy = (itempos - INVETORY_WEAR_SIZE) / 8;

	int xx, yy;

	if (!CheckOutOfInventory(Obj, itemposy, yl))	//OK
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
				*(BYTE*)(Obj.pInventoryMap + (itemposy + y) * 8 + (itemposx + x)) = set_byte;
			}

			else
			{
				return;
			}
		}
	}
}

bool gObjFixInventoryPointer(CGameObject &Obj)
{

	if (gObjIsConnected(Obj) == 0)
	{
		return false;
	}

	if (Obj.pntInventory == Obj.pntInventory1)
	{
		return true;
	}

	if (Obj.pntInventory == Obj.pntInventory2)
	{
		if (Obj.pTransaction == 1)
		{
			return false;
		}
		else
		{

			for (int n = 0; n < INVENTORY_SIZE; n++)
			{
				Obj.pntInventory2[n]->Clear();
			}
		}
	}
	else
	{

	}

	gObjSetInventory1Pointer(Obj);

	return false;
}

BYTE gObjInventoryDeleteItem(CGameObject &Obj, int itempos)
{
	if (itempos < INVETORY_WEAR_SIZE || itempos == 236)
	{
		int MagicDel = gObjMagicDel(Obj, Obj.pntInventory[itempos]->m_Special[0], Obj.pntInventory[itempos]->m_Level);

		if (MagicDel >= 0)
		{
			gGameProtocol.GCMagicListOneDelSend(Obj, MagicDel,
				Obj.pntInventory[itempos]->m_Special[0],
				Obj.pntInventory[itempos]->m_Level, 0, 0);
		}

		Obj.pntInventory[itempos]->Clear();
	}
	else
	{
		gObjInventoryItemSet(Obj, itempos, 255);
		Obj.pntInventory[itempos]->Clear();
	}

	return true;
}

BYTE gObjWarehouseDeleteItem(CGameObject &Obj, int itempos)
{
	gObjWarehouseItemSet(Obj, itempos, -1);
	Obj.pntWarehouse[itempos]->Clear();

	return true;
}

BYTE gObjChaosBoxDeleteItem(CGameObject &Obj, int itempos)
{
	gObjChaosItemSet(Obj, itempos, -1);
	Obj.pntChaosBox[itempos]->Clear();

	return true;
}

BYTE gObjInventoryInsertItem(CGameObject &Obj, int item_num)
{
	int w, h, map_num, iwidth, iheight;
	BYTE blank;

	blank = 0;
	map_num = Obj.MapNumber;

	if (MapC[map_num].m_CItemObject[item_num].live == 0)
	{
		return -1;
	}

	if (MapC[map_num].m_CItemObject[item_num].GetSize((int &)iwidth, (int &)iheight) == 0)
	{
		sLog->outBasic("error: Item does not exist %s %d", __FILE__, __LINE__);
		return -1;
	}

	for (h = 0; h < 8; h++)
	{
		for (w = 0; w < 8; w++)
		{
			if (*(BYTE*)(Obj.pInventoryMap + h * 8 + w) == 255)
			{
				blank = gObjOnlyInventoryRectCheck(Obj, w, h, iwidth, iheight);

				if (blank == 254)
				{
					goto GOTO_EndFunc;
				}

				if (blank != 255)
				{
					// TODO
					//Obj.pntInventory[blank] = MapC[map_num]->m_CItemObject[item_num];
					gObjInventoryItemSet(Obj, blank, Obj.pntInventory[blank]->m_Type);
					return blank;
				}
			}
		}
	}
GOTO_EndFunc:
	return -1;

}

DWORD gGetItemNumber()
{
	return gItemNumberCount;
}

void gPlusItemNumber()
{
	gItemNumberCount++;
}

BYTE gObjInventoryInsertItem(CGameObject &Obj, CItemObject &item)
{
	int w, h, iwidth, iheight;
	BYTE blank = 0;

	if (item.GetSize((int&)iwidth, (int&)iheight) == 0)
	{
		sLog->outBasic("error: Item does not exist %s %d", __FILE__, __LINE__);
		return -1;
	}

	for (h = 0; h < MAX_INVENTORY_H; h++)
	{
		for (w = 0; w < 8; w++)
		{
			if (*(BYTE*)(Obj.pInventoryMap + h * 8 + w) == 255)
			{
				blank = gObjInventoryRectCheck(Obj, w, h, iwidth, iheight);

				if (blank == 254)
				{
					goto GOTO_EndFunc;
				}

				if (blank != 255)
				{
					if (gObjCheckSerial0ItemList(&item) != 0)
					{
						MsgOutput(Obj, Lang.GetText(0, 259));
						return -1;
					}

					if (gObjInventorySearchSerialNumber(Obj, item.m_Number) == 0)
					{
						return -1;
					}

					Obj.pntInventory[blank] = &item;

					gObjInventoryItemSet(Obj, blank, Obj.pntInventory[blank]->m_Type);
					return blank;
				}
			}
		}
	}

GOTO_EndFunc:
	return -1;
}

BYTE gObjOnlyInventoryInsertItem(CGameObject &Obj, CItemObject &item)
{
	int w, h, iwidth, iheight;
	BYTE blank = 0;

	if (item.GetSize((int&)iwidth, (int&)iheight) == 0)
	{
		sLog->outBasic("error: Item does not exist %s %d", __FILE__, __LINE__);
		return -1;
	}

	for (h = 0; h < MAX_INVENTORY_H; h++)	//OK
	{
		for (w = 0; w < 8; w++)
		{
			if (*(BYTE*)(Obj.pInventoryMap + h * 8 + w) == 255)
			{
				blank = gObjOnlyInventoryRectCheck(Obj, w, h, iwidth, iheight);

				if (blank == 254)
				{
					goto GOTO_EndFunc;
				}

				if (blank != 255)
				{
					if (gObjCheckSerial0ItemList(&item) != 0)
					{
						MsgOutput(Obj, Lang.GetText(0, 259));
						return -1;
					}

					if (gObjInventorySearchSerialNumber(Obj, item.m_Number) == 0)
					{
						return -1;
					}

					Obj.pntInventory[blank] = &item;

					gObjInventoryItemSet(Obj, blank, Obj.pntInventory[blank]->m_Type);
					return blank;
				}
			}
		}
	}

GOTO_EndFunc:
	return -1;
}

BYTE gObjInventoryInsertItem(CGameObject &Obj, CMapItem &item)
{
	int w, h, iwidth, iheight;
	BYTE blank = 0;

	CItemObject copyitem;

	if (item.GetSize((int &)iwidth, (int &)iheight) == 0)
	{
		return -1;
	}

	for (h = 0; h < MAX_INVENTORY_H; h++)
	{
		for (w = 0; w < 8; w++)
		{
			if (*(BYTE*)(Obj.pInventoryMap + h * 8 + w) == 255)
			{
				blank = gObjOnlyInventoryRectCheck(Obj, w, h, iwidth, iheight);
				if (blank == 254)
				{
					goto GOTO_EndFunc;
				}

				if (blank != 255)
				{
					copyitem.m_Level = item.m_Level;
					copyitem.m_Durability = item.m_Durability;

					copyitem.Convert(item.m_Type, item.m_Option1, item.m_Option2, item.m_Option3, item.m_NewOption, item.m_SetOption, item.m_ItemOptionEx, item.m_SocketOption, item.m_BonusSocketOption, 0, CURRENT_DB_VERSION);
					copyitem.SetPetItemInfo(item.m_PetItem_Level, item.m_PetItem_Exp);

					copyitem.m_Number = item.m_Number;

					Obj.pntInventory[blank] = &copyitem;

					gObjInventoryItemSet(Obj, blank, Obj.pntInventory[blank]->m_Type);
					return blank;
				}
			}
		}
	}

GOTO_EndFunc:
	return -1;
}




BOOL gObjSearchItem(CGameObject &Obj, int item_type, int add_dur, int nLevel)
{
	for (int n = INVETORY_WEAR_SIZE; n < MAIN_INVENTORY_SIZE; n++)
	{
		if (Obj.pntInventory[n]->IsItem() == 1)
		{
			if (Obj.pntInventory[n]->m_Type == item_type)
			{
				if (Obj.pntInventory[n]->m_Level == nLevel)
				{
					if ((int)Obj.pntInventory[n]->m_Durability + add_dur <= 3)
					{
						Obj.pntInventory[n]->m_Durability += add_dur;
						gGameProtocol.GCItemObjectDurSend(Obj, n, Obj.pntInventory[n]->m_Durability, 0);
						return true;
					}
				}
			}
		}
	}
	return false;
}




BOOL gObjSearchItemMinus(CGameObject &Obj, int pos, int m_dur)
{
	int n = pos;
	if (n < 0 || n >(MAIN_INVENTORY_SIZE - 1))
	{
		return 1;
	}

	if (Obj.pntInventory[n]->IsItem() == 1)
	{
		if ((int)Obj.pntInventory[n]->m_Durability - m_dur > 0)
		{
			if (Obj.pntInventory[n]->m_Type >= ITEMGET(14, 0) && Obj.pntInventory[n]->m_Type <= ITEMGET(14, 6) || Obj.pntInventory[n]->m_Type >= ITEMGET(14, 35) && Obj.pntInventory[n]->m_Type <= ITEMGET(14, 40) || Obj.pntInventory[n]->m_Type == ITEMGET(14, 70) || Obj.pntInventory[n]->m_Type == ITEMGET(14, 71))
			{
				Obj.m_PlayerData->PotionTime = GetTickCount();
			}
			Obj.pntInventory[n]->m_Durability -= m_dur;
			gGameProtocol.GCItemObjectDurSend(Obj, n, Obj.pntInventory[n]->m_Durability, 1);
			return 1;
		}
	}
	return 0;
}



BYTE gObjShopBuyInventoryInsertItem(CGameObject &Obj, CItemObject &item)
{
	int w, h, iwidth, iheight;
	BYTE blank = 0;

	if (item.GetSize(iwidth, iheight) == 0)
	{
		sLog->outBasic("error: Item does not exist %s %d", __FILE__, __LINE__);
		return -1;
	}

	for (h = 0; h < MAX_INVENTORY_H; h++)	//24
	{
		for (w = 0; w < 8; w++)
		{
			if (*(BYTE*)(Obj.pInventoryMap + h * 8 + w) == 255)
			{
				blank = gObjOnlyInventoryRectCheck(Obj, w, h, iwidth, iheight);

				if (blank == 254)
				{
					goto GOTO_EndFunc;
				}

				if (blank != 255)
				{
					Obj.pntInventory[blank] = &item;
					Obj.pntInventory[blank]->m_Number = 0;

					gObjInventoryItemSet(Obj, blank, Obj.pntInventory[blank]->m_Type);
					return blank;
				}
			}
		}
	}
GOTO_EndFunc:
	return -1;
}

BYTE gObjShopBuyMuunInventoryInsertItem(CGameObject &Obj, CItemObject &item)
{
	int w, h, iwidth, iheight;
	BYTE blank = 0;

	if (item.GetSize(iwidth, iheight) == 0)
	{
		sLog->outBasic("error: Item does not exist %s %d", __FILE__, __LINE__);
		return -1;
	}

	return gObjMuunInventoryInsertItem(Obj, item);
}

BYTE gObjShopBuyEventInventoryInsertItem(CGameObject &Obj, CItemObject &item)
{
	int w, h, iwidth, iheight;
	BYTE blank = 0;

	if (item.GetSize(iwidth, iheight) == 0)
	{
		sLog->outBasic("error: Item does not exist %s %d", __FILE__, __LINE__);
		return -1;
	}

	for (h = 0; h < 4; h++)	//24
	{
		for (w = 0; w < 8; w++)
		{
			if (*(BYTE*)(Obj.EventInventoryMap + h * 8 + w) == 255)
			{
				blank = gObjEventInventoryRectCheck(Obj, w, h, iwidth, iheight);

				if (blank == 254)
				{
					goto GOTO_EndFunc;
				}

				if (blank != 255)
				{
					Obj.pntEventInventory[blank] = &item;
					Obj.pntEventInventory[blank]->m_Number = 0;

					gObjEventInventoryItemSet(Obj, blank, Obj.pntEventInventory[blank]->m_Type);
					return blank;
				}
			}
		}
	}
GOTO_EndFunc:
	return -1;
}

BYTE gObjInventoryInsertItemTemp(CGameObject &Obj, CMapItem * Item)
{
	int w, h, iwidth, iheight;
	BYTE blank = 0;

	if (Item->GetSize((int &)iwidth, (int &)iheight) == 0)
	{
		sLog->outBasic("error: Item does not exist %s %d", __FILE__, __LINE__);
		return -1;
	}

	for (h = 0; h < MAX_INVENTORY_H; h++)	//24
	{
		for (w = 0; w < 8; w++)
		{
			if (*(BYTE*)(Obj.pInventoryMap + h * 8 + w) == 255)
			{
				blank = gObjInventoryRectCheck(Obj, w, h, iwidth, iheight);

				if (blank == 254)
				{
					goto GOTO_EndFunc;
				}

				if (blank != 255)
				{
					return blank;
				}
			}
		}
	}
GOTO_EndFunc:
	return -1;
}

BYTE gObjInventoryRectCheck(CGameObject &Obj, int sx, int sy, int width, int height)
{
	int x, y;
	int blank = 0;

	if (sx + width > 8)
	{
		return -1;
	}

	if (sy + height > MAX_INVENTORY_H2)	//28
	{
		return -2;
	}
	// ----
	bool bResult = CheckOutOfInventory(Obj, sy, height);
	// ----
	if (!bResult)
	{
		if (bResult == -1)
		{
			return -2;
		}
		// ----
		return -1;
	}
	// ----
	int xx, yy;

	for (y = 0; y < height; y++)
	{

		yy = sy + y;

		for (x = 0; x < width; x++)
		{
			xx = sx + x;

			if (InventoryExtentCheck(xx, yy, 8, MAX_INVENTORY_H2))
			{
				if (*(BYTE*)(Obj.pInventoryMap + (sy + y) * 8 + (sx + x)) != 255)
				{
					blank += 1;
					return -1;
				}
			}
			else
			{
				sLog->outBasic("error : %s %d", __FILE__, __LINE__);
				return -1;
			}
		}
	}

	if (blank == 0)
	{
		return sx + sy * 8 + 12;	//strange
	}
	return  -1;
}




BYTE gObjOnlyInventoryRectCheck(CGameObject &Obj, int sx, int sy, int width, int height)
{
	int x, y;
	int blank = 0;

	if (sx + width > 8)
	{
		return -1;
	}

	if (sy + height > 4 * Obj.m_PlayerData->m_InventoryExpansion + 8)
	{
		return -2;
	}

	int nInventoyHeightofCurUser = CheckOutOfInventory(Obj, sy, height);
	if (!nInventoyHeightofCurUser)
	{
		if (nInventoyHeightofCurUser == -1)
		{
			return -2;
		}
		return -1;
	}

	int xx, yy;

	for (y = 0; y < height; y++)
	{

		yy = sy + y;

		for (x = 0; x < width; x++)
		{
			xx = sx + x;

			if (InventoryExtentCheck(xx, yy, 8, MAX_INVENTORY_H))	//24
			{
				if (*(BYTE*)(Obj.pInventoryMap + (sy + y) * 8 + (sx + x)) != 255)
				{
					blank += 1;
					return -1;
				}
			}
			else
			{
				sLog->outBasic("error : %s %d", __FILE__, __LINE__);
				return -1;
			}
		}
	}

	if (blank == 0)
	{
		return sx + sy * 8 + 12;	//strange again
	}
	return  -1;
}

BOOL CheckInventoryEmptySpace(CGameObject &Obj, int iItemHeight, int iItemWidth)
{
	int h = 0;
	int w = 0;
	BYTE blank = 0;

	for (h = 0; h < MAX_INVENTORY_H; h++)	//24
	{
		for (w = 0; w < 8; w++)
		{
			if (*(BYTE*)(Obj.pInventoryMap + (h) * 8 + (w)) == 255)
			{
				blank = gObjOnlyInventoryRectCheck(Obj, w, h, iItemWidth, iItemHeight);

				if (blank == 0xFE)
					return FALSE;

				if (blank != 0xFF)
					return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL gObjIsItemPut(CGameObject &Obj, CItemObject * lpItem, int pos) // check this
{
	//sLog->outError( "[K2] lpItem->m_TwoHand %d Obj.pntInventory[1]->IsItem() %d", lpItem->m_TwoHand, Obj.pntInventory[1]->IsItem());
	if (lpItem->m_TwoHand == 1)
	{
		if (pos == 0 || pos == 1)
		{
			if (Obj.pntInventory[1]->IsItem() == 1)
			{
				if (Obj.pntInventory[1]->m_Type == ITEMGET(4, 7) || Obj.pntInventory[1]->m_Type == ITEMGET(4, 15) || (Obj.pntInventory[1]->m_Type >= ITEMGET(4, 32) && Obj.pntInventory[1]->m_Type <= ITEMGET(4, 35)))
				{

				}
				else
				{
					return false;
				}
			}
		}
	}

	if (Obj.Class == CLASS_DARKLORD &&
		lpItem->m_Type >> 9 == 2 &&
		lpItem->m_Part != pos)
	{
		return FALSE;
	}

	if (Obj.Class == CLASS_WIZARD &&
		lpItem->m_Type >> 9 == 5 &&
		lpItem->m_Part != pos)
	{
		return FALSE;
	}

	if (Obj.Class == CLASS_SUMMONER &&
		lpItem->m_Type >> 9 == 5 &&
		lpItem->m_Part != pos)
	{
		return FALSE;
	}

	/*	if(pos == 1)
	{
	if(Obj.pntInventory[0]->IsItem() == 1)
	{
	if(lpItem->m_Type == ITEMGET(4,7) || lpItem->m_Type == ITEMGET(4,15))
	{

	}
	else
	{
	if(Obj.pntInventory[0]->m_TwoHand)
	{
	return false;
	}
	}
	}
	}
	*/
	if (lpItem->m_Part != pos)
	{
		if (pos == 1 || pos == 0)
		{
			if (lpItem->m_Part == 1 || lpItem->m_Part == 0)
			{

			}
			else
			{
				return false;
			}
		}
		else if (pos == 11)
		{
			if (lpItem->m_Part == 10)
			{

			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	if ((Obj.m_PlayerData->Strength + Obj.AddStrength) < (lpItem->m_RequireStrength - lpItem->m_HJOpStrength))
	{
		return false;
	}

	if ((Obj.m_PlayerData->Dexterity + Obj.AddDexterity) < (lpItem->m_RequireDexterity - lpItem->m_HJOpDexterity))
	{
		return false;
	}

	if (lpItem->IsPeriodItemExpire() == TRUE)
	{
		return false;
	}

	if (Obj.Level + Obj.m_PlayerData->MasterLevel < lpItem->m_RequireLevel)
	{
		return false;
	}

	if (lpItem->m_Type >= 0 && lpItem->m_Type < ITEMGET(12, 0))
	{
		if ((Obj.m_PlayerData->Vitality + Obj.AddVitality) < lpItem->m_RequireVitality)
		{
			return false;
		}

		if ((Obj.m_PlayerData->Energy + Obj.AddEnergy) < lpItem->m_RequireEnergy)
		{
			return false;
		}
	}

	if ((Obj.Leadership + Obj.AddLeadership) < lpItem->m_RequireLeaderShip)
	{
		return false;
	}

	if (GetWingType(lpItem->m_Type, WCF_ITEMNUMBER) != NONE_WING)
	{
		if (Obj.Level < lpItem->m_RequireLevel)
		{
			return false;
		}
	}

	else if (lpItem->m_Type >= ITEMGET(13, 0) && lpItem->m_Type <= ITEMGET(13, 15))
	{
		if (lpItem->m_Type == ITEMGET(13, 10))
		{
			if (lpItem->m_Level <= 2)
			{
				if (Obj.Level < 20)
				{
					return false;
				}
			}
			else
			{
				if (Obj.Level < 50)
				{
					return false;
				}
			}
		}
		else if (lpItem->m_Type == ITEMGET(13, 2))
		{
			if (Obj.MapNumber == 7 || Obj.Level < lpItem->m_RequireLevel)
			{
				return false;
			}
		}
		else if (lpItem->m_Type == ITEMGET(13, 3))
		{
			if (Obj.MapNumber == 7 || Obj.Level < lpItem->m_RequireLevel)
			{
				return false;
			}
		}
		else
		{
			if (Obj.Level < lpItem->m_RequireLevel)
			{
				return false;
			}
		}
	}

	if (lpItem->m_Type == ITEMGET(13, 20) && lpItem->m_Level == 0)
	{
		int count = gObjGetItemCountInEquipment(Obj, 13, 20, 0);

		if (count != 0)
		{
			return false;
		}
	}

	if (lpItem->m_Type == ITEMGET(13, 107) && lpItem->m_Level == 0)
	{
		int count = gObjGetItemCountInEquipment(Obj, 13, 107, 0);

		if (count != 0)
		{
			return false;
		}
	}

	if (lpItem->m_Type == ITEMGET(13, 10))
	{
		if (CC_MAP_RANGE(Obj.MapNumber) || Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
		{
			return false;
		}
	}

	if (lpItem->m_Type == ITEMGET(13, 37))
	{
		if (CC_MAP_RANGE(Obj.MapNumber) || Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
		{
			return false;
		}
	}

	if (lpItem->m_Type == ITEMGET(13, 37))
	{
		if (CC_MAP_RANGE(Obj.MapNumber) || Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
		{
			return false;
		}
	}

	if (lpItem->m_Type == ITEMGET(13, 76) || lpItem->m_Type == ITEMGET(13, 77) || lpItem->m_Type == ITEMGET(13, 78))
	{
		if (CC_MAP_RANGE(Obj.MapNumber) || Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
		{
			return false;
		}
	}

	if (IT_MAP_RANGE(Obj.MapNumber))
	{
		if (g_IT_Event.CheckChangeRing(lpItem->m_Type))
		{
			return false;
		}
	}

	if (lpItem->m_Type == ITEMGET(13, 122))
	{
		if (CC_MAP_RANGE(Obj.MapNumber) || Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
		{
			return false;
		}
	}

	if (lpItem->m_Type == ITEMGET(13, 38))
	{
		int count = gObjGetItemCountInEquipment(Obj, 13, 38, 0);

		if (count != 0)
		{
			return false;
		}
	}

	if (lpItem->m_Type == ITEMGET(13, 39))
	{
		if (CC_MAP_RANGE(Obj.MapNumber) || Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
		{
			return false;
		}

		int count = 0;

		for (int n = 0; n < 7; n++)
		{
			count = gObjGetItemCountInEquipment(Obj, 13, 10, n);

			if (count != 0)
			{
				return false;
			}
		}

		count = gObjGetItemCountInEquipment(Obj, 13, 39, 0);

		if (count != 0)
		{
			return false;
		}

		count = gObjGetItemCountInEquipment(Obj, 13, 40, 0);

		if (count != 0)
		{
			return false;
		}

		count = gObjGetItemCountInEquipment(Obj, 13, 41, 0);

		if (count != 0)
		{
			return false;
		}

		count = gObjGetItemCountInEquipment(Obj, 13, 42, 0);

		if (count != 0)
		{
			return false;
		}
	}

	if (lpItem->m_Type == ITEMGET(13, 40))
	{
		if (CC_MAP_RANGE(Obj.MapNumber) || Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
		{
			return false;
		}

		int count = 0;

		for (int n = 0; n < 7; n++)
		{
			count = gObjGetItemCountInEquipment(Obj, 13, 10, n);

			if (count != 0)
			{
				return false;
			}
		}

		count = gObjGetItemCountInEquipment(Obj, 13, 39, 0);

		if (count != 0)
		{
			return false;
		}

		count = gObjGetItemCountInEquipment(Obj, 13, 40, 0);

		if (count != 0)
		{
			return false;
		}

		count = gObjGetItemCountInEquipment(Obj, 13, 41, 0);

		if (count != 0)
		{
			return false;
		}

		count = gObjGetItemCountInEquipment(Obj, 13, 42, 0);

		if (count != 0)
		{
			return false;
		}
	}

	if (lpItem->m_Type == ITEMGET(13, 130))
	{
		int count = gObjGetItemCountInEquipment(Obj, 13, 132, 0);

		if (count != 0)
		{
			return false;
		}
	}

	if (lpItem->m_Type == ITEMGET(13, 131))
	{
		int count = gObjGetItemCountInEquipment(Obj, 13, 133, 0);

		if (count != 0)
		{
			return false;
		}
	}

	if (lpItem->m_Type == ITEMGET(13, 132))
	{
		int count = gObjGetItemCountInEquipment(Obj, 13, 130, 0);

		if (count != 0)
		{
			return false;
		}
	}

	if (lpItem->m_Type == ITEMGET(13, 133))
	{
		int count = gObjGetItemCountInEquipment(Obj, 13, 131, 0);

		if (count != 0)
		{
			return false;
		}
	}

	if (lpItem->m_Type == ITEMGET(13, 128))
	{
		int count = gObjGetItemCountInEquipment(Obj, 13, 129, 0);

		if (count != 0)
		{
			return false;
		}
	}

	if (lpItem->m_Type == ITEMGET(13, 129))
	{
		int count = gObjGetItemCountInEquipment(Obj, 13, 128, 0);

		if (count != 0)
		{
			return false;
		}
	}

	if (lpItem->m_Type == ITEMGET(13, 41))
	{
		if (CC_MAP_RANGE(Obj.MapNumber) || Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
		{
			return false;
		}

		int count = 0;

		for (int n = 0; n < 7; n++)
		{
			count = gObjGetItemCountInEquipment(Obj, 13, 10, n);

			if (count != 0)
			{
				return false;
			}
		}

		count = gObjGetItemCountInEquipment(Obj, 13, 39, 0);

		if (count != 0)
		{
			return false;
		}

		count = gObjGetItemCountInEquipment(Obj, 13, 40, 0);

		if (count != 0)
		{
			return false;
		}

		count = gObjGetItemCountInEquipment(Obj, 13, 41, 0);

		if (count != 0)
		{
			return false;
		}

		count = gObjGetItemCountInEquipment(Obj, 13, 42, 0);

		if (count != 0)
		{
			return false;
		}
	}

	if (lpItem->m_Type == ITEMGET(13, 42))
	{
		if (CC_MAP_RANGE(Obj.MapNumber) || Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
		{
			return false;
		}

		int count = 0;

		for (int n = 0; n < 7; n++)
		{
			count = gObjGetItemCountInEquipment(Obj, 13, 10, n);

			if (count != 0)
			{
				return false;
			}
		}

		count = gObjGetItemCountInEquipment(Obj, 13, 39, 0);

		if (count != 0)
		{
			return false;
		}

		count = gObjGetItemCountInEquipment(Obj, 13, 40, 0);

		if (count != 0)
		{
			return false;
		}

		count = gObjGetItemCountInEquipment(Obj, 13, 41, 0);

		if (count != 0)
		{
			return false;
		}

		count = gObjGetItemCountInEquipment(Obj, 13, 42, 0);

		if (count != 0)
		{
			return false;
		}
	}

	if (lpItem->m_Type == ITEMGET(13, 10))
	{
		int count = 0;

		for (int n = 0; n < 7; n++)
		{
			count = gObjGetItemCountInEquipment(Obj, 13, 10, n);

			if (count != 0)
			{
				return false;
			}
		}

		count = gObjGetItemCountInEquipment(Obj, 13, 39, 0);

		if (count != 0)
		{
			return false;
		}

		count = gObjGetItemCountInEquipment(Obj, 13, 40, 0);

		if (count != 0)
		{
			return false;
		}

		count = gObjGetItemCountInEquipment(Obj, 13, 41, 0);

		if (count != 0)
		{
			return false;
		}

		count = gObjGetItemCountInEquipment(Obj, 13, 42, 0);

		if (count != 0)
		{
			return false;
		}
	}

	if (lpItem->m_Type == ITEMGET(13, 169) || lpItem->m_Type == ITEMGET(13, 170))
	{
		if (CC_MAP_RANGE(Obj.MapNumber) || Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
		{
			return false;
		}

		if (gObjGetItemCountInEquipment(Obj, 13, 169, 0) > 0 ||
			gObjGetItemCountInEquipment(Obj, 13, 170, 0) > 0)
		{
			return false;
		}
	}

	if (g_PentagramSystem.IsPentagramItem(lpItem->m_Type))
	{
		BYTE btMainAttribute = lpItem->m_BonusSocketOption & 0x0F;

		if (btMainAttribute != 1 &&
			btMainAttribute != 2 &&
			btMainAttribute != 3 &&
			btMainAttribute != 4 &&
			btMainAttribute != 5)
		{
			return false;
		}
	}

	else if (lpItem->m_Type >= ITEMGET(14, 00))
	{
		return false;
	}

	return true;
}




BYTE gObjWerehouseRectCheck(CGameObject &Obj, int sx, int sy, int width, int height)
{
	int x, y;
	int blank = 0;

	if (sx + width > 8)
	{
		return -1;
	}

	if (sx + width < 0)
	{
		return -1;
	}

	if (!CheckOutOfWarehouse(Obj, sy, height))
	{
		return -1;
	}

	int xx, yy;

	

	for (y = 0; y < height; y++)
	{

		yy = sy + y;

		for (x = 0; x < width; x++)
		{
			xx = sx + x;

			if (WarehouseExtentCheck(xx, yy, 8, MAX_WAREHOUSE_H))
			{
				if (*(BYTE*)(Obj.WarehouseMap + (sy + y) * 8 + (sx + x)) != 255)
				{
					blank += 1;
					return -1;
				}
			}
			else
			{
				sLog->outBasic("error : %s %d", __FILE__, __LINE__);
				return -1;
			}
		}
	}

	if (blank == 0)
	{
		return sx + sy * 8;
	}
	return  -1;
}




BYTE gObjMapRectCheck(BYTE * lpMapBuf, int sx, int sy, int ex, int ey, int width, int height)
{
	int x, y;
	int blank = 0;

	if (sx + width > ex || sx + width < 0)
	{
		return -1;
	}

	if (sy + height > ey || sy + height < 0)
	{
		return -1;
	}

	int xx, yy;

	for (y = 0; y < height; y++)
	{
		yy = sy + y;

		for (x = 0; x < width; x++)
		{
			xx = sx + x;

			if (ExtentCheck(xx, yy, ex, ey) == 1)
			{
				if (*(BYTE*)(lpMapBuf + (sy + y)*ex + (sx + x)) != 255)
				{
					blank += 1;
					return -1;
				}
			}
			else
			{
				sLog->outBasic("error : %s %d", __FILE__, __LINE__);
				return -1;
			}
		}
	}

	if (blank == 0)
	{
		return sy * ex + sx;
	}
	return  -1;

	return -1;
}




void gObjWarehouseItemBoxSet(CGameObject &Obj, int itempos, int xl, int yl, BYTE set_byte)
{
	int itemposx = (itempos) % 8;
	int itemposy = (itempos) / 8;

	int xx, yy;

	if (!CheckOutOfWarehouse(Obj, itemposy, yl))
	{
		return;
	}

	for (int y = 0; y < yl; y++)
	{
		yy = itemposy + y;

		for (int x = 0; x < xl; x++)
		{
			xx = itemposx + x;

			if (WarehouseExtentCheck(xx, yy, 8, MAX_WAREHOUSE_H))
			{
				*(BYTE*)(Obj.WarehouseMap + (itemposy + y) * 8 + (itemposx + x)) = set_byte;
			}
			else
			{
				sLog->outBasic("error : %d/%d  %s %d", xx, yy, __FILE__, __LINE__);
				return;
			}
		}
	}
}




void gObjWarehouseItemSet(CGameObject &Obj, int itempos, BYTE set_byte)
{
	int width;
	int height;
	if (itempos < 0)
	{
		return;
	}
	else if (itempos >(WAREHOUSE_SIZE - 1))
	{
		return;
	}

	if (Obj.pntWarehouse[itempos]->GetSize((int&)width, (int &)height) == 0)
	{
		sLog->outBasic("error: Item does not exist %s %d", __FILE__, __LINE__);
		return;
	}

	gObjWarehouseItemBoxSet(Obj, itempos, width, height, set_byte);
}




void gObjChaosItemBoxSet(CGameObject &Obj, int itempos, int xl, int yl, BYTE set_byte)
{
	int itemposx = (itempos) % 8;
	int itemposy = (itempos) / 8;

	int xx, yy;

	for (int y = 0; y < yl; y++)
	{
		yy = itemposy + y;

		for (int x = 0; x < xl; x++)
		{
			xx = itemposx + x;

			if (ExtentCheck(xx, yy, 8, 4) == 1)
			{
				*(BYTE*)(Obj.ChaosBoxMap + (itemposy + y) * 8 + (itemposx + x)) = set_byte;
			}
			else
			{
				sLog->outBasic("error : %d/%d  %s %d", xx, yy, __FILE__, __LINE__);
				return;
			}
		}
	}
}




void gObjChaosItemSet(CGameObject &Obj, int itempos, BYTE set_byte)
{
	int width;
	int height;

	if (itempos < 0)
	{
		return;
	}

	else if (itempos >(CHAOS_BOX_SIZE - 1))
	{
		return;
	}

	if (Obj.pntChaosBox[itempos]->GetSize((int&)width, (int &)height) == 0)
	{
		sLog->outBasic("error: Item does not exist %s %d", __FILE__, __LINE__);
		return;
	}

	gObjChaosItemBoxSet(Obj, itempos, width, height, set_byte);
}




BYTE gObjChaosBoxInsertItemPos(CGameObject &Obj, CItemObject &item, int pos, int source)
{

	CGameObject lpObj;
	BYTE TempMap[CHAOS_BOX_MAP_SIZE];

	int w, h, iwidth, iheight;

	int blank, useClass;

	if (pos < 0 || pos >(CHAOS_BOX_SIZE - 1))
	{
		return -1;
	}

	useClass = 0;

	if (Obj.pntChaosBox[pos]->IsItem() == 1)
	{
		return -1;
	}

	if (item.IsItem() == 0)
	{
		return -1;
	}

	w = pos % 8;
	h = pos / 8;

	if (ExtentCheck(w, h, 8, 4) == 0)
	{
		return -1;
	}

	item.GetSize((int &)iwidth, (int &)iheight);

	if (source >= 0)
	{
		std::memcpy(TempMap, Obj.ChaosBoxMap, CHAOS_BOX_MAP_SIZE);
		gObjChaosItemBoxSet(Obj, source, iwidth, iheight, 255);
	}

	if (*(BYTE*)(Obj.ChaosBoxMap + h * 8 + w) != 255)
	{
		if (source >= 0)
		{
			std::memcpy(Obj.ChaosBoxMap, TempMap, CHAOS_BOX_MAP_SIZE);
		}
		return -1;
	}

	blank = gObjMapRectCheck(Obj.ChaosBoxMap, w, h, 8, 4, iwidth, iheight);

	if (blank == 255)
	{
		if (source >= 0)
		{
			std::memcpy(Obj.ChaosBoxMap, TempMap, CHAOS_BOX_MAP_SIZE);
		}
		return -1;
	}

	Obj.pntChaosBox[pos] = &item;
	return pos;
}




BYTE gObjWarehouseInsertItemPos(CGameObject &Obj, CItemObject &item, int pos, int source)
{
	CGameObject lpObj;
	BYTE TempMap[WAREHOUSE_SIZE];

	int w, h, iwidth, iheight;

	int blank, useClass;

	if (pos < 0 || pos >(WAREHOUSE_SIZE - 1))	//OK
	{
		return -1;
	}

	useClass = 0;

	if (Obj.pntWarehouse[pos]->IsItem() == 1)
	{
		return -1;
	}

	if (item.IsItem() == 0)
	{
		return -1;
	}

	w = pos % 8;
	h = pos / 8;

	if (!WarehouseExtentCheck(w, h, 8, MAX_WAREHOUSE_H))
	{
		return -1;
	}

	item.GetSize((int &)iwidth, (int &)iheight);

	if (source >= 0)
	{
		std::memcpy(TempMap, Obj.WarehouseMap, WAREHOUSE_SIZE);
		gObjWarehouseItemBoxSet(Obj, source, iwidth, iheight, 255);
	}

	if (*(BYTE*)(Obj.WarehouseMap + h * 8 + w) != 255)
	{
		if (source >= 0)
		{
			std::memcpy(Obj.WarehouseMap, TempMap, WAREHOUSE_SIZE);
		}
		return -1;
	}

	blank = gObjWerehouseRectCheck(Obj, w, h, iwidth, iheight);

	if (blank == 255)
	{
		if (source >= 0)
		{
			std::memcpy(Obj.WarehouseMap, TempMap, WAREHOUSE_SIZE);
		}
		return -1;
	}

	Obj.pntWarehouse[pos] = &item;
	return pos;
}




BYTE gObjInventoryInsertItemPos(CGameObject &Obj, CItemObject &item, int pos, BOOL RequestCheck)
{
	if (pos < 0 || pos > INVENTORY_SIZE)	//236 from 6.2
	{
		return -1;
	}

	CGameObject lpObj;
	BYTE TempInventoryMap[INVENTORY_MAP_SIZE];

	int w, h, iwidth, iheight;

	int blank, useClass = 0;

	lpObj = lpObj;

	if (Obj.pntInventory[pos]->IsItem() == 1)
	{
		return -1;
	}

	if (item.IsItem() == 0)
	{
		return -1;
	}

	if (pos < 12 || pos == 236)
	{
		useClass = item.IsClass(Obj.Class, Obj.m_PlayerData->ChangeUP);

		if (useClass == 0)
		{
			sLog->outBasic("error-L2: It is not his class");
			return -1;
		}

		if (RequestCheck != 0)
		{
			if (gObjIsItemPut(Obj, &item, pos) == 0)
			{
				return -1;
			}
		}
	}
	else
	{
		w = (pos - 12) % 8;
		h = (pos - 12) / 8;

		item.GetSize((int &)iwidth, (int &)iheight);

		bool bResult = CheckOutOfInventory(Obj, h, iheight);
		// ----
		if (!bResult)
		{
			if (bResult == -1)
			{
				return -2;
			}
			// ----
			return -1;
		}
		// ----
		if (!InventoryExtentCheck(w, h, 8, MAX_INVENTORY_H2))	//OK
		{
			return -1;
		}

		std::memcpy(TempInventoryMap, Obj.pInventoryMap, INVENTORY_MAP_SIZE);	//OK

		if (*(BYTE*)(Obj.pInventoryMap + h * 8 + w) != 255)
		{
			std::memcpy(Obj.pInventoryMap, TempInventoryMap, INVENTORY_MAP_SIZE);	//OK
			return -1;
		}

		blank = gObjInventoryRectCheck(Obj, w, h, iwidth, iheight);

		if (blank >= 254)
		{
			std::memcpy(Obj.pInventoryMap, TempInventoryMap, INVENTORY_MAP_SIZE);	//OK
			return -1;
		}
	}

	Obj.pntInventory[pos] = &item;

	if (pos > 11)
	{
		gObjInventoryItemSet(Obj, pos, 1);
	}

	if (pos < 12 || pos == 236)
	{
		CItemObject * lpItem = Obj.pntInventory[pos];

		if (lpItem->m_Option1)
		{
			int result = gObjWeaponMagicAdd(Obj, lpItem->m_Special[0], lpItem->m_Level);

			if (result >= 0)
			{
				gGameProtocol.GCMagicListOneSend(Obj, result, lpItem->m_Special[0], lpItem->m_Level, 0, 0);
			}
		}
	}
	return pos;
}

BOOL gObjInventorySearchSerialNumber(CGameObject &Obj, UINT64 serial)
{
	UINT64 s_num;
	int count = 0;

	if (g_ConfigRead.ItemSerialCheck == 0)
	{
		return true;
	}

	if (serial == 0 || serial == (UINT64)-1)
	{
		return true;
	}

	for (int n = 0; n < INVENTORY_SIZE; n++)
	{
		s_num = Obj.pntInventory[n]->GetNumber();

		if (s_num != 0 && s_num == serial && s_num != (UINT64)-1)
		{
			count++;
		}
	}

	if (count <= 1)
	{
		return true;
	}

	for (int n = 0; n < INVENTORY_SIZE; n++)
	{
		s_num = Obj.pntInventory[n]->GetNumber();

		if (s_num != 0 && s_num == serial && s_num != (UINT64)-1)
		{
			sLog->outBasic("error-L1: CopyItem Id[%s] Char[%s] Item[%s] InventoryPos[%d] serial[%I64d]", Obj.AccountID, Obj.Name, Obj.pntInventory[n]->GetName(), n, s_num);
			gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 15), Obj, 1);
			gObjUserKill(Obj);
		}
	}

	return false;
}




BOOL gObjWarehouseSearchSerialNumber(CGameObject &Obj, UINT64 sirial)
{
	UINT64 s_num;
	int count = 0;

	if (g_ConfigRead.ItemSerialCheck == 0)
	{
		return true;
	}

	if (sirial == 0 || sirial == (UINT64)-1)
	{
		return true;
	}

	for (int n = 0; n < WAREHOUSE_SIZE; n++)
	{
		s_num = Obj.pntWarehouse[n]->GetNumber();

		if (s_num != 0 && s_num == sirial && s_num != (UINT64)-1)
		{
			count++;
		}
	}

	if (count <= 1)
	{
		return true;
	}

	for (int n = 0; n < WAREHOUSE_SIZE; n++)
	{
		s_num = Obj.pntWarehouse[n]->GetNumber();

		if (s_num != 0 && s_num == sirial && s_num != (UINT64)-1)
		{
			sLog->outBasic("error-L1: CopyItem Id[%s] Char[%s] Item[%s] warehousepos[%d] serial[%I64d]", Obj.AccountID, Obj.Name, Obj.pntWarehouse[n]->GetName(), n, s_num);
			gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 15), Obj, 1);
			gObjUserKill(Obj);
		}
	}

	return false;
}




BYTE gObjInventoryMoveItem(CGameObject &Obj, BYTE source, BYTE target, int& durSsend, int& durTsend, BYTE sFlag, BYTE tFlag, BYTE* siteminfo)
{
	CGameObject lpObj;
	BYTE TempInventoryMap[225];
	int size = 225;
	int w, h;
	int iwidth, iheight;
	int blank;
	UINT64 s_num;

	CItemObject * sitem;
	CItemObject * titem;

	int bPersonalShopTrans = 0;
	int bSourceIsPShop = 0;

	durSsend = 0;
	durTsend = 0;

	if (Obj.m_bMapSvrMoveReq == true || Obj.m_bMapSvrMoveQuit == true || Obj.m_State == 32)
	{
		sLog->outError("[gObjInventoryMoveItem] Can't move item in inventory - MapServerMove");
		return -1;
	}

	int useClass = 0;
	// __try - TODO - work out why needed.
	{
		if (sFlag == 2 || tFlag == 2)
		{
			if (Obj.m_IfState->type != 6)
			{
				return -1;
			}

			if (Obj.m_IfState->state == 0)
			{
				return -1;
			}
		}

		switch (sFlag)
		{
		case 0:
			if ((source < 0 || source >(MAIN_INVENTORY_SIZE - 1)) && source != 236)
			{
				return -1;
			}

			if (Obj.pntInventory[source]->IsItem() == 0)
			{
				return -1;
			}

			sitem = Obj.pntInventory[source];

			if (Obj.MapNumber == MAP_INDEX_ICARUS)
			{
				if (source == 8)
				{
					if (sitem->m_Type == ITEMGET(13, 3))
					{
						if (Obj.pntInventory[7]->IsItem() == 0)
						{
							return -1;
						}
					}
					else if (sitem->m_Type == ITEMGET(13, 37))
					{
						if (Obj.pntInventory[7]->IsItem() == 0)
						{
							return -1;
						}
					}

					else if (sitem->m_Type == ITEMGET(13, 4)) //season4 add-on
					{
						if (Obj.pntInventory[7]->IsItem() == 0)
						{
							return -1;
						}
					}
				}
				else if (source == 7) //OK
				{
					if (Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type != ITEMGET(13, 3) &&
						//season4 add-on
						Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type != ITEMGET(13, 37) &&
						Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type != ITEMGET(13, 4))
					{
						return -1;
					}
				}
			}

			break;
		case 2:
			if (source < 0 || source >(WAREHOUSE_SIZE - 1))
			{
				return -1;
			}

			if (Obj.pntWarehouse[source]->IsItem() == 0)
			{
				return -1;
			}

			sitem = Obj.pntWarehouse[source];
			break;
		case 3:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 19:
		case 20:
			if (source < 0 || source >(CHAOS_BOX_SIZE - 1))
			{
				return -1;
			}

			if (Obj.pntChaosBox[source]->IsItem() == 0)
			{
				return -1;
			}

			sitem = Obj.pntChaosBox[source];
			break;
		case 4:
			if (Obj.m_bPShopOpen == 1)
			{
				return -1;
			}

			EnterCriticalSection(&Obj.m_critPShopTrade);

			if (Obj.m_bPShopTransaction == 1)
			{
				LeaveCriticalSection(&Obj.m_critPShopTrade);
				return -1;
			}

			Obj.m_bPShopTransaction = 1;
			bPersonalShopTrans = 1;
			bSourceIsPShop = 1;

			LeaveCriticalSection(&Obj.m_critPShopTrade);

			if (source < MAIN_INVENTORY_SIZE || source >(INVENTORY_SIZE - 1))
			{
				return -1;
			}

			if (Obj.pntInventory[source]->IsItem() == 0)
			{
				return -1;
			}

			sitem = Obj.pntInventory[source];

			break;
		case 17:
			if (source < 0 || source >(CHAOS_BOX_SIZE - 1))
			{
				return -1;
			}

			if (Obj.m_PlayerData->pPentagramMixBox[source]->IsItem() == 0)
			{
				return -1;
			}

			sitem = Obj.m_PlayerData->pPentagramMixBox[source];
			break;
		default: return -1;
		}

		BYTE level;
		BYTE op1;
		BYTE op2;
		BYTE op3;
		BYTE dur;
		int type;

		BufferItemtoConvert3(siteminfo, type, level, op1, op2, op3, dur);

		if (sitem->m_Type != type || sitem->m_Level != level || sitem->m_Option1 != op1 || sitem->m_Option2 != op2 || sitem->m_Option3 != op3)
		{
			return -1;
		}

		switch (tFlag)
		{
		case 0:
			if ((target < 0 || target >(MAIN_INVENTORY_SIZE - 1)) && target != 236)
			{
				return -1;
			}

			titem = Obj.pntInventory[target];
			break;
		case 2:
			if (target < 0 || target >(WAREHOUSE_SIZE - 1))
			{
				return -1;
			}

			if (sitem->IsPeriodItem() == TRUE)
			{
				return -1;
			}

			titem = Obj.pntWarehouse[target];
			break;
		case 3:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 19:
		case 20:
			if (target < 0 || target >(CHAOS_BOX_SIZE - 1))
			{
				return -1;
			}

			titem = Obj.pntChaosBox[target];

			if (sFlag == 0)
			{
				if (tFlag == 5)
				{
					if (sitem->m_Type == ITEMGET(12, 15) ||
						sitem->m_Type == ITEMGET(14, 13) ||
						sitem->m_Type == ITEMGET(14, 14) ||
						sitem->m_Type == ITEMGET(14, 16) ||
						sitem->m_Type == ITEMGET(14, 22) ||
						sitem->m_Type == ITEMGET(13, 31) ||
						sitem->m_Type == ITEMGET(14, 53) ||
						//Season4.5 add-on
						sitem->m_Type == ITEMGET(13, 83) ||
						sitem->m_Type == ITEMGET(13, 84) ||
						sitem->m_Type == ITEMGET(13, 85) ||
						sitem->m_Type == ITEMGET(13, 86) ||
						sitem->m_Type == ITEMGET(13, 87) ||
						sitem->m_Type == ITEMGET(13, 88) ||
						sitem->m_Type == ITEMGET(13, 89) ||
						sitem->m_Type == ITEMGET(13, 90) ||
						sitem->m_Type == ITEMGET(13, 91) ||
						sitem->m_Type == ITEMGET(13, 92) ||
						//6.2
						sitem->m_Type == ITEMGET(14, 103) ||
						sitem->m_Type == ITEMGET(14, 104) ||
						sitem->m_Type == ITEMGET(14, 105) ||
						sitem->m_Type == ITEMGET(14, 106) ||
						sitem->m_Type == ITEMGET(14, 107) ||
						sitem->m_Type == ITEMGET(14, 108) ||
						sitem->m_Type == ITEMGET(14, 121) ||
						sitem->m_Type == ITEMGET(14, 122) ||
						sitem->m_Type == ITEMGET(14, 112) ||
						sitem->m_Type == ITEMGET(14, 113))
					{

					}
					else
					{
						return -1;
					}
				}
				else if (tFlag == 6)
				{
					if (g_kJewelOfHarmonySystem.IsJewelOfHarmonyOriginal(sitem->m_Type) == 0)
					{
						return -1;
					}
				}
				else if (tFlag == 7)
				{
					if (g_kJewelOfHarmonySystem.IsEnableToMakeSmeltingStoneItem(sitem) == 0)
					{
						gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 278), Obj, 1);
						return -1;
					}

					if (IsJumpingEventItem(sitem->m_Type) == TRUE)
					{
						gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 278), Obj, 1);
						return -1;
					}
				}
				else if (tFlag == 8)
				{
					if (g_kJewelOfHarmonySystem.IsStrengthenByJewelOfHarmony(sitem) == 0)
					{
						return -1;
					}
				}
				else if (tFlag == 9)
				{
					if (sitem->m_Type != ITEMGET(14, 54) && sitem->m_Type != ITEMGET(14, 92) && sitem->m_Type != ITEMGET(14, 93) && sitem->m_Type != ITEMGET(14, 95))
					{
						return -1;
					}
				}
				else if (tFlag == 10)
				{
					if (sitem->m_Type != ITEMGET(14, 88) && sitem->m_Type != ITEMGET(14, 89) && sitem->m_Type != ITEMGET(14, 90))
					{
						return -1;
					}
				}
				else if (tFlag == 11)
				{

				}
				else if (tFlag == 12)
				{

				}
				else if (tFlag == 13)
				{

				}
				else if (tFlag == 14)
				{

				}
				else if (tFlag == 15)
				{
					for (int i = 0; i < CHAOS_BOX_SIZE; i++)
					{
						if (Obj.pntChaosBox[i]->IsItem())
						{
							return -1;
						}
					}
					if (sitem->m_Type == ITEMGET(13, 137) || sitem->m_Type == ITEMGET(13, 142))
					{
						if (Obj.Class == CLASS_MAGUMSA)
						{
							MsgOutput(Obj, Lang.GetText(0, 608));
							return -1;
						}
					}
					if (sitem->m_Type == ITEMGET(13, 143) || sitem->m_Type == ITEMGET(13, 138))
					{
						if (Obj.Class == CLASS_RAGEFIGHTER)
						{
							MsgOutput(Obj, Lang.GetText(0, 609));
							return -1;
						}
					}
				}
				else if (tFlag == 16)
				{
					for (int i = 0; i < CHAOS_BOX_SIZE; i++)
					{
						if (Obj.pntChaosBox[i]->IsItem())
						{
							return -1;
						}
					}
				}

				else if (tFlag == 19)
				{

				}

				else if (tFlag == 20)
				{
					if (sitem->m_Type != ITEMGET(14, 21) || sitem->m_Level != 3)
					{
						return -1;
					}
				}

				else
				{
					if (sitem->m_Level < 4 && sitem->m_Option3 * 4 < 4)
					{
						if (sitem->m_Type == ITEMGET(12, 15) || sitem->m_Type == ITEMGET(14, 13) || sitem->m_Type == ITEMGET(14, 14) || sitem->m_Type == ITEMGET(14, 16) || sitem->m_Type == ITEMGET(14, 22) || sitem->m_Type == ITEMGET(13, 14))
						{

						}
						else if (sitem->m_Type >= ITEMGET(12, 0) && sitem->m_Type <= ITEMGET(12, 2))
						{

						}
						else if (sitem->m_Type >= ITEMGET(12, 36) && sitem->m_Type <= ITEMGET(12, 43))
						{

						}
						else if (sitem->m_Type >= ITEMGET(12, 262) && sitem->m_Type <= ITEMGET(12, 265))
						{

						}
						else if (sitem->m_Type == ITEMGET(12, 49) || sitem->m_Type <= ITEMGET(12, 50))
						{

						}
						else if (sitem->m_Type == ITEMGET(14, 17) || sitem->m_Type == ITEMGET(14, 18))
						{

						}
						else if (sitem->m_Type == ITEMGET(13, 49) || sitem->m_Type == ITEMGET(13, 50))
						{

						}
						else if (sitem->m_Type == ITEMGET(13, 2) && sitem->m_Durability == 255.0f)
						{

						}
						else if (sitem->m_Type == ITEMGET(13, 16) || sitem->m_Type == ITEMGET(13, 17))
						{

						}

						else if (sitem->m_Type == ITEMGET(12, 30) || sitem->m_Type == ITEMGET(12, 31))
						{

						}

						else if (sitem->m_Type == ITEMGET(14, 31))
						{

						}

						else if (sitem->m_Type == ITEMGET(12, 26))
						{

						}

						else if (sitem->m_Type == ITEMGET(12, 136))
						{

						}

						else if (sitem->m_Type == ITEMGET(13, 276))
						{

						}

						else if (sitem->m_Type == ITEMGET(13, 277))
						{

						}

						else if (
							sitem->m_Type == ITEMGET(12, 30) ||
							sitem->m_Type == ITEMGET(12, 31) ||
							sitem->m_Type == ITEMGET(13, 32) ||
							sitem->m_Type == ITEMGET(13, 33) ||
							sitem->m_Type == ITEMGET(13, 34) ||
							sitem->m_Type == ITEMGET(13, 35) ||
							sitem->m_Type == ITEMGET(13, 36) ||
							sitem->m_Type == ITEMGET(13, 37) ||
							sitem->m_Type == ITEMGET(13, 51) ||
							sitem->m_Type == ITEMGET(13, 52) ||
							sitem->m_Type == ITEMGET(13, 53) ||
							sitem->m_Type == ITEMGET(12, 140) ||
							sitem->m_Type == ITEMGET(12, 137) ||
							sitem->m_Type == ITEMGET(13, 147)
							)
						{

						}
						else if (sitem->m_Type == ITEMGET(14, 3) || sitem->m_Type == ITEMGET(14, 38) || sitem->m_Type == ITEMGET(14, 39))
						{

						}
						else if (g_kJewelOfHarmonySystem.IsJewelOfHarmonyPurity(sitem->m_Type) == 1)
						{

						}
						else if (sitem->m_Type == ITEMGET(12, 30))
						{

						}
						else if (sitem->m_Type == ITEMGET(12, 31))
						{

						}
						else if (sitem->m_Type == ITEMGET(13, 51))
						{

						}
						else if (sitem->m_Type == ITEMGET(13, 52))
						{

						}
						else if (sitem->m_Type == ITEMGET(13, 53))
						{

						}
						else if (sitem->m_Type >= ITEMGET(13, 83) && sitem->m_Type <= ITEMGET(13, 92))
						{

						}
						else if (sitem->m_Type == ITEMGET(12, 140))
						{

						}
						else if (sitem->m_Type == ITEMGET(12, 137))
						{

						}
						else if (sitem->m_Type == ITEMGET(14, 31))
						{

						}
						else if (sitem->m_Type == ITEMGET(14, 53))
						{

						}
						else if (sitem->m_Type == ITEMGET(14, 96))
						{

						}
						else if (sitem->m_Type >= ITEMGET(14, 112) && sitem->m_Type <= ITEMGET(14, 113))
						{

						}
						else if (sitem->m_Type >= ITEMGET(14, 121) && sitem->m_Type <= ITEMGET(14, 122))
						{

						}
						else if (sitem->m_Type >= ITEMGET(14, 103) && sitem->m_Type <= ITEMGET(14, 108))
						{

						}

						else if (sitem->m_Type >= ITEMGET(14, 176) && sitem->m_Type <= ITEMGET(14, 180))
						{

						}

						else if (sitem->m_Type >= ITEMGET(14, 181) && sitem->m_Type <= ITEMGET(14, 190))
						{

						}

						else if (sitem->m_Type >= ITEMGET(14, 191) && sitem->m_Type <= ITEMGET(14, 202))
						{

						}

						else if (sitem->m_Type >= ITEMGET(14, 210) && sitem->m_Type <= ITEMGET(14, 212))
						{

						}

						else if (sitem->m_Type >= ITEMGET(13, 351) && sitem->m_Type <= ITEMGET(13, 402))
						{

						}

						else if (sitem->m_Type == ITEMGET(13, 407)) //Ancient Hero Soul
						{
						}
						else if (sitem->m_Type == ITEMGET(14, 341)) //////Shard Stone
						{
						}
						else if (sitem->m_Type == ITEMGET(14, 386)) //////Anvil
						{

						}
						else if (sitem->m_Type == ITEMGET(14, 342)) //////Steal
						{

						}
						else if (sitem->m_Type == ITEMGET(14, 343)) //////Hammer
						{

						}
						else if (sitem->m_Type == ITEMGET(14, 389)) //////Anvil
						{
							//
						}
						else
							//
						{
							return -1;
						}
					}
					else if (sitem->m_Type == ITEMGET(14, 11))
					{
						return -1;
					}
					else if (IsJumpingEventItem(sitem->m_Type) == TRUE)
					{
						return -1;
					}
				}
			}
			break;
		case 4:
			if (gDoPShopOpen == 0)
			{
				return -1;
			}

			if (Obj.m_bPShopOpen == 1)
			{
				return -1;
			}

			if (sitem->m_Type == ITEMGET(14, 11) && sitem->m_Level == 13)
			{
				return -1;
			}

			if (g_kJewelOfHarmonySystem.IsStrengthenByJewelOfHarmony(sitem) == 1)
			{
				gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 268), Obj, 1);
				return -1;
			}

			if (IsJumpingEventItem(sitem->m_Type) == 1)
			{
				return -1;
			}

			if (IsPersonalStoreItem(sitem->m_Type))
			{
				if (sitem->m_Type == ITEMGET(12, 26) && sitem->m_Level)
				{
					return -1;
				}
			}

			else
			{
				if (sitem->m_Type != ITEMGET(14, 21) || sitem->m_Level != 3)
				{
					return -1;
				}
			}

			if (g_ConfigRead.data.common.StoreExCItemObjectLimit == FALSE)
			{
				if (sitem->m_NewOption == 63)
				{
					return -1;
				}
			}

			if (sitem->m_Type == ITEMGET(13, 38))
			{
				gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 286), Obj, 1);
				return -1;
			}

			if (sitem->m_Type == ITEMGET(13, 39))
			{
				return -1;
			}

			EnterCriticalSection(&Obj.m_critPShopTrade);

			if (Obj.m_bPShopTransaction == 1)
			{
				if (bSourceIsPShop == 1)
				{
					Obj.m_bPShopTransaction = 1;
					bPersonalShopTrans = 1;
				}
				else
				{
					sLog->outBasic("[PShop] [%s][%s] PShop Item Move Request Failed : Already Trade With Other", Obj.AccountID, Obj.Name);
					LeaveCriticalSection(&Obj.m_critPShopTrade);
					return -1;
				}
			}
			else
			{
				Obj.m_bPShopTransaction = 1;
				bPersonalShopTrans = 1;
			}

			LeaveCriticalSection(&Obj.m_critPShopTrade);

			if (target < MAIN_INVENTORY_SIZE || target >(INVENTORY_SIZE - 1))
			{
				return -1;
			}

			titem = Obj.pntInventory[target];

			break;
		case 17:
			if (target > CHAOS_BOX_SIZE - 1)
			{
				return -1;
			}

			titem = Obj.m_PlayerData->pPentagramMixBox[target];
			break;
		default: return -1;
		}

		s_num = sitem->GetNumber();

		if (gObjCheckSerial0ItemList(sitem))
		{
			MsgOutput(Obj, Lang.GetText(0, 259));
			return -1;
		}

		if (gObjInventorySearchSerialNumber(Obj, s_num) == 0)
		{
			return -1;
		}

		if (gObjWarehouseSearchSerialNumber(Obj, s_num) == 0)
		{
			return -1;
		}

		if (titem->IsItem() == 1)
		{
			if (sFlag != 0)
			{
				return -1;
			}

			if (sitem->m_Type != titem->m_Type)
			{
				return -1;
			}

			if (sitem->m_Level != titem->m_Level)
			{
				return -1;
			}

			int max_count = IsOverlapItem(titem->m_Type);

			if (max_count == 0)
			{
				return -1;
			}

			int t_dur = titem->m_Durability;

			if (t_dur < max_count)
			{
				int dif_dur = max_count - int(titem->m_Durability);

				if (dif_dur > sitem->m_Durability)
				{
					dif_dur = sitem->m_Durability;
				}

				titem->m_Durability += dif_dur;
				sitem->m_Durability -= dif_dur;

				if (titem->m_Type == ITEMGET(14, 29) && max_count <= titem->m_Durability)
				{
					titem->m_Durability -= max_count;

					if (titem->m_Durability == 0)
					{
						gObjInventoryItemSet(Obj, target, 255);
						Obj.pntInventory[target]->Clear();
						gGameProtocol.GCInventoryItemDeleteSend(Obj, target, 1);
						durTsend = 0;
					}
					else
					{
						durTsend = 1;
					}

					ItemCreate(Obj, 235, Obj.X, Obj.Y, ItemGetNumberMake(14, 28), sitem->m_Level, 0, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
				}

				if ((sitem->m_Type == ITEMGET(14, 70) || sitem->m_Type == ITEMGET(14, 71) || sitem->m_Type == ITEMGET(14, 94) || sitem->m_Type == ITEMGET(14, 133)) && titem->m_Durability < 50.0f)
				{
					max_count = 50;
				}

				if (titem->m_Type == ITEMGET(14, 110) && max_count <= titem->m_Durability)
				{
					titem->m_Durability -= max_count;

					if (titem->m_Durability == 0)
					{
						gObjInventoryItemSet(Obj, target, 255);
						Obj.pntInventory[target]->Clear();
						gGameProtocol.GCInventoryItemDeleteSend(Obj, target, 1);
						durTsend = 0;
					}
					else
					{
						durTsend = 1;
					}

					ItemCreate(Obj, 235, Obj.X, Obj.Y, ItemGetNumberMake(14, 111), sitem->m_Level, 0, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
				}

				if (titem->m_Type == ITEMGET(12, 144) && max_count <= titem->m_Durability && titem->m_BonusSocketOption == sitem->m_BonusSocketOption)
				{
					titem->m_Durability -= max_count;

					BYTE btMainAttribute = titem->m_BonusSocketOption;

					if (titem->m_Durability == 0)
					{
						gObjInventoryDeleteItem(Obj, target);
						gGameProtocol.GCInventoryItemDeleteSend(Obj, target, 1);
						durTsend = 0;
					}
					else
					{
						durTsend = 1;
					}

					ItemCreate(Obj, 235, Obj.X, Obj.Y, ItemGetNumberMake(12, 145), sitem->m_Level, 0, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, sitem->m_BonusSocketOption);
				}

				if (titem->m_Type == ITEMGET(13, 145) && max_count <= titem->m_Durability)
				{
					titem->m_Durability -= max_count;

					if (titem->m_Durability == 0)
					{
						gObjInventoryItemSet(Obj, target, 255);
						Obj.pntInventory[target]->Clear();
						gGameProtocol.GCInventoryItemDeleteSend(Obj, target, 1);
						durTsend = 0;
					}
					else
					{
						durTsend = 1;
					}

					ItemCreate(Obj, 235, Obj.X, Obj.Y, ItemGetNumberMake(13, 146), sitem->m_Level, 0, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
				}

				if (titem->m_Type == ITEMGET(14, 290) && max_count <= titem->m_Durability)
				{
					titem->m_Durability -= max_count;

					if (titem->m_Durability == 0)
					{
						gObjInventoryItemSet(Obj, target, 255);
						Obj.pntInventory[target]->Clear();
						gGameProtocol.GCInventoryItemDeleteSend(Obj, target, 1);
						durTsend = 0;
					}
					else
					{
						durTsend = 1;
					}

					ItemCreate(Obj, 235, Obj.X, Obj.Y, ItemGetNumberMake(14, 293), sitem->m_Level, 0, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
				}

				if (titem->m_Type == ITEMGET(14, 101) && max_count <= titem->m_Durability)
				{
					titem->m_Durability -= max_count;

					if (titem->m_Durability == 0)
					{
						gObjInventoryItemSet(Obj, target, 255);
						Obj.pntInventory[target]->Clear();
						gGameProtocol.GCInventoryItemDeleteSend(Obj, target, 1);
						durTsend = 0;
					}
					else
					{
						durTsend = 1;
					}

					ItemCreate(Obj, 235, Obj.X, Obj.Y, ItemGetNumberMake(14, 102), sitem->m_Level, 0, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
				}

				if (sitem->m_Durability > 0)
				{
					durSsend = 1;
					durTsend = 1;
				}
				else
				{
					switch (sFlag)
					{
					case 0:
						gObjInventoryDeleteItem(Obj, source);
						gGameProtocol.GCInventoryItemDeleteSend(Obj, source, 1);
						durTsend = 1;
					default:	return 0;
					}
				}
			}
			return -1;
		}

		if (IT_MAP_RANGE(Obj.MapNumber) == TRUE)
		{
			if (Obj.pntInventory[source]->m_Type == ITEMGET(14, 223))
			{
				g_IT_Event.SetRelicsInventoryPos(Obj.MapNumber, Obj, target);
				sLog->outBasic("[ ITR ][gObjInventoryMoveItem]Name:%s,Index:%d,target:%d", Obj.Name, Obj.m_Index, target);
			}
		}

		switch (tFlag)
		{
		case 0:
		case 4:
			if (target < INVETORY_WEAR_SIZE || target == 236)
			{
				if (Obj.MapNumber == MAP_INDEX_ICARUS)
				{
					if (target == 10)
					{
						if (Obj.pntInventory[source]->m_Type == ITEMGET(13, 10))
						{
							return -1;
						}
					}
					else if (target == 11)
					{
						if (Obj.pntInventory[source]->m_Type == ITEMGET(13, 10))
						{
							return -1;
						}
					}
					if (target == 10)
					{
						if (Obj.pntInventory[source]->m_Type == ITEMGET(13, 40))
						{
							return -1;
						}
					}
					else if (target == 11)
					{
						if (Obj.pntInventory[source]->m_Type == ITEMGET(13, 40))
						{
							return -1;
						}
					}
				}

				if (target < INVETORY_WEAR_SIZE || target == 236)
				{
					if (Obj.MapNumber == MAP_INDEX_ICARUS || Obj.MapNumber == MAP_INDEX_KANTURU_BOSS)
					{
						if (target == 10)
						{
							if (Obj.pntInventory[source]->m_Type == ITEMGET(13, 39) || Obj.pntInventory[source]->m_Type == ITEMGET(13, 40) ||
								Obj.pntInventory[source]->m_Type == ITEMGET(13, 10))
							{
								return -1;
							}
						}
						else if (target == 11)
						{
							if (Obj.pntInventory[source]->m_Type == ITEMGET(13, 39) || Obj.pntInventory[source]->m_Type == ITEMGET(13, 40) ||
								Obj.pntInventory[source]->m_Type == ITEMGET(13, 10))
							{
								return -1;
							}
						}
					}
				}

				if (g_MineSystem.IsPickax(Obj.pntInventory[source]->m_Type) == TRUE && Obj.m_bPShopOpen)
					return -1;

				useClass = sitem->IsClass(Obj.Class, Obj.m_PlayerData->ChangeUP);

				if (useClass == 0)
					return -1;

				if (gObjIsItemPut(Obj, sitem, target) == 0)
					return -1;

				if (!CheckCanWearResetItem(Obj, Obj.pntInventory[source]->m_Type))
					return -1;

			}
			else
			{
				if (target >= MAIN_INVENTORY_SIZE && target != 236)
				{
					if (Obj.pntInventory[source]->m_Type == ITEMGET(12, 26))
					{
						if (Obj.pntInventory[source]->m_Level == 1 ||
							Obj.pntInventory[source]->m_Level == 2 ||
							Obj.pntInventory[source]->m_Level == 3 ||
							Obj.pntInventory[source]->m_Level == 4 ||
							Obj.pntInventory[source]->m_Level == 5)
						{
							return -1;
						}
					}
				}

				w = (target - INVETORY_WEAR_SIZE) % 8;
				h = (target - INVETORY_WEAR_SIZE) / 8;

				if (!InventoryExtentCheck(w, h, 8, MAX_INVENTORY_H2))
				{
					return -1;
				}

				sitem->GetSize((int &)iwidth, (int &)iheight);
				std::memcpy(TempInventoryMap, Obj.pInventoryMap, size);

				switch (sFlag)
				{
				case 0:
				case 4:
					if (source > (INVETORY_WEAR_SIZE - 1) && source != 236)
					{
						gObjInventoryItemBoxSet(Obj, source, iwidth, iheight, 255);
					}
					break;
				default: break;
				}

				if (*(BYTE*)(Obj.pInventoryMap + h * 8 + w) != 255)
				{
					std::memcpy(Obj.pInventoryMap, TempInventoryMap, size);
					return -1;
				}

				blank = gObjInventoryRectCheck(Obj, w, h, iwidth, iheight);

				if (blank >= 254)
				{
					std::memcpy(Obj.pInventoryMap, TempInventoryMap, size);
					return -1;
				}
			}

			s_num = sitem->GetNumber();
			titem = sitem;
			BOOL bSendEquip;

			switch (sFlag)
			{
			case 0:
			case 4:
				Obj.pntInventory[target] = Obj.pntInventory[source];

				if (source < 204 || source == 236 || target >= 204 && target != 236)
				{
					if (source >= 204 && source != 236 && target >= 204 && target != 236)
						g_PersonalStore.GDMovePShopItem(Obj, source, target);
				}

				else
				{
					g_PersonalStore.GDDelPShopItemValue(Obj, source);
				}

				bSendEquip = FALSE;

				if ((source == 11 || source == 10)
					&& (sitem->m_Type == ITEMGET(13, 169) || sitem->m_Type == ITEMGET(13, 170))
					|| (target == 11 || target == 10)
					&& (titem->m_Type == ITEMGET(13, 169) || titem->m_Type == ITEMGET(13, 170)))
					bSendEquip = TRUE;

				gObjInventoryDeleteItem(Obj, source);

				if (bSendEquip)
				{
					gGameProtocol.GCEquipmentSend(Obj);
				}

				gObjSetInventoryEquipment(Obj, target);

				if (target > 12 && g_LuckyItemManager.IsLuckyItemEquipment(Obj.pntInventory[target]->m_Type))
				{
					CItemObject * lpItem = Obj.pntInventory[target];
					g_LuckyItemManager.GDReqLuckyItemInsert(lpItem, Obj.m_Index);
				}
				break;
			case 2:
				if (g_PentagramSystem.IsPentagramItem(*Obj.pntWarehouse[source]) == true)
				{
					g_PentagramSystem.SwitchPentagramJewel(Obj, Obj.pntWarehouse[source], 0);
					g_PentagramSystem.GCPentagramJewelInfo(Obj, 0);
					g_PentagramSystem.GCPentagramJewelInfo(Obj, 1);

					CItemObject Item;
					std::memcpy(&Item, &Obj.pntWarehouse[source], sizeof(Item));
					ItemByteConvert(siteminfo, Item);
				}
				Obj.pntInventory[target] = Obj.pntWarehouse[source];
				gObjWarehouseDeleteItem(Obj, source);

				if (target > 12 && g_LuckyItemManager.IsLuckyItemEquipment(Obj.pntInventory[target]->m_Type))
				{
					CItemObject * lpItem = Obj.pntInventory[target];
					g_LuckyItemManager.GDReqLuckyItemInsert(lpItem, Obj.m_Index);
				}
				break;
				break;
			case 17:
				Obj.pntInventory[target] = Obj.m_PlayerData->pPentagramMixBox[source];
				gObjPentagramMixBoxDeleteItem(Obj, source);
				break;
			case 3:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
			case 16:
			case 19:
			case 20:
				Obj.pntInventory[target] = Obj.pntChaosBox[source];
				gObjChaosBoxDeleteItem(Obj, source);
				break;
			default: break;
			}

			if (target > (INVETORY_WEAR_SIZE - 1) && target != 236)
			{
				gObjInventoryItemSet(Obj, target, 1);
			}

			if (target < INVETORY_WEAR_SIZE || target == 236)
			{
				CItemObject * lpItem = Obj.pntInventory[target];
				if (lpItem->m_Option1)
				{
					int s_pos = gObjWeaponMagicAdd(Obj, lpItem->m_Special[0], lpItem->m_Level);
					if (s_pos >= 0)
					{
						gGameProtocol.GCMagicListOneSend(Obj, s_pos, lpItem->m_Special[0], lpItem->m_Level, 0, 0);
					}
				}
			}
			break;
		case 2:
			BYTE res_1;
			switch (sFlag)
			{
			case 0:
				if (Obj.pntInventory[source]->m_Type == ITEMGET(13, 20) || Obj.pntInventory[source]->m_Type == ITEMGET(13, 107))
				{
					if (Obj.pntInventory[source]->m_Level == 0 ||
						Obj.pntInventory[source]->m_Level == 1)
					{
						return -1;
					}
				}

				if (Obj.pntInventory[source]->m_Type == ITEMGET(14, 11))
				{
					if (Obj.pntInventory[source]->m_Level == 13)
					{
						return -1;
					}
				}

				if (Obj.pntInventory[source]->m_Type == ITEMGET(12, 26))
				{
					if (Obj.pntInventory[source]->m_Level == 1 ||
						Obj.pntInventory[source]->m_Level == 2 ||
						Obj.pntInventory[source]->m_Level == 3 ||
						Obj.pntInventory[source]->m_Level == 4 ||
						Obj.pntInventory[source]->m_Level == 5)
					{
						return -1;
					}
				}

				if (Obj.pntInventory[source]->m_Type == ITEMGET(13, 39))
				{
					return -1;
				}

				if (Obj.pntInventory[source]->m_Type == ITEMGET(13, 42)) //Season 2.5 add-on
				{
					return -1;
				}

				if (Obj.pntInventory[source]->m_QuestItem && !g_QuestExpManager.IsQuestItemAtt(Obj.pntInventory[source]->m_Type, 4)) //Season 2.5 add-on
				{
					return -1;
				}

				if (Obj.pntInventory[source]->m_Type == ITEMGET(13, 70)) //Season 4 add-on
				{
					if (Obj.pntInventory[source]->m_Durability == 1.0f) //??
					{
						return -1;
					}
				}

				if (Obj.pntInventory[source]->IsPeriodItem())
				{
					return -1;
				}

				if (!IsStoreWarehouseItem(Obj.pntInventory[source]->m_Type) && (Obj.pntInventory[source]->m_Type != ITEMGET(14, 21) || Obj.pntInventory[source]->m_Level != 3))
				{
					return -1;
				}

				if (Obj.pntInventory[source]->m_Type == ITEMGET(14, 163))
				{
					return -1;
				}

				res_1 = gObjWarehouseInsertItemPos(Obj, *Obj.pntInventory[source], target, -1);
				break;
			case 2:
				res_1 = gObjWarehouseInsertItemPos(Obj, *Obj.pntWarehouse[source], target, source);
				break;
			default: break;
			}

			if (res_1 == 255)
			{
				return -1;
			}

			switch (sFlag)
			{
			case 0:
				if (g_PentagramSystem.IsPentagramItem(*Obj.pntInventory[source]) == true)
				{
					g_PentagramSystem.SwitchPentagramJewel(Obj, Obj.pntInventory[source], 1);
					g_PentagramSystem.GCPentagramJewelInfo(Obj, 0);
					g_PentagramSystem.GCPentagramJewelInfo(Obj, 1);

					CItemObject Item;
					std::memcpy(&Item, &Obj.pntInventory[source], sizeof(Item));
					ItemByteConvert(siteminfo, Item);
				}
				Obj.pntWarehouse[target] = Obj.pntInventory[source];
				gObjInventoryDeleteItem(Obj, source);
				break;
			case 2:
				Obj.pntWarehouse[target] = Obj.pntWarehouse[source];
				gObjWarehouseDeleteItem(Obj, source);
				break;
			default: break;
			}

			gObjWarehouseItemSet(Obj, target, 1);

			if (sFlag == 0)
			{
				if (source < INVETORY_WEAR_SIZE || source == 236)
				{
					if (source == 10 || source == 11)
					{
						if (Obj.pntWarehouse[target]->m_Type == ITEMGET(13, 10))
						{
							gObjUseSkill.SkillChangeUse(Obj);
						}
						if (Obj.pntWarehouse[target]->m_Type == ITEMGET(13, 40))
						{
							gObjUseSkill.SkillChangeUse(Obj);
						}
						if (Obj.pntWarehouse[target]->m_Type == ITEMGET(13, 41))
						{
							gObjUseSkill.SkillChangeUse(Obj);
						}
						if (Obj.pntWarehouse[target]->m_Type == ITEMGET(13, 42))
						{
							gObjUseSkill.SkillChangeUse(Obj);
						}
						if (Obj.pntWarehouse[target]->m_Type == ITEMGET(13, 68))
						{
							gObjUseSkill.SkillChangeUse(Obj);
						}
						if (Obj.pntWarehouse[target]->m_Type == ITEMGET(13, 76))
						{
							gObjUseSkill.SkillChangeUse(Obj);
						}
						if (Obj.pntWarehouse[target]->m_Type == ITEMGET(13, 77))
						{
							gObjUseSkill.SkillChangeUse(Obj);
						}
						if (Obj.pntWarehouse[target]->m_Type == ITEMGET(13, 78))
						{
							gObjUseSkill.SkillChangeUse(Obj);
						}
						if (Obj.pntWarehouse[target]->m_Type == ITEMGET(13, 122))
						{
							gObjUseSkill.SkillChangeUse(Obj);
						}
						if (Obj.pntWarehouse[target]->m_Type == ITEMGET(13, 163))
						{
							gObjUseSkill.SkillChangeUse(Obj);
						}
						if (Obj.pntWarehouse[target]->m_Type == ITEMGET(13, 164))
						{
							gObjUseSkill.SkillChangeUse(Obj);
						}
						if (Obj.pntWarehouse[target]->m_Type == ITEMGET(13, 165))
						{
							gObjUseSkill.SkillChangeUse(Obj);
						}
						if (Obj.pntWarehouse[target]->m_Type == ITEMGET(13, 268))
						{
							gObjUseSkill.SkillChangeUse(Obj);
						}
					}

					gObjMakePreviewCharSet(Obj);
					gGameProtocol.GCEquipmentChange(Obj, source);
				}
			}
			return 2;
		case 3:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 19:
		case 20:
			BYTE res_2;
			switch (sFlag)
			{
			case 0:
				res_2 = gObjChaosBoxInsertItemPos(Obj, *Obj.pntInventory[source], target, -1);
				break;
			case 3:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
			case 16:
			case 19:
			case 20:
				res_2 = gObjChaosBoxInsertItemPos(Obj, *Obj.pntChaosBox[source], target, source);
				break;
			default: break;
			}

			if (res_2 == 255)
			{
				return -1;
			}

			switch (sFlag)
			{
			case 0:
				Obj.pntChaosBox[target] = Obj.pntInventory[source];
				gObjInventoryDeleteItem(Obj, source);
				break;
			case 3:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
			case 16:
			case 19:
			case 20:
				Obj.pntChaosBox[target] = Obj.pntChaosBox[source];
				gObjChaosBoxDeleteItem(Obj, source);
				break;
			default: break;
			}

			gObjChaosItemSet(Obj, target, 1);
			gGameProtocol.GCChaosMachinePriceSend(Obj);
			if (sFlag == 0)
			{
				if (source < INVETORY_WEAR_SIZE || source == 236)
				{
					if (source == 10 || source == 11)
					{
						if (Obj.pntChaosBox[target]->m_Type == ITEMGET(13, 10))
						{
							gObjUseSkill.SkillChangeUse(Obj);
						}

						if (Obj.pntChaosBox[target]->m_Type == ITEMGET(13, 39))
						{
							gObjUseSkill.SkillChangeUse(Obj);
						}

						if (Obj.pntChaosBox[target]->m_Type == ITEMGET(13, 40))
						{
							gObjUseSkill.SkillChangeUse(Obj);
						}

						if (Obj.pntChaosBox[target]->m_Type == ITEMGET(13, 41))
						{
							gObjUseSkill.SkillChangeUse(Obj);
						}

						if (Obj.pntChaosBox[target]->m_Type == ITEMGET(13, 42))
						{
							gObjUseSkill.SkillChangeUse(Obj);
						}

						if (Obj.pntChaosBox[target]->m_Type == ITEMGET(13, 68))
						{
							gObjUseSkill.SkillChangeUse(Obj);
						}

						if (Obj.pntChaosBox[target]->m_Type == ITEMGET(13, 76))
						{
							gObjUseSkill.SkillChangeUse(Obj);
						}

						if (Obj.pntChaosBox[target]->m_Type == ITEMGET(13, 77))
						{
							gObjUseSkill.SkillChangeUse(Obj);
						}

						if (Obj.pntChaosBox[target]->m_Type == ITEMGET(13, 78))
						{
							gObjUseSkill.SkillChangeUse(Obj);
						}

						if (Obj.pntChaosBox[target]->m_Type == ITEMGET(13, 122))
						{
							gObjUseSkill.SkillChangeUse(Obj);
						}
						if (Obj.pntChaosBox[target]->m_Type == ITEMGET(13, 163))
						{
							gObjUseSkill.SkillChangeUse(Obj);
						}
						if (Obj.pntChaosBox[target]->m_Type == ITEMGET(13, 164))
						{
							gObjUseSkill.SkillChangeUse(Obj);
						}
						if (Obj.pntChaosBox[target]->m_Type == ITEMGET(13, 165))
						{
							gObjUseSkill.SkillChangeUse(Obj);
						}
						if (Obj.pntChaosBox[target]->m_Type == ITEMGET(13, 268))
						{
							gObjUseSkill.SkillChangeUse(Obj);
						}
					}

					gObjMakePreviewCharSet(Obj);
					gGameProtocol.GCEquipmentChange(Obj, source);
				}
			}
			return tFlag;
		case 17:
			BYTE res_3;
			if (sFlag == 17)
			{
				res_3 = gObjPentagramMixBoxInsertItemPos(Obj, *Obj.m_PlayerData->pPentagramMixBox[source], target, source);
			}

			else
			{
				res_3 = gObjPentagramMixBoxInsertItemPos(Obj, *Obj.pntInventory[source], target, -1);
			}

			if (res_3 == 0xFF)
			{
				return -1;
			}

			if (sFlag == 17)
			{
				Obj.m_PlayerData->pPentagramMixBox[target] = Obj.m_PlayerData->pPentagramMixBox[source];
				gObjPentagramMixBoxDeleteItem(Obj, source);
			}

			else
			{
				Obj.m_PlayerData->pPentagramMixBox[target] = Obj.pntInventory[source];
				gObjInventoryDeleteItem(Obj, source);
			}

			gObjPentagramMixItemSet(Obj, target, 1);

			if (sFlag == 0 && source == 236)
			{
				gGameProtocol.GCEquipmentChange(Obj, source);
			}

			return tFlag;
			break;

		default: break;
		}

		if (sFlag == 0 && (source < INVETORY_WEAR_SIZE || source == 236))
		{
			if (Obj.pntInventory[source]->IsItem() == 1)
			{
				if (Obj.pntInventory[source]->m_Type == ITEMGET(13, 10))
				{
					gObjUseSkill.SkillChangeUse(Obj);
				}

				if (Obj.pntInventory[source]->m_Type == ITEMGET(13, 39))
				{
					gObjUseSkill.SkillChangeUse(Obj);
				}

				if (Obj.pntInventory[source]->m_Type == ITEMGET(13, 40))
				{
					gObjUseSkill.SkillChangeUse(Obj);
				}

				if (Obj.pntInventory[source]->m_Type == ITEMGET(13, 41))
				{
					gObjUseSkill.SkillChangeUse(Obj);
				}

				if (Obj.pntInventory[source]->m_Type == ITEMGET(13, 42))
				{
					gObjUseSkill.SkillChangeUse(Obj);
				}

				if (Obj.pntInventory[source]->m_Type == ITEMGET(13, 68))
				{
					gObjUseSkill.SkillChangeUse(Obj);
				}

				if (Obj.pntInventory[source]->m_Type == ITEMGET(13, 76))
				{
					gObjUseSkill.SkillChangeUse(Obj);
				}

				if (Obj.pntInventory[source]->m_Type == ITEMGET(13, 77))
				{
					gObjUseSkill.SkillChangeUse(Obj);
				}

				if (Obj.pntInventory[source]->m_Type == ITEMGET(13, 78))
				{
					gObjUseSkill.SkillChangeUse(Obj);
				}

				if (Obj.pntInventory[source]->m_Type == ITEMGET(13, 122))
				{
					gObjUseSkill.SkillChangeUse(Obj);
				}

				if (Obj.pntInventory[source]->m_Type == ITEMGET(13, 163))
				{
					gObjUseSkill.SkillChangeUse(Obj);
				}

				if (Obj.pntInventory[source]->m_Type == ITEMGET(13, 164))
				{
					gObjUseSkill.SkillChangeUse(Obj);
				}

				if (Obj.pntInventory[source]->m_Type == ITEMGET(13, 165))
				{
					gObjUseSkill.SkillChangeUse(Obj);
				}

				if (Obj.pntInventory[source]->m_Type == ITEMGET(13, 268))
				{
					gObjUseSkill.SkillChangeUse(Obj);
				}
			}
			else
			{
				if (source == 10 || source == 11)
				{
					if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 10))
					{
						gObjUseSkill.SkillChangeUse(Obj);
					}

					if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 39))
					{
						gObjUseSkill.SkillChangeUse(Obj);
					}

					if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 40))
					{
						gObjUseSkill.SkillChangeUse(Obj);
					}

					if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 41))
					{
						gObjUseSkill.SkillChangeUse(Obj);
					}

					if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 42))
					{
						gObjUseSkill.SkillChangeUse(Obj);
					}

					if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 68))
					{
						gObjUseSkill.SkillChangeUse(Obj);
					}

					if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 76))
					{
						gObjUseSkill.SkillChangeUse(Obj);
					}

					if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 77))
					{
						gObjUseSkill.SkillChangeUse(Obj);
					}

					if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 78))
					{
						gObjUseSkill.SkillChangeUse(Obj);
					}

					if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 122))
					{
						gObjUseSkill.SkillChangeUse(Obj);
					}
					if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 163))
					{
						gObjUseSkill.SkillChangeUse(Obj);
					}
					if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 164))
					{
						gObjUseSkill.SkillChangeUse(Obj);
					}
					if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 165))
					{
						gObjUseSkill.SkillChangeUse(Obj);
					}
					if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 268))
					{
						gObjUseSkill.SkillChangeUse(Obj);
					}
				}
			}

			gObjMakePreviewCharSet(Obj);
			gGameProtocol.GCEquipmentChange(Obj, source);
		}

		if (tFlag == 0 && (target < INVETORY_WEAR_SIZE || target == 236))
		{
			if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 10))
			{
				gObjUseSkill.SkillChangeUse(Obj);
			}

			if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 39))
			{
				gObjUseSkill.SkillChangeUse(Obj);
			}

			if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 40))
			{
				gObjUseSkill.SkillChangeUse(Obj);
			}

			if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 41))
			{
				gObjUseSkill.SkillChangeUse(Obj);
			}

			if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 42))
			{
				gObjUseSkill.SkillChangeUse(Obj);
			}

			if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 68))
			{
				gObjUseSkill.SkillChangeUse(Obj);
			}

			if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 76))
			{
				gObjUseSkill.SkillChangeUse(Obj);
			}

			if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 77))
			{
				gObjUseSkill.SkillChangeUse(Obj);
			}

			if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 78))
			{
				gObjUseSkill.SkillChangeUse(Obj);
			}

			if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 122))
			{
				gObjUseSkill.SkillChangeUse(Obj);
			}
			if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 163))
			{
				gObjUseSkill.SkillChangeUse(Obj);
			}
			if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 164))
			{
				gObjUseSkill.SkillChangeUse(Obj);
			}
			if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 165))
			{
				gObjUseSkill.SkillChangeUse(Obj);
			}
			if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 268))
			{
				gObjUseSkill.SkillChangeUse(Obj);
			}

			gObjMakePreviewCharSet(Obj);
			gGameProtocol.GCEquipmentChange(Obj, target);
		}
	}
	//__finally
	{
		if (bPersonalShopTrans == 1)
		{
			Obj.m_bPShopTransaction = 0;
		}
	}
	return false;
}




BYTE gObjTradeRectCheck(CGameObject &Obj, int sx, int sy, int width, int height)
{
	int x;
	int y;
	int blank = 0;
	int xx;
	int yy;

	if ((sx + width) > 8)
	{
		return -1;
	}

	if ((sy + height) > 4)
	{
		return -1;
	}

	for (y = 0; y < height; y++)
	{
		yy = sy + y;

		for (x = 0; x < width; x++)
		{
			xx = sx + x;

			if (::ExtentCheck(xx, yy, 8, 4) == TRUE)
			{
				if (*(BYTE*)(Obj.TradeMap + yy * 8 + xx) != (BYTE)-1)
				{
					blank++;
					return -1;
				}
			}
			else
			{
				return -1;
			}
		}
	}

	if (blank == 0)
	{
		return (sy * 8 + sx);
	}

	return -1;
}




BOOL gObjTradeItemBoxSet(CGameObject &Obj, int itempos, int xl, int yl, BYTE set_byte)
{
	int itemposx = (itempos) % 8;
	int itemposy = (itempos) / 8;

	int xx, yy;

	for (int y = 0; y < yl; y++)
	{
		yy = itemposy + y;

		for (int x = 0; x < xl; x++)
		{
			xx = itemposx + x;

			if (ExtentCheck(xx, yy, 8, 4) == 1)
			{
				*(BYTE*)(Obj.TradeMap + yy * 8 + xx) = set_byte;
			}
			else
			{
				return false;
			}
		}
	}
	return true;
}




BYTE gObjTradeInventoryMove(CGameObject &Obj, BYTE source, BYTE target)
{
	int iwidth, iheight;
	UINT64 s_num;
	BYTE itembuf[13];
	if (source > TRADE_BOX_SIZE)
	{
		return -1;
	}

	if (target > MAIN_INVENTORY_SIZE && target != 236)
	{
		return -1;
	}

	if (Obj.TargetNumber < 0)
	{
		return -1;
	}

	if (Obj.pntTrade[source]->IsItem() == 0)
	{
		return -1;
	}

	if (Obj.m_IfState->use == 0 || Obj.m_IfState->type != 1)
	{
		return -1;
	}

	if (IsEventItem(*Obj.pntTrade[source]) == TRUE)
	{
		return -1;
	}

	if (gObjInventoryInsertItemPos(Obj, *Obj.pntTrade[source], target, 1) == 255)
	{
		return -1;
	}

	s_num = 0;
	s_num = Obj.pntTrade[source]->m_Number;

	Obj.pntTrade[source]->GetSize((int &)iwidth, (int &)iheight);

	gObjTradeItemBoxSet(Obj, source, iwidth, iheight, 255);
	Obj.pntTrade[source]->Clear();

	ItemByteConvert(itembuf, *Obj.pntTrade[source]);

	gGameProtocol.GCTradeOtherDel(*Obj.GetTarget(), source);
	if (getGameObject(Obj.TargetNumber)->m_PlayerData->ISBOT == true && g_BotSystem.GetBotType(*Obj.GetTarget()) == BOT_TRADE)
	{
		g_BotSystem.CheckAlchemist(Obj, getGameObject(Obj.TargetNumber)->m_PlayerData->wBotIndex);
	}
	if (gObjCheckSerial0ItemList(Obj.pntTrade[source]) != 0)
	{
		MsgOutput(Obj, Lang.GetText(0, 259));
		return -1;
	}

	if (gObjInventorySearchSerialNumber(Obj, s_num) == 0)
	{
		return -1;
	}

	if (target < INVETORY_WEAR_SIZE || target == 236)
	{
		if (Obj.pntInventory[target]->IsItem() == 1)
		{
			if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 10))
			{
				gObjUseSkill.SkillChangeUse(Obj);
			}
			if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 39))
			{
				gObjUseSkill.SkillChangeUse(Obj);
			}
			if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 40))
			{
				gObjUseSkill.SkillChangeUse(Obj);
			}
			if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 41))
			{
				gObjUseSkill.SkillChangeUse(Obj);
			}
			if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 42))
			{
				gObjUseSkill.SkillChangeUse(Obj);
			}
			if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 68))
			{
				gObjUseSkill.SkillChangeUse(Obj);
			}
			if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 76))
			{
				gObjUseSkill.SkillChangeUse(Obj);
			}
			if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 77))
			{
				gObjUseSkill.SkillChangeUse(Obj);
			}
			if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 78))
			{
				gObjUseSkill.SkillChangeUse(Obj);
			}
			if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 122))
			{
				gObjUseSkill.SkillChangeUse(Obj);
			}
			if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 163))
			{
				gObjUseSkill.SkillChangeUse(Obj);
			}
			if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 164))
			{
				gObjUseSkill.SkillChangeUse(Obj);
			}
			if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 165))
			{
				gObjUseSkill.SkillChangeUse(Obj);
			}
			if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 268))
			{
				gObjUseSkill.SkillChangeUse(Obj);
			}
		}

		gObjMakePreviewCharSet(Obj);
		gGameProtocol.GCEquipmentChange(Obj, target);
	}

	return false;
}




BYTE gObjInventoryTradeMove(CGameObject &Obj, BYTE source, BYTE target)
{
	int h, w, iwidth, iheight, blank;
	UINT64 s_num;

	if (source > MAIN_INVENTORY_SIZE)
	{
		return -1;
	}

	if (Obj.pntInventory[source]->IsItem() == 0)
	{
		return -1;
	}

	if (Obj.TargetNumber < 0)
	{
		return -1;
	}

	if (Obj.m_IfState->use == 0 || Obj.m_IfState->type != 1)
	{
		return -1;
	}

	if (getGameObject(Obj.TargetNumber)->m_PlayerData->bt_BotType == 1)
	{
		//check for item

	}

	if (Obj.pntInventory[source]->m_Type == ITEMGET(13, 20))
	{
		if (Obj.pntInventory[source]->m_Level == 0 ||
			Obj.pntInventory[source]->m_Level == 1)
		{
			return -1;
		}
	}

	if (Obj.pntInventory[source]->m_Type == ITEMGET(13, 20))
	{
		if (Obj.pntInventory[source]->m_Level == 2)
		{
			return -1;
		}
	}

	if (Obj.pntInventory[source]->m_Type == ITEMGET(13, 107))
	{
		if (Obj.pntInventory[source]->m_Level == 0 ||
			Obj.pntInventory[source]->m_Level == 1)
		{
			return -1;
		}
	}

	if (Obj.pntInventory[source]->m_Type == ITEMGET(13, 107))
	{
		if (Obj.pntInventory[source]->m_Level == 2)
		{
			return -1;
		}
	}

	if (Obj.pntInventory[source]->IsPeriodItem() == TRUE)
	{
		return -1;
	}

	if (Obj.pntInventory[source]->m_Type == ITEMGET(14, 11))
	{
		if (Obj.pntInventory[source]->m_Level == 13)
		{
			return -1;
		}
	}

	if (Obj.pntInventory[source]->m_Type == ITEMGET(12, 26))
	{
		if (Obj.pntInventory[source]->m_Level == 1 ||
			Obj.pntInventory[source]->m_Level == 2 ||
			Obj.pntInventory[source]->m_Level == 3 ||
			Obj.pntInventory[source]->m_Level == 4 ||
			Obj.pntInventory[source]->m_Level == 5)
		{
			return -1;
		}
	}

	if (Obj.pntInventory[source]->m_Type == ITEMGET(13, 38))
	{
		return -1;
	}

	if (Obj.pntInventory[source]->m_Type == ITEMGET(13, 39))
	{
		return -1;
	}

	if (!IsTransactionItem(Obj.pntInventory[source]->m_Type))
	{
		if (!(Obj.pntInventory[source]->m_Type == ITEMGET(14, 21) && Obj.pntInventory[source]->m_Level == 3))
		{
			return -1;
		}
	}

	Obj.pntInventory[source]->GetSize((int &)iwidth, (int &)iheight);
	s_num = Obj.pntInventory[source]->GetNumber();

	if (gObjCheckSerial0ItemList(Obj.pntInventory[source]) != 0)
	{
		MsgOutput(Obj, Lang.GetText(0, 259));
		return -1;
	}

	if (gObjInventorySearchSerialNumber(Obj, s_num) == 0)
	{
		return -1;
	}

	w = target % 8;
	h = target >> 3;

	if (ExtentCheck(w, h, 8, 4) == 0)
	{
		return -1;
	}

	if (*(BYTE*)(Obj.TradeMap + h * 8 + w) == 255)
	{
		BYTE itembuf[13];
		blank = gObjTradeRectCheck(Obj, w, h, iwidth, iheight);

		if (blank == 255)
		{
			return -1;
		}

		CGameObject lpObjTarget = *getGameObject(Obj.TargetNumber);
		Obj.pntTrade[blank] = Obj.pntInventory[source];
		gObjInventoryDeleteItem(Obj, source);
		gObjTradeItemBoxSet(Obj, blank, iwidth, iheight, Obj.pntTrade[blank]->m_Type);
		ItemByteConvert(itembuf, *Obj.pntTrade[blank]);
		gGameProtocol.GCTradeOtherAdd(lpObjTarget, blank, itembuf);
		if (getGameObject(Obj.TargetNumber)->m_PlayerData->ISBOT == true && g_BotSystem.GetBotType(lpObjTarget) == BOT_TRADE)
		{
			g_BotSystem.CheckAlchemist(Obj, getGameObject(Obj.TargetNumber)->m_PlayerData->wBotIndex);
		}

		g_PentagramSystem.GCTransPentagramJewelViewInfo(Obj, Obj.pntTrade[blank]);

		if (source < INVETORY_WEAR_SIZE || source == 236)
		{
			if (source == 10 || source == 11)
			{
				if (Obj.pntTrade[blank]->m_Type == ITEMGET(13, 10))
				{
					gObjUseSkill.SkillChangeUse(Obj);
				}
				if (Obj.pntTrade[blank]->m_Type == ITEMGET(13, 39))
				{
					gObjUseSkill.SkillChangeUse(Obj);
				}
				if (Obj.pntTrade[blank]->m_Type == ITEMGET(13, 40))
				{
					gObjUseSkill.SkillChangeUse(Obj);
				}
				if (Obj.pntTrade[blank]->m_Type == ITEMGET(13, 41))
				{
					gObjUseSkill.SkillChangeUse(Obj);
				}
				if (Obj.pntTrade[blank]->m_Type == ITEMGET(13, 42))
				{
					gObjUseSkill.SkillChangeUse(Obj);
				}
				if (Obj.pntTrade[blank]->m_Type == ITEMGET(13, 68))
				{
					gObjUseSkill.SkillChangeUse(Obj);
				}
				if (Obj.pntTrade[blank]->m_Type == ITEMGET(13, 76))
				{
					gObjUseSkill.SkillChangeUse(Obj);
				}
				if (Obj.pntTrade[blank]->m_Type == ITEMGET(13, 77))
				{
					gObjUseSkill.SkillChangeUse(Obj);
				}
				if (Obj.pntTrade[blank]->m_Type == ITEMGET(13, 78))
				{
					gObjUseSkill.SkillChangeUse(Obj);
				}
				if (Obj.pntTrade[blank]->m_Type == ITEMGET(13, 122))
				{
					gObjUseSkill.SkillChangeUse(Obj);
				}
				if (Obj.pntTrade[blank]->m_Type == ITEMGET(13, 163))
				{
					gObjUseSkill.SkillChangeUse(Obj);
				}
				if (Obj.pntTrade[blank]->m_Type == ITEMGET(13, 164))
				{
					gObjUseSkill.SkillChangeUse(Obj);
				}
				if (Obj.pntTrade[blank]->m_Type == ITEMGET(13, 165))
				{
					gObjUseSkill.SkillChangeUse(Obj);
				}
				if (Obj.pntTrade[blank]->m_Type == ITEMGET(13, 268))
				{
					gObjUseSkill.SkillChangeUse(Obj);
				}
			}
			gObjMakePreviewCharSet(Obj);
			gGameProtocol.GCEquipmentChange(Obj, source);
		}
		return true;
	}
	return -1;
}




BYTE gObjTradeTradeMove(CGameObject &Obj, BYTE source, BYTE target)
{
	int h, w;
	int iwidth, iheight;
	BYTE TempTradeMap[TRADE_BOX_MAP_SIZE];
	int blank;

	if (source > TRADE_BOX_SIZE)
	{
		return -1;
	}
	if (source == target)
	{
		return -1;
	}
	if (Obj.pntTrade[source]->IsItem() == 0)
	{
		return -1;
	}
	if (Obj.pntTrade[target]->IsItem() == 1)
	{
		return -1;
	}
	if (Obj.TargetNumber < 0)
	{
		return -1;
	}
	if (Obj.m_IfState->use == 0 || Obj.m_IfState->type != 1)
	{
		return -1;
	}

	Obj.pntTrade[source]->GetSize((int &)iwidth, (int &)iheight);
	std::memcpy(TempTradeMap, Obj.TradeMap, TRADE_BOX_MAP_SIZE);
	gObjTradeItemBoxSet(Obj, source, iwidth, iheight, 255);

	w = target % 8;
	h = target / 8;

	if (ExtentCheck(w, h, 8, 4) == 0)
	{
		std::memcpy(Obj.TradeMap, TempTradeMap, TRADE_BOX_MAP_SIZE);
		return -1;
	}

	if (*(BYTE*)(Obj.TradeMap + h * 8 + w) == 255)
	{
		BYTE itembuf[13];
		blank = gObjTradeRectCheck(Obj, w, h, iwidth, iheight);

		if (blank == 255)
		{
			std::memcpy(Obj.TradeMap, TempTradeMap, TRADE_BOX_MAP_SIZE);
			return -1;
		}

		Obj.pntTrade[blank] = Obj.pntTrade[source];
		Obj.pntTrade[source]->Clear();
		gGameProtocol.GCTradeOtherDel(Obj, source);
		gObjTradeItemBoxSet(Obj, blank, iwidth, iheight, Obj.pntTrade[blank]->m_Type);
		ItemByteConvert(itembuf, *Obj.pntTrade[blank]);
		gGameProtocol.GCTradeOtherAdd(Obj, blank, itembuf);
		g_PentagramSystem.GCTransPentagramJewelViewInfo(Obj, Obj.pntTrade[blank]);
		return true;
	}

	std::memcpy(Obj.TradeMap, TempTradeMap, TRADE_BOX_MAP_SIZE);
	return -1;
}




void gObjTradeCancel(CGameObject &Obj)
{
	if (Obj.Type != OBJ_USER || Obj.Connected != PLAYER_PLAYING)
	{
		return;
	}
	if (Obj.m_IfState->use != 1)
	{
		return;
	}
	if (Obj.m_IfState->type != 1)
	{
		return;
	}

	

	gObjInventoryRollback(Obj);
	g_MixSystem.ChaosBoxInit(Obj);
	g_PentagramMixSystem.PentagramMixBoxInit(&Obj);
	Obj.TargetNumber = -1;
	Obj.m_IfState->use = 0;
	Obj.m_IfState->state = 0; //season4.5 add-on
	gObjCharTradeClear(Obj);
	gGameProtocol.GCMoneySend(Obj, Obj.m_PlayerData->Money);
	GCItemObjectListSend(Obj);
	gGameProtocol.GCEventInventoryItemListSend(Obj);
	gGameProtocol.GCEquipmentSend(Obj);
	g_PeriodItemEx.OnRequestPeriodItemList(Obj);
	gGameProtocol.GCMagicListMultiSend(Obj, 0);
	gObjUseSkill.SkillChangeUse(Obj);

	if (Obj.m_PlayerData->GuildNumber > 0)
	{
		gGameProtocol.GCGuildViewportNowPaint(Obj, Obj.m_PlayerData->GuildName, 0, 0);
		gObjNotifyUpdateUnionV1(Obj);
		gObjNotifyUpdateUnionV2(Obj);
	}
}




void gObjTempInventoryItemBoxSet(BYTE * TempMap, int itempos, int xl, int yl, BYTE set_byte)
{
	int itemposx = (itempos - INVETORY_WEAR_SIZE) % 8;
	int itemposy = (itempos - INVETORY_WEAR_SIZE) / 8;

	for (int y = 0; y < yl; y++)
	{
		for (int x = 0; x < xl; x++)
		{
			*(BYTE*)(TempMap + (itemposy + y) * 8 + (itemposx + x)) = set_byte;
		}
	}
}




BYTE gObjTempInventoryRectCheck(CGameObject &Obj, BYTE * TempMap, int sx, int sy, int width, int height)
{
	int x, y, blank = 0;

	if (sx + width > 8)
	{
		return -1;
	}
	if (sy + height > MAX_INVENTORY_H)	//24
	{
		return -2;
	}
	// ----
	bool bResult = CheckOutOfInventory(Obj, sy, height);
	// ----
	if (!bResult)
	{
		if (bResult == -1)
		{
			return -2;
		}
		// ----
		return -1;
	}
	// ----
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			if (InventoryExtentCheck(sx + x, sy + y, 8, MAX_INVENTORY_H) && *(BYTE*)(TempMap + (sy + y) * 8 + (sx + x)) != 255)
			{
				blank += 1;
				return -1;
			}
		}
	}
	if (blank == 0)
	{
		return sx + sy * 8 + INVETORY_WEAR_SIZE;
	}
	return -1;
}




BYTE gObjTempInventoryInsertItem(CGameObject &Obj, CItemObject &item, BYTE * TempMap)
{
	int w, h, iwidth, iheight;
	BYTE blank = 0;

	if (item.GetSize(iwidth, iheight) == 0)
	{
		return -1;
	}

	for (h = 0; h < 4 * Obj.m_PlayerData->m_InventoryExpansion + 8; h++)
	{
		for (w = 0; w < 8; w++)
		{
			if (*(BYTE*)(TempMap + h * 8 + w) == 255)
			{
				blank = gObjTempInventoryRectCheck(Obj, TempMap, w, h, iwidth, iheight);

				if (blank == 254)
				{
					goto GOTO_EndFunc;
				}
				if (blank != 255)
				{
					gObjTempInventoryItemBoxSet(TempMap, blank, iwidth, iheight, item.m_Type);
					return blank;
				}
			}
		}
	}
GOTO_EndFunc:
	return -1;
}




BOOL TradeItemInventoryPutTest(CGameObject &Obj)
{
	BYTE TempInventoryMap[INVENTORY_MAP_SIZE];
	BYTE TempEventInventoryMap[EVENT_INVENTORY_MAP_SIZE];

	int result = 1;
	CGameObject* target = Obj.GetTarget();

	if (gObjIsConnected(Obj) == 0)
	{
		return false;
	}

	std::memcpy(TempInventoryMap, Obj.pInventoryMap, INVENTORY_MAP_SIZE);
	std::memcpy(TempEventInventoryMap, Obj.EventInventoryMap, EVENT_INVENTORY_MAP_SIZE);

	for (int n = 0; n < TRADE_BOX_SIZE; n++)
	{
		if (target->pntTrade[n]->IsItem() == 1)
		{
			if (IsEventItem(*target->pntTrade[n]))
			{
				if (gObjTempEventInventoryInsertItem(Obj, *target->pntTrade[n], TempEventInventoryMap) == 255)
				{
					return false;
				}
			}

			else
			{
				if (gObjTempInventoryInsertItem(Obj, *target->pntTrade[n], TempInventoryMap) == 255)
				{
					return false;
				}
			}

			result = 1;
		}
	}

	if (result == 1)
	{
		return true;
	}

	return false;
}




BOOL TradeitemInventoryPut(CGameObject &Obj)
{
	int number;
	BYTE insert;
	char szItemName[50] = "Item";
	BYTE ExOption[8];

	number = Obj.TargetNumber;
	CGameObject* target = getGameObject(number);

	if (gObjIsConnected(*target) == 0)
	{
		return false;
	}

	for (int n = 0; n < TRADE_BOX_SIZE; n++)
	{
		if (target->pntTrade[n]->IsItem() == 1)
		{
			if (g_PentagramSystem.IsPentagramItem(*target->pntTrade[n]) == true)
			{
				target->pntTrade[n]->m_Durability -= 1.0;
			}

			if (IsEventItem(*target->pntTrade[n]))
			{
				insert = gObjEventInventoryInsertItem(Obj, *target->pntTrade[n]);
			}

			else
			{
				insert = gObjInventoryInsertItem(Obj, *target->pntTrade[n]);
			}

			if (insert != 255)
			{
				if (g_PentagramSystem.IsPentagramItem(*target->pntTrade[n]) == true)
				{
					int nJewelCount = 0;
					gTransPentagramJewelInfo(Obj, insert, number, *target->pntTrade[n], &nJewelCount);
				}

				strcpy(szItemName, target->pntTrade[n]->GetName());
				ItemIsBufExOption(ExOption, target->pntTrade[n]);

			}
			else
			{
				return false;
			}
		}
	}
	return true;
}

bool gObjTradeFullExCItemObjectCheck(CGameObject &Obj)
{
	if (g_ConfigRead.data.common.TradeExCItemObjectLimit == true)
		return false;

	for (int i = 0; i < TRADE_BOX_SIZE; i++)
	{
		if (Obj.pntTrade[i]->IsItem())
		{
			if (Obj.pntTrade[i]->m_NewOption == 63)
				return true;
		}
	}

	return false;
}

bool gObjTradeItemSerialCheck(CGameObject &Obj)
{
	if (g_ConfigRead.data.common.BlockTradeItems == true)
		return false;

	for (int i = 0; i < TRADE_BOX_SIZE; i++)
	{
		if (Obj.pntTrade[i]->IsItem())
		{
			if (Obj.pntTrade[i]->m_Number == (UINT64)-1)
				return true;
		}
	}

	return false;
}



void gObjTradeOkButton(CGameObject &Obj)
{
	BYTE result = 1;
	int number = Obj.TargetNumber;
	CGameObject* target = getGameObject(number);

	if (number < 0)
	{
		return;
	}

	if (gObjIsConnected(*target) == 0)
	{
		return;
	}

	if (target->TargetNumber != Obj.m_Index)
	{
		return;
	}

	if (Obj.TradeOk != 1 || target->TradeOk != 1)
	{
		return;
	}

	if (TradeItemInventoryPutTest(Obj) == 1)
	{
		if (TradeItemInventoryPutTest(*target) == 1)
		{
			result = 1;
		}
		else
		{
			result = 2;
		}
	}
	else
	{
		result = 2;
	}

	if (gObjCheckMaxZen(Obj, target->TradeMoney) == 0)
	{
		result = 5;
	}

	if (gObjCheckMaxZen(*target, Obj.TradeMoney) == 0)
	{
		result = 5;
	}

	if (g_kJewelOfHarmonySystem.IsEnableToTrade(*target) == 0 || g_kJewelOfHarmonySystem.IsEnableToTrade(Obj) == 0)
	{
		result = 4;
		gObjTradeCancel(Obj);
		gObjTradeCancel(*target);
		gGameProtocol.CGTradeResult(Obj, result);
		gGameProtocol.CGTradeResult(*target, result);
		return;
	}

	if (g_PentagramSystem.IsEnableToTradePentagramItem(Obj) == 0 || g_PentagramSystem.IsEnableToTradePentagramItem(Obj) == 0)
	{
		result = 6;
		gObjTradeCancel(Obj);
		gObjTradeCancel(*target);
		gGameProtocol.CGTradeResult(Obj, result);
		gGameProtocol.CGTradeResult(*target, result);
		return;
	}

	if (g_PentagramSystem.IsEnableTransPentagramJewelInfo(Obj, number) == false)
	{
		result = 7;
		gObjTradeCancel(Obj);
		gObjTradeCancel(*target);
		gGameProtocol.CGTradeResult(Obj, result);
		gGameProtocol.CGTradeResult(*target, result);
		return;
	}

	if (gObjTradeFullExCItemObjectCheck(*target) == true || gObjTradeFullExCItemObjectCheck(Obj) == true)
	{
		result = 4;
		gObjTradeCancel(Obj);
		gObjTradeCancel(*target);
		gGameProtocol.CGTradeResult(Obj, result);
		gGameProtocol.CGTradeResult(*target, result);
		return;
	}

	if (gObjTradeItemSerialCheck(*target) == true || gObjTradeItemSerialCheck(Obj) == true)
	{
		result = 4;
		gObjTradeCancel(Obj);
		gObjTradeCancel(*target);
		gGameProtocol.CGTradeResult(Obj, result);
		gGameProtocol.CGTradeResult(*target, result);
		return;
	}

	if (result == 1)
	{
		TradeitemInventoryPut(Obj);
		TradeitemInventoryPut(*target);
		gObjInventoryCommit(Obj);
		gObjInventoryCommit(*target);
		gObjMakePreviewCharSet(Obj);
		gObjMakePreviewCharSet(*target);

		Obj.m_PlayerData->Money -= Obj.TradeMoney;
		if (Obj.m_PlayerData->Money < 0)
		{
			Obj.m_PlayerData->Money = 0;
		}
		target->m_PlayerData->Money -= target->TradeMoney;
		if (target->m_PlayerData->Money < 0)
		{
			target->m_PlayerData->Money = 0;
		}

		Obj.m_PlayerData->Money += target->TradeMoney;
		target->m_PlayerData->Money += Obj.TradeMoney;

		GJSetCharacterInfo(Obj, 0);
		GJSetCharacterInfo(*target, 0);
	}
	else
	{
		result = 2;
		gObjTradeCancel(Obj);
		gObjTradeCancel(*target);
		gGameProtocol.CGTradeResult(Obj, result);
		gGameProtocol.CGTradeResult(*target, result);
		return;
	}

	Obj.TargetNumber = -1;
	Obj.m_IfState->use = 0;
	target->TargetNumber = -1;
	target->m_IfState->use = 0;
	Obj.TradeOk = 0;
	target->TradeOk = 0;
	Obj.TradeMoney = 0;
	target->TradeMoney = 0;

	gGameProtocol.GCMoneySend(Obj, Obj.m_PlayerData->Money);
	gGameProtocol.GCMoneySend(*target, target->m_PlayerData->Money);
	gGameProtocol.CGTradeResult(Obj, result);
	gGameProtocol.CGTradeResult(*target, result);

	GCItemObjectListSend(Obj);
	GCItemObjectListSend(*target);

	gGameProtocol.GCEventInventoryItemListSend(Obj);
	gGameProtocol.GCEventInventoryItemListSend(*target);

	g_PeriodItemEx.OnRequestPeriodItemList(Obj);
	g_PeriodItemEx.OnRequestPeriodItemList(*target);

	for (int n = 0; n < TRADE_BOX_SIZE; n++)
	{
		Obj.pntTrade[n]->Clear();
		target->pntTrade[n]->Clear();
	}

	gObjNotifyUpdateUnionV1(*target);
	gObjNotifyUpdateUnionV2(*target);
}




BYTE LevelSmallConvert(CGameObject &Obj, int inventoryindex)
{
	int level = Obj.pntInventory[inventoryindex]->m_Level;

	if (level == 15)
	{
		return 7;
	}

	if (level >= 12 && level <= 14)
	{
		return 6;
	}

	if (level == 11)
	{
		return 5;
	}

	if (level >= 9 && level <= 10)
	{
		return 4;
	}

	if (level >= 7 && level <= 8)
	{
		return 3;
	}

	if (level >= 5 && level <= 6)
	{
		return 2;
	}

	if (level >= 3 && level <= 4)
	{
		return 1;
	}

	return 0;
}




BYTE LevelSmallConvert(int level)
{
	if (level == 15)
	{
		return 7;
	}

	if (level >= 12 && level <= 14)
	{
		return 6;
	}

	if (level == 11)
	{
		return 5;
	}

	if (level >= 9 && level <= 10)
	{
		return 4;
	}

	if (level >= 7 && level <= 8)
	{
		return 3;
	}

	if (level >= 5 && level <= 6)
	{
		return 2;
	}

	if (level >= 3 && level <= 4)
	{
		return 1;
	}

	return 0;
}




void gObjAttackDamageCalc(CGameObject &Obj)
{
	return;	// Here goes a Macro
}




void gObjMakePreviewCharSet(CGameObject &Obj)
{
	BYTE index;
	int itemindex;
	memset(Obj.CharSet, 0, sizeof(Obj.CharSet));

	Obj.CharSet[CS_CLASS] = CS_SET_CLASS_SX(Obj.Class);

	if (Obj.m_PlayerData->ChangeUP == 1)
	{
		Obj.CharSet[CS_CLASS] |= CS_SET_CHANGEUP_SX;
	}

	if (Obj.m_PlayerData->ChangeUP == 2)
	{
		Obj.CharSet[CS_CLASS] |= CS_SET_CHANGEUP_SX;
		Obj.CharSet[CS_CLASS] |= CS_SET_CHANGEUP2_SX;
	}

	if (Obj.m_ActionNumber == 0x80)
	{
		Obj.CharSet[CS_CLASS] |= 1;
	}

	else if (Obj.m_ActionNumber == 0x81)
	{
		Obj.CharSet[CS_CLASS] |= 2;
	}

	else
	{
		Obj.CharSet[CS_CLASS] = Obj.CharSet[CS_CLASS];
	}

	// Set Righth HAnd Item
	if (Obj.pntInventory[0]->m_Type < ITEMGET(0, 0))
	{
		Obj.CharSet[CS_WEAPON1_DATA] |= (BYTE)DBI_GET_TYPE(-1);
		Obj.CharSet[CS_WEAPON1_TYPE] = (BYTE)DBI_GET_INDEX(-1);
	}
	else
	{
		Obj.CharSet[CS_WEAPON1_DATA] |= DBI_GET_TYPE(Obj.pntInventory[0]->m_Type);
		Obj.CharSet[CS_WEAPON1_TYPE] = DBI_GET_INDEX(Obj.pntInventory[0]->m_Type);
	}

	// Set Left Hand
	if (Obj.pntInventory[1]->m_Type < ITEMGET(0, 0))
	{
		Obj.CharSet[CS_WEAPON2_DATA] |= (BYTE)DBI_GET_TYPE(-1);
		Obj.CharSet[CS_WEAPON2_TYPE] = (BYTE)DBI_GET_INDEX(-1);
	}
	else
	{
		Obj.CharSet[CS_WEAPON2_DATA] |= DBI_GET_TYPE(Obj.pntInventory[1]->m_Type);
		Obj.CharSet[CS_WEAPON2_TYPE] = DBI_GET_INDEX(Obj.pntInventory[1]->m_Type);
	}

	// Set Helmet
	if (Obj.pntInventory[2]->m_Type < ITEMGET(0, 0) && Obj.m_PlayerData->ISBOT == false)
	{
		Obj.CharSet[13] |= CS_SET_HELMET1(-1);
		Obj.CharSet[9] |= CS_SET_HELMET2(-1);
		Obj.CharSet[3] |= CS_SET_HELMET3(-1);
	}
	else if (Obj.m_PlayerData->ISBOT == true)
	{
		Obj.CharSet[13] |= CS_SET_HELMET1(ITEMGET(7, 59));
		Obj.CharSet[9] |= CS_SET_HELMET2(ITEMGET(7, 59));
		Obj.CharSet[3] |= CS_SET_HELMET3(ITEMGET(7, 59));
	}
	else
	{
		Obj.CharSet[13] |= CS_SET_HELMET1(Obj.pntInventory[2]->m_Type);
		Obj.CharSet[9] |= CS_SET_HELMET2(Obj.pntInventory[2]->m_Type);
		Obj.CharSet[3] |= CS_SET_HELMET3(Obj.pntInventory[2]->m_Type);
	}
	// Set Armor
	if (Obj.pntInventory[3]->m_Type < ITEMGET(0, 0) && Obj.m_PlayerData->ISBOT == false)
	{
		Obj.CharSet[14] |= CS_SET_ARMOR1(-1);
		Obj.CharSet[9] |= CS_SET_ARMOR2(-1);
		Obj.CharSet[3] |= CS_SET_ARMOR3(-1);
	}
	else if (Obj.m_PlayerData->ISBOT == true)
	{
		Obj.CharSet[14] |= CS_SET_ARMOR1(ITEMGET(8, 59));
		Obj.CharSet[9] |= CS_SET_ARMOR2(ITEMGET(8, 59));
		Obj.CharSet[3] |= CS_SET_ARMOR3(ITEMGET(8, 59));
	}
	else
	{
		Obj.CharSet[14] |= CS_SET_ARMOR1(Obj.pntInventory[3]->m_Type);
		Obj.CharSet[9] |= CS_SET_ARMOR2(Obj.pntInventory[3]->m_Type);
		Obj.CharSet[3] |= CS_SET_ARMOR3(Obj.pntInventory[3]->m_Type);
	}

	// Set Pants
	if (Obj.pntInventory[4]->m_Type < ITEMGET(0, 0) && Obj.m_PlayerData->ISBOT == false)
	{
		Obj.CharSet[14] |= CS_SET_PANTS1(-1);
		Obj.CharSet[9] |= CS_SET_PANTS2(-1);
		Obj.CharSet[4] |= CS_SET_PANTS3(-1);
	}
	else if (Obj.m_PlayerData->ISBOT == true)
	{
		Obj.CharSet[14] |= CS_SET_PANTS1(ITEMGET(9, 59));
		Obj.CharSet[9] |= CS_SET_PANTS2(ITEMGET(9, 59));
		Obj.CharSet[4] |= CS_SET_PANTS3(ITEMGET(9, 59));
	}
	else
	{
		Obj.CharSet[14] |= CS_SET_PANTS1(Obj.pntInventory[4]->m_Type);
		Obj.CharSet[9] |= CS_SET_PANTS2(Obj.pntInventory[4]->m_Type);
		Obj.CharSet[4] |= CS_SET_PANTS3(Obj.pntInventory[4]->m_Type);
	}

	// Set Gloves
	if (Obj.pntInventory[5]->m_Type < ITEMGET(0, 0) && Obj.m_PlayerData->ISBOT == false)
	{
		Obj.CharSet[15] |= CS_SET_GLOVES1(-1);
		Obj.CharSet[9] |= CS_SET_GLOVES2(-1);
		Obj.CharSet[4] |= CS_SET_GLOVES3(-1);
	}
	else if (Obj.m_PlayerData->ISBOT == true)
	{
		Obj.CharSet[15] |= CS_SET_GLOVES1(ITEMGET(10, 59));
		Obj.CharSet[9] |= CS_SET_GLOVES2(ITEMGET(10, 59));
		Obj.CharSet[4] |= CS_SET_GLOVES3(ITEMGET(10, 59));
	}
	else
	{
		Obj.CharSet[15] |= CS_SET_GLOVES1(Obj.pntInventory[5]->m_Type);
		Obj.CharSet[9] |= CS_SET_GLOVES2(Obj.pntInventory[5]->m_Type);
		Obj.CharSet[4] |= CS_SET_GLOVES3(Obj.pntInventory[5]->m_Type);
	}

	// Set Boots
	if (Obj.pntInventory[6]->m_Type < ITEMGET(0, 0) && Obj.m_PlayerData->ISBOT == false)
	{
		Obj.CharSet[15] |= CS_SET_BOOTS1(-1);
		Obj.CharSet[9] |= CS_SET_BOOTS2(-1);
		Obj.CharSet[5] |= CS_SET_BOOTS3(-1);
	}
	else if (Obj.m_PlayerData->ISBOT == true)
	{
		Obj.CharSet[15] |= CS_SET_BOOTS1(ITEMGET(11, 59));
		Obj.CharSet[9] |= CS_SET_BOOTS2(ITEMGET(11, 59));
		Obj.CharSet[5] |= CS_SET_BOOTS3(ITEMGET(11, 59));
	}
	else
	{
		Obj.CharSet[15] |= CS_SET_BOOTS1(Obj.pntInventory[6]->m_Type);
		Obj.CharSet[9] |= CS_SET_BOOTS2(Obj.pntInventory[6]->m_Type);
		Obj.CharSet[5] |= CS_SET_BOOTS3(Obj.pntInventory[6]->m_Type);
	}

	index = 0;

	// Set Part onf Wings
	if (Obj.pntInventory[7]->m_Type < ITEMGET(0, 0))
	{
		index = 0;
	}

	index |= 3;

	if (Obj.pntInventory[8]->m_Type > 0)
	{
		if (Obj.pntInventory[8]->m_Type == ITEMGET(13, 67) || Obj.pntInventory[8]->m_Type == ITEMGET(13, 123))
		{
			index = 2;
		}
		else
		{
			index = Obj.pntInventory[8]->m_Type & 3;
		}
	}

	int levelindex;
	Obj.CharSet[5] |= index;

	if (Obj.m_PlayerData->ISBOT == false)
	{
		levelindex = CS_SET_SMALLLEVEL_RH(LevelSmallConvert(Obj, 0));
		levelindex |= CS_SET_SMALLLEVEL_LH(LevelSmallConvert(Obj, 1));
		levelindex |= CS_SET_SMALLLEVEL_HELMET(LevelSmallConvert(Obj, 2));
		levelindex |= CS_SET_SMALLLEVEL_ARMOR(LevelSmallConvert(Obj, 3));
		levelindex |= CS_SET_SMALLLEVEL_PANTS(LevelSmallConvert(Obj, 4));
		levelindex |= CS_SET_SMALLLEVEL_GLOVES(LevelSmallConvert(Obj, 5));
		levelindex |= CS_SET_SMALLLEVEL_BOOTS(LevelSmallConvert(Obj, 6));
	}

	else
	{
		levelindex = CS_SET_SMALLLEVEL_RH(LevelSmallConvert(Obj, 0));
		levelindex |= CS_SET_SMALLLEVEL_LH(LevelSmallConvert(Obj, 1));
		levelindex |= CS_SET_SMALLLEVEL_HELMET(7);
		levelindex |= CS_SET_SMALLLEVEL_ARMOR(7);
		levelindex |= CS_SET_SMALLLEVEL_PANTS(7);
		levelindex |= CS_SET_SMALLLEVEL_GLOVES(7);
		levelindex |= CS_SET_SMALLLEVEL_BOOTS(7);
	}

	Obj.CharSet[6] = CS_SET_SMALLLEVEL1(levelindex);
	Obj.CharSet[7] = CS_SET_SMALLLEVEL2(levelindex);
	Obj.CharSet[8] = CS_SET_SMALLLEVEL3(levelindex);

	/*

	if ( (Obj.pntInventory[7]->m_Type >= ITEMGET(12,3) && Obj.pntInventory[7]->m_Type <= ITEMGET(12,6)) || (Obj.pntInventory[7]->m_Type == ITEMGET(13,30)) || (Obj.pntInventory[7]->m_Type == ITEMGET(12,41)) || (Obj.pntInventory[7]->m_Type == ITEMGET(12,42)))
	{

	Obj.CharSet[5] |= CS_SET_WING1(-1);

	if ( Obj.pntInventory[7]->m_Type == ITEMGET(13,30) )
	{
	Obj.CharSet[9] |= 5;
	}
	else if ( Obj.pntInventory[7]->m_Type == ITEMGET(12,41) )
	{
	Obj.CharSet[9] |= 6;
	}
	else if ( Obj.pntInventory[7]->m_Type == ITEMGET(12,42) )
	{
	Obj.CharSet[9] |= 7;
	}
	else
	{
	Obj.CharSet[9] |= (Obj.pntInventory[7]->m_Type - 2) & 0x07;
	}

	}*/

	// Set Excellent Items
	Obj.CharSet[10] = 0;

	if (Obj.pntInventory[2]->IsExtItem() != FALSE)
		Obj.CharSet[10] = (char)0x80;

	if (Obj.pntInventory[3]->IsExtItem() != FALSE)
		Obj.CharSet[10] |= 0x40;

	if (Obj.pntInventory[4]->IsExtItem() != FALSE)
		Obj.CharSet[10] |= 0x20;

	if (Obj.pntInventory[5]->IsExtItem() != FALSE)
		Obj.CharSet[10] |= 0x10;

	if (Obj.pntInventory[6]->IsExtItem() != FALSE)
		Obj.CharSet[10] |= 0x8;

	if (Obj.pntInventory[0]->IsExtItem() != FALSE)
		Obj.CharSet[10] |= 0x04;

	if (Obj.pntInventory[1]->IsExtItem() != FALSE)
		Obj.CharSet[10] |= 0x02;

	// Set Set Items
	Obj.CharSet[11] = 0;

	if (Obj.pntInventory[2]->IsSetItem() != FALSE)
		Obj.CharSet[11] = (char)0x80;

	if (Obj.pntInventory[3]->IsSetItem() != FALSE)
		Obj.CharSet[11] |= 0x40;

	if (Obj.pntInventory[4]->IsSetItem() != FALSE)
		Obj.CharSet[11] |= 0x20;

	if (Obj.pntInventory[5]->IsSetItem() != FALSE)
		Obj.CharSet[11] |= 0x10;

	if (Obj.pntInventory[6]->IsSetItem() != FALSE)
		Obj.CharSet[11] |= 0x8;

	if (Obj.pntInventory[0]->IsSetItem() != FALSE)
		Obj.CharSet[11] |= 0x04;

	if (Obj.pntInventory[1]->IsSetItem() != FALSE)
		Obj.CharSet[11] |= 0x02;

	gObjCalCharacter.CalcCharacter(Obj);

	if (Obj.m_PlayerData->IsFullSetItem != false)
		Obj.CharSet[11] |= 0x01;

	/*
	13
	0	"Guardian Angel"
	1	"Imp"
	2	"Horn of Uniria"
	3	"Horn of Dinorant"
	4	"Horn of Dark Horse"
	*/
	if ((Obj.pntInventory[8]->m_Type & 0x03) != 0)
	{
		if (Obj.pntInventory[8]->m_Type > 0)
		{
			Obj.CharSet[10] |= 0x01;
		}
	}

	index = 0;
	int helperid = Obj.pntInventory[8]->m_Type;
	if (helperid != ITEMGET(13, 4))
	{
		if (helperid < ITEMGET(0, 0))
		{
			index |= CS_SET_HELPER(-1);
		}
		else
		{
			index |= CS_SET_HELPER(helperid);
		}
	}
	else if (helperid != ITEMGET(13, 5))
	{
		if (helperid < ITEMGET(0, 0))
		{
			index |= CS_SET_HELPER(-1);
		}
		else
		{
			index |= CS_SET_HELPER(helperid);
		}
	}

	if (Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 4))	// Dark Horse
	{
		Obj.CharSet[12] |= 0x01;
	}

	if (Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 5))	// Dark Raven
	{
		Obj.CharSet[12] |= 0x02;
	}
	//Hermex Fix for Charset 16 to prevent Wings Level 3 Visual Bug
	//Obj.CharSet[16] = 0;

	/*

	if( Obj.pntInventory[7]->m_Type >= ITEMGET(12,36) && Obj.pntInventory[7]->m_Type <= ITEMGET(12,43) ) //Third Wings
	{
	Obj.CharSet[16] |= (Obj.pntInventory[7]->m_Type - 35) * 32; // * 4
	}*/

	if ((Obj.pntInventory[10]->IsItem() == TRUE && Obj.pntInventory[10]->m_Type == ITEMGET(13, 169) && Obj.pntInventory[10]->m_Durability > 0.0) ||
		(Obj.pntInventory[11]->IsItem() == TRUE && Obj.pntInventory[11]->m_Type == ITEMGET(13, 169) && Obj.pntInventory[11]->m_Durability > 0.0))
	{
		Obj.CharSet[12] |= 0x08;
	}

	if ((Obj.pntInventory[10]->IsItem() == TRUE && Obj.pntInventory[10]->m_Type == ITEMGET(13, 170) && Obj.pntInventory[10]->m_Durability > 0.0) ||
		(Obj.pntInventory[11]->IsItem() == TRUE && Obj.pntInventory[11]->m_Type == ITEMGET(13, 170) && Obj.pntInventory[11]->m_Durability > 0.0))
	{
		Obj.CharSet[12] |= 0x08;
	}

	if (Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 37))	// Fenrir
	{
		Obj.CharSet[10] &= 0xFE;
		Obj.CharSet[12] &= 0xFE;
		Obj.CharSet[12] |= 0x04;
		Obj.CharSet[16] = 0;
		Obj.CharSet[17] = 0;

		if (Obj.pntInventory[Obj.m_btInvenPetPos]->IsFenrirIncLastAttackDamage() != FALSE)
		{
			Obj.CharSet[16] |= 0x01; //1
		}

		if (Obj.pntInventory[Obj.m_btInvenPetPos]->IsFenrirDecLastAttackDamage() != FALSE)
		{
			Obj.CharSet[16] |= 0x02; //2
		}

		if (Obj.pntInventory[Obj.m_btInvenPetPos]->IsFenrirIllusion() != FALSE)
		{
			Obj.CharSet[17] |= 0x01; //4
		}
	}

	/*

	if((Obj.pntInventory[7]->m_Type >= ITEMGET(12,36) && Obj.pntInventory[7]->m_Type <= ITEMGET(12,40)) || Obj.pntInventory[7]->m_Type == ITEMGET(12,43))
	{
	Obj.CharSet[5] |= 0xC;
	Obj.CharSet[16] |= ((Obj.pntInventory[7]->m_Type - 35) & 0x07 ); // 4 *

	if(Obj.pntInventory[7]->m_Type == ITEMGET(12, 43))
	{
	Obj.CharSet[16] |= 0x18;
	}
	}*/

	WORD ItemIndex = ITEM_GET_INDEX(Obj.pntInventory[7]->m_Type);

	if (ItemIndex >= 0 && ItemIndex <= 6)
	{
		Obj.CharSet[16] |= (0x04 * (ItemIndex + 1));
	}

	else if (ItemIndex >= 36 && ItemIndex <= 43)
	{
		Obj.CharSet[9] |= 0x01;
		Obj.CharSet[16] |= (0x04 * (ItemIndex - 36));
	}

	else if ((ItemIndex >= 49 && ItemIndex <= 50) || (ItemIndex >= 130 && ItemIndex <= 135))
	{
		Obj.CharSet[9] |= 0x02;

		if (ItemIndex == 49 || ItemIndex == 50)
		{
			Obj.CharSet[16] |= (0x04 * (ItemIndex - 49));
		}

		else if (ItemIndex >= 130 && ItemIndex <= 135)
		{
			Obj.CharSet[16] |= (0x04 * (0x02 + (ItemIndex - 130)));
		}
	}

	else if (ItemIndex >= 262 && ItemIndex <= 269)
	{
		Obj.CharSet[9] |= 0x03;

		if (ItemIndex >= 262 && ItemIndex <= 265)
		{
			Obj.CharSet[16] |= (0x04 * (ItemIndex - 262));
		}

		if (ItemIndex == 266 || ItemIndex == 268)
		{
			Obj.CharSet[16] |= 0x10;
		}

		if (ItemIndex == 267)
		{
			Obj.CharSet[16] |= 0x14;
		}

		if (ItemIndex == 269)
		{
			Obj.CharSet[16] |= 0x1C;
		}
	}

	else if (ItemIndex == 30)
	{
		Obj.CharSet[9] |= 0x03;
		Obj.CharSet[16] |= 0x18;
	}

	else if (ItemIndex == 270)
	{
		Obj.CharSet[9] |= 0x04;
	}

	else if (ItemIndex == 278)
	{
		Obj.CharSet[9] |= 0x04;
		Obj.CharSet[16] |= 0x04;
	}

	itemindex = Obj.pntInventory[8]->m_Type;

	switch (itemindex)
	{
	case ITEMGET(13, 64):
		Obj.CharSet[16] |= 0x20;
		break;
	case ITEMGET(13, 65):
		Obj.CharSet[16] |= 0x40;
		break;
	case ITEMGET(13, 67):
		Obj.CharSet[10] |= 0x01;
		Obj.CharSet[16] |= 0x80;
		break;
	case ITEMGET(13, 80):
		Obj.CharSet[16] |= 0xE0;
		break;
	case ITEMGET(13, 106):
		Obj.CharSet[16] |= 0xA0;
		break;
	case ITEMGET(13, 123):
		Obj.CharSet[16] |= 0x60;
		break;
	default:
		break;
	}
}

void gObjViewportPaint(HWND hWnd)
{

	int n;
	HDC hdc;
	char szTemp[256];
	int count = 0;
	int count2 = 0;
	int count3 = 0;
	int playerc = 0;
	int totalplayer = 0;

	hdc = GetDC(hWnd);

	for (n = 0; n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		if (getGameObject(n)->Live != FALSE)
		{
			if (getGameObject(n)->Connected != PLAYER_EMPTY)
			{
				if (gCurPaintMapNumber == getGameObject(n)->MapNumber)
				{
					if (getGameObject(n)->Type == OBJ_USER && getGameObject(n)->m_PlayerData->ISBOT == false)
					{
						playerc++;
					}
				}
			}
		}

		if (getGameObject(n)->Type == OBJ_USER && getGameObject(n)->Connected != PLAYER_EMPTY && getGameObject(n)->m_PlayerData->ISBOT == false)
		{
			totalplayer++;
		}
		else if (getGameObject(n)->Connected != PLAYER_EMPTY)
		{
			count++;
		}
	}

	if (gCurPaintType == 1)
	{
		RECT rect;
		int iStartX = 150;
		int iStartY = 100;
		int iWidth = 1;
		int iHeight = 1;
		int iMagnifying = 3;
		int x;
		int y;

		hdc = GetDC(hWnd);

		HBRUSH hCharacterBrush = CreateSolidBrush(RGB(255, 0, 0));
		HBRUSH hMonsterBrush = CreateSolidBrush(RGB(255, 128, 0));
		HBRUSH hNpcBrush = CreateSolidBrush(RGB(0, 255, 255));
		HBRUSH hItemBrush = CreateSolidBrush(RGB(0, 0, 255));
		HBRUSH hCrywolfMovePath = CreateSolidBrush(RGB(0, 255, 0));
		int iOldBkMode = SetBkMode(hdc, TRANSPARENT);

		// Set a Gray backgraound on non-walkable areas
		if (MapC[gCurPaintMapNumber].m_attrbuf != NULL)
		{
			for (y = 0; y < 255; y++)
			{
				for (x = 0; x < 255; x++)
				{
					if ((MapC[gCurPaintMapNumber].m_attrbuf[y * 256 + x] & 4) == 4 || (MapC[gCurPaintMapNumber].m_attrbuf[y * 256 + x] & 8) == 8)
					{
						rect.left = iStartX + y * iWidth*iMagnifying;
						rect.right = (iStartX + y * iWidth*iMagnifying) + (iWidth*iMagnifying);
						rect.top = iStartY + x * iHeight*iMagnifying;
						rect.bottom = (iStartY + x * iHeight*iMagnifying) + (iHeight*iMagnifying);

						FillRect(hdc, &rect, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
					}
				}
			}
		}
		//#error message("put else if gObjViewportPaint")
		//27F1C
		if (TMonsterAIElement::s_MonsterAIMovePath[MAP_INDEX_CRYWOLF_FIRSTZONE].m_bDataLoad != FALSE && gCurPaintMapNumber == 34)
		{
			TMonsterAIMovePath & MovePath = TMonsterAIElement::s_MonsterAIMovePath[MAP_INDEX_CRYWOLF_FIRSTZONE];

			for (int i = 0; i < MovePath.m_iMovePathSpotCount; i++)
			{
				rect.left = iStartX + MovePath.m_MovePathInfo[i].m_iPathY*iWidth*iMagnifying;
				rect.right = (iStartX + MovePath.m_MovePathInfo[i].m_iPathY*iWidth*iMagnifying) + (iWidth*iMagnifying);
				rect.top = iStartY + MovePath.m_MovePathInfo[i].m_iPathX*iHeight*iMagnifying;
				rect.bottom = (iStartY + MovePath.m_MovePathInfo[i].m_iPathX*iHeight*iMagnifying) + (iHeight*iMagnifying);

				FillRect(hdc, &rect, hCrywolfMovePath);

				wsprintf(szTemp, "(%d,%d)", MovePath.m_MovePathInfo[i].m_iPathX, MovePath.m_MovePathInfo[i].m_iPathY);
			}
		}

		for (n = 0; n < g_ConfigRead.server.GetObjectMax(); n++)
		{
			if (getGameObject(n)->Live != FALSE)
			{
				if (getGameObject(n)->Connected != PLAYER_EMPTY)
				{
					if (gCurPaintMapNumber == getGameObject(n)->MapNumber)
					{
						if (getGameObject(n)->Type == OBJ_USER)
						{
							rect.left = iStartX + getGameObject(n)->Y*iWidth*iMagnifying;
							rect.right = (iStartX + getGameObject(n)->Y*iWidth*iMagnifying) + ((iWidth + 2)*iMagnifying);
							rect.top = iStartY + getGameObject(n)->X*iHeight*iMagnifying;
							rect.bottom = (iStartY + getGameObject(n)->X*iHeight*iMagnifying) + ((iHeight + 2)*iMagnifying);

							FillRect(hdc, &rect, hCharacterBrush);

							wsprintf(szTemp, "%s (%d,%d)", getGameObject(n)->Name, getGameObject(n)->X, getGameObject(n)->Y);
							TextOut(hdc, rect.left, rect.bottom, szTemp, strlen(szTemp));
						}
						else if (getGameObject(n)->Type == OBJ_MONSTER)
						{
							rect.left = iStartX + getGameObject(n)->Y*iWidth*iMagnifying;
							rect.right = (iStartX + getGameObject(n)->Y*iWidth*iMagnifying) + ((iWidth)*iMagnifying);
							rect.top = iStartY + getGameObject(n)->X*iHeight*iMagnifying;
							rect.bottom = (iStartY + getGameObject(n)->X*iHeight*iMagnifying) + ((iHeight)*iMagnifying);

							FillRect(hdc, &rect, hMonsterBrush);

							if (getGameObject(n)->m_iCurrentAI != 0)
							{
								if (getGameObject(n)->m_iGroupMemberGuid == 0)
								{
									FillRect(hdc, &rect, hCharacterBrush);

									wsprintf(szTemp, "%s (%d,%d) HP(%f/%f)", getGameObject(n)->Name, getGameObject(n)->m_iGroupNumber, getGameObject(n)->m_iGroupMemberGuid, getGameObject(n)->Life, getGameObject(n)->MaxLife);
									TextOut(hdc, rect.left, rect.bottom, szTemp, strlen(szTemp));
								}
							}
						}
					}
				}
			}
		}

		for (int n = 0; n < g_ConfigRead.server.GetObjectMaxItem(); n++)
		{
			if (MapC[gCurPaintMapNumber].m_CItemObject[n].IsItem())
			{
				rect.left = iStartX + MapC[gCurPaintMapNumber].m_CItemObject[n].py*iWidth*iMagnifying;
				rect.right = (iStartX + MapC[gCurPaintMapNumber].m_CItemObject[n].py*iWidth*iMagnifying) + (iWidth*iMagnifying);
				rect.top = iStartY + MapC[gCurPaintMapNumber].m_CItemObject[n].px*iHeight*iMagnifying;
				rect.bottom = (iStartY + MapC[gCurPaintMapNumber].m_CItemObject[n].px*iHeight*iMagnifying) + (iHeight*iMagnifying);

				FillRect(hdc, &rect, hItemBrush);

				wsprintf(szTemp, "%s", MapC[gCurPaintMapNumber].m_CItemObject[n].GetName());
				TextOut(hdc, rect.left, rect.bottom, szTemp, strlen(szTemp));
			}
		}

		DeleteObject((HGDIOBJ)hCharacterBrush);
		DeleteObject((HGDIOBJ)hMonsterBrush);
		DeleteObject((HGDIOBJ)hNpcBrush);
		DeleteObject((HGDIOBJ)hItemBrush);
		DeleteObject((HGDIOBJ)hCrywolfMovePath);
		SetBkMode(hdc, iOldBkMode);
	}

	gObjTotalUser = totalplayer;
	gObjTotalMonster = count;

	ReleaseDC(hWnd, hdc);
}



static int FrustrumX[MAX_ARRAY_FRUSTRUM];
static int FrustrumY[MAX_ARRAY_FRUSTRUM];

void InitFrustrum()
{
	float CameraViewFar = 2400.f;	// ???? ??
	float CameraViewNear = CameraViewFar * 0.19f;// ??? ??
	float CameraViewTarget = CameraViewFar * 0.53f;// ??????? ??? ??g
	float WidthFar = 1190.f;// ?????? ???? ??
	float WidthNear = 550.f;// ??????? ???? ??

	vec3_t p[4];
	Vector(-WidthFar, CameraViewFar - CameraViewTarget, 0.f, p[0]);
	Vector(WidthFar, CameraViewFar - CameraViewTarget, 0.f, p[1]);
	Vector(WidthNear, CameraViewNear - CameraViewTarget, 0.f, p[2]);
	Vector(-WidthNear, CameraViewNear - CameraViewTarget, 0.f, p[3]);
	vec3_t Angle;
	float Matrix[3][4];
	Vector(0.f, 0.f, 45.f, Angle);
	AngleMatrix(Angle, Matrix);
	vec3_t Frustrum[4];
	for (int i = 0; i < 4; i++)
	{
		VectorRotate(p[i], Matrix, Frustrum[i]);
		FrustrumX[i] = (int)(Frustrum[i][0] * 0.01f);
		FrustrumY[i] = (int)(Frustrum[i][1] * 0.01f);
	}
}

void InitFrustrumEx() // 1.01.10B GS KOR, changed values
{
	float CameraViewFar = 3200.f;
	float CameraViewNear = CameraViewFar * 0.19f;
	float CameraViewTarget = CameraViewFar * 0.53f;
	float WidthFar = 1390.f;
	float WidthNear = 750.f;

	vec3_t p[4];
	Vector(-WidthFar, CameraViewFar - CameraViewTarget, 0.f, p[0]);
	Vector(WidthFar, CameraViewFar - CameraViewTarget, 0.f, p[1]);
	Vector(WidthNear, CameraViewNear - CameraViewTarget, 0.f, p[2]);
	Vector(-WidthNear, CameraViewNear - CameraViewTarget, 0.f, p[3]);
	vec3_t Angle;
	float Matrix[3][4];
	Vector(0.f, 0.f, 45.f, Angle);
	AngleMatrix(Angle, Matrix);
	vec3_t Frustrum[4];
	for (int i = 0; i < 4; i++)
	{
		VectorRotate(p[i], Matrix, Frustrum[i]);
		FrustrumX[i] = (int)(Frustrum[i][0] * 0.01f);
		FrustrumY[i] = (int)(Frustrum[i][1] * 0.01f);
	}
}




void CreateFrustrum(int x, int y, CGameObject &Obj)
{
	

	for (int i = 0; i < MAX_ARRAY_FRUSTRUM; i++)
	{
		Obj.FrustrumX[i] = FrustrumX[i] + x;
		Obj.FrustrumY[i] = FrustrumY[i] + y;
	}
}





bool TestFrustrum2(int x, int y, CGameObject &Obj)
{
	int j = 3;
	

	for (int i = 0; i < MAX_ARRAY_FRUSTRUM; j = i, i++)
	{
		if ((Obj.FrustrumX[i] - x) * (Obj.FrustrumY[j] - y) - (Obj.FrustrumX[j] - x) * (Obj.FrustrumY[i] - y) < 0.0f)
		{
			return false;
		}
	}
	return true;
}

BOOL gObjCheckViewport(CGameObject &Obj, int x, int y)
{
	

	if (x < Obj.X - 15 || x > Obj.X + 15 || y < Obj.Y - 15 || y > Obj.Y + 15)
	{
		return false;
	}

	if (Obj.m_bBlind == true)
	{
		if (x < Obj.X - 2 || x > Obj.X + 2 || y < Obj.Y - 2 || y > Obj.Y + 2)
		{
			return 2;
		}
	}

	int j = 3;

	for (int i = 0; i < MAX_ARRAY_FRUSTRUM; j = i, i++)
	{
		int frustrum = (Obj.FrustrumX[i] - x) * (Obj.FrustrumY[j] - y) - (Obj.FrustrumX[j] - x) * (Obj.FrustrumY[i] - y);
		if (frustrum < 0)
		{
			return false;
		}
	}
	return true;
}


void gObjViewportClose(CGameObject &Obj)
{
	int tObjNum, i;
	int MVL = MAX_VIEWPORT;

	if (Obj.Type == OBJ_MONSTER)
	{
		MVL = MaxViewportMonster;
	}

	if (Obj.Connected == PLAYER_PLAYING)
	{
		for (int n = 0; n < MVL; n++)
		{
			if (Obj.VpPlayer2[n].state != 0)
			{
				if (Obj.VpPlayer2[n].type != 5)
				{
					tObjNum = Obj.VpPlayer2[n].number;

					i = 0;

					while (true)
					{
						if (getGameObject(tObjNum)->VpPlayer[i].number == Obj.m_Index)
						{
							if (getGameObject(tObjNum)->VpPlayer[i].type == Obj.Type)
							{
								getGameObject(tObjNum)->VpPlayer[i].state = 3;
								break;
							}
						}

						if (i < MVL - 1)
						{
							i++;
						}
						else
						{
							break;
						}
					}

					i = 0;

					while (true)
					{
						if (getGameObject(tObjNum)->VpPlayer2[i].number == Obj.m_Index)
						{
							if (getGameObject(tObjNum)->VpPlayer2[i].type == Obj.Type)
							{
								getGameObject(tObjNum)->VpPlayer2[i].state = 0;
								break;
							}
						}

						if (i < MVL - 1)
						{
							i++;
						}
						else
						{
							break;
						}
					}
				}
			}
		}
	}
	gObjClearViewport(Obj);
}



#define MP 1

inline void gObjViewportListCreate(CGameObject &Obj)
{
	int result;
	int mapnum;

	if (Obj.Connected != PLAYER_PLAYING)
	{
		return;
	}

	if (Obj.RegenOk > 0)
	{
		return;
	}

	mapnum = Obj.MapNumber;
	gItemLoop2 = 0;

	if (Obj.Type == OBJ_USER)
	{
		MapClass * lpMap = &MapC[mapnum];

		for (int n = 0; n < g_ConfigRead.server.GetObjectMaxItem(); n++)
		{
			if (lpMap->m_CItemObject[n].live)
			{
				gItemLoop2++;

				if (lpMap->m_CItemObject[n].m_State == 1 || lpMap->m_CItemObject[n].m_State == 2)
				{
					if (gObjCheckViewport(Obj, lpMap->m_CItemObject[n].px, lpMap->m_CItemObject[n].py) == 1)
					{
						result = ViewportAdd(Obj, *getGameObject(n), 5);
					}
				}
			}
		}
	}

	if (&Obj == NULL)
	{
		gItemLoopMax = gItemLoop2;
		gItemLoop = gItemLoop2;
	}

	CGameObject* lpTempObj;

	if (Obj.Type == OBJ_USER)
	{

#if (MP==1)
#pragma omp parallel for
#endif
		for (int n = 0; n < g_ConfigRead.server.GetObjectMax(); n++)
		{
			lpTempObj = getGameObject(n);

			if (lpTempObj->Connected == PLAYER_PLAYING && Obj.m_Index != n)
			{
				if (lpTempObj->m_State == 1 || lpTempObj->m_State == 2)
				{
					if (mapnum == lpTempObj->MapNumber)
					{
						if (gObjCheckViewport(Obj, lpTempObj->X, lpTempObj->Y) == 1)
						{
							result = ViewportAdd(Obj, *lpTempObj, lpTempObj->Type);
							result = ViewportAdd2(Obj, *lpTempObj, Obj.Type);
						}
					}
				}
			}
		}
	}
	else if (Obj.Type == OBJ_MONSTER || Obj.Type == OBJ_NPC)
	{

#if (MP==1)
#pragma omp parallel for
#endif
		for (int n = g_ConfigRead.server.GetObjectMaxMonster(); n < g_ConfigRead.server.GetObjectMax(); n++)
		{
			lpTempObj = getGameObject(n);

			if (lpTempObj->Connected == PLAYER_PLAYING && Obj.m_Index != n)
			{
				if (lpTempObj->m_State == 1 || lpTempObj->m_State == 2)
				{
					if (mapnum == lpTempObj->MapNumber)
					{
						if (gObjCheckViewport(Obj, lpTempObj->X, lpTempObj->Y) == 1)
						{
							result = ViewportAdd(Obj, *lpTempObj, getGameObject(n)->Type);
							result = ViewportAdd2(Obj, *lpTempObj, Obj.Type);
						}
					}
				}
			}
		}
	}
}



int  ViewportAdd(CGameObject &Obj, CGameObject &ObjTarget, int aType)
{
	int blank = -1;
	int count = 0;

	if (ObjTarget.Class == 523 || ObjTarget.Class == 603)
	{
		return -1;
	}

	int MVL = MAX_VIEWPORT;

	if (Obj.Type == OBJ_MONSTER)
	{
		MVL = MaxViewportMonster;
	}

	for (int n = 0; n < MVL; n++)
	{
		if (Obj.VpPlayer[n].state != 0)
		{
			if (Obj.VpPlayer[n].number == ObjTarget.m_Index)
			{
				if (Obj.VpPlayer[n].type == aType)
				{
					return -1;
				}
			}
		}
		else
		{
			if (blank < 0)
			{
				blank = n;
			}
		}
	}

	if (blank < 0)
	{
		return -1;
	}

	Obj.VpPlayer[blank].state = 1;
	Obj.VpPlayer[blank].number = ObjTarget.m_Index;
	Obj.VpPlayer[blank].type = aType;
	Obj.VPCount++;

	if (Obj.Type == OBJ_MONSTER || Obj.Type == OBJ_NPC)
	{
		if (aType == 1)
		{
			if (Obj.m_iCurrentAI != 0)
			{
				Obj.m_Agro.SetAgro(ObjTarget, 1000);
			}
		}
	}
	return blank;
}




int  ViewportAdd2(CGameObject &Obj, CGameObject &ObjTarget, int aType)
{
	int blank = -1;
	int count = 0;
	int MVL = MAX_VIEWPORT;

	if (Obj.Type == OBJ_MONSTER)
	{
		MVL = MaxViewportMonster;
	}

	for (int n = 0; n < MVL; n++)
	{
		if (Obj.VpPlayer2[n].state != 0)
		{
			if (Obj.VpPlayer2[n].number == ObjTarget.m_Index)
			{
				if (Obj.VpPlayer2[n].type == aType)
				{
					return -1;
				}
			}
		}
		else
		{
			if (blank < 0)
			{
				blank = n;
			}
		}
	}

	if (blank < 0)
	{
		return -1;
	}

	Obj.VpPlayer2[blank].state = 1;
	Obj.VpPlayer2[blank].number = ObjTarget.m_Index;
	Obj.VpPlayer2[blank].type = aType;
	Obj.VPCount2++;

	return blank;
}




void ViewportDel(CGameObject &Obj, int aDelIndex)
{
	if (gObjIsConnected(Obj) == 0)
	{
		return;
	}

	if (gObjIsConnected(*getGameObject(aDelIndex)) == 0)
	{
		return;
	}

	
	int MVL = MAX_VIEWPORT;

	if (Obj.Type == OBJ_MONSTER)
	{
		MVL = MaxViewportMonster;
	}

	for (int n = 0; n < MVL; n++)
	{
		if (Obj.VpPlayer[n].state == 1 || Obj.VpPlayer[n].state == 2)
		{
			if (Obj.VpPlayer[n].number == aDelIndex)
			{
				Obj.VpPlayer[n].state = 0;
				Obj.VpPlayer[n].number = -1;
				Obj.VPCount--;
			}
		}
	}
}




void Viewport2Del(CGameObject &Obj, int aDelIndex)
{
	if (gObjIsConnected(Obj) == 0)
	{
		return;
	}

	if (gObjIsConnected(*getGameObject(aDelIndex)) == 0)
	{
		return;
	}

	
	int MVL = MAX_VIEWPORT;

	if (Obj.Type == OBJ_MONSTER)
	{
		MVL = MaxViewportMonster;
	}

	for (int n = 0; n < MVL; n++)
	{
		if (Obj.VpPlayer2[n].state == 1 || Obj.VpPlayer2[n].state == 2)
		{
			if (Obj.VpPlayer2[n].number == aDelIndex)
			{
				Obj.VpPlayer2[n].state = 0;
				Obj.VpPlayer2[n].number = -1;
				Obj.VPCount2--;
			}
		}
	}
}




void gObjViewportAllDel(CGameObject &Obj)
{
	if (gObjIsConnected(Obj) == 0)
	{
		return;
	}
	
	int MVL = MAX_VIEWPORT;

	if (Obj.Type == OBJ_MONSTER)
	{
		MVL = MaxViewportMonster;
	}

	for (int n = 0; n < MVL; n++)
	{
		if (Obj.VpPlayer[n].state == 1 || Obj.VpPlayer[n].state == 2)
		{
			Viewport2Del(*Obj.VpPlayer[n].user, Obj.m_Index);
		}

		if (Obj.VpPlayer2[n].state == 1 || Obj.VpPlayer2[n].state == 2)
		{
			ViewportDel(*Obj.VpPlayer2[n].user, Obj.m_Index);
		}
	}
}




void gObjViewportListDestroy(CGameObject &Obj)
{
	short tObjNum;
	int n;
	CGameObject lpObj;
	int MVL;

	if (Obj.Connected != PLAYER_PLAYING)
	{
		return;
	}

	lpObj = lpObj;
	MVL = MAX_VIEWPORT;

	if (Obj.Type == OBJ_MONSTER)
	{
		MVL = MaxViewportMonster;
	}

	for (n = 0; n < MVL; n++)
	{
		int mapnum;
		if (Obj.VpPlayer[n].state == 1 || Obj.VpPlayer[n].state == 2)
		{
			tObjNum = Obj.VpPlayer[n].number;
			CGameObject* lpObjTarget = getGameObject(tObjNum);

			if (ObjectMaxRange(tObjNum) == false)
			{
				continue;
			}

			switch (Obj.VpPlayer[n].type)
			{
			case 5:
				mapnum = Obj.MapNumber;

				if (MapC[mapnum].m_CItemObject[tObjNum].IsItem() == 1)
				{
					if (MapC[mapnum].m_CItemObject[tObjNum].m_State == 8)
					{
						Obj.VpPlayer[n].state = 3;
					}
					else
					{
						if (gObjCheckViewport(Obj, MapC[mapnum].m_CItemObject[tObjNum].px, MapC[mapnum].m_CItemObject[tObjNum].py) == 0)
						{
							Obj.VpPlayer[n].state = 3;
						}
					}
				}
				else
				{
					if (MapC[mapnum].m_CItemObject[tObjNum].live == 0 || MapC[mapnum].m_CItemObject[tObjNum].Give == 0)
					{
						Obj.VpPlayer[n].state = 3;
					}
					else
					{
						Obj.VpPlayer[n].state = 3;
					}
				}
				break;
			default:
				if (lpObjTarget->m_State == 1)
				{
					if (lpObjTarget->Live == 0)
					{
						Obj.VpPlayer[n].state = 3;
					}
				}

				if (lpObjTarget->Connected == PLAYER_EMPTY ||
					lpObjTarget->m_State == 8 ||
					lpObjTarget->Teleport != 0 ||
					lpObjTarget->m_State == 32)
				{
					Obj.VpPlayer[n].state = 3;
				}
				else
				{
					bool bDelete = false;

					if (Obj.m_bBlind == true)
					{
						if (gObjCheckViewport(Obj, lpObjTarget->X, lpObjTarget->Y) != 1)
						{
							bDelete = true;
						}
					}

					else if (!gObjCheckViewport(Obj, lpObjTarget->X, lpObjTarget->Y))
					{
						bDelete = true;
					}

					if (bDelete)
					{
						Obj.VpPlayer[n].state = 3;

						if (Obj.Type == OBJ_MONSTER || Obj.Type == OBJ_NPC)
						{
							if (lpObjTarget->Type == OBJ_USER)
							{
								if (Obj.m_iCurrentAI != 0)
								{
									Obj.m_Agro.DelAgro(*lpObjTarget);
								}
							}
						}
					}
				}
				break;
			}
		}

		if (Obj.VpPlayer2[n].state == 1 || Obj.VpPlayer2[n].state == 2)
		{
			tObjNum = Obj.VpPlayer2[n].number;
			CGameObject* lpObjTarget = getGameObject(tObjNum);

			if (lpObjTarget->Connected < PLAYER_PLAYING)
			{
				Obj.VpPlayer2[n].state = 0;
				Obj.VpPlayer2[n].number = -1;
				Obj.VPCount2--;
			}
			else
			{
				if (gObjCheckViewport(*lpObjTarget, Obj.X, Obj.Y) == 0)
				{
					Obj.VpPlayer2[n].state = 0;
					Obj.VpPlayer2[n].number = -1;
					Obj.VPCount2--;
				}
			}
		}
	}
}

void gObjStateSetCreate(CGameObject &Obj)
{
	

	if (Obj.Connected < PLAYER_PLAYING)
	{
		return;
	}

	DWORD dwNowTick = GetTickCount();

	if (Obj.DieRegen == 1)
	{
		if (dwNowTick - Obj.RegenTime > Obj.MaxRegenTime + 1000)
		{
			gObjClearBuffEffect(Obj, CLEAR_TYPE_NON_PCS_ITEM_EFFECT);
			Obj.DieRegen = 2;
			Obj.m_State = 8;
		}
	}

	if (Obj.Type == OBJ_USER)
	{
		if (Obj.m_State == 8)
		{
			gObjReady4Relife(Obj);
		}
		else
		{
			gObjCheckTimeOutValue(Obj, (DWORD &)dwNowTick);
		}
	}

	if (Obj.Teleport == 1)
	{
		if (Obj.DieRegen != 0)
		{
			Obj.Teleport = 0;
		}
		else if (GetTickCount() - Obj.TeleportTime > 100)
		{
			Obj.Teleport = 2;
		}
	}

	else if (Obj.Teleport == 2)
	{
		Obj.Teleport = 3;

		if (Obj.Type == OBJ_USER)
		{
			Obj.TX = Obj.X;
			Obj.TY = Obj.Y;

			CreateFrustrum(Obj.X, Obj.Y, Obj);
			gGameProtocol.GCTeleportSend(Obj, 0, Obj.MapNumber, Obj.X, Obj.Y, Obj.Dir);
			gObjViewportListProtocolCreate(Obj);
			GCItemObjectListSend(Obj); // avoid making copy of item in inventory after teleport

			if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
			{
				if (Obj.MapNumber == MAP_INDEX_CASTLESIEGE)
				{
					if (g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
					{
						g_CastleSiege.NotifySelfCsJoinSide(Obj);
						g_CastleSiege.NotifyCsSelfLeftTime(Obj);
					}
				}
			}

			Obj.Teleport = 0;
		}

		else if (Obj.Type == OBJ_NPC && Obj.Class == 229)
		{
			int x = Obj.X;
			int y = Obj.Y;

			Obj.X = x;
			Obj.Y = y;

			Obj.TX = Obj.X;
			Obj.TY = Obj.Y;

			CreateFrustrum(Obj.X, Obj.Y, Obj);

			Obj.m_State = 1;
			Obj.PathStartEnd = 0;
		}
		else if (Obj.Type == OBJ_MONSTER)
		{
			Obj.TX = Obj.X;
			Obj.TY = Obj.Y;

			CreateFrustrum(Obj.X, Obj.Y, Obj);

			gObjViewportListCreate(Obj);
			gObjViewportListProtocolCreate(Obj);

			Obj.Teleport = 0;
			Obj.PathStartEnd = 0;
			Obj.PathCur = 0;
			Obj.PathCount = 0;
		}
		else
		{
			int x, y;

			if (Obj.m_RecallMon >= 0)
			{
				x = Obj.X;
				y = Obj.Y;
			}
			else
			{
				x = Obj.X + rand() % 6 - 3;
				y = Obj.Y + rand() % 6 - 3;
			}

			Obj.X = x;
			Obj.Y = y;

			Obj.TX = Obj.X;
			Obj.TY = Obj.Y;

			if (Obj.m_RecallMon >= 0)
			{
				gObjViewportListProtocolCreate(Obj);
				Obj.PathStartEnd = 0;
			}
		}
	}

	if (Obj.RegenOk == 2 && Obj.Type == OBJ_USER)
	{
		Obj.m_State = 1;
		Obj.RegenOk = 3;
		Obj.X = Obj.RegenMapX;
		Obj.Y = Obj.RegenMapY;
		Obj.MapNumber = Obj.RegenMapNumber;

		Obj.TX = Obj.X;
		Obj.TY = Obj.Y;

		CreateFrustrum(Obj.X, Obj.Y, Obj);

		if (DragonEvent->GetState() > 0)
		{
			if (DragonEvent->GetMapNumber() == Obj.MapNumber)
			{
				gGameProtocol.GCMapEventStateSend(Obj.MapNumber, 1, 1);
			}
		}

		if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
		{
			if (Obj.MapNumber == MAP_INDEX_CASTLESIEGE)
			{
				gGameProtocol.GCAnsCsNotifyStart(Obj, CASTLESIEGECHECKSTATE(g_CastleSiege.GetCastleState()));

				if (g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					g_CastleSiege.NotifySelfCsJoinSide(Obj);
					g_CastleSiege.NotifyCsSelfLeftTime(Obj);
				}
			}
		}

		gEledoradoEvent.CheckGoldDercon(Obj.MapNumber);

	}
}
#pragma pack (1)
struct PMSG_CHARREGEN
{
	PBMSG_HEAD2 h;
	BYTE subcode;
	BYTE MapX;
	BYTE MapY;
	WORD MapNumber;
	WORD Dir;
	WORD Life;
	WORD Mana;
	WORD wShield;
	WORD BP;
	WORD unk;
	BYTE ExpHHH; //
	BYTE ExpHHL; //
	BYTE ExpHLH; //
	BYTE ExpHLL; //
	BYTE ExpLHH; //
	BYTE ExpLHL; //
	BYTE ExpLLH; //
	BYTE ExpLLL; //

	DWORD Money;
	int IGCLife;
	int IGCMana;
};
#pragma pack ()

void gObjSetState()
{
	int n;
	CGameObject* lpObj;

	for (int n = 0; n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		lpObj = getGameObject(n);

		if (lpObj->Connected > PLAYER_LOGGED)
		{
			if (lpObj->m_State == 1)
			{
				lpObj->m_State = 2;
				if (lpObj->RegenOk == 3)
				{
					lpObj->RegenOk = 0;
				}
			}

			if (lpObj->Teleport == 3)
			{
				lpObj->Teleport = 0;
			}

			if (lpObj->DieRegen == 2)
			{
				if (lpObj->m_bGMSummon)
				{
					gObjDel(*lpObj);
					continue;
				}

				int mapnum = lpObj->MapNumber;
				int x = lpObj->X;
				int y = lpObj->Y;

				lpObj->m_bAttackerKilled = 0;

				if (DS_MAP_RANGE(lpObj->MapNumber))
				{
					if (lpObj->Type >= OBJ_MONSTER)
					{
						g_DevilSquare.gDevilSquareMonsterRegen(*lpObj);
						continue;
					}
				}

				if (lpObj->MapNumber == MAP_INDEX_DEVILSQUARE_FINAL)
				{
					if (lpObj->Type >= OBJ_MONSTER)
					{
						g_DevilSquareFinal.DSFMonsterRegen(*lpObj);
						continue;
					}
				}

				if (lpObj->m_iCurrentAI != 0 && lpObj->m_iRegenType != 0)
					continue;

				if (lpObj->MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE)
				{
					if (g_CrywolfSync.GetCrywolfState() == 3 && lpObj->Type >= OBJ_MONSTER)
						continue;
				}

				if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
				{
					if (lpObj->MapNumber == MAP_INDEX_KANTURU_BOSS && lpObj->Type == OBJ_MONSTER)
					{
						continue;
					}
				}

				if (lpObj->m_iCurrentAI != 0 && lpObj->m_iRegenType != 0)
				{
					continue;
				}

				if (lpObj->Type >= 586 && lpObj->Type <= 602)
				{
					if (g_ArcaBattle.IsArcaBattleServer() == TRUE)
					{
						continue;
					}
				}

				if (lpObj->Type >= 627 && lpObj->Type <= 641)
				{
					if (g_ArcaBattle.IsArcaBattleServer() == TRUE)
					{
						continue;
					}
				}

				if (lpObj->Type >= 504 && lpObj->Type <= 521)
				{
					continue;
				}

				if (lpObj->Type >= 523 && lpObj->Type <= 528)
				{
					continue;
				}

				if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
				{
					if (lpObj->MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE)
					{
						if (g_CrywolfSync.GetCrywolfState() == 3)
						{
							if (lpObj->Type >= OBJ_MONSTER)
							{
								continue;
							}
						}
					}
				}

				if (lpObj->m_bIsInMonsterHerd != 0)
				{
					if (lpObj->m_lpMonsterHerd != 0)
					{
						lpObj->m_lpMonsterHerd->MonsterRegenAction(*lpObj);
					}
					continue;
				}

				if (BC_MAP_RANGE(lpObj->MapNumber) && lpObj->Type >= OBJ_MONSTER)
				{
					int iBridgeIndex = g_BloodCastle.GetBridgeIndex(lpObj->MapNumber);

					if (lpObj->Type == OBJ_MONSTER)
					{
						if (lpObj->Class != 131 && BC_STATUE_RANGE(lpObj->Class - 132) == 0)
						{
							g_BloodCastle.ChangeMonsterState(iBridgeIndex, *lpObj);
						}

						else
						{
							gObjDel(*lpObj);
							continue;
						}
					}
				}

				if (lpObj->Class == 541 || lpObj->Class == 542)
				{
					g_DoppelGanger.DelTreasureBox(lpObj->m_Index, lpObj->Class);
					continue;
				}

				if (lpObj->m_Attribute == 60)
				{
					gObjDel(*lpObj);
					continue;
				}

				if (lpObj->Class >= 678 && lpObj->Class <= 680)
				{
					gObjDel(*lpObj);
					continue;
				}

				if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
				{
					if (lpObj->m_Attribute == 61)
					{
						gObjDel(*lpObj);
						continue;
					}

					if (lpObj->m_Attribute == 62)
					{
						switch (lpObj->Class)
						{
						case 295:
						case 300:
						case 301:
						case 302:
						case 303:
							gObjDel(*lpObj);
							continue;
						}

						if (lpObj->m_dwLastCheckTick + 600000 < GetTickCount())
						{
							gObjDel(*lpObj);
							continue;
						}
					}
				}

				if (lpObj->Type >= OBJ_MONSTER)
				{
					if (lpObj->Class == 287 || lpObj->Class == 286)
					{
						g_CsNPC_Mercenary.DeleteMercenary(*lpObj);
						gObjDel(*lpObj);
						continue;
					}
				}

				if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
				{
					if (lpObj->Type >= OBJ_MONSTER)
					{
						if (lpObj->Class == 278)
						{
							g_CsNPC_LifeStone.DeleteLifeStone(*lpObj);
							gObjDel(*lpObj);
							continue;
						}
					}
				}

				int State = 1;

				if (lpObj->Type >= OBJ_MONSTER)
				{
					if ((lpObj->Class >= 504 && lpObj->Class <= 521) || (lpObj->Class >= 523 && lpObj->Class <= 528))
					{
						State = 0;
						lpObj->Life = lpObj->MaxLife;
						lpObj->DieRegen = 0;
					}

					if (lpObj->Class >= 586 && lpObj->Class <= 602 && g_ArcaBattle.IsArcaBattleServer() == TRUE)
					{
						State = 0;
						lpObj->DieRegen = 0;
					}

					if (lpObj->Class >= 627 && lpObj->Class <= 641 && g_ArcaBattle.IsArcaBattleServer() == TRUE)
					{
						State = 0;
						lpObj->DieRegen = 0;
					}

					if (lpObj->Class >= 560 && lpObj->Class <= 561)
					{
						State = 0;
						lpObj->DieRegen = 0;
					}
				}

				PMSG_CHARREGEN pMsg;

				if (State != 0)
				{
					lpObj->Life = lpObj->MaxLife + lpObj->AddLife;
					lpObj->Mana = lpObj->MaxMana + lpObj->AddMana;
					lpObj->iShield = lpObj->iMaxShield + lpObj->iAddShield;

					lpObj->Live = 1;

					gObjRemoveBuffEffect(*lpObj, BUFFTYPE_POISON);
					gObjRemoveBuffEffect(*lpObj, BUFFTYPE_FREEZE);
					gObjRemoveBuffEffect(*lpObj, BUFFTYPE_STONE);

					lpObj->Teleport = 0;
					lpObj->m_ViewState = 0;

					for (int i = 0; i < MAX_SELF_DEFENSE; i++)
					{
						lpObj->SelfDefenseTime[i] = 0;
					}

					gObjTimeCheckSelfDefense(*lpObj);

					gObjClearBuffEffect(*lpObj, CLEAR_TYPE_NON_PCS_ITEM_EFFECT);
					gObjClearViewport(*lpObj);

					if (lpObj->Type == OBJ_USER)
					{
						if (g_iUseCharacterAutoRecuperationSystem != 0 && lpObj->Level <= g_iCharacterRecuperationMaxLevel)
						{
							lpObj->m_iAutoRecuperationTime = GetTickCount();
						}
						else
						{
							lpObj->m_iAutoRecuperationTime = 0;
						}

						lpObj->dwShieldAutoRefillTimer = GetTickCount();

						if (lpObj->m_PlayerData->ISBOT == false && lpObj->m_Index >= g_ConfigRead.server.GetObjectStartUserIndex())
						{
							gDarkSpirit[lpObj->m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].SetMode(CDarkSpirit::ePetItem_Mode::PetItem_Mode_Normal, -1);
						}

						if (lpObj->KillerType == 3)
						{
							if (!g_NewPVP.IsDuel(*lpObj))
							{
								MapC[lpObj->MapNumber].GetRandomLengthPos(lpObj->X, lpObj->Y, 18);
							}
						}

						else if (lpObj->KillerType == 2)
						{
							int w_Type = 0;

							if (lpObj->m_PlayerData->lpGuild != 0)
							{
								if (lpObj->m_PlayerData->lpGuild->WarType == 1)
								{
									w_Type = 1;
								}
							}

							if (w_Type == 0)
							{
								MapC[lpObj->MapNumber].GetRandomLengthPos((short &)lpObj->X, (short &)lpObj->Y, 18);
							}
							else if (lpObj->m_PlayerData->lpGuild->WarState)
							{
								int BattleGroundIndex = lpObj->m_PlayerData->lpGuild->BattleGroundIndex;
								gBattleGetTeamPosition(BattleGroundIndex, lpObj->m_PlayerData->lpGuild->BattleTeamCode, (short &)lpObj->X, (short &)lpObj->Y);
							}
						}

						else if (lpObj->MapNumber == 6 && lpObj->m_PlayerData->RegisterdLMS == TRUE)
						{
							int room = lpObj->m_PlayerData->RegisteredLMSRoom;

							for (int i = 0; i < g_LastManStanding.m_Rooms[room].regCount; i++)
							{
								if (lpObj->m_Index == g_LastManStanding.m_Rooms[room].m_Data[i].iIndex)
								{
									int xrand = g_LastManStanding.m_Rooms[room].X2 - g_LastManStanding.m_Rooms[room].X1;
									int yrand = g_LastManStanding.m_Rooms[room].Y2 - g_LastManStanding.m_Rooms[room].Y1;
									lpObj->MapNumber = 6;
									lpObj->X = g_LastManStanding.m_Rooms[room].X1 + rand() % xrand;
									lpObj->Y = g_LastManStanding.m_Rooms[room].Y1 + rand() % yrand;
									MsgOutput(*lpObj, Lang.GetText(0, 413));
								}
							}
						}

						else if (g_MapAttr.CheckUserRegen(lpObj->MapNumber) == true)
						{

						}

						else if (lpObj->MapNumber == MAP_INDEX_RORENCIA || lpObj->MapNumber == MAP_INDEX_DUNGEON)
						{
							lpObj->MapNumber = MAP_INDEX_RORENCIA;
							MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, (short &)lpObj->X, (short &)lpObj->Y);
						}
						else if (DS_MAP_RANGE(lpObj->MapNumber))
						{
							g_DevilSquare.DieProcDevilSquare(*lpObj);
							lpObj->MapNumber = MAP_INDEX_NORIA;
							MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, (short &)lpObj->X, (short &)lpObj->Y);
						}
						else if (lpObj->MapNumber == MAP_INDEX_ICARUS)
						{
							lpObj->MapNumber = MAP_INDEX_DEVIAS;
							MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, (short &)lpObj->X, (short &)lpObj->Y);
						}

						else if (BC_MAP_RANGE(lpObj->MapNumber))
						{
							lpObj->MapNumber = MAP_INDEX_DEVIAS;
							MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, (short &)lpObj->X, (short &)lpObj->Y);
						}

						else if (CC_MAP_RANGE(lpObj->MapNumber) && g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
						{
							lpObj->MapNumber = MAP_INDEX_LORENMARKET;
							MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, (short &)lpObj->X, (short &)lpObj->Y);
						}

						else if (CC_MAP_RANGE(lpObj->MapNumber))
						{
							lpObj->MapNumber = MAP_INDEX_DEVIAS;
							MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, (short &)lpObj->X, (short &)lpObj->Y);
						}
						else if (lpObj->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
						{
							lpObj->MapNumber = MAP_INDEX_LORENMARKET;
							MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, (short &)lpObj->X, (short &)lpObj->Y);
						}
						else if (KALIMA_MAP_RANGE(lpObj->MapNumber))
						{
							lpObj->MapNumber = MAP_INDEX_DEVIAS;
							MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, (short &)lpObj->X, (short &)lpObj->Y);
						}
						else if (lpObj->MapNumber == MAP_INDEX_AIDA)
						{
							MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, (short &)lpObj->X, (short &)lpObj->Y);
						}

						else if (lpObj->MapNumber == MAP_INDEX_BARRACKS || lpObj->MapNumber == MAP_INDEX_REFUGEE)
						{
							int mgt = 118;
							short x, y, level;
							BYTE map = lpObj->MapNumber, dir;
							int result = gGateC.GetGate(mgt, (short &)x, (short &)y, (BYTE &)map, (BYTE &)dir, (short &)level);

							if (result >= 0)
							{
								lpObj->MapNumber = map;
								lpObj->X = x;
								lpObj->Y = y;
							}
						}

						else if (lpObj->MapNumber == MAP_INDEX_ARENA && lpObj->IsInBattleGround == false)
						{
							int mgt = 50;
							short x, y, level;
							BYTE map = lpObj->MapNumber, dir;
							int result = gGateC.GetGate(mgt, (short &)x, (short &)y, (BYTE &)map, (BYTE &)dir, (short &)level);

							if (result >= 0)
							{
								lpObj->MapNumber = map;
								lpObj->X = x;
								lpObj->Y = y;
							}
						}

						else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && lpObj->MapNumber == MAP_INDEX_CASTLEHUNTZONE)
						{
							int mgt = 106;
							short x, y, level;
							BYTE map = lpObj->MapNumber, dir;

							int result = gGateC.GetGate(mgt, (short &)x, (short &)y, (BYTE &)map, (BYTE &)dir, (short &)level);

							if (result >= 0)
							{
								lpObj->MapNumber = map;
								lpObj->X = x;
								lpObj->Y = y;
							}
						}

						else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && lpObj->MapNumber == MAP_INDEX_CASTLESIEGE)
						{
							int mgt;
							short x, y, level;

							if (!g_CsNPC_LifeStone.SetReSpawnUserXY(*lpObj))
							{
								mgt = 100;

								if (lpObj->m_btCsJoinSide == 1)
								{
									mgt = 105;
								}
								else
								{
									mgt = 100;
								}

								BYTE map = lpObj->MapNumber, dir;
								int result = gGateC.GetGate(mgt, (short &)x, (short &)y, (BYTE &)map, (BYTE &)dir, (short &)level);

								if (result >= 0)
								{
									lpObj->MapNumber = map;
									lpObj->X = x;
									lpObj->Y = y;
								}
							}
						}

						else if (IT_MAP_RANGE(lpObj->MapNumber))
						{
							BYTE btTeam = g_IT_Event.GetUserTeam(lpObj->MapNumber, lpObj->m_Index);

							if (btTeam == 0xFF)
							{
								lpObj->MapNumber = MAP_INDEX_LORENMARKET;
								MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, (short &)lpObj->X, (short &)lpObj->Y);
							}

							else if (btTeam == 0)
							{
								int mgt = lpObj->MapNumber + 103;
								short x, y, level;
								BYTE map = lpObj->MapNumber, dir;
								int result = gGateC.GetGate(mgt, (short &)x, (short &)y, (BYTE &)map, (BYTE &)dir, (short &)level);

								if (result >= 0)
								{
									lpObj->MapNumber = map;
									lpObj->X = x;
									lpObj->Y = y;
								}

								g_IT_Event.IllusionTempleUserDie(*lpObj);
							}

							else if (btTeam == 1)
							{
								int mgt = lpObj->MapNumber + 109;
								short x, y, level;
								BYTE map = lpObj->MapNumber, dir;
								int result = gGateC.GetGate(mgt, (short &)x, (short &)y, (BYTE &)map, (BYTE &)dir, (short &)level);

								if (result >= 0)
								{
									lpObj->MapNumber = map;
									lpObj->X = x;
									lpObj->Y = y;
								}

								g_IT_Event.IllusionTempleUserDie(*lpObj);
							}

							else
							{
								lpObj->MapNumber = MAP_INDEX_LORENMARKET;
								MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, (short &)lpObj->X, (short &)lpObj->Y);
							}

							if (lpObj->m_nITR_RelicsTick > 0 || lpObj->m_wITR_NpcType > 0 || lpObj->m_byITR_StoneState != 99 || lpObj->m_bITR_RegisteringRelics == true || lpObj->m_bITR_GettingRelics == true)
							{
								g_IT_Event.CancleStoneState(lpObj->m_wITR_NpcType, lpObj->m_byITR_StoneState, lpObj->MapNumber);
								lpObj->m_nITR_RelicsTick = 0;
								lpObj->m_wITR_NpcType = 0;
								lpObj->m_byITR_StoneState = 99;
								lpObj->m_bITR_RegisteringRelics = 0;
								lpObj->m_bITR_GettingRelics = 0;
							}
						}

						else if (lpObj->MapNumber == MAP_INDEX_DEVILSQUARE_FINAL)
						{
							lpObj->MapNumber = MAP_INDEX_LORENMARKET;
							MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, (short &)lpObj->X, (short &)lpObj->Y);
						}

						else if (IMPERIAL_MAP_RANGE(lpObj->MapNumber))
						{
							BYTE btMapNumber = lpObj->MapNumber, dir;
							short x, y, level;
							int result = gGateC.GetGate(22, x, y, btMapNumber, dir, level);

							if (result >= 0)
							{
								lpObj->MapNumber = btMapNumber;
								lpObj->X = x;
								lpObj->Y = y;
							}

							g_ImperialGuardian.GCMissionFailUserDie(*lpObj);
						}

						else if (lpObj->MapNumber == MAP_INDEX_ARCA_WAR || lpObj->MapNumber == MAP_INDEX_DEBENTER_ARCA_WAR)
						{
							if (g_ArcaBattle.IsArcaBattleServer() == TRUE && g_AcheronGuardianEvent.GetProcState() == 1)
							{
								PeriodBuffInfo BuffInfo;
								LPPERIOD_BUFF_EFFECT_INFO lpPeriBuff = g_BuffEffectSlot.GetPeriodBuffInfo(184);

								if (!lpPeriBuff)
									return;

								if (gObjAddPeriodBuffEffect(*lpObj, lpPeriBuff, lpPeriBuff->dwDuration) == TRUE)
								{
									BuffInfo.wBuffIndex = 184;
									BuffInfo.btEffectType1 = 20;
									BuffInfo.btEffectType2 = 0;
									BuffInfo.lDuration = lpPeriBuff->dwDuration;

									g_BuffEffect.RequestPeriodBuffInsert(*lpObj, &BuffInfo);
									sLog->outBasic("[ArcaBattle] ArcaBattle Die Penalty [%s][%s]", lpObj->AccountID, lpObj->Name);
								}
							}

							int mgt = 426;
							BYTE map = lpObj->MapNumber;
							BYTE dir;
							short x, y, level;
							int result = gGateC.GetGate(mgt, (short &)x, (short &)y, (BYTE &)map, (BYTE &)dir, (short &)level);

							if (result >= 0)
							{
								lpObj->MapNumber = map;
								lpObj->X = x;
								lpObj->Y = y;
							}
						}
						else if (lpObj->MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE)
						{
							if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
							{
								MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, (short &)lpObj->X, (short &)lpObj->Y);
							}

							else
							{
								if (g_Crywolf.GetCrywolfState() == 4)
								{
									int mgt = 118;
									short x, y, level;
									BYTE map = lpObj->MapNumber, dir;

									int result = gGateC.GetGate(mgt, (short &)x, (short &)y, (BYTE &)map, (BYTE &)dir, (short &)level);

									if (result >= 0)
									{
										lpObj->MapNumber = map;
										lpObj->X = x;
										lpObj->Y = y;
									}
									else //1st JMP
									{
										MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, (short &)lpObj->X, (short &)lpObj->Y);

									}
								}
								else // 2nd JMP
								{
									MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, (short &)lpObj->X, (short &)lpObj->Y);

								}
							}
						}

						else if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE && lpObj->MapNumber == MAP_INDEX_KANTURU_BOSS)
						{
							int mgt = 137;
							short x, y, level;
							BYTE map = lpObj->MapNumber, dir;
							int result = gGateC.GetGate(mgt, (short &)x, (short &)y, (BYTE &)map, (BYTE &)dir, (short &)level);

							if (result >= 0)
							{
								lpObj->MapNumber = map;
								lpObj->X = x;
								lpObj->Y = y;
							}

							g_KanturuBattleUserMng.DeleteUserData(*lpObj);
						}

						else if (lpObj->MapNumber == MAP_INDEX_KANTURU1)
						{
							int mgt = 138;
							short x, y, level;
							BYTE map = lpObj->MapNumber, dir;

							int result = gGateC.GetGate(mgt, (short &)x, (short &)y, (BYTE &)map, (BYTE &)dir, (short &)level);

							if (result >= 0)
							{
								lpObj->MapNumber = map;
								lpObj->X = x;
								lpObj->Y = y;
							}
						}
						else if (lpObj->MapNumber == MAP_INDEX_KANTURU2)
						{
							int mgt = 139;
							short x, y, level;
							BYTE map = lpObj->MapNumber, dir;

							int result = gGateC.GetGate(mgt, (short &)x, (short &)y, (BYTE &)map, (BYTE &)dir, (short &)level);

							if (result >= 0)
							{
								lpObj->MapNumber = map;
								lpObj->X = x;
								lpObj->Y = y;
							}
						}

						else if (lpObj->MapNumber == MAP_INDEX_CALMNESS)
						{
							int mgt = 273;
							short x, y, level;
							BYTE map = lpObj->MapNumber, dir;

							int result = gGateC.GetGate(mgt, (short &)x, (short &)y, (BYTE &)map, (BYTE &)dir, (short &)level);

							if (result >= 0)
							{
								lpObj->MapNumber = map;
								lpObj->X = x;
								lpObj->Y = y;
							}
						}

						else if (lpObj->MapNumber == MAP_INDEX_RAKLION)
						{
							int mgt = 287;
							short x, y, level;
							BYTE map = lpObj->MapNumber, dir;

							int result = gGateC.GetGate(mgt, (short &)x, (short &)y, (BYTE &)map, (BYTE &)dir, (short &)level);

							if (result >= 0)
							{
								lpObj->MapNumber = map;
								lpObj->X = x;
								lpObj->Y = y;
							}
						}

						else if (lpObj->MapNumber == MAP_INDEX_HATCHERY)
						{
							int mgt = 287;
							short x, y, level;
							BYTE map = lpObj->MapNumber, dir;

							int result = gGateC.GetGate(mgt, (short &)x, (short &)y, (BYTE &)map, (BYTE &)dir, (short &)level);

							if (result >= 0)
							{
								lpObj->MapNumber = map;
								lpObj->X = x;
								lpObj->Y = y;
							}
							g_RaklionBattleUserMng.DeleteUserData(*lpObj);
						}

						else if (DG_MAP_RANGE(lpObj->MapNumber))
						{
							g_DoppelGanger.LeaveDoppelganger(*lpObj);

							int mgt = 267;
							short x, y, level;
							BYTE map = lpObj->MapNumber, dir;

							int result = gGateC.GetGate(mgt, (short &)x, (short &)y, (BYTE &)map, (BYTE &)dir, (short &)level);

							if (result >= 0)
							{
								lpObj->MapNumber = map;
								lpObj->X = x;
								lpObj->Y = y;
							}

							if (g_DoppelGanger.GetDoppelgangerState() == 2)
							{
								g_DoppelGanger.SendDoppelgangerResult(*lpObj, 1);
							}
						}

						else if (lpObj->MapNumber == MAP_INDEX_VULCAN)
						{
							int mgt = 294;
							short x, y, level;
							BYTE map = lpObj->MapNumber, dir;

							int result = gGateC.GetGate(mgt, (short &)x, (short &)y, (BYTE &)map, (BYTE &)dir, (short &)level);

							if (result >= 0)
							{
								lpObj->MapNumber = map;
								lpObj->X = x;
								lpObj->Y = y;
							}
						}

						else if (lpObj->MapNumber == MAP_INDEX_KALRUTAN1 || lpObj->MapNumber == MAP_INDEX_KALRUTAN2)
						{
							int mgt = 335;
							short x, y, level;
							BYTE map = lpObj->MapNumber, dir;

							int result = gGateC.GetGate(mgt, (short &)x, (short &)y, (BYTE &)map, (BYTE &)dir, (short &)level);

							if (result >= 0)
							{
								lpObj->MapNumber = map;
								lpObj->X = x;
								lpObj->Y = y;
							}
						}

						else if (lpObj->MapNumber == MAP_INDEX_ACHERON || lpObj->MapNumber == MAP_INDEX_DEBENTER || lpObj->MapNumber == MAP_INDEX_NARS || lpObj->MapNumber == MAP_INDEX_URUK_MOUNTAIN)
						{
							int mgt = 417;
							short x, y, level;
							BYTE map = lpObj->MapNumber, dir;

							int result = gGateC.GetGate(mgt, (short &)x, (short &)y, (BYTE &)map, (BYTE &)dir, (short &)level);

							if (result >= 0)
							{
								lpObj->MapNumber = map;
								lpObj->X = x;
								lpObj->Y = y;
							}
						}

						else if (lpObj->MapNumber == MAP_INDEX_FEREA)
						{
							int mgt = 509;
							short x, y, level;
							WORD map = lpObj->MapNumber, dir;

							int result = gGateC.GetGate(mgt, (short &)x, (short &)y, (BYTE &)map, (BYTE &)dir, (short &)level);

							if (result >= 0)
							{
								lpObj->MapNumber = map;
								lpObj->X = x;
								lpObj->Y = y;
							}
						}

						else if (lpObj->MapNumber == MAP_INDEX_NIXIE_LAKE)
						{
							int mgt = 522;
							short x, y, level;
							WORD map = lpObj->MapNumber, dir;

							int result = gGateC.GetGate(mgt, (short &)x, (short &)y, (BYTE &)map, (BYTE &)dir, (short &)level);

							if (result >= 0)
							{
								lpObj->MapNumber = map;
								lpObj->X = x;
								lpObj->Y = y;
							}
						}

						else
						{
							MapC[lpObj->MapNumber].GetMapPos(lpObj->MapNumber, (short &)lpObj->X, (short &)lpObj->Y);
						}

						short m_Result = g_MapServerManager.CheckMoveMapSvr(*lpObj, lpObj->MapNumber, lpObj->m_sPrevMapSvrCode);

						if (m_Result != g_ConfigRead.server.GetGameServerCode())
						{
							if (m_Result == -1)
							{
								sLog->outError("[MapServerMng] Map Server Move Fail : CheckMoveMapSvr() == -1 [%s][%s] (%d)", lpObj->AccountID, lpObj->Name, lpObj->m_Index);
								continue;
							}

							int _map = lpObj->MapNumber;
							int _x = lpObj->X;
							int _y = lpObj->Y;

							lpObj->MapNumber = mapnum;
							lpObj->X = x;
							lpObj->Y = y;

							GJReqMapSvrMove(*lpObj, m_Result, _map, _x, _y);
							sLog->outBasic("[MapServerMng] Request to Move Map Server : (%d) - [%s][%s] (%d)", m_Result, lpObj->AccountID, lpObj->Name, lpObj->m_Index);
							continue;
						}

						lpObj->TX = lpObj->X;
						lpObj->TY = lpObj->Y;

						CreateFrustrum(lpObj->X, lpObj->Y, *lpObj);

						pMsg.h.c = 0xC3;
						pMsg.h.headcode = 0xF3;
						pMsg.h.size = sizeof(pMsg);
						pMsg.subcode = 0x04;
						pMsg.Dir = lpObj->Dir;
						pMsg.MapX = lpObj->X;
						pMsg.MapY = lpObj->Y;
						pMsg.Life = lpObj->Life;
						pMsg.Mana = lpObj->Mana;
						pMsg.unk = 0;
						UINT64 Experience = lpObj->m_PlayerData->Experience;

						if (g_MasterLevelSkillTreeSystem.IsMasterLevelUser(*lpObj) == TRUE)
						{
							Experience = lpObj->m_PlayerData->MasterExperience;
						}

						pMsg.ExpHHH = SET_NUMBERH(SET_NUMBERHW(HIDWORD(Experience)));
						pMsg.ExpHHL = SET_NUMBERL(SET_NUMBERHW(HIDWORD(Experience)));
						pMsg.ExpHLH = SET_NUMBERH(SET_NUMBERLW(HIDWORD(Experience)));
						pMsg.ExpHLL = SET_NUMBERL(SET_NUMBERLW(HIDWORD(Experience)));
						pMsg.ExpLHH = SET_NUMBERH(SET_NUMBERHW(LODWORD(Experience)));
						pMsg.ExpLHL = SET_NUMBERL(SET_NUMBERHW(LODWORD(Experience)));
						pMsg.ExpLLH = SET_NUMBERH(SET_NUMBERLW(LODWORD(Experience)));
						pMsg.ExpLLL = SET_NUMBERL(SET_NUMBERLW(LODWORD(Experience)));

						pMsg.MapNumber = lpObj->MapNumber;
						pMsg.Money = lpObj->m_PlayerData->Money;
						pMsg.wShield = lpObj->iShield;
						pMsg.BP = lpObj->BP;

						pMsg.IGCLife = lpObj->Life;
						pMsg.IGCMana = lpObj->Mana;

						lpObj->PathCount = 0;

						IOCP.DataSend(n, (BYTE *)&pMsg, pMsg.h.size);

						gObjViewportListProtocolCreate(*lpObj);

						g_SendNPCInfo.SendNpcCoordinate(lpObj->m_Index, lpObj->MapNumber, 0);
						g_SendNPCInfo.SendPortalCoordinate(lpObj->m_Index, lpObj->MapNumber);

						if (DragonEvent->GetState())
						{
							if (DragonEvent->GetMapNumber() == lpObj->MapNumber)
							{
								gGameProtocol.GCMapEventStateSend(lpObj->MapNumber, 1, 1);
							}
						}

						gEledoradoEvent.CheckGoldDercon(lpObj->MapNumber);

						if (g_DevilSquareFinal.m_bEVENT_ENABLE == TRUE && lpObj->MapNumber == MAP_INDEX_LORENMARKET &&
							g_DevilSquareFinal.m_nDSF_TYPE != 0 && g_DevilSquareFinal.m_nUserCount == 0)
						{
							g_DevilSquareFinal.SendDSFResult(*lpObj);
						}

						if (lpObj->Authority == 32) //season4 add-on
						{
							gObjAddBuffEffect(*lpObj, BUFFTYPE_GM_MARK, 0, 0, 0, 0, -10);
						}

						if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && lpObj->MapNumber == MAP_INDEX_CASTLESIEGE)
						{
							gGameProtocol.GCAnsCsNotifyStart(*lpObj, CASTLESIEGEREADY(g_CastleSiege.GetCastleState()));

							if (g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
							{
								g_CastleSiege.NotifySelfCsJoinSide(*lpObj);
								g_CastleSiege.NotifyCsSelfLeftTime(*lpObj);
							}

						}
					}
					else
					{
						if (gObjMonsterRegen(*lpObj) == 0)
						{
							continue;
						}

						CreateFrustrum(lpObj->X, lpObj->Y, *lpObj);

						if (IT_MAP_RANGE(lpObj->MapNumber) && lpObj->Class == 380)
						{
							sLog->outBasic("[Illusion Temple] (%d) Status Regen OK (%d: %d/%d)",
								lpObj->MapNumber - 44, lpObj->MapNumber,
								lpObj->X, lpObj->Y);
						}
					}

					lpObj->DieRegen = 0;
					lpObj->m_State = 1;
				}
			}
		}
	}

	for (int n = 0; n < MAX_NUMBER_MAP; n++)
	{
		MapC[n].StateSetDestroy();
	}
}

void gObjSecondProc()
{
	/*if (CharSaveLog == NULL)
	{
		CharSaveLog = new CLogToFile("CHAR_SAVE_LOG", g_ConfigRead.server.GetUserSaveLogPath(), 256);
	}*/

	if (gDisconnect == 1)
		return;

	CGameObject* lpObj;

	for each (std::pair<int, CGameObject*> ObjEntry in gGameObjects)
	{
		CGameObject* lpObj = ObjEntry.second;

		if (lpObj->Connected > PLAYER_LOGGED)
		{
			if (lpObj->m_bMapSvrMoveQuit == 1)
			{
				if (GetTickCount() - lpObj->m_dwMapSvrQuitTick > 30000)
				{
					gObjDel(*lpObj);
					continue;
				}
			}

			gObjSkillUseProc(*lpObj);
			gObjCheckBuffEffectList(*lpObj);

			if (lpObj->Type == OBJ_USER)
			{
				g_CMuunSystem.CheckMuunItemPeriodData(*lpObj);
			}

			if (lpObj->Type == OBJ_NPC && lpObj->Class == 229)
			{
				gQeustNpcTeleport.Run(*lpObj);
			}

			if (lpObj->Type == OBJ_NPC && lpObj->Class == 568)
			{
				gQuestExpNpcTeleport.Run(*lpObj);
			}

			if (lpObj->Type == OBJ_MONSTER)
			{
				if (lpObj->Class == 275)
				{
					gObjRefillMonsterHP(*lpObj, giKundunRefillHPSec);
				}

				if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
				{
					if (lpObj->Class == 283)
					{
						g_CsNPC_GuardianStatue.GuardianStatueAct(*lpObj);
						continue;
					}

					if (lpObj->Class == 278)
					{
						g_CsNPC_LifeStone.LifeStoneAct(*lpObj);
						continue;
					}
				}

				if (lpObj->Class == 285)
				{
					g_CsNPC_Guardian.GuardianAct(*lpObj);
					continue;
				}

				if (lpObj->Class == 647 || lpObj->Class == 648 || lpObj->Class == 649 || lpObj->Class == 650)
				{
					if (lpObj->Live == TRUE && GetTickCount() - lpObj->m_Disappear_NewYearMonster > 600000)
					{
						lpObj->Live = FALSE;
						lpObj->m_State = 4;
						lpObj->RegenTime = GetTickCount();
						lpObj->DieRegen = 1;
						lpObj->PathCount = 0;
						lpObj->m_Disappear_NewYearMonster = 0;
						gObjDel(*lpObj);

					}
				}

				if (lpObj->Class == 653 || lpObj->Class == 654 || lpObj->Class == 655 || lpObj->Class == 656 || lpObj->Class == 657)
				{
					if (lpObj->Live == TRUE && GetTickCount() - lpObj->m_Disappear_Monster > 1800000)
					{
						lpObj->Live = FALSE;
						lpObj->m_State = 4;
						lpObj->RegenTime = GetTickCount();
						lpObj->DieRegen = 1;
						lpObj->PathCount = 0;
						lpObj->m_Disappear_Monster = 0;

						sLog->outBasic("[ColossusEvent][GoldMonsterTimeout] MonsterClass : [%d], MonsterIndex : [%d], Map[%d], X[%d], Y[%d]",
							lpObj->Class, *lpObj, lpObj->MapNumber, lpObj->X, lpObj->Y);

						gObjDel(*lpObj);
					}
				}

				if (lpObj->Class == 674 || lpObj->Class == 675 || lpObj->Class == 676 || lpObj->Class == 677)
				{
					if (lpObj->Live == TRUE && lpObj->m_Attribute == 60 && GetTickCount() - lpObj->m_Disappear_Monster > 1800000)
					{
						lpObj->Live = FALSE;
						lpObj->m_State = 4;
						lpObj->RegenTime = GetTickCount();
						lpObj->DieRegen = 1;
						lpObj->PathCount = 0;
						lpObj->m_Disappear_Monster = 0;

						sLog->outBasic("[PetEvent][GoldMonsterTimeout] MonsterClass : [%d], MonsterIndex : [%d], Map[%d], X[%d], Y[%d]",
							lpObj->Class, *lpObj, lpObj->MapNumber, lpObj->X, lpObj->Y);

						gObjDel(*lpObj);
					}
				}

				if (lpObj->Class == 681 || lpObj->Class == 690)
				{
					if (GetTickCount() - lpObj->m_Disappear_Monster > g_EvoMonMng.GetEvoMonTimeLimit() * 60 * 1000)
					{
						g_EvoMonMng.EndTimeProcess(*lpObj);
					}
				}

				if (lpObj->Class == 704 || lpObj->Class == 705)
				{
					if (GetTickCount() - lpObj->m_Disappear_Monster > 5000)
					{
						g_EvoMonMng.FieldProcess(*lpObj);
					}
				}

				if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
				{
					if (lpObj->Class == 288)
					{
						g_CsNPC_CannonTower.CannonTowerAct(*lpObj);
						continue;
					}
					g_Crywolf.CrywolfMonsterAct(*lpObj);
				}
			}

			if (lpObj->Type == OBJ_NPC)
			{
				if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
				{
					if ((lpObj->Class < 204) ? FALSE : (lpObj->Class > 209) ? FALSE : TRUE)
					{
						g_Crywolf.CrywolfNpcAct(*lpObj);
					}

					if (lpObj->Class == 216)
					{
						g_CsNPC_CastleCrown.CastleCrownAct(*lpObj);
						continue;
					}

					if (CASTLE_CROWNSWITCH_CLASS_RANGE(lpObj->Class))
					{
						g_CsNPC_CastleCrownSwitch.CastleCrownSwitchAct(*lpObj);
						continue;
					}
				}

				if (lpObj->Class == 221 || lpObj->Class == 222)
				{
					g_CsNPC_Weapon.WeaponAct(*lpObj);
				}
			}

			if (lpObj->Type == OBJ_USER)
			{
				gObjManaPotionFill(*lpObj);
				gObjRestPotionFill(*lpObj);

				//gObjCheckXYMapTile(Obj,3);

				if (lpObj->ChatLimitTime > 0)
				{
					lpObj->ChatLimitTimeSec++;

					if (lpObj->ChatLimitTimeSec > 60)
					{
						lpObj->ChatLimitTimeSec = 0;
						lpObj->ChatLimitTime--;

						if (lpObj->ChatLimitTime < 1)
						{
							lpObj->ChatLimitTime = 0;
							MsgOutput(*lpObj, Lang.GetText(0, 49));
						}
					}
				}

				if (lpObj->m_dwDCTimer > 0)
				{
					if ((GetTickCount64() - lpObj->m_dwDCTimer) > 5000)
					{
						//BYTE btKillClient[] = { 0xC1, 0x04, 0xFA, 0xA6 };
						//IOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, btKillClient, sizeof(btKillClient));
						//	sLog->outError( "(%d)[%s][%s][%s] User did not disconnect upon request. Modified client?", Obj, lpObj->m_PlayerData->ConnectUser->IP, lpObj->AccountID, lpObj->Name);
					}
				}

				if (g_iUseCharacterAutoRecuperationSystem != 0)
				{
					if (lpObj->Level <= g_iCharacterRecuperationMaxLevel)
					{
						gProcessAutoRecuperation(*lpObj);
					}
				}

				if (lpObj->Type == OBJ_USER)
				{
					gObjShieldAutoRefill(*lpObj);
				}

				if (lpObj->Type == OBJ_USER && lpObj->m_LastTeleportTime > 0)
				{
					lpObj->m_LastTeleportTime--;
				}

				if ((GetTickCount() - lpObj->m_PlayerData->PacketTime) > 1000)
				{
					lpObj->m_PlayerData->PacketsPerSecond = 0;
					lpObj->m_PlayerData->PacketTime = GetTickCount();
				}

				gObjDelayLifeCheck(*lpObj);
				gObjSecondDurDown(*lpObj);

				if (lpObj->PartyNumber >= 0)
				{
					gParty.PartyMemberLifeSend(lpObj->PartyNumber);

					if (lpObj->Type == OBJ_USER)
					{
						if (lpObj->m_PlayerData->m_bSendMemberPos)
						{
							gGameProtocol.GCPartyMemberPosition(lpObj->m_Index);
						}
					}
				}

				if (lpObj->m_ShopTime >= 1 && lpObj->m_ShopTime < 255)
				{
					lpObj->m_ShopTime++;
				}

				int BattleGround = 1;
				int m_BattleTimer = gCheckBattleGroundTimer();

				if ((lpObj->Authority & 2) == 2)
				{
					BattleGround = 0;
					gGameProtocol.GCTimeViewSend(*lpObj, 0);
				}

				if ((lpObj->Penalty & 2) == 2)
				{

				}

				if (lpObj->MapNumber == MAP_INDEX_ARENA && BattleGround == 1)
				{
					BattleGround = 1;

					if (lpObj->m_PlayerData->lpGuild != 0)
					{
						if (lpObj->m_PlayerData->lpGuild->WarState != 0)
						{
							int CheckBattleGround = gCheckBattleGround(*lpObj);

							if (CheckBattleGround != lpObj->m_PlayerData->lpGuild->BattleGroundIndex)
							{
								if (lpObj->m_PlayerData->lpGuild->WarType == 1)
								{
									BattleGround = 0;
								}
								else
								{
									BattleGround = 0;
								}
							}
							else
							{
								BattleGround = 0;
							}
						}
					}
					else
					{
						BattleGround = 1;
					}

					if (BattleGround != 0)
					{
						int CheckBattleGround = gCheckBattleGround(*lpObj);

						if (CheckBattleGround >= 0)
						{
							if (lpObj->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL && g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
							{
								gObjMoveGate(*lpObj, 333);
							}

							else
							{
								gObjMoveGate(*lpObj, 17);
							}
						}
					}

					gObjMuBotProc(*lpObj);
				}
			}
		}

		if (lpObj->Connected >= PLAYER_LOGGED && lpObj->Type == OBJ_USER && lpObj->CloseCount > 0)
		{
			if (lpObj->CloseCount == 1)
			{
				if (lpObj->CloseType == 1)
				{
					if (gObjGameClose(*lpObj) == 1)
					{
						gGameProtocol.GCCloseMsgSend(*lpObj, 1);
						GJUpdateMatchDBUserCharacters(*lpObj);
					}
				}
				else if (lpObj->CloseType == 0)
				{
					gGameProtocol.GCCloseMsgSend(*lpObj, 0);
					IOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
				}
				else if (lpObj->CloseType == 2)
				{
					gGameProtocol.GCCloseMsgSend(*lpObj, 2);
				}
			}
			else
			{
				char Msg[128];
				wsprintf(Msg, Lang.GetText(0, 29), lpObj->CloseCount - 1);
				gGameProtocol.GCServerMsgStringSend(Msg, *lpObj, 1);
			}
			lpObj->CloseCount--;
		}

		if (lpObj->Connected > PLAYER_LOGGED &&	lpObj->Type == OBJ_USER && lpObj->m_PlayerData->ISBOT == false)
		{
			if (GetTickCount() - lpObj->AutoSaveTime > 600000)
			{
				GJSetCharacterInfo(*lpObj, 0);
				lpObj->AutoSaveTime = GetTickCount();
			}

			if ((lpObj->Penalty & 2) == 2 && lpObj->ChatBlockTime > 0)
			{
				time_t currtime = time(NULL);

				if (currtime > lpObj->ChatBlockTime)
				{
					lpObj->PenaltyMask &= ~1;
					lpObj->Penalty &= ~2;
					lpObj->ChatBlockTime = 0;
				}
			}

			if (lpObj->m_PlayerData->AntiHackCheckTime > 0 && GetTickCount() - lpObj->m_PlayerData->AntiHackCheckTime > 180000 && lpObj->m_bOff == false)
			{
				/*	sLog->outError( "[%s][%s][%s][%s] AntiHack breach #1-> No response",
				lpObj->AccountID, lpObj->Name, lpObj->m_PlayerData->ConnectUser->IP, lpObj->m_PlayerData->HWID);

				AntiHackLog->Output("[%s][%s][%s][%s] AntiHack breach #1-> No response",
				lpObj->AccountID, lpObj->Name, lpObj->m_PlayerData->ConnectUser->IP, lpObj->m_PlayerData->HWID);

				if (g_ConfigRead.antihack.AntiHackBreachDisconnectUser == true)
				{
				gGameProtocol.GCSendDisableReconnect(Obj);
				//IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
				}*/
			}


			if (g_ConfigRead.data.common.bHitHackDetection)
			{
				if (lpObj->m_PlayerData->dwLastHitHackTick > 0 && GetTickCount64() - lpObj->m_PlayerData->dwLastHitHackTick > 180000 && lpObj->m_bOff == false)
				{
					sLog->outError("[%s][%s][%s][%s] AntiHack breach #2 -> No response",
						lpObj->AccountID, lpObj->Name, lpObj->m_PlayerData->ConnectUser->IP, lpObj->m_PlayerData->HWID);

					if (g_ConfigRead.antihack.AntiHackBreachDisconnectUser == true)
					{
						gGameProtocol.GCSendDisableReconnect(*lpObj);
						//IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
					}
				}
			}

			if (!lpObj->m_bOff)
			{
				if (lpObj->CheckSumTime > 0 && GetTickCount() - lpObj->CheckSumTime > 3000)
				{
					//IOCP.CloseClient(n);
				}

				if (lpObj->CrcCheckTime > 0 && GetTickCount() - lpObj->CrcCheckTime > 30000)
				{
					sLog->outError("[CRC] [%s][%s] No response - DC", lpObj->AccountID, lpObj->Name);
					gGameProtocol.GCSendDisableReconnect(*lpObj);
					//IOCP.CloseClient(n);
				}

				if (lpObj->AgiCheckTime > 0 && GetTickCount() - lpObj->AgiCheckTime > 180000)
				{
					if (g_ConfigRead.antihack.EnabledAgiCheck == true)
					{
						//IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
					}

					else
					{
						lpObj->AgiCheckTime = GetTickCount();
					}
				}

				gObjInterfaceTimeCheck(*lpObj);
				gObjTimeCheckSelfDefense(*lpObj);
			}
		}

		if (lpObj->Connected == PLAYER_CONNECTED || lpObj->Connected == PLAYER_LOGGED || lpObj->Connected == PLAYER_PLAYING)
		{
			if (lpObj->Type == OBJ_USER && lpObj->m_PlayerData->ISBOT == false && lpObj->m_bOff == false)
			{
#ifdef GS_AUTHORITY
				if (lpObj->Connected >= PLAYER_LOGGED)
				{
					if (GetTickCount() - lpObj->ConnectCheckTime > 60000)
					{
						IOCP.CloseClient(lpObj->PerSocketContext, 0);
					}
				}
				else
				{
					if (GetTickCount() - lpObj->ConnectCheckTime > 30000)
					{
						IOCP.CloseClient(lpObj->PerSocketContext, 0);
					}
				}
#else
				if (lpObj->Connected >= PLAYER_LOGGED)
				{
					if (GetTickCount() - lpObj->ConnectCheckTime > 60000)
					{
						IOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->PerSocketContext, 0);
						sLog->outBasic("Game working response error causes conclusion [%d][%s][%s][%s]", *lpObj, lpObj->AccountID, lpObj->Name, lpObj->m_PlayerData->ConnectUser->IP);
					}
				}
				else
				{
					if (GetTickCount() - lpObj->ConnectCheckTime > 30000)
					{
						IOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->PerSocketContext, 0);
						sLog->outBasic("Response error after connection causes conclusion [%d][%s][%s][%s]", *lpObj, lpObj->AccountID, lpObj->Name, lpObj->m_PlayerData->ConnectUser->IP);
					}
				}
#endif
			}

			//Anti Speed Move Hack
			if (g_ConfigRead.MovingSpeedCheck == TRUE)
			{
				if (lpObj->m_PlayerData->MovingIgnore > 0)
					lpObj->m_PlayerData->MovingIgnore--;

				if (lpObj->m_PlayerData->MovingIgnore == 0 && lpObj->m_PlayerData->MovingDistance >= g_ConfigRead.MovingSpeedDistance && lpObj->MapNumber != MAP_INDEX_HATCHERY)
				{
					if (g_ConfigRead.MovingSpeedLog != 0)
						sLog->outBasic("[%s][%s] Move Speed Hack Detected [%d]", lpObj->AccountID, lpObj->Name, lpObj->m_PlayerData->MovingDistance);

					if (g_ConfigRead.MovingSpeedDC)
						IOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
				}
				else
				{
					lpObj->m_PlayerData->MovingDistance = 0;
				}
			}
		}
	}
}




void gObjManaPotionFill(CGameObject &Obj)
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	Obj.m_ManaFillCount++;

	float tmana;
	int ChangeMana = 0;

	if (Obj.m_ManaFillCount > 2)
	{
		Obj.m_ManaFillCount = 0;

		int BP, AddBP;
		BYTE MapAttr;
		int rBP, CurBP;

		if (Obj.MaxMana + Obj.AddMana > Obj.Mana)
		{
			float percent = 3.7f;

			if (!(BC_MAP_RANGE(Obj.MapNumber) ||
				CC_MAP_RANGE(Obj.MapNumber) ||
				KALIMA_MAP_RANGE(Obj.MapNumber) ||
				DS_MAP_RANGE(Obj.MapNumber) ||
				IT_MAP_RANGE(Obj.MapNumber) ||
				Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL) &&
				!(Obj.MapNumber == MAP_INDEX_CASTLESIEGE ||
					Obj.MapNumber == MAP_INDEX_CASTLEHUNTZONE))
			{
				int iValue = gObjGetTotalValueOfEffect(Obj, EFFECTTYPE_RECOVERY_MANA); //season3 add-on

				if (iValue > 0)
				{
					percent += percent * iValue / 100.0;	//1.01.00 fix
				}
			}

			if (percent > 0.0 && Obj.m_PlayerData->m_MPSkillOpt->iMpsAutoRecoverMana > 0.0)
			{
				percent += Obj.m_PlayerData->m_MPSkillOpt->iMpsAutoRecoverMana;
			}

			tmana = Obj.Mana;
			tmana += (Obj.MaxMana + Obj.AddMana)*percent / 100.0f;

			tmana += Obj.m_PlayerData->m_JewelOfHarmonyEffect->HJOpAddRefillMP;

			if (Obj.m_PlayerData->m_btRefillMPSocketOption > 0) //season4 add-on
			{
				tmana += Obj.m_PlayerData->m_btRefillMPSocketOption;
			}

			if (Obj.MaxMana + Obj.AddMana < tmana)
			{
				tmana = Obj.MaxMana + Obj.AddMana;
			}

			Obj.Mana = tmana;
			ChangeMana = 1;
		}

		BP = Obj.MaxBP + Obj.AddBP;

		if (Obj.BP < BP)
		{
			MapAttr = MapC[Obj.MapNumber].GetAttr((short &)Obj.X, (short &)Obj.Y);
			rBP = 2;

			if ((MapAttr & 1) != 0)
			{
				rBP = 5;
			}

			if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_SANTA_LUCKY))
			{
				rBP = 5;
				if ((MapAttr & 1) != 0)
				{
					rBP = 10;
				}
			}

			if (Obj.Class == CLASS_KNIGHT)
			{
				AddBP = rBP + (BP * 5 / 100);
			}
			else
			{
				AddBP = rBP + (BP * 3 / 100);
			}

			if (Obj.m_PlayerData->m_MPSkillOpt->iMpsAutoRecoverAG > 0.0)
			{
				AddBP += BP * Obj.m_PlayerData->m_MPSkillOpt->iMpsAutoRecoverAG;
			}

			if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_SOUL_POTION) != 0)
			{
				AddBP += 8;
			}

			AddBP += Obj.m_PlayerData->SetOpIncAGValue;

			if (AddBP < 1)
			{
				AddBP = 1;
			}

			CurBP = Obj.BP;
			CurBP += AddBP;

			if (CurBP > BP)
			{
				CurBP = BP;
			}

			Obj.BP = CurBP;

			ChangeMana = 1;
		}

		if (ChangeMana != 0)
		{
			gGameProtocol.GCManaSend(Obj, Obj.Mana, 255, 0, Obj.BP);
		}
	}
}




void gObjRestPotionFill(CGameObject &Obj)
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.m_Rest != 0)
	{
		float tlife;
		float tmana;
		float percent = 3.0;
		Obj.m_LifeFillCount++;

		if (Obj.m_LifeFillCount > 4)
		{
			Obj.m_LifeFillCount = 0;

			tlife = Obj.Life;

			if (Obj.pntInventory[10]->IsItem() == 1)
			{
				if (Obj.pntInventory[10]->m_Option3 != 0 && Obj.pntInventory[10]->m_Type != ITEMGET(13, 24))
				{
					percent += Obj.pntInventory[10]->m_Option3;
				}
			}
			else if (Obj.pntInventory[11]->IsItem() == 1)
			{
				if (Obj.pntInventory[11]->m_Option3 != 0 && Obj.pntInventory[11]->m_Type != ITEMGET(13, 24))
				{
					percent += Obj.pntInventory[11]->m_Option3;
				}
			}
			else if (Obj.pntInventory[9]->IsItem() == 1)
			{
				if (Obj.pntInventory[9]->m_Option3 != 0 && Obj.pntInventory[9]->m_Type != ITEMGET(13, 28))
				{
					percent += Obj.pntInventory[9]->m_Option3;
				}
			}

			if (percent > 0.0 && Obj.m_PlayerData->m_MPSkillOpt->iMpsAutoRecoverLife > 0.0)
			{
				percent += Obj.m_PlayerData->m_MPSkillOpt->iMpsAutoRecoverLife;
			}

			percent += gObjGetTotalValueOfEffect(Obj, EFFECTTYPE_RECOVERY_HP);
			percent += Obj.AutoHPRecovery;

			tlife += (Obj.MaxLife + Obj.AddLife)*percent / 100.0f;
			tlife += Obj.m_PlayerData->m_JewelOfHarmonyEffect->HJOpAddRefillHP;

			if (Obj.m_PlayerData->m_btRefillHPSocketOption > 0) //season4 add-on
			{
				tlife += Obj.m_PlayerData->m_btRefillHPSocketOption;
			}

			if ((Obj.MaxLife + Obj.AddLife) < tlife)
			{
				tlife = Obj.MaxLife + Obj.AddLife;
			}

			Obj.Life = tlife;

			gGameProtocol.GCReFillSend(Obj, Obj.Life, -1, 0, Obj.iShield);
		}

		percent = 3.0f;
		percent += gObjGetTotalValueOfEffect(Obj, EFFECTTYPE_RECOVERY_MANA);

		tmana = Obj.Mana + Obj.AddMana;
		tmana += (Obj.MaxMana + Obj.AddMana) * percent / 100.0f;
		tmana += Obj.m_PlayerData->m_JewelOfHarmonyEffect->HJOpAddRefillMP;

		if ((Obj.MaxMana + Obj.AddMana) < tmana)
		{
			tmana = Obj.MaxMana + Obj.AddMana;
		}

		Obj.Mana = tmana;

		gGameProtocol.GCManaSend(Obj, Obj.Mana, 255, 0, Obj.BP);
	}

	else
	{
		float percent = 0;
		float tlife;
		if (Obj.pntInventory[10]->IsItem() == 1)
		{
			if (Obj.pntInventory[10]->m_Option3 != 0 && Obj.pntInventory[10]->m_Durability != 0.0f && Obj.pntInventory[10]->m_Type != ITEMGET(13, 24))
			{
				percent += Obj.pntInventory[10]->m_Option3;
			}
		}

		if (Obj.pntInventory[11]->IsItem() == 1)
		{
			if (Obj.pntInventory[11]->m_Option3 != 0 && Obj.pntInventory[11]->m_Durability != 0.0f && Obj.pntInventory[11]->m_Type != ITEMGET(13, 24))
			{
				percent += Obj.pntInventory[11]->m_Option3;
			}
		}

		if (Obj.pntInventory[9]->IsItem() == 1)
		{
			if (Obj.pntInventory[9]->m_Option3 != 0 && Obj.pntInventory[9]->m_Durability != 0.0f && Obj.pntInventory[9]->m_Type != ITEMGET(13, 28))
			{
				percent += Obj.pntInventory[9]->m_Option3;
			}
		}

		if (Obj.pntInventory[7]->IsItem() == 1)
		{
			if (Obj.pntInventory[7]->m_Option3 != 0 && Obj.pntInventory[7]->m_Special[2] == 85 && Obj.pntInventory[7]->m_Durability != 0.0f)
			{
				percent += Obj.pntInventory[7]->m_Option3;
			}
		}

		if (!(BC_MAP_RANGE(Obj.MapNumber) ||
			CC_MAP_RANGE(Obj.MapNumber) ||
			KALIMA_MAP_RANGE(Obj.MapNumber) ||
			DS_MAP_RANGE(Obj.MapNumber) ||
			IT_MAP_RANGE(Obj.MapNumber) ||
			Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL) &&
			!(Obj.MapNumber == MAP_INDEX_CASTLESIEGE ||
				Obj.MapNumber == MAP_INDEX_CASTLEHUNTZONE))
		{
			int iValue = gObjGetTotalValueOfEffect(Obj, EFFECTTYPE_RECOVERY_HP);

			if (iValue > 0)
			{
				percent += iValue;
			}
		}

		if (Obj.m_PlayerData->m_MPSkillOpt->iMpsAutoRecoverLife > 0.0)
		{
			percent += Obj.m_PlayerData->m_MPSkillOpt->iMpsAutoRecoverLife;
		}

		percent += Obj.AutoHPRecovery;

		if (percent != 0.0)
		{
			Obj.m_LifeFillCount++;

			if (Obj.m_LifeFillCount > 6)
			{
				Obj.m_LifeFillCount = 0;

				tlife = Obj.Life;

				tlife += (Obj.MaxLife + Obj.AddLife) * percent / 100.f;
				tlife += Obj.m_PlayerData->m_JewelOfHarmonyEffect->HJOpAddRefillHP;

				if (Obj.m_PlayerData->m_btRefillHPSocketOption > 0) //season4 add-on
				{
					tlife += Obj.m_PlayerData->m_btRefillHPSocketOption;
				}

				if ((Obj.MaxLife + Obj.AddLife) < tlife)
				{
					tlife = Obj.MaxLife + Obj.AddLife;
				}

				Obj.Life = tlife;

				gGameProtocol.GCReFillSend(Obj, Obj.Life, -1, 0, Obj.iShield);
			}
		}
	}

	if (Obj.iFillShield > 0)
	{
		int iFillSDPoint = 0;
		if (Obj.iFillShieldCount <= 0)
		{
			Obj.iFillShieldCount = 1;
		}

		iFillSDPoint = Obj.iFillShieldMax / Obj.iFillShieldCount;

		Obj.iShield += iFillSDPoint;

		Obj.iFillShield -= iFillSDPoint;

		if (Obj.iFillShield < iFillSDPoint)
		{
			iFillSDPoint = Obj.iFillShield;
			Obj.iFillShield = 0;

			if (iFillSDPoint < 0)
			{
				iFillSDPoint = 0;
			}
		}

		if (Obj.iShield >(Obj.iMaxShield + Obj.iAddShield))
		{
			Obj.iShield = Obj.iMaxShield + Obj.iAddShield;

			Obj.iFillShield = 0;
			Obj.iFillShieldMax = 0;
		}

		gGameProtocol.GCReFillSend(Obj, Obj.Life, -1, 0, Obj.iShield);
	}

	if (Obj.FillLife > 0)
	{
		float cur = Obj.FillLifeMax * 34.0f / 100.0f;

		if (Obj.FillLifeCount == 2)
		{
			cur = Obj.FillLifeMax * 54.0f / 100.0f;
		}

		if (Obj.FillLifeCount == 4)
		{
			cur = Obj.FillLifeMax * 25.0f / 100.0f;
		}

		if (Obj.FillLife < cur)
		{
			cur = Obj.FillLife;
			Obj.FillLife = 0;

			if (cur < 0)
			{
				cur = 0;
			}
		}
		else
		{
			Obj.FillLife -= cur;
		}

		Obj.Life += cur;

		if ((Obj.MaxLife + Obj.AddLife) < Obj.Life)
		{
			Obj.Life = Obj.MaxLife + Obj.AddLife;
			Obj.FillLife = 0;
		}

		gGameProtocol.GCReFillSend(Obj, Obj.Life, -1, 0, Obj.iShield);
	}
}




void gObjUseDrink(CGameObject &Obj, int level)
{
	int time;

	if (level == 2)
	{
		time = 90;
		gGameProtocol.GCItemObjectUseSpecialTimeSend(Obj, 1, time);
		Obj.m_SkillInfo->RemedyOfLoveEffect = 1;
		Obj.m_SkillInfo->RemedyOfLoveTime = 90;
	}
	else if (level == 1)
	{
		time = 180;
		gGameProtocol.GCItemObjectUseSpecialTimeSend(Obj, 0, time);
	}
	else if (level == 0x4D)
	{
		time = 60;
		gGameProtocol.GCItemObjectUseSpecialTimeSend(Obj, 2, time);
	}
	else
	{
		time = 80;
		gGameProtocol.GCItemObjectUseSpecialTimeSend(Obj, 0, time);
	}
}

void gObjViewportListProtocolDestroy(CGameObject &Obj)
{
	BYTE sendBuf[255];
	int lOfs, count;
	count = 0;
	lOfs = 4;

	PBMSG_COUNT pCount;
	PBMSG_COUNT2 pCount2;

	pViewportDestroy.NumberH = SET_NUMBERH(Obj.m_Index);
	pViewportDestroy.NumberL = SET_NUMBERL(Obj.m_Index);

	std::memcpy(&sendBuf[lOfs], &pViewportDestroy, 2);

	lOfs += 2;
	count += 1;

	pCount.h.c = 0xC1;
	pCount.h.headcode = 0x14;
	pCount.h.size = lOfs;
	pCount.count = count;

	std::memcpy(sendBuf, &pCount, sizeof(pCount));

	if (count < 1)
	{
		return;
	}

	gGameProtocol.MsgSendV2(Obj, sendBuf, lOfs);
}

void gObjViewportListProtocolCreate(CGameObject &Obj)
{
	int n, tObjNum;
	BYTE sendBuf[5000];
	int lOfs, count, moncount;
	PWMSG_COUNT pwCount;

	count = 0;
	moncount = 0;
	int iViewportSize = 0;
	lOfs = sizeof(pwCount);

	int MVL = MAX_VIEWPORT;

	if (Obj.Type == OBJ_MONSTER)
	{
		MVL = MaxViewportMonster;
	}

	if (Obj.Type == OBJ_USER)
	{
		if (Obj.m_Change >= 0 && (Obj.m_PlayerData->m_EnableUseChangeSkin == true || IsTransformCharacterSkin(Obj.m_Change) == false))
		{
			pViewportCreateChange.NumberH = HIBYTE(Obj.m_Index);
			pViewportCreateChange.NumberL = LOBYTE(Obj.m_Index);

			pViewportCreateChange.NumberH |= (0x01 << 7);

			pViewportCreateChange.X = (BYTE)Obj.X;
			pViewportCreateChange.Y = (BYTE)Obj.Y;
			pViewportCreateChange.TX = (BYTE)Obj.TX;
			pViewportCreateChange.TY = (BYTE)Obj.TY;
			pViewportCreateChange.SkinH = HIBYTE(LOWORD(Obj.m_Change));
			pViewportCreateChange.SkinL = LOBYTE(LOWORD(Obj.m_Change));
			if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_GM_MARK) == true)	gObjAddBuffEffect(Obj, BUFFTYPE_GM_MARK, 0, 0, 0, 0, -10);
			pViewportCreateChange.DirAndPkLevel = Obj.Dir << 4;

			if (Obj.m_PlayerData->RegisterdLMS == 1 && Obj.MapNumber == g_LastManStanding.m_Cfg.iPVPMap && g_LastManStanding.m_Rooms[Obj.m_PlayerData->RegisteredLMSRoom].bState >= 2)
			{
				pViewportCreateChange.DirAndPkLevel |= (6 & 0x0F);
			}
			else
			{
				pViewportCreateChange.DirAndPkLevel |= (Obj.m_PK_Level & 0x0F);
			}

			pViewportCreateChange.PentagramMainAttribute = Obj.m_iPentagramMainAttribute;
			pViewportCreateChange.MuunItemH = SET_NUMBERH(Obj.m_wMuunItem);
			pViewportCreateChange.MuunItemL = SET_NUMBERL(Obj.m_wMuunItem);
			pViewportCreateChange.MuunSubItemH = SET_NUMBERH(Obj.m_wMuunSubItem);
			pViewportCreateChange.MuunSubItemL = SET_NUMBERL(Obj.m_wMuunSubItem);
			pViewportCreateChange.MuunRideItemH = SET_NUMBERH(Obj.m_wMuunRideItem);
			pViewportCreateChange.MuunRideItemL = SET_NUMBERL(Obj.m_wMuunRideItem);

			if (Obj.m_wMuunRideItem == (WORD)-1)
			{
				pViewportCreateChange.MuunRideItemH = SET_NUMBERH(Obj.m_wInvenPet);
				pViewportCreateChange.MuunRideItemL = SET_NUMBERL(Obj.m_wInvenPet);
			}

			pViewportCreateChange.LevelH = SET_NUMBERH(Obj.Level);
			pViewportCreateChange.LevelL = SET_NUMBERL(Obj.Level);
			pViewportCreateChange.MaxLifeHH = (int)(Obj.AddLife + Obj.MaxLife) >> 24;
			pViewportCreateChange.MaxLifeHL = (int)(Obj.AddLife + Obj.MaxLife) >> 8;
			pViewportCreateChange.MaxLifeLH = (int)(Obj.AddLife + Obj.MaxLife) >> 16;
			pViewportCreateChange.MaxLifeLL = (int)(Obj.AddLife + Obj.MaxLife);
			pViewportCreateChange.CurLifeHH = (int)Obj.Life >> 24;
			pViewportCreateChange.CurLifeHL = (int)Obj.Life >> 8;
			pViewportCreateChange.CurLifeLH = (int)Obj.Life >> 16;
			pViewportCreateChange.CurLifeLL = (int)Obj.Life;

			Obj.CharSet[0] &= 0xFC;
			Obj.CharSet[0] |= Obj.m_ViewState & 0x03;

			std::memcpy(&pViewportCreateChange.CharSet, Obj.CharSet, 18);

			if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
			{
				std::memcpy(pViewportCreateChange.Id, Obj.m_PlayerData->m_RealNameOfUBF, MAX_ACCOUNT_LEN);
				pViewportCreateChange.nServerCodeOfHomeWorld = Obj.m_PlayerData->m_nServerCodeOfHomeWorld;
			}

			else
			{
				std::memcpy(pViewportCreateChange.Id, Obj.Name, MAX_ACCOUNT_LEN);
				pViewportCreateChange.nServerCodeOfHomeWorld = 0;
			}

			if (strlen(pViewportCreateChange.Id) < 1)
			{
				sLog->outBasic("[UBF][gObjViewportListProtocolCreate][Name Setting Failed] (%d) ID : %s, Name: %s)",
					Obj.m_Index, Obj.AccountID, Obj.Name);
			}

			pViewportCreateChange.BuffEffectCount = iViewportSize = gObjMakeViewportState(Obj, pViewportCreateChange.BuffEffectList);

			std::memcpy((sendBuf + lOfs), (char*)&pViewportCreateChange, sizeof(PMSG_VIEWPORTCREATE_CHANGE) - (32 - iViewportSize));
			lOfs += sizeof(PMSG_VIEWPORTCREATE_CHANGE) - (32 - iViewportSize);
			count++;

			if (count < 1)
			{
				return;
			}
			pwCount.h.headcode = 0x45;
		}
		else
		{
			pViewportCreate.NumberH = HIBYTE(Obj.m_Index);
			pViewportCreate.NumberL = LOBYTE(Obj.m_Index);

			Obj.CharSet[0] &= 0xFC;
			Obj.CharSet[0] |= Obj.m_ViewState & 0x03;

			if (Obj.m_Change < 0)
			{
				Obj.m_PlayerData->m_EnableUseChangeSkin = false;
			}

			else
			{
				if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_GM_MARK) == TRUE)
				{
					gObjAddBuffEffect(Obj, BUFFTYPE_GM_MARK, 0, 0, 0, 0, -10);
				}
			}

			if (Obj.m_State == 1)
			{
				if (!Obj.Teleport)
				{
					pViewportCreate.NumberH |= (0x01 << 7);
				}
			}

			pViewportCreate.X = (BYTE)Obj.X;
			pViewportCreate.Y = (BYTE)Obj.Y;
			pViewportCreate.TX = (BYTE)Obj.TX;
			pViewportCreate.TY = (BYTE)Obj.TY;
			pViewportCreate.DirAndPkLevel = Obj.Dir << 4;

			if (Obj.m_PlayerData->RegisterdLMS == 1 && Obj.MapNumber == g_LastManStanding.m_Cfg.iPVPMap && g_LastManStanding.m_Rooms[Obj.m_PlayerData->RegisteredLMSRoom].bState >= 2)
			{
				pViewportCreate.DirAndPkLevel |= (6 & 0x0F);
			}
			else
			{
				pViewportCreate.DirAndPkLevel |= (Obj.m_PK_Level & 0x0F);
			}

			std::memcpy(&pViewportCreate.CharSet, Obj.CharSet, 18);

			if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
			{
				std::memcpy(pViewportCreate.Id, Obj.m_PlayerData->m_RealNameOfUBF, MAX_ACCOUNT_LEN);
				pViewportCreate.nServerCodeOfHomeWorld = Obj.m_PlayerData->m_nServerCodeOfHomeWorld;
			}

			else
			{
				std::memcpy(pViewportCreate.Id, Obj.Name, MAX_ACCOUNT_LEN);
				pViewportCreate.nServerCodeOfHomeWorld = 0;
			}

			if (strlen(pViewportCreate.Id) < 1)
			{
				sLog->outBasic("[UBF][gObjViewportListProtocolCreate_2][Name Setting Failed] (%d) ID : %s, Name: %s)",
					Obj.m_Index, Obj.AccountID, Obj.Name);
			}

			pViewportCreate.PentagramMainAttribute = Obj.m_iPentagramMainAttribute;
			pViewportCreate.MuunItemH = SET_NUMBERH(Obj.m_wMuunItem);
			pViewportCreate.MuunItemL = SET_NUMBERL(Obj.m_wMuunItem);
			pViewportCreate.MuunSubItemH = SET_NUMBERH(Obj.m_wMuunSubItem);
			pViewportCreate.MuunSubItemL = SET_NUMBERL(Obj.m_wMuunSubItem);
			pViewportCreate.MuunRideItemH = SET_NUMBERH(Obj.m_wMuunRideItem);
			pViewportCreate.MuunRideItemL = SET_NUMBERL(Obj.m_wMuunRideItem);

			if (Obj.m_wMuunRideItem == (WORD)-1)
			{
				pViewportCreate.MuunRideItemH = SET_NUMBERH(Obj.m_wInvenPet);
				pViewportCreate.MuunRideItemL = SET_NUMBERL(Obj.m_wInvenPet);
			}

			pViewportCreate.LevelH = SET_NUMBERH(Obj.Level);
			pViewportCreate.LevelL = SET_NUMBERL(Obj.Level);
			pViewportCreate.MaxLifeHH = (int)(Obj.AddLife + Obj.MaxLife) >> 24;
			pViewportCreate.MaxLifeHL = (int)(Obj.AddLife + Obj.MaxLife) >> 8;
			pViewportCreate.MaxLifeLH = (int)(Obj.AddLife + Obj.MaxLife) >> 16;
			pViewportCreate.MaxLifeLL = (int)(Obj.AddLife + Obj.MaxLife);
			pViewportCreate.CurLifeHH = (int)Obj.Life >> 24;
			pViewportCreate.CurLifeHL = (int)Obj.Life >> 8;
			pViewportCreate.CurLifeLH = (int)Obj.Life >> 16;
			pViewportCreate.CurLifeLL = (int)Obj.Life;
			pViewportCreate.BuffEffectCount = iViewportSize = gObjMakeViewportState(Obj, pViewportCreate.BuffEffectList);

			std::memcpy((sendBuf + lOfs), (char*)&pViewportCreate, sizeof(PMSG_VIEWPORTCREATE));
			lOfs += sizeof(PMSG_VIEWPORTCREATE) - (32 - iViewportSize);
			count++;

			if (count < 1)
			{
				return;
			}

			pwCount.h.headcode = 0x12;
		}

		pwCount.h.c = 0xC2;
		pwCount.h.sizeH = SET_NUMBERH(lOfs);
		pwCount.h.sizeL = SET_NUMBERL(lOfs);
		pwCount.count = count;

		std::memcpy(sendBuf, &pwCount, sizeof(pwCount));

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, sendBuf, lOfs); //[K2]

		if (Obj.m_PlayerData->GuildNumber != 0)
		{
			GuildInfoOfs = 0;
			GuildInfoOfs = 5;

			PMSG_SIMPLE_GUILDVIEWPORT pGuild;

			pGuild.GuildNumber = Obj.m_PlayerData->lpGuild->Number;
			pGuild.NumberH = SET_NUMBERH(Obj.m_Index) & 0x7F;
			pGuild.NumberL = SET_NUMBERL(Obj.m_Index);

			pGuild.NumberH |= 0x80;

			pGuild.btGuildType = Obj.m_PlayerData->lpGuild->btGuildType;
			pGuild.btGuildStatus = Obj.m_PlayerData->GuildStatus;

			if (g_CastleSiegeSync.CheckCastleOwnerMember(Obj) == TRUE || //season 4 add-on
				g_CastleSiegeSync.CheckCastleOwnerUnionMember(Obj) == TRUE)
			{
				pGuild.btOwnerStatus = 1;
			}
			else
			{
				pGuild.btOwnerStatus = 0;
			}

			std::memcpy(&GuildInfoBuf[GuildInfoOfs], &pGuild, sizeof(pGuild));

			GuildInfoOfs += sizeof(pGuild);

			PMSG_SIMPLE_GUILDVIEWPORT_COUNT pwCount;

			pwCount.h.c = 0xC2;
			pwCount.h.headcode = 0x65;
			pwCount.h.sizeH = SET_NUMBERH(GuildInfoOfs);
			pwCount.h.sizeL = SET_NUMBERL(GuildInfoOfs);
			pwCount.Count = 1;

			std::memcpy(GuildInfoBuf, &pwCount, sizeof(pwCount));

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, GuildInfoBuf, GuildInfoOfs);
		}

		g_GensSystem.SendGensInfo(Obj);

		for (int n = 0; n < MVL; n++)
		{
			if (Obj.VpPlayer2[n].state != 0)
			{
				tObjNum = Obj.VpPlayer2[n].number;

				if (tObjNum >= 0)
				{
					if (Obj.VpPlayer2[n].type == OBJ_USER)
					{
						IOCP.DataSend(tObjNum, sendBuf, lOfs);

						if (GuildInfoCount != 0)
						{
							IOCP.DataSend(tObjNum, GuildInfoBuf, GuildInfoOfs);
						}

						if (GuildUserCount != 0)
						{
							IOCP.DataSend(tObjNum, GuildUserBuf, GuildUserOfs);
						}
					}
				}
			}
		}

		g_GensSystem.GensViewportListProtocol(Obj);
	}
	else
	{
		count = 0;

		if ((Obj.m_RecallMon >= 0 && (Obj.Class < 678 || Obj.Class > 680)) || g_EvoMonMng.IsEvolutedEvoMon(Obj) == true || Obj.Class == 690)
		{
			pCallMonsterViewportCreate.NumberH = HIBYTE(Obj.m_Index);
			pCallMonsterViewportCreate.NumberL = LOBYTE(Obj.m_Index);
			if (Obj.m_State == 1)
				pCallMonsterViewportCreate.NumberH |= (0x01 << 7);

			pCallMonsterViewportCreate.Type_HI = HIBYTE(Obj.Class);
			pCallMonsterViewportCreate.Type_LO = LOBYTE(Obj.Class);
			pCallMonsterViewportCreate.X = (BYTE)Obj.X;
			pCallMonsterViewportCreate.Y = (BYTE)Obj.Y;
			pCallMonsterViewportCreate.TX = (BYTE)Obj.TX;
			pCallMonsterViewportCreate.TY = (BYTE)Obj.TY;
			pCallMonsterViewportCreate.Path = Obj.Dir << 4;
			pCallMonsterViewportCreate.PentagramMainAttribute = Obj.m_iPentagramMainAttribute;
			pCallMonsterViewportCreate.LevelH = SET_NUMBERH(Obj.Level);
			pCallMonsterViewportCreate.LevelL = SET_NUMBERL(Obj.Level);
			pCallMonsterViewportCreate.MaxLifeHH = (int)(Obj.AddLife + Obj.MaxLife) >> 24;
			pCallMonsterViewportCreate.MaxLifeHL = (int)(Obj.AddLife + Obj.MaxLife) >> 8;
			pCallMonsterViewportCreate.MaxLifeLH = (int)(Obj.AddLife + Obj.MaxLife) >> 16;
			pCallMonsterViewportCreate.MaxLifeLL = (int)(Obj.AddLife + Obj.MaxLife);
			pCallMonsterViewportCreate.CurLifeHH = (int)Obj.Life >> 24;
			pCallMonsterViewportCreate.CurLifeHL = (int)Obj.Life >> 8;
			pCallMonsterViewportCreate.CurLifeLH = (int)Obj.Life >> 16;
			pCallMonsterViewportCreate.CurLifeLL = (int)Obj.Life;

			int iOwnerIndex = Obj.m_RecallMon;

			if (Obj.Class == 681 || Obj.Class == 690)
			{
				iOwnerIndex = Obj.m_nEvoMonOwnerIndex;
			}

			if (Obj.Class == 707) // Dark Wizard Phantom
			{
				pCallMonsterViewportCreate.SummonType = OBJ_USER;

				if (ObjectMaxRange(iOwnerIndex) == true && getGameObject(iOwnerIndex)->Type == OBJ_USER)
				{
					std::memcpy(pCallMonsterViewportCreate.CharSet, getGameObject(iOwnerIndex)->CharSet, 18);
				}
			}

			else
			{
				pCallMonsterViewportCreate.SummonType = OBJ_MONSTER;
				memset(pCallMonsterViewportCreate.CharSet, 0x00, CHAR_SET_SIZE);
			}

			if (ObjectMaxRange(iOwnerIndex) == true && getGameObject(iOwnerIndex)->Type == OBJ_USER)
			{
				std::memcpy(pCallMonsterViewportCreate.Id, getGameObject(iOwnerIndex)->Name, sizeof(pCallMonsterViewportCreate.Id));

			}
			else
			{
				memset(pCallMonsterViewportCreate.Id, 0x00, sizeof(pCallMonsterViewportCreate.Id));
			}

			if (ObjectMaxRange(iOwnerIndex) == true && getGameObject(iOwnerIndex)->Type == OBJ_USER)
			{
				std::memcpy(pCallMonsterViewportCreate.Id, getGameObject(iOwnerIndex)->Name, MAX_ACCOUNT_LEN);
			}

			else memset(pCallMonsterViewportCreate.Id, 0, MAX_ACCOUNT_LEN);

			pCallMonsterViewportCreate.BuffEffectCount = iViewportSize = gObjMakeViewportState(Obj, pCallMonsterViewportCreate.BuffEffectList);

			std::memcpy((sendBuf + lOfs), (char*)&pCallMonsterViewportCreate, sizeof(PMSG_CALLMONSTER_VIEWPORTCREATE));
			lOfs += sizeof(PMSG_CALLMONSTER_VIEWPORTCREATE) - (32 - iViewportSize);
			count++;
		}
		else
		{
			pMonsterViewportCreate.NumberH = HIBYTE(Obj.m_Index);
			pMonsterViewportCreate.NumberL = LOBYTE(Obj.m_Index);
			if (Obj.m_State == 1 && Obj.Teleport == 0)
			{
				pMonsterViewportCreate.NumberH |= (0x01 << 7);
				pMonsterViewportCreate.NumberH |= (0x01 << 6);
			}
			pMonsterViewportCreate.Type_HI = HIBYTE(Obj.Class);
			pMonsterViewportCreate.Type_LO = LOBYTE(Obj.Class);
			pMonsterViewportCreate.X = (BYTE)Obj.X;
			pMonsterViewportCreate.Y = (BYTE)Obj.Y;
			pMonsterViewportCreate.TX = (BYTE)Obj.TX;
			pMonsterViewportCreate.TY = (BYTE)Obj.TY;
			pMonsterViewportCreate.Path = Obj.Dir << 4;
			pMonsterViewportCreate.PentagramMainAttribute = Obj.m_iPentagramMainAttribute;
			pMonsterViewportCreate.LevelH = SET_NUMBERH(Obj.Level);
			pMonsterViewportCreate.LevelL = SET_NUMBERL(Obj.Level);
			pMonsterViewportCreate.MaxLifeHH = (int)(Obj.AddLife + Obj.MaxLife) >> 24;
			pMonsterViewportCreate.MaxLifeHL = (int)(Obj.AddLife + Obj.MaxLife) >> 8;
			pMonsterViewportCreate.MaxLifeLH = (int)(Obj.AddLife + Obj.MaxLife) >> 16;
			pMonsterViewportCreate.MaxLifeLL = (int)(Obj.AddLife + Obj.MaxLife);
			pMonsterViewportCreate.CurLifeHH = (int)Obj.Life >> 24;
			pMonsterViewportCreate.CurLifeHL = (int)Obj.Life >> 8;
			pMonsterViewportCreate.CurLifeLH = (int)Obj.Life >> 16;
			pMonsterViewportCreate.CurLifeLL = (int)Obj.Life;
			pMonsterViewportCreate.BuffEffectCount = iViewportSize = gObjMakeViewportState(Obj, pMonsterViewportCreate.BuffEffectList);
			std::memcpy((sendBuf + lOfs), (char*)&pMonsterViewportCreate, sizeof(PMSG_MONSTER_VIEWPORTCREATE));
			lOfs += sizeof(PMSG_MONSTER_VIEWPORTCREATE) - (32 - iViewportSize);
			moncount++;
		}

		if (count > 0)
		{
			PWMSG_COUNT pwCount;

			pwCount.h.c = 0xC2;
			pwCount.h.headcode = 0x1F;
			pwCount.h.sizeH = SET_NUMBERH(lOfs);
			pwCount.h.sizeL = SET_NUMBERL(lOfs);
			pwCount.count = count;

			std::memcpy(sendBuf, &pwCount, sizeof(pwCount));
			gGameProtocol.MsgSendV2(Obj, sendBuf, lOfs);
		}

		if (moncount > 0)
		{
			PWMSG_COUNT pwCount;

			pwCount.h.c = 0xC2;
			pwCount.h.headcode = 0x13;
			pwCount.h.sizeH = SET_NUMBERH(lOfs);
			pwCount.h.sizeL = SET_NUMBERL(lOfs);
			pwCount.count = moncount;

			std::memcpy(sendBuf, &pwCount, sizeof(pwCount));
			gGameProtocol.MsgSendV2(Obj, sendBuf, lOfs);
		}
	}
}

void gObjViewportListProtocol(CGameObject &Obj)
{
	if (Obj.Connected < PLAYER_PLAYING)
	{
		return;
	}

	PBMSG_COUNT pCount;
	PBMSG_COUNT2 pCount2;
	CGameObject* lpTargetObj;
	int lOfs_Item, count_Item, ItemStructSize = sizeof(PMSG_ITEMVIEWPORTCREATE), lOfs, MonlOfs, callMonlOfs;
	int pMsgSize;
	short tObjNum;
	BYTE count, monstercount, callmonstercount;

	BYTE sendBuf[5000];
	BYTE MonstersendBuf[5000];
	BYTE callMonstersendBuf[5000];
	BYTE ItemBuf[5000];

	GuildInfoOfs = 0;
	GuildInfoCount = 0;
	GensInfoOfs = 0;
	GensInfoCount = 0;
	GuildUserOfs = 0;
	GuildUserCount = 0;
	ChangeCount = 0;
	GuildInfoOfs = 5;
	GuildUserOfs = 5;
	GensInfoOfs = 6;
	lOfsChange = 5;

	lOfs = 4;
	lOfs_Item = 5;
	count = 0;
	int iViewportSize = 0;
	count_Item = 0;

	if (Obj.Type == OBJ_USER)
	{
		for (int n = 0; n < MAX_VIEWPORT; n++)
		{
			if (Obj.VpPlayer[n].state == 3)
			{
				tObjNum = Obj.VpPlayer[n].number;

				if (tObjNum >= 0)
				{
					switch (Obj.VpPlayer[n].type)
					{
					case 1:
					case 2:
					case 3:
						pViewportDestroy.NumberH = SET_NUMBERH(tObjNum);
						pViewportDestroy.NumberL = SET_NUMBERL(tObjNum);

						std::memcpy(&sendBuf[lOfs], &pViewportDestroy, sizeof(pViewportDestroy));
						lOfs += sizeof(pViewportDestroy);
						count += 1;
						break;
					case 5:
						pItemViewportDestroy.NumberH = SET_NUMBERH(tObjNum);
						pItemViewportDestroy.NumberL = SET_NUMBERL(tObjNum);

						std::memcpy(&ItemBuf[lOfs_Item], &pItemViewportDestroy, sizeof(pItemViewportDestroy));

						lOfs_Item += sizeof(pItemViewportDestroy);
						count_Item += 1;
						break;
					}
				}

				Obj.VpPlayer[n].state = 0;
				Obj.VpPlayer[n].number = -1;
				Obj.VPCount -= 1;
			}
		}
	}

	else if (Obj.Type == OBJ_MONSTER || Obj.Type == OBJ_NPC)
	{
		for (int n = 0; n < MaxViewportMonster; n++)
		{
			if (Obj.VpPlayer[n].state == 3)
			{
				Obj.VpPlayer[n].state = 0;
				Obj.VpPlayer[n].number = -1;
				Obj.VPCount -= 1;
			}
		}
	}

	if (Obj.Type == OBJ_USER)
	{
		if (count > 0)
		{
			pCount.h.c = 0xC1;
			pCount.h.headcode = 0x14;
			pCount.h.size = lOfs;
			pCount.count = count;

			std::memcpy(sendBuf, &pCount, sizeof(pCount));
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, sendBuf, lOfs);
		}

		if (count_Item > 0)
		{
			pItemCount.h.c = 0xC2;
			pItemCount.h.headcode = 0x21;
			pItemCount.h.sizeH = SET_NUMBERH(lOfs_Item);
			pItemCount.h.sizeL = SET_NUMBERL(lOfs_Item);
			pItemCount.count = count_Item;

			std::memcpy(ItemBuf, &pItemCount, sizeof(pItemCount));
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, ItemBuf, lOfs_Item);
		}
	}

	if (Obj.Type == OBJ_MONSTER || Obj.Type == OBJ_NPC)
	{
		for (int n = 0; n < MaxViewportMonster; n++)
		{
			if (Obj.VpPlayer[n].state == 1)
			{
				Obj.VpPlayer[n].state = 2;
			}
		}
	}
	else
	{
		count = 0;
		monstercount = 0;
		count_Item = 0;
		callmonstercount = 0;
		lOfs = 5;
		MonlOfs = lOfs;
		callMonlOfs = MonlOfs;
		lOfs_Item = 5;
		pMsgSize = 44;

		if (Obj.Type == OBJ_USER)
		{
			for (int n = 0; n < MAX_VIEWPORT; n++)
			{
				if (Obj.VpPlayer[n].state == 1)
				{
					tObjNum = Obj.VpPlayer[n].number;

					if (tObjNum >= 0)
					{
						if ((getGameObject(tObjNum)->Authority & 0x20) == 0x20 && gObjCheckUsedBuffEffect(*getGameObject(tObjNum), BUFFTYPE_INVISABLE))
							continue;

						if (g_NewPVP.IsObserver(*getGameObject(tObjNum)) == TRUE)
						{
							continue;
						}

						switch (Obj.VpPlayer[n].type)
						{
						case 1:
							lpTargetObj = getGameObject(tObjNum);

							if (lpTargetObj->m_Change >= 0 && (lpTargetObj->m_PlayerData->m_EnableUseChangeSkin == true || IsTransformCharacterSkin(lpTargetObj->m_Change) == false))
							{
								pViewportCreateChange.NumberH = SET_NUMBERH(tObjNum);
								pViewportCreateChange.NumberL = SET_NUMBERL(tObjNum);

								lpTargetObj->CharSet[0] &= 0xFC;
								lpTargetObj->CharSet[0] |= lpTargetObj->m_ViewState & 0x03;

								if ((lpTargetObj->Authority & 0x20) == 0x20)
								{
									gObjAddBuffEffect(*lpTargetObj, BUFFTYPE_GM_MARK, 0, 0, 0, 0, -10);
								}

								if (lpTargetObj->m_State == 1 && lpTargetObj->Teleport == 0)
								{
									pViewportCreateChange.NumberH |= 0x80;
								}

								pViewportCreateChange.X = lpTargetObj->X;
								pViewportCreateChange.Y = lpTargetObj->Y;
								pViewportCreateChange.TX = lpTargetObj->TX;
								pViewportCreateChange.TY = lpTargetObj->TY;
								pViewportCreateChange.SkinH = SET_NUMBERH((lpTargetObj->m_Change & 0xFFFF) & 0xFFFF);
								pViewportCreateChange.SkinL = SET_NUMBERL((lpTargetObj->m_Change & 0xFFFF) & 0xFFFF);
								pViewportCreateChange.DirAndPkLevel = lpTargetObj->Dir << 4;

								if (lpTargetObj->m_PlayerData->RegisterdLMS == 1 && Obj.MapNumber == g_LastManStanding.m_Cfg.iPVPMap && g_LastManStanding.m_Rooms[Obj.m_PlayerData->RegisteredLMSRoom].bState >= 2)
								{
									pViewportCreateChange.DirAndPkLevel |= (6 & 0x0F);
								}
								else
								{
									pViewportCreateChange.DirAndPkLevel |= (lpTargetObj->m_PK_Level & 0x0F);
								}

								Obj.CharSet[0] &= 0xFC;
								Obj.CharSet[0] |= Obj.m_ViewState & 0x03;

								pViewportCreateChange.PentagramMainAttribute = lpTargetObj->m_iPentagramMainAttribute;
								pViewportCreateChange.MuunItemH = SET_NUMBERH(lpTargetObj->m_wMuunItem);
								pViewportCreateChange.MuunItemL = SET_NUMBERL(lpTargetObj->m_wMuunItem);
								pViewportCreateChange.MuunSubItemH = SET_NUMBERH(lpTargetObj->m_wMuunSubItem);
								pViewportCreateChange.MuunSubItemL = SET_NUMBERL(lpTargetObj->m_wMuunSubItem);
								pViewportCreateChange.MuunRideItemH = SET_NUMBERH(lpTargetObj->m_wMuunRideItem);
								pViewportCreateChange.MuunRideItemL = SET_NUMBERL(lpTargetObj->m_wMuunRideItem);

								if (lpTargetObj->m_wMuunRideItem == (WORD)-1)
								{
									pViewportCreateChange.MuunRideItemH = SET_NUMBERH(lpTargetObj->m_wInvenPet);
									pViewportCreateChange.MuunRideItemL = SET_NUMBERL(lpTargetObj->m_wInvenPet);
								}

								pViewportCreateChange.LevelH = SET_NUMBERH(lpTargetObj->Level);
								pViewportCreateChange.LevelL = SET_NUMBERL(lpTargetObj->Level);
								pViewportCreateChange.MaxLifeHH = (int)(lpTargetObj->AddLife + lpTargetObj->MaxLife) >> 24;
								pViewportCreateChange.MaxLifeHL = (int)(lpTargetObj->AddLife + lpTargetObj->MaxLife) >> 8;
								pViewportCreateChange.MaxLifeLH = (int)(lpTargetObj->AddLife + lpTargetObj->MaxLife) >> 16;
								pViewportCreateChange.MaxLifeLL = (int)(lpTargetObj->AddLife + lpTargetObj->MaxLife);
								pViewportCreateChange.CurLifeHH = (int)lpTargetObj->Life >> 24;
								pViewportCreateChange.CurLifeHL = (int)lpTargetObj->Life >> 8;
								pViewportCreateChange.CurLifeLH = (int)lpTargetObj->Life >> 16;
								pViewportCreateChange.CurLifeLL = (int)lpTargetObj->Life;

								std::memcpy(pViewportCreateChange.CharSet, lpTargetObj->CharSet, sizeof(pViewportCreateChange.CharSet)); //New for Fix Rings of Transformation

								if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
								{
									std::memcpy(pViewportCreateChange.Id, lpTargetObj->m_PlayerData->m_RealNameOfUBF, MAX_ACCOUNT_LEN);
									pViewportCreateChange.nServerCodeOfHomeWorld = lpTargetObj->m_PlayerData->m_nServerCodeOfHomeWorld;
								}

								else
								{
									std::memcpy(pViewportCreateChange.Id, lpTargetObj->Name, sizeof(pViewportCreateChange.Id));
									pViewportCreateChange.nServerCodeOfHomeWorld = 0;
								}

								if (strlen(pViewportCreateChange.Id) < 1)
								{
									sLog->outBasic("[UBF][gObjViewportListProtocol][Name Setting Failed] (%d) ID : %s, Name: %s)",
										Obj.m_Index, Obj.AccountID, Obj.Name);
								}

								pViewportCreateChange.BuffEffectCount = iViewportSize = gObjMakeViewportState(*lpTargetObj, pViewportCreateChange.BuffEffectList);

								std::memcpy((SendGBufChange + lOfsChange), (char*)&pViewportCreateChange, sizeof(PMSG_VIEWPORTCREATE_CHANGE));
								lOfsChange += sizeof(PMSG_VIEWPORTCREATE_CHANGE) - (MAX_BUFFEFFECT - iViewportSize);
								ChangeCount++;
							}
							else
							{
								pViewportCreate.NumberH = SET_NUMBERH(tObjNum);
								pViewportCreate.NumberL = SET_NUMBERL(tObjNum);

								lpTargetObj->CharSet[0] &= 0xFC;
								lpTargetObj->CharSet[0] |= lpTargetObj->m_ViewState & 0x03;

								if (lpTargetObj->m_Change < 0)
								{
									lpTargetObj->m_PlayerData->m_EnableUseChangeSkin = false;
								}

								else
								{
									if (gObjCheckUsedBuffEffect(*lpTargetObj, BUFFTYPE_GM_MARK) == TRUE)
									{
										gObjAddBuffEffect(*lpTargetObj, BUFFTYPE_GM_MARK, 0, 0, 0, 0, -10);
									}
								}

								if (lpTargetObj->m_State == 1 && lpTargetObj->Teleport == 0)
								{
									pViewportCreate.NumberH |= 0x80;
								}

								pViewportCreate.X = lpTargetObj->X;
								pViewportCreate.Y = lpTargetObj->Y;
								pViewportCreate.TX = lpTargetObj->TX;
								pViewportCreate.TY = lpTargetObj->TY;

								pViewportCreate.DirAndPkLevel = lpTargetObj->Dir << 4;
								if (Obj.m_PlayerData->RegisterdLMS == 1 && Obj.MapNumber == g_LastManStanding.m_Cfg.iPVPMap && g_LastManStanding.m_Rooms[Obj.m_PlayerData->RegisteredLMSRoom].bState >= 2)
								{
									pViewportCreate.DirAndPkLevel |= (6 & 0x0F);
								}
								else
								{
									pViewportCreate.DirAndPkLevel |= (lpTargetObj->m_PK_Level & 0x0F);
								}

								pViewportCreate.PentagramMainAttribute = lpTargetObj->m_iPentagramMainAttribute;
								pViewportCreate.MuunItemH = SET_NUMBERH(lpTargetObj->m_wMuunItem);
								pViewportCreate.MuunItemL = SET_NUMBERL(lpTargetObj->m_wMuunItem);
								pViewportCreate.MuunSubItemH = SET_NUMBERH(lpTargetObj->m_wMuunSubItem);
								pViewportCreate.MuunSubItemL = SET_NUMBERL(lpTargetObj->m_wMuunSubItem);
								pViewportCreate.MuunRideItemH = SET_NUMBERH(lpTargetObj->m_wMuunRideItem);
								pViewportCreate.MuunRideItemL = SET_NUMBERL(lpTargetObj->m_wMuunRideItem);

								if (lpTargetObj->m_wMuunRideItem == (WORD)-1)
								{
									pViewportCreate.MuunRideItemH = SET_NUMBERH(lpTargetObj->m_wInvenPet);
									pViewportCreate.MuunRideItemL = SET_NUMBERL(lpTargetObj->m_wInvenPet);
								}
								pViewportCreate.LevelH = SET_NUMBERH(lpTargetObj->Level);
								pViewportCreate.LevelL = SET_NUMBERL(lpTargetObj->Level);
								pViewportCreate.MaxLifeHH = (int)(lpTargetObj->AddLife + lpTargetObj->MaxLife) >> 24;
								pViewportCreate.MaxLifeHL = (int)(lpTargetObj->AddLife + lpTargetObj->MaxLife) >> 8;
								pViewportCreate.MaxLifeLH = (int)(lpTargetObj->AddLife + lpTargetObj->MaxLife) >> 16;
								pViewportCreate.MaxLifeLL = (int)(lpTargetObj->AddLife + lpTargetObj->MaxLife);
								pViewportCreate.CurLifeHH = (int)lpTargetObj->Life >> 24;
								pViewportCreate.CurLifeHL = (int)lpTargetObj->Life >> 8;
								pViewportCreate.CurLifeLH = (int)lpTargetObj->Life >> 16;
								pViewportCreate.CurLifeLL = (int)lpTargetObj->Life;

								std::memcpy(pViewportCreate.CharSet, lpTargetObj->CharSet, sizeof(pViewportCreate.CharSet));

								if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
								{
									std::memcpy(pViewportCreate.Id, lpTargetObj->m_PlayerData->m_RealNameOfUBF, MAX_ACCOUNT_LEN);
									pViewportCreate.nServerCodeOfHomeWorld = lpTargetObj->m_PlayerData->m_nServerCodeOfHomeWorld;
								}

								else
								{
									std::memcpy(pViewportCreate.Id, lpTargetObj->Name, sizeof(pViewportCreate.Id));
									pViewportCreate.nServerCodeOfHomeWorld = 0;
								}

								if (strlen(pViewportCreate.Id) < 1)
								{
									sLog->outBasic("[UBF][gObjViewportListProtocol_2][Name Setting Failed] (%d) ID : %s, Name: %s)",
										Obj.m_Index, Obj.AccountID, Obj.Name);
								}

								pViewportCreate.BuffEffectCount = iViewportSize = gObjMakeViewportState(*lpTargetObj, pViewportCreate.BuffEffectList);

								std::memcpy((sendBuf + lOfs), (char*)&pViewportCreate, sizeof(PMSG_VIEWPORTCREATE));
								lOfs += sizeof(PMSG_VIEWPORTCREATE) - (MAX_BUFFEFFECT - iViewportSize);
								count++;
							}

							if (Obj.Type == OBJ_USER)
							{
								if (lpTargetObj->m_PlayerData->lpGuild != 0)
								{
									PMSG_SIMPLE_GUILDVIEWPORT pGuild;

									pGuild.GuildNumber = lpTargetObj->m_PlayerData->lpGuild->Number;
									pGuild.NumberH = SET_NUMBERH(lpTargetObj->m_Index) & 0x7F;
									pGuild.NumberL = SET_NUMBERL(lpTargetObj->m_Index);

									pGuild.btGuildStatus = lpTargetObj->m_PlayerData->GuildStatus;
									pGuild.btGuildType = lpTargetObj->m_PlayerData->lpGuild->btGuildType;

									if (Obj.m_PlayerData->lpGuild != 0)
									{
										pGuild.btGuildRelationShip = gObjGetRelationShip(Obj, *lpTargetObj);
									}
									else
									{
										pGuild.btGuildRelationShip = 0;
									}

									//if (strcmp(lpTargetObj->m_PlayerData->lpGuild->Names[0], lpTargetObj->Name) == 0)
									//{
										pGuild.NumberH |= 0x80;
									//}

									if (g_CastleSiegeSync.CheckCastleOwnerMember(*lpTargetObj) == TRUE || //season 4 add-on
										g_CastleSiegeSync.CheckCastleOwnerUnionMember(*lpTargetObj) == TRUE)
									{
										pGuild.btOwnerStatus = 1;
									}
									else
									{
										pGuild.btOwnerStatus = 0;
									}

									std::memcpy(&GuildInfoBuf[GuildInfoOfs], &pGuild, sizeof(pGuild));
									GuildInfoOfs += sizeof(pGuild);
									GuildInfoCount += 1;
								}

								if (false)
								{

									PMSG_GUILDVIEWPORT_USER pGuildUserViewport;

									if (ViewGuildMng.Add(lpTargetObj->m_PlayerData->GuildNumber, lpTargetObj->m_Index) == 1 && lpTargetObj->m_PlayerData->lpGuild != 0)
									{
										PMSG_GUILDVIEWPORT pGuildViewport;

										pGuildViewport.NumberH = SET_NUMBERH(lpTargetObj->m_PlayerData->lpGuild->Number);
										pGuildViewport.NumberL = SET_NUMBERL(lpTargetObj->m_PlayerData->lpGuild->Number);

										strcpy(pGuildViewport.GuildName, lpTargetObj->m_PlayerData->lpGuild->Name);
										std::memcpy(pGuildViewport.Mark, lpTargetObj->m_PlayerData->lpGuild->Mark, sizeof(pGuildViewport.Mark));
										std::memcpy(&GuildInfoBuf[GuildInfoOfs], &pGuildViewport, sizeof(pGuildViewport));

										GuildInfoOfs += sizeof(pGuildViewport);
										GuildInfoCount += 1;
									}

									pGuildUserViewport.NumberH = SET_NUMBERH(lpTargetObj->m_Index) & 0x7F;
									pGuildUserViewport.NumberL = SET_NUMBERL(lpTargetObj->m_Index);

									//if (lpTargetObj->m_PlayerData->lpGuild != 0 && strcmp(lpTargetObj->m_PlayerData->lpGuild->Names[0], lpTargetObj->Name) == 0)
									//{
										pGuildUserViewport.NumberH |= 0x80;
									//}

									if (lpTargetObj->m_PlayerData->lpGuild != 0)
									{
										pGuildUserViewport.GNumberH = SET_NUMBERH(lpTargetObj->m_PlayerData->lpGuild->Number);
										pGuildUserViewport.GNumberL = SET_NUMBERL(lpTargetObj->m_PlayerData->lpGuild->Number);

										std::memcpy(&GuildUserBuf[GuildUserOfs], &pGuildUserViewport, sizeof(pGuildUserViewport));

										GuildUserOfs += sizeof(pGuildUserViewport);
										GuildUserCount += 1;
									}
								}

								if (g_GensSystem.GetGensInfluence(*lpTargetObj))
								{
									PMSG_GENS_MEMBER_VIEWPORT_INFO pGensMsg;

									pGensMsg.btInfluence = g_GensSystem.GetGensInfluence(*lpTargetObj);
									pGensMsg.NumberH = SET_NUMBERH(lpTargetObj->m_Index);
									pGensMsg.NumberL = SET_NUMBERL(lpTargetObj->m_Index);
									pGensMsg.iGensRanking = lpTargetObj->m_PlayerData->m_GensRanking;
									pGensMsg.iGensClass = lpTargetObj->m_PlayerData->m_GensClass;
									pGensMsg.iContributePoint = lpTargetObj->m_PlayerData->m_ContributePoint;

									std::memcpy(&GensInfoBuf[GensInfoOfs], &pGensMsg, sizeof(pGensMsg));

									GensInfoOfs += sizeof(pGensMsg);
									GensInfoCount += 1;
								}
							}
							break;
						case 2:
						case 3:
							if (Obj.Type == OBJ_USER)
							{
								lpTargetObj = getGameObject(tObjNum);

								if ((lpTargetObj->m_RecallMon >= 0 && (lpTargetObj->Class < 678 || lpTargetObj->Class > 680)) || g_EvoMonMng.IsEvolutedEvoMon(*lpTargetObj) == true 
									|| lpTargetObj->Class == 690)
								{
									pCallMonsterViewportCreate.NumberH = SET_NUMBERH(tObjNum);
									pCallMonsterViewportCreate.NumberL = SET_NUMBERL(tObjNum);

									if (lpTargetObj->m_State == 1)
									{
										pCallMonsterViewportCreate.NumberH |= 0x80;
									}

									pCallMonsterViewportCreate.Type_HI = SET_NUMBERH(lpTargetObj->Class);
									pCallMonsterViewportCreate.Type_LO = SET_NUMBERL(lpTargetObj->Class);

									pCallMonsterViewportCreate.X = lpTargetObj->X;
									pCallMonsterViewportCreate.Y = lpTargetObj->Y;
									pCallMonsterViewportCreate.TX = lpTargetObj->TX;
									pCallMonsterViewportCreate.TY = lpTargetObj->TY;
									pCallMonsterViewportCreate.Path = lpTargetObj->Dir << 4;

									pCallMonsterViewportCreate.PentagramMainAttribute = lpTargetObj->m_iPentagramMainAttribute;
									pCallMonsterViewportCreate.LevelH = SET_NUMBERH(lpTargetObj->Level);
									pCallMonsterViewportCreate.LevelL = SET_NUMBERL(lpTargetObj->Level);
									pCallMonsterViewportCreate.MaxLifeHH = (int)(lpTargetObj->AddLife + lpTargetObj->MaxLife) >> 24;
									pCallMonsterViewportCreate.MaxLifeHL = (int)(lpTargetObj->AddLife + lpTargetObj->MaxLife) >> 8;
									pCallMonsterViewportCreate.MaxLifeLH = (int)(lpTargetObj->AddLife + lpTargetObj->MaxLife) >> 16;
									pCallMonsterViewportCreate.MaxLifeLL = (int)(lpTargetObj->AddLife + lpTargetObj->MaxLife);
									pCallMonsterViewportCreate.CurLifeHH = (int)lpTargetObj->Life >> 24;
									pCallMonsterViewportCreate.CurLifeHL = (int)lpTargetObj->Life >> 8;
									pCallMonsterViewportCreate.CurLifeLH = (int)lpTargetObj->Life >> 16;
									pCallMonsterViewportCreate.CurLifeLL = (int)lpTargetObj->Life;

									int iOwnerIndex = lpTargetObj->m_RecallMon;

									if (lpTargetObj->Class == 681 || lpTargetObj->Class == 690)
									{
										iOwnerIndex = lpTargetObj->m_nEvoMonOwnerIndex;
									}

									if (lpTargetObj->Class == 707) // Dark Wizard Phantom
									{
										pCallMonsterViewportCreate.SummonType = OBJ_USER;

										if (ObjectMaxRange(iOwnerIndex) == true && getGameObject(iOwnerIndex)->Type == OBJ_USER)
										{
											std::memcpy(pCallMonsterViewportCreate.CharSet, getGameObject(iOwnerIndex)->CharSet, 18);
										}
									}

									else
									{
										pCallMonsterViewportCreate.SummonType = OBJ_MONSTER;
										memset(pCallMonsterViewportCreate.CharSet, 0x00, CHAR_SET_SIZE);
									}

									if (ObjectMaxRange(iOwnerIndex) == true && getGameObject(iOwnerIndex)->Type == OBJ_USER)
									{
										std::memcpy(pCallMonsterViewportCreate.Id, getGameObject(iOwnerIndex)->Name, sizeof(pCallMonsterViewportCreate.Id));

									}
									else
									{
										memset(pCallMonsterViewportCreate.Id, 0x00, sizeof(pCallMonsterViewportCreate.Id));
									}

									pCallMonsterViewportCreate.BuffEffectCount = iViewportSize = gObjMakeViewportState(*lpTargetObj, pCallMonsterViewportCreate.BuffEffectList);

									std::memcpy((callMonstersendBuf + callMonlOfs), (char*)&pCallMonsterViewportCreate, sizeof(PMSG_CALLMONSTER_VIEWPORTCREATE));
									callMonlOfs += sizeof(PMSG_CALLMONSTER_VIEWPORTCREATE) - (MAX_BUFFEFFECT - iViewportSize);
									callmonstercount++;;
								}
								else
								{
									pMonsterViewportCreate.NumberH = SET_NUMBERH(tObjNum);
									pMonsterViewportCreate.NumberL = SET_NUMBERL(tObjNum);

									if (lpTargetObj->m_State == 1)
									{
										pMonsterViewportCreate.NumberH |= 0x80;

										if (lpTargetObj->Teleport != 0)
										{
											pMonsterViewportCreate.NumberH |= 0x40;
										}
									}

									pMonsterViewportCreate.Type_HI = SET_NUMBERH(lpTargetObj->Class);
									pMonsterViewportCreate.Type_LO = SET_NUMBERL(lpTargetObj->Class);

									pMonsterViewportCreate.PentagramMainAttribute = lpTargetObj->m_iPentagramMainAttribute;
									pMonsterViewportCreate.LevelH = SET_NUMBERH(lpTargetObj->Level);
									pMonsterViewportCreate.LevelL = SET_NUMBERL(lpTargetObj->Level);
									pMonsterViewportCreate.MaxLifeHH = (int)(lpTargetObj->AddLife + lpTargetObj->MaxLife) >> 24;
									pMonsterViewportCreate.MaxLifeHL = (int)(lpTargetObj->AddLife + lpTargetObj->MaxLife) >> 8;
									pMonsterViewportCreate.MaxLifeLH = (int)(lpTargetObj->AddLife + lpTargetObj->MaxLife) >> 16;
									pMonsterViewportCreate.MaxLifeLL = (int)(lpTargetObj->AddLife + lpTargetObj->MaxLife);
									pMonsterViewportCreate.CurLifeHH = (int)lpTargetObj->Life >> 24;
									pMonsterViewportCreate.CurLifeHL = (int)lpTargetObj->Life >> 8;
									pMonsterViewportCreate.CurLifeLH = (int)lpTargetObj->Life >> 16;
									pMonsterViewportCreate.CurLifeLL = (int)lpTargetObj->Life;

									if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
									{
										if (lpTargetObj->Class == 278)
										{
											if (g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
											{
												if (Obj.m_btCsJoinSide == lpTargetObj->m_btCsJoinSide)
													pMonsterViewportCreate.Type_HI |= 0x80;
											}

											if (Obj.m_PlayerData->lpGuild && lpTargetObj->m_PlayerData->lpGuild)
											{
												if (Obj.m_PlayerData->lpGuild == lpTargetObj->m_PlayerData->lpGuild)
												{
													pMonsterViewportCreate.Type_HI |= 0x80;
												}
											}

											pMonsterViewportCreate.Type_HI |= ((lpTargetObj->m_btCreationState << 4) & 0x70);
										}
									}

									pMonsterViewportCreate.X = lpTargetObj->X;
									pMonsterViewportCreate.Y = lpTargetObj->Y;
									pMonsterViewportCreate.TX = lpTargetObj->TX;
									pMonsterViewportCreate.TY = lpTargetObj->TY;
									pMonsterViewportCreate.Path = lpTargetObj->Dir << 4;

									if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
									{
										if (lpTargetObj->Class == 277)
										{
											if (lpTargetObj->m_btCsGateOpen == 0)
											{
												gObjRemoveBuffEffect(*lpTargetObj, BUFFTYPE_CASTLE_DOOR_STATE);
											}
											else
											{
												gObjAddBuffEffect(*lpTargetObj, BUFFTYPE_CASTLE_DOOR_STATE);
											}
										}

										if (lpTargetObj->Class == 216)
										{
											if (g_CastleSiege.GetRegCrownAvailable() == FALSE)
											{
												gObjRemoveBuffEffect(*lpTargetObj, 20);
											}
											else
											{
												gObjAddBuffEffect(*lpTargetObj, 20);
											}
										}
									}

									pMonsterViewportCreate.BuffEffectCount = iViewportSize = gObjMakeViewportState(*lpTargetObj, pMonsterViewportCreate.BuffEffectList);

									std::memcpy((MonstersendBuf + MonlOfs), (char*)&pMonsterViewportCreate, sizeof(PMSG_MONSTER_VIEWPORTCREATE));
									MonlOfs += sizeof(PMSG_MONSTER_VIEWPORTCREATE) - (MAX_BUFFEFFECT - iViewportSize);
									monstercount++;
								}
							}
							break;
						case 5:
							if (Obj.Type == OBJ_USER)
							{
								pItemViewportCreate.NumberH = SET_NUMBERH(tObjNum);
								pItemViewportCreate.NumberL = SET_NUMBERL(tObjNum);
								if (Obj.m_bOffLevel && g_OffLevel.m_PickItemSettings.bPickUpItems)
								{
									g_OffLevel.CheckAndPickUpItem(Obj, &MapC[Obj.MapNumber].m_CItemObject[tObjNum], tObjNum);
								}
								if (MapC[Obj.MapNumber].m_CItemObject[tObjNum].m_State == 1)
								{
									pItemViewportCreate.NumberH |= 0x80;
								}

								pItemViewportCreate.px = MapC[Obj.MapNumber].m_CItemObject[tObjNum].px;
								pItemViewportCreate.py = MapC[Obj.MapNumber].m_CItemObject[tObjNum].py;

								if (MapC[Obj.MapNumber].m_CItemObject[tObjNum].m_Type == ITEMGET(14, 15))
								{
									WORD MoneyHW = SET_NUMBERHW(MapC[Obj.MapNumber].m_CItemObject[tObjNum].m_BuyMoney);
									WORD MoneyLW = SET_NUMBERLW(MapC[Obj.MapNumber].m_CItemObject[tObjNum].m_BuyMoney);

									pItemViewportCreate.ItemInfo[0] = BYTE(MapC[Obj.MapNumber].m_CItemObject[tObjNum].m_Type) % 255;
									pItemViewportCreate.ItemInfo[1] = SET_NUMBERL(MoneyHW);
									pItemViewportCreate.ItemInfo[2] = SET_NUMBERH(MoneyLW);
									pItemViewportCreate.ItemInfo[4] = SET_NUMBERL(MoneyLW);
									pItemViewportCreate.ItemInfo[3] = 0;
									pItemViewportCreate.ItemInfo[5] = (MapC[Obj.MapNumber].m_CItemObject[tObjNum].m_Type & 0x1E00) >> 5;
									pItemViewportCreate.ItemInfo[6] = 0;
									pItemViewportCreate.ItemInfo[7] = BYTE(MapC[Obj.MapNumber].m_CItemObject[tObjNum].m_SocketOption[0]);
									pItemViewportCreate.ItemInfo[8] = BYTE(MapC[Obj.MapNumber].m_CItemObject[tObjNum].m_SocketOption[1]);
									pItemViewportCreate.ItemInfo[9] = BYTE(MapC[Obj.MapNumber].m_CItemObject[tObjNum].m_SocketOption[2]);
									pItemViewportCreate.ItemInfo[10] = BYTE(MapC[Obj.MapNumber].m_CItemObject[tObjNum].m_SocketOption[3]);
									pItemViewportCreate.ItemInfo[11] = BYTE(MapC[Obj.MapNumber].m_CItemObject[tObjNum].m_SocketOption[4]);
									std::memcpy(&ItemBuf[lOfs_Item], &pItemViewportCreate, sizeof(pItemViewportCreate));
									lOfs_Item += ItemStructSize;
								}
								else
								{
									ItemByteConvert(pItemViewportCreate.ItemInfo, (MapC[Obj.MapNumber].m_CItemObject[tObjNum]));
									std::memcpy(&ItemBuf[lOfs_Item], &pItemViewportCreate, sizeof(pItemViewportCreate));
									lOfs_Item += ItemStructSize;
								}

								count_Item += 1;
							}
							break;
						}
					}
					Obj.VpPlayer[n].state = 2;
				}
			}
		}
		if (Obj.Type == OBJ_USER)
		{
			if (count > 0)
			{
				PWMSG_COUNT pCount;

				pCount.h.c = 0xC2;
				pCount.h.headcode = 0x12;
				pCount.h.sizeH = SET_NUMBERH(lOfs);
				pCount.h.sizeL = SET_NUMBERL(lOfs);
				pCount.count = count;

				std::memcpy(sendBuf, &pCount, sizeof(pCount));
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&sendBuf, lOfs);
			}

			if (ChangeCount > 0)
			{
				PWMSG_COUNT pCount;

				pCount.h.c = 0xC2;
				pCount.h.headcode = 0x45;
				pCount.h.sizeH = SET_NUMBERH(lOfsChange);
				pCount.h.sizeL = SET_NUMBERL(lOfsChange);
				pCount.count = ChangeCount;

				std::memcpy(SendGBufChange, &pCount, sizeof(pCount));
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&SendGBufChange, lOfsChange);
			}

			if (monstercount > 0)
			{
				PWMSG_COUNT pCount;

				pCount.h.c = 0xC2;
				pCount.h.headcode = 0x13;
				pCount.h.sizeH = SET_NUMBERH(MonlOfs);
				pCount.h.sizeL = SET_NUMBERL(MonlOfs);
				pCount.count = monstercount;

				std::memcpy(MonstersendBuf, &pCount, sizeof(pCount));
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&MonstersendBuf, MonlOfs);
			}

			if (callmonstercount > 0)
			{
				PWMSG_COUNT pCount;

				pCount.h.c = 0xC2;
				pCount.h.headcode = 0x1F;
				pCount.h.sizeH = SET_NUMBERH(callMonlOfs);
				pCount.h.sizeL = SET_NUMBERL(callMonlOfs);
				pCount.count = callmonstercount;

				std::memcpy(callMonstersendBuf, &pCount, sizeof(pCount));
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&callMonstersendBuf, callMonlOfs);
			}

			if (count_Item > 0)
			{
				PWMSG_COUNT pCount;

				pCount.h.c = 0xC2;
				pCount.h.headcode = 0x20;
				pCount.count = count_Item;
				pCount.h.sizeH = SET_NUMBERH(lOfs_Item);
				pCount.h.sizeL = SET_NUMBERL(lOfs_Item);

				std::memcpy(ItemBuf, &pCount, sizeof(pCount));
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&ItemBuf, lOfs_Item);
			}

			if (GuildInfoCount != 0)
			{
				PWMSG_COUNT pCount;

				pCount.h.headcode = 0x65;
				pCount.h.c = 0xC2;
				pCount.h.sizeH = SET_NUMBERH(GuildInfoOfs);
				pCount.h.sizeL = SET_NUMBERL(GuildInfoOfs);
				pCount.count = GuildInfoCount;

				std::memcpy(GuildInfoBuf, &pCount, sizeof(pCount));
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&GuildInfoBuf, GuildInfoOfs);
			}

			if (GensInfoCount != 0)
			{
				PMSG_SEND_GENS_MEMBER_VIEWPORT pGensCount;
				pGensCount.h.c = 0xC2;
				pGensCount.h.headcode = 0xF8;
				pGensCount.h.subcode = 0x05;
				pGensCount.h.sizeH = SET_NUMBERH(GensInfoOfs);
				pGensCount.h.sizeL = SET_NUMBERL(GensInfoOfs);
				pGensCount.Count = GensInfoCount;

				std::memcpy(GensInfoBuf, &pGensCount, sizeof(pGensCount));
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&GensInfoBuf, GensInfoOfs);
			}
		}
	}
}

void gObjTeleportMagicUse(CGameObject &Obj, BYTE x, BYTE y)	// Check if x and y are BYTE
{

	

	if (MAX_MAP_RANGE(Obj.MapNumber) == FALSE)
	{
		return;
	}

	if (Obj.Teleport != 0)
	{
		return;
	}

	Obj.TeleportTime = GetTickCount();
	Obj.PathCount = 0;
	Obj.Teleport = 1;
	Obj.m_ViewState = 1;

	Obj.X = x;
	Obj.Y = y;
	Obj.TX = x;
	Obj.TY = y;

	MapC[Obj.MapNumber].ClearStandAttr(Obj.m_OldX, Obj.m_OldY);
	MapC[Obj.MapNumber].SetStandAttr(Obj.TX, Obj.TY);

	Obj.m_OldX = Obj.TX;
	Obj.m_OldY = Obj.TY;

	gObjViewportListProtocolDestroy(Obj);
}




BOOL gObjMoveGate(CGameObject &Obj, int gt)
{
	int result;
	short x, y;
	BYTE mapNumber = 0, dir;
	short level;
	int userlevel;

	if (g_ConfigRead.UseMLforWarp == 1)
	{
		userlevel = Obj.Level + Obj.m_PlayerData->MasterLevel;
	}
	else
	{
		userlevel = Obj.Level;
	}

	int movefail = 0;

	if (Obj.m_SkillInfo->LordSummonTime != 0)
	{
		char szMsg[255];
		Obj.m_SkillInfo->LordSummonTime = 0;
		wsprintf(szMsg, Lang.GetText(0, 146));
		gGameProtocol.GCServerMsgStringSend(szMsg, Obj, 1);
	}

	if (Obj.Type == OBJ_USER && Obj.m_PlayerData->ISBOT == false && Obj.m_Index >= g_ConfigRead.server.GetObjectStartUserIndex())
	{
		gDarkSpirit[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].SetMode(CDarkSpirit::ePetItem_Mode::PetItem_Mode_Normal, -1);
	}

	if (BC_MAP_RANGE(Obj.MapNumber))
	{
		int iBridgeIndex = g_BloodCastle.GetBridgeIndex(Obj.MapNumber);

		if (g_BloodCastle.GetCurrentState(iBridgeIndex) == BC_STATE_PLAYING)
		{
			g_BloodCastle.SearchUserDropQuestItem(Obj);
		}
		else
		{
			g_BloodCastle.SearchUserDeleteQuestItem(Obj);
		}
	}

	if (Obj.Type == OBJ_USER && Obj.m_PlayerData->m_bIsMining)
	{
		g_MineSystem.CheckMoveMapWhileMining(Obj);
	}

	if (Obj.RegenOk != 0)
	{
		sLog->outBasic("Regen OK %s %d", __FILE__, __LINE__);
		x = Obj.X;
		y = Obj.Y;
		mapNumber = Obj.MapNumber;
		dir = Obj.Dir;
		movefail = 1;

		Obj.RegenMapNumber = Obj.MapNumber;
		Obj.RegenMapX = x;
		Obj.RegenMapY = y;

		gObjClearViewport(Obj);

		gGameProtocol.GCTeleportSend(Obj, gt, mapNumber, Obj.X, Obj.Y, Obj.Dir);

		if (Obj.m_Change >= 0)
		{
			gObjViewportListProtocolCreate(Obj);
		}

		Obj.RegenOk = 1;

		return false;
	}

	if (gGateC.IsGate(gt) == false)
	{

		sLog->outBasic("Cannnot find gate number %d (%s %d)", gt, __FILE__, __LINE__);

		x = Obj.X;
		y = Obj.Y;
		mapNumber = Obj.MapNumber;
		dir = Obj.Dir;
		movefail = 1;

		Obj.RegenMapNumber = Obj.MapNumber;
		Obj.RegenMapX = x;
		Obj.RegenMapY = y;

		gObjClearViewport(Obj);

		gGameProtocol.GCTeleportSend(Obj, gt, mapNumber, Obj.X, Obj.Y, Obj.Dir);

		if (Obj.m_Change >= 0)
		{
			gObjViewportListProtocolCreate(Obj);
		}

		Obj.RegenOk = 1;

		return false;
	}

	if (gt >= 58 && gt <= 61)
	{
		if (Obj.m_bDevilSquareAuth == 0)
		{
			sLog->outBasic("[DevilSquare] [%s][%s] Not Used DevilSquare Invitation In DevilSquare", Obj.AccountID, Obj.Name);
			return false;
		}
		else
		{
			Obj.m_bDevilSquareAuth = 0;
		}
	}

	result = gGateC.GetGate(gt, (short &)x, (short &)y, (BYTE &)mapNumber, (BYTE &)dir, (short &)level);

	if (result < 0)
	{
		sLog->outBasic("error : %d (%s %d)", result, __FILE__, __LINE__);

		x = Obj.X;
		y = Obj.Y;
		mapNumber = Obj.MapNumber;
		dir = Obj.Dir;

		movefail = 1;
	}

	if (mapNumber == MAP_INDEX_ACHERON || mapNumber == MAP_INDEX_DEBENTER || mapNumber == MAP_INDEX_NARS)
	{
		BOOL bIsABOccupyZone = g_ArcaBattle.IsArcaBattleOccupyZone(Obj, gt);

		if (bIsABOccupyZone == TRUE)
		{
			if (gObjCheckUsedBuffEffect(Obj, 179) || gObjCheckUsedBuffEffect(Obj, 180) || gObjCheckUsedBuffEffect(Obj, 181) || gObjCheckUsedBuffEffect(Obj, 182) || gObjCheckUsedBuffEffect(Obj, 183))
			{
				gObjAddBuffEffect(Obj, 187, 0, 0, 0, 0, -10);
			}

			else
			{
				if (gObjCheckUsedBuffEffect(Obj, 187))
				{
					gObjRemoveBuffEffect(Obj, 187);
				}
			}
		}

		g_ArcaBattle.SendArcaBattleOccupyZone(Obj, gt);
	}

	else
	{
		if (gObjCheckUsedBuffEffect(Obj, 187))
		{
			gObjRemoveBuffEffect(Obj, 187);
		}
	}

	if (mapNumber == MAP_INDEX_URUK_MOUNTAIN)
	{
		SendLordSilvesterBlockInfo(Obj, g_bSilvesterEntranceBlock == true ? 0 : 1);
	}

	if (g_GensSystem.IsMapBattleZone(Obj.MapNumber) == TRUE && Obj.Type == OBJ_USER && GetTickCount() - Obj.m_PlayerData->m_dwGensHitTeleportDelay < (g_GensSystem.GetGensAttackWarpDelay() * 1000))
	{
		x = Obj.X;
		y = Obj.Y;
		mapNumber = Obj.MapNumber;
		dir = Obj.Dir;

		movefail = 1;

		Obj.RegenMapNumber = Obj.MapNumber;
		Obj.RegenMapX = x;
		Obj.RegenMapY = y;

		gObjClearViewport(Obj);
		gGameProtocol.GCTeleportSend(Obj, gt, mapNumber, Obj.X, Obj.Y, Obj.Dir);

		if (Obj.m_Change >= 0)
		{
			gObjViewportListProtocolCreate(Obj);
		}

		Obj.RegenOk = 1;

		return FALSE;
	}

	else
	{
		if (Obj.Type == OBJ_USER)
		{
			Obj.m_PlayerData->m_dwGensHitTeleportDelay = 0;
		}
	}

	if (g_GensSystem.IsMapBattleZone(mapNumber) && (g_GensSystem.IsUserBattleZoneEnable(Obj) == FALSE || g_GensSystem.GetEntryAllowType(Obj, mapNumber) == false))
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 414), Obj, 1);

		x = Obj.X;
		y = Obj.Y;
		mapNumber = Obj.MapNumber;
		dir = Obj.Dir;

		movefail = 1;

		Obj.RegenMapNumber = Obj.MapNumber;
		Obj.RegenMapX = x;
		Obj.RegenMapY = y;

		gObjClearViewport(Obj);
		gGameProtocol.GCTeleportSend(Obj, gt, mapNumber, Obj.X, Obj.Y, Obj.Dir);

		if (Obj.m_Change >= 0)
		{
			gObjViewportListProtocolCreate(Obj);
		}

		Obj.RegenOk = 1;

		return FALSE;
	}

	if (Obj.m_bPShopOpen == 1)
	{
		int bMove = 1;

		if (Obj.MapNumber == MAP_INDEX_CASTLESIEGE && mapNumber == MAP_INDEX_RORENCIA)
		{
			bMove = 0;
		}

		if (Obj.MapNumber == MAP_INDEX_RORENCIA && mapNumber == MAP_INDEX_CASTLESIEGE)
		{
			bMove = 0;
		}

		if (bMove == 0)
		{
			sLog->outBasic("[PSHOP ITEM CLONNING] Move Failed : %d (%s %s)", result, Obj.AccountID, Obj.Name);

			x = Obj.X;
			y = Obj.Y;
			mapNumber = Obj.MapNumber;
			dir = Obj.Dir;
			movefail = 1;

			Obj.RegenMapX = x;
			Obj.RegenMapY = y;

			Obj.RegenMapNumber = mapNumber;
			Obj.RegenMapNumber = Obj.MapNumber;

			Obj.RegenMapX = x;
			Obj.RegenMapY = y;

			gObjClearViewport(Obj);

			gGameProtocol.GCTeleportSend(Obj, gt, mapNumber, Obj.X, Obj.Y, Obj.Dir);

			if (Obj.m_Change >= 0)
			{
				gObjViewportListProtocolCreate(Obj);
			}

			Obj.RegenOk = 1;
			return false;
		}
	}

	if (BC_MAP_RANGE(mapNumber)
		|| CC_MAP_RANGE(mapNumber)
		|| DS_MAP_RANGE(mapNumber)
		|| IT_MAP_RANGE(mapNumber)
		|| DG_MAP_RANGE(mapNumber)
		|| IMPERIAL_MAP_RANGE(mapNumber)
		|| mapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL
		|| mapNumber == MAP_INDEX_DEVILSQUARE_FINAL)
	{
		if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_ATTDEF_POWER_INC) != NULL)
		{
			gObjRemoveBuffEffect(Obj, BUFFTYPE_ATTDEF_POWER_INC);
		}

		if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_ATTACK_POWER_INC) != NULL)
		{
			gObjRemoveBuffEffect(Obj, BUFFTYPE_ATTACK_POWER_INC);
		}

		if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_DEFENSE_POWER_INC) != NULL)
		{
			gObjRemoveBuffEffect(Obj, BUFFTYPE_DEFENSE_POWER_INC);
		}

		if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_CRITICAL_DMG_INC) != NULL)
		{
			gObjRemoveBuffEffect(Obj, BUFFTYPE_CRITICAL_DMG_INC);
		}

		if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_HP_INC) != NULL)
		{
			gObjRemoveBuffEffect(Obj, BUFFTYPE_HP_INC);
		}

		if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_MELEE_DEFENSE_INC) != NULL)
		{
			gObjRemoveBuffEffect(Obj, BUFFTYPE_MELEE_DEFENSE_INC);
		}

		if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_HP_INC_STR) != NULL)
		{
			gObjRemoveBuffEffect(Obj, BUFFTYPE_HP_INC_STR);
		}

		if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_HP_INC_MAS) != NULL)
		{
			gObjRemoveBuffEffect(Obj, BUFFTYPE_HP_INC_MAS);
		}

		if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_MELEE_DEFENSE_INC_STR) != NULL)
		{
			gObjRemoveBuffEffect(Obj, BUFFTYPE_MELEE_DEFENSE_INC_STR);
		}
	}

	if (IT_MAP_RANGE(Obj.MapNumber))
	{
		if (g_ConfigRead.server.GetServerType() == SERVER_NORMAL && mapNumber == MAP_INDEX_LORENMARKET)
		{
			g_IT_Event.DropRelicsItem(Obj.MapNumber, Obj);
			g_IT_Event.Leave_ITR(Obj, Obj.MapNumber);

			if (Obj.m_nITR_RelicsTick > 0 || Obj.m_wITR_NpcType > 0 || Obj.m_byITR_StoneState != 99 || Obj.m_bITR_RegisteringRelics == true || Obj.m_bITR_GettingRelics == true)
			{
				g_IT_Event.CancleStoneState(Obj.m_wITR_NpcType, Obj.m_byITR_StoneState, Obj.MapNumber);
				Obj.m_nITR_RelicsTick = 0;
				Obj.m_wITR_NpcType = 0;
				Obj.m_byITR_StoneState = 99;
				Obj.m_bITR_RegisteringRelics = 0;
				Obj.m_bITR_GettingRelics = 0;
			}
		}
	}

	if (Obj.MapNumber == MAP_INDEX_DEVILSQUARE_FINAL && g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE && mapNumber != MAP_INDEX_DEVILSQUARE_FINAL)
	{
		g_DevilSquareFinal.Leave_DSF(Obj);
	}

	if (DG_MAP_RANGE(Obj.MapNumber) && mapNumber != Obj.MapNumber)
	{
		g_DoppelGanger.LeaveDoppelganger(Obj);

		if (g_DoppelGanger.GetDoppelgangerState() == 2)
		{
			g_DoppelGanger.SendDoppelgangerResult(Obj, 1);
		}

		if (Obj.PartyNumber >= 0)
		{
			int nPartyNumber = Obj.PartyNumber;

			if (gParty.GetCount(nPartyNumber) <= 1)
			{
				for (int i = 0; i < 5; i++)
				{
					int index = gParty.m_PartyS[nPartyNumber].Number[i];

					if (index >= 0)
					{
						gParty.Delete(nPartyNumber, Obj.m_Index);

						getGameObject(index)->PartyNumber = -1;
						getGameObject(index)->PartyTargetUser = -1;

						if (getGameObject(index)->Connected == PLAYER_PLAYING)
							gGameProtocol.GCPartyDelUserSend(*getGameObject(index));
					}
				}
			}

			else
			{
				gGameProtocol.CGPartyListAll(nPartyNumber);
			}
		}
	}

	if (IMPERIAL_MAP_RANGE(Obj.MapNumber) && mapNumber != Obj.MapNumber && Obj.PartyNumber >= 0)
	{
		int party_number = Obj.PartyNumber;

		if (gParty.GetCount(party_number) <= 1)
		{
			for (int index = 0; index < MAX_USER_IN_PARTY; index++)
			{
				int userindex = gParty.m_PartyS[party_number].Number[index];

				if (userindex >= 0)
				{
					gParty.Delete(party_number, Obj.m_Index);
					getGameObject(userindex)->PartyNumber = -1;
					getGameObject(userindex)->PartyTargetUser = -1;

					if (getGameObject(userindex)->Connected == PLAYER_PLAYING)
					{
						gGameProtocol.GCPartyDelUserSend(*getGameObject(userindex));
					}
				}
			}

			gParty.Destroy(party_number);
		}

		else
		{
			gGameProtocol.CGPartyListAll(party_number);
		}
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (result == 97)
		{
			if (g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
			{
				if (Obj.m_btCsJoinSide != 1 && g_CastleSiege.GetCastleTowerAccessable() == FALSE)
				{
					gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 167), Obj, 1);

					x = Obj.X;
					y = Obj.Y;
					mapNumber = Obj.MapNumber;
					dir = Obj.Dir;
					movefail = 1;

					Obj.RegenMapNumber = Obj.MapNumber;
					Obj.RegenMapX = x;
					Obj.RegenMapY = y;

					gObjClearViewport(Obj);

					gGameProtocol.GCTeleportSend(Obj, gt, mapNumber, Obj.X, Obj.Y, Obj.Dir);

					if (Obj.m_Change >= 0)
					{
						gObjViewportListProtocolCreate(Obj);
					}

					Obj.RegenOk = 1;
					return false;
				}

				if (Obj.m_btCsJoinSide == 0)
				{
					gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 168), Obj, 1);

					x = Obj.X;
					y = Obj.Y;
					mapNumber = Obj.MapNumber;
					dir = Obj.Dir;
					movefail = 1;

					Obj.RegenMapNumber = Obj.MapNumber;
					Obj.RegenMapX = x;
					Obj.RegenMapY = y;

					gObjClearViewport(Obj);

					gGameProtocol.GCTeleportSend(Obj, gt, mapNumber, Obj.X, Obj.Y, Obj.Dir);

					if (Obj.m_Change >= 0)
					{
						gObjViewportListProtocolCreate(Obj);
					}

					Obj.RegenOk = 1;
					return false;
				}
			}
			else if (g_CastleSiege.CheckCastleOwnerUnionMember(Obj) == FALSE && g_CastleSiege.m_TowerEntryWithoutAlliance == false)
			{
				gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 169), Obj, 1);

				x = Obj.X;
				y = Obj.Y;
				mapNumber = Obj.MapNumber;
				dir = Obj.Dir;
				movefail = 1;

				Obj.RegenMapNumber = Obj.MapNumber;
				Obj.RegenMapX = x;
				Obj.RegenMapY = y;

				gObjClearViewport(Obj);

				gGameProtocol.GCTeleportSend(Obj, gt, mapNumber, Obj.X, Obj.Y, Obj.Dir);

				if (Obj.m_Change >= 0)
				{
					gObjViewportListProtocolCreate(Obj);
				}

				Obj.RegenOk = 1;
				return false;
			}
		}
	}

	if (mapNumber == MAP_INDEX_ATHLANSE)
	{
		if (Obj.m_btInvenPetPos != 0 && Obj.m_wInvenPet != (WORD)-1)
		{
			if (Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 2))
			{
				gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 3), Obj, 1);

				x = Obj.X;
				y = Obj.Y;
				mapNumber = Obj.MapNumber;
				dir = Obj.Dir;
				movefail = 1;

				Obj.RegenMapNumber = Obj.MapNumber;
				Obj.RegenMapX = x;
				Obj.RegenMapY = y;
				gObjClearViewport(Obj);
				gGameProtocol.GCTeleportSend(Obj, gt, mapNumber, Obj.X, Obj.Y, Obj.Dir);
				if (Obj.m_Change >= 0)
				{
					gObjViewportListProtocolCreate(Obj);
				}
				Obj.RegenOk = 1;
				return false;
			}

			if (Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 3))
			{
				gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 153), Obj, 1);

				x = Obj.X;
				y = Obj.Y;
				mapNumber = Obj.MapNumber;
				dir = Obj.Dir;
				movefail = 1;

				Obj.RegenMapNumber = Obj.MapNumber;
				Obj.RegenMapX = x;
				Obj.RegenMapY = y;
				gObjClearViewport(Obj);
				gGameProtocol.GCTeleportSend(Obj, gt, mapNumber, Obj.X, Obj.Y, Obj.Dir);
				if (Obj.m_Change >= 0)
				{
					gObjViewportListProtocolCreate(Obj);
				}
				Obj.RegenOk = 1;
				return false;
			}
		}
	}

	if (mapNumber == MAP_INDEX_ICARUS)
	{
		if (!(Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 3)
			|| Obj.pntInventory[7]->m_Type == ITEMGET(13, 30)
			|| Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 37)
			|| Obj.pntInventory[7]->m_Type == ITEMGET(12, 49)
			|| Obj.pntInventory[7]->m_Type == ITEMGET(12, 50)
			|| (Obj.pntInventory[7]->m_Type >= ITEMGET(12, 0) && Obj.pntInventory[7]->m_Type <= ITEMGET(12, 6))
			|| (Obj.pntInventory[7]->m_Type >= ITEMGET(12, 36) && Obj.pntInventory[7]->m_Type <= ITEMGET(12, 43))
			|| (Obj.pntInventory[7]->m_Type >= ITEMGET(12, 130) && Obj.pntInventory[7]->m_Type <= ITEMGET(12, 135))
			|| (Obj.pntInventory[7]->m_Type >= ITEMGET(12, 262) && Obj.pntInventory[7]->m_Type <= ITEMGET(12, 265))
			|| Obj.pntInventory[7]->m_Type == ITEMGET(12, 266)
			|| Obj.pntInventory[7]->m_Type == ITEMGET(12, 267)
			|| Obj.pntInventory[7]->m_Type == ITEMGET(12, 268)
			|| Obj.pntInventory[7]->m_Type == ITEMGET(12, 269)
			|| Obj.pntInventory[7]->m_Type == ITEMGET(12, 270)
			|| Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 2)
			|| Obj.pntInventory[11]->m_Type == ITEMGET(13, 10)
			|| Obj.pntInventory[10]->m_Type == ITEMGET(13, 10)
			|| Obj.pntInventory[10]->m_Type == ITEMGET(13, 39)
			|| Obj.pntInventory[11]->m_Type == ITEMGET(13, 39)
			|| Obj.pntInventory[10]->m_Type == ITEMGET(13, 40)
			|| Obj.pntInventory[11]->m_Type == ITEMGET(13, 40)))
		{
			gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 153), Obj, 1);

			x = Obj.X;
			y = Obj.Y;
			mapNumber = Obj.MapNumber;
			dir = Obj.Dir;
			movefail = 1;
			Obj.RegenMapNumber = Obj.MapNumber;
			Obj.RegenMapX = x;
			Obj.RegenMapY = y;

			gGameProtocol.GCTeleportSend(Obj, gt, mapNumber, Obj.X, Obj.Y, Obj.Dir);
			gObjClearViewportOfMine(Obj);

			if (Obj.m_Change >= 0)
			{
				gObjViewportListProtocolCreate(Obj);
			}
			Obj.RegenOk = 1;
			return false;
		}

		if (Obj.m_RecallMon >= 0)
		{
			gGameProtocol.GCRecallMonLife(Obj, 60, 0);
			gObjMonsterCallKill(Obj);
		}
	}

	if (mapNumber == MAP_INDEX_HATCHERY) //season4 add-on
	{
		if (g_Raklion.GetRaklionState() >= RAKLION_STATE_CLOSE_DOOR)
		{
			gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 297), Obj, 1);
			x = Obj.X;
			y = Obj.Y;
			mapNumber = Obj.MapNumber;
			dir = Obj.Dir;
			movefail = 1;

			Obj.RegenMapNumber = Obj.MapNumber;
			Obj.RegenMapX = x;
			Obj.RegenMapY = y;

			gGameProtocol.GCTeleportSend(Obj, gt, mapNumber, Obj.X, Obj.Y, Obj.Dir);
			gObjClearViewportOfMine(Obj);

			if (Obj.m_Change >= 0)
			{
				gObjViewportListProtocolCreate(Obj);
			}
			Obj.RegenOk = 1;
			return false;
		}

		if (gt == 290)
		{
			g_RaklionBattleUserMng.AddUserData(Obj);
		}
	}

	if (mapNumber == MAP_INDEX_RAKLION) //season4.0 add-on
	{
		if (gt == 292)
		{
			if (g_Raklion.GetRaklionState() >= RAKLION_STATE_CLOSE_DOOR)
			{
				gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 298), Obj, 1);
				x = Obj.X;
				y = Obj.Y;
				mapNumber = Obj.MapNumber;
				dir = Obj.Dir;
				movefail = 1;

				Obj.RegenMapNumber = Obj.MapNumber;
				Obj.RegenMapX = x;
				Obj.RegenMapY = y;

				gGameProtocol.GCTeleportSend(Obj, gt, mapNumber, Obj.X, Obj.Y, Obj.Dir);
				gObjClearViewportOfMine(Obj);

				if (Obj.m_Change >= 0)
				{
					gObjViewportListProtocolCreate(Obj);
				}
				Obj.RegenOk = 1;
				return false;
			}
			g_RaklionBattleUserMng.DeleteUserData(Obj);
		}
		else
		{
			g_RaklionUtil.NotifyRaklionCurrentState(Obj, g_Raklion.GetRaklionState(), g_Raklion.GetRaklionStateDetail());
		}
	}

	if (!KALIMA_MAP_RANGE(mapNumber) && KALIMA_MAP_RANGE(Obj.MapNumber))
	{
		int iKalimaGateIndex = Obj.m_iKalimaGateIndex;

		if (g_KalimaGate.DeleteKalimaGate(Obj))
		{
			sLog->outBasic("[Kalima] [%s][%s] Kalima Gate Vanished - User MoveGate (SummonIndex:%d, EnterCount:%d)", Obj.AccountID, Obj.Name, iKalimaGateIndex, getGameObject(iKalimaGateIndex)->m_cKalimaGateEnterCount);
		}
	}

	if (Obj.Type == OBJ_USER && g_MapAttr.CheckVIP(mapNumber) > Obj.m_PlayerData->VipType)
	{
		MsgOutput(Obj, Lang.GetText(0, 415));
		x = Obj.X;
		y = Obj.Y;
		mapNumber = Obj.MapNumber;
		dir = Obj.Dir;
		movefail = 1;

		Obj.RegenMapNumber = Obj.MapNumber;
		Obj.RegenMapX = x;
		Obj.RegenMapY = y;

		gGameProtocol.GCTeleportSend(Obj, gt, mapNumber, Obj.X, Obj.Y, Obj.Dir);
		gObjClearViewportOfMine(Obj);

		if (Obj.m_Change >= 0)
		{
			gObjViewportListProtocolCreate(Obj);
		}

		Obj.RegenOk = 1;
		return false;
	}

	if (Obj.Type == OBJ_USER && Obj.m_PlayerData->m_bUsePartyMatching == true)
	{
		if (mapNumber == MAP_INDEX_ARCA_WAR ||
			mapNumber == MAP_INDEX_DEBENTER_ARCA_WAR ||
			mapNumber == MAP_INDEX_VULCAN ||
			mapNumber == MAP_INDEX_CHAOSCASTLE1 ||
			mapNumber == MAP_INDEX_CHAOSCASTLE2 ||
			mapNumber == MAP_INDEX_CHAOSCASTLE3 ||
			mapNumber == MAP_INDEX_CHAOSCASTLE4 ||
			mapNumber == MAP_INDEX_CHAOSCASTLE5 ||
			mapNumber == MAP_INDEX_CHAOSCASTLE6 ||
			mapNumber == MAP_INDEX_CHAOSCASTLE7)
		{
			x = Obj.X;
			y = Obj.Y;
			mapNumber = Obj.MapNumber;
			dir = Obj.Dir;
			movefail = 1;

			Obj.RegenMapNumber = Obj.MapNumber;
			Obj.RegenMapX = x;
			Obj.RegenMapY = y;

			gObjClearViewport(Obj);
			gGameProtocol.GCTeleportSend(Obj, gt, mapNumber, Obj.X, Obj.Y, Obj.Dir);

			PMSG_ANS_PARTYMATCHING_ERROR pMsg;
			PHeadSubSetB((BYTE*)&pMsg, 0xEF, 0x09, sizeof(pMsg));

			pMsg.iResult = -1;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
			return FALSE;
		}
	}

	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		if (mapNumber == MAP_INDEX_KANTURU_BOSS)
		{
			if (!(Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 3)
				|| Obj.pntInventory[7]->m_Type == ITEMGET(13, 30)
				|| Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 37)
				|| Obj.pntInventory[7]->m_Type == ITEMGET(12, 49)
				|| Obj.pntInventory[7]->m_Type == ITEMGET(12, 50)
				|| (Obj.pntInventory[7]->m_Type >= ITEMGET(12, 0) && Obj.pntInventory[7]->m_Type <= ITEMGET(12, 6))
				|| (Obj.pntInventory[7]->m_Type >= ITEMGET(12, 262) && Obj.pntInventory[7]->m_Type <= ITEMGET(12, 265))
				|| Obj.pntInventory[7]->m_Type == ITEMGET(12, 266)
				|| Obj.pntInventory[7]->m_Type == ITEMGET(12, 267)
				|| Obj.pntInventory[7]->m_Type == ITEMGET(12, 268)
				|| Obj.pntInventory[7]->m_Type == ITEMGET(12, 269)
				|| Obj.pntInventory[7]->m_Type == ITEMGET(12, 270)
				|| (Obj.pntInventory[7]->m_Type >= ITEMGET(12, 36) && Obj.pntInventory[7]->m_Type <= ITEMGET(12, 43)))
				|| Obj.pntInventory[11]->m_Type == ITEMGET(13, 10)
				|| Obj.pntInventory[10]->m_Type == ITEMGET(13, 10)
				|| Obj.pntInventory[11]->m_Type == ITEMGET(13, 39)
				|| Obj.pntInventory[10]->m_Type == ITEMGET(13, 39)
				|| Obj.pntInventory[11]->m_Type == ITEMGET(13, 40)
				|| Obj.pntInventory[10]->m_Type == ITEMGET(13, 40))
			{
				gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 153), Obj, 1);

				x = Obj.X;
				y = Obj.Y;
				mapNumber = Obj.MapNumber;
				dir = Obj.Dir;
				movefail = 1;

				Obj.RegenMapNumber = Obj.MapNumber;
				Obj.RegenMapX = x;
				Obj.RegenMapY = y;

				gGameProtocol.GCTeleportSend(Obj, gt, mapNumber, Obj.X, Obj.Y, Obj.Dir);
				gObjClearViewportOfMine(Obj);

				if (Obj.m_Change >= 0)
				{
					gObjViewportListProtocolCreate(Obj);
				}

				Obj.RegenOk = 1;
				return false;
			}
		}
	}

	x = Obj.X;
	y = Obj.Y;
	mapNumber = Obj.MapNumber;
	dir = Obj.Dir;
	level = Obj.Level + Obj.m_PlayerData->MasterLevel;

	if (gGateC.GetGate(gt, (short &)x, (short &)y, (BYTE &)mapNumber, (BYTE &)dir, (short &)level) != -1)
	{
		if (g_MapAttr.CheckBlockEntry(mapNumber) == true)
		{
			gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 153), Obj, 1);
			x = Obj.X;
			y = Obj.Y;
			mapNumber = Obj.MapNumber;
			dir = Obj.Dir;
			movefail = 1;

			Obj.RegenMapNumber = Obj.MapNumber;
			Obj.RegenMapX = x;
			Obj.RegenMapY = y;

			gGameProtocol.GCTeleportSend(Obj, gt, mapNumber, Obj.X, Obj.Y, Obj.Dir);
			gObjClearViewportOfMine(Obj);

			if (Obj.m_Change >= 0)
			{
				gObjViewportListProtocolCreate(Obj);
			}

			Obj.RegenOk = 1;
			return false;
		}
	}

	if (Obj.Class == 4 || Obj.Class == 3 || Obj.Class == 6 || Obj.Class == 7)
	{
		if (level > 0)
		{
			level = (level / 3) * 2;
		}
	}

	if (level != 0)
	{
		if (userlevel < level)
		{
			char szTemp[256];
			wsprintf(szTemp, Lang.GetText(0, 30), level);
			gGameProtocol.GCServerMsgStringSend(szTemp, Obj, 1);

			x = Obj.X;
			y = Obj.Y;
			mapNumber = Obj.MapNumber;
			dir = Obj.Dir;
			movefail = 1;
		}
	}

	if (Obj.m_IfState->use != 0)
	{
		x = Obj.X;
		y = Obj.Y;
		mapNumber = Obj.MapNumber;
		dir = Obj.Dir;
		movefail = 1;
	}

	if (Obj.DieRegen != 0)
	{
		x = Obj.X;
		y = Obj.Y;
		mapNumber = Obj.MapNumber;
		dir = Obj.Dir;
		movefail = 1;
	}

	if (gGateC.GetGate(gt, (short &)x, (short &)y, (BYTE &)mapNumber, (BYTE &)dir, (short &)level) != -1)
	{
		if (Obj.m_Change == 404 || Obj.m_Change == 405) // iluzje z IT
		{
			if (!IT_MAP_RANGE(mapNumber))
			{
				Obj.m_Change = -1;
				gObjClearViewport(Obj);
				gObjViewportListProtocolCreate(Obj);
			}
		}
	}

	if (movefail == 0)
	{
		if (gGateC.GetGate(gt, (short &)x, (short &)y, (BYTE &)mapNumber, (BYTE &)dir, (short &)level) != -1)
		{
			if (Obj.MapNumber != mapNumber)
			{
				if (MapNumberCheck(mapNumber) == 0)
				{
					sLog->outBasic("error-L3 : %s %d", __FILE__, __LINE__);
					return false;
				}
				short sSvrCode = g_MapServerManager.CheckMoveMapSvr(Obj, mapNumber, Obj.m_sPrevMapSvrCode);
				if (sSvrCode != g_ConfigRead.server.GetGameServerCode())
				{
					if (sSvrCode == -1)
					{
						sLog->outError("[MapServerMng] Map Server Move Fail : CheckMoveMapSvr() == -1 [%s][%s] (%d)", Obj.AccountID, Obj.Name, Obj.m_Index);
						return false;
					}

					if (gGateC.CheckGateLevel(Obj, gt) == 0)
					{
						x = Obj.X;
						y = Obj.Y;
						mapNumber = Obj.MapNumber;
						dir = Obj.Dir;
						movefail = 1;

						Obj.RegenMapNumber = Obj.MapNumber;

						Obj.RegenMapX = x;
						Obj.RegenMapY = y;

						gObjClearViewport(Obj);

						gGameProtocol.GCTeleportSend(Obj, gt, mapNumber, Obj.X, Obj.Y, Obj.Dir);

						if (Obj.m_Change >= 0)
						{
							gObjViewportListProtocolCreate(Obj);
						}

						Obj.RegenOk = 1;

						if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
						{
							if (Obj.MapNumber == MAP_INDEX_CASTLESIEGE)
							{
								if (g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
								{
									g_CastleSiege.NotifySelfCsJoinSide(Obj);
									g_CastleSiege.NotifyCsSelfLeftTime(Obj);
								}
							}
						}

						return false;
					}

					GJReqMapSvrMove(Obj, sSvrCode, mapNumber, x, y);
					sLog->outBasic("[MapServerMng] Request to Move Map Server : (%d) - [%s][%s] (%d)", sSvrCode, Obj.AccountID, Obj.Name, Obj.m_Index);
					return false;
				}
			}
		}

		if ((mapNumber == MAP_INDEX_ARCA_WAR || mapNumber == MAP_INDEX_DEBENTER_ARCA_WAR) && g_ArcaBattle.IsArcaBattleServer() == TRUE && g_AcheronGuardianEvent.GetProcState() < 3)
		{
			if (g_ArcaBattle.GetState() < 7)
			{
				x = Obj.X;
				y = Obj.Y;
				mapNumber = Obj.MapNumber;
				dir = Obj.Dir;
				movefail = 1;

				Obj.RegenMapNumber = Obj.MapNumber;

				Obj.RegenMapX = x;
				Obj.RegenMapY = y;

				gObjClearViewportOfMine(Obj);

				gGameProtocol.GCTeleportSend(Obj, gt, mapNumber, Obj.X, Obj.Y, Obj.Dir);

				if (Obj.m_Change >= 0)
				{
					gObjViewportListProtocolCreate(Obj);
				}

				Obj.RegenOk = 1;

				return 0;
			}

			if (g_ArcaBattle.GetGuildMasterPos(Obj, gt, x, y) == FALSE)
			{
				g_ArcaBattle.GetUserPos(gt, x, y);
			}

			g_ArcaBattle.SetUserMapInfo(Obj, gt);
		}

		if (Obj.Type == OBJ_USER)
		{
			Obj.m_PlayerData->MoveGate = gt;
		}

		Obj.m_State = 32;

		WORD nCurrentMap = Obj.MapNumber;

		Obj.X = x;
		Obj.Y = y;
		Obj.TX = x;
		Obj.TY = y;
		Obj.MapNumber = mapNumber;
		Obj.Dir = dir;
		Obj.PathCount = 0;
		Obj.Teleport = 0;

		if (Obj.m_PlayerData->m_bUsePartyMatching)
		{
			if (DG_MAP_RANGE(Obj.MapNumber) || IMPERIAL_MAP_RANGE(Obj.MapNumber))
			{
				if (Obj.m_PlayerData->m_bPartyMatchingLeader)
				{
					GDReqCancelPartyMatching(Obj, 0);
				}
				else
				{
					PMSG_ANS_PARTYMATCHING_ERROR pMsg;

					PHeadSubSetB((BYTE*)&pMsg, 0xEFu, 0x09, sizeof(pMsg));
					pMsg.iResult = -2;
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
				}
			}
		}

		gObjViewportListProtocolDestroy(Obj);
		gGameProtocol.GCTeleportSend(Obj, gt, mapNumber, Obj.X, Obj.Y, Obj.Dir);
		gObjViewportListProtocolCreate(Obj);
		gObjClearViewport(Obj);

		if (Obj.m_Change >= 0)
			gObjViewportListProtocolCreate(Obj);

		Obj.RegenMapNumber = mapNumber;
		Obj.RegenMapX = x;
		Obj.RegenMapY = y;
		Obj.RegenOk = 1;

		if (nCurrentMap != mapNumber)
		{
			g_SendNPCInfo.SendNpcCoordinate(Obj.m_Index, Obj.MapNumber, 0);
			g_SendNPCInfo.SendPortalCoordinate(Obj.m_Index, Obj.MapNumber);
		}

		if (g_DevilSquareFinal.m_bEVENT_ENABLE == true && Obj.MapNumber == MAP_INDEX_DEVILSQUARE_FINAL &&
			g_DevilSquareFinal.m_nDSF_TYPE != 0 && g_DevilSquareFinal.m_bWinnerResultMsgSend == true)
		{
			g_DevilSquareFinal.SendDSFResult(Obj);
		}
	}
	else
	{
		gGameProtocol.GCTeleportSend(Obj, gt, mapNumber, Obj.X, Obj.Y, Obj.Dir);
		Obj.RegenOk = 1;

		if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
		{
			if (Obj.MapNumber == MAP_INDEX_CASTLESIEGE)
			{
				if (g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					g_CastleSiege.NotifySelfCsJoinSide(Obj);
					g_CastleSiege.NotifyCsSelfLeftTime(Obj);
				}
			}
		}
	}

	if (movefail != 0)
	{
		return false;
	}

	if (Obj.Type == OBJ_USER)
	{
		Obj.m_LastTeleportTime = 10;

		g_NewPVP.Reset(Obj);

		if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_INVISABLE) != 0)
		{
			if (Obj.MapNumber != MAP_INDEX_CASTLESIEGE)
			{
				gObjViewportListProtocolDestroy(Obj);
			}
		}
	}

	return true;
}




void gObjTeleport(CGameObject &Obj, int map, int x, int y)
{
	if (MapNumberCheck(map) == 0)
	{
		sLog->outBasic("error-L3 : %s %d", __FILE__, __LINE__);
		return;
	}

	if (Obj.MapNumber != map)
	{
		short sSvrCode = g_MapServerManager.CheckMoveMapSvr(Obj, map, Obj.m_sPrevMapSvrCode);

		if (sSvrCode != g_ConfigRead.server.GetGameServerCode())
		{
			if (sSvrCode == -1)
			{
				sLog->outError("[MapServerMng] Map Server Move Fail : CheckMoveMapSvr() == -1 [%s][%s] (%d)", Obj.AccountID, Obj.Name, Obj.m_Index);
				return;
			}

			GJReqMapSvrMove(Obj, sSvrCode, map, x, y);
			sLog->outBasic("[MapServerMng] Request to Move Map Server : (%d) - [%s][%s] (%d)", sSvrCode, Obj.AccountID, Obj.Name, Obj.m_Index);
			return;
		}
	}

	if (Obj.m_IfState->use == 1 && Obj.m_IfState->type == 1)
	{
		gGameProtocol.CGTradeCancelButtonRecv(Obj);
	}

	if (Obj.m_IfState->use == 1 && Obj.m_IfState->type == 6)
	{
		gGameProtocol.CGWarehouseUseEnd(Obj);
	}

	int nCurrentMapNum = Obj.MapNumber;

	Obj.m_State = 32;
	Obj.X = x;
	Obj.Y = y;

	Obj.TX = x;
	Obj.TY = y;

	Obj.MapNumber = map;
	Obj.PathCount = 0;
	Obj.Teleport = 0;

	if (Obj.Type == OBJ_USER && Obj.m_PlayerData->m_bIsMining)
		g_MineSystem.CheckMoveMapWhileMining(Obj);

	gObjViewportListProtocolDestroy(Obj);
	gGameProtocol.GCTeleportSend(Obj, 1, map, Obj.X, Obj.Y, Obj.Dir);
	gObjViewportListProtocolCreate(Obj);

	gObjClearViewport(Obj);

	if (Obj.m_Change >= 0)
	{
		gObjViewportListProtocolCreate(Obj);
	}

	Obj.RegenMapNumber = map;
	Obj.RegenMapX = x;
	Obj.RegenMapY = y;
	Obj.RegenOk = 1;
}




void gObjMoveDataLoadingOK(CGameObject &Obj)
{
	if (Obj.RegenOk == 1)
	{
		Obj.RegenOk = 2;
	}
}




class CMagicInf* gObjGetMagic(CGameObject &Obj, int mIndex)
{
	if (mIndex < 0 || mIndex > MAX_MAGIC - 1)
	{
		sLog->outBasic("Number of magic exceeds (%s %d)", __FILE__, __LINE__);
		return NULL;
	}

	if (Obj.Magic[mIndex].IsMagic() == FALSE)
	{
		return NULL;
	}

	return &Obj.Magic[mIndex];
}

CMagicInf * gObjGetMagicSearch(CGameObject &Obj, WORD skillnumber)
{

	if (skillnumber == 58)
	{
		skillnumber = 40;
	}

	if (skillnumber == 395)
	{
		skillnumber = 392;
	}

	for (int n = 0; n < MAGIC_SIZE; n++)
	{
		if (Obj.Magic[n].IsMagic() == 1)
		{
			if (Obj.Magic[n].m_Skill == skillnumber)
			{
				/*if(gObjMagicEnergyCheck(Obj,Obj.Magic[n].m_Skill) == 0)
				{
				gGameProtocol.GCServerMsgStringSend(Lang.GetText(0,147),Obj.m_Index,1);
				return false;
				}
				else
				{
				return &Obj.Magic[n];
				}*/
				return &Obj.Magic[n]; //Added by HermeX
			}
		}
	}
	return false;
}




int	gObjMagicManaUse(CGameObject &Obj, CMagicInf * lpMagic)
{
	float mana = Obj.Mana;
	float DecreaseMana;

	if (Obj.Type != OBJ_USER)
	{
		return false;
	}

	if (lpMagic == 0)
	{
		return false;
	}

	if (g_MasterLevelSkillTreeSystem.CheckMasterLevelSkill(lpMagic->m_Skill) == true)
	{
		DecreaseMana = MagicDamageC.SkillGetMana(lpMagic->m_Skill);
		DecreaseMana += DecreaseMana * g_MasterLevelSkillTreeSystem.GetMasterSkillManaIncRate(lpMagic->m_Level) / 100.0;
	}

	else
	{
		DecreaseMana = MagicDamageC.SkillGetMana(lpMagic->m_Skill);
	}

	DecreaseMana -= DecreaseMana * Obj.m_PlayerData->m_MPSkillOpt->iMpsDecreaseMana / 100.0;

	if (DecreaseMana < 1.0)
		DecreaseMana = 1.0;

	mana -= DecreaseMana;

	if (mana < 0)
	{
		return -1;
	}

	return mana;
}




int  gObjMagicBPUse(CGameObject &Obj, CMagicInf * lpMagic)
{
	int bp = Obj.BP;

	if (Obj.Type != OBJ_USER)
	{
		return false;
	}

	if (lpMagic == 0)
	{
		return false;
	}

	int usebp = MagicDamageC.SkillGetBP(lpMagic->m_Skill);

	if (Obj.m_PlayerData->m_btAGReduceRate > 0)
	{
		usebp -= usebp * Obj.m_PlayerData->m_btAGReduceRate / 100;
	}

	bp -= usebp;

	if (bp < 0)
	{
		return -1;
	}

	return bp;
}

void gObjMagicAddEnergyCheckOnOff(int flag)
{
	g_EnergyCheckOff = flag;
}

int  gObjMagicAdd(CGameObject &Obj, WORD Type, WORD Index, BYTE Level, WORD & SkillNumber)
{

	int skill = -1, n, reqeng, reqleadership;

	skill = GetSkillNumberInex(Type, Index, Level);

	if (skill < 0)
	{
		return -1;
	}

	if (g_EnergyCheckOff == 1)
	{
		reqeng = MagicDamageC.SkillGetRequireEnergy(skill);

		if (reqeng < 0)
		{
			return -1;
		}

		if ((Obj.m_PlayerData->Energy + Obj.AddEnergy) < reqeng)
		{
			return -1;
		}
	}

	reqleadership = MagicDamageC.GetskillRequireLeadership(skill);

	if (reqleadership < 0)
	{
		return -1;
	}

	if ((Obj.Leadership + Obj.AddLeadership) < reqleadership)
	{
		return -1;
	}

	if (MagicDamageC.SkillGetRequireClass(Obj.Class, Obj.m_PlayerData->ChangeUP, skill) < 1)
	{
		return -1;
	}

	if (g_MasterLevelSkillTreeSystem.IsExistMLSBasedSkill(Obj, skill) == TRUE)
	{
		return -1;
	}

	for (int n = 0; n < MAGIC_SIZE; n++)
	{
		if (Obj.Magic[n].IsMagic() == 1)
		{
			skill = GetSkillNumberInex(Type, Index, Level);

			if (skill < 0)
			{
				return -1;
			}

			if (Obj.Magic[n].m_Skill == skill)
			{
				return -1;
			}
		}
	}

	skill = -1;

	for (int n = 0; n < MAGIC_SIZE; n++)
	{
		if (Obj.Magic[n].IsMagic() == 0)
		{
			skill = Obj.Magic[n].Set(Index, Level);
			if (skill < 0)
			{
				return -1;
			}
			else
			{
				Obj.MagicCount++;
				SkillNumber = skill;
				return n;
			}
		}
	}

	return -1;
}




int  gObjMagicDel(CGameObject &Obj, WORD aSkill, BYTE Level)
{
	if (g_MasterLevelSkillTreeSystem.MLS_WeaponSkillDel(Obj, aSkill, Level) != FALSE) //season4 add-on
	{
		return -1;
	}

	for (int n = 0; n < MAGIC_SIZE; n++)
	{
		if (Obj.Magic[n].IsMagic() == 1)
		{
			if (Obj.Magic[n].m_Skill == aSkill)
			{
				Obj.Magic[n].Clear();
				return n;
			}
		}
	}
	return -1;
}




int  gObjMagicAdd(CGameObject &Obj, WORD aSkill, BYTE Level)
{
	int skill = -1, n, reqeng;

	skill = -1;

	if (g_EnergyCheckOff == 1 && Obj.Type == OBJ_USER && Obj.m_PlayerData->ISBOT == false)
	{
		reqeng = MagicDamageC.SkillGetRequireEnergy(aSkill);

		if (reqeng < 0)
		{
			return -1;
		}

		if ((Obj.m_PlayerData->Energy + Obj.AddEnergy) < reqeng)
		{
			return -1;
		}
	}

	if (MagicDamageC.SkillGetRequireClass(Obj.Class, Obj.m_PlayerData->ChangeUP, aSkill) < 1 && Obj.Type == OBJ_USER && Obj.m_PlayerData->ISBOT == false)
	{
		return -1;
	}

	if (g_MasterLevelSkillTreeSystem.IsExistMLSBasedSkill(Obj, aSkill) == true)
	{
		return -1;
	}

	for (int n = 0; n < MAGIC_SIZE; n++)
	{
		if (Obj.Magic[n].IsMagic() == 1)
		{
			if (Obj.Magic[n].m_Skill == aSkill)
			{
				if (g_EnergyCheckOff == 0)
				{
					return -1;
				}

				return -1;
			}
		}
	}

	for (int n = 0; n < MAGIC_SIZE; n++)
	{
		if (Obj.Magic[n].IsMagic() == 0)
		{
			skill = Obj.Magic[n].Set(aSkill, Level);

			if (skill < 0)
			{
				return -1;
			}
			else
			{
				Obj.MagicCount++;
				//sLog->outBasic("[gObjMagicAdd] (%s) %d - Skill:%d, Level:%d", Obj.Name, Obj.MagicCount, aSkill, Level);
				return n;
			}
		}
	}

	return -1;
}




int gObjWeaponMagicAdd(CGameObject &Obj, WORD aSkill, BYTE Level)
{
	int skill = -1, n;
	skill = -1;

	if (MagicDamageC.SkillGetRequireClass(Obj.Class, Obj.m_PlayerData->ChangeUP, aSkill) < 1)
	{
		return -1;
	}

	if (g_MasterLevelSkillTreeSystem.MLS_WeaponSkillAdd(Obj, aSkill, Level) == true)
	{
		return -1;
	}

	for (int n = 0; n < MAGIC_SIZE; n++)
	{
		if (Obj.Magic[n].IsMagic() == 0)
		{
			skill = Obj.Magic[n].Set(aSkill, Level);
			if (skill < 0)
			{
				return -1;
			}
			else
			{
				Obj.MagicCount++;
				return n;
			}
		}
	}

	return -1;
}




int gObjMonsterMagicAdd(CGameObject &Obj, WORD aSkill, BYTE Level)
{
	int skill = -1;
	for (int n = 0; n < MAGIC_SIZE; n++)
	{
		if (Obj.Magic[n].IsMagic() == FALSE)
		{
			skill = Obj.Magic[n].Set(aSkill, Level);

			if (skill < 0)
			{
				return -1;
			}

			Obj.MagicCount++;
			return n;
		}
	}

	return -1;
}




BOOL gObjMagicEnergyCheck(CGameObject &Obj, WORD aSkill)
{
	int reqeng, reqlevel;

	reqeng = MagicDamageC.SkillGetRequireEnergy(aSkill);

	if (reqeng < 0)
	{
		return false;
	}

	if ((Obj.m_PlayerData->Energy + Obj.AddEnergy) < reqeng)
	{
		return false;
	}

	reqlevel = MagicDamageC.SkillGetRequireLevel(aSkill);

	if (Obj.Level < reqlevel)
	{
		return false;
	}

	return true;
}

BOOL gObjSpecialItemLevelUp(CGameObject &Obj, int source, int target)
{
	if (source < 0 || source > MAIN_INVENTORY_SIZE - 1)
	{
		return false;
	}

	if (target < 0 || target > MAIN_INVENTORY_SIZE - 1)
	{
		return false;
	}

	if (Obj.pntInventory[source]->IsItem() == 0)
	{
		return false;
	}

	if (Obj.pntInventory[target]->IsItem() == 0)
	{
		return false;
	}

	if (g_LuckyItemManager.IsLuckyItemEquipment(Obj.pntInventory[target]->m_Type))	//1.01.00
	{
		return false;
	}

	Obj.pntInventory[target]->m_Level++;

	if (Obj.pntInventory[target]->m_Level > 15)
	{
		Obj.pntInventory[target]->m_Level = 15;
	}

	gObjMakePreviewCharSet(Obj);

	if (g_PentagramSystem.IsPentagramItem(Obj.pntInventory[target]->m_Type) == false)
	{
		float levelitemdur = ItemGetDurability(Obj.pntInventory[target]->m_Type, Obj.pntInventory[target]->m_Level, Obj.pntInventory[target]->IsExtItem(), Obj.pntInventory[target]->IsSetItem());

		Obj.pntInventory[target]->m_Durability = levelitemdur * Obj.pntInventory[target]->m_Durability / Obj.pntInventory[target]->m_BaseDurability;
	}

	Obj.pntInventory[target]->Convert(
		Obj.pntInventory[target]->m_Type,
		Obj.pntInventory[target]->m_Option1,
		Obj.pntInventory[target]->m_Option2,
		Obj.pntInventory[target]->m_Option3,
		Obj.pntInventory[target]->m_NewOption,
		Obj.pntInventory[target]->m_SetOption,
		Obj.pntInventory[target]->m_ItemOptionEx,
		Obj.pntInventory[target]->m_SocketOption,
		Obj.pntInventory[target]->m_BonusSocketOption,
		0,
		CURRENT_DB_VERSION);

	return true;
}

BOOL gObjSpecialItemRepair(CGameObject &Obj, int source, int target)
{
	if (source < 0 || source > MAIN_INVENTORY_SIZE - 1)
	{
		return false;
	}

	if (target < 0 || target > MAIN_INVENTORY_SIZE - 1)
	{
		return false;
	}

	if (Obj.pntInventory[source]->IsItem() == 0)
	{
		return false;
	}

	if (Obj.pntInventory[target]->IsItem() == 0)
	{
		return false;
	}

	Obj.pntInventory[target]->m_Durability = 255.0;

	gObjMakePreviewCharSet(Obj);

	Obj.pntInventory[target]->Convert(
		Obj.pntInventory[target]->m_Type,
		Obj.pntInventory[target]->m_Option1,
		Obj.pntInventory[target]->m_Option2,
		Obj.pntInventory[target]->m_Option3,
		Obj.pntInventory[target]->m_NewOption,
		Obj.pntInventory[target]->m_SetOption,
		Obj.pntInventory[target]->m_ItemOptionEx,
		Obj.pntInventory[target]->m_SocketOption,
		Obj.pntInventory[target]->m_BonusSocketOption,
		0,
		CURRENT_DB_VERSION);

	return true;
}

BOOL gObjItemLevelUp(CGameObject &Obj, int source, int target)
{
	if (source < 0 || source > MAIN_INVENTORY_SIZE - 1)
	{
		return false;
	}

	if (target < 0 || target > MAIN_INVENTORY_SIZE - 1)
	{
		return false;
	}

	if (Obj.pntInventory[source]->IsItem() == 0)
	{
		return false;
	}

	if (Obj.pntInventory[target]->IsItem() == 0)
	{
		return false;
	}

	BOOL bExceptionCheckResult = IsExceptionJewelOfBlessInchant(Obj, source, target);

	if (bExceptionCheckResult)
	{
		return bExceptionCheckResult;
	}

	if (IsNotApplyJewelToItems(Obj.pntInventory[target]->m_Type) == TRUE)
	{
		return false;
	}

	if (g_LuckyItemManager.IsLuckyItemEquipment(Obj.pntInventory[target]->m_Type) && g_ConfigRead.data.common.LuckyItemAllowUseJewels == false)	//1.01.00
	{
		return false;
	}

	if (Obj.pntInventory[target]->m_Level >= 6)
	{
		return false;
	}

	Obj.pntInventory[target]->m_Level++;
	gObjMakePreviewCharSet(Obj);

	if (g_PentagramSystem.IsPentagramItem(Obj.pntInventory[target]->m_Type) == false)
	{
		float levelitemdur = ItemGetDurability(Obj.pntInventory[target]->m_Type, Obj.pntInventory[target]->m_Level, Obj.pntInventory[target]->IsExtItem(), Obj.pntInventory[target]->IsSetItem());

		Obj.pntInventory[target]->m_Durability = levelitemdur * Obj.pntInventory[target]->m_Durability / Obj.pntInventory[target]->m_BaseDurability;
	}

	Obj.pntInventory[target]->Convert(
		Obj.pntInventory[target]->m_Type,
		Obj.pntInventory[target]->m_Option1,
		Obj.pntInventory[target]->m_Option2,
		Obj.pntInventory[target]->m_Option3,
		Obj.pntInventory[target]->m_NewOption,
		Obj.pntInventory[target]->m_SetOption,
		Obj.pntInventory[target]->m_ItemOptionEx,
		Obj.pntInventory[target]->m_SocketOption,
		Obj.pntInventory[target]->m_BonusSocketOption,
		0,
		CURRENT_DB_VERSION);

	return true;
}

BOOL gObjItemRandomLevelUp(CGameObject &Obj, int source, int target)
{
	if (source < 0 || source > MAIN_INVENTORY_SIZE - 1)
	{
		return false;
	}

	if (target < 0 || target > MAIN_INVENTORY_SIZE - 1)
	{
		return false;
	}

	if (Obj.pntInventory[source]->IsItem() == 0)
	{
		return false;
	}

	if (Obj.pntInventory[target]->IsItem() == 0)
	{
		return false;
	}

	if (IsNotApplyJewelToItems(Obj.pntInventory[target]->m_Type) == TRUE)
	{
		return false;
	}

	if (g_LuckyItemManager.IsLuckyItemEquipment(Obj.pntInventory[target]->m_Type) && g_ConfigRead.data.common.LuckyItemAllowUseJewels == false)	//1.01.00
	{
		return false;
	}

	if (Obj.pntInventory[target]->m_Level >= 9)
	{
		return false;
	}


	if (Obj.pntInventory[target]->m_Level > 9)
	{
		Obj.pntInventory[target]->m_Level = 9;
	}
	else
	{
		gObjMakePreviewCharSet(Obj);
	}

	if (g_PentagramSystem.IsPentagramItem(Obj.pntInventory[target]->m_Type) == false)
	{
		float levelitemdur = ItemGetDurability(Obj.pntInventory[target]->m_Type, Obj.pntInventory[target]->m_Level, Obj.pntInventory[target]->IsExtItem(), Obj.pntInventory[target]->IsSetItem());

		Obj.pntInventory[target]->m_Durability = levelitemdur * Obj.pntInventory[target]->m_Durability / Obj.pntInventory[target]->m_BaseDurability;
	}

	Obj.pntInventory[target]->Convert(
		Obj.pntInventory[target]->m_Type,
		Obj.pntInventory[target]->m_Option1,
		Obj.pntInventory[target]->m_Option2,
		Obj.pntInventory[target]->m_Option3,
		Obj.pntInventory[target]->m_NewOption,
		Obj.pntInventory[target]->m_SetOption,
		Obj.pntInventory[target]->m_ItemOptionEx,
		Obj.pntInventory[target]->m_SocketOption,
		Obj.pntInventory[target]->m_BonusSocketOption,
		0,
		CURRENT_DB_VERSION);

	if (g_kJewelOfHarmonySystem.IsStrengthenByJewelOfHarmony(Obj.pntInventory[target]) == 1)
	{
		if (g_kJewelOfHarmonySystem.IsActive(Obj.pntInventory[target]) == 0)
		{
			gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 271), Obj, 1);
		}
	}

	return true;
}

BOOL gObjItemRandomOption3Up(CGameObject &Obj, int source, int target)
{
	if (source < 0 || source > MAIN_INVENTORY_SIZE - 1)
	{
		return false;
	}

	if (target < 0 || target > MAIN_INVENTORY_SIZE - 1)
	{
		return false;
	}

	if (Obj.pntInventory[source]->IsItem() == 0)
	{
		return false;
	}

	if (Obj.pntInventory[target]->IsItem() == 0)
	{
		return false;
	}

	if (IsNotApplyJewelToItems(Obj.pntInventory[target]->m_Type) == TRUE)
	{
		return false;
	}

	if (Obj.pntInventory[target]->m_Type >= ITEMGET(12, 200) && Obj.pntInventory[target]->m_Type <= ITEMGET(12, 220))
	{
		return false;
	}

	if (Obj.pntInventory[target]->m_Type >= ITEMGET(12, 306) && Obj.pntInventory[target]->m_Type <= ITEMGET(12, 308))
	{
		return false;
	}

	int _r = rand() % 100;
	char maxopt = g_ConfigRead.data.common.Is28Opt == true ? 7 : 4;

	if (Obj.pntInventory[target]->m_Option3 == 0)
	{
		if ((Obj.pntInventory[target]->m_Type >= ITEMGET(12, 3) && Obj.pntInventory[target]->m_Type <= ITEMGET(12, 6)) ||
			Obj.pntInventory[target]->m_Type == ITEMGET(12, 49) || Obj.pntInventory[target]->m_Type == ITEMGET(12, 42))
		{
			Obj.pntInventory[target]->m_NewOption &= 0xDF;

			if (rand() % 2 == 1)
			{
				Obj.pntInventory[target]->m_NewOption |= 0x20;
			}
		}
	}

	if (Obj.pntInventory[target]->m_Option3 >= maxopt)
	{
		return false;
	}

	if (Obj.pntInventory[target]->m_Option3 < 7)
	{
		if (_r < 50)
		{
			Obj.pntInventory[target]->m_Option3++;
		}
		else
		{
			Obj.pntInventory[target]->m_Option3 = 0;
		}
	}
	else
	{
		return false;
	}

	gObjMakePreviewCharSet(Obj);

	float levelitemdur = ItemGetDurability(Obj.pntInventory[target]->m_Type, Obj.pntInventory[target]->m_Level, Obj.pntInventory[target]->IsExtItem(), Obj.pntInventory[target]->IsSetItem());

	Obj.pntInventory[target]->m_Durability = levelitemdur * Obj.pntInventory[target]->m_Durability / Obj.pntInventory[target]->m_BaseDurability;

	Obj.pntInventory[target]->Convert(
		Obj.pntInventory[target]->m_Type,
		Obj.pntInventory[target]->m_Option1,
		Obj.pntInventory[target]->m_Option2,
		Obj.pntInventory[target]->m_Option3,
		Obj.pntInventory[target]->m_NewOption,
		Obj.pntInventory[target]->m_SetOption,
		Obj.pntInventory[target]->m_ItemOptionEx,
		Obj.pntInventory[target]->m_SocketOption,
		Obj.pntInventory[target]->m_BonusSocketOption,
		0,
		CURRENT_DB_VERSION);

	BYTE NewOption[8];

	ItemIsBufExOption(NewOption, Obj.pntInventory[target]);

	return true;
}




void gObjAbilityReSet(CGameObject &Obj)
{
	Obj.m_PlayerData->LevelUpPoint = (Obj.Level - 1) * 5;

	Obj.m_PlayerData->Strength = DCInfo.DefClass[Obj.Class].Strength;
	Obj.m_PlayerData->Dexterity = DCInfo.DefClass[Obj.Class].Dexterity;
	Obj.m_PlayerData->Vitality = DCInfo.DefClass[Obj.Class].Vitality;
	Obj.m_PlayerData->Energy = DCInfo.DefClass[Obj.Class].Energy;

	Obj.MaxLife = DCInfo.DefClass[Obj.Class].MaxLife;
	Obj.MaxMana = DCInfo.DefClass[Obj.Class].MaxMana;

	if (Obj.Class == CLASS_WIZARD)
	{
		Obj.MaxLife += (Obj.Level - 1)*0.5f;
		Obj.MaxMana += 2.0f * (Obj.Level - 1);

		Obj.Life = Obj.MaxLife;
		Obj.Mana = Obj.MaxMana;
	}
	else
	{
		Obj.MaxLife += 2.0f * (Obj.Level - 1);
		Obj.MaxMana += (Obj.Level - 1)*0.5f;

		Obj.Life = Obj.MaxLife;
		Obj.Mana = Obj.MaxMana;
	}

	IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
}




BOOL gObjTargetGuildWarCheck(CGameObject &Obj, CGameObject &TargetObj)
{
	if (Obj.Type != OBJ_USER)
	{
		return false;
	}

	if (TargetObj.Type != OBJ_USER)
	{
		return false;
	}

	if (Obj.m_PlayerData->GuildNumber < 1 || TargetObj.m_PlayerData->GuildNumber < 1)
	{
		return false;
	}

	if (Obj.m_PlayerData->lpGuild->WarState != 1 || TargetObj.m_PlayerData->lpGuild->WarState != 1)
	{
		return false;
	}

	if (strcmp(Obj.m_PlayerData->lpGuild->TargetGuildName, TargetObj.m_PlayerData->lpGuild->Name))
	{
		return false;
	}
	return true;
}




void gObjGuildWarEndSend(CGameObject &Obj, BYTE Result1, BYTE Result2)
{
	if (Obj.m_PlayerData->lpGuild == 0)
	{
		sLog->outBasic("error-L3 : %s %d", __FILE__, __LINE__);
		return;
	}

	if (Obj.m_PlayerData->lpGuild->WarState != 1)
	{
		sLog->outBasic("error-L3 : %s %d", __FILE__, __LINE__);
		return;
	}

	if (Obj.m_PlayerData->lpGuild->lpTargetGuildNode == 0)
	{
		sLog->outBasic("error-L3 : %s %d", __FILE__, __LINE__);
		return;
	}

	if (Result1 == 1)
	{
		gObjGuildWarItemGive(*Obj.m_PlayerData->lpGuild, *Obj.m_PlayerData->lpGuild->lpTargetGuildNode);
	}
	else if (Result1 == 3)
	{
		gObjGuildWarItemGive(*Obj.m_PlayerData->lpGuild->lpTargetGuildNode, *Obj.m_PlayerData->lpGuild);
	}
	else if (Result1 == 1)
	{
		gObjGuildWarItemGive(*Obj.m_PlayerData->lpGuild->lpTargetGuildNode, *Obj.m_PlayerData->lpGuild);
	}

	for (int n = 0; n < MAX_USER_GUILD; n++)
	{
		if (Obj.m_PlayerData->lpGuild->Use[n] && Obj.m_PlayerData->lpGuild->Index[n] >= 0)
		{
			CGameObject* lpObj = getGameObject(Obj.m_PlayerData->lpGuild->Index[n] );
			getGameObject(Obj.m_PlayerData->lpGuild->Index[n])->IsInBattleGround = 0;
			gGameProtocol.GCGuildWarEnd(*lpObj, Result1, Obj.m_PlayerData->lpGuild->lpTargetGuildNode->Name);
		}
	}

	for (int n = 0; n < MAX_USER_GUILD; n++)
	{
		if (Obj.m_PlayerData->lpGuild->lpTargetGuildNode->Use[n] && Obj.m_PlayerData->lpGuild->lpTargetGuildNode->Index[n] >= 0)
		{
			CGameObject* lpObj = getGameObject( Obj.m_PlayerData->lpGuild->lpTargetGuildNode->Index[n] );
			getGameObject(Obj.m_PlayerData->lpGuild->lpTargetGuildNode->Index[n])->IsInBattleGround = 0;
			gGameProtocol.GCGuildWarEnd(*lpObj, Result2, Obj.m_PlayerData->lpGuild->Name);
		}
	}

	PMSG_NOTICE pNotice;
	TNotice::MakeNoticeMsgEx(&pNotice, 0, Lang.GetText(0, 416), Obj.m_PlayerData->lpGuild->Name, Obj.m_PlayerData->lpGuild->lpTargetGuildNode->Name, Result1, Result2);
	TNotice::SendNoticeToAllUser(&pNotice);
}

void gObjGuildWarEndSend(GUILD_INFO_STRUCT * lpGuild1, GUILD_INFO_STRUCT * lpGuild2, BYTE Result1, BYTE Result2)
{
	
	if (lpGuild1 == 0 || lpGuild2 == 0)
	{
		return;
	}

	if (lpGuild1->WarState != 1 || lpGuild2->WarState != 1)
	{
		return;
	}

	for (int n = 0; n < MAX_USER_GUILD; n++)
	{
		CGameObject* lpObjGuild1 = getGameObject(lpGuild1->Index[n]);
		CGameObject* lpObjGuild2 = getGameObject(lpGuild1->Index[n]);
		if (lpGuild1->Use[n] && lpGuild1->Index[n] >= 0)
		{
			lpObjGuild1->IsInBattleGround = 0;
			gGameProtocol.GCGuildWarEnd(*lpObjGuild1, Result1, lpGuild2->Name);
		}
		if (lpGuild2->Use[n] && lpGuild2->Index[n] >= 0)
		{
			getGameObject(lpGuild2->Index[n])->IsInBattleGround = 0;
			gGameProtocol.GCGuildWarEnd(*lpObjGuild1, Result2, lpGuild1->Name);
		}
	}

	PMSG_NOTICE pNotice;
	TNotice::MakeNoticeMsgEx(&pNotice, 0, Lang.GetText(0, 416), lpGuild1->Name, lpGuild2->lpTargetGuildNode->Name, Result1, Result2);
	TNotice::SendNoticeToAllUser(&pNotice);
}




void gObjGuildWarEnd(GUILD_INFO_STRUCT &lpGuild, GUILD_INFO_STRUCT &lpTargetGuild)
{
	lpGuild.WarDeclareState = 0;
	lpGuild.WarState = 0;

	if (lpTargetGuild.Number != 0)
	{
		lpTargetGuild.WarDeclareState = 0;
		lpTargetGuild.WarState = 0;

		if (lpGuild.lpTargetGuildNode != 0)
		{
			GUILD_INFO_STRUCT * lpTguild = lpGuild.lpTargetGuildNode;

			lpGuild.lpTargetGuildNode = 0;
			lpTargetGuild.lpTargetGuildNode = 0;
		}
	}
	else
	{
		lpGuild.lpTargetGuildNode = 0;
	}
}




BOOL gObjGuildWarProc(GUILD_INFO_STRUCT * lpGuild1, GUILD_INFO_STRUCT * lpGuild2, int score)
{
	int totalscore;
	int n;
	int maxscore;
	char szTemp[0x100];

	if (lpGuild1 == 0 || lpGuild2 == 0)
	{
		return false;
	}

	totalscore = 1;
	maxscore = 20;

	lpGuild1->PlayScore += score;

	wsprintf(szTemp, "%s ( %d ) VS %s ( %d )", lpGuild1->Name, lpGuild1->PlayScore, lpGuild2->Name, lpGuild2->PlayScore);
	sLog->outBasic(szTemp);

	if (lpGuild1->WarType == 1)
	{
		SetBattleTeamScore(lpGuild1->BattleGroundIndex, lpGuild1->BattleTeamCode, lpGuild1->PlayScore);
		maxscore = 100;
	}
	else
	{
		maxscore = 20;
	}

	cManager.BattleInfoSend(GetBattleTeamName(0, 0), GetBattleTeamScore(0, 0), GetBattleTeamName(0, 1), GetBattleTeamScore(0, 1));

	if (lpGuild1->PlayScore >= maxscore)
	{
		if (lpGuild1->PlayScore > maxscore && lpGuild2->PlayScore == 0)
		{
			totalscore = 3;
		}
		else if (lpGuild1->PlayScore > maxscore && lpGuild2->PlayScore <= 10)
		{
			totalscore = 2;
		}

		lpGuild1->TotalScore += totalscore;
		DGGuildScoreUpdate(lpGuild1->Name, lpGuild1->TotalScore);
		DGGuildScoreUpdate(lpGuild2->Name, lpGuild2->TotalScore);

		lpGuild1->PlayScore = 0;
		lpGuild2->PlayScore = 0;

		return true;
	}

	for (int n = 0; n < MAX_USER_GUILD; n++)
	{
		if (lpGuild1->Use[n] && lpGuild1->Index[n] >= 0)
		{
			gGameProtocol.GCGuildWarScore(*getGameObject(lpGuild1->Index[n]));
		}
		if (lpGuild2->Use[n] && lpGuild2->Index[n] >= 0)
		{
			gGameProtocol.GCGuildWarScore(*getGameObject(lpGuild2->Index[n]));
		}
	}
	return false;
}


BOOL gObjGuildIsLeader(CGameObject &Obj, CGameObject &TargetObj)
{
	if (strcmp(Obj.Name, TargetObj.m_PlayerData->lpGuild->Users[0]->Name) == 0)
		return TRUE;
	return FALSE;
}

BOOL gObjGuildWarCheck(CGameObject &Obj, CGameObject &TargetObj)
{
	int score = 1;

	if (gObjTargetGuildWarCheck(Obj, TargetObj) == 0)
	{
		return false;
	}

	if (gObjGuildIsLeader(Obj, TargetObj))
	{
		score = 2;
	}

	GUILD_INFO_STRUCT * lpGuild = Obj.m_PlayerData->lpGuild;
	GUILD_INFO_STRUCT * lpTargetGuild = TargetObj.m_PlayerData->lpGuild;

	if (gObjGuildWarProc(lpGuild, lpTargetGuild, score) == 1)
	{
		if (lpGuild->WarType == 1)
		{
			gObjGuildWarEndSend(lpGuild, lpTargetGuild, 1, 0);
			gBattleGroundEnable(lpGuild->BattleGroundIndex, 0);
			gObjGuildWarEnd(*lpGuild, *lpTargetGuild);
			cManager.BattleInfoSend(GetBattleTeamName(0, 0), 255, GetBattleTeamName(0, 1), 255);
		}
		else
		{
			gObjGuildWarEndSend(Obj, 1, 0);
			gObjAddMsgSendDelay(Obj, 4, Obj, 2000, 0);
		}
	}
	return true;
}




BOOL gObjGuildWarMasterClose(CGameObject &Obj)
{
	if (Obj.m_PlayerData->GuildNumber < 1)
	{
		return false;
	}

	// TODO - Is GuildMember Owner.
	/*
	if (strcmp(Obj.m_PlayerData->lpGuild->Names[0], Obj.Name))
	{
		return false;
	}
	*/

	gObjGuildWarEndSend(Obj, 3, 2);

	if (Obj.m_PlayerData->lpGuild->WarType == 1 && Obj.m_PlayerData->lpGuild->BattleGroundIndex < 1)
	{
		gBattleGroundEnable(Obj.m_PlayerData->lpGuild->BattleGroundIndex, 0);
		cManager.BattleInfoSend(GetBattleTeamName(0, 0), 255, GetBattleTeamName(0, 1), 255);
	}
	gObjGuildWarEnd(*Obj.m_PlayerData->lpGuild, *Obj.m_PlayerData->lpGuild->lpTargetGuildNode);
	return true;
}




int gObjGuildWarItemGive(GUILD_INFO_STRUCT * lpWinGuild, GUILD_INFO_STRUCT * lpLoseGuild)
{
	return true;
	int n;
	int r_userindex[MAX_USER_GUILD];
	int r_usercount;

	for (int n = 0; n < MAX_USER_GUILD; n++)
	{
		if (lpLoseGuild->Use[n] > 0)
		{
			r_userindex[r_usercount] = lpLoseGuild->Index[n];
			r_usercount++;
		}
	}

	if (r_usercount < 1)
	{
		sLog->outBasic("error2: No user in the loser guild  is in game %s %d", __FILE__, __LINE__);
		return false;
	}

	int lose_user = r_userindex[rand() % r_usercount];

	if (lose_user < 0)
	{
		sLog->outBasic("error2: No user in the loser guild  is in game %s %d", __FILE__, __LINE__);
		return false;
	}

	if (getGameObject(lose_user)->Connected < PLAYER_PLAYING)
	{
		sLog->outBasic("error2: No user in the loser guild  is in game %s %d", __FILE__, __LINE__);
		return false;
	}

	r_usercount = 0;

	for (int n = 0; n < MAX_USER_GUILD; n++)
	{
		if (lpWinGuild->Use[n] > 0)
		{
			r_userindex[r_usercount] = lpWinGuild->Index[n];
			r_usercount++;
		}
	}

	if (r_usercount < 1)
	{
		sLog->outBasic("error2: No user in the winner guild is in game");
		return false;
	}

	int win_user = r_userindex[rand() % r_usercount];
	
	if (win_user < 0)
	{
		sLog->outBasic("error2: No user in the winner guild is in game");
		return false;
	}

	CGameObject* lpObjWinUser = getGameObject(win_user);

	if (lpObjWinUser->Connected < PLAYER_PLAYING)
	{
		sLog->outBasic("error2: No user in the winner guild is in game");
		return false;
	}

	CGameObject* lpObjLoser = getGameObject(lose_user);

	int count = 24;

	int number;

	while (count--)
	{
		number = rand() % 12;

		if (lpObjLoser->pntInventory[number]->IsItem() == 1)
		{
			if (gObjInventoryInsertItem(*lpObjWinUser, *lpObjLoser->pntInventory[number]) != 0xFF)
			{
				return true;
			}
			break;
		}
	}

	count = 64;

	while (count--)
	{
		number = rand() % 64 + 12;

		if (lpObjLoser->pntInventory[number]->IsItem() == 1)
		{
			if (gObjInventoryInsertItem(*lpObjWinUser, *lpObjLoser->pntInventory[number]) != 0xFF)
			{
				return true;
			}
			break;
		}
	}
	return true;
}




int gObjGetPkTime(CGameObject &Obj, int& hour, int& min)
{
	int pktime = 0;

	if (Obj.m_PK_Time > 0)
	{
		pktime = Obj.m_PK_Time / 60;
	}

	if (Obj.m_PK_Level == 4)
	{
		hour = (180 - pktime) / 60;
		min = (180 - pktime) % 60;
		return true;
	}

	if (Obj.m_PK_Level == 5)
	{
		hour = (360 - pktime) / 60;
		min = (360 - pktime) % 60;
		return true;
	}

	if (Obj.m_PK_Level >= 6)
	{
		hour = (((Obj.m_PK_Count - 3) * 60 + 540) - pktime) / 60;
		min = (((Obj.m_PK_Count - 3) * 60 + 540) - pktime) % 60;
		return true;
	}

	return false;
}




BOOL gObjMonsterCall(CGameObject &Obj, int MonsterType, int x, int y)
{

	if (Obj.MapNumber == MAP_INDEX_ICARUS)
	{
		return false;
	}

	if (Obj.m_RecallMon >= 0)
	{

		gGameProtocol.GCRecallMonLife(Obj, 60, 0);
		gObjMonsterCallKill(Obj);
		return false;
	}

	int result = gObjAddCallMon();

	CGameObject* lpMonster = lpMonster;

	if (result >= 0)
	{
		lpMonster->X = x;
		lpMonster->Y = y;

		lpMonster->MTX = x;
		lpMonster->MTY = y;
		lpMonster->Dir = 2;

		lpMonster->MapNumber = Obj.MapNumber;

		gObjSetMonster(*lpMonster, MonsterType);

		lpMonster->m_RecallMon = Obj.m_Index;
		lpMonster->m_Attribute = 100;

		lpMonster->TargetNumber = -1;
		lpMonster->m_ActState->Emotion = 0;
		lpMonster->m_ActState->Attack = 0;
		lpMonster->m_ActState->EmotionCount = 0;

		lpMonster->PathCount = 0;

		Obj.m_RecallMon = result;
		lpMonster->m_MoveRange = 15;

		gGameProtocol.GCRecallMonLife(Obj, lpMonster->MaxLife, lpMonster->Life);
		sLog->outBasic("[Summon Monster] [%s][%s] Try to Summon Monster - Succeed (SummonIndex:%d)", Obj.AccountID, Obj.Name, result);
		return true;
	}
	return false;
}




void gObjMonsterCallKill(CGameObject &Obj)
{

	sLog->outBasic("[Summon Monster] [%s][%s] Try to Kill Summoned Monster", Obj.AccountID, Obj.Name);

	if (Obj.m_RecallMon < 0)
	{
		return;
	}

	int callmon = Obj.m_RecallMon;
	CGameObject* lpCallMon = getGameObject(callmon);

	if (lpCallMon->Type != OBJ_MONSTER)
	{
		return;
	}

	if (lpCallMon->m_RecallMon != Obj.m_Index)
	{
		return;
	}

	if (Obj.m_RecallMon >= 0)
	{
		if (gObjIsConnected(*lpCallMon) == 1)
		{
			sLog->outBasic("[Summon Monster] [%s][%s] Try to Kill Summoned Monster - Found Summoned Monster (SummonIndex:%d)", Obj.AccountID, Obj.Name, Obj.m_RecallMon);
			lpCallMon->m_RecallMon = -1;
		}
	}

	Obj.m_RecallMon = -1;
	gObjDel(*lpCallMon);

	sLog->outBasic("[Summon Monster] [%s][%s] Try to Kill Summoned Monster - Finished to Kill Summoned Monster (SummonIndex:%d)", Obj.AccountID, Obj.Name, callmon);
}




BOOL gObjCheckTeleportArea(CGameObject &Obj, BYTE x, BYTE y)
{

	if (x < (Obj.X - 8)
		|| x >(Obj.X + 8)
		|| y < (Obj.Y - 8)
		|| y >(Obj.Y + 8))
	{
		return false;
	}

	int mapnumber = Obj.MapNumber;

	if (mapnumber < 0 || mapnumber >= MAX_NUMBER_MAP)
	{
		return false;
	}

	if (IMPERIAL_MAP_RANGE(mapnumber))
	{
		return false;
	}

	BYTE attr = MapC[mapnumber].GetAttr(x, y);

	if (attr)
	{
		return false;
	}

	attr = MapC[mapnumber].GetAttr(Obj.X, Obj.Y);

	if (attr & 1)
	{
		return false;
	}

	return true;
}

bool gObjCheckMoveArea(CGameObject &Obj, int X, int Y)
{
	int iXDiff = abs(Obj.X - X);
	int iYDiff = abs(Obj.Y - Y);

	if (iXDiff > 30 || iYDiff > 30)
	{
		return false;
	}

	return true;
}



BOOL gObjCheckAttackAreaUsedPath(CGameObject &Obj, int TarObjIndex)
{
	int X, Y;
	if (Obj.PathCount > 1)
	{
		X = Obj.PathX[Obj.PathCount - 1];
		Y = Obj.PathY[Obj.PathCount - 1];
	}
	else
	{
		return false;
	}

	if (getGameObject(TarObjIndex)->X < (X - 10)
		|| getGameObject(TarObjIndex)->X >(X + 10)
		|| getGameObject(TarObjIndex)->Y < (Y - 10)
		|| getGameObject(TarObjIndex)->Y >(Y + 10))
	{
		return false;
	}
	return true;
}




BOOL gObjCheckattackAreaUsedViewPort(CGameObject &Obj, int TarObjIndex)
{
	
	CGameObject lpTargetObj = *getGameObject(TarObjIndex);

	if (lpTargetObj.m_Index == Obj.m_Index)
	{
		return true;
	}

	int MVL = MAX_VIEWPORT;

	if (Obj.Type == OBJ_MONSTER)
	{
		MVL = MaxViewportMonster;
	}

	for (int n = 0; n < MVL; n++)
	{
		if (Obj.VpPlayer[n].state)
		{
			if (Obj.VpPlayer[n].number == lpTargetObj.m_Index)
			{
				return true;
			}
		}

		if (Obj.VpPlayer2[n].state)
		{
			if (Obj.VpPlayer2[n].number == lpTargetObj.m_Index)
			{
				return true;
			}
		}
	}
	return false;
}




int  gObjCheckAttackArea(CGameObject &Obj, int TarObjIndex)
{

	if (gObjCheckattackAreaUsedViewPort(Obj, TarObjIndex) == 0)
	{
		return 1;
	}

	int mapnumber = Obj.MapNumber;

	if (mapnumber < 0 || mapnumber >= MAX_NUMBER_MAP)
	{
		return 2;
	}

	int tarmapnumber = getGameObject(TarObjIndex)->MapNumber;

	if (tarmapnumber < 0 || tarmapnumber >= MAX_NUMBER_MAP)
	{
		return 3;
	}

	if (tarmapnumber != mapnumber)
	{
		return 4;
	}

	BYTE attr;

	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		attr = MapC[mapnumber].GetAttr(getGameObject(TarObjIndex)->X, getGameObject(TarObjIndex)->Y);

		if (attr && !(attr & 2))
		{
			return 5;
		}

	}

	else
	{
		attr = 0;
	}

	attr = MapC[mapnumber].GetAttr(Obj.X, Obj.Y);

	if (attr && !(attr & 2))
	{
		return 6;
	}

	return false;
}




BOOL gUserFindDevilSquareInvitation(CGameObject &Obj)
{
	

	for (int n = 0; n < MAIN_INVENTORY_SIZE; n++)
	{
		if (Obj.pntInventory[n]->IsItem() == 1)
		{
			if (Obj.pntInventory[n]->m_Type == ITEMGET(14, 19))
			{
				return true;
			}

			if (Obj.pntInventory[n]->m_Type == ITEMGET(13, 46))
			{
				return true;
			}
		}
	}
	return false;
}




BOOL gUserFindDevilSquareKeyEyes(CGameObject &Obj)
{
	
	int bFindKey = 0;
	int bFindEyes = 0;

	for (int n = 0; n < MAIN_INVENTORY_SIZE; n++)
	{
		if (Obj.pntInventory[n]->IsItem() == 1)
		{
			if (Obj.pntInventory[n]->m_Type == ITEMGET(14, 17))
			{
				bFindEyes = 1;
			}

			if (Obj.pntInventory[n]->m_Type == ITEMGET(14, 18))
			{
				bFindKey = 1;
			}
		}
	}

	if (bFindKey != 0 && bFindEyes != 0)
	{
		return true;
	}
	return false;
}




void gObjSendUserStatistic(CGameObject &Obj, int startLevel, int endLevel)
{
	if (gDisconnect == 1)
	{
		return;
	}

	if (endLevel < startLevel)
	{
		endLevel = g_ConfigRead.data.common.UserMaxLevel;
	}

	int n;
	CGameObject lpObj;
	int sClassCount[8];

	sClassCount[CLASS_WIZARD] = 0;
	sClassCount[CLASS_KNIGHT] = 0;
	sClassCount[CLASS_ELF] = 0;
	sClassCount[CLASS_MAGUMSA] = 0;
	sClassCount[CLASS_DARKLORD] = 0;
	sClassCount[CLASS_SUMMONER] = 0;
	sClassCount[CLASS_RAGEFIGHTER] = 0;
	sClassCount[CLASS_GROWLANCER] = 0;

	for (int n = g_ConfigRead.server.GetObjectStartUserIndex(); n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		lpObj = *getGameObject(n);

		if (Obj.Connected > PLAYER_LOGGED)
		{
			if (Obj.Level >= startLevel && Obj.Level <= endLevel)
			{
				sClassCount[Obj.Class]++;
			}

			sClassCount[8]++;
		}
	}

	MsgOutput(Obj, Lang.GetText(0, 578), startLevel, endLevel, sClassCount[7],
		Lang.GetText(0, 202), sClassCount[CLASS_WIZARD],
		Lang.GetText(0, 203), sClassCount[CLASS_KNIGHT],
		Lang.GetText(0, 204), sClassCount[CLASS_ELF],
		Lang.GetText(0, 205), sClassCount[CLASS_MAGUMSA],
		Lang.GetText(0, 206), sClassCount[CLASS_DARKLORD],
		Lang.GetText(0, 207), sClassCount[CLASS_SUMMONER],
		Lang.GetText(0, 208), sClassCount[CLASS_RAGEFIGHTER],
		Lang.GetText(0, 684), sClassCount[CLASS_GROWLANCER]);
}




CGameObject* gObjFind(char * targetcharname)
{
	if (gDisconnect == TRUE)
	{
		return nullptr;
	}

	int n;
	CGameObject* lpObj;

	for (n = g_ConfigRead.server.GetObjectStartUserIndex(); n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		lpObj = getGameObject(n);

		if (lpObj->Connected > PLAYER_LOGGED)
		{
			if (lpObj->Name[0] == targetcharname[0])
			{
				if (strcmp(lpObj->Name, targetcharname) == 0)
				{
					return lpObj;
				}
			}
		}
	}

	return NULL;

}




BOOL gObjFind10EventChip(CGameObject &Obj)
{
	
	int count = 0;

	for (int n = 0; n < MAIN_INVENTORY_SIZE; n++)
	{
		if (Obj.pntInventory[n]->IsItem() == 1)
		{
			if (Obj.pntInventory[n]->m_Type == ITEMGET(14, 21))
			{
				count++;

				if (count >= 10)
				{
					return true;
				}
			}
		}
	}
	return false;
}




BOOL gObjDelete10EventChip(CGameObject &Obj)
{
	
	int count = 0;

	for (int n = 0; n < MAIN_INVENTORY_SIZE; n++)
	{
		if (Obj.pntInventory[n]->IsItem() == 1)
		{
			if (Obj.pntInventory[n]->m_Type == ITEMGET(14, 21))
			{
				gObjInventoryDeleteItem(Obj, n);
				gGameProtocol.GCInventoryItemDeleteSend(Obj, n, 1);
				sLog->outBasic("[EventChip] [%s][%s] Delete Rena (%d)", Obj.AccountID, Obj.Name, n);
				count++;

				if (count >= 10)
				{
					return true;
				}
			}
		}
	}
	return false;
}

void gObjSetPosition(CGameObject &Obj, int x, int y)
{
	if (MAX_MAP_RANGE(Obj.MapNumber) == FALSE)
		return;

	
	PMSG_POSISTION_SET pMove;

	pMove.h.c = 0xC1;
	pMove.h.headcode = 0x10;
	pMove.h.size = sizeof(pMove);

	pMove.X = x;
	pMove.Y = y;

	Obj.m_Rest = 0;

	MapC[Obj.MapNumber].ClearStandAttr(Obj.m_OldX, Obj.m_OldY);
	MapC[Obj.MapNumber].SetStandAttr(x, y);

	Obj.m_OldX = x;
	Obj.m_OldY = y;

	gGameProtocol.RecvPositionSetProc(&pMove, Obj);
}




int  gObjGetItemCountInEquipment(CGameObject &Obj, int itemtype, int itemindex, int itemlevel)
{
	
	int count = 0;

	for (int n = 0; n < MAX_PLAYER_EQUIPMENT; n++)
	{
		if (Obj.pntInventory[n]->IsItem() == TRUE)
		{
			if (Obj.pntInventory[n]->m_Type == ((itemtype * MAX_SUBTYPE_ITEMS) + itemindex))
			{
				if (itemlevel != -1)
				{
					if (Obj.pntInventory[n]->m_Level != itemlevel)
					{
						continue;
					}
				}

				count++;
			}
		}
	}

	return count;
}





int  gObjGetItemCountInIventory(CGameObject &Obj, int itemtype, int itemindex, int itemlevel)
{
	
	int count = 0;

	for (int n = 0; n < MAIN_INVENTORY_SIZE; n++)
	{
		if (Obj.pntInventory[n]->IsItem() == TRUE)
		{
			if (Obj.pntInventory[n]->m_Type == ((itemtype * MAX_SUBTYPE_ITEMS) + itemindex))
			{
				if (itemlevel != -1)
				{
					if (Obj.pntInventory[n]->m_Level != itemlevel)
					{
						continue;
					}
				}

				count++;
			}
		}
	}

	return count;
}




int gObjGetItemCountInIventory(CGameObject &Obj, int itemnum)
{
	
	int count = 0;

	for (int n = 0; n < MAIN_INVENTORY_SIZE; n++)
	{
		if (Obj.pntInventory[n]->IsItem() == TRUE)
		{
			if (Obj.pntInventory[n]->m_Type == itemnum)
			{
				count++;
			}
		}
	}

	return count;

}




int  gObjGetManaItemPos(CGameObject &Obj)	// [Failure : -1]
{
	
	int count = 0;

	for (int n = 0; n < MAIN_INVENTORY_SIZE; n++)
	{
		if (Obj.pntInventory[n]->IsItem() == TRUE)
		{
			if (Obj.pntInventory[n]->m_Type == ITEMGET(14, 4) || Obj.pntInventory[n]->m_Type == ITEMGET(14, 5) || Obj.pntInventory[n]->m_Type == ITEMGET(14, 6))
			{
				return n;
			}
		}
	}

	return -1;
}




void gObjDelteItemCountInInventory(CGameObject &Obj, int itemtype, int itemindex, int count)
{
	
	int delcount = 0;

	for (int n = 0; n < MAIN_INVENTORY_SIZE; n++)
	{
		if (Obj.pntInventory[n]->IsItem() == 1)
		{
			if (Obj.pntInventory[n]->m_Type == itemtype * 512 + itemindex)
			{
				gObjInventoryDeleteItem(Obj, n);
				gGameProtocol.GCInventoryItemDeleteSend(Obj, n, 1);
				sLog->outBasic("[DeleteItem] [%s][%s] Delete (%d,%d)(%d)", Obj.AccountID, Obj.Name, itemtype, itemindex, n);
				delcount++;

				if (delcount >= count)
				{
					return;
				}
			}
		}
	}
}




void gObjGetStatPointState(CGameObject &Obj, short& AddPoint, short& MaxAddPoint, short& MinusPoint, short& MaxMinusPoint)
{
	if (Obj.Level <= 5)
	{
		AddPoint = 0;
		MaxAddPoint = 0;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	

	int total_point = Obj.m_PlayerData->Strength + Obj.m_PlayerData->Dexterity + Obj.m_PlayerData->Vitality + Obj.m_PlayerData->Energy + Obj.Leadership + Obj.m_PlayerData->LevelUpPoint;
	int ori_point;

	if (Obj.Class == CLASS_DARKLORD || Obj.Class == CLASS_MAGUMSA || Obj.Class == CLASS_RAGEFIGHTER || Obj.Class == CLASS_GROWLANCER)
	{
		ori_point = DCInfo.GetDefPoint(Obj.Class) + (Obj.Level - 1) * 7;
	}
	else
	{
		ori_point = DCInfo.GetDefPoint(Obj.Class) + (Obj.Level - 1) * 5;
	}

	if (g_QuestInfo.GetQuestState(Obj, 0) == 2)
	{
		ori_point += 10;
	}

	if (g_QuestInfo.GetQuestState(Obj, 1) == 2)
	{
		ori_point += 10;
	}

	if (g_QuestInfo.GetQuestState(Obj, 2) == 2)
	{
		ori_point += Obj.Level - 220;
	}

	//Season3 Quest MaxPoint Calculation
	if (g_QuestInfo.GetQuestState(Obj, 4) == 2)
	{
		ori_point += 20;
	}

	if (g_QuestInfo.GetQuestState(Obj, 5) == 2)
	{
		ori_point += 20;
	}

	if (g_QuestInfo.GetQuestState(Obj, 6) == 2)
	{
		ori_point += 30;
	}

	int addpoint = ori_point;

	addpoint += gStatMng.GetMaxStat(Obj.Level, Obj.Class);

	AddPoint = total_point - ori_point;
	MaxAddPoint = gStatMng.GetMaxStat(Obj.Level, Obj.Class);
	MinusPoint = Obj.m_PlayerData->iFruitPoint;
	MaxMinusPoint = gStatMng.GetMaxMinusStat(Obj.Level, Obj.Class);
}




BOOL gObjCheckStatPointUp(CGameObject &Obj)
{
	

	if (Obj.Type != OBJ_USER)
	{
		return FALSE;
	}

	int total_point = Obj.m_PlayerData->Strength + Obj.m_PlayerData->Dexterity + Obj.m_PlayerData->Vitality + Obj.m_PlayerData->Energy + Obj.Leadership + Obj.m_PlayerData->LevelUpPoint;
	int ori_point;

	if (Obj.Class == CLASS_DARKLORD || Obj.Class == CLASS_MAGUMSA || Obj.Class == CLASS_RAGEFIGHTER || Obj.Class == CLASS_GROWLANCER)
	{
		ori_point = DCInfo.GetDefPoint(Obj.Class) + (Obj.Level - 1) * 7;
	}
	else
	{
		ori_point = DCInfo.GetDefPoint(Obj.Class) + (Obj.Level - 1) * 5;
	}

	if (g_QuestInfo.GetQuestState(Obj, 0) == 2)
	{
		ori_point += 10;
	}

	if (g_QuestInfo.GetQuestState(Obj, 1) == 2)
	{
		ori_point += 10;
	}

	if (g_QuestInfo.GetQuestState(Obj, 2) == 2)
	{
		ori_point += Obj.Level - 220;
	}

	//Season3 Quest MaxPoint Calculation
	if (g_QuestInfo.GetQuestState(Obj, 4) == 2)
	{
		ori_point += 20;
	}

	if (g_QuestInfo.GetQuestState(Obj, 5) == 2)
	{
		ori_point += 20;
	}

	if (g_QuestInfo.GetQuestState(Obj, 6) == 2)
	{
		ori_point += 30;
	}

	ori_point += gStatMng.GetMaxStat(Obj.Level, Obj.Class);

	if (total_point < ori_point)
	{
		return true;
	}
	return false;
}




BOOL gObjCheckStatPointDown(CGameObject &Obj)
{
	

	if (Obj.Type != OBJ_USER)
	{
		return FALSE;
	}

	int total_point = Obj.m_PlayerData->Strength + Obj.m_PlayerData->Dexterity + Obj.m_PlayerData->Vitality + Obj.m_PlayerData->Energy + Obj.m_PlayerData->LevelUpPoint;
	int ori_point;

	if (Obj.Class == CLASS_DARKLORD || Obj.Class == CLASS_MAGUMSA || Obj.Class == CLASS_RAGEFIGHTER)
	{
		ori_point = DCInfo.GetDefPoint(Obj.Class) + (Obj.Level - 1) * 7;
	}
	else
	{
		ori_point = DCInfo.GetDefPoint(Obj.Class) + (Obj.Level - 1) * 5;
	}

	if (g_QuestInfo.GetQuestState(Obj, 0) == 2)
	{
		ori_point += 10;
	}

	if (g_QuestInfo.GetQuestState(Obj, 1) == 2)
	{
		ori_point += 10;
	}

	//Aplied Webzen Fix for MaxPoint Calculation
	if (g_QuestInfo.GetQuestState(Obj, 2) == 2)
	{
		ori_point += Obj.Level - 220;
	}

	//Season3 Quest MaxPoint Calculation
	if (g_QuestInfo.GetQuestState(Obj, 4) == 2)
	{
		ori_point += 20;
	}

	if (g_QuestInfo.GetQuestState(Obj, 5) == 2)
	{
		ori_point += 20;
	}

	if (g_QuestInfo.GetQuestState(Obj, 6) == 2)
	{
		ori_point += 30;
	}

	ori_point += gStatMng.GetMaxStat(Obj.Level, Obj.Class);

	if (total_point > ori_point)
	{
		return true;
	}
	return false;
}




void gObjUseCircle(CGameObject &Obj, int pos)
{
	int level = Obj.pntInventory[pos]->m_Level;
	int iSerial = Obj.pntInventory[pos]->m_Number;

	PMSG_DEFRESULT pResult;

	PHeadSetB((BYTE*)&pResult, 0x2C, sizeof(pResult));

	pResult.result = 0;

	if (Obj.Level <= 10 || level > 3)
	{
		pResult.result |= 0xC0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (gObjCheckStatPointUp(Obj) == 0)
	{
		pResult.result |= 0xC0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	sLog->outBasic("[StatUp] [%s][%s] Use StatUpItem Level:%d Pos:%d serial:%I64d", Obj.AccountID, Obj.Name, level, pos, iSerial);

	gObjInventoryItemSet(Obj, pos, 0xFF);
	Obj.pntInventory[pos]->Clear();
	gGameProtocol.GCInventoryItemDeleteSend(Obj, pos, 1);

	short AddPoint;
	short MaxAddPoint;
	short MinusPoint;
	short MaxMinusPoint;

	gObjGetStatPointState(Obj, (short &)AddPoint, (short &)(MaxAddPoint), (short &)MinusPoint, (short &)MaxMinusPoint);

	int iSuccessRate = 0;

	if (AddPoint <= 10)
	{
		iSuccessRate = 100;
	}
	else if ((AddPoint - 10) < MaxAddPoint * (double)0.1)
	{
		iSuccessRate = 90;
	}
	else if ((AddPoint - 10) < MaxAddPoint * (double)0.3)
	{
		iSuccessRate = 80;
	}
	else if ((AddPoint - 10) < MaxAddPoint * (double)0.5)
	{
		iSuccessRate = 70;
	}
	else if ((AddPoint - 10) < MaxAddPoint * (double)0.8)
	{
		iSuccessRate = 60;
	}
	else
	{
		iSuccessRate = 50;
	}

	int incvalue = 0;

	if (rand() % 100 < iSuccessRate)
	{
		iSuccessRate = rand() % 100;

		int AddStat = 0;

		if (iSuccessRate < 70)
		{
			AddStat = 1;
		}
		else if (iSuccessRate < 95)
		{
			AddStat = 2;
		}
		else
		{
			AddStat = 3;
		}

		if ((AddStat + AddPoint) > MaxAddPoint)
		{
			AddStat = 1;
		}

		pResult.result |= level * 16;
		pResult.result |= AddStat;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pResult, pResult.h.size);

		incvalue = AddStat;

		sLog->outBasic("[StatUp] [%s][%s] Success [%d][%d] %d", Obj.AccountID, Obj.Name, level, incvalue, iSerial);
	}
	else
	{
		pResult.result |= 0x40;
		pResult.result |= level * 16;
		pResult.result = pResult.result;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pResult, pResult.h.size);
		sLog->outBasic("[StatUp] [%s][%s] Fail [%d] %d", Obj.AccountID, Obj.Name, level, iSerial);
		return;
	}

	switch (level)
	{
	case 3:
		Obj.m_PlayerData->Strength += incvalue;
		break;
	case 2:
		Obj.m_PlayerData->Dexterity += incvalue;
		break;
	case 1:
		Obj.m_PlayerData->Vitality += incvalue;
		Obj.MaxLife += Obj.VitalityToLife * incvalue;
		gGameProtocol.GCReFillSend(Obj, Obj.MaxLife + Obj.AddLife, 0xFE, 0, Obj.iAddShield + Obj.iMaxShield);
		break;
	case 0:
		Obj.m_PlayerData->Energy += incvalue;
		Obj.MaxMana += Obj.EnergyToMana * incvalue;
		gObjSetBP(Obj);
		gGameProtocol.GCManaSend(Obj, Obj.MaxMana + Obj.AddMana, 0xFE, 0, Obj.MaxBP);
		break;
	default: break;
	}

	gObjCalCharacter.CalcCharacter(Obj);
}

void gObjUsePlusStatFruit(CGameObject &Obj, int pos)
{
	if (Obj.Class > 6 || Obj.Class < 0)
	{
		return;
	}

	int iItemLevel = Obj.pntInventory[pos]->m_Level;
	int iItemSerial = Obj.pntInventory[pos]->m_Number;
	int iCharacterClass = Obj.Class;

	PMSG_USE_STAT_FRUIT pResult;

	PHeadSetB((BYTE*)&pResult, 0x2C, sizeof(pResult));

	pResult.result = 2;
	pResult.btFruitType = iItemLevel;
	pResult.btStatValue = 0;

	if (Obj.Level <= 9 || iItemLevel > 4)
	{
		pResult.result = 2;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pResult, pResult.h.size);
		return;
	}

	if (iItemLevel == 4 && Obj.Class != 4)
	{
		pResult.result = 2;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pResult, pResult.h.size);
		return;
	}

	for (int iItemIndex = 0; iItemIndex < INVETORY_WEAR_SIZE; iItemIndex++)
	{
		if (Obj.pntInventory[iItemIndex]->IsItem())
		{
			if (g_ConfigRead.data.common.joinmuFruitRequireUnWearItem == true)
			{
				pResult.result = 16;
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pResult, pResult.h.size);
				return;
			}
		}
	}

	if (gObjCheckStatPointUp(Obj) == false)
	{
		pResult.result = 33;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pResult, pResult.h.size);
		return;
	}

	sLog->outBasic("[StatUp] [%s][%s] Use StatUpItem Level:%d Pos:%d serial:%I64d",
		Obj.AccountID, Obj.Name, iItemLevel, pos, iItemSerial);

	gObjInventoryItemSet(Obj, pos, 0xFF);
	Obj.pntInventory[pos]->Clear();
	gGameProtocol.GCInventoryItemDeleteSend(Obj, pos, 1);

	short AddPoint;
	short MaxAddPoint;
	short MinusPoint;
	short MaxMinusPoint;

	gObjGetStatPointState(Obj, (short &)AddPoint, (short &)MaxAddPoint, (short &)MinusPoint, (short &)MaxMinusPoint);

	int iSuccessRate = 0;

	if (AddPoint <= 10)
	{
		iSuccessRate = 100;
	}
	else if ((AddPoint - 10) < (MaxAddPoint * (double)0.1))
	{
		iSuccessRate = 90;
	}
	else if ((AddPoint - 10) < (MaxAddPoint * (double)0.3))
	{
		iSuccessRate = 80;
	}
	else if ((AddPoint - 10) < (MaxAddPoint * (double)0.5))
	{
		iSuccessRate = 70;
	}
	else if ((AddPoint - 10) < (MaxAddPoint * (double)0.8))
	{
		iSuccessRate = 60;
	}
	else
	{
		iSuccessRate = 50;
	}

	int iIncStat = 0;
	int bSuccess = 0;

	if (rand() % 100 < iSuccessRate)
	{
		bSuccess = 1;
	}

	if (bSuccess != 0)
	{
		iSuccessRate = rand() % 100;

		int AddStat = 0;

		if (iSuccessRate < 70)
		{
			AddStat = 1;
		}
		else if (iSuccessRate < 95)
		{
			AddStat = 2;
		}
		else
		{
			AddStat = 3;
		}

		if ((AddStat + AddPoint) > MaxAddPoint)
		{
			AddStat = 1;
		}

		pResult.result = 0;
		pResult.btStatValue = AddStat;
		pResult.btFruitType = iItemLevel;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pResult, pResult.h.size);
		iIncStat = AddStat;

		sLog->outBasic("[StatUp] [%s][%s] Success [%d][%d] %d",
			Obj.AccountID, Obj.Name, iItemLevel, iIncStat, iItemSerial);
	}
	else
	{
		pResult.result = 1;
		pResult.btStatValue = 0;
		pResult.btFruitType = iItemLevel;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pResult, pResult.h.size);

		sLog->outBasic("[StatUp] [%s][%s] Fail [%d] %d",
			Obj.AccountID, Obj.Name, iItemLevel, iItemSerial);
		return;
	}

	switch (iItemLevel)
	{
	case 4: Obj.Leadership += iIncStat;	break;
	case 3: Obj.m_PlayerData->Strength += iIncStat;	break;
	case 2: Obj.m_PlayerData->Dexterity += iIncStat;	break;
	case 1:
		Obj.m_PlayerData->Vitality += iIncStat;
		Obj.MaxLife += Obj.VitalityToLife * iIncStat;
		break;
	case 0:
		Obj.m_PlayerData->Energy += iIncStat;
		Obj.MaxMana += Obj.EnergyToMana * iIncStat;
		gObjSetBP(Obj);
		break;
	default: break;
	}

	gObjCalCharacter.CalcCharacter(Obj);
	gGameProtocol.GCReFillSend(Obj, Obj.MaxLife + Obj.AddLife, 0xFE, 0, Obj.iAddShield + Obj.iMaxShield);
	gObjSetBP(Obj);
	gGameProtocol.GCManaSend(Obj, Obj.MaxMana + Obj.AddMana, 0xFE, 0, Obj.MaxBP + Obj.AddBP);
}


void gObjUseMinusStatFruit(CGameObject &Obj, int pos)
{
	if (Obj.Class > 6 || Obj.Class < 0)
	{
		return;
	}

	int iItemLevel = Obj.pntInventory[pos]->m_Level;
	int iItemSerial = Obj.pntInventory[pos]->m_Number;
	int iCharacterClass = Obj.Class;

	PMSG_USE_STAT_FRUIT pResult;

	PHeadSetB((BYTE*)&pResult, 0x2C, sizeof(pResult));

	pResult.result = 5;
	pResult.btFruitType = iItemLevel;
	pResult.btStatValue = 0;

	if (Obj.Level <= 9 || iItemLevel > 4)
	{
		pResult.result = 5;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pResult, pResult.h.size);
		return;
	}

	if (iItemLevel == 4 && Obj.Class != 4)
	{
		pResult.result = 5;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pResult, pResult.h.size);
		return;
	}

	for (int iItemIndex = 0; iItemIndex < INVETORY_WEAR_SIZE; iItemIndex++)
	{
		if (Obj.pntInventory[iItemIndex]->IsItem())
		{
			if (g_ConfigRead.data.common.joinmuFruitRequireUnWearItem == true)
			{
				pResult.result = 16;
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pResult, pResult.h.size);
				return;
			}
		}
	}

	int iDefaultStat = 0;
	int iPresentStat = 0;
	int bEnableUseFruit = 1;

	if (iItemLevel == 0)
	{
		iDefaultStat = DCInfo.DefClass[iCharacterClass].Energy;
		iPresentStat = Obj.m_PlayerData->Energy;

		if (iPresentStat <= iDefaultStat)
		{
			bEnableUseFruit = 0;
		}
	}
	else if (iItemLevel == 1)
	{
		iDefaultStat = DCInfo.DefClass[iCharacterClass].Vitality;
		iPresentStat = Obj.m_PlayerData->Vitality;

		if (iPresentStat <= iDefaultStat)
		{
			bEnableUseFruit = 0;
		}
	}
	else if (iItemLevel == 2)
	{
		iDefaultStat = DCInfo.DefClass[iCharacterClass].Dexterity;
		iPresentStat = Obj.m_PlayerData->Dexterity;

		if (iPresentStat <= iDefaultStat)
		{
			bEnableUseFruit = 0;
		}
	}
	else if (iItemLevel == 3)
	{
		iDefaultStat = DCInfo.DefClass[iCharacterClass].Strength;
		iPresentStat = Obj.m_PlayerData->Strength;

		if (iPresentStat <= iDefaultStat)
		{
			bEnableUseFruit = 0;
		}
	}
	else if (iItemLevel == 4)
	{
		iDefaultStat = DCInfo.DefClass[iCharacterClass].Leadership;
		iPresentStat = Obj.Leadership;

		if (iPresentStat <= iDefaultStat)
		{
			bEnableUseFruit = 0;
		}
	}

	if (bEnableUseFruit == 0)
	{
		pResult.result = 38;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pResult, pResult.h.size);
		return;
	}

	short AddPoint;
	short MaxAddPoint;
	short MinusPoint;
	short MaxMinusPoint = 0;

	gObjGetStatPointState(Obj, (short &)AddPoint, (short &)MaxAddPoint, (short &)MinusPoint, (short &)MaxMinusPoint);

	if (MinusPoint >= MaxMinusPoint || MinusPoint < 0)
	{
		pResult.result = 37;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pResult, pResult.h.size);
		return;
	}

	sLog->outBasic("[StatDown] [%s][%s] Use StatDownItem Level:%d Pos:%d serial:%I64d",
		Obj.AccountID, Obj.Name, iItemLevel, pos, iItemSerial);

	gObjInventoryItemSet(Obj, pos, 0xFF);
	Obj.pntInventory[pos]->Clear();
	gGameProtocol.GCInventoryItemDeleteSend(Obj, pos, 1);

	int iDecStat = 0;
	int bSuccess = 0;

	int iSuccessRate = 100 - Obj.Level / 6;

	if (iSuccessRate < 0)
	{
		iSuccessRate = 0;
	}

	if (rand() % 100 < iSuccessRate)
	{
		bSuccess = 1;
	}

	if (MinusPoint <= 10)
	{
		bSuccess = 1;
	}

	if (bSuccess != 0)
	{
		iSuccessRate = rand() % 100;

		if (iSuccessRate < 50)
		{
			iDecStat = 1;
		}
		else if (iSuccessRate < 75)
		{
			iDecStat = 3;
		}
		else if (iSuccessRate < 91)
		{
			iDecStat = 5;
		}
		else if (iSuccessRate < 98)
		{
			iDecStat = 7;
		}
		else
		{
			iDecStat = 9;
		}

		if ((MinusPoint + iDecStat) >= MaxMinusPoint)
		{
			iDecStat = MaxMinusPoint - MinusPoint;
		}

		if ((iPresentStat - iDecStat) < iDefaultStat)
		{
			iDecStat = iPresentStat - iDefaultStat;
		}

		pResult.result = 3;
		pResult.btFruitType = iItemLevel;
		pResult.btStatValue = iDecStat;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pResult, pResult.h.size);
		sLog->outBasic("[StatDown] [%s][%s] Success [%d][%d] %d",
			Obj.AccountID, Obj.Name, iItemLevel, iDecStat, iItemSerial);
	}
	else
	{
		pResult.result = 4;
		pResult.btFruitType = iItemLevel;
		pResult.btStatValue = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pResult, pResult.h.size);
		sLog->outBasic("[StatDown] [%s][%s] Fail [%d] %d",
			Obj.AccountID, Obj.Name, iItemLevel, iItemSerial);
		return;
	}

	switch (iItemLevel)
	{
	case 4:	Obj.Leadership -= iDecStat;	break;
	case 3: Obj.m_PlayerData->Strength -= iDecStat;	break;
	case 2: Obj.m_PlayerData->Dexterity -= iDecStat;	break;
	case 1:

		Obj.m_PlayerData->Vitality -= iDecStat;
		Obj.MaxLife -= Obj.VitalityToLife *iDecStat;
		break;

	case 0:
		Obj.m_PlayerData->Energy -= iDecStat;
		Obj.MaxMana -= Obj.EnergyToMana * iDecStat;
		gObjSetBP(Obj);
		break;

	default:
		break;
	}

	int iOldLevelUpPoint = Obj.m_PlayerData->LevelUpPoint;
	Obj.m_PlayerData->LevelUpPoint += iDecStat;

	int iOldFruitPoint = Obj.m_PlayerData->LevelUpPoint;
	Obj.m_PlayerData->iFruitPoint += iDecStat;

	sLog->outBasic("[StatDown] [%s][%s] MinusStat[%d] -> LevelUpPoint Old(%d)/New(%d)  FruitPoint Old(%d)/New(%d)",
		Obj.AccountID, Obj.Name, iDecStat, iOldLevelUpPoint, Obj.m_PlayerData->LevelUpPoint, iOldFruitPoint, Obj.m_PlayerData->iFruitPoint);

	gObjCalCharacter.CalcCharacter(Obj);
	gGameProtocol.GCReFillSend(Obj, Obj.MaxLife + Obj.AddLife, 0xFE, 0, Obj.iAddShield + Obj.iMaxShield);
	gObjSetBP(Obj);
	gGameProtocol.GCManaSend(Obj, Obj.MaxMana + Obj.AddMana, 0xFE, 0, Obj.MaxBP);
}

void CashShopExMinusStatFruit(CGameObject &Obj, int pos) //GS-CS Need Decompile
{
	if (Obj.Class > 6 || Obj.Class < 0)
	{
		return;
	}

	int iItemLevel = 0;

	switch (Obj.pntInventory[pos]->m_Type)
	{
	case 6710:
		iItemLevel = 3;
		break;
	case 6711:
		iItemLevel = 2;
		break;
	case 6712:
		iItemLevel = 1;
		break;
	case 6713:
		iItemLevel = 0;
		break;
	case 6714:
		iItemLevel = 4;
		break;
	}

	int iItemSerial = Obj.pntInventory[pos]->m_Number;
	int iCharacterClass = Obj.Class;
	int iStatPoint = Obj.pntInventory[pos]->m_Durability * 10.0;

	PMSG_USE_STAT_FRUIT pResult;
	PHeadSetB((BYTE*)&pResult, 0x2C, sizeof(pResult));

	pResult.result = 8;
	pResult.btFruitType = iItemLevel;
	pResult.btStatValue = 0;

	for (int iItemIndex = 0; iItemIndex < INVETORY_WEAR_SIZE; iItemIndex++)
	{
		if (Obj.pntInventory[iItemIndex]->IsItem())
		{
			if (g_ConfigRead.data.common.joinmuFruitRequireUnWearItem == true)
			{
				pResult.result = 16;
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pResult, pResult.h.size);
				return;
			}
		}
	}

	bool bEnableFruit = true;

	switch (pResult.btFruitType)
	{
	case 3:
		if (Obj.m_PlayerData->Strength == DCInfo.DefClass[iCharacterClass].Strength)
		{
			bEnableFruit = false;
		}
		break;
	case 2:
		if (Obj.m_PlayerData->Dexterity == DCInfo.DefClass[iCharacterClass].Dexterity)
		{
			bEnableFruit = false;
		}
		break;
	case 1:
		if (Obj.m_PlayerData->Vitality == DCInfo.DefClass[iCharacterClass].Vitality)
		{
			bEnableFruit = false;
		}
		break;
	case 0:
		if (Obj.m_PlayerData->Energy == DCInfo.DefClass[iCharacterClass].Energy)
		{
			bEnableFruit = false;
		}
		break;
	case 5:
		if (Obj.Leadership = DCInfo.DefClass[iCharacterClass].Leadership)
		{
			bEnableFruit = false;
		}
		break;
	}

	int iDefaultStat, iPresentStat;

	switch (pResult.btFruitType)
	{
	case 3:
		iDefaultStat = DCInfo.DefClass[iCharacterClass].Strength;
		iPresentStat = Obj.m_PlayerData->Strength;
		if (iPresentStat - iStatPoint < iDefaultStat)
			iStatPoint = iPresentStat - iDefaultStat;
		break;
	case 2:
		iDefaultStat = DCInfo.DefClass[iCharacterClass].Dexterity;
		iPresentStat = Obj.m_PlayerData->Dexterity;
		if (iPresentStat - iStatPoint < iDefaultStat)
			iStatPoint = iPresentStat - iDefaultStat;
		break;
	case 1:
		iDefaultStat = DCInfo.DefClass[iCharacterClass].Vitality;
		iPresentStat = Obj.m_PlayerData->Vitality;
		if (iPresentStat - iStatPoint < iDefaultStat)
			iStatPoint = iPresentStat - iDefaultStat;
		break;
	case 0:
		iDefaultStat = DCInfo.DefClass[iCharacterClass].Energy;
		iPresentStat = Obj.m_PlayerData->Energy;
		if (iPresentStat - iStatPoint < iDefaultStat)
			iStatPoint = iPresentStat - iDefaultStat;
		break;
	case 4:
		iDefaultStat = DCInfo.DefClass[iCharacterClass].Leadership;
		iPresentStat = Obj.Leadership;
		if (iPresentStat - iStatPoint < iDefaultStat)
			iStatPoint = iPresentStat - iDefaultStat;
		break;
	}

	if (bEnableFruit)
	{
		sLog->outBasic("[InGameShop][ExMinusStatPoint] [%s][%s] Use StatDownItem Level:%d,Pos:%d,Dur:%d,Serial:%I64d",
			Obj.AccountID, Obj.Name, Obj.pntInventory[pos]->m_Level,
			pos, Obj.pntInventory[pos]->m_Durability, iItemSerial);

		gObjInventoryItemSet(Obj, pos, 0xFF);
		Obj.pntInventory[pos]->Clear();
		gGameProtocol.GCInventoryItemDeleteSend(Obj, pos, 1);

		pResult.result = 6;
		pResult.btFruitType = iItemLevel;
		pResult.btStatValue = iStatPoint;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		switch (iItemLevel)
		{
		case 4:	Obj.Leadership -= iStatPoint;	break;
		case 3: Obj.m_PlayerData->Strength -= iStatPoint;	break;
		case 2: Obj.m_PlayerData->Dexterity -= iStatPoint;	break;
		case 1:
			Obj.m_PlayerData->Vitality -= iStatPoint;
			Obj.MaxLife -= Obj.VitalityToLife *iStatPoint;
			break;

		case 0:
			Obj.m_PlayerData->Energy -= iStatPoint;
			Obj.MaxMana -= Obj.EnergyToMana * iStatPoint;
			gObjSetBP(Obj);
			break;
		}
		Obj.m_PlayerData->LevelUpPoint += iStatPoint;
		gObjCalCharacter.CalcCharacter(Obj);
	}
	else
	{
		pResult.result = 38;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		sLog->outBasic("[InGameShop][ExMinusStatPoint] [%s][%s] MinusStat[%d]",
			Obj.AccountID, Obj.Name, iStatPoint);

		gObjCalCharacter.CalcCharacter(Obj);
		gGameProtocol.GCReFillSend(Obj, Obj.MaxLife + Obj.AddLife, 0xFE, 0, Obj.iAddShield + Obj.iMaxShield);
		gObjSetBP(Obj);
		gGameProtocol.GCManaSend(Obj, Obj.MaxMana + Obj.AddMana, 0xFE, 0, Obj.MaxBP);
	}
}



void gObjCalcMaxLifePower(CGameObject &Obj)
{
	

	switch (Obj.Class)
	{
	case CLASS_WIZARD:
		Obj.m_MaxLifePower = Obj.Level + Obj.m_PlayerData->Vitality + 110;
		break;
	case CLASS_KNIGHT:
		Obj.m_MaxLifePower = Obj.Level * 2 + Obj.m_PlayerData->Vitality + 60;
		break;
	case CLASS_ELF:
		Obj.m_MaxLifePower = Obj.Level + Obj.m_PlayerData->Vitality + 80;
		break;
	case CLASS_MAGUMSA:
		Obj.m_MaxLifePower = Obj.Level + Obj.m_PlayerData->Vitality + 110;
		break;
	case CLASS_DARKLORD:
		Obj.m_MaxLifePower = Obj.Level + Obj.m_PlayerData->Vitality + 110;
		break;
	case CLASS_SUMMONER:
		Obj.m_MaxLifePower = Obj.Level + Obj.m_PlayerData->Vitality + 110;
		break;
	case CLASS_RAGEFIGHTER:
		Obj.m_MaxLifePower = Obj.Level + Obj.m_PlayerData->Vitality + 110;
		break;
	case CLASS_GROWLANCER:
		Obj.m_MaxLifePower = Obj.Level + Obj.m_PlayerData->Vitality + 110;
		break;
	default:
		Obj.m_MaxLifePower = 0;
		break;
	}
}




void gObjDelayLifeCheck(CGameObject &Obj)
{
	

	if (Obj.m_CheckLifeTime > 0)
	{
		Obj.m_CheckLifeTime--;

		if (Obj.m_CheckLifeTime <= 0)
		{
			Obj.m_CheckLifeTime = 0;

			if (Obj.Life < 0)
			{
				Obj.Life = 0;
			}

			if (Obj.lpAttackObj != 0)
			{
				gObjLifeCheck(Obj, *Obj.lpAttackObj, 0, 1, 0, 0, 0, 0, 0);
			}
		}
	}
}

BOOL gObjDuelCheck(CGameObject &Obj)
{
	BOOL bRetVal = FALSE;
	if (Obj.Type == OBJ_USER)
	{
		int iDuelIndex = Obj.m_iDuelUser;
		if (CHECK_LIMIT(iDuelIndex, g_ConfigRead.server.GetObjectMax()))
		{
			bRetVal = TRUE;
		}
	}
	return bRetVal;
}

BOOL gObjDuelCheck(CGameObject &Obj, CGameObject &lpTargetObj)
{
	BOOL bRetVal = FALSE;
	if (Obj.Type == OBJ_USER && lpTargetObj.Type == OBJ_USER)
	{
		if (Obj.m_iDuelUser == lpTargetObj.m_Index && lpTargetObj.m_iDuelUser == Obj.m_Index)
		{
			bRetVal = TRUE;
		}
	}
	return bRetVal;
}

void gObjResetDuel(CGameObject &Obj)
{
	int iDuelIndex = Obj.m_iDuelUser;
	CGameObject* lpObj = getGameObject(iDuelIndex);
	if (ObjectMaxRange(iDuelIndex))
	{
		Obj.m_iDuelUser = -1;
		Obj.m_iDuelUserReserved = -1;
		Obj.m_btDuelScore = 0;
		Obj.m_iDuelTickCount = 0;

		if (Obj.Type == OBJ_USER && Obj.m_PlayerData->ISBOT == false && Obj.m_Index >= g_ConfigRead.server.GetObjectStartUserIndex())
		{
			gDarkSpirit[Obj.m_Index - g_ConfigRead.server.GetObjectStartUserIndex()].ReSetTarget(*lpObj);
			gDarkSpirit[iDuelIndex - g_ConfigRead.server.GetObjectStartUserIndex()].ReSetTarget(Obj);
			g_CMuunSystem.ReSetTarget(Obj, *lpObj);
			g_CMuunSystem.ReSetTarget(*lpObj, Obj);
		}

		lpObj->m_iDuelUser = -1;
		lpObj->m_iDuelUserReserved = -1;
		lpObj->m_iDuelUserRequested = -1;
		lpObj->m_iDuelTickCount = 0;
	}
}

void SkillFrustrum(BYTE bangle, CGameObject &Obj)
{
	

	vec3_t p[4];
	Vector(-6, 6, 0, p[0]);
	Vector(6, 6, 0, p[1]);
	Vector(1, 0, 0, p[2]);
	Vector(-1, 0, 0, p[3]);

	vec3_t Angle;

	Vector(0.f, 0.f, (bangle * 360 / 255), Angle);

	float Matrix[3][4];

	AngleMatrix(Angle, Matrix);

	vec3_t vFrustrum[4];

	for (int i = 0; i < 4; i++)
	{
		VectorRotate(p[i], Matrix, vFrustrum[i]);

		Obj.fSkillFrustrumX[i] = (int)vFrustrum[i][0] + Obj.X;
		Obj.fSkillFrustrumY[i] = (int)vFrustrum[i][1] + Obj.Y;
	}
}




BOOL SkillTestFrustrum(int x, int y, CGameObject &Obj)
{
	

	int j = 3;

	if (Obj.m_bOffLevel)
		return true;

	for (int i = 0; i < MAX_ARRAY_FRUSTRUM; j = i, i++)
	{
		if ((Obj.fSkillFrustrumX[i] - x) * (Obj.fSkillFrustrumY[j] - y) - (Obj.fSkillFrustrumX[j] - x) * (Obj.fSkillFrustrumY[i] - y) < 0.0f)
		{
			return false;
		}
	}
	return true;
}



BOOL gObjCheckMaxZen(CGameObject &Obj, int nAddZen)
{
	__int64 _Money = 0;
	_Money = (__int64)Obj.m_PlayerData->Money + (__int64)nAddZen;

	if (_Money > (__int64)MAX_ZEN)
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 142), Obj, 1);
		return FALSE;
	}

	return TRUE;
}




void MakeRandomSetItem(CGameObject &Obj)
{
	int itemnum = gSetItemOption.GenRandomItemNum();

	int SetOption = gSetItemOption.GenSetOption(itemnum);

	int option1rand;
	int option2rand;
	int option3rand;
	int optionc;
	int Option1 = 0;
	int Option2 = 0;
	int Option3 = 0;

	option1rand = 6;
	option2rand = 4;

	option3rand = rand() % 100;
	optionc = rand() % 3;

	if (rand() % 100 < option2rand)
	{
		Option2 = 1;
	}

	switch (optionc)
	{
	case 0:
		if (option3rand < 4)
		{
			Option3 = 3;
		}
		break;
	case 1:
		if (option3rand < 8)
		{
			Option3 = 2;
		}
		break;
	case 2:
		if (option3rand < 12)
		{
			Option3 = 1;
		}
		break;
	default: break;
	}

	Option1 = 1;

	ItemCreate(Obj, Obj.MapNumber, Obj.X, Obj.Y, itemnum, 0, 0, Option1, Option2, Option3, Obj.m_Index, 0, SetOption, 0, 0, 0);
}

void MakeRandomSetItem(CGameObject &Obj, _stGremoryCaseItem & stItem)
{
	int itemnum = gSetItemOption.GenRandomItemNum();

	int SetOption = gSetItemOption.GenSetOption(itemnum);

	int option1rand;
	int option2rand;
	int option3rand;
	int optionc;
	int Option1 = 0;
	int Option2 = 0;
	int Option3 = 0;

	option1rand = 6;
	option2rand = 4;

	option3rand = rand() % 100;
	optionc = rand() % 3;

	if (rand() % 100 < option2rand)
	{
		Option2 = 1;
	}

	switch (optionc)
	{
	case 0:
		if (option3rand < 4)
		{
			Option3 = 3;
		}
		break;
	case 1:
		if (option3rand < 8)
		{
			Option3 = 2;
		}
		break;
	case 2:
		if (option3rand < 12)
		{
			Option3 = 1;
		}
		break;
	default: break;
	}

	Option1 = 1;

	stItem.wItemID = itemnum;
	stItem.btItemLevel = 0;
	stItem.btItemDurability = 0;
	stItem.btItemSkill = Option1;
	stItem.btItemLuck = Option2;
	stItem.btItemOption = Option3;
	stItem.btItemExcOption = 0;
	stItem.btItemSetOption = SetOption;
}



void MakeRewardSetItem(CGameObject &Obj, BYTE cDropX, BYTE cDropY, int iRewardType, int iMapnumber)
{
	int itemnum = gSetItemOption.GenRandomItemNum();

	int SetOption = gSetItemOption.GenSetOption(itemnum);

	int option1rand;
	int option2rand;
	int option3rand;
	int optionc;
	int Option1 = 0;
	int Option2 = 0;
	int Option3 = 0;

	option1rand = 6;
	option2rand = 4;

	option3rand = rand() % 100;
	optionc = rand() % 3;

	if (rand() % 100 < option2rand)
	{
		Option2 = 1;
	}

	switch (optionc)
	{
	case 0:
		if (option3rand < 4)
		{
			Option3 = 3;
		}
		break;
	case 1:
		if (option3rand < 8)
		{
			Option3 = 2;
		}
		break;
	case 2:
		if (option3rand < 12)
		{
			Option3 = 1;
		}
		break;
	default: break;
	}

	Option1 = 1;

	if (cDropX == 0 && cDropY == 0)
	{
		cDropX = Obj.X;
		cDropY = Obj.Y;
	}

	ItemCreate(Obj, iMapnumber, cDropX, cDropY, itemnum, 0, 0, Option1, Option2, Option3, Obj.m_Index, 0, SetOption, 0, 0, 0);

	if (iRewardType == 1)
	{
		sLog->outBasic("[Reward][KUNDUN] [%s][%s] Set Item itemnum:[%d] skill:[%d] luck:[%d] option:[%d] SetOption:[%d]",
			Obj.AccountID, Obj.Name, itemnum, Option1, Option2, Option3, SetOption);
	}
	else
	{
		sLog->outBasic("[Reward][Etc] [%s][%s] Set Item itemnum:[%d] skill:[%d] luck:[%d] option:[%d] SetOption:[%d]",
			Obj.AccountID, Obj.Name, itemnum, Option1, Option2, Option3, SetOption);
	}
}

void MakeRewardSetItem(CGameObject &Obj, BYTE cDropX, BYTE cDropY, int iRewardType, int iMapnumber, _stGremoryCaseItem & stItem)
{
	int itemnum = gSetItemOption.GenRandomItemNum();

	int SetOption = gSetItemOption.GenSetOption(itemnum);

	int option1rand;
	int option2rand;
	int option3rand;
	int optionc;
	int Option1 = 0;
	int Option2 = 0;
	int Option3 = 0;

	option1rand = 6;
	option2rand = 4;

	option3rand = rand() % 100;
	optionc = rand() % 3;

	if (rand() % 100 < option2rand)
	{
		Option2 = 1;
	}

	switch (optionc)
	{
	case 0:
		if (option3rand < 4)
		{
			Option3 = 3;
		}
		break;
	case 1:
		if (option3rand < 8)
		{
			Option3 = 2;
		}
		break;
	case 2:
		if (option3rand < 12)
		{
			Option3 = 1;
		}
		break;
	default: break;
	}

	Option1 = 1;

	if (cDropX == 0 && cDropY == 0)
	{
		cDropX = Obj.X;
		cDropY = Obj.Y;
	}

	stItem.wItemID = itemnum;
	stItem.btItemLevel = 0;
	stItem.btItemDurability = 0;
	stItem.btItemSkill = Option1;
	stItem.btItemLuck = Option2;
	stItem.btItemOption = Option3;
	stItem.btItemExcOption = 0;
	stItem.btItemSetOption = SetOption;

	if (iRewardType == 1)
	{
		sLog->outBasic("[Reward][KUNDUN] [%s][%s] Set Item itemnum:[%d] skill:[%d] luck:[%d] option:[%d] SetOption:[%d]",
			Obj.AccountID, Obj.Name, itemnum, Option1, Option2, Option3, SetOption);
	}
	else
	{
		sLog->outBasic("[Reward][Etc] [%s][%s] Set Item itemnum:[%d] skill:[%d] luck:[%d] option:[%d] SetOption:[%d]",
			Obj.AccountID, Obj.Name, itemnum, Option1, Option2, Option3, SetOption);
	}
}



void gObjRecall(CGameObject &Obj, int mapnumber, int x, int y)
{
	if (Obj.MapNumber != mapnumber)
	{
		Obj.X = x;
		Obj.Y = y;

		Obj.TX = x;
		Obj.TY = y;

		Obj.MapNumber = mapnumber;

		Obj.PathCount = 0;
		Obj.Teleport = 0;

		gObjClearViewport(Obj);
		gGameProtocol.GCTeleportSend(Obj, -1, mapnumber, Obj.X, Obj.Y, Obj.Dir);

		if (Obj.m_Change >= 0)
		{
			gObjViewportListProtocolCreate(Obj);
		}

		Obj.RegenMapNumber = mapnumber;
		Obj.RegenMapX = x;
		Obj.RegenMapY = y;
		Obj.RegenOk = 1;
	}
	else
	{
		PMSG_MAGICATTACK_RESULT pAttack;

		PHeadSetBE((BYTE *)&pAttack, 0x19, sizeof(pAttack));

		pAttack.MagicNumberH = SET_NUMBERH(AT_SKILL_TELEPORT);
		pAttack.MagicNumberL = SET_NUMBERL(AT_SKILL_TELEPORT);
		pAttack.SourceNumberH = SET_NUMBERH(Obj.m_Index);
		pAttack.SourceNumberL = SET_NUMBERL(Obj.m_Index);
		pAttack.TargetNumberH = SET_NUMBERH(Obj.TargetNumber);
		pAttack.TargetNumberL = SET_NUMBERL(Obj.TargetNumber);

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pAttack, pAttack.h.size);
		gGameProtocol.MsgSendV2(Obj, (BYTE*)&pAttack, pAttack.h.size);

		gObjTeleportMagicUse(Obj, x, y);
	}
}




void gObjSetExpPetItem(CGameObject &Obj, UINT64 exp)
{
	

	if (Obj.Class != 4)
	{
		return;
	}

	exp *= gDarkSpiritAddExperience;

	if (Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 4) && Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 5))
	{
		UINT64 addexp = exp * 10 / 100;

		if (Obj.pntInventory[Obj.m_btInvenPetPos]->AddPetItemExp(addexp))
		{
			if (gObjUseInvenPetCheck(Obj, Obj.pntInventory[Obj.m_btInvenPetPos], 1) == 0)
			{
				Obj.pntInventory[Obj.m_btInvenPetPos]->PetItemLevelDown(addexp);
				MsgOutput(Obj, Lang.GetText(0, 137));
			}
			else
			{
				sLog->outBasic("[%s][%s][PetItemLevelUp] [%s] Level:[%d]Exp:[%I64d]AddExp:[%I64d]",
					Obj.AccountID, Obj.Name, Obj.pntInventory[Obj.m_btInvenPetPos]->GetName(), Obj.pntInventory[Obj.m_btInvenPetPos]->m_PetItem_Level, Obj.pntInventory[Obj.m_btInvenPetPos]->m_PetItem_Exp, addexp);
				CDarkSpirit::SendLevelmsg(Obj, Obj.m_btInvenPetPos, 0, 0xFE);
				gObjCalCharacter.CalcCharacter(Obj);
			}
		}

		if (Obj.pntInventory[Obj.m_btInvenPetPos]->AddPetItemExp(addexp))
		{
			if (gObjUseInvenPetCheck(Obj, Obj.pntInventory[Obj.m_btInvenPetPos], 1) == 0)
			{
				Obj.pntInventory[Obj.m_btInvenPetPos]->PetItemLevelDown(addexp);
				MsgOutput(Obj, Lang.GetText(0, 138));
			}
			else
			{
				sLog->outBasic("[%s][%s][PetItemLevelUp] [%s] Level:[%d]Exp:[%I64d]AddExp:[%I64d]",
					Obj.AccountID, Obj.Name, Obj.pntInventory[Obj.m_btInvenPetPos]->GetName(), Obj.pntInventory[Obj.m_btInvenPetPos]->m_PetItem_Level, Obj.pntInventory[Obj.m_btInvenPetPos]->m_PetItem_Exp, addexp);
				CDarkSpirit::SendLevelmsg(Obj, Obj.m_btInvenPetPos, 1, 0xFE);
				gObjCalCharacter.CalcCharacter(Obj);
			}
		}
	}
	else if (Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 4))
	{
		UINT64 addexp = exp * 20 / 100;

		if (Obj.pntInventory[Obj.m_btInvenPetPos]->AddPetItemExp(addexp))
		{
			if (gObjUseInvenPetCheck(Obj, Obj.pntInventory[Obj.m_btInvenPetPos], 1) == 0)
			{
				Obj.pntInventory[Obj.m_btInvenPetPos]->PetItemLevelDown(addexp);
				MsgOutput(Obj, Lang.GetText(0, 138));
			}
			else
			{
				CDarkSpirit::SendLevelmsg(Obj, Obj.m_btInvenPetPos, 1, 0xFE);
				gObjCalCharacter.CalcCharacter(Obj);
			}
		}
	}
	else if (Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 5))
	{
		UINT64 addexp = exp * 20 / 100;

		if (Obj.pntInventory[Obj.m_btInvenPetPos]->AddPetItemExp(addexp))
		{
			if (gObjUseInvenPetCheck(Obj, Obj.pntInventory[Obj.m_btInvenPetPos], 1) == 0)
			{
				Obj.pntInventory[Obj.m_btInvenPetPos]->PetItemLevelDown(addexp);
				MsgOutput(Obj, Lang.GetText(0, 137));
			}
			else
			{
				sLog->outBasic("[%s][%s][PetItemLevelUp] [%s] Level:[%d]Exp:[%I64d]AddExp:[%I64d]",
					Obj.AccountID, Obj.Name, Obj.pntInventory[Obj.m_btInvenPetPos]->GetName(), Obj.pntInventory[Obj.m_btInvenPetPos]->m_PetItem_Level, Obj.pntInventory[Obj.m_btInvenPetPos]->m_PetItem_Exp, addexp);
				CDarkSpirit::SendLevelmsg(Obj, Obj.m_btInvenPetPos, 0, 0xFE);
				gObjCalCharacter.CalcCharacter(Obj);
			}
		}
	}
}




BOOL gObjGetRandomItemDropLocation(int iMapNumber, BYTE& cX, BYTE& cY, int iRangeX, int iRangeY, int iLoopCount)
{
	int iUX = cX;
	int iUY = cY;

	if (iRangeX <= 0)
	{
		iRangeX = 1;
	}

	if (iRangeY <= 0)
	{
		iRangeY = 1;
	}

	if (iLoopCount <= 0)
	{
		iLoopCount = 1;
	}

	while (iLoopCount-- > 0)
	{
		cX = (rand() % (iRangeX + 1)) * (((rand() % 2 == 0) ? -1 : 1)) + iUX;
		cY = (rand() % (iRangeY + 1)) * (((rand() % 2 == 0) ? -1 : 1)) + iUY;

		BYTE attr = MapC[iMapNumber].GetAttr(cX, cY);

		if ((attr & 4) != 4 && (attr & 8) != 8)
		{
			return TRUE;
		}
	}

	return FALSE;
}





BOOL gObjGetRandomFreeLocation(int iMapNumber, BYTE &cX, BYTE & cY, int iRangeX, int iRangeY, int iLoopCount)
{
	int iUX = cX;
	int iUY = cY;

	if (iRangeX <= 0)
	{
		iRangeX = 1;
	}

	if (iRangeY <= 0)
	{
		iRangeY = 1;
	}

	if (iLoopCount <= 0)
	{
		iLoopCount = 1;
	}

	while (iLoopCount-- > 0)
	{
		cX = (rand() % (iRangeX + 1)) * (((rand() % 2 == 0) ? -1 : 1)) + iUX;
		cY = (rand() % (iRangeY + 1)) * (((rand() % 2 == 0) ? -1 : 1)) + iUY;

		BYTE attr = MapC[iMapNumber].GetAttr(cX, cY);

		if (attr == 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}





int gObjCheckAttackTypeMagic(int iClass, int iSkill)
{
	if (iSkill == 0)
	{
		return 0;
	}

	return MagicDamageC.GetSkillType(iSkill);
}





int gObjGetGuildUnionNumber(CGameObject &Obj)
{
	int iUnion = 0;

	if (Obj.m_PlayerData->lpGuild != NULL)
	{
		iUnion = (Obj.m_PlayerData->lpGuild->iGuildUnion == 0) ? Obj.m_PlayerData->lpGuild->Number : Obj.m_PlayerData->lpGuild->iGuildUnion;
	}

	return iUnion;

}




void gObjGetGuildUnionName(CGameObject &Obj, char* szUnionName, int iUnionNameLen)
{
	szUnionName[0] = 0;

	if (Obj.m_PlayerData->lpGuild == NULL)
	{
		return;
	}

	if (Obj.m_PlayerData->lpGuild->iGuildUnion == 0)
	{
		return;
	}

	TUnionInfo * pUnionInfo = UnionManager.SearchUnion(Obj.m_PlayerData->lpGuild->iGuildUnion);

	if (pUnionInfo != NULL)
	{
		std::memcpy(szUnionName, pUnionInfo->m_szMasterGuild, iUnionNameLen);
	}
}




BOOL gObjCheckRival(CGameObject &Obj, CGameObject lpTargetObj)
{
	if (lpTargetObj.Type != OBJ_USER)
	{
		return false;
	}

	if (Obj.Type != OBJ_USER)
	{
		return false;
	}

	if (lpTargetObj.m_PlayerData->lpGuild == 0 || Obj.m_PlayerData->lpGuild == 0)
	{
		return false;
	}

	if (lpTargetObj.m_PlayerData->lpGuild->iGuildRival == 0 || Obj.m_PlayerData->lpGuild->iGuildRival == 0)
	{
		return false;
	}

	int iUnion = !Obj.m_PlayerData->lpGuild->iGuildUnion ? Obj.m_PlayerData->lpGuild->Number : Obj.m_PlayerData->lpGuild->iGuildUnion;

	if (UnionManager.GetGuildRelationShip(iUnion, lpTargetObj.m_PlayerData->lpGuild->Number) == 2)
	{
		return true;
	}
	return false;
}




int gObjGetRelationShip(CGameObject &Obj, CGameObject &TargetObj)
{
	if (Obj.m_PlayerData->lpGuild == NULL || TargetObj.m_PlayerData->lpGuild == NULL)
	{
		return 0;
	}

	int iUnion = (!Obj.m_PlayerData->lpGuild->iGuildUnion) ? Obj.m_PlayerData->lpGuild->Number : Obj.m_PlayerData->lpGuild->iGuildUnion;

	return UnionManager.GetGuildRelationShip(iUnion, TargetObj.m_PlayerData->lpGuild->Number);
}

struct PMSG_UNION_VIEWPORT_NOTIFY_COUNT {

	// static data ------------------------------------

	// non-static data --------------------------------
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PWMSG_HEAD h;
	/*<thisrel this+0x4>*/ /*|0x1|*/ BYTE btCount;

	// base classes -----------------------------------

	// friends ----------------------------------------

	// static functions -------------------------------

	// non-virtual functions --------------------------

	// virtual functions ------------------------------
};
// <size 0x5>

struct PMSG_UNION_VIEWPORT_NOTIFY {

	// static data ------------------------------------

	// non-static data --------------------------------
	/*<thisrel this+0x0>*/ /*|0x1|*/ BYTE btNumberH;
	/*<thisrel this+0x1>*/ /*|0x1|*/ BYTE btNumberL;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iGuildNumber;
	/*<thisrel this+0x8>*/ /*|0x1|*/ BYTE btGuildRelationShip;
	/*<thisrel this+0x9>*/ /*|0x8|*/ char szUnionName[8];

	// base classes -----------------------------------

	// friends ----------------------------------------

	// static functions -------------------------------

	// non-virtual functions --------------------------

	// virtual functions ------------------------------
};
// <size 0x14>

void gObjNotifyUpdateUnionV1(CGameObject &Obj)
{
	char cBUFFER_V1[6000];
	int iVp1Count = 0;

	memset(cBUFFER_V1, 0x00, sizeof(cBUFFER_V1));

	PMSG_UNION_VIEWPORT_NOTIFY_COUNT * lpMsg = (PMSG_UNION_VIEWPORT_NOTIFY_COUNT *)&cBUFFER_V1;
	PMSG_UNION_VIEWPORT_NOTIFY * lpMsgBody = (PMSG_UNION_VIEWPORT_NOTIFY *)&cBUFFER_V1[sizeof(PMSG_UNION_VIEWPORT_NOTIFY_COUNT)];

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (!ObjectMaxRange(Obj.VpPlayer[n].number))
		{
			continue;
		}

		if (Obj.VpPlayer[n].state == 2 && Obj.VpPlayer[n].type == OBJ_USER)
		{
			CGameObject* lpTargetObj = getGameObject(Obj.VpPlayer[n].number);

			if (lpTargetObj == 0)
			{
				continue;
			}

			if (lpTargetObj->m_PlayerData->lpGuild == 0)
			{
				continue;
			}

			gObjGetGuildUnionName(*lpTargetObj, lpMsgBody[iVp1Count].szUnionName, sizeof(lpMsgBody[iVp1Count].szUnionName));

			lpMsgBody[iVp1Count].btGuildRelationShip = gObjGetRelationShip(Obj, *lpTargetObj);
			lpMsgBody[iVp1Count].btNumberL = SET_NUMBERL(WORD(lpTargetObj->m_Index));
			lpMsgBody[iVp1Count].btNumberH = SET_NUMBERH(WORD(lpTargetObj->m_Index));
			lpMsgBody[iVp1Count].iGuildNumber = lpTargetObj->m_PlayerData->lpGuild->Number;
			iVp1Count++;
		}
	}

	if (iVp1Count > 0 && iVp1Count <= MAX_VIEWPORT)
	{
		lpMsg->btCount = iVp1Count;

		PHeadSetW((BYTE *)lpMsg, 0x67, iVp1Count * sizeof(PMSG_UNION_VIEWPORT_NOTIFY) + sizeof(PMSG_UNION_VIEWPORT_NOTIFY_COUNT));

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)lpMsg, ((lpMsg->h.sizeL & 0xFF) & 0xFF | ((lpMsg->h.sizeH & 0xFF) & 0xFF) << 8) & 0xFFFF);
	}
	else
	{
		if (iVp1Count != 0)
		{
			sLog->outBasic("[Union ViewPort] ERROR : iVp1Count is OUT of BOUND: %d", iVp1Count);
		}
	}
}





void gObjNotifyUpdateUnionV2(CGameObject &Obj)
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.m_PlayerData->lpGuild == 0)
	{
		return;
	}

	char cBUFFER_V2[100] = { 0 };

	PMSG_UNION_VIEWPORT_NOTIFY_COUNT * lpMsg2 = (PMSG_UNION_VIEWPORT_NOTIFY_COUNT *)&cBUFFER_V2;
	PMSG_UNION_VIEWPORT_NOTIFY * lpMsgBody2 = (PMSG_UNION_VIEWPORT_NOTIFY *)&cBUFFER_V2[sizeof(PMSG_UNION_VIEWPORT_NOTIFY_COUNT)];

	gObjGetGuildUnionName(Obj, lpMsgBody2->szUnionName, sizeof(lpMsgBody2->szUnionName));
	lpMsgBody2->btNumberL = SET_NUMBERL(WORD(Obj.m_Index));
	lpMsgBody2->btNumberH = SET_NUMBERH(WORD(Obj.m_Index));

	lpMsgBody2->iGuildNumber = Obj.m_PlayerData->lpGuild->Number;
	lpMsgBody2->btGuildRelationShip = 0;
	lpMsg2->btCount = 1;

	PHeadSetW((BYTE *)lpMsg2, 0x67, sizeof(lpMsg2) + sizeof(lpMsgBody2[0]) + 1);

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)lpMsg2, ((lpMsg2->h.sizeL & 0xFF) & 0xFF | ((lpMsg2->h.sizeH & 0xFF) & 0xFF) << 8) & 0xFFFF);

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (Obj.VpPlayer2[n].type == OBJ_USER && Obj.VpPlayer2[n].state != 0)
		{
			CGameObject *lpTargetObj = Obj.VpPlayer2[n].user;

			if (lpTargetObj->m_PlayerData->lpGuild != 0)
			{
				lpMsgBody2->btGuildRelationShip = gObjGetRelationShip(*lpTargetObj, Obj);
			}

			if (lpMsgBody2->btGuildRelationShip != 1)
			{
				IOCP.DataSend(Obj.VpPlayer2[n].number, (BYTE *)lpMsg2, ((lpMsg2->h.sizeL & 0xFF) & 0xFF | ((lpMsg2->h.sizeH & 0xFF) & 0xFF) << 8) & 0xFFFF);
			}
		}
	}
}





void gObjUnionUpdateProc(CGameObject &Obj)
{

	if (gObjIsConnected(Obj) == FALSE)
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
		return;

	if (Obj.m_PlayerData->lpGuild == NULL)
	{
		return;
	}

	if (Obj.m_PlayerData->lpGuild->CheckTimeStamp(Obj.m_PlayerData->iGuildUnionTimeStamp) != FALSE)
	{
		return;
	}

	Obj.m_PlayerData->iGuildUnionTimeStamp = Obj.m_PlayerData->lpGuild->GetTimeStamp();
	gObjNotifyUpdateUnionV1(Obj);
	gObjNotifyUpdateUnionV2(Obj);

}

// GUILD_INFO_STRUCT 0x4e0a10





void gObjSetKillCount(CGameObject &Obj, int iOption)	// Option : [0 : SetToZero] [1 : Increase] [2 : Decrease]
{
	if (gObjIsConnected(Obj) == FALSE)
	{
		return;
	}

	

	if (iOption == 1)
	{
		if (Obj.m_btKillCount <= 254)
		{
			Obj.m_btKillCount++;
		}
	}
	else if (iOption == 2)
	{
		if (Obj.m_btKillCount > 0)
		{
			Obj.m_btKillCount--;
		}
	}
	else if (iOption == 0)
	{
		Obj.m_btKillCount = 0;
	}

	PMSG_KILLCOUNT pMsg = { 0 };
	PHeadSubSetB((BYTE*)&pMsg, 0xB8, 0x01, sizeof(pMsg));

	pMsg.btKillCount = Obj.m_btKillCount;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, sizeof(pMsg));
}

struct PMSG_NOTIFY_REGION_OF_WEAPON
{
	PBMSG_HEAD2 h;
	BYTE btWeaponType;
	BYTE btPointX;
	BYTE btPointY;
};

struct PMSG_NOTIFY_TARGET_OF_WEAPON
{
	PBMSG_HEAD2 h;
	BYTE NumberH;
	BYTE NumberL;
	BYTE btWeaponType;
};

struct PMSG_WEAPON_OWNER_VIEWPORT_NOTIFY_COUNT
{
	struct PWMSG_HEAD h;
	BYTE btCount;
};

struct PMSG_WEAPON_OWNER_VIEWPORT_NOTIFY
{
	BYTE btObjType;
	BYTE btObjClassH;
	BYTE btObjClassL;
	BYTE btObjIndexH;
	BYTE btObjIndexL;
	BYTE btX;
	BYTE btY;
	BYTE CharSet[17]; //lol
	BYTE btViewSkillStateCount;
	BYTE btViewSkillState[32];
};

void gObjNotifyUseWeaponV1(CGameObject &OwnerObj, CGameObject &WeaponObj, int iTargetX, int iTargetY)
{

	PMSG_NOTIFY_REGION_OF_WEAPON pNotifyRegionMsg = { 0 };

	PHeadSubSetB((BYTE*)&pNotifyRegionMsg, 0xB7, 2, sizeof(pNotifyRegionMsg));

	pNotifyRegionMsg.btPointX = iTargetX;
	pNotifyRegionMsg.btPointY = iTargetY;

	PMSG_NOTIFY_TARGET_OF_WEAPON pNotifyTargetMsg = { 0 };

	PHeadSubSetB((BYTE*)&pNotifyTargetMsg, 0xB7, 3, sizeof(pNotifyTargetMsg));

	if (WeaponObj.Class == 221)
	{
		pNotifyRegionMsg.btWeaponType = 1;
		pNotifyTargetMsg.btWeaponType = 1;
	}
	else if (WeaponObj.Class == 222)
	{
		pNotifyRegionMsg.btWeaponType = 2;
		pNotifyTargetMsg.btWeaponType = 2;
	}

	char cBUFFER_V1[6000];
	int iVp1Count = 0;

	memset(cBUFFER_V1, 0x00, sizeof(cBUFFER_V1));

	PMSG_WEAPON_OWNER_VIEWPORT_NOTIFY_COUNT * lpMsg = (PMSG_WEAPON_OWNER_VIEWPORT_NOTIFY_COUNT *)&cBUFFER_V1;
	PMSG_WEAPON_OWNER_VIEWPORT_NOTIFY * lpMsgBody = (PMSG_WEAPON_OWNER_VIEWPORT_NOTIFY *)&cBUFFER_V1[sizeof(PMSG_WEAPON_OWNER_VIEWPORT_NOTIFY_COUNT)];

	for (int n = 0; n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		CGameObject* lpTargetObj = getGameObject(n);

		if (gObjIsConnected(*lpTargetObj) == 0)
		{
			continue;
		}

		if (OwnerObj.MapNumber != lpTargetObj->MapNumber)
		{
			continue;
		}

		if (abs(lpTargetObj->X - iTargetX) > 6)
		{
			continue;
		}

		if (abs(lpTargetObj->Y - iTargetY) > 6)
		{
			continue;
		}

		lpMsgBody[iVp1Count].btObjClassH = SET_NUMBERH(lpTargetObj->Class);
		lpMsgBody[iVp1Count].btObjClassL = SET_NUMBERL(lpTargetObj->Class);
		lpMsgBody[iVp1Count].btObjIndexH = SET_NUMBERH(lpTargetObj->m_Index);
		lpMsgBody[iVp1Count].btObjIndexH = SET_NUMBERL(lpTargetObj->m_Index);
		lpMsgBody[iVp1Count].btX = lpTargetObj->X;
		lpMsgBody[iVp1Count].btY = lpTargetObj->Y;
		lpMsgBody[iVp1Count].btViewSkillStateCount = gObjMakeViewportState(*lpTargetObj, lpMsgBody[iVp1Count].btViewSkillState); //Season3 changed

		if (lpTargetObj->Type == OBJ_USER)
		{
			lpMsgBody[iVp1Count].btObjType = OBJ_USER;
			lpMsgBody[iVp1Count].btObjClassH = CS_SET_CLASS(lpTargetObj->Class);
			lpMsgBody[iVp1Count].btObjClassL = 0;

			std::memcpy(lpMsgBody[iVp1Count].CharSet, &lpTargetObj->CharSet[1], sizeof(lpMsgBody[iVp1Count].CharSet));
		}
		else if (lpTargetObj->Type == OBJ_MONSTER)
		{
			lpMsgBody[iVp1Count].btObjType = OBJ_MONSTER;
			memset(lpMsgBody[iVp1Count].CharSet, 0x00, sizeof(lpMsgBody[iVp1Count].CharSet));
		}
		else
		{
			lpMsgBody[iVp1Count].btObjType = OBJ_NPC;
			memset(lpMsgBody[iVp1Count].CharSet, 0x00, sizeof(lpMsgBody[iVp1Count].CharSet));
		}

		iVp1Count++;

		if (lpTargetObj->Type == OBJ_USER)
		{
			IOCP.DataSend(lpTargetObj->m_Index, (BYTE*)&pNotifyRegionMsg, sizeof(pNotifyRegionMsg));

			if (abs(lpTargetObj->X - iTargetX) > 3)
			{
				continue;
			}

			if (abs(lpTargetObj->Y - iTargetY) > 3)
			{
				continue;
			}

			pNotifyTargetMsg.NumberH = SET_NUMBERH(lpTargetObj->m_Index);
			pNotifyTargetMsg.NumberL = SET_NUMBERL(lpTargetObj->m_Index);
		}
	}

	if (iVp1Count > 0 && iVp1Count <= 100)
	{
		lpMsg->btCount = iVp1Count;
		PHeadSetW((BYTE*)lpMsg, 0x68, iVp1Count * sizeof(PMSG_WEAPON_OWNER_VIEWPORT_NOTIFY) + sizeof(PMSG_WEAPON_OWNER_VIEWPORT_NOTIFY_COUNT));

		IOCP.DataSend(OwnerObj.m_PlayerData->ConnectUser->Index, (BYTE*)lpMsg, ((lpMsg->h.sizeL & 0xFF) & 0xFF | ((lpMsg->h.sizeH & 0xFF) & 0xFF) << 8) & 0xFFFF);
	}
}





void gObjNotifyUseWeaponDamage(CGameObject &WeaponObj, int iTargetX, int iTargetY)
{
	if (WeaponObj.Class == 221 || WeaponObj.Class == 222)
	{
		for (int i = 0; i < g_ConfigRead.server.GetObjectMax(); i++)
		{
			CGameObject* lpObj = getGameObject(i);

			if (WeaponObj.MapNumber != lpObj->MapNumber)
			{
				continue;
			}

			if (abs(lpObj->X - iTargetX) > 3)
			{
				continue;
			}

			if (abs(lpObj->Y - iTargetY) > 3)
			{
				continue;
			}

			g_CsNPC_Weapon.AddWeaponDamagedTargetInfo(WeaponObj.m_Index, i, 100);
		}
	}

}





void gObjUseBlessAndSoulPotion(CGameObject &Obj, int iItemLevel)
{
	

	if (iItemLevel == 0)	// Bless Potion
	{
		gObjAddBuffEffect(Obj, BUFFTYPE_BLESS_POTION, 0, 0, 0, 0, 60);
	}
	else if (iItemLevel == 1) // Soul Potion
	{
		gObjAddBuffEffect(Obj, BUFFTYPE_SOUL_POTION, 0, 0, 0, 0, 60);
		gObjUseDrink(Obj, 0x4D);
	}
}





void gObjWeaponDurDownInCastle(CGameObject &Obj, CGameObject lpTargetObj, int iDecValue)
{
	if (Obj.Type == OBJ_MONSTER || Obj.Type == OBJ_NPC)
	{
		return;
	}

	if (g_MineSystem.IsEquipPickax(Obj))
	{
		return;
	}

	int itargetdefence = 0;

	CItemObject * Right = Obj.pntInventory[0];
	CItemObject * Left = Obj.pntInventory[1];

	int bIsRightDurDown = 0;
	int bIsLeftDurDown = 0;

	if (Obj.Class == 1 || Obj.Class == 3 || Obj.Class == 4 || Obj.Class == 6)
	{
		if (Right->m_Type >= ITEMGET(0, 0) && Right->m_Type < ITEMGET(4, 0)
			&& Left->m_Type >= ITEMGET(0, 0) && Left->m_Type < ITEMGET(4, 0))
		{
			bIsRightDurDown = 1;
			bIsLeftDurDown = 1;
		}
	}

	if (Right->m_Type >= ITEMGET(4, 0) && Right->m_Type <= ITEMGET(4, 6)
		|| Right->m_Type >= ITEMGET(4, 8) && Right->m_Type <= ITEMGET(4, 14)
		|| Right->m_Type == ITEMGET(4, 16) && Right->m_Type <= ITEMGET(4, 31))
	{
		bIsRightDurDown = 1;
	}
	else if (Right->m_Type >= 0 && Right->m_Type < ITEMGET(4, 0))
	{
		bIsRightDurDown = 1;
	}
	else if (Right->m_Type >= ITEMGET(5, 0) && Right->m_Type < ITEMGET(6, 0))
	{
		bIsRightDurDown = 1;
	}

	if (bIsRightDurDown != 0)
	{
		int iRet = Right->SimpleDurabilityDown(iDecValue);

		if (iRet != 0)
		{
			gGameProtocol.GCItemObjectDurSend(Obj, 0, Right->m_Durability, 0);

			if (iRet == 2)
			{
				gObjCalCharacter.CalcCharacter(Obj);
				gObjCalCharacter.CalcMLSkillItemOption(Obj);
			}
		}
	}

	if (bIsLeftDurDown != 0)
	{
		int iRet = Left->SimpleDurabilityDown(iDecValue);

		if (iRet != 0)
		{
			gGameProtocol.GCItemObjectDurSend(Obj, 1, Left->m_Durability, 0);

			if (iRet == 2)
			{
				gObjCalCharacter.CalcCharacter(Obj);
				gObjCalCharacter.CalcMLSkillItemOption(Obj);
			}
		}
	}
}

void gObjCheckTimeOutValue(CGameObject &Obj, DWORD& rNowTick)
{
	long lTick;
	long lSecond;

	lTick = rNowTick - Obj.m_dwLastCheckTick;
	lSecond = lTick / 1000;

	if (lSecond == 0)
	{
		return;
	}

	Obj.m_dwLastCheckTick = rNowTick;
}



void MsgOutput(CGameObject &Obj, char* msg, ...)
{
	char szBuffer[512] = "";
	va_list pArguments;

	va_start(pArguments, msg);
	vsprintf(&szBuffer[0], msg, pArguments);
	va_end(pArguments);

	gGameProtocol.GCServerMsgStringSend(&szBuffer[0], Obj, 1);
}





void gProcessAutoRecuperation(CGameObject &Obj)
{
	if (Obj.Life == (Obj.MaxLife + Obj.AddLife)
		&& Obj.Mana == (Obj.MaxMana + Obj.AddMana)
		&& Obj.BP == (Obj.MaxBP + Obj.AddBP))
	{
		Obj.m_iAutoRecuperationTime = GetTickCount();
		return;
	}

	if ((GetTickCount() - Obj.m_iAutoRecuperationTime) >= 10000 && (GetTickCount() - Obj.m_iAutoRecuperationTime) < 15000)
	{
		if (Obj.Life < (Obj.MaxLife + Obj.AddLife))
		{
			Obj.Life += 1.0f;

			if (Obj.Life >(Obj.MaxLife + Obj.AddLife))
			{
				Obj.Life = Obj.MaxLife + Obj.AddLife;
			}

			gGameProtocol.GCReFillSend(Obj, Obj.Life, -1, 0, Obj.iShield);
		}

		if (Obj.Mana < (Obj.MaxMana + Obj.AddMana) || Obj.BP < (Obj.MaxBP + Obj.AddBP))
		{
			Obj.Mana += 1.0f;

			if (Obj.Mana >(Obj.MaxMana + Obj.AddMana))
			{
				Obj.Mana = Obj.MaxMana + Obj.AddMana;
			}

			Obj.BP += 1;

			if (Obj.BP > (Obj.MaxBP + Obj.AddBP))
			{
				Obj.BP = Obj.MaxBP + Obj.AddBP;
			}

			gGameProtocol.GCManaSend(Obj, Obj.Mana, 0xFF, 0, Obj.BP);
		}
	}
	else if ((GetTickCount() - Obj.m_iAutoRecuperationTime) >= 15000 && (GetTickCount() - Obj.m_iAutoRecuperationTime) < 25000)
	{
		if (Obj.Life < (Obj.MaxLife + Obj.AddLife))
		{
			Obj.Life += 5.0f;

			if (Obj.Life >(Obj.MaxLife + Obj.AddLife))
			{
				Obj.Life = Obj.MaxLife + Obj.AddLife;
			}

			gGameProtocol.GCReFillSend(Obj, Obj.Life, -1, 0, Obj.iShield);
		}

		if (Obj.Mana < (Obj.MaxMana + Obj.AddMana) || Obj.BP < (Obj.MaxBP + Obj.AddBP))
		{
			Obj.Mana += 5.0f;

			if (Obj.Mana >(Obj.MaxMana + Obj.AddMana))
			{
				Obj.Mana = Obj.MaxMana + Obj.AddMana;
			}

			Obj.BP += 5;

			if (Obj.BP > (Obj.MaxBP + Obj.AddBP))
			{
				Obj.BP = Obj.MaxBP + Obj.AddBP;
			}

			gGameProtocol.GCManaSend(Obj, Obj.Mana, 0xFF, 0, Obj.BP);
		}
	}
	else if ((GetTickCount() - Obj.m_iAutoRecuperationTime) >= 25000)
	{
		if (Obj.Life < (Obj.MaxLife + Obj.AddLife))
		{
			Obj.Life += 10.0f;

			if (Obj.Life >(Obj.MaxLife + Obj.AddLife))
			{
				Obj.Life = Obj.MaxLife + Obj.AddLife;
			}

			gGameProtocol.GCReFillSend(Obj, Obj.Life, -1, 0, Obj.iShield);
		}

		if (Obj.Mana < (Obj.MaxMana + Obj.AddMana) || Obj.BP < (Obj.MaxBP + Obj.AddBP))
		{
			Obj.Mana += 10.0f;

			if (Obj.Mana >(Obj.MaxMana + Obj.AddMana))
			{
				Obj.Mana = Obj.MaxMana + Obj.AddMana;
			}

			Obj.BP += 10;

			if (Obj.BP > (Obj.MaxBP + Obj.AddBP))
			{
				Obj.BP = Obj.MaxBP + Obj.AddBP;
			}

			gGameProtocol.GCManaSend(Obj, Obj.Mana, 0xFF, 0, Obj.BP);
		}
	}
}

void gObjShieldAutoRefill(CGameObject &Obj)
{
	int iRefillPoint = 0;
	int iShieldRefillOption = 0;

	if (g_ConfigRead.g_ShieldSystemOn == 0)
	{
		return;
	}

	if (g_ConfigRead.g_ShieldAutoRefillOn == 0)
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (MAX_MAP_RANGE(Obj.MapNumber) == FALSE)
	{
		return;
	}

	if (g_ConfigRead.g_ShieldAutoRefillOnSafeZone == 1)
	{
		BYTE btMapAttribute = MapC[Obj.MapNumber].GetAttr(Obj.X, Obj.Y);

		if ((btMapAttribute & 1) != 1 && Obj.m_PlayerData->m_ItemOptionExFor380->OpRefillOn == 0)
		{
			Obj.dwShieldAutoRefillTimer = GetTickCount();
			return;
		}
	}

	if (Obj.iShield >= (Obj.iMaxShield + Obj.iAddShield))
	{
		Obj.dwShieldAutoRefillTimer = GetTickCount();
		return;
	}

	iShieldRefillOption = Obj.m_PlayerData->m_ItemOptionExFor380->OpAddRefillSD;

	int iRefillExpression = 0;

	float fRefillExpressionA = (Obj.iMaxShield + Obj.iAddShield) / 30;
	float fRefillExpressionB = iShieldRefillOption + 100;

	iRefillExpression = ((fRefillExpressionA * fRefillExpressionB) / 100.0f) / 25.0f;

	unsigned long dwTick = GetTickCount() - Obj.dwShieldAutoRefillTimer;

	if (dwTick >= 25000)
	{
		iRefillPoint = iRefillExpression * 3;
	}
	else if (dwTick >= 15000)
	{
		iRefillPoint = (iRefillExpression * 25) / 10;
	}
	else if (dwTick >= 10000)
	{
		iRefillPoint = iRefillExpression * 2;
	}
	else
	{
		return;
	}

	iRefillPoint++; //season4 changed (fix)

	if (iRefillPoint == 0)
	{
		return;
	}

	iRefillPoint += iRefillPoint * Obj.m_PlayerData->m_MPSkillOpt->iMpsSDSpeed / 100.0;

	Obj.iShield += iRefillPoint;

	if (Obj.iShield > (Obj.iMaxShield + Obj.iAddShield))
	{
		Obj.iShield = Obj.iMaxShield + Obj.iAddShield;
	}

	gGameProtocol.GCReFillSend(Obj, Obj.Life, -1, 0, Obj.iShield);
}

int gObjCheckOverlapItemUsingDur(CGameObject &Obj, int iMaxOverlapped, int iItemType, int iItemLevel)
{
	for (int x = INVETORY_WEAR_SIZE; x < MAIN_INVENTORY_SIZE; x++)
	{
		if (Obj.pntInventory[x]->IsItem() == 1
			&& Obj.pntInventory[x]->m_Type == (short)iItemType
			&& Obj.pntInventory[x]->m_Level == (short)iItemLevel)
		{
			int iITEM_DUR = Obj.pntInventory[x]->m_Durability;

			if ((((iITEM_DUR) < 0) ? FALSE : ((iITEM_DUR) > iMaxOverlapped - 1) ? FALSE : TRUE))
			{
				return x;
			}
		}
	}

	return -1;
}

int gObjOverlapItemUsingDur(CItemObject &Item, int iMapNumber, int iItemNumber, CGameObject &Obj, int iMaxOverlapped, int iItemType, int iItemLevel)
{
	for (int iLoop = 0; iLoop < MAIN_INVENTORY_SIZE; iLoop++)
	{
		int iInventoryIndex = gObjCheckOverlapItemUsingDur(Obj, iMaxOverlapped, iItemType, iItemLevel);

		if (MAIN_INVENTORY_RANGE(iInventoryIndex))
		{
			int iItemDur = Obj.pntInventory[iInventoryIndex]->m_Durability + Item.m_Durability;

			if (iItemDur <= iMaxOverlapped)
			{
				if (MapC[iMapNumber].ItemGive(Obj, Item.m_Number, 1) == 1)
				{
					return iInventoryIndex;
				}
			}

			else
			{
				Item.m_Durability = iItemDur - iMaxOverlapped;
				Obj.pntInventory[iInventoryIndex]->m_Durability = iMaxOverlapped;

				gGameProtocol.GCItemObjectDurSend(Obj, iInventoryIndex, Obj.pntInventory[iInventoryIndex]->m_Durability, 0);
			}
		}

		else
		{
			return -1;
		}
	}

	return -1;
}

int gObjCheckOverlapEventItemUsingDur(CGameObject &Obj, int iMaxOverlapped, int iItemType, int iItemLevel)
{
	for (int x = 0; x < EVENT_INVENTORY_SIZE; x++)
	{
		if (Obj.pntEventInventory[x]->IsItem() == 1
			&& Obj.pntEventInventory[x]->m_Type == (short)iItemType
			&& Obj.pntEventInventory[x]->m_Level == (short)iItemLevel)
		{
			int iITEM_DUR = Obj.pntEventInventory[x]->m_Durability;

			if ((((iITEM_DUR) < 0) ? FALSE : ((iITEM_DUR) > iMaxOverlapped - 1) ? FALSE : TRUE))
			{
				return x;
			}
		}
	}

	return -1;
}

int gObjOverlapEventItemUsingDur(CItemObject* lpItem, int iMapNumber, int iItemNumber, CGameObject &Obj, int iMaxOverlapped, int iItemType, int iItemLevel)
{
	for (int iLoop = 0; iLoop < EVENT_INVENTORY_SIZE; iLoop++)
	{
		int iInventoryIndex = gObjCheckOverlapEventItemUsingDur(Obj, iMaxOverlapped, iItemType, iItemLevel);

		if (EVENT_INVENTORY_RANGE(iInventoryIndex))
		{
			int iItemDur = Obj.pntEventInventory[iInventoryIndex]->m_Durability + lpItem->m_Durability;

			if (iItemDur <= iMaxOverlapped)
			{
				if (MapC[iMapNumber].ItemGive(Obj, iItemNumber, 1) == 1)
				{
					return iInventoryIndex;
				}
			}

			else
			{
				lpItem->m_Durability = iItemDur - iMaxOverlapped;
				Obj.pntEventInventory[iInventoryIndex]->m_Durability = iMaxOverlapped;

				gGameProtocol.GCEventItemDurSend(Obj, iInventoryIndex, Obj.pntEventInventory[iInventoryIndex]->m_Durability);
			}
		}

		else
		{
			return -1;
		}
	}

	return -1;
}

int gObjCheckSerial0ItemList(class CItemObject* lpItem)
{
	if (g_ConfigRead.ItemSerialCheck == 0)
	{
		return false;
	}

	if (lpItem->m_Type == ITEMGET(14, 13)
		|| lpItem->m_Type == ITEMGET(14, 14)
		|| lpItem->m_Type == ITEMGET(14, 16)
		|| lpItem->m_Type == ITEMGET(14, 22)
		|| lpItem->m_Type == ITEMGET(12, 15)
		|| lpItem->m_Type == ITEMGET(12, 30)
		|| lpItem->m_Type == ITEMGET(12, 31)
		|| lpItem->m_Type == ITEMGET(14, 31))
	{
		if (lpItem->GetNumber() == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

int gObjCheckInventorySerial0Item(CGameObject &Obj)
{
	UINT64 iItemSerial;
	int iCount;
	int i;

	if (g_ConfigRead.ItemSerialCheck == 0)
	{
		return false;
	}

	iCount = 0;

	for (int i = 0; i < INVENTORY_SIZE; i++)
	{
		if (Obj.pntInventory[i]->IsItem() == 0)
		{
			continue;
		}

		if (gObjCheckSerial0ItemList(Obj.pntInventory[i]) == 0)
		{
			continue;
		}

		iItemSerial = Obj.pntInventory[i]->GetNumber();

		if (iItemSerial == 0)
		{
			sLog->outBasic("[ANTI-HACK][Serial 0 Item] (%s)(%s) Item(%s) Pos(%d)",
				Obj.AccountID, Obj.Name, Obj.pntInventory[i]->GetName(), i);
			iCount++;
		}
	}

	if (iCount > 0)
	{
		return true;
	}

	return false;
}



BOOL gCheckSkillDistance(CGameObject &Obj, int aTargetIndex, int iSkillNum)
{
	if (g_ConfigRead.SkillDistanceCheck == 0)
		return TRUE;

	if (iSkillNum == 40)
		return TRUE;

	if (iSkillNum == 392)
		return TRUE;

	int iSkillDistance = MagicDamageC.GetSkillDistance(iSkillNum);

	if (iSkillDistance == -1)
		return FALSE;

	iSkillDistance += g_ConfigRead.SkillDistanceTolerance;
	
	CGameObject lpTargetObj = *getGameObject(aTargetIndex);

	if (abs(Obj.X - lpTargetObj.X) > iSkillDistance ||
		abs(Obj.Y - lpTargetObj.Y) > iSkillDistance)
	{
		sLog->outBasic("[SKILL DISTANCE CHECK] [%s][%s] Invalid Skill attacker(%d, %d), defender(%d, %d), skillnum:%d skilldistance:%d",
			Obj.AccountID, Obj.Name, Obj.X, Obj.Y, lpTargetObj.X, lpTargetObj.Y, iSkillNum, iSkillDistance);
		return FALSE;
	}

	return TRUE;
}

void gObjSaveChaosBoxItemList(CGameObject &Obj)
{
	BYTE ExOption[MAX_EXOPTION_SIZE];

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pntChaosBox[n]->IsItem() == TRUE)
		{
			ItemIsBufExOption(ExOption, Obj.pntChaosBox[n]);

			sLog->outBasic("[ChaosBoxItemList][Lost ItemList] [%s][%s] [%d,%s,%d,%d,%d,%d] Serial:[%I64d][%d] Ex:[%d,%d,%d,%d,%d,%d,%d] Set:[%d] 380:[%d] HO:[%d,%d] E:[%d]",
				Obj.AccountID, Obj.Name, n, Obj.pntChaosBox[n]->GetName(), Obj.pntChaosBox[n]->m_Level, Obj.pntChaosBox[n]->m_Option1, Obj.pntChaosBox[n]->m_Option2, Obj.pntChaosBox[n]->m_Option3, Obj.pntChaosBox[n]->m_Number,
				(int)Obj.pntChaosBox[n]->m_Durability, ExOption[0], ExOption[1], ExOption[2], ExOption[3], ExOption[4], ExOption[5], ExOption[6], Obj.pntChaosBox[n]->m_SetOption,
				Obj.pntChaosBox[n]->m_ItemOptionEx >> 7, g_kJewelOfHarmonySystem.GetItemStrengthenOption(Obj.pntChaosBox[n]), g_kJewelOfHarmonySystem.GetItemOptionLevel(Obj.pntChaosBox[n]),
				Obj.pntChaosBox[n]->m_BonusSocketOption);
		}
	}

	//DSMSG_CHAOSMACHINE_LOSTITEMS pMsg;
	//PHeadSetW((BYTE*)&pMsg, 0xC4, sizeof(pMsg));

	//std::memcpy(pMsg.szAccountID, Obj.AccountID, MAX_ACCOUNT_LEN + 1);
	//std::memcpy(pMsg.szName, Obj.Name, MAX_ACCOUNT_LEN + 1);
	//ItemByteConvert32(pMsg.btLostItems, Obj.pChaosBox, CHAOS_BOX_SIZE);

	//wsDataCli.DataSend((char *)&pMsg, sizeof(pMsg));
}

BOOL gObjGetRandomFreeArea(int iMapNumber, BYTE &cX, BYTE &cY, int iSX, int iSY, int iDX, int iDY, int iLoopCount)
{
	if (MAX_MAP_RANGE(iMapNumber) == FALSE)
	{
		return FALSE;
	}

	if (iSX <= 0) iSX = 1;
	if (iSY <= 0) iSY = 1;
	if (iDX <= 0) iDX = 1;
	if (iDY <= 0) iDY = 1;

	if (iSX >= 256) iSX = 255;
	if (iSY >= 256) iSY = 255;
	if (iDX >= 256) iDX = 255;
	if (iDY >= 256) iDY = 255;

	if (iLoopCount <= 0) iLoopCount = 1;

	while (iLoopCount-- > 0)
	{
		cX = (rand() % (iDX - iSX)) + iSX;
		cY = (rand() % (iDY - iSY)) + iSY;

		BYTE btMapAttr = MapC[iMapNumber].GetAttr(cX, cY);

		if (btMapAttr == 0)
			return TRUE;
	}
	return FALSE;
}

void gObjAddSpecificSkillsForSiege(CGameObject &Obj)
{

}

int gObjCalcHPPercent(double Life, double MaxLife)
{
	if (Life <= 0)
	{
		return 0;
	}

	return (int)Life * 100 / (int)MaxLife;
}

bool CheckAuthorityCondition(int AuthorityCode, CGameObject &Obj) //0043F3E0 identical season4.5 add-on
{
	return ((Obj.Authority&AuthorityCode) == Obj.Authority) ? true : false;
}

void gObjReqMapSvrAuth(CGameObject &Obj)
{
	if (Obj.Connected != PLAYER_CONNECTED)
	{
		sLog->outBasic("%s is not connected", Obj.AccountID);
		IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
		return;
	}

	GJReqMapSvrAuth(Obj, Obj.AccountID, Obj.m_MapServerAuthInfo->szCharName,
		Obj.m_MapServerAuthInfo->iJA1, Obj.m_MapServerAuthInfo->iJA2,
		Obj.m_MapServerAuthInfo->iJA3, Obj.m_MapServerAuthInfo->iJA4);

	sLog->outBasic("[MapServerMng] Map Server Join Send : [%s][%s](%d)",
		Obj.AccountID, Obj.m_MapServerAuthInfo->szCharName, Obj.m_Index);

	memset(Obj.m_MapServerAuthInfo->szCharName, 0x00, MAX_ACCOUNT_LEN + 2);
	Obj.m_MapServerAuthInfo->iJA1 = 0;
	Obj.m_MapServerAuthInfo->iJA2 = 0;
	Obj.m_MapServerAuthInfo->iJA3 = 0;
	Obj.m_MapServerAuthInfo->iJA4 = 0;
}

int gGetPartyMaxLevel(int nPartyNumber)
{
	int nResult = -1;

	for (int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		int nUserIndex = gParty.m_PartyS[nPartyNumber].Number[i];

		if (nUserIndex <= 0)
		{
			continue;
		}

		if (getGameObject(nUserIndex)->Type != OBJ_USER)
		{
			continue;
		}

		if (nResult < (getGameObject(nUserIndex)->Level + getGameObject(nUserIndex)->m_PlayerData->MasterLevel))
		{
			nResult = getGameObject(nUserIndex)->Level + getGameObject(nUserIndex)->m_PlayerData->MasterLevel;
		}
	}

	return nResult;
}

int gGetLowHPMonster(int nZoneIndex, int nIndex, int nDistance)
{
	CGameObject lpObj = *getGameObject(nIndex);
	int nTargetIndex = -1;
	int nTempTargetLife = 0;
	int nTargetLife = 0;

	_stZoneInfo * result = g_ImperialGuardian.GetZoneInfo(result, nZoneIndex);

	DWORD nMonsterCount = result->m_vtMonsterIndexInfo.size();

	for (int i = 0; i < nMonsterCount; i++)
	{
		_stMonsterIndexInfo monsterIndexInfo = result->m_vtMonsterIndexInfo.at(i);

		if (gObjCalDistance(lpObj, *getGameObject(monsterIndexInfo.m_nIndex)) < nDistance)
		{
			if (monsterIndexInfo.m_bLive == true)
			{
				nTempTargetLife = getGameObject(monsterIndexInfo.m_nIndex)->Life / getGameObject(monsterIndexInfo.m_nIndex)->MaxLife * 100;

				if (nTempTargetLife < nTargetLife || !nTargetLife)
				{
					if (nTempTargetLife != 100)
					{
						if (nTempTargetLife > 0)
						{
							nTargetLife = nTempTargetLife;
							nTargetIndex = monsterIndexInfo.m_nIndex;
						}
					}
				}
			}
		}
	}

	return nTargetIndex;
}

BOOL gObjGamblingInventoryCheck(CGameObject &Obj, int iWidth, int iHeight)
{
	for (int h = 0; h < MAX_INVENTORY_H; h++)
	{
		for (int w = 0; w < 8; w++)
		{
			if (*(BYTE*)(Obj.pInventoryMap + h * 8 + w) == 0xFF)
			{
				BYTE blank = gObjOnlyInventoryRectCheck(Obj, w, h, iWidth, iHeight);

				if (blank == 0xFE)
					return FALSE;
				if (blank != 0xFF)
					return TRUE;
			}
		}
	}

	return FALSE;
}

void gObjPentagramMixBoxSet(CGameObject &Obj, int itempos, int xl, int yl, BYTE set_byte)
{
	int itemposx = (itempos) % 8;
	int itemposy = (itempos) / 8;

	int xx, yy;

	for (int y = 0; y < yl; y++)
	{
		yy = itemposy + y;

		for (int x = 0; x < xl; x++)
		{
			xx = itemposx + x;

			if (ExtentCheck(xx, yy, 8, 4) == 1)
			{
				*(BYTE*)(Obj.m_PlayerData->pPentagramMixBoxMap + (itemposy + y) * 8 + (itemposx + x)) = set_byte;
			}
			else
			{
				sLog->outBasic("error : %d/%d  %s %d", xx, yy, __FILE__, __LINE__);
				return;
			}
		}
	}
}

void gObjPentagramMixItemSet(CGameObject &Obj, int itempos, BYTE set_byte)
{
	int width;
	int height;
	if (itempos < 0)
	{
		return;
	}
	else if (itempos >(CHAOS_BOX_SIZE - 1))
	{
		return;
	}

	if (Obj.m_PlayerData->pPentagramMixBox[itempos]->GetSize((int&)width, (int &)height) == 0)
	{
		sLog->outBasic("error: Item does not exist %s %d", __FILE__, __LINE__);
		return;
	}

	gObjPentagramMixBoxSet(Obj, itempos, width, height, set_byte);
}

BYTE gObjPentagramMixBoxInsertItemPos(CGameObject &Obj, CItemObject &item, int pos, int source)
{
	CGameObject lpObj;
	BYTE TempMap[CHAOS_BOX_MAP_SIZE];

	int w, h, iwidth, iheight;

	int blank, useClass;

	if (pos < 0 || pos >(CHAOS_BOX_SIZE - 1))
	{
		return -1;
	}

	useClass = 0;

	lpObj = lpObj;

	if (Obj.m_PlayerData->pPentagramMixBox[pos]->IsItem() == 1)
	{
		return -1;
	}

	if (item.IsItem() == 0)
	{
		return -1;
	}

	w = pos % 8;
	h = pos / 8;

	if (ExtentCheck(w, h, 8, 4) == 0)
	{
		return -1;
	}

	item.GetSize((int &)iwidth, (int &)iheight);

	if (source >= 0)
	{
		std::memcpy(TempMap, Obj.m_PlayerData->pPentagramMixBoxMap, CHAOS_BOX_MAP_SIZE);
		gObjPentagramMixBoxSet(Obj, source, iwidth, iheight, 255);
	}

	if (*(BYTE*)(Obj.m_PlayerData->pPentagramMixBoxMap + h * 8 + w) != 255)
	{
		if (source >= 0)
		{
			std::memcpy(Obj.m_PlayerData->pPentagramMixBoxMap, TempMap, CHAOS_BOX_MAP_SIZE);
		}
		return -1;
	}

	blank = gObjMapRectCheck(*Obj.m_PlayerData->pPentagramMixBoxMap, w, h, 8, 4, iwidth, iheight);

	if (blank == 255)
	{
		if (source >= 0)
		{
			std::memcpy(Obj.m_PlayerData->pPentagramMixBoxMap, TempMap, CHAOS_BOX_MAP_SIZE);
		}
		return -1;
	}

	Obj.m_PlayerData->pPentagramMixBox[pos] = &item;
	return pos;
}

BYTE gObjPentagramMixBoxInsertItem(CGameObject &Obj, CItemObject &item)
{
	if (item.IsItem() == FALSE)
	{
		return -1;
	}

	int iwidth, iheight;
	item.GetSize(iwidth, iheight);
	BYTE blank = 0;

	for (int h = 0; h < 4; h++)
	{
		for (int w = 0; w < 8; w++)
		{
			if (*(BYTE*)(Obj.m_PlayerData->pPentagramMixBoxMap + h * 8 + w) == 255)
			{
				blank = gObjPentagramMixBoxRectCheck(Obj, w, h, iwidth, iheight);

				if (blank == 254)
				{
					goto GOTO_EndFunc;
				}

				if (blank != 255)
				{
					gObjPentagramMixBoxInsertItemPos(Obj, item, blank, -1);
					return blank;
				}
			}
		}
	}
GOTO_EndFunc:
	return -1;
}

BYTE gObjPentagramMixBoxRectCheck(CGameObject &Obj, int sx, int sy, int width, int height)
{
	int x;
	int y;
	int xx;
	int yy;

	if ((sx + width) > 8)
	{
		return -1;
	}

	if ((sy + height) > 4)
	{
		return -2;
	}

	for (y = 0; y < height; y++)
	{
		yy = sy + y;

		for (x = 0; x < width; x++)
		{
			xx = sx + x;

			if (::ExtentCheck(xx, yy, 8, 4) == TRUE)
			{
				if (*(BYTE*)(Obj.m_PlayerData->pPentagramMixBoxMap + yy * 8 + xx) != (BYTE)-1)
				{
					return -1;
				}
			}
			else
			{
				sLog->outBasic("error : %s %d", __FILE__, __LINE__);
				return -1;
			}
		}
	}

	return (sy * 8 + sx);
}

BYTE gObjPentagramMixBoxDeleteItem(CGameObject &Obj, int itempos)
{
	gObjPentagramMixItemSet(Obj, itempos, -1);
	Obj.m_PlayerData->pPentagramMixBox[itempos]->Clear();
	return TRUE;
}

BOOL gTransPentagramJewelInfoTest(CGameObject &Obj, int targetIndex, CItemObject sourceItem)
{
	BYTE iTargetDBJewelIndex = 0xFF;

	for (int i = 0;; i++)
	{
		if (i >= 5)
			return TRUE;

		if (sourceItem.m_SocketOption[i] < 0xFE)
			break;
	}

	for (int index = 0; index < 254; index++)
	{
		BOOL bIndexExist = FALSE;

		for (int j = 0; j < 254; j++)
		{
			if (Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btJewelIndex != 0xFF)
			{
				if (Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btJewelIndex == index)
				{
					bIndexExist = TRUE;
				}
			}
		}

		if (!bIndexExist)
		{
			iTargetDBJewelIndex = index;
			break;
		}
	}

	if (iTargetDBJewelIndex == 0xFF)
	{
		sLog->outBasic("[ gTransPentagramJewelInfo ] PentagramJewelIndex Area Not Exist!!");
		return FALSE;
	}

	return TRUE;
}

int gTransPentagramJewelInfo(CGameObject &Obj, BYTE madeItemPos, int targetIndex, CItemObject sourceItem, int *nJewelCount)
{
	
	CGameObject lpTargetObj = *getGameObject(targetIndex);

	if (Obj.Type != OBJ_USER)
	{
		return FALSE;
	}

	if (lpTargetObj.Type != OBJ_USER)
	{
		return FALSE;
	}

	BYTE btJewelPos;
	BYTE btItemType;
	WORD wItemIndex;
	BYTE btMainAttribute;
	BYTE btLevel;
	BYTE btRank1OptionNum;
	BYTE btRank1Level;
	BYTE btRank2OptionNum;
	BYTE btRank2Level;
	BYTE btRank3OptionNum;
	BYTE btRank3Level;
	BYTE btRank4OptionNum;
	BYTE btRank4Level;
	BYTE btRank5OptionNum;
	BYTE btRank5Level;
	BYTE btTargetDBJewelIndex = 0xFF;
	BOOL bSave = FALSE;
	*nJewelCount = 0;

	for (int i = 0; i < 5; i++)
	{
		if (sourceItem.m_SocketOption[i] < 0xFE)
		{
			for (int index = 0; index < 254; index++)
			{
				BOOL bIndexExist = FALSE;

				for (int j = 0; j < 254; j++)
				{
					if (Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btJewelIndex != 0xFF)
					{
						if (Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btJewelIndex == index)
						{
							bIndexExist = TRUE;
						}
					}
				}

				if (!bIndexExist)
				{
					btTargetDBJewelIndex = index;
					break;
				}
			}

			if (btTargetDBJewelIndex == 0xFF)
			{
				sLog->outBasic("[TransPentagramJewelInfo][%s][%s] PentagramJewelIndex Area Not Exist!!", Obj.AccountID, Obj.Name);
				return FALSE;
			}

			bSave = TRUE;
			btJewelPos = 0xFF;
			*nJewelCount++;
			Obj.pntInventory[madeItemPos]->m_SocketOption[i] = btTargetDBJewelIndex;

			for (int k = 0; k < 254; k++)
			{
				if (lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[k].btJewelIndex == sourceItem.m_SocketOption[i])
				{
					btJewelPos = 0;
					btItemType = lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[k].btItemType;
					wItemIndex = lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[k].wItemIndex;
					btMainAttribute = lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[k].btMainAttribute;
					btLevel = lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[k].btLevel;
					btRank1OptionNum = lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[k].btRank1OptionNum;
					btRank1Level = lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[k].btRank1Level;
					btRank2OptionNum = lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[k].btRank2OptionNum;
					btRank2Level = lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[k].btRank2Level;
					btRank3OptionNum = lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[k].btRank3OptionNum;
					btRank3Level = lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[k].btRank3Level;
					btRank4OptionNum = lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[k].btRank4OptionNum;
					btRank4Level = lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[k].btRank4Level;
					btRank5OptionNum = lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[k].btRank5OptionNum;
					btRank5Level = lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[k].btRank5Level;
					break;
				}
			}

			if (btJewelPos)
				return FALSE;

			for (int l = 0; l < 254; l++)
			{
				if (Obj.m_PlayerData->m_PentagramJewelInfo_Inven[l].btJewelIndex == 0xFF)
				{
					Obj.m_PlayerData->m_PentagramJewelInfo_Inven[l].btJewelPos = 0;
					Obj.m_PlayerData->m_PentagramJewelInfo_Inven[l].btJewelIndex = btTargetDBJewelIndex;
					Obj.m_PlayerData->m_PentagramJewelInfo_Inven[l].btItemType = btItemType;
					Obj.m_PlayerData->m_PentagramJewelInfo_Inven[l].wItemIndex = wItemIndex;
					Obj.m_PlayerData->m_PentagramJewelInfo_Inven[l].btMainAttribute = btMainAttribute;
					Obj.m_PlayerData->m_PentagramJewelInfo_Inven[l].btLevel = btLevel;
					Obj.m_PlayerData->m_PentagramJewelInfo_Inven[l].btRank1OptionNum = btRank1OptionNum;
					Obj.m_PlayerData->m_PentagramJewelInfo_Inven[l].btRank1Level = btRank1Level;
					Obj.m_PlayerData->m_PentagramJewelInfo_Inven[l].btRank2OptionNum = btRank2OptionNum;
					Obj.m_PlayerData->m_PentagramJewelInfo_Inven[l].btRank2Level = btRank2Level;
					Obj.m_PlayerData->m_PentagramJewelInfo_Inven[l].btRank3OptionNum = btRank3OptionNum;
					Obj.m_PlayerData->m_PentagramJewelInfo_Inven[l].btRank3Level = btRank3Level;
					Obj.m_PlayerData->m_PentagramJewelInfo_Inven[l].btRank4OptionNum = btRank4OptionNum;
					Obj.m_PlayerData->m_PentagramJewelInfo_Inven[l].btRank4Level = btRank4Level;
					Obj.m_PlayerData->m_PentagramJewelInfo_Inven[l].btRank5OptionNum = btRank5OptionNum;
					Obj.m_PlayerData->m_PentagramJewelInfo_Inven[l].btRank5Level = btRank5Level;
					break;
				}
			}

			g_PentagramSystem.DBREQ_InsertPentagramJewel(Obj, btJewelPos, btTargetDBJewelIndex,
				btItemType, wItemIndex, btMainAttribute, btLevel, btRank1OptionNum, btRank1Level,
				btRank2OptionNum, btRank2Level, btRank3OptionNum, btRank3Level,
				btRank4OptionNum, btRank4Level, btRank5OptionNum, btRank5Level);

			sLog->outBasic("[TransPentagramJewelInfo][%s][%s] PentagramJewel DB Save : DBIndex = %d, ItemType = %d, ItemIndex = %d, MainAttribute = %d, 1RankN/L = %d/%d, 2RankN/L = %d/%d, 3RankN/L = %d/%d, 4RankN/L = %d/%d, 5RankN/L = %d/%d ",
				Obj.AccountID, Obj.Name, btTargetDBJewelIndex, btItemType, wItemIndex, btMainAttribute, btRank1OptionNum, btRank1Level,
				btRank2OptionNum, btRank2Level, btRank3OptionNum, btRank3Level, btRank4OptionNum, btRank4Level, btRank5OptionNum, btRank5Level);

			for (int m = 0; m < 254; m++)
			{
				if (lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[m].btJewelIndex == sourceItem.m_SocketOption[i])
				{
					lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[m].btJewelPos = 0;
					lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[m].btJewelIndex = -1;
					lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[m].btItemType = -1;
					lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[m].wItemIndex = -1;
					lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[m].btMainAttribute = -1;
					lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[m].btLevel = 0;
					lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[m].btRank1OptionNum = -1;
					lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[m].btRank1Level = -1;
					lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[m].btRank2OptionNum = -1;
					lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[m].btRank2Level = -1;
					lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[m].btRank3OptionNum = -1;
					lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[m].btRank3Level = -1;
					lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[m].btRank4OptionNum = -1;
					lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[m].btRank4Level = -1;
					lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[m].btRank5OptionNum = -1;
					lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[m].btRank5Level = -1;
					break;
				}
			}

			g_PentagramSystem.DBREQ_DelPentagramJewel(lpTargetObj, 0, sourceItem.m_SocketOption[i]);
		}
	}

	if (bSave)
	{
		g_PentagramSystem.GCPentagramJewelInfo(Obj, 0);
	}
}

WING_TYPE GetWingType(WORD type, WING_CHECK_FLAG eFlag)
{
	if (eFlag == WING_CHECK_FLAG::WCF_ITEMNUMBER)
	{
		if (type == ITEMGET(12, 0) || type == ITEMGET(12, 1) || type == ITEMGET(12, 2) || type == ITEMGET(12, 41))
		{
			return WING_TYPE::FIRST_WING;
		}

		else if (type == ITEMGET(12, 3) || type == ITEMGET(12, 4) || type == ITEMGET(12, 5) || type == ITEMGET(12, 6)
			|| type == ITEMGET(13, 30) || type == ITEMGET(12, 42) || type == ITEMGET(12, 49) || type == ITEMGET(12, 269))
		{
			return WING_TYPE::SECOND_WING;
		}

		else if (type == ITEMGET(12, 36) || type == ITEMGET(12, 37) || type == ITEMGET(12, 38) || type == ITEMGET(12, 39) || type == ITEMGET(12, 40) ||
			type == ITEMGET(12, 43) || type == ITEMGET(12, 50) || type == ITEMGET(12, 268) || type == ITEMGET(12, 270))
		{
			return WING_TYPE::THIRD_WING;
		}

		else if ((type >= ITEMGET(12, 130) && type <= ITEMGET(12, 135)) || type == ITEMGET(12, 278))
		{
			return WING_TYPE::LITTLE_WING;
		}

		else if (type >= ITEMGET(12, 262) && type <= ITEMGET(12, 265))
		{
			return WING_TYPE::MONSTER_WRING;
		}

		else if (type == ITEMGET(12, 266))
		{
			return WING_TYPE::CHAOSCASTLE_WING;
		}

		else if (type == ITEMGET(12, 267))
		{
			return WING_TYPE::GOODANDDEVIL_WING;
		}

		else
		{
			return WING_TYPE::NONE_WING;
		}
	}

	else
	{
		if (type == 0 || type == 1 || type == 2 || type == 41)
		{
			return WING_TYPE::FIRST_WING;
		}

		else if (type == 3 || type == 4 || type == 5 || type == 6 || type == 30 || type == 42 || type == 49 || type == 269)
		{
			return WING_TYPE::SECOND_WING;
		}

		else if (type == 36 || type == 37 || type == 38 || type == 39 || type == 40 || type == 43 || type == 50 || type == 268 || type == 270)
		{
			return WING_TYPE::THIRD_WING;
		}

		else if ((type >= 130 && type <= 135) || type == 278)
		{
			return WING_TYPE::LITTLE_WING;
		}

		else if (type >= 262 && type <= 265)
		{
			return WING_TYPE::MONSTER_WRING;
		}

		else if (type == 266)
		{
			return WING_TYPE::CHAOSCASTLE_WING;
		}

		else if (type == 267)
		{
			return WING_TYPE::GOODANDDEVIL_WING;
		}

		else
		{
			return WING_TYPE::NONE_WING;
		}
	}

	return WING_TYPE::NONE_WING;
}

void GetBundleCount(int count, int div, int& itemcount, int& left)
{
	itemcount = count / div;
	left = count % div;
}
int gObjGetJewelCountInInventory(CGameObject &Obj, BYTE type)
{
	
	int returnCount = 0;
	switch (type)
	{
	case 1: //Jewel of Bless
	{
		for (int i = 0; i < INVENTORY_SIZE; i++)
		{
			if (Obj.pntInventory[i]->m_Type == ITEMGET(12, 30))
			{
				switch (Obj.pntInventory[i]->m_Level)
				{
				case 0:
					returnCount += 10;
					break;
				case 1:
					returnCount += 20;
					break;
				case 2:
					returnCount += 30;
					break;
				}
			}
			if (Obj.pntInventory[i]->m_Type == ITEMGET(14, 13))
			{
				returnCount++;
			}
		}
		return returnCount;
	}
	break;
	case 2: // jewel of soul
	{
		for (int i = 0; i < INVENTORY_SIZE; i++)
		{
			if (Obj.pntInventory[i]->m_Type == ITEMGET(12, 31))
			{
				switch (Obj.pntInventory[i]->m_Level)
				{
				case 0:
					returnCount += 10;
					break;
				case 1:
					returnCount += 20;
					break;
				case 2:
					returnCount += 30;
					break;
				}
			}
			if (Obj.pntInventory[i]->m_Type == ITEMGET(14, 14))
			{
				returnCount++;
			}
		}
		return returnCount;
	}
	break;
	case 3: // Jewel of Chaos
	{
		for (int i = 0; i < INVENTORY_SIZE; i++)
		{
			if (Obj.pntInventory[i]->m_Type == ITEMGET(12, 141))
			{
				switch (Obj.pntInventory[i]->m_Level)
				{
				case 0:
					returnCount += 10;
					break;
				case 1:
					returnCount += 20;
					break;
				case 2:
					returnCount += 30;
					break;
				}
			}
			if (Obj.pntInventory[i]->m_Type == ITEMGET(12, 15))
			{
				returnCount++;
			}
		}
		return returnCount;
	}
	break;
	}
}

void gObjAutoPartySet(CGameObject &Obj /* master */, CGameObject &TargetObj /*requestor*/)
{
	if (Obj.Type != OBJ_USER || TargetObj.Type != OBJ_USER)
	{
		sLog->outError("error %d", __LINE__);
		return;
	}

	if (!gObjIsConnected(TargetObj) || !gObjIsConnected(Obj))
	{
		gGameProtocol.GCResultSend(TargetObj, 0x41, 0x03);
		return;
	}

	if (Obj.CloseCount >= 0 || TargetObj.CloseCount >= 0)
	{
		gGameProtocol.GCResultSend(TargetObj, 0x41, 0x00);
		return;
	}

	if (TargetObj.m_PlayerData->m_bUsePartyMatching)
	{
		gGameProtocol.GCResultSend(TargetObj, 0x41, 0x09);
		return;
	}

	if (g_NewPVP.IsDuel(Obj) || g_NewPVP.IsDuel(TargetObj))
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 315), TargetObj, 1);
		return;
	}

	if (g_NewPVP.IsObserver(TargetObj) || g_NewPVP.IsObserver(Obj))
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 316), TargetObj, 1);
		return;
	}

	if (CC_MAP_RANGE(Obj.m_Index) || CC_MAP_RANGE(TargetObj.m_Index))
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 116), TargetObj, 1);
		return;
	}

	if (Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 573), TargetObj, 1);
		return;
	}

	if (IT_MAP_RANGE(Obj.MapNumber) || IT_MAP_RANGE(TargetObj.MapNumber))
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 291), TargetObj, 1);
		return;
	}

	if (g_ArcaBattle.IsArcaBattleServer() && Obj.m_PlayerData->GuildNumber != Obj.m_PlayerData->GuildNumber)
	{
		if (g_AcheronGuardianEvent.IsPlayStart() == false)
		{
			gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 522), TargetObj, 1);
			return;
		}
	}

	if (g_GensSystem.IsMapBattleZone(Obj.MapNumber) || g_GensSystem.IsMapBattleZone(TargetObj.MapNumber))
	{
		if (g_GensSystem.CanGensCreatePartyOnBattleZone() == FALSE)
		{
			gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 533), TargetObj, 1);
			return;
		}
	}

	if (g_GensSystem.GetGensInfluence(Obj) != g_GensSystem.GetGensInfluence(TargetObj))
	{
		if (g_GensSystem.CanGensJoinPartyWhileOppositeGens() == FALSE)
		{
			gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 288), TargetObj, 1);
			return;
		}
	}

	if (TargetObj.PartyNumber >= 0)
	{
		return;
	}

	char szTemp[250];
	int pnumber = -1;
	if (Obj.PartyNumber < 0)
	{
		Obj.PartyNumber = gParty.Create(Obj, Obj.DBNumber, Obj.Level);
	}

	if (Obj.PartyNumber >= 0)
	{
		pnumber = Obj.PartyNumber;
		int iPartyPos = gParty.Add(Obj.PartyNumber, TargetObj, TargetObj.DBNumber, TargetObj.Level);

		if (iPartyPos >= 0)
		{
			TargetObj.PartyNumber = Obj.PartyNumber;
			wsprintf(szTemp, Lang.GetText(0, 20), TargetObj.Name);
			gGameProtocol.GCServerMsgStringSend(szTemp, Obj, 1);
			wsprintf(szTemp, Lang.GetText(0, 20), Obj.Name);
			gGameProtocol.GCServerMsgStringSend(szTemp, TargetObj, 1);
			gParty.Paint(pnumber);
		}
		else if (iPartyPos == -1)
		{
			gGameProtocol.GCResultSend(Obj, 0x41, 2);
			gGameProtocol.GCResultSend(TargetObj, 0x41, 2);
		}
		else if (iPartyPos == -2)
		{
			//result = 2;
			wsprintf(szTemp, Lang.GetText(0, 21), TargetObj.Name);
			gGameProtocol.GCServerMsgStringSend(szTemp, TargetObj, 1);
		}
	}

	if (pnumber >= 0)
	{
		gGameProtocol.CGPartyListAll(pnumber);
	}
}

BYTE gObjMuunInventoryInsertItemPos(CGameObject &Obj, CItemObject &item, int pos)
{
	if (pos < 0 || pos > MUUN_INVENTORY_SIZE)
	{
		return -1;
	}

	

	if (Obj.pntMuunInventory[pos]->IsItem() == TRUE)
	{
		return -1;
	}

	if (item.IsItem() == FALSE)
	{
		return -1;
	}

	Obj.pntMuunInventory[pos] = &item;
	return pos;
}

bool gObjFixMuunInventoryPointer(CGameObject &Obj)
{
	if (!gObjIsConnected(Obj))
	{
		sLog->outBasic("[Fix Muun Inv.Ptr] [%s][%s] - disconnected", Obj.AccountID, Obj.Name);
		return false;
	}

	if (Obj.pntMuunInventory == Obj.pntMuunInventory1)
	{
		return true;
	}

	if (Obj.pntMuunInventory == Obj.pntMuunInventory2)
	{
		if (Obj.pTransaction == 1)
		{
			sLog->outBasic("[Fix Muun Inv.Ptr] [%s][%s] - Transaction == 1, Do not change Pointer", Obj.AccountID, Obj.Name);
			return false;
		}

		sLog->outBasic("[Fix Muun Inv.Ptr] [%s][%s] - Muun Inventory Pointer was 2", Obj.AccountID, Obj.Name);

		for (int n = 0; n < MUUN_INVENTORY_SIZE; n++)
		{
			Obj.pntMuunInventory2[n]->Clear();
		}
	}

	else
	{
		sLog->outBasic("[Fix Muun Inv.Ptr] [%s][%s] - Muun Inventory Pointer was Wrong", Obj.AccountID, Obj.Name);
	}

	gObjSetMuunInventory1Pointer(Obj);
	return false;
}

void gObjSetMuunInventory1Pointer(CGameObject &Obj)
{
	Obj.pntMuunInventory = Obj.pntMuunInventory1;
}

void gObjSetMuunInventory2Pointer(CGameObject &Obj)
{
	Obj.pntMuunInventory = Obj.pntMuunInventory2;
}

BYTE gObjChkMuunInventoryEmpty(CGameObject &Obj)
{
	for (int i = 2; i < MUUN_INVENTORY_SIZE; ++i)
	{
		if (!Obj.pntMuunInventory[i]->IsItem())
			return i;
	}
	return -1;
}

BYTE gObjMuunInventoryInsertItem(CGameObject &Obj, CItemObject &item)
{
	CItemObject copyitem;
	int iwidth, iheight;

	if (item.GetSize(iwidth, iheight) == 0)
	{
		sLog->outBasic("error: Item does not exist %s %d", __FILE__, __LINE__);
		return -1;
	}

	BYTE Ret = gObjChkMuunInventoryEmpty(Obj);

	if (Ret == (BYTE)-1)
	{
		return (BYTE)-1;
	}

	if (Obj.pntMuunInventory[Ret]->IsItem() == TRUE)
	{
		return -1;
	}

	std::memcpy(&Obj.pntMuunInventory[Ret], &item, sizeof(Obj.pntMuunInventory[Ret]));

	if (g_CMuunSystem.IsMuunItem(item.m_Type) == TRUE && g_CMuunSystem.IsMuunExpireDate(item.m_Type) == FALSE)
	{
		g_CMuunSystem.SetMuunItemAddPeriodData(Obj, item.m_Type, item.m_Number);
	}

	return Ret;
}

BYTE gObjMuunInventoryInsertItem(CGameObject &Obj, CMapItem *item)
{
	int iwidth, iheight;
	BYTE blank = 0;

	CItemObject copyitem;

	if (item->GetSize((int &)iwidth, (int &)iheight) == 0)
	{
		return -1;
	}

	BYTE Ret = gObjChkMuunInventoryEmpty(Obj);

	if (Ret == 0xFF)
	{
		return -1;
	}

	if (Obj.pntMuunInventory[Ret]->IsItem() == TRUE)
	{
		return -1;
	}

	copyitem.m_Level = item->m_Level;
	copyitem.m_Durability = item->m_Durability;

	copyitem.Convert(item->m_Type, item->m_Option1, item->m_Option2, item->m_Option3, item->m_NewOption, item->m_SetOption, item->m_ItemOptionEx, item->m_SocketOption, item->m_BonusSocketOption, 0, 3);

	copyitem.m_Number = item->m_Number;

	Obj.pntMuunInventory[Ret] = &copyitem;

	if (g_CMuunSystem.IsMuunItem(item->m_Type) == TRUE && g_CMuunSystem.IsMuunExpireDate(item->m_Type) == FALSE)
	{
		g_CMuunSystem.SetMuunItemAddPeriodData(Obj, item->m_Type, item->m_Number);
	}

	return Ret;
}

BYTE gObjMuunInventoryDeleteItem(CGameObject &Obj, int itempos)
{
	g_CMuunSystem.ClearPeriodMuunItemData(Obj, Obj.pntMuunInventory[itempos]->m_Type, Obj.pntMuunInventory[itempos]->m_Number);
	Obj.pntMuunInventory[itempos]->Clear();
	return 1;
}

BYTE gObjMuunInvenMove(CGameObject &Obj, int *durSsend, int *durTsend, BYTE source, BYTE target, BYTE *siteminfo)
{
	if (source < 0 > source > MUUN_INVENTORY_SIZE - 1)
	{
		sLog->outBasic("error : %d", __LINE__);
		return -1;
	}

	if (target < 0 > target > MUUN_INVENTORY_SIZE - 1)
	{
		sLog->outBasic("error : %d", __LINE__);
		return -1;
	}

	if (source == target)
	{
		sLog->outBasic("error : %d", __LINE__);
		return -1;
	}

	if (Obj.pntMuunInventory[source]->IsItem() == FALSE)
	{
		return -1;
	}

	if (Obj.pntMuunInventory[target]->IsItem() == TRUE)
	{
		return -1;
	}

	if (target < 2)
	{
		if (g_CMuunSystem.IsMuunItem(Obj.pntMuunInventory[source]->m_Type) == FALSE)
		{
			return -1;
		}

		for (int i = 0; i < 2; i++)
		{
			if (Obj.pntMuunInventory[i]->IsItem() == TRUE && source != i)
			{
				int EvolutionMuunItemIndex = g_CMuunSystem.GetEvolutionMuunItemIndex(Obj.pntMuunInventory[i]->m_Type);

				if (EvolutionMuunItemIndex == -1)
				{
					int nBeforeMuunIndex = g_CMuunSystem.GetBeforeEvolutionMuunItemIndex(Obj.pntMuunInventory[i]->m_Type);

					if (Obj.pntMuunInventory[i]->m_Type == Obj.pntMuunInventory[source]->m_Type ||
						nBeforeMuunIndex == Obj.pntMuunInventory[source]->m_Type)
					{
						return -1;
					}
				}

				else if (Obj.pntMuunInventory[i]->m_Type == Obj.pntMuunInventory[source]->m_Type ||
					EvolutionMuunItemIndex == Obj.pntMuunInventory[source]->m_Type)
				{
					return -1;
				}
			}
		}
	}

	Obj.pntMuunInventory[target] = Obj.pntMuunInventory[source];
	Obj.pntMuunInventory[source]->Clear();

	ItemByteConvert(siteminfo, *Obj.pntMuunInventory[target]);

	if (target < 2 || source < 2)
	{
		LPITEM_ATTRIBUTE pItemAttribute = GetItemAttr(Obj.pntMuunInventory[target]->m_Type);

		if (!pItemAttribute)
		{
			return -1;
		}

		if (pItemAttribute->ItemKindA != 12)
		{
			return -1;
		}

		if (pItemAttribute->ItemKindA == 12 && !pItemAttribute->ItemKindB)
		{
			return -1;
		}

		g_CMuunSystem.MuunItemEquipment(Obj, target, source);

		if (target && !source)
		{
			gGameProtocol.GCMuunEquipmentChange(Obj, 0);
		}

		else
		{
			gGameProtocol.GCMuunEquipmentChange(Obj, 0);
		}
	}

	return 22;
}

int gObjMuunInventorySearchSerialNumber(CGameObject &Obj, UINT64 serial)
{
	int result;
	int count;

	UINT64 s_num;
	UINT64 s_numa;

	count = 0;
	if (g_ConfigRead.ItemSerialCheck == 0)
	{
		if (serial)
		{
			for (int n = 0; n < MUUN_INVENTORY_SIZE; ++n)
			{
				s_num = Obj.pntMuunInventory[n]->GetNumber();
				if (s_num)
				{
					if (s_num == serial)
						++count;
				}
			}
			if (count > 1)
			{
				for (int na = 0; na < MUUN_INVENTORY_SIZE; ++na)
				{
					s_numa = Obj.pntMuunInventory[na]->GetNumber();
					if (s_numa && s_numa == serial)
					{
						gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 565), Obj, 1);
						sLog->outBasic("MuunInventory CopyItem %s", Obj.AccountID);
						gObjUserKill(Obj);
					}
				}
				result = 0;
			}
			else
			{
				result = 1;
			}
		}
		else
		{
			result = 1;
		}
	}
	else
	{
		result = 1;
	}
	return result;
}

BOOL IsNotApplyJewelToItems(WORD sItemType)
{
	if (sItemType == ITEMGET(4, 7) || sItemType == ITEMGET(4, 15))
	{
		return TRUE;
	}

	if (sItemType == ITEMGET(0, 41))
	{
		return TRUE;
	}

	WING_TYPE enWingType = GetWingType(sItemType, WCF_ITEMNUMBER);

	if (enWingType != NONE_WING)
	{
		if (enWingType == CHAOSCASTLE_WING || enWingType == GOODANDDEVIL_WING)
		{
			return TRUE;
		}

		else
		{
			return FALSE;
		}
	}

	if (sItemType >= ITEMGET(12, 200) && sItemType <= ITEMGET(12, 220))
	{
		return FALSE;
	}

	if (sItemType >= ITEMGET(12, 306) && sItemType <= ITEMGET(12, 308))
	{
		return FALSE;
	}

	if (sItemType < ITEMGET(12, 0))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL UseBundleOfBlessJewel(CGameObject &Obj, int source, int target)
{
	if (source < 0 || source > MAIN_INVENTORY_SIZE)
	{
		return FALSE;
	}

	if (target < 0 || target > MAIN_INVENTORY_SIZE)
	{
		return FALSE;
	}

	if (Obj.pntInventory[source]->IsItem() == FALSE)
	{
		return FALSE;
	}

	if (Obj.pntInventory[target]->IsItem() == FALSE)
	{
		return FALSE;
	}

	if (g_MineSystem.IsPickax(Obj.pntInventory[target]->m_Type) == FALSE)
	{
		return FALSE;
	}

	CItemObject * ItemPickax = Obj.pntInventory[target];
	short sRepairValue = g_MineSystem.GetRepairValueOfJewel(Obj.pntInventory[source]->m_Type, Obj.pntInventory[source]->m_Level);

	if (sRepairValue < 0)
	{
		ItemPickax->m_Durability = 255.0;
	}

	if (ItemPickax->m_Durability + sRepairValue > 255.0)
	{
		return FALSE;
	}

	ItemPickax->m_Durability += sRepairValue;
	sLog->outBasic("[MineSystem][PICKAX][BUNDLE_REPAIR][SUCCESS][%s][%s][Type:%d][Index:%d][Level:%d][Serial:%I64d] - %d/255 (+%d)",
		Obj.AccountID, Obj.Name, Obj.pntInventory[source]->m_Type / 512, Obj.pntInventory[source]->m_Type % 512, Obj.pntInventory[source]->m_Level,
		Obj.pntInventory[source]->m_Number, (int)Obj.pntInventory[source]->m_Durability, sRepairValue);

	return TRUE;
}

BOOL IsExceptionJewelOfBlessInchant(CGameObject &Obj, int source, int target)
{
	if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 37))
	{
		CItemObject * ItemFenrir = Obj.pntInventory[target];

		if (ItemFenrir->m_Durability >= 255)
		{
			return false;
		}

		if (ItemFenrir->IsPeriodItem() == TRUE)
		{
			return false;
		}

		if (rand() % 10000 < g_iFenrirRepairRate)
		{
			int iAddDur = rand() % 150 + 50;

			if ((ItemFenrir->m_Durability + iAddDur) > 255)
			{
				ItemFenrir->m_Durability = 255.0f;
			}
			else
			{
				ItemFenrir->m_Durability += iAddDur;
			}

			MsgOutput(Obj, Lang.GetText(0, 257), int(ItemFenrir->m_Durability));

			sLog->outBasic("[FENRIR REPAIR][SUCCESS] [%s][%s] - %d/255 (+%d)", Obj.AccountID, Obj.Name, (int)ItemFenrir->m_Durability, iAddDur);
		}
		else
		{
			MsgOutput(Obj, Lang.GetText(0, 258));

			sLog->outBasic("[FENRIR REPAIR][FAILED] [%s][%s] - %d/255", Obj.AccountID, Obj.Name, (int)ItemFenrir->m_Durability);
		}

		return true;
	}

	else if (g_MineSystem.IsPickax(Obj.pntInventory[target]->m_Type) == TRUE)
	{
		CItemObject * ItemPickax = Obj.pntInventory[target];
		short sRepairValue = g_MineSystem.GetRepairValueOfJewel(Obj.pntInventory[source]->m_Type, Obj.pntInventory[source]->m_Level);

		if (sRepairValue < 0)
		{
			return FALSE;
		}

		if (ItemPickax->m_Durability + sRepairValue > 255.0)
		{
			return FALSE;
		}

		ItemPickax->m_Durability += sRepairValue;
		sLog->outBasic("[MineSystem][PICKAX][REPAIR][SUCCESS][%s][%s][Type:%d][Index:%d][Level:%d][Serial:%I64d] - %d/255 (+%d)",
			Obj.AccountID, Obj.Name, Obj.pntInventory[source]->m_Type / 512, Obj.pntInventory[source]->m_Type % 512, Obj.pntInventory[source]->m_Level,
			Obj.pntInventory[source]->m_Number, (int)Obj.pntInventory[source]->m_Durability, sRepairValue);

		return TRUE;
	}

	else if (Obj.pntInventory[target]->m_Type == ITEMGET(13, 268))
	{
		CItemObject * DSFRing = Obj.pntInventory[target];

		if (DSFRing->m_Durability >= 255)
		{
			return false;
		}

		if ((DSFRing->m_Durability + 50) > 255)
		{
			DSFRing->m_Durability = 255.0f;
		}
		else
		{
			DSFRing->m_Durability += 50;
		}

		sLog->outBasic("[DSF_REWARD][SUCCESS] [%s][%s] - %d/255 (+%d)", Obj.AccountID, Obj.Name, (int)DSFRing->m_Durability, 50);

		return TRUE;
	}

	return FALSE;
}

BOOL IsTransformCharacterSkin(int pChange)
{
	switch (pChange)
	{
	case 2:
	case 7:
	case 8:
	case 9:
	case 14:
	case 41:
	case 372:
	case 373:
	case 374:
	case 378:
	case 477:
	case 503:
	case 548:
	case 616:
	case 617:
	case 625:
	case 626:
	case 642:
	case 646:
	case 706:
		return TRUE;
	default:
		return FALSE;
	}
	return FALSE;
}

BOOL gCountBlessSoulChaosJewel(CGameObject &Obj, short & sBlessJewelCount, short & s10BlessJewelCount, short & s20BlessJewelCount, short & s30BlessJewelCount, short & sSoulJewelCount, short & s10SoulJewelCount, short & s20SoulJewelCount, short & s30SoulJewelCount, short & sChaosJewelCount, short & s10ChaosJewelCount, short & s20ChaosJewelCount, short & s30ChaosJewelCount)
{
	if (!gObjIsConnected(Obj))
	{
		return FALSE;
	}

	for (int n = 0; n < MAIN_INVENTORY_SIZE; n++)
	{
		if (Obj.pntInventory[n]->IsItem() == TRUE)
		{
			switch (Obj.pntInventory[n]->m_Type)
			{
			case ITEMGET(14, 13):
				sBlessJewelCount++;
				break;
			case ITEMGET(12, 30):
				if (Obj.pntInventory[n]->m_Level == 0)
				{
					s10BlessJewelCount++;
				}
				else if (Obj.pntInventory[n]->m_Level == 1)
				{
					s20BlessJewelCount++;
				}
				else if (Obj.pntInventory[n]->m_Level == 2)
				{
					s30BlessJewelCount++;
				}
				break;
			case ITEMGET(14, 14):
				sSoulJewelCount++;
				break;
			case ITEMGET(12, 31):
				if (Obj.pntInventory[n]->m_Level == 0)
				{
					s10SoulJewelCount++;
				}
				else if (Obj.pntInventory[n]->m_Level == 1)
				{
					s20SoulJewelCount++;
				}
				else if (Obj.pntInventory[n]->m_Level == 2)
				{
					s30SoulJewelCount++;
				}
				break;
			case ITEMGET(12, 15):
				sChaosJewelCount++;
				break;
			case ITEMGET(12, 141):
				if (Obj.pntInventory[n]->m_Level == 0)
				{
					s10ChaosJewelCount++;
				}
				else if (Obj.pntInventory[n]->m_Level == 1)
				{
					s20ChaosJewelCount++;
				}
				else if (Obj.pntInventory[n]->m_Level == 2)
				{
					s30ChaosJewelCount++;
				}
				break;
			}
		}
	}

	return TRUE;
}

BOOL gJewelBuyItemValueTest(CGameObject &ObjBuyer, CGameObject &ObjSeller, short sBlessJewelPrice, short sSoulJewelPrice, short sChaosJewelPrice, short sBlessJewelCount, short s10BlessJewelCount, short s20BlessJewelCount, short s30BlessJewelCount, short sSoulJewelCount, short s10SoulJewelCount, short s20SoulJewelCount, short s30SoulJewelCount, short sChaosJewelCount, short s10ChaosJewelCount, short s20ChaosJewelCount, short s30ChaosJewelCount,
	short &sBlessJewelNeedCount, short &s10BlessJewelNeedCount, short &s20BlessJewelNeedCount, short &s30BlessJewelNeedCount, short &sSoulJewelNeedCount, short &s10SoulJewelNeedCount, short &s20SoulJewelNeedCount, short &s30SoulJewelNeedCount, short &sChaosJewelNeedCount, short &s10ChaosJewelNeedCount, short &s20ChaosJewelNeedCount, short &s30ChaosJewelNeedCount)
{
	if (!gObjIsConnected(ObjBuyer))
	{
		return FALSE;
	}

	if (!gObjIsConnected(ObjSeller))
	{
		return FALSE;
	}

	short sBPrice = sBlessJewelPrice;
	short sSPrice = sSoulJewelPrice;
	short sCPrice = sChaosJewelPrice;

	BOOL bMinus30BJ = FALSE;
	BOOL bMinus30SJ = FALSE;
	BOOL bMinus30CJ = FALSE;

	short sChaosNeedCount;
	short s10ChaosNeedCount;
	short s20ChaosNeedCount;
	short s30ChaosNeedCount;
	short sSoulNeedCount;
	short s10SoulNeedCount;
	short s20SoulNeedCount;
	short s30SoulNeedCount;
	short sBlessNeedCount;
	short s10BlessNeedCount;
	short s20BlessNeedCount;
	short s30BlessNeedCount;

	s30BlessNeedCount = sBlessJewelPrice / 30;
	if (s30BlessNeedCount >= s30BlessJewelCount)
	{
		s30BlessNeedCount = s30BlessJewelCount;
	}

	while (true)
	{
		sBPrice = sBPrice - 30 * s30BlessNeedCount;
		s20BlessNeedCount = sBPrice / 20;

		if (s20BlessNeedCount >= s20BlessJewelCount)
			s20BlessNeedCount = s20BlessJewelCount;

		sBPrice = sBPrice - 20 * s20BlessNeedCount;
		s10BlessNeedCount = sBPrice / 10;

		if (s10BlessNeedCount >= s10BlessJewelCount)
			s10BlessNeedCount = s10BlessJewelCount;

		sBPrice = sBPrice - 10 * s10BlessNeedCount;
		sBlessNeedCount = sBPrice;

		if (sBlessNeedCount >= sBlessJewelCount)
			sBlessNeedCount = sBlessJewelCount;

		if ((sBPrice - sBlessNeedCount) <= 0)
			break;

		if (s30BlessNeedCount <= 0 || bMinus30BJ)
			return 14;

		s30BlessNeedCount--;
		sBPrice = sBlessJewelPrice;
		bMinus30BJ = TRUE;
	}

	s30SoulNeedCount = sSoulJewelPrice / 30;
	if (s30SoulNeedCount >= s30SoulJewelCount)
	{
		s30SoulNeedCount = s30SoulJewelCount;
	}

	while (true)
	{
		sSPrice = sSPrice - 30 * s30SoulNeedCount;
		s20SoulNeedCount = sSPrice / 20;

		if (s20SoulNeedCount >= s20SoulJewelCount)
			s20SoulNeedCount = s20SoulJewelCount;

		sSPrice = sSPrice - 20 * s20SoulNeedCount;
		s10SoulNeedCount = sSPrice / 10;

		if (s10SoulNeedCount >= s10SoulJewelCount)
			s10SoulNeedCount = s10SoulJewelCount;

		sSPrice = sSPrice - 10 * s10SoulNeedCount;
		sSoulNeedCount = sSPrice;

		if (sSoulNeedCount >= sSoulJewelCount)
			sSoulNeedCount = sSoulJewelCount;

		if ((sSPrice - sSoulNeedCount) <= 0)
			break;

		if (s30SoulNeedCount <= 0 || bMinus30SJ)
			return 15;

		s30SoulNeedCount--;
		sSPrice = sSoulJewelPrice;
		bMinus30SJ = TRUE;
	}

	s30ChaosNeedCount = sChaosJewelPrice / 30;
	if (s30ChaosNeedCount >= s30ChaosJewelCount)
	{
		s30ChaosNeedCount = s30ChaosJewelCount;
	}

	while (true)
	{
		sCPrice = sCPrice - 30 * s30ChaosNeedCount;
		s20ChaosNeedCount = sCPrice / 20;

		if (s20ChaosNeedCount >= s20ChaosJewelCount)
			s20ChaosNeedCount = s20ChaosJewelCount;

		sCPrice = sCPrice - 20 * s20ChaosNeedCount;
		s10ChaosNeedCount = sCPrice / 10;

		if (s10ChaosNeedCount >= s10ChaosJewelCount)
			s10ChaosNeedCount = s10ChaosJewelCount;

		sCPrice = sCPrice - 10 * s10ChaosNeedCount;
		sChaosNeedCount = sCPrice;

		if (sChaosNeedCount >= sChaosJewelCount)
			sChaosNeedCount = sChaosJewelCount;

		if ((sCPrice - sChaosNeedCount) <= 0)
			break;

		if (s30ChaosNeedCount <= 0 || bMinus30CJ)
			return 16;

		s30ChaosNeedCount--;
		sCPrice = sChaosJewelPrice;
		bMinus30CJ = TRUE;
	}

	sBlessJewelNeedCount = sBlessNeedCount;
	s10BlessJewelNeedCount = s10BlessNeedCount;
	s20BlessJewelNeedCount = s20BlessNeedCount;
	s30BlessJewelNeedCount = s30BlessNeedCount;
	sSoulJewelNeedCount = sSoulNeedCount;
	s10SoulJewelNeedCount = s10SoulNeedCount;
	s20SoulJewelNeedCount = s20SoulNeedCount;
	s30SoulJewelNeedCount = s30SoulNeedCount;
	sChaosJewelNeedCount = sChaosNeedCount;
	s10ChaosJewelNeedCount = s10ChaosNeedCount;
	s20ChaosJewelNeedCount = s20ChaosNeedCount;
	s30ChaosJewelNeedCount = s30ChaosNeedCount;

	return FALSE;
}

BOOL gJewelInventoryPutTest(CGameObject &ObjBuyer, CGameObject &ObjSeller, short sBlessJewelPrice, short sSoulJewelPrice, short sChaosJewelPrice)
{
	if (!gObjIsConnected(ObjBuyer))
	{
		return FALSE;
	}

	if (!gObjIsConnected(ObjSeller))
	{
		return FALSE;
	}

	int iBJCommisionCount = round(sBlessJewelPrice * 0.03);
	int iSJCommisionCount = round(sSoulJewelPrice * 0.03);
	int iCJCommisionCount = round(sChaosJewelPrice * 0.03);

	if (iBJCommisionCount > 50)
		iBJCommisionCount = 50;

	if (iSJCommisionCount > 50)
		iSJCommisionCount = 50;

	if (iCJCommisionCount > 50)
		iCJCommisionCount = 50;

	if (CheckInventoryEmptySpaceCount(ObjSeller, 1, 1) >=
		((sChaosJewelPrice - iCJCommisionCount) % 30 % 20 % 10
			+ (sChaosJewelPrice - iCJCommisionCount) % 30 % 20 / 10
			+ (sChaosJewelPrice - iCJCommisionCount) % 30 / 20
			+ (sChaosJewelPrice - iCJCommisionCount) / 30
			+ (sSoulJewelPrice - iSJCommisionCount) % 30 % 20 % 10
			+ (sSoulJewelPrice - iSJCommisionCount) % 30 % 20 / 10
			+ (sSoulJewelPrice - iSJCommisionCount) % 30 / 20
			+ (sSoulJewelPrice - iSJCommisionCount) / 30
			+ (sBlessJewelPrice - iBJCommisionCount) % 30 % 20 % 10
			+ (sBlessJewelPrice - iBJCommisionCount) % 30 % 20 / 10
			+ (sBlessJewelPrice - iBJCommisionCount) % 30 / 20
			+ (sBlessJewelPrice - iBJCommisionCount) / 30))
	{
		return TRUE;
	}

	else
	{
		return 100;
	}
}

BOOL gJewelInventoryPut(CGameObject &ObjBuyer, CGameObject &ObjSeller, short sBlessJewelPrice, short sSoulJewelPrice, short sChaosJewelPrice, short sBlessJewelNeedCount, short s10BlessJewelNeedCount, short s20BlessJewelNeedCount, short s30BlessJewelNeedCount, short sSoulJewelNeedCount, short s10SoulJewelNeedCount, short s20SoulJewelNeedCount, short s30SoulJewelNeedCount, short sChaosJewelNeedCount, short s10ChaosJewelNeedCount, short s20ChaosJewelNeedCount, short s30ChaosJewelNeedCount)
{
	if (!gObjIsConnected(ObjBuyer))
	{
		return FALSE;
	}

	if (!gObjIsConnected(ObjSeller))
	{
		return FALSE;
	}

	short sBJNeedCount = sBlessJewelNeedCount;
	short s10BJNeedCount = s10BlessJewelNeedCount;
	short s20BJNeedCount = s20BlessJewelNeedCount;
	short s30BJNeedCount = s30BlessJewelNeedCount;
	short sSJNeedCount = sSoulJewelNeedCount;
	short s10SJNeedCount = s10SoulJewelNeedCount;
	short s20SJNeedCount = s20SoulJewelNeedCount;
	short s30SJNeedCount = s30SoulJewelNeedCount;
	short sCJNeedCount = sChaosJewelNeedCount;
	short s10CJNeedCount = s10ChaosJewelNeedCount;
	short s20CJNeedCount = s20ChaosJewelNeedCount;
	short s30CJNeedCount = s30ChaosJewelNeedCount;

	for (int n = 0; n < 204; n++)
	{
		if (ObjBuyer.pntInventory[n]->IsItem() == TRUE)
		{
			if (ObjBuyer.pntInventory[n]->m_Type == ITEMGET(12, 30))
			{
				if (ObjBuyer.pntInventory[n]->m_Level == 2)
				{
					if (s30BJNeedCount > 0)
					{
						gObjInventoryItemSet(ObjBuyer, n, -1);
						ObjBuyer.pntInventory[n]->Clear();
						gGameProtocol.GCInventoryItemDeleteSend(ObjBuyer, n, TRUE);
						s30BJNeedCount--;
					}
				}
			}

			if (ObjBuyer.pntInventory[n]->m_Type == ITEMGET(12, 31))
			{
				if (ObjBuyer.pntInventory[n]->m_Level == 2)
				{
					if (s30SJNeedCount > 0)
					{
						gObjInventoryItemSet(ObjBuyer, n, -1);
						ObjBuyer.pntInventory[n]->Clear();
						gGameProtocol.GCInventoryItemDeleteSend(ObjBuyer, n, TRUE);
						s30SJNeedCount--;
					}
				}
			}

			if (ObjBuyer.pntInventory[n]->m_Type == ITEMGET(12, 141))
			{
				if (ObjBuyer.pntInventory[n]->m_Level == 2)
				{
					if (s30CJNeedCount > 0)
					{
						gObjInventoryItemSet(ObjBuyer, n, -1);
						ObjBuyer.pntInventory[n]->Clear();
						gGameProtocol.GCInventoryItemDeleteSend(ObjBuyer, n, TRUE);
						s30CJNeedCount--;
					}
				}
			}
		}
	}

	for (int n = 0; n < 204; n++)
	{
		if (ObjBuyer.pntInventory[n]->IsItem() == TRUE)
		{
			if (ObjBuyer.pntInventory[n]->m_Type == ITEMGET(12, 30))
			{
				if (ObjBuyer.pntInventory[n]->m_Level == 1)
				{
					if (s20BJNeedCount > 0)
					{
						gObjInventoryItemSet(ObjBuyer, n, -1);
						ObjBuyer.pntInventory[n]->Clear();
						gGameProtocol.GCInventoryItemDeleteSend(ObjBuyer, n, TRUE);
						s20BJNeedCount--;
					}
				}
			}

			if (ObjBuyer.pntInventory[n]->m_Type == ITEMGET(12, 31))
			{
				if (ObjBuyer.pntInventory[n]->m_Level == 1)
				{
					if (s20SJNeedCount > 0)
					{
						gObjInventoryItemSet(ObjBuyer, n, -1);
						ObjBuyer.pntInventory[n]->Clear();
						gGameProtocol.GCInventoryItemDeleteSend(ObjBuyer, n, TRUE);
						s20SJNeedCount--;
					}
				}
			}

			if (ObjBuyer.pntInventory[n]->m_Type == ITEMGET(12, 141))
			{
				if (ObjBuyer.pntInventory[n]->m_Level == 1)
				{
					if (s20CJNeedCount > 0)
					{
						gObjInventoryItemSet(ObjBuyer, n, -1);
						ObjBuyer.pntInventory[n]->Clear();
						gGameProtocol.GCInventoryItemDeleteSend(ObjBuyer, n, TRUE);
						s20CJNeedCount--;
					}
				}
			}
		}
	}

	for (int n = 0; n < 204; n++)
	{
		if (ObjBuyer.pntInventory[n]->IsItem() == TRUE)
		{
			if (ObjBuyer.pntInventory[n]->m_Type == ITEMGET(12, 30))
			{
				if (ObjBuyer.pntInventory[n]->m_Level == 0)
				{
					if (s10BJNeedCount > 0)
					{
						gObjInventoryItemSet(ObjBuyer, n, -1);
						ObjBuyer.pntInventory[n]->Clear();
						gGameProtocol.GCInventoryItemDeleteSend(ObjBuyer, n, TRUE);
						s10BJNeedCount--;
					}
				}
			}

			if (ObjBuyer.pntInventory[n]->m_Type == ITEMGET(12, 31))
			{
				if (ObjBuyer.pntInventory[n]->m_Level == 0)
				{
					if (s10SJNeedCount > 0)
					{
						gObjInventoryItemSet(ObjBuyer, n, -1);
						ObjBuyer.pntInventory[n]->Clear();
						gGameProtocol.GCInventoryItemDeleteSend(ObjBuyer, n, TRUE);
						s10SJNeedCount--;
					}
				}
			}

			if (ObjBuyer.pntInventory[n]->m_Type == ITEMGET(12, 141))
			{
				if (ObjBuyer.pntInventory[n]->m_Level == 0)
				{
					if (s10CJNeedCount > 0)
					{
						gObjInventoryItemSet(ObjBuyer, n, -1);
						ObjBuyer.pntInventory[n]->Clear();
						gGameProtocol.GCInventoryItemDeleteSend(ObjBuyer, n, TRUE);
						s10CJNeedCount--;
					}
				}
			}
		}
	}

	for (int n = 0; n < 204; n++)
	{
		if (ObjBuyer.pntInventory[n]->IsItem() == TRUE)
		{
			if (ObjBuyer.pntInventory[n]->m_Type == ITEMGET(14, 13))
			{
				if (sBJNeedCount > 0)
				{
					gObjInventoryItemSet(ObjBuyer, n, -1);
					ObjBuyer.pntInventory[n]->Clear();
					gGameProtocol.GCInventoryItemDeleteSend(ObjBuyer, n, TRUE);
					sBJNeedCount--;
				}
			}

			if (ObjBuyer.pntInventory[n]->m_Type == ITEMGET(14, 14))
			{
				if (sSJNeedCount > 0)
				{
					gObjInventoryItemSet(ObjBuyer, n, -1);
					ObjBuyer.pntInventory[n]->Clear();
					gGameProtocol.GCInventoryItemDeleteSend(ObjBuyer, n, TRUE);
					sSJNeedCount--;
				}
			}

			if (ObjBuyer.pntInventory[n]->m_Type == ITEMGET(12, 15))
			{
				if (sCJNeedCount > 0)
				{
					gObjInventoryItemSet(ObjBuyer, n, -1);
					ObjBuyer.pntInventory[n]->Clear();
					gGameProtocol.GCInventoryItemDeleteSend(ObjBuyer, n, TRUE);
					sCJNeedCount--;
				}
			}
		}
	}

	int iBJCommisionCount = round(sBlessJewelPrice * 0.03);
	int iSJCommisionCount = round(sSoulJewelPrice * 0.03);
	int iCJCommisionCount = round(sChaosJewelPrice * 0.03);

	if (iBJCommisionCount > 50)
		iBJCommisionCount = 50;

	if (iSJCommisionCount > 50)
		iSJCommisionCount = 50;

	if (iCJCommisionCount > 50)
		iCJCommisionCount = 50;

	short s30BlessCount = (sBlessJewelPrice - iBJCommisionCount) / 30;
	short s20BlessCount = (sBlessJewelPrice - iBJCommisionCount) % 30 / 20;
	short s10BlessCount = (sBlessJewelPrice - iBJCommisionCount) % 30 % 20 / 10;
	short sBlessCount = (sBlessJewelPrice - iBJCommisionCount) % 30 % 20 % 10;

	short s30SoulCount = (sSoulJewelPrice - iSJCommisionCount) / 30;
	short s20SoulCount = (sSoulJewelPrice - iSJCommisionCount) % 30 / 20;
	short s10SoulCount = (sSoulJewelPrice - iSJCommisionCount) % 30 % 20 / 10;
	short sSoulCount = (sSoulJewelPrice - iSJCommisionCount) % 30 % 20 % 10;

	short s30ChaosCount = (sChaosJewelPrice - iCJCommisionCount) / 30;
	short s20ChaosCount = (sChaosJewelPrice - iCJCommisionCount) % 30 / 20;
	short s10ChaosCount = (sChaosJewelPrice - iCJCommisionCount) % 30 % 20 / 10;
	short sChaosCount = (sChaosJewelPrice - iCJCommisionCount) % 30 % 20 % 10;

	if (s30BlessCount > 0)
	{
		for (int i30B = 0; i30B < s30BlessCount; i30B++)
		{
			ItemCreate(ObjSeller, 235, ObjSeller.X, ObjSeller.Y, ITEMGET(12, 30), 2, 0, 0, 0, 0, ObjSeller.m_Index, 0, 0, 0, 0, 0);

			sLog->outBasic("[PShop][Create Jewel Put Seller Inven] - User(ID:%s,Name:%s) Item(Name:%s,Code:%d,level:%d)",
				ObjSeller.AccountID, ObjSeller.Name, ItemAttribute[ITEMGET(12, 30)].Name, ITEMGET(12, 30), 2);
		}
	}

	if (s20BlessCount > 0)
	{
		for (int i20B = 0; i20B < s20BlessCount; i20B++)
		{
			ItemCreate(ObjSeller, 235, ObjSeller.X, ObjSeller.Y, ITEMGET(12, 30), 1, 0, 0, 0, 0, ObjSeller.m_Index, 0, 0, 0, 0, 0);

			sLog->outBasic("[PShop][Create Jewel Put Seller Inven] - User(ID:%s,Name:%s) Item(Name:%s,Code:%d,level:%d)",
				ObjSeller.AccountID, ObjSeller.Name, ItemAttribute[ITEMGET(12, 30)].Name, ITEMGET(12, 30), 1);
		}
	}

	if (s10BlessCount > 0)
	{
		for (int i10B = 0; i10B < s10BlessCount; i10B++)
		{
			ItemCreate(ObjSeller, 235, ObjSeller.X, ObjSeller.Y, ITEMGET(12, 30), 0, 0, 0, 0, 0, ObjSeller.m_Index, 0, 0, 0, 0, 0);

			sLog->outBasic("[PShop][Create Jewel Put Seller Inven] - User(ID:%s,Name:%s) Item(Name:%s,Code:%d,level:%d)",
				ObjSeller.AccountID, ObjSeller.Name, ItemAttribute[ITEMGET(12, 30)].Name, ITEMGET(12, 30), 0);
		}
	}

	if (sBlessCount > 0)
	{
		for (int iB = 0; iB < sBlessCount; iB++)
		{
			ItemCreate(ObjSeller, 235, ObjSeller.X, ObjSeller.Y, ITEMGET(14, 13), 0, 0, 0, 0, 0, ObjSeller.m_Index, 0, 0, 0, 0, 0);

			sLog->outBasic("[PShop][Create Jewel Put Seller Inven] - User(ID:%s,Name:%s) Item(Name:%s,Code:%d,level:%d)",
				ObjSeller.AccountID, ObjSeller.Name, ItemAttribute[ITEMGET(14, 13)].Name, ITEMGET(14, 13), 0);
		}
	}

	if (s30SoulCount > 0)
	{
		for (int i30S = 0; i30S < s30SoulCount; i30S++)
		{
			ItemCreate(ObjSeller, 235, ObjSeller.X, ObjSeller.Y, ITEMGET(12, 31), 2, 0, 0, 0, 0, ObjSeller.m_Index, 0, 0, 0, 0, 0);

			sLog->outBasic("[PShop][Create Jewel Put Seller Inven] - User(ID:%s,Name:%s) Item(Name:%s,Code:%d,level:%d)",
				ObjSeller.AccountID, ObjSeller.Name, ItemAttribute[ITEMGET(12, 31)].Name, ITEMGET(12, 31), 2);
		}
	}

	if (s20SoulCount > 0)
	{
		for (int i20S = 0; i20S < s20SoulCount; i20S++)
		{
			ItemCreate(ObjSeller, 235, ObjSeller.X, ObjSeller.Y, ITEMGET(12, 31), 1, 0, 0, 0, 0, ObjSeller.m_Index, 0, 0, 0, 0, 0);

			sLog->outBasic("[PShop][Create Jewel Put Seller Inven] - User(ID:%s,Name:%s) Item(Name:%s,Code:%d,level:%d)",
				ObjSeller.AccountID, ObjSeller.Name, ItemAttribute[ITEMGET(12, 31)].Name, ITEMGET(12, 31), 1);
		}
	}

	if (s10SoulCount > 0)
	{
		for (int i10S = 0; i10S < s10SoulCount; i10S++)
		{
			ItemCreate(ObjSeller, 235, ObjSeller.X, ObjSeller.Y, ITEMGET(12, 31), 0, 0, 0, 0, 0, ObjSeller.m_Index, 0, 0, 0, 0, 0);

			sLog->outBasic("[PShop][Create Jewel Put Seller Inven] - User(ID:%s,Name:%s) Item(Name:%s,Code:%d,level:%d)",
				ObjSeller.AccountID, ObjSeller.Name, ItemAttribute[ITEMGET(12, 31)].Name, ITEMGET(12, 31), 0);
		}
	}

	if (sSoulCount > 0)
	{
		for (int iS = 0; iS < sSoulCount; iS++)
		{
			ItemCreate(ObjSeller, 235, ObjSeller.X, ObjSeller.Y, ITEMGET(14, 14), 0, 0, 0, 0, 0, ObjSeller.m_Index, 0, 0, 0, 0, 0);

			sLog->outBasic("[PShop][Create Jewel Put Seller Inven] - User(ID:%s,Name:%s) Item(Name:%s,Code:%d,level:%d)",
				ObjSeller.AccountID, ObjSeller.Name, ItemAttribute[ITEMGET(14, 14)].Name, ITEMGET(14, 14), 0);
		}
	}

	if (s30ChaosCount > 0)
	{
		for (int i30C = 0; i30C < s30ChaosCount; i30C++)
		{
			ItemCreate(ObjSeller, 235, ObjSeller.X, ObjSeller.Y, ITEMGET(12, 141), 2, 0, 0, 0, 0, ObjSeller.m_Index, 0, 0, 0, 0, 0);

			sLog->outBasic("[PShop][Create Jewel Put Seller Inven] - User(ID:%s,Name:%s) Item(Name:%s,Code:%d,level:%d)",
				ObjSeller.AccountID, ObjSeller.Name, ItemAttribute[ITEMGET(12, 141)].Name, ITEMGET(12, 141), 2);
		}
	}

	if (s20ChaosCount > 0)
	{
		for (int i20C = 0; i20C < s20ChaosCount; i20C++)
		{
			ItemCreate(ObjSeller, 235, ObjSeller.X, ObjSeller.Y, ITEMGET(12, 141), 1, 0, 0, 0, 0, ObjSeller.m_Index, 0, 0, 0, 0, 0);

			sLog->outBasic("[PShop][Create Jewel Put Seller Inven] - User(ID:%s,Name:%s) Item(Name:%s,Code:%d,level:%d)",
				ObjSeller.AccountID, ObjSeller.Name, ItemAttribute[ITEMGET(12, 141)].Name, ITEMGET(12, 141), 1);
		}
	}

	if (s10ChaosCount > 0)
	{
		for (int i10C = 0; i10C < s10ChaosCount; i10C++)
		{
			ItemCreate(ObjSeller, 235, ObjSeller.X, ObjSeller.Y, ITEMGET(12, 141), 0, 0, 0, 0, 0, ObjSeller.m_Index, 0, 0, 0, 0, 0);

			sLog->outBasic("[PShop][Create Jewel Put Seller Inven] - User(ID:%s,Name:%s) Item(Name:%s,Code:%d,level:%d)",
				ObjSeller.AccountID, ObjSeller.Name, ItemAttribute[ITEMGET(12, 141)].Name, ITEMGET(12, 141), 0);
		}
	}

	if (sChaosCount > 0)
	{
		for (int iC = 0; iC < sChaosCount; iC++)
		{
			ItemCreate(ObjSeller, 235, ObjSeller.X, ObjSeller.Y, ITEMGET(12, 15), 0, 0, 0, 0, 0, ObjSeller.m_Index, 0, 0, 0, 0, 0);

			sLog->outBasic("[PShop][Create Jewel Put Seller Inven] - User(ID:%s,Name:%s) Item(Name:%s,Code:%d,level:%d)",
				ObjSeller.AccountID, ObjSeller.Name, ItemAttribute[ITEMGET(12, 15)].Name, ITEMGET(12, 15), 0);
		}
	}

	return FALSE;
}

int CheckInventoryEmptySpaceCount(CGameObject &Obj, int iItemHeight, int iItemWidth)
{
	int nCount = 0;

	for (int h = 0; h < MAX_INVENTORY_H; h++)
	{
		for (int w = 0; w < 8; w++)
		{
			if (*(BYTE*)(Obj.pInventoryMap + h * 8 + w) == 255)
			{
				BYTE blank = gObjOnlyInventoryRectCheck(Obj, w, h, iItemWidth, iItemHeight);

				if (blank != 255 && blank != 254)
				{
					nCount++;
				}
			}
		}
	}

	return nCount;
}

BOOL SummonGoldColossusMonster(CGameObject &Obj, int monsterIndex, int nCount, int bIsHelpMon)
{
	if (nCount < 1)
	{
		nCount = 1;
	}

	if ((MapC[Obj.MapNumber].GetAttr(Obj.X, Obj.Y) & 1) == 1)
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 566), Obj, TRUE);
		return FALSE;
	}

	if (BC_MAP_RANGE(Obj.MapNumber))
	{
		return FALSE;
	}

	if (CC_MAP_RANGE(Obj.MapNumber))
	{
		return FALSE;
	}

	if (DS_MAP_RANGE(Obj.MapNumber))
	{
		return FALSE;
	}

	if (DG_MAP_RANGE(Obj.MapNumber))
	{
		return FALSE;
	}

	if (IMPERIAL_MAP_RANGE(Obj.MapNumber))
	{
		return FALSE;
	}

	if (KALIMA_MAP_RANGE(Obj.MapNumber))
	{
		return FALSE;
	}

	if (Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		return FALSE;
	}

	if (Obj.MapNumber == MAP_INDEX_HATCHERY)
	{
		return FALSE;
	}

	if (Obj.MapNumber == MAP_INDEX_ICARUS)
	{
		return FALSE;
	}

	if (Obj.MapNumber == MAP_INDEX_VULCANROOM)
	{
		return FALSE;
	}

	if (Obj.MapNumber == MAP_INDEX_ARCA_WAR || Obj.MapNumber == MAP_INDEX_DEBENTER_ARCA_WAR)
	{
		return FALSE;
	}

	if (Obj.MapNumber == MAP_INDEX_BARRACKS || Obj.MapNumber == MAP_INDEX_REFUGEE)
	{
		return FALSE;
	}

	if (Obj.MapNumber == MAP_INDEX_VULCAN)
	{
		return FALSE;
	}

	if (ITL_MAP_RANGE(Obj.MapNumber))
	{
		return FALSE;
	}

	LPMONSTER_ATTRIBUTE lpMonsterAttr = 0;

	if (monsterIndex)
	{
		lpMonsterAttr = gMAttr.GetAttr(monsterIndex);
	}

	if (!lpMonsterAttr)
	{
		return FALSE;
	}

	for (int i = 0; i < nCount; i++)
	{
		int nResult = gObjAddMonster(Obj.MapNumber);

		if (nResult < 0)
		{
			return FALSE;
		}
		CGameObject* lpObjMonster = new CGameObject();
		getGameObject(nResult)->m_Disappear_Monster = GetTickCount();
		gObjSetMonster(*lpObjMonster, lpMonsterAttr->m_Index);
		getGameObject(nResult)->DieRegen = 0;
		getGameObject(nResult)->m_bGMSummon = 1;
		getGameObject(nResult)->m_PosNum = -1;
		getGameObject(nResult)->X = Obj.X;
		getGameObject(nResult)->Y = Obj.Y;
		getGameObject(nResult)->MapNumber = Obj.MapNumber;
		getGameObject(nResult)->TX = getGameObject(nResult)->X;
		getGameObject(nResult)->TY = getGameObject(nResult)->Y;
		getGameObject(nResult)->m_OldX = getGameObject(nResult)->X;
		getGameObject(nResult)->m_OldY = getGameObject(nResult)->Y;
		getGameObject(nResult)->StartX = LOBYTE(getGameObject(nResult)->X);
		getGameObject(nResult)->StartY = LOBYTE(getGameObject(nResult)->Y);
		getGameObject(nResult)->Level = LOWORD(lpMonsterAttr->m_Level);
		getGameObject(nResult)->m_Attribute = 60;
		getGameObject(nResult)->MaxRegenTime = 5000;
		getGameObject(nResult)->Dir = rand() % 8;
		getGameObject(nResult)->m_SummonCharDBNum = Obj.DBNumber;

		if (bIsHelpMon == 1)
			getGameObject(nResult)->m_bIsHelpMon = 1;

		sLog->outBasic("[ColossusEvent][SummonMonster][%s][%s] MonsterClass : [%d], MonsterIndex : [%d], Map[%d], X[%d], Y[%d]",
			Obj.AccountID, Obj.Name, monsterIndex, nResult, getGameObject(nResult)->MapNumber, getGameObject(nResult)->X, getGameObject(nResult)->Y);
		return TRUE;
	}
}

BOOL SummonPetEventMonster(CGameObject &Obj)
{
	if ((MapC[Obj.MapNumber].GetAttr(Obj.X, Obj.Y) & 1) == 1)
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 566), Obj, TRUE);
		return FALSE;
	}

	if (BC_MAP_RANGE(Obj.MapNumber))
	{
		return FALSE;
	}

	if (CC_MAP_RANGE(Obj.MapNumber))
	{
		return FALSE;
	}

	if (DS_MAP_RANGE(Obj.MapNumber))
	{
		return FALSE;
	}

	if (DG_MAP_RANGE(Obj.MapNumber))
	{
		return FALSE;
	}

	if (IMPERIAL_MAP_RANGE(Obj.MapNumber))
	{
		return FALSE;
	}

	if (KALIMA_MAP_RANGE(Obj.MapNumber))
	{
		return FALSE;
	}

	if (Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		return FALSE;
	}

	if (Obj.MapNumber == MAP_INDEX_HATCHERY)
	{
		return FALSE;
	}

	if (Obj.MapNumber == MAP_INDEX_ICARUS)
	{
		return FALSE;
	}

	if (Obj.MapNumber == MAP_INDEX_VULCANROOM)
	{
		return FALSE;
	}

	if (Obj.MapNumber == MAP_INDEX_ARCA_WAR || Obj.MapNumber == MAP_INDEX_DEBENTER_ARCA_WAR)
	{
		return FALSE;
	}

	if (Obj.MapNumber == MAP_INDEX_BARRACKS || Obj.MapNumber == MAP_INDEX_REFUGEE)
	{
		return FALSE;
	}

	if (Obj.MapNumber == MAP_INDEX_VULCAN)
	{
		return FALSE;
	}

	if (ITL_MAP_RANGE(Obj.MapNumber))
	{
		return FALSE;
	}

	int nMonsterRate = GetLargeRand() % 10000;
	int monsterIndex;

	if (nMonsterRate >= g_ConfigRead.data.common.PetEventBookMoonRabbitSpawnRate)
	{
		if (nMonsterRate >= g_ConfigRead.data.common.PetEventBookLuckyBagSpawnRate + g_ConfigRead.data.common.PetEventBookMoonRabbitSpawnRate)
		{
			if (nMonsterRate >= g_ConfigRead.data.common.PetEventBookFireFlameSpawnRate + g_ConfigRead.data.common.PetEventBookLuckyBagSpawnRate + g_ConfigRead.data.common.PetEventBookMoonRabbitSpawnRate)
			{
				if (nMonsterRate >= g_ConfigRead.data.common.PetEventBookGoldGoblinSpawnRate + g_ConfigRead.data.common.PetEventBookFireFlameSpawnRate + g_ConfigRead.data.common.PetEventBookLuckyBagSpawnRate + g_ConfigRead.data.common.PetEventBookMoonRabbitSpawnRate)
				{
					return 0;
				}

				monsterIndex = 677;
			}

			else
			{
				monsterIndex = 676;
			}
		}

		else
		{
			monsterIndex = 675;
		}
	}

	else
	{
		monsterIndex = 674;
	}

	LPMONSTER_ATTRIBUTE lpMonsterAttr = 0;

	if (monsterIndex)
	{
		lpMonsterAttr = gMAttr.GetAttr(monsterIndex);
	}

	if (!lpMonsterAttr)
	{
		return FALSE;
	}

	int nResult = gObjAddMonster(Obj.MapNumber);

	if (nResult < 0)
	{
		return FALSE;
	}

	CGameObject* lpObjPet = new CGameObject();
	lpObjPet->m_Disappear_Monster = GetTickCount();
	gObjSetMonster(*lpObjPet, lpMonsterAttr->m_Index);
	lpObjPet->DieRegen = 0;
	lpObjPet->m_bGMSummon = 1;
	lpObjPet->m_PosNum = -1;
	lpObjPet->X = Obj.X;
	lpObjPet->Y = Obj.Y;
	lpObjPet->MapNumber = Obj.MapNumber;
	lpObjPet->TX = lpObjPet->X;
	lpObjPet->TY = lpObjPet->Y;
	lpObjPet->m_OldX = lpObjPet->X;
	lpObjPet->m_OldY = lpObjPet->Y;
	lpObjPet->StartX = LOBYTE(lpObjPet->X);
	lpObjPet->StartY = LOBYTE(lpObjPet->Y);
	lpObjPet->Level = LOWORD(lpMonsterAttr->m_Level);
	lpObjPet->m_Attribute = 60;
	lpObjPet->MaxRegenTime = 5000;
	lpObjPet->Dir = rand() % 8;

	sLog->outBasic("[PetEvent][SummonMonster][%s][%s] MonsterClass : [%d], MonsterIndex : [%d], Map[%d], X[%d], Y[%d]",
		Obj.AccountID, Obj.Name, monsterIndex, nResult, lpObjPet->MapNumber, lpObjPet->X, lpObjPet->Y);

	return TRUE;
}

BOOL NewYearSummonMonster(CGameObject &Obj, int monsterIndex)
{
	if ((MapC[Obj.MapNumber].GetAttr(Obj.X, Obj.Y) & 1) == 1)
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 566), Obj, TRUE);
		return FALSE;
	}

	if (BC_MAP_RANGE(Obj.MapNumber))
	{
		return FALSE;
	}

	if (CC_MAP_RANGE(Obj.MapNumber))
	{
		return FALSE;
	}

	if (DS_MAP_RANGE(Obj.MapNumber))
	{
		return FALSE;
	}

	if (DG_MAP_RANGE(Obj.MapNumber))
	{
		return FALSE;
	}

	if (IMPERIAL_MAP_RANGE(Obj.MapNumber))
	{
		return FALSE;
	}

	if (KALIMA_MAP_RANGE(Obj.MapNumber))
	{
		return FALSE;
	}

	if (Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		return FALSE;
	}

	if (Obj.MapNumber == MAP_INDEX_HATCHERY)
	{
		return FALSE;
	}

	if (Obj.MapNumber == MAP_INDEX_ICARUS)
	{
		return FALSE;
	}

	if (Obj.MapNumber == MAP_INDEX_VULCANROOM)
	{
		return FALSE;
	}

	if (Obj.MapNumber == MAP_INDEX_ARCA_WAR || Obj.MapNumber == MAP_INDEX_DEBENTER_ARCA_WAR)
	{
		return FALSE;
	}

	if (Obj.MapNumber == MAP_INDEX_BARRACKS || Obj.MapNumber == MAP_INDEX_REFUGEE)
	{
		return FALSE;
	}

	if (Obj.MapNumber == MAP_INDEX_VULCAN)
	{
		return FALSE;
	}

	if (ITL_MAP_RANGE(Obj.MapNumber))
	{
		return FALSE;
	}

	LPMONSTER_ATTRIBUTE lpMonsterAttr = 0;

	if (monsterIndex)
	{
		lpMonsterAttr = gMAttr.GetAttr(monsterIndex);
	}

	if (!lpMonsterAttr)
	{
		return FALSE;
	}

	int nResult = gObjAddMonster(Obj.MapNumber);

	if (nResult < 0)
	{
		return FALSE;
	}

	CGameObject* lpObjMonster = new CGameObject();
	lpObjMonster->m_Disappear_NewYearMonster = GetTickCount();
	gObjSetMonster(*lpObjMonster, lpMonsterAttr->m_Index);
	lpObjMonster->DieRegen = 0;
	lpObjMonster->m_bGMSummon = 1;
	lpObjMonster->m_PosNum = -1;
	lpObjMonster->X = Obj.X;
	lpObjMonster->Y = Obj.Y;
	lpObjMonster->MapNumber = Obj.MapNumber;
	lpObjMonster->TX = lpObjMonster->X;
	lpObjMonster->TY = lpObjMonster->Y;
	lpObjMonster->m_OldX = lpObjMonster->X;
	lpObjMonster->m_OldY = lpObjMonster->Y;
	lpObjMonster->StartX = LOBYTE(lpObjMonster->X);
	lpObjMonster->StartY = LOBYTE(lpObjMonster->Y);
	lpObjMonster->Level = LOWORD(lpMonsterAttr->m_Level);
	lpObjMonster->m_Attribute = 60;
	lpObjMonster->MaxRegenTime = 5000;
	lpObjMonster->Dir = rand() % 8;

	return TRUE;
}

int gObjGetAutoPartyUserCount()
{
	int counter = 0;
	for each (std::pair<int,CGameObject*> ObjEntry in gGameObjects)
	{
		CGameObject* lpObj = ObjEntry.second;
		if (lpObj->Connected == PLAYER_PLAYING && lpObj->Type == OBJ_USER)
		{
			if (lpObj->m_PlayerData->bActiveSetParty)
				counter++;
		}
	}
	return counter;
}

int gObjGetOffTradeUsercount()
{
	int counter = 0;
	for each (std::pair<int,CGameObject*> ObjEntry in gGameObjects)
	{
		CGameObject* lpObj = ObjEntry.second;
		if (lpObj->Connected == PLAYER_PLAYING && lpObj->Type == OBJ_USER)
		{
			if (lpObj->m_bOff && !lpObj->m_bOffLevel)
				counter++;
		}
	}
	return counter;
}

void gObjDisconnectOffTraders()
{
	for each (std::pair<int, CGameObject*> ObjEntry in gGameObjects)
	{
		CGameObject* lpObj = ObjEntry.second;
		if (lpObj->Connected > PLAYER_EMPTY)
		{
			if (lpObj->Type == OBJ_USER && lpObj->m_bOff && !lpObj->m_bOffLevel)
			{
				GJSetCharacterInfo(*lpObj, FALSE);
				lpObj->m_bOff = false;
				gObjDel(*lpObj);
				IOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
			}
		}
	}
}

void gObjDisconnectOffLevelers()
{
	for each (std::pair<int, CGameObject*> ObjEntry in gGameObjects)
	{
		CGameObject* lpObj = ObjEntry.second;
		if (lpObj->Connected > PLAYER_EMPTY)
		{
			if (lpObj->Type == OBJ_USER && lpObj->m_bOff && lpObj->m_bOffLevel)
			{
				GJSetCharacterInfo(*lpObj, FALSE);
				lpObj->m_bOff = false;
				gObjDel(*lpObj);
				IOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
			}
		}
	}
}

void gObjInvenPetDamage(CGameObject &Obj, int damage)
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	BOOL send_dur = FALSE;

	if (Obj.m_btInvenPetPos == 0)
	{
		return;
	}

	if (Obj.m_wInvenPet == (WORD)-1)
	{
		return;
	}

	if (Obj.pntInventory[Obj.m_btInvenPetPos]->IsItem() == FALSE)
	{
		return;
	}

	if (Obj.pntInventory[Obj.m_btInvenPetPos]->IsPeriodItem() == TRUE)
	{
		return;
	}

	float fN = 10.0f;
	CItemObject * sprite = Obj.pntInventory[Obj.m_btInvenPetPos];
	float fdamage = damage;

	if (sprite->m_Type == ITEMGET(13, 2) && g_ConfigRead.pet.DamageDisableForPet[DAMAGE_OFF_UNIRIA] == false)
	{
		if (Obj.m_PlayerData->m_MPSkillOpt->iMpsDurabilityReduction3 > 0.0)
		{
			fdamage = damage / Obj.m_PlayerData->m_MPSkillOpt->iMpsDurabilityReduction3;
		}

		fdamage = (fdamage) / 10.0f;
		fdamage /= 10.0f;
		sprite->m_Durability -= fdamage;
		send_dur = 1;
	}

	else if (sprite->m_Type == ITEMGET(13, 3) && g_ConfigRead.pet.DamageDisableForPet[DAMAGE_OFF_DINORANT] == false)
	{
		if (Obj.m_PlayerData->m_MPSkillOpt->iMpsDurabilityReduction3 > 0.0)
		{
			fdamage = damage / Obj.m_PlayerData->m_MPSkillOpt->iMpsDurabilityReduction3;
		}

		fdamage = (fdamage) / 10.0f;
		fdamage /= 10.0f;
		sprite->m_Durability -= fdamage;
		send_dur = 1;
	}

	else if (sprite->m_Type == ITEMGET(13, 4) && g_ConfigRead.pet.DamageDisableForPet[DAMAGE_OFF_DLHORSE] == false)
	{
		if (sprite->m_Durability < 1.0f)
		{
			return;
		}

		fdamage = (damage * 2) / 10.0f;
		fdamage /= 10.0f;
		fdamage += 1.0f;
		sprite->m_DurabilitySmall += (int)fdamage;
		int DurabilityVal = 1500 + Obj.m_PlayerData->m_MPSkillOpt->iMpsPetDurDownSpeed;

		if (sprite->m_DurabilitySmall > DurabilityVal)
		{
			sprite->m_Durability -= 1.0f;
			sprite->m_DurabilitySmall = 0;
			send_dur = 1;

			if (sprite->m_Durability < 1.0f)
			{
				sprite->m_Durability = 0;

				UINT64 iPetExperience = Obj.pntInventory[Obj.m_btInvenPetPos]->m_PetItem_Exp;

				if (sprite->DecPetItemExp(10) != FALSE)
				{
					sLog->outBasic("[%s][%s][PetItemExpDown] [%s] Level:[%d]Exp:[%I64d]DecExp[%I64d]",
						Obj.AccountID, Obj.Name, Obj.pntInventory[Obj.m_btInvenPetPos]->GetName(),
						Obj.pntInventory[Obj.m_btInvenPetPos]->m_PetItem_Level, Obj.pntInventory[Obj.m_btInvenPetPos]->m_PetItem_Exp,
						iPetExperience - Obj.pntInventory[Obj.m_btInvenPetPos]->m_PetItem_Exp);
					CDarkSpirit::SendLevelmsg(Obj, Obj.m_btInvenPetPos, 1, (BYTE)-1);
				}

				sLog->outBasic("[%s][%s] Dark Lord Pet is Broken because durability is exhausted [%I64d]",
					Obj.AccountID, Obj.Name, Obj.pntInventory[Obj.m_btInvenPetPos]->m_Number);
			}

			gGameProtocol.GCItemObjectDurSend(Obj, Obj.m_btInvenPetPos, sprite->m_Durability, 0);
		}

		return;
	}

	else if (sprite->m_Type == ITEMGET(13, 37) && g_ConfigRead.pet.DamageDisableForPet[DAMAGE_OFF_FENRIR] == false)
	{
		if (sprite->m_Durability < 1.0f)
		{
			return;
		}

		int iMaxDurSmall = g_iFenrirDefaultMaxDurSmall;
		int iMultipleDmg = 2;

		if (Obj.Class == CLASS_WIZARD)
		{

		}

		else if (Obj.Class == CLASS_KNIGHT)
		{

		}

		else if (Obj.Class == CLASS_ELF)
		{
			iMaxDurSmall = g_iFenrirElfMaxDurSmall;
			iMultipleDmg = 2;
		}

		if (sprite->IsFenrirIllusion() == TRUE)
		{
			iMaxDurSmall = g_ConfigRead.pet.GoldFenrirDurabilityRate;
		}

		if (Obj.m_PlayerData->m_MPSkillOpt->iMpsDurabilityReduction3 > 0.0)
		{
			fdamage = damage / Obj.m_PlayerData->m_MPSkillOpt->iMpsDurabilityReduction3;
		}

		fdamage = fdamage * iMultipleDmg / 10.0f;
		fdamage /= 10.0f;
		fdamage += 1.0f;

		sprite->m_DurabilitySmall += (int)fdamage;

		if (sprite->m_DurabilitySmall > iMaxDurSmall)
		{
			sprite->m_Durability -= 1.0f;
			sprite->m_DurabilitySmall = 0;

			if (sprite->m_Durability < 1.0f)
			{
				sprite->m_Durability = 0;
			}

			send_dur = 1;
		}
	}

	if (sprite->m_Durability < 0.0f)
	{
		sprite->m_Durability = 0.0f; // fixed durability error (previously gs was sending negative values)
	}

	if (send_dur == TRUE)
	{
		gGameProtocol.GCItemObjectDurSend(Obj, Obj.m_btInvenPetPos, sprite->m_Durability, 0);

		if (sprite->m_Durability < 1.0)
		{
			int itemnum = sprite->m_Type;
			int iInvenPetPos = Obj.m_btInvenPetPos;

			sLog->outBasic("[%s][%s] %s+%d Item is broken because durability is exhausted", Obj.AccountID, Obj.Name,
				Obj.pntInventory[iInvenPetPos]->GetName(), Obj.pntInventory[iInvenPetPos]->m_Level);

			sLog->outBasic("[CRenewal][InvenPet][ZeroDur][%s][%s] PetIndex:[%d], Pos:[%d] InvenPet Durability is Zero. => Remove Inven Pet",
				Obj.AccountID, Obj.Name, sprite->m_Type, iInvenPetPos);

			PMSG_REQ_INVENTORY_EQUIPMENT_ITEM pMsg;
			pMsg.btItemPos = iInvenPetPos;
			pMsg.btValue = (BYTE)-1;

			gGameProtocol.CGInventoryEquipment(&pMsg, Obj);
			gObjInventoryDeleteItem(Obj, iInvenPetPos);
			gGameProtocol.GCInventoryItemDeleteSend(Obj, iInvenPetPos, 0);

			if (Obj.MapNumber == MAP_INDEX_ICARUS && itemnum == ITEMGET(13, 3) || Obj.MapNumber == MAP_INDEX_ICARUS && itemnum == ITEMGET(13, 37))
			{
				int pos = -1;

				for (int i = MAX_PLAYER_EQUIPMENT; i < MAIN_INVENTORY_SIZE; i++)
				{
					if (Obj.pntInventory[i]->IsItem() == TRUE)
					{
						if (Obj.pntInventory[i]->m_Type == ITEMGET(13, 3) || Obj.pntInventory[i]->m_Type == ITEMGET(13, 37))
						{
							if (Obj.pntInventory[i]->m_Durability > 0.0)
							{
								pos = i;
								break;
							}
						}
					}
				}

				if (pos == -1)
				{
					if (Obj.pntInventory[7]->IsItem() == FALSE)
					{
						gObjMoveGate(Obj, 22);
					}
				}

				else
				{
					PMSG_REQ_INVENTORY_EQUIPMENT_ITEM pMsg;
					pMsg.btItemPos = iInvenPetPos;
					pMsg.btValue = (BYTE)-2;

					gGameProtocol.CGInventoryEquipment(&pMsg, Obj);
					gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 82), Obj, 1);
				}
			}
		}
	}
}

BOOL gObjUseInvenPetCheck(CGameObject &Obj, CItemObject *lpItem, int flag)
{
	if (flag == 1)
	{
		if (!gObjInventorySearchSerialNumber(Obj, lpItem->m_Number))
		{
			return FALSE;
		}

		if (!gObjWarehouseSearchSerialNumber(Obj, lpItem->m_Number))
		{
			return FALSE;
		}

		if (lpItem->m_Durability <= 0.0)
		{
			return FALSE;
		}

		if (Obj.m_Rest != 0)
		{
			return FALSE;
		}

		if (CC_MAP_RANGE(Obj.MapNumber) == TRUE)
		{
			return FALSE;
		}

		if (Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
		{
			return FALSE;
		}

		int useClass = lpItem->IsClass(Obj.Class, Obj.m_PlayerData->ChangeUP);

		if (useClass == FALSE)
		{
			return FALSE;
		}

		if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_BLESS) == TRUE)
		{

			int nEffectValue = 0;
			gObjGetValueOfBuffIndex(Obj, BUFFTYPE_BLESS, &nEffectValue, NULL);

			int nTmpLeadership = 0;

			if (Obj.Class == CLASS_DARKLORD)
			{
				nTmpLeadership = Obj.Leadership - nEffectValue;
			}

			if (Obj.m_PlayerData->Strength - nEffectValue + Obj.AddStrength < lpItem->m_RequireStrength - lpItem->m_HJOpStrength)
			{
				return FALSE;
			}

			if (Obj.m_PlayerData->Dexterity - nEffectValue + Obj.AddDexterity < lpItem->m_RequireDexterity - lpItem->m_HJOpDexterity)
			{
				return FALSE;
			}

			if (lpItem->m_Type >= ITEMGET(0, 0) && lpItem->m_Type < ITEMGET(12, 0))
			{
				if (Obj.m_PlayerData->Vitality - nEffectValue + Obj.AddVitality < lpItem->m_RequireVitality)
				{
					return FALSE;
				}

				if (Obj.m_PlayerData->Energy - nEffectValue + Obj.AddEnergy < lpItem->m_RequireEnergy)
				{
					return FALSE;
				}
			}

			if (nTmpLeadership + Obj.AddLeadership < lpItem->m_RequireLeaderShip)
			{
				return FALSE;
			}
		}

		else
		{
			if (Obj.m_PlayerData->Strength + Obj.AddStrength < lpItem->m_RequireStrength - lpItem->m_HJOpStrength)
			{
				return FALSE;
			}

			if (Obj.m_PlayerData->Dexterity + Obj.AddDexterity < lpItem->m_RequireDexterity - lpItem->m_HJOpDexterity)
			{
				return FALSE;
			}

			if (lpItem->m_Type >= ITEMGET(0, 0) && lpItem->m_Type < ITEMGET(12, 0))
			{
				if (Obj.m_PlayerData->Vitality + Obj.AddVitality < lpItem->m_RequireVitality)
				{
					return FALSE;
				}

				if (Obj.m_PlayerData->Energy + Obj.AddEnergy < lpItem->m_RequireEnergy)
				{
					return FALSE;
				}
			}
			if (Obj.Leadership + Obj.AddLeadership < lpItem->m_RequireLeaderShip)
			{
				return FALSE;
			}

		}

		if (Obj.Level < lpItem->m_RequireLevel)
		{
			return FALSE;
		}

		if (lpItem->m_Type >= ITEMGET(13, 0) && lpItem->m_Type < ITEMGET(13, 15))
		{
			if (lpItem->m_Type == ITEMGET(13, 2))
			{
				if (Obj.MapNumber == MAP_INDEX_ICARUS || Obj.MapNumber == MAP_INDEX_ATHLANSE)
				{
					return FALSE;
				}
			}

			else if (lpItem->m_Type == ITEMGET(13, 3))
			{
				if (Obj.MapNumber == MAP_INDEX_ICARUS)
				{
					return FALSE;
				}
			}
		}
		return TRUE;
	}

	else
	{
		if (Obj.MapNumber == MAP_INDEX_ICARUS)
		{
			if ((lpItem->m_Type == ITEMGET(13, 3) || lpItem->m_Type == ITEMGET(13, 37)) && lpItem->m_Durability == 0.0)
			{
				return TRUE;
			}

			if (lpItem->m_Type == ITEMGET(13, 3))
			{
				if (Obj.pntInventory[7]->IsItem() == FALSE)
				{
					return FALSE;
				}
			}

			else if (lpItem->m_Type == ITEMGET(13, 37))
			{
				if (Obj.pntInventory[7]->IsItem() == FALSE)
				{
					return FALSE;
				}
			}

			else if (lpItem->m_Type == ITEMGET(13, 4))
			{
				if (Obj.pntInventory[7]->IsItem() == FALSE)
				{
					return FALSE;
				}
			}
		}

		return TRUE;
	}
}

BOOL IsInvenPet(WORD sIndex)
{
	return sIndex >= ITEMGET(13, 2) && (sIndex <= ITEMGET(13, 5) || sIndex == ITEMGET(13, 37));
}

BOOL IsInvenItem(WORD sIndex)
{
	switch (sIndex)
	{
	case ITEMGET(13, 128):
	case ITEMGET(13, 129):
	case ITEMGET(13, 130):
	case ITEMGET(13, 131):
	case ITEMGET(13, 132):
	case ITEMGET(13, 133):
	case ITEMGET(13, 134):
	case ITEMGET(13, 177):
	case ITEMGET(13, 178):
	case ITEMGET(13, 179):
		return TRUE;
	default:
		return FALSE;
	}
}

int gObjCalDistanceTX(CGameObject &Obj1, CGameObject &Obj2)
{
	if (Obj1.TX == Obj2.TX && Obj1.TY == Obj2.TY)
	{
		return 0;
	}

	float tx = Obj1.TX - Obj2.TX;
	float ty = Obj1.TY - Obj2.TY;

	return sqrt((tx*tx) + (ty*ty));
}

void Check_SameSerialItem(CGameObject &Obj, BYTE bCase, bool & bCheckFail)
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	DWORD dwSameSerialItemCount = 0;
	UINT64 i64Serial = 0LL;

	std::set<UINT64> UserAllItemList;

	if (bCase == 1)
	{
		Obj.m_PlayerData->dwOpenWareCnt++;
	}

	else if (bCase == 2)
	{
		Obj.m_PlayerData->dwCloseWareCnt++;
	}

	DWORD dwInvenDeleteCnt = 0;
	DWORD dwWareDeleteCnt = 0;
	bool bNeedFullItemLog = false;

	if (Obj.m_PlayerData->dwOpenWareCnt == 1 || Obj.m_PlayerData->dwCloseWareCnt == 1)
	{
		bNeedFullItemLog = true;

		sLog->outBasic("[SERIAL CHECK] Performing Serial Checking due to Open/Close WareHouse - ACC:[%s], NAME:[%s],Open:[%d], Close:[%d], case:[%d]",
			Obj.AccountID, Obj.Name, Obj.m_PlayerData->dwOpenWareCnt, Obj.m_PlayerData->dwCloseWareCnt, bCase);
	}

	if (bNeedFullItemLog == true)
	{
		DWORD dwAllWareCount = 0;
		DWORD dwNoneZeroItemCount = 0;

		for (int iPos = 0; iPos < WAREHOUSE_SIZE; iPos++)
		{
			if (Obj.pntWarehouse[iPos]->IsItem() == false)
			{
				continue;
			}

			dwAllWareCount++;

			if (Obj.pntWarehouse[iPos]->GetNumber())
			{
				sLog->outBasic("[SERIAL CHECK] Warehouse Item Acc:[%s],Name:[%s],SR:[%I64d],POS:[%d],LVL:[%d],ItemName:[%s]",
					Obj.AccountID, Obj.Name, Obj.pntWarehouse[iPos]->GetNumber(), iPos, Obj.pntWarehouse[iPos]->m_Level, Obj.pntWarehouse[iPos]->GetName());

				dwNoneZeroItemCount++;
			}
		}

		sLog->outBasic("[SERIAL CHECK] Warehouse Item List Complete Acc:[%s],Name:[%s], CNT:[%d] NON-ZERO-CNT:[%d]",
			Obj.AccountID, Obj.Name, dwAllWareCount, dwNoneZeroItemCount);

		DWORD dwInvenCnt = 0;
		DWORD dwInvenNonZero = 0;

		for (int iPos = 0; iPos < INVENTORY_SIZE; iPos++)
		{
			if (Obj.pntInventory[iPos]->IsItem() == false)
			{
				continue;
			}

			dwInvenCnt++;

			if (Obj.pntInventory[iPos]->GetNumber())
			{
				sLog->outBasic("[SERIAL CHECK] Inventory Item Acc:[%s],Name:[%s],SR:[%I64d],POS:[%d],LVL:[%d],ItemName:[%s]",
					Obj.AccountID, Obj.Name, Obj.pntInventory[iPos]->GetNumber(), iPos, Obj.pntInventory[iPos]->m_Level, Obj.pntInventory[iPos]->GetName());

				dwInvenNonZero++;
			}
		}

		sLog->outBasic("[SERIAL CHECK] Inventory Item List Complete Acc:[%s],Name:[%s], CNT:[%d] NON-ZERO-CNT:[%d]",
			Obj.AccountID, Obj.Name, dwInvenCnt, dwInvenNonZero);

		for (int iPos = 0; iPos < WAREHOUSE_SIZE; iPos++)
		{
			if (Obj.pntWarehouse[iPos]->IsItem() == false)
			{
				continue;
			}

			UINT64 i64Serial = Obj.pntWarehouse[iPos]->GetNumber();

			if (i64Serial)
			{
				std::pair<std::set<UINT64>::iterator, bool> ret = UserAllItemList.insert(i64Serial);

				if (ret.second == false)
				{
					sLog->outBasic("[SERIAL CHECK] ~~~~~~ DUPLICATED ITEM FOUND [WAREHOUSE] - ITEM WILL BE DELETED ~~~~~~ ACC:[%s],Name:[%s],SERIAL:[%I64d],POS:[%d],LVL:[%d],ItemName:[%s]",
						Obj.AccountID, Obj.Name, i64Serial, iPos, Obj.pntWarehouse[iPos]->m_Level, Obj.pntWarehouse[iPos]->GetName());

					gObjWarehouseDeleteItem(Obj, iPos);
					dwWareDeleteCnt++;
					dwSameSerialItemCount++;
				}
			}
		}

		for (int iPos = 0; iPos < INVENTORY_SIZE; iPos++)
		{
			if (Obj.pntInventory[iPos]->IsItem() == false)
			{
				continue;
			}

			UINT64 i64Serial = Obj.pntInventory[iPos]->GetNumber();

			if (i64Serial)
			{
				std::pair<std::set<UINT64>::iterator, bool> ret = UserAllItemList.insert(i64Serial);

				if (ret.second == false)
				{
					sLog->outBasic("[SERIAL CHECK] ~~~~~~ DUPLICATED ITEM FOUND [INVENTORY] - ITEM WILL BE DELETED ~~~~~~ ACC:[%s],Name:[%s],SERIAL:[%I64d],POS:[%d],LVL:[%d],ItemName:[%s]",
						Obj.AccountID, Obj.Name, i64Serial, iPos, Obj.pntInventory[iPos]->m_Level, Obj.pntInventory[iPos]->GetName());

					gObjInventoryDeleteItem(Obj, iPos);
					dwInvenDeleteCnt++;
					dwSameSerialItemCount++;
				}
			}
		}

		if (dwSameSerialItemCount)
		{
			Obj.m_PlayerData->bHasSameSerialItem = true;
			bCheckFail = true;

			sLog->outBasic("[SERIAL CHECK][RESULT] ~~~~~~~ FAILED ~~~~~~ ACC : [%s], CharName :[%s], AllCnt:[%d], Case : [%d] InvenCnt:[%d], WareCnt:[%d] , IP :[%s]",
				Obj.AccountID, Obj.Name, dwSameSerialItemCount, bCase, dwInvenDeleteCnt, dwWareDeleteCnt, Obj.m_PlayerData->ConnectUser->IP);

			sLog->outBasic("[SERIAL CHECK] ACC:[%s],NAME:[%s],IP:[%s] - DISCONNECT",
				Obj.AccountID, Obj.Name, Obj.m_PlayerData->ConnectUser->IP);

			sLog->outBasic("[SERIAL CHECK] ACC:[%s],NAME:[%s],IP:[%s] - DISCONNECT",
				Obj.AccountID, Obj.Name, Obj.m_PlayerData->ConnectUser->IP);

			gGameProtocol.GCSendDisableReconnect(Obj);
		}

		else
		{
			sLog->outBasic("[SERIAL CHECK][RESULT] ~~~~~~~ SUCCESS ~~~~~~ ACC : [%s], CharName :[%s], Case : [%d], IP :[%s]",
				Obj.AccountID, Obj.Name, bCase, Obj.m_PlayerData->ConnectUser->IP);
		}
	}

	else if (bCase == 3)
	{
		sLog->outBasic("[SERIAL CHECK] Performing Serial Checking due to JewelMixSystem - ACC:[%s], NAME:[%s], case:[%d]",
			Obj.AccountID, Obj.Name, bCase);

		DWORD dwInvenCnt = 0;
		DWORD dwInvenNonZero = 0;

		for (int iPos = 0; iPos < INVENTORY_SIZE; iPos++)
		{
			if (Obj.pntInventory[iPos]->IsItem() == false)
			{
				continue;
			}

			dwInvenCnt++;

			if (Obj.pntInventory[iPos]->GetNumber())
			{
				sLog->outBasic("[SERIAL CHECK] Inventory Item Acc:[%s],Name:[%s],SR:[%I64d],POS:[%d],LVL:[%d],ItemName:[%s]",
					Obj.AccountID, Obj.Name, Obj.pntInventory[iPos]->GetNumber(), iPos, Obj.pntInventory[iPos]->m_Level, Obj.pntInventory[iPos]->GetName());

				dwInvenNonZero++;
			}
		}

		sLog->outBasic("[SERIAL CHECK] Inventory Item List Complete Acc:[%s],Name:[%s], CNT:[%d] NON-ZERO-CNT:[%d]",
			Obj.AccountID, Obj.Name, dwInvenCnt, dwInvenNonZero);

		for (int iPos = 0; iPos < INVENTORY_SIZE; iPos++)
		{
			if (Obj.pntInventory[iPos]->IsItem() == false)
			{
				continue;
			}

			UINT64 i64Serial = Obj.pntInventory[iPos]->GetNumber();

			if (i64Serial)
			{
				std::pair<std::set<UINT64>::iterator, bool> ret = UserAllItemList.insert(i64Serial);

				if (ret.second == false)
				{
					sLog->outBasic("[SERIAL CHECK] ~~~~~~ DUPLICATED ITEM FOUND [INVENTORY] - ITEM WILL BE DELETED ~~~~~~ ACC:[%s],Name:[%s],SERIAL:[%I64d],POS:[%d],LVL:[%d],ItemName:[%s]",
						Obj.AccountID, Obj.Name, i64Serial, iPos, Obj.pntInventory[iPos]->m_Level, Obj.pntInventory[iPos]->GetName());

					gObjInventoryDeleteItem(Obj, iPos);
					dwInvenDeleteCnt++;
					dwSameSerialItemCount++;
				}
			}
		}

		if (dwSameSerialItemCount)
		{
			Obj.m_PlayerData->bHasSameSerialItem = true;
			bCheckFail = true;

			sLog->outBasic("[SERIAL CHECK][RESULT] ~~~~~~~ FAILED ~~~~~~ ACC : [%s], CharName :[%s], AllCnt:[%d], Case : [%d] InvenCnt:[%d], IP :[%s]",
				Obj.AccountID, Obj.Name, dwSameSerialItemCount, bCase, dwInvenDeleteCnt, Obj.m_PlayerData->ConnectUser->IP);

			sLog->outBasic("[SERIAL CHECK] ACC:[%s],NAME:[%s],IP:[%s] - DISCONNECT",
				Obj.AccountID, Obj.Name, Obj.m_PlayerData->ConnectUser->IP);

			sLog->outBasic("[SERIAL CHECK] ACC:[%s],NAME:[%s],IP:[%s] - DISCONNECT",
				Obj.AccountID, Obj.Name, Obj.m_PlayerData->ConnectUser->IP);

			gGameProtocol.GCSendDisableReconnect(Obj);
		}

		else
		{
			sLog->outBasic("[SERIAL CHECK][RESULT] ~~~~~~~ SUCCESS ~~~~~~ ACC : [%s], CharName :[%s], Case : [%d], IP :[%s]",
				Obj.AccountID, Obj.Name, bCase, Obj.m_PlayerData->ConnectUser);
		}
	}
}

// Season X Mass Combination System
// Check for Empty space in inventory for multiple items
bool gObjChaosBoxPutItemTest(CGameObject &Obj, CItemObject Item, BYTE btCount)
{
	BYTE TempInventoryMap[INVENTORY_MAP_SIZE];
	BYTE TempEventInventoryMap[EVENT_INVENTORY_MAP_SIZE];

	int result = 1;

	std::memcpy(TempInventoryMap, Obj.pInventoryMap, INVENTORY_MAP_SIZE);
	std::memcpy(TempEventInventoryMap, Obj.EventInventoryMap, EVENT_INVENTORY_MAP_SIZE);

	for (int n = 0; n < btCount; n++)
	{
		if (Item.IsItem() == 1)
		{
			if (IsEventItem(Item))
			{
				if (gObjTempEventInventoryInsertItem(Obj, Item, TempEventInventoryMap) == 255)
				{
					return false;
				}
			}

			else
			{
				if (gObjTempInventoryInsertItem(Obj, Item, TempInventoryMap) == 255)
				{
					return false;
				}
			}

			result = 1;
		}
	}

	if (result == 1)
	{
		return true;
	}

	return false;
}

BYTE gObjChaosBoxInsertItemTemp(CGameObject &Obj, CItemObject * Item)
{
	int w, h, iwidth, iheight;
	BYTE blank = 0;

	if (Item->GetSize((int &)iwidth, (int &)iheight) == 0)
	{
		return -1;
	}

	for (h = 0; h < 4; h++)
	{
		for (w = 0; w < 8; w++)
		{
			if (*(BYTE*)(Obj.ChaosBoxMap + h * 8 + w) == 255)
			{
				blank = gObjMapRectCheck(Obj.ChaosBoxMap, w, h, 8, 4, iwidth, iheight);

				if (blank != 255)
				{
					return blank;
				}
			}
		}
	}

	return -1;
}

BOOL gObjUnicornSprite(CGameObject &Obj) // Season 5 Episode 2 JPN
{
	if (Obj.Type != OBJ_USER)
	{
		return FALSE;
	}

	if (Obj.pntInventory[8]->IsItem() == FALSE)
	{
		return FALSE;
	}

	CItemObject * Unicorn = Obj.pntInventory[8];

	if (Unicorn->m_Type == ITEMGET(13, 106) && Unicorn->m_Durability > 0.0f)
	{
		return TRUE;
	}

	return FALSE;
}

//
void gObjUseRecoveryPotion(CGameObject &Obj, int pos, double value) //00518400
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.pntInventory[pos]->m_Type == ITEMGET(14, 94))
	{
		float loc1 = (float(Obj.AddLife) + Obj.MaxLife);

		Obj.Life = Obj.Life + (value * loc1);

		if (Obj.Life > loc1)
		{
			Obj.Life = loc1;
		}

		gGameProtocol.GCReFillSend(Obj, Obj.Life, -1, 1, Obj.iShield);
	}
	else if (Obj.pntInventory[pos]->m_Type == ITEMGET(14, 133))	//1.01.00
	{
		float Add = Obj.iAddShield + Obj.iMaxShield;
		Obj.iShield = Obj.iShield + Add * value;
		if (Obj.iShield > Add)
		{
			Obj.iShield = Add;
		}
		gGameProtocol.GCReFillSend(Obj, Obj.Life, 0xFFu, 1, Obj.iShield);
	}
	else
	{
		float loc2 = (float(Obj.AddMana) + Obj.MaxMana);

		Obj.Mana = Obj.Mana + (value * loc2);

		if (Obj.Mana > loc2)
		{
			Obj.Mana = loc2;
		}

		gGameProtocol.GCManaSend(Obj, Obj.Mana, -1, 0, Obj.BP);
	}

	Obj.pntInventory[pos]->m_Durability -= 1.0f;

	if (Obj.pntInventory[pos]->m_Durability > 0.0f)
	{
		gGameProtocol.GCItemObjectDurSend(Obj, pos, Obj.pntInventory[pos]->m_Durability, 1);
	}
	else
	{
		gObjInventoryItemSet(Obj, pos, -1);
		Obj.pntInventory[pos]->Clear();
		gGameProtocol.GCInventoryItemDeleteSend(Obj, pos, 1);
	}
	return;
}

void gObjMuBotProc(CGameObject &Obj)
{
	if (g_ConfigRead.data.mubot.enable == FALSE)
	{
		return;
	}

	if (Obj.m_bMUBOT == TRUE)
	{
		Obj.m_iMUBOT_TIME++;

		PMSG_MUBOT_USE pMsg;
		PHeadSubSetB((BYTE*)&pMsg, 0xBF, 0x51, sizeof(pMsg));

		if (Obj.m_iMUBOT_TIME >= 60000)
		{
			pMsg.Status = 1;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
			return;
		}

		if (5 < 1)
		{
			return;
		}

		if ((Obj.m_iMUBOT_TIME % (5 * 60)) == 0)
		{
			Obj.m_iMUBOT_STAGE = 1 + Obj.m_iMUBOT_TIME / (60 * 200);

			if (Obj.m_iMUBOT_STAGE > 5)
			{
				Obj.m_iMUBOT_STAGE = 5;
			}

			int RequireZen = 10000;

			switch (Obj.m_iMUBOT_STAGE)
			{
			case 5:	RequireZen = 50000;	break;
			case 4:	RequireZen = 40000;	break;
			case 3:	RequireZen = 30000;	break;
			case 2:	RequireZen = 20000;	break;
			case 1:	RequireZen = 10000;	break;
			default:
			{
				return;
			}
			}

			if (Obj.m_PlayerData->Money < RequireZen)
			{
				pMsg.Status = 2;
				pMsg.Time = 0;
				pMsg.Money = 0;
			}
			else
			{
				Obj.m_PlayerData->Money -= RequireZen;
				gGameProtocol.GCMoneySend(Obj, Obj.m_PlayerData->Money);

				pMsg.Status = 0;
				pMsg.Money = RequireZen;
				pMsg.Time = Obj.m_iMUBOT_TIME / 60;
			}

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		}
	}
}

int GetWarehouseUsedHowMuch(int UserLevel, BOOL IsLock)
{
	if (g_ConfigRead.data.common.FreeWarehouse == true)
	{
		return 0;
	}

	int rZen = 0;
	rZen = (UserLevel * UserLevel)* 0.1 * 0.4;

	if (bCanWarehouseLock == TRUE)
	{
		if (IsLock != 0)
		{
			rZen = rZen + (UserLevel * 2);
		}
	}
	if (rZen < 1)
	{
		rZen = 1;
	}

	if (rZen >= 1000)
	{
		rZen = (rZen / 100) * 100;
	}

	else if (rZen >= 100)
	{
		rZen = (rZen / 10) * 10;
	}

	if (rZen == 0)
	{
		rZen = 1;
	}

	return rZen;
}
