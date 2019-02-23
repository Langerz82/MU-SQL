////////////////////////////////////////////////////////////////////////////////
// TNotice.cpp
#include "StdAfx.h"
#include "TNotice.h"
//#include "User/CUserData.h"
#include "IOCP.h"
#include "util.h"
//#include "GameMain.h"
//#include "configread.h"

// GS-N 0.99.60T 0x0048CD90 - Completed
//	GS-N	1.00.18	JPN	0x004A7F00	-	Completed



void TNotice::MakeNoticeMsg(void * lpNotice, BYTE btType, LPSTR szNoticeMsg)
{
	PMSG_NOTICE * pNotice = (PMSG_NOTICE *)lpNotice;
	pNotice->type  = btType;
	std::memcpy(pNotice->Notice,szNoticeMsg,strlen(szNoticeMsg));
	wsprintf(pNotice->Notice, szNoticeMsg);
	PHeadSetB((BYTE*)pNotice, 0x0D, strlen(pNotice->Notice) + sizeof(PMSG_NOTICE) - sizeof(pNotice->Notice) + 1 );
}

void TNotice::MakeNoticeMsgEx(void * lpNotice, BYTE btType, LPSTR szNoticeMsg, ...)
{
	PMSG_NOTICE * pNotice = (PMSG_NOTICE *)lpNotice;
	pNotice->type = btType;
	va_list pArguments;
	va_start(pArguments, szNoticeMsg);
	vsprintf((char*)pNotice->Notice, (char*)szNoticeMsg, pArguments);
	va_end(pArguments);
	PHeadSetB((BYTE*)pNotice, 0x0D, strlen(pNotice->Notice) + sizeof(PMSG_NOTICE) - sizeof(pNotice->Notice) + 1);
}

void TNotice::SetNoticeProperty(void * lpNotice, BYTE btType, DWORD dwColor, BYTE btCount, WORD wDelay, BYTE btSpeed)
{
	PMSG_NOTICE * pNotice =(PMSG_NOTICE *) lpNotice;

	pNotice->type = btType;
	pNotice->dwColor = dwColor;
	pNotice->btCount = btCount;
	pNotice->wDelay = wDelay;
	pNotice->btSpeed = btSpeed;
}

/*void TNotice::SendNoticeToAllUser(void * lpNotice)
{
	PMSG_NOTICE * pNotice = (PMSG_NOTICE *)lpNotice;

	for ( int n = g_ConfigRead.server.GetObjectStartUserIndex() ; n < g_ConfigRead.server.GetObjectMax() ; n++)
	{
		if ( getGameObject(n)->Connected == PLAYER_PLAYING )
		{
			if ( getGameObject(n)->Type  == OBJ_USER )
			{
				IOCP.DataSend(n, (BYTE*)pNotice  , pNotice->h.size  );
			}
		}
	}
}*/

void TNotice::SendNoticeToUser(int aIndex, void * lpNotice)
{
	PMSG_NOTICE * pNotice = (PMSG_NOTICE *)lpNotice;
	GIOCP.DataSend(aIndex, (BYTE*)pNotice, pNotice->h.size  );
}


/*void TNotice::AllSendServerMsg(LPSTR chatmsg)
{
	PMSG_NOTICE pNotice;
	
	MakeNoticeMsg((TNotice *)&pNotice, 0,  chatmsg);

	for ( int n = g_ConfigRead.server.GetObjectStartUserIndex() ; n < g_ConfigRead.server.GetObjectMax() ; n++)
	{
		if ( getGameObject(n)->Connected == PLAYER_PLAYING )
		{
			if ( getGameObject(n)->Type  == OBJ_USER )
			{
				IOCP.DataSend(n, (BYTE*)&pNotice , pNotice.h.size );
			}
		}
	}
}*/

void TNotice::GCServerMsgStringSend(LPSTR szMsg, int aIndex, BYTE type)
{
	PMSG_NOTICE pNotice;
	
	MakeNoticeMsg(&pNotice, type, szMsg);
	GIOCP.DataSend(aIndex, (BYTE*)&pNotice, pNotice.h.size);
}


TNotice::TNotice(BYTE btType)
{
	this->m_Notice.btCount = 1;
	this->m_Notice.btSpeed = 20;
	this->m_Notice.dwColor = _ARGB(255, 255, 200, 80 );
	this->m_Notice.wDelay = 0;
	this->m_Notice.type = btType;
}





/*void TNotice::SendToAllUser(LPSTR szMsg, ...)
{
	va_list pArguments;
	va_start(pArguments, szMsg);
	vsprintf(this->m_Notice.Notice, (char*)szMsg, pArguments);
	va_end(pArguments);
	PHeadSetB((BYTE*)&this->m_Notice, 0x0D, strlen(this->m_Notice.Notice)  + sizeof(PMSG_NOTICE) - sizeof(this->m_Notice.Notice) + 1);

	for ( int n = g_ConfigRead.server.GetObjectStartUserIndex() ; n < g_ConfigRead.server.GetObjectMax() ; n++)
	{
		if ( getGameObject(n)->Connected == PLAYER_PLAYING )
		{
			if ( getGameObject(n)->Type  == OBJ_USER )
			{
				IOCP.DataSend(n, (BYTE*)&this->m_Notice , this->m_Notice.h.size );
			}
		}
	}
}*/

void TNotice::SendToUser(int aIndex, LPSTR szMsg, ...)
{
	va_list pArguments;
	va_start(pArguments, szMsg);
	vsprintf(this->m_Notice.Notice, (char*)szMsg, pArguments);
	va_end(pArguments);
	PHeadSetB((BYTE*)&this->m_Notice, 0x0D, strlen(this->m_Notice.Notice)  + sizeof(PMSG_NOTICE) - sizeof(this->m_Notice.Notice) + 1);
	GIOCP.DataSend(aIndex, (BYTE*)&this->m_Notice , this->m_Notice.h.size );
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

