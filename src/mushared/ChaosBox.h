////////////////////////////////////////////////////////////////////////////////
// ChaosBox.h
#ifndef CHAOSBOX_H
#define CHAOSBOX_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "protocol.h"
#include "zzzitem.h"

#define MAX_TALISMAN_OF_LUCK 10
#define CHAOS_BOX_SIZE 32
#define MIN_CHAOS_ITEM_LEVEL 4
#define BLOODANGEL_MIX_LIST_SIZE 32
#define BLOODANGEL_MIX_LIST_SIZE2 40

#define CHAOS_BOX_RANGE(x) (((x)<0)?FALSE:((x)>CHAOS_BOX_SIZE-1)?FALSE:TRUE )

enum CHAOS_TYPE
{
	CHAOS_TYPE_DEFAULT = 0x1,
	CHAOS_TYPE_DEVILSQUARE = 0x2,
	CHAOS_TYPE_UPGRADE_10 = 0x3,
	CHAOS_TYPE_UPGRADE_11 = 0x4,
	CHAOS_TYPE_UPGRADE_12 = 0x16,
	CHAOS_TYPE_UPGRADE_13 = 0x17,
	CHAOS_TYPE_DINORANT = 0x5,
	CHAOS_TYPE_FRUIT = 0x6,
	CHAOS_TYPE_SECOND_WING = 0x7,
	CHAOS_TYPE_BLOODCATLE = 0x8,
	CHAOS_TYPE_FIRST_WING = 0xb,
	CHAOS_TYPE_SETITEM = 0xc,
	CHAOS_TYPE_DARKHORSE = 0xd,
	CHAOS_TYPE_DARKSPIRIT = 0xe,
	CHAOS_TYPE_CLOAK = 0x18,
	CHAOS_TYPE_BLESS_POTION = 0xf,
	CHAOS_TYPE_SOUL_POTION = 0x10,
	CHAOS_TYPE_LIFE_STONE = 0x11,
	CHAOS_TYPE_CASTLE_SPECIAL_ITEM_MIX = 0x12,
	CHAOS_TYPE_HT_BOX = 0x14,
	CHAOS_TYPE_FENRIR_01 = 0x19,
	CHAOS_TYPE_FENRIR_02 = 0x1a,
	CHAOS_TYPE_FENRIR_03 = 0x1b,
	CHAOS_TYPE_FENRIR_04 = 0x1c,
	CHAOS_TYPE_COMPOUNDPOTION_LV1 = 0x1e,
	CHAOS_TYPE_COMPOUNTPOTION_LV2 = 0x1f,
	CHAOS_TYPE_COMPOUNTPOTION_LV3 = 0x20,
	CHAOS_TYPE_JEWELOFHARMONY_PURITY = 0x21,
	CHAOS_TYPE_JEWELOFHARMONY_MIX_SMELTINGITEM = 0x22,
	CHAOS_TYPE_JEWELOFHARMONY_RESTORE_ITEM = 0x23,
	CHAOS_TYPE_380_OPTIONITEM = 0x24,
	CHAOS_TYPE_OLD_PAPER = 0x25,
	CHAOS_TYPE_CONDOR_FEATHER = 0x26,
	CHAOS_TYPE_THIRD_WING = 0x27,
	CHAOS_TYPE_LOTTERY_MIX = 0x28,
	CHAOS_TYPE_BLOSSOM_MIX = 0x29,
	CHAOS_TYPE_SEED_EXTRACT = 0x2a,
	CHAOS_TYPE_SEED_SPHERE_COMPOSITE = 0x2b,
	CHAOS_TYPE_SET_SEED_SPHERE = 0x2c,
	CHAOS_TYPE_SEED_SPHERE_REMOVE = 0x2d,
	CHAOS_TYPE_SECROMICON = 0x2e,
	CHAOS_TYPE_PREMIUM_BOX = 0x2f,
	CHAOS_TYPE_SUMMONS_MIX = 0x30,
	CHAOS_TYPE_UPGRADE_14 = 0x31,
	CHAOS_TYPE_UPGRADE_15 = 0x32,
	CHAOS_TYPE_LUCKYITEM_EXCHANGE = 0x33,
	CHAOS_TYPE_LUCKYITEM_REFINE = 0x34,
	CHAOS_TYPE_ARCABATTLE_BOOTYMIX = 0x36,
	CHAOS_TYPE_ARCABATTLE_SIGNOFLORD = 0x37,
	CHAOS_TYPE_MONSTERWING = 0x38,
	CHAOS_TYPE_SOCKETWEAPON = 0x39,
	CHAOS_TYPE_HEARTBEAT_BOX = 0x3A,
	CHAOS_TYPE_CCF_REWARD = 0x4B,
	CHAOS_TYPE_DSF_REWARD = 0x4C,
	CHAOS_TYPE_ANCIENT_SOUL_ITEM = 0x4D,
	CHAOS_TYPE_BLOODANGEL_ITEM_MIX_START = 0x4D,
	CHAOS_TYPE_BLOODANGEL_ITEM_MIX_END = 0x85,
	CHAOS_TYPE_ADVANCED_WING = 0xC8
};

