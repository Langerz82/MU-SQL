////////////////////////////////////////////////////////////////////////////////
// Authorization.h
#ifndef AUTHORIZATION_H
#define AUTHORIZATION_H

#include "wsJoinServerCli.h"

#define MAX_AUTH_SERVERS 2
#define AUTH_PORT 52854 // IGC
#define AUTH_PROTOCOL_VERSION 16 // IGC
#define GAMESERVER_AUTH_VERSION 10000

#define LICENSE_PERMANENT 1

enum ERROR_CODE
{
	RSA_INIT_FAIL = 0,
	CREATE_SOCKET_FAIL = 10000,
	CONNECTION_FAILED = 10001,
	RSA_LOGIN_FAIL = 20000,
	INCORRECT_GAMESERVER_AUTH_PROTOCOL_VERSION = 30000,
	UNKNOWN_GAMESERVER_VERSION = 31000,
	INCORRECT_GAMESERVER_VERSION_IN_DB = 32000,
	NO_LICENSE_ENTRY_IN_DB = 40000,
	NO_LICENSE_FOR_THIS_VERSION = 40001,
	NO_LICENSE_IN_REGISTRY = 40002,
	LICENSE_BANNED = 41000,
	LICENSE_EXPIRED = 42000,
	LICENSE_INVALID_KEY = 43000,
	LICENSE_GAMESERVER_LIMIT_REACHED = 44000,
	FUNCTION_POINTER_SET_ERROR = 50000,
	AUTH_PING_THREAD_FAIL = 51000,
	AUTH_CONN_THREAD_FAIL = 51100,
	WINLICENSE_INTEGRITY_CHECK_ERROR = 52000,
	WINLICENSE_PROTECTION_CHECK_ERROR = 52001,
	AUTH_MAIN_KEY_CHECK_ERROR = 53000,
	ACTIVATION_ERROR_NO_LICENSE_DATA = 60000,
	ACTIVATION_ERROR_WINLICENSE_FAIL = 60001,
	ACTIVATION_ERROR_GAMESERVER_VERSION_INCORRECT_IN_DB = 60002,
};



enum GAMESERVER_SEASONS
{
	SERVER_UNKNOWN = 0xFF,
	SERVER_SEASON6_EP3 = 0x00,
	SERVER_EX700 = 0x10,
	SERVER_EX700_PLUS = 0x20,
	SERVER_SEASON8_EP3 = 0x30,
	SERVER_SEASON9 = 0x40
};

enum GAMESERVER_TYPES
{
	GAMESERVER_NORMAL = 0x00,
	GAMESERVER_SIEGE = 0x10,
	GAMESERVER_ARCA = 0x20,
	GAMESERVER_MARKET = 0x30
};

enum AUTH_NUMBERS
{
	AUTH_NOT_INITIALIZED = 0xFF,
	AUTH_MAIN = 0x00,
	AUTH_BACKUP = 0x01
};

#pragma pack (1)

struct PMSG_CONNRESULT
{
	BYTE HEAD;
	WORD SIZE;
	DWORD SEED;
};

struct PMSG_HANDSHAKE
{
	BYTE HEAD;
	WORD SIZE;
	BYTE PROTOCOLVERSION;
	int eSize;
	int nSize;
};

struct PMSG_LICENSE
{
	BYTE HEAD;
	WORD SIZE;
	BYTE PROTOCOLVERSION;
	BYTE SEASON;
	DWORD GSVERSION;
	DWORD GSTYPE;
	DWORD GSPORT;
	WORD GSCODE;
	char Key[16 + (8*17) +1];
	char HWID[100];
};

struct PMSG_ACTIVATE
{
	BYTE HEAD;
	WORD SIZE;
	BYTE PROTOCOLVERSION;
	DWORD GSVERSION;
	BYTE KEYTABLE[20];
	DWORD Status;
	DWORD Addons;
	int Days;
};

struct PMSG_AUTH
{
	BYTE HEAD;
	WORD SIZE;
	BYTE PROTOCOLVERSION;
	BYTE SEASON;
	BYTE KEYINDEX;
	BYTE KEY;
	int Days;
};

struct PMSG_SENDINFO
{
	BYTE HEAD;
	WORD SIZE;
	char CustomerName[50];
	char ServerName[30];
	bool S6;
	bool Ex700;
	bool Ex702;
	bool SE;
	bool Launcher;
	char ExpiryDate[50];
};

