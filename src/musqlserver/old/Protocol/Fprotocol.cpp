// Fprotocol.cpp: implementation of the Fprotocol class.
//
//////////////////////////////////////////////////////////////////////

#include "Common\Common.h"
#include "Fprotocol.h"
#include "EDSprotocol.h"
#include "util.h"
#include "IOCP.h"
#include "ServerEngine.h"
#include <string.h>
using namespace std;

CFriendSystem::CFriendSystem()
{
	this->m_iMaxFriends = 50;
	//InitializeCriticalSection(&m_csMapFriendMaster);
}

CFriendSystem::~CFriendSystem()
{
	//DeleteCriticalSection(&m_csMapFriendMaster);
}

FRIEND_MASTER* CFriendSystem::GetFriendMaster(char *szMaster)
{
	if(szMaster == NULL)
		return NULL;

	if(strlen(szMaster) < 4)
		return NULL;

	char szM[11] = {0};
	std::strncpy(szM, szMaster, 10);

	std::map<string, FRIEND_MASTER, strCmp>::iterator it;
	it = this->m_MapFriendManager.find(szM);
	if(it == this->m_MapFriendManager.end())
		return NULL;

	return &it->second;
}

BOOL CFriendSystem::FriendExists(char *szMaster, char *szFriend)
{
	int i;
	FRIEND_MASTER* lpMaster;

	char szM[11] = {0};
	std::strncpy(szM, szMaster, 10);

	char szF[11] = {0};
	std::strncpy(szF, szFriend, 10);

	//EnterCriticalSection(&m_csMapFriendMaster);
	lpMaster = GetFriendMaster(szM);

	if(lpMaster == NULL)
	{
		//LeaveCriticalSection(&m_csMapFriendMaster);
		return FALSE;
	}

	int size = (int)lpMaster->m_vecFriends.size();

	for( i=0; i < size; i++)
	{
		if(strcmp(lpMaster->m_vecFriends[i].m_szName, szF) == 0)
			break;
	}

	//LeaveCriticalSection(&m_csMapFriendMaster);
	if(i == size)
		return FALSE;
	else
		return TRUE;
}

BOOL CFriendSystem::CreateFriendMaster(char *szMaster, int iNumber, int iServer)
{

	char szM[11] = {0};
	std::strncpy(szM, szMaster, 10);

	FRIEND_MASTER* lpM;
	lpM = GetFriendMaster(szM);
	if(lpM)
	{
		lpM->m_iNumber = iNumber;
		lpM->m_iServer = iServer;
		lpM->m_btState = iServer;
		return TRUE;
	}

	FRIEND_MASTER Master;
	Master.m_iNumber = iNumber;
	Master.m_iServer = iServer;
	Master.m_btState = iServer;

	//EnterCriticalSection(&this->m_csMapFriendMaster);
	this->m_MapFriendManager[szM] = Master;
	//LeaveCriticalSection(&this->m_csMapFriendMaster);

	return TRUE;
}

BOOL CFriendSystem::DeleteFriendMaster(char *szMaster)
{
	std::map<string, FRIEND_MASTER, strCmp>::iterator it;

	char szM[11] = {0};
	std::strncpy(szM, szMaster, 10);

	//EnterCriticalSection(&m_csMapFriendMaster);
	it = this->m_MapFriendManager.find(szM);
	if(it == this->m_MapFriendManager.end())
	{
		//LeaveCriticalSection(&m_csMapFriendMaster);
		return FALSE;
	}

	this->m_MapFriendManager.erase(it);
	//LeaveCriticalSection(&m_csMapFriendMaster);

	return TRUE;
		
}

int CFriendSystem::GetFriendGuid(char *szMaster)
{

	char szM[11] = {0};
	std::strncpy(szM, szMaster, 10);

	int guid = -1;
	FRIEND_MASTER* lpMaster;
	//EnterCriticalSection(&m_csMapFriendMaster);

	lpMaster = this->GetFriendMaster(szM);
	if(lpMaster)
		guid = lpMaster->m_iGUID;
	//LeaveCriticalSection(&m_csMapFriendMaster);

	return guid;
}

BYTE CFriendSystem::GetFriendState(char *szMaster)
{
	BYTE btState = -1;
	FRIEND_MASTER* lpMaster;

	//EnterCriticalSection(&m_csMapFriendMaster);

	lpMaster = GetFriendMaster(szMaster);
	if(lpMaster)
		btState = lpMaster->m_btState;
	//LeaveCriticalSection(&m_csMapFriendMaster);

	return btState;
}