struct PMSG_CHAOSMIXRESULT
{
	PBMSG_HEAD h;	// C1:86
	BYTE Result;	// 3
	BYTE ItemInfo[MAX_ITEM_INFO];	// 4
	BYTE Pos; // Season X addon, for Massive Combination
};
struct PMSG_SEEDRESULT
{
	PBMSG_HEAD h;	// C1:86
	BYTE Result;	// 3
	BYTE Result1;
};
static struct BLOODANGEL_MIX_LIST2
{
	int iMixType;
	int iNeedItemType;
	int iResultItemType;
} g_BloodAngelMixList2[BLOODANGEL_MIX_LIST_SIZE2] = {

	// sets dark knight
	215, ITEMGET(13,351), ITEMGET(8, 98), // 7007
	208, ITEMGET(13,352), ITEMGET(7, 98), // 7008
	223, ITEMGET(13,353), ITEMGET(9, 98), // 7009
	237, ITEMGET(13,354), ITEMGET(11, 98), // 7010
	
	//sets dark wizard
	216, ITEMGET(13,359), ITEMGET(8, 99), // 7015
	209, ITEMGET(13,360), ITEMGET(7, 99), // 7016
	224, ITEMGET(13,361), ITEMGET(9, 99), // 7017
	231, ITEMGET(13,362), ITEMGET(10, 99), // 7018

	// sets fairy elf
	217, ITEMGET(13,363), ITEMGET(8, 100), // 7019
	210, ITEMGET(13,364), ITEMGET(7, 100), // 7020
	225, ITEMGET(13,365), ITEMGET(9, 100), // 7021
	232, ITEMGET(13,366), ITEMGET(10, 100), // 7022
	240, ITEMGET(13,367), ITEMGET(8, 100), // 7023
	241, ITEMGET(13,368), ITEMGET(7, 100), // 7024
	242, ITEMGET(13,369), ITEMGET(9, 100), // 7025
	243, ITEMGET(13,370), ITEMGET(10, 100), // 7026

	// sets summoner
	218, ITEMGET(13,387), ITEMGET(8, 101), // 7043
	211, ITEMGET(13,388), ITEMGET(7, 101), // 7044
	226, ITEMGET(13,389), ITEMGET(9, 101), // 7045
	233, ITEMGET(13,390), ITEMGET(10, 101), // 7046

	// sets darklord
	220, ITEMGET(13,379), ITEMGET(8, 103), // 7035
	212, ITEMGET(13,380), ITEMGET(7, 103), // 7036
	228, ITEMGET(13,381), ITEMGET(9, 103), // 7037
	235, ITEMGET(13,382), ITEMGET(10, 103), // 7038

	// sets ragefighter
	221, ITEMGET(13,391), ITEMGET(8, 104), // 7047
	213, ITEMGET(13,392), ITEMGET(7, 104), // 7048
	229, ITEMGET(13,393), ITEMGET(9, 104), // 7049
	239, ITEMGET(13,394), ITEMGET(11, 104), // 7050

	//sets grow lancer
	222, ITEMGET(13,399), ITEMGET(8, 105), // 7055
	214, ITEMGET(13,400), ITEMGET(7, 105), // 7056
	230, ITEMGET(13,401), ITEMGET(9, 105), // 7057
	236, ITEMGET(13,402), ITEMGET(10, 105), // 7058

	// sets magic
	219, ITEMGET(13,371), ITEMGET(8, 102), // 7027
	227, ITEMGET(13,373), ITEMGET(9, 102), // 7029
	234, ITEMGET(13,374), ITEMGET(10, 102), // 7030
	238, ITEMGET(13,372), ITEMGET(11, 102), // 7028
	
	244, ITEMGET(13,375), ITEMGET(8, 102), // 7031
	245, ITEMGET(13,377), ITEMGET(9, 102), // 7033
	246, ITEMGET(13,378), ITEMGET(10, 102), // 7034
	247, ITEMGET(13,376), ITEMGET(11, 102), // 7032

};

