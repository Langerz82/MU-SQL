// ------------------------------
// Decompiled by Deathway
// Date : 2007-03-09
// ------------------------------
//------------------------------------------
// GameMain.cpp
//------------------------------------------
// GS-N 0.99.60T 0x004F4490
//	GS-N	1.00.18	JPN	0x0051FE00	-	Completed
#include "StdAfx.h"
#include "Lang.h"
#include "gameGlobals.h"
#include "util.h"
#include "GameServer.h"
#include "DirPath.h"
#include "Logging/Log.h"
#include "wsJoinServerCli.h"
#include "DoppelGanger.h"

#include "CLoginCount.h"
#include "MapClass.h"
#include "MonsterRegenSystem.h"
#include "WzUdp.h"
#include "classdef.h"
#include "prodef.h"
#include "wsGameServer.h"
#include "EledoradoEvent.h"
#include "ObjUseSkill.h"
#include "WhisperCash.h"
#include "MasterLevelSkillTreeSystem.h"
#include "SProtocol.h"
#include "ChaosCastle.h"
#include "DevilSquare.h"
#include "Gate.h"
#include "User/CUserData.h"
#include "PacketCheckSum.h"
#include "DragonEvent.h"
#include "Event/AttackEvent/AttackEvent.h"
#include "MapServerManager.h"
#include "QuestInfo.h"
#include "SkillHitBox.h"
#include "CItemObjectDrop.h"
#include "EventManagement.h"
#include "SetItemOption.h"
#include "ItemSocketOptionSystem.h"
//#include "EDSProtocol.h"
#include "MonsterItemMng.h"
#include "ConMember.h"
#include "GMMng.h"
#include "BuffScriptLoader.h"
#include "BattleSoccerManager.h"
#include "GuildClass.h"
#include "MoveCommand.h"
#include "Event/BloodCastle/BloodCastle.h"
#include "Shop.h"
#include "ItemAddOption.h"
#include "Kanturu.h"
#include "KanturuMonsterMng.h"
#include "SkillAdditionInfo.h"
#include "TMonsterSkillElement.h"
#include "TMonsterSkillUnit.h"
#include "TMonsterSkillManager.h"
#include "TMonsterAIElement.h"
#include "TMonsterAIAutomata.h"
#include "TMonsterAIUnit.h"
#include "TMonsterAIRule.h"
#include "TMonsterAIGroup.h"
//Added by HermeX
#include "Crywolf.h"
#include "CrywolfAltar.h"
#include "CashShop.h"
#include "CashLotterySystem.h"
#include "ImperialGuardian.h"
#include "MonsterStatCalc.h"
#include "Raklion.h"
#include "RaklionMonsterMng.h"
#include "Event/BonusEvent/BonusEvent.h"
#include "AntiSwear.h"
#include "ObjCalCharacter.h"
#include "GamblingSystem.h"
#include "configread.h"
#include "VipSys.h"
#include "NewsSystem.h"
#include "Authorization.h"
#include "SetItemDrop.h"
#include "PentagramSystem.h"
#include "PentagramMixSystem.h"
#include "LogToFile.h"
#include "MapAttribute.h"
#include "NewPVP.h"
#include "MonsterKillCount.h"
#include "PeriodItemEx.h"
#include "QuestExpInfo.h"
#include "QuestExpLuaBind.h"
#include "LuaFun.h"
#include "ExpManager.h"
#include "BagManager.h"
#include "LuckyItemManager.h"
#include "SkillUseTime.h"
#include "ProhibitedSymbols.h"
#include "TUnion.h"
#include "MuunSystem.h";
#include "ItemOptionTypeMng.h"
#include "AppointItemDrop.h"
#include "SendNPCInfo.h"
#include "ChaosCastleFinal.h"
#include "MineSystem.h"
#include "ObjAttack.h"
#include "IllusionTempleEvent_Renewal.h"
#include "Event/AcheronGuardianEvent/AcheronGuardianEvent.h"
#include "StatSpecialize.h"
#include "SkillSpecialize.h"
#include "OfflineLevelling.h"
#include "DevilSquareFinal.h"
#include "BotSystem.h"
#include "GensSystem.h"
#include "Marry.h"
#include "EventMonSetBase.h"
#include "SkillSafeZoneUse.h"
#include "FormulaParse.h"
#include "EvolutionMonsterMng.h"
#include "RuudStore.h"
#include "PvPBalance.h"

//+-----------------------------------------
//	Castle Siege
//+-----------------------------------------
#include "CastleDeepEvent.h"
#include "LastManStanding.h"
//------------------------------------------
// GameMain.cpp Variables
//------------------------------------------
#include "ItemDropEx.h"
#include "Notice.h"
#include "./Custom/CustomEventDrop/CustomEventDrop.h"
#include "UserDieItemDrop.h"
#include "FloodSystem.h"
#include "Item_Bags.h"
#include "./Custom/CustomItemValue/CustomItemValue.h"
#include "ResetTable.h"
#include "ItemOptionRate.h"
#include "SocketItemType.h"
#include "ItemOption.h"
#include "CustomMaxStats.h"
#include "CustomMaxStats.h"
#include "PVPZone.h"
#include "ItemUpgradeJewel.h"
#include "CustomMichi.h"

