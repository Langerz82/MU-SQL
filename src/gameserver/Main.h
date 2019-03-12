#ifndef _MU_MAIN_GS_H_
#define _MU_MAIN_GS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "StdAfx.h"
#include "resource.h"
//#include "ServerEngine.h"
#include "Database/Database/DatabaseEnv.h"
#include "Game/Filters/AntiSwear.h"
#include "Game/Filters/ProhibitedSymbols.h"

//#include "wsJoinServerCli.h"
#include "wsGameServer.h"
#include "DirPath.h"
#include "MapClass.h"
#include "classdef.h"
#include "CLoginCount.h"
//#include "DragonEvent.h"
//#include "AttackEvent.h"
#include "WhisperCash.h"
//#include "WzUdp.h"
#include "IOCP.h"
#include "MonsterAttr.h"
#include "MonsterSetBase.h"
#include "PartyClass.h"
//#include "CastleSiege.h"
//#include "CastleDeepEvent.h"
//#include "Crywolf.h"
#include "GuildClass.h"


class ConnectDatabaseConnection;
class MUDatabase;


#define SET_NUMBERH(x) ( (BYTE)((DWORD)(x)>>(DWORD)8) )
#define SET_NUMBERL(x) ( (BYTE)((DWORD)(x) & 0xFF) )
#define SET_NUMBERHW(x) ( (WORD)((DWORD)(x)>>(DWORD)16) )
#define SET_NUMBERLW(x) ( (WORD)((DWORD)(x) & 0xFFFF) )

extern WORD g_GameServerPort;
extern DWORD g_MaxConnectionsPerIP;
extern DWORD g_MaxPacketPerSec;
extern WORD g_ServerCode;

extern std::string g_HostURL;
extern std::string g_WhiteListIP;

extern std::string g_ServerAddress;
extern std::string g_DBPort;
extern std::string g_UserID;
extern std::string g_Password;
extern std::string g_MuOnlineDB;

extern DWORD g_MachineIDConnectionLimitPerGroup;

extern std::string szWANIP;
extern WORD g_GameServerListPort;

extern std::string g_logsDir;
extern std::string g_logsEntryCount;
extern std::string g_logsEntry[10]; // up to 10 logs

extern BOOL gDisconnectHackUser;

extern char szClientVersion[8];

extern BOOL JoinServerConnected;
extern BOOL DataServerConnected;
extern BOOL GameServerCreated;
extern BOOL IsEventChipServerConnected;
//extern CDragonEvent * DragonEvent;
//extern CAttackEvent * AttackEvent;

