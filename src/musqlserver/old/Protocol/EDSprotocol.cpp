// EDSprotocol.cpp: implementation of the EDSprotocol class.
//
//////////////////////////////////////////////////////////////////////

#include "EDSprotocol.h"
#include "ServerEngine.h"
#include "DSProtocol.h"
#include "Fprotocol.h"
#include "pugixml.hpp"
#include "Utility/util.h"
#include "giocp.h"
#include "Main.h"

CExDataServerProtocol::CExDataServerProtocol()
{
	this->m_MapGuildManager.clear();
	this->m_MapUnionManager.clear();
}

CExDataServerProtocol::~CExDataServerProtocol()
{

}

GUILD_INFO_STRUCT* CExDataServerProtocol::GetGuild(char *szGuild)
{
	char szG[9]={0};
	strncpy(szG, szGuild, 8);
	MAP_GUILD_INFO::iterator it = this->m_MapGuildManager.find(szG);

	if(it == this->m_MapGuildManager.end())
		return NULL;

	return (GUILD_INFO_STRUCT*) it->second;
}

GUILD_INFO_STRUCT* CExDataServerProtocol::GetGuild(int iNumber)
{
	MAP_GUILD_INFO::iterator it, end;
	it = this->m_MapGuildManager.begin();
	end = this->m_MapGuildManager.end();

	GUILD_INFO_STRUCT* guild;
	for(; it != end; it++)
	{
		guild = (GUILD_INFO_STRUCT*) it->second;
		if(guild->m_iNumber == iNumber)
			break;
	}
	
	if(it == end)
		return NULL;

	return guild;
}

GUILD_MEMBER* CExDataServerProtocol::GetGuildMember(char *szGuild, char *szName)
{
	GUILD_INFO_STRUCT* lpGuild;

	lpGuild = GetGuild(szGuild);
	if( lpGuild == FALSE )
		return NULL;

	char szN[11]={0};
	strncpy(szN, szName, 10);

	MAP_GUILD_MEMBER::iterator it;
	it = lpGuild->m_mapGuildMember.find(szN);

	if(it == lpGuild->m_mapGuildMember.end())
		return NULL;

	return it->second;
}

GUILD_MEMBER* CExDataServerProtocol::GetGuildMember(int nGuildNumber, char *szName)
{
	GUILD_INFO_STRUCT* lpGuild;

	lpGuild = GetGuild(nGuildNumber);
	if (lpGuild == FALSE)
		return NULL;

	char szN[11] = { 0 };
	strncpy(szN, szName, 10);

	MAP_GUILD_MEMBER::iterator it;
	it = lpGuild->m_mapGuildMember.find(szN);

	if (it == lpGuild->m_mapGuildMember.end())
		return NULL;

	return it->second;
}

int CExDataServerProtocol::GetGuildMemberCount(char *szGuildName)
{
	GUILD_INFO_STRUCT* lpGuild;

	lpGuild = GetGuild(szGuildName);
	if (lpGuild == FALSE)
		return 0;

	return (int)lpGuild->m_mapGuildMember.size();
}

int CExDataServerProtocol::GetGuildMemberCount(int nGuildNumber)
{
	GUILD_INFO_STRUCT* lpGuild;

	lpGuild = GetGuild(nGuildNumber);
	if (lpGuild == FALSE)
		return 0;

	return (int)lpGuild->m_mapGuildMember.size();
}

BOOL CExDataServerProtocol::GuildExists(char *szGuild)
{
	if(this->GetGuild(szGuild) != NULL)
		return TRUE;
	else
		return FALSE;
}

BOOL CExDataServerProtocol::GuildMemberExists(char *szGuild, char *szName)
{
	if(this->GetGuildMember(szGuild, szName) != NULL)
		return TRUE;
	else
		return FALSE;
}

STR_UNION_MEMBER_DATA* CExDataServerProtocol::GetUnionData(int iGuild)
{
	MAP_MEMBER_DATA::iterator it;
	it = this->m_MapUnionManager.find(iGuild);
	if(it == this->m_MapUnionManager.end())
		return NULL;

	return it->second;
}

BOOL CExDataServerProtocol::DBConnect()
{
	if (FALSE)
	{
		sLog.outError("[ERROR] - EXDATASERVER CANNOT CONNECT TO MSSQL (GUILD)");
		return FALSE;
	}

	if (FALSE)
	{
		sLog.outError("[ERROR] - EXDATASERVER CANNOT CONNECT TO MSSQL (UNION)");
		return FALSE;
	}

	if (FALSE)
	{
		sLog.outError("[ERROR] - EXDATASERVER CANNOT CONNECT TO MSSQL (GUILD MATCHING)");
		return FALSE;
	}

	if (FALSE)
	{
		sLog.outError("[ERROR] - EXDATASERVER CANNOT CONNECT TO MSSQL (PARTY MATCHING)");
		return FALSE;
	}

	sLog.outBasic("[SUCCESS] - EXDATASERVER CONNECT MSSQL SUCCESS");

	return TRUE;
}

BOOL CExDataServerProtocol::Init()
{
	if (!g_UseExDataServer || g_DSMode == TRUE)
	{
		return FALSE;
	}

	if(this->DBConnect() == FALSE)
		return FALSE;

	DWORD dwStartTickCount = GetTickCount();

	this->m_MapGuildManager.clear();

	if(FALSE)
	{
		sLog.outError("[MuOnlineDB] Guild Info is missing");
		return FALSE;
	}

	if(true)
	{
		do
		{
			char szMsg[128];

			char szGuildName[9] = {0};
			char szMaster[11] = {0};
			int iNumber, iType, iScore;
			char szNotice[129] = {0};
			BYTE Mark[32] = {0};
			
			GUILD_INFO_STRUCT* lpGuildInfo = NULL;
			
			wsprintf(szMsg, "SELECT G_Notice FROM Guild Where G_Name = '%s'", szGuildName);
			

			std::ZeroMemory(szMsg, sizeof(szMsg));
			wsprintf(szMsg,"SELECT G_Mark FROM Guild WHERE G_Name = '%s'",szGuildName);
			

			if(AddGuild(szGuildName, szMaster, iNumber, szNotice, iScore, Mark, iType))
			{

//				AddUnion(iNumber, lpGuildInfo->m_iUnionGuild));
			}
			else
			{
				sLog.outError("Can't add guild [%s][%s]", szGuildName, szMaster);
			}


			int iMember = 0;

			while(iMember < MAX_MEMBER_GUILD && false)
			{
				int iStatus;
				char szMembName[11] = {0};
				

				AddGuildMember(szGuildName, szMembName, iStatus);
				iMember++;
			}
		}
		while(false);
	}

	// after all guilds info is loaded
	// add unions :D

	MAP_GUILD_INFO::iterator it, end;
	it = this->m_MapGuildManager.begin();
	end = this->m_MapGuildManager.end();

	GUILD_INFO_STRUCT* guild;
	for (; it != end; it++) {
		guild = (GUILD_INFO_STRUCT*)it->second;
		AddUnion(guild->m_iNumber, guild->m_iUnionGuild);
	}
	sLog.outBasic( "[MuOnlineDB] GuildData Loaded in %d msec", GetTickCount() - dwStartTickCount);


	this->m_GensSystemEDS.InitGensSystem();
	this->m_PartyMatchingEDS.Init();

	return TRUE;
}

void CExDataServerProtocol::ProtocolCore(int aIndex, BYTE HeadCode, LPBYTE aRecv, int iSize)
{
#if (TRACE_PACKET == 1 )
	LogAddHeadHex("EX_DATA_SERVER", aRecv, iSize);
#endif
	switch ( HeadCode )
	{
		case 0x00:
			m_EXDSProtocol.ExDataServerLogin(aIndex, (SDHP_SERVERINFO *)aRecv);
			break;
		case 0x02:
			m_EXDSProtocol.GDCharCloseRecv(aIndex, (SDHP_USERCLOSE *) aRecv);
			break;
		
		case 0x30:
			m_EXDSProtocol.GDGuildCreateSend(aIndex, (SDHP_GUILDCREATE *)aRecv);
			break;

		case 0x31:
			m_EXDSProtocol.GDGuildDestroyRecv(aIndex, (SDHP_GUILDDESTROY *) aRecv);
			break;

		case 0x32:
			m_EXDSProtocol.GDGuildMemberAdd(aIndex, (SDHP_GUILDMEMBERADD *) aRecv);
			break;
		case 0x33:
			m_EXDSProtocol.GDGuildMemberDel(aIndex, (SDHP_GUILDMEMBERDEL *) aRecv);
			break;

		/*case 0x34:	Packet Sended to Update Guilds - Not Called by any function of GS, so here is useless
			break;*/

		case 0x35:
			m_EXDSProtocol.DGGuildMemberInfoRequest(aIndex, (SDHP_GUILDMEMBER_INFO_REQUEST *) aRecv);
			break;

		case 0x37:
			m_EXDSProtocol.DGGuildScoreUpdate(aIndex, (SDHP_GUILDSCOREUPDATE *) aRecv);
			break;
		case 0x38:
			m_EXDSProtocol.GDGuildNoticeSave(aIndex, (SDHP_GUILDNOTICE *) aRecv);
			break;
		case 0x39:
			m_EXDSProtocol.GDGuildMemberAddWithoutUserIndex(aIndex, (SDHP_GUILDMEMBERADD_WITHOUT_USERINDEX *)aRecv);
			break;

		case 0x50:
			m_EXDSProtocol.GDGuildServerGroupChattingSend(aIndex, (EXSDHP_SERVERGROUP_GUILD_CHATTING_RECV *) aRecv);
			break;
		case 0x51:
			m_EXDSProtocol.GDUnionServerGroupChattingSend(aIndex, (EXSDHP_SERVERGROUP_UNION_CHATTING_RECV *) aRecv);
			break;
		case 0x52:
			m_EXDSProtocol.GDGensServerGroupChattingSend(aIndex, (EXSDHP_SERVERGROUP_GENS_CHATTING_RECV *) aRecv);
			break;
		case 0x53:
			{
				PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

				switch ( lpDef->subcode )
				{
					case 0x01:

						break;
					case 0x02:

						break;
				}
			}
			break;	
		case 0xE1:
			m_EXDSProtocol.GDGuildReqAssignStatus(aIndex, (EXSDHP_GUILD_ASSIGN_STATUS_REQ *) aRecv);
			break;
		case 0xE2:
			m_EXDSProtocol.GDGuildReqAssignType(aIndex, (EXSDHP_GUILD_ASSIGN_TYPE_REQ *) aRecv);
			break;

		case 0xE5:
			m_EXDSProtocol.DGRelationShipAnsJoin(aIndex, (EXSDHP_RELATIONSHIP_JOIN_REQ *)aRecv);
			break;

		case 0xE6:
			m_EXDSProtocol.DGRelationShipAnsBreakOff(aIndex, (EXSDHP_RELATIONSHIP_BREAKOFF_REQ *)aRecv);
			break;

		case 0xE9:
			m_EXDSProtocol.DGUnionListRecv(aIndex, (EXSDHP_UNION_LIST_REQ *)aRecv);
			break;

		case 0xEB:
			{
				PMSG_DEFAULT2 * lpDef = (PMSG_DEFAULT2 *)aRecv;

				switch ( lpDef->subcode )
				{
					case 0x01:
						m_EXDSProtocol.DGRelationShipAnsKickOutUnionMember(aIndex, (EXSDHP_KICKOUT_UNIONMEMBER_REQ *)aRecv);
						break;
				}
			}
			break;	
		case 0x60:
			m_EXDSProtocol.m_FriendSystemEDS.FriendListRequest(aIndex, (FHP_FRIENDLIST_REQ*)aRecv);
			break;
		case 0x62:
			m_EXDSProtocol.m_FriendSystemEDS.FriendStateClientRecv( aIndex, (FHP_FRIEND_STATE_C*)aRecv);
			break;

		case 0x63:
			m_EXDSProtocol.m_FriendSystemEDS.FriendAddRequest(aIndex, (FHP_FRIEND_ADD_REQ*)aRecv);
			break;

		case 0x64:
			m_EXDSProtocol.m_FriendSystemEDS.WaitFriendAddRequest( aIndex, (FHP_WAITFRIEND_ADD_REQ*)aRecv);
			break;

		case 0x65:
			m_EXDSProtocol.m_FriendSystemEDS.FriendDelRequest(aIndex, (FHP_FRIEND_ADD_REQ*)aRecv);
			break;
		case 0x70:
			m_EXDSProtocol.m_FriendSystemEDS.FriendMemoSend( aIndex, (FHP_FRIEND_MEMO_SEND*)aRecv);
			break;
		case 0x71:
			m_EXDSProtocol.m_FriendSystemEDS.FriendMemoListReq( aIndex, (FHP_FRIEND_MEMO_LIST_REQ*)aRecv);
			break;
		case 0x72:
			m_EXDSProtocol.m_FriendSystemEDS.FriendMemoReadReq( aIndex, (FHP_FRIEND_MEMO_RECV_REQ*)aRecv);
			break;
		case 0x73:
			m_EXDSProtocol.m_FriendSystemEDS.FriendMemoDelReq( aIndex, (FHP_FRIEND_MEMO_DEL_REQ*)aRecv);
			break;

		case 0x66:
			m_EXDSProtocol.FriendChatRoomCreateReq(aIndex, (FHP_FRIEND_CHATROOM_CREATE_REQ *) aRecv);
			break;

		case 0x74:
			m_EXDSProtocol.FriendChatRoomInvitationReq(aIndex, (FHP_FRIEND_INVITATION_REQ *) aRecv);
			break;

		case 0xA0:
			m_EXDSProtocol.FriendChatRoomCreateAns(aIndex, (FCHP_CHATROOM_CREATE_RESULT*) aRecv);
			break;

		case 0xA3:
			{
				PMSG_DEFAULT2 * lpMsg = (PMSG_DEFAULT2 *)(aRecv);

				switch (lpMsg->subcode)
				{
					case 0x00:
						m_EXDSProtocol.GDReqGuildMatchingList(aIndex, (_stReqGuildMatchingList *)aRecv);
						break;
					case 0x01:
						m_EXDSProtocol.GDReqGuildMatchingListSearchWord(aIndex, (_stReqGuildMatchingListSearchWord *)aRecv);
						break;
					case 0x02:
						m_EXDSProtocol.GDReqRegGuildMatchingList(aIndex, (_stReqGuildMatchingData *)aRecv);
						break;
					case 0x03:
						m_EXDSProtocol.GDReqDelMatchingList(aIndex, (_stReqDelGuildMatchingList *)aRecv);
						break;
					case 0x04:
						m_EXDSProtocol.GDReqJoinGuildMatchingList(aIndex, (_stRegWaitGuildMatching *)aRecv);
						break;
					case 0x05:
						m_EXDSProtocol.GDReqCancelJoinGuildMatching(aIndex, (_stReqDelWaitGuildMatchingList *)aRecv);
						break;
					case 0x06:
						m_EXDSProtocol.GDReqAllowJoinGuildMatching(aIndex, (_stReqAllowJoinGuildMatching *)aRecv);
						break;
					case 0x07:
						m_EXDSProtocol.GDReqWaitGuildMatchingList(aIndex, (_stReqWaitGuildMatchingList *)aRecv);
						break;
					case 0x08:
						m_EXDSProtocol.GDReqGetWaitStateListGuildMatching(aIndex, (_stReqWaitStateListGuildMatching *)aRecv);
						break;
				}
			}
			break;

		case 0xA4:
			{
				PMSG_DEFAULT2 * lpMsg = (PMSG_DEFAULT2 *)(aRecv);

				switch (lpMsg->subcode)
				{
					case 0x00:
						m_EXDSProtocol.GDReqRegWantedPartyMember(aIndex, (_stReqRegWantedPartyMember *)aRecv);
						break;
					case 0x01:
						m_EXDSProtocol.GDReqGetPartyMatchingList(aIndex, (_stReqGetPartyMatchingList *)aRecv);
						break;
					case 0x02:
						m_EXDSProtocol.GDReqJoinMemberPartyMatching(aIndex, (_stReqJoinMemberPartyMatching *)aRecv);
						break;
					case 0x03:
						m_EXDSProtocol.GDReqJoinMemberStateList(aIndex, (_stReqJoinMemberStateListPartyMatching *)aRecv);
						break;
					case 0x04:
						m_EXDSProtocol.GDReqJoinMemberStateListLeader(aIndex, (_stReqWaitListPartyMatching *)aRecv);
						break;
					case 0x05:
						m_EXDSProtocol.GDReqAcceptMemberJoin(aIndex, (_stReqAddPartyMember *)aRecv);
						break;
					case 0x06:
						m_EXDSProtocol.GDReqCancelPartyMatching(aIndex, (_stReqCancelPartyMatching *)aRecv);
						break;
					case 0x07:
						m_EXDSProtocol.GDReqDeletePartyUser(aIndex, (_stReqDelPartyUserPartyMatching *)aRecv);
						break;
					case 0x11:
						m_EXDSProtocol.GDReqSendPartyMemberList(aIndex, (_stReqSendPartyMemberList *)aRecv);
						break;
					case 0x12:
						m_EXDSProtocol.GDSendChatMsgPartyMatching(aIndex, (_stReqChattingPartyMatching *)aRecv);
						break;
				}
			}
			break;

		case 0xF8:
			{
				PMSG_DEFAULT2 * lpMsg = (PMSG_DEFAULT2 *)(aRecv);

				switch ( lpMsg->subcode )
				{
					case 0x01:
						m_EXDSProtocol.m_GensSystemEDS.GDReqGensInfo(aIndex, (PMSG_REQ_ABUSING_INFO *) aRecv);
						break;

					case 0x03:

						break;

					case 0x05:

						break;

					case 0x07:

						break;

					case 0x08:

						break;
						
					case 0x09:
						m_EXDSProtocol.m_GensSystemEDS.GDReqAbusingInfo(aIndex, (PMSG_REQ_ABUSING_INFO *)aRecv);
						break;

					case 0x0C:

						break;

					case 0x0E:

						break;

					case 0x0F:
						m_EXDSProtocol.m_GensSystemEDS.GDReqGensMemberCount(aIndex, (PMSG_REQ_GENS_MEMBER_COUNT *)aRecv);
						break;

					case 0x11:
						m_EXDSProtocol.m_GensSystemEDS.GDReqGensMemberCount(aIndex, (PMSG_REQ_GENS_MEMBER_COUNT *)aRecv);
						break;

					case 0x12:
						m_EXDSProtocol.m_GensSystemEDS.ManualRefreshRanking(1);
						break;

					case 0x13:
						m_EXDSProtocol.m_GensSystemEDS.GDReqGensRewardDay(aIndex, (PMSG_REQ_GENS_REWARD_DAY *)aRecv);
						break;

				}
			}
		break;
						

	}
}

void CExDataServerProtocol::ExDataServerLogin(int aIndex, SDHP_SERVERINFO * lpMsg)
{
	SDHP_RESULT pResult = {0};

	PHeadSetB((LPBYTE)&pResult, 0x00, sizeof(pResult));
	pResult.Result = 1;

	// Case GameServer
	if ( lpMsg->Type == 1 )
	{
		int i;
		for(i=0; i < g_dwMaxServerGroups; i++)
		{
			if(g_Server[i].m_Index != -1
				&& g_Server[i].m_State == SS_GUILD
				&& g_Server[i].m_ServerCode == lpMsg->ServerCode)
				break;
		}

		if(i == g_dwMaxServerGroups)
		{
			g_Server[aIndex].m_State = SS_GUILD;
			g_Server[aIndex].m_ServerCode = lpMsg->ServerCode;

			MAP_GUILD_INFO::iterator it, end;
			it = this->m_MapGuildManager.begin();
			end = this->m_MapGuildManager.end();
			for (; it != end; it++)
				((GUILD_INFO_STRUCT*)it->second)->m_bGSHasData = 0;

			sLog.outBasic("[ExDB] GameServer with ServerCode[%d] connected.", lpMsg->ServerCode);

		}
		else
		{
			sLog.outError("[ExDB] GameServer with ServerCode[%d] is already connected.", lpMsg->ServerCode);
			pResult.Result = 0;
		}
	}
	// Case ChatServer
	else if ( lpMsg->Type == 2 )
	{
		g_Server[aIndex].m_State = SS_CHAT;
		sLog.outError("[ChatServer] Chat Server Connected");
	}

	DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size, __FUNCTION__);
}

