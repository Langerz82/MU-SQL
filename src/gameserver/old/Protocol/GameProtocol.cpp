#include "StdAfx.h"
#include "GameProtocol.h"
#include "Main.h"
#include "IOCP.h"
#include "generalStructs.h"
#include "ConnectEngine.h"
#include "Utility/util.h"

#include "TNotice.h"

GameProtocol GSProtocol;

GameProtocol::GameProtocol()
{
}

GameProtocol::~GameProtocol()
{
}

bool GameProtocol::DataEncryptCheck(int aIndex, BYTE protoNum, BOOL Encrypt)
{
#ifndef EMU_NOCRYPT
	if (Encrypt == FALSE)
	{
		
		sLog->outError("Error-L1 : Not Encrypt %s %d", aIndex, protoNum);
		//this->GCSendDisableReconnect(aIndex);
		GIOCP.CloseClient(aIndex);
		return false;
	}
#endif
	return true;
}


void GameProtocol::ProtocolCore(BYTE protoNum, unsigned char * aRecv, int aLen, int aIndex, BOOL Encrypt)
{

	/*if (gStalkProtocol)
	{
		if (gStalkProtocolId[0] == gObj[aIndex].AccountID[0])
		{
			if (gStalkProtocolId[1] == gObj[aIndex].AccountID[1])
			{
				if (!strcmp(gStalkProtocolId, gObj[aIndex].AccountID))
				{
					g_Log.AddHeadHex("DATA RECV", aRecv, aLen);
				}
			}
		}
	}*/
	//g_Log.AddC(TColor::Red, "K2(ProtocolCore) : %x %x %x", aRecv[0], aRecv[1], aRecv[2]);
	if (protoNum == 0xF1 || protoNum == 0xF3)
	{
		switch (protoNum)
		{
		case 0x0E:
			if (this->DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE)
			{
				this->CGLiveClient((PMSG_CLIENTTIME *)aRecv, aIndex);
			}
			break;
		case 0xF1:
		{
			PMSG_DEFAULT2 * lpMsg2 = (PMSG_DEFAULT2 *)aRecv;

			switch (lpMsg2->subcode)
			{
			case 0x01:
				if (this->DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE)
				{

					this->CSPJoinIdPassRequest((PMSG_IDPASS *)aRecv, aIndex);
				}
				break;
			case 0x02:
				if (this->DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE)
				{
					//if (gObj[aIndex].m_bMapSvrMoveQuit == false)
						//this->CGClientCloseMsg((PMSG_CLIENTCLOSE *)aRecv, aIndex);
					//else
					//{
					GIOCP.CloseClient(aIndex);
					//}
				}
				break;
			case 0x03:
				if (this->DataEncryptCheck(aIndex, protoNum, Encrypt) != FALSE)
				{
					this->CGClientMsg((PMSG_CLIENTMSG *)aRecv, aIndex);
				}
				break;
			default:
				sLog->outBasic("UNKNOWN F1 PACKET: %x %x %x %x %x %x %x %x %x %x", aRecv[0], aRecv[1], aRecv[2], aRecv[3], aRecv[4], aRecv[5], aRecv[6], aRecv[7], aRecv[8], aRecv[9]);
				break;

			}
		}
		break;
		}
	}
}


void GameProtocol::SCPJoinResultSend(int uIndex, BYTE result)
{
	PMSG_JOINRESULT pResult;

	memset(&pResult, 0, sizeof(pResult));

	pResult.h.size = sizeof(pResult);
	pResult.h.c = 0xC1;
	pResult.h.headcode = 0xF1;
	pResult.scode = 0x00;
	pResult.result = result;
	pResult.NumberH = SET_NUMBERH(uIndex);
	pResult.NumberL = SET_NUMBERL(uIndex);
	pResult.CliVersion[0] = szClientVersion[0];
	pResult.CliVersion[1] = szClientVersion[1];
	pResult.CliVersion[2] = szClientVersion[2];
	pResult.CliVersion[3] = szClientVersion[3];
	pResult.CliVersion[4] = szClientVersion[4];

	GIOCP.DataSend(uIndex, (unsigned char*)&pResult, pResult.h.size);
	//gObj[aIndex].ConnectCheckTime = GetTickCount();
}


void GameProtocol::CGLiveClient(PMSG_CLIENTTIME * lpClientTime, short aIndex)
{

}




