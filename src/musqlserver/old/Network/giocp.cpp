#include "IOCP.h"
#include "PacketEngineServer.h" // Encryption/Decryption
#include "GameMain.h"
#include "GameServer.h"
#include "PacketEncrypt.h"
#include "FloodSystem.h"
#include "GOFunctions.h"

CIOCP IOCP;

void CIOCP::GiocpInit()
{
	ExSendBuf = new BYTE[MAX_EXSENDBUF_SIZE];
}

void CIOCP::GiocpDelete()
{
	delete[] ExSendBuf;
}

bool CIOCP::CreateGIocp(int server_port)
{
	unsigned long ThreadID;

	g_ServerPort = server_port;

	InitializeCriticalSection(&criti);

	g_IocpThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)IocpServerWorkerEP, this, 0, &ThreadID);

	if (g_IocpThreadHandle == 0)
	{
		sLog->outBasic("CreateThread() failed with error %d", GetLastError());
		return 0;
	}
	else
	{
		return 1;
	}
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

bool CIOCP::CreateListenSocket()
{
	sockaddr_in InternetAddr;
	int nRet;

	g_Listen = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (g_Listen == -1)
	{
		sLog->outBasic("WSASocket() failed with error %d", WSAGetLastError());
		return 0;
	}
	else
	{
		InternetAddr.sin_family = AF_INET;
		InternetAddr.sin_addr.S_un.S_addr = htonl(0);
		InternetAddr.sin_port = htons(g_ServerPort);
		nRet = ::bind(g_Listen, (sockaddr*)&InternetAddr, 16);

		if (nRet == -1)
		{
			sLog->outError("Bind error - server cannot be launched twice. Please terminate game server and restart.");
			SendMessage(ghWnd, WM_CLOSE, 0, 0);	// Kill aplication
			return 0;
		}
		else
		{
			nRet = listen(g_Listen, 5);
			if (nRet == -1)
			{
				sLog->outBasic("listen() failed with error %d", WSAGetLastError());
				return 0;
			}
			else
			{
				return 1;
			}
		}
	}
}