void CExDataServerProtocol::GDCharCloseRecv(int aIndex, SDHP_USERCLOSE * aRecv)
{
	if (this->m_PartyMatchingEDS.DisconnectMember(aRecv->CharName) == TRUE)
	{
		PARTYMATCHING_PARTYDATA * lpPartyData = this->m_PartyMatchingEDS.GetPartyDataByMember(aRecv->CharName);

		if (lpPartyData)
		{
			this->SendPartyMatchingMemberList(lpPartyData->szLeaderName, 0);
		}
	}

	if( aRecv->Type == 1 )
	{
		this->m_FriendSystemEDS.FriendClose(aIndex, (LPBYTE)aRecv);
	}

	else
	{
		this->m_FriendSystemEDS.FriendClose(aIndex, (LPBYTE)aRecv);

		char szGuild[9] = {0};
        char szName[11] = {0};
        strncpy(szGuild, aRecv->GuildName, 8);
        strncpy(szName, aRecv->CharName, 10);

		GUILD_MEMBER* lpMemb = (GUILD_MEMBER*) GetGuildMember(aRecv->GuildName, aRecv->CharName);
		if (lpMemb)
		{
			lpMemb->m_btConnected = -1;
			lpMemb->m_iUserIndex = -1;
		}
			

		sLog.outBasic("[ExDB] member close: [%s]", aRecv->CharName);

		for(int i=0; i < g_dwMaxServerGroups; i++)
		{
			if(g_Server[i].m_Index != -1
				&& g_Server[i].m_Type == ST_EXDATASERVER
				&& g_Server[i].m_State == SS_GUILD)
			{
				DataSend(i, (LPBYTE)aRecv, aRecv->h.size, __FUNCTION__);
			}
		}
	}
}

void CExDataServerProtocol::GDGuildCreateSend(int aIndex, SDHP_GUILDCREATE * aRecv)
{
	SDHP_GUILDCREATE_RESULT Result;

	PHeadSetB((LPBYTE)&Result, 0x30, sizeof(Result));

	std::memcpy(Result.Master, aRecv->Master, sizeof(Result.Master));
	std::memcpy(Result.GuildName, aRecv->GuildName, sizeof(Result.GuildName));
	std::memcpy(Result.Mark, aRecv->Mark, sizeof(Result.Mark));
	Result.NumberL = aRecv->NumberL;
	Result.NumberH = aRecv->NumberH;

	char szMaster[11];
	std::memcpy(szMaster, aRecv->Master, 10);

	Result.Result = 0;
	Result.Flag = 1;

	if(strlen(aRecv->GuildName) < 4)
	{
		Result.Result = 2;
		Result.Flag = 1;
		sLog.outBasic( "GuildName is too short [%s]", aRecv->GuildName);

		sLog.outBasic("[Guild Create Request] Send Result [%d].", Result.Result);
		DataSend(aIndex, (LPBYTE)&Result, sizeof(Result), __FUNCTION__);
		return;
	}

	if(strlen(aRecv->Master) < 3)
	{
		Result.Result = 2;
		Result.Flag = 1;
		sLog.outBasic( "GuildMaster Name is too short [%s]", aRecv->Master);

		sLog.outBasic("[Guild Create Request] Send Result [%d].", Result.Result);
		DataSend(aIndex, (LPBYTE)&Result, sizeof(Result), __FUNCTION__);
		return;
	}

	if(!SpaceSyntexCheck(aRecv->GuildName))
	{
		Result.Result = 4;
		Result.Flag = 1;

		sLog.outBasic("[Guild Create Request] Send Result [%d].", Result.Result);
		DataSend(aIndex, (LPBYTE)&Result, Result.h.size, __FUNCTION__);
		return;
	}

	else if(!QuoteSpaceSyntaxCheck(aRecv->GuildName))
	{
		Result.Result = 5;
		Result.Flag = 1;

		sLog.outBasic("[Guild Create Request] Send Result [%d].", Result.Result);
		DataSend(aIndex, (LPBYTE)&Result, Result.h.size, __FUNCTION__);
		return;
	}

	else if(!PercentSyntaxCheck(aRecv->GuildName))
	{
		Result.Result = 5;
		Result.Flag = 1;

		sLog.outBasic("[Guild Create Request] Send Result [%d].", Result.Result);
		DataSend(aIndex, (LPBYTE)&Result, Result.h.size, __FUNCTION__);
		return;
	}

	else
	{
		sLog.outBasic("[Guild Create Request] Guild [%s], GuildMaster [%s].", Result.GuildName, Result.Master);
		int res = CreateDBGuild(aRecv->GuildName, aRecv->Master, aRecv->Mark);
		
		switch(res)
		{
			case 1:
				Result.Result = 0;
			break;
			
			case 2:
				Result.Result = 3;
			break;

			case 3:
				Result.Result = -1;
			break;
		}

		if(res == 0)
		{
			Result.GuildNumber = GetDBGuildNumber(aRecv->GuildName);
			if(Result.GuildNumber != -1)
			{
				AddGuild(aRecv->GuildName, aRecv->Master, Result.GuildNumber, NULL, 0, aRecv->Mark, aRecv->btGuildType);
				if(UpdateGuildMemberStatus(aRecv->GuildName, aRecv->Master, 128))
				{
					if(UpdateGuildType(aRecv->GuildName, aRecv->btGuildType))
					{
				
						Result.Result = 1;

						for(int i=0; i < g_dwMaxServerGroups; i++)
						{
							if(g_Server[i].m_Index != -1
								&& g_Server[i].m_Type == ST_EXDATASERVER
								&& g_Server[i].m_State == SS_GUILD)
							{
								if(i == aIndex)
									Result.Flag = 1;
								else
									Result.Flag = 0;
								sLog.outBasic("[Guild Create Request] Send Result [%d] to Server [%d]", Result.Result, g_Server[i].m_ServerCode);			
								DataSend(i, (LPBYTE)&Result, Result.h.size, __FUNCTION__);
							}
						}
						
						DGGuildMasterListSend(aRecv->GuildName);
						SendGuildMemberInfo(aIndex, aRecv->Master, MAKE_NUMBERW(Result.NumberH, Result.NumberL));
						sLog.outBasic("[Guild Create Request] Create Guild: [%s], Master: [%s]", aRecv->GuildName, aRecv->Master);
						return;
					}
					else
					{
						sLog.outBasic("[Guild Create Request] [ERROR] UpdateGuildType: Guild: [%s], Type: [%d]", aRecv->GuildName, aRecv->btGuildType);
					}
				}
				else
				{
					sLog.outBasic("[Guild Create Request] [ERROR] UpdateGuildMemberStatus: Guild: [%s], Member: [%s]", aRecv->GuildName, aRecv->Master);
				}
			}
		}
		else
		{
			sLog.outBasic("[Guild Create Request] Send Result [%d].", Result.Result);
			DataSend(aIndex, (LPBYTE)&Result, Result.h.size, __FUNCTION__);
			return;
		}
	}
}

int CExDataServerProtocol::CreateDBGuild(char* szGuild, char* szMaster, LPBYTE pMark)
{
	if(this->GuildExists(szGuild))
	{
		sLog.outBasic( "Guild [%s] already exists.", szGuild);
		return 1;
	}

	return 0;
}

int CExDataServerProtocol::GetDBGuildNumber(char *szGuild)
{
	return 0;
}

BOOL CExDataServerProtocol::AddGuild(char *szGuild, char *szMaster, int iNumber, char *lpNotice, int iScore, LPBYTE pMark, int iType)
{
	if(GuildExists(szGuild))
		return FALSE;

	GUILD_INFO_STRUCT* Guild = new GUILD_INFO_STRUCT;
	Guild->m_iNumber = iNumber;
	strcpy(Guild->m_szGuildName, szGuild);
	strcpy(Guild->m_szGuildMaster, szMaster);
	std::memcpy(Guild->m_Mark, pMark, 32);
	if(lpNotice)
		strncpy(Guild->m_Notice, lpNotice, 128);
	Guild->m_Score = iScore;
	Guild->m_btGuildType = iType;
	Guild->m_bGSHasData = FALSE;
	
	STR_GUILD_MEMBER* memb = new STR_GUILD_MEMBER();
	memb->m_btConnected = -1;
	memb->m_btStatus = 128;
	strncpy(memb->m_szMemberName, szMaster, 10);
	
	Guild->m_mapGuildMember[szMaster] = memb;
	m_MapGuildManager.emplace(szGuild, Guild);

	CreateRelationShipData(iNumber);

	sLog.outBasic("[AddGuild] Guild [%s][%d] Create Guild Master: [%s]", Guild->m_szGuildName, iNumber, Guild->m_szGuildMaster);

	return TRUE;
}

BOOL CExDataServerProtocol::DelGuild(char *szGuild)
{
	MAP_GUILD_INFO::iterator it;
	it = this->m_MapGuildManager.find(szGuild);
	if(it == this->m_MapGuildManager.end())
		return FALSE;

	((GUILD_INFO_STRUCT*)it->second)->m_mapGuildMember.clear();
	this->m_MapGuildManager.erase(it);

	return TRUE;
}

BOOL CExDataServerProtocol::MasterDBExists(char *szMaster)
{
	BOOL res = FALSE;

	return TRUE;
}

BOOL CExDataServerProtocol::DelDBGuild(char *szGuild)
{
	BOOL res;
	return res;
}

BOOL CExDataServerProtocol::DelAllDBGuildMember(char *szGuild)
{
	BOOL res;
	return res;
}

BOOL CExDataServerProtocol::AddGuildMember(char *szGuild, char *szName, BYTE btStatus, BYTE btConnected)
{
	GUILD_INFO_STRUCT* lpGuild;
	lpGuild = this->GetGuild(szGuild);
	if(lpGuild == NULL)
		return FALSE;

	if(this->GuildMemberExists(szGuild, szName))
		return FALSE;

	STR_GUILD_MEMBER* Memb = new STR_GUILD_MEMBER();
	strcpy(Memb->m_szMemberName, szName);
	Memb->m_btStatus = btStatus;
	Memb->m_btConnected = btConnected;

	lpGuild->m_mapGuildMember[szName] = Memb;
	
	sLog.outBasic("[AddGuildMember] Guild [%s] Member Add: [%s]", szGuild, Memb->m_szMemberName);
	return TRUE;
}

BOOL CExDataServerProtocol::UpdateGuildMemberStatus(char *szGuild, char *szName, BYTE btStatus)
{
	STR_GUILD_MEMBER* lpMemb;
	lpMemb = GetGuildMember(szGuild, szName);
	if(lpMemb == NULL)
	{
		if(GuildExists(szGuild))
		{
			sLog.outBasic( "[ERROR] Guild Member Status: member [%s] doesn't exist in guild [%s]", szName, szGuild);
			return FALSE;
		}
		else
		{
			sLog.outBasic( "[ERROR] Guild Member Status: guild [%s] doesn't exist", szGuild);
			return FALSE;
		}
	}

	lpMemb->m_btStatus = btStatus;

	return TRUE;
}

BOOL CExDataServerProtocol::UpdateGuildType(char *szGuild, BYTE btType)
{
	GUILD_INFO_STRUCT* lpGuild;
	lpGuild = GetGuild(szGuild);
	
	if(lpGuild == NULL)
	{
		sLog.outError("[ERROR] Guild Type: guild [%s] doesn't exist", szGuild);
		return FALSE;
	}

	lpGuild->m_btGuildType = btType;

	return TRUE;
}

void CExDataServerProtocol::SendGuildMemberInfo(int aIndex, char *szName, int iUserIndex)
{
//	char szMember[11] = {0};
//	strncpy(szMember, szName, 10);

	char szGuild[11] = {0};
	int iLevel, iStatus;
	if(GetGuildMemberInfo(szName, szGuild, iLevel, iStatus) == FALSE)
	{
		sLog.outBasic("[Send Guild Member Info] No info returned from DB.");
		return;
	}

	GUILD_INFO_STRUCT* lpGuild;
	STR_GUILD_MEMBER* lpMemb;

	lpGuild = GetGuild(szGuild);
	if(lpGuild == NULL)
	{
		sLog.outBasic("[Send Guild Member Info] error: Guild [%s] not found.", szGuild );
		return;
	}

	lpMemb = GetGuildMember(szGuild, szName);
	if(lpMemb == NULL)
	{
		sLog.outBasic("[Send Guild Member Info] error: Member [%s] not found in Guild [%s].", szName, szGuild );
		return;
	}

	lpMemb->m_btConnected = g_Server[aIndex].m_ServerCode;
	lpMemb->m_iUserIndex = iUserIndex;

	DGGuildMasterListSend(szGuild);

	DGRelationShipListSend(aIndex, lpGuild->m_iNumber, 1, 0);
	DGRelationShipListSend(aIndex, lpGuild->m_iNumber, 2, 0);

	SDHP_GUILDMEMBER_INFO GuildMembInfo;
	ZeroMemory(&GuildMembInfo, sizeof(GuildMembInfo));
	PHeadSetB((LPBYTE)&GuildMembInfo, 0x35, sizeof(GuildMembInfo));

	GuildMembInfo.btGuildStatus = lpMemb->m_btStatus;
	strncpy(GuildMembInfo.MemberID, lpMemb->m_szMemberName, 10);
	GuildMembInfo.pServer = lpMemb->m_btConnected;
	GuildMembInfo.btGuildType = lpGuild->m_btGuildType;
	strncpy(GuildMembInfo.GuildName, lpGuild->m_szGuildName, 8);

	sLog.outBasic("[Send Guild Member Info] Guild information send: Name[%s] Guild[%s]", szName, szGuild);

	for(int i=0; i < g_dwMaxServerGroups; i++)
	{
		if(g_Server[i].m_Index != -1
			&& g_Server[i].m_Type == ST_EXDATASERVER
			&& g_Server[i].m_State == SS_GUILD)
			{

				DataSend(i, (LPBYTE)&GuildMembInfo, GuildMembInfo.h.size, __FUNCTION__);
			}
	}

	if (lpMemb->m_btStatus == 128)
	{
		this->SendUseGuildMatchingGuild(lpMemb->m_szMemberName, lpGuild->m_iNumber);
		this->SendNotiGuildMatchingForGuildMaster(lpGuild->m_iNumber);
	}
}

BOOL CExDataServerProtocol::GetGuildMemberInfo(char *szName, OUT char *szGuild, OUT int& riLevel, OUT int& riStatus)
{

	return TRUE;
}

void CExDataServerProtocol::DGGuildMasterListSend(char *szGuild)
{
	GUILD_INFO_STRUCT* lpGuild;
	lpGuild = GetGuild(szGuild);

	if(lpGuild == NULL)
		return;

	if (lpGuild->m_bGSHasData == TRUE)
	{
		return;
	}

	int bufsize = sizeof(SDHP_GUILDALL_COUNT)+sizeof(SDHP_GUILDALL)*MAX_MEMBER_GUILD;
	BYTE *lpData = new BYTE[bufsize];
	ZeroMemory(lpData, bufsize);

	SDHP_GUILDALL_COUNT* lpGuildAllCnt;
	lpGuildAllCnt = (SDHP_GUILDALL_COUNT*)lpData;

	SDHP_GUILDALL* lpGuildAll;
	lpGuildAll = (SDHP_GUILDALL*)(lpData+sizeof(SDHP_GUILDALL_COUNT));

	lpGuildAllCnt->btGuildType = lpGuild->m_btGuildType;
	strncpy(lpGuildAllCnt->GuildName, lpGuild->m_szGuildName, 8);
	strncpy(lpGuildAllCnt->Master, lpGuild->m_szGuildMaster, 10);

	lpGuildAllCnt->iGuildRival = lpGuild->m_iRivalGuild;
	if(lpGuild->m_iRivalGuild)
	{
		GUILD_INFO_STRUCT* lpG;
		lpG = GetGuild(lpGuild->m_iRivalGuild);
		if(lpG)
			strncpy(lpGuildAllCnt->szGuildRivalName, lpG->m_szGuildName, 8);

	}
	lpGuildAllCnt->iGuildUnion = lpGuild->m_iUnionGuild;

	std::memcpy(lpGuildAllCnt->Mark, lpGuild->m_Mark, 32);
	lpGuildAllCnt->Number = lpGuild->m_iNumber;
	lpGuildAllCnt->score = lpGuild->m_Score;

	MAP_GUILD_MEMBER::iterator it, end;
	it = lpGuild->m_mapGuildMember.begin();
	end = lpGuild->m_mapGuildMember.end();

	int i;
	for(i=0; i < MAX_MEMBER_GUILD && it != end; it++, i++)
	{
		strncpy(lpGuildAll[i].MemberID, it->second->m_szMemberName, 10);
		lpGuildAll[i].btGuildStatus = it->second->m_btStatus;
		lpGuildAll[i].pServer = it->second->m_btConnected;

		sLog.outBasic("[GuildMasterListSend] Guild [%s] Member send[%s]", szGuild, it->second->m_szMemberName);
	}

	lpGuildAllCnt->Count = i;

	int datasize = sizeof(SDHP_GUILDALL_COUNT)+ i*sizeof(SDHP_GUILDALL);

	PHeadSetW((LPBYTE)lpGuildAllCnt, 0x36, datasize);

	for(i=0; i < g_dwMaxServerGroups; i++)
	{
		if(g_Server[i].m_Index != -1
			&& g_Server[i].m_Type == ST_EXDATASERVER 
			&& g_Server[i].m_State == SS_GUILD)
			{

				DataSend(i, (LPBYTE)lpData, datasize, __FUNCTION__);
			}
	}

	delete [] lpData;

	lpGuild->m_bGSHasData = TRUE;
}

void CExDataServerProtocol::DGGuildInfoRequest(int aIndex, SDHP_GUILDMEMBER_INFO_GUILDNAME_REQUEST * aRecv)
{
	GUILD_INFO_STRUCT* lpGuild;
	lpGuild = GetGuild(aRecv->szGuildName);

	if (lpGuild == NULL)
		return;

	if (lpGuild->m_bGSHasData == TRUE)
	{
		return;
	}

	int bufsize = sizeof(SDHP_GUILDALL_COUNT) + sizeof(SDHP_GUILDALL)*MAX_MEMBER_GUILD;
	BYTE *lpData = new BYTE[bufsize];
	ZeroMemory(lpData, bufsize);

	SDHP_GUILDALL_COUNT* lpGuildAllCnt;
	lpGuildAllCnt = (SDHP_GUILDALL_COUNT*)lpData;

	SDHP_GUILDALL* lpGuildAll;
	lpGuildAll = (SDHP_GUILDALL*)(lpData + sizeof(SDHP_GUILDALL_COUNT));

	lpGuildAllCnt->btGuildType = lpGuild->m_btGuildType;
	strncpy(lpGuildAllCnt->GuildName, lpGuild->m_szGuildName, 8);
	strncpy(lpGuildAllCnt->Master, lpGuild->m_szGuildMaster, 10);

	lpGuildAllCnt->iGuildRival = lpGuild->m_iRivalGuild;
	if (lpGuild->m_iRivalGuild)
	{
		GUILD_INFO_STRUCT* lpG;
		lpG = GetGuild(lpGuild->m_iRivalGuild);
		if (lpG)
			strncpy(lpGuildAllCnt->szGuildRivalName, lpG->m_szGuildName, 8);

	}
	lpGuildAllCnt->iGuildUnion = lpGuild->m_iUnionGuild;

	std::memcpy(lpGuildAllCnt->Mark, lpGuild->m_Mark, 32);
	lpGuildAllCnt->Number = lpGuild->m_iNumber;
	lpGuildAllCnt->score = lpGuild->m_Score;

	MAP_GUILD_MEMBER::iterator it, end;
	it = lpGuild->m_mapGuildMember.begin();
	end = lpGuild->m_mapGuildMember.end();

	int i;
	for (i = 0; i < MAX_MEMBER_GUILD && it != end; it++, i++)
	{
		strncpy(lpGuildAll[i].MemberID, it->second->m_szMemberName, 10);
		lpGuildAll[i].btGuildStatus = it->second->m_btStatus;
		lpGuildAll[i].pServer = it->second->m_btConnected;

		sLog.outBasic("[GuildMasterListSend] Guild [%s] Member send[%s]", aRecv->szGuildName, it->second->m_szMemberName);
	}

	lpGuildAllCnt->Count = i;

	int datasize = sizeof(SDHP_GUILDALL_COUNT) + i*sizeof(SDHP_GUILDALL);

	PHeadSetW((LPBYTE)lpGuildAllCnt, 0x36, datasize);

	for (i = 0; i < g_dwMaxServerGroups; i++)
	{
		if (g_Server[i].m_Index != -1
			&& g_Server[i].m_Type == ST_EXDATASERVER
			&& g_Server[i].m_State == SS_GUILD)
		{

			DataSend(i, (LPBYTE)lpData, datasize, __FUNCTION__);
		}
	}

	delete[] lpData;

	lpGuild->m_bGSHasData = TRUE;
}

