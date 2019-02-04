////////////////////////////////////////////////////////////////////////////////
// BattleGround.h
// ------------------------------
// Decompiled by Deathway
// Date : 2007-07-09
// ------------------------------
#ifndef BATTLEGROUND_H
#define BATTLEGROUND_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CBattleGround
{

public:

	CBattleGround();
	virtual ~CBattleGround();

	void  SetGroundRect(int map, int x, int y, int xl, int yl);
	void  SetBattleType(int type);
	void  Enable(BOOL enable);
	int   GetUsed();
	void  SetTeamMaster(int n, LPSTR szId);
	void  SetTeamName(int n, LPSTR szName);
	LPSTR  GetTeamName(int n);
	void  SetTimer(int g_time);
	int   GetTimer();
	int   GetScore(int team);
	void  SetScore(int team,  int score);

public:

  char m_szTeamName1[9];	// 4
  char m_szTeamName2[9];	// D
  int m_Score1;	// 18
  int m_Score2;	// 1C
  char m_szID1[11];	// 20
  char m_szID2[11];	// 2B
  RECT m_rect;	// 38
  BOOL m_Set;	// 48
  BOOL m_Type;	// 4C
  int m_Used;	// 50
  int m_MapNumber;	// 54
  int m_nTimer;	// 58
  ULONGLONG m_i64CurTimer;	// 5C

};


#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

