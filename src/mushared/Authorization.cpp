////////////////////////////////////////////////////////////////////////////////
// Authorization.cpp
#include "StdAfx.h"
#include "Authorization.h"
#include "user.h"
#include "GameMain.h"
#include "TLog.h"
#include "configread.h"
#include "GameServer.h"
#include "resource.h"
#include "OfflineLevelling.h"

HANDLE hThread;
CAuthSystem g_AuthSys;

BYTE KeyTable [] = { 0xF3, 0xFB, 0xA9, 0xE0, 0xCC, 0x11, 0x26, 0x75, 0x9A, 0x8A, 0x06, 0x8C, 0x66, 0x34, 0xB3, 0x27, 0xBD, 0x7F, 0x57, 0x7C }; // Main Auth<->GS KEY

CAuthSystem::CAuthSystem() : m_AuthServer(true, true)
{
	this->m_AuthServer.SetProtocolCore(CAuthSystem::AuthProtocolCore);
	this->m_btAuthServerID = AUTH_NOT_INITIALIZED;
	this->m_RSA = NULL;
	this->AuthDCTime = 0;
	this->ReconnectRetry = 0;
	memset(&this->m_KeyTable, 0x00, 20);
};

CAuthSystem::~CAuthSystem()
{
	delete m_AuthKey;
	delete m_RSA;
}


void CAuthSystem::AuthProtocolCore(BYTE protoNum, unsigned char* aRecv, int aLen)
{
	switch(protoNum)
	{
		case 0x9A:
			{
				g_AuthSys.SendLicenseInfo(aRecv);
			}
			break;

		case 0x36:
			{
				g_AuthSys.ValidateServer((PMSG_ACTIVATE*)aRecv);
			}
			break;

		case 0xA7:
			{
				g_AuthSys.Ping((PMSG_AUTH *)aRecv);
			}
			break;

		case 0x71:
			{
				g_AuthSys.MessageHandle((PMSG_MESSAGE *)aRecv);
			}
			break;

		case 0xEA:
			{
				g_AuthSys.RecvLicenseInfo((PMSG_SENDINFO *)aRecv);
			}
			break;
			
		case 0xE7:
			{
				g_AuthSys.ActivationAnswer((PMSG_LICANS *)aRecv);
			}
			break;

		case 0xF0:
			{
				g_AuthSys.RecvConnResult((PMSG_CONNRESULT *)aRecv);
			}
			break;
	}
}

bool CAuthSystem::ConnectToAuth()
{
	if(this->m_AuthServer.CreateSocket(ghWnd) == FALSE)
	{
		this->MessageHandle(CREATE_SOCKET_FAIL);
		return false;
	}

	for(int i=0;i<MAX_AUTH_SERVERS;i++)
	{
		if(this->m_AuthServer.Connect(g_AuthServers[i].IP, AUTH_PORT, WM_GM_AUTH_SERVER_MSG_PROC) == TRUE)
		{
			this->m_btAuthServerID = i;
			g_Log.Add("[Auth] Connected to Auth (%d)", this->m_btAuthServerID+1);
			return true;
		}

		else
		{
			this->m_AuthServer.CreateSocket(ghWnd);
		}
	}

	this->MessageHandle(CONNECTION_FAILED);

	return false;
}

bool CAuthSystem::Reconnect()
{	
	for(int i=0;i<MAX_AUTH_SERVERS;i++)
	{
		if(this->m_AuthServer.Connect(g_AuthServers[i].IP, AUTH_PORT, WM_GM_AUTH_SERVER_MSG_PROC) == TRUE)
		{
			this->m_btAuthServerID = i;
			g_Log.Add("[Auth] Reconnected to Auth (%d)", this->m_btAuthServerID+1);
			return true;
		}

		else
		{
			this->m_AuthServer.CreateSocket(ghWnd);
		}
	}
	g_Log.Add("[Auth] Unable to reconnect -> waiting for next reconnection (30s)");
	return false;
	
}

void CAuthSystem::RecvConnResult(PMSG_CONNRESULT *pMsg)
{
	//VM_DOLPHIN_RED_START

	if(pMsg->SEED == NULL)
	{
		pMsg->SEED = time(NULL);
	}

	if(this->CreateRSAKey(pMsg->SEED) == FALSE)
	{
		return;
	}

	else
	{
		g_Log.Add("[Auth] Module Initializing Completed");
		this->AuthServerLogin();
	}

	//VM_DOLPHIN_RED_END
}