BOOL JoinServerConnected;
BOOL DataServerConnected;
BOOL GameServerCreated;
BOOL IsEventChipServerConnected;
CDragonEvent * DragonEvent;
CAttackEvent * AttackEvent;
WzUdp gUdpSoc;	// line 256
int	 gCheckSumSwitch;
int  gSpeedHackPenalty;
BOOL gEnableEventNPCTalk;
BOOL gEnableServerDivision;
BOOL gEvent1;
int  gMonsterHp;
BOOL gMerryXMasNpcEvent;
BOOL gHappyNewYearNpcEvent;
int  gEvent1ItemDropTodayCount;
char gStalkProtocolId[11];
BOOL gXMasEvent;
BOOL gFireCrackerEvent;
BOOL gHeartOfLoveEvent;
BOOL gMedalEvent;
BOOL gEventChipEvent;
BOOL gWriteSkillLog;
BOOL g_bStoneItemDrop;
int  g_iKundunMarkDropRate;
int  g_iMarkOfTheLord;
int	 g_iLuckyCoin;
int  g_iDarkLordHeartDropRate;
int  g_iDarkLordHeartOffEventRate;
int g_iMysteriousBeadDropRate1;
int g_iMysteriousBeadDropRate2;
int g_iHiddenTreasureBoxOfflineRate;
BOOL bIsIgnorePacketSpeedHackDetect;
BOOL gIsKickDetecHackCountLimit;
BOOL gIsItemDropRingOfTransform;
BOOL gDoPShopOpen;
BOOL gWriteChatLog;
BOOL gDisconnectHackUser;
BOOL GSInfoSendFlag = FALSE;

///////////////////////////////////////////////////////////////////////////////

DWORD JoinServerDCTime = 0;

CwsGameServer wsGServer;	// line : 213GameServer
wsJoinServerCli wsJServerCli;	// line : 214 Join Server
wsJoinServerCli wsDataCli;	// line : 215 DataServer
wsJoinServerCli wsExDbCli;	// line : 239 Extra DataBase Server
CDirPath gDirPath;	// line : 248 Directory Path
MapClass MapC[MAX_NUMBER_MAP];	// line 249	// Map Manager
CMonsterAttr gMAttr;	// line 250
CMonsterSetBase gMSetBase;	// line 251
classdef DCInfo;	// line 252:
CWhisperCash WhisperCash;	// line 253
PartyClass gParty;	// line 254

CGuildClass Guild;	// line 265

CLoginCount gLCount[3];	// line 329

UINT64 * gLevelExperience;
char szGameServerExeSerial[24];

BOOL gIsDropDarkLordItem;
int  gSleeveOfLordDropRate;
int  gSleeveOfLordDropLevel;

int  gSoulOfDarkHorseDropRate;
int  gSoulOfDarkHorseropLevel;
int  gSoulOfDarkSpiritDropRate;
int  gSoulOfDarkSpiritDropLevel;
float gDarkSpiritAddExperience;
BOOL gIsDropGemOfDefend;
int  gGemOfDefendDropRate;
int  gGemOfDefendDropLevel;
int  g_iUseCharacterAutoRecuperationSystem;
int  g_iCharacterRecuperationMaxLevel;
int g_iServerGroupGuildChatting;
int g_iServerGroupUnionChatting;
BOOL g_bRibbonBoxEvent;
int g_iRedRibbonBoxDropLevelMin;
int g_iRedRibbonBoxDropLevelMax;
int g_iRedRibbonBoxDropRate;
int g_iGreenRibbonBoxDropLevelMin;
int g_iGreenRibbonBoxDropLevelMax;
int g_iGreenRibbonBoxDropRate;
int g_iBlueRibbonBoxDropLevelMin;
int g_iBlueRibbonBoxDropLevelMax;
int g_iBlueRibbonBoxDropRate;
BOOL g_bChocolateBoxEvent;
int g_iPinkChocolateBoxDropLevelMin;
int g_iPinkChocolateBoxDropLevelMax;
int g_iPinkChocolateBoxDropRate;
int g_iRedChocolateBoxDropLevelMin;
int g_iRedChocolateBoxDropLevelMax;
int g_iRedChocolateBoxDropRate;
int g_iBlueChocolateBoxDropLevelMin;
int g_iBlueChocolateBoxDropLevelMax;
int g_iBlueChocolateBoxDropRate;
BOOL g_bCandyBoxEvent;
int g_iLightPurpleCandyBoxDropLevelMin;
int g_iLightPurpleCandyBoxDropLevelMax;
int g_iLightPurpleCandyBoxDropRate;
int g_iVermilionCandyBoxDropLevelMin;
int g_iVermilionCandyBoxDropLevelMax;
int g_iVermilionCandyBoxDropRate;
int g_iDeepBlueCandyBoxDropLevelMin;
int g_iDeepBlueCandyBoxDropLevelMax;
int g_iDeepBlueCandyBoxDropRate;
BOOL g_bFenrirStuffItemDrop;
int g_iFenrirStuff_01_DropLv_Min;
int g_iFenrirStuff_01_DropLv_Max;
int g_iFenrirStuff_01_DropMap;
int g_iFenrirStuff_01_DropRate;
int g_iFenrirStuff_02_DropLv_Min;
int g_iFenrirStuff_02_DropLv_Max;
int g_iFenrirStuff_02_DropMap;
int g_iFenrirStuff_02_DropRate;
int g_iFenrirStuff_03_DropLv_Min;
int g_iFenrirStuff_03_DropLv_Max;
int g_iFenrirStuff_03_DropMap;
int g_iFenrirStuff_03_DropRate;
int g_iFenrirRepairRate;
int g_iFenrirDefaultMaxDurSmall;
int g_iFenrirElfMaxDurSmall;
int g_iFenrir_01Level_MixRate;
int g_iFenrir_02Level_MixRate;
int g_iFenrir_03Level_MixRate;
int g_iSkillDistanceCheck;
int g_iSkillDistanceCheckTemp;
int g_iSkillDistanceKick;
int g_iSkillDistanceKickCount;
int g_iSkillDiatanceKickCheckTime;
BOOL g_bHallowinDayEventOn;
int g_iHallowinDayEventItemDropRate;
int g_iHallowinDayEventJOLBlessDropRate;
int g_iHallowinDayEventJOLAngerDropRaTe;
int g_iHallowinDayEventJOLScreamDropRate;
int g_iHallowinDayEventJOLFoodDropRate;
int g_iHallowinDayEventJOLDrinkDropRate;
int g_iHallowinDayEventJOLPolymorphRingDropRate;
BOOL g_bSantaPolymorphRingDropOn;
int g_iSantaPolymorphRingDropRate;
int g_iCondorFlameDropRate;
int g_iShadowPhantomMaxLevel;
bool g_bBlossomBoxDropOn;
int g_iBlossomBoxDropRate;

