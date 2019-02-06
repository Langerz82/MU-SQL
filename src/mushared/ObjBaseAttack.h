////////////////////////////////////////////////////////////////////////////////
// ObjBaseAttack.h
#ifndef OBJBASEATTACK_H
#define OBJBASEATTACK_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "user.h"

class CObjBaseAttack
{

public:

	CObjBaseAttack();
	virtual ~CObjBaseAttack();

	int DecreaseArrow(LPOBJ lpObj);
	int CheckAttackArea(LPOBJ lpObj, LPOBJ lpTargetObj);
	int PkCheck(LPOBJ lpObj, LPOBJ lpTargetObj);
	int ResistanceCheck(LPOBJ lpObj, LPOBJ lpTargetObj, int skill);
	int MissCheckPvP(LPOBJ lpObj, LPOBJ lpTargetObj, int skill, int skillSuccess, int magicsend, BOOL& bAllMiss, BYTE RFAttack);
	int GetTargetDefense(LPOBJ lpObj, LPOBJ lpTargetObj, int& MsgDamage, int& iOriginTargetDefense);
	int GetPartyMemberCount(LPOBJ lpObj);

	virtual int MissCheck(LPOBJ lpObj, LPOBJ lpTargetObj, int skill, int skillSuccess, int magicsend, BOOL& bAllMiss, BYTE RFAttack);	// 4

};

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