static struct BLOODANGEL_MIX_LIST
{
	int iMixType;
	int iNeedItemType;
	int iResultItemType;
} g_BloodAngelMixList[BLOODANGEL_MIX_LIST_SIZE] = {
	78, ITEMGET(13,351), ITEMGET(8, 98),
	79, ITEMGET(13,352), ITEMGET(7, 98),
	80, ITEMGET(13,353), ITEMGET(9, 98),
	81, ITEMGET(13,354), ITEMGET(11, 98),

	86, ITEMGET(13,359), ITEMGET(8, 99),
	87, ITEMGET(13,360), ITEMGET(7, 99),
	88, ITEMGET(13,361), ITEMGET(9, 99),
	89, ITEMGET(13,362), ITEMGET(10, 99),

	90, ITEMGET(13,363), ITEMGET(8, 100),
	91, ITEMGET(13,364), ITEMGET(7, 100),
	92, ITEMGET(13,365), ITEMGET(9, 100),
	93, ITEMGET(13,366), ITEMGET(10, 100),

	98, ITEMGET(13,371), ITEMGET(8, 102),
	99, ITEMGET(13,372), ITEMGET(11, 102),
	100, ITEMGET(13,373), ITEMGET(9, 102),
	101, ITEMGET(13,374), ITEMGET(10, 102),

	106, ITEMGET(13,379), ITEMGET(8, 103),
	107, ITEMGET(13,380), ITEMGET(7, 103),
	108, ITEMGET(13,381), ITEMGET(9, 103),
	109, ITEMGET(13,382), ITEMGET(10, 103),

	114, ITEMGET(13,387), ITEMGET(8, 101),
	115, ITEMGET(13,388), ITEMGET(7, 101),
	116, ITEMGET(13,389), ITEMGET(9, 101),
	117, ITEMGET(13,390), ITEMGET(10, 101),

	118, ITEMGET(13,391), ITEMGET(8, 104),
	119, ITEMGET(13,392), ITEMGET(7, 104),
	120, ITEMGET(13,393), ITEMGET(9, 104),
	121, ITEMGET(13,394), ITEMGET(11,104),

	126, ITEMGET(13,399), ITEMGET(8, 105),
	127, ITEMGET(13,400), ITEMGET(7,105),
	128, ITEMGET(13,401), ITEMGET(9,105),
	129, ITEMGET(13,402), ITEMGET(10, 105)
};

#define CB_ERROR					0
#define CB_SUCCESS					1
#define CB_NOT_ENOUGH_ZEN			2
#define CB_TOO_MANY_ITEMS			3
#define CB_LOW_LEVEL_USER			4
#define CB_LACKING_MIX_ITEMS		6
#define CB_INCORRECT_MIX_ITEMS		7
#define CB_INVALID_ITEM_LEVEL		8
#define CB_USER_CLASS_LOW_LEVEL		9
#define CB_NO_BC_CORRECT_ITEMS		10
#define CB_BC_NOT_ENOUGH_ZEN		11
#define CB_MULTIMIX_RESULT			16
#define CB_ARCA_MARK_REG_SUCCESS	193
#define CB_ARCA_MARK_REG_ERROR		194
#define CB_EL_UPGRADEFAIL			225
#define CB_EL_REFINEFAIL			226
#define CB_EL_FAIL_CHAOSASSEMBLY	227
#define CB_NOT_ENOUGH_EMPTY_SPACEX	240
#define CB_NOT_ENOUGH_EMPTY_SPACE	241
#define CB_EL_COMBINATIONFAIL		247
#define CB_EL_SCRIPTERROR			248
#define CB_EL_NOT_ENOUGH_ZEN		249
#define CB_EL_LACKING_MIX_ITEMS		250
#define CB_EL_INCORRECT_MIX_ITEMS	251
#define CB_EL_SCRIPTERROR_2			252
#define CB_EL_PERSONALSTORE			253
#define CB_EL_ALREADY_OPEN			254

