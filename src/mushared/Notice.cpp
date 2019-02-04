// Notice.cpp: implementation of the CNotice class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Notice.h"
#include "MemScript.h"
#include "winutil.h"
#include "TLog.h"
#include "configread.h"

CNotice gNotice;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNotice::CNotice() // OK
{
	this->m_count = 0;

	this->m_NoticeValue = 0;

	this->m_NoticeTime = GetTickCount();
}

CNotice::~CNotice() // OK
{

}

void CNotice::Load(char* path) // OK
{
	CMemScript* lpMemScript = new CMemScript;

	if (lpMemScript == 0)
	{
		g_Log.MsgBox(MEM_SCRIPT_ALLOC_ERROR, path);
		return;
	}

	if (lpMemScript->SetBuffer(path) == 0)
	{
		g_Log.MsgBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	this->m_count = 0;

	try
	{
		while (true)
		{
			if (lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			if (strcmp("end", lpMemScript->GetString()) == 0)
			{
				break;
			}

			NOTICE_INFO info;

			memset(&info, 0, sizeof(info));

			strcpy_s(info.Message, lpMemScript->GetString());

			info.Type = lpMemScript->GetAsNumber();

			info.Count = lpMemScript->GetAsNumber();

			info.Opacity = lpMemScript->GetAsNumber();

			info.Delay = lpMemScript->GetAsNumber();

			info.Color = RGB(lpMemScript->GetAsNumber(), lpMemScript->GetAsNumber(), lpMemScript->GetAsNumber());

			info.Speed = lpMemScript->GetAsNumber();

			info.RepeatTime = lpMemScript->GetAsNumber() * 1000;

			this->SetInfo(info);
		}
	}
	catch (...)
	{
		g_Log.MsgBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

void CNotice::SetInfo(NOTICE_INFO info) // OK
{
	if (this->m_count < 0 || this->m_count >= MAX_NOTICE)
	{
		return;
	}

	this->m_NoticeInfo[this->m_count++] = info;
}

void CNotice::MainProc() // OK
{
	if (this->m_count == 0)
	{
		return;
	}

	NOTICE_INFO* lpInfo = &this->m_NoticeInfo[this->m_NoticeValue];

	if ((GetTickCount() - this->m_NoticeTime) >= ((DWORD)lpInfo->RepeatTime))
	{
		this->m_NoticeValue = (((this->m_NoticeValue + 1) >= this->m_count) ? 0 : (this->m_NoticeValue + 1));
		this->m_NoticeTime = GetTickCount();
		this->GCNoticeSendToAll(lpInfo->Type, lpInfo->Count, lpInfo->Opacity, lpInfo->Delay, lpInfo->Color, lpInfo->Speed, "%s", lpInfo->Message);
	}
}

void CNotice::GCNoticeSend(int aIndex, BYTE type, BYTE count, BYTE opacity, WORD delay, DWORD color, BYTE speed, char* message, ...) // OK
{
	char buff[256] = { 0 };

	va_list arg;
	va_start(arg, message);
	vsprintf_s(buff, message, arg);
	va_end(arg);

	int size = strlen(buff);

	size = ((size>MAX_MESSAGE_SIZE) ? MAX_MESSAGE_SIZE : size);

	PMSG_NOTICE_SEND pMsg;

	pMsg.header.set((LPBYTE)&pMsg, 0x0D, (sizeof(pMsg) - (sizeof(pMsg.message) - (size + 1))));

	pMsg.type = type;

	pMsg.count = count;

	pMsg.opacity = opacity;

	pMsg.delay = delay;

	pMsg.color = color;

	pMsg.speed = speed;

	memcpy(pMsg.message, buff, size);

	pMsg.message[size] = 0;

	IOCP.DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
}

void CNotice::GCNoticeSendToAll(BYTE type, BYTE count, BYTE opacity, WORD delay, DWORD color, BYTE speed, char* message, ...) // OK
{
	char buff[256] = { 0 };

	va_list arg;
	va_start(arg, message);
	vsprintf_s(buff, message, arg);
	va_end(arg);

	int size = strlen(buff);

	size = ((size>MAX_MESSAGE_SIZE) ? MAX_MESSAGE_SIZE : size);

	PMSG_NOTICE_SEND pMsg;

	pMsg.header.set((LPBYTE)&pMsg, 0x0D, (sizeof(pMsg) - (sizeof(pMsg.message) - (size + 1))));

	pMsg.type = type;

	pMsg.count = count;

	pMsg.opacity = opacity;

	pMsg.delay = delay;

	pMsg.color = color;

	pMsg.speed = speed;

	memcpy(pMsg.message, buff, size);

	pMsg.message[size] = 0;

	for (int n = g_ConfigRead.server.GetObjectStartUserIndex(); n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		if (gObjIsConnectedGP(n) != 0)
		{
			IOCP.DataSend(n, (BYTE*)&pMsg, pMsg.header.size);
		}
	}
}

void CNotice::NewMessageDevTeam(int aIndex, char* message, ...) // OK
{
	char buff[256] = { 0 };

	va_list arg;
	va_start(arg, message);
	vsprintf_s(buff, message, arg);
	va_end(arg);

	int size = strlen(buff);

	size = ((size>MAX_MESSAGE_SIZE) ? MAX_MESSAGE_SIZE : size);

	PMSG_NOTICE_DEV_SEND pMsg;

	pMsg.header.set((LPBYTE)&pMsg, 0x00, (sizeof(pMsg) - (sizeof(pMsg.message) - (size + 1))));

	memcpy(pMsg.message, buff, size);

	pMsg.message[size] = 0;

	IOCP.DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);

}

void CNotice::NewNoticeSend(int aIndex, BYTE count, BYTE opacity, WORD delay, DWORD color, BYTE speed, char* message, ...) // OK
{
	char buff[256] = { 0 };

	va_list arg;
	va_start(arg, message);
	vsprintf_s(buff, message, arg);
	va_end(arg);

	int size = strlen(buff);

	size = ((size>MAX_MESSAGE_SIZE) ? MAX_MESSAGE_SIZE : size);

	PMSG_NOTICE_SEND_NEW pMsg;

	pMsg.header.set((LPBYTE)&pMsg, 0x00, (sizeof(pMsg) - (sizeof(pMsg.message) - (size + 1))));

	pMsg.count = count;

	pMsg.opacity = opacity;

	pMsg.delay = delay;

	pMsg.color = color;

	pMsg.speed = speed;

	memcpy(pMsg.message, buff, size);

	pMsg.message[size] = 0;

	IOCP.DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
}

void CNotice::ChatSend(int Index, char* szChat, ...)
{
	char buff[256] = { 0 };

	va_list arg;
	va_start(arg, szChat);
	vsprintf_s(buff, szChat, arg);
	va_end(arg);

	int size = strlen(buff);

	size = ((size>MAX_MESSAGE_SIZE) ? MAX_MESSAGE_SIZE : size);

	PMSG_NOTICE_MONSTER_SEND pMsg;

	pMsg.header.set((LPBYTE)&pMsg, 0x00, (sizeof(pMsg) - (sizeof(pMsg.message) - (size + 1))));

	memcpy(pMsg.message, buff, size);

	pMsg.message[size] = 0;

	for (Index = g_ConfigRead.server.GetObjectStartUserIndex(); Index < g_ConfigRead.server.GetObjectMax(); Index++)
	{
		if (gObjIsConnectedGP(Index) != 0)
		{
			IOCP.DataSend(Index, (BYTE*)&pMsg, pMsg.header.size);
		}
	}

	IOCP.DataSend(Index, (BYTE*)&pMsg, pMsg.header.size);
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