void CExDataServerProtocol::DGGuildMemberInfoRequest(int aIndex, SDHP_GUILDMEMBER_INFO_REQUEST * aRecv)
{
	char szName[11] = {0};
	strncpy(szName, aRecv->MemberID, 10);

	sLog.outBasic("[Guild Member Info Request] Member [%s].",szName);

	int iUserIndex = MAKE_NUMBERW(aRecv->NumberH, aRecv->NumberL);

	SendGuildMemberInfo(aIndex, szName, iUserIndex);
}

void CExDataServerProtocol::GDGuildDestroyRecv(int aIndex, SDHP_GUILDDESTROY * aRecv)
{
	SDHP_GUILDDESTROY_RESULT DelResult;
	ZeroMemory(&DelResult, sizeof(DelResult));
	PHeadSetB((LPBYTE)&DelResult, 0x31, sizeof(DelResult));

	strncpy(DelResult.GuildName, aRecv->GuildName, 8);
	strncpy(DelResult.Master, aRecv->Master, 10);
	DelResult.NumberH = aRecv->NumberH;
	DelResult.NumberL = aRecv->NumberL;

	DelResult.Flag = 1;
	DelResult.Result = 0;

	sLog.outBasic("[Guild Destroy Request] GuildName [%s], GuildMaster [%s].", DelResult.GuildName, DelResult.Master);

	if(strlen(DelResult.Master) < 3)
	{
		sLog.outBasic( "error-L3 MasterName < 3: [%s]", DelResult.Master);
		DelResult.Result = 3;
	}

	if(strlen(DelResult.GuildName) < 2)
	{
		sLog.outBasic( "error-L3 GuildName < 2: [%s]", DelResult.GuildName);
		DelResult.Result = 3;
	}

	if(DelResult.Result == 3)
	{
		for(int i=0; i < g_dwMaxServerGroups; i++)
		{
			if(g_Server[i].m_Index != -1
				&& g_Server[i].m_Type == ST_EXDATASERVER
				&& g_Server[i].m_State == SS_GUILD)
				{
					if(i == aIndex)
						DelResult.Flag = 1;
					else
						DelResult.Flag = 0;

					sLog.outBasic("[Guild Destroy Request] Send Result [%d] to Server [%d].", DelResult.Result, g_Server[i].m_ServerCode);
					DataSend(i, (LPBYTE)&DelResult, DelResult.h.size, __FUNCTION__);
				}
		}
		return;
	}

	if(IsCSGuild(aRecv->GuildName))
	{
		DelResult.Result = 3;

		for(int i=0; i < g_dwMaxServerGroups; i++)
		{
			if(g_Server[i].m_Index != -1
				&& g_Server[i].m_Type == ST_EXDATASERVER
				&& g_Server[i].m_State == SS_GUILD)
				{
					if(i == aIndex)
						DelResult.Flag = 1;
					else
						DelResult.Flag = 0;
				
					sLog.outBasic("[Guild Destroy Request] Send Result [%d] to Server [%d].", DelResult.Result, g_Server[i].m_ServerCode);
					DataSend(i, (LPBYTE)&DelResult, DelResult.h.size, __FUNCTION__);
				}
		}

		return;
	}

	DelResult.Result = 2;

	GUILD_INFO_STRUCT* lpGuild;
	lpGuild = GetGuild(DelResult.GuildName);
	if(lpGuild)
	{
		if(lpGuild->m_iUnionGuild != 0)
		{		
			if(lpGuild->m_iUnionGuild != lpGuild->m_iNumber)
			{
				if(UnionBreak(GetGuild(lpGuild->m_iUnionGuild), lpGuild) == 0)
					DelResult.Result = 3;
			}
			else
				DelResult.Result = 3;
		}

		if(lpGuild->m_iRivalGuild != 0)
		{
			if(RivalBreak(lpGuild, GetGuild(lpGuild->m_iRivalGuild)) == 0)
				DelResult.Result = 3;
		}

	}
	else
		sLog.outBasic("[Guild Destroy Request] Guild [%s] not found.", DelResult.Result);

	if(DelResult.Result == 3)
	{
		for(int i=0; i < g_dwMaxServerGroups; i++)
		{
			if(g_Server[i].m_Index != -1
				&& g_Server[i].m_Type == ST_EXDATASERVER
				&& g_Server[i].m_State == SS_GUILD)
				{
					if(i == aIndex)
						DelResult.Flag = 1;
					else
						DelResult.Flag = 0;

					sLog.outBasic("[Guild Destroy Request] Send Result [%d] to Server [%d].", DelResult.Result, g_Server[i].m_ServerCode);
					DataSend(i, (LPBYTE)&DelResult, DelResult.h.size, __FUNCTION__);
				}
		}
		return;
	}

	if(MasterDBExists(DelResult.Master))
	{
		if(DelAllDBGuildMember(DelResult.GuildName))
		{
			if(DelDBGuild(DelResult.GuildName))
			{
				DelGuild(DelResult.GuildName);
				DelResult.Result = 4;
			}
			else
			{
				sLog.outBasic("[Guild Destroy Request] [DelAllDBGuildMember] Failed to delete All GuildMember in DB : %s", DelResult.GuildName);
			}
		}
		else
		{
			sLog.outBasic("[Guild Destroy Request] [DelDBGuild] Failed to delete Guild in DB : %s", DelResult.GuildName);
		}
	}
	else
		sLog.outBasic("[Guild Destroy Request] GuildMaster [%s] not found in DB.", DelResult.Result);

	for(int i=0; i < g_dwMaxServerGroups; i++)
	{
		if(g_Server[i].m_Index != -1
			&& g_Server[i].m_Type == ST_EXDATASERVER
			&& g_Server[i].m_State == SS_GUILD)
			{
				if(i == aIndex)
					DelResult.Flag = 1;
				else
					DelResult.Flag = 0;

				sLog.outBasic("[Guild Destroy Request] Send Result [%d] to Server [%d].", DelResult.Result, g_Server[i].m_ServerCode);
				DataSend(i, (LPBYTE)&DelResult, DelResult.h.size, __FUNCTION__);
			}
	}
}

BOOL CExDataServerProtocol::IsCSGuild(char *szGuild)
{
	return FALSE;
}

BOOL CExDataServerProtocol::UpdateScore(char *szGuild, int iScore)
{
	return FALSE;
}

BOOL CExDataServerProtocol::SaveNotice(char *szGuild, char *szNotice)
{

	// set Notice
	char szTmp[256]={0};
	char szDBNotice[129] = {0};

	//sprintf(szDBNotice, szNotice);
	std::memcpy(szDBNotice,szNotice,strlen(szNotice));
	for(int i=0;i<sizeof(szNotice);i++)
	{
		if(szNotice[i] == '%')
		{
			return TRUE;
		}
	}
	wsprintf(szTmp, "UPDATE Guild SET G_Notice=? where G_Name='%s'", szGuild);



	return TRUE;
}

void CExDataServerProtocol::DGGuildScoreUpdate(int aIndex, SDHP_GUILDSCOREUPDATE * aRecv)
{
	sLog.outBasic("[Guild Score Update Request] GuildName [%s], Score [%d]", aRecv->GuildName, aRecv->Score);

	if(strlen(aRecv->GuildName) < 4)
	{
		sLog.outBasic( "error-L2: [%s] guild name is short", aRecv->GuildName);
		return;
	}

	if(UpdateScore(aRecv->GuildName, aRecv->Score))
	{

		sLog.outBasic("[Guild Score Update Request] Score updated.");
		for(int i=0; i < g_dwMaxServerGroups; i++)
		{
			if(g_Server[i].m_Index != -1
				&& g_Server[i].m_Type == ST_EXDATASERVER
				&& g_Server[i].m_State == SS_GUILD)
				{
					DataSend(i, (LPBYTE)aRecv, aRecv->h.size, __FUNCTION__);
				}
		}
	}
	else
		sLog.outBasic("[Guild Score Update Request] Failed to update guild score.");
}

void CExDataServerProtocol::GDGuildNoticeSave(int aIndex, SDHP_GUILDNOTICE * aRecv)
{
	sLog.outBasic("[Guild Notice Save Request] GuildName [%s].", aRecv->GuildName);

	if(strlen(aRecv->GuildName) < 4)
	{
		sLog.outBasic( "error-L2: [%s] guild name is short", aRecv->GuildName);
		return;
	}

	if (PercentSyntaxCheck(aRecv->szGuildNotice) == FALSE)
	{
		return;
	}

	if(SaveNotice(aRecv->GuildName, aRecv->szGuildNotice))
	{

		sLog.outBasic("[Guild Notice Save Request] Notice Saved.");
		for(int i=0; i < g_dwMaxServerGroups; i++)
		{
			if(g_Server[i].m_Index != -1
				&& g_Server[i].m_Type == ST_EXDATASERVER
				&& g_Server[i].m_State == SS_GUILD)
				{

					DataSend(i, (LPBYTE)aRecv, aRecv->h.size, __FUNCTION__);
				}
		}
	}
	else
		sLog.outBasic("[Guild Notice Save Request] Failed to save.");

}

void CExDataServerProtocol::GDGuildServerGroupChattingSend(int aIndex, EXSDHP_SERVERGROUP_GUILD_CHATTING_RECV * aRecv)
{

	for(int i=0; i < g_dwMaxServerGroups; i++)
	{
		if(g_Server[i].m_Index != -1
			&& g_Server[i].m_Type == ST_EXDATASERVER
			&& g_Server[i].m_State == SS_GUILD)
			{
				if(i != aIndex)
					DataSend(i, (LPBYTE)aRecv, aRecv->h.size, __FUNCTION__);
			}
	}
}

void CExDataServerProtocol::GDUnionServerGroupChattingSend(int aIndex,EXSDHP_SERVERGROUP_UNION_CHATTING_RECV * aRecv)
{
	for(int i=0; i < g_dwMaxServerGroups; i++)
	{
		if(g_Server[i].m_Index != -1
			&& g_Server[i].m_Type == ST_EXDATASERVER
			&& g_Server[i].m_State == SS_GUILD)
			{
				if(i != aIndex)
					DataSend(i, (LPBYTE)aRecv, aRecv->h.size, __FUNCTION__);
			}
	}
}

void CExDataServerProtocol::GDGensServerGroupChattingSend(int aIndex,EXSDHP_SERVERGROUP_GENS_CHATTING_RECV * aRecv)
{
	for(int i=0; i < g_dwMaxServerGroups; i++)
	{
		if(g_Server[i].m_Index != -1
			&& g_Server[i].m_Type == ST_EXDATASERVER
			&& g_Server[i].m_State == SS_GUILD)
			{
				if(i != aIndex)
					DataSend(i, (LPBYTE)aRecv, aRecv->h.size, __FUNCTION__);
			}
	}
}

void CExDataServerProtocol::GDReqGuildPeriodBuffInsert(int aIndex, PMSG_REQ_GUILD_PERIODBUFF_INSERT * aRecv)
{
	GUILD_INFO_STRUCT* lpGuild = this->GetGuild(aRecv->szGuildName);

	if ( !lpGuild )
	{
		return;
	}


}

void CExDataServerProtocol::GDReqGuildPeriodBuffDelete(int aIndex, PMSG_REQ_GUILD_PERIODBUFF_DELETE * aRecv)
{

}

void CExDataServerProtocol::GDGuildReqAssignStatus(int aIndex, EXSDHP_GUILD_ASSIGN_STATUS_REQ * aRecv)
{
	EXSDHP_GUILD_ASSIGN_STATUS_RESULT Result;
	ZeroMemory(&Result, sizeof(Result));
	PHeadSetB((LPBYTE)&Result, 0xE1, sizeof(Result));

	Result.btGuildStatus = aRecv->btGuildStatus;
	Result.btType = aRecv->btType;
	strncpy(Result.szGuildName, aRecv->szGuildName, 8);
	strncpy(Result.szTargetName, aRecv->szTargetName, 10);
	Result.wUserIndex = aRecv->wUserIndex;

	sLog.outBasic("[GuildMember Assign Status Request] GuildName [%s], Member [%s], Status [%d].",
		Result.szGuildName, Result.szTargetName, Result.btGuildStatus);

	Result.btFlag = 1;
	Result.btResult = 0;

	if(UpdateGuildMemberStatus(aRecv->szGuildName, aRecv->szTargetName, aRecv->btGuildStatus))
	{
		Result.btResult = 1;
	}

	sLog.outBasic("[GuildMember Assign Status Request] Send Result [%d].", Result.btResult);
	DataSend(aIndex, (LPBYTE)&Result, Result.h.size, __FUNCTION__);

	if(Result.btResult)
	{
		Result.btFlag = 0;
		for(int i=0; i < g_dwMaxServerGroups; i++)
		{
			if(g_Server[i].m_Index != -1
				&& g_Server[i].m_Type == ST_EXDATASERVER
				&& g_Server[i].m_State == SS_GUILD)
				{
					if(i != aIndex)
					{
						sLog.outBasic("[GuildMember Assign Status Request] Send Result [%d] to Server [%d].", Result.btResult, g_Server[i].m_ServerCode);
						DataSend(i, (LPBYTE)&Result, Result.h.size, __FUNCTION__);
					}
				}
		}	
	}
}

void CExDataServerProtocol::GDGuildReqAssignType(int aIndex, EXSDHP_GUILD_ASSIGN_TYPE_REQ * aRecv)
{
	EXSDHP_GUILD_ASSIGN_TYPE_RESULT Result;
	std::ZeroMemory(&Result, sizeof(Result));
	PHeadSetB((LPBYTE)&Result, 0xE2, sizeof(Result));

	Result.btGuildType = aRecv->btGuildType;
	strncpy(Result.szGuildName, aRecv->szGuildName, 8);
	Result.wUserIndex = aRecv->wUserIndex;

	sLog.outBasic("[Guild Assign Type Request] GuildName [%s], Type [%d].", Result.szGuildName, Result.btGuildType);

	Result.btFlag = 1;
	Result.btResult = 0;

	if(UpdateGuildType(aRecv->szGuildName, aRecv->btGuildType))
	{
		Result.btResult = 1;
	}

	sLog.outBasic("[Guild Assign Type Request] Send Result [%d].", Result.btResult);
	DataSend(aIndex, (LPBYTE)&Result, Result.h.size, __FUNCTION__);

	if(Result.btResult)
	{
		Result.btFlag = 0;
		for(int i=0; i < g_dwMaxServerGroups; i++)
		{
			if(g_Server[i].m_Index != -1
				&& g_Server[i].m_Type == ST_EXDATASERVER
				&& g_Server[i].m_State == SS_GUILD)
				{
					if(i != aIndex)
					{
						sLog.outBasic("[Guild Assign Type Request] Send Result [%d] to Server [%d].", Result.btResult, g_Server[i].m_ServerCode);
						DataSend(i, (LPBYTE)&Result, Result.h.size, __FUNCTION__);
					}
				}
		}	
	}

}

BOOL CExDataServerProtocol::DBGuildMemberExists(char *szName)
{
	BOOL res = FALSE;
	return res;
}

BOOL CExDataServerProtocol::AddDBGuildMember(char *szGuild, char *szName)
{
	BOOL res = FALSE;
	return res;
}

void CExDataServerProtocol::GDGuildMemberAdd(int aIndex, SDHP_GUILDMEMBERADD * aRecv)
{
	SDHP_GUILDMEMBERADD_RESULT Result;
	ZeroMemory(&Result, sizeof(Result));

	PHeadSetB((LPBYTE)&Result, 0x32, sizeof(Result));

	Result.NumberH = aRecv->NumberH;
	Result.NumberL = aRecv->NumberL;
	strncpy(Result.GuildName, aRecv->GuildName, 8);
	strncpy(Result.MemberID, aRecv->MemberID, 10);

	sLog.outBasic("[Guild Join Request] GuildName [%s], Member [%s].", Result.GuildName, Result.MemberID);

	Result.pServer = g_Server[aIndex].m_ServerCode;
	Result.Flag = 1;

	if(strlen(Result.GuildName) < 4)
	{
		Result.Result = 0;
		sLog.outBasic( "error-L2: [%s] Guild Name is short", Result.GuildName);

		sLog.outBasic("[Guild Join Request] Send Result [%d].", Result.Result);
		DataSend(aIndex, (LPBYTE)&Result, Result.h.size, __FUNCTION__);
		return;
	}

	if(strlen(Result.MemberID) < 4)
	{
		Result.Result = 0;
		sLog.outBasic( "error-L2: [%s] Member Name is short", Result.MemberID);

		sLog.outBasic("[Guild Join Request] Send Result [%d].", Result.Result);
		DataSend(aIndex, (LPBYTE)&Result, Result.h.size, __FUNCTION__);
		return;
	}

	if(DBGuildMemberExists(Result.MemberID))
	{
		Result.Result = 4;
		sLog.outBasic("[Guild Join Request] [DBGuildMemberExists] Member [%s] is already in guild", Result.MemberID);

		sLog.outBasic("[Guild Join Request] Send Result [%d].", Result.Result);
		DataSend(aIndex, (LPBYTE)&Result, Result.h.size, __FUNCTION__);
		return;
	}

	if(AddDBGuildMember(Result.GuildName, Result.MemberID) == FALSE)
	{
		Result.Result = 4;
		sLog.outBasic( "[Guild Join Request] [AddDBGuildMember] failed to add [%s] in guild [%s]", Result.MemberID, Result.GuildName);

		sLog.outBasic("[Guild Join Request] Send Result [%d].", Result.Result);
		DataSend(aIndex, (LPBYTE)&Result, Result.h.size, __FUNCTION__);
		return;
	}
	else
		Result.Result = 1;

	if(AddGuildMember(Result.GuildName, Result.MemberID, 0, g_Server[aIndex].m_ServerCode) == FALSE)
	{
		sLog.outBasic("error-L3: Failed to add member [%s] to guild [%s]", Result.MemberID, Result.GuildName);
	}

	for(int i=0; i < g_dwMaxServerGroups; i++)
	{
		if (g_Server[i].m_Index != -1
			&& g_Server[i].m_Type == ST_EXDATASERVER
			&& g_Server[i].m_State == SS_GUILD)
		{
			if (i == aIndex)
				Result.Flag = 1;
			else
				Result.Flag = 0;

			sLog.outBasic("[Guild Join Request] Send Result [%d] to Server [%d]", Result.Result, g_Server[i].m_ServerCode);

			DataSend(i, (LPBYTE)&Result, Result.h.size, __FUNCTION__);

		}
	}

	this->UpdateGuildMatchingMemberCount(Result.GuildName, this->GetGuildMemberCount(Result.GuildName));
	SendGuildMemberInfo(aIndex, Result.MemberID, MAKE_NUMBERW(Result.NumberH, Result.NumberL));
}

