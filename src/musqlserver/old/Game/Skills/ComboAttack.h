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

#include "MuDefines.h"
#include "generalStructs.h"

class CComboAttack
{

public:

	CComboAttack();
	virtual ~CComboAttack();

private:

	int GetSkillPos(int skillnum);

public:

	BOOL CheckCombo(LPGameObject &lpObj, int skillnum);

};

extern CComboAttack gComboAttack;

#endif

