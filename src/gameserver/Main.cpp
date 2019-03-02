/// \addtogroup Connect Server Daemon
/// @{
/// \file


#include "StdAfx.h"
#include "Main.h"
#include "Logging/Log.h"
#include "Asio/IoContext.h"
#include "IOCP.h"
#include "ServerData.h"

// Extracted from GameMain.cpp
#include "Lang.h"
//#include "gamemain.h"
//#include "winutil.h"
#include "GameServer.h"
#include "DirPath.h"
//#include "TLog.h"
#include "wsJoinServerCli.h"
#include "DoppelGanger.h"
//#include "DSProtocol.h"
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
#include "CUserData.h"
#include "CGameObject.h"
#include "GOFunctions.h"
#include "GOEventFunctions.h"
#include "PacketCheckSum.h"
#include "DragonEvent.h"
#include "AttackEvent.h"
#include "MapServerManager.h"
#include "QuestInfo.h"
#include "SkillHitBox.h"
#include "CItemDrop.h"
#include "EventManagement.h"
#include "SetItemOption.h"
#include "ItemSocketOptionSystem.h"
#include "EDSProtocol.h"
#include "MonsterItemMng.h"
#include "ConMember.h"
#include "GMMng.h"
#include "BuffScriptLoader.h"
#include "BattleSoccerManager.h"
#include "GuildClass.h"
#include "MoveCommand.h"
#include "BloodCastle.h"
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
#include "BonusEvent.h"
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
//#include "LogToFile.h"
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
#include "AcheronGuardianEvent.h"
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



#include "database/Database/DatabaseLoader.h"
#include "database/Database/MySQLThreading.h"

#include "Config/iniReader/cpp/INIReader.h"

#include "revision.h"
#include "Util.h"

#include <iostream>
#include <csignal>

#include <openssl/opensslv.h>
#include <openssl/crypto.h>

#include <ace/Get_Opt.h>
#include <ace/Dev_Poll_Reactor.h>
#include <ace/TP_Reactor.h>
#include <ace/ACE.h>
#include <ace/Acceptor.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/Reactor.h>

#include <boost/asio/signal_set.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

#ifdef WIN32
#include "ServiceWin32.h"
char serviceName[20] = "Mu Game Server";
char serviceLongName[100] = "muOnline MySQL Server service";
char serviceDescription[255] = "Massive Network Game Object Server";

/*
 * -1 - not in service mode
 *  0 - stopped
 *  1 - running
 *  2 - paused
 */
int m_ServiceStatus = -1;
#else
#include "PosixDaemon.h"
#endif

INIReader* iniReader;

WORD g_ConnectServerPort;
DWORD g_MaxConnectionsPerIP = 1000;
DWORD g_MaxPacketPerSec = 16;
WORD g_FTPPort;
std::string g_HostURL;
std::string g_FTPLogin;
std::string g_FTPPassword;
std::string g_VersionFile;
std::string g_ClientVersion;
std::string g_WhiteListIP;
BOOL gDisconnectHackUser = FALSE;
DWORD g_MachineIDConnectionLimitPerGroup = 10;
WORD g_ServerCode;
WORD g_GameServerListPort;
WORD g_MagumsaCreateMinLevel;
WORD g_DarkLordCreateMinLevel;
WORD g_GrowLancerCreateMinLevel;

// GLobals thats whats up.
std::string szWANIP;
int g_dwMaxServerGroups;

BOOL g_PwEncrypt;
BOOL g_DSMode;

std::string g_AWHostPass;
std::string g_MapSvrFilePath;

std::string g_logsDir;
std::string g_logsEntryCount;
std::string g_logsEntry[10]; // up to 10 logs

std::string g_ServerAddress;
std::string g_DBPort;
std::string g_UserID;
std::string g_Password;
std::string g_MuOnlineDB;

char szClientVersion[8] = "118260";

int  gCharacterDeleteMinLevel = 40;
BOOL gCreateCharacter = 1;


void UnhookSignals();
void HookSignals();

