#include "StdAfx.h"
#include "IOCP.h"
#include "ConnectEngine.h"
#include "LoginProtocol.h"
#include "Main.h"
#include "generalStructs.h"
#include "PacketEngineServer.h"
#include "PacketEncrypt.h"
#include "LoginProtocol.h"

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <ace/Dev_Poll_Reactor.h>
#include <ace/TP_Reactor.h>


#include <string>

CIOCP IOCP;

class _PER_SOCKET_CONTEXT;

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
	ACE_Reactor::instance()->initialized();
}

void CIOCP::ProcessEvents() {

	ACE_Time_Value interval(0, 1000000);

	ACE_Reactor::instance()->handle_events(interval);
}

void CIOCP::GiocpDelete()
{
	delete[] ExSendBuf;
}

void CIOCP::DestroyGIocp()
{
}


bool CIOCP::CreateListenSocket(WORD uiPort, LPSTR ipAddress)
{
	ACE_INET_Addr bind_addr(uiPort, ipAddress);

	if (g_HostSocket.open(bind_addr, ACE_Reactor::instance(), ACE_NONBLOCK) == -1)
	{
		sLog->outError("MuSQL Game Server can not bind to %s:%d", ipAddress, uiPort);
		return 0;
	}

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
	criti.lock();

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
			criti.unlock();
			return;
		}
	}
