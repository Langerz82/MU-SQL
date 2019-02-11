////////////////////////////////////////////////////////////////////////////////
// gObjMonster.cpp
#include "GameServer.h"
#include "gObjMonster.h"
#include "MonsterSetBase.h"
#include "Main.h"
#include "BattleSoccerManager.h"
#include "Mercenary.h"
#include "GameProtocol.h"
#include "BloodCastle.h"
#include "KalimaGate.h"
#include "MonsterHerd.h"
#include "CrywolfSync.h"
#include "TMonsterSkillManager.h"
#include "MonsterItemMng.h"
#include "ChaosCastle.h"
#include "QuestInfo.h"
#include "DevilSquare.h"
#include "BuffEffect.h"
#include "BuffEffectSlot.h"
#include "ImperialGuardian.h"
#include "CastleSiege.h"
#include "LifeStone.h"
#include "Raklion.h"
#include "DoppelGanger.h"
#include "ItemSocketOptionSystem.h"
#include "CashShop.h"
#include "CItemDrop.h"
#include "configread.h"
#include "SetItemDrop.h"
#include "PentagramSystem.h"
#include "ObjUseSkill.h"
#include "MapAttribute.h"
#include "GensSystem.h"
#include "NewPVP.h"
#include "MonsterKillCount.h"
#include "MineSystem.h"
#include "QuestExpProgMng.h"
#include "BagManager.h"
#include "AppointItemDrop.h"
#include "SendNPCInfo.h"
#include "ChaosCastleFinal.h"
#include "MuRummyMng.h"
#include "LargeRand.h"
#include "VipSys.h"
#include "AcheronGuardianEvent.h"
#include "ItemOptionTypeMng.h"
#include "BonusEvent.h"
#include "Kanturu.h"
#include "EledoradoEvent.h"
#include "EventMonSetBase.h"
#include "MonsterRegenSystem.h"
#include "EvolutionMonsterMng.h"
#include "ItemDropEx.h"
#include "ItemBags.h"
#include "CustomMaxStats.h"
#include "SocketItemType.h"
#include "Notice.h"

// GS-N 0.99.60T 0x0040EC50
//	GS-N	1.00.18	JPN	0x004129D0	-	Completed

CQeustNpcTeleport gQeustNpcTeleport("Marlon");	// line : 91
CQeustNpcTeleport gQuestExpNpcTeleport("Zyro");

bool g_bActiveUrukTrap = false;
bool g_bSilvesterEntranceBlock = false;
UINT64 g_i64LordSilvesterBlockTime = 0;

void gObjMonsterMoveRegen(int x, int y, CGameObject* lpObj)
{
	gMSetBase.GetBoxPosition(lpObj->MapNumber, x-1, y-1, x+1, y+1, lpObj->X, lpObj->Y);

	lpObj->TX = lpObj->X;
	lpObj->TY = lpObj->Y;
	lpObj->MTX = lpObj->X;
	lpObj->MTY = lpObj->Y;
	lpObj->StartX = lpObj->X;
	lpObj->StartY = lpObj->Y;
}

BOOL gObjMonsterRegen(LPGameObject &lpObj)
{
	int map[3];

	map[0] = 0;
	map[1] = 2;
	map[2] = 3;

	gObjViewportListProtocolDestroy(lpObj);
	gObjViewportClose(lpObj);

	lpObj->m_ActState.Attack = 0;
	lpObj->m_ActState.EmotionCount = 0;
	lpObj->m_ActState.Escape = 0;
	lpObj->m_ActState.Move = 0;
	lpObj->m_ActState.Rest = 0;
	lpObj->m_ActState.Emotion = 0;
	lpObj->TargetNumber = -1;
	lpObj->NextActionTime = 5000;
	lpObj->PathStartEnd = 0;
	lpObj->PathCur = 0;
	lpObj->PathCount = 0;
	lpObj->m_nPhaseLordSilvester = 0;

	if ( lpObj->Class == 673 )
	{
		lpObj->m_iPentagramMainAttribute = 6;
		g_bActiveUrukTrap = false;
		g_bSilvesterEntranceBlock = false;
		g_i64LordSilvesterBlockTime = 0;

		UnBlockLordSilvesterEntrance();
		SendLordSilvesterBlockInfo(-1, 1);

	}
	if ( lpObj->m_iPentagramMainAttribute == 6 )
	{
		switch ( rand()%5 )
		{
			case 0:
				lpObj->m_iPentagramMainAttribute = EL_FIRE;
				break;
			case 1:
				lpObj->m_iPentagramMainAttribute = EL_WATER;
				break;
			case 2:
				lpObj->m_iPentagramMainAttribute = EL_EARTH;
				break;
			case 3:
				lpObj->m_iPentagramMainAttribute = EL_WIND;
				break;
			case 4:
				lpObj->m_iPentagramMainAttribute = EL_DARKNESS;
				break;
		}
	}

	if ( lpObj->Class == 43 )
	{
		lpObj->MapNumber = map[ rand() % 3 ];

		if ( gMSetBase.GetBoxPosition(lpObj->MapNumber, 10, 10, 240, 240, lpObj->X, lpObj->Y)== FALSE )
		{
			lpObj->Live = FALSE;
			lpObj->m_State = 4;
			lpObj->RegenTime = GetTickCount();
			lpObj->DieRegen = TRUE;
			return FALSE;
		}

		lpObj->TX = lpObj->X;
		lpObj->TY = lpObj->Y;
		lpObj->MTX = lpObj->X;
		lpObj->MTY = lpObj->Y;
		lpObj->StartX = lpObj->X;
		lpObj->StartY = lpObj->Y;
	}

	else if ( DragonEvent->IsDragonEventMonster(lpObj->m_Index) == true )
	{
		gObjDel(lpObj->m_Index);
		return FALSE;
	}

	else if ( AttackEvent->IsAttackEventMonster(lpObj->m_Index) == true )
	{
		gObjDel(lpObj->m_Index);
		return FALSE;
	}

	else if (gEledoradoEvent.IsEledoradoMonster(lpObj->m_Index) == true)
	{
		gObjDel(lpObj->m_Index);
		return FALSE;
	}
 
	else if ( (lpObj->m_Attribute < 51) ? FALSE : (lpObj->m_Attribute > 58)? FALSE : TRUE )
	{
		lpObj->Live = TRUE;
		lpObj->DieRegen = FALSE;
		CreateFrustrum(lpObj->X, lpObj->Y, lpObj->m_Index);
		gObjClearViewport(lpObj);
		gObjViewportListCreate(lpObj->m_Index);
		gObjViewportListProtocolCreate(lpObj);
		return FALSE;
	}

	else if (lpObj->Class == 460 || lpObj->Class == 461 || lpObj->Class == 462) //Season 4.5 addon
	{
		if (g_Raklion.GetRaklionState() != RAKLION_STATE_END)
		{
			return FALSE;
		}
	}
	else if (lpObj->Class == 459)//Season 4.5 addon
	{
		if (g_Raklion.GetRaklionState() != RAKLION_STATE_STANDBY || g_Raklion.GetRaklionState() != RAKLION_STATE_READY)
		{
			lpObj->Live = FALSE;
			lpObj->m_State = 4;
			lpObj->RegenTime = GetTickCount();
			lpObj->DieRegen = TRUE;
			return FALSE;
		}
	}
	else if ((lpObj->Class == 457 && lpObj->MapNumber == MAP_INDEX_HATCHERY) || (lpObj->Class == 458 && lpObj->MapNumber == MAP_INDEX_HATCHERY))//Season 4.5 addon
	{
		lpObj->Live = FALSE;
		lpObj->m_State = 4;
		lpObj->RegenTime = GetTickCount();
		lpObj->DieRegen = TRUE;
		return FALSE;
	}

	else if ( lpObj->Class >= 504 && lpObj->Class <= 521 )
	{
		return FALSE;
	}

	else if (lpObj->Class == 681 || lpObj->Class == 690)
	{
		return FALSE;
	}

	else if ( lpObj->Class >= 523 && lpObj->Class <= 528 )
	{
		return FALSE;
	}

	else if ( lpObj->Class >= 586 && lpObj->Class <= 602 && g_ArcaBattle.IsArcaBattleServer())
	{
		return FALSE;
	}

	else if ( lpObj->Class >= 627 && lpObj->Class <= 641 && g_ArcaBattle.IsArcaBattleServer())
	{
		return FALSE;
	}

	else if ( lpObj->Class >= 658 && lpObj->Class <= 668 )
	{
		return FALSE;
	}

	else if ( lpObj->Class >= 678 && lpObj->Class <= 680 )
	{
		return FALSE;
	}

	else if ( lpObj->Class >= 560 && lpObj->Class <= 561 )
	{
		return FALSE;
	}

	else if ( lpObj->MapNumber == MAP_INDEX_DEVILSQUARE_FINAL )
	{
		return FALSE;
	}

	else if (BC_MAP_RANGE(lpObj->MapNumber))
	{
		int iBridgeIndex = g_BloodCastle.GetBridgeIndex(lpObj->MapNumber);
	//	));
		BLOODCASTLE_MONSTER_POSITION * lpPos = g_BloodCastle.GetMonsterPosData(lpObj->m_PosNum, iBridgeIndex, lpObj->Class);

		//sLog->outBasic("[Blood Castle][Bug Tracker] (%d) Regen Monster (Class:%d) (Index:%d) (PosNum:%d) (lpPos:%d)",
		//	iBridgeIndex + 1, lpObj->Class, lpObj->m_Index, lpObj->m_PosNum, lpPos != nullptr ? true : false);

		if (lpPos == nullptr)
		{
			sLog->outError( "[Blood Castle][Error] lpPos == nullptr (%d) (%d) (%d)", lpObj->m_PosNum, iBridgeIndex, lpObj->Class);
			return FALSE;
		}

		g_BloodCastle.GetPosition(lpPos, lpObj->MapNumber, lpObj->X, lpObj->Y);
		lpObj->MTX = lpObj->X;
		lpObj->MTY = lpObj->Y;
		lpObj->TX = lpObj->X;
		lpObj->TY = lpObj->Y;
		lpObj->StartX = lpObj->X;
		lpObj->StartY = lpObj->Y;
	}
	
	else if (DS_MAP_RANGE(lpObj->MapNumber))
	{
		g_DevilSquare.GetPosition(lpObj->m_PosNum, lpObj->m_bDevilSquareIndex, lpObj->MapNumber, lpObj->X, lpObj->Y);
		lpObj->MTX = lpObj->X;
		lpObj->MTY = lpObj->Y;
		lpObj->TX = lpObj->X;
		lpObj->TY = lpObj->Y;
		lpObj->StartX = lpObj->X;
		lpObj->StartY = lpObj->Y;
	}

	else if (g_MineSystem.IsTwinkle(lpObj->Class) == TRUE)
	{
		lpObj->MTX = lpObj->X;
		lpObj->MTY = lpObj->Y;
		lpObj->TX = lpObj->X;
		lpObj->TY = lpObj->Y;
		lpObj->StartX = lpObj->X;
		lpObj->StartY = lpObj->Y;
	}

	else if (gEventMSetBase.IsEventMonster(lpObj->m_Index) == true)
	{
		gEventMSetBase.RegenMonster(lpObj->m_Index);
	}

	else if ( gMSetBase.GetPosition(lpObj->m_PosNum, lpObj->MapNumber, lpObj->X, lpObj->Y) == FALSE )
	{
		if ( g_ConfigRead.server.GetServerType() == SERVER_CASTLE && lpObj->m_Attribute == 62 )
		{
			BYTE cX = lpObj->X;
			BYTE cY = lpObj->Y;

			if(gObjGetRandomFreeLocation(lpObj->MapNumber,(BYTE&)cX,(BYTE&)cY,5,5,30) == 0 )
			{
				return FALSE;
			}
			else
			{
				lpObj->X = cX;
				lpObj->Y = cY;
			}
		}
		else
		{
			lpObj->Live = FALSE;
			lpObj->m_State = 4;
			lpObj->RegenTime = GetTickCount();
			lpObj->DieRegen = TRUE;
			return FALSE;
		}
	}
	else
	{
		lpObj->MTX = lpObj->X;
		lpObj->MTY = lpObj->Y;
		lpObj->TX = lpObj->X;
		lpObj->TY = lpObj->Y;
		lpObj->StartX = lpObj->X;
		lpObj->StartY = lpObj->Y;
		int iPentagramMainAttribute = 0;
		gMSetBase.GetPentagramMainAttribute(lpObj->m_PosNum, &iPentagramMainAttribute);

		if ( iPentagramMainAttribute )
		{
			lpObj->m_iPentagramMainAttribute = iPentagramMainAttribute;
		}
	}

	gObjMonsterHitDamageInit(lpObj);

	if ( lpObj->MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE && g_CrywolfSync.GetCrywolfState() >= 2 && g_CrywolfSync.GetCrywolfState() <= 5  ) // Crywolf
	{
	
	}

	else if ( g_CrywolfSync.GetOccupationState() == 0 && g_CrywolfSync.GetApplyBenefit() == TRUE)
	{
		lpObj->Life = ( lpObj->m_iScriptMaxLife * g_CrywolfSync.GetMonHPBenefitRate()  ) / 100;
		lpObj->MaxLife = (lpObj->m_iScriptMaxLife * g_CrywolfSync.GetMonHPBenefitRate() ) / 100;
	}

	if (lpObj->Class >= 605 && lpObj->Class <= 607)
	{
		g_MineSystem.SetTwinkleInfo(lpObj->m_Index);
	}

	return TRUE;
}







int gObjMonsterViewportIsCharacter(LPGameObject &lpObj)
{
	int tObjNum;

	for ( int n = 0; n<MaxViewportMonster ; n++ )
	{
		if ( lpObj->VpPlayer2[n].state  != 0 )
		{
			tObjNum = lpObj->VpPlayer2[n].number;

			if ( tObjNum == lpObj->TargetNumber ) 
			{
				return n;
			}
		}
	}

	return -1;
}






void gObjMonsterHitDamageInit(LPGameObject &lpObj)
{
	for ( int i = 0; i< MAX_ST_HIT_DAMAGE ; i++ )
	{
		lpObj->sHD[i].number  = -1;
	}

	lpObj->sHDCount = 0;
}






int gObjMonsterHitDamageUserDel(LPGameObject &lpObj)
{
	int delok = 0;
	int delindex = -1;

	for (int n = 0; n < MAX_ST_HIT_DAMAGE; n++)
	{
		if (lpObj->sHD[n].number >= 0)
		{
			delok = FALSE;

			if (gObjIsConnected(lpObj->sHD[n].number) == TRUE)
			{
				if ((GetTickCount() - lpObj->sHD[n].LastHitTime) > 20000)
				{
					delok = TRUE;
				}

				if (gGameObjects[lpObj->sHD[n].number].MapNumber != lpObj->MapNumber)
				{
					delok = TRUE;
				}
				else if (gObjCheckTileArea(lpObj->m_Index, gGameObjects[lpObj->sHD[n].number].X, gGameObjects[lpObj->sHD[n].number].Y, 20) == FALSE)
				{
					delok = TRUE;
				}
			}
			else
			{
				delok = TRUE;
			}

			if (delok == TRUE)
			{
				lpObj->sHD[n].number = -1;
				delindex = n;
				lpObj->sHDCount--;
			}

		}
	}

	return delindex;
}





void gObjMonsterSetHitDamage(LPGameObject &lpObj, int hit_player, int hit_damage)
{
	int blank = -1;

	for (int n=0;n<MAX_ST_HIT_DAMAGE;n++)
	{
		if ( lpObj->sHD[n].number >= 0 )
		{
			if ( lpObj->sHD[n].number == hit_player )
			{
				lpObj->sHD[n].HitDamage += hit_damage;
				lpObj->sHD[n].LastHitTime = GetTickCount();

				if ( lpObj->sHD[n].HitDamage > lpObj->MaxLife )
				{
					lpObj->sHD[n].HitDamage = lpObj->MaxLife;
				}

				return;
			}
		}
		else if ( blank < 0 )
		{
			blank = n;
		}
	}

	if ( blank < 0 )
	{
		blank = gObjMonsterHitDamageUserDel(lpObj);
	}

	if ( blank >= 0 )
	{
		lpObj->sHD[blank].number = hit_player;
		lpObj->sHD[blank].HitDamage = hit_damage;

		if ( lpObj->sHD[blank].HitDamage > lpObj->MaxLife )
		{
			lpObj->sHD[blank].HitDamage = lpObj->MaxLife;
		}
	
		lpObj->sHD[blank].LastHitTime = GetTickCount();
		lpObj->sHDCount++;
	}
	else
	{
		sLog->outBasic("error-L2 : Hit Damage Set error");
	}
}






int gObjMonsterTopHitDamageUser(LPGameObject &lpMonObj)
{
	int MaxHitDamage = 0;
	int MaxHitDamageUser = -1;

	for ( int n=0;n<MAX_ST_HIT_DAMAGE;n++)
	{
		if ( lpMonObj->sHD[n].number >= 0 )
		{
			if ( lpMonObj->sHD[n].HitDamage > MaxHitDamage )
			{
				MaxHitDamage = lpMonObj->sHD[n].HitDamage;
				MaxHitDamageUser = lpMonObj->sHD[n].number;
			}
		}
	}

	return MaxHitDamageUser;
}







int	 gObjMonsterLastHitDamageUser(LPGameObject &lpMonObj, int & hitindex)
{
	int LastHitTime = 0;
	int LastHitUser = -1;

	for ( int n=0;n<MAX_ST_HIT_DAMAGE;n++)
	{
		if ( lpMonObj->sHD[n].number >= 0 )
		{
			if ( lpMonObj->sHD[n].LastHitTime > LastHitTime )
			{
				LastHitTime = lpMonObj->sHD[n].LastHitTime;
				LastHitUser = lpMonObj->sHD[n].number;
				hitindex = n;
			}
		}
	}

	return LastHitUser;
}

int gObjMonsterGetDamageUser(LPGameObject &lpMonObj, CGameObject* lpUserObj)
{
	for (int n = 0; n<MAX_ST_HIT_DAMAGE; n++)
	{
		if (lpMonObj->sHD[n].number >= 0)
		{
			if (lpMonObj->sHD[n].number == lpUserObj->m_Index)
			{
				return lpMonObj->sHD[n].HitDamage;
			}
		}
	}
}