bool stopEvent = false;                                     ///< Setting it to true stops the server

//typedef BYTE BYTE;

BOOL JoinServerConnected;
BOOL DataServerConnected;
BOOL GameServerCreated;
BOOL IsEventChipServerConnected;
CDragonEvent * DragonEvent;
CAttackEvent * AttackEvent;
//WzUdp gUdpSoc;	// line 256
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

BOOL bCanTrade = 1;
BOOL bCanChaosBox = 1;
BOOL bCanWarehouseLock = 1;

BOOL gEnableBattleSoccer = 1;
int  gLootingTime = 3;
int  IsMultiWareHouse = 1;
int  g_WareHouseCount = 3;
int gItemDisapearTime = 120;
int  gEvent1ItemDropTodayMax = 1;
int  gEvent1ItemDropTodayPercent = 80;
int  gCharacterDeleteMinLevel = 40;
BOOL gCreateCharacter = 1;
int  g_XMasEvent_StarOfXMasDropRate = 80;
int  g_XMasEvent_ItemDropRateForStarOfXMas = 2;
int  gFireCrackerDropRate = 80;
int  g_ItemDropRateForgFireCracker = 2;
int  gHeartOfLoveDropRate = 80;
int  g_ItemDropRateForgHeartOfLove = 2;
int  gGoldMedalDropRate = 2;
int  gSilverMedalDropRate = 2;
int  g_ItemDropRateForGoldMedal = 2;
int  g_ItemDropRateForSilverMedal = 2;
int  gBoxOfGoldDropRate = 2;
int  g_ItemDropRateForBoxOfGold = 2;
int  g_EventChipDropRateForBoxOfGold = 2;
int  gDQChaosSuccessRateLevel1 = 75;
int  gDQChaosSuccessRateLevel2 = 70;
int  gDQChaosSuccessRateLevel3 = 65;
int  gDQChaosSuccessRateLevel4 = 60;
int  gDQChaosSuccessRateLevel5 = 55;
int  gDQChaosSuccessRateLevel6 = 50;
int	 gDQChaosSuccessRateLevel7 = 45;
DWORD  gAttackSpeedTimeLimit = 130;
DWORD  gHackCheckCount = 5;
float gDecTimePerAttackSpeed = 5.33f;
int  gMinimumAttackSpeedTime = 8;
int  gDetectedHackKickCount = 10;
BOOL gItemDropRingOfTransform = 1;
int  gQuestNPCTeleportTime = 60;
BOOL gEnableCheckPenetrationSkill = TRUE;

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


bool initDB()
{
	g_ServerAddress = iniReader->GetString("SQL", "ServerAddress", "127.0.0.1");
	g_DBPort = iniReader->GetString("SQL", "Port", "3306");
	g_UserID = iniReader->GetString("SQL", "User", "sa");
	g_Password = iniReader->GetString("SQL", "Pass", "sa123");
	g_MuOnlineDB = iniReader->GetString("SQL", "MuOnlineDB", "mu_data");

	MySQL::Library_Init();

	DatabaseLoader loader("server.connectserver", DatabaseLoader::DATABASE_NONE);
	loader.ConnectInfo((char*) g_ServerAddress.c_str(), (char*)g_DBPort.c_str(),
		(char*)g_UserID.c_str(), (char*)g_Password.c_str(), (char*)g_MuOnlineDB.c_str());

	loader.AddDatabase(gMUDatabase, "Connect");

	if (!loader.Load())
		return false;

	
	sLog->outBasic("Started auth database connection pool.");
	return true;
}

/// Close the connection to the database
void StopDB()
{	
	//gMUDatabase.Close();
	MySQL::Library_End();
}

void KeepDatabaseAliveHandler(std::weak_ptr<boost::asio::deadline_timer> dbPingTimerRef, int32 dbPingInterval, boost::system::error_code const& error)
{
	if (!error)
	{
		if (std::shared_ptr<boost::asio::deadline_timer> dbPingTimer = dbPingTimerRef.lock())
		{
			sLog->outBasic("Ping MySQL to keep connection alive");
			//ConnectDatabase.KeepAlive();

			dbPingTimer->expires_from_now(boost::posix_time::minutes(dbPingInterval));
			dbPingTimer->async_wait(std::bind(&KeepDatabaseAliveHandler, dbPingTimerRef, dbPingInterval, std::placeholders::_1));
		}
	}
}


