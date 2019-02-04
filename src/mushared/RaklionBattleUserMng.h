// RaklionBattleUserMng.h: interface for the CRaklionBattleUserMng class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAKLIONBATTLEUSERMNG_H__5E0EED22_E99F_4CF2_8B03_D310005F8D11__INCLUDED_)
#define AFX_RAKLIONBATTLEUSERMNG_H__5E0EED22_E99F_4CF2_8B03_D310005F8D11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
using namespace std;

class CRaklionBattleUserMng  
{
public:
	std::vector<int> m_BattleUser;

public:
	CRaklionBattleUserMng();
	virtual ~CRaklionBattleUserMng();
	void ResetAllData();
	int AddUserData(int iIndex);
	int DeleteUserData(int iIndex);
	void CheckUserState();
	int MoveAllUser(int iGateNumber);
	int GetUserCount();
	int IsEmpty();
	int IsBattleUser(int iIndex);
	int GetUserObjIndex(int iBattleUserNumber);
	void LogBattleWinnerUserInfo(BYTE btFlag, int iElapsedTime);

};

extern CRaklionBattleUserMng	g_RaklionBattleUserMng;

#endif // !defined(AFX_RAKLIONBATTLEUSERMNG_H__5E0EED22_E99F_4CF2_8B03_D310005F8D11__INCLUDED_)


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