BOOL gObjMonsterMoveCheck(LPGameObject &lpObj, int tx, int ty)
{
	if ( lpObj->m_ActState.Emotion  == 1 )
	{
		return TRUE;
	}

	if ((lpObj->Class >= 647 && lpObj->Class <= 650) && lpObj->m_PosNum < 0)
	{
		return TRUE;
	}

	if ((lpObj->Class >= 653 && lpObj->Class <= 657) && lpObj->m_PosNum < 0)
	{
		return TRUE;
	}

	if ((lpObj->Class >= 674 && lpObj->Class <= 677) && lpObj->m_PosNum < 0)
	{
		return TRUE;
	}

	if (DragonEvent->IsDragonEventMonster(lpObj->m_Index) == true)
	{
		DRAGON_EVENT_INFO * lpPos = DragonEvent->GetMonsterPos(lpObj->Class);

		if (lpPos == NULL)
		{
			return FALSE;
		}

		tx -= lpObj->StartX;
		ty -= lpObj->StartY;

		int dis = sqrt(static_cast<float>(tx * tx + ty * ty));

		if (dis < 1 || dis > lpPos->m_Dis)
		{
			return FALSE;
		}

		return TRUE;
	}

	if (AttackEvent->IsAttackEventMonster(lpObj->m_Index) == true)
	{
		ATTACKEVENT_MONSTER_POS * lpPos = AttackEvent->GetMonsterPos(lpObj->Class);

		if (lpPos == NULL)
		{
			return FALSE;
		}

		tx -= lpObj->StartX;
		ty -= lpObj->StartY;

		int dis = sqrt(static_cast<float>(tx * tx + ty * ty));

		if (dis < 1 || dis > lpPos->m_Dis)
		{
			return FALSE;
		}

		return TRUE;
	}

	if (gEledoradoEvent.IsEledoradoMonster(lpObj->m_Index) == true)
	{
		ELEDORARO_MONSTER_POS * lpPos = gEledoradoEvent.GetMonsterPos(lpObj->Class);

		if (lpPos == NULL)
		{
			return FALSE;
		}

		tx -= lpObj->StartX;
		ty -= lpObj->StartY;

		int dis = sqrt(static_cast<float>(tx * tx + ty * ty));

		if (dis < 1 || dis > lpPos->m_Dis)
		{
			return FALSE;
		}

		return TRUE;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE && lpObj->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		if (lpObj->Class == 644 || lpObj->Class == 645)
		{
			if (g_ChaosCastleFinal.m_CCFMP[lpObj->m_PosNum].m_Dis < 1)
			{
				return FALSE;
			}

			tx -= lpObj->StartX;
			ty -= lpObj->StartY;

			int dis = sqrt(static_cast<float>(tx * tx + ty * ty));

			if (dis < 1 || dis > g_ChaosCastleFinal.m_CCFMP[lpObj->m_PosNum].m_Dis)
			{
				return FALSE;
			}

			return TRUE;
		}
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE && lpObj->MapNumber == MAP_INDEX_DEVILSQUARE_FINAL)
	{
		if (lpObj->m_PosNum == -1)
		{
			return FALSE;
		}

		if ( g_DevilSquareFinal.m_DSFMonsterRegenInfo[lpObj->m_PosNum].MonsterDis < 1 )
		{
			return FALSE;
		}

		tx -= lpObj->StartX;
		ty -= lpObj->StartY;

		int dis = sqrt(static_cast<float>(tx * tx + ty * ty));

		if ( dis > g_DevilSquareFinal.m_DSFMonsterRegenInfo[lpObj->m_PosNum].MonsterDis)
		{
			return FALSE;
		}
	}

	if (CC_MAP_RANGE(lpObj->MapNumber) == TRUE)
	{
		CHAOSCASTLE_MONSTER_POSITION * lpPos = g_ChaosCastle.GetMonsterPosData(lpObj->m_PosNum, lpObj->m_cChaosCastleIndex);

		if (lpPos == NULL)
		{
			return FALSE;
		}

		tx -= lpObj->StartX;
		ty -= lpObj->StartY;

		int dis = sqrt(static_cast<float>(tx * tx + ty * ty));
		
		if (dis < 1 || dis > lpPos->m_Dis)
		{
			return FALSE;
		}

		return TRUE;
	}

	if (DS_MAP_RANGE(lpObj->MapNumber) == TRUE)
	{
		DEVILSQUARE_MONSTER_POSITION * lpPos = g_DevilSquare.GetMonsterPosData(lpObj->m_PosNum, lpObj->m_bDevilSquareIndex);

		if (lpPos == NULL)
		{
			return FALSE;
		}

		tx -= lpObj->StartX;
		ty -= lpObj->StartY;

		int dis = sqrt(static_cast<float>(tx * tx + ty * ty));

		if (dis < 1 || dis > lpPos->m_Dis)
		{
			return FALSE;
		}

		return TRUE;
	}

	if (BC_MAP_RANGE(lpObj->MapNumber) == TRUE)
	{
		int iBridgeIndex = g_BloodCastle.GetBridgeIndex(lpObj->MapNumber);
		BLOODCASTLE_MONSTER_POSITION * lpPos = g_BloodCastle.GetMonsterPosData(lpObj->m_PosNum, iBridgeIndex, lpObj->Class);

		if (lpPos == NULL)
		{
			return FALSE;
		}

		tx -= lpObj->StartX;
		ty -= lpObj->StartY;

		int dis = sqrt(static_cast<float>(tx * tx + ty * ty));

		if (dis < 1 || dis > lpPos->m_Dis)
		{
			return FALSE;
		}

		return TRUE;
	}

	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE && (lpObj->MapNumber == MAP_INDEX_KANTURU_BOSS || lpObj->MapNumber == MAP_INDEX_HATCHERY) && lpObj->m_PosNum < 0)
	{
		if (lpObj->Class == 459 || lpObj->Class == 460 || lpObj->Class == 461 || lpObj->Class == 462)
		{
			return FALSE;
		}

		tx -= lpObj->StartX;
		ty -= lpObj->StartY;
		int dis = sqrt(static_cast<float>(tx*tx + ty*ty));

		if (dis > 30)
		{
			sLog->outBasic("[ KANTURU/RAKLION ][ Debug - m_PosNum ] Fail %s(Index:%d) X%d-Y%d -> X%d-Y%d(%d)",
				lpObj->Name, lpObj->Class, lpObj->StartX, lpObj->StartY, tx, ty, dis);
			return FALSE;
		}
	}

	if (gEventMSetBase.IsEventMonster(lpObj->m_Index) == true)
	{
		tx -= lpObj->StartX;
		ty -= lpObj->StartY;

		int dis = sqrt(static_cast<float>(tx * tx + ty * ty));

		if (dis > gEventMSetBase.GetDistance(lpObj->m_Index))
		{
			return FALSE;
		}

		return TRUE;
	}

	else
	{
		LPMONSTER_POSITION lpPos = &gMSetBase.m_Mp[lpObj->m_PosNum];

		if ( lpPos->m_Dis < 1 )
		{
			return FALSE;
		}

		tx -= lpObj->StartX;
		ty -= lpObj->StartY;

		int dis = sqrt(static_cast<float>(tx * tx + ty * ty));

		if ( dis > lpPos->m_Dis)
		{
			return FALSE;
		}	
	}

	return TRUE;
}






BOOL gObjMonsterGetTargetPos(LPGameObject &lpObj)
{
	int tpx;	// Target Player X
	int tpy;
	int mtx;	// Monster Target X
	int mty;
	int searchp = 0;
	int sn = 0;
	int searchcount = MAX_ROAD_PATH_TABLE/2-1;
	BYTE attr;
	BOOL result;
	CGameObject* lpTargetObj;

	if( lpObj->m_MoveRange == 0 && 
		(gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_STONE) == true ||
		 gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_STUN) == true ||
		 gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_SLEEP) == true ||
		 gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_FREEZE_2) == true ||
		 gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_EARTH_BINDS) == true))
	{
		return FALSE;
	}

	if (!MAX_MAP_RANGE(lpObj->MapNumber))
	{
		return FALSE;
	}

	int tObjNum = lpObj->TargetNumber;

	if ( ObjectMaxRange(tObjNum) == FALSE )
	{
		return FALSE;
	}

	lpTargetObj = &gGameObjects[tObjNum];

	if ( lpTargetObj->Teleport != 0 )
	{
		return FALSE;
	}

	int vpn = gObjMonsterViewportIsCharacter(lpObj);

	if ( vpn < 0 )
	{
		return FALSE;
	}
	
	tpx = lpTargetObj->X;
	mtx = tpx;
	tpy = lpTargetObj->Y;
	mty = tpy;
	int dis;

	if ( lpObj->m_AttackType >= 100 )
	{
		dis = lpObj->m_AttackRange + 2;
	}
	else
	{
		dis = lpObj->m_AttackRange;
	}

	if ( lpObj->X < mtx )
	{
		tpx -= dis;
	}

	if ( lpObj->X > mtx )
	{
		tpx += dis;
	}

	if ( lpObj->Y < mty )
	{
		tpy -= dis;
	}

	if ( lpObj->Y > mty )
	{
		tpy += dis;
	}

	searchp = GetPathPacketDirPos( (lpTargetObj->X - tpx), (lpTargetObj->Y - tpy) ) * 2;

	if ( MapC[lpObj->MapNumber].GetStandAttr(tpx, tpy) == 0 )
	{
		while ( searchcount-- )
		{
			mtx = lpTargetObj->X + RoadPathTable[searchp];
			mty = lpTargetObj->Y + RoadPathTable[1+searchp];
			attr = MapC[lpObj->MapNumber].GetAttr(mtx, mty);
			result = gObjMonsterMoveCheck(lpObj, mtx, mty);

			if ( lpObj->Class == 249 )
			{
				if ( (attr&2)!=2 && result == TRUE )
				{
					lpObj->MTX = mtx;
					lpObj->MTY = mty;
					return TRUE;
				}
			}
			else if ( (attr&1) != 1 && (attr&2) != 2 && (attr&4) != 4 && (attr&8) != 8 && result == TRUE )
			{
				lpObj->MTX = mtx;
				lpObj->MTY = mty;
				return TRUE;
			}

			searchp += 2;

			if ( searchp > MAX_ROAD_PATH_TABLE-1 )
			{
				searchp = 0;
			}
		}

		return FALSE;
	}

	attr = MapC[lpObj->MapNumber].GetAttr(tpx, tpy);
	result = gObjMonsterMoveCheck(lpObj, mtx, mty);

	if ( lpObj->Class == 249 )
	{
		if ( (attr&2)!=2 && result == TRUE )
		{
			lpObj->MTX = tpx;
			lpObj->MTY = tpy;
			return TRUE;
		}
	}
	else if ( (attr&1) != 1 && (attr&2) != 2 && (attr&4) != 4 && (attr&8) != 8 && result == TRUE )
	{
		lpObj->MTX = tpx;
		lpObj->MTY = tpy;
		return TRUE;
	}

	return FALSE;
}










BOOL gObjGetTargetPos(LPGameObject &lpObj, int sx, int sy, int & tx , int & ty)
{
	int tpx;	// Target Player X
	int tpy;
	int mtx;	// Monster Target X
	int mty;
	int searchp = 0;
	int sn = 0;
	int searchcount = MAX_ROAD_PATH_TABLE/2-1;
	BYTE attr;
	int dis;

	tpx = sx;
	mtx = tpx;
	tpy = sy;
	mty = tpy;

	if (!MAX_MAP_RANGE(lpObj->MapNumber))
	{
		return FALSE;
	}

	if ( lpObj->m_AttackType >= 100 )
	{
		dis = lpObj->m_AttackRange + 2;
	}
	else
	{
		dis = lpObj->m_AttackRange;
	}

	if ( lpObj->X < mtx )
	{
		tpx -= dis;
	}

	if ( lpObj->X > mtx )
	{
		tpx += dis;
	}

	if ( lpObj->Y < mty )
	{
		tpy -= dis;
	}

	if ( lpObj->Y > mty )
	{
		tpy += dis;
	}

	searchp = GetPathPacketDirPos( sx - tpx, sy - tpy ) * 2;

	if ( MapC[lpObj->MapNumber].GetStandAttr(tpx, tpy) == 0 )
	{
		while ( searchcount-- )
		{
			mtx = sx + RoadPathTable[searchp];
			mty = sy + RoadPathTable[1+searchp];
			attr = MapC[lpObj->MapNumber].GetAttr(mtx, mty);
			
			if ( (attr&1) != 1 && (attr&2) != 2 && (attr&4) != 4 && (attr&8) != 8 )
			{
				tx = mtx;
				ty = mty;
				return TRUE;
			}

			searchp += 2;

			if ( searchp > MAX_ROAD_PATH_TABLE-1 )
			{
				searchp = 0;
			}
		}

		return FALSE;
	}

	attr = MapC[lpObj->MapNumber].GetAttr(tpx, tpy);

	if ( lpObj->m_Attribute >= 100 )
	{
		if (  (attr&2) != 2 && (attr&4) != 4 && (attr&8) != 8 )
		{
			tx = tpx;
			ty = tpy;
			return TRUE;
		}
	}
	else if  ( (attr&1) != 1 && (attr&2) != 2 && (attr&4) != 4 && (attr&8) != 8 )
	{
		tx = tpx;
		ty = tpy;
		return TRUE;
	}

	return FALSE;
}











int gObjCallMonsterSetEnemy(LPGameObject &lpObj, int Target)
{
	if ( lpObj->Type != OBJ_USER )
	{
		return -1;
	}

	CGameObject* lpCallObj = &gGameObjects[lpObj->m_RecallMon];

	if ( lpCallObj->TargetNumber >= 0 )
	{
		return -1;
	}

	if ( lpCallObj->TargetNumber != Target )
	{
		lpCallObj->TargetNumber = Target;
		lpCallObj->m_ActState.EmotionCount = 30;
		lpCallObj->m_ActState.Emotion = 1;
	}

	return -1;
}









int gObjMonsterSearchEnemy(LPGameObject &lpObj, BYTE objtype)
{
	int n;
	int tx;
	int ty;
	int dis;
	int mindis = lpObj->m_ViewRange;
	int searchtarget = -1;
	int tObjNum;
	int count = 3;
	int t1 = objtype;
	int t2 = objtype;

	for (n=0;n<MaxViewportMonster;n++)
	{
		tObjNum = lpObj->VpPlayer2[n].number;

		if ( tObjNum >= 0 )
		{
			if ( (gGameObjects[tObjNum].Type == t1 || gGameObjects[tObjNum].Type == t2) && (gGameObjects[tObjNum].Live != FALSE) )
			{
				if ( ((gGameObjects[tObjNum].Class >= 100 && gGameObjects[tObjNum].Class < 110 ) || gGameObjects[tObjNum].Class == 523) || (gGameObjects[tObjNum].Type == OBJ_MONSTER && gGameObjects[tObjNum].m_RecallMon >= 0) )
				{

				}
				
				else
				{
					if (g_NewPVP.IsObserver(gGameObjects[tObjNum]) == TRUE)
					{
						continue;
					}

					if ( (gGameObjects[tObjNum].Authority &2) == 2 || (gGameObjects[tObjNum].Authority & 0x20) == 0x20)
					{
						if ( (gGameObjects[tObjNum].GameMaster & GM_MONSTERS_INVISIBLE) == GM_MONSTERS_INVISIBLE )
						{
							continue;
						}
					}

					
					if ( gGameObjects[tObjNum].Teleport == 0 )
					{
						tx = lpObj->X - gGameObjects[tObjNum].X;
						ty = lpObj->Y - gGameObjects[tObjNum].Y;
						dis = sqrt(static_cast<float>(tx * tx + ty * ty));
						lpObj->VpPlayer2[n].dis = dis;

						if ( dis < mindis )
						{
							searchtarget = tObjNum;
							mindis = dis;
						}
					}
				}
			}
		}
	}

	return searchtarget;
}






int gObjGuardSearchEnemy(LPGameObject &lpObj)
{
	int n;
	int tx;
	int ty;
	int dis;
	int mindis = lpObj->m_ViewRange;
	int searchtarget = -1;
	int tObjNum;
	BYTE attr;

	if ( g_GensSystem.IsMapBattleZone(lpObj->MapNumber) )
	{
		return -1;
	}

	for (n=0;n<MaxViewportMonster;n++)
	{
		tObjNum = lpObj->VpPlayer2[n].number;

		if ( tObjNum >= 0 && MAX_MAP_RANGE(gGameObjects[tObjNum].MapNumber))
		{
			BOOL bEnableAttack = FALSE; //Season 2.5 add-on

			if( gGameObjects[tObjNum].PartyNumber >= 0) //Season 2.5 add-on
			{
				if( gParty.GetPKPartyPenalty(gGameObjects[tObjNum].PartyNumber) > 4 )
				{
					bEnableAttack = TRUE;
				}
				
			}

			else if( gGameObjects[tObjNum].m_PK_Level > 4 ) 
			{
				bEnableAttack = TRUE;
			}
			
			if ( gGameObjects[tObjNum].Type == OBJ_USER )
			{
				if(bEnableAttack == TRUE) //Season 2.5 add-on
				{
					attr = MapC[gGameObjects[tObjNum].MapNumber].GetAttr(gGameObjects[tObjNum].X, gGameObjects[tObjNum].Y);

					if ( (attr&1) != 1 )
					{
						tx = lpObj->X - gGameObjects[tObjNum].X;
						ty = lpObj->Y - gGameObjects[tObjNum].Y;
						dis = sqrt(static_cast<float>(tx * tx + ty * ty));
						lpObj->VpPlayer2[n].dis = dis;

						if ( dis < mindis )
						{
							searchtarget = tObjNum;
							mindis = dis;
						}
					}
				}
			}
		}
	}

	return searchtarget;
}
	