void CExDataServerProtocol::GDGuildMemberAddWithoutUserIndex(int aIndex, SDHP_GUILDMEMBERADD_WITHOUT_USERINDEX * aRecv)
{
	SDHP_GUILDMEMBERADD_RESULT_WITHOUT_USERINDEX Result;
	ZeroMemory(&Result, sizeof(Result));

	PHeadSetB((LPBYTE)&Result, 0x39, sizeof(Result));

	strncpy(Result.GuildName, aRecv->GuildName, 8);
	strncpy(Result.MemberID, aRecv->MemberID, 10);

	sLog.outBasic("[Guild Join Request] GuildName [%s], Member [%s].", Result.GuildName, Result.MemberID);

	Result.pServer = g_Server[aIndex].m_ServerCode;
	Result.Flag = 1;
	Result.Number = -1;

	if (strlen(Result.GuildName) < 4)
	{
		Result.Result = 0;
		sLog.outBasic( "error-L2: [%s] Guild Name is short", Result.GuildName);

		sLog.outBasic("[Guild Join Request] Send Result [%d].", Result.Result);
		DataSend(aIndex, (LPBYTE)&Result, Result.h.size, __FUNCTION__);
		return;
	}

	if (strlen(Result.MemberID) < 4)
	{
		Result.Result = 0;
		sLog.outBasic( "error-L2: [%s] Member Name is short", Result.MemberID);

		sLog.outBasic("[Guild Join Request] Send Result [%d].", Result.Result);
		DataSend(aIndex, (LPBYTE)&Result, Result.h.size, __FUNCTION__);
		return;
	}

	if (DBGuildMemberExists(Result.MemberID))
	{
		Result.Result = 4;
		sLog.outBasic("[Guild Join Request] [DBGuildMemberExists] Member [%s] is already in guild", Result.MemberID);

		sLog.outBasic("[Guild Join Request] Send Result [%d].", Result.Result);
		DataSend(aIndex, (LPBYTE)&Result, Result.h.size, __FUNCTION__);
		return;
	}

	if (AddDBGuildMember(Result.GuildName, Result.MemberID) == FALSE)
	{
		Result.Result = 4;
		sLog.outBasic( "[Guild Join Request] [AddDBGuildMember] failed to add [%s] in guild [%s]", Result.MemberID, Result.GuildName);

		sLog.outBasic("[Guild Join Request] Send Result [%d].", Result.Result);
		DataSend(aIndex, (LPBYTE)&Result, Result.h.size, __FUNCTION__);
		return;
	}
	else
		Result.Result = 1;

	if (AddGuildMember(Result.GuildName, Result.MemberID, 0, g_Server[aIndex].m_ServerCode) == FALSE)
	{
		sLog.outBasic("error-L3: Failed to add member [%s] to guild [%s]", Result.MemberID, Result.GuildName);
	}

	//EnterCriticalSection(&this->m_FriendSystemEDS.m_csMapFriendMaster);

	FRIEND_MASTER* lpUserData = this->m_FriendSystemEDS.GetFriendMaster(Result.MemberID);

	if (lpUserData)
	{
		Result.Number = lpUserData->m_iNumber;
	}

	//LeaveCriticalSection(&this->m_FriendSystemEDS.m_csMapFriendMaster);

	for (int i = 0; i < g_dwMaxServerGroups; i++)
	{
		if (g_Server[i].m_Index != -1
			&& g_Server[i].m_Type == ST_EXDATASERVER
			&& g_Server[i].m_State == SS_GUILD)
		{
			if (i == aIndex)
				Result.Flag = 1;
			else
				Result.Flag = 0;

			sLog.outBasic("[Guild Join Request] Send Result [%d] to Server [%d]", Result.Result, g_Server[i].m_ServerCode);

			DataSend(i, (LPBYTE)&Result, Result.h.size, __FUNCTION__);

		}
	}

	this->UpdateGuildMatchingMemberCount(Result.GuildName, this->GetGuildMemberCount(Result.GuildName));
}

BOOL CExDataServerProtocol::DelDBGuildMember(char *szName)
{
	return FALSE;
}

BOOL CExDataServerProtocol::DelGuildMember(char *szGuild, char *szName)
{
	GUILD_INFO_STRUCT* lpGuild;
	lpGuild = GetGuild(szGuild);
	if(lpGuild == NULL)
		return FALSE;

	MAP_GUILD_MEMBER::iterator it, end;
	it = lpGuild->m_mapGuildMember.find(szName);
	end = lpGuild->m_mapGuildMember.end();

	if(it == end)
		return FALSE;

	lpGuild->m_mapGuildMember.erase(it);

	return TRUE;
}

void CExDataServerProtocol::GDGuildMemberDel(int aIndex, SDHP_GUILDMEMBERDEL * aRecv)
{
	SDHP_GUILDMEMBERDEL_RESULT Result;
	ZeroMemory(&Result, sizeof(Result));

	PHeadSetB((LPBYTE)&Result, 0x33, sizeof(Result));

	Result.NumberH = aRecv->NumberH;
	Result.NumberL = aRecv->NumberL;
	strncpy(Result.GuildName, aRecv->GuildName, 8);
	strncpy(Result.MemberID, aRecv->MemberID, 10);

	sLog.outBasic("[Guild Member Withdraw Request] GuildName [%s], Member [%s].", Result.GuildName, Result.MemberID);

	Result.Flag = 1;
	Result.Result = 3;

	if(strlen(Result.GuildName) < 4)
	{
		sLog.outBasic( "error-L2: [%s] Guild Name is short", Result.GuildName);
		
		sLog.outBasic("[Guild Member Withdraw Request] Send Result [%d]", Result.Result);
		DataSend(aIndex, (LPBYTE)&Result, Result.h.size, __FUNCTION__);
		return;
	}

	if(strlen(Result.MemberID) < 4)
	{
		sLog.outBasic( "error-L2: [%s] Member Name is short", Result.MemberID);

		sLog.outBasic("[Guild Member Withdraw Request] Send Result [%d]", Result.Result);
		DataSend(aIndex, (LPBYTE)&Result, Result.h.size, __FUNCTION__);
		return;
	}

	if(DelDBGuildMember(Result.MemberID) == FALSE)
	{
		sLog.outBasic("[Guild Member Withdraw Request] [DelDBGuildMember] Failed delete member [%s] from guild [%s]", Result.MemberID, Result.GuildName);

		sLog.outBasic("[Guild Member Withdraw Request] Send Result [%d]", Result.Result);
		DataSend(aIndex, (LPBYTE)&Result, Result.h.size, __FUNCTION__);
		return;
	}

	if(DelGuildMember(Result.GuildName, Result.MemberID) == FALSE)
	{
		sLog.outBasic("[Guild Member Withdraw Request] [DelGuildMember] Guild[%s] Member[%s] delete fail.", Result.GuildName, Result.MemberID);
	}
	else
		Result.Result = 1;

	for(int i=0; i < g_dwMaxServerGroups; i++)
	{
		if(g_Server[i].m_Index != -1
			&& g_Server[i].m_Type == ST_EXDATASERVER
			&& g_Server[i].m_State == SS_GUILD)
			{
				if(i == aIndex)
					Result.Flag = 1;
				else
					Result.Flag = 0;

				sLog.outBasic("[Guild Member Withdraw Request] Send Result [%d] to Server[%d]", Result.Result, g_Server[i].m_ServerCode);

				DataSend(i, (LPBYTE)&Result, Result.h.size, __FUNCTION__);
			}
	}

	this->UpdateGuildMatchingMemberCount(Result.GuildName, this->GetGuildMemberCount(Result.GuildName));
}

BOOL CExDataServerProtocol::CreateRelationShipData(int iGuild)
{
	if(GetUnionData(iGuild))
		return FALSE;

	STR_UNION_MEMBER_DATA* ud = new STR_UNION_MEMBER_DATA();
	ud->m_vecUnionMember.push_back(iGuild);

	this->m_MapUnionManager.emplace(iGuild, ud);

	sLog.outBasic("[CreateRelationShipData] Guild:[%d]", iGuild);

	return TRUE;
}

BOOL DestroyRelationShipData(int iGuild)
{
	return TRUE;
}

void CExDataServerProtocol::DGRelationShipAnsJoin(int aIndex, EXSDHP_RELATIONSHIP_JOIN_REQ * aRecv)
{
	EXSDHP_RELATIONSHIP_JOIN_RESULT Result;

	PHeadSetB((LPBYTE)&Result, 0xE5, sizeof(Result));

	sLog.outBasic("[RelationShip Join Request] RelationShipType [%d], RequestGuild [%d], TargetGuild [%d].",
		aRecv->btRelationShipType, aRecv->iRequestGuildNum, aRecv->iTargetGuildNum);

	Result.btRelationShipType = aRecv->btRelationShipType;
	Result.iRequestGuildNum = aRecv->iRequestGuildNum;
	Result.iTargetGuildNum = aRecv->iTargetGuildNum;
	Result.wRequestUserIndex = aRecv->wRequestUserIndex;
	Result.wTargetUserIndex = aRecv->wTargetUserIndex;
	Result.btFlag = 1;
	GUILD_INFO_STRUCT* lpRG;
	GUILD_INFO_STRUCT* lpTG;

	lpRG = GetGuild(aRecv->iRequestGuildNum);
	lpTG = GetGuild(aRecv->iTargetGuildNum);

	if(lpRG && lpTG)
	{
		strncpy(Result.szRequestGuildName, lpRG->m_szGuildName, 9);
		strncpy(Result.szTargetGuildName, lpTG->m_szGuildName, 9);
		Result.btResult = RelationShipOperation(aRecv->iRequestGuildNum, aRecv->iTargetGuildNum, aRecv->btRelationShipType, 0);
	}
	else
	{
		if(lpRG == NULL)
			sLog.outBasic("[RelationShip Join Request] RequestGuild with number [%d] not found.", aRecv->iRequestGuildNum);

		if(lpTG == NULL)
			sLog.outBasic("[RelationShip Join Request] TargetGuild with number [%d] not found.", aRecv->iTargetGuildNum);

	}

	sLog.outBasic("[RelationShip Join Request] Send Result [%d]", Result.btResult);
	DataSend(aIndex, (LPBYTE)&Result, Result.h.size, __FUNCTION__);

	if(Result.btResult != 0 && Result.btResult != 0x10)
	{
		Result.btFlag = 0;
		for(int i=0; i < g_dwMaxServerGroups; i++)
		{
			if(g_Server[i].m_Index != -1
				&& g_Server[i].m_Type == ST_EXDATASERVER
				&& g_Server[i].m_State == SS_GUILD)
				{
					if(i != aIndex)
					{
						sLog.outBasic("[RelationShip Join Request] Send Result [%d] to Server [%d]", 
							Result.btResult, g_Server[i].m_ServerCode);

						DataSend(i, (LPBYTE)&Result, Result.h.size, __FUNCTION__);
					}
				}
		}
	}
}

void CExDataServerProtocol::DGRelationShipAnsBreakOff(int aIndex, EXSDHP_RELATIONSHIP_BREAKOFF_REQ * aRecv)
{
	EXSDHP_RELATIONSHIP_BREAKOFF_RESULT Result;
	ZeroMemory(&Result, sizeof(Result));

	PHeadSetB((LPBYTE)&Result, 0xE6, sizeof(Result));

	sLog.outBasic("[RelationShip Break Request] RelationShipType [%d], RequestGuild [%d], TargetGuild [%d].", 
		aRecv->btRelationShipType, aRecv->iRequestGuildNum, aRecv->iTargetGuildNum);

	Result.btRelationShipType = aRecv->btRelationShipType;
	Result.iRequestGuildNum = aRecv->iRequestGuildNum;
	Result.iTargetGuildNum = aRecv->iTargetGuildNum;
	Result.wRequestUserIndex = aRecv->wRequestUserIndex;
	Result.wTargetUserIndex = aRecv->wTargetUserIndex;
	Result.btFlag = 1;

	GUILD_INFO_STRUCT* lpRG;
	GUILD_INFO_STRUCT* lpTG;

	lpRG = GetGuild(aRecv->iRequestGuildNum);
	lpTG = GetGuild(aRecv->iTargetGuildNum);

	if(lpRG && lpTG)
	{
		Result.btResult = RelationShipOperation(aRecv->iRequestGuildNum, aRecv->iTargetGuildNum, aRecv->btRelationShipType, 1);
	}
	else
	{
		if(lpRG == NULL)
			sLog.outBasic("[RelationShip Break Request] RequestGuild with number [%d] not found.", aRecv->iRequestGuildNum);

		if(lpTG == NULL)
			sLog.outBasic("[RelationShip Break Request] TargetGuild with number [%d] not found.", aRecv->iTargetGuildNum);

	}

	sLog.outBasic("[RelationShip Break Request] Send Result [%d]", Result.btResult);
	DataSend(aIndex, (LPBYTE)&Result, Result.h.size, __FUNCTION__);

	if(Result.btResult != 0 && Result.btResult != 0x10)
	{
		Result.btFlag = 0;
		for(int i=0; i < g_dwMaxServerGroups; i++)
		{
			if(g_Server[i].m_Index != -1
				&& g_Server[i].m_Type == ST_EXDATASERVER
				&& g_Server[i].m_State == SS_GUILD)
				{
					if(i != aIndex)
					{
						sLog.outBasic("[RelationShip Break Request] Send Result [%d] to Server [%d]", 
							Result.btResult, g_Server[i].m_ServerCode);

						DataSend(i, (LPBYTE)&Result, Result.h.size, __FUNCTION__);
					}
				}
		}
	}

}

void CExDataServerProtocol::DGUnionListRecv(int aIndex, EXSDHP_UNION_LIST_REQ * aRecv)
{
	EXSDHP_UNION_LIST *lpList;
	EXSDHP_UNION_LIST_COUNT *lpListCnt;

	STR_UNION_MEMBER_DATA* lpUD = NULL;
	int cnt = 0, size, res = 0;

	sLog.outBasic("[Union List Request] UnionMasterGuild Number: [%d]", aRecv->iUnionMasterGuildNumber);

	GUILD_INFO_STRUCT* lpGuild;
	GUILD_INFO_STRUCT* lpG;

	if(aRecv->iUnionMasterGuildNumber != 0)
	{
		lpGuild = GetGuild(aRecv->iUnionMasterGuildNumber);
		if(lpGuild)
			sLog.outBasic("[Union List Request] UnionMasterGuild Name: [%s]", lpGuild->m_szGuildName);
		else
			sLog.outBasic("[Union List Request] Guild with number [%d] not found.", aRecv->iUnionMasterGuildNumber);
		lpUD = GetUnionData(aRecv->iUnionMasterGuildNumber);
		cnt = (int)lpUD->m_vecUnionMember.size();
	}
	
	size = sizeof(*lpListCnt)+sizeof(*lpList)*cnt;
	LPBYTE lpData = new BYTE[size];
	ZeroMemory(lpData, size);

	lpListCnt = (EXSDHP_UNION_LIST_COUNT *)lpData;
	lpList = (EXSDHP_UNION_LIST *)(lpData+sizeof(*lpListCnt));
	
	PHeadSetW((LPBYTE)lpListCnt, 0xE9, size);

	lpListCnt->iTimeStamp = GetTickCount();
	lpListCnt->wRequestUserIndex = aRecv->wRequestUserIndex;

	if(lpGuild && cnt != 0)
	{
		STR_UNION_MEMBER_DATA* lpUD;
		if(lpGuild->m_iUnionGuild)
			lpUD = GetUnionData(lpGuild->m_iUnionGuild);
		else
			lpUD = GetUnionData(lpGuild->m_iNumber);
		if(lpUD)
		{
			int s1 = (int)lpUD->m_vecUnionMember.size();
			lpListCnt->btUnionMemberNum = s1;

			for(int j=0; j < s1; j++)
			{
				lpG = GetGuild(lpUD->m_vecUnionMember[j]);
				if(lpG == NULL)
					continue;

				if(lpG->m_iRivalGuild == 0)
					continue;

				lpG = GetGuild(lpG->m_iRivalGuild);
				if(lpG == NULL)
					continue;

				STR_UNION_MEMBER_DATA* lpUD2;
				if(lpG->m_iUnionGuild)
					lpUD2 = GetUnionData(lpG->m_iUnionGuild);
				else
					lpUD2 = GetUnionData(lpG->m_iNumber);

				if(lpUD2 == NULL)
					continue;

				lpListCnt->btRivalMemberNum += (unsigned char)lpUD2->m_vecUnionMember.size();
			}
		}

		
		lpListCnt->btCount = cnt;
		for(int i=0; i < cnt; i++)
		{
			lpG = GetGuild(lpUD->m_vecUnionMember[i]);
			if(lpG)
			{
				lpList[i].btMemberNum = (unsigned char)lpG->m_mapGuildMember.size();
				std::memcpy(lpList[i].Mark, lpG->m_Mark, 32);
				strncpy(lpList[i].szGuildName, lpG->m_szGuildName, 8);
				sLog.outBasic("[Union List Request] Send GuildName: [%s].", lpG->m_szGuildName);
			}
		}

		lpListCnt->btResult = 1;
	}

	sLog.outBasic("[Union List Request] Send UnionList with result: [%d]", lpListCnt->btResult);

	DataSend(aIndex, lpData, size, __FUNCTION__);

	delete [] lpData;
}

int CExDataServerProtocol::RelationShipOperation(int iReqGuild, int iTargGuild, int relation_type, int operation)
{
	GUILD_INFO_STRUCT* lpReqGuild;
	GUILD_INFO_STRUCT* lpTargGuild;

	lpReqGuild = GetGuild(iReqGuild);
	lpTargGuild = GetGuild(iTargGuild);

	if(lpReqGuild == NULL || lpTargGuild == NULL)
		return 0;

	if(IsCSGuild(lpReqGuild->m_szGuildName))
		return 0x10;

	if(IsCSGuild(lpTargGuild->m_szGuildName))
		return 0x10;

	switch(operation)
	{
		case 0:
			return RelationShipJoin(lpReqGuild, lpTargGuild, relation_type);

		case 1:
			return RelationShipBreak(lpTargGuild, lpReqGuild, relation_type);

		default:
			return 0;
	}
}

int CExDataServerProtocol::RelationShipJoin(GUILD_INFO_STRUCT* lpReqGuild, GUILD_INFO_STRUCT* lpTargGuild, int type)
{
	switch(type)
	{
		case 1:
			return UnionJoin(lpReqGuild, lpTargGuild);

		case 2:
			return RivalJoin(lpReqGuild, lpTargGuild);

		default:
			return 0;
	}
}

int CExDataServerProtocol::UnionJoin(GUILD_INFO_STRUCT* lpReqGuild,GUILD_INFO_STRUCT* lpTargGuild)
{
	if(AddUnion(lpReqGuild, lpTargGuild) == FALSE)
		return 0;

	if(UpdateDBUnion(lpReqGuild->m_iNumber, lpTargGuild->m_iNumber) == FALSE)
		return 0;

	if(UpdateDBUnion(lpTargGuild->m_iNumber, lpTargGuild->m_iNumber) == FALSE)
		return 0;
	
	DGRelationShipListSend(-1, lpTargGuild->m_iNumber, 1, 1);
	DGRelationShipListSend(-1, lpTargGuild->m_iNumber, 2, 1);
	DGRelationShipNotificationSend(lpTargGuild->m_iNumber, 1);

	SendListToAllRivals(lpTargGuild);

	return 1;
}

int CExDataServerProtocol::RivalJoin(GUILD_INFO_STRUCT* lpReqGuild,GUILD_INFO_STRUCT* lpTargGuild)
{
	if(lpReqGuild->m_iRivalGuild != 0 || lpTargGuild->m_iRivalGuild != 0)
		return 0;

	if(UpdateDBRival(lpReqGuild->m_iNumber, lpTargGuild->m_iNumber) == FALSE)
		return 0;

	if(UpdateDBRival(lpTargGuild->m_iNumber, lpReqGuild->m_iNumber) == FALSE)
		return 0;

	lpReqGuild->m_iRivalGuild = lpTargGuild->m_iNumber;
	lpTargGuild->m_iRivalGuild = lpReqGuild->m_iNumber;

	DGRelationShipListSend(-1, lpReqGuild->m_iNumber, 2, 1);
	DGRelationShipListSend(-1, lpTargGuild->m_iNumber, 2, 1);

	SendListToAllRivals(lpTargGuild);

	return 1;
}

int CExDataServerProtocol::RelationShipBreak(GUILD_INFO_STRUCT* lpMasterGuild, GUILD_INFO_STRUCT* lpKickGuild, int type)
{
	switch(type)
	{
		case 1:
			return UnionBreak(lpMasterGuild, lpKickGuild);

		case 2:
			return RivalBreak(lpMasterGuild, lpKickGuild);

		default:
			return 0;
	}
}

int CExDataServerProtocol::UnionBreak(GUILD_INFO_STRUCT* lpMasterGuild,GUILD_INFO_STRUCT* lpKickGuild)
{
	if(lpMasterGuild == NULL || lpKickGuild == NULL)
		return 0;

	if(KickUnion(lpMasterGuild, lpKickGuild) == FALSE)
		return 0;

	if(UpdateDBUnion(lpKickGuild->m_iNumber, 0) == FALSE)
		return 0;

	if(lpMasterGuild->m_iUnionGuild == 0)
	{
		if(UpdateDBUnion(lpMasterGuild->m_iNumber, 0) == FALSE)
			return 0;

		DGRelationShipNotificationSend(lpMasterGuild->m_iNumber, 0x10);
	}

	DGRelationShipListSend(-1, lpMasterGuild->m_iNumber, 1, 1);
	DGRelationShipListSend(-1, lpMasterGuild->m_iNumber, 2, 1);
	DGRelationShipNotificationSend(lpMasterGuild->m_iNumber, 1);

	DGRelationShipListSend(-1, lpKickGuild->m_iNumber, 1, 1);
	DGRelationShipListSend(-1, lpKickGuild->m_iNumber, 2, 1);
	DGRelationShipNotificationSend(lpKickGuild->m_iNumber, 1);

	SendListToAllRivals(lpMasterGuild);
	SendListToAllRivals(lpKickGuild);

	return 1;
}