class CMixSystem
{
public:
	CMixSystem();
	virtual ~CMixSystem();

	void GCChaosMixSend(int aIndex, BYTE result, CItem* lpItem);
	BOOL ChaosBoxCheck(LPOBJ lpObj);
	BOOL ChaosBoxInit(LPOBJ lpObj);
	BOOL ChaosBoxItemDown(LPOBJ lpObj);
	int ChaosBoxMix(LPOBJ lpObj, int & Result2, int & WingNum);
	int  CheckDevilSquareItem(LPOBJ lpObj, int & eventitemcount, int & itemlevel);
	void LogDQChaosItem(LPOBJ lpObj);
	BOOL DevilSquareEventChaosMix(LPOBJ lpObj, BOOL bCheckType, int iItemLevel);
	void LogChaosItem(LPOBJ lpObj, LPSTR sLogType);
	BOOL PlusItemLevelChaosMix(LPOBJ lpObj, int mixType);
	void PlusItemLevelMixTest(LPOBJ lpObj, int type);
	void ChaosMixCharmItemUsed(LPOBJ lpObj);
	BOOL IsDeleteItem(int iItemCode);
	BOOL PegasiaChaosMix(LPOBJ lpObj);
	BOOL CircleChaosMix(LPOBJ lpObj);
	BOOL WingChaosMix(LPOBJ lpObj);
	BOOL IllusionTempleItemChaosMix(LPOBJ lpObj);
	BOOL IsMixPossibleItem(int iItemCode);
	BOOL CheckWingItem(int iItemCode);
	BOOL Check2ndWingItem(int iItemCode);
	BOOL Check3rdWingItem(int iItemCode);
	BOOL CheckLevelCondition(CItem * lpItem, short Level, BYTE Op1, BYTE Op2, BYTE Op3, BYTE SetOption, BYTE NewOption);
	BOOL FeatherOfCondorMix(LPOBJ lpObj);
	BOOL ThirdWingLevel2ChaosMix(LPOBJ lpObj);
	void ThirdWingMixFail(LPOBJ lpObj);
	void ThirdWingMixFailItemPanalty(CItem * lpItem);
	void DefaultChaosMix(LPOBJ lpObj);
	void DevilSquareItemChaosMix(LPOBJ lpObj);
	void BloodCastleItemChaosMix(LPOBJ lpObj);
	void SetItemChaosMix(LPOBJ lpObj);
	void DarkHorseChaosMix(LPOBJ lpObj);
	void DarkSpiritChaosMix(LPOBJ lpObj);
	void BlessPotionChaosMix(LPOBJ lpObj);
	void SoulPotionChaosMix(LPOBJ lpObj);
	void LifeStoneChaosMix(LPOBJ lpObj);
	void CastleSpecialItemMix(LPOBJ lpObj);
	void HiddenTreasureBoxItemMix(LPOBJ lpObj);
	void Fenrir_01Level_Mix(LPOBJ lpObj);
	void Fenrir_02Level_Mix(LPOBJ lpObj);
	void Fenrir_03Level_Mix(LPOBJ lpObj);
	void Fenrir_04Upgrade_Mix(LPOBJ lpObj);
	void ShieldPotionLv1_Mix(LPOBJ lpObj);
	void ShieldPotionLv2_Mix(LPOBJ lpObj);
	void ShieldPotionLv3_Mix(LPOBJ lpObj);
	void LotteryItemMix(LPOBJ lpObj, int type);
	BOOL SocketItemUpgradeMixS12(int aIndex, BYTE ItemPos, BYTE SocketUpgradeNotePos, BYTE JoBlessPos1, BYTE JoBlessPos2, BYTE JoBlessPos3, BYTE JoBlessPos4, BYTE JoBlessPos5, BYTE JoSoulPos1, BYTE JoSoulPos2, BYTE JoSoulPos3, BYTE JoSoulPos4, BYTE JoSoulPos5, BYTE JoChaosPos);
	void SeedExtractMix(LPOBJ lpObj);
	BOOL SeedExtractMixS12(int aIndex, BYTE AncientPos, BYTE ExePos, BYTE JOHPos, BYTE JOCPos, BYTE ChaosPos);
	void SeedSphereCompositeMix(LPOBJ lpObj);
	void SeedSphereCompositeMixS12(int aIndex, BYTE SeedPos, BYTE SpherePos, BYTE JocPos, BYTE ChaosPos);
	BOOL SeedSphereEnhance(int aIndex, BYTE Sphere1, BYTE Sphere2, BYTE Rune);
	void SetSeedSphereMix(LPOBJ lpObj, BYTE btPos);
	void SetSeedSphereMixS12(int aIndex, BYTE ItemPos, BYTE Slot, BYTE SeedPos, BYTE JocPos, BYTE ChaosPos);
	void SeedSphereRemoveMix(LPOBJ lpObj, BYTE btPos);
	void SeedSphereRemoveMixS12(int aIndex, BYTE ItemPos, BYTE Slot, BYTE JogPos, BYTE JohPos1, BYTE JohPos2, BYTE JohPos3, BYTE JohPos4, BYTE JohPos5, BYTE ChaosPos1, BYTE ChaosPos2, BYTE ChaosPos3, BYTE ChaosPos4, BYTE ChaosPos5);
	void SecromiconMix(LPOBJ lpObj);
	void PremiumBoxMix(LPOBJ lpObj);
	void CherryBlossomMix(LPOBJ lpObj);
	void ItemRefineMix(LPOBJ lpObj);
	void MonsterWingMix(LPOBJ lpObj);
	void SummonsMix(LPOBJ lpObj, int type);
	void CCFRewardMix(LPOBJ lpObj);
	void DSFRewardMix(LPOBJ lpObj);
	void AncientSoulItemMix(LPOBJ lpObj);
	void BloodAngelUnSealMix(LPOBJ lpObj, int iMixType);
	void BloodAngelUnSealMix2(LPOBJ lpObj, int iMixType);
	void DarkAngelMix(LPOBJ lpObj, int iMixType);
	void DarkAngelNextMix(LPOBJ lpObj, int iMixType);
	void DarkAngelWeaponMix(LPOBJ lpObj, int iMixType);
	void ArchangelHammerMix(LPOBJ lpObj);
	void BlessedAAMix(LPOBJ lpObj);
	BOOL AdvancedWingMix(LPOBJ lpObj);
	void BloodCastleMix(LPOBJ lpObj);
	BOOL CheckHeroSoulItem(int iItemCode);
	bool GetElementalTalismanOfLuckRate(LPOBJ lpObj, int* rate);
	bool GetTalismanOfLuckRate(LPOBJ lpObj, int* rate);
	// bloodangel mix
	BOOL bloodangelmix78(LPOBJ lpObj);
	BOOL bloodangelmix79(LPOBJ lpObj);
	BOOL bloodangelmix80(LPOBJ lpObj);
	BOOL bloodangelmix81(LPOBJ lpObj);

