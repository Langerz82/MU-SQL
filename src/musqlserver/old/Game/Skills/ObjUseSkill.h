////////////////////////////////////////////////////////////////////////////////
// ObjUseSkill.h
#ifndef OBJUSESKILL_H
#define OBJUSESKILL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MagicInf.h"
#include "User/CUserData.h"
#include "MuLua.h"

struct SkillConfig
{
	int SternTime;
	int SwellManaTime;
	int CloakingTime;

	int IceArrowTime;

	int PoisonPercent; // custom
	int DeathPoisonPercent; // custom

	bool EnableSiegeOnAllMaps;
	bool UseCalcForTripleShot; // custom - joinmu

	int PoisonSkillSuccessRate;
	int PoisonSkillTime;

	bool EnableWizardSkillWhileTeleport;
};

class CObjUseSkill
{

public:

	CObjUseSkill();
	virtual ~CObjUseSkill();
	friend class CObjAttack;

	bool LoadSkillInfo();

	int SpeedHackCheck(CGameObject &Obj);
	int EnableSkill(WORD Skill);
	int GetUseMana(CGameObject &Obj, CMagicInf* lpMagic);
	int GetAddUseMana(CGameObject &Obj, CMagicInf* lpMagic);
	int GetUseBP(CGameObject &Obj, CMagicInf* lpMagic);
	void UseSkill(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic);
	void UseSkill(CGameObject &Obj, CMagicInf* lpMagic, BYTE x, BYTE y, BYTE dir, BYTE TargetPos, int aTargetIndex);
	int RunningSkill(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic, BOOL bCombo);
	void MaGumSkillDefenseDown(CGameObject &Obj, int aTargetIndex, int skill_level);
	void KnightSkillAddLife(CGameObject &Obj, CMagicInf * lpMagic);
	void WizardMagicDefense(CGameObject &Obj, int aTargetIndex, CMagicInf * lpMagic);
	void SkillDefense(CGameObject &Obj, int aTargetIndex, CMagicInf * lpMagic);
	void SkillAttack(CGameObject &Obj, int aTargetIndex, CMagicInf * lpMagic);
	int SkillChangeUse(CGameObject &Obj);
	void SkillHealing(CGameObject &Obj, int aTargetIndex, CMagicInf * lpMagic);
	int SkillMonsterCall(CGameObject &Obj, int MonsterType, int x, int y);
	int SkillDeathPoison(CGameObject &Obj, CMagicInf* lpMagic, BYTE x, BYTE y, BYTE dir, BYTE TargetPos, int aTargetIndex);
	int SkillDeathPoisonForMedusa(CGameObject &Obj, CMagicInf* lpMagic, BYTE x, BYTE y, int aTargetIndex);
	int SkillSuddenIce(CGameObject &Obj, CMagicInf* lpMagic, BYTE x, BYTE y, BYTE dir, BYTE TargetPos, int aTargetIndex);
	int SkillHellFire2Start(CGameObject &Obj, CMagicInf* lpMagic);
	int SkillHellFire2(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic);
	void SkillBlowOfFury(CGameObject &Obj, CMagicInf* lpMagic, BYTE x, BYTE y, BYTE dir, BYTE TargetPos, int aTargetIndex, BOOL isCombo);
	void SkillWheel(CGameObject &Obj, CMagicInf* lpMagic, int aTargetIndex, BOOL isCombo);
	int SkillKnightBlow(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic, BOOL isCombo);
	void SkillPowerSlash(CGameObject &Obj, CMagicInf* lpMagic, BYTE x, BYTE y, BYTE dir, BYTE Targetangle, int aTargetIndex);
	void SkillFrustrum(CGameObject &Obj, float fangle, float tx, float ty);
	int GetTargetLinePath(int sx, int sy, int tx, int ty,  int* PathX,int* PathY, int distance);
	int GetAngle(int x, int y, int tx,  int ty);
	int CalDistance(int x, int y, int x1, int y1);
	void SkillFrustrum2(CGameObject &Obj, float fangle, float tx, float ty);
	void SkillFrustrum3(CGameObject &Obj, float fangle, float tx, float ty, float f1, float f2);
	int SkillAreaCheck(int angel, int x, int y,  int tx, int ty,  int dis,  int arc);
	int SkillSpear(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillFireBurst(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillDarkHorseAttack(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic);
	int RecallPartyCheck(CGameObject &Obj, int skill_level);
	void SkillRecallParty(CGameObject &Obj, int skill_level);
	void SkillAddCriticalDamage(CGameObject &Obj, int skill_level, CMagicInf* lpMagic);
	void SkillElectricSpark(CGameObject &Obj, CMagicInf* lpMagic, BYTE x, BYTE y, BYTE dir, BYTE TargetPos, int aTargetIndex);
	int SkillSummon(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillImmuneToMagic(CGameObject &Obj, CMagicInf* lpMagic);
	int SkillImmuneToHarm(CGameObject &Obj, CMagicInf* lpMagic);
	void SkillKnightRush(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillWizardJavalin(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillElfRush(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillMagumOneFlash(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillMagumDeathCannon(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillDarklordSpaceSplit(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillBrandOfSkill(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillRemoveStun(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillAddMana(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillCloaking(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillRemoveCloaking(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillRemoveMagic(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic);
	int RemoveCloakingEffect(CGameObject &Obj);
	int SkillStun(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic, BYTE btX, BYTE btY, BYTE btDir, BYTE btTargetPos);
	int RemoveAllCharacterInvalidMagicAndSkillState(CGameObject &Obj);
	int RemoveSpecificSkillState(CGameObject &Obj, int iSkillNumber);
	int SkillFenrirAttack(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillInfinityArrow(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillFireScream(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic);
	int FireScreamExplosionAttack(CGameObject &Obj, CGameObject lpTargetObj, int iAttackDamage, int iAddDamage);
	void SpecificSkillAdditionTreat(CGameObject &Obj, CGameObject lpTargetObj, CMagicInf* lpMagic, int iAttackDamage);
	int SkillBookSahamutt(CGameObject &Obj, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex);
	int SkillSummonerSleep(CGameObject &Obj, int aTargetIndex, CMagicInf * lpMagic);
	int SkillDrainLife(CGameObject &Obj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillDamageReflect(CGameObject &Obj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillBookNeil(CGameObject &Obj, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex);
	int SkillInnovation(CGameObject &Obj, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex);
	int SkillWeakness(CGameObject &Obj, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex);
	int SkillChainLighting(CGameObject &Obj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillBlowOfDestruction(CGameObject &Obj, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex, BOOL bCombo);
	int SkillAreaMonsterAttack(CGameObject &Obj, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex, int Distance, int MultiAttack, int AttackDelay);
	void SkillLightingShock(CGameObject &Obj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillMagicCircle(CGameObject &Obj, int aTargetIndex, CMagicInf * lpMagic);
	int SkillShieldRefill(CGameObject &Obj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillBerserker(CGameObject &Obj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillGiganticStorm(CGameObject &Obj, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex);
	int SkillFiveShot(CGameObject &Obj, CMagicInf * lpMagic, BYTE TargetPos, int aTargetIndex);
	int SkillBirds(CGameObject &Obj, CMagicInf * lpMagic, BYTE TargetPos, int aTargetIndex);
	int SkillFlameStrike(CGameObject &Obj, CMagicInf * lpMagic, BYTE TargetPos, int aTargetIndex);
	void NewSkillProc(CGameObject &Obj, CMagicInf *lpMagic, BYTE x, BYTE y, CGameObject &ObjTarget, int aMsgSubCode, BOOL bCombo)
	int SkillMonkBuff(CGameObject &Obj, CMagicInf * lpMagic);
	int SkillMonkBuffApplyParty(CGameObject &Obj, CMagicInf * lpMagic);
	int SkillMonkBarrageJustOneTarget(CGameObject &Obj, CMagicInf *lpMagic, int aTargetIndex);
	int SkillCharge(CGameObject &Obj, CMagicInf *lpMagic, int aTargetIndex);
	int SkillMonkDarkSideGetTargetIndex(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic, WORD *Target);
	void SkillDarkSide(int aindex, int aTargetIndex, CMagicInf * lpMagic);
	int SkillPhoenixShot(CGameObject &Obj, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex);
	int SkillEvilMonster(CGameObject &Obj, int aTargetIndex, CMagicInf *lpMagic);
	void SplashDamage(CGameObject &Obj, CGameObject lpTargetObj, int nSkill, int nAttackDamage, int nDistance, int nDamageRate);
	void SkillSummonForLordSilvester(CGameObject &Obj, int iTargetIndex, CMagicInf *lpMagic);
	void SkillSpinStep(CGameObject &Obj, int iTargetIndex, CMagicInf *lpMagic);
	void SkillGrowLancerAttackOneTarget(CGameObject &Obj, int aTargetIndex, CMagicInf *lpMagic);
	int SkillBreche(CGameObject &Obj, CMagicInf* lpMagic, BYTE x, BYTE y, int aTargetIndex);
	int SkillShiningPeak(CGameObject &Obj, CMagicInf *lpMagic, int aTargetIndex);
	int SkillBuffGrowLancer(CGameObject &Obj, CMagicInf *lpMagic, int aTargetIndex);
	int SkillBuffPartyGrowLancer(CGameObject &Obj, CMagicInf *lpMagic, int aTargetIndex);
	
	void SkillWindSoul(CGameObject lpObj, CMagicInf * lpMagic, int aTargetIndex);
	void SkillFireBlood(CGameObject lpObj, CMagicInf * lpMagic, int aTargetIndex);
	void SkillIceBlood(CGameObject lpObj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillFireBeast(CGameObject &Obj, int aTargetIndex, CMagicInf *lpMagic);
	int SkillIceBeast(CGameObject &Obj, int aTargetIndex, CMagicInf *lpMagic);
	int SkillMeteorStorm (CGameObject &Obj, CMagicInf* lpMagic, BYTE x, BYTE y, int aTargetIndex);
	void SkillSoulSeker(CGameObject lpObj, CMagicInf * lpMagic, int aTargetIndex);
	void SkillDarkBlast(CGameObject lpObj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillArchangelWillBuff(CGameObject &Obj, CMagicInf * lpMagic);

	SkillConfig	m_SkillData;
	MULua m_Lua;
};

int  DecPartyMemberHPandMP(CGameObject &Obj);

extern CObjUseSkill gObjUseSkill;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

