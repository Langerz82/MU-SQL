////////////////////////////////////////////////////////////////////////////////
// MonsterHerd.h
// ------------------------------
// Decompiled by Deathway
// Date : 2007-07-09
// ------------------------------
#ifndef MONSTERHERD_H
#define MONSTERHERD_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "User/CUserData.h"

struct _MONSTER_HERD_DATA 
{
	int m_iIndex;	// 0
	int m_iType;	// 4
	BYTE m_iX;	// 8
	BYTE m_iY;	// 9
	BOOL m_bRegen;	// C

	_MONSTER_HERD_DATA()
	{
		this->m_iIndex = -1;
		this->m_iType = -1;
		this->m_iX = -1;
		this->m_iY = -1;
		this->m_bRegen = FALSE;
	}
};


class MonsterHerd
{

public:	

	MonsterHerd();
	virtual ~MonsterHerd();

  	virtual BOOL SetTotalInfo(int iMapNumber, int iRadius, int iStartX, int iStartY);	// 4
	virtual BOOL AddMonster(int iMonsterType, BOOL bRegen, BOOL bAttackFirst);	// 8
	virtual void SetRadius(int iRadius);	// C
	virtual void SetPosition(BYTE iTX, BYTE iTY);	// 10
	virtual BOOL Start();	// 14
	virtual void Stop();	// 18
	virtual BOOL CheckInRadius(int iIndex);	// 1C
	virtual BOOL GetCurrentLocation(BYTE&cX, BYTE& cY);	// 20
	virtual BOOL GetRandomLocation(BYTE& cX, BYTE& cY);	// 24
	virtual BOOL CheckLocation(BYTE& cX, BYTE& cY);	// 28
	virtual int MoveHerd(BYTE iTX, BYTE iTY);	// 2C
	virtual int MonsterHerdItemDrop(LPGameObject &lpObj);	// 30
	virtual _MONSTER_HERD_DATA* GetMonsterData(int iIndex);	// 34
	virtual void BeenAttacked(LPGameObject &lpObj, CGameObject* lpTargetObj);	// 38
	virtual void OrderAttack(LPGameObject &lpObj, CGameObject* lpTargetObj, int iAttackPercent);	// 3C
	virtual void MonsterBaseAct(LPGameObject &lpObj);	// 40
	virtual void MonsterMoveAction(LPGameObject &lpObj);	// 44
	virtual void MonsterAttackAction(LPGameObject &lpObj, CGameObject* lpTargetObj);	// 48
	virtual void MonsterDieAction(LPGameObject &lpObj);	// 4C
	virtual int MonsterRegenAction(LPGameObject &lpObj);	// 50
	virtual int GetPosIndex();
	virtual int GetEndPosIndex();
	virtual void SetPosIndex(int nPosIndex);
	virtual BOOL IsMove();

public:

	int m_iHerdIndex;	// 4
	int m_iMapNumber;	// 8
	int m_iCUR_X;	// C
	int m_iCUR_Y;	// 10
	int m_iRADIUS;	// 14
	BOOL m_bHasInfo;	// 18
	BOOL m_bHerdActive;	// 1C
	std::map<int,_MONSTER_HERD_DATA> m_mapMonsterHerd;	// 20
	CRITICAL_SECTION m_critMonsterHerd;	// 30

};

#endif



////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