void GameProtocol::CSPJoinIdPassRequest(PMSG_IDPASS* lpMsg, int aIndex)
{
	char serial[17];
	char hwid[25];
	char* tAccountID;
	char* tPass;
	char* tHWID;

	STR_CS_USER* lpUser = getCSUser(aIndex);

	if (lpMsg->CliVersion[0] != szClientVersion[0] ||
		lpMsg->CliVersion[1] != szClientVersion[1] ||
		lpMsg->CliVersion[2] != szClientVersion[2] ||
		lpMsg->CliVersion[3] != szClientVersion[3] ||
		lpMsg->CliVersion[4] != szClientVersion[4])
	{
		GCJoinResult(JS_BAD_CLIENT_VERSION, aIndex);
		GIOCP.CloseClient(aIndex);
		return;
	}

	//if (lpMsg->ServerSeason != g_AuthSys.GetSeason())
	//{
	//	sLog->outError("[IP: %s][aIndex: %d] connecting with DLL for different Game Season, review and correct (if required) %d", lpUser->IP, aIndex, lpMsg->ServerSeason);
	//}
	tAccountID = lpMsg->Id;
	tPass = lpMsg->Pass;

//#ifdef EMU_NOCRYPT
	BuxConvert(tAccountID, MAX_ACCOUNT_LEN);
	BuxConvert(tPass, 20);
//#endif
	
	ProcessClientHWID(lpMsg->HWID, hwid);

	if (PercentSyntaxCheck(tAccountID) == FALSE)
	{
		this->GCJoinResult(JS_BAD_CLIENT_VERSION, aIndex);
		GIOCP.CloseClient(aIndex);
		return;
	}

	serial[16] = 0;
	memcpy(serial, lpMsg->CliSerial, sizeof(lpMsg->CliSerial));
	tHWID = serial;

	/*if (strcmp(serial, szGameServerExeSerial) != 0)
	{
	this->GCJoinResult(JS_BAD_CLIENT_VERSION, aIndex);
	GIOCP.CloseClient(aIndex);
	return;
	}*/

	if (tHWID == NULL)
	{
		GIOCP.CloseClient(aIndex);
		return;
	}

	/*if (g_ConfigRead.server.IsConnectMemberUse() == TRUE)
	{
	if (ConMember.IsMember(id) == FALSE)
	{
	this->GCJoinResult(JS_ONLY_VIP_MEMBERS, aIndex);
	return;
	}
	}

	if (g_ConfigRead.server.IsTestServer() == TRUE)
	{
	if (ConMember.IsMember(id) == FALSE)
	{
	this->GCJoinResult(2, aIndex);
	this->GCServerMsgStringSend(Lang.GetText(0, 518), aIndex, 0);
	return;
	}
	}*/

	//CGameObject* lpObj = new CGameObject();

	/*if (PacketCheckTime(aIndex) == FALSE)
	{
	GIOCP.CloseClient(aIndex);
	return;
	}*/

	if (lpUser->Connected != PLAYER_CONNECTED)
	{
		GIOCP.CloseClient(aIndex);
		return;
	}

	//SDHP_IDPASS spMsg;
	//PHeadSetB((LPBYTE)&spMsg, 0x01, sizeof(spMsg));
	//spMsg.Number = aIndex;
	//memcpy(spMsg.Id, lpMsg->Id, sizeof(spMsg.Id));
	//memcpy(spMsg.Pass, lpMsg->Pass, sizeof(spMsg.Pass));
	//memcpy(spMsg.HWID, hwid, sizeof(hwid));
	//strcpy(spMsg.IpAddress, lpUser->IP);



	//wsJServerCli.DataSend((char*)&spMsg, spMsg.h.size);
	// Prevent SQLInjection
	if (SQLSyntexCheck(tAccountID) == FALSE ||
		SQLSyntexCheck(tPass) == FALSE)
	{
		return;
	}

	if (SpaceSyntexCheck(tAccountID) == FALSE ||
		SpaceSyntexCheck(tPass) == FALSE)
	{
		return;
	}

	if (QuoteSpaceSyntaxCheck(tAccountID) == FALSE ||
		QuoteSpaceSyntaxCheck(tPass) == FALSE)
	{
		return;
	}

	if (PercentSyntaxCheck(tAccountID) == FALSE ||
		PercentSyntaxCheck(tPass) == FALSE)
	{
		return;
	}

	BOOL bErrorFlag = FALSE;
	int BlocCode = 0;
	int tResult = 0;
	//EnterCriticalSection(&this->userCheck);

	if (SpaceSyntexCheck(tAccountID) == TRUE)
	{
		// Add User to List
		if (bErrorFlag == FALSE)
		{
			QueryResult* res = this->m_AccDB.Fetch("SELECT memb_guid, bloc_code, memb__pwd FROM MEMB_INFO WHERE memb___id='%s'", tAccountID);
			if (res != nullptr)
			{
				if ((*res)->GetRowCount() == 0)
				{
					bErrorFlag = TRUE;
					sLog->outError("[MeMuOnline] Account doesn't exist - ID : %s", tAccountID);
				}
				if (bErrorFlag == FALSE)
				{
					//pResult.UserNumber = this->m_UserData.GetUserNumber();
					//this->m_UserData.IncUserNumber();
					//pResult.DBNumber = this->m_AccountDB.GetAsInteger(0);

					BlocCode = this->m_AccDB.GetAsInteger(1);

					/*if (g_PwEncrypt != PWENC_NONE)
					{

						this->m_AccountDB.Close();

						if (g_PwEncrypt == PWENC_WZMD5)
						{

							BYTE btBinaryPass[32] = { 0 };
							char szTemp[100];
							wsprintf(szTemp, "SELECT memb__pwd from MEMB_INFO where memb___id='%s'", szAccountID);

							if (this->m_AccountDB.GetAsBinary(szTemp, btBinaryPass, sizeof(btBinaryPass)) < 0)
							{
								bErrorFlag = TRUE;
								pResult.result = 0;
								g_Log.Add("[MeMuOnlineDB] Password field is blank - ID : %s", szAccountID);
							}

							DWORD dwAccKey = this->m_UserData.MakeAccountKey(szAccountID);

							WZMD5 pMD5Hash;

							if (pMD5Hash.WZMD5_CheckValue(szPass, (char*)btBinaryPass, dwAccKey) == false)
							{
								pResult.result = 0;
								bErrorFlag = TRUE;
								g_Log.Add("[MeMuOnline] Wrong Password - ID : %s", szAccountID);
							}
						}
						else if (g_PwEncrypt == PWENC_MD5)
						{
							this->m_AccountDB.Close();

							char szTemp[100];
							char szPassword[33] = { 0 };

							wsprintf(szTemp, "SELECT memb__pwd from MEMB_INFO where memb___id='%s'", szAccountID);

							this->m_AccountDB.GetAsString("memb__pwd", szPassword, sizeof(szPassword) - 1);

							if (strlen(szPassword) == 0)
							{
								bErrorFlag = TRUE;
								pResult.result = 0;
								g_Log.Add("[MeMuOnlineDB] Password field is blank - ID : %s", szAccountID);
							}

							if (md5(std::string(szPass)).compare(std::string(szPassword)) != 0)
							{
								pResult.result = 0;
								bErrorFlag = TRUE;
								g_Log.Add("[MeMuOnline] Wrong Password - ID : %s", szAccountID);
							}
						}
					}
					else*/
					{
						TCHAR tPassword2[21] = { 0 };
						this->m_AccDB.GetAsString(2, tPassword2, sizeof(tPassword2) - 1);

						if (lstrcmp(tPass, tPassword2))
						{
							bErrorFlag = TRUE;
							sLog->outError("[MeMuOnline] Wrong Password - ID : %s", tAccountID);
						}
					}
				}


				/*if (bErrorFlag == FALSE && this->m_ServerData.GetVIPLevel(this->m_ServerData.MuLoginFindServer(g_Server[aIndex].m_ServerCode)) != 0)
				{
					this->m_AccountDB.ExecQuery("EXEC VIPSystem_CheckAccount '%s'", szAccountID);
					this->m_AccountDB.Fetch();

					int VIP = this->m_AccountDB.GetAsInteger("Result");
					int Type = this->m_AccountDB.GetAsInteger("Type");
					this->m_AccountDB.Close();

					if (VIP == 0)
					{
						pResult.result = 64;
						bErrorFlag = TRUE;
						g_Log.Add("[MeMuOnline] Account is not a VIP - ID : %s", szAccountID);
					}

					else if (Type < this->m_ServerData.GetVIPLevel(this->m_ServerData.MuLoginFindServer(g_Server[aIndex].m_ServerCode)))
					{
						pResult.result = 64;
						bErrorFlag = TRUE;
						g_Log.Add("[MeMuOnline] Account has too low VIP level - ID : %s", szAccountID);
					}

				}*/

				/*if (bErrorFlag == FALSE)
				{
					this->m_AccountDB.ExecQuery("SELECT * FROM IGC_MachineID_Banned WHERE HWID = '%s'", aRecv->HWID);
					BOOL btResult = this->m_AccountDB.Fetch();

					if (btResult == SQL_SUCCESS)
					{
						pResult.result = 5;
						bErrorFlag = TRUE;
						g_Log.Add("[MeMuOnline] MachineID banned: %s", aRecv->HWID);
					}

					this->m_AccountDB.Close();
				}*/
			}
		}

		if (BlocCode > 0)
		{
			bErrorFlag = TRUE;

			if (BlocCode == 0x46)
				tResult = 15;
			else if (BlocCode == 0x41)
				tResult = 14;
			else if (BlocCode == 0x49)
				tResult = 17;
			else
				tResult = 5;
		}

		/*if (bErrorFlag == FALSE && tResult != 0)
		{
			int Index = this->m_UserData.MuLoginFindUser(szAccountID);

			if (Index != -1)
			{
				if (this->m_UserData.CheckOffTrade(Index) == true)
				{
					this->GCUserKillSend(Index, true);
				}
			}

			if (this->m_UserData.CheckAccountID(szAccountID) == TRUE && this->m_UserData.CheckMoveTimeOut(szAccountID) == FALSE)
			{
				g_Log.Add("[MeMuOnline] Account already connected ID : %s", szAccountID);
				JGOtherJoin(aIndex, szAccountID);
				pResult.result = 3;
				bErrorFlag = TRUE;
			}

			if (this->m_UserData.CheckHWIDLimit_Local(g_Server[aIndex].m_ServerCode, aRecv->HWID, this->m_ServerData.GetHWIDUseCount(this->m_ServerData.MuLoginFindServer(g_Server[aIndex].m_ServerCode))) == FALSE)
			{
				g_Log.Add("[MeMuOnline] Machine ID Limit Reached (Local) (%d) ServerCode (%d) : Account:%s HWID:%s", this->m_ServerData.GetHWIDUseCount(this->m_ServerData.MuLoginFindServer(g_Server[aIndex].m_ServerCode)), g_Server[aIndex].m_ServerCode, szAccountID, aRecv->HWID);
				JGOtherJoin(aIndex, szAccountID);
				pResult.result = 4;
				bErrorFlag = TRUE;
			}

			if (this->m_UserData.CheckHWIDLimit_Group(g_MapServerManager.GetMapSvrGroup(g_Server[aIndex].m_ServerCode), aRecv->HWID) == FALSE)
			{
				g_Log.Add("[MeMuOnline] Machine ID Limit Reached (Group) (%d) MapSvrGroup (%d) : Account:%s HWID:%s", g_MachineIDConnectionLimitPerGroup, g_MapServerManager.GetMapSvrGroup(g_Server[aIndex].m_ServerCode), szAccountID, aRecv->HWID);
				JGOtherJoin(aIndex, szAccountID);
				pResult.result = 4;
				bErrorFlag = TRUE;
			}
		}

		if (bErrorFlag == FALSE)
		{
			BOOL bResult = this->m_UserData.MuLoginAddUser(g_Server[aIndex].m_ServerCode, g_MapServerManager.GetMapSvrGroup(g_Server[aIndex].m_ServerCode), szAccountID, aRecv->Number, aRecv->IpAddress, aRecv->HWID);

			if (bResult == FALSE)
			{
				pResult.result = 4;
			}

			else if (bResult == 2)
			{
				pResult.result = 3;
			}

			else
			{
				InsertDataMuLog(this->m_ServerData.GetServerName(this->m_ServerData.MuLoginFindServer(g_Server[aIndex].m_ServerCode)), szAccountID, aRecv->IpAddress, "Connect", aRecv->HWID);
				this->m_AccountDB.ExecQuery("EXEC WZ_CONNECT_MEMB '%s','%s','%s'",
					szAccountID, this->m_ServerData.GetServerName(this->m_ServerData.MuLoginFindServer(g_Server[aIndex].m_ServerCode)), szIp);
			}
		}*/
	}

	//LeaveCriticalSection(&userCheck);

	GSProtocol.GCJoinResult(1, aIndex);

	std::strcpy(lpUser->AccountID,lpMsg->Id);
	std::strcpy(lpUser->Password,lpMsg->Pass);
	std::strcpy(lpUser->HWID,lpMsg->HWID);

	CSPJoinIdPassRequest2(lpMsg, aIndex);

}



