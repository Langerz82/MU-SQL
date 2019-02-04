
// ------------------------------
// Decompiled by Deathway
// Date : 2007-03-09
// ------------------------------
// GS-N 0.99.60T MyWinsockBase.cpp	Status : Completed - Review procedure MyWinsockBase::Startup() - Finished, just SAR problem -SHR
//	GS-N	1.00.18	JPN	0x0048CD90	-	Completed

#include "stdafx.h"
#include "MyWinsockBase.h"

MyWinsockBase::MyWinsockBase()
{
	this->m_socket = INVALID_SOCKET;
	this->Startup();
}

MyWinsockBase::~MyWinsockBase()
{
	WSACleanup();
}

BOOL MyWinsockBase::Startup()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = WINSOCK_VERSION;
	err=WSAStartup(wVersionRequested, &wsaData);
	
	if ( err != 0 )
	{
		MessageBox(NULL, "WINSOCK STARTUP FAILED", "Error", MB_OK|MB_APPLMODAL|MB_TOPMOST);
		return FALSE;
	}

	if ( ((wsaData.wVersion &0xFF & 0xFF )   != (2 ) ) || ( ( ((DWORD)wsaData.wVersion >> (DWORD)8) &0xFF )  != (2)) )	// Lacking Some Code Here	It said SAR and is SHR
	{
		WSACleanup();
		MessageBox(NULL, "WINSOCK VERSION INVALID", "Error", MB_OK|MB_APPLMODAL|MB_TOPMOST);
		return FALSE;
	}
	else
	{
		this->m_socket = INVALID_SOCKET;
		this->m_iMaxSockets = wsaData.iMaxSockets ;
		this->m_Connect = FALSE;
		return TRUE;
	}
}

		

BOOL MyWinsockBase::CreateSocket(HWND hWnd)
{
	int size;
	size=4;

	this->m_socket=socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	
	if (this->m_socket == INVALID_SOCKET )
	{
		return FALSE;
	}

	this->m_hWnd =hWnd;
	return TRUE;
}

BOOL MyWinsockBase::Close()
{
	closesocket(this->m_socket );
	this->m_socket=INVALID_SOCKET;

	this->m_Connect=FALSE;

	return TRUE;
}

BOOL MyWinsockBase::Close(SOCKET socket)
{
	shutdown(socket, SD_RECEIVE);
	closesocket(socket);

	this->m_socket=INVALID_SOCKET;
	this->m_Connect =FALSE;
	return TRUE;
}

int MyWinsockBase::Close2(SOCKET socket)
{
	shutdown(this->m_socket , SD_SEND);
	closesocket(socket);

	this->m_socket=INVALID_SOCKET;
	this->m_Connect=0;
	return 1;
}

SOCKET MyWinsockBase::GetSocket()
{
	return this->m_socket;
}

int MyWinsockBase::GetRecvBuffSize()
{
	return this->m_recvbufsize;
}

int MyWinsockBase::GetSendBuffSize()
{
	return this->m_sendbufsize;
}

int MyWinsockBase::GetConnect()
{
	return this->m_Connect;
}


void MyWinsockBase::SetConnect(BOOL connected)	
{
	this->m_Connect = connected;
}



////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

