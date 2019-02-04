////////////////////////////////////////////////////////////////////////////////
// configread.h
#ifndef CONFIGREAD_H
#define CONFIGREAD_H

#include "classdef.h"
#include "TLog.h"
#include "MuLua.h"
#include "MapClass.h"

#define MAX_VIP 4

enum DISABLE_MSB_DAMAGE_EFFECT
{
	MSB_DISABLE_MONSTER = 0,
	MSB_DISABLE_CHARACTER = 1,
	MSB_DISABLE_CHARACTER_FENRIR = 2
};

enum SERVER_TYPE
{
	SERVER_NORMAL = 0,
	SERVER_CASTLE = 1,
	SERVER_ARCA = 2,
	SERVER_BATTLECORE = 3
};

enum MU_EVENT_TABLE
{
	EV_TABLE_BC = 0,
	EV_TABLE_DS = 1,
	EV_TABLE_CC = 2,
	EV_TABLE_IT = 3,
	EV_TABLE_CS = 4,
	EV_TABLE_DEEP = 5,
	EV_TABLE_CW = 6,
	EV_TABLE_KANTURU = 7,
	EV_TABLE_RAKLION = 8,
	EV_TABLE_DG = 9,
	EV_TABLE_IF = 10,
	EV_TABLE_RING = 11,
	EV_TABLE_XMAS = 12,
	EV_TABLE_ARCA = 13,
	EV_TABLE_ACHERON = 14,
	EV_TABLE_CCF = 15,
	EV_TABLE_DSF = 16,
	EV_TABLE_LMS = 17
};

struct SERVER_CONFIG
{
public:

	void SetEventTable();
	void ReadServerInfo(bool bObjInit);
	bool GetStateFromEventTable(int iServerType, int iEventType) { return this->bEventTable[iServerType][iEventType]; }

	char * GetServerName() { return this->szServerName; }

	int GetGameServerCode() { return this->iGameServerCode; }
	bool IsTestServer() { return this->bIsTestServer; }
	bool IsConnectMemberUse() { return this->bIsConnectMemberUse; }
	int GetServerType() { return this->iServerType; }

	int GetObjectMax() { return this->iObjectMax; }
	int GetObjectMaxMonster() { return this->iObjectMaxMonster; }
	int GetObjectMaxUser() { return this->iObjectMaxUser; }
	int GetObjectMaxItem() { return this->iObjectMaxItem; }
	int GetObjectMaxCallMonster() { return this->iObjectMaxCallMonster; }
	int GetObjectStartUserIndex() { return this->iObjectStartUserIndex; }

	char * GetJoinServerIP() { return this->szJoinServerIP; }
	char * GetDataServerIP() { return this->szDataServerIP; }
	char * GetExDBIP() { return this->szExDBIP; }
	char * GetConnectServerIP() { return this->szConnectServerIP; }

	int GetJoinServerPort() { return this->iJoinServerPort; }
	int GetDataServerPort() { return this->iDataServerPort; }
	int GetExDBPort() { return this->iExDBPort; }
	int GetConnectServerPort() { return this->iConnectServerPort; }
	int GetGameServerPort() { return this->iGameServerPort; }

	bool IsLogCreate() { return this->bLogCreate; }
	int GetLogMaxFileSize() { return this->iLogFileMaxSizeMB; }

	int GetHWIDUseLimit() { return this->iHWIDUseLimit; }

	char * GetCashShopLogPath() { return this->szCashShopLogPath; }
	char * GetUserSaveLogPath() { return this->szUserSaveLogPath; }
	char * GetCommandLogPath() { return this->szCommandLogPath; }
	char * GetServerLogPath() { return this->szServerLogPath; }
	char * GetGMLogPath() { return this->szGMLogPath; }
	char * GetTradeLogPath() { return this->szTradeLogPath; }
	char * GetMsgLogPath() { return this->szMsgLogPath; }
	char * GetAntiHackLogPath() { return this->szAntiHackLogPath; }
	char * GetBotShopLogPath() { return this->szBotShopLogPath; }
	char * GetSerialCheckLogPath() { return this->szSerialCheckLogPath; }

private:
	char szServerName[50];
	int iGameServerCode;
	bool bIsTestServer;
	bool bIsConnectMemberUse;
	int iServerType;