	BOOL bloodangelmix86(LPOBJ lpObj);
	BOOL bloodangelmix87(LPOBJ lpObj);
	BOOL bloodangelmix88(LPOBJ lpObj);
	BOOL bloodangelmix89(LPOBJ lpObj);

	BOOL bloodangelmix90(LPOBJ lpObj);
	BOOL bloodangelmix91(LPOBJ lpObj);
	BOOL bloodangelmix92(LPOBJ lpObj);
	BOOL bloodangelmix93(LPOBJ lpObj);

	BOOL bloodangelmix98(LPOBJ lpObj);
	BOOL bloodangelmix99(LPOBJ lpObj);
	BOOL bloodangelmix100(LPOBJ lpObj);
	BOOL bloodangelmix101(LPOBJ lpObj);

	BOOL bloodangelmix106(LPOBJ lpObj);
	BOOL bloodangelmix107(LPOBJ lpObj);
	BOOL bloodangelmix108(LPOBJ lpObj);
	BOOL bloodangelmix109(LPOBJ lpObj);

	BOOL bloodangelmix114(LPOBJ lpObj);
	BOOL bloodangelmix115(LPOBJ lpObj);
	BOOL bloodangelmix116(LPOBJ lpObj);
	BOOL bloodangelmix117(LPOBJ lpObj);

