// SocketItemType.cpp: implementation of the CSocketItemType class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SocketItemType.h"
#include "ItemOptionTypeMng.h"
#include "MemScript.h"
#include "winutil.h"
#include "TLog.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSocketItemType gSocketItemType;

CSocketItemType::CSocketItemType() // OK
{
	this->m_SocketItemTypeInfo.clear();
}

CSocketItemType::~CSocketItemType() // OK
{

}

void CSocketItemType::Load(char* path) // OK
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

	this->m_SocketItemTypeInfo.clear();

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

			SOCKET_ITEM_TYPE_INFO info;

			memset(&info, 0, sizeof(info));

			info.Index = SafeGetItem(ITEMGET(lpMemScript->GetNumber(), lpMemScript->GetAsNumber()));

			//info.MaxSocket = lpMemScript->GetAsNumber();

			this->m_SocketItemTypeInfo.insert(std::pair<int, SOCKET_ITEM_TYPE_INFO>(info.Index, info));
		}
	}
	catch (...)
	{
		g_Log.MsgBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

bool CSocketItemType::CheckSocketItemType(int index) // OK
{
	std::map<int, SOCKET_ITEM_TYPE_INFO>::iterator it = this->m_SocketItemTypeInfo.find(index);

	if (it == this->m_SocketItemTypeInfo.end())
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

long CSocketItemType::GetSocketItemMaxSocket(int index) // OK
{
	std::map<int, SOCKET_ITEM_TYPE_INFO>::iterator it = this->m_SocketItemTypeInfo.find(index);

	if (it == this->m_SocketItemTypeInfo.end())
	{
		return 0;
	}
	else
	{
		return it->second.MaxSocket;
	}
}


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