int CFriendSystem::GetFriendServer(char *szMaster)
{
	int iServer = -1;
	FRIEND_MASTER* lpMaster;

	//EnterCriticalSection(&m_csMapFriendMaster);

	lpMaster = GetFriendMaster(szMaster);
	if(lpMaster)
		iServer = lpMaster->m_iServer;
	//LeaveCriticalSection(&m_csMapFriendMaster);

	return iServer;
}

int CFriendSystem::GetFriendNumber(char *szMaster)
{
	int iNumber = -1;
	FRIEND_MASTER* lpMaster;

	//EnterCriticalSection(&m_csMapFriendMaster);

	lpMaster = GetFriendMaster(szMaster);
	if(lpMaster)
		iNumber = lpMaster->m_iNumber;
	//LeaveCriticalSection(&m_csMapFriendMaster);

	return iNumber;
}

BOOL CFriendSystem::FriendDBConnect()
{

	{
		sLog->outError("[ERROR] - FRIEND DB CANNOT CONNECT TO MSSQL");
		return FALSE;
	}


	{
		sLog->outError("[ERROR] - FRIEND DB CANNOT CONNECT TO MSSQL");
		return FALSE;
	}

	sLog->outBasic("[SUCCESS] - FRIEND DB CONNECT MSSQL SUCCESS");

	return TRUE;
}

int CFriendSystem::ExDBGetIndexByCode(int iServerCode)
{
	if(iServerCode < 0)
		return -1;

	for(int i=0; i < g_dwMaxServerGroups; i++)
	{
		if(g_Server[i].m_Index != -1
				&& g_Server[i].m_Type == ST_EXDATASERVER
				&& g_Server[i].m_State == SS_GUILD
				&& g_Server[i].m_ServerCode == iServerCode)
		{
			return i;
		}
	}

	return -1;
}

void CFriendSystem::FriendClose(CGameObject &Obj, BYTE* lpMsg)
{
	char szMaster[11] = {0};

	std::strncpy(szMaster, ((SDHP_USERCLOSE *)lpMsg)->CharName, 10);
	
	FRIEND_MASTER* lpMaster;
	lpMaster = GetFriendMaster(szMaster);
	if(lpMaster == NULL)
		return;

	lpMaster->m_btState = -1;
	SendStateToAllFriends(aIndex, szMaster);
	DeleteFriendMaster(szMaster);
}

BOOL CFriendSystem::UserGuidDBCreate(char *szName)
{
	char Name[MAX_ACCOUNT_LEN+1];
	Name[10] = 0;
	std::memcpy(Name, szName, MAX_ACCOUNT_LEN);



	return false;
}

BOOL CFriendSystem::GetDBGuidAndMemoTotal(char *szName, int& guid, int& memo_total)
{
	BOOL res = FALSE;

	return res;
}


BOOL CFriendSystem::GetDBFriendList(char *szMaster)
{
	FRIEND_MASTER* lpMaster;

	BOOL res = TRUE;
	lpMaster = GetFriendMaster(szMaster);
	if(lpMaster == NULL)
		return FALSE;

	if(GetDBGuidAndMemoTotal(szMaster, lpMaster->m_iGUID, lpMaster->m_iMemoTotal) == FALSE)
	{
		res = FALSE;
		if(UserGuidDBCreate(szMaster))
		{
			res = GetDBGuidAndMemoTotal(szMaster, lpMaster->m_iGUID, lpMaster->m_iMemoTotal);
		}
	}

	if(res == FALSE)
		return FALSE;

	res = FALSE;
	int i=0;
	char szFriend[11] = {0};



	return res;
}

