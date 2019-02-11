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

	int SpeedHackCheck(CGameObject &lpObj);
	int EnableSkill(WORD Skill);
	int GetUseMana(CGameObject &lpObj, CMagicInf* lpMagic);
	int GetAddUseMana(CGameObject &lpObj, CMagicInf* lpMagic);
	int GetUseBP(CGameObject &lpObj, CMagicInf* lpMagic);
	void UseSkill(CGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	void UseSkill(CGameObject &lpObj, CMagicInf* lpMagic, BYTE x, BYTE y, BYTE dir, BYTE TargetPos, int aTargetIndex);
	int RunningSkill(CGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic, BOOL bCombo);
	void MaGumSkillDefenseDown(CGameObject &lpObj, int aTargetIndex, int skill_level);
	void KnightSkillAddLife(CGameObject &lpObj, CMagicInf * lpMagic);
	void WizardMagicDefense(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic);
	void SkillDefense(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic);
	void SkillAttack(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic);
	int SkillChangeUse(CGameObject &lpObj);
	void SkillHealing(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic);
	int SkillMonsterCall(CGameObject &lpObj, int MonsterType, int x, int y);
	int SkillDeathPoison(CGameObject &lpObj, CMagicInf* lpMagic, BYTE x, BYTE y, BYTE dir, BYTE TargetPos, int aTargetIndex);
	int SkillDeathPoisonForMedusa(CGameObject &lpObj, CMagicInf* lpMagic, BYTE x, BYTE y, int aTargetIndex);
	int SkillSuddenIce(CGameObject &lpObj, CMagicInf* lpMagic, BYTE x, BYTE y, BYTE dir, BYTE TargetPos, int aTargetIndex);
	int SkillHellFire2Start(CGameObject &lpObj, CMagicInf* lpMagic);
	int SkillHellFire2(CGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	void SkillBlowOfFury(CGameObject &lpObj, CMagicInf* lpMagic, BYTE x, BYTE y, BYTE dir, BYTE TargetPos, int aTargetIndex, BOOL isCombo);
	void SkillWheel(CGameObject &lpObj, CMagicInf* lpMagic, int aTargetIndex, BOOL isCombo);
	int SkillKnightBlow(CGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic, BOOL isCombo);
	void SkillPowerSlash(CGameObject &lpObj, CMagicInf* lpMagic, BYTE x, BYTE y, BYTE dir, BYTE Targetangle, int aTargetIndex);
	void SkillFrustrum(CGameObject &lpObj, float fangle, float tx, float ty);
	int GetTargetLinePath(int sx, int sy, int tx, int ty,  int* PathX,int* PathY, int distance);
	int GetAngle(int x, int y, int tx,  int ty);
	int CalDistance(int x, int y, int x1, int y1);
	void SkillFrustrum2(CGameObject &lpObj, float fangle, float tx, float ty);
	void SkillFrustrum3(CGameObject &lpObj, float fangle, float tx, float ty, float f1, float f2);
	int SkillAreaCheck(int angel, int x, int y,  int tx, int ty,  int dis,  int arc);
	int SkillSpear(CGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillFireBurst(CGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillDarkHorseAttack(CGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int RecallPartyCheck(CGameObject &lpObj, int skill_level);
	void SkillRecallParty(CGameObject &lpObj, int skill_level);
	void SkillAddCriticalDamage(CGameObject &lpObj, int skill_level, CMagicInf* lpMagic);
	void SkillElectricSpark(CGameObject &lpObj, CMagicInf* lpMagic, BYTE x, BYTE y, BYTE dir, BYTE TargetPos, int aTargetIndex);
	int SkillSummon(CGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillImmuneToMagic(CGameObject &lpObj, CMagicInf* lpMagic);
	int SkillImmuneToHarm(CGameObject &lpObj, CMagicInf* lpMagic);
	void SkillKnightRush(CGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillWizardJavalin(CGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillElfRush(CGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillMagumOneFlash(CGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillMagumDeathCannon(CGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillDarklordSpaceSplit(CGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillBrandOfSkill(CGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillRemoveStun(CGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillAddMana(CGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillCloaking(CGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillRemoveCloaking(CGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillRemoveMagic(CGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int RemoveCloakingEffect(CGameObject &lpObj);
	int SkillStun(CGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic, BYTE btX, BYTE btY, BYTE btDir, BYTE btTargetPos);
	int RemoveAllCharacterInvalidMagicAndSkillState(CGameObject &lpObj);
	int RemoveSpecificSkillState(CGameObject &lpObj, int iSkillNumber);
	int SkillFenrirAttack(CGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillInfinityArrow(CGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int SkillFireScream(CGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic);
	int FireScreamExplosionAttack(CGameObject &lpObj, CGameObject lpTargetObj, int iAttackDamage, int iAddDamage);
	void SpecificSkillAdditionTreat(CGameObject &lpObj, CGameObject lpTargetObj, CMagicInf* lpMagic, int iAttackDamage);
	int SkillBookSahamutt(CGameObject &lpObj, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex);
	int SkillSummonerSleep(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic);
	int SkillDrainLife(CGameObject &lpObj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillDamageReflect(CGameObject &lpObj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillBookNeil(CGameObject &lpObj, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex);
	int SkillInnovation(CGameObject &lpObj, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex);
	int SkillWeakness(CGameObject &lpObj, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex);
	int SkillChainLighting(CGameObject &lpObj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillBlowOfDestruction(CGameObject &lpObj, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex, BOOL bCombo);
	int SkillAreaMonsterAttack(CGameObject &lpObj, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex, int Distance, int MultiAttack, int AttackDelay);
	void SkillLightingShock(CGameObject &lpObj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillMagicCircle(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic);
	int SkillShieldRefill(CGameObject &lpObj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillBerserker(CGameObject &lpObj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillGiganticStorm(CGameObject &lpObj, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex);
	int SkillFiveShot(CGameObject &lpObj, CMagicInf * lpMagic, BYTE TargetPos, int aTargetIndex);
	int SkillBirds(CGameObject &lpObj, CMagicInf * lpMagic, BYTE TargetPos, int aTargetIndex);
	int SkillFlameStrike(CGameObject &lpObj, CMagicInf * lpMagic, BYTE TargetPos, int aTargetIndex);
	void NewSkillProc(CGameObject &lpObj, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex, int aMsgSubCode, BOOL bCombo);
	int SkillMonkBuff(CGameObject &lpObj, CMagicInf * lpMagic);
	int SkillMonkBuffApplyParty(CGameObject &lpObj, CMagicInf * lpMagic);
	int SkillMonkBarrageJustOneTarget(CGameObject &lpObj, CMagicInf *lpMagic, int aTargetIndex);
	int SkillCharge(CGameObject &lpObj, CMagicInf *lpMagic, int aTargetIndex);
	int SkillMonkDarkSideGetTargetIndex(CGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic, WORD *Target);
	void SkillDarkSide(int aindex, int aTargetIndex, CMagicInf * lpMagic);
	int SkillPhoenixShot(CGameObject &lpObj, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex);
	int SkillEvilMonster(CGameObject &lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void SplashDamage(CGameObject &lpObj, CGameObject lpTargetObj, int nSkill, int nAttackDamage, int nDistance, int nDamageRate);
	void SkillSummonForLordSilvester(CGameObject &lpObj, int iTargetIndex, CMagicInf *lpMagic);
	void SkillSpinStep(CGameObject &lpObj, int iTargetIndex, CMagicInf *lpMagic);
	void SkillGrowLancerAttackOneTarget(CGameObject &lpObj, int aTargetIndex, CMagicInf *lpMagic);
	int SkillBreche(CGameObject &lpObj, CMagicInf* lpMagic, BYTE x, BYTE y, int aTargetIndex);
	int SkillShiningPeak(CGameObject &lpObj, CMagicInf *lpMagic, int aTargetIndex);
	int SkillBuffGrowLancer(CGameObject &lpObj, CMagicInf *lpMagic, int aTargetIndex);
	int SkillBuffPartyGrowLancer(CGameObject &lpObj, CMagicInf *lpMagic, int aTargetIndex);
	
	void SkillWindSoul(CGameObject lpObj, CMagicInf * lpMagic, int aTargetIndex);
	void SkillFireBlood(CGameObject lpObj, CMagicInf * lpMagic, int aTargetIndex);
	void SkillIceBlood(CGameObject lpObj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillFireBeast(CGameObject &lpObj, int aTargetIndex, CMagicInf *lpMagic);
	int SkillIceBeast(CGameObject &lpObj, int aTargetIndex, CMagicInf *lpMagic);
	int SkillMeteorStorm (CGameObject &lpObj, CMagicInf* lpMagic, BYTE x, BYTE y, int aTargetIndex);
	void SkillSoulSeker(CGameObject lpObj, CMagicInf * lpMagic, int aTargetIndex);
	void SkillDarkBlast(CGameObject lpObj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillArchangelWillBuff(CGameObject &lpObj, CMagicInf * lpMagic);

	SkillConfig	m_SkillData;
	MULua m_Lua;
};

int  DecPartyMemberHPandMP(CGameObject &lpObj);

extern CObjUseSkill gObjUseSkill;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

