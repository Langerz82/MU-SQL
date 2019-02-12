////////////////////////////////////////////////////////////////////////////////
// ObjUseSkill.cpp
#include "StdAfx.h"
#include "ObjUseSkill.h"
#include "ObjCalCharacter.h"
#include "GameMain.h"
#include "ChaosCastle.h"
#include "User/CUserData.h"
#include "zzzmathlib.h"
#include "SkillHitBox.h"
#include "MoveCommand.h"
#include "SkillAdditionInfo.h"
#include "BuffEffect.h"
#include "BuffEffectSlot.h"
#include "IllusionTempleEvent_Renewal.h"
#include "Logging/Log.h"
#include "Raklion.h"
#include "RaklionBattleUserMng.h"
#include "CastleSiegeSync.h"
#include "CastleSiege.h"
#include "util.h"
#include "MasterLevelSkillTreeSystem.h"
#include "GensSystem.h"
#include "configread.h"
#include "NewPVP.h"
#include "DoppelGanger.h"
#include "ObjBaseAttack.h"
#include "ObjAttack.h"
#include "SkillUseTime.h"

#include "MineSystem.h"
#include "PentagramSystem.h"
#include "BotSystem.h"
#include "Event/ArcaBattle/ArcaBattle.h"
#include "MapAttribute.h"
#include "SkillSafeZoneUse.h"

// GS-N 0.99.60T 0x0049AE30
//	GS-N	1.00.18	JPN	0x004B7770 - Completed
// todo: mastering skills for summoner
// Duration master skills not tested
CObjUseSkill gObjUseSkill;

CObjUseSkill::CObjUseSkill() : m_Lua(true)
{
	return;
}

CObjUseSkill::~CObjUseSkill()
{
	return;
}

bool CObjUseSkill::LoadSkillInfo()
{
	CIniReader ReadSkill(g_ConfigRead.GetPath("\\Skills\\IGC_SkillSettings.ini"));

	this->m_SkillData.SternTime = ReadSkill.ReadInt("SkillInfo", "SternTime", 10);
	this->m_SkillData.SwellManaTime = ReadSkill.ReadInt("SkillInfo", "SwellManaTime", 60);
	this->m_SkillData.CloakingTime = ReadSkill.ReadInt("SkillInfo", "CloakingTime", 60);
	this->m_SkillData.IceArrowTime = ReadSkill.ReadInt("SkillInfo", "IceArrowTime", 7);

	this->m_SkillData.PoisonPercent = ReadSkill.ReadInt("joinmu", "PoisonDamagePercent", 3); // PRIVATE CUSTOM - JOINMU
	this->m_SkillData.DeathPoisonPercent = ReadSkill.ReadInt("joinmu", "DeathPoisonPercent", 3); // PRIVATE CUSTOM - JOINMU

	this->m_SkillData.EnableSiegeOnAllMaps = ReadSkill.ReadInt("SiegeSkills", "EnableOnAllMaps", 1);
	this->m_SkillData.UseCalcForTripleShot = ReadSkill.ReadInt("joinmu", "UseLuaCalcForTripleShot", 0); // PRIVATE CUSTOM - JOINMU
	this->m_SkillData.PoisonSkillSuccessRate = ReadSkill.ReadInt("joinmu", "PoisonSkillSuccessRate", 10);
	this->m_SkillData.PoisonSkillTime = ReadSkill.ReadInt("joinmu", "PoisonSkillTime", 10);

	this->m_SkillData.EnableWizardSkillWhileTeleport = ReadSkill.ReadInt("SkillInfo", "CanDarkWizardUseSkillsWhileTeleport", 0);

	this->m_Lua.DoFile(g_ConfigRead.GetPath("\\Scripts\\Skills\\RegularSkillCalc.lua"));

	this->m_Lua.DoFile(g_ConfigRead.GetPath("\\Scripts\\Skills\\SkillRuudCalc.lua"));

	return true;
}

int  CObjUseSkill::GetTargetLinePath(int sx, int sy, int tx, int ty, int * PathX, int * PathY, int distance)
{
	int x, y;
	int delta_x, delta_y;
	int half;

	int error = 0;

	int incx = 1, incy = 1;
	int dis;
	int count;

	if (sx > tx)
	{
		incx = -1;
		delta_x = sx - tx;
	}
	else
	{
		delta_x = tx - sx;
	}

	if (sy > ty)
	{
		incy = -1;
		delta_y = sy - ty;
	}
	else
	{
		delta_y = ty - sy;
	}

	dis = sqrt((float)(delta_x * delta_x + delta_y * delta_y));
	dis = distance - dis;

	count = 0;
	PathX[count] = sx;
	PathY[count] = sy;
	count++;

	if (delta_x > delta_y)
	{
		y = sy;
		half = delta_x / 2;

		if (incx > 0)
		{
			for (x = (sx + 1); x <= (tx + dis); x++)
			{
				error += delta_y;

				if (error > half)
				{
					y += incy;
					error -= delta_x;
				}

				PathX[count] = x;
				PathY[count] = y;

				count++;
			}
		}
		else
		{
			for (x = (sx - 1); x >= (tx - dis); x--)
			{
				error += delta_y;

				if (error > half)
				{
					y += incy;
					error -= delta_x;
				}

				PathX[count] = x;
				PathY[count] = y;

				count++;
			}
		}
	}
	else
	{
		x = sx;
		half = delta_y / 2;

		if (incy > 0)
		{
			for (y = (sy + 1); y <= (ty + dis); y++)
			{
				error += delta_x;

				if (error > half)
				{
					x += incx;
					error -= delta_y;
				}

				PathX[count] = x;
				PathY[count] = y;
				count++;
			}
		}
		else
		{
			for (y = (sy - 1); y >= (ty - dis); y--)
			{
				error += delta_x;

				if (error > half)
				{
					x += incx;
					error -= delta_y;
				}

				PathX[count] = x;
				PathY[count] = y;
				count++;
			}
		}
	}
	return count;
}



BOOL CObjUseSkill::SpeedHackCheck(CGameObject &lpObj)
{
	

	int iTimeCalc = GetTickCount() - lpObj.m_LastAttackTime;

	if (lpObj.Type != OBJ_USER)
	{
		return TRUE;
	}

	if (iTimeCalc < lpObj.m_DetectSpeedHackTime)
	{
		lpObj.m_DetectCount++;
		lpObj.m_SumLastAttackTime += iTimeCalc;

		if (lpObj.m_DetectCount > gHackCheckCount)
		{
			lpObj.m_DetectedHackKickCount++;

			if (gIsKickDetecHackCountLimit != 0 && lpObj.m_DetectedHackKickCount > gDetectedHackKickCount)
			{
				sLog->outBasic("[%s][%s] Kick DetecHackCountLimit Over User (%d)", lpObj.AccountID, lpObj.Name, lpObj.m_DetectedHackKickCount);
				GSProtocol.GCSendDisableReconnect(aIndex);
				//IOCP.CloseClient(aIndex);
				return false;
			}

			lpObj.m_SpeedHackPenalty = gSpeedHackPenalty;

			if (gIsKickDetecHackCountLimit != 0)
			{
				sLog->outBasic("[%s][%s] Attack Speed Is Wrong MagicSkill (%d)(%d) Penalty %d", lpObj.AccountID, lpObj.Name, lpObj.m_DetectSpeedHackTime, lpObj.m_SumLastAttackTime / lpObj.m_DetectCount, lpObj.m_SpeedHackPenalty);
			}
		}
	}
	else
	{
		lpObj.m_SumLastAttackTime = 0;
		lpObj.m_DetectCount = 0;
	}

	lpObj.m_LastAttackTime = GetTickCount();

	if (bIsIgnorePacketSpeedHackDetect != 0 && lpObj.m_SpeedHackPenalty > 0)
	{
		lpObj.m_SpeedHackPenalty--;
		sLog->outBasic("[%s][%s] Apply Attack Speed Penalty (%d left)", lpObj.AccountID, lpObj.Name, lpObj.m_SpeedHackPenalty);
		return false;
	}
	return true;
}


BOOL CObjUseSkill::EnableSkill(WORD Skill)
{
	if (g_MasterLevelSkillTreeSystem.CheckMasterLevelSkill(Skill))
	{
		int iBaseMLS = g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(Skill);

		if (iBaseMLS == AT_MSKILL_DW_MANASHIELD1
			|| iBaseMLS == 388
			|| iBaseMLS == 330
			|| iBaseMLS == 413
			|| iBaseMLS == 414
			|| iBaseMLS == 481
			|| iBaseMLS == 356
			|| iBaseMLS == 382
			|| iBaseMLS == 512
			|| iBaseMLS == 508
			|| iBaseMLS == 385
			|| iBaseMLS == 391
			|| iBaseMLS == 336
			|| iBaseMLS == 331
			|| iBaseMLS == 417
			|| iBaseMLS == 420
			|| iBaseMLS == 490
			|| iBaseMLS == 482
			|| iBaseMLS == 518
			|| iBaseMLS == 487
			|| iBaseMLS == 454
			|| iBaseMLS == 455
			|| iBaseMLS == 456
			|| iBaseMLS == 458
			|| iBaseMLS == 326
			|| iBaseMLS == 327
			|| iBaseMLS == 328
			|| iBaseMLS == AT_MSKILL_DK_LUNGE1
			|| iBaseMLS == 332
			|| iBaseMLS == 333
			|| iBaseMLS == 337
			|| iBaseMLS == 339
			|| iBaseMLS == 340
			|| iBaseMLS == 342
			|| iBaseMLS == 343
			|| iBaseMLS == 344
			|| iBaseMLS == 346
			|| iBaseMLS == 360
			|| iBaseMLS == 363
			|| iBaseMLS == 378
			|| iBaseMLS == 379
			|| iBaseMLS == AT_MSKILL_DW_EXPWIZARDRY1
			|| iBaseMLS == 381
			|| iBaseMLS == AT_MSKILL_DW_EXPWIZARDRY2
			|| iBaseMLS == 384
			|| iBaseMLS == 386
			|| iBaseMLS == 387
			|| iBaseMLS == 389
			|| iBaseMLS == 392
			|| iBaseMLS == 393
			|| iBaseMLS == 404
			|| iBaseMLS == 406
			|| iBaseMLS == 415
			|| iBaseMLS == 416
			|| iBaseMLS == 418
			|| iBaseMLS == 419
			|| iBaseMLS == 425
			|| iBaseMLS == 426
			|| iBaseMLS == 427
			|| iBaseMLS == 428
			|| iBaseMLS == 429
			|| iBaseMLS == 430
			|| iBaseMLS == 432
			|| iBaseMLS == 433
			|| iBaseMLS == 441
			|| iBaseMLS == 459
			|| iBaseMLS == 460
			|| iBaseMLS == 461
			|| iBaseMLS == 462
			|| iBaseMLS == 466
			|| iBaseMLS == 468
			|| iBaseMLS == AT_MSKILL_SUM_BERSERK1
			|| iBaseMLS == 470
			|| iBaseMLS == 472
			|| iBaseMLS == 479
			|| iBaseMLS == 480
			|| iBaseMLS == 483
			|| iBaseMLS == 484
			|| iBaseMLS == 486
			|| iBaseMLS == 488
			|| iBaseMLS == 489
			|| iBaseMLS == 492
			|| iBaseMLS == 493
			|| iBaseMLS == 494
			|| iBaseMLS == 495
			|| iBaseMLS == 496
			|| iBaseMLS == 497
			|| iBaseMLS == 509
			|| iBaseMLS == 510
			|| iBaseMLS == 514
			|| iBaseMLS == 515
			|| iBaseMLS == 516
			|| iBaseMLS == 517
			|| iBaseMLS == 519
			|| iBaseMLS == 520
			|| iBaseMLS == 523
			|| iBaseMLS == 539
			|| iBaseMLS == 551
			|| iBaseMLS == 552
			|| iBaseMLS == 554
			|| iBaseMLS == 555
			|| iBaseMLS == 556
			|| iBaseMLS == 558
			|| iBaseMLS == 559
			|| iBaseMLS == 560
			|| iBaseMLS == 561
			|| iBaseMLS == 562
			|| iBaseMLS == 563
			|| iBaseMLS == 564
			|| iBaseMLS == 565
			|| iBaseMLS == 566
			|| iBaseMLS == 567
			|| iBaseMLS == 575
			|| iBaseMLS == 577
			|| iBaseMLS == 411
			|| iBaseMLS == 424
			|| iBaseMLS == 431
			|| iBaseMLS == 463
			|| iBaseMLS == 491
			|| iBaseMLS == 522
			|| iBaseMLS == 521
			|| iBaseMLS == 524
			|| iBaseMLS == 631
			|| iBaseMLS == AT_MSKILL_OBSIDIAN_POWUP
			|| iBaseMLS == 696
			|| iBaseMLS == 697
			|| iBaseMLS == 698
			|| iBaseMLS == 699
			|| iBaseMLS == AT_MSKILL_CIRCLE_SHIELD_POWUP
			|| iBaseMLS == AT_MSKILL_CIRCLE_SHIELD_MASTERY
			|| iBaseMLS == AT_MSKILL_WRATH_POWUP
			|| iBaseMLS == AT_MSKILL_WRATH_PROFICIENCY
			|| iBaseMLS == AT_MSKILL_WRATH_MASTERY)
		{
			return true;
		}
	}

	if (Skill != AT_SKILL_STORM
		&& Skill != AT_SKILL_EVIL
		&& Skill != AT_SKILL_FLAME
		&& Skill != AT_SKILL_HELL
		&& Skill != AT_SKILL_BLAST
		&& Skill != AT_SKILL_INFERNO
		&& Skill != AT_SKILL_CROSSBOW
		&& Skill != AT_SKILL_DEVILFIRE
		&& Skill != AT_SKILL_FLASH
		&& Skill != AT_SKILL_WHEEL
		&& Skill != AT_SKILL_SWORD6
		&& Skill != AT_SKILL_KNIGHTSPEAR
		&& Skill != AT_SKILL_KNIGHTADDLIFE
		&& Skill != AT_SKILL_STRIKE
		&& Skill != AT_SKILL_KNIGHTDINORANT
		&& Skill != AT_SKILL_ELFHARDEN
		&& Skill != AT_SKILL_PENETRATION
		&& Skill != AT_SKILL_DEFENSEDOWN
		&& Skill != AT_SKILL_MAGICDEFENSE
		&& Skill != AT_SKILL_BLOWOFFURY
		&& Skill != AT_SKILL_EXPPOISON
		&& Skill != AT_SKILL_EXPICE
		&& Skill != AT_SKILL_EXPHELL
		&& Skill != AT_SKILL_ELECTRICSPARK
		&& Skill != AT_SKILL_DARKHORSE_ATTACK
		&& Skill != AT_SKILL_BRAND_OF_SKILL
		&& Skill != AT_SKILL_STUN
		&& Skill != AT_SKILL_REMOVAL_STUN
		&& Skill != AT_SKILL_ADD_MANA
		&& Skill != AT_SKILL_INVISIBLE
		&& Skill != AT_SKILL_REMOVAL_INVISIBLE
		&& Skill != AT_SKILL_REMOVAL_MAGIC
		&& Skill != AT_SKILL_FENRIR_ATTACK
		&& Skill != AT_SKILL_INFINITY_ARROW
		&& Skill != AT_SKILL_FIRESCREAM
		&& Skill != AT_SKILL_SAHAMUTT
		&& Skill != AT_SKILL_NEIL
		&& Skill != AT_SKILL_RAGEUL
		&& Skill != AT_SKILL_LIGHTING_SHOCK
		&& Skill != AT_SKILL_SLEEP
		&& Skill != AT_SKILL_DRAIN_LIFE
		&& Skill != AT_SKILL_DAMAGE_REFLECT
		&& Skill != AT_SKILL_INNOVATION
		&& Skill != AT_SKILL_WEAKNESS
		&& Skill != AT_SKILL_CHAIN_LIGHTING
		&& Skill != AT_SKILL_BLOW_OF_DESTRUCTION
		&& Skill != AT_SKILL_MAGIC_CIRCLE
		&& Skill != AT_SKILL_SD_RECOVERY
		&& Skill != AT_SKILL_FLAME_STRIKE
		&& Skill != AT_SKILL_GIGANTIC_STORM
		&& Skill != AT_SKILL_BIRDS
		&& Skill != AT_SKILL_SUICIDE
		&& Skill != AT_SKILL_FIVE_SHOT
		&& Skill != AT_SKILL_BERSERKER
		&& Skill != AT_SKILL_LARGE_BLOWER
		&& Skill != AT_SKILL_REDARMOR_IGNORE
		&& Skill != AT_SKILL_FITNESS
		&& Skill != AT_SKILL_DSRINCREASE
		&& Skill != AT_SKILL_DARK_SIDE
		&& Skill != AT_SKILL_CHAIN_DRIVE
		&& Skill != AT_SKILL_DRAGON_SLASH
		&& Skill != AT_SKILL_DRAGON_ROAR
		&& Skill != AT_SKILL_PHOENIX_SHOT
		&& Skill != AT_SKILL_SPIN_STEP
		&& Skill != AT_SKILL_CIRCLE_SHIELD
		&& Skill != AT_SKILL_OBSIDIAN
		&& Skill != AT_SKILL_MAGIC_PIN
		&& Skill != AT_SKILL_CLASH
		&& Skill != AT_SKILL_HARSH_STRIKE
		&& Skill != AT_SKILL_SHINING_PEAK
		&& Skill != AT_SKILL_WRATH
		&& Skill != AT_SKILL_BRECHE
		&& Skill != AT_SKILL_LORDSILVESTER_SUMMON
		&& Skill != AT_SKILL_RUSH
		&& Skill != AT_SKILL_FIRE_BLOW
		&& Skill != AT_SKILL_METEOR_STRIKESM
		&& Skill != AT_SKILL_METEOR_STORM
		&& Skill != AT_SKILL_SOUL_SAKER
		&& Skill != AT_SKILL_FOCUS_SHOT
		&& Skill != AT_SKILL_FIRE_BIRD
		&& Skill != AT_SKILL_AQUA_BIST
		&& Skill != AT_SKILL_ICE_BLOOD
		&& Skill != AT_SKILL_FIRE_BLOOD
		&& Skill != AT_SKILL_DARKBLAST
		&& Skill != AT_SKILL_METEOR_STRIKEMG
		&& Skill != AT_SKILL_WINDSOUL
		&& Skill != AT_SKILL_DARK_PHOENIX_SHOT
		&& Skill != AT_SKILL_ARCHANGEL_WILL)
	{
		return false;
	}

	return true;
}



int  CObjUseSkill::GetUseMana(CGameObject &lpObj, CMagicInf * lpMagic)
{
	
	float mana = lpObj.Mana;

	if (lpObj.Type != OBJ_USER)
	{
		return 0;
	}

	if (lpMagic == NULL)
	{
		return 0;
	}

	float DecreaseMana;

	if (g_MasterLevelSkillTreeSystem.CheckMasterLevelSkill(lpMagic->m_Skill) == true)
	{
		DecreaseMana = MagicDamageC.SkillGetMana(lpMagic->m_Skill);
		DecreaseMana += DecreaseMana * g_MasterLevelSkillTreeSystem.GetMasterSkillManaIncRate(lpMagic->m_Level) / 100.0;
		DecreaseMana += this->GetAddUseMana(lpObj, lpMagic);
	}

	else
	{
		DecreaseMana = MagicDamageC.SkillGetMana(lpMagic->m_Skill);
		DecreaseMana += this->GetAddUseMana(lpObj, lpMagic);
	}

	DecreaseMana -= DecreaseMana * lpObj.m_PlayerData->m_MPSkillOpt.iMpsDecreaseMana / 100.0;
	
	if (DecreaseMana < 1.0)
		DecreaseMana = 1.0;

	mana -= DecreaseMana;

	if (mana < 0.0f)
	{
		return -1;
	}

	return mana;
}



int  CObjUseSkill::GetUseBP(CGameObject &lpObj, CMagicInf * lpMagic)
{
	

	int bp = lpObj.BP;

	if (lpObj.Type != OBJ_USER)
	{
		return 0;
	}

	if (lpMagic == 0)
	{
		return 0;
	}

	int usingbp = MagicDamageC.SkillGetBP(lpMagic->m_Skill);

	usingbp -= usingbp * lpObj.m_PlayerData->SetOpDecreaseAG / 100;

	if (lpObj.m_PlayerData->m_btAGReduceRate > 0)
		usingbp -= usingbp * lpObj.m_PlayerData->m_btAGReduceRate / 100;

	bp -= usingbp;

	if (bp < 0)
	{
		return -1;
	}

	return bp;
}

void CObjUseSkill::UseSkill(CGameObject &lpObj, CMagicInf * lpMagic, BYTE x, BYTE y, BYTE dir, BYTE TargetPos, int aTargetIndex)
{
	

	int bCombo = 0;

	if (gComboAttack.CheckCombo(aIndex, lpMagic->m_Skill))
	{
		bCombo = 1;
	}

	if (g_SkillUseTime.CheckSkillTime(lpObj, lpMagic->m_Skill) == false)
	{
		return;
	}

	if (g_ConfigRead.EnableAttackBlockInSafeZone == TRUE)
	{
		BYTE btAttr = MapC[lpObj.MapNumber].GetAttr(lpObj.X, lpObj.Y);

		if ((btAttr & 1) == 1)
		{
			if (g_SkillSafeZone.CanUseSkill(lpObj, lpMagic) == false)
			{
				return;
			}
		}
	}

	if (lpObj.Type == OBJ_USER)
	{
		if (lpObj.m_PlayerData->SkillDelay.Check(lpMagic->m_Skill) == FALSE)
		{
			return;
		}

		if (lpObj.m_PlayerData->SkillHellFire2State && lpMagic->m_Skill != AT_SKILL_EXPHELL)
		{
			CMagicInf * lpMagic = gObjGetMagicSearch(lpObj, AT_SKILL_EXPHELL);

			if (lpMagic != 0)
			{
				gObjUseSkill.RunningSkill(lpObj.m_Index, 0, lpMagic, 0);
			}

			return;
		}

		else if (lpObj.m_PlayerData->SkillStrengthenHellFire2State && lpMagic->m_Skill != 392)
		{
			CMagicInf * lpMagic = gObjGetMagicSearch(lpObj, 392);

			if (lpMagic != 0)
			{
				gObjUseSkill.RunningSkill(lpObj.m_Index, 0, lpMagic, 0);
			}

			return;
		}
	}

	int usemana = this->GetUseMana(aIndex, lpMagic);

	if (lpMagic->m_Skill == AT_SKILL_DARKHORSE_ATTACK) //season4 add-on
	{
		usemana = lpObj.Mana;
	}

	if (usemana >= 0)
	{
		int usebp = gObjMagicBPUse(&gGameObjects[aIndex], lpMagic);

		if (usebp >= 0)
		{
			lpObj.UseMagicNumber = lpMagic->m_Skill;
			lpObj.UseMagicTime = GetTickCount();
			lpObj.UseMagicCount = 0;

			lpObj.Mana = usemana;
			lpObj.BP = usebp;

			GSProtocol.GCManaSend(lpObj.m_Index, lpObj.Mana, 0xFF, 0, lpObj.BP);

			PMSG_DURATION_MAGIC_SEND pMsg;

			PHeadSetBE((PBYTE)&pMsg, 0x1E, sizeof(pMsg));

			pMsg.MagicNumberH = SET_NUMBERH(lpMagic->m_Skill);
			pMsg.MagicNumberL = SET_NUMBERL(lpMagic->m_Skill);
			pMsg.X = x;
			pMsg.Y = y;
			pMsg.Dir = dir;
			pMsg.NumberH = SET_NUMBERH(aIndex);
			pMsg.NumberL = SET_NUMBERL(aIndex);

			if (lpObj.Type == OBJ_USER)
			{
				IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (PBYTE)&pMsg, pMsg.h.size);
			}

			if (CC_MAP_RANGE(lpObj.MapNumber) || lpObj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
			{
				if (g_MasterLevelSkillTreeSystem.CheckMasterLevelSkill(lpMagic->m_Skill) != FALSE) //Season3 add-on
				{
					int GetSkillID = g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill);
					switch (GetSkillID)
					{
					case 356:	//1.01.00
					case 360:
					case 363:
					case AT_MSKILL_DW_MANASHIELD1:
					case 404:
					case 406:
					case 413:
					case 417:
					case 420:
						break;
					default:
						GSProtocol.MsgSendV2(lpObj, (LPBYTE)&pMsg, pMsg.h.size);
					}
				}
				else
				{
					switch (lpMagic->m_Skill)
					{
					case AT_SKILL_MAGICDEFENSE:
					case AT_SKILL_HEALING:
					case AT_SKILL_DEFENSE:
					case AT_SKILL_ATTACK:
					case AT_SKILL_KNIGHTADDLIFE:
						break;
					default:
						GSProtocol.MsgSendV2(lpObj, (PBYTE)&pMsg, pMsg.h.size);
					}
				}
			}
			else
			{
				GSProtocol.MsgSendV2(lpObj, (PBYTE)&pMsg, pMsg.h.size);
			}
		}

		if (g_MasterLevelSkillTreeSystem.CheckMasterLevelSkill(lpMagic->m_Skill) == true)
		{
			g_MasterLevelSkillTreeSystem.RunningSkill_MLS(lpObj, aTargetIndex, lpMagic, bCombo, x, y, TargetPos);
			return;
		}

		if (lpMagic->m_Skill == AT_SKILL_SWORD6)
		{
			this->SkillPowerSlash(aIndex, lpMagic, x, y, dir, TargetPos, aTargetIndex);
		}

		if (lpMagic->m_Skill == AT_SKILL_FIRE_BLOOD)
		{
			this->SkillFireBlood(lpObj, lpMagic, aTargetIndex);
		}

		if (lpMagic->m_Skill == AT_SKILL_ICE_BLOOD)
		{
			this->SkillIceBlood(lpObj, lpMagic, aTargetIndex);
		}

		if (lpMagic->m_Skill == AT_SKILL_EXPPOISON)
		{
			if (lpObj.Class == 561 && lpMagic->m_Skill == AT_SKILL_EXPPOISON)
			{
				this->SkillDeathPoisonForMedusa(aIndex, lpMagic, x, y, aTargetIndex);
			}

			else
			{
				this->SkillDeathPoison(aIndex, lpMagic, x, y, dir, TargetPos, aTargetIndex);
			}
		}
		else if (lpMagic->m_Skill == AT_SKILL_SAHAMUTT)
		{
			this->SkillBookSahamutt(aIndex, lpMagic, x, y, aTargetIndex);
		}
		else if (lpMagic->m_Skill == AT_SKILL_ARCHANGEL_WILL)
		{
			this->SkillArchangelWillBuff(aIndex, lpMagic);
		}
		else if (lpMagic->m_Skill == AT_SKILL_NEIL)
		{
			this->SkillBookNeil(aIndex, lpMagic, x, y, aTargetIndex);
		}
		else if (lpMagic->m_Skill == AT_SKILL_EXPICE)
		{
			this->SkillSuddenIce(aIndex, lpMagic, x, y, dir, TargetPos, aTargetIndex);
		}
		else if (lpMagic->m_Skill == AT_SKILL_BLOWOFFURY)
		{
			this->SkillBlowOfFury(aIndex, lpMagic, x, y, dir, TargetPos, aTargetIndex, bCombo);
		}
		else if (lpMagic->m_Skill == AT_SKILL_WHEEL)
		{
			this->SkillWheel(aIndex, lpMagic, aTargetIndex, bCombo);
		}
		else if (lpMagic->m_Skill == AT_SKILL_ELECTRICSPARK)
		{
			this->SkillElectricSpark(aIndex, lpMagic, x, y, dir, TargetPos, aTargetIndex);
		}
		else if (lpMagic->m_Skill == AT_SKILL_DARKHORSE_ATTACK)
		{
			this->SkillDarkHorseAttack(lpObj.m_Index, aTargetIndex, lpMagic);
		}
		else if (lpMagic->m_Skill == AT_SKILL_STUN)
		{
			this->SkillStun(aIndex, aTargetIndex, lpMagic, x, y, dir, TargetPos);
		}
		else if (lpMagic->m_Skill == AT_SKILL_FENRIR_ATTACK)
		{
			this->SkillFenrirAttack(aIndex, aTargetIndex, lpMagic);
		}
		else if (lpMagic->m_Skill == AT_SKILL_DRAIN_LIFE)
		{
			this->SkillDrainLife(aIndex, lpMagic, aTargetIndex);
		}
		else if (lpMagic->m_Skill == AT_SKILL_WEAKNESS)
		{
			this->SkillWeakness(aIndex, lpMagic, x, y, aTargetIndex);
		}
		else if (lpMagic->m_Skill == AT_SKILL_INNOVATION)
		{
			this->SkillInnovation(aIndex, lpMagic, x, y, aTargetIndex);
		}
		else if (lpMagic->m_Skill == AT_SKILL_CHAIN_LIGHTING)
		{
			this->SkillChainLighting(aIndex, lpMagic, aTargetIndex);
		}
		else if (lpMagic->m_Skill == AT_SKILL_BLOW_OF_DESTRUCTION)
		{
			this->SkillBlowOfDestruction(aIndex, lpMagic, x, y, aTargetIndex, bCombo);
		}
		else if (lpMagic->m_Skill == AT_SKILL_RAGEUL)
		{
			this->NewSkillProc(aIndex, lpMagic, x, y, aTargetIndex, 59, 0);
		}
		else if (lpMagic->m_Skill == AT_SKILL_LIGHTING_SHOCK)
		{
			this->SkillLightingShock(aIndex, lpMagic, aTargetIndex);
		}
		else if (lpMagic->m_Skill == AT_SKILL_GIGANTIC_STORM)
		{
			this->SkillGiganticStorm(aIndex, lpMagic, x, y, aTargetIndex);
		}
		else if (lpMagic->m_Skill == AT_SKILL_FLAME_STRIKE)
		{
			this->SkillFlameStrike(aIndex, lpMagic, TargetPos, aTargetIndex);
		}
		else if (lpMagic->m_Skill == AT_SKILL_BIRDS)
		{
			this->SkillBirds(aIndex, lpMagic, TargetPos, aTargetIndex);
		}

		else if (lpMagic->m_Skill == AT_SKILL_FIVE_SHOT)
		{
			this->SkillFiveShot(aIndex, lpMagic, TargetPos, aTargetIndex);
		}

		else if (lpMagic->m_Skill == AT_SKILL_SUICIDE)
		{
			g_DoppelGanger.SelfExplosion(aIndex, lpMagic, aTargetIndex);
		}

		else if (lpMagic->m_Skill == AT_SKILL_LORDSILVESTER_SUMMON)
		{
			this->SkillSummonForLordSilvester(aIndex, aTargetIndex, lpMagic);
		}

		else if (lpMagic->m_Skill == AT_SKILL_DRAGON_ROAR)
		{
			if (ObjectMaxRange(aTargetIndex) == FALSE)
			{
				sLog->outBasic("[InvalidTargetIndex][CObjUseSkill.UseSkill][AT_SKILL_DRAGON_ROAR] Index :%d , AccountID : %s ", aIndex, lpObj.AccountID);
				return;
			}

			this->SkillAreaMonsterAttack(aIndex, lpMagic, gGameObjects[aTargetIndex]->X, gGameObjects[aTargetIndex]->Y, aTargetIndex, 3, 1, 0);
		}

		else if (lpMagic->m_Skill == AT_SKILL_DARK_SIDE)
		{
			this->SkillDarkSide(aIndex, aTargetIndex, lpMagic);
		}

		else if (lpMagic->m_Skill == AT_SKILL_PHOENIX_SHOT || lpMagic->m_Skill == AT_SKILL_DARK_PHOENIX_SHOT)
		{
			if (ObjectMaxRange(aTargetIndex) == FALSE)
			{
				sLog->outBasic("[InvalidTargetIndex][CObjUseSkill.UseSkill][AT_SKILL_PHOENIXSHOT] Index :%d , AccountID : %s ", aIndex, lpObj.AccountID);
				return;
			}

			this->SkillAreaMonsterAttack(aIndex, lpMagic, gGameObjects[aTargetIndex]->X, gGameObjects[aTargetIndex]->Y, aTargetIndex, 2, 1, 0);
		}

		else if (lpMagic->m_Skill == AT_SKILL_BRECHE || lpMagic->m_Skill == 696 || lpMagic->m_Skill == 697 || lpMagic->m_Skill == 698)
		{
			this->SkillBreche(aIndex, lpMagic, x, y, aTargetIndex);
		}

		else if (lpMagic->m_Skill == AT_SKILL_WINDSOUL)
		{
			this->SkillWindSoul(lpObj, lpMagic, aTargetIndex);
		}
		else if (lpMagic->m_Skill == AT_SKILL_DARKBLAST)
		{
			this->SkillDarkBlast(lpObj, lpMagic, aTargetIndex);
		}
		else if (lpMagic->m_Skill == AT_SKILL_METEOR_STORM)
		{
			this->SkillMeteorStorm(aIndex, lpMagic, x, y, aTargetIndex);
		}
		else if (lpMagic->m_Skill == AT_SKILL_FIRE_BLOW)
		{
			this->SkillKnightBlow(lpObj.m_Index, aTargetIndex, lpMagic, bCombo);
		}
		else if (lpMagic->m_Skill == AT_SKILL_ICE_BLOOD)
		{
			this->SkillAreaMonsterAttack(aIndex, lpMagic, gGameObjects[aTargetIndex]->X, gGameObjects[aTargetIndex]->Y, aTargetIndex, 3, 1, 0);
		}
		else if (lpMagic->m_Skill == AT_SKILL_SOUL_SAKER)
		{
			this->SkillSoulSeker(lpObj, lpMagic, aTargetIndex);
		}
		else if (lpMagic->m_Skill == AT_SKILL_FIRE_BIRD)
		{
			this->SkillFireBeast(aIndex, aTargetIndex, lpMagic);
		}
		else if (lpMagic->m_Skill == AT_SKILL_AQUA_BIST)
		{
			this->SkillIceBeast(aIndex, aTargetIndex, lpMagic);
		}
	}
}



