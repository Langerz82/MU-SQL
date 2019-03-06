////////////////////////////////////////////////////////////////////////////////
// AntiSpeed.h
#ifndef _ANTI_SPEED_H
#define _ANTI_SPEED_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "GameProtocol.h"
#include "CUserData.h"

#include <queue>

class CUserData;

class CAttackBase{
public:
	bool m_InUse;

	virtual void Process() = 0;
};

class CAttackMelee : public CAttackBase{
	CGameObject*  m_TargetObj;
	CGameObject* m_Obj;
public:
	CAttackMelee(CGameObject &Obj, CGameObject &TargetObj);
	void Process();
};

class CAttackMagic : public CAttackBase{
	CGameObject*  m_Obj;
	BYTE* m_Msg;
public:
	CAttackMagic(CGameObject &Obj, BYTE* pmsg, int len);
	~CAttackMagic();
	void Process();
};

class CAttackRange : public CAttackBase{
	CGameObject*  m_Obj;
	BYTE* m_Msg;
	int m_Type;
public:
	CAttackRange(CGameObject &Obj, BYTE* pmsg, int len, int type);
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
	CGameObject* m_Obj;
	BYTE* m_Msg;
	int m_Len;
	int m_Type;
public:
	CAttackMsg(CGameObject &Obj, BYTE* pmsg, int len, int type);
	~CAttackMsg();

	void Process();
};

class CAttackHandler{
public:
	CAttackMsg* m_pMsg;
};

class CAttackQueue{
private:
	CGameObject &Obj;
	std::queue<CAttackMsg*> m_Queue;
	

	void CheckSize();
	void Clear();
public:
	int m_SwordDelay;
	int m_HandDelay;
	int m_MagicDelay;
	
	bool m_LastFrameAttack;

	CAttackQueue(CGameObject* lpObj);
	~CAttackQueue();
//	void Push(CGameObject & lpTargetObj);
//	void Push(BYTE* msg, int len);
	void Push(BYTE* msg, int len, int type);

	void ProcessQueue();
	static bool ThreadActive;
	static VOID AttackQueueProc(std::vector<CGameObject*> gObj);
	
	CGameObject* m_Obj;
	//CRITICAL_SECTION m_CritQueue;
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