void gObjMonsterStateProc(LPGameObject &lpObj, int aMsgCode, int aIndex, int aMsgSubCode)
{
	if ( lpObj->m_iMonsterBattleDelay > 0 )
	{
		return;
	}

	switch ( aMsgCode )
	{
		case 0:

			if ( lpObj->m_Attribute == 0 )
			{
				return;
			}
			
			if ( lpObj->Live == FALSE || lpObj->m_State != 2)
			{
				return;
			}

			if ( lpObj->m_ActState.Emotion == 0 )
			{
				lpObj->m_ActState.Emotion = 1;
				lpObj->m_ActState.EmotionCount = 10;
			}
			else if ( lpObj->m_ActState.Emotion == 1 )
			{
				lpObj->m_ActState.EmotionCount = 10;
			}

			if ( lpObj->Class >= 504 && lpObj->Class <= 511 && lpObj->MaxLife * 0.3 >= lpObj->Life && gObjCheckUsedBuffEffect(lpObj, 81) == false )
			{
				CMagicInf cMagicInf;
				memset(&cMagicInf, 0x00, sizeof(cMagicInf));

				cMagicInf.m_Skill = AT_SKILL_BERSERKER;
				gObjUseSkill.SkillBerserker(lpObj->m_Index, &cMagicInf, lpObj->m_Index);
				GSProtocol.GCUseMonsterSkillSend(lpObj, lpObj, 59);
				lpObj->m_ActState.Attack = 0;
				lpObj->m_ActState.Move = 0;
				lpObj->m_ActState.Emotion = 0;
				lpObj->NextActionTime = 1000;
			}

			if ( lpObj->Class >= 504 && lpObj->Class <= 521 )
			{
				g_ImperialGuardian.SetTargetMoveAllMonster(lpObj->m_nZoneIndex, aIndex);
			}

			if ( lpObj->m_ActState.Attack == 0 && lpObj->PathStartEnd == 0)
			{
				if ( ObjectMaxRange(aIndex) )
				{
					int map = lpObj->MapNumber;
					BYTE attr;
					int dis = gObjCalDistance(lpObj, &gGameObjects[aIndex]);
					int range;

					if ( lpObj->m_AttackType >= 100 )
					{
						range = lpObj->m_AttackRange +2;
					}
					else
					{
						range = lpObj->m_AttackRange;
					}

					if ( dis <= range )
					{
						if ( lpObj->m_RecallMon >= 0 )
						{
							if ( lpObj->m_RecallMon >= 0 )
							{
								if ( lpObj->Type == OBJ_MONSTER )
								{
									lpObj->TargetNumber = aIndex;
								}
							}
							else
							{
								lpObj->TargetNumber = aIndex;
							}
						}
						else if ( (rand()%100) < 90 )
						{
							if ( lpObj->m_RecallMon >= 0 )
							{
								if ( lpObj->Type == OBJ_MONSTER )
								{
									lpObj->TargetNumber = aIndex;
								}
							}
							else
							{
								lpObj->TargetNumber = aIndex;
							}
						}
					}
					else
					{
						if ( IMPERIAL_MAP_RANGE(lpObj->MapNumber) == TRUE &&
							lpObj->Class >= 504 && lpObj->Class <= 521 )
						{
							g_ImperialGuardian.SetTargetMoveAllMonster(lpObj->m_nZoneIndex, aIndex);
						}

						else
						{
							BYTE wall = 0;

							if (MAX_MAP_RANGE(map) == FALSE)
							{
								sLog->outError( "[ERROR] map is out of range %s %d", __FILE__, __LINE__);
								return;
							}

							wall = MapC[map].CheckWall2(lpObj->X, lpObj->Y, lpObj->X, lpObj->Y);
	
							if ( wall == 1 )
							{
								attr = MapC[map].GetAttr(lpObj->X, lpObj->Y);

								if ( (attr&1) != 1 )
								{
									if ( lpObj->TargetNumber < 0 )
									{
										lpObj->TargetNumber = aIndex;
									}
								}
							}
						}
					}

					if ( lpObj->m_bIsInMonsterHerd != false && lpObj->TargetNumber == aIndex )
					{
						if ( lpObj->m_lpMonsterHerd )
						{
							lpObj->m_lpMonsterHerd->BeenAttacked (lpObj, &gGameObjects[aIndex]);
						}
					}
				}
			}
			else
			{
				if ( (rand() % 2 )== 1 && lpObj->PathStartEnd == 0)
				{
					int IndexEnemy = lpObj->TargetNumber;
					if(ObjectMaxRange(IndexEnemy))
					{
						int EnemyMap = gGameObjects[IndexEnemy].MapNumber;

						int enemydis = gObjCalDistance(lpObj, &gGameObjects[aIndex]);
						int range;

						if ( lpObj->m_AttackType >= 100 )
						{
							range = lpObj->m_AttackRange + 2;
						}
						else
						{
							range = lpObj->m_AttackRange;
						}

						if ( enemydis <= range )
						{
							lpObj->m_ActState.Attack = 1;
							lpObj->TargetNumber = aIndex;
						}
						else
						{
							if ( MapC[EnemyMap].CheckWall2(lpObj->X, lpObj->Y, gGameObjects[IndexEnemy].X, gGameObjects[IndexEnemy].Y) == 1 )
							{
								lpObj->m_ActState.Attack = 1;
								lpObj->TargetNumber = aIndex;
							}
						}
					}
				}
				else
				{
					int MaxLife = lpObj->MaxLife;
					MaxLife >>= 1;

					if ( MaxLife > lpObj->Life )
					{
						if ( lpObj->m_Attribute != 2 )
						{
							lpObj->m_ActState.Emotion = 2;
							lpObj->m_ActState.EmotionCount = 2;
						}
					}
				}
			}
			break;

		case 1:
			if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
			{
				if (lpObj->m_btCsNpcType)
				{
					switch (lpObj->m_btCsNpcType)
					{
					case 1:	g_CastleSiege.DelNPC(lpObj->m_Index, lpObj->Class, lpObj->m_iCsNpcExistVal, TRUE);	break;
					case 2:	g_CastleSiege.DelNPC(lpObj->m_Index, lpObj->Class, lpObj->m_iCsNpcExistVal, FALSE); break;
					case 3:	g_CastleSiege.DelNPC(lpObj->m_Index, lpObj->Class, lpObj->m_iCsNpcExistVal, FALSE);	break;
					}

					if (lpObj->Class == 287 || lpObj->Class == 286)
						g_CsNPC_Mercenary.DeleteMercenary(lpObj->m_Index);

					if (lpObj->Class == 278)
						g_CsNPC_LifeStone.DeleteLifeStone(lpObj->m_Index);

					gObjDel(lpObj->m_Index);
				}
			}

			if ( KALIMA_MAP_RANGE(lpObj->MapNumber)  )
			{
				if ( lpObj->Class == 161 || lpObj->Class == 181 || lpObj->Class == 189 || lpObj->Class == 197 || lpObj->Class == 267 )
				{
					g_KalimaGate.CreateKalimaGate2(aIndex, lpObj->MapNumber, lpObj->X, lpObj->Y);
				}
			}

			if (lpObj->Type == OBJ_USER)
			{
				g_QuestExpProgMng.ChkUserQuestTypeMonsterKill(&gGameObjects[aIndex], lpObj);
				g_QuestExpProgMng.QuestMonsterItemDrop(0, &gGameObjects[aIndex], lpObj);
			}

			if ( lpObj->Class == 673 )
			{
				g_bActiveUrukTrap = false;
				g_bSilvesterEntranceBlock = false;
				g_i64LordSilvesterBlockTime = 0;

				UnBlockLordSilvesterEntrance();
				SendLordSilvesterBlockInfo(-1, 1);
			}

			if ( lpObj->Class >= 678 && lpObj->Class <= 680 )
			{
			
			}

			gObjMonsterDieGiveItem(lpObj, &gGameObjects[aIndex] );
			lpObj->NextActionTime = 500;

			if ( lpObj->m_RecallMon >= 0 )
			{
				gObjMonsterCallKill(lpObj->m_RecallMon);
			}

			if ( lpObj->Class == 409 || lpObj->Class == 410 || lpObj->Class == 411 || lpObj->Class == 412 )
			{
				g_QuestInfo.AddMonsterKillCount(lpObj);
			}

			if (  BC_MAP_RANGE(lpObj->MapNumber) != FALSE && lpObj->Type >= OBJ_MONSTER)
			{
				int iBridgeIndex = g_BloodCastle.GetBridgeIndex(lpObj->MapNumber); //season3 add-on
				g_BloodCastle.KillMonsterProc(iBridgeIndex, lpObj);
			}
			
			break;

		case 2:
			if ( lpObj->Live != FALSE )
			{
				if ( BC_MAP_RANGE(lpObj->MapNumber) == FALSE )
				{
					if ( lpObj->Class != 131 ||  ((  (lpObj->Class-132)<0)?FALSE:((lpObj->Class-132)>2)?FALSE:TRUE)==FALSE )
					{
						gObjBackSpring(lpObj, &gGameObjects[aIndex]);
					}
				}
			}

			break;

		case 3:
			lpObj->TargetNumber = -1;
			lpObj->LastAttackerID = -1;
			lpObj->m_ActState.Emotion = 0;
			lpObj->m_ActState.Attack = 0;
			lpObj->m_ActState.Move = 0;
			lpObj->NextActionTime = 1000;
			break;

		case 4:
			lpObj->m_ActState.Emotion = 3;
			lpObj->m_ActState.EmotionCount = 1;
			break;

		case 5:
			gObjMemFree(lpObj->m_Index);
			break;

		case 6:
			if ( lpObj->Live != FALSE )
			{
				gObjBackSpring2(lpObj, &gGameObjects[aIndex], 2);
			}

			break;

		case 7:
			if ( lpObj->Live != FALSE )
			{
				gObjBackSpring2(lpObj, &gGameObjects[aIndex], 3);
			}
			break;

		case 55:
			gObjAttack(lpObj, &gGameObjects[aIndex], NULL, FALSE, 0, 0, FALSE, 0, 0);
			break;

		case 56:
			{
				CGameObject* lpTargetObj = &gGameObjects[aIndex];

				if ( gObjCheckUsedBuffEffect(lpTargetObj, BUFFTYPE_POISON) == FALSE )
				{
					if ( retResistance(lpTargetObj, 1) == 0 )
					{
						lpTargetObj->lpAttackObj = lpObj;
						gObjAddBuffEffect(lpTargetObj, BUFFTYPE_POISON, EFFECTTYPE_POISON_DMG_TICK, 3, 0, 0, aMsgSubCode);
					}
				}
			}
			break;

		case 57:
			{
				CGameObject* lpTargetObj = &gGameObjects[aIndex];
				gObjBackSpring2(lpTargetObj, lpObj, aMsgSubCode);
			}
			break;

		case 50:
			{
				if ( lpObj->Class == 533 )
				{
					CMagicInf* lpMagic = gObjGetMagicSearch(lpObj, aMsgSubCode);
					gObjAttack(lpObj, &gGameObjects[aIndex], lpMagic, 0, 1, 0, 0, 0, 0);
				}

				else
				{
					if ( lpObj->Live == TRUE )
					{
						CMagicInf* lpMagic = gObjGetMagicSearch(lpObj, aMsgSubCode);
						gObjAttack(lpObj, &gGameObjects[aIndex], lpMagic, 0, 1, 0, 0, 0, 0);
					}
				}
			}
			break;

		case 62:
			{
				int nDamage = 20 * lpObj->MaxLife / 100;
				gObjAttack(lpObj, &gGameObjects[aIndex], 0, 0, 0, nDamage, 0, 0, 0);
			}
			break;
	}
}




void gObjMonsterProcess(LPGameObject &lpObj)
{
	gObjMsgProc(lpObj);

	if ( lpObj->Live == FALSE )
	{
		return;
	}

	if ( gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_SLEEP) == TRUE )
	{
		return;
	}

	if ( lpObj->m_iMonsterBattleDelay > 0 )
	{
		lpObj->m_iMonsterBattleDelay--;
	}

	if ( (GetTickCount() - lpObj->CurActionTime ) < (lpObj->NextActionTime + lpObj->DelayActionTime) )
	{
		return;
	}

	lpObj->CurActionTime = GetTickCount();

	if ( BC_MAP_RANGE(lpObj->MapNumber) != FALSE )
	{
		if ( lpObj->Class == 131|| ((lpObj->Class-132<0)?FALSE:(lpObj->Class-132>2)?FALSE:TRUE) != FALSE )
		{
			return;
		}
	}

	if ( (lpObj->Class >= 460 && lpObj->Class <= 462) && lpObj->MapNumber== 58 )
	{
		return;
	}

	if (lpObj->Class == 704 || lpObj->Class == 705)
	{
		return;
	}

	if ( ((lpObj->m_Attribute < 51)?FALSE:(lpObj->m_Attribute > 58)?FALSE:TRUE) != FALSE )
	{
		if ( lpObj->m_Attribute == 58 )
		{
			g_KalimaGate.KalimaGateAct2(lpObj->m_Index);
			return;
		}

		g_KalimaGate.KalimaGateAct(lpObj->m_Index);
		return;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (lpObj->Class == 283)
		{
			return;
		}
		else if (lpObj->Class == 288)
		{
			return;
		}
		else if (lpObj->Class == 278)
		{
			return;
		}
	}

	if ( lpObj->Class == 689 && g_bActiveUrukTrap == true )
	{
		gObjMonsterTrapAct(lpObj);
	}

	if ( (lpObj->Class >= 100 && lpObj->Class <= 110) || lpObj->Class == 523 )
	{
		gObjMonsterTrapAct(lpObj);
		
	}
	else if ( lpObj->Class == 200 )
	{
		int ground;
		int team = gCheckGoal(lpObj->X, lpObj->Y, ground);

		if ( team >= 0 )
		{
			gObjMonsterRegen(lpObj);
			gBattleSoccerScoreUpdate(ground, team);
			return;
		}
	}

	else if ( lpObj->Class >= 524 && lpObj->Class <= 528 )
	{
		return;
	}

	else if ( lpObj->Class >= 504 && lpObj->Class <= 521 )
	{
		g_ImperialGuardian.MonsterBaseAct(lpObj);
	}

	else if ( lpObj->Class >= 598 && lpObj->Class <= 602 )
	{
		return;
	}

	else if ( lpObj->Class >= 627 && lpObj->Class <= 631 )
	{
		return;
	}

	else 
	{
		if ( lpObj->Class == 287 || lpObj->Class == 286 )
		{
			g_CsNPC_Mercenary.MercenaryAct(lpObj->m_Index);
		}

		if ( lpObj->Type == OBJ_MONSTER && DG_MAP_RANGE(lpObj->MapNumber) )
		{
			g_DoppelGanger.CheckDoppelgangerMonsterPos(lpObj);
		}

		if ( lpObj->m_bIsInMonsterHerd != false )
		{
			if ( lpObj->m_lpMonsterHerd != NULL )
			{
				lpObj->m_lpMonsterHerd->MonsterBaseAct(lpObj);
			}
		}
		else
		{
			gObjMonsterBaseAct(lpObj);
		}
	}

	if ( lpObj->m_Attribute == 100 )
	{
		if ( lpObj->m_RecallMon >= 0 && lpObj->m_RecallMon < g_ConfigRead.server.GetObjectMax() )
		{
			CGameObject* lpCallMonObj;
			BOOL Success = FALSE;

			lpCallMonObj = &gGameObjects[lpObj->m_RecallMon];

			if ( lpObj->MapNumber != lpCallMonObj->MapNumber )
			{
				Success = TRUE;
			}
	
			if ( gObjCalDistance(lpCallMonObj, lpObj)> 14 )
			{
				Success = TRUE;
			}

			if ( Success == TRUE )
			{
				gObjTeleportMagicUse(lpObj->m_Index, (BYTE)lpCallMonObj->X+1, lpCallMonObj->Y);
				lpObj->MapNumber = lpCallMonObj->MapNumber;

				return;
			}
		}
	}

	if ( lpObj->m_ActState.Move != 0 )
	{
		if ( PathFindMoveMsgSend(lpObj ) == TRUE )
		{
			lpObj->m_ActState.Move  = (DWORD)0;
		}

		lpObj->m_ActState.Move = (DWORD)0;

		return;
	}

	if ( lpObj->m_ActState.Attack == 1 )
	{
		if (lpObj->Connected == PLAYER_PLAYING && lpObj->Type == OBJ_MONSTER && lpObj->Class == 459)
		{
			return;
		}

		if ( TMonsterSkillManager::CheckMonsterSkill(lpObj->Class) )
		{
			BOOL bEnableAttack = TRUE;
			int iTargetNumber = lpObj->TargetNumber;

			if (iTargetNumber < 0 )
				bEnableAttack = FALSE;

			else if ( gGameObjects[iTargetNumber].Live == FALSE || gGameObjects[iTargetNumber].Teleport != 0)
				bEnableAttack = FALSE;

			else if ( gGameObjects[iTargetNumber].Connected <= PLAYER_LOGGED || gGameObjects[iTargetNumber].CloseCount != -1 )
				bEnableAttack = FALSE;

			if ( bEnableAttack == FALSE )
			{
				lpObj->TargetNumber = -1;
				lpObj->m_ActState.Emotion = 0;
				lpObj->m_ActState.Attack = 0;
				lpObj->m_ActState.Move = 0;
				lpObj->NextActionTime = 1000;
				return;
			}
			
			if ( rand()%4 == 0 && lpObj->Class != 504 )
			{
				PMSG_ATTACK pAttackMsg;

				pAttackMsg.AttackAction = 120;
				pAttackMsg.DirDis = lpObj->Dir;
				pAttackMsg.NumberH = (BYTE)((DWORD)iTargetNumber >>(DWORD)8);
				pAttackMsg.NumberL = iTargetNumber &0xFF;

				GSProtocol.GCActionSend(lpObj, 120, lpObj->m_Index, iTargetNumber);
				gObjAttack(lpObj, &gGameObjects[iTargetNumber], NULL, FALSE, 0, 0, FALSE, 0, 0);
			}

			else if (lpObj->Class == 681 && g_EvoMonMng.IsEvolutedEvoMon(lpObj->m_Index) == false)
			{
				PMSG_ATTACK pAttackMsg;

				pAttackMsg.AttackAction = 120;
				pAttackMsg.DirDis = lpObj->Dir;
				pAttackMsg.NumberH = (BYTE)((DWORD)iTargetNumber >> (DWORD)8);
				pAttackMsg.NumberL = iTargetNumber & 0xFF;

				GSProtocol.GCActionSend(lpObj, 120, lpObj->m_Index, iTargetNumber);
				gObjAttack(lpObj, &gGameObjects[iTargetNumber], NULL, FALSE, 0, 0, FALSE, 0, 0);
			}

			else
			{
				TMonsterSkillManager::UseMonsterSkill(lpObj->m_Index, iTargetNumber, 0, -1, NULL);
			}

			lpObj->m_ActState.Attack = 0;
		}
		else
		{
			int AttackType = lpObj->m_AttackType;
			int lc6 = 0;

			if ( AttackType >= 100 )
			{
				if ( (rand()%5) == 0 )
				{
					AttackType -= 100;
					lc6 = TRUE;
				}
				else
				{
					AttackType = 0;
				}
			}

			if ( lpObj->Class >= 533 && lpObj->Class <= 539 )
			{
				lc6 = FALSE;
			}

			if ( lc6 != FALSE || lpObj->m_AttackType == 50)
			{
				int iTargetNumber = lpObj->TargetNumber;

				if (iTargetNumber >= 0 )
				{
					if ( gGameObjects[iTargetNumber].Connected > PLAYER_LOGGED && gGameObjects[iTargetNumber].CloseCount == -1 )
					{
						if ( gGameObjects[iTargetNumber].Live == FALSE )
						{
							lpObj->TargetNumber = -1;
							lpObj->m_ActState.Emotion = 0;
							lpObj->m_ActState.Attack = 0;
							lpObj->m_ActState.Move = 0;
							lpObj->NextActionTime = 1000;
						}
						else if ( gGameObjects[iTargetNumber].Teleport == 0 ) // if is not dead
						{
							gObjMonsterMagicAttack(lpObj, -1);

							if ( lpObj->Type == OBJ_MONSTER )
							{
								if ( lpObj->m_bIsInMonsterHerd != false )
								{
									if (lpObj->m_lpMonsterHerd != NULL )
									{
										lpObj->m_lpMonsterHerd->MonsterAttackAction(lpObj, &gGameObjects[iTargetNumber]);
									}
								}
							}
						}
					}
					else
					{
						lpObj->TargetNumber = -1;
						lpObj->m_ActState.Emotion = 0;
						lpObj->m_ActState.Attack = 0;
						lpObj->m_ActState.Move = 0;
						lpObj->NextActionTime = 1000;
					}
				}
			}
			else
			{
				int iTargetNumber = lpObj->TargetNumber;

				if (iTargetNumber >= 0 )
				{
					if ( gGameObjects[iTargetNumber].Connected > PLAYER_LOGGED &&  gGameObjects[iTargetNumber].CloseCount == -1 )
					{
						if ( gGameObjects[iTargetNumber].Live == FALSE )
						{
							lpObj->TargetNumber = -1;
							lpObj->m_ActState.Emotion = 0;
							lpObj->m_ActState.Attack = 0;
							lpObj->m_ActState.Move = 0;
							lpObj->NextActionTime = 1000;
						}
						else if ( gGameObjects[iTargetNumber].Teleport == 0 )
						{
							gObjMonsterAttack(lpObj, &gGameObjects[iTargetNumber]);

							if ( lpObj->Type == OBJ_MONSTER )
							{
								if ( lpObj->m_bIsInMonsterHerd != false )
								{
									if (lpObj->m_lpMonsterHerd != NULL )
									{
										lpObj->m_lpMonsterHerd->MonsterAttackAction(lpObj, &gGameObjects[iTargetNumber]);
									}
								}
							}
						}
						
					}
					else
					{
						lpObj->TargetNumber = -1;
						lpObj->m_ActState.Emotion = 0;
						lpObj->m_ActState.Attack = 0;
						lpObj->m_ActState.Move = 0;
						lpObj->NextActionTime = 1000;
					}
				}
			}

			lpObj->m_ActState.Attack = 0;
		}
	}
}

void gObjMonsterMagicAttack(LPGameObject &lpObj, int iMonsterClass)
{
	int tObjNum;
	int count = 0;
	PMSG_BEATTACK_COUNT pCount;
	PMSG_BEATTACK pAttack;
	BYTE AttackSendBuff[256];
	int ASBOfs = 0;
	PMSG_DURATION_MAGIC_RECV pDuration;

	pDuration.MagicNumberH = 0;
	pDuration.MagicNumberL = 0;
	pDuration.Dir = 0;
	pDuration.X = lpObj->X;
	pDuration.Y = lpObj->Y;

	GSProtocol.CGDurationMagicRecv((LPBYTE)&pDuration, lpObj->m_Index);

	pCount.h.c = 0xC1;

#if(ENABLE_SEASON12 == 1)
	pCount.h.headcode = HEADCODE_ATTACK_MULTI_TARGET;
#else
	pCount.h.headcode = 0xDF;
#endif

	pCount.h.size = 0;
	pCount.MagicNumberH = 0;
	pCount.MagicNumberL = 0;
	pCount.Count = 0;
	pCount.X = lpObj->X;
	pCount.Y = lpObj->Y;
	ASBOfs = sizeof(pCount);

	while ( true )
	{
		if ( lpObj->VpPlayer2[count].state )
		{
			if ( lpObj->VpPlayer2[count].type == OBJ_USER )
			{
				tObjNum = lpObj->VpPlayer2[count].number;

				if ( tObjNum >= 0 )
				{
					if ( lpObj->Class == 77 )	// Phoenix of Darkness
					{
						pAttack.NumberH = SET_NUMBERH(tObjNum);
						pAttack.NumberL = SET_NUMBERL(tObjNum);
						memcpy(&AttackSendBuff[ASBOfs], &pAttack, sizeof(pAttack));
						ASBOfs+= sizeof(pAttack);
						pCount.Count++;
					}
					else if ( lpObj->Class == 275 || gObjCalDistance(lpObj, &gGameObjects[tObjNum]) < 6 )
					{
						pAttack.NumberH = SET_NUMBERH(tObjNum);
						pAttack.NumberL = SET_NUMBERL(tObjNum);
						memcpy(&AttackSendBuff[ASBOfs], &pAttack, sizeof(pAttack));
						ASBOfs+= sizeof(pAttack);
						pCount.Count++;
					}
				}
			}
		}

		count++;

		if ( count > MaxViewportMonster-1 )
			break;
	}

	if ( pCount.Count > 0 )
	{
		pCount.h.size = ASBOfs;
		memcpy(AttackSendBuff, &pCount, sizeof(pCount));

		if ( lpObj->Class == 161 || lpObj->Class == 181 || lpObj->Class == 189 || lpObj->Class == 197 || lpObj->Class == 267 || lpObj->Class == 275 )
		{
			gObjMonsterBeattackRecv(AttackSendBuff, lpObj->m_Index);
		}
		else
		{
			GSProtocol.CGBeattackRecv(AttackSendBuff, lpObj->m_Index, TRUE);
		}
	}
}

void gObjUseMonsterSpecialAbillity(LPGameObject &lpMonsterObj)
{
	if ( lpMonsterObj->Class == 275 )	// Kundun
	{
		PMSG_MAGICATTACK pAttackMsg;

		pAttackMsg.MagicNumberH = 0;
		pAttackMsg.MagicNumberL = 1;
		pAttackMsg.NumberH = SET_NUMBERH(lpMonsterObj->TargetNumber);
		pAttackMsg.NumberL = SET_NUMBERL(lpMonsterObj->TargetNumber);
		pAttackMsg.Dis = 0;

		if ( (lpMonsterObj->MaxLife / 25.0f) > lpMonsterObj->Life  )
		{
			GSProtocol.CGMagicAttack((LPBYTE)&pAttackMsg, lpMonsterObj->m_Index);
			GSProtocol.CGMagicAttack((LPBYTE)&pAttackMsg, lpMonsterObj->m_Index);
		}
		else if ( (lpMonsterObj->MaxLife / 5.0f) > lpMonsterObj->Life  )
		{
			GSProtocol.CGMagicAttack((LPBYTE)&pAttackMsg, lpMonsterObj->m_Index);
		}
	}
}