void CObjUseSkill::UseSkill(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic)
{
	
	CGameObject lpTargetObj = &gGameObjects[aTargetIndex];
	BOOL bCombo = FALSE;

	if (g_SkillUseTime.CheckSkillTime(lpObj, lpMagic->m_Skill) == false)
	{
		return;
	}

	if (g_ConfigRead.EnableAttackBlockInSafeZone == TRUE)
	{
		BYTE btAttr = MapC[lpObj.MapNumber].GetAttr(lpObj.X, lpObj.Y);

		if ((btAttr & 1) == 1)
		{
			if (g_SkillSafeZone.CanUseSkill(lpObj, lpMagic) == false)
			{
				return;
			}
		}
	}
	if (gComboAttack.CheckCombo(aIndex, lpMagic->m_Skill))
	{
		bCombo = TRUE;
	}

	if (lpObj.Type == OBJ_USER)
	{
		if (lpObj.m_PlayerData->SkillDelay.Check(lpMagic->m_Skill) == FALSE)
		{
			return;
		}
		if (lpObj.m_PlayerData->SkillHellFire2State && lpMagic->m_Skill != AT_SKILL_EXPHELL)
		{
			CMagicInf * lpMagic = gObjGetMagicSearch(lpObj, AT_SKILL_EXPHELL);

			if (lpMagic != 0)
			{
				gObjUseSkill.RunningSkill(lpObj.m_Index, 0, lpMagic, 0);
			}

			return;
		}

		else if (lpObj.m_PlayerData->SkillStrengthenHellFire2State && lpMagic->m_Skill != 392)
		{
			CMagicInf * lpMagic = gObjGetMagicSearch(lpObj, 392);

			if (lpMagic != 0)
			{
				gObjUseSkill.RunningSkill(lpObj.m_Index, 0, lpMagic, 0);
			}

			return;
		}
	}

	if (lpMagic->m_Skill == AT_SKILL_EXPHELL && aTargetIndex != 58)
	{
		this->RunningSkill(aIndex, aTargetIndex, lpMagic, bCombo);
		lpObj.UseMagicNumber = lpMagic->m_Skill;
		lpObj.UseMagicTime = GetTickCount();
		lpObj.UseMagicCount = 0;
		return;
	}

	if (lpMagic->m_Skill == 392 && aTargetIndex != 395)
	{
		this->RunningSkill(aIndex, aTargetIndex, lpMagic, bCombo);
		lpObj.UseMagicNumber = lpMagic->m_Skill;
		lpObj.UseMagicTime = GetTickCount();
		lpObj.UseMagicCount = 0;
		return;
	}

	if (lpMagic->m_Skill == AT_SKILL_DARK_SIDE || lpMagic->m_Skill == 559 || lpMagic->m_Skill == 563)
	{
		if (lpObj.Type == OBJ_USER && lpObj.m_PlayerData->DarkSideCount > 1)
		{
			this->RunningSkill(aIndex, aTargetIndex, lpMagic, bCombo);
			lpObj.UseMagicNumber = lpMagic->m_Skill;
			lpObj.UseMagicTime = GetTickCount();
			lpObj.UseMagicCount = 0;
			return;
		}
	}

	int usemana = this->GetUseMana(aIndex, lpMagic);

	if (usemana >= 0)
	{
		int usebp = this->GetUseBP(aIndex, lpMagic);

		if (usebp >= 0)
		{
			if (this->RunningSkill(aIndex, aTargetIndex, lpMagic, bCombo) == 1)
			{
				lpObj.UseMagicNumber = lpMagic->m_Skill;
				lpObj.UseMagicTime = GetTickCount();
				lpObj.UseMagicCount = 0;

				lpObj.Mana = usemana;
				lpObj.BP = usebp;

				GSProtocol.GCManaSend(aIndex, lpObj.Mana, 0xFF, 0, lpObj.BP);
			}
		}
	}
}



BOOL CObjUseSkill::RunningSkill(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic, BOOL bCombo)
{
	
	CGameObject lpTargetObj = &gGameObjects[aTargetIndex];
	WORD MagicNumber = lpMagic->m_Skill;

	lpObj.m_SkillNumber = MagicNumber;

	if (gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_INVISABLE) == TRUE && (lpObj.Authority & 0x20) != 0x20)
	{
		this->RemoveCloakingEffect(lpObj.m_Index);
	}

	if (g_MasterLevelSkillTreeSystem.CheckMasterLevelSkill(lpMagic->m_Skill))
	{
		return g_MasterLevelSkillTreeSystem.RunningSkill_MLS(lpObj, aTargetIndex, lpMagic, bCombo, 0, 0, 0);
	}

	if (lpObj.Type == OBJ_USER && lpObj.m_PlayerData->DarkSideCount > 0)
	{
		if (lpMagic->m_Skill != 263 && lpMagic->m_Skill != 559 && lpMagic->m_Skill != 563)
		{
			for (int i = 0; i < 5; i++)
			{
				lpObj.m_PlayerData->DarkSideTarget[i] = 10000;
			}

			lpObj.m_PlayerData->DarkSideCount = 0;
			return FALSE;
		}

		BOOL bAttack = FALSE;
		for (int j = 0; j < 5; j++)
		{
			if (lpObj.m_PlayerData->DarkSideTarget[j] == aTargetIndex)
			{
				lpObj.m_PlayerData->DarkSideTarget[j] = 10000;
				--lpObj.m_PlayerData->DarkSideCount;
				bAttack = TRUE;
				break;
			}
		}

		if (bAttack == FALSE)
		{
			for (int i = 0; i < 5; i++)
			{
				lpObj.m_PlayerData->DarkSideTarget[i] = 10000;
			}

			lpObj.m_PlayerData->DarkSideCount = 0;
			return FALSE;
		}
	}

	else if (lpObj.Type == OBJ_USER && lpObj.m_PlayerData->DarkSideCount == 0)
	{
		if (lpMagic->m_Skill == 263 || lpMagic->m_Skill == 559 || lpMagic->m_Skill == 563)
		{
			return TRUE;
		}
	}

	switch (MagicNumber)
	{
	case AT_SKILL_BLOCKING:
		if (lpObj.pInventory[1].IsItem() == 1)
		{
			if (lpObj.pInventory[1].m_Type >= ITEMGET(6, 4) && lpObj.pInventory[1].m_Type < ITEMGET(7, 0))
			{
				if (lpObj.pInventory[1].m_Option1 != 0)
				{
					lpObj.m_SkillTime = GetTickCount() + 4000;
					GSProtocol.GCActionSend(lpObj, 18, lpObj.m_Index, aTargetIndex);
				}
			}
		}
		break;
	case AT_SKILL_POISON:
	case AT_SKILL_METEO:
	case AT_SKILL_THUNDER:
	case AT_SKILL_FIREBALL:
	case AT_SKILL_FLAME:
	case AT_SKILL_SLOW:
	case AT_SKILL_ENERGYBALL:
	case AT_SKILL_SWORD1:
	case AT_SKILL_SWORD2:
	case AT_SKILL_SWORD3:
	case AT_SKILL_SWORD4:
	case AT_SKILL_SWORD5:
	case AT_SKILL_CROSSBOW:
	case AT_SKILL_BOW:
	case AT_SKILL_WHEEL:
	case AT_SKILL_BLOWOFFURY:
	case AT_SKILL_KNIGHTSPEAR:
	case AT_SKILL_PENETRATION:
	case AT_SKILL_SWORD6:
		gObjAttack(lpObj, lpTargetObj, lpMagic, 1, 0, 0, 0, 0, 0);
		break;
	case AT_SKILL_HEALING:	this->SkillHealing(aIndex, aTargetIndex, lpMagic); break;
	case AT_SKILL_MAGICDEFENSE:	this->WizardMagicDefense(aIndex, aTargetIndex, lpMagic); break;
	case AT_SKILL_KNIGHTADDLIFE:	this->KnightSkillAddLife(aIndex, lpMagic); break;
	case AT_SKILL_DEFENSE:	this->SkillDefense(aIndex, aTargetIndex, lpMagic); break;
	case AT_SKILL_ATTACK:	this->SkillAttack(aIndex, aTargetIndex, lpMagic); break;
	case AT_SKILL_BERSERKER: this->SkillBerserker(aIndex, lpMagic, aTargetIndex); break;

	case AT_SKILL_CALLMON1:	return this->SkillMonsterCall(lpObj.m_Index, 26, lpObj.X - 1, lpObj.Y + 1);
	case AT_SKILL_CALLMON2:	return this->SkillMonsterCall(lpObj.m_Index, 32, lpObj.X - 1, lpObj.Y + 1);
	case AT_SKILL_CALLMON3:	return this->SkillMonsterCall(lpObj.m_Index, 21, lpObj.X - 1, lpObj.Y + 1);
	case AT_SKILL_CALLMON4:	return this->SkillMonsterCall(lpObj.m_Index, 20, lpObj.X - 1, lpObj.Y + 1);
	case AT_SKILL_CALLMON5:	return this->SkillMonsterCall(lpObj.m_Index, 10, lpObj.X - 1, lpObj.Y + 1);
	case AT_SKILL_CALLMON6:	return this->SkillMonsterCall(lpObj.m_Index, 150, lpObj.X - 1, lpObj.Y + 1);
	case AT_SKILL_CALLMON7:	return this->SkillMonsterCall(lpObj.m_Index, 151, lpObj.X - 1, lpObj.Y + 1);

	case AT_SKILL_EXPHELL:	return this->SkillHellFire2(lpObj.m_Index, aTargetIndex, lpMagic);
	case AT_SKILL_STRIKE:	return this->SkillKnightBlow(lpObj.m_Index, aTargetIndex, lpMagic, bCombo);

	case AT_SKILL_SPEAR:	return this->SkillSpear(lpObj.m_Index, aTargetIndex, lpMagic);
	case AT_SKILL_FIREBURST:	return this->SkillFireBurst(lpObj.m_Index, aTargetIndex, lpMagic);
	case AT_SKILL_DARKHORSE_ATTACK:	return this->SkillDarkHorseAttack(lpObj.m_Index, aTargetIndex, lpMagic);
	case AT_SKILL_RECALL_PARTY:	this->SkillRecallParty(aIndex, lpMagic->m_Level); break;
	case AT_SKILL_ADD_CRITICALDAMAGE:	this->SkillAddCriticalDamage(aIndex, lpMagic->m_Level, lpMagic); break;

	case AT_SKILL_DEFENSEDOWN:	this->MaGumSkillDefenseDown(lpObj.m_Index, aTargetIndex, 1); break;

	case AT_SKILL_SUMMON:		this->SkillSummon(lpObj.m_Index, aTargetIndex, lpMagic); break;
	case AT_SKILL_IMMUNE_TO_MAGIC:	this->SkillImmuneToMagic(lpObj.m_Index, lpMagic); break;
	case AT_SKILL_IMMUNE_TO_HARM:	this->SkillImmuneToHarm(lpObj.m_Index, lpMagic); break;
	case AT_SKILL_RUSH:	this->SkillKnightRush(lpObj.m_Index, aTargetIndex, lpMagic); break;
	case AT_SKILL_JAVALIN:	this->SkillWizardJavalin(lpObj.m_Index, aTargetIndex, lpMagic); break;
	case AT_SKILL_DEEPIMPACT:	this->SkillElfRush(lpObj.m_Index, aTargetIndex, lpMagic); break;
	case AT_SKILL_ONE_FLASH:	this->SkillMagumOneFlash(lpObj.m_Index, aTargetIndex, lpMagic); break;
	case AT_SKILL_DEATH_CANNON:	this->SkillMagumDeathCannon(lpObj.m_Index, aTargetIndex, lpMagic); break;
	case AT_SKILL_SPACE_SPLIT:	this->SkillDarklordSpaceSplit(lpObj.m_Index, aTargetIndex, lpMagic); break;

	case AT_SKILL_BRAND_OF_SKILL:	this->SkillBrandOfSkill(aIndex, aTargetIndex, lpMagic); break;

	case AT_SKILL_REMOVAL_STUN:	this->SkillRemoveStun(aIndex, aTargetIndex, lpMagic); break;
	case AT_SKILL_ADD_MANA:	this->SkillAddMana(aIndex, aTargetIndex, lpMagic); break;
	case AT_SKILL_INVISIBLE:	this->SkillCloaking(aIndex, aTargetIndex, lpMagic); break;
	case AT_SKILL_REMOVAL_INVISIBLE:	this->SkillRemoveCloaking(aIndex, aTargetIndex, lpMagic); break;
	case AT_SKILL_REMOVAL_MAGIC:	this->SkillRemoveMagic(aIndex, aTargetIndex, lpMagic); break;

	case AT_SKILL_INFINITY_ARROW:	this->SkillInfinityArrow(aIndex, aTargetIndex, lpMagic); break;

	case AT_SKILL_SLEEP: this->SkillSummonerSleep(aIndex, aTargetIndex, lpMagic); break;
	case AT_SKILL_DAMAGE_REFLECT: this->SkillDamageReflect(aIndex, lpMagic, aTargetIndex); break;
	case AT_SKILL_MAGIC_CIRCLE: this->SkillMagicCircle(aIndex, aTargetIndex, lpMagic); break;
	case AT_SKILL_SD_RECOVERY: this->SkillShieldRefill(aIndex, lpMagic, aTargetIndex); break;

	case AT_SKILL_LARGE_BLOWER:
	case AT_SKILL_UPPER_BEAST:
	case AT_SKILL_CHAIN_DRIVE:
		this->SkillMonkBarrageJustOneTarget(aIndex, lpMagic, aTargetIndex);
		break;

	case AT_SKILL_REDARMOR_IGNORE:
		this->SkillMonkBuff(aIndex, lpMagic);
		break;

	case AT_SKILL_FITNESS:
	case AT_SKILL_DSRINCREASE:
		this->SkillMonkBuffApplyParty(aIndex, lpMagic);
		break;

	case AT_SKILL_SPIN_STEP:
		this->SkillSpinStep(aIndex, aTargetIndex, lpMagic);
		break;

	case AT_SKILL_HARSH_STRIKE:
	case AT_SKILL_MAGIC_PIN:
		this->SkillGrowLancerAttackOneTarget(aIndex, aTargetIndex, lpMagic);
		break;

	case AT_SKILL_SHINING_PEAK:
		this->SkillShiningPeak(aIndex, lpMagic, aTargetIndex);
		break;

	case AT_SKILL_OBSIDIAN:
		this->SkillBuffPartyGrowLancer(aIndex, lpMagic, aTargetIndex);
		break;

	case AT_SKILL_WRATH:
	case AT_SKILL_CIRCLE_SHIELD:
		this->SkillBuffGrowLancer(aIndex, lpMagic, aTargetIndex);
		break;

	default:
		gObjAttack(lpObj, lpTargetObj, lpMagic, 1, 0, 0, 0, 0, 0);
		break;
	}

	return true;
}



void CObjUseSkill::MaGumSkillDefenseDown(CGameObject &lpObj, int aTargetIndex, int skill_level)
{
	int skillSuccess = 1;
	
	CGameObject lpTargetObj = &gGameObjects[aTargetIndex];

	if (lpObj.Type == OBJ_USER && lpObj.Class != CLASS_MAGUMSA)
	{
		return;
	}

	if (!ObjectMaxRange(aTargetIndex))
	{
		return;
	}

	if (retCalcSkillResistance(lpTargetObj, 1))
	{
		gObjAddBuffEffect(lpTargetObj, BUFFTYPE_DEFENSE_POWER_DEC, EFFECTTYPE_DECREASE_DEFENSE, 10, 0, 0, 10);
		GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_DEFENSEDOWN, lpTargetObj.m_Index, skillSuccess);
	}
}



void CObjUseSkill::KnightSkillAddLife(CGameObject &lpObj, CMagicInf * lpMagic)
{
	int skillSuccess = true;
	

	if (lpObj.Type != OBJ_USER && lpObj.m_RecallMon == -1)
	{
		return;
	}

	if ((lpObj.Type == OBJ_USER) && (lpObj.Class != CLASS_KNIGHT && lpObj.Class != CLASS_MAGUMSA && lpObj.m_PlayerData->ISBOT == false))
	{
		return;
	}

	if (lpObj.Level < 120)
	{
		MsgOutput(aIndex, Lang.GetText(0, 503));
		return;
	}

	int number;
	int partynum = 0;
	int totallevel = 0;
	int partycount;
	int dis;
	float skillbonus = 0;

	int ApplyPartyIndex[MAX_USER_IN_PARTY];

	int viewplayer = 0;
	int viewpercent = 0;

	CGameObject lpPartyObj;

	memset(ApplyPartyIndex, -1, sizeof(ApplyPartyIndex));

	partynum = lpObj.PartyNumber;

	if (partynum != -1)
	{
		partycount = gParty.m_PartyS[partynum].Count;

		for (int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			number = gParty.m_PartyS[partynum].Number[n];

			if (number >= 0)
			{
				lpPartyObj = &gGameObjects[number];

				if (lpObj.MapNumber == lpPartyObj.MapNumber)
				{
					dis = gObjCalDistance(lpObj, &gGameObjects[number]);

					if (dis < 10)
					{
						ApplyPartyIndex[n] = lpPartyObj.m_Index;
						viewplayer++;
					}
				}
			}
		}
	}

	if (viewplayer > 1)
	{
		if (viewplayer == 2)
		{
			viewpercent = 1;
		}
		else if (viewplayer == 3)
		{
			viewpercent = 2;
		}
		else if (viewplayer == 4)
		{
			viewpercent = 3;
		}
		else
		{
			viewpercent = 4;
		}
	}

	double iaddLifepower = 0.0;
	int iLifeTime = 0;
	int iaddlife = 0;

	this->m_Lua.Generic_Call("KnightSkillAddLife", "iii>di", (lpObj.m_PlayerData->Vitality + lpObj.AddVitality), (lpObj.m_PlayerData->Energy + lpObj.AddEnergy), viewpercent, &iaddLifepower, &iLifeTime);

	if (lpObj.Type == OBJ_USER && partynum == -1 && lpObj.m_PlayerData->ISBOT == false)
	{
		iaddlife = int(lpObj.MaxLife) * iaddLifepower / 100;

		if (iaddlife > 3000000)
		{
			iaddlife = 3000000;
		}

		gObjAddBuffEffect(lpObj, BUFFTYPE_HP_INC, EFFECTTYPE_HP, iaddlife, 0, 0, iLifeTime);
		GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, lpObj.m_Index, skillSuccess);
		gObjCalCharacter.CalcCharacter(lpObj.m_Index);
	}

	else if (lpObj.Type == OBJ_USER && lpObj.m_PlayerData->ISBOT == true)
	{
		iaddlife = (gGameObjects[lpObj.BuffPlayerIndex]->MaxLife) * (iaddLifepower + 100) / 100;

		if (iaddlife > 3000000)
		{
			iaddlife = 3000000;
		}

		iLifeTime = g_BotSystem.GetSkillTime(lpObj.m_Index, lpMagic->m_Skill);

		gObjAddBuffEffect(&gGameObjects[lpObj.BuffPlayerIndex], BUFFTYPE_HP_INC, EFFECTTYPE_HP, iaddlife, 0, 0, iLifeTime);
		GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, lpObj.m_Index, skillSuccess);
		gObjCalCharacter.CalcCharacter(lpObj.BuffPlayerIndex);
	}
	else
	{
		for (int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			if (ApplyPartyIndex[n] != -1)
			{
				lpPartyObj = &gGameObjects[ApplyPartyIndex[n]];

				iaddlife = lpPartyObj.MaxLife * iaddLifepower / 100;

				gObjAddBuffEffect(lpPartyObj, BUFFTYPE_HP_INC, EFFECTTYPE_HP, iaddlife, 0, 0, iLifeTime);
				GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, lpPartyObj.m_Index, skillSuccess);
				gObjCalCharacter.CalcCharacter(ApplyPartyIndex[n]);
			}
		}
	}
}


void CObjUseSkill::WizardMagicDefense(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic)
{
	int skillSuccess = true;
	double skillInc = 0.0;
	int skillTime = 0;
	
	CGameObject lpTargetObj = &gGameObjects[aTargetIndex];

	if (lpObj.Type != OBJ_USER && lpObj.m_RecallMon == -1)
	{
		return;
	}

	if (lpObj.Class != CLASS_WIZARD && lpObj.Class != CLASS_MAGUMSA && lpObj.m_PlayerData->ISBOT == false)
	{
		return;
	}

	if (lpObj.PartyNumber != lpTargetObj.PartyNumber && lpObj.m_PlayerData->ISBOT == false)
	{
		return;
	}

	this->m_Lua.Generic_Call("WizardMagicDefense", "iiii>di", aIndex, aTargetIndex, (lpObj.m_PlayerData->Dexterity + lpObj.AddDexterity), (lpObj.m_PlayerData->Energy + lpObj.AddEnergy), &skillInc, &skillTime);

	if (gObjCheckUsedBuffEffect(lpTargetObj, BUFFTYPE_MELEE_DEFENSE_INC) == true)
	{
		gObjRemoveBuffEffect(lpTargetObj, BUFFTYPE_MELEE_DEFENSE_INC);
	}

	if (gObjCheckUsedBuffEffect(lpTargetObj, BUFFTYPE_MELEE_DEFENSE_INC_STR) == true)
	{
		gObjRemoveBuffEffect(lpTargetObj, BUFFTYPE_MELEE_DEFENSE_INC_STR);
	}

	gObjAddBuffEffect(lpTargetObj, BUFFTYPE_MELEE_DEFENSE_INC, EFFECTTYPE_IMPROVE_MELEE_DEFENSE, skillInc, EFFECTTYPE_MELEE_DEFENSE_DOWN_MANA, 20, skillTime);

	GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, lpTargetObj.m_Index, skillSuccess);
}

void CObjUseSkill::SkillDefense(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic)
{
	int skillSuccess = true;
	
	CGameObject lpTargetObj = &gGameObjects[aTargetIndex];

	if (lpTargetObj.Type != OBJ_USER && lpTargetObj.m_RecallMon == -1)
	{
		return;
	}

	if (lpObj.Class != CLASS_ELF && lpObj.m_PlayerData->ISBOT == false)
	{
		return;
	}

	if ((CC_MAP_RANGE(lpObj.MapNumber) || lpObj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL) && lpObj.m_Index != lpTargetObj.m_Index)
	{
		return;
	}

	double skill_defense = 0.0;
	double skill_time = 0.0;

	this->m_Lua.Generic_Call("ElfDefense", "iiii>dd", lpTargetObj.Class, aIndex, aTargetIndex, (lpObj.m_PlayerData->Energy + lpObj.AddEnergy), &skill_defense, &skill_time);

	if (lpTargetObj.Class == CLASS_KNIGHT)
	{
		skill_defense += skill_defense / 10.0f;
	}

	if (lpObj.m_PlayerData->ISBOT == true)
	{
		skill_time = g_BotSystem.GetSkillTime(aIndex, lpMagic->m_Skill);
	}

	gObjAddBuffEffect(lpTargetObj, BUFFTYPE_DEFENSE_POWER_INC, EFFECTTYPE_IMPROVE_DEFENSE, skill_defense, 0, 0, skill_time);

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj.VpPlayer2[n].type == OBJ_MONSTER && lpObj.VpPlayer2[n].state != 0)
		{
			CGameObject lpMonster = &gGameObjects[lpObj.VpPlayer2[n]->number];
			if (lpMonster->m_iCurrentAI != 0)
			{
				lpMonster->m_Agro->IncAgro(lpObj.m_Index, int(skill_defense) / 10);
			}
		}
	}

	GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_DEFENSE, lpTargetObj.m_Index, skillSuccess);
}