	int iObjectMax;
	int iObjectMaxMonster;
	int iObjectMaxUser;
	int iObjectMaxItem;
	int iObjectMaxCallMonster;
	int iObjectStartUserIndex;

	char szJoinServerIP[20];
	char szDataServerIP[20];
	char szExDBIP[20];
	char szConnectServerIP[20];

	int iJoinServerPort;
	int iDataServerPort;
	int iExDBPort;
	int iConnectServerPort;
	int iGameServerPort;

	bool bLogCreate;
	int iLogFileMaxSizeMB;

	int iHWIDUseLimit;

	char szCashShopLogPath[100];
	char szUserSaveLogPath[100];
	char szCommandLogPath[100];
	char szServerLogPath[100];
	char szGMLogPath[100];
	char szTradeLogPath[100];
	char szMsgLogPath[100];
	char szAntiHackLogPath[100];
	char szBotShopLogPath[100];
	char szSerialCheckLogPath[100];

	bool bEventTable[5][18];
};

struct COMMON_CONFIG
{
	bool IsGuardSay;
	std::string GuardSayText;
	short GuardTalkChance;
	std::string WelcomeMessage;
	BYTE EnterGameMessage;
	bool ThirdQuestCounter;
	bool PKEnterEventAllow;
	BYTE AllianceMinGuildMember;
	BYTE AllianceMaxGuilds;
	BYTE PersonalShopMoneyType;
	bool SavePrivateChat;
	BYTE MaxGuildMember;
	bool BlockTradeFFFFFFFFItems;
	bool AncHarmonyLimit;	
	bool TradeBlock;
	bool EnableSellAllItems;
	int TradeExcItemLimit;
	int StoreExcItemLimit;
	int ShopExcItemLimit;
	int TradeHarmonyBlock;
	int Item380DropMap;
	int TempUserManagerClearDataTime;
	bool AllowGetLuckyItemGround;
	int LuckyItemDurabilityTime;
	int LuckyItemAllowUseJewels;
	int SpiritMapDropRate;
	int SpiritMapDropLevel;
	int GoldenDragonBoxCount;
	int GreatGoldenDragonBoxCount;
	bool AutoPartySet;
	bool bReconnectSystem;

	SYSTEMTIME EventEndDate;
	bool EventInventoryOpen;

	float  gAddZen;
	float  gAddZenML;
	float gAddZenDiv;

	WORD  MLMonsterMinLevel;
	WORD  UserMaxLevel; 

	int PetEventBookMoonRabbitSpawnRate;
	int PetEventBookLuckyBagSpawnRate;
	int PetEventBookFireFlameSpawnRate;
	int PetEventBookGoldGoblinSpawnRate;

	bool GuildCreate;
	bool GuildDestroy;
	int GuildCreateLevel;
	bool CastleGuildDestroyLimit;
	bool FreeWarehouse;
	bool DisableMSBEffect[3];

	bool CancelItemSaleEnable;
	float CancelItemSaleMultipler;
	int CancelItemSaleExpiryTime;

	int XMasSantaMinReset;
	int XMasSantaFirstPrizeMaxCount;
	int XMasSantaSecondPrizeMaxCount;

	bool CheckValidItem;

	bool BossKillMessage;

	bool bHitHackDetection;
	int  iHitHackMaxAgility;

	bool joinmuIsDecreaseAllow;
	bool joinmuFruitRequireUnWearItem;
	bool joinmuDropItemUnderCharacter[MAX_TYPE_PLAYER];
	bool joinmuDurabilityDownDisable;

