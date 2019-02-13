////////////////////////////////////////////////////////////////////////////////
// gObjMonster.h
#ifndef GOBJMONSTER_H
#define GOBJMONSTER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Logging/Log.h"
#include "User/CUserData.h"

#define MAX_QUEST_TELEPORT 4
#define RING_OF_TRANSFORM_BASE_RATE 10000
#define FIRE_CRACKER_BASE_RATE 10000

extern bool g_bActiveUrukTrap;
extern bool g_bSilvesterEntranceBlock;
extern UINT64 g_i64LordSilvesterBlockTime;

struct QuestNPCTeleportPos 
{
	int mapnum;	// 0
	int x;	// 4
	int y;	// 8
	int dir;	// C
};


class CQeustNpcTeleport	// size == 0x68
{

public:

	CQeustNpcTeleport(std::string strNPCName)	// line : 97
	{
		this->LastMapNumber = -1;
		this->RefCount = 0;
		InitializeCriticalSection(&this->CsRef);
		this->TimeCount = 0;
		this->m_QuestNPCTeleportPos[0].mapnum = MAP_INDEX_DEVIAS;
		this->m_QuestNPCTeleportPos[0].x = 198;
		this->m_QuestNPCTeleportPos[0].y = 47;
		this->m_QuestNPCTeleportPos[0].dir = 2;
		this->m_QuestNPCTeleportPos[1].mapnum = MAP_INDEX_RORENCIA;
		this->m_QuestNPCTeleportPos[1].x = 137;
		this->m_QuestNPCTeleportPos[1].y = 87;
		this->m_QuestNPCTeleportPos[1].dir = 1;
		this->m_QuestNPCTeleportPos[2].mapnum = MAP_INDEX_NORIA;
		this->m_QuestNPCTeleportPos[2].x = 169;
		this->m_QuestNPCTeleportPos[2].y = 89;
		this->m_QuestNPCTeleportPos[2].dir = 2;
		this->m_QuestNPCTeleportPos[3].mapnum = MAP_INDEX_ATHLANSE;
		this->m_QuestNPCTeleportPos[3].x = 17;
		this->m_QuestNPCTeleportPos[3].y = 0x19;
		this->m_QuestNPCTeleportPos[3].dir = 2;
		this->m_strNPCName = strNPCName;
	};	// line : 114

	~CQeustNpcTeleport()	// line : 115
	{
		DeleteCriticalSection(&this->CsRef);
	};	// line : 115

	void TalkRefAdd()	// line : 118
	{
		EnterCriticalSection(&this->CsRef);
		this->RefCount++;
		sLog->outBasic("%s RefCount Inc = %d", this->m_strNPCName.c_str(), this->RefCount);
		LeaveCriticalSection(&this->CsRef);
		
	};	// line : 123

	void TalkRefDel()	// line : 126
	{
		EnterCriticalSection(&this->CsRef);
		this->RefCount--;

		if ( this->RefCount < 0 )
		{
			this->RefCount = 0;
		}
		
		sLog->outBasic("%s RefCount Dec= %d", this->m_strNPCName.c_str(), this->RefCount);
		LeaveCriticalSection(&this->CsRef);
	};	// line : 135

	//int __thiscall GetRefCount();

	void Run(CGameObject &Obj);

	int RefCount;	// 0
	CRITICAL_SECTION CsRef;	// 4
	int TimeCount; // 1C
	int LastMapNumber;	// 20
	QuestNPCTeleportPos m_QuestNPCTeleportPos[MAX_QUEST_TELEPORT];	// 24
	std::string m_strNPCName;

};


extern CQeustNpcTeleport gQeustNpcTeleport;
extern CQeustNpcTeleport gQuestExpNpcTeleport;

void gObjMonsterMoveRegen(int x, int y, CGameObject lpObj);
int gObjMonsterSelectSkillForMedusa(CGameObject &Obj);
BOOL gObjMonsterRegen(CGameObject &Obj);
int  gObjMonsterViewportIsCharacter(CGameObject &Obj);
void gObjMonsterHitDamageInit(CGameObject &Obj);
int  gObjMonsterHitDamageUserDel(CGameObject &Obj);
void gObjMonsterSetHitDamage(CGameObject &Obj, int hit_player, int hit_damage);
int  gObjMonsterTopHitDamageUser(CGameObject &MonObj);
int  gObjMonsterLastHitDamageUser(CGameObject &MonObj, int & hitindex);
int gObjMonsterGetDamageUser(CGameObject &MonObj, CGameObject lpUserObj);
BOOL gObjMonsterMoveCheck(CGameObject &Obj, int tx, int ty);
BOOL gObjMonsterGetTargetPos(CGameObject &Obj);
BOOL gObjGetTargetPos(CGameObject &Obj, int sx, int sy, int & tx , int & ty);
int  gObjCallMonsterSetEnemy(CGameObject &Obj, int Target);
int  gObjMonsterSearchEnemy(CGameObject &Obj, BYTE objtype);
int  gObjGuardSearchEnemy(CGameObject &Obj);
void gObjMonsterStateProc(CGameObject &ObjSource, int aMsgCode, CGameObject &ObjTarget, int aMsgSubCode);
void gObjMonsterProcess(CGameObject &Obj);
void gObjMonsterMagicAttack(CGameObject &Obj, int iMonsterClass);
void gObjUseMonsterSpecialAbillity(CGameObject &MonsterObj);
void gObjMonsterBeattackRecv(BYTE * lpRecv, CGameObject &Obj);
void gObjMonsterAttack(CGameObject &Obj, CGameObject lpTargetObj);
BOOL PathFindMoveMsgSend(CGameObject &Obj);
void gObjMonsterMoveAction(CGameObject &Obj);
void gObjMonsterBaseAct(CGameObject &Obj);
void gObjTrapAttackEnemySearchX(CGameObject &Obj, int count);
void gObjTrapAttackEnemySearchY(CGameObject &Obj, int count);
void gObjTrapAttackEnemySearch(CGameObject &Obj);
void gObjTrapAttackEnemySearchRange(CGameObject &Obj,int iRange);
void gObjMonsterTrapAct(CGameObject &Obj);
BYTE NewOptionRand(int level, int OptCount);
BYTE NewOptionRand2(int MinOptCount, int MaxOptCount);
void InventoryDropItem(CGameObject &Obj, int DropItem);
BOOL IsCanNotItemDtopInDevilSquare(int ItemType);
void gObjRefillMonsterHP(CGameObject &MonsterObj, int iRefillHPSec);
void gObjMonsterDieGiveItem(CGameObject &Obj, CGameObject lpTargetObj);
BOOL gEventMonsterItemDrop(CGameObject &Obj, CGameObject lpTargetObj);
void gObjMonsterSummonSkillForLordSilvester(CGameObject &Obj);
void KillLordSilvesterRecallMon(int nLordSilvesterIndex, int nRecallMonIndex);
void BlockLordSilvesterEntrance();
void UnBlockLordSilvesterEntrance();
void SendLordSilvesterBlockInfo(CGameObject &Obj, BYTE btMapSetType);

static struct LORDSILVESTER_ENTRANCE_POS
{
	BYTE btStartX;
	BYTE btStartY;
	BYTE btEndX;
	BYTE btEndY;
} g_iLordSilvester_Entrance[6] =
{
	0x77, 0x5D, 0x7A, 0x5F,
	0x83, 0x5F, 0x86, 0x61,
	0x95, 0x6B, 0x97, 0x6E,
	0x8C, 0x81, 0x8E, 0x84,
	0x7A, 0x80, 0x7C, 0x83,
	0x6E, 0x76, 0x70, 0x7A
};

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