void GameProtocol::CSPJoinIdPassRequest2(PMSG_IDPASS* lpMsg, int aIndex)
{
	STR_CS_USER* lpUserCS = getCSUser(aIndex);
	CUserData* lpUser = new CUserData();
	lpUser->Init();
	lpUser->ConnectUser = lpUserCS;
	insertUserObject(lpUser);

	lpUserCS->CheckTick = WorldTimer::getMSTime();
	lpUserCS->CheckTick2 = WorldTimer::getMSTime();
	lpUserCS->ConnectCheckTime = WorldTimer::getMSTime();
	lpUserCS->CheckSpeedHack = true;
	lpUserCS->AgiCheckTime = WorldTimer::getMSTime();
	lpUserCS->LoginMsgSnd = 1;
	lpUserCS->LoginMsgCount++;
	lpUserCS->m_cAccountItemBlock = 0;
	lpUserCS->VipType = 0;
	lpUserCS->VipEffect = VIP_EFFECT_NONE;
	lpUserCS->AntiHackCheckTime = WorldTimer::getMSTime();
	lpUserCS->dwLastHitHackTick = WorldTimer::getMSTime();


	QueryResult* result = this->m_AccDB.Fetch("SELECT `Id` FROM AccountCharacter WHERE `Id`='%s'", lpUserCS->AccountID);
	if (!result || (*result)->GetRowCount() == 0)
	{
		this->m_AccDB.ExecQuery("INSERT INTO AccountCharacter (`Id`) VALUES ('%s')", lpUserCS->AccountID);
		return;
	}

	QueryResult* result2 = this->m_AccDB.Fetch("SELECT MoveCnt,GameID1,GameID2,GameID3,GameID4,GameID5 FROM AccountCharacter WHERE Id='%s'", lpUserCS->AccountID);

	TCHAR CharName[5][11] = { 0 };
	memset(CharName, 0x00, sizeof(CharName));

	//pCount->MoveCnt = (BYTE)this->m_AccDB.GetAsInteger(0);

	this->m_AccDB.GetAsString(1, lpUser->Characters[0].Name, MAX_ACCOUNT_LEN);
	this->m_AccDB.GetAsString(2, lpUser->Characters[1].Name, MAX_ACCOUNT_LEN);
	this->m_AccDB.GetAsString(3, lpUser->Characters[2].Name, MAX_ACCOUNT_LEN);
	this->m_AccDB.GetAsString(4, lpUser->Characters[3].Name, MAX_ACCOUNT_LEN);
	this->m_AccDB.GetAsString(5, lpUser->Characters[4].Name, MAX_ACCOUNT_LEN);

	sLog->outBasic("[%s] - characters: [%s][%s][%s][%s][%s]", lpUserCS->AccountID, lpUser->Characters[0].Name,
		lpUser->Characters[1].Name, lpUser->Characters[2].Name, lpUser->Characters[3].Name, lpUser->Characters[4].Name);

	QueryResult* result3 = this->m_AccDB.Fetch("SELECT WarehouseExpansion, Summoner, RageFighter, SecCode FROM AccountCharacter WHERE Id='%s'", lpUserCS->AccountID);

	lpUser->m_WarehouseExpansion = this->m_AccDB.GetAsInteger(0);
	lpUser->m_iSecurityCode = this->m_AccDB.GetAsInteger(3);
	//lpUser->CharacterSlotCount = this->m_AccDB.GetAsInteger(4);

	BOOL Summoner = this->m_AccDB.GetAsInteger(1);
	BOOL RF = this->m_AccDB.GetAsInteger(2);

	if (Summoner == TRUE)
		lpUserCS->EnableCharacterCreate |= 1;
	if (RF == TRUE)
		lpUserCS->EnableCharacterCreate |= 8;

	sLog->outBasic("[%s] - Account Data: [SU:%d][RF:%d][Expanded WH:%d]", lpUserCS->AccountID, Summoner, RF, lpUser->m_WarehouseExpansion);

	int iCharCount = 0;

	for (int i = 0; i<5; i++)
	{
		char* charName = lpUser->Characters[i].Name;
		if (charName != 0)
		{
			if (lstrlen(charName) >= 4)
			{

				QueryResult* result4 = this->m_AccDB.Fetch("SELECT `Name`,cLevel,`Class`,CtlCode, `RESETS` FROM `character` WHERE `Name` = '%s'", charName);
				if (result4 && (*result4)->GetRowCount() == 1)
				{
					char szTemp[200];

					lpUser->Characters[i].Level = (WORD)this->m_AccDB.GetAsInteger(1);
					lpUser->Characters[i].Class = (BYTE)this->m_AccDB.GetAsInteger(2);
					lpUser->Characters[i].CtlCode = (BYTE)this->m_AccDB.GetAsInteger(3);
					lpUser->Characters[i].Resets = (WORD)this->m_AccDB.GetAsInteger(4);

					if (lpUser->Characters[i].Level >= g_MagumsaCreateMinLevel && (lpUserCS->EnableCharacterCreate & 4) != 4)
						lpUserCS->EnableCharacterCreate |= 4;

					if (lpUser->Characters[i].Level >= g_DarkLordCreateMinLevel && (lpUserCS->EnableCharacterCreate & 2) != 2)
						lpUserCS->EnableCharacterCreate |= 2;

					if (lpUser->Characters[i].Level >= g_GrowLancerCreateMinLevel && (lpUserCS->EnableCharacterCreate & 0x10) != 0x10)
						lpUserCS->EnableCharacterCreate |= 0x10;
					

					BYTE btInvetory[INVENTORY_BINARY_SIZE];
					wsprintf(szTemp, "SELECT Inventory FROM `character` WHERE `Name` = '%s'", charName);
					int ret = this->m_AccDB.GetAsBinary(szTemp, lpUser->Characters[i].Inventory, sizeof(lpUser->Characters[i].Inventory));

					int dbsize = 32;

					//memset(pCL->dbInventory, -1, sizeof(pCL->dbInventory));
					
					// Not sure needed.
					for (int j = 0; j<12; j++)
					{
						if (lpUser->Characters[i].Inventory[0 + j * dbsize] == 0xFF && (lpUser->Characters[i].Inventory[7 + j * dbsize] & 0x80) == 0x80 && (lpUser->Characters[i].Inventory[10 + j * dbsize] & 0xF0) == 0xF0)
						{
							lpUser->Characters[i].dbInventory[j * 4] = -1;
							lpUser->Characters[i].dbInventory[j * 4 + 1] = -1;
							lpUser->Characters[i].dbInventory[j * 4 + 2] = -1;
							lpUser->Characters[i].dbInventory[j * 4 + 3] = -1;
						}
						else
						{
							lpUser->Characters[i].dbInventory[j * 4] = (lpUser->Characters[i].Inventory[0 + j * dbsize]);	// 0..7 bits of Item
							lpUser->Characters[i].dbInventory[j * 4 + 1] = (lpUser->Characters[i].Inventory[1 + j * dbsize]);	// Get Level of Item
							lpUser->Characters[i].dbInventory[j * 4 + 2] = (lpUser->Characters[i].Inventory[7 + j * dbsize]);	// 8 bit     of Item
							lpUser->Characters[i].dbInventory[j * 4 + 3] = (lpUser->Characters[i].Inventory[9 + j * dbsize]);	// 9..12 bit of Item
						}
					}

					// Resets obtained earlier (Maybe Remove?)
					/*QueryResult* result5 = this->m_AccDB.Fetch("SELECT RESETS FROM Character WHERE Name='%s'", CharName[i]);

					if ((*result5)->GetRowCount() == 1)
					{
						pCL->Resets = (WORD)this->m_AccDB.GetAsInteger(0);
					}*/

					/*if (aRecv->IsUnityBattleFieldServer == FALSE)
					{
					QueryResult* result6 = this->m_AccDB.ExecQuery("SELECT G_Status FROM GuildMember WHERE Name='%s'", CharName[i]);

					if ((*result6)->GetRowCount() == 1)
					{
					pCL->btGuildStatus = (BYTE)this->m_AccDB.GetAsInteger(0);
					}

					else
					{
					pCL->btGuildStatus = 0xFF;
					}
					}

					else
					{
					pCL->btGuildStatus = 0xFF;
					}*/

					/*if (aRecv->IsUnityBattleFieldServer == TRUE)
					{
					QueryResult* res = this->m_BattleCoreDB.Fetch("CALL IGC_BattleCore_GetRealName('%s');", CharName[i]);


					this->m_BattleCoreDB.GetAsString(0, pCL->UnityBFRealName, MAX_ACCOUNT_LEN + 1);
					pCL->ServerCodeOfHomeWorld = this->m_BattleCoreDB.GetAsInteger(1);
					}
					else
					{
					strcpy(pCL->UnityBFRealName, CharName[i]);
					pCL->ServerCodeOfHomeWorld = 0;
					}*/
					iCharCount++;
				}
			}
		}
	}
	lpUser->CharacterSlotCount = iCharCount;
	/*if (g_MagumsaCreateMinLevel == 0 && (pCount->EnableCharacterCreate & 4) != 4)
	{
	pCount->EnableCharacterCreate |= 4;
	}

	if (g_DarkLordCreateMinLevel == 0 && (pCount->EnableCharacterCreate & 2) != 2)
	{
	pCount->EnableCharacterCreate |= 2;
	}

	if (g_GrowLancerCreateMinLevel == 0 && (pCount->EnableCharacterCreate & 0x10) != 0x10)
	{
	pCount->EnableCharacterCreate |= 0x10;
	}*/

	sLog->outBasic("Connect Account: %s / MachineID: %s", lpUserCS->AccountID, lpMsg->HWID);
	lpUserCS->m_bMapSvrMoveReq = false;
	lpUserCS->m_bMapSvrMoveReq_2 = false;
	lpUserCS->m_sPrevMapSvrCode = -1;
	lpUserCS->m_sDestMapNumber = -1;
	//lpUser->m_btDestX = 0;
	//lpUser->m_btDestY = 0;

	JGPGetCharList(aIndex);
}


