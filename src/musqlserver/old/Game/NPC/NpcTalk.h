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

BOOL NpcTalk(CGameObject &Npc, CGameObject lpObj);
bool Npc_Dialog(CGameObject &Npc, CGameObject lpObj);

int NpcBattleAnnouncer(CGameObject &Npc, CGameObject lpObj);
BOOL NpcFasi(CGameObject &Npc, CGameObject lpObj);
BOOL NpcGuildMasterTalk(CGameObject &Npc, CGameObject lpObj);
BOOL NpcRolensiaGuard(CGameObject &Npc, CGameObject lpObj);
BOOL NpcChaosGoblelin(CGameObject &Npc, CGameObject lpObj);
BOOL NpcRusipher(CGameObject &Npc, CGameObject lpObj);
BOOL NpcPosionGirl(CGameObject &Npc, CGameObject lpObj);
BOOL NpcDeviasWizard(CGameObject &Npc, CGameObject lpObj);
BOOL NpcDeviasGuard(CGameObject &Npc, CGameObject lpObj);
BOOL NpcDeviasWareHousemen(CGameObject &Npc, CGameObject lpObj);
BOOL NpcWarehouse(CGameObject &Npc, CGameObject lpObj);
BOOL NpcNoriaRara(CGameObject &Npc, CGameObject lpObj);
BOOL NpcDeviasMadam(CGameObject &Npc, CGameObject lpObj);
BOOL NpcEventChipNPC(CGameObject &Npc, CGameObject lpObj);
BOOL NpcRorensiaSmith(CGameObject &Npc, CGameObject lpObj);
BOOL NpcNoriJangIn(CGameObject &Npc, CGameObject lpObj);
BOOL NpcQuestCheck(CGameObject &Npc, CGameObject lpObj);
BOOL NpcServerDivision(CGameObject &Npc, CGameObject lpObj);
BOOL NpcRoadMerchant(CGameObject &Npc, CGameObject lpObj);
BOOL NpcAngelKing(CGameObject &Npc, CGameObject lpObj);
BOOL NpcAngelMessanger(CGameObject &Npc, CGameObject lpObj);
BOOL NpcRolensiaMadam(CGameObject &Npc, CGameObject lpObj);
BOOL NpcDeviasWeapon(CGameObject &Npc, CGameObject lpObj);
BOOL NpcDarkSpiritTrainer(CGameObject &Npc, CGameObject lpObj);
BOOL NpcJewelMixDealer(CGameObject &Npc, CGameObject lpObj);
BOOL NpcSiegeMachine_Attack(CGameObject &Npc, CGameObject lpObj);
BOOL NpcSiegeMachine_Defense(CGameObject &Npc, CGameObject lpObj);
BOOL NpcElderCircle(CGameObject &Npc, CGameObject lpObj);
BOOL NpcCastleGuard(CGameObject &Npc, CGameObject lpObj);
BOOL NpcCastleGateLever(CGameObject &Npc, CGameObject lpObj);
BOOL NpcCastleCrown(CGameObject &Npc, CGameObject lpObj);
BOOL NpcCastleSwitch(CGameObject &Npc, CGameObject lpObj);
BOOL NpcCastleHuntZoneGuard(CGameObject &Npc, CGameObject lpObj);
bool NpcShadowPhantom(int iObjIndex);
BOOL NpcMainatenceMachine(CGameObject &Npc, CGameObject lpObj);
BOOL NpcReira(CGameObject &Npc, CGameObject lpObj);
BOOL NpcChaosCardMaster(CGameObject &Npc, CGameObject lpObj);
bool NpcWerewolfQuarrel(CGameObject &Npc, CGameObject lpObj);
bool NpcGateKeeper(CGameObject &Npc, CGameObject lpObj);
BOOL NpcSeedMaster(CGameObject &Npc, CGameObject lpObj);
BOOL NpcSeedResearcher(CGameObject &Npc, CGameObject lpObj);
bool NpcTitus(CGameObject &Npc, CGameObject lpObj);
bool NpcLugard(CGameObject &Npc, CGameObject lpObj);
bool NpcSilverBox(CGameObject &Npc, CGameObject lpObj);
bool NpcGoldBox(CGameObject &Npc, CGameObject lpObj);
bool NpcJerint(CGameObject &Npc, CGameObject lpObj);
bool NpcGens(CGameObject &Npc, CGameObject lpObj);
bool NpcJulia(CGameObject &Npc, CGameObject lpObj);
bool NpcDavid(CGameObject &Npc, CGameObject lpObj);
bool NpcLeoHelper(CGameObject &Npc,CGameObject lpObj);
bool NpcSantaClause(CGameObject &Npc,CGameObject lpObj);
bool NpcTercia(CGameObject &Npc, CGameObject lpObj);
bool NpcVeina(CGameObject &Npc, CGameObject lpObj);
bool NpcZyro(CGameObject &Npc, CGameObject lpObj);
bool NpcAcheronEntrance(CGameObject &Npc, CGameObject lpObj);
bool NpcElementalMaster(CGameObject &Npc, CGameObject lpObj);
bool NpcArcaWar(CGameObject &Npc, CGameObject lpObj);
bool NpcMarketWall(CGameObject &Npc, CGameObject lpObj);
bool NpcMonicaTalk(CGameObject &Npc, CGameObject lpObj);
#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