int CExDataServerProtocol::RivalBreak(GUILD_INFO_STRUCT* lpMasterGuild,GUILD_INFO_STRUCT* lpKickGuild)
{
	if(lpMasterGuild == NULL || lpKickGuild == NULL)
		return 0;

	if(lpMasterGuild->m_iRivalGuild != lpKickGuild->m_iNumber ||
		lpKickGuild->m_iRivalGuild != lpMasterGuild->m_iNumber)
		return 0;

	if(UpdateDBRival(lpMasterGuild->m_iNumber, 0) == FALSE)
		return 0;

	if(UpdateDBRival(lpKickGuild->m_iNumber, 0) == FALSE)
		return 0;

	lpMasterGuild->m_iRivalGuild = 0;
	lpKickGuild->m_iRivalGuild = 0;

	DGRelationShipListSend(-1, lpMasterGuild->m_iNumber, 2, 1);
	DGRelationShipListSend(-1, lpKickGuild->m_iNumber, 2, 1);

	return 1;
}

BOOL CExDataServerProtocol::AddUnion(GUILD_INFO_STRUCT* lpReqGuild,GUILD_INFO_STRUCT* lpTargGuild)
{
	if(lpReqGuild == NULL || lpTargGuild == NULL)
		return FALSE;

	if( lpReqGuild->m_iUnionGuild != 0)
		return FALSE;

	if( lpTargGuild->m_iUnionGuild != 0 && lpTargGuild->m_iNumber != lpTargGuild->m_iUnionGuild)
		return FALSE;

	if(AddUnion(lpReqGuild->m_iNumber, lpTargGuild->m_iNumber))
	{
		lpReqGuild->m_iUnionGuild = lpTargGuild->m_iNumber;
		lpTargGuild->m_iUnionGuild = lpTargGuild->m_iNumber;
		return TRUE;
	}

	return FALSE;
}

BOOL CExDataServerProtocol::AddUnion(int iReqGuild, int iTargGuild)
{
	if(iReqGuild == iTargGuild)
		return FALSE;

	STR_UNION_MEMBER_DATA* lpUD = GetUnionData(iTargGuild);
	if(lpUD == NULL)
	{
		if(iTargGuild)
			sLog.outBasic("[AddUnion] FAILED - ReqGuild: [%d], TargGuild: [%d]", iReqGuild, iTargGuild);

		return FALSE;
	}

	lpUD->m_vecUnionMember.push_back(iReqGuild);

	sLog.outBasic("[AddUnion] SUCCESS - ReqGuild: [%d], TargGuild: [%d]", iReqGuild, iTargGuild);
	return TRUE;

}

BOOL CExDataServerProtocol::KickUnion(GUILD_INFO_STRUCT* lpMasterGuild,GUILD_INFO_STRUCT* lpKickGuild)
{
	if(lpMasterGuild == NULL || lpKickGuild == NULL)
		return FALSE;

	if(lpMasterGuild->m_iNumber == lpKickGuild->m_iNumber)
		return FALSE;

    STR_UNION_MEMBER_DATA* lpUD = GetUnionData(lpMasterGuild->m_iNumber);
	if(lpUD == NULL)
		return FALSE;
	
	int size = (int)lpUD->m_vecUnionMember.size();
	int i;
	for(i=0; i < size; i++)
	{
		if(lpUD->m_vecUnionMember[i] == lpKickGuild->m_iNumber)
			break;
	}

	if(i == size)
		return FALSE;

	lpUD->m_vecUnionMember.erase(lpUD->m_vecUnionMember.begin()+i);

	lpKickGuild->m_iUnionGuild = 0;
	if(lpUD->m_vecUnionMember.size() < 2)
		lpMasterGuild->m_iUnionGuild = 0;

	return TRUE;
}


BOOL CExDataServerProtocol::UpdateDBUnion(int iGuild, int iUnion)
{
	return FALSE;
}

BOOL CExDataServerProtocol::UpdateDBRival(int iGuild, int iRival)
{
	return FALSE;
}

void CExDataServerProtocol::DGRelationShipListSend(int aIndex, int iGuild, int relation_type, BOOL snd_all)
{
	EXSDHP_UNION_RELATIONSHIP_LIST List;
	ZeroMemory(&List, sizeof(List));

	sLog.outBasic("[RelationShip List Send] RelationShipType [%d], GuildNumber [%d].", relation_type, iGuild);

	PHeadSetW((LPBYTE)&List, 0xE7, sizeof(List));
	List.btRelationShipType = relation_type;
	GUILD_INFO_STRUCT* lpGuild;

	lpGuild = GetGuild(iGuild);
	if(lpGuild == NULL)
	{
		sLog.outBasic("[RelationShip List Send] error: Guild with number [%d] not found.", iGuild);
		return;
	}

	if(lpGuild->m_iUnionGuild != 0)
		iGuild = lpGuild->m_iUnionGuild;

	sLog.outBasic("[RelationShip List Send] UnionMasterGuild: [%d]", iGuild);

	lpGuild = GetGuild(iGuild);
	if(lpGuild == NULL)
	{
		sLog.outBasic("[RelationShip List Send] error: Guild with number [%d] not found.", iGuild);
		return;
	}

	List.iUnionMasterGuildNumber = lpGuild->m_iNumber;

	if(lpGuild->m_iUnionGuild)
		strncpy(List.szUnionMasterGuildName, lpGuild->m_szGuildName, 8);

	if(relation_type == 1)
	{
		STR_UNION_MEMBER_DATA* lpUD = GetUnionData(lpGuild->m_iNumber);
		if(lpUD == NULL)
		{
			sLog.outBasic("[RelationShip List Send] error: No union data for Guild [%s].", lpGuild->m_szGuildName);
			return;
		}

		int size = (int)lpUD->m_vecUnionMember.size();
		int i;
		for(i=0; i < size; i++)
			List.iRelationShipMember[i] = lpUD->m_vecUnionMember[i];	
		
		List.btRelationShipMemberCount = size;
		sLog.outBasic("[RelationShip List Send] RelationShipMemberCount: [%d] sent.", size);
	}
	else if(relation_type == 2)
	{

		STR_UNION_MEMBER_DATA* lpUD = GetUnionData(lpGuild->m_iNumber);
		if(lpUD == NULL)
		{
			sLog.outBasic("[RelationShip List Send] error: No union data for Guild [%s].", lpGuild->m_szGuildName);
			return;
		}

		int size = (int)lpUD->m_vecUnionMember.size();
		int i, j;
		GUILD_INFO_STRUCT* lpG = NULL;
		int cnt = 0;

		for(i=0; i < size; i++)
		{
			lpG = GetGuild(lpUD->m_vecUnionMember[i]);
			if(lpG == NULL)
				continue;

			if(lpG->m_iRivalGuild == 0)
				continue;

			// get rival guild
			lpG = GetGuild(lpG->m_iRivalGuild);

			if(lpG == NULL)
				continue;

			STR_UNION_MEMBER_DATA* lpUD2 = NULL;

			// get allies of rival guild
			if(lpG->m_iUnionGuild)
				lpG = GetGuild(lpG->m_iUnionGuild);
						
			if(lpG == NULL)
				continue;

			lpUD2 = GetUnionData(lpG->m_iNumber);
			if(lpUD2 == NULL)
				continue;

			// get all allies of rival guild
			int s2 = (int)lpUD2->m_vecUnionMember.size();
			for(j=0; j < s2; j++)
			{
				List.iRelationShipMember[cnt] = lpUD2->m_vecUnionMember[j];
				cnt++;
			}

		}

		List.btRelationShipMemberCount = cnt;

		sLog.outBasic("[RelationShip List Send] RelationShipMemberCount[%d] send.", cnt);

/*
		if(lpGuild->m_iRivalGuild)
		{
			STR_UNION_MEMBER_DATA* lpUD = GetUnionData(lpGuild->m_iRivalGuild);
			if(lpUD == NULL)
				return;

			int size = lpUD->m_vecUnionMember.size();
			int i;
			for(i=0; i < size; i++)
				List.iRelationShipMember[i] = lpUD->m_vecUnionMember[i];
			
			List.btRelationShipMemberCount = size;
		}
		else
		{
			List.btRelationShipMemberCount = 0;
		}
*/
	}

	if(snd_all)
	{
		for(int i=0; i < g_dwMaxServerGroups; i++)
		{
			if(g_Server[i].m_Index != -1
				&& g_Server[i].m_Type == ST_EXDATASERVER
				&& g_Server[i].m_State == SS_GUILD)
				{

					sLog.outBasic("[RelationShip List Send] List Send to Server [%d].", g_Server[i].m_ServerCode);
					DataSend(i, (LPBYTE)&List, sizeof(List), __FUNCTION__);
				}
		}
	}
	{
		if(aIndex != -1)
		{
			sLog.outBasic("[RelationShip List Send] List Send.");
			DataSend(aIndex, (LPBYTE)&List, sizeof(List), __FUNCTION__);
		}
		else
			sLog.outBasic("[RelationShip List Send] error: ServerIndex = -1.");
	}
}

void CExDataServerProtocol::DGRelationShipNotificationSend(int iGuild, int iUpdateFlag)
{
	GUILD_INFO_STRUCT* lpGuild;
	lpGuild = GetGuild(iGuild);

	sLog.outBasic("[RelationShip Notification Send] Guild [%d].", iGuild);

	if(lpGuild == NULL)
	{
		sLog.outBasic("[RelationShip Notification Send] error: Guild [%d] not found.", iGuild);
		return;
	}

	if(lpGuild->m_iUnionGuild != 0)
		iGuild = lpGuild->m_iUnionGuild;

	sLog.outBasic("[RelationShip Notification Send] UnionMasterGuild: [%d].", iGuild);

	STR_UNION_MEMBER_DATA* lpUD;

	lpUD = GetUnionData(iGuild);
	if(lpUD == NULL)
	{
		sLog.outBasic("[RelationShip Notification Send] error: No union data for Guild [%d].", iGuild);
		return;
	}

	EXSDHP_NOTIFICATION_RELATIONSHIP List;
	ZeroMemory(&List, sizeof(List));

	PHeadSetW((LPBYTE)&List, 0xE8, sizeof(List));
	List.btUpdateFlag = iUpdateFlag;
	int size = (int)lpUD->m_vecUnionMember.size();
	List.btGuildListCount = size;
	
	sLog.outBasic("[RelationShip Notification Send] Guild List Count: [%d].", size);

	int i;
	for(i=0; i < size; i++)
	{
		List.iGuildList[i] = lpUD->m_vecUnionMember[i];
	}

	for(i=0; i < g_dwMaxServerGroups; i++)
	{
		if(g_Server[i].m_Index != -1
			&& g_Server[i].m_Type == ST_EXDATASERVER
			&& g_Server[i].m_State == SS_GUILD)
			{

				sLog.outBasic("[RelationShip Notification Send] Send to Server [%d].", g_Server[i].m_ServerCode);
				DataSend(i, (LPBYTE)&List, sizeof(List), __FUNCTION__);
			}
	}
}

void CExDataServerProtocol::DGRelationShipAnsKickOutUnionMember(int aIndex, EXSDHP_KICKOUT_UNIONMEMBER_REQ *aRecv)
{
	EXSDHP_KICKOUT_UNIONMEMBER_RESULT Result;
	ZeroMemory(&Result, sizeof(Result));

	PHeadSubSetB((LPBYTE)&Result, 0xEB, 1, sizeof(Result));
	Result.btRelationShipType = aRecv->btRelationShipType;
	Result.btResult = 0;
	strncpy(Result.szUnionMasterGuildName, aRecv->szUnionMasterGuildName, 8);
	strncpy(Result.szUnionMemberGuildName, aRecv->szUnionMemberGuildName, 8);
	Result.wRequestUserIndex = aRecv->wRequestUserIndex;
	Result.btResult = 0;
	Result.btFlag = 1;

	sLog.outBasic("[Kick Union Member Request] UnionMasterGuild [%s], UnionMemberGuild [%s]", Result.
		szUnionMasterGuildName, Result.szUnionMemberGuildName);

	GUILD_INFO_STRUCT* lpMasterGuild;
	GUILD_INFO_STRUCT* lpKickGuild;
	lpMasterGuild = GetGuild(Result.szUnionMasterGuildName);
	lpKickGuild = GetGuild(Result.szUnionMemberGuildName);
	
	if(lpMasterGuild && lpKickGuild)
	{
		Result.btResult = RelationShipOperation(lpKickGuild->m_iNumber, lpMasterGuild->m_iNumber, aRecv->btRelationShipType, 1);
	}
	else
	{
		if(lpMasterGuild == NULL)
			sLog.outBasic("[Kick Union Member Request] error: UnionMasterGuild [%s] not found.", Result.szUnionMasterGuildName);

		if(lpKickGuild)
			sLog.outBasic("[Kick Union Member Request] error: UnionMemberGuild [%s] not found.", Result.szUnionMemberGuildName);
	}

	sLog.outBasic("[Kick Union Member Request] Send Result [%d].", Result.btResult);
	DataSend(aIndex, (LPBYTE)&Result, Result.h.size, __FUNCTION__);

	if(Result.btResult != 0 && Result.btResult != 0x10)
	{
		Result.btFlag = 0;
		for(int i=0; i < g_dwMaxServerGroups; i++)
		{
			if(g_Server[i].m_Index != -1
				&& g_Server[i].m_Type == ST_EXDATASERVER
				&& g_Server[i].m_State == SS_GUILD)
				{
					if(i != aIndex)
					{
						sLog.outBasic("[Kick Union Member Request] Send Result [%d] to Server [%d]", Result.btResult, g_Server[i].m_ServerCode);
						DataSend(i, (LPBYTE)&Result, Result.h.size, __FUNCTION__);
					}
				}
		}
	}
}

void CExDataServerProtocol::SendListToAllRivals(GUILD_INFO_STRUCT* lpGuild)
{
	if(lpGuild == NULL)
		return;

	if(lpGuild->m_iUnionGuild)
		lpGuild = GetGuild(lpGuild->m_iUnionGuild);

	if(lpGuild == NULL)
		return;

	STR_UNION_MEMBER_DATA* lpUD = GetUnionData(lpGuild->m_iNumber);
	if(lpUD == NULL)
		return;

	int size = (int)lpUD->m_vecUnionMember.size();
	for(int i=0; i < size; i++)
	{
		GUILD_INFO_STRUCT* lpG;
		lpG = GetGuild(lpUD->m_vecUnionMember[i]);
		if(lpG == NULL)
			continue;

		if(lpG->m_iRivalGuild)
		{
			DGRelationShipListSend(-1, lpG->m_iRivalGuild, 2, 1);
			DGRelationShipNotificationSend(lpG->m_iRivalGuild, 1);
		}
	}
}

int CExDataServerProtocol::GetChatServer()
{
	for(int i=0; i < g_dwMaxServerGroups; i++)
	{
		if(g_Server[i].m_Index != -1
			&& g_Server[i].m_Type == ST_EXDATASERVER
			&& g_Server[i].m_State == SS_CHAT)
			{
				return i;
			}
	}

	return -1;
}

void CExDataServerProtocol::FCHRoomCreateReq(int aIndex, char *szName, char *szFriendName, short Number, short ServerId, short FriendNumber, short FriendServerId)
{
	sLog.outBasic("[ChatServer] Room Create Request Name [%s], FriendName [%s].", szName, szFriendName);
	if(aIndex < 0)
	{
		sLog.outError("error-L3: ChatServer index: [%d]", aIndex);
		return;
	}

	FCHP_CHATROOM_CREATE_REQ Req;
	ZeroMemory(&Req, sizeof(Req));

	PHeadSetB((LPBYTE)&Req, 0xA0, sizeof(Req));

	strncpy(Req.Name, szName, 10);
	strncpy(Req.FriendName, szFriendName, 10);

	Req.Number = Number;
	Req.FriendNumber = FriendNumber;
	Req.ServerId = ServerId;
	Req.FriendServerId = FriendServerId;
	Req.Type = 1;

	DataSend(aIndex, (LPBYTE)&Req, Req.h.size, __FUNCTION__);
}

void CExDataServerProtocol::FriendChatRoomCreateAns(int aIndex, FCHP_CHATROOM_CREATE_RESULT* lpMsg)
{
	FHP_FRIEND_CHATROOM_CREATE_RESULT Result;
	ZeroMemory(&Result, sizeof(Result));
	
	PHeadSetB((LPBYTE)&Result, 0x66, sizeof(Result));

	Result.Result = lpMsg->Result;
	Result.Number = lpMsg->Number;
	Result.RoomNumber = lpMsg->RoomNumber;
	Result.Ticket = lpMsg->Ticket;
	Result.Type = lpMsg->Type;
	strncpy(Result.Name, lpMsg->Name, 10);
	strncpy(Result.FriendName, lpMsg->FriendName, 10);

	if (aIndex >= 0 && g_Server[aIndex].m_Type == ST_EXDATASERVER && g_Server[aIndex].m_State == SS_CHAT) // fixed
	{
		strncpy(Result.ServerIp, szWANIP, sizeof(Result.ServerIp));
	}
	
	char szName[11] = {0};
	char szFriend[11] = {0};
	strncpy(szName, Result.Name, 10);
	strncpy(szFriend, Result.FriendName, 10);

	if(lpMsg->Result)
	{
		sLog.outBasic("[ChatServer] Room Create Result [%d], Name [%s], Friend [%s], Room [%d], Ticket[%d]", 
			lpMsg->Result, szName, szFriend, lpMsg->RoomNumber, lpMsg->Ticket);
	}
	else
	{
		sLog.outBasic("[ChatServer] Room Create Result [%d], Name [%s], Friend [%s].", 
			lpMsg->Result, szName, szFriend);

	}

	DataSend(lpMsg->ServerId, (LPBYTE)&Result, Result.h.size, __FUNCTION__);
}

void CExDataServerProtocol::FriendChatRoomCreateReq(int aIndex, FHP_FRIEND_CHATROOM_CREATE_REQ* lpMsg)
{
	FCHP_CHATROOM_CREATE_RESULT Result;
	ZeroMemory(&Result, sizeof(Result));

	strncpy(Result.Name, lpMsg->Name, 10);
	strncpy(Result.FriendName, lpMsg->fName, 10);
	Result.Number = lpMsg->Number;
	Result.RoomNumber = -1;
	Result.ServerId = aIndex;
	Result.Ticket = -1;
	Result.Type = 1;

	char szMaster[11] = {0};
	char szFriend[11] = {0};
	strncpy(szMaster, lpMsg->Name, 10);
	strncpy(szFriend, lpMsg->fName, 10);

	sLog.outBasic("[ChatRoom Create Request] Name [%s], FriendName [%s].", szMaster, szFriend);
	int ChatId = GetChatServer();

	if(ChatId < 0)
	{
		Result.Result = 2;
		sLog.outBasic( "error-L1: ChatServer Not Found.");
		FriendChatRoomCreateAns(-1, &Result);
		return;
	}

	
	Result.Result = -1;


	if(this->m_FriendSystemEDS.FriendExists(szMaster, szFriend) == FALSE)
	{
		sLog.outBasic("[ChatRoom Create Request] Friend [%s] not found in [%s]'s friend list.", szFriend, szMaster);
		FriendChatRoomCreateAns(-1, &Result);
		return;
	}

	int FriendServerId = 0; // stub
	if(FriendServerId < 0)
	{
		FriendChatRoomCreateAns(-1, &Result);
		return;
	}

	FCHRoomCreateReq(GetChatServer(), szMaster, szFriend, 
		lpMsg->Number, aIndex, this->m_FriendSystemEDS.GetFriendNumber(szFriend), FriendServerId);

}

void CExDataServerProtocol::FCHChatRoomInvitationReq(int aIndex, short RoomNumber,	char *szName, short Number,	short ServerId,	BYTE Type)
{
	sLog.outBasic("[ChatServer] Room: [%d] Invitation Request - FriendName: [%s].", RoomNumber, szName);

	if(aIndex < 0)
	{
		sLog.outError("error-L3: ChatServer index: [%d]", aIndex);
		return;
	}

	FCHP_CHATROOM_INVITATION_REQ Req;
	ZeroMemory(&Req, sizeof(Req));

	PHeadSetB((LPBYTE)&Req, 0xA1, sizeof(Req));

	Req.Number = Number;
	Req.RoomNumber = RoomNumber;
	Req.ServerId = ServerId;
	Req.Type = Type;
	strncpy(Req.Name, szName, 10);

	DataSend(aIndex, (LPBYTE)&Req, Req.h.size, __FUNCTION__);
}

