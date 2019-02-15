////////////////////////////////////////////////////////////////////////////////
// MasterLevelSkillTreeSystem.cpp
#include "StdAfx.h"
#include "MasterLevelSkillTreeSystem.h"
#include "MasterSkillSystem.h"
#include "Logging/Log.h"
#include "GameMain.h"
#include "configread.h"
#include "ObjCalCharacter.h"
#include "BuffEffectSlot.h"
#include "BuffEffect.h"
#include "SkillAdditionInfo.h"
#include "ChaosCastle.h"
#include "ObjUseSkill.h"
#include "SkillHitBox.h"
#include "zzzmathlib.h"
#include "GensSystem.h"
#include "ObjAttack.h"
#include "Event/ArcaBattle/ArcaBattle.h"
#include "util.h"
#include "CustomMaxStats.h"

CMasterLevelSkillTreeSystem g_MasterLevelSkillTreeSystem;

MLST_Value_Table::MLST_Value_Table()
{
	this->fValueTable = NULL;
}

MLST_Value_Table::~MLST_Value_Table()
{
	if (this->fValueTable != NULL)
	{
		delete[] this->fValueTable;
		this->fValueTable = NULL;
	}
}

CMasterLevelSkillTreeSystem::CMasterLevelSkillTreeSystem(void) : m_Lua(true)
{
	this->m_MLSValueTable = NULL;
}

CMasterLevelSkillTreeSystem::~CMasterLevelSkillTreeSystem(void)
{
	if (this->m_MLSValueTable != NULL)
	{
		delete[] this->m_MLSValueTable;
		this->m_MLSValueTable = NULL;
	}
}

void CMasterLevelSkillTreeSystem::Init()
{
	memset(&this->m_MLSTable, 0x00, sizeof(this->m_MLSTable));

	if (this->m_MLSValueTable != NULL)
	{
		delete[] this->m_MLSValueTable;
		this->m_MLSValueTable = NULL;
	}

	this->m_MLSValueTable = new MLST_Value_Table[MAX_MASTER_SKILL_TYPE];

	for (int i = 0; i < MAX_MASTER_SKILL_TYPE; i++)
	{
		this->m_MLSValueTable[i].Init(this->GetMaxMasterSkillLevel());
	}

	lua_State* pLuaState = this->m_Lua.GetLua();

	if (pLuaState == NULL)
	{
		sLog->outBasic("[MasterLevelSkillTreeSystem] - Error - [%s] [%d]", __FILE__, __LINE__);
		return;
	}

	this->m_Lua.DoFile(g_ConfigRead.GetPath("\\Scripts\\Skills\\MasterSkillPoint.lua"));
	this->m_Lua.DoFile(g_ConfigRead.GetPath("\\Scripts\\Skills\\MasterSkillCalc.lua"));

	this->m_Lua.Generic_Call("MLS_ValueInit", ">");
}

void CMasterLevelSkillTreeSystem::AddToValueTable(int iSkillValueID, const char * szFunctionName, int iSkillValueType)
{
	if (iSkillValueID < 0 || iSkillValueID > MAX_MASTER_SKILL_TYPE)
	{
		sLog->outBasic("[MasterLevelSkillTreeSystem] - Error - Wrong iSkillValueID [%d] - [%s] [%d]", iSkillValueID, __FILE__, __LINE__);
		return;
	}

	this->m_MLSValueTable[iSkillValueID].iValueType = iSkillValueType;
	this->m_MLSValueTable[iSkillValueID].fValueTable[0] = 0.0;

	double fValue = 0.0;

	for (int i = 1; i <= this->GetMaxMasterSkillLevel(); i++)
	{
		this->m_Lua.Generic_Call(szFunctionName, "i>d", i, &fValue);
		this->m_MLSValueTable[iSkillValueID].fValueTable[i] = this->RoundValue(fValue, 2);
	}
}

void CMasterLevelSkillTreeSystem::Load(const char *lpszFileName)
{
	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(lpszFileName);

	if (res.status != pugi::status_ok)
	{
		sLog->outError("[MasterLevelSkillTreeSystem] - Can't Load %s (%s)", lpszFileName, res.description());
		return;
	}

	int iIndex = -1;
	int iTempIndex, iRank, iPos, iPoint, iMaxPoint, iParentSkill1, iParentSkill2, iSkillID;

	pugi::xml_node mainXML = file.child("MasterSkillTree");

	this->m_wMaxMasterSkillLevel = mainXML.attribute("MaxSkillLevel").as_int(20); // private custom !!!
	this->Init();

	for (pugi::xml_node class_node = mainXML.child("Class"); class_node; class_node = class_node.next_sibling())
	{
		int iClass = class_node.attribute("ID").as_int();

		for (pugi::xml_node tree = class_node.child("Tree"); tree; tree = tree.next_sibling())
		{
			int iTreeType = tree.attribute("Type").as_int();

			for (pugi::xml_node skill = tree.child("Skill"); skill; skill = skill.next_sibling())
			{
				iIndex = skill.attribute("Index").as_int();

				if (iIndex % 36 == 0)
				{
					iRank = 8;
					iPos = 3;
				}

				else
				{
					iTempIndex = iIndex % 36 - 1;
					iRank = iTempIndex / 4;
					iPos = iTempIndex % 4;
				}

				iPoint = skill.attribute("ReqMinPoint").as_int();
				iMaxPoint = skill.attribute("MaxPoint").as_int();
				iParentSkill1 = skill.attribute("ParentSkill1").as_int();
				iParentSkill2 = skill.attribute("ParentSkill2").as_int();
				iSkillID = skill.attribute("MagicNumber").as_int();

				for (int i = 0; i < MAX_TYPE_PLAYER; i++)
				{
					if (CHARACTER_CLASS_TABLE[i][0] & iClass)
					{
						this->m_MLSTable[CHARACTER_CLASS_TABLE[i][1]].iAbility[iTreeType].iElement[iRank][iPos].m_dwIndex = iIndex;
						this->m_MLSTable[CHARACTER_CLASS_TABLE[i][1]].iAbility[iTreeType].iElement[iRank][iPos].m_byConsumePoint = iPoint;
						this->m_MLSTable[CHARACTER_CLASS_TABLE[i][1]].iAbility[iTreeType].iElement[iRank][iPos].m_byMaxPoint = iMaxPoint;
						this->m_MLSTable[CHARACTER_CLASS_TABLE[i][1]].iAbility[iTreeType].iElement[iRank][iPos].m_dwParentSkill1 = iParentSkill1;
						this->m_MLSTable[CHARACTER_CLASS_TABLE[i][1]].iAbility[iTreeType].iElement[iRank][iPos].m_dwParentSkill2 = iParentSkill2;
						this->m_MLSTable[CHARACTER_CLASS_TABLE[i][1]].iAbility[iTreeType].iElement[iRank][iPos].m_dwSkillID = iSkillID;
					}
				}
			}
		}
	}

}

void CMasterLevelSkillTreeSystem::InitPassiveSkillData(CGameObject &Obj)
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	Obj.m_PlayerData->m_MPSkillOpt.Clear();
}

void CMasterLevelSkillTreeSystem::CalcPassiveSkillData(CGameObject &Obj, int iMLSkill, int iSkillLevel)
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.m_PlayerData->Strength < MagicDamageC.GetRequireStrength(iMLSkill))
	{
		return;
	}

	if (Obj.m_PlayerData->Dexterity < MagicDamageC.GetRequireDexterity(iMLSkill))
	{
		return;
	}

	float iSkillValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(iMLSkill), iSkillLevel);

	switch (this->GetBaseMasterLevelSkill(iMLSkill))
	{
	case 325:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAttackSuccessRate = iSkillValue;
		break;
	case 347:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncreasePvPAttackRate = iSkillValue;
		break;
	case 301:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncreasePvPDefenseRate = iSkillValue;
		break;
	case 300:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsDurabilityReduction1 = iSkillValue;
		Obj.m_PlayerData->m_MPSkillOpt.btMpsDownDur1Level = iSkillLevel;
		break;
	case 305:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsDurabilityReduction2 = iSkillValue;
		break;
	case 312:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsDurabilityReduction3 = iSkillValue;
		break;
	case 304:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsResistancePoison = iSkillValue;
		break;
	case 308:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsResistanceThunder = iSkillValue;
		break;
	case 311:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsResistanceIce = iSkillValue;
		break;
	case 307:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAutoRecoverLife = iSkillValue;
		break;
	case 313:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncDefenseSuccessRate = iSkillValue;
		break;
	case 309:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsDefence = iSkillValue;
		break;
	case 334:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsMaxHP = iSkillValue;
		break;
	case 338:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsMaxMana = iSkillValue;
		break;
	case 341:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsMaxBP = iSkillValue;
		break;
	case 362:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsMonsterDieGetMana = iSkillValue;
		break;
	case 359:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsMonsterDieGetLife = iSkillValue;
		break;
	case 358:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsMonsterDieGetSD = iSkillValue;
		break;
	case 303:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAutoRecoverMana = iSkillValue;
		break;
	case 310:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAutoRecoverAG = iSkillValue;
		break;
	case 302:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsMaxSD = iSkillValue;
		break;
	case 306:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsSDSpeed = iSkillValue;
		break;
	case 364:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsMaxAttackDamage = iSkillValue;
		break;
	case 361:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsMinAttackDamage = iSkillValue;
		break;
	case 357:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsDecreaseMana = iSkillValue;
		break;
	case 407:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsMaxMagicDamage = iSkillValue;
		break;
	case 405:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsMinMagicDamage = iSkillValue;
		break;
	case 535:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsPetDurDownSpeed = iSkillValue;
		break;
	case 335:
	case 485:
	case 513:
	case 557:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAddPhysicDamage = iSkillValue;
		break;
	case 348:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsTwoHandSwordAddDamage = iSkillValue;
		break;
	case 349:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsSwordAddDamage = iSkillValue;
		break;
	case AT_MSKILL_MACE_STRENGTHENER:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsMaceStrength = iSkillValue;
		break;
	case 351:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsSpearAddDamage = iSkillValue;
		break;
	case 352:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsTwoHandsPvPAttack = iSkillValue;
		break;
	case 353:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAddAttackSpeed = iSkillValue;
		break;
	case 354:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAddSturn = iSkillValue;
		break;
	case 355:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAddDoubleDamage = iSkillValue;
		break;
	case 386:
	case 488:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAddMagicDamage = iSkillValue;
		break;
	case 397:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsStaffMagicPower = iSkillValue;
		break;
	case 398:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsTwoHandsMagicPower = iSkillValue;
		break;
	case 399:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncShield = iSkillValue;
		break;
	case 400:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsStaffAttackSpeed = iSkillValue;
		break;
	case 401:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsTwoHandsStaffManAttack = iSkillValue;
		break;
	case 402:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsShieldBlockRate = iSkillValue;
		break;
	case 435:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsBowAddDamage = iSkillValue;
		break;
	case 438:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsBowAddAttackSpeed = iSkillValue;
		break;
	case 436:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsTwoHandsBowAddDamage = iSkillValue;
		break;
	case 439:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsTwoHandsBowManAttack = iSkillValue;
		break;
	case 421:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsElfAddPhysicDamage = iSkillValue;
		break;
	case 415:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsCallMonHp = iSkillValue;
		break;
	case 419:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsCallMonDefense = iSkillValue;
		break;
	case 428:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsCallMonAttack = iSkillValue;
		break;
	case 442:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAddMinAttack = iSkillValue;
		break;
	case 443:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAddMaxAttack = iSkillValue;
		break;
	case 448:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncDamageFireProper = iSkillValue;
		break;
	case 449:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncDamageWindProper = iSkillValue;
		break;
	case 450:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncDamageThunderProper = iSkillValue;
		break;
	case 451:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncDotDamage = iSkillValue;
		break;
	case 452:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAddSturn2 = iSkillValue;
		break;
	case 453:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAddSpring = iSkillValue;
		break;
	case 457:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncMagicPower = iSkillValue;
		break;
	case 465:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAddMagicPowerStick = iSkillValue;
		break;
	case 466:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAddCurseMagicBook = iSkillValue;
		break;
	case 467:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAddManAttackStick = iSkillValue;
		break;
	case 468:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAddAttackSpeedMagicBook = iSkillValue;
		break;
	case 471:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncMinMagicDamage = iSkillValue;
		break;
	case 526:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAddDarkSpiritDamage = iSkillValue;
		break;
	case 530:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAddCriticalDamageRate = iSkillValue;
		break;
	case 534:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAddExcellentDamageRate = iSkillValue;
		break;
	case 527:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAddCeptorDamage = iSkillValue;
		break;
	case 531:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncCeptorManAttack = iSkillValue;
		break;
	case 528:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAddShieldDefense = iSkillValue;
		break;
	case 532:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAddShieldBlockingRage = iSkillValue;
		break;
	case 529:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAddPetAttack = iSkillValue;
		break;
	case 533:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAddControlAttack = iSkillValue;
		break;
	case 556:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAddBlockingRate = iSkillValue;
		break;
	case 568:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsMonkWeaponDamage = iSkillValue;
		break;
	case 571:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsMonkDoubleDamage = iSkillValue;
		break;
	case 574:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsMonkAddVitalToDefense = iSkillValue;
		break;
	case 599:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAttackSuccessRate = iSkillValue;
		break;
	case 603:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncreasePvPAttackRate = iSkillValue;
		break;
	case 579:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncreasePvPDefenseRate = iSkillValue;
		break;
	case 578:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsDurabilityReduction1 = iSkillValue;
		Obj.m_PlayerData->m_MPSkillOpt.btMpsDownDur1Level = iSkillLevel;
		break;
	case 583:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsDurabilityReduction2 = iSkillValue;
		break;
	case 590:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsDurabilityReduction3 = iSkillValue;
		break;
	case 582:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsResistancePoison = iSkillValue;
		break;
	case 586:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsResistanceThunder = iSkillValue;
		break;
	case 589:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsResistanceIce = iSkillValue;
		break;
	case 585:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAutoRecoverLife = iSkillValue;
		break;
	case 591:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncDefenseSuccessRate = iSkillValue;
		break;
	case 587:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsDefence = iSkillValue;
		break;
	case 600:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsMaxHP = iSkillValue;
		break;
	case 601:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsMaxMana = iSkillValue;
		break;
	case 602:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsMaxBP = iSkillValue;
		break;
	case 608:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsMonsterDieGetMana = iSkillValue;
		break;
	case 606:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsMonsterDieGetLife = iSkillValue;
		break;
	case 605:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsMonsterDieGetSD = iSkillValue;
		break;
	case 581:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAutoRecoverMana = iSkillValue;
		break;
	case 588:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAutoRecoverAG = iSkillValue;
		break;
	case 580:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsMaxSD = iSkillValue;
		break;
	case 584:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsSDSpeed = iSkillValue;
		break;
	case 609:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAddMaxAttack = iSkillValue;
		break;
	case 607:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAddMinAttack = iSkillValue;
		break;
	case 604:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsDecreaseMana = iSkillValue;
		break;
	case 314:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsImmuneRate = iSkillValue;
		break;
	case 315:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncSetItemDefense = iSkillValue;
		break;
	case 316:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsReturnEnemyAttack = iSkillValue;
		break;
	case 317:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncEnergyStat = iSkillValue;
		break;
	case 318:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncVitalStat = iSkillValue;
		break;
	case 319:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncDexStat = iSkillValue;
		break;
	case 320:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncPowerStat = iSkillValue;
		break;
	case 506:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncLeadershipStat = iSkillValue;
		break;
	case 321:
	case 374:
	case 409:
	case 445:
	case 475:
	case 504:
	case 548:
	case 684:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAddWingDamageBlock = iSkillValue;
		break;
	case 627:
	case 638:
	case 647:
	case 657:
	case 667:
	case 674:
	case 681:
	case 717:
		Obj.m_PlayerData->m_Resistance_Stun = iSkillValue;
		break;
	case 626: // steel armor
	case 637:
	case 646:
	case 656:
	case 666:
	case 673:
	case 680:
	case 716:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncSteelArmor = iSkillValue;
		break;
	case 322:
	case 375:
	case 410:
	case 446:
	case 476:
	case 505:
	case 549:
	case 685:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAddWingDefense = iSkillValue;
		break;
	case 324:
	case 377:
	case 412:
	case 447:
	case 478:
	case 507:
	case 550:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAddWingDamage = iSkillValue;
		break;
	case 360:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsMasManaRate_Wizard = iSkillValue;
		break;
	case 363:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsAutoRecoverAG = iSkillValue;
		break;
	case 366:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncCriticalDamageRate = iSkillValue;
		break;
	case 367:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsRecoverManaRate = iSkillValue;
		break;
	case 368:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsRecoverHPRate = iSkillValue;
		break;
	case 369:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncExcellentDamageRate = iSkillValue;
		break;
	case 370:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncDoubleDamageRate = iSkillValue;
		break;
	case 371:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncIgnoreEnemyBlock = iSkillValue;
		break;
	case 372:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsRecoverSDRate = iSkillValue;
		break;
	case 373:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncTripleDamageRate = iSkillValue;
		break;
	case 345:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsComboDamage = iSkillValue;
		break;
	case 473:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncMaxMagicDamage = iSkillValue;
		break;
	case 536:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncDarkSpiritAttackSpeed = iSkillValue;
		break;
	case 538:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncDoubleDamageRate = iSkillValue;
		break;
	case 539:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsDarkSpiritIgnoreEnemyDefense = iSkillValue;
		break;
	case 592:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsImmuneRate = iSkillValue;
		break;
	case 593:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncSetItemDefense = iSkillValue;
		break;
	case 594:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsReturnEnemyAttack = iSkillValue;
		break;
	case 595:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncEnergyStat = iSkillValue;
		break;
	case 596:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncVitalStat = iSkillValue;
		break;
	case 597:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncDexStat = iSkillValue;
		break;
	case 598:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncPowerStat = iSkillValue;
		break;
	case 610:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncCriticalDamageRate = iSkillValue;
		break;
	case 611:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsRecoverManaRate = iSkillValue;
		break;
	case 612:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsRecoverHPRate = iSkillValue;
		break;
	case 613:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncExcellentDamageRate = iSkillValue;
		break;
	case 614:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncDoubleDamageRate = iSkillValue;
		break;
	case 615:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncIgnoreEnemyBlock = iSkillValue;
		break;
	case 616:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsRecoverSDRate = iSkillValue;
		break;
	case 617:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncTripleDamageRate = iSkillValue;
		break;
	case 623:
	case 634:
	case 643:
	case 653:
	case 663:
	case 670:
	case AT_MSKILL_RF_677:
	case AT_MSKILL_GL_713:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsShieldBlockRate = iSkillValue;
		break;
	case AT_MSKILL_GL_712:
		Obj.m_PlayerData->m_MPSkillOpt.iMpsRageDamage = iSkillValue;
		break;
	default:
		break;
	}

	gObjCalCharacter.CalcCharacter(Obj.m_Index);
}

int CMasterLevelSkillTreeSystem::CheckSkillCondition(CGameObject &Obj, int iMLSkill, int iSkillLevel)
{
	if (gObjIsConnected(lpObj) == FALSE)
	{
		gGameProtocol.GCResultSend(Obj.m_Index, 0x51, 0x03);
		return 2;
	}

	if (iMLSkill < 0 || iMLSkill > MAX_SKILL - 1)
	{
		return 3;
	}

	if (this->IsMasterLevelUser(lpObj) == false)
	{
		return 2;
	}

	if (this->CheckMasterLevelSkill(iMLSkill) == false)
	{
		return 4;
	}

	if (this->GetMaxPointOfMasterSkill(Obj.Class, iMLSkill) < iSkillLevel)
	{
		return 4;
	}

	int iReqMLPoint = this->GetRequireMLPoint(Obj.Class, iMLSkill);

	if (iReqMLPoint < 0)
	{
		return 4;
	}

	if (Obj.m_PlayerData->MasterPoint < iReqMLPoint)
	{
		return 4;
	}

	if (iSkillLevel != 1)
	{
		return 1;
	}

	if (this->CheckCompleteBrandOfSkill(lpObj, iMLSkill) == false)
	{
		return 4;
	}

	if (this->CheckPreviousRankSkill(lpObj, iMLSkill, 1) == false)
	{
		return 4;
	}

	return 1;
}

BOOL CMasterLevelSkillTreeSystem::CheckCompleteBrandOfSkill(CGameObject &Obj, int iMLSkill)
{
	if (MagicDamageC.GetSkillUseType(iMLSkill) == 4)
	{
		int iBrandOfSkill = MagicDamageC.GetBrandOfSkill(iMLSkill);
		BOOL iCompleteSkill = FALSE;

		if (iBrandOfSkill > 0)
		{
			for (int iCount = 0; iCount < MAX_MAGIC; iCount++)
			{
				if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == iBrandOfSkill)
				{
					iCompleteSkill = TRUE;
					break;
				}
			}
		}

		if (iCompleteSkill == FALSE)
		{
			return FALSE;
		}
	}

	std::pair<int, int>iBrandOfSkill = std::pair<int, int>(this->GetParentSkill1(Obj.Class, iMLSkill), this->GetParentSkill2(Obj.Class, iMLSkill));

	if (iBrandOfSkill.first <= 0)
	{
		return TRUE;
	}

	BOOL iCompleteSkill1 = FALSE;
	BOOL iCompleteSkill2 = FALSE;

	if (iBrandOfSkill.first > 0)
	{
		for (int i = 0; i<MAX_MAGIC; i++)
		{
			if (Obj.Magic[i].IsMagic() == TRUE &&
				Obj.Magic[i].m_Skill == iBrandOfSkill.first &&
				Obj.Magic[i].m_Level >= 10)
			{
				iCompleteSkill1 = TRUE;
				break;
			}
		}
	}

	if (iBrandOfSkill.second > 0)
	{
		for (int i = 0; i<MAX_MAGIC; i++)
		{
			if (Obj.Magic[i].IsMagic() == TRUE &&
				Obj.Magic[i].m_Skill == iBrandOfSkill.second &&
				Obj.Magic[i].m_Level >= 10)
			{
				iCompleteSkill2 = TRUE;
				break;
			}
		}
	}

	if ((iBrandOfSkill.first + iBrandOfSkill.second) == iBrandOfSkill.first)
	{
		if (iCompleteSkill1 == TRUE)
		{
			return TRUE;
		}
	}

	else if ((iBrandOfSkill.first + iBrandOfSkill.second) == iBrandOfSkill.second)
	{
		if (iCompleteSkill2 == TRUE)
		{
			return TRUE;
		}
	}

	else if (iCompleteSkill1 == TRUE && iCompleteSkill2 == TRUE)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CMasterLevelSkillTreeSystem::CheckMasterLevelSkill(int iMLSkill)
{
	int iSkillUseType = MagicDamageC.GetSkillUseType(iMLSkill);

	return iSkillUseType == 3 || iSkillUseType == 4;
}

BOOL CMasterLevelSkillTreeSystem::IsExistMLSBasedSkill(CGameObject &Obj, int iSkill)
{
	for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
	{
		if (Obj.Magic[iCount].IsMagic() == TRUE &&
			this->CheckMasterLevelSkill(Obj.Magic[iCount].m_Skill) == true &&
			iSkill == MagicDamageC.GetBrandOfSkill(Obj.Magic[iCount].m_Skill))
		{
			return TRUE;
		}
	}

	return FALSE;
}

int CMasterLevelSkillTreeSystem::GetBaseMasterLevelSkill(int iMLSkill)
{
	if (iMLSkill < 0 || iMLSkill > MAX_SKILL - 1)
	{
		return -1;
	}

	if (this->CheckMasterLevelSkill(iMLSkill) == true)
	{
		return iMLSkill;
	}

	return -1;
}

int CMasterLevelSkillTreeSystem::gObjMagicAdd_MLS(CGameObject lpObj, int iMLSkill, int iSkillLevel)
{
	if (g_EnergyCheckOff == TRUE)
	{
		int reqeng = MagicDamageC.SkillGetRequireEnergy(iMLSkill);

		if (reqeng < 0)
		{
			sLog->outBasic("error-L2: Skill energy not enough");
			return -1;
		}

		if (Obj.Class == CLASS_KNIGHT)
			reqeng -= 10;
		if (Obj.Class == CLASS_ELF)
			reqeng -= 5;
		if (Obj.Class == CLASS_DARKLORD)
			reqeng -= 5;

		if ((Obj.m_PlayerData->Energy + Obj.AddEnergy) < reqeng)
		{
			sLog->outBasic("error-L2: Skill energy not enough");
			return -1;
		}
	}

	if (MagicDamageC.SkillGetRequireClass(Obj.Class, Obj.m_PlayerData->ChangeUP, iMLSkill) < 1 && Obj.Type == OBJ_USER && Obj.m_PlayerData->ISBOT == false)
	{
		sLog->outBasic("error-L2: His class is not allowed to use the skill [ID: %d]", iMLSkill);
		return -1;
	}

	for (int n = 0; n<MAX_MAGIC; n++)
	{
		if (Obj.Magic[n].IsMagic() == TRUE && Obj.Magic[n].m_Skill == iMLSkill && Obj.Magic[n].m_Level == iSkillLevel)
		{
			sLog->outBasic("Same skill already exists %d %d", Obj.Magic[n].m_Skill, iMLSkill);
			return -1;
		}
	}

	if (MagicDamageC.GetSkillUseType(iMLSkill) == 3)
	{
		if (iSkillLevel > 1)
		{
			for (int n = 0; n<MAX_MAGIC; n++)
			{
				if (Obj.Magic[n].IsMagic() == TRUE && Obj.Magic[n].m_Skill == iMLSkill && Obj.Magic[n].m_Level == iSkillLevel - 1)
				{
					if (Obj.Magic[n].UpdateMasterSkill(iMLSkill, iSkillLevel) < 0)
					{
						return -1;
					}

					else
					{
						return n;
					}
				}
			}
		}
	}

	else if (MagicDamageC.GetSkillUseType(iMLSkill) == 4)
	{
		int iBrand = MagicDamageC.GetBrandOfSkill(iMLSkill);

		for (int n = 0; n<MAX_MAGIC; n++)
		{
			if (Obj.Magic[n].IsMagic() == TRUE)
			{
				if (Obj.Magic[n].m_Skill == iBrand)
					Obj.Magic[n].m_Pass = 1;
				if (Obj.Magic[n].m_Skill == iMLSkill)
				{
					if (Obj.Magic[n].UpdateMasterSkill(iMLSkill, iSkillLevel) < 0)
					{
						return -1;
					}

					else
					{
						return n;
					}
				}
			}
		}
	}

	else
	{
		return -1;
	}

	for (int n = 0; n<MAX_MAGIC; n++)
	{
		if (Obj.Magic[n].IsMagic() == FALSE)
		{
			if (Obj.Magic[n].Set(iMLSkill, iSkillLevel) >= 0)
			{
				Obj.MagicCount++;

				if (MagicDamageC.GetSkillUseType(iMLSkill) == 4 && !MagicDamageC.GetBrandOfSkill(iMLSkill))
				{
					gGameProtocol.GCMagicListOneSend(Obj.m_Index, n, Obj.Magic[n].m_Skill, Obj.Magic[n].m_Level, 0, 0);
				}

				return n;
			}

			else
			{
				sLog->outBasic("Same magic already exists %s %d", __FILE__, __LINE__);
				return -1;
			}
		}
	}

	sLog->outBasic("No space to add magic %s %d", __FILE__, __LINE__);
	return -1;
}

void CMasterLevelSkillTreeSystem::CGReqGetMasterLevelSkillTree(CGameObject &Obj)
{
	if (gObjIsConnected(Obj.m_Index) == FALSE)
	{
		gGameProtocol.GCSendDisableReconnect(Obj.m_Index);
		//IOCP.CloseClient(Obj.m_Index); // lol
		return;
	}

	BYTE sendbuf[3072];
	PMSG_MASTERLEVELSKILLTREE_COUNT pCountMsg;
	PMSG_MASTERLEVELSKILLTREE pElementMsg;
	

	PHeadSubSetW((BYTE*)&pCountMsg, 0xF3, 0x53, sizeof(pCountMsg));

	int nCount = 0;
	memset(sendbuf, 0x00, sizeof(sendbuf));
	int lOfs = 12;

	for (int i = 0; i<MAX_MAGIC; i++)
	{
		if (Obj.Magic[i].IsMagic() == TRUE && this->CheckMasterLevelSkill(Obj.Magic[i].m_Skill) == true)
		{
			pElementMsg.btMasterSkillUIIndex = this->GetMasterSkillUIIndex(Obj.Class, Obj.Magic[i].m_Skill);
			pElementMsg.btMasterSkillLevel = Obj.Magic[i].m_Level;

			if (this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[i].m_Skill), Obj.Magic[i].m_Level) <= 0.0)
			{
				pElementMsg.fMasterSkillCurValue = -1.0;
			}

			else
			{
				pElementMsg.fMasterSkillCurValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[i].m_Skill), Obj.Magic[i].m_Level);
			}

			if (this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[i].m_Skill), Obj.Magic[i].m_Level + 1) <= 0.0)
			{
				pElementMsg.fMasterSkillNextValue = -1.0;
			}

			else
			{
				pElementMsg.fMasterSkillNextValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[i].m_Skill), Obj.Magic[i].m_Level + 1);
			}

			nCount++;
			std::memcpy(&sendbuf[lOfs], &pElementMsg, sizeof(pElementMsg));
			lOfs += sizeof(pElementMsg);
		}
	}

	pCountMsg.count = nCount;
	pCountMsg.h.sizeH = HIBYTE(lOfs);
	pCountMsg.h.sizeL = LOBYTE(lOfs);

	std::memcpy(sendbuf, &pCountMsg, sizeof(pCountMsg));

	IOCP.DataSend(Obj.m_Index, sendbuf, lOfs);
}

