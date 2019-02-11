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

	int DecreaseArrow(LPGameObject &lpObj);
	int CheckAttackArea(LPGameObject &lpObj, LPGameObject lpTargetObj);
	int PkCheck(LPGameObject &lpObj, LPGameObject lpTargetObj);
	int ResistanceCheck(LPGameObject &lpObj, LPGameObject lpTargetObj, int skill);
	int MissCheckPvP(LPGameObject &lpObj, LPGameObject lpTargetObj, int skill, int skillSuccess, int magicsend, BOOL& bAllMiss, BYTE RFAttack);
	int GetTargetDefense(LPGameObject &lpObj, LPGameObject lpTargetObj, int& MsgDamage, int& iOriginTargetDefense);
	int GetPartyMemberCount(LPGameObject &lpObj);

	virtual int MissCheck(LPGameObject &lpObj, LPGameObject lpTargetObj, int skill, int skillSuccess, int magicsend, BOOL& bAllMiss, BYTE RFAttack);	// 4

};

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

