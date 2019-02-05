////////////////////////////////////////////////////////////////////////////////
// MasterSkillSystem.h
#ifndef MASTER_SKILL_SYSTEM
#define MASTER_SKILL_SYSTEM

#pragma pack (1)

enum eMasterSkillType
{
	// DK MSKILL
	AT_MSKILL_DK_CYCLONE1 = 326,
	AT_MSKILL_DK_SLASH1 = 327,
	AT_MSKILL_DK_FALLINGSLASH1 = 328,
	AT_MSKILL_DK_LUNGE1 = 329,
	AT_MSKILL_DK_TWISTINGSLASH1 = 330,
	AT_MSKILL_DK_RAGEFULBLOW1 = 331,
	AT_MSKILL_DK_TWISTINGSLASH2 = 332,
	AT_MSKILL_DK_RAGEFULBLOW2 = 333,
	AT_MSKILL_DK_DEATHSTAB1 = 336,
	AT_MSKILL_DK_DEATHSTAB2 = 339,
	AT_MSKILL_DK_DEATHSTAB3 = 342,
	AT_MSKILL_DK_DESTRUCTION1 = 337,
	AT_MSKILL_DK_DESTRUCTION2 = 340,
	AT_MSKILL_DK_BLOODSTORM = 344,
	AT_MSKILL_DK_BLOODSTORM1 = 346,
	AT_MSKILL_DK_DESTRUCTION3 = 343,
	AT_MSKILL_MACE_STRENGTHENER = 350,
	AT_MSKILL_DK_SWELLLIFE1 = 356,
	AT_MSKILL_DK_SWELLLIFE2 = 360,
	AT_MSKILL_DK_SWELLLIFE3 = 363,
	AT_MSKILL_DK_IRONDEFENSE = 632,
	AT_MSKILL_DK_IRONDEFENSE1 = 633,
	// DW MSKILL
	AT_MSKILL_DW_FLAME1 = 378,
	AT_MSKILL_DW_LIGHTNING1 = 379,
	AT_MSKILL_DW_EXPWIZARDRY1 = 380,
	AT_MSKILL_DW_INFERNO1 = 381,
	AT_MSKILL_DW_BLAST1 = 382,
	AT_MSKILL_DW_EXPWIZARDRY2 = 383,
	AT_MSKILL_DW_POISON1 = 384,
	AT_MSKILL_DW_EVILSPIRIT1 = 385,
	AT_MSKILL_DW_DECAY1 = 387,
	AT_MSKILL_DW_HELLFIRE1 = 388,
	AT_MSKILL_DW_ICE1 = 389,
	AT_MSKILL_DW_METEORIT1 = 390,
	AT_MSKILL_DW_ICESTORM1 = 391,
	AT_MSKILL_DW_NOVA1 = 392,
	AT_MSKILL_DW_ICESTORM2 = 393,
	AT_MSKILL_DW_METEORIT2 = 394,
	AT_MSKILL_DW_MANASHIELD1 = 403,
	AT_MSKILL_DW_MANASHIELD2 = 404,
	AT_MSKILL_DW_MANASHIELD3 = 406,
	AT_MSKILL_DW_EARTHPRISON = 495,
	AT_MSKILL_DW_EARTHPRISON1 = 497,
	// FE MSKILL
	AT_MSKILL_FE_HEAL1 = 413,
	AT_MSKILL_FE_TRIPLESHOT1 = 414,
	AT_MSKILL_FE_PENETRATION1 = 416,
	AT_MSKILL_FE_DEFENSEBUFF1 = 417,
	AT_MSKILL_FE_TRIPLESHOT2 = 418,
	AT_MSKILL_FE_ATTACKBUFF1 = 420,
	AT_MSKILL_FE_ATTACKBUFF2 = 422,
	AT_MSKILL_FE_DEFENSEBUFF2 = 423,
	AT_MSKILL_FE_ICEARROW1 = 424,
	AT_MSKILL_FE_CURE = 425,
	AT_MSKILL_FE_PARTYHEAL = 426,
	AT_MSKILL_FE_PARTYHEAL1 = 429,
	AT_MSKILL_FE_POISONARROW = 427,
	AT_MSKILL_FE_POISONARROW1 = 434,
	AT_MSKILL_FE_BLESS = 430,
	AT_MSKILL_FE_BLESS1 = 433,
	AT_MSKILL_FE_MULTISHOT1 = 411,
	AT_MSKILL_FE_MULTISHOT2 = 431,
	AT_MSKILL_FE_SUMMONSATYROS = 432,
	AT_MSKILL_FE_INFINITY1 = 441,
	// SUM MSKILL
	AT_MSKILL_SUM_SLEEP1 = 454,
	AT_MSKILL_SUM_CHAINLIGHTNING1 = 455,
	AT_MSKILL_SUM_LIGHTNSHOCK1 = 456,
	AT_MSKILL_SUM_DRAINLIFE1 = 458,
	AT_MSKILL_SUM_DRAINLIFE2 = 462,
	AT_MSKILL_SUM_WEAKNESS1 = 459,
	AT_MSKILL_SUM_INNOVATION1 = 460,
	AT_MSKILL_SUM_BLIND = 461,
	AT_MSKILL_SUM_BLIND1 = 463,
	AT_MSKILL_SUM_BERSERK1 = 469,
	AT_MSKILL_SUM_BERSERK2 = 470,
	AT_MSKILL_SUM_BERSERK3 = 472,
	// MG MSKILL
	AT_MSKILL_MG_CYCLONE1 = 479,
	AT_MSKILL_MG_LIGHTNING1 = 480,
	AT_MSKILL_MG_TWISTINGSLASH1 = 481,
	AT_MSKILL_MG_POWERSLASH1 = 482,
	AT_MSKILL_MG_FLAME1 = 483,
	AT_MSKILL_MG_BLAST1 = 484,
	AT_MSKILL_MG_INFERNO1 = 486,
	AT_MSKILL_MG_EVILSPIRIT1 = 487,
	AT_MSKILL_MG_ICE1 = 489,
	AT_MSKILL_MG_ICE2 = 491,
	AT_MSKILL_MG_FIRESLASH1 = 490,
	AT_MSKILL_MG_FIRESLASH2 = 493,
	AT_MSKILL_MG_FLAMESTRIKE1 = 492,
	AT_MSKILL_MG_FLAMESTRIKE2 = 494,
	AT_MSKILL_MG_GIGANTICSTORM1 = 496,
	// DL MSKILL
	AT_MSKILL_DL_504 = 504, //Emperor Cape Absorption PowUp
	AT_MSKILL_DL_505 = 505, //Emperor Cape Defense PowUp
	AT_MSKILL_DL_506 = 506, //Adds Command Stat
	AT_MSKILL_DL_507 = 507, //Emperor Cape Attack PowUp
	AT_MSKILL_DL_FIREBURST1 = 508,
	AT_MSKILL_DL_FORCEWAVE1 = 509,
	AT_MSKILL_DL_CRITICALDAMAGE1 = 511,
	AT_MSKILL_DL_EARTHSHAKE1 = 512,
	AT_MSKILL_DL_FIREBURST2 = 514,
	AT_MSKILL_DL_CRITICALDAMAGE2 = 515,
	AT_MSKILL_DL_EARTHSHAKE2 = 516,
	AT_MSKILL_DL_CRITICALDAMAGE3 = 517,
	AT_MSKILL_DL_FIRESCREAM1 = 518,
	AT_MSKILL_DL_ELECTRICSPARK1 = 519,
	AT_MSKILL_DL_FIRESCREAM2 = 520,
	AT_MSKILL_DL_IRONDEFENSE = 521,
	AT_MSKILL_DL_IRONDEFENSE1 = 524,
	AT_MSKILL_DL_CRITICALDAMAGE4 = 522,
	AT_MSKILL_DL_CHAOTICDESEAR1 = 523,
	// DL SKILL 2
	AT_MSKILL_DL_524 = 524, //Iron Defense Strengthener
	AT_MSKILL_DL_526 = 526, //Dark Spirit Strengthener
	AT_MSKILL_DL_527 = 527, //Scepter Strengthener
	AT_MSKILL_DL_528 = 528, //Shield Strengthener
	AT_MSKILL_DL_529 = 529, //Use Scepter : Pet Strengthener
	AT_MSKILL_DL_530 = 530, //Dark Spirit Strengthener (2)
	AT_MSKILL_DL_531 = 531, //Scepter Mastery
	AT_MSKILL_DL_532 = 532, //Shield Mastery
	AT_MSKILL_DL_533 = 533, //Command Attack Increase
	AT_MSKILL_DL_534 = 534, //Dark Spirit Strengthener (3)
	AT_MSKILL_DL_535 = 535, //Pet Durability Strengthener
	AT_MSKILL_DL_536 = 536, //Dark Spirit Strengthener (4)
	AT_MSKILL_DL_538 = 538, //Dark Spirit Strengthener (5)
	AT_MSKILL_DL_539 = 539, //Spirit Lord
	// RF SKILL
	AT_MSKILL_RF_CLOAKABSPOWUP = 548,
	AT_MSKILL_RF_CLOAKDEFPOWUP = 549,
	AT_MSKILL_RF_CLOAKATTAPOWUP = 550,
	AT_MSKILL_RF_KILLBLOW1 = 551,
	AT_MSKILL_RF_KILLBLOW2 = 554,
	AT_MSKILL_RF_UPERCUT1 = 552,
	AT_MSKILL_RF_UPERCUT2 = 555,
	AT_MSKILL_RF_CHAINDRIVE1 = 558,
	AT_MSKILL_RF_CHAINDRIVE2 = 562,
	AT_MSKILL_RF_DARKSIDE1 = 559,
	AT_MSKILL_RF_DARKSIDE2 = 563,
	AT_MSKILL_RF_DRAGONRORE1 = 560,
	AT_MSKILL_RF_DRAGONRORE2 = 561,
	AT_MSKILL_RF_DRAGONSLASH1 = 564,
	AT_MSKILL_RF_DRAGONSLASH2 = 565,
	AT_MSKILL_RF_DEFRATE1 = 569,
	AT_MSKILL_RF_DEFRATE2 = 572,
	AT_MSKILL_RF_FITNESS1 = 573,
	// rf skill 2
	AT_MSKILL_RF_574 = 574, //Defense Switch
	AT_MSKILL_RF_578 = 578, //Durability Reduction (1)
	AT_MSKILL_RF_579 = 579, //Increase PvP Defense Rate
	AT_MSKILL_RF_580 = 580, //Increase Maximum SD
	AT_MSKILL_RF_581 = 581, //Increase Mana Recovery Rate
	AT_MSKILL_RF_582 = 582, //Increase Poison Resistance
	AT_MSKILL_RF_583 = 583, //Durability Reduction (2)
	AT_MSKILL_RF_584 = 584, //Increase SD Recovery Rate
	AT_MSKILL_RF_585 = 585, //Increase HP Recovery Rate
	AT_MSKILL_RF_586 = 586, //Increase Lightning Resistance
	AT_MSKILL_RF_587 = 587, //Increases Defense
	AT_MSKILL_RF_588 = 588, //Increases AG Recovery Rate
	AT_MSKILL_RF_589 = 589, //Increase Ice Resistance
	AT_MSKILL_RF_590 = 590, //Durability Reduction(3)
	AT_MSKILL_RF_591 = 591, //Increase Defense Success Rate
	AT_MSKILL_RF_592 = 592, //Cast Invincibility
	AT_MSKILL_RF_593 = 593, //Increase Set Defense
	AT_MSKILL_RF_594 = 594, //Vengeance
	AT_MSKILL_RF_595 = 595, //Increase Energy
	AT_MSKILL_RF_596 = 596, //Stamina Increases
	AT_MSKILL_RF_597 = 597, //Increase Agility
	AT_MSKILL_RF_598 = 598, //Increase Strength
	AT_MSKILL_RF_599 = 599, //Increase Attack Success Rate
	AT_MSKILL_RF_600 = 600, //Increase Maximum HP
	AT_MSKILL_RF_601 = 601, //Increase Maximum Mana
	AT_MSKILL_RF_602 = 602, //Increase Maximum AG
	AT_MSKILL_RF_603 = 603, //Increase PvP Attack Rate
	AT_MSKILL_RF_604 = 604, //Decrease Mana
	AT_MSKILL_RF_605 = 605, //Recover SD from Monster Kills
	AT_MSKILL_RF_606 = 606, //Recover HP from Monster Kills
	AT_MSKILL_RF_607 = 607, //Increase Minimum Attack Power
	AT_MSKILL_RF_608 = 608, //Recover Mana from Monster Kills
	AT_MSKILL_RF_609 = 609, //Increase Maximum Attack Power
	AT_MSKILL_RF_610 = 610, //Increases Critical DMG Chance
	AT_MSKILL_RF_611 = 611, //Recover Mana Fully
	AT_MSKILL_RF_612 = 612, //Recovers HP Fully
	AT_MSKILL_RF_613 = 613, //Increase Excellent DMG Chance
	AT_MSKILL_RF_614 = 614, //Increase Double Damage Chance
	AT_MSKILL_RF_615 = 615, //Increase Ignore Def Chance
	AT_MSKILL_RF_616 = 616, //Recovers SD Fully
	AT_MSKILL_RF_617 = 617, //Increase Triple Damage Chance
	AT_MSKILL_ILLUSION = 642, // Illusion
	AT_MSKILL_RF_677 = 677, //Shield Block
	AT_MSKILL_RF_678 = 678, //Weapon Block
	AT_MSKILL_RF_679 = 679, //Protection Shield
	AT_MSKILL_RF_680 = 680, //Steel Armor
	AT_MSKILL_RF_681 = 681, //Strong Mind
	AT_MSKILL_RF_682 = 682, //Absorb Life
	AT_MSKILL_RF_683 = 683, //Absorb Shield
	// LANCER SKILL
	AT_MSKILL_GL_684 = 684, //Cloak of Transcendence Absorption PowUp
	AT_MSKILL_GL_685 = 685, //Cloak of Transcendence Defense PowUp
	AT_MSKILL_GL_686 = 686, //Cloak of Transcendence Attack PowUp
	AT_MSKILL_GL_687 = 687, //Spin Step PowUpHarsh Strike PowUp
	AT_MSKILL_GL_688 = 688, //Harsh Strike PowUp
	AT_MSKILL_GL_689 = 689, //Weapon Mastery
	AT_MSKILL_GL_690 = 690, //Spin Step Mastery
	AT_MSKILL_GL_691 = 691, //Harsh Strike Mastery
	AT_MSKILL_GL_692 = 692, //Magic Pin PowUp
	AT_MSKILL_OBSIDIAN_POWUP = 693, //Obsidian PowUp
	AT_MSKILL_GL_694 = 694, //Magic Pin Proficiency
	AT_MSKILL_GL_695 = 695, //Magic Pin Mastery
	AT_MSKILL_GL_696 = 696, //Breche PowUp
	AT_MSKILL_GL_697 = 697, //Breche Proficiency
	AT_MSKILL_GL_698 = 698, //Breche Mastery
	AT_MSKILL_GL_699 = 699, //Shining Peak PowUp
	AT_MSKILL_BURST = 700, //Burst
	AT_MSKILL_BURST_POWUP = 701, //Burst PowUp
	AT_MSKILL_GL_702 = 702, //Lance PowUp
	AT_MSKILL_CIRCLE_SHIELD_POWUP = 703, //Circle Shield PowUp
	AT_MSKILL_GL_704 = 704, //Shield PowUp
	AT_MSKILL_GL_705 = 705, //Lance Mastery
	AT_MSKILL_CIRCLE_SHIELD_MASTERY = 706, //Circle Shield Mastery
	AT_MSKILL_GL_707 = 707, //Shield Mastery
	AT_MSKILL_WRATH_POWUP = 708, //Wrath PowUp
	AT_MSKILL_WRATH_PROFICIENCY = 709, //Wrath Proficiency
	AT_MSKILL_WRATH_MASTERY = 710, //Wrath Mastery
    AT_MSKILL_GL_711 = 711, //Increases Retaliation DMG
    AT_MSKILL_GL_712 = 712, //Increases Rage DMG
    AT_MSKILL_GL_713 = 713, //Shield Block
    AT_MSKILL_GL_714 = 714, //Weapon Block
    AT_MSKILL_GL_715 = 715, //Protection Shield
    AT_MSKILL_GL_716 = 716, //Steel Armor
    AT_MSKILL_GL_717 = 717, //Strong Mind
    AT_MSKILL_GL_718 = 718, //Absorb Life
    AT_MSKILL_GL_719 = 719, //Absorb Shield
    // skills
    AT_MSKILL_GL_720 = 720, //Immune I
    AT_MSKILL_GL_721 = 721, //Immune II
    AT_MSKILL_GL_722 = 722, //Berserker I
};