void CMasterLevelSkillTreeSystem::CGReqGetMasterLevelSkill(PMSG_REQ_MASTERLEVEL_SKILL *lpMsg, CGameObject &Obj)
{
	if (!gObjIsConnected(Obj.m_Index))
	{
		gGameProtocol.GCSendDisableReconnect(Obj.m_Index);
		//IOCP.CloseClient(Obj.m_Index);
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	PMSG_ANS_MASTERLEVELSKILLTREE pMsg;

	
	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x52, sizeof(pMsg));

	pMsg.btResult = 0;
	pMsg.wMasterLevelPoint = Obj.m_PlayerData->MasterPoint;
	pMsg.dwMasterSkillIndex = -1;
	pMsg.dwMasterSkillLevel = 0;

	if (Obj.m_IfState->use && Obj.m_IfState->type == 1)
	{
		sLog->outBasic("[MasterSkill] [%s][%s] Fail(Use Trade Interface) - Add Magic List", Obj.AccountID, Obj.Name);

		pMsg.btResult = 6;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	int iCurSkillLevel = this->CheckSkillLevel(lpObj, lpMsg->iMasterLevelSkill);
	int iSkillLevel = this->GetRequireMLPoint(Obj.Class, lpMsg->iMasterLevelSkill) + iCurSkillLevel;
	int iResult = this->CheckSkillCondition(lpObj, lpMsg->iMasterLevelSkill, iSkillLevel);

	if (iResult != TRUE)
	{
		sLog->outBasic("[MasterSkill] [%s][%s] Fail - Result:%d, Skill:%d Level:%d MLPoint:%d",
			Obj.AccountID, Obj.Name, iResult, lpMsg->iMasterLevelSkill, iSkillLevel, Obj.m_PlayerData->MasterPoint);
		pMsg.btResult = iResult;
	}

	else
	{
		if (this->gObjMagicAdd_MLS(lpObj, lpMsg->iMasterLevelSkill, iSkillLevel) < 0)
		{
			sLog->outBasic("[MasterSkill] [%s][%s] Fail - Add Magic List, Skill:%d Level:%d", Obj.AccountID, Obj.Name, lpMsg->iMasterLevelSkill, iSkillLevel);
		}

		else
		{
			int iReqMLPoint = this->GetRequireMLPoint(Obj.Class, lpMsg->iMasterLevelSkill);

			if (iReqMLPoint < 0)
			{
				return;
			}

			Obj.m_PlayerData->MasterPoint -= iReqMLPoint;
			Obj.m_PlayerData->MasterPointUsed += iReqMLPoint;

			pMsg.btResult = TRUE;
			pMsg.wMasterLevelPoint = Obj.m_PlayerData->MasterPoint;
			pMsg.btMasterSkillUIIndex = this->GetMasterSkillUIIndex(Obj.Class, lpMsg->iMasterLevelSkill);
			pMsg.dwMasterSkillIndex = lpMsg->iMasterLevelSkill;
			pMsg.dwMasterSkillLevel = iSkillLevel;

			if (this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMsg->iMasterLevelSkill), iSkillLevel) <= 0.0)
			{
				pMsg.fMasterSkillCurValue = -1.0;
			}

			else
			{
				pMsg.fMasterSkillCurValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMsg->iMasterLevelSkill), iSkillLevel);
			}

			if (this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMsg->iMasterLevelSkill), iSkillLevel + 1) <= 0.0)
			{
				pMsg.fMasterSkillNextValue = -1.0;
			}

			else
			{
				pMsg.fMasterSkillNextValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMsg->iMasterLevelSkill), iSkillLevel + 1);
			}

			this->CalcPassiveSkillData(lpObj, lpMsg->iMasterLevelSkill, iSkillLevel);

			//sLog->outBasic("[MasterSkill] [%s][%s] Success - Add Magic List, Skill:%d Level:%d MLPoint:%d",Obj.AccountID, Obj.Name, lpMsg->iMasterLevelSkill, iSkillLevel, Obj.m_PlayerData->MasterPoint);
		}
	}

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}

BOOL CMasterLevelSkillTreeSystem::RunningSkill_MLS(CGameObject lpObj, int aTargetIndex, CMagicInf *lpMagic, BOOL bCombo, BYTE x, BYTE y, BYTE aTargetPos)
{
	if (!lpObj || !lpMagic)
	{
		return FALSE;
	}

	if (this->m_Lua.GetLua() == NULL)
	{
		return FALSE;
	}

	CGameObject lpTargetObj = &getGameObject(aTargetIndex);

	switch (lpMagic->m_Skill)
	{
	case AT_MSKILL_DW_MANASHIELD1:
		this->MLS_WizardMagicDefense(lpObj, aTargetIndex, lpMagic);
		return 1;
	case 413:
		this->MLS_Healing(lpObj, aTargetIndex, lpMagic);
		return 1;
	case 330:
	case 332:
	case 481:
		this->MLS_Wheel(lpObj, lpMagic, aTargetIndex, bCombo);
		return 1;
	case AT_MSKILL_DK_SWELLLIFE1:
		this->MLS_KnightSkillAddLife(lpObj, lpMagic);
		return 1;
	case AT_MSKILL_DK_SWELLLIFE2:
		this->MLS_SkillFulKnightSkillAddLife(lpObj, lpMagic);
		return 1;
	case AT_MSKILL_DK_SWELLLIFE3:
		this->MLS_MasteryKnightSkillAddLife(lpObj, lpMagic);
		break;
	case 512:
	case 516:
		this->MLS_DarkHorseAttack(lpObj, aTargetIndex, lpMagic);
		return 1;
	case 508:
	case 514:
		this->MLS_FireBurst(lpObj, aTargetIndex, lpMagic);
		return 1;
	case 391:
		this->MLS_SuddenIce(Obj.m_Index, lpMagic, x, y, aTargetIndex);
		return 1;
	case 336:
		this->MLS_KnightBlow(Obj.m_Index, aTargetIndex, lpMagic, bCombo);
		return 1;
	case 331:
	case 333:
		this->MLS_SkillBlowOfFury(Obj.m_Index, lpMagic, x, y, aTargetIndex, bCombo);
		return 1;
	case 417:
		this->MLS_SkillDefense(Obj.m_Index, aTargetIndex, lpMagic);
		return 1;
	case 420:
		this->MLS_SkillAttack(Obj.m_Index, aTargetIndex, lpMagic);
		return 1;
	case 482:
		this->MLS_PowerSlash(Obj.m_Index, lpMagic, x, y, aTargetPos, aTargetIndex);
		return 1;
	case 454:
		this->MLS_SkillSleep(Obj.m_Index, aTargetIndex, lpMagic);
		return 1;
	case 455:
		this->MLS_SkillChainLightning(Obj.m_Index, aTargetIndex, lpMagic);
		return 1;
	case 456:
		this->MLS_SkillLightningShock(Obj.m_Index, aTargetIndex, lpMagic);
		return 1;
	case 458:
		this->MLS_SkillDrainLife(Obj.m_Index, aTargetIndex, lpMagic);
		return 1;
	case 326:
		this->MLS_SkillSword4(lpObj, lpMagic, aTargetIndex, bCombo);
		return 1;
	case 479:
		this->MLS_SkillSword4_Magumsa(lpObj, lpMagic, aTargetIndex, 0);
		return 1;
	case 327:
		this->MLS_SkillSword5(lpObj, lpMagic, aTargetIndex, bCombo);
		return 1;
	case 328:
		this->MLS_SkillSword1(lpObj, lpMagic, aTargetIndex, bCombo);
		return 1;
	case AT_MSKILL_DK_LUNGE1:
		this->MLS_SkillSword2(lpObj, lpMagic, aTargetIndex, bCombo);
		return 1;
	case 337:
		this->MLS_SkillStrengthenDesruction(lpObj, lpMagic, x, y, aTargetIndex, bCombo);
		return 1;
	case AT_MSKILL_DW_EXPWIZARDRY1:
		this->MLS_SkillStrengthenMagicPower(lpObj, lpMagic, aTargetIndex);
		return 1;
	case AT_MSKILL_DW_EXPWIZARDRY2:
		this->MLS_SkillSkillfulMagicPower(lpObj, lpMagic, aTargetIndex);
		return 1;
	case 379:
	case 480:
		this->MLS_SkillStrengthenThunder(lpObj, lpMagic, aTargetIndex);
		return 1;
	case 384:
		this->MLS_SkillStrengthenPoison(lpObj, lpMagic, aTargetIndex);
		return 1;
	case 387:
		this->MLS_SkillStrengthenExPoison(Obj.m_Index, lpMagic, x, y, aTargetIndex);
		return 1;
	case 389:
	case 489:
		this->MLS_SkillStrengthenIce(lpObj, lpMagic, aTargetIndex);
		return 1;
	case 404:
		this->MLS_SkillWizardMagicDefense(lpObj, lpMagic, aTargetIndex);
		return 1;
	case 422:
		this->MLS_SkillStrenghtenAttack(Obj.m_Index, aTargetIndex, lpMagic);
		return 1;
	case 423:
		this->MLS_SkillStrengthenDefense(Obj.m_Index, aTargetIndex, lpMagic);
		return 1;
	case 441:
		this->MLS_SkillInfinityArrow(Obj.m_Index, aTargetIndex, lpMagic);
		return 1;
	case AT_MSKILL_SUM_BERSERK1:
		this->MLS_SkillStrengthenBerserker(Obj.m_Index, lpMagic);
		return 1;
	case 470:
		this->MLS_SkillSkillfulBerserker(Obj.m_Index, lpMagic);
		return 1;
	case 472:
		this->MLS_SkillMasteryBerserker(Obj.m_Index, lpMagic);
		return 1;
	case 511:
		this->MLS_SkillStrendthenAddCriticalDamage(Obj.m_Index, lpMagic);
		return 1;
	case 515:
		this->MLS_SkillSkillFulAddCriticalDamage(Obj.m_Index, lpMagic);
		return 1;
	case 517:
		this->MLS_SkillMasteryAddCriticalDamage(Obj.m_Index, lpMagic);
		return 1;
	case 509:
		this->MLS_SkillStrengthenSpear(Obj.m_Index, aTargetIndex, lpMagic);
		return 1;
	case 424:
		gObjAttack(lpObj, lpTargetObj, lpMagic, 1, 0, 0, 0, 0, 0);
		return 1;
	case 551:
	case 552:
	case 554:
	case 555:
	case 558:
	case 562:
		this->MLS_SkillMonkBarrageJustOneTarget(Obj.m_Index, lpMagic, aTargetIndex);
		return 1;
	case 559:
	case 563:
		gObjUseSkill.SkillDarkSide(Obj.m_Index, aTargetIndex, lpMagic);
		return 1;
	case 692:
	case 694:
	case 695:
		gObjUseSkill.SkillGrowLancerAttackOneTarget(Obj.m_Index, aTargetIndex, lpMagic);
		return 1;
	case 688:
	case 691:
		gObjUseSkill.SkillGrowLancerAttackOneTarget(Obj.m_Index, aTargetIndex, lpMagic);
		return 1;
	case AT_MSKILL_GL_696:
	case AT_MSKILL_GL_697:
	case AT_MSKILL_GL_698:
		gObjUseSkill.SkillBreche(Obj.m_Index, lpMagic, x, y, aTargetIndex);
		return 1;
	case AT_MSKILL_OBSIDIAN_POWUP: //
		this->SkillObsidianPowUpGrowLancer(lpObj, lpMagic, aTargetIndex);
		return 1;
	case AT_MSKILL_GL_699:
		gObjUseSkill.SkillShiningPeak(Obj.m_Index, lpMagic, aTargetIndex);
		return 1;
	case AT_MSKILL_CIRCLE_SHIELD_POWUP:
		this->SkillCircleShieldGrowLancer(Obj.m_Index, lpMagic, aTargetIndex);
		return 1;
	case AT_MSKILL_CIRCLE_SHIELD_MASTERY:
		this->SkillCircleShieldGrowLancer(Obj.m_Index, lpMagic, aTargetIndex);
		return 1;
	case AT_MSKILL_WRATH_POWUP:
		this->SkillWrathPowUpGrowLancer(Obj.m_Index, lpMagic, aTargetIndex);
		return 1;
	case AT_MSKILL_WRATH_PROFICIENCY:
		this->SkillWrathProficiencyGrowLancer(Obj.m_Index, lpMagic, aTargetIndex);
		return 1;
	case AT_MSKILL_WRATH_MASTERY:
		this->SkillWrathMasteryGrowLancer(Obj.m_Index, lpMagic, aTargetIndex);
		return 1;
	case 631:
		gObjUseSkill.SkillKnightRush(Obj.m_Index, aTargetIndex, lpMagic);
		return 1;
	case 687:
	case 690:
		gObjUseSkill.SkillSpinStep(Obj.m_Index, aTargetIndex, lpMagic);
		return 1;
	case 560:
	case 561:
		this->MLS_SkillStrengthenDragonRoar(Obj.m_Index, lpMagic, getGameObject(aTargetIndex)->X, getGameObject(aTargetIndex)->Y, aTargetIndex, 3, TRUE, 0);
		return 1;
	case 569:
		this->MLS_SkillStrengthenBlockRate(Obj.m_Index, lpMagic);
		return 1;
	case 572:
		this->MLS_SkillMasteryBlockRate(Obj.m_Index, lpMagic);
		return 1;
	case 573:
		this->MLS_SkillStrengthenIncVital(Obj.m_Index, lpMagic);
		return 1;
	case 575:
		this->MLS_SkillStrengthenIgnoreEnemyDefense(Obj.m_Index, lpMagic);
		return 1;
	case 344:
	case 346:
		this->MLS_SkillBloodStorm(lpObj, lpMagic, x, y, aTargetIndex, bCombo);
		return 1;
	case 495:
	case 497:
		this->MLS_SkillEarthPrison(lpObj, aTargetIndex, lpMagic);
		return 1;
	case 427:
	case 434:
		this->MLS_SkillPoisonArrow(lpObj, lpMagic, aTargetIndex, bCombo);
		return 1;
	case 425:
		this->MLS_SkillCure(lpObj, lpMagic, aTargetIndex);
		return 1;
	case 432:
		this->MLS_SkillMonsterCall(Obj.m_Index, 597, Obj.X - 1, Obj.Y + 1);
		return 1;
	case 426:
	case 429:
		this->MLS_SkillPartyHealing(Obj.m_Index, aTargetIndex, lpMagic);
		return 1;
	case 430:
		this->MLS_SkillBless(lpObj, aTargetIndex, lpMagic);
		return 1;
	case 433:
		this->MLS_SkillStrengthenBless(lpObj, aTargetIndex, lpMagic);
		return 1;
	case 461:
	case 463:
		this->MLS_SkillBlind(lpObj, aTargetIndex, lpMagic);
		return 1;
	case AT_MSKILL_DK_IRONDEFENSE:
	case AT_MSKILL_DK_IRONDEFENSE1:
		this->SkillIronDefenseDK(lpObj, aTargetIndex, lpMagic);
		return 1;
	case AT_MSKILL_DL_IRONDEFENSE:
	case AT_MSKILL_DL_IRONDEFENSE1:
		this->SkillIronDefenseDL(lpObj, aTargetIndex, lpMagic);
		return 1;
	case 565:
		this->MLS_SkillBloodHowling(lpObj, aTargetIndex, lpMagic);
		return 1;
	case 339:
		this->MLS_SkillfulKnightBlow(Obj.m_Index, aTargetIndex, lpMagic, bCombo);
		return 1;
	case 342:
		this->MLS_MasteryKnightBlow(Obj.m_Index, aTargetIndex, lpMagic, bCombo);
		return 1;
	case 340:
		this->MLS_SkillSkillfulDesruction(lpObj, lpMagic, x, y, aTargetIndex, bCombo);
		return 1;
	case 343:
		this->MLS_SkillMasteryDesruction(lpObj, lpMagic, x, y, aTargetIndex, bCombo);
		return 1;
	case 393:
		this->MLS_MasterySuddenIce(Obj.m_Index, lpMagic, x, y, aTargetIndex);
		return 1;
	case 392:
		this->MLS_SkillHellBust(Obj.m_Index, aTargetIndex, lpMagic);
		return 1;
	case 390:
	case 394:
		gObjAttack(lpObj, lpTargetObj, lpMagic, 1, 0, 0, 0, 0, 0);
		return 1;
	case 411:
	case 431:
		this->MLS_SkillStrengthenMultiShot(Obj.m_Index, lpMagic, aTargetPos, aTargetIndex);
		return 1;
	case 406:
		this->MLS_SkillMasteryWizardMagicDefense(lpObj, lpMagic, aTargetIndex);
		break;
	case 459:
		this->MLS_SkillStrengthenWeakness(Obj.m_Index, lpMagic, x, y, aTargetIndex);
		return 1;
	case 460:
		this->MLS_SkillStrengthenEnervation(Obj.m_Index, lpMagic, x, y, aTargetIndex);
		return 1;
	case 462:
		this->MLS_SkillDrainLife(Obj.m_Index, aTargetIndex, lpMagic);
		return 1;
	case 491:
		this->MLS_SkillMasteryIce(lpObj, lpMagic, aTargetIndex);
		return 1;
	case 492:
	case 494:
		this->MLS_SkillStrengthenFlameStrike(Obj.m_Index, lpMagic, aTargetPos, aTargetIndex);
		return 1;
	case 496:
		this->MLS_SkillGiganticStorm(Obj.m_Index, lpMagic, x, y, aTargetIndex);
		return 1;
	case 519:
		this->MLS_SkillStrengthenElectricSpark(Obj.m_Index, lpMagic, x, y, aTargetPos, aTargetIndex);
		return 1;
	case AT_MSKILL_DL_CRITICALDAMAGE4:
		this->MLS_SkillMasteryAddCriticalDamage2(Obj.m_Index, lpMagic);
		return 1;
	case 523:
		this->MLS_SkillStrengthenchaoticDesair(Obj.m_Index, lpMagic, aTargetPos, aTargetIndex);
		return 1;
	case 564:
	case 566:
		gObjAttack(lpObj, lpTargetObj, lpMagic, 1, 0, 0, 0, 0, 0);
		return 1;
	case 567:
		this->MLS_SkillStrengthenBloodHowling(lpObj, aTargetIndex, lpMagic);
		return 1;
	case AT_MSKILL_BURST:
	case AT_MSKILL_BURST_POWUP:
		this->SkillBurstMasteryGrowLancer(Obj.m_Index, lpMagic, aTargetIndex);
		return 1;
	default:
		return 1;
	}

	return TRUE;
}

void CMasterLevelSkillTreeSystem::MLS_WizardMagicDefense(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic)
{
	CGameObject lpTargetObj = &getGameObject(aTargetIndex);

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.m_RecallMon != -1)
	{
		return;
	}

	if (Obj.Class != CLASS_WIZARD && Obj.Class != CLASS_MAGUMSA)
	{
		return;
	}

	if (Obj.PartyNumber != lpTargetObj.PartyNumber)
	{
		return;
	}

	double fEffectValue = 0.0;
	int iSkillTime = 0;

	this->m_Lua.Generic_Call("WizardMagicDefense_Level1", "iiii>di", Obj.m_Index, aTargetIndex, Obj.m_PlayerData->Dexterity + Obj.AddDexterity, Obj.AddEnergy + Obj.m_PlayerData->Energy, &fEffectValue, &iSkillTime);

	float fSkillValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);

	gObjAddBuffEffect(lpTargetObj, BUFFTYPE_MELEE_DEFENSE_INC, EFFECTTYPE_IMPROVE_MELEE_DEFENSE, fEffectValue + fSkillValue, EFFECTTYPE_MELEE_DEFENSE_DOWN_MANA, g_SkillAdditionInfo.GetSoulBarrierManaRate_Level(lpMagic->m_Level), iSkillTime);
	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, TRUE);
}

void CMasterLevelSkillTreeSystem::MLS_Healing(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic)
{
	CGameObject lpTargetObj = &getGameObject(aTargetIndex);

	if (lpTargetObj.Type != OBJ_USER && lpTargetObj.m_RecallMon == -1)
	{
		return;
	}

	if (Obj.Class != CLASS_ELF)
	{
		return;
	}

	if ((CC_MAP_RANGE(Obj.MapNumber) || Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL) && Obj.m_Index != lpTargetObj.m_Index)
	{
		return;
	}

	double fAddLife = 0.0;

	this->m_Lua.Generic_Call("ElfHeal", "iiii>d", lpTargetObj.Class, Obj.m_Index, aTargetIndex, Obj.m_PlayerData->Energy + Obj.AddEnergy, &fAddLife);

	float fSkillValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);

	if (fSkillValue > 0.0)
	{
		fAddLife = ((fAddLife * fSkillValue) / 100.0) + fAddLife;
	}

	lpTargetObj.Life += fAddLife;

	if ((lpTargetObj.MaxLife + lpTargetObj.AddLife) < lpTargetObj.Life)
	{
		lpTargetObj.Life = lpTargetObj.MaxLife + lpTargetObj.AddLife;
	}

	if (lpTargetObj.Type == OBJ_USER)
	{
		gGameProtocol.GCReFillSend(lpTargetObj.m_Index, lpTargetObj.Life, 0xFF, 0, lpTargetObj.iShield);
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, TRUE);

		for (int n = 0; n < MAX_VIEWPORT; n++)
		{
			if (Obj.VpPlayer2[n].type == OBJ_MONSTER && Obj.VpPlayer2[n].state != 0)
			{
				CGameObject lpMonster = &getGameObject(Obj.VpPlayer2[n]->number);

				if (lpMonster->m_iCurrentAI != 0)
				{
					lpMonster->m_Agro->IncAgro(Obj.m_Index, (Obj.m_PlayerData->Energy / 5) / 40);
				}
			}
		}
	}

	else if (lpTargetObj.m_RecallMon >= 0)
	{
		gGameProtocol.GCRecallMonLife(lpTargetObj.m_RecallMon, lpTargetObj.MaxLife, lpTargetObj.Life);
	}
}

void CMasterLevelSkillTreeSystem::MLS_Wheel(CGameObject &Obj, CMagicInf *lpMagic, int aTargetIndex, BOOL isCombo)
{
	int count = 0;
	int DuelIndex = Obj.m_iDuelUser;
	int tObjNum;
	BOOL EnableAttack;

	while (true)
	{
		if (Obj.VpPlayer2[count].state != 0)
		{
			tObjNum = Obj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = FALSE;

				if (Obj.VpPlayer2[count].type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon < 0)
				{
					EnableAttack = TRUE;
				}

				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = TRUE;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && Obj.VpPlayer2[count].type != OBJ_NPC && getGameObject(tObjNum)->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = TRUE;
				}

				else
				{
					int CallMonIndex = getGameObject(tObjNum)->m_Index;

					if (getGameObject(tObjNum)->Type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon >= 0)
					{
						CallMonIndex = getGameObject(getGameObject(tObjNum)->m_RecallMon)->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &getGameObject(CallMonIndex)) == TRUE)
					{
						EnableAttack = TRUE;
					}

					if (getGameObject(tObjNum)->Class >= 678 && getGameObject(tObjNum)->Class <= 680)
					{
						EnableAttack = TRUE;
					}
				}

				if (EnableAttack == TRUE)
				{
					if (gObjCalDistance(lpObj, &getGameObject(tObjNum)) <= MagicDamageC.GetSkillDistance(lpMagic->m_Skill))
					{
						gObjAttack(lpObj, &getGameObject(tObjNum), lpMagic, FALSE, TRUE, FALSE, isCombo, FALSE, FALSE);
					}
				}
			}
		}

		count++;

		if (count > MAX_VIEWPORT - 1)
			break;
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillSword4(CGameObject &Obj, CMagicInf *lpMagic, int aTargetIndex, BOOL isCombo)
{
	CGameObject lpTargetObj = &getGameObject(aTargetIndex);

	if (gObjIsConnected(Obj.m_Index) == FALSE && gObjIsConnected(lpTargetObj.m_Index) == FALSE)
	{
		return;
	}

	if (Obj.Type != OBJ_USER || Obj.Class != CLASS_KNIGHT)
	{
		return;
	}

	gObjAttack(lpObj, lpTargetObj, lpMagic, 0, 1, 0, isCombo, 0, 0);
}

void CMasterLevelSkillTreeSystem::MLS_SkillSword4_Magumsa(CGameObject &Obj, CMagicInf *lpMagic, int aTargetIndex, BOOL isCombo)
{
	CGameObject lpTargetObj = &getGameObject(aTargetIndex);

	if (gObjIsConnected(Obj.m_Index) == FALSE && gObjIsConnected(lpTargetObj.m_Index) == FALSE)
	{
		return;
	}

	if (Obj.Type != OBJ_USER || Obj.Class != CLASS_MAGUMSA)
	{
		return;
	}

	gObjAttack(lpObj, lpTargetObj, lpMagic, 0, 1, 0, isCombo, 0, 0);
}

void CMasterLevelSkillTreeSystem::MLS_SkillSword5(CGameObject &Obj, CMagicInf *lpMagic, int aTargetIndex, BOOL isCombo)
{
	CGameObject lpTargetObj = &getGameObject(aTargetIndex);

	if (gObjIsConnected(Obj.m_Index) == FALSE && gObjIsConnected(lpTargetObj.m_Index) == FALSE)
	{
		return;
	}

	if (Obj.Type != OBJ_USER || Obj.Class != CLASS_KNIGHT)
	{
		return;
	}

	gObjAttack(lpObj, lpTargetObj, lpMagic, 0, 1, 0, isCombo, 0, 0);
}

void CMasterLevelSkillTreeSystem::MLS_SkillSword1(CGameObject &Obj, CMagicInf *lpMagic, int aTargetIndex, BOOL isCombo)
{
	CGameObject lpTargetObj = &getGameObject(aTargetIndex);

	if (gObjIsConnected(Obj.m_Index) == FALSE && gObjIsConnected(lpTargetObj.m_Index) == FALSE)
	{
		return;
	}

	if (Obj.Type != OBJ_USER || Obj.Class != CLASS_KNIGHT)
	{
		return;
	}

	gObjAttack(lpObj, lpTargetObj, lpMagic, 0, 1, 0, isCombo, 0, 0);
}