void CObjUseSkill::SkillAttack(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic)
{
	int skillSuccess = true;
	
	CGameObject lpTargetObj = &gGameObjects[aTargetIndex];

	if (lpTargetObj.Type != OBJ_USER && lpTargetObj.m_RecallMon == -1)
	{
		return;
	}

	if (lpObj.Class != CLASS_ELF && lpObj.m_PlayerData->ISBOT == false)
	{
		return;
	}

	if ((CC_MAP_RANGE(lpObj.MapNumber) || lpObj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL) && lpObj.m_Index != lpTargetObj.m_Index)
	{
		return;
	}

	double skill_attack = 0.0;
	double skill_time = 0.0;

	this->m_Lua.Generic_Call("ElfAttack", "iiii>dd", lpTargetObj.Class, aIndex, aTargetIndex, (lpObj.m_PlayerData->Energy + lpObj.AddEnergy), &skill_attack, &skill_time);

	if (lpTargetObj.Class == CLASS_KNIGHT)
	{
		skill_attack += skill_attack / 10.0f;
	}

	if (lpObj.m_PlayerData->ISBOT == true)
	{
		skill_time = g_BotSystem.GetSkillTime(aIndex, lpMagic->m_Skill);
	}

	if (g_ConfigRead.UseAllBuffAttack == 0)
	{
		if (gObjCheckPowerfulEffect(lpTargetObj, BUFFTYPE_ATTACK_POWER_INC, skill_attack, 0) == TRUE)
		{
			GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_ATTACK, lpTargetObj.m_Index, FALSE);
			return;
		}
		else if (gObjCheckUsedBuffEffect(lpTargetObj, BUFFTYPE_BERSERKER) == TRUE)
		{
			GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_ATTACK, lpTargetObj.m_Index, FALSE);
			return;
		}
		else if (gObjCheckUsedBuffEffect(lpTargetObj, BUFFTYPE_BERSERKER_STR) == TRUE)
		{
			GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_ATTACK, lpTargetObj.m_Index, FALSE);
			return;
		}
		else if (gObjCheckUsedBuffEffect(lpTargetObj, BUFFTYPE_BERSERKER_MAS) == TRUE)
		{
			GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_ATTACK, lpTargetObj.m_Index, FALSE);
			return;
		}
		else if (gObjCheckUsedBuffEffect(lpTargetObj, BUFFTYPE_BERSERKER_PRO) == TRUE)
		{
			GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_ATTACK, lpTargetObj.m_Index, FALSE);
			return;
		}
	}

	gObjAddBuffEffect(lpTargetObj, BUFFTYPE_ATTACK_POWER_INC, EFFECTTYPE_IMPROVE_DAMAGE, skill_attack, 0, 0, skill_time);

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj.VpPlayer2[n].type == OBJ_MONSTER && lpObj.VpPlayer2[n].state != 0)
		{
			CGameObject lpMonster = &gGameObjects[lpObj.VpPlayer2[n]->number];
			if (lpMonster->m_iCurrentAI != 0)
			{
				lpMonster->m_Agro->IncAgro(lpObj.m_Index, int(skill_attack) / 10);
			}
		}
	}

	GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_ATTACK, lpTargetObj.m_Index, skillSuccess);
}



BOOL CObjUseSkill::SkillChangeUse(CGameObject &lpObj)
{
	

	int skill_level = -1;

	if (lpObj.Type != OBJ_USER)
	{
		return false;
	}

	if (lpObj.pInventory[10].IsItem() == 1 && lpObj.pInventory[10].m_Type == ITEMGET(13, 10))
	{
		if (lpObj.pInventory[10].m_Durability < 1.0f)
		{
			skill_level = -1;
		}
		else
		{
			skill_level = lpObj.pInventory[10].m_Level;
		}
	}

	if (lpObj.pInventory[11].IsItem() == 1 && skill_level == -1 && lpObj.pInventory[11].m_Type == ITEMGET(13, 10))
	{
		if (lpObj.pInventory[11].m_Durability < 1.0f)
		{
			skill_level = -1;
		}
		else
		{
			skill_level = lpObj.pInventory[11].m_Level;
		}
	}

	if (skill_level > 7)
	{
		return false;
	}

	if (lpObj.pInventory[10].IsItem() == 1 && lpObj.pInventory[10].m_Type == ITEMGET(13, 39))
	{
		if (lpObj.pInventory[10].m_Durability < 1.0f)
		{
			skill_level = -1;
		}
		else
		{
			skill_level = 372;
		}
	}

	if (lpObj.pInventory[11].IsItem() == 1 && skill_level == -1 && lpObj.pInventory[11].m_Type == ITEMGET(13, 39))
	{
		if (lpObj.pInventory[11].m_Durability < 1.0f)
		{
			skill_level = -1;
		}
		else
		{
			skill_level = 372;
		}
	}

	if (lpObj.pInventory[10].IsItem() == 1 && lpObj.pInventory[10].m_Type == ITEMGET(13, 40))
	{
		if (lpObj.pInventory[10].m_Durability < 1.0f)
		{
			skill_level = -1;
		}
		else
		{
			skill_level = 373;
		}
	}

	if (lpObj.pInventory[11].IsItem() == 1 && skill_level == -1 && lpObj.pInventory[11].m_Type == ITEMGET(13, 40))
	{
		if (lpObj.pInventory[11].m_Durability < 1.0f)
		{
			skill_level = -1;
		}
		else
		{
			skill_level = 373;
		}
	}

	if (lpObj.pInventory[10].IsItem() == 1 && lpObj.pInventory[10].m_Type == ITEMGET(13, 41))
	{
		if (lpObj.pInventory[10].m_Durability < 1.0f)
		{
			skill_level = -1;
		}
		else
		{
			skill_level = 374;
		}
	}

	if (lpObj.pInventory[11].IsItem() == 1 && skill_level == -1 && lpObj.pInventory[11].m_Type == ITEMGET(13, 41))
	{
		if (lpObj.pInventory[11].m_Durability < 1.0f)
		{
			skill_level = -1;
		}
		else
		{
			skill_level = 374;
		}
	}

	if (lpObj.pInventory[10].IsItem() == 1 && lpObj.pInventory[10].m_Type == ITEMGET(13, 42))
	{
		if (lpObj.pInventory[10].m_Durability < 1.0f)
		{
			skill_level = -1;
		}
		else
		{
			skill_level = 378;
		}
	}

	if (lpObj.pInventory[11].IsItem() == 1 && skill_level == -1 && lpObj.pInventory[11].m_Type == ITEMGET(13, 42))
	{
		if (lpObj.pInventory[11].m_Durability < 1.0f)
		{
			skill_level = -1;
		}
		else
		{
			skill_level = 378;
		}
	}

	if (lpObj.pInventory[10].IsItem() == 1 && lpObj.pInventory[10].m_Type == ITEMGET(13, 68))
	{
		if (lpObj.pInventory[10].m_Durability < 1.0f)
		{
			skill_level = -1;
		}
		else
		{
			skill_level = 477;
		}
	}

	if (lpObj.pInventory[11].IsItem() == 1 && lpObj.pInventory[11].m_Type == ITEMGET(13, 68))
	{
		if (lpObj.pInventory[11].m_Durability < 1.0f)
		{
			skill_level = -1;
		}
		else
		{
			skill_level = 477;
		}
	}

	if (lpObj.pInventory[10].IsItem() == 1 && lpObj.pInventory[10].m_Type == ITEMGET(13, 76))
	{
		if (lpObj.pInventory[10].m_Durability < 1.0f)
		{
			skill_level = -1;
		}
		else
		{
			skill_level = 503;
		}
	}

	if (lpObj.pInventory[11].IsItem() == 1 && lpObj.pInventory[11].m_Type == ITEMGET(13, 76))
	{
		if (lpObj.pInventory[11].m_Durability < 1.0f)
		{
			skill_level = -1;
		}
		else
		{
			skill_level = 503;
		}
	}

	if (lpObj.pInventory[10].IsItem() == 1 && lpObj.pInventory[10].m_Type == ITEMGET(13, 77))
	{
		if (lpObj.pInventory[10].m_Durability < 1.0f)
		{
			skill_level = -1;
		}
		else
		{
			skill_level = 616;
		}
	}

	if (lpObj.pInventory[11].IsItem() == 1 && lpObj.pInventory[11].m_Type == ITEMGET(13, 77))
	{
		if (lpObj.pInventory[11].m_Durability < 1.0f)
		{
			skill_level = -1;
		}
		else
		{
			skill_level = 616;
		}
	}

	if (lpObj.pInventory[10].IsItem() == 1 && lpObj.pInventory[10].m_Type == ITEMGET(13, 78))
	{
		if (lpObj.pInventory[10].m_Durability < 1.0f)
		{
			skill_level = -1;
		}
		else
		{
			skill_level = 617;
		}
	}

	if (lpObj.pInventory[11].IsItem() == 1 && lpObj.pInventory[11].m_Type == ITEMGET(13, 78))
	{
		if (lpObj.pInventory[11].m_Durability < 1.0f)
		{
			skill_level = -1;
		}
		else
		{
			skill_level = 617;
		}
	}

	if (lpObj.pInventory[10].IsItem() == 1 && lpObj.pInventory[10].m_Type == ITEMGET(13, 122))
	{
		if (lpObj.pInventory[10].m_Durability < 1.0f)
		{
			skill_level = -1;
		}
		else
		{
			skill_level = 548;
		}
	}

	if (lpObj.pInventory[11].IsItem() == 1 && lpObj.pInventory[11].m_Type == ITEMGET(13, 122))
	{
		if (lpObj.pInventory[11].m_Durability < 1.0f)
		{
			skill_level = -1;
		}
		else
		{
			skill_level = 548;
		}
	}
	if (lpObj.pInventory[10].IsItem() == 1 && lpObj.pInventory[10].m_Type == ITEMGET(13, 163))
	{
		if (lpObj.pInventory[10].m_Durability < 1.0f)
		{
			skill_level = -1;
		}
		else
		{
			skill_level = 625;
		}
	}

	if (lpObj.pInventory[11].IsItem() == 1 && lpObj.pInventory[11].m_Type == ITEMGET(13, 163))
	{
		if (lpObj.pInventory[11].m_Durability < 1.0f)
		{
			skill_level = -1;
		}
		else
		{
			skill_level = 625;
		}
	}
	if (lpObj.pInventory[10].IsItem() == 1 && lpObj.pInventory[10].m_Type == ITEMGET(13, 164))
	{
		if (lpObj.pInventory[10].m_Durability < 1.0f)
		{
			skill_level = -1;
		}
		else
		{
			skill_level = 626;
		}
	}

	if (lpObj.pInventory[11].IsItem() == 1 && lpObj.pInventory[11].m_Type == ITEMGET(13, 164))
	{
		if (lpObj.pInventory[11].m_Durability < 1.0f)
		{
			skill_level = -1;
		}
		else
		{
			skill_level = 626;
		}
	}

	if (lpObj.pInventory[10].IsItem() == 1 && lpObj.pInventory[10].m_Type == ITEMGET(13, 165))
	{
		if (lpObj.pInventory[10].m_Durability < 1.0f)
		{
			skill_level = -1;
		}
		else
		{
			skill_level = 642;
		}
	}

	if (lpObj.pInventory[11].IsItem() == 1 && lpObj.pInventory[11].m_Type == ITEMGET(13, 165))
	{
		if (lpObj.pInventory[11].m_Durability < 1.0f)
		{
			skill_level = -1;
		}
		else
		{
			skill_level = 642;
		}
	}

	if (lpObj.pInventory[11].IsItem() == 1 && lpObj.pInventory[11].m_Type == ITEMGET(13, 166))
	{
		if (lpObj.pInventory[11].m_Durability < 1.0f)
		{
			skill_level = -1;
		}
		else
		{
			skill_level = 646;
		}
	}

	if (lpObj.pInventory[10].IsItem() == 1 && lpObj.pInventory[10].m_Type == ITEMGET(13, 268))
	{
		if (lpObj.pInventory[10].m_Durability < 1.0f)
		{
			skill_level = -1;
		}
		else
		{
			skill_level = 706;
		}
	}

	if (lpObj.pInventory[11].IsItem() == 1 && lpObj.pInventory[11].m_Type == ITEMGET(13, 268))
	{
		if (lpObj.pInventory[11].m_Durability < 1.0f)
		{
			skill_level = -1;
		}
		else
		{
			skill_level = 706;
		}
	}

	if (IT_MAP_RANGE(lpObj.MapNumber))
	{
		if (g_IT_Event.GetIllusionTempleState(lpObj.MapNumber))
		{
			BYTE team = g_IT_Event.GetUserTeam(lpObj.MapNumber, lpObj.m_Index);

			if (team == 0)
			{
				skill_level = 404;
			}

			else if (team == 1)
			{
				skill_level = 405;
			}
		}
	}

	int change = -1;

	switch (skill_level)
	{
	case 0:		change = 2;				break;
	case 1:		change = 7;				break;
	case 2:		change = 14;			break;
	case 3:		change = 8;				break;
	case 4:		change = 9;				break;
	case 5:		change = 41;			break;
	case 372:	change = skill_level;	break;
	case 373:	change = skill_level;	break;
	case 374:	change = skill_level;	break;
	case 378:	change = skill_level;	break;
	case 404:
	case 405:	change = skill_level;	break;
	case 477:	change = skill_level;	break;
	case 503:	change = skill_level;	break;
	case 548:	change = skill_level;	break;
	case 616:	change = skill_level;	break;
	case 617:	change = skill_level;	break;
	case 642:	change = skill_level;	break;
	case 626:	change = skill_level;	break;
	case 625:	change = skill_level;	break;
	case 646:	change = skill_level;	break;
	default:	change = -1;			break;
	}

	if (lpObj.m_Change >= 0 && lpObj.m_Change == change)
	{
		return false;
	}

	if (lpObj.m_Change == 41 && change != 41)
	{
		lpObj.AddLife = 0;

		GSProtocol.GCReFillSend(lpObj.m_Index, lpObj.Life, 0xFF, 0, lpObj.iShield);
		GSProtocol.GCReFillSend(lpObj.m_Index, lpObj.AddLife + lpObj.MaxLife, 0xFE, 0, lpObj.iAddShield + lpObj.iMaxShield);
	}

	if (lpObj.m_Change == 372 && change != 372)
	{
		lpObj.AddLife = 0;
		gObjCalCharacter.CalcCharacter(lpObj.m_Index);

		GSProtocol.GCReFillSend(lpObj.m_Index, lpObj.Life, 0xFF, 0, lpObj.iShield);
		GSProtocol.GCReFillSend(lpObj.m_Index, lpObj.AddLife + lpObj.MaxLife, 0xFE, 0, lpObj.iAddShield + lpObj.iMaxShield);
	}

	if (lpObj.m_Change == 503 || lpObj.m_Change == 548 || lpObj.m_Change == 616 || lpObj.m_Change == 617)
	{
		gObjCalCharacter.CalcCharacter(lpObj.m_Index);
	}

	lpObj.m_Change = change;
	gObjViewportListProtocolCreate(lpObj);

	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		g_CastleSiege.NotifySelfCsJoinSide(lpObj.m_Index);
	}

	PMSG_KILLCOUNT pMsg = { 0 };

	PHeadSubSetB((LPBYTE)&pMsg, 0xB8, 0x01, sizeof(pMsg));

	pMsg.btKillCount = lpObj.m_btKillCount;

	IOCP.DataSend(lpObj.m_Index, (PBYTE)&pMsg, sizeof(pMsg));
	return true;
}



void CObjUseSkill::SkillHealing(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic)
{
	int skillSuccess = true;
	int skillbonus = 0;
	
	CGameObject lpTargetObj = &gGameObjects[aTargetIndex];

	if (lpTargetObj.Type != OBJ_USER && lpTargetObj.m_RecallMon == -1)
	{
		return;
	}

	if (lpObj.Class != CLASS_ELF && lpObj.m_PlayerData->ISBOT == false)
	{
		return;
	}

	if ((CC_MAP_RANGE(lpObj.MapNumber) || lpObj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL) && lpObj.m_Index != lpTargetObj.m_Index)
	{
		return;
	}

	int addlife = 0;
	this->m_Lua.Generic_Call("ElfHeal", "iiii>i", lpTargetObj.Class, lpObj.m_PlayerData->Energy + lpObj.AddEnergy, aIndex, aTargetIndex, &addlife);

	lpTargetObj.Life += addlife;
	if ((lpTargetObj.MaxLife + lpTargetObj.AddLife) < lpTargetObj.Life)
	{
		lpTargetObj.Life = lpTargetObj.MaxLife + lpTargetObj.AddLife;
	}

	if (lpTargetObj.Type == OBJ_USER)
	{
		GSProtocol.GCReFillSend(lpTargetObj.m_Index, lpTargetObj.Life, 0xFF, 0, lpTargetObj.iShield);
		GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_HEALING, lpTargetObj.m_Index, skillSuccess);

		for (int n = 0; n < MAX_VIEWPORT; n++)
		{
			if (lpObj.VpPlayer2[n].type == OBJ_MONSTER && lpObj.VpPlayer2[n].state != 0)
			{
				CGameObject lpMonster = &gGameObjects[lpObj.VpPlayer2[n]->number];
				if (lpMonster->m_iCurrentAI != 0)
				{
					lpMonster->m_Agro->IncAgro(lpObj.m_Index, (lpObj.m_PlayerData->Energy / 5) / 40);
				}
			}
		}
	}
	else if (lpTargetObj.m_RecallMon >= 0)
	{
		GSProtocol.GCRecallMonLife(lpTargetObj.m_RecallMon, lpTargetObj.MaxLife, lpTargetObj.Life);
	}
}

int CObjUseSkill::SkillMonsterCall(CGameObject &lpObj, int MonsterType, int x, int y)
{
	if (lpObj.Type != OBJ_USER)
	{
		return false;
	}

	if (lpObj.MapNumber == MAP_INDEX_ICARUS)
	{
		return false;
	}

	if (CC_MAP_RANGE(lpObj.MapNumber) || lpObj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		return false;
	}

	if (lpObj.m_RecallMon >= 0)
	{
		GSProtocol.GCRecallMonLife(aIndex, 60, 0);
		gObjMonsterCallKill(aIndex);
		return false;
	}

	int result = gObjAddCallMon();

	if (result >= 0)
	{
		gGameObjects[result]->X = x;
		gGameObjects[result]->Y = y;
		gGameObjects[result]->MTX = x;
		gGameObjects[result]->MTY = y;
		gGameObjects[result]->Dir = 2;
		gGameObjects[result]->MapNumber = lpObj.MapNumber;
		gObjSetMonster(result, MonsterType);
		gGameObjects[result]->m_RecallMon = aIndex;
		gGameObjects[result]->m_Attribute = 100;
		gGameObjects[result]->TargetNumber = (WORD)-1;
		gGameObjects[result]->m_ActState.Emotion = 0;
		gGameObjects[result]->m_ActState.Attack = 0;
		gGameObjects[result]->m_ActState.EmotionCount = 0;
		gGameObjects[result]->PathCount = 0;

		lpObj.m_RecallMon = result;
		gGameObjects[result]->m_MoveRange = 15;

		if (lpObj.m_PlayerData->m_MPSkillOpt.iMpsCallMonHp > 0.0)
		{
			gGameObjects[result]->Life += gGameObjects[result]->Life * lpObj.m_PlayerData->m_MPSkillOpt.iMpsCallMonHp / 100.0;
			gGameObjects[result]->MaxLife += gGameObjects[result]->MaxLife * lpObj.m_PlayerData->m_MPSkillOpt.iMpsCallMonHp / 100.0;
		}

		if (lpObj.m_PlayerData->m_MPSkillOpt.iMpsCallMonDefense > 0.0)
		{
			gGameObjects[result]->m_Defense += gGameObjects[result]->m_Defense * lpObj.m_PlayerData->m_MPSkillOpt.iMpsCallMonDefense / 100.0;
		}

		if (lpObj.m_PlayerData->m_MPSkillOpt.iMpsCallMonAttack > 0.0)
		{
			gGameObjects[result]->m_AttackDamageMin += gGameObjects[result]->m_AttackDamageMin * lpObj.m_PlayerData->m_MPSkillOpt.iMpsCallMonAttack / 100.0;
			gGameObjects[result]->m_AttackDamageMax += gGameObjects[result]->m_AttackDamageMax * lpObj.m_PlayerData->m_MPSkillOpt.iMpsCallMonAttack / 100.0;
		}

		GSProtocol.GCRecallMonLife(gGameObjects[result]->m_RecallMon, gGameObjects[result]->MaxLife, gGameObjects[result]->Life);
		return true;
	}
	return false;
}



void CObjUseSkill::SkillBlowOfFury(CGameObject &lpObj, CMagicInf * lpMagic, BYTE x, BYTE y, BYTE dir, BYTE TargetPos, int aTargetIndex, BOOL isCombo)
{
	
	int tObjNum;
	int count = 0;
	int DuelIndex = lpObj.m_iDuelUser;
	int HitCount = 0;
	int bAttack = 0;
	int EnableAttack;

	while (true)
	{
		if (lpObj.VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = 0;

				if (lpObj.VpPlayer2[count].type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && lpObj.VpPlayer2[count].type != OBJ_NPC && gGameObjects[tObjNum]->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = 1;
				}

				else
				{
					int CallMonIndex = gGameObjects[tObjNum]->m_Index;

					if (gGameObjects[tObjNum]->Type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon >= 0)
					{
						CallMonIndex = gGameObjects[gGameObjects[tObjNum]->m_RecallMon]->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &gGameObjects[CallMonIndex]) == 1)
					{
						EnableAttack = 1;
					}

					if (gGameObjects[tObjNum]->Class >= 678 && gGameObjects[tObjNum]->Class <= 680)
					{
						EnableAttack = 1;
					}
				}

				if (EnableAttack != 0)
				{
					if (gObjCalDistance(lpObj, &gGameObjects[tObjNum]) <= MagicDamageC.GetSkillDistance(lpMagic->m_Skill))
					{
						bAttack = 0;

						if (HitCount > 15)
						{
							bAttack = 0;
						}

						if (HitCount >= 10)
						{
							if (rand() % 2)
							{
								bAttack = 1;
							}
						}
						else
						{
							bAttack = 1;
						}

						if (bAttack != 0)
						{
							gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, 500, lpMagic->m_Skill, isCombo);
						}
					}
				}
			}
		}

		count++;
		if (count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}
}



void CObjUseSkill::SkillWheel(CGameObject &lpObj, CMagicInf* lpMagic, int aTargetIndex, BOOL isCombo)
{
	
	int tObjNum;
	int count = 0;
	int HitCount = 0;
	int bAttack;
	int DuelIndex = lpObj.m_iDuelUser;
	int EnableAttack;

	while (true)
	{
		if (lpObj.VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = 0;

				if (lpObj.VpPlayer2[count].type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon < 0)
				{
					EnableAttack = 1;
				}

				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && lpObj.VpPlayer2[count].type != OBJ_NPC && gGameObjects[tObjNum]->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = 1;
				}

				else
				{
					int CallMonIndex = gGameObjects[tObjNum]->m_Index;

					if (gGameObjects[tObjNum]->Type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon >= 0)
					{
						CallMonIndex = gGameObjects[gGameObjects[tObjNum]->m_RecallMon]->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &gGameObjects[CallMonIndex]) == 1)
					{
						EnableAttack = 1;
					}

					if (gGameObjects[tObjNum]->Class >= 678 && gGameObjects[tObjNum]->Class <= 680)
					{
						EnableAttack = 1;
					}
				}

				if (EnableAttack != 0)
				{
					if (gObjCalDistance(lpObj, &gGameObjects[tObjNum]) <= MagicDamageC.GetSkillDistance(lpMagic->m_Skill))
					{
						bAttack = 0;

						if (HitCount > 15)
						{
							bAttack = 0;
						}

						if (HitCount >= 10)
						{
							if (rand() % 2)
							{
								bAttack = 1;
							}
						}
						else
						{
							bAttack = 1;
						}

						if (bAttack != 0)
						{
							gObjAttack(lpObj, &gGameObjects[tObjNum], lpMagic, 0, 1, 0, isCombo, 0, 0);
						}
					}
				}
			}
		}

		count++;
		if (count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}
}



void CObjUseSkill::SkillPowerSlash(CGameObject &lpObj, CMagicInf * lpMagic, BYTE x, BYTE y, BYTE dir, BYTE targetangle, int aTargetIndex)
{
	
	int StartDis = 1;
	int tObjNum;
	::SkillFrustrum(targetangle, aIndex);
	int count = 0;
	int HitCount = 0;
	int bAttack;
	int DuelIndex = lpObj.m_iDuelUser;
	int EnableAttack;

	while (true)
	{
		if (lpObj.VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = 0;

				if (lpObj.VpPlayer2[count].type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon < 0)
				{
					EnableAttack = 1;
				}

				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && lpObj.VpPlayer2[count].type != OBJ_NPC && gGameObjects[tObjNum]->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = 1;
				}

				else
				{
					int CallMonIndex = gGameObjects[tObjNum]->m_Index;

					if (gGameObjects[tObjNum]->Type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon >= 0)
					{
						CallMonIndex = gGameObjects[gGameObjects[tObjNum]->m_RecallMon]->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &gGameObjects[CallMonIndex]) == 1)
					{
						EnableAttack = 1;
					}

					if (gGameObjects[tObjNum]->Class >= 678 && gGameObjects[tObjNum]->Class <= 680)
					{
						EnableAttack = 1;
					}
				}

				if (EnableAttack != 0)
				{
					if (SkillTestFrustrum(gGameObjects[tObjNum]->X, gGameObjects[tObjNum]->Y, aIndex))
					{
						bAttack = 0;

						if (HitCount > 10)
						{
							bAttack = 0;
						}

						if (HitCount >= 5)
						{
							if (rand() % 2)
							{
								bAttack = 1;
							}
						}
						else
						{
							bAttack = 1;
						}

						if (bAttack != 0)
						{
							gObjAttack(lpObj, &gGameObjects[tObjNum], lpMagic, 1, 1, 0, 0, 0, 0);
						}

						HitCount++;
					}
				}
			}
		}

		count++;
		if (count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}
}



int CObjUseSkill::SkillDeathPoison(CGameObject &lpObj, CMagicInf * lpMagic, BYTE x, BYTE y, BYTE dir, BYTE TargetPos, int aTargetIndex)
{
	
	int tObjNum;
	int count = 0;
	int FirstHit = 0;
	int HitCount = 0;
	int bAttack;
	int DuelIndex = lpObj.m_iDuelUser;
	int EnableAttack;
	CGameObject lpTargetObj = &gGameObjects[aTargetIndex];

	while (true)
	{
		if (lpObj.VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = 0;

				if (lpObj.VpPlayer2[count].type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon < 0)
				{
					EnableAttack = 1;
				}

				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && lpObj.VpPlayer2[count].type != OBJ_NPC && gGameObjects[tObjNum]->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = 1;
				}

				else
				{
					int CallMonIndex = gGameObjects[tObjNum]->m_Index;

					if (gGameObjects[tObjNum]->Type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon >= 0)
					{
						CallMonIndex = gGameObjects[gGameObjects[tObjNum]->m_RecallMon]->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &gGameObjects[CallMonIndex]) == 1)
					{
						EnableAttack = 1;
					}

					if (gGameObjects[tObjNum]->Class >= 678 && gGameObjects[tObjNum]->Class <= 680)
					{
						EnableAttack = 1;
					}
				}
				if (EnableAttack != 0)
				{
					if (this->CalDistance(x, y, gGameObjects[tObjNum]->X, gGameObjects[tObjNum]->Y) < 4)
					{
						bAttack = 0;

						if (HitCount > 10)
						{
							bAttack = 0;
						}

						if (HitCount >= 5)
						{
							if (rand() % 2)
							{
								bAttack = 1;
							}
						}
						else
						{
							bAttack = 1;
						}

						if (bAttack != 0)
						{
							int delay = rand() % 500;
							gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, delay, lpMagic->m_Skill, 0);
						}
					}
				}
			}
		}

		count++;
		if (count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}
	return true;
}

int CObjUseSkill::SkillDeathPoisonForMedusa(CGameObject &lpObj, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex)
{
	
	int tObjNum;
	int count = 0;
	int EnableAttack;
	CGameObject lpTargetObj = &gGameObjects[aTargetIndex];

	while (true)
	{
		if (lpObj.VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = 0;

				if (lpObj.VpPlayer2[count].type == OBJ_USER && gGameObjects[tObjNum]->m_RecallMon < 0)
				{
					EnableAttack = 1;
				}

				if (gGameObjects[tObjNum]->Class >= 678 && gGameObjects[tObjNum]->Class <= 680)
				{
					EnableAttack = TRUE;
				}

				if (EnableAttack == TRUE)
				{
					if (this->CalDistance(x, y, gGameObjects[tObjNum]->X, gGameObjects[tObjNum]->Y) < 8)
					{
						if (aIndex != tObjNum)
						{
							if (gObjCalDistance(lpTargetObj, &gGameObjects[tObjNum]) <= 3)
							{
								int delay = rand() % 500;

								gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, delay, lpMagic->m_Skill, 0);

								if (gObjCheckUsedBuffEffect(&gGameObjects[tObjNum], BUFFTYPE_POISON) == FALSE)
								{
									if (!retResistance(&gGameObjects[tObjNum], 1))
									{
										gGameObjects[tObjNum]->lpAttackObj = lpObj;
										gObjAddBuffEffect(&gGameObjects[tObjNum], BUFFTYPE_POISON, EFFECTTYPE_POISON_DMG_TICK, 3, 0, 0, 20);
									}
								}
							}
						}
					}
				}
			}
		}

		count++;
		if (count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}
	return true;
}

