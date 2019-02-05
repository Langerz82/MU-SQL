

/// \addtogroup realmd Realm Daemon
/// @{
/// \file


#include "Common.h"

// old includes

#include "Main.h"
#include "old/Protocols/DSprotocol.h"
#include "old/Protocols/Sprotocol.h"

#include "old/Giocp.h"
#include "old/Protocols/EDSprotocol.h"
#include "old/MapServerManager.h"
#include "old/Item/ItemSerial.h"

#include "Database/DatabaseEnv.h"
#include "Config/Config.h"
#include "Log.h"

#include "SystemConfig.h"
#include "revision.h"
#include "Util.h"
#include <openssl/opensslv.h>
#include <openssl/crypto.h>

#include <ace/Get_Opt.h>
#include <ace/Dev_Poll_Reactor.h>
#include <ace/TP_Reactor.h>
#include <ace/ACE.h>
#include <ace/Acceptor.h>
#include <ace/SOCK_Acceptor.h>

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

// Defines
#define MAX_ACCOUNT_LEN 10
#define MAX_TYPE_PLAYER 8
#define MAX_NUMBER_MAP 101
#define CHECK_LIMIT(value, limit) (((value)< 0)?FALSE:((value)> limit-1)?FALSE:TRUE)

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

TCHAR g_MuOnlineDNS[64];
TCHAR g_MeMuOnlineDNS[64];
TCHAR g_EventServerDNS[64];
TCHAR g_RankingServerDNS[64];
TCHAR g_UserID[64];
TCHAR g_Password[64];
TCHAR g_ServerName[64];
TCHAR g_AWHostPass[32];
TCHAR g_MapSvrFilePath[96];



CDataServerProtocol m_DSProtocol;
CLoginServerProtocol m_JSProtocol;
CExDataServerProtocol m_EXDSProtocol;

bool StartDB();
void UnhookSignals();
void HookSignals();

bool stopEvent = false;                                     ///< Setting it to true stops the server

DatabaseType DataDatabase;
DatabaseType EventsDatabase;
DatabaseType RankingDatabase;

typedef unsigned char BYTE;



/// Print out the usage string for this program on the console.
void usage(const char* prog)
{
    sLog.outString("Usage: \n %s [<options>]\n"
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

/// Launch the realm server
extern int main(int argc, char** argv)
{
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
                    sLog.outError("Runtime-Error: -%c unsupported argument %s", cmd_opts.opt_opt(), mode);
                    usage(argv[0]);
                    Log::WaitBeforeContinueIfNeed();
                    return 1;
                }
                break;
            }
            case ':':
                sLog.outError("Runtime-Error: -%c option requires an input argument", cmd_opts.opt_opt());
                usage(argv[0]);
                Log::WaitBeforeContinueIfNeed();
                return 1;
            default:
                sLog.outError("Runtime-Error: bad format of commandline arguments");
                usage(argv[0]);
                Log::WaitBeforeContinueIfNeed();
                return 1;
        }
    }

#ifdef WIN32                                                // windows service command need execute before config read
    switch (serviceDaemonMode)
    {
        case 'i':
            if (WinServiceInstall())
                { sLog.outString("Installing service"); }
            return 1;
        case 'u':
            if (WinServiceUninstall())
                { sLog.outString("Uninstalling service"); }
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

    sLog.Initialize();

    sLog.outString("%s [realm-daemon]", REVISION_NR);
    sLog.outString("<Ctrl-C> to stop.\n");

    DETAIL_LOG("%s (Library: %s)", OPENSSL_VERSION_TEXT, SSLeay_version(SSLEAY_VERSION));
    if (SSLeay() < 0x009080bfL)
    {
        DETAIL_LOG("WARNING: Outdated version of OpenSSL lib. Logins to server may not work!");
        DETAIL_LOG("WARNING: Minimal required version [OpenSSL 0.9.8k]");
    }

    DETAIL_LOG("Using ACE: %s", ACE_VERSION);

#if defined (ACE_HAS_EVENT_POLL) || defined (ACE_HAS_DEV_POLL)
    ACE_Reactor::instance(new ACE_Reactor(new ACE_Dev_Poll_Reactor(ACE::max_handles(), 1), 1), true);
#else
    ACE_Reactor::instance(new ACE_Reactor(new ACE_TP_Reactor(), true), true);
#endif

    sLog.outBasic("Max allowed open files is %d", ACE::max_handles());

    /// MuSQLServer PID file creation
    std::string pidfile = "";
    if (!pidfile.empty())
    {
        //uint32 pid = CreatePIDFile(pidfile);
		int32 pid = 0; // stub
        if (!pid)
        {
            sLog.outError("Can not create PID file %s.\n", pidfile.c_str());
            Log::WaitBeforeContinueIfNeed();
            return 1;
        }

        sLog.outString("Daemon PID: %u\n", pid);
    }

    ///- Initialize the database connection
    if (!StartDB())
    {
        Log::WaitBeforeContinueIfNeed();
        return 1;
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
                    sLog.outError("Processors marked in UseProcessors bitmask (hex) %x not accessible for MuSQLServer. Accessible processors bitmask (hex): %x", Aff, appAff);
                }
                else
                {
                    if (SetProcessAffinityMask(hProcess, curAff))
                        { sLog.outString("Using processors (bitmask, hex): %x", curAff); }
                    else
                        { sLog.outError("Can't set used processors (hex): %x", curAff); }
                }
            }
            sLog.outString();
        }

        bool Prio = false;

        if (Prio)
        {
            if (SetPriorityClass(hProcess, HIGH_PRIORITY_CLASS))
                { sLog.outString("MuSQLServer process priority class set to HIGH"); }
            else
                { sLog.outError("Can't set MuSQLServer process priority class."); }
            sLog.outString();
        }
    }
