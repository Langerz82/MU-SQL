// This IOCP interface is for both the GameServer and LoginServer.

#ifndef _MU_IOCP_H
#define _MU_IOCP_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "CQueue.h"
#include "SocketBuffer/BufferedSocket.h"

#include <ace/Get_Opt.h>
#include <ace/Dev_Poll_Reactor.h>
#include <ace/TP_Reactor.h>
#include <ace/ACE.h>
#include <ace/Acceptor.h>
#include <ace/SOCK_Acceptor.h>

class BufferedSocket;

struct _PER_IO_CONTEXT;
struct _PER_SOCKET_CONTEXT;

class CIOCP: public BufferedSocket
{
public:

	void GiocpInit();
	void GiocpDelete();
	void DestroyGIocp();
	bool CreateListenSocket(WORD uiPort, LPSTR ipAddress);
	bool RecvDataParse(_PER_IO_CONTEXT * lpIOContext, int uIndex);
	bool DataSend(int uIndex, LPBYTE lpMsg, DWORD dwSize, bool Encrypt = true);
	bool IoSendSecond(_PER_SOCKET_CONTEXT * lpPerSocketContext);
	bool IoMoreSend(_PER_SOCKET_CONTEXT * lpPerSocketContext);
	void CloseClient(_PER_SOCKET_CONTEXT * lpPerSocketContext, int result);
	void CloseClient(int index);
	void ResponErrorCloseClient(int index);

	void OnAccept(void) override;
	void OnRead(void) override;
	void OnClose(void) override;

	inline int getUserDataIndex(const char* socketKey)
	{
		std::map<const char*, int>::iterator pUO = g_UserIDMap.find(socketKey);
		if (pUO == g_UserIDMap.end())
		{
			sLog->outError("UserObject does not exist. %s %d\n%s", __FILE__, __LINE__);
			return -1;
		}
		else
			return pUO->second;
	};

private:

	BYTE* ExSendBuf;
	int g_ServerPort;
	HANDLE g_IocpThreadHandle;
	HANDLE g_ThreadHandles[MAX_IO_THREAD_HANDLES];
	CRITICAL_SECTION criti;
	enum SOCKET_FLAG;
	DWORD g_dwThreadCount;
	HANDLE g_CompletionPort;
	SOCKET g_Listen;
	ACE_Acceptor<BufferedSocket, ACE_SOCK_Acceptor> g_buffSocket;
	std::map<const char*, int> g_UserIDMap;

	static void IocpServerWorkerEP(void *pThis)
	{
		CIOCP *pt = (CIOCP*)pThis;
		pt->IocpServerWorker(pThis);
	}

	static void ServerWorkerThreadEP(void *pThis)
	{
		CIOCP *pt = (CIOCP*)pThis;
		pt->ServerWorkerThread();
	}

	DWORD IocpServerWorker(void * p);
	DWORD ServerWorkerThread();

};

extern CRITICAL_SECTION scriti;

BOOL IocpServerStart();
BOOL CreateGIocp(int server_port, eSERVER_TYPE eServerType);
void DestroyGIocp();
BOOL CreateListenSocket(DWORD dwServerCountIndex);
unsigned long __stdcall IocpServerWorker(DWORD pIocpServerParameter);
unsigned long __stdcall ServerWorkerThread(HANDLE CompletionPortID);
BOOL RecvDataParse(_PER_IO_CONTEXT * lpIOContext, int uIndex);
BOOL DataSend(int uIndex, LPBYTE lpMsg, DWORD dwSize, char* szFunction = nullptr);
BOOL IoSendSecond(_PER_SOCKET_CONTEXT * lpPerSocketContext);
BOOL IoMoreSend(_PER_SOCKET_CONTEXT * lpPerSocketContext);
//bool UpdateCompletionPort(char* sockKey, int ClientIndex, BOOL bAddToList);

void CloseClient(_PER_SOCKET_CONTEXT * lpPerSocketContext, int result);
void CloseClient(int index);
void ResponErrorCloseClient(int index);


/*
extern CQueue m_DSQueue1;
extern CQueue m_DSQueue2;
extern CQueue m_DSQueue3;
extern CQueue m_DSQueue4;
extern CQueue m_DSQueue5;
extern CQueue m_JSQueue;
extern CQueue m_EXDSQueue;
*/

extern CIOCP IOCP;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

