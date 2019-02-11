// This IOCP interface is for both the GameServer and LoginServer.

#ifndef _MU_IOCP_H
#define _MU_IOCP_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "generalStructs.h"

#include "CQueue.h"

enum eSERVER_TYPE
{
	ST_NONE = 0,
	ST_JOINSERVER = 1,
	ST_DATASERVER = 2,
	ST_EXDATASERVER = 3,
};

struct _PER_IO_CONTEXT;
struct _PER_SOCKET_CONTEXT;

class CIOCP
{
public:

	void GiocpInit();
	void GiocpDelete();
	bool CreateGIocp(int server_port);
	void DestroyGIocp();
	bool CreateListenSocket();
	bool RecvDataParse(_PER_IO_CONTEXT * lpIOContext, int uIndex);
	bool DataSend(int aIndex, unsigned char* lpMsg, DWORD dwSize, bool Encrypt = true);
	bool IoSendSecond(_PER_SOCKET_CONTEXT * lpPerSocketContext);
	bool IoMoreSend(_PER_SOCKET_CONTEXT * lpPerSocketContext);
	bool UpdateCompletionPort(SOCKET sd, int ClientIndex, BOOL bAddToList);
	void CloseClient(_PER_SOCKET_CONTEXT * lpPerSocketContext, int result);
	void CloseClient(int index);
	void ResponErrorCloseClient(int index);

private:

	unsigned char* ExSendBuf;
	int g_ServerPort;
	HANDLE g_IocpThreadHandle;
	HANDLE g_ThreadHandles[MAX_IO_THREAD_HANDLES];
	CRITICAL_SECTION criti;
	enum SOCKET_FLAG;
	DWORD g_dwThreadCount;
	HANDLE g_CompletionPort;
	SOCKET g_Listen;

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
BOOL DataSend(LPGameObject &lpObj, unsigned char* lpMsg, DWORD dwSize, char* szFunction = nullptr);
BOOL IoSendSecond(_PER_SOCKET_CONTEXT * lpPerSocketContext);
BOOL IoMoreSend(_PER_SOCKET_CONTEXT * lpPerSocketContext);
BOOL UpdateCompletionPort(SOCKET sd, int ClientIndex, DWORD dwServerTypeCount);
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