#endif

    // server has started up successfully => enable async DB requests
    DataDatabase.AllowAsyncTransactions();
	EventsDatabase.AllowAsyncTransactions();
	RankingDatabase.AllowAsyncTransactions();

////OLD CODE
// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	_set_printf_count_output(TRUE);
	CreateDirectory("LOG", NULL);

	sLog.outBasic( "Initializing...");
	

	//GetPrivateProfileString(
	LoadAllowableIpList("./AllowedIPList.ini");
	GetPrivateProfileString("SETTINGS", "MapServerInfoPath", "..\\Data\\MapServerInfo.ini", g_MapSvrFilePath, sizeof(g_MapSvrFilePath), ".\\DataServer.ini");
	GetPrivateProfileString("SETTINGS", "WanIP", "127.0.0.1", szWANIP, 150, ".\\DataServer.ini");
	//std::memcpy(szWANIP, ValidateAndResolveIP(szWANIP), 15); // temp
	g_MapServerManager.LoadMapData(g_MapSvrFilePath);
	//SendMessage(ghWnd, WM_TIMER, WM_LOG_PAINT, NULL);

	gObjServerInit();
	//IniteDataServer();
	IocpServerStart();

////OLD CODE END

    // maximum counter for next ping
    uint32 numLoops = (10 * (MINUTE * 1000000 / 100000));
    uint32 loopCounter = 0;

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

        if ((++loopCounter) == numLoops)
        {
            loopCounter = 0;
            DETAIL_LOG("Ping MySQL to keep connection alive");
            DataDatabase.Ping();
			EventsDatabase.Ping();
			RankingDatabase.Ping();
        }
#ifdef WIN32
        if (m_ServiceStatus == 0) { stopEvent = true; }
        while (m_ServiceStatus == 2) { Sleep(1000); }
#endif
    }

    ///- Wait for the delay thread to exit
    DataDatabase.HaltDelayThread();
	EventsDatabase.HaltDelayThread();
	RankingDatabase.HaltDelayThread();

    ///- Remove signal handling before leaving
    UnhookSignals();

    sLog.outString("Halting process...");
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

/// Initialize connection to the database
bool StartDB()
{
	std::string dbConn1 = "";
	std::string dbConn2 = "";
	std::string dbConn3 = "";

    if (dbConn1.empty())
    {
        sLog.outError("Database not specified");
        return false;
    }
	if (dbConn2.empty())
	{
		sLog.outError("Database not specified");
		return false;
	}
	if (dbConn3.empty())
	{
		sLog.outError("Database not specified");
		return false;
	}

    sLog.outString("Database total connections: %i", 1 + 1);

    if (!DataDatabase.Initialize(dbConn1.c_str()))
    {
        sLog.outError("Can not connect to Data database");
        return false;
    }
	if (!EventsDatabase.Initialize(dbConn2.c_str()))
	{
		sLog.outError("Can not connect to Events database");
		return false;
	}
	if (!RankingDatabase.Initialize(dbConn3.c_str()))
	{
		sLog.outError("Can not connect to Ranking database");
		return false;
	}

    return true;
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
