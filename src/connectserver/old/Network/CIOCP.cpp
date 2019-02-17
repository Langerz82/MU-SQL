#include "StdAfx.h"
#include "IOCP.h"
#include "ConnectEngine.h"
#include "ConnectProtocol.h"
#include "Main.h"
#include "generalStructs.h"

CIOCP IOCP;

void CIOCP::GiocpInit()
{
	ExSendBuf = new BYTE[MAX_EXSENDBUF_SIZE];

#if defined (ACE_HAS_EVENT_POLL) || defined (ACE_HAS_DEV_POLL)
	ACE_Reactor::instance(new ACE_Reactor(new ACE_Dev_Poll_Reactor(ACE::max_handles(), 1), 1), true);
#else
	ACE_Reactor::instance(new ACE_Reactor(new ACE_TP_Reactor(), true), true);
#endif
	InitializeCriticalSection(&criti);
}

void CIOCP::GiocpDelete()
{
	delete[] ExSendBuf;
}

void CIOCP::DestroyGIocp()
{
	closesocket(g_Listen);

	for (DWORD dwCPU = 0; dwCPU < g_dwThreadCount; dwCPU++)
	{
		TerminateThread(g_ThreadHandles[dwCPU], 0);
	}

	TerminateThread(g_IocpThreadHandle, 0);

	if (g_CompletionPort != NULL)
	{
		CloseHandle(g_CompletionPort);
		g_CompletionPort = NULL;
	}

}


bool CIOCP::CreateListenSocket(UINT16 uiPort, LPSTR ipAddress)
{
	sockaddr_in InternetAddr;
	int nRet;

	ACE_INET_Addr bind_addr(uiPort, ipAddress);

	if (g_buffSocket.open(bind_addr, ACE_Reactor::instance(), ACE_NONBLOCK) == -1)
	{
		sLog->outError("MuSQL Game Server can not bind to %s:%d", ipAddress, uiPort);
		return 0;
	}
}

void CIOCP::OnAccept()
{
	//boost::uuids::uuid socketKey(boost::uuids::random_generator());
	boost::uuids::uuid socketKey();

	char ipAddress[20], charSockKey[32];
	int lenIP = strlen(this->get_remote_address().c_str());
	std::memcpy(ipAddress, this->get_remote_address().c_str(), sizeof(lenIP));
	std::memcpy(charSockKey, socketKey, sizeof(32));
	STR_CS_USER* ObjCSUser = UserAdd(charSockKey, ipAddress);
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

	this->peer().set_handle((ACE_HANDLE)charSockKey);
	ObjCSUser->PerSocketContext->dwIOCount = 0;

	_PER_SOCKET_CONTEXT* sockCtx = ObjCSUser->PerSocketContext;

	memset(&sockCtx->IOContext[0].m_Overlapped, 0, sizeof(WSAOVERLAPPED));
	memset(&sockCtx->IOContext[1].m_Overlapped, 0, sizeof(WSAOVERLAPPED));

	sockCtx->IOContext[0].m_wsabuf.buf = (char*)&sockCtx->IOContext[0].Buffer;
	sockCtx->IOContext[0].m_wsabuf.len = MAX_IO_BUFFER_SIZE;
	sockCtx->IOContext[0].nTotalBytes = 0;
	sockCtx->IOContext[0].nSentBytes = 0;
	sockCtx->IOContext[0].nWaitIO = 0;
	sockCtx->IOContext[0].nSecondOfs = 0;
	sockCtx->IOContext[0].IOOperation = 0;
	sockCtx->IOContext[1].m_wsabuf.buf = (char*)sockCtx->IOContext[1].Buffer;
	sockCtx->IOContext[1].m_wsabuf.len = MAX_IO_BUFFER_SIZE;
	sockCtx->IOContext[1].nTotalBytes = 0;
	sockCtx->IOContext[1].nSentBytes = 0;
	sockCtx->IOContext[1].nWaitIO = 0;
	sockCtx->IOContext[1].nSecondOfs = 0;
	sockCtx->IOContext[1].IOOperation = 1;
	sockCtx->nIndex = ObjCSUser->Index;

	sockCtx->IOContext[0].nWaitIO = 1;
	sockCtx->dwIOCount++;

	g_UserIDMap.insert(std::pair<char*, int>(charSockKey, ObjCSUser->Index));

	LeaveCriticalSection(&criti);
	//SCPJoinResultSend(*lpObj, 1);
}

