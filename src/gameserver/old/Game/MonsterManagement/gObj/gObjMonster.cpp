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
#include "CItemObjectDrop.h"
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
#include "Bag.h"
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

void gObjMonsterMoveRegen(int x, int y, CGameObject lpObj)
{
	gMSetBase.GetBoxPosition(Obj.MapNumber, x-1, y-1, x+1, y+1, Obj.X, Obj.Y);

	Obj.TX = Obj.X;
	Obj.TY = Obj.Y;
	Obj.MTX = Obj.X;
	Obj.MTY = Obj.Y;
	Obj.StartX = Obj.X;
	Obj.StartY = Obj.Y;
}

BOOL gObjMonsterRegen(CGameObject &Obj)
{
	int map[3];

	map[0] = 0;
	map[1] = 2;
	map[2] = 3;

	gObjViewportListProtocolDestroy(lpObj);
	gObjViewportClose(lpObj);

	Obj.m_ActState.Attack = 0;
	Obj.m_ActState.EmotionCount = 0;
	Obj.m_ActState.Escape = 0;
	Obj.m_ActState.Move = 0;
	Obj.m_ActState.Rest = 0;
	Obj.m_ActState.Emotion = 0;
	Obj.TargetNumber = -1;
	Obj.NextActionTime = 5000;
	Obj.PathStartEnd = 0;
	Obj.PathCur = 0;
	Obj.PathCount = 0;
	Obj.m_nPhaseLordSilvester = 0;

	if ( Obj.Class == 673 )
	{
		Obj.m_iPentagramMainAttribute = 6;
		g_bActiveUrukTrap = false;
		g_bSilvesterEntranceBlock = false;
		g_i64LordSilvesterBlockTime = 0;

		UnBlockLordSilvesterEntrance();
		SendLordSilvesterBlockInfo(-1, 1);

	}
	if ( Obj.m_iPentagramMainAttribute == 6 )
	{
		switch ( rand()%5 )
		{
			case 0:
				Obj.m_iPentagramMainAttribute = EL_FIRE;
				break;
			case 1:
				Obj.m_iPentagramMainAttribute = EL_WATER;
				break;
			case 2:
				Obj.m_iPentagramMainAttribute = EL_EARTH;
				break;
			case 3:
				Obj.m_iPentagramMainAttribute = EL_WIND;
				break;
			case 4:
				Obj.m_iPentagramMainAttribute = EL_DARKNESS;
				break;
		}
	}

	if ( Obj.Class == 43 )
	{
		Obj.MapNumber = map[ rand() % 3 ];

		if ( gMSetBase.GetBoxPosition(Obj.MapNumber, 10, 10, 240, 240, Obj.X, Obj.Y)== FALSE )
		{
			Obj.Live = FALSE;
			Obj.m_State = 4;
			Obj.RegenTime = GetTickCount();
			Obj.DieRegen = TRUE;
			return FALSE;
		}

		Obj.TX = Obj.X;
		Obj.TY = Obj.Y;
		Obj.MTX = Obj.X;
		Obj.MTY = Obj.Y;
		Obj.StartX = Obj.X;
		Obj.StartY = Obj.Y;
	}

	else if ( DragonEvent->IsDragonEventMonster(Obj.m_Index) == true )
	{
		gObjDel(Obj.m_Index);
		return FALSE;
	}

	else if ( AttackEvent->IsAttackEventMonster(Obj.m_Index) == true )
	{
		gObjDel(Obj.m_Index);
		return FALSE;
	}

	else if (gEledoradoEvent.IsEledoradoMonster(Obj.m_Index) == true)
	{
		gObjDel(Obj.m_Index);
		return FALSE;
	}
 
	else if ( (Obj.m_Attribute < 51) ? FALSE : (Obj.m_Attribute > 58)? FALSE : TRUE )
	{
		Obj.Live = TRUE;
		Obj.DieRegen = FALSE;
		CreateFrustrum(Obj.X, Obj.Y, Obj.m_Index);
		gObjClearViewport(lpObj);
		gObjViewportListCreate(Obj.m_Index);
		gObjViewportListProtocolCreate(lpObj);
		return FALSE;
	}

	else if (Obj.Class == 460 || Obj.Class == 461 || Obj.Class == 462) //Season 4.5 addon
	{
		if (g_Raklion.GetRaklionState() != RAKLION_STATE_END)
		{
			return FALSE;
		}
	}
	else if (Obj.Class == 459)//Season 4.5 addon
	{
		if (g_Raklion.GetRaklionState() != RAKLION_STATE_STANDBY || g_Raklion.GetRaklionState() != RAKLION_STATE_READY)
		{
			Obj.Live = FALSE;
			Obj.m_State = 4;
			Obj.RegenTime = GetTickCount();
			Obj.DieRegen = TRUE;
			return FALSE;
		}
	}
	else if ((Obj.Class == 457 && Obj.MapNumber == MAP_INDEX_HATCHERY) || (Obj.Class == 458 && Obj.MapNumber == MAP_INDEX_HATCHERY))//Season 4.5 addon
	{
		Obj.Live = FALSE;
		Obj.m_State = 4;
		Obj.RegenTime = GetTickCount();
		Obj.DieRegen = TRUE;
		return FALSE;
	}

	else if ( Obj.Class >= 504 && Obj.Class <= 521 )
	{
		return FALSE;
	}

	else if (Obj.Class == 681 || Obj.Class == 690)
	{
		return FALSE;
	}

	else if ( Obj.Class >= 523 && Obj.Class <= 528 )
	{
		return FALSE;
	}

	else if ( Obj.Class >= 586 && Obj.Class <= 602 && g_ArcaBattle.IsArcaBattleServer())
	{
		return FALSE;
	}

	else if ( Obj.Class >= 627 && Obj.Class <= 641 && g_ArcaBattle.IsArcaBattleServer())
	{
		return FALSE;
	}

	else if ( Obj.Class >= 658 && Obj.Class <= 668 )
	{
		return FALSE;
	}

	else if ( Obj.Class >= 678 && Obj.Class <= 680 )
	{
		return FALSE;
	}

	else if ( Obj.Class >= 560 && Obj.Class <= 561 )
	{
		return FALSE;
	}

	else if ( Obj.MapNumber == MAP_INDEX_DEVILSQUARE_FINAL )
	{
		return FALSE;
	}

	else if (BC_MAP_RANGE(Obj.MapNumber))
	{
		int iBridgeIndex = g_BloodCastle.GetBridgeIndex(Obj.MapNumber);
	//	));
		BLOODCASTLE_MONSTER_POSITION * lpPos = g_BloodCastle.GetMonsterPosData(Obj.m_PosNum, iBridgeIndex, Obj.Class);

		//sLog->outBasic("[Blood Castle][Bug Tracker] (%d) Regen Monster (Class:%d) (Index:%d) (PosNum:%d) (lpPos:%d)",
		//	iBridgeIndex + 1, Obj.Class, Obj.m_Index, Obj.m_PosNum, lpPos != nullptr ? true : false);

		if (lpPos == nullptr)
		{
			sLog->outError( "[Blood Castle][Error] lpPos == nullptr (%d) (%d) (%d)", Obj.m_PosNum, iBridgeIndex, Obj.Class);
			return FALSE;
		}

		g_BloodCastle.GetPosition(lpPos, Obj.MapNumber, Obj.X, Obj.Y);
		Obj.MTX = Obj.X;
		Obj.MTY = Obj.Y;
		Obj.TX = Obj.X;
		Obj.TY = Obj.Y;
		Obj.StartX = Obj.X;
		Obj.StartY = Obj.Y;
	}
	
	else if (DS_MAP_RANGE(Obj.MapNumber))
	{
		g_DevilSquare.GetPosition(Obj.m_PosNum, Obj.m_bDevilSquareIndex, Obj.MapNumber, Obj.X, Obj.Y);
		Obj.MTX = Obj.X;
		Obj.MTY = Obj.Y;
		Obj.TX = Obj.X;
		Obj.TY = Obj.Y;
		Obj.StartX = Obj.X;
		Obj.StartY = Obj.Y;
	}

	else if (g_MineSystem.IsTwinkle(Obj.Class) == TRUE)
	{
		Obj.MTX = Obj.X;
		Obj.MTY = Obj.Y;
		Obj.TX = Obj.X;
		Obj.TY = Obj.Y;
		Obj.StartX = Obj.X;
		Obj.StartY = Obj.Y;
	}

	else if (gEventMSetBase.IsEventMonster(Obj.m_Index) == true)
	{
		gEventMSetBase.RegenMonster(Obj.m_Index);
	}

	else if ( gMSetBase.GetPosition(Obj.m_PosNum, Obj.MapNumber, Obj.X, Obj.Y) == FALSE )
	{
		if ( g_ConfigRead.server.GetServerType() == SERVER_CASTLE && Obj.m_Attribute == 62 )
		{
			BYTE cX = Obj.X;
			BYTE cY = Obj.Y;

			if(gObjGetRandomFreeLocation(Obj.MapNumber,(BYTE&)cX,(BYTE&)cY,5,5,30) == 0 )
			{
				return FALSE;
			}
			else
			{
				Obj.X = cX;
				Obj.Y = cY;
			}
		}
		else
		{
			Obj.Live = FALSE;
			Obj.m_State = 4;
			Obj.RegenTime = GetTickCount();
			Obj.DieRegen = TRUE;
			return FALSE;
		}
	}
	else
	{
		Obj.MTX = Obj.X;
		Obj.MTY = Obj.Y;
		Obj.TX = Obj.X;
		Obj.TY = Obj.Y;
		Obj.StartX = Obj.X;
		Obj.StartY = Obj.Y;
		int iPentagramMainAttribute = 0;
		gMSetBase.GetPentagramMainAttribute(Obj.m_PosNum, &iPentagramMainAttribute);

		if ( iPentagramMainAttribute )
		{
			Obj.m_iPentagramMainAttribute = iPentagramMainAttribute;
		}
	}

	gObjMonsterHitDamageInit(lpObj);

	if ( Obj.MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE && g_CrywolfSync.GetCrywolfState() >= 2 && g_CrywolfSync.GetCrywolfState() <= 5  ) // Crywolf
	{
	
	}

	else if ( g_CrywolfSync.GetOccupationState() == 0 && g_CrywolfSync.GetApplyBenefit() == TRUE)
	{
		Obj.Life = ( Obj.m_iScriptMaxLife * g_CrywolfSync.GetMonHPBenefitRate()  ) / 100;
		Obj.MaxLife = (Obj.m_iScriptMaxLife * g_CrywolfSync.GetMonHPBenefitRate() ) / 100;
	}

	if (Obj.Class >= 605 && Obj.Class <= 607)
	{
		g_MineSystem.SetTwinkleInfo(Obj.m_Index);
	}

	return TRUE;
}







int gObjMonsterViewportIsCharacter(CGameObject &Obj)
{
	int tObjNum;

	for ( int n = 0; n<MaxViewportMonster ; n++ )
	{
		if ( Obj.VpPlayer2[n].state  != 0 )
		{
			tObjNum = Obj.VpPlayer2[n].number;

			if ( tObjNum == Obj.TargetNumber ) 
			{
				return n;
			}
		}
	}

	return -1;
}






void gObjMonsterHitDamageInit(CGameObject &Obj)
{
	for ( int i = 0; i< MAX_ST_HIT_DAMAGE ; i++ )
	{
		Obj.sHD[i].number  = -1;
	}

	Obj.sHDCount = 0;
}






int gObjMonsterHitDamageUserDel(CGameObject &Obj)
{
	int delok = 0;
	int delindex = -1;

	for (int n = 0; n < MAX_ST_HIT_DAMAGE; n++)
	{
		if (Obj.sHD[n].number >= 0)
		{
			delok = FALSE;

			if (gObjIsConnected(Obj.sHD[n].number) == TRUE)
			{
				if ((GetTickCount() - Obj.sHD[n].LastHitTime) > 20000)
				{
					delok = TRUE;
				}

				if (getGameObject(Obj.sHD[n]->number)->MapNumber != Obj.MapNumber)
				{
					delok = TRUE;
				}
				else if (gObjCheckTileArea(Obj.m_Index, getGameObject(Obj.sHD[n]->number]->X, getGameObject(Obj.sHD[n)->number)->Y, 20) == FALSE)
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
				Obj.sHD[n].number = -1;
				delindex = n;
				Obj.sHDCount--;
			}

		}
	}

	return delindex;
}





