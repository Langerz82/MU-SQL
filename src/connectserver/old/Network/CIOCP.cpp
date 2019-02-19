#include "StdAfx.h"
#include "IOCP.h"
#include "ConnectEngine.h"
#include "ConnectProtocol.h"
#include "Main.h"
#include "generalStructs.h"

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <ace/Dev_Poll_Reactor.h>
#include <ace/TP_Reactor.h>


#include <string>

CIOCP IOCP;

//ACE_Reactor* g_Reactor = new ACE_Reactor();

void CIOCP::GiocpInit()
{
	ExSendBuf = new BYTE[MAX_EXSENDBUF_SIZE];

	
#if defined (ACE_HAS_EVENT_POLL) || defined (ACE_HAS_DEV_POLL)
	//CIOCP::g_Reactor = new ACE_Reactor(new ACE_Dev_Poll_Reactor(ACE::max_handles(), 1);
	ACE_Reactor::instance(new ACE_Reactor(new ACE_Dev_Poll_Reactor(ACE::max_handles(), 1), 1), true);
#else
	//CIOCP::g_Reactor = new ACE_Reactor(new ACE_TP_Reactor(), true);
	ACE_Reactor::instance(new ACE_Reactor(new ACE_TP_Reactor(), true), true);
#endif
	
	//ACE_Reactor::instance()->initialized();

	InitializeCriticalSection(&criti);
}

void CIOCP::ProcessEvents() {
	//g_Reactor->handle_events();
	ACE_Time_Value interval(0, 1000000);

	ACE_Reactor::instance()->handle_events(interval);
	//ACE_Reactor::instance()->run_reactor_event_loop(interval);
	sLog->outBasic("Pooing too.");
}

void CIOCP::GiocpDelete()
{
	delete[] ExSendBuf;
}

void CIOCP::DestroyGIocp()
{
	closesocket(g_Listen);

	if (g_CompletionPort != NULL)
	{
		CloseHandle(g_CompletionPort);
		g_CompletionPort = NULL;
	}

}


bool CIOCP::CreateListenSocket(WORD uiPort, LPSTR ipAddress)
{
	ACE_INET_Addr bind_addr(uiPort, ipAddress);
	
	//g_Reactor.initialized();

	if (g_HostSocket.open(bind_addr, ACE_Reactor::instance(), ACE_NONBLOCK) == -1)
	{
		sLog->outError("MuSQL Game Server can not bind to %s:%d", ipAddress, uiPort);
		return 0;
	}
	//g_HostSocket.acceptor().open(bind_addr);
	
	return 1;
}

int CIOCP::open(void* arg)
{
	if (Base::open(arg) == -1)
	{
		return -1;
	}

	ACE_INET_Addr addr;

	if (peer().get_remote_addr(addr) == -1)
	{
		return -1;
	}

	char address[128];

	addr.get_host_addr(address, 128);

	this->remote_address_ = address;

	this->OnAccept(peer().get_handle());

	Base::open(arg);

	return 0;
}

/*virtual*/ int CIOCP::handle_close(ACE_HANDLE /*h*/, ACE_Reactor_Mask /*m*/)
{
	this->OnClose();

	Base::handle_close();

	return 0;
}

int CIOCP::OnClose()
{
	g_UserIDMap.erase(this->peer().get_handle());

	this->peer().close_reader();
	this->peer().close_writer();

	reactor()->remove_handler(this, ACE_Event_Handler::DONT_CALL | ACE_Event_Handler::ALL_EVENTS_MASK);

	return 0;
}

