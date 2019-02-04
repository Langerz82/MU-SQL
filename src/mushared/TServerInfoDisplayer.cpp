// TServerInfoDisplayer.cpp: implementation of the TServerInfoDisplayer class.
//	GS-N	1.00.18	JPN	0x004A9500	-	Completed
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TServerInfoDisplayer.h"
#include "Gamemain.h"
#include "LogToFile.h"
#include "CastleSiege.h"
#include "Crywolf.h"
#include "Authorization.h"
#include "configread.h"

TServerInfoDisplayer g_ServerInfoDisplayer;
static CLogToFile * SERVER_CONN_STATE_LOG;

static char * ServerCSText[1] = 
{
	"[- CASTLE SIEGE ONGOING -]"
};

static char * ServerCWText[1] = 
{
	"[- CRYWOLF ONGOING -]"
};

static char * ServerTypeText[1] =
{
	"[- MUSERVER SEASON 12 EP2 -]" //"[-  Common  -]" or "]+[  Castle  ]+["
};

static char * ServerTypeTextVersion[1] =
{
	"[- IGC SERVER v12.0.1.0 -]" //"[-  Common  -]" or "]+[  Castle  ]+["
};

static char * ServerErrorText[1] = 
{
	"[- ERROR -]"
};

static char * ErrorMessge[4] = 
{
	"JoinServer - Disconnected",
	"DataServer - Disconnected",
	"ExDataServer - Disconnected",
	"AuthServer - reconnection failed, authentication lost"
};

static char * AuthMessage[1] =
{
	"AuthServer - disconnected, reconnecting",
};



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TServerInfoDisplayer::TServerInfoDisplayer()
{
	this->m_bValidJSConnection = FALSE;
	this->m_bValidDSConnection = FALSE;
	this->m_bValidEXDSConnection = FALSE;
	this->m_bValidASConnection = FALSE;

	this->InitGDIObject();
}

TServerInfoDisplayer::~TServerInfoDisplayer()
{
	delete SERVER_CONN_STATE_LOG;
	this->DelGDIObject();
}


void TServerInfoDisplayer::InitGDIObject()
{
	this->m_hFont = CreateFont(70, 0, 0, 0, FW_LIGHT, FALSE, TRUE, FALSE, 
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_DONTCARE, "Tahoma");

	this->m_hNormalBrush = CreateSolidBrush(RGB(0x2B, 0x2B, 0x2B));
	this->m_hAbnormalBrush = CreateSolidBrush(RGB(255, 32, 37));
}


void TServerInfoDisplayer::DelGDIObject()
{
	DeleteObject(this->m_hFont);
	DeleteObject(this->m_hNormalBrush);
	DeleteObject(this->m_hAbnormalBrush);
}


void TServerInfoDisplayer::Run(HWND hWnd)
{
	this->CheckDSConnection();
	this->CheckJSConnection();
	this->CheckEXDSConnection();
	this->CheckASConnection();

	this->CheckLogFileSize();

	this->PaintAllInfo(hWnd, 0, 20);
}

void TServerInfoDisplayer::CheckLogFileSize()
{
	if(SERVER_CONN_STATE_LOG == NULL)
	{
		SERVER_CONN_STATE_LOG = new CLogToFile("SERVER_CONN_STATE_LOG", g_ConfigRead.server.GetServerLogPath(), 256);
	}

	return;
}


void TServerInfoDisplayer::CheckJSConnection()
{
	if ( wsJServerCli.GetSocket() == INVALID_SOCKET )
		this->m_bValidJSConnection = FALSE;
	else
		this->m_bValidJSConnection = TRUE;
}


void TServerInfoDisplayer::CheckDSConnection()
{
	if ( wsDataCli.GetSocket() == INVALID_SOCKET )
		this->m_bValidDSConnection = FALSE;
	else
		this->m_bValidDSConnection = TRUE;
}

void TServerInfoDisplayer::CheckEXDSConnection()
{
	if ( wsExDbCli.GetSocket() == INVALID_SOCKET )
		this->m_bValidEXDSConnection = FALSE;
	else
		this->m_bValidEXDSConnection = TRUE;

	if ( g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE )
	{
		this->m_bValidEXDSConnection = TRUE;
	}
}

void TServerInfoDisplayer::CheckASConnection()
{
#ifdef GS_AUTHORITY
	if ( g_AuthSys.m_AuthServer.GetSocket() == INVALID_SOCKET )
		this->m_bValidASConnection = FALSE;
	else
		this->m_bValidASConnection = TRUE;
#else
	this->m_bValidASConnection = TRUE;
#endif
}

