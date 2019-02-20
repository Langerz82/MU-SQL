
// TODO - Make sure program signalling works.

/// \addtogroup realmd Realm Daemon
/// @{
/// \file

#include "StdAfx.h"
#include "Main.h"
#include "Logging/Log.h"
#include "Asio/IoContext.h"
#include "Config/iniReader/INIReaderImp.cpp"
#include "IOCP.h"
#include "ServerData.h"

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
#include <ace/Reactor.h>

#include <boost/asio/signal_set.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

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

INIReaderImp ini(".\\ConnectServer.ini");

WORD g_ConnectServerPort;
WORD g_ConnectServerUDP;
DWORD g_MaxConnectionsPerIP = 1000;
DWORD g_MaxPacketPerSec = 16;
uint16 g_UDPPort;
WORD g_FTPPort;
std::string g_HostURL;
std::string g_FTPLogin;
std::string g_FTPPassword;
std::string g_VersionFile;
std::string g_ClientVersion;
std::string g_WhiteListIP;
BOOL gDisconnectHackUser = FALSE;
DWORD g_MachineIDConnectionLimitPerGroup = 10;

// GLobals thats whats up.
std::string szWANIP;
int g_dwMaxServerGroups = ini.GetInteger("SETTINGS", "MAX_SERVER", 10) * MAX_SERVER_TYPE;

BOOL g_DSMode = ini.GetInteger("SETTINGS", "DataServerOnlyMode", 0);
BOOL g_UseJoinServer = ini.GetInteger("SETTINGS", "UseJoinServer", 1);

std::string g_AWHostPass;
std::string g_MapSvrFilePath;

std::string g_logsDir;
std::string g_logsEntryCount;
std::string g_logsEntry[10]; // up to 10 logs

void UnhookSignals();
void HookSignals();

bool stopEvent = false;                                     ///< Setting it to true stops the server

typedef BYTE BYTE;



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
	return true;
}

void LoadLogConfig()
{


	g_logsDir = ini.GetString("Logger", "LogDirectory", "logs").c_str());

	std::vector<std::string> vecLogEntries;
	std::vector<std::string> vecLogEntryNames;
	int i = 0;
	while (true)
	{
		TCHAR tempChars[128];
		std::string temp = "";
		std::string logEntry = StringFormat("LogEntry%d", i++);
		tempChars = ini.GetString("Logger", logEntry.c_str(), "").c_str();
		//std::cout << tempChars << std::endl;
		temp.assign(tempChars, sizeof(tempChars));
		if (strcmp(temp.c_str(), "") == 0)
			break;
		vecLogEntries.push_back(temp);
		vecLogEntryNames.push_back(logEntry);
	}
	std::vector<std::string> vecAppendEntries;
	std::vector<std::string> vecAppendEntryNames;
	i = 0;
	while (true)
	{
		TCHAR tempChars[128];
		std::string temp = "";
		std::string appendEntry = StringFormat("AppendEntry%d", i++);
		tempChars = ini.GetString("Appender", appendEntry.c_str(), "").c_str();
		//std::cout << tempChars << std::endl;
		temp.assign(tempChars, sizeof(tempChars));
		if (strcmp(temp.c_str(), "") == 0)
			break;
		vecAppendEntries.push_back(temp);
		vecAppendEntryNames.push_back(appendEntry);
	}
	sLog->Initialize(nullptr, ".", vecLogEntries, vecAppendEntries, vecLogEntryNames, vecAppendEntryNames);
}

/// Launch the realm server
extern int main(int argc, char** argv)
{
	LoadLogConfig();

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

	std::shared_ptr<Asio::IoContext> ioContext = std::make_shared<Asio::IoContext>();

	// Set signal handlers
	boost::asio::signal_set signals(*ioContext, SIGINT, SIGTERM);
#if WIN32
	signals.add(SIGBREAK);
#endif
	signals.async_wait(std::bind(&SignalHandler, std::weak_ptr<Asio::IoContext>(ioContext), std::placeholders::_1, std::placeholders::_2));

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


	m_ServerData.LoadServerFile("IGC_ServerList.xml");
	m_ServerData.LoadNewsFile("News.dat");

////OLD CODE
// TODO: Place code here.

	//_set_printf_count_output(TRUE);
    const char* path = _filePath.c_str();
    fs::path dir(path);
    if(fs::create_directory("LOG"))
    {
        std::cerr<< "Directory Created: "<<_filePath<<std::endl;
    }

	sLog->outBasic( "Initializing...");


	//GetPrivateProfileString(
	LoadAllowableIpList("./AllowedIPList.ini");
	WORD g_JoinServerListPort = ini.getInt("SETTINGS", "TCP_PORT", 44405);
	szWANIP = ini.getString("SETTINGS", "WanIP", "127.0.0.1");
	//std::memcpy(szWANIP, ValidateAndResolveIP(szWANIP), 15); // temp
	//g_MapServerManager.LoadMapData(g_MapSvrFilePath);
	//SendMessage(ghWnd, WM_TIMER, WM_LOG_PAINT, NULL);

	gObjServerInit();
	//IniteDataServer();
	IOCP.GiocpInit();
	sLog->outBasic("CreateListenSocket - Started.");
	IOCP.CreateListenSocket(g_JoinServerListPort, szWANIP);
	sLog->outBasic("CreateListenSocket - Finished.");
////OLD CODE END

    // maximum counter for next ping
    //uint32 numLoops = (10 * (MINUTE * 1000000 / 100000));
    uint32 loopCounter = 0;

	// TODO - Make sure working from TC
	sLog->outBasic("ioContext->run()");
	//ioContext->run();
	sLog->outBasic("ioContext->run() - finished");
//#ifndef WIN32
//    detachDaemon();
//#endif
	sLog->outBasic("Entering loop.");
    ///- Wait for termination signal
    while (true)
    {
        // dont move this outside the loop, the reactor will modify it


        //if (ACE_Reactor::instance()->run_reactor_event_loop(interval) == -1)
        //    { break; }

		CIOCP::ProcessEvents();

		sLog->outBasic("Server Farting.");

#ifdef WIN32
        //if (m_ServiceStatus == 0) { stopEvent = true; }
        //while (m_ServiceStatus == 2) { Sleep(1000); }
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