	BOOL bloodangelmix118(LPOBJ lpObj);
	BOOL bloodangelmix119(LPOBJ lpObj);
	BOOL bloodangelmix120(LPOBJ lpObj);
	BOOL bloodangelmix121(LPOBJ lpObj);

	BOOL bloodangelmix126(LPOBJ lpObj);
	BOOL bloodangelmix127(LPOBJ lpObj);
	BOOL bloodangelmix128(LPOBJ lpObj);
	BOOL bloodangelmix129(LPOBJ lpObj);

	void CheckEmptySpace_MultiMix(PMSG_REQ_CHAOS_MULTIMIX_CHECK * aRecv, int aIndex);

	void BlessPotionChaosMix_Multi(LPOBJ lpObj, int iMixCount);
	void SoulPotionChaosMix_Multi(LPOBJ lpObj, int iMixCount);
	void CircleChaosMix_Multi(LPOBJ lpObj, int iMixCount);
	void PegasiaChaosMix_Multi(LPOBJ lpObj, int iMixCount);
	void BloodCastleItemChaosMix_Multi(LPOBJ lpObj, int iMixCount);
	void DevilSquareItemChaosMix_Multi(LPOBJ lpObj, int iMixCount);
	void ShieldPotionLv1_Mix_Multi(LPOBJ lpObj, int iMixCount);
	void ShieldPotionLv2_Mix_Multi(LPOBJ lpObj, int iMixCount);
	void ShieldPotionLv3_Mix_Multi(LPOBJ lpObj, int iMixCount);
	void PremiumBoxMix_Multi(LPOBJ lpObj, int iMixCount);
	void JewelOfHarmonyItemPurityMix(LPOBJ lpObj);
	void PurityJewelOfHarmony_MultiMix(struct OBJECTSTRUCT * lpObj, int iMixCount);
private:
	int			CHAOS_GEM_ITEMINDEX;
	int			BLESS_GEM_ITEMINDEX;
	int			SOUL_GEM_ITEMINDEX;
	int			CREATURE_GEM_ITEMINDEX;
	int         GUARDIAN_GEM_ITEMINDEX;
	int			BLESS_GEM_BUNDLE_ITEMINDEX;
	int			SOUL_GEM_BULDLE_ITEMINDEX;
	int			CONDOR_FEATHER_ITEMINDEX;
	int			CONDOR_FLAME_ITEMINDEX;
	int			WING_OF_STORM_ITEMINDEX;
	int			WING_OF_ETERNAL_ITEMINDEX;
	int			WING_OF_ILLUSION_ITEMINDEX;
	int			WING_OF_RUIN_ITEMINDEX;
	int			CAPE_OF_EMPEROR_ITEMINDEX;
	int			WING_OF_DIMENSION_ITEMINDEX;
	int			CAPE_OF_OVERRULE_ITEMINDEX;
	int			CLOAK_OF_TRANSCEDENCE_ITEMINDEX;
	int			FRUIT_ITEMINDEX;
	int			DINORANT_ITEMINDEX;
	int			BLOODCASTLE_INVITATION_ITEMINDEX;
	int			DEVILSQUARE_INVITATION_ITEMINDEX;
	int			SMALL_SD_POTION_ITEMINDEX;
	int			MEDIUM_SD_POTION_ITEMINDEX;
	int			LARGE_SD_POTION_ITEMINDEX;
	int			BLESS_POTION_ITEMINDEX;
	int			SOUL_POTION_ITEMINDEX;
	int			GOLDEN_BOX_ITEMINDEX;
	int			SILVER_BOX_ITEMINDEX;
	int			HIGH_REFINE_STONE_ITEMINDEX;
	int			LOW_REFINE_STONE_ITEMINDEX;
	int			HARMONY_JEWEL_ITEMINDEX;
	int			m_i3rdWingOpt_LuckRate;
};

extern CMixSystem g_MixSystem;
#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