void CAuthSystem::ValidateServer(PMSG_ACTIVATE *pMsg)
{
	//VM_DOLPHIN_RED_START

	if(this->m_KeyTable[0] == 0 && this->m_KeyTable[10] == 0 &&
		this->m_KeyTable[15] == 0) // First connect
	{
		memcpy(&this->m_KeyTable, pMsg->KEYTABLE, 20);

		if(memcmp(&this->m_KeyTable, KeyTable, 20) == 0 && pMsg->Status == TRUE)
		{
			g_Log.AddC(TColor::Green, "[Auth] Status OK");

			this->SetAddons(pMsg->Addons);
			this->SetExpiryTime(pMsg->Days);
			this->AuthDCTime = 0;
/*
			if(pMsg->Days <= 0 && pMsg->Days != LICENSE_PERMANENT)
			{
				char Text[200];
				wsprintf(Text, "Your license expired and will stop being supported in %d day(s). \nPlease to extend subscription by making appropriate payment.", 7-abs(this->GetExpiryTime()));
				g_Log.MsgBox(MB_OK | MB_ICONWARNING, "Warning", Text);
				g_Log.AddC(TColor::Red, "License is EXPIRED, please Renew (Days left: %d)", 7-abs(this->GetExpiryTime()) );
			}
*/
			AllServerStart();
		}
	}

	else // Reconnect
	{
		if(memcmp(&this->m_KeyTable, KeyTable, 20) == 0 && pMsg->Status == TRUE)
		{
			g_Log.Add("[Auth] License re-Sync OK");
			this->AuthDCTime = 0;
			
		}

		else
		{
			ExitProcess(0x10000);
			return;
		}
	}

	//VM_DOLPHIN_RED_END
}
void CAuthSystem::AuthServerLogin()
{
	//VM_DOLPHIN_RED_START
	BYTE Buff[2048];

	if ( this->m_RSA == NULL )
	{
		this->SendMessage(RSA_INIT_FAIL);
		this->MessageHandle(RSA_INIT_FAIL);
		return;
	}

	CPublicRsaKey * Key = this->m_RSA->GetPublicRsaKey();

	if(Key == NULL)
	{
		this->SendMessage(RSA_LOGIN_FAIL);
		this->MessageHandle(RSA_LOGIN_FAIL);
		return;
	}

	PMSG_HANDSHAKE p;
	p.HEAD = 0x00;
	p.eSize = Key->m_ESize;
	p.nSize = Key->m_NSize;
	p.PROTOCOLVERSION = AUTH_PROTOCOL_VERSION;
	p.SIZE= sizeof(p) + Key->m_ESize + Key->m_NSize;

	memcpy(&Buff, &p, sizeof(p));
	memcpy(&Buff[sizeof(p)], Key->m_EBuff, Key->m_ESize);
	memcpy(&Buff[sizeof(p)+Key->m_ESize], Key->m_NBuff, Key->m_NSize);

	// UNSECURED PACKET
	this->SendData((LPBYTE)&Buff, p.SIZE, FALSE);

	g_Log.Add("[Auth] Logging to Auth ID: %d", this->m_btAuthServerID+1);

	//VM_DOLPHIN_RED_END
}

void CAuthSystem::SendLicenseInfo(LPBYTE aRecv)
{
#ifdef GS_AUTHORITY
	PMSG_HANDSHAKE * lpMsg = (PMSG_HANDSHAKE *)aRecv;

	//VM_DOLPHIN_RED_START

	if(lpMsg->PROTOCOLVERSION != AUTH_PROTOCOL_VERSION)
	{
		g_Log.Add("[Auth] Protocol Version MISMATCH (Auth:%d Client:%d)", lpMsg->PROTOCOLVERSION, AUTH_PROTOCOL_VERSION);
		this->SendMessage(INCORRECT_GAMESERVER_AUTH_PROTOCOL_VERSION);
		this->MessageHandle(INCORRECT_GAMESERVER_AUTH_PROTOCOL_VERSION);
		return;
	}

	if(this->MakeAuthKey(aRecv) == false)
	{
		g_Log.AddC(TColor::Red, "[Auth] Login failed - cannot create Authorization Key");
		return;
	}

	else
	{
		g_Log.Add("[Auth] Login Success, checking license data... (Season Code:%X) (GameServer Type:%X)", this->GetSeason(), this->GetGSType());
	}

	if(WLRegGetStatus(NULL) == wlIsRegistered)
	{

		PMSG_LICENSE pMsg;
		pMsg.HEAD = 0x79;
		pMsg.SIZE = sizeof(pMsg);
		pMsg.PROTOCOLVERSION = AUTH_PROTOCOL_VERSION;
		pMsg.GSVERSION = GAMESERVER_AUTH_VERSION;
		pMsg.SEASON = this->GetSeason();
		pMsg.GSTYPE = this->GetGSType();
		pMsg.GSPORT = g_ConfigRead.server.GetGameServerPort();
		pMsg.GSCODE = g_ConfigRead.server.GetGameServerCode();

		WLRegGetDynSmartKey(pMsg.Key);
		WLHardwareGetID(pMsg.HWID);
		ParseHardwareID(pMsg.HWID);

		this->SendData((LPBYTE)&pMsg, pMsg.SIZE, TRUE);
	}

	else
	{
		this->Activate();
	}

	//VM_DOLPHIN_RED_END

#endif
}