extern int	gCheckSumSwitch;
extern int  gSpeedHackPenalty;
extern BOOL gEnableEventNPCTalk;
extern BOOL gEnableServerDivision;
extern BOOL gEvent1;
extern int  gMonsterHp;
extern BOOL gMerryXMasNpcEvent;
extern BOOL gHappyNewYearNpcEvent;
extern int  gEvent1ItemDropTodayCount;
extern char gStalkProtocolId[11];
extern BOOL gXMasEvent;
extern BOOL gFireCrackerEvent;
extern BOOL gHeartOfLoveEvent;
extern BOOL gMedalEvent;
extern BOOL gEventChipEvent;
extern BOOL gWriteSkillLog;
extern BOOL g_bStoneItemDrop;
extern int  g_iKundunMarkDropRate;
extern int  g_iMarkOfTheLord;
extern int  g_iLuckyCoin;
extern int	g_iXmasEventLuckNumber1st;
extern int	g_iXmasEventLuckNumber2nd;
extern int  g_iDarkLordHeartDropRate;
extern int  g_iDarkLordHeartOffEventRate;
extern int g_iMysteriousBeadDropRate1;
extern int g_iMysteriousBeadDropRate2;
extern int g_iHiddenTreasureBoxOfflineRate;
extern BOOL bIsIgnorePacketSpeedHackDetect;
extern BOOL gIsKickDetecHackCountLimit;
extern BOOL gIsItemDropRingOfTransform;
extern BOOL gDoPShopOpen;
extern BOOL gWriteChatLog;
extern BOOL gDisconnectHackUser;
extern BOOL GSInfoSendFlag;
//float gItemBuyPriceMultiplier;
//float gItemSellPriceMultiplier;
extern DWORD JoinServerDCTime;
//extern CwsGameServer wsGServer;	// line : 213GameServer
//extern wsJoinServerCli wsJServerCli;	// line : 214 Join Server
//extern wsJoinServerCli wsDataCli;	// line : 215 DataServer
//extern wsJoinServerCli wsExDbCli;	// line : 239 Extra DataBase Server
extern CDirPath gDirPath;	// line : 248 Directory Path
extern MapClass MapC[MAX_NUMBER_MAP];	// line 249	// Map Manager
extern CMonsterAttr gMAttr;	// line 250
extern CMonsterSetBase gMSetBase;	// line 251
extern classdef DCInfo;	// line 252:
extern CWhisperCash WhisperCash;	// line 253
extern PartyClass gParty;	// line 254
extern CGuildClass Guild;	// line 265
//extern WzUdp gUdpSoc;	// line 256
//extern WzUdp gLauncherUdp;
extern CLoginCount gLCount[3];	// line 329
extern UINT64 * gLevelExperience;
extern char szGameServerExeSerial[24];
extern BOOL gIsDropDarkLordItem;
extern int  gSleeveOfLordDropRate;
extern int  gSleeveOfLordDropLevel;
extern int  gSoulOfDarkHorseDropRate;
extern int  gSoulOfDarkHorseropLevel;
extern int  gSoulOfDarkSpiritDropRate;
extern int  gSoulOfDarkSpiritDropLevel;
extern float gDarkSpiritAddExperience;
extern BOOL gIsDropGemOfDefend;
extern int  gGemOfDefendDropRate;
extern int  gGemOfDefendDropLevel;
extern int  g_iUseCharacterAutoRecuperationSystem;
extern int  g_iCharacterRecuperationMaxLevel;
extern int g_iServerGroupGuildChatting;
extern int g_iServerGroupUnionChatting;
extern BOOL g_bRibbonBoxEvent;
extern int g_iRedRibbonBoxDropLevelMin;
extern int g_iRedRibbonBoxDropLevelMax;
extern int g_iRedRibbonBoxDropRate;
extern int g_iGreenRibbonBoxDropLevelMin;
extern int g_iGreenRibbonBoxDropLevelMax;
extern int g_iGreenRibbonBoxDropRate;
extern int g_iBlueRibbonBoxDropLevelMin;
extern int g_iBlueRibbonBoxDropLevelMax;
extern int g_iBlueRibbonBoxDropRate;
extern BOOL g_bChocolateBoxEvent;
extern int g_iPinkChocolateBoxDropLevelMin;
extern int g_iPinkChocolateBoxDropLevelMax;
extern int g_iPinkChocolateBoxDropRate;
extern int g_iRedChocolateBoxDropLevelMin;
extern int g_iRedChocolateBoxDropLevelMax;
extern int g_iRedChocolateBoxDropRate;
extern int g_iBlueChocolateBoxDropLevelMin;
extern int g_iBlueChocolateBoxDropLevelMax;
extern int g_iBlueChocolateBoxDropRate;
extern BOOL g_bCandyBoxEvent;
extern int g_iLightPurpleCandyBoxDropLevelMin;
extern int g_iLightPurpleCandyBoxDropLevelMax;
extern int g_iLightPurpleCandyBoxDropRate;
extern int g_iVermilionCandyBoxDropLevelMin;
extern int g_iVermilionCandyBoxDropLevelMax;
extern int g_iVermilionCandyBoxDropRate;
extern int g_iDeepBlueCandyBoxDropLevelMin;
extern int g_iDeepBlueCandyBoxDropLevelMax;
extern int g_iDeepBlueCandyBoxDropRate;
extern BOOL g_bFenrirStuffItemDrop;
extern int g_iFenrirStuff_01_DropLv_Min;
extern int g_iFenrirStuff_01_DropLv_Max;
extern int g_iFenrirStuff_01_DropMap;
extern int g_iFenrirStuff_01_DropRate;
extern int g_iFenrirStuff_02_DropLv_Min;
extern int g_iFenrirStuff_02_DropLv_Max;
extern int g_iFenrirStuff_02_DropMap;
extern int g_iFenrirStuff_02_DropRate;
extern int g_iFenrirStuff_03_DropLv_Min;
extern int g_iFenrirStuff_03_DropLv_Max;
extern int g_iFenrirStuff_03_DropMap;
extern int g_iFenrirStuff_03_DropRate;
extern int g_iFenrirRepairRate;
extern int g_iFenrirDefaultMaxDurSmall;
extern int g_iFenrirElfMaxDurSmall;
extern int g_iFenrir_01Level_MixRate;
extern int g_iFenrir_02Level_MixRate;
extern int g_iFenrir_03Level_MixRate;
extern int g_iSkillDistanceCheck;
extern int g_iSkillDistanceCheckTemp;
extern int g_iSkillDistanceKick;
extern int g_iSkillDistanceKickCount;
extern int g_iSkillDiatanceKickCheckTime;
extern BOOL g_bHallowinDayEventOn;
extern int g_iHallowinDayEventItemDropRate;
extern int g_iHallowinDayEventJOLBlessDropRate;
extern int g_iHallowinDayEventJOLAngerDropRaTe;
extern int g_iHallowinDayEventJOLScreamDropRate;
extern int g_iHallowinDayEventJOLFoodDropRate;
extern int g_iHallowinDayEventJOLDrinkDropRate;
extern int g_iHallowinDayEventJOLPolymorphRingDropRate;
extern BOOL g_bSantaPolymorphRingDropOn;
extern int g_iSantaPolymorphRingDropRate;
extern int g_iCondorFlameDropRate;
extern bool g_bBlossomBoxDropOn;
extern int g_iBlossomBoxDropRate;
extern int g_iShadowPhantomMaxLevel;