void gObjMonsterBeattackRecv(BYTE * lpRecv, int aIndex)
{
	PMSG_BEATTACK_COUNT * lpCount = (PMSG_BEATTACK_COUNT *)lpRecv;
	int lOfs = sizeof(PMSG_BEATTACK_COUNT);
	int tNumber;
	WORD MagicNumber = MAKE_NUMBERW(lpCount->MagicNumberH, lpCount->MagicNumberL);
	CMagicInf * lpMagic = gObjGetMagic(&gGameObjects[aIndex], MagicNumber);

	if ( lpMagic == NULL )
	{
		sLog->outBasic("error-L3 %s %d", __FILE__, __LINE__);
		return;
	}

	PMSG_BEATTACK * lpMsg;
	int lOfs2 = lOfs;
	int pTargetNumber[128]={0};

	for (int i=0;i<lpCount->Count;i++)
	{
		lpMsg = (PMSG_BEATTACK *)&lpRecv[lOfs2];
		pTargetNumber[i] = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
		lOfs2+= sizeof(PMSG_BEATTACK);
	}
	
	for (int n=0;n<lpCount->Count;n++)
	{
		lpMsg = (PMSG_BEATTACK *)&lpRecv[lOfs];
		tNumber = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
		gObjAttack(&gGameObjects[aIndex], &gGameObjects[tNumber], lpMagic, 1, 1, 0 ,0, 0, 0);
		lOfs+= sizeof(PMSG_BEATTACK);
	}
}

static int skillevo = 100;

void gObjMonsterAttack(LPGameObject &lpObj, CGameObject* lpTargetObj)
{
	int AttackType = lpObj->m_AttackType;
	WORD MagicNumber;
	if ( AttackType >= 100 )
		AttackType = 0;

	if ( lpObj->m_iMonsterBattleDelay > 0 )
		return;
	
	if ( lpObj->Class == 144 || lpObj->Class == 174 || lpObj->Class == 182 || lpObj->Class == 190 ||
		lpObj->Class == 260 || lpObj->Class == 268 || lpObj->Class == 334 )
	{
		if ( rand()%2 )
		{
			PMSG_MAGICATTACK pAttackMsg;

			pAttackMsg.MagicNumberH = 0;
			pAttackMsg.MagicNumberL = 0;
			pAttackMsg.NumberH = SET_NUMBERH(lpObj->TargetNumber);
			pAttackMsg.NumberL = SET_NUMBERL(lpObj->TargetNumber);
			pAttackMsg.Dis = 0;

			GSProtocol.CGMagicAttack((LPBYTE)&pAttackMsg, lpObj->m_Index);
			return;
		}

		AttackType = 0;
	}

	if ( lpObj->Class == 161 || lpObj->Class == 181 || lpObj->Class == 189 ||
		lpObj->Class == 197 || lpObj->Class == 267 || lpObj->Class == 275  )
	{
		PMSG_MAGICATTACK pAttackMsg;
		MagicNumber = rand()%6+1;
		pAttackMsg.MagicNumberH = HIBYTE(MagicNumber);
		pAttackMsg.MagicNumberL = LOBYTE(MagicNumber);
		pAttackMsg.NumberH = SET_NUMBERH(lpObj->TargetNumber);
		pAttackMsg.NumberL = SET_NUMBERL(lpObj->TargetNumber);
		pAttackMsg.Dis = 0;

		GSProtocol.CGMagicAttack((LPBYTE)&pAttackMsg, lpObj->m_Index);

		if ( MagicNumber == 1 || MagicNumber == 2 || MagicNumber == 0 )
		{
			gObjUseMonsterSpecialAbillity(lpObj);	// Kundun
		}

		gObjMonsterMagicAttack(lpObj, lpObj->Class);
		return;
	}

	if ( lpObj->Class == 149 || lpObj->Class == 179 || lpObj->Class == 187 ||
		lpObj->Class == 195 || lpObj->Class == 265 || lpObj->Class == 273 || lpObj->Class == 335 )
	{
		PMSG_MAGICATTACK pAttackMsg;
		MagicNumber = rand()%2+1;
		pAttackMsg.MagicNumberH = HIBYTE(MagicNumber);
		pAttackMsg.MagicNumberL = LOBYTE(MagicNumber);
		pAttackMsg.NumberH = SET_NUMBERH(lpObj->TargetNumber);
		pAttackMsg.NumberL = SET_NUMBERL(lpObj->TargetNumber);
		pAttackMsg.Dis = 0;

		GSProtocol.CGMagicAttack((LPBYTE)&pAttackMsg, lpObj->m_Index);
		return;
	}
	
	if (     lpObj->Class == 145
          || lpObj->Class == 175
          || lpObj->Class == 183
          || lpObj->Class == 191
          || lpObj->Class == 261
          || lpObj->Class == 269
          || lpObj->Class == 146
          || lpObj->Class == 176
          || lpObj->Class == 184
          || lpObj->Class == 192
          || lpObj->Class == 262
          || lpObj->Class == 270
          || lpObj->Class == 147
          || lpObj->Class == 177
          || lpObj->Class == 185
          || lpObj->Class == 193
          || lpObj->Class == 263
          || lpObj->Class == 271
          || lpObj->Class == 148
          || lpObj->Class == 178
          || lpObj->Class == 186
          || lpObj->Class == 194
          || lpObj->Class == 264
          || lpObj->Class == 272
          || lpObj->Class == 160
          || lpObj->Class == 180
          || lpObj->Class == 188
          || lpObj->Class == 196
          || lpObj->Class == 266
		  || lpObj->Class == 274
		  || lpObj->Class == 332
		  || lpObj->Class == 333
		  || lpObj->Class == 336
	      || lpObj->Class == 337)
	{
		if ( (rand()%2) )
		{
			PMSG_MAGICATTACK pAttackMsg;
			pAttackMsg.MagicNumberH = 0;
			pAttackMsg.MagicNumberL = 1;
			pAttackMsg.NumberH = SET_NUMBERH(lpObj->TargetNumber);
			pAttackMsg.NumberL = SET_NUMBERL(lpObj->TargetNumber);
			pAttackMsg.Dis = 0;

			GSProtocol.CGMagicAttack((LPBYTE)&pAttackMsg, lpObj->m_Index);
			return;
		}
		AttackType = 0;
	}
	
	if (	 lpObj->Class == 89
          || lpObj->Class == 95
          || lpObj->Class == 112
          || lpObj->Class == 118
          || lpObj->Class == 124
          || lpObj->Class == 130
          || lpObj->Class == 143
		  || lpObj->Class == 433)
	{

		PMSG_MAGICATTACK pAttackMsg;
		MagicNumber = rand()%2+1;
		pAttackMsg.MagicNumberH = HIBYTE(MagicNumber);
		pAttackMsg.MagicNumberL = LOBYTE(MagicNumber);
		pAttackMsg.NumberH = SET_NUMBERH(lpObj->TargetNumber);
		pAttackMsg.NumberL = SET_NUMBERL(lpObj->TargetNumber);
		pAttackMsg.Dis = 0;

		GSProtocol.CGMagicAttack((LPBYTE)&pAttackMsg, lpObj->m_Index);
	}
	else if (	   lpObj->Class == 163
            || lpObj->Class == 165
            || lpObj->Class == 167
            || lpObj->Class == 169
            || lpObj->Class == 171
            || lpObj->Class == 173
			|| lpObj->Class == 427)
	{
		PMSG_MAGICATTACK pAttackMsg;

		pAttackMsg.MagicNumberH = 0;
		pAttackMsg.MagicNumberL = 1;
		pAttackMsg.NumberH = SET_NUMBERH(lpObj->TargetNumber);
		pAttackMsg.NumberL = SET_NUMBERL(lpObj->TargetNumber);
		pAttackMsg.Dis = 0;

		GSProtocol.CGMagicAttack((LPBYTE)&pAttackMsg, lpObj->m_Index);
	}
	else if ( lpObj->Class == 66 || lpObj->Class == 73 || lpObj->Class == 77 )
	{
		PMSG_MAGICATTACK pAttackMsg;

		pAttackMsg.MagicNumberH = 0;
		pAttackMsg.MagicNumberL = 1;
		pAttackMsg.NumberH = SET_NUMBERH(lpObj->TargetNumber);
		pAttackMsg.NumberL = SET_NUMBERL(lpObj->TargetNumber);
		pAttackMsg.Dis = 0;

		GSProtocol.CGMagicAttack((LPBYTE)&pAttackMsg, lpObj->m_Index);
	}

	else if ( lpObj->Class >= 533 && lpObj->Class <= 539 )
	{
		PMSG_DURATION_MAGIC_RECV pAttackMsg;

		pAttackMsg.Dir = lpObj->Dir;
		pAttackMsg.MagicNumberH = 0;
		pAttackMsg.MagicNumberL = 0;
		pAttackMsg.X = lpTargetObj->X;
		pAttackMsg.Y = lpTargetObj->Y;
		pAttackMsg.NumberH = SET_NUMBERH(lpObj->TargetNumber);
		pAttackMsg.NumberL = SET_NUMBERL(lpObj->TargetNumber);

		GSProtocol.CGDurationMagicRecv((LPBYTE)&pAttackMsg, lpObj->m_Index);
	}

	else if ( lpObj->Class == 561 )
	{
		int Skill = gObjMonsterSelectSkillForMedusa(lpObj);

		if(Skill == 1)
		{
			PMSG_MAGICATTACK pAttackMsg;

			pAttackMsg.MagicNumberH = SET_NUMBERH(Skill);
			pAttackMsg.MagicNumberL = SET_NUMBERL(Skill);
			pAttackMsg.NumberH = SET_NUMBERH(lpObj->TargetNumber);
			pAttackMsg.NumberL = SET_NUMBERL(lpObj->TargetNumber);

			GSProtocol.CGMagicAttack((LPBYTE)&pAttackMsg, lpObj->m_Index);
		}

		else
		{
			PMSG_DURATION_MAGIC_RECV pAttackMsg;

			pAttackMsg.MagicNumberH = SET_NUMBERH(Skill);
			pAttackMsg.MagicNumberL = SET_NUMBERL(Skill);
			pAttackMsg.X = lpTargetObj->X;
			pAttackMsg.Y = lpTargetObj->Y;
			pAttackMsg.Dis = 31;
			pAttackMsg.Dir = 31;
			pAttackMsg.NumberH = SET_NUMBERH(lpObj->TargetNumber);
			pAttackMsg.NumberL = SET_NUMBERL(lpObj->TargetNumber);

			GSProtocol.CGDurationMagicRecv((LPBYTE)&pAttackMsg, lpObj->m_Index);
		}
	}
	
	else
	{
		switch ( AttackType ) 
		{
			case 0:
				{
					PMSG_ATTACK pAttackMsg;

					pAttackMsg.AttackAction = 120;
					pAttackMsg.DirDis = lpObj->Dir;
					pAttackMsg.NumberH = SET_NUMBERH(lpObj->TargetNumber);
					pAttackMsg.NumberL = SET_NUMBERL(lpObj->TargetNumber);

					GSProtocol.CGAttack(&pAttackMsg, lpObj->m_Index);
				}
				break;
			
			default:
				{
					PMSG_MAGICATTACK pAttackMsg;
					pAttackMsg.MagicNumberH = 0;
					pAttackMsg.MagicNumberL = 0;
					pAttackMsg.NumberH = SET_NUMBERH(lpObj->TargetNumber);
					pAttackMsg.NumberL = SET_NUMBERL(lpObj->TargetNumber);
					pAttackMsg.Dis = 0;

					GSProtocol.CGMagicAttack((LPBYTE)&pAttackMsg, lpObj->m_Index);
				}
		}
	}
}

BOOL PathFindMoveMsgSend(LPGameObject &lpObj)
{
	if (gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_STONE) == true ||
		gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_STUN) == true ||
		gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_SLEEP) == true ||
		gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_FREEZE_2) == true ||
		gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_EARTH_BINDS) == true)
	{
		return FALSE;
	}

	if ( MAX_MAP_RANGE(lpObj->MapNumber) == FALSE ) return FALSE;

	PATH_t path;
	PMSG_MOVE pMove;
	unsigned char bPath[8];
	

	pMove.h.c = 0xC1;

#if(ENABLE_SEASON12 == 1)
	pMove.h.headcode = HEADCODE_MOVE;
#else
	pMove.h.headcode = 0xD4;
#endif
	pMove.h.size = sizeof(pMove);
	pMove.X = lpObj->X;
	pMove.Y = lpObj->Y;

	BOOL bPathFound = FALSE;

	if ( lpObj->Type == OBJ_MONSTER && lpObj->m_bIsInMonsterHerd != FALSE )
	{
		bPathFound = MapC[lpObj->MapNumber].PathFinding3(lpObj->X, lpObj->Y, lpObj->MTX, lpObj->MTY, &path);
	}
	else
	{
		bPathFound = MapC[lpObj->MapNumber].PathFinding2(lpObj->X, lpObj->Y, lpObj->MTX, lpObj->MTY, &path);
	}

	if ( bPathFound )
	{
		int tx;
		int ty;
		int sx;
		int sy;
		BYTE pos = 0;

		memset(bPath, 0, sizeof(bPath));
		bPath[0] = (lpObj->Dir *16) + ((path.PathNum-1)&0x0F);
		sx = lpObj->X;
		sy = lpObj->Y;
		lpObj->PathX[0] = lpObj->X;
		lpObj->PathY[0] = lpObj->Y;
		lpObj->PathDir[0] = lpObj->Dir;

		for (int n=1;n<path.PathNum;n++)
		{
			tx = path.PathX[n];
			ty = path.PathY[n];
			pos = GetPathPacketDirPos(tx-sx, ty-sy);
			lpObj->PathX[n] = tx;
			lpObj->PathY[n] = ty;
			lpObj->PathDir[n] = pos;
			sx = tx;
			sy = ty;

			if ( (n%2)==1 )
			{
				bPath[(n+1)/2] = (pos * 16);
			}
			else
			{
				bPath[(n+1)/2] |= pos; 
			}
		}

		lpObj->Dir = bPath[0] >> 4;
		lpObj->PathCount = bPath[0] & 0x0F;
		lpObj->PathCur = 1;
		lpObj->PathTime = GetTickCount();
		bPath[0] &= 0x0F;
		bPath[0] |= pos*16;
		memcpy(pMove.Path, bPath, sizeof(pMove.Path));

		if ( lpObj->PathCount > 0 )
		{
			MapC[lpObj->MapNumber].ClearStandAttr(lpObj->m_OldX, lpObj->m_OldY);
			MapC[lpObj->MapNumber].SetStandAttr(sx, sy);
			lpObj->m_OldX = sx;
			lpObj->m_OldY = sy;
		}

		GSProtocol.PMoveProc(&pMove, lpObj->m_Index);
		return TRUE;
	}

	return FALSE;
}


void gObjMonsterMoveAction(LPGameObject &lpObj)
{
	if( gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_STONE) == true || 
		gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_STUN) == true  ||
		gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_SLEEP) == true ||
		gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_FREEZE_2) == true ||
		gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_EARTH_BINDS) == true)
	{
		return;
	}

	if (!MAX_MAP_RANGE(lpObj->MapNumber))
	{
		return;
	}

	int maxmoverange = lpObj->m_MoveRange*2+1;
	int searchc=10;
	lpObj->NextActionTime = 1000;
	BYTE tpx;
	BYTE tpy;

	while ( searchc-- != 0 )
	{
		__try
		{
			tpx = (lpObj->X - lpObj->m_MoveRange) + (rand()%maxmoverange);
			tpy = (lpObj->Y - lpObj->m_MoveRange) + (rand()%maxmoverange);
		}
		__except(maxmoverange=1, 1)
		{

		}

		int mchk = gObjMonsterMoveCheck(lpObj, tpx, tpy);
		BYTE attr = MapC[lpObj->MapNumber].GetAttr(tpx, tpy);

		if ( (lpObj->Class ==249 || lpObj->Class == 247) && mchk )	// Guard
		{
			if ( (attr&2)!=2)
			{
				lpObj->MTX = tpx;
				lpObj->MTY = tpy;
				lpObj->m_ActState.Move = 1;
				lpObj->NextActionTime = 3000;
				return;
			}
		}
		else
		{
			if ( (attr&1)!=1 && (attr&2)!=2 && (attr&4)!=4 && (attr&8)!= 8 && mchk )
			{
				lpObj->TargetNumber = -1;
				lpObj->m_ActState.Attack = 0;
				lpObj->NextActionTime = 500;
				lpObj->m_ActState.Emotion = 0;
				lpObj->MTX = tpx;
				lpObj->MTY = tpy;
				lpObj->m_ActState.Move = 1;
				return;
			}
		}
	}
}



