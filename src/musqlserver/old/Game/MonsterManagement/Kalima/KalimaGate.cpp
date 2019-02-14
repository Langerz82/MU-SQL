
#include "KalimaGate.h"
#include "DevilSquare.h"
#include "BloodCastle.h"
#include "ChaosCastle.h"
#include "GameMain.h"

CKalimaGate g_KalimaGate;



CKalimaGate::CKalimaGate()
{
	InitializeCriticalSection(&this->m_critKalimaGate );
}




CKalimaGate::~CKalimaGate()
{
	DeleteCriticalSection(&this->m_critKalimaGate );
}





BOOL CKalimaGate::CreateKalimaGate(CGameObject &Obj, BYTE btLevel, BYTE cTX, BYTE cTY)
{
	BOOL bKalimaGateCreateSucceed=FALSE;
	int iMonsterIndex=-1;

	
	EnterCriticalSection(&this->m_critKalimaGate );
	
	__try
	{
		int iKalimaGateLevel;
		int iMapNumber;
		BYTE btMapAttr;
		int iMonsterType;
		BYTE cX;
		BYTE cY;

		
		if ( gObjIsConnected(Obj.m_Index) == PLAYER_EMPTY )
		{
			return false;
		}

		sLog->outBasic("[Kalima] [%s][%s] Try to Create Kalima Gate", Obj.AccountID, Obj.Name);
		iKalimaGateLevel = this->GetKalimaGateLevel(Obj.m_Index);

		if ( iKalimaGateLevel < btLevel-1 )
		{
			return false;
		}

		iKalimaGateLevel = btLevel-1;

		if (  DS_MAP_RANGE(Obj.MapNumber) )
		{
			return false;
		}

		if ( BC_MAP_RANGE(Obj.MapNumber) )
		{
			return false;
		}

		if ( CC_MAP_RANGE(Obj.MapNumber) )
		{
			return false;
		}

		if (Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
		{
			return false;
		}

		if ( IT_MAP_RANGE(Obj.MapNumber) )
		{
			return false;
		}

		if ( IMPERIAL_MAP_RANGE(Obj.MapNumber) )
		{
			return false;
		}

		if ( KALIMA_FLOOR_RANGE(iKalimaGateLevel)== FALSE )
		{
			sLog->outBasic("[Kalima] [%s][%s] Failed to Summon Kalima Gate - Gate Level Check Error : %d",
				Obj.AccountID, Obj.Name, iKalimaGateLevel+1);
			return false;
		}

		iMapNumber = Obj.MapNumber;

		btMapAttr = MapC[iMapNumber].GetAttr(Obj.X, Obj.Y);

		if ( btMapAttr &1 != 0 )
		{
			sLog->outBasic("[Kalima] [%s][%s] Failed to Summon Kalima Gate - Called in Saftey Area (Map:%d, X:%d, Y:%d)",
				Obj.AccountID, Obj.Name, Obj.MapNumber,
				Obj.X, Obj.Y);
			return false;
		}

		if ( Obj.m_cKalimaGateExist == TRUE )
		{
			gGameProtocol.GCServerMsgStringSend(Lang.GetText(0,150), iIndex, 1);
			sLog->outBasic("[Kalima] [%s][%s] Failed to Summon Kalima Gate - Already Have Gate (SummonIndex:%d)",
				Obj.AccountID, Obj.Name, Obj.m_iKalimaGateIndex);
			return false;
		}

		iMonsterType = iKalimaGateLevel+152;
		cX = cTX;
		cY = cTY;

		btMapAttr = MapC[iMapNumber].GetAttr(cX, cY);

		if ( btMapAttr != 0 )
		{
			if (this->GetRandomLocation(iMapNumber, cX, cY) == 0)
			{
			sLog->outBasic("[Kalima] [%s][%s] Failed to Summon Kalima Gate - Failed to get Location (MapNumber:%d, X:%d, Y:%d)",
				Obj.AccountID, Obj.Name, Obj.MapNumber, cX, cY);
			return false;
			}
		}

		if ( Obj.MapNumber == 10 )
		{
			sLog->outBasic("[Kalima] [%s][%s] Failed to Summon Kalima Gate - Uable to Summon in this Map (MapNumber:%d)",
				Obj.AccountID, Obj.Name, Obj.MapNumber);
			return false;
		}

		iMonsterIndex = gObjAddMonster(iMapNumber);

		if ( iMonsterIndex >= 0 )
		{
			LPMONSTER_ATTRIBUTE lpMATTR = gMAttr.GetAttr(iMonsterType);
			
			if ( lpMATTR == NULL )
			{
				gObjDel(iMonsterIndex);
				sLog->outBasic("[Kalima] [%s][%s] Kalima Gate Vanished - lpMATTR == NULL (SummonIndex:%d)",
					Obj.AccountID, Obj.Name, iMonsterIndex);
				return false;
			}

			gObjSetMonster(iMonsterIndex, iMonsterType);
			gGameObjects[iMonsterIndex]->Live = TRUE;
			gGameObjects[iMonsterIndex]->Life = 1000.0;
			gGameObjects[iMonsterIndex]->MaxLife = 1000.0;
			gGameObjects[iMonsterIndex]->m_PosNum = -1;
			gGameObjects[iMonsterIndex]->X = cX;
			gGameObjects[iMonsterIndex]->Y = cY;
			gGameObjects[iMonsterIndex]->MTX = cX;
			gGameObjects[iMonsterIndex]->MTY = cY;
			gGameObjects[iMonsterIndex]->TX = cX;
			gGameObjects[iMonsterIndex]->TY = cY;
			gGameObjects[iMonsterIndex]->m_OldX = cX;
			gGameObjects[iMonsterIndex]->m_OldY = cY;
			gGameObjects[iMonsterIndex]->StartX = cX;
			gGameObjects[iMonsterIndex]->StartY = cY;
			gGameObjects[iMonsterIndex]->MapNumber = iMapNumber;
			gGameObjects[iMonsterIndex]->m_MoveRange = 0;
			gGameObjects[iMonsterIndex]->m_cKalimaGateEnterCount = 0;

			Obj.m_cKalimaGateExist = TRUE;
			Obj.m_iKalimaGateIndex = iMonsterIndex;

			gGameObjects[iMonsterIndex]->Level = lpMATTR->m_Level;
			gGameObjects[iMonsterIndex]->Type = OBJ_MONSTER;
			gGameObjects[iMonsterIndex]->MaxRegenTime = 1000;
			gGameObjects[iMonsterIndex]->Dir = 1;

			gGameObjects[iMonsterIndex]->m_RecallMon = iIndex;
			gGameObjects[iMonsterIndex]->RegenTime = GetTickCount();
			gGameObjects[iMonsterIndex]->m_Attribute = iKalimaGateLevel + 51;
			gGameObjects[iMonsterIndex]->DieRegen = FALSE;
			
			sLog->outBasic("[Kalima] [%s][%s] Create Kalima Gate Successfuly (Live:%d, GateTick:%u, DieRegen:%d, Map:%d, X:%d, Y:%d, SummonIndex:%d, OwnerIndex:%d, GateNo.:%d)",
				Obj.AccountID, Obj.Name, gGameObjects[iMonsterIndex]->Live, gGameObjects[iMonsterIndex]->RegenTime,
				gGameObjects[iMonsterIndex]->DieRegen, gGameObjects[iMonsterIndex]->MapNumber, gGameObjects[iMonsterIndex]->X,
				gGameObjects[iMonsterIndex]->Y, iMonsterIndex, iIndex, iKalimaGateLevel+1);
			bKalimaGateCreateSucceed = TRUE;
		}
	}
	__finally
	{
		LeaveCriticalSection(&this->m_critKalimaGate);
	}

	return true;
}





void CKalimaGate::KalimaGateAct(CGameObject &Obj)
{
	CGameObject lpObj=NULL;
	CGameObject lpObjCallOwner=NULL;

	if ( ObjectMaxRange(iIndex ) == FALSE)
	{
		return;
	}

	if ( Obj.Live == FALSE )
	{
		return;
	}

	if ( gObjIsConnected( Obj.m_RecallMon ) == PLAYER_EMPTY )
	{
		sLog->outBasic("[Kalima] Kalima Gate Vanished - Summoner Vanished (SummonIndex:%d, EnterCount:%d)",
			iIndex, Obj.m_cKalimaGateEnterCount );
		gGameProtocol.GCDiePlayerSend( Obj, iIndex, 0, 0);
		gObjDel(Obj.m_Index);
		gObjCharZeroSet(Obj.m_Index);
		return;
	}

	lpObj = Obj;
	lpObjCallOwner = &gGameObjects[lpObj.m_RecallMon];

	if ( gGameObjects[Obj.m_RecallMon]->DieRegen > 0 )
	{
		lpObjCallOwner->AccountID[MAX_ACCOUNT_LEN]=0;
		lpObjCallOwner->Name[MAX_ACCOUNT_LEN]=0;

		sLog->outBasic("[Kalima] [%s][%s] Kalima Gate Vanished - Summoner Died (SummonIndex:%d, EnterCount:%d)",
			lpObjCallOwner->AccountID, lpObjCallOwner->Name,
			iIndex, Obj.m_cKalimaGateEnterCount );
		this->DeleteKalimaGate(iIndex, Obj.m_RecallMon);
		return;
	}

	if ( Obj.m_cKalimaGateEnterCount >= MAX_KALIMA_ENTER )
	{
		lpObjCallOwner->AccountID[MAX_ACCOUNT_LEN]=0;
		lpObjCallOwner->Name[MAX_ACCOUNT_LEN]=0;
		
		sLog->outBasic("[Kalima] [%s][%s] Kalima Gate Vanished - Enter Count Over (SummonIndex:%d, EnterCount:%d)",
			lpObjCallOwner->AccountID, lpObjCallOwner->Name,
			iIndex, Obj.m_cKalimaGateEnterCount );
		this->DeleteKalimaGate(iIndex, Obj.m_RecallMon);
		return;
	}
		
	DWORD dwTickCount = GetTickCount();

	if ( dwTickCount > lpObj.RegenTime + KALIMA_GATE_TIMEOUT )
	{
		lpObjCallOwner->AccountID[MAX_ACCOUNT_LEN]=0;
		lpObjCallOwner->Name[MAX_ACCOUNT_LEN]=0;
		
		sLog->outBasic("[Kalima] [%s][%s] Kalima Gate Vanished - Time Out (SummonIndex:%d, EnterCount:%d)",
			lpObjCallOwner->AccountID, lpObjCallOwner->Name,
			iIndex, Obj.m_cKalimaGateEnterCount );
		this->DeleteKalimaGate(lpObj.m_Index, lpObjCallOwner->m_Index);
		return;
	}

	if ( lpObj.VPCount < 1 )
	{
		return;
	}

	int tObjNum=-1;

	for ( int i=0;i<MaxViewportMonster;i++)
	{
		tObjNum = lpObj.VpPlayer[i].number;

		if ( tObjNum >= 0 )
		{
			if ( gGameObjects[tObjNum]->Type == OBJ_USER )
			{
				if ( gGameObjects[tObjNum]->Live != FALSE )
				{
					if ( abs(lpObj.Y - gGameObjects[tObjNum]->Y) < 2 )
					{
						if ( abs(lpObj.X - gGameObjects[tObjNum]->X) < 2 )
						{
							if ( lpObj.m_RecallMon == tObjNum || (gGameObjects[tObjNum]->PartyNumber != -1 && lpObjCallOwner->PartyNumber == gGameObjects[tObjNum]->PartyNumber ))
							{
								int iMoveMapLevel = lpObj.m_Attribute -51;
								int iMapNumber = iMoveMapLevel + 24;
								int iGateNumber = g_iKalimaGateGateIndex[iMoveMapLevel];
								int iOverLevel = this->GetKalimaGateLevel(tObjNum);

								if ( iOverLevel >= iMoveMapLevel )
								{
									if ( gObjMoveGate(tObjNum, iGateNumber) == FALSE )
									{
										sLog->outBasic("[Kalima] [%s][%s] - [%s][%s] move fail (MapNumber:%d)", 
											lpObjCallOwner->AccountID, lpObjCallOwner->Name,
											gGameObjects[tObjNum]->AccountID, gGameObjects[tObjNum]->Name, iMapNumber);
									}
									else
									{
										sLog->outBasic("[Kalima] [%s][%s] - [%s][%s] Transported by Kalima Gate (SummonIndex:%d, GateNo.:%d, MapNumber:%d)",
											lpObjCallOwner->AccountID, lpObjCallOwner->Name, 
											gGameObjects[tObjNum]->AccountID, gGameObjects[tObjNum]->Name, iIndex, iMoveMapLevel + 1, iMapNumber);
										Obj.m_cKalimaGateEnterCount ++;

										if ( Obj.m_cKalimaGateEnterCount >= MAX_KALIMA_ENTER )
										{
											return;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}





int CKalimaGate::CheckOverlapKundunMark(CGameObject &Obj, BYTE btLevel)
{
	for ( int x = 0;x<MAIN_INVENTORY_SIZE;x++ )
	{
		if ( Obj.pInventory[x]->IsItem() == TRUE )
		{
			if ( Obj.pInventory[x]->m_Type == ITEMGET(14, 29) )
			{
				if ( Obj.pInventory[x]->m_Level == btLevel )
				{
					int iITEM_DUR = Obj.pInventory[x]->m_Durability;

					if (iITEM_DUR >= 0 && iITEM_DUR <= IsOverlapItem(Obj.pInventory[x]->m_Type))
					{
						return x;
					}
				}
			}
		}
	}

	return -1;
}



BOOL CKalimaGate::GetRandomLocation(int iMapNumber, BYTE & cX, BYTE & cY)
{
	int iCount = 100;
	int iUX = cX;
	int iUY = cY;

	while ( iCount-- > 0 )
	{
		cX = ( rand() %6) * (((rand()%2==0)?-1:1)) + iUX;
		cY = (rand()%6) * (((rand()%2==0)?-1:1)) + iUY;

		BYTE btMapAttr = MapC[iMapNumber].GetAttr(cX,cY);

		if ( btMapAttr == 0 )
		{
			return TRUE;
		}
	}

	return FALSE;
}
		




			
int CKalimaGate::GetKalimaGateLevel(CGameObject &Obj)
{
	if ( Obj.Class == CLASS_MAGUMSA || Obj.Class == CLASS_DARKLORD || Obj.Class == CLASS_RAGEFIGHTER )	// DarkLord and MagicGLadiaro
	{
		for ( int i =0;i<KALIMA_FLOORS;i++)
		{
			if ( Obj.Level >= g_sttKALIMAGATE_LEVEL[i]->iLOWER_BOUND_MAGUMSA )
			{
				if ( Obj.Level <= g_sttKALIMAGATE_LEVEL[i]->iUPPER_BOUND_MAGUMSA )
				{
					return i;
				}
			}
		}
	}
	else
	{
		for ( int i =0;i<KALIMA_FLOORS;i++)
		{
			if ( Obj.Level >= g_sttKALIMAGATE_LEVEL[i]->iLOWER_BOUND )
			{
				if ( Obj.Level <= g_sttKALIMAGATE_LEVEL[i]->iUPPER_BOUND )
				{
					return i;
				}
			}
		}
	}
	return -1;
}


BOOL CKalimaGate::DeleteKalimaGate(CGameObject &Obj, CGameObject &ObjOwner)
{
	EnterCriticalSection(&this->m_critKalimaGate);

	__try
	{
		if ( !ObjectMaxRange(iKalimaGateIndex ) || !ObjectMaxRange(iCallOwnerIndex ))
		{
			sLog->outBasic("[Kalima] DeleteKalimaGate() - out of Index (iKalimaGateIndex:%d, iCallOwnerIndex:%d)",
				iKalimaGateIndex, iCallOwnerIndex);
			return 0;
		}

		gGameProtocol.GCDiePlayerSend( &gGameObjects[iKalimaGateIndex], iKalimaGateIndex, 0 , 0);
		gGameObjects[iCallOwnerIndex]->m_cKalimaGateExist = FALSE;
		gGameObjects[iCallOwnerIndex]->m_iKalimaGateIndex = -1;
		gObjDel(iKalimaGateIndex);
		gObjCharZeroSet(iKalimaGateIndex);
	}
	__finally
	{
		LeaveCriticalSection(&this->m_critKalimaGate);
	}
	return true;
}




BOOL CKalimaGate::DeleteKalimaGate(CGameObject &Obj)
{
	if ( ObjectMaxRange(iCallOwnerIndex ) == FALSE )
	{	
		sLog->outBasic("[Kalima] DeleteKalimaGate() - out of Index (iCallOwnerIndex:%d)",
			iCallOwnerIndex);
		return false;
	}

	int iKalimaGateIndex = gGameObjects[iCallOwnerIndex]->m_iKalimaGateIndex;

	if (  ObjectMaxRange (iKalimaGateIndex ) == FALSE )
	{	
		return false;
	}

	if ( gGameObjects[iKalimaGateIndex]->Live == FALSE || !KALIMA_ATTRIBUTE_RANGE(gGameObjects[iKalimaGateIndex]->m_Attribute) )
	{
		return FALSE;
	}

	return this->DeleteKalimaGate(iKalimaGateIndex, iCallOwnerIndex);
}


BOOL CKalimaGate::CreateKalimaGate2(CGameObject &Obj, int iMonMapNumber, BYTE cTX, BYTE cTY)
{	
	BOOL bKalimaGateCreateSucceed = FALSE;
	int iMonsterIndex = -1;

	EnterCriticalSection(&this->m_critKalimaGate);

	__try
	{
		if ( !gObjIsConnected(Obj.m_Index))
			return FALSE;

		sLog->outBasic("[Kalima] [%s][%s] Try to Create Kalima Gate (NextMap)",
			Obj.AccountID, Obj.Name);

		if ( iMonMapNumber < MAP_INDEX_KALIMA1 || iMonMapNumber > MAP_INDEX_KALIMA6-1 )
		{
			sLog->outBasic("[Kalima] [%s][%s] Failed to Summon Kalima Gate (NextMap) - MapNumber Error : %d",
				Obj.AccountID, Obj.Name, iMonMapNumber);

			return FALSE;
		}

		if ( Obj.m_cKalimaGateExist == TRUE )
		{
			gGameProtocol.GCServerMsgStringSend(Lang.GetText(0,150), iIndex, 1);

			sLog->outBasic("[Kalima] [%s][%s] Failed to Summon Kalima Gate (NextMap) - Already Have Gate (SummonIndex:%d)",
				Obj.AccountID, Obj.Name, Obj.m_iKalimaGateIndex);
	
			return FALSE;
		}

		int iMonsterType = iMonMapNumber + 129;
		BYTE cX = cTX;
		BYTE cY = cTY;
		BYTE btMapAttr = MapC[iMonMapNumber].GetAttr(cX, cY);

		if ( btMapAttr != 0 )
		{
			if ( this->GetRandomLocation(iMonMapNumber, cX, cY) == FALSE )
			{
				sLog->outBasic("[Kalima] [%s][%s] Failed to Summon Kalima Gate (NextMap) - Failed to get Location (MapNumber:%d, X:%d, Y:%d)",
					Obj.AccountID, Obj.Name, iMonMapNumber, cX, cY);

				return FALSE;
			}
		}

		iMonsterIndex = gObjAddMonster(iMonMapNumber);

		if ( iMonsterIndex >= 0 )
		{
			MONSTER_ATTRIBUTE * lpMATTR = gMAttr.GetAttr(iMonsterType);

			if ( !lpMATTR )
			{
				gObjDel(iMonsterIndex);

				sLog->outBasic("[Kalima] [%s][%s] Kalima Gate Vanished (NextMap) - lpMATTR == NULL (SummonIndex:%d)",
					Obj.AccountID, Obj.Name, iMonsterIndex);

				return FALSE;
			}

			gObjSetMonster(iMonsterIndex, iMonsterType);
			gGameObjects[iMonsterIndex]->Live = TRUE;
			gGameObjects[iMonsterIndex]->Life = 1000.0;
			gGameObjects[iMonsterIndex]->MaxLife = 1000.0;
			gGameObjects[iMonsterIndex]->m_PosNum = -1;
			gGameObjects[iMonsterIndex]->X = cX;
			gGameObjects[iMonsterIndex]->Y = cY;
			gGameObjects[iMonsterIndex]->MTX = cX;
			gGameObjects[iMonsterIndex]->MTY = cY;
			gGameObjects[iMonsterIndex]->TX = cX;
			gGameObjects[iMonsterIndex]->TY = cY;
			gGameObjects[iMonsterIndex]->m_OldX = cX;
			gGameObjects[iMonsterIndex]->m_OldY = cY;
			gGameObjects[iMonsterIndex]->StartX = cX;
			gGameObjects[iMonsterIndex]->StartY = cY;
			gGameObjects[iMonsterIndex]->MapNumber = iMonMapNumber;
			gGameObjects[iMonsterIndex]->m_MoveRange = 0;
			gGameObjects[iMonsterIndex]->m_cKalimaGateEnterCount = 0;

			Obj.m_cKalimaGateExist = TRUE;
			Obj.m_iKalimaGateIndex = iMonsterIndex;

			gGameObjects[iMonsterIndex]->Level = lpMATTR->m_Level;
			gGameObjects[iMonsterIndex]->Type = OBJ_MONSTER;
			gGameObjects[iMonsterIndex]->MaxRegenTime = 1000;
			gGameObjects[iMonsterIndex]->Dir = 1;

			gGameObjects[iMonsterIndex]->m_RecallMon = iIndex;
			gGameObjects[iMonsterIndex]->RegenTime = GetTickCount();
			gGameObjects[iMonsterIndex]->m_Attribute = 58;
			gGameObjects[iMonsterIndex]->DieRegen = FALSE;
			
			sLog->outBasic("[Kalima] [%s][%s] Create Kalima Gate Successfuly (NextMap) (Live:%d, GateTick:%u, DieRegen:%d, Map:%d, X:%d, Y:%d, SummonIndex:%d, OwnerIndex:%d)",
				Obj.AccountID, Obj.Name, gGameObjects[iMonsterIndex]->Live, gGameObjects[iMonsterIndex]->RegenTime,
				gGameObjects[iMonsterIndex]->DieRegen, gGameObjects[iMonsterIndex]->MapNumber, gGameObjects[iMonsterIndex]->X,
				gGameObjects[iMonsterIndex]->Y, iMonsterIndex, iIndex);
			bKalimaGateCreateSucceed = TRUE;

		}
	}
	__finally
	{
		LeaveCriticalSection(&this->m_critKalimaGate);
	}

	return TRUE;
}

/////////////////////////////////////////////////
// START REVIEW HERE

void CKalimaGate::KalimaGateAct2(CGameObject &Obj)
{
	CGameObject lpObj = NULL;
	CGameObject lpObjCallOwner = NULL;

	if ( ObjectMaxRange(Obj.m_Index) == FALSE )
		return;

	if ( Obj.Live == 0 )
		return;

	if ( !gObjIsConnected(Obj.m_RecallMon ) )
	{
		sLog->outBasic("[Kalima] Kalima Gate Vanished (NextMap) - Summoner Vanished (SummonIndex:%d, EnterCount:%d)",
			iIndex, Obj.m_cKalimaGateEnterCount);

		gGameProtocol.GCDiePlayerSend(Obj, iIndex, 0, 0);
		gObjDel(Obj.m_Index);
		gObjCharZeroSet(Obj.m_Index);

		return;
	}

	lpObj = Obj;
	lpObjCallOwner = &gGameObjects[lpObj.m_RecallMon];

	if ( Obj.m_cKalimaGateEnterCount >= MAX_KALIMA_ENTER )
	{
		lpObjCallOwner->AccountID[MAX_ACCOUNT_LEN]=0;
		lpObjCallOwner->Name[MAX_ACCOUNT_LEN]=0;

		sLog->outBasic("[Kalima] [%s][%s] Kalima Gate Vanished - Enter Count Over (SummonIndex:%d, EnterCount:%d)",
			lpObjCallOwner->AccountID, lpObjCallOwner->Name, iIndex, Obj.m_cKalimaGateEnterCount);

		this->DeleteKalimaGate(iIndex, Obj.m_RecallMon);

		return;
	}

	DWORD dwTickCount = GetTickCount();

	if ( dwTickCount > (lpObj.RegenTime+60000) )
	{
		lpObjCallOwner->AccountID[MAX_ACCOUNT_LEN]=0;
		lpObjCallOwner->Name[MAX_ACCOUNT_LEN]=0;

		sLog->outBasic("[Kalima] [%s][%s] Kalima Gate Vanished (NextMap) - Time Out (SummonIndex:%d, EnterCount:%d)",
			lpObjCallOwner->AccountID, lpObjCallOwner->Name, iIndex, Obj.m_cKalimaGateEnterCount);

		this->DeleteKalimaGate(lpObj.m_Index, lpObjCallOwner->m_Index);

		return;
	}

	if ( lpObj.VPCount < 1 )
		return;

	if ( lpObj.MapNumber < MAP_INDEX_KALIMA1 || lpObj.MapNumber > MAP_INDEX_KALIMA6-1 )
		return;

	int tObjNum = -1;

	for ( int i=0;i<MaxViewportMonster;i++)
	{
		tObjNum = lpObj.VpPlayer[i].number;

		if ( tObjNum >= 0 )
		{
			if ( gGameObjects[tObjNum]->Type == OBJ_USER )
			{
				if ( gGameObjects[tObjNum]->Live )
				{
					if ( abs(lpObj.Y - gGameObjects[tObjNum]->Y) < 2 )
					{
						if ( abs(lpObj.X - gGameObjects[tObjNum]->X) < 2 )
						{
							if ( lpObj.m_RecallMon == tObjNum || (gGameObjects[tObjNum]->PartyNumber != -1 && lpObjCallOwner->PartyNumber == gGameObjects[tObjNum]->PartyNumber ))
							{
								int iMoveMapLevel = lpObj.MapNumber - MAP_INDEX_KALIMA1+1;	// #error "Deatwway - remove the -1
								int iGateNumber = g_iKalimaGateGateIndex[iMoveMapLevel];

								if ( !gObjMoveGate(tObjNum, iGateNumber))
								{
									sLog->outBasic("[Kalima] [%s][%s] - [%s][%s] move fail (MapNumber:%d)",
										lpObjCallOwner->AccountID, lpObjCallOwner->Name,
										gGameObjects[tObjNum]->AccountID, gGameObjects[tObjNum]->Name,
										lpObj.MapNumber);
								}
								else
								{
									sLog->outBasic("[Kalima] [%s][%s] - [%s][%s] Transported by Kalima Gate (NextMap) (SummonIndex:%d, GateNo.:%d, MapNumber:%d)",
										lpObjCallOwner->AccountID, lpObjCallOwner->Name,
										gGameObjects[tObjNum]->AccountID, gGameObjects[tObjNum]->Name,
										iIndex, iMoveMapLevel+1, lpObj.MapNumber);

									Obj.m_cKalimaGateEnterCount++;

									if ( Obj.m_cKalimaGateEnterCount >= MAX_KALIMA_ENTER )
										break;
								}
							}
						}
					}
				}
			}
		}
	}
}



int CKalimaGate::GetKalimaGateLevel2(CGameObject &Obj)
{
	if ( Obj.Class == CLASS_MAGUMSA || Obj.Class == CLASS_DARKLORD || Obj.Class == CLASS_RAGEFIGHTER )	// DarkLord and MagicGLadiaro
	{
		for ( int i =0;i<KALIMA_FLOORS;i++)
		{
			if ( Obj.Level >= g_sttKALIMAGATE_LEVEL[i]->iLOWER_BOUND_MAGUMSA )
			{
				if ( Obj.Level <= g_sttKALIMAGATE_LEVEL[i]->iUPPER_BOUND_MAGUMSA )
				{
					return i;
				}
			}
		}
	}
	else
	{
		for ( int i =0;i<KALIMA_FLOORS;i++)
		{
			if ( Obj.Level >= g_sttKALIMAGATE_LEVEL[i]->iLOWER_BOUND )
			{
				if ( Obj.Level <= g_sttKALIMAGATE_LEVEL[i]->iUPPER_BOUND )
				{
					return i;
				}
			}
		}
	}
	return -1;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

