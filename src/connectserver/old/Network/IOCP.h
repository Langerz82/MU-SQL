// This IOCP interface is for both the GameServer and LoginServer.

#ifndef _MU_IOCP_H
#define _MU_IOCP_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
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

class CIOCP : public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>
{
public:
	typedef ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH> Base;

	CIOCP();
	~CIOCP();
	void GiocpInit();
	bool CreateListenSocket(WORD uiPort, LPSTR ipAddress);
	bool RecvDataParse(_PER_IO_CONTEXT * lpIOContext, int uIndex);
	bool DataSend(int uIndex, LPBYTE lpMsg, DWORD dwSize, bool Encrypt = true);
	void CloseClients();
	void CloseClient(int index);

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

	static void ProcessEvents();
private:

	BYTE * ExSendBuf;
	int g_ServerPort;
	std::mutex criti;
	DWORD g_dwThreadCount;
	ACE_Acceptor<CIOCP, ACE_SOCK_Acceptor> g_HostSocket;
	std::map<ACE_HANDLE, STR_CS_USER*> g_UserIDMap;

	std::string remote_address_;
	ACE_Message_Block input_buffer_;
	ssize_t noblk_send(ACE_Message_Block& message_block);
};


extern CIOCP IOCP;

#endif