struct PMSG_LICREQ
{
	BYTE HEAD;
	WORD SIZE;
	BYTE PROTOCOLVERSION;
	DWORD GSVERSION;
	char HWID[100];
	WORD Port;
};

/*struct PMSG_LICANS
{
	BYTE HEAD;
	WORD SIZE;
	BYTE PROTOCOLVERSION;
	DWORD GSVERSION;
	DWORD License;
	DWORD Addons;
	char Key[16 + (8*17) +1];
	char CustomerName[200];
	char ServerName[100];
	WORD Port;
};*/

struct PMSG_LICANS
{
	 BYTE HEAD;
	 WORD SIZE;
	 BYTE PROTOCOLVERSION;
	 DWORD GSVERSION;
	 DWORD License;
	 DWORD Addons;
	 char Key[16 + (8*17) +1];
	 char CustomerName[200];
	 char ServerName[100];
	 WORD Port;
};
struct PMSG_MESSAGE
{
	BYTE HEAD;
	WORD SIZE;
	BYTE PROTOCOLVERSION;
	DWORD Message;
	char Text[200];
};

struct PMSG_GETLIC
{
	BYTE HEAD;
	WORD SIZE;
};

#pragma pack ()

static struct AUTH_SERVERS
{
	char IP[32];
} g_AuthServers[MAX_AUTH_SERVERS] = {

	"158.69.204.41", //"87.98.246.104",
	"158.69.204.41"  //"91.234.146.69",
};

bool CheckAuthThread();

class CAuthSystem
{
public:

	CAuthSystem();
	~CAuthSystem();

	friend void AuthClientMsgProc(WPARAM wParam, LPARAM lParam);
	friend class TServerInfoDisplayer;
	friend void gObjSecondProc();
	friend int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
	friend bool CheckAuthThread();
	

	static void AuthProtocolCore(BYTE protoNum, unsigned char* aRecv, int aLen);
	void RecvConnResult(PMSG_CONNRESULT *pMsg);
	void Activate();
	bool ConnectToAuth();
	bool Reconnect();
	void AuthServerLogin();
	void SendLicenseInfo(LPBYTE aRecv);
	void ValidateServer(PMSG_ACTIVATE *pMsg);
	void PeriodCheck(PMSG_ACTIVATE *pMsg);
	void Ping(PMSG_AUTH *pMsg);
	void MessageHandle(PMSG_MESSAGE *pMsg);
	void MessageHandle(DWORD Message);
	void SendMessage(DWORD Message);
	void ActivationAnswer(PMSG_LICANS *pMsg);
	void GetLicenseInfo();
	void RecvLicenseInfo(PMSG_SENDINFO *pMsg);
	void ParseErrorCode(DWORD Message);

	void Test();

	void SetExpiryTime(int Expiry) { this->ExpiryTime = Expiry; }
	int GetExpiryTime() { return this->ExpiryTime; }

	bool IsLongDisconnect()
	{ 
		//VM_TIGER_LONDON_START 

		if(this->ReconnectRetry >= 60)
		{ 
			return true;
		} 

		else 
		{ 
			return false;
		} 

		//VM_TIGER_LONDON_END
	}

	DWORD GetSeason();
	DWORD GetGSType();

	void SetAddons(DWORD Addons);

	bool CreateRSAKey(int Seed);
	bool MakeAuthKey(LPBYTE lpRecv);

	void __fastcall LicenseParse(BYTE Status, DWORD Addons, int &Valid);

	static void AuthConnectionProcess(void * lpThread);

	void SendData(LPBYTE lpMsg, DWORD dwSize, BOOL Encrypt);

private:
	static void AuthThread(void * lpThread);

	HANDLE hConnThread;
	DWORD dwConnThreadID;
	int m_FailCount;
	DWORD AuthDCTime;
	int ExpiryTime;
	int ReconnectRetry;

	CRsa * m_RSA;
	CPublicRsaKey * m_AuthKey;
	wsJoinServerCli m_AuthServer;
	BYTE m_btAuthServerID;
	BYTE m_KeyTable[20];
};

void ParseHardwareID(char *HWID);

extern CAuthSystem g_AuthSys;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

