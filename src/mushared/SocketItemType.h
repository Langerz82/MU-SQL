// SocketItemType.h: interface for the CSocketItemType class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "zzzitem.h"

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

extern CSocketItemType gSocketItemType;


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

