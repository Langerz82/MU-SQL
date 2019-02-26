/// \addtogroup Connect Server Daemon
/// @{
/// \file


#include "StdAfx.h"
#include "Main.h"
#include "Logging/Log.h"
#include "Asio/IoContext.h"
#include "IOCP.h"
#include "ServerData.h"

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

typedef BYTE BYTE;



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
		mSleep(50);

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