void gObjMonsterBaseAct(LPGameObject &lpObj)
{
	CGameObject* lpTargetObj = NULL;

	if ( lpObj->TargetNumber >= 0 )
		lpTargetObj = &gGameObjects[lpObj->TargetNumber];
	else
		lpObj->m_ActState.Emotion = 0;

	if ( gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_SLEEP) == TRUE )
		return;

	if ( lpObj->m_ActState.Emotion == 0 )
	{
		if ( lpObj->m_Attribute )
		{
			if ( lpObj->m_ActState.Attack )
			{
				lpObj->m_ActState.Attack = 0;
				lpObj->TargetNumber = -1;
				lpObj->NextActionTime = 500;
			}

			int actcode1 = rand()%2;

			if ( lpObj->m_Attribute == 100 )
				actcode1 = 1;
			
			if ( actcode1 == 0 )
			{
				lpObj->m_ActState.Rest = 1;
				lpObj->NextActionTime = 500;
			}

			if ( lpObj->Class != 673 || lpObj->VPCount2 >= 1 )
			{
				if ( lpObj->m_MoveRange > 0 && !gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_STONE) && !gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_STUN) 
					&& !gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_SLEEP) && !gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_FREEZE_2) && !gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_EARTH_BINDS) )
				{
					if ( lpObj->m_Attribute != 100 )
					{
						gObjMonsterMoveAction(lpObj);
					}

					else
					{
						int tx=0;
						int ty=0;

						if ( lpObj->m_RecallMon >= 0 )
						{
							if ( gGameObjects[lpObj->m_RecallMon].Connected > PLAYER_LOGGED )
							{
								CGameObject* lpRecallObj = &gGameObjects[lpObj->m_RecallMon];

								if ( lpRecallObj->m_Rest == FALSE )
								{
									if ( gObjGetTargetPos(lpObj, lpRecallObj->X, lpRecallObj->Y, tx, ty) == TRUE )
									{
										lpObj->MTX = tx;
										lpObj->MTY = ty;
										lpObj->m_ActState.Move = 1;
										lpObj->NextActionTime = 1000;
									}
								}
							}
						}
					}
				}
			}

			else
			{
				g_bActiveUrukTrap = false;
				float fMaxHP = lpObj->MaxLife;
				float fCurrentHP = lpObj->Life;
				int nAddLife = 10000;

				if ( lpObj->MaxLife > lpObj->Life )
				{
					lpObj->Life += nAddLife;

					if ( lpObj->MaxLife * 0.7 >= lpObj->Life )
					{
						if ( lpObj->MaxLife * 0.5 >= lpObj->Life )
						{
							if ( lpObj->MaxLife * 0.2 < lpObj->Life )
							{
								lpObj->m_nPhaseLordSilvester = 2;
							}
						}

						else
						{
							lpObj->m_nPhaseLordSilvester = 1;
						}
					}

					else
					{
						lpObj->m_nPhaseLordSilvester = 0;
					}

					if ( lpObj->MaxLife < lpObj->Life )
						lpObj->Life = lpObj->MaxLife;

				}

				if (g_bSilvesterEntranceBlock == true && (GetTickCount64() - g_i64LordSilvesterBlockTime >= 60000))
				{
					lpObj->Life = lpObj->MaxLife;

					g_bSilvesterEntranceBlock = false;
					g_i64LordSilvesterBlockTime = 0;

					UnBlockLordSilvesterEntrance();
					SendLordSilvesterBlockInfo(-1, 1);

				}

				if ( lpObj->MaxLife == lpObj->Life && gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_MONSTER_MAGIC_IMMUNE) == TRUE )
				{
					gObjRemoveBuffEffect(lpObj, BUFFTYPE_MONSTER_MAGIC_IMMUNE);

					for (int i = 0; i < 3; i++)
					{
						if ( lpObj->m_nRecallMonIndex[i] != -1 )
						{
							gObjDel(lpObj->m_nRecallMonIndex[i]);
							lpObj->m_nRecallMonIndex[i] = -1;
						}
					}
				}
			}

			if (lpObj->Class == 249 || lpObj->Class == 247 )	// Guard
			{
				if ( g_ConfigRead.data.common.IsGuardSay )
				{
					if ( !(rand()%g_ConfigRead.data.common.GuardTalkChance) )
					{
						if ( lpObj->m_PK_Count == 0 )
						{
							GSProtocol.ChatSend(lpObj,(char *)g_ConfigRead.data.common.GuardSayText.c_str());	// Need Translation
						}
					}
				}

				int iTargetNumber = gObjGuardSearchEnemy(lpObj);
				lpObj->TargetNumber = gObjGuardSearchEnemy(lpObj);

				if (iTargetNumber >= 0 )
				{
					if ( gGameObjects[iTargetNumber].Class >= 248 )
					{
						lpObj->TargetNumber = -1;
					}
				}
			}
			else 
			{
				if ( lpObj->m_Attribute == 100 )
				{
					int iTargetNumber = gObjMonsterSearchEnemy(lpObj, OBJ_MONSTER);

					if ( lpObj->Class >= 678 && lpObj->Class <= 680 )
					{
						iTargetNumber = gObjMonsterSearchEnemy(lpObj, OBJ_USER);
					}

					if ( IMPERIAL_MAP_RANGE(lpObj->MapNumber) == TRUE )
					{
						if ( lpObj->Live == FALSE || g_ImperialGuardian.IsAttackAbleMonster(iTargetNumber) == false )
						{
							lpObj->TargetNumber = -1;
						}
					}

					if (g_ArcaBattle.IsArcaBattleServer() == TRUE)
					{
						if (g_AcheronGuardianEvent.IsPlayStart() == false)
						{
							if (lpObj->m_RecallMon != -1 && ObjectMaxRange(iTargetNumber) == true)
							{
								if (g_ArcaBattle.IsEnableAttackObelisk(&gGameObjects[lpObj->m_RecallMon], gGameObjects[iTargetNumber].Class) == FALSE)
								{
									lpObj->TargetNumber = -1;
								}
							}
						}
					}
				}

				else
				{
					lpObj->TargetNumber = gObjMonsterSearchEnemy(lpObj, OBJ_USER);
				}
			}

			if ( lpObj->TargetNumber >= 0 )
			{
				lpObj->m_ActState.EmotionCount = 30;
				lpObj->m_ActState.Emotion = 1;
			}
		}
	}
	else if ( lpObj->m_ActState.Emotion == 1 )
	{
		if ( lpObj->Class == 673 )
		{
			g_bActiveUrukTrap = true;
			g_bSilvesterEntranceBlock = true;
			g_i64LordSilvesterBlockTime = GetTickCount64();

			BlockLordSilvesterEntrance();
			SendLordSilvesterBlockInfo(-1, 0);

		}

		if ( lpObj->m_ActState.EmotionCount > 0 )
		{
			lpObj->m_ActState.EmotionCount--;
		}
		else
		{
			lpObj->m_ActState.Emotion = 0;
		}

		if ( lpObj->TargetNumber >= 0 && lpObj->PathStartEnd == 0)
		{
			if ( BC_MAP_RANGE(lpObj->MapNumber) )
			{
				int iRAND_CHANGE_TARGET = rand()%10;

				if ( iRAND_CHANGE_TARGET == 3 )
					lpObj->LastAttackerID = -1;

				if ( iRAND_CHANGE_TARGET == 1 )
				{
					if ( lpObj->LastAttackerID != -1 && lpObj->LastAttackerID != lpObj->TargetNumber )
					{
						if ( gGameObjects[lpObj->LastAttackerID].Connected > PLAYER_LOGGED &&
							lpObj->MapNumber == gGameObjects[lpObj->LastAttackerID].MapNumber )
						{
							lpObj->TargetNumber = lpObj->LastAttackerID;
							lpTargetObj = &gGameObjects[lpObj->LastAttackerID];
						}
					}
				}
			}

			int dis = gObjCalDistance(lpObj, lpTargetObj);
			int attackrange;

			if ( lpObj->m_AttackType >= 100 )	// #warning Change this To Level
				attackrange = lpObj->m_AttackRange+2;
			else
				attackrange = lpObj->m_AttackRange;

			if ( dis <= attackrange )
			{
				int tuser = lpObj->TargetNumber;

				if (!ObjectMaxRange(tuser))
				{
					return;
				}

				int map = gGameObjects[tuser].MapNumber;
				BYTE attr;

				if ( MAX_MAP_RANGE(map) == FALSE )
				{
					sLog->outError( "[ERROR] MAX_MAP_RANGE (gObjMonster) == FALSE (%d)", map);
					return;
				}

				if ( MapC[map].CheckWall(lpObj->X, lpObj->Y, gGameObjects[tuser].X, gGameObjects[tuser].Y) == TRUE )
				{
					attr = MapC[map].GetAttr(gGameObjects[tuser].X, gGameObjects[tuser].Y);

					if ( (attr&1) != 1 )
					{
						lpObj->m_ActState.Attack = 1;
					}
					else
					{
						lpObj->TargetNumber = -1;
						lpObj->m_ActState.EmotionCount = 30;
						lpObj->m_ActState.Emotion = 1;
					}

					lpObj->Dir = GetPathPacketDirPos(lpTargetObj->X-lpObj->X, lpTargetObj->Y-lpObj->Y);
					lpObj->NextActionTime = lpObj->m_AttackSpeed;
				}
			}
			else
			{
				if ( gObjMonsterGetTargetPos(lpObj) == TRUE )
				{
					if ( MAX_MAP_RANGE(lpObj->MapNumber) == FALSE )
					{
						sLog->outError( "[ERROR] MAX_MAP_RANGE (gObjMonster) == FALSE (%d)", lpObj->MapNumber);
						return;
					}

					if ( MapC[lpObj->MapNumber].CheckWall(lpObj->X, lpObj->Y, lpObj->MTX, lpObj->MTY) == TRUE )
					{
						lpObj->m_ActState.Move = 1;
						lpObj->NextActionTime = 400;
						lpObj->Dir = GetPathPacketDirPos(lpTargetObj->X-lpObj->X, lpTargetObj->Y-lpObj->Y);
					}
					else
					{
						gObjMonsterMoveAction(lpObj);
						lpObj->m_ActState.Emotion = 3;
						lpObj->m_ActState.EmotionCount = 10;

					}
				}
				else
				{
					gObjMonsterMoveAction(lpObj);
				}
			}
		}
	}
	else if ( lpObj->m_ActState.Emotion == 2 )
	{
		if ( lpObj->m_ActState.EmotionCount > 0 )
		{
			lpObj->m_ActState.EmotionCount--;
		}
		else
		{
			lpObj->m_ActState.Emotion = 0;
		}

		lpObj->m_ActState.Move = 1;
		lpObj->NextActionTime = 800;

		if ( lpTargetObj )
		{
			int tdir = GetPathPacketDirPos(lpTargetObj->X-lpObj->X, lpTargetObj->Y-lpObj->Y)*2;
			lpObj->MTX += RoadPathTable[tdir] * (-3);
			lpObj->MTY += RoadPathTable[tdir+1] * (-3);
		}
	}
	else if ( lpObj->m_ActState.Emotion == 3 )
	{
		if ( lpObj->m_ActState.EmotionCount > 0 )
		{
			lpObj->m_ActState.EmotionCount--;
		}
		else
		{
			lpObj->m_ActState.Emotion = 0;
		}

		lpObj->m_ActState.Move = 0;
		lpObj->m_ActState.Attack = 0;
		lpObj->NextActionTime = 400;
	}
}



void gObjTrapAttackEnemySearchX(LPGameObject &lpObj, int count)
{
	lpObj->TargetNumber = -1;
	int tObjNum;
	int pos = lpObj->X;
	int y = lpObj->Y;

	for(int n =0;n<count;n++)
	{
		pos++;
		for(int i =0; i< MaxViewportMonster; i++)
		{
			tObjNum = lpObj->VpPlayer2[i].number;

			if(tObjNum >= 0)
			{
				if(gGameObjects[tObjNum].Type==OBJ_USER)
				{
					if(gGameObjects[tObjNum].Live)
					{
						if(y == gGameObjects[tObjNum].Y)
						{
							if(pos == gGameObjects[tObjNum].X)
							{
								if ( (gGameObjects[tObjNum].Authority &2) == 2 || (gGameObjects[tObjNum].Authority & 0x20) == 0x20)
								{
									if ( (gGameObjects[tObjNum].GameMaster & GM_MONSTERS_INVISIBLE) == GM_MONSTERS_INVISIBLE )
									{
										continue;
									}
								}

								lpObj->TargetNumber = tObjNum;
								return;
							}
						}
					}
				}
			}
		}
	}
}




void gObjTrapAttackEnemySearchY(LPGameObject &lpObj, int count)
{
	lpObj->TargetNumber = -1;
	int tObjNum;
	int pos = lpObj->Y;
	int x = lpObj->X;

	for(int n =0;n<count;n++)
	{
		pos--;
		for(int i =0; i< MaxViewportMonster; i++)
		{
			tObjNum = lpObj->VpPlayer2[i].number;

			if(tObjNum >= 0)
			{
				if(gGameObjects[tObjNum].Type==1)
				{
					if(gGameObjects[tObjNum].Live)
					{
						if(pos == gGameObjects[tObjNum].Y)
						{
							if(x == gGameObjects[tObjNum].X)
							{
								if ( (gGameObjects[tObjNum].Authority &2) == 2 || (gGameObjects[tObjNum].Authority & 0x20) == 0x20)
								{
									if ( (gGameObjects[tObjNum].GameMaster & GM_MONSTERS_INVISIBLE) == GM_MONSTERS_INVISIBLE )
									{
										continue;
									}
								}
									
								lpObj->TargetNumber = tObjNum;
								return;
							}
						}
					}
				}
			}
		}
	}
}



void gObjTrapAttackEnemySearch(LPGameObject &lpObj)
{
	int tObjNum;
	lpObj->TargetNumber = -1;

	for(int i = 0; i < MaxViewportMonster;i++)
	{
		tObjNum = lpObj->VpPlayer2[i].number;

		if(tObjNum >= 0)
		{
			if(gGameObjects[tObjNum].Type==1)
			{
				if(gGameObjects[tObjNum].Live)
				{
					if(lpObj->Y == gGameObjects[tObjNum].Y)
					{
						if(lpObj->X == gGameObjects[tObjNum].X)
						{
							if ( (gGameObjects[tObjNum].Authority &2) == 2 || (gGameObjects[tObjNum].Authority & 0x20) == 0x20)
							{
								if ( (gGameObjects[tObjNum].GameMaster & GM_MONSTERS_INVISIBLE) == GM_MONSTERS_INVISIBLE )
								{
									continue;
								}
							}
								
							lpObj->TargetNumber = tObjNum;
							return;
						}
					}
				}
			}
		}
	}
}

void gObjTrapAttackEnemySearchRange(LPGameObject &lpObj,int iRange)
{
	int tObjNum = -1;
	int iTargetingRate = 0;
	int iSuccessRate = 0;

	lpObj->TargetNumber = -1;

	if(lpObj->VPCount <= 0)
	{
		return;
	}

	iTargetingRate = 100 / lpObj->VPCount;

	for (int i = 0; i < MaxViewportMonster; i++)
	{
		tObjNum = lpObj->VpPlayer2[i].number;

		if(tObjNum >= 0)
		{
			if(gGameObjects[tObjNum].Type==1)
			{
				if(gGameObjects[tObjNum].Live)
				{
					if((lpObj->Y - iRange) <= gGameObjects[tObjNum].Y && (lpObj->Y + iRange) >= gGameObjects[tObjNum].Y)
					{
						if((lpObj->X - iRange) <= gGameObjects[tObjNum].X && (lpObj->X + iRange) >= gGameObjects[tObjNum].X)
						{
							if ( (gGameObjects[tObjNum].Authority &2) == 2 || (gGameObjects[tObjNum].Authority & 0x20) == 0x20)
							{
								if ( (gGameObjects[tObjNum].GameMaster & GM_MONSTERS_INVISIBLE) == GM_MONSTERS_INVISIBLE )
								{
									continue;
								}
							}
								
							lpObj->TargetNumber = tObjNum;
							iSuccessRate = rand()%100;

							if(iSuccessRate < iTargetingRate)
							{
								break;
							}						
						}
					}
				}
			}
		}
	}
}

void gObjMonsterTrapAct(LPGameObject &lpObj)
{
	if(lpObj->VPCount2 < 1)
	{
		return;
	}

	if(lpObj->m_AttackRange > 0)
	{
		if(lpObj->Dir == 3)
		{
			gObjTrapAttackEnemySearchX(lpObj,lpObj->m_AttackRange+1);
		}

		else if(lpObj->Dir == 1)
		{
			gObjTrapAttackEnemySearchY(lpObj,lpObj->m_AttackRange+1);
		}

		else if(lpObj->Dir == 8)
		{
			gObjTrapAttackEnemySearchRange(lpObj,lpObj->m_AttackRange);
		}

		if (lpObj->Class == 689)
		{
			gObjTrapAttackEnemySearchRange(lpObj, lpObj->m_AttackRange);
		}
	}
	else
	{
		gObjTrapAttackEnemySearch(lpObj);
	}

	if(lpObj->TargetNumber >= 0)
	{
		lpObj->m_ActState.Attack = 1;
		lpObj->NextActionTime = lpObj->m_AttackSpeed;
	}
	else
	{
		lpObj->NextActionTime = lpObj->m_MoveSpeed;
	}
}

BYTE NewOptionRand(int level, int OptCount)
{
	int NewOption = 0;
	int count = 0;

	while (true)
	{
		if (count >= OptCount || count >= 6)
		{
			break;
		}

		int value = 1 << (rand() % 6);

		if ((NewOption & value) == 0)
		{
			NewOption |= value;
			count++;
		}
	}

	return NewOption;
}

BYTE NewOptionRand2(int MinOptCount, int MaxOptCount)
{
	int ExcOption = 0;

	if (MinOptCount)
	{
		int ExcOptionCount = MinOptCount;
		// ----
		if (ExcOptionCount != MaxOptCount)
		{
			ExcOptionCount += rand() % (1 + (MaxOptCount - ExcOptionCount));
		}
		// ----
		while (ExcOptionCount != 0)
		{
			BYTE ExcValue = 1 << (rand() % 6);
			// ----
			if ((ExcOption & ExcValue) == 0)
			{
				ExcOption |= ExcValue;
				ExcOptionCount--;
			}
		}
	}

	return ExcOption;
}

void InventoryDropItem(LPGameObject &lpObj, int DropItem)
{
	return;	// there is NO MACRO NOR CODE
}

BOOL IsCanNotItemDtopInDevilSquare(int ItemType)
{
	int checkitemtype = ItemType  /MAX_SUBTYPE_ITEMS;
	int checkitemindex = ItemType % MAX_SUBTYPE_ITEMS;

	if ( checkitemtype >= 7 && checkitemtype <= 11 )
	{
		if (checkitemindex== 17 ||checkitemindex == 18||checkitemindex== 19 )
		{
			return FALSE;
		}
	}
	else if ( checkitemtype == 0 )
	{
		if (checkitemindex == 17 || checkitemindex == 18)
		{
			return FALSE;
		}
	}
	else if (checkitemtype == 12 )
	{
		if (checkitemindex== 12 || checkitemindex == 13 || checkitemindex == 14||
	checkitemindex == 16 || checkitemindex == 17 || checkitemindex == 18||
	checkitemindex == 19)
		{
			return FALSE;
		}
	}
	else if ( ItemType == ITEMGET(4,17) || ItemType ==ITEMGET(5,9)  )
	{
		return FALSE;
	}

	return TRUE;

}

void gObjRefillMonsterHP(LPGameObject &lpMonsterObj, int iRefillHPSec)
{
	int iMonsterHP = lpMonsterObj->Life;
	int iRefillHP = 0;

	if(lpMonsterObj->Live==0)
	{
		return;
	}

	if(lpMonsterObj->Class == 275)
	{
	CGameObject* lpObj;
		if(lpMonsterObj->iObjectSecTimer > giKundunRefillHPTime)
		{
			iRefillHP = rand() % ((giKundunRefillHP > 0)? giKundunRefillHP : 1) ;

			lpMonsterObj->iObjectSecTimer = 0;
		}
		else
		{
			lpMonsterObj->iObjectSecTimer++;
		}

		if(lpMonsterObj->iObjectSecTimer % giKundunHPLogSaveTime == 0)
		{
			if(gObjIsConnected(lpMonsterObj->LastAttackerID))
			{
				lpObj = &gGameObjects[lpMonsterObj->LastAttackerID];
			}
		}

		if(!g_CrywolfSync.GetOccupationState())
		{
			if(g_CrywolfSync.GetKundunHPRefillState()==1)
			{
				if(g_CrywolfSync.GetApplyBenefit() == TRUE)
				{
					iRefillHP = 0;
					iRefillHPSec = 0;
				}
			}
		}
	}

	iMonsterHP = iMonsterHP + iRefillHP + iRefillHPSec;

	if(lpMonsterObj->MaxLife < iMonsterHP)
	{
		lpMonsterObj->Life = lpMonsterObj->MaxLife;
	}
	else
	{
		lpMonsterObj->Life = iMonsterHP;
	}
}

int gObjMonsterSelectSkillForMedusa(LPGameObject &lpObj)
{
	int nTargetCnt = 0;
	int nSplashTargetCnt = 0;
	

	for (int i = 0; i < MAX_VIEWPORT; i++ )
	{
		int nObjNum = lpObj->VpPlayer2[i].number;

		if ( nObjNum < 0 )
			break;

		if ( gGameObjects[nObjNum].Type == OBJ_USER && gGameObjects[nObjNum].Live == TRUE )
			nTargetCnt++;

		int iTargetNumber = lpObj->TargetNumber;

		if ( ObjectMaxRange(iTargetNumber) )
		{
			if ( gObjCalDistance(&gGameObjects[iTargetNumber], &gGameObjects[nObjNum]) <= 3 )
			{
				if (iTargetNumber != nObjNum )
					nSplashTargetCnt++;
			}
		}
	}

	int nSkillNumber;

	if ( nTargetCnt >= 2 )
	{
		nSkillNumber = (rand() % 20) + 1;

		if ( nSkillNumber > 4 )
			nSkillNumber = 4;
	}

	else
	{
		nSkillNumber = (rand() % 10) + 1;
		
		if ( nSkillNumber > 4 )
			nSkillNumber = 1;
	}

	if ( nSplashTargetCnt > 0 && nSplashTargetCnt >= nTargetCnt / 2 && ( (rand() % 10) < 7) )
		nSkillNumber = 3;

	return nSkillNumber;
}

