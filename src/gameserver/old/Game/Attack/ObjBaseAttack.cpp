////////////////////////////////////////////////////////////////////////////////
// ObjBaseAttack.cpp
#include "stdafx.h"
#include "ObjBaseAttack.h"
#include "GameMain.h"
#include "Logging/Log.h"
#include "DevilSquare.h"
#include "BloodCastle.h"
#include "ChaosCastle.h"
#include "IllusionTempleEvent_Renewal.h"
#include "BuffEffectSlot.h"
#include "BuffEffect.h"
#include "LastManStanding.h"
#include "ObjUseSkill.h"
#include "configread.h"
#include "MapAttribute.h"
#include "MasterLevelSkillTreeSystem.h"
#include "CrywolfAltar.h"
#include "CrywolfStatue.h"
#include "ChaosCastleFinal.h"
#include "MuunSystem.h"
#include "PvPBalance.h"
#include "CustomMaxStats.h"

// GS-N 0.99.60T 0x00499B40
//	GS-N	1.00.18	JPN	0x004B5E50	-	Completed

//Review by HermeX
//#include "GameServer.h"

CObjBaseAttack::CObjBaseAttack()
{
	return;
}



CObjBaseAttack::~CObjBaseAttack()
{
	return;
}



BOOL CObjBaseAttack::DecreaseArrow(CGameObject &Obj)
{
	if ( Obj.Class == CLASS_ELF && Obj.Type == OBJ_USER && g_ConfigRead.data.common.joinmuIsDecreaseAllow == true )
	{
		CItemObject * Left = &Obj.pntInventory[0];
		CItemObject * Right  = &Obj.pntInventory[1];

		/*(if ( (Right->m_Type >= ITEMGET(4,8) && Right->m_Type <= ITEMGET(4,14)) ||
			  Right->m_Type == ITEMGET(4,16) || Right->m_Type == ITEMGET(4,18) ||
			  Right->m_Type == ITEMGET(4,19) || Right->m_Type == ITEMGET(4,20) ||
			  Right->m_Type == ITEMGET(4,21) || Right->m_Type == ITEMGET(4,22) ||
			  Right->m_Type == ITEMGET(4,23) || Right->m_Type == ITEMGET(4,24))
		{
			if ( Left->m_Type == ITEMGET(4,7) )
			{
				if ( Left->m_Durability < 1.0f )
				{
					return FALSE;
				}

				Left->m_Durability -= 1.0f;
				gGameProtocol.GCItemObjectDurSend(Obj.m_Index, 1, Left->m_Durability, 0);

				if ( Left->m_Durability < 1.0f )
				{
					Obj.pntInventory[1]->Clear();
					gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, 1, 0);
				}
			}
			else
			{
				return FALSE;
			}
		}
		*/ if ( (Left->m_Type >= ITEMGET(4,0) && Left->m_Type <= ITEMGET(4,31) ))
		{
			if ( Right->m_Type == ITEMGET(4,15) || Right->m_Type == ITEMGET(4, 7))
			{
				if ( Right->m_Durability < 1.0f )
				{
					return FALSE;
				}
				Right->m_Durability -= 1.0f;
				GCItemObjectDurSend(Obj.m_Index, 1, Right->m_Durability, 0);

				if ( Right->m_Durability < 1.0f )
				{
					Obj.pntInventory[1]->Clear();
					GCInventoryItemDeleteSend(Obj.m_Index, 1, 0);
				}
			}
			else if (Right->m_Type == ITEMGET(4, 32) || Right->m_Type == ITEMGET(4, 33) || Right->m_Type == ITEMGET(4, 34) || Right->m_Type == ITEMGET(4, 35))
			{
				if (Right->m_Durability < 1.0f)
				{
					return FALSE;
				}

				Right->m_Durability -= 0.0f;

				GCItemObjectDurSend(Obj.m_Index, 1, Right->m_Durability, 0);
			}
			else
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}



BOOL CObjBaseAttack::CheckAttackArea(CGameObject &Obj, CGameObject lpTargetObj)
{
	if( lpTargetObj.MapNumber == g_LastManStanding.m_Cfg.iPVPMap)
	{
		return TRUE;
	}

	if ( lpTargetObj.Type == OBJ_USER || lpTargetObj.m_RecallMon >= 0 )
	{
		BYTE attr = MapC[lpTargetObj.MapNumber].GetAttr(lpTargetObj.X, lpTargetObj.Y);

		if ( (attr&1) == 1 )
		{
			return FALSE;
		}
	}

	if ( Obj.Type == OBJ_USER )
	{
		int iRet = gObjCheckAttackArea(Obj.m_Index, lpTargetObj.m_Index);

		if ( iRet != 0 )
		{
			sLog->outBasic("[%s][%s] Try Attack In Not Attack Area (%s,%d,%d) errortype = %d",Obj.AccountID, Obj.Name, Lang.GetMap(0,0+Obj.MapNumber),
				Obj.X, Obj.Y, iRet);

			if ( bIsIgnorePacketSpeedHackDetect != FALSE )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL CObjBaseAttack::PkCheck(CGameObject &Obj, CGameObject lpTargetObj)
{
	if (Obj.Type == OBJ_USER && lpTargetObj.Type == OBJ_USER)
	{
		if (lpTargetObj.Level <= 5 || Obj.Level <= 5)
		{
			return FALSE;
		}

		if (gObjGetRelationShip(lpObj, lpTargetObj) == 2) // Rivals
		{
			if (g_MaxStatsInfo.GetClass.IsNonPvP[Obj.MapNumber] == FALSE)
			{
				return TRUE;
			}
		}

		if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
		{
			if (Obj.MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
			{
				return TRUE;
			}
		}

		if (g_ArcaBattle.IsArcaBattleServer() == TRUE)
		{
			return TRUE;
		}

		if (g_ConfigRead.EnableAttackBlockInSafeZone == TRUE)
		{
			BYTE btAttr1 = MapC[Obj.MapNumber].GetAttr(Obj.X, Obj.Y);
			BYTE btAttr2 = MapC[Obj.MapNumber].GetAttr(lpTargetObj.X, lpTargetObj.Y);

			if ((btAttr1 & 1) == 1 || (btAttr2 & 1) == 1)
			{
				return FALSE;
			}
		}

		if ( Obj.m_PlayerData->lpGuild != NULL && lpTargetObj.m_PlayerData->lpGuild != NULL )
		{
			if ( Obj.m_PlayerData->lpGuild->WarState == 1 && lpTargetObj.m_PlayerData->lpGuild->WarState == 1 )
			{
				if ( Obj.m_PlayerData->lpGuild->Number == lpTargetObj.m_PlayerData->lpGuild->Number )
				{
					return FALSE;
				}
			}
		}

		if(DG_MAP_RANGE(Obj.MapNumber) || IMPERIAL_MAP_RANGE(Obj.MapNumber))
		{
			return false;
		}

		if(IT_MAP_RANGE(Obj.MapNumber) && IT_MAP_RANGE(lpTargetObj.MapNumber))
		{
			return true;
		}

		if(Obj.MapNumber == MAP_INDEX_DEVILSQUARE_FINAL)
		{
			return false;
		}

		if(Obj.MapNumber == g_LastManStanding.m_Cfg.iPVPMap)
		{
			if(Obj.m_PlayerData->RegisterdLMS == TRUE || lpTargetObj.m_PlayerData->RegisterdLMS == TRUE)
			{
				if(g_LastManStanding.m_Rooms[Obj.m_PlayerData->RegisteredLMSRoom].bState != 3)
				{
					return false;
				}
			}
		}
		if ( gObjTargetGuildWarCheck(lpObj, lpTargetObj) == FALSE && lpTargetObj.m_PlayerData->lpGuild != NULL && lpTargetObj.m_PlayerData->lpGuild->WarState != 0)
		{
			if ( lpTargetObj.m_PlayerData->lpGuild->WarType == 1 && lpTargetObj.MapNumber != MAP_INDEX_ARENA)
			{
				if (!g_MaxStatsInfo.GetClass.IsNonPvP[Obj.MapNumber])
				{
					return TRUE;
				}
			}

			if ( CC_MAP_RANGE(lpTargetObj.MapNumber) == FALSE )
			{
				return FALSE;
			}

			if (lpTargetObj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
			{
				return FALSE;
			}
		}

		else if ( CC_MAP_RANGE(Obj.MapNumber) != FALSE )
		{
			if ( g_ChaosCastle.GetCurrentState(g_ChaosCastle.GetChaosCastleIndex(Obj.MapNumber)) != 2 )
			{
				return FALSE;
			}
		}

		else if (Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
		{
			if (g_ChaosCastleFinal.GetCurrentState() != 2)
			{
				return FALSE;
			}
		}

		else if (g_MaxStatsInfo.GetClass.IsNonPvP[Obj.MapNumber] != FALSE )
		{
			return FALSE;
		}

		if( g_MapAttr.GetPvPState(Obj.MapNumber) == 1 )
		{
			return FALSE;
		}

		if ( DS_MAP_RANGE(Obj.MapNumber) ) // DEvil
		{
			return FALSE;
		}

		if ( BC_MAP_RANGE(Obj.MapNumber) )
		{
			return FALSE;
		}

		if ( gObjDuelCheck(lpObj, lpTargetObj) == FALSE )
		{
			if ( gObjDuelCheck(lpTargetObj) )
			{
				return FALSE;
			}
		}
	}
	return true;
}



BOOL CObjBaseAttack::ResistanceCheck(CGameObject &Obj, CGameObject lpTargetObj, int skill)
{
	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (lpTargetObj.Type != OBJ_USER)
		{
			if (lpTargetObj.Class == 277 || lpTargetObj.Class == 283 || lpTargetObj.Class == 288 || lpTargetObj.Class == 278 || lpTargetObj.Class == 215 || lpTargetObj.Class == 216 || lpTargetObj.Class == 217 || lpTargetObj.Class == 218 || lpTargetObj.Class == 219)
			{
				return FALSE;
			}
		}

		if (lpTargetObj.Type != OBJ_USER)
		{
			if (CRYWOLF_ALTAR_CLASS_RANGE(lpTargetObj.Class) != FALSE || CRYWOLF_STATUE_CHECK(lpTargetObj.Class) != FALSE) //HermeX Fix @28/01/2010
			{
				return FALSE;
			}
		}
	}

	if (lpTargetObj.Type == OBJ_USER)
	{
		if (lpTargetObj.m_PlayerData->m_MPSkillOpt->iMpsImmuneRate > 0.0 && lpTargetObj.m_PlayerData->m_MPSkillOpt->iMpsImmuneRate >= (rand() % 100) && !gObjCheckUsedBuffEffect(lpTargetObj, BUFFTYPE_INVISIBLE2))
		{
			gGameProtocol.GCMagicAttackNumberSend(lpTargetObj, 323, lpTargetObj.m_Index, 1);
			gObjAddBuffEffect(lpTargetObj, BUFFTYPE_INVISIBLE2, 0, 0, 0, 0, 5);
		}
	}

	if ( skill == 62 )
	{
		gObjBackSpring2(lpTargetObj, lpObj, 3);
	}

	if (skill == 512) //Earth Quake Master
	{
		gObjBackSpring2(lpTargetObj, lpObj, 3);
	}

	if (skill == 516)
	{
		gObjBackSpring2(lpTargetObj, lpObj, 3);
	}

	float fValue = 0;

	if (skill == 512)
	{
		for (int i = 0; i < MAX_MAGIC; i++)
		{
			if (Obj.Magic[i].IsMagic() == TRUE && Obj.Magic[i].m_Skill == skill)
			{
				fValue = g_MasterLevelSkillTreeSystem.GetMasterSkillValue(MagicDamageC.SkillGet(skill), Obj.Magic[i].m_Level);
				break;
			}
		}

		if (rand() % 100 < fValue)
		{
			gObjBackSpring(lpTargetObj, lpObj);
		}
	}

	if (skill == 19 
		||skill == 20 
		|| skill == 21 
		|| skill == 22 
		|| skill == 23
		|| skill == 270
		|| skill == 326
		|| skill == 327
		|| skill == 328
		|| skill == AT_MSKILL_DK_LUNGE1
		|| skill == 479
		)
	{
		gObjAddMsgSendDelay(lpTargetObj, 2, Obj.m_Index, 150, 0);
	}
	else if ( skill == 3 || skill == 379 || skill == 480 )	// Poison
	{
		if ( retResistance(lpTargetObj, 2) == 0 )
		{
			gObjAddMsgSendDelay(lpTargetObj, 2, Obj.m_Index, 150, 0);
			return TRUE;
		}

		return FALSE;
	}

	else if (skill == 1 || skill == 384)
	{
		if (gObjCheckUsedBuffEffect(lpTargetObj, BUFFTYPE_POISON) == FALSE)
		{
			if (g_ConfigRead.data.common.IsJoinMu == 1)
			{
				if (rand() % 100 < gObjUseSkill.m_SkillData.PoisonSkillSuccessRate)
				{
					lpTargetObj.lpAttackObj = lpObj;
					lpTargetObj.m_SkillNumber = skill;
					gObjAddBuffEffect(lpTargetObj, BUFFTYPE_POISON, EFFECTTYPE_POISON_DMG_TICK, gObjUseSkill.m_SkillData.PoisonPercent, 0, 0, gObjUseSkill.m_SkillData.PoisonSkillTime);

				}
				else
				{
					return FALSE;
				}
			}
			else
			{
				if (retResistance(lpTargetObj, 1) == 0)
				{
					lpTargetObj.lpAttackObj = lpObj;
					lpTargetObj.m_SkillNumber = skill;
					gObjAddBuffEffect(lpTargetObj, BUFFTYPE_POISON, EFFECTTYPE_POISON_DMG_TICK, gObjUseSkill.m_SkillData.PoisonPercent, 0, 0, 20);
				}
				else
				{
					return FALSE;
				}
			}
		}
		else
		{
			return FALSE;
		}
	}
	else if (skill == 38 || skill == 387) // Death Poison
	{
		if (g_ConfigRead.data.common.IsJoinMu == 1)
		{
			if (rand() % 100 < gObjUseSkill.m_SkillData.PoisonSkillSuccessRate)
			{
				lpTargetObj.lpAttackObj = lpObj;
				lpTargetObj.m_SkillNumber = skill;
				gObjAddBuffEffect(lpTargetObj, BUFFTYPE_POISON, EFFECTTYPE_POISON_DMG_TICK, gObjUseSkill.m_SkillData.DeathPoisonPercent, 0, 0, gObjUseSkill.m_SkillData.PoisonSkillTime);
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			if (retResistance(lpTargetObj, 1) == 0)
			{
				lpTargetObj.lpAttackObj = lpObj;
				lpTargetObj.m_SkillNumber = skill;
				gObjAddBuffEffect(lpTargetObj, BUFFTYPE_POISON, EFFECTTYPE_POISON_DMG_TICK, gObjUseSkill.m_SkillData.DeathPoisonPercent, 0, 0, 20);
			}
			else
			{
				return FALSE;
			}
		}

		return TRUE;
	}

	else if (skill == 7 || skill == 39 || skill == 389 || skill == 489 || skill == 391 || skill == 393 || skill == 491) //Season4 add-on )
	{
		if ( gObjCheckUsedBuffEffect(lpTargetObj, BUFFTYPE_FREEZE) == FALSE )
		{
			if ( retResistance(lpTargetObj, 0) == 0 )
			{
				if (g_ConfigRead.EnableFreezeEffect == 1)
				{
					lpTargetObj.DelayActionTime = 800;
					lpTargetObj.DelayLevel = 1;
					lpTargetObj.lpAttackObj = lpObj;
					lpTargetObj.m_SkillNumber = skill;
					gObjAddBuffEffect(lpTargetObj, BUFFTYPE_FREEZE, EFFECTTYPE_REDUCE_MOVE_SPEED, 0, 0, 0, g_ConfigRead.EnableFreezeEffectTime);
				}
			}

			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}

	else if ( skill == 51 || skill == 424 ) // Ice Arrow
	{
		if (gObjCheckUsedBuffEffect(lpTargetObj, BUFFTYPE_STONE) == FALSE)
		{
			if (retResistance(lpTargetObj, 0) == 0)
			{
				lpTargetObj.lpAttackObj = lpObj;
				gObjAddBuffEffect(lpTargetObj, BUFFTYPE_STONE, 0, 0, 0, 0, gObjUseSkill.m_SkillData.IceArrowTime);
				lpTargetObj.PathCount = 0;
				lpTargetObj.PathStartEnd = 0;
				gObjSetPosition(lpTargetObj.m_Index, lpTargetObj.X, lpTargetObj.Y);
				return true;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}

	return TRUE;
}



BOOL CObjBaseAttack::MissCheck(CGameObject &Obj, CGameObject lpTargetObj, int skill, int skillSuccess, int magicsend, BOOL& bAllMiss, BYTE RFAttack)
{
	int iAttackRate = 0;

	__try
	{
		iAttackRate = 0;
		int iDefenseRate = lpTargetObj.m_SuccessfulBlocking;

		BYTE MSBDamage = 0;	// MonsterSetBasse Damage

		if (IT_MAP_RANGE(lpTargetObj.MapNumber) && lpTargetObj.Type == OBJ_USER)
		{
			if (g_IT_Event.GetIllusionTempleState(lpTargetObj.MapNumber) == 2)
			{
				if (g_IT_Event.CheckSkillProdection(lpTargetObj.m_nITR_Index, lpTargetObj.MapNumber) == TRUE)
				{
					gGameProtocol.GCDamageSend(Obj.m_Index, lpTargetObj.m_Index, 0, 0, 0, 0);
					return 0;
				}

				if (lpTargetObj.PartyNumber == Obj.PartyNumber)
				{
					gGameProtocol.GCDamageSend(Obj.m_Index, lpTargetObj.m_Index, 0, 0, 0, 0);
					return 0;
				}
			}
		}

		if ( Obj.Type == OBJ_USER )	// Miss for Uses
		{
			iAttackRate = Obj.m_PlayerData->m_AttackRatePvM;
			iAttackRate += iAttackRate * Obj.m_PlayerData->SetOpImproveSuccessAttackRate / 100;
		}
		else	// Miss for Monsters
		{
			iAttackRate = Obj.m_AttackRating;
		}

		if (Obj.Type == OBJ_USER)
		{
			iAttackRate += Obj.m_PlayerData->m_MPSkillOpt->iMpsAttackSuccessRate;
		}

		if (gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_BLIND_2) == true)
		{
			int nDecRate = 0;
			gObjGetValueOfBuffIndex(lpObj, BUFFTYPE_BLIND_2, &nDecRate, 0);

			if (nDecRate > 0)
			{
				iAttackRate -= iAttackRate * nDecRate / 100.0;
			}
		}

		if ( iAttackRate < iDefenseRate )
		{
			bAllMiss = TRUE;
		}

		if ( bAllMiss != FALSE )	// When All Miss
		{
			if ( (rand()%100) >= 5 )
			{
				if (RFAttack)
				{
					if (Obj.Class == CLASS_RAGEFIGHTER)
					{
						if (skill == 261 || skill == 263 || skill == 552 || skill == 555 || skill == 559 || skill == 563)
						{
							if (RFAttack % 2)
							{
								MSBDamage |= 0x10;
							}

							else
							{
								MSBDamage |= 0x20;
							}
						}

						else
						{
							if (RFAttack % 4)
							{
								MSBDamage |= 0x10;
							}

							else
							{
								MSBDamage |= 0x20;
							}
						}
					}

					else if (Obj.Class == CLASS_GROWLANCER)
					{
						if (skill == 276 || skill == 274 || skill == 277 || skill == 699 || skill == 688 || skill == 691 || skill == 692 || skill == 694 || skill == 695)
						{
							if (RFAttack % 2)
							{
								MSBDamage |= 0x10;
							}

							else
							{
								MSBDamage |= 0x20;
							}
						}
					}
				}

				gGameProtocol.GCDamageSend(Obj.m_Index, lpTargetObj.m_Index, 0, 0, MSBDamage, 0);

				if (magicsend != 0 )
				{
					gGameProtocol.GCMagicAttackNumberSend(lpObj, skill, lpTargetObj.m_Index, skillSuccess);
				}

				return FALSE;
			}
		}
		else	// if the is a chance  ot hit the target
		{
			if ( (rand()%iAttackRate) < iDefenseRate)
			{
				if (RFAttack)
				{
					if (Obj.Class == CLASS_RAGEFIGHTER)
					{
						if (skill == 261 || skill == 263 || skill == 552 || skill == 555 || skill == 559 || skill == 563)
						{
							if (RFAttack % 2)
							{
								MSBDamage |= 0x10;
							}

							else
							{
								MSBDamage |= 0x20;
							}
						}

						else
						{
							if (RFAttack % 4)
							{
								MSBDamage |= 0x10;
							}

							else
							{
								MSBDamage |= 0x20;
							}
						}
					}

					else if (Obj.Class == CLASS_GROWLANCER)
					{
						if (skill == 276 || skill == 274 || skill == 277 || skill == 699 || skill == 688 || skill == 691 || skill == 692 || skill == 694 || skill == 695)
						{
							if (RFAttack % 2)
							{
								MSBDamage |= 0x10;
							}

							else
							{
								MSBDamage |= 0x20;
							}
						}
					}
				}

				gGameProtocol.GCDamageSend(Obj.m_Index, lpTargetObj.m_Index, 0, 0, MSBDamage, 0);

				if (magicsend != 0 )
				{
					gGameProtocol.GCMagicAttackNumberSend(lpObj, skill, lpTargetObj.m_Index, skillSuccess);
				}

				return FALSE;
			}
		}
	}
	__except (iAttackRate = 1, 1)
	{
		sLog->outBasic("error2: %s's level is 0", Obj.Name);
		return FALSE;
	}

	return TRUE;
}



BOOL CObjBaseAttack::MissCheckPvP(CGameObject &Obj , CGameObject lpTargetObj, int skill, int skillSuccess, int magicsend, BOOL & bAllMiss, BYTE RFAttack)
{
	if (lpObj == NULL)
	{
		return FALSE;
	}

	if (lpTargetObj == NULL)
	{
		return FALSE;
	}

	if (lpTargetObj.Type != OBJ_USER || Obj.Type != OBJ_USER)
	{
		return FALSE;
	}

	float iAttackRate = 0;
	float iDefenseRate = 0;
	int AttackLevel = Obj.Level + Obj.m_PlayerData->MasterLevel;
	int DefenseLevel = lpTargetObj.Level + lpTargetObj.m_PlayerData->MasterLevel;
	int iAttackSuccessRate = 0;

	if (IT_MAP_RANGE(lpTargetObj.MapNumber) && lpTargetObj.Type == OBJ_USER)
	{
		if (g_IT_Event.GetIllusionTempleState(lpTargetObj.MapNumber) == 2)
		{
			if (g_IT_Event.CheckSkillProdection(lpTargetObj.m_nITR_Index, lpTargetObj.MapNumber) == TRUE)
			{
				gGameProtocol.GCDamageSend(Obj.m_Index, lpTargetObj.m_Index, 0, 0, 0, 0);
				return 0;
			}

			if (lpTargetObj.PartyNumber == Obj.PartyNumber)
			{
				gGameProtocol.GCDamageSend(Obj.m_Index, lpTargetObj.m_Index, 0, 0, 0, 0);
				return 0;
			}
		}
	}

#if (ENABLE_CUSTOM_PVPBALANCE == 1)
	g_PvPBalance.modifyAttackRate(Obj.m_Index, lpTargetObj.m_Index, iAttackRate);
	g_PvPBalance.modifyDefenseRate(Obj.m_Index, lpTargetObj.m_Index, iDefenseRate);
#endif

	iAttackRate = Obj.m_PlayerData->m_AttackRatePvP;
	iDefenseRate = lpTargetObj.m_PlayerData->m_DefenseRatePvP;

	if ( iAttackRate <= 0.0f || iDefenseRate <= 0.0f || AttackLevel <= 0 || lpTargetObj == 0 ) return FALSE;

	iAttackRate += Obj.m_PlayerData->m_ItemOptionExFor380->OpAddAttackSuccessRatePVP;
	iDefenseRate += lpTargetObj.m_PlayerData->m_ItemOptionExFor380->OpAddDefenseSuccessRatePvP;

	iAttackRate += Obj.m_PlayerData->m_JewelOfHarmonyEffect->HJOpAddAttackSuccessRatePVP;
	iDefenseRate += lpTargetObj.m_PlayerData->m_JewelOfHarmonyEffect->HJOpAddDefenseSuccessRatePvP;

	iAttackRate += Obj.m_PlayerData->m_MPSkillOpt->iMpsIncreasePvPAttackRate;
	iDefenseRate += lpTargetObj.m_PlayerData->m_MPSkillOpt->iMpsIncreasePvPDefenseRate;
	
	float iExpressionA = ( iAttackRate * 10000.0f ) / ( iAttackRate + iDefenseRate );	// #formula
	float iExpressionB = ( AttackLevel * 10000 ) / ( AttackLevel + DefenseLevel );	// #formula

	iExpressionA /= 10000.0f;
	iExpressionB /= 10000.0f;
			
	iAttackSuccessRate = 100.0f * iExpressionA * g_ConfigRead.g_fSuccessAttackRateOption * iExpressionB;
	
	if ( (DefenseLevel - AttackLevel) >= 100 )
	{
		iAttackSuccessRate -= 5;
	}

	else if ( (DefenseLevel - AttackLevel) >= 200 )
	{
		iAttackSuccessRate -= 10;
	}

	else if ( (DefenseLevel - AttackLevel) >= 300 )
	{
		iAttackSuccessRate -= 15;
	}

	if (gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_BLIND_2) == true)
	{
		int nDecRate = 0;
		gObjGetValueOfBuffIndex(lpObj, BUFFTYPE_BLIND_2, &nDecRate, 0);

		if (nDecRate > 0)
		{
			iAttackRate -= iAttackRate * nDecRate / 100.0;
		}
	}

	DWORD dwRate = rand() % 100;

	if ( dwRate > iAttackSuccessRate )
	{
		BYTE MsgDamage = 0;

		if (RFAttack)
		{
			if (Obj.Class == CLASS_RAGEFIGHTER)
			{
				if (skill == 261 || skill == 263 || skill == 552 || skill == 555 || skill == 559 || skill == 563)
				{
					if (RFAttack % 2)
					{
						MsgDamage |= 0x10;
					}

					else
					{
						MsgDamage |= 0x20;
					}
				}

				else
				{
					if (RFAttack % 4)
					{
						MsgDamage |= 0x10;
					}

					else
					{
						MsgDamage |= 0x20;
					}
				}
			}

			else if (Obj.Class == CLASS_GROWLANCER)
			{
				if (skill == 276 || skill == 274 || skill == 277 || skill == 699 || skill == 688 || skill == 691 || skill == 692 || skill == 694 || skill == 695)
				{
					if (RFAttack % 2)
					{
						MsgDamage |= 0x10;
					}

					else
					{
						MsgDamage |= 0x20;
					}
				}
			}
		}

		gGameProtocol.GCDamageSend(Obj.m_Index, lpTargetObj.m_Index, 0, 0, MsgDamage, 0);

		if ( g_ConfigRead.g_bShieldComboMissOptionOn == TRUE )
		{
			if ( Obj.m_PlayerData->comboSkill.ProgressIndex >= 0 )
			{
				sLog->outBasic("[Shield] ComboSkill Cancel! [%s][%s]",Obj.AccountID, Obj.Name);
				Obj.m_PlayerData->comboSkill.Init();
			}
		}

		return FALSE;
	}

	return TRUE;
}

int  CObjBaseAttack::GetTargetDefense(CGameObject &Obj, CGameObject lpTargetObj, int& MsgDamage, int& iOriginTargetDefense)
{
	int targetdefense = lpTargetObj.m_Defense;

	if (Obj.Type == OBJ_USER && lpTargetObj.Type == OBJ_USER)
	{
		targetdefense += lpTargetObj.m_PlayerData->m_ItemOptionExFor380->OpAddDefense / 2;
	}

	int decdef = gObjGetTotalValueOfEffect(lpTargetObj, EFFECTTYPE_DECREASE_DEFENSE);
	targetdefense -= decdef * targetdefense / 100;

	if (gObjCheckUsedBuffEffect(lpTargetObj, BUFFTYPE_ARCA_WATERTOWER) == true)
	{
		int nEffectValue = 0;
		gObjGetValueOfBuffIndex(lpTargetObj, BUFFTYPE_ARCA_WATERTOWER, &nEffectValue, 0);
		targetdefense += nEffectValue;
	}

	int nMuunItemEffectValue = 0;

	if (g_CMuunSystem.GetMuunItemValueOfOptType(lpTargetObj, MUUN_INC_DEFENSE, &nMuunItemEffectValue, 0) == TRUE)
	{
		targetdefense += 10 * nMuunItemEffectValue / 20;
	}
	
	if ( lpTargetObj.m_MonsterSkillElementInfo.m_iSkillElementDefenseTime > 0 )
	{
		targetdefense += lpTargetObj.m_MonsterSkillElementInfo.m_iSkillElementDefense;

		if ( targetdefense <0 )
			targetdefense = 0;

	}

	float percentdamage = 0;

	if ( Obj.pntInventory[7]->IsItem() != FALSE )
	{
		percentdamage += Obj.m_PlayerData->m_WingExcOption->iWingOpIgnoreEnemyDefense;
	}

	if ( gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_MONK_IGNORE_ENEMY_DEFENSE))
	{
		int nEffectValue1 = 0;
		int nEffectValue2 = 0;
		gObjGetValueOfBuffIndex(lpObj, BUFFTYPE_MONK_IGNORE_ENEMY_DEFENSE, &nEffectValue1, &nEffectValue2);

		percentdamage += nEffectValue1;

		if (nEffectValue2 > 0)
		{
			if (rand() % 100 <= 10)
			{
				float nEffectValue = nEffectValue2 * lpTargetObj.m_SuccessfulBlocking / 100.0;
				gObjAddBuffEffect(lpTargetObj, BUFFTYPE_PENETRATE_ARMOR, EFFECTTYPE_DECREASE_DEFENSE, nEffectValue2, EFFECTTYPE_DECREASE_DEFENSE_RATE, nEffectValue, 10);
			}
		}
	}

	if(Obj.Type == OBJ_USER)
	{
		percentdamage += Obj.m_PlayerData->SetOpIgnoreDefense;

		if (Obj.m_PlayerData->m_MPSkillOpt->iMpsIgnoreEnemyDefence > 0.0)
		{
			percentdamage += Obj.m_PlayerData->m_MPSkillOpt->iMpsIgnoreEnemyDefence;
		}

		if (Obj.m_PlayerData->m_MPSkillOpt->iMpsIncIgnoreEnemyBlock > 0.0)
		{
			percentdamage += Obj.m_PlayerData->m_MPSkillOpt->iMpsIncIgnoreEnemyBlock;
		}
	}

	if (lpTargetObj.Type == OBJ_USER)
	{
		percentdamage -= lpTargetObj.m_PlayerData->m_Resistance_Perfect;
	}

	iOriginTargetDefense = targetdefense;
	
	if ( percentdamage != 0.0)
	{
		if ( (rand()%100) <= percentdamage)
		{
			targetdefense = 0;
			MsgDamage = 1;
		}
	}

	return targetdefense;
}





int  CObjBaseAttack::GetPartyMemberCount(CGameObject &Obj)
{
	CGameObject lpPartyObj;
	int partynum = Obj.PartyNumber;

	if ( ObjectMaxRange(partynum) == FALSE )
	{
		return 0;
	}

	int partycount = gParty.m_PartyS[partynum].Count;
	int retcount = 0;

	for ( int n=0;n<MAX_USER_IN_PARTY;n++)
	{
		int memberindex = gParty.m_PartyS[partynum].Number[n];

		if ( memberindex >= 0 )
		{
			lpPartyObj = getGameObject(memberindex);

			if ( Obj.MapNumber == lpPartyObj.MapNumber )
			{
				int dis = gObjCalDistance(lpObj, getGameObject(memberindex));
				
				if ( dis < MAX_PARTY_DISTANCE_EFFECT )
				{
					retcount++;
				}
			}
		}
	}

	return retcount;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

