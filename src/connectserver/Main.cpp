
// TODO - Make sure program signalling works.

/// \addtogroup realmd Realm Daemon
/// @{
/// \file

#include "Main.h"
#include "database/Logging/AppenderDB.h"
#include "Asio/IoContext.h"
#include "CSDatabase.h"

#include "IOCP.h"

#include "database/Database/DatabaseEnv.h"
#include "database/Database/DatabaseLoader.h"
#include "database/Database/MySQLThreading.h"

#include "Config/Config.h"

#include "SystemConfig.h"
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

#include <boost/asio/signal_set.hpp>
#include <boost/program_options.hpp>

#ifdef WIN32
#include "ServiceWin32.h"
char serviceName[] = "muSqlServer";
char serviceLongName[] = "muOnline MySQL Server service";
char serviceDescription[] = "Massive Network Game Object Server";
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


// GLobals thats whats up.
TCHAR szWANIP[150];
int g_dwMaxServerGroups = GetPrivateProfileInt("SETTINGS", "MAX_SERVER", 10, ".\\DataServer.ini") * MAX_SERVER_TYPE;
WORD g_JoinServerListPort = GetPrivateProfileInt("SETTINGS", "JoinServerPort", 55970, ".\\DataServer.ini");
WORD g_DataServerListPort = GetPrivateProfileInt("SETTINGS", "DataServerPort", 55960, ".\\DataServer.ini");
WORD g_ExDataServerListPort = GetPrivateProfileInt("SETTINGS", "ExDataServerPort", 55906, ".\\DataServer.ini");
BOOL g_PwEncrypt = GetPrivateProfileInt("SQL", "PasswordEncryptType", 1, ".\\DataServer.ini");
BOOL g_DSMode = GetPrivateProfileInt("SETTINGS", "DataServerOnlyMode", 0, ".\\DataServer.ini");
BOOL g_UseJoinServer = GetPrivateProfileInt("SETTINGS", "UseJoinServer", 1, ".\\DataServer.ini");
BOOL g_UseDataServer = GetPrivateProfileInt("SETTINGS", "UseDataServer", 1, ".\\DataServer.ini");
BOOL g_UseExDataServer = GetPrivateProfileInt("SETTINGS", "UseExDataServer", 1, ".\\DataServer.ini");
DWORD g_GensRankingUpdateTime = 2;
DWORD g_GensLeaveAfterDays = GetPrivateProfileInt("GensSystem", "GensReJoinDaysLimit", 7, ".\\DataServer.ini");
DWORD g_MachineIDConnectionLimitPerGroup = GetPrivateProfileInt("SETTINGS", "MachineIDConnectionLimitPerGroup", 3, ".\\DataServer.ini");
WORD g_MagumsaCreateMinLevel = GetPrivateProfileInt("SETTINGS", "MagicGladiatorCreateMinLevel", 220, ".\\DataServer.ini");
WORD g_DarkLordCreateMinLevel = GetPrivateProfileInt("SETTINGS", "DarkLordCreateMinLevel", 250, ".\\DataServer.ini");
WORD g_GrowLancerCreateMinLevel = GetPrivateProfileInt("SETTINGS", "GrowLancerCreateMinLevel", 200, ".\\DataServer.ini");
int g_iShowAllQueriesInDS = GetPrivateProfileInt("SETTINGS", "DisplayAllQueries", 1, ".\\DataServer.ini");
int g_iConnectStatSyncEnable = GetPrivateProfileInt("SETTINGS", "MembStatSync", 0, ".\\DataServer.ini");;
int g_DSBattleCoreEnable = GetPrivateProfileInt("SETTINGS", "DSBattleCore", 0, ".\\DataServer.ini");;

TCHAR g_ServerAddress[64];
TCHAR g_DBPort[8];
TCHAR g_UserID[64];
TCHAR g_Password[64];
TCHAR g_MuOnlineDB[64];
//TCHAR g_MeMuOnlineDNS[64];
TCHAR g_EventServerDB[64];
TCHAR g_RankingServerDB[64];

TCHAR g_AWHostPass[32];
TCHAR g_MapSvrFilePath[96];

TCHAR g_logsDir[64];
TCHAR g_logsEntryCount[2];
std::string g_logsEntry[10]; // up to 10 logs

int g_MaxPacketPerSec = 1000;
int g_MaxConnectionsPerIP = 16;

void UnhookSignals();
void HookSignals();

bool stopEvent = false;                                     ///< Setting it to true stops the server

typedef BYTE BYTE;

bool initDB()
{
	MySQL::Library_Init();

	DatabaseLoader loader("server.connectserver", DatabaseLoader::DATABASE_NONE);
	loader.ConnectInfo(g_ServerAddress, g_DBPort, g_UserID, g_Password, g_MuOnlineDB);
	loader.AddDatabase(ConnectDatabase, "Connect");

	if (!loader.Load())
		return false;

	sLog->outBasic("Started auth database connection pool.");
	return true;
}

/// Close the connection to the database
void StopDB()
{
	ConnectDatabase.Close();
	MySQL::Library_End();
}