void CMasterLevelSkillTreeSystem::MLS_SkillSword2(CGameObject &Obj, CMagicInf *lpMagic, int aTargetIndex, BOOL isCombo)
{
	CGameObject lpTargetObj = &getGameObject(aTargetIndex);

	if (gObjIsConnected(Obj.m_Index) == FALSE && gObjIsConnected(lpTargetObj.m_Index) == FALSE)
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	gObjAttack(lpObj, lpTargetObj, lpMagic, 0, 1, 0, isCombo, 0, 0);
}

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenDesruction(CGameObject &Obj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex, int isCombo)
{
	int count = 0;
	int DuelIndex = Obj.m_iDuelUser;
	int nHitCount = 0;
	BOOL bAttack = FALSE;
	BOOL EnableAttack;
	int tObjNum;

	if (this->MLS_CalDistance(Obj.X, Obj.Y, x, y) <= MagicDamageC.GetSkillDistance(lpMagic->m_Skill))
	{
		while (true)
		{
			if (Obj.VpPlayer2[count].state != 0)
			{
				tObjNum = Obj.VpPlayer2[count].number;

				if (tObjNum >= 0)
				{
					EnableAttack = FALSE;

					if (Obj.VpPlayer2[count].type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon < 0)
					{
						EnableAttack = TRUE;
					}

					else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
					{
						EnableAttack = TRUE;
					}

					else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && Obj.VpPlayer2[count].type != OBJ_NPC && getGameObject(tObjNum)->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
					{
						EnableAttack = TRUE;
					}

					else
					{
						int CallMonIndex = getGameObject(tObjNum)->m_Index;

						if (getGameObject(tObjNum)->Type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon >= 0)
						{
							CallMonIndex = getGameObject(getGameObject(tObjNum)->m_RecallMon)->m_Index;
						}

						if (gObjTargetGuildWarCheck(lpObj, &getGameObject(CallMonIndex)) == TRUE)
						{
							EnableAttack = TRUE;
						}

						if (getGameObject(tObjNum)->Class >= 678 && getGameObject(tObjNum)->Class <= 680)
						{
							EnableAttack = TRUE;
						}
					}

					if (EnableAttack == TRUE)
					{
						if (getGameObject(tObjNum)->X >= x - 3
							&& getGameObject(tObjNum)->X <= x + 3
							&& getGameObject(tObjNum)->Y >= y - 3
							&& getGameObject(tObjNum)->Y <= y + 3)
						{
							getGameObject(tObjNum)->lpAttackObj = lpObj;
							nHitCount++;

							if (nHitCount <= 4)
							{
								bAttack = TRUE;
							}

							else if (rand() % 2)
							{
								bAttack = TRUE;
							}

							if (nHitCount > 8)
							{
								bAttack = FALSE;
							}

							if (bAttack == TRUE)
							{
								gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, 500, lpMagic->m_Skill, 0);
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
}

void CMasterLevelSkillTreeSystem::MLS_KnightSkillAddLife(CGameObject &Obj, CMagicInf * lpMagic)
{
	if (Obj.Type != OBJ_USER && Obj.m_RecallMon == -1)
	{
		return;
	}

	if (Obj.Class != CLASS_KNIGHT && Obj.Class != CLASS_MAGUMSA)
	{
		return;
	}

	int number;
	int partynum = 0;
	int totallevel = 0;
	int partycount;
	int dis;

	int ApplyPartyIndex[MAX_USER_IN_PARTY];

	int viewplayer = 0;
	int viewpercent = 0;

	CGameObject lpPartyObj;

	memset(ApplyPartyIndex, -1, sizeof(ApplyPartyIndex));

	partynum = Obj.PartyNumber;

	if (partynum != -1)
	{
		partycount = gParty.m_PartyS[partynum].Count;

		for (int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			number = gParty.m_PartyS[partynum].Number[n];

			if (number >= 0)
			{
				lpPartyObj = &getGameObject(number);

				if (Obj.MapNumber == lpPartyObj.MapNumber)
				{
					dis = gObjCalDistance(lpObj, &getGameObject(number));

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

	double fAddValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);
	double faddLifepower = 0.0;
	int iLifeTime = 0;

	this->m_Lua.Generic_Call("KnightSkillAddLife_Level1", "iii>di", Obj.m_PlayerData->Vitality + Obj.AddVitality, Obj.m_PlayerData->Energy + Obj.AddEnergy, viewpercent, &faddLifepower, &iLifeTime);

	faddLifepower += fAddValue;

	if (partynum == -1)
	{
		gObjAddBuffEffect(lpObj, BUFFTYPE_HP_INC, EFFECTTYPE_HP, (Obj.MaxLife * faddLifepower / 100.0), 0, 0, iLifeTime);
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, Obj.m_Index, TRUE);
	}

	else
	{
		for (int i = 0; i<MAX_USER_IN_PARTY; i++)
		{
			if (ApplyPartyIndex[i] != -1)
			{
				gObjAddBuffEffect(&getGameObject(ApplyPartyIndex[i]], BUFFTYPE_HP_INC, EFFECTTYPE_HP, (getGameObject(ApplyPartyIndex[i))->MaxLife * faddLifepower / 100.0), 0, 0, iLifeTime);
				gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, ApplyPartyIndex[i], TRUE);
			}
		}
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillFulKnightSkillAddLife(CGameObject &Obj, CMagicInf * lpMagic)
{
	if (Obj.Type != OBJ_USER && Obj.m_RecallMon == -1)
	{
		return;
	}

	if (Obj.Class != CLASS_KNIGHT && Obj.Class != CLASS_MAGUMSA)
	{
		return;
	}

	int number;
	int partynum = 0;
	int totallevel = 0;
	int partycount;
	int dis;

	int ApplyPartyIndex[MAX_USER_IN_PARTY];

	int viewplayer = 0;
	int viewpercent = 0;

	CGameObject lpPartyObj;

	memset(ApplyPartyIndex, -1, sizeof(ApplyPartyIndex));

	partynum = Obj.PartyNumber;

	if (partynum != -1)
	{
		partycount = gParty.m_PartyS[partynum].Count;

		for (int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			number = gParty.m_PartyS[partynum].Number[n];

			if (number >= 0)
			{
				lpPartyObj = &getGameObject(number);

				if (Obj.MapNumber == lpPartyObj.MapNumber)
				{
					dis = gObjCalDistance(lpObj, &getGameObject(number));

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

	double faddLifepower = 0.0;
	int iLifeTime = 0;
	int iPreSkill = this->GetParentSkill1(Obj.Class, lpMagic->m_Skill);

	if (iPreSkill <= 0)
	{
		this->m_Lua.Generic_Call("KnightSkillAddLife_Level2", "iii>di", Obj.m_PlayerData->Vitality + Obj.AddVitality, Obj.m_PlayerData->Energy + Obj.AddEnergy, viewpercent, &faddLifepower, &iLifeTime);
	}

	else
	{
		int iPreLevel = this->GetCurSkillLevel(lpObj, iPreSkill);

		if (iPreLevel > 0)
		{
			this->m_Lua.Generic_Call("KnightSkillAddLife_Level2", "iii>di", Obj.m_PlayerData->Vitality + Obj.AddVitality, Obj.m_PlayerData->Energy + Obj.AddEnergy, viewpercent, &faddLifepower, &iLifeTime);
			faddLifepower += this->GetMasterSkillValue(MagicDamageC.SkillGet(iPreSkill), iPreLevel);
		}
	}

	double faddManapower = 0.0;

	if (lpMagic->m_Skill == 360)
	{
		faddManapower = viewpercent + this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);
	}

	if (partynum == -1)
	{
		gObjAddBuffEffect(lpObj, BUFFTYPE_HP_INC_STR, EFFECTTYPE_HP, (Obj.MaxLife * faddLifepower / 100.0), EFFECTTYPE_MANA, (Obj.MaxMana * faddManapower / 100.0), iLifeTime);
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, Obj.m_Index, TRUE);
	}

	else
	{
		for (int i = 0; i<MAX_USER_IN_PARTY; i++)
		{
			if (ApplyPartyIndex[i] != -1)
			{
				gObjAddBuffEffect(&getGameObject(ApplyPartyIndex[i]], BUFFTYPE_HP_INC_STR, EFFECTTYPE_HP, (getGameObject(ApplyPartyIndex[i]]->MaxLife * faddLifepower / 100.0), EFFECTTYPE_MANA, (getGameObject(ApplyPartyIndex[i))->MaxMana * faddManapower / 100.0), iLifeTime);
				gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, ApplyPartyIndex[i], TRUE);
			}
		}
	}
}

void CMasterLevelSkillTreeSystem::MLS_MasteryKnightSkillAddLife(CGameObject &Obj, CMagicInf * lpMagic)
{
	if (Obj.Type != OBJ_USER && Obj.m_RecallMon == -1)
	{
		return;
	}

	if (Obj.Class != CLASS_KNIGHT && Obj.Class != CLASS_MAGUMSA)
	{
		return;
	}

	int number;
	int partynum = 0;
	int totallevel = 0;
	int partycount;
	int dis;

	int ApplyPartyIndex[MAX_USER_IN_PARTY];

	int viewplayer = 0;
	int viewpercent = 0;

	CGameObject lpPartyObj;

	memset(ApplyPartyIndex, -1, sizeof(ApplyPartyIndex));

	partynum = Obj.PartyNumber;

	if (partynum != -1)
	{
		partycount = gParty.m_PartyS[partynum].Count;

		for (int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			number = gParty.m_PartyS[partynum].Number[n];

			if (number >= 0)
			{
				lpPartyObj = &getGameObject(number);

				if (Obj.MapNumber == lpPartyObj.MapNumber)
				{
					dis = gObjCalDistance(lpObj, &getGameObject(number));

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

	double faddLifepower = 0.0;
	double faddManapower = 0.0;
	int iLifeTime = 0;

	int iPreSkill = this->GetParentSkill1(Obj.Class, lpMagic->m_Skill);

	if (iPreSkill <= 0)
	{
		faddManapower = viewpercent;
	}

	else
	{
		int iPrePreSkill = this->GetParentSkill1(Obj.Class, iPreSkill);

		if (iPrePreSkill <= 0)
		{
			this->m_Lua.Generic_Call("KnightSkillAddLife_Level3", "iii>di", Obj.m_PlayerData->Vitality + Obj.AddVitality, Obj.m_PlayerData->Energy + Obj.AddEnergy, viewpercent, &faddLifepower, &iLifeTime);
		}

		else
		{
			int iPrePreLevel = this->GetCurSkillLevel(lpObj, iPrePreSkill);

			if (iPrePreLevel > 0)
			{
				this->m_Lua.Generic_Call("KnightSkillAddLife_Level3", "iii>di", Obj.m_PlayerData->Vitality + Obj.AddVitality, Obj.m_PlayerData->Energy + Obj.AddEnergy, viewpercent, &faddLifepower, &iLifeTime);
				faddLifepower += this->GetMasterSkillValue(MagicDamageC.SkillGet(iPrePreSkill), iPrePreLevel);
			}

			else
			{
				this->m_Lua.Generic_Call("KnightSkillAddLife_Level3", "iii>di", Obj.m_PlayerData->Vitality + Obj.AddVitality, Obj.m_PlayerData->Energy + Obj.AddEnergy, viewpercent, &faddLifepower, &iLifeTime);
			}
		}

		int iPreLevel = this->GetCurSkillLevel(lpObj, iPreSkill);

		if (iPreLevel > 0)
		{
			faddManapower = viewpercent + this->GetMasterSkillValue(MagicDamageC.SkillGet(iPreSkill), iPreLevel);
		}
	}

	if (partynum == -1)
	{
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncMaxBP = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);
		gObjAddBuffEffect(lpObj, BUFFTYPE_HP_INC_STR, EFFECTTYPE_HP, (Obj.MaxLife * faddLifepower / 100.0), EFFECTTYPE_MANA, (Obj.MaxMana * faddManapower / 100.0), iLifeTime);
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, Obj.m_Index, TRUE);
	}

	else
	{
		for (int i = 0; i<MAX_USER_IN_PARTY; i++)
		{
			if (ApplyPartyIndex[i] != -1)
			{
				getGameObject(ApplyPartyIndex[i])->m_PlayerData->m_MPSkillOpt.iMpsIncMaxBP = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);
				gObjAddBuffEffect(&getGameObject(ApplyPartyIndex[i]], BUFFTYPE_HP_INC_STR, EFFECTTYPE_HP, (getGameObject(ApplyPartyIndex[i]]->MaxLife * faddLifepower / 100.0), EFFECTTYPE_MANA, (getGameObject(ApplyPartyIndex[i))->MaxMana * faddManapower / 100.0), iLifeTime);
				gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, ApplyPartyIndex[i], TRUE);
			}
		}
	}
}

int CMasterLevelSkillTreeSystem::MLS_SkillStrengthenMagicPower(CGameObject &Obj, CMagicInf* lpMagic, int aTargetIndex)
{
	if (Obj.Type != OBJ_USER)
	{
		return false;
	}

	if (Obj.Class != CLASS_WIZARD)
	{
		return false;
	}

	if (gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_MAGIC_POWER_INC_STR) == TRUE)
	{
		return false;
	}

	double SkillEffect = 0.0;
	int Time = 0;

	gObjAddBuffEffect(lpObj, BUFFTYPE_MAGIC_POWER_INC_STR, EFFECTTYPE_MAGICPOWER_INC, ((Obj.m_PlayerData->Energy + Obj.AddEnergy) / 9.0 * 0.20),
		EFFECTTYPE_MAGICPOWERMAX_INC, (Obj.m_MagicDamageMax / 100.0 * this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level)), 1800);

	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, Obj.m_Index, TRUE);
	return true;
}

void CMasterLevelSkillTreeSystem::MLS_SkillSkillfulMagicPower(CGameObject &Obj, CMagicInf* lpMagic, int aTargetIndex)
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.Class != CLASS_WIZARD)
	{
		return;
	}

	if (gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_MAGIC_POWER_INC_MAS) == TRUE)
	{
		return;
	}

	Obj.m_PlayerData->m_MPSkillOpt.iMpsCriticalRateInc = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);
	gObjAddBuffEffect(lpObj, BUFFTYPE_MAGIC_POWER_INC_MAS, EFFECTTYPE_MAGICPOWER_INC, ((Obj.m_PlayerData->Energy + Obj.AddEnergy) / 9.0 * 0.20),
		EFFECTTYPE_MAGICPOWERMAX_INC, (Obj.m_MagicDamageMax / 100.0 * this->GetBrandOfMasterSkillValue(lpObj, lpMagic->m_Skill, 1)), 1800);

	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, Obj.m_Index, TRUE);
	//return true;
}

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenFlame(CGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex)
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	gObjAttack(lpObj, &getGameObject(aTargetIndex), lpMagic, TRUE, 0, 0, 0, 0, 0);
}

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenThunder(CGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex)
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	gObjAttack(lpObj, &getGameObject(aTargetIndex), lpMagic, TRUE, 0, 0, 0, 0, 0);
}

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenInferno(CGameObject &Obj, CMagicInf *lpMagic)
{
	return;
}

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenHellFire(CGameObject lpObj, CMagicInf *lpMagic)
{
	int count = 0;
	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, Obj.m_Index, TRUE);

	while (true)
	{
		if (Obj.VpPlayer2[count].state)
		{
			if (Obj.VpPlayer2[count].type == OBJ_MONSTER)
			{
				int tObjNum = Obj.VpPlayer2[count].number;

				if (tObjNum >= 0)
				{
					if (gObjCalDistance(lpObj, &getGameObject(tObjNum)) < 4)
					{
						gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, 200, lpMagic->m_Skill, 0);
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

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenPoison(CGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex)
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	gObjAttack(lpObj, &getGameObject(aTargetIndex), lpMagic, TRUE, 0, 0, 0, 0, 0);
}

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenExPoison(CGameObject &Obj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex)
{
	

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	int count = 0;
	int DuelIndex = Obj.m_iDuelUser;
	int tObjNum;
	BOOL EnableAttack;

	while (true)
	{
		if (Obj.VpPlayer2[count].state != 0)
		{
			tObjNum = Obj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = FALSE;

				if (Obj.VpPlayer2[count].type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon < 0)
				{
					EnableAttack = TRUE;
				}

				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = TRUE;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && Obj.VpPlayer2[count].type != OBJ_NPC && getGameObject(tObjNum)->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = TRUE;
				}

				else
				{
					int CallMonIndex = getGameObject(tObjNum)->m_Index;

					if (getGameObject(tObjNum)->Type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon >= 0)
					{
						CallMonIndex = getGameObject(getGameObject(tObjNum)->m_RecallMon)->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &getGameObject(CallMonIndex)) == TRUE)
					{
						EnableAttack = TRUE;
					}

					if (getGameObject(tObjNum)->Class >= 678 && getGameObject(tObjNum)->Class <= 680)
					{
						EnableAttack = TRUE;
					}
				}

				if (EnableAttack == TRUE)
				{
					if (this->MLS_CalDistance(x, y, getGameObject(tObjNum)->X, getGameObject(tObjNum)->Y) < 4)
					{
						gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, rand() % 500, lpMagic->m_Skill, 0);
					}
				}
			}
		}

		count++;

		if (count > MAX_VIEWPORT - 1)
			break;
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenIce(CGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex)
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	gObjAttack(lpObj, &getGameObject(aTargetIndex), lpMagic, TRUE, 0, 0, 0, 0, 0);
}

void CMasterLevelSkillTreeSystem::MLS_SkillWizardMagicDefense(CGameObject &Obj, CMagicInf *lpMagic, int aTargetIndex)
{
	CGameObject lpTargetObj = &getGameObject(aTargetIndex);

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.m_RecallMon != -1)
	{
		return;
	}

	if (Obj.Class != CLASS_WIZARD && Obj.Class != CLASS_MAGUMSA)
	{
		return;
	}

	if (Obj.PartyNumber != lpTargetObj.PartyNumber)
	{
		return;
	}

	double fDefenseValue = 0.0;
	int iSkillTime = 0;

	int iPreSkill = this->GetParentSkill1(Obj.Class, lpMagic->m_Skill);

	if (iPreSkill <= 0)
	{
		this->m_Lua.Generic_Call("WizardMagicDefense_Level2", "iiii>di", Obj.m_Index, aTargetIndex, Obj.AddDexterity + Obj.m_PlayerData->Dexterity, Obj.AddEnergy + Obj.m_PlayerData->Energy, &fDefenseValue, &iSkillTime);
	}

	else
	{
		int iPreLevel = this->GetCurSkillLevel(lpObj, iPreSkill);

		if (iPreLevel > 0)
		{
			this->m_Lua.Generic_Call("WizardMagicDefense_Level2", "iiii>di", Obj.m_Index, aTargetIndex, Obj.AddDexterity + Obj.m_PlayerData->Dexterity, Obj.AddEnergy + Obj.m_PlayerData->Energy, &fDefenseValue, &iSkillTime);
			fDefenseValue += this->GetMasterSkillValue(MagicDamageC.SkillGet(iPreSkill), iPreLevel);
		}
	}

	gObjAddBuffEffect(lpTargetObj, BUFFTYPE_MELEE_DEFENSE_INC, EFFECTTYPE_IMPROVE_MELEE_DEFENSE, fDefenseValue,
		EFFECTTYPE_MELEE_DEFENSE_DOWN_MANA, g_SkillAdditionInfo.GetSoulBarrierManaRate_Level(lpMagic->m_Level),
		iSkillTime + this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level));

	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, lpTargetObj.m_Index, TRUE);
}

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenPenetration(CGameObject lpObj, CMagicInf *lpMagic, int aTargetIndex)
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	gObjAttack(lpObj, &getGameObject(aTargetIndex), lpMagic, TRUE, 0, 0, 0, 0, 0);
}

void CMasterLevelSkillTreeSystem::MLS_SkillInfinityArrow(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic)
{
	

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.Class != CLASS_ELF)
	{
		return;
	}

	if (Obj.m_PlayerData->ChangeUP == FALSE)
	{
		return;
	}

	if (Obj.m_Index != aTargetIndex)
	{
		return;
	}

	if (gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_INFINITY_ARROW_STR) == TRUE)
	{
		return;
	}

	Obj.m_SkillInfo.fInfinityArrowIncRate = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);
	gObjAddBuffEffect(lpObj, BUFFTYPE_INFINITY_ARROW_STR, 0, 0, 0, 0, -10);
	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, Obj.m_Index, TRUE);

	sLog->outBasic("[%s][%s] Use Infinity Arrow Skill (Time:%d)(Character Level : %d)(ChangeUp: %d)",
		Obj.AccountID, Obj.Name, g_SkillAdditionInfo.GetInfinityArrowSkillTime(), Obj.Level, Obj.m_PlayerData->ChangeUP);
}

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenBerserker(CGameObject &Obj, CMagicInf *lpMagic)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	

	int iDuration = 0;
	int iEffectUPValue = 0;
	int iEffectDownValue = 0;

	if (Obj.Type == OBJ_USER)
	{
		this->m_Lua.Generic_Call("SummonerBerserker_Level1", "i>iii", Obj.m_PlayerData->Energy + Obj.AddEnergy, &iEffectUPValue, &iEffectDownValue, &iDuration);
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncPercentBerserker1 = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);
	}

	else
	{
		iDuration = 600000;
		iEffectUPValue = 0;
		iEffectDownValue = 0;
	}

	gObjAddBuffEffect(lpObj, BUFFTYPE_BERSERKER_STR, EFFECTTYPE_BERSERKER_UP, iEffectUPValue, EFFECTTYPE_BERSERKER_DOWN, iEffectDownValue, iDuration);
	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, Obj.m_Index, TRUE);
}

void CMasterLevelSkillTreeSystem::MLS_SkillSkillfulBerserker(CGameObject &Obj, CMagicInf *lpMagic)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	lua_Reader();
	

	int iDuration = 0;
	int iEffectUPValue = 0;
	int iEffectDownValue = 0;

	if (Obj.Type == OBJ_USER)
	{
		this->m_Lua.Generic_Call("SummonerBerserker_Level2", "i>iii", Obj.m_PlayerData->Energy + Obj.AddEnergy, &iEffectUPValue, &iEffectDownValue, &iDuration);
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncPercentBerserker1 = this->GetBrandOfMasterSkillValue(lpObj, lpMagic->m_Skill, 1);
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncPercentBerserker2 = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);
	}

	else
	{
		iDuration = 600000;
		iEffectUPValue = 0;
		iEffectDownValue = 0;
	}

	gObjAddBuffEffect(lpObj, BUFFTYPE_BERSERKER_MAS, EFFECTTYPE_BERSERKER_UP, iEffectUPValue, EFFECTTYPE_BERSERKER_DOWN, iEffectDownValue, iDuration);
	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, Obj.m_Index, TRUE);
}

void CMasterLevelSkillTreeSystem::MLS_SkillMasteryBerserker(CGameObject &Obj, CMagicInf *lpMagic)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	

	int iDuration = 0;
	int iEffectUPValue = 0;
	int iEffectDownValue = 0;

	if (Obj.Type == OBJ_USER)
	{
		this->m_Lua.Generic_Call("SummonerBerserker_Level3", "i>iii", Obj.m_PlayerData->Energy + Obj.AddEnergy, &iEffectUPValue, &iEffectDownValue, &iDuration);
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncPercentBerserker1 = this->GetBrandOfMasterSkillValue(lpObj, lpMagic->m_Skill, 2);
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncPercentBerserker2 = this->GetBrandOfMasterSkillValue(lpObj, lpMagic->m_Skill, 1);
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncValueBerserker3 = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);
	}

	else
	{
		iDuration = 600000;
		iEffectUPValue = 0;
		iEffectDownValue = 0;
	}

	gObjAddBuffEffect(lpObj, BUFFTYPE_BERSERKER_PRO, EFFECTTYPE_BERSERKER_UP, iEffectUPValue, EFFECTTYPE_BERSERKER_DOWN, iEffectDownValue, iDuration);
	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, Obj.m_Index, TRUE);
}

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenBlast(CGameObject &Obj, CMagicInf* lpMagic, int aTargetIndex)
{
	if (Obj.Type = OBJ_USER)
	{
		return;
	}

	gObjAttack(lpObj, &getGameObject(aTargetIndex), lpMagic, TRUE, 0, 0, 0, 0, 0);
}

void CMasterLevelSkillTreeSystem::MLS_SkillStrendthenAddCriticalDamage(CGameObject &Obj, CMagicInf* lpMagic)
{
	

	int number;
	int ApplyPartyIndex[MAX_USER_IN_PARTY];
	CGameObject lpPartyObj;
	memset(ApplyPartyIndex, -1, sizeof(ApplyPartyIndex));

	int partynum = Obj.PartyNumber;

	if (partynum != -1)
	{
		for (int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			number = gParty.m_PartyS[partynum].Number[n];

			if (number >= 0)
			{
				lpPartyObj = &getGameObject(number);

				if (Obj.MapNumber == lpPartyObj.MapNumber)
				{
					if (gObjCalDistance(lpObj, &getGameObject(number)) < 10)
					{
						ApplyPartyIndex[n] = lpPartyObj.m_Index;
					}
				}
			}
		}
	}

	int addcriticaldamagevalue = 0;
	int SkillTime = 0;

	this->m_Lua.Generic_Call("DarkLordCriticalDamage_Level1", "ii>ii", Obj.Leadership + Obj.AddLeadership, Obj.m_PlayerData->Energy + Obj.AddEnergy, &addcriticaldamagevalue, &SkillTime);

	addcriticaldamagevalue += this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);

	if (partynum == -1)
	{
		gObjAddBuffEffect(lpObj, BUFFTYPE_CRITICAL_DMG_INC, EFFECTTYPE_CRITICAL_DMG, addcriticaldamagevalue, 0, 0, SkillTime);
		MsgOutput(Obj.m_Index, Lang.GetText(0, 134), SkillTime);
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, Obj.m_Index, TRUE);
	}

	else
	{
		for (int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			if (ApplyPartyIndex[n] != -1)
			{
				lpPartyObj = &getGameObject(ApplyPartyIndex[n]);

				gObjAddBuffEffect(lpPartyObj, BUFFTYPE_CRITICAL_DMG_INC, EFFECTTYPE_CRITICAL_DMG, addcriticaldamagevalue, 0, 0, SkillTime);
				MsgOutput(ApplyPartyIndex[n], Lang.GetText(0, 134), SkillTime);
				gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, lpPartyObj.m_Index, TRUE);
			}
		}
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillSkillFulAddCriticalDamage(CGameObject &Obj, CMagicInf* lpMagic)
{
	

	int number;
	int ApplyPartyIndex[MAX_USER_IN_PARTY];
	CGameObject lpPartyObj;
	memset(ApplyPartyIndex, -1, sizeof(ApplyPartyIndex));

	int partynum = Obj.PartyNumber;

	if (partynum != -1)
	{
		for (int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			number = gParty.m_PartyS[partynum].Number[n];

			if (number >= 0)
			{
				lpPartyObj = &getGameObject(number);

				if (Obj.MapNumber == lpPartyObj.MapNumber)
				{
					if (gObjCalDistance(lpObj, &getGameObject(number)) < 10)
					{
						ApplyPartyIndex[n] = lpPartyObj.m_Index;
					}
				}
			}
		}
	}

	int addcriticaldamagevalue = 0;
	int SkillTime = 0;

	this->m_Lua.Generic_Call("DarkLordCriticalDamage_Level2", "ii>ii", Obj.Leadership + Obj.AddLeadership, Obj.m_PlayerData->Energy + Obj.AddEnergy, &addcriticaldamagevalue, &SkillTime);

	addcriticaldamagevalue += this->GetBrandOfMasterSkillValue(lpObj, lpMagic->m_Skill, 1);
	SkillTime += this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);

	if (partynum == -1)
	{
		gObjAddBuffEffect(lpObj, BUFFTYPE_CRITICAL_DMG_INC, EFFECTTYPE_CRITICAL_DMG, addcriticaldamagevalue, 0, 0, SkillTime);
		MsgOutput(Obj.m_Index, Lang.GetText(0, 134), SkillTime);
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, Obj.m_Index, TRUE);
	}

	else
	{
		for (int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			if (ApplyPartyIndex[n] != -1)
			{
				lpPartyObj = &getGameObject(ApplyPartyIndex[n]);

				gObjAddBuffEffect(lpPartyObj, BUFFTYPE_CRITICAL_DMG_INC, EFFECTTYPE_CRITICAL_DMG, addcriticaldamagevalue, 0, 0, SkillTime);
				MsgOutput(ApplyPartyIndex[n], Lang.GetText(0, 134), SkillTime);
				gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, lpPartyObj.m_Index, TRUE);
			}
		}
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillMasteryAddCriticalDamage(CGameObject &Obj, CMagicInf* lpMagic)
{
	

	int number;
	int ApplyPartyIndex[MAX_USER_IN_PARTY];
	CGameObject lpPartyObj;
	memset(ApplyPartyIndex, -1, sizeof(ApplyPartyIndex));

	int partynum = Obj.PartyNumber;

	if (partynum != -1)
	{
		for (int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			number = gParty.m_PartyS[partynum].Number[n];

			if (number >= 0)
			{
				lpPartyObj = &getGameObject(number);

				if (Obj.MapNumber == lpPartyObj.MapNumber)
				{
					if (gObjCalDistance(lpObj, &getGameObject(number)) < 10)
					{
						ApplyPartyIndex[n] = lpPartyObj.m_Index;
					}
				}
			}
		}
	}

	int addcriticaldamagevalue = 0;
	int SkillTime = 0;

	this->m_Lua.Generic_Call("DarkLordCriticalDamage_Level3", "ii>ii", Obj.Leadership + Obj.AddLeadership, Obj.m_PlayerData->Energy + Obj.AddEnergy, &addcriticaldamagevalue, &SkillTime);

	addcriticaldamagevalue += this->GetBrandOfMasterSkillValue(lpObj, lpMagic->m_Skill, 2);
	SkillTime += this->GetBrandOfMasterSkillValue(lpObj, lpMagic->m_Skill, 1);

	float fRate = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);

	if (partynum == -1)
	{
		gObjAddBuffEffect(lpObj, BUFFTYPE_CRITICAL_DMG_INC_STR, EFFECTTYPE_CRITICAL_DMG, addcriticaldamagevalue, EFFECTTYPE_CRITICALDAMAGE, fRate, SkillTime);
		MsgOutput(Obj.m_Index, Lang.GetText(0, 134), SkillTime);
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, Obj.m_Index, TRUE);
	}

	else
	{
		for (int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			if (ApplyPartyIndex[n] != -1)
			{
				lpPartyObj = &getGameObject(ApplyPartyIndex[n]);

				gObjAddBuffEffect(lpPartyObj, BUFFTYPE_CRITICAL_DMG_INC_STR, EFFECTTYPE_CRITICAL_DMG, addcriticaldamagevalue, EFFECTTYPE_CRITICALDAMAGE, fRate, SkillTime);
				MsgOutput(ApplyPartyIndex[n], Lang.GetText(0, 134), SkillTime);
				gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, lpPartyObj.m_Index, TRUE);
			}
		}
	}
}

void CMasterLevelSkillTreeSystem::MLS_DarkHorseAttack(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic)
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (gObjDarkHorse(lpObj) == FALSE)
	{
		return;
	}

	int count = 0;
	int DuelIndex = Obj.m_iDuelUser;
	int tObjNum;
	BOOL EnableAttack;

	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, Obj.m_Index, TRUE);

	while (true)
	{
		if (Obj.VpPlayer2[count].state != 0)
		{
			tObjNum = Obj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = FALSE;

				if (Obj.VpPlayer2[count].type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon < 0)
				{
					EnableAttack = TRUE;
				}

				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = TRUE;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && Obj.VpPlayer2[count].type != OBJ_NPC && getGameObject(tObjNum)->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = TRUE;
				}

				else
				{
					int CallMonIndex = getGameObject(tObjNum)->m_Index;

					if (getGameObject(tObjNum)->Type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon >= 0)
					{
						CallMonIndex = getGameObject(getGameObject(tObjNum)->m_RecallMon)->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &getGameObject(CallMonIndex)) == TRUE)
					{
						EnableAttack = TRUE;
					}

					if (getGameObject(tObjNum)->Class >= 678 && getGameObject(tObjNum)->Class <= 680)
					{
						EnableAttack = TRUE;
					}
				}

				if (EnableAttack == TRUE)
				{
					if (gObjCalDistance(lpObj, &getGameObject(tObjNum)) < 5)
					{
						gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, 500, lpMagic->m_Skill, 0);
					}
				}
			}
		}

		count++;

		if (count > MAX_VIEWPORT - 1)
			break;
	}
}