	bool IsJoinMu;
	bool joinmuDisableWarpLevelBenefits;
	bool joinmuSellPriceZero;
	bool joinmuRemoveItemSellDropRestriction;
	bool joinmuMultiChannelWhisper;
	bool Is28Opt;
};

struct Reset
{
	bool enable;
	unsigned short iMinLevel;
	int iCost;
	int iResetLimit;
	bool ResetML;
	bool ResetStats;
	int NormalPointPerReset;
	int SaveOldPoint;
	bool RemoveEquipment;
	bool MoveToCharacterSelect;
	int iBlockLevelUpPointAfterResets;
	int iBlockMLPointAfterResets;
};

struct POST
{
	bool bPostEnable;
	bool bGPostEnable;
	int iPostPrice;
	int iGPostPrice;
	WORD wPostLevel;
	WORD wGPostLevel;
	BYTE btPostColorRGB[3];
	BYTE btGPostColorRGB[3];
	DWORD dwPostCooldown;
	DWORD dwGPostCooldown;
};

struct CHAT_COLORS
{
	BYTE btInfoMsg[3];
	BYTE btErrorMsg[3];
	BYTE btChatMsg[3];
	BYTE btWhisperMsg[3];
	BYTE btPartyMsg[3];
	BYTE btGuildMsg[3];
	BYTE btGensMsg[3];
	BYTE btAllianceMsg[3];
	BYTE btGMChatMsg[3];
};

struct ANTIHACK_CONFIG
{
	bool EnabledAgiCheck;
	BYTE EnablePacketTimeCheck;
	DWORD PacketTimeMinTime;
	BYTE EnableHackDetectMessage;
	std::string HackDetectMessage;
	bool isInvalidDLLVersionDC;
	DWORD AgilityDelayCheckTime;
	bool EnabledSkillCheckTime;
	bool EnabledSkillCheckDC;
	DWORD SkillCheckWrongCastsToDC;
	bool EnabledSkillCheckDebugMode;
	bool AntiHackBreachDisconnectUser;
	bool AntiHackRecvHookProtect;
};

struct MUBOT_CONFIG
{
	bool enable;
	unsigned short minlevel;
	unsigned short minreset;
	int cost;
	char viplevel;
	int autodisabletime;
};

struct CONFIG_STRUCT
{
	COMMON_CONFIG common;
	Reset reset;
	POST post;
	MUBOT_CONFIG mubot;
	CHAT_COLORS chatcolor;
};

struct CHAOS_MACHINE
{
	// nuevo
	BYTE PlusItemLevel10Rate;
	BYTE PlusItemLevel11Rate;
	BYTE PlusItemLevel12Rate;
	BYTE PlusItemLevel13Rate;
	BYTE PlusItemLevel14Rate;
	BYTE PlusItemLevel15Rate;
	BYTE PlusItemMixResultNotice;
	BYTE PlusItemAddLuckRate;
	//==============

	BYTE DarkHorseMix;
	BYTE DarkRavenMix;

	BYTE BlessPotionMix;
	BYTE SoulPotionMix;
	BYTE LifeStoneMix;
	BYTE CastleLordMix;

	BYTE Fenrir1stLevel;
	BYTE Fenrir2ndLevel;
	BYTE Fenrir3rdLevel;

	BYTE DSDefault;
	BYTE DS1;
	BYTE DS2;
	BYTE DS3;
	BYTE DS4;
	BYTE DS5;
	BYTE DS6;
	BYTE DS7;

	BYTE DinorantMix;

	BYTE Wing1MaxRate;
	
	BYTE Wing2MaxRate;
	int Wing2MixRateLuck;

	BYTE Wing25MaxRate;
	BYTE CapeLordMaxRate;
	BYTE ITMix;

	int Wing3MixRateLuck;
	BYTE Wing3MaxRate;
	BYTE FeatherCondorMaxRate;

	BYTE SocketWeaponMaxRate;
	int SocketWeaponRequireMoney;

