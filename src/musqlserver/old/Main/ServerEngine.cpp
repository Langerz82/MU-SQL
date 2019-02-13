// ServerEngine.cpp: implementation of the CServerEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "ServerEngine.h"
#include "Main.h"
//#include "Sprotocol.h"
//#include "CoreDSprotocol.h"
//#include "EDSprotocol.h"
#include "pugixml.hpp"

#define szModule "Main"

IPList g_IpList[25];
STR_SERVER_ENGINE* g_Server = NULL;
int ipcount = 0;
int servercount = 0;

void gObjServerInit()
{
	g_Server = new STR_SERVER_ENGINE[g_dwMaxServerGroups]; 

	if ( g_Server == NULL )
	{
		sLog->outError("[Server Engine] Memory Allocation Error");
		return;
	}

	memset(g_Server, 0, sizeof(STR_SERVER_ENGINE) * g_dwMaxServerGroups);

	for(int i=0;i<g_dwMaxServerGroups;i++)
	{
		g_Server[i].m_Index = -1;
		g_Server[i].m_Socket = INVALID_SOCKET;
		g_Server[i].m_State = SS_CLOSED;
		g_Server[i].m_Type = ST_NONE;
		g_Server[i].m_ProtocolCore = NULL;
		g_Server[i].PerSocketContext = new _PER_SOCKET_CONTEXT;
		g_Server[i].m_ServerCode = -1;
	}

	servercount = 0;
	if(g_iConnectStatSyncEnable == TRUE)
	{
		DWORD ThreadId;
		CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(gObjConnectStatSyncThread), nullptr, 0, &ThreadId);
	}
}

int gObjServerAddSearch()
{
	if ( g_Server == NULL )
		return -1;

	for(int i=0;i<g_dwMaxServerGroups;i++)
	{
		if ( g_Server[i].m_Index == -1 )
		{
			return i;
		}
	}

	return -1;
}

int gObjServerAdd(SOCKET Socket, char * Ip, int ServerIndex, eSERVER_TYPE eServerType)
{
	g_Server[ServerIndex].m_Index = ServerIndex;
	memcpy(g_Server[ServerIndex].m_ServerIp, Ip, 16);
	g_Server[ServerIndex].m_State = SS_CONNECTED;
	g_Server[ServerIndex].m_Type = eServerType;
	g_Server[ServerIndex].m_Socket = Socket;

	switch ( g_Server[ServerIndex].m_Type )
	{
		case ST_JOINSERVER:
			g_Server[ServerIndex].m_ProtocolCore = CLoginServerProtocol::ProtocolCore;
			break;
	}
	
	sLog->outBasic("[Server Engine] Connect : Index : %d - IP : %s - ServerType : %d",
		ServerIndex, Ip, eServerType);

	servercount++;
	return ServerIndex;
}


void gObjServerDel(CGameObject &Obj)
{

	EnterCriticalSection(&scriti);

	if(g_Server[aIndex].m_Type == ST_JOINSERVER)
	{
		// TODO
		//m_JSProtocol.DisconnectServer(g_Server[aIndex].m_ServerCode);
	}

	g_Server[aIndex].m_Index = -1;
	g_Server[aIndex].m_ServerCode = -1;
	g_Server[aIndex].m_Socket = INVALID_SOCKET;
	g_Server[aIndex].m_ServerIp[0];
	g_Server[aIndex].m_State = SS_CLOSED;
	g_Server[aIndex].m_Type = ST_NONE;
	g_Server[aIndex].m_ProtocolCore = NULL;
	g_Server[aIndex].m_ConnectPort = -1;
	servercount--;

	LeaveCriticalSection(&scriti);
}

void LoadAllowableIpList(LPSTR filename)
{
	memset(g_IpList,0,sizeof(g_IpList));

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(filename);

	if (res.status != pugi::status_ok)
	{
		sLog->outError("LoadAllowedIpList() error - file load error (%s)", res.description());
		return;
	}

	pugi::xml_node mainXML = file.child("AllowedIPList");
	int count = 0;

	for (pugi::xml_node ip = mainXML.child("IP"); ip; ip = ip.next_sibling())
	{
		strcpy(g_IpList[count].m_IP, ip.attribute("Address").as_string());
		count++;
	}

	ipcount = count;
	sLog->outBasic("%s File Loaded successfuly", filename);
}
//memb___id

void gObjConnectStatSyncProc()
{
	LPSTR szAccount;
	for(int i=0;i<m_JSProtocol.m_UserData.m_MuLoginUserDataCount;i++)
	{
		szAccount = m_JSProtocol.m_UserData.GetAccountID(i);
		if(FALSE)
		{
			m_JSProtocol.DisconnectPlayer(m_JSProtocol.m_UserData.GetAccountID(i));
			sLog->outBasic("[%s] Force Disconnect ConnectStat = 0 but Player is Online");
		}
	}
}

void gObjConnectStatSyncThread()
{
	while(true)
	{
		gObjConnectStatSyncProc();
		Sleep(2000);
	}
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