int CObjUseSkill::SkillSuddenIce(CGameObject &lpObj, CMagicInf * lpMagic, BYTE x, BYTE y, BYTE dir, BYTE TargetPos, int aTargetIndex)
{
	
	int tObjNum;
	int count = 0;
	int FirstHit = 0;
	int HitCount = 0;
	int bAttack;
	CGameObject lpTargetObj = &gGameObjects[aTargetIndex];
	int DuelIndex = lpObj.m_iDuelUser;
	int EnableAttack;

	while (true)
	{
		if (lpObj.VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = 0;

				if (lpObj.VpPlayer2[count].type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon < 0)
				{
					EnableAttack = 1;
				}

				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && lpObj.VpPlayer2[count].type != OBJ_NPC && gGameObjects[tObjNum]->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = 1;
				}

				else
				{
					int CallMonIndex = gGameObjects[tObjNum]->m_Index;

					if (gGameObjects[tObjNum]->Type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon >= 0)
					{
						CallMonIndex = gGameObjects[gGameObjects[tObjNum]->m_RecallMon]->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &gGameObjects[CallMonIndex]) == 1)
					{
						EnableAttack = 1;
					}

					if (gGameObjects[tObjNum]->Class >= 678 && gGameObjects[tObjNum]->Class <= 680)
					{
						EnableAttack = 1;
					}
				}

				if (EnableAttack != 0)
				{
					if (this->CalDistance(x, y, gGameObjects[tObjNum]->X, gGameObjects[tObjNum]->Y) <= MagicDamageC.GetSkillDistance(lpMagic->m_Skill))
					{
						bAttack = 0;

						if (HitCount > 10)
						{
							bAttack = 0;
						}

						if (HitCount >= 5)
						{
							if (rand() % 2)
							{
								bAttack = 1;
							}
						}
						else
						{
							bAttack = 1;
						}

						if (bAttack != 0)
						{
							int delay = rand() % 500;
							gObjAttack(lpObj, &gGameObjects[tObjNum], lpMagic, 1, 1, 0, 0, 0, 0);
						}
					}
				}
			}
		}

		count++;
		if (count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}
	return true;
}


BOOL CObjUseSkill::SkillHellFire2(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic)
{
	if (aTargetIndex == 58)
	{
		return this->SkillHellFire2Start(aIndex, lpMagic);
	}

	

	if (lpObj.Type != OBJ_USER)
	{
		return false;
	}

	int tObjNum;

	if (lpObj.m_PlayerData->SkillHellFire2State == 0)
	{
		sLog->outBasic("[%s][%s] Nova skill didn't cast", lpObj.AccountID, lpObj.Name); //HermeX Fix
		return false;
	}

	lpObj.m_PlayerData->SkillHellFire2State = 0;
	int count = 0;
	lpObj.m_PlayerData->SkillHellFire2Time = 0;

	CGameObject lpTargetObj = &gGameObjects[aTargetIndex];
	int DuelIndex = lpObj.m_iDuelUser;
	int EnableAttack;

	while (true)
	{
		if (lpObj.VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = 0;

				if (lpObj.VpPlayer2[count].type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon < 0)
				{
					EnableAttack = 1;
				}

				else if (aTargetIndex == tObjNum || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && lpObj.VpPlayer2[count].type != OBJ_NPC && gGameObjects[tObjNum]->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = 1;
				}

				else
				{
					int CallMonIndex = gGameObjects[tObjNum]->m_Index;

					if (gGameObjects[tObjNum]->Type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon >= 0)
					{
						CallMonIndex = gGameObjects[gGameObjects[tObjNum]->m_RecallMon]->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &gGameObjects[CallMonIndex]) == 1)
					{
						EnableAttack = 1;
					}

					if (gGameObjects[tObjNum]->Class >= 678 && gGameObjects[tObjNum]->Class <= 680)
					{
						EnableAttack = 1;
					}
				}

				if (EnableAttack != 0)
				{
					if (gObjCalDistance(lpObj, &gGameObjects[tObjNum]) <= MagicDamageC.GetSkillDistance(lpMagic->m_Skill))
					{
						gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, 600, lpMagic->m_Skill, 0);
					}
				}
			}
		}

		count++;
		if (count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}

	GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_EXPHELL, lpObj.m_Index, 1);
	return true;
}



BOOL CObjUseSkill::SkillHellFire2Start(CGameObject &lpObj, CMagicInf * lpMagic)
{
	

	if (lpObj.Type != OBJ_USER)
	{
		return false;
	}

	if (lpObj.m_PlayerData->SkillHellFire2State != 0)
	{
		sLog->outBasic("[%s][%s] Nova skill already casted", lpObj.AccountID, lpObj.Name); // HermeX Fix
		return false;
	}

	lpObj.m_PlayerData->SkillHellFire2Time = GetTickCount();
	lpObj.m_PlayerData->SkillHellFire2State = 1;
	lpObj.m_PlayerData->SkillHellFire2Count = 0;

	GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_EXPHELL_START, aIndex, 1);
	return true;
}



BOOL CObjUseSkill::SkillKnightBlow(CGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic, BOOL isCombo)
{
	
	int StartDis = 1;
	int tObjNum;

	float fangle = this->GetAngle(lpObj.X, lpObj.Y, gGameObjects[aTargetIndex]->X, gGameObjects[aTargetIndex]->Y);

	this->SkillFrustrum(aIndex, fangle, 1.5f, 3.0f);

	int count = 0;
	int loopcount = 0;
	int attackcheck;

	GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, 1);

	int DuelIndex = lpObj.m_iDuelUser;
	int EnableAttack;

	while (true)
	{
		if (lpObj.VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = 0;

				if (lpObj.VpPlayer2[count].type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
				else
				{
					int CallMonIndex = gGameObjects[tObjNum]->m_Index;

					if (gGameObjects[tObjNum]->Type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon >= 0)
					{
						CallMonIndex = gGameObjects[gGameObjects[tObjNum]->m_RecallMon]->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &gGameObjects[CallMonIndex]) == 1)
					{
						EnableAttack = 1;
					}

					if (gGameObjects[tObjNum]->Class >= 678 && gGameObjects[tObjNum]->Class <= 680)
					{
						EnableAttack = 1;
					}
				}

				if (EnableAttack != 0)
				{
					attackcheck = 0;

					if (loopcount == 0)
					{
						attackcheck = 1;
					}
					else if (rand() % 3 == 0)
					{
						attackcheck = 1;
					}

					if (attackcheck != 0)
					{
						if (::SkillTestFrustrum(gGameObjects[tObjNum]->X, gGameObjects[tObjNum]->Y, aIndex))
						{
							gObjAttack(lpObj, &gGameObjects[tObjNum], lpMagic, 0, 1, 0, isCombo, 0, 0);
						}
					}
				}
			}
		}

		count++;
		if (count > MAX_VIEWPORT - 1)
		{
			loopcount++;
			count = 0;
			if (loopcount >= 3)
			{
				break;
			}
		}
	}
	return true;
}



void CObjUseSkill::SkillFrustrum(CGameObject &lpObj, float fangle, float tx, float ty)
{
	

	vec3_t p[4];

	Vector(-tx, ty, 0.f, p[0]);
	Vector(tx, ty, 0.f, p[1]);
	Vector(1.0f, 0.f, 0.f, p[2]);
	Vector(-1.0f, 0.f, 0.f, p[3]);

	vec3_t Angle;

	Vector(0.f, 0.f, fangle, Angle);

	float Matrix[3][4];

	vec3_t vFrustrum[4];

	AngleMatrix(Angle, Matrix);

	for (int i = 0; i < 4; i++)
	{
		VectorRotate(p[i], Matrix, vFrustrum[i]);

		lpObj.fSkillFrustrumX[i] = (int)vFrustrum[i][0] + lpObj.X;
		lpObj.fSkillFrustrumY[i] = (int)vFrustrum[i][1] + lpObj.Y;
	}
}



void CObjUseSkill::SkillFrustrum2(CGameObject &lpObj, float fangle, float tx, float ty)
{
	

	vec3_t p[4];

	Vector(0.f, ty, 0.f, p[0]);
	Vector(0.f, ty, 0.f, p[1]);
	Vector(tx, 1.0f, 0.f, p[2]);
	Vector(-tx, 1.0f, 0.f, p[3]);

	vec3_t Angle;

	Vector(0.f, 0.f, fangle, Angle);

	float Matrix[3][4];

	vec3_t vFrustrum[4];

	AngleMatrix(Angle, Matrix);

	for (int i = 0; i < 4; i++)
	{
		VectorRotate(p[i], Matrix, vFrustrum[i]);

		lpObj.fSkillFrustrumX[i] = (int)vFrustrum[i][0] + lpObj.X;
		lpObj.fSkillFrustrumY[i] = (int)vFrustrum[i][1] + lpObj.Y;
	}
}



int  CObjUseSkill::GetAngle(int x, int y, int tx, int ty)
{
	double diffX = x - tx;
	double diffY = y - ty;

	double rad = atan2(diffY, diffX);

	int angle = rad * 180 / 3.141592741012573 + 90;

	if (angle < 0)
	{
		angle += 360;
	}
	return angle;
}



int  CObjUseSkill::CalDistance(int x, int y, int x1, int y1)
{
	if (x == x1 && y == y1)
	{
		return 0;
	}

	float tx = x - x1;
	float ty = y - y1;

	return sqrt(tx*tx + ty*ty);
}



BOOL CObjUseSkill::SkillAreaCheck(int angel, int x, int y, int tx, int ty, int dis, int arc)
{
	int minangel = angel - arc;
	int maxangel = angel + arc;

	if (this->CalDistance(x, y, tx, ty) > dis)
	{
		return false;
	}

	int targetangle = this->GetAngle(x, y, tx, ty);

	if (minangel < 0)
	{
		if (targetangle > maxangel)
		{
			if (targetangle > minangel + 360)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else if (maxangel > targetangle)
		{
			return false;
		}
	}
	else if (maxangel > 360)
	{
		if (targetangle < minangel)
		{
			if (targetangle < maxangel - 360)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else if (maxangel < targetangle)
		{
			return false;
		}
	}
	else if (minangel > targetangle)
	{
		return false;
	}
	else if (maxangel < targetangle)
	{
		return false;
	}
	return true;
}



BOOL CObjUseSkill::SkillSpear(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic)
{
	
	int StartDis = 1;
	int tObjNum;

	int iangle = this->GetAngle(lpObj.X, lpObj.Y, gGameObjects[aTargetIndex]->X, gGameObjects[aTargetIndex]->Y);
	int count = 0;
	int loopcount = 0;
	int attackcheck;

	GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, 1);

	if (lpObj.SkillLongSpearChange == 0)
	{
		gObjAttack(lpObj, &gGameObjects[aTargetIndex], lpMagic, 0, 1, 0, 0, 0, 0);
		return true;
	}

	int DuelIndex = lpObj.m_iDuelUser;
	int EnableAttack;

	while (true)
	{
		if (lpObj.VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = 0;

				if (lpObj.VpPlayer2[count].type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
				else
				{
					int CallMonIndex = gGameObjects[tObjNum]->m_Index;

					if (gGameObjects[tObjNum]->Type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon >= 0)
					{
						CallMonIndex = gGameObjects[gGameObjects[tObjNum]->m_RecallMon]->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &gGameObjects[CallMonIndex]) == 1)
					{
						EnableAttack = 1;
					}

					if (gGameObjects[tObjNum]->Class >= 678 && gGameObjects[tObjNum]->Class <= 680)
					{
						EnableAttack = 1;
					}
				}

				if (EnableAttack != 0)
				{
					attackcheck = 0;

					if (loopcount == 0)
					{
						attackcheck = 1;
					}
					else if (rand() % 3 == 0)
					{
						attackcheck = 1;
					}

					if (attackcheck != 0)
					{
						if (SkillSpearHitBox.HitCheck(iangle, lpObj.X, lpObj.Y, gGameObjects[tObjNum]->X, gGameObjects[tObjNum]->Y))
						{
							gObjAttack(lpObj, &gGameObjects[tObjNum], lpMagic, 0, 1, 0, 0, 0, 0);
						}
					}
				}
			}
		}

		count++;
		if (count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}
	return true;
}



BOOL CObjUseSkill::SkillFireBurst(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic)
{
	
	int StartDis = 1;
	int tObjNum;
	int count = 0;
	int loopcount = 0;

	GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_FIREBURST, aTargetIndex, 1);
	gObjAttack(lpObj, &gGameObjects[aTargetIndex], lpMagic, 0, 1, 0, 0, 0, 0);

	int attackcheck;
	int DuelIndex = lpObj.m_iDuelUser;
	int EnableAttack;
	int delaytime;

	while (true)
	{
		if (lpObj.VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj.VpPlayer2[count].number;

			if (tObjNum >= 0 && aTargetIndex != tObjNum)
			{
				EnableAttack = 0;

				if (lpObj.VpPlayer2[count].type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
				else
				{
					int CallMonIndex = gGameObjects[tObjNum]->m_Index;

					if (gGameObjects[tObjNum]->Type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon >= 0)
					{
						CallMonIndex = gGameObjects[gGameObjects[tObjNum]->m_RecallMon]->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &gGameObjects[CallMonIndex]) == 1)
					{
						EnableAttack = 1;
					}

					if (gGameObjects[tObjNum]->Class >= 678 && gGameObjects[tObjNum]->Class <= 680)
					{
						EnableAttack = 1;
					}
				}

				if (EnableAttack != 0)
				{
					attackcheck = 0;

					if (loopcount == 0)
					{
						attackcheck = 1;
					}
					else if (rand() % 3 == 0)
					{
						attackcheck = 1;
					}

					if (attackcheck != 0)
					{
						if (gObjCalDistance(&gGameObjects[aTargetIndex], &gGameObjects[tObjNum]) < 3)
						{
							delaytime = (rand() * 17) % 300 + 500;

							gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, delaytime, lpMagic->m_Skill, 0);
						}
					}
				}
			}
		}

		count++;
		if (count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}
	return true;
}



BOOL CObjUseSkill::SkillDarkHorseAttack(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic)
{
	
	int tObjNum;
	int count = 0;
	int HitCount = 0;
	int bAttack;
	int DuelIndex = lpObj.m_iDuelUser;
	int EnableAttack;

	if (lpObj.Type == OBJ_USER)
	{
		if (gObjDarkHorse(lpObj) == FALSE)
		{
			return false;
		}
	}

	GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, lpObj.m_Index, 1);

	while (true)
	{
		if (lpObj.VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = 0;
				if ((gGameObjects[tObjNum]->Class < 100 || gGameObjects[tObjNum]->Class > 110) && gGameObjects[tObjNum]->Class != 689)
				{
					if (lpObj.VpPlayer2[count].type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon < 0)
					{
						EnableAttack = 1;
					}
					else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
					{
						EnableAttack = 1;
					}

					else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && lpObj.VpPlayer2[count].type != OBJ_NPC && gGameObjects[tObjNum]->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
					{
						if (g_CastleSiege.m_bCastleSiegeFriendlyFire == false &&
							lpObj.m_btCsJoinSide == gGameObjects[tObjNum]->m_btCsJoinSide)
						{
							EnableAttack = 0;
						}

						else
						{
							EnableAttack = 1;
						}
					}

					else
					{
						int CallMonIndex = gGameObjects[tObjNum]->m_Index;

						if (gGameObjects[tObjNum]->Type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon >= 0)
						{
							CallMonIndex = gGameObjects[gGameObjects[tObjNum]->m_RecallMon]->m_Index;
						}

						if (gObjTargetGuildWarCheck(lpObj, &gGameObjects[CallMonIndex]) == 1)
						{
							EnableAttack = 1;
						}

						if (gGameObjects[tObjNum]->Class >= 678 && gGameObjects[tObjNum]->Class <= 680)
						{
							EnableAttack = 1;
						}
					}

					if (EnableAttack != 0)
					{
						if (gObjCalDistance(lpObj, &gGameObjects[tObjNum]) < 5)
						{
							bAttack = 0;

							if (HitCount > 15)
							{
								bAttack = 0;
							}

							if (HitCount >= 10)
							{
								if (rand() % 2)
								{
									bAttack = 1;
								}
							}
							else
							{
								bAttack = 1;
							}

							if (bAttack != 0)
							{
								gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, 500, lpMagic->m_Skill, 0);
							}
						}
					}
				}
			}
		}

		count++;
		if (count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}
	return true;
}



BOOL CObjUseSkill::RecallPartyCheck(CGameObject &lpObj, int skill_level)
{
	
	int number;
	int partynum = 0;

	partynum = lpObj.PartyNumber;

	int partycount;
	CGameObject lpPartyObj;
	int recallcount = 0;

	if (partynum != -1)
	{
		partycount = gParty.m_PartyS[partynum].Count;

		for (int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			number = gParty.m_PartyS[partynum].Number[n];

			if (number >= 0)
			{
				lpPartyObj = &gGameObjects[number];

				if (lpPartyObj.m_Index != lpObj.m_Index)
				{
					if (lpPartyObj.MapNumber == lpObj.MapNumber)
					{
						if (lpPartyObj.m_SkillInfo.LordSummonTime != 0)
						{
							return false;
						}
					}
				}
			}
		}
	}
	return true;
}



void CObjUseSkill::SkillRecallParty(CGameObject &lpObj, int skill_level)
{
	int skillSuccess = true;
	

	if (lpObj.Type != OBJ_USER && lpObj.m_RecallMon == -1)
	{
		return;
	}

	if (this->RecallPartyCheck(aIndex, skill_level) == 0)
	{
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 132), aIndex, 1);
		return;
	}

	if (gMoveCommand.CheckMainToMove(lpObj) == 0)
	{
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 141), lpObj.m_Index, 1);
		return;
	}

	if (gMoveCommand.CheckInterfaceToMove(lpObj) == 0)
	{
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 133), lpObj.m_Index, 1);
		return;
	}

	if (lpObj.MapNumber == MAP_INDEX_HATCHERY &&
		lpObj.Connected == PLAYER_PLAYING &&
		(g_Raklion.GetRaklionState() == RAKLION_STATE_CLOSE_DOOR ||
			g_Raklion.GetRaklionState() == RAKLION_STATE_ALL_USER_DIE ||
			g_Raklion.GetRaklionState() == RAKLION_STATE_NOTIFY_4 ||
			g_Raklion.GetRaklionState() == RAKLION_STATE_END))
	{
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 299), aIndex, 1);
		return;
	}

	if (g_GensSystem.IsMapBattleZone(lpObj.MapNumber) == TRUE)
	{
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 533), aIndex, 1);
		return;
	}

	if (IMPERIAL_MAP_RANGE(lpObj.MapNumber) == TRUE)
	{
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 147), aIndex, 1);
		return;
	}

	int number;
	int partynum;
	int partycount;
	CGameObject lpPartyObj;
	int recallcount;

	partynum = 0;
	partynum = lpObj.PartyNumber;

	GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_RECALL_PARTY, lpObj.m_Index, skillSuccess);

	recallcount = 0;

	if (partynum != -1)
	{
		partycount = gParty.m_PartyS[partynum].Count;

		for (int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			number = gParty.m_PartyS[partynum].Number[n];

			if (number >= 0)
			{
				lpPartyObj = &gGameObjects[number];

				if (g_NewPVP.IsDuel(gGameObjects[number]))
				{
					GSProtocol.GCServerMsgStringSendEx(aIndex, 1, Lang.GetText(0, 322), lpPartyObj.Name);
					continue;
				}

				if (lpPartyObj.m_Index != lpObj.m_Index)
				{
					if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
					{
						if (lpPartyObj.m_btCsJoinSide != lpObj.m_btCsJoinSide)
						{
							GSProtocol.GCServerMsgStringSendEx(aIndex, 1, Lang.GetText(0, 511), lpPartyObj.Name);
							continue;
						}
					}

					int movelevel = gMoveCommand.GetMoveLevel(lpObj.MapNumber, lpObj.X, lpObj.Y, lpPartyObj.Class);
					int bCheckMainToMove = gMoveCommand.CheckMainToMove(lpPartyObj);
					int bCheckEquipmentToMove = gMoveCommand.CheckEquipmentToMove(lpPartyObj, lpObj.MapNumber);
					int bCheckInterfaceToMove = gMoveCommand.CheckInterfaceToMove(lpPartyObj);
					bool bCheckPk = g_ConfigRead.pk.CanSummonByDarkLord(lpPartyObj.m_PK_Level);
					int iVipLevel = g_MapAttr.CheckVIP(lpObj.MapNumber);

					if (lpPartyObj.Level >= movelevel
						&& movelevel != -1
						&& bCheckMainToMove != false
						&& bCheckEquipmentToMove != false
						&& bCheckInterfaceToMove != false
						&& bCheckPk != false
						&& lpPartyObj.m_PlayerData->VipType >= iVipLevel)
					{
						int addx;
						int addy;
						int count = 50;
						int Find = 0;

						while (count--)
						{
							addx = lpObj.X + rand() % 9 - 4;
							addy = lpObj.Y + rand() % 9 - 4;

							if (gObjCheckTeleportArea(lpObj.m_Index, addx, addy))
							{
								Find = 1;
								break;
							}
						}

						if (Find == 0)
						{
							addx = lpObj.X;
							addy = lpObj.Y;
						}

						lpPartyObj.m_SkillInfo.LordSummonTime = 7;
						lpPartyObj.m_SkillInfo.LordSummonMapNumber = lpObj.MapNumber;
						lpPartyObj.m_SkillInfo.LordSummonX = addx;
						lpPartyObj.m_SkillInfo.LordSummonY = addy;

						if (lpObj.MapNumber == MAP_INDEX_HATCHERY)
						{
							g_RaklionBattleUserMng.AddUserData(lpPartyObj.m_Index);
						}

						if (lpPartyObj.m_PlayerData->m_bIsMining)
						{
							g_MineSystem.CheckMoveMapWhileMining(lpPartyObj.m_Index);
						}

						GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 143), lpPartyObj.m_Index, 1);
						recallcount++;
					}
					else
					{
						GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 144), lpPartyObj.m_Index, 1);
					}
				}
			}
		}
	}

	if (recallcount != 0)
	{
		char msg[255];
		wsprintf(msg, Lang.GetText(0, 136), recallcount);
		GSProtocol.GCServerMsgStringSend(msg, lpObj.m_Index, 1);
	}
}



void CObjUseSkill::SkillAddCriticalDamage(CGameObject &lpObj, int skill_level, CMagicInf* lpMagic)
{
	int skillSuccess = true;
	
	int number;

	if (lpObj.Type != OBJ_USER && lpObj.m_RecallMon == -1)
	{
		return;
	}

	if (lpObj.Class != CLASS_DARKLORD && lpObj.m_PlayerData->ISBOT == false)
	{
		return;
	}

	int partynum = 0;
	int totallevel = 0;
	int partycount;
	int dis;

	int ApplyPartyIndex[MAX_USER_IN_PARTY];
	CGameObject lpPartyObj;
	memset(ApplyPartyIndex, -1, sizeof(ApplyPartyIndex));

	partynum = lpObj.PartyNumber;

	if (partynum != -1)
	{
		partycount = gParty.m_PartyS[partynum].Count;

		for (int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			number = gParty.m_PartyS[partynum].Number[n];

			if (number >= 0)
			{
				lpPartyObj = &gGameObjects[number];

				if (lpObj.MapNumber == lpPartyObj.MapNumber)
				{
					dis = gObjCalDistance(lpObj, &gGameObjects[number]);
					if (dis < 10)
					{
						ApplyPartyIndex[n] = lpPartyObj.m_Index;
					}
				}
			}
		}
	}

	int addcriticaldamagevalue = 0;
	int SkillTime = 0;

	this->m_Lua.Generic_Call("DarkLordCriticalDamage", "ii>ii", (lpObj.Leadership + lpObj.AddLeadership), (lpObj.m_PlayerData->Energy + lpObj.AddEnergy), &addcriticaldamagevalue, &SkillTime);

	if (partynum == -1 && lpObj.m_PlayerData->ISBOT == false)
	{
		gObjAddBuffEffect(lpObj, BUFFTYPE_CRITICAL_DMG_INC, EFFECTTYPE_CRITICAL_DMG, addcriticaldamagevalue, 0, 0, SkillTime);
		MsgOutput(aIndex, Lang.GetText(0, 134), SkillTime);
		GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, lpObj.m_Index, skillSuccess);
	}
	else if (lpObj.m_PlayerData->ISBOT == true)
	{
		SkillTime = g_BotSystem.GetSkillTime(lpObj.m_Index, lpMagic->m_Skill);
		gObjAddBuffEffect(&gGameObjects[lpObj.BuffPlayerIndex], BUFFTYPE_CRITICAL_DMG_INC, EFFECTTYPE_CRITICAL_DMG, addcriticaldamagevalue, 0, 0, SkillTime);

		MsgOutput(lpObj.BuffPlayerIndex, Lang.GetText(0, 134), SkillTime);
		GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, lpObj.m_Index, skillSuccess);
	}
	else
	{
		for (int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			if (ApplyPartyIndex[n] != -1)
			{
				lpPartyObj = &gGameObjects[ApplyPartyIndex[n]];
				gObjAddBuffEffect(lpPartyObj, BUFFTYPE_CRITICAL_DMG_INC, EFFECTTYPE_CRITICAL_DMG, addcriticaldamagevalue, 0, 0, SkillTime);

				MsgOutput(ApplyPartyIndex[n], Lang.GetText(0, 134), SkillTime);
				GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, lpPartyObj.m_Index, skillSuccess);
			}
		}
	}
}

void CObjUseSkill::SkillElectricSpark(CGameObject &lpObj, CMagicInf * lpMagic, BYTE x, BYTE y, BYTE dir, BYTE TargetPos, int aTargetIndex)
{
	

	int TargePosx = lpObj.X - (8 - (TargetPos & 15));
	int TargePosy = lpObj.Y - (8 - ((TargetPos & 240) >> 4));

	int tObjNum;
	int count = 0;
	int DuelIndex = lpObj.m_iDuelUser;
	int iangle = this->GetAngle(lpObj.X, lpObj.Y, TargePosx, TargePosy);
	int delaytime;
	int Hit = 0;
	int EnableAttack;

	while (true)
	{
		if (lpObj.VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = 0;

				if (lpObj.VpPlayer2[count].type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon < 0)
				{
					EnableAttack = 1;
				}

				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && lpObj.VpPlayer2[count].type != OBJ_NPC && gGameObjects[tObjNum]->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = 1;
				}

				else
				{
					int CallMonIndex = gGameObjects[tObjNum]->m_Index;

					if (gGameObjects[tObjNum]->Type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon >= 0)
					{
						CallMonIndex = gGameObjects[gGameObjects[tObjNum]->m_RecallMon]->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &gGameObjects[CallMonIndex]) == 1)
					{
						EnableAttack = 1;
					}

					if (gGameObjects[tObjNum]->Class >= 678 && gGameObjects[tObjNum]->Class <= 680)
					{
						EnableAttack = 1;
					}
				}
				if (EnableAttack != 0)
				{
					if (SkillElectricSparkHitBox.HitCheck(iangle, lpObj.X, lpObj.Y, gGameObjects[tObjNum]->X, gGameObjects[tObjNum]->Y))
					{
						delaytime = 500;
						Hit = 1;

						gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, delaytime, lpMagic->m_Skill, 0);
					}
				}
			}
		}

		count++;
		if (count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}

	if (Hit != 0)
	{
		DecPartyMemberHPandMP(lpObj);
	}
}



int  DecPartyMemberHPandMP(CGameObject &lpObj)
{
	CGameObject lpPartyObj;
	int partynum = lpObj.PartyNumber;

	if (ObjectMaxRange(partynum) == false)
	{
		return false;
	}

	int partycount = gParty.m_PartyS[partynum].Count;
	int retcount = 0;

	for (int n = 0; n < MAX_USER_IN_PARTY; n++)
	{
		int number = gParty.m_PartyS[partynum].Number[n];

		if (number >= 0)
		{
			lpPartyObj = &gGameObjects[number];

			if (lpObj.MapNumber == lpPartyObj.MapNumber && lpObj.m_Index != number)
			{
				int dis = gObjCalDistance(lpObj, &gGameObjects[number]);
				if (dis < 10)
				{
					lpPartyObj = &gGameObjects[number];

					int declife = lpPartyObj.Life * 20.0f / 100.0f;
					int decmana = lpPartyObj.Mana * 5.0f / 100.0f;

					lpPartyObj.Life = lpPartyObj.Life - declife;
					lpPartyObj.Mana = lpPartyObj.Mana - decmana;

					GSProtocol.GCReFillSend(lpPartyObj.m_Index, lpPartyObj.Life, 0xFF, 0, lpPartyObj.iShield);
					GSProtocol.GCManaSend(lpPartyObj.m_Index, lpPartyObj.Mana, 0xFF, 0, lpPartyObj.BP);
					retcount++;
				}
			}
		}
	}

	return retcount;
}



BOOL CObjUseSkill::SkillSummon(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic)
{
	if (ObjectMaxRange(aIndex) == false)
	{
		return false;
	}

	int iSummonCount = 1;
	int iSummonMonterType = 0;
	BYTE bDoSummon = 1;
	BYTE bSummonSuccess = 0;
	BYTE cSummonX = 0;
	BYTE cSummonY = 0;

	

	switch (lpObj.Class)
	{
	case 161:
		iSummonMonterType = rand() % 2 + 147;
		bDoSummon = (rand() % 15) ? false : true;
		break;
	case 181:
		iSummonMonterType = rand() % 2 + 177;
		bDoSummon = (rand() % 15) ? false : true;
		break;
	case 189:
		iSummonMonterType = rand() % 2 + 185;
		bDoSummon = (rand() % 15) ? false : true;
		break;
	case 197:
		iSummonMonterType = rand() % 2 + 193;
		bDoSummon = (rand() % 15) ? false : true;
		break;
	case 267:
		iSummonMonterType = rand() % 2 + 263;
		bDoSummon = (rand() % 15) ? false : true;
		break;
	case 275:
		iSummonMonterType = rand() % 2 + 271;
		bDoSummon = (rand() % 15) ? false : true;
		break;
	}

	if (bDoSummon == false)
	{
		return false;
	}

	for (int iSC = 0; iSC < iSummonCount; iSC++)
	{
		BYTE cSX = lpObj.X;
		BYTE cSY = lpObj.Y;

		if (gObjGetRandomFreeLocation(lpObj.MapNumber, (BYTE &)cSX, (BYTE &)cSY, 2, 2, 30) == 1)
		{
			int result = gObjAddMonster(lpObj.MapNumber);

			if (result >= 0)
			{
				gGameObjects[result]->m_PosNum = (WORD)-1;
				gGameObjects[result]->X = cSX;
				gGameObjects[result]->Y = cSY;
				gGameObjects[result]->MapNumber = lpObj.MapNumber;
				gGameObjects[result]->TX = gGameObjects[result]->X;
				gGameObjects[result]->TY = gGameObjects[result]->Y;
				gGameObjects[result]->m_OldX = gGameObjects[result]->X;
				gGameObjects[result]->m_OldY = gGameObjects[result]->Y;
				gGameObjects[result]->Dir = 1;
				gGameObjects[result]->StartX = gGameObjects[result]->X;
				gGameObjects[result]->StartY = gGameObjects[result]->Y;
				gObjSetMonster(result, iSummonMonterType);
				gGameObjects[result]->m_Attribute = 60;
				gGameObjects[result]->MaxRegenTime = 0;
				gGameObjects[result]->Dir = rand() % 8;
				bSummonSuccess = 1;
				cSummonX = gGameObjects[result]->StartX;
				cSummonY = gGameObjects[result]->StartY;
			}
		}
	}

	GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_SUMMON, (((cSummonX & 0xFF) & 0xFF) | ((cSummonY & 0xFF) & 0xFF) << 8) & 0xFFFF, bSummonSuccess);
	return bSummonSuccess;
}



