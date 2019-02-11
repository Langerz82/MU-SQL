////////////////////////////////////////////////////////////////////////////////
// MasterLevelSkillTreeSystem.h
#ifndef MASTERLEVELSKILLTREESYSTEM_H
#define MASTERLEVELSKILLTREESYSTEM_H

#include "StdAfx.h"
#include "User/CUserData.h"
#include "classdef.h"
#include "MasterSkillSystem.h"
#include "ObjAttack.h"

#define MAX_SKILL_POS 4
#define MAX_TREE_TYPE 3
#define MAX_MASTER_SKILL_TYPE 450
#define MAX_MASTER_SKILL_RANK 9

struct MLST_element
{
	int		m_dwIndex;
	BYTE	m_byConsumePoint;
	BYTE	m_byMaxPoint;
	DWORD	m_dwParentSkill1;
	DWORD	m_dwParentSkill2;
	DWORD	m_dwSkillID;
};

struct MLSTree
{
	MLST_element		iElement[MAX_MASTER_SKILL_RANK][MAX_SKILL_POS];
};

struct MLST_table
{
	BYTE		btClass;
	BYTE		btType;
	BYTE		btWidth;
	MLSTree		iAbility[MAX_TREE_TYPE];
};

enum MLST_Value_Types
{
	MLST_VALUE_NORMAL = 0,
	MLST_VALUE_DAMAGE = 1,
	MLST_VALUE_MANA_INC = 2
};

struct MLST_Value_Table
{
	MLST_Value_Table();
	~MLST_Value_Table();

	void Init(int iMaxSkillLevel)
	{
		if (this->fValueTable != NULL)
		{
			delete[] this->fValueTable;
			this->fValueTable = NULL;
		}

		this->fValueTable = new float[iMaxSkillLevel + 1];
		this->iValueType = MLST_VALUE_NORMAL;

		for (int i = 0; i <= iMaxSkillLevel; i++)
		{
			this->fValueTable[i] = 0.0;
		}
	}

	int iValueType;
	float * fValueTable;
};

static const int CHARACTER_CLASS_TABLE[MAX_TYPE_PLAYER][2] = {
	2,		CLASS_WIZARD,
	1,		CLASS_KNIGHT,
	4,		CLASS_ELF,
	16,		CLASS_MAGUMSA,
	8,		CLASS_SUMMONER,
	32,		CLASS_DARKLORD,
	64,		CLASS_RAGEFIGHTER,
	128,	CLASS_GROWLANCER
};

struct PMSG_MASTERLEVELSKILLTREE_COUNT
{
	PWMSG_HEAD2	h;
	int count;				// 8
};

struct PMSG_MASTERLEVELSKILLTREE
{
	BYTE btMasterSkillUIIndex;
	BYTE btMasterSkillLevel;
	float fMasterSkillCurValue;
	float fMasterSkillNextValue;
	BYTE btUnk;
};

struct PMSG_ANS_MASTERLEVELSKILLTREE
{
	PBMSG_HEAD2	h;
	BYTE btResult;			// 4
	WORD wMasterLevelPoint;		// 6
	BYTE btMasterSkillUIIndex;	// 8
	int dwMasterSkillIndex;		// C
	int dwMasterSkillLevel;			// 10
	float fMasterSkillCurValue;			// 14
	float fMasterSkillNextValue;		// 18
};

#pragma once
class CMasterLevelSkillTreeSystem : public CMasterLevelSystem
{
public:
	CMasterLevelSkillTreeSystem(void);
	virtual ~CMasterLevelSkillTreeSystem(void);
	friend CObjAttack;
	friend void gObjCharZeroSet(int aIndex);

	void Init();
	void AddToValueTable(int iSkillValueID, const char * szFunctionName, int iSkillValueType);
	void Load(const char *lpszFileName);

