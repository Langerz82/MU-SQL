////////////////////////////////////////////////////////////////////////////////
// MonsterAttr.h
#ifndef _MU_MONSTERATTR_H
#define _MU_MONSTERATTR_H

#include "StdAfx.h"
#include "User/CUserData.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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

extern CMonsterAttr gMAttr;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