void CExDataServerProtocol::FriendChatRoomInvitationReq(int aIndex, FHP_FRIEND_INVITATION_REQ * lpMsg)
{
	FHP_FRIEND_INVITATION_RET Result;
	std::ZeroMemory(&Result, sizeof(Result));

	PHeadSetB((LPBYTE)&Result, 0x74, sizeof(Result));
	Result.Number = lpMsg->Number;
	Result.WindowGuid = lpMsg->WindowGuid;
	Result.Result = 0;
	strncpy(Result.Name, lpMsg->Name, 10);

	char szMaster[11] = {0};
	char szFriend[11] = {0};

	strncpy(szMaster, lpMsg->Name, 10);
	strncpy(szFriend, lpMsg->FriendName, 10);

	sLog.outBasic("[ChatRoom Invitation Request] Name[%s] Room[%d] FriendName[%s].", szMaster, lpMsg->RoomNumber, szFriend);
	if(this->m_FriendSystemEDS.FriendExists(szMaster, szFriend))
	{
		int FriendServerId; // stub
		if(FriendServerId > -1)
		{
			FCHChatRoomInvitationReq(GetChatServer(), lpMsg->RoomNumber, 
				szFriend, this->m_FriendSystemEDS.GetFriendNumber(szFriend), FriendServerId, 1);
			Result.Result = 1;
		}
	}
	
	sLog.outBasic("[ChatRoom Invitation Request] Send Result [%d].", Result.Result);
	DataSend(aIndex, (LPBYTE)&Result, Result.h.size, __FUNCTION__);
}

void CExDataServerProtocol::GensManualRefreshRanking(BYTE Type)
{
	if (g_UseExDataServer == TRUE && !g_DSMode)
	{
		this->m_GensSystemEDS.ManualRefreshRanking(Type);
	}
	else
	{
		sLog.outError("Option available only with ExDataServer enabled");
		return;
	}
}

void CExDataServerProtocol::GensRankingProcess()
{
	if (g_UseExDataServer == TRUE && !g_DSMode)
	{
		this->m_GensSystemEDS.GensRankingProcess();
	}
	else
	{
		return;
	}
}

// Gens System Class

GensSystem_EDS::GensSystem_EDS() //: m_RankingLog("Gens_Ranking", ".\\GENS_RANKING_LOG", 256)
{
	InitializeCriticalSection(&this->m_GensCriticalSection);
	memset(this->m_GensRankingData, 0x00, sizeof(this->m_GensRankingData));

	this->m_mapGensRankDuprian.clear();
	this->m_mapGensRankVanert.clear();




	this->m_DuprianUserCount = 0;
	this->m_VanertUserCount = 0;
	this->m_LeaveDateDayDiff = 0;
	
	this->m_GensRankingUpdateTimeTick = GetTickCount();
}

GensSystem_EDS::~GensSystem_EDS()
{
	DeleteCriticalSection(&this->m_GensCriticalSection);



};

BOOL GensSystem_EDS::InitGensSystem()
{
	if (!g_UseExDataServer || g_DSMode == TRUE)
	{
		return FALSE;
	}

	sLog.outBasic("[Gens System] Initializing Gens...");

	g_GensRankingUpdateTime = GetPrivateProfileInt("GensSystem", "GensRankingUpdateTimeHour", 2, ".\\DataServer.ini");
	GetPrivateProfileString("GensSystem", "GensRankingPath", "..\\Data\\Character\\GensSystem.ini", g_GensRankingPath, sizeof(g_GensRankingPath), ".\\DataServer.ini");

	this->LoadGensData(g_GensRankingPath);

	sLog.outBasic("[Gens System] Current Month: %d", this->m_RewardMonth);

	this->MakeRanking();

	return TRUE;
}

BOOL GensSystem_EDS::LoadGensData(LPSTR lpszFileName)
{
	if (lpszFileName == NULL)
	{
		sLog.outBasic("[ GensRanking ] - File load error : File Name Error");
		return FALSE;
	}

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(lpszFileName);

	if (res.status != pugi::status_ok)
	{
		sLog.outBasic("[ GensRanking ] - Can't Load %s (%s)", lpszFileName, res.description());
		return FALSE;
	}

	pugi::xml_node mainXML = file.child("GensSystem");
	pugi::xml_node reward = mainXML.child("RewardSettings");

	this->m_GensRewardStartDay = reward.attribute("StartDay").as_int();
	this->m_GensRewardEndDay = reward.attribute("EndDay").as_int();
	this->m_MinRankToGainReward = reward.attribute("MinRankToGainReward").as_int(300);

	pugi::xml_node ranking = mainXML.child("RankingSettings");

	for (pugi::xml_node gens = ranking.child("Gens"); gens; gens = gens.next_sibling())
	{
		int iClass = gens.attribute("Class").as_int();

		if (iClass < 1 || iClass > MAX_GENS_CLASS)
		{
			sLog.outBasic("Wrong Gens Class in %s file (%d)", lpszFileName, iClass);
			continue;
		}

		this->m_GensRankingData[iClass - 1].GensClass = iClass;
		this->m_GensRankingData[iClass - 1].ContributePoint = gens.attribute("ReqContributePoint").as_int();
		this->m_GensRankingData[iClass - 1].RankingPosition = gens.attribute("RankingPosition").as_int();
		this->m_GensRankingData[iClass - 1].RewardItemGroup = gens.attribute("RewardItemCat").as_int();
		this->m_GensRankingData[iClass - 1].RewardItemNumber = gens.attribute("RewardItemIndex").as_int();
		this->m_GensRankingData[iClass - 1].RewardItemCount = gens.attribute("RewardItemCount").as_int();
	}

	return TRUE;
}

int GensSystem_EDS::CalcGensClass(int ContributePoint, int Ranking)
{
	if (ContributePoint < this->m_GensRankingData[8].ContributePoint)
	{
		if (ContributePoint < this->m_GensRankingData[9].ContributePoint)
		{
			if (ContributePoint < this->m_GensRankingData[10].ContributePoint)
			{
				if (ContributePoint < this->m_GensRankingData[11].ContributePoint)
				{
					if (ContributePoint < this->m_GensRankingData[12].ContributePoint)
					{
						if (ContributePoint < this->m_GensRankingData[13].ContributePoint)
						{
							return 0;
						}
						else 
						{
							return 14;
						}
					}
					else
					{
						return 13;
					}
				}
				else 
				{
					return 12;
				}
			}
			else
			{
				return 11;
			}
		}
		else
		{
			return 10;
		}
	}
	else
	{
		if (Ranking > this->m_GensRankingData[0].RankingPosition)
		{
			if (Ranking > this->m_GensRankingData[1].RankingPosition)
			{
				if (Ranking > this->m_GensRankingData[2].RankingPosition)
				{
					if (Ranking > this->m_GensRankingData[3].RankingPosition)
					{
						if (Ranking > this->m_GensRankingData[4].RankingPosition)
						{
							if (Ranking > this->m_GensRankingData[5].RankingPosition)
							{
								if (Ranking > this->m_GensRankingData[6].RankingPosition)
								{
									if (Ranking > this->m_GensRankingData[7].RankingPosition)
									{
										return 9;
									}
									else
									{
										return 8;
									}
								}
								else
								{
									return 7;
								}
							}
							else 
							{
								return 6;
							}
						}
						else 
						{
							return 5;
						}
					}
					else
					{
						return 4;
					}
				}
				else
				{
					return 3;
				}
			}
			else 
			{
				return 2;
			}
		}
		else
		{
			return 1;
		}
	}
	return 0;
}

int GensSystem_EDS::GetGensInfluence(char* szUserName)
{
	EnterCriticalSection(&this->m_GensCriticalSection);

	LeaveCriticalSection(&this->m_GensCriticalSection);
	return 0;
}

void GensSystem_EDS::GensRankingProcess()
{
	if(GetTickCount() - this->m_GensRankingUpdateTimeTick < g_GensRankingUpdateTime*1000*60*60)
	{
		return;
	}

	this->MakeRanking();

	sLog.outBasic("[Gens System] Processing Ranking Refresh (by Timer)");
	this->m_GensRankingUpdateTimeTick = GetTickCount();
}

void GensSystem_EDS::GDReqGensInfo(int aIndex, PMSG_REQ_ABUSING_INFO *aRecv)
{
	EnterCriticalSection(&this->m_GensCriticalSection);

	PMSG_ANS_GENS_INFO_EXDB pMsg;
	
	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x02, sizeof(pMsg));

	pMsg.wIndexH = aRecv->bIndexH;
	pMsg.wIndexL = aRecv->bIndexL;

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);

	LeaveCriticalSection(&this->m_GensCriticalSection);
}

void GensSystem_EDS::GDReqRegGensMember(int aIndex, PMSG_REQ_REG_GENS_MEMBER_EXDB *aRecv)
{
	EnterCriticalSection(&this->m_GensCriticalSection);

	PMSG_ANS_REG_GENS_MEMBER_EXDB pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x04, sizeof(pMsg));

	pMsg.bIndexH = aRecv->bIndexH;
	pMsg.bIndexL = aRecv->bIndexL;
	pMsg.bInfluence = aRecv->bInfluence;

	sLog.outBasic("[Gens System] Request to Register: Name:%s Influence:%d -> RESULT: %d", aRecv->Name, aRecv->bInfluence, pMsg.bResult);

	if(pMsg.bResult == 0)
	{
		GENS_USER pUser;

		pUser.iContributePoint = 0;
		pUser.iGensClass = 14;
		pUser.iRank = 0;
		memcpy(pUser.Name, aRecv->Name, 10);
		pUser.Name[10] = 0;

		if(pMsg.bInfluence == 1)
		{
			this->m_mapGensRankDuprian[aRecv->Name] = pUser;
			sLog.outBasic( "[Gens System] Add New User (DUPRIAN) (NAME:%s)", pUser.Name);
		}
		
		else if(pMsg.bInfluence == 2)
		{
			this->m_mapGensRankVanert[aRecv->Name] = pUser;
			sLog.outBasic("[Gens System] Add New User (VANERT) (NAME:%s)", pUser.Name);
		}
	}

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);

	LeaveCriticalSection(&this->m_GensCriticalSection);
}
	
void GensSystem_EDS::GDReqSecedeGensMember(int aIndex, PMSG_REQ_SECEDE_GENS_MEMBER_EXDB *aRecv)
{
	EnterCriticalSection(&this->m_GensCriticalSection);

	PMSG_ANS_SECEDE_GENS_MEMBER_EXDB pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x06, sizeof(pMsg));








	pMsg.bIndexH = aRecv->bIndexH;
	pMsg.bIndexL = aRecv->bIndexL;

	sLog.outBasic("[Gens System] Request to Secede: Name:%s -> RESULT: %d", aRecv->Name, pMsg.bResult);

	if(pMsg.bResult == 0)
	{
		std::map<string, GENS_USER, strCmp>::iterator It;

		It = this->m_mapGensRankDuprian.find(aRecv->Name);

		if(It != this->m_mapGensRankDuprian.end())
		{
			this->m_mapGensRankDuprian.erase(It);
			sLog.outBasic( "[Gens System] Delete User (DUPRIAN) (NAME:%s)", aRecv->Name);
		}

		else
		{
			It = this->m_mapGensRankVanert.find(aRecv->Name);

			if(It != this->m_mapGensRankVanert.end())
			{
				this->m_mapGensRankVanert.erase(It);
				sLog.outBasic("[Gens System] Delete User (VANERT) (NAME:%s)", aRecv->Name);
			}
		}
	}
		
	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);

	LeaveCriticalSection(&this->m_GensCriticalSection);
}

void GensSystem_EDS::GDReqSaveContributePoint(int aIndex, PMSG_REQ_SAVE_CONTRIBUTE_POINT_EXDB *aRecv)
{
	EnterCriticalSection(&this->m_GensCriticalSection);

	int iResult = 1; // stub
	if(iResult == 1)
	{
		sLog.outBasic("[Gens System] Save Result: OK (Name:%s ContributePoint:%d GensClass:%d)", aRecv->Name, aRecv->iContributePoint, aRecv->iGensClass);
	}

	else
	{
		sLog.outBasic("[Gens System] Save Result: FAIL (Name:%s ContributePoint:%d GensClass:%d)", aRecv->Name, aRecv->iContributePoint, aRecv->iGensClass);
	}

	LeaveCriticalSection(&this->m_GensCriticalSection);
}

void GensSystem_EDS::GDReqSaveAbusingKillUserName(int aIndex, PMSG_REQ_SAVE_ABUSING_KILLUSER_EXDB *aRecv)
{
	EnterCriticalSection(&this->m_GensCriticalSection);

	int Count = 0;

	LeaveCriticalSection(&this->m_GensCriticalSection);
}

void GensSystem_EDS::GDReqAbusingInfo(int aIndex, PMSG_REQ_ABUSING_INFO *aRecv)
{
	EnterCriticalSection(&this->m_GensCriticalSection);

	PMSG_ANS_ABUSING_INFO pMsg;



	memset(pMsg.KillCount, 0x00, sizeof(pMsg.KillCount));
	memset(pMsg.szCharNames, 0x00, sizeof(pMsg.szCharNames));

	pMsg.iKillUserCount = 0;

	std::memcpy(pMsg.Name, aRecv->Name, 11);
	
	pMsg.bIndexH = aRecv->bIndexH;
	pMsg.bIndexL = aRecv->bIndexL;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x0A, sizeof(pMsg));

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);

	LeaveCriticalSection(&this->m_GensCriticalSection);
}

void GensSystem_EDS::GDReqGensRewardCheck(int aIndex, PMSG_REQ_GENS_REWARD_CHECK_EXDB *aRecv)
{
	EnterCriticalSection(&this->m_GensCriticalSection);

	PMSG_ANS_GENS_REWARD_EXDB pMsg;
	SYSTEMTIME CurrTime;
	GetLocalTime(&CurrTime);

	pMsg.iInfluence = aRecv->iInfluence;
	pMsg.bIndexH = aRecv->wIndexH;
	pMsg.bIndexL = aRecv->wIndexL;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x0D, sizeof(pMsg));

	if(aRecv->iInfluence == LEAVED_INFLUENCE || aRecv->iInfluence == NONE_INFLUENCE)
	{
		pMsg.bResult = 6;
		DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
		LeaveCriticalSection(&this->m_GensCriticalSection);
		return;
	}

	if(!(CurrTime.wDay >= this->m_GensRewardStartDay && CurrTime.wDay <= this->m_GensRewardEndDay))
	{
		pMsg.bResult = 1;
		DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
		LeaveCriticalSection(&this->m_GensCriticalSection);
		return;
	}


	int Reward = 0; // stub
	if (Reward == 0) // No Reward
	{

		pMsg.bResult = 2;
		DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
		LeaveCriticalSection(&this->m_GensCriticalSection);
		return;
	}

	if (Reward == 2) // Already given
	{

		pMsg.bResult = 4;
		DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
		LeaveCriticalSection(&this->m_GensCriticalSection);
		return;
	}

	pMsg.bResult = 0;


	
	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);

	LeaveCriticalSection(&this->m_GensCriticalSection);
}

void GensSystem_EDS::GDReqGensRewardComplete(int aIndex, PMSG_REQ_GENS_REWARD_COMPLETE_EXDB *aRecv)
{
	EnterCriticalSection(&this->m_GensCriticalSection);


	LeaveCriticalSection(&this->m_GensCriticalSection);
}

void GensSystem_EDS::GDReqGensMemberCount(int aIndex, PMSG_REQ_GENS_MEMBER_COUNT *aRecv)
{
	EnterCriticalSection(&this->m_GensCriticalSection);

	PMSG_ANS_GENS_MEMBER_COUNT_EXDB pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x10, sizeof(pMsg));

	pMsg.iDuprianMemberCount = this->m_DuprianUserCount;
	pMsg.iVanertMemberCount = this->m_VanertUserCount;

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);

	LeaveCriticalSection(&this->m_GensCriticalSection);
}

void GensSystem_EDS::GDReqGensRewardDay(int aIndex, PMSG_REQ_GENS_REWARD_DAY *aRecv)
{
	EnterCriticalSection(&this->m_GensCriticalSection);

	PMSG_ANS_GENS_REWARD_DAY_CHECK_EXDB pMsg;
	SYSTEMTIME CurrTime;
	GetLocalTime(&CurrTime);

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x14, sizeof(pMsg));

	pMsg.bIndexH = aRecv->bIndexH;
	pMsg.bIndexL = aRecv->bIndexL;

	if(CurrTime.wDay >= this->m_GensRewardStartDay && CurrTime.wDay <= this->m_GensRewardEndDay)
	{
		pMsg.iRewardDay = TRUE;
	}

	else
	{
		pMsg.iRewardDay = FALSE;
	}

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);

	LeaveCriticalSection(&this->m_GensCriticalSection);
}

void GensSystem_EDS::ReloadCurrentMonth()
{
	EnterCriticalSection(&this->m_GensCriticalSection);







	LeaveCriticalSection(&this->m_GensCriticalSection);
}

void GensSystem_EDS::MakeRanking()
{
	EnterCriticalSection(&this->m_GensCriticalSection);

	sLog.outBasic("[Gens System] Ranking Making [START]");
	//this->m_RankingLog.Output("--------------- [ GENS RANKING START ] ---------------");

	this->m_mapGensRankDuprian.clear();
	this->m_mapGensRankVanert.clear();

	int Position = 1;
	int Class;
	int DBRank;
	float DPoint = 0.0, VPoint = 0.0, AllPoints = 0.0;

	AllPoints = VPoint + DPoint;

	SYSTEMTIME m_CurrentTime;
	GetLocalTime(&m_CurrentTime);

	for(std::map<string, GENS_USER, strCmp>::iterator It = this->m_mapGensRankDuprian.begin(); It != this->m_mapGensRankDuprian.end(); It++)
	{
		sLog.outBasic( "[Gens System] RANKING (DUPRIAN): [NAME:%s] [CONTRIBUTION:%d] [CLASS:%d] [RANK:%d]", It->second.Name, It->second.iContributePoint, It->second.iGensClass, It->second.iRank);
		//this->m_RankingLog.Output("RANKING (DUPRIAN): [NAME:%s] [CONTRIBUTION:%d] [CLASS:%d] [RANK:%d]", It->second.Name, It->second.iContributePoint, It->second.iGensClass, It->second.iRank);
	}

	for(std::map<string, GENS_USER, strCmp>::iterator It = this->m_mapGensRankVanert.begin(); It != this->m_mapGensRankVanert.end(); It++)
	{
		sLog.outBasic("[Gens System] RANKING (VANERT): [NAME:%s] [CONTRIBUTION:%d] [CLASS:%d] [RANK:%d]", It->second.Name, It->second.iContributePoint, It->second.iGensClass, It->second.iRank);
		//this->m_RankingLog.Output("RANKING (VANERT): [NAME:%s] [CONTRIBUTION:%d] [CLASS:%d] [RANK:%d]", It->second.Name, It->second.iContributePoint, It->second.iGensClass, It->second.iRank);
	}

	float DPercent = 0.0;
	float VPercent = 0.0;

	if (AllPoints > 0.0)
	{
		DPoint *= 100.0 / AllPoints;
		VPoint *= 100.0 / AllPoints;
	}

	sLog.outBasic("[Gens System] (D:%0.2f) VS (V:%0.2f)", DPercent, VPercent);
	//this->m_RankingLog.Output("RANKING RESULTS: (D:%f) VS (V:%f)", DPercent, VPercent);

	sLog.outBasic("[Gens System] Ranking Making [END]");
	//this->m_RankingLog.Output("--------------- [ GENS RANKING END ] ---------------");

	LeaveCriticalSection(&this->m_GensCriticalSection);
}

void GensSystem_EDS::ManualRefreshRanking(BYTE Type)
{
	sLog.outBasic("[Gens System] Processing Ranking refresh (Manual, by: %s)", Type == 0 ? "System Administrator" : "Game Master");

	this->ReloadCurrentMonth();
	this->MakeRanking();
}

