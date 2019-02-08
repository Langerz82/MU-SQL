////////////////////////////////////////////////////////////////////////////////
// MoveCommand.cpp
//GameServer 1.00.77 JPN - Completed
//GameServer 1.00.90 JPN - Completed
#include "StdAfx.h"
#include "MoveCommand.h"
#include "DevilSquare.h"
#include "KalimaGate.h"
#include "ChaosCastle.h"
#include "CastleSiegeSync.h"
#include "IllusionTempleEvent_Renewal.h"
#include "RaklionBattleUserMng.h"
#include "Logging/Log.h"
#include "GensSystem.h"
#include "Gate.h"
#include "Gamemain.h"
#include "configread.h"
#include "CustomMaxStats.h"

CMoveCommand gMoveCommand;

CMoveCommand::CMoveCommand()
{
	return;
}

CMoveCommand::~CMoveCommand()
{
	return;
}

void CMoveCommand::Init()
{
	memset(this->m_MoveCommandData, 0, sizeof(this->m_MoveCommandData));

	for (int i =0;i<MAX_MOVE_COMMAND;i++)
	{
		this->m_MoveCommandData[i].Index = -1;
	}
}

int CMoveCommand::Load(char* filename)
{
	this->Init();

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(filename);

	if (res.status != pugi::status_ok)
	{
		sLog->outError("%s File Load Error (%s)", filename, res.description());
		return 0;
	}

	int counter=0;
	pugi::xml_node warpsettings = file.child("WarpSettings");

	for (pugi::xml_node warp = warpsettings.child("Warp"); warp; warp = warp.next_sibling())
	{
		this->m_MoveCommandData[counter].Index = warp.attribute("Index").as_int();
		strcpy(this->m_MoveCommandData[counter].Name, warp.attribute("ServerName").as_string());
		strcpy(this->m_MoveCommandData[counter].EngName, warp.attribute("ClientName").as_string());
		this->m_MoveCommandData[counter].NeedLevel = warp.attribute("MinLevel").as_int();
		this->m_MoveCommandData[counter].MaxLevel = warp.attribute("MaxLevel").as_int();
		this->m_MoveCommandData[counter].NeedZen = warp.attribute("ReqMoney").as_int();
		this->m_MoveCommandData[counter].GateNumber = warp.attribute("GateNumber").as_int();

		counter++;

		if (counter >= MAX_MOVE_COMMAND)
		{
			sLog->outError("Warp List Limit Reached (60)");
			break;
		}
	}
	
	return TRUE;
}

int CMoveCommand::LoadMoveLevel(char* filename)
{
	memset(this->m_MoveLevel, -1, sizeof(this->m_MoveLevel));

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(filename);

	if (res.status != pugi::status_ok)
	{
		sLog->outError("MoveLevel %s File Load Error (%s)", filename, res.description());
		return 0;
	}

	int counter = 0;
	pugi::xml_node movesettings = file.child("MoveSettings");

	for (pugi::xml_node move = movesettings.child("Move"); move; move = move.next_sibling())
	{
		this->m_MoveLevel[counter].MoveLevel = move.attribute("Level").as_int();
		this->m_MoveLevel[counter].MapNumber = move.attribute("MapNumber").as_int();
		this->m_MoveLevel[counter].X = move.attribute("StartX").as_int();
		this->m_MoveLevel[counter].Y = move.attribute("StartY").as_int();
		this->m_MoveLevel[counter].TX = move.attribute("EndX").as_int();
		this->m_MoveLevel[counter].TY = move.attribute("EndY").as_int();

		counter++;
	}

	return TRUE;
}

int CMoveCommand::GetMoveLevel(int mapnumber, int x, int y, int Class)
{
	for( int i=0;i<MAX_MOVE_COMMAND;i++)
	{
		if ( this->m_MoveLevel[i].MapNumber == mapnumber )
		{
			if ( x >= this->m_MoveLevel[i].X && x <= this->m_MoveLevel[i].TX && y >= this->m_MoveLevel[i].Y && y <= this->m_MoveLevel[i].TY )
			{
				if(mapnumber == MAP_INDEX_CALMNESS)
				{
					return this->m_MoveLevel[i].MoveLevel;
				}

				if ( Class == CLASS_DARKLORD || Class == CLASS_MAGUMSA || Class == CLASS_RAGEFIGHTER || Class == CLASS_GROWLANCER)
				{
					return this->m_MoveLevel[i].MoveLevel * 2 / 3;
				}
				else
				{
					return this->m_MoveLevel[i].MoveLevel;
				}
			}
		}
	}

	return -1;
}

