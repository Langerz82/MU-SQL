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

	int SpeedHackCheck(LPGameObject &lpObj);
	int EnableSkill(WORD Skill);
	int GetUseMana(LPGameObject &lpObj, CMagicInf* lpMagic);
	int GetAddUseMana(LPGameObject &lpObj, CMagicInf* lpMagic);
	int GetUseBP(LPGameObject &lpObj, CMagicInf* lpMagic);
	void UseSkill(LPGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	void UseSkill(LPGameObject &lpObj, CMagicInf* lpMagic, BYTE x, BYTE y, BYTE dir, BYTE TargetPos, int aTargetIndex);
	int RunningSkill(LPGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic, BOOL bCombo);
	void MaGumSkillDefenseDown(LPGameObject &lpObj, int aTargetIndex, int skill_level);
	void KnightSkillAddLife(LPGameObject &lpObj, CMagicInf * lpMagic);
	void WizardMagicDefense(LPGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic);
	void SkillDefense(LPGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic);
	void SkillAttack(LPGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic);
	int SkillChangeUse(LPGameObject &lpObj);
	void SkillHealing(LPGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic);
	int SkillMonsterCall(LPGameObject &lpObj, int MonsterType, int x, int y);
	int SkillDeathPoison(LPGameObject &lpObj, CMagicInf* lpMagic, BYTE x, BYTE y, BYTE dir, BYTE TargetPos, int aTargetIndex);
	int SkillDeathPoisonForMedusa(LPGameObject &lpObj, CMagicInf* lpMagic, BYTE x, BYTE y, int aTargetIndex);
	int SkillSuddenIce(LPGameObject &lpObj, CMagicInf* lpMagic, BYTE x, BYTE y, BYTE dir, BYTE TargetPos, int aTargetIndex);
	int SkillHellFire2Start(LPGameObject &lpObj, CMagicInf* lpMagic);
	int SkillHellFire2(LPGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	void SkillBlowOfFury(LPGameObject &lpObj, CMagicInf* lpMagic, BYTE x, BYTE y, BYTE dir, BYTE TargetPos, int aTargetIndex, BOOL isCombo);
	void SkillWheel(LPGameObject &lpObj, CMagicInf* lpMagic, int aTargetIndex, BOOL isCombo);
	int SkillKnightBlow(LPGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic, BOOL isCombo);
	void SkillPowerSlash(LPGameObject &lpObj, CMagicInf* lpMagic, BYTE x, BYTE y, BYTE dir, BYTE Targetangle, int aTargetIndex);
	void SkillFrustrum(LPGameObject &lpObj, float fangle, float tx, float ty);
	int GetTargetLinePath(int sx, int sy, int tx, int ty,  int* PathX,int* PathY, int distance);
	int GetAngle(int x, int y, int tx,  int ty);
	int CalDistance(int x, int y, int x1, int y1);
	void SkillFrustrum2(LPGameObject &lpObj, float fangle, float tx, float ty);
	void SkillFrustrum3(LPGameObject &lpObj, float fangle, float tx, float ty, float f1, float f2);
	int SkillAreaCheck(int angel, int x, int y,  int tx, int ty,  int dis,  int arc);
	int SkillSpear(LPGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillFireBurst(LPGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillDarkHorseAttack(LPGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int RecallPartyCheck(LPGameObject &lpObj, int skill_level);
	void SkillRecallParty(LPGameObject &lpObj, int skill_level);
	void SkillAddCriticalDamage(LPGameObject &lpObj, int skill_level, CMagicInf* lpMagic);
	void SkillElectricSpark(LPGameObject &lpObj, CMagicInf* lpMagic, BYTE x, BYTE y, BYTE dir, BYTE TargetPos, int aTargetIndex);
	int SkillSummon(LPGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillImmuneToMagic(LPGameObject &lpObj, CMagicInf* lpMagic);
	int SkillImmuneToHarm(LPGameObject &lpObj, CMagicInf* lpMagic);
	void SkillKnightRush(LPGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillWizardJavalin(LPGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillElfRush(LPGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillMagumOneFlash(LPGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillMagumDeathCannon(LPGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillDarklordSpaceSplit(LPGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillBrandOfSkill(LPGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillRemoveStun(LPGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillAddMana(LPGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillCloaking(LPGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillRemoveCloaking(LPGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillRemoveMagic(LPGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int RemoveCloakingEffect(LPGameObject &lpObj);
	int SkillStun(LPGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic, BYTE btX, BYTE btY, BYTE btDir, BYTE btTargetPos);
	int RemoveAllCharacterInvalidMagicAndSkillState(LPGameObject &lpObj);
	int RemoveSpecificSkillState(LPGameObject &lpObj, int iSkillNumber);
	int SkillFenrirAttack(LPGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillInfinityArrow(LPGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillFireScream(LPGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int FireScreamExplosionAttack(LPGameObject &lpObj, LPGameObject lpTargetObj, int iAttackDamage, int iAddDamage);
	void SpecificSkillAdditionTreat(LPGameObject &lpObj, LPGameObject lpTargetObj, CMagicInf* lpMagic, int iAttackDamage);
	int SkillBookSahamutt(LPGameObject &lpObj, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex);
	int SkillSummonerSleep(LPGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic);
	int SkillDrainLife(LPGameObject &lpObj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillDamageReflect(LPGameObject &lpObj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillBookNeil(LPGameObject &lpObj, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex);
	int SkillInnovation(LPGameObject &lpObj, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex);
	int SkillWeakness(LPGameObject &lpObj, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex);
	int SkillChainLighting(LPGameObject &lpObj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillBlowOfDestruction(LPGameObject &lpObj, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex, BOOL bCombo);
	int SkillAreaMonsterAttack(LPGameObject &lpObj, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex, int Distance, int MultiAttack, int AttackDelay);
	void SkillLightingShock(LPGameObject &lpObj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillMagicCircle(LPGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic);
	int SkillShieldRefill(LPGameObject &lpObj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillBerserker(LPGameObject &lpObj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillGiganticStorm(LPGameObject &lpObj, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex);
	int SkillFiveShot(LPGameObject &lpObj, CMagicInf * lpMagic, BYTE TargetPos, int aTargetIndex);
	int SkillBirds(LPGameObject &lpObj, CMagicInf * lpMagic, BYTE TargetPos, int aTargetIndex);
	int SkillFlameStrike(LPGameObject &lpObj, CMagicInf * lpMagic, BYTE TargetPos, int aTargetIndex);
	void NewSkillProc(LPGameObject &lpObj, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex, int aMsgSubCode, BOOL bCombo);
	int SkillMonkBuff(LPGameObject &lpObj, CMagicInf * lpMagic);
	int SkillMonkBuffApplyParty(LPGameObject &lpObj, CMagicInf * lpMagic);
	int SkillMonkBarrageJustOneTarget(LPGameObject &lpObj, CMagicInf *lpMagic, int aTargetIndex);
	int SkillCharge(LPGameObject &lpObj, CMagicInf *lpMagic, int aTargetIndex);
	int SkillMonkDarkSideGetTargetIndex(LPGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic, WORD *Target);
	void SkillDarkSide(int aindex, int aTargetIndex, CMagicInf * lpMagic);
	int SkillPhoenixShot(LPGameObject &lpObj, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex);
	int SkillEvilMonster(LPGameObject &lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void SplashDamage(LPGameObject &lpObj, LPGameObject lpTargetObj, int nSkill, int nAttackDamage, int nDistance, int nDamageRate);
	void SkillSummonForLordSilvester(LPGameObject &lpObj, int iTargetIndex, CMagicInf *lpMagic);
	void SkillSpinStep(LPGameObject &lpObj, int iTargetIndex, CMagicInf *lpMagic);
	void SkillGrowLancerAttackOneTarget(LPGameObject &lpObj, int aTargetIndex, CMagicInf *lpMagic);
	int SkillBreche(LPGameObject &lpObj, CMagicInf* lpMagic, BYTE x, BYTE y, int aTargetIndex);
	int SkillShiningPeak(LPGameObject &lpObj, CMagicInf *lpMagic, int aTargetIndex);
	int SkillBuffGrowLancer(LPGameObject &lpObj, CMagicInf *lpMagic, int aTargetIndex);
	int SkillBuffPartyGrowLancer(LPGameObject &lpObj, CMagicInf *lpMagic, int aTargetIndex);
	
	void SkillWindSoul(LPGameObject lpObj, CMagicInf * lpMagic, int aTargetIndex);
	void SkillFireBlood(LPGameObject lpObj, CMagicInf * lpMagic, int aTargetIndex);
	void SkillIceBlood(LPGameObject lpObj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillFireBeast(LPGameObject &lpObj, int aTargetIndex, CMagicInf *lpMagic);
	int SkillIceBeast(LPGameObject &lpObj, int aTargetIndex, CMagicInf *lpMagic);
	int SkillMeteorStorm (LPGameObject &lpObj, CMagicInf* lpMagic, BYTE x, BYTE y, int aTargetIndex);
	void SkillSoulSeker(LPGameObject lpObj, CMagicInf * lpMagic, int aTargetIndex);
	void SkillDarkBlast(LPGameObject lpObj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillArchangelWillBuff(LPGameObject &lpObj, CMagicInf * lpMagic);

	SkillConfig	m_SkillData;
	MULua m_Lua;
};

int  DecPartyMemberHPandMP(LPGameObject &lpObj);

extern CObjUseSkill gObjUseSkill;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

