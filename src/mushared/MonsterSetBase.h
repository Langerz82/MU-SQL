////////////////////////////////////////////////////////////////////////////////
// MonsterSetBase.h
#ifndef MONSTERSETBASE_H
#define MONSTERSETBASE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "user.h"

enum eArrangeType
{
	Arrange_NpcSpawn = 0,
	Arrange_MultiMonsterSpawn = 1,
	Arrange_SingleMonsterSpawn = 2,
	Arrange_ElementalMonsterMultiSpawn = 3
};

typedef struct MONSTER_POSITION
{
	BYTE m_ArrangeType;	// 0
	WORD m_Type;	// 2
	BYTE m_MapNumber;	// 4
	BYTE m_Dis;	// 5
	BYTE m_X;	// 6
	BYTE m_Y;	// 7
	BYTE m_Dir;	// 8
	BYTE m_W;	// 9
	BYTE m_H;	// A
	DWORD m_PentagramMainAttribute;

} MONSTER_POSITION, * LPMONSTER_POSITION;
class CMonsterSetBase
{

public:

	void Init();
	void Delete();

	void LoadSetBase(char* filename);

	CMonsterSetBase();
	virtual ~CMonsterSetBase();

	int GetPosition(int TableNum, short MapNumber, short& x, short& y);
	int GetBoxPosition(int mapnumber, int ax, int ay, int aw, int ah, short& mx, short& my);
	void SetBoxPosition(int TableNum, int mapnumber, int ax, int ay, int aw, int ah);
	void GetPentagramMainAttribute(int TableNum, int* iPentagramMainAtrribute);

public:

  MONSTER_POSITION * m_Mp;	// 4
  int m_Count;	// 10FE4

};

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

