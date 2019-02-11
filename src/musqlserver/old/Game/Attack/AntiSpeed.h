////////////////////////////////////////////////////////////////////////////////
// AntiSpeed.h
#ifndef _ANTI_SPEED_H
#define _ANTI_SPEED_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "GameProtocol.h"
#include "CGameObject.h"
#include "custTypedef.h"
#include "MuDefines.h"

#include <queue>


class CAttackBase{
public:
	bool m_InUse;

	virtual void Process() = 0;
};

class CAttackMelee : public CAttackBase{
	LPGameObject  m_TargetObj;
	LPGameObject  m_Obj;
public:
	CAttackMelee(LPGameObject &lpObj, LPGameObject &lpTargetObj);
	void Process();
};

class CAttackMagic : public CAttackBase{
	LPGameObject  m_Obj;
	BYTE* m_Msg;
public:
	CAttackMagic(LPGameObject &lpObj, BYTE* pmsg, int len);
	~CAttackMagic();
	void Process();
};

class CAttackRange : public CAttackBase{
	LPGameObject  m_Obj;
	BYTE* m_Msg;
	int m_Type;
public:
	CAttackRange(LPGameObject &lpObj, BYTE* pmsg, int len, int type);
	~CAttackRange();

	void Process();
};


enum ATTACK_TYPE{
	ATTACK_MELEE = 0,
	ATTACK_MAGIC = 1,
	ATTACK_RANGE_OLD = 2,
	ATTACK_RANGE_NEW = 3,
	ATTACK_RAGE_FIGHTER = 4,
};

class CAttackMsg{
	LPGameObject m_Obj;
	BYTE* m_Msg;
	int m_Len;
	int m_Type;
public:
	CAttackMsg(LPGameObject &lpObj, BYTE* pmsg, int len, int type);
	~CAttackMsg();

	void Process();
};

class CAttackHandler{
public:
	CAttackMsg* m_pMsg;
};

class CAttackQueue{
private:
	int aIndex;
	std::queue<CAttackMsg*> m_Queue;
	

	void CheckSize();
	void Clear();
public:
	int m_SwordDelay;
	int m_HandDelay;
	int m_MagicDelay;
	
	bool m_LastFrameAttack;

	CAttackQueue(LPGameObject &lpObj);
	~CAttackQueue();
//	void Push(LPGameObject & lpTargetObj);
//	void Push(unsigned char* msg, int len);
	void Push(unsigned char* msg, int len, int type);

	void ProcessQueue();
	static bool ThreadActive;
	static VOID AttackQueueProc(std::vector<LPGameObject> gObj);
	
	LPGameObject m_Obj;
	CRITICAL_SECTION m_CritQueue;
};

class CAntiSpeed{
private:
	std::queue<CAttackHandler*> m_Queue;
public:
	void ProcessQueue();

	static bool GlobalThreadActive;
	static VOID GlobalQueueProc();
};

extern CAntiSpeed* g_AntiSpeed;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

