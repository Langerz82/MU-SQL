// CastleCrown.cpp: implementation of the CCastleCrown class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CastleCrown.h"
#include "CastleSiege.h"
#include "User/CUserData.h"
#include "Logging/Log.h"
#include "GOFunctions.h"

CCastleCrown g_CsNPC_CastleCrown;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCastleCrown::CCastleCrown()
{

}

CCastleCrown::~CCastleCrown()
{

}

void CCastleCrown::CastleCrownAct(CGameObject &Obj)
{
	if ( Obj.Type != OBJ_NPC ||
		 Obj.Class != 216 )
		 return;

	CGameObject *lpUserObj = getGameObject(g_CastleSiege.GetCrownUserIndex());
	if ( !gObjIsConnected(*lpUserObj) )
	{
		g_CastleSiege.ResetCrownUserIndex();
		g_CastleSiege.SetCrownAccessUserX(0);
		g_CastleSiege.SetCrownAccessUserY(0);
		g_CastleSiege.ResetCrownAccessTickCount();

		return;
	}
	
	if ( lpUserObj->MapNumber == MAP_INDEX_CASTLESIEGE &&
		 lpUserObj->m_btCsJoinSide >= 2 &&
		 lpUserObj->m_bCsGuildInvolved == 1 )
	{
		if ( lpUserObj->Y == g_CastleSiege.GetCrownAccessUserY() &&
			 lpUserObj->X == g_CastleSiege.GetCrownAccessUserX() )
		{
			if ( gObjIsConnected(*getGameObject(g_CastleSiege.GetCrownSwitchUserIndex(217))) &&
				 gObjIsConnected(*getGameObject(g_CastleSiege.GetCrownSwitchUserIndex(218))) )
			{
				if (lpUserObj->m_btCsJoinSide == getGameObject(g_CastleSiege.GetCrownSwitchUserIndex(217))->m_btCsJoinSide &&
					lpUserObj->m_btCsJoinSide == getGameObject(g_CastleSiege.GetCrownSwitchUserIndex(218))->m_btCsJoinSide )	// #warning ??? maybe is 218??? 
				{
					DWORD dwTime = GetTickCount() - g_CastleSiege.GetCrownAccessTickCount();
		
					sLog->outBasic("[CastleSiege] [Reg. Accumulating] Accumulated Crown AccessTime : acc(%d) + %d [%s](%s)(%s)",
						lpUserObj->m_iAccumulatedCrownAccessTime, dwTime,
						lpUserObj->m_PlayerData->GuildName, lpUserObj->m_PlayerData->ConnectUser->AccountID, lpUserObj->Name);

				}
				else
				{
					GSProtocol.GCAnsCsAccessCrownState(lpUserObj, 2);
					g_CastleSiege.ResetCrownUserIndex();
					g_CastleSiege.SetCrownAccessUserX(0);
					g_CastleSiege.SetCrownAccessUserY(0);
					g_CastleSiege.ResetCrownAccessTickCount();

					sLog->outBasic("[CastleSiege] [%s][%s] Register Castle Crown Canceled (GUILD:%s)",
						lpUserObj->m_PlayerData->ConnectUser->AccountID, lpUserObj->Name, lpUserObj->m_PlayerData->GuildName);
				}
			}
			else
			{
				GSProtocol.GCAnsCsAccessCrownState(lpUserObj, 2);
				g_CastleSiege.ResetCrownUserIndex();
				g_CastleSiege.SetCrownAccessUserX(0);
				g_CastleSiege.SetCrownAccessUserY(0);
				g_CastleSiege.ResetCrownAccessTickCount();
			}
		}
		else
		{
			GSProtocol.GCAnsCsAccessCrownState(lpUserObj, 2);
			g_CastleSiege.ResetCrownUserIndex();
			g_CastleSiege.SetCrownAccessUserX(0);
			g_CastleSiege.SetCrownAccessUserY(0);
			g_CastleSiege.ResetCrownAccessTickCount();
		}
	}
	else
	{
		GSProtocol.GCAnsCsAccessCrownState(lpUserObj, 2);
		g_CastleSiege.ResetCrownUserIndex();
		g_CastleSiege.SetCrownAccessUserX(0);
		g_CastleSiege.SetCrownAccessUserY(0);
		g_CastleSiege.ResetCrownAccessTickCount();
	}
}