	BYTE CherryBlossomWhiteNeedCount;
	BYTE CherryBlossomRedNeedCount;
	BYTE CherryBlossomGoldNeedCount;
};

struct PENALTY
{
	BYTE	ExpDecRate;
	BYTE	DropDecRate;
};

enum PET_DAMAGE_TYPES
{
	DAMAGE_OFF_ANGEL = 0,
	DAMAGE_OFF_SATAN = 1,
	DAMAGE_OFF_UNIRIA = 2,
	DAMAGE_OFF_DINORANT = 3,
	DAMAGE_OFF_DLHORSE = 4,
	DAMAGE_OFF_RAVEN = 5,
	DAMAGE_OFF_FENRIR = 6,
	DAMAGE_OFF_DEMON = 7,
	DAMAGE_OFF_SPIRIT_ANGEL = 8,
	DAMAGE_OFF_RUDOLF = 9,
	DAMAGE_OFF_PANDA = 10,
	DAMAGE_OFF_UNICORN = 11,
	DAMAGE_OFF_SKELETON = 12,
	DAMAGE_OFF_MUUN = 13
};
	
struct PETCONFIG
{
	WORD GoldFenrirDefenseRise;
	WORD GoldFenrirAttackRise;
	WORD GoldFenrirDurabilityRate;

	WORD RedFenrirAddExp;
	WORD BlueFenrirAddExp;
	WORD BlackFenrirAddExp;
	WORD GoldFenrirAddExp;

	WORD WizardRingAddDamage;
	WORD WizardRingAddMagicDamage;
	WORD WizardRingAddAttackSpeed;

	WORD LethalRingAddDamage;
	WORD LethalRingAddMagicDamage;
	WORD LethalRingAddAttackSpeed;

	WORD UnicornAddDefense;

	WORD EliteSkeletonPetAddDamage;
	WORD EliteSkeletonPetAddAttackSpeed;

	WORD EliteSkeletonRingAddDefense;

	WORD ChristmasRingAddDamage;
	WORD ChristmasRingAddMagicDamage;

	WORD PandaRingAddDamage;
	WORD BrownPandaRingAddDamage;
	WORD PinkPandaRingAddDamage;
	
	WORD SkeletonRingAddDamage;

	WORD RobotKnightRingAddDefense;
	WORD RobotKnightRingAddDamage;

	WORD MiniRobotRingAddAttackSpeed;
	WORD MiniRobotRingAddDamage;

	WORD MageRingAddDamage;

	WORD DecorationRingAddDamage;
	WORD DecorationRingAddAttackSpeed;

	WORD BlessedDecorationRingAddDamage;
	WORD BlessedDecorationRingAddAttackSpeed;

	WORD DSFRingAddDamage;
	WORD DSFRingAddAttackSpeed;
	WORD DSFRingAddDefense;
	WORD DSFRingAddHP;

	WORD AngelAddHP;
	WORD AngelAddDefense;

	WORD SatanAddDamage;

	WORD DemonAddDamage;
	WORD DemonAddAttackSpeed;

	WORD SafeGuardAddHP;
	WORD SafeGuardAddDefense;

	WORD PandaPetAddDefense;

	bool DamageDisableForPet[14];
};

struct OFFTRADE
{
	bool enabled;
	BYTE Maps[MAX_NUMBER_MAP];
	BYTE CoinType;
};

struct CALCHAR
{
	WORD PvPDamageRate[MAX_TYPE_PLAYER][MAX_TYPE_PLAYER];
	WORD ElementalPvPDamageRate[MAX_TYPE_PLAYER][MAX_TYPE_PLAYER];
	WORD PvMDamageRate[MAX_TYPE_PLAYER];
	WORD ElementalPvMDamageRate[MAX_TYPE_PLAYER];
	WORD PvPDamageRate_DarkSpirit[MAX_TYPE_PLAYER][MAX_TYPE_PLAYER];
	WORD PvMDamageRate_DarkSpirit;
	WORD MaximumDamageDec;
	bool ElementalDamageAffectSDOnly;
	float DarkHorseDamageMultiplier;
	float DarkRavenDamageMultiplier;
	WORD ReflectDamage;
};

