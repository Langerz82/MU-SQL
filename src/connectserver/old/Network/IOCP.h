// This IOCP interface is for both the GameServer and LoginServer.

#ifndef _MU_IOCP_H
#define _MU_IOCP_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
//#include "CQueue.h"
#include "SocketBuffer/BufferedSocket.h"
#include "generalStructs.h"

#include <ace/Get_Opt.h>
#include <ace/ACE.h>
#include <ace/Acceptor.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/Reactor.h>

class BufferedSocket;

struct _PER_IO_CONTEXT;
struct _PER_SOCKET_CONTEXT;
struct STR_CS_USER;

class CIOCP: public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>
{
public:
	typedef ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH> Base;

	CIOCP() : input_buffer_(4096) {};
	~CIOCP() {};
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

	STR_CS_USER* getUserData(ACE_HANDLE handle)
	{
		std::map<ACE_HANDLE, STR_CS_USER*>::iterator pUO = g_UserIDMap.find(handle);
		if (pUO == g_UserIDMap.end())
		{
			sLog->outBasic("UserObject does not exist. %s %d\n", __FILE__, __LINE__);
			return nullptr;
		}
		else
			return pUO->second;
	};

	int OnAccept(ACE_HANDLE handle);
	int OnRead(ACE_HANDLE handle);
	int OnClose();

	virtual int handle_input(ACE_HANDLE = ACE_INVALID_HANDLE) override;
	virtual int handle_output(ACE_HANDLE = ACE_INVALID_HANDLE) override;
	virtual int open(void*) override;
	virtual int handle_close(ACE_HANDLE = ACE_INVALID_HANDLE,
		ACE_Reactor_Mask = ACE_Event_Handler::ALL_EVENTS_MASK);

	void CreateUserData(ACE_HANDLE handle);

	const std::string& get_remote_address(void) const
	{
		return this->remote_address_;
	};

	//static ACE_Reactor* g_Reactor;

	static void ProcessEvents();
private:

	BYTE* ExSendBuf;
	int g_ServerPort;
	//HANDLE g_IocpThreadHandle;
	//HANDLE g_ThreadHandles[MAX_IO_THREAD_HANDLES];
	//CRITICAL_SECTION criti;
	std::mutex criti;
	//enum SOCKET_FLAG;
	DWORD g_dwThreadCount;
	//HANDLE g_CompletionPort;
	//SOCKET g_Listen;
	ACE_Acceptor<CIOCP, ACE_SOCK_Acceptor> g_HostSocket;

	std::map<ACE_HANDLE, STR_CS_USER*> g_UserIDMap;

	std::string remote_address_;
	ACE_Message_Block input_buffer_;


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

/*
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
*/


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

