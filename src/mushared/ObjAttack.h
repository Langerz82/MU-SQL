////////////////////////////////////////////////////////////////////////////////
// ObjAttack.h
#ifndef OBJATTACK_H
#define OBJATTACK_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MagicInf.h"
#include "ObjBaseAttack.h"

class CObjAttack : public CObjBaseAttack
{

public:

	CObjAttack();
	virtual ~CObjAttack();

	void Init();

	BOOL Attack(LPOBJ lpObj, LPOBJ lpTargetObj, CMagicInf* lpMagic,  int magicsend, unsigned char MSBFlag, int AttackDamage, BOOL bCombo, BYTE RFAttack, BYTE byReflect, BYTE byPentagramAttack);
	BOOL PentagramAttack(LPOBJ lpObj, LPOBJ lpTargetObj, CMagicInf* lpMagic, BYTE MSBFlag, int MsgDamage, int AttackDamage, int iTargetDefense);
	int GetAttackDamage(LPOBJ lpObj, LPOBJ lpTargetObj, int targetDefense, int& effect, CMagicInf* lpMagic);
	int GetAttackDamageWizard(LPOBJ lpObj, LPOBJ lpTargetObj, int targetDefense, CMagicInf* lpMagic, int& effect);
	int GetAttackDamageSummoner(LPOBJ lpObj, LPOBJ lpTargetObj, int targetDefense, CMagicInf* lpMagic, int& effect);
	int GetShieldDamage(LPOBJ lpObj, LPOBJ lpTargetObj, int iAttackDamage);
	int GetBuffTypePhysicalIncrease(LPOBJ lpObj, int *iAttackBerserkerMin, int *iAttackBerserkerMax, int nBuffType);
	int GetElementalDamage(LPOBJ lpObj, LPOBJ lpTargetObj, char* DamageType1, char* DamageType2, int iAttackDamage, int iTargetDefense);

private:
	MULua m_Lua;

};

BOOL gObjDenorantSprite(LPOBJ lpObj);
BOOL gObjDarkHorse(LPOBJ lpObj);
BOOL gObjFenrir(LPOBJ lpObj);

extern CObjAttack gclassObjAttack;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

