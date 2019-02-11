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

BOOL NpcTalk(CGameObject &lpNpc, CGameObject lpObj);
bool Npc_Dialog(CGameObject &lpNpc, CGameObject lpObj);

int NpcBattleAnnouncer(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcFasi(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcGuildMasterTalk(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcRolensiaGuard(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcChaosGoblelin(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcRusipher(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcPosionGirl(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcDeviasWizard(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcDeviasGuard(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcDeviasWareHousemen(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcWarehouse(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcNoriaRara(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcDeviasMadam(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcEventChipNPC(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcRorensiaSmith(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcNoriJangIn(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcQuestCheck(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcServerDivision(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcRoadMerchant(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcAngelKing(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcAngelMessanger(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcRolensiaMadam(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcDeviasWeapon(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcDarkSpiritTrainer(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcJewelMixDealer(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcSiegeMachine_Attack(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcSiegeMachine_Defense(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcElderCircle(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcCastleGuard(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcCastleGateLever(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcCastleCrown(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcCastleSwitch(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcCastleHuntZoneGuard(CGameObject &lpNpc, CGameObject lpObj);
bool NpcShadowPhantom(int iObjIndex);
BOOL NpcMainatenceMachine(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcReira(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcChaosCardMaster(CGameObject &lpNpc, CGameObject lpObj);
bool NpcWerewolfQuarrel(CGameObject &lpNpc, CGameObject lpObj);
bool NpcGateKeeper(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcSeedMaster(CGameObject &lpNpc, CGameObject lpObj);
BOOL NpcSeedResearcher(CGameObject &lpNpc, CGameObject lpObj);
bool NpcTitus(CGameObject &lpNpc, CGameObject lpObj);
bool NpcLugard(CGameObject &lpNpc, CGameObject lpObj);
bool NpcSilverBox(CGameObject &lpNpc, CGameObject lpObj);
bool NpcGoldBox(CGameObject &lpNpc, CGameObject lpObj);
bool NpcJerint(CGameObject &lpNpc, CGameObject lpObj);
bool NpcGens(CGameObject &lpNpc, CGameObject lpObj);
bool NpcJulia(CGameObject &lpNpc, CGameObject lpObj);
bool NpcDavid(CGameObject &lpNpc, CGameObject lpObj);
bool NpcLeoHelper(CGameObject &lpNpc,CGameObject lpObj);
bool NpcSantaClause(CGameObject &lpNpc,CGameObject lpObj);
bool NpcTercia(CGameObject &lpNpc, CGameObject lpObj);
bool NpcVeina(CGameObject &lpNpc, CGameObject lpObj);
bool NpcZyro(CGameObject &lpNpc, CGameObject lpObj);
bool NpcAcheronEntrance(CGameObject &lpNpc, CGameObject lpObj);
bool NpcElementalMaster(CGameObject &lpNpc, CGameObject lpObj);
bool NpcArcaWar(CGameObject &lpNpc, CGameObject lpObj);
bool NpcMarketWall(CGameObject &lpNpc, CGameObject lpObj);
bool NpcMonicaTalk(CGameObject &lpNpc, CGameObject lpObj);
#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

