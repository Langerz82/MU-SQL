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

void CGuardianStatue::GuardianStatueAct(CGameObject &Obj)
{
	if( ::gObjIsConnected(Obj.m_Index) == FALSE )
		return;

	CGameObject lpObj = Obj;

	if( Obj.VPCount < 1 )
		return;

	int tObjNum = -1;

	for( int i = 0; i < MAX_VIEWPORT; i++ )
	{
		tObjNum = Obj.VpPlayer[i].number;

		if( tObjNum >= 0 )
		{
			if( getGameObject(tObjNum)->Type == OBJ_USER )
			{
				if( getGameObject(tObjNum)->Live )
				{
					if( getGameObject(tObjNum)->m_btCsJoinSide == 1 )
					{
						if( (abs(Obj.X - getGameObject(tObjNum)->X) <= 3) && (abs(Obj.Y - getGameObject(tObjNum)->Y) <= 3) )
						{
							BOOL bIsMaxLife = FALSE;
							BOOL bIsMaxMana = FALSE;
							BOOL bIsMaxBP	= FALSE;

							if( getGameObject(tObjNum)->Life < (getGameObject(tObjNum)->Life+getGameObject(tObjNum)->AddLife) )
							{
								getGameObject(tObjNum)->Life = getGameObject(tObjNum)->Life + ((getGameObject(tObjNum)->AddLife+getGameObject(tObjNum)->MaxLife) * (Obj.m_btCsNpcRgLevel+1)) / 100;

								if( getGameObject(tObjNum)->Life > (getGameObject(tObjNum)->Life+getGameObject(tObjNum)->AddLife) )
								{
									getGameObject(tObjNum)->Life = getGameObject(tObjNum)->Life+getGameObject(tObjNum)->AddLife;
								}
								bIsMaxLife = TRUE;
							}

							if( getGameObject(tObjNum)->Mana < (getGameObject(tObjNum)->Mana+getGameObject(tObjNum)->AddMana) )
							{
								getGameObject(tObjNum)->Mana = getGameObject(tObjNum)->Mana + ((getGameObject(tObjNum)->AddMana+getGameObject(tObjNum)->MaxMana) * (Obj.m_btCsNpcRgLevel+1)) / 100;

								if( getGameObject(tObjNum)->Mana > (getGameObject(tObjNum)->Mana+getGameObject(tObjNum)->AddMana) )
								{
									getGameObject(tObjNum)->Mana = getGameObject(tObjNum)->Mana+getGameObject(tObjNum)->AddMana;
								}
								bIsMaxMana = TRUE;
							}

							if( getGameObject(tObjNum)->BP < (getGameObject(tObjNum)->BP+getGameObject(tObjNum)->AddBP) )
							{
								getGameObject(tObjNum)->BP = getGameObject(tObjNum)->BP + ((getGameObject(tObjNum)->AddBP+getGameObject(tObjNum)->MaxBP) * (Obj.m_btCsNpcRgLevel+1)) / 100;

								if( getGameObject(tObjNum)->BP > (getGameObject(tObjNum)->BP+getGameObject(tObjNum)->AddBP) )
								{
									getGameObject(tObjNum)->BP = getGameObject(tObjNum)->BP+getGameObject(tObjNum)->AddBP;
								}
								bIsMaxMana = TRUE;
							}

							if( bIsMaxLife ) 
							{
								gGameProtocol.GCReFillSend(tObjNum, getGameObject(tObjNum)->Life, 0xFF, 1, getGameObject(tObjNum)->iShield);
							}
							if( bIsMaxMana || bIsMaxBP )
							{
								gGameProtocol.GCManaSend(tObjNum, getGameObject(tObjNum)->Mana, 0xFF, 0, getGameObject(tObjNum)->BP);
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

