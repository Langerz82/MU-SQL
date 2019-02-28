#ifndef _MU_GOUSERFUNCTIONS_H_
#define _MU_GOUSERFUNCTIONS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "CUserData.h"
#include "GameProtocol.h"

void gObjReqMapSvrAuth(CGameObject &Obj)
{
	if (Obj.Connected != PLAYER_CONNECTED)
	{
		sLog->outBasic("%s is not connected", Obj.AccountID);
		IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
		return;
	}

	GJReqMapSvrAuth(Obj, Obj.AccountID, Obj.m_MapServerAuthInfo->szCharName,
		Obj.m_MapServerAuthInfo->iJA1, Obj.m_MapServerAuthInfo->iJA2,
		Obj.m_MapServerAuthInfo->iJA3, Obj.m_MapServerAuthInfo->iJA4);

	sLog->outBasic("[MapServerMng] Map Server Join Send : [%s][%s](%d)",
		Obj.AccountID, Obj.m_MapServerAuthInfo->szCharName, Obj.m_Index);

	memset(Obj.m_MapServerAuthInfo->szCharName, 0x00, MAX_ACCOUNT_LEN + 2);
	Obj.m_MapServerAuthInfo->iJA1 = 0;
	Obj.m_MapServerAuthInfo->iJA2 = 0;
	Obj.m_MapServerAuthInfo->iJA3 = 0;
	Obj.m_MapServerAuthInfo->iJA4 = 0;
}


#endif
