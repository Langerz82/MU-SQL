////////////////////////////////////////////////////////////////////////////////
// ObjBaseAttack.cpp
#include "stdafx.h"
#include "ObjBaseAttack.h"
#include "Main.h"
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
#include "GameProtocol.h"
#include "GOFunctions.h"
#include "ArcaBattle.h"

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
		CItemObject * Left = Obj.pntInventory[0];
		CItemObject * Right  = Obj.pntInventory[1];

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
				GSProtocol.GCItemDurSend(Obj.m_Index, 1, Left->m_Durability, 0);

				if ( Left->m_Durability < 1.0f )
				{
					Obj.pntInventory[1]->Clear();
					GSProtocol.GCInventoryItemDeleteSend(Obj.m_Index, 1, 0);
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
				GSProtocol.GCItemDurSend(&Obj, 1, Right->m_Durability, 0);

				if ( Right->m_Durability < 1.0f )
				{
					Obj.pntInventory[1]->Clear();
					GSProtocol.GCInventoryItemDeleteSend(&Obj, 1, 0);
				}
			}
			else if (Right->m_Type == ITEMGET(4, 32) || Right->m_Type == ITEMGET(4, 33) || Right->m_Type == ITEMGET(4, 34) || Right->m_Type == ITEMGET(4, 35))
			{
				if (Right->m_Durability < 1.0f)
				{
					return FALSE;
				}

				Right->m_Durability -= 0.0f;

				GSProtocol.GCItemDurSend(&Obj, 1, Right->m_Durability, 0);
			}
			else
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}



