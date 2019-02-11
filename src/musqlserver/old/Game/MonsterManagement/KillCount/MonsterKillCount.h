////////////////////////////////////////////////////////////////////////////////
// MonsterKillCount.h
#ifndef MONSTER_KILL_COUNT_H
#define MONSTER_KILL_COUNT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"

class CKillCountMng
{
public:
	CKillCountMng();
	~CKillCountMng();

	void ReadConfig(LPSTR szFile);

	void CheckMonsterKillCount(int monsterid, CGameObject lpObj);

private:
	int i_MonsterIds[MAX_MONSTER_TYPE];
	
};
extern CKillCountMng g_KillCountMng;
#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