struct PMSG_MASTER_INFO_SEND
{
    PBMSG_HEAD2 h; //C1:F3:50
	short MasterLevel;
	BYTE MLExpHHH;
	BYTE MLExpHHL;
	BYTE MLExpHLH;
	BYTE MLExpHLL;
	BYTE MLExpLHH;
	BYTE MLExpLHL;
	BYTE MLExpLLH;
	BYTE MLExpLLL;
	BYTE MLNextExpHHH;
	BYTE MLNextExpHHL;
	BYTE MLNextExpHLH;
	BYTE MLNextExpHLL;
	BYTE MLNextExpLHH;
	BYTE MLNextExpLHL;
	BYTE MLNextExpLLH; 
	BYTE MLNextExpLLL;
	WORD MasterPoint;
	WORD MaxLife;
	WORD MaxMana;
	WORD MaxShield;
	WORD MaxBP;
	int IGCLife;
	int IGCMana;
};

struct PMSG_MASTER_LEVEL_UP_SEND
{
    PBMSG_HEAD2 h;
	WORD MasterLevel;
	WORD GainPoint;
	WORD MLPoint;
	WORD MaxPoint;
	WORD MaxLife;
	WORD MaxMana;
	WORD MaxShield;
    WORD MaxBP;
	int IGCMaxLife;
	int IGCMaxMana;
};

