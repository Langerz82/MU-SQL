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
	friend void gObjCharZeroSet(CGameObject &Obj);

	void Init();
	void AddToValueTable(int iSkillValueID, const char * szFunctionName, int iSkillValueType);
	void Load(const char *lpszFileName);

	void InitPassiveSkillData(CGameObject* lpObj);
	void CalcPassiveSkillData(CGameObject* lpObj, int iMLSkill, int iSkillLevel);
	int CheckSkillCondition(CGameObject lpObj, int iMLSkill, int iSkillLevel);
	int CheckCompleteBrandOfSkill(CGameObject lpObj, int iMLSkill);
	int CheckMasterLevelSkill(int iMLSkill);
	int IsExistMLSBasedSkill(CGameObject lpObj, int iSkill);
	int GetBaseMasterLevelSkill(int iMLSkill);
	int gObjMagicAdd_MLS(CGameObject lpObj, int iMLSkill, int iSkillLevel);
	void CGReqGetMasterLevelSkillTree(CGameObject &Obj);
	void CGReqGetMasterLevelSkill(PMSG_REQ_MASTERLEVEL_SKILL *lpMsg, CGameObject &Obj);
	int RunningSkill_MLS(CGameObject lpObj, int aTargetIndex, CMagicInf *lpMagic, BOOL bCombo, BYTE x, BYTE y, BYTE aTargetPos);
	int CheckUsableWeaponSkill(CGameObject lpObj, int nCount);
	int GetRequireMLPoint(int iClass, int iSkill);
	int GetMaxPointOfMasterSkill(int iClass, int iSkill);
	float GetMasterSkillValue(int iType, int iPoint);
	float GetMasterSkillManaIncRate(int iPoint);
	float GetBrandOfMasterSkillValue(CGameObject lpObj, int iSkill, int iStep);
	int CheckPreviousRankSkill(CGameObject lpObj, int iSkill, int iLevel);
	int CheckSkillLevel(CGameObject lpObj, int iSkill);
	int GetMasterSkillUIIndex(int iClass, int iSkill);
	int GetParentSkill1(int iClass, int iSkill);
	int GetParentSkill2(int iClass, int iSkill);
	void SetItemMLPassiveSkill(CGameObject lpObj, int iType);
	void SetPetItemMLPassiveSkill(CGameObject lpObj, int iType);
	void SetWingMLPassiveSkill(CGameObject lpObj, int iType);
	int GetCurSkillLevel(CGameObject lpObj, int iSkill);
	int GetScriptClassCode(int iClass);
	float RoundValue(long double dbValue, int iPos);
	BYTE ResetMasterSkill(CGameObject &Obj, int nTreeType);
	BYTE CheckMasterSkillPoint(CGameObject &Obj, int nTreeType);
	BYTE CheckSkillInTree(int iClass, int iSkill, int iTreeKind);
	float GetSkillAttackDamage(CGameObject lpObj, int iSkill);

	// Skills
	void MLS_WizardMagicDefense(CGameObject lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_Healing(CGameObject lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_Wheel(CGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex, BOOL isCombo);
	void MLS_SkillSword4(CGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex, BOOL isCombo);
	void MLS_SkillSword4_Magumsa(CGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex, BOOL isCombo);
	void MLS_SkillSword5(CGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex, BOOL isCombo);
	void MLS_SkillSword1(CGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex, BOOL isCombo);
	void MLS_SkillSword2(CGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex, BOOL isCombo);
	void MLS_SkillStrengthenDesruction(CGameObject lpObj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex, int isCombo);
	void MLS_KnightSkillAddLife(CGameObject lpObj, CMagicInf *lpMagic);
	void MLS_SkillFulKnightSkillAddLife(CGameObject lpObj, CMagicInf *lpMagic);
	void MLS_MasteryKnightSkillAddLife(CGameObject lpObj, CMagicInf *lpMagic);
	int MLS_SkillStrengthenMagicPower(CGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex);
	void MLS_SkillSkillfulMagicPower(CGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex);
	void MLS_SkillStrengthenFlame(CGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex);
	void MLS_SkillStrengthenThunder(CGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex);
	void MLS_SkillStrengthenInferno(CGameObject lpObj, CMagicInf *lpMagic);
	void MLS_SkillStrengthenHellFire(CGameObject lpObj, CMagicInf *lpMagic);
	void MLS_SkillStrengthenPoison(CGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex);
	void MLS_SkillStrengthenExPoison(CGameObject &Obj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex);
	void MLS_SkillStrengthenIce(CGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex);
	void MLS_SkillWizardMagicDefense(CGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex);
	void MLS_SkillStrengthenPenetration(CGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex);
	void MLS_SkillInfinityArrow(CGameObject &Obj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillStrengthenBerserker(CGameObject &Obj, CMagicInf *lpMagic);
	void MLS_SkillSkillfulBerserker(CGameObject &Obj, CMagicInf *lpMagic);
	void MLS_SkillMasteryBerserker(CGameObject &Obj, CMagicInf *lpMagic);
	void MLS_SkillStrengthenBlast(CGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex);
	void MLS_SkillStrendthenAddCriticalDamage(CGameObject &Obj, CMagicInf *lpMagic);
	void MLS_SkillSkillFulAddCriticalDamage(CGameObject &Obj, CMagicInf *lpMagic);
	void MLS_SkillMasteryAddCriticalDamage(CGameObject &Obj, CMagicInf *lpMagic);
	void MLS_DarkHorseAttack(CGameObject lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_FireBurst(CGameObject lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillStrengthenSpear(CGameObject &Obj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillAreaMonsterAttack(CGameObject &Obj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex, int aDistance, BOOL bPVP, int nSuccessRate, int nDelay);
	void MLS_SuddenIce(CGameObject &Obj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex);
	int MLS_CalDistance(int x, int y, int x1, int y1);
	int MLS_GetAngle(int x, int y, int tx, int ty);
	void MLS_SkillFrustrum(CGameObject &Obj, float fangle, float tx, float ty);
	void MLS_KnightBlow(CGameObject &Obj, int aTargetIndex, CMagicInf *lpMagic, BOOL isCombo);
	void MLS_SkillBlowOfFury(CGameObject &Obj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex, BOOL isCombo);
	void MLS_SkillDefense(CGameObject &Obj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillAttack(CGameObject &Obj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillStrengthenDefense(CGameObject &Obj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillStrenghtenAttack(CGameObject &Obj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_MaGumSkillDefenseDown(CGameObject &Obj, int aTargetIndex);
	void MLS_PowerSlash(CGameObject &Obj, CMagicInf *lpMagic, BYTE x, BYTE y, BYTE Targetangle, int aTargetIndex);
	void MLS_FireScream(CGameObject &Obj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillMonkBarrageJustOneTarget(CGameObject &Obj, CMagicInf *lpMagic, int aTargetIndex);
	void MLS_SkillStrengthenDragonRoar(CGameObject &Obj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex, int aDistance, BOOL bPVP, int nDelay);
	void MLS_SkillStrengthenBlockRate(CGameObject &Obj, CMagicInf *lpMagic);
	void MLS_SkillMasteryBlockRate(CGameObject &Obj, CMagicInf *lpMagic);
	void MLS_SkillStrengthenIncVital(CGameObject &Obj, CMagicInf *lpMagic);
	void MLS_SkillStrengthenIgnoreEnemyDefense(CGameObject &Obj, CMagicInf *lpMagic);
	void MLS_SkillBloodStorm(CGameObject lpObj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex, BOOL isCombo);
	void MLS_SkillEarthPrison(CGameObject lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillPoisonArrow(CGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex, BOOL isCombo);
	void MLS_SkillCure(CGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex);
	void MLS_SkillMonsterCall(CGameObject &Obj, int MonsterType, int x, int y);
	void MLS_SkillPartyHealing(CGameObject &Obj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillBless(CGameObject lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillStrengthenBless(CGameObject lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillBlind(CGameObject lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void SkillIronDefenseDK(CGameObject lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void SkillIronDefenseDL(CGameObject lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillBloodHowling(CGameObject lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillStrengthenBloodHowling(CGameObject lpObj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillfulKnightBlow(CGameObject &Obj, int aTargetIndex, CMagicInf *lpMagic, BOOL isCombo);
	void MLS_MasteryKnightBlow(CGameObject &Obj, int aTargetIndex, CMagicInf *lpMagic, BOOL isCombo);
	void MLS_SkillSkillfulDesruction(CGameObject lpObj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex, BOOL isCombo);
	void MLS_SkillMasteryDesruction(CGameObject lpObj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex, BOOL isCombo);
	void MLS_MasterySuddenIce(CGameObject &Obj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex);
	void MLS_SkillStrengthenMultiShot(CGameObject &Obj, CMagicInf *lpMagic, BYTE bAngle, int aTargetIndex);
	void MLS_SkillHellBust(CGameObject &Obj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillHellBustStart(CGameObject &Obj, CMagicInf *lpMagic);
	void MLS_SkillMasteryWizardMagicDefense(CGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex);
	void MLS_SkillStrengthenWeakness(CGameObject &Obj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex);
	void MLS_SkillStrengthenEnervation(CGameObject &Obj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex);
	void MLS_SkillMasteryIce(CGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex);
	void MLS_SkillStrengthenFlameStrike(CGameObject &Obj, CMagicInf *lpMagic, BYTE bAngle, int aTargetIndex);
	void MLS_SkillGiganticStorm(CGameObject &Obj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex);
	void MLS_SkillMasteryDefenseDown(CGameObject &Obj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillMasteryAddCriticalDamage2(CGameObject &Obj, CMagicInf *lpMagic);
	void MLS_SkillFrustrum3(CGameObject &Obj, BYTE bAngle, float tx1, float ty1, float tx2, float ty2);
	void MLS_SkillStrengthenElectricSpark(CGameObject &Obj, CMagicInf *lpMagic, BYTE x, BYTE y, BYTE TargetPos, int aTargetIndex);
	void MLS_SkillStrengthenchaoticDesair(CGameObject &Obj, CMagicInf *lpMagic, BYTE bAngle, int aTargetIndex);
	bool MLS_WeaponSkillDel(CGameObject lpObj, WORD aSkill, BYTE Level);
	bool MLS_WeaponSkillAdd(CGameObject lpObj, WORD aSkill, BYTE Level);
	void MLS_SkillSleep(CGameObject &Obj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillChainLightning(CGameObject &Obj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillLightningShock(CGameObject &Obj, int aTargetIndex, CMagicInf *lpMagic);
	void MLS_SkillDrainLife(CGameObject &Obj, int aTargetIndex, CMagicInf *lpMagic);
	void SkillCircleShieldGrowLancer(CGameObject &Obj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillWrathPowUpGrowLancer(CGameObject &Obj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillWrathProficiencyGrowLancer(CGameObject &Obj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillWrathMasteryGrowLancer(CGameObject &Obj, CMagicInf * lpMagic, int aTargetIndex);
	int SkillObsidianPowUpGrowLancer(CGameObject &Obj, CMagicInf * lpMagic, int aTargetIndex);
	void SkillBurstMasteryGrowLancer(CGameObject &Obj, CMagicInf * lpMagic, int aTargetIndex);
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