void CMasterLevelSkillTreeSystem::MLS_FireBurst(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic)
{
	CGameObject lpTargetObj = &getGameObject(aTargetIndex);
	int count = 0;
	int DuelIndex = Obj.m_iDuelUser;
	int tObjNum;
	BOOL EnableAttack;

	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, TRUE);
	gObjAttack(lpObj, lpTargetObj, lpMagic, 0, TRUE, 0, 0, 0, 0);

	while (true)
	{
		if (Obj.VpPlayer2[count].state != 0)
		{
			tObjNum = Obj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = FALSE;

				if (Obj.VpPlayer2[count].type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon < 0)
				{
					EnableAttack = TRUE;
				}

				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = TRUE;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && Obj.VpPlayer2[count].type != OBJ_NPC && getGameObject(tObjNum)->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = TRUE;
				}

				else
				{
					int CallMonIndex = getGameObject(tObjNum)->m_Index;

					if (getGameObject(tObjNum)->Type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon >= 0)
					{
						CallMonIndex = getGameObject(getGameObject(tObjNum)->m_RecallMon)->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &getGameObject(CallMonIndex)) == TRUE)
					{
						EnableAttack = TRUE;
					}

					if (getGameObject(tObjNum)->Class >= 678 && getGameObject(tObjNum)->Class <= 680)
					{
						EnableAttack = TRUE;
					}
				}

				if (EnableAttack == TRUE)
				{
					if (gObjCalDistance(lpObj, &getGameObject(tObjNum)) < 3)
					{
						gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, (rand() * 17) % 300 + 500, lpMagic->m_Skill, 0);
					}
				}
			}
		}

		count++;

		if (count > MAX_VIEWPORT - 1)
			break;
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenSpear(CGameObject &Obj, int aTargetIndex, CMagicInf *lpMagic)
{
	
	int count = 0;

	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, TRUE);

	if (Obj.SkillLongSpearChange == false)
	{
		gObjAttack(lpObj, &getGameObject(aTargetIndex), lpMagic, 0, 1, 0, 0, 0, 0);
		return;
	}

	int DuelIndex = Obj.m_iDuelUser;
	int iangle = this->MLS_GetAngle(Obj.X, Obj.Y, getGameObject(aTargetIndex)->X, getGameObject(aTargetIndex)->Y);
	int tObjNum;
	BOOL EnableAttack;

	while (true)
	{
		if (Obj.VpPlayer2[count].state != 0)
		{
			tObjNum = Obj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = FALSE;

				if (Obj.VpPlayer2[count].type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon < 0)
				{
					EnableAttack = TRUE;
				}

				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = TRUE;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && Obj.VpPlayer2[count].type != OBJ_NPC && getGameObject(tObjNum)->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = TRUE;
				}

				else
				{
					int CallMonIndex = getGameObject(tObjNum)->m_Index;

					if (getGameObject(tObjNum)->Type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon >= 0)
					{
						CallMonIndex = getGameObject(getGameObject(tObjNum)->m_RecallMon)->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &getGameObject(CallMonIndex)) == TRUE)
					{
						EnableAttack = TRUE;
					}

					if (getGameObject(tObjNum)->Class >= 678 && getGameObject(tObjNum)->Class <= 680)
					{
						EnableAttack = TRUE;
					}
				}

				if (EnableAttack == TRUE)
				{
					if (SkillSpearHitBox.HitCheck(iangle, Obj.X, Obj.Y, getGameObject(tObjNum)->X, getGameObject(tObjNum)->Y) == TRUE)
					{
						gObjAttack(lpObj, &getGameObject(tObjNum), lpMagic, 0, 1, 0, 0, 0, 0);
					}
				}
			}
		}

		count++;

		if (count > MAX_VIEWPORT - 1)
			break;
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillAreaMonsterAttack(CGameObject &Obj, CMagicInf* lpMagic, BYTE x, BYTE y, int aTargetIndex, int aDistance, BOOL bPVP, int nSuccessRate, int nDelay) // function not used
{
	return;
}

void CMasterLevelSkillTreeSystem::MLS_SuddenIce(CGameObject &Obj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex)
{
	

	int count = 0;
	int DuelIndex = Obj.m_iDuelUser;
	int tObjNum;
	BOOL EnableAttack;

	while (true)
	{
		if (Obj.VpPlayer2[count].state != 0)
		{
			tObjNum = Obj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = FALSE;

				if (Obj.VpPlayer2[count].type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon < 0)
				{
					EnableAttack = TRUE;
				}

				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = TRUE;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && Obj.VpPlayer2[count].type != OBJ_NPC && getGameObject(tObjNum)->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = TRUE;
				}

				else
				{
					int CallMonIndex = getGameObject(tObjNum)->m_Index;

					if (getGameObject(tObjNum)->Type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon >= 0)
					{
						CallMonIndex = getGameObject(getGameObject(tObjNum)->m_RecallMon)->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &getGameObject(CallMonIndex)) == TRUE)
					{
						EnableAttack = TRUE;
					}

					if (getGameObject(tObjNum)->Class >= 678 && getGameObject(tObjNum)->Class <= 680)
					{
						EnableAttack = TRUE;
					}
				}

				if (EnableAttack == TRUE)
				{
					if (this->MLS_CalDistance(x, y, getGameObject(tObjNum)->X, getGameObject(tObjNum)->Y) < 4)
					{
						gObjAttack(lpObj, &getGameObject(tObjNum), lpMagic, 1, 1, 0, 0, 0, 0);
					}
				}
			}
		}

		count++;

		if (count > MAX_VIEWPORT - 1)
			break;
	}
}

int CMasterLevelSkillTreeSystem::MLS_CalDistance(int x, int y, int x1, int y1)
{
	if (x == x1 && y == y1)
	{
		return 0;
	}

	float tx = x - x1;
	float ty = y - y1;

	return sqrt(tx*tx + ty*ty);
}

int CMasterLevelSkillTreeSystem::MLS_GetAngle(int x, int y, int tx, int ty)
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

void CMasterLevelSkillTreeSystem::MLS_SkillFrustrum(CGameObject &Obj, float fangle, float tx, float ty)
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

		Obj.fSkillFrustrumX[i] = (int)vFrustrum[i][0] + Obj.X;
		Obj.fSkillFrustrumY[i] = (int)vFrustrum[i][1] + Obj.Y;
	}
}

void CMasterLevelSkillTreeSystem::MLS_KnightBlow(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic, BOOL isCombo)
{
	

	float fangle = this->MLS_GetAngle(Obj.X, Obj.Y, getGameObject(aTargetIndex)->X, getGameObject(aTargetIndex)->Y);

	float fDistance = MagicDamageC.GetSkillDistance(lpMagic->m_Skill) + 1.0;
	this->MLS_SkillFrustrum(Obj.m_Index, fangle, 1.5, fDistance);

	int count = 0;
	int loopcount = 0;
	int DuelIndex = Obj.m_iDuelUser;
	int tObjNum;
	BOOL EnableAttack;

	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, TRUE);

	while (true)
	{
		if (Obj.VpPlayer2[count].state != 0)
		{
			tObjNum = Obj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = FALSE;

				if (Obj.VpPlayer2[count].type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon < 0)
				{
					EnableAttack = TRUE;
				}

				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = TRUE;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && Obj.VpPlayer2[count].type != OBJ_NPC && getGameObject(tObjNum)->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = TRUE;
				}

				else
				{
					int CallMonIndex = getGameObject(tObjNum)->m_Index;

					if (getGameObject(tObjNum)->Type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon >= 0)
					{
						CallMonIndex = getGameObject(getGameObject(tObjNum)->m_RecallMon)->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &getGameObject(CallMonIndex)) == TRUE)
					{
						EnableAttack = TRUE;
					}

					if (getGameObject(tObjNum)->Class >= 678 && getGameObject(tObjNum)->Class <= 680)
					{
						EnableAttack = TRUE;
					}
				}

				if (EnableAttack == TRUE)
				{
					BOOL attackcheck = FALSE;

					if (loopcount)
					{
						if ((rand() % 3) == 0)
						{
							attackcheck = TRUE;
						}
					}

					else
					{
						attackcheck = TRUE;
					}

					if (attackcheck == TRUE)
					{
						if (SkillTestFrustrum(getGameObject(tObjNum)->X, getGameObject(tObjNum)->Y, Obj.m_Index))
						{
							gObjAttack(lpObj, &getGameObject(tObjNum), lpMagic, 0, TRUE, 0, isCombo, 0, 0);
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
				break;
		}
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillBlowOfFury(CGameObject &Obj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex, BOOL isCombo)
{
	
	int count = 0;
	int DuelIndex = Obj.m_iDuelUser;
	int tObjNum;
	BOOL EnableAttack;

	while (true)
	{
		if (Obj.VpPlayer2[count].state != 0)
		{
			tObjNum = Obj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = FALSE;

				if (Obj.VpPlayer2[count].type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon < 0)
				{
					EnableAttack = TRUE;
				}

				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = TRUE;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && Obj.VpPlayer2[count].type != OBJ_NPC && getGameObject(tObjNum)->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = TRUE;
				}

				else
				{
					int CallMonIndex = getGameObject(tObjNum)->m_Index;

					if (getGameObject(tObjNum)->Type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon >= 0)
					{
						CallMonIndex = getGameObject(getGameObject(tObjNum)->m_RecallMon)->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &getGameObject(CallMonIndex)) == TRUE)
					{
						EnableAttack = TRUE;
					}

					if (getGameObject(tObjNum)->Class >= 678 && getGameObject(tObjNum)->Class <= 680)
					{
						EnableAttack = TRUE;
					}
				}

				if (EnableAttack == TRUE)
				{
					if (gObjCalDistance(lpObj, &getGameObject(tObjNum)) <= MagicDamageC.GetSkillDistance(lpMagic->m_Skill))
					{
						gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, 500, lpMagic->m_Skill, isCombo);
					}
				}
			}
		}

		count++;

		if (count > MAX_VIEWPORT - 1)
			break;
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillDefense(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic)
{
	
	CGameObject lpTargetObj = &getGameObject(aTargetIndex);

	if (lpTargetObj.Type != OBJ_USER && lpTargetObj.m_RecallMon == -1)
	{
		return;
	}

	if (Obj.Class != CLASS_ELF)
	{
		return;
	}

	if ((CC_MAP_RANGE(Obj.MapNumber) || Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL) && Obj.m_Index != lpTargetObj.m_Index)
	{
		return;
	}

	double skill_defense = 0.0;
	double skill_time = 0.0;

	this->m_Lua.Generic_Call("ElfDefense_Level1", "iiii>dd", lpTargetObj.Class, Obj.m_Index, aTargetIndex, Obj.m_PlayerData->Energy + Obj.AddEnergy, &skill_defense, &skill_time);

	skill_defense += skill_defense * this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level) / 100.0;

	if (gObjCheckPowerfulEffect(lpTargetObj, BUFFTYPE_DEFENSE_POWER_INC, skill_defense, 0) == TRUE)
	{
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, FALSE);
		return;
	}

	gObjAddBuffEffect(lpTargetObj, BUFFTYPE_DEFENSE_POWER_INC, EFFECTTYPE_IMPROVE_DEFENSE, skill_defense, 0, 0, skill_time);

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (Obj.VpPlayer2[n].type == OBJ_MONSTER && Obj.VpPlayer2[n].state != 0)
		{
			CGameObject lpMonster = &getGameObject(Obj.VpPlayer2[n]->number);
			if (lpMonster->m_iCurrentAI != 0)
			{
				lpMonster->m_Agro->IncAgro(Obj.m_Index, int(skill_defense) / 10);
			}
		}
	}

	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, TRUE);
}

void CMasterLevelSkillTreeSystem::MLS_SkillAttack(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic)
{
	
	CGameObject lpTargetObj = &getGameObject(aTargetIndex);

	if (lpTargetObj.Type != OBJ_USER && lpTargetObj.m_RecallMon == -1)
	{
		return;
	}

	if (Obj.Class != CLASS_ELF)
	{
		return;
	}

	if ((CC_MAP_RANGE(Obj.MapNumber) || Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL) && Obj.m_Index != lpTargetObj.m_Index)
	{
		return;
	}

	double skill_attack = 0.0;
	double skill_time = 0.0;

	this->m_Lua.Generic_Call("ElfAttack_Level1", "iiii>dd", lpTargetObj.Class, Obj.m_Index, aTargetIndex, Obj.m_PlayerData->Energy + Obj.AddEnergy, &skill_attack, &skill_time);

	skill_attack += skill_attack * this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level) / 100.0;

	
	if (g_ConfigRead.UseAllBuffAttack == 0)
	{
		if (gObjCheckPowerfulEffect(lpTargetObj, BUFFTYPE_ATTACK_POWER_INC, skill_attack, 0) == TRUE)
		{
			gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, FALSE);
			return;
		}

		else if (gObjCheckUsedBuffEffect(lpTargetObj, BUFFTYPE_BERSERKER) == TRUE)
		{
			gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, FALSE);
			return;
		}

		else if (gObjCheckUsedBuffEffect(lpTargetObj, BUFFTYPE_BERSERKER_STR) == TRUE)
		{
			gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, FALSE);
			return;
		}

		else if (gObjCheckUsedBuffEffect(lpTargetObj, BUFFTYPE_BERSERKER_MAS) == TRUE)
		{
			gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, FALSE);
			return;
		}

		else if (gObjCheckUsedBuffEffect(lpTargetObj, BUFFTYPE_BERSERKER_PRO) == TRUE)
		{
			gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, FALSE);
			return;
		}
	}

	gObjAddBuffEffect(lpTargetObj, BUFFTYPE_ATTACK_POWER_INC, EFFECTTYPE_IMPROVE_DAMAGE, skill_attack, 0, 0, skill_time);

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (Obj.VpPlayer2[n].type == OBJ_MONSTER && Obj.VpPlayer2[n].state != 0)
		{
			CGameObject lpMonster = &getGameObject(Obj.VpPlayer2[n]->number);
			if (lpMonster->m_iCurrentAI != 0)
			{
				lpMonster->m_Agro->IncAgro(Obj.m_Index, int(skill_attack) / 10);
			}
		}
	}

	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, TRUE);
}

void CMasterLevelSkillTreeSystem::MLS_SkillStrenghtenAttack(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic)
{
	
	CGameObject lpTargetObj = &getGameObject(aTargetIndex);

	if (lpTargetObj.Type != OBJ_USER && lpTargetObj.m_RecallMon == -1)
	{
		return;
	}

	if (Obj.Class != CLASS_ELF)
	{
		return;
	}

	if ((CC_MAP_RANGE(Obj.MapNumber) || Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL) && Obj.m_Index != lpTargetObj.m_Index)
	{
		return;
	}

	float aMLSkillValue = 0.0;

	int iPreSkill = this->GetParentSkill1(Obj.Class, lpMagic->m_Skill);

	if (iPreSkill > 0)
	{
		int iPreLevel = this->GetCurSkillLevel(lpObj, iPreSkill);

		if (iPreLevel > 0)
		{
			aMLSkillValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(iPreSkill), iPreLevel);
		}
	}

	float fValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);

	double skill_attack = 0.0;
	double skill_time = 0.0;

	this->m_Lua.Generic_Call("ElfAttack_Level2", "iiii>dd", lpTargetObj.Class, Obj.m_Index, aTargetIndex, Obj.m_PlayerData->Energy + Obj.AddEnergy, &skill_attack, &skill_time);

	skill_attack += skill_attack * (aMLSkillValue + fValue) / 100.0;

	if (gObjCheckPowerfulEffect(lpTargetObj, BUFFTYPE_ATTACK_POWER_INC, skill_attack, 0) == TRUE)
	{
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, FALSE);
		return;
	}

	else if (gObjCheckUsedBuffEffect(lpTargetObj, BUFFTYPE_BERSERKER) == TRUE)
	{
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, FALSE);
		return;
	}

	else if (gObjCheckUsedBuffEffect(lpTargetObj, BUFFTYPE_BERSERKER_STR) == TRUE)
	{
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, FALSE);
		return;
	}

	else if (gObjCheckUsedBuffEffect(lpTargetObj, BUFFTYPE_BERSERKER_MAS) == TRUE)
	{
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, FALSE);
		return;
	}

	else if (gObjCheckUsedBuffEffect(lpTargetObj, BUFFTYPE_BERSERKER_PRO) == TRUE)
	{
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, FALSE);
		return;
	}

	gObjAddBuffEffect(lpTargetObj, BUFFTYPE_ATTACK_POWER_INC, EFFECTTYPE_IMPROVE_DAMAGE, skill_attack, 0, 0, skill_time + fValue);

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (Obj.VpPlayer2[n].type == OBJ_MONSTER && Obj.VpPlayer2[n].state != 0)
		{
			CGameObject lpMonster = &getGameObject(Obj.VpPlayer2[n]->number);
			if (lpMonster->m_iCurrentAI != 0)
			{
				lpMonster->m_Agro->IncAgro(Obj.m_Index, int(skill_attack) / 10);
			}
		}
	}

	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, TRUE);
}

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenDefense(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic)
{
	
	CGameObject lpTargetObj = &getGameObject(aTargetIndex);

	if (lpTargetObj.Type != OBJ_USER && lpTargetObj.m_RecallMon == -1)
	{
		return;
	}

	if (Obj.Class != CLASS_ELF)
	{
		return;
	}

	if ((CC_MAP_RANGE(Obj.MapNumber) || Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL) && Obj.m_Index != lpTargetObj.m_Index)
	{
		return;
	}

	float aMLSkillValue = 0.0;

	int iPreSkill = this->GetParentSkill1(Obj.Class, lpMagic->m_Skill);

	if (iPreSkill > 0)
	{
		int iPreLevel = this->GetCurSkillLevel(lpObj, iPreSkill);

		if (iPreLevel > 0)
		{
			aMLSkillValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(iPreSkill), iPreLevel);
		}
	}

	float fValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);

	double skill_defense = 0.0;
	double skill_time = 0.0;

	this->m_Lua.Generic_Call("ElfDefense_Level2", "iiii>dd", lpTargetObj.Class, Obj.m_Index, aTargetIndex, Obj.m_PlayerData->Energy + Obj.AddEnergy, &skill_defense, &skill_time);

	skill_defense += skill_defense * (aMLSkillValue + fValue) / 100.0;
	skill_time += fValue;

	if (gObjCheckPowerfulEffect(lpTargetObj, BUFFTYPE_DEFENSE_POWER_INC, skill_defense, 0) == TRUE)
	{
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, FALSE);
		return;
	}

	gObjAddBuffEffect(lpTargetObj, BUFFTYPE_DEFENSE_POWER_INC, EFFECTTYPE_IMPROVE_DEFENSE, skill_defense, 0, 0, skill_time);

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (Obj.VpPlayer2[n].type == OBJ_MONSTER && Obj.VpPlayer2[n].state != 0)
		{
			CGameObject lpMonster = &getGameObject(Obj.VpPlayer2[n]->number);
			if (lpMonster->m_iCurrentAI != 0)
			{
				lpMonster->m_Agro->IncAgro(Obj.m_Index, int(skill_defense) / 10);
			}
		}
	}

	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, TRUE);
}

void CMasterLevelSkillTreeSystem::MLS_MaGumSkillDefenseDown(CGameObject &Obj, int aTargetIndex)
{
	int skillSuccess = 1;
	
	CGameObject lpTargetObj = &getGameObject(aTargetIndex);

	if (Obj.Type == OBJ_USER && Obj.Class != CLASS_MAGUMSA)
	{
		return;
	}

	if (retCalcSkillResistance(lpTargetObj, R_POISON))
	{
		gObjAddBuffEffect(lpTargetObj, BUFFTYPE_DEFENSE_POWER_DEC, EFFECTTYPE_DECREASE_DEFENSE, 10, 0, 0, 10);
		gGameProtocol.GCMagicAttackNumberSend(lpObj, AT_MSKILL_MG_FIRESLASH1, lpTargetObj.m_Index, skillSuccess);
	}
}