DWORD CIOCP::IocpServerWorker(void * p)
{
	SYSTEM_INFO SystemInfo;
	DWORD ThreadID;
	SOCKET Accept;
	int nRet;
	int ClientIndex;
	sockaddr_in cAddr;
	in_addr cInAddr;
	int cAddrlen;
	_PER_SOCKET_CONTEXT * lpPerSocketContext;
	int RecvBytes;
	unsigned long Flags;



	cAddrlen = 16;
	lpPerSocketContext = 0;
	Flags = 0;

	GetSystemInfo(&SystemInfo);

	g_dwThreadCount = SystemInfo.dwNumberOfProcessors * 2;

	if (g_dwThreadCount > MAX_IO_THREAD_HANDLES)
	{
		g_dwThreadCount = MAX_IO_THREAD_HANDLES;
		sLog->outBasic("[WARNING]: IOCP Thread Handles set to 16");
	}

	__try
	{

		g_CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);

		if (g_CompletionPort == NULL)
		{
			sLog->outBasic("CreateIoCompletionPort failed with error: %d", GetLastError());
			__leave;
		}

		for (int n = 0; n < g_dwThreadCount; n++)
		{
			HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ServerWorkerThreadEP, this, 0, &ThreadID);

			if (hThread == 0)
			{
				sLog->outBasic("CreateThread() failed with error %d", GetLastError());
				__leave;
			}

			g_ThreadHandles[n] = hThread;

			CloseHandle(hThread);
		}

		if (CreateListenSocket() == 0)
		{

		}
		else
		{
			while (true)
			{
				Accept = WSAAccept(g_Listen, (sockaddr*)&cAddr, &cAddrlen, NULL, 0);

				if (Accept == -1)
				{
					EnterCriticalSection(&criti);
					sLog->outBasic("WSAAccept() failed with error %d", WSAGetLastError());
					LeaveCriticalSection(&criti);
					continue;
				}

				EnterCriticalSection(&criti);
				memcpy(&cInAddr, &cAddr.sin_addr, sizeof(cInAddr));

				ClientIndex = gObjAddSearch(Accept, inet_ntoa(cInAddr));

#if FILE_FLOOD_SYSTEMSWITCH
				if (AntiFlood.Enabled == 1)
				{
					if (AntiFlood.Check(inet_ntoa(cInAddr)) == false)
					{
						closesocket(Accept);
						LeaveCriticalSection(&criti);
						continue;
					}
				}
#endif

				/*if (ClientIndex == -1 || !ObjectMaxRange(ClientIndex))
				{
					sLog->outError("error-L2 : ClientIndex = -1");
					closesocket(Accept);
					LeaveCriticalSection(&criti);
					continue;
				}*/

				if (UpdateCompletionPort(Accept, ClientIndex, 1) == 0)
				{
					sLog->outError("error-L1 : %d %d CreateIoCompletionPort failed with error %d", Accept, ClientIndex, GetLastError());
					closesocket(Accept);
					LeaveCriticalSection(&criti);
					continue;
				}

				if (gObjInit() == -1)
				{
					sLog->outError("error-L1 : %d %d gObjAdd() failed with error %d", Accept, ClientIndex, GetLastError());
					LeaveCriticalSection(&criti);
					closesocket(Accept);
					continue;
				}
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
				sockCtx->m_socket = Accept;
				sockCtx->nIndex = ClientIndex;

				/* TODO
				nRet = WSARecv(Accept, sockCtx->IOContext[0].m_wsabuf.buf, 1, (unsigned long*)&RecvBytes, &Flags,
					sockCtx->IOContext[0].m_Overlapped.Internal, NULL);
				*/

				if (nRet == -1)
				{
					if (WSAGetLastError() != WSA_IO_PENDING)
					{
						sLog->outError("error-L1 : WSARecv() failed with error %d", WSAGetLastError());
						sockCtx->IOContext[0].nWaitIO = 4;
						CloseClient(sockCtx, 0);
						LeaveCriticalSection(&criti);
						continue;
					}
				}

				sockCtx->IOContext[0].nWaitIO = 1;
				sockCtx->dwIOCount++;

				LeaveCriticalSection(&criti);
				SCPJoinResultSend(*lpObj, 1);
			}
		}
	}
	__finally
	{

		if (g_CompletionPort != NULL)
		{
			for (int i = 0; i < g_dwThreadCount; i++)
			{
				PostQueuedCompletionStatus(g_CompletionPort, 0, 0, 0);
			}
		}

		if (g_CompletionPort != NULL)
		{
			CloseHandle(g_CompletionPort);
			g_CompletionPort = NULL;
		}
		if (g_Listen != INVALID_SOCKET)
		{
			closesocket(g_Listen);
			g_Listen = INVALID_SOCKET;
		}
	}

	return 1;

}