void gObjMonsterDieGiveItem(LPGameObject &lpObj, CGameObject* lpTargetObj)
{

	int store_count=0;
	int ExtDropPer=0;
	int DropItemNum=0;
	int type;
	int level;
	int level1;
	int level2;
	int level3;
	int x;
	int y;
	float dur=0;
	int Option1=0;
	int Option2=0;
	int Option3=0;
	unsigned char NOption=0;
	BOOL item_drop=0;
	int n;
	CItem * DropItem=NULL;

	if ( gObjCheckUsedBuffEffect(lpTargetObj, BUFFTYPE_HACKTOOL_PENALTY) == TRUE )
	{
		return;
	}

	if ( lpObj->MapNumber == MAP_INDEX_DEVILSQUARE_FINAL )
	{
		return;
	}

	g_KillCountMng.CheckMonsterKillCount(lpObj->Class, lpTargetObj);
	g_CashShop.AddGPMonster(lpObj);

	if (g_MonsterRegenSystem.MonsterKillCheck(lpTargetObj, lpObj))
	{
		return;
	}

	if ( g_QuestInfo.MonsterItemDrop(lpObj) ) //Third Quest from Season3 Monster Fix
		return;

	if( g_NewPVP.IsVulcanusMonster(lpObj->Class) )
	{
		if( g_NewPVP.DropItem(lpTargetObj, lpObj) )
		{
			return;
		}
	}

	if (lpObj->MapNumber == MAP_INDEX_ACHERON)
	{
		if (g_ArcaBattle.DropItem(lpTargetObj, lpObj))
		{
			return;
		}
	}

	if (g_BagManager.IsBag(lpTargetObj->m_Index, BAG_MONSTER, lpObj->Class, lpObj->m_Index) == true)
	{
		int iMaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

		if (iMaxHitUser == -1)
		{
			iMaxHitUser = lpTargetObj->m_Index;
		}

		if (g_BagManager.SearchAndUseBag(iMaxHitUser, BAG_MONSTER, lpObj->Class, lpObj->m_Index) == true)
		{
			return;
		}
	}

	if ( lpObj->Class >= 598 && lpObj->Class <= 602 )
	{
		return;
	}

	if (lpObj->Class == 681 || lpObj->Class == 690)
	{
		return;
	}

	if (lpObj->Class == 734 || lpObj->Class == 746) // Lord Ferea + Boss Nixie Lake
	{
		return;
	}

	if (lpObj->Class >= 627 && lpObj->Class <= 631)
	{
		int nMaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

		if (nMaxHitUser == -1)
		{
			nMaxHitUser = lpTargetObj->m_Index;
		}

		sLog->outBasic("[AcheronGuardianEvent] BossName: %s, CharacterName: %s", lpObj->Name, gGameObjects[nMaxHitUser].Name);
		g_AcheronGuardianEvent.DestroyObelisk(&gGameObjects[nMaxHitUser], lpObj->MapNumber, lpObj->X, lpObj->Y, nMaxHitUser);
		return;
	}

	if ( lpObj->Class >= 504 && lpObj->Class <= 511 )
	{
		return;
	}

	if ( DG_MAP_RANGE(lpObj->MapNumber) )
	{
		return;
	}

	if ( lpObj->Class == 523 )
	{
		return;
	}

	if ( lpObj->Class >= 523 && lpObj->Class <= 528 )
	{
		return;
	}

	if (g_MineSystem.IsTwinkle(lpObj->Class))
	{
		return;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (lpObj->m_btCsNpcType != 0)
		{
			return;
		}
	}

	if (lpObj->Class == 561) // Medusa 
	{
		g_BagManager.SearchAndUseBag(lpTargetObj->m_Index, BAG_EVENT, EVENTBAG_MEDUSA, lpTargetObj->m_Index);
		return;
	}

	if ( lpObj->Class == 275 ) // Kundun 
	{
		g_BagManager.SearchAndUseBag(lpTargetObj->m_Index, BAG_EVENT, EVENTBAG_KUNDUN, lpTargetObj->m_Index);
		return;
	}
	
	if ( lpObj->Class == 249 || lpTargetObj->Class == 249 ||	// Guard
		lpObj->Class == 247 || lpTargetObj->Class == 247 )	// Guard
	{
		return;
	}

	if ( lpObj->m_RecallMon >= 0 )
	{
		return;
	}
	
	if ( lpObj->Class == 131 )	// Castle Gate
	{
		return;
	}

	if (lpObj->Class == 283)
	{
		return;
	}

	if (lpObj->Class == 288)
	{
		return;
	}

	if (lpObj->Class == 278)
	{
		return;
	}

	if (lpObj->Class == 460 || lpObj->Class == 461 || lpObj->Class == 462) // Season 4.5 Addon
	{
		return;
	}
	
	if ( BC_STATUE_RANGE(lpObj->Class-132) )	// Blood Castle Statue
	{
		return;
	}
	
	if ( CC_MAP_RANGE(lpObj->MapNumber) )
	{
		g_ChaosCastle.SearchNDropMonsterItem(g_ChaosCastle.GetChaosCastleIndex(lpObj->MapNumber), lpObj->m_Index, lpTargetObj->m_Index);
		
		gObjDel(lpObj->m_Index);
		
		return;
	}

	if (lpObj->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL && g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		g_ChaosCastleFinal.SearchCCFNDropMonsterItem(lpObj->m_Index, lpTargetObj->m_Index);

		gObjDel(lpObj->m_Index);

		return;
	}
	
	if ( lpObj->m_bIsInMonsterHerd )
	{
		MonsterHerd * lpMH = lpObj->m_lpMonsterHerd;

		if ( lpMH )
		{
			if ( lpMH->MonsterHerdItemDrop(lpObj) )
			{
				return;
			}
		}
	}
	
	int itemrate = lpObj->m_ItemRate;
	int moneyrate = lpObj->m_MoneyRate;
	if (itemrate < 1)
		itemrate = 1;

//	if (itemrate > 100)
//		itemrate = 100;

	if (moneyrate < 1)
		moneyrate = 1;

	if (lpObj->Class == 43 ) // GoldenBudgeDragon
	{
		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenBudgeDragon, lpObj->m_Index, lpObj->MapNumber, lpObj->X, lpObj->Y);
		return;
	}
	if ( lpObj->Class == 53) // GoldenTitan
	{
		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenTitan, lpObj->m_Index, lpObj->MapNumber, lpObj->X, lpObj->Y);
		return;
	}
	if (lpObj->Class == 54) // GoldenSoldier
	{
		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenSoldier, lpObj->m_Index, lpObj->MapNumber, lpObj->X, lpObj->Y);
		return;
	}
	if ( lpObj->Class == 55 )	// Death King
	{

	}
	if (lpObj->Class == 78)	// GoldenGoblin
	{
		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenGoblin, lpObj->m_Index, lpObj->MapNumber, lpObj->X, lpObj->Y);
		return;
	}
	if (lpObj->Class == 502)	// GoldenRabbit
	{
		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenRabbit, lpObj->m_Index, lpObj->MapNumber, lpObj->X, lpObj->Y);
		return;
	}
	if (lpObj->Class == 493)	// GoldenDarkKnight
	{
		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenDarkKnight, lpObj->m_Index, lpObj->MapNumber, lpObj->X, lpObj->Y);
		return;
	}
	if (lpObj->Class == 79)	// GoldenDerkon
	{
		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenDerkon, lpObj->m_Index, lpObj->MapNumber, lpObj->X, lpObj->Y);
		return;
	}
	if (lpObj->Class == 501)	// GoldenGreatDragon
	{
		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenGreatDragon, lpObj->m_Index, lpObj->MapNumber, lpObj->X, lpObj->Y);
		return;
	}
	if (lpObj->Class == 80)	// GoldenLizard
	{

		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenLizard, lpObj->m_Index, lpObj->MapNumber, lpObj->X, lpObj->Y);
		return;
	}
	if (lpObj->Class == 81)	// GoldenVepar
	{

		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenVepar, lpObj->m_Index, lpObj->MapNumber, lpObj->X, lpObj->Y);
		return;
	}
	if (lpObj->Class == 494)	// GoldenDevil
	{

		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenDevil, lpObj->m_Index, lpObj->MapNumber, lpObj->X, lpObj->Y);
		return;
	}
	if (lpObj->Class == 82)	// GoldenTantalos
	{

		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenTantalos, lpObj->m_Index, lpObj->MapNumber, lpObj->X, lpObj->Y);
		return;
	}
	if (lpObj->Class == 83)	// GoldenWheel
	{

		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenWheel, lpObj->m_Index, lpObj->MapNumber, lpObj->X, lpObj->Y);
		return;
	}
	if (lpObj->Class == 495)	// GoldenStoneGolem
	{

		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenStoneGolem, lpObj->m_Index, lpObj->MapNumber, lpObj->X, lpObj->Y);
		return;
	}
	if (lpObj->Class == 496)	// GoldenCrust
	{

		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenCrust, lpObj->m_Index, lpObj->MapNumber, lpObj->X, lpObj->Y);
		return;
	}
	if (lpObj->Class == 497)	// GoldenSatyros
	{

		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenSatyros, lpObj->m_Index, lpObj->MapNumber, lpObj->X, lpObj->Y);
		return;
	}
	if (lpObj->Class == 498)	// GoldenTwinTail
	{

		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenTwinTail, lpObj->m_Index, lpObj->MapNumber, lpObj->X, lpObj->Y);
		return;
	}
	if (lpObj->Class == 499)	// GoldenIronKnight
	{

		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenIronKnight, lpObj->m_Index, lpObj->MapNumber, lpObj->X, lpObj->Y);
		return;
	}
	if (lpObj->Class == 500)	// GoldenNapin
	{

		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenNapin, lpObj->m_Index, lpObj->MapNumber, lpObj->X, lpObj->Y);
		return;
	}

	if (lpObj->Class == 652)
	{
		g_BagManager.SearchAndUseBag(lpTargetObj->m_Index, BAG_EVENT, EVENTBAG_GOLDCOLOSSUS_GOBLIN, lpTargetObj->m_Index);
		return;
	}

	if (lpObj->Class == 653)
	{
		g_BagManager.SearchAndUseBag(lpTargetObj->m_Index, BAG_EVENT, EVENTBAG_GOLDCOLOSSUS_TITAN, lpTargetObj->m_Index);
		return;
	}

	if (lpObj->Class == 654)
	{
		g_BagManager.SearchAndUseBag(lpTargetObj->m_Index, BAG_EVENT, EVENTBAG_GOLDCOLOSSUS_TANTALUS, lpTargetObj->m_Index);
		return;
	}

	if (lpObj->Class == 655)
	{
		g_BagManager.SearchAndUseBag(lpTargetObj->m_Index, BAG_EVENT, EVENTBAG_GOLDCOLOSSUS_EROHIM, lpTargetObj->m_Index);
		return;
	}

	if (lpObj->Class == 673)
	{
		g_BagManager.SearchAndUseBag(lpTargetObj->m_Index, BAG_EVENT, EVENTBAG_LORDSILVESTER, lpTargetObj->m_Index);
		return;
	}

	if (lpObj->Class == 656)
	{
		g_BagManager.SearchAndUseBag(lpTargetObj->m_Index, BAG_EVENT, EVENTBAG_GOLDCOLOSSUS_HELLMAINE, lpTargetObj->m_Index);

		int iWingCount = 0;
		int iWingExpireTime = 0;
		g_ConfigRead.m_ItemCalcLua.Generic_Call("GoldHellMaine_DropWing", ">ii", &iWingCount, &iWingExpireTime);

		if (iWingCount == 0)
		{
			return;
		}

		int iLootIndex = lpTargetObj->m_Index;

		for (int i = g_ConfigRead.server.GetObjectStartUserIndex(); i < g_ConfigRead.server.GetObjectMax(); i++)
		{
			if (gGameObjects[i].Connected == PLAYER_PLAYING && gGameObjects[i].m_SummonCharDBNum == lpObj->DBNumber)
			{
				iLootIndex = i;
				break;
			}
		}

		int iExOption = 0;
		BYTE btNewExcOption[5];
		memset(btNewExcOption, -1, sizeof(btNewExcOption));

		iExOption = g_ItemOptionTypeMng.WingExcOptionRand(GetItemKindA(ITEMGET(12, 267)), GetItemKindB(ITEMGET(12, 267)), btNewExcOption);

		for (int iNum = 0; iNum < iWingCount; iNum++)
		{
			ItemSerialCreateSend(lpTargetObj->m_Index, lpTargetObj->MapNumber, lpTargetObj->X, lpTargetObj->Y, ITEMGET(12, 267), 0, 0, 0, 0, 0, iLootIndex, iExOption, 0, iWingExpireTime, btNewExcOption, 0);
		}

		return;
	}

	if (lpObj->Class == 657)
	{
		g_BagManager.SearchAndUseBag(lpTargetObj->m_Index, BAG_EVENT, EVENTBAG_GOLDCOLOSSUS_KUNDUN, lpTargetObj->m_Index);

		int iWingCount = 0;
		int iWingExpireTime = 0;
		g_ConfigRead.m_ItemCalcLua.Generic_Call("GoldKundun_DropWing", ">ii", &iWingCount, &iWingExpireTime);

		if (iWingCount == 0)
		{
			return;
		}

		int iLootIndex = lpTargetObj->m_Index;

		for (int i = g_ConfigRead.server.GetObjectStartUserIndex(); i < g_ConfigRead.server.GetObjectMax(); i++)
		{
			if (gGameObjects[i].Connected == PLAYER_PLAYING && gGameObjects[i].m_SummonCharDBNum == lpObj->DBNumber)
			{
				iLootIndex = i;
				break;
			}
		}

		int iExOption = 0;
		BYTE btNewExcOption[5];
		memset(btNewExcOption, -1, sizeof(btNewExcOption));

		iExOption = g_ItemOptionTypeMng.WingExcOptionRand(GetItemKindA(ITEMGET(12, 267)), GetItemKindB(ITEMGET(12, 267)), btNewExcOption);

		for (int iNum = 0; iNum < iWingCount; iNum++)
		{
			ItemSerialCreateSend(lpTargetObj->m_Index, lpTargetObj->MapNumber, lpTargetObj->X, lpTargetObj->Y, ITEMGET(12, 267), 0, 0, 0, 0, 0, iLootIndex, iExOption, 0, iWingExpireTime, btNewExcOption, 0);
		}

		return;
	}

	if ( gEventMonsterItemDrop(lpObj, lpTargetObj) )
		return;

	if (g_CAppointItemDrop.AppointItemDrop(lpTargetObj, lpObj))
		return;

	if ( g_PentagramSystem.ElementDrop(lpObj, lpTargetObj))
		return;

	if (lpObj->MapNumber == MAP_INDEX_ACHERON || lpObj->MapNumber == MAP_INDEX_DEBENTER || lpObj->MapNumber == MAP_INDEX_NARS || lpObj->MapNumber == MAP_INDEX_URUK_MOUNTAIN)
	{
		BYTE MithrilSocketBonus = (lpObj->m_iPentagramMainAttribute == ELEMENT_NONE) ? (1 + rand() % 5) : lpObj->m_iPentagramMainAttribute;

		if (rand() % 10000 < g_PentagramSystem.m_iMithril_DropRate)
		{
			ItemSerialCreateSend(lpTargetObj->m_Index, lpTargetObj->MapNumber, lpTargetObj->X, lpTargetObj->Y, ITEMGET(12, 144), 0, 1, 0, 0, 0, lpTargetObj->m_Index, 0, 0, 0, 0, MithrilSocketBonus);
			return;
		}
	}

	if (SetItemDrop.DropItem(lpObj, lpTargetObj))
		return;

	if ( ItemDrop.DropItem(lpTargetObj, lpObj) )
		return;

#if (ENABLE_CUSTOM_ITEMDROPEX == 1)
	if (g_DropEx.DropItem(lpObj, lpTargetObj))
	{
		return;
	}
#endif

	if ( lpTargetObj->Level <= 20 )
	{
		if ( (rand()%10000) < 2000 )
		{
			int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);
			x = lpObj->X;
			y = lpObj->Y;
			dur = 1.0f;
			level = 0;
			type = ItemGetNumberMake(14, 0);	// Apple
			ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, level, dur,
				Option1, Option2, Option3, MaxHitUser, 0, 0, 0, 0, 0);
		}
	}

	int iExcDropRate = g_MaxStatsInfo.GetGeneralDrop.ExcItemDropPer + g_VipSystem.GetExcDropBonus(lpTargetObj) + g_BonusEvent.GetAddExcDrop() + g_GensSystem.GetBattleZoneExcDropBonus(lpTargetObj);

	if (lpTargetObj->Type == OBJ_USER)
	{
		if (rand() % 10000 < iExcDropRate)
		{
			ExtDropPer = TRUE;
		}
	}

	int ItemDropPer = g_MaxStatsInfo.GetGeneralDrop.ItemDropPer + g_VipSystem.GetDropBonus(lpTargetObj) + g_BonusEvent.GetAddDrop() + g_GensSystem.GetBattleZoneDropBonus(lpTargetObj);
	
	if(lpTargetObj->Type == OBJ_USER)
	{
		ItemDropPer += g_MaxStatsInfo.GetGeneralDrop.ItemDropPer * lpTargetObj->m_PlayerData->SetOpImproveItemDropRate / 100;
	}

	ItemDropPer += ItemDropPer * (gObjGetTotalValueOfEffect(lpTargetObj, EFFECTTYPE_ITEMDROPRATE) / 100.0f); // 7 - ADD_OPTION_DROP_RATE
	ItemDropPer += g_MapAttr.GetItemDropBonus(lpTargetObj->MapNumber);

	if(lpTargetObj->pInventory[10].m_Type == ITEMGET(13,134))
		ItemDropPer += 20;
	if(lpTargetObj->pInventory[11].m_Type == ITEMGET(13,134))
		ItemDropPer += 20;

	if ( ExtDropPer == TRUE )
	{
		DropItem = g_MonsterItemMng.GetItemEx(lpObj->Level - 25);

		if ( !DropItem )
			item_drop = FALSE;
		else
		{
			int foundChangeupitem=0;
			
			for ( int i=0;i<MAX_TYPE_PLAYER;i++)
			{
				if ( DropItem->m_RequireClass[i] > 1 )
				{
					foundChangeupitem = TRUE;
					break;
				}
			}

			if ( foundChangeupitem )
				ExtDropPer = rand()%100;

			if ( ExtDropPer )
				if ( (rand()%itemrate) < ItemDropPer )
					item_drop=TRUE;
		}
	}
	else
	{
		if ( (rand()%itemrate) < ItemDropPer )
		{
			if(lpTargetObj->MapNumber == MAP_INDEX_VULCAN && lpObj->MapNumber == MAP_INDEX_VULCAN
				&& gObjCheckUsedBuffEffect(lpTargetObj, BUFFTYPE_GLORYOFGLADIATOR))
			{
				DropItem = g_MonsterItemMng.GetItemEx(lpObj->Level + 25);
			}
			else
			{
				DropItem = g_MonsterItemMng.GetItemEx(lpObj->Level);
			}

			if (!DropItem)
				item_drop = FALSE;
			else
				item_drop = TRUE;
		}
	}

	if ( item_drop )
	{
		if ( !DropItem->IsItem() )
		{
			item_drop = FALSE;
		}

		if ( DS_MAP_RANGE(lpObj->MapNumber ) )
		{
			if ( !IsCanNotItemDtopInDevilSquare(DropItem->m_Type) )
			{
				if ( (rand()%10) )
				{
					item_drop = FALSE;
				}
			}
		}

		if (gSocketItemType.CheckSocketItemType(DropItem->m_Type) == true )
		{
			ExtDropPer = FALSE;

			if ( g_SocketOptionSystem.IsEnableDropSocketItemMap(lpObj->MapNumber) == false )
			{
				item_drop = FALSE;
			}
		}

		if (g_kItemSystemFor380.Is380Item(DropItem) == true)
		{
			if (g_ConfigRead.data.common.Item380DropMap != -1)
			{
				if (g_ConfigRead.data.common.Item380DropMap != lpObj->MapNumber)
				{
					item_drop = FALSE;
				}
			}
		}

		if (IsBloodAngelItem(DropItem->m_Type) == true)
		{
			if (ExtDropPer == TRUE)
			{
				if (lpObj->MapNumber != MAP_INDEX_NARS)
				{
					item_drop = FALSE;
				}
			}

			else
			{
				if (lpObj->MapNumber != MAP_INDEX_DEBENTER && lpObj->MapNumber != MAP_INDEX_DEBENTER_ARCA_WAR && lpObj->MapNumber != MAP_INDEX_URUK_MOUNTAIN)
				{
					item_drop = FALSE;
				}
			}
		}

		if ( DropItem->m_Type == ITEMGET(13,14) && lpObj->MapNumber != MAP_INDEX_ICARUS) // Loch Feather
			item_drop = FALSE;

		if ( g_CrywolfSync.GetOccupationState() == 1 && g_CrywolfSync.GetApplyPenalty() == TRUE)
		{
			// Jewels
			if ( DropItem->m_Type == ITEMGET(14,13) ||  DropItem->m_Type == ITEMGET(14,14) ||  DropItem->m_Type == ITEMGET(14,16) ||
				 DropItem->m_Type == ITEMGET(14,22) ||  DropItem->m_Type == ITEMGET(12,15) ||  DropItem->m_Type == ITEMGET(14,31) )
			{
				if ( (rand()%100) > g_CrywolfSync.GetGemDropPenaltiyRate() )
				{
					item_drop = FALSE;
				}

			}
		}
	}

	if (item_drop)
	{
		type = DropItem->m_Type;
		level = DropItem->m_Level;

		if ( ExtDropPer == TRUE )
		{
			dur = ItemGetDurability(DropItem->m_Type, 0, 1, 0);
		}
		else
		{
			dur = DropItem->m_Durability;
		}

		x = lpObj->X;
		y = lpObj->Y;

		int option1rand;
		int option2rand;
		int option3rand;
		int optionc;
		int NOptionCount = 1 + (rand() % 2);

		LPITEM_ATTRIBUTE p = &ItemAttribute[type];
		BYTE btSocketOption[5];
		memset(btSocketOption, 0, sizeof(btSocketOption));

		if ( ExtDropPer == TRUE )
		{
			option1rand = g_MaxStatsInfo.GetGeneralDrop.ExcSkillDropRate;
			option2rand = g_MaxStatsInfo.GetGeneralDrop.ExcLuckDropRate;
			option3rand=rand()%100;
			optionc=rand()%3;
			NOption = NewOptionRand2(g_MaxStatsInfo.GetExcellentDrop.MonsterMinOptionCount, g_MaxStatsInfo.GetExcellentDrop.MonsterMaxOptionCount); //NewOptionRand(lpObj->Level, NOptionCount);
			level = 0;
		}
		else
		{
			option1rand = g_MaxStatsInfo.GetGeneralDrop.SkillDropRate;
			option2rand = g_MaxStatsInfo.GetGeneralDrop.ItemLuckDrop;
			option3rand=rand()%100;
			optionc=rand()%3;
			NOption = 0;
		}

		if ( (rand()%100) < option1rand )
			Option1 = 1;

		if ( (rand()%100) < option2rand )
			Option2 = 1;

		switch ( optionc )
		{
			case 0:
				if ( option3rand < 4 )
					Option3=3;
				break;
			case 1:
				if ( option3rand < 8 )
					Option3=2;
				break;
			case 2:
				if ( option3rand < 12 )
					Option3=1;
				break;
		}

		if ( lpObj->Class == 43 ) // Golden Budge Dragon
		{
			Option1 = DropItem->m_Option1;
			Option2 = DropItem->m_Option2;
			Option3 = DropItem->m_Option3;
		}

		if ( type == ITEMGET(12,15) ||	// Chaos
			 type == ITEMGET(14,13) ||	// Bless
			 type == ITEMGET(14,14) ||	// Soul
			 type == ITEMGET(14,31) ||	// Guardian
			 type == ITEMGET(14,16) ||
			 type == ITEMGET(14,42) ||
			 type == ITEMGET(12,35) ||
			 (type >= ITEMGET(12,7) && type <= ITEMGET(12,14)) ||
			 (type >= ITEMGET(12,16) && type <= ITEMGET(12,24)) ||
			 (type >= ITEMGET(12,44) && type <= ITEMGET(12,48)) || (type >= ITEMGET(15,0) && type <= ITEMGET(16,0)) )
		{
			Option1=0;
			Option2=0;
			Option3=0;
			NOption=0;
		}

		int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

		if ( DropItem->m_serial )
		{
			ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, level, dur,
				Option1, Option2, Option3, MaxHitUser, NOption, 0, 0, btSocketOption, 0);
		}
		else
		{
			MapC[lpObj->MapNumber].MonsterItemDrop(type, level, dur, x, y, 
				Option1, Option2, Option3, NOption, 0, MaxHitUser, 0, 0, 0, -1, 0);
		}

	}
	else if (lpObj->MonsterMoneyDrop < 1)
	{
		return;
	}
	else if ((rand() % moneyrate) < 10)
	{
		int x = lpObj->X;
		int y = lpObj->Y;
		double money = lpObj->MonsterMoneyDrop * g_ConfigRead.data.common.gAddZenDiv;
		//
		if (lpTargetObj->m_PlayerData->DbClass == CLASS_WIZARDEVO || lpTargetObj->m_PlayerData->DbClass == CLASS_KNIGHTEVO || lpTargetObj->m_PlayerData->DbClass == CLASS_ELFEVO || lpTargetObj->m_PlayerData->DbClass == CLASS_MAGICEVO ||
			lpTargetObj->m_PlayerData->DbClass == CLASS_DARKLORDEVO || lpTargetObj->m_PlayerData->DbClass == CLASS_SUMMONEREVO || lpTargetObj->m_PlayerData->DbClass == CLASS_RAGEFIGHTEREVO || lpTargetObj->m_PlayerData->DbClass == CLASS_GROWLANCEREVO)
		{
			money += (money / 100.0f) * lpTargetObj->MonsterDieGetMoney;
			money = money * g_ConfigRead.data.common.gAddZenML;
		}
		else
		{
			money += (money / 100.0f) * lpTargetObj->MonsterDieGetMoney;
			money = money * g_ConfigRead.data.common.gAddZen;
		}

		if (lpTargetObj->Type == OBJ_USER)
		{
			if (lpTargetObj->m_PlayerData->m_MPSkillOpt.iMpsPlusZen > 0.0)
			{
				money += money * lpTargetObj->m_PlayerData->m_MPSkillOpt.iMpsPlusZen / 100.0;
			}
		}

		money += money * g_MAP_SETTINGS[lpTargetObj->MapNumber].drop_zen_increase / 100;

		if (money < 100.0f)
		{
			money = 100.0f;
		}

		if (money > 15000000.0f)
		{
			money = 15000000.0f;
		}

		if (DS_MAP_RANGE(lpObj->MapNumber))
		{
			int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

			if (MaxHitUser == -1)
			{
				return;
			}

			gGameObjects[MaxHitUser].m_nEventMoney += (__int64)money;
		}
		else
		{
			MapC[lpObj->MapNumber].MoneyItemDrop(money, x, y);
		}
	}

	if ( lpObj->MonsterMoneyDrop < 1 )
		return;

	if ( DS_MAP_RANGE(lpObj->MapNumber) )
		return;
	
	if ( (rand()%400) == 1 )
	{
		for ( n=0;n<4;n++)
		{
			int x = lpObj->X-2;
			int y = lpObj->Y-2;
			x+= rand()%3;
			y+= rand()%3;
			int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);
			if (MaxHitUser != -1 && gGameObjects[MaxHitUser].Type == OBJ_USER)
			{
				if (g_ConfigRead.data.common.joinmuDropItemUnderCharacter[gGameObjects[MaxHitUser].Class] == true)
				{
					MapC[lpObj->MapNumber].MoneyItemDrop(lpObj->MonsterMoneyDrop, gGameObjects[MaxHitUser].X, gGameObjects[MaxHitUser].Y);
				}
				else
				{
					MapC[lpObj->MapNumber].MoneyItemDrop(lpObj->MonsterMoneyDrop, x, y);
				}
			}
			else
			{
				MapC[lpObj->MapNumber].MoneyItemDrop(lpObj->MonsterMoneyDrop, x, y);
			}
		}
	}

	if ( gEvent1ItemDropTodayPercent > 0 )
	{
		if ( (rand()%gEvent1ItemDropTodayPercent) == 0 )
		{
			if ( gEvent1ItemDropTodayCount < gEvent1ItemDropTodayMax )
			{
				gEvent1ItemDropTodayCount++;
				type = ItemGetNumberMake(14, 12);	// Heart
				dur = 0;
				x = lpObj->X;
				y = lpObj->Y;
				level = 1;
				ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y,
					type, level, dur, 0, 0, 0, -1, 0, 0, 0, 0, 0);
			}
		}
	}

}