void CAuthSystem::Ping(PMSG_AUTH *pMsg)
{
	if(this->GetSeason() != pMsg->SEASON)
	{
		g_Log.AddC(TColor::Red, "[Auth] Internal Error - Season code is wrong!");
		return;
	}

	if(pMsg->Days != this->GetExpiryTime() )
	{
		this->SetExpiryTime(pMsg->Days);
	}

	if(pMsg->KEY != this->m_KeyTable[pMsg->KEYINDEX])
	{
		ExitProcess(20000);
	}
}

void CAuthSystem::Activate()
{
#ifdef GS_AUTHORITY
	//VM_DOLPHIN_RED_START
	PMSG_LICREQ pMsg;

	pMsg.HEAD = 0xA4;
	pMsg.SIZE = sizeof(pMsg);
	pMsg.PROTOCOLVERSION = AUTH_PROTOCOL_VERSION;
	pMsg.Port = g_ConfigRead.server.GetGameServerPort();
	pMsg.GSVERSION = GAMESERVER_AUTH_VERSION;
	WLHardwareGetID(pMsg.HWID);
	ParseHardwareID(pMsg.HWID);

	this->SendData((LPBYTE)&pMsg, pMsg.SIZE, TRUE);
	//VM_DOLPHIN_RED_END
#endif
}

void CAuthSystem::MessageHandle(PMSG_MESSAGE * pMsg)
{
#ifdef GS_AUTHORITY
	//VM_TIGER_LONDON_START
	char msg[600];
	char title[200];
	char HWID[100];
	WLHardwareGetID(HWID);
	if(pMsg->Text[0] != 0x00)
	{
		sprintf(msg, "ERROR - CONTACT WITH EMU SUPPORT (CODE: %d)\n(HWID: %s)\n\nINFO: %s", pMsg->Message, HWID, pMsg->Text);
	}

	else
	{
		sprintf(msg, "ERROR - CONTACT WITH EMU SUPPORT (CODE: %d) (HWID:%s)", pMsg->Message, HWID);
	}

	sprintf(title, "ERROR %d", pMsg->Message);
		
	MessageBox(0, msg, title, MB_OK|MB_ICONERROR|MB_TOPMOST);
	ExitProcess(1);
	this->ParseErrorCode(pMsg->Message);
	//VM_TIGER_LONDON_END
#endif
}

void CAuthSystem::MessageHandle(DWORD Message)
{
#ifdef GS_AUTHORITY
	//VM_TIGER_LONDON_START
	char msg[600];
	char title[200];
	char HWID[100];
	WLHardwareGetID(HWID);
	sprintf(msg, "ERROR - CONTACT WITH EMU SUPPORT (CODE: %d) (HWID:%s)", Message, HWID);
	sprintf(title, "ERROR %d", Message);
		
	MessageBox(0, msg, title, MB_OK|MB_ICONERROR|MB_TOPMOST);
	this->ParseErrorCode(Message);
	//VM_TIGER_LONDON_END
#endif
}

void CAuthSystem::SendMessage(DWORD Message)
{
	//ENCODE_START
	PMSG_MESSAGE pMsg;

	pMsg.HEAD = 0x71;
	pMsg.SIZE = sizeof(pMsg);
	pMsg.PROTOCOLVERSION = AUTH_PROTOCOL_VERSION;
	pMsg.Message = Message;
	memset(pMsg.Text, 0x00, sizeof(pMsg.Text));

	this->SendData((LPBYTE)&pMsg, pMsg.SIZE, TRUE);
	//ENCODE_END
}