void SignalHandler(std::weak_ptr<Asio::IoContext> ioContextRef, boost::system::error_code const& error, int /*signalNumber*/)
{
	if (!error)
		if (std::shared_ptr<Asio::IoContext> ioContext = ioContextRef.lock())
			ioContext->stop();
}

/// Print out the usage string for this program on the console.
void usage(const char* prog)
{
    sLog->outBasic("Usage: \n %s [<options>]\n"
#ifdef WIN32
                   "    Running as service functions:\n\r"
                   "    -s run                   run as service\n\r"
                   "    -s install               install service\n\r"
                   "    -s uninstall             uninstall service\n\r"
#else
                   "    Running as daemon functions:\n\r"
                   "    -s run                   run as daemon\n\r"
                   "    -s stop                  stop daemon\n\r"
#endif
                   , prog);
}

void LoadLog()
{
    g_logsDir = iniReader->GetString("Logger", "LogDirectory", "logs");

	std::vector<std::string> vecLogEntries;
	std::vector<std::string> vecLogEntryNames;
	int i = 0;
	while (true)
	{
		std::string tempChars;
		std::string temp = "";
		std::string logEntry = StringFormat("LogEntry%d", i++);
		tempChars = iniReader->GetString("Logger", logEntry, "");
		if (tempChars == "")
			break;
		vecLogEntries.push_back(tempChars);
		vecLogEntryNames.push_back(logEntry);
	}

	std::vector<std::string> vecAppendEntries;
	std::vector<std::string> vecAppendEntryNames;
	i = 0;
	while (true)
	{
		std::string tempChars;
		std::string temp = "";
		std::string appendEntry = StringFormat("AppendEntry%d", i++);
		tempChars = iniReader->GetString("Appender", appendEntry, "");
		if (tempChars == "")
			break;
		vecAppendEntries.push_back(tempChars);
		vecAppendEntryNames.push_back(appendEntry);
	}
	sLog->Initialize(nullptr, ".", vecLogEntries, vecAppendEntries, vecLogEntryNames, vecAppendEntryNames);
}

void LoadConfig()
{
    std::string filename = "GameServer.ini";
    iniReader = new INIReader(filename);
    if (iniReader->ParseError() < 0) {
        std::cout << StringFormat("Can't load '%s', Error code: %d\n", filename, iniReader->ParseError());
        return;
    }
    std::cout << StringFormat("Config loaded from '%s': version=", filename)
              << iniReader->GetInteger("protocol", "version", -1) << ", name="
              << iniReader->Get("user", "name", "UNKNOWN") << ", email="
              << iniReader->Get("user", "email", "UNKNOWN") << ", pi="
              << iniReader->GetReal("user", "pi", -1) << ", active="
              << iniReader->GetBoolean("user", "active", true) << "\n";

    g_DSMode = iniReader->GetInteger("SETTINGS", "DataServerOnlyMode", 0);
	g_GameServerListPort = iniReader->GetInteger("SETTINGS", "TCP_PORT", 44407);
	szWANIP = iniReader->GetString("SETTINGS", "WanIP", "127.0.0.1");
	g_ServerCode = iniReader->GetInteger("SETTINGS", "ServerCode", 0);

	g_MagumsaCreateMinLevel = iniReader->GetInteger("SETTINGS", "MagicGladiatorCreateMinLevel", 220);
	g_DarkLordCreateMinLevel = iniReader->GetInteger("SETTINGS", "DarkLordCreateMinLevel", 250);
	g_GrowLancerCreateMinLevel = iniReader->GetInteger("SETTINGS", "GrowLancerCreateMinLevel", 200);

	SwearFilter.LoadFile("./IGCData/IGC_ProhibitedWords.xml");
	g_prohibitedSymbols.LoadSymbolFile("./IGCData/IGC_ProhibitedSymbols.xml");

}

