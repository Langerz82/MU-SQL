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

	BOOL Attack(LPGameObject &lpObj, LPGameObject lpTargetObj, CMagicInf* lpMagic,  int magicsend, unsigned char MSBFlag, int AttackDamage, BOOL bCombo, BYTE RFAttack, BYTE byReflect, BYTE byPentagramAttack);
	BOOL PentagramAttack(LPGameObject &lpObj, LPGameObject lpTargetObj, CMagicInf* lpMagic, BYTE MSBFlag, int MsgDamage, int AttackDamage, int iTargetDefense);
	int GetAttackDamage(LPGameObject &lpObj, LPGameObject lpTargetObj, int targetDefense, int& effect, CMagicInf* lpMagic);
	int GetAttackDamageWizard(LPGameObject &lpObj, LPGameObject lpTargetObj, int targetDefense, CMagicInf* lpMagic, int& effect);
	int GetAttackDamageSummoner(LPGameObject &lpObj, LPGameObject lpTargetObj, int targetDefense, CMagicInf* lpMagic, int& effect);
	int GetShieldDamage(LPGameObject &lpObj, LPGameObject lpTargetObj, int iAttackDamage);
	int GetBuffTypePhysicalIncrease(LPGameObject &lpObj, int *iAttackBerserkerMin, int *iAttackBerserkerMax, int nBuffType);
	int GetElementalDamage(LPGameObject &lpObj, LPGameObject lpTargetObj, char* DamageType1, char* DamageType2, int iAttackDamage, int iTargetDefense);

private:
	MULua m_Lua;

};

BOOL gObjDenorantSprite(LPGameObject &lpObj);
BOOL gObjDarkHorse(LPGameObject &lpObj);
BOOL gObjFenrir(LPGameObject &lpObj);

extern CObjAttack gclassObjAttack;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

