// GuardianStatue.cpp: implementation of the CGuardianStatue class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "GuardianStatue.h"
#include "User/CUserData.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGuardianStatue g_CsNPC_GuardianStatue;
CGuardianStatue::CGuardianStatue()
{

}

CGuardianStatue::~CGuardianStatue()
{

}

void CGuardianStatue::GuardianStatueAct(int iIndex)
{
	if( ::gObjIsConnected(iIndex) == FALSE )
		return;

	CGameObject lpObj = &gGameObjects[iIndex];

	if( lpObj.VPCount < 1 )
		return;

	int tObjNum = -1;

	for( int i = 0; i < MAX_VIEWPORT; i++ )
	{
		tObjNum = lpObj.VpPlayer[i].number;

		if( tObjNum >= 0 )
		{
			if( gGameObjects[tObjNum]->Type == OBJ_USER )
			{
				if( gGameObjects[tObjNum]->Live )
				{
					if( gGameObjects[tObjNum]->m_btCsJoinSide == 1 )
					{
						if( (abs(lpObj.X - gGameObjects[tObjNum]->X) <= 3) && (abs(lpObj.Y - gGameObjects[tObjNum]->Y) <= 3) )
						{
							BOOL bIsMaxLife = FALSE;
							BOOL bIsMaxMana = FALSE;
							BOOL bIsMaxBP	= FALSE;

							if( gGameObjects[tObjNum]->Life < (gGameObjects[tObjNum]->Life+gGameObjects[tObjNum]->AddLife) )
							{
								gGameObjects[tObjNum]->Life = gGameObjects[tObjNum]->Life + ((gGameObjects[tObjNum]->AddLife+gGameObjects[tObjNum]->MaxLife) * (lpObj.m_btCsNpcRgLevel+1)) / 100;

								if( gGameObjects[tObjNum]->Life > (gGameObjects[tObjNum]->Life+gGameObjects[tObjNum]->AddLife) )
								{
									gGameObjects[tObjNum]->Life = gGameObjects[tObjNum]->Life+gGameObjects[tObjNum]->AddLife;
								}
								bIsMaxLife = TRUE;
							}

							if( gGameObjects[tObjNum]->Mana < (gGameObjects[tObjNum]->Mana+gGameObjects[tObjNum]->AddMana) )
							{
								gGameObjects[tObjNum]->Mana = gGameObjects[tObjNum]->Mana + ((gGameObjects[tObjNum]->AddMana+gGameObjects[tObjNum]->MaxMana) * (lpObj.m_btCsNpcRgLevel+1)) / 100;

								if( gGameObjects[tObjNum]->Mana > (gGameObjects[tObjNum]->Mana+gGameObjects[tObjNum]->AddMana) )
								{
									gGameObjects[tObjNum]->Mana = gGameObjects[tObjNum]->Mana+gGameObjects[tObjNum]->AddMana;
								}
								bIsMaxMana = TRUE;
							}

							if( gGameObjects[tObjNum]->BP < (gGameObjects[tObjNum]->BP+gGameObjects[tObjNum]->AddBP) )
							{
								gGameObjects[tObjNum]->BP = gGameObjects[tObjNum]->BP + ((gGameObjects[tObjNum]->AddBP+gGameObjects[tObjNum]->MaxBP) * (lpObj.m_btCsNpcRgLevel+1)) / 100;

								if( gGameObjects[tObjNum]->BP > (gGameObjects[tObjNum]->BP+gGameObjects[tObjNum]->AddBP) )
								{
									gGameObjects[tObjNum]->BP = gGameObjects[tObjNum]->BP+gGameObjects[tObjNum]->AddBP;
								}
								bIsMaxMana = TRUE;
							}

							if( bIsMaxLife ) 
							{
								gGameProtocol.GCReFillSend(tObjNum, gGameObjects[tObjNum]->Life, 0xFF, 1, gGameObjects[tObjNum]->iShield);
							}
							if( bIsMaxMana || bIsMaxBP )
							{
								gGameProtocol.GCManaSend(tObjNum, gGameObjects[tObjNum]->Mana, 0xFF, 0, gGameObjects[tObjNum]->BP);
							}
						}
					}
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