/// Launch the realm server
extern int main(int argc, char** argv)
{
	LoadConfig();
	LoadLog();

    ///- Command line parsing
    char* options = ":s:";

    ACE_Get_Opt cmd_opts(argc, argv, options);
    cmd_opts.long_option("version", 'v');

    char serviceDaemonMode = '\0';

    int option;
    while ((option = cmd_opts()) != EOF)
    {
        switch (option)
        {
            case 's':
            {
                const char* mode = cmd_opts.opt_arg();

                if (!strcmp(mode, "run"))
                    { serviceDaemonMode = 'r'; }
#ifdef WIN32
                else if (!strcmp(mode, "install"))
                    { serviceDaemonMode = 'i'; }
                else if (!strcmp(mode, "uninstall"))
                    { serviceDaemonMode = 'u'; }
#else
                else if (!strcmp(mode, "stop"))
                    { serviceDaemonMode = 's'; }
#endif
                else
                {
                    sLog->outError("Runtime-Error: -%c unsupported argument %s", cmd_opts.opt_opt(), mode);
                    usage(argv[0]);
                    return 1;
                }
                break;
            }
            case ':':
                sLog->outError("Runtime-Error: -%c option requires an input argument", cmd_opts.opt_opt());
                usage(argv[0]);
                return 1;
            default:
                sLog->outError("Runtime-Error: bad format of commandline arguments");
                usage(argv[0]);
                return 1;
        }
    }

#ifdef WIN32                                                // windows service command need execute before config read
    switch (serviceDaemonMode)
    {
        case 'i':
            if (WinServiceInstall())
                { sLog->outBasic("Installing service"); }
            return 1;
        case 'u':
            if (WinServiceUninstall())
                { sLog->outBasic("Uninstalling service"); }
            return 1;
        case 'r':
            WinServiceRun();
            break;
    }
#endif


#ifndef WIN32                                               // posix daemon commands need apply after config read
    switch (serviceDaemonMode)
    {
        case 'r':
            startDaemon();
            break;
        case 's':
            stopDaemon();
            break;
    }
#endif

	// Initialize the database connection
	if (!initDB())
	{
		sLog->outError("ERROR: Cannot connect to DB.");
		return 1;
	}
	std::shared_ptr<void> dbHandle(nullptr, [](void*) { StopDB(); });

	
	std::shared_ptr<Asio::IoContext> ioContext = std::make_shared<Asio::IoContext>();

	// Set signal handlers
	boost::asio::signal_set signals(*ioContext, SIGINT, SIGTERM);
#if WIN32
	signals.add(SIGBREAK);
#endif
	signals.async_wait(std::bind(&SignalHandler, std::weak_ptr<Asio::IoContext>(ioContext), std::placeholders::_1, std::placeholders::_2));

	// Enabled a timed callback for handling the database keep alive ping
	int32 dbPingInterval = 30;
	std::shared_ptr<boost::asio::deadline_timer> dbPingTimer = std::make_shared<boost::asio::deadline_timer>(*ioContext);
	dbPingTimer->expires_from_now(boost::posix_time::minutes(dbPingInterval));
	dbPingTimer->async_wait(std::bind(&KeepDatabaseAliveHandler, std::weak_ptr<boost::asio::deadline_timer>(dbPingTimer), dbPingInterval, std::placeholders::_1));

	
    sLog->outBasic("%s [realm-daemon]", REVISION_NR);
    sLog->outBasic("<Ctrl-C> to stop.\n");

    sLog->outBasic("%s (Library: %s)", OPENSSL_VERSION_TEXT, SSLeay_version(SSLEAY_VERSION));
    if (SSLeay() < 0x009080bfL)
    {
        sLog->outError("WARNING: Outdated version of OpenSSL lib. Logins to server may not work!");
		sLog->outError("WARNING: Minimal required version [OpenSSL 0.9.8k]");
    }

    sLog->outBasic("Using ACE: %s", ACE_VERSION);

    sLog->outBasic("Max allowed open files is %d", ACE::max_handles());

    /// MuSQLServer PID file creation
    std::string pidfile = "";
    if (!pidfile.empty())
    {
        //uint32 pid = CreatePIDFile(pidfile);
		int32 pid = 0; // stub
        if (!pid)
        {
            sLog->outError("Can not create PID file %s.\n", pidfile.c_str());
            return 1;
        }
        sLog->outBasic("Daemon PID: %u\n", pid);
    }

    ///- Catch termination signals
    HookSignals();

    ///- Handle affinity for multiple processors and process priority on Windows
#ifdef WIN32
    {
        HANDLE hProcess = GetCurrentProcess();

        uint32 Aff = 0;
        if (Aff > 0)
        {
            ULONG_PTR appAff;
            ULONG_PTR sysAff;

            if (GetProcessAffinityMask(hProcess, &appAff, &sysAff))
            {
                ULONG_PTR curAff = Aff & appAff;            // remove non accessible processors

                if (!curAff)
                {
                    sLog->outError("Processors marked in UseProcessors bitmask (hex) %x not accessible for MuSQLServer. Accessible processors bitmask (hex): %x", Aff, appAff);
                }
                else
                {
                    if (SetProcessAffinityMask(hProcess, curAff))
                        { sLog->outBasic("Using processors (bitmask, hex): %x", curAff); }
                    else
                        { sLog->outError("Can't set used processors (hex): %x", curAff); }
                }
            }
        }

        bool Prio = false;

        if (Prio)
        {
            if (SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS))
                { sLog->outBasic("MuSQLServer process priority class set to HIGH"); }
            else
                { sLog->outError("Can't set MuSQLServer process priority class."); }
        }
    }