void GameProtocol::JGPGetCharList(int aIndex)
{
	STR_CS_USER* lpUserCS = getCSUser(aIndex);
	CUserData* lpUser = getUserObject(aIndex);

	//SDHP_CHARLISTCOUNT * lpCount = (SDHP_CHARLISTCOUNT *)lpRecv;
	//SDHP_CHARLIST * lpCL;
	//char szId[MAX_ACCOUNT_LEN + 1];
	BYTE sendbuf[256];
	int lOfs = 0;
	int lsOfs = sizeof(SDHP_CHARLISTCOUNT);
	BYTE index;
	PMSG_CHARLISTCOUNT pCLCount;	// Packet Char List Count
	PMSG_CHARLIST_S9 pCList;
	WORD TempInventory[12];

	PMSG_RESET_INFO_CHARLIST pRMsg = { -1 };
	pRMsg.h.c = 0xC1;
	pRMsg.h.headcode = 0xFA;
	pRMsg.h.subcode = 0x0A;
	pRMsg.h.size = sizeof(pRMsg);

	pCLCount.h.c = 0xC1;
	pCLCount.h.headcode = 0xF3;
	pCLCount.subcode = 0x00;//K2
	pCLCount.Count = lpUser->CharacterSlotCount;
	
	//szId[MAX_ACCOUNT_LEN] = 0;
	//memcpy(szId, lpCount->AccountId, MAX_ACCOUNT_LEN);

	/*if (::gObjIsAccontConnect(aIndex, szId) == FALSE)
	{
		g_Log.AddC(TColor::Red, "Requested character list doesn't match the user (%s)%s", gObj[aIndex].AccountID, szId);
		IOCP.CloseClient(aIndex);

		return;
	}*/


	if (lpUser->m_iSecurityCode != 0)
	{
		lpUser->m_bSecurityCheck = false;
		//GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 534), aIndex, 0); // TODO
	}
	else
	{
		lpUser->m_bSecurityCheck = true;
	}

	pCLCount.MaxClass = 0x1F;//lpCount->EnableCharacterCreate;
	pCLCount.MoveCnt = 0;
	pCLCount.WhExpansion = lpUser->m_WarehouseExpansion;
	pCLCount.CharacterSlotCount = lpUser->CharacterSlotCount;


	PMSG_CHARLIST_ENABLE_CREATION pMsgC;
	PHeadSubSetB((LPBYTE)&pMsgC, 0xDE, 0x00, sizeof(pMsgC));

	pMsgC.EnableClass = lpUserCS->EnableCharacterCreate;

	GIOCP.DataSend(aIndex, (UCHAR *)&pMsgC, sizeof(PMSG_CHARLIST_ENABLE_CREATION));

	memset(sendbuf, 0, sizeof(sendbuf));
	lOfs += sizeof(PMSG_CHARLISTCOUNT);

	if (lpUser->CharacterSlotCount > 0)
	{
		lpUser->m_NameConvertOfUBF.InitData(); // UBF Name Conversion

		for (int n = 0; n < 5; n++)
		{
			memset(&pCList, 0, sizeof(pCList));
			//lpCL = (SDHP_CHARLIST *)&lpRecv[lsOfs];
			pRMsg.Reset[n] = lpUser->Characters[n].Resets;
			pCList.Index = n;
			pCList.Level = lpUser->Characters[n].Level;
			pCList.CtlCode = lpUser->Characters[n].CtlCode;
			//pCList.btGuildStatus = lpUser->Characters[n].btGuildStatus;

			if (lpUserCS->m_cAccountItemBlock != 0)
				pCList.CtlCode |= 0x10;	// Set Block Item

			/*if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
			{
				std::string tmpKeyName;
				std::string tmpRealName;

				tmpKeyName.insert(0, lpCL->Name, MAX_ACCOUNT_LEN);
				tmpRealName.insert(0, lpCL->UnityBFRealName, MAX_ACCOUNT_LEN);

				lpUser->m_NameConvertOfUBF.InputData(n, tmpKeyName.c_str(), tmpRealName.c_str(), lpCL->ServerCodeOfHomeWorld, MAX_ACCOUNT_LEN);
				g_Log.Add("[UBF][ConvertInputData][%d][%s][KeyName: %s][RealName: %s] ServerCode: %d",
					aIndex, lpUser.AccountID, tmpKeyName.c_str(), tmpRealName.c_str(), lpCL->ServerCodeOfHomeWorld);

				memcpy(pCList.Name, lpCL->UnityBFRealName, MAX_ACCOUNT_LEN);
			}*/

			//else
			//{
				memcpy(pCList.Name, lpUser->Characters[n].Name, MAX_ACCOUNT_LEN);
			//}

			pCList.CharSet[CS_CLASS] = CS_GET_CLASS_SX(lpUser->Characters[n].Class);
			int changeup = CS_GET_CHANGEUP_SX(lpUser->Characters[n].Class);

			if (changeup == 1)
			{
				pCList.CharSet[CS_CLASS] |= CS_SET_CHANGEUP_SX;
			}

			if (changeup == 2)
			{
				pCList.CharSet[CS_CLASS] |= CS_SET_CHANGEUP_SX;
				pCList.CharSet[CS_CLASS] |= CS_SET_CHANGEUP2_SX; // wrong order
			}

			pCList.CharSet[9] = 0;
			//memcpy(lpUser.Name1[n], pCList.Name, MAX_ACCOUNT_LEN);

			if (lpUser->Characters[n].dbInventory[0] == (BYTE)-1 && (lpUser->Characters[n].dbInventory[2] & 0x80) == 0x80 && (lpUser->Characters[n].dbInventory[3] & 0xF0) == 0xF0)
			{
				TempInventory[0] = -1;
			}
			else
			{
				TempInventory[0] = (lpUser->Characters[n].dbInventory[0] + (lpUser->Characters[n].dbInventory[2] & 0x80) * 2) + (lpUser->Characters[n].dbInventory[3] & 0xF0) * 32;
			}

			if (lpUser->Characters[n].dbInventory[4] == (BYTE)-1 && (lpUser->Characters[n].dbInventory[6] & 0x80) == 0x80 && (lpUser->Characters[n].dbInventory[7] & 0xF0) == 0xF0)
			{
				TempInventory[1] = -1;
			}
			else
			{
				TempInventory[1] = (lpUser->Characters[n].dbInventory[4] + (lpUser->Characters[n].dbInventory[6] & 0x80) * 2) + (lpUser->Characters[n].dbInventory[7] & 0xF0) * 32;
			}

			if (lpUser->Characters[n].dbInventory[8] == (BYTE)-1 && (lpUser->Characters[n].dbInventory[10] & 0x80) == 0x80 && (lpUser->Characters[n].dbInventory[11] & 0xF0) == 0xF0)
			{
				TempInventory[2] = 0x1FF;
			}
			else
			{
				TempInventory[2] = ((lpUser->Characters[n].dbInventory[8] + (lpUser->Characters[n].dbInventory[10] & 0x80) * 2) + (lpUser->Characters[n].dbInventory[11] & 0xF0) * 32) % MAX_SUBTYPE_ITEMS;
			}

			if (lpUser->Characters[n].dbInventory[12] == (BYTE)-1 && (lpUser->Characters[n].dbInventory[14] & 0x80) == 0x80 && (lpUser->Characters[n].dbInventory[15] & 0xF0) == 0xF0)
			{
				TempInventory[3] = 0x1FF;
			}
			else
			{
				TempInventory[3] = ((lpUser->Characters[n].dbInventory[12] + (lpUser->Characters[n].dbInventory[14] & 0x80) * 2) + (lpUser->Characters[n].dbInventory[15] & 0xF0) * 32) % MAX_SUBTYPE_ITEMS;
			}

			if (lpUser->Characters[n].dbInventory[16] == (BYTE)-1 && (lpUser->Characters[n].dbInventory[18] & 0x80) == 0x80 && (lpUser->Characters[n].dbInventory[19] & 0xF0) == 0xF0)
			{
				TempInventory[4] = 0x1FF;
			}
			else
			{
				TempInventory[4] = ((lpUser->Characters[n].dbInventory[16] + (lpUser->Characters[n].dbInventory[18] & 0x80) * 2) + (lpUser->Characters[n].dbInventory[19] & 0xF0) * 32) % MAX_SUBTYPE_ITEMS;
			}

			if (lpUser->Characters[n].dbInventory[20] == (BYTE)-1 && (lpUser->Characters[n].dbInventory[22] & 0x80) == 0x80 && (lpUser->Characters[n].dbInventory[23] & 0xF0) == 0xF0)
			{
				TempInventory[5] = 0x1FF;
			}
			else
			{
				TempInventory[5] = ((lpUser->Characters[n].dbInventory[20] + (lpUser->Characters[n].dbInventory[22] & 0x80) * 2) + (lpUser->Characters[n].dbInventory[23] & 0xF0) * 32) % MAX_SUBTYPE_ITEMS;
			}

			if (lpUser->Characters[n].dbInventory[24] == (BYTE)-1 && (lpUser->Characters[n].dbInventory[26] & 0x80) == 0x80 && (lpUser->Characters[n].dbInventory[27] & 0xF0) == 0xF0)
			{
				TempInventory[6] = 0x1FF;
			}
			else
			{
				TempInventory[6] = ((lpUser->Characters[n].dbInventory[24] + (lpUser->Characters[n].dbInventory[26] & 0x80) * 2) + (lpUser->Characters[n].dbInventory[27] & 0xF0) * 32) % MAX_SUBTYPE_ITEMS;
			}

			if (lpUser->Characters[n].dbInventory[28] == (BYTE)-1 && (lpUser->Characters[n].dbInventory[30] & 0x80) == 0x80 && (lpUser->Characters[n].dbInventory[31] & 0xF0) == 0xF0)
			{
				TempInventory[7] = 0x1FF;
			}
			else
			{
				TempInventory[7] = ((lpUser->Characters[n].dbInventory[28] + (lpUser->Characters[n].dbInventory[30] & 0x80) * 2) + (lpUser->Characters[n].dbInventory[31] & 0xF0) * 32) % MAX_SUBTYPE_ITEMS;
			}

			if (lpUser->Characters[n].dbInventory[32] == (BYTE)-1 && (lpUser->Characters[n].dbInventory[34] & 0x80) == 0x80 && (lpUser->Characters[n].dbInventory[35] & 0xF0) == 0xF0)
			{
				TempInventory[8] = 0x1FF;
			}
			else
			{
				TempInventory[8] = ((lpUser->Characters[n].dbInventory[32] + (lpUser->Characters[n].dbInventory[34] & 0x80) * 2) + (lpUser->Characters[n].dbInventory[35] & 0xF0) * 32) % MAX_SUBTYPE_ITEMS;
			}

			pCList.CharSet[12] |= DBI_GET_TYPE(TempInventory[0]);
			pCList.CharSet[1] = TempInventory[0] % 256;

			pCList.CharSet[13] |= DBI_GET_TYPE(TempInventory[1]);
			pCList.CharSet[2] = TempInventory[1] % 256;

			pCList.CharSet[13] |= (int)(TempInventory[2] & 0x1E0) >> 5;
			pCList.CharSet[9] |= (int)(TempInventory[2] & 0x10) << 3;
			pCList.CharSet[3] |= (int)(TempInventory[2] & 0x0F) << 4;

			pCList.CharSet[14] |= (int)(TempInventory[3] & 0x1E0) >> 1;
			pCList.CharSet[9] |= (int)(TempInventory[3] & 0x10) << 2;
			pCList.CharSet[3] |= (int)(TempInventory[3] & 0x0F);

			pCList.CharSet[14] |= (int)(TempInventory[4] & 0x1E0) >> 5;
			pCList.CharSet[9] |= (int)(TempInventory[4] & 0x10) << 1;
			pCList.CharSet[4] |= (int)(TempInventory[4] & 0x0F) << 4;

			pCList.CharSet[15] |= (int)(TempInventory[5] & 0x1E0) >> 1;
			pCList.CharSet[9] |= (int)(TempInventory[5] & 0x10);
			pCList.CharSet[4] |= (int)(TempInventory[5] & 0x0F);

			pCList.CharSet[15] |= (int)(TempInventory[6] & 0x1E0) >> 5;
			pCList.CharSet[9] |= (int)(TempInventory[6] & 0x10) >> 1;
			pCList.CharSet[5] |= (int)(TempInventory[6] & 0x0F) << 4;

			index = 0;

			if ((TempInventory[8] & 0x04) == 0)
			{
				if (TempInventory[8] == 0x1FF)
				{
					index |= 0x03;
				}
				else
				{
					index |= (TempInventory[8]) & 0x03;
				}
			}
			else
			{
				index |= 0x03;
			}

			pCList.CharSet[5] |= index;

			int levelindex = 0;

			if (TempInventory[0] != (WORD)-1)
			{
				levelindex = LevelSmallConvert(DBI_GET_LEVEL(lpUser->Characters[n].dbInventory[1]));
			}

			if (TempInventory[1] != (WORD)-1)
			{
				levelindex |= (int)LevelSmallConvert(DBI_GET_LEVEL(lpUser->Characters[n].dbInventory[5])) << 3;
			}

			if (TempInventory[2] < 0x1FF)
			{
				levelindex |= (int)LevelSmallConvert(DBI_GET_LEVEL(lpUser->Characters[n].dbInventory[9])) << 6;
			}

			if (TempInventory[3] < 0x1FF)
			{
				levelindex |= (int)LevelSmallConvert(DBI_GET_LEVEL(lpUser->Characters[n].dbInventory[13])) << 9;
			}

			if (TempInventory[4] < 0x1FF)
			{
				levelindex |= (int)LevelSmallConvert(DBI_GET_LEVEL(lpUser->Characters[n].dbInventory[17])) << 12;
			}

			if (TempInventory[5] < 0x1FF)
			{
				levelindex |= (int)LevelSmallConvert(DBI_GET_LEVEL(lpUser->Characters[n].dbInventory[21])) << 15;
			}

			if (TempInventory[6] < 0x1FF)
			{
				levelindex |= (int)LevelSmallConvert(DBI_GET_LEVEL(lpUser->Characters[n].dbInventory[25])) << 18;
			}

			pCList.CharSet[6] = ((int)levelindex >> 0x10) & 0xFF;
			pCList.CharSet[7] = ((int)levelindex >> 0x08) & 0xFF;
			pCList.CharSet[8] = ((int)levelindex) & 0xFF;

			pCList.CharSet[10] = 0;

			if ((TempInventory[8] & 0x03) != 0 && TempInventory[8] != 0x1FF)
			{
				pCList.CharSet[10] |= 0x01;
			}

			pCList.CharSet[11] = 0;

			if ((TempInventory[8] & 0x04) != 0 && TempInventory[8] != 0x1FF)
			{
				pCList.CharSet[12] |= 0x01;
			}

			if ((TempInventory[8] & 0x05) != 0 && TempInventory[8] != 0x1FF)
			{
				pCList.CharSet[12] |= 0x01;
			}

			pCList.CharSet[16] = 0;
			pCList.CharSet[17] = 0;

			if (TempInventory[8] == 0x25 && TempInventory[8] != 0x1FF)
			{
				pCList.CharSet[10] &= 0xFE;
				pCList.CharSet[12] &= 0xFE;
				pCList.CharSet[12] |= 0x04;
				BYTE btExcellentOption = lpUser->Characters[n].dbInventory[34] & 0x3F;

				if ((btExcellentOption & 1) == 1)
				{
					pCList.CharSet[16] |= 0x01;
				}

				if ((btExcellentOption & 2) == 2)
				{
					pCList.CharSet[16] |= 0x02;
				}

				if ((btExcellentOption & 4) == 4) //Golden Fenrir
				{
					pCList.CharSet[17] |= 0x01;
				}
			}

			if (TempInventory[7] >= 0 && TempInventory[7] <= 6)
			{
				pCList.CharSet[16] |= (0x04 * (TempInventory[7] + 1));
			}

			else if (TempInventory[7] >= 36 && TempInventory[7] <= 43)
			{
				pCList.CharSet[9] |= 0x01;
				pCList.CharSet[16] |= (0x04 * (TempInventory[7] - 36));
			}

			else if ((TempInventory[7] >= 49 && TempInventory[7] <= 50) || (TempInventory[7] >= 130 && TempInventory[7] <= 135))
			{
				pCList.CharSet[9] |= 0x02;

				if (TempInventory[7] == 49 || TempInventory[7] == 50)
				{
					pCList.CharSet[16] |= (0x04 * (TempInventory[7] - 49));
				}

				else if (TempInventory[7] >= 130 && TempInventory[7] <= 135)
				{
					pCList.CharSet[16] |= (0x04 * (0x02 + (TempInventory[7] - 130)));
				}
			}

			else if (TempInventory[7] >= 262 && TempInventory[7] <= 269)
			{
				pCList.CharSet[9] |= 0x03;

				if (TempInventory[7] >= 262 && TempInventory[7] <= 265)
				{
					pCList.CharSet[16] |= (0x04 * (TempInventory[7] - 262));
				}

				if (TempInventory[7] == 266 || TempInventory[7] == 268)
				{
					pCList.CharSet[16] |= 0x10;
				}

				if (TempInventory[7] == 267)
				{
					pCList.CharSet[16] |= 0x14;
				}

				if (TempInventory[7] == 269)
				{
					pCList.CharSet[16] |= 0x1C;
				}
			}

			else if (TempInventory[7] == 30)
			{
				pCList.CharSet[9] |= 0x03;
				pCList.CharSet[16] |= 0x18;
			}

			else if (TempInventory[7] == 270)
			{
				pCList.CharSet[9] |= 0x04;
			}

			else if (TempInventory[7] == 278)
			{
				pCList.CharSet[9] |= 0x04;
				pCList.CharSet[16] |= 0x04;
			}

			int itemindex = TempInventory[8];
			switch (itemindex)
			{
			case 64:
				pCList.CharSet[16] |= 0x20;
				break;
			case 65:
				pCList.CharSet[16] |= 0x40;
				break;
			case 67:
				//pCList.CharSet[5] &= 0xFC;
				pCList.CharSet[10] |= 0x01;
				pCList.CharSet[16] |= 0x80;
				break;
			case 80:
				pCList.CharSet[16] |= 0xE0;
				break;
			case 106:
				pCList.CharSet[16] |= 0xA0;
				break;
			case 123:
				pCList.CharSet[16] |= 0x60;
				//pCList.CharSet[5] -= 0x03;
				break;
			default:
				break;
			}
			memcpy(&sendbuf[lOfs], &pCList, sizeof(pCList));
			lsOfs += sizeof(SDHP_CHARLIST);
			lOfs += sizeof(pCList);
		}
	}
	pCLCount.h.size = lOfs;
	memcpy(sendbuf, &pCLCount, sizeof(PMSG_CHARLISTCOUNT));
	GIOCP.DataSend(aIndex, (LPBYTE)&pRMsg, sizeof(pRMsg));
	GIOCP.DataSend(aIndex, sendbuf, lOfs);

	/*if (gObjUseSkill.m_SkillData.EnableSiegeOnAllMaps)
	{
		BYTE EnableSiege[4] = { 0xC1, 0x04, 0xFA, 0xA0 };
		IOCP.DataSend(aIndex, EnableSiege, sizeof(EnableSiege));
	}*/

	//g_GensSystem.SendBattleZoneData(&lpUser);
	GSProtocol.GCSetCharColors(aIndex);

	BYTE EnableDMG[4] = { 0xC1, 0x04, 0xFA, 0xA9 }; //DMG over 65k
	GIOCP.DataSend(aIndex, EnableDMG, sizeof(EnableDMG));

	BYTE DisableCheat[4] = { 0xC1, 0x04, 0xF3, 0xFA }; //Disable Cheat
	GIOCP.DataSend(aIndex, DisableCheat, sizeof(DisableCheat));
}


