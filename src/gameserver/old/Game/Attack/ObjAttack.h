////////////////////////////////////////////////////////////////////////////////
// ObjAttack.h
#ifndef OBJATTACK_H
#define OBJATTACK_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MagicInf.h"
#include "ObjBaseAttack.h"
#include "MuLua.h"

class CObjAttack : public CObjBaseAttack
{

public:

	CObjAttack();
	virtual ~CObjAttack();

	void Init();

	BOOL Attack(CGameObject &Obj, CGameObject &TargetObj, CMagicInf* lpMagic,  int magicsend, BYTE MSBFlag, int AttackDamage, BOOL bCombo, BYTE RFAttack, BYTE byReflect, BYTE byPentagramAttack);
	BOOL PentagramAttack(CGameObject &Obj, CGameObject &TargetObj, CMagicInf* lpMagic, BYTE MSBFlag, int MsgDamage, int AttackDamage, int iTargetDefense);
	int GetAttackDamage(CGameObject &Obj, CGameObject &TargetObj, int targetDefense, int& effect, CMagicInf* lpMagic);
	int GetAttackDamageWizard(CGameObject &Obj, CGameObject &TargetObj, int targetDefense, CMagicInf* lpMagic, int& effect);
	int GetAttackDamageSummoner(CGameObject &Obj, CGameObject &TargetObj, int targetDefense, CMagicInf* lpMagic, int& effect);
	int GetShieldDamage(CGameObject &Obj, CGameObject &TargetObj, int iAttackDamage);
	int GetBuffTypePhysicalIncrease(CGameObject &Obj, int *iAttackBerserkerMin, int *iAttackBerserkerMax, int nBuffType);
	int GetElementalDamage(CGameObject &Obj, CGameObject &TargetObj, char* DamageType1, char* DamageType2, int iAttackDamage, int iTargetDefense);

private:
	MULua m_Lua;

};

BOOL gObjDenorantSprite(CGameObject &Obj);
BOOL gObjDarkHorse(CGameObject &Obj);
BOOL gObjFenrir(CGameObject &Obj);

extern CObjAttack gcObjAttack;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