void gObjMonsterSetHitDamage(CGameObject &Obj, int hit_player, int hit_damage)
{
	int blank = -1;

	for (int n=0;n<MAX_ST_HIT_DAMAGE;n++)
	{
		if ( Obj.sHD[n].number >= 0 )
		{
			if ( Obj.sHD[n].number == hit_player )
			{
				Obj.sHD[n].HitDamage += hit_damage;
				Obj.sHD[n].LastHitTime = GetTickCount();

				if ( Obj.sHD[n].HitDamage > Obj.MaxLife )
				{
					Obj.sHD[n].HitDamage = Obj.MaxLife;
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
		Obj.sHD[blank].number = hit_player;
		Obj.sHD[blank].HitDamage = hit_damage;

		if ( Obj.sHD[blank].HitDamage > Obj.MaxLife )
		{
			Obj.sHD[blank].HitDamage = Obj.MaxLife;
		}
	
		Obj.sHD[blank].LastHitTime = GetTickCount();
		Obj.sHDCount++;
	}
	else
	{
		sLog->outBasic("error-L2 : Hit Damage Set error");
	}
}






int gObjMonsterTopHitDamageUser(CGameObject &MonObj)
{
	int MaxHitDamage = 0;
	int MaxHitDamageUser = -1;

	for ( int n=0;n<MAX_ST_HIT_DAMAGE;n++)
	{
		if ( lpMonObj.sHD[n].number >= 0 )
		{
			if ( lpMonObj.sHD[n].HitDamage > MaxHitDamage )
			{
				MaxHitDamage = lpMonObj.sHD[n].HitDamage;
				MaxHitDamageUser = lpMonObj.sHD[n].number;
			}
		}
	}

	return MaxHitDamageUser;
}







int	 gObjMonsterLastHitDamageUser(CGameObject &MonObj, int & hitindex)
{
	int LastHitTime = 0;
	int LastHitUser = -1;

	for ( int n=0;n<MAX_ST_HIT_DAMAGE;n++)
	{
		if ( lpMonObj.sHD[n].number >= 0 )
		{
			if ( lpMonObj.sHD[n].LastHitTime > LastHitTime )
			{
				LastHitTime = lpMonObj.sHD[n].LastHitTime;
				LastHitUser = lpMonObj.sHD[n].number;
				hitindex = n;
			}
		}
	}

	return LastHitUser;
}

int gObjMonsterGetDamageUser(CGameObject &MonObj, CGameObject lpUserObj)
{
	for (int n = 0; n<MAX_ST_HIT_DAMAGE; n++)
	{
		if (lpMonObj.sHD[n].number >= 0)
		{
			if (lpMonObj.sHD[n].number == lpUserObj.m_Index)
			{
				return lpMonObj.sHD[n].HitDamage;
			}
		}
	}
}





BOOL gObjMonsterMoveCheck(CGameObject &Obj, int tx, int ty)
{
	if ( Obj.m_ActState.Emotion  == 1 )
	{
		return TRUE;
	}

	if ((Obj.Class >= 647 && Obj.Class <= 650) && Obj.m_PosNum < 0)
	{
		return TRUE;
	}

	if ((Obj.Class >= 653 && Obj.Class <= 657) && Obj.m_PosNum < 0)
	{
		return TRUE;
	}

	if ((Obj.Class >= 674 && Obj.Class <= 677) && Obj.m_PosNum < 0)
	{
		return TRUE;
	}

	if (DragonEvent->IsDragonEventMonster(Obj.m_Index) == true)
	{
		DRAGON_EVENT_INFO * lpPos = DragonEvent->GetMonsterPos(Obj.Class);

		if (lpPos == NULL)
		{
			return FALSE;
		}

		tx -= Obj.StartX;
		ty -= Obj.StartY;

		int dis = sqrt(static_cast<float>(tx * tx + ty * ty));

		if (dis < 1 || dis > lpPos->m_Dis)
		{
			return FALSE;
		}

		return TRUE;
	}

	if (AttackEvent->IsAttackEventMonster(Obj.m_Index) == true)
	{
		ATTACKEVENT_MONSTER_POS * lpPos = AttackEvent->GetMonsterPos(Obj.Class);

		if (lpPos == NULL)
		{
			return FALSE;
		}

		tx -= Obj.StartX;
		ty -= Obj.StartY;

		int dis = sqrt(static_cast<float>(tx * tx + ty * ty));

		if (dis < 1 || dis > lpPos->m_Dis)
		{
			return FALSE;
		}

		return TRUE;
	}

	if (gEledoradoEvent.IsEledoradoMonster(Obj.m_Index) == true)
	{
		ELEDORARO_MONSTER_POS * lpPos = gEledoradoEvent.GetMonsterPos(Obj.Class);

		if (lpPos == NULL)
		{
			return FALSE;
		}

		tx -= Obj.StartX;
		ty -= Obj.StartY;

		int dis = sqrt(static_cast<float>(tx * tx + ty * ty));

		if (dis < 1 || dis > lpPos->m_Dis)
		{
			return FALSE;
		}

		return TRUE;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE && Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		if (Obj.Class == 644 || Obj.Class == 645)
		{
			if (g_ChaosCastleFinal.m_CCFMP[Obj.m_PosNum].m_Dis < 1)
			{
				return FALSE;
			}

			tx -= Obj.StartX;
			ty -= Obj.StartY;

			int dis = sqrt(static_cast<float>(tx * tx + ty * ty));

			if (dis < 1 || dis > g_ChaosCastleFinal.m_CCFMP[Obj.m_PosNum].m_Dis)
			{
				return FALSE;
			}

			return TRUE;
		}
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE && Obj.MapNumber == MAP_INDEX_DEVILSQUARE_FINAL)
	{
		if (Obj.m_PosNum == -1)
		{
			return FALSE;
		}

		if ( g_DevilSquareFinal.m_DSFMonsterRegenInfo[Obj.m_PosNum].MonsterDis < 1 )
		{
			return FALSE;
		}

		tx -= Obj.StartX;
		ty -= Obj.StartY;

		int dis = sqrt(static_cast<float>(tx * tx + ty * ty));

		if ( dis > g_DevilSquareFinal.m_DSFMonsterRegenInfo[Obj.m_PosNum].MonsterDis)
		{
			return FALSE;
		}
	}

	if (CC_MAP_RANGE(Obj.MapNumber) == TRUE)
	{
		CHAOSCASTLE_MONSTER_POSITION * lpPos = g_ChaosCastle.GetMonsterPosData(Obj.m_PosNum, Obj.m_cChaosCastleIndex);

		if (lpPos == NULL)
		{
			return FALSE;
		}

		tx -= Obj.StartX;
		ty -= Obj.StartY;

		int dis = sqrt(static_cast<float>(tx * tx + ty * ty));
		
		if (dis < 1 || dis > lpPos->m_Dis)
		{
			return FALSE;
		}

		return TRUE;
	}

	if (DS_MAP_RANGE(Obj.MapNumber) == TRUE)
	{
		DEVILSQUARE_MONSTER_POSITION * lpPos = g_DevilSquare.GetMonsterPosData(Obj.m_PosNum, Obj.m_bDevilSquareIndex);

		if (lpPos == NULL)
		{
			return FALSE;
		}

		tx -= Obj.StartX;
		ty -= Obj.StartY;

		int dis = sqrt(static_cast<float>(tx * tx + ty * ty));

		if (dis < 1 || dis > lpPos->m_Dis)
		{
			return FALSE;
		}

		return TRUE;
	}

	if (BC_MAP_RANGE(Obj.MapNumber) == TRUE)
	{
		int iBridgeIndex = g_BloodCastle.GetBridgeIndex(Obj.MapNumber);
		BLOODCASTLE_MONSTER_POSITION * lpPos = g_BloodCastle.GetMonsterPosData(Obj.m_PosNum, iBridgeIndex, Obj.Class);

		if (lpPos == NULL)
		{
			return FALSE;
		}

		tx -= Obj.StartX;
		ty -= Obj.StartY;

		int dis = sqrt(static_cast<float>(tx * tx + ty * ty));

		if (dis < 1 || dis > lpPos->m_Dis)
		{
			return FALSE;
		}

		return TRUE;
	}

	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE && (Obj.MapNumber == MAP_INDEX_KANTURU_BOSS || Obj.MapNumber == MAP_INDEX_HATCHERY) && Obj.m_PosNum < 0)
	{
		if (Obj.Class == 459 || Obj.Class == 460 || Obj.Class == 461 || Obj.Class == 462)
		{
			return FALSE;
		}

		tx -= Obj.StartX;
		ty -= Obj.StartY;
		int dis = sqrt(static_cast<float>(tx*tx + ty*ty));

		if (dis > 30)
		{
			sLog->outBasic("[ KANTURU/RAKLION ][ Debug - m_PosNum ] Fail %s(Index:%d) X%d-Y%d -> X%d-Y%d(%d)",
				Obj.Name, Obj.Class, Obj.StartX, Obj.StartY, tx, ty, dis);
			return FALSE;
		}
	}

	if (gEventMSetBase.IsEventMonster(Obj.m_Index) == true)
	{
		tx -= Obj.StartX;
		ty -= Obj.StartY;

		int dis = sqrt(static_cast<float>(tx * tx + ty * ty));

		if (dis > gEventMSetBase.GetDistance(Obj.m_Index))
		{
			return FALSE;
		}

		return TRUE;
	}

	else
	{
		LPMONSTER_POSITION lpPos = &gMSetBase.m_Mp[Obj.m_PosNum];

		if ( lpPos->m_Dis < 1 )
		{
			return FALSE;
		}

		tx -= Obj.StartX;
		ty -= Obj.StartY;

		int dis = sqrt(static_cast<float>(tx * tx + ty * ty));

		if ( dis > lpPos->m_Dis)
		{
			return FALSE;
		}	
	}

	return TRUE;
}






BOOL gObjMonsterGetTargetPos(CGameObject &Obj)
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
	CGameObject lpTargetObj;

	if( Obj.m_MoveRange == 0 && 
		(gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_STONE) == true ||
		 gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_STUN) == true ||
		 gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_SLEEP) == true ||
		 gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_FREEZE_2) == true ||
		 gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_EARTH_BINDS) == true))
	{
		return FALSE;
	}

	if (!MAX_MAP_RANGE(Obj.MapNumber))
	{
		return FALSE;
	}

	int tObjNum = Obj.TargetNumber;

	if ( ObjectMaxRange(tObjNum) == FALSE )
	{
		return FALSE;
	}

	lpTargetObj = getGameObject(tObjNum);

	if ( lpTargetObj.Teleport != 0 )
	{
		return FALSE;
	}

	int vpn = gObjMonsterViewportIsCharacter(lpObj);

	if ( vpn < 0 )
	{
		return FALSE;
	}
	
	tpx = lpTargetObj.X;
	mtx = tpx;
	tpy = lpTargetObj.Y;
	mty = tpy;
	int dis;

	if ( Obj.m_AttackType >= 100 )
	{
		dis = Obj.m_AttackRange + 2;
	}
	else
	{
		dis = Obj.m_AttackRange;
	}

	if ( Obj.X < mtx )
	{
		tpx -= dis;
	}

	if ( Obj.X > mtx )
	{
		tpx += dis;
	}

	if ( Obj.Y < mty )
	{
		tpy -= dis;
	}

	if ( Obj.Y > mty )
	{
		tpy += dis;
	}

	searchp = GetPathPacketDirPos( (lpTargetObj.X - tpx), (lpTargetObj.Y - tpy) ) * 2;

	if ( MapC[Obj.MapNumber].GetStandAttr(tpx, tpy) == 0 )
	{
		while ( searchcount-- )
		{
			mtx = lpTargetObj.X + RoadPathTable[searchp];
			mty = lpTargetObj.Y + RoadPathTable[1+searchp];
			attr = MapC[Obj.MapNumber].GetAttr(mtx, mty);
			result = gObjMonsterMoveCheck(lpObj, mtx, mty);

			if ( Obj.Class == 249 )
			{
				if ( (attr&2)!=2 && result == TRUE )
				{
					Obj.MTX = mtx;
					Obj.MTY = mty;
					return TRUE;
				}
			}
			else if ( (attr&1) != 1 && (attr&2) != 2 && (attr&4) != 4 && (attr&8) != 8 && result == TRUE )
			{
				Obj.MTX = mtx;
				Obj.MTY = mty;
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

	attr = MapC[Obj.MapNumber].GetAttr(tpx, tpy);
	result = gObjMonsterMoveCheck(lpObj, mtx, mty);

	if ( Obj.Class == 249 )
	{
		if ( (attr&2)!=2 && result == TRUE )
		{
			Obj.MTX = tpx;
			Obj.MTY = tpy;
			return TRUE;
		}
	}
	else if ( (attr&1) != 1 && (attr&2) != 2 && (attr&4) != 4 && (attr&8) != 8 && result == TRUE )
	{
		Obj.MTX = tpx;
		Obj.MTY = tpy;
		return TRUE;
	}

	return FALSE;
}










BOOL gObjGetTargetPos(CGameObject &Obj, int sx, int sy, int & tx , int & ty)
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

	if (!MAX_MAP_RANGE(Obj.MapNumber))
	{
		return FALSE;
	}

	if ( Obj.m_AttackType >= 100 )
	{
		dis = Obj.m_AttackRange + 2;
	}
	else
	{
		dis = Obj.m_AttackRange;
	}

	if ( Obj.X < mtx )
	{
		tpx -= dis;
	}

	if ( Obj.X > mtx )
	{
		tpx += dis;
	}

	if ( Obj.Y < mty )
	{
		tpy -= dis;
	}

	if ( Obj.Y > mty )
	{
		tpy += dis;
	}

	searchp = GetPathPacketDirPos( sx - tpx, sy - tpy ) * 2;

	if ( MapC[Obj.MapNumber].GetStandAttr(tpx, tpy) == 0 )
	{
		while ( searchcount-- )
		{
			mtx = sx + RoadPathTable[searchp];
			mty = sy + RoadPathTable[1+searchp];
			attr = MapC[Obj.MapNumber].GetAttr(mtx, mty);
			
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

	attr = MapC[Obj.MapNumber].GetAttr(tpx, tpy);

	if ( Obj.m_Attribute >= 100 )
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











int gObjCallMonsterSetEnemy(CGameObject &Obj, int Target)
{
	if ( Obj.Type != OBJ_USER )
	{
		return -1;
	}

	CGameObject lpCallObj = getGameObject(Obj.m_RecallMon);

	if ( lpCallObj.TargetNumber >= 0 )
	{
		return -1;
	}

	if ( lpCallObj.TargetNumber != Target )
	{
		lpCallObj.TargetNumber = Target;
		lpCallObj.m_ActState.EmotionCount = 30;
		lpCallObj.m_ActState.Emotion = 1;
	}

	return -1;
}









int gObjMonsterSearchEnemy(CGameObject &Obj, BYTE objtype)
{
	int n;
	int tx;
	int ty;
	int dis;
	int mindis = Obj.m_ViewRange;
	int searchtarget = -1;
	int tObjNum;
	int count = 3;
	int t1 = objtype;
	int t2 = objtype;

	for (n=0;n<MaxViewportMonster;n++)
	{
		tObjNum = Obj.VpPlayer2[n].number;

		if ( tObjNum >= 0 )
		{
			if ( (getGameObject(tObjNum)->Type == t1 || getGameObject(tObjNum)->Type == t2) && (getGameObject(tObjNum)->Live != FALSE) )
			{
				if ( ((getGameObject(tObjNum)->Class >= 100 && getGameObject(tObjNum)->Class < 110 ) || getGameObject(tObjNum)->Class == 523) || (getGameObject(tObjNum)->Type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon >= 0) )
				{

				}
				
				else
				{
					if (g_NewPVP.IsObserver(getGameObject(tObjNum)) == TRUE)
					{
						continue;
					}

					if ( (getGameObject(tObjNum)->Authority &2) == 2 || (getGameObject(tObjNum)->Authority & 0x20) == 0x20)
					{
						if ( (getGameObject(tObjNum)->GameMaster & GM_MONSTERS_INVISIBLE) == GM_MONSTERS_INVISIBLE )
						{
							continue;
						}
					}

					
					if ( getGameObject(tObjNum)->Teleport == 0 )
					{
						tx = Obj.X - getGameObject(tObjNum)->X;
						ty = Obj.Y - getGameObject(tObjNum)->Y;
						dis = sqrt(static_cast<float>(tx * tx + ty * ty));
						Obj.VpPlayer2[n].dis = dis;

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






int gObjGuardSearchEnemy(CGameObject &Obj)
{
	int n;
	int tx;
	int ty;
	int dis;
	int mindis = Obj.m_ViewRange;
	int searchtarget = -1;
	int tObjNum;
	BYTE attr;

	if ( g_GensSystem.IsMapBattleZone(Obj.MapNumber) )
	{
		return -1;
	}

	for (n=0;n<MaxViewportMonster;n++)
	{
		tObjNum = Obj.VpPlayer2[n].number;

		if ( tObjNum >= 0 && MAX_MAP_RANGE(getGameObject(tObjNum)->MapNumber))
		{
			BOOL bEnableAttack = FALSE; //Season 2.5 add-on

			if( getGameObject(tObjNum)->PartyNumber >= 0) //Season 2.5 add-on
			{
				if( gParty.GetPKPartyPenalty(getGameObject(tObjNum)->PartyNumber) > 4 )
				{
					bEnableAttack = TRUE;
				}
				
			}

			else if( getGameObject(tObjNum)->m_PK_Level > 4 ) 
			{
				bEnableAttack = TRUE;
			}
			
			if ( getGameObject(tObjNum)->Type == OBJ_USER )
			{
				if(bEnableAttack == TRUE) //Season 2.5 add-on
				{
					attr = MapC[getGameObject(tObjNum)->MapNumber]->GetAttr(getGameObject(tObjNum)->X, getGameObject(tObjNum)->Y);

					if ( (attr&1) != 1 )
					{
						tx = Obj.X - getGameObject(tObjNum)->X;
						ty = Obj.Y - getGameObject(tObjNum)->Y;
						dis = sqrt(static_cast<float>(tx * tx + ty * ty));
						Obj.VpPlayer2[n].dis = dis;

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
	




void gObjMonsterStateProc(CGameObject &ObjSource, int aMsgCode, CGameObject &ObjTarget, int aMsgSubCode)
{
	if ( ObjSource.m_iMonsterBattleDelay > 0 )
	{
		return;
	}

	switch ( aMsgCode )
	{
		case 0:

			if ( ObjSource.m_Attribute == 0 )
			{
				return;
			}
			
			if ( ObjSource.Live == FALSE || ObjSource.m_State != 2)
			{
				return;
			}

			if ( ObjSource.m_ActState.Emotion == 0 )
			{
				ObjSource.m_ActState.Emotion = 1;
				ObjSource.m_ActState.EmotionCount = 10;
			}
			else if ( ObjSource.m_ActState.Emotion == 1 )
			{
				ObjSource.m_ActState.EmotionCount = 10;
			}

			if ( ObjSource.Class >= 504 && ObjSource.Class <= 511 && ObjSource.MaxLife * 0.3 >= ObjSource.Life && gObjCheckUsedBuffEffect(ObjSource, 81) == false )
			{
				CMagicInf cMagicInf;
				memset(&cMagicInf, 0x00, sizeof(cMagicInf));

				cMagicInf.m_Skill = AT_SKILL_BERSERKER;
				gObjUseSkill.SkillBerserker(ObjSource.m_Index, &cMagicInf, ObjSource.m_Index);
				gGameProtocol.GCUseMonsterSkillSend(ObjSource, ObjSource, 59);
				ObjSource.m_ActState.Attack = 0;
				ObjSource.m_ActState.Move = 0;
				ObjSource.m_ActState.Emotion = 0;
				ObjSource.NextActionTime = 1000;
			}

			if ( ObjSource.Class >= 504 && ObjSource.Class <= 521 )
			{
				g_ImperialGuardian.SetTargetMoveAllMonster(ObjSource.m_nZoneIndex, Obj.m_Index);
			}

			if ( ObjSource.m_ActState.Attack == 0 && ObjSource.PathStartEnd == 0)
			{
				if ( ObjectMaxRange(Obj.m_Index) )
				{
					int map = ObjSource.MapNumber;
					BYTE attr;
					int dis = gObjCalDistance(ObjSource, getGameObject(Obj.m_Index));
					int range;

					if ( ObjSource.m_AttackType >= 100 )
					{
						range = ObjSource.m_AttackRange +2;
					}
					else
					{
						range = ObjSource.m_AttackRange;
					}

					if ( dis <= range )
					{
						if ( ObjSource.m_RecallMon >= 0 )
						{
							if ( ObjSource.m_RecallMon >= 0 )
							{
								if ( ObjSource.Type == OBJ_MONSTER )
								{
									ObjSource.TargetNumber = Obj.m_Index;
								}
							}
							else
							{
								ObjSource.TargetNumber = Obj.m_Index;
							}
						}
						else if ( (rand()%100) < 90 )
						{
							if ( ObjSource.m_RecallMon >= 0 )
							{
								if ( ObjSource.Type == OBJ_MONSTER )
								{
									ObjSource.TargetNumber = Obj.m_Index;
								}
							}
							else
							{
								ObjSource.TargetNumber = Obj.m_Index;
							}
						}
					}
					else
					{
						if ( IMPERIAL_MAP_RANGE(ObjSource.MapNumber) == TRUE &&
							ObjSource.Class >= 504 && ObjSource.Class <= 521 )
						{
							g_ImperialGuardian.SetTargetMoveAllMonster(ObjSource.m_nZoneIndex, Obj.m_Index);
						}

						else
						{
							BYTE wall = 0;

							if (MAX_MAP_RANGE(map) == FALSE)
							{
								sLog->outError( "[ERROR] map is out of range %s %d", __FILE__, __LINE__);
								return;
							}

							wall = MapC[map].CheckWall2(ObjSource.X, ObjSource.Y, ObjSource.X, ObjSource.Y);
	
							if ( wall == 1 )
							{
								attr = MapC[map].GetAttr(ObjSource.X, ObjSource.Y);

								if ( (attr&1) != 1 )
								{
									if ( ObjSource.TargetNumber < 0 )
									{
										ObjSource.TargetNumber = Obj.m_Index;
									}
								}
							}
						}
					}

					if ( ObjSource.m_bIsInMonsterHerd != false && ObjSource.TargetNumber == Obj.m_Index )
					{
						if ( ObjSource.m_lpMonsterHerd )
						{
							ObjSource.m_lpMonsterHerd->BeenAttacked (ObjSource, getGameObject(Obj.m_Index));
						}
					}
				}
			}
			else
			{
				if ( (rand() % 2 )== 1 && ObjSource.PathStartEnd == 0)
				{
					int IndexEnemy = ObjSource.TargetNumber;
					if(ObjectMaxRange(IndexEnemy))
					{
						int EnemyMap = getGameObject(IndexEnemy)->MapNumber;

						int enemydis = gObjCalDistance(ObjSource, getGameObject(Obj.m_Index));
						int range;

						if ( ObjSource.m_AttackType >= 100 )
						{
							range = ObjSource.m_AttackRange + 2;
						}
						else
						{
							range = ObjSource.m_AttackRange;
						}

						if ( enemydis <= range )
						{
							ObjSource.m_ActState.Attack = 1;
							ObjSource.TargetNumber = Obj.m_Index;
						}
						else
						{
							if ( MapC[EnemyMap].CheckWall2(ObjSource.X, ObjSource.Y, getGameObject(IndexEnemy)->X, getGameObject(IndexEnemy)->Y) == 1 )
							{
								ObjSource.m_ActState.Attack = 1;
								ObjSource.TargetNumber = Obj.m_Index;
							}
						}
					}
				}
				else
				{
					int MaxLife = ObjSource.MaxLife;
					MaxLife >>= 1;

					if ( MaxLife > ObjSource.Life )
					{
						if ( ObjSource.m_Attribute != 2 )
						{
							ObjSource.m_ActState.Emotion = 2;
							ObjSource.m_ActState.EmotionCount = 2;
						}
					}
				}
			}
			break;

		case 1:
			if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
			{
				if (ObjSource.m_btCsNpcType)
				{
					switch (ObjSource.m_btCsNpcType)
					{
					case 1:	g_CastleSiege.DelNPC(ObjSource.m_Index, ObjSource.Class, ObjSource.m_iCsNpcExistVal, TRUE);	break;
					case 2:	g_CastleSiege.DelNPC(ObjSource.m_Index, ObjSource.Class, ObjSource.m_iCsNpcExistVal, FALSE); break;
					case 3:	g_CastleSiege.DelNPC(ObjSource.m_Index, ObjSource.Class, ObjSource.m_iCsNpcExistVal, FALSE);	break;
					}

					if (ObjSource.Class == 287 || ObjSource.Class == 286)
						g_CsNPC_Mercenary.DeleteMercenary(ObjSource.m_Index);

					if (ObjSource.Class == 278)
						g_CsNPC_LifeStone.DeleteLifeStone(ObjSource.m_Index);

					gObjDel(ObjSource.m_Index);
				}
			}

			if ( KALIMA_MAP_RANGE(ObjSource.MapNumber)  )
			{
				if ( ObjSource.Class == 161 || ObjSource.Class == 181 || ObjSource.Class == 189 || ObjSource.Class == 197 || ObjSource.Class == 267 )
				{
					g_KalimaGate.CreateKalimaGate2(Obj.m_Index, ObjSource.MapNumber, ObjSource.X, ObjSource.Y);
				}
			}

			if (ObjSource.Type == OBJ_USER)
			{
				g_QuestExpProgMng.ChkUserQuestTypeMonsterKill(getGameObject(Obj.m_Index), ObjSource);
				g_QuestExpProgMng.QuestMonsterItemDrop(0, getGameObject(Obj.m_Index), ObjSource);
			}

			if ( ObjSource.Class == 673 )
			{
				g_bActiveUrukTrap = false;
				g_bSilvesterEntranceBlock = false;
				g_i64LordSilvesterBlockTime = 0;

				UnBlockLordSilvesterEntrance();
				SendLordSilvesterBlockInfo(-1, 1);
			}

			if ( ObjSource.Class >= 678 && ObjSource.Class <= 680 )
			{
			
			}

			gObjMonsterDieGiveItem(ObjSource, getGameObject(Obj.m_Index) );
			ObjSource.NextActionTime = 500;

			if ( ObjSource.m_RecallMon >= 0 )
			{
				gObjMonsterCallKill(ObjSource.m_RecallMon);
			}

			if ( ObjSource.Class == 409 || ObjSource.Class == 410 || ObjSource.Class == 411 || ObjSource.Class == 412 )
			{
				g_QuestInfo.AddMonsterKillCount(ObjSource);
			}

			if (  BC_MAP_RANGE(ObjSource.MapNumber) != FALSE && ObjSource.Type >= OBJ_MONSTER)
			{
				int iBridgeIndex = g_BloodCastle.GetBridgeIndex(ObjSource.MapNumber); //season3 add-on
				g_BloodCastle.KillMonsterProc(iBridgeIndex, ObjSource);
			}
			
			break;

		case 2:
			if ( ObjSource.Live != FALSE )
			{
				if ( BC_MAP_RANGE(ObjSource.MapNumber) == FALSE )
				{
					if ( ObjSource.Class != 131 ||  ((  (ObjSource.Class-132)<0)?FALSE:((ObjSource.Class-132)>2)?FALSE:TRUE)==FALSE )
					{
						gObjBackSpring(ObjSource, getGameObject(Obj.m_Index));
					}
				}
			}

			break;

		case 3:
			ObjSource.TargetNumber = -1;
			ObjSource.LastAttackerID = -1;
			ObjSource.m_ActState.Emotion = 0;
			ObjSource.m_ActState.Attack = 0;
			ObjSource.m_ActState.Move = 0;
			ObjSource.NextActionTime = 1000;
			break;

		case 4:
			ObjSource.m_ActState.Emotion = 3;
			ObjSource.m_ActState.EmotionCount = 1;
			break;

		case 5:
			gObjMemFree(ObjSource.m_Index);
			break;

		case 6:
			if ( ObjSource.Live != FALSE )
			{
				gObjBackSpring2(ObjSource, getGameObject(Obj.m_Index), 2);
			}

			break;

		case 7:
			if ( ObjSource.Live != FALSE )
			{
				gObjBackSpring2(ObjSource, getGameObject(Obj.m_Index), 3);
			}
			break;

		case 55:
			gObjAttack(ObjSource, getGameObject(Obj.m_Index), NULL, FALSE, 0, 0, FALSE, 0, 0);
			break;

		case 56:
			{
				CGameObject lpTargetObj = getGameObject(Obj.m_Index);

				if ( gObjCheckUsedBuffEffect(lpTargetObj, BUFFTYPE_POISON) == FALSE )
				{
					if ( retResistance(lpTargetObj, 1) == 0 )
					{
						lpTargetObj.lpAttackObj = ObjSource;
						gObjAddBuffEffect(lpTargetObj, BUFFTYPE_POISON, EFFECTTYPE_POISON_DMG_TICK, 3, 0, 0, aMsgSubCode);
					}
				}
			}
			break;

		case 57:
			{
				CGameObject lpTargetObj = getGameObject(Obj.m_Index);
				gObjBackSpring2(lpTargetObj, ObjSource, aMsgSubCode);
			}
			break;

		case 50:
			{
				if ( ObjSource.Class == 533 )
				{
					CMagicInf* lpMagic = gObjGetMagicSearch(ObjSource, aMsgSubCode);
					gObjAttack(ObjSource, getGameObject(Obj.m_Index), lpMagic, 0, 1, 0, 0, 0, 0);
				}

				else
				{
					if ( ObjSource.Live == TRUE )
					{
						CMagicInf* lpMagic = gObjGetMagicSearch(ObjSource, aMsgSubCode);
						gObjAttack(ObjSource, getGameObject(Obj.m_Index), lpMagic, 0, 1, 0, 0, 0, 0);
					}
				}
			}
			break;

		case 62:
			{
				int nDamage = 20 * ObjSource.MaxLife / 100;
				gObjAttack(ObjSource, getGameObject(Obj.m_Index), 0, 0, 0, nDamage, 0, 0, 0);
			}
			break;
	}
}




void gObjMonsterProcess(CGameObject &Obj)
{
	gObjMsgProc(lpObj);

	if ( Obj.Live == FALSE )
	{
		return;
	}

	if ( gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_SLEEP) == TRUE )
	{
		return;
	}

	if ( Obj.m_iMonsterBattleDelay > 0 )
	{
		Obj.m_iMonsterBattleDelay--;
	}

	if ( (GetTickCount() - Obj.CurActionTime ) < (Obj.NextActionTime + Obj.DelayActionTime) )
	{
		return;
	}

	Obj.CurActionTime = GetTickCount();

	if ( BC_MAP_RANGE(Obj.MapNumber) != FALSE )
	{
		if ( Obj.Class == 131|| ((Obj.Class-132<0)?FALSE:(Obj.Class-132>2)?FALSE:TRUE) != FALSE )
		{
			return;
		}
	}

	if ( (Obj.Class >= 460 && Obj.Class <= 462) && Obj.MapNumber== 58 )
	{
		return;
	}

	if (Obj.Class == 704 || Obj.Class == 705)
	{
		return;
	}

	if ( ((Obj.m_Attribute < 51)?FALSE:(Obj.m_Attribute > 58)?FALSE:TRUE) != FALSE )
	{
		if ( Obj.m_Attribute == 58 )
		{
			g_KalimaGate.KalimaGateAct2(Obj.m_Index);
			return;
		}

		g_KalimaGate.KalimaGateAct(Obj.m_Index);
		return;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (Obj.Class == 283)
		{
			return;
		}
		else if (Obj.Class == 288)
		{
			return;
		}
		else if (Obj.Class == 278)
		{
			return;
		}
	}

	if ( Obj.Class == 689 && g_bActiveUrukTrap == true )
	{
		gObjMonsterTrapAct(lpObj);
	}

	if ( (Obj.Class >= 100 && Obj.Class <= 110) || Obj.Class == 523 )
	{
		gObjMonsterTrapAct(lpObj);
		
	}
	else if ( Obj.Class == 200 )
	{
		int ground;
		int team = gCheckGoal(Obj.X, Obj.Y, ground);

		if ( team >= 0 )
		{
			gObjMonsterRegen(lpObj);
			gBattleSoccerScoreUpdate(ground, team);
			return;
		}
	}

	else if ( Obj.Class >= 524 && Obj.Class <= 528 )
	{
		return;
	}

	else if ( Obj.Class >= 504 && Obj.Class <= 521 )
	{
		g_ImperialGuardian.MonsterBaseAct(lpObj);
	}

	else if ( Obj.Class >= 598 && Obj.Class <= 602 )
	{
		return;
	}

	else if ( Obj.Class >= 627 && Obj.Class <= 631 )
	{
		return;
	}

	else 
	{
		if ( Obj.Class == 287 || Obj.Class == 286 )
		{
			g_CsNPC_Mercenary.MercenaryAct(Obj.m_Index);
		}

		if ( Obj.Type == OBJ_MONSTER && DG_MAP_RANGE(Obj.MapNumber) )
		{
			g_DoppelGanger.CheckDoppelgangerMonsterPos(lpObj);
		}

		if ( Obj.m_bIsInMonsterHerd != false )
		{
			if ( Obj.m_lpMonsterHerd != NULL )
			{
				Obj.m_lpMonsterHerd->MonsterBaseAct(lpObj);
			}
		}
		else
		{
			gObjMonsterBaseAct(lpObj);
		}
	}

	if ( Obj.m_Attribute == 100 )
	{
		if ( Obj.m_RecallMon >= 0 && Obj.m_RecallMon < g_ConfigRead.server.GetObjectMax() )
		{
			CGameObject lpCallMonObj;
			BOOL Success = FALSE;

			lpCallMonObj = getGameObject(Obj.m_RecallMon);

			if ( Obj.MapNumber != lpCallMonObj.MapNumber )
			{
				Success = TRUE;
			}
	
			if ( gObjCalDistance(lpCallMonObj, lpObj)> 14 )
			{
				Success = TRUE;
			}

			if ( Success == TRUE )
			{
				gObjTeleportMagicUse(Obj.m_Index, (BYTE)lpCallMonObj.X+1, lpCallMonObj.Y);
				Obj.MapNumber = lpCallMonObj.MapNumber;

				return;
			}
		}
	}

	if ( Obj.m_ActState.Move != 0 )
	{
		if ( PathFindMoveMsgSend(lpObj ) == TRUE )
		{
			Obj.m_ActState.Move  = (DWORD)0;
		}

		Obj.m_ActState.Move = (DWORD)0;

		return;
	}

	if ( Obj.m_ActState.Attack == 1 )
	{
		if (Obj.Connected == PLAYER_PLAYING && Obj.Type == OBJ_MONSTER && Obj.Class == 459)
		{
			return;
		}

		if ( TMonsterSkillManager::CheckMonsterSkill(Obj.Class) )
		{
			BOOL bEnableAttack = TRUE;
			int iTargetNumber = Obj.TargetNumber;

			if (iTargetNumber < 0 )
				bEnableAttack = FALSE;

			else if ( getGameObject(iTargetNumber)->Live == FALSE || getGameObject(iTargetNumber)->Teleport != 0)
				bEnableAttack = FALSE;

			else if ( getGameObject(iTargetNumber)->Connected <= PLAYER_LOGGED || getGameObject(iTargetNumber)->CloseCount != -1 )
				bEnableAttack = FALSE;

			if ( bEnableAttack == FALSE )
			{
				Obj.TargetNumber = -1;
				Obj.m_ActState.Emotion = 0;
				Obj.m_ActState.Attack = 0;
				Obj.m_ActState.Move = 0;
				Obj.NextActionTime = 1000;
				return;
			}
			
			if ( rand()%4 == 0 && Obj.Class != 504 )
			{
				PMSG_ATTACK pAttackMsg;

				pAttackMsg.AttackAction = 120;
				pAttackMsg.DirDis = Obj.Dir;
				pAttackMsg.NumberH = (BYTE)((DWORD)iTargetNumber >>(DWORD)8);
				pAttackMsg.NumberL = iTargetNumber &0xFF;

				gGameProtocol.GCActionSend(lpObj, 120, Obj.m_Index, iTargetNumber);
				gObjAttack(lpObj, getGameObject(iTargetNumber), NULL, FALSE, 0, 0, FALSE, 0, 0);
			}

			else if (Obj.Class == 681 && g_EvoMonMng.IsEvolutedEvoMon(Obj.m_Index) == false)
			{
				PMSG_ATTACK pAttackMsg;

				pAttackMsg.AttackAction = 120;
				pAttackMsg.DirDis = Obj.Dir;
				pAttackMsg.NumberH = (BYTE)((DWORD)iTargetNumber >> (DWORD)8);
				pAttackMsg.NumberL = iTargetNumber & 0xFF;

				gGameProtocol.GCActionSend(lpObj, 120, Obj.m_Index, iTargetNumber);
				gObjAttack(lpObj, getGameObject(iTargetNumber), NULL, FALSE, 0, 0, FALSE, 0, 0);
			}

			else
			{
				TMonsterSkillManager::UseMonsterSkill(Obj.m_Index, iTargetNumber, 0, -1, NULL);
			}

			Obj.m_ActState.Attack = 0;
		}
		else
		{
			int AttackType = Obj.m_AttackType;
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

			if ( Obj.Class >= 533 && Obj.Class <= 539 )
			{
				lc6 = FALSE;
			}

			if ( lc6 != FALSE || Obj.m_AttackType == 50)
			{
				int iTargetNumber = Obj.TargetNumber;

				if (iTargetNumber >= 0 )
				{
					if ( getGameObject(iTargetNumber)->Connected > PLAYER_LOGGED && getGameObject(iTargetNumber)->CloseCount == -1 )
					{
						if ( getGameObject(iTargetNumber)->Live == FALSE )
						{
							Obj.TargetNumber = -1;
							Obj.m_ActState.Emotion = 0;
							Obj.m_ActState.Attack = 0;
							Obj.m_ActState.Move = 0;
							Obj.NextActionTime = 1000;
						}
						else if ( getGameObject(iTargetNumber)->Teleport == 0 ) // if is not dead
						{
							gObjMonsterMagicAttack(lpObj, -1);

							if ( Obj.Type == OBJ_MONSTER )
							{
								if ( Obj.m_bIsInMonsterHerd != false )
								{
									if (Obj.m_lpMonsterHerd != NULL )
									{
										Obj.m_lpMonsterHerd->MonsterAttackAction(lpObj, getGameObject(iTargetNumber));
									}
								}
							}
						}
					}
					else
					{
						Obj.TargetNumber = -1;
						Obj.m_ActState.Emotion = 0;
						Obj.m_ActState.Attack = 0;
						Obj.m_ActState.Move = 0;
						Obj.NextActionTime = 1000;
					}
				}
			}
			else
			{
				int iTargetNumber = Obj.TargetNumber;

				if (iTargetNumber >= 0 )
				{
					if ( getGameObject(iTargetNumber)->Connected > PLAYER_LOGGED &&  getGameObject(iTargetNumber)->CloseCount == -1 )
					{
						if ( getGameObject(iTargetNumber)->Live == FALSE )
						{
							Obj.TargetNumber = -1;
							Obj.m_ActState.Emotion = 0;
							Obj.m_ActState.Attack = 0;
							Obj.m_ActState.Move = 0;
							Obj.NextActionTime = 1000;
						}
						else if ( getGameObject(iTargetNumber)->Teleport == 0 )
						{
							gObjMonsterAttack(lpObj, getGameObject(iTargetNumber));

							if ( Obj.Type == OBJ_MONSTER )
							{
								if ( Obj.m_bIsInMonsterHerd != false )
								{
									if (Obj.m_lpMonsterHerd != NULL )
									{
										Obj.m_lpMonsterHerd->MonsterAttackAction(lpObj, getGameObject(iTargetNumber));
									}
								}
							}
						}
						
					}
					else
					{
						Obj.TargetNumber = -1;
						Obj.m_ActState.Emotion = 0;
						Obj.m_ActState.Attack = 0;
						Obj.m_ActState.Move = 0;
						Obj.NextActionTime = 1000;
					}
				}
			}

			Obj.m_ActState.Attack = 0;
		}
	}
}

void gObjMonsterMagicAttack(CGameObject &Obj, int iMonsterClass)
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
	pDuration.X = Obj.X;
	pDuration.Y = Obj.Y;

	gGameProtocol.CGDurationMagicRecv((BYTE*)&pDuration, Obj.m_Index);

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
	pCount.X = Obj.X;
	pCount.Y = Obj.Y;
	ASBOfs = sizeof(pCount);

	while ( true )
	{
		if ( Obj.VpPlayer2[count].state )
		{
			if ( Obj.VpPlayer2[count].type == OBJ_USER )
			{
				tObjNum = Obj.VpPlayer2[count].number;

				if ( tObjNum >= 0 )
				{
					if ( Obj.Class == 77 )	// Phoenix of Darkness
					{
						pAttack.NumberH = SET_NUMBERH(tObjNum);
						pAttack.NumberL = SET_NUMBERL(tObjNum);
						std::memcpy(&AttackSendBuff[ASBOfs], &pAttack, sizeof(pAttack));
						ASBOfs+= sizeof(pAttack);
						pCount.Count++;
					}
					else if ( Obj.Class == 275 || gObjCalDistance(lpObj, getGameObject(tObjNum)) < 6 )
					{
						pAttack.NumberH = SET_NUMBERH(tObjNum);
						pAttack.NumberL = SET_NUMBERL(tObjNum);
						std::memcpy(&AttackSendBuff[ASBOfs], &pAttack, sizeof(pAttack));
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
		std::memcpy(AttackSendBuff, &pCount, sizeof(pCount));

		if ( Obj.Class == 161 || Obj.Class == 181 || Obj.Class == 189 || Obj.Class == 197 || Obj.Class == 267 || Obj.Class == 275 )
		{
			gObjMonsterBeattackRecv(AttackSendBuff, Obj.m_Index);
		}
		else
		{
			gGameProtocol.CGBeattackRecv(AttackSendBuff, Obj.m_Index, TRUE);
		}
	}
}

void gObjUseMonsterSpecialAbillity(CGameObject &MonsterObj)
{
	if ( lpMonsterObj.Class == 275 )	// Kundun
	{
		PMSG_MAGICATTACK pAttackMsg;

		pAttackMsg.MagicNumberH = 0;
		pAttackMsg.MagicNumberL = 1;
		pAttackMsg.NumberH = SET_NUMBERH(lpMonsterObj.TargetNumber);
		pAttackMsg.NumberL = SET_NUMBERL(lpMonsterObj.TargetNumber);
		pAttackMsg.Dis = 0;

		if ( (lpMonsterObj.MaxLife / 25.0f) > lpMonsterObj.Life  )
		{
			gGameProtocol.CGMagicAttack((BYTE*)&pAttackMsg, lpMonsterObj.m_Index);
			gGameProtocol.CGMagicAttack((BYTE*)&pAttackMsg, lpMonsterObj.m_Index);
		}
		else if ( (lpMonsterObj.MaxLife / 5.0f) > lpMonsterObj.Life  )
		{
			gGameProtocol.CGMagicAttack((BYTE*)&pAttackMsg, lpMonsterObj.m_Index);
		}
	}
}

void gObjMonsterBeattackRecv(BYTE * lpRecv, CGameObject &Obj)
{
	PMSG_BEATTACK_COUNT * lpCount = (PMSG_BEATTACK_COUNT *)lpRecv;
	int lOfs = sizeof(PMSG_BEATTACK_COUNT);
	int tNumber;
	WORD MagicNumber = MAKE_NUMBERW(lpCount->MagicNumberH, lpCount->MagicNumberL);
	CMagicInf * lpMagic = gObjGetMagic(getGameObject(Obj.m_Index), MagicNumber);

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
		gObjAttack(getGameObject(Obj.m_Index), getGameObject(tNumber), lpMagic, 1, 1, 0 ,0, 0, 0);
		lOfs+= sizeof(PMSG_BEATTACK);
	}
}

static int skillevo = 100;

void gObjMonsterAttack(CGameObject &Obj, CGameObject lpTargetObj)
{
	int AttackType = Obj.m_AttackType;
	WORD MagicNumber;
	if ( AttackType >= 100 )
		AttackType = 0;

	if ( Obj.m_iMonsterBattleDelay > 0 )
		return;
	
	if ( Obj.Class == 144 || Obj.Class == 174 || Obj.Class == 182 || Obj.Class == 190 ||
		Obj.Class == 260 || Obj.Class == 268 || Obj.Class == 334 )
	{
		if ( rand()%2 )
		{
			PMSG_MAGICATTACK pAttackMsg;

			pAttackMsg.MagicNumberH = 0;
			pAttackMsg.MagicNumberL = 0;
			pAttackMsg.NumberH = SET_NUMBERH(Obj.TargetNumber);
			pAttackMsg.NumberL = SET_NUMBERL(Obj.TargetNumber);
			pAttackMsg.Dis = 0;

			gGameProtocol.CGMagicAttack((BYTE*)&pAttackMsg, Obj.m_Index);
			return;
		}

		AttackType = 0;
	}

	if ( Obj.Class == 161 || Obj.Class == 181 || Obj.Class == 189 ||
		Obj.Class == 197 || Obj.Class == 267 || Obj.Class == 275  )
	{
		PMSG_MAGICATTACK pAttackMsg;
		MagicNumber = rand()%6+1;
		pAttackMsg.MagicNumberH = HIBYTE(MagicNumber);
		pAttackMsg.MagicNumberL = LOBYTE(MagicNumber);
		pAttackMsg.NumberH = SET_NUMBERH(Obj.TargetNumber);
		pAttackMsg.NumberL = SET_NUMBERL(Obj.TargetNumber);
		pAttackMsg.Dis = 0;

		gGameProtocol.CGMagicAttack((BYTE*)&pAttackMsg, Obj.m_Index);

		if ( MagicNumber == 1 || MagicNumber == 2 || MagicNumber == 0 )
		{
			gObjUseMonsterSpecialAbillity(lpObj);	// Kundun
		}

		gObjMonsterMagicAttack(lpObj, Obj.Class);
		return;
	}

	if ( Obj.Class == 149 || Obj.Class == 179 || Obj.Class == 187 ||
		Obj.Class == 195 || Obj.Class == 265 || Obj.Class == 273 || Obj.Class == 335 )
	{
		PMSG_MAGICATTACK pAttackMsg;
		MagicNumber = rand()%2+1;
		pAttackMsg.MagicNumberH = HIBYTE(MagicNumber);
		pAttackMsg.MagicNumberL = LOBYTE(MagicNumber);
		pAttackMsg.NumberH = SET_NUMBERH(Obj.TargetNumber);
		pAttackMsg.NumberL = SET_NUMBERL(Obj.TargetNumber);
		pAttackMsg.Dis = 0;

		gGameProtocol.CGMagicAttack((BYTE*)&pAttackMsg, Obj.m_Index);
		return;
	}
	
	if (     Obj.Class == 145
          || Obj.Class == 175
          || Obj.Class == 183
          || Obj.Class == 191
          || Obj.Class == 261
          || Obj.Class == 269
          || Obj.Class == 146
          || Obj.Class == 176
          || Obj.Class == 184
          || Obj.Class == 192
          || Obj.Class == 262
          || Obj.Class == 270
          || Obj.Class == 147
          || Obj.Class == 177
          || Obj.Class == 185
          || Obj.Class == 193
          || Obj.Class == 263
          || Obj.Class == 271
          || Obj.Class == 148
          || Obj.Class == 178
          || Obj.Class == 186
          || Obj.Class == 194
          || Obj.Class == 264
          || Obj.Class == 272
          || Obj.Class == 160
          || Obj.Class == 180
          || Obj.Class == 188
          || Obj.Class == 196
          || Obj.Class == 266
		  || Obj.Class == 274
		  || Obj.Class == 332
		  || Obj.Class == 333
		  || Obj.Class == 336
	      || Obj.Class == 337)
	{
		if ( (rand()%2) )
		{
			PMSG_MAGICATTACK pAttackMsg;
			pAttackMsg.MagicNumberH = 0;
			pAttackMsg.MagicNumberL = 1;
			pAttackMsg.NumberH = SET_NUMBERH(Obj.TargetNumber);
			pAttackMsg.NumberL = SET_NUMBERL(Obj.TargetNumber);
			pAttackMsg.Dis = 0;

			gGameProtocol.CGMagicAttack((BYTE*)&pAttackMsg, Obj.m_Index);
			return;
		}
		AttackType = 0;
	}
	
	if (	 Obj.Class == 89
          || Obj.Class == 95
          || Obj.Class == 112
          || Obj.Class == 118
          || Obj.Class == 124
          || Obj.Class == 130
          || Obj.Class == 143
		  || Obj.Class == 433)
	{

		PMSG_MAGICATTACK pAttackMsg;
		MagicNumber = rand()%2+1;
		pAttackMsg.MagicNumberH = HIBYTE(MagicNumber);
		pAttackMsg.MagicNumberL = LOBYTE(MagicNumber);
		pAttackMsg.NumberH = SET_NUMBERH(Obj.TargetNumber);
		pAttackMsg.NumberL = SET_NUMBERL(Obj.TargetNumber);
		pAttackMsg.Dis = 0;

		gGameProtocol.CGMagicAttack((BYTE*)&pAttackMsg, Obj.m_Index);
	}
	else if (	   Obj.Class == 163
            || Obj.Class == 165
            || Obj.Class == 167
            || Obj.Class == 169
            || Obj.Class == 171
            || Obj.Class == 173
			|| Obj.Class == 427)
	{
		PMSG_MAGICATTACK pAttackMsg;

		pAttackMsg.MagicNumberH = 0;
		pAttackMsg.MagicNumberL = 1;
		pAttackMsg.NumberH = SET_NUMBERH(Obj.TargetNumber);
		pAttackMsg.NumberL = SET_NUMBERL(Obj.TargetNumber);
		pAttackMsg.Dis = 0;

		gGameProtocol.CGMagicAttack((BYTE*)&pAttackMsg, Obj.m_Index);
	}
	else if ( Obj.Class == 66 || Obj.Class == 73 || Obj.Class == 77 )
	{
		PMSG_MAGICATTACK pAttackMsg;

		pAttackMsg.MagicNumberH = 0;
		pAttackMsg.MagicNumberL = 1;
		pAttackMsg.NumberH = SET_NUMBERH(Obj.TargetNumber);
		pAttackMsg.NumberL = SET_NUMBERL(Obj.TargetNumber);
		pAttackMsg.Dis = 0;

		gGameProtocol.CGMagicAttack((BYTE*)&pAttackMsg, Obj.m_Index);
	}

	else if ( Obj.Class >= 533 && Obj.Class <= 539 )
	{
		PMSG_DURATION_MAGIC_RECV pAttackMsg;

		pAttackMsg.Dir = Obj.Dir;
		pAttackMsg.MagicNumberH = 0;
		pAttackMsg.MagicNumberL = 0;
		pAttackMsg.X = lpTargetObj.X;
		pAttackMsg.Y = lpTargetObj.Y;
		pAttackMsg.NumberH = SET_NUMBERH(Obj.TargetNumber);
		pAttackMsg.NumberL = SET_NUMBERL(Obj.TargetNumber);

		gGameProtocol.CGDurationMagicRecv((BYTE*)&pAttackMsg, Obj.m_Index);
	}

	else if ( Obj.Class == 561 )
	{
		int Skill = gObjMonsterSelectSkillForMedusa(lpObj);

		if(Skill == 1)
		{
			PMSG_MAGICATTACK pAttackMsg;

			pAttackMsg.MagicNumberH = SET_NUMBERH(Skill);
			pAttackMsg.MagicNumberL = SET_NUMBERL(Skill);
			pAttackMsg.NumberH = SET_NUMBERH(Obj.TargetNumber);
			pAttackMsg.NumberL = SET_NUMBERL(Obj.TargetNumber);

			gGameProtocol.CGMagicAttack((BYTE*)&pAttackMsg, Obj.m_Index);
		}

		else
		{
			PMSG_DURATION_MAGIC_RECV pAttackMsg;

			pAttackMsg.MagicNumberH = SET_NUMBERH(Skill);
			pAttackMsg.MagicNumberL = SET_NUMBERL(Skill);
			pAttackMsg.X = lpTargetObj.X;
			pAttackMsg.Y = lpTargetObj.Y;
			pAttackMsg.Dis = 31;
			pAttackMsg.Dir = 31;
			pAttackMsg.NumberH = SET_NUMBERH(Obj.TargetNumber);
			pAttackMsg.NumberL = SET_NUMBERL(Obj.TargetNumber);

			gGameProtocol.CGDurationMagicRecv((BYTE*)&pAttackMsg, Obj.m_Index);
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
					pAttackMsg.DirDis = Obj.Dir;
					pAttackMsg.NumberH = SET_NUMBERH(Obj.TargetNumber);
					pAttackMsg.NumberL = SET_NUMBERL(Obj.TargetNumber);

					gGameProtocol.CGAttack(&pAttackMsg, Obj.m_Index);
				}
				break;
			
			default:
				{
					PMSG_MAGICATTACK pAttackMsg;
					pAttackMsg.MagicNumberH = 0;
					pAttackMsg.MagicNumberL = 0;
					pAttackMsg.NumberH = SET_NUMBERH(Obj.TargetNumber);
					pAttackMsg.NumberL = SET_NUMBERL(Obj.TargetNumber);
					pAttackMsg.Dis = 0;

					gGameProtocol.CGMagicAttack((BYTE*)&pAttackMsg, Obj.m_Index);
				}
		}
	}
}

BOOL PathFindMoveMsgSend(CGameObject &Obj)
{
	if (gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_STONE) == true ||
		gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_STUN) == true ||
		gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_SLEEP) == true ||
		gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_FREEZE_2) == true ||
		gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_EARTH_BINDS) == true)
	{
		return FALSE;
	}

	if ( MAX_MAP_RANGE(Obj.MapNumber) == FALSE ) return FALSE;

	PATH_t path;
	PMSG_MOVE pMove;
	BYTE bPath[8];
	

	pMove.h.c = 0xC1;

#if(ENABLE_SEASON12 == 1)
	pMove.h.headcode = HEADCODE_MOVE;
#else
	pMove.h.headcode = 0xD4;
#endif
	pMove.h.size = sizeof(pMove);
	pMove.X = Obj.X;
	pMove.Y = Obj.Y;

	BOOL bPathFound = FALSE;

	if ( Obj.Type == OBJ_MONSTER && Obj.m_bIsInMonsterHerd != FALSE )
	{
		bPathFound = MapC[Obj.MapNumber].PathFinding3(Obj.X, Obj.Y, Obj.MTX, Obj.MTY, &path);
	}
	else
	{
		bPathFound = MapC[Obj.MapNumber].PathFinding2(Obj.X, Obj.Y, Obj.MTX, Obj.MTY, &path);
	}

	if ( bPathFound )
	{
		int tx;
		int ty;
		int sx;
		int sy;
		BYTE pos = 0;

		memset(bPath, 0, sizeof(bPath));
		bPath[0] = (Obj.Dir *16) + ((path.PathNum-1)&0x0F);
		sx = Obj.X;
		sy = Obj.Y;
		Obj.PathX[0] = Obj.X;
		Obj.PathY[0] = Obj.Y;
		Obj.PathDir[0] = Obj.Dir;

		for (int n=1;n<path.PathNum;n++)
		{
			tx = path.PathX[n];
			ty = path.PathY[n];
			pos = GetPathPacketDirPos(tx-sx, ty-sy);
			Obj.PathX[n] = tx;
			Obj.PathY[n] = ty;
			Obj.PathDir[n] = pos;
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

		Obj.Dir = bPath[0] >> 4;
		Obj.PathCount = bPath[0] & 0x0F;
		Obj.PathCur = 1;
		Obj.PathTime = GetTickCount();
		bPath[0] &= 0x0F;
		bPath[0] |= pos*16;
		std::memcpy(pMove.Path, bPath, sizeof(pMove.Path));

		if ( Obj.PathCount > 0 )
		{
			MapC[Obj.MapNumber].ClearStandAttr(Obj.m_OldX, Obj.m_OldY);
			MapC[Obj.MapNumber].SetStandAttr(sx, sy);
			Obj.m_OldX = sx;
			Obj.m_OldY = sy;
		}

		gGameProtocol.PMoveProc(&pMove, Obj.m_Index);
		return TRUE;
	}

	return FALSE;
}


void gObjMonsterMoveAction(CGameObject &Obj)
{
	if( gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_STONE) == true || 
		gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_STUN) == true  ||
		gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_SLEEP) == true ||
		gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_FREEZE_2) == true ||
		gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_EARTH_BINDS) == true)
	{
		return;
	}

	if (!MAX_MAP_RANGE(Obj.MapNumber))
	{
		return;
	}

	int maxmoverange = Obj.m_MoveRange*2+1;
	int searchc=10;
	Obj.NextActionTime = 1000;
	BYTE tpx;
	BYTE tpy;

	while ( searchc-- != 0 )
	{
		__try
		{
			tpx = (Obj.X - Obj.m_MoveRange) + (rand()%maxmoverange);
			tpy = (Obj.Y - Obj.m_MoveRange) + (rand()%maxmoverange);
		}
		__except(maxmoverange=1, 1)
		{

		}

		int mchk = gObjMonsterMoveCheck(lpObj, tpx, tpy);
		BYTE attr = MapC[Obj.MapNumber].GetAttr(tpx, tpy);

		if ( (Obj.Class ==249 || Obj.Class == 247) && mchk )	// Guard
		{
			if ( (attr&2)!=2)
			{
				Obj.MTX = tpx;
				Obj.MTY = tpy;
				Obj.m_ActState.Move = 1;
				Obj.NextActionTime = 3000;
				return;
			}
		}
		else
		{
			if ( (attr&1)!=1 && (attr&2)!=2 && (attr&4)!=4 && (attr&8)!= 8 && mchk )
			{
				Obj.TargetNumber = -1;
				Obj.m_ActState.Attack = 0;
				Obj.NextActionTime = 500;
				Obj.m_ActState.Emotion = 0;
				Obj.MTX = tpx;
				Obj.MTY = tpy;
				Obj.m_ActState.Move = 1;
				return;
			}
		}
	}
}



void gObjMonsterBaseAct(CGameObject &Obj)
{
	CGameObject lpTargetObj = NULL;

	if ( Obj.TargetNumber >= 0 )
		lpTargetObj = getGameObject(Obj.TargetNumber);
	else
		Obj.m_ActState.Emotion = 0;

	if ( gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_SLEEP) == TRUE )
		return;

	if ( Obj.m_ActState.Emotion == 0 )
	{
		if ( Obj.m_Attribute )
		{
			if ( Obj.m_ActState.Attack )
			{
				Obj.m_ActState.Attack = 0;
				Obj.TargetNumber = -1;
				Obj.NextActionTime = 500;
			}

			int actcode1 = rand()%2;

			if ( Obj.m_Attribute == 100 )
				actcode1 = 1;
			
			if ( actcode1 == 0 )
			{
				Obj.m_ActState.Rest = 1;
				Obj.NextActionTime = 500;
			}

			if ( Obj.Class != 673 || Obj.VPCount2 >= 1 )
			{
				if ( Obj.m_MoveRange > 0 && !gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_STONE) && !gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_STUN) 
					&& !gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_SLEEP) && !gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_FREEZE_2) && !gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_EARTH_BINDS) )
				{
					if ( Obj.m_Attribute != 100 )
					{
						gObjMonsterMoveAction(lpObj);
					}

					else
					{
						int tx=0;
						int ty=0;

						if ( Obj.m_RecallMon >= 0 )
						{
							if ( getGameObject(Obj.m_RecallMon)->Connected > PLAYER_LOGGED )
							{
								CGameObject lpRecallObj = getGameObject(Obj.m_RecallMon);

								if ( lpRecallObj.m_Rest == FALSE )
								{
									if ( gObjGetTargetPos(lpObj, lpRecallObj.X, lpRecallObj.Y, tx, ty) == TRUE )
									{
										Obj.MTX = tx;
										Obj.MTY = ty;
										Obj.m_ActState.Move = 1;
										Obj.NextActionTime = 1000;
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
				float fMaxHP = Obj.MaxLife;
				float fCurrentHP = Obj.Life;
				int nAddLife = 10000;

				if ( Obj.MaxLife > Obj.Life )
				{
					Obj.Life += nAddLife;

					if ( Obj.MaxLife * 0.7 >= Obj.Life )
					{
						if ( Obj.MaxLife * 0.5 >= Obj.Life )
						{
							if ( Obj.MaxLife * 0.2 < Obj.Life )
							{
								Obj.m_nPhaseLordSilvester = 2;
							}
						}

						else
						{
							Obj.m_nPhaseLordSilvester = 1;
						}
					}

					else
					{
						Obj.m_nPhaseLordSilvester = 0;
					}

					if ( Obj.MaxLife < Obj.Life )
						Obj.Life = Obj.MaxLife;

				}

				if (g_bSilvesterEntranceBlock == true && (GetTickCount64() - g_i64LordSilvesterBlockTime >= 60000))
				{
					Obj.Life = Obj.MaxLife;

					g_bSilvesterEntranceBlock = false;
					g_i64LordSilvesterBlockTime = 0;

					UnBlockLordSilvesterEntrance();
					SendLordSilvesterBlockInfo(-1, 1);

				}

				if ( Obj.MaxLife == Obj.Life && gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_MONSTER_MAGIC_IMMUNE) == TRUE )
				{
					gObjRemoveBuffEffect(lpObj, BUFFTYPE_MONSTER_MAGIC_IMMUNE);

					for (int i = 0; i < 3; i++)
					{
						if ( Obj.m_nRecallMonIndex[i] != -1 )
						{
							gObjDel(Obj.m_nRecallMonIndex[i]);
							Obj.m_nRecallMonIndex[i] = -1;
						}
					}
				}
			}

			if (Obj.Class == 249 || Obj.Class == 247 )	// Guard
			{
				if ( g_ConfigRead.data.common.IsGuardSay )
				{
					if ( !(rand()%g_ConfigRead.data.common.GuardTalkChance) )
					{
						if ( Obj.m_PK_Count == 0 )
						{
							gGameProtocol.ChatSend(lpObj,(char *)g_ConfigRead.data.common.GuardSayText.c_str());	// Need Translation
						}
					}
				}

				int iTargetNumber = gObjGuardSearchEnemy(lpObj);
				Obj.TargetNumber = gObjGuardSearchEnemy(lpObj);

				if (iTargetNumber >= 0 )
				{
					if ( getGameObject(iTargetNumber)->Class >= 248 )
					{
						Obj.TargetNumber = -1;
					}
				}
			}
			else 
			{
				if ( Obj.m_Attribute == 100 )
				{
					int iTargetNumber = gObjMonsterSearchEnemy(lpObj, OBJ_MONSTER);

					if ( Obj.Class >= 678 && Obj.Class <= 680 )
					{
						iTargetNumber = gObjMonsterSearchEnemy(lpObj, OBJ_USER);
					}

					if ( IMPERIAL_MAP_RANGE(Obj.MapNumber) == TRUE )
					{
						if ( Obj.Live == FALSE || g_ImperialGuardian.IsAttackAbleMonster(iTargetNumber) == false )
						{
							Obj.TargetNumber = -1;
						}
					}

					if (g_ArcaBattle.IsArcaBattleServer() == TRUE)
					{
						if (g_AcheronGuardianEvent.IsPlayStart() == false)
						{
							if (Obj.m_RecallMon != -1 && ObjectMaxRange(iTargetNumber) == true)
							{
								if (g_ArcaBattle.IsEnableAttackObelisk(getGameObject(Obj.m_RecallMon], getGameObject(iTargetNumber)->Class) == FALSE)
								{
									Obj.TargetNumber = -1;
								}
							}
						}
					}
				}

				else
				{
					Obj.TargetNumber = gObjMonsterSearchEnemy(lpObj, OBJ_USER);
				}
			}

			if ( Obj.TargetNumber >= 0 )
			{
				Obj.m_ActState.EmotionCount = 30;
				Obj.m_ActState.Emotion = 1;
			}
		}
	}
	else if ( Obj.m_ActState.Emotion == 1 )
	{
		if ( Obj.Class == 673 )
		{
			g_bActiveUrukTrap = true;
			g_bSilvesterEntranceBlock = true;
			g_i64LordSilvesterBlockTime = GetTickCount64();

			BlockLordSilvesterEntrance();
			SendLordSilvesterBlockInfo(-1, 0);

		}

		if ( Obj.m_ActState.EmotionCount > 0 )
		{
			Obj.m_ActState.EmotionCount--;
		}
		else
		{
			Obj.m_ActState.Emotion = 0;
		}

		if ( Obj.TargetNumber >= 0 && Obj.PathStartEnd == 0)
		{
			if ( BC_MAP_RANGE(Obj.MapNumber) )
			{
				int iRAND_CHANGE_TARGET = rand()%10;

				if ( iRAND_CHANGE_TARGET == 3 )
					Obj.LastAttackerID = -1;

				if ( iRAND_CHANGE_TARGET == 1 )
				{
					if ( Obj.LastAttackerID != -1 && Obj.LastAttackerID != Obj.TargetNumber )
					{
						if ( getGameObject(Obj.LastAttackerID)->Connected > PLAYER_LOGGED &&
							Obj.MapNumber == getGameObject(Obj.LastAttackerID)->MapNumber )
						{
							Obj.TargetNumber = Obj.LastAttackerID;
							lpTargetObj = getGameObject(Obj.LastAttackerID);
						}
					}
				}
			}

			int dis = gObjCalDistance(lpObj, lpTargetObj);
			int attackrange;

			if ( Obj.m_AttackType >= 100 )	// #warning Change this To Level
				attackrange = Obj.m_AttackRange+2;
			else
				attackrange = Obj.m_AttackRange;

			if ( dis <= attackrange )
			{
				int tuser = Obj.TargetNumber;

				if (!ObjectMaxRange(tuser))
				{
					return;
				}

				int map = getGameObject(tuser)->MapNumber;
				BYTE attr;

				if ( MAX_MAP_RANGE(map) == FALSE )
				{
					sLog->outError( "[ERROR] MAX_MAP_RANGE (gObjMonster) == FALSE (%d)", map);
					return;
				}

				if ( MapC[map].CheckWall(Obj.X, Obj.Y, getGameObject(tuser)->X, getGameObject(tuser)->Y) == TRUE )
				{
					attr = MapC[map].GetAttr(getGameObject(tuser)->X, getGameObject(tuser)->Y);

					if ( (attr&1) != 1 )
					{
						Obj.m_ActState.Attack = 1;
					}
					else
					{
						Obj.TargetNumber = -1;
						Obj.m_ActState.EmotionCount = 30;
						Obj.m_ActState.Emotion = 1;
					}

					Obj.Dir = GetPathPacketDirPos(lpTargetObj.X-Obj.X, lpTargetObj.Y-Obj.Y);
					Obj.NextActionTime = Obj.m_AttackSpeed;
				}
			}
			else
			{
				if ( gObjMonsterGetTargetPos(lpObj) == TRUE )
				{
					if ( MAX_MAP_RANGE(Obj.MapNumber) == FALSE )
					{
						sLog->outError( "[ERROR] MAX_MAP_RANGE (gObjMonster) == FALSE (%d)", Obj.MapNumber);
						return;
					}

					if ( MapC[Obj.MapNumber].CheckWall(Obj.X, Obj.Y, Obj.MTX, Obj.MTY) == TRUE )
					{
						Obj.m_ActState.Move = 1;
						Obj.NextActionTime = 400;
						Obj.Dir = GetPathPacketDirPos(lpTargetObj.X-Obj.X, lpTargetObj.Y-Obj.Y);
					}
					else
					{
						gObjMonsterMoveAction(lpObj);
						Obj.m_ActState.Emotion = 3;
						Obj.m_ActState.EmotionCount = 10;

					}
				}
				else
				{
					gObjMonsterMoveAction(lpObj);
				}
			}
		}
	}
	else if ( Obj.m_ActState.Emotion == 2 )
	{
		if ( Obj.m_ActState.EmotionCount > 0 )
		{
			Obj.m_ActState.EmotionCount--;
		}
		else
		{
			Obj.m_ActState.Emotion = 0;
		}

		Obj.m_ActState.Move = 1;
		Obj.NextActionTime = 800;

		if ( lpTargetObj )
		{
			int tdir = GetPathPacketDirPos(lpTargetObj.X-Obj.X, lpTargetObj.Y-Obj.Y)*2;
			Obj.MTX += RoadPathTable[tdir] * (-3);
			Obj.MTY += RoadPathTable[tdir+1] * (-3);
		}
	}
	else if ( Obj.m_ActState.Emotion == 3 )
	{
		if ( Obj.m_ActState.EmotionCount > 0 )
		{
			Obj.m_ActState.EmotionCount--;
		}
		else
		{
			Obj.m_ActState.Emotion = 0;
		}

		Obj.m_ActState.Move = 0;
		Obj.m_ActState.Attack = 0;
		Obj.NextActionTime = 400;
	}
}



void gObjTrapAttackEnemySearchX(CGameObject &Obj, int count)
{
	Obj.TargetNumber = -1;
	int tObjNum;
	int pos = Obj.X;
	int y = Obj.Y;

	for(int n =0;n<count;n++)
	{
		pos++;
		for(int i =0; i< MaxViewportMonster; i++)
		{
			tObjNum = Obj.VpPlayer2[i].number;

			if(tObjNum >= 0)
			{
				if(getGameObject(tObjNum)->Type==OBJ_USER)
				{
					if(getGameObject(tObjNum)->Live)
					{
						if(y == getGameObject(tObjNum)->Y)
						{
							if(pos == getGameObject(tObjNum)->X)
							{
								if ( (getGameObject(tObjNum)->Authority &2) == 2 || (getGameObject(tObjNum)->Authority & 0x20) == 0x20)
								{
									if ( (getGameObject(tObjNum)->GameMaster & GM_MONSTERS_INVISIBLE) == GM_MONSTERS_INVISIBLE )
									{
										continue;
									}
								}

								Obj.TargetNumber = tObjNum;
								return;
							}
						}
					}
				}
			}
		}
	}
}




void gObjTrapAttackEnemySearchY(CGameObject &Obj, int count)
{
	Obj.TargetNumber = -1;
	int tObjNum;
	int pos = Obj.Y;
	int x = Obj.X;

	for(int n =0;n<count;n++)
	{
		pos--;
		for(int i =0; i< MaxViewportMonster; i++)
		{
			tObjNum = Obj.VpPlayer2[i].number;

			if(tObjNum >= 0)
			{
				if(getGameObject(tObjNum)->Type==1)
				{
					if(getGameObject(tObjNum)->Live)
					{
						if(pos == getGameObject(tObjNum)->Y)
						{
							if(x == getGameObject(tObjNum)->X)
							{
								if ( (getGameObject(tObjNum)->Authority &2) == 2 || (getGameObject(tObjNum)->Authority & 0x20) == 0x20)
								{
									if ( (getGameObject(tObjNum)->GameMaster & GM_MONSTERS_INVISIBLE) == GM_MONSTERS_INVISIBLE )
									{
										continue;
									}
								}
									
								Obj.TargetNumber = tObjNum;
								return;
							}
						}
					}
				}
			}
		}
	}
}



void gObjTrapAttackEnemySearch(CGameObject &Obj)
{
	int tObjNum;
	Obj.TargetNumber = -1;

	for(int i = 0; i < MaxViewportMonster;i++)
	{
		tObjNum = Obj.VpPlayer2[i].number;

		if(tObjNum >= 0)
		{
			if(getGameObject(tObjNum)->Type==1)
			{
				if(getGameObject(tObjNum)->Live)
				{
					if(Obj.Y == getGameObject(tObjNum)->Y)
					{
						if(Obj.X == getGameObject(tObjNum)->X)
						{
							if ( (getGameObject(tObjNum)->Authority &2) == 2 || (getGameObject(tObjNum)->Authority & 0x20) == 0x20)
							{
								if ( (getGameObject(tObjNum)->GameMaster & GM_MONSTERS_INVISIBLE) == GM_MONSTERS_INVISIBLE )
								{
									continue;
								}
							}
								
							Obj.TargetNumber = tObjNum;
							return;
						}
					}
				}
			}
		}
	}
}

void gObjTrapAttackEnemySearchRange(CGameObject &Obj,int iRange)
{
	int tObjNum = -1;
	int iTargetingRate = 0;
	int iSuccessRate = 0;

	Obj.TargetNumber = -1;

	if(Obj.VPCount <= 0)
	{
		return;
	}

	iTargetingRate = 100 / Obj.VPCount;

	for (int i = 0; i < MaxViewportMonster; i++)
	{
		tObjNum = Obj.VpPlayer2[i].number;

		if(tObjNum >= 0)
		{
			if(getGameObject(tObjNum)->Type==1)
			{
				if(getGameObject(tObjNum)->Live)
				{
					if((Obj.Y - iRange) <= getGameObject(tObjNum)->Y && (Obj.Y + iRange) >= getGameObject(tObjNum)->Y)
					{
						if((Obj.X - iRange) <= getGameObject(tObjNum)->X && (Obj.X + iRange) >= getGameObject(tObjNum)->X)
						{
							if ( (getGameObject(tObjNum)->Authority &2) == 2 || (getGameObject(tObjNum)->Authority & 0x20) == 0x20)
							{
								if ( (getGameObject(tObjNum)->GameMaster & GM_MONSTERS_INVISIBLE) == GM_MONSTERS_INVISIBLE )
								{
									continue;
								}
							}
								
							Obj.TargetNumber = tObjNum;
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

void gObjMonsterTrapAct(CGameObject &Obj)
{
	if(Obj.VPCount2 < 1)
	{
		return;
	}

	if(Obj.m_AttackRange > 0)
	{
		if(Obj.Dir == 3)
		{
			gObjTrapAttackEnemySearchX(lpObj,Obj.m_AttackRange+1);
		}

		else if(Obj.Dir == 1)
		{
			gObjTrapAttackEnemySearchY(lpObj,Obj.m_AttackRange+1);
		}

		else if(Obj.Dir == 8)
		{
			gObjTrapAttackEnemySearchRange(lpObj,Obj.m_AttackRange);
		}

		if (Obj.Class == 689)
		{
			gObjTrapAttackEnemySearchRange(lpObj, Obj.m_AttackRange);
		}
	}
	else
	{
		gObjTrapAttackEnemySearch(lpObj);
	}

	if(Obj.TargetNumber >= 0)
	{
		Obj.m_ActState.Attack = 1;
		Obj.NextActionTime = Obj.m_AttackSpeed;
	}
	else
	{
		Obj.NextActionTime = Obj.m_MoveSpeed;
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

void InventoryDropItem(CGameObject &Obj, int DropItem)
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

void gObjRefillMonsterHP(CGameObject &MonsterObj, int iRefillHPSec)
{
	int iMonsterHP = lpMonsterObj.Life;
	int iRefillHP = 0;

	if(lpMonsterObj.Live==0)
	{
		return;
	}

	if(lpMonsterObj.Class == 275)
	{
	CGameObject lpObj;
		if(lpMonsterObj.iObjectSecTimer > giKundunRefillHPTime)
		{
			iRefillHP = rand() % ((giKundunRefillHP > 0)? giKundunRefillHP : 1) ;

			lpMonsterObj.iObjectSecTimer = 0;
		}
		else
		{
			lpMonsterObj.iObjectSecTimer++;
		}

		if(lpMonsterObj.iObjectSecTimer % giKundunHPLogSaveTime == 0)
		{
			if(gObjIsConnected(lpMonsterObj.LastAttackerID))
			{
				lpObj = getGameObject(lpMonsterObj.LastAttackerID);
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

	if(lpMonsterObj.MaxLife < iMonsterHP)
	{
		lpMonsterObj.Life = lpMonsterObj.MaxLife;
	}
	else
	{
		lpMonsterObj.Life = iMonsterHP;
	}
}

int gObjMonsterSelectSkillForMedusa(CGameObject &Obj)
{
	int nTargetCnt = 0;
	int nSplashTargetCnt = 0;
	

	for (int i = 0; i < MAX_VIEWPORT; i++ )
	{
		int nObjNum = Obj.VpPlayer2[i].number;

		if ( nObjNum < 0 )
			break;

		if ( getGameObject(nObjNum)->Type == OBJ_USER && getGameObject(nObjNum)->Live == TRUE )
			nTargetCnt++;

		int iTargetNumber = Obj.TargetNumber;

		if ( ObjectMaxRange(iTargetNumber) )
		{
			if ( gObjCalDistance(getGameObject(iTargetNumber), getGameObject(nObjNum)) <= 3 )
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

void gObjMonsterDieGiveItem(CGameObject &Obj, CGameObject lpTargetObj)
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
	BYTE NOption=0;
	BOOL item_drop=0;
	int n;
	CItemObject * DropItem=NULL;

	if ( gObjCheckUsedBuffEffect(lpTargetObj, BUFFTYPE_HACKTOOL_PENALTY) == TRUE )
	{
		return;
	}

	if ( Obj.MapNumber == MAP_INDEX_DEVILSQUARE_FINAL )
	{
		return;
	}

	g_KillCountMng.CheckMonsterKillCount(Obj.Class, lpTargetObj);
	g_CashShop.AddGPMonster(lpObj);

	if (g_MonsterRegenSystem.MonsterKillCheck(lpTargetObj, lpObj))
	{
		return;
	}

	if ( g_QuestInfo.MonsterItemDrop(lpObj) ) //Third Quest from Season3 Monster Fix
		return;

	if( g_NewPVP.IsVulcanusMonster(Obj.Class) )
	{
		if( g_NewPVP.DropItem(lpTargetObj, lpObj) )
		{
			return;
		}
	}

	if (Obj.MapNumber == MAP_INDEX_ACHERON)
	{
		if (g_ArcaBattle.DropItem(lpTargetObj, lpObj))
		{
			return;
		}
	}

	if (g_BagManager.IsBag(lpTargetObj.m_Index, BAG_MONSTER, Obj.Class, Obj.m_Index) == true)
	{
		int iMaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

		if (iMaxHitUser == -1)
		{
			iMaxHitUser = lpTargetObj.m_Index;
		}

		if (g_BagManager.SearchAndUseBag(iMaxHitUser, BAG_MONSTER, Obj.Class, Obj.m_Index) == true)
		{
			return;
		}
	}

	if ( Obj.Class >= 598 && Obj.Class <= 602 )
	{
		return;
	}

	if (Obj.Class == 681 || Obj.Class == 690)
	{
		return;
	}

	if (Obj.Class == 734 || Obj.Class == 746) // Lord Ferea + Boss Nixie Lake
	{
		return;
	}

	if (Obj.Class >= 627 && Obj.Class <= 631)
	{
		int nMaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

		if (nMaxHitUser == -1)
		{
			nMaxHitUser = lpTargetObj.m_Index;
		}

		sLog->outBasic("[AcheronGuardianEvent] BossName: %s, CharacterName: %s", Obj.Name, getGameObject(nMaxHitUser)->Name);
		g_AcheronGuardianEvent.DestroyObelisk(getGameObject(nMaxHitUser), Obj.MapNumber, Obj.X, Obj.Y, nMaxHitUser);
		return;
	}

	if ( Obj.Class >= 504 && Obj.Class <= 511 )
	{
		return;
	}

	if ( DG_MAP_RANGE(Obj.MapNumber) )
	{
		return;
	}

	if ( Obj.Class == 523 )
	{
		return;
	}

	if ( Obj.Class >= 523 && Obj.Class <= 528 )
	{
		return;
	}

	if (g_MineSystem.IsTwinkle(Obj.Class))
	{
		return;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (Obj.m_btCsNpcType != 0)
		{
			return;
		}
	}

	if (Obj.Class == 561) // Medusa 
	{
		g_BagManager.SearchAndUseBag(lpTargetObj.m_Index, BAG_EVENT, EVENTBAG_MEDUSA, lpTargetObj.m_Index);
		return;
	}

	if ( Obj.Class == 275 ) // Kundun 
	{
		g_BagManager.SearchAndUseBag(lpTargetObj.m_Index, BAG_EVENT, EVENTBAG_KUNDUN, lpTargetObj.m_Index);
		return;
	}
	
	if ( Obj.Class == 249 || lpTargetObj.Class == 249 ||	// Guard
		Obj.Class == 247 || lpTargetObj.Class == 247 )	// Guard
	{
		return;
	}

	if ( Obj.m_RecallMon >= 0 )
	{
		return;
	}
	
	if ( Obj.Class == 131 )	// Castle Gate
	{
		return;
	}

	if (Obj.Class == 283)
	{
		return;
	}

	if (Obj.Class == 288)
	{
		return;
	}

	if (Obj.Class == 278)
	{
		return;
	}

	if (Obj.Class == 460 || Obj.Class == 461 || Obj.Class == 462) // Season 4.5 Addon
	{
		return;
	}
	
	if ( BC_STATUE_RANGE(Obj.Class-132) )	// Blood Castle Statue
	{
		return;
	}
	
	if ( CC_MAP_RANGE(Obj.MapNumber) )
	{
		g_ChaosCastle.SearchNDropMonsterItem(g_ChaosCastle.GetChaosCastleIndex(Obj.MapNumber), Obj.m_Index, lpTargetObj.m_Index);
		
		gObjDel(Obj.m_Index);
		
		return;
	}

	if (Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL && g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		g_ChaosCastleFinal.SearchCCFNDropMonsterItem(Obj.m_Index, lpTargetObj.m_Index);

		gObjDel(Obj.m_Index);

		return;
	}
	
	if ( Obj.m_bIsInMonsterHerd )
	{
		MonsterHerd * lpMH = Obj.m_lpMonsterHerd;

		if ( lpMH )
		{
			if ( lpMH->MonsterHerdItemDrop(lpObj) )
			{
				return;
			}
		}
	}
	
	int itemrate = Obj.m_ItemRate;
	int moneyrate = Obj.m_MoneyRate;
	if (itemrate < 1)
		itemrate = 1;

//	if (itemrate > 100)
//		itemrate = 100;

	if (moneyrate < 1)
		moneyrate = 1;

	if (Obj.Class == 43 ) // GoldenBudgeDragon
	{
		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenBudgeDragon, Obj.m_Index, Obj.MapNumber, Obj.X, Obj.Y);
		return;
	}
	if ( Obj.Class == 53) // GoldenTitan
	{
		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenTitan, Obj.m_Index, Obj.MapNumber, Obj.X, Obj.Y);
		return;
	}
	if (Obj.Class == 54) // GoldenSoldier
	{
		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenSoldier, Obj.m_Index, Obj.MapNumber, Obj.X, Obj.Y);
		return;
	}
	if ( Obj.Class == 55 )	// Death King
	{

	}
	if (Obj.Class == 78)	// GoldenGoblin
	{
		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenGoblin, Obj.m_Index, Obj.MapNumber, Obj.X, Obj.Y);
		return;
	}
	if (Obj.Class == 502)	// GoldenRabbit
	{
		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenRabbit, Obj.m_Index, Obj.MapNumber, Obj.X, Obj.Y);
		return;
	}
	if (Obj.Class == 493)	// GoldenDarkKnight
	{
		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenDarkKnight, Obj.m_Index, Obj.MapNumber, Obj.X, Obj.Y);
		return;
	}
	if (Obj.Class == 79)	// GoldenDerkon
	{
		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenDerkon, Obj.m_Index, Obj.MapNumber, Obj.X, Obj.Y);
		return;
	}
	if (Obj.Class == 501)	// GoldenGreatDragon
	{
		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenGreatDragon, Obj.m_Index, Obj.MapNumber, Obj.X, Obj.Y);
		return;
	}
	if (Obj.Class == 80)	// GoldenLizard
	{

		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenLizard, Obj.m_Index, Obj.MapNumber, Obj.X, Obj.Y);
		return;
	}
	if (Obj.Class == 81)	// GoldenVepar
	{

		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenVepar, Obj.m_Index, Obj.MapNumber, Obj.X, Obj.Y);
		return;
	}
	if (Obj.Class == 494)	// GoldenDevil
	{

		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenDevil, Obj.m_Index, Obj.MapNumber, Obj.X, Obj.Y);
		return;
	}
	if (Obj.Class == 82)	// GoldenTantalos
	{

		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenTantalos, Obj.m_Index, Obj.MapNumber, Obj.X, Obj.Y);
		return;
	}
	if (Obj.Class == 83)	// GoldenWheel
	{

		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenWheel, Obj.m_Index, Obj.MapNumber, Obj.X, Obj.Y);
		return;
	}
	if (Obj.Class == 495)	// GoldenStoneGolem
	{

		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenStoneGolem, Obj.m_Index, Obj.MapNumber, Obj.X, Obj.Y);
		return;
	}
	if (Obj.Class == 496)	// GoldenCrust
	{

		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenCrust, Obj.m_Index, Obj.MapNumber, Obj.X, Obj.Y);
		return;
	}
	if (Obj.Class == 497)	// GoldenSatyros
	{

		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenSatyros, Obj.m_Index, Obj.MapNumber, Obj.X, Obj.Y);
		return;
	}
	if (Obj.Class == 498)	// GoldenTwinTail
	{

		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenTwinTail, Obj.m_Index, Obj.MapNumber, Obj.X, Obj.Y);
		return;
	}
	if (Obj.Class == 499)	// GoldenIronKnight
	{

		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenIronKnight, Obj.m_Index, Obj.MapNumber, Obj.X, Obj.Y);
		return;
	}
	if (Obj.Class == 500)	// GoldenNapin
	{

		g_EventItemBagManager.OpenSpecial(EventBagSpecial::GoldenNapin, Obj.m_Index, Obj.MapNumber, Obj.X, Obj.Y);
		return;
	}

	if (Obj.Class == 652)
	{
		g_BagManager.SearchAndUseBag(lpTargetObj.m_Index, BAG_EVENT, EVENTBAG_GOLDCOLOSSUS_GOBLIN, lpTargetObj.m_Index);
		return;
	}

	if (Obj.Class == 653)
	{
		g_BagManager.SearchAndUseBag(lpTargetObj.m_Index, BAG_EVENT, EVENTBAG_GOLDCOLOSSUS_TITAN, lpTargetObj.m_Index);
		return;
	}

	if (Obj.Class == 654)
	{
		g_BagManager.SearchAndUseBag(lpTargetObj.m_Index, BAG_EVENT, EVENTBAG_GOLDCOLOSSUS_TANTALUS, lpTargetObj.m_Index);
		return;
	}

	if (Obj.Class == 655)
	{
		g_BagManager.SearchAndUseBag(lpTargetObj.m_Index, BAG_EVENT, EVENTBAG_GOLDCOLOSSUS_EROHIM, lpTargetObj.m_Index);
		return;
	}

	if (Obj.Class == 673)
	{
		g_BagManager.SearchAndUseBag(lpTargetObj.m_Index, BAG_EVENT, EVENTBAG_LORDSILVESTER, lpTargetObj.m_Index);
		return;
	}

	if (Obj.Class == 656)
	{
		g_BagManager.SearchAndUseBag(lpTargetObj.m_Index, BAG_EVENT, EVENTBAG_GOLDCOLOSSUS_HELLMAINE, lpTargetObj.m_Index);

		int iWingCount = 0;
		int iWingExpireTime = 0;
		g_ConfigRead.m_ItemCalcLua.Generic_Call("GoldHellMaine_DropWing", ">ii", &iWingCount, &iWingExpireTime);

		if (iWingCount == 0)
		{
			return;
		}

		int iLootIndex = lpTargetObj.m_Index;

		for each (std::pair<int,CGameObject*> ObjEntry in gGameObjects)
		{
			if (getGameObject(i)->Connected == PLAYER_PLAYING && getGameObject(i)->m_SummonCharDBNum == Obj.DBNumber)
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
			ItemCreate(lpTargetObj.m_Index, lpTargetObj.MapNumber, lpTargetObj.X, lpTargetObj.Y, ITEMGET(12, 267), 0, 0, 0, 0, 0, iLootIndex, iExOption, 0, iWingExpireTime, btNewExcOption, 0);
		}

		return;
	}

	if (Obj.Class == 657)
	{
		g_BagManager.SearchAndUseBag(lpTargetObj.m_Index, BAG_EVENT, EVENTBAG_GOLDCOLOSSUS_KUNDUN, lpTargetObj.m_Index);

		int iWingCount = 0;
		int iWingExpireTime = 0;
		g_ConfigRead.m_ItemCalcLua.Generic_Call("GoldKundun_DropWing", ">ii", &iWingCount, &iWingExpireTime);

		if (iWingCount == 0)
		{
			return;
		}

		int iLootIndex = lpTargetObj.m_Index;

		for each (std::pair<int,CGameObject*> ObjEntry in gGameObjects)
		{
			if (getGameObject(i)->Connected == PLAYER_PLAYING && getGameObject(i)->m_SummonCharDBNum == Obj.DBNumber)
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
			ItemCreate(lpTargetObj.m_Index, lpTargetObj.MapNumber, lpTargetObj.X, lpTargetObj.Y, ITEMGET(12, 267), 0, 0, 0, 0, 0, iLootIndex, iExOption, 0, iWingExpireTime, btNewExcOption, 0);
		}

		return;
	}

	if ( gEventMonsterItemDrop(lpObj, lpTargetObj) )
		return;

	if (g_CAppointItemDrop.AppointItemDrop(lpTargetObj, lpObj))
		return;

	if ( g_PentagramSystem.ElementDrop(lpObj, lpTargetObj))
		return;

	if (Obj.MapNumber == MAP_INDEX_ACHERON || Obj.MapNumber == MAP_INDEX_DEBENTER || Obj.MapNumber == MAP_INDEX_NARS || Obj.MapNumber == MAP_INDEX_URUK_MOUNTAIN)
	{
		BYTE MithrilSocketBonus = (Obj.m_iPentagramMainAttribute == ELEMENT_NONE) ? (1 + rand() % 5) : Obj.m_iPentagramMainAttribute;

		if (rand() % 10000 < g_PentagramSystem.m_iMithril_DropRate)
		{
			ItemCreate(lpTargetObj.m_Index, lpTargetObj.MapNumber, lpTargetObj.X, lpTargetObj.Y, ITEMGET(12, 144), 0, 1, 0, 0, 0, lpTargetObj.m_Index, 0, 0, 0, 0, MithrilSocketBonus);
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

	if ( lpTargetObj.Level <= 20 )
	{
		if ( (rand()%10000) < 2000 )
		{
			int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);
			x = Obj.X;
			y = Obj.Y;
			dur = 1.0f;
			level = 0;
			type = ItemGetNumberMake(14, 0);	// Apple
			ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, level, dur,
				Option1, Option2, Option3, MaxHitUser, 0, 0, 0, 0, 0);
		}
	}

	int iExcDropRate = g_MaxStatsInfo.GetGeneralDrop.ExCItemObjectDropPer + g_VipSystem.GetExcDropBonus(lpTargetObj) + g_BonusEvent.GetAddExcDrop() + g_GensSystem.GetBattleZoneExcDropBonus(lpTargetObj);

	if (lpTargetObj.Type == OBJ_USER)
	{
		if (rand() % 10000 < iExcDropRate)
		{
			ExtDropPer = TRUE;
		}
	}

	int ItemDropPer = g_MaxStatsInfo.GetGeneralDrop.ItemDropPer + g_VipSystem.GetDropBonus(lpTargetObj) + g_BonusEvent.GetAddDrop() + g_GensSystem.GetBattleZoneDropBonus(lpTargetObj);
	
	if(lpTargetObj.Type == OBJ_USER)
	{
		ItemDropPer += g_MaxStatsInfo.GetGeneralDrop.ItemDropPer * lpTargetObj.m_PlayerData->SetOpImproveItemDropRate / 100;
	}

	ItemDropPer += ItemDropPer * (gObjGetTotalValueOfEffect(lpTargetObj, EFFECTTYPE_ITEMDROPRATE) / 100.0f); // 7 - ADD_OPTION_DROP_RATE
	ItemDropPer += g_MapAttr.GetItemDropBonus(lpTargetObj.MapNumber);

	if(lpTargetObj.pntInventory[10]->m_Type == ITEMGET(13,134))
		ItemDropPer += 20;
	if(lpTargetObj.pntInventory[11]->m_Type == ITEMGET(13,134))
		ItemDropPer += 20;

	if ( ExtDropPer == TRUE )
	{
		DropItem = g_MonsterItemMng.GetItemEx(Obj.Level - 25);

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
			if(lpTargetObj.MapNumber == MAP_INDEX_VULCAN && Obj.MapNumber == MAP_INDEX_VULCAN
				&& gObjCheckUsedBuffEffect(lpTargetObj, BUFFTYPE_GLORYOFGLADIATOR))
			{
				DropItem = g_MonsterItemMng.GetItemEx(Obj.Level + 25);
			}
			else
			{
				DropItem = g_MonsterItemMng.GetItemEx(Obj.Level);
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

		if ( DS_MAP_RANGE(Obj.MapNumber ) )
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

			if ( g_SocketOptionSystem.IsEnableDropSocketItemMap(Obj.MapNumber) == false )
			{
				item_drop = FALSE;
			}
		}

		if (g_kItemSystemFor380.Is380Item(DropItem) == true)
		{
			if (g_ConfigRead.data.common.Item380DropMap != -1)
			{
				if (g_ConfigRead.data.common.Item380DropMap != Obj.MapNumber)
				{
					item_drop = FALSE;
				}
			}
		}

		if (IsBloodAngelItem(DropItem->m_Type) == true)
		{
			if (ExtDropPer == TRUE)
			{
				if (Obj.MapNumber != MAP_INDEX_NARS)
				{
					item_drop = FALSE;
				}
			}

			else
			{
				if (Obj.MapNumber != MAP_INDEX_DEBENTER && Obj.MapNumber != MAP_INDEX_DEBENTER_ARCA_WAR && Obj.MapNumber != MAP_INDEX_URUK_MOUNTAIN)
				{
					item_drop = FALSE;
				}
			}
		}

		if ( DropItem->m_Type == ITEMGET(13,14) && Obj.MapNumber != MAP_INDEX_ICARUS) // Loch Feather
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

		x = Obj.X;
		y = Obj.Y;

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
			NOption = NewOptionRand2(g_MaxStatsInfo.GetExcellentDrop.MonsterMinOptionCount, g_MaxStatsInfo.GetExcellentDrop.MonsterMaxOptionCount); //NewOptionRand(Obj.Level, NOptionCount);
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

		if ( Obj.Class == 43 ) // Golden Budge Dragon
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
			ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, level, dur,
				Option1, Option2, Option3, MaxHitUser, NOption, 0, 0, btSocketOption, 0);
		}
		else
		{
			MapC[Obj.MapNumber].MonsterItemDrop(type, level, dur, x, y, 
				Option1, Option2, Option3, NOption, 0, MaxHitUser, 0, 0, 0, -1, 0);
		}

	}
	else if (Obj.MonsterMoneyDrop < 1)
	{
		return;
	}
	else if ((rand() % moneyrate) < 10)
	{
		int x = Obj.X;
		int y = Obj.Y;
		double money = Obj.MonsterMoneyDrop * g_ConfigRead.data.common.gAddZenDiv;
		//
		if (lpTargetObj.m_PlayerData->DbClass == CLASS_WIZARDEVO || lpTargetObj.m_PlayerData->DbClass == CLASS_KNIGHTEVO || lpTargetObj.m_PlayerData->DbClass == CLASS_ELFEVO || lpTargetObj.m_PlayerData->DbClass == CLASS_MAGICEVO ||
			lpTargetObj.m_PlayerData->DbClass == CLASS_DARKLORDEVO || lpTargetObj.m_PlayerData->DbClass == CLASS_SUMMONEREVO || lpTargetObj.m_PlayerData->DbClass == CLASS_RAGEFIGHTEREVO || lpTargetObj.m_PlayerData->DbClass == CLASS_GROWLANCEREVO)
		{
			money += (money / 100.0f) * lpTargetObj.MonsterDieGetMoney;
			money = money * g_ConfigRead.data.common.gAddZenML;
		}
		else
		{
			money += (money / 100.0f) * lpTargetObj.MonsterDieGetMoney;
			money = money * g_ConfigRead.data.common.gAddZen;
		}

		if (lpTargetObj.Type == OBJ_USER)
		{
			if (lpTargetObj.m_PlayerData->m_MPSkillOpt->iMpsPlusZen > 0.0)
			{
				money += money * lpTargetObj.m_PlayerData->m_MPSkillOpt->iMpsPlusZen / 100.0;
			}
		}

		money += money * g_MAP_SETTINGS[lpTargetObj.MapNumber].drop_zen_increase / 100;

		if (money < 100.0f)
		{
			money = 100.0f;
		}

		if (money > 15000000.0f)
		{
			money = 15000000.0f;
		}

		if (DS_MAP_RANGE(Obj.MapNumber))
		{
			int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

			if (MaxHitUser == -1)
			{
				return;
			}

			getGameObject(MaxHitUser)->m_nEventMoney += (__int64)money;
		}
		else
		{
			MapC[Obj.MapNumber].MoneyItemDrop(money, x, y);
		}
	}

	if ( Obj.MonsterMoneyDrop < 1 )
		return;

	if ( DS_MAP_RANGE(Obj.MapNumber) )
		return;
	
	if ( (rand()%400) == 1 )
	{
		for ( n=0;n<4;n++)
		{
			int x = Obj.X-2;
			int y = Obj.Y-2;
			x+= rand()%3;
			y+= rand()%3;
			int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);
			if (MaxHitUser != -1 && getGameObject(MaxHitUser)->Type == OBJ_USER)
			{
				if (g_ConfigRead.data.common.joinmuDropItemUnderCharacter[getGameObject(MaxHitUser)->Class) == true)
				{
					MapC[Obj.MapNumber].MoneyItemDrop(Obj.MonsterMoneyDrop, getGameObject(MaxHitUser)->X, getGameObject(MaxHitUser)->Y);
				}
				else
				{
					MapC[Obj.MapNumber].MoneyItemDrop(Obj.MonsterMoneyDrop, x, y);
				}
			}
			else
			{
				MapC[Obj.MapNumber].MoneyItemDrop(Obj.MonsterMoneyDrop, x, y);
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
				x = Obj.X;
				y = Obj.Y;
				level = 1;
				ItemCreate(Obj.m_Index, Obj.MapNumber, x, y,
					type, level, dur, 0, 0, 0, -1, 0, 0, 0, 0, 0);
			}
		}
	}

}


