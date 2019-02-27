#ifndef MU_INTRO_PROTOCOL_H
#define MU_INTRO_PROTOCOL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "prodef.h"
#include "generalStructs.h"
//#include "GuildClass.h"
//#include "user.h"
//#include "MapClass.h"
#include "GensSystemProtocol.h"
#include "Game/User/CUserData.h"
//#include "Shop.h"
#include "Query.h"


class IntroProtocol
{
public:

	IntroProtocol();
	~IntroProtocol();

	void ProtocolCore(BYTE protoNum, unsigned char * aRecv, int aLen, STR_CS_USER* csUser, BOOL Encrypt);
	void CGLiveClient(PMSG_CLIENTTIME* lpClientTime, STR_CS_USER* csUser);
	void GCServerMsgStringSend(LPSTR szMsg, STR_CS_USER* csUser, BYTE type);
	void GCServerMsgStringSendEx(STR_CS_USER* csUser, BYTE type, LPSTR szMsg, ...);
	void SCPJoinResultSend(int aIndex, BYTE result);
	void CSPJoinIdPassRequest(PMSG_IDPASS* lpMsg, STR_CS_USER* csUser);
	void GCJoinResult(BYTE result, int aIndex);
	void CGClientCloseMsg(PMSG_CLIENTCLOSE* lpMsg, int aIndex);
	void GCCloseMsgSend(STR_CS_USER* csUser, BYTE result);
	void CGClientMsg(PMSG_CLIENTMSG* lpMsg, STR_CS_USER* csUser);
	void CGPCharacterCreate(PMSG_CHARCREATE* lpMsg, STR_CS_USER* csUser);
	void CGPCharDel(PMSG_CHARDELETE* lpMsg, STR_CS_USER* csUser);
	void GCSendDisableReconnect(int aIndex);
	bool PacketCheckTime(int aIndex);
	void GCSetCharColors(int aIndex);

	void JGCharacterCreateFailSend(int aIndex, char* id);
	void JGPGetCharList(CUserData* lpUser);

	bool First = true;

	CQuery m_CharDB;
	CQuery m_AccDB;

private:
	bool DataEncryptCheck(int aIndex, BYTE protoNum, BOOL Encrypt);
	std::mutex criti;
};

extern IntroProtocol IProtocol;

#endif