BOOL gEventMonsterItemDrop(LPGameObject &lpObj, CGameObject* lpTargetObj)
{
	int type;
	int level;
	int x;
	int y;
	float dur=0;
	int Option1=0;
	int Option2=0;
	int Option3=0;

	if ( g_bFenrirStuffItemDrop )
	{
		Option1=0;
		Option2=0;
		Option3=0;
		BOOL bDropStuff=FALSE;

		if ( lpObj->Level >= g_iFenrirStuff_01_DropLv_Min && lpObj->Level <= g_iFenrirStuff_01_DropLv_Max &&
			lpObj->MapNumber == g_iFenrirStuff_01_DropMap &&
			bDropStuff == FALSE )
		{
			if ( (rand()%10000) < g_iFenrirStuff_01_DropRate ) 
			{
				bDropStuff = TRUE;
				type = ItemGetNumberMake(13, 32);
			}
		}

		if ( lpObj->Level >= g_iFenrirStuff_02_DropLv_Min && lpObj->Level <= g_iFenrirStuff_02_DropLv_Max &&
			lpObj->MapNumber == g_iFenrirStuff_02_DropMap &&
			bDropStuff == FALSE )
		{
			if ( (rand()%10000) < g_iFenrirStuff_02_DropRate ) 
			{
				bDropStuff = TRUE;
				type = ItemGetNumberMake(13, 33);
			}
		}

		if ( lpObj->Level >= g_iFenrirStuff_03_DropLv_Min && lpObj->Level <= g_iFenrirStuff_03_DropLv_Max &&
			lpObj->MapNumber == g_iFenrirStuff_03_DropMap &&
			bDropStuff == FALSE )
		{
			if ( (rand()%10000) < g_iFenrirStuff_03_DropRate ) 
			{
				bDropStuff = TRUE;
				type = ItemGetNumberMake(13, 34);
			}
		}

		if ( bDropStuff )
		{
			level = 0;
			x = lpObj->X;
			y = lpObj->Y;
			dur = 1.0f;
			int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

			ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, level, dur,
				Option1, Option2, Option3, MaxHitUser, 0, 0, 0, 0, 0);

			return TRUE;
		}
	}

	int SocketSphereItemType = 0;
	SocketSphereItemType = g_SocketOptionSystem.GetSphereDropInfo(lpObj->Level);

	if (SocketSphereItemType != -1)// Season 4.5 addon
	{
		int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

		ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, lpObj->X, lpObj->Y, SocketSphereItemType, 0, 1, 0, 0, 0, MaxHitUser, 0, 0, 0, 0, 0);

		return TRUE;
	}

	int TetraCrystalItemType = 0;
	TetraCrystalItemType = g_SocketOptionSystem.GetTetraDropInfo(lpObj->Level);

	if (TetraCrystalItemType != -1)// Season 9 addon
	{
		int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

		ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, lpObj->X, lpObj->Y, TetraCrystalItemType, 0, 1, 0, 0, 0, MaxHitUser, 0, 0, 0, 0, 0);

		return TRUE;
	}

	if (g_ConfigRead.g_CompoundPotionDropOn == TRUE )
	{
		BOOL bDropStuff = FALSE;
		int iType = 0;

		if ( lpObj->Level >= g_ConfigRead.g_iCompoundPotionLv1DropLevel && (rand()%10000) <= g_ConfigRead.g_iCompoundPotionLv1DropRate &&
			bDropStuff == FALSE )
		{
			bDropStuff = TRUE;
			iType = ItemGetNumberMake(14, 38);
		}

		if ( bDropStuff == FALSE && lpObj->Level >= g_ConfigRead.g_iCompoundPotionLv2DropLevel && (rand()%10000) <= g_ConfigRead.g_iCompoundPotionLv2DropRate  )
		{
			bDropStuff = TRUE;
			iType = ItemGetNumberMake(14, 39);
		}
		

		if ( !bDropStuff )
		{
			if ( lpObj->Level >= g_ConfigRead.g_iCompoundPotionLv3DropLevel && (rand()%10000) <= g_ConfigRead.g_iCompoundPotionLv3DropRate )
			{
				bDropStuff = TRUE;
				iType = ItemGetNumberMake(14, 40);
			}
		}

		if ( bDropStuff == 1 )
		{
			int level = 0;
			int dur = 1;
			int iMaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

			ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, lpObj->X, lpObj->Y, iType, level, dur,
				0,0,0, iMaxHitUser, 0, 0, 0, 0, 0);

			return TRUE;
		}
	}

	if ( gIsItemDropRingOfTransform )
	{
		if ( (rand()%10000) < gItemDropRingOfTransform )
		{
			LPMONSTER_ATTRIBUTE lpattr = gMAttr.GetAttr(lpObj->Class);

			if ( lpattr )
			{
				int level = lpattr->m_Level;
				int callbead_level = 0;

				if ( level < 0 )
					level = 0;

				callbead_level = level / 10;

				if ( callbead_level > 0 )
					callbead_level--;

				if ( callbead_level > 5 )
					callbead_level = 5;

				dur = rand()%100+100.0f;
				x = lpObj->X;
				y = lpObj->Y;
				level = callbead_level;
				type = ItemGetNumberMake(13, 10);
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, level, dur,
					Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
	}

	if ( gFireCrackerEvent ) 
	{
		int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

		if ( MaxHitUser != -1 )
		{
			int partycount = gParty.GetPartyCount(gGameObjects[MaxHitUser].PartyNumber);

			if ( partycount > 0 )
			{
				LPMONSTER_ATTRIBUTE lpattr = gMAttr.GetAttr(lpObj->Class);

				if ( lpattr )
				{
					if ( lpattr->m_Level >= 17 )
					{
						if ( (rand()%10000) < gFireCrackerDropRate )
						{
							dur = 255.0f;
							x = lpObj->X;
							y = lpObj->Y;
							level = 2;
							type = ItemGetNumberMake(14, 11);

							ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, level, dur,
								Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

							return TRUE;
						}
					}
				}
			}
		}
	}

	if ( rand()%10000 < gSilverBoxDropRate )
	{
		type = ItemGetNumberMake(14,122);
		x = lpObj->X;
		y = lpObj->Y;
		int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

		ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, 0, 0,
			0, 0, 0, MaxHitUser, 0, 0, 0, 0, 0);

		return TRUE;
	}

	if ( rand()%10000 < gGoldBoxDropRate )
	{
		type = ItemGetNumberMake(14,121);
		x = lpObj->X;
		y = lpObj->Y;
		int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

		ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, 0, 0,
			0, 0, 0, MaxHitUser, 0, 0, 0, 0, 0);

		return TRUE;
	}

	if (g_CMuRummyMng.IsMuRummyEventOn() == true)
	{
		if (rand() % 10000 < g_CMuRummyMng.GetMuRummyEventItemDropRate())
		{
			type = ItemGetNumberMake(14, 215);
			x = lpObj->X;
			y = lpObj->Y;
			int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

			ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, 0, 1.0,
				0, 0, 0, MaxHitUser, 0, 0, 0, 0, 0);

			return TRUE;
		}
	}

	if ( gHeartOfLoveEvent )
	{
		if ( lpObj->MapNumber < 7 )
		{
			LPMONSTER_ATTRIBUTE lpattr = gMAttr.GetAttr(lpObj->Class);

			if ( lpattr )
			{
				if ( lpattr->m_Level >=17 )
				{
					if ( (rand()%10000) < gHeartOfLoveDropRate )
					{
						dur = 255.0f;
						x = lpObj->X;
						y = lpObj->Y;
						level = 3;
						type = ItemGetNumberMake(14, 11);
						int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

						ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, level, dur,
							Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

						return TRUE;
					}
				}
			}
		}
	}

	if ( gMedalEvent )
	{
		if ( lpObj->MapNumber == 1 || lpObj->MapNumber == 2 )	// Silver Medal
		{
			if ( (rand()%10000) < gSilverMedalDropRate )
			{
				dur = 255.0f;
				x = lpObj->X;
				y = lpObj->Y;
				level = 5;
				type = ItemGetNumberMake(14, 11);
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, level, dur,
					Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
		else if ( lpObj->MapNumber == 4 || lpObj->MapNumber == 7 || lpObj->MapNumber == 8 )	// Gold Medal
		{
			if ( (rand()%10000) < gGoldMedalDropRate )
			{
				dur = 255.0f;
				x = lpObj->X;
				y = lpObj->Y;
				level = 6;
				type = ItemGetNumberMake(14, 11);
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, level, dur,
					Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
	}

	if ( gEventChipEvent )
	{
		if ( (rand()%10000) < gBoxOfGoldDropRate )
		{
			dur = 255.0f;
			x = lpObj->X;
			y = lpObj->Y;
			level = 7;
			type = ItemGetNumberMake(14, 11);
			int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

			ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, level, dur,
				Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

			return TRUE;
		}
	}

	if ( (rand()%10000) < g_iKundunMarkDropRate )
	{
		Option1=0;
		Option2=0;
		Option3=0;
		dur = 1.0f;
		x = lpObj->X;
		y = lpObj->Y;
		level = 0;

		if ( lpObj->Level < 25 )
			level = 0;
		else if ( lpObj->Level < 47 )
			level = 1;
		else if ( lpObj->Level < 66 )
			level = 2;
		else if ( lpObj->Level < 78 )
			level = 3;
		else if ( lpObj->Level < 84 )
			level = 4;
		else if ( lpObj->Level < 92 )
			level = 5;
		else if (lpObj->Level < 114)
			level = 6;
		else if (lpObj->Level > 115 && lpObj->Level < g_ConfigRead.data.common.UserMaxLevel + 1)
			level = 7;

		if ( level == 0 )
			return FALSE;

		type = ItemGetNumberMake(14, 29);
		int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);
		ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, level, dur,
			Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

		return TRUE;
	}

	if ((rand() % 10000) < g_DoppelGanger.GetMirrorDimensionDropRate() && lpObj->Level >= g_DoppelGanger.GetMirrorDimensionDropLevel())
	{
		x = lpObj->X;
		y = lpObj->Y;
		dur = 1.0f;
		type = ItemGetNumberMake(14, 110);
		int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);
		ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, 0, dur,
			0, 0, 0, MaxHitUser, 0, 0, 0, 0, 0);

		return true;
	}

	if ((rand() % 10000) < g_ImperialGuardian.GetPaperScrapDropRate() && lpObj->Level >= g_ImperialGuardian.GetPaperScrapDropLevel())
	{
		x = lpObj->X;
		y = lpObj->Y;
		dur = 1.0f;
		type = ItemGetNumberMake(14, 101);
		int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);
		ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, 0, dur,
			0, 0, 0, MaxHitUser, 0, 0, 0, 0, 0);

		return true;
	}

	if ( (rand()%10000) < g_iMarkOfTheLord )
	{
		Option1=0;
		Option2=0;
		Option3=0;
		level = 3;
		dur = 1.0f;
		x = lpObj->X;
		y = lpObj->Y;
		type = ItemGetNumberMake(14, 21);
		int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

		ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, level, dur,
			Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

		return TRUE;
	}

	if ( (rand()%10000) < g_iLuckyCoin )
	{
		Option1=0;
		Option2=0;
		Option3=0;
		level = 0;
		dur = 1.0f;
		x = lpObj->X;
		y = lpObj->Y;
		type = ItemGetNumberMake(14, 100);
		int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

		ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, level, dur,
			Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

		return TRUE;
	}

	if ( gIsDropDarkLordItem )
	{
		if ( lpObj->Level >= gSleeveOfLordDropLevel )
		{
			if ( (rand()%10000) < gSleeveOfLordDropRate)
			{
				type = ItemGetNumberMake(13,14);
				level = 1;
				x = lpObj->X;
				y = lpObj->Y;
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, level, dur,
					Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}

		if ( lpObj->Level >= gSoulOfDarkHorseropLevel )
		{
			if ( (rand()%10000) < gSoulOfDarkHorseDropRate)
			{
				type = ItemGetNumberMake(13,31);
				level = 0;
				x = lpObj->X;
				y = lpObj->Y;
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, level, dur,
					Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}

		if ( lpObj->Level >= gSoulOfDarkSpiritDropLevel )
		{
			if ( (rand()%10000) < gSoulOfDarkSpiritDropRate )
			{
				type = ItemGetNumberMake(13,31);
				level = 1;
				x = lpObj->X;
				y = lpObj->Y;
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, level, dur,
					Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
	}

	if ( gIsDropGemOfDefend )
	{
		if ( lpObj->MapNumber == MAP_INDEX_KALIMA7 || lpObj->MapNumber == MAP_INDEX_CASTLEHUNTZONE )
		{
			if ( lpObj->Level >= gGemOfDefendDropLevel )
			{
				if ( (rand()%10000)< gGemOfDefendDropRate )
				{
					type = ItemGetNumberMake(14,31);
					level = 0;
					x = lpObj->X;
					y = lpObj->Y;
					int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

					ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, level, dur,
						Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

					return TRUE;
				}
			}
		}
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (gIsDropSetItemInCastleHuntZone != 0)	//Land of Trials
		{
			if (lpObj->MapNumber == MAP_INDEX_CASTLEHUNTZONE)
			{
				if (lpObj->Level >= gSetItemInCastleHuntZoneDropLevel) //Good
				{
					if ((rand() % 10000) < gSetItemInCastleHuntZoneDropRate)
					{
						int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

						if (ObjectMaxRange(MaxHitUser))
						{
							sLog->outBasic("[Castle HuntZone] Drop SetItem [%s][%s] ", gGameObjects[MaxHitUser].AccountID, gGameObjects[MaxHitUser].Name);
							::MakeRewardSetItem(MaxHitUser, lpObj->X, lpObj->Y, 0, lpObj->MapNumber);
							return TRUE;
						}
					}
				}
			}
		}
	}

	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		if (lpObj->MapNumber == MAP_INDEX_KANTURU2)
		{
			if ((rand() % 10000) < g_Kanturu.GetMoonStoneDropRate())
			{
				type = ItemGetNumberMake(13, 38);	// Moon Stone
				level = 0;
				x = lpObj->X;
				y = lpObj->Y;
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, level, dur, Option1, Option2, Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}

		if (lpObj->MapNumber == MAP_INDEX_KANTURU1 || lpObj->MapNumber == MAP_INDEX_KANTURU2 || lpObj->MapNumber == MAP_INDEX_KANTURU_BOSS)
		{
			if ((rand() % 10000) < g_Kanturu.GetJewelOfHarmonyDropRate())
			{
				type = ItemGetNumberMake(14, 41);
				level = 0;
				x = lpObj->X;
				y = lpObj->Y;
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, level, dur, Option1, Option2, Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
	}

	if ( lpObj->MapNumber == MAP_INDEX_AIDA || lpObj->MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE)
	{
		bool bDropMysteriousBead = false;

		switch ( lpObj->Class )
		{
			case 304:	case 305:	case 306:
			case 307:	case 308:	case 309:
				if ( (rand()%10000) < g_iMysteriousBeadDropRate1 )
					bDropMysteriousBead = true;
				break;

			case 310:	case 311:	case 312:	case 313:
				if ( (rand()%10000) < g_iMysteriousBeadDropRate2 )
					bDropMysteriousBead = true;
				break;
		}

		if ( bDropMysteriousBead == true )
		{
			type = ItemGetNumberMake(12,26);
			level = 0;
			x = lpObj->X;
			y = lpObj->Y;
			int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

			ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, level, dur,
				Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

			return TRUE;
		}
	}

	if ( g_DevilSquare.IsEventEnable() == true )
	{
		if (!DS_MAP_RANGE(lpObj->MapNumber))
		{
			Option1 = 0;
			Option2 = 0;
			Option3 = 0;

			if ((rand() % 10000) < g_DevilSquare.GetEyeDropRate())
			{
				dur = 1.0f;
				x = lpObj->X;
				y = lpObj->Y;

				if (lpObj->Level < 36)
					level = 1;
				else if (lpObj->Level < 47)
					level = 2;
				else if (lpObj->Level < 60)
					level = 3;
				else if (lpObj->Level < 70)
					level = 4;
				else if (lpObj->Level < 80)
					level = 5;
				else if (lpObj->Level < 90)
					level = 6;
				else
					level = 7;

				type = ItemGetNumberMake(14, 17);

				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, level, dur,
					Option1, Option2, Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}

			if ((rand() % 10000) < g_DevilSquare.GetKeyDropRate())
			{
				dur = 1.0f;
				x = lpObj->X;
				y = lpObj->Y;

				if (lpObj->Level < 36)
					level = 1;
				else if (lpObj->Level < 47)
					level = 2;
				else if (lpObj->Level < 60)
					level = 3;
				else if (lpObj->Level < 70)
					level = 4;
				else if (lpObj->Level < 80)
					level = 5;
				else if (lpObj->Level < 90)
					level = 6;
				else
					level = 7;

				type = ItemGetNumberMake(14, 18);

				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, level, dur,
					Option1, Option2, Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
	}

	if ( g_BloodCastle.IsEventEnable() == true )
	{
		if (!BC_MAP_RANGE(lpObj->MapNumber))
		{
			Option1 = 0;
			Option2 = 0;
			Option3 = 0;

			if ((rand() % 1000) < g_ConfigRead.GetArchangelScrollDropRate)
			{
				dur = 1.0f;
				x = lpObj->X;
				y = lpObj->Y;

				if (lpObj->Level < 36)
					level = 1;
				else if (lpObj->Level < 47)
					level = 2;
				else if (lpObj->Level < 60)
					level = 3;
				else if (lpObj->Level < 70)
					level = 4;
				else if (lpObj->Level < 80)
					level = 5;
				else if (lpObj->Level < 90)
					level = 6;
				else
					level = 7;

				type = ItemGetNumberMake(13, 16);

				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, level, dur,
					Option1, Option2, Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}

			if ((rand() % 10000) < g_ConfigRead.GetBloodBoneDropRate)
			{
				dur = 1.0f;
				x = lpObj->X;
				y = lpObj->Y;

				if (lpObj->Level < 36)
					level = 1;
				else if (lpObj->Level < 47)
					level = 2;
				else if (lpObj->Level < 60)
					level = 3;
				else if (lpObj->Level < 70)
					level = 4;
				else if (lpObj->Level < 80)
					level = 5;
				else if (lpObj->Level < 90)
					level = 6;
				else
					level = 7;

				type = ItemGetNumberMake(13, 17);

				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, level, dur,
					Option1, Option2, Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
	}

	if ( g_bRibbonBoxEvent )
	{
		if ( lpObj->Level >= g_iRedRibbonBoxDropLevelMin && lpObj->Level <= g_iRedRibbonBoxDropLevelMax )
		{
			if ( (rand()%10000) < g_iRedRibbonBoxDropRate )
			{
				type = ItemGetNumberMake(12,32);
				level = 0;
				x = lpTargetObj->X;
				y = lpTargetObj->Y;
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemSerialCreateSend(lpTargetObj->m_Index, lpTargetObj->MapNumber, x, y, type, level, dur,
					Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
		else if ( lpObj->Level >= g_iGreenRibbonBoxDropLevelMin && lpObj->Level <= g_iGreenRibbonBoxDropLevelMax )
		{
			if ( (rand()%10000) < g_iGreenRibbonBoxDropRate )
			{
				type = ItemGetNumberMake(12,33);
				level = 0;
				x = lpTargetObj->X;
				y = lpTargetObj->Y;
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemSerialCreateSend(lpTargetObj->m_Index, lpTargetObj->MapNumber, x, y, type, level, dur,
					Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
		else if ( lpObj->Level >= g_iBlueRibbonBoxDropLevelMin && lpObj->Level <= g_iBlueRibbonBoxDropLevelMax )
		{
			if ( (rand()%10000) < g_iBlueRibbonBoxDropRate )
			{
				type = ItemGetNumberMake(12,34);
				level = 0;
				x = lpTargetObj->X;
				y = lpTargetObj->Y;
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemSerialCreateSend(lpTargetObj->m_Index, lpTargetObj->MapNumber, x, y, type, level, dur,
					Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
	}

	if ( g_bChocolateBoxEvent )
	{
		if ( lpObj->Level >= g_iPinkChocolateBoxDropLevelMin && lpObj->Level <= g_iPinkChocolateBoxDropLevelMax )
		{
			if ( (rand()%10000) < g_iPinkChocolateBoxDropRate )
			{
				type = ItemGetNumberMake(14,32);
				level = 0;
				x = lpTargetObj->X;
				y = lpTargetObj->Y;
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemSerialCreateSend(lpTargetObj->m_Index, lpTargetObj->MapNumber, x, y, type, level, dur,
					Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
		else if ( lpObj->Level >= g_iRedChocolateBoxDropLevelMin && lpObj->Level <= g_iRedChocolateBoxDropLevelMax )
		{
			if ( (rand()%10000) < g_iRedChocolateBoxDropRate )
			{
				type = ItemGetNumberMake(14,33);
				level = 0;
				x = lpTargetObj->X;
				y = lpTargetObj->Y;
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemSerialCreateSend(lpTargetObj->m_Index, lpTargetObj->MapNumber, x, y, type, level, dur,
					Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
		else if ( lpObj->Level >= g_iBlueChocolateBoxDropLevelMin && lpObj->Level <= g_iBlueChocolateBoxDropLevelMax )
		{
			if ( (rand()%10000) < g_iBlueChocolateBoxDropRate )
			{
				type = ItemGetNumberMake(14,34);
				level = 0;
				x = lpTargetObj->X;
				y = lpTargetObj->Y;
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemSerialCreateSend(lpTargetObj->m_Index, lpTargetObj->MapNumber, x, y, type, level, dur,
					Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
	}

	if ( g_bBlossomBoxDropOn )
	{
		if ( (rand()%10000) < g_iBlossomBoxDropRate )
		{
			type = ItemGetNumberMake(14,84);
			level = 0;
			x = lpTargetObj->X;
			y = lpTargetObj->Y;
			int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

			ItemSerialCreateSend(lpTargetObj->m_Index, lpTargetObj->MapNumber, x, y, type, level, dur,
				Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);
			
			return TRUE;
		}
	}

	if ( g_bCandyBoxEvent )
	{
		if ( lpObj->Level >= g_iLightPurpleCandyBoxDropLevelMin && lpObj->Level <= g_iLightPurpleCandyBoxDropLevelMax )
		{
			if ( (rand()%10000) < g_iLightPurpleCandyBoxDropRate )
			{
				type = ItemGetNumberMake(14,32);
				level = 1;
				x = lpTargetObj->X;
				y = lpTargetObj->Y;
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemSerialCreateSend(lpTargetObj->m_Index, lpTargetObj->MapNumber, x, y, type, level, dur,
					Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
		else if ( lpObj->Level >= g_iVermilionCandyBoxDropLevelMin && lpObj->Level <= g_iVermilionCandyBoxDropLevelMax )
		{
			if ( (rand()%10000) < g_iVermilionCandyBoxDropRate )
			{
				type = ItemGetNumberMake(14,33);
				level = 1;
				x = lpTargetObj->X;
				y = lpTargetObj->Y;
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemSerialCreateSend(lpTargetObj->m_Index, lpTargetObj->MapNumber, x, y, type, level, dur,
					Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
		else if ( lpObj->Level >= g_iDeepBlueCandyBoxDropLevelMin && lpObj->Level <= g_iDeepBlueCandyBoxDropLevelMax )
		{
			if ( (rand()%10000) < g_iDeepBlueCandyBoxDropRate )
			{
				type = ItemGetNumberMake(14,34);
				level = 1;
				x = lpTargetObj->X;
				y = lpTargetObj->Y;
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemSerialCreateSend(lpTargetObj->m_Index, lpTargetObj->MapNumber, x, y, type, level, dur,
					Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
	}

	if ( g_bHallowinDayEventOn )
	{
		bool bIsBossMonster = false;

		if ( lpObj->m_Index == 349 || lpObj->m_Index == 364 || lpObj->m_Index == 361 ||
			 lpObj->m_Index == 362 || lpObj->m_Index == 363 )
			bIsBossMonster = true;

		if ( !bIsBossMonster )
		{
			if ( (rand()%10000) < g_iHallowinDayEventItemDropRate  )
			{
				type = ItemGetNumberMake(14,45);
				level = 0;
				x = lpObj->X;
				y = lpObj->Y;
				int iMaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemSerialCreateSend(lpTargetObj->m_Index, lpObj->MapNumber, x, y, type, level, dur,
					Option1,Option2,Option3, iMaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
	}

	if ( g_bSantaPolymorphRingDropOn )
	{
		bool bIsBossMonster = false;

		if ( lpObj->m_Index == 349 || lpObj->m_Index == 364 || lpObj->m_Index == 361 ||
			 lpObj->m_Index == 362 || lpObj->m_Index == 363 )
			bIsBossMonster = true;

		if ( !bIsBossMonster )
		{
			if ( (rand()%10000) < g_iSantaPolymorphRingDropRate  )
			{
				type = ItemGetNumberMake(13,41);
				level = 0;
				x = lpObj->X;
				y = lpObj->Y;
				int iMaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemSerialCreateSend(lpTargetObj->m_Index, lpObj->MapNumber, x, y, type, level, dur,
					Option1,Option2,Option3, iMaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
	}

	if ( lpObj->MapNumber == MAP_INDEX_BARRACKS )
	{
		if ( (rand()%10000) < g_iCondorFlameDropRate )
		{
			type = ItemGetNumberMake(13,52);	// Condor Flame
			level = 0;
			x = lpObj->X;
			y = lpObj->Y;
			int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

			ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, level, dur,
			Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);
			return TRUE;
		}			
	}

	if ( lpObj->Level >= g_ConfigRead.data.common.SpiritMapDropLevel && rand()% 10000 < g_ConfigRead.data.common.SpiritMapDropRate )
	{
		type = ItemGetNumberMake(13,145);	// Spirit Map Fragment
		level = 0;
		x = lpObj->X;
		y = lpObj->Y;
		int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

		ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, 0, 1.0, 0, 0, 0, MaxHitUser, 0, 0, 0, 0, 0);
		return TRUE;
	}

	if (g_EvoMonMng.IsEvoMonEnable() == true)
	{
		if (lpObj->Level >= g_EvoMonMng.GetSummonScrollDropLevel() && rand() % 10000 < g_EvoMonMng.GetSummonScrollDropLevel())
		{
			type = ItemGetNumberMake(14, 278);	// Piece of Evomon Scroll
			level = 0;
			x = lpObj->X;
			y = lpObj->Y;
			int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

			ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, x, y, type, 0, 1.0, 0, 0, 0, MaxHitUser, 0, 0, 0, 0, 0);
			return TRUE;
		}
	}

	return FALSE;
}

void gObjMonsterSummonSkillForLordSilvester(LPGameObject &lpObj)
{
	PMSG_DURATION_MAGIC_RECV pDuration;
	
	pDuration.Dir = 31;
	pDuration.X = lpObj->X;
	pDuration.Y = lpObj->Y;
	pDuration.MagicNumberH = 0;
	pDuration.MagicNumberL = 1;
	pDuration.NumberH = HIBYTE(lpObj->TargetNumber);
	pDuration.NumberL = LOBYTE(lpObj->TargetNumber);

	GSProtocol.CGDurationMagicRecv((LPBYTE)&pDuration, lpObj->m_Index);

	lpObj->m_ActState.Attack = 0;
	lpObj->m_ActState.Move = 0;
	lpObj->m_ActState.Rest = 1;
	lpObj->NextActionTime = 3500;
	lpObj->m_nPhaseLordSilvester++;

	gObjAddBuffEffect(lpObj, ::BUFFTYPE_MONSTER_MAGIC_IMMUNE, 0, 0, 0, 0, 1209600000);
}

void KillLordSilvesterRecallMon(int nLordSilvesterIndex, int nRecallMonIndex)
{
	if (!ObjectMaxRange(nLordSilvesterIndex))
	{
		return;
	}

	if (!ObjectMaxRange(nRecallMonIndex))
	{
		return;
	}

	CGameObject* lpObj = &gGameObjects[nLordSilvesterIndex];

	if (lpObj->Type != OBJ_MONSTER)
	{
		return;
	}

	BOOL bAllKillRecallMon = TRUE;

	for (int i = 0; i < 3; i++)
	{
		if ( lpObj->m_nRecallMonIndex[i] == nRecallMonIndex )
		{
			lpObj->m_nRecallMonIndex[i] = -1;
		}
	}

	for (int i = 0; i < 3; i++)
	{
		if ( lpObj->m_nRecallMonIndex[i] > -1 )
		{
			bAllKillRecallMon = FALSE;
		}
	}

	if ( bAllKillRecallMon == TRUE )
	{
		gObjRemoveBuffEffect(lpObj, BUFFTYPE_MONSTER_MAGIC_IMMUNE);
	}
}

void BlockLordSilvesterEntrance()
{
	for (int i = 0; i < 6; i++)
	{
		for (int j = ::g_iLordSilvester_Entrance[i].btStartX; j <= ::g_iLordSilvester_Entrance[i].btEndX; j++)
		{
			for (int k = ::g_iLordSilvester_Entrance[i].btStartY; k <= ::g_iLordSilvester_Entrance[i].btEndY; k++)
			{
				MapC[MAP_INDEX_URUK_MOUNTAIN].m_attrbuf[k * 256 + j] |= 4;
			}
		}
	}
}

void UnBlockLordSilvesterEntrance()
{
	for (int i = 0; i < 6; i++)
	{
		for (int j = ::g_iLordSilvester_Entrance[i].btStartX; j <= ::g_iLordSilvester_Entrance[i].btEndX; j++)
		{
			for (int k = ::g_iLordSilvester_Entrance[i].btStartY; k <= ::g_iLordSilvester_Entrance[i].btEndY; k++)
			{
				MapC[MAP_INDEX_URUK_MOUNTAIN].m_attrbuf[k * 256 + j] &= ~4;
			}
		}
	}
}

void SendLordSilvesterBlockInfo(LPGameObject &lpObj, BYTE btMapSetType)
{
	char cTEMP_BUF[256];
	PMSG_SETMAPATTR_COUNT * lpMsg = (PMSG_SETMAPATTR_COUNT *)cTEMP_BUF;

	PHeadSetB((LPBYTE)lpMsg, 0x46, sizeof(PMSG_SETMAPATTR_COUNT) + sizeof(PMSG_SETMAPATTR) * 12);
	PMSG_SETMAPATTR * lpMsgBody = (PMSG_SETMAPATTR *)&cTEMP_BUF[7];
	lpMsg->btType = 0;
	lpMsg->btCount = 6;
	lpMsg->btMapAttr = 4;
	lpMsg->btMapSetType = btMapSetType;

	lpMsgBody[0].btX = ::g_iLordSilvester_Entrance[0].btStartX;
	lpMsgBody[0].btY = ::g_iLordSilvester_Entrance[0].btStartY;
	lpMsgBody[1].btX = ::g_iLordSilvester_Entrance[0].btEndX;
	lpMsgBody[1].btY = ::g_iLordSilvester_Entrance[0].btEndY;

	lpMsgBody[2].btX = ::g_iLordSilvester_Entrance[1].btStartX;
	lpMsgBody[2].btY = ::g_iLordSilvester_Entrance[1].btStartY;
	lpMsgBody[3].btX = ::g_iLordSilvester_Entrance[1].btEndX;
	lpMsgBody[3].btY = ::g_iLordSilvester_Entrance[1].btEndY;

	lpMsgBody[4].btX = ::g_iLordSilvester_Entrance[2].btStartX;
	lpMsgBody[4].btY = ::g_iLordSilvester_Entrance[2].btStartY;
	lpMsgBody[5].btX = ::g_iLordSilvester_Entrance[2].btEndX;
	lpMsgBody[5].btY = ::g_iLordSilvester_Entrance[2].btEndY;

	lpMsgBody[6].btX = ::g_iLordSilvester_Entrance[3].btStartX;
	lpMsgBody[6].btY = ::g_iLordSilvester_Entrance[3].btStartY;
	lpMsgBody[7].btX = ::g_iLordSilvester_Entrance[3].btEndX;
	lpMsgBody[7].btY = ::g_iLordSilvester_Entrance[3].btEndY;

	lpMsgBody[8].btX = ::g_iLordSilvester_Entrance[4].btStartX;
	lpMsgBody[8].btY = ::g_iLordSilvester_Entrance[4].btStartY;
	lpMsgBody[9].btX = ::g_iLordSilvester_Entrance[4].btEndX;
	lpMsgBody[9].btY = ::g_iLordSilvester_Entrance[4].btEndY;

	lpMsgBody[10].btX = ::g_iLordSilvester_Entrance[5].btStartX;
	lpMsgBody[10].btY = ::g_iLordSilvester_Entrance[5].btStartY;
	lpMsgBody[11].btX = ::g_iLordSilvester_Entrance[5].btEndX;
	lpMsgBody[11].btY = ::g_iLordSilvester_Entrance[5].btEndY;

	if (aIndex == -1)
	{
		for (int i = g_ConfigRead.server.GetObjectMaxMonster(); i < g_ConfigRead.server.GetObjectMax(); i++)
		{
			if (gGameObjects[i].MapNumber == MAP_INDEX_URUK_MOUNTAIN) //season3 changed
			{
				if (gGameObjects[i].Connected > PLAYER_LOGGED)
				{
					IOCP.DataSend(i, (LPBYTE)lpMsg, lpMsg->h.size);
				}
			}
		}
	}

	else
	{
		IOCP.DataSend(aIndex, (LPBYTE)lpMsg, lpMsg->h.size);
	}
}

void CQeustNpcTeleport::Run(LPGameObject &lpObj)
{
	if ( this->RefCount > 0 )
	{
		return;
	}

	this->TimeCount++;

	if ( this->TimeCount > gQuestNPCTeleportTime )
	{
		PMSG_MAGICATTACK_RESULT pMsg;

		this->TimeCount = 0;
		PHeadSetBE((LPBYTE)&pMsg, 0x19, sizeof(pMsg));
		pMsg.MagicNumberH = SET_NUMBERH(AT_SKILL_TELEPORT);
		pMsg.MagicNumberL = SET_NUMBERL(AT_SKILL_TELEPORT);
		pMsg.SourceNumberH = SET_NUMBERH(aIndex);
		pMsg.SourceNumberL = SET_NUMBERL(aIndex);
		pMsg.TargetNumberH = SET_NUMBERH(aIndex);
		pMsg.TargetNumberL = SET_NUMBERL(aIndex);

		GSProtocol.MsgSendV2(&gGameObjects[aIndex], (UCHAR*)&pMsg, pMsg.h.size);

		gObjViewportListProtocolDestroy(&gGameObjects[aIndex]);
		gObjClearViewport(&gGameObjects[aIndex]);

		int tableindex;

		while ( true )
		{
			tableindex = rand() % MAX_QUEST_TELEPORT;

			if ( this->LastMapNumber != tableindex )
			{
				this->LastMapNumber = tableindex;
				break;
			}
		}

		g_SendNPCInfo.SendPortalCoordinateMoveNpc(lpObj->MapNumber,
			this->m_QuestNPCTeleportPos[tableindex].mapnum,
			lpObj->X, lpObj->Y,
			this->m_QuestNPCTeleportPos[tableindex].x,
			this->m_QuestNPCTeleportPos[tableindex].y,
			lpObj->Class);

		lpObj->X = this->m_QuestNPCTeleportPos[tableindex].x;
		lpObj->Y = this->m_QuestNPCTeleportPos[tableindex].y;
		lpObj->TX = lpObj->X;
		lpObj->TY = lpObj->Y;
		lpObj->MTX = lpObj->X;
		lpObj->MTY = lpObj->Y;
		lpObj->m_OldX = lpObj->TX;
		lpObj->m_OldY = lpObj->TY;
		lpObj->MapNumber = this->m_QuestNPCTeleportPos[tableindex].mapnum;
		lpObj->Dir = this->m_QuestNPCTeleportPos[tableindex].dir;
		lpObj->StartX = lpObj->X;
		lpObj->StartY = lpObj->Y;
		lpObj->m_State = 1;
		lpObj->PathCount = 0;

		sLog->outBasic("[Quest] %s Teleport MAP : %s", this->m_strNPCName.c_str(), Lang.GetMap(0, this->m_QuestNPCTeleportPos[tableindex].mapnum));
	}
}


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

