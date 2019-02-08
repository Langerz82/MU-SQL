////////////////////////////////////////////////////////////////////////////////
// wsJoinServerCli.cpp
// ------------------------------
// Decompiled by Deathway
// Date : 2007-03-09
// ------------------------------

// GS-N 0.99.60T Status : Completed
//	GS-N	1.00.18	JPN	0x0048D170	-	Completed

#include "stdafx.h"
#include "wsJoinServerCli.h"
#include "TLog.h"
#include "Authorization.h"

wsJoinServerCli::wsJoinServerCli(bool RSA, bool AuthProtocol)
{
	this->m_bUseRSA = RSA;
	this->m_UseAuthProtocol = AuthProtocol;
}

wsJoinServerCli::~wsJoinServerCli()
{
	return;
}

BOOL wsJoinServerCli::SetProtocolCore(WsProtocolCore pc)
{
	this->wsProtocolCore = pc;
	return TRUE;
}
	
BOOL wsJoinServerCli::Connect(LPSTR ip_addr, WORD port, DWORD WinMsgNum)
{
	hostent* host;
	sockaddr_in addr;
	int nResult;

	host = 0;

	if ( this->m_hWnd  == 0 )
	{
		sLog.outBasic("Connect Error %s %d", __FILE__, __LINE__);
		return FALSE;
	}

	addr.sin_family = AF_INET;
	addr.sin_port =  htons(port);
	if(port == AUTH_PORT)
	{
		addr.sin_addr.S_un.S_addr = -1;
	}
	else
	{
		addr.sin_addr.S_un.S_addr = inet_addr(ip_addr);
	}
	if ( addr.sin_addr.S_un.S_addr == -1 )
	{
		host = gethostbyname(ip_addr);
		if ( host == 0 )
		{
			sLog.outBasic("server name not found [%s]", ip_addr);
			return FALSE;
		}

		memcpy(&addr.sin_addr.S_un.S_addr, *host->h_addr_list  , host->h_length  );	
	}

	nResult=connect(this->m_socket, (sockaddr*)&addr, 16);

	if (nResult== -1)
	{
		int iError = WSAGetLastError();

		if (iError != WSAEWOULDBLOCK)
		{
			this->Close(this->m_socket );
			return FALSE;
		}
	}

	nResult=WSAAsyncSelect(this->m_socket, this->m_hWnd , WinMsgNum, FD_READ|FD_WRITE|FD_CLOSE);

	if ( nResult == -1 )
	{
		closesocket(this->m_socket );
		sLog.outBasic("Client WSAAsyncSelect error %d", WSAGetLastError());
		return FALSE;
	}
	this->m_SendBufLen=0;
	this->SetConnect(TRUE);

	return TRUE;
}