void CAuthSystem::ActivationAnswer(PMSG_LICANS * pMsg)
{
#ifdef GS_AUTHORITY
	//VM_TIGER_LONDON_START

	if(WLRegSmartKeyCheck(pMsg->CustomerName, pMsg->ServerName, NULL, pMsg->Key) == TRUE)
	{
		WLRegSmartKeyInstallToRegistry(pMsg->CustomerName, pMsg->ServerName, NULL, pMsg->Key);
		
		MessageBox(0, "Activation Completed: Click OK to close and run Data Server first.", "Success", MB_OK|MB_ICONINFORMATION|MB_TOPMOST);
		//ExitProcess(0);
	}

	else
	{
		this->SendMessage(ACTIVATION_ERROR_WINLICENSE_FAIL);
		this->MessageHandle(ACTIVATION_ERROR_WINLICENSE_FAIL);
		return;

	}

	//VM_TIGER_LONDON_END
#endif

}

void CAuthSystem::AuthConnectionProcess(void *lpThread)
{
	CAuthSystem * pAuth = (CAuthSystem *)(lpThread);

	while ( 1 )
	{
		if ( pAuth->m_btAuthServerID == AUTH_NOT_INITIALIZED ) // first run
		{
			pAuth->m_AuthServer.CreateSocket(ghWnd);

			if ( pAuth->ConnectToAuth() == false )
			{
				return;
			}
		}

		else // reconnect
		{
			if ( pAuth->m_AuthServer.GetSocket() == INVALID_SOCKET )
			{	
				if ( GetTickCount() - pAuth->AuthDCTime >= 60000 )
				{
					pAuth->ReconnectRetry++;
					pAuth->m_AuthServer.CreateSocket(ghWnd);

					if ( pAuth->Reconnect() == false )
					{
						pAuth->m_AuthServer.Close();
						pAuth->AuthDCTime = GetTickCount();

						if ( pAuth->ReconnectRetry == 60 )
						{
							GameMonsterAllCloseAndReLoad();
						}
					}

					else
					{
						if( pAuth->IsLongDisconnect() == true )
						{
							GameMonsterAllCloseAndReLoad();
						}

						pAuth->ReconnectRetry = 0;
						pAuth->AuthDCTime = 0;
					}
				}
			}
		}

		Sleep(1000);
	}
}

void CAuthSystem::GetLicenseInfo()
{
	//ENCODE_START

	PMSG_GETLIC pMsg;
	pMsg.HEAD = 0x15;
	pMsg.SIZE = sizeof(pMsg);

	this->SendData((LPBYTE)&pMsg, pMsg.SIZE, TRUE);

	//ENCODE_END
}

void CAuthSystem::RecvLicenseInfo(PMSG_SENDINFO *pMsg)
{
#ifdef GS_AUTHORITY
	//VM_TIGER_LONDON_START

	HWND hDlg = CreateDialog(hInst, (LPCTSTR)IDD_LICENSEINFO, ghWnd, (DLGPROC)LicenseInfo);

	char HWID[100];
	WLHardwareGetID(HWID);
	ParseHardwareID(HWID);

	SetDlgItemText(hDlg, IDC_EDIT1, pMsg->CustomerName);
	SetDlgItemText(hDlg, IDC_EDIT2, pMsg->ServerName);
	SetDlgItemText(hDlg, IDC_EDIT3, HWID);
	SetDlgItemText(hDlg, IDC_EDIT4, pMsg->ExpiryDate);

	if(pMsg->S6 == true)
	{
		CheckDlgButton(hDlg, IDC_CHECK2, BST_CHECKED);
	}

	if(pMsg->Ex700 == true)
	{
		CheckDlgButton(hDlg, IDC_CHECK3, BST_CHECKED);
	}

	if(pMsg->Ex702 == true)
	{
		CheckDlgButton(hDlg, IDC_CHECK5, BST_CHECKED);
	}

	if(pMsg->SE == true)
	{
		CheckDlgButton(hDlg, IDC_CHECK1, BST_CHECKED);
	}

	if(pMsg->Launcher == true)
	{
		CheckDlgButton(hDlg, IDC_CHECK4, BST_CHECKED);
	}

	ShowWindow(hDlg, SW_SHOW);
	UpdateWindow(hDlg);

	//VM_TIGER_LONDON_END
#endif
}

bool CheckAuthThread()
{
#ifdef GS_AUTHORITY
	//VM_TIGER_LONDON_START
	DWORD ExitCode;

	GetExitCodeThread(g_AuthSys.hConnThread, &ExitCode);
	if(ExitCode != STILL_ACTIVE)
	{
		g_AuthSys.SendMessage(AUTH_CONN_THREAD_FAIL);
		g_AuthSys.MessageHandle(AUTH_CONN_THREAD_FAIL);
		return false;
	}

	//VM_TIGER_LONDON_END
	return true;

#else
	return true;
#endif
	
}