void KeepDatabaseAliveHandler(std::weak_ptr<boost::asio::deadline_timer> dbPingTimerRef, int32 dbPingInterval, boost::system::error_code const& error)
{
	if (!error)
	{
		if (std::shared_ptr<boost::asio::deadline_timer> dbPingTimer = dbPingTimerRef.lock())
		{
			sLog->outBasic("Ping MySQL to keep connection alive");
			ConnectDatabase.KeepAlive();

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

bool InitDataServer()
{
	GetPrivateProfileString("SQL", "ServerAddress", "127.0.0.1", g_ServerAddress, sizeof(g_ServerAddress), ".\\DataServer.ini");
	GetPrivateProfileString("SQL", "Port", "3306", g_DBPort, sizeof(g_DBPort), ".\\DataServer.ini");
	GetPrivateProfileString("SQL", "User", "sa", g_UserID, sizeof(g_UserID), ".\\DataServer.ini");
	GetPrivateProfileString("SQL", "Pass", "sa", g_Password, sizeof(g_Password), ".\\DataServer.ini");
	GetPrivateProfileString("SQL", "MuOnlineDB", "MuOnline", g_MuOnlineDB, sizeof(g_MuOnlineDB), ".\\DataServer.ini");
	//GetPrivateProfileString("SQL", "MeMuOnlineDB", "MuOnline", g_MeMuOnlineDB, sizeof(g_MeMuOnlineDB), ".\\DataServer.ini");
	//GetPrivateProfileString("SQL", "EventDB", "MuEvent", g_EventServerDB, sizeof(g_EventServerDB), ".\\DataServer.ini");
	//GetPrivateProfileString("SQL", "RankingDB", "MuRanking", g_RankingServerDB, sizeof(g_RankingServerDB), ".\\DataServer.ini");
	return true;
}

void LoadLogConfig()
{
	GetPrivateProfileString("Logger", "LogDirectory", "logs", g_logsDir, sizeof(g_logsDir), ".\\DataServer.ini");
	GetPrivateProfileString("Logger", "LogEntries", "2", g_logsEntryCount, sizeof(g_logsEntryCount), ".\\DataServer.ini");
	int entryCount = atoi(g_logsEntryCount);
	std::vector<std::string> vecLogEntries;
	LPTSTR tempChars[10][128];
	for (int i = 0; i < entryCount; ++i)
	{
		GetPrivateProfileString("Logger", "LogEntry", "", tempChars[i][0], sizeof(tempChars[i][0]), ".\\DataServer.ini");
		vecLogEntries.push_back(tempChars[i][0]);
	}
	sLog->RegisterAppender<AppenderDB>();
	sLog->Initialize(nullptr, g_logsDir, vecLogEntries);
}

/// Launch the realm server
extern int main(int argc, char** argv)
{
	LoadLogConfig();

    ///- Command line parsing
    char const* options = ":s:";

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
		return 1;

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

#if defined (ACE_HAS_EVENT_POLL) || defined (ACE_HAS_DEV_POLL)
    ACE_Reactor::instance(new ACE_Reactor(new ACE_Dev_Poll_Reactor(ACE::max_handles(), 1), 1), true);
#else
    ACE_Reactor::instance(new ACE_Reactor(new ACE_TP_Reactor(), true), true);
#endif

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

    // server has started up successfully => enable async DB requests
    //DataDatabase.AllowAsyncTransactions();
	//EventsDatabase.AllowAsyncTransactions();
	//RankingDatabase.AllowAsyncTransactions();

////OLD CODE
// TODO: Place code here.

	_set_printf_count_output(TRUE);
	CreateDirectory("LOG", NULL);

	sLog->outBasic( "Initializing...");
	

	//GetPrivateProfileString(
	LoadAllowableIpList("./AllowedIPList.ini");
	GetPrivateProfileString("SETTINGS", "MapServerInfoPath", "..\\Data\\MapServerInfo.ini", g_MapSvrFilePath, sizeof(g_MapSvrFilePath), ".\\DataServer.ini");
	GetPrivateProfileString("SETTINGS", "WanIP", "127.0.0.1", szWANIP, 150, ".\\DataServer.ini");
	//std::memcpy(szWANIP, ValidateAndResolveIP(szWANIP), 15); // temp
	//g_MapServerManager.LoadMapData(g_MapSvrFilePath);
	//SendMessage(ghWnd, WM_TIMER, WM_LOG_PAINT, NULL);

	gObjServerInit();
	//IniteDataServer();
	IocpServerStart();

////OLD CODE END

    // maximum counter for next ping
    //uint32 numLoops = (10 * (MINUTE * 1000000 / 100000));
    uint32 loopCounter = 0;

	// TODO - Make sure working from TC
	ioContext->run();

#ifndef WIN32
    detachDaemon();
#endif
    ///- Wait for termination signal
    while (!stopEvent)
    {
        // dont move this outside the loop, the reactor will modify it
        ACE_Time_Value interval(0, 100000);

        if (ACE_Reactor::instance()->run_reactor_event_loop(interval) == -1)
            { break; }

#ifdef WIN32
        if (m_ServiceStatus == 0) { stopEvent = true; }
        while (m_ServiceStatus == 2) { Sleep(1000); }
#endif
    }

    ///- Remove signal handling before leaving
    UnhookSignals();

    sLog->outBasic("Halting process...");

	signals.cancel();

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
