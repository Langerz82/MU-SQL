#include "IOCP.h"
#include "PacketEngineServer.h" // Encryption/Decryption
#include "Main.h"
#include "GameServer.h"
#include "PacketEncrypt.h"
#include "FloodSystem.h"
#include "GOFunctions.h"
#include "Logging/Log.h"
#include "ConnectEngine.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_serialize.hpp>

CIOCP IOCP;

void CIOCP::GiocpInit()
{
	ExSendBuf = new BYTE[MAX_EXSENDBUF_SIZE];

#if defined (ACE_HAS_EVENT_POLL) || defined (ACE_HAS_DEV_POLL)
	ACE_Reactor::instance(new ACE_Reactor(new ACE_Dev_Poll_Reactor(ACE::max_handles(), 1), 1), true);
#else
	ACE_Reactor::instance(new ACE_Reactor(new ACE_TP_Reactor(), true), true);
#endif

}

void CIOCP::GiocpDelete()
{
	delete[] ExSendBuf;
}

bool CIOCP::CreateGIocp(int server_port)
{
	g_ServerPort = server_port;

	InitializeCriticalSection(&criti);
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


bool CIOCP::CreateListenSocket(uint16 uiPort, LPTSTR ipAddress)
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
	boost::uuids::uuid socketKey(boost::uuids::random_generator());
	
	char ipAddress[20], charSockKey[32];
	int lenIP = strlen(this->get_remote_address().c_str());
	std::memcpy(ipAddress, this->get_remote_address().c_str(), sizeof(lenIP));
	std::memcpy(charSockKey, socketKey, sizeof(32));
	int ClientIndex = UserAdd(charSockKey, ipAddress);

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
	gUsers[ClientIndex]->PerSocketContext->dwIOCount = 0;

	CGameObject* lpObj = getGameObject(ClientIndex);
	_PER_SOCKET_CONTEXT* sockCtx = lpObj->m_PlayerData->ConnectUser->PerSocketContext;

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
	sockCtx->nIndex = ClientIndex;

	sockCtx->IOContext[0].nWaitIO = 1;
	sockCtx->dwIOCount++;

	CreateIoCompletionPort(ClientIndex);

	g_UserIDMap.insert(std::pair<char*, int>(charSockKey, ClientIndex));

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
	CUserData* lpUser = gUserObjects[userIndex];

	EnterCriticalSection(&criti);

	lpPerSocketContext = lpUser->ConnectUser->PerSocketContext;
	lpPerSocketContext->dwIOCount--;

	RecvBytes = 0;
	lpIOContext->nSentBytes += dwIoSize;

	lpIOContext->nWaitIO = 0;
	Flags = 0;

	lpIOContext->nSentBytes = recv_len();

	memset(&lpIOContext->m_Overlapped, 0, sizeof(WSAOVERLAPPED));

	lpIOContext->IOOperation = 0;
	if (this->peer.get_handle() != ACE_INVALID_HANDLE)
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

//#define USERSA 0
#define USERIJ 
bool CIOCP::RecvDataParse(_PER_IO_CONTEXT * lpIOContext, int uIndex)
{
	BYTE* recvbuf;
	int lOfs;
	int size;
	BYTE headcode;
	BYTE xcode;
	CUserData* lpUser = gUserObjects[uIndex];

	if (lpIOContext->nSentBytes < 3)
	{
		return true;
	}

	lOfs = 0;
	size = 0;
	xcode = 0;
	recvbuf = lpIOContext->Buffer;

	int recvsize = lpIOContext->nSentBytes;

	BYTE byDec[9216];

	while (true)
	{
		if (recvbuf[lOfs] == 0xC1 ||
			recvbuf[lOfs] == 0xC3)
		{
			PBMSG_HEAD* lphead = (PBMSG_HEAD*)(recvbuf + lOfs);
			size = lphead->size;
			headcode = lphead->headcode;
			xcode = recvbuf[lOfs];
		}
		else if (recvbuf[lOfs] == 0xC2 ||
			recvbuf[lOfs] == 0xC4)
		{
			PWMSG_HEAD* lphead = (PWMSG_HEAD*)(recvbuf + lOfs);
			size = ((WORD)(lphead->sizeH) << 8);
			size |= (WORD)(lphead->sizeL);
			headcode = lphead->headcode;
			xcode = recvbuf[lOfs];
		}

		else
		{
			sLog->outBasic("error-L1 : Header error (%s %d)lOfs:%d, size:%d",
				__FILE__, __LINE__,
				lOfs,
				lpIOContext->nSentBytes);

			lpIOContext->nSentBytes = 0;
			return FALSE;
		}

		if (size <= 0)
		{
			sLog->outBasic("error-L1 : size %d",
				size);

			return false;
		}

		if (size <= lpIOContext->nSentBytes)
		{
			if (xcode == 0xC3)
			{
				int ret = g_PacketEncrypt.Decrypt(&byDec[2], &recvbuf[lOfs + 2], size - 2);

				if (ret < 0)
				{
					sLog->outError("[%s][Packet-Decrypt BYTE] Error: ret < 0 %x/%x/%x)", lpUser->ConnectUser->IP, recvbuf[lOfs], recvbuf[lOfs + 1], recvbuf[lOfs + 2]);
				}

				else
				{
					BYTE* pDecBuf = byDec;

					headcode = pDecBuf[2];
					byDec[0] = 0xC1;
					byDec[1] = ret + 2;
					lpUser->PacketsPerSecond++;

					if (lpUser->PacketsPerSecond >= g_ConfigRead.PacketLimit)
					{
						sLog->outError("[ANTI-HACK] Packets Per Second: %d / %d", lpUser->PacketsPerSecond, g_ConfigRead.PacketLimit);
						//GSProtocol.GCSendDisableReconnect(uIndex);
						IOCP.CloseClient(uIndex);
						return 0;
					}

					CStreamPacketEngine_Server PacketStream;
					PacketStream.Clear();

					if (PacketStream.AddData(byDec, ret + 2) == 0)
					{

						sLog->outError("error-L1 : CStreamPacketEngine Adding Error : ip = %s index:%s HEAD:%x (%s,%d)",
							lpUser->ConnectUser->IP,
							lpUser->ConnectUser->Index,
							headcode,
							__FILE__, __LINE__);

						return 0;
					}

					if (PacketStream.ExtractPacket(byDec) != 0)
					{
						sLog->outError("error-L2 : CStreamPacketEngine Adding Error : ip = %s index:%s HEAD:%x (%s,%d)",
							lpUser->ConnectUser->IP,
							lpUser->ConnectUser->Index,
							headcode,
							__FILE__, __LINE__);
						return 0;
					}

					//ProtocolCore(headcode, byDec, ret, *lpObj, 1);
				}
			}

			else if (xcode == 0xC4)
			{
				int ret = g_PacketEncrypt.Decrypt(&byDec[3], &recvbuf[lOfs + 3], size - 3);

				if (ret < 0)
				{
					sLog->outError("[Packet-Decrypt WORD] Error: ret < 0 %x/%x/%x)", recvbuf[lOfs], recvbuf[lOfs + 1], recvbuf[lOfs + 2]);
				}

				else
				{
					BYTE* pDecBuf = byDec;

					headcode = pDecBuf[3];
					byDec[0] = 0xC2;
					WORD size = (ret & 0xFFFF) + 3;
					byDec[1] = SET_NUMBERH(size);
					byDec[2] = SET_NUMBERL(size);

					lpObjUser->PacketsPerSecond++;
					if (lpObjUser->PacketsPerSecond >= g_ConfigRead.PacketLimit)
					{
						sLog->outError("[ANTI-HACK] Packets Per Second: %d / %d", lpObjUser->PacketsPerSecond, g_ConfigRead.PacketLimit);
						//GSProtocol.GCSendDisableReconnect(uIndex);
						IOCP.CloseClient(uIndex);
						return 0;
					}

					CStreamPacketEngine_Server PacketStream;

					PacketStream.Clear();
					if (PacketStream.AddData(byDec, ret + 3) == 0)
					{
						sLog->outError("error-L1 : CStreamPacketEngine Adding Error : ip = %s account:%s name:%s HEAD:%x (%s,%d) State:%d",
							lpObjUser->ConnectUser->IP, lpObj->AccountID, lpObj->Name, headcode, __FILE__, __LINE__, lpObj->Connected);
						return false;
					}

					if (PacketStream.ExtractPacket(byDec) != 0)
					{
						sLog->outError("error-L1 : CStreamPacketEngine ExtractPacket Error : ip = %s account:%s name:%s HEAD:%x (%s,%d) State:%d",
							lpObjUser->ConnectUser->IP, lpObj->AccountID, lpObj->Name, headcode, __FILE__, __LINE__, lpObj->Connected);
						return false;
					}

					ProtocolCore(headcode, byDec, ret, *lpObj, 1);
				}
			}
			else
			{
				CStreamPacketEngine_Server ps;
				ps.Clear();

				if (ps.AddData(&recvbuf[lOfs], size) == 0)
				{
					sLog->outError("error-L1 : CStreamPacketEngine Adding Error : ip = %s account:%s name:%s HEAD:%x (%s,%d) State:%d",
						lpObjUser->ConnectUser->IP, lpObj->AccountID, lpObj->Name, headcode, __FILE__, __LINE__, lpObj->Connected);
					return 0;
				}

				if (ps.ExtractPacket(byDec) != 0)
				{
					sLog->outError("error-L1 : CStreamPacketEngine ExtractPacket Error : ip = %s account:%s name:%s HEAD:%x (%s,%d) State:%d",
						lpObjUser->ConnectUser->IP, lpObj->AccountID, lpObj->Name, headcode, __FILE__, __LINE__, lpObj->Connected);
					return 0;
				}

				lpObjUser->PacketsPerSecond++;
				if (lpObjUser->PacketsPerSecond >= g_ConfigRead.PacketLimit)
				{
					sLog->outError("[ANTI-HACK] Packets Per Second: %d / %d", lpObjUser->PacketsPerSecond, g_ConfigRead.PacketLimit);
					GCSendDisableReconnect(*getGameObject(uIndex));
					//IOCP.CloseClient(uIndex);
					return 0;
				}

				ProtocolCore(headcode, byDec, size, *getGameObject(uIndex), 0); // here
			}

			lOfs += size; // wait
			lpIOContext->nSentBytes -= size;

			if (lpIOContext->nSentBytes <= 0)
			{
				break;
			}
		}
		else if (lOfs > 0)
		{
			if (lpIOContext->nSentBytes < 1)
			{
				sLog->outBasic("error-L1 : recvbuflen 1 %s %d", __FILE__, __LINE__);
				break;
			}

			if (lpIOContext->nSentBytes < MAX_IO_BUFFER_SIZE)
			{
				std::memcpy(recvbuf, &recvbuf[lOfs], lpIOContext->nSentBytes);
				sLog->outBasic("Message copy %d", lpIOContext->nSentBytes);
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

bool CIOCP::DataSend(int uIndex, LPBYTE lpMsg, DWORD dwSize, bool Encrypt)
{
	unsigned long SendBytes;
	_PER_SOCKET_CONTEXT * lpPerSocketContext;
	BYTE * SendBuf;
	BYTE BUFFER[65535];
	CGameObject* lpObj = getGameObjectFromUser(uIndex);

	EnterCriticalSection(&criti);

	if (gStalkProtocol)
	{
		if (gStalkProtocolId[0] == lpObj->AccountID[0])
		{
			if (gStalkProtocolId[1] == lpObj->AccountID[1])
			{
				if (!strcmp(gStalkProtocolId, lpObj->AccountID))
				{
					sLog->outMessage("general", LOG_LEVEL_DEBUG, "DATA SEND %s %d", lpMsg, dwSize);
				}
			}
		}
	}

	if (lpMsg[0] != 0xC1 && lpMsg[0] != 0xC2 &&
		lpMsg[0] != 0xC3 && lpMsg[0] != 0xC4)
	{
		sLog->outError("[ERROR] Trying to send packet without HEADER (%s)(%s)", lpObj->AccountID, lpObj->Name);
		LeaveCriticalSection(&criti);
		return FALSE;
	}

#ifdef EMU_NOCRYPT
	if (lpMsg[0] == 0xC3)
		lpMsg[0] = 0xC1;
	if (lpMsg[0] == 0xC4)
		lpMsg[0] = 0xC2;
#endif

#ifdef C3C4_DISABLECRYPT
	if (lpMsg[0] == 0xC3)
		lpMsg[0] = 0xC1;
	if (lpMsg[0] == 0xC4)
		lpMsg[0] = 0xC2;
#endif

	if (lpMsg[0] == 0xC3 || lpMsg[0] == 0xC4)
	{
		int ret;
		BYTE btsize;

		if (lpMsg[0] == 0xC3)
		{
			btsize = lpMsg[1];
			ret = g_PacketEncrypt.Encrypt(&ExSendBuf[2], &lpMsg[1], dwSize - 1);
			ExSendBuf[0] = 0xC3;
			ExSendBuf[1] = ret + 2;
			SendBuf = ExSendBuf;
			dwSize = ret + 2;
			lpMsg[1] = btsize;
		}
		else
		{
			btsize = lpMsg[2];
			ret = g_PacketEncrypt.Encrypt(&ExSendBuf[3], &lpMsg[2], dwSize - 2);
			ExSendBuf[0] = 0xC4;
			ExSendBuf[1] = SET_NUMBERH(ret + 3);
			ExSendBuf[2] = SET_NUMBERL(ret + 3);
			SendBuf = ExSendBuf;
			dwSize = ret + 3;
		}
	}

	else
	{
		SendBuf = lpMsg;
	}

	if (lpObj->Connected < PLAYER_CONNECTED)
	{
		LeaveCriticalSection(&criti);
		return false;
	}

	lpPerSocketContext = lpObj->m_PlayerData->ConnectUser->PerSocketContext;

	if (dwSize > sizeof(lpPerSocketContext->IOContext[0].Buffer))
	{
		sLog->outBasic("Error : Max msg(%d) %s %d", dwSize, __FILE__, __LINE__);
		CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
		LeaveCriticalSection(&criti);
		return false;
	}

	_PER_IO_CONTEXT  * lpIoCtxt;

	lpIoCtxt = &lpPerSocketContext->IOContext[1];

	if (lpIoCtxt->nWaitIO > 0)
	{
		if ((lpIoCtxt->nSecondOfs + dwSize) > MAX_IO_BUFFER_SIZE - 1)
		{
			sLog->outBasic("(%d)error-L2 MAX BUFFER OVER %d %d %d [%s][%s]", lpObj->m_Index, lpIoCtxt->nTotalBytes, lpIoCtxt->nSecondOfs, dwSize, lpObj->AccountID, lpObj->Name);
			lpIoCtxt->nWaitIO = 0;
			CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
			LeaveCriticalSection(&criti);
			return true;
		}

		std::memcpy(&lpIoCtxt->BufferSecond[lpIoCtxt->nSecondOfs], SendBuf, dwSize);
		lpIoCtxt->nSecondOfs += dwSize;
		LeaveCriticalSection(&criti);
		return true;
	}

	lpIoCtxt->nTotalBytes = 0;

	if (lpIoCtxt->nSecondOfs > 0)
	{
		std::memcpy(lpIoCtxt->Buffer, lpIoCtxt->BufferSecond, lpIoCtxt->nSecondOfs);
		lpIoCtxt->nTotalBytes = lpIoCtxt->nSecondOfs;
		lpIoCtxt->nSecondOfs = 0;
	}

	if ((lpIoCtxt->nTotalBytes + dwSize) > MAX_IO_BUFFER_SIZE - 1)
	{
		sLog->outBasic("(%d)error-L2 MAX BUFFER OVER %d %d [%s][%s]", lpObj->m_Index, lpIoCtxt->nTotalBytes, dwSize, lpObj->AccountID, lpObj->Name);
		lpIoCtxt->nWaitIO = 0;
		CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
		LeaveCriticalSection(&criti);
		return false;
	}

	std::memcpy(&lpIoCtxt->Buffer[lpIoCtxt->nTotalBytes], SendBuf, dwSize);
	lpIoCtxt->nTotalBytes += dwSize;
	lpIoCtxt->m_wsabuf.buf = (char*)&lpIoCtxt->Buffer;
	lpIoCtxt->m_wsabuf.len = lpIoCtxt->nTotalBytes;
	lpIoCtxt->nSentBytes = 0;
	lpIoCtxt->IOOperation = 1;

	if (WSASend(lpObj->m_socket, lpIoCtxt.m_wsabuf, 1, &SendBytes, 0, lpIoCtxt.m_Overlapped, NULL) == -1)
	{

		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			lpIoCtxt->nWaitIO = 0;

			if (lpIoCtxt->m_wsabuf.buf[0] == 0xC1)
			{
				sLog->outBasic("(%d)WSASend(%d) failed with error [%x][%x] %d %s ", __LINE__, lpObj->m_Index, (BYTE)lpIoCtxt->m_wsabuf.buf[0],
					(BYTE)lpIoCtxt->m_wsabuf.buf[2], WSAGetLastError(), lpObj->m_PlayerData->ConnectUser->IP);
			}
			else if (lpIoCtxt->m_wsabuf.buf[0] == 0xC2)
			{
				sLog->outBasic("(%d)WSASend(%d) failed with error [%x][%x] %d %s ", __LINE__, lpObj->m_Index, (BYTE)lpIoCtxt->m_wsabuf.buf[0],
					(BYTE)lpIoCtxt->m_wsabuf.buf[3], WSAGetLastError(), lpObj->m_PlayerData->ConnectUser->IP);
			}
			CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
			LeaveCriticalSection(&criti);
			return false;
		}
	}
	else
	{
		lpPerSocketContext->dwIOCount++;
	}

	lpIoCtxt->nWaitIO = 1;
	LeaveCriticalSection(&criti);
	return true;
}



bool CIOCP::IoSendSecond(_PER_SOCKET_CONTEXT * lpPerSocketContext)
{
	unsigned long SendBytes;
	_PER_IO_CONTEXT * lpIoCtxt;

	EnterCriticalSection(&criti);
	int userIndex = lpPerSocketContext->nIndex;
	CGameObject* lpObj = getGameObjectFromUser(userIndex);

	lpIoCtxt = &lpPerSocketContext->IOContext[1];

	if (lpIoCtxt->nWaitIO > 0)
	{
		LeaveCriticalSection(&criti);
		return false;
	}

	lpIoCtxt->nTotalBytes = 0;
	if (lpIoCtxt->nSecondOfs > 0)
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

	if (WSASend(lpObj->m_socket, &lpIoCtxt->m_wsabuf, 1, &SendBytes, 0, &lpIoCtxt->m_Overlapped, NULL) == -1)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			sLog->outBasic("WSASend(%d) failed with error %d %s ", __LINE__, WSAGetLastError(), lpObj->m_PlayerData->ConnectUser->IP);
			CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
			LeaveCriticalSection(&criti);
			return false;
		}
	}
	else
	{
		lpPerSocketContext->dwIOCount++;
	}

	lpIoCtxt->nWaitIO = 1;
	LeaveCriticalSection(&criti);

	return true;
}

bool CIOCP::IoMoreSend(_PER_SOCKET_CONTEXT * lpPerSocketContext)
{
	unsigned long SendBytes;
	CGameObject* lpObj;
	_PER_IO_CONTEXT * lpIoCtxt;

	EnterCriticalSection(&criti);
	CGameObject* lpObj = getGameObjectFromUser(lpPerSocketContext->nIndex);
	lpIoCtxt = &lpPerSocketContext->IOContext[1];

	if ((lpIoCtxt->nTotalBytes - lpIoCtxt->nSentBytes) < 0)
	{
		LeaveCriticalSection(&criti);
		return false;
	}

	lpIoCtxt->m_wsabuf.buf = (char*)&lpIoCtxt->Buffer[lpIoCtxt->nSentBytes];
	lpIoCtxt->m_wsabuf.len = lpIoCtxt->nTotalBytes - lpIoCtxt->nSentBytes;
	lpIoCtxt->IOOperation = 1;

	if (WSASend(lpObj->m_socket, lpIoCtxt->m_wsabuf, 1, &SendBytes, 0, lpIoCtxt->m_Overlapped, NULL) == -1)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			sLog->outBasic("WSASend(%d) failed with error %d %s ", __LINE__, WSAGetLastError(), lpObj->m_PlayerData->ConnectUser->IP);
			CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
			LeaveCriticalSection(&criti);
			return false;
		}
	}
	else
	{
		lpPerSocketContext->dwIOCount++;
	}

	lpIoCtxt->nWaitIO = 1;
	LeaveCriticalSection(&criti);
	return true;
}