void TServerInfoDisplayer::PaintAllInfo(HWND hWnd, int iTopLeftX, int iTopLeftY)
{
	HDC hDC = GetDC(hWnd);
	RECT rect;
	GetClientRect(hWnd, &rect);
	rect.top = 0;
	rect.bottom = 100;
	int iLineCount = 0;

	int iOldBkMode = SetBkMode(hDC, TRANSPARENT);

	if ( this->m_bValidJSConnection != FALSE && this->m_bValidDSConnection != FALSE && g_AuthSys.IsLongDisconnect() == false )
		FillRect(hDC, &rect, this->m_hNormalBrush);
	else
		FillRect(hDC, &rect, this->m_hAbnormalBrush);

	if ( this->m_bValidJSConnection == FALSE )
	{
		SetTextColor(hDC, RGB(0, 0, 0));
		TextOut(hDC, iTopLeftX, iTopLeftY+iLineCount*15, ErrorMessge[0], strlen(ErrorMessge[0]));
		iLineCount++;
	}

	if ( this->m_bValidDSConnection == FALSE )
	{
		SetTextColor(hDC, RGB(0, 0, 0));
		TextOut(hDC, iTopLeftX, iTopLeftY+iLineCount*15, ErrorMessge[1], strlen(ErrorMessge[1]));
		iLineCount++;
	}

	if ( this->m_bValidEXDSConnection == FALSE )
	{
		SetTextColor(hDC, RGB(0, 0, 0));
		TextOut(hDC, iTopLeftX, iTopLeftY+iLineCount*15, ErrorMessge[2], strlen(ErrorMessge[2]));
		iLineCount++;
	}

	if ( this->m_bValidASConnection == FALSE )
	{
		if ( g_AuthSys.IsLongDisconnect() == true )
		{
			SetTextColor(hDC, RGB(0, 0, 0));
			TextOut(hDC, iTopLeftX, iTopLeftY+iLineCount*15, ErrorMessge[3], strlen(ErrorMessge[3]));
			iLineCount++;
		}

		else
		{
			SetTextColor(hDC, RGB(255, 255, 255));
			TextOut(hDC, iTopLeftX, iTopLeftY+iLineCount*15, AuthMessage[0], strlen(AuthMessage[0]));
			iLineCount++;
		}
	}

	if ( g_ConfigRead.server.GetServerType() == SERVER_CASTLE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE )
	{
		SetTextColor(hDC, RGB(255, 255, 255));
	}

	if ( this->m_bValidJSConnection == FALSE || this->m_bValidDSConnection == FALSE || this->m_bValidEXDSConnection == FALSE || this->m_bValidASConnection == FALSE )
	{
		SERVER_CONN_STATE_LOG->Output("Server Interconnection Check - [JS/%d] [DS/%d] [EX/%d] [AUTH/%d]",
			this->m_bValidJSConnection, this->m_bValidDSConnection,
			this->m_bValidEXDSConnection, this->m_bValidASConnection);
	}

	HFONT pOldFont = (HFONT)SelectObject(hDC, this->m_hFont);
	SetTextColor(hDC, RGB(0xE2, 0xF0, 0x63));

	if ( this->m_bValidJSConnection == FALSE || this->m_bValidDSConnection == FALSE || this->m_bValidEXDSConnection == FALSE || (this->m_bValidASConnection == FALSE && g_AuthSys.IsLongDisconnect() == true) )
	{
		SetTextColor(hDC, RGB(0, 0, 0));
		TextOut(hDC,250, 20, ServerErrorText[0], strlen(ServerErrorText[0]));
	}

	else if ( g_ConfigRead.server.GetServerType() == SERVER_CASTLE && g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE )
	{
		SetTextColor(hDC, RGB(255, 255, 255));
		TextOut(hDC,250, 20, ServerCSText[0], strlen(ServerCSText[0]));
	}

	else if ( g_Crywolf.GetCrywolfState() != CRYWOLF_STATE_NONE )
	{
		SetTextColor(hDC, RGB(255, 255, 255));
		TextOut(hDC,250, 20, ServerCWText[0], strlen(ServerCWText[0]));
	}

	else
	{
		TextOutA(hDC, 150, 20, ServerTypeText[0], strlen(ServerTypeText[0]));
		//TextOutA(hDC, 80, 50, ServerTypeTextVersion[0], strlen(ServerTypeTextVersion[0]));
	} //ServerTypeTextVersion

	SelectObject(hDC, pOldFont);
	SetBkMode(hDC, iOldBkMode);
	ReleaseDC(hWnd, hDC);
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

