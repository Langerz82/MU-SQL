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

	CIOCP();
	~CIOCP();
	void GiocpInit();
	void GiocpDelete();
	void DestroyGIocp();
	bool CreateListenSocket(WORD uiPort, LPSTR ipAddress);
	bool RecvDataParse1(_PER_IO_CONTEXT * lpIOContext, int uIndex);
	bool RecvDataParse2(_PER_IO_CONTEXT * lpIOContext, int uIndex);
	bool DataSend(int uIndex, LPBYTE lpMsg, DWORD dwSize, bool Encrypt = true);
	void CloseClient(_PER_SOCKET_CONTEXT * lpPerSocketContext, int result);
	void CloseClient(int index);
	void ResponErrorCloseClient(int index);

	STR_CS_USER* getCSByHandle(ACE_HANDLE handle)
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
	int OnRead(ACE_HANDLE handle, int len);
	int OnClose(ACE_HANDLE h);
	void CloseClients(_PER_SOCKET_CONTEXT * lpPerSocketContext, int result);

	virtual int handle_input(ACE_HANDLE = ACE_INVALID_HANDLE) override;
	virtual int handle_output(ACE_HANDLE = ACE_INVALID_HANDLE) override;
	virtual int open(void*) override;
	virtual int handle_close(ACE_HANDLE = ACE_INVALID_HANDLE,
		ACE_Reactor_Mask = ACE_Event_Handler::ALL_EVENTS_MASK);
	
	bool send(BYTE* buf, size_t len);
	size_t recv_len(void) const;
	bool recv_soft(char* buf, size_t len);
	bool recv(char* buf, size_t len);
	void recv_skip(size_t len);
	void close_connection(void);

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
	std::mutex criti;
	DWORD g_dwThreadCount;
	ACE_Acceptor<CIOCP, ACE_SOCK_Acceptor> g_HostSocket;
	std::map<ACE_HANDLE, STR_CS_USER*> g_UserIDMap;

	std::string remote_address_;
	ACE_Message_Block input_buffer_;
	ssize_t noblk_send(ACE_Message_Block& message_block);



};



/*
extern CQueue m_DSQueue1;
extern CQueue m_DSQueue2;
extern CQueue m_DSQueue3;
extern CQueue m_DSQueue4;
extern CQueue m_DSQueue5;
extern CQueue m_JSQueue;
extern CQueue m_EXDSQueue;
*/

extern CIOCP GIOCP;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