	void InitPassiveSkillData(OBJECTSTRUCT* lpObj);
	void CalcPassiveSkillData(OBJECTSTRUCT* lpObj, int iMLSkill, int iSkillLevel);
	int CheckSkillCondition(LPGameObject lpObj, int iMLSkill, int iSkillLevel);
	int CheckCompleteBrandOfSkill(LPGameObject lpObj, int iMLSkill);
	int CheckMasterLevelSkill(int iMLSkill);
	int IsExistMLSBasedSkill(LPGameObject lpObj, int iSkill);
	int GetBaseMasterLevelSkill(int iMLSkill);
	int gObjMagicAdd_MLS(LPGameObject lpObj, int iMLSkill, int iSkillLevel);
	void CGReqGetMasterLevelSkillTree(int aIndex);
	void CGReqGetMasterLevelSkill(PMSG_REQ_MASTERLEVEL_SKILL *lpMsg, int aIndex);
	int RunningSkill_MLS(LPGameObject lpObj, int aTargetIndex, CMagicInf *lpMagic, BOOL bCombo, BYTE x, BYTE y, BYTE aTargetPos);
	int CheckUsableWeaponSkill(LPGameObject lpObj, int nCount);
	int GetRequireMLPoint(int iClass, int iSkill);
	int GetMaxPointOfMasterSkill(int iClass, int iSkill);
	float GetMasterSkillValue(int iType, int iPoint);
	float GetMasterSkillManaIncRate(int iPoint);
	float GetBrandOfMasterSkillValue(LPGameObject lpObj, int iSkill, int iStep);
	int CheckPreviousRankSkill(LPGameObject lpObj, int iSkill, int iLevel);
	int CheckSkillLevel(LPGameObject lpObj, int iSkill);
	int GetMasterSkillUIIndex(int iClass, int iSkill);
	int GetParentSkill1(int iClass, int iSkill);
	int GetParentSkill2(int iClass, int iSkill);
	void SetItemMLPassiveSkill(LPGameObject lpObj, int iType);
	void SetPetItemMLPassiveSkill(LPGameObject lpObj, int iType);
	void SetWingMLPassiveSkill(LPGameObject lpObj, int iType);
	int GetCurSkillLevel(LPGameObject lpObj, int iSkill);
	int GetScriptClassCode(int iClass);
	float RoundValue(long double dbValue, int iPos);
	BYTE ResetMasterSkill(LPGameObject &lpObj, int nTreeType);
	BYTE CheckMasterSkillPoint(LPGameObject &lpObj, int nTreeType);
	BYTE CheckSkillInTree(int iClass, int iSkill, int iTreeKind);
	float GetSkillAttackDamage(LPGameObject lpObj, int iSkill);