BOOL CObjUseSkill::SkillImmuneToMagic(CGameObject &lpObj, CMagicInf * lpMagic)
{
	if (ObjectMaxRange(aIndex) == false)
	{
		return false;
	}

	

	if (gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_MONSTER_MAGIC_IMMUNE) == 0 && gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_MONSTER_MELEE_IMMUNE) == 0)
	{
		gObjAddBuffEffect(lpObj, BUFFTYPE_MONSTER_MAGIC_IMMUNE, 0, 0, 0, 0, 10);
		GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_IMMUNE_TO_MAGIC, lpObj.m_Index, 1);
		return true;
	}
	return false;
}



BOOL CObjUseSkill::SkillImmuneToHarm(CGameObject &lpObj, CMagicInf * lpMagic)
{
	if (ObjectMaxRange(aIndex) == false)
	{
		return false;
	}

	

	if (gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_MONSTER_MAGIC_IMMUNE) == 0 && gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_MONSTER_MELEE_IMMUNE) == 0)
	{
		gObjAddBuffEffect(lpObj, BUFFTYPE_MONSTER_MELEE_IMMUNE, 0, 0, 0, 0, 10);
		GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_IMMUNE_TO_HARM, lpObj.m_Index, 1);
		return true;
	}
	return false;
}



BOOL CObjUseSkill::RemoveAllCharacterInvalidMagicAndSkillState(CGameObject &lpObj)
{
	if (lpObj.Type != OBJ_USER)
	{
		return false;
	}

	//BuffEffectC.ClearBuff(lpObj);

	return true;
}


//
void CObjUseSkill::SkillKnightRush(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic)
{
	

	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	GSProtocol.GCMagicAttackNumberSend(lpObj, 631, aTargetIndex, 1);
	gObjAttack(lpObj, &gGameObjects[aTargetIndex], lpMagic, TRUE, 0, 0, 0, 0, 0);
}



BOOL CObjUseSkill::SkillWizardJavalin(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic)
{
	if (ObjectMaxRange(aIndex) == false)
	{
		return false;
	}

	

	if (lpObj.MapNumber != MAP_INDEX_CASTLESIEGE && lpObj.MapNumber != MAP_INDEX_CASTLEHUNTZONE && this->m_SkillData.EnableSiegeOnAllMaps == false)
	{
		return false;
	}

	GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, 1);

	int iDelay = 500;

	gObjAddAttackProcMsgSendDelay(lpObj, 50, aTargetIndex, iDelay, lpMagic->m_Skill, 0);
	gObjAddAttackProcMsgSendDelay(lpObj, 50, aTargetIndex, iDelay + 200, lpMagic->m_Skill, 0);
	return true;
}



BOOL CObjUseSkill::SkillElfRush(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic)
{
	if (ObjectMaxRange(aIndex) == false)
	{
		return false;
	}

	

	if (lpObj.MapNumber != MAP_INDEX_CASTLESIEGE && lpObj.MapNumber != MAP_INDEX_CASTLEHUNTZONE && this->m_SkillData.EnableSiegeOnAllMaps == false)
	{
		return false;
	}

	GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, 1);
	gObjAddAttackProcMsgSendDelay(lpObj, 50, aTargetIndex, 800, lpMagic->m_Skill, 0);
	return true;
}



BOOL CObjUseSkill::SkillMagumOneFlash(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic)
{
	if (ObjectMaxRange(aIndex) == false)
	{
		return false;
	}

	

	if (lpObj.MapNumber != MAP_INDEX_CASTLESIEGE && lpObj.MapNumber != MAP_INDEX_CASTLEHUNTZONE && this->m_SkillData.EnableSiegeOnAllMaps == false)
	{
		return false;
	}

	GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, 1);
	gObjAttack(lpObj, &gGameObjects[aTargetIndex], lpMagic, 0, 1, 0, 0, 0, 0);
	gObjAddAttackProcMsgSendDelay(lpObj, 50, aTargetIndex, 100, lpMagic->m_Skill, 0);
	return true;
}



BOOL CObjUseSkill::SkillMagumDeathCannon(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic)
{
	if (ObjectMaxRange(aIndex) == false)
	{
		return false;
	}

	

	if (lpObj.MapNumber != MAP_INDEX_CASTLESIEGE && lpObj.MapNumber != MAP_INDEX_CASTLEHUNTZONE && this->m_SkillData.EnableSiegeOnAllMaps == false)
	{
		return false;
	}

	GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, 1);
	gObjAttack(lpObj, &gGameObjects[aTargetIndex], lpMagic, 0, 1, 0, 0, 0, 0);
	return true;
}



BOOL CObjUseSkill::SkillDarklordSpaceSplit(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic)
{
	if (ObjectMaxRange(aIndex) == false)
	{
		return false;
	}

	

	if (lpObj.MapNumber != MAP_INDEX_CASTLESIEGE && lpObj.MapNumber != MAP_INDEX_CASTLEHUNTZONE && this->m_SkillData.EnableSiegeOnAllMaps == false)
	{
		return false;
	}

	GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, 1);
	gObjAddAttackProcMsgSendDelay(lpObj, 50, aTargetIndex, 500, lpMagic->m_Skill, 0);
	return true;
}



BOOL CObjUseSkill::SkillBrandOfSkill(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic)
{
	if (ObjectMaxRange(aIndex) == false)
	{
		return false;
	}

	
	int tObjNum;

	if (lpObj.MapNumber != MAP_INDEX_CASTLESIEGE && lpObj.MapNumber != MAP_INDEX_CASTLEHUNTZONE && this->m_SkillData.EnableSiegeOnAllMaps == false)
	{
		return false;
	}

	int count = 0;

	gObjAddBuffEffect(lpObj, BUFFTYPE_DARKLORD_CASTLE_SKILL, 0, 0, 0, 0, 10);
	GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_BRAND_OF_SKILL, lpObj.m_Index, 1);

	while (true)
	{
		if (lpObj.VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj.VpPlayer2[count].number;

			if (ObjectMaxRange(tObjNum))
			{
				if (lpObj.VpPlayer2[count].type == OBJ_USER)
				{
					if (gObjCalDistance(lpObj, &gGameObjects[tObjNum]) < 5)
					{
						gObjAddBuffEffect(&gGameObjects[tObjNum], BUFFTYPE_DARKLORD_CASTLE_SKILL, 0, 0, 0, 0, 10);
						GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_BRAND_OF_SKILL, tObjNum, 1);
					}
				}
			}
		}

		count++;

		if (count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}
	return true;
}



BOOL CObjUseSkill::SkillStun(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic, BYTE btX, BYTE btY, BYTE btDir, BYTE btTargetPos)
{
	if (ObjectMaxRange(aIndex) == false)
	{
		return false;
	}

	

	int TargePosx = lpObj.X - (8 - (btTargetPos & 15));
	int TargePosy = lpObj.Y - (8 - ((btTargetPos & 240) >> 4));

	int iangle = this->GetAngle(lpObj.X, lpObj.Y, TargePosx, TargePosy);

	if (lpObj.MapNumber != MAP_INDEX_CASTLESIEGE && lpObj.MapNumber != MAP_INDEX_CASTLEHUNTZONE && this->m_SkillData.EnableSiegeOnAllMaps == false)
	{
		return false;
	}

	int tObjNum;
	int count = 0;

	int iStunTime = g_ConfigRead.StunTimeStunSkill67;

	gObjSetKillCount(aIndex, 0);

	while (true)
	{
		if (lpObj.VpPlayer2[count].state != 0)
		{
			if (lpObj.VpPlayer2[count].type == OBJ_USER || lpObj.VpPlayer2[count].type == OBJ_MONSTER)
			{
				tObjNum = lpObj.VpPlayer2[count].number;

				if (ObjectMaxRange(tObjNum))
				{
					if (gObjCalDistance(lpObj, &gGameObjects[tObjNum]) <= 4)
					{
						if (SkillElectricSparkHitBox.HitCheck(iangle, lpObj.X, lpObj.Y, gGameObjects[tObjNum]->X, gGameObjects[tObjNum]->Y))
						{
							if (g_ConfigRead.EnableStunEffect == 1)
							{
								gObjAddBuffEffect(&gGameObjects[tObjNum], BUFFTYPE_STUN, 0, 0, 0, 0, iStunTime);
							}

							GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_STUN, tObjNum, 1);
							gObjSetPosition(tObjNum, gGameObjects[tObjNum]->X, gGameObjects[tObjNum]->Y);
						}
					}
				}
			}
		}

		count++;

		if (count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}
	return true;
}



BOOL CObjUseSkill::SkillRemoveStun(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic)
{

	if (ObjectMaxRange(aIndex) == false)
	{
		return false;
	}

	

	if (lpObj.MapNumber != MAP_INDEX_CASTLESIEGE && lpObj.MapNumber != MAP_INDEX_CASTLEHUNTZONE && this->m_SkillData.EnableSiegeOnAllMaps == false)
	{
		return false;
	}

	int tObjNum;
	int count = 0;

	gObjSetKillCount(aIndex, 0);
	GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_REMOVAL_STUN, lpObj.m_Index, 1);

	while (true)
	{
		if (lpObj.VpPlayer2[count].state != 0)
		{

			tObjNum = lpObj.VpPlayer2[count].number;
			if (ObjectMaxRange(tObjNum))
			{
				if (lpObj.VpPlayer2[count].type == OBJ_USER)
				{
					if (gObjCalDistance(lpObj, &gGameObjects[tObjNum]) < 6)
					{
						gObjRemoveBuffEffect(&gGameObjects[tObjNum], BUFFTYPE_STUN);
						GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_REMOVAL_STUN, tObjNum, 1);
					}
				}
			}
		}

		count++;

		if (count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}
	return true;
}



BOOL CObjUseSkill::SkillAddMana(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic)
{
	if (ObjectMaxRange(aIndex) == false)
	{
		return false;
	}

	

	if (lpObj.MapNumber != MAP_INDEX_CASTLESIEGE && lpObj.MapNumber != MAP_INDEX_CASTLEHUNTZONE && this->m_SkillData.EnableSiegeOnAllMaps == false)
	{
		return false;
	}

	int tObjNum;
	int count = 0;
	gObjSetKillCount(aIndex, 0);

	int iSwelledMana = short(lpObj.MaxMana + lpObj.AddMana) * 50 / 100;

	gObjAddBuffEffect(lpObj, BUFFTYPE_MANA_INC, EFFECTTYPE_MANA, iSwelledMana, 0, 0, 60);

	GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_ADD_MANA, lpObj.m_Index, 1);

	while (true)
	{
		if (lpObj.VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj.VpPlayer2[count].number;

			if (ObjectMaxRange(tObjNum) && lpObj.VpPlayer2[count].type == OBJ_USER)
			{
				if (gObjCalDistance(lpObj, &gGameObjects[tObjNum]) < 5)
				{
					if (lpObj.m_PlayerData->GuildNumber > 0 && gGameObjects[tObjNum]->m_PlayerData->GuildNumber > 0)
					{
						if (lpObj.m_PlayerData->GuildNumber != gGameObjects[tObjNum]->m_PlayerData->GuildNumber)
						{
							count++;
							continue;
						}

						iSwelledMana = short(gGameObjects[tObjNum]->MaxMana + gGameObjects[tObjNum]->AddMana) * 50 / 100;

						gObjAddBuffEffect(&gGameObjects[tObjNum], BUFFTYPE_MANA_INC, EFFECTTYPE_MANA, iSwelledMana, 0, 0, 60);
						GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_ADD_MANA, tObjNum, 1);
					}
				}
			}
		}

		count++;

		if (count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}
	return true;
}



BOOL CObjUseSkill::SkillCloaking(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic)
{
	if (ObjectMaxRange(aIndex) == false)
	{
		return false;
	}

	if (ObjectMaxRange(aTargetIndex) == false)
	{
		return false;
	}

	
	CGameObject lpTargetObj = &gGameObjects[aTargetIndex];

	if (lpObj.MapNumber != MAP_INDEX_CASTLESIEGE && lpObj.MapNumber != MAP_INDEX_CASTLEHUNTZONE && this->m_SkillData.EnableSiegeOnAllMaps == false)
	{
		return false;
	}

	if (lpTargetObj.Type != OBJ_USER)
	{
		sLog->outBasic("[ANTI-HACK][Cloaking Skill] [%s][%s] Invalid Targeting  TargetType [%d] TargetClass [%d]",
			lpObj.AccountID, lpObj.Name, lpTargetObj.Type, lpTargetObj.Class);
		return false;
	}

	gObjSetKillCount(aIndex, 0);

	GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_INVISIBLE, lpObj.m_Index, 1);
	//lpTargetObj.m_iSkillInvisibleTime = 60;

	gObjAddBuffEffect(lpTargetObj, BUFFTYPE_INVISABLE, 0, 0, 0, 0, 60);
	GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_INVISIBLE, aTargetIndex, 1);
	return true;
}



BOOL CObjUseSkill::SkillRemoveCloaking(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic)
{
	if (ObjectMaxRange(aIndex) == false)
	{
		return false;
	}

	

	if (lpObj.MapNumber != MAP_INDEX_CASTLESIEGE && lpObj.MapNumber != MAP_INDEX_CASTLEHUNTZONE && this->m_SkillData.EnableSiegeOnAllMaps == false)
	{
		return false;
	}

	if ((lpObj.Authority & 0x20) == 0x20 && gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_INVISABLE) == TRUE)
	{
		return false;
	}

	int tObjNum;
	int count = 0;

	gObjSetKillCount(aIndex, 0);
	GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_REMOVAL_INVISIBLE, lpObj.m_Index, 1);

	while (true)
	{
		if (lpObj.VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj.VpPlayer2[count].number;

			if (ObjectMaxRange(tObjNum) && lpObj.VpPlayer2[count].type == OBJ_USER)
			{
				if (gObjCalDistance(lpObj, &gGameObjects[tObjNum]) < 5)
				{
					gObjRemoveBuffEffect(&gGameObjects[tObjNum], BUFFTYPE_INVISABLE);
					GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_REMOVAL_INVISIBLE, tObjNum, 1);
				}
			}
		}

		count++;

		if (count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}
	return true;
}



BOOL CObjUseSkill::SkillRemoveMagic(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic)
{
	if (ObjectMaxRange(aIndex) == false)
	{
		return false;
	}

	

	if (lpObj.MapNumber != MAP_INDEX_CASTLESIEGE && lpObj.MapNumber != MAP_INDEX_CASTLEHUNTZONE && this->m_SkillData.EnableSiegeOnAllMaps == false)
	{
		return false;
	}

	int tObjNum;
	int count = 0;

	gObjSetKillCount(aIndex, 0);
	GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_REMOVAL_MAGIC, lpObj.m_Index, 1);

	while (true)
	{
		if (lpObj.VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj.VpPlayer2[count].number;

			if (ObjectMaxRange(tObjNum) && lpObj.VpPlayer2[count].type == OBJ_USER)
			{
				if (gObjCalDistance(lpObj, &gGameObjects[tObjNum]) < 5)
				{
					this->RemoveAllCharacterInvalidMagicAndSkillState(&gGameObjects[tObjNum]);
					GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_REMOVAL_MAGIC, tObjNum, 1);
				}
			}
		}

		count++;

		if (count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////

BOOL CObjUseSkill::SkillFireScream(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic)
{
	if (g_SkillAdditionInfo.GetFireScreamSkill() == false)
	{
		return false;
	}

	
	CGameObject lpTargetObj = &gGameObjects[aTargetIndex];

	if (gObjIsConnected(lpObj.m_Index) == false && gObjIsConnected(lpTargetObj.m_Index) == false)
	{
		return false;
	}

	if (lpObj.Type != OBJ_USER)
	{
		return false;
	}

	if (lpObj.Class != CLASS_DARKLORD)
	{
		return false;
	}

	gObjAttack(lpObj, lpTargetObj, lpMagic, 0, 0, 0, 0, 0, 0);
	return true;
}


BOOL CObjUseSkill::FireScreamExplosionAttack(CGameObject &lpObj, CGameObject lpTargetObj, int iAttackDamage, int iAddDamage)
{
	if (gObjIsConnected(lpObj.m_Index) == false && gObjIsConnected(lpTargetObj.m_Index) == false)
	{
		return false;
	}

	if (rand() % 10000 > g_SkillAdditionInfo.GetFireScreamExplosionRate())
	{
		return false;
	}

	int iExplosionDamage = iAttackDamage / 10;

	int iTarObjNum;

	if (iExplosionDamage < 1)
	{
		return true;
	}

	if (iAddDamage > 0)
	{
		iExplosionDamage += iAddDamage;
	}

	int iCount = 0;
	int bEnableAttack;

	while (true)
	{
		if (lpObj.VpPlayer2[iCount].state != 0)
		{
			iTarObjNum = lpObj.VpPlayer2[iCount].number;

			if (iTarObjNum >= 0)
			{
				bEnableAttack = 0;

				if (gGameObjects[iTarObjNum]->Type == OBJ_MONSTER && gGameObjects[iTarObjNum]->m_RecallMon < 0)
				{
					bEnableAttack = 1;
				}

				else if (lpTargetObj.Type == OBJ_USER && lpObj.Type == OBJ_USER && gGameObjects[iTarObjNum]->Type != OBJ_NPC)
				{
					bEnableAttack = 1;
				}

				else if (lpTargetObj.m_cChaosCastleIndex == gGameObjects[iTarObjNum]->m_cChaosCastleIndex && lpTargetObj.m_cChaosCastleIndex != -1)
				{
					bEnableAttack = 1;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && lpObj.VpPlayer2[iCount].type != OBJ_NPC && gGameObjects[iTarObjNum]->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					if (lpObj.m_btCsJoinSide != gGameObjects[iTarObjNum]->m_btCsJoinSide && gGameObjects[iTarObjNum]->m_btCsJoinSide > 0)
					{
						bEnableAttack = TRUE;
					}
				}

				if (gGameObjects[iTarObjNum]->Class >= 678 && gGameObjects[iTarObjNum]->Class <= 680)
				{
					bEnableAttack = 1;
				}

				if (bEnableAttack != 0)
				{
					if (lpObj.m_Index != gGameObjects[iTarObjNum]->m_Index)
					{
						int iSkillDistance = gObjCalDistance(lpTargetObj, &gGameObjects[iTarObjNum]);

						if (iSkillDistance <= g_SkillAdditionInfo.GetFireScreamExplosionAttackDistance())
						{
							gObjAddMsgSendDelay(lpObj, 54, iTarObjNum, 100, iExplosionDamage);
						}
					}
				}
			}
		}

		iCount++;

		if (iCount > MAX_VIEWPORT - 1)
		{
			break;
		}
	}
	return true;
}

BOOL CObjUseSkill::RemoveCloakingEffect(CGameObject &lpObj)
{
	if (ObjectMaxRange(aIndex) == false)
	{
		return false;
	}

	gObjRemoveBuffEffect(&gGameObjects[aIndex], BUFFTYPE_INVISABLE);
	return true;
}

void CObjUseSkill::SpecificSkillAdditionTreat(CGameObject &lpObj, CGameObject lpTargetObj, CMagicInf * lpMagic, int iAttackDamage)
{
	WORD bySkillNum = lpMagic->m_Skill;

	if (bySkillNum == AT_SKILL_FIRESCREAM || bySkillNum == 518 || bySkillNum == 520)
	{
		int iAddDamage = 0;

		if (lpMagic->m_Skill == 520)
		{
			iAddDamage = g_MasterLevelSkillTreeSystem.GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);
		}

		gObjUseSkill.FireScreamExplosionAttack(lpObj, lpTargetObj, iAttackDamage, iAddDamage);
	}
}

int CObjUseSkill::RemoveSpecificSkillState(CGameObject &lpObj, int iSkillNumber)
{
	if (lpObj.Type != OBJ_USER)
	{
		return false;
	}

	int BuffNum = 0;

	if (iSkillNumber == AT_SKILL_ATTACK)
	{
		BuffNum = 1;
	}
	else if (iSkillNumber == AT_SKILL_DEFENSE)
	{
		BuffNum = 2;
	}

	gObjRemoveBuffEffect(lpObj, BuffNum);
	return true;
}

int CObjUseSkill::SkillFenrirAttack(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic)
{
	
	int tObjNum;
	int count = 0;
	int HitCount = 0;
	int bAttack;
	int DuelIndex = lpObj.m_iDuelUser;
	int EnableAttack;

	if (lpObj.Level < 300)
	{
		return false;
	}

	while (true)
	{
		if (lpObj.VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = 0;

				if ((gGameObjects[tObjNum]->Class < 100 || gGameObjects[tObjNum]->Class > 110) && gGameObjects[tObjNum]->Class != 689)
				{
					if (lpObj.VpPlayer2[count].type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon < 0)
					{
						EnableAttack = 1;
					}
					else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
					{
						EnableAttack = 1;
					}

					else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && lpObj.VpPlayer2[count].type != OBJ_NPC && gGameObjects[tObjNum]->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
					{
						EnableAttack = 1;
					}

					else
					{
						int CallMonIndex = gGameObjects[tObjNum]->m_Index;

						if (gGameObjects[tObjNum]->Type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon >= 0)
						{
							CallMonIndex = gGameObjects[gGameObjects[tObjNum]->m_RecallMon]->m_Index;
						}

						if (gObjTargetGuildWarCheck(lpObj, &gGameObjects[CallMonIndex]) == 1)
						{
							EnableAttack = 1;
						}

						if (gGameObjects[tObjNum]->Class >= 678 && gGameObjects[tObjNum]->Class <= 680)
						{
							EnableAttack = 1;
						}
					}
				}

				if (EnableAttack != 0)
				{
					if (gObjCalDistance(lpObj, &gGameObjects[tObjNum]) < 6)
					{
						bAttack = 0;

						if (HitCount >= 5)
						{
							bAttack = 0;
						}
						else
						{
							bAttack = 1;
						}

						if (tObjNum == aTargetIndex)
						{
							bAttack = 1;
						}

						if (bAttack != 0)
						{
							gObjAddAttackProcMsgSendDelay(lpObj, 53, tObjNum, 300, lpMagic->m_Skill, 0);
							HitCount++;
						}
					}
				}
			}
		}

		count++;

		if (count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}
	return true;
}

int CObjUseSkill::SkillInfinityArrow(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic)
{
	

	if (lpObj.Type != OBJ_USER)
	{
		return false;
	}

	if (lpObj.Class != CLASS_ELF || lpObj.m_PlayerData->ChangeUP == 0)
	{
		return false;
	}

	if (lpObj.m_Index != aTargetIndex)
	{
		return false;
	}

	int Time = g_SkillAdditionInfo.GetInfinityArrowSkillTime();

	gObjAddBuffEffect(lpObj, BUFFTYPE_INFINITY_ARROW, 0, 0, 0, 0, -10);
	GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, lpObj.m_Index, 1);
	sLog->outBasic("[%s][%s] Use Infinity Arrow Skill (Time:%d)(Character Level : %d)(ChangeUp: %d)",
		lpObj.AccountID, lpObj.Name, Time, lpObj.Level, lpObj.m_PlayerData->ChangeUP);

	return true;
}

int CObjUseSkill::GetAddUseMana(CGameObject &lpObj, CMagicInf * lpMagic)
{
	int iRetValue = 0;

	if (lpObj.Class == CLASS_ELF && lpObj.Type == OBJ_USER && lpObj.m_PlayerData->ChangeUP != FALSE && (gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_INFINITY_ARROW) == TRUE || gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_INFINITY_ARROW_STR) == TRUE))
	{
		if (lpMagic->m_Skill == AT_SKILL_CROSSBOW
			|| lpMagic->m_Skill == AT_SKILL_BOW
			|| lpMagic->m_Skill == AT_SKILL_ELFHARDEN
			|| lpMagic->m_Skill == AT_SKILL_PENETRATION
			|| lpMagic->m_Skill == AT_SKILL_5CROSSBOW
			|| g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == 414
			|| lpMagic->m_Skill == 416
			|| lpMagic->m_Skill == 418
			|| lpMagic->m_Skill == 424)
		{
			int iArrowLevel = 0;

			CItem * Right = &lpObj.pInventory[0];
			CItem * Left = &lpObj.pInventory[1];

			if (Right->m_Type >= ITEMGET(4, 8) && Right->m_Type <= ITEMGET(4, 14)
				|| Right->m_Type == ITEMGET(4, 16)
				|| Right->m_Type == ITEMGET(4, 18)
				|| Right->m_Type == ITEMGET(4, 19)
				|| Right->m_Type == ITEMGET(4, 26))
			{
				if (Left->m_Type == ITEMGET(4, 7))
				{
					iArrowLevel = Left->m_Level;
				}
			}
			else if (Right->m_Type >= ITEMGET(4, 0) && Right->m_Type <= ITEMGET(4, 6)
				|| Right->m_Type == ITEMGET(4, 17)
				|| Right->m_Type == ITEMGET(4, 20)
				|| Right->m_Type == ITEMGET(4, 21)
				|| Right->m_Type == ITEMGET(4, 22)
				|| Right->m_Type == ITEMGET(4, 23)
				|| Right->m_Type == ITEMGET(4, 24)
				|| Right->m_Type == ITEMGET(4, 25))
			{
				if (Left->m_Type == ITEMGET(4, 15))
				{
					iArrowLevel = Left->m_Level;
				}
			}

			if (iArrowLevel == 0)
			{
				iRetValue = g_SkillAdditionInfo.GetInfinityArrowMPConsumptionPlus0();
			}
			else if (iArrowLevel == 1)
			{
				iRetValue = g_SkillAdditionInfo.GetInfinityArrowMPConsumptionPlus1();
			}
			else if (iArrowLevel == 2)
			{
				iRetValue = g_SkillAdditionInfo.GetInfinityArrowMPConsumptionPlus2();
			}
			else if (iArrowLevel == 3)
			{
				iRetValue = g_SkillAdditionInfo.GetInfinityArrowMPConsumptionPlus3();
			}
		}
	}

	return iRetValue;
}