extern DWORD dwgCheckSum[MAX_CHECKSUM_KEY];
extern DWORD  gItemNumberCount;
extern BOOL gStalkProtocol;
extern int  gYear;
extern BOOL gOnlyFireCrackerEffectUse;
extern int  giKundunRefillHPSec;
extern int  giKundunRefillHP;
extern int  giKundunRefillHPTime;
extern int  giKundunHPLogSaveTime;
extern BOOL gUseNPGGChecksum;
extern char szGameServerVersion[24];
extern char szClientVersion[8];
extern BOOL bCanTrade;
extern BOOL bCanChaosBox;
extern BOOL bCanWarehouseLock;
extern BOOL gEnableBattleSoccer;
extern int  gLootingTime;
extern int gItemDisapearTime;
extern int  IsMultiWareHouse;
extern int  g_WareHouseCount;
extern int  gEvent1ItemDropTodayMax;
extern int  gEvent1ItemDropTodayPercent;
extern int  gCharacterDeleteMinLevel;
extern BOOL gCreateCharacter;
extern int  g_XMasEvent_StarOfXMasDropRate;
extern int  g_XMasEvent_ItemDropRateForStarOfXMas;
extern int  gFireCrackerDropRate;
extern int  g_ItemDropRateForgFireCracker;
extern int  gHeartOfLoveDropRate;
extern int  g_ItemDropRateForgHeartOfLove;
extern int  gGoldMedalDropRate;
extern int  gSilverMedalDropRate;
extern int  g_ItemDropRateForGoldMedal;
extern int  g_ItemDropRateForSilverMedal;
extern int  gBoxOfGoldDropRate;
extern int  g_ItemDropRateForBoxOfGold;
extern int  g_EventChipDropRateForBoxOfGold;
extern int  gDQChaosSuccessRateLevel1;
extern int  gDQChaosSuccessRateLevel2;
extern int  gDQChaosSuccessRateLevel3;
extern int  gDQChaosSuccessRateLevel4;
extern int  gDQChaosSuccessRateLevel5;
extern int  gDQChaosSuccessRateLevel6;
extern int	gDQChaosSuccessRateLevel7;
extern DWORD  gAttackSpeedTimeLimit;
extern DWORD  gHackCheckCount;
extern float gDecTimePerAttackSpeed;
extern int  gMinimumAttackSpeedTime;
extern int  gDetectedHackKickCount;
extern BOOL gItemDropRingOfTransform;
extern int  gQuestNPCTeleportTime;
extern BOOL gEnableCheckPenetrationSkill;

extern char gIsDropCmd;

extern int gSilverBoxDropRate;
extern int gGoldBoxDropRate;

extern int gSetPartyExp2;
extern int gSetPartyExp3;
extern int gSetPartyExp4;
extern int gSetPartyExp5;
extern int gPartyExp1;
extern int gPartyExp2;
extern int gPartyExp3;
extern int gPartyExp4;
extern int gPartyExp5;

extern int gWeaponDefenseDamage;
extern int gWeaponAttackDamage;

extern CCastleSiege g_CastleSiege;
extern CCastleDeepEvent g_CastleDeepEvent;
extern int gIsDropSetItemInCastleHuntZone;
extern int gSetItemInCastleHuntZoneDropRate;
extern int gSetItemInCastleHuntZoneDropLevel;

#endif