void  CIOCP::CreateUserData(ACE_HANDLE handle)
{
	EnterCriticalSection(&criti);

	boost::uuids::basic_random_generator<boost::mt19937> gen;
	boost::uuids::uuid socketUUID = gen();

	const std::string sTemp = to_string(socketUUID);
	const char* sockKey = sTemp.c_str();

	char ipAddress[20];
	int lenIP = strlen(this->get_remote_address().c_str());
	std::memcpy(ipAddress, this->get_remote_address().c_str(), sizeof(lenIP));

	STR_CS_USER* ObjCSUser = UserAdd(sockKey, ipAddress);
	ObjCSUser->Socket = &this->peer();

	_PER_SOCKET_CONTEXT * lpPerSocketContext;
	int RecvBytes;
	unsigned long Flags;

#if FILE_FLOOD_SYSTEMSWITCH
	if (AntiFlood.Enabled == 1)
	{
		if (AntiFlood.Check(inet_ntoa(cInAddr)) == false)
		{
			closesocket(Accept);
			LeaveCriticalSection(&criti);
			return;
		}
	}
#endif

	//this->peer().set_handle((ACE_HANDLE)sockKey);
	ObjCSUser->PerSocketContext->dwIOCount = 0;

	_PER_SOCKET_CONTEXT* sockCtx = ObjCSUser->PerSocketContext;

	memset(&sockCtx->IOContext[0].m_Overlapped, 0, sizeof(WSAOVERLAPPED));
	memset(&sockCtx->IOContext[1].m_Overlapped, 0, sizeof(WSAOVERLAPPED));

	sockCtx->IOContext[0].m_wsabuf.buf = (char*)&sockCtx->IOContext[0].Buffer;
	sockCtx->IOContext[0].m_wsabuf.len = MAX_IO_BUFFER_SIZE;
	sockCtx->IOContext[0].IOOperation = 0;
	sockCtx->IOContext[1].m_wsabuf.buf = (char*)sockCtx->IOContext[1].Buffer;
	sockCtx->IOContext[1].m_wsabuf.len = MAX_IO_BUFFER_SIZE;
	sockCtx->IOContext[1].IOOperation = 1;
	sockCtx->nIndex = ObjCSUser->Index;

	sockCtx->IOContext[0].nWaitIO = 1;
	sockCtx->dwIOCount++;

	
	g_UserIDMap.insert(std::pair<ACE_HANDLE, STR_CS_USER*>(
		this->peer().get_handle(), ObjCSUser));

	// Spoof the CS that we have a game server.
	/*SDHP_SERVERINFO serverInfo;
	serverInfo.Port = 3000;
	serverInfo.ServerCode = 1;
	std::strcpy(serverInfo.ServerName , "Penis");
	serverInfo.ServerVIP = 0;
	serverInfo.Type = 0;
	CLoginServerProtocol::ProtocolCore(ObjCSUser->Index, 0x00, (LPBYTE) &serverInfo, sizeof(SDHP_SERVERINFO));*/

	// Send Server List.
	SCConnectResultSend(*ObjCSUser, 1);

	PostQueuedCompletionStatus(g_CompletionPort, 0, 0, 0);

	LeaveCriticalSection(&criti);

	//SCSendServerList(*ObjCSUser);
}

int CIOCP::OnAccept(ACE_HANDLE handle)
{
	this->CreateUserData(handle);
	return -1;
}

int CIOCP::OnRead(ACE_HANDLE handle)
{
	unsigned long Flags;
	DWORD dwSendNumBytes = 0;
	BOOL bSuccess = FALSE;
	int nRet;
#ifdef _WIN64
	ULONGLONG ClientIndex;
#else
	ULONG ClientIndex;
#endif
	_PER_SOCKET_CONTEXT * lpPerSocketContext = nullptr;
	LPOVERLAPPED lpOverlapped = 0;
	_PER_IO_CONTEXT * lpIOContext = nullptr;

	EnterCriticalSection(&criti);

	STR_CS_USER* lpUser = this->getUserData(this->peer().get_handle());
	if (lpUser == nullptr)
	{
		sLog->outError("Could not retrieve the User Object.");
		return errno == EWOULDBLOCK ? 0 : -1;
	}

	lpPerSocketContext = lpUser->PerSocketContext;
	if (lpPerSocketContext == nullptr)
		return 0;
	
	lpIOContext = lpUser->PerSocketContext->IOContext;
	if (lpIOContext == nullptr)
		return 0;

	lpPerSocketContext->dwIOCount--;

	lpIOContext->nWaitIO = 0;
	Flags = 0;

	unsigned long RecvBytes = 0; // = this->input_buffer_.space();
	ssize_t n = this->peer().recv(this->input_buffer_.wr_ptr(), this->input_buffer_.space());

	if (n < 0)
	{
		// blocking signal or error
		return errno == EWOULDBLOCK ? 0 : -1;
	}
	else if (n == 0)
	{
		// EOF
		return -1;
	}
	RecvBytes = n;
	lpIOContext->nSentBytes = n;
	lpIOContext->m_wsabuf.len = n;
	std::memcpy(&lpIOContext->m_wsabuf.buf, this->input_buffer_.base(), sizeof(n));
	
	RecvDataParse(lpIOContext, lpUser->Index);

	lpIOContext->IOOperation = 0;

	this->input_buffer_.reset();
	/*if (this->peer().get_handle() != ACE_INVALID_HANDLE)
	{
		try
		{
			lpIOContext->m_wsabuf.len = RecvBytes;			
			this->peer().recv(lpIOContext->m_wsabuf.buf, RecvBytes);

			this->RecvDataParse(lpIOContext, userIndex);
		}
		catch (const std::exception &e)
		{
			char text[1000];
			sprintf(text, "Exception %s was detected", e.what());
			sLog->outError(text);
		}
	}
	else
	{
		LeaveCriticalSection(&criti);
	}*/

	lpPerSocketContext->dwIOCount++;
	lpIOContext->nWaitIO = 1;

	LeaveCriticalSection(&criti);

	return 1;
}