struct ITEMCONFIG
{

};

struct ITEM_PRICES
{
	BYTE btItemSellPriceDivisor;
	int iJewelOfBlessPrice;
	int	iJewelOfSoulPrice;
	int iJewelOfChaosPrice;
	int iJewelOfLifePrice;
	int iJewelOfCreationPrice;
	int iCrestOfMonarchPrice;
	int iLochFeatherPrice;
	int iJewelOfGuardianPrice;
	int iWereRabbitEggPrice;
	int iAngelPrice;
	int iImpPrice;
	int iUniriaPrice;
	int iDinorantPrice;
	int iInvisiblityCloakPrice;
	int iMoonstonePendantPrice;
	int iArmorOfGuardianPrice;
	int iDevilSquareInvitationPrice;
	int iKalimaTicketPrice;
	int iIllusionTempleTicketPrice;
};

struct PK_EXP_DECREASE
{
	int iLevelMin;
	int iLevelMax;
	int iDecRate;
};

struct PK_LEVEL_CONFIG
{
	BYTE btPKLevel;
	int iPKReqPoint;
	int iItemDropRate_Monster;
	int iItemDropRate_Character;
	int iZenLossRate_Monster;
	int iZenLossRate_Character;
	int iWarpCostMultiplier;
	bool bCanSummonByDarkLord;
	std::vector<PK_EXP_DECREASE> m_vtExpDec;
};

struct PK_CONFIG
{
	int iPKPointDeductionDivisor;
	int iPKPointIncForMurderer;
	bool bPkLevelIncreaseOff;
	bool bPkPenaltyDisable;
	bool bPkClearEnable;
	bool bPkCanUseShops;
	bool bPkDropExpensiveItems;
	int iPkMaxLevelItemDrop;
	int iPkClearCost;
	bool bIsCostMultiplyByKillCount;
	PK_LEVEL_CONFIG m_LevelConfig[7];

	int GetPKReqPoint(int iPKLevel)
	{
		if (iPKLevel < 0 || iPKLevel >= 7)
		{
			g_Log.AddC(TColor::Red, "Error %s - wrong PK Level (%d)", __FUNCTION__, iPKLevel);
			return 0;
		}

		return this->m_LevelConfig[iPKLevel].iPKReqPoint;
	}

	int GetItemDropRate(int iPKLevel, bool bIsPvP)
	{
		if (iPKLevel < 0 || iPKLevel >= 7)
		{
			g_Log.AddC(TColor::Red, "Error %s - wrong PK Level (%d)", __FUNCTION__, iPKLevel);
			return 0;
		}

		if (bIsPvP == true)
		{
			return this->m_LevelConfig[iPKLevel].iItemDropRate_Character;
		}

		else
		{
			return this->m_LevelConfig[iPKLevel].iItemDropRate_Monster;
		}
	}

	int GetZenLossRate(int iPKLevel, bool bIsPvP)
	{
		if (iPKLevel < 0 || iPKLevel >= 7)
		{
			g_Log.AddC(TColor::Red, "Error %s - wrong PK Level (%d)", __FUNCTION__, iPKLevel);
			return 0;
		}

		if (bIsPvP == true)
		{
			return this->m_LevelConfig[iPKLevel].iZenLossRate_Character;
		}

		else
		{
			return this->m_LevelConfig[iPKLevel].iZenLossRate_Monster;
		}
	}

	int GetWarpCostMultiplier(int iPKLevel)
	{
		if (iPKLevel < 0 || iPKLevel >= 7)
		{
			g_Log.AddC(TColor::Red, "Error %s - wrong PK Level (%d)", __FUNCTION__, iPKLevel);
			return 0;
		}

		return this->m_LevelConfig[iPKLevel].iWarpCostMultiplier;
	}