DWORD CAuthSystem::GetSeason()
{
	return SERVER_SEASON9;
}

DWORD CAuthSystem::GetGSType()
{
	return g_ConfigRead.server.GetServerType() == SERVER_CASTLE ? GAMESERVER_SIEGE : GAMESERVER_NORMAL;
}

void CAuthSystem::SetAddons(DWORD Addons)
{
	//VM_TIGER_LONDON_START
/*	g_ConfigRead.OffLevel = FALSE;

	if((Addons & 4) == 4)
	{*/
		//g_Log.AddC(TColor::Green, "Offline Leveling Active");
		//g_ConfigRead.OffLevel = TRUE;
/*	}

	if(g_OffLevel.b_Enabled == true && g_ConfigRead.OffLevel == false)
	{
		MessageBoxA(0,"Your license does not have Offlevel plugin purchased, please disable OffLevel system","WARNING",MB_OK|MB_TOPMOST);
		ExitProcess(101);
	}
*/
	//VM_TIGER_LONDON_END
}

void CAuthSystem::ParseErrorCode(DWORD Message)
{
	//ENCODE_START

	if(Message == LICENSE_EXPIRED)
	{
		gObjAllLogOut();
		ExitProcess(10000);
	}

	else
	{
		ExitProcess(20000);
	}

	//ENCODE_END
}

bool CAuthSystem::CreateRSAKey(int Seed)
{
	//VM_TIGER_LONDON_START

	if(this->m_RSA != NULL)
	{
		delete this->m_RSA;
		this->m_RSA = NULL;
		this->m_AuthServer.SetRSA(NULL);
	}

	if(this->m_AuthKey != NULL)
	{
		delete this->m_AuthKey;
		this->m_AuthKey = NULL;
	}

	RNG m_RNG;

	m_RNG.SetSeed(Seed);

	this->m_RSA = new CRsa(256, m_RNG);
	this->m_AuthServer.SetRSA(this->m_RSA);
	this->m_AuthServer.SetProtocolCore(&this->AuthProtocolCore); 

	m_RNG.Clear();

	return true;

	//VM_TIGER_LONDON_END
}

bool CAuthSystem::MakeAuthKey(LPBYTE lpRecv)
{
	//VM_TIGER_LONDON_START

	PMSG_HANDSHAKE * lpMsg = (PMSG_HANDSHAKE *)lpRecv;

	BYTE * EKey = new BYTE[lpMsg->eSize];
	BYTE * NKey = new BYTE[lpMsg->nSize];

	memcpy(EKey, &lpRecv[sizeof(PMSG_HANDSHAKE)], lpMsg->eSize);
	memcpy(NKey, &lpRecv[sizeof(PMSG_HANDSHAKE)+lpMsg->eSize], lpMsg->nSize);

	if(this->m_AuthKey == NULL)
	{
		this->m_AuthKey = new CPublicRsaKey(256/8, NKey, lpMsg->nSize, EKey, lpMsg->eSize);
		delete [] EKey;
		delete [] NKey;

		return true;
	}

	else
	{
		g_Log.AddC(TColor::Red, "[ERROR] AUTH KEY IS NOT NULL!");	
		delete [] EKey;
		delete [] NKey;

		return false;
	}

	//VM_TIGER_LONDON_END
}

void CAuthSystem::SendData(LPBYTE lpMsg, DWORD dwSize, BOOL Encrypt)
{
	//VM_TIGER_LONDON_START
	if ( Encrypt )
	{
		if ( this->m_AuthKey == NULL )
		{
			g_Log.AddC(TColor::Red, "[ERROR] Auth Key is EMPTY!");
			return;
		}

		BYTE BUFFER_OUT[10000];
		int outsize = 0;

		CMuRSA::EncPacket(this->m_AuthKey, lpMsg, dwSize, BUFFER_OUT, outsize, 1);

		this->m_AuthServer.DataSend((char *)BUFFER_OUT, outsize);
	}

	else
	{
		this->m_AuthServer.DataSend((char *)lpMsg, dwSize);
	}

	//VM_TIGER_LONDON_END
}

void CAuthSystem::Test()
{

}

void ParseHardwareID(char *HWID)
{
	return;

	if(strlen(HWID) < 39)
	{
		return;
	}

	HWID[25] = '1';
	HWID[26] = 'C';
	HWID[27] = 'C';
	HWID[28] = '1';

	HWID[30] = '1';
	HWID[31] = 'C';
	HWID[32] = 'C';
	HWID[33] = '1';
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