int CObjUseSkill::SkillBookSahamutt(CGameObject &lpObj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex)
{
	int count = 0;
	int tObjNum;
	int EnableAttack;
	int DuelIndex = lpObj.m_iDuelUser;
	
	GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_SAHAMUTT, aTargetIndex, 1);
	int Distance = MagicDamageC.GetSkillDistance(lpMagic->m_Skill);

	if (this->CalDistance(lpObj.X, lpObj.Y, lpObj.X, lpObj.Y) <= Distance)
	{
		while (true)
		{
			if (lpObj.VpPlayer2[count].state != 0)
			{
				tObjNum = lpObj.VpPlayer2[count].number;

				if (tObjNum >= 0)
				{
					EnableAttack = 0;

					if (lpObj.VpPlayer2[count].type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon < 0)
					{
						EnableAttack = 1;
					}
					else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
					{
						EnableAttack = 1;
					}

					else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && gGameObjects[tObjNum]->Type != OBJ_NPC &&
						gGameObjects[tObjNum]->MapNumber == MAP_INDEX_CASTLESIEGE &&
						g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
					{
						EnableAttack = 1;
					}

					else
					{
						int CallMonIndex = gGameObjects[tObjNum]->m_Index;

						if (gGameObjects[tObjNum]->Type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon >= 0)
						{
							CallMonIndex = gGameObjects[gGameObjects[tObjNum]->m_RecallMon]->m_Index;
						}

						if (gObjTargetGuildWarCheck(lpObj, &gGameObjects[CallMonIndex]) == 1)
						{
							EnableAttack = 1;
						}
					}
					if (EnableAttack)
					{
						if (gGameObjects[tObjNum]->X >= x - 2)
						{
							if (gGameObjects[tObjNum]->X <= x + 2)
							{
								if (gGameObjects[tObjNum]->Y >= y - 2)
								{
									if (gGameObjects[tObjNum]->Y <= y + 2)
									{
										gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, 1000, lpMagic->m_Skill, 0);
									}

								}
							}
						}
					}
				}
			}
			count++;
			if (count > MAX_VIEWPORT - 1)
				break;
		}
	}
	return 1;
}
int CObjUseSkill::SkillSummonerSleep(CGameObject &lpObj, int aTargetIndex, CMagicInf *lpMagic)
{
	 //loc2

	int result = 0; //loc3 ebpC

	int tObjNum; //loc4 -> ebp-10

	int angle = this->GetAngle(lpObj.X, lpObj.Y, gGameObjects[aTargetIndex]->X, gGameObjects[aTargetIndex]->Y); //loc5 ebp14

	int count = 0; //loc6 ebp18

	int DuelIndex; //loc7 ebp1c

	BOOL bEnableAttack; //loc8 ebp20

	GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, 1);

	DuelIndex = lpObj.m_iDuelUser;

	while (true)
	{
		if (lpObj.VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				bEnableAttack = FALSE; //ebp20

				if (lpObj.VpPlayer2[count].type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon < 0)
				{
					bEnableAttack = TRUE;
				}
				else if (tObjNum == aTargetIndex && DuelIndex == tObjNum)
				{
					bEnableAttack = TRUE;
				}
				else if (CC_MAP_RANGE(lpObj.MapNumber) != FALSE || IT_MAP_RANGE(lpObj.MapNumber) != FALSE || lpObj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
				{
					bEnableAttack = TRUE;
				}

				else if (g_ArcaBattle.IsArcaBattleServer() == TRUE)
				{
					bEnableAttack = TRUE;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && lpObj.VpPlayer2[count].type != OBJ_NPC &&
					gGameObjects[tObjNum]->MapNumber == MAP_INDEX_CASTLESIEGE &&
					g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					bEnableAttack = TRUE;
				}

				else
				{
					int CallMonIndex = gGameObjects[tObjNum]->m_Index;

					if (gGameObjects[tObjNum]->Type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon >= 0)
					{
						CallMonIndex = gGameObjects[gGameObjects[tObjNum]->m_RecallMon]->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &gGameObjects[CallMonIndex]) == TRUE)
					{
						bEnableAttack = TRUE;
					}

					if (gGameObjects[tObjNum]->Class >= 678 && gGameObjects[tObjNum]->Class <= 680)
					{
						bEnableAttack = TRUE;
					}
				}

				if (bEnableAttack != FALSE)
				{
					if (tObjNum == aTargetIndex)
					{
						if (gCheckSkillDistance(aIndex, aTargetIndex, lpMagic->m_Skill) != FALSE)
						{
							gObjAttack(lpObj, &gGameObjects[aTargetIndex], lpMagic, 1, 0, 0, 0, 0, 0);
							result = 1;
							break;
						}
					}
				}
			}
		}

		count++;

		if (count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}

	return result;
}
int CObjUseSkill::SkillDrainLife(CGameObject &lpObj, CMagicInf * lpMagic, int aTargetIndex)
{
	
	GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, 1);
	gObjAddAttackProcMsgSendDelay(lpObj, 50, aTargetIndex, 700, lpMagic->m_Skill, 0);
	return true;
}

int CObjUseSkill::SkillDamageReflect(CGameObject &lpObj, CMagicInf *lpMagic, int aTargetIndex)
{
	if (!ObjectMaxRange(aIndex))
	{
		return 0;
	}

	

	int Index = aIndex;

	if (ObjectMaxRange(aTargetIndex))
	{
		Index = aTargetIndex;
	}

	int RefDmg = 0;
	int Time = 0;

	if (lpObj.m_PlayerData->ISBOT == true)
	{
		Time = g_BotSystem.GetSkillTime(aIndex, lpMagic->m_Skill);
	}

	this->m_Lua.Generic_Call("SummonerDamageReflect", "i>ii", (lpObj.m_PlayerData->Energy + lpObj.AddEnergy), &RefDmg, &Time);

	gObjAddBuffEffect(&gGameObjects[Index], BUFFTYPE_DAMAGE_REFLECT, EFFECTTYPE_DAMAGEREFLECT, RefDmg, 0, 0, Time);
	GSProtocol.GCMagicAttackNumberSend(&gGameObjects[aIndex], lpMagic->m_Skill, Index, 1);

	return TRUE;
}
int CObjUseSkill::SkillBookNeil(CGameObject &lpObj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex)
{
	if (!ObjectMaxRange(aTargetIndex))
	{
		return 1;
	}
	int count = 0;
	int tObjNum;
	int EnableAttack;
	int DuelIndex = lpObj.m_iDuelUser;

	
	CGameObject lpTargetObj = &gGameObjects[aTargetIndex];
	int angle = this->GetAngle(lpObj.X, lpObj.Y, lpTargetObj.X, lpTargetObj.Y);
	GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_NEIL, aTargetIndex, 1);
	int Distance = MagicDamageC.GetSkillDistance(lpMagic->m_Skill);

	if (this->CalDistance(lpObj.X, lpObj.Y, lpObj.X, lpObj.Y) <= Distance)
	{
		while (true)
		{
			if (lpObj.VpPlayer2[count].state != 0)
			{
				tObjNum = lpObj.VpPlayer2[count].number;

				if (tObjNum >= 0)
				{
					EnableAttack = 0;

					if (lpObj.VpPlayer2[count].type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon < 0)
					{
						EnableAttack = 1;
					}
					else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
					{
						EnableAttack = 1;
					}

					else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && gGameObjects[tObjNum]->Type != OBJ_NPC &&
						gGameObjects[tObjNum]->MapNumber == MAP_INDEX_CASTLESIEGE &&
						g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
					{
						EnableAttack = 1;
					}

					else
					{
						int CallMonIndex = gGameObjects[tObjNum]->m_Index;

						if (gGameObjects[tObjNum]->Type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon >= 0)
						{
							CallMonIndex = gGameObjects[gGameObjects[tObjNum]->m_RecallMon]->m_Index;
						}

						if (gObjTargetGuildWarCheck(lpObj, &gGameObjects[CallMonIndex]) == 1)
						{
							EnableAttack = 1;
						}
					}
					if (EnableAttack)
					{
						if (gGameObjects[tObjNum]->X >= x - 2)
						{
							if (gGameObjects[tObjNum]->X <= x + 2)
							{
								if (gGameObjects[tObjNum]->Y >= y - 2)
								{
									if (gGameObjects[tObjNum]->Y <= y + 2)
									{
										gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, 1000, lpMagic->m_Skill, 0);
									}

								}
							}
						}
					}
				}
			}
			count++;
			if (count > MAX_VIEWPORT - 1)
				break;
		}
	}
	return 1;
}
int CObjUseSkill::SkillInnovation(CGameObject &lpObj, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex)
{
	
	int EnableAttack;
	int count = 0;
	int tObjNum;
	int attacked = 0;
	int DuelIndex = lpObj.m_iDuelUser;
	GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, 1);
	while (true)
	{
		if (lpObj.VpPlayer2[count].state != 0)
		{
			EnableAttack = 0;

			tObjNum = lpObj.VpPlayer2[count].number;
			if (lpObj.VpPlayer2[count].type != OBJ_MONSTER || gGameObjects[tObjNum]->m_RecallMon > 0)
			{
				if (tObjNum == DuelIndex)
				{
					EnableAttack = 1;
				}
				else if (CC_MAP_RANGE(lpObj.MapNumber) || lpObj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
				{
					EnableAttack = 1;
				}
				else if (IT_MAP_RANGE(lpObj.MapNumber))
				{
					EnableAttack = 1;
				}
				else if (g_ArcaBattle.IsArcaBattleServer() == TRUE)
				{
					EnableAttack = 1;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && gGameObjects[tObjNum]->Type != OBJ_NPC &&
					gGameObjects[tObjNum]->MapNumber == MAP_INDEX_CASTLESIEGE &&
					g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					if (g_CastleSiege.m_bCastleSiegeFriendlyFire == false &&
						lpObj.m_btCsJoinSide == gGameObjects[tObjNum]->m_btCsJoinSide)
					{
						EnableAttack = 0;
					}

					else
					{
						EnableAttack = 1;
					}
				}
			}
			else if (gGameObjects[tObjNum]->Type == OBJ_MONSTER)
			{
				if (gGameObjects[tObjNum]->m_RecallMon >= 0)
				{
					if (gObjTargetGuildWarCheck(lpObj, &gGameObjects[tObjNum]))
					{
						EnableAttack = 1;
					}

					if (gGameObjects[tObjNum]->Class >= 678 && gGameObjects[tObjNum]->Class <= 680)
					{
						EnableAttack = TRUE;
					}
				}
				else
				{
					EnableAttack = 1;
				}
			}
			if (EnableAttack)
			{
				if (gObjCalDistance(lpObj, &gGameObjects[tObjNum]) <= 6)
				{
					gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, 700, lpMagic->m_Skill, 0);
					++attacked;
					if (attacked >= 5)
						break;
				}
			}
		}
		++count;
		if (count >= MAX_VIEWPORT - 1)
			break;
	}
	return 1;
}
int CObjUseSkill::SkillWeakness(CGameObject &lpObj, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex)
{
	
	int EnableAttack;
	int count = 0;
	int tObjNum;
	int attacked = 0;
	int DuelIndex = lpObj.m_iDuelUser;
	GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, 1);
	while (true)
	{
		if (lpObj.VpPlayer2[count].state != 0)
		{
			EnableAttack = 0;
			tObjNum = lpObj.VpPlayer2[count].number;
			if (lpObj.VpPlayer2[count].type != OBJ_MONSTER || gGameObjects[tObjNum]->m_RecallMon > 0)
			{
				if (tObjNum == DuelIndex)
				{
					EnableAttack = 1;
				}
				else if (CC_MAP_RANGE(lpObj.MapNumber) || lpObj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
				{
					EnableAttack = 1;
				}
				else if (IT_MAP_RANGE(lpObj.MapNumber))
				{
					EnableAttack = 1;
				}
				else if (g_ArcaBattle.IsArcaBattleServer() == TRUE)
				{
					EnableAttack = 1;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && gGameObjects[tObjNum]->Type != OBJ_NPC &&
					gGameObjects[tObjNum]->MapNumber == MAP_INDEX_CASTLESIEGE &&
					g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					if (g_CastleSiege.m_bCastleSiegeFriendlyFire == false &&
						lpObj.m_btCsJoinSide == gGameObjects[tObjNum]->m_btCsJoinSide)
					{
						EnableAttack = 0;
					}

					else
					{
						EnableAttack = 1;
					}
				}

			}
			else if (gGameObjects[tObjNum]->Type == OBJ_MONSTER)
			{
				if (gGameObjects[tObjNum]->m_RecallMon >= 0)
				{
					if (gObjTargetGuildWarCheck(lpObj, &gGameObjects[tObjNum]))
					{
						EnableAttack = 1;
					}

					if (gGameObjects[tObjNum]->Class >= 678 && gGameObjects[tObjNum]->Class <= 680)
					{
						EnableAttack = TRUE;
					}
				}

				else
				{
					EnableAttack = 1;
				}
			}
			if (EnableAttack)
			{
				if (gObjCalDistance(lpObj, &gGameObjects[tObjNum]) <= 6)
				{
					gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, 700, lpMagic->m_Skill, 0);
					++attacked;
					if (attacked >= 5)
						break;
				}
			}
		}
		++count;
		if (count >= MAX_VIEWPORT - 1)
			break;
	}
	return 1;
}

int CObjUseSkill::SkillChainLighting(CGameObject &lpObj, CMagicInf *lpMagic, int aTargetIndex)
{
	if (!ObjectMaxRange(aIndex))
	{
		return FALSE;
	}

	if (!ObjectMaxRange(aTargetIndex))
	{
		return FALSE;
	}

	
	CGameObject lpTargetObj = &gGameObjects[aTargetIndex];

	BOOL EnableAttack = FALSE;
	int count = 0;
	int DuelIndex = lpObj.m_iDuelUser;
	int nChainTarget[3];

	nChainTarget[0] = -1;
	nChainTarget[1] = -1;
	nChainTarget[2] = -1;

	GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, 1);

	if (!gCheckSkillDistance(aIndex, aTargetIndex, lpMagic->m_Skill))
	{
		return 0;
	}

	nChainTarget[0] = aTargetIndex;

	while (true)
	{
		if (lpObj.VpPlayer2[count].state != 0)
		{
			int tObjNum = lpObj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = 0;

				if (lpObj.VpPlayer2[count].type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && gGameObjects[tObjNum]->Type != OBJ_NPC &&
					gGameObjects[tObjNum]->MapNumber == MAP_INDEX_CASTLESIEGE &&
					g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = 1;
				}

				else
				{
					int CallMonIndex = gGameObjects[tObjNum]->m_Index;

					if (gGameObjects[tObjNum]->Type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon >= 0)
					{
						CallMonIndex = gGameObjects[gGameObjects[tObjNum]->m_RecallMon]->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &gGameObjects[CallMonIndex]) == 1)
					{
						EnableAttack = 1;
					}

					if (gGameObjects[tObjNum]->Class >= 678 && gGameObjects[tObjNum]->Class <= 680)
					{
						EnableAttack = 1;
					}
				}

				if (EnableAttack == TRUE)
				{
					if (gGameObjects[tObjNum]->X >= gGameObjects[aTargetIndex]->X - 1 && gGameObjects[tObjNum]->X <= gGameObjects[aTargetIndex]->X + 1
						&& gGameObjects[tObjNum]->Y >= gGameObjects[aTargetIndex]->Y - 1 && gGameObjects[tObjNum]->Y <= gGameObjects[aTargetIndex]->Y + 1)
					{
						nChainTarget[1] = tObjNum;

						if (nChainTarget[2] != -1)
							break;

					}
					else if (gGameObjects[tObjNum]->X >= gGameObjects[aTargetIndex]->X - 2 && gGameObjects[tObjNum]->X <= gGameObjects[aTargetIndex]->X + 2
						&& gGameObjects[tObjNum]->Y >= gGameObjects[aTargetIndex]->Y - 2 && gGameObjects[tObjNum]->Y <= gGameObjects[aTargetIndex]->Y + 2)
					{
						nChainTarget[2] = tObjNum;

						if (nChainTarget[1] != -1)
							break;
					}
				}
			}
		}

		count++;
		if (count > MAX_VIEWPORT - 1)
			break;
	}

	if (nChainTarget[2] == -1)
		nChainTarget[2] = nChainTarget[1];

	if (nChainTarget[1] == -1)
	{
		nChainTarget[1] = nChainTarget[0];
		nChainTarget[2] = nChainTarget[0];
	}

	PMSG_CHAIN_MAGIC pMsg;
	pMsg.MagicNumberH = HIBYTE(lpMagic->m_Skill);
	pMsg.MagicNumberL = LOBYTE(lpMagic->m_Skill);
	pMsg.wUserIndex = aIndex;
	pMsg.nCount = 3;

	char SendByte[256];
	int nOffset = sizeof(PMSG_CHAIN_MAGIC);

	PMSG_CHAIN_MAGIC_OBJECT pMagicObj;

	for (int i = 0; i < 3; i++)
	{
		pMagicObj.nTargetIndex = nChainTarget[i];

		memcpy(&SendByte[nOffset], &pMagicObj, sizeof(PMSG_CHAIN_MAGIC_OBJECT));
		nOffset += sizeof(PMSG_CHAIN_MAGIC_OBJECT);
	}

	PHeadSubSetB((LPBYTE)&pMsg, 0xBF, 0x0A, nOffset);
	memcpy(&SendByte, &pMsg, sizeof(pMsg));

	if (lpObj.Type == OBJ_USER)
	{
		IOCP.DataSend(lpObj.m_Index, (LPBYTE)SendByte, nOffset);
	}

	GSProtocol.MsgSendV2(lpObj, (LPBYTE)SendByte, nOffset);

	for (int i = 0; i < 3; i++)
	{
		gObjAddAttackProcMsgSendDelay(lpObj, 50, nChainTarget[i], 200 * i + 200, lpMagic->m_Skill, i + 1);
	}

	return 1;

}

int CObjUseSkill::SkillBlowOfDestruction(CGameObject &lpObj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex, BOOL bCombo)
{
	if (ObjectMaxRange(aIndex) == 0)
	{
		return 0;
	}
	
	BOOL EnableAttack;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj.VpPlayer2[n].state <= 0)
		{
			continue;
		}
		EnableAttack = FALSE;
		int index = lpObj.VpPlayer2[n].number;

		if (gGameObjects[index]->Type == OBJ_MONSTER && gGameObjects[index]->m_RecallMon < 0)
		{
			EnableAttack = TRUE;
		}

		else if (aTargetIndex == index || index == lpObj.m_iDuelUser)
		{
			EnableAttack = TRUE;
		}

		else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && gGameObjects[index]->Type != OBJ_NPC &&
			gGameObjects[index]->MapNumber == MAP_INDEX_CASTLESIEGE &&
			g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
		{
			EnableAttack = TRUE;
		}

		int summon = aIndex;

		if (lpObj.Type == OBJ_MONSTER && lpObj.m_RecallMon >= 0)
		{
			summon = lpObj.m_RecallMon;
		}

		if (gObjTargetGuildWarCheck(&gGameObjects[summon], lpObj) != 0)
		{
			EnableAttack = TRUE;
		}

		if (this->CalDistance(x, y, gGameObjects[index]->X, gGameObjects[index]->Y) < 3 && EnableAttack)
		{
			gObjAddAttackProcMsgSendDelay(lpObj, 50, index, 400, lpMagic->m_Skill, bCombo);

			if (bCombo != 0)
			{
				GSProtocol.GCMagicAttackNumberSend(lpObj, 59, aIndex, 1);
			}
		}
	}

	return 1;
}