	bool CanSummonByDarkLord(int iPKLevel)
	{
		if (iPKLevel < 0 || iPKLevel >= 7)
		{
			g_Log.AddC(TColor::Red, "Error %s - wrong PK Level (%d)", __FUNCTION__, iPKLevel);
			return true;
		}

		return this->m_LevelConfig[iPKLevel].bCanSummonByDarkLord;
	}

	int GetExpDecreaseRate(int iPKLevel, int iLevel)
	{
		if (iPKLevel < 0 || iPKLevel >= 7)
		{
			g_Log.AddC(TColor::Red, "Error %s - wrong PK Level (%d)", __FUNCTION__, iPKLevel);
			return 0;
		}

		for (std::vector<PK_EXP_DECREASE>::iterator It = this->m_LevelConfig[iPKLevel].m_vtExpDec.begin(); It != this->m_LevelConfig[iPKLevel].m_vtExpDec.end(); It++)
		{
			if (iLevel >= It->iLevelMin && iLevel <= It->iLevelMax)
			{
				return It->iDecRate;
			}
		}

		return 0;
	}
};

class CConfigread
{
public:

	CConfigread();
	virtual ~CConfigread();

	void ReadServerInfo(bool bObjInit)
	{
		this->server.ReadServerInfo(bObjInit);
	}

	void SetEventTable() { this->server.SetEventTable(); }

	void ReadConfig();
	void ReadOffTradeConfig();
	void ReadPetSettings();
	void ReadPKSettings();
	void ReadCustomSettings();
	void ReadMuHelperSettings();
	void ReadShieldGageSettings();
	void LoadAntiHackConfig();
	void ReadSocketMixSettings();
	void ReadSkillStunsSettings();

	void ReadCalcCharacter();
	LPSTR GetPath(LPSTR path);

	SERVER_CONFIG server;
	CONFIG_STRUCT data;
	ANTIHACK_CONFIG antihack;
	CHAOS_MACHINE mix;
	OFFTRADE offtrade;
	PETCONFIG pet;
	CALCHAR calc;
	ITEM_PRICES itemPrices;
	PK_CONFIG pk;

	bool OffLevel;//

	// Rate scroll / bone   bloodcastle item
	int GetBloodBoneDropRate;
	int GetArchangelScrollDropRate;

	int  BloodCastleRateCloack[7];
	int  BloodCastleMoneyCloack[7];

	// Pvp Balance Debug
	bool PvpDebug;

	int NixieLakeEntrance;

	int MixHarmonyVip;
	int MixHarmonyFree;

	int StunTimeDeathStabMastery;
	int StunTimeChaoticDiseier;
	int StunTimeClash275;
	int StunTimeEarthshakeMastery;
	int StunTimeFireBurstMastery;
	int StunTimeDragonSlasherMastery;
	int StunTimeMeteorMastery;
	int StunTimeMultiShotMastery;
	int StunTimeBlindStrengthener;
	int StunTimeEarthTomeMastery;
	int StunTimeMaceMastery;
	int StunTimeStunSkill67;
	int StunTimeIceMastery;

	int EnableStunEffect;
	int EnableFreezeEffect;
	int EnableFreezeEffectTime;
	/* RESET CONFIG */
	short ResetSwitch;
	short ResetSwitchReward;
	short ResetType;
	short ResetPlusGrandPoints;
	short ResetMoneyType;

	short ResetCheckItem;
	short ResetMove;
	short ResetLevel;
	int   ResetMoney;
	int   ResetPoint;
	short ResetLeadership;
	short ResetMultiply;
	int   ResetLimit;
	short ResetStartLevel;
	short ResetPointRate[8];
	char  ResetCommand[32];
	char  ResetMsg[12][64];

	short ResetDynamicEnable;
	float ResetDynamicXFactor;