int CIOCP::handle_input(ACE_HANDLE handle)
{
	return this->OnRead(handle);

	Base::handle_input(handle);
	return -1;
}

int CIOCP::handle_output(ACE_HANDLE handle)
{
	Base::handle_output(handle);
	return -1;
}


bool CIOCP::RecvDataParse(_PER_IO_CONTEXT * lpIOContext, int uIndex)	
{
	BYTE* recvbuf;
	int lOfs;
	int size;
	BYTE headcode;
	BYTE xcode;
	STR_CS_USER* lpUser = getCSUser(uIndex);

	EnterCriticalSection(&criti);

	if ( lpIOContext->nSentBytes < 3 )
	{
		return true;
	}

	lOfs=0;
	size=0;
	xcode=0;
	recvbuf = lpIOContext->Buffer;
	
	while ( true )
	{
		if ( recvbuf[lOfs] == 0xC1 ||
			 recvbuf[lOfs] == 0xC3 )
		{
			BYTE * pBuf;

			pBuf = &recvbuf[lOfs];
			size = pBuf[1];
			headcode = pBuf[2];
			xcode = recvbuf[lOfs];
		}
		else if ( recvbuf[lOfs] == 0xC2 ||
			      recvbuf[lOfs] == 0xC4 )
		{
			BYTE * pBuf;

			pBuf = &recvbuf[lOfs];
			size = pBuf[1] * 256;
			size |= pBuf[2];
			headcode = pBuf[3];
			xcode = recvbuf[lOfs];
		}

		else
		{
			sLog->outError("error-L1: Header error (%s %d)lOfs:%d, size:%d", __FILE__, __LINE__, lOfs, lpIOContext->nSentBytes);
			lpIOContext->nSentBytes = 0;
			return false;
		}

		if ( size <= 0 )
		{
			sLog->outError("error-L1: size %d",
				size);

			return false;
		}

		if ( size <= lpIOContext->nSentBytes )
		{
			lpUser->PacketCount++;

			if (lpUser->PacketCount >= g_MaxPacketPerSec /*&& strcmp(lpUser->IP, g_WhiteListIP)*/)
			{
				sLog->outError("[ANTI-FLOOD] Packets Per Second: %d / %d, IP: %d", lpUser->PacketCount, g_MaxPacketPerSec, lpUser->IP);
				this->CloseClient(uIndex);
				return false;
			}

			CSProtocolCore(headcode, &recvbuf[lOfs], size, lpUser->Index, 0, 0);

			lOfs += size;
			lpIOContext->nSentBytes  -= size;

			if ( lpIOContext->nSentBytes <= 0 )
			{
				break;
			}
		}
		else if ( lOfs > 0 )
		{
			if ( lpIOContext->nSentBytes < 1 )
			{
				sLog->outError("error-L1: recvbuflen 1 %s %d", __FILE__, __LINE__);
				break;
			}

			if ( lpIOContext->nSentBytes < MAX_IO_BUFFER_SIZE ) 
			{
				std::memcpy(recvbuf, &recvbuf[lOfs], lpIOContext->nSentBytes);
				sLog->outError("Message copy %d", lpIOContext->nSentBytes);
			}
			break;
		
		}
		else
		{
			break;
		}
		
	}

	LeaveCriticalSection(&criti);
	return true;
}

