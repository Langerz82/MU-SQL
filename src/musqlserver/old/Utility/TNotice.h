#ifndef MU_TNOTICE_H
#define MU_TNOTICE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GameProtocol.h"

struct PMSG_NOTICE;

__inline DWORD _ARGB(BYTE a, BYTE r, BYTE g, BYTE b){ return a*16777216 + b*65536 + g*256 + r;}; // in tnotice.h : line 8

class TNotice
{
public:

	static void MakeNoticeMsg(void * lpNotice, BYTE btType, LPSTR szNoticeMsg);
	static void MakeNoticeMsgEx(void * lpNotice, BYTE btType, LPSTR szNoticeMsg, ...);
	static void SetNoticeProperty(void * lpNotice, BYTE btType, DWORD dwColor, BYTE btCount, WORD wDelay, BYTE btSpeed);
	static void SendNoticeToAllUser(void * lpNotice);
	static void SendNoticeToUser(CGameObject &Obj, void * lpNotice);
	static void AllSendServerMsg(LPSTR chatmsg);
	static void GCServerMsgStringSend(LPSTR szMsg, CGameObject &Obj, BYTE type);

	TNotice(BYTE btType);

	void __cdecl SendToUser(CGameObject &Obj, LPSTR szMsg, ...);
	void __cdecl SendToAllUser(LPSTR szMsg, ...);

	// Dont have prefix
	//void __thiscall SetSpeed(BYTE);
	//void __thiscall SetColor(unsigned long);
	//void __thiscall SetCount(BYTE);
	//void __thiscall SetDelay(unsigned short);
	//void __thiscall SetType(BYTE);

	PMSG_NOTICE m_Notice;
};


#endif