int CObjUseSkill::SkillAreaMonsterAttack(CGameObject &lpObj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex, int Distance, int MultiAttack, int AttackDelay)
{
	
	CGameObject lpTargetObj = &gGameObjects[aTargetIndex];

	int HitCount = 4;
	int MaxHitCount = 8;
	int CurrHitCount = 0;
	int count = 0;
	int tObjNum;
	int EnableAttack;
	int DuelIndex = lpObj.m_iDuelUser;

	if (lpMagic->m_Skill == AT_SKILL_GIGANTIC_STORM)
	{
		HitCount = 8;
		MaxHitCount = 12;
	}

	if (lpMagic->m_Skill == AT_SKILL_METEOR_STORM)
	{
		HitCount = 8;
		MaxHitCount = 12;
	}

	int iSkillDistance = MagicDamageC.GetSkillDistance(lpMagic->m_Skill);

	if (this->CalDistance(x, y, lpObj.X, lpObj.Y) <= iSkillDistance)
	{
		while (true)
		{
			if (lpObj.VpPlayer2[count].state != 0)
			{
				tObjNum = lpObj.VpPlayer2[count].number;
				if (tObjNum >= 0)
				{
					EnableAttack = 0;

					if (lpObj.VpPlayer2[count].type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon < 0)
					{
						EnableAttack = 1;
					}

					if ((tObjNum == aTargetIndex && MultiAttack == TRUE) || DuelIndex == tObjNum)
					{
						EnableAttack = 1;
					}

					else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && gGameObjects[tObjNum]->Type != OBJ_NPC &&
						gGameObjects[tObjNum]->MapNumber == MAP_INDEX_CASTLESIEGE &&
						g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
					{
						EnableAttack = 1;
					}

					if (lpObj.VpPlayer2[count].type == OBJ_NPC || lpObj.MapNumber != MAP_INDEX_CASTLESIEGE || g_CastleSiegeSync.GetCastleState() != 7)
					{
						int CallMonIndex = gGameObjects[tObjNum]->m_Index;

						if (gGameObjects[tObjNum]->Type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon >= 0)
						{
							CallMonIndex = gGameObjects[gGameObjects[tObjNum]->m_RecallMon]->m_Index;
						}

						if (gObjTargetGuildWarCheck(lpObj, &gGameObjects[CallMonIndex]) == 1)
						{
							EnableAttack = 1;
						}

						if (gGameObjects[tObjNum]->Class >= 678 && gGameObjects[tObjNum]->Class <= 680)
						{
							EnableAttack = TRUE;
						}
					}

					if (gGameObjects[tObjNum]->Live == FALSE)
					{
						EnableAttack = FALSE;
					}

					if (EnableAttack == TRUE)
					{
						if (gGameObjects[tObjNum]->X >= x - Distance)
						{
							if (gGameObjects[tObjNum]->X <= x + Distance)
							{
								if (gGameObjects[tObjNum]->Y >= y - Distance)
								{
									if (gGameObjects[tObjNum]->Y <= y + Distance)
									{
										gGameObjects[tObjNum]->lpAttackObj = lpObj;
										++CurrHitCount;

										BOOL Attack = FALSE;

										if (CurrHitCount <= HitCount)
										{
											Attack = TRUE;
										}

										else
										{
											Attack = rand() % 2;
										}

										if (CurrHitCount > MaxHitCount)
										{
											Attack = FALSE;
										}

										if (Attack)
										{
											if (AttackDelay)
											{
												if (lpMagic->m_Skill == AT_SKILL_GIGANTIC_STORM && lpObj.Class == 561 && gObjCheckUsedBuffEffect(&gGameObjects[tObjNum], BUFFTYPE_FREEZE) == FALSE)
												{
													gGameObjects[tObjNum]->DelayActionTime = 800;
													gGameObjects[tObjNum]->DelayLevel = 1;
													gGameObjects[tObjNum]->lpAttackObj = lpObj;
													gObjAddBuffEffect(&gGameObjects[tObjNum], BUFFTYPE_FREEZE, 0, 0, 0, 0, 3);
												}

												if ((lpMagic->m_Skill == AT_SKILL_METEOR_STORM) == FALSE)
												{
													gGameObjects[tObjNum]->DelayActionTime = 800;
													gGameObjects[tObjNum]->DelayLevel = 1;
													gGameObjects[tObjNum]->lpAttackObj = lpObj;
													//gObjAddBuffEffect(&gGameObjects[tObjNum], BUFFTYPE_FREEZE, 0, 0, 0, 0, 3);
												}

												gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, AttackDelay, lpMagic->m_Skill, 0);
											}
											else
											{
												if (lpMagic->m_Skill != AT_SKILL_DRAGON_ROAR && lpMagic->m_Skill != AT_SKILL_PHOENIX_SHOT && lpMagic->m_Skill != 560)
												{
													gObjAttack(lpObj, &gGameObjects[tObjNum], lpMagic, 1, 0, 0, 0, 0, 0);
												}

												else
												{
													int Dec = 0;
													BYTE btAttr = MapC[gGameObjects[tObjNum]->MapNumber]->GetAttr(gGameObjects[tObjNum]->X, gGameObjects[tObjNum]->Y);

													if ((btAttr & 1) != 1)
													{
														if (lpMagic->m_Skill == AT_SKILL_PHOENIX_SHOT && rand() % 100 < 10)
														{
															if (gGameObjects[tObjNum]->Type == OBJ_MONSTER)
															{
																Dec = 50;
															}

															else if (gGameObjects[tObjNum]->Type == OBJ_USER)
															{
																Dec = 10;
															}

															else
															{
																Dec = 0;
															}

															gObjAddBuffEffect(&gGameObjects[tObjNum], BUFFTYPE_MONK_DECREASE_DEFENSE_RATE, EFFECTTYPE_DECREASE_DEFENSE_RATE, Dec, 0, 0, 10);
														}
													}

													for (int i = 1; i <= 4; i++)
													{
														if (i == 1)
															gObjAttack(lpObj, &gGameObjects[tObjNum], lpMagic, 1, 1, 0, 0, 1, 0);
														else
															gObjAttack(lpObj, &gGameObjects[tObjNum], lpMagic, 0, 1, 0, 0, i, 0);
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
			++count;
			if (count > MAX_VIEWPORT - 1)
				break;
		}
	}
	return 1;
}

void CObjUseSkill::NewSkillProc(CGameObject &lpObj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex, int aMsgSubCode, BOOL bCombo)
{
	if (aMsgSubCode == 59)
	{
		GSProtocol.GCMagicAttackNumberSend(&gGameObjects[aIndex], lpMagic->m_Skill, aTargetIndex, 1);
	}

	if (lpMagic->m_Skill == AT_SKILL_RAGEUL)
	{
		this->SkillAreaMonsterAttack(aIndex, lpMagic, x, y, aTargetIndex, 3, 1, 0);
	}

	if (aMsgSubCode >= 59 && aMsgSubCode <= 64)
	{
		gObjAddMsgSendDelay(&gGameObjects[aIndex], 59, aTargetIndex, 1000, (MAKELONG(MAKEWORD(x, y), aMsgSubCode)));
	}
}

void CObjUseSkill::SkillLightingShock(CGameObject &lpObj, CMagicInf *lpMagic, int aTargetIndex)
{
	

	BOOL EnableAttack;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		EnableAttack = FALSE;

		if (lpObj.VpPlayer2[n].state <= 0)
		{
			continue;
		}

		int index = lpObj.VpPlayer2[n].number;

		if (!ObjectMaxRange(index))
		{
			continue;
		}

		if (gGameObjects[index]->Type == OBJ_MONSTER && gGameObjects[index]->m_RecallMon < 0)
		{
			EnableAttack = TRUE;
		}

		if (lpObj.Type == OBJ_MONSTER && gGameObjects[index]->Type == OBJ_USER) // DoppelGanger Fix
		{
			EnableAttack = TRUE;
		}

		if (aTargetIndex == index || index == lpObj.m_iDuelUser)
		{
			EnableAttack = TRUE;
		}

		if (CC_MAP_RANGE(lpObj.MapNumber) || IT_MAP_RANGE(lpObj.MapNumber) || lpObj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
		{
			EnableAttack = TRUE;
		}

		else if (g_ArcaBattle.IsArcaBattleServer() == TRUE)
		{
			EnableAttack = TRUE;
		}

		else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
		{
			if (gGameObjects[index]->Type != OBJ_NPC &&
				gGameObjects[index]->MapNumber == MAP_INDEX_CASTLESIEGE &&
				g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
			{
				EnableAttack = TRUE;
			}
		}

		int summon = aIndex;

		if (lpObj.Type == OBJ_MONSTER && lpObj.m_RecallMon >= 0)
		{
			summon = lpObj.m_RecallMon;
		}

		if (gObjTargetGuildWarCheck(&gGameObjects[summon], lpObj) != 0)
		{
			EnableAttack = TRUE;
		}

		if (gGameObjects[index]->Class >= 678 && gGameObjects[index]->Class <= 680)
		{
			EnableAttack = TRUE;
		}

		if (gObjCalDistance(lpObj, &gGameObjects[index]) <= 6 && EnableAttack == TRUE)
		{
			gObjAddAttackProcMsgSendDelay(lpObj, 50, index, 300, lpMagic->m_Skill, 0);
		}
	}
}

int CObjUseSkill::SkillMagicCircle(CGameObject &lpObj, int aTargetIndex, CMagicInf *lpMagic)
{
	if (lpObj.m_PlayerData->ISBOT == true)
	{
		gObjAddBuffEffect(&gGameObjects[lpObj.BuffPlayerIndex], BUFFTYPE_MAGIC_POWER_INC, EFFECTTYPE_MAGICPOWER_INC, 20, 0, 0, g_BotSystem.GetSkillTime(aIndex, lpMagic->m_Skill));
	}

	else
	{
		

		int iEffect = (lpObj.m_PlayerData->Energy + lpObj.AddEnergy) / 9 * 0.20;
		gObjAddBuffEffect(lpObj, BUFFTYPE_MAGIC_POWER_INC, EFFECTTYPE_MAGICPOWER_INC, iEffect, 0, 0, 1800);
	}

	GSProtocol.GCMagicAttackNumberSend(&gGameObjects[aIndex], lpMagic->m_Skill, aIndex, 1);
	return 1;
}

int CObjUseSkill::SkillShieldRefill(CGameObject &lpObj, CMagicInf *lpMagic, int aTargetIndex)
{
	int skillSuccess = true;
	
	CGameObject lpTargetObj = &gGameObjects[aTargetIndex];
	int skill_level = lpMagic->m_Level;

	if (lpTargetObj.Type != OBJ_USER && lpTargetObj.m_RecallMon == -1)
	{
		GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, 0);
		return 0;
	}

	if (lpObj.Class != CLASS_ELF)
	{
		GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, 0);
		return 0;
	}

	if ((CC_MAP_RANGE(lpObj.MapNumber) || lpObj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL) && lpObj.m_Index != lpTargetObj.m_Index)
	{
		GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, 0);
		return 0;
	}

	if (gObjDuelCheck(lpTargetObj) != 0) //nice trick
	{
		lpTargetObj = lpObj;
	}

	if (lpTargetObj.iShield < lpTargetObj.iAddShield + lpTargetObj.iMaxShield)
	{
		double skill_shield = 0.0; // lpObj.m_PlayerData->Energy / 4 + lpObj.Level;
		this->m_Lua.Generic_Call("ElfShieldRecovery", "ii>d", (int)(lpObj.m_PlayerData->Energy + lpObj.AddEnergy), (int)lpObj.Level, &skill_shield);

		lpTargetObj.iShield += skill_shield;
		if (lpTargetObj.iShield > lpTargetObj.iAddShield + lpTargetObj.iMaxShield)
		{
			lpTargetObj.iShield = lpTargetObj.iAddShield + lpTargetObj.iMaxShield;
		}
		if (lpTargetObj.Type == OBJ_USER)
		{
			GSProtocol.GCReFillSend(lpTargetObj.m_Index, lpTargetObj.Life, 0xFF, 0, lpTargetObj.iShield);
			GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_SD_RECOVERY, lpTargetObj.m_Index, 1);

			for (int n = 0; n < MAX_VIEWPORT; n++)
			{
				if (lpObj.VpPlayer2[n].type == OBJ_MONSTER && lpObj.VpPlayer2[n].state != 0)
				{
					CGameObject lpMonster = &gGameObjects[lpObj.VpPlayer2[n]->number];
					if (lpMonster->m_iCurrentAI != 0)
					{
						lpMonster->m_Agro->IncAgro(lpObj.m_Index, lpObj.m_PlayerData->Energy / 5 / 40);
					}
				}
			}
		}
	}
	else
	{
		GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_SD_RECOVERY, lpTargetObj.m_Index, 0);
	}

	return 1;
}

int CObjUseSkill::SkillGiganticStorm(CGameObject &lpObj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex)
{
	return this->SkillAreaMonsterAttack(aIndex, lpMagic, x, y, aTargetIndex, 6, 1, 400);
}

void CObjUseSkill::SkillFrustrum3(CGameObject &lpObj, float fangle, float tx1, float ty1, float tx2, float ty2)
{
	

	vec3_t p[4];
	Vector(-tx1, ty1, 0.f, p[0]);
	Vector(tx1, ty1, 0.f, p[1]);
	Vector(tx2, ty2, 0.f, p[2]);
	Vector(-tx2, ty2, 0.f, p[3]);

	vec3_t Angle;
	Vector(0.f, 0.f, fangle * 360 / (BYTE)255, Angle);

	float Matrix[3][4];
	vec3_t vFrustrum[4];

	AngleMatrix(Angle, Matrix);

	for (int i = 0; i < 4; i++)
	{
		VectorRotate(p[i], Matrix, vFrustrum[i]);

		lpObj.fSkillFrustrumX[i] = (int)vFrustrum[i][0] + lpObj.X;
		lpObj.fSkillFrustrumY[i] = (int)vFrustrum[i][1] + lpObj.Y;
	}
}

int CObjUseSkill::SkillFiveShot(CGameObject &lpObj, CMagicInf *lpMagic, BYTE TargetPos, int aTargetIndex)
{
	

	this->SkillFrustrum3(aIndex, TargetPos, 6.0f, 7.0f, 1.0f, 0); //S4 New Skill Frustrum OK :)

	vec3_t Angle; //Good
	Vector(0.f, 6.0f, 0.f, Angle); //3,4,5 Good

	vec3_t p[4]; //Good size OK

	Vector(0.f, 0.f, 40.0f, p[0]); //Good
	Vector(0.f, 0.f, 20.0f, p[1]); //Good
	Vector(0.f, 0.f, 340.0f, p[2]); //Good
	Vector(0.f, 0.f, 320.0f, p[3]); //Good

	float Matrix[3][4]; //Good

	vec3_t vFrustrum[5]; //Good

	AngleMatrix(p[0], Matrix); //Good
	VectorRotate(Angle, Matrix, vFrustrum[0]); //loc44 Good

	AngleMatrix(p[1], Matrix); //Good
	VectorRotate(Angle, Matrix, vFrustrum[1]); //loc41 Good

	Vector(0.f, 6.0f, 0.f, vFrustrum[2]);//36,37,38

	AngleMatrix(p[2], Matrix); //Good
	VectorRotate(Angle, Matrix, vFrustrum[3]); //loc35 Good

	AngleMatrix(p[3], Matrix); //Good
	VectorRotate(Angle, Matrix, vFrustrum[4]); //loc32 Good

	Vector(0.f, 0.f, (TargetPos * 360 / (BYTE)255), p[0]); //15, 16, 17 Good

	AngleMatrix((float*)p, Matrix); //Good

///////////////////////////////////////////////////////////////////////////////////////////////////
	int Value1[5];
	int Value2[5];

	vec3_t vFrustrum2[5];

	for (int i = 0; i < 5; i++) //loc70 Good
	{
		VectorRotate(vFrustrum[i], Matrix, vFrustrum2[i]);

		Value1[i] = (int)vFrustrum2[i][0] + lpObj.X;
		Value2[i] = (int)vFrustrum2[i][1] + lpObj.Y;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////

	int tObjNum; //loc71
	int count = 0; //loc72
	int loc73 = 0; //loc73
	int DuelIndex = lpObj.m_iDuelUser; //loc74
	BOOL bEnableAttack; //loc75

	while (true)
	{
		if (lpObj.VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				bEnableAttack = FALSE;

				if (lpObj.VpPlayer2[count].type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon < 0)
				{
					bEnableAttack = TRUE;
				}
				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					bEnableAttack = TRUE;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && lpObj.VpPlayer2[count].type != OBJ_NPC &&
					gGameObjects[tObjNum]->MapNumber == MAP_INDEX_CASTLESIEGE &&
					g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					bEnableAttack = TRUE;
				}

				else
				{
					int CallMonIndex = gGameObjects[tObjNum]->m_Index;

					if (gGameObjects[tObjNum]->Type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon >= 0)
					{
						CallMonIndex = gGameObjects[gGameObjects[tObjNum]->m_RecallMon]->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &gGameObjects[CallMonIndex]) == TRUE)
					{
						bEnableAttack = TRUE;
					}

					if (gGameObjects[tObjNum]->Class >= 678 && gGameObjects[tObjNum]->Class <= 680)
					{
						bEnableAttack = TRUE;
					}
				}

				if (bEnableAttack != FALSE)
				{
					int Value3[5];

					for (int j = 0; j < 5; j++)
					{
						Value3[j] = (((int)Value1[j] - lpObj.X) * (gGameObjects[tObjNum]->Y - lpObj.Y)) - (((int)Value2[j] - lpObj.Y) * (gGameObjects[tObjNum]->X - lpObj.X));

						if (SkillTestFrustrum(gGameObjects[tObjNum]->X, gGameObjects[tObjNum]->Y, aIndex))
						{
							if (Value3[j] > -5 && Value3[j] < 5)
							{
								gObjAttack(lpObj, &gGameObjects[tObjNum], lpMagic, 1, 0, 0, 0, 0, 0);
							}
						}
					}
				}
			}
		}

		count++;

		if (count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}

	return 1;
}

int CObjUseSkill::SkillBirds(CGameObject &lpObj, CMagicInf *lpMagic, BYTE TargetPos, int aTargetIndex)
{
	int count = 0;
	int tObjNum;
	int EnableAttack = 0;
	int HitCount = 0;
	this->SkillFrustrum3(aIndex, TargetPos, 1.5, 6.0, 1.5, 0.0);
	
	int DuelIndex = lpObj.m_iDuelUser;
	while (true)
	{
		if (lpObj.VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj.VpPlayer2[count].number;
			EnableAttack = 0;
			if (tObjNum >= 0)
			{
				if (lpObj.VpPlayer2[count].type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
				else if (lpObj.VpPlayer2[count].type == OBJ_USER && lpObj.Class == 561)
				{
					EnableAttack = 1;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && lpObj.VpPlayer2[count].type != OBJ_NPC &&
					gGameObjects[tObjNum]->MapNumber == MAP_INDEX_CASTLESIEGE &&
					g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = TRUE;
				}

				else
				{
					int CallMonIndex = gGameObjects[tObjNum]->m_Index;

					if (gGameObjects[tObjNum]->Type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon >= 0)
					{
						CallMonIndex = gGameObjects[gGameObjects[tObjNum]->m_RecallMon]->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &gGameObjects[CallMonIndex]) == TRUE)
					{
						EnableAttack = TRUE;
					}

					if (gGameObjects[tObjNum]->Class >= 678 && gGameObjects[tObjNum]->Class <= 680)
					{
						EnableAttack = TRUE;
					}
				}
			}
			if (EnableAttack)
			{
				if (SkillTestFrustrum(gGameObjects[tObjNum]->X, gGameObjects[tObjNum]->Y, aIndex) == TRUE)
				{
					int Attack = 0;
					if (HitCount < 8)
					{
						Attack = 1;
					}
					else
					{
						Attack = rand() % 2;
					}
					if (Attack)
					{
						gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, 200, lpMagic->m_Skill, 0);
					}
					if (ObjectMaxRange(aTargetIndex))
					{
						if (tObjNum == aTargetIndex)
						{
							gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, 300, lpMagic->m_Skill, 0);
						}
					}
				}
			}
		}
		++count;
		if (count > MAX_VIEWPORT - 1)
			break;
	}
	return 1;
}

int CObjUseSkill::SkillFlameStrike(CGameObject &lpObj, CMagicInf *lpMagic, BYTE TargetPos, int aTargetIndex)
{
	int count = 0;
	int tObjNum;
	int EnableAttack = 0;
	int HitCount = 0;
	this->SkillFrustrum3(aIndex, TargetPos, 2.0, 4.0, 5.0, 0.0);
	
	int DuelIndex = lpObj.m_iDuelUser;
	while (true)
	{
		if (lpObj.VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj.VpPlayer2[count].number;
			EnableAttack = 0;
			if (tObjNum >= 0)
			{
				if (lpObj.VpPlayer2[count].type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon < 0)
				{
					EnableAttack = 1;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && lpObj.VpPlayer2[count].type != OBJ_NPC &&
					gGameObjects[tObjNum]->MapNumber == MAP_INDEX_CASTLESIEGE &&
					g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = TRUE;
				}

				int CallMonIndex = gGameObjects[tObjNum]->m_Index;

				if (gGameObjects[tObjNum]->Type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon >= 0)
				{
					CallMonIndex = gGameObjects[gGameObjects[tObjNum]->m_RecallMon]->m_Index;
				}

				if (gObjTargetGuildWarCheck(lpObj, &gGameObjects[CallMonIndex]) == 1)
				{
					EnableAttack = 1;
				}

				if (gGameObjects[tObjNum]->Class >= 678 && gGameObjects[tObjNum]->Class <= 680)
				{
					EnableAttack = TRUE;
				}

				if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
			}
			if (EnableAttack)
			{
				if (SkillTestFrustrum(gGameObjects[tObjNum]->X, gGameObjects[tObjNum]->Y, aIndex) == TRUE &&
					gObjCalDistance(lpObj, &gGameObjects[tObjNum]) <= MagicDamageC.GetSkillDistance(lpMagic->m_Skill))
				{
					int Attack = 0;
					if (HitCount < 8)
					{
						Attack = 1;
					}
					else
					{
						Attack = rand() % 2;
					}
					if (Attack)
					{
						gObjAttack(lpObj, &gGameObjects[tObjNum], lpMagic, 1, 0, 0, 0, 0, 0);
						gObjAttack(lpObj, &gGameObjects[tObjNum], lpMagic, 1, 0, 0, 0, 0, 0);
					}
				}
			}
		}
		++count;
		if (count > MAX_VIEWPORT - 1)
			break;
	}
	return 1;
}

int CObjUseSkill::SkillBerserker(CGameObject &lpObj, CMagicInf *lpMagic, int aTargetIndex)
{
	if (!ObjectMaxRange(aIndex))
		return 0;

	
	CGameObject lpTargetObj = lpObj;

	if (lpObj.Type == OBJ_USER && lpObj.Class != CLASS_SUMMONER && lpObj.m_PlayerData->ISBOT == false)
		return 0;

	if (lpObj.Type == OBJ_USER && lpObj.m_PlayerData->ISBOT == true && lpObj.BuffPlayerIndex != -1)
	{
		lpTargetObj = &gGameObjects[lpObj.BuffPlayerIndex];
	}

	int EffectUPValue = 0;
	int EffectDownValue = 0;
	int Time = 0;

	if (lpObj.Type == OBJ_USER)
	{
		this->m_Lua.Generic_Call("SummonerBerserker", "i>iii", (lpObj.m_PlayerData->Energy + lpObj.AddEnergy), &EffectUPValue, &EffectDownValue, &Time);
	}

	if (lpObj.Type == OBJ_USER && lpObj.m_PlayerData->ISBOT == true)
	{
		Time = g_BotSystem.GetSkillTime(aIndex, lpMagic->m_Skill);
	}

	if (lpObj.Type != OBJ_USER)
	{
		Time = 600000;
		EffectUPValue = 0;
		EffectDownValue = 0;
	}

	gObjAddBuffEffect(lpTargetObj, BUFFTYPE_BERSERKER, EFFECTTYPE_BERSERKER_UP, EffectUPValue, EFFECTTYPE_BERSERKER_DOWN, EffectDownValue, Time);
	GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, lpTargetObj.m_Index, 1);

	return 1;
}

int CObjUseSkill::SkillPhoenixShot(CGameObject &lpObj, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex)
{
	
	CGameObject lpTargetObj = &gGameObjects[aIndex];
	BOOL EnableAttack;
	int attackcheck;
	int loopcount = 0;
	int delaytime = 0;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj.VpPlayer2[n].state <= 0)
		{
			continue;
		}
		EnableAttack = FALSE;
		int index = lpObj.VpPlayer2[n].number;

		if (gGameObjects[index]->Type == OBJ_MONSTER && gGameObjects[index]->m_RecallMon < 0)
		{
			EnableAttack = TRUE;
		}

		if (aTargetIndex == index || index == lpObj.m_iDuelUser)
		{
			EnableAttack = TRUE;
		}

		else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && gGameObjects[index]->Type != OBJ_NPC &&
			gGameObjects[index]->MapNumber == MAP_INDEX_CASTLESIEGE &&
			g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
		{
			EnableAttack = TRUE;
		}

		int summon = aIndex;

		if (lpObj.Type == OBJ_MONSTER && lpObj.m_RecallMon >= 0)
		{
			summon = lpObj.m_RecallMon;
		}

		if (gObjTargetGuildWarCheck(&gGameObjects[summon], lpObj) != 0)
		{
			EnableAttack = TRUE;
		}

		if (EnableAttack != 0)
		{
			attackcheck = 0;

			if (loopcount == 0)
			{
				attackcheck = 1;
			}
			else if (rand() % 3 == 0)
			{
				attackcheck = 1;
			}

			if (attackcheck != 0)
			{
				if (gObjCalDistance(&gGameObjects[aTargetIndex], &gGameObjects[index]) < 3)
				{
					delaytime = (rand() * 17) % 300 + 300;

					gObjAddAttackProcMsgSendDelay(lpObj, 53, index, delaytime, lpMagic->m_Skill, 0);
					gObjAddAttackProcMsgSendDelay(lpObj, 50, index, delaytime, lpMagic->m_Skill, 0);
					gObjAddAttackProcMsgSendDelay(lpObj, 50, index, delaytime + 100, lpMagic->m_Skill, 0);
					gObjAddAttackProcMsgSendDelay(lpObj, 50, index, delaytime + 100, lpMagic->m_Skill, 0);
				}
			}
		}
	}
	return 1;
}

int CObjUseSkill::SkillEvilMonster(CGameObject &lpObj, int aTargetIndex, CMagicInf *lpMagic)
{
	

	int tObjNum;
	int count = 0;
	int attackcount = 0;
	int EnableAttack = FALSE;

	while (true)
	{
		tObjNum = lpObj.VpPlayer2[count].number;
		EnableAttack = FALSE;

		if (tObjNum >= 0 && lpObj.VpPlayer2[count].type == OBJ_USER)
		{
			EnableAttack = TRUE;
			if (gGameObjects[tObjNum]->TargetNumber >= 0)
			{
				if (tObjNum != aTargetIndex)
					EnableAttack = FALSE;
			}

			if (EnableAttack == TRUE && gObjCalDistance(lpObj, &gGameObjects[tObjNum]) < 6)
			{
				BOOL Attack = attackcount < 5;
				if (tObjNum == aTargetIndex)
					Attack = TRUE;

				if (Attack)
				{
					gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, 300, lpMagic->m_Skill, 0);
					attackcount++;

					if (gObjCheckUsedBuffEffect(&gGameObjects[tObjNum], BUFFTYPE_POISON) == FALSE)
					{
						if (!retResistance(&gGameObjects[tObjNum], 1))
						{
							gGameObjects[tObjNum]->lpAttackObj = lpObj;
							gObjAddBuffEffect(&gGameObjects[tObjNum], BUFFTYPE_POISON, EFFECTTYPE_POISON_DMG_TICK, 3, 0, 0, 20);
						}
					}
				}
			}
		}

		count++;
		if (count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}

	return TRUE;
}

int CObjUseSkill::SkillMonkBuff(CGameObject &lpObj, CMagicInf * lpMagic)
{
	

	if (lpObj.Type != OBJ_USER)
	{
		return 0;
	}

	if (lpObj.Class != CLASS_RAGEFIGHTER)
	{
		return 0;
	}

	if (lpMagic == NULL)
	{
		return 0;
	}

	if (lpMagic->m_Skill != AT_SKILL_REDARMOR_IGNORE)
	{
		return 0;
	}

	int Ignore = ((lpObj.m_PlayerData->Energy + lpObj.AddEnergy) - 404) / 100.0 + 3.0;

	if (Ignore > 10)
		Ignore = 10;

	gObjAddBuffEffect(lpObj, BUFFTYPE_MONK_IGNORE_ENEMY_DEFENSE, EFFECTTYPE_IGNORE_OPPONENT_DEFENSE, Ignore, 0, 0, lpObj.m_PlayerData->Energy / 5 + 60);
	GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, lpObj.m_Index, TRUE);

	return 1;
}

int CObjUseSkill::SkillMonkBuffApplyParty(CGameObject &lpObj, CMagicInf * lpMagic)
{
	if (ObjectMaxRange(aIndex) == FALSE)
	{
		return FALSE;
	}

	

	if (lpObj.Type != OBJ_USER && lpObj.m_RecallMon == -1)
	{
		return FALSE;
	}

	if (lpObj.Class != CLASS_RAGEFIGHTER)
	{
		return FALSE;
	}

	int partynum = 0;
	int partyindex[5];

	memset(partyindex, -1, sizeof(partyindex));

	partynum = lpObj.PartyNumber;

	if (partynum != -1)
	{
		for (int i = 0; i < MAX_USER_IN_PARTY; i++)
		{
			int index = gParty.m_PartyS[partynum].Number[i];

			if (index >= 0 && index < g_ConfigRead.server.GetObjectMax())
			{
				CGameObject lpPartyObj = &gGameObjects[index];

				if (lpPartyObj.MapNumber == lpObj.MapNumber)
				{
					if (gObjCalDistance(lpObj, lpPartyObj) < 4)
					{
						partyindex[i] = index;
					}
				}
			}
		}
	}

	int skill_improve = 0;

	BYTE BuffIndex = BUFFTYPE_NONE;

	switch (lpMagic->m_Skill)
	{
	case AT_SKILL_FITNESS:
		BuffIndex = BUFFTYPE_MONK_INCREASE_HEALTH;
		break;
	case AT_SKILL_DSRINCREASE:
		BuffIndex = BUFFTYPE_MONK_INCREASE_DEFENSE_RATE;
		break;
	default:
		return FALSE;
	}

	if (lpMagic->m_Skill == AT_SKILL_FITNESS)
	{
		skill_improve = (((lpObj.m_PlayerData->Energy + lpObj.AddEnergy) - 132) / 10.0 + 30.0);
	}

	else if (lpMagic->m_Skill == AT_SKILL_DSRINCREASE)
	{
		skill_improve = (((lpObj.m_PlayerData->Energy + lpObj.AddEnergy) - 80) / 10.0 + 10.0);

		if (skill_improve > 100)
			skill_improve = 100;
	}

	else
	{
		return FALSE;
	}

	if (partynum == -1)
	{
		if (lpMagic->m_Skill == AT_SKILL_FITNESS)
		{
			gObjAddBuffEffect(lpObj, BuffIndex, EFFECTTYPE_MONK_VITALITY, skill_improve, 0, 0, lpObj.m_PlayerData->Energy / 5 + 60);
			GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aIndex, TRUE);
			gObjCalCharacter.CalcCharacter(aIndex);
		}

		else if (lpMagic->m_Skill == AT_SKILL_DSRINCREASE)
		{
			gObjAddBuffEffect(lpObj, BuffIndex, EFFECTTYPE_IMPROVE_DEFENSE_RATE, skill_improve, 0, 0, lpObj.m_PlayerData->Energy / 5 + 60);
			GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aIndex, TRUE);
		}
	}

	else
	{
		for (int j = 0; j < 5; j++)
		{
			if (partyindex[j] != -1)
			{
				CGameObject lpPartyObj = &gGameObjects[partyindex[j]];

				if (lpMagic->m_Skill == AT_SKILL_FITNESS)
				{
					gObjAddBuffEffect(lpPartyObj, BuffIndex, EFFECTTYPE_MONK_VITALITY, skill_improve, 0, 0, lpObj.m_PlayerData->Energy / 5 + 60);
					GSProtocol.GCMagicAttackNumberSend(lpPartyObj, lpMagic->m_Skill, partyindex[j], TRUE);
					gObjCalCharacter.CalcCharacter(partyindex[j]);
				}

				else
				{
					gObjAddBuffEffect(lpPartyObj, BuffIndex, EFFECTTYPE_IMPROVE_DEFENSE_RATE, skill_improve, 0, 0, lpObj.m_PlayerData->Energy / 5 + 60);
					GSProtocol.GCMagicAttackNumberSend(lpPartyObj, lpMagic->m_Skill, partyindex[j], TRUE);
				}
			}
		}
	}

	return TRUE;
}

int CObjUseSkill::SkillMonkBarrageJustOneTarget(CGameObject &lpObj, CMagicInf * lpMagic, int aTargetIndex)
{
	

	if (ObjectMaxRange(aTargetIndex) == FALSE)
	{
		sLog->outBasic("[InvalidTargetIndex][SkillMonkBarrageJustOneTarget] Index :%d , AccountID : %s", aIndex, lpObj.AccountID);
		return FALSE;
	}

	CGameObject lpTargetObj = &gGameObjects[aTargetIndex];

	BOOL EnableAttack = FALSE;
	int count = 0;
	int tObjNum;
	int CallMonIndex;
	int HitCount = 0;

	int nDistance = MagicDamageC.GetSkillDistance(lpMagic->m_Skill);

	while (true)
	{
		if (lpObj.VpPlayer2[count].state == TRUE)
		{
			tObjNum = lpObj.VpPlayer2[count].number;
			if (tObjNum >= 0)
			{
				if (tObjNum == aTargetIndex)
				{
					break;
				}
			}
		}

		count++;

		if (count > MAX_VIEWPORT - 1)
			return FALSE;
	}

	if (lpObj.VpPlayer2[count].type == OBJ_USER)
	{
		if (g_GensSystem.IsMapBattleZone(lpObj.MapNumber) == TRUE)
		{
			EnableAttack = TRUE;
		}

		if (gclassObjAttack.PkCheck(lpObj, lpTargetObj) == TRUE)
		{
			EnableAttack = TRUE;
		}
	}

	else if (lpTargetObj.Type == OBJ_MONSTER || gGameObjects[tObjNum]->m_RecallMon < 0)
	{
		EnableAttack = TRUE;
	}

	else if (tObjNum == lpObj.m_iDuelUser)
	{
		EnableAttack = TRUE;
	}

	else if (gGameObjects[tObjNum]->Type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon >= 0)
	{
		CallMonIndex = gGameObjects[gGameObjects[tObjNum]->m_RecallMon]->m_Index;

		if (gObjTargetGuildWarCheck(lpObj, &gGameObjects[CallMonIndex]) == 1)
		{
			EnableAttack = TRUE;
		}

		if (gGameObjects[tObjNum]->Class >= 678 && gGameObjects[tObjNum]->Class <= 680)
		{
			EnableAttack = TRUE;
		}
	}

	if (EnableAttack == TRUE && this->CalDistance(gGameObjects[aTargetIndex]->X, gGameObjects[aTargetIndex]->Y, gGameObjects[tObjNum]->X, gGameObjects[tObjNum]->Y) < nDistance)
	{
		switch (lpMagic->m_Skill)
		{
		case AT_SKILL_LARGE_BLOWER:
			HitCount = 5;
			break;
		case AT_SKILL_UPPER_BEAST:
			HitCount = 3;
			break;
		case AT_SKILL_CHAIN_DRIVE:
			HitCount = 9;
			break;
		case AT_SKILL_DARK_SIDE:
			HitCount = 3;
			break;
		default:
			HitCount = 0;
			break;
		}

		if ((lpMagic->m_Skill == AT_SKILL_CHAIN_DRIVE) && rand() % 100 < 40)
		{
			BYTE btAttr = MapC[lpTargetObj.MapNumber].GetAttr(lpTargetObj.X, lpTargetObj.Y);

			if ((btAttr & 1) != 1)
			{
				lpTargetObj.lpAttackObj = lpObj;
				lpTargetObj.DelayActionTime = 1000;
				lpTargetObj.DelayLevel = 1;

				gObjAddBuffEffect(lpTargetObj, BUFFTYPE_COLD, EFFECTTYPE_REDUCE_MOVE_SPEED, 0, 0, 0, 10);
			}
		}

		else if ((lpMagic->m_Skill == AT_SKILL_UPPER_BEAST) && rand() % 100 < 10)
		{
			gObjAddBuffEffect(lpTargetObj, BUFFTYPE_DEFENSE_POWER_DEC, EFFECTTYPE_DECREASE_DEFENSE, 10, 0, 0, 10);
		}

		else if ((lpMagic->m_Skill == AT_SKILL_LARGE_BLOWER) && rand() % 100 < 10)
		{
			gObjAddBuffEffect(lpTargetObj, BUFFTYPE_CURSED_ATTACK_DOWN, EFFECTTYPE_CURSED_ATTACKDOWN, 5, 0, 0, 10);
		}

		for (int i = 1; i < HitCount; i++)
		{
			if (i % (HitCount - 1) == 1)
				gObjAttack(lpObj, &gGameObjects[tObjNum], lpMagic, 1, 1, 0, 0, i, 0);
			else
				gObjAttack(lpObj, &gGameObjects[tObjNum], lpMagic, 0, 1, 0, 0, i, 0);
		}

	}
}

int CObjUseSkill::SkillCharge(CGameObject &lpObj, CMagicInf *lpMagic, int aTargetIndex)
{
	if (ObjectMaxRange(aIndex) == FALSE)
	{
		return FALSE;
	}

	
	CGameObject lpTargetObj = &gGameObjects[aTargetIndex];

	if (lpObj.MapNumber == MAP_INDEX_CASTLESIEGE || lpObj.MapNumber == MAP_INDEX_CASTLEHUNTZONE || this->m_SkillData.EnableSiegeOnAllMaps == true)
	{
		if (lpObj.MapNumber == lpTargetObj.MapNumber)
		{
			GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, TRUE);
			gObjAttack(lpObj, lpTargetObj, lpMagic, 0, 1, 0, 0, 0, 0);
			return TRUE;
		}
	}

	return FALSE;
}

int CObjUseSkill::SkillMonkDarkSideGetTargetIndex(CGameObject &lpObj, int aTargetIndex, CMagicInf* lpMagic, WORD *Target)
{
	

	int count = 0;
	int HitCount = 0;
	int DuelUser = lpObj.m_iDuelUser;

	int nDistance = MagicDamageC.GetSkillDistance(lpMagic->m_Skill);

	if (lpMagic->m_Skill == 563)
	{
		int iPoint = g_MasterLevelSkillTreeSystem.GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);
		nDistance += iPoint / 10;
	}

	if (Target != NULL)
	{
		if (lpMagic->m_Skill == 263 || lpMagic->m_Skill == 559 || lpMagic->m_Skill == 563)
		{
			if (aTargetIndex)
			{
				while (true)
				{
					if (lpObj.VpPlayer2[count].state == 1)
					{
						int tObjNum = lpObj.VpPlayer2[count].number;

						if (tObjNum >= 0)
						{
							if (tObjNum != 10000 && lpObj.VpPlayer2[count].type == OBJ_USER && tObjNum == aTargetIndex)
							{
								if (this->CalDistance(lpObj.X, lpObj.Y, gGameObjects[tObjNum]->X, gGameObjects[tObjNum]->Y) < nDistance)
								{
									*Target = tObjNum;
									HitCount = 1;
									break;
								}
							}
						}
					}

					count++;

					if (count > MAX_VIEWPORT - 1)
					{
						break;
					}
				}
			}

			count = 0;

			while (true)
			{
				if (lpObj.VpPlayer2[count].state == 1)
				{
					int tObjNum = lpObj.VpPlayer2[count].number;

					if (tObjNum >= 0)
					{
						CGameObject lpTargetObj = &gGameObjects[tObjNum];

						BOOL EnableAttack = FALSE;
						int CallMonIndex = -1;

						if (lpTargetObj.Type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon < 0)
						{
							EnableAttack = TRUE;
						}

						else if (tObjNum == lpObj.m_iDuelUser)
						{
							EnableAttack = TRUE;
						}

						else if (gGameObjects[tObjNum]->Type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon >= 0)
						{
							CallMonIndex = gGameObjects[gGameObjects[tObjNum]->m_RecallMon]->m_Index;

							if (gObjTargetGuildWarCheck(lpObj, &gGameObjects[CallMonIndex]) == 1)
							{
								EnableAttack = TRUE;
							}

							if (gGameObjects[tObjNum]->Class >= 678 && gGameObjects[tObjNum]->Class <= 680)
							{
								EnableAttack = TRUE;
							}
						}

						if (EnableAttack == TRUE && gObjAttackQ(lpTargetObj) == FALSE)
						{
							EnableAttack = FALSE;
						}

						if (EnableAttack == TRUE)
						{
							if (this->CalDistance(lpObj.X, lpObj.Y, lpTargetObj.X, lpTargetObj.Y) < nDistance)
							{
								Target[HitCount] = tObjNum;
								HitCount++;

								if (HitCount >= 5)
									break;
							}
						}
					}
				}

				count++;
				if (count > MAX_VIEWPORT - 1)
					break;
			}
		}
	}

	return TRUE;
}