bool CIOCP::DataSend(int uIndex, LPBYTE lpMsg, DWORD dwSize, bool Encrypt)
{
	unsigned long SendBytes;
	_PER_SOCKET_CONTEXT * lpPerSocketContext;
	//LPBYTE SendBuf;
	//BYTE BUFFER[65535];

	EnterCriticalSection(&criti);

	STR_CS_USER* lpCSUser = getCSUser(uIndex);
	//std::memcpy(SendBuf, lpMsg, dwSize);

	if (lpCSUser->ConnectionState == 0  )
	{
		LeaveCriticalSection(&criti);
		return false;
	}

	lpPerSocketContext = lpCSUser->PerSocketContext;

	/*if ( dwSize > sizeof(lpPerSocketContext->IOContext[0].Buffer))
	{
		sLog->outError("Error: Max msg(%d) %s %d", dwSize, __FILE__, __LINE__);
		CloseClient(uIndex);
		LeaveCriticalSection(&criti);
		return false;
	}*/

	_PER_IO_CONTEXT  * lpIoCtxt;

	lpIoCtxt = &lpPerSocketContext->IOContext[1];

	/*
	if ( lpIoCtxt->nWaitIO > 0 )
	{
		if ( ( lpIoCtxt->nSecondOfs + dwSize ) > MAX_IO_BUFFER_SIZE-1 )
		{
			sLog->outError("(%d)error-L2: MAX BUFFER OVER %d %d %d", uIndex, lpIoCtxt->nTotalBytes, lpIoCtxt->nSecondOfs, dwSize);
			lpIoCtxt->nWaitIO = 0;
			CloseClient(uIndex);
			LeaveCriticalSection(&criti);
			return true;
		}

		std::memcpy( &lpIoCtxt->BufferSecond[lpIoCtxt->nSecondOfs], lpMsg, dwSize);
		lpIoCtxt->nSecondOfs += dwSize;
		LeaveCriticalSection(&criti);
		return true;
	}

	lpIoCtxt->nTotalBytes = 0;
	
	if ( lpIoCtxt->nSecondOfs > 0 )
	{
		std::memcpy(lpIoCtxt->Buffer, lpIoCtxt->BufferSecond, lpIoCtxt->nSecondOfs);
		lpIoCtxt->nTotalBytes = lpIoCtxt->nSecondOfs;
		lpIoCtxt->nSecondOfs = 0;
	}

	if ( (lpIoCtxt->nTotalBytes+dwSize) > MAX_IO_BUFFER_SIZE-1 )
	{
		sLog->outError("(%d)error-L2: MAX BUFFER OVER %d %d", uIndex, lpIoCtxt->nTotalBytes, dwSize);
		lpIoCtxt->nWaitIO = 0;
		CloseClient(uIndex);
		LeaveCriticalSection(&criti);
		return false;
	}*/

	//std::memcpy( &lpIoCtxt->Buffer[lpIoCtxt->nTotalBytes], lpMsg, dwSize);
	lpIoCtxt->nTotalBytes += dwSize;
	//lpIoCtxt->m_wsabuf.buf = lpMsg;
	//lpIoCtxt->m_wsabuf.len = dwSize;
	lpIoCtxt->nSentBytes = 0;
	lpIoCtxt->IOOperation = 1;
	
	lpCSUser->Socket->send(lpMsg, dwSize);
	
	lpPerSocketContext->dwIOCount ++;
	lpIoCtxt->nWaitIO = 1;
	LeaveCriticalSection(&criti);
	return true;
}