void GameProtocol::GCJoinResult(BYTE result, int aIndex)
{
	PMSG_RESULT  pResult;

	PHeadSubSetB((LPBYTE)&pResult, 0xF1, 0x01, sizeof(pResult));
	pResult.result = result;
	GIOCP.DataSend(aIndex, (UCHAR*)&pResult, pResult.h.size, false);
}



void GameProtocol::CGClientMsg(PMSG_CLIENTMSG* lpMsg, int aIndex)
{
	STR_CS_USER* lpUser = getCSUser(aIndex);
	if (lpMsg->Flag)
	{

	}
	else
	{
		if (lpUser->m_ClientHackLogCount < 100)
		{

		}

		lpUser->m_ClientHackLogCount++;

		if (lpUser->m_ClientHackLogCount >= 100)
			lpUser->m_ClientHackLogCount = 100;
	}

	char msg[255];

	wsprintf(msg, "Client HackCheck %d", lpMsg->Flag);
}


void GameProtocol::CGClientCloseMsg(PMSG_CLIENTCLOSE * lpMsg, int aIndex)
{
	STR_CS_USER* lpUser = getCSUser(aIndex);
	switch (lpMsg->Flag)
	{
	case 0:	// Close Game
		lpUser->m_bOff = 0;
		GIOCP.CloseClient(aIndex);
		break;

	case 2:	// ServerList
		lpUser->m_bOff = 0;
		GIOCP.CloseClient(aIndex);
		break;

	case 1: // Select Character
		lpUser->m_bOff = 0;
		GIOCP.CloseClient(aIndex);
		break;
	}
}


