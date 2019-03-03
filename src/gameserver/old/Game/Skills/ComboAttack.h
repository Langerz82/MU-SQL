////////////////////////////////////////////////////////////////////////////////
// ComboAttack.h
// ------------------------------
// Decompiled by Deathway
// Date : 2007-05-09
// ------------------------------
#ifndef MU_COMBOATTACK_H
#define MU_COMBOATTACK_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "custTypedef.h"
#include "generalStructs.h"

struct ComboSkillData
{

public:

	void Init()	// line : 19
	{
		this->dwTime = 0;
		this->Skill[0] = -1;
		this->Skill[1] = -1;
		this->Skill[2] = -1;
		this->ProgressIndex = -1;
	};	// line : 23

	DWORD dwTime;	// 0
	WORD Skill[3];	// 4
	int ProgressIndex;	// 8

};

class CComboAttack
{

public:

	CComboAttack();
	virtual ~CComboAttack();

private:

	int GetSkillPos(int skillnum);

public:

	BOOL CheckCombo(CGameObject &Obj, int skillnum);

};

extern CComboAttack gComboAttack;

#endif

