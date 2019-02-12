#ifndef _MU_TMONSTERSKILLELEMENT_U
#define _MU_TMONSTERSKILLELEMENT_U

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "TSkillElement.h"

#define MAX_MONSTER_SKILL_ELEMENT	(200)

enum MONSTER_SKILL_ELEMENT_TYPE
{
	MSE_TYPE_STUN = 0x0,
	MSE_TYPE_MOVE = 0x1,
	MSE_TYPE_HP = 0x2,
	MSE_TYPE_MP = 0x3,
	MSE_TYPE_AG = 0x4,
	MSE_TYPE_DEFENSE = 0x5,
	MSE_TYPE_ATTACK = 0x6,
	MSE_TYPE_DURABILITY = 0x7,
	MSE_TYPE_SUMMON = 0x8,
	MSE_TYPE_PUSH = 0x9,
	MSE_TYPE_STAT_ENERGY = 0xa,
	MSE_TYPE_STAT_STRENGTH = 0xb,
	MSE_TYPE_STAT_DEXITERITY = 0xc,
	MSE_TYPE_STAT_VITALITY = 0xd,
	MSE_TYPE_REMOVE_SKILL = 0xe,
	MSE_TYPE_RESIST_SKILL = 0xf,
	MSE_TYPE_IMMUNE_SKILL = 0x10,
	MSE_TYPE_TELEPORT_SKILL = 0x11,
	MSE_TYPE_DOUBLE_HP = 0x12,
	MSE_TYPE_POISON = 0x13,
	MSE_TYPE_NORMALATTACK = 0x14,
	MSE_TYPE_IMMUNE_ALL_SKILL = 0x15,
	MSE_TYPE_PERCENT_DAMAGE = 0x16
};

enum MONSTER_SKILL_ELEMENT_INCDEC_TYPE
{
	MSE_INCDEC_TYPE_NONE = 0x0,
	MSE_INCDEC_TYPE_PERCENTINC = 0x1,
	MSE_INCDEC_TYPE_PERCENTDEC = 0x2,
	MSE_INCDEC_TYPE_CONSTANTINC = 0xb,
	MSE_INCDEC_TYPE_CONSTANTDEC = 0xc,
	MSE_INCDEC_TYPE_CYCLE_PERCENT = 0x64,
	MSE_INCDEC_TYPE_CYCLE_PERCENTINC = 0x65,
	MSE_INCDEC_TYPE_CYCLE_PERCENTDEC = 0x66,
	MSE_INCDEC_TYPE_CYCLE_CONSTANT = 0x6e,
	MSE_INCDEC_TYPE_CYCLE_CONSTANTINC = 0x6f,
	MSE_INCDEC_TYPE_CYCLE_CONSTANTDEC = 0x70,
};


class TMonsterSkillElement  : public  TSkillElement
{

public:

	virtual void ForceSkillElement(CGameObject &Obj, int iTargetIndex);	// 0
	TMonsterSkillElement();
	
	virtual ~TMonsterSkillElement();	// 4

	int ApplyElementStun(CGameObject &Obj, int iTargetIndex);
	int ApplyElementMove(CGameObject &Obj, int iTargetIndex);
	int ApplyElementHP(CGameObject &Obj, int iTargetIndex);
	int ApplyElementMP(CGameObject &Obj, int iTargetIndex);
	int ApplyElementAG(CGameObject &Obj, int iTargetIndex);
	int ApplyElementDefense(CGameObject &Obj, int iTargetIndex);
	int ApplyElementAttack(CGameObject &Obj, int iTargetIndex);
	int ApplyElementDurability(CGameObject &Obj, int iTargetIndex);
	int ApplyElementSummon(CGameObject &Obj, int iTargetIndex);
	int ApplyElementPush(CGameObject &Obj, int iTargetIndex);
	int ApplyElementStatEnergy(CGameObject &Obj, int iTargetIndex);
	int ApplyElementStatStrength(CGameObject &Obj, int iTargetIndex);
	int ApplyElementStatDexiterity(CGameObject &Obj, int iTargetIndex);
	int ApplyElementStatVitality(CGameObject &Obj, int iTargetIndex);
	int ApplyElementRemoveSkill(CGameObject &Obj, int iTargetIndex);
	int ApplyElementResistSkill(CGameObject &Obj, int iTargetIndex);
	int ApplyElementImmuneSkill(CGameObject &Obj, int iTargetIndex);
	int ApplyElementTeleportSkill(CGameObject &Obj, int iTargetIndex);
	int ApplyElementDoubleHP(CGameObject &Obj, int iTargetIndex);
	int ApplyElementPoison(CGameObject &Obj, int iTargetIndex);
	int ApplyElementNormalAttack(CGameObject &Obj, int iTargetIndex);
	int ApplyElementImmuneAllSkill(CGameObject &Obj, int iTargetIndex);
	int ApplyElementPercentDamageNormalAttack(CGameObject &Obj, int iTargetIndex);

	void Reset();
	char* GetElementName(){return this->m_szElementName;}

	static int LoadData(char* lpszFileName);
	static int DelAllSkillElement();
	static TMonsterSkillElement* FindSkillElement(int iElementNumber);

private:

	char m_szElementName[50];	// 4
	int m_iElementNumber;	// 18
	int m_iElementType;	// 1C
	int m_iSuccessRate;	// 20
	int m_iContinuanceTime;	// 24
	int m_iIncAndDecType;	// 28
	int m_iIncAndDecValue;	// 2C
	int m_iNullifiedSkill;	// 30
	int m_iCharacterClass;	// 34
	int m_iCharacterLevel;	// 38

public:

	static BOOL s_bDataLoad;
	static TMonsterSkillElement s_MonsterSkillElementArray[MAX_MONSTER_SKILL_ELEMENT];

};

#endif