BOOL CMoveCommand::CheckMainToMove(LPOBJ lpObj)
{
	if ( BC_MAP_RANGE(lpObj->MapNumber) != FALSE )
	{
		return FALSE;
	}

	if ( CC_MAP_RANGE(lpObj->MapNumber) != FALSE )
	{
		return FALSE;
	}

	if (lpObj->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		return FALSE;
	}

	if ( KALIMA_MAP_RANGE(lpObj->MapNumber) != FALSE )
	{
		return FALSE;
	}

	if ( DS_MAP_RANGE(lpObj->MapNumber) != FALSE )
	{
		return FALSE;
	}

	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		if (lpObj->MapNumber == MAP_INDEX_KANTURU_BOSS)
		{
			return FALSE;
		}
	}

	if ( IT_MAP_RANGE(lpObj->MapNumber) != FALSE )
	{
		if (g_IT_Event.CheckRelics(lpObj->m_Index) != FALSE)
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CMoveCommand::CheckEquipmentToMove(LPOBJ lpObj, int iTargetMapNumber)
{
	if ( iTargetMapNumber == MAP_INDEX_ATHLANSE )
	{
		if ( lpObj->m_btInvenPetPos != 0 && lpObj->m_wInvenPet != (WORD)-1 )
		{
			if ( lpObj->pInventory[lpObj->m_btInvenPetPos].m_Type == ITEMGET(13,2) )
			{
				return FALSE;
			}

			if ( lpObj->pInventory[lpObj->m_btInvenPetPos].m_Type == ITEMGET(13,3) )
			{
				return FALSE;
			}
		}
	}

	if ( iTargetMapNumber == MAP_INDEX_ICARUS )
	{
		if ( (lpObj->pInventory[lpObj->m_btInvenPetPos].m_Type != ITEMGET(13,3) &&
			 lpObj->pInventory[7].m_Type != ITEMGET(13,30) &&
			 lpObj->pInventory[lpObj->m_btInvenPetPos].m_Type != ITEMGET(13,37) &&
			 lpObj->pInventory[7].m_Type != ITEMGET(12,36) &&//Season 4.5 addon
			 lpObj->pInventory[7].m_Type != ITEMGET(12,37) &&//Season 4.5 addon
			 lpObj->pInventory[7].m_Type != ITEMGET(12,38) &&//Season 4.5 addon
			 lpObj->pInventory[7].m_Type != ITEMGET(12,39) &&//Season 4.5 addon
			 lpObj->pInventory[7].m_Type != ITEMGET(12,40) &&//Season 4.5 addon
			 lpObj->pInventory[7].m_Type != ITEMGET(12,41) &&//Season 4.5 addon
			 lpObj->pInventory[7].m_Type != ITEMGET(12,42) &&//Season 4.5 addon
			 lpObj->pInventory[7].m_Type != ITEMGET(12,43) &&//Season 4.5 addon
			 lpObj->pInventory[7].m_Type != ITEMGET(12,49) &&//Seaason 6.1 addon
			 lpObj->pInventory[7].m_Type != ITEMGET(12,50) &&
			 lpObj->pInventory[lpObj->m_btInvenPetPos].m_Type != ITEMGET(13,4) &&//Season 4.5 addon
			lpObj->pInventory[lpObj->m_btInvenPetPos].m_Type != ITEMGET(13,5) &&//Season 4.5 addon
			 !(lpObj->pInventory[7].m_Type >= ITEMGET(12,262) && lpObj->pInventory[7].m_Type <= ITEMGET(12,265)) &&
			 lpObj->pInventory[7].m_Type != ITEMGET(12,266) && // Season 8
			 lpObj->pInventory[7].m_Type != ITEMGET(12,267) && // Season 8
			 lpObj->pInventory[7].m_Type != ITEMGET(12,268) && // season 9
			lpObj->pInventory[7].m_Type != ITEMGET(12, 269) && // season 10
			lpObj->pInventory[7].m_Type != ITEMGET(12, 270) && // season 10
			 !(lpObj->pInventory[7].m_Type >= ITEMGET(12,130) && lpObj->pInventory[7].m_Type <= ITEMGET(12,135)) &&
			 (lpObj->pInventory[7].m_Type < ITEMGET(12,0) || lpObj->pInventory[7].m_Type > ITEMGET(12,6)) ) ||
			 lpObj->pInventory[lpObj->m_btInvenPetPos].m_Type == ITEMGET(13,2) ||
			 lpObj->pInventory[11].m_Type == ITEMGET(13,10) ||
			 lpObj->pInventory[10].m_Type == ITEMGET(13,10))
		{
			return FALSE;
		}

		if ( lpObj->m_RecallMon >= 0 )
		{
			GSProtocol.GCRecallMonLife(lpObj->m_Index, 60, 0);
			gObjMonsterCallKill(lpObj->m_Index);
		}
	}

	return TRUE;
}

BOOL CMoveCommand::CheckInterfaceToMove(LPOBJ lpObj)
{
	if ( lpObj->m_IfState.use == 1 )
	{
		return FALSE;
	}

	if ( lpObj->m_bPShopOpen == true )
	{
		return FALSE;
	}

	return TRUE;
}

int CMoveCommand::FindIndex(LPSTR mapname)
{
	for ( int i=0;i<MAX_MOVE_COMMAND;i++)
	{
		if ( this->m_MoveCommandData[i].Index == -1 )
		{
			return -1;
		}

		if ( strcmp(mapname, this->m_MoveCommandData[i].Name) == 0 || strcmpi(mapname, this->m_MoveCommandData[i].EngName) == 0  )
		{
			return i;
		}
	}

	return -1;
}

int CMoveCommand::FindIndex(int iMapIndex)
{
	for (int i = 0; i<MAX_MOVE_COMMAND; i++)
	{
		if (this->m_MoveCommandData[i].Index == -1)
		{
			return -1;
		}

		if (this->m_MoveCommandData[i].Index == iMapIndex)
		{
			return i;
		}
	}

	return -1;
}

BOOL CMoveCommand::CheckMoveMapBound(int iMapIndex)
{
	switch ( iMapIndex )
	{
		case 1:
		case 2:
		case 3:
		case 6:
		case 10:
		case 16:
			return TRUE;
	}
	
	return FALSE;
}

bool CMoveCommand::GetInfoByName(char* name, MOVE_COMMAND_DATA* lpInfo) // OK
{
	for (std::map<int, MOVE_COMMAND_DATA>::iterator it = this->m_MoveInfo.begin(); it != this->m_MoveInfo.end(); it++)
	{
		if (_stricmp(it->second.Name, name) == 0)
		{
			(*lpInfo) = it->second;
			return 1;
		}
	}

	return 0;
}

BOOL CMoveCommand::Move(LPOBJ lpObj, int iMapIndex)
{
	int m_number = -1;
	int overlevel = 0;
	int subMoney = 2000;

	int index = this->FindIndex(iMapIndex);

	if (index == -1)
	{
		sLog->outBasic("[MapMove Failed] Invalid Map Index (%s/%s) (Index : %d)", lpObj->AccountID, lpObj->Name, iMapIndex);
		return FALSE;
	}

	int nMoveIndex = this->m_MoveCommandData[index].Index;

	if ( g_GensSystem.IsMoveMapBattleZone(nMoveIndex) )
	{
		if (g_MaxStatsInfo.GetClass.IsNonPvP[lpObj->MapNumber])
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0,305), lpObj->m_Index, 1);
			return FALSE;
		}

		if ( !g_GensSystem.IsUserBattleZoneEnable(lpObj) )
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0,499), lpObj->m_Index, 1);
			return FALSE;
		}

		if ( IMPERIAL_MAP_RANGE(lpObj->MapNumber) != FALSE )
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0,499), lpObj->m_Index, 1);
			return FALSE;
		}

		if ( IT_MAP_RANGE(lpObj->MapNumber) != FALSE )
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0,499), lpObj->m_Index, 1);
			return FALSE;
		}

		GSProtocol.GCServerMsgStringSend(Lang.GetText(0,500), lpObj->m_Index, 1);
		g_GensSystem.MoveInBattleZonePartySplit(lpObj);
	}
	//sLog->outBasic("#4 TEST %d", g_CastleSiegeSync.GetCastleState());
	if ( nMoveIndex == 24 ) //season 3.0 changed
	{
		if ( g_CastleSiegeSync.GetCastleState() == 7 )
		{
			//return FALSE;
		}

		if ( g_CastleSiegeSync.CheckCastleOwnerMember(lpObj->m_Index) == FALSE && g_CastleSiegeSync.CheckCastleOwnerUnionMember(lpObj->m_Index) == FALSE)
		{
			//return FALSE;
		}
	}
	//sLog->outBasic("#5 TEST");
	if(IT_MAP_RANGE(lpObj->MapNumber) != FALSE) //season 3.0 add-on
	{
		if(nMoveIndex == 11 || nMoveIndex == 12 || nMoveIndex == 13 || nMoveIndex == 23)
		{
			return FALSE;
		}
	}

	if ( index == -1 )
	{
		return FALSE;
	}

	int NeedLevel = this->m_MoveCommandData[index].NeedLevel;
	int NeedZen   = this->m_MoveCommandData[index].NeedZen;
	int GateNumber= this->m_MoveCommandData[index].GateNumber;

	NeedZen *= g_GensSystem.PkPenaltyAddNeedZenMapMove(lpObj);
	
	if(g_ConfigRead.data.common.joinmuDisableWarpLevelBenefits == false)
	{
		if ( lpObj->Class == CLASS_DARKLORD || lpObj->Class == CLASS_MAGUMSA || lpObj->Class == CLASS_RAGEFIGHTER || lpObj->Class == CLASS_GROWLANCER)
		{
			if ( NeedLevel > 0 )
			{
				if (GateNumber != 273) //season 3.0 add-on (Swamp of Calmmness)
				{
					NeedLevel = NeedLevel *2 / 3;
				}
			}
		}
	}
	if ( GateNumber >= 0 )
	{
		if (g_ConfigRead.UseMLforWarp == 1)
		{
			if (NeedLevel <= lpObj->Level + lpObj->m_PlayerData->MasterLevel)
			{
				if ((lpObj->m_PlayerData->Money - NeedZen) >= 0)
				{
					if (gObjMoveGate(lpObj->m_Index, GateNumber) != FALSE)
					{
						lpObj->m_bPShopRedrawAbs = true;
						lpObj->m_PlayerData->Money -= NeedZen;

						GSProtocol.GCMoneySend(lpObj->m_Index, lpObj->m_PlayerData->Money);

						if (lpObj->MapNumber == MAP_INDEX_HATCHERY)//Season 4.5 addon
						{
							g_RaklionBattleUserMng.DeleteUserData(lpObj->m_Index);
						}

						return TRUE;
					}
				}
				else
				{
					GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 37), lpObj->m_Index, 1);
				}
			}
			else
			{
				char szTemp[256];
				wsprintf(szTemp, Lang.GetText(0, 30), NeedLevel);
				GSProtocol.GCServerMsgStringSend(szTemp, lpObj->m_Index, 1);
			}
		}
		else
		{
			if (NeedLevel <= lpObj->Level)
			{
				if ((lpObj->m_PlayerData->Money - NeedZen) >= 0)
				{
					if (gObjMoveGate(lpObj->m_Index, GateNumber) != FALSE)
					{
						lpObj->m_bPShopRedrawAbs = true;
						lpObj->m_PlayerData->Money -= NeedZen;

						GSProtocol.GCMoneySend(lpObj->m_Index, lpObj->m_PlayerData->Money);

						if (lpObj->MapNumber == MAP_INDEX_HATCHERY)//Season 4.5 addon
						{
							g_RaklionBattleUserMng.DeleteUserData(lpObj->m_Index);
						}

						return TRUE;
					}
				}
				else
				{
					GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 37), lpObj->m_Index, 1);
				}
			}
			else
			{
				char szTemp[256];
				wsprintf(szTemp, Lang.GetText(0, 30), NeedLevel);
				GSProtocol.GCServerMsgStringSend(szTemp, lpObj->m_Index, 1);
			}
		}
	}

	return FALSE;

}