void CObjUseSkill::SplashDamage(CGameObject &lpObj, CGameObject lpTargetObj, int nSkill, int nAttackDamage, int nDistance, int nDamageRate)
{
	if (gObjIsConnected(lpObj) && gObjIsConnected(lpTargetObj))
	{
		if (nAttackDamage > 0)
		{
			int iCount = 0;
			BOOL bEnableAttack = FALSE;

			while (true)
			{
				if (lpObj.VpPlayer2[iCount].state)
				{
					int iTarObjNum = lpObj.VpPlayer2[iCount].number;

					if (iTarObjNum >= 0)
					{
						bEnableAttack = FALSE;

						if (lpObj.VpPlayer2[iTarObjNum].type == OBJ_MONSTER && gGameObjects[iTarObjNum]->m_RecallMon < 0)
						{
							bEnableAttack = TRUE;
						}

						if (lpTargetObj.m_cChaosCastleIndex != -1 && lpTargetObj.m_cChaosCastleIndex == gGameObjects[iTarObjNum]->m_cChaosCastleIndex)
						{
							bEnableAttack = TRUE;
						}

						if (lpObj.Type == OBJ_USER && lpTargetObj.Type == OBJ_USER && gGameObjects[iTarObjNum]->Type != OBJ_NPC)
						{
							bEnableAttack = TRUE;
						}

						if (gGameObjects[iTarObjNum]->Class >= 678 && gGameObjects[iTarObjNum]->Class <= 680)
						{
							bEnableAttack = TRUE;
						}

						if (bEnableAttack == TRUE)
						{
							if (lpObj.m_Index != iTarObjNum)
							{
								if (gObjCalDistance(lpTargetObj, &gGameObjects[iTarObjNum]) <= nDistance)
								{
									gObjAddMsgSendDelay(lpObj, 58, iTarObjNum, 300, nDamageRate * nAttackDamage / 100);
								}
							}
						}
					}
				}

				iCount++;

				if (iCount > MAX_VIEWPORT - 1)
					break;
			}
		}
	}
}

void CObjUseSkill::SkillDarkSide(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic)
{
	int iTargetCount = 0;
	WORD wTargetList[5];

	for (int i = 0; i < 5; i++)
	{
		wTargetList[i] = 10000;
	}

	if (lpObj.CloseCount < 0 &&
		lpMagic != NULL &&
		this->SkillMonkDarkSideGetTargetIndex(aIndex, aTargetIndex, lpMagic, wTargetList) == TRUE)
	{
		PMSG_RAGE_ATTACK_RANGE_ANS pMsg;
		PHeadSetBE((LPBYTE)&pMsg, 0x4B, sizeof(pMsg));

		memcpy(&pMsg.Target, wTargetList, sizeof(wTargetList));
		pMsg.MagicNumber = lpMagic->m_Skill;;

		lpObj.m_PlayerData->DarkSideCount = 0;

		int iTargetCount = 0;

		for (int i = 0; i < 5; i++)
		{
			if (wTargetList[i] != 10000)
			{
				lpObj.m_PlayerData->DarkSideCount++;
				gObjAddAttackProcMsgSendDelay(&gGameObjects[aIndex], 61, aIndex, 150 + (iTargetCount * 150), lpMagic->m_Skill, wTargetList[i]);
				iTargetCount++;
			}
		}

		memcpy(lpObj.m_PlayerData->DarkSideTarget, wTargetList, sizeof(wTargetList));
		IOCP.DataSend(lpObj.m_Index, (LPBYTE)&pMsg, pMsg.h.size);
	}
}

void CObjUseSkill::SkillSummonForLordSilvester(CGameObject &lpObj, int iTargetIndex, CMagicInf *lpMagic)
{
	if (!ObjectMaxRange(aIndex))
	{
		return;
	}

	int nCallMonster[3];
	nCallMonster[0] = 678;
	nCallMonster[1] = 679;
	nCallMonster[2] = 680;

	

	for (int iSC = 0; iSC < 3; iSC++)
	{
		BYTE cSX = lpObj.X;
		BYTE cSY = lpObj.Y;

		if (gObjGetRandomFreeLocation(lpObj.MapNumber, cSX, cSY, 5, 5, 30) == TRUE)
		{
			int result = gObjAddMonster(lpObj.MapNumber);
			int nMonsterIndex = nCallMonster[iSC];
			LPMONSTER_ATTRIBUTE lpm = gMAttr.GetAttr(nMonsterIndex);

			if (result >= 0)
			{
				gGameObjects[result]->m_PosNum = -1;
				gGameObjects[result]->X = cSX;
				gGameObjects[result]->Y = cSY;
				gGameObjects[result]->MapNumber = lpObj.MapNumber;
				gGameObjects[result]->TX = gGameObjects[result]->X;
				gGameObjects[result]->TY = gGameObjects[result]->Y;
				gGameObjects[result]->m_OldX = gGameObjects[result]->X;
				gGameObjects[result]->m_OldY = gGameObjects[result]->Y;
				gGameObjects[result]->Dir = 1;
				gGameObjects[result]->StartX = gGameObjects[result]->X;
				gGameObjects[result]->StartY = gGameObjects[result]->Y;
				gObjSetMonster(result, nMonsterIndex);
				gGameObjects[result]->m_Attribute = 100;
				gGameObjects[result]->MaxRegenTime = 0;
				gGameObjects[result]->Dir = rand() % 8;
				gGameObjects[result]->m_RecallMon = aIndex;
				lpObj.m_nRecallMonIndex[iSC] = result;

				if (lpm->m_PentagramMainAttribute == 6)
				{
					switch (rand() % 5)
					{
					case 0:
						gGameObjects[result]->m_iPentagramMainAttribute = EL_FIRE;
						break;
					case 1:
						gGameObjects[result]->m_iPentagramMainAttribute = EL_WATER;
						break;
					case 2:
						gGameObjects[result]->m_iPentagramMainAttribute = EL_EARTH;
						break;
					case 3:
						gGameObjects[result]->m_iPentagramMainAttribute = EL_WIND;
						break;
					case 4:
						gGameObjects[result]->m_iPentagramMainAttribute = EL_DARKNESS;
						break;
					}
				}

				else if (lpm->m_PentagramMainAttribute > 0)
				{
					gGameObjects[result]->m_iPentagramMainAttribute = lpm->m_PentagramMainAttribute;
				}

				gGameObjects[result]->m_iPentagramAttributePattern = lpm->m_PentagramAttributePattern;
				gGameObjects[result]->m_iPentagramDefense = lpm->m_PentagramDefense;
				gGameObjects[result]->m_iPentagramAttackMin = lpm->m_PentagramAttackMin;
				gGameObjects[result]->m_iPentagramAttackMax = lpm->m_PentagramAttackMax;
				gGameObjects[result]->m_iPentagramAttackRating = lpm->m_PentagramAttackRating;
				gGameObjects[result]->m_iPentagramDefenseRating = lpm->m_PentagramDefenseRating;

			}
		}
	}
}

void CObjUseSkill::SkillSpinStep(CGameObject &lpObj, int iTargetIndex, CMagicInf * lpMagic)
{
	
	CGameObject lpTargetObj = &gGameObjects[iTargetIndex];

	if (gObjIsConnected(lpObj.m_Index) == FALSE && gObjIsConnected(lpTargetObj.m_Index) == FALSE)
	{
		return;
	}

	if (lpObj.Type != OBJ_USER || lpObj.Class != CLASS_GROWLANCER)
	{
		return;
	}

	bool EnableAttack = false;
	int count = 0;
	int tObjNum;
	int iDuelIndex = lpObj.m_iDuelUser;

	while (true)
	{
		if (lpObj.VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = 0;

				if (lpObj.VpPlayer2[count].type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if (tObjNum == iTargetIndex || tObjNum == iDuelIndex)
				{
					EnableAttack = 1;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && lpObj.VpPlayer2[count].type != OBJ_NPC && gGameObjects[tObjNum]->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = 1;
				}

				else
				{
					int CallMonIndex = gGameObjects[tObjNum]->m_Index;

					if (gGameObjects[tObjNum]->Type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon >= 0)
					{
						CallMonIndex = gGameObjects[gGameObjects[tObjNum]->m_RecallMon]->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &gGameObjects[CallMonIndex]) == 1)
					{
						EnableAttack = 1;
					}

					if (gGameObjects[tObjNum]->Class >= 678 && gGameObjects[tObjNum]->Class <= 680)
					{
						EnableAttack = 1;
					}
				}

				if (EnableAttack != 0)
				{
					if (gObjCalDistance(lpTargetObj, &gGameObjects[tObjNum]) <= MagicDamageC.GetSkillDistance(lpMagic->m_Skill)) // 2x2 around target
					{
						gObjAttack(lpObj, &gGameObjects[tObjNum], lpMagic, 1, 0, 0, 0, 0, 0);
					}
				}
			}
		}

		count++;

		if (count > MAX_VIEWPORT - 1)
			break;
	}
}

void CObjUseSkill::SkillGrowLancerAttackOneTarget(CGameObject &lpObj, int aTargetIndex, CMagicInf * lpMagic)
{
	

	if (ObjectMaxRange(aTargetIndex) == FALSE)
	{
		sLog->outBasic("[InvalidTargetIndex][SkillGrowLancerAttackOneTarget] Index :%d , AccountID : %s", aIndex, lpObj.AccountID);
		return;
	}

	CGameObject lpTargetObj = &gGameObjects[aTargetIndex];

	BOOL EnableAttack = FALSE;
	int count = 0;
	int tObjNum;
	int CallMonIndex;
	int HitCount = 0;

	int nDistance = MagicDamageC.GetSkillDistance(lpMagic->m_Skill);

	while (true)
	{
		if (lpObj.VpPlayer2[count].state == TRUE)
		{
			tObjNum = lpObj.VpPlayer2[count].number;
			if (tObjNum >= 0)
			{
				if (tObjNum == aTargetIndex)
				{
					break;
				}
			}
		}

		count++;

		if (count > MAX_VIEWPORT - 1)
			return;
	}

	if (lpObj.VpPlayer2[count].type == OBJ_USER)
	{
		if (g_GensSystem.IsMapBattleZone(lpObj.MapNumber) == TRUE)
		{
			EnableAttack = TRUE;
		}

		if (gclassObjAttack.PkCheck(lpObj, lpTargetObj) == TRUE)
		{
			EnableAttack = TRUE;
		}
	}

	else if (lpTargetObj.Type == OBJ_MONSTER || gGameObjects[tObjNum]->m_RecallMon < 0)
	{
		EnableAttack = TRUE;
	}

	else if (tObjNum == lpObj.m_iDuelUser)
	{
		EnableAttack = TRUE;
	}

	else if (gGameObjects[tObjNum]->Type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon >= 0)
	{
		CallMonIndex = gGameObjects[gGameObjects[tObjNum]->m_RecallMon]->m_Index;

		if (gObjTargetGuildWarCheck(lpObj, &gGameObjects[CallMonIndex]) == 1)
		{
			EnableAttack = TRUE;
		}

		if (gGameObjects[tObjNum]->Class >= 678 && gGameObjects[tObjNum]->Class <= 680)
		{
			EnableAttack = TRUE;
		}
	}

	int nHitCount = 0;

	if (lpMagic->m_Skill == 276 || lpMagic->m_Skill == 688 || lpMagic->m_Skill == 691) // Harsh Strike
	{
		nHitCount = 3; // 2 attacks
	}

	else if (lpMagic->m_Skill == 274 || lpMagic->m_Skill == 692 || lpMagic->m_Skill == 694 || lpMagic->m_Skill == 695) // Magic Pin
	{
		nHitCount = 3; // 3 attacks
	}


	if (EnableAttack == TRUE && this->CalDistance(gGameObjects[aTargetIndex]->X, gGameObjects[aTargetIndex]->Y, gGameObjects[tObjNum]->X, gGameObjects[tObjNum]->Y) < nDistance)
	{
		for (int i = 1; i < nHitCount; i++)
		{
			if ((i % (nHitCount - 1)) == 1)
				gObjAttack(lpObj, &gGameObjects[tObjNum], lpMagic, 1, 1, 0, 0, i, 0);
			else
				gObjAttack(lpObj, &gGameObjects[tObjNum], lpMagic, 0, 1, 0, 0, i, 0);
		}

		if (lpMagic->m_Skill == 274 || lpMagic->m_Skill == 692 || lpMagic->m_Skill == 694 || lpMagic->m_Skill == 695)
		{
			CMagicInf * MagicPinExp = &DefMagicInf[AT_SKILL_MAGIC_PIN_EXPLOSION];
			gObjAttack(lpObj, &gGameObjects[tObjNum], MagicPinExp, 1, 0, 0, 0, 0, 0);
		}

	}
}

int CObjUseSkill::SkillBreche(CGameObject &lpObj, CMagicInf * lpMagic, BYTE x, BYTE y, int aTargetIndex)
{
	
	int tObjNum;
	int count = 0;
	int HitCount = 0;
	int bAttack;
	int DuelIndex = lpObj.m_iDuelUser;
	int EnableAttack;
	CGameObject lpTargetObj = &gGameObjects[aTargetIndex];

	while (true)
	{
		if (lpObj.VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = 0;

				if (lpObj.VpPlayer2[count].type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon < 0)
				{
					EnableAttack = 1;
				}

				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && lpObj.VpPlayer2[count].type != OBJ_NPC && gGameObjects[tObjNum]->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = 1;
				}

				else
				{
					int CallMonIndex = gGameObjects[tObjNum]->m_Index;

					if (gGameObjects[tObjNum]->Type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon >= 0)
					{
						CallMonIndex = gGameObjects[gGameObjects[tObjNum]->m_RecallMon]->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &gGameObjects[CallMonIndex]) == 1)
					{
						EnableAttack = 1;
					}

					if (gGameObjects[tObjNum]->Class >= 678 && gGameObjects[tObjNum]->Class <= 680)
					{
						EnableAttack = 1;
					}
				}
				if (EnableAttack != 0)
				{
					if (this->CalDistance(x, y, gGameObjects[tObjNum]->X, gGameObjects[tObjNum]->Y) < MagicDamageC.GetSkillDistance(lpMagic->m_Skill))
					{
						bAttack = 0;

						if (HitCount > 4)
						{
							bAttack = 0;
						}

						else
						{
							bAttack = 1;
							HitCount++;
						}

						if (bAttack != 0)
						{
							gObjAttack(lpObj, &gGameObjects[tObjNum], lpMagic, 1, 1, 0, 0, 0, 0);
						}
					}
				}
			}
		}

		count++;
		if (count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}
	return true;
}

int CObjUseSkill::SkillShiningPeak(CGameObject &lpObj, CMagicInf * lpMagic, int aTargetIndex)
{
	

	if (ObjectMaxRange(aTargetIndex) == FALSE)
	{
		return FALSE;
	}

	CGameObject lpTargetObj = &gGameObjects[aTargetIndex];

	BOOL EnableAttack = FALSE;
	int count = 0;
	int tObjNum;
	int CallMonIndex;
	int HitCount = 0;

	int nDistance = MagicDamageC.GetSkillDistance(lpMagic->m_Skill);

	while (true)
	{
		if (lpObj.VpPlayer2[count].state == TRUE)
		{
			tObjNum = lpObj.VpPlayer2[count].number;
			if (tObjNum >= 0)
			{
				if (tObjNum == aTargetIndex)
				{
					break;
				}
			}
		}

		count++;

		if (count > MAX_VIEWPORT - 1)
			return FALSE;
	}

	if (lpObj.VpPlayer2[count].type == OBJ_USER)
	{
		if (g_GensSystem.IsMapBattleZone(lpObj.MapNumber) == TRUE)
		{
			EnableAttack = TRUE;
		}

		if (gclassObjAttack.PkCheck(lpObj, lpTargetObj) == TRUE)
		{
			EnableAttack = TRUE;
		}
	}

	else if (lpTargetObj.Type == OBJ_MONSTER || gGameObjects[tObjNum]->m_RecallMon < 0)
	{
		EnableAttack = TRUE;
	}

	else if (tObjNum == lpObj.m_iDuelUser)
	{
		EnableAttack = TRUE;
	}

	else if (gGameObjects[tObjNum]->Type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon >= 0)
	{
		CallMonIndex = gGameObjects[gGameObjects[tObjNum]->m_RecallMon]->m_Index;

		if (gObjTargetGuildWarCheck(lpObj, &gGameObjects[CallMonIndex]) == 1)
		{
			EnableAttack = TRUE;
		}

		if (gGameObjects[tObjNum]->Class >= 678 && gGameObjects[tObjNum]->Class <= 680)
		{
			EnableAttack = TRUE;
		}
	}

	int nHitCount = 4;
	BYTE byPentagramAttack = TRUE;

	if (EnableAttack == TRUE && this->CalDistance(gGameObjects[aTargetIndex]->X, gGameObjects[aTargetIndex]->Y, gGameObjects[tObjNum]->X, gGameObjects[tObjNum]->Y) < nDistance)
	{
		int nHitCount = 4;

		for (int i = 1; i < nHitCount; i++, byPentagramAttack = FALSE)
		{
			if ((i % (nHitCount - 1)) == 1)
				gObjAttack(lpObj, &gGameObjects[tObjNum], lpMagic, 1, 1, 0, 0, i, 0, byPentagramAttack);
			else
				gObjAttack(lpObj, &gGameObjects[tObjNum], lpMagic, 0, 1, 0, 0, i, 0, byPentagramAttack);
		}
	}
}

int CObjUseSkill::SkillBuffGrowLancer(CGameObject &lpObj, CMagicInf * lpMagic, int aTargetIndex)
{
	if (ObjectMaxRange(aIndex) == FALSE)
	{
		return FALSE;
	}

	

	if (lpObj.Type != OBJ_USER && lpObj.m_RecallMon == -1)
	{
		return FALSE;
	}

	if (lpObj.Class != CLASS_GROWLANCER)
	{
		return FALSE;
	}

	int skill_improve = 0;
	int skill_improve2 = 0;
	int skill_time = 0;

	BYTE BuffIndex = BUFFTYPE_NONE;

	if (lpMagic->m_Skill == AT_SKILL_WRATH)
	{
		BuffIndex = BUFFTYPE_WRATH;
		this->m_Lua.Generic_Call("GrowLancerWrath", "iii>iii", lpObj.m_PlayerData->Strength + lpObj.AddStrength, lpObj.m_PlayerData->Dexterity + lpObj.AddDexterity,
			lpObj.m_PlayerData->Energy + lpObj.AddEnergy,
			&skill_improve, &skill_improve2, &skill_time);

		gObjAddBuffEffect(lpObj, BuffIndex, EFFECTTYPE_WRATH_INC_DAMAGE, skill_improve, EFFECTTYPE_DECREASE_DEFENSE, skill_improve2, skill_time);
	}

	else if (lpMagic->m_Skill == AT_SKILL_CIRCLE_SHIELD)
	{
		BuffIndex = BUFFTYPE_CIRCLE_SHIELD;
		this->m_Lua.Generic_Call("GrowLancerCircleShield", "iii>iii", lpObj.m_PlayerData->Strength + lpObj.AddStrength, lpObj.m_PlayerData->Dexterity + lpObj.AddDexterity,
			lpObj.m_PlayerData->Energy + lpObj.AddEnergy,
			&skill_improve, &skill_improve2, &skill_time);

		lpObj.m_SkillInfo.fCircleShieldRate = skill_improve2;
		gObjAddBuffEffect(lpObj, BuffIndex, EFFECTTYPE_AG_OPPONENT_DECREASE, skill_improve, 0, 0, skill_time);
	}

	else
	{
		return FALSE;
	}

	GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aIndex, TRUE);
	return TRUE;
}

int CObjUseSkill::SkillBuffPartyGrowLancer(CGameObject &lpObj, CMagicInf * lpMagic, int aTargetIndex)
{
	if (ObjectMaxRange(aIndex) == FALSE)
	{
		return FALSE;
	}

	

	if (lpObj.Type != OBJ_USER && lpObj.m_RecallMon == -1)
	{
		return FALSE;
	}

	if (lpObj.Class != CLASS_GROWLANCER)
	{
		return FALSE;
	}

	int partynum = 0;
	int partyindex[5];

	memset(partyindex, -1, sizeof(partyindex));

	partynum = lpObj.PartyNumber;

	if (partynum != -1)
	{
		for (int i = 0; i < MAX_USER_IN_PARTY; i++)
		{
			int index = gParty.m_PartyS[partynum].Number[i];

			if (index >= 0 && index < g_ConfigRead.server.GetObjectMax())
			{
				CGameObject lpPartyObj = &gGameObjects[index];

				if (lpPartyObj.MapNumber == lpObj.MapNumber)
				{
					if (gObjCalDistance(lpObj, lpPartyObj) < 4)
					{
						partyindex[i] = index;
					}
				}
			}
		}
	}

	int skill_improve = 0;
	int skill_time = 0;

	BYTE BuffIndex = BUFFTYPE_OBSIDIAN;

	gObjUseSkill.m_Lua.Generic_Call("GrowLancerObsidian", "iii>ii", (lpObj.m_PlayerData->Strength + lpObj.AddStrength), (lpObj.m_PlayerData->Dexterity + lpObj.AddDexterity),
		(lpObj.m_PlayerData->Energy + lpObj.AddEnergy), &skill_improve, &skill_time);

	if (partynum == -1)
	{
		gObjAddBuffEffect(lpObj, BuffIndex, EFFECTTYPE_IMPROVE_DAMAGE, skill_improve, 0, 0, skill_time);
		GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aIndex, TRUE);
	}

	else
	{
		for (int j = 0; j < 5; j++)
		{
			if (partyindex[j] != -1)
			{
				CGameObject lpPartyObj = &gGameObjects[partyindex[j]];

				gObjAddBuffEffect(lpPartyObj, BuffIndex, EFFECTTYPE_IMPROVE_DAMAGE, skill_improve, 0, 0, skill_time);
				GSProtocol.GCMagicAttackNumberSend(lpPartyObj, lpMagic->m_Skill, partyindex[j], TRUE);
			}
		}
	}

	return TRUE;
}

void CObjUseSkill::SkillWindSoul(CGameObject lpObj, CMagicInf * lpMagic, int aTargetIndex)
{
	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_WINDSOUL, aTargetIndex, 1);
	gObjAttack(lpObj, &gGameObjects[aTargetIndex], lpMagic, TRUE, 0, 0, 0, 0, 0);
}

void CObjUseSkill::SkillIceBlood(CGameObject lpObj, CMagicInf * lpMagic, int aTargetIndex)
{
	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_ICE_BLOOD, aTargetIndex, 1);
	gObjAttack(lpObj, &gGameObjects[aTargetIndex], lpMagic, TRUE, 0, 0, 0, 0, 0);
}

void CObjUseSkill::SkillDarkBlast(CGameObject lpObj, CMagicInf * lpMagic, int aTargetIndex)
{
	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_DARKBLAST, aTargetIndex, 1);
	gObjAttack(lpObj, &gGameObjects[aTargetIndex], lpMagic, TRUE, 0, 0, 0, 0, 0);
}

void CObjUseSkill::SkillFireBlood(CGameObject lpObj, CMagicInf * lpMagic, int aTargetIndex)
{
	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_FIRE_BLOOD, aTargetIndex, 1);
	gObjAttack(lpObj, &gGameObjects[aTargetIndex], lpMagic, TRUE, 0, 0, 0, 0, 0);
}

int CObjUseSkill::SkillFireBeast(CGameObject &lpObj, int aTargetIndex, CMagicInf *lpMagic)
{
	
	int StartDis = 1;
	int tObjNum;
	int count = 0;
	int loopcount = 0;

	GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_FIRE_BIRD, aTargetIndex, 1);
	gObjAttack(lpObj, &gGameObjects[aTargetIndex], lpMagic, 0, 1, 0, 0, 0, 0);

	int attackcheck;
	int DuelIndex = lpObj.m_iDuelUser;
	int EnableAttack;
	int delaytime;

	while (true)
	{
		if (lpObj.VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj.VpPlayer2[count].number;

			if (tObjNum >= 0 && aTargetIndex != tObjNum)
			{
				EnableAttack = 0;

				if (lpObj.VpPlayer2[count].type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
				else
				{
					int CallMonIndex = gGameObjects[tObjNum]->m_Index;

					if (gGameObjects[tObjNum]->Type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon >= 0)
					{
						CallMonIndex = gGameObjects[gGameObjects[tObjNum]->m_RecallMon]->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &gGameObjects[CallMonIndex]) == 1)
					{
						EnableAttack = 1;
					}

					if (gGameObjects[tObjNum]->Class >= 678 && gGameObjects[tObjNum]->Class <= 680)
					{
						EnableAttack = 1;
					}
				}
			}
		}

		count++;
		if (count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}
	return true;
}

int CObjUseSkill::SkillIceBeast(CGameObject &lpObj, int aTargetIndex, CMagicInf *lpMagic)
{
	
	int StartDis = 1;
	int tObjNum;
	int count = 0;
	int loopcount = 0;

	GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_ICE_BLOOD, aTargetIndex, 1);
	gObjAttack(lpObj, &gGameObjects[aTargetIndex], lpMagic, 0, 1, 0, 0, 0, 0);

	int attackcheck;
	int DuelIndex = lpObj.m_iDuelUser;
	int EnableAttack;
	int delaytime;

	while (true)
	{
		if (lpObj.VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj.VpPlayer2[count].number;

			if (tObjNum >= 0 && aTargetIndex != tObjNum)
			{
				EnableAttack = 0;

				if (lpObj.VpPlayer2[count].type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
				else
				{
					int CallMonIndex = gGameObjects[tObjNum]->m_Index;

					if (gGameObjects[tObjNum]->Type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon >= 0)
					{
						CallMonIndex = gGameObjects[gGameObjects[tObjNum]->m_RecallMon]->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &gGameObjects[CallMonIndex]) == 1)
					{
						EnableAttack = 1;
					}

					if (gGameObjects[tObjNum]->Class >= 678 && gGameObjects[tObjNum]->Class <= 680)
					{
						EnableAttack = 1;
					}
				}
			}
		}

		count++;
		if (count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}
	return true;
}

int CObjUseSkill::SkillMeteorStorm(CGameObject &lpObj, CMagicInf* lpMagic, BYTE x, BYTE y, int aTargetIndex)
{
	
	int tObjNum;
	int count = 0;
	int HitCount = 0;
	int bAttack;
	int DuelIndex = lpObj.m_iDuelUser;
	int EnableAttack;
	CGameObject lpTargetObj = &gGameObjects[aTargetIndex];

	while (true)
	{
		if (lpObj.VpPlayer2[count].state != 0)
		{
			tObjNum = lpObj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = 0;

				if (lpObj.VpPlayer2[count].type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon < 0)
				{
					EnableAttack = 1;
				}

				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && lpObj.VpPlayer2[count].type != OBJ_NPC && gGameObjects[tObjNum]->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = 1;
				}

				else
				{
					int CallMonIndex = gGameObjects[tObjNum]->m_Index;

					if (gGameObjects[tObjNum]->Type == OBJ_MONSTER && gGameObjects[tObjNum]->m_RecallMon >= 0)
					{
						CallMonIndex = gGameObjects[gGameObjects[tObjNum]->m_RecallMon]->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &gGameObjects[CallMonIndex]) == 1)
					{
						EnableAttack = 1;
					}

					if (gGameObjects[tObjNum]->Class >= 678 && gGameObjects[tObjNum]->Class <= 680)
					{
						EnableAttack = 1;
					}
				}
				if (EnableAttack != 0)
				{
					if (this->CalDistance(x, y, gGameObjects[tObjNum]->X, gGameObjects[tObjNum]->Y) < MagicDamageC.GetSkillDistance(lpMagic->m_Skill))
					{
						bAttack = 0;

						if (HitCount > 4)
						{
							bAttack = 0;
						}

						else
						{
							bAttack = 1;
							HitCount++;
						}

						if (bAttack != 0)
						{
							gObjAttack(lpObj, &gGameObjects[tObjNum], lpMagic, 1, 1, 0, 0, 0, 0);
						}
					}
				}
			}
		}

		count++;
		if (count > MAX_VIEWPORT - 1)
		{
			break;
		}
	}
	return true;
}

void CObjUseSkill::SkillSoulSeker(CGameObject lpObj, CMagicInf * lpMagic, int aTargetIndex)
{
	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	GSProtocol.GCMagicAttackNumberSend(lpObj, AT_SKILL_SOUL_SAKER, aTargetIndex, 1);
	gObjAttack(lpObj, &gGameObjects[aTargetIndex], lpMagic, TRUE, 0, 0, 0, 0, 0);
}

int CObjUseSkill::SkillArchangelWillBuff(CGameObject &lpObj, CMagicInf * lpMagic)
{
	

	if (lpObj.Type != OBJ_USER)
	{
		return 0;
	}

	if (lpMagic == NULL)
	{
		return 0;
	}

	if (lpMagic->m_Skill != AT_SKILL_ARCHANGEL_WILL)
	{
		return 0;
	}
	
	gObjAddBuffEffect(lpObj, BUFFTYPE_ARCHANGEL_WILL, EFFECTTYPE_IMPROVE_DAMAGE, 200, EFFECTTYPE_IMPROVE_DAMAGE, 50, 60);
	GSProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, lpObj.m_Index, TRUE);

	return 1;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

