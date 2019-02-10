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

	BOOL Attack(CGameObject* lpObj, CGameObject* lpTargetObj, CMagicInf* lpMagic,  int magicsend, unsigned char MSBFlag, int AttackDamage, BOOL bCombo, BYTE RFAttack, BYTE byReflect, BYTE byPentagramAttack);
	BOOL PentagramAttack(CGameObject* lpObj, CGameObject* lpTargetObj, CMagicInf* lpMagic, BYTE MSBFlag, int MsgDamage, int AttackDamage, int iTargetDefense);
	int GetAttackDamage(CGameObject* lpObj, CGameObject* lpTargetObj, int targetDefense, int& effect, CMagicInf* lpMagic);
	int GetAttackDamageWizard(CGameObject* lpObj, CGameObject* lpTargetObj, int targetDefense, CMagicInf* lpMagic, int& effect);
	int GetAttackDamageSummoner(CGameObject* lpObj, CGameObject* lpTargetObj, int targetDefense, CMagicInf* lpMagic, int& effect);
	int GetShieldDamage(CGameObject* lpObj, CGameObject* lpTargetObj, int iAttackDamage);
	int GetBuffTypePhysicalIncrease(CGameObject* lpObj, int *iAttackBerserkerMin, int *iAttackBerserkerMax, int nBuffType);
	int GetElementalDamage(CGameObject* lpObj, CGameObject* lpTargetObj, char* DamageType1, char* DamageType2, int iAttackDamage, int iTargetDefense);

private:
	MULua m_Lua;

};

BOOL gObjDenorantSprite(CGameObject* lpObj);
BOOL gObjDarkHorse(CGameObject* lpObj);
BOOL gObjFenrir(CGameObject* lpObj);

extern CObjAttack gclassObjAttack;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