void CMasterLevelSkillTreeSystem::MLS_PowerSlash(CGameObject &Obj, CMagicInf* lpMagic, BYTE x, BYTE y, BYTE Targetangle, int aTargetIndex)
{
	
	int iSkillDis = MagicDamageC.GetSkillDistance(lpMagic->m_Skill);

	this->MLS_SkillFrustrum(Obj.m_Index, (360 * Targetangle / 255), iSkillDis + 1.0, iSkillDis + 1.0);

	int count = 0;
	int HitCount = 0;
	int DuelIndex = Obj.m_iDuelUser;
	int tObjNum;
	BOOL EnableAttack;
	BOOL bAttack;

	while (true)
	{
		if (Obj.VpPlayer2[count].state != 0)
		{
			tObjNum = Obj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = FALSE;

				if (Obj.VpPlayer2[count].type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon < 0)
				{
					EnableAttack = TRUE;
				}

				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = TRUE;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && Obj.VpPlayer2[count].type != OBJ_NPC && getGameObject(tObjNum)->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = TRUE;
				}

				else
				{
					int CallMonIndex = getGameObject(tObjNum)->m_Index;

					if (getGameObject(tObjNum)->Type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon >= 0)
					{
						CallMonIndex = getGameObject(getGameObject(tObjNum)->m_RecallMon)->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &getGameObject(CallMonIndex)) == TRUE)
					{
						EnableAttack = TRUE;
					}

					if (getGameObject(tObjNum)->Class >= 678 && getGameObject(tObjNum)->Class <= 680)
					{
						EnableAttack = TRUE;
					}
				}

				if (EnableAttack == TRUE)
				{
					if (SkillTestFrustrum(getGameObject(tObjNum)->X, getGameObject(tObjNum)->Y, Obj.m_Index) == TRUE)
					{
						if (gObjCalDistance(lpObj, &getGameObject(tObjNum)) <= iSkillDis)
						{
							bAttack = FALSE;

							if (HitCount > 10)
								bAttack = FALSE;
							if (HitCount < 5)
								bAttack = TRUE;
							else
								bAttack = rand() % 2;

							if (bAttack == TRUE)
							{
								gObjAttack(lpObj, &getGameObject(tObjNum), lpMagic, TRUE, TRUE, 0, 0, 0, 0);
							}

							HitCount++;
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

void CMasterLevelSkillTreeSystem::MLS_FireScream(CGameObject &Obj, int aTargetIndex, CMagicInf *lpMagic)
{
	if (g_SkillAdditionInfo.GetFireScreamSkill() == false)
	{
		return;
	}

	
	CGameObject lpTargetObj = &getGameObject(aTargetIndex);

	if (gObjIsConnected(Obj.m_Index) == false && gObjIsConnected(lpTargetObj.m_Index) == false)
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.Class != CLASS_DARKLORD)
	{
		return;
	}

	gObjAttack(lpObj, lpTargetObj, lpMagic, 0, 0, 0, 0, 0, 0);
}

void CMasterLevelSkillTreeSystem::MLS_SkillMonkBarrageJustOneTarget(CGameObject &Obj, CMagicInf* lpMagic, int aTargetIndex)
{
	

	if (ObjectMaxRange(aTargetIndex) == FALSE)
	{
		sLog->outBasic("[InvalidTargetIndex][MLS_SkillMonkBarrageJustOneTarget] Index :%d , AccountID : %s", Obj.m_Index, Obj.AccountID);
		return;
	}

	CGameObject lpTargetObj = &getGameObject(aTargetIndex);

	BOOL EnableAttack;
	int nViewPortObjectCount = 0;
	int nTargetObjNum;
	int nMaxBarrageCount;

	int nDistance = MagicDamageC.GetSkillDistance(lpMagic->m_Skill);

	if (lpMagic->m_Skill == 563)
	{
		int iPoint = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);
		nDistance += iPoint / 10;
	}

	while (true)
	{
		if (Obj.VpPlayer2[nViewPortObjectCount].state == TRUE)
		{
			nTargetObjNum = Obj.VpPlayer2[nViewPortObjectCount].number;
			if (nTargetObjNum >= 0)
			{
				if (nTargetObjNum == aTargetIndex)
				{
					break;
				}
			}
		}

		nViewPortObjectCount++;

		if (nViewPortObjectCount > MAX_VIEWPORT - 1)
			return;
	}

	EnableAttack = FALSE;

	if (Obj.VpPlayer2[nViewPortObjectCount].type == OBJ_USER)
	{
		if (g_GensSystem.IsMapBattleZone(Obj.MapNumber) == TRUE)
		{
			EnableAttack = TRUE;
		}

		else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && lpTargetObj.MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
		{
			EnableAttack = TRUE;
		}

		else if (gclassObjAttack.PkCheck(lpObj, lpTargetObj) == TRUE)
		{
			EnableAttack = TRUE;
		}
	}

	else if (getGameObject(nTargetObjNum)->Type == OBJ_MONSTER && getGameObject(nTargetObjNum)->m_RecallMon >= 0)
	{
		int CallMonIndex = getGameObject(getGameObject(nTargetObjNum)->m_RecallMon)->m_Index;

		if (gObjTargetGuildWarCheck(lpObj, &getGameObject(CallMonIndex)) == 1)
		{
			EnableAttack = TRUE;
		}

		if (getGameObject(nTargetObjNum)->Class >= 678 && getGameObject(nTargetObjNum)->Class <= 680)
		{
			EnableAttack = TRUE;
		}
	}

	else if (lpTargetObj.Type == OBJ_MONSTER || lpTargetObj.m_RecallMon < 0)
	{
		EnableAttack = TRUE;
	}

	if (EnableAttack == TRUE && this->MLS_CalDistance(Obj.X, Obj.Y, lpTargetObj.X, lpTargetObj.Y) < nDistance)
	{
		switch (lpMagic->m_Skill)
		{
		case 551:
		case 554:
			nMaxBarrageCount = 5;
			break;
		case 552:
		case 555:
			nMaxBarrageCount = 3;
			break;
		case 558:
		case 562:
			nMaxBarrageCount = 9;
			break;
		case 559:
		case 563:
			nMaxBarrageCount = 3;
			break;
		default:
			nMaxBarrageCount = 0;
			break;
		}

		switch (lpMagic->m_Skill)
		{
		case 558:
		case 562:
			if (rand() % 100 <= 40)
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
			break;
		case 551:
			if (rand() % 100 <= 10)
				gObjAddBuffEffect(lpTargetObj, BUFFTYPE_CURSED_ATTACK_DOWN, EFFECTTYPE_CURSED_ATTACKDOWN, 5, 0, 0, 10);
			break;
		case 554:
			if (rand() % 100 <= (this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level) + 10))
				gObjAddBuffEffect(lpTargetObj, BUFFTYPE_CURSED_ATTACK_DOWN, EFFECTTYPE_CURSED_ATTACKDOWN, 5, 0, 0, 10);
			break;
		case 552:
			if (rand() % 100 <= 10)
				gObjAddBuffEffect(lpTargetObj, BUFFTYPE_DEFENSE_POWER_DEC, EFFECTTYPE_DECREASE_DEFENSE, 10, 0, 0, 10);
			break;
		case 555:
			if (rand() % 100 <= (this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level) + 10))
				gObjAddBuffEffect(lpTargetObj, BUFFTYPE_DEFENSE_POWER_DEC, EFFECTTYPE_DECREASE_DEFENSE, 10, 0, 0, 10);
			break;
		}

		if (lpMagic->m_Skill == 562)
		{
			if (rand() % 100 <= this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level))
			{
				gObjAddBuffEffect(lpTargetObj, BUFFTYPE_DEBUFF_FROST, EFFECTTYPE_GIVE_DMG_TICK, 100, 0, 0, 10);
			}
		}

		for (int i = 1; i < nMaxBarrageCount; i++)
		{
			if (i % (nMaxBarrageCount - 1) == 1)
				gObjAttack(lpObj, lpTargetObj, lpMagic, 1, 1, 0, 0, i, 0);
			else
				gObjAttack(lpObj, lpTargetObj, lpMagic, 0, 1, 0, 0, i, 0);
		}
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenDragonRoar(CGameObject &Obj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex, int aDistance, BOOL bPVP, int nDelay)
{
	

	int DuelIndex = Obj.m_iDuelUser;
	int nHitCount = 0;
	BOOL bAttack = FALSE;
	int nMinCount = 4;
	int nMaxCount = 8;

	BOOL EnableAttack = FALSE;
	int tObjNum;
	int count = 0;

	if (this->MLS_CalDistance(Obj.X, Obj.Y, x, y) <= MagicDamageC.GetSkillDistance(lpMagic->m_Skill))
	{
		while (true)
		{
			if (Obj.VpPlayer2[count].state != 0)
			{
				tObjNum = Obj.VpPlayer2[count].number;

				if (tObjNum >= 0)
				{
					EnableAttack = FALSE;

					if (Obj.VpPlayer2[count].type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon < 0)
					{
						EnableAttack = TRUE;
					}

					else if ((tObjNum == aTargetIndex && bPVP == TRUE) || DuelIndex == tObjNum)
					{
						EnableAttack = TRUE;
					}

					else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && Obj.VpPlayer2[count].type != OBJ_NPC && getGameObject(tObjNum)->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
					{
						EnableAttack = TRUE;
					}

					else if (Obj.VpPlayer2[count].type == OBJ_USER && tObjNum == aTargetIndex && bPVP == TRUE)
					{
						if (g_GensSystem.IsMapBattleZone(Obj.MapNumber) == TRUE)
						{
							EnableAttack = TRUE;
						}

						else if (gclassObjAttack.PkCheck(lpObj, &getGameObject(aTargetIndex)) == TRUE)
						{
							EnableAttack = TRUE;
						}
					}

					else
					{
						int CallMonIndex = getGameObject(tObjNum)->m_Index;

						if (getGameObject(tObjNum)->Type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon >= 0)
						{
							CallMonIndex = getGameObject(getGameObject(tObjNum)->m_RecallMon)->m_Index;
						}

						if (gObjTargetGuildWarCheck(lpObj, &getGameObject(CallMonIndex)) == TRUE)
						{
							EnableAttack = TRUE;
						}

						if (getGameObject(tObjNum)->Class >= 678 && getGameObject(tObjNum)->Class <= 680)
						{
							EnableAttack = TRUE;
						}
					}

					if (EnableAttack == TRUE)
					{
						if (getGameObject(tObjNum)->X >= x - aDistance
							&& getGameObject(tObjNum)->X <= x + aDistance
							&& getGameObject(tObjNum)->Y >= y - aDistance
							&& getGameObject(tObjNum)->Y <= y + aDistance)
						{
							getGameObject(tObjNum)->lpAttackObj = lpObj;
							nHitCount++;

							if (nHitCount <= nMinCount)
							{
								bAttack = TRUE;
							}

							else if (nHitCount > nMaxCount)
							{
								bAttack = FALSE;
							}

							else
							{
								bAttack = rand() % 2;
							}

							if (bAttack == TRUE)
							{
								if (nDelay)
								{
									gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, nDelay, lpMagic->m_Skill, 0);
								}

								else if (lpMagic->m_Skill != 560 && lpMagic->m_Skill != 561)
								{
									gObjAttack(lpObj, &getGameObject(tObjNum), lpMagic, 1, 0, 0, 0, 0, 0);
								}

								else
								{
									for (int i = 1; i <= 4; i++)
									{
										if (i == 1)
											gObjAttack(lpObj, &getGameObject(tObjNum), lpMagic, 1, 1, 0, 0, i, 0);
										else
											gObjAttack(lpObj, &getGameObject(tObjNum), lpMagic, 0, 1, 0, 0, i, 0);
									}

									if (lpMagic->m_Skill == 561)
									{
										if (this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level) >= (rand() % 100))
										{
											gObjAddBuffEffect(&getGameObject(tObjNum), BUFFTYPE_DEBUFF_FIRE, EFFECTTYPE_GIVE_DMG_TICK, 100, 0, 0, 10);
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
				break;
		}
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenBlockRate(CGameObject &Obj, CMagicInf* lpMagic)
{
	

	if (Obj.Type != OBJ_USER && Obj.m_RecallMon == -1)
	{
		return;
	}

	if (Obj.Class != CLASS_RAGEFIGHTER)
	{
		return;
	}

	int number;
	int ApplyPartyIndex[MAX_USER_IN_PARTY];
	CGameObject lpPartyObj;
	memset(ApplyPartyIndex, -1, sizeof(ApplyPartyIndex));

	int partynum = Obj.PartyNumber;

	if (partynum != -1)
	{
		for (int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			number = gParty.m_PartyS[partynum].Number[n];

			if (number >= 0)
			{
				lpPartyObj = &getGameObject(number);

				if (Obj.MapNumber == lpPartyObj.MapNumber)
				{
					if (gObjCalDistance(lpObj, &getGameObject(number)) < 10)
					{
						ApplyPartyIndex[n] = lpPartyObj.m_Index;
					}
				}
			}
		}
	}

	int nEffectValue = (Obj.m_PlayerData->Energy + Obj.AddEnergy) / 50.0 + 10.0;

	if (nEffectValue > 30)
		nEffectValue = 30;

	nEffectValue += this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);

	if (partynum == -1)
	{
		gObjAddBuffEffect(lpObj, BUFFTYPE_MONK_INCREASE_DEFENSE_RATE_STR, EFFECTTYPE_IMPROVE_DEFENSE_RATE, nEffectValue, 0, 0, Obj.m_PlayerData->Energy / 5 + 60);
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, Obj.m_Index, TRUE);
	}

	else
	{
		for (int i = 0; i<MAX_USER_IN_PARTY; i++)
		{
			if (ApplyPartyIndex[i] != -1)
			{
				gObjAddBuffEffect(&getGameObject(ApplyPartyIndex[i]), BUFFTYPE_MONK_INCREASE_DEFENSE_RATE_STR, EFFECTTYPE_IMPROVE_DEFENSE_RATE, nEffectValue, 0, 0, Obj.m_PlayerData->Energy / 5 + 60);
				gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, ApplyPartyIndex[i], TRUE);
			}
		}
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillMasteryBlockRate(CGameObject &Obj, CMagicInf* lpMagic)
{
	

	if (Obj.Type != OBJ_USER && Obj.m_RecallMon == -1)
	{
		return;
	}

	if (Obj.Class != CLASS_RAGEFIGHTER)
	{
		return;
	}

	int number;
	int ApplyPartyIndex[MAX_USER_IN_PARTY];
	CGameObject lpPartyObj;
	memset(ApplyPartyIndex, -1, sizeof(ApplyPartyIndex));

	int partynum = Obj.PartyNumber;

	if (partynum != -1)
	{
		for (int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			number = gParty.m_PartyS[partynum].Number[n];

			if (number >= 0)
			{
				lpPartyObj = &getGameObject(number);

				if (Obj.MapNumber == lpPartyObj.MapNumber)
				{
					if (gObjCalDistance(lpObj, &getGameObject(number)) < 10)
					{
						ApplyPartyIndex[n] = lpPartyObj.m_Index;
					}
				}
			}
		}
	}

	int nEffectValue = (Obj.m_PlayerData->Energy + Obj.AddEnergy) / 50.0 + 10.0;

	if (nEffectValue > 30)
		nEffectValue = 30;

	nEffectValue += this->GetBrandOfMasterSkillValue(lpObj, lpMagic->m_Skill, 1);
	float fDefenseValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);

	if (partynum == -1)
	{
		gObjAddBuffEffect(lpObj, BUFFTYPE_MONK_INCREASE_DEFENSE_RATE_STR, EFFECTTYPE_IMPROVE_DEFENSE_RATE, nEffectValue, EFFECTTYPE_IMPROVE_DEFENSE, fDefenseValue, Obj.m_PlayerData->Energy / 5 + 60);
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, Obj.m_Index, TRUE);
	}

	else
	{
		for (int i = 0; i<MAX_USER_IN_PARTY; i++)
		{
			if (ApplyPartyIndex[i] != -1)
			{
				gObjAddBuffEffect(&getGameObject(ApplyPartyIndex[i]), BUFFTYPE_MONK_INCREASE_DEFENSE_RATE_STR, EFFECTTYPE_IMPROVE_DEFENSE_RATE, nEffectValue, EFFECTTYPE_IMPROVE_DEFENSE, fDefenseValue, Obj.m_PlayerData->Energy / 5 + 60);
				gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, ApplyPartyIndex[i], TRUE);
			}
		}
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenIncVital(CGameObject &Obj, CMagicInf* lpMagic)
{
	

	if (Obj.Type != OBJ_USER && Obj.m_RecallMon == -1)
	{
		return;
	}

	if (Obj.Class != CLASS_RAGEFIGHTER)
	{
		return;
	}

	int number;
	int ApplyPartyIndex[MAX_USER_IN_PARTY];
	CGameObject lpPartyObj;
	memset(ApplyPartyIndex, -1, sizeof(ApplyPartyIndex));

	int partynum = Obj.PartyNumber;

	if (partynum != -1)
	{
		for (int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			number = gParty.m_PartyS[partynum].Number[n];

			if (number >= 0)
			{
				lpPartyObj = &getGameObject(number);

				if (Obj.MapNumber == lpPartyObj.MapNumber)
				{
					if (gObjCalDistance(lpObj, &getGameObject(number)) < 10)
					{
						ApplyPartyIndex[n] = lpPartyObj.m_Index;
					}
				}
			}
		}
	}

	int nEffectValue = ((Obj.m_PlayerData->Energy + Obj.AddEnergy) - 132) / 10.0 + 30.0;

	nEffectValue += this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);

	if (partynum == -1)
	{
		gObjAddBuffEffect(lpObj, BUFFTYPE_MONK_INCREASE_HEALTH_STR, EFFECTTYPE_MONK_VITALITY, nEffectValue, 0, 0, Obj.m_PlayerData->Energy / 5 + 60);
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, Obj.m_Index, TRUE);
		gObjCalCharacter.CalcCharacter(Obj.m_Index);
	}

	else
	{
		for (int i = 0; i<MAX_USER_IN_PARTY; i++)
		{
			if (ApplyPartyIndex[i] != -1)
			{
				gObjAddBuffEffect(&getGameObject(ApplyPartyIndex[i]), BUFFTYPE_MONK_INCREASE_HEALTH_STR, EFFECTTYPE_MONK_VITALITY, nEffectValue, 0, 0, Obj.m_PlayerData->Energy / 5 + 60);
				gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, ApplyPartyIndex[i], TRUE);
				gObjCalCharacter.CalcCharacter(ApplyPartyIndex[i]);
			}
		}
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenIgnoreEnemyDefense(CGameObject &Obj, CMagicInf *lpMagic)
{
	

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.Class != CLASS_RAGEFIGHTER)
	{
		return;
	}

	if (lpMagic == NULL)
	{
		return;
	}

	int nEffectValue = ((Obj.m_PlayerData->Energy + Obj.AddEnergy) - 404) / 100.0 + 3.0;

	if (nEffectValue > 10)
		nEffectValue = 10;

	gObjAddBuffEffect(lpObj, BUFFTYPE_MONK_IGNORE_ENEMY_DEFENSE, EFFECTTYPE_DECREASE_DEFENSE_RATE, nEffectValue, EFFECTTYPE_MONK_DECREASE_DEFENSE, this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level), Obj.m_PlayerData->Energy / 5 + 60);
	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, Obj.m_Index, TRUE);
}

// eX700 skills

void CMasterLevelSkillTreeSystem::MLS_SkillBloodStorm(CGameObject &Obj, CMagicInf* lpMagic, BYTE x, BYTE y, int aTargetIndex, BOOL isCombo)
{
	int DuelIndex = Obj.m_iDuelUser;
	int nHitCount = 0;
	BOOL bAttack = FALSE;
	int tObjNum;
	int count = 0;
	BOOL EnableAttack = FALSE;

	if (this->MLS_CalDistance(Obj.X, Obj.Y, x, y) <= MagicDamageC.GetSkillDistance(lpMagic->m_Skill))
	{
		while (true)
		{
			if (Obj.VpPlayer2[count].state != 0)
			{
				tObjNum = Obj.VpPlayer2[count].number;

				if (tObjNum >= 0)
				{
					EnableAttack = FALSE;

					if (Obj.VpPlayer2[count].type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon < 0)
					{
						EnableAttack = TRUE;
					}

					else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
					{
						EnableAttack = TRUE;
					}

					else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && Obj.VpPlayer2[count].type != OBJ_NPC && getGameObject(tObjNum)->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
					{
						EnableAttack = TRUE;
					}

					else
					{
						int CallMonIndex = getGameObject(tObjNum)->m_Index;

						if (getGameObject(tObjNum)->Type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon >= 0)
						{
							CallMonIndex = getGameObject(getGameObject(tObjNum)->m_RecallMon)->m_Index;
						}

						if (gObjTargetGuildWarCheck(lpObj, &getGameObject(CallMonIndex)) == TRUE)
						{
							EnableAttack = TRUE;
						}

						if (getGameObject(tObjNum)->Class >= 678 && getGameObject(tObjNum)->Class <= 680)
						{
							EnableAttack = TRUE;
						}
					}

					if (EnableAttack == TRUE)
					{
						if (getGameObject(tObjNum)->X >= x - 2
							&& getGameObject(tObjNum)->X <= x + 2
							&& getGameObject(tObjNum)->Y >= y - 2
							&& getGameObject(tObjNum)->Y <= y + 2)
						{
							getGameObject(tObjNum)->lpAttackObj = lpObj;
							nHitCount++;

							if (nHitCount <= 4)
							{
								bAttack = TRUE;
							}

							else if (rand() % 2)
							{
								bAttack = TRUE;
							}

							if (nHitCount > 8)
							{
								bAttack = FALSE;
							}

							if (bAttack == TRUE)
							{
								gObjAddAttackProcMsgSendDelay(lpObj, 60, tObjNum, 1000, lpMagic->m_Skill, isCombo);
								gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, 1500, lpMagic->m_Skill, 0);
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
}

void CMasterLevelSkillTreeSystem::MLS_SkillEarthPrison(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic)
{
	int nHitCount = 0;
	BOOL bAttack = FALSE;
	int DuelIndex = Obj.m_iDuelUser;
	int count = 0;
	int tObjNum;
	BOOL EnableAttack = FALSE;

	while (true)
	{
		if (Obj.VpPlayer2[count].state != 0)
		{
			tObjNum = Obj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = FALSE;

				if (getGameObject(tObjNum)->Class >= 100 && getGameObject(tObjNum)->Class <= 110)
				{
					EnableAttack = FALSE;
				}

				if (getGameObject(tObjNum)->Class == 689)
				{
					EnableAttack = FALSE;
				}

				else if (Obj.VpPlayer2[count].type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon < 0)
				{
					EnableAttack = TRUE;
				}

				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = TRUE;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && Obj.VpPlayer2[count].type != OBJ_NPC && getGameObject(tObjNum)->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = TRUE;
				}

				else
				{
					int CallMonIndex = getGameObject(tObjNum)->m_Index;

					if (getGameObject(tObjNum)->Type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon >= 0)
					{
						CallMonIndex = getGameObject(getGameObject(tObjNum)->m_RecallMon)->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &getGameObject(CallMonIndex)) == TRUE)
					{
						EnableAttack = TRUE;
					}

					if (getGameObject(tObjNum)->Class >= 678 && getGameObject(tObjNum)->Class <= 680)
					{
						EnableAttack = TRUE;
					}
				}

				if (EnableAttack == TRUE)
				{
					if (gObjCalDistance(lpObj, &getGameObject(tObjNum)) <= 3)
					{
						bAttack = TRUE;
						nHitCount++;

						if (nHitCount > 10)
							bAttack = FALSE;

						if (bAttack == TRUE)
						{
							gObjAddAttackProcMsgSendDelay(lpObj, 60, tObjNum, 1200, lpMagic->m_Skill, 0);
							gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, 1500, lpMagic->m_Skill, 0);
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

void CMasterLevelSkillTreeSystem::MLS_SkillPoisonArrow(CGameObject &Obj, CMagicInf *lpMagic, int aTargetIndex, BOOL isCombo)
{
	CGameObject lpTargetObj = &getGameObject(aTargetIndex);

	if (gObjIsConnected(Obj.m_Index) == FALSE && gObjIsConnected(lpTargetObj.m_Index) == FALSE)
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	gObjAttack(lpObj, lpTargetObj, lpMagic, 1, 0, 0, isCombo, 0, 0);
	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, TRUE);
}

void CMasterLevelSkillTreeSystem::MLS_SkillCure(CGameObject &Obj, CMagicInf *lpMagic, int aTargetIndex)
{
	CGameObject lpTargetObj = &getGameObject(aTargetIndex);

	if (gObjIsConnected(Obj.m_Index) == FALSE && gObjIsConnected(lpTargetObj.m_Index) == FALSE)
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	gObjRemoveOneDebuffEffect(lpTargetObj);
	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, TRUE);
}

void CMasterLevelSkillTreeSystem::MLS_SkillMonsterCall(CGameObject &Obj, int MonsterType, int x, int y)
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.MapNumber == MAP_INDEX_ICARUS)
	{
		return;
	}

	if (CC_MAP_RANGE(Obj.MapNumber) || Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		return;
	}

	if (Obj.m_RecallMon >= 0)
	{
		gGameProtocol.GCRecallMonLife(Obj.m_Index, 60, 0);
		gObjMonsterCallKill(Obj.m_Index);
		return;
	}

	int result = gObjAddCallMon();

	if (result >= 0)
	{
		getGameObject(result)->X = x;
		getGameObject(result)->Y = y;
		getGameObject(result)->MTX = x;
		getGameObject(result)->MTY = y;
		getGameObject(result)->Dir = 2;
		getGameObject(result)->MapNumber = Obj.MapNumber;
		gObjSetMonster(result, MonsterType);
		getGameObject(result)->m_RecallMon = Obj.m_Index;
		getGameObject(result)->m_Attribute = 100;
		getGameObject(result)->TargetNumber = -1;
		getGameObject(result)->m_ActState.Emotion = 0;
		getGameObject(result)->m_ActState.Attack = 0;
		getGameObject(result)->m_ActState.EmotionCount = 0;
		getGameObject(result)->PathCount = 0;

		Obj.m_RecallMon = result;
		getGameObject(result)->m_MoveRange = 15;

		if (Obj.m_PlayerData->m_MPSkillOpt.iMpsCallMonHp > 0.0)
		{
			getGameObject(result)->Life += getGameObject(result)->Life * Obj.m_PlayerData->m_MPSkillOpt.iMpsCallMonHp / 100.0;
			getGameObject(result)->MaxLife += getGameObject(result)->MaxLife * Obj.m_PlayerData->m_MPSkillOpt.iMpsCallMonHp / 100.0;
		}

		if (Obj.m_PlayerData->m_MPSkillOpt.iMpsCallMonDefense > 0.0)
		{
			getGameObject(result)->m_Defense += getGameObject(result)->m_Defense * Obj.m_PlayerData->m_MPSkillOpt.iMpsCallMonDefense / 100.0;
		}

		if (Obj.m_PlayerData->m_MPSkillOpt.iMpsCallMonAttack > 0.0)
		{
			getGameObject(result)->m_AttackDamageMin += getGameObject(result)->m_AttackDamageMin * Obj.m_PlayerData->m_MPSkillOpt.iMpsCallMonAttack / 100.0;
			getGameObject(result)->m_AttackDamageMax += getGameObject(result)->m_AttackDamageMax * Obj.m_PlayerData->m_MPSkillOpt.iMpsCallMonAttack / 100.0;
		}

		gGameProtocol.GCRecallMonLife(getGameObject(result)->m_RecallMon, getGameObject(result)->MaxLife, getGameObject(result)->Life);
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillPartyHealing(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic)
{
	if (ObjectMaxRange(Obj.m_Index) == false)
	{
		return;
	}

	if (ObjectMaxRange(aTargetIndex) == false)
	{
		return;
	}

	
	CGameObject lpTargetObj = &getGameObject(aTargetIndex);

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (lpTargetObj.Type != OBJ_USER)
	{
		return;
	}

	int count = 0;
	int nChainTarget[3] = { -1, -1, -1 };
	int partynum = Obj.PartyNumber;

	if (gCheckSkillDistance(Obj.m_Index, aTargetIndex, lpMagic->m_Skill) == FALSE)
	{
		return;
	}

	if (partynum > -1 && partynum != lpTargetObj.PartyNumber)
	{
		sLog->outBasic("[party healing] mismatch party number. target : %d, (%d - %d)", aTargetIndex, Obj.PartyNumber, lpTargetObj.PartyNumber);
		return;
	}

	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, TRUE);

	nChainTarget[0] = aTargetIndex;
	count++;

	if (partynum > -1)
	{
		int partycount = gParty.m_PartyS[partynum].Count;
		BOOL EnableHealing = FALSE;

		for (int nIndex = 0; nIndex < MAX_USER_IN_PARTY; nIndex++)
		{
			int number = gParty.m_PartyS[partynum].Number[nIndex];

			if (number >= 0)
			{
				if (Obj.m_Index == number)
					continue;

				if (aTargetIndex == number)
					continue;

				CGameObject lpPartyObj = &getGameObject(number);

				if (lpPartyObj)
				{
					if (lpPartyObj.X >= lpTargetObj.X - 3 &&
						lpPartyObj.X <= lpTargetObj.X + 3 &&
						lpPartyObj.Y >= lpTargetObj.Y - 3 &&
						lpPartyObj.Y <= lpTargetObj.Y + 3)
					{
						EnableHealing = TRUE;
						nChainTarget[1] = number;
						count++;
						break;
					}
				}
			}
		}

		if (EnableHealing == TRUE)
		{
			for (int nIndex = 0; nIndex < MAX_USER_IN_PARTY; nIndex++)
			{
				int number = gParty.m_PartyS[partynum].Number[nIndex];

				if (number >= 0)
				{
					if (nChainTarget[1] == number)
						continue;

					if (Obj.m_Index == number)
						continue;

					if (aTargetIndex == number)
						continue;

					CGameObject lpPartyObj = &getGameObject(number);

					if (lpPartyObj)
					{
						if (lpPartyObj.X >= lpTargetObj.X - 3 &&
							lpPartyObj.X <= lpTargetObj.X + 3 &&
							lpPartyObj.Y >= lpTargetObj.Y - 3 &&
							lpPartyObj.Y <= lpTargetObj.Y + 3)
						{
							EnableHealing = TRUE;
							nChainTarget[2] = number;
							count++;
							break;
						}
					}
				}
			}
		}
	}

	if (count >= 1)
	{
		PMSG_CHAIN_MAGIC pMsg;
		PMSG_CHAIN_MAGIC_OBJECT pMagicObj;
		char SendByte[256];

		pMsg.MagicNumberH = HIBYTE(lpMagic->m_Skill);
		pMsg.MagicNumberL = LOBYTE(lpMagic->m_Skill);
		pMsg.wUserIndex = Obj.m_Index;
		pMsg.nCount = count;
		int nOffset = 10;

		for (int i = 0; i<count; i++)
		{
			pMagicObj.nTargetIndex = nChainTarget[i];
			std::memcpy(&SendByte[nOffset], &pMagicObj, 2);
			nOffset += 2;
		}

		PHeadSubSetB((BYTE*)&pMsg, 0xBF, 0x18, nOffset);
		std::memcpy(&SendByte, &pMsg, sizeof(pMsg));

		if (Obj.Type == OBJ_USER)
		{
			IOCP.DataSend(Obj.m_Index, (BYTE*)&SendByte, nOffset);
		}

		gGameProtocol.MsgSendV2(lpObj, (BYTE*)&SendByte, nOffset);
	}

	int nHP = 0;
	int nApplyHP = 0;

	this->m_Lua.Generic_Call("ElfPartyHealing", "i>i", Obj.m_PlayerData->Energy + Obj.AddEnergy, &nHP);

	for (int n = 0; n<count; n++)
	{
		if (ObjectMaxRange(nChainTarget[n]) == false)
			continue;

		nApplyHP = nHP - (nHP * n * 0.2);

		if (lpMagic->m_Skill == 429)
		{
			nApplyHP += nApplyHP * this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level) / 100.0;
		}

		gObjAddMsgSendDelay(&getGameObject(nChainTarget[n]], 16, nChainTarget[n), 200 * n + 200, nApplyHP);
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillBless(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic)
{
	if (ObjectMaxRange(aTargetIndex) == false)
	{
		return;
	}

	CGameObject lpTargetObj = &getGameObject(aTargetIndex);

	if (lpTargetObj.Type != OBJ_USER)
	{
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, FALSE);
		return;
	}

	if (gObjCheckUsedBuffEffect(lpTargetObj, BUFFTYPE_BLESS))
		gObjRemoveBuffEffect(lpTargetObj, BUFFTYPE_BLESS);

	int iEffectValue = 0;
	this->m_Lua.Generic_Call("ElfBless_Level1", "i>i", Obj.m_PlayerData->Energy + Obj.AddEnergy, &iEffectValue);

	// protection for exceeding stats over maximum allowed
	int Strength = lpTargetObj.m_PlayerData->Strength + lpTargetObj.AddStrength;
	int Dexterity = lpTargetObj.m_PlayerData->Dexterity + lpTargetObj.AddDexterity;
	int Vitality = lpTargetObj.m_PlayerData->Vitality + lpTargetObj.AddVitality;
	int Energy = lpTargetObj.m_PlayerData->Energy + lpTargetObj.AddEnergy;
	int Leadership = lpTargetObj.Leadership + lpTargetObj.AddLeadership;

	if ((Strength + iEffectValue) > g_MaxStatsInfo.GetClass.MaxStats[Obj.Class][0])
	{
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, FALSE);
		return;
	}

	if ((Dexterity + iEffectValue) > g_MaxStatsInfo.GetClass.MaxStats[Obj.Class][1])
	{
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, FALSE);
		return;
	}

	if ((Vitality + iEffectValue) > g_MaxStatsInfo.GetClass.MaxStats[Obj.Class][2])
	{
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, FALSE);
		return;
	}

	if ((Energy + iEffectValue) > g_MaxStatsInfo.GetClass.MaxStats[Obj.Class][3])
	{
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, FALSE);
		return;
	}

	if ((Leadership + iEffectValue) > g_MaxStatsInfo.GetClass.MaxStats[Obj.Class][4])
	{
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, FALSE);
		return;
	}

	EnterCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);
	lpTargetObj.m_PlayerData->AgilityCheckDelay = GetTickCount();

	gObjAddBuffEffect(lpTargetObj, BUFFTYPE_BLESS, EFFECTTYPE_ELF_BLESS, iEffectValue, 0, 0, MagicDamageC.GetSkillKeepTime(lpMagic->m_Skill));

	PMSG_USE_STAT_FRUIT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x2C, sizeof(pMsg));

	pMsg.result = 17;
	pMsg.btStatValue = iEffectValue;
	pMsg.btFruitType = 7;

	IOCP.DataSend(aTargetIndex, (BYTE*)&pMsg, pMsg.h.size);
	LeaveCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);

	gObjCalCharacter.CalcCharacter(aTargetIndex);
	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, TRUE);
}

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenBless(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic)
{
	if (ObjectMaxRange(aTargetIndex) == false)
	{
		return;
	}

	CGameObject lpTargetObj = &getGameObject(aTargetIndex);

	if (lpTargetObj.Type != OBJ_USER)
	{
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, FALSE);
		return;
	}

	if (gObjCheckUsedBuffEffect(lpTargetObj, BUFFTYPE_BLESS))
		gObjRemoveBuffEffect(lpTargetObj, BUFFTYPE_BLESS);

	double fValue = 0.0;
	this->m_Lua.Generic_Call("ElfBless_Level2", "i>d", Obj.m_PlayerData->Energy + Obj.AddEnergy, &fValue);

	fValue += this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);

	// protection for exceeding stats over maximum allowed
	int Strength = lpTargetObj.m_PlayerData->Strength + lpTargetObj.AddStrength;
	int Dexterity = lpTargetObj.m_PlayerData->Dexterity + lpTargetObj.AddDexterity;
	int Vitality = lpTargetObj.m_PlayerData->Vitality + lpTargetObj.AddVitality;
	int Energy = lpTargetObj.m_PlayerData->Energy + lpTargetObj.AddEnergy;
	int Leadership = lpTargetObj.Leadership + lpTargetObj.AddLeadership;

	if ((Strength + fValue) > g_MaxStatsInfo.GetClass.MaxStats[Obj.Class][0])
	{
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, FALSE);
		return;
	}

	if ((Dexterity + fValue) > g_MaxStatsInfo.GetClass.MaxStats[Obj.Class][1])
	{
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, FALSE);
		return;
	}

	if ((Vitality + fValue) > g_MaxStatsInfo.GetClass.MaxStats[Obj.Class][2])
	{
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, FALSE);
		return;
	}

	if ((Energy + fValue) > g_MaxStatsInfo.GetClass.MaxStats[Obj.Class][3])
	{
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, FALSE);
		return;
	}

	if ((Leadership + fValue) > g_MaxStatsInfo.GetClass.MaxStats[Obj.Class][4])
	{
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, FALSE);
		return;
	}

	EnterCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);

	lpTargetObj.m_PlayerData->AgilityCheckDelay = GetTickCount();

	gObjAddBuffEffect(lpTargetObj, BUFFTYPE_BLESS, EFFECTTYPE_ELF_BLESS, fValue, 0, 0, MagicDamageC.GetSkillKeepTime(lpMagic->m_Skill));

	PMSG_USE_STAT_FRUIT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x2C, sizeof(pMsg));

	pMsg.result = 17;
	pMsg.btStatValue = fValue;
	pMsg.btFruitType = 7;

	IOCP.DataSend(aTargetIndex, (BYTE*)&pMsg, pMsg.h.size);
	LeaveCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);

	gObjCalCharacter.CalcCharacter(aTargetIndex);
	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, TRUE);
}

void CMasterLevelSkillTreeSystem::MLS_SkillBlind(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic)
{
	if (ObjectMaxRange(aTargetIndex) == false)
	{
		return;
	}

	CGameObject lpTargetObj = &getGameObject(aTargetIndex);

	if (rand() % 100 <= ((Obj.m_PlayerData->Energy + Obj.AddEnergy) / 100 + 5))
	{
		gObjAddBuffEffect(lpTargetObj, BUFFTYPE_BLIND_2, EFFECTTYPE_BLIND, 50, 0, 0, MagicDamageC.GetSkillKeepTime(lpMagic->m_Skill));
		gObjViewportListCreate(lpTargetObj.m_Index);
		gObjAttack(lpObj, lpTargetObj, lpMagic, 1, 1, 0, 0, 0, 0);
	}

	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, TRUE);
}

void CMasterLevelSkillTreeSystem::SkillIronDefenseDK(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic)
{
	if (ObjectMaxRange(aTargetIndex) == false)
	{
		return;
	}

	CGameObject lpTargetObj = &getGameObject(aTargetIndex);

	int AddDefense = g_ConfigRead.m_Arg1DK;
	int AddLife = g_ConfigRead.m_Arg2DK;

	int BuffNumber = BUFFTYPE_IRON_DEFENSE;

	if (lpMagic->m_Skill == AT_MSKILL_DK_IRONDEFENSE1)
	{
		BuffNumber = BUFFTYPE_IRON_DEFENSE_STR;

		float fSkillValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);

		AddDefense += fSkillValue;
		AddLife += fSkillValue;
	}

	AddDefense = (AddDefense * 10) / 20;

	gObjAddBuffEffect(lpTargetObj, BuffNumber, EFFECTTYPE_IMPROVE_DEFENSE, AddDefense, EFFECTTYPE_HP, AddLife, g_ConfigRead.m_Arg3DK);
	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, TRUE);
}

