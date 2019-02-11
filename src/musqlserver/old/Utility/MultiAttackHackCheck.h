////////////////////////////////////////////////////////////////////////////////
// MultiAttackHackCheck.h
#ifndef MULTIATTACKHACKCHECK_H
#define MULTIATTACKHACKCHECK_H

#include "StdAfx.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef OBJ_MONSTER
	#define OBJ_MONSTER 2
#endif

#define MAX_MULTI_ATTACK_INFO 40
#define MULTI_ATTACK_SERIAL_SIZE 255

struct MULTIATTACK_HACKCHECK
{
	short number;	// 0
	BYTE Serial[MULTI_ATTACK_SERIAL_SIZE];	// 2
};

class CMultiAttackHackCheck
{

public:

	CMultiAttackHackCheck();
	virtual ~CMultiAttackHackCheck();

	void Init();
	BOOL Insert(CGameObject &aTarget, BYTE skillnum, BYTE serial);
	int CheckPenetrationSkill(int aTargetIndex, BYTE skillnum, BYTE serial);
	int CheckFireScreamSkill(int iTargetIndex, BYTE btSkillnum, BYTE btSerial);

private:

	int m_iCount;	// 4
	MULTIATTACK_HACKCHECK m_Table[MAX_MULTI_ATTACK_INFO];	// 8

};

extern CMultiAttackHackCheck * gMultiAttackHackCheck;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