void CIOCP::CloseClient(_PER_SOCKET_CONTEXT * lpPerSocketContext, int result)
{
	int index = -1;
	index = lpPerSocketContext->nIndex;
	CGameObject *lpObj = getGameObjectFromUser(index);

	if (lpObj->m_bOff)
	{
		return;
	}

	if (lpObj->m_socket != INVALID_SOCKET)
	{
		if (closesocket(lpObj->m_socket) == -1)
		{
			if (WSAGetLastError() != WSAENOTSOCK)
			{
				return;
			}
		}
		lpObj->m_socket = INVALID_SOCKET;
	}
	gObjDel(*lpObj);
}


void CIOCP::CloseClient(int index) // go to game
{
	CGameObject* lpObj = getGameObjectFromUser(index);
	if (lpObj->Connected == PLAYER_EMPTY)
	{
		return;
	}

	EnterCriticalSection(&criti);

	if (lpObj->m_socket != INVALID_SOCKET && lpObj->m_bOff == false)
	{
		closesocket(lpObj->m_socket);
		lpObj->m_socket = INVALID_SOCKET;
	}

	LeaveCriticalSection(&criti);
}

void CIOCP::ResponErrorCloseClient(int index)
{
	if (getGameObject(index)->Connected == PLAYER_EMPTY)
	{
		return;
	}

	EnterCriticalSection(&criti);

	closesocket(getGameObject(index)->m_socket);
	getGameObject(index)->m_socket = INVALID_SOCKET;
	gObjDel(*getGameObject(index));

	LeaveCriticalSection(&criti);
}