bool CIOCP::IoSendSecond(_PER_SOCKET_CONTEXT * lpPerSocketContext)
{
	// TODO - userIndex.
	unsigned long SendBytes;
	_PER_IO_CONTEXT * lpIoCtxt;
	

	EnterCriticalSection(&criti);
	int uIndex = lpPerSocketContext->nIndex;
	STR_CS_USER* lpCSUser = getCSUser(uIndex);

	lpIoCtxt = &lpPerSocketContext->IOContext[1];

	if ( lpIoCtxt->nWaitIO > 0 )
	{
		LeaveCriticalSection(&criti);
		return false;
	}

	lpIoCtxt->nTotalBytes = 0;
	if ( lpIoCtxt->nSecondOfs > 0 )
	{
		std::memcpy(lpIoCtxt->Buffer, lpIoCtxt->BufferSecond, lpIoCtxt->nSecondOfs);
		lpIoCtxt->nTotalBytes = lpIoCtxt->nSecondOfs;
		lpIoCtxt->nSecondOfs = 0;
	}
	else
	{
		LeaveCriticalSection(&criti);
		return false;
	}

	lpIoCtxt->m_wsabuf.buf = (char*)&lpIoCtxt->Buffer;
	lpIoCtxt->m_wsabuf.len = lpIoCtxt->nTotalBytes;
	lpIoCtxt->nSentBytes = 0;
	lpIoCtxt->IOOperation = 1;

	ACE_SOCK_Stream* socket = lpCSUser->Socket;
	socket->send(lpIoCtxt->m_wsabuf.buf, lpIoCtxt->m_wsabuf.len);

	lpPerSocketContext->dwIOCount++;
	
	lpIoCtxt->nWaitIO = 1;
	LeaveCriticalSection(&criti);
	
	return true;
}

bool CIOCP::IoMoreSend(_PER_SOCKET_CONTEXT * lpPerSocketContext)
{
	unsigned long SendBytes;
	_PER_IO_CONTEXT * lpIoCtxt;

	EnterCriticalSection(&criti);
	int uIndex = lpPerSocketContext->nIndex;
	STR_CS_USER* lpCSUser = getCSUser(uIndex);
	lpIoCtxt = &lpPerSocketContext->IOContext[1];

	if ( (lpIoCtxt->nTotalBytes - lpIoCtxt->nSentBytes) < 0 )
	{
		LeaveCriticalSection(&criti);
		return false;
	}

	lpIoCtxt->m_wsabuf.buf = (char*)&lpIoCtxt->Buffer[lpIoCtxt->nSentBytes];
	lpIoCtxt->m_wsabuf.len = lpIoCtxt->nTotalBytes - lpIoCtxt->nSentBytes;
	lpIoCtxt->IOOperation = 1;

	this->peer().send(lpIoCtxt->m_wsabuf.buf, lpIoCtxt->m_wsabuf.len);
	
	lpIoCtxt->nWaitIO = 1;
	LeaveCriticalSection(&criti);
	return true;
}


void CIOCP::CloseClient(_PER_SOCKET_CONTEXT * lpPerSocketContext, int result)
{
	int index = -1;
	index = lpPerSocketContext->nIndex ;
	STR_CS_USER* lpCSUser;
	for each (auto user in gCSUsers)
	{
		lpCSUser = getCSUser(index);
		user.second->Socket->close();
		UserDelete(index);
	}
}

void CIOCP::CloseClient(int index)
{
	STR_CS_USER* lpCSUser = getCSUser(index);

	if (lpCSUser->ConnectionState == 0 )
	{
		sLog->outError("error-L1 : CloseClient connect error");
		return;
	}

	EnterCriticalSection(&criti);

	lpCSUser->Socket->close();

	LeaveCriticalSection(&criti);
}

void CIOCP::ResponErrorCloseClient(int index)
{
	STR_CS_USER* lpCSUser = getCSUser(index);

	if (lpCSUser->ConnectionState == 0 )
	{
		sLog->outError("error-L1 : CloseClient connect error");
		return;
	}

	EnterCriticalSection(&criti);
	closesocket(lpCSUser->Index);
	lpCSUser->Socket = nullptr;
	UserDelete(lpCSUser->Index);
	LeaveCriticalSection(&criti);
}
