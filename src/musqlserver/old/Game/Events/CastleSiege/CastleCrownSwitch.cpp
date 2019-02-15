// CastleCrownSwitch.cpp: implementation of the CCastleCrownSwitch class.
//	GS-CS	1.00.19	JPN	0x00561370	-	Completed
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CastleCrownSwitch.h"

#include "CastleSiege.h"
#include "User/CUserData.h"
#include "Logging/Log.h"

CCastleCrownSwitch g_CsNPC_CastleCrownSwitch;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCastleCrownSwitch::CCastleCrownSwitch()
{
	return;
}

CCastleCrownSwitch::~CCastleCrownSwitch()
{
	return;
}

void CCastleCrownSwitch::CastleCrownSwitchAct(CGameObject &Obj)
{
	if ( !gObjIsConnected(Obj.m_Index))
		return;

	if ( Obj.Type != OBJ_NPC ||
		((Obj.Class < 217 )?FALSE:(Obj.Class > 219-1 )?FALSE:TRUE)==FALSE )
		return;

	if ( !gObjIsConnected(g_CastleSiege.GetCrownSwitchUserIndex(Obj.Class)) )
	{
		g_CastleSiege.ResetCrownSwitchUserIndex(Obj.Class);

		if ( g_CastleSiege.GetRegCrownAvailable() == 1 )
		{
			g_CastleSiege.SetRegCrownAvailable(FALSE);
			g_CastleSiege.NotifyCrownState(1);
		}

		return;
	}
	
	CGameObject lpObj= Obj;
	CGameObject lpUserObj = &getGameObject(g_CastleSiege.GetCrownSwitchUserIndex(Obj.Class));
	g_CastleSiege.NotifyCrownSwitchInfo(Obj.m_Index);

	if ( lpUserObj.MapNumber == MAP_INDEX_CASTLESIEGE &&
		 lpUserObj.m_btCsJoinSide >= 2 )
	{
		if ( abs(lpObj.Y - lpUserObj.Y) <= 3 &&
			 abs(lpObj.X - lpUserObj.X) <= 3 )
		{
			int iCrownIndex1 = g_CastleSiege.GetCrownSwitchUserIndex(217);
			int iCrownIndex2 = g_CastleSiege.GetCrownSwitchUserIndex(218);

			if (gObjIsConnected(iCrownIndex1) && gObjIsConnected(iCrownIndex2) )
			{
				if ( getGameObject(iCrownIndex1)->m_btCsJoinSide == getGameObject(iCrownIndex2)->m_btCsJoinSide )
				{
					if ( g_CastleSiege.GetRegCrownAvailable() == 0 )
					{
						g_CastleSiege.SetRegCrownAvailable(TRUE);
						g_CastleSiege.NotifyCrownState(0);
					}
				}
			}
		}
		else
		{
			gGameProtocol.GCAnsCsAccessSwitchState(lpUserObj.m_Index, lpObj.m_Index, -1, 0);
			g_CastleSiege.ResetCrownSwitchUserIndex(Obj.Class);

			if ( g_CastleSiege.GetRegCrownAvailable() == 1 )
			{
				g_CastleSiege.SetRegCrownAvailable(FALSE);
				g_CastleSiege.NotifyCrownState(1);
			}

			sLog->outBasic("[CastleSiege] [%s][%s] Push Castle Crown Switch Canceled (GUILD:%s) - CS X:%d/Y:%d",
				lpUserObj.AccountID, lpUserObj.Name, lpUserObj.m_PlayerData->GuildName, lpObj.X, lpObj.Y);
		}
	}
	else
	{
		gGameProtocol.GCAnsCsAccessSwitchState(lpUserObj.m_Index, lpObj.m_Index, -1, 0);

		g_CastleSiege.ResetCrownSwitchUserIndex(Obj.Class);

		if ( g_CastleSiege.GetRegCrownAvailable() == 1 )
		{
			g_CastleSiege.SetRegCrownAvailable(FALSE);
			g_CastleSiege.NotifyCrownState(1);
		}
	}
}



////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

