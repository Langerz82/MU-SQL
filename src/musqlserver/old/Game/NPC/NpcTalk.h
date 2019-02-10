////////////////////////////////////////////////////////////////////////////////
// NpcTalk.h
#ifndef NPCTALK_H
#define NPCTALK_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



struct GC_ANS_NPC_TALK_V1
{
	PHEADB	PHeader;	// C3:30
	BYTE NpcNumber;
	BYTE Param1;
	BYTE Param2;
	BYTE Param3;
	BYTE Param4;
};

struct GC_ANS_NPC_TALK_V2
{
	PHEADB	PHeader;	// C3:30
	BYTE NpcNumber;
	BYTE Param1;
	BYTE Param2;
	BYTE Param3;
	BYTE Param4;
	BYTE Param5;
	BYTE Param6;
};

struct PMSG_ANS_DIG_NPC_CLICK // 1.01.00
{
	PBMSG_HEAD2 h;
	WORD	wNPCIndex;
	DWORD	dwContributePoint;
};

BOOL NpcTalk(LPGameObject &lpNpc, CGameObject* lpObj);
bool Npc_Dialog(LPGameObject &lpNpc, CGameObject* lpObj);

int NpcBattleAnnouncer(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcFasi(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcGuildMasterTalk(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcRolensiaGuard(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcChaosGoblelin(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcRusipher(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcPosionGirl(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcDeviasWizard(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcDeviasGuard(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcDeviasWareHousemen(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcWarehouse(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcNoriaRara(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcDeviasMadam(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcEventChipNPC(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcRorensiaSmith(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcNoriJangIn(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcQuestCheck(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcServerDivision(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcRoadMerchant(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcAngelKing(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcAngelMessanger(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcRolensiaMadam(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcDeviasWeapon(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcDarkSpiritTrainer(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcJewelMixDealer(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcSiegeMachine_Attack(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcSiegeMachine_Defense(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcElderCircle(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcCastleGuard(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcCastleGateLever(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcCastleCrown(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcCastleSwitch(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcCastleHuntZoneGuard(LPGameObject &lpNpc, CGameObject* lpObj);
bool NpcShadowPhantom(int iObjIndex);
BOOL NpcMainatenceMachine(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcReira(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcChaosCardMaster(LPGameObject &lpNpc, CGameObject* lpObj);
bool NpcWerewolfQuarrel(LPGameObject &lpNpc, CGameObject* lpObj);
bool NpcGateKeeper(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcSeedMaster(LPGameObject &lpNpc, CGameObject* lpObj);
BOOL NpcSeedResearcher(LPGameObject &lpNpc, CGameObject* lpObj);
bool NpcTitus(LPGameObject &lpNpc, CGameObject* lpObj);
bool NpcLugard(LPGameObject &lpNpc, CGameObject* lpObj);
bool NpcSilverBox(LPGameObject &lpNpc, CGameObject* lpObj);
bool NpcGoldBox(LPGameObject &lpNpc, CGameObject* lpObj);
bool NpcJerint(LPGameObject &lpNpc, CGameObject* lpObj);
bool NpcGens(LPGameObject &lpNpc, CGameObject* lpObj);
bool NpcJulia(LPGameObject &lpNpc, CGameObject* lpObj);
bool NpcDavid(LPGameObject &lpNpc, CGameObject* lpObj);
bool NpcLeoHelper(LPGameObject &lpNpc,CGameObject* lpObj);
bool NpcSantaClause(LPGameObject &lpNpc,CGameObject* lpObj);
bool NpcTercia(LPGameObject &lpNpc, CGameObject* lpObj);
bool NpcVeina(LPGameObject &lpNpc, CGameObject* lpObj);
bool NpcZyro(LPGameObject &lpNpc, CGameObject* lpObj);
bool NpcAcheronEntrance(LPGameObject &lpNpc, CGameObject* lpObj);
bool NpcElementalMaster(LPGameObject &lpNpc, CGameObject* lpObj);
bool NpcArcaWar(LPGameObject &lpNpc, CGameObject* lpObj);
bool NpcMarketWall(LPGameObject &lpNpc, CGameObject* lpObj);
bool NpcMonicaTalk(LPGameObject &lpNpc, CGameObject* lpObj);
#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