struct PMSG_MASTER_SKILL_RECV
{
    PBMSG_HEAD2 h; //C1:F3:52
	WORD MasterSkill;
    WORD MasterEmpty;
};

struct PMSG_MASTER_SKILL_SEND
{
    PBMSG_HEAD2 h; //C1:F3:52
    BYTE type;
	BYTE flag;
	WORD MasterPoint;
	WORD MasterSkill;
    WORD MasterEmpty;
	DWORD ChkSum;
};

#pragma pack ()
class CMasterLevelSystem
{
public:
	CMasterLevelSystem(void);
	virtual ~CMasterLevelSystem(void);
	bool MasterLevelUp(LPOBJ lpObj, UINT64 addexp, int iMonsterType, const char* szEventType);
	UINT64 gObjNextMLExpCal(LPOBJ lpObj);
	bool IsMasterLevelUser(LPOBJ lpObj);
	bool CheckMLGetExp(LPOBJ lpObj, LPOBJ lpTargetObj);
	void SetExpTable();

	void SendMLData(LPOBJ lpObj);
	void InitData(LPOBJ lpObj);
	int GetDieDecExpRate(LPOBJ lpObj);
	int GetDieDecMoneyRate(LPOBJ lpObj);

	UINT64 * gMasterExperience;
};
//extern CMasterSkillSystem g_MasterLevelSystem;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