#endif

	//this->peer().set_handle((ACE_HANDLE)sockKey);
	ObjCSUser->PerSocketContext->dwIOCount = 0;

	_PER_SOCKET_CONTEXT* sockCtx = ObjCSUser->PerSocketContext;

	//memset(&sockCtx->IOContext[0].m_Overlapped, 0, sizeof(WSAOVERLAPPED));
	//memset(&sockCtx->IOContext[1].m_Overlapped, 0, sizeof(WSAOVERLAPPED));

	//sockCtx->IOContext[0].m_wsabuf.buf = (char*)&sockCtx->IOContext[0].Buffer;
	//sockCtx->IOContext[0].m_wsabuf.len = MAX_IO_BUFFER_SIZE;
	sockCtx->IOContext[0].IOOperation = 0;
	//sockCtx->IOContext[1].m_wsabuf.buf = (char*)sockCtx->IOContext[1].Buffer;
	//sockCtx->IOContext[1].m_wsabuf.len = MAX_IO_BUFFER_SIZE;
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
	//SCConnectResultSend(*ObjCSUser, 1);

	//PostQueuedCompletionStatus(g_CompletionPort, 0, 0, 0);

	criti.unlock();

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
	//LPOVERLAPPED lpOverlapped = 0;
	_PER_IO_CONTEXT * lpIOContext = nullptr;

	criti.lock();

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
	lpIOContext->nbBytes = n;
	lpIOContext->nTotalBytes += n;
	std::memcpy(&lpIOContext->Buffer, this->input_buffer_.base(), sizeof(n));

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
		criti.unlock();
	}*/

	lpPerSocketContext->dwIOCount++;
	lpIOContext->nWaitIO = 1;

	criti.unlock();

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

	if (lpIOContext->nbBytes < 3)
	{
		return true;
	}

	lOfs = 0;
	size = 0;
	xcode = 0;
	recvbuf = lpIOContext->Buffer;

	int recvsize = lpIOContext->nbBytes;

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
				lpIOContext->nbBytes);

			lpIOContext->nbBytes = 0;
			return FALSE;
		}

		if (size <= 0)
		{
			sLog->outBasic("error-L1 : size %d",
				size);

			return false;
		}

		if (size <= lpIOContext->nbBytes)
		{
			if (xcode == 0xC3)
			{
				int ret = g_PacketEncrypt.Decrypt(&byDec[2], &recvbuf[lOfs + 2], size - 2);

				if (ret < 0)
				{
					sLog->outError("[%s][Packet-Decrypt BYTE] Error: ret < 0 %x/%x/%x)", lpUser->IP, recvbuf[lOfs], recvbuf[lOfs + 1], recvbuf[lOfs + 2]);
				}

				else
				{
					BYTE* pDecBuf = byDec;

					headcode = pDecBuf[2];
					byDec[0] = 0xC1;
					byDec[1] = ret + 2;
					lpUser->PacketCount++;

					if (lpUser->PacketCount >= g_MaxPacketPerSec)
					{
						sLog->outError("[ANTI-HACK] Packets Per Second: %d / %d", lpUser->PacketCount, g_MaxPacketPerSec);
						IOCP.CloseClient(uIndex);
						return 0;
					}

					CStreamPacketEngine_Server PacketStream;
					PacketStream.Clear();

					if (PacketStream.AddData(byDec, ret + 2) == 0)
					{

						sLog->outError("error-L1 : CStreamPacketEngine Adding Error : ip = %s index:%s HEAD:%x (%s,%d)",
							lpUser->IP,
							lpUser->Index,
							headcode,
							__FILE__, __LINE__);

						return 0;
					}

					if (PacketStream.ExtractPacket(byDec) != 0)
					{
						sLog->outError("error-L2 : CStreamPacketEngine Adding Error : ip = %s index:%s HEAD:%x (%s,%d)",
							lpUser->IP,
							lpUser->Index,
							headcode,
							__FILE__, __LINE__);
						return 0;
					}

					m_JSProtocol.LoginProtocolCore(uIndex, headcode, byDec, ret);
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

					lpUser->PacketCount++;
					if (lpUser->PacketCount >= g_MaxPacketPerSec)
					{
						sLog->outError("[ANTI-HACK] Packets Per Second: %d / %d", lpUser->PacketCount, g_MaxPacketPerSec);
						//gGameProtocol.GCSendDisableReconnect(uIndex);
						IOCP.CloseClient(uIndex);
						return 0;
					}

					CStreamPacketEngine_Server PacketStream;

					PacketStream.Clear();
					if (PacketStream.AddData(byDec, ret + 3) == 0)
					{
						sLog->outError("error-L1 : CStreamPacketEngine Adding Error : ip = %s account:%s HEAD:%x (%s,%d)",
							lpUser->IP, lpUser->Index, headcode, __FILE__, __LINE__);
						return false;
					}

					if (PacketStream.ExtractPacket(byDec) != 0)
					{
						sLog->outError("error-L1 : CStreamPacketEngine ExtractPacket Error : ip = %s account:%s HEAD:%x (%s,%d)",
							lpUser->IP, lpUser->Index, headcode, __FILE__, __LINE__);
						return false;
					}

					//GameProtocolCore(headcode, byDec, ret, uIndex, 1);
				}
			}
			else
			{
				CStreamPacketEngine_Server ps;
				ps.Clear();

				if (ps.AddData(&recvbuf[lOfs], size) == 0)
				{
					sLog->outError("error-L1 : CStreamPacketEngine Adding Error : ip = %s account:%s HEAD:%x (%s,%d)",
						lpUser->IP, lpUser->Index, headcode, __FILE__, __LINE__);
					return 0;
				}

				if (ps.ExtractPacket(byDec) != 0)
				{
					sLog->outError("error-L1 : CStreamPacketEngine ExtractPacket Error : ip = %s account:%s HEAD:%x (%s,%d)",
						lpUser->IP, lpUser->Index, headcode, __FILE__, __LINE__);
					return 0;
				}

				lpUser->PacketCount++;
				if (lpUser->PacketCount >= g_MaxPacketPerSec)
				{
					sLog->outError("[ANTI-HACK] Packets Per Second: %d / %d", lpUser->PacketCount, g_MaxPacketPerSec);
					//GCSendDisableReconnect(uIndex);
					return 0;
				}

				//GameProtocolCore(headcode, byDec, size, uIndex, 0); // here
			}

			lOfs += size; // wait
			lpIOContext->nbBytes -= size;

			if (lpIOContext->nbBytes <= 0)
			{
				break;
			}
		}
		else if (lOfs > 0)
		{
			if (lpIOContext->nbBytes < 1)
			{
				sLog->outBasic("error-L1 : recvbuflen 1 %s %d", __FILE__, __LINE__);
				break;
			}

			if (lpIOContext->nbBytes < MAX_IO_BUFFER_SIZE)
			{
				std::memcpy(recvbuf, &recvbuf[lOfs], lpIOContext->nbBytes);
				sLog->outBasic("Message copy %d", lpIOContext->nbBytes);
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
	_PER_SOCKET_CONTEXT * lpPerSocketContext;

	criti.lock();

	STR_CS_USER* lpCSUser = getCSUser(uIndex);

	if (lpCSUser->ConnectionState == 0  )
	{
		criti.unlock();
		return false;
	}

	lpPerSocketContext = lpCSUser->PerSocketContext;

	_PER_IO_CONTEXT  * lpIoCtxt;

	lpIoCtxt = &lpPerSocketContext->IOContext[1];

	std::memcpy(lpIoCtxt->Buffer, lpMsg, dwSize);
	lpIoCtxt->nTotalBytes += dwSize;
	lpIoCtxt->nbBytes = dwSize;
	lpIoCtxt->IOOperation = 1;

	lpCSUser->Socket->send(lpIoCtxt->Buffer, lpIoCtxt->nbBytes);

	lpPerSocketContext->dwIOCount ++;
	lpIoCtxt->nWaitIO = 1;
	criti.unlock();
	return true;
}

void CIOCP::CloseClient(_PER_SOCKET_CONTEXT * lpPerSocketContext, int result)
{
	for (std::pair<int,STR_CS_USER*> user : gCSUsers)
	{
		user.second->Socket->close();
		UserDelete(user.second->Index);
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

	criti.lock();

	lpCSUser->Socket->close();

	criti.unlock();
}

void CIOCP::ResponErrorCloseClient(int index)
{
	STR_CS_USER* lpCSUser = getCSUser(index);

	if (lpCSUser->ConnectionState == 0 )
	{
		sLog->outError("error-L1 : CloseClient connect error");
		return;
	}

	criti.lock();
	//closesocket(lpCSUser->Index);
	lpCSUser->Socket = nullptr;
	UserDelete(lpCSUser->Index);
	criti.unlock();
}