void GameProtocol::GCCloseMsgSend(int aIndex, BYTE result)
{
	PMSG_RESULT pMsg;

	PHeadSubSetBE((LPBYTE)&pMsg, 0xF1, 0x02, sizeof(pMsg));
	pMsg.result = result;

	GIOCP.DataSend(aIndex, (UCHAR*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCServerMsgStringSend(LPSTR  szMsg, int aIndex, BYTE type)
{
	PMSG_NOTICE pNotice;

	TNotice::MakeNoticeMsg((TNotice*)&pNotice, type, szMsg);
	GIOCP.DataSend(aIndex, (UCHAR*)&pNotice, pNotice.h.size);
}

void GameProtocol::GCServerMsgStringSendAll(LPSTR  szMsg, BYTE type)
{
	PMSG_NOTICE pNotice;

	TNotice::MakeNoticeMsg((TNotice*)&pNotice, type, szMsg);
	//this->DataSendAll((UCHAR*)&pNotice, pNotice.h.size); // TODO
}

void GameProtocol::GCServerMsgStringSendAllEx(BYTE type, LPSTR szMsg, ...)
{
	va_list pArguments;
	char szBuffer[512];

	va_start(pArguments, szMsg);
	vsprintf(szBuffer, szMsg, pArguments);

	//this->GCServerMsgStringSendAll(szBuffer, type);
}

void GameProtocol::GCServerMsgStringSendEx(int aIndex, BYTE type, LPSTR szMsg, ...)
{
	va_list pArguments;
	char szBuffer[512];

	va_start(pArguments, szMsg);
	vsprintf(szBuffer, szMsg, pArguments);

	this->GCServerMsgStringSend(szBuffer, aIndex, type);
}

void GameProtocol::GCSetCharColors(int aIndex)
{
	PMSG_SET_CHAT_COLOR pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xFA, 0x12, sizeof(pMsg));
	/*
	memcpy(pMsg.btInfoMsg, g_ConfigRead.data.chatcolor.btInfoMsg, sizeof(pMsg.btInfoMsg));
	memcpy(pMsg.btErrorMsg, g_ConfigRead.data.chatcolor.btErrorMsg, sizeof(pMsg.btErrorMsg));
	memcpy(pMsg.btChatMsg, g_ConfigRead.data.chatcolor.btChatMsg, sizeof(pMsg.btChatMsg));
	memcpy(pMsg.btWhisperMsg, g_ConfigRead.data.chatcolor.btWhisperMsg, sizeof(pMsg.btWhisperMsg));
	memcpy(pMsg.btPartyMsg, g_ConfigRead.data.chatcolor.btPartyMsg, sizeof(pMsg.btPartyMsg));
	memcpy(pMsg.btGuildMsg, g_ConfigRead.data.chatcolor.btGuildMsg, sizeof(pMsg.btGuildMsg));
	memcpy(pMsg.btAllianceMsg, g_ConfigRead.data.chatcolor.btAllianceMsg, sizeof(pMsg.btAllianceMsg));
	memcpy(pMsg.btGensMsg, g_ConfigRead.data.chatcolor.btGensMsg, sizeof(pMsg.btGensMsg));
	memcpy(pMsg.btGMChatMsg, g_ConfigRead.data.chatcolor.btGMChatMsg, sizeof(pMsg.btGMChatMsg));
	*/
	memcpy(pMsg.btInfoMsg, "1", sizeof(pMsg.btInfoMsg));
	memcpy(pMsg.btErrorMsg, "1", sizeof(pMsg.btErrorMsg));
	memcpy(pMsg.btChatMsg, "1", sizeof(pMsg.btChatMsg));
	memcpy(pMsg.btWhisperMsg, "1", sizeof(pMsg.btWhisperMsg));
	memcpy(pMsg.btPartyMsg, "1", sizeof(pMsg.btPartyMsg));
	memcpy(pMsg.btGuildMsg, "1", sizeof(pMsg.btGuildMsg));
	memcpy(pMsg.btAllianceMsg, "1", sizeof(pMsg.btAllianceMsg));
	memcpy(pMsg.btGensMsg, "1", sizeof(pMsg.btGensMsg));
	memcpy(pMsg.btGMChatMsg, "1", sizeof(pMsg.btGMChatMsg));

	GIOCP.DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
}




