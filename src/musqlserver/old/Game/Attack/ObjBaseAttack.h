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

	int DecreaseArrow(CGameObject &Obj);
	int CheckAttackArea(CGameObject &Obj, CGameObject lpTargetObj);
	int PkCheck(CGameObject &Obj, CGameObject lpTargetObj);
	int ResistanceCheck(CGameObject &Obj, CGameObject lpTargetObj, int skill);
	int MissCheckPvP(CGameObject &Obj, CGameObject lpTargetObj, int skill, int skillSuccess, int magicsend, BOOL& bAllMiss, BYTE RFAttack);
	int GetTargetDefense(CGameObject &Obj, CGameObject lpTargetObj, int& MsgDamage, int& iOriginTargetDefense);
	int GetPartyMemberCount(CGameObject &Obj);

	virtual int MissCheck(CGameObject &Obj, CGameObject lpTargetObj, int skill, int skillSuccess, int magicsend, BOOL& bAllMiss, BYTE RFAttack);	// 4

};

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