	// Skills
	void MLS_WizardMagicDefense(LPGameObject lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_Healing(LPGameObject lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_Wheel(LPGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex, BOOL isCombo);
	void MLS_SkillSword4(LPGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex, BOOL isCombo);
	void MLS_SkillSword4_Magumsa(LPGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex, BOOL isCombo);
	void MLS_SkillSword5(LPGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex, BOOL isCombo);
	void MLS_SkillSword1(LPGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex, BOOL isCombo);
	void MLS_SkillSword2(LPGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex, BOOL isCombo);
	void MLS_SkillStrengthenDesruction(LPGameObject lpObj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex, int isCombo);
	void MLS_KnightSkillAddLife(LPGameObject lpObj, CMagicInf *lpMagic);
	void MLS_SkillFulKnightSkillAddLife(LPGameObject lpObj, CMagicInf *lpMagic);
	void MLS_MasteryKnightSkillAddLife(LPGameObject lpObj, CMagicInf *lpMagic);
	int MLS_SkillStrengthenMagicPower(LPGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex);
	void MLS_SkillSkillfulMagicPower(LPGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex);
	void MLS_SkillStrengthenFlame(LPGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex);
	void MLS_SkillStrengthenThunder(LPGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex);
	void MLS_SkillStrengthenInferno(LPGameObject lpObj, CMagicInf *lpMagic);
	void MLS_SkillStrengthenHellFire(LPGameObject lpObj, CMagicInf *lpMagic);
	void MLS_SkillStrengthenPoison(LPGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex);
	void MLS_SkillStrengthenExPoison(LPGameObject &lpObj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex);
	void MLS_SkillStrengthenIce(LPGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex);
	void MLS_SkillWizardMagicDefense(LPGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex);
	void MLS_SkillStrengthenPenetration(LPGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex);
	void MLS_SkillInfinityArrow(LPGameObject &lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillStrengthenBerserker(LPGameObject &lpObj, CMagicInf *lpMagic);
	void MLS_SkillSkillfulBerserker(LPGameObject &lpObj, CMagicInf *lpMagic);
	void MLS_SkillMasteryBerserker(LPGameObject &lpObj, CMagicInf *lpMagic);
	void MLS_SkillStrengthenBlast(LPGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex);
	void MLS_SkillStrendthenAddCriticalDamage(LPGameObject &lpObj, CMagicInf *lpMagic);
	void MLS_SkillSkillFulAddCriticalDamage(LPGameObject &lpObj, CMagicInf *lpMagic);
	void MLS_SkillMasteryAddCriticalDamage(LPGameObject &lpObj, CMagicInf *lpMagic);
	void MLS_DarkHorseAttack(LPGameObject lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_FireBurst(LPGameObject lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillStrengthenSpear(LPGameObject &lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillAreaMonsterAttack(LPGameObject &lpObj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex, int aDistance, BOOL bPVP, int nSuccessRate, int nDelay);
	void MLS_SuddenIce(LPGameObject &lpObj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex);
	int MLS_CalDistance(int x, int y, int x1, int y1);
	int MLS_GetAngle(int x, int y, int tx, int ty);
	void MLS_SkillFrustrum(LPGameObject &lpObj, float fangle, float tx, float ty);
	void MLS_KnightBlow(LPGameObject &lpObj, int aTargetIndex, CMagicInf *lpMagic, BOOL isCombo);
	void MLS_SkillBlowOfFury(LPGameObject &lpObj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex, BOOL isCombo);
	void MLS_SkillDefense(LPGameObject &lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillAttack(LPGameObject &lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillStrengthenDefense(LPGameObject &lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillStrenghtenAttack(LPGameObject &lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_MaGumSkillDefenseDown(LPGameObject &lpObj, int aTargetIndex);
	void MLS_PowerSlash(LPGameObject &lpObj, CMagicInf *lpMagic, BYTE x, BYTE y, BYTE Targetangle, int aTargetIndex);
	void MLS_FireScream(LPGameObject &lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillMonkBarrageJustOneTarget(LPGameObject &lpObj, CMagicInf *lpMagic, int aTargetIndex);
	void MLS_SkillStrengthenDragonRoar(LPGameObject &lpObj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex, int aDistance, BOOL bPVP, int nDelay);
	void MLS_SkillStrengthenBlockRate(LPGameObject &lpObj, CMagicInf *lpMagic);
	void MLS_SkillMasteryBlockRate(LPGameObject &lpObj, CMagicInf *lpMagic);
	void MLS_SkillStrengthenIncVital(LPGameObject &lpObj, CMagicInf *lpMagic);
	void MLS_SkillStrengthenIgnoreEnemyDefense(LPGameObject &lpObj, CMagicInf *lpMagic);
	void MLS_SkillBloodStorm(LPGameObject lpObj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex, BOOL isCombo);
	void MLS_SkillEarthPrison(LPGameObject lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillPoisonArrow(LPGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex, BOOL isCombo);
	void MLS_SkillCure(LPGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex);
	void MLS_SkillMonsterCall(LPGameObject &lpObj, int MonsterType, int x, int y);
	void MLS_SkillPartyHealing(LPGameObject &lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillBless(LPGameObject lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillStrengthenBless(LPGameObject lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillBlind(LPGameObject lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void SkillIronDefenseDK(LPGameObject lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void SkillIronDefenseDL(LPGameObject lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillBloodHowling(LPGameObject lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillStrengthenBloodHowling(LPGameObject lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillfulKnightBlow(LPGameObject &lpObj, int aTargetIndex, CMagicInf *lpMagic, BOOL isCombo);
	void MLS_MasteryKnightBlow(LPGameObject &lpObj, int aTargetIndex, CMagicInf *lpMagic, BOOL isCombo);
	void MLS_SkillSkillfulDesruction(LPGameObject lpObj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex, BOOL isCombo);
	void MLS_SkillMasteryDesruction(LPGameObject lpObj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex, BOOL isCombo);
	void MLS_MasterySuddenIce(LPGameObject &lpObj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex);
	void MLS_SkillStrengthenMultiShot(LPGameObject &lpObj, CMagicInf *lpMagic, BYTE bAngle, int aTargetIndex);
	void MLS_SkillHellBust(LPGameObject &lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillHellBustStart(LPGameObject &lpObj, CMagicInf *lpMagic);
	void MLS_SkillMasteryWizardMagicDefense(LPGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex);
	void MLS_SkillStrengthenWeakness(LPGameObject &lpObj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex);
	void MLS_SkillStrengthenEnervation(LPGameObject &lpObj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex);
	void MLS_SkillMasteryIce(LPGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex);
	void MLS_SkillStrengthenFlameStrike(LPGameObject &lpObj, CMagicInf *lpMagic, BYTE bAngle, int aTargetIndex);
	void MLS_SkillGiganticStorm(LPGameObject &lpObj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex);
	void MLS_SkillMasteryDefenseDown(LPGameObject &lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillMasteryAddCriticalDamage2(LPGameObject &lpObj, CMagicInf *lpMagic);
	void MLS_SkillFrustrum3(LPGameObject &lpObj, BYTE bAngle, float tx1, float ty1, float tx2, float ty2);
	void MLS_SkillStrengthenElectricSpark(LPGameObject &lpObj, CMagicInf *lpMagic, BYTE x, BYTE y, BYTE TargetPos, int aTargetIndex);
	void MLS_SkillStrengthenchaoticDesair(LPGameObject &lpObj, CMagicInf *lpMagic, BYTE bAngle, int aTargetIndex);
	bool MLS_WeaponSkillDel(LPGameObject lpObj, WORD aSkill, BYTE Level);
	bool MLS_WeaponSkillAdd(LPGameObject lpObj, WORD aSkill, BYTE Level);
	void MLS_SkillSleep(LPGameObject &lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillChainLightning(LPGameObject &lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillLightningShock(LPGameObject &lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillDrainLife(LPGameObject &lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void SkillCircleShieldGrowLancer(LPGameObject &lpObj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillWrathPowUpGrowLancer(LPGameObject &lpObj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillWrathProficiencyGrowLancer(LPGameObject &lpObj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillWrathMasteryGrowLancer(LPGameObject &lpObj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillObsidianPowUpGrowLancer(LPGameObject &lpObj, CMagicInf * lpMagic, int aTargetIndex);
	void SkillBurstMasteryGrowLancer(LPGameObject &lpObj, CMagicInf * lpMagic, int aTargetIndex);
	// FINISHED

	int GetMaxMasterSkillLevel() { return this->m_wMaxMasterSkillLevel; }

	MULua				m_Lua;
private:

	MLST_table			m_MLSTable[MAX_TYPE_PLAYER];
	MLST_Value_Table	* m_MLSValueTable; //[MAX_MASTER_SKILL_TYPE];
	WORD				m_wMaxMasterSkillLevel; // private custom
};

extern CMasterLevelSkillTreeSystem g_MasterLevelSkillTreeSystem;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

