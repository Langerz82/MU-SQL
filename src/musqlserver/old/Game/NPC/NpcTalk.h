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

BOOL NpcTalk(LPGameObject &lpNpc, LPGameObject lpObj);
bool Npc_Dialog(LPGameObject &lpNpc, LPGameObject lpObj);

int NpcBattleAnnouncer(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcFasi(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcGuildMasterTalk(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcRolensiaGuard(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcChaosGoblelin(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcRusipher(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcPosionGirl(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcDeviasWizard(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcDeviasGuard(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcDeviasWareHousemen(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcWarehouse(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcNoriaRara(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcDeviasMadam(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcEventChipNPC(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcRorensiaSmith(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcNoriJangIn(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcQuestCheck(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcServerDivision(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcRoadMerchant(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcAngelKing(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcAngelMessanger(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcRolensiaMadam(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcDeviasWeapon(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcDarkSpiritTrainer(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcJewelMixDealer(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcSiegeMachine_Attack(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcSiegeMachine_Defense(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcElderCircle(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcCastleGuard(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcCastleGateLever(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcCastleCrown(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcCastleSwitch(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcCastleHuntZoneGuard(LPGameObject &lpNpc, LPGameObject lpObj);
bool NpcShadowPhantom(int iObjIndex);
BOOL NpcMainatenceMachine(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcReira(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcChaosCardMaster(LPGameObject &lpNpc, LPGameObject lpObj);
bool NpcWerewolfQuarrel(LPGameObject &lpNpc, LPGameObject lpObj);
bool NpcGateKeeper(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcSeedMaster(LPGameObject &lpNpc, LPGameObject lpObj);
BOOL NpcSeedResearcher(LPGameObject &lpNpc, LPGameObject lpObj);
bool NpcTitus(LPGameObject &lpNpc, LPGameObject lpObj);
bool NpcLugard(LPGameObject &lpNpc, LPGameObject lpObj);
bool NpcSilverBox(LPGameObject &lpNpc, LPGameObject lpObj);
bool NpcGoldBox(LPGameObject &lpNpc, LPGameObject lpObj);
bool NpcJerint(LPGameObject &lpNpc, LPGameObject lpObj);
bool NpcGens(LPGameObject &lpNpc, LPGameObject lpObj);
bool NpcJulia(LPGameObject &lpNpc, LPGameObject lpObj);
bool NpcDavid(LPGameObject &lpNpc, LPGameObject lpObj);
bool NpcLeoHelper(LPGameObject &lpNpc,LPGameObject lpObj);
bool NpcSantaClause(LPGameObject &lpNpc,LPGameObject lpObj);
bool NpcTercia(LPGameObject &lpNpc, LPGameObject lpObj);
bool NpcVeina(LPGameObject &lpNpc, LPGameObject lpObj);
bool NpcZyro(LPGameObject &lpNpc, LPGameObject lpObj);
bool NpcAcheronEntrance(LPGameObject &lpNpc, LPGameObject lpObj);
bool NpcElementalMaster(LPGameObject &lpNpc, LPGameObject lpObj);
bool NpcArcaWar(LPGameObject &lpNpc, LPGameObject lpObj);
bool NpcMarketWall(LPGameObject &lpNpc, LPGameObject lpObj);
bool NpcMonicaTalk(LPGameObject &lpNpc, LPGameObject lpObj);
#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