void CFriendSystem::FriendListSend(CGameObject &Obj, char *szMaster)
{
	FRIEND_MASTER* lpMaster;

	sLog->outBasic("[Friend List Send] Name[%s].", szMaster);
	lpMaster = GetFriendMaster(szMaster);
	if(lpMaster == NULL)
	{
		sLog->outBasic("[Friend List Send] FrienMaster[%s] not found.", szMaster);
		return;
	}

	FHP_FRIENDLIST_COUNT* lpFriendListCnt;
	FHP_FRIENDLIST* lpFriendList;

	int cnt = (int)lpMaster->m_vecFriends.size();
	int size = sizeof(*lpFriendListCnt)+sizeof(*lpFriendList)*cnt;
	BYTE* lpData = new BYTE[size];
	std::ZeroMemory(lpData, size);

	lpFriendListCnt = (FHP_FRIENDLIST_COUNT*)lpData;
	lpFriendList = (FHP_FRIENDLIST*)(lpData+sizeof(*lpFriendListCnt));
	PHeadSetW(lpData, 0x60, size);
	lpFriendListCnt->Count = cnt;
	lpFriendListCnt->MailCount = lpMaster->m_iMemoTotal;
	lpFriendListCnt->Number = lpMaster->m_iNumber;
	lpFriendListCnt->Server = lpMaster->m_iServer;
	std::strncpy(lpFriendListCnt->Name, szMaster, 10);
	
	sLog->outBasic("[Friend List Send] Friend Count[%d].", cnt);
	for(int i=0; i < cnt; i++)
	{
		FRIEND_MASTER* lpFr;
		int iServ = -1;
		lpFr = GetFriendMaster(lpMaster->m_vecFriends[i].m_szName);
		if(lpFr)
		{
			// check if friendship is established
			if(FriendExists(lpMaster->m_vecFriends[i].m_szName, szMaster))
				iServ = lpFr->m_btState;
		}

		lpFriendList[i].Server = iServ;
		std::strncpy(lpFriendList[i].Name, lpMaster->m_vecFriends[i].m_szName, 10);
		
		char szFriend[11] = {0};
		std::strncpy(szFriend, lpFriendList[i].Name, 10);
		sLog->outBasic("[Friend List Send] Friend[%s] send.", szFriend);
	}

	DataSend(lpObj.m_Index, lpData, size, __FUNCTION__);

	delete [] lpData;
}

BOOL CFriendSystem::GetDBWaitFriend(char *szMaster, OUT char *szWaitFriend)
{
	FRIEND_MASTER* lpMaster;
	lpMaster = GetFriendMaster(szMaster);

	if(lpMaster == NULL)
		return FALSE;

	BOOL res = FALSE;


	return res;
}

BOOL CFriendSystem::FriendWaitSend(CGameObject &Obj, char *szMaster)
{
	FHP_WAITFRIENDLIST_COUNT WaitFr;
	std::ZeroMemory(&WaitFr, sizeof(WaitFr));
	PHeadSetB((BYTE*)&WaitFr, 0x61, sizeof(WaitFr));

	
	char szWaitFriend[11] = {0};

	FRIEND_MASTER* lpMaster;
	lpMaster = GetFriendMaster(szMaster);
	if(lpMaster == NULL)
		return FALSE;

	if(GetDBWaitFriend(szMaster, szWaitFriend) == FALSE)
		return FALSE;

	std::strncpy(WaitFr.FriendName, szWaitFriend, 10);
	std::strncpy(WaitFr.Name, szMaster, 10);
	WaitFr.Number = lpMaster->m_iNumber;

	DataSend(lpObj.m_Index, (BYTE*)&WaitFr, WaitFr.h.size, __FUNCTION__);

	return TRUE;
}

void CFriendSystem::SendState(CGameObject &Obj, char *szMaster, int iNumber, char *szName, BYTE btState)
{
	FHP_FRIEND_STATE State;
	std::ZeroMemory(&State, sizeof(State));

	PHeadSetB((BYTE*)&State, 0x62, sizeof(State));

	std::strncpy(State.Name, szMaster, 10);
	std::strncpy(State.FriendName, szName, 10);
	State.Number = iNumber;
	State.State = btState;

	DataSend(lpObj.m_Index, (BYTE*)&State, State.h.size, __FUNCTION__);
}

void CFriendSystem::SendStateToAllFriends(CGameObject &Obj, char *szMaster)
{
	FRIEND_MASTER* lpMaster;
	lpMaster = GetFriendMaster(szMaster);
	if(lpMaster == NULL)
		return;

	int cnt = (int)lpMaster->m_vecFriends.size();

	for(int i=0; i < cnt; i++)
	{
		FRIEND_MASTER* lpFr;
		lpFr = GetFriendMaster(lpMaster->m_vecFriends[i].m_szName);
		if(lpFr)
		{
			// check if friendship is established
			if(FriendExists(lpMaster->m_vecFriends[i].m_szName, szMaster))
			{
				int index = ExDBGetIndexByCode(lpFr->m_iServer);
				if(index != -1)
				{
					SendState(index, 
						lpMaster->m_vecFriends[i].m_szName, 
						lpFr->m_iNumber, szMaster, lpMaster->m_btState);
				}
			}
		}
	}
}