	short ResetBonusWCoinC;
	short ResetBonusGoblinPoint;

	char WriteResetLog;
	short StartingMap[8];
	//=====================================
	// SHIELD GAGE SYSTEM
	BOOL g_bShieldComboMissOptionOn;
	int g_CompoundPotionDropOn;
	int g_iCompoundPotionLv1DropRate;
	int g_iCompoundPotionLv2DropRate;
	int g_iCompoundPotionLv3DropRate;
	int g_iCompoundPotionLv1DropLevel;
	int g_iCompoundPotionLv2DropLevel;
	int g_iCompoundPotionLv3DropLevel;
	int g_ShieldAutoRefillOn;
	int g_ShieldAutoRefillOnSafeZone;
	int g_iShieldPotionLv1MixSuccessRate;
	int g_iShieldPotionLv1MixMoney;
	int g_iShieldPotionLv2MixSuccessRate;
	int g_iShieldPotionLv2MixMoney;
	int g_iShieldPotionLv3MixSuccessRate;
	int g_iShieldPotionLv3MixMoney;
	int g_iShieldGageConstA;
	int g_iShieldGageConstB;
	int g_ShieldSystemOn;
	int g_iDamageDevideToSDRate;
	float g_fSuccessAttackRateOption;
    // no estan def
	int g_iDamageDevideToHPRate;
	int g_iSDChargingOption;
	int g_iConstNumberOfShieldPoint;
	//========================================
    // summon scroll mix
	int g_iSummonScrollMix1;
	int g_iSummonScrollMix1Zen;

	int g_iSummonScrollMix2;
	int g_iSummonScrollMix2Zen;

	int g_iSummonScrollMix3;
	int g_iSummonScrollMix3Zen;

	/* ANTIHACK CONFIG */
	char ItemSerialCheck;

	char SkillDistanceCheck;
	short SkillDistanceTolerance;
	char SkillDistanceDC;
	short SkillDistanceCountDC;
	short SkillDistanceTimeDC;

	char MovingSpeedCheck;
	char MovingSpeedLog;
	char MovingSpeedDC;
	short MovingSpeedDistance;
	short MovingSpeedTime;

	char MovingHackCheck;
	short MovingHackDistance;

	short m_CheckSpeedHack;
	short m_CheckSpeedHackTolerance;
	short m_CheckSpeedHackLimit;

	bool EnabledAntiRefTimeCheck;
	DWORD AntiRefCheckTime;
	BYTE EnableAutoBanUserAccount;
	BYTE EnableAttackBlockInSafeZone;

	DWORD MainCRC;
	DWORD DLLCRC;
	DWORD PlayerCRC;
	DWORD SkillCRC;
	
	int PacketLimit;
	/*------------------------------*/	
	bool UseCalcAttackSpeed;
	bool UseDamageCalc;
	bool UseMagicDamageCalc;
	bool UseCalcDefenseSuccessRate_PvM;
	bool UseCalcDefenseSuccessRate_PvP;
	bool UseCalcDefense;

	int RandomBuyRuudLuck;
	int SeedSphereEnhance;
	int SeedSphereRemove;
	int IsSocketItemUpgradeMix;
	int IsSocketItemUpgradeMixZen;

	//--------------
	// IronDefense
	float m_Arg1DK;
	float m_Arg2DK;
	float m_Arg3DK;
	float m_Arg1DL;
	float m_Arg2DL;
	float m_Arg3DL;
	// ============
	int m_HelperSwitch;
	int m_HelperActiveLevel;
	int m_HelperActiveDelay;
	int m_HelperActiveMoney[5];

	// Options Min/Max in ItemBags
	int MinOptionCount;
	int MaxOptionCount;

	int UseAllBuffAttack;

	int UseMLforWarp;

	char FTPAddress[200];
	char HashCode[200];

	char IGCData[20];

	char CurrPath[260];

	MULua m_ItemCalcLua;

}; extern CConfigread g_ConfigRead;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