BOOL wsJoinServerCli::DataSend(PCHAR buf, int len)
{
	int nResult;
	int nLeft;
	int nDx;	// Degeneration X :)
	LPBYTE sendbuf;
	int* sendbuflen;
	
	if ( len <= 0 )
	{
		return 0;
	}

	if ( this->m_socket  == INVALID_SOCKET )	// 
	{
		return 0;
	}

	nDx = 0;

	sendbuf = this->m_SendBuf;
	sendbuflen= &len;	// Strange, dangeorus

	if ( this->m_SendBufLen   > 0 )
	{
		if ( (this->m_SendBufLen +len) < 819200 )
		{
			memcpy( &this->m_SendBuf[this->m_SendBufLen]  , buf, len);	// Review this later

			this->m_SendBufLen +=len;
			return 1;
		}
		this->Close();
		return FALSE;
	}

	nLeft= *sendbuflen;

	while ( true )
	{
		nResult=send(this->m_socket, (char*)&buf[nDx], (*sendbuflen - nDx), 0);

		if ( nResult == -1)
		{
			if ( WSAGetLastError() != WSAEWOULDBLOCK )
			{
				sLog.outBasic("error-L3 : %d (%s %d)", WSAGetLastError(), __FILE__, __LINE__);
				*sendbuflen=0;
				this->Close(this->m_socket );
				return FALSE;
			}

			if ( (*sendbuflen + nLeft) > 819200 )
			{
				sLog.outError("error-L3 : buffer error closed %d, %d", (*sendbuflen+nLeft), 819200);
				*sendbuflen = 0;
				this->Close(this->m_socket);
				return FALSE;
			}

			if ( nDx >= 0 )
			{
				using namespace std;
				memcpy( &this->m_SendBuf[this->m_SendBufLen],  &buf[nDx], nLeft);
				
				this->m_SendBufLen +=nLeft;

				sLog.outBasic("error-L3 : Copy1 : nLeft:%d len:%d nDx:%d", nLeft, this->m_SendBufLen , nDx);
				return TRUE;
			}
		}
		else if ( nResult == 0 )
		{
			sLog.outBasic("error-L3 : send()  result is zero", WSAGetLastError());
			break;
		}

		if ( nResult >0 )
		{
			nDx += nResult;
			nLeft -= nResult;
			*sendbuflen -= nResult;
		}
		if ( nLeft <= 0)
		{
			break;
		}
	}
	return TRUE;
}



BOOL wsJoinServerCli::FDWRITE_MsgDataSend()
{
	int nResult;
	int nDx;	// Degeneration X :)
	LPBYTE sendbuf;
	int* sendbuflen;

	nDx=0;
	sendbuf=&this->m_SendBuf[0];
	sendbuflen=&this->m_SendBufLen ;

	if (this->m_SendBufLen < 1)
	{
		return TRUE;
	}

	while (*sendbuflen>0)
	{
		nResult=send(this->m_socket, (PCHAR)&sendbuf[nDx], *sendbuflen-nDx, 0);
		if (nResult == -1)
		{
			if ( WSAGetLastError() != WSAEWOULDBLOCK)
			{
				sLog.outBasic("FD_WRITE send() ���� %d %d", WSAGetLastError(), *sendbuflen);
				this->Close();
				return FALSE;
			}
			else
			{
				sLog.outBasic("FD_WRITE send() WSAEWOULDBLOCK : %d", WSAGetLastError());
				break;
			}
		}
		if (nResult <= 0)
		{
			sLog.outBasic("send() result is zero %d", WSAGetLastError());
			break;
		}
		sLog.outBasic("error-L3 : nDx %d m_SendBufLen %d", nDx, this->m_SendBufLen );

		if (nResult > 0)
		{
			nDx += nResult;
			*sendbuflen -= nResult;
		}

		sLog.outBasic("error-L3 : nDx %d m_SendBufLen %d", nDx, this->m_SendBufLen );
	}
	return TRUE;
}