void CFriendSystem::FriendListRequest(CGameObject &Obj, FHP_FRIENDLIST_REQ* lpMsg)
{
	
	char szMaster[11]={0};
	std::strncpy(szMaster, lpMsg->Name, 10);

	sLog->outBasic("[Friend List Request] Name[%s].", szMaster);
	if(CreateFriendMaster(szMaster, lpMsg->Number, lpMsg->pServer) == FALSE)
		return;

	GetDBFriendList(szMaster);
	FriendListSend(aIndex, szMaster);
	FriendWaitSend(aIndex, szMaster);
	SendStateToAllFriends(aIndex, szMaster);

	FHP_FRIEND_MEMO_LIST_REQ req;
	std::ZeroMemory(&req, sizeof(req));
	PHeadSetB((BYTE*)&req, 0x71, sizeof(req));

	std::strncpy(req.Name, lpMsg->Name, 10);
	req.Number = lpMsg->Number;
	FriendMemoListReq(aIndex,&req);

	int ServerIndex = this->ExDBGetIndexByCode(lpMsg->pServer);

	m_EXDSProtocol.GuildMatchingSendAcceptAndRejectInfo(aIndex, szMaster, lpMsg->Number, ServerIndex);

	if (m_EXDSProtocol.m_PartyMatchingEDS.ConnectMember(lpMsg->Name, lpMsg->Number, lpMsg->pServer, ServerIndex) == TRUE)
	{
		PARTYMATCHING_PARTYDATA * lpPartyData = m_EXDSProtocol.m_PartyMatchingEDS.GetPartyDataByMember(lpMsg->Name);

		if (lpPartyData)
		{
			if (strcmp(lpPartyData->szLeaderName, lpMsg->Name))
			{
				if (lpPartyData->m_UserData[0].iServerIndex == ServerIndex)
				{
					m_EXDSProtocol.FixAddPartyMember(lpPartyData->m_UserData[0].iUserIndex, lpMsg->Number, aIndex);
				}
			}

			else
			{
				for (int i = 1; i < 5; i++)
				{
					if (lpPartyData->m_UserData[i].iUsedInfo == TRUE && lpPartyData->m_UserData[i].iUserConnected == TRUE)
					{
						if (lpPartyData->m_UserData[i].iServerIndex == aIndex)
						{
							m_EXDSProtocol.FixAddPartyMember(lpPartyData->m_UserData[0].iUserIndex, lpPartyData->m_UserData[i].iUserIndex, aIndex);
						}
					}
				}
			}

			m_EXDSProtocol.SendPartyMatchingMemberList(lpPartyData->m_UserData[0].szUserName, 0);
		}
	}
}

BOOL CFriendSystem::AddFriend(char *szMaster, char *szFriend)
{
	FRIEND_MASTER* lpMaster;
	lpMaster = GetFriendMaster(szMaster);
	if(lpMaster == NULL)
		return FALSE;

	if( lpMaster->m_vecFriends.size() == this->m_iMaxFriends)
	{
		sLog->outBasic("[%s]'s friend list is full - %d", szMaster, this->m_iMaxFriends);
		return FALSE;
	}

	lpMaster->m_vecFriends.push_back(szFriend);

	sLog->outBasic("[%s] became [%s]'s friend.", szFriend, szMaster);
	return TRUE;
}

int CFriendSystem::AddDBWaitFriend(char *szMaster, char *szFriend)
{
	int res = 1;

	return res;
}

