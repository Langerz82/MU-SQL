////////////////////////////////////////////////////////////////////////////////
// MonsterAttr.h
#ifndef MONSTERATTR_H
#define MONSTERATTR_H

#include "user.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct MONSTER_ATTRIBUTE
{
	int m_Index;	// 0
	int m_Rate;	// 4
	char m_Name[50];	// 8
	int m_Level;	// 1C
	int m_iAINumber;	// 20
	int m_iScriptHP;	// 24
	int m_Hp;	// 28
	int m_Mp;	// 2C
	int m_DamageMin;	// 30
	int m_DamageMax;	// 34
	int m_Defense;	// 38
	int m_MagicDefense;	// 3C
	int m_AttackRating;	// 40
	int m_Successfulblocking;	// 44
	int m_MoveRange;	// 48
	int m_AttackRange;	// 4C
	int m_AttackType;	// 50
	int m_ViewRange;	// 54
	int m_MoveSpeed;	// 58
	int m_AttackSpeed;	// 5C
	int m_RegenTime;	// 60
	int m_Attribute;	// 64
	int m_ItemRate;	// 68
	int m_MoneyRate;	// 6C
	int m_MaxItemLevel;	// 70
	BYTE m_Resistance[MAX_RESISTENCE_TYPE];	// 74
	DWORD m_MonsterSkill;	// 7C
	int m_PentagramMainAttribute;
	int m_PentagramAttributePattern;
	int m_PentagramDefense;
	int m_PentagramAttackMin;
	int m_PentagramAttackMax;
	int m_PentagramAttackRating;
	int m_PentagramDefenseRating;
	BYTE m_btExpType;

} MONSTER_ATTRIBUTE, *LPMONSTER_ATTRIBUTE;

class CMonsterAttr
{

public:

	LPMONSTER_ATTRIBUTE GetAttr(int aClass);
	void LoadAttr(char* filename);

	CMonsterAttr();
	virtual ~CMonsterAttr();

private:

	MONSTER_ATTRIBUTE m_MonsterAttr[MAX_MONSTER_TYPE];	// 4
};

#define FILE_CUSTOM_MONSTERSTATINFO		"IGC_AddMonsterStats.xml"

struct CUSTOM_MONSTER_ATTR
{
	float HPMul;
	float DmgMul;
	float DefMul;
	float ARateMul;
	float DRateMul;
};

class CCustomMonsterAttr
{
public:
	CCustomMonsterAttr();
	virtual ~CCustomMonsterAttr();

	void Load(char* szFileName);

public:
	CUSTOM_MONSTER_ATTR m_DefaultData;
	std::map<int, CUSTOM_MONSTER_ATTR*> m_MonsterData;
}; extern CCustomMonsterAttr gCMAttr;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