void CMasterLevelSkillTreeSystem::SkillIronDefenseDL(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic)
{
	if (ObjectMaxRange(aTargetIndex) == false)
	{
		return;
	}

	CGameObject lpTargetObj = &getGameObject(aTargetIndex);

	int AddDefense = g_ConfigRead.m_Arg1DL;
	int AddLife = g_ConfigRead.m_Arg2DL;

	int BuffNumber = BUFFTYPE_IRON_DEFENSE;

	if (lpMagic->m_Skill == AT_MSKILL_DL_IRONDEFENSE1)
	{
		BuffNumber = BUFFTYPE_IRON_DEFENSE_STR;

		float fSkillValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);

		AddDefense += fSkillValue;
		AddLife += fSkillValue;
	}

	AddDefense = (AddDefense * 10) / 20;

	gObjAddBuffEffect(lpTargetObj, BuffNumber, EFFECTTYPE_IMPROVE_DEFENSE, AddDefense, EFFECTTYPE_HP, AddLife, g_ConfigRead.m_Arg3DL);
	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, TRUE);
}

void CMasterLevelSkillTreeSystem::MLS_SkillBloodHowling(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic)
{
	if (ObjectMaxRange(aTargetIndex) == false)
	{
		return;
	}

	CGameObject lpTargetObj = &getGameObject(aTargetIndex);

	gObjAddBuffEffect(lpTargetObj, BUFFTYPE_BLOOD_HOWL, 0, 0, 0, 0, MagicDamageC.GetSkillKeepTime(lpMagic->m_Skill));
	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, TRUE);
}

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenBloodHowling(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic)
{
	if (ObjectMaxRange(aTargetIndex) == false)
	{
		return;
	}

	CGameObject lpTargetObj = &getGameObject(aTargetIndex);

	if (lpTargetObj.Type != OBJ_USER)
	{
		return;
	}

	gObjAddBuffEffect(lpTargetObj, BUFFTYPE_BLOOD_HOWL_STR, 0, 0, 0, 0, MagicDamageC.GetSkillKeepTime(lpMagic->m_Skill));
	lpTargetObj.m_PlayerData->m_MPSkillOpt.iMpsIncDamageBloodHowling = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);
	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, TRUE);
}

void CMasterLevelSkillTreeSystem::MLS_SkillfulKnightBlow(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic, BOOL isCombo)
{
	
	CGameObject lpTargetObj = &getGameObject(aTargetIndex);

	float fangle = this->MLS_GetAngle(Obj.X, Obj.Y, getGameObject(aTargetIndex)->X, getGameObject(aTargetIndex)->Y);

	float fDistance = MagicDamageC.GetSkillDistance(lpMagic->m_Skill) + 1.0;
	this->MLS_SkillFrustrum(Obj.m_Index, fangle, 1.5, fDistance);

	int count = 0;
	int loopcount = 0;
	int DuelIndex = Obj.m_iDuelUser;
	int tObjNum;
	BOOL EnableAttack;

	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, TRUE);
	float fValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);

	while (true)
	{
		if (Obj.VpPlayer2[count].state != 0)
		{
			tObjNum = Obj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = FALSE;

				if (Obj.VpPlayer2[count].type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon < 0)
				{
					EnableAttack = TRUE;
				}

				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = TRUE;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && Obj.VpPlayer2[count].type != OBJ_NPC && getGameObject(tObjNum)->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = TRUE;
				}

				else
				{
					int CallMonIndex = getGameObject(tObjNum)->m_Index;

					if (getGameObject(tObjNum)->Type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon >= 0)
					{
						CallMonIndex = getGameObject(getGameObject(tObjNum)->m_RecallMon)->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &getGameObject(CallMonIndex)) == TRUE)
					{
						EnableAttack = TRUE;
					}

					if (getGameObject(tObjNum)->Class >= 678 && getGameObject(tObjNum)->Class <= 680)
					{
						EnableAttack = TRUE;
					}
				}

				if (g_GensSystem.IsMapBattleZone(Obj.MapNumber) == TRUE)
				{
					EnableAttack = TRUE;
				}

				if (gclassObjAttack.PkCheck(lpObj, lpTargetObj) == FALSE)
				{
					EnableAttack = FALSE;
				}

				if (EnableAttack == TRUE)
				{
					BOOL attackcheck = FALSE;

					if (loopcount)
					{
						if ((rand() % 3) == 0)
						{
							attackcheck = TRUE;
						}
					}

					else
					{
						attackcheck = TRUE;
					}

					if (attackcheck == TRUE)
					{
						if (SkillTestFrustrum(getGameObject(tObjNum)->X, getGameObject(tObjNum)->Y, Obj.m_Index))
						{
							getGameObject(tObjNum)->lpAttackObj = lpObj;
							gObjAttack(lpObj, &getGameObject(tObjNum), lpMagic, 0, 1, 0, isCombo, 0, 0);

							if (fValue > (rand() % 100))
							{
								if (gObjCheckUsedBuffEffect(&getGameObject(tObjNum), BUFFTYPE_BLEEDING) == TRUE)
								{
									gObjRemoveBuffEffect(&getGameObject(tObjNum), BUFFTYPE_BLEEDING);
								}

								gObjAddBuffEffect(&getGameObject(tObjNum), BUFFTYPE_BLEEDING, EFFECTTYPE_GIVE_DMG_TICK, (Obj.m_PlayerData->Strength + Obj.AddStrength) / 10, 0, 0, 10);
							}
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
				break;
		}
	}
}