void CFriendSystem::FriendAddRequest( CGameObject &Obj, FHP_FRIEND_ADD_REQ* lpMsg)
{
	FHP_FRIEND_ADD_RESULT Result;
	std::ZeroMemory(&Result, sizeof(Result));
	PHeadSetB((BYTE*)&Result, 0x63, sizeof(Result));

	std::strncpy(Result.Name, lpMsg->Name, 10);
	std::strncpy(Result.FriendName, lpMsg->FriendName, 10);
	Result.Result = 2;
	Result.Server = -1;
	Result.Number = lpMsg->Number;

	char szMaster[11] = {0};
	char szFriend[11] = {0};
	std::strncpy(szMaster, Result.Name, 10);
	std::strncpy(szFriend, Result.FriendName, 10);

	sLog->outBasic("[Friend Add Request] Name[%s] FriendName[%s].", szMaster, szFriend);

	if(strcmp(szMaster, szFriend) == 0)
	{
		// own id
		Result.Result = 5;

		sLog->outBasic("[Friend Add Request] Send Result [%d].", Result.Result);
		DataSend(lpObj.m_Index, (BYTE*)&Result, Result.h.size, __FUNCTION__);
		return;
	}

	FRIEND_MASTER* lpMaster;
	lpMaster = GetFriendMaster(szMaster);
	if(lpMaster == NULL)
	{
		sLog->outBasic("[Friend Add Request] Send Result [%d].", Result.Result);
		DataSend(lpObj.m_Index, (BYTE*)&Result, Result.h.size, __FUNCTION__);
		return;
	}

	if(FriendExists(szMaster, szFriend))
	{
		Result.Result = 4;

		sLog->outBasic("[Friend Add Request] Send Result [%d].", Result.Result);
		DataSend(lpObj.m_Index, (BYTE*)&Result, Result.h.size, __FUNCTION__);
		return;
	}

	if(lpMaster->m_vecFriends.size() == this->m_iMaxFriends)
	{
		Result.Result = 3;

		sLog->outBasic("[Friend Add Request] Send Result [%d].", Result.Result);
		DataSend(lpObj.m_Index, (BYTE*)&Result, Result.h.size, __FUNCTION__);
		return;
	}

	int res = AddDBWaitFriend(szMaster, szFriend);
	
	// DB result, GUID doesn't exists, or select error
	if(res > 1 && res < 6)
	{
		Result.Result = 0;

		sLog->outBasic("[Friend Add Request] Send Result [%d].", Result.Result);
		DataSend(lpObj.m_Index, (BYTE*)&Result, Result.h.size, __FUNCTION__);
		return;
	}

	// DB result, lvl< 6
	if(res == 6)
	{
		Result.Result = 6;
		sLog->outBasic("[Friend Add Request] Send Result [%d].", Result.Result);
		DataSend(lpObj.m_Index, (BYTE*)&Result, Result.h.size, __FUNCTION__);
		return;
	}

	// DB, other errors
	if(res != 0 && res != 8)
	{
		Result.Result = 2;
		sLog->outBasic("[Friend Add Request] Send Result [%d].", Result.Result);
		DataSend(lpObj.m_Index, (BYTE*)&Result, Result.h.size, __FUNCTION__);
		return;
	}

	FRIEND_MASTER* lpFr;
	lpFr = GetFriendMaster(szFriend);

	Result.Server = -1;
	BYTE btState = 0;
	if(lpFr)
	{
		// check if friendship is established
		if(FriendExists(szFriend, szMaster))
		{
			Result.Server = lpFr->m_btState;
			SendState(aIndex, szFriend, lpFr->m_iNumber, szMaster, lpMaster->m_btState);
		}

		int index = ExDBGetIndexByCode(lpFr->m_iServer);
		if(index != -1)
			FriendWaitSend(index, szFriend); // fixed
	}


	AddFriend(szMaster, szFriend);

	Result.Result = 1;

	sLog->outBasic("[Friend Add Request] Send Result [%d].", Result.Result);
	DataSend(lpObj.m_Index, (BYTE*)&Result, Result.h.size, __FUNCTION__);

}

int CFriendSystem::DelDBWaitFriend(char *szMaster, char *szFriend)
{
	int res = 1;

	if(true)
	{
		if(false)
		{
			if(res != 0)
				sLog->outError("WZ_WaitFriendDel returned: [%d]", res);
		}
		else
			sLog->outError("WZ_WaitFriendDel no result returned.");
	}
	else
		sLog->outError("Error executing WZ_WaitFriendDel.");

	return res;
}

BOOL CFriendSystem::AddDBFriend(char *szMaster, char *szFriend)
{
	int res = -1;

	if(res == 1)
		return TRUE;

	
	return FALSE;
	
}

BOOL CFriendSystem::DelDBFriend(char *szMaster, char *szFriend)
{
	int res = -1;

	if(res == 1)
		return TRUE;

	
	return FALSE;
}

int CFriendSystem::WaitFriendAdd(char *szMaster, char *szFriend)
{
	FRIEND_MASTER* lpMaster;

	lpMaster = GetFriendMaster(szMaster);
	if(lpMaster == NULL)
		return 0;

	if(FriendExists(szMaster, szFriend))
		return 4;

	if(lpMaster->m_vecFriends.size() == this->m_iMaxFriends)
	{
		sLog->outBasic("[%s]'s friend list is full - %d", szMaster, m_iMaxFriends);
		return 3;
	}

	if(AddDBFriend(szMaster, szFriend) == FALSE)
		return 0;

	if(AddFriend(szMaster, szFriend) == FALSE)
		return 0;

	FRIEND_MASTER* lpFr;
	lpFr = GetFriendMaster(szFriend);
	if(lpFr)
	{
		int index = ExDBGetIndexByCode(lpFr->m_iServer);
		if(index != -1)
			SendState(index, szFriend, lpFr->m_iNumber, szMaster, lpMaster->m_btState);
	}

	return 1;
}