DWORD dwgCheckSum[MAX_CHECKSUM_KEY];
BOOL g_bCastleGuildDestoyLimit;
DWORD  gItemNumberCount;
BOOL gStalkProtocol;
int  gYear;
BOOL gOnlyFireCrackerEffectUse;
int  giKundunRefillHPSec;
int  giKundunRefillHP;
int  giKundunRefillHPTime;
int  giKundunHPLogSaveTime;
BOOL gUseNPGGChecksum;

////////////////////////////////////////////////////////////////////

char szClientVersion[8]="000000";
BOOL bCanTrade=1;
BOOL bCanChaosBox=1;
BOOL bCanWarehouseLock=1;

BOOL gEnableBattleSoccer=1;
int  gLootingTime=3;
int  IsMultiWareHouse=1;
int  g_WareHouseCount=3;
int gItemDisapearTime=120;
int  gEvent1ItemDropTodayMax=1;
int  gEvent1ItemDropTodayPercent=80;
int  gCharacterDeleteMinLevel=40;
BOOL gCreateCharacter=1;
int  g_XMasEvent_StarOfXMasDropRate=80;
int  g_XMasEvent_ItemDropRateForStarOfXMas=2;
int  gFireCrackerDropRate=80;
int  g_ItemDropRateForgFireCracker=2;
int  gHeartOfLoveDropRate=80;
int  g_ItemDropRateForgHeartOfLove=2;
int  gGoldMedalDropRate=2;
int  gSilverMedalDropRate=2;
int  g_ItemDropRateForGoldMedal=2;
int  g_ItemDropRateForSilverMedal=2;
int  gBoxOfGoldDropRate=2;
int  g_ItemDropRateForBoxOfGold=2;
int  g_EventChipDropRateForBoxOfGold=2;
int  gDQChaosSuccessRateLevel1=75;
int  gDQChaosSuccessRateLevel2=70;
int  gDQChaosSuccessRateLevel3=65;
int  gDQChaosSuccessRateLevel4=60;
int  gDQChaosSuccessRateLevel5=55;
int  gDQChaosSuccessRateLevel6=50;
int	 gDQChaosSuccessRateLevel7=45;
DWORD  gAttackSpeedTimeLimit=130;
DWORD  gHackCheckCount=5;
float gDecTimePerAttackSpeed=5.33f;
int  gMinimumAttackSpeedTime=8;
int  gDetectedHackKickCount=10;
BOOL gItemDropRingOfTransform=1;
int  gQuestNPCTeleportTime=60;
BOOL gEnableCheckPenetrationSkill=TRUE;

short gPostLevel;
int gPostMoney;
char gIsDropCmd;

int gSilverBoxDropRate;
int gGoldBoxDropRate;

int gSetPartyExp2;
int gSetPartyExp3;
int gSetPartyExp4;
int gSetPartyExp5;
int gPartyExp1;
int gPartyExp2;
int gPartyExp3;
int gPartyExp4;
int gPartyExp5;

int gWeaponDefenseDamage;
int gWeaponAttackDamage;

int gIsDropSetItemInCastleHuntZone;
int gSetItemInCastleHuntZoneDropRate;
int gSetItemInCastleHuntZoneDropLevel;