int wsJoinServerCli::DataRecv()
{
	int nResult;
	unsigned char* recvbuf;
	int* recvbuflen;
	int lOfs;
	int size;
	BYTE headcode;

	recvbuf= (UCHAR*)&this->m_RecvBuf[0]   ;
	recvbuflen=&this->m_RecvBufLen ;
	nResult=recv(this->m_socket , (CHAR*)&recvbuf[*recvbuflen], (819200-*recvbuflen), 0);

	if ( nResult == 0 )
	{
		sLog.outBasic("error-L3 : closed %d - buff:%d(%d) ",
			WSAGetLastError(), this->m_RecvBufLen, 819200 - *recvbuflen);

		return 1;
	}
	else if (nResult == SOCKET_ERROR )
	{
		if ( WSAGetLastError() == WSAEWOULDBLOCK )
		{
			return 1;
		}
		sLog.outBasic("error-L3 : recv error %d", WSAGetLastError() );
		return 1;
	}

	*recvbuflen += nResult;

	if (*recvbuflen<3)
	{
		return 3;
	}

	lOfs=0;
	size=0;

	while ( true )
	{

		if ( this->m_UseAuthProtocol == true )
		{
			if( this->m_bUseRSA == true && recvbuf[lOfs] != 0xF0 ) // like this
			{
				int outsize;
				BYTE packDec[9000];

				int origsize = nResult - lOfs;

				CPrivateRsaKey * PrvKey = this->m_RSA->GetPrivateRsaKey();

				if ( PrvKey == NULL )
				{
					sLog.outError( "[PACKET-ENCODE] ERROR: PrvKey Pointer is NULL");
					break;
				}

				if(CMuRSA::DecPacket(PrvKey, &recvbuf[lOfs], origsize, packDec, outsize, 1) != 0) // look this is iff success if result is NOT 0 - fail 0 false 1 true? 0 - ok 1,2,3,4,5 etc - packet decrypt fail
				{
					lOfs += origsize;
					*recvbuflen -= origsize; // try this, maybe it is a solution,  I cant reproduce the freeze when I want xD Restart GS

					if ( *recvbuflen <= 0)
					{
						break;
					}

					continue;
				}

				size = *(WORD*)&packDec[1];
				headcode = packDec[0];

				if ( size <= 0 )
				{
					sLog.outBasic("error-L2 : size %d", size);
					return 0;
				}

				if ( size <= *recvbuflen )
				{
					this->wsProtocolCore(headcode, &packDec[0], size);
					lOfs += origsize;
					*recvbuflen -= origsize;

					if ( *recvbuflen <= 0)
					{
						break;
					}
				}
		
				else if ( lOfs > 0 )
				{
					if ( *recvbuflen < 1 ) 
					{
						break;
					}

					memcpy(recvbuf, &recvbuf[lOfs], *recvbuflen);
					break;
				}
				else
				{
					break;
				}
			}

			else
			{
				size = *(WORD*)&recvbuf[lOfs+1];
				headcode = recvbuf[lOfs];

				if ( size <= 0 )
				{
					sLog.outBasic("error-L2 : size %d",size);
					return 0;
				}

				if ( size <= *recvbuflen )
				{
					this->wsProtocolCore(headcode, &recvbuf[lOfs], size);
					lOfs += size;
					*recvbuflen -= size;

					if ( *recvbuflen <= 0)
					{
						break;
					}
				}
		
				else if ( lOfs > 0 )
				{
					if ( *recvbuflen < 1 ) 
					{
						break;
					}

					memcpy(recvbuf, &recvbuf[lOfs], *recvbuflen);
					break;
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			if (recvbuf[lOfs] == 0xC1 )	// Type packet handling C1 Type
			{
				unsigned char* c1subbase = (UCHAR*)&recvbuf[lOfs];
				size=c1subbase[1];
				headcode=c1subbase[2];
			}
			else if ( recvbuf[lOfs] == 0xC2 ) // Type packet handling C2 Type
			{
				unsigned char* c2subbase=(UCHAR*)&recvbuf[lOfs];
				size = c2subbase[1] * 256;
				size |= c2subbase[2];
				headcode = c2subbase[3];
			}
			else
			{
				sLog.outBasic("error-L2 : header error (%s %d)lOfs:%d, size:%d", __FILE__, __LINE__, lOfs, *recvbuflen);
				*recvbuflen = 0;
				return 0;
			}

			if ( size <= 0 )
			{
				sLog.outBasic("error-L2 : size %d", size);
				return 0;
			}

			if ( size <= *recvbuflen )
			{
				this->wsProtocolCore(headcode, &recvbuf[lOfs], size);
				lOfs += size;
				*recvbuflen -= size;

				if ( *recvbuflen <= 0)
				{
					break;
				}
			}
		
			else if ( lOfs > 0 )
			{
				if ( *recvbuflen < 1 ) 
				{
					break;
				}

				memcpy(recvbuf, &recvbuf[lOfs], *recvbuflen);
				break;
			}
			else
			{
				break;
			}
		}
	}
	return 0;
}




////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