void CIOCP::OnRead()
{
	//HANDLE CompletionPort;
	DWORD dwIoSize;
	unsigned long RecvBytes;
	unsigned long Flags;
	DWORD dwSendNumBytes = 0;
	BOOL bSuccess = FALSE;
	int nRet;
#ifdef _WIN64
	ULONGLONG ClientIndex;
#else
	ULONG ClientIndex;
#endif
	_PER_SOCKET_CONTEXT * lpPerSocketContext = 0;
	LPOVERLAPPED lpOverlapped = 0;
	_PER_IO_CONTEXT * lpIOContext = 0;

	int userIndex = getUserDataIndex((const char*)get_handle());
	STR_CS_USER* lpUser = getCSUser(userIndex);

	EnterCriticalSection(&criti);

	lpPerSocketContext = lpUser->PerSocketContext;
	lpPerSocketContext->dwIOCount--;

	RecvBytes = 0;
	lpIOContext->nSentBytes += dwIoSize;

	lpIOContext->nWaitIO = 0;
	Flags = 0;

	lpIOContext->nSentBytes = recv_len();

	memset(&lpIOContext->m_Overlapped, 0, sizeof(WSAOVERLAPPED));

	lpIOContext->IOOperation = 0;
	if (this->peer().get_handle() != ACE_INVALID_HANDLE)
	{
		try
		{
			lpIOContext->m_wsabuf.len = recv_len();
			recv(lpIOContext->m_wsabuf.buf, recv_len());
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
	}
	lpPerSocketContext->dwIOCount++;
	lpIOContext->nWaitIO = 1;

	LeaveCriticalSection(&criti);
}

bool CIOCP::RecvDataParse(_PER_IO_CONTEXT * lpIOContext, int uIndex)	
{
	BYTE* recvbuf;
	int lOfs;
	int size;
	BYTE headcode;
	BYTE xcode;
	STR_CS_USER* lpUser = getCSUser(uIndex);

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

			CSProtocolCore(headcode, &recvbuf[lOfs], size, *lpUser, 0, 0);

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

	return true;
}

bool CIOCP::DataSend(int uIndex, BYTE* lpMsg, DWORD dwSize, bool Encrypt)
{
	unsigned long SendBytes;
	_PER_SOCKET_CONTEXT * lpPerSocketContext;
	BYTE * SendBuf;
	BYTE BUFFER[65535];
	STR_CS_USER* lpCSUser = getCSUser(uIndex);

	EnterCriticalSection(&criti);

	if ( ((uIndex < 0)? FALSE : (uIndex > 1000-1)? FALSE : TRUE )  == FALSE )
	{
		sLog->outError("error-L2: Index(%d) %x %x %x ", dwSize, lpMsg[0], lpMsg[1], lpMsg[2]);
		LeaveCriticalSection(&criti);
		return false;
	}

	SendBuf = lpMsg;

	if (lpCSUser->ConnectionState == 0  )
	{
		LeaveCriticalSection(&criti);
		return false;
	}

	lpPerSocketContext = lpCSUser->PerSocketContext;

	if ( dwSize > sizeof(lpPerSocketContext->IOContext[0].Buffer))
	{
		sLog->outError("Error: Max msg(%d) %s %d", dwSize, __FILE__, __LINE__);
		CloseClient(uIndex);
		LeaveCriticalSection(&criti);
		return false;
	}

	_PER_IO_CONTEXT  * lpIoCtxt;

	lpIoCtxt = &lpPerSocketContext->IOContext[1];

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

		std::memcpy( &lpIoCtxt->BufferSecond[lpIoCtxt->nSecondOfs], SendBuf, dwSize);
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
	}

	std::memcpy( &lpIoCtxt->Buffer[lpIoCtxt->nTotalBytes], SendBuf, dwSize);
	lpIoCtxt->nTotalBytes += dwSize;
	lpIoCtxt->m_wsabuf.buf = (char*)&lpIoCtxt->Buffer;
	lpIoCtxt->m_wsabuf.len = lpIoCtxt->nTotalBytes;
	lpIoCtxt->nSentBytes = 0;
	lpIoCtxt->IOOperation = 1;
	
	ACE_SOCK_Stream* socket = lpCSUser->Socket;
	socket->send(lpIoCtxt->m_wsabuf.buf, lpIoCtxt->m_wsabuf.len);

	/*if ( WSASend( lpUser->socket, lpIoCtxt->m_wsabuf , 1, &SendBytes, 0, lpIoCtxt->m_Overlapped, NULL) == -1 ) // TODO - Linux Equivalent
	{

		if ( WSAGetLastError() != WSA_IO_PENDING )	
		{
			lpIoCtxt->nWaitIO = 0;
			
			if ( lpIoCtxt->m_wsabuf.buf[0] == 0xC1 )
			{
				sLog->outError("(%d)WSASend(%d) failed with error [%x][%x] %d %s ", __LINE__, uIndex, (BYTE)lpIoCtxt->m_wsabuf.buf[0],
					(BYTE)lpIoCtxt->m_wsabuf.buf[2], WSAGetLastError(), lpUser->IP);
			}
			else if ( lpIoCtxt->m_wsabuf.buf[0] == 0xC2 )
			{
				sLog->outError("(%d)WSASend(%d) failed with error [%x][%x] %d %s ", __LINE__, uIndex, (BYTE)lpIoCtxt->m_wsabuf.buf[0],
					(BYTE)lpIoCtxt->m_wsabuf.buf[3], WSAGetLastError(), lpUser->IP);
			}
			CloseClient(uIndex);
			LeaveCriticalSection(&criti);
			return false;
		}
	}
	else
	{
		lpPerSocketContext->dwIOCount ++;
	}*/
	
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
	/*
	{
		if ( WSAGetLastError() != WSA_IO_PENDING )
		{
			sLog->outError("WSASend(%d) failed with error %d %s ", __LINE__, WSAGetLastError(), lpUser->IP);
			CloseClient(uIndex);
			LeaveCriticalSection(&criti);
			return false;
		}
	}*/
	//else
	//{
	lpPerSocketContext->dwIOCount++;
	//}
	
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

	ACE_SOCK_Stream* socket = lpCSUser->Socket;
	socket->send(lpIoCtxt->m_wsabuf.buf, lpIoCtxt->m_wsabuf.len);
	/*
	if ( WSASend(lpUser->socket, lpIoCtxt->m_wsabuf, 1, &SendBytes, 0, lpIoCtxt->m_Overlapped, NULL) == -1 )
	{
		if ( WSAGetLastError() != WSA_IO_PENDING )
		{
			sLog->outError("WSASend(%d) failed with error %d %s ", __LINE__, WSAGetLastError(), lpUser->IP);
			CloseClient(uIndex);
			LeaveCriticalSection(&criti);
			return false;
		}
	}
	else
	{
		lpPerSocketContext->dwIOCount ++;
	}
	*/
	
	lpIoCtxt->nWaitIO = 1;
	LeaveCriticalSection(&criti);
	return true;
}

/*bool CIOCP::UpdateCompletionPort(SOCKET sd, int ClientIndex, BOOL bAddToList)
{
	_PER_SOCKET_CONTEXT * lpPerSocketContext = NULL;
	STR_CS_USER* lpUser = getUser(ClientIndex);

	HANDLE cp = CreateIoCompletionPort((HANDLE) sd, g_CompletionPort, ClientIndex, 0);

	if ( cp == 0 )
	{
		sLog->outError("CreateIoCompletionPort: %d", GetLastError() );
		return FALSE;
	}

	lpUser->PerSocketContext->dwIOCount = 0;
	return TRUE;
}*/

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