BOOL gEventMonsterItemDrop(CGameObject &Obj, CGameObject lpTargetObj)
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

		if ( Obj.Level >= g_iFenrirStuff_01_DropLv_Min && Obj.Level <= g_iFenrirStuff_01_DropLv_Max &&
			Obj.MapNumber == g_iFenrirStuff_01_DropMap &&
			bDropStuff == FALSE )
		{
			if ( (rand()%10000) < g_iFenrirStuff_01_DropRate ) 
			{
				bDropStuff = TRUE;
				type = ItemGetNumberMake(13, 32);
			}
		}

		if ( Obj.Level >= g_iFenrirStuff_02_DropLv_Min && Obj.Level <= g_iFenrirStuff_02_DropLv_Max &&
			Obj.MapNumber == g_iFenrirStuff_02_DropMap &&
			bDropStuff == FALSE )
		{
			if ( (rand()%10000) < g_iFenrirStuff_02_DropRate ) 
			{
				bDropStuff = TRUE;
				type = ItemGetNumberMake(13, 33);
			}
		}

		if ( Obj.Level >= g_iFenrirStuff_03_DropLv_Min && Obj.Level <= g_iFenrirStuff_03_DropLv_Max &&
			Obj.MapNumber == g_iFenrirStuff_03_DropMap &&
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
			x = Obj.X;
			y = Obj.Y;
			dur = 1.0f;
			int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

			ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, level, dur,
				Option1, Option2, Option3, MaxHitUser, 0, 0, 0, 0, 0);

			return TRUE;
		}
	}

	int SocketSphereItemType = 0;
	SocketSphereItemType = g_SocketOptionSystem.GetSphereDropInfo(Obj.Level);

	if (SocketSphereItemType != -1)// Season 4.5 addon
	{
		int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

		ItemCreate(Obj.m_Index, Obj.MapNumber, Obj.X, Obj.Y, SocketSphereItemType, 0, 1, 0, 0, 0, MaxHitUser, 0, 0, 0, 0, 0);

		return TRUE;
	}

	int TetraCrystalItemType = 0;
	TetraCrystalItemType = g_SocketOptionSystem.GetTetraDropInfo(Obj.Level);

	if (TetraCrystalItemType != -1)// Season 9 addon
	{
		int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

		ItemCreate(Obj.m_Index, Obj.MapNumber, Obj.X, Obj.Y, TetraCrystalItemType, 0, 1, 0, 0, 0, MaxHitUser, 0, 0, 0, 0, 0);

		return TRUE;
	}

	if (g_ConfigRead.g_CompoundPotionDropOn == TRUE )
	{
		BOOL bDropStuff = FALSE;
		int iType = 0;

		if ( Obj.Level >= g_ConfigRead.g_iCompoundPotionLv1DropLevel && (rand()%10000) <= g_ConfigRead.g_iCompoundPotionLv1DropRate &&
			bDropStuff == FALSE )
		{
			bDropStuff = TRUE;
			iType = ItemGetNumberMake(14, 38);
		}

		if ( bDropStuff == FALSE && Obj.Level >= g_ConfigRead.g_iCompoundPotionLv2DropLevel && (rand()%10000) <= g_ConfigRead.g_iCompoundPotionLv2DropRate  )
		{
			bDropStuff = TRUE;
			iType = ItemGetNumberMake(14, 39);
		}
		

		if ( !bDropStuff )
		{
			if ( Obj.Level >= g_ConfigRead.g_iCompoundPotionLv3DropLevel && (rand()%10000) <= g_ConfigRead.g_iCompoundPotionLv3DropRate )
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

			ItemCreate(Obj.m_Index, Obj.MapNumber, Obj.X, Obj.Y, iType, level, dur,
				0,0,0, iMaxHitUser, 0, 0, 0, 0, 0);

			return TRUE;
		}
	}

	if ( gIsItemDropRingOfTransform )
	{
		if ( (rand()%10000) < gItemDropRingOfTransform )
		{
			LPMONSTER_ATTRIBUTE lpattr = gMAttr.GetAttr(Obj.Class);

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
				x = Obj.X;
				y = Obj.Y;
				level = callbead_level;
				type = ItemGetNumberMake(13, 10);
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, level, dur,
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
			int partycount = gParty.GetPartyCount(getGameObject(MaxHitUser)->PartyNumber);

			if ( partycount > 0 )
			{
				LPMONSTER_ATTRIBUTE lpattr = gMAttr.GetAttr(Obj.Class);

				if ( lpattr )
				{
					if ( lpattr->m_Level >= 17 )
					{
						if ( (rand()%10000) < gFireCrackerDropRate )
						{
							dur = 255.0f;
							x = Obj.X;
							y = Obj.Y;
							level = 2;
							type = ItemGetNumberMake(14, 11);

							ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, level, dur,
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
		x = Obj.X;
		y = Obj.Y;
		int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

		ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, 0, 0,
			0, 0, 0, MaxHitUser, 0, 0, 0, 0, 0);

		return TRUE;
	}

	if ( rand()%10000 < gGoldBoxDropRate )
	{
		type = ItemGetNumberMake(14,121);
		x = Obj.X;
		y = Obj.Y;
		int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

		ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, 0, 0,
			0, 0, 0, MaxHitUser, 0, 0, 0, 0, 0);

		return TRUE;
	}

	if (g_CMuRummyMng.IsMuRummyEventOn() == true)
	{
		if (rand() % 10000 < g_CMuRummyMng.GetMuRummyEventItemDropRate())
		{
			type = ItemGetNumberMake(14, 215);
			x = Obj.X;
			y = Obj.Y;
			int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

			ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, 0, 1.0,
				0, 0, 0, MaxHitUser, 0, 0, 0, 0, 0);

			return TRUE;
		}
	}

	if ( gHeartOfLoveEvent )
	{
		if ( Obj.MapNumber < 7 )
		{
			LPMONSTER_ATTRIBUTE lpattr = gMAttr.GetAttr(Obj.Class);

			if ( lpattr )
			{
				if ( lpattr->m_Level >=17 )
				{
					if ( (rand()%10000) < gHeartOfLoveDropRate )
					{
						dur = 255.0f;
						x = Obj.X;
						y = Obj.Y;
						level = 3;
						type = ItemGetNumberMake(14, 11);
						int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

						ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, level, dur,
							Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

						return TRUE;
					}
				}
			}
		}
	}

	if ( gMedalEvent )
	{
		if ( Obj.MapNumber == 1 || Obj.MapNumber == 2 )	// Silver Medal
		{
			if ( (rand()%10000) < gSilverMedalDropRate )
			{
				dur = 255.0f;
				x = Obj.X;
				y = Obj.Y;
				level = 5;
				type = ItemGetNumberMake(14, 11);
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, level, dur,
					Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
		else if ( Obj.MapNumber == 4 || Obj.MapNumber == 7 || Obj.MapNumber == 8 )	// Gold Medal
		{
			if ( (rand()%10000) < gGoldMedalDropRate )
			{
				dur = 255.0f;
				x = Obj.X;
				y = Obj.Y;
				level = 6;
				type = ItemGetNumberMake(14, 11);
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, level, dur,
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
			x = Obj.X;
			y = Obj.Y;
			level = 7;
			type = ItemGetNumberMake(14, 11);
			int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

			ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, level, dur,
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
		x = Obj.X;
		y = Obj.Y;
		level = 0;

		if ( Obj.Level < 25 )
			level = 0;
		else if ( Obj.Level < 47 )
			level = 1;
		else if ( Obj.Level < 66 )
			level = 2;
		else if ( Obj.Level < 78 )
			level = 3;
		else if ( Obj.Level < 84 )
			level = 4;
		else if ( Obj.Level < 92 )
			level = 5;
		else if (Obj.Level < 114)
			level = 6;
		else if (Obj.Level > 115 && Obj.Level < g_ConfigRead.data.common.UserMaxLevel + 1)
			level = 7;

		if ( level == 0 )
			return FALSE;

		type = ItemGetNumberMake(14, 29);
		int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);
		ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, level, dur,
			Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

		return TRUE;
	}

	if ((rand() % 10000) < g_DoppelGanger.GetMirrorDimensionDropRate() && Obj.Level >= g_DoppelGanger.GetMirrorDimensionDropLevel())
	{
		x = Obj.X;
		y = Obj.Y;
		dur = 1.0f;
		type = ItemGetNumberMake(14, 110);
		int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);
		ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, 0, dur,
			0, 0, 0, MaxHitUser, 0, 0, 0, 0, 0);

		return true;
	}

	if ((rand() % 10000) < g_ImperialGuardian.GetPaperScrapDropRate() && Obj.Level >= g_ImperialGuardian.GetPaperScrapDropLevel())
	{
		x = Obj.X;
		y = Obj.Y;
		dur = 1.0f;
		type = ItemGetNumberMake(14, 101);
		int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);
		ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, 0, dur,
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
		x = Obj.X;
		y = Obj.Y;
		type = ItemGetNumberMake(14, 21);
		int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

		ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, level, dur,
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
		x = Obj.X;
		y = Obj.Y;
		type = ItemGetNumberMake(14, 100);
		int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

		ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, level, dur,
			Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

		return TRUE;
	}

	if ( gIsDropDarkLordItem )
	{
		if ( Obj.Level >= gSleeveOfLordDropLevel )
		{
			if ( (rand()%10000) < gSleeveOfLordDropRate)
			{
				type = ItemGetNumberMake(13,14);
				level = 1;
				x = Obj.X;
				y = Obj.Y;
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, level, dur,
					Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}

		if ( Obj.Level >= gSoulOfDarkHorseropLevel )
		{
			if ( (rand()%10000) < gSoulOfDarkHorseDropRate)
			{
				type = ItemGetNumberMake(13,31);
				level = 0;
				x = Obj.X;
				y = Obj.Y;
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, level, dur,
					Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}

		if ( Obj.Level >= gSoulOfDarkSpiritDropLevel )
		{
			if ( (rand()%10000) < gSoulOfDarkSpiritDropRate )
			{
				type = ItemGetNumberMake(13,31);
				level = 1;
				x = Obj.X;
				y = Obj.Y;
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, level, dur,
					Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
	}

	if ( gIsDropGemOfDefend )
	{
		if ( Obj.MapNumber == MAP_INDEX_KALIMA7 || Obj.MapNumber == MAP_INDEX_CASTLEHUNTZONE )
		{
			if ( Obj.Level >= gGemOfDefendDropLevel )
			{
				if ( (rand()%10000)< gGemOfDefendDropRate )
				{
					type = ItemGetNumberMake(14,31);
					level = 0;
					x = Obj.X;
					y = Obj.Y;
					int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

					ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, level, dur,
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
			if (Obj.MapNumber == MAP_INDEX_CASTLEHUNTZONE)
			{
				if (Obj.Level >= gSetItemInCastleHuntZoneDropLevel) //Good
				{
					if ((rand() % 10000) < gSetItemInCastleHuntZoneDropRate)
					{
						int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

						if (ObjectMaxRange(MaxHitUser))
						{
							sLog->outBasic("[Castle HuntZone] Drop SetItem [%s][%s] ", getGameObject(MaxHitUser)->AccountID, getGameObject(MaxHitUser)->Name);
							::MakeRewardSetItem(MaxHitUser, Obj.X, Obj.Y, 0, Obj.MapNumber);
							return TRUE;
						}
					}
				}
			}
		}
	}

	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		if (Obj.MapNumber == MAP_INDEX_KANTURU2)
		{
			if ((rand() % 10000) < g_Kanturu.GetMoonStoneDropRate())
			{
				type = ItemGetNumberMake(13, 38);	// Moon Stone
				level = 0;
				x = Obj.X;
				y = Obj.Y;
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, level, dur, Option1, Option2, Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}

		if (Obj.MapNumber == MAP_INDEX_KANTURU1 || Obj.MapNumber == MAP_INDEX_KANTURU2 || Obj.MapNumber == MAP_INDEX_KANTURU_BOSS)
		{
			if ((rand() % 10000) < g_Kanturu.GetJewelOfHarmonyDropRate())
			{
				type = ItemGetNumberMake(14, 41);
				level = 0;
				x = Obj.X;
				y = Obj.Y;
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, level, dur, Option1, Option2, Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
	}

	if ( Obj.MapNumber == MAP_INDEX_AIDA || Obj.MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE)
	{
		bool bDropMysteriousBead = false;

		switch ( Obj.Class )
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
			x = Obj.X;
			y = Obj.Y;
			int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

			ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, level, dur,
				Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

			return TRUE;
		}
	}

	if ( g_DevilSquare.IsEventEnable() == true )
	{
		if (!DS_MAP_RANGE(Obj.MapNumber))
		{
			Option1 = 0;
			Option2 = 0;
			Option3 = 0;

			if ((rand() % 10000) < g_DevilSquare.GetEyeDropRate())
			{
				dur = 1.0f;
				x = Obj.X;
				y = Obj.Y;

				if (Obj.Level < 36)
					level = 1;
				else if (Obj.Level < 47)
					level = 2;
				else if (Obj.Level < 60)
					level = 3;
				else if (Obj.Level < 70)
					level = 4;
				else if (Obj.Level < 80)
					level = 5;
				else if (Obj.Level < 90)
					level = 6;
				else
					level = 7;

				type = ItemGetNumberMake(14, 17);

				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, level, dur,
					Option1, Option2, Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}

			if ((rand() % 10000) < g_DevilSquare.GetKeyDropRate())
			{
				dur = 1.0f;
				x = Obj.X;
				y = Obj.Y;

				if (Obj.Level < 36)
					level = 1;
				else if (Obj.Level < 47)
					level = 2;
				else if (Obj.Level < 60)
					level = 3;
				else if (Obj.Level < 70)
					level = 4;
				else if (Obj.Level < 80)
					level = 5;
				else if (Obj.Level < 90)
					level = 6;
				else
					level = 7;

				type = ItemGetNumberMake(14, 18);

				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, level, dur,
					Option1, Option2, Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
	}

	if ( g_BloodCastle.IsEventEnable() == true )
	{
		if (!BC_MAP_RANGE(Obj.MapNumber))
		{
			Option1 = 0;
			Option2 = 0;
			Option3 = 0;

			if ((rand() % 1000) < g_ConfigRead.GetArchangelScrollDropRate)
			{
				dur = 1.0f;
				x = Obj.X;
				y = Obj.Y;

				if (Obj.Level < 36)
					level = 1;
				else if (Obj.Level < 47)
					level = 2;
				else if (Obj.Level < 60)
					level = 3;
				else if (Obj.Level < 70)
					level = 4;
				else if (Obj.Level < 80)
					level = 5;
				else if (Obj.Level < 90)
					level = 6;
				else
					level = 7;

				type = ItemGetNumberMake(13, 16);

				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, level, dur,
					Option1, Option2, Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}

			if ((rand() % 10000) < g_ConfigRead.GetBloodBoneDropRate)
			{
				dur = 1.0f;
				x = Obj.X;
				y = Obj.Y;

				if (Obj.Level < 36)
					level = 1;
				else if (Obj.Level < 47)
					level = 2;
				else if (Obj.Level < 60)
					level = 3;
				else if (Obj.Level < 70)
					level = 4;
				else if (Obj.Level < 80)
					level = 5;
				else if (Obj.Level < 90)
					level = 6;
				else
					level = 7;

				type = ItemGetNumberMake(13, 17);

				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, level, dur,
					Option1, Option2, Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
	}

	if ( g_bRibbonBoxEvent )
	{
		if ( Obj.Level >= g_iRedRibbonBoxDropLevelMin && Obj.Level <= g_iRedRibbonBoxDropLevelMax )
		{
			if ( (rand()%10000) < g_iRedRibbonBoxDropRate )
			{
				type = ItemGetNumberMake(12,32);
				level = 0;
				x = lpTargetObj.X;
				y = lpTargetObj.Y;
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemCreate(lpTargetObj.m_Index, lpTargetObj.MapNumber, x, y, type, level, dur,
					Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
		else if ( Obj.Level >= g_iGreenRibbonBoxDropLevelMin && Obj.Level <= g_iGreenRibbonBoxDropLevelMax )
		{
			if ( (rand()%10000) < g_iGreenRibbonBoxDropRate )
			{
				type = ItemGetNumberMake(12,33);
				level = 0;
				x = lpTargetObj.X;
				y = lpTargetObj.Y;
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemCreate(lpTargetObj.m_Index, lpTargetObj.MapNumber, x, y, type, level, dur,
					Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
		else if ( Obj.Level >= g_iBlueRibbonBoxDropLevelMin && Obj.Level <= g_iBlueRibbonBoxDropLevelMax )
		{
			if ( (rand()%10000) < g_iBlueRibbonBoxDropRate )
			{
				type = ItemGetNumberMake(12,34);
				level = 0;
				x = lpTargetObj.X;
				y = lpTargetObj.Y;
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemCreate(lpTargetObj.m_Index, lpTargetObj.MapNumber, x, y, type, level, dur,
					Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
	}

	if ( g_bChocolateBoxEvent )
	{
		if ( Obj.Level >= g_iPinkChocolateBoxDropLevelMin && Obj.Level <= g_iPinkChocolateBoxDropLevelMax )
		{
			if ( (rand()%10000) < g_iPinkChocolateBoxDropRate )
			{
				type = ItemGetNumberMake(14,32);
				level = 0;
				x = lpTargetObj.X;
				y = lpTargetObj.Y;
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemCreate(lpTargetObj.m_Index, lpTargetObj.MapNumber, x, y, type, level, dur,
					Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
		else if ( Obj.Level >= g_iRedChocolateBoxDropLevelMin && Obj.Level <= g_iRedChocolateBoxDropLevelMax )
		{
			if ( (rand()%10000) < g_iRedChocolateBoxDropRate )
			{
				type = ItemGetNumberMake(14,33);
				level = 0;
				x = lpTargetObj.X;
				y = lpTargetObj.Y;
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemCreate(lpTargetObj.m_Index, lpTargetObj.MapNumber, x, y, type, level, dur,
					Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
		else if ( Obj.Level >= g_iBlueChocolateBoxDropLevelMin && Obj.Level <= g_iBlueChocolateBoxDropLevelMax )
		{
			if ( (rand()%10000) < g_iBlueChocolateBoxDropRate )
			{
				type = ItemGetNumberMake(14,34);
				level = 0;
				x = lpTargetObj.X;
				y = lpTargetObj.Y;
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemCreate(lpTargetObj.m_Index, lpTargetObj.MapNumber, x, y, type, level, dur,
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
			x = lpTargetObj.X;
			y = lpTargetObj.Y;
			int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

			ItemCreate(lpTargetObj.m_Index, lpTargetObj.MapNumber, x, y, type, level, dur,
				Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);
			
			return TRUE;
		}
	}

	if ( g_bCandyBoxEvent )
	{
		if ( Obj.Level >= g_iLightPurpleCandyBoxDropLevelMin && Obj.Level <= g_iLightPurpleCandyBoxDropLevelMax )
		{
			if ( (rand()%10000) < g_iLightPurpleCandyBoxDropRate )
			{
				type = ItemGetNumberMake(14,32);
				level = 1;
				x = lpTargetObj.X;
				y = lpTargetObj.Y;
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemCreate(lpTargetObj.m_Index, lpTargetObj.MapNumber, x, y, type, level, dur,
					Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
		else if ( Obj.Level >= g_iVermilionCandyBoxDropLevelMin && Obj.Level <= g_iVermilionCandyBoxDropLevelMax )
		{
			if ( (rand()%10000) < g_iVermilionCandyBoxDropRate )
			{
				type = ItemGetNumberMake(14,33);
				level = 1;
				x = lpTargetObj.X;
				y = lpTargetObj.Y;
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemCreate(lpTargetObj.m_Index, lpTargetObj.MapNumber, x, y, type, level, dur,
					Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
		else if ( Obj.Level >= g_iDeepBlueCandyBoxDropLevelMin && Obj.Level <= g_iDeepBlueCandyBoxDropLevelMax )
		{
			if ( (rand()%10000) < g_iDeepBlueCandyBoxDropRate )
			{
				type = ItemGetNumberMake(14,34);
				level = 1;
				x = lpTargetObj.X;
				y = lpTargetObj.Y;
				int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemCreate(lpTargetObj.m_Index, lpTargetObj.MapNumber, x, y, type, level, dur,
					Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
	}

	if ( g_bHallowinDayEventOn )
	{
		bool bIsBossMonster = false;

		if ( Obj.m_Index == 349 || Obj.m_Index == 364 || Obj.m_Index == 361 ||
			 Obj.m_Index == 362 || Obj.m_Index == 363 )
			bIsBossMonster = true;

		if ( !bIsBossMonster )
		{
			if ( (rand()%10000) < g_iHallowinDayEventItemDropRate  )
			{
				type = ItemGetNumberMake(14,45);
				level = 0;
				x = Obj.X;
				y = Obj.Y;
				int iMaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemCreate(lpTargetObj.m_Index, Obj.MapNumber, x, y, type, level, dur,
					Option1,Option2,Option3, iMaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
	}

	if ( g_bSantaPolymorphRingDropOn )
	{
		bool bIsBossMonster = false;

		if ( Obj.m_Index == 349 || Obj.m_Index == 364 || Obj.m_Index == 361 ||
			 Obj.m_Index == 362 || Obj.m_Index == 363 )
			bIsBossMonster = true;

		if ( !bIsBossMonster )
		{
			if ( (rand()%10000) < g_iSantaPolymorphRingDropRate  )
			{
				type = ItemGetNumberMake(13,41);
				level = 0;
				x = Obj.X;
				y = Obj.Y;
				int iMaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

				ItemCreate(lpTargetObj.m_Index, Obj.MapNumber, x, y, type, level, dur,
					Option1,Option2,Option3, iMaxHitUser, 0, 0, 0, 0, 0);

				return TRUE;
			}
		}
	}

	if ( Obj.MapNumber == MAP_INDEX_BARRACKS )
	{
		if ( (rand()%10000) < g_iCondorFlameDropRate )
		{
			type = ItemGetNumberMake(13,52);	// Condor Flame
			level = 0;
			x = Obj.X;
			y = Obj.Y;
			int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

			ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, level, dur,
			Option1,Option2,Option3, MaxHitUser, 0, 0, 0, 0, 0);
			return TRUE;
		}			
	}

	if ( Obj.Level >= g_ConfigRead.data.common.SpiritMapDropLevel && rand()% 10000 < g_ConfigRead.data.common.SpiritMapDropRate )
	{
		type = ItemGetNumberMake(13,145);	// Spirit Map Fragment
		level = 0;
		x = Obj.X;
		y = Obj.Y;
		int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

		ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, 0, 1.0, 0, 0, 0, MaxHitUser, 0, 0, 0, 0, 0);
		return TRUE;
	}

	if (g_EvoMonMng.IsEvoMonEnable() == true)
	{
		if (Obj.Level >= g_EvoMonMng.GetSummonScrollDropLevel() && rand() % 10000 < g_EvoMonMng.GetSummonScrollDropLevel())
		{
			type = ItemGetNumberMake(14, 278);	// Piece of Evomon Scroll
			level = 0;
			x = Obj.X;
			y = Obj.Y;
			int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

			ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, 0, 1.0, 0, 0, 0, MaxHitUser, 0, 0, 0, 0, 0);
			return TRUE;
		}
	}

	return FALSE;
}

void gObjMonsterSummonSkillForLordSilvester(CGameObject &Obj)
{
	PMSG_DURATION_MAGIC_RECV pDuration;
	
	pDuration.Dir = 31;
	pDuration.X = Obj.X;
	pDuration.Y = Obj.Y;
	pDuration.MagicNumberH = 0;
	pDuration.MagicNumberL = 1;
	pDuration.NumberH = HIBYTE(Obj.TargetNumber);
	pDuration.NumberL = LOBYTE(Obj.TargetNumber);

	gGameProtocol.CGDurationMagicRecv((BYTE*)&pDuration, Obj.m_Index);

	Obj.m_ActState.Attack = 0;
	Obj.m_ActState.Move = 0;
	Obj.m_ActState.Rest = 1;
	Obj.NextActionTime = 3500;
	Obj.m_nPhaseLordSilvester++;

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

	CGameObject lpObj = getGameObject(nLordSilvesterIndex);

	if (Obj.Type != OBJ_MONSTER)
	{
		return;
	}

	BOOL bAllKillRecallMon = TRUE;

	for (int i = 0; i < 3; i++)
	{
		if ( Obj.m_nRecallMonIndex[i] == nRecallMonIndex )
		{
			Obj.m_nRecallMonIndex[i] = -1;
		}
	}

	for (int i = 0; i < 3; i++)
	{
		if ( Obj.m_nRecallMonIndex[i] > -1 )
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

void SendLordSilvesterBlockInfo(CGameObject &Obj, BYTE btMapSetType)
{
	char cTEMP_BUF[256];
	PMSG_SETMAPATTR_COUNT * lpMsg = (PMSG_SETMAPATTR_COUNT *)cTEMP_BUF;

	PHeadSetB((BYTE*)lpMsg, 0x46, sizeof(PMSG_SETMAPATTR_COUNT) + sizeof(PMSG_SETMAPATTR) * 12);
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

	if (Obj.m_Index == -1)
	{
		for (int i = g_ConfigRead.server.GetObjectMaxMonster(); i < g_ConfigRead.server.GetObjectMax(); i++)
		{
			if (getGameObject(i)->MapNumber == MAP_INDEX_URUK_MOUNTAIN) //season3 changed
			{
				if (getGameObject(i)->Connected > PLAYER_LOGGED)
				{
					IOCP.DataSend(i, (BYTE*)lpMsg, lpMsg->h.size);
				}
			}
		}
	}

	else
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)lpMsg, lpMsg->h.size);
	}
}

void CQeustNpcTeleport::Run(CGameObject &Obj)
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
		PHeadSetBE((BYTE*)&pMsg, 0x19, sizeof(pMsg));
		pMsg.MagicNumberH = SET_NUMBERH(AT_SKILL_TELEPORT);
		pMsg.MagicNumberL = SET_NUMBERL(AT_SKILL_TELEPORT);
		pMsg.SourceNumberH = SET_NUMBERH(Obj.m_Index);
		pMsg.SourceNumberL = SET_NUMBERL(Obj.m_Index);
		pMsg.TargetNumberH = SET_NUMBERH(Obj.m_Index);
		pMsg.TargetNumberL = SET_NUMBERL(Obj.m_Index);

		gGameProtocol.MsgSendV2(getGameObject(Obj.m_Index), (UCHAR*)&pMsg, pMsg.h.size);

		gObjViewportListProtocolDestroy(getGameObject(Obj.m_Index));
		gObjClearViewport(getGameObject(Obj.m_Index));

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

		g_SendNPCInfo.SendPortalCoordinateMoveNpc(Obj.MapNumber,
			this->m_QuestNPCTeleportPos[tableindex].mapnum,
			Obj.X, Obj.Y,
			this->m_QuestNPCTeleportPos[tableindex].x,
			this->m_QuestNPCTeleportPos[tableindex].y,
			Obj.Class);

		Obj.X = this->m_QuestNPCTeleportPos[tableindex].x;
		Obj.Y = this->m_QuestNPCTeleportPos[tableindex].y;
		Obj.TX = Obj.X;
		Obj.TY = Obj.Y;
		Obj.MTX = Obj.X;
		Obj.MTY = Obj.Y;
		Obj.m_OldX = Obj.TX;
		Obj.m_OldY = Obj.TY;
		Obj.MapNumber = this->m_QuestNPCTeleportPos[tableindex].mapnum;
		Obj.Dir = this->m_QuestNPCTeleportPos[tableindex].dir;
		Obj.StartX = Obj.X;
		Obj.StartY = Obj.Y;
		Obj.m_State = 1;
		Obj.PathCount = 0;

		sLog->outBasic("[Quest] %s Teleport MAP : %s", this->m_strNPCName.c_str(), Lang.GetMap(0, this->m_QuestNPCTeleportPos[tableindex].mapnum));
	}
}


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