void CFriendSystem::WaitFriendAddRequest( CGameObject &Obj,  FHP_WAITFRIEND_ADD_REQ* lpMsg)
{
	FHP_WAITFRIEND_ADD_RESULT Result;
	std::ZeroMemory(&Result, sizeof(Result));

	PHeadSetB((BYTE*)&Result, 0x64, sizeof(Result));
	Result.Number = lpMsg->Number;

	std::strncpy(Result.Name, lpMsg->Name, 10);
	std::strncpy(Result.FriendName, lpMsg->FriendName, 10);
	Result.pServer = -1;
	
	char szMaster[11] = {0};
	char szFriend[11] = {0};

	std::strncpy(szMaster, lpMsg->Name, 10);
	std::strncpy(szFriend, lpMsg->FriendName, 10);

	sLog->outBasic("[WaitFriend Add Request] Name[%s], FriendName[%s].", szMaster, szFriend);

	if(lpMsg->Result == 0)
	{
		Result.Result = 2;
		DelDBWaitFriend(szMaster, szFriend);
		sLog->outBasic("[WaitFriend Add Request] [%s] declined [%s]'s request to be a friend.", szMaster, szFriend);
		DataSend(lpObj.m_Index, (BYTE*)&Result, Result.h.size, __FUNCTION__);
		return;
	}

	sLog->outBasic("[WaitFriend Add Request] [%s] accepted [%s]'s request to be a friend.", szMaster, szFriend);

	FRIEND_MASTER* lpFr;
	lpFr = GetFriendMaster(szFriend);
	if(lpFr)
		Result.pServer = lpFr->m_btState;

	Result.Result = WaitFriendAdd(szMaster, szFriend);
	if(Result.Result != 1)
		DelDBWaitFriend(szMaster, szFriend);

	FriendWaitSend(aIndex, szMaster);
	DataSend(lpObj.m_Index, (BYTE*)&Result, Result.h.size, __FUNCTION__);
}

void CFriendSystem::FriendStateClientRecv( CGameObject &Obj, FHP_FRIEND_STATE_C* lpMsg)
{

	char szMaster[11] = {0};
	std::strncpy(szMaster, lpMsg->Name, 10);

	sLog->outBasic("[Friend State Receive] Name: [%s], State: [%d].", szMaster, lpMsg->State);
	FRIEND_MASTER* lpMaster; 
	lpMaster = GetFriendMaster(szMaster);
	if(lpMaster == NULL)
	{
		sLog->outBasic("[Friend State Receive] error: FriendMaster [%s] not found.", szMaster);
		return;
	}

	switch(lpMsg->State)
	{
		case 0:
			// refuse chat
			lpMaster->m_btState = -2;
		break;

		case 2:
			// ???, client shows "Cannot Use"
			lpMaster->m_btState = -3;
		break;

		default:
			// display server
			lpMaster->m_btState = lpMaster->m_iServer;
	}

	SendStateToAllFriends(aIndex, szMaster);
}

BOOL CFriendSystem::DelFriend(char *szMaster, char *szFriend)
{
	FRIEND_MASTER* lpMaster;
	lpMaster = GetFriendMaster(szMaster);
	if(lpMaster == NULL)
		return FALSE;

	if(FriendExists(szMaster, szFriend) == FALSE)
		return FALSE;

	int cnt = (int)lpMaster->m_vecFriends.size();
	for(int i=0; i < cnt; i++)
	{
		if(strcmp(lpMaster->m_vecFriends[i].m_szName, szFriend) == 0)
		{
			lpMaster->m_vecFriends.erase(lpMaster->m_vecFriends.begin()+i);
			sLog->outBasic("[%s] dropped [%s] from his friend list.", szMaster, szFriend);
			break;
		}

	}
	return TRUE;
}

void CFriendSystem::FriendDelRequest( CGameObject &Obj, FHP_FRIEND_ADD_REQ* lpMsg)
{
	FHP_FRIEND_DEL_RESULT Result;
	std::ZeroMemory(&Result, sizeof(Result));

	PHeadSetB((BYTE*)&Result, 0x65, sizeof(Result));

	Result.Number = lpMsg->Number;

	char szMaster[11] = {0};
	char szFriend[11] = {0};
	
	std::strncpy(Result.Name, lpMsg->Name, 10);
	std::strncpy(Result.FriendName, lpMsg->FriendName, 10);

	std::strncpy(szMaster, lpMsg->Name, 10);
	std::strncpy(szFriend, lpMsg->FriendName, 10);

	sLog->outBasic("[Friend Delete Request] Name[%s], FriendName[%s].", szMaster, szFriend);

	Result.Result = DelFriend(szMaster, szFriend);
	if(Result.Result)
		Result.Result = DelDBFriend(szMaster, szFriend);

	sLog->outBasic("[Friend Delete Request] Send Result [%d].", Result.Result);
	DataSend(lpObj.m_Index, (BYTE*)&Result, Result.h.size, __FUNCTION__);

	FRIEND_MASTER* lpFr;
	lpFr = GetFriendMaster(szFriend);
	if(lpFr)
	{
		int index = ExDBGetIndexByCode(lpFr->m_iServer);

		if(index != -1)
			SendState(index, szFriend, lpFr->m_iNumber, szMaster, -1);
	}
}