#endif

    if(fs::create_directory("LOG"))
    {
        sLog->outBasic("Directory LOG Created.");
    }

	sLog->outBasic( "Initializing...");

	//LoadAllowableIpList("./AllowedIPList.ini");

	//g_MapServerManager.LoadMapData(g_MapSvrFilePath);


	//gObjServerInit();
	GIOCP.GiocpInit();
	sLog->outBasic("CreatingListenSocket - address: %s, port: %d.", szWANIP, g_GameServerListPort);
	GIOCP.CreateListenSocket(g_GameServerListPort, (char*) szWANIP.c_str());

	// TODO - Make sure working from TC
	//sLog->outBasic("ioContext->run()");
	//ioContext->run();

#ifndef WIN32
    detachDaemon();
#endif
	sLog->outBasic("Entering loop.");
    ///- Wait for termination signal
    while (true)
    {

		GIOCP.ProcessEvents();
		mSleep(100);

#ifdef WIN32
        if (m_ServiceStatus == 0) { stopEvent = true; }
        while (m_ServiceStatus == 2) { Sleep(1000); }
#endif
    }

    ///- Remove signal handling before leaving
    UnhookSignals();

    sLog->outBasic("Halting process...");

	signals.cancel();

    delete iniReader;
    return 0;
}

/// Handle termination signals
/** Put the global variable stopEvent to 'true' if a termination signal is caught **/
void OnSignal(int s)
{
    switch (s)
    {
        case SIGINT:
        case SIGTERM:
            stopEvent = true;
            break;
#ifdef _WIN32
        case SIGBREAK:
            stopEvent = true;
            break;
#endif
    }

    signal(s, OnSignal);
}

/// Define hook 'OnSignal' for all termination signals
void HookSignals()
{
    signal(SIGINT, OnSignal);
    signal(SIGTERM, OnSignal);
#ifdef _WIN32
    signal(SIGBREAK, OnSignal);
#endif
}

/// Unhook the signals before leaving
void UnhookSignals()
{
    signal(SIGINT, 0);
    signal(SIGTERM, 0);
#ifdef _WIN32
    signal(SIGBREAK, 0);
#endif
}

/// @}
