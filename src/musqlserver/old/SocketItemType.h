// SocketItemType.h: interface for the CSocketItemType class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _SOCKETITEMTYPE_H
#define _SOCKETITEMTYPE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "Item/Item.h"

#define FILE_CUSTOM_NEWSOCKETTYPE		"SocketSystem\\IGC_SocketItemType.ini"

struct SOCKET_ITEM_TYPE_INFO
{
	int Index;
	int MaxSocket;
};

class CSocketItemType
{
public:
	CSocketItemType();
	virtual ~CSocketItemType();
	void Load(char* path);
	bool CheckSocketItemType(int index);
	long GetSocketItemMaxSocket(int index);
private:
	std::map<int, SOCKET_ITEM_TYPE_INFO> m_SocketItemTypeInfo;
};

#endif // _SOCKETITEMTYPE_H

extern CSocketItemType gSocketItemType;



////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