BOOL CObjBaseAttack::CheckAttackArea(CGameObject &Obj, CGameObject &TargetObj)
{
	if( TargetObj.MapNumber == g_LastManStanding.m_Cfg.iPVPMap)
	{
		return TRUE;
	}

	if ( TargetObj.Type == OBJ_USER || TargetObj.m_RecallMon >= 0 )
	{
		BYTE attr = MapC[TargetObj.MapNumber].GetAttr(TargetObj.X, TargetObj.Y);

		if ( (attr&1) == 1 )
		{
			return FALSE;
		}
	}

	if ( Obj.Type == OBJ_USER )
	{
		int iRet = gObjCheckAttackArea(Obj, TargetObj.m_Index);

		if ( iRet != 0 )
		{
			sLog->outBasic("[%s][%s] Try Attack In Not Attack Area (%s,%d,%d) errortype = %d", Obj.m_PlayerData->ConnectUser->AccountID, Obj.Name, Lang.GetMap(0,0+Obj.MapNumber),
				Obj.X, Obj.Y, iRet);

			if ( bIsIgnorePacketSpeedHackDetect != FALSE )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL CObjBaseAttack::PkCheck(CGameObject &Obj, CGameObject &TargetObj)
{
	if (Obj.Type == OBJ_USER && TargetObj.Type == OBJ_USER)
	{
		if (TargetObj.Level <= 5 || Obj.Level <= 5)
		{
			return FALSE;
		}

		if (gObjGetRelationShip(Obj, TargetObj) == 2) // Rivals
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
			BYTE btAttr2 = MapC[Obj.MapNumber].GetAttr(TargetObj.X, TargetObj.Y);

			if ((btAttr1 & 1) == 1 || (btAttr2 & 1) == 1)
			{
				return FALSE;
			}
		}

		if ( Obj.m_PlayerData->lpGuild != NULL && TargetObj.m_PlayerData->lpGuild != NULL )
		{
			if ( Obj.m_PlayerData->lpGuild->WarState == 1 && TargetObj.m_PlayerData->lpGuild->WarState == 1 )
			{
				if ( Obj.m_PlayerData->lpGuild->Number == TargetObj.m_PlayerData->lpGuild->Number )
				{
					return FALSE;
				}
			}
		}

		if(DG_MAP_RANGE(Obj.MapNumber) || IMPERIAL_MAP_RANGE(Obj.MapNumber))
		{
			return false;
		}

		if(IT_MAP_RANGE(Obj.MapNumber) && IT_MAP_RANGE(TargetObj.MapNumber))
		{
			return true;
		}

		if(Obj.MapNumber == MAP_INDEX_DEVILSQUARE_FINAL)
		{
			return false;
		}

		if(Obj.MapNumber == g_LastManStanding.m_Cfg.iPVPMap)
		{
			if(Obj.m_PlayerData->RegisterdLMS == TRUE || TargetObj.m_PlayerData->RegisterdLMS == TRUE)
			{
				if(g_LastManStanding.m_Rooms[Obj.m_PlayerData->RegisteredLMSRoom].bState != 3)
				{
					return false;
				}
			}
		}
		if ( gObjTargetGuildWarCheck(Obj, TargetObj) == FALSE && TargetObj.m_PlayerData->lpGuild != NULL && TargetObj.m_PlayerData->lpGuild->WarState != 0)
		{
			if ( TargetObj.m_PlayerData->lpGuild->WarType == 1 && TargetObj.MapNumber != MAP_INDEX_ARENA)
			{
				if (!g_MaxStatsInfo.GetClass.IsNonPvP[Obj.MapNumber])
				{
					return TRUE;
				}
			}

			if ( CC_MAP_RANGE(TargetObj.MapNumber) == FALSE )
			{
				return FALSE;
			}

			if (TargetObj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
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

		if ( gObjDuelCheck(Obj, TargetObj) == FALSE )
		{
			if ( gObjDuelCheck(TargetObj) )
			{
				return FALSE;
			}
		}
	}
	return true;
}



BOOL CObjBaseAttack::ResistanceCheck(CGameObject &Obj, CGameObject &TargetObj, int skill)
{
	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (TargetObj.Type != OBJ_USER)
		{
			if (TargetObj.Class == 277 || TargetObj.Class == 283 || TargetObj.Class == 288 || TargetObj.Class == 278 || TargetObj.Class == 215 || TargetObj.Class == 216 || TargetObj.Class == 217 || TargetObj.Class == 218 || TargetObj.Class == 219)
			{
				return FALSE;
			}
		}

		if (TargetObj.Type != OBJ_USER)
		{
			if (CRYWOLF_ALTAR_CLASS_RANGE(TargetObj.Class) != FALSE || CRYWOLF_STATUE_CHECK(TargetObj.Class) != FALSE) //HermeX Fix @28/01/2010
			{
				return FALSE;
			}
		}
	}

	if (TargetObj.Type == OBJ_USER)
	{
		if (TargetObj.m_PlayerData->m_MPSkillOpt->iMpsImmuneRate > 0.0 && TargetObj.m_PlayerData->m_MPSkillOpt->iMpsImmuneRate >= (rand() % 100) && !gObjCheckUsedBuffEffect(TargetObj, BUFFTYPE_INVISIBLE2))
		{
			GSProtocol.GCMagicAttackNumberSend(&TargetObj, 323, TargetObj.m_Index, 1);
			gObjAddBuffEffect(TargetObj, BUFFTYPE_INVISIBLE2, 0, 0, 0, 0, 5);
		}
	}

	if ( skill == 62 )
	{
		gObjBackSpring2(TargetObj, Obj, 3);
	}

	if (skill == 512) //Earth Quake Master
	{
		gObjBackSpring2(TargetObj, Obj, 3);
	}

	if (skill == 516)
	{
		gObjBackSpring2(TargetObj, Obj, 3);
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
			gObjBackSpring(TargetObj, Obj);
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
		gObjAddMsgSendDelay(TargetObj, 2, Obj, 150, 0);
	}
	else if ( skill == 3 || skill == 379 || skill == 480 )	// Poison
	{
		if ( retResistance(TargetObj, 2) == 0 )
		{
			gObjAddMsgSendDelay(TargetObj, 2, Obj, 150, 0);
			return TRUE;
		}

		return FALSE;
	}

	else if (skill == 1 || skill == 384)
	{
		if (gObjCheckUsedBuffEffect(TargetObj, BUFFTYPE_POISON) == FALSE)
		{
			if (g_ConfigRead.data.common.IsJoinMu == 1)
			{
				if (rand() % 100 < gObjUseSkill.m_SkillData.PoisonSkillSuccessRate)
				{
					TargetObj.lpAttackObj = &Obj;
					TargetObj.m_SkillNumber = skill;
					gObjAddBuffEffect(TargetObj, BUFFTYPE_POISON, EFFECTTYPE_POISON_DMG_TICK, gObjUseSkill.m_SkillData.PoisonPercent, 0, 0, gObjUseSkill.m_SkillData.PoisonSkillTime);

				}
				else
				{
					return FALSE;
				}
			}
			else
			{
				if (retResistance(TargetObj, 1) == 0)
				{
					TargetObj.lpAttackObj = &Obj;
					TargetObj.m_SkillNumber = skill;
					gObjAddBuffEffect(TargetObj, BUFFTYPE_POISON, EFFECTTYPE_POISON_DMG_TICK, gObjUseSkill.m_SkillData.PoisonPercent, 0, 0, 20);
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
				TargetObj.lpAttackObj = &Obj;
				TargetObj.m_SkillNumber = skill;
				gObjAddBuffEffect(TargetObj, BUFFTYPE_POISON, EFFECTTYPE_POISON_DMG_TICK, gObjUseSkill.m_SkillData.DeathPoisonPercent, 0, 0, gObjUseSkill.m_SkillData.PoisonSkillTime);
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			if (retResistance(TargetObj, 1) == 0)
			{
				TargetObj.lpAttackObj = &Obj;
				TargetObj.m_SkillNumber = skill;
				gObjAddBuffEffect(TargetObj, BUFFTYPE_POISON, EFFECTTYPE_POISON_DMG_TICK, gObjUseSkill.m_SkillData.DeathPoisonPercent, 0, 0, 20);
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
		if ( gObjCheckUsedBuffEffect(TargetObj, BUFFTYPE_FREEZE) == FALSE )
		{
			if ( retResistance(TargetObj, 0) == 0 )
			{
				if (g_ConfigRead.EnableFreezeEffect == 1)
				{
					TargetObj.DelayActionTime = 800;
					TargetObj.DelayLevel = 1;
					TargetObj.lpAttackObj = &Obj;
					TargetObj.m_SkillNumber = skill;
					gObjAddBuffEffect(TargetObj, BUFFTYPE_FREEZE, EFFECTTYPE_REDUCE_MOVE_SPEED, 0, 0, 0, g_ConfigRead.EnableFreezeEffectTime);
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
		if (gObjCheckUsedBuffEffect(TargetObj, BUFFTYPE_STONE) == FALSE)
		{
			if (retResistance(TargetObj, 0) == 0)
			{
				TargetObj.lpAttackObj = &Obj;
				gObjAddBuffEffect(TargetObj, BUFFTYPE_STONE, 0, 0, 0, 0, gObjUseSkill.m_SkillData.IceArrowTime);
				TargetObj.PathCount = 0;
				TargetObj.PathStartEnd = 0;
				gObjSetPosition(TargetObj, TargetObj.X, TargetObj.Y);
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



BOOL CObjBaseAttack::MissCheck(CGameObject &Obj, CGameObject &TargetObj, int skill, int skillSuccess, int magicsend, BOOL& bAllMiss, BYTE RFAttack)
{
	int iAttackRate = 0;

	__try
	{
		iAttackRate = 0;
		int iDefenseRate = TargetObj.m_SuccessfulBlocking;

		BYTE MSBDamage = 0;	// MonsterSetBasse Damage

		if (IT_MAP_RANGE(TargetObj.MapNumber) && TargetObj.Type == OBJ_USER)
		{
			if (g_IT_Event.GetIllusionTempleState(TargetObj.MapNumber) == 2)
			{
				if (g_IT_Event.CheckSkillProdection(TargetObj.m_nITR_Index, TargetObj.MapNumber) == TRUE)
				{
					GSProtocol.GCDamageSend(&Obj, &TargetObj, 0, 0, 0, 0);
					return 0;
				}

				if (TargetObj.PartyNumber == Obj.PartyNumber)
				{
					GSProtocol.GCDamageSend(&Obj, &TargetObj, 0, 0, 0, 0);
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

		if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_BLIND_2) == true)
		{
			int nDecRate = 0;
			gObjGetValueOfBuffIndex(Obj, BUFFTYPE_BLIND_2, &nDecRate, 0);

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

				GSProtocol.GCDamageSend(&Obj, &TargetObj, 0, 0, MSBDamage, 0);

				if (magicsend != 0 )
				{
					GSProtocol.GCMagicAttackNumberSend(&Obj, skill, TargetObj.m_Index, skillSuccess);
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

				GSProtocol.GCDamageSend(&Obj, &TargetObj, 0, 0, MSBDamage, 0);

				if (magicsend != 0 )
				{
					GSProtocol.GCMagicAttackNumberSend(&Obj, skill, TargetObj.m_Index, skillSuccess);
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



BOOL CObjBaseAttack::MissCheckPvP(CGameObject &Obj , CGameObject &TargetObj, int skill, int skillSuccess, int magicsend, BOOL & bAllMiss, BYTE RFAttack)
{
	if (TargetObj.Type != OBJ_USER || Obj.Type != OBJ_USER)
	{
		return FALSE;
	}

	float iAttackRate = 0;
	float iDefenseRate = 0;
	int AttackLevel = Obj.Level + Obj.m_PlayerData->MasterLevel;
	int DefenseLevel = TargetObj.Level + TargetObj.m_PlayerData->MasterLevel;
	int iAttackSuccessRate = 0;

	if (IT_MAP_RANGE(TargetObj.MapNumber) && TargetObj.Type == OBJ_USER)
	{
		if (g_IT_Event.GetIllusionTempleState(TargetObj.MapNumber) == 2)
		{
			if (g_IT_Event.CheckSkillProdection(TargetObj.m_nITR_Index, TargetObj.MapNumber) == TRUE)
			{
				GSProtocol.GCDamageSend(&Obj, &TargetObj, 0, 0, 0, 0);
				return 0;
			}

			if (TargetObj.PartyNumber == Obj.PartyNumber)
			{
				GSProtocol.GCDamageSend(&Obj, &TargetObj, 0, 0, 0, 0);
				return 0;
			}
		}
	}

#if (ENABLE_CUSTOM_PVPBALANCE == 1)
	g_PvPBalance.modifyAttackRate(Obj, TargetObj, iAttackRate);
	g_PvPBalance.modifyDefenseRate(Obj, TargetObj, iDefenseRate);
#endif

	iAttackRate = Obj.m_PlayerData->m_AttackRatePvP;
	iDefenseRate = TargetObj.m_PlayerData->m_DefenseRatePvP;

	if ( iAttackRate <= 0.0f || iDefenseRate <= 0.0f || AttackLevel <= 0 ) return FALSE;

	iAttackRate += Obj.m_PlayerData->m_ItemOptionExFor380->OpAddAttackSuccessRatePVP;
	iDefenseRate += TargetObj.m_PlayerData->m_ItemOptionExFor380->OpAddDefenseSuccessRatePvP;

	iAttackRate += Obj.m_PlayerData->m_JewelOfHarmonyEffect->HJOpAddAttackSuccessRatePVP;
	iDefenseRate += TargetObj.m_PlayerData->m_JewelOfHarmonyEffect->HJOpAddDefenseSuccessRatePvP;

	iAttackRate += Obj.m_PlayerData->m_MPSkillOpt->iMpsIncreasePvPAttackRate;
	iDefenseRate += TargetObj.m_PlayerData->m_MPSkillOpt->iMpsIncreasePvPDefenseRate;
	
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

	if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_BLIND_2) == true)
	{
		int nDecRate = 0;
		gObjGetValueOfBuffIndex(Obj, BUFFTYPE_BLIND_2, &nDecRate, 0);

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

		GSProtocol.GCDamageSend(&Obj, &TargetObj, 0, 0, MsgDamage, 0);

		if ( g_ConfigRead.g_bShieldComboMissOptionOn == TRUE )
		{
			if ( Obj.m_PlayerData->comboSkill->ProgressIndex >= 0 )
			{
				sLog->outBasic("[Shield] ComboSkill Cancel! [%s][%s]",Obj.m_PlayerData->ConnectUser->AccountID, Obj.Name);
				Obj.m_PlayerData->comboSkill->Init();
			}
		}

		return FALSE;
	}

	return TRUE;
}

int  CObjBaseAttack::GetTargetDefense(CGameObject &Obj, CGameObject &TargetObj, int& MsgDamage, int& iOriginTargetDefense)
{
	int targetdefense = TargetObj.m_Defense;

	if (Obj.Type == OBJ_USER && TargetObj.Type == OBJ_USER)
	{
		targetdefense += TargetObj.m_PlayerData->m_ItemOptionExFor380->OpAddDefense / 2;
	}

	int decdef = gObjGetTotalValueOfEffect(TargetObj, EFFECTTYPE_DECREASE_DEFENSE);
	targetdefense -= decdef * targetdefense / 100;

	if (gObjCheckUsedBuffEffect(TargetObj, BUFFTYPE_ARCA_WATERTOWER) == true)
	{
		int nEffectValue = 0;
		gObjGetValueOfBuffIndex(TargetObj, BUFFTYPE_ARCA_WATERTOWER, &nEffectValue, 0);
		targetdefense += nEffectValue;
	}

	int nMuunItemEffectValue = 0;

	if (g_CMuunSystem.GetMuunItemValueOfOptType(TargetObj, MUUN_INC_DEFENSE, &nMuunItemEffectValue, 0) == TRUE)
	{
		targetdefense += 10 * nMuunItemEffectValue / 20;
	}
	
	if ( TargetObj.m_MonsterSkillElementInfo.m_iSkillElementDefenseTime > 0 )
	{
		targetdefense += TargetObj.m_MonsterSkillElementInfo.m_iSkillElementDefense;

		if ( targetdefense <0 )
			targetdefense = 0;

	}

	float percentdamage = 0;

	if ( Obj.pntInventory[7]->IsItem() != FALSE )
	{
		percentdamage += Obj.m_PlayerData->m_WingExcOption->iWingOpIgnoreEnemyDefense;
	}

	if ( gObjCheckUsedBuffEffect(Obj, BUFFTYPE_MONK_IGNORE_ENEMY_DEFENSE))
	{
		int nEffectValue1 = 0;
		int nEffectValue2 = 0;
		gObjGetValueOfBuffIndex(Obj, BUFFTYPE_MONK_IGNORE_ENEMY_DEFENSE, &nEffectValue1, &nEffectValue2);

		percentdamage += nEffectValue1;

		if (nEffectValue2 > 0)
		{
			if (rand() % 100 <= 10)
			{
				float nEffectValue = nEffectValue2 * TargetObj.m_SuccessfulBlocking / 100.0;
				gObjAddBuffEffect(TargetObj, BUFFTYPE_PENETRATE_ARMOR, EFFECTTYPE_DECREASE_DEFENSE, nEffectValue2, EFFECTTYPE_DECREASE_DEFENSE_RATE, nEffectValue, 10);
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

	if (TargetObj.Type == OBJ_USER)
	{
		percentdamage -= TargetObj.m_PlayerData->m_Resistance_Perfect;
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
	CGameObject* lpPartyObj;
	int partynum = Obj.PartyNumber;

	int partycount = gParty.m_PartyS[partynum].Count;
	int retcount = 0;

	for ( int n=0;n<MAX_USER_IN_PARTY;n++)
	{
		int memberindex = gParty.m_PartyS[partynum].Number[n];

		if ( memberindex >= 0 )
		{
			lpPartyObj = getGameObject(memberindex);

			if ( Obj.MapNumber == lpPartyObj->MapNumber )
			{
				int dis = gObjCalDistance(Obj, *lpPartyObj);
				
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