DWORD CIOCP::ServerWorkerThread()
{
	HANDLE CompletionPort;
	DWORD dwIoSize;
	unsigned long RecvBytes;
	unsigned long Flags;
	DWORD dwSendNumBytes;
	BOOL bSuccess;
	int nRet;
#ifdef _WIN64
	ULONGLONG ClientIndex;
#else
	ULONG ClientIndex;
#endif
	_PER_SOCKET_CONTEXT * lpPerSocketContext;
	LPOVERLAPPED lpOverlapped;
	_PER_IO_CONTEXT * lpIOContext;


	CompletionPort = this->g_CompletionPort;
	dwSendNumBytes = 0;
	bSuccess = 0;
	lpPerSocketContext = 0;
	lpOverlapped = 0;
	lpIOContext = 0;

	while (true)
	{
		// TODO - Find Equiv.
		bSuccess = GetQueuedCompletionStatus(CompletionPort, &dwIoSize, &ClientIndex, &lpOverlapped, -1); // WAIT_FOREVER

		CGameObject* lpObj = getGameObject(ClientIndex);
		if (lpObj->m_PlayerData->ConnectUser->socket == INVALID_SOCKET)
		{
			continue;
		}
		if (bSuccess == 0)
		{
			if (lpOverlapped != 0)
			{
				int aError = GetLastError();
				if ((aError != ERROR_NETNAME_DELETED) && (aError != ERROR_CONNECTION_ABORTED) && (aError != ERROR_OPERATION_ABORTED) && (aError != ERROR_SEM_TIMEOUT) && (aError != ERROR_HOST_UNREACHABLE))
				{
					EnterCriticalSection(&criti);
					sLog->outBasic("Error Thread : GetQueueCompletionStatus( %d )", GetLastError());
					LeaveCriticalSection(&criti);
					return 0;
				}
			}
		}

		EnterCriticalSection(&criti);

		lpPerSocketContext = lpObj->m_PlayerData->ConnectUser->PerSocketContext;
		lpPerSocketContext->dwIOCount--;

		if (dwIoSize == 0)
		{
			CloseClient(lpPerSocketContext, 0);
			LeaveCriticalSection(&criti);
			continue;
		}

		lpIOContext = (_PER_IO_CONTEXT *)lpOverlapped;

		if (lpIOContext == 0)
		{
			LeaveCriticalSection(&criti);
			continue;
		}

		if (lpIOContext->IOOperation == 1)
		{
			lpIOContext->nSentBytes += dwIoSize;

			if (lpIOContext->nSentBytes >= lpIOContext->nTotalBytes)
			{
				lpIOContext->nWaitIO = 0;

				if (lpIOContext->nSecondOfs > 0)
				{
					IoSendSecond(lpPerSocketContext);
				}
			}
			else
			{
				IoMoreSend(lpPerSocketContext);
			}

		}
		else if (lpIOContext->IOOperation == 0)
		{
			RecvBytes = 0;
			lpIOContext->nSentBytes += dwIoSize;

			if (RecvDataParse(lpIOContext, lpPerSocketContext->nIndex) == 0)
			{
				sLog->outBasic("error-L1 : Socket Header error %d, %d", WSAGetLastError(), lpPerSocketContext->nIndex);
				CloseClient(lpPerSocketContext, 0);
				LeaveCriticalSection(&criti);
				continue;
			}

			lpIOContext->nWaitIO = 0;
			Flags = 0;
			memset(&lpIOContext->m_Overlapped, 0, sizeof(WSAOVERLAPPED));
			lpIOContext->m_wsabuf.len = MAX_IO_BUFFER_SIZE - lpIOContext->nSentBytes;
			lpIOContext->m_wsabuf.buf = (char*)&lpIOContext->Buffer[lpIOContext->nSentBytes];
			lpIOContext->IOOperation = 0;
			if (lpPerSocketContext->m_socket != INVALID_SOCKET)
			{
				try
				{
					// TODO find equiv.
					nRet = WSARecv(lpPerSocketContext->m_socket, *lpIOContext->m_wsabuf, 1, &RecvBytes, &Flags,
						&lpIOContext->m_Overlapped, NULL);
				}
				catch (const std::exception &e)
				{
					char text[1000];
					sprintf(text, "Exception %s was detected", e.what());
					MessageBoxA(0, text, 0, MB_OK | MB_TOPMOST);
					///LeaveCriticalSection(&criti);
					//continue;
				}
			}
			else
			{
				LeaveCriticalSection(&criti);
				continue;
			}

			if (nRet == -1)
			{
				if (WSAGetLastError() != WSA_IO_PENDING)
				{
					sLog->outBasic("WSARecv() failed with error %d", WSAGetLastError());
					CloseClient(lpPerSocketContext, 0);
					LeaveCriticalSection(&criti);
					continue;
				}
			}

			lpPerSocketContext->dwIOCount++;
			lpIOContext->nWaitIO = 1;
		}
		LeaveCriticalSection(&criti);

	}

	return 1;
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
	CGameObject* lpObj = getGameObject(uIndex);
	CUserData* lpObjUser = getGameObject(uIndex)->m_PlayerData;

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
					sLog->outError("[%s][Packet-Decrypt BYTE] Error: ret < 0 %x/%x/%x)", lpObjUser->ConnectUser->IP, recvbuf[lOfs], recvbuf[lOfs + 1], recvbuf[lOfs + 2]);
				}

				else
				{
					BYTE* pDecBuf = byDec;

					headcode = pDecBuf[2];
					byDec[0] = 0xC1;
					byDec[1] = ret + 2;
					lpObjUser->PacketsPerSecond++;

					if (lpObjUser->PacketsPerSecond >= g_ConfigRead.PacketLimit)
					{
						sLog->outError("[ANTI-HACK] Packets Per Second: %d / %d", lpObjUser->PacketsPerSecond, g_ConfigRead.PacketLimit);
						//gGameProtocol.GCSendDisableReconnect(uIndex);
						IOCP.CloseClient(uIndex);
						return 0;
					}

					CStreamPacketEngine_Server PacketStream;
					PacketStream.Clear();

					if (PacketStream.AddData(byDec, ret + 2) == 0)
					{

						sLog->outError("error-L1 : CStreamPacketEngine Adding Error : ip = %s account:%s name:%s HEAD:%x (%s,%d) State:%d",
							lpObjUser->ConnectUser->IP,
							lpObjUser->ConnectUser->Index, lpObj->Name,
							headcode,
							__FILE__, __LINE__,
							lpObj->Connected);

						return 0;
					}

					if (PacketStream.ExtractPacket(byDec) != 0)
					{
						sLog->outError("error-L1 : CStreamPacketEngine ExtractPacket Error : ip = %s account:%s name:%s HEAD:%x (%s,%d) State:%d",
							lpObjUser->ConnectUser->IP, lpObjUser->ConnectUser->Index, lpObj->Name, headcode, __FILE__, __LINE__, lpObj->Connected);

						return 0;
					}

					ProtocolCore(headcode, byDec, ret, *lpObj, 1);
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
						//gGameProtocol.GCSendDisableReconnect(uIndex);
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
				memcpy(recvbuf, &recvbuf[lOfs], lpIOContext->nSentBytes);
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

bool CIOCP::DataSend(int aIndex, BYTE* lpMsg, DWORD dwSize, bool Encrypt)
{
	unsigned long SendBytes;
	_PER_SOCKET_CONTEXT * lpPerSocketContext;
	BYTE * SendBuf;
	BYTE BUFFER[65535];
	CGameObject* lpObj = getGameObject(aIndex);


	EnterCriticalSection(&criti);

	if (ObjectMaxRange(aIndex) == FALSE)
	{
		sLog->outBasic("error-L2 : Index(%d) %x %x %x ", dwSize, lpMsg[0], lpMsg[1], lpMsg[2]);
		LeaveCriticalSection(&criti);
		return false;
	}

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

	lpPerSocketContext = *lpObj->m_PlayerData->ConnectUser.PerSocketContext;

	if (dwSize > sizeof(lpPerSocketContext->IOContext[0].Buffer))
	{
		sLog->outBasic("Error : Max msg(%d) %s %d", dwSize, __FILE__, __LINE__);
		CloseClient(aIndex);
		LeaveCriticalSection(&criti);
		return false;
	}

	_PER_IO_CONTEXT  * lpIoCtxt;

	lpIoCtxt = &lpPerSocketContext->IOContext[1];

	if (lpIoCtxt->nWaitIO > 0)
	{
		if ((lpIoCtxt->nSecondOfs + dwSize) > MAX_IO_BUFFER_SIZE - 1)
		{
			sLog->outBasic("(%d)error-L2 MAX BUFFER OVER %d %d %d [%s][%s]", aIndex, lpIoCtxt->nTotalBytes, lpIoCtxt->nSecondOfs, dwSize, lpObj->AccountID, lpObj->Name);
			lpIoCtxt->nWaitIO = 0;
			CloseClient(aIndex);
			LeaveCriticalSection(&criti);
			return true;
		}

		memcpy(&lpIoCtxt->BufferSecond[lpIoCtxt->nSecondOfs], SendBuf, dwSize);
		lpIoCtxt->nSecondOfs += dwSize;
		LeaveCriticalSection(&criti);
		return true;
	}

	lpIoCtxt->nTotalBytes = 0;

	if (lpIoCtxt->nSecondOfs > 0)
	{
		memcpy(lpIoCtxt->Buffer, lpIoCtxt->BufferSecond, lpIoCtxt->nSecondOfs);
		lpIoCtxt->nTotalBytes = lpIoCtxt->nSecondOfs;
		lpIoCtxt->nSecondOfs = 0;
	}

	if ((lpIoCtxt->nTotalBytes + dwSize) > MAX_IO_BUFFER_SIZE - 1)
	{
		sLog->outBasic("(%d)error-L2 MAX BUFFER OVER %d %d [%s][%s]", aIndex, lpIoCtxt->nTotalBytes, dwSize, lpObj->AccountID, lpObj->Name);
		lpIoCtxt->nWaitIO = 0;
		CloseClient(aIndex);
		LeaveCriticalSection(&criti);
		return false;
	}

	memcpy(&lpIoCtxt->Buffer[lpIoCtxt->nTotalBytes], SendBuf, dwSize);
	lpIoCtxt->nTotalBytes += dwSize;
	lpIoCtxt->m_wsabuf.buf = (char*)&lpIoCtxt->Buffer;
	lpIoCtxt->m_wsabuf.len = lpIoCtxt->nTotalBytes;
	lpIoCtxt->nSentBytes = 0;
	lpIoCtxt->IOOperation = 1;

	if (WSASend(lpObj->m_socket, lpIoCtxt->m_wsabuf, 1, &SendBytes, 0, lpIoCtxt->m_Overlapped, NULL) == -1)
	{

		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			lpIoCtxt->nWaitIO = 0;

			if (lpIoCtxt->m_wsabuf.buf[0] == 0xC1)
			{
				sLog->outBasic("(%d)WSASend(%d) failed with error [%x][%x] %d %s ", __LINE__, aIndex, (BYTE)lpIoCtxt->wsabuf.buf[0],
					(BYTE)lpIoCtxt->m_wsabuf.buf[2], WSAGetLastError(), lpObj->m_PlayerData->ConnectUser->IP);
			}
			else if (lpIoCtxt->m_wsabuf.buf[0] == 0xC2)
			{
				sLog->outBasic("(%d)WSASend(%d) failed with error [%x][%x] %d %s ", __LINE__, aIndex, (BYTE)lpIoCtxt->wsabuf.buf[0],
					(BYTE)lpIoCtxt->m_wsabuf.buf[3], WSAGetLastError(), lpObj->m_PlayerData->ConnectUser->IP);
			}
			CloseClient(aIndex);
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
	CGameObject* lpObj;
	_PER_IO_CONTEXT * lpIoCtxt;

	EnterCriticalSection(&criti);
	int aIndex = lpPerSocketContext->nIndex;
	lpObj = getGameObject(aIndex);
	lpIoCtxt = &lpPerSocketContext->IOContext[1];

	if (lpIoCtxt->nWaitIO > 0)
	{
		LeaveCriticalSection(&criti);
		return false;
	}

	lpIoCtxt->nTotalBytes = 0;
	if (lpIoCtxt->nSecondOfs > 0)
	{
		memcpy(lpIoCtxt->Buffer, lpIoCtxt->BufferSecond, lpIoCtxt->nSecondOfs);
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
			sLog->outBasic("WSASend(%d) failed with error %d %s ", __LINE__, WSAGetLastError(), lpObjUser->ConnectUser->IP);
			CloseClient(aIndex);
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
	int aIndex = lpPerSocketContext->nIndex;
	lpObj = getGameObject(aIndex);
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
			sLog->outBasic("WSASend(%d) failed with error %d %s ", __LINE__, WSAGetLastError(), lpObjUser->ConnectUser->IP);
			CloseClient(aIndex);
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

bool CIOCP::UpdateCompletionPort(SOCKET sd, int ClientIndex, BOOL bAddToList)
{
	_PER_SOCKET_CONTEXT * lpPerSocketContext = NULL;

	HANDLE cp = CreateIoCompletionPort((HANDLE)sd, g_CompletionPort, ClientIndex, 0);

	if (cp == 0)
	{
		return FALSE;
	}

	getGameObject(ClientIndex)->m_PlayerData->ConnectUser->PerSocketContext->dwIOCount = 0;
	return TRUE;
}

void CIOCP::CloseClient(_PER_SOCKET_CONTEXT * lpPerSocketContext, int result)
{
	int index = -1;
	index = lpPerSocketContext->nIndex;

	if (index >= g_ConfigRead.server.GetObjectStartUserIndex() && index < g_ConfigRead.server.GetObjectMax())
	{
		if (getGameObject(index)->m_bOff)
		{
			return;
		}

		if (getGameObject(index)->m_socket != INVALID_SOCKET)
		{
			if (closesocket(getGameObject(index)->m_socket) == -1)
			{
				if (WSAGetLastError() != WSAENOTSOCK)
				{
					return;
				}
			}
			getGameObject(index)->m_socket = INVALID_SOCKET;
		}

		gObjDel(*getGameObject(index));
	}
}


void CIOCP::CloseClient(int index) // go to game
{
	if (index < 0 || index > g_ConfigRead.server.GetObjectMax() - 1)
	{
		return;
	}

	if (getGameObject(index)->Connected == PLAYER_EMPTY)
	{
		return;
	}

	EnterCriticalSection(&criti);

	if (getGameObject(index)->m_socket != INVALID_SOCKET && getGameObject(index)->m_bOff == false)
	{
		closesocket(getGameObject(index)->m_socket);
		getGameObject(index)->m_socket = INVALID_SOCKET;
	}

	else
	{
		//	closesocket(getGameObject(index)->m_socket );
	}

	LeaveCriticalSection(&criti);
}

void CIOCP::ResponErrorCloseClient(int index)
{
	if (index < 0 || index > g_ConfigRead.server.GetObjectMax() - 1)
	{
		return;
	}

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


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

