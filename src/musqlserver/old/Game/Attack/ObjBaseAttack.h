////////////////////////////////////////////////////////////////////////////////
// ObjBaseAttack.h
#ifndef OBJBASEATTACK_H
#define OBJBASEATTACK_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CUserData.h"

class CObjBaseAttack
{

public:

	CObjBaseAttack();
	virtual ~CObjBaseAttack();

	int DecreaseArrow(CGameObject* lpObj);
	int CheckAttackArea(CGameObject* lpObj, CGameObject* lpTargetObj);
	int PkCheck(CGameObject* lpObj, CGameObject* lpTargetObj);
	int ResistanceCheck(CGameObject* lpObj, CGameObject* lpTargetObj, int skill);
	int MissCheckPvP(CGameObject* lpObj, CGameObject* lpTargetObj, int skill, int skillSuccess, int magicsend, BOOL& bAllMiss, BYTE RFAttack);
	int GetTargetDefense(CGameObject* lpObj, CGameObject* lpTargetObj, int& MsgDamage, int& iOriginTargetDefense);
	int GetPartyMemberCount(CGameObject* lpObj);

	virtual int MissCheck(CGameObject* lpObj, CGameObject* lpTargetObj, int skill, int skillSuccess, int magicsend, BOOL& bAllMiss, BYTE RFAttack);	// 4

};

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