void CFriendSystem::MemoHeaderSend(CGameObject &Obj, WORD wNumber, MEMO_HEADER * lpMemoHead )
{
	FHP_FRIEND_MEMO_LIST MemoHead;
	std::ZeroMemory(&MemoHead, sizeof(MemoHead));

	PHeadSetW((BYTE*)&MemoHead, 0x71, sizeof(MemoHead));
	MemoHead.Number = wNumber;
	std::memcpy(MemoHead.Date, lpMemoHead->Date, 30);
	std::memcpy(MemoHead.SendName, lpMemoHead->SendName, 10);
	std::memcpy(MemoHead.RecvName, lpMemoHead->RecvName, 10);
	std::memcpy(MemoHead.Subject, lpMemoHead->Subject, 60);
	MemoHead.MemoIndex = lpMemoHead->MemoIndex;
	MemoHead.read = lpMemoHead->read;

	DataSend(lpObj.m_Index, (BYTE*)&MemoHead, sizeof(MemoHead), __FUNCTION__);
}

void CFriendSystem::MemoListSend(CGameObject &Obj, WORD wNumber, char *szName)
{

	char szMaster[11] = {0};
	std::strncpy(szMaster, szName, 10);

	sLog->outBasic("[Mail List Send] Name: [%s].", szMaster);
	int guid = GetFriendGuid(szMaster);
	if(guid == -1)
	{
		sLog->outBasic("[Mail List Send] error: GUID = -1.");
		return;
	}

	int cnt = 0;


	sLog->outBasic("[Mail List Send] Mail Count: %d.", cnt);
}

void CFriendSystem::FriendMemoListReq( CGameObject &Obj, FHP_FRIEND_MEMO_LIST_REQ* lpMsg)
{

	char szName[11] = {0};
	std::strncpy(szName, lpMsg->Name, 10);

	sLog->outBasic("[Mail List Request] Name[%s].", szName);
	MemoListSend(aIndex, lpMsg->Number, szName);
}

int CFriendSystem::DBWriteMail(MEMO_SEND_HEADER * lpMemoSendHdr, BYTE* Photo, BYTE btPhotoSize, char *sMemo, int memo_size)
{
	char szSendName[11] = {0};
	char szRecvName[11] = {0};

	std::strncpy(szSendName, lpMemoSendHdr->Name, 10);
	std::strncpy(szRecvName, lpMemoSendHdr->ToName, 10);


	return -1;
}

void CFriendSystem::FriendMemoSend( CGameObject &Obj, FHP_FRIEND_MEMO_SEND* lpMsg)
{
	FHP_FRIEND_MEMO_SEND_RESULT Result;
	std::ZeroMemory(&Result, sizeof(Result));
	PHeadSetB((BYTE*)&Result, 0x70, sizeof(Result));

	Result.Number = lpMsg->Number;
	Result.WindowGuid = lpMsg->WindowGuid;
	std::strncpy(Result.Name, lpMsg->Name, 10);
	
	char szName[11] = {0};
	std::strncpy(szName, Result.Name, 10);

	MEMO_SEND_HEADER head;
	std::ZeroMemory(&head, sizeof(head));

	memcpy(&head, lpMsg->Name, sizeof(head));
	
	int photo_size;
	int msg_size = MAKEWORD(lpMsg->h.sizeL, lpMsg->h.sizeH);
	BYTE* lpMemo;
	BYTE* lpPhoto;
	
	photo_size = 18;
	lpMemo = (BYTE*)lpMsg->Memo;
	lpPhoto = lpMsg->Photo;

	char szMaster[11]={0};
	std::strncpy(szMaster, lpMsg->ToName, 10);

	sLog->outBasic("[Mail Send Request] Name[%s], ToName[%s], MailSize[%d], PhotoSize[%d].", 
		szName, szMaster, lpMsg->MemoSize, photo_size);

	int res = -1;

	if(lpMsg->MemoSize <= 1000)
		res = DBWriteMail(&head, lpPhoto, photo_size, (char *)lpMemo, lpMsg->MemoSize);
	else
		sLog->outBasic("[Mail Send Request] error: Mail Size > 1000.");

	if(res == -2)
		Result.Result = 2;
	else if(res == -3)
		Result.Result = 3;
	else if(res == -6)
		Result.Result = 6;
	else if(res < 0)
		Result.Result = 0;
	else
		Result.Result = 1;

	sLog->outBasic("[Mail Send Request] Send Result [%d].", Result.Result);
	DataSend(lpObj.m_Index, (BYTE*)&Result, Result.h.size, __FUNCTION__);
	
	if(Result.Result != 1)
		return;
	
	
	FRIEND_MASTER* lpFr;
	lpFr = GetFriendMaster(szMaster);

	if(!lpFr)
	{
		return;
	}

	MEMO_HEADER memo_head;
	memo_head.MemoIndex = res;
	memo_head.read = 0;
	std::strncpy(memo_head.Subject, lpMsg->Subject, 60);
	std::strncpy(memo_head.RecvName, lpMsg->ToName, 10);
	std::strncpy(memo_head.SendName, lpMsg->Name, 10);
	
	// TODO
	SYSTEMTIME st;
	//GetLocalTime(&st);

	wsprintf(memo_head.Date, "%04d-%02d-%02d %02d:%02d:%02d", st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond);

	memo_head.read = 2;

	int index = ExDBGetIndexByCode(lpFr->m_iServer);
	if (index != -1)
		MemoHeaderSend(index, lpFr->m_iNumber, &memo_head);
}