BOOL CMoveCommand::Move(LPOBJ lpObj, LPSTR mapname)
{
	int m_number = -1;
	int overlevel = 0;
	int subMoney = 2000;

	int index = this->FindIndex(mapname);

	if (index == -1)
	{
		sLog->outBasic("[MapMove Failed] Invalid Map Index (%s/%s) (Index : %d)", lpObj->AccountID, lpObj->Name, mapname);
		return FALSE;
	}

	int nMoveIndex = this->m_MoveCommandData[index].Index;

	if (g_GensSystem.IsMoveMapBattleZone(nMoveIndex))
	{
		if (g_MaxStatsInfo.GetClass.IsNonPvP[lpObj->MapNumber])
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 305), lpObj->m_Index, 1);
			return FALSE;
		}

		if (!g_GensSystem.IsUserBattleZoneEnable(lpObj))
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 499), lpObj->m_Index, 1);
			return FALSE;
		}

		if (IMPERIAL_MAP_RANGE(lpObj->MapNumber) != FALSE)
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 499), lpObj->m_Index, 1);
			return FALSE;
		}

		if (IT_MAP_RANGE(lpObj->MapNumber) != FALSE)
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 499), lpObj->m_Index, 1);
			return FALSE;
		}

		GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 500), lpObj->m_Index, 1);
		g_GensSystem.MoveInBattleZonePartySplit(lpObj);
	}
	//sLog->outBasic("#4 TEST %d", g_CastleSiegeSync.GetCastleState());
	if (nMoveIndex == 24) //season 3.0 changed
	{
		if (g_CastleSiegeSync.GetCastleState() == 7)
		{
			//return FALSE;
		}

		if (g_CastleSiegeSync.CheckCastleOwnerMember(lpObj->m_Index) == FALSE && g_CastleSiegeSync.CheckCastleOwnerUnionMember(lpObj->m_Index) == FALSE)
		{
			//return FALSE;
		}
	}
	//sLog->outBasic("#5 TEST");
	if (IT_MAP_RANGE(lpObj->MapNumber) != FALSE) //season 3.0 add-on
	{
		if (nMoveIndex == 11 || nMoveIndex == 12 || nMoveIndex == 13 || nMoveIndex == 23)
		{
			return FALSE;
		}
	}

	if (index == -1)
	{
		return FALSE;
	}

	int NeedLevel = this->m_MoveCommandData[index].NeedLevel;
	int NeedZen = this->m_MoveCommandData[index].NeedZen;
	int GateNumber = this->m_MoveCommandData[index].GateNumber;

	NeedZen *= g_GensSystem.PkPenaltyAddNeedZenMapMove(lpObj);

	if (g_ConfigRead.data.common.joinmuDisableWarpLevelBenefits == false)
	{
		if (lpObj->Class == CLASS_DARKLORD || lpObj->Class == CLASS_MAGUMSA || lpObj->Class == CLASS_RAGEFIGHTER || lpObj->Class == CLASS_GROWLANCER)
		{
			if (NeedLevel > 0)
			{
				if (GateNumber != 273) //season 3.0 add-on (Swamp of Calmmness)
				{
					NeedLevel = NeedLevel * 2 / 3;
				}
			}
		}
	}
	if (GateNumber >= 0)
	{
		if (g_ConfigRead.UseMLforWarp == 1)
		{
			if (NeedLevel <= lpObj->Level + lpObj->m_PlayerData->MasterLevel)
			{
				if ((lpObj->m_PlayerData->Money - NeedZen) >= 0)
				{
					if (gObjMoveGate(lpObj->m_Index, GateNumber) != FALSE)
					{
						lpObj->m_bPShopRedrawAbs = true;
						lpObj->m_PlayerData->Money -= NeedZen;

						GSProtocol.GCMoneySend(lpObj->m_Index, lpObj->m_PlayerData->Money);

						if (lpObj->MapNumber == MAP_INDEX_HATCHERY)//Season 4.5 addon
						{
							g_RaklionBattleUserMng.DeleteUserData(lpObj->m_Index);
						}

						return TRUE;
					}
				}
				else
				{
					GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 37), lpObj->m_Index, 1);
				}
			}
			else
			{
				if (NeedLevel <= lpObj->Level)
				{
					if ((lpObj->m_PlayerData->Money - NeedZen) >= 0)
					{
						if (gObjMoveGate(lpObj->m_Index, GateNumber) != FALSE)
						{
							lpObj->m_bPShopRedrawAbs = true;
							lpObj->m_PlayerData->Money -= NeedZen;

							GSProtocol.GCMoneySend(lpObj->m_Index, lpObj->m_PlayerData->Money);

							if (lpObj->MapNumber == MAP_INDEX_HATCHERY)//Season 4.5 addon
							{
								g_RaklionBattleUserMng.DeleteUserData(lpObj->m_Index);
							}

							return TRUE;
						}
					}
					else
					{
						GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 37), lpObj->m_Index, 1);
					}
				}
			}
		}
		else
		{
			char szTemp[256];
			wsprintf(szTemp, Lang.GetText(0, 30), NeedLevel);
			GSProtocol.GCServerMsgStringSend(szTemp, lpObj->m_Index, 1);
		}
	}

	return FALSE;

}