void CMasterLevelSkillTreeSystem::MLS_MasteryKnightBlow(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic, BOOL isCombo)
{
	
	CGameObject lpTargetObj = &getGameObject(aTargetIndex);

	float fangle = this->MLS_GetAngle(Obj.X, Obj.Y, getGameObject(aTargetIndex)->X, getGameObject(aTargetIndex)->Y);

	float fDistance = MagicDamageC.GetSkillDistance(lpMagic->m_Skill) + 1.0;
	this->MLS_SkillFrustrum(Obj.m_Index, fangle, 1.5, fDistance);

	int count = 0;
	int loopcount = 0;
	int DuelIndex = Obj.m_iDuelUser;
	int tObjNum;
	BOOL EnableAttack;

	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, TRUE);

	int iStunTime = 0;
	
	iStunTime = g_ConfigRead.StunTimeDeathStabMastery;

	float fPrevValue = 0.0;
	int iPreSkill = this->GetParentSkill1(Obj.Class, lpMagic->m_Skill);

	if (iPreSkill > 0)
	{
		int iPreLevel = this->GetCurSkillLevel(lpObj, iPreSkill);

		if (iPreLevel > 0)
		{
			fPrevValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(iPreSkill), iPreLevel);
		}
	}

	float fValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);

	while (true)
	{
		if (Obj.VpPlayer2[count].state != 0)
		{
			tObjNum = Obj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = FALSE;

				if (Obj.VpPlayer2[count].type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon < 0)
				{
					EnableAttack = TRUE;
				}

				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = TRUE;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && Obj.VpPlayer2[count].type != OBJ_NPC && getGameObject(tObjNum)->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = TRUE;
				}

				else
				{
					int CallMonIndex = getGameObject(tObjNum)->m_Index;

					if (getGameObject(tObjNum)->Type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon >= 0)
					{
						CallMonIndex = getGameObject(getGameObject(tObjNum)->m_RecallMon)->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &getGameObject(CallMonIndex)) == TRUE)
					{
						EnableAttack = TRUE;
					}

					if (getGameObject(tObjNum)->Class >= 678 && getGameObject(tObjNum)->Class <= 680)
					{
						EnableAttack = TRUE;
					}
				}

				if (g_GensSystem.IsMapBattleZone(Obj.MapNumber) == TRUE)
				{
					EnableAttack = TRUE;
				}

				if (gclassObjAttack.PkCheck(lpObj, lpTargetObj) == FALSE)
				{
					EnableAttack = FALSE;
				}

				if (EnableAttack == TRUE)
				{
					BOOL attackcheck = FALSE;

					if (loopcount)
					{
						if ((rand() % 3) == 0)
						{
							attackcheck = TRUE;
						}
					}

					else
					{
						attackcheck = TRUE;
					}

					if (attackcheck == TRUE)
					{
						if (SkillTestFrustrum(getGameObject(tObjNum)->X, getGameObject(tObjNum)->Y, Obj.m_Index))
						{
							getGameObject(tObjNum)->lpAttackObj = lpObj;
							gObjAttack(lpObj, &getGameObject(tObjNum), lpMagic, 0, 1, 0, isCombo, 0, 0);

							if (fPrevValue >= (rand() % 100))
							{
								if (gObjCheckUsedBuffEffect(&getGameObject(tObjNum), BUFFTYPE_BLEEDING) == TRUE)
								{
									gObjRemoveBuffEffect(&getGameObject(tObjNum), BUFFTYPE_BLEEDING);
								}

								gObjAddBuffEffect(&getGameObject(tObjNum), BUFFTYPE_BLEEDING, EFFECTTYPE_GIVE_DMG_TICK, (Obj.m_PlayerData->Strength + Obj.AddStrength) / 10, 0, 0, 10);
							}

							if (g_ConfigRead.EnableStunEffect == 1)
							{
								int iStunEffect = fValue;

								if (getGameObject(tObjNum)->Type == OBJ_USER)
								{
									iStunEffect -= getGameObject(tObjNum)->m_PlayerData->m_Resistance_Stun;
								}

								if (iStunEffect >= (rand() % 100))
								{
									gObjAddBuffEffect(&getGameObject(tObjNum), BUFFTYPE_STUN, 0, 0, 0, 0, iStunTime);
									gObjSetPosition(tObjNum, getGameObject(tObjNum)->X, getGameObject(tObjNum)->Y);
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
			loopcount++;
			count = 0;

			if (loopcount >= 3)
				break;
		}
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillSkillfulDesruction(CGameObject lpObj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex, BOOL isCombo)
{
	int count = 0;
	int DuelIndex = Obj.m_iDuelUser;
	int nHitCount = 0;
	BOOL bAttack = FALSE;
	BOOL EnableAttack;
	int tObjNum;
	float fValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);

	if (this->MLS_CalDistance(Obj.X, Obj.Y, x, y) <= MagicDamageC.GetSkillDistance(lpMagic->m_Skill))
	{
		while (true)
		{
			if (Obj.VpPlayer2[count].state != 0)
			{
				tObjNum = Obj.VpPlayer2[count].number;

				if (tObjNum >= 0)
				{
					EnableAttack = FALSE;

					if (Obj.VpPlayer2[count].type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon < 0)
					{
						EnableAttack = TRUE;
					}

					else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
					{
						EnableAttack = TRUE;
					}

					else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && Obj.VpPlayer2[count].type != OBJ_NPC && getGameObject(tObjNum)->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
					{
						EnableAttack = TRUE;
					}

					else
					{
						int CallMonIndex = getGameObject(tObjNum)->m_Index;

						if (getGameObject(tObjNum)->Type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon >= 0)
						{
							CallMonIndex = getGameObject(getGameObject(tObjNum)->m_RecallMon)->m_Index;
						}

						if (gObjTargetGuildWarCheck(lpObj, &getGameObject(CallMonIndex)) == TRUE)
						{
							EnableAttack = TRUE;
						}

						if (getGameObject(tObjNum)->Class >= 678 && getGameObject(tObjNum)->Class <= 680)
						{
							EnableAttack = TRUE;
						}
					}

					if (EnableAttack == TRUE)
					{
						if (getGameObject(tObjNum)->X >= x - 3
							&& getGameObject(tObjNum)->X <= x + 3
							&& getGameObject(tObjNum)->Y >= y - 3
							&& getGameObject(tObjNum)->Y <= y + 3)
						{
							getGameObject(tObjNum)->lpAttackObj = lpObj;
							nHitCount++;

							if (nHitCount <= 4)
							{
								bAttack = TRUE;
							}

							else if (rand() % 2)
							{
								bAttack = TRUE;
							}

							if (nHitCount > 8)
							{
								bAttack = FALSE;
							}

							if (bAttack == TRUE)
							{
								gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, 500, lpMagic->m_Skill, 0);

								if (fValue >= (rand() % 100))
								{
									if (g_ConfigRead.EnableFreezeEffect == 1)
									{
										gObjAddBuffEffect(&getGameObject(tObjNum), BUFFTYPE_FREEZE_2, 0, 0, 0, 0, 3);

										getGameObject(tObjNum)->PathCount = 0;
										getGameObject(tObjNum)->PathStartEnd = 0;

										gObjSetPosition(tObjNum, getGameObject(tObjNum)->X, getGameObject(tObjNum)->Y);
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
}

void CMasterLevelSkillTreeSystem::MLS_SkillMasteryDesruction(CGameObject lpObj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex, BOOL isCombo)
{
	int count = 0;
	int DuelIndex = Obj.m_iDuelUser;
	int nHitCount = 0;
	BOOL bAttack = FALSE;
	BOOL EnableAttack;
	int tObjNum;

	float fPrevValue = this->GetBrandOfMasterSkillValue(lpObj, lpMagic->m_Skill, 1);
	float fValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);

	if (this->MLS_CalDistance(Obj.X, Obj.Y, x, y) <= MagicDamageC.GetSkillDistance(lpMagic->m_Skill))
	{
		while (true)
		{
			if (Obj.VpPlayer2[count].state != 0)
			{
				tObjNum = Obj.VpPlayer2[count].number;

				if (tObjNum >= 0)
				{
					EnableAttack = FALSE;

					if (Obj.VpPlayer2[count].type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon < 0)
					{
						EnableAttack = TRUE;
					}

					else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
					{
						EnableAttack = TRUE;
					}

					else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && Obj.VpPlayer2[count].type != OBJ_NPC && getGameObject(tObjNum)->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
					{
						EnableAttack = TRUE;
					}

					else
					{
						int CallMonIndex = getGameObject(tObjNum)->m_Index;

						if (getGameObject(tObjNum)->Type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon >= 0)
						{
							CallMonIndex = getGameObject(getGameObject(tObjNum)->m_RecallMon)->m_Index;
						}

						if (gObjTargetGuildWarCheck(lpObj, &getGameObject(CallMonIndex)) == TRUE)
						{
							EnableAttack = TRUE;
						}

						if (getGameObject(tObjNum)->Class >= 678 && getGameObject(tObjNum)->Class <= 680)
						{
							EnableAttack = TRUE;
						}
					}

					if (EnableAttack == TRUE)
					{
						if (getGameObject(tObjNum)->X >= x - 3
							&& getGameObject(tObjNum)->X <= x + 3
							&& getGameObject(tObjNum)->Y >= y - 3
							&& getGameObject(tObjNum)->Y <= y + 3)
						{
							getGameObject(tObjNum)->lpAttackObj = lpObj;
							nHitCount++;

							if (nHitCount <= 4)
							{
								bAttack = TRUE;
							}

							else if (rand() % 2)
							{
								bAttack = TRUE;
							}

							if (nHitCount > 8)
							{
								bAttack = FALSE;
							}

							if (bAttack == TRUE)
							{
								gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, 500, lpMagic->m_Skill, 0);

								if (fPrevValue >= (rand() % 100))
								{
									if (g_ConfigRead.EnableFreezeEffect == 1)
									{
										gObjAddBuffEffect(&getGameObject(tObjNum), BUFFTYPE_FREEZE_2, 0, 0, 0, 0, 3);

										getGameObject(tObjNum)->PathCount = 0;
										getGameObject(tObjNum)->PathStartEnd = 0;

										gObjSetPosition(tObjNum, getGameObject(tObjNum)->X, getGameObject(tObjNum)->Y);
									}
								}

								if (fValue >= (rand() % 100))
								{
									if (!gObjCheckUsedBuffEffect(&getGameObject(tObjNum), BUFFTYPE_ATTACK_SPEED_DEC))
									{
										gObjAddBuffEffect(&getGameObject(tObjNum), BUFFTYPE_ATTACK_SPEED_DEC, EFFECTTYPE_REDUCE_ATTACK_SPEED, 0, 0, 0, 5);
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
}

void CMasterLevelSkillTreeSystem::MLS_MasterySuddenIce(CGameObject &Obj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex)
{
	

	int count = 0;
	int DuelIndex = Obj.m_iDuelUser;
	int tObjNum;
	BOOL EnableAttack;
	float fRate = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);

	while (true)
	{
		if (Obj.VpPlayer2[count].state != 0)
		{
			tObjNum = Obj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = FALSE;

				if (Obj.VpPlayer2[count].type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon < 0)
				{
					EnableAttack = TRUE;
				}

				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = TRUE;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && Obj.VpPlayer2[count].type != OBJ_NPC && getGameObject(tObjNum)->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = TRUE;
				}

				else
				{
					int CallMonIndex = getGameObject(tObjNum)->m_Index;

					if (getGameObject(tObjNum)->Type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon >= 0)
					{
						CallMonIndex = getGameObject(getGameObject(tObjNum)->m_RecallMon)->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &getGameObject(CallMonIndex)) == TRUE)
					{
						EnableAttack = TRUE;
					}

					if (getGameObject(tObjNum)->Class >= 678 && getGameObject(tObjNum)->Class <= 680)
					{
						EnableAttack = TRUE;
					}
				}

				if (EnableAttack == TRUE)
				{
					if (this->MLS_CalDistance(x, y, getGameObject(tObjNum)->X, getGameObject(tObjNum)->Y) < 4)
					{
						gObjAttack(lpObj, &getGameObject(tObjNum), lpMagic, 1, 1, 0, 0, 0, 0);

						if (fRate >= (rand() % 100))
						{
							if (g_ConfigRead.EnableFreezeEffect == 1)
							{
								gObjAddBuffEffect(&getGameObject(tObjNum), BUFFTYPE_FREEZE_2, 0, 0, 0, 0, 3);

								getGameObject(tObjNum)->lpAttackObj = lpObj;
								getGameObject(tObjNum)->PathCount = 0;
								getGameObject(tObjNum)->PathStartEnd = 0;

								gObjSetPosition(tObjNum, getGameObject(tObjNum)->X, getGameObject(tObjNum)->Y);
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

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenMultiShot(CGameObject &Obj, CMagicInf *lpMagic, BYTE bAngle, int aTargetIndex)
{
	CGameObject &Obj = (CGameObject*)&getGameObject(Obj.m_Index);

	this->MLS_SkillFrustrum3(Obj.m_Index, bAngle, 6.0f, 7.0f, 1.0f, 0);

	vec3_t p;
	Vector(0.f, 6.0f, 0.f, p);

	vec3_t Angle[4];
	float Matrix[3][4];
	vec3_t vLine[5];

	Vector(0.f, 0.f, 40.0f, Angle[0]);
	Vector(0.f, 0.f, 20.0f, Angle[1]);
	Vector(0.f, 0.f, 340.0f, Angle[2]);
	Vector(0.f, 0.f, 320.0f, Angle[3]);

	AngleMatrix(Angle[0], Matrix);
	VectorRotate(p, Matrix, vLine[0]);
	AngleMatrix(Angle[1], Matrix);
	VectorRotate(p, Matrix, vLine[1]);
	Vector(0.f, 6.0f, 0.f, vLine[2]);

	AngleMatrix(Angle[2], Matrix);
	VectorRotate(p, Matrix, vLine[3]);
	AngleMatrix(Angle[3], Matrix);
	VectorRotate(p, Matrix, vLine[4]);
	Vector(0.f, 0.f, (bAngle * 360 / (BYTE)255), Angle[0]);

	AngleMatrix((float*)Angle, Matrix);

	int vSkillLineX[5];
	int vSkillLineY[5];

	vec3_t vFrustrum[5];

	for (int i = 0; i < 5; i++)
	{
		VectorRotate(vLine[i], Matrix, vFrustrum[i]);
		vSkillLineX[i] = (int)vFrustrum[i][0] + Obj.X;
		vSkillLineY[i] = (int)vFrustrum[i][1] + Obj.Y;
	}

	int tObjNum;
	int count = 0;
	int HitCount = 0;
	int DuelIndex = Obj.m_iDuelUser;
	BOOL EnableAttack;

	while (true)
	{
		if (Obj.VpPlayer2[count].state != 0)
		{
			tObjNum = Obj.VpPlayer2[count].number;
			if (tObjNum >= 0)
			{
				EnableAttack = FALSE;

				if (Obj.VpPlayer2[count].type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon < 0)
				{
					EnableAttack = TRUE;
				}
				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = TRUE;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && Obj.VpPlayer2[count].type != OBJ_NPC && getGameObject(tObjNum)->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = TRUE;
				}

				else
				{
					int CallMonIndex = getGameObject(tObjNum)->m_Index;

					if (getGameObject(tObjNum)->Type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon >= 0)
					{
						CallMonIndex = getGameObject(getGameObject(tObjNum)->m_RecallMon)->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &getGameObject(CallMonIndex)) == TRUE)
					{
						EnableAttack = TRUE;
					}

					if (getGameObject(tObjNum)->Class >= 678 && getGameObject(tObjNum)->Class <= 680)
					{
						EnableAttack = TRUE;
					}
				}

				if (EnableAttack)
				{
					int nResult[5];
					for (int i = 0; i < 5; i++)
					{
						nResult[i] = (((int)vSkillLineX[i] - Obj.X) * (getGameObject(tObjNum)->Y - Obj.Y)) - (((int)vSkillLineY[i] - Obj.Y) * (getGameObject(tObjNum)->X - Obj.X));
						if (SkillTestFrustrum(getGameObject(tObjNum)->X, getGameObject(tObjNum)->Y, Obj.m_Index))
						{
							if (nResult[i] > -5 && nResult[i] < 5)
							{
								gObjAttack(lpObj, &getGameObject(tObjNum), lpMagic, 1, 0, 0, FALSE, 0, 0);
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

void CMasterLevelSkillTreeSystem::MLS_SkillHellBust(CGameObject &Obj, int aTargetIndex, CMagicInf *lpMagic)
{
	if (aTargetIndex == 395)
	{
		this->MLS_SkillHellBustStart(Obj.m_Index, lpMagic);
		return;
	}

	

	if (Obj.m_PlayerData->SkillStrengthenHellFire2State == 0)
	{
		sLog->outBasic("[%s][%s] [MLS] Nova skill didn't cast", Obj.AccountID, Obj.Name); //HermeX Fix
		return;
	}

	Obj.m_PlayerData->SkillStrengthenHellFire2State = 0;
	int count = 0;
	Obj.m_PlayerData->SkillStrengthenHellFire2Time = 0;

	CGameObject lpTargetObj = &getGameObject(aTargetIndex);
	int DuelIndex = Obj.m_iDuelUser;
	BOOL EnableAttack = FALSE;
	int tObjNum;

	while (true)
	{
		if (Obj.VpPlayer2[count].state != 0)
		{
			tObjNum = Obj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = 0;

				if (Obj.VpPlayer2[count].type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if (aTargetIndex == tObjNum || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && Obj.VpPlayer2[count].type != OBJ_NPC && getGameObject(tObjNum)->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = TRUE;
				}

				else
				{
					int CallMonIndex = getGameObject(tObjNum)->m_Index;

					if (getGameObject(tObjNum)->Type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon >= 0)
					{
						CallMonIndex = getGameObject(getGameObject(tObjNum)->m_RecallMon)->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &getGameObject(CallMonIndex)) == 1)
					{
						EnableAttack = 1;
					}

					if (getGameObject(tObjNum)->Class >= 678 && getGameObject(tObjNum)->Class <= 680)
					{
						EnableAttack = TRUE;
					}
				}

				if (EnableAttack != 0)
				{
					if (gObjCalDistance(lpObj, &getGameObject(tObjNum)) <= MagicDamageC.GetSkillDistance(lpMagic->m_Skill))
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

	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, Obj.m_Index, 1);
}

void CMasterLevelSkillTreeSystem::MLS_SkillHellBustStart(CGameObject &Obj, CMagicInf * lpMagic)
{
	

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.m_PlayerData->SkillStrengthenHellFire2State != 0)
	{
		sLog->outBasic("[%s][%s] [MLS] Nova skill already casted", Obj.AccountID, Obj.Name);
		return;
	}

	Obj.m_PlayerData->SkillStrengthenHellFire2Time = GetTickCount();
	Obj.m_PlayerData->SkillStrengthenHellFire2State = 1;
	Obj.m_PlayerData->SkillStrengthenHellFire2Count = 0;

	gGameProtocol.GCMagicAttackNumberSend(lpObj, 395, Obj.m_Index, 1);
}

void CMasterLevelSkillTreeSystem::MLS_SkillMasteryWizardMagicDefense(CGameObject &Obj, CMagicInf *lpMagic, int aTargetIndex)
{
	CGameObject lpTargetObj = &getGameObject(aTargetIndex);

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (lpTargetObj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.m_RecallMon != -1)
	{
		return;
	}

	if (Obj.Class != CLASS_WIZARD && Obj.Class != CLASS_MAGUMSA)
	{
		return;
	}

	if (Obj.PartyNumber != lpTargetObj.PartyNumber)
	{
		return;
	}

	double fDefenseValue = 0.0;
	double fSkillValidTime = 0.0;

	this->m_Lua.Generic_Call("WizardMagicDefense_Level3", "iiii>dd", Obj.m_Index, aTargetIndex, Obj.AddDexterity + Obj.m_PlayerData->Dexterity, Obj.AddEnergy + Obj.m_PlayerData->Energy, &fDefenseValue, &fSkillValidTime);

	fDefenseValue += this->GetBrandOfMasterSkillValue(lpObj, lpMagic->m_Skill, 2);
	fSkillValidTime += this->GetBrandOfMasterSkillValue(lpObj, lpMagic->m_Skill, 1);

	lpTargetObj.m_PlayerData->m_MPSkillOpt.iMpsMasManaRate_Wizard = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);

	gObjAddBuffEffect(lpTargetObj, BUFFTYPE_MELEE_DEFENSE_INC_STR, EFFECTTYPE_IMPROVE_MELEE_DEFENSE, fDefenseValue, EFFECTTYPE_MELEE_DEFENSE_DOWN_MANA, g_SkillAdditionInfo.GetSoulBarrierManaRate_Level(lpMagic->m_Skill), fSkillValidTime);
	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, TRUE);
}

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenWeakness(CGameObject &Obj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex)
{
	

	int count = 0;
	int HitCount = 0;
	int DuelIndex = Obj.m_iDuelUser;
	int tObjNum;
	BOOL EnableAttack;

	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, TRUE);

	while (true)
	{
		if (Obj.VpPlayer2[count].state != 0)
		{
			tObjNum = Obj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = FALSE;

				if (Obj.VpPlayer2[count].type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon < 0)
				{
					EnableAttack = TRUE;
				}
				else if (DuelIndex == tObjNum)
				{
					EnableAttack = TRUE;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && Obj.VpPlayer2[count].type != OBJ_NPC && getGameObject(tObjNum)->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = TRUE;
				}

				else if (CC_MAP_RANGE(Obj.MapNumber) == TRUE || Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
				{
					EnableAttack = TRUE;
				}

				else if (IT_MAP_RANGE(Obj.MapNumber) == TRUE)
				{
					EnableAttack = TRUE;
				}

				else
				{
					int CallMonIndex = getGameObject(tObjNum)->m_Index;

					if (getGameObject(tObjNum)->Type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon >= 0)
					{
						CallMonIndex = getGameObject(getGameObject(tObjNum)->m_RecallMon)->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &getGameObject(CallMonIndex)) == TRUE)
					{
						EnableAttack = TRUE;
					}

					if (getGameObject(tObjNum)->Class >= 678 && getGameObject(tObjNum)->Class <= 680)
					{
						EnableAttack = TRUE;
					}
				}

				if (EnableAttack)
				{
					if (gObjCalDistance(lpObj, &getGameObject(tObjNum)) < 5)
					{
						gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, 700, lpMagic->m_Skill, 0);
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

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenEnervation(CGameObject &Obj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex)
{
	

	int count = 0;
	int HitCount = 0;
	int DuelIndex = Obj.m_iDuelUser;
	int tObjNum;
	BOOL EnableAttack;

	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, TRUE);

	while (true)
	{
		if (Obj.VpPlayer2[count].state != 0)
		{
			tObjNum = Obj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = FALSE;

				if (Obj.VpPlayer2[count].type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon < 0)
				{
					EnableAttack = TRUE;
				}
				else if (DuelIndex == tObjNum)
				{
					EnableAttack = TRUE;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && Obj.VpPlayer2[count].type != OBJ_NPC && getGameObject(tObjNum)->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = TRUE;
				}

				else if (CC_MAP_RANGE(Obj.MapNumber) == TRUE || Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
				{
					EnableAttack = TRUE;
				}

				else if (IT_MAP_RANGE(Obj.MapNumber) == TRUE)
				{
					EnableAttack = TRUE;
				}

				else
				{
					int CallMonIndex = getGameObject(tObjNum)->m_Index;

					if (getGameObject(tObjNum)->Type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon >= 0)
					{
						CallMonIndex = getGameObject(getGameObject(tObjNum)->m_RecallMon)->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &getGameObject(CallMonIndex)) == TRUE)
					{
						EnableAttack = TRUE;
					}

					if (getGameObject(tObjNum)->Class >= 678 && getGameObject(tObjNum)->Class <= 680)
					{
						EnableAttack = TRUE;
					}
				}

				if (EnableAttack)
				{
					if (gObjCalDistance(lpObj, &getGameObject(tObjNum)) < 5)
					{
						gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, 700, lpMagic->m_Skill, 0);
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

void CMasterLevelSkillTreeSystem::MLS_SkillMasteryIce(CGameObject &Obj, CMagicInf* lpMagic, int aTargetIndex)
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	CGameObject lpTargetObj = &getGameObject(aTargetIndex);
	float fValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);

	gObjAttack(lpObj, lpTargetObj, lpMagic, 1, 0, 0, 0, 0, 0);

	int iStunTime = g_ConfigRead.StunTimeIceMastery;

	if (fValue >= (rand() % 100))
	{
		BYTE btAttr = MapC[Obj.MapNumber].GetAttr(Obj.X, Obj.Y);

		if ((btAttr & 1) != 1)
		{
			if (g_ConfigRead.EnableFreezeEffect == 1)
			{
				gObjAddBuffEffect(lpTargetObj, BUFFTYPE_FREEZE_2, 0, 0, 0, 0, iStunTime);
				lpTargetObj.lpAttackObj = lpObj;
				lpTargetObj.PathCount = 0;
				lpTargetObj.PathStartEnd = 0;
				gObjSetPosition(aTargetIndex, lpTargetObj.X, lpTargetObj.Y);
			}
		}
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenFlameStrike(CGameObject &Obj, CMagicInf *lpMagic, BYTE bAngle, int aTargetIndex)
{
	

	this->MLS_SkillFrustrum3(Obj.m_Index, bAngle, 2.0, 4.0, 5.0, 0.0);

	int count = 0;
	int HitCount = 0;
	int DuelIndex = Obj.m_iDuelUser;
	int tObjNum;
	BOOL EnableAttack = FALSE;
	BOOL bAttack = FALSE;

	while (true)
	{
		if (Obj.VpPlayer2[count].state != 0)
		{
			tObjNum = Obj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = FALSE;

				if (Obj.VpPlayer2[count].type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon < 0)
				{
					EnableAttack = TRUE;
				}

				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = TRUE;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && Obj.VpPlayer2[count].type != OBJ_NPC && getGameObject(tObjNum)->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = TRUE;
				}

				else
				{
					int CallMonIndex = getGameObject(tObjNum)->m_Index;

					if (getGameObject(tObjNum)->Type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon >= 0)
					{
						CallMonIndex = getGameObject(getGameObject(tObjNum)->m_RecallMon)->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &getGameObject(CallMonIndex)) == TRUE)
					{
						EnableAttack = TRUE;
					}

					if (getGameObject(tObjNum)->Class >= 678 && getGameObject(tObjNum)->Class <= 680)
					{
						EnableAttack = TRUE;
					}
				}

				if (EnableAttack == TRUE)
				{
					if (SkillTestFrustrum(getGameObject(tObjNum)->X, getGameObject(tObjNum)->Y, Obj.m_Index) &&
						gObjCalDistance(lpObj, &getGameObject(tObjNum)) <= MagicDamageC.GetSkillDistance(lpMagic->m_Skill))
					{
						bAttack = FALSE;

						if (HitCount < 8)
							bAttack = TRUE;

						else if (rand() % 2)
							bAttack = TRUE;

						if (bAttack)
						{
							gObjAttack(lpObj, &getGameObject(tObjNum), lpMagic, 1, 0, 0, 0, 0, 0);
							gObjAttack(lpObj, &getGameObject(tObjNum), lpMagic, 1, 0, 0, 0, 0, 0);
						}

						HitCount++;
					}
				}
			}
		}

		count++;
		if (count > MAX_VIEWPORT - 1)
			break;
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillGiganticStorm(CGameObject &Obj, CMagicInf *lpMagic, BYTE x, BYTE y, int aTargetIndex)
{
	
	int count = 0;
	int DuelIndex = Obj.m_iDuelUser;
	int nHitCount = 0;
	BOOL bAttack = FALSE;
	int tObjNum;
	BOOL EnableAttack = FALSE;

	int nMinCount = 8;
	int nMaxCount = 12;

	if (this->MLS_CalDistance(Obj.X, Obj.Y, x, y) <= MagicDamageC.GetSkillDistance(lpMagic->m_Skill))
	{
		while (true)
		{
			if (Obj.VpPlayer2[count].state != 0)
			{
				tObjNum = Obj.VpPlayer2[count].number;

				if (tObjNum >= 0)
				{
					EnableAttack = FALSE;

					if (Obj.VpPlayer2[count].type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon < 0)
					{
						EnableAttack = TRUE;
					}

					else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
					{
						EnableAttack = TRUE;
					}

					else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && Obj.VpPlayer2[count].type != OBJ_NPC && getGameObject(tObjNum)->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
					{
						EnableAttack = TRUE;
					}

					else
					{
						int CallMonIndex = getGameObject(tObjNum)->m_Index;

						if (getGameObject(tObjNum)->Type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon >= 0)
						{
							CallMonIndex = getGameObject(getGameObject(tObjNum)->m_RecallMon)->m_Index;
						}

						if (gObjTargetGuildWarCheck(lpObj, &getGameObject(CallMonIndex)) == TRUE)
						{
							EnableAttack = TRUE;
						}

						if (getGameObject(tObjNum)->Class >= 678 && getGameObject(tObjNum)->Class <= 680)
						{
							EnableAttack = TRUE;
						}
					}

					if (EnableAttack == TRUE)
					{
						if (getGameObject(tObjNum)->X >= x - 6
							&& getGameObject(tObjNum)->X <= x + 6
							&& getGameObject(tObjNum)->Y >= y - 6
							&& getGameObject(tObjNum)->Y <= y + 6)
						{
							getGameObject(tObjNum)->lpAttackObj = lpObj;
							nHitCount++;

							if (nHitCount <= nMinCount)
							{
								bAttack = TRUE;
							}

							else if (rand() % 2)
							{
								bAttack = TRUE;
							}

							if (nHitCount > nMaxCount)
							{
								bAttack = FALSE;
							}

							if (bAttack == TRUE)
							{
								gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, 400, lpMagic->m_Skill, 0);
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
}

void CMasterLevelSkillTreeSystem::MLS_SkillMasteryDefenseDown(CGameObject &Obj, int aTargetIndex, CMagicInf *lpMagic)
{
	
	CGameObject lpTargetObj = &getGameObject(aTargetIndex);

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.Class != CLASS_MAGUMSA)
	{
		return;
	}

	if (ObjectMaxRange(aTargetIndex) == false)
	{
		return;
	}

	float fDownValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);

	if (retCalcSkillResistance(lpTargetObj, 1))
	{
		gObjAddBuffEffect(lpTargetObj, BUFFTYPE_DECREASE_DEFENSE_RATE, EFFECTTYPE_DECREASE_DEFENSE, fDownValue, 0, 0, 10);
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, TRUE);
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillMasteryAddCriticalDamage2(CGameObject &Obj, CMagicInf* lpMagic)
{
	

	int number;
	int ApplyPartyIndex[MAX_USER_IN_PARTY];
	CGameObject lpPartyObj;
	memset(ApplyPartyIndex, -1, sizeof(ApplyPartyIndex));

	int partynum = Obj.PartyNumber;

	if (partynum != -1)
	{
		for (int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			number = gParty.m_PartyS[partynum].Number[n];

			if (number >= 0)
			{
				lpPartyObj = &getGameObject(number);

				if (Obj.MapNumber == lpPartyObj.MapNumber)
				{
					if (gObjCalDistance(lpObj, &getGameObject(number)) < 10)
					{
						ApplyPartyIndex[n] = lpPartyObj.m_Index;
					}
				}
			}
		}
	}

	int addcriticaldamagevalue = 0;
	int SkillTime = 0;

	this->m_Lua.Generic_Call("DarkLordCriticalDamage_Level4", "ii>ii", Obj.Leadership + Obj.AddLeadership, Obj.m_PlayerData->Energy + Obj.AddEnergy, &addcriticaldamagevalue, &SkillTime);

	addcriticaldamagevalue += this->GetBrandOfMasterSkillValue(lpObj, lpMagic->m_Skill, 3);
	SkillTime += this->GetBrandOfMasterSkillValue(lpObj, lpMagic->m_Skill, 2);

	float fRate = this->GetBrandOfMasterSkillValue(lpObj, lpMagic->m_Skill, 1);

	if (partynum == -1)
	{
		Obj.m_PlayerData->m_MPSkillOpt.iMpsIncExcellentDamageRate = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);
		gObjAddBuffEffect(lpObj, BUFFTYPE_CRITICAL_DMG_INC_MAS, EFFECTTYPE_CRITICAL_DMG, addcriticaldamagevalue, EFFECTTYPE_CRITICALDAMAGE, fRate, SkillTime);
		MsgOutput(Obj.m_Index, Lang.GetText(0, 134), SkillTime);
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, Obj.m_Index, TRUE);
	}

	else
	{
		for (int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			if (ApplyPartyIndex[n] != -1)
			{
				lpPartyObj = &getGameObject(ApplyPartyIndex[n]);
				lpPartyObj.m_PlayerData->m_MPSkillOpt.iMpsIncExcellentDamageRate = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);
				gObjAddBuffEffect(lpPartyObj, BUFFTYPE_CRITICAL_DMG_INC_MAS, EFFECTTYPE_CRITICAL_DMG, addcriticaldamagevalue, EFFECTTYPE_CRITICALDAMAGE, fRate, SkillTime);
				MsgOutput(ApplyPartyIndex[n], Lang.GetText(0, 134), SkillTime);
				gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, lpPartyObj.m_Index, TRUE);
			}
		}
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillFrustrum3(CGameObject &Obj, BYTE bAngle, float tx1, float ty1, float tx2, float ty2)
{
	CGameObject &Obj = (CGameObject*)&getGameObject(Obj.m_Index);

	vec3_t p[4];
	Vector(-tx1, ty1, 0.f, p[0]);
	Vector(tx1, ty1, 0.f, p[1]);
	Vector(tx2, ty2, 0.f, p[2]);
	Vector(-tx2, ty2, 0.f, p[3]);

	vec3_t Angle;
	Vector(0.f, 0.f, bAngle * 360 / (BYTE)255, Angle);

	float Matrix[3][4];
	vec3_t vFrustrum[4];

	AngleMatrix(Angle, Matrix);

	for (int i = 0; i < 4; i++)
	{
		VectorRotate(p[i], Matrix, vFrustrum[i]);

		Obj.fSkillFrustrumX[i] = (int)vFrustrum[i][0] + Obj.X;
		Obj.fSkillFrustrumY[i] = (int)vFrustrum[i][1] + Obj.Y;
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenElectricSpark(CGameObject &Obj, CMagicInf* lpMagic, BYTE x, BYTE y, BYTE TargetPos, int aTargetIndex)
{
	

	int TargePosx = Obj.X - (8 - (TargetPos & 15));
	int TargePosy = Obj.Y - (8 - ((TargetPos & 240) >> 4));

	int tObjNum;
	int count = 0;
	int DuelIndex = Obj.m_iDuelUser;
	int iangle = this->MLS_GetAngle(Obj.X, Obj.Y, TargePosx, TargePosy);
	int delaytime;
	int Hit = 0;
	int EnableAttack;

	while (true)
	{
		if (Obj.VpPlayer2[count].state != 0)
		{
			tObjNum = Obj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = 0;

				if (Obj.VpPlayer2[count].type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && Obj.VpPlayer2[count].type != OBJ_NPC && getGameObject(tObjNum)->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = TRUE;
				}

				else
				{
					int CallMonIndex = getGameObject(tObjNum)->m_Index;

					if (getGameObject(tObjNum)->Type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon >= 0)
					{
						CallMonIndex = getGameObject(getGameObject(tObjNum)->m_RecallMon)->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &getGameObject(CallMonIndex)) == 1)
					{
						EnableAttack = 1;
					}

					if (getGameObject(tObjNum)->Class >= 678 && getGameObject(tObjNum)->Class <= 680)
					{
						EnableAttack = TRUE;
					}
				}
				if (EnableAttack != 0)
				{
					if (SkillElectricSparkHitBox.HitCheck(iangle, Obj.X, Obj.Y, getGameObject(tObjNum)->X, getGameObject(tObjNum)->Y))
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

void CMasterLevelSkillTreeSystem::MLS_SkillStrengthenchaoticDesair(CGameObject &Obj, CMagicInf* lpMagic, BYTE bAngle, int aTargetIndex)
{
	CGameObject &Obj = (CGameObject*)&getGameObject(Obj.m_Index);

	int skillSuccess = 1;

	this->MLS_SkillFrustrum3(Obj.m_Index, bAngle, 1.5f, 6.0f, 1.5f, 0);

	int tObjNum;
	int count = 0;
	int HitCount = 0;
	BOOL bAttack;
	int DuelIndex = Obj.m_iDuelUser;
	BOOL EnableAttack;

	while (true)
	{
		if (Obj.VpPlayer2[count].state != 0)
		{
			tObjNum = Obj.VpPlayer2[count].number;
			if (tObjNum >= 0)
			{
				EnableAttack = FALSE;

				if (Obj.VpPlayer2[count].type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon < 0)
				{
					EnableAttack = TRUE;
				}
				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = TRUE;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && Obj.VpPlayer2[count].type != OBJ_NPC && getGameObject(tObjNum)->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					EnableAttack = TRUE;
				}

				else
				{
					int CallMonIndex = getGameObject(tObjNum)->m_Index;

					if (getGameObject(tObjNum)->Type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon >= 0)
					{
						CallMonIndex = getGameObject(getGameObject(tObjNum)->m_RecallMon)->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &getGameObject(CallMonIndex)) == TRUE)
					{
						EnableAttack = TRUE;
					}

					if (getGameObject(tObjNum)->Class >= 678 && getGameObject(tObjNum)->Class <= 680)
					{
						EnableAttack = TRUE;
					}
				}

				if (EnableAttack)
				{
					if (SkillTestFrustrum(getGameObject(tObjNum)->X, getGameObject(tObjNum)->Y, Obj.m_Index))
					{
						bAttack = FALSE;

						if (HitCount >= 8)
						{
							if (rand() % 2)
							{
								bAttack = TRUE;
							}
						}
						else bAttack = TRUE;

						if (bAttack)
						{
							gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, 200, lpMagic->m_Skill, 0);
						}

						if (ObjectMaxRange(aTargetIndex) == true && tObjNum == aTargetIndex)
						{
							gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, 300, lpMagic->m_Skill, 0);
						}

						HitCount++;
					}
				}
			}
		}

		count++;
		if (count > MAX_VIEWPORT - 1)	break;
	}
}

bool CMasterLevelSkillTreeSystem::MLS_WeaponSkillDel(CGameObject &Obj, WORD aSkill, BYTE Level)
{
	for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
	{
		if (Obj.Magic[iCount].IsMagic() == TRUE)
		{
			if (this->CheckMasterLevelSkill(Obj.Magic[iCount].m_Skill))
			{
				int iBaseMLS = this->GetBaseMasterLevelSkill(Obj.Magic[iCount].m_Skill);

				if (iBaseMLS > 0)
				{
					int BrandSkill = MagicDamageC.GetNormalBrandOfSkill(iBaseMLS);

					if (aSkill == BrandSkill)
					{
						if (MagicDamageC.IsItemAttachedSkill(BrandSkill) && this->IsExistMLSBasedSkill(lpObj, iBaseMLS) == false)
						{
							gGameProtocol.GCMagicListOneDelSend(Obj.m_Index, iCount, Obj.Magic[iCount].m_Skill, Obj.Magic[iCount].m_Level, 0, 0);
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}

bool CMasterLevelSkillTreeSystem::MLS_WeaponSkillAdd(CGameObject &Obj, WORD aSkill, BYTE Level)
{
	for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
	{
		if (Obj.Magic[iCount].IsMagic() == TRUE)
		{
			if (this->CheckMasterLevelSkill(Obj.Magic[iCount].m_Skill))
			{
				int iBaseMLS = this->GetBaseMasterLevelSkill(Obj.Magic[iCount].m_Skill);

				if (iBaseMLS > 0)
				{
					if (aSkill == MagicDamageC.GetNormalBrandOfSkill(iBaseMLS) &&
						!this->IsExistMLSBasedSkill(lpObj, iBaseMLS))
					{
						gGameProtocol.GCMagicListOneSend(Obj.m_Index, iCount, Obj.Magic[iCount].m_Skill, Obj.Magic[iCount].m_Level, 0, 0);
						return true;
					}
				}
			}
		}
	}

	return false;
}

BOOL CMasterLevelSkillTreeSystem::CheckUsableWeaponSkill(CGameObject &Obj, int nCount)
{
	if (!this->CheckMasterLevelSkill(Obj.Magic[nCount].m_Skill))
	{
		return TRUE;
	}

	int iBaseMLS = this->GetBaseMasterLevelSkill(Obj.Magic[nCount].m_Skill);

	if (iBaseMLS <= 0)
	{
		return TRUE;
	}

	if (MagicDamageC.GetSkillUseType(iBaseMLS) != 4)
	{
		return TRUE;
	}

	int BrandOfSkill = MagicDamageC.GetNormalBrandOfSkill(iBaseMLS);

	if (MagicDamageC.IsItemAttachedSkill(Obj.Magic[nCount].m_Skill) == false)
	{
		return TRUE;
	}

	if (Obj.pntInventory[1]->IsItem() == TRUE && Obj.pntInventory[1]->m_Special[0] == BrandOfSkill)
	{
		return TRUE;
	}

	if (Obj.pntInventory[0]->IsItem() == TRUE && Obj.pntInventory[0]->m_Special[0] == BrandOfSkill)
	{
		return TRUE;
	}

	return FALSE;

}

void CMasterLevelSkillTreeSystem::MLS_SkillSleep(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic)
{
	
	int result = 0;
	int tObjNum;
	int count = 0;
	int DuelIndex;
	BOOL bEnableAttack;

	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, 1);

	DuelIndex = Obj.m_iDuelUser;

	while (true)
	{
		if (Obj.VpPlayer2[count].state != 0)
		{
			tObjNum = Obj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				bEnableAttack = FALSE;

				if (Obj.VpPlayer2[count].type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon < 0)
				{
					bEnableAttack = TRUE;
				}
				else if (tObjNum == aTargetIndex && DuelIndex == tObjNum)
				{
					bEnableAttack = TRUE;
				}
				else if (CC_MAP_RANGE(Obj.MapNumber) != FALSE || IT_MAP_RANGE(Obj.MapNumber) != FALSE || Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
				{
					bEnableAttack = TRUE;
				}

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && Obj.VpPlayer2[count].type != OBJ_NPC && getGameObject(tObjNum)->MapNumber == MAP_INDEX_CASTLESIEGE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					bEnableAttack = TRUE;
				}

				else
				{
					int CallMonIndex = getGameObject(tObjNum)->m_Index;

					if (getGameObject(tObjNum)->Type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon >= 0)
					{
						CallMonIndex = getGameObject(getGameObject(tObjNum)->m_RecallMon)->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &getGameObject(CallMonIndex)) == TRUE)
					{
						bEnableAttack = TRUE;
					}

					if (getGameObject(tObjNum)->Class >= 678 && getGameObject(tObjNum)->Class <= 680)
					{
						bEnableAttack = TRUE;
					}
				}

				if (bEnableAttack != FALSE)
				{
					if (tObjNum == aTargetIndex)
					{
						if (gCheckSkillDistance(Obj.m_Index, aTargetIndex, lpMagic->m_Skill) != FALSE)
						{
							gObjAttack(lpObj, &getGameObject(aTargetIndex), lpMagic, 1, 0, 0, 0, 0, 0);
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
}

void CMasterLevelSkillTreeSystem::MLS_SkillChainLightning(CGameObject &Obj, int aTargetIndex, CMagicInf *lpMagic)
{
	if (ObjectMaxRange(Obj.m_Index) == false)	return;
	if (ObjectMaxRange(aTargetIndex) == false)	return;

	CGameObject &Obj = (CGameObject*)&getGameObject(Obj.m_Index);
	CGameObject* lpTargetObj = (CGameObject*)&getGameObject(aTargetIndex);

	int tObjNum;
	int count = 0;
	int nChainTarget[3] = { -1, -1, -1 };
	int DuelIndex;
	BOOL EnableAttack;

	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, 1);

	DuelIndex = Obj.m_iDuelUser;

	if (gCheckSkillDistance(Obj.m_Index, aTargetIndex, lpMagic->m_Skill) == FALSE)
	{
		return;
	}

	nChainTarget[0] = aTargetIndex;

	while (true)
	{
		if (Obj.VpPlayer2[count].state != 0)
		{
			tObjNum = Obj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = FALSE;

				if (Obj.VpPlayer2[count].type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon < 0 && ((getGameObject(tObjNum)->Class < 100 || getGameObject(tObjNum)->Class > 110) || getGameObject(tObjNum)->Class != 689))
				{
					EnableAttack = TRUE;
				}

				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = TRUE;
				}

				else
				{
					int CallMonIndex = getGameObject(tObjNum)->m_Index;

					if (getGameObject(tObjNum)->Type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon >= 0)
					{
						CallMonIndex = getGameObject(getGameObject(tObjNum)->m_RecallMon)->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &getGameObject(CallMonIndex)) == TRUE)
					{
						EnableAttack = TRUE;
					}

					if (getGameObject(tObjNum)->Class >= 678 && getGameObject(tObjNum)->Class <= 680)
					{
						EnableAttack = TRUE;
					}
				}

				if (EnableAttack)
				{
					if (getGameObject(tObjNum)->X >= lpTargetObj.X - 1 && getGameObject(tObjNum)->X <= lpTargetObj.X + 1 &&
						getGameObject(tObjNum)->Y >= lpTargetObj.Y - 1 && getGameObject(tObjNum)->Y <= lpTargetObj.Y + 1 &&
						getGameObject(tObjNum)->Type == OBJ_MONSTER)
					{
						nChainTarget[1] = tObjNum;
						if (nChainTarget[2] != -1)	break;
					}
					else if (getGameObject(tObjNum)->X >= lpTargetObj.X - 2 && getGameObject(tObjNum)->X <= lpTargetObj.X + 2 &&
						getGameObject(tObjNum)->Y >= lpTargetObj.Y - 2 && getGameObject(tObjNum)->Y <= lpTargetObj.Y + 2 &&
						getGameObject(tObjNum)->Type == OBJ_MONSTER)
					{
						nChainTarget[2] = tObjNum;
						if (nChainTarget[1] != -1)	break;
					}
				}
			}
		}
		count++;
		if (count > MAX_VIEWPORT - 1)	break;
	}

	if (nChainTarget[2] == -1)
	{
		nChainTarget[2] = nChainTarget[1];
	}

	if (nChainTarget[1] == -1)
	{
		nChainTarget[1] = nChainTarget[0];
		nChainTarget[2] = nChainTarget[1];
	}

	int nOffSet = 0;

	PMSG_CHAIN_MAGIC pMsg;
	pMsg.MagicNumberH = HIBYTE(lpMagic->m_Skill);
	pMsg.MagicNumberL = LOBYTE(lpMagic->m_Skill);
	pMsg.wUserIndex = Obj.m_Index;
	pMsg.nCount = 3;
	nOffSet += sizeof(pMsg);

	BYTE SendByte[256];

	PMSG_CHAIN_MAGIC_OBJECT	pMagicObj;

	for (int n = 0; n<3; n++)
	{
		pMagicObj.nTargetIndex = nChainTarget[n];
		std::memcpy(&SendByte[nOffSet], &pMagicObj, sizeof(PMSG_CHAIN_MAGIC_OBJECT));
		nOffSet += sizeof(PMSG_CHAIN_MAGIC_OBJECT);
	}

	PHeadSubSetB((BYTE*)&pMsg, 0xBF, 0x0A, nOffSet);

	std::memcpy(SendByte, &pMsg, sizeof(pMsg));

	if (Obj.Type == OBJ_USER)
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, SendByte, pMsg.h.size);

	gGameProtocol.MsgSendV2(lpObj, SendByte, pMsg.h.size);

	for (int n = 0; n<3; n++)
	{
		gObjAddAttackProcMsgSendDelay(lpObj, 50, nChainTarget[n], (n * 200) + 200, lpMagic->m_Skill, n + 1);
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillLightningShock(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic)
{
	

	BOOL EnableAttack;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		EnableAttack = FALSE;

		if (Obj.VpPlayer2[n].state <= 0)
		{
			continue;
		}

		int index = Obj.VpPlayer2[n].number;

		if (getGameObject(index)->Type == OBJ_MONSTER && getGameObject(index)->m_RecallMon < 0)
		{
			EnableAttack = TRUE;
		}

		if (Obj.Type == OBJ_MONSTER && getGameObject(index)->Type == OBJ_USER) // DoppelGanger Fix
		{
			EnableAttack = TRUE;
		}

		if (aTargetIndex == index || index == Obj.m_iDuelUser)
		{
			EnableAttack = TRUE;
		}

		if (CC_MAP_RANGE(Obj.MapNumber) || IT_MAP_RANGE(Obj.MapNumber) || Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
		{
			EnableAttack = TRUE;
		}

		else if (g_ArcaBattle.IsArcaBattleServer() == TRUE)
		{
			EnableAttack = TRUE;
		}

		else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
		{
			if (getGameObject(index)->Type != OBJ_NPC &&
				getGameObject(index)->MapNumber == MAP_INDEX_CASTLESIEGE &&
				g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
			{
				EnableAttack = TRUE;
			}
		}

		int summon = Obj.m_Index;

		if (Obj.Type == OBJ_MONSTER && Obj.m_RecallMon >= 0)
		{
			summon = Obj.m_RecallMon;
		}

		if (gObjTargetGuildWarCheck(&getGameObject(summon), lpObj) != 0)
		{
			EnableAttack = TRUE;
		}

		if (getGameObject(index)->Class >= 678 && getGameObject(index)->Class <= 680)
		{
			EnableAttack = TRUE;
		}

		if (gObjCalDistance(lpObj, &getGameObject(index)) <= 6 && EnableAttack == TRUE)
		{
			gObjAddAttackProcMsgSendDelay(lpObj, 50, index, 300, lpMagic->m_Skill, 0);
		}
	}
}

void CMasterLevelSkillTreeSystem::MLS_SkillDrainLife(CGameObject &Obj, int aTargetIndex, CMagicInf* lpMagic)
{
	if (ObjectMaxRange(aTargetIndex) == false)
	{
		return;
	}

	
	int tObjNum;
	int count = 0;

	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, 1);

	int DuelIndex = Obj.m_iDuelUser;
	BOOL EnableAttack;

	while (true)
	{
		if (Obj.VpPlayer2[count].state != 0)
		{
			tObjNum = Obj.VpPlayer2[count].number;

			if (tObjNum >= 0)
			{
				EnableAttack = 0;

				if (Obj.VpPlayer2[count].type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon < 0)
				{
					EnableAttack = 1;
				}
				else if (tObjNum == aTargetIndex || DuelIndex == tObjNum)
				{
					EnableAttack = 1;
				}
				else
				{
					int CallMonIndex = getGameObject(tObjNum)->m_Index;

					if (getGameObject(tObjNum)->Type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon >= 0)
					{
						CallMonIndex = getGameObject(getGameObject(tObjNum)->m_RecallMon)->m_Index;
					}

					if (gObjTargetGuildWarCheck(lpObj, &getGameObject(CallMonIndex)) == 1)
					{
						EnableAttack = 1;
					}

					if (getGameObject(tObjNum)->Class >= 678 && getGameObject(tObjNum)->Class <= 680)
					{
						EnableAttack = TRUE;
					}
				}

				if (EnableAttack != 0)
				{
					if (tObjNum == aTargetIndex)
					{
						if (gCheckSkillDistance(Obj.m_Index, aTargetIndex, lpMagic->m_Skill) != FALSE)
						{
							gObjAddAttackProcMsgSendDelay(lpObj, 50, tObjNum, 700, lpMagic->m_Skill, 0);
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
}

// End of MLS functions ( Skills )

int CMasterLevelSkillTreeSystem::GetRequireMLPoint(int iClass, int iSkill)
{
	if (iClass < 0 || iClass >= MAX_TYPE_PLAYER)
	{
		sLog->outBasic("[MasterLevelSkillTreeSystem] GetRequireMLPoint - Invalid Class Code  : %d", iClass);
		return -1;
	}

	if (iSkill < 0 || iSkill > 722)
	{
		sLog->outBasic("[MasterLevelSkillTreeSystem] GetRequireMLPoint - Invalid Skill Code  : %d", iSkill);
		return -1;
	}

	for (int iTreeKind = 0; iTreeKind < MAX_TREE_TYPE; iTreeKind++)
	{
		for (int iRank = 0; iRank < MAX_MASTER_SKILL_RANK; iRank++)
		{
			for (int iPos = 0; iPos < MAX_SKILL_POS; iPos++)
			{
				if (this->m_MLSTable[iClass].iAbility[iTreeKind].iElement[iRank][iPos].m_dwSkillID == iSkill)
				{
					return this->m_MLSTable[iClass].iAbility[iTreeKind].iElement[iRank][iPos].m_byConsumePoint;
				}
			}
		}
	}

	return -1;
}

int CMasterLevelSkillTreeSystem::GetMaxPointOfMasterSkill(int iClass, int iSkill)
{
	if (iClass < 0 || iClass >= MAX_TYPE_PLAYER)
	{
		sLog->outBasic("[MasterLevelSkillTreeSystem] GetMaxPointOfMasterSkill - Invalid Class Code  : %d", iClass);
		return -1;
	}

	if (iSkill < 0 || iSkill > 722)
	{
		sLog->outBasic("[MasterLevelSkillTreeSystem] GetMaxPointOfMasterSkill - Invalid Skill Code  : %d", iSkill);
		return -1;
	}

	for (int iTreeKind = 0; iTreeKind < MAX_TREE_TYPE; iTreeKind++)
	{
		for (int iRank = 0; iRank < MAX_MASTER_SKILL_RANK; iRank++)
		{
			for (int iPos = 0; iPos < MAX_SKILL_POS; iPos++)
			{
				if (this->m_MLSTable[iClass].iAbility[iTreeKind].iElement[iRank][iPos].m_dwSkillID == iSkill)
				{
					return this->m_MLSTable[iClass].iAbility[iTreeKind].iElement[iRank][iPos].m_byMaxPoint;
				}
			}
		}
	}

	return -1;
}

float CMasterLevelSkillTreeSystem::GetMasterSkillValue(int iType, int iPoint)
{
	if (iPoint < 0 || iPoint > this->GetMaxMasterSkillLevel() + 1)
	{
		return 0.0;
	}

	if (iType < 0 || iType > MAX_MASTER_SKILL_TYPE)
	{
		return 0.0;
	}

	if (iPoint == this->GetMaxMasterSkillLevel() + 1 || iPoint == 0)
	{
		return 0.0;
	}

	return this->m_MLSValueTable[iType].fValueTable[iPoint];
}

float CMasterLevelSkillTreeSystem::GetMasterSkillManaIncRate(int iPoint)
{
	if (iPoint < 1 || iPoint > this->GetMaxMasterSkillLevel())
	{
		return -1.0;
	}

	for (int i = 0; i < MAX_MASTER_SKILL_TYPE; i++)
	{
		if (this->m_MLSValueTable[i].iValueType == MLST_VALUE_MANA_INC)
		{
			return this->m_MLSValueTable[i].fValueTable[iPoint];
		}
	}

	return -1.0;
}

float CMasterLevelSkillTreeSystem::GetBrandOfMasterSkillValue(CGameObject &Obj, int iSkill, int iStep)
{
	int nStep = iStep;
	int nPrevSkill = iSkill;
	int nSkill = iSkill;

	while (nStep && nPrevSkill)
	{
		WORD nBrandOfSkill = MagicDamageC.GetBrandOfSkill(nSkill);
		nPrevSkill = nBrandOfSkill;
		nSkill = nBrandOfSkill;
		nStep--;
	}

	if (this->CheckMasterLevelSkill(nSkill))
	{
		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == nSkill)
			{
				return this->GetMasterSkillValue(MagicDamageC.SkillGet(nSkill), Obj.Magic[iCount].m_Level);
			}
		}
	}

	return 0.0;
}

int CMasterLevelSkillTreeSystem::CheckPreviousRankSkill(CGameObject &Obj, int iSkill, int iLevel)
{
	if (iSkill < 0 || iSkill > 722)
	{
		return -1;
	}

	int nTreeKind = -1;
	int nRank = MagicDamageC.GetSkillRank(iSkill) - 1;

	if (nRank < 1)
	{
		return 1;
	}

	int iClass = Obj.Class;

	for (int iTreeKind = 0; iTreeKind < MAX_TREE_TYPE; iTreeKind++)
	{
		for (int iRank = 0; iRank < MAX_MASTER_SKILL_RANK; iRank++)
		{
			for (int iPos = 0; iPos < MAX_SKILL_POS; iPos++)
			{
				if (this->m_MLSTable[iClass].iAbility[iTreeKind].iElement[iRank][iPos].m_dwSkillID == iSkill)
				{
					nTreeKind = iTreeKind;
					break;
				}
			}
		}
	}

	int nPoint = 0;
	nRank--;

	for (int iPos = 0; iPos < MAX_SKILL_POS; iPos++)
	{
		for (int i = 0; i<MAX_MAGIC; i++)
		{
			if (Obj.Magic[i].IsMagic() == TRUE &&
				this->CheckMasterLevelSkill(Obj.Magic[i].m_Skill) == true)
			{
				if (Obj.Magic[i].m_Skill == this->m_MLSTable[iClass].iAbility[nTreeKind].iElement[nRank][iPos].m_dwSkillID)
				{
					nPoint |= Obj.Magic[i].m_Level;
				}
			}
		}
	}

	return nPoint >= 10;
}

int CMasterLevelSkillTreeSystem::CheckSkillLevel(CGameObject &Obj, int iSkill)
{
	for (int i = 0; i<MAX_MAGIC; i++)
	{
		if (Obj.Magic[i].IsMagic() == TRUE && Obj.Magic[i].m_Skill == iSkill)
		{
			return Obj.Magic[i].m_Level;
		}
	}

	return 0;
}

int CMasterLevelSkillTreeSystem::GetMasterSkillUIIndex(int iClass, int iSkill)
{
	if (iClass < 0 || iClass >= MAX_TYPE_PLAYER)
	{
		return -1;
	}

	if (iSkill < 0 || iSkill > 722)
	{
		return -1;
	}

	for (int iTreeKind = 0; iTreeKind < MAX_TREE_TYPE; iTreeKind++)
	{
		for (int iRank = 0; iRank < MAX_MASTER_SKILL_RANK; iRank++)
		{
			for (int iPos = 0; iPos < MAX_SKILL_POS; iPos++)
			{
				if (this->m_MLSTable[iClass].iAbility[iTreeKind].iElement[iRank][iPos].m_dwSkillID == iSkill)
				{
					return this->m_MLSTable[iClass].iAbility[iTreeKind].iElement[iRank][iPos].m_dwIndex;
				}
			}
		}
	}

	return -1;
}

int CMasterLevelSkillTreeSystem::GetParentSkill1(int iClass, int iSkill)
{
	for (int iTreeKind = 0; iTreeKind < MAX_TREE_TYPE; iTreeKind++)
	{
		for (int iRank = 0; iRank < MAX_MASTER_SKILL_RANK; iRank++)
		{
			for (int iPos = 0; iPos < MAX_SKILL_POS; iPos++)
			{
				if (this->m_MLSTable[iClass].iAbility[iTreeKind].iElement[iRank][iPos].m_dwSkillID == iSkill)
				{
					return this->m_MLSTable[iClass].iAbility[iTreeKind].iElement[iRank][iPos].m_dwParentSkill1;
				}
			}
		}
	}

	return 0;
}

int CMasterLevelSkillTreeSystem::GetParentSkill2(int iClass, int iSkill)
{
	for (int iTreeKind = 0; iTreeKind < MAX_TREE_TYPE; iTreeKind++)
	{
		for (int iRank = 0; iRank < MAX_MASTER_SKILL_RANK; iRank++)
		{
			for (int iPos = 0; iPos < MAX_SKILL_POS; iPos++)
			{
				if (this->m_MLSTable[iClass].iAbility[iTreeKind].iElement[iRank][iPos].m_dwSkillID == iSkill)
				{
					return this->m_MLSTable[iClass].iAbility[iTreeKind].iElement[iRank][iPos].m_dwParentSkill2;
				}
			}
		}
	}

	return 0;
}

void CMasterLevelSkillTreeSystem::SetItemMLPassiveSkill(CGameObject &Obj, int iType)
{
	switch (iType)
	{
	case ITEM_TWOHAND_SWORD:
	{
		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 348)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsTwoHandSwordAddDamage = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				break;
			}
		}

		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 352)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsTwoHandsPvPAttack = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				return;
			}
		}
	}
	break;
	case ITEM_ONEHAND_SWORD:
	{
		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 349)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsSwordAddDamage = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				break;
			}
		}

		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 353)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsAddAttackSpeed = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				return;
			}
		}
	}
	break;
	case ITEM_MACE:
	{
		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == AT_MSKILL_MACE_STRENGTHENER)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsMaceStrength = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				break;
			}
		}

		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 354)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsAddSturn = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				return;
			}
		}
	}
	break;
	case ITEM_SPEAR:
	{
		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 351)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsSpearAddDamage = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				break;
			}
		}

		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 355)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsAddDoubleDamage = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				return;
			}
		}
	}
	break;
	case ITEM_ONEHAND_STAFF:
	{
		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 397)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsStaffMagicPower = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				break;
			}
		}

		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 400)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsStaffAttackSpeed = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				return;
			}
		}
	}
	break;
	case ITEM_TWOHAND_STAFF:
	{
		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 398)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsTwoHandsMagicPower = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				break;
			}
		}

		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 401)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsTwoHandsStaffManAttack = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				return;
			}
		}
	}
	break;
	case ITEM_SHIELD:
	{
		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE)
			{
				if (Obj.Magic[iCount].m_Skill == 399)
				{
					Obj.m_PlayerData->m_MPSkillOpt.iMpsIncShield = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
					break;
				}

				if (Obj.Magic[iCount].m_Skill == 437)
				{
					Obj.m_PlayerData->m_MPSkillOpt.iMpsElfAddShield = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
					break;
				}

				if (Obj.Magic[iCount].m_Skill == 528)
				{
					Obj.m_PlayerData->m_MPSkillOpt.iMpsAddShieldDefense = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
					break;
				}
			}
		}

		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE)
			{
				if (Obj.Magic[iCount].m_Skill == 402)
				{
					Obj.m_PlayerData->m_MPSkillOpt.iMpsShieldBlockRate = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
					return;
				}

				if (Obj.Magic[iCount].m_Skill == 440)
				{
					Obj.m_PlayerData->m_MPSkillOpt.iMpsElfShieldBlockRate = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
					return;
				}

				if (Obj.Magic[iCount].m_Skill == 532)
				{
					Obj.m_PlayerData->m_MPSkillOpt.iMpsAddShieldBlockingRage = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
					return;
				}

				if (Obj.Magic[iCount].m_Skill == 623)
				{
					Obj.m_PlayerData->m_MPSkillOpt.iMpsShieldBlockRate = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
					return;
				}

				if (Obj.Magic[iCount].m_Skill == 634)
				{
					Obj.m_PlayerData->m_MPSkillOpt.iMpsShieldBlockRate = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
					return;
				}

				if (Obj.Magic[iCount].m_Skill == 643)
				{
					Obj.m_PlayerData->m_MPSkillOpt.iMpsShieldBlockRate = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
					return;
				}

				if (Obj.Magic[iCount].m_Skill == 653)
				{
					Obj.m_PlayerData->m_MPSkillOpt.iMpsShieldBlockRate = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
					return;
				}

				if (Obj.Magic[iCount].m_Skill == 663)
				{
					Obj.m_PlayerData->m_MPSkillOpt.iMpsShieldBlockRate = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
					return;
				}

				if (Obj.Magic[iCount].m_Skill == 677)
				{
					Obj.m_PlayerData->m_MPSkillOpt.iMpsShieldBlockRate = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
					return;
				}

				if (Obj.Magic[iCount].m_Skill == 713)
				{
					Obj.m_PlayerData->m_MPSkillOpt.iMpsShieldBlockRate = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
					return;
				}
			}
		}
	}
	break;
	case ITEM_BOW:
	{
		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 435)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsBowAddDamage = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				break;
			}
		}

		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 438)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsBowAddAttackSpeed = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				return;
			}
		}
	}
	break;
	case ITEM_CROSSBOW:
	{
		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 436)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsTwoHandsBowAddDamage = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				break;
			}
		}

		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 439)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsTwoHandsBowManAttack = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				return;
			}
		}
	}
	break;
	case ITEM_SUMMONER_STAFF:
	{
		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 465)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsAddMagicPowerStick = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				break;
			}
		}

		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 467)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsAddManAttackStick = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				return;
			}
		}
	}
	break;
	case ITEM_SUMMONER_BOOK:
	{
		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 466)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsAddCurseMagicBook = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				break;
			}
		}

		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 468)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsAddAttackSpeedMagicBook = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				return;
			}
		}
	}
	break;
	case ITEM_DARKLORD_SCEPTER:
	{
		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 527)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsAddCeptorDamage = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				break;
			}
		}

		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 529)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsAddPetAttack = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				break;
			}
		}

		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 531)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsIncCeptorManAttack = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				break;
			}
		}

		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 533)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsAddControlAttack = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				return;
			}
		}
	}
	break;
	case ITEM_RAGEFIGHTER_WEAPON:
	{
		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 568)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsMonkWeaponDamage = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				break;
			}
		}

		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 571)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsMonkDoubleDamage = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				return;
			}
		}
	}
	break;
	default:
		return;
	}
}