void CExDataServerProtocol::GDReqGuildMatchingList(int aIndex, _stReqGuildMatchingList * lpMsg)
{
	_stAnsGuildMatchingList pMsg;

	PHeadSubSetW((LPBYTE)&pMsg, 0xA3, 0x00, sizeof(pMsg));

	pMsg.nUserIndex = lpMsg->nUserIndex;
	pMsg.nPage = lpMsg->nPage;

	int nTotalPage = 0;

	int nTotalEntry = 0; // stub
	int nCount = 0; // stub

	if (nTotalEntry > 0)
	{
		nTotalPage = nTotalEntry / 9;

		if (nTotalEntry % 9 > 0)
		{
			nTotalPage++;
		}

		if (nTotalEntry < 9)
		{
			nTotalPage = 1;
		}
	}

	pMsg.nTotalPage = nTotalPage;
	pMsg.nListCount = nCount;

	if (nCount <= 0)
	{
		pMsg.nResult = -1;
	}

	else
	{
		pMsg.nResult = 0;
	}

	DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CExDataServerProtocol::GDReqGuildMatchingListSearchWord(int aIndex, _stReqGuildMatchingListSearchWord *lpMsg)
{
	_stAnsGuildMatchingList pMsg;

	int nCount = 0;

	PHeadSubSetW((LPBYTE)&pMsg, 0xA3, 0x00, sizeof(pMsg));

	pMsg.nUserIndex = lpMsg->nUserIndex;
	pMsg.nPage = lpMsg->nPage;

	int nTotalPage = 0;

	int nTotalEntry = 0; //stub
	if (nTotalEntry > 0)
	{
		nTotalPage = nTotalEntry / 9;

		if (nTotalEntry % 9 > 0)
		{
			nTotalPage++;
		}

		if (nTotalEntry < 9)
		{
			nTotalPage = 1;
		}
	}

	pMsg.nTotalPage = nTotalPage;
	pMsg.nListCount = nCount;

	if (nCount <= 0)
	{
		pMsg.nResult = -1;
	}

	else
	{
		pMsg.nResult = 0;
	}

	DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CExDataServerProtocol::GDReqRegGuildMatchingList(int aIndex, _stReqGuildMatchingData * lpMsg)
{
	_stAnsGuildMatchingData pMsg;

	pMsg.nUserIndex = lpMsg->nUserIndex;
	pMsg.nResult = 0;
	PHeadSubSetB((LPBYTE)&pMsg, 0xA3, 0x02, sizeof(pMsg));

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CExDataServerProtocol::GDReqDelMatchingList(int aIndex, _stReqDelGuildMatchingList * lpMsg)
{
	_stAnsDelGuildMatchingList pMsg;
	_stGuildMatchingAllowListDB stAllowList[80];

	pMsg.nResult = 0;
	pMsg.nUserIndex = lpMsg->nUserIndex;
	PHeadSubSetB((LPBYTE)&pMsg, 0xA3, 0x03, sizeof(pMsg));

	GUILD_INFO_STRUCT* lpGuild = this->GetGuild(lpMsg->nGuildNumber);

	if (!lpGuild)
	{
		DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
		return;
	}

	memset(&stAllowList, 0x00, sizeof(stAllowList));


	_stAnsNotiGuildMatching pNotiMsg;
	PHeadSubSetB((LPBYTE)&pNotiMsg, 0xA3, 0x09, sizeof(pNotiMsg));

	FRIEND_MASTER* lpUser = NULL;

	//EnterCriticalSection(&this->m_FriendSystemEDS.m_csMapFriendMaster);

	int Count = 0; // stub
	int ServerIndex = 0; // stub

	for (int i = 0; i < Count && i < 80; i++)
	{
		lpUser = this->m_FriendSystemEDS.GetFriendMaster(stAllowList[i].szApplicantName);

		if (!lpUser)
		{
			continue;
		}



		if (ServerIndex == -1)
		{
			continue;
		}

		pNotiMsg.nResult = 3;
		pNotiMsg.nUserIndex = lpUser->m_iNumber;

		DataSend(ServerIndex, (LPBYTE)&pNotiMsg, pNotiMsg.h.size, __FUNCTION__);
	}

	//LeaveCriticalSection(&this->m_FriendSystemEDS.m_csMapFriendMaster);

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CExDataServerProtocol::GDReqJoinGuildMatchingList(int aIndex, _stRegWaitGuildMatching * lpMsg)
{
	_stAnsWaitGuildMatching pMsg;

	int State = -1;

	PHeadSubSetB((LPBYTE)&pMsg, 0xA3, 0x04, sizeof(pMsg));
	pMsg.nUserIndex = lpMsg->nUserIndex;

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);

	if (pMsg.nResult == 0)
	{
		this->SendNotiGuildMatchingForGuildMaster(lpMsg->stAllowList.nGuildNumber);
	}
}

void CExDataServerProtocol::GDReqCancelJoinGuildMatching(int aIndex, _stReqDelWaitGuildMatchingList * lpMsg)
{
	_stAnsDelWaitGuildMatchingList pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xA3, 0x05, sizeof(pMsg));

	pMsg.nUserIndex = lpMsg->nUserIndex;
	pMsg.nType = lpMsg->nType;

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CExDataServerProtocol::GDReqAllowJoinGuildMatching(int aIndex, _stReqAllowJoinGuildMatching * lpMsg)
{
	_stAnsAllowJoinGuildMatching pMsg;

	// Step 1
	// Check if guild member exists (user is already in guild)

	BOOL bIsAlreadyInGuild	= this->DBGuildMemberExists(lpMsg->szName);
	int SetMatchingState	= 0;
	int Result				= 0;
	int NotiState			= 1;

	if (bIsAlreadyInGuild == TRUE) // Fail - user is already in this guild
	{
		Result = -4;
	}

	// TODO

	// Step 6
	// Send result

	pMsg.nResult = Result;
	pMsg.nAllowType = lpMsg->nAllowType;
	pMsg.nUserIndex = lpMsg->nUserIndex;
	memcpy(pMsg.szGuildName, lpMsg->szGuildName, MAX_GUILD_LEN + 1);
	memcpy(pMsg.szMemberName, lpMsg->szName, MAX_ACCOUNT_LEN + 1);

	PHeadSubSetB((LPBYTE)&pMsg, 0xA3, 0x06, sizeof(pMsg));

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);

	// Step 7
	// Notify guild member about matching result

	_stAnsNotiGuildMatching pNotiMsg;
	PHeadSubSetB((LPBYTE)&pNotiMsg, 0xA3, 0x09, sizeof(pNotiMsg));

	//EnterCriticalSection(&this->m_FriendSystemEDS.m_csMapFriendMaster);

	FRIEND_MASTER* lpUser = this->m_FriendSystemEDS.GetFriendMaster(pMsg.szMemberName);
	BOOL IsUserFound = FALSE;
	int iUserServerCode = -1;

	if (lpUser)
	{
		pNotiMsg.nUserIndex = lpUser->m_iNumber;
		pNotiMsg.nResult = NotiState;
		IsUserFound = TRUE;
		iUserServerCode = lpUser->m_iServer;
	}

	for (int i = 0; i < g_dwMaxServerGroups; i++)
	{
		if (g_Server[i].m_Index != -1
			&& g_Server[i].m_State == SS_GUILD
			&& g_Server[i].m_Type == ST_EXDATASERVER
			&& g_Server[i].m_ServerCode == iUserServerCode
			&& IsUserFound == TRUE)
		{
			DataSend(i, (LPBYTE)&pNotiMsg, pNotiMsg.h.size, __FUNCTION__);
		}
	}

	//LeaveCriticalSection(&this->m_FriendSystemEDS.m_csMapFriendMaster);
}

void CExDataServerProtocol::GDReqWaitGuildMatchingList(int aIndex, _stReqWaitGuildMatchingList * lpMsg)
{
	_stAnsWaitGuildMatchingList pMsg;

	int Count = 0;

	if (Count <= 0)
	{
		pMsg.nResult = -3;
	}

	else
	{
		pMsg.nResult = 0;
	}

	pMsg.nListCount = Count;
	pMsg.nUserIndex = lpMsg->nUserIndex;

	PHeadSubSetW((LPBYTE)&pMsg, 0xA3, 0x07, sizeof(pMsg));
	DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CExDataServerProtocol::GDReqGetWaitStateListGuildMatching(int aIndex, _stReqWaitStateListGuildMatching * lpMsg)
{
	_stAnsWaitStateListGuildMatching pMsg;

	pMsg.nUserIndex = lpMsg->nUserIndex;
	PHeadSubSetB((LPBYTE)&pMsg, 0xA3, 0x08, sizeof(pMsg));

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CExDataServerProtocol::GuildMatchingSendAcceptAndRejectInfo(int aIndex, char* szUserName, int nUserIndex, int nServerIndex)
{
	_stAnsNotiGuildMatching pNotiMsg;

	if (!szUserName)
	{
		return;
	}


	PHeadSubSetB((LPBYTE)&pNotiMsg, 0xA3, 0x09, sizeof(pNotiMsg));

	pNotiMsg.nUserIndex = nUserIndex;


	DataSend(nServerIndex, (LPBYTE)&pNotiMsg, sizeof(pNotiMsg), __FUNCTION__);
}

void CExDataServerProtocol::SendNotiGuildMatchingForGuildMaster(int nGuildNumber)
{
	_stAnsNotiGuildMatchingForGuildMaster pMsg;

	GUILD_INFO_STRUCT* lpGuild = this->GetGuild(nGuildNumber);

	if (!lpGuild)
	{
		return;
	}

	//EnterCriticalSection(&this->m_FriendSystemEDS.m_csMapFriendMaster);

	FRIEND_MASTER* lpUser = this->m_FriendSystemEDS.GetFriendMaster(lpGuild->m_szGuildMaster);

	if (!lpUser)
	{
		//LeaveCriticalSection(&this->m_FriendSystemEDS.m_csMapFriendMaster);
		return;
	}



	int nResult = 0; // stub

	if (nResult != 1)
	{
		//LeaveCriticalSection(&this->m_FriendSystemEDS.m_csMapFriendMaster);
		return;
	}

	PHeadSubSetB((LPBYTE)&pMsg, 0xA3, 0x10, sizeof(pMsg));

	pMsg.nUserIndex = lpUser->m_iNumber;
	pMsg.nResult = 0;


	int ServerIndex = 0;
	if (ServerIndex == -1)
	{
		//LeaveCriticalSection(&this->m_FriendSystemEDS.m_csMapFriendMaster);
		return;
	}

	DataSend(ServerIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
	//LeaveCriticalSection(&this->m_FriendSystemEDS.m_csMapFriendMaster);
}

void CExDataServerProtocol::SendUseGuildMatchingGuild(char* szName, int nGuildNumber)
{
	_stAnsUseGuildMatchingGuild pMsg;

	STR_GUILD_MEMBER* lpMember = this->GetGuildMember(nGuildNumber, szName);

	if (!lpMember)
	{
		return;
	}

	//EnterCriticalSection(&this->m_FriendSystemEDS.m_csMapFriendMaster);

	FRIEND_MASTER* lpUser = this->m_FriendSystemEDS.GetFriendMaster(szName);

	if (!lpUser)
	{
		//LeaveCriticalSection(&this->m_FriendSystemEDS.m_csMapFriendMaster);
		return;
	}

	PHeadSubSetB((LPBYTE)&pMsg, 0xA3, 0x11, sizeof(pMsg));
	pMsg.nUserIndex = lpUser->m_iNumber;

	//LeaveCriticalSection(&this->m_FriendSystemEDS.m_csMapFriendMaster);

	int ServerIndex = 0;
	if (ServerIndex == -1)
	{
		return;
	}

	DataSend(ServerIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CExDataServerProtocol::UpdateGuildMatchingMemberCount(char *szGuildName, int nGuildMemberCnt)
{



}

void CExDataServerProtocol::GDReqRegWantedPartyMember(int aIndex, _stReqRegWantedPartyMember * lpMsg)
{
	_stAnsRegWantedPartyMember pMsg;

	//EnterCriticalSection(&this->m_FriendSystemEDS.m_csMapFriendMaster);

	FRIEND_MASTER* lpLeaderData = this->m_FriendSystemEDS.GetFriendMaster(lpMsg->szPartyLeaderName);



	pMsg.nUserIndex = lpMsg->nUserIndex;
	PHeadSubSetB((LPBYTE)&pMsg, 0xA4, 0x00, sizeof(pMsg));


	//LeaveCriticalSection(&this->m_FriendSystemEDS.m_csMapFriendMaster);

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CExDataServerProtocol::GDReqGetPartyMatchingList(int aIndex, _stReqGetPartyMatchingList * lpMsg)
{
	_stAnsGetPartyMatchingList pMsg;


	PHeadSubSetW((LPBYTE)&pMsg, 0xA4, 0x01, sizeof(pMsg));

	pMsg.nUserIndex = lpMsg->nUserIndex;
	pMsg.nPage = lpMsg->nPage;

	int nTotalPage = 0;

	int nTotalEntry = 0; // stub
	if (nTotalEntry > 0)
	{
		nTotalPage = nTotalEntry / 6;

		if (nTotalEntry % 6 > 0)
		{
			nTotalPage++;
		}

		if (nTotalEntry < 6)
		{
			nTotalPage = 1;
		}
	}

	int nCount = 0;
	pMsg.nTotalPage = nTotalPage;
	pMsg.nListCount = nCount;
	pMsg.nResult = 0;

	DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CExDataServerProtocol::GDReqJoinMemberPartyMatching(int aIndex, _stReqJoinMemberPartyMatching * lpMsg)
{
	_stAnsJoinMemberPartyMatching pMsg;



	BOOL bExistOnWaitList = FALSE;

	//EnterCriticalSection(&this->m_FriendSystemEDS.m_csMapFriendMaster);

	FRIEND_MASTER* lpPartyMaster = this->m_FriendSystemEDS.GetFriendMaster(lpMsg->szLeaderName);
	FRIEND_MASTER* lpPartyMember = this->m_FriendSystemEDS.GetFriendMaster(lpMsg->szMemberName);

	int MasterServerIndex = -1, MemberServerIndex = -1;

	if (pMsg.nResult == 0 && lpPartyMaster != NULL && lpPartyMaster->m_iServer != -1)
	{
		_stAnsRequestJoinPartyMatchingNoti pNotiMsg;
		PHeadSubSetB((LPBYTE)&pNotiMsg, 0xA4, 0x08, sizeof(pNotiMsg));

		pNotiMsg.nUserIndex = lpPartyMaster->m_iNumber;

		DataSend(MasterServerIndex, (LPBYTE)&pNotiMsg, pNotiMsg.h.size, __FUNCTION__);
	}

	PHeadSubSetB((LPBYTE)&pMsg, 0xA4, 0x02, sizeof(pMsg));

	pMsg.nUserIndex = lpMsg->nUserIndex;

	//LeaveCriticalSection(&this->m_FriendSystemEDS.m_csMapFriendMaster);
	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CExDataServerProtocol::GDReqJoinMemberStateList(int aIndex, _stReqJoinMemberStateListPartyMatching *lpMsg)
{
	_stAnsJoinMemberStateListPartyMatching pMsg;


	PHeadSubSetB((LPBYTE)&pMsg, 0xA4, 0x03, sizeof(pMsg));
	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CExDataServerProtocol::GDReqJoinMemberStateListLeader(int aIndex, _stReqWaitListPartyMatching *lpMsg)
{
	_stAnsWaitListPartyMatching pMsg;

	int nCount = 0;

	pMsg.nListCount = nCount;
	pMsg.nResult = 0;
	pMsg.nUserIndex = lpMsg->nUserIndex;
	PHeadSubSetW((LPBYTE)&pMsg, 0xA4, 0x04, sizeof(pMsg));

	DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CExDataServerProtocol::GDReqAcceptMemberJoin(int aIndex, _stReqAddPartyMember *lpMsg)
{
	_stAnsAddPartyMember pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xA4, 0x05, sizeof(pMsg));

	BOOL bUserFound = FALSE;
	//EnterCriticalSection(&this->m_FriendSystemEDS.m_csMapFriendMaster);
	FRIEND_MASTER* lpMember = this->m_FriendSystemEDS.GetFriendMaster(lpMsg->szMemberName);
	std::memcpy(pMsg.szMemberName, lpMsg->szMemberName, MAX_ACCOUNT_LEN + 1);

	int nMasterIndex = -1, nMemberIndex = -1, nMemberServerIndex = -1, nMemberServer = -1;

	if (lpMember)
	{
		bUserFound = TRUE;
		nMemberIndex = lpMember->m_iNumber;

		nMemberServer = lpMember->m_iServer;
	}

	else if (lpMsg->btAlreadyParty == TRUE)
	{
		bUserFound = TRUE;
		nMemberIndex = lpMsg->nMemberIndex;
		nMemberServerIndex = aIndex;
		nMemberServer = g_Server[aIndex].m_ServerCode;
	}

	//LeaveCriticalSection(&this->m_FriendSystemEDS.m_csMapFriendMaster);

	pMsg.nUserIndex = lpMsg->nUserIndex;
	pMsg.nMemberIndex = nMemberIndex;
	pMsg.btManualJoin = lpMsg->btManualJoin;

	if (lpMsg->btAlreadyParty == TRUE)
	{
		int iResult = this->m_PartyMatchingEDS.AddPartyMember(lpMsg->szLeaderName, lpMsg->szMemberName, nMemberIndex, nMemberServer, nMemberServerIndex);

		if (iResult == 0)
		{
			PARTYMATCHING_PARTYDATA * lpPartyData = this->m_PartyMatchingEDS.GetPartyDataByMaster(lpMsg->szLeaderName);

			if (lpPartyData)
			{



			}
		}
	}

	else
	{
		if (bUserFound == FALSE)
		{
			pMsg.nResult = -1;
			pMsg.btSendType = 0;
			DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
			return;
		}

		if (lpMsg->btType == 1)
		{
			pMsg.nResult = this->m_PartyMatchingEDS.AddPartyMember(lpMsg->szLeaderName, lpMsg->szMemberName, nMemberIndex, nMemberServer, nMemberServerIndex);
			pMsg.btSendType = 0;
			pMsg.btType = lpMsg->btType;

			if (pMsg.nResult == 0)
			{
				PARTYMATCHING_PARTYDATA * lpPartyData = this->m_PartyMatchingEDS.GetPartyDataByMaster(lpMsg->szLeaderName);

				if (lpPartyData)
				{



				}
			}

			DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);

			pMsg.btSendType = 1;
			DataSend(nMemberServerIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);

			if (nMemberServerIndex == aIndex)
			{
				this->FixAddPartyMember(lpMsg->nUserIndex, nMemberIndex, aIndex);
			}

			this->SendPartyMatchingMemberList(lpMsg->szLeaderName, 0);
		}

		else
		{
			pMsg.nUserIndex = lpMsg->nUserIndex;
			pMsg.nMemberIndex = nMemberIndex;
			pMsg.btType = lpMsg->btAlreadyParty;
			pMsg.btSendType = 0;
			pMsg.nResult = 0;

			DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);

			pMsg.btSendType = 1;
			DataSend(nMemberServerIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
		}




	}
}

void CExDataServerProtocol::GDReqCancelPartyMatching(int aIndex, _stReqCancelPartyMatching *lpMsg)
{
	_stAnsCancelPartyMatching pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xA4, 0x06, sizeof(pMsg));

	pMsg.btType = lpMsg->btType;
	pMsg.nResult = 0;

	if (lpMsg->btType)
	{




		PARTYMATCHING_PARTYDATA * lpPartyData = this->m_PartyMatchingEDS.GetPartyDataByMember(lpMsg->szName);

		if (lpPartyData)
		{
			this->m_PartyMatchingEDS.DelPartyMember(lpPartyData->szLeaderName, lpMsg->szName);





			this->SendPartyMatchingMemberList(lpPartyData->szLeaderName, 0);
		}
	}

	else
	{




		PARTYMATCHING_PARTYDATA * lpPartyData = this->m_PartyMatchingEDS.GetPartyDataByMaster(lpMsg->szName);

		if (lpPartyData)
		{
			if (strcmp(lpPartyData->szLeaderName, lpMsg->szName))
			{
				this->m_PartyMatchingEDS.DelPartyMember(lpMsg->szName, lpPartyData->szLeaderName);
		



			}

			else
			{
				if (lpPartyData->m_UserCount > 1)
				{
					for (int i = 1; i < 5; i++)
					{
						if (lpPartyData->m_UserData[i].iUsedInfo == TRUE && lpPartyData->m_UserData[i].iUserConnected == TRUE)
						{
							pMsg.nUserIndex = lpPartyData->m_UserData[i].iUserIndex;
							DataSend(lpPartyData->m_UserData[i].iServerIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
						}
					}
				}

				this->m_PartyMatchingEDS.DeleteParty(lpMsg->szName);



				int nCount = 0;

				_PARTY_MEMBER_WAIT_LIST stList[80];


				{




					nCount++;
				}



				if (nCount > 0)
				{
					//EnterCriticalSection(&this->m_FriendSystemEDS.m_csMapFriendMaster);

					for (int i = 0; i < nCount; i++)
					{
						FRIEND_MASTER* lpUser = this->m_FriendSystemEDS.GetFriendMaster(stList[i].szName);

						if (lpUser)
						{
							pMsg.btType = lpMsg->btType;
							pMsg.nUserIndex = lpUser->m_iNumber;
							pMsg.nResult = 1;


							int ServerIndex = 0; // stub
							DataSend(ServerIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
						}
					}

					//LeaveCriticalSection(&this->m_FriendSystemEDS.m_csMapFriendMaster);
				}
			}
		}

		else
		{
			pMsg.nResult = -1;
		}
	}

	pMsg.btType = lpMsg->btType;
	pMsg.nUserIndex = lpMsg->nUserIndex;

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CExDataServerProtocol::GDReqDeletePartyUser(int aIndex, _stReqDelPartyUserPartyMatching *lpMsg)
{
	_stAnsDelPartyUserPartyMatching pMsg;

	PARTYMATCHING_PARTYDATA * lpPartyData = this->m_PartyMatchingEDS.GetPartyDataByMember(lpMsg->szTargetName);
	PARTYMATCHING_PARTYUSERDATA * lpMemberData = this->m_PartyMatchingEDS.GetPartyMemberData(lpMsg->szTargetName);

	if (!lpPartyData || !lpMemberData)
	{
		return;
	}

	char szLeaderName[MAX_ACCOUNT_LEN + 1];
	memcpy(szLeaderName, lpPartyData->m_UserData[0].szUserName, MAX_ACCOUNT_LEN + 1);

	BOOL bResult = 0;

	int nLeaderServerIndex = lpPartyData->m_UserData[0].iServerIndex;
	int nLeaderIndex = lpPartyData->m_UserData[0].iUserIndex;
	int nUserServerIndex = -1;
	int nUserIndex = -1;

	for (int i = 0; i < 5; i++)
	{
		if (!strcmp(lpPartyData->m_UserData[i].szUserName, lpMsg->szTargetName))
		{
			nUserIndex = lpPartyData->m_UserData[i].iUserIndex;
			nUserServerIndex = lpPartyData->m_UserData[i].iServerIndex;
		}
	}

	PHeadSubSetB((LPBYTE)&pMsg, 0xA4, 0x07, sizeof(pMsg));
	pMsg.nUserIndex = lpMsg->nUserIndex;
	pMsg.nTargetIndex = nUserIndex;
	pMsg.nResult = 0;
	pMsg.btType = 0;

	if (pMsg.nUserIndex == pMsg.nTargetIndex)
	{
		DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
	}

	else
	{
		pMsg.btType = 0;
		DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);

		if (nUserServerIndex < 0)
		{
			sLog.outBasic("[Party Matching] User not connected (%s)", lpMsg->szTargetName);
			return;
		}

		pMsg.btType = 1;
		DataSend(nUserServerIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
	}
}

void CExDataServerProtocol::GDReqSendPartyMemberList(int aIndex, _stReqSendPartyMemberList * lpMsg)
{
	this->SendPartyMatchingMemberList(lpMsg->szLeaderName, 0);
}

void CExDataServerProtocol::GDSendChatMsgPartyMatching(int aIndex, _stReqChattingPartyMatching * lpMsg)
{
	_stAnsChattingPartyMatching pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xA4, 0x12, sizeof(pMsg));

	PARTYMATCHING_PARTYDATA * lpPartyData = this->m_PartyMatchingEDS.GetPartyDataByNumber(lpMsg->nPartyIndex);

	if (!lpPartyData)
	{
		return;
	}

	pMsg.nPartyIndex = lpMsg->nPartyIndex;
	memset(pMsg.szChat, 0x00, sizeof(pMsg.szChat));
	memcpy(pMsg.szSendCharName, lpMsg->szSendCharName, MAX_ACCOUNT_LEN + 1);
	memcpy(pMsg.szChat, lpMsg->szChat, 63);

	for (int i = 0; i < 5; i++)
	{
		if (lpPartyData->m_UserData[i].iUsedInfo == TRUE && lpPartyData->m_UserData[i].iUserConnected == TRUE)
		{
			pMsg.nRecvUserIndex = lpPartyData->m_UserData[i].iUserIndex;
			std::memcpy(pMsg.szSendCharName, lpMsg->szSendCharName, MAX_ACCOUNT_LEN + 1);
			std::memcpy(pMsg.szChat, lpMsg->szChat, 63);

			DataSend(lpPartyData->m_UserData[i].iServerIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
		}
	}
}

void CExDataServerProtocol::AutoAddPartyMember(char *szLeaderName, char *szMemberName, int nUserIndex, int nServerIndex)
{
	EnterCriticalSection(&this->m_FriendSystemEDS.m_csMapFriendMaster);

	FRIEND_MASTER* lpMaster = this->m_FriendSystemEDS.GetFriendMaster(szLeaderName);
	FRIEND_MASTER* lpUser = this->m_FriendSystemEDS.GetFriendMaster(szMemberName);

	if (!lpMaster || !lpUser)
	{
		LeaveCriticalSection(&this->m_FriendSystemEDS.m_csMapFriendMaster);
		return;
	}

	_stAnsAddPartyMember pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xA4, 0x05, sizeof(pMsg));

	PARTYMATCHING_PARTYDATA * lpPartyData = this->m_PartyMatchingEDS.GetPartyDataByMaster(szLeaderName);

	if (lpPartyData == NULL)
	{
		pMsg.nResult = -3;
		pMsg.btSendType = 1;

		DataSend(nServerIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);

		sLog.outBasic("[Party Matching] Add Error - party with leader (%s) not exists", szLeaderName);
		LeaveCriticalSection(&this->m_FriendSystemEDS.m_csMapFriendMaster);
		return;
	}

	pMsg.nResult = this->m_PartyMatchingEDS.AddPartyMember(szLeaderName, szMemberName, nUserIndex, lpUser->m_iServer, nServerIndex);
	pMsg.nUserIndex = lpMaster->m_iNumber;
	pMsg.nMemberIndex = lpUser->m_iNumber;
	pMsg.btType = 2;
	pMsg.btSendType = 0;
	pMsg.btManualJoin = 0;

	int nMasterServerIndex = 0; // stub
	DataSend(nMasterServerIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);

	pMsg.btSendType = 1;
	DataSend(nServerIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);

	if (nMasterServerIndex == nServerIndex)
	{
		this->FixAddPartyMember(lpMaster->m_iNumber, lpUser->m_iNumber, nServerIndex);
	}

	//LeaveCriticalSection(&this->m_FriendSystemEDS.m_csMapFriendMaster);
	this->SendPartyMatchingMemberList(szLeaderName, 0);
}

void CExDataServerProtocol::FixAddPartyMember(int nLeaderIndex, int nMemberIndex, int nServerIndex)
{
	_stAnsAddRealPartyMember pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xA4, 0x10, sizeof(pMsg));

	pMsg.nLeaderUserIndex = nLeaderIndex;
	pMsg.nMemberUserIndex = nMemberIndex;

	DataSend(nServerIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CExDataServerProtocol::SendPartyMatchingMemberList(char* szLeaderName, BYTE btType)
{
	PARTYMATCHING_PARTYDATA * lpPartyData = this->m_PartyMatchingEDS.GetPartyDataByMaster(szLeaderName);

	if (!lpPartyData)
	{
		return;
	}

	if (lpPartyData->m_UserCount <= 1)
	{
		btType = 1;
	}

	for (int i = 0; i < 5; i++)
	{
		if (lpPartyData->m_UserData[i].iUserConnected == TRUE && lpPartyData->m_UserData[i].iUsedInfo == TRUE)
		{
			_stAnsSendPartyMemberList pMsg;
			PHeadSubSetB((LPBYTE)&pMsg, 0xA4, 0x11, sizeof(pMsg));

			pMsg.nMemberCount = lpPartyData->m_UserCount;
			pMsg.nUserIndex = lpPartyData->m_UserData[i].iUserIndex;
			pMsg.nPartyMatchingIndex = lpPartyData->m_PartyMatchingIndex;

			if (btType)
			{
				pMsg.nMemberCount = 0;
			}

			else
			{
				for (int j = 0; j < 5; j++)
				{
					if (lpPartyData->m_UserData[j].iUsedInfo == TRUE)
					{
						memcpy(pMsg.stList[j].Name, lpPartyData->m_UserData[j].szUserName, MAX_ACCOUNT_LEN + 1);
						pMsg.stList[j].bUse = lpPartyData->m_UserData[j].iUsedInfo;
						pMsg.stList[j].nServerChannel = lpPartyData->m_UserData[j].iServerNumber + 1;
						pMsg.stList[j].nUserIndex = lpPartyData->m_UserData[j].iUserIndex;
						pMsg.stList[j].bFlag = lpPartyData->m_UserData[j].iServerIndex == lpPartyData->m_UserData[i].iServerIndex; // j == i
					}
				}
			}

			DataSend(lpPartyData->m_UserData[i].iServerIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
		}
	}
}

void CExDataServerProtocol::SendPartyMatchingOneList(char *szMemberName)
{
	PARTYMATCHING_PARTYDATA * lpPartyData = this->m_PartyMatchingEDS.GetPartyDataByMember(szMemberName);

	if (!lpPartyData)
	{
		return;
	}

	for (int i = 0; i < 5; i++)
	{
		if (lpPartyData->m_UserData[i].iUsedInfo == TRUE && lpPartyData->m_UserData[i].iUserConnected == TRUE)
		{
			if (!strcmp(szMemberName, lpPartyData->m_UserData[i].szUserName))
			{
				_stAnsSendPartyMemberList pMsg;
				PHeadSubSetB((LPBYTE)&pMsg, 0xA4, 0x11, sizeof(pMsg));

				pMsg.nMemberCount = lpPartyData->m_UserCount;
				pMsg.nPartyMatchingIndex = lpPartyData->m_PartyMatchingIndex;
				pMsg.nUserIndex = lpPartyData->m_UserData[i].iUserIndex;
				pMsg.nMemberCount = 0;

				DataSend(lpPartyData->m_UserData[i].iServerIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
				return;
			}
		}
	}
}

CPartyMatchingData::CPartyMatchingData()
{
	this->m_vtPartyData.clear();
	InitializeCriticalSection(&this->m_criti);
}

CPartyMatchingData::~CPartyMatchingData()
{
	DeleteCriticalSection(&this->m_criti);
}

void CPartyMatchingData::Init()
{

}

PARTYMATCHING_PARTYDATA * CPartyMatchingData::GetPartyDataByMaster(char *szLeaderName)
{
	for (std::vector<PARTYMATCHING_PARTYDATA>::iterator It = this->m_vtPartyData.begin(); It != this->m_vtPartyData.end(); It++)
	{
		if (strcmp(It->szLeaderName, szLeaderName) == 0)
		{
			return &*It;
		}
	}

	return NULL;
}

PARTYMATCHING_PARTYDATA * CPartyMatchingData::GetPartyDataByMember(char *szMemberName)
{
	for (std::vector<PARTYMATCHING_PARTYDATA>::iterator It = this->m_vtPartyData.begin(); It != this->m_vtPartyData.end(); It++)
	{
		for (int i = 0; i < 5;i++)
		{
			if (!strcmp(It->m_UserData[i].szUserName, szMemberName))
			{
				return &*It;
			}
		}
	}

	return NULL;
}

PARTYMATCHING_PARTYDATA * CPartyMatchingData::GetPartyDataByNumber(int nPartyMatchingIndex)
{
	for (std::vector<PARTYMATCHING_PARTYDATA>::iterator It = this->m_vtPartyData.begin(); It != this->m_vtPartyData.end(); It++)
	{
		if (It->m_PartyMatchingIndex == nPartyMatchingIndex)
		{
			return &*It;
		}
	}

	return NULL;
}

PARTYMATCHING_PARTYUSERDATA * CPartyMatchingData::GetPartyMemberData(char *szMemberName)
{
	for (std::vector<PARTYMATCHING_PARTYDATA>::iterator It = this->m_vtPartyData.begin(); It != this->m_vtPartyData.end(); It++)
	{
		for (int i = 0; i < 5; i++)
		{
			if (!strcmp(It->m_UserData[i].szUserName, szMemberName))
			{
				return &It->m_UserData[i];
			}
		}
	}

	return NULL;
}

void CPartyMatchingData::CreateParty(int Server, int ServerIndex, int nLeaderIndex, char *szLeaderName)
{
	EnterCriticalSection(&this->m_criti);

	if (this->GetPartyDataByMaster(szLeaderName) != NULL)
	{
		sLog.outError("[Party Matching] ERROR - Party with Leader %s already exists", szLeaderName);
		return;
	}

	PARTYMATCHING_PARTYDATA m_PartyData;

	m_PartyData.m_PartyMatchingIndex = (int)this->m_vtPartyData.size();
	m_PartyData.m_UserCount = 1;
	memcpy(m_PartyData.szLeaderName, szLeaderName, MAX_ACCOUNT_LEN + 1);
	m_PartyData.m_UserData[0].iUsedInfo = TRUE;
	m_PartyData.m_UserData[0].iUserConnected = TRUE;
	m_PartyData.m_UserData[0].iServerIndex = ServerIndex;
	m_PartyData.m_UserData[0].iServerNumber = Server;
	m_PartyData.m_UserData[0].iUserIndex = nLeaderIndex;
	memcpy(m_PartyData.m_UserData[0].szUserName, szLeaderName, MAX_ACCOUNT_LEN + 1);

	this->m_vtPartyData.push_back(m_PartyData);

	sLog.outBasic("[Party Matching] Create Party (Leader: %s) (Total Party Count:%d)", szLeaderName, this->m_vtPartyData.size());

	LeaveCriticalSection(&this->m_criti);
}

void CPartyMatchingData::DeleteParty(char *szLeaderName)
{
	EnterCriticalSection(&this->m_criti);
	PARTYMATCHING_PARTYDATA * lpPartyData = this->GetPartyDataByMaster(szLeaderName);

	if (!lpPartyData)
	{
		return;
	}

	for (std::vector<PARTYMATCHING_PARTYDATA>::iterator it = this->m_vtPartyData.begin(); it != this->m_vtPartyData.end(); it++)
	{
		if (strcmp(szLeaderName, it->szLeaderName) == 0)
		{
			this->m_vtPartyData.erase(it);

			sLog.outBasic("[Party Matching] Delete Party (Leader: %s) (Total Party Count:%d)", szLeaderName, this->m_vtPartyData.size());
			break;
		}
	}

	LeaveCriticalSection(&this->m_criti);
}

int CPartyMatchingData::AddPartyMember(char *szLeaderName, char *szMemberName, int nMemberIndex, int nMemberServer, int nMemberServerIndex)
{
	PARTYMATCHING_PARTYDATA * lpPartyData = NULL;
	BOOL bPartyFound = FALSE;
	int bResult = 0;

	EnterCriticalSection(&this->m_criti);
	
	lpPartyData = this->GetPartyDataByMaster(szLeaderName);

	if (lpPartyData != NULL)
	{
		bPartyFound = TRUE;
	}

	if (bPartyFound == FALSE)
	{
		bResult = -1;
	}

	else
	{
		for (int i = 0; i < 5; i++)
		{
			if (strcmp(szMemberName, lpPartyData->m_UserData[i].szUserName) == 0)
			{
				bResult = -4;
				break;
			}
		}

		if (bResult != -4)
		{
			bResult = -2;

			for (int i = 0; i < 5; i++)
			{
				if (lpPartyData->m_UserData[i].iUsedInfo == FALSE)
				{
					lpPartyData->m_UserCount++;
					lpPartyData->m_UserData[i].iUsedInfo = TRUE;
					lpPartyData->m_UserData[i].iUserConnected = TRUE;
					lpPartyData->m_UserData[i].iUserIndex = nMemberIndex;
					lpPartyData->m_UserData[i].iServerNumber = nMemberServer;
					lpPartyData->m_UserData[i].iServerIndex = nMemberServerIndex;
					std::memcpy(lpPartyData->m_UserData[i].szUserName, szMemberName, MAX_ACCOUNT_LEN + 1);

					sLog.outBasic("[Party Matching] Add Party Member (Leader: %s) (Member: %s) (Total users in party: %d)", szLeaderName, szMemberName, lpPartyData->m_UserCount);
					bResult = 0;
					break;
				}
			}
		}
	}

	LeaveCriticalSection(&this->m_criti);

	return bResult;
}

int CPartyMatchingData::DelPartyMember(char *szLeaderName, char *szMemberName)
{
	EnterCriticalSection(&this->m_criti);

	PARTYMATCHING_PARTYDATA * lpPartyData = this->GetPartyDataByMaster(szLeaderName);

	if (lpPartyData == NULL)
	{
		LeaveCriticalSection(&this->m_criti);
		return -1;
	}

	for (int i = 0; i < 5; i++)
	{
		if (lpPartyData->m_UserData[i].iUsedInfo == TRUE)
		{
			if (!strcmp(szMemberName, lpPartyData->m_UserData[i].szUserName))
			{
				lpPartyData->m_UserData[i].iUsedInfo = FALSE;
				lpPartyData->m_UserData[i].iUserConnected = FALSE;
				lpPartyData->m_UserData[i].iServerIndex = -1;
				lpPartyData->m_UserData[i].iServerNumber = -1;
				lpPartyData->m_UserData[i].iUserIndex = -1;
				memset(lpPartyData->m_UserData[i].szUserName, 0x00, sizeof(lpPartyData->m_UserData[i].szUserName));
				lpPartyData->m_UserCount--;

				sLog.outBasic("[Party Matching] Delete Party Member (Leader: %s) (Member: %s) (Total users in party: %d)", szLeaderName, szMemberName, lpPartyData->m_UserCount);

				LeaveCriticalSection(&this->m_criti);
				return 0;
			}
		}
	}

	LeaveCriticalSection(&this->m_criti);
	return -2;
}

int CPartyMatchingData::ConnectMember(char *szMemberName, int nMemberIndex, int nChannelNumber, int nChannelIndex)
{
	PARTYMATCHING_PARTYDATA * lpPartyData = this->GetPartyDataByMember(szMemberName);

	if (!lpPartyData)
	{
		return -1;
	}

	EnterCriticalSection(&this->m_criti);

	int iArrayNum;

	for (iArrayNum = 0;; iArrayNum++)
	{
		if (iArrayNum >= 5)
		{
			LeaveCriticalSection(&this->m_criti);
			return 0;
		}

		if (!strcmp(lpPartyData->m_UserData[iArrayNum].szUserName, szMemberName))
			break;
	}

	lpPartyData->m_UserData[iArrayNum].iUserIndex = nMemberIndex;
	lpPartyData->m_UserData[iArrayNum].iServerNumber = nChannelNumber;
	lpPartyData->m_UserData[iArrayNum].iServerIndex = nChannelIndex;
	lpPartyData->m_UserData[iArrayNum].iUserConnected = TRUE;

	sLog.outBasic("[Party Matching] Member connected (Name:%s) (Leader:%s)", szMemberName, lpPartyData->szLeaderName);
	LeaveCriticalSection(&this->m_criti);

	return 1;
}

int CPartyMatchingData::DisconnectMember(char *szMemberName)
{
	PARTYMATCHING_PARTYDATA * lpPartyData = this->GetPartyDataByMember(szMemberName);

	if (!lpPartyData)
	{
		return -1;
	}

	EnterCriticalSection(&this->m_criti);

	BOOL bMemberFound = FALSE;
	BOOL bPartyDelete = TRUE;

	for (int i = 0; i < 5; i++)
	{
		if (!strcmp(szMemberName, lpPartyData->m_UserData[i].szUserName))
		{
			lpPartyData->m_UserData[i].iServerIndex = -1;
			lpPartyData->m_UserData[i].iServerNumber = -1;
			lpPartyData->m_UserData[i].iUserConnected = FALSE;

			sLog.outBasic("[Party Matching] Member disconnected (Name:%s) (Leader:%s)", szMemberName, lpPartyData->szLeaderName);
			bMemberFound = TRUE;
		}

		if (i >= 0 && lpPartyData->m_UserData[i].iUsedInfo == TRUE)
		{
			if (lpPartyData->m_UserData[i].iUserConnected == TRUE)
			{
				bPartyDelete = FALSE;
			}
		}
	}

	if (lpPartyData->m_UserCount <= 1)
		bPartyDelete = TRUE;


	LeaveCriticalSection(&this->m_criti);
	return bMemberFound;
}