int GetKalimaGateLevel(LPOBJ lpObj)
{
	if ( lpObj->Class == CLASS_MAGUMSA || lpObj->Class == CLASS_DARKLORD || lpObj->Class == CLASS_RAGEFIGHTER )
	{
		for ( int i=0;i<KALIMA_FLOORS;i++)
		{
			if ( lpObj->Level >= g_sttKALIMAGATE_LEVEL[i].iLOWER_BOUND_MAGUMSA && lpObj->Level <= g_sttKALIMAGATE_LEVEL[i].iUPPER_BOUND_MAGUMSA )
			{
				return i;
			}
		}
	}
	else
	{
		for ( int i=0;i<KALIMA_FLOORS;i++)
		{
			if ( lpObj->Level >= g_sttKALIMAGATE_LEVEL[i].iLOWER_BOUND && lpObj->Level <= g_sttKALIMAGATE_LEVEL[i].iUPPER_BOUND )
			{
				return i;
			}
		}
	}

	return -1;
}

BOOL CMoveCommand::MoveFree2Kalima(LPOBJ lpObj)
{
	int iIdx = GetKalimaGateLevel(lpObj);

	if ( iIdx >= 6 )
	{
		iIdx = 5;
	}

	if ( iIdx < 0 )
	{
		return FALSE;
	}

	int iGateNumber = iIdx + 88;

	return gObjMoveGate(lpObj->m_Index, iGateNumber);
}

LPMOVE_COMMAND_DATA CMoveCommand::GetMoveCommandData(int nMapIndex)
{
	int nIndex = -1;
	for(int i=0; i<50; i++)
	{
		if(m_MoveCommandData[i].Index == nMapIndex)
		{
			nIndex = i;
			break;
		}
	}
	if( nIndex < 0 )	return NULL;
	return (LPMOVE_COMMAND_DATA)&m_MoveCommandData[nIndex];
}

LPMOVE_COMMAND_DATA CMoveCommand::GetMoveCommandDataByMapNumber(WORD wMapNumber)
{
	int nIndex = -1;
	short x, y, level;
	BYTE mapnum, dir;
	for(int i=0; i<50; i++)
	{
		gGateC.GetGate(m_MoveCommandData[i].GateNumber, x, y, mapnum, dir, level);
		if(mapnum == wMapNumber)
		{
			nIndex = i;
			break;
		}
	}
	if( nIndex < 0 )	return NULL;
	return (LPMOVE_COMMAND_DATA)&m_MoveCommandData[nIndex];
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