void CMasterLevelSkillTreeSystem::SetPetItemMLPassiveSkill(CGameObject &Obj, int iType)
{
	if (iType == ITEMGET(13, 4))
	{
		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 510)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsIncDarkHorseDefense = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				return;
			}
		}
	}
}

void CMasterLevelSkillTreeSystem::SetWingMLPassiveSkill(CGameObject &Obj, int iType)
{
	switch (iType)
	{
	case ITEMGET(12, 36):
	{
		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 324)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsAddWingDamage = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				return;
			}
		}
	}
	break;
	case ITEMGET(12, 37):
	{
		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 377)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsAddWingDamage = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				return;
			}
		}
	}
	break;
	case ITEMGET(12, 38):
	{
		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 412)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsAddWingDamage = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				return;
			}
		}
	}
	break;
	case ITEMGET(12, 39):
	{
		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 478)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsAddWingDamage = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				return;
			}
		}
	}
	break;
	case ITEMGET(12, 40):
	{
		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 507)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsAddWingDamage = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				return;
			}
		}
	}
	break;
	case ITEMGET(12, 43):
	{
		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 447)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsAddWingDamage = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				return;
			}
		}
	}
	break;
	case ITEMGET(12, 50):
	{
		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 550)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsAddWingDamage = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				return;
			}
		}
	}
	break;
	case ITEMGET(12, 270):
	{
		for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
		{
			if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == 686)
			{
				Obj.m_PlayerData->m_MPSkillOpt.iMpsAddWingDamage = this->GetMasterSkillValue(MagicDamageC.SkillGet(Obj.Magic[iCount].m_Skill), Obj.Magic[iCount].m_Level);
				return;
			}
		}
	}
	break;
	default:
		return;
	}
}

int CMasterLevelSkillTreeSystem::GetCurSkillLevel(CGameObject &Obj, int iSkill)
{
	for (int iCount = 0; iCount<MAX_MAGIC; iCount++)
	{
		if (Obj.Magic[iCount].IsMagic() == TRUE && Obj.Magic[iCount].m_Skill == iSkill)
		{
			return Obj.Magic[iCount].m_Level;
		}
	}

	return -1;
}

int CMasterLevelSkillTreeSystem::GetScriptClassCode(int iClass)
{
	if (iClass < 0 || iClass >= MAX_TYPE_PLAYER)
	{
		sLog->outBasic("[MasterLevelSkillTreeSystem] GetScriptClassCode - Invalid Class Code  : %d", iClass);
		return -1;
	}

	for (int i = 0; i<MAX_TYPE_PLAYER; i++)
	{
		if (CHARACTER_CLASS_TABLE[i][1] == iClass)
		{
			return CHARACTER_CLASS_TABLE[i][0];
		}
	}

	return -1;
}

float CMasterLevelSkillTreeSystem::RoundValue(long double dbValue, int iPos)
{
	char szPos[16];
	char szTemp[256];

	sprintf(szPos, "%%.%d", iPos);
	strcat(szPos, "f");
	sprintf(szTemp, szPos, dbValue);

	return atof(szTemp);
}

BYTE CMasterLevelSkillTreeSystem::ResetMasterSkill(CGameObject &Obj, int nTreeType)
{
	

	if (!lpObj)
	{
		return -1;
	}

	if (nTreeType < 0 || nTreeType > 3)
	{
		return -1;
	}

	int iClass = Obj.Class;

	for (int iCount = 0; iCount < MAX_MAGIC; iCount++)
	{
		if (Obj.Magic[iCount].IsMagic() == TRUE && this->CheckMasterLevelSkill(Obj.Magic[iCount].m_Skill) &&
			(nTreeType == 0 || this->CheckSkillInTree(iClass, Obj.Magic[iCount].m_Skill, nTreeType - 1)))
		{
			if (MagicDamageC.GetSkillUseType(Obj.Magic[iCount].m_Skill) == 3)
			{
				Obj.m_PlayerData->MasterPoint += Obj.Magic[iCount].m_Level;
				Obj.Magic[iCount].Clear();
			}

			else
			{
				int iBrandOfSkill = MagicDamageC.GetBrandOfSkill(Obj.Magic[iCount].m_Skill);

				if (iBrandOfSkill > 0)
				{
					if (this->CheckMasterLevelSkill(iBrandOfSkill))
					{
						Obj.m_PlayerData->MasterPoint += Obj.Magic[iCount].m_Level;
						Obj.Magic[iCount].Clear();
					}

					else
					{
						CItemObject * lpLeftItem = &Obj.pntInventory[1];
						CItemObject * lpRightItem = &Obj.pntInventory[0];

						if (lpLeftItem->m_Special[0] == iBrandOfSkill)
						{
							Obj.m_PlayerData->MasterPoint += Obj.Magic[iCount].m_Level;
							Obj.Magic[iCount].UpdateMasterSkill(iBrandOfSkill, lpLeftItem->m_Level);
						}

						else if (lpRightItem->m_Special[0] == iBrandOfSkill)
						{
							Obj.m_PlayerData->MasterPoint += Obj.Magic[iCount].m_Level;
							Obj.Magic[iCount].UpdateMasterSkill(iBrandOfSkill, lpRightItem->m_Level);
						}

						else
						{
							Obj.m_PlayerData->MasterPoint += Obj.Magic[iCount].m_Level;
							Obj.Magic[iCount].UpdateMasterSkill(iBrandOfSkill, 0);
						}
					}
				}

				else
				{
					Obj.m_PlayerData->MasterPoint += Obj.Magic[iCount].m_Level;
					Obj.Magic[iCount].Clear();
				}
			}
		}
	}

	sLog->outBasic("[%s][%s] Master Skill Reset (TreeType: %d)", Obj.AccountID, Obj.Name, nTreeType);
	gObjCloseSet(Obj.m_Index, 1);

	return 0;
}

BYTE CMasterLevelSkillTreeSystem::CheckMasterSkillPoint(CGameObject &Obj, int nTreeType)
{
	
	int nSkillCnt = 0;

	if (!lpObj)
	{
		return -1;
	}

	if (nTreeType < 0 || nTreeType > MAX_TREE_TYPE)
	{
		return -1;
	}

	int iClass = Obj.Class;

	if (nTreeType == 0)
	{
		return 1;
	}

	for (int n = 0; n<MAX_MAGIC; n++)
	{
		if (Obj.Magic[n].IsMagic() == TRUE &&
			this->CheckMasterLevelSkill(Obj.Magic[n].m_Skill) == true &&
			nTreeType > 0)
		{
			if (this->CheckSkillInTree(iClass, Obj.Magic[n].m_Skill, nTreeType - 1))
			{
				nSkillCnt++;
			}
		}
	}

	return nSkillCnt > 0;
}

BYTE CMasterLevelSkillTreeSystem::CheckSkillInTree(int iClass, int iSkill, int iTreeKind)
{
	for (int iRank = 0; iRank < MAX_MASTER_SKILL_RANK; iRank++)
	{
		for (int iPos = 0; iPos < MAX_SKILL_POS; iPos++)
		{
			if (this->m_MLSTable[iClass].iAbility[iTreeKind].iElement[iRank][iPos].m_dwSkillID == iSkill)
			{
				return true;
			}
		}
	}

	return false;
}

float CMasterLevelSkillTreeSystem::GetSkillAttackDamage(CGameObject &Obj, int iSkill)
{
	if (!lpObj)
	{
		return 0.0;
	}

	if (!this->CheckMasterLevelSkill(iSkill))
	{
		return 0.0;
	}

	int nGroup = MagicDamageC.GetSkillGroup(iSkill);
	WORD nBrandOfSkill = iSkill;
	WORD iBrandOfSkill = 0;
	int nCnt = 0;
	BOOL bReturnAddDamage = FALSE;
	int iValueType;

	while (this->CheckMasterLevelSkill(nBrandOfSkill))
	{
		iBrandOfSkill = nBrandOfSkill;

		if (MagicDamageC.GetSkillGroup(nBrandOfSkill) != nGroup)
		{
			iBrandOfSkill = 0;
			break;
		}

		iValueType = MagicDamageC.SkillGet(nBrandOfSkill);

		if (this->m_MLSValueTable[iValueType].iValueType == MLST_VALUE_DAMAGE && nBrandOfSkill != 520)
		{
			bReturnAddDamage = TRUE;
			break;
		}

		nBrandOfSkill = MagicDamageC.GetBrandOfSkill(nBrandOfSkill);
		nCnt++;

		if (nCnt > 2)
		{
			sLog->outBasic("[GetSkillAttackDamage] fail!!! - %d", iSkill);
			iBrandOfSkill = 0;
			break;
		}
	}

	if (iBrandOfSkill > 0)
	{
		if (bReturnAddDamage == TRUE)
		{
			for (int n = 0; n<MAX_MAGIC; n++)
			{
				if (Obj.Magic[n].IsMagic() == TRUE && Obj.Magic[n].m_Skill == iBrandOfSkill)
				{
					return this->GetMasterSkillValue(iValueType, Obj.Magic[n].m_Level);
				}
			}
		}
	}

	return 0.0;
}

void CMasterLevelSkillTreeSystem::SkillCircleShieldGrowLancer(CGameObject &Obj, CMagicInf * lpMagic, int aTargetIndex)
{
	if (ObjectMaxRange(Obj.m_Index) == FALSE)
	{
		return;
	}

	

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.Class != CLASS_GROWLANCER)
	{
		return;
	}

	int skill_improve = 0;
	int skill_improve2 = 0;
	int skill_time = 0;
	
	if (lpMagic->m_Skill == AT_MSKILL_CIRCLE_SHIELD_POWUP)
	{
		this->m_Lua.Generic_Call("GrowLancerCircleShield_Level1", "iii>iii", Obj.m_PlayerData->Strength + Obj.AddStrength, Obj.m_PlayerData->Dexterity + Obj.AddDexterity,
			Obj.m_PlayerData->Energy + Obj.AddEnergy,
			&skill_improve, &skill_improve2, &skill_time);

		Obj.m_SkillInfo.fCircleShieldRate = skill_improve2;
		gObjAddBuffEffect(lpObj, BUFFTYPE_CIRCLE_SHIELD_STR, EFFECTTYPE_AG_OPPONENT_DECREASE, skill_improve, 0, 0, skill_time);
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, Obj.m_Index, TRUE);
	}
	else if (lpMagic->m_Skill == AT_MSKILL_CIRCLE_SHIELD_MASTERY)
	{
		this->m_Lua.Generic_Call("GrowLancerCircleShield_Level2", "iii>iii", Obj.m_PlayerData->Strength + Obj.AddStrength, Obj.m_PlayerData->Dexterity + Obj.AddDexterity,
			Obj.m_PlayerData->Energy + Obj.AddEnergy,
			&skill_improve, &skill_improve2, &skill_time);

		Obj.m_SkillInfo.fCircleShieldRate = skill_improve2;
		gObjAddBuffEffect(lpObj, BUFFTYPE_CIRCLE_SHIELD_MAS, EFFECTTYPE_AG_OPPONENT_DECREASE, skill_improve, 0, 0, skill_time);
		gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, Obj.m_Index, TRUE);
	}
	else
	{
		return;
	}

	//return TRUE;
}

int CMasterLevelSkillTreeSystem::SkillWrathPowUpGrowLancer(CGameObject &Obj, CMagicInf * lpMagic, int aTargetIndex)
{
	if (ObjectMaxRange(Obj.m_Index) == FALSE)
	{
		return FALSE;
	}

	

	if (Obj.Type != OBJ_USER && Obj.m_RecallMon == -1)
	{
		return FALSE;
	}

	if (Obj.Class != CLASS_GROWLANCER)
	{
		return FALSE;
	}

	int skill_improve = 0;
	int skill_improve2 = 0;
	int skill_time = 0;

	if (lpMagic->m_Skill == AT_MSKILL_WRATH_POWUP)
	{
		this->m_Lua.Generic_Call("GrowLancerWrath_Level1", "iii>iii", Obj.m_PlayerData->Strength + Obj.AddStrength, Obj.m_PlayerData->Dexterity + Obj.AddDexterity,
			Obj.m_PlayerData->Energy + Obj.AddEnergy,
			&skill_improve, &skill_improve2, &skill_time);

		gObjAddBuffEffect(lpObj, BUFFTYPE_WRATH_STR, EFFECTTYPE_WRATH_INC_DAMAGE, skill_improve, EFFECTTYPE_DECREASE_DEFENSE, skill_improve2, skill_time);
	}
	else
	{
		return FALSE;
	}

	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, Obj.m_Index, TRUE);
	return TRUE;
}

int CMasterLevelSkillTreeSystem::SkillWrathProficiencyGrowLancer(CGameObject &Obj, CMagicInf * lpMagic, int aTargetIndex)
{
	if (ObjectMaxRange(Obj.m_Index) == FALSE)
	{
		return FALSE;
	}

	

	if (Obj.Type != OBJ_USER)
	{
		return FALSE;
	}

	if (Obj.Class != CLASS_GROWLANCER)
	{
		return FALSE;
	}

	int skill_improve = 0;
	int skill_improve2 = 0;
	int skill_time = 0;

	if (lpMagic->m_Skill == AT_MSKILL_WRATH_PROFICIENCY)
	{
		this->m_Lua.Generic_Call("GrowLancerWrath_Level2", "iii>iii", Obj.m_PlayerData->Strength + Obj.AddStrength, Obj.m_PlayerData->Dexterity + Obj.AddDexterity,
			Obj.m_PlayerData->Energy + Obj.AddEnergy,
			&skill_improve, &skill_improve2, &skill_time);

		gObjAddBuffEffect(lpObj, BUFFTYPE_WRATH_PRO, EFFECTTYPE_WRATH_INC_DAMAGE, skill_improve, EFFECTTYPE_DECREASE_DEFENSE, skill_improve2, skill_time);
	}
	else
	{
		return FALSE;
	}

	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, Obj.m_Index, TRUE);
	return TRUE;
}

int CMasterLevelSkillTreeSystem::SkillWrathMasteryGrowLancer(CGameObject &Obj, CMagicInf * lpMagic, int aTargetIndex)
{
	if (ObjectMaxRange(Obj.m_Index) == FALSE)
	{
		return FALSE;
	}

	

	if (Obj.Type != OBJ_USER)
	{
		return FALSE;
	}

	if (Obj.Class != CLASS_GROWLANCER)
	{
		return FALSE;
	}

	int skill_improve = 0;
	int skill_improve2 = 0;
	int skill_time = 0;

	this->m_Lua.Generic_Call("GrowLancerWrath_Level3", "iii>iii", Obj.m_PlayerData->Strength + Obj.AddStrength, Obj.m_PlayerData->Dexterity + Obj.AddDexterity,
		Obj.m_PlayerData->Energy + Obj.AddEnergy,&skill_improve, &skill_improve2, &skill_time);

	gObjAddBuffEffect(lpObj, BUFFTYPE_WRATH_MAS, EFFECTTYPE_WRATH_INC_DAMAGE, skill_improve, EFFECTTYPE_DECREASE_DEFENSE, skill_improve2, skill_time);
	
	gGameProtocol.GCMagicAttackNumberSend(lpObj, AT_MSKILL_WRATH_MASTERY, Obj.m_Index, TRUE);
	return TRUE;
}

int CMasterLevelSkillTreeSystem::SkillObsidianPowUpGrowLancer(CGameObject &Obj, CMagicInf * lpMagic, int aTargetIndex)
{
	if (Obj.Type != OBJ_USER)
	{
		return false;
	}

	if (Obj.Class != CLASS_GROWLANCER)
	{
		return false;
	}

	int partynum = 0;
	int partyindex[5];

	memset(partyindex, -1, sizeof(partyindex));

	partynum = Obj.PartyNumber;

	if (partynum != -1)
	{
		for (int i = 0; i < MAX_USER_IN_PARTY; i++)
		{
			int index = gParty.m_PartyS[partynum].Number[i];

			if (index >= 0 && index < g_ConfigRead.server.GetObjectMax())
			{
				CGameObject lpPartyObj = &getGameObject(index);

				if (lpPartyObj.MapNumber == Obj.MapNumber)
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
	
	this->m_Lua.Generic_Call("GrowLancerObsidian_Level1", "iii>ii", (Obj.m_PlayerData->Strength + Obj.AddStrength), (Obj.m_PlayerData->Dexterity + Obj.AddDexterity),
		(Obj.m_PlayerData->Energy + Obj.AddEnergy), &skill_improve, &skill_time);

	if (partynum == -1)
	{
		gObjAddBuffEffect(lpObj, BUFFTYPE_OBSIDIAN_STR, EFFECTTYPE_IMPROVE_DAMAGE, skill_improve, 0, 0, skill_time);
	}
	else
	{
		for (int j = 0; j < 5; j++)
		{
			if (partyindex[j] != -1)
			{
				CGameObject lpPartyObj = &getGameObject(partyindex[j]);

				gObjAddBuffEffect(lpPartyObj, BUFFTYPE_OBSIDIAN_STR, EFFECTTYPE_IMPROVE_DAMAGE, skill_improve, 0, 0, skill_time);
			}
		}
	}

	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, Obj.m_Index, TRUE);
	return TRUE;
}

void CMasterLevelSkillTreeSystem::SkillBurstMasteryGrowLancer(CGameObject &Obj, CMagicInf * lpMagic, int aTargetIndex)
{
	if (ObjectMaxRange(aTargetIndex) == false)
	{
		return;
	}

	CGameObject lpObj = &getGameObject(aTargetIndex);

	float AddDefense = 5.0f;
	int AddTime = 60;

	int BuffNumber = BUFFTYPE_BURST;

	if (lpMagic->m_Skill == AT_MSKILL_BURST_POWUP)
	{
		BuffNumber = BUFFTYPE_BURST_STR;

		int fSkillValue = this->GetMasterSkillValue(MagicDamageC.SkillGet(lpMagic->m_Skill), lpMagic->m_Level);

		AddTime -= fSkillValue;
	}

	gObjAddBuffEffect(lpObj, BuffNumber, EFFECTTYPE_IMPROVE_DEFENSE, AddDefense, 0, 0, AddTime);
	gGameProtocol.GCMagicAttackNumberSend(lpObj, lpMagic->m_Skill, aTargetIndex, TRUE);
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