BOOL CFriendSystem::DBReadMail(int iMemoId, int iGuid, MEMO_READ * lpMemoRead, LPDWORD lpdwMemoSize, BYTE* lpPhoto, BYTE* lpPhotoSize)
{
	return FALSE;
}

void CFriendSystem::FriendMemoReadReq( CGameObject &Obj, FHP_FRIEND_MEMO_RECV_REQ* lpMsg)
{

	MEMO_READ memo;
	std::ZeroMemory(&memo, sizeof(memo));
	DWORD memo_size=0;
	BYTE Photo[30];
	std::memset(Photo, -1, sizeof(Photo));
	BYTE btPhotoSize=18;
	char szMaster[11]={0};
	std::strncpy(szMaster, lpMsg->Name, 10);

	sLog->outBasic("[Mail Read Request] Name: [%s], MemoIndex: [%d].", szMaster, lpMsg->MemoIndex);
	int guid = -1;
	guid = GetFriendGuid(szMaster);
	if(guid < 0)
	{
		sLog->outBasic("[Mail Read Request] error: GUID = -1.");
		return;
	}

	if(DBReadMail(lpMsg->MemoIndex, guid, &memo, &memo_size, Photo, &btPhotoSize))
	{
		FHP_FRIEND_MEMO_RECV Result;
		std::ZeroMemory(&Result, sizeof(Result));

		PHeadSetW((BYTE*)&Result, 0x72, sizeof(Result)-sizeof(Result.Memo)+memo_size);

		Result.MemoIndex = lpMsg->MemoIndex;
		Result.Number = lpMsg->Number;
		std::strncpy(Result.Name, lpMsg->Name, 10);

		Result.MemoSize = memo_size;
		memcpy(&Result.Dir, &memo, sizeof(memo));
		memcpy(Result.Photo, Photo, 18);
		
		DataSend(lpObj.m_Index, (BYTE*)&Result, sizeof(Result)-sizeof(Result.Memo)+memo_size, __FUNCTION__);

		sLog->outBasic("[Mail Read Request] Send Mail: size[%d], photo size[%d].", memo_size, btPhotoSize);
	}
}

BOOL CFriendSystem::DBDelMail(char *szName, int iMemoId)
{
	int res = FALSE;

	return res;
}

void CFriendSystem::FriendMemoDelReq( CGameObject &Obj, FHP_FRIEND_MEMO_DEL_REQ* lpMsg)
{

	FHP_FRIEND_MEMO_DEL_RESULT Result;
	std::ZeroMemory(&Result, sizeof(Result));
	PHeadSetB((BYTE*)&Result, 0x73, sizeof(Result));

	Result.MemoIndex = lpMsg->MemoIndex;
	Result.Number = lpMsg->Number;
	std::strncpy(Result.Name, lpMsg->Name, 10);

	char szName[11]={0};
	std::strncpy(szName, lpMsg->Name, 10);

	sLog->outBasic("[Mail Delete Request] Name[%s], MamoIndex[%d].", szName, lpMsg->MemoIndex);

	Result.Result = DBDelMail(szName, lpMsg->MemoIndex);

	sLog->outBasic("[Mail Delete Request] Send Result [%d].", Result.Result);
	DataSend(lpObj.m_Index, (BYTE*)&Result, Result.h.size, __FUNCTION__);
}


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

