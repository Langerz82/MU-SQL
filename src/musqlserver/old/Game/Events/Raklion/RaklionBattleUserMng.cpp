// RaklionBattleUserMng.cpp: implementation of the CRaklionBattleUserMng class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RaklionBattleUserMng.h"
#include "User/CUserData.h"
#include "Raklion.h"
#include "RaklionUtil.h"
#include "Logging/Log.h"
#include "configread.h"
CRaklionBattleUserMng	g_RaklionBattleUserMng;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRaklionBattleUserMng::CRaklionBattleUserMng()
{
	ResetAllData();
}

CRaklionBattleUserMng::~CRaklionBattleUserMng()
{

}

void CRaklionBattleUserMng::ResetAllData()
{
	while(!m_BattleUser.empty())
	{
		m_BattleUser.pop_back();
	}

	m_BattleUser.clear();
}

BOOL CRaklionBattleUserMng::AddUserData(CGameObject &Obj)
{
	if(gObjIsConnected(iIndex) == FALSE)
	{
		sLog->outBasic("[ RAKLION ][ BattleUser ] Add User Fail - Disconnect User [%s][%s]", Obj.AccountID, Obj.Name);
		return FALSE;
	}

	m_BattleUser.push_back(iIndex);

	g_RaklionUtil.NotifyRaklionCurrentState(iIndex, g_Raklion.GetRaklionState(), g_Raklion.GetRaklionStateDetail());

	sLog->outBasic("[ RAKLION ][ Battle User ] Add User [%s][%s] Current Battle User:%d", Obj.AccountID, Obj.Name, m_BattleUser.size());
	return TRUE;
}

BOOL CRaklionBattleUserMng::DeleteUserData(CGameObject &Obj)
{
	if( iIndex < 0 || iIndex > g_ConfigRead.server.GetObjectMax()-1 )
	{
		sLog->outError( "[ RAKLION ][ BattleUser ] Delete User Fail - Unvalid Index:%d", iIndex);
		return FALSE;
	}

	for(int iCount=0; iCount < m_BattleUser.size(); iCount++)
	{
		if(m_BattleUser[iCount] == iIndex)
		{
			m_BattleUser.erase(m_BattleUser.begin()+iCount);
			sLog->outBasic("[ RAKLION ][ BattleUser ] Delete User - [%s][%s] Current Battle User:%d", Obj.AccountID, Obj.Name, m_BattleUser.size());
			return TRUE;
		}
	}
	return FALSE;
}

void CRaklionBattleUserMng::CheckUserState()
{
	for(int iCount=0; iCount<m_BattleUser.size(); iCount++)
	{
		if( !gObjIsConnected(m_BattleUser[iCount]))
		{
			DeleteUserData(m_BattleUser[iCount]);
			sLog->outBasic("[ RAKLION ][ BattleUser ] Delete User - Disconnect [%s][%s]", gGameObjects[m_BattleUser[iCount]]->AccountID, gGameObjects[m_BattleUser[iCount]]->Name);
		}

		if( gGameObjects[m_BattleUser[iCount]]->MapNumber != MAP_INDEX_HATCHERY &&
			 gGameObjects[m_BattleUser[iCount]]->m_State == 2 &&
			 gGameObjects[m_BattleUser[iCount]]->Live == 1 &&
			 gGameObjects[m_BattleUser[iCount]]->m_SkillInfo.LordSummonMapNumber != MAP_INDEX_HATCHERY )
		{
			DeleteUserData(m_BattleUser[iCount]);
			sLog->outBasic("[ RAKLION ][ BattleUser ] Delete User - Map Move [%s][%s]", gGameObjects[m_BattleUser[iCount]]->AccountID, gGameObjects[m_BattleUser[iCount]]->Name);
		}
	}
}

BOOL CRaklionBattleUserMng::MoveAllUser(int iGateNumber)
{
	BOOL bMoveGateSuccess = FALSE;

	for(int iCount=0; iCount<m_BattleUser.size(); iCount++)
	{
		bMoveGateSuccess = gObjMoveGate(m_BattleUser[iCount], iGateNumber);
		if(bMoveGateSuccess == TRUE)
		{
			sLog->outBasic("[ RAKLION ][ BattleUser ] [%s][%s] MoveGate(%d)", gGameObjects[m_BattleUser[iCount]]->AccountID, gGameObjects[m_BattleUser[iCount]]->Name, iGateNumber);
		}
		else
		{
			DeleteUserData(m_BattleUser[iCount]);
			sLog->outBasic("[ RAKLION ][ BattleUser ] [%s][%s] MoveGate Fail (%d)", gGameObjects[m_BattleUser[iCount]]->AccountID, gGameObjects[m_BattleUser[iCount]]->Name, iGateNumber);
			gObjMoveGate(m_BattleUser[iCount], 286); //Raklion Entrance
		}
	}
	return TRUE;
}

int CRaklionBattleUserMng::GetUserCount()
{
	return m_BattleUser.size();
}

int CRaklionBattleUserMng::IsEmpty()
{
	return (BYTE)m_BattleUser.empty();
}

void CRaklionBattleUserMng::LogBattleWinnerUserInfo(BYTE btFlag, int iElapsedTime)
{
/*	int iIndex = -1;
	float fTime = iElapsedTime / 1000.0f;

	for(int iCount=0; iCount<GetUserCount(); iCount++)
	{
		iIndex = m_BattleUser[iCount];
		LogAddTD("[ RAKLION ][ BATTLE WINNER ] [%d/ElapsedTime:%0.3f] [%s][%s] MapNumber[%d]-(%d/%d)", btFlag, fTime, Obj.AccountID, Obj.Name, Obj.MapNumber, Obj.X, Obj.Y);
		//g_RaklionUtil.SendDataRaklionTimeAttackEvent(iIndex, btFlag, fTime);
	}*/
}

int CRaklionBattleUserMng::GetUserObjIndex(int iBattleUserNumber)
{
	int iObjIndex = m_BattleUser[iBattleUserNumber];
	return iObjIndex;
}

BOOL CRaklionBattleUserMng::IsBattleUser(CGameObject &Obj)
{
	for(int iCount=0; iCount<GetUserCount(); iCount++)
	{
		if(iIndex == m_BattleUser[iCount])
		{
			return TRUE;
		}
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

