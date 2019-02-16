#include "StdAfx.h"
#include "GameProtocol.h"
#include "Event.h"
#include "gameGlobals.h"
#include "TNotice.h"
#include "QuestInfo.h"
#include "ConMember.h"
#include "GameServer.h"
#include "GMMng.h"
#include "BattleSoccerManager.h"
#include "BuffEffectSlot.h"
#include "ChaosBox.h"
#include "CastleSiegeSync.h"
#include "TUnion.h"
#include "ArcaBattle.h"
#include "JewelMixSystem.h"
#include "Weapon.h"
#include "ImperialGuardian.h"
#include "MasterLevelSkillTreeSystem.h"
#include "KalimaGate.h"
#include "ObjUseSkill.h"
#include "ObjCalCharacter.h"
#include "NpcTalk.h"
#include "Shop.h"
#include "PeriodItemEx.h"
#include "BattleSoccer.h"
#include "BattleSoccerManager.h"
#include "gObjMonster.h"
#include "ComboAttack.h"
#include "Gate.h"
#include "MoveCommand.h"
#include "MultiAttackHackCheck.h"
#include "ItemAddOption.h"
#include "JewelOfHarmonySystem.h"
#include "PacketCheckSum.h"
#include "DarkSpirit.h"
#include "Crywolf.h"
#include "CrywolfAltar.h"
#include "CrywolfSync.h"
#include "DevilSquare.h"
#include "KanturuEntranceNPC.h"
#include "CashShop.h"
#include "PentagramSystem.h"
#include "IllusionTempleEvent_Renewal.h"
#include "Sprotocol.h"
#include "Mercenary.h"
#include "ItemSocketOptionSystem.h"
#include "Guardian.h"
#include "DoppelGanger.h"
#include "AntiSwear.h"
#include "ChaosCastle.h"
#include "ChaosCastleFinal.h"
#include "GamblingSystem.h"
#include "configread.h"
//#include "bcore.h"
#include "GensSystem.h"
#include "GensSystemProtocol.h"
#include "NewPVP.h"
#include "MapServerManager.h"
#include "MineSystem.h"
#include "VipSys.h"
#include "LifeStone.h"
#include "QuestExpProtocol.h"
#include "QuestExpProgMng.h"
#include "QuestExpInfo.h"
#include "QuestExpLuaBind.h"
#include "LuaFun.h"
#include "BagManager.h"
#include "SetItemOption.h"
#include "LuckyItemManager.h"
#include "PentagramSystem.h"
#include "PentagramMixSystem.h"
#include "ProhibitedSymbols.h"
#include "Authorization.h"
#include "PersonalStore.h"
#include "MuunSystem.h"
#include "MuRummyMng.h"
#include "KeyGenerater.h"
#include "AcheronGuardianEvent.h"
#include "GremoryCase.h"
#include "DevilSquareFinal.h"
#include "BotSystem.h"
#include "ExpManager.h"
#include "EvolutionMonsterMng.h"
#include "RuudStore.h"
#include "UserChatBlockMng.hpp"
#include "CustomMaxStats.h"
#include "ItemUpgradeJewel.h"
#include "SocketItemType.h"
#include "CustomMichi.h"
#include "NewPVP.h"
#include "GOEventFunctions.h"

#include <string>

GameProtocol gGameProtocol;

GameProtocol::GameProtocol()
{
}

GameProtocol::~GameProtocol()
{
}

bool GameProtocol::PacketCheckTime(CGameObject &Obj)
{
	if ((GetTickCount() - Obj.m_PacketCheckTime) < 300)
	{
		return false;
	}

	Obj.m_PacketCheckTime = GetTickCount();
	return true;
}

bool GameProtocol::DataEncryptCheck(CGameObject &Obj, BYTE protoNum, BOOL Encrypt)
{
#ifndef EMU_NOCRYPT
	if (Encrypt == FALSE)
	{
		sLog->outBasic("Error-L1 : Not Encrypt %s %d", Obj.AccountID, protoNum);
		//this->GCSendDisableReconnect(Obj);
		IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
		return false;
	}
#endif
	return true;
}


void GameProtocol::ProtocolCore(BYTE protoNum, BYTE* aRecv, int aLen, CGameObject &Obj, BOOL Encrypt)
{
}

void GameProtocol::MsgSendV2(CGameObject &Obj, BYTE* Msg, int size)
{
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (Obj.VpPlayer2[n].type == OBJ_USER)
		{
			if (Obj.VpPlayer2[n].state != 0)
			{
				IOCP.DataSend(getGameObject(Obj.VpPlayer2[n].number)->m_PlayerData->ConnectUser->Index, Msg, size);
			}
		}
	}
}

void GameProtocol::CGLiveClient(PMSG_CLIENTTIME * lpClientTime, CGameObject &Obj)
{
#ifdef GS_AUTHORITY
	BYTE Decrypted[200];
	std::memcpy(Decrypted, lpClientTime, sizeof(PMSG_CLIENTTIME));
	//StringNarrow
	WLBufferDecrypt(&Decrypted[3], lpClientTime->h.size - 3, "12094389340236896570890436753");
	lpClientTime = (PMSG_CLIENTTIME *)Decrypted;
#endif

	if (Obj.Connected > PLAYER_CONNECTED)
	{
		Obj.ConnectCheckTime = GetTickCount();
	}

#ifndef GS_AUTHORITY
	return;
#endif

	if (Obj.CheckSpeedHack != false)
	{
		DWORD dwTime = MAKE_NUMBERDW(lpClientTime->TimeH, lpClientTime->TimeL);

		int systemtick = GetTickCount() - Obj.CheckTick2;
		int usertick = dwTime - Obj.CheckTick;
		int checktime = systemtick - usertick;

		Obj.CheckTickCount++;

		if (Obj.CheckTickCount > 45)
		{
			Obj.CheckTick = dwTime;
			Obj.CheckTickCount = 0;
		}
	}

	if (Obj.Connected == PLAYER_PLAYING && g_ConfigRead.m_CheckSpeedHack != 0 && lpClientTime->AttackSpeed > Obj.m_AttackSpeed && abs((int)(Obj.m_AttackSpeed - lpClientTime->AttackSpeed)) > g_ConfigRead.m_CheckSpeedHackTolerance)
	{
		sLog->outError( "[%s][%s] Client Speed Hack Detected [%d][%d]", Obj.AccountID, Obj.Name, Obj.m_AttackSpeed, lpClientTime->AttackSpeed);
		IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
		return;
	}

	if (Obj.Connected == PLAYER_PLAYING && g_ConfigRead.m_CheckSpeedHack != 0 && lpClientTime->MagicSpeed > Obj.m_MagicSpeed && abs((int)(Obj.m_MagicSpeed - lpClientTime->MagicSpeed)) > g_ConfigRead.m_CheckSpeedHackTolerance)
	{
		sLog->outError( "[%s][%s] Client Speed Hack Detected [%d][%d]", Obj.AccountID, Obj.Name, Obj.m_MagicSpeed, lpClientTime->MagicSpeed);
		IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
		return;
	}

	/*if (Obj.Connected == PLAYER_PLAYING)
	{
		if (g_ConfigRead.m_CheckSpeedHack == 1)
		{
			if ((Obj.m_AttackSpeed + g_ConfigRead.m_CheckSpeedHackTolerance) < lpClientTime->AttackSpeed || (Obj.m_MagicSpeed + g_ConfigRead.m_CheckSpeedHackTolerance) < lpClientTime->MagicSpeed)
			{
				sLog->outError( "[%s][%s][%s] ClientSpeedHack Detected : Editing AttackSpeed [%d][%d] [%d][%d]", Obj.AccountID, Obj.Name, Obj.m_PlayerData->ConnectUser->IP, Obj.m_AttackSpeed, lpClientTime->AttackSpeed, Obj.m_MagicSpeed, lpClientTime->MagicSpeed);

				Obj.m_AttackSpeedHackDetectedCount++;

				if (Obj.m_AttackSpeedHackDetectedCount >= g_ConfigRead.m_CheckSpeedHackLimit)
				{
					IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
				}
			}
			else
			{
				Obj.m_AttackSpeedHackDetectedCount = 0;
			}
		}
	}*/
}




void GameProtocol::GCCheckMainExeKeySend(CGameObject &Obj)
{
	PMSG_CHECK_MAINEXE pMsg;

	Obj.CheckSumTableNum = rand() % MAX_CHECKSUM_KEY;
	Obj.CheckSumTime = GetTickCount();
	PHeadSetB((BYTE*)&pMsg, 0x03, sizeof(pMsg));
	pMsg.m_wKey = EncryptCheckSumKey(Obj.CheckSumTableNum);

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}





WORD GameProtocol::EncryptCheckSumKey(WORD wSource)
{
	WORD wRandom = rand() % 64;
	WORD wAcc = ((wSource & 0x3F0) * 64) | (wRandom * 16) | (wSource & 0x0F);
	return wAcc ^ 0xB479;
}




void GameProtocol::CGCheckMainRecv(PMSG_CHECK_MAINEXE_RESULT * lpMsg, CGameObject &Obj)
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (!gCheckSumSwitch)
	{
		Obj.CheckSumTime = 0;
		return;
	}

	if (Obj.CheckSumTableNum < 0 || Obj.CheckSumTableNum > MAX_CHECKSUM_KEY - 1)
	{
		sLog->outBasic("error-L1 : [%s][%s] CheckSum-Exe error", Obj.AccountID, Obj.Name);
		this->GCSendDisableReconnect(Obj);
		//IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
		return;
	}

	if (dwgCheckSum[Obj.CheckSumTableNum] != lpMsg->m_dwKey)
	{
		sLog->outBasic("error-L1 : CheckSum-Exe error %d %d %d [%s]", dwgCheckSum[Obj.CheckSumTableNum],
			lpMsg->m_dwKey, Obj.CheckSumTableNum, Obj.AccountID);
		this->GCSendDisableReconnect(Obj);
		//IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
		return;
	}

	Obj.CheckSumTime = 0;
}





void GameProtocol::PEchoProc(BYTE * aMsg, int aLen, CGameObject &Obj)
{
	for (int n = 0; n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		if (getGameObject(n)->Connected >= PLAYER_CONNECTED)
		{
			IOCP.DataSend(n, aMsg, aLen);
		}
	}
}







void GameProtocol::GCResultSend(CGameObject &Obj, BYTE headcode, BYTE result)
{
	PMSG_DEFRESULT pResult;

	PHeadSetB((BYTE*)&pResult, headcode, sizeof(pResult));
	pResult.result = result;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);
}



void GameProtocol::ChatSend(CGameObject &Obj, char* szChat)
{
	int len = strlen(szChat);

	if (len < 1)
		return;

	if (len > MAX_CHAT_LEN - 1)
		return;

	PMSG_CHATDATA_NUMBER pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x01, len + 6);
	std::strcpy(pMsg.chatmsg, szChat);
	pMsg.NumberH = SET_NUMBERH(Obj.m_Index);
	pMsg.NumberL = SET_NUMBERL(Obj.m_Index);

	this->MsgSendV2(Obj, (UCHAR*)&pMsg, pMsg.h.size);
}



void GameProtocol::vChatSend(CGameObject &Obj, char* szChat, ...)
{
	char szBuffer[256] = "";
	va_list pArguments;

	va_start(pArguments, szChat);
	vsprintf(szBuffer, szChat, pArguments);
	va_end(pArguments);

	int len = strlen(szBuffer);

	if (len < 1)
		return;

	if (len > MAX_CHAT_LEN - 1)
		return;

	/*	char szTempMsg[4096] = {0};
		MultiByteToWideChar(1258, 0, szChat, len, (WORD*)szTempMsg, sizeof(szTempMsg));
		std::memcpy(&pMsg.szChatMsg, szTempMsg, sizeof(pMsg.szChatMsg));
		pMsg.szChatMsg[MAX_CHAT_LEN-2]=0;
		pMsg.szChatMsg[MAX_CHAT_LEN-1]=0;
		PHeadSetB((BYTE*)&pMsg, 0x01, wcslen((WORD*)pMsg.szChatMsg)*2 + (sizeof(GC_CHATSEND)-sizeof(pMsg.szChatMsg)));
		pMsg.sIndex[0] = Obj >> 8;
		pMsg.sIndex[1] = Obj & 0xFF;*/

	PMSG_CHATDATA_NUMBER pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x01, len + 6);
	std::strcpy(pMsg.chatmsg, szBuffer);
	pMsg.NumberH = SET_NUMBERH(Obj.m_Index);
	pMsg.NumberL = SET_NUMBERL(Obj.m_Index);

	this->MsgSendV2(Obj, (UCHAR*)&pMsg, pMsg.h.size);
}





void GameProtocol::AllSendServerMsg(char* chatmsg)
{
	PMSG_NOTICE pNotice;

	TNotice::MakeNoticeMsg((TNotice*)&pNotice, 0x00, chatmsg);

	for (int n = g_ConfigRead.server.GetObjectStartUserIndex(); n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		if (getGameObject(n)->Connected == PLAYER_PLAYING)
		{
			if (getGameObject(n)->Type == OBJ_USER)
			{
				IOCP.DataSend(n, (BYTE*)&pNotice, pNotice.h.size);
			}
		}
	}
}



void GameProtocol::DataSendAll(BYTE* lpMsg, int iMsgSize)
{
	for (int n = g_ConfigRead.server.GetObjectStartUserIndex(); n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		if (getGameObject(n)->Connected == PLAYER_PLAYING)
		{
			if (getGameObject(n)->Type == OBJ_USER)
			{
				IOCP.DataSend(n, (BYTE*)lpMsg, iMsgSize);
			}
		}
	}
}




void GameProtocol::ChatTargetSend(CGameObject &Obj, char * szChat, int senduser)
{
	int len = strlen(szChat);

	if (len < 1 || len > MAX_CHAT_LEN - 1)
		return;

	PMSG_CHATDATA_NUMBER pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x01, len + 6);
	std::strcpy(pMsg.chatmsg, szChat);
	pMsg.NumberH = SET_NUMBERH(Obj.m_Index);
	pMsg.NumberL = SET_NUMBERL(Obj.m_Index);
	pMsg.chatmsg[len + 1] = 0;

	IOCP.DataSend(senduser, (UCHAR*)&pMsg, pMsg.h.size);
}

void GameProtocol::PChatProc(PMSG_CHATDATA * lpChat, CGameObject &Obj)
{
	int n;
	
	int number;
	int slen = strlen(lpChat->chatmsg);

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (slen < 1)
		return;

	if (slen > MAX_CHAT_LEN - 1)
	{
		sLog->outBasic("[Anti-HACK][PChatProc][%s][%s] Chat Message Len : %d", Obj.AccountID, Obj.Name, slen);
		return;
	}

	if (GetTickCount() - Obj.m_ChatFloodTime < 1000)
	{
		Obj.m_ChatFloodCount++;

		if (Obj.m_ChatFloodCount > 6)
		{
			sLog->outError( "Obj.m_ChatFloodCOunt > 6");
			//this->GCSendDisableReconnect(Obj);
			IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
			return;
		}
	}

	else
	{
		Obj.m_ChatFloodCount = 0;
	}

	Obj.m_ChatFloodTime = GetTickCount();

	char szId[MAX_ACCOUNT_LEN + 1];
	szId[MAX_ACCOUNT_LEN] = 0;
	CHAT_LOG_DATA pChatMsg;

	if (g_ConfigRead.server.GetServerType() != SERVER_BATTLECORE)
	{
		std::memcpy(szId, Obj.Name, MAX_ACCOUNT_LEN);
	}

	else
	{
		std::memcpy(szId, Obj.m_PlayerData->m_RealNameOfUBF, MAX_ACCOUNT_LEN);
	}
	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		char szBuff[100];
		std::memset(szBuff, 0x00, sizeof(szBuff));

		if (lpChat->chatmsg[0] != '!' &&
			lpChat->chatmsg[0] != '`' &&
			lpChat->chatmsg[0] != '@' &&
			lpChat->chatmsg[0] != '$' &&
			lpChat->chatmsg[0] != '~')
		{
			if (lpChat->chatmsg[0] == '/')
			{
				sprintf(szBuff, "%s", lpChat->chatmsg);
			}

			else
			{
				sprintf(szBuff, "[#%d]%s", Obj.m_PlayerData->m_nServerCodeOfHomeWorld, lpChat->chatmsg);
			}
		}

		else
		{
			sprintf(szBuff, "%c[#%d]%s", lpChat->chatmsg[0], Obj.m_PlayerData->m_nServerCodeOfHomeWorld, lpChat->chatmsg);
		}

		int nSize = strlen(szBuff) - slen;
		lpChat->h.size += nSize;
		std::memcpy(lpChat->chatmsg, szBuff, lpChat->h.size);
	}

	std::memcpy(lpChat->chatid, szId, MAX_ACCOUNT_LEN);

	int szTargetNameCount = 0;
	if (gWriteChatLog)
	{
		pChatMsg.h.c = 0xC1;
		pChatMsg.h.headcode = 0x02;
		std::memcpy(pChatMsg.AccountID, Obj.AccountID, MAX_ACCOUNT_LEN);
		std::memcpy(pChatMsg.Name, Obj.Name, MAX_ACCOUNT_LEN);
		pChatMsg.AccountID[MAX_ACCOUNT_LEN] = 0;
		pChatMsg.Name[MAX_ACCOUNT_LEN] = 0;
		pChatMsg.wServer = g_ConfigRead.server.GetGameServerCode();
		pChatMsg.btType = 0xFF;
	}

	//sLog->outBasic("[%s][%s]: %s",Obj.AccountID,Obj.Name,lpChat->chatmsg);
	if ((Obj.Penalty & 2) == 2)
		return;

	if (Obj.m_PlayerData->m_bSecurityCheck == false)
	{
		char seps[2] = " ";
		char string[256];
		char * szCmdToken;
		std::strcpy(string, lpChat->chatmsg);
		szCmdToken = strtok(string, seps);

		if (lpChat->chatmsg[0] == '/' && cManager.GetCmd(szCmdToken) == 431)
		{
		}

		else
		{
			return;
		}
	}
	switch (lpChat->chatmsg[0])
	{
	case '!':	// Global Announcement
		if (slen > 2)
		{
			if ((Obj.AuthorityCode & 1) == 1)
			{
				if (lpChat->chatmsg[1] == '!')
				{
					char Msg[128];
					sprintf(Msg, "[%s][%s]: %s", g_ConfigRead.server.GetServerName(), Obj.Name, &lpChat->chatmsg[2]);
					GS_GDReqMapSvrMsgMultiCast(g_MapServerManager.GetMapSvrGroup(), Msg);

					sLog->outBasic("Announcement: %s %s %s", Obj.AccountID, Obj.Name, &lpChat->chatmsg[2]);

					if (gWriteChatLog)
					{
						std::memcpy(pChatMsg.szChatMsg, &lpChat->chatmsg[2], MAX_CHAT_LEN - 1);
						pChatMsg.szChatMsg[MAX_CHAT_LEN] = 0;
						pChatMsg.btType = 0x03;
					}
					return;
				}
				else
				{

					PMSG_NOTICE pNotice;
					TNotice::MakeNoticeMsgEx(&pNotice, 0, "%s : %s", Obj.Name, &lpChat->chatmsg[1]);
					TNotice::SendNoticeToAllUser(&pNotice);

					sLog->outBasic("Announcement: %s %s %s", Obj.AccountID, Obj.Name, &lpChat->chatmsg[1]);

					if (gWriteChatLog)
					{
						std::memcpy(pChatMsg.szChatMsg, &lpChat->chatmsg[1], MAX_CHAT_LEN - 1);
						pChatMsg.szChatMsg[MAX_CHAT_LEN] = 0;
						pChatMsg.btType = 0x03;
					}
					return;
				}
			}
		}
		break;
	case '/':	// Command
		if (slen > 2)
		{

			cManager.ManagementProc(Obj, lpChat->chatmsg, Obj);
			return;
		}
		break;
	}

	// Other Commands
	if (Obj.ChatLimitTime > 0)
	{
		MsgOutput(Obj, Lang.GetText(0, 139), Obj.ChatLimitTime);
		return;
	}

	// Party Message
	if (lpChat->chatmsg[0] == '~' || lpChat->chatmsg[0] == ']')
	{
		if (Obj.m_PlayerData->m_bUsePartyMatching == false || Obj.m_PlayerData->m_nPartyMatchingIndex < 0)
		{
			if (Obj.PartyNumber >= 0)
			{
				int partycount = gParty.GetPartyCount(Obj.PartyNumber);

				if (partycount >= 0)
				{
					if (gWriteChatLog)
					{
						std::memcpy(pChatMsg.szChatMsg, &lpChat->chatmsg[1], MAX_CHAT_LEN - 1);
						pChatMsg.szChatMsg[MAX_CHAT_LEN] = 0;
						pChatMsg.btType = 0x01;
					}

					for (n = 0; n < MAX_USER_IN_PARTY; n++)
					{
						number = gParty.m_PartyS[Obj.PartyNumber].Number[n];

						if (number >= 0)
						{
							IOCP.DataSend(number, (BYTE*)lpChat, lpChat->h.size);

							if (gWriteChatLog)
							{
								std::strcpy(pChatMsg.szTargetName[szTargetNameCount], getGameObject(number)->Name);
								szTargetNameCount++;
							}
						}
					}
				}
			}
		}

		else
		{
			if (gWriteChatLog)
			{
				std::memcpy(pChatMsg.szChatMsg, &lpChat->chatmsg[1], MAX_CHAT_LEN - 1);
				pChatMsg.szChatMsg[MAX_CHAT_LEN] = 0;
				pChatMsg.btType = 0x01;
			}

			GDSendChatMsgPartyMatching(lpChat->chatmsg, Obj.Name, Obj.m_PlayerData->m_nPartyMatchingIndex);
		}
	}

	else if (lpChat->chatmsg[0] == '$')
	{
		if (Obj.m_PlayerData->m_GensInfluence == DUPRIAN_INFLUENCE || Obj.m_PlayerData->m_GensInfluence == VANERT_INFLUENCE)
		{
			for each (std::pair<int, CGameObject*> ObjEntry in gGameObjects)
			{
				if (gObjIsConnected(*ObjEntry.second) && ObjEntry.second->m_PlayerData->m_GensInfluence == Obj.m_PlayerData->m_GensInfluence)
				{
					IOCP.DataSend(n, (BYTE*)lpChat, lpChat->h.size);
				}
			}

			GDGensServerGroupChattingSend(Obj.m_PlayerData->m_GensInfluence, lpChat);
		}
	}

	// Guild
	else if (lpChat->chatmsg[0] == '@')
	{
		if (Obj.m_PlayerData->GuildNumber > 0)
		{
			// Notice
			if (lpChat->chatmsg[1] == '>')
			{
				if (Obj.Name == Obj.m_PlayerData->lpGuild->Users[0]->Name)
				{
					if (!strcmp(Obj.Name, Obj.m_PlayerData->lpGuild->Users[0]->Name))
					{
						for (int i = 0; i < strlen(lpChat->chatmsg); i++)
						{
							if (lpChat->chatmsg[i] == '%')
							{
								MsgOutput(Obj, Lang.GetText(0, 514));
								return;
							}
						}
						GDGuildNoticeSave(Obj.m_PlayerData->lpGuild->Name, &lpChat->chatmsg[2]);
						sLog->outBasic("Guild announcement %s %s", Obj.m_PlayerData->lpGuild->Name, lpChat->chatmsg);

						if (gWriteChatLog)
						{
							std::memcpy(pChatMsg.szChatMsg, &lpChat->chatmsg[2], MAX_CHAT_LEN - 2);
							pChatMsg.szChatMsg[MAX_CHAT_LEN] = 0;
							pChatMsg.btType = 0x04;
						}
					}
				}
			}
			// Aliances
			else if (lpChat->chatmsg[1] == '@' && Obj.m_PlayerData->lpGuild->iGuildUnion)
			{
				int iGuildCount = 0;
				int iGuildList[MAX_UNION_GUILD] = { 0 };

				if (UnionManager.GetGuildUnionMemberList(Obj.m_PlayerData->lpGuild->iGuildUnion, iGuildCount, iGuildList) == TRUE)
				{
					for (int i = 0; i < iGuildCount; i++)
					{
						GUILD_INFO_STRUCT *lpGuildInfo = Guild.SearchGuild_Number(iGuildList[i]);

						if (!lpGuildInfo)
							continue;

						for (n = 0; n < MAX_USER_GUILD; n++)
						{
							if (lpGuildInfo->Use[n])
							{
								number = lpGuildInfo->Index[n];

								if (number >= 0)
								{
									if (lpGuildInfo->Users[n]->Name == getGameObject(number)->Name)
									{
										if (!strcmp(lpGuildInfo->Users[n]->Name, getGameObject(number)->Name))
										{
											IOCP.DataSend(number, (BYTE*)lpChat, lpChat->h.size);
										}
									}
								}
							}
						}
					}
				}

				if (g_iServerGroupUnionChatting == TRUE)
					GDUnionServerGroupChattingSend(Obj.m_PlayerData->lpGuild->iGuildUnion, lpChat);
			}
			// Just Guild
			else
			{
				int count = Obj.m_PlayerData->lpGuild->Count;

				if (count >= 0)
				{
					for (n = 0; n < MAX_USER_GUILD; n++)
					{
						if (Obj.m_PlayerData->lpGuild->Use[n])
						{
							number = Obj.m_PlayerData->lpGuild->Index[n];

							if (number >= 0)
							{
								if (Obj.m_PlayerData->lpGuild->Users[n]->Name == getGameObject(number)->Name)
								{
									if (!strcmp(Obj.m_PlayerData->lpGuild->Users[n]->Name, getGameObject(number)->Name))
									{
										IOCP.DataSend(number, (BYTE*)lpChat, lpChat->h.size);
									}
								}
							}
						}
					}
				}

				if (g_iServerGroupGuildChatting == TRUE)
				{
					if (Obj.m_PlayerData->lpGuild->Count > 1)
					{
						GDGuildServerGroupChattingSend(Obj.m_PlayerData->lpGuild->Number, lpChat);
					}
				}

				if (gWriteChatLog)
				{
					std::memcpy(pChatMsg.szChatMsg, &lpChat->chatmsg[1], MAX_CHAT_LEN - 1);
					pChatMsg.szChatMsg[MAX_CHAT_LEN] = 0;
					pChatMsg.btType = 0x02;
				}
			}

			if (gWriteChatLog)
			{
				std::strcpy(pChatMsg.szTargetName[szTargetNameCount], Obj.m_PlayerData->GuildName);
				szTargetNameCount++;
			}
		}
	}
	else
	{
		if ((Obj.Authority & 0x20) == 0x20 && gObjCheckUsedBuffEffect(Obj, BUFFTYPE_INVISABLE) == TRUE)
		{
			return;
		}

		if (!g_prohibitedSymbols.Validate(lpChat->chatmsg, strlen(lpChat->chatmsg), TYPE_CHAT))
		{
			MsgOutput(Obj, Lang.GetText(0, 545));
			return;
		}

		if (SwearFilter.CompareText(lpChat->chatmsg) == true && lpChat->chatmsg[0] != '@' && lpChat->chatmsg[0] != '~')
		{
			++Obj.m_PlayerData->SwearWarning;

			if (Obj.m_PlayerData->SwearWarning >= 3)
			{
				MsgOutput(Obj, Lang.GetText(0, 515));
				//this->GCSendDisableReconnect(Obj);
				IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
			}
			else
			{
				MsgOutput(Obj, Lang.GetText(0, 516), Obj.m_PlayerData->SwearWarning);
			}
			return;
		}


		if (g_NewPVP.IsDuel(Obj) || g_NewPVP.IsObserver(Obj))
		{
			g_NewPVP.ChatMsgSend(Obj, (BYTE*)lpChat, lpChat->h.size);
		}

		else
		{
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)lpChat, lpChat->h.size);

			if (g_GensSystem.IsMapBattleZone(Obj.MapNumber))
			{
				g_GensSystem.BattleZoneChatMsgSend(Obj, (BYTE*)lpChat, lpChat->h.size);
			}

			else
			{
				this->MsgSendV2(Obj, (BYTE*)lpChat, lpChat->h.size);
			}
		}

		if (gWriteChatLog)
		{
			std::memcpy(pChatMsg.szChatMsg, lpChat->chatmsg, MAX_CHAT_LEN);
			pChatMsg.szChatMsg[MAX_CHAT_LEN] = 0;
			pChatMsg.btType = 0x00;

		}
	}

	if (gWriteChatLog)
	{
		if (pChatMsg.btType != 0xFF)
		{
			pChatMsg.btType |= szTargetNameCount << 4;
			pChatMsg.h.size = sizeof(pChatMsg) - (5 - szTargetNameCount) - 1;
		}
	}
}


void GameProtocol::CGChatRecv(PMSG_CHATDATA_NUMBER * lpMsg, CGameObject &Obj)
{
	if (PacketCheckTime(Obj) == FALSE)
	{
		return;
	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)lpMsg, lpMsg->h.size);
	MsgSendV2(Obj, (BYTE*)lpMsg, lpMsg->h.size);
}




void GameProtocol::GCServerMsgSend(BYTE msg, CGameObject &Obj)
{
	PMSG_SERVERMSG pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x0C, sizeof(pMsg));
	pMsg.MsgNumber = msg;
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}




void GameProtocol::GCServerMsgStringSend(LPSTR  szMsg, CGameObject &Obj, BYTE type)
{
	PMSG_NOTICE pNotice;

	TNotice::MakeNoticeMsg((TNotice*)&pNotice, type, szMsg);
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pNotice, pNotice.h.size);
}

void GameProtocol::GCServerMsgStringSendAll(LPSTR  szMsg, BYTE type)
{
	PMSG_NOTICE pNotice;

	TNotice::MakeNoticeMsg((TNotice*)&pNotice, type, szMsg);
	this->DataSendAll((UCHAR*)&pNotice, pNotice.h.size);
}

void GameProtocol::GCServerMsgStringSendAllEx(BYTE type, LPSTR szMsg, ...)
{
	va_list pArguments;
	char szBuffer[512];

	va_start(pArguments, szMsg);
	vsprintf(szBuffer, szMsg, pArguments);

	this->GCServerMsgStringSendAll(szBuffer, type);
}

void GameProtocol::GCServerMsgStringSendEx(CGameObject &Obj, BYTE type, LPSTR szMsg, ...)
{
	va_list pArguments;
	char szBuffer[512];

	va_start(pArguments, szMsg);
	vsprintf(szBuffer, szMsg, pArguments);

	this->GCServerMsgStringSend(szBuffer, Obj, type);
}


void GameProtocol::GCServerMsgStringSendGuild(GUILD_INFO_STRUCT &lpNode, LPSTR szMsg, BYTE type)
{
	for (int n = 0; n < MAX_USER_GUILD; n++)
	{
		if (lpNode.Use[n] > 0 && lpNode.Index[n] >= 0)
		{
			GCServerMsgStringSend((char*)szMsg, *lpNode.Users[n], type);
		}
	}
}

void GameProtocol::GCEventStateSend(CGameObject &Obj, BYTE state, BYTE event)
{
	PMSG_EVENT pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x0B, sizeof(pMsg));
	pMsg.Event = event;
	pMsg.State = state;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}



void GameProtocol::GCMapEventStateSend(int map, BYTE state, BYTE event)
{
	PMSG_EVENT pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x0B, sizeof(pMsg));
	pMsg.Event = event;
	pMsg.State = state;

	for (int n = 0; n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		if (getGameObject(n)->Connected == PLAYER_PLAYING && getGameObject(n)->Type == OBJ_USER && map == getGameObject(n)->MapNumber)
		{
			IOCP.DataSend(n, (UCHAR*)&pMsg, pMsg.h.size);
		}
	}
}



void GameProtocol::CGChatWhisperRecv(PMSG_CHATDATA_WHISPER* lpMsg, CGameObject &Obj)
{
	char tid[11];
	PMSG_CHATDATA_WHISPER pWhisper;
	int len;
	
	int index;

	if (PacketCheckTime(Obj) == FALSE)
	{
		return;
	}

	if (Obj.ChatLimitTime > 0)
	{
		MsgOutput(Obj, Lang.GetText(0, 139), Obj.ChatLimitTime);
		return;
	}

	if ((Obj.Penalty & 2) == 2)
	{
		return;
	}

	if (g_ConfigRead.data.common.SavePrivateChat == true)
	{
		sLog->outBasic(Lang.GetText(0, 517), Obj.AccountID, lpMsg->id, lpMsg->chatmsg);
	}

	tid[10] = 0;
	std::memcpy(tid, lpMsg->id, sizeof(lpMsg->id));
	index = WhisperCash.AddCash(tid);

	if (index < 0)
	{
		if (g_ConfigRead.data.common.joinmuMultiChannelWhisper == true)
		{
			//ifjoinmu
			Obj.m_Whispering->iWhisperSent = true;
			Obj.m_Whispering->wResponseCount = 0;
			PMSG_RECV_CHATDATA_WHISPER pDSMsg;
			pDSMsg.h.set((BYTE*)&pDSMsg, 0xC2, 0x00, sizeof(pDSMsg));
			std::memcpy(pDSMsg.fromId, Obj.Name, sizeof(pDSMsg.fromId));
			pDSMsg.wMapSvrNum = g_MapServerManager.GetMapSvrGroup();
			std::memcpy(pDSMsg.id, lpMsg->id, sizeof(lpMsg->id));
			std::memcpy(pDSMsg.chatmsg, lpMsg->chatmsg, sizeof(lpMsg->chatmsg));
			pDSMsg.OriginPlayerIndex = Obj.m_Index;
			wsDataCli.DataSend((char *)&pDSMsg, pDSMsg.h.size);
			return;
		}
		else
		{
			GCServerMsgSend(0, Obj);
			return;
		}
	}

	if (!UserChatBlockMng::getInstance()->isCanReceiveChat(Obj.m_Index, index))
	{
		return;
	}

	PHeadSetB((BYTE*)&pWhisper, 0x02, sizeof(pWhisper));
	std::strcpy(pWhisper.id, Obj.Name);
	std::memcpy(pWhisper.chatmsg, lpMsg->chatmsg, sizeof(lpMsg->chatmsg));

	//pWhisper.szChatMsg[MAX_CHAT_LEN-2] = 0;
	//pWhisper.chatmsg[MAX_CHAT_LEN-1] = 0;
	len = strlen(pWhisper.chatmsg);

	if (getGameObject(index)->m_PlayerData->bActiveSetParty == true)
	{
		if (len >= 4)
		{
			if (!strcmp(pWhisper.chatmsg, getGameObject(index)->m_PlayerData->PartyPassword))
			{
				gObjAutoPartySet(*getGameObject(index), Obj);
			}
		}
	}
	if (len > 0)
	{
		pWhisper.h.size -= sizeof(pWhisper.chatmsg);
		pWhisper.h.size += len + 1;

		if (g_GensSystem.IsMapBattleZone(Obj.MapNumber))
		{
			int OwnInfluence = g_GensSystem.GetGensInfluence(Obj);
			int TarInfluence = g_GensSystem.GetGensInfluence(*getGameObject(index));

			if (OwnInfluence == TarInfluence || CheckAuthorityCondition(98, Obj) || CheckAuthorityCondition(98, *getGameObject(index)))
			{
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pWhisper, pWhisper.h.size);
			}
		}

		else
		{
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pWhisper, pWhisper.h.size);
		}
	}
}

void GameProtocol::SCPJoinResultSend(CGameObject &Obj, BYTE result)
{
	PMSG_JOINRESULT pResult;

	std::memset(&pResult, 0, sizeof(pResult));

	pResult.h.size = sizeof(pResult);
	pResult.h.c = 0xC1;
	pResult.h.headcode = 0xF1;
	pResult.scode = 0x00;
	pResult.result = result;
	pResult.NumberH = SET_NUMBERH(Obj.m_Index);
	pResult.NumberL = SET_NUMBERL(Obj.m_Index);
	pResult.CliVersion[0] = szClientVersion[0];
	pResult.CliVersion[1] = szClientVersion[1];
	pResult.CliVersion[2] = szClientVersion[2];
	pResult.CliVersion[3] = szClientVersion[3];
	pResult.CliVersion[4] = szClientVersion[4];

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
	Obj.ConnectCheckTime = GetTickCount();
}




void GameProtocol::CSPJoinIdPassRequest(PMSG_IDPASS* lpMsg, CGameObject &Obj)
{
	char serial[17];
	char id[11];
	char hwid[25];

	if (lpMsg->CliVersion[0] != szClientVersion[0] ||
		lpMsg->CliVersion[1] != szClientVersion[1] ||
		lpMsg->CliVersion[2] != szClientVersion[2] ||
		lpMsg->CliVersion[3] != szClientVersion[3] ||
		lpMsg->CliVersion[4] != szClientVersion[4])
	{
		GCJoinResult(JS_BAD_CLIENT_VERSION, Obj);
		IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
		return;
	}

	if (lpMsg->ServerSeason != g_AuthSys.GetSeason())
	{
		sLog->outError( "[IP: %s][Obj: %d] connecting with DLL for different Game Season, review and correct (if required) %d", Obj.m_PlayerData->ConnectUser->IP, Obj, lpMsg->ServerSeason);

		//this->GCServerMsgStringSend(Lang.GetText(0,547), Obj, 0);
		//GCJoinResult(JS_BAD_CLIENT_VERSION, Obj);
		//IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
		//return;
	}

#ifdef EMU_NOCRYPT
	BuxConvert(lpMsg->Id, MAX_ACCOUNT_LEN);
	BuxConvert(lpMsg->Pass, 20);
#endif

	id[10] = 0;
	std::memcpy(id, lpMsg->Id, sizeof(lpMsg->Id));
	BuxConvert(id, MAX_ACCOUNT_LEN);
	ProcessClientHWID(lpMsg->HWID, hwid);

	if (PercentSyntaxCheck(id) == FALSE)
	{
		this->GCJoinResult(JS_BAD_CLIENT_VERSION, Obj);
		IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
		return;
	}

	serial[16] = 0;
	std::memcpy(serial, lpMsg->CliSerial, sizeof(lpMsg->CliSerial));

	if (strcmp(serial, szGameServerExeSerial) != 0)
	{
		this->GCJoinResult(JS_BAD_CLIENT_VERSION, Obj);
		IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
		return;
	}

	if (lpMsg->HWID == NULL)
	{
		IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
		return;
	}

	if (g_ConfigRead.server.IsConnectMemberUse() == TRUE)
	{
		if (ConMember.IsMember(id) == FALSE)
		{
			this->GCJoinResult(JS_ONLY_VIP_MEMBERS, Obj);
			return;
		}
	}

	if (g_ConfigRead.server.IsTestServer() == TRUE)
	{
		if (ConMember.IsMember(id) == FALSE)
		{
			this->GCJoinResult(2, Obj);
			this->GCServerMsgStringSend(Lang.GetText(0, 518), Obj, 0);
			return;
		}
	}

	

	if (PacketCheckTime(Obj) == FALSE)
	{
		IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
		return;
	}

	if (Obj.Connected != PLAYER_CONNECTED)
	{
		IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
		return;
	}

	if (Obj.LoginMsgSnd != FALSE)
	{
		if (Obj.LoginMsgSnd == TRUE)
		{

		}
		else
		{

		}

		return;
	}

	SDHP_IDPASS spMsg;

	PHeadSetB((BYTE*)&spMsg, 0x01, sizeof(spMsg));
	spMsg.Number = Obj.m_Index;
	std::memcpy(spMsg.Id, lpMsg->Id, sizeof(spMsg.Id));
	std::memcpy(spMsg.Pass, lpMsg->Pass, sizeof(spMsg.Pass));
	std::memcpy(spMsg.HWID, hwid, sizeof(hwid));
	std::strcpy(spMsg.IpAddress, Obj.m_PlayerData->ConnectUser->IP);
	Obj.CheckTick = lpMsg->TickCount;
	Obj.CheckTick2 = GetTickCount();
	Obj.ConnectCheckTime = GetTickCount();
	Obj.CheckSpeedHack = true;
	Obj.ConnectCheckTime = GetTickCount();
	Obj.AgiCheckTime = GetTickCount();
	Obj.LoginMsgSnd = 1;
	Obj.LoginMsgCount++;
	Obj.AccountID[MAX_ACCOUNT_LEN] = 0;
	std::memcpy(Obj.AccountID, id, MAX_ACCOUNT_LEN);
	std::memcpy(Obj.m_PlayerData->HWID, hwid, sizeof(hwid));
	Obj.m_cAccountItemBlock = 0;
	Obj.m_PlayerData->VipType = 0;
	Obj.m_PlayerData->VipEffect = VIP_EFFECT_NONE;
	Obj.m_PlayerData->AntiHackCheckTime = GetTickCount();
	Obj.m_PlayerData->dwLastHitHackTick = GetTickCount64();
	wsJServerCli.DataSend((char*)&spMsg, spMsg.h.size);
	sLog->outBasic("Connect Account: %s / MachineID: %s", Obj.AccountID, lpMsg->HWID);
	Obj.m_bMapSvrMoveReq = false;
	Obj.m_bMapSvrMoveReq_2 = false;
	Obj.m_sPrevMapSvrCode = -1;
	Obj.m_sDestMapNumber = -1;
	Obj.m_btDestX = 0;
	Obj.m_btDestY = 0;
}

void GameProtocol::GCJoinBillCheckSend(LPSTR AccountId, CGameObject &Obj)
{
	SDHP_BILLSEARCH pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x06, sizeof(pMsg));
	std::memcpy(pMsg.Id, AccountId, sizeof(pMsg.Id));
	pMsg.Number = Obj.m_Index;

	wsJServerCli.DataSend((char*)&pMsg, pMsg.h.size);
}




void GameProtocol::GCJoinResult(BYTE result, CGameObject &Obj)
{
	PMSG_RESULT  pResult;

	PHeadSubSetB((BYTE*)&pResult, 0xF1, 0x01, sizeof(pResult));
	pResult.result = result;
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);
}



void GameProtocol::GCJoinSocketResult(BYTE result, SOCKET Socket)
{
	PMSG_RESULT pResult;

	PHeadSubSetB((BYTE*)&pResult, 0xF1, 0x01, sizeof(pResult));
	pResult.result = result;

	wsGServer.DataSocketSend(Socket, (char*)&pResult, pResult.h.size);
}




void GameProtocol::CGClientCloseMsg(PMSG_CLIENTCLOSE * lpMsg, CGameObject &Obj)
{
	switch (lpMsg->Flag)
	{
	case 0:	// Close Game
		Obj.m_bOff = 0;
		gObjCloseSet(Obj, 0);
		break;

	case 2:	// ServerList
		Obj.m_bOff = 0;
		gObjCloseSet(Obj, 2);
		break;

	case 1: // Select Character
		Obj.m_bOff = 0;
		gObjCloseSet(Obj, 1);
		break;
	}
}




void GameProtocol::GCCloseMsgSend(CGameObject &Obj, BYTE result)
{
	PMSG_RESULT pMsg;

	PHeadSubSetBE((BYTE*)&pMsg, 0xF1, 0x02, sizeof(pMsg));
	pMsg.result = result;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}



void GameProtocol::CGClientMsg(PMSG_CLIENTMSG* lpMsg, CGameObject &Obj)
{
	if (lpMsg->Flag)
	{

	}
	else
	{
		if (Obj.m_ClientHackLogCount < 100)
		{

		}

		Obj.m_ClientHackLogCount++;

		if (Obj.m_ClientHackLogCount >= 100)
			Obj.m_ClientHackLogCount = 100;
	}

	char msg[255];

	wsprintf(msg, "Client HackCheck %d", lpMsg->Flag);
}

void GameProtocol::CGPCharacterCreate(PMSG_CHARCREATE * lpMsg, CGameObject &Obj)
{
	if (!PacketCheckTime(Obj))
	{
		JGCharacterCreateFailSend(Obj, lpMsg->Name);
		return;
	}

	if (Obj.Connected < PLAYER_LOGGED)
	{
		IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
		return;
	}

	if (Obj.Connected == PLAYER_PLAYING)
	{
		//this->GCSendDisableReconnect(Obj);
		IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);

		return;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 620), Obj, 1);
		JGCharacterCreateFailSend(Obj, lpMsg->Name);
		return;
	}

	if (Obj.m_PlayerData->m_bSecurityCheck == false)
	{
		GCServerMsgStringSend(Lang.GetText(0, 512), Obj, 1);
		JGCharacterCreateFailSend(Obj, lpMsg->Name);
		return;
	}

	if (!gCreateCharacter)
	{
		GCServerMsgStringSend(Lang.GetText(0, 513), Obj, 1);
		JGCharacterCreateFailSend(Obj, lpMsg->Name);
		return;
	}

	if (lpMsg->ClassSkin == 0x00 || lpMsg->ClassSkin == 0x10 || lpMsg->ClassSkin == 0x20 || lpMsg->ClassSkin == 0x30 || lpMsg->ClassSkin == 0x40 || lpMsg->ClassSkin == 0x50 || lpMsg->ClassSkin == 0x60 || lpMsg->ClassSkin == 0x70)
	{

	}
	else
	{
		JGCharacterCreateFailSend(Obj, lpMsg->Name);
		return;
	}

	if (lpMsg->ClassSkin == 0x30) // MG
	{
		if ((Obj.EnableCharacterCreate & 4) != 4)
		{
			JGCharacterCreateFailSend(Obj, lpMsg->Name);
			return;
		}
	}

	if (lpMsg->ClassSkin == 0x40) // DL
	{
		if ((Obj.EnableCharacterCreate & 2) != 2)
		{
			JGCharacterCreateFailSend(Obj, lpMsg->Name);
			return;
		}
	}

	if (lpMsg->ClassSkin == 0x50)
	{
		if ((Obj.EnableCharacterCreate & 1) != 1)
		{
			JGCharacterCreateFailSend(Obj, lpMsg->Name);
			return;
		}
	}

	if (lpMsg->ClassSkin == 0x60)
	{
		if ((Obj.EnableCharacterCreate & 8) != 8)
		{
			JGCharacterCreateFailSend(Obj, lpMsg->Name);
			return;
		}
	}

	if (lpMsg->ClassSkin == 0x70)
	{
		if ((Obj.EnableCharacterCreate & 0x10) != 0x10)
		{
			JGCharacterCreateFailSend(Obj, lpMsg->Name);
			return;
		}
	}

	char szName[11];
	szName[10] = 0;
	std::memcpy(szName, lpMsg->Name, MAX_ACCOUNT_LEN);
	if (!g_prohibitedSymbols.Validate(szName, strlen(szName), TYPE_NAME))
	{
		JGCharacterCreateFailSend(Obj, lpMsg->Name);
		return;
	}

	if (SwearFilter.CompareText(lpMsg->Name))
	{
		JGCharacterCreateFailSend(Obj, lpMsg->Name);
		return;
	}

	if (strstr(lpMsg->Name, "[A]") != 0)
	{
		JGCharacterCreateFailSend(Obj, lpMsg->Name);
		return;
	}

	SDHP_CREATECHAR sCreate;

	std::memset(&sCreate, 0, sizeof(sCreate));
	PHeadSetB((BYTE*)&sCreate, 0x04, sizeof(sCreate));
	sCreate.ClassSkin = lpMsg->ClassSkin;
	sCreate.Number = Obj.m_Index;
	std::memcpy(sCreate.AccountId, Obj.AccountID, MAX_ACCOUNT_LEN);
	std::memcpy(sCreate.Name, lpMsg->Name, MAX_ACCOUNT_LEN);
	char szTemp[11];
	char szTemp2[11];
	szTemp[10] = 0;
	szTemp2[10] = 0;
	std::memcpy(szTemp, sCreate.Name, MAX_ACCOUNT_LEN);
	std::memcpy(szTemp2, sCreate.AccountId, MAX_ACCOUNT_LEN);

	//sLog->outBasic("Character Created : %s,%s", szTemp2, szTemp);

	wsDataCli.DataSend((char*)&sCreate, sCreate.h.size);
	for (int n = 0; n < 5; n++)
	{

		if (strcmp(Obj.Name1[n], " ") == 0)
		{

			std::memcpy(Obj.Name1[n], szTemp, MAX_ACCOUNT_LEN);
			break;
		}
		if (strcmp(Obj.Name1[n], "") == 0)
		{

			std::memcpy(Obj.Name1[n], szTemp, MAX_ACCOUNT_LEN);
			break;
		}
	}
}

void GameProtocol::CGPCharDel(PMSG_CHARDELETE * lpMsg, CGameObject &Obj)
{

	if (!PacketCheckTime(Obj))
		return;

	if (Obj.Connected < PLAYER_LOGGED)
	{
		IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
		return;
	}

	if (Obj.Connected == PLAYER_PLAYING)
	{
		this->GCSendDisableReconnect(Obj);
		//IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
		return;
	}

	if (Obj.bEnableDelCharacter == FALSE)
	{
		this->GCSendDisableReconnect(Obj);
		//	IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
		return;
	}

	if (Obj.m_PlayerData->GuildNumber > 0 && Obj.m_PlayerData->lpGuild != NULL)
	{
		this->GCSendDisableReconnect(Obj);
		//IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
		return;
	}

	SDHP_CHARDELETE pCDel;
	PMSG_RESULT pResult;

	PHeadSubSetB((BYTE*)&pResult, 0xF3, 0x02, sizeof(pResult));
	pResult.result = 0;

	if (Obj.m_cAccountItemBlock)
	{
		pResult.result = 3;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (!g_ConfigRead.data.common.GuildDestroy)
	{
		pResult.result = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		pResult.result = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (Obj.m_PlayerData->m_bSecurityCheck == false)
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 513), Obj, 1);
		pResult.result = 3;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	char szJoomin[21];
	char szTemp[20];
	char szTemp2[20];
	bool Go = false;
	std::memset(szJoomin, 0, sizeof(szJoomin));
	std::memcpy(szJoomin, lpMsg->LastJoominNumber, sizeof(lpMsg->LastJoominNumber));

	if (gObjPasswordCheck(Obj, szJoomin) == FALSE)
	{
		pResult.result = 2;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	PHeadSetB((BYTE*)&pCDel, 0x05, sizeof(pCDel));
	pCDel.Number = Obj.m_Index;
	std::memcpy(pCDel.AccountID, Obj.AccountID, sizeof(pCDel.AccountID));
	std::memcpy(pCDel.Name, lpMsg->Name, sizeof(lpMsg->Name));
	pCDel.Guild = 0;

	if (Obj.m_PlayerData->GuildNumber > 0 && Obj.m_PlayerData->lpGuild != NULL)
	{
		if (!strcmp(Obj.Name, Obj.m_PlayerData->lpGuild->Users[0]->Name))
		{
			pCDel.Guild = 1;	// Master
		}
		else
		{
			pCDel.Guild = 2;	// Member
		}

		std::memcpy(pCDel.GuildName, Obj.m_PlayerData->lpGuild->Name, MAX_GUILD_LEN);
	}

	g_UnityBattleField.GDReqCancelUnityBattleField(Obj, 2, lpMsg->Name);

	szTemp[10] = 0;
	szTemp2[10] = 0;
	std::memcpy(szTemp, pCDel.Name, MAX_ACCOUNT_LEN);
	std::memcpy(szTemp2, pCDel.AccountID, MAX_ACCOUNT_LEN);

	wsDataCli.DataSend((PCHAR)&pCDel, pCDel.h.size);
	Obj.Level = 0;
}



void GameProtocol::CGPCharacterMapJoinRequest(PMSG_CHARMAPJOIN * lpMsg, CGameObject &Obj)
{
	if (!PacketCheckTime(Obj))
	{
		//JGCharacterCreateFailSend(Obj, lpMsg->Name);
		return;
	}

	if (!gObjIsAccontConnect(Obj, Obj.AccountID))
		return;

	if (Obj.Connected == PLAYER_PLAYING)
		return;

	Obj.bEnableDelCharacter = FALSE;

	char _name[MAX_ACCOUNT_LEN + 1];
	SDHP_DBCHARINFOREQUEST pCRequest;

	PHeadSetB((BYTE*)&pCRequest, 0x06, sizeof(pCRequest));
	std::memset(_name, 0, MAX_ACCOUNT_LEN);

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		std::string tmpCmpName;
		tmpCmpName.insert(0, lpMsg->Name, MAX_ACCOUNT_LEN);

		if (Obj.m_PlayerData->m_NameConvertOfUBF.IsDuplicationOfName() == TRUE)
		{
			std::memcpy(_name, Obj.m_PlayerData->m_NameConvertOfUBF.ConvertSlotIndexToName(lpMsg->btSlotIndex), MAX_ACCOUNT_LEN);
		}

		else
		{
			std::memcpy(_name, Obj.m_PlayerData->m_NameConvertOfUBF.ConvertRealNameToName(tmpCmpName.c_str()), MAX_ACCOUNT_LEN);
		}
	}

	else
	{
		std::memcpy(_name, lpMsg->Name, MAX_ACCOUNT_LEN);
	}

	std::memcpy(pCRequest.Name, _name, MAX_ACCOUNT_LEN);
	std::strcpy(pCRequest.AccountID, Obj.AccountID);
	pCRequest.Number = Obj.m_Index;

	wsDataCli.DataSend((char*)&pCRequest, pCRequest.h.size);
}

void GameProtocol::GCLevelUpMsgSend(CGameObject &Obj, int iSendEffect)
{
	short AddPoint = 0;
	short MaxAddPoint = 0;
	short MinusPoint = 0;
	short MaxMinusPoint = 0;

	gObjGetStatPointState(Obj, AddPoint, MaxAddPoint, MinusPoint, MaxMinusPoint);

	PMSG_LEVELUP pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x05, sizeof(pMsg));
	pMsg.Level = Obj.Level;
	pMsg.LevelUpPoint = Obj.m_PlayerData->LevelUpPoint;
	pMsg.MaxLife = Obj.MaxLife + Obj.AddLife;
	pMsg.MaxMana = Obj.MaxMana + Obj.AddMana;
	pMsg.MaxBP = Obj.MaxBP + Obj.AddBP;
	pMsg.wMaxShield = Obj.iMaxShield + Obj.iAddShield;
	pMsg.AddPoint = AddPoint;
	pMsg.MaxAddPoint = MaxAddPoint;
	pMsg.MinusPoint = MinusPoint;
	pMsg.MaxMinusPoint = MaxMinusPoint;
	pMsg.IGCMaxLife = Obj.MaxLife + Obj.AddLife;
	pMsg.IGCMaxMana = Obj.MaxMana + Obj.AddMana;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

	if (iSendEffect == 1)
		GCSendEffectInfo(Obj, 0x10);
}

void GameProtocol::CGLevelUpPointAdd(PMSG_LVPOINTADD * lpMsg, CGameObject &Obj)
{
	if (!gObjIsConnected(Obj))
	{
		this->GCSendDisableReconnect(Obj);
		//IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		this->GCSendDisableReconnect(Obj);
		//IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
		return;
	}

	EnterCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);

	Obj.m_PlayerData->AgilityCheckDelay = GetTickCount();

	PMSG_LVPOINTADDRESULT pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x06, sizeof(pMsg));
	pMsg.ResultType = 0;

	if (::gObjLevelUpPointAdd(lpMsg->Type, &Obj, Obj) == TRUE)
	{
		pMsg.ResultType = 0x10;
		pMsg.ResultType += lpMsg->Type;

		switch (lpMsg->Type)
		{
		case 2:	// Vit
			pMsg.MaxLifeAndMana = Obj.MaxLife + Obj.AddLife;
			pMsg.IGCMaxLifeAndMana = Obj.MaxLife + Obj.AddLife;
			break;

		case 3:	// Energy
			pMsg.MaxLifeAndMana = Obj.MaxMana + Obj.AddMana;
			pMsg.IGCMaxLifeAndMana = Obj.MaxMana + Obj.AddMana;
			break;
		}

		pMsg.wMaxShield = Obj.iMaxShield + Obj.iAddShield;
		gObjSetBP(Obj);
		pMsg.MaxBP = Obj.MaxBP + Obj.AddBP;
	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR *)&pMsg, pMsg.h.size);
	LeaveCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);

	/*PMSG_ADDSTATS_RESULT pAddStats;
	PHeadSubSetB((BYTE*)&pAddStats, 0xEC, 0x25, sizeof(pAddStats));
	pAddStats.str = Obj.m_PlayerData->Strength;
	pAddStats.stradd = Obj.AddStrength;
	pAddStats.agi = Obj.m_PlayerData->Dexterity;
	pAddStats.agiadd = Obj.AddDexterity;
	pAddStats.vit = Obj.m_PlayerData->Vitality;
	pAddStats.vitadd = Obj.AddVitality;
	pAddStats.ene = Obj.m_PlayerData->Energy;
	pAddStats.eneadd = Obj.AddEnergy;
	pAddStats.cmd = Obj.Leadership;
	pAddStats.cmdadd = Obj.AddLeadership;
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pAddStats, pAddStats.h.size); */

	GCPlayerStatsPanelNew(Obj);
	GCPlayerStatsPanelRates(Obj);
}

void GameProtocol::GCInventoryItemOneSend(CGameObject &Obj, int pos)
{
	if (!Obj.pntInventory[pos]->IsItem())
		return;

	PMSG_INVENTORYITEMMODIFY pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x14, sizeof(pMsg));
	pMsg.Pos = pos;
	ItemByteConvert(pMsg.ItemInfo, *Obj.pntInventory[pos]);

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR *)&pMsg, pMsg.h.size);
}

void GameProtocol::GCPkLevelSend(CGameObject &Obj, BYTE pklevel)
{
	PMSG_PKLEVEL pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x08, sizeof(pMsg));
	pMsg.NumberH = SET_NUMBERH(Obj.m_Index);
	pMsg.NumberL = SET_NUMBERL(Obj.m_Index);
	pMsg.PkLevel = pklevel;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
	MsgSendV2(&Obj, (UCHAR*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCMagicListOneSend(CGameObject &Obj, char Pos, WORD type, BYTE level, WORD skill, BYTE btListType)
{
	PMSG_MAGICLISTCOUNT pCount;
	PMSG_MAGICLIST pList;
	BYTE sendbuf[1000];

	int lOfs = sizeof(pCount);
	PHeadSubSetW((BYTE*)&pCount, 0xF3, 0x11, 0);

	pCount.Count = 0xFE;
	pCount.btListType = btListType;

	pList.Pos = Pos;
	pList.wSkillNum = type;
	pList.btSkillLevel = level << 3;
	pList.btSkillLevel |= (BYTE)skill & 0x07;

	std::memcpy(&sendbuf[lOfs], &pList, sizeof(pList));
	lOfs += sizeof(pList);

	pCount.h.sizeH = SET_NUMBERH(lOfs);
	pCount.h.sizeL = SET_NUMBERL(lOfs);
	std::memcpy(sendbuf, &pCount, sizeof(pCount));

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, sendbuf, lOfs);
}


void GameProtocol::GCMagicListOneDelSend(CGameObject &Obj, char Pos, WORD type, BYTE level, WORD skill, BYTE btListType)
{
	PMSG_MAGICLISTCOUNT pCount;
	PMSG_MAGICLIST pList;
	BYTE sendbuf[1000];

	int lOfs = sizeof(pCount);
	PHeadSubSetW((BYTE*)&pCount, 0xF3, 0x11, 0);

	pCount.h.sizeH = 0;
	pCount.h.sizeL = 0;
	pCount.Count = 0xFF;
	pCount.btListType = btListType;

	pList.Pos = Pos;
	pList.wSkillNum = type;
	pList.btSkillLevel = level << 3;
	pList.btSkillLevel |= (BYTE)skill & 0x07;

	std::memcpy(&sendbuf[lOfs], &pList, sizeof(pList));
	lOfs += sizeof(pList);

	pCount.h.sizeH = SET_NUMBERH(lOfs);
	pCount.h.sizeL = SET_NUMBERL(lOfs);
	std::memcpy(sendbuf, &pCount, sizeof(pCount));

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, sendbuf, lOfs);
}


void GameProtocol::GCMagicListMultiSend(CGameObject &Obj, BYTE btListType)
{
	WORD BrandSkill;
	PMSG_MAGICLISTCOUNT pCount;
	PMSG_MAGICLIST pList;
	BYTE sendbuf[1000];

	int lOfs = sizeof(pCount);
	int Count = 0;
	pCount.btListType = btListType;

	PHeadSubSetW((BYTE*)&pCount, 0xF3, 0x11, 0);

	for (int n = 0; n < MAX_MAGIC; n++)
	{
		if (Obj.Magic[n].IsMagic() == TRUE)
		{
			if (MagicDamageC.GetSkillUseType(Obj.Magic[n].m_Skill) == 3)
			{
				continue;
			}

			if (!g_MasterLevelSkillTreeSystem.CheckUsableWeaponSkill(Obj, n))
			{
				continue;
			}

			if (g_MasterLevelSkillTreeSystem.IsExistMLSBasedSkill(Obj, Obj.Magic[n].m_Skill))
			{
				continue;
			}

			if (Obj.Magic[n].m_Pass == true)
			{
				continue;
			}

			pList.Pos = n;
			pList.wSkillNum = (WORD)Obj.Magic[n].m_Skill;
			pList.btSkillLevel = Obj.Magic[n].m_Level << 3;
			pList.btSkillLevel |= Obj.Magic[n].m_Skill & 0x07;

			std::memcpy(&sendbuf[lOfs], &pList, sizeof(pList));
			lOfs += sizeof(pList);

			Count++;

			if (MagicDamageC.IsItemAttachedSkill(Obj.Magic[n].m_Skill))
			{
				BrandSkill = MagicDamageC.GetNormalBrandOfSkill(Obj.Magic[n].m_Skill);

				if (!Obj.pntInventory[1]->IsItem() || Obj.pntInventory[1]->m_Special[0] != BrandSkill)
				{
					continue;
				}

				if (!Obj.pntInventory[0]->IsItem() || Obj.pntInventory[0]->m_Special[0] != BrandSkill)
				{
					continue;
				}

				pList.Pos = n + 1;
				pList.wSkillNum = LOWORD(Obj.Magic[n].m_Skill);
				pList.btSkillLevel = Obj.Magic[n].m_Level << 3;
				pList.btSkillLevel |= Obj.Magic[n].m_Skill & 0x07;

				std::memcpy(&sendbuf[lOfs], &pList, sizeof(pList));
				lOfs += sizeof(pList);

				Count++;
			}
		}
	}

	pCount.Count = Count;
	pCount.h.sizeH = SET_NUMBERH(lOfs);
	pCount.h.sizeL = SET_NUMBERL(lOfs);
	std::memcpy(sendbuf, &pCount, sizeof(pCount));

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, sendbuf, lOfs);
}

void GameProtocol::GCEquipmentSend(CGameObject &Obj)
{
	PMSG_EQUIPMENTLIST pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x13, sizeof(pMsg));
	pMsg.NumberH = SET_NUMBERH(Obj.m_Index);
	pMsg.NumberL = SET_NUMBERL(Obj.m_Index);
	gObjMakePreviewCharSet(Obj);
	
	std::memcpy(pMsg.Equipment, Obj.CharSet, sizeof(Obj.CharSet));

	MsgSendV2(Obj, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCRecallMonLife(CGameObject &Obj, int maxlife, int life)
{
	if (maxlife <= 0)
		return;

	PMSG_RECALLMONLIFE pMsg;
	BYTE per = life * 100 / maxlife;
	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x20, sizeof(pMsg));
	pMsg.Life = per;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCTimeViewSend(CGameObject &Obj, int second)
{
	PMSG_TIMEVIEW pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x22, sizeof(pMsg));
	pMsg.Second = second;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR *)&pMsg, pMsg.h.size);
}

void GameProtocol::GCGoalSend(CGameObject &Obj, char* Name1, BYTE score1, char* Name2, BYTE score2)
{
	PMSG_GOALSEND pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x23, sizeof(pMsg));
	std::memcpy(pMsg.RedTeamName, Name1, sizeof(pMsg.RedTeamName));
	pMsg.RedTeamScore = score1;
	std::memcpy(pMsg.BlueTeamName, Name2, sizeof(pMsg.BlueTeamName));
	pMsg.BlueTeamScore = score2;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCSkillKeyRecv(PMSG_SKILLKEY * lpMsg, CGameObject &Obj)
{
	if (!gObjIsGamePlaing(&Obj))
		return;

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	DGOptionDataSend(Obj, Obj.Name,
		lpMsg->SkillKey, lpMsg->GameOption,
		lpMsg->QkeyDefine, lpMsg->WkeyDefine, lpMsg->EkeyDefine, lpMsg->ChatWindow, lpMsg->RkeyDefine, ntohl(lpMsg->QWERLevel), Obj.m_PlayerData->m_EnableUseChangeSkin);
}

void GameProtocol::GCSkillKeySend(CGameObject &Obj, BYTE* keybuffer, BYTE GO, BYTE Qk, BYTE Wk, BYTE Ek, BYTE ChatWnd, BYTE Rk, int QWER)
{
	PMSG_SKILLKEY pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x30, sizeof(pMsg));
	std::memcpy(pMsg.SkillKey, keybuffer, sizeof(pMsg.SkillKey));
	pMsg.GameOption = GO;
	pMsg.QkeyDefine = Qk;
	pMsg.WkeyDefine = Wk;
	pMsg.EkeyDefine = Ek;
	pMsg.ChatWindow = ChatWnd;
	pMsg.RkeyDefine = Rk;
	pMsg.QWERLevel = QWER;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCMoneySend(CGameObject &Obj, DWORD money)
{
	PMSG_ITEMGETRESULT pMsg;

	pMsg.h.c = 0xC3;
	pMsg.h.headcode = 0x22;
	pMsg.h.size = sizeof(pMsg);
	pMsg.result = 0xFE;

	WORD hiWord = SET_NUMBERHW(money);
	WORD loWord = SET_NUMBERLW(money);
	pMsg.Data[0] = SET_NUMBERH(hiWord);
	pMsg.Data[1] = SET_NUMBERL(hiWord);
	pMsg.Data[2] = SET_NUMBERH(loWord);
	pMsg.Data[3] = SET_NUMBERL(loWord);

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}


void GameProtocol::GCItemObjectInventoryPutSend(CGameObject &Obj, BYTE result, BYTE iteminfo1, BYTE iteminfo2)
{
	PMSG_ITEMGETRESULT pResult;

	pResult.h.c = 0xC3;
	pResult.h.headcode = 0x22;
	pResult.h.size = sizeof(pResult);
	pResult.result = result;
	pResult.Data[0] = iteminfo1;
	pResult.Data[1] = iteminfo2;
	pResult.h.size -= 2;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);
}



void GameProtocol::CGItemGetRequest(PMSG_ITEMGETREQUEST * lpMsg, CGameObject &Obj, BYTE ID, BYTE ID1)
{
	int item_num, map_num;
	PMSG_ITEMGETRESULT pResult;
	int type;
	int level;
	int special;
	int NOption;
	char szItemName[50];
	CMapItem * lpItem;

	pResult.h.c = 0xC3;
	pResult.h.headcode = 0x22;
	pResult.h.size = sizeof(pResult);
	pResult.result = -1;

	if (!gObjIsConnected(Obj))
	{
		IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.CloseType != -1)
		return;

	if (Obj.m_PlayerData->m_bSecurityCheck == false)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	if (Obj.DieRegen != 0)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	if (g_NewPVP.IsObserver(Obj) == TRUE)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	if (Obj.m_IfState->use != 0)
	{
		if (Obj.m_IfState->type != 3)
		{
			this->GCServerMsgStringSend(Lang.GetText(0, 31), Obj, 1);
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

			return;
		}
	}

	if (!::gObjFixInventoryPointer(Obj))

		if (Obj.pTransaction == TRUE)
		{
			return;

		}

	CMuRummyInfo * pMuRummyInfo = Obj.m_PlayerData->m_pCMuRummyInfo;

	if (!pMuRummyInfo)
	{
		return;
	}

	if (pMuRummyInfo->IsWaitReward() == true)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

		this->GCServerMsgStringSend(Lang.GetText(0, 561), Obj, 1);
		return;
	}

	item_num = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	if (MaxItemTypeRange(item_num) == FALSE)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	map_num = Obj.MapNumber;

	if (MAX_MAP_RANGE(map_num) == FALSE)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	lpItem = &MapC[map_num].m_CItemObject[item_num];

	if (lpItem->IsItem() == TRUE && lpItem->Give == false && lpItem->live == true)
	{
		if (IT_MAP_RANGE(map_num) == TRUE)
		{
			if (lpItem->m_Type == ITEMGET(14, 223))
			{
				if (g_ConfigRead.server.GetServerType() == SERVER_NORMAL)
				{
					if (g_IT_Event.CheckRelics(Obj.m_Index) == TRUE)
					{
						pResult.result = -1;
						pResult.h.size -= sizeof(pResult.Data);
						IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

						return;
					}
				}
			}
		}

		if (lpItem->m_QuestItem != false)
		{
			bool bIsGetItem = g_QuestInfo.GetQuestItem(Obj, lpItem->m_Type, lpItem->m_Level);
			bool bIsQuestExpItem = g_QuestExpProgMng.IsQuestDropItem(Obj, lpItem->m_Type, lpItem->m_Level);

			if (bIsGetItem == false && bIsQuestExpItem == false)
			{
				pResult.result = -1;
				pResult.h.size -= sizeof(pResult.Data);
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
				return;
			}
		}

		if (IsJumpingEventItem(lpItem->m_Type))
		{
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}

		LPITEM_ATTRIBUTE pItemAttribute = GetItemAttr(lpItem->m_Type);

		if (!pItemAttribute)
		{
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}

		if (pItemAttribute->ItemKindA == 12)
		{
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}

		if (lpItem->m_Type == ITEMGET(13, 20))	// Wizard Ring
		{
			switch (lpItem->m_Level)
			{
			case 0:
			{
				int iWRCount = gObjGetItemCountInIventory(Obj, lpItem->m_Type / MAX_SUBTYPE_ITEMS, lpItem->m_Type%MAX_SUBTYPE_ITEMS, lpItem->m_Level);

				if (iWRCount > 0)
				{
					pResult.result = -1;
					pResult.h.size -= sizeof(pResult.Data);

					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
					this->GCServerMsgStringSend(Lang.GetText(0, 105), Obj, 1);

					return;
				}
				break;
			}
			case 1:

				pResult.result = -1;
				pResult.h.size -= sizeof(pResult.Data);

				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
				return;
				break;

			}
		}

		if (lpItem->m_Type == ITEMGET(13, 20) && lpItem->m_Level == 2)	// Wizard Ring
		{
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

			return;
		}

		if (lpItem->m_Type == ITEMGET(13, 38))	// MoonStonePendant
		{
			int count = gObjGetItemCountInIventory(Obj, lpItem->m_Type / MAX_SUBTYPE_ITEMS,
				lpItem->m_Type % MAX_SUBTYPE_ITEMS, lpItem->m_Level);

			if (count > 0)
			{
				pResult.result = -1;
				pResult.h.size -= sizeof(pResult.Data);

				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
				GCServerMsgStringSend(Lang.GetText(0, 285), Obj, 1);

				return;
			}
		}

		if (lpItem->m_Type == ITEMGET(13, 39))	// ChangeRing
		{
			int count = gObjGetItemCountInIventory(Obj, lpItem->m_Type / MAX_SUBTYPE_ITEMS,
				lpItem->m_Type % MAX_SUBTYPE_ITEMS, lpItem->m_Level);

			if (count > 0)
			{
				pResult.result = -1;
				pResult.h.size -= sizeof(pResult.Data);

				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
				GCServerMsgStringSend(Lang.GetText(0, 287), Obj, 1);

				return;
			}
		}

		type = lpItem->m_Type;
		level = lpItem->m_Level;
		special = lpItem->m_Special[0];
		NOption = lpItem->m_NewOption;
		std::strcpy(szItemName, lpItem->GetName());

		if (lpItem->m_Type == ITEMGET(14, 15)) // Zen
		{
			if (MapC[map_num].ItemGive(Obj, item_num, false) == TRUE)
			{
				if (!gObjCheckMaxZen(Obj, lpItem->m_BuyMoney))
				{
					if (Obj.m_PlayerData->Money < MAX_ZEN)
					{
						Obj.m_PlayerData->Money = MAX_ZEN;
						pResult.result = -2;
						WORD hiWord = SET_NUMBERHW(Obj.m_PlayerData->Money);
						WORD loWord = SET_NUMBERLW(Obj.m_PlayerData->Money);
						pResult.Data[0] = SET_NUMBERH(hiWord);
						pResult.Data[1] = SET_NUMBERL(hiWord);
						pResult.Data[2] = SET_NUMBERH(loWord);
						pResult.Data[3] = SET_NUMBERL(loWord);

						IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

						return;
					}

					pResult.result = -1;
					pResult.h.size -= sizeof(pResult.Data);

					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

					return;
				}

				Obj.m_PlayerData->Money += lpItem->m_BuyMoney;
				pResult.result = -2;

				WORD hiWord = SET_NUMBERHW(Obj.m_PlayerData->Money);
				WORD loWord = SET_NUMBERLW(Obj.m_PlayerData->Money);
				pResult.Data[0] = SET_NUMBERH(hiWord);
				pResult.Data[1] = SET_NUMBERL(hiWord);
				pResult.Data[2] = SET_NUMBERH(loWord);
				pResult.Data[3] = SET_NUMBERL(loWord);
				pResult.h.size -= 3;
			}

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		}
		else
		{
			if (lpItem->m_Type == ITEMGET(13, 32))
			{
				int pos = gObjOverlapItemUsingDur(*lpItem, map_num, item_num, Obj, IsOverlapItem(lpItem->m_Type), ITEMGET(13, 32), 0);

				if (MAIN_INVENTORY_RANGE(pos) != FALSE)
				{
					pResult.result = -3;
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
					Obj.pntInventory[pos]->m_Durability += lpItem->m_Durability;
					GCItemObjectDurSend(Obj, pos, Obj.pntInventory[pos]->m_Durability, 0);

					return;
				}
			}

			if (lpItem->m_Type == ITEMGET(13, 33))
			{
				int pos = gObjOverlapItemUsingDur(*lpItem, map_num, item_num, Obj, IsOverlapItem(lpItem->m_Type), ITEMGET(13, 33), 0);

				if (MAIN_INVENTORY_RANGE(pos) != FALSE)
				{
					pResult.result = -3;
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
					Obj.pntInventory[pos]->m_Durability += lpItem->m_Durability;
					GCItemObjectDurSend(Obj, pos, Obj.pntInventory[pos]->m_Durability, 0);

					return;
				}
			}

			if (lpItem->m_Type == ITEMGET(13, 34))
			{
				int pos = gObjOverlapItemUsingDur(*getGameObject(lpItem->m_Number), map_num, item_num, Obj, IsOverlapItem(lpItem->m_Type), ITEMGET(13, 34), 0);

				if (MAIN_INVENTORY_RANGE(pos) != FALSE)
				{
					pResult.result = -3;
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
					Obj.pntInventory[pos]->m_Durability += lpItem->m_Durability;
					GCItemObjectDurSend(Obj, pos, Obj.pntInventory[pos]->m_Durability, 0);

					return;
				}
			}

			if (lpItem->m_Type == ITEMGET(14, 70) || lpItem->m_Type == ITEMGET(14, 71) || lpItem->m_Type == ITEMGET(14, 94)
				|| lpItem->m_Type == ITEMGET(14, 133)	//6.2
				)
			{
				int pos = gObjOverlapItemUsingDur((lpItem) ? ((CItemObject *)&lpItem->m_Number) : NULL, map_num, item_num, Obj, 50, lpItem->m_Type, 0);

				if (MAIN_INVENTORY_RANGE(pos) != FALSE)
				{
					pResult.result = -3;
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
					Obj.pntInventory[pos]->m_Durability += lpItem->m_Durability;
					GCItemObjectDurSend(Obj, pos, Obj.pntInventory[pos]->m_Durability, 0);
					return;
				}
			}

			if (lpItem->m_Type == ITEMGET(13, 147)) // Trophy of Battle
			{
				int pos = gObjOverlapItemUsingDur((lpItem) ? ((CItemObject *)&lpItem->m_Number) : NULL, map_num, item_num, Obj, IsOverlapItem(lpItem->m_Type), ITEMGET(13, 147), 0);

				if (MAIN_INVENTORY_RANGE(pos) != FALSE)
				{
					pResult.result = -3;
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
					Obj.pntInventory[pos]->m_Durability += lpItem->m_Durability;
					GCItemObjectDurSend(Obj, pos, Obj.pntInventory[pos]->m_Durability, 0);

					return;
				}
			}

			if (lpItem->m_Type == ITEMGET(14, 29)) // Symbol of Kundun
			{
				for (int n = 0; n < MAIN_INVENTORY_SIZE; n++)
				{
					int pos = ::g_KalimaGate.CheckOverlapKundunMark(Obj, lpItem->m_Level);

					if (MAIN_INVENTORY_RANGE(pos))
					{
						if (MapC[map_num].ItemGive(Obj, item_num, true) == TRUE)
						{
							BYTE NewOption[MAX_EXOPTION_SIZE];

							::ItemIsBufExOption(NewOption, (lpItem != NULL) ? (CItemObject*)&lpItem->m_Number : NULL);

							pResult.result = -3;

							IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

							Obj.pntInventory[pos]->m_Durability += lpItem->m_Durability;

							if (Obj.pntInventory[pos]->m_Durability >= IsOverlapItem(lpItem->m_Type))
							{
								int NewDur = Obj.pntInventory[pos]->m_Durability - IsOverlapItem(lpItem->m_Type);
								::gObjInventoryItemSet(Obj, pos, -1);
								Obj.pntInventory[pos]->Clear();
								this->GCInventoryItemDeleteSend(Obj, pos, 1);
								::ItemCreate(Obj, 235, Obj.X, Obj.Y, ItemGetNumberMake(14, 28),
									lpItem->m_Level, 0, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);

								if (NewDur > 0)
								{
									lpItem->m_State = 2;
									lpItem->Give = false;
									lpItem->live = true;
									lpItem->m_Durability = NewDur;

									continue;

								}
							}
							else
							{
								GCItemObjectDurSend(Obj, pos, Obj.pntInventory[pos]->m_Durability, 0);

							}
							return;
						}
					}
					else
					{
						break;
					}
				}
			}
			if (lpItem->m_Type == ITEMGET(14, 110)) // Double Goer
			{
				int pos = gObjOverlapItemUsingDur((lpItem) ? ((CItemObject *)&lpItem->m_Number) : NULL, map_num, item_num, Obj, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 110), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
					Obj.pntInventory[pos]->m_Durability += lpItem->m_Durability;

					if (Obj.pntInventory[pos]->m_Durability >= IsOverlapItem(lpItem->m_Type))
					{
						int NewDur = Obj.pntInventory[pos]->m_Durability - IsOverlapItem(lpItem->m_Type);
						::gObjInventoryItemSet(Obj, pos, -1);
						Obj.pntInventory[pos]->Clear();
						this->GCInventoryItemDeleteSend(Obj, pos, 1);
						::ItemCreate(Obj, 235, Obj.X, Obj.Y, ItemGetNumberMake(14, 111),
							lpItem->m_Level, 0, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);

						if (NewDur > 0)
						{
							lpItem->m_State = 2;
							lpItem->Give = false;
							lpItem->live = true;
							lpItem->m_Durability = NewDur;

							//	continue;

						}
					}
					else
					{
						GCItemObjectDurSend(Obj, pos, Obj.pntInventory[pos]->m_Durability, 0);

					}
					return;
				}
				else
				{

				}
			}
			if (lpItem->m_Type == ITEMGET(12, 144)) // Mithril
			{
				for (int j = 0; j < MAIN_INVENTORY_SIZE; j++)
				{
					BYTE btMainAtribute = lpItem->m_BonusSocketOption & 0x0F;
					int iInventoryPos = g_PentagramSystem.CheckOverlapMythrilPiece(Obj, lpItem->m_Type, btMainAtribute);

					if (!MAIN_INVENTORY_RANGE(iInventoryPos))
					{
						break;
					}

					int Dur = Obj.pntInventory[iInventoryPos]->m_Durability + lpItem->m_Durability;

					if (Dur <= IsOverlapItem(lpItem->m_Type) && MapC[map_num].ItemGive(Obj, item_num, 1) == TRUE)
					{
						BYTE NewOption[MAX_EXOPTION_SIZE];

						::ItemIsBufExOption(NewOption, (lpItem != NULL) ? (CItemObject*)&lpItem->m_Number : NULL);

						pResult.result = -3;
						IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

						Obj.pntInventory[iInventoryPos]->m_Durability += lpItem->m_Durability;

						if (Obj.pntInventory[iInventoryPos]->m_Durability < IsOverlapItem(lpItem->m_Type))
						{
							this->GCItemObjectDurSend(Obj, iInventoryPos, Obj.pntInventory[iInventoryPos]->m_Durability, 0);
							return;
						}

						int NewDur = Obj.pntInventory[iInventoryPos]->m_Durability - IsOverlapItem(lpItem->m_Type);
						::gObjInventoryItemSet(Obj, iInventoryPos, -1);
						Obj.pntInventory[iInventoryPos]->Clear();
						this->GCInventoryItemDeleteSend(Obj, iInventoryPos, 1);

						if (lpItem->m_Type == ITEMGET(12, 144))
						{
							ItemCreate(Obj, 233, Obj.X, Obj.Y, ItemGetNumberMake(12, 145), 0, 0, 0, 0, 0,
								Obj, 0, 0, 0, 0, lpItem->m_BonusSocketOption);

							sLog->outBasic("[Pentagram] [%s][%s] Make Mythril (Left:%d)", Obj.AccountID, Obj.Name, NewDur);
						}

						if (NewDur <= 0)
							return;

						lpItem->m_State = 2;
						lpItem->Give = 0;
						lpItem->live = 1;
						lpItem->m_Durability = NewDur;
					}
				}
			}

			//============
			if (lpItem->m_Type == ITEMGET(14, 290)) // Elemental Powder
			{
				int pos = gObjOverlapItemUsingDur((lpItem) ? ((CItemObject *)&lpItem->m_Number) : NULL, map_num, item_num, Obj, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 290), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
					Obj.pntInventory[pos]->m_Durability += lpItem->m_Durability;

					if (Obj.pntInventory[pos]->m_Durability >= IsOverlapItem(lpItem->m_Type))
					{
						int NewDur = Obj.pntInventory[pos]->m_Durability - IsOverlapItem(lpItem->m_Type);
						::gObjInventoryItemSet(Obj, pos, -1);
						Obj.pntInventory[pos]->Clear();
						this->GCInventoryItemDeleteSend(Obj, pos, 1);
						::ItemCreate(Obj, 235, Obj.X, Obj.Y, ItemGetNumberMake(14, 293), lpItem->m_Level, 0, 0, 0, 0, Obj, 0, 0, 0, 0, 0);

						if (NewDur > 0)
						{
							lpItem->m_State = 2;
							lpItem->Give = false;
							lpItem->live = true;
							lpItem->m_Durability = NewDur;

							//	continue;

						}
					}
					else
					{
						GCItemObjectDurSend(Obj, pos, Obj.pntInventory[pos]->m_Durability, 0);

					}
					return;
				}
				else
				{

				}
			}

			if (lpItem->m_Type == ITEMGET(13, 145)) // Spirit Map
			{
				int pos = gObjOverlapItemUsingDur((lpItem) ? ((CItemObject *)&lpItem->m_Number) : NULL, map_num, item_num, Obj, IsOverlapItem(lpItem->m_Type), ITEMGET(13, 145), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
					Obj.pntInventory[pos]->m_Durability += lpItem->m_Durability;

					if (Obj.pntInventory[pos]->m_Durability >= IsOverlapItem(lpItem->m_Type))
					{
						int NewDur = Obj.pntInventory[pos]->m_Durability - IsOverlapItem(lpItem->m_Type);
						::gObjInventoryItemSet(Obj, pos, -1);
						Obj.pntInventory[pos]->Clear();
						this->GCInventoryItemDeleteSend(Obj, pos, 1);
						::ItemCreate(Obj, 235, Obj.X, Obj.Y, ItemGetNumberMake(13, 146),
							lpItem->m_Level, 0, 0, 0, 0, Obj, 0, 0, 0, 0, 0);

						if (NewDur > 0)
						{
							lpItem->m_State = 2;
							lpItem->Give = false;
							lpItem->live = true;
							lpItem->m_Durability = NewDur;

							//	continue;

						}
					}
					else
					{
						GCItemObjectDurSend(Obj, pos, Obj.pntInventory[pos]->m_Durability, 0);

					}
					return;
				}
				else
				{

				}
			}
			if (lpItem->m_Type == ITEMGET(14, 101)) // Imperial Fort
			{
				int pos = gObjOverlapItemUsingDur((lpItem) ? ((CItemObject *)&lpItem->m_Number) : NULL, map_num, item_num, Obj, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 101), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
					Obj.pntInventory[pos]->m_Durability += lpItem->m_Durability;

					if (Obj.pntInventory[pos]->m_Durability >= IsOverlapItem(lpItem->m_Type))
					{
						int NewDur = Obj.pntInventory[pos]->m_Durability - IsOverlapItem(lpItem->m_Type);
						::gObjInventoryItemSet(Obj, pos, -1);
						Obj.pntInventory[pos]->Clear();
						this->GCInventoryItemDeleteSend(Obj, pos, 1);
						::ItemCreate(Obj, 235, Obj.X, Obj.Y, ItemGetNumberMake(14, 102),
							lpItem->m_Level, 0, 0, 0, 0, Obj, 0, 0, 0, 0, 0);

						if (NewDur > 0)
						{
							lpItem->m_State = 2;
							lpItem->Give = false;
							lpItem->live = true;
							lpItem->m_Durability = NewDur;

							//	continue;

						}
					}
					else
					{
						GCItemObjectDurSend(Obj, pos, Obj.pntInventory[pos]->m_Durability, 0);

					}
					return;
				}
				else
				{

				}
			}

			if (lpItem->m_Type == ITEMGET(14, 100)) // Lucky Coin
			{
				int pos = gObjOverlapItemUsingDur((lpItem) ? ((CItemObject *)&lpItem->m_Number) : NULL, map_num, item_num, Obj, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 100), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
					Obj.pntInventory[pos]->m_Durability += lpItem->m_Durability;

					GCItemObjectDurSend(Obj, pos, Obj.pntInventory[pos]->m_Durability, 0);

					return;
				}
				else
				{

				}
			}

			if (lpItem->m_Type == ITEMGET(14, 21) && lpItem->m_Level == 3) // Sign of Lord
			{
				for (int n = 0; n < MAIN_INVENTORY_SIZE; n++)
				{
					int pos = ::g_CastleSiegeSync.CheckOverlapCsMarks(Obj);

					if (MAIN_INVENTORY_RANGE(pos))
					{
						int Dur = Obj.pntInventory[pos]->m_Durability + lpItem->m_Durability;

						if (Dur <= IsOverlapItem(lpItem->m_Type))
						{
							if (MapC[map_num].ItemGive(Obj, item_num, true) == TRUE)
							{
								pResult.result = -3;
								IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
								Obj.pntInventory[pos]->m_Durability += lpItem->m_Durability;
								GCItemObjectDurSend(Obj, pos, Obj.pntInventory[pos]->m_Durability, 0);
								return;
							}
						}
						else
						{
							lpItem->m_Durability = Dur - IsOverlapItem(lpItem->m_Type);
							Obj.pntInventory[pos]->m_Durability = IsOverlapItem(lpItem->m_Type);
							GCItemObjectDurSend(Obj, pos, Obj.pntInventory[pos]->m_Durability, 0);
						}

					}
					else
					{
						break;
					}
				}
			}

			if (lpItem->m_Type == ITEMGET(14, 90)) // Golden cherry blossom
			{
				int pos = gObjOverlapItemUsingDur((lpItem) ? ((CItemObject *)&lpItem->m_Number) : NULL, map_num, item_num, Obj, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 90), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

					Obj.pntInventory[pos]->m_Durability += lpItem->m_Durability;

					GCItemObjectDurSend(Obj, pos, Obj.pntInventory[pos]->m_Durability, 0);

					return;
				}

				else
				{

				}
			}

			if (lpItem->m_Type == ITEMGET(14, 89)) // Red cherry blossom
			{
				int pos = gObjOverlapItemUsingDur((lpItem) ? ((CItemObject *)&lpItem->m_Number) : NULL, map_num, item_num, Obj, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 89), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

					Obj.pntInventory[pos]->m_Durability += lpItem->m_Durability;

					GCItemObjectDurSend(Obj, pos, Obj.pntInventory[pos]->m_Durability, 0);

					return;
				}

				else
				{

				}
			}

			if (lpItem->m_Type == ITEMGET(14, 88)) // White cherry blossom
			{
				int pos = gObjOverlapItemUsingDur((lpItem) ? ((CItemObject *)&lpItem->m_Number) : NULL, map_num, item_num, Obj, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 88), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

					Obj.pntInventory[pos]->m_Durability += lpItem->m_Durability;

					GCItemObjectDurSend(Obj, pos, Obj.pntInventory[pos]->m_Durability, 0);

					return;
				}

				else
				{

				}
			}

			if (lpItem->m_Type == ITEMGET(14, 153)) // White cherry blossom
			{
				int pos = gObjOverlapItemUsingDur((lpItem) ? ((CItemObject *)&lpItem->m_Number) : NULL, map_num, item_num, Obj, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 153), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

					Obj.pntInventory[pos]->m_Durability += lpItem->m_Durability;

					GCItemObjectDurSend(Obj, pos, Obj.pntInventory[pos]->m_Durability, 0);

					return;
				}

				else
				{

				}
			}

			if (lpItem->m_Type == ITEMGET(14, 154)) // White cherry blossom
			{
				int pos = gObjOverlapItemUsingDur((lpItem) ? ((CItemObject *)&lpItem->m_Number) : NULL, map_num, item_num, Obj, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 154), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

					Obj.pntInventory[pos]->m_Durability += lpItem->m_Durability;

					GCItemObjectDurSend(Obj, pos, Obj.pntInventory[pos]->m_Durability, 0);

					return;
				}

				else
				{

				}
			}

			if (lpItem->m_Type == ITEMGET(14, 155)) // White cherry blossom
			{
				int pos = gObjOverlapItemUsingDur((lpItem) ? ((CItemObject *)&lpItem->m_Number) : NULL, map_num, item_num, Obj, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 155), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

					Obj.pntInventory[pos]->m_Durability += lpItem->m_Durability;

					GCItemObjectDurSend(Obj, pos, Obj.pntInventory[pos]->m_Durability, 0);

					return;
				}

				else
				{

				}
			}

			if (lpItem->m_Type == ITEMGET(14, 156)) // White cherry blossom
			{
				int pos = gObjOverlapItemUsingDur((lpItem) ? ((CItemObject *)&lpItem->m_Number) : NULL, map_num, item_num, Obj, IsOverlapItem(lpItem->m_Type), ITEMGET(14, 156), 0);
				if (MAIN_INVENTORY_RANGE(pos))
				{
					pResult.result = -3;

					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

					Obj.pntInventory[pos]->m_Durability += lpItem->m_Durability;

					GCItemObjectDurSend(Obj, pos, Obj.pntInventory[pos]->m_Durability, 0);

					return;
				}

				else
				{

				}
			}

			if (g_LuckyItemManager.IsLuckyItemEquipment(lpItem->m_Type) && g_ConfigRead.data.common.AllowGetLuckyItemGround == false)
			{
				char szSetItemName[48] = { 0 };
				int tmpSetOption = 0;
				BYTE NewOption[8] = { 0 };

				ItemIsBufExOption(NewOption, (lpItem != NULL) ? (CItemObject*)&lpItem->m_Number : NULL);

				if (gSetItemOption.IsSetItem(lpItem->m_Type))
				{
					if (lpItem->m_SetOption & 1)
					{
						tmpSetOption = 1;
					}

					else
					{
						if (lpItem->m_SetOption & 2)
						{
							tmpSetOption = 2;
						}
					}

					std::strcpy(szSetItemName, gSetItemOption.GetSetOptionName(type, tmpSetOption));
				}
				pResult.result = -1;

				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
				this->GCServerMsgStringSend(Lang.GetText(0, 519), Obj, 1);
				return;
			}

			if (lpItem->m_QuestItem == true && g_QuestExpManager.IsQuestItemAtt(lpItem->m_Type, 1) == true)
			{
				for (int n = 12; n < INVENTORY_SIZE; n++)
				{
					int pos = -1;

					if (Obj.pntInventory[n]->m_Type == lpItem->m_Type &&
						Obj.pntInventory[n]->m_Durability < IsOverlapItem(lpItem->m_Type))
					{
						pos = n;
					}

					if (INVENTORY_RANGE(pos) == true)
					{
						if (Obj.pntInventory[pos]->m_Durability + lpItem->m_Durability > IsOverlapItem(lpItem->m_Type))
						{
							lpItem->m_Durability = Obj.pntInventory[pos]->m_Durability + lpItem->m_Durability - IsOverlapItem(lpItem->m_Type);
							Obj.pntInventory[pos]->m_Durability = IsOverlapItem(lpItem->m_Type);
							this->GCItemObjectDurSend(Obj, pos, Obj.pntInventory[pos]->m_Durability, 0);
						}

						else
						{
							if (MapC[map_num].ItemGive(Obj, item_num, 1) == TRUE)
							{
								pResult.result = -3;
								IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

								Obj.pntInventory[pos]->m_Durability += lpItem->m_Durability;
								GCItemObjectDurSend(Obj, pos, Obj.pntInventory[pos]->m_Durability, 0);
								return;
							}
						}
					}
				}
			}

			pResult.result = ::gObjInventoryInsertItemTemp(Obj, *lpItem);

			if (pResult.result != 0xFF)
			{
				ItemByteConvert((BYTE*)&pResult.Data[0], lpItem->m_Type, lpItem->m_Option1, lpItem->m_Option2,
					lpItem->m_Option3, lpItem->m_Level, lpItem->m_Durability, lpItem->m_NewOption,
					lpItem->m_SetOption, lpItem->m_JewelOfHarmonyOption, lpItem->m_ItemOptionEx, lpItem->m_SocketOption, lpItem->m_BonusSocketOption, 0);
				pResult.i = ID;
				pResult.ii = ID1;
				pResult.x = 0xFF;
				pResult.xx = 0x0C;
				if (MapC[map_num].ItemGive(Obj, item_num, false) == TRUE)
				{
					BYTE pos = ::gObjInventoryInsertItem(Obj, lpItem);

					if (pos == 0xFF)
						pResult.result = -1;

					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

					if (pos == 0xFF)
					{
						::GCItemObjectListSend(Obj);
						g_PeriodItemEx.OnRequestPeriodItemList(&Obj);
					}
					else
					{
						BYTE NewOption[MAX_EXOPTION_SIZE];

						::ItemIsBufExOption(NewOption, (lpItem != NULL) ? (CItemObject*)&lpItem->m_Number : NULL);

						this->GCSendGetItemInfoForParty(Obj, lpItem);

						if (BC_MAP_RANGE(map_num))
						{
							int iBridgeIndex = g_BloodCastle.GetBridgeIndex(map_num);
							if (g_BloodCastle.CheckQuestItemSerial(iBridgeIndex, lpItem))
							{
								g_BloodCastle.CatchQuestItemByUser(iBridgeIndex, Obj, lpItem->m_Level);
							}
						}

						if (IT_MAP_RANGE(map_num) && lpItem->m_Type == ITEMGET(14, 223))
						{
							if (g_ConfigRead.server.GetServerType() == SERVER_NORMAL)
							{
								g_IT_Event.SetRelicsInventoryPos(Obj.MapNumber, Obj, pos);
								g_IT_Event.SendRelicsUserInfo(&Obj);

							}
						}

						if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
						{
							if (Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
							{
								g_ChaosCastleFinal.SetUBFGetReward(Obj, lpItem->m_Type, lpItem->m_Number, pos);
							}

							else if (Obj.MapNumber == MAP_INDEX_DEVILSQUARE_FINAL)
							{
								g_DevilSquareFinal.SetUBFGetReward(Obj, lpItem->m_Type, lpItem->m_Number, pos);
							}

							else if (CC_MAP_RANGE(Obj.MapNumber) == TRUE)
							{
								g_ChaosCastle.SetUBFGetReward(Obj, lpItem->m_Type, lpItem->m_Number, pos);
							}
						}

						if (g_ArcaBattle.IsArcaBattleServer() == TRUE && lpItem->m_Type == ITEMGET(13, 147))
						{
							g_ArcaBattle.BootyItemGetCnt(Obj);
						}

						if (lpItem->m_PeriodItemDuration > 0)
						{
							g_PeriodItemEx.SetPeriodItemInfo(Obj, lpItem->m_Type, lpItem->m_Number, lpItem->m_PeriodItemDuration);

							this->GCInventoryItemOneSend(Obj, pos);
							g_PeriodItemEx.SendPeriodItemInfoOnce(Obj, Obj.pntInventory[pos]);
						}
					}
				}
				else
				{
					pResult.result = -1;
					pResult.h.size -= sizeof(pResult.Data);

					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
				}
			}
			else
			{
				pResult.result = -1;
				pResult.h.size -= sizeof(pResult.Data);

				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
			}
		}
	}
	else
	{
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);
	}
}

bool GameProtocol::CGItemDropRequest(PMSG_ITEMTHROW * lpMsg, CGameObject &Obj, BOOL drop_type)
{
	BOOL CopyItem = FALSE;
	BYTE NewOption[MAX_EXOPTION_SIZE];

	if (!PacketCheckTime(Obj))
		return FALSE;

	if (Obj.Type != OBJ_USER)
	{
		return FALSE;
	}

	PMSG_ITEMTHROW_RESULT pResult;

	PHeadSetB((BYTE*)&pResult, 0x23, sizeof(pResult));
	pResult.Result = true;
	pResult.Ipos = lpMsg->Ipos;

	if (Obj.m_IfState->use != 0)
	{
		if (Obj.m_IfState->type != 3)
		{
			pResult.Result = false;
			return FALSE;
		}
	}

	if (Obj.m_PlayerData->m_bSecurityCheck == false)
	{
		MsgOutput(Obj, Lang.GetText(0, 822));
		pResult.Result = false;
	}

	if (Obj.CloseType != -1)
		pResult.Result = false;

	if (Obj.m_bMapSvrMoveQuit == true)
	{
		pResult.Result = false;
	}

	if (!::gObjFixInventoryPointer(Obj))

		if (Obj.pTransaction == 1)
		{
			pResult.Result = false;

			return false;
		}

	

	if (lpMsg->Ipos == 0xFF)	// Null Position
	{
		pResult.Result = false;
	}

	if (lpMsg->Ipos < 0 || lpMsg->Ipos > MAIN_INVENTORY_SIZE - 1)	// Out of Bounds
		pResult.Result = false;

	if (!gObjCanItemTouch(Obj, 0))
		pResult.Result = false;

	if (!IsDumpItem(Obj.pntInventory[lpMsg->Ipos]->m_Type) && (Obj.pntInventory[lpMsg->Ipos]->m_Type != ITEMGET(13, 20) || !Obj.pntInventory[lpMsg->Ipos]->m_Level))
		pResult.Result = false;

	if (g_LuckyItemManager.IsLuckyItemEquipment(Obj.pntInventory[lpMsg->Ipos]->m_Type) || g_LuckyItemManager.IsLuckyItemTicket(Obj.pntInventory[lpMsg->Ipos]->m_Type))
	{
		pResult.Result = false;
	}

	// Icarus Preventions about Dinorant
	if (Obj.MapNumber == MAP_INDEX_ICARUS)
	{
		if (lpMsg->Ipos == Obj.m_btInvenPetPos)
		{
			if (Obj.pntInventory[lpMsg->Ipos]->m_Type == ITEMGET(13, 3)) // Dinorant
			{
				if (Obj.pntInventory[7]->IsItem() == FALSE)
				{
					pResult.Result = false;
				}
			}
		}
		else if (lpMsg->Ipos == 7)
		{
			if (Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type != ITEMGET(13, 3)) // Dinorant 
			{
				pResult.Result = false;
			}
		}
	}

	if (Obj.pntInventory[lpMsg->Ipos]->m_Type == ITEMGET(12, 30) ||
		Obj.pntInventory[lpMsg->Ipos]->m_Type == ITEMGET(12, 31) ||
		Obj.pntInventory[lpMsg->Ipos]->m_Type == ITEMGET(13, 36) ||
		Obj.pntInventory[lpMsg->Ipos]->m_Type == ITEMGET(13, 37))
	{
		pResult.Result = false;
	}

	if (GetWingType(Obj.pntInventory[lpMsg->Ipos]->m_Type, WCF_ITEMNUMBER) != NONE_WING && g_ConfigRead.pk.bPkPenaltyDisable == FALSE)
	{
		pResult.Result = false;
	}
	if (g_ConfigRead.data.common.joinmuRemoveItemSellDropRestriction == false)
	{
		if ((Obj.pntInventory[lpMsg->Ipos]->m_Type >= ITEMGET(13, 0) && Obj.pntInventory[lpMsg->Ipos]->m_Type <= ITEMGET(13, 3))
			|| Obj.pntInventory[lpMsg->Ipos]->m_Type == ITEMGET(14, 13) ||
			Obj.pntInventory[lpMsg->Ipos]->m_Type == ITEMGET(14, 14) ||
			Obj.pntInventory[lpMsg->Ipos]->m_Type == ITEMGET(14, 16) ||
			(Obj.pntInventory[lpMsg->Ipos]->m_Type >= ITEMGET(12, 0)
				&& Obj.pntInventory[lpMsg->Ipos]->m_Type <= ITEMGET(12, 6)) ||
				(Obj.pntInventory[lpMsg->Ipos]->m_Type >= ITEMGET(12, 36)
					&& Obj.pntInventory[lpMsg->Ipos]->m_Type <= ITEMGET(12, 40))
			|| Obj.pntInventory[lpMsg->Ipos]->m_Type == ITEMGET(12, 15)
			|| (Obj.pntInventory[lpMsg->Ipos]->m_Level > g_ConfigRead.pk.iPkMaxLevelItemDrop
				&& Obj.pntInventory[lpMsg->Ipos]->m_Type < ITEMGET(12, 0))
			|| (Obj.pntInventory[lpMsg->Ipos]->IsSetItem() != FALSE && !g_ConfigRead.pk.bPkDropExpensiveItems)
			|| (Obj.pntInventory[lpMsg->Ipos]->IsExtItem() != FALSE && !g_ConfigRead.pk.bPkDropExpensiveItems))
		{
			if (g_ConfigRead.pk.bPkPenaltyDisable == FALSE)
			{
				pResult.Result = false;
			}
		}
	}
	if (g_kJewelOfHarmonySystem.IsStrengthenByJewelOfHarmony(Obj.pntInventory[lpMsg->Ipos]) == TRUE)
	{
		pResult.Result = false;
		this->GCServerMsgStringSend(Lang.GetText(0, 272), Obj, 1);
	}

	if (g_kJewelOfHarmonySystem.IsJewelOfHarmonyOriginal(Obj.pntInventory[lpMsg->Ipos]->m_Type) ||
		g_kJewelOfHarmonySystem.IsJewelOfHarmonyPurity(Obj.pntInventory[lpMsg->Ipos]->m_Type) ||
		g_kJewelOfHarmonySystem.IsJewelOfHarmonySmeltingItems(Obj.pntInventory[lpMsg->Ipos]->m_Type))
	{
		pResult.Result = false;
	}

	if (Obj.pntInventory[lpMsg->Ipos]->m_Type == ITEMGET(13, 39))
	{
		pResult.Result = false;
	}

	if (Obj.pntInventory[lpMsg->Ipos]->IsPeriodItem() == TRUE)
	{
		pResult.Result = false;
	}

	if (g_NewPVP.IsDuel(*lpObj))
	{
		pResult.Result = false;
	}

	if (IsJumpingEventItem(Obj.pntInventory[lpMsg->Ipos]->m_Type) == TRUE)
	{
		pResult.Result = false;
	}

	if (pResult.Result == 1)
	{
		::ItemIsBufExOption(NewOption, Obj.pntInventory[lpMsg->Ipos]);

		int map = Obj.MapNumber;
		int type = Obj.pntInventory[lpMsg->Ipos]->m_Type;
		int level = Obj.pntInventory[lpMsg->Ipos]->m_Level;
		float dur = Obj.pntInventory[lpMsg->Ipos]->m_Durability;
		BOOL bIsItem = Obj.pntInventory[lpMsg->Ipos]->IsItem();
		BYTE Option1 = Obj.pntInventory[lpMsg->Ipos]->m_Option1;
		BYTE Option2 = Obj.pntInventory[lpMsg->Ipos]->m_Option2;
		BYTE Option3 = Obj.pntInventory[lpMsg->Ipos]->m_Option3;
		BYTE NOption = Obj.pntInventory[lpMsg->Ipos]->m_NewOption;
		UINT64 serial = Obj.pntInventory[lpMsg->Ipos]->m_Number;
		int PetLevel = Obj.pntInventory[lpMsg->Ipos]->m_PetItem_Level;
		UINT64 PetExp = Obj.pntInventory[lpMsg->Ipos]->m_PetItem_Exp;
		BYTE SOption = Obj.pntInventory[lpMsg->Ipos]->m_SetOption;
		BYTE btItemEffectEx = Obj.pntInventory[lpMsg->Ipos]->m_ItemOptionEx;
		UINT64 serial2 = Obj.pntInventory[lpMsg->Ipos]->m_Number;
		char szItemName[50] = "Item";
		BYTE JOHOption = g_kJewelOfHarmonySystem.GetItemStrengthenOption(Obj.pntInventory[lpMsg->Ipos]);
		BYTE JOHOptionLevel = g_kJewelOfHarmonySystem.GetItemOptionLevel(Obj.pntInventory[lpMsg->Ipos]);

		//Season 4 add-on
		BYTE SocketOption[5];
		BYTE SocketIndex;
		g_SocketOptionSystem.GetSocketOption(Obj.pntInventory[lpMsg->Ipos], SocketOption, SocketIndex);

		for (int iOption = 0; iOption < 5; iOption++)
		{
			SocketOption[iOption] = Obj.pntInventory[lpMsg->Ipos]->m_SocketOption[iOption];
		}

		if (gObjCheckSerial0ItemList(Obj.pntInventory[lpMsg->Ipos]) != FALSE)
		{
			MsgOutput(Obj, Lang.GetText(0, 259));

			CopyItem = TRUE;
			pResult.Result = false;
			bIsItem = FALSE;
		}

		if (!::gObjInventorySearchSerialNumber(lpObj, serial2))	// ANTI DUPE SYSTEM
		{
			CopyItem = TRUE;
			pResult.Result = false;
			bIsItem = FALSE;
		}

		if (bIsItem == TRUE)
		{
			std::strcpy(szItemName, Obj.pntInventory[lpMsg->Ipos]->GetName());

			if (g_BagManager.SearchAndUseBag(Obj, BAG_COMMON, type, level) == true)
			{
				gObjInventoryDeleteItem(Obj, lpMsg->Ipos);
			}
			else if (type == ITEMGET(14, 51))
			{
				gObjInventoryDeleteItem(Obj, lpMsg->Ipos);
				FireworksOpenEven(&Obj);
			}
			else if (type == ITEMGET(14, 63))
			{
				gObjInventoryDeleteItem(Obj, lpMsg->Ipos);
				FireworksOpenEven(&Obj);

			}
			else if (type == ITEMGET(14, 210))
			{
				if (NewYearSummonMonster(lpObj, 647) == FALSE)
				{
					pResult.Result = false;
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}

				gObjInventoryDeleteItem(Obj, lpMsg->Ipos);
			}

			else if (type == ITEMGET(14, 211))
			{
				if (NewYearSummonMonster(lpObj, 648) == FALSE)
				{
					pResult.Result = false;
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}

				gObjInventoryDeleteItem(Obj, lpMsg->Ipos);
			}

			else if (type == ITEMGET(14, 212))
			{
				if (NewYearSummonMonster(lpObj, 649) == FALSE)
				{
					pResult.Result = false;
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}

				gObjInventoryDeleteItem(Obj, lpMsg->Ipos);
			}

			else if (type == ITEMGET(14, 213))
			{
				if (NewYearSummonMonster(lpObj, 650) == FALSE)
				{
					pResult.Result = false;
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}

				gObjInventoryDeleteItem(Obj, lpMsg->Ipos);
			}

			else if (type == ITEMGET(14, 217))
			{
				if (SummonGoldColossusMonster(lpObj, 653, 1, 0) == FALSE)
				{
					pResult.Result = false;
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}

				gObjInventoryDeleteItem(Obj, lpMsg->Ipos);
			}

			else if (type == ITEMGET(14, 218))
			{
				if (SummonGoldColossusMonster(lpObj, 654, 1, 0) == FALSE)
				{
					pResult.Result = false;
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}

				gObjInventoryDeleteItem(Obj, lpMsg->Ipos);
			}

			else if (type == ITEMGET(14, 219))
			{
				if (SummonGoldColossusMonster(lpObj, 655, 1, 0) == FALSE)
				{
					pResult.Result = false;
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}

				gObjInventoryDeleteItem(Obj, lpMsg->Ipos);
			}

			else if (type == ITEMGET(14, 220))
			{
				if (SummonGoldColossusMonster(lpObj, 656, 1, 0) == FALSE)
				{
					pResult.Result = false;
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}

				gObjInventoryDeleteItem(Obj, lpMsg->Ipos);
			}

			else if (type == ITEMGET(14, 221))
			{
				if (SummonGoldColossusMonster(lpObj, 657, 1, 0) == FALSE)
				{
					pResult.Result = false;
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}

				SummonGoldColossusMonster(lpObj, 654, 5, 1);
				gObjInventoryDeleteItem(Obj, lpMsg->Ipos);
			}

			else if (type == ITEMGET(14, 234))
			{
				if (SummonPetEventMonster(lpObj) == FALSE)
				{
					pResult.Result = false;
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}

				gObjInventoryDeleteItem(Obj, lpMsg->Ipos);
			}
			else if (type == ITEMGET(14, 286))
			{
				gObjInventoryDeleteItem(Obj, lpMsg->Ipos);
				pResult.Result = true;

				BYTE SocketBonus = (Obj.m_iPentagramMainAttribute == ELEMENT_NONE) ? (1 + rand() % 5) : Obj.m_iPentagramMainAttribute;
				BYTE SocketOption[5];
				BYTE btSocketOption[5];

				SocketOption[0] = -1;
				SocketOption[1] = -1;
				SocketOption[2] = -1;
				SocketOption[3] = -1;
				SocketOption[4] = -1;

				int scount = 4;
				int count = 0;

				do
				{
					int slot = rand() % 4;

					if (SocketOption[slot] == (BYTE)-1)
					{
						SocketOption[slot] = 0xFE;
						count++;
					}
				} while (scount != count);

				for (int k = 0; k < 5; k++)
				{
					if (k)
					{
						btSocketOption[k] = -1;
					}

					else
					{
						btSocketOption[k] = 1;
					}
				}

				ItemCreate(Obj, Obj.MapNumber, Obj.X, Obj.Y, ITEMGET(12, 215), 0, 0, 0, 0, 0, Obj.m_Index, 0, 0, 0, SocketOption, SocketBonus);
				ItemCreate(Obj, Obj.MapNumber, Obj.X, Obj.Y, ITEMGET(12, 222), 0, 0, 0, 0, 0, Obj.m_Index, 0, 0, 0, btSocketOption, SocketBonus);
				ItemCreate(Obj, Obj.MapNumber, Obj.X, Obj.Y, ITEMGET(12, 232), 0, 0, 0, 0, 0, Obj.m_Index, 0, 0, 0, btSocketOption, SocketBonus);
				ItemCreate(Obj, Obj.MapNumber, Obj.X, Obj.Y, ITEMGET(12, 242), 0, 0, 0, 0, 0, Obj.m_Index, 0, 0, 0, btSocketOption, SocketBonus);
				ItemCreate(Obj, Obj.MapNumber, Obj.X, Obj.Y, ITEMGET(12, 252), 0, 0, 0, 0, 0, Obj.m_Index, 0, 0, 0, btSocketOption, SocketBonus);
			}

			else if (type == ITEMGET(12, 26))
			{
				switch (level)
				{
				case 0:
				{
					gObjInventoryDeleteItem(Obj, lpMsg->Ipos);
					pResult.Result = true;
					int ltype = ITEMGET(12, 26);
					int randor = rand() % 100;
					int Level;

					if (randor < 10)
					{
						Level = 3;
					}
					else if (randor < 45)
					{
						Level = 1;
					}
					else
					{
						Level = 2;
					}

					ItemCreate(Obj, Obj.MapNumber, Obj.X, Obj.Y,
						ltype, Level, 1, 0, 0, 0, Obj, 0, 0, 0, 0, 0);

				}
				break;
				case 1: case 2: case 3:
				{
					gObjInventoryDeleteItem(Obj, lpMsg->Ipos);
					pResult.Result = true;

					PMSG_SERVERCMD ServerCmd;

					PHeadSubSetB((BYTE*)&ServerCmd, 0xF3, 0x40, sizeof(ServerCmd));
					ServerCmd.CmdType = 0;
					ServerCmd.X = Obj.X;
					ServerCmd.Y = Obj.Y;

					MsgSendV2(lpObj, (BYTE*)&ServerCmd, sizeof(ServerCmd));
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&ServerCmd, sizeof(ServerCmd));

				}
				break;
				case 4:

					break;
				case 5:
					gObjInventoryDeleteItem(Obj, lpMsg->Ipos);
					pResult.Result = true;

					if ((rand() % 10000) < g_iHiddenTreasureBoxOfflineRate)
					{
						EGReqRegHTOfflineGift(Obj);
					}
					else
					{
						int money = 1000000;
						MapC[Obj.MapNumber].MoneyItemDrop(money, Obj.X, Obj.Y);

					}
					break;
				}
			}
			else if (type == ITEMGET(13, 20) && level == 1) // Wizard Ring A
			{
				if (Obj.Level >= 40)
				{
					::gObjInventoryDeleteItem(Obj, lpMsg->Ipos);
					g_BagManager.SearchAndUseBag(Obj, BAG_EVENT, EVENTBAG_WARRIORRING_1, Obj.m_Index);
				}
				else
				{
					PMSG_NOTICE pNotice;

					TNotice::MakeNoticeMsg((TNotice *)&pNotice, 10, Lang.GetText(0, 106));
					TNotice::SetNoticeProperty((TNotice *)&pNotice, 10, _ARGB(255, 128, 149, 196), 1, 0, 20);
					TNotice::SendNoticeToUser(Obj, (TNotice *)&pNotice);
					pResult.Result = false;
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}
			}
			else if (type == ITEMGET(13, 20) && level == 2) // Wizard Ring B
			{
				if (Obj.Level >= 80)
				{
					::gObjInventoryDeleteItem(Obj, lpMsg->Ipos);
					g_BagManager.SearchAndUseBag(Obj, BAG_EVENT, EVENTBAG_WARRIORRING_2, Obj.m_Index);
				}
				else
				{
					PMSG_NOTICE pNotice;

					TNotice::MakeNoticeMsg((TNotice *)&pNotice, 10, Lang.GetText(0, 109));
					TNotice::SetNoticeProperty((TNotice *)&pNotice, 10, _ARGB(255, 128, 149, 196), 1, 0, 20);
					TNotice::SendNoticeToUser(Obj, (TNotice *)&pNotice);
					pResult.Result = false;
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}
			}
			else if (type == ITEMGET(14, 28)) // Lost Map
			{
				if (g_KalimaGate.CreateKalimaGate(Obj, level, lpMsg->px, lpMsg->py))
				{
					gObjInventoryDeleteItem(Obj, lpMsg->Ipos);
				}
				else
				{
					pResult.Result = false;
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}
			}
			else if (type == ITEMGET(14, 64) && drop_type != 1)
			{
				pResult.Result = false;
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
				return FALSE;
			}
			else if (type == ITEMGET(14, 20) && (((level - 1 < 0) ? FALSE : (level - 1 > 4) ? FALSE : TRUE) != FALSE)) // Remedy Of Love
			{
				gObjInventoryDeleteItem(Obj, lpMsg->Ipos);
			}
			else if (type == ITEMGET(13, 7)) // Contract (Summon)
			{
				int iMonsterID = 0;

				if (level == 0)
					iMonsterID = 286;
				else if (level == 1)
					iMonsterID = 287;

				if (g_CsNPC_Mercenary.CreateMercenary(Obj, iMonsterID, lpMsg->px, lpMsg->py))
				{
					::gObjInventoryDeleteItem(Obj, lpMsg->Ipos);
				}
				else
				{
					pResult.Result = false;
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}
			}
			else if (type == ITEMGET(13, 11)) // Guardian / LifeStone
			{
				BOOL bResult = FALSE;

				if (level == 0)
					bResult = g_CsNPC_Guardian.CreateGuardian(Obj);

				else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && level == 1)
					bResult = g_CsNPC_LifeStone.CreateLifeStone(Obj);

				if (bResult == TRUE)
					::gObjInventoryDeleteItem(Obj, lpMsg->Ipos);
				else
				{
					pResult.Result = false;
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}
			}

			else if ((type >= ITEMGET(14, 236) && type <= ITEMGET(14, 241)) && level == 0)
			{
				gObjInventoryDeleteItem(Obj, lpMsg->Ipos);

				switch (type)
				{
				case ITEMGET(14, 236):
					g_BagManager.SearchAndUseBag(Obj, BAG_EVENT, EVENTBAG_ITR_1, Obj.m_Index);
					break;
				case ITEMGET(14, 237):
					g_BagManager.SearchAndUseBag(Obj, BAG_EVENT, EVENTBAG_ITR_2, Obj.m_Index);
					break;
				case ITEMGET(14, 238):
					g_BagManager.SearchAndUseBag(Obj, BAG_EVENT, EVENTBAG_ITR_3, Obj.m_Index);
					break;
				case ITEMGET(14, 239):
					g_BagManager.SearchAndUseBag(Obj, BAG_EVENT, EVENTBAG_ITR_4, Obj.m_Index);
					break;
				case ITEMGET(14, 240):
					g_BagManager.SearchAndUseBag(Obj, BAG_EVENT, EVENTBAG_ITR_5, Obj.m_Index);
					break;
				case ITEMGET(14, 241):
					g_BagManager.SearchAndUseBag(Obj, BAG_EVENT, EVENTBAG_ITR_6, Obj.m_Index);
					break;
				}

			}

			else
			{

				int OwnerIndex = Obj.m_Index;

				if ((type >= ITEMGET(14, 0) && type <= ITEMGET(14, 10)) || (type == ITEMGET(4, 7)) || type == ITEMGET(4, 15) || type == ITEMGET(14, 64))
					OwnerIndex = -1;

				if (MapC[map].ItemDrop(type, level, dur, lpMsg->px, lpMsg->py, Option1, Option2, Option3,
					NOption, SOption, serial2, OwnerIndex, PetLevel, PetExp, btItemEffectEx, SocketOption, SocketIndex, 0) == TRUE)
				{
					gObjInventoryDeleteItem(Obj, lpMsg->Ipos);
					pResult.Result = true;

					if (drop_type == FALSE)
					{

					}
					else
					{

					}
				}
				else
				{
					pResult.Result = false;
				}
			}
		}
		else	// bIsItem
		{
			pResult.Result = false;
		}
	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

	if (CopyItem != FALSE)
	{
		::GCItemObjectListSend(Obj);
		g_PeriodItemEx.OnRequestPeriodItemList(lpObj);
	}

	if (pResult.Result == 1)
	{
		if (lpMsg->Ipos < 12)
		{
			if (lpMsg->Ipos == 10 || lpMsg->Ipos == 11)
			{
				gObjUseSkill.SkillChangeUse(Obj);
			}

			::gObjMakePreviewCharSet(Obj);

			PMSG_USEREQUIPMENTCHANGED pChange;

			PHeadSetB((BYTE*)&pChange, 0x25, sizeof(pChange));
			pChange.NumberH = SET_NUMBERH(Obj.m_Index);
			pChange.NumberL = SET_NUMBERL(Obj.m_Index);
			ItemByteConvert(pChange.ItemInfo, *Obj.pntInventory[lpMsg->Ipos]);
			pChange.ItemInfo[I_OPTION] = lpMsg->Ipos << 4;
			pChange.ItemInfo[I_OPTION] |= LevelSmallConvert(Obj, lpMsg->Ipos) & 0x0F;
			pChange.Element = Obj.m_iPentagramMainAttribute;
			if (pChange.ItemInfo[5] == 0xF0)
				pChange.ItemInfo[5] = 0xF1;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pChange, pChange.h.size);
			MsgSendV2(lpObj, (BYTE*)&pChange, pChange.h.size);
		}
	}

	return pResult.Result;
}

void GameProtocol::GCItemObjectMoveResultSend(CGameObject &Obj, BYTE result, BYTE pos, BYTE* const ItemInfo)
{
	PMSG_INVENTORYITEMMOVE_RESULT pMsg;

	pMsg.h.c = 0xC3;
	pMsg.h.headcode = 0x24;
	pMsg.h.size = sizeof(pMsg);
	pMsg.result = result;
	pMsg.Pos = pos;
	pMsg.ItemInfo[I_TYPE] = ItemInfo[I_TYPE];
	pMsg.ItemInfo[I_OPTION] = ItemInfo[I_OPTION];
	pMsg.ItemInfo[I_DUR] = ItemInfo[I_DUR];
	pMsg.ItemInfo[I_NOPTION] = ItemInfo[I_NOPTION];
	pMsg.ItemInfo[I_SOPTION] = ItemInfo[I_SOPTION];
	pMsg.ItemInfo[I_380OPTION] = ItemInfo[I_380OPTION];
	pMsg.ItemInfo[I_JOHOPTION] = ItemInfo[I_JOHOPTION];
	pMsg.ItemInfo[I_SOCKET1] = ItemInfo[I_SOCKET1];
	pMsg.ItemInfo[I_SOCKET2] = ItemInfo[I_SOCKET2];
	pMsg.ItemInfo[I_SOCKET3] = ItemInfo[I_SOCKET3];
	pMsg.ItemInfo[I_SOCKET4] = ItemInfo[I_SOCKET4];
	pMsg.ItemInfo[I_SOCKET5] = ItemInfo[I_SOCKET5];

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::CGInventoryItemMove(PMSG_INVENTORYITEMMOVE * lpMsg, CGameObject &Obj)
{
	BYTE result;
	BYTE ItemInfo[MAX_ITEM_INFO];

	if (!PacketCheckTime(Obj))
	{
		this->GCItemObjectMoveResultSend(Obj, -1, 0, ItemInfo);
		return;
	}

	if (!::gObjIsConnectedGP(Obj))
	{
		this->GCItemObjectMoveResultSend(Obj, -1, 0, ItemInfo);
		return;
	}

	if (Obj.m_PlayerData->m_bSecurityCheck == false)
	{
		MsgOutput(Obj, Lang.GetText(0, 822));
		this->GCItemObjectMoveResultSend(Obj, -1, 0, ItemInfo);
		return;
	}

	
	int it_type = 0;

	if (Obj.m_PlayerData->m_ReqWarehouseOpen != false)
	{
		this->GCItemObjectMoveResultSend(Obj, -1, 0, (BYTE*)&ItemInfo);

		return;
	}

	int source = lpMsg->source_item_num;
	int target = lpMsg->target_item_num;

	if (lpMsg->tFlag == 2 || lpMsg->sFlag == 2)
		it_type = 6;

	if (lpMsg->tFlag == 0 && lpMsg->sFlag == 0)
		it_type = 8;

	if ((lpMsg->tFlag == 0 && lpMsg->sFlag == 4) ||
		(lpMsg->tFlag == 4 && lpMsg->sFlag == 0) ||
		(lpMsg->tFlag == 4 && lpMsg->sFlag == 4))
		it_type = 8;

	if (gObjCanItemTouch(lpObj, it_type) == FALSE)
	{
		if (lpMsg->sItemInfo[I_TYPE] == ITEMGET(4, 7) || lpMsg->sItemInfo[I_TYPE] == ITEMGET(4, 15))
		{
			if (target >= 12 || source < 12)
			{
				this->GCItemObjectMoveResultSend(Obj, -1, 0, (BYTE*)&ItemInfo);
				return;
			}
		}
		else
		{
			this->GCItemObjectMoveResultSend(Obj, -1, 0, (BYTE*)&ItemInfo);
			return;
		}
	}

	if (Obj.DieRegen != FALSE)
	{
		this->GCItemObjectMoveResultSend(Obj, -1, 0, (BYTE*)&ItemInfo);
		return;
	}

	if (Obj.Teleport != FALSE)
	{
		this->GCItemObjectMoveResultSend(Obj, -1, 0, (BYTE*)&ItemInfo);
		return;
	}

	ItemInfo[I_TYPE] = lpMsg->sItemInfo[I_TYPE];
	ItemInfo[I_OPTION] = lpMsg->sItemInfo[I_OPTION];
	ItemInfo[I_DUR] = lpMsg->sItemInfo[I_DUR];
	ItemInfo[I_NOPTION] = lpMsg->sItemInfo[I_NOPTION];
	ItemInfo[I_SOPTION] = lpMsg->sItemInfo[I_SOPTION];
	ItemInfo[I_380OPTION] = lpMsg->sItemInfo[I_380OPTION];
	ItemInfo[I_JOHOPTION] = lpMsg->sItemInfo[I_JOHOPTION];
	ItemInfo[I_SOCKET1] = lpMsg->sItemInfo[I_SOCKET1];
	ItemInfo[I_SOCKET2] = lpMsg->sItemInfo[I_SOCKET2];
	ItemInfo[I_SOCKET3] = lpMsg->sItemInfo[I_SOCKET3];
	ItemInfo[I_SOCKET4] = lpMsg->sItemInfo[I_SOCKET4];
	ItemInfo[I_SOCKET5] = lpMsg->sItemInfo[I_SOCKET5];

	if (lpMsg->sFlag == 3 || lpMsg->tFlag == 3)
	{
		if (Obj.m_IfState->use < 1 || Obj.m_IfState->type != 7)
		{
			this->GCItemObjectMoveResultSend(Obj, -1, 0, (BYTE*)&ItemInfo);
			return;
		}

		if (Obj.ChaosLock == TRUE)
		{
			this->GCItemObjectMoveResultSend(Obj, -1, 0, (BYTE*)&ItemInfo);

			return;
		}
	}

	if (lpMsg->sFlag == 5 || lpMsg->tFlag == 5)
	{
		if (Obj.m_IfState->use < 1 || Obj.m_IfState->type != 13)
		{
			this->GCItemObjectMoveResultSend(Obj, -1, 0, (BYTE*)&ItemInfo);
			return;
		}

		if (Obj.ChaosLock == TRUE)
		{
			this->GCItemObjectMoveResultSend(Obj, -1, 0, (BYTE*)&ItemInfo);

			return;
		}
	}

	if (lpMsg->sFlag == 6 || lpMsg->sFlag == 7 || lpMsg->sFlag == 8 ||
		lpMsg->tFlag == 6 || lpMsg->tFlag == 7 || lpMsg->tFlag == 8)
	{
		if (Obj.m_IfState->use < 1 || Obj.m_IfState->type != 7)
		{
			this->GCItemObjectMoveResultSend(Obj, -1, 0, (BYTE*)&ItemInfo);
			return;
		}

		if (Obj.ChaosLock == TRUE)
		{
			this->GCItemObjectMoveResultSend(Obj, -1, 0, (BYTE*)&ItemInfo);

			return;
		}
	}
	if (lpMsg->sFlag == 9 || lpMsg->tFlag == 9)
	{
		if (Obj.m_IfState->use < 1 || Obj.m_IfState->type != 7)
		{
			this->GCItemObjectMoveResultSend(Obj, -1, 0, (BYTE*)&ItemInfo);
			return;
		}

		if (Obj.ChaosLock == TRUE)
		{
			this->GCItemObjectMoveResultSend(Obj, -1, 0, (BYTE*)&ItemInfo);

			return;
		}
	}
	if (lpMsg->sFlag == 11 || lpMsg->sFlag == 12 || lpMsg->sFlag == 13 || lpMsg->sFlag == 14 || lpMsg->sFlag == 15 || lpMsg->sFlag == 16 || lpMsg->sFlag == 17 ||
		lpMsg->tFlag == 11 || lpMsg->tFlag == 12 || lpMsg->tFlag == 13 || lpMsg->tFlag == 14 || lpMsg->tFlag == 15 || lpMsg->tFlag == 16 || lpMsg->tFlag == 17 || lpMsg->tFlag == 19)
	{
		if (Obj.m_IfState->use < 1 || Obj.m_IfState->type != 7)
		{
			this->GCItemObjectMoveResultSend(Obj, -1, 0, (BYTE*)&ItemInfo);
			return;
		}

		if (Obj.ChaosLock == TRUE)
		{
			this->GCItemObjectMoveResultSend(Obj, -1, 0, (BYTE*)&ItemInfo);

			return;
		}
	}


	if (lpMsg->sFlag == 2 || lpMsg->tFlag == 2)
	{
		if (Obj.m_IfState->use < 1 || Obj.m_IfState->type != 6)
		{
			this->GCItemObjectMoveResultSend(Obj, -1, 0, (BYTE*)&ItemInfo);
			return;
		}
	}
	if (lpMsg->sFlag == 1 && lpMsg->tFlag == 0)
	{
		if (Obj.m_IfState->use < 1 || Obj.m_IfState->type != 1)
		{
			this->GCItemObjectMoveResultSend(Obj, -1, 0, (BYTE*)&ItemInfo);
			return;
		}
	}

	if (lpMsg->sFlag == 2 && lpMsg->tFlag == 0)
	{
		int money = ::GetWarehouseUsedHowMuch(Obj.Level, Obj.WarehousePW);

		if ((Obj.m_PlayerData->Money - money) < 1 && (Obj.WarehouseMoney - money) < 1)
		{
			this->GCItemObjectMoveResultSend(Obj, -1, 0, (BYTE*)&ItemInfo);
			char szTemp[256];
			wsprintf(szTemp, Lang.GetText(0, 154), money);
			this->GCServerMsgStringSend(szTemp, Obj, 1);

			return;
		}

		if (::bCanWarehouseLock == TRUE)
		{
			if (Obj.WarehouseLock == 1)
			{
				this->GCItemObjectMoveResultSend(Obj, -1, 0, (BYTE*)&ItemInfo);
				this->GCServerMsgStringSend(Lang.GetText(0, 155), Obj, 1);

				return;
			}
		}
	}
	int type = (ItemInfo[I_TYPE] + ((ItemInfo[I_NOPTION] & 0x80) * 2)) + ((ItemInfo[I_380OPTION] & 0xF0) << 5);

	if (it_type != 8)
	{
		LPITEM_ATTRIBUTE lpItemAttr = GetItemAttr(type);

		if (lpItemAttr == NULL)
		{
			this->GCItemObjectMoveResultSend(Obj, -1, 0, (BYTE*)&ItemInfo);
			return;
		}

		if (lpItemAttr->QuestItem != false && !g_QuestExpManager.IsQuestItemAtt(type, 4))
		{
			this->GCItemObjectMoveResultSend(Obj, -1, 0, (BYTE*)&ItemInfo);
			return;
		}

		if (type == ITEMGET(14, 64)) // cursed castle water
		{
			this->GCItemObjectMoveResultSend(Obj, -1, 0, (BYTE*)&ItemInfo);
			return;
		}
	}

	if (lpMsg->sFlag == 21 && lpMsg->tFlag != 21 && lpMsg->tFlag != 1)
	{
		this->GCItemObjectMoveResultSend(Obj, -1, 0, ItemInfo);
		return;
	}

	if (lpMsg->sFlag == 1)
	{
		if (lpMsg->tFlag != 21)
		{
			if (lpMsg->tFlag != 1)
			{
				if (IsEventItem(*Obj.pntTrade[source]) == TRUE)
				{
					GCItemObjectMoveResultSend(Obj, -1, 0, ItemInfo);
					return;
				}
			}
		}
	}
	if (lpMsg->tFlag != 21)
	{
		int type = (ItemInfo[I_TYPE] + ((ItemInfo[I_NOPTION] & 0x80) * 2)) + ((ItemInfo[I_380OPTION] & 0xF0) << 5);

		if (IsEventItem(*getGameObject(type)) == TRUE)
		{
			this->GCItemObjectMoveResultSend(Obj, -1, 0, ItemInfo);
			return;
		}
	}

	if (lpMsg->sFlag == 22 && lpMsg->tFlag != 22)
	{
		this->GCItemObjectMoveResultSend(Obj, -1, 0, ItemInfo);
		return;
	}

	if ((lpMsg->sFlag == 0 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 2 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 2) ||
		(lpMsg->sFlag == 2 && lpMsg->tFlag == 2) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 3) ||

		(lpMsg->sFlag == 0 && lpMsg->tFlag == 5) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 6) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 7) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 8) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 9) ||
		(lpMsg->sFlag == 9 && lpMsg->tFlag == 9) ||
		(lpMsg->sFlag == 9 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 4) ||
		(lpMsg->sFlag == 4 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 4 && lpMsg->tFlag == 4) ||
		(lpMsg->sFlag == 5 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 5 && lpMsg->tFlag == 5) ||
		(lpMsg->sFlag == 6 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 6 && lpMsg->tFlag == 6) ||
		(lpMsg->sFlag == 7 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 7 && lpMsg->tFlag == 7) ||
		(lpMsg->sFlag == 8 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 8 && lpMsg->tFlag == 8) ||
		(lpMsg->sFlag == 3 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 3 && lpMsg->tFlag == 3) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 10) ||
		(lpMsg->sFlag == 10 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 11) ||
		(lpMsg->sFlag == 11 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 12) ||
		(lpMsg->sFlag == 12 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 13) ||
		(lpMsg->sFlag == 13 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 14) ||
		(lpMsg->sFlag == 14 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 15) ||
		(lpMsg->sFlag == 15 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 16) ||
		(lpMsg->sFlag == 16 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 17) ||
		(lpMsg->sFlag == 17 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 11 && lpMsg->tFlag == 11) ||
		(lpMsg->sFlag == 12 && lpMsg->tFlag == 12) ||
		(lpMsg->sFlag == 13 && lpMsg->tFlag == 13) ||
		(lpMsg->sFlag == 14 && lpMsg->tFlag == 14) ||
		(lpMsg->sFlag == 15 && lpMsg->tFlag == 15) ||
		(lpMsg->sFlag == 16 && lpMsg->tFlag == 16) ||
		(lpMsg->sFlag == 17 && lpMsg->tFlag == 17) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 19) ||
		(lpMsg->sFlag == 19 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 19 && lpMsg->tFlag == 19) ||
		(lpMsg->sFlag == 0 && lpMsg->tFlag == 20) ||
		(lpMsg->sFlag == 20 && lpMsg->tFlag == 0) ||
		(lpMsg->sFlag == 20 && lpMsg->tFlag == 20))
	{
		BOOL DurSSend;
		BOOL DurTSend;

		result = gObjInventoryMoveItem(Obj, source, target, DurSSend, DurTSend, lpMsg->sFlag, lpMsg->tFlag, (BYTE*)&ItemInfo);

		if (Obj.m_IfState->use == 1 && Obj.m_IfState->type == 3) // Shop
		{
			this->GCPriceSend(Obj, 1, 0);
		}

		this->GCItemObjectMoveResultSend(Obj, result, target, (BYTE*)&ItemInfo);
		if (DurSSend != FALSE)
			this->GCItemObjectDurSend(Obj, source, Obj.pntInventory[source]->m_Durability, FALSE);

		if (DurTSend != FALSE)
			this->GCItemObjectDurSend(Obj, target, Obj.pntInventory[target]->m_Durability, FALSE);
		if (result != 0xFF && lpMsg->sFlag == 2 && lpMsg->tFlag == 0)
		{
			int money = ::GetWarehouseUsedHowMuch(Obj.Level, Obj.WarehousePW);

			if ((Obj.m_PlayerData->Money - money) > 0)
			{
				int iZen = Obj.m_PlayerData->Money;
				Obj.m_PlayerData->Money -= money;
				this->GCMoneySend(Obj, Obj.m_PlayerData->Money);

				//sLog->outBasic("Pay WareHouse Money(In Inventory) : %d - %d = %d",iZen, money, Obj.m_PlayerData->Money);
			}
			else if ((Obj.WarehouseMoney - money) > 0)
			{
				int iZen = Obj.WarehouseMoney;
				Obj.WarehouseMoney -= money;

				//sLog->outBasic("Pay WareHouse Money(In WareHouse) : %d - %d = %d",iZen, money, Obj.WarehouseMoney);

				this->GCWarehouseInventoryMoneySend(Obj, 1, Obj.m_PlayerData->Money, Obj.WarehouseMoney);
			}
		}
		g_PeriodItemEx.SendPeriodItemInfoOnce(lpObj, *Obj.pntInventory[target]);

		return;
	}

	if (lpMsg->sFlag == 22 && lpMsg->tFlag == 22)
	{
		result = gObjMuunInvenMove(lpObj, 0, 0, source, target, (BYTE*)ItemInfo);
		GCItemObjectMoveResultSend(Obj, result, target, (BYTE*)ItemInfo);
	}

	if (lpMsg->sFlag == 1 && lpMsg->tFlag == 21)
	{
		result = gObjTradeEventInventoryMove(*lpObj, source, target);

		if (result == 255)
		{
			this->GCItemObjectMoveResultSend(Obj, -1, target, ItemInfo);
			this->CGTradeCancelButtonRecv(Obj);

		}

		else
		{
			this->GCItemObjectMoveResultSend(Obj, result, target, ItemInfo);
		}

		if (result == 255)
		{
			Obj.m_IfState->state = 0;
			this->GCEventInventoryItemListSend(Obj);

		}

		else
		{
			Obj.TradeOk = 0;
			getGameObject(Obj.TargetNumber)->TradeOk = 0;
			GCTradeOkButtonSend(*getGameObject(Obj.TargetNumber), 2);
			GCTradeOkButtonSend(Obj, 0);

		}

		return;
	}

	if (lpMsg->sFlag == 21 && lpMsg->tFlag == 1)
	{
		result = gObjEventInventoryTradeMove(lpObj, source, target);
		GCItemObjectMoveResultSend(Obj, result, target, ItemInfo);

		return;
	}

	if (lpMsg->sFlag == 21 && lpMsg->tFlag == 21)
	{
		BOOL DurSSend;
		BOOL DurTSend;

		result = gObjEventInvenMove(lpObj, &DurSSend, &DurTSend, source, target);
		this->GCItemObjectMoveResultSend(Obj, result, target, ItemInfo);

		if (result == 255)
		{
			return;
		}

		if (DurSSend)
		{
			this->GCEventItemDurSend(Obj, source, (int)Obj.pntEventInventory[source]->m_Durability);
		}

		if (DurTSend)
		{
			this->GCEventItemDurSend(Obj, target, (int)Obj.pntEventInventory[target]->m_Durability);
		}

		if (Obj.pntEventInventory[target]->m_Durability == 0.0)
		{
			GCEventInventoryItemDeleteSend(Obj, target, TRUE);
		}

		return;
	}

	if (lpMsg->sFlag == 1 && lpMsg->tFlag == 1)
	{
		result = gObjTradeTradeMove(lpObj, source, target);
		this->GCItemObjectMoveResultSend(Obj, result, target, (BYTE*)&ItemInfo);
		return;
	}

	if (lpMsg->sFlag == 1 && lpMsg->tFlag == 0)
	{
		result = gObjTradeInventoryMove(lpObj, source, target);

		if (result == 0xFF)
		{
			this->GCItemObjectMoveResultSend(Obj, 0xFF, target, (BYTE*)&ItemInfo);
			this->CGTradeCancelButtonRecv(Obj);
		}
		else
		{
			this->GCItemObjectMoveResultSend(Obj, result, target, (BYTE*)&ItemInfo);
		}

		if (result == 0xFF)
		{
			Obj.m_IfState->state = 0;
			GCItemObjectListSend(Obj);
			g_PeriodItemEx.OnRequestPeriodItemList(lpObj);
		}

		if (Obj.TargetNumber >= 0)
		{
			Obj.TradeOk = false;
			getGameObject(Obj.TargetNumber)->TradeOk = false;
			this->GCTradeOkButtonSend(*Obj.GetTarget(), 2);
			this->GCTradeOkButtonSend(Obj, 0);
		}

		return;
	}

	if (lpMsg->sFlag == 0 && lpMsg->tFlag == 1)
	{
		result = gObjInventoryTradeMove(lpObj, source, target);
		this->GCItemObjectMoveResultSend(Obj, result, target, (BYTE*)&ItemInfo);
	}
}



void GameProtocol::GCEquipmentChange(CGameObject &Obj, int pos)
{
	PMSG_USEREQUIPMENTCHANGED pChange;
	

	PHeadSetB((BYTE*)&pChange, 0x25, sizeof(PMSG_USEREQUIPMENTCHANGED));
	pChange.NumberH = SET_NUMBERH(Obj.m_Index);
	pChange.NumberL = SET_NUMBERL(Obj.m_Index);
	ItemByteConvert(pChange.ItemInfo, *Obj.pntInventory[pos]);
	pChange.ItemInfo[I_OPTION] = pos << 4;
	pChange.ItemInfo[I_OPTION] |= LevelSmallConvert(Obj, pos) & MAX_ITEM_LEVEL;
	pChange.Element = Obj.m_iPentagramMainAttribute;
	if (pChange.ItemInfo[5] == 0xF0)
		pChange.ItemInfo[5] = 0xF1;
	MsgSendV2(lpObj, (BYTE*)&pChange, pChange.h.size);
}

void GameProtocol::CGTalkRequestRecv(PMSG_TALKREQUEST * lpMsg, CGameObject &Obj)
{
	
	int DealerNumber;
	PMSG_TALKRESULT pResult;
	PMSG_SHOPITEMCOUNT pShopItemCount;

	if (!PacketCheckTime(Obj))
		return;

	if (!gObjIsConnectedGP(Obj))
	{
		return;
	}

	if (Obj.CloseType != -1)
		return;

	if (Obj.m_PlayerData->m_bSecurityCheck == false)
	{
		MsgOutput(Obj, Lang.GetText(0, 822));
		return;
	}

	BYTE SendByte[4096];
	int lOfs = 0;

	if (Obj.m_IfState->use > 0)
		return;

	DealerNumber = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	if (DealerNumber < 0 || DealerNumber > g_ConfigRead.server.GetObjectMax() - 1)
		return;

	if (Obj.MapNumber != getGameObject(DealerNumber)->MapNumber)
		return;

	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		if (getGameObject(DealerNumber)->Class == 367)
		{
			if ((Obj.X < (getGameObject(DealerNumber)->X - 5)) || (Obj.X > (getGameObject(DealerNumber)->X + 5)) || (Obj.Y < (getGameObject(DealerNumber)->Y - 10)) || (Obj.Y > (getGameObject(DealerNumber)->Y + 5)))
			{
				return;
			}
		}
		else if ((Obj.X < (getGameObject(DealerNumber)->X - 5)) || (Obj.X > (getGameObject(DealerNumber)->X + 5)) || (Obj.Y < (getGameObject(DealerNumber)->Y - 5)) || (Obj.Y > (getGameObject(DealerNumber)->Y + 5)))
		{
			return;
		}

	}
	else
	{
		if ((Obj.X < (getGameObject(DealerNumber)->X - 5)) || (Obj.X > (getGameObject(DealerNumber)->X + 5)) || (Obj.Y < (getGameObject(DealerNumber)->Y - 5)) || (Obj.Y > (getGameObject(DealerNumber)->Y + 5)))
		{
			return;
		}
	}

	if (getGameObject(DealerNumber)->Type == OBJ_NPC)
	{
		if (NpcTalk(*getGameObject(DealerNumber), lpObj) == TRUE)
		{
			this->GCAnsCsMapSvrTaxInfo(Obj, 1, g_CastleSiegeSync.GetTaxRateChaos(Obj));
			return;
		}
	}
	// getGameObject(DealerNumber)->Type
	if (::gObjFixInventoryPointer(Obj) == false)
	{

	}

	if (Obj.pTransaction == 1)
	{
		return;
	}

	if (getGameObject(DealerNumber)->Class == 234)	// Npc Server Division
	{
		pResult.h.c = 0xC3;
		pResult.h.headcode = 0x30;
		pResult.h.size = sizeof(pResult);
		pResult.result = 5;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (getGameObject(DealerNumber)->Type == OBJ_NPC && getGameObject(DealerNumber)->m_btNpcType == NPC_TYPES::NPC_NONE)
		return;

	if (getGameObject(DealerNumber)->m_btNpcType == NPC_TYPES::NPC_SHOP)
	{
		SHOP_DATA * lpShopData = g_ShopMng.GetShop(lpObj, *getGameObject(DealerNumber));

		if (!lpShopData)
		{
			return;
		}
	}

	if (gMerryXMasNpcEvent == TRUE)
	{
		if ((rand() % 6) == 0)
		{
			ChatTargetSend(*getGameObject(DealerNumber), Lang.GetText(0, 55), Obj.m_Index);
		}
	}

	if (::gHappyNewYearNpcEvent == TRUE)
	{
		if ((rand() % 6) == 0)
		{
			ChatTargetSend(*getGameObject(DealerNumber), Lang.GetText(0, 56), Obj.m_Index);
		}
	}

	SHOP_DATA * lpShopData = g_ShopMng.GetShop(lpObj, *getGameObject(DealerNumber));

	Obj.TargetNpcNumber = DealerNumber;
	Obj.m_IfState->use = 1;
	Obj.m_IfState->type = 3;
	Obj.m_ShopTime = 0;
	pResult.h.c = 0xC3;
	pResult.h.headcode = 0x30;
	pResult.h.size = sizeof(pResult);

	if (getGameObject(DealerNumber)->m_btNpcType == NPC_TYPES::NPC_SHOP && lpShopData->btIsMossMerchant == TRUE)
	{
		pResult.result = 0x22;
	}

	else
	{
		pResult.result = 0x00;
	}

	if (getGameObject(DealerNumber)->m_btNpcType == NPC_TYPES::NPC_SHOP && lpShopData->btIsMossMerchant == TRUE && !g_GamblingItemBag.IsEnabled())	return;

	if (getGameObject(DealerNumber)->m_btNpcType == NPC_TYPES::NPC_WAREHOUSE)
	{
		if (Obj.m_PlayerData->m_JoinUnityBattle == true || g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
		{
			this->GCServerMsgStringSend(Lang.GetText(0, 619), Obj, 1);
			return;
		}

		if (Obj.m_PlayerData->m_ReqWarehouseOpen != false)
		{
			return;
		}

		if (Obj.m_PlayerData->m_bSecurityCheck == false)
		{
			return;
		}

		if (Obj.m_btOpenWarehouse == TRUE)
		{
			return;
		}

		Obj.m_PlayerData->m_ReqWarehouseOpen = true;
		Obj.m_IfState->type = 6;
		Obj.m_IfState->state = 0;
	}

	else if (getGameObject(DealerNumber)->m_btNpcType == NPC_TYPES::NPC_CHAOSMIX)
	{
		if (bCanChaosBox == TRUE)
		{
			if (Obj.m_bPShopOpen == true)
			{
				GCServerMsgStringSend(Lang.GetText(0, 112), Obj, 1);

				return;
			}

			Obj.m_IfState->type = 7;
			Obj.m_IfState->state = 0;
			pResult.result = 3;
			if (getGameObject(DealerNumber)->Class == 450)
				pResult.result = 22;
			else if (getGameObject(DealerNumber)->Class == 478)
				pResult.result = 32;
			Obj.bIsChaosMixCompleted = false;
			pResult.level1 = gDQChaosSuccessRateLevel1;
			pResult.level2 = gDQChaosSuccessRateLevel2;
			pResult.level3 = gDQChaosSuccessRateLevel3;
			pResult.level4 = gDQChaosSuccessRateLevel4;
			pResult.level5 = gDQChaosSuccessRateLevel5;
			pResult.level6 = gDQChaosSuccessRateLevel6;
			pResult.level7 = gDQChaosSuccessRateLevel7;

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
			gObjInventoryTrans(Obj);

			gObjItemTextSave(lpObj);
			gObjWarehouseTextSave(lpObj);
			GCAnsCsMapSvrTaxInfo(Obj, 1, ::g_CastleSiegeSync.GetTaxRateChaos(Obj));
		}
	}

	else if (lpShopData->btIsMossMerchant == TRUE)
	{
		pResult.result = 34;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
	}

	else
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
	}

	if (getGameObject(DealerNumber)->m_btNpcType == NPC_TYPES::NPC_WAREHOUSE)
	{
		if (Obj.m_bMapSvrMoveReq == true)
		{
			Obj.m_PlayerData->m_ReqWarehouseOpen = false;
			return;
		}

		if (Obj.m_State == 32)
		{
			Obj.m_PlayerData->m_ReqWarehouseOpen = false;
			return;
		}

		if (Obj.m_bMapSvrMoveQuit == true)
		{
			Obj.m_PlayerData->m_ReqWarehouseOpen = false;
			return;
		}

		if (Obj.m_IfState->use && Obj.m_IfState->type == 19)
		{
			return;
		}

		if (Obj.WarehouseSaveLock == TRUE)
		{
			Obj.m_PlayerData->m_ReqWarehouseOpen = false;
			return;
		}

		Obj.WarehouseCount = 0;
		GDGetWarehouseList(Obj, Obj.AccountID);
		return;
	}

	if (getGameObject(DealerNumber)->m_btNpcType == NPC_TYPES::NPC_CHAOSMIX)
		return;

	else if (getGameObject(DealerNumber)->m_btNpcType == NPC_TYPES::NPC_SHOP)
	{
		lOfs = 0;

		lOfs += sizeof(pShopItemCount);

		int size = lOfs + lpShopData->m_ShopData.SendItemDataLen;
		PHeadSetW((BYTE*)&pShopItemCount, 0x31, size);
		pShopItemCount.Type = 0;
		pShopItemCount.count = lpShopData->m_ShopData.ItemCount;
		std::memcpy(SendByte, &pShopItemCount, sizeof(pShopItemCount));
		std::memcpy(&SendByte[lOfs], lpShopData->m_ShopData.SendItemData, lpShopData->m_ShopData.SendItemDataLen);

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, SendByte, size);
		GCAnsCsMapSvrTaxInfo(Obj, 2, ::g_CastleSiegeSync.GetTaxRateStore(Obj));
	}
}




void GameProtocol::GCUserWarehouseSend(CGameObject &Obj)
{
	if (Obj.m_IfState->type != 6)
		return;

	if (Obj.m_IfState->type == 6 && Obj.m_IfState->state == 1)
		return;

	if (Obj.m_btOpenWarehouse == TRUE)
	{
		return;
	}

	PMSG_SHOPITEMCOUNT pMsg;
	BYTE SendByte[4096];
	int lOfs = sizeof(pMsg);

	PHeadSetW((BYTE*)&pMsg, 0x31, 0);
	pMsg.count = 0;
	pMsg.Type = 0;

	for (int n = 0; n < WAREHOUSE_SIZE; n++)
	{
		if (Obj.pntWarehouse[n]->IsItem() == TRUE)
		{
			SendByte[lOfs] = n;
			lOfs++;
			ItemByteConvert(&SendByte[lOfs], *Obj.pntWarehouse[n]);
			lOfs += MAX_ITEM_INFO;
			pMsg.count++;
		}
	}

	pMsg.h.sizeH = SET_NUMBERH(lOfs);
	pMsg.h.sizeL = SET_NUMBERL(lOfs);
	std::memcpy(SendByte, &pMsg, sizeof(pMsg));

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, SendByte, lOfs);
	this->GCWarehouseInventoryMoneySend(Obj, 1, Obj.m_PlayerData->Money, Obj.WarehouseMoney);

	if (Obj.m_IfState->type == 6)
	{
		Obj.m_IfState->state = 1;
	}

	Obj.m_btOpenWarehouse = TRUE;
}

void GameProtocol::CGBuyRequestRecv(PMSG_BUYREQUEST * lpMsg, CGameObject &Obj)
{
	
	PMSG_BUYRESULT pResult;

	PHeadSetB((BYTE*)&pResult, 0x32, sizeof(pResult));
	pResult.Result = -1;

	if (!PacketCheckTime(Obj))
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.CloseType != -1)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (Obj.TargetNpcNumber == -1)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	CGameObject lpShopObj = *getGameObject(Obj.TargetNpcNumber);

	SHOP_DATA * lpShopData = g_ShopMng.GetShop(lpObj, lpShopObj);

	if (!lpShopData)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (!::gObjFixInventoryPointer(Obj))
	{

	}

	if (Obj.m_ShopTime == 0)
		Obj.m_ShopTime = 1;

	if (lpShopData->btIsMossMerchant == TRUE)
	{
		if (Obj.m_ShopTime > g_GamblingItemBag.GetGamblingShopTime())
		{
			if (Obj.m_IfState->use && Obj.m_IfState->type == 3)
			{
				Obj.TargetNpcNumber = -1;
				pResult.Result = 0xFE;
				Obj.m_IfState->use = 0;
				Obj.m_IfState->type = 0;
			}

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
			GCServerMsgStringSend(Lang.GetText(0, 520), Obj, 1);
			return;
		}
	}

	if (Obj.m_ShopTime > 60)
	{
		if (Obj.m_IfState->use != 0 && Obj.m_IfState->type == 3)
		{
			Obj.TargetNpcNumber = -1;
			Obj.m_IfState->use = 0;
			Obj.m_IfState->type = 0;
		}

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (Obj.m_IfState->use > 0)
	{
		if (Obj.m_IfState->type != 3)
		{
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}
	}

	if (lpMsg->Pos > MAX_ITEM_IN_SHOP - 1)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (Obj.m_PlayerData->m_bIsCancelItemSale == true)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (lpShopData->m_ShopData.m_item[lpMsg->Pos]->IsItem() == TRUE)
	{
		int iStoreTaxMoney = lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_BuyMoney + (int)((__int64)lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_BuyMoney * (__int64)g_CastleSiegeSync.GetTaxRateStore(Obj) / (__int64)100);

		if (iStoreTaxMoney < 0)
			iStoreTaxMoney = 0;

		int iStoreTaxMoney2 = (int)((__int64)lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_BuyMoney * (__int64)g_CastleSiegeSync.GetTaxRateStore(Obj) / (__int64)100);

		if (iStoreTaxMoney2 < 0)
			iStoreTaxMoney2 = 0;

		if (g_ArcaBattle.IsArcaBattleServer() == 1 && iStoreTaxMoney > 0)
		{
			iStoreTaxMoney *= 3;
		}

		if (lpShopData->btIsMossMerchant == TRUE)
		{
			int iTaxGambling = g_GamblingItemBag.GetGamblingValue() * g_CastleSiegeSync.GetTaxRateStore(Obj) / 100;
			iStoreTaxMoney = g_GamblingItemBag.GetGamblingValue() + iTaxGambling;
		}

		if (Obj.m_PlayerData->Money < iStoreTaxMoney)
			pResult.Result = -1;
		else
		{
			BOOL bNoItem = TRUE;

			if ((lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type >= ITEMGET(14, 0) && lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type <= ITEMGET(14, 8)) ||
				(lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type >= ITEMGET(14, 35) && lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type <= ITEMGET(14, 40)))
			{
				int dur = lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Durability;

				if (dur == 0)
					dur = 1;

				if (::gObjSearchItem(Obj, lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type, dur, lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Level) == TRUE)
				{
					bNoItem = FALSE;
					Obj.m_PlayerData->Money -= iStoreTaxMoney;
					::g_CastleSiegeSync.AddTributeMoney(iStoreTaxMoney2);

					if (Obj.m_PlayerData->Money < 0)
						Obj.m_PlayerData->Money = 0;

					this->GCMoneySend(Obj, Obj.m_PlayerData->Money);
				}
			}

			if (lpShopData->btIsMossMerchant == TRUE)
			{
				if (!g_GamblingItemBag.IsEnabled())
				{
					pResult.Result = 0xFF;
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
					return;
				}

				if (bNoItem)
				{
					int iCount = 0;

					while (iCount <= lpShopData->m_ShopData.ItemCount)
					{
						if (lpMsg->Pos == lpShopData->m_ShopData.SendItemData[iCount + (12 * iCount)])
						{
							pResult.Result = g_GamblingItemBag.GetGamblingSystemItem(Obj, 229, 0, 0, iCount);
							break;
						}
						iCount++;
					}

					pResult.Result = 0xFF;
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
					return;
				}
			}

			if (bNoItem)
			{
				if (lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type == ITEMGET(13, 239) ||// Muun Energy Converter
					lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type >= ITEMGET(13, 212) && // Pets Muun
					lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type <= ITEMGET(13, 219) ||// Pets Muun
					lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type >= ITEMGET(13, 223) && // Pets Muun
					lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type <= ITEMGET(13, 275) ||// Pets Muun
					lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type >= ITEMGET(13, 278) && // Pets Muun
					lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type <= ITEMGET(13, 349) ||// Pets Muun
					lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type >= ITEMGET(13, 409) && // Pets Muun
					lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type <= ITEMGET(13, 464) ||// Pets Muun
					lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type >= ITEMGET(13, 480) && // Pets Muun
					lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type <= ITEMGET(13, 503)) // Pets Muun
				{
					BYTE btRet = gObjMuunInventoryInsertItem(Obj, *lpShopData->m_ShopData.m_item[lpMsg->Pos]);

					if (btRet == 0xFF)
					{
						pResult.Result = 0xFF;
						IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
						return;
					}

					ItemByteConvert(pResult.ItemInfo, *lpShopData->m_ShopData.m_item[lpMsg->Pos]);
					int buymoney = lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_BuyMoney;
					Obj.m_PlayerData->Money -= iStoreTaxMoney;
					g_CastleSiegeSync.AddTributeMoney(iStoreTaxMoney2);
					if (Obj.m_PlayerData->Money < 0) Obj.m_PlayerData->Money = 0;
					GCMoneySend(Obj, Obj.m_PlayerData->Money);
					int iTaxRate = g_CastleSiegeSync.GetTaxRateStore(Obj);

					this->GCMuunInventoryItemOneSend(Obj, btRet);

					pResult.Result = 0xFF;
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
					return;
				}

				if (lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_serial == 0)
				{
					LPITEM_ATTRIBUTE p = GetItemAttr(lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type);

					if (!p)
					{
						pResult.Result = 0xFF;
						IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
						return;
					}

					pResult.Result = gObjShopBuyInventoryInsertItem(Obj, *lpShopData->m_ShopData.m_item[lpMsg->Pos]);

					if (pResult.Result != 0xFF)
					{

						ItemByteConvert(pResult.ItemInfo, *lpShopData->m_ShopData.m_item[lpMsg->Pos]);
						int buymoney = lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_BuyMoney;
						Obj.m_PlayerData->Money -= iStoreTaxMoney;
						g_CastleSiegeSync.AddTributeMoney(iStoreTaxMoney2);
						if (Obj.m_PlayerData->Money < 0) Obj.m_PlayerData->Money = 0;
						GCMoneySend(Obj, Obj.m_PlayerData->Money);
						int iTaxRate = g_CastleSiegeSync.GetTaxRateStore(Obj);
					}
				}

				else
				{
					LPITEM_ATTRIBUTE p = GetItemAttr(lpShopData->m_ShopData.m_item[lpMsg->Pos].m_Type);

					if (!p)
					{
						pResult.Result = 0xFF;
						IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
						return;
					}

					if (CheckInventoryEmptySpace(&Obj, p->Height, p->Width) == FALSE)
					{
						pResult.Result = 0xFF;
						IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
						return;
					}

					int buymoney = lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_BuyMoney;
					Obj.m_PlayerData->Money -= iStoreTaxMoney;
					g_CastleSiegeSync.AddTributeMoney(iStoreTaxMoney2);
					if (Obj.m_PlayerData->Money < 0) Obj.m_PlayerData->Money = 0;
					GCMoneySend(Obj, Obj.m_PlayerData->Money);
					int iTaxRate = g_CastleSiegeSync.GetTaxRateStore(Obj);

					ItemCreate(Obj, 222, 0, 0, 
						lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Type,
						lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Level,
						lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Durability,
						lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Option1,
						lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Option2,
						lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_Option3,
						Obj.m_Index, 
						lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_NewOption,
						lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_SetOption,
						0, 
						lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_SocketOption, 
						lpShopData->m_ShopData.m_item[lpMsg->Pos]->m_BonusSocketOption);

				}
			}
		}
	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
}



struct PMSG_SELLRESULT
{
	PBMSG_HEAD2 h;	// C1:33
	BYTE Result;	// 3
	DWORD Money;	// 4
};



void GameProtocol::CGSellRequestRecv(PMSG_SELLREQUEST * lpMsg, CGameObject &Obj)
{
	PMSG_SELLRESULT pResult;
	

	PHeadSetB((BYTE*)&pResult, 0x33, sizeof(pResult));
	pResult.Result = false;
	pResult.Money = 0;

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.CloseType != -1)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (Obj.m_IfState->use > 0 && Obj.m_IfState->type != 3)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (!PacketCheckTime(Obj))
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (!::gObjFixInventoryPointer(Obj))
	{

	}

	if (Obj.pTransaction == 1)
	{
		return;
	}

	if (Obj.TargetNpcNumber == -1)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	CGameObject lpShopObj = *getGameObject(Obj.TargetNpcNumber);

	SHOP_DATA * lpShopData = g_ShopMng.GetShop(lpObj, lpShopObj);

	if (!lpShopData)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (!gObjCanItemTouch(lpObj, 3))
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (lpMsg->Pos > MAIN_INVENTORY_SIZE - 1 && lpMsg->Pos != 236)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (gObjCheckSerial0ItemList(Obj.pntInventory[lpMsg->Pos]) != FALSE)
	{
		MsgOutput(Obj, Lang.GetText(0, 259));
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if (g_ConfigRead.data.common.ShopExCItemObjectLimit == FALSE)
	{
		if (Obj.pntInventory[lpMsg->Pos]->m_NewOption == 63)
		{
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}
	}

	if (g_ConfigRead.data.common.EnableSellAllItems == false)
	{
		if (Obj.pntInventory[lpMsg->Pos]->m_Type == ITEMGET(14, 11)) // Box of Luck
		{
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}

		if (Obj.pntInventory[lpMsg->Pos]->m_QuestItem != false && !g_QuestExpManager.IsQuestItemAtt(Obj.pntInventory[lpMsg->Pos]->m_Type, 8))
		{
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}

		if (Obj.pntInventory[lpMsg->Pos]->m_Type == ITEMGET(13, 20) && Obj.pntInventory[lpMsg->Pos]->m_Level == 1) // Wizards Ring +1
		{
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}

		if (Obj.pntInventory[lpMsg->Pos]->m_Type == ITEMGET(13, 20) && Obj.pntInventory[lpMsg->Pos]->m_Level == 2) // Wizards Ring +2
		{
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}

		if (Obj.pntInventory[lpMsg->Pos]->m_Type == ITEMGET(12, 26) &&
			(Obj.pntInventory[lpMsg->Pos]->m_Level == 1 ||
				Obj.pntInventory[lpMsg->Pos]->m_Level == 2 ||
				Obj.pntInventory[lpMsg->Pos]->m_Level == 3 ||
				Obj.pntInventory[lpMsg->Pos]->m_Level == 4 ||
				Obj.pntInventory[lpMsg->Pos]->m_Level == 5))
		{
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}

		if (g_kJewelOfHarmonySystem.IsStrengthenByJewelOfHarmony(Obj.pntInventory[lpMsg->Pos]) == TRUE && g_ConfigRead.data.common.joinmuRemoveItemSellDropRestriction == false)
		{
			GCServerMsgStringSend(Lang.GetText(0, 267), Obj, 1);
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}

		if (Obj.pntInventory[lpMsg->Pos]->IsPeriodItem() == TRUE && Obj.pntInventory[lpMsg->Pos]->IsPeriodItemExpire() == FALSE)
		{
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}

		if (Obj.pntInventory[lpMsg->Pos]->IsPeriodItem() == TRUE)
		{
			if (Obj.pntInventory[lpMsg->Pos]->m_Type == ITEMGET(12, 266) ||
				Obj.pntInventory[lpMsg->Pos]->m_Type == ITEMGET(12, 267))
			{
				Obj.pntInventory[lpMsg->Pos]->SetPeriodItemExpire();
				g_PeriodItemEx.RemovePeriodItemDataByForce(lpObj, Obj.pntInventory[lpMsg->Pos]->m_Type, Obj.pntInventory[lpMsg->Pos]->m_Number);
			}

			else if (Obj.pntInventory[lpMsg->Pos]->IsPeriodItemExpire() == FALSE)
			{
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
				return;
			}
		}
	}

	if (Obj.pntInventory[lpMsg->Pos]->m_Type == ITEMGET(14, 12) && Obj.pntInventory[lpMsg->Pos]->m_Level == 1) // Heart+1
	{
		pResult.Result = true;
		pResult.Money = Obj.m_PlayerData->Money;
		gObjInventoryItemSet(Obj, lpMsg->Pos, -1);
		Obj.pntInventory[lpMsg->Pos]->Clear();
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if (!IsSellToNPCItemObject(Obj.pntInventory[lpMsg->Pos]->m_Type))
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (Obj.pntInventory[lpMsg->Pos]->IsItem() == FALSE)
	{
		pResult.Result = false;
		pResult.Money = Obj.m_PlayerData->Money;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

	}
	else
	{
		BYTE NewOption[MAX_EXOPTION_SIZE];
		int iAddZen = 0;
		int type = Obj.pntInventory[lpMsg->Pos]->m_Type;

		if (type == ITEMGET(13, 4) || type == ITEMGET(13, 5)) // Pets
			Obj.pntInventory[lpMsg->Pos]->PetValue();
		else
			Obj.pntInventory[lpMsg->Pos]->Value();

		iAddZen = Obj.pntInventory[lpMsg->Pos]->m_SellMoney;

		if (gObjCheckMaxZen(Obj, iAddZen) == FALSE)
		{
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}

		Obj.m_PlayerData->Money += iAddZen;
		pResult.Result = true;
		pResult.Money = Obj.m_PlayerData->Money;


		ItemIsBufExOption(NewOption, Obj.pntInventory[lpMsg->Pos]);

		if (g_PentagramSystem.IsPentagramItem(Obj.pntInventory[lpMsg->Pos]))
		{
			g_PentagramSystem.DelPentagramJewelInfo(Obj, Obj.pntInventory[lpMsg->Pos]);
		}

		bool bSendEquip = false;

		if ((Obj.pntInventory[lpMsg->Pos]->m_Type == ITEMGET(13, 169) || Obj.pntInventory[lpMsg->Pos]->m_Type == ITEMGET(13, 170)) && (lpMsg->Pos == 10 || lpMsg->Pos == 11))
		{
			bSendEquip = true;
		}

		if (IsExpensiveItem(Obj.pntInventory[lpMsg->Pos]) == TRUE && Obj.pntInventory[lpMsg->Pos]->IsPeriodItem() == FALSE)
		{
			g_CancelItemSale.GDReqAddItemToList(Obj, *Obj.pntInventory[lpMsg->Pos], iAddZen);
		}

		::gObjInventoryDeleteItem(Obj, lpMsg->Pos);

		if (bSendEquip == true)
		{
			this->GCEquipmentSend(Obj);
		}

		if (lpMsg->Pos == 10 || lpMsg->Pos == 11)
		{
			::gObjUseSkill.SkillChangeUse(Obj);
		}

		::gObjMakePreviewCharSet(Obj);

	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
}


int GameProtocol::GetNeedMoneyItemDurRepaire(/*CGameObject &Obj, */CItemObject *  DurItem, BOOL RequestPos)
{
	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		return 1;
	}

	int iMoney = 0;
	int type = DurItem->m_Type;
	float ItemDurability = DurItem->m_Durability;

	if (type == ITEMGET(13, 10)) // Transformation Ring
		return 0;

	if (::HasItemDurability(type) != FALSE)
	{
		float lc4;
		float lc5 = 0;
		lc4 = DurItem->m_BaseDurability;

		if (ItemDurability == lc4)
			return 0;

		float lc6 = 1.0f - ItemDurability / lc4;
		int lc7;

		if (type == ITEMGET(13, 4) || type == ITEMGET(13, 5)) // Pets
		{
			lc7 = DurItem->m_BuyMoney;
		}
		else
		{
			lc7 = DurItem->m_BuyMoney / 3;

		}

		if (lc7 > 400000000) lc7 = 400000000;

		if (lc7 >= 1000)
			lc7 = lc7 / 100 * 100;
		else if (lc7 >= 100)
			lc7 = lc7 / 10 * 10;

		float lc8 = sqrt(static_cast<float>(lc7));
		float lc9 = sqrt(sqrt(static_cast<float>(lc7)));
		lc5 = 3.0f * lc8 * lc9;
		lc5 *= lc6;
		lc5 += 1.0f;

		if (ItemDurability <= 0.0f)
		{
			if (type == ITEMGET(13, 4) || type == ITEMGET(13, 5)) // Pets
				lc5 *= 2;
			else
				lc5 *= ::GetAllRepairItemRate(type);

		}

		if (RequestPos == TRUE)
		{
			lc5 += lc5 * 0.05f;
		}
		iMoney = lc5;

		if (iMoney >= 1000)
			iMoney = iMoney / 100 * 100;
		else if (iMoney >= 100)	// Deathway fix Here -> Must be 100 and >=
			iMoney = iMoney / 10 * 10;

	}
	else
	{
		iMoney = -1;
	}

	return iMoney;
}



struct PMSG_ITEMDURREPAIR_RESULT
{
	PBMSG_HEAD2 h;	// C1:34
	int Money;	// 4
};



void GameProtocol::ItemDurRepaire(CGameObject &Obj, /*CGameObject &Obj, */CItemObject * DurItem, int pos, int RequestPos)
{
	PMSG_ITEMDURREPAIR_RESULT pResult;
	int result = TRUE;

	PHeadSetB((BYTE*)&pResult, 0x34, sizeof(pResult));
	int itemtype = DurItem->m_Type;

	if (Obj.m_IfState->type != 13)
	{
		if (itemtype == ITEMGET(13, 4) || itemtype == ITEMGET(13, 5))
		{
			pResult.Money = 0;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

			return;
		}
	}

	if (DurItem->IsPeriodItem() == TRUE)
	{
		pResult.Money = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if (itemtype >= ITEMGET(14, 0) || (itemtype >= ITEMGET(13, 0) && itemtype < ITEMGET(13, 4)) || itemtype == ITEMGET(13, 10) || (itemtype >= ITEMGET(12, 7) && itemtype <= ITEMGET(12, 35)) || itemtype == ITEMGET(4, 7) || itemtype == ITEMGET(4, 15))
	{
		pResult.Money = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if (itemtype == ITEMGET(13, 38))
	{
		pResult.Money = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if (itemtype == ITEMGET(13, 39))
	{
		pResult.Money = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if (itemtype == ITEMGET(13, 40))
	{
		pResult.Money = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if (itemtype == ITEMGET(13, 41))
	{
		pResult.Money = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if (itemtype == ITEMGET(13, 42))
	{
		pResult.Money = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	pResult.Money = GetNeedMoneyItemDurRepaire(/*Obj, */DurItem, RequestPos);

	if (pResult.Money <= 0)
	{
		pResult.Money = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if ((Obj.m_PlayerData->Money - pResult.Money) < 1)
		pResult.Money = 0;
	else
	{
		Obj.m_PlayerData->Money -= pResult.Money;
		pResult.Money = Obj.m_PlayerData->Money;
		DurItem->m_Durability = (float)((int)DurItem->m_BaseDurability);
		DurItem->Convert(DurItem->m_Type, DurItem->m_Option1, DurItem->m_Option2, DurItem->m_Option3,
			DurItem->m_NewOption, DurItem->m_SetOption, DurItem->m_ItemOptionEx, 0, -1, 0, CURRENT_DB_VERSION);
		GCItemObjectDurSend(Obj, pos, DurItem->m_Durability, FALSE);
	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
}




void GameProtocol::CGModifyRequestItem(PMSG_ITEMDURREPAIR * lpMsg, CGameObject &Obj)
{
	PMSG_ITEMDURREPAIR_RESULT pResult;

	PHeadSetB((BYTE*)&pResult, 0x34, sizeof(pResult));
	pResult.Money = 0;

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.CloseType != -1)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (!PacketCheckTime(Obj))
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (lpMsg->Requestpos == 1 && Obj.Level < 50)
	{
		pResult.Money = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (lpMsg->Position == 0xFF)
	{
		for (int n = 0; n < MAIN_INVENTORY_SIZE; n++)
		{
			if (Obj.pntInventory[n]->IsItem())
			{
				if (IsCashItem(Obj.pntInventory[n]->m_Type) == TRUE)
					continue;

				if (IsPremiumItem(Obj.pntInventory[n]->m_Type) == TRUE)
					continue;

				if (Obj.pntInventory[n]->IsPeriodItem() == TRUE)
					continue;

				if (Obj.pntInventory[n]->IsPeriodItemExpire() == TRUE)
					continue;

				if (g_LuckyItemManager.IsLuckyItemEquipment(Obj.pntInventory[n]->m_Type))	//season 6.1 add-on
					continue;

				if (Obj.pntInventory[n]->m_Type == ITEMGET(13, 20) && (Obj.pntInventory[n]->m_Level == 0 || Obj.pntInventory[n]->m_Level == 1))
					continue;

				if (Obj.pntInventory[n]->m_Type == ITEMGET(13, 20) && Obj.pntInventory[n]->m_Level == 2)
					continue;

				if (Obj.pntInventory[n]->m_Type == ITEMGET(13, 4) || Obj.pntInventory[n]->m_Type == ITEMGET(13, 5))
					continue;

				if (Obj.pntInventory[n]->m_Type == ITEMGET(13, 37))
					continue;

				if (Obj.pntInventory[n]->m_Type == ITEMGET(13, 38))
					continue;

				if (Obj.pntInventory[n]->m_Type == ITEMGET(13, 40))
					continue;

				if (Obj.pntInventory[n]->m_Type == ITEMGET(13, 41))
					continue;

				if (Obj.pntInventory[n]->m_Type == ITEMGET(13, 42))
					continue;

				if (Obj.pntInventory[n]->m_Type == ITEMGET(13, 51))
					continue;

				if (Obj.pntInventory[n]->m_Type == ITEMGET(13, 64))
					continue;

				if (Obj.pntInventory[n]->m_Type == ITEMGET(13, 65))
					continue;

				if (Obj.pntInventory[n]->m_Type == ITEMGET(13, 67))
					continue;

				if (Obj.pntInventory[n]->m_Type == ITEMGET(13, 70))
					continue;

				if (Obj.pntInventory[n]->m_Type == ITEMGET(13, 76))
					continue;

				if (Obj.pntInventory[n]->m_Type == ITEMGET(13, 77))
					continue;

				if (Obj.pntInventory[n]->m_Type == ITEMGET(13, 78))
					continue;

				if (Obj.pntInventory[n]->m_Type == ITEMGET(13, 80))
					continue;

				if (Obj.pntInventory[n]->m_Type == ITEMGET(13, 106))
					continue;

				if (Obj.pntInventory[n]->m_Type == ITEMGET(13, 123))
					continue;

				if (Obj.pntInventory[n]->m_Type == ITEMGET(13, 39))
					continue;

				if (Obj.pntInventory[n]->m_Type == ITEMGET(12, 26) &&
					(Obj.pntInventory[n]->m_Level == 1 ||
						Obj.pntInventory[n]->m_Level == 2 ||
						Obj.pntInventory[n]->m_Level == 3 ||
						Obj.pntInventory[n]->m_Level == 4 ||
						Obj.pntInventory[n]->m_Level == 5))
					continue;

				if (!IsRepairItem(Obj.pntInventory[n]->m_Type))
					continue;

				ItemDurRepaire(lpObj,/*Obj,*/ Obj.pntInventory[n], n, lpMsg->Requestpos);
			}
		}

		gObjCalCharacter.CalcCharacter(Obj);
		return;
	}

	if (lpMsg->Position > MAIN_INVENTORY_SIZE - 1)
	{
		pResult.Money = 0;
		return;
	}

	if (Obj.pntInventory[lpMsg->Position]->IsPeriodItem() == TRUE)
		return;

	if (IsCashItem(Obj.pntInventory[lpMsg->Position]->m_Type) == TRUE)
		return;

	if (IsPremiumItem(Obj.pntInventory[lpMsg->Position]->m_Type) == TRUE)
		return;

	if (Obj.pntInventory[lpMsg->Position]->IsPeriodItemExpire() == TRUE)
		return;

	if (g_LuckyItemManager.IsLuckyItemEquipment(Obj.pntInventory[lpMsg->Position]->m_Type))	//season 6.1 add-on
		return;

	if (Obj.pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 20) && (Obj.pntInventory[lpMsg->Position]->m_Level == 0 || Obj.pntInventory[lpMsg->Position]->m_Level == 1))
		return;

	if (Obj.pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 20) && Obj.pntInventory[lpMsg->Position]->m_Level == 2)
		return;

	if (Obj.pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 37))
		return;

	if (Obj.pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 38))
		return;

	if (Obj.pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 40))
		return;

	if (Obj.pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 41))
		return;

	if (Obj.pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 42))
		return;

	if (Obj.pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 51))
		return;

	if (Obj.pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 70))
		return;

	if (Obj.pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 66))
		return;

	if (Obj.pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 76))
		return;

	if (Obj.pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 77))
		return;

	if (Obj.pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 78))
		return;

	if (Obj.pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 80))
		return;

	if (Obj.pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 106))
		return;

	if (Obj.pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 123))
		return;

	if (Obj.pntInventory[lpMsg->Position]->m_Type == ITEMGET(13, 39))
		return;

	if (!IsRepairItem(Obj.pntInventory[lpMsg->Position]->m_Type))
		return;

	ItemDurRepaire(lpObj,/*Obj,*/ Obj.pntInventory[lpMsg->Position], lpMsg->Position, lpMsg->Requestpos);
	gObjCalCharacter.CalcCharacter(Obj);
}



struct PMSG_TRADE_REQUESTSEND
{
	PBMSG_HEAD2 h;	// C3:36
	char szId[10];	// 3
};


void GameProtocol::CGTradeRequestSend(PMSG_TRADE_REQUEST * lpMsg, CGameObject &Obj)
{
	if (bCanTrade == FALSE)
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 57), Obj, 1);
		this->GCServerMsgStringSend(Lang.GetText(0, 58), Obj, 1);
		return;
	}

	if (Obj.CloseType != -1)
		return;

	if (!PacketCheckTime(Obj))
		return;

	int number = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	if (number == Obj.m_Index)
		return;

	CGameObject* lpObjTarget = getGameObject(number);

	if (!gObjIsConnected(*lpObjTarget))
		return;

	if (lpObjTarget->Type != OBJ_USER || Obj.Type != OBJ_USER) // ANTI_CRASH
	{
		return;
	}

	if (Obj.m_PlayerData->m_bSecurityCheck == false)
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 521), Obj, 1);
		return;
	}

	if (lpObjTarget->m_PlayerData->m_bSecurityCheck == false)
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 606), Obj, 1);
		return;
	}

	if (lpObjTarget->m_bMapSvrMoveReq == true || Obj.m_bMapSvrMoveReq == true)
	{
		return;
	}

	if (lpObjTarget->m_bMapSvrMoveQuit == true || Obj.m_bMapSvrMoveQuit == true)
	{
		return;
	}

	if (lpObjTarget->CloseCount >= 0)
		return;

	if (DS_MAP_RANGE(lpObjTarget->MapNumber) != FALSE)
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 12), Obj, 1);
		return;
	}

	if (Obj.m_bPShopOpen == true)
		return;

	if (lpObjTarget->m_bPShopOpen == true)
		return;

	if (CC_MAP_RANGE(lpObjTarget->MapNumber) != FALSE)
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 114), Obj, 1);
		return;
	}

	if (lpObjTarget->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 572), Obj, 1);
		return;
	}

	if (Obj.m_PlayerData->m_JoinUnityBattle == true ||
		lpObjTarget->m_PlayerData->m_JoinUnityBattle == true ||
		g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 618), Obj, 1);
		return;
	}

	if (IT_MAP_RANGE(lpObjTarget->MapNumber) != FALSE)
	{
		GCServerMsgStringSend(Lang.GetText(0, 290), Obj, 1);
		return;
	}

	if (DG_MAP_RANGE(lpObjTarget->MapNumber) != FALSE)
	{
		GCServerMsgStringSend(Lang.GetText(0, 303), Obj, 1);
		return;
	}

	if (IMPERIAL_MAP_RANGE(lpObjTarget->MapNumber) != FALSE)
	{
		GCServerMsgStringSend(Lang.GetText(0, 304), Obj, 1);
		return;
	}

	if (BC_MAP_RANGE(Obj.MapNumber) != FALSE)
	{
		int iBridgeIndex = g_BloodCastle.GetBridgeIndex(Obj.MapNumber);
		if (g_BloodCastle.GetCurrentState(iBridgeIndex) != 1 || g_BloodCastle.CheckCanEnter(iBridgeIndex) == false)
		{
			this->GCServerMsgStringSend(Lang.GetText(0, 107), Obj, 1);
			return;
		}
	}
	if (lpObjTarget->m_PlayerData->ISBOT == true && g_BotSystem.GetBotType(*lpObjTarget) == BOT_TRADE)
	{
		g_BotSystem.AlchemistTradeOpen(Obj, number);
		return;
	}
	if (lpObjTarget->m_PlayerData->ISBOT == true && g_BotSystem.GetBotType(*lpObjTarget) == BOT_BUFFER)
	{
		lpObjTarget->BuffPlayerIndex = Obj.m_Index;
		g_BotSystem.BuffPlayer(lpObjTarget->BufferIndex, Obj);
		return;
	}

	if ((lpObjTarget->m_Option & 1) != 1)
	{
		this->GCTradeResponseSend(0, Obj, lpObjTarget->Name, 0, 0);
		return;
	}

	if (Obj.m_IfState->use > 0)
	{
		this->GCTradeResponseSend(3, Obj, lpObjTarget->Name, 0, 0);
		return;
	}

	if (lpObjTarget->m_IfState->use > 0)
	{
		this->GCTradeResponseSend(2, Obj, lpObjTarget->Name, 0, 0);
		return;
	}

	if (g_NewPVP.IsDuel(Obj) || g_NewPVP.IsDuel(*lpObjTarget))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 311), Obj, 1);
		return;
	}

	if (g_NewPVP.IsObserver(Obj) || g_NewPVP.IsObserver(*lpObjTarget))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 312), Obj, 1);
		return;
	}

	if (::gObjFixInventoryPointer(Obj) == false)
	{

	}

	if (Obj.pTransaction == 1)
	{
		return;
	}

	Obj.m_IfState->use = 1;
	Obj.m_IfState->state = 0;
	Obj.m_IfState->type = 1;
	Obj.TargetNumber = number;
	lpObjTarget->m_IfState->use = 1;
	lpObjTarget->m_IfState->state = 0;
	lpObjTarget->m_IfState->type = 1;
	lpObjTarget->TargetNumber = Obj.m_Index;
	Obj.m_InterfaceTime = GetTickCount();
	lpObjTarget->m_InterfaceTime = GetTickCount();

	PMSG_TRADE_REQUESTSEND pMsg;

	pMsg.h.c = 0xC3;
	pMsg.h.headcode = 0x36;
	pMsg.h.size = sizeof(pMsg);
	std::memcpy(pMsg.szId, Obj.Name, sizeof(pMsg.szId));

	IOCP.DataSend(number, (BYTE*)&pMsg, pMsg.h.size);

	sLog->outBasic("Request to trade [%s][%s][%s] to [%s][%s][%s]", Obj.AccountID, Obj.Name, Obj.m_PlayerData->ConnectUser->IP,
		lpObjTarget->AccountID, lpObjTarget->Name, lpObjTarget->m_PlayerData->ConnectUser->IP);
}



void GameProtocol::CGTradeResponseRecv(PMSG_TRADE_RESPONSE * lpMsg, CGameObject &Obj)
{
	BOOL Result = TRUE;
	int number = Obj.TargetNumber;
	CGameObject* lpObjTarget = getGameObject(number);

	if (ObjectMaxRange(number) == FALSE)
	{
		if (Obj.m_IfState->use != FALSE)
		{
			if (Obj.m_IfState->type == 1)
			{
				Obj.m_IfState->use = FALSE;
				Obj.TargetNumber = -1;
			}
		}

		lpMsg->Response = false;
		GCTradeResponseSend(lpMsg->Response, Obj, Obj.Name, 0, 0);
		return;
	}

	if (gObjIsConnectedGP(Obj) == FALSE)
	{
		Result = FALSE;
		lpMsg->Response = false;
	}

	if (gObjIsConnectedGP(lpObjTarget) == FALSE)
	{
		Result = FALSE;
		lpMsg->Response = false;
	}

	if (Obj.CloseType != -1)
	{
		Result = FALSE;
		lpMsg->Response = false;
	}

	if (Obj.Type == OBJ_USER && Obj.m_bPShopOpen == true)
	{
		Result = FALSE;
		lpMsg->Response = false;
	}

	if (lpObjTarget->Type == OBJ_USER && lpObjTarget->m_bPShopOpen == true)
	{
		Result = FALSE;
		lpMsg->Response = false;
	}

	if (Obj.X < (lpObjTarget->X - 2) || Obj.X >(lpObjTarget->X + 2) || Obj.Y < (lpObjTarget->Y - 2) || Obj.Y >(lpObjTarget->Y + 2))
	{
		Result = FALSE;
		lpMsg->Response = false;
	}

	if (Obj.m_PlayerData->ISBOT == FALSE)
	{
		if (Obj.m_IfState->use == FALSE || Obj.m_IfState->type != 1 || Obj.m_IfState->state != 0)
		{
			return;
		}
	}

	if (lpObjTarget->m_IfState->use == FALSE || lpObjTarget->m_IfState->type != 1 || lpObjTarget->m_IfState->state != 0)
	{
		return;
	}

	if (gObjFixInventoryPointer(Obj) == false)
	{

	}

	if (Obj.pTransaction == 1 && Obj.m_PlayerData->ISBOT == FALSE)
	{
		return;
	}

	if (lpMsg->Response == false)
	{
		GCTradeResponseSend(lpMsg->Response, *lpObjTarget, Obj.Name, 0, 0);
		Result = FALSE;
	}
	else
	{
		Result = TRUE;

		for (int i = 0; i < TRADE_BOX_SIZE; i++)
		{
			Obj.pntTrade[i]->Clear();
			lpObjTarget->pntTrade[i]->Clear();
		}

		std::memset(Obj.TradeMap, (BYTE)-1, TRADE_BOX_MAP_SIZE);
		std::memset(lpObjTarget->TradeMap, (BYTE)-1, TRADE_BOX_MAP_SIZE);
		Obj.m_IfState->state = 1;
		lpObjTarget->m_IfState->state = 1;
		Obj.TradeMoney = 0;
		lpObjTarget->TradeMoney = 0;

		if (::gObjInventoryTrans(Obj) == FALSE)
			Result = 2;

		if (::gObjInventoryTrans(*lpObjTarget) == FALSE)
			Result = 3;

		if (Result != TRUE)
		{
			lpMsg->Response = false;
			GCTradeResponseSend(lpMsg->Response, *lpObjTarget, Obj.Name, 0, 0);
			GCTradeResponseSend(lpMsg->Response, Obj, lpObjTarget->Name, 0, 0);

			if (Result == 2)
			{

			}
			else if (Result == 3)
			{

			}

			Result = FALSE;
		}
		else
		{
			lpMsg->Response = true;
			GCTradeResponseSend(lpMsg->Response, Obj, Obj.Name, Obj.Level, Obj.m_PlayerData->GuildNumber);
			GCTradeResponseSend(lpMsg->Response, *lpObjTarget, lpObjTarget->Name, lpObjTarget->Level, lpObjTarget->m_PlayerData->GuildNumber);

		}
	}

	if (Result == FALSE)
	{
		if (Obj.m_IfState->use != FALSE && Obj.m_IfState->type == 1)
		{
			Obj.m_IfState->use = FALSE;
			Obj.TargetNumber = -1;
		}

		if (lpObjTarget->TargetNumber == Obj)
		{
			if (lpObjTarget->m_IfState->use != FALSE && lpObjTarget->m_IfState->type == 1)
			{
				lpObjTarget->m_IfState->use = FALSE;
				lpObjTarget->TargetNumber = -1;
			}
		}
	}
}




bool GameProtocol::GCTradeResponseSend(BYTE response, CGameObject &Obj, LPSTR id, WORD level, int GuildNumber)
{
	PMSG_TRADE_RESPONSE pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x37, sizeof(pMsg));
	pMsg.Response = response;
	pMsg.Level = level;
	pMsg.GuildNumber = GuildNumber;
	std::memcpy(pMsg.Id, id, sizeof(pMsg.Id));

	return IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}

struct PMSG_TRADE_OTHER_DEL
{
	PBMSG_HEAD2 h;	// C1:38
	BYTE TradeItemIndex;	// 3
};

int GameProtocol::GCTradeOtherDel(CGameObject &Obj, BYTE tradeindex)
{
	PMSG_TRADE_OTHER_DEL pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x38, sizeof(pMsg));
	pMsg.TradeItemIndex = tradeindex;

	return IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}

struct PMSG_TRADE_OTHER_MOVE
{
	PBMSG_HEAD2 h;	// C1:39
	BYTE TradeItemIndex;	// 3
	BYTE ItemInfo[MAX_ITEM_INFO];	// 4
};


int GameProtocol::GCTradeOtherAdd(CGameObject &Obj, BYTE tradeindex, BYTE* iteminfo)
{
	PMSG_TRADE_OTHER_MOVE pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x39, sizeof(pMsg));
	pMsg.TradeItemIndex = tradeindex;
	pMsg.ItemInfo[I_TYPE] = iteminfo[I_TYPE];
	pMsg.ItemInfo[I_OPTION] = iteminfo[I_OPTION];
	pMsg.ItemInfo[I_DUR] = iteminfo[I_DUR];
	pMsg.ItemInfo[I_NOPTION] = iteminfo[I_NOPTION];
	pMsg.ItemInfo[I_SOPTION] = iteminfo[I_SOPTION];
	pMsg.ItemInfo[5] = iteminfo[5];
	pMsg.ItemInfo[6] = iteminfo[6];
	pMsg.ItemInfo[7] = iteminfo[7];
	pMsg.ItemInfo[8] = iteminfo[8];
	pMsg.ItemInfo[9] = iteminfo[9];
	pMsg.ItemInfo[10] = iteminfo[10];
	pMsg.ItemInfo[11] = iteminfo[11];

	return IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}



void GameProtocol::CGTradeMoneyRecv(PMSG_TRADE_GOLD * lpMsg, CGameObject &Obj)
{
	PMSG_DEFRESULT pMsg;
	int money = 0;
	int number;

	if (!PacketCheckTime(Obj))
		return;

	if (Obj.CloseType != -1)
	{
		return;
	}

	number = Obj.TargetNumber;

	if (!ObjectMaxRange(number))
	{
		return;
	}

	if (getGameObject(number)->CloseType != -1)
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (lpObjTarget->Type != OBJ_USER)
	{
		return;
	}

	if (gObjIsConnectedGP(*lpObjTarget) == FALSE)
	{
		return;
	}

	if (Obj.m_IfState->use == FALSE && Obj.m_IfState->type != 1)
		return;

	if (gObjCanItemTouch(Obj, 1) == FALSE)
		return;

	if (lpMsg->Money > 1000000000)
		return;

	money = lpMsg->Money;

	if ((Obj.m_PlayerData->Money - money) < 0)
		return;

	GCMoneySend(Obj, Obj.m_PlayerData->Money - money);

	PHeadSetB((BYTE*)&pMsg, 0x3A, sizeof(pMsg));
	pMsg.result = 1;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

	if (number >= 0)
	{
		Obj.TradeMoney = money;
		Obj.TradeOk = false;
		getGameObject(number)->TradeOk = false;

		GCTradeOkButtonSend(*lpObjTarget, 2);
		GCTradeOkButtonSend(Obj, 0);
		GCTradeMoneyOther(*lpObjTarget, Obj.TradeMoney);
	}

	sLog->outBasic("[%s][%s][%s] -> [%s][%s][%s]: Trade Money %d", Obj.AccountID, Obj.Name, Obj.m_PlayerData->ConnectUser->IP, getGameObject(number)->AccountID, getGameObject(number)->Name, getGameObject(number)->m_PlayerData->ConnectUser->IP, money);
}

struct PMSG_TRADE_OTHER_GOLD
{
	PBMSG_HEAD2 h;	// C1:3B
	DWORD Money;	// 4
};


bool GameProtocol::GCTradeMoneyOther(CGameObject &Obj, DWORD money)
{
	PMSG_TRADE_OTHER_GOLD pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x3B, sizeof(pMsg));
	pMsg.Money = money;

	return IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}



bool GameProtocol::GCTradeOkButtonSend(CGameObject &Obj, BYTE flag)
{
	PMSG_TRADE_OKBUTTON pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x3C, sizeof(pMsg));
	pMsg.Flag = flag;

	return IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}


void GameProtocol::CGTradeOkButtonRecv(PMSG_TRADE_OKBUTTON * lpMsg, CGameObject &Obj)
{
	int number = Obj.TargetNumber;

	if (number < 0 || number > g_ConfigRead.server.GetObjectMax() - 1)
		return;
	CGameObject* lpObjTarget = getGameObject(number);

	if (lpObjTarget->Type == OBJ_USER && lpObjTarget->m_PlayerData->ISBOT == 1)
	{
		if (lpMsg->Flag == 1)
		{
			g_BotSystem.AlchemistTradeOk(Obj, number);
			return;
		}
	}

	if (!gObjIsConnected(*lpObjTarget))
		return;

	if (Obj.Type != OBJ_USER)
		return;

	if (lpObjTarget->Type != OBJ_USER)
		return;

	if (Obj.m_IfState->use < 1 || Obj.m_IfState->state != 1)
		return;

	if (!lpObjTarget->m_PlayerData->ISBOT)
	{
		if (lpObjTarget->m_IfState->use < 1 || lpObjTarget->m_IfState->state != 1)
			return;
	}
	if (Obj.CloseType != -1)
		return;

	if (lpObjTarget->CloseType != -1)
		return;

	if (lpMsg->Flag == 1)
	{
		if (Obj.TradeOk == false)
		{
			Obj.TradeOk = true;
			GCTradeOkButtonSend(*lpObjTarget, 1);
		}
	}
	else if (lpMsg->Flag == 0)
	{
		if (Obj.TradeOk == true)
		{
			GCTradeOkButtonSend(*lpObjTarget, 0);
			Obj.TradeOk = false;
		}
	}

	gObjTradeOkButton(Obj);
	sLog->outBasic("[%s][%s][%s] -> [%s][%s][%s]: Trade Accept!", Obj.AccountID, Obj.Name, Obj.m_PlayerData->ConnectUser->IP, lpObjTarget->AccountID, lpObjTarget->Name, lpObjTarget->m_PlayerData->ConnectUser->IP);
}


void GameProtocol::CGTradeCancelButtonRecv(CGameObject &Obj)
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	int number = Obj.TargetNumber;
	CGameObject* lpObjTarget = getGameObject(number);
	gObjTradeCancel(Obj);
	CGTradeResult(Obj, 0);

	if (number >= 0)
	{
		gObjTradeCancel(*lpObjTarget);
		CGTradeResult(*lpObjTarget, 0);
	}

	sLog->outBasic("[%s][%s][%s]: Trade Cancel Button Press", Obj.AccountID, Obj.Name, Obj.m_PlayerData->ConnectUser->IP);
}


struct PMSG_TRADE_RESULT
{
	PBMSG_HEAD2 h;	// C1:3D
	BYTE Result;	// 3
};


void GameProtocol::CGTradeResult(CGameObject &Obj, BYTE result)
{
	PMSG_TRADE_RESULT pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x3D, sizeof(pMsg));
	pMsg.Result = result;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}

struct PMSG_PARTYREQUESTSEND
{
	PBMSG_HEAD2 h;	// C1:40
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
};

void GameProtocol::CGPartyRequestRecv(PMSG_PARTYREQUEST * lpMsg, CGameObject &Obj)
{
	int number = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
	PMSG_PARTYREQUESTSEND pMsg;

	if (number < 0 || number > g_ConfigRead.server.GetObjectMax() - 1)
		return;

	if (!PacketCheckTime(Obj))
		return;

	if (!gObjIsConnected(*getGameObject(number)))
	{
		this->GCResultSend(Obj, 0x41, 0x03);
		return;
	}

	if (Obj.CloseCount >= 0 || getGameObject(number)->CloseCount >= 0)
	{
		this->GCResultSend(Obj, 0x41, 0x00);
		return;
	}

	if (getGameObject(number)->m_PlayerData->m_bUsePartyMatching)
	{
		GCResultSend(Obj, 0x41, 0x09);
		return;
	}

	if (getGameObject(number)->Type != OBJ_USER)
	{
		this->GCSendDisableReconnect(Obj);
		//IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
		return;
	}

	if (g_NewPVP.IsDuel(Obj) || g_NewPVP.IsDuel(*getGameObject(number)))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 315), Obj, 1);
		return;
	}

	if (g_NewPVP.IsObserver(Obj))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 316), Obj, 1);
		return;
	}

	if (CC_MAP_RANGE(Obj.MapNumber))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 116), Obj, 1);
		return;
	}

	if (Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 573), Obj, 1);
		return;
	}

	if (IT_MAP_RANGE(Obj.MapNumber))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 291), Obj, 1);
		return;
	}

	if (Obj.m_IfState->use > 0)
	{
		this->GCResultSend(Obj, 0x41, 0x00);
		return;
	}

	if (getGameObject(number)->m_IfState->use > 0)
	{
		this->GCResultSend(Obj, 0x41, 0x00);
		return;
	}

	if ((getGameObject(number)->m_Option & 1) != TRUE)
	{
		this->GCResultSend(Obj, 0x41, 0x01);
		return;
	}

	if (Obj.PartyNumber >= 0)
	{
		int iUserIndex;
		int iDbNumber;
		gParty.GetIndexUser(Obj.PartyNumber, 0, iUserIndex, iDbNumber);

		if (iUserIndex < 0 || (iUserIndex != Obj.m_Index && Obj.DBNumber != iDbNumber))
		{
			this->GCResultSend(Obj, 0x41, 0x00);
			return;

		}
	}

	if (Obj.PartyTargetUser >= 0)
		return;

	if (getGameObject(number)->PartyNumber >= 0)
	{
		this->GCResultSend(Obj, 0x41, 0x04);
		return;
	}

	if (getGameObject(number)->PartyTargetUser >= 0)
	{
		this->GCResultSend(Obj, 0x41, 0x00);
		return;
	}

	if (g_GensSystem.GetGensInfluence(&Obj) != g_GensSystem.GetGensInfluence(*getGameObject(number)) && g_GensSystem.CanGensJoinPartyWhileOppositeGens() == FALSE)
	{
		this->GCResultSend(Obj, 0x41, 0x06);
		return;
	}

	if (g_GensSystem.IsMapBattleZone(Obj.MapNumber) == TRUE && g_GensSystem.CanGensCreatePartyOnBattleZone() == FALSE)
	{
		this->GCResultSend(Obj, 0x41, 0x07);
		return;
	}

	if (g_ArcaBattle.IsArcaBattleServer() && Obj.m_PlayerData->GuildNumber != getGameObject(number)->m_PlayerData->GuildNumber)
	{
		if (g_AcheronGuardianEvent.IsPlayStart() == false)
		{
			GCServerMsgStringSend(Lang.GetText(0, 522), Obj, 1);
			return;
		}
	}

	if (Obj.m_PlayerData->RegisterdLMS == 1)
	{
		MsgOutput(Obj, Lang.GetText(0, 523));
		return;
	}

	if (getGameObject(number)->m_PlayerData->RegisterdLMS == 1)
	{
		MsgOutput(Obj, Lang.GetText(0, 524));
		return;
	}

	int maxlevel = 0;
	int minlevel = 0;

	if (getGameObject(number)->PartyNumber >= 0)
	{

		if (gParty.Isleader(getGameObject(number)->PartyNumber, *getGameObject(number), getGameObject(number)->DBNumber) == FALSE)
		{
			char szTemp[256];
			wsprintf(szTemp, "%s is already in a party.", getGameObject(number)->Name);
			GCServerMsgStringSend(szTemp, Obj, 1);
			return;
		}

		if (gParty.GetLevel(getGameObject(number)->PartyNumber, maxlevel, minlevel) == TRUE)
		{
			int limmaxlevel;
			int limmaxlevel2;

			if (maxlevel > getGameObject(number)->Level)
				limmaxlevel = maxlevel;
			else
				limmaxlevel = getGameObject(number)->Level;

			if (maxlevel < getGameObject(number)->Level)
				limmaxlevel2 = maxlevel;
			else
				limmaxlevel2 = getGameObject(number)->Level;

			if ((limmaxlevel - limmaxlevel2) > g_MaxStatsInfo.GetClass.Max_Party_Level_Dif)
			{
				this->GCResultSend(Obj, 0x41, 0x05);
				// Restricted level for the party is %d
				return;
			}

			int limminlevel;
			int limminlevel2;

			if (maxlevel > getGameObject(number)->Level)
				limminlevel = minlevel;
			else
				limminlevel = getGameObject(number)->Level;

			if (maxlevel < getGameObject(number)->Level)
				limminlevel2 = minlevel;
			else
				limminlevel2 = getGameObject(number)->Level;

			if ((limminlevel - limminlevel2) > g_MaxStatsInfo.GetClass.Max_Party_Level_Dif)
			{
				this->GCResultSend(Obj, 0x41, 0x05);
				// Restricted level for the party is %d
				return;
			}
		}
	}
	else
	{
		short sMaxMinLevel[2];

		if (Obj.Level > getGameObject(number)->Level)
			sMaxMinLevel[1] = Obj.Level;
		else
			sMaxMinLevel[1] = getGameObject(number)->Level;

		if (Obj.Level < getGameObject(number)->Level)
			sMaxMinLevel[0] = Obj.Level;
		else
			sMaxMinLevel[0] = getGameObject(number)->Level;

		if ((sMaxMinLevel[1] - sMaxMinLevel[0]) > g_MaxStatsInfo.GetClass.Max_Party_Level_Dif)
		{
			this->GCResultSend(Obj, 0x41, 0x05);
			// Restricted level for the party is %d
			return;
		}
	}

	Obj.m_IfState->use = TRUE;
	Obj.m_IfState->type = 2;
	Obj.m_IfState->state = 0;
	getGameObject(number)->m_IfState->use = TRUE;
	getGameObject(number)->m_IfState->type = 2;
	getGameObject(number)->m_IfState->state = 0;
	Obj.TargetNumber = number;
	getGameObject(number)->TargetNumber = Obj.m_Index;
	Obj.m_InterfaceTime = GetTickCount();
	getGameObject(number)->m_InterfaceTime = GetTickCount();
	Obj.PartyTargetUser = number;

	PHeadSetB((BYTE*)&pMsg, 0x40, sizeof(pMsg));
	pMsg.NumberH = SET_NUMBERH(Obj.m_Index);
	pMsg.NumberL = SET_NUMBERL(Obj.m_Index);

	IOCP.DataSend(number, (BYTE*)&pMsg, pMsg.h.size);
}



void GameProtocol::CGPartyRequestResultRecv(PMSG_PARTYREQUESTRESULT * lpMsg, CGameObject &Obj)
{
	int number;
	BYTE result = false;
	char szTemp[256];
	int pnumber = -1;

	if (!PacketCheckTime(Obj))
		return;

	number = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
	CGameObject* lpObjTarget = getGameObject(number);

	if (!gObjIsConnected(Obj))
		return;

	if (!gObjIsConnected(*lpObjTarget))
	{
		GCResultSend(Obj, 0x41, 0x04);
	}
	else
	{
		result = true;
	}

	if (Obj.m_PlayerData->m_bUsePartyMatching)
	{
		GCResultSend(Obj, 0x41, 0x09);
		return;
	}

	if (lpObjTarget->MapNumber != Obj.MapNumber)
	{
		result = false;
		GCResultSend(*lpObjTarget, 0x41, 0x00);
	}

	if (Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 573), Obj, 1);
		result = false;
		GCResultSend(*lpObjTarget, 0x41, 0x00);
	}

	if (CC_MAP_RANGE(Obj.MapNumber))
	{
		GCServerMsgStringSend(Lang.GetText(0, 116), Obj, 1);
		result = false;
		this->GCResultSend(*lpObjTarget, 0x41, 0x00);
	}

	if (lpMsg->Result == 0)
	{
		result = false;
		this->GCResultSend(*lpObjTarget, 0x41, 0x01);
	}

	if (lpObjTarget->m_IfState->use == 0 || lpObjTarget->m_IfState->type != 2)
	{
		result = false;
		this->GCResultSend(*lpObjTarget, 0x41, 0x00);
	}

	if (Obj.m_IfState->use == 0 || Obj.m_IfState->type != 2)
	{
		result = false;
		this->GCResultSend(*lpObjTarget, 0x41, 0x00);
	}

	if (result == 1)
	{
		if (lpObjTarget->PartyNumber < 0)
		{
			lpObjTarget->PartyNumber = gParty.Create(*lpObjTarget, lpObjTarget->DBNumber, lpObjTarget->Level);
		}

		if (lpObjTarget->PartyNumber >= 0)
		{
			// lpObjTarget = lpObjTarget->PartyNumber; // TODO line corrupted.
			int iPartyPos = gParty.Add(lpObjTarget->PartyNumber, Obj, Obj.DBNumber, Obj.Level);

			if (iPartyPos >= 0)
			{
				Obj.PartyNumber = lpObjTarget->PartyNumber;
				result = true;

				if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
				{
					wsprintf(szTemp, Lang.GetText(0, 20), Obj.m_PlayerData->m_RealNameOfUBF);
					this->GCServerMsgStringSend(szTemp, *lpObjTarget, 1);
					wsprintf(szTemp, Lang.GetText(0, 20), lpObjTarget->m_PlayerData->m_RealNameOfUBF);
					this->GCServerMsgStringSend(szTemp, Obj, 1);
				}

				else
				{
					wsprintf(szTemp, Lang.GetText(0, 20), Obj.Name);
					this->GCServerMsgStringSend(szTemp, *lpObjTarget, 1);
					wsprintf(szTemp, Lang.GetText(0, 20), lpObjTarget->Name);
					this->GCServerMsgStringSend(szTemp, Obj, 1);
				}

				gParty.Paint(p*lpObjTarget);

				if (lpObjTarget->m_PlayerData->m_bUsePartyMatching == true)
				{
					char szLeaderName[MAX_ACCOUNT_LEN + 1];
					std::memcpy(szLeaderName, lpObjTarget->Name, MAX_ACCOUNT_LEN + 1);
					GDReqAcceptMemberJoin(*lpObjTarget, 1, szLeaderName, Obj.Name, 1);
				}
			}
			else if (iPartyPos == -1)
			{
				GCResultSend(*lpObjTarget, 0x41, 2);
				GCResultSend(Obj, 0x41, 2);
			}
			else if (iPartyPos == -2)
			{
				result = 2;
				wsprintf(szTemp, Lang.GetText(0, 21), Obj.Name);
				GCServerMsgStringSend(szTemp, Obj, 1);
			}
		}
	}

	if (Obj.m_IfState->use != 0 && Obj.m_IfState->type == 2)
	{
		Obj.m_IfState->use = 0;
		Obj.PartyTargetUser = -1;
	}

	if (lpObjTarget->m_IfState->use != 0 && lpObjTarget->m_IfState->type == 2)
	{
		lpObjTarget->m_IfState->use = 0;
		lpObjTarget->PartyTargetUser = -1;
	}

	if (p*lpObjTarget >= 0)
	{
		CGPartyListAll(p*lpObjTarget);
	}
}

void GameProtocol::CGPartyList(CGameObject &Obj)
{
	PMSG_PARTYLISTCOUNT pCount;
	PMSG_PARTYLIST pList;
	BYTE sendbuf[512];
	int lOfs = 0;

	if (!this->PacketCheckTime(Obj))
		return;

	PHeadSetB((BYTE*)&pCount, 0x42, sizeof(pCount));
	pCount.Count = 0;
	pCount.Result = false;

	int number;
	int pnumber = Obj.PartyNumber;

	if (pnumber >= 0)
		pCount.Count = gParty.GetCount(pnumber);

	lOfs += sizeof(pCount);

	if (pCount.Count != 0)
	{
		for (int n = 0; n < MAX_USER_IN_PARTY; n++)
		{
			number = gParty.m_PartyS[pnumber]->Number[n];

			if (number >= 0)
			{
				if (gObjIsConnected(*getGameObject(number), gParty.m_PartyS[pnumber].DbNumber[n]) == TRUE)
				{
					std::memset(&pList, 0, sizeof(pList));
					std::memcpy(pList.szId, getGameObject(number)->Name, MAX_ACCOUNT_LEN);
					pList.Number = n;
					pList.MapNumber = getGameObject(number)->MapNumber;
					pList.X = getGameObject(number)->X;
					pList.Y = getGameObject(number)->Y;
					pList.Life = getGameObject(number)->Life;
					pList.MaxLife = getGameObject(number)->MaxLife + getGameObject(number)->AddLife;
					pList.nServerChannel = getGameObject(number)->m_PlayerData->m_nServerChannel + 1;
					pList.nMana = getGameObject(number)->Mana;
					pList.nMaxMana = getGameObject(number)->MaxMana;

					std::memcpy(&sendbuf[lOfs], &pList, sizeof(pList));
					lOfs += sizeof(pList);
				}
			}
		}

		pCount.Result = true;
	}

	pCount.h.size = lOfs;
	std::memcpy(sendbuf, &pCount, sizeof(pCount));

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&sendbuf, lOfs);
}



void GameProtocol::CGPartyListAll(int pnumber)
{
	PMSG_PARTYLISTCOUNT pCount;
	PMSG_PARTYLIST pList;
	BYTE sendbuf[512];
	int lOfs = 0;
	int number;

	if (pnumber < 0)
		return;

	if (gParty.IsParty(pnumber) == FALSE)
		return;

	PHeadSetB((BYTE*)&pCount, 0x42, sizeof(pCount));
	pCount.Result = false;
	pCount.Count = gParty.GetCount(pnumber);

	if (pCount.Count == 0)
		return;

	pCount.Result = true;
	lOfs += sizeof(pCount);

	for (int n = 0; n < MAX_USER_IN_PARTY; n++)
	{
		number = gParty.m_PartyS[pnumber].Number[n];

		if (number >= 0)
		{
			if (gObjIsConnected(*getGameObject(number), gParty.m_PartyS[pnumber].DbNumber[n]) == TRUE)
			{
				std::memset(&pList, 0, sizeof(pList));

				if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
				{
					std::memcpy(pList.szId, getGameObject(number)->m_PlayerData->m_RealNameOfUBF, MAX_ACCOUNT_LEN);
				}

				else
				{
					std::memcpy(pList.szId, getGameObject(number)->Name, MAX_ACCOUNT_LEN);
				}

				pList.Number = n;
				pList.MapNumber = getGameObject(number)->MapNumber;
				pList.X = getGameObject(number)->X;
				pList.Y = getGameObject(number)->Y;
				pList.Life = getGameObject(number)->Life;
				pList.MaxLife = getGameObject(number)->MaxLife;
				pList.nServerChannel = getGameObject(number)->m_PlayerData->m_nServerChannel + 1;
				pList.nMana = getGameObject(number)->Mana;
				pList.nMaxMana = getGameObject(number)->MaxMana;

				std::memcpy(&sendbuf[lOfs], &pList, sizeof(pList));
				lOfs += sizeof(pList);
			}
		}
	}

	pCount.h.size = lOfs;
	std::memcpy(sendbuf, &pCount, sizeof(pCount));

	for (int n = 0; n < MAX_USER_IN_PARTY; n++)
	{
		number = gParty.m_PartyS[pnumber].Number[n];

		if (number >= 0)
		{
			IOCP.DataSend(number, sendbuf, lOfs);
		}
	}

	for (int n = 0; n < MAX_USER_IN_PARTY; n++)
	{
		number = gParty.m_PartyS[pnumber].Number[n];

		if (number >= 0)
		{
			this->GCDisplayBuffeffectPartyMember(*getGameObject(number));
		}
	}
}


void GameProtocol::CGPartyDelUser(PMSG_PARTYDELUSER * lpMsg, CGameObject &Obj, int nType)
{
	int usernumber = -1;
	int dbnumber = -1;

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.m_PlayerData->m_bUsePartyMatching == true)
	{
		char szTargetName[MAX_ACCOUNT_LEN + 1];
		std::memcpy(szTargetName, lpMsg->szTargetName, MAX_ACCOUNT_LEN + 1);

		if (nType == 0)
		{
			std::memcpy(szTargetName, Obj.Name, MAX_ACCOUNT_LEN + 1);
		}

		if (nType == 1)
		{
			GDReqDeletePartyUser(Obj, szTargetName);
		}

		if (Obj.PartyNumber < 0)
		{
			return;
		}

		if (gParty.RevisionIndexUser(Obj.PartyNumber, &lpMsg->Number, szTargetName) == FALSE)
		{
			return;
		}

	}

	if (Obj.PartyNumber < 0)
	{
		return;
	}

	if (!gParty.GetIndexUser(Obj.PartyNumber, lpMsg->Number, usernumber, dbnumber))
	{
		return;
	}

	if (usernumber < 0)
	{
		return;
	}

	int count;
	int number;
	int pnumber = Obj.PartyNumber;

	if (pnumber < 0)
	{
		return;
	}

	count = gParty.GetCount(pnumber);

	if (count < 1)
	{
		return;
	}

	if (!DG_MAP_RANGE(getGameObject(usernumber)->MapNumber))
	{
		if (!lpMsg->Number)
		{
			BOOL bDPUser = FALSE;

			for (int n = 1; n < 5; n++)
			{
				int iIndex = gParty.m_PartyS[pnumber].Number[n];

				if (iIndex != -1)
				{
					if (DG_MAP_RANGE(Obj.MapNumber))
					{
						bDPUser = TRUE;
						break;
					}
				}
			}

			if (bDPUser == TRUE && count > 2)
			{
				gParty.Delete(pnumber, lpMsg->Number);
				count = gParty.GetCount(Obj.PartyNumber);
				getGameObject(usernumber)->PartyNumber = -1;
				getGameObject(usernumber)->PartyTargetUser = -1;
				gParty.UpdatePKPartyPanalty(pnumber);
				gParty.ChangeLeader(pnumber);
				this->GCPartyDelUserSend(*getGameObject(usernumber));
				this->CGPartyListAll(pnumber);
				return;
			}
		}

		if (!IMPERIAL_MAP_RANGE(getGameObject(usernumber)->MapNumber))
		{
			if (!lpMsg->Number)
			{
				BOOL bIGUser = FALSE;

				for (int i = 1; i < 5; i++)
				{
					int iIndex = gParty.m_PartyS[pnumber].Number[i];
					if (iIndex != -1)
					{
						if (IMPERIAL_MAP_RANGE(Obj.MapNumber))
						{
							bIGUser = TRUE;
							break;
						}
					}
				}

				if (bIGUser == TRUE && count > 2)
				{
					gParty.Delete(pnumber, lpMsg->Number);
					count = gParty.GetCount(Obj.PartyNumber);
					getGameObject(usernumber)->PartyNumber = -1;
					getGameObject(usernumber)->PartyTargetUser = -1;
					gParty.UpdatePKPartyPanalty(pnumber);
					gParty.ChangeLeader(pnumber);
					this->GCPartyDelUserSend(*getGameObject(usernumber));
					this->CGPartyListAll(pnumber);
					return;
				}
			}

			if (getGameObject(usernumber)->m_PlayerData->lpGuild)
			{
				if (getGameObject(usernumber)->m_PlayerData->lpGuild->WarState == 1 && getGameObject(usernumber)->IsInBattleGround)
				{
					this->GCServerMsgStringSend(Lang.GetText(0, 574), Obj, TRUE);
					return;
				}

				if (usernumber == gParty.m_PartyS[pnumber].Number[0])
				{
					for (int j = 0; j < 5; j++)
					{
						int number = gParty.m_PartyS[pnumber].Number[j];
						if (number >= 0
							&& getGameObject(number)->m_PlayerData->lpGuild
							&& getGameObject(number)->m_PlayerData->lpGuild->WarState == TRUE
							&& getGameObject(number)->IsInBattleGround)
						{
							this->GCServerMsgStringSend(Lang.GetText(0, 574), Obj, TRUE);
							return;
						}
					}
				}
			}

			if (lpMsg->Number && count > 2)
			{
				gParty.Delete(pnumber, lpMsg->Number);
				count = gParty.GetCount(Obj.PartyNumber);
				getGameObject(usernumber)->PartyNumber = -1;
				getGameObject(usernumber)->PartyTargetUser = -1;
				gParty.UpdatePKPartyPanalty(pnumber);
				this->GCPartyDelUserSend(getGameObject(usernumber));
				CGPartyListAll(pnumber);
			}
			else
			{
				if (!IT_MAP_RANGE(getGameObject(usernumber)->MapNumber) || lpMsg->Number || count <= 2)
				{
					if (IT_MAP_RANGE(getGameObject(usernumber)->MapNumber) && count <= 2)
					{
						gParty.Delete(pnumber, lpMsg->Number);
						count = gParty.GetCount(Obj.PartyNumber);
						getGameObject(usernumber)->PartyNumber = -1;
						getGameObject(usernumber)->PartyTargetUser = -1;
						gParty.UpdatePKPartyPanalty(pnumber);
						if (!lpMsg->Number)
							gParty.ChangeLeader(pnumber);
						this->GCPartyDelUserSend(*getGameObject(usernumber));
						this->CGPartyListAll(pnumber);
					}
					else
					{
						if (!ITL_MAP_RANGE(getGameObject(usernumber)->MapNumber) || lpMsg->Number || count <= 2)
						{
							if (ITL_MAP_RANGE(getGameObject(usernumber)->MapNumber) && count <= 2)
							{
								gParty.Delete(pnumber, lpMsg->Number);
								count = gParty.GetCount(Obj.PartyNumber);
								getGameObject(usernumber)->PartyNumber = -1;
								getGameObject(usernumber)->PartyTargetUser = -1;
								gParty.UpdatePKPartyPanalty(pnumber);
								if (!lpMsg->Number)
									gParty.ChangeLeader(pnumber);
								this->GCPartyDelUserSend(getGameObject(usernumber));
								this->CGPartyListAll(pnumber);
							}
							else
							{
								for (int index = 0; index < 5; index++)
								{
									number = gParty.m_PartyS[pnumber].Number[index];

									if (number >= 0)
									{
										gParty.Delete(pnumber, index);
										getGameObject(number)->PartyNumber = -1;
										getGameObject(number)->PartyTargetUser = -1;
										if (getGameObject(number)->m_PlayerData->m_bUsePartyMatching == true)
											this->GCPartyDelUserSendNoMessage(*getGameObject(usernumber));
										else
											this->GCPartyDelUserSend(*getGameObject(usernumber));
									}
								}

								gParty.Destroy(pnumber);
							}
						}
						else
						{
							gParty.Delete(pnumber, lpMsg->Number);
							count = gParty.GetCount(Obj.PartyNumber);
							getGameObject(usernumber)->PartyNumber = -1;
							getGameObject(usernumber)->PartyTargetUser = -1;
							gParty.UpdatePKPartyPanalty(pnumber);
							gParty.ChangeLeader(pnumber);
							this->GCPartyDelUserSend(*getGameObject(usernumber));
							this->CGPartyListAll(pnumber);
							if (!count)
								gParty.Destroy(pnumber);
						}
					}
				}
				else
				{
					gParty.Delete(number, lpMsg->Number);
					count = gParty.GetCount(Obj.PartyNumber);
					getGameObject(usernumber)->PartyNumber = -1;
					getGameObject(usernumber)->PartyTargetUser = -1;
					gParty.UpdatePKPartyPanalty(pnumber);
					gParty.ChangeLeader(pnumber);
					this->GCPartyDelUserSend(*getGameObject(usernumber));
					this->CGPartyListAll(pnumber);
					if (!count)
						gParty.Destroy(pnumber);
				}
			}
		}
	}
}

struct PMSG_PARTYDELUSERSEND
{
	PBMSG_HEAD2 h;	// C1:43
	BYTE btSendMessage;
};


void GameProtocol::GCPartyDelUserSend(CGameObject &Obj)
{
	PMSG_PARTYDELUSERSEND pMsg;

	pMsg.btSendMessage = FALSE;
	PHeadSetB((BYTE*)&pMsg, 0x43, sizeof(pMsg));

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCPartyDelUserSendNoMessage(CGameObject &Obj)
{
	PMSG_PARTYDELUSERSEND pMsg;

	pMsg.btSendMessage = TRUE;
	PHeadSetB((BYTE*)&pMsg, 0x43, sizeof(pMsg));

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}

struct PMSG_GUILDQUESTSEND
{
	PBMSG_HEAD2 h;	// C1:50
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
};

void GameProtocol::CGGuildRequestRecv(PMSG_GUILDJOINQ * lpMsg, CGameObject &Obj)
{
	PMSG_GUILDQUESTSEND pMsg;
	int number = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
	CGameObject* lpObjTarget = getGameObject(number);

	if (!gObjIsConnected(lpObjTarget))
	{
		GCResultSend(Obj, 0x51, 0x03);
		return;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (g_CastleSiegeSync.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
		{
			this->GCServerMsgStringSend(Lang.GetText(0, 195), Obj, 1);
			return;
		}
	}

	if (lpObjTarget->Type != OBJ_USER)
	{
		this->GCSendDisableReconnect(Obj);
		//IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
		return;
	}

	if (Obj.m_PlayerData->m_bSecurityCheck == false)
	{
		GCResultSend(Obj, 0x51, 0x06);
		return;
	}

	if (Obj.m_IfState->use > 0)
	{
		GCResultSend(Obj, 0x51, 0x06);
		return;
	}

	if (lpObjTarget->m_IfState->use > 0)
	{
		GCResultSend(Obj, 0x51, 0x06);
		return;
	}

	if (IT_MAP_RANGE(Obj.MapNumber))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 292), Obj, 1);
		return;
	}

	if (g_NewPVP.IsDuel(Obj) || g_NewPVP.IsDuel(*lpObjTarget))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 317), Obj, 1);
		return;
	}

	if (g_NewPVP.IsObserver(Obj))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 318), Obj, 1);
		return;
	}

	if (Obj.Level < 6)
	{
		GCResultSend(Obj, 0x51, 0x07);
		return;
	}

	if (Obj.m_PlayerData->GuildNumber > 0)
	{
		GCResultSend(Obj, 0x51, 0x05);
		return;
	}

	if ((lpObjTarget->m_Option & 1) != 1)
	{
		GCResultSend(Obj, 0x51, 0x00);
		return;
	}

	if (lpObjTarget->m_PlayerData->GuildNumber > 0)
	{
		if (strcmp(lpObjTarget->m_PlayerData->lpGuild->Users[0]->Name, lpObjTarget->Name))
		{
			if (lpObjTarget->m_PlayerData->GuildStatus != 64)
			{
				GCResultSend(Obj, 0x51, 0x04);
				return;
			}
		}

		if (lpObjTarget->m_PlayerData->lpGuild->WarState)
		{
			GCResultSend(Obj, 0x51, 0x06);
			return;
		}

		int MaxGuildMember;

		if (lpObjTarget->Class == CLASS_DARKLORD)
			MaxGuildMember = (lpObjTarget->Level + lpObjTarget->m_PlayerData->MasterLevel) / 10 + lpObjTarget->Leadership / 10;
		else
			MaxGuildMember = (lpObjTarget->Level + lpObjTarget->m_PlayerData->MasterLevel) / 10;

		if (MaxGuildMember > g_ConfigRead.data.common.MaxGuildMember)
			MaxGuildMember = g_ConfigRead.data.common.MaxGuildMember;

		if (lpObjTarget->m_PlayerData->lpGuild->TotalCount >= MaxGuildMember)
		{
			GCResultSend(Obj, 0x51, 0x02);
			return;
		}

		int OwnInfluence = g_GensSystem.GetGensInfluence(&Obj);
		int TarInfluence = g_GensSystem.GetGensInfluence(*lpObjTarget);

		int iArcaBattleState = g_ArcaBattle.GetState();

		if (iArcaBattleState > 2 && iArcaBattleState < 9)
		{
			this->GCResultSend(Obj, 0x51, 0xA4);
			return;
		}

		if (OwnInfluence <= NONE_INFLUENCE)
		{
			GCResultSend(Obj, 0x51, 0xA3);
			return;
		}

		if (TarInfluence <= NONE_INFLUENCE)
		{
			GCResultSend(Obj, 0x51, 0xA1);
			return;
		}

		if (OwnInfluence != TarInfluence && g_GensSystem.CanGensJoinGuildWhileOppositeGens() == FALSE)
		{
			GCResultSend(Obj, 0x51, 0xA2);
			return;
		}
	}
	else
	{
		GCResultSend(Obj, 0x51, 0x04);
		return;
	}

	Obj.m_IfState->use = 1;
	Obj.m_IfState->type = 4;
	Obj.m_IfState->state = 0;

	lpObjTarget->m_IfState->use = 1;
	lpObjTarget->m_IfState->type = 4;
	lpObjTarget->m_IfState->state = 0;

	Obj.TargetNumber = number;
	lpObjTarget->TargetNumber = Obj.m_Index;
	Obj.m_InterfaceTime = GetTickCount();
	lpObjTarget->m_InterfaceTime = GetTickCount();

	PHeadSetB((BYTE*)&pMsg, 0x50, sizeof(pMsg));
	pMsg.NumberH = SET_NUMBERH(Obj.m_Index);
	pMsg.NumberL = SET_NUMBERL(Obj.m_Index);

	IOCP.DataSend(number, (BYTE*)&pMsg, pMsg.h.size);

}



void GameProtocol::CGGuildRequestResultRecv(PMSG_GUILDQRESULT * lpMsg, CGameObject &Obj) //GS-CS Decompiled 100%
{
	int number;
	BYTE result = 0;
	int pnumber = -1;

	if (!PacketCheckTime(Obj))
		return;

	if (g_CastleSiegeSync.GetCastleState() == 7) //Good
	{
		MsgOutput(Obj, (Lang.GetText(0, 195)));
		return;
	}

	number = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	if (number < 0 || number > g_ConfigRead.server.GetObjectMax() - 1)
		return;

	if (!gObjIsConnected(&Obj))
	{
		GCResultSend(Obj, 0x51, 0x02);
	}
	else
	{
		result = 1;
	}

	if (lpMsg->Result == 0)
	{
		result = 0;
		GCResultSend(Obj, 0x51, 0x00);
	}

	if (result == 1)
	{
		if (Obj.m_PlayerData->GuildNumber > 0)
		{
			GDGuildMemberAdd(number, Obj.m_PlayerData->lpGuild->Name, getGameObject(number)->Name);
			GDReqCancelJoinGuildMatching(number, Obj.Name, 1);
		}
	}

	if (Obj.m_IfState->use && Obj.m_IfState->type == 4)
		Obj.m_IfState->use = 0;

	if (getGameObject(number)->m_IfState->use && getGameObject(number)->m_IfState->type == 4)
		getGameObject(number)->m_IfState->use = 0;
}

struct PMSG_GUILDLIST
{
	char Name[10];	// 0
	BYTE Number;	// A
	BYTE ConnectAServer;	// B
	BYTE btGuildStatus;	// C
};

struct PMSG_GUILDLISTCOUNT_EX700
{
	PWMSG_HEAD h;	// C2:52
	BYTE Result;	// 4
	BYTE Count;	// 5
	int TotalScore;	// 8
	BYTE Score;	// C
	char szRivalGuild[5][8];	// D
};

struct PMSG_GUILDLISTCOUNT_S6
{
	PWMSG_HEAD h;	// C2:52
	BYTE Result;	// 4
	BYTE Count;	// 5
	int TotalScore;	// 8
	BYTE Score;	// C
	char szRivalGuild[8];
};

void GameProtocol::CGGuildListAll(int pnumber)
{
	if (getGameObject(pnumber)->Type != OBJ_USER)
	{
		return;
	}

	if (getGameObject(pnumber)->m_PlayerData->lpGuild == NULL)
		return;

	PMSG_GUILDLISTCOUNT_EX700 pCount;
	PMSG_GUILDLIST pList;
	char sendbuf[1500];
	int lOfs = 0;
	int number;
	int n;
	int guildmaster = 0;

	PHeadSetW((BYTE*)&pCount, 0x52, 0);
	pCount.Result = 0;
	pCount.Count = 0;
	lOfs += sizeof(pCount);
	pCount.h.sizeH = SET_NUMBERH(lOfs);
	pCount.h.sizeL = SET_NUMBERL(lOfs);

	if (getGameObject(pnumber)->m_PlayerData->GuildNumber < 1)
	{
		IOCP.DataSend(pnumber, (BYTE*)&pCount, lOfs);
		return;
	}

	pCount.Count = getGameObject(pnumber)->m_PlayerData->lpGuild->Count;
	pCount.TotalScore = getGameObject(pnumber)->m_PlayerData->lpGuild->TotalScore;
	pCount.Score = getGameObject(pnumber)->m_PlayerData->lpGuild->PlayScore;
	std::memset(pCount.szRivalGuild, 0, sizeof(pCount.szRivalGuild));

	if (getGameObject(pnumber)->m_PlayerData->lpGuild->iGuildRival)
	{
		int iGuildCount = 0;
		int iGuildList[MAX_UNION_GUILD] = { 0 };

		if (UnionManager.GetGuildUnionMemberList(getGameObject(pnumber)->m_PlayerData->lpGuild->iGuildRival, iGuildCount, iGuildList) == TRUE)
		{
			for (int i = 0; i < iGuildCount; i++)
			{
				GUILD_INFO_STRUCT *lpGuildInfo = Guild.SearchGuild_Number(iGuildList[i]);

				if (!lpGuildInfo)
					continue;

				std::memcpy(pCount.szRivalGuild[i], lpGuildInfo->Name, MAX_GUILD_LEN);
			}
		}

		else
		{
			std::memcpy(pCount.szRivalGuild[0], getGameObject(pnumber)->m_PlayerData->lpGuild->szGuildRivalName, MAX_GUILD_LEN);
		}
	}

	pCount.Result = 1;
	guildmaster = 1;

	if (pCount.Count == 0)
		return;

	pCount.Count = 0;

	for (n = 0; n < MAX_USER_GUILD; n++)
	{
		number = getGameObject(pnumber)->m_PlayerData->lpGuild->Use[n];

		if (number > 0)
		{
			std::memset(&pList, 0, sizeof(pList));
			std::memcpy(pList.Name, getGameObject(pnumber)->m_PlayerData->lpGuild->Users[n]->Name, sizeof(pList.Name));
			pList.Number = getGameObject(pnumber)->m_PlayerData->lpGuild->pServer[n];
			pList.ConnectAServer = (short)getGameObject(pnumber)->m_PlayerData->lpGuild->pServer[n] & 0x7F;

			if (getGameObject(pnumber)->m_PlayerData->lpGuild->pServer[n] >= 0)
				pList.ConnectAServer |= 0x80;

			pList.btGuildStatus = getGameObject(pnumber)->m_PlayerData->lpGuild->GuildStatus[n];
			std::memcpy(&sendbuf[lOfs], &pList, sizeof(pList));
			lOfs += sizeof(pList);
			pCount.Count++;
		}
	}

	pCount.h.sizeH = SET_NUMBERH(lOfs);
	pCount.h.sizeL = SET_NUMBERL(lOfs);
	std::memcpy(sendbuf, (BYTE*)&pCount, sizeof(pCount));

	IOCP.DataSend(pnumber, (BYTE*)sendbuf, lOfs);
}

struct PMSG_GUILDDELUSER_RESULT
{
	PBMSG_HEAD2 h;	// C1:53
	BYTE Result;	// 3
};

void GameProtocol::CGGuildDelUser(PMSG_GUILDDELUSER * lpMsg, CGameObject &Obj)
{
	if (!PacketCheckTime(Obj))
		return;

	if (Obj < g_ConfigRead.server.GetObjectStartUserIndex() || Obj > g_ConfigRead.server.GetObjectMax() - 1)
		return;

	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (g_CastleSiegeSync.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE) //Good
		{
			MsgOutput(Obj, Lang.GetText(0, 196));
			return;
		}
	}

	if (Obj.m_PlayerData->GuildNumber < 1)
		return;

	char memberid[11];
	char guildname[11];
	char joomin[21];
	std::memset(joomin, 0, sizeof(joomin));
	PMSG_GUILDDELUSER_RESULT pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x53, sizeof(pMsg));
	pMsg.Result = 3;

	if (!g_ConfigRead.data.common.GuildDestroy)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	int iArcaBattleState = g_ArcaBattle.GetState();

	if (iArcaBattleState > 2 && iArcaBattleState < 9)
	{
		pMsg.Result = 6;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (Obj.m_PlayerData->m_bSecurityCheck == false)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	std::memset(memberid, 0, sizeof(memberid));
	std::memset(guildname, 0, sizeof(guildname));
	std::memcpy(memberid, lpMsg->Name, MAX_ACCOUNT_LEN);
	std::strcpy(guildname, Obj.m_PlayerData->lpGuild->Name);

	if (!strcmp(Obj.m_PlayerData->lpGuild->Users[0]->Name, Obj.Name))
	{
		std::memset(joomin, 0, sizeof(joomin));
		std::memcpy(joomin, lpMsg->Password, sizeof(lpMsg->Password));

		if (gObjPasswordCheck(Obj, joomin) == FALSE)
		{
			pMsg.Result = 0;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
			return;
		}

		if (!strcmp(memberid, Obj.Name))
		{
			if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && g_ConfigRead.data.common.CastleGuildDestroyLimit == TRUE)
			{
				if (strcmp(Obj.m_PlayerData->lpGuild->Name, g_CastleSiegeSync.GetCastleOwnerGuild()) == 0)
				{
					MsgOutput(Obj, Lang.GetText(0, 194));
					return;
				}
			}

			gObjGuildWarMasterClose(&Obj);
			GDGuildDestroySend(Obj, Obj.m_PlayerData->lpGuild->Name, Obj.Name);
			g_ArcaBattle.ReqRemoveRewardGuildBuff(Obj.m_PlayerData->lpGuild->Name);
			g_ArcaBattle.GDReqMarkRegDel(Obj, Obj.m_PlayerData->GuildNumber);
		}
		else
		{
			if (Guild.GetGuildMemberStatus(Obj.m_PlayerData->lpGuild->Name, memberid) == -1)
			{
				pMsg.Result = 0;
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
				return;
			}

			g_ArcaBattle.ReqGuildMemberDelBuffDel(memberid, guildname);
			GDGuildMemberDel(Obj, guildname, memberid);
		}

	}
	else
	{
		if (strcmp(memberid, Obj.Name))
		{
			pMsg.Result = 0;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
			return;
		}

		std::memset(joomin, 0, sizeof(joomin));
		std::memcpy(joomin, lpMsg->Password, sizeof(lpMsg->Password));

		if (gObjPasswordCheck(Obj, joomin) == FALSE)
		{
			MsgOutput(Obj, Lang.GetText(0, 525));
		}

		else
		{
			g_ArcaBattle.ReqGuildMemberDelBuffDel(Obj, guildname);
			GDGuildMemberDel(Obj, guildname, memberid);
		}

	}
}



void GameProtocol::GCGuildDelUserResult(CGameObject &Obj, BYTE Result)
{
	PMSG_GUILDDELUSER_RESULT pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x53, sizeof(pMsg));
	pMsg.Result = Result;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}



struct PMSG_GUILDMASTERQUESTION
{
	PBMSG_HEAD2 h;	// C1:54
};

void GameProtocol::GCGuildMasterQuestionSend(CGameObject &Obj)
{
	if (Obj.m_IfState->use > 0)
	{
		return;
	}

	PMSG_GUILDMASTERQUESTION pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x54, sizeof(pMsg));
	Obj.m_IfState->use = 1;
	Obj.m_IfState->type = 5;
	Obj.m_IfState->state = 0;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}



void GameProtocol::CGGuildMasterAnswerRecv(PMSG_GUILDMASTERANSWER * lpMsg, CGameObject &Obj)
{
	if (lpMsg->Result == 1)
	{
		if (!gObjGuildMasterCapacityTest(&Obj))
		{
			if (Obj.m_IfState->use && Obj.m_IfState->type == 5)
			{
				Obj.m_IfState->use = 0;
			}

			return;
		}

		GCGuildMasterManagerRun(Obj);
		return;
	}

	if (Obj.m_IfState->use && Obj.m_IfState->type == 5)
	{
		Obj.m_IfState->use = 0;
	}
}


struct PMSG_GUILDMASTERMANAGER_RUN
{
	PBMSG_HEAD2 h;	// C1:55
};

void GameProtocol::GCGuildMasterManagerRun(CGameObject &Obj)
{
	PMSG_GUILDMASTERMANAGER_RUN pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x55, sizeof(pMsg));

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}



void GameProtocol::CGGuildMasterInfoSave(CGameObject &Obj, PMSG_GUILDINFOSAVE * lpMsg)
{
	char GuildName[MAX_GUILD_LEN + 1];
	GuildName[MAX_GUILD_LEN] = '\0';
	std::memcpy(GuildName, lpMsg->GuildName, MAX_GUILD_LEN);
	int len = strlen(GuildName);

	if (len <= 2 || len > MAX_GUILD_LEN)
	{
		PMSG_GUILDCREATED_RESULT pMsg;

		PHeadSetB((BYTE*)&pMsg, 0x56, sizeof(pMsg));
		pMsg.Result = 2;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

		if (Obj.m_IfState->use && Obj.m_IfState->type == 5)
		{
			Obj.m_IfState->use = 0;
		}

		return;
	}

	if (!g_prohibitedSymbols.Validate(GuildName, len, TYPE_GUILDNAME))
	{
		MsgOutput(Obj, Lang.GetText(0, 546));
		return;
	}

	GDReqCancelJoinGuildMatching(Obj, Obj.Name, 1);
	GDGuildCreateSend(Obj, GuildName, Obj.Name, lpMsg->Mark, lpMsg->btGuildType);

	if (Obj.m_IfState->use && Obj.m_IfState->type == 5)
	{
		Obj.m_IfState->use = 0;
	}
}



void GameProtocol::CGGuildMasterCreateCancel(CGameObject &Obj)
{
	if (Obj.Type != OBJ_USER)
		return;

	if (Obj.m_PlayerData->GuildNumber > 0)
		return;

	if (Obj.m_IfState->use && Obj.m_IfState->type == 5)
	{
		Obj.m_IfState->use = 0;
	}
}


void GameProtocol::GCGuildViewportNowPaint(CGameObject &Obj, char* guildname, BYTE* mark, BOOL isGuildMaster)
{
	GUILD_INFO_STRUCT * lpGuild = Guild.SearchGuild(guildname);

	if (lpGuild == NULL)
		return;

	
	BYTE _GuildInfoBuf[256] = { 0 };
	int _GuildInfoOfs = 5;

	if (Obj.m_PlayerData->lpGuild == NULL)
		return;

	PMSG_SIMPLE_GUILDVIEWPORT pMsg;
	pMsg.GuildNumber = Obj.m_PlayerData->lpGuild->Number;
	pMsg.NumberH = SET_NUMBERH(Obj.m_Index) & 0x7F;
	pMsg.NumberL = SET_NUMBERL(Obj.m_Index);

	if (isGuildMaster != FALSE)
		pMsg.NumberH |= 0x80;

	if (isGuildMaster != FALSE)
		pMsg.btGuildStatus = 0x80;
	else
		pMsg.btGuildStatus = Obj.m_PlayerData->GuildStatus;

	pMsg.btGuildType = Obj.m_PlayerData->lpGuild->btGuildType;
	pMsg.btGuildRelationShip = 0;
	Obj.m_PlayerData->iGuildUnionTimeStamp = 0;
	std::memcpy(&_GuildInfoBuf[_GuildInfoOfs], &pMsg, sizeof(pMsg));
	_GuildInfoOfs += sizeof(pMsg);

	PMSG_SIMPLE_GUILDVIEWPORT_COUNT pGVCount;

	pGVCount.h.c = 0xC2;
	pGVCount.h.headcode = 0x65;
	pGVCount.h.sizeH = SET_NUMBERH(_GuildInfoOfs);
	pGVCount.h.sizeL = SET_NUMBERL(_GuildInfoOfs);
	pGVCount.Count = 1;
	std::memcpy(_GuildInfoBuf, &pGVCount, sizeof(pGVCount));

	if (!CC_MAP_RANGE(Obj.MapNumber) && Obj.MapNumber != MAP_INDEX_CHAOSCASTLE_SURVIVAL)
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, _GuildInfoBuf, _GuildInfoOfs);

	MsgSendV2(lpObj, _GuildInfoBuf, _GuildInfoOfs);
}



struct PMSG_GUILDDEL_VIEWPORT_NOW
{
	PBMSG_HEAD2 h;	// C1:5D
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
};



void GameProtocol::GCGuildViewportDelNow(CGameObject &Obj, BOOL isGuildMaster)
{
	PMSG_GUILDDEL_VIEWPORT_NOW pMsg;

	

	PHeadSetB((BYTE*)&pMsg, 0x5D, sizeof(pMsg));
	pMsg.NumberH = SET_NUMBERH(Obj.m_Index) & 0x7F;
	pMsg.NumberL = SET_NUMBERL(Obj.m_Index);

	if (isGuildMaster != FALSE)
	{
		pMsg.NumberH |= 0x80;
	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	MsgSendV2(lpObj, (BYTE*)&pMsg, pMsg.h.size);
}



void GameProtocol::GCManagerGuildWarEnd(char * GuildName)
{
	GUILD_INFO_STRUCT * lpNode = Guild.SearchGuild(GuildName);

	if (lpNode == NULL)
	{
		return;
	}

	int n = 0;
	int warmaster = -1;

	while (true)
	{
		if (getGameObject(n)->Type == OBJ_USER)
		{
			if (getGameObject(n)->Connected > PLAYER_LOGGED)
			{
				if (getGameObject(n)->Name == lpNode->Users[0]->Name)
				{
					if (strcmp(getGameObject(n)->Name, lpNode->Users[0]->Name) == 0)
					{
						warmaster = n;
						break;
					}
				}
			}
		}

		if (n < g_ConfigRead.server.GetObjectMax() - 1)
		{
			n++;
		}
		else
		{
			break;
		}
	}

	if (warmaster >= 1)
	{
		BYTE Result1 = 0;
		BYTE Result2 = 0;

		if (lpNode != NULL)
		{
			if (lpNode->lpTargetGuildNode != NULL)
			{
				if (lpNode->PlayScore == 0 && lpNode->lpTargetGuildNode->PlayScore == 0)
				{
					Result1 = 6;
					Result2 = 6;
				}
				else
				{
					Result1 = 0;
					Result2 = 1;
				}
			}
		}

		if (lpNode->WarType == 1)
		{
			gBattleGroundEnable(lpNode->BattleGroundIndex, FALSE);
			gObjGuildWarEndSend(*lpNode, *lpNode->lpTargetGuildNode, Result1, Result2);
			gObjGuildWarEnd(*lpNode, *lpNode->lpTargetGuildNode);

			cManager.BattleInfoSend(::GetBattleTeamName(0, 0), -1, ::GetBattleTeamName(0, 1), -1);
		}
		else
		{
			gObjGuildWarEndSend(*lpNode, *lpNode->lpTargetGuildNode, Result1, Result2);
			gObjGuildWarEnd(*lpNode, *lpNode->lpTargetGuildNode);
		}
	}
}



void GameProtocol::GCManagerGuildWarSet(LPSTR GuildName1, LPSTR GuildName2, int type)
{
	GUILD_INFO_STRUCT * lpNode = Guild.SearchGuild(GuildName1);

	if (lpNode == NULL)
	{
		return;
	}

	int n = 0;
	int warmaster = -1;

	while (true)
	{
		if (getGameObject(n)->Type == OBJ_USER)
		{
			if (getGameObject(n)->Connected > PLAYER_LOGGED)
			{
				if (getGameObject(n)->Name == lpNode->Users[0]->Name)
				{
					if (strcmp(getGameObject(n)->Name, lpNode->Users[0]->Name) == 0)
					{
						warmaster = n;
						break;
					}
				}
			}
		}

		if (n < g_ConfigRead.server.GetObjectMax() - 1)
		{
			n++;
		}
		else
		{
			break;
		}
	}

	if (warmaster >= 1)
	{
		this->GCGuildWarRequestResult(GuildName2, *getGameObject(warmaster), type);
	}
}


struct PMSG_GUILDWARREQUEST_RESULT
{
	PBMSG_HEAD2 h;	// C1:60
	BYTE Result;	// 3
};

void GameProtocol::GCGuildWarRequestResult(LPSTR GuildName, CGameObject &Obj, int type)
{
	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		return;
	}

	if (Obj.m_IfState->use > 0 && Obj.m_IfState->type != 12)
	{
		return;
	}

	PMSG_GUILDWARREQUEST_RESULT pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x60, sizeof(pMsg));
	pMsg.Result = 3;

	if (Obj.m_PlayerData->GuildNumber < 1)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	GUILD_INFO_STRUCT * lpMyGuild = Obj.m_PlayerData->lpGuild;

	if (!lpMyGuild)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (lpMyGuild->WarState == 1 || lpMyGuild->WarDeclareState == 1)
	{
		pMsg.Result = 4;

		//Error on declaring war : %d %d %s
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

		return;
	}

	if (g_ConfigRead.pk.bPkPenaltyDisable == FALSE && Obj.m_PK_Level >= 6)
	{
		pMsg.Result = 4;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

		return;
	}

	if (strcmp(lpMyGuild->Users[0]->Name, Obj.Name))
	{
		pMsg.Result = 5;

		// Error on declaring war : 0x05 %s %s
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

		return;
	}

	char _guildname[MAX_GUILD_LEN + 1];
	std::memset(_guildname, 0, sizeof(_guildname));
	std::memcpy(_guildname, GuildName, MAX_GUILD_LEN);

	if (!strncmp(lpMyGuild->Name, GuildName, MAX_GUILD_LEN))
		return;

	GUILD_INFO_STRUCT * lpNode = Guild.SearchGuild(_guildname);

	if (lpNode)
	{
		if (lpNode->WarState == 1 || lpNode->WarDeclareState == 1)
		{
			pMsg.Result = 4;

			//Error on declaring war : %d %d %s
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

			return;
		}

		if (lpMyGuild->iGuildUnion != 0 && lpMyGuild->iGuildUnion == lpNode->iGuildUnion)
		{
			return;
		}

		int n = g_ConfigRead.server.GetObjectStartUserIndex();	// #warning Change this 0 to ObjectStartUserIndex
		int warmaster = -1;

		while (true)
		{
			CGameObject* lpObjTemp = getGameObject(n);
			if (lpObjTemp->Type == OBJ_USER)
			{
				if (lpObjTemp->Connected > PLAYER_LOGGED)
				{
					if (lpObjTemp->Name == lpNode->Users[0]->Name)
					{
						if (!strcmp(lpObjTemp->Name, lpNode->Users[0]->Name))
						{
							if (g_ConfigRead.pk.bPkPenaltyDisable == FALSE && lpObjTemp->m_PK_Level >= 6)
							{
								pMsg.Result = 4;

								IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

								return;
							}

							warmaster = n;
							break;
						}
					}
				}
			}

			if (n < g_ConfigRead.server.GetObjectMax() - 1)
				n++;
			else
				break;
		}

		if (warmaster >= 1)
		{
			if (CC_MAP_RANGE(Obj.MapNumber) || CC_MAP_RANGE(getGameObject(warmaster)->MapNumber))
			{
				GCServerMsgStringSend(Lang.GetText(0, 117), Obj, 1);
				return;
			}

			if (Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL || getGameObject(warmaster)->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
			{
				GCServerMsgStringSend(Lang.GetText(0, 575), Obj, 1);
				return;
			}

			if ((getGameObject(warmaster)->m_Option & 1) != 1)
			{
				pMsg.Result = 4;
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

				return;
			}

			pMsg.Result = 1;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
			GCGuildWarRequestSend(lpMyGuild->Name, getGameObject(warmaster), type);
			lpMyGuild->WarDeclareState = 1;
			lpNode->WarDeclareState = 1;
			lpMyGuild->WarType = type;
			lpNode->WarType = type;

			// Declared guild war [%s][%s] sGuild:(%s) TargetGuild:(%s)
			std::strcpy(lpMyGuild->TargetGuildName, lpNode->Name);
			std::strcpy(lpNode->TargetGuildName, lpMyGuild->Name);
			lpMyGuild->lpTargetGuildNode = lpNode;
			lpNode->lpTargetGuildNode = lpMyGuild;
		}
		else
		{
			pMsg.Result = 2;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

			//return;
		}
	}
	else
	{
		pMsg.Result = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

		return;
	}
}


struct PMSG_GUILDWARSEND
{
	PBMSG_HEAD2 h;	// C1:61
	char GuildName[8];	// 3
	BYTE Type;	// B
};

void GameProtocol::GCGuildWarRequestSend(LPSTR GuildName, CGameObject &Obj, int type) //GS-CS OK
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		PMSG_GUILDWARSEND pMsg;

		PHeadSetB((BYTE*)&pMsg, 0x61, sizeof(pMsg));
		pMsg.Type = type;
		std::memcpy(pMsg.GuildName, GuildName, MAX_GUILD_LEN);

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

	}
}


struct PMSG_GUILDWAR_DECLARE
{
	PBMSG_HEAD2 h;	// C1:62
	char GuildName[8];	// 3
	BYTE Type;	// B
	BYTE TeamCode;	// C
};


void GameProtocol::GCGuildWarRequestSendRecv(PMSG_GUILDWARSEND_RESULT * lpMsg, CGameObject &Obj)
{
	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		return;
	}

	if (Obj.Connected != 3)
		return;

	PMSG_GUILDWAR_DECLARE pMsg;
	int count = 0;
	int g_call = 0;

	PHeadSetB((BYTE*)&pMsg, 0x62, sizeof(pMsg));
	pMsg.Type = 0;

	GUILD_INFO_STRUCT * lpMyNode = Obj.m_PlayerData->lpGuild;

	if (!lpMyNode)
	{
		PMSG_GUILDWARREQUEST_RESULT pResult;

		pResult.h.c = 0xC1;
		pResult.h.headcode = 0x60;
		pResult.h.size = sizeof(pResult);	// #error Change fro pResult
		pResult.Result = 0;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (lpMyNode->lpTargetGuildNode && lpMsg->Result)
	{
		int iTarGetIndex = lpMyNode->lpTargetGuildNode->Index[0];

		if (!ObjectMaxRange(iTarGetIndex))
		{
			return;
		}

		if (BC_MAP_RANGE(getGameObject(iTarGetIndex)->MapNumber) ||
			CC_MAP_RANGE(getGameObject(iTarGetIndex)->MapNumber) ||
			DS_MAP_RANGE(getGameObject(iTarGetIndex)->MapNumber) ||
			DG_MAP_RANGE(getGameObject(iTarGetIndex)->MapNumber) ||
			IT_MAP_RANGE(getGameObject(iTarGetIndex)->MapNumber) ||
			IMPERIAL_MAP_RANGE(getGameObject(iTarGetIndex)->MapNumber))
		{
			PMSG_NOTICE pNotice;

			TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 117));

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pNotice, pNotice.h.size);

			lpMsg->Result = 0;
		}

		if (getGameObject(iTarGetIndex)->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
		{
			PMSG_NOTICE pNotice;

			TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 575));

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pNotice, pNotice.h.size);

			lpMsg->Result = 0;
		}
	}

	pMsg.Type = lpMyNode->WarType;

	if (lpMsg->Result == 0)
	{
		if (lpMyNode->lpTargetGuildNode)
		{
			if (lpMyNode->lpTargetGuildNode->WarDeclareState == 1)
			{
				lpMyNode->lpTargetGuildNode->WarDeclareState = 0;
				lpMyNode->WarDeclareState = 0;

				PMSG_GUILDWARREQUEST_RESULT pResult;

				PHeadSetB((BYTE*)&pResult, 0x60, sizeof(pResult));
				pResult.Result = 6;

				if (lpMyNode->lpTargetGuildNode->Index[0] >= 0)
				{
					IOCP.DataSend(lpMyNode->lpTargetGuildNode->Index[0], (BYTE*)&pResult, pResult.h.size);
				}
			}
		}
	}
	else //if ( lpMsg->Result == 1 )
	{
		if (lpMyNode->WarDeclareState == 1)
		{
			if (lpMyNode->lpTargetGuildNode)
			{
				if (lpMyNode->lpTargetGuildNode->WarDeclareState == 1)
				{
					PMSG_TELEPORT pTeleportMsg;

					if (lpMyNode->WarType == 1)
					{
						lpMyNode->BattleGroundIndex = gCheckBlankBattleGround();

						switch (lpMyNode->BattleGroundIndex)
						{
						case 0xFF:
							lpMyNode->WarDeclareState = 0;
							lpMyNode->WarState = 0;
							lpMyNode->lpTargetGuildNode->WarDeclareState = 0;
							lpMyNode->lpTargetGuildNode->WarState = 0;

							PMSG_GUILDWARREQUEST_RESULT pResult;

							PHeadSetB((BYTE*)&pResult, 0x60, sizeof(pResult));
							pResult.Result = 4;

							IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
							return;
						}

						pTeleportMsg.MoveNumber = 51;
						lpMyNode->lpTargetGuildNode->BattleGroundIndex = lpMyNode->BattleGroundIndex;

						if (gBSGround[0]->m_BallIndex >= 0)
							gObjMonsterRegen(*getGameObject(gBSGround[0]->m_BallIndex));

						BattleSoccerGoalEnd(0);
						lpMyNode->PlayScore = 0;
						lpMyNode->lpTargetGuildNode->PlayScore = 0;
						gBattleGroundEnable(lpMyNode->BattleGroundIndex, TRUE);
						gSetBattleTeamMaster(lpMyNode->BattleGroundIndex, 0, lpMyNode->Name, lpMyNode);
						gSetBattleTeamMaster(lpMyNode->BattleGroundIndex, 1, lpMyNode->lpTargetGuildNode->Name, lpMyNode->lpTargetGuildNode);
					}

					lpMyNode->WarDeclareState = 2;
					lpMyNode->WarState = 1;
					lpMyNode->lpTargetGuildNode->WarDeclareState = 2;
					lpMyNode->lpTargetGuildNode->WarState = 1;
					lpMyNode->PlayScore = 0;
					lpMyNode->lpTargetGuildNode->PlayScore = 0;
					lpMyNode->BattleTeamCode = 0;
					lpMyNode->lpTargetGuildNode->BattleTeamCode = 1;
					std::memset(pMsg.GuildName, 0, sizeof(pMsg.GuildName));
					std::memcpy(pMsg.GuildName, lpMyNode->lpTargetGuildNode->Name, sizeof(pMsg.GuildName));
					pMsg.TeamCode = lpMyNode->BattleTeamCode;
					count = 0;

					for (int n = 0; n < MAX_USER_GUILD; n++)
					{
						if (lpMyNode->Use[n])
						{
							if (lpMyNode->Index[n] >= 0)
							{
								g_call = 0;

								if (n > 0)
								{
									if (lpMyNode->WarType == 1)
									{
										if (getGameObject(lpMyNode->Index[0])->PartyNumber >= 0)
										{
											if (getGameObject(lpMyNode->Index[0])->PartyNumber == getGameObject(lpMyNode->Index[n])->PartyNumber)
											{
												g_call = 1;
											}
										}
									}
									else
									{
										g_call = 1;
									}
								}
								else if (n == 0)
								{
									g_call = 1;
								}

								if (g_call)
								{
									IOCP.DataSend(lpMyNode->Index[n], (BYTE*)&pMsg, pMsg.h.size);

									// Declared guild war against (%s)
									GCGuildWarScore(*getGameObject(lpMyNode->Index[n]));
									int x = 60;

									if (lpMyNode->WarType == 1)
									{
										lpMyNode->Users[n]->IsInBattleGround = true;
										pTeleportMsg.MoveNumber = 51;

										if (g_ConfigRead.pk.bPkPenaltyDisable || getGameObject(lpMyNode->Index[n])->m_PK_Level < 6)
										{
											gObjTeleport(*lpMyNode->Users[n], 6, x++, 153);
											count++;
										}
									}
								}
							}
						}
					}

					std::memset(pMsg.GuildName, 0, sizeof(pMsg.GuildName));
					std::memcpy(pMsg.GuildName, lpMyNode->Name, sizeof(pMsg.GuildName));

					pMsg.TeamCode = lpMyNode->lpTargetGuildNode->BattleTeamCode;
					count = 0;

					for (int n = 0; n < MAX_USER_GUILD; n++)
					{
						if (lpMyNode->lpTargetGuildNode->Use[n])
						{
							if (lpMyNode->lpTargetGuildNode->Index[n] >= 0)
							{
								g_call = 0;

								if (n > 0)
								{
									if (lpMyNode->WarType == 1)
									{
										if (getGameObject(lpMyNode->lpTargetGuildNode->Index[0])->PartyNumber >= 0)
										{
											if (getGameObject(lpMyNode->lpTargetGuildNode->Index[0])->PartyNumber == getGameObject(lpMyNode->lpTargetGuildNode->Index[n])->PartyNumber)
											{
												g_call = 1;
											}
										}
									}
									else
									{
										g_call = 1;
									}
								}
								else if (n == 0)
								{
									g_call = 1;
								}

								if (g_call)
								{
									IOCP.DataSend(lpMyNode->lpTargetGuildNode->Index[n], (BYTE*)&pMsg, pMsg.h.size);

									// Declared guild war against (%s)
									GCGuildWarScore(*getGameObject(lpMyNode->lpTargetGuildNode->Index[n]));
									int x = 59;

									if (lpMyNode->lpTargetGuildNode->WarType == 1)
									{

										if (g_ConfigRead.pk.bPkPenaltyDisable != 0 || getGameObject(lpMyNode->lpTargetGuildNode->Index[n])->m_PK_Level < 6)
										{
											lpMyNode->lpTargetGuildNode->Users[n]->IsInBattleGround = true;
											gObjTeleport(*lpMyNode->lpTargetGuildNode->Users[n], 6, x++, 164);
											count++;
										}
									}
								}
							}
						}
					}

					cManager.BattleInfoSend(GetBattleTeamName(0, 0),
						GetBattleTeamScore(0, 0),
						GetBattleTeamName(0, 1),
						GetBattleTeamScore(0, 1));

					if (lpMyNode->WarType == 1)
					{
						gObjAddMsgSendDelay(Obj, 5, Obj, 10000, 0);
						GCServerMsgStringSendGuild(*lpMyNode->lpTargetGuildNode, Lang.GetText(0, 62), 1);
						GCServerMsgStringSendGuild(*lpMyNode, Lang.GetText(0, 62), 1);
					}
				}
			}
		}
	}
}



void GameProtocol::GCGuildWarDeclare(CGameObject &Obj, LPSTR _guildname)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		PMSG_GUILDWAR_DECLARE pMsg;

		PHeadSetB((BYTE*)&pMsg, 0x62, sizeof(pMsg));
		std::memcpy(pMsg.GuildName, _guildname, sizeof(pMsg.GuildName));

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}
}


struct PMSG_GUILDWAR_END
{
	PBMSG_HEAD2 h;	// C1:63
	BYTE Result;	// 3
	char GuildName[8];	// 4
};

void GameProtocol::GCGuildWarEnd(CGameObject &Obj, BYTE result, char* _guildname)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		PMSG_GUILDWAR_END pMsg;

		PHeadSetB((BYTE*)&pMsg, 0x63, sizeof(pMsg));
		pMsg.Result = result;
		std::memcpy(pMsg.GuildName, _guildname, sizeof(pMsg.GuildName));

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}
}


struct PMSG_GUILDSCORE
{
	PBMSG_HEAD2 h;	// C1:64
	BYTE Score1;	// 3
	BYTE Score2;	// 4
	BYTE Type;	// 5
};


void GameProtocol::GCGuildWarScore(CGameObject &Obj)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		if (Obj.Type != OBJ_USER)
			return;

		if (Obj.m_PlayerData->GuildNumber < 1)
			return;

		PMSG_GUILDSCORE pMsg;

		PHeadSetB((BYTE*)&pMsg, 0x64, sizeof(pMsg));
		pMsg.Score1 = 0;
		pMsg.Score2 = 0;
		pMsg.Type = 0;

		if (Obj.m_PlayerData->lpGuild != NULL)
		{
			pMsg.Score1 = Obj.m_PlayerData->lpGuild->PlayScore;
		}

		if (Obj.m_PlayerData->lpGuild->lpTargetGuildNode != NULL)
		{
			pMsg.Score2 = Obj.m_PlayerData->lpGuild->lpTargetGuildNode->PlayScore;
		}

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}
}



void GameProtocol::CGWarehouseMoneyInOut(CGameObject &Obj, PMSG_WAREHOUSEMONEYINOUT* lpMsg)
{
	int money = 0;

	if (!gObjIsConnected(Obj))
	{
		GCWarehouseInventoryMoneySend(Obj, 0, 0, 0);
		return;
	}

	
	money = lpMsg->Money;

	if (Obj.m_IfState->state == 0)
	{
		GCWarehouseInventoryMoneySend(Obj, 0, 0, 0);
		return;
	}

	if (Obj.m_IfState->use < 1 || Obj.m_IfState->type != 6)
	{
		GCWarehouseInventoryMoneySend(Obj, 0, 0, 0);

		return;
	}

	switch (lpMsg->Type)
	{
	case 0x00:
		if (money <= 0 || money > MAX_WAREHOUSE_ZEN)
		{
			GCWarehouseInventoryMoneySend(Obj, 0, 0, 0);
			return;
		}

		if ((Obj.WarehouseMoney + money) > MAX_WAREHOUSE_ZEN)
		{
			GCWarehouseInventoryMoneySend(Obj, 0, 0, 0);
			return;
		}

		if (money > Obj.m_PlayerData->Money)
		{
			GCWarehouseInventoryMoneySend(Obj, 0, 0, 0);
			return;
		}

		Obj.m_PlayerData->Money -= money;
		Obj.WarehouseMoney += money;
		break;
	case 0x01:
	{
		if (bCanWarehouseLock == TRUE)
		{
			if (Obj.WarehouseLock == 1)
			{
				GCWarehouseInventoryMoneySend(Obj, 0, 0, 0);
				GCServerMsgStringSend(Lang.GetText(0, 155), Obj, 1);
				return;
			}
		}

		if (money <= 0 || money > MAX_WAREHOUSE_ZEN)
		{
			GCWarehouseInventoryMoneySend(Obj, 0, 0, 0);
			return;
		}

		if (money > Obj.WarehouseMoney)
		{
			GCWarehouseInventoryMoneySend(Obj, 0, 0, 0);
			return;
		}

		Obj.WarehouseMoney -= money;
		Obj.m_PlayerData->Money += money;
		int rZen = GetWarehouseUsedHowMuch(Obj.Level, Obj.WarehousePW);

		if ((Obj.m_PlayerData->Money - rZen) > 0)
		{
			int oldmoney = Obj.m_PlayerData->Money;
			Obj.m_PlayerData->Money -= rZen;
			GCMoneySend(Obj, Obj.m_PlayerData->Money);

		}
		else if ((Obj.WarehouseMoney - rZen) > 0)
		{
			int oldmoney = Obj.WarehouseMoney;
			Obj.WarehouseMoney -= rZen;

			GCWarehouseInventoryMoneySend(Obj, 1, Obj.m_PlayerData->Money, Obj.WarehouseMoney);
		}
		else
		{
			Obj.WarehouseMoney += money;
			Obj.m_PlayerData->Money -= money;
			GCWarehouseInventoryMoneySend(Obj, 0, 0, 0);
			char msg[255];

			wsprintf(msg, Lang.GetText(0, 154), rZen);
			GCServerMsgStringSend(msg, Obj, 1);
			return;
		}
	}
	break;

	default:
		GCWarehouseInventoryMoneySend(Obj, 0, 0, 0);
		return;
	}

	Obj.WarehouseCount++;
	GCWarehouseInventoryMoneySend(Obj, 1, Obj.m_PlayerData->Money, Obj.WarehouseMoney);
}

void GameProtocol::GCWarehouseInventoryMoneySend(CGameObject &Obj, BYTE result, int money, int wmoney)
{
	PMSG_MONEY pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x81, sizeof(pMsg));
	pMsg.Result = result;
	pMsg.iMoney = money;
	pMsg.wMoney = wmoney;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}



void GameProtocol::CGWarehouseUseEnd(CGameObject &Obj)
{
	if (!gObjIsConnectedGP(Obj))
	{
		return;
	}

	

	PMSG_DEFAULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x82, sizeof(pMsg));
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	bool bCheck = false;
	Check_SameSerialItem(Obj, 2, bCheck);

	if (Obj.m_btOpenWarehouse == FALSE)
	{
		AntiHackLog->Output("[%s][%s] Attempt Multi-Ware dupe", Obj.AccountID, Obj.Name);
		return;
	}

	gObjItemTextSave(lpObj);
	GDSetWarehouseList(Obj, TRUE);
	GDUserItemSave(lpObj);
	g_PentagramSystem.DBREQ_SetPentagramJewel(Obj, 0);
	g_PentagramSystem.DBREQ_SetPentagramJewel(Obj, 1);
}



struct PMSG_WAREHOUSESTATE
{
	PBMSG_HEAD2 h;	// C1:83
	BYTE State;	// 3
};


void GameProtocol::GCWarehouseStateSend(CGameObject &Obj, BYTE state)
{
	PMSG_WAREHOUSESTATE pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x83, sizeof(pMsg));
	pMsg.State = state;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}



void GameProtocol::GCWarehouseRecivePassword(CGameObject &Obj, PMSG_WAREHOUSEPASSSEND* lpMsg)
{
	if (bCanWarehouseLock == FALSE)
		return;

	int pw = lpMsg->Pass;

	switch (lpMsg->Type)
	{
	case 0x00:
		if (Obj.WarehouseLock)
		{
			if (Obj.WarehousePW == pw)
			{
				Obj.WarehouseLock = FALSE;
				GCWarehouseStateSend(Obj, 0x0C);
			}
			else
			{
				GCWarehouseStateSend(Obj, 0x0A);
			}
		}
		break;
	case 0x01:
		if (!Obj.WarehouseLock || Obj.WarehouseLock == 0xFF)
		{
			char szJoomin[21];

			std::memset(szJoomin, 0, sizeof(szJoomin));
			std::memcpy(szJoomin, lpMsg->LastJoominNumber, sizeof(lpMsg->LastJoominNumber));

			if (gObjPasswordCheck(Obj, szJoomin) == FALSE)
			{
				GCWarehouseStateSend(Obj, 0x0D);
				return;
			}

			Obj.WarehousePW = pw;
			Obj.WarehouseLock = FALSE;
			GCWarehouseStateSend(Obj, 0x0C);
		}
		else
		{
			GCWarehouseStateSend(Obj, 0x0B);
		}
		break;
	case 0x02:
	{
		char szJoomin[21];

		std::memset(szJoomin, 0, sizeof(szJoomin));
		std::memcpy(szJoomin, lpMsg->LastJoominNumber, sizeof(lpMsg->LastJoominNumber));

		pw = atoi(szJoomin);

		if (gObjPasswordCheck(Obj, szJoomin) == FALSE)
		{
			GCWarehouseStateSend(Obj, 0x0D);
			return;
		}

		Obj.WarehouseLock = FALSE;
		Obj.WarehousePW = 0;
		GCWarehouseStateSend(Obj, Obj.WarehouseLock);
	}
	break;
	}
}



void GameProtocol::GCUserChaosBoxSend(CGameObject &Obj, int iChaosBoxType)
{
	if (iChaosBoxType == 1)
	{
		if (Obj.m_IfState->type != 13)
		{
			return;
		}

		if (Obj.m_IfState->type == 13 && Obj.m_IfState->state == 1)
		{
			return;
		}
	}
	else
	{
		if (Obj.m_IfState->type != 7)
		{
			return;
		}

		if (Obj.m_IfState->type == 7 && Obj.m_IfState->state == 1)
		{
			return;
		}
	}

	PMSG_SHOPITEMCOUNT pMsg;
	BYTE SendByte[2048];
	int lOfs = sizeof(pMsg);
	PHeadSetW((BYTE*)&pMsg, 0x31, 0);
	pMsg.count = 0;

	if (iChaosBoxType == 1)
	{
		pMsg.Type = 5;
	}
	else
	{
		pMsg.Type = 3;
	}

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pntChaosBox[n]->IsItem() == TRUE)
		{
			SendByte[lOfs] = n;
			lOfs++;
			ItemByteConvert(&SendByte[lOfs], Obj.pntChaosBox[n]);
			lOfs += MAX_ITEM_INFO;
			pMsg.count++;
		}
	}

	pMsg.h.sizeH = SET_NUMBERH(lOfs);
	pMsg.h.sizeL = SET_NUMBERL(lOfs);
	std::memcpy(SendByte, &pMsg, sizeof(pMsg));

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, SendByte, lOfs);
}



void GameProtocol::CGChaosBoxItemMixButtonClick(PMSG_CHAOSMIX* aRecv, CGameObject &Obj)
{
	if (!gObjIsConnectedGP(Obj))
	{
		return;
	}

	

	if (Obj.ChaosLock == TRUE)
	{
		GCServerMsgStringSend(Lang.GetText(0, 11), Obj, 1);
		return;
	}

	if (Obj.m_PlayerData->m_bSecurityCheck == false)
	{
		return;
	}

	if (Obj.m_bPShopOpen == true)
	{
		GCServerMsgStringSend(Lang.GetText(0, 112), Obj, 1);
		return;
	}

	//season4 add-on start
	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;
	if (aRecv->Type == 3 ||
		aRecv->Type == 4 ||
		aRecv->Type == 22 ||
		aRecv->Type == 23)
	{
		int pos = 0;
		int validitemcount = 0;

		for (int n = 0; n < CHAOS_BOX_SIZE; n++) //loc8
		{
			if (Obj.pntChaosBox[n]->IsItem() == TRUE)
			{
				if (Obj.pntChaosBox[n]->m_Level == 9)
				{
					pos = n;
					validitemcount++;
				}
				else if (Obj.pntChaosBox[n]->m_Level == 10)
				{
					pos = n;
					validitemcount++;
				}
				else if (Obj.pntChaosBox[n]->m_Level == 11)
				{
					pos = n;
					validitemcount++;
				}
				else if (Obj.pntChaosBox[n]->m_Level == 12)
				{
					pos = n;
					validitemcount++;
				}
				else if (Obj.pntChaosBox[n]->m_Level == 13) //new
				{
					pos = n;
					validitemcount++;
				}
				else if (Obj.pntChaosBox[n]->m_Level == 14)
				{
					pos = n;
					validitemcount++;
				}
			}
		}

		if (validitemcount != 1)
		{
			pMsg.Result = 0;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
			return;
		}

		int iItemWidth = 0;
		int iItemHeight = 0;

		Obj.pntChaosBox[pos]->GetSize(iItemWidth, iItemHeight);

		if (!CheckInventoryEmptySpace(*lpObj, iItemHeight, iItemWidth))
		{
			pMsg.Result = 0;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
			return;
		}

		if (g_LuckyItemManager.IsLuckyItemEquipment(Obj.pntChaosBox[pos]->m_Type) == TRUE)
		{
			pMsg.Result = 7;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
			return;
		}
	}
	//season4 add-on end

	Obj.bIsChaosMixCompleted = true;
	int iMixType = aRecv->Type;

	//if (iMixType >= CHAOS_TYPE_BLOODANGEL_ITEM_MIX_START && iMixType <= CHAOS_TYPE_BLOODANGEL_ITEM_MIX_END)
	//{
	//	g_MixSystem.BloodAngelUnSealMix(lpObj, iMixType);
	//g_MixSystem.AncientSoulItemMix(lpObj);
	//}
	//else
	if (iMixType >= 0xC8 && iMixType <= 0xCF)
		g_MixSystem.DarkAngelWeaponMix(lpObj, iMixType);
	else
		if (iMixType >= 0x88 && iMixType <= 0xA7)
			g_MixSystem.DarkAngelMix(lpObj, iMixType);
		else
			if (iMixType >= 0xA8 && iMixType <= 0xC7)
				g_MixSystem.DarkAngelNextMix(lpObj, iMixType);
			else
			{

				switch (iMixType)	// Season X Main ENG
				{
				case CHAOS_TYPE_DEVILSQUARE:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.DevilSquareItemChaosMix(lpObj);
					}

					else
					{
						g_MixSystem.DevilSquareItemChaosMix_Multi(lpObj, aRecv->MixCount);
					}
					break;
				case 0x86:
					g_MixSystem.ArchangelHammerMix(lpObj);
					break;
				case 0x87:
					g_MixSystem.BlessedAAMix(lpObj);
					break;
				case 0x40:
					g_MixSystem.DarkAngelWeaponMix(lpObj, iMixType);
					break;
				case CHAOS_TYPE_UPGRADE_10:
					g_MixSystem.PlusItemLevelChaosMix(lpObj, 3);
					break;
				case CHAOS_TYPE_UPGRADE_11:
					g_MixSystem.PlusItemLevelChaosMix(lpObj, 4);
					break;
				case CHAOS_TYPE_UPGRADE_12:
					g_MixSystem.PlusItemLevelChaosMix(lpObj, 22);
					break;
				case CHAOS_TYPE_UPGRADE_13:
					g_MixSystem.PlusItemLevelChaosMix(lpObj, 23);
					break;
				case CHAOS_TYPE_UPGRADE_14:
					g_MixSystem.PlusItemLevelChaosMix(lpObj, 49);
					break;
				case CHAOS_TYPE_UPGRADE_15:
					g_MixSystem.PlusItemLevelChaosMix(lpObj, 50);
					break;
				case CHAOS_TYPE_DINORANT:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.PegasiaChaosMix(lpObj);
					}

					else
					{
						g_MixSystem.PegasiaChaosMix_Multi(lpObj, aRecv->MixCount);
					}
					break;
				case CHAOS_TYPE_FRUIT:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.CircleChaosMix(lpObj);
					}

					else
					{
						g_MixSystem.CircleChaosMix_Multi(lpObj, aRecv->MixCount);
					}
					break;
				case CHAOS_TYPE_SECOND_WING:
				case CHAOS_TYPE_CLOAK:
					g_MixSystem.WingChaosMix(lpObj);
					break;
				case CHAOS_TYPE_BLOODCATLE:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.BloodCastleMix(lpObj);
					}
					else
					{
						g_MixSystem.BloodCastleItemChaosMix_Multi(lpObj, aRecv->MixCount);
					}
					break;
				case CHAOS_TYPE_DEFAULT:
				case CHAOS_TYPE_FIRST_WING:
					g_MixSystem.DefaultChaosMix(lpObj);
					break;
				case CHAOS_TYPE_SETITEM:	// #warning mix domyslnie wylaczony w s2 i wyzej
					g_MixSystem.SetItemChaosMix(lpObj);
					break;
				case CHAOS_TYPE_DARKHORSE:
					g_MixSystem.DarkHorseChaosMix(lpObj);
					break;
				case CHAOS_TYPE_DARKSPIRIT:
					g_MixSystem.DarkSpiritChaosMix(lpObj);
					break;
				case CHAOS_TYPE_BLESS_POTION:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.BlessPotionChaosMix(lpObj);
					}

					else
					{
						g_MixSystem.BlessPotionChaosMix_Multi(lpObj, aRecv->MixCount);
					}
					break;
				case CHAOS_TYPE_SOUL_POTION:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.SoulPotionChaosMix(lpObj);
					}

					else
					{
						g_MixSystem.SoulPotionChaosMix_Multi(lpObj, aRecv->MixCount);
					}
					break;

				case CHAOS_TYPE_LIFE_STONE:
					g_MixSystem.LifeStoneChaosMix(lpObj);
					break;
				case CHAOS_TYPE_CASTLE_SPECIAL_ITEM_MIX:
					g_MixSystem.CastleSpecialItemMix(lpObj);
					break;
				case CHAOS_TYPE_HT_BOX:
					g_MixSystem.HiddenTreasureBoxItemMix(lpObj);
					break;
				case CHAOS_TYPE_FENRIR_01:
					g_MixSystem.Fenrir_01Level_Mix(lpObj);
					break;
				case CHAOS_TYPE_FENRIR_02:
					g_MixSystem.Fenrir_02Level_Mix(lpObj);
					break;
				case CHAOS_TYPE_FENRIR_03:
					g_MixSystem.Fenrir_03Level_Mix(lpObj);
					break;
				case CHAOS_TYPE_FENRIR_04:
					g_MixSystem.Fenrir_04Upgrade_Mix(lpObj);
					break;
				case CHAOS_TYPE_COMPOUNDPOTION_LV1:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.ShieldPotionLv1_Mix(lpObj);
					}

					else
					{
						g_MixSystem.ShieldPotionLv1_Mix_Multi(lpObj, aRecv->MixCount);
					}
					break;
				case CHAOS_TYPE_COMPOUNTPOTION_LV2:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.ShieldPotionLv2_Mix(lpObj);
					}

					else
					{
						g_MixSystem.ShieldPotionLv2_Mix_Multi(lpObj, aRecv->MixCount);
					}
					break;
				case CHAOS_TYPE_COMPOUNTPOTION_LV3:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.ShieldPotionLv3_Mix(lpObj);
					}

					else
					{
						g_MixSystem.ShieldPotionLv3_Mix_Multi(lpObj, aRecv->MixCount);
					}
					break;
				case CHAOS_TYPE_JEWELOFHARMONY_PURITY:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.JewelOfHarmonyItemPurityMix(lpObj);
					}
					else
					{
						g_MixSystem.PurityJewelOfHarmony_MultiMix(lpObj, aRecv->MixCount);
					}
					break;
				case CHAOS_TYPE_JEWELOFHARMONY_MIX_SMELTINGITEM:
					if (aRecv->MixCount == 1)
					{
						g_kJewelOfHarmonySystem.MakeSmeltingStoneItem(lpObj);
					}
					else
					{
						g_kJewelOfHarmonySystem.MakeSmeltingStoneItem_MultiMix(lpObj, aRecv->MixCount);
					}
					break;
				case CHAOS_TYPE_JEWELOFHARMONY_RESTORE_ITEM:
					g_kJewelOfHarmonySystem.RestoreStrengthenItem(lpObj);
					break;
				case CHAOS_TYPE_380_OPTIONITEM:
					g_kItemSystemFor380.ChaosMix380ItemOption(lpObj);
					break;
				case CHAOS_TYPE_LOTTERY_MIX:
					g_MixSystem.LotteryItemMix(lpObj, 0);
					break;
				case CHAOS_TYPE_OLD_PAPER:
					g_MixSystem.IllusionTempleItemChaosMix(lpObj);
					break;
				case CHAOS_TYPE_CONDOR_FEATHER:
					g_MixSystem.FeatherOfCondorMix(lpObj);
					break;
				case CHAOS_TYPE_THIRD_WING:
					g_MixSystem.ThirdWingLevel2ChaosMix(lpObj);
					break;
				case CHAOS_TYPE_BLOSSOM_MIX:
					g_MixSystem.CherryBlossomMix(lpObj);
					break;
				case CHAOS_TYPE_SEED_EXTRACT:
					g_MixSystem.SeedExtractMix(lpObj);
					break;
				case CHAOS_TYPE_SEED_SPHERE_COMPOSITE:
					g_MixSystem.SeedSphereCompositeMix(lpObj);
					break;
				case CHAOS_TYPE_SET_SEED_SPHERE:
					g_MixSystem.SetSeedSphereMix(lpObj, aRecv->Pos);
					break;
				case CHAOS_TYPE_SEED_SPHERE_REMOVE:
					g_MixSystem.SeedSphereRemoveMix(lpObj, aRecv->Pos);
					break;
				case CHAOS_TYPE_SECROMICON:
					g_MixSystem.SecromiconMix(lpObj);
					break;
				case CHAOS_TYPE_PREMIUM_BOX:
					if (aRecv->MixCount == 1)
					{
						g_MixSystem.PremiumBoxMix(lpObj);
					}

					else
					{
						g_MixSystem.PremiumBoxMix_Multi(lpObj, aRecv->MixCount);
					}
					break;
				case CHAOS_TYPE_SUMMONS_MIX:
					g_MixSystem.SummonsMix(lpObj, 0);
					break;
				case CHAOS_TYPE_LUCKYITEM_EXCHANGE:
					g_LuckyItemManager.LuckyItemTicketExchange(lpObj);
					break;
				case CHAOS_TYPE_LUCKYITEM_REFINE:
					g_LuckyItemManager.LuckyItemSmelting(lpObj);
					break;
				case CHAOS_TYPE_SOCKETWEAPON:
					g_MixSystem.ItemRefineMix(lpObj);
					break;
				case CHAOS_TYPE_ARCABATTLE_BOOTYMIX:
					g_ArcaBattle.BootyExchange(lpObj);
					break;
				case CHAOS_TYPE_ARCABATTLE_SIGNOFLORD:
					g_ArcaBattle.CGReqMarkRegButtonClick(lpObj);
					break;
				case CHAOS_TYPE_MONSTERWING:
					g_MixSystem.MonsterWingMix(lpObj);
					break;
				case CHAOS_TYPE_CCF_REWARD:
					g_MixSystem.CCFRewardMix(lpObj);
					break;
				case CHAOS_TYPE_DSF_REWARD:
					g_MixSystem.DSFRewardMix(lpObj);
					break;

				case CHAOS_TYPE_ANCIENT_SOUL_ITEM:
					g_MixSystem.AncientSoulItemMix(lpObj);
					break;
				case CHAOS_TYPE_ADVANCED_WING:
					g_MixSystem.AdvancedWingMix(lpObj);
					break;
				case 78:
					g_MixSystem.bloodangelmix78(lpObj);
					break;
				case 79:
					g_MixSystem.bloodangelmix79(lpObj);
					break;
				case 80:
					g_MixSystem.bloodangelmix80(lpObj);
					break;
				case 81:
					g_MixSystem.bloodangelmix81(lpObj);
					break;
				case 86:
					g_MixSystem.bloodangelmix86(lpObj);
					break;
				case 87:
					g_MixSystem.bloodangelmix87(lpObj);
					break;
				case 88:
					g_MixSystem.bloodangelmix88(lpObj);
					break;
				case 89:
					g_MixSystem.bloodangelmix89(lpObj);
					break;
				case 90:
					g_MixSystem.bloodangelmix90(lpObj);
					break;
				case 91:
					g_MixSystem.bloodangelmix91(lpObj);
					break;
				case 92:
					g_MixSystem.bloodangelmix92(lpObj);
					break;
				case 93:
					g_MixSystem.bloodangelmix93(lpObj);
					break;
				case 98:
					g_MixSystem.bloodangelmix98(lpObj);
					break;
				case 99:
					g_MixSystem.bloodangelmix99(lpObj);
					break;
				case 100:
					g_MixSystem.bloodangelmix100(lpObj);
					break;
				case 101:
					g_MixSystem.bloodangelmix101(lpObj);
					break;
				case 106:
					g_MixSystem.bloodangelmix106(lpObj);
					break;
				case 107:
					g_MixSystem.bloodangelmix107(lpObj);
					break;
				case 108:
					g_MixSystem.bloodangelmix108(lpObj);
					break;
				case 109:
					g_MixSystem.bloodangelmix109(lpObj);
					break;
				case 114:
					g_MixSystem.bloodangelmix114(lpObj);
					break;
				case 115:
					g_MixSystem.bloodangelmix115(lpObj);
					break;
				case 116:
					g_MixSystem.bloodangelmix116(lpObj);
					break;
				case 117:
					g_MixSystem.bloodangelmix117(lpObj);
					break;
				case 118:
					g_MixSystem.bloodangelmix118(lpObj);
					break;
				case 119:
					g_MixSystem.bloodangelmix119(lpObj);
					break;
				case 120:
					g_MixSystem.bloodangelmix120(lpObj);
					break;
				case 121:
					g_MixSystem.bloodangelmix121(lpObj);
					break;
				case 126:
					g_MixSystem.bloodangelmix126(lpObj);
					break;
				case 127:
					g_MixSystem.bloodangelmix127(lpObj);
					break;
				case 128:
					g_MixSystem.bloodangelmix128(lpObj);
					break;
				case 129:
					g_MixSystem.bloodangelmix129(lpObj);
					break;
					//se 0x8F..0xA9: //Armor DA 
					//	g_MixSystem.DarkAngelMix(lpObj, iMixType);
					//	break;
				default:
					sLog->outBasic("[%s][%s] Undefine chaosmix type detect %d", Obj.AccountID, Obj.Name, iMixType);
					break;
				}

			}
}



void GameProtocol::CGChaosBoxUseEnd(CGameObject &Obj)
{
	if (!gObjIsConnectedGP(Obj))
	{
		return;
	}

	

	PMSG_DEFAULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x87, sizeof(pMsg));
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

	if (Obj.m_IfState->use && Obj.m_IfState->type == 7)
	{
		Obj.m_IfState->use = 0;
		Obj.m_IfState->state = 0;
	}

	g_MixSystem.ChaosBoxInit(lpObj);
	g_PentagramMixSystem.PentagramMixBoxInit(lpObj);
	gObjInventoryCommit(Obj);
	Obj.m_bIsCastleNPCUpgradeCompleted = false;
}





void GameProtocol::PMoveProc(PMSG_MOVE* lpMove, CGameObject &Obj)
{
	if (Obj.m_bOffLevel == true)
	{
		return;
	}

	PMSG_RECVMOVE pMove;
	short n;
	short pathtable;
	short ax;
	short ay;
	int sx;
	int sy;
	int tick = GetTickCount();
	

	if (Obj.Connected != PLAYER_PLAYING)
	{
		return; // for reconnect incase player is using MuBot
	}
	if (MAX_MAP_RANGE(Obj.MapNumber) == FALSE)
	{
		return;
	}

	if (Obj.Type == OBJ_USER && (g_ConfigRead.MovingHackCheck == TRUE || g_ConfigRead.MovingSpeedCheck == TRUE))
	{
		short ix = abs(Obj.X - lpMove->X);
		short iy = abs(Obj.Y - lpMove->Y);

		if (g_ConfigRead.MovingSpeedCheck == TRUE)
		{
			Obj.m_PlayerData->MovingDistance += (ix + iy);
		}

		if (g_ConfigRead.MovingHackCheck == TRUE)
		{
			if (ix > g_ConfigRead.MovingHackDistance || iy > g_ConfigRead.MovingHackDistance)
			{
				gObjSetPosition(Obj, Obj.X, Obj.Y);
				return;
			}
		}
	}

	if (Obj.RegenOk > 0)
		return;

	if (Obj.m_bMapSvrMoveReq == true)
		return;

	if (Obj.m_bMapSvrMoveReq_2 == true)
		return;

	if (Obj.m_SkillInfo.LordSummonTime)
	{
		Obj.m_SkillInfo.LordSummonTime = 0;
		char msg[255];
		wsprintf(msg, Lang.GetText(0, 146));
		GCServerMsgStringSend(msg, Obj, 1);
	}

	if ((GetTickCount() - Obj.m_LastMoveTime) < 100)
		return;

	if (Obj.Teleport)
		return;

	if (gObjCheckUsedBuffEffect(&Obj, BUFFTYPE_STONE) || gObjCheckUsedBuffEffect(&Obj, BUFFTYPE_STUN) || gObjCheckUsedBuffEffect(&Obj, BUFFTYPE_SLEEP)
		|| gObjCheckUsedBuffEffect(&Obj, BUFFTYPE_FREEZE_2) || gObjCheckUsedBuffEffect(&Obj, BUFFTYPE_EARTH_BINDS))
	{
		gObjSetPosition(Obj, Obj.X, Obj.Y);
		return;
	}

	if (Obj.Type == OBJ_USER && gObjCheckMoveArea(Obj, lpMove->X, lpMove->Y) == false)
	{
		gObjSetPosition(Obj, Obj.X, Obj.Y);

		PMSG_TELEPORT_RESULT pTeleportResult;

		pTeleportResult.h.c = 0xC3;
		pTeleportResult.h.size = sizeof(pTeleportResult);
		pTeleportResult.h.headcode = 0x1C;
		pTeleportResult.Type = 0;
		pTeleportResult.Unk = 0;
		pTeleportResult.MapNumber = Obj.MapNumber;
		pTeleportResult.Unk1 = 0;
		pTeleportResult.MapX = Obj.X;
		pTeleportResult.MapY = Obj.Y;
		pTeleportResult.Dir = Obj.Dir;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pTeleportResult, pTeleportResult.h.size);
		return;
	}

	Obj.m_LastMoveTime = GetTickCount();
	Obj.m_Rest = 0;
	Obj.PathCur = 0;
	Obj.Dir = lpMove->Path[0] >> 4;
	Obj.PathCount = lpMove->Path[0] & 0x0F;

	if (Obj.PathCount > 15)
	{
		return;
	}

	for (n = 0; n < 15; n++)
	{
		Obj.PathX[n] = 0;
		Obj.PathY[n] = 0;
		Obj.PathOri[n] = 0;
	}

	sx = lpMove->X;
	sy = lpMove->Y;

	if (DG_MAP_RANGE(Obj.MapNumber) == TRUE)
	{
		if (g_DoppelGanger.GetDoppelgangerState() == 1 && g_DoppelGanger.CheckMapTile(Obj.MapNumber, Obj.X, Obj.Y) == FALSE)
		{
			gObjSetPosition(Obj, Obj.X, Obj.Y);
			return;
		}
	}

	/*
	if ( gObjCheckXYMapTile(lpObj, 1) == TRUE )
	{
		Obj.PathCount = 0;
		Obj.PathCur = 0;
		Obj.PathStartEnd = 0;
		gObjSetPosition(Obj, Obj.X, Obj.Y);
		return;
	}*/

	Obj.PathX[0] = lpMove->X;
	ax = Obj.PathX[0];
	Obj.PathY[0] = lpMove->Y;
	ay = Obj.PathY[0];
	Obj.PathDir[0] = Obj.Dir;
	Obj.PathStartEnd = 1;

	if (Obj.PathCount > 0)
	{
		Obj.PathCur = 1;
		Obj.PathCount++;
	}

	for (n = 1; n < Obj.PathCount; n++)
	{
		if ((n % 2) == 1)
		{
			pathtable = lpMove->Path[(n + 1) / 2] >> 4;
		}
		else
		{
			pathtable = lpMove->Path[(n + 1) / 2] & 0x0F;
		}

		ax += RoadPathTable[pathtable * 2];
		ay += RoadPathTable[pathtable * 2 + 1];

		Obj.PathOri[n - 1] = pathtable;
		Obj.PathDir[n] = pathtable;
		Obj.PathX[n] = ax;
		Obj.PathY[n] = ay;
	}

	if (Obj.PathCount > 0)
	{
		int nextX;
		int nextY;
		BYTE mapnumber;
		BYTE attr;
		nextX = Obj.PathX[1];
		nextY = Obj.PathY[1];
		mapnumber = Obj.MapNumber;
		attr = MapC[mapnumber].GetAttr(nextX, nextY);

		if ((((BYTE)attr & (BYTE)4) == (BYTE)4) || (((BYTE)attr & (BYTE)8) == (BYTE)8))
		{
			for (n = 0; n < 15; n++)
			{
				Obj.PathX[n] = 0;
				Obj.PathY[n] = 0;
				Obj.PathOri[n] = 0;
			}

			Obj.PathCount = 0;
			Obj.PathCur = 0;
			Obj.PathStartEnd = 0;
			gObjSetPosition(Obj, Obj.X, Obj.Y);
			return;
		}
	}

	Obj.TX = (BYTE)ax;
	Obj.TY = (BYTE)ay;

	if (Obj.Type == OBJ_USER && IMPERIAL_MAP_RANGE(Obj.MapNumber))
	{
		int iCX = Obj.X;
		int iCY = Obj.Y;
		BOOL bSetStand = FALSE;
		WORD wMapAttr = 0;

		for (int iCOUNT = 0; iCOUNT < 256; iCOUNT++)
		{
			if (iCX > Obj.TX)
				iCX--;
			if (iCX < Obj.TX)
				iCX++;
			if (iCY > Obj.TY)
				iCY--;
			if (iCY < Obj.TY)
				iCY++;

			wMapAttr = MapC[Obj.MapNumber].GetAttr(iCX, iCY);

			if (wMapAttr & 0x10)
			{
				bSetStand = TRUE;
				break;
			}

			if (iCX == Obj.TX && iCY == Obj.TY)
			{
				break;
			}

			if (bSetStand == 1)
			{
				Obj.m_Rest = 1;
				Obj.PathCur = 0;
				Obj.PathCount = 0;
				Obj.PathStartEnd = 0;
				gObjSetPosition(Obj, Obj.X, Obj.Y);

				PMSG_ACTIONRESULT pActionResult;

				PHeadSetB((BYTE*)&pActionResult, 0x18, sizeof(pActionResult));
				pActionResult.NumberH = SET_NUMBERH(Obj.m_Index);
				pActionResult.NumberL = SET_NUMBERL(Obj.m_Index);
				pActionResult.ActionNumber = 0x7A;
				pActionResult.Dir = Obj.Dir;
				pActionResult.TargetNumberH = 0;
				pActionResult.TargetNumberL = 0;

				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pActionResult, sizeof(pActionResult));
				return;
			}
		}
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (Obj.Type == OBJ_USER)
		{
			int nextX = Obj.X;
			int nextY = Obj.Y;
			int iResult = 0; //should be BOOL
			WORD attr = 0;

			for (int i = 0; i < 256; i++)
			{
				if (nextX > Obj.TX)
				{
					nextX -= 1;
				}

				if (nextX < Obj.TX)
				{
					nextX += 1;
				}

				if (nextY > Obj.TY)
				{
					nextY -= 1;
				}

				if (nextY < Obj.TY)
				{
					nextY += 1;
				}

				attr = MapC[Obj.MapNumber].GetAttr(nextX, nextY);

				if ((attr & 16) == 16)
				{
					iResult = 1;
					break;
				}

				if (nextX == Obj.TX)
				{
					if (nextY == Obj.TY)
					{
						break;
					}
				}
			}

			if (iResult == 1)
			{
				Obj.m_Rest = 1;
				Obj.PathCur = 0;
				Obj.PathCount = 0;
				Obj.PathStartEnd = 0;

				gObjSetPosition(Obj, Obj.X, Obj.Y);

				PMSG_ACTIONRESULT pActionResult;

				PHeadSetB((BYTE*)&pActionResult, 0x18, sizeof(pActionResult));
				pActionResult.NumberH = SET_NUMBERH(Obj.m_Index);
				pActionResult.NumberL = SET_NUMBERL(Obj.m_Index);
				pActionResult.ActionNumber = 0x7A;
				pActionResult.Dir = Obj.Dir;
				pActionResult.TargetNumberH = 0;
				pActionResult.TargetNumberL = 0;

				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pActionResult, sizeof(pActionResult));
				return;
			}
		}
	}

	if (BC_MAP_RANGE(Obj.MapNumber) && Obj.Type == OBJ_USER)
	{
		int iBridgeIndex = g_BloodCastle.GetBridgeIndex(Obj.MapNumber);
		switch (g_BloodCastle.GetCurrentState(iBridgeIndex))
		{
		case 0x01:
			if (Obj.TY > 15)
			{
				Obj.m_Rest = 1;
				Obj.PathCur = 0;
				Obj.PathCount = 0;
				Obj.PathStartEnd = 0;
				gObjSetPosition(Obj, Obj.X, Obj.Y);

				PMSG_ACTIONRESULT pActionResult;

				PHeadSetB((BYTE*)&pActionResult, 0x18, sizeof(pActionResult));
				pActionResult.NumberH = SET_NUMBERH(Obj.m_Index);
				pActionResult.NumberL = SET_NUMBERL(Obj.m_Index);
				pActionResult.ActionNumber = 0x7A;
				pActionResult.Dir = Obj.Dir;
				pActionResult.TargetNumberH = 0;
				pActionResult.TargetNumberL = 0;

				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pActionResult, sizeof(pActionResult));
				return;
			}
			break;
		case 0x02:
			if (Obj.Y > 17 && g_BloodCastle.CheckPlayStart(iBridgeIndex) == false)
			{
				gObjMoveGate(Obj, iBridgeIndex + 66);
				return;
			}

			if (Obj.TY > 15 && g_BloodCastle.CheckPlayStart(iBridgeIndex) == false)
			{
				Obj.m_Rest = 1;
				Obj.PathCur = 0;
				Obj.PathCount = 0;
				Obj.PathStartEnd = 0;
				gObjSetPosition(Obj, Obj.X, Obj.Y);

				PMSG_ACTIONRESULT pActionResult;

				PHeadSetB((BYTE*)&pActionResult, 0x18, sizeof(pActionResult));
				pActionResult.NumberH = SET_NUMBERH(Obj.m_Index);
				pActionResult.NumberL = SET_NUMBERL(Obj.m_Index);
				pActionResult.ActionNumber = 0x7A;
				pActionResult.Dir = Obj.Dir;
				pActionResult.TargetNumberH = 0;
				pActionResult.TargetNumberL = 0;

				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pActionResult, sizeof(pActionResult));
				return;
			}

			if (Obj.TY > 76 && g_BloodCastle.CheckCastleDoorLive(iBridgeIndex) == true)
			{
				Obj.m_Rest = 1;
				Obj.PathCur = 0;
				Obj.PathCount = 0;
				Obj.PathStartEnd = 0;
				gObjSetPosition(Obj, Obj.X, Obj.Y);

				PMSG_ACTIONRESULT pActionResult;

				PHeadSetB((BYTE*)&pActionResult, 0x18, sizeof(pActionResult));
				pActionResult.NumberH = SET_NUMBERH(Obj.m_Index);
				pActionResult.NumberL = SET_NUMBERL(Obj.m_Index);
				pActionResult.ActionNumber = 0x7A;
				pActionResult.Dir = Obj.Dir;
				pActionResult.TargetNumberH = 0;
				pActionResult.TargetNumberL = 0;

				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pActionResult, sizeof(pActionResult));
				return;
			}
			break;
		}
	}

	PHeadSetB((BYTE*)&pMove, 0xD7, sizeof(pMove));
	pMove.NumberH = SET_NUMBERH(Obj.m_Index);
	pMove.NumberL = SET_NUMBERL(Obj.m_Index);
	pMove.X = ax;
	pMove.Y = ay;
	pMove.Path = Obj.Dir << 4;

	if (Obj.Type == OBJ_USER)
	{
		if (Obj.m_byCCF_CanEnter == 0xFF)
		{
			Obj.m_byCCF_CanEnter = 0;

			if (Obj.m_bCCF_UI_Using == TRUE)
			{
				Obj.m_bCCF_UI_Using = FALSE;
			}
		}

		if (g_ConfigRead.server.GetServerType() == SERVER_NORMAL)
		{
			if (IT_MAP_RANGE(Obj.MapNumber))
			{
				if (Obj.m_nITR_RelicsTick > 0 || Obj.m_wITR_NpcType > 0 || Obj.m_byITR_StoneState != 99 || Obj.m_bITR_RegisteringRelics == true || Obj.m_bITR_GettingRelics == true)
				{
					g_IT_Event.CancleStoneState(Obj.m_wITR_NpcType, Obj.m_byITR_StoneState, Obj.MapNumber);
					Obj.m_nITR_RelicsTick = 0;
					Obj.m_wITR_NpcType = 0;
					Obj.m_byITR_StoneState = 99;
					Obj.m_bITR_RegisteringRelics = false;
					Obj.m_bITR_GettingRelics = false;
				}
			}
		}

		if (Obj.m_IfState->use)
		{
			if (Obj.m_IfState->type == 3)
			{
				Obj.TargetNpcNumber = -1;
				Obj.m_IfState->type = 0;
				Obj.m_IfState->use = 0;
			}
		}

		if (!gObjPositionCheck(lpObj))
		{
			Obj.PathCur = 0;
			Obj.PathCount = 0;
			ax = Obj.X;
			ay = Obj.Y;
			Obj.TX = (BYTE)ax;
			Obj.TY = (BYTE)ay;
			pMove.X = ax;
			pMove.Y = ay;
		}

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMove, pMove.h.size);
	}

	int MVL = MAX_VIEWPORT;

	if (Obj.Type == OBJ_MONSTER)
		MVL = MaxViewportMonster;

	for (n = 0; n < MVL; n++)
	{
		if (Obj.VpPlayer2[n].state == TRUE)
		{
			int number = Obj.VpPlayer2[n].number;

			if (number >= 0)
			{
				if (getGameObject(number)->Connected > PLAYER_CONNECTED)
				{
					if (getGameObject(number)->Live)
					{
						if (getGameObject(number)->Type == OBJ_USER)
						{
							IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index.VpPlayer2[n].number, (BYTE*)&pMove, pMove.h.size);
						}
					}
				}
				else
				{
					Obj.VpPlayer2[n].number = -1;
					Obj.VpPlayer2[n].state = 0;
					Obj.VPCount2--;
				}
			}
		}
	}

	MapC[Obj.MapNumber].ClearStandAttr(Obj.m_OldX, Obj.m_OldY);
	MapC[Obj.MapNumber].SetStandAttr(Obj.TX, Obj.TY);
	Obj.m_OldX = Obj.TX;
	Obj.m_OldY = Obj.TY;
	Obj.X = sx;
	Obj.Y = sy;
	Obj.m_ViewState = 0;

	if (Obj.Type == OBJ_USER) // Recv Check
	{
		if (rand() % 5 == 0)
		{
			this->GCSendRecvCheck(Obj);
		}
	}
}



void GameProtocol::RecvPositionSetProc(PMSG_POSISTION_SET * lpMove, CGameObject &Obj)
{
	short n;

	if (this->PacketCheckTime(Obj) == FALSE)
	{
		return;
	}

	if (Obj.Teleport != 0)
	{
		return;
	}

	if (MAX_MAP_RANGE(Obj.MapNumber) == FALSE)
	{
		return;
	}

	//K2 AntiHack Teleport
	int XDif = 0;
	int YDif = 0;

	if (Obj.X > lpMove->X)
		XDif = Obj.X - lpMove->X;
	else
		XDif = lpMove->X - Obj.X;

	if (Obj.Y > lpMove->Y)
		YDif = Obj.Y - lpMove->Y;
	else
		YDif = lpMove->Y - Obj.Y;

	//sLog->outBasic("[K2] XDif %d YDif %d", XDif, YDif);
	if ((XDif > 6) || (YDif > 6))
	{
		PMSG_TELEPORT_RESULT pTeleportResult;

		pTeleportResult.h.c = 0xC3;
		pTeleportResult.h.size = sizeof(pTeleportResult);
		pTeleportResult.h.headcode = 0x1C;
		pTeleportResult.Type = 0;
		pTeleportResult.Unk = 0;
		pTeleportResult.MapNumber = Obj.MapNumber;
		pTeleportResult.Unk1 = 0;
		pTeleportResult.MapX = Obj.X;
		pTeleportResult.MapY = Obj.Y;
		pTeleportResult.Dir = Obj.Dir;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pTeleportResult, pTeleportResult.h.size);

		PMSG_NOTICE pNotice;

		TNotice::MakeNoticeMsg((TNotice *)&pNotice, 1, "[K2][Anti-Cheat] Teleport Hack Blocked.");
		sLog->outBasic("[K2][Anti-Cheat] Teleport Hack Blocked. [AccID=%s | CharName=%s]", Obj.AccountID, Obj.Name);
		TNotice::SendNoticeToUser(Obj, (TNotice *)&pNotice);

		return;
	}
	Obj.X = lpMove->X;
	Obj.Y = lpMove->Y;

	if (CC_MAP_RANGE(Obj.MapNumber) != FALSE)
	{
		if ((GetTickCount() - Obj.m_iChaosCastleBlowTime) < 1000)
		{
			return;
		}
	}

	if (Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		if ((GetTickCount() - Obj.m_nCCF_BlowTime) < 1000)
		{
			return;
		}
	}

	if (gObjCheckXYMapTile(lpObj, 2) == TRUE)
	{
		return;
	}

	PMSG_RECV_POSISTION_SET pMove;

	PHeadSetB((BYTE*)&pMove, 0x10, sizeof(pMove));
	pMove.NumberH = SET_NUMBERH(Obj.m_Index);
	pMove.NumberL = SET_NUMBERL(Obj.m_Index);
	pMove.X = lpMove->X;
	pMove.Y = lpMove->Y;
	Obj.TX = lpMove->X;
	Obj.TY = lpMove->Y;

	if (::gObjPositionCheck(lpObj) == FALSE)
	{
		return;
	}

	CreateFrustrum(Obj.X, Obj.Y, Obj);

	if (Obj.Type == OBJ_USER)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR *)&pMove, pMove.h.size);
	}

	int MVL = MAX_VIEWPORT;

	if (Obj.Type == OBJ_MONSTER)
	{
		MVL = MaxViewportMonster;
	}

	for (n = 0; n < MVL; n++)
	{
		if (Obj.VpPlayer2[n].type == OBJ_USER)
		{
			if (Obj.VpPlayer2[n].state == 1)
			{
				int Index = Obj.VpPlayer2[n].number;

				if (ObjectMaxRange(Index) == false)
				{
					continue;
				}

				if (getGameObject(Index)->Connected > PLAYER_CONNECTED && getGameObject(Index)->Live != FALSE)
				{
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index.VpPlayer2[n].number, (UCHAR *)&pMove, pMove.h.size);
				}
				else
				{
					Obj.VpPlayer2[n].number = -1;
					Obj.VpPlayer2[n].state = FALSE;
					Obj.VPCount2--;
				}
			}
		}
	}

	MapC[Obj.MapNumber].ClearStandAttr(Obj.m_OldX, Obj.m_OldY);
	MapC[Obj.MapNumber].SetStandAttr(Obj.TX, Obj.TY);

	Obj.m_OldX = Obj.TX;
	Obj.m_OldY = Obj.TY;

}



void GameProtocol::CGAttack(PMSG_ATTACK* lpMsg, CGameObject &Obj)
{
	CGameObject* lpTargetObj;
	int usernumber = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	if (!getGameObject(usernumber)->Live)
		return;

	if (Obj.Type == OBJ_USER  && Obj.m_PlayerData->m_bSecurityCheck == false)
		return;
	int Allowed = 120;

	if (Obj.m_AttackSpeed >= 300)
	{
		Allowed = 170; //6.26
	}
	else if ((Obj.m_AttackSpeed > 150) && (Obj.m_AttackSpeed < 300))
		Allowed = 190;
	else if ((Obj.m_AttackSpeed > 0) && (Obj.m_AttackSpeed < 150))
		Allowed = 190;
	if (Obj.Type == OBJ_USER)
	{
		//10:50:00
		//10:51:00
		//1000 iTimeCalc < Allowed 300//
		int iTimeCalc = GetTickCount() - Obj.m_LastAttackTime;
		Obj.m_LastAttackTime = GetTickCount();
		if (iTimeCalc < Allowed) //1 1200
		{
			//sLog->outBasic("[K2 ANTIHACK] Speed Hack Detect %s %s %d %d", Obj.AccountID, Obj.Name, Allowed, Obj.m_AttackSpeed);
			return;
		}
	}
	/*if (iTimeCalc < Obj.m_DetectSpeedHackTime) //season4 changed (fix)
	{
		Obj.m_DetectCount++;
		Obj.m_SumLastAttackTime += iTimeCalc;

		if (Obj.m_DetectCount > gHackCheckCount)
		{
			Obj.m_DetectedHackKickCount++;
			Obj.m_SpeedHackPenalty = gSpeedHackPenalty;

			if (gIsKickDetecHackCountLimit)
			{
				if (Obj.m_DetectedHackKickCount > gDetectedHackKickCount)
				{
					sLog->outBasic("[%s][%s] Kick DetecHackCountLimit Over User (%d)", Obj.AccountID, Obj.Name, Obj.m_DetectedHackKickCount);
					this->GCSendDisableReconnect(Obj);
					//IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
					return;
				}

				sLog->outBasic("[%s][%s] Attack Speed Is Wrong Normal (%d)(%d) Penalty %d", Obj.AccountID, Obj.Name, Obj.m_DetectSpeedHackTime, Obj.m_SumLastAttackTime / Obj.m_DetectCount, Obj.m_SpeedHackPenalty);
			}

		}

		Obj.m_LastAttackTime = GetTickCount();
	}
	else
	{
		Obj.m_SumLastAttackTime = 0;
		Obj.m_DetectCount = 0;
	}
	*/
	Obj.m_LastAttackTime = GetTickCount();

	if (bIsIgnorePacketSpeedHackDetect)
	{
		if (Obj.m_SpeedHackPenalty > 0)
		{
			Obj.m_SpeedHackPenalty--;

			return;
		}
	}

	lpTargetObj = getGameObject(usernumber);
	Obj.Dir = lpMsg->DirDis;
	// (lpMsg->AttackAction == 6)
	GCActionSend(Obj, lpMsg->AttackAction, *lpTargetObj);
	gComboAttack.CheckCombo(Obj, 0);
	gObjAttack(Obj, *lpTargetObj, NULL, FALSE, FALSE, 0, FALSE, 0, 0);

	Obj.UseMagicNumber = 0;
}

void GameProtocol::GCDamageSend(CGameObject &Obj, CGameObject &ObjTarget, int AttackDamage, int MSBFlag, int MSBDamage, int iShieldDamage)
{
	PMSG_ATTACKRESULT pResult;

	PHeadSetB((BYTE*)&pResult, 0xDF, sizeof(pResult));
	pResult.NumberH = SET_NUMBERH(TargetIndex);
	pResult.NumberL = SET_NUMBERL(TargetIndex);
	pResult.DamageH = SET_NUMBERH(AttackDamage);
	pResult.DamageL = SET_NUMBERL(AttackDamage);
	pResult.btShieldDamageH = SET_NUMBERH(iShieldDamage);
	pResult.btShieldDamageL = SET_NUMBERL(iShieldDamage);
	pResult.IGCDamage = AttackDamage;
	pResult.ElementalDmg = iShieldDamage;

	if (MSBFlag != FALSE)
	{
		pResult.NumberH &= 0x7F;
		pResult.NumberH |= 0x80;
	}

	pResult.DamageTypeH = HIBYTE(MSBDamage);
	pResult.DamageTypeL = LOBYTE(MSBDamage);

	if (getGameObject(TargetIndex)->Type == OBJ_USER)
	{
		IOCP.DataSend(TargetIndex, (BYTE*)&pResult, pResult.h.size);
	}

	if (cManager.WatchTargetIndex == TargetIndex || cManager.WatchTargetIndex == Obj)
	{
		cManager.DataSend((BYTE*)&pResult, pResult.h.size);
	}

	if (Obj.Type == OBJ_USER)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
	}
}


struct PMSG_DAMAGE
{
	PBMSG_HEAD2 h;	// C1:F3:07
	BYTE subcode;	// 3
	BYTE DamageH;	// 4
	BYTE DamageL;	// 5
	BYTE btShieldDamageH;	// 6
	BYTE btShieldDamageL;	// 7
};

void GameProtocol::GCDamageSendPoison(CGameObject &Obj, int damage, int iShieldDamage)
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	PMSG_DAMAGE pDamage;

	PHeadSubSetB((BYTE*)&pDamage, 0xF3, 0x07, sizeof(pDamage));
	pDamage.DamageH = SET_NUMBERH(damage);
	pDamage.DamageL = SET_NUMBERL(damage);
	pDamage.btShieldDamageH = SET_NUMBERH(iShieldDamage);
	pDamage.btShieldDamageL = SET_NUMBERL(iShieldDamage);

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pDamage, pDamage.h.size);
}


struct PMSG_KILLPLAYER
{
	PBMSG_HEAD2 h;	// C1:16
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	BYTE ExpH;	// 5
	BYTE ExpL;	// 6
	BYTE DamageH;	// 7
	BYTE DamageL;	// 8
};


void GameProtocol::GCKillPlayerExpSend(CGameObject &Obj, CGameObject &ObjTarget, int exp, int AttackDamage, BOOL MSBFlag)
{
	PMSG_KILLPLAYER_EXT pkillMsg;

	PHeadSetBE((BYTE*)&pkillMsg, 0x9c, sizeof(pkillMsg));
	pkillMsg.NumberH = SET_NUMBERH(TargetIndex);
	pkillMsg.NumberL = SET_NUMBERL(TargetIndex);
	pkillMsg.ExpH = SET_NUMBERHW(exp);
	pkillMsg.ExpL = SET_NUMBERLW(exp);
	pkillMsg.DamageH = SET_NUMBERH(AttackDamage);
	pkillMsg.DamageL = SET_NUMBERL(AttackDamage);

	if (MSBFlag != FALSE)
	{
		pkillMsg.NumberH &= 0x7F;
		pkillMsg.NumberH |= 0x80;
	}

	if (BC_MAP_RANGE(Obj.MapNumber) != FALSE)
	{
		g_BloodCastle.AddExperience(Obj, exp);
	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pkillMsg, pkillMsg.h.size);
}

void GameProtocol::GCKillPlayerMasterExpSend(CGameObject &Obj, CGameObject &ObjTarget, int exp, int AttackDamage, BOOL MSBFlag)
{
	PMSG_KILLPLAYER_EXT pkillMsg;

	PHeadSetBE((BYTE*)&pkillMsg, 0x9C, sizeof(pkillMsg));
	pkillMsg.NumberH = SET_NUMBERH(TargetIndex);
	pkillMsg.NumberL = SET_NUMBERL(TargetIndex);
	pkillMsg.ExpH = SET_NUMBERHW(exp);
	pkillMsg.ExpL = SET_NUMBERLW(exp);
	pkillMsg.DamageH = SET_NUMBERH(AttackDamage);
	pkillMsg.DamageL = SET_NUMBERL(AttackDamage);

	if (MSBFlag != FALSE)
	{
		pkillMsg.NumberH &= 0x7F;
		pkillMsg.NumberH |= 0x80;
	}

	if (BC_MAP_RANGE(Obj.MapNumber) != FALSE)
	{
		g_BloodCastle.AddExperience(Obj, exp);
	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pkillMsg, pkillMsg.h.size);
}



struct PMSG_DIEPLAYER
{
	PBMSG_HEAD2 h;	// C1:17
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	BYTE Skill;	// 5
	BYTE KillerNumberH;	// 6
	BYTE KillerNumberL;	// 7
};


void GameProtocol::GCDiePlayerSend(CGameObject &Obj, int TargetIndex, BYTE skill, int KillerIndex)
{
	PMSG_DIEPLAYER pDieMsg;

	PHeadSetB((BYTE*)&pDieMsg, 0x17, sizeof(pDieMsg));
	pDieMsg.NumberH = SET_NUMBERH(TargetIndex);
	pDieMsg.NumberL = SET_NUMBERL(TargetIndex);
	pDieMsg.Skill = skill;
	pDieMsg.KillerNumberH = SET_NUMBERH(KillerIndex);
	pDieMsg.KillerNumberL = SET_NUMBERL(KillerIndex);
	int iSize = pDieMsg.h.size;

	MsgSendV2(lpObj, (BYTE*)&pDieMsg, iSize);

	if (Obj.Type == OBJ_USER)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pDieMsg, iSize);
	}
}



void GameProtocol::GCActionSend(CGameObject &Obj, BYTE ActionNumber, CGameObject &ObjTarget)
{
	PMSG_ACTIONRESULT pActionResult;

	PHeadSetB((BYTE*)&pActionResult, 0x18, sizeof(pActionResult));
	pActionResult.NumberH = SET_NUMBERH(Obj.m_Index);
	pActionResult.NumberL = SET_NUMBERL(Obj.m_Index);
	pActionResult.ActionNumber = ActionNumber;
	pActionResult.Dir = Obj.Dir;
	pActionResult.TargetNumberH = SET_NUMBERH(aTargetIndex);
	pActionResult.TargetNumberL = SET_NUMBERL(aTargetIndex);

	MsgSendV2(lpObj, (BYTE*)&pActionResult, pActionResult.h.size);
}



void GameProtocol::CGActionRecv(PMSG_ACTION * lpMsg, CGameObject &Obj)
{
	if (lpMsg->Dir > 7 || lpMsg->h.size > 5)
	{
		//this->GCSendDisableReconnect(Obj);
		IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
	}

	PMSG_ACTIONRESULT pResult;
	

	PHeadSetB((BYTE*)&pResult, 0x18, sizeof(pResult));
	pResult.ActionNumber = lpMsg->ActionNumber;
	pResult.NumberH = SET_NUMBERH(Obj.m_Index);
	pResult.NumberL = SET_NUMBERL(Obj.m_Index);
	Obj.m_ActionNumber = lpMsg->ActionNumber;
	Obj.Dir = lpMsg->Dir;
	pResult.Dir = Obj.Dir;
	pResult.TargetNumberH = lpMsg->iTargetIndexH;
	pResult.TargetNumberL = lpMsg->iTargetIndexL;

	if (Obj.m_ActionNumber == 0x80)
	{
		Obj.m_ViewState = 1;
		Obj.m_Rest = Obj.m_ActionNumber;
	}

	else if (Obj.m_ActionNumber == 0x81)
	{
		Obj.m_ViewState = 2;
		Obj.m_Rest = Obj.m_ActionNumber;
	}

	else if (Obj.m_ActionNumber == 0x82)
	{
		Obj.m_ViewState = 3;
		Obj.m_Rest = Obj.m_ActionNumber;
	}

	if (GetTickCount() - Obj.m_ActionTime < 500)
	{
		Obj.m_ActionCount++;

		if (Obj.m_ActionCount > 9)
		{
			//this->GCSendDisableReconnect(Obj);
			IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
			return;
		}
	}

	else
	{
		Obj.m_ActionCount = 0;
	}

	Obj.m_ActionTime = GetTickCount();

	int MVL = MAX_VIEWPORT;

	if (Obj.Type == OBJ_MONSTER)
		MVL = MaxViewportMonster;

	for (int n = 0; n < MVL; n++)
	{
		if (Obj.VpPlayer2[n].type == OBJ_USER)
		{
			if (Obj.VpPlayer2[n].state != 0 && Obj.VpPlayer2[n].state != 0x10 && Obj.VpPlayer2[n].state != 0x08)
			{
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index.VpPlayer2[n].number, (BYTE*)&pResult, pResult.h.size);
			}
		}
	}
}



void GameProtocol::CGMagicAttack(PMSG_MAGICATTACK* lpMsg, CGameObject &Obj)
{
	//PMSG_MAGICATTACK * lpMsg = (PMSG_MAGICATTACK *)(lpRecv);

	CGameObject lpObj;
	CGameObject lpTargetObj;
	WORD MagicNumber = MAKE_NUMBERW(lpMsg->MagicNumberH, lpMsg->MagicNumberL);
	int usernumber = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	if (getGameObject(usernumber)->Live == 0 || getGameObject(usernumber)->Connected < PLAYER_LOGGED)
		return;

	if (Obj.CloseCount >= 0)
		return;

	if (Obj.Type == OBJ_USER && Obj.m_PlayerData->m_bSecurityCheck == false)
		return;

	lpObj = &Obj;
	lpTargetObj = *getGameObject(usernumber);

	CMagicInf * lpMagic;
	int attackret = gObjCheckAttackArea(Obj, usernumber);

	if (attackret)
	{

		if (Obj.Class == 275)	// Kundun
			Obj.TargetNumber = -1;

		if (bIsIgnorePacketSpeedHackDetect)
			return;
	}

	if (Obj.Type == OBJ_USER)
	{

		if (MagicNumber == 58) // Nova cast
			usernumber = 58;

		if (MagicNumber == 395)
			usernumber = 395;

		lpMagic = gObjGetMagicSearch(lpObj, MagicNumber);
		if (!lpMagic)
			return;

		if (lpMagic->m_Skill == AT_SKILL_RECALL_PARTY)
		{
			if (g_NewPVP.IsDuel(*lpObj) || g_NewPVP.IsDuel(*lpTargetObj))
			{
				return;
			}
		}
		if (MagicDamageC.SkillGetRequireClass(Obj.Class, Obj.m_PlayerData->ChangeUP, lpMagic->m_Skill) < 1)
			return;
		if (MagicDamageC.CheckStatus(lpMagic->m_Skill, Obj.m_PlayerData->GuildStatus) == 0)
		{
			return;
		}
		if (MagicDamageC.CheckKillCount(lpMagic->m_Skill, Obj.m_btKillCount) < 0)
		{
			return;
		}

		if (Obj.Teleport && gObjUseSkill.m_SkillData.EnableWizardSkillWhileTeleport == false)
		{
			return;
		}
	}
	else
	{
		lpMagic = gObjGetMagic(lpObj, MagicNumber);
		if (!lpMagic)
			return;
	}

	if (lpMagic->m_Skill != 24 && gObjUseSkill.SpeedHackCheck(Obj) == FALSE)
		return;

	if (!gCheckSkillDistance(Obj, usernumber, lpMagic->m_Skill))
	{
		DWORD dwTick = GetTickCount() - Obj.m_dwSkillDistanceErrorTick;

		if (dwTick > (g_ConfigRead.SkillDistanceTimeDC * 1000))
		{
			Obj.m_iSkillDistanceErrorCount = 0;
			Obj.m_dwSkillDistanceErrorTick = GetTickCount();
		}

		Obj.m_iSkillDistanceErrorCount++;

		if (Obj.m_iSkillDistanceErrorCount > g_ConfigRead.SkillDistanceCountDC)
		{
			if (g_ConfigRead.SkillDistanceDC == 1)
			{
				//this->GCSendDisableReconnect(Obj);
				IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
				return;
			}

			Obj.m_iSkillDistanceErrorCount = 0;
		}

		return;
	}

	gObjUseSkill.UseSkill(Obj, usernumber, lpMagic);
}

void GameProtocol::CGRageAttack(PMSG_RAGEATTACK_REQ* lpRecv, CGameObject &Obj)
{
	CGameObject* lpTargetObj;

	PMSG_RAGEATTACK_REQ * lpMsg = (PMSG_RAGEATTACK_REQ *)(lpRecv);

	WORD MagicNumber = MAKE_NUMBERW(lpMsg->MagicNumberH, lpMsg->MagicNumberL);
	int usernumber = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
	CMagicInf * lpMagic;

	lpTargetObj = getGameObject(usernumber);

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.Teleport && gObjUseSkill.m_SkillData.EnableWizardSkillWhileTeleport == false)
	{
		return;
	}

	if (Obj.CloseCount >= 0)
	{
		return;
	}

	if (MagicNumber == 263 || MagicNumber == 559 || MagicNumber == 563)
	{
		this->GCMonkMagicAttack(Obj, MagicNumber, *getGameObject(Obj.m_PlayerData->DarkSideTarget[0]), TRUE);
		return;
	}

	if (usernumber < 0 || usernumber > g_ConfigRead.server.GetObjectMax() - 1)
	{
		return;
	}

	if (lpTargetObj->Live == 0 || lpTargetObj->Connected < PLAYER_LOGGED)
		return;

	if (Obj.Type == OBJ_USER)
	{
		lpMagic = gObjGetMagicSearch(lpObj, MagicNumber);

		if (!lpMagic)
			return;

		if (lpMagic->m_Skill == AT_SKILL_RECALL_PARTY)
		{
			if (g_NewPVP.IsDuel(*lpObj) || g_NewPVP.IsDuel(*lpTargetObj))
			{
				return;
			}
		}

		if (MagicDamageC.SkillGetRequireClass(Obj.Class, Obj.m_PlayerData->ChangeUP, lpMagic->m_Skill) < 1)
			return;

		if (MagicDamageC.CheckStatus(lpMagic->m_Skill, Obj.m_PlayerData->GuildStatus) == 0)
		{
			return;
		}

		if (MagicDamageC.CheckKillCount(lpMagic->m_Skill, Obj.m_btKillCount) < 0)
		{
			return;
		}

		if (Obj.m_PlayerData->m_bSecurityCheck == false)
		{
			return;
		}
	}

	if (!gCheckSkillDistance(Obj, usernumber, lpMagic->m_Skill))
	{
		DWORD dwTick = GetTickCount() - Obj.m_dwSkillDistanceErrorTick;

		if (dwTick > (g_ConfigRead.SkillDistanceTimeDC * 1000))
		{
			Obj.m_iSkillDistanceErrorCount = 0;
			Obj.m_dwSkillDistanceErrorTick = GetTickCount();
		}

		Obj.m_iSkillDistanceErrorCount++;

		if (Obj.m_iSkillDistanceErrorCount > g_ConfigRead.SkillDistanceCountDC)
		{
			if (g_ConfigRead.SkillDistanceDC == 1)
			{
				//this->GCSendDisableReconnect(Obj);
				IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
				return;
			}

			Obj.m_iSkillDistanceErrorCount = 0;
		}

		return;
	}

	this->GCMonkMagicAttack(Obj, MagicNumber, *lpTargetObj, TRUE);
}

void GameProtocol::CGRageAttackRange(PMSG_RAGE_ATTACK_RANGE *lpMsg, CGameObject &Obj)
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.Teleport && gObjUseSkill.m_SkillData.EnableWizardSkillWhileTeleport == false)
	{
		return;
	}

	CMagicInf * lpMagic;
	

	WORD MagicNumber = MAKE_NUMBERW(lpMsg->MagicNumberH, lpMsg->MagicNumberL);
	WORD TargetIndex = MAKE_NUMBERW(lpMsg->TargetIndexH, lpMsg->TargetIndexL);
	WORD Target[5];

	for (int i = 0; i < 5; i++)
	{
		Target[i] = 10000;
	}

	lpMagic = gObjGetMagicSearch(lpObj, MagicNumber);

	if (lpMagic)
	{
		if (gObjUseSkill.SkillMonkDarkSideGetTargetIndex(Obj, TargetIndex, lpMagic, Target) == TRUE)
		{
			PMSG_RAGE_ATTACK_RANGE_ANS pMsg;
			PHeadSetBE((BYTE*)&pMsg, 0x4B, sizeof(pMsg));

			std::memcpy(&pMsg.Target, Target, sizeof(Target));
			pMsg.MagicNumber = lpMagic->m_Skill;;

			Obj.m_PlayerData->DarkSideCount = 0;

			for (int i = 0; i < 5; i++)
			{
				if (Target[i] != 10000)
				{
					Obj.m_PlayerData->DarkSideCount++;
				}
			}

			std::memcpy(Obj.m_PlayerData->DarkSideTarget, Target, sizeof(Target));
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		}
	}
}

void GameProtocol::GCMagicAttackNumberSend(CGameObject &Obj, WORD MagicNumber, CGameObject &ObjTarget, BYTE skillsuccess)
{
	PMSG_MAGICATTACK_RESULT pAttack;

	PHeadSetBE((BYTE*)&pAttack, 0x19, sizeof(pAttack));
	pAttack.MagicNumberH = SET_NUMBERH(MagicNumber);
	pAttack.MagicNumberL = SET_NUMBERL(MagicNumber);
	pAttack.SourceNumberH = SET_NUMBERH(Obj.m_Index);
	pAttack.SourceNumberL = SET_NUMBERL(Obj.m_Index);
	pAttack.TargetNumberH = SET_NUMBERH(usernumber);
	pAttack.TargetNumberL = SET_NUMBERL(usernumber);
	pAttack.TargetNumberH &= 0x7F;

	if (skillsuccess == 1)
		pAttack.TargetNumberH |= 0x80;

	if (Obj.Type == OBJ_USER)
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pAttack, pAttack.h.size);

	if (CC_MAP_RANGE(Obj.MapNumber) || Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		if (g_MasterLevelSkillTreeSystem.CheckMasterLevelSkill(MagicNumber) == true)
		{
			switch (MagicNumber)
			{
			case 356:
			case 403:
			case 413:
			case 417:
			case 420:
				return;
			default:
				break;
			}
		}

		else
		{
			switch (MagicNumber)
			{
			case 16:
			case 26:
			case 27:
			case 28:
			case 48:
				return;
			default:
				break;
			}
		}
	}

	MsgSendV2(ObjTarget, (BYTE*)&pAttack, pAttack.h.size);
}

void GameProtocol::GCMonkMagicAttack(CGameObject &Obj, WORD MagicNumber, CGameObject &ObjTarget, BYTE skillsuccess)
{
	PMSG_RAGEATTACK pAttack;

	PHeadSetBE((BYTE*)&pAttack, 0x4A, sizeof(pAttack));
	pAttack.MagicNumberH = SET_NUMBERH(MagicNumber);
	pAttack.MagicNumberL = SET_NUMBERL(MagicNumber);
	pAttack.SourceNumberH = SET_NUMBERH(Obj.m_Index);
	pAttack.SourceNumberL = SET_NUMBERL(Obj.m_Index);
	pAttack.TargetNumberH = SET_NUMBERH(usernumber);
	pAttack.TargetNumberL = SET_NUMBERL(usernumber);
	pAttack.TargetNumberH &= 0x7F;

	if (skillsuccess == 1)
		pAttack.TargetNumberH |= 0x80;

	if (Obj.Type == OBJ_USER)
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pAttack, pAttack.h.size);

	MsgSendV2(lpObj, (BYTE*)&pAttack, pAttack.h.size);
}

void GameProtocol::GCMagicCancelSend(CGameObject &Obj, WORD MagicNumber)
{
	PMSG_MAGICCANCEL pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x1B, sizeof(pMsg));
	pMsg.MagicNumberL = SET_NUMBERL(MagicNumber);
	pMsg.MagicNumberH = SET_NUMBERH(MagicNumber);
	pMsg.NumberH = SET_NUMBERH(Obj.m_Index);
	pMsg.NumberL = SET_NUMBERL(Obj.m_Index);

	if (Obj.Type == OBJ_USER)
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);

	MsgSendV2(lpObj, (UCHAR*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCUseEffectItem(CGameObject &Obj, int iEffectUseOption, int iOptionType, int iEffectType, int iLeftTime)
{
	PMSG_USEEFFECTITEM pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x2D, sizeof(pMsg));
	pMsg.byEffectOption = iEffectUseOption;
	pMsg.wOptionType = iOptionType;
	pMsg.wEffectType = iEffectType;
	pMsg.iLeftTime = iLeftTime;
	pMsg.btBuffEffectType = 0;
	//	pMsg.dwEffectValue = 0;

	if (Obj.Type == OBJ_USER)
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}

void GameProtocol::CGMagicCancel(PMSG_MAGICCANCEL * lpMsg, CGameObject &Obj)
{

	WORD MagicNumber = MAKE_NUMBERW(lpMsg->MagicNumberL, lpMsg->MagicNumberH);

	switch (MagicNumber)
	{
	case 77:
		gObjRemoveBuffEffect(lpObj, 6);
		break;
	case 233:
		gObjRemoveBuffEffect(lpObj, 82);
		break;
	case 380:
		gObjRemoveBuffEffect(lpObj, 138);
		break;
	case 383:
		gObjRemoveBuffEffect(lpObj, 139);
		break;
	case 441:
		gObjRemoveBuffEffect(lpObj, 143);
		break;
	}
}

struct PMSG_USE_MONSTERSKILL
{
	PBMSG_HEAD2 h;	// C1:69
	WORD btMonsterSkillNumber;	// 3
	WORD wObjIndex;	// 4
	WORD wTargetObjIndex;	// 6
};

void GameProtocol::GCUseMonsterSkillSend(CGameObject &Obj, CGameObject &TargetObj, int iSkillNumber)
{
	PMSG_USE_MONSTERSKILL pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0x69, 0x00, sizeof(pMsg));
	pMsg.btMonsterSkillNumber = iSkillNumber;
	pMsg.wObjIndex = Obj.m_Index;
	pMsg.wTargetObjIndex = TargetObj.m_Index;

	if (Obj.Type == OBJ_USER)
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);

	MsgSendV2(Obj, (UCHAR*)&pMsg, pMsg.h.size);
}

struct PMSG_VIEWSKILLSTATE
{
	PBMSG_HEAD2 h;	// C1:07
	BYTE State;	// 3
	BYTE NumberH;	// 4
	BYTE NumberL;	// 5
	BYTE BuffEffect; // 6
};

void GameProtocol::GCStateInfoSend(CGameObject &Obj, BYTE state, int BuffViewport)
{
	PMSG_VIEWSKILLSTATE pMsg;

	BYTE BuffCount = 0;

	PHeadSetB((BYTE*)&pMsg, 0x07, sizeof(pMsg));

	pMsg.State = state;

	pMsg.NumberH = HIBYTE(Obj);
	pMsg.NumberL = LOBYTE(Obj);

	pMsg.BuffEffect = BuffViewport;

	if (Obj.Type == OBJ_USER)
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

	if (CC_MAP_RANGE(Obj.MapNumber) || Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)	return;

	MsgSendV2(lpObj, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::CGTeleportRecv(PMSG_TELEPORT* lpMsg, CGameObject &Obj)
{
	if (gObjCheckUsedBuffEffect(&Obj, BUFFTYPE_STUN) == TRUE || gObjCheckUsedBuffEffect(&Obj, BUFFTYPE_SLEEP) == TRUE
		|| gObjCheckUsedBuffEffect(&Obj, BUFFTYPE_FREEZE_2) || gObjCheckUsedBuffEffect(&Obj, BUFFTYPE_EARTH_BINDS))
	{
		PMSG_TELEPORT_RESULT pTeleportResult;

		pTeleportResult.h.c = 0xC3;
		pTeleportResult.h.size = sizeof(pTeleportResult);
		pTeleportResult.h.headcode = 0x1C;
		pTeleportResult.Type = 0;
		pTeleportResult.Unk = 0;
		pTeleportResult.MapNumber = Obj.MapNumber;
		pTeleportResult.Unk1 = 0;
		pTeleportResult.MapX = Obj.X;
		pTeleportResult.MapY = Obj.Y;
		pTeleportResult.Dir = Obj.Dir;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pTeleportResult, pTeleportResult.h.size);
		return;
	}

	

	if (Obj.m_bMapSvrMoveReq == true || Obj.m_bMapSvrMoveReq_2 == true || Obj.m_bMapSvrMoveQuit == true || Obj.m_State == 32)
	{
		return;
	}

	if (IT_MAP_RANGE(Obj.MapNumber))
	{
		if (g_IT_Event.GetIllusionTempleState(Obj.MapNumber) == 2)
		{
			if (g_IT_Event.CheckRelics(Obj.m_Index) != FALSE)
			{
				PMSG_TELEPORT_RESULT pTeleportResult;

				pTeleportResult.h.c = 0xC3;
				pTeleportResult.h.size = sizeof(pTeleportResult);
				pTeleportResult.h.headcode = 0x1C;
				pTeleportResult.Type = 0;
				pTeleportResult.Unk = 0;
				pTeleportResult.MapNumber = Obj.MapNumber;
				pTeleportResult.Unk1 = 0;
				pTeleportResult.MapX = Obj.X;
				pTeleportResult.MapY = Obj.Y;
				pTeleportResult.Dir = Obj.Dir;

				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pTeleportResult, pTeleportResult.h.size);
				return;
			}

			if (g_IT_Event.CheckSkillRestraint(Obj.m_nITR_Index, Obj.MapNumber) != FALSE)
			{
				PMSG_TELEPORT_RESULT pTeleportResult;

				pTeleportResult.h.c = 0xC3;
				pTeleportResult.h.size = sizeof(pTeleportResult);
				pTeleportResult.h.headcode = 0x1C;
				pTeleportResult.Type = 0;
				pTeleportResult.Unk = 0;
				pTeleportResult.MapNumber = Obj.MapNumber;
				pTeleportResult.Unk1 = 0;
				pTeleportResult.MapX = Obj.X;
				pTeleportResult.MapY = Obj.Y;
				pTeleportResult.Dir = Obj.Dir;

				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pTeleportResult, pTeleportResult.h.size);
				return;
			}
		}
	}

	if (IT_MAP_RANGE(Obj.MapNumber))
	{
		if (g_IT_Event.GetIllusionTempleState(Obj.MapNumber) == 1)
		{
			PMSG_TELEPORT_RESULT pTeleportResult;

			pTeleportResult.h.c = 0xC3;
			pTeleportResult.h.size = sizeof(pTeleportResult);
			pTeleportResult.h.headcode = 0x1C;
			pTeleportResult.Type = 0;
			pTeleportResult.Unk = 0;
			pTeleportResult.MapNumber = Obj.MapNumber;
			pTeleportResult.Unk1 = 0;
			pTeleportResult.MapX = Obj.X;
			pTeleportResult.MapY = Obj.Y;
			pTeleportResult.Dir = Obj.Dir;

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pTeleportResult, pTeleportResult.h.size);
			return;
		}
	}

	if (CC_MAP_RANGE(Obj.MapNumber) != FALSE) //season 4.0 add-on (fix)
	{
		if (g_ChaosCastle.GetCurrentState(Obj.m_cChaosCastleIndex) == 3)
		{
			PMSG_TELEPORT_RESULT pTeleportResult;

			pTeleportResult.h.c = 0xC3;
			pTeleportResult.h.size = sizeof(pTeleportResult);
			pTeleportResult.h.headcode = 0x1C;
			pTeleportResult.Type = 0;
			pTeleportResult.Unk = 0;
			pTeleportResult.MapNumber = Obj.MapNumber;
			pTeleportResult.Unk1 = 0;
			pTeleportResult.MapX = Obj.X;
			pTeleportResult.MapY = Obj.Y;
			pTeleportResult.Dir = Obj.Dir;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pTeleportResult, pTeleportResult.h.size);
			return;
		}
	}

	if (Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL && g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		if (g_ChaosCastleFinal.GetCurrentState() == 3)
		{
			PMSG_TELEPORT_RESULT pTeleportResult;

			pTeleportResult.h.c = 0xC3;
			pTeleportResult.h.size = sizeof(pTeleportResult);
			pTeleportResult.h.headcode = 0x1C;
			pTeleportResult.Type = 0;
			pTeleportResult.Unk = 0;
			pTeleportResult.MapNumber = Obj.MapNumber;
			pTeleportResult.Unk1 = 0;
			pTeleportResult.MapX = Obj.X;
			pTeleportResult.MapY = Obj.Y;
			pTeleportResult.Dir = Obj.Dir;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pTeleportResult, pTeleportResult.h.size);
			return;
		}
	}

	if (DG_MAP_RANGE(Obj.MapNumber) != FALSE)
	{
		if (g_DoppelGanger.GetDoppelgangerState() != 2)
		{
			PMSG_TELEPORT_RESULT pTeleportResult;

			pTeleportResult.h.c = 0xC3;
			pTeleportResult.h.size = sizeof(pTeleportResult);
			pTeleportResult.h.headcode = 0x1C;
			pTeleportResult.Type = 0;
			pTeleportResult.Unk = 0;
			pTeleportResult.MapNumber = Obj.MapNumber;
			pTeleportResult.Unk1 = 0;
			pTeleportResult.MapX = Obj.X;
			pTeleportResult.MapY = Obj.Y;
			pTeleportResult.Dir = Obj.Dir;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pTeleportResult, pTeleportResult.h.size);
			return;
		}
	}

	if (lpMsg->MoveNumber == 0)
	{
		BYTE x = lpMsg->MapX;
		BYTE y = lpMsg->MapY;

		if (gObjCheckTeleportArea(Obj, x, y) == FALSE)
		{

			PMSG_TELEPORT_RESULT pTeleportResult;

			pTeleportResult.h.c = 0xC3;
			pTeleportResult.h.size = sizeof(pTeleportResult);
			pTeleportResult.h.headcode = 0x1C;
			pTeleportResult.Type = 0;
			pTeleportResult.Unk = 0;
			pTeleportResult.MapNumber = Obj.MapNumber;
			pTeleportResult.Unk1 = 0;
			pTeleportResult.MapX = Obj.X;
			pTeleportResult.MapY = Obj.Y;
			pTeleportResult.Dir = Obj.Dir;

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pTeleportResult, pTeleportResult.h.size);
			return;
		}

		if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
		{
			if (Obj.MapNumber == MAP_INDEX_CASTLESIEGE)
			{
				if (g_CastleSiege.CheckTeleportMagicAxisY(Obj.Y, x, y) == FALSE)
				{
					y = Obj.Y;
				}
			}
		}

		CMagicInf * lpMagic;
		PMSG_MAGICATTACK_RESULT pAttack;
		PHeadSetBE((BYTE*)&pAttack, 0x19, sizeof(pAttack));
		pAttack.MagicNumberH = SET_NUMBERH(AT_SKILL_TELEPORT);
		pAttack.MagicNumberL = SET_NUMBERL(AT_SKILL_TELEPORT);
		pAttack.SourceNumberH = SET_NUMBERH(Obj.m_Index);
		pAttack.SourceNumberL = SET_NUMBERL(Obj.m_Index);
		pAttack.TargetNumberH = SET_NUMBERH(Obj.m_Index);
		pAttack.TargetNumberL = SET_NUMBERL(Obj.m_Index);

		lpMagic = gObjGetMagicSearch(&Obj, AT_SKILL_TELEPORT);

		if (lpMagic)
		{
			int usemana = gObjMagicManaUse(&Obj, lpMagic);

			if (usemana >= 0)
			{
				int usebp = gObjMagicBPUse(&Obj, lpMagic);

				if (usebp >= 0)
				{
					if (Obj.Type == OBJ_USER)
						IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pAttack, pAttack.h.size);

					MsgSendV2(&Obj, (BYTE*)&pAttack, pAttack.h.size);
					gObjTeleportMagicUse(Obj, x, y);
					Obj.Mana = usemana;
					Obj.BP = usebp;
					GCManaSend(Obj, Obj.Mana, -1, 0, Obj.BP);
				}
			}
		}
	}

	if (IMPERIAL_MAP_RANGE(Obj.MapNumber) == TRUE)
	{
		int nCurrentZoneIndex = g_ImperialGuardian.GetCurrentZoneIndex(Obj);

		if (nCurrentZoneIndex >= 0)
		{
			g_ImperialGuardian.CGEnterPortal(Obj, nCurrentZoneIndex + 1);
		}
	}

	if (Obj.m_IfState->use == 1 && Obj.m_IfState->type == 6)
	{
		return;
	}

	if (gGateC.IsInGate(Obj, lpMsg->MoveNumber))
	{
		gObjMoveGate(Obj, lpMsg->MoveNumber);
		return;
	}

	gObjClearViewport(&Obj);
	GCTeleportSend(&Obj, lpMsg->MoveNumber, Obj.MapNumber, Obj.X, Obj.Y, Obj.Dir);

	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (Obj.MapNumber == MAP_INDEX_CASTLESIEGE)
		{
			if (g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
			{
				g_CastleSiege.NotifySelfCsJoinSide(Obj);
				g_CastleSiege.NotifyCsSelfLeftTime(Obj);
			}
		}
	}
}
struct PMSG_RESULT_MOVEDEVILSQUARE
{
	PBMSG_HEAD2 h;	// C1:90
	BYTE Result;	// 3
};

void GameProtocol::CGReqMapMove(PMSG_REQ_MAPMOVE *lpMsg, CGameObject &Obj)
{
	PMSG_ANS_MAPMOVE pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0x8E, 0x03, sizeof(pMsg));

	pMsg.btResult = 0;

	if (Obj.Type != OBJ_USER)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (g_KeyGenerater.CheckKeyValue(Obj.m_PlayerData->m_dwMapMoveKeyValue, lpMsg->dwBlockKey) == FALSE)
	{
		pMsg.btResult = 12;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (Obj.Connected < PLAYER_PLAYING)
	{
		pMsg.btResult = 11;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (Obj.Teleport != 0)
	{
		pMsg.btResult = 2;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (Obj.m_PlayerData->m_bSecurityCheck == false)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (Obj.m_bMapSvrMoveReq == true || Obj.m_bMapSvrMoveReq_2 == true || Obj.m_bMapSvrMoveQuit == true || Obj.m_State == 32)
	{
		return;
	}

	if (Obj.m_bPShopOpen == true)
	{
		if (Obj.MapNumber == MAP_INDEX_CASTLESIEGE && lpMsg->wMapIndex == MAP_INDEX_RORENCIA)
		{
			pMsg.btResult = 3;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
			return;
		}

		if (Obj.MapNumber == MAP_INDEX_RORENCIA && lpMsg->wMapIndex == MAP_INDEX_CASTLESIEGE)
		{
			pMsg.btResult = 3;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
			return;
		}
	}

	if (Obj.m_IfState->use == 1 && Obj.m_IfState->type == 6)
	{
		pMsg.btResult = 2;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (Obj.DieRegen == TRUE)
	{
		pMsg.btResult = 2;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (Obj.RegenOk != FALSE)
	{
		pMsg.btResult = 2;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (pMsg.btResult)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	gMoveCommand.Move(lpObj, lpMsg->wMapIndex);
}

void GameProtocol::GCSendMapMoveCheckSum(CGameObject &Obj)
{
	PMSG_MAPMOVE_CHECKSUM pMsg;

	pMsg.dwKeyValue = Obj.m_PlayerData->m_dwMapMoveKeyValue;
	PHeadSubSetB((BYTE*)&pMsg, 0x8E, 0x01, sizeof(pMsg));

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::CGTargetTeleportRecv(PMSG_TARGET_TELEPORT * lpMsg, CGameObject &Obj)
{

	if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_STONE) || gObjCheckUsedBuffEffect(&Obj, BUFFTYPE_STUN) || gObjCheckUsedBuffEffect(&Obj, BUFFTYPE_SLEEP) ||
		gObjCheckUsedBuffEffect(Obj, BUFFTYPE_FREEZE_2) || gObjCheckUsedBuffEffect(Obj, BUFFTYPE_EARTH_BINDS))
		return;

	CMagicInf * lpMagic;
	BYTE x;
	BYTE y;
	x = lpMsg->MapX;
	y = lpMsg->MapY;
	int iTargetIndex = MAKE_NUMBERW(lpMsg->NumberL, lpMsg->NumberH);	// #error? parameters inverse?

	if (!gObjIsConnectedGP(iTargetIndex))
	{
		return;
	}

	if (IT_MAP_RANGE(getGameObject(iTargetIndex)->MapNumber))
	{
		if (g_IT_Event.CheckRelics(iTargetIndex))
		{
			return;
		}
	}

	if (Obj.PartyNumber != getGameObject(iTargetIndex)->PartyNumber ||
		Obj.PartyNumber == -1 ||
		getGameObject(iTargetIndex)->PartyNumber == -1)
	{
		return;
	}

	if (!gObjCheckTeleportArea(*getGameObject(iTargetIndex), x, y))
	{
		return;
	}

	PMSG_MAGICATTACK_RESULT pAttack;

	PHeadSetBE((BYTE*)&pAttack, 0x19, sizeof(pAttack));
	WORD MagicNumber = AT_SKILL_TARGET_TELEPORT;
	pAttack.MagicNumberH = SET_NUMBERH(MagicNumber);
	pAttack.MagicNumberL = SET_NUMBERL(MagicNumber);
	pAttack.SourceNumberH = SET_NUMBERH(Obj.m_Index);
	pAttack.SourceNumberL = SET_NUMBERL(Obj.m_Index);
	pAttack.TargetNumberH = SET_NUMBERH(iTargetIndex);	// #error? parameters inverse?
	pAttack.TargetNumberL = SET_NUMBERL(iTargetIndex);	// #error? parameters inverse?
	lpMagic = gObjGetMagicSearch(Obj, AT_SKILL_TARGET_TELEPORT);

	if (lpMagic)
	{
		int usemana = gObjMagicManaUse(&Obj, lpMagic);

		if (usemana >= 0)
		{
			int usebp = gObjMagicBPUse(Obj, lpMagic);

			if (usebp >= 0)
			{
				if (Obj.Type == OBJ_USER)
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pAttack, pAttack.h.size);

				MsgSendV2(Obj, (BYTE*)&pAttack, pAttack.h.size);
				gObjTeleportMagicUse(*getGameObject(iTargetIndex), x, y);
				Obj.Mana = usemana;
				Obj.BP = usebp;
				GCManaSend(Obj, Obj.Mana, -1, 0, Obj.BP);

				return;
			}
		}
	}
}




void GameProtocol::GCTeleportSend(CGameObject &Obj, WORD MoveNumber, BYTE MapNumber, BYTE MapX, BYTE MapY, BYTE Dir)
{
	PMSG_TELEPORT_RESULT pMsg;

	if (Obj.Type != OBJ_USER)
		return;

	pMsg.h.c = 0xC3;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x1C;
	if (MoveNumber > 0)
	{
		pMsg.Type = 256; // Warp
	}
	else
	{
		pMsg.Type = 0; // Teleportacja
	}
	pMsg.MapNumber = MapNumber;
	pMsg.Unk1 = 0;
	pMsg.Unk = 0;
	pMsg.MapX = MapX;
	pMsg.MapY = MapY;
	pMsg.Dir = Dir;
	// mapserver

	if (MoveNumber == 0)
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
	else
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}

#include "SkillHitBox.h"

void GameProtocol::CGBeattackRecv(BYTE* lpRecv, CGameObject &Obj, int magic_send)
{
	if (Obj < 0 || Obj > g_ConfigRead.server.GetObjectMax() - 1)
	{
		return;
	}

	// Set the lpRecv to a Predeterminated Packet
	PMSG_BEATTACK_COUNT * lpCount = (PMSG_BEATTACK_COUNT *)lpRecv;

	// Check the Protocol
	if (lpCount->h.headcode != 0xD3)
	{
		return;
	}

	// Check if the count is leess than 1
	if (lpCount->Count < 1)
	{
		return;
	}

	// Set the Max limitation to 5
	if (lpCount->Count > 5)
	{
		lpCount->Count = 5;
	}

	// Set the Start of the Offset
	int lOfs = sizeof(PMSG_BEATTACK_COUNT);
	CMagicInf * lpMagic;
	int tNumber;
	PMSG_BEATTACK * lpMsg;

	// Set lpMagic according
	// if Obj is OBJ_USER
	// or if it is OBJ_MONSTER
	if (Obj.Type == OBJ_USER)
	{
		BYTE attr = MapC[Obj.MapNumber].GetAttr(Obj.X, Obj.Y);

		if ((attr & 1) == 1) //safezone
		{
			gObjSetPosition(Obj, Obj.X, Obj.Y);
			return;
		}
		//season 4.5 add-on end

		WORD MagicNumber = MAKE_NUMBERW(lpCount->MagicNumberH, lpCount->MagicNumberL);

		lpMagic = gObjGetMagicSearch(&Obj, MagicNumber);
	}
	else
	{
		WORD MagicNumber = MAKE_NUMBERW(lpCount->MagicNumberH, lpCount->MagicNumberL);

		lpMagic = gObjGetMagic(&Obj, MagicNumber);
	}

	if (Obj.Type == OBJ_USER && Obj.m_PlayerData->m_bSecurityCheck == false)
		return;

	if (Obj.Teleport && gObjUseSkill.m_SkillData.EnableWizardSkillWhileTeleport == false)
		return;

	// Check if there is Magic
	if (lpMagic == NULL)
	{
		return;
	}

	// Avoid use Skill of the following types
	if (lpMagic->m_Skill == 42 ||
		lpMagic->m_Skill == 43 ||
		lpMagic->m_Skill == 41 ||
		g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == 336 ||
		g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == 331 ||
		lpMagic->m_Skill == 333 ||
		lpMagic->m_Skill == 339 ||
		lpMagic->m_Skill == 342 ||
		lpMagic->m_Skill == 332 ||
		lpMagic->m_Skill == 330 ||
		lpMagic->m_Skill == AT_SKILL_FIRE_BLOW)
	{
		return;
	}

	if (Obj.Type == OBJ_USER)
	{
		// Check if Player can use Magic
		// Acording to its Class
		if (MagicDamageC.SkillGetRequireClass(Obj.Class,
			Obj.m_PlayerData->ChangeUP,
			lpMagic->m_Skill) < 1)
		{
			// It's not his magic [%s][%s] .
			gObjUserKill(Obj);
			return;
		}

		// Check the time cast Spell
		// No longer that 8 seconds
		if ((GetTickCount() - Obj.UseMagicTime) > 8000)
		{
			sLog->outError( "Too long time passed after casting magic [%s][%s] (%d)(%d)",
				Obj.AccountID, Obj.Name, lpMagic->m_Skill, GetTickCount() - Obj.UseMagicTime);

			return;
		}

		// Check if user cast to much Spells
		// Npo longer thant 4
		if (Obj.UseMagicCount > 4)
		{
			return;
		}

		Obj.UseMagicCount++;
	}

	// Avoid Attack out of the predeterminated
	// Magic need to be one of the followingla
	//	
	//	8	"Twister"
	//	9	"Evil Spirit"
	//	10	"Hellfire"
	//	13	"Blast"
	//	14	"Inferno"
	//	5	"Flame"
	//	24	"Triple Shot"
	//	50	"Flame of Evil(Monster)"
	//	12	"Aquaflash"
	//	41	"Twisting Slash"
	//	47	"Impale"
	//	43	"Death Stab"
	//	42	"Rageful Blow"
	//	52	"Penetration"
	//	55	"Fire Slash"
	//	78	"#Unknown"
	//  Master Triple Shot
	//  Master CometFall
	//  Master FireSlash
	//  235 "Five Shot"

	if (g_MasterLevelSkillTreeSystem.CheckMasterLevelSkill(lpMagic->m_Skill))
	{
		int iBaseMLS = g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill);

		if (iBaseMLS != 403
			&& iBaseMLS != 388
			&& iBaseMLS != 330
			&& iBaseMLS != 413
			&& iBaseMLS != 414
			&& iBaseMLS != 481
			&& iBaseMLS != 356
			&& iBaseMLS != 382
			&& iBaseMLS != 512
			&& iBaseMLS != 508
			&& iBaseMLS != 385
			&& iBaseMLS != 391
			&& iBaseMLS != 336
			&& iBaseMLS != 331
			&& iBaseMLS != 417
			&& iBaseMLS != 420
			&& iBaseMLS != 490
			&& iBaseMLS != 482
			&& iBaseMLS != 518
			&& iBaseMLS != 487
			&& iBaseMLS != 326
			&& iBaseMLS != 327
			&& iBaseMLS != 328
			&& iBaseMLS != AT_MSKILL_DK_LUNGE1
			&& iBaseMLS != 332
			&& iBaseMLS != 333
			&& iBaseMLS != 337
			&& iBaseMLS != 339
			&& iBaseMLS != 340
			&& iBaseMLS != 342
			&& iBaseMLS != 343
			&& iBaseMLS != 344
			&& iBaseMLS != 346
			&& iBaseMLS != 360
			&& iBaseMLS != 363
			&& iBaseMLS != 378
			&& iBaseMLS != 379
			&& iBaseMLS != AT_MSKILL_DW_EXPWIZARDRY1
			&& iBaseMLS != 381
			&& iBaseMLS != AT_MSKILL_DW_EXPWIZARDRY2
			&& iBaseMLS != 384
			&& iBaseMLS != 386
			&& iBaseMLS != 387
			&& iBaseMLS != 389
			&& iBaseMLS != 392
			&& iBaseMLS != 393
			&& iBaseMLS != 404
			&& iBaseMLS != 406
			&& iBaseMLS != 415
			&& iBaseMLS != 416
			&& iBaseMLS != 418
			&& iBaseMLS != 419
			&& iBaseMLS != 425
			&& iBaseMLS != 426
			&& iBaseMLS != 427
			&& iBaseMLS != 428
			&& iBaseMLS != 429
			&& iBaseMLS != 430
			&& iBaseMLS != 432
			&& iBaseMLS != 433
			&& iBaseMLS != 441
			&& iBaseMLS != 459
			&& iBaseMLS != 460
			&& iBaseMLS != 461
			&& iBaseMLS != 462
			&& iBaseMLS != 466
			&& iBaseMLS != 468
			&& iBaseMLS != AT_MSKILL_SUM_BERSERK1
			&& iBaseMLS != 470
			&& iBaseMLS != 472
			&& iBaseMLS != 479
			&& iBaseMLS != 480
			&& iBaseMLS != 483
			&& iBaseMLS != 484
			&& iBaseMLS != 486
			&& iBaseMLS != 488
			&& iBaseMLS != 489
			&& iBaseMLS != 492
			&& iBaseMLS != 493
			&& iBaseMLS != 494
			&& iBaseMLS != 495
			&& iBaseMLS != 496
			&& iBaseMLS != 497
			&& iBaseMLS != 509
			&& iBaseMLS != 510
			&& iBaseMLS != 514
			&& iBaseMLS != 515
			&& iBaseMLS != 516
			&& iBaseMLS != 519
			&& iBaseMLS != 520
			&& iBaseMLS != 523
			&& iBaseMLS != 539
			&& iBaseMLS != 551
			&& iBaseMLS != 552
			&& iBaseMLS != 554
			&& iBaseMLS != 555
			&& iBaseMLS != 556
			&& iBaseMLS != 558
			&& iBaseMLS != 559
			&& iBaseMLS != 560
			&& iBaseMLS != 561
			&& iBaseMLS != 562
			&& iBaseMLS != 563
			&& iBaseMLS != 564
			&& iBaseMLS != 565
			&& iBaseMLS != 566
			&& iBaseMLS != 567
			&& iBaseMLS != 575
			&& iBaseMLS != 577
			&& iBaseMLS != 411
			&& iBaseMLS != 424
			&& iBaseMLS != 431
			&& iBaseMLS != 463
			&& iBaseMLS != 491
			&& iBaseMLS != 522
			&& iBaseMLS != 521
			&& iBaseMLS != 524
			&& iBaseMLS != 551
			&& iBaseMLS != 552
			&& iBaseMLS != 554
			&& iBaseMLS != 555
			&& iBaseMLS != 556
			&& iBaseMLS != 558
			&& iBaseMLS != 559
			&& iBaseMLS != 560
			&& iBaseMLS != 561
			&& iBaseMLS != 562
			&& iBaseMLS != 563
			&& iBaseMLS != 564
			&& iBaseMLS != 565
			&& iBaseMLS != 566
			&& iBaseMLS != 567
			&& iBaseMLS != 575
			&& iBaseMLS != 577
			&& iBaseMLS != 631
			&& iBaseMLS != AT_MSKILL_OBSIDIAN_POWUP
			&& iBaseMLS != AT_MSKILL_CIRCLE_SHIELD_POWUP
			&& iBaseMLS != AT_MSKILL_CIRCLE_SHIELD_MASTERY
			&& iBaseMLS != AT_MSKILL_WRATH_POWUP
			&& iBaseMLS != AT_MSKILL_WRATH_PROFICIENCY
			&& iBaseMLS != AT_MSKILL_WRATH_MASTERY)
		{
			return;
		}
	}

	else
	{
		if (lpMagic->m_Skill != 8
			&& lpMagic->m_Skill != 9
			&& lpMagic->m_Skill != 10
			&& lpMagic->m_Skill != 13
			&& lpMagic->m_Skill != 14
			&& lpMagic->m_Skill != 5
			&& lpMagic->m_Skill != 24
			&& lpMagic->m_Skill != 50
			&& lpMagic->m_Skill != 12
			&& lpMagic->m_Skill != 41
			&& lpMagic->m_Skill != 47
			&& lpMagic->m_Skill != 43
			&& lpMagic->m_Skill != 42
			&& lpMagic->m_Skill != 52
			&& lpMagic->m_Skill != 55
			&& lpMagic->m_Skill != 78
			&& lpMagic->m_Skill != 277) // Penetration
		{
			return;
		}
	}

	if (lpMagic->m_Skill == 24 || lpMagic->m_Skill == 78 || g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == 414 || g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == 518
		|| lpMagic->m_Skill == 418 || lpMagic->m_Skill == 520)
	{
		int NSAttackSerial = lpCount->Serial;

		if (Obj.OSAttackSerial >= 255 && (Obj.OSAttackSerial - lpCount->Serial) > 50)
		{
			Obj.OSAttackSerial = -1;
		}

		if (NSAttackSerial > Obj.OSAttackSerial)
		{
			Obj.OSAttackSerial = NSAttackSerial;
			
		}
	}

	if (lpMagic->m_Skill == AT_SKILL_EVIL ||
		g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == 385 ||
		g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == 487)
	{
		if (Obj.Class != CLASS_WIZARD && Obj.Class != CLASS_MAGUMSA)
		{
			return;
		}
	}

	if (bIsIgnorePacketSpeedHackDetect)
	{
		

		if (Obj.m_SpeedHackPenalty > 0)
		{
			Obj.m_SpeedHackPenalty--;
			return;
		}
	}

	

	int lOfs2 = lOfs;
	int pTargetNumber[128];

	for (int i = 0; i < lpCount->Count; i++)
	{
		lpMsg = (PMSG_BEATTACK *)&lpRecv[lOfs2];
		pTargetNumber[i] = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
		lOfs2 += sizeof(PMSG_BEATTACK);

		if (i >= 1)
		{
			int nTargetDistance = gObjCalDistanceTX(*getGameObject(pTargetNumber[0]), *getGameObject(pTargetNumber[i]));

			if (getGameObject(pTargetNumber[i])->Type == OBJ_MONSTER && nTargetDistance > 10)
			{
				sLog->outBasic("User Hack Detected [%s][%s] Skill: %d, Monster Distance: %d, Cnt:%d",
					Obj.AccountID, Obj.Name, lpMagic->m_Skill, nTargetDistance, lpCount->Count);
			}
		}
	}

	for (int n = 0; n < lpCount->Count; n++)
	{
		lpMsg = (PMSG_BEATTACK *)&lpRecv[lOfs];
		tNumber = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
		

		if (!lpMsg->MagicKey && (lpMagic->m_Skill == AT_SKILL_EVIL || g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == 385 || g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == 487))	// "Evil Spirit"
		{
			return;
		}

		if (lpMsg->MagicKey && (lpMagic->m_Skill == AT_SKILL_EVIL || g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == 385 || g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == 487))	// "Evil Spirit"
		{
			if (Obj.DurMagicKeyChecker.IsValidDurationTime(lpMsg->MagicKey) == FALSE)
			{
				lOfs += sizeof(PMSG_BEATTACK);

				continue;
			}

			if (Obj.DurMagicKeyChecker.IsValidCount(lpMsg->MagicKey) == FALSE)
			{
				lOfs += sizeof(PMSG_BEATTACK);

				continue;
			}
		}

		if (gWriteSkillLog)
		{
			sLog->outBasic("Magic Attack3 : %d, serial = %d, Tgt =  %d, cnt = %d",
				lpMagic->m_Skill, lpCount->Serial, tNumber, lpCount->Count);
		}

		//season4 add-on start
		if (ObjectMaxRange(tNumber) == false)
		{
			continue;
		}

		int DistanceCheck = gObjCalDistance(lpObj, *getGameObject(tNumber));

		if (DistanceCheck > 13)
		{
			lOfs += sizeof(PMSG_BEATTACK);
			continue;
		}
		//season4 add-on end

		for (int i = 0; i < lpCount->Count; i++)
		{
			if (n != i)
			{
				if (pTargetNumber[i] == tNumber)
				{
					//this->GCSendDisableReconnect(Obj);
					IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
					return;
				}
			}
		}

		if (tNumber >= 0 && tNumber < g_ConfigRead.server.GetObjectMax() - 1)
		{
			if (lpMagic->m_Skill == AT_SKILL_DEFENSEDOWN)
			{
				gObjUseSkill.MaGumSkillDefenseDown(Obj, tNumber, lpMagic->m_Level);
				gObjAttack(&Obj, *getGameObject(tNumber), lpMagic, TRUE, 1, 0, FALSE, 0, 0);
			}

			else if (lpMagic->m_Skill == AT_SKILL_FIRESCREAM)
			{
				gObjUseSkill.SkillFireScream(Obj, tNumber, lpMagic);
			}

			else if (g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == AT_MSKILL_MG_FIRESLASH1)
			{
				g_MasterLevelSkillTreeSystem.MLS_MaGumSkillDefenseDown(Obj, tNumber);
				gObjAttack(Obj, *getGameObject(tNumber), lpMagic, TRUE, 1, 0, FALSE, 0, 0);
			}

			else if (g_MasterLevelSkillTreeSystem.GetBaseMasterLevelSkill(lpMagic->m_Skill) == AT_MSKILL_MG_FIRESLASH2)
			{
				g_MasterLevelSkillTreeSystem.MLS_SkillMasteryDefenseDown(Obj, tNumber, lpMagic);
				gObjAttack(Obj, *getGameObject(tNumber), lpMagic, TRUE, 1, 0, FALSE, 0, 0);
			}

			else if (lpMagic->m_Skill == AT_MSKILL_DL_FIRESCREAM1 || lpMagic->m_Skill == AT_MSKILL_DL_FIRESCREAM2)
			{
				g_MasterLevelSkillTreeSystem.MLS_FireScream(Obj, tNumber, lpMagic);
			}
			else if (lpMagic->m_Skill == AT_SKILL_SHINING_PEAK)
			{
				gObjUseSkill.SkillShiningPeak(Obj, lpMagic, tNumber);
			}
			else if (magic_send)
			{
				gObjAttack(Obj, *getGameObject(tNumber), lpMagic, TRUE, 1, 0, FALSE, 0, 0);
			}
			else
			{
				gObjAttack(Obj, *getGameObject(tNumber), lpMagic, FALSE, 1, 0, FALSE, 0, 0);
			}

		}

		lOfs += sizeof(PMSG_BEATTACK);
	}
}

void GameProtocol::CGDurationMagicRecv(PMSG_DURATION_MAGIC_RECV* lpMsg, CGameObject &Obj)
{
	if (Obj < 0 || Obj > g_ConfigRead.server.GetObjectMax() - 1)
	{
		return;
	}

	//PMSG_DURATION_MAGIC_RECV * lpMsg = (PMSG_DURATION_MAGIC_RECV *)(lpRecv);

	CMagicInf * lpMagic;
	

	if (Obj.Teleport && gObjUseSkill.m_SkillData.EnableWizardSkillWhileTeleport == false)
	{
		return;
	}

	WORD MagicNumber = MAKE_NUMBERW(lpMsg->MagicNumberH, lpMsg->MagicNumberL);
	if (Obj.Type == OBJ_USER)
	{
		lpMagic = gObjGetMagicSearch(lpObj, MagicNumber);
	}
	else
	{
		lpMagic = gObjGetMagic(lpObj, MagicNumber);
	}

	if (lpMagic == NULL)
	{
		return;
	}

	if (Obj.Type == OBJ_USER)
	{
		if (MagicDamageC.CheckStatus(lpMagic->m_Skill, Obj.m_PlayerData->GuildStatus) == FALSE)
		{
			return;
		}

		if (MagicDamageC.CheckKillCount(lpMagic->m_Skill, Obj.m_btKillCount) < 0)
		{
			return;
		}

		if (Obj.m_PlayerData->m_bSecurityCheck == false)
		{
			return;
		}
	}

	Obj.UseMagicNumber = 0;

	if (lpMagic->m_Skill != 24)
	{
		if (!gObjUseSkill.SpeedHackCheck(Obj))
		{
			return;
		}
	}

	if (!gObjUseSkill.EnableSkill(lpMagic->m_Skill))
	{
		return;
	}

	int aTargetIndex = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	gObjUseSkill.UseSkill(Obj, lpMagic, lpMsg->X, lpMsg->Y, lpMsg->Dir, lpMsg->TargetPos, aTargetIndex);

	if (lpMsg->MagicKey)
		Obj.DurMagicKeyChecker.SetDurationTime(lpMsg->MagicKey, GetTickCount());
}

void GameProtocol::CGUseItemRecv(PMSG_USEITEM* lpMsg, CGameObject &Obj)
{
	int pos;
	CItemObject * CItemObject;
	int iItemUseType = lpMsg->btItemUseType;

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	// Check User States
	if (Obj.m_IfState->use && Obj.m_IfState->type != 3)
	{
		GCReFillSend(Obj, Obj.Life, 0xFD, 1, Obj.iShield);

		return;
	}

	if (Obj.m_PlayerData->m_bSecurityCheck == false)
	{
		GCReFillSend(Obj, Obj.Life, 0xFD, 1, Obj.iShield);
		MsgOutput(Obj, Lang.GetText(0, 822));
		return;
	}

	// Check if User is exiting
	if (Obj.CloseType != -1)
	{
		GCReFillSend(Obj, Obj.Life, 0xFD, 1, Obj.iShield);
		return;
	}

	// Fix Inventory Pointer
	if (gObjFixInventoryPointer(Obj) == false)
	{
		sLog->outBasic("[Fix Inv.Ptr] False Location - %s, %d",
			__FILE__, __LINE__);
		//return;
	}

	// Check if there is a Transaction
	// Dupe
	if (Obj.pTransaction == 1)
	{
		return;
	}

	if (g_NewPVP.IsObserver(Obj))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 319), Obj, 1);
		return;
	}

	// Set Var Pos
	pos = lpMsg->inventoryPos;

	// Check if the pos is in bounds
	if (pos > MAIN_INVENTORY_SIZE - 1)
	{
		// error : Location is over the range. %s/%d
		GCReFillSend(Obj, Obj.Life, 0xFD, 1, Obj.iShield);

		return;
	}

	// check if the item
	// have the same position
	// as the new position
	if (lpMsg->inventoryPos == lpMsg->invenrotyTarget)
	{
		GCReFillSend(Obj, Obj.Life, 0xFD, 1, Obj.iShield);

		return;
	}

	// Set Pointer
	CItemObject* lpItem = &Obj.pntInventory[pos];
	lpItem->FixIt(); //K2
	//lpItem.FixIt;
	// The item is ITEM
	if (lpItem->IsItem())
	{
		// Check if item have 0 as Serial
		if (gObjCheckSerial0ItemList(lpItem))
		{
			MsgOutput(Obj, Lang.GetText(0, 259));

			GCReFillSend(Obj, Obj.Life, 0xFD, 1, Obj.iShield);

			return;
		}

		// Check the item Serial
		if (gObjInventorySearchSerialNumber(Obj,
			lpItem->GetNumber()) == FALSE)
		{
			GCReFillSend(Obj, Obj.Life, 0xFD, 1, Obj.iShield);
			return;
		}

		if (lpItem->m_serial && !gObjCanItemTouch(&Obj, 1))
		{
			GCReFillSend(Obj, Obj.Life, 0xFD, 1, Obj.iShield);
			return;
		}

		if (lpItem->m_Type == ITEMGET(14, 0) || lpItem->m_Type == ITEMGET(14, 1) || lpItem->m_Type == ITEMGET(14, 2) || lpItem->m_Type == ITEMGET(14, 3) || lpItem->m_Type == ITEMGET(14, 133))
		{
			int potion;
			for (potion = 0; potion < g_Michi.FixHackPotions.m_LifePotionMultiplyCount; potion++)
			{
				if (potion == 0 || (rand() % 100) < g_Michi.FixHackPotions.m_LifePotionMultiplyRate)
				{
					g_Michi.UseHealingPotion(lpItem, pos, Obj);
				}
			}
		}
		else if (lpItem->m_Type == ITEMGET(14, 4) ||	// Small Mana Potion
			lpItem->m_Type == ITEMGET(14, 5) || lpItem->m_Type == ITEMGET(14, 6))
		{
			int tMana = (lpItem->m_Value * 10) - (Obj.Level);	// #formula

			if (tMana < 0)
			{
				tMana = 0;
			}

			switch (lpItem->m_Type)
			{
			case ITEMGET(14, 4):	// Small Mana Potion
				tMana += ((int)(Obj.MaxMana + Obj.AddMana)) * 20 / 100;
				break;
			case ITEMGET(14, 5):	// Mana Potion
				tMana += ((int)(Obj.MaxMana + Obj.AddMana)) * 30 / 100;
				break;
			case ITEMGET(14, 6):	// Large Mana Potion
				tMana += ((int)(Obj.MaxMana + Obj.AddMana)) * 40 / 100;
				break;
			}

			Obj.Mana += tMana;

			if (Obj.Mana > (Obj.MaxMana + Obj.AddMana - 1.0f))
				Obj.Mana = Obj.MaxMana + Obj.AddMana;

			GCManaSend(Obj, (WORD)Obj.Mana, 0xFF, 0, Obj.BP);

			if (!gObjSearchItemMinus(&Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}
		}
		else if (lpItem->m_Type == ITEMGET(14, 36))
		{
			int iShieldGage = (Obj.iMaxShield + Obj.iAddShield) * 35 / 100;	// #formula

			if (Obj.iShield < 0)
				Obj.iShield = 0;

			if (Obj.iFillShield > 0)
			{
				Obj.iShield += Obj.iFillShield;

				if (Obj.iShield > (Obj.iMaxShield + Obj.iAddShield))
					Obj.iShield = Obj.iMaxShield + Obj.iAddShield;
			}

			Obj.iFillShieldCount = 2;
			Obj.iFillShield = iShieldGage;
			Obj.iFillShieldMax = iShieldGage;

			GCSendEffectInfo(Obj, 3);

			if (!gObjSearchItemMinus(&Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}
		}
		else if (lpItem->m_Type == ITEMGET(14, 35))
		{
			int iShieldGage = (Obj.iMaxShield + Obj.iAddShield) * 25 / 100;	// #formula

			if (Obj.iShield < 0)
				Obj.iShield = 0;

			if (Obj.iFillShield > 0)
			{
				Obj.iShield += Obj.iFillShield;

				if (Obj.iShield > (Obj.iMaxShield + Obj.iAddShield))
					Obj.iShield = Obj.iMaxShield + Obj.iAddShield;
			}

			Obj.iFillShieldCount = 2;
			Obj.iFillShield = iShieldGage;
			Obj.iFillShieldMax = iShieldGage;

			GCSendEffectInfo(Obj, 3);

			if (!gObjSearchItemMinus(&Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}
		}
		else if (lpItem->m_Type == ITEMGET(14, 37))
		{
			int iShieldGage = (Obj.iMaxShield + Obj.iAddShield) * 45 / 100; // #formula

			if (Obj.iShield < 0)
				Obj.iShield = 0;

			if (Obj.iFillShield > 0)
			{
				Obj.iShield += Obj.iFillShield;

				if (Obj.iShield > (Obj.iMaxShield + Obj.iAddShield))
					Obj.iShield = Obj.iMaxShield + Obj.iAddShield;
			}

			Obj.iFillShieldCount = 2;
			Obj.iFillShield = iShieldGage;
			Obj.iFillShieldMax = iShieldGage;

			GCSendEffectInfo(Obj, 3);

			if (!gObjSearchItemMinus(Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}
		}
		else if (lpItem->m_Type == ITEMGET(14, 38))	// Small Compound Potion 
		{
			int iShieldGage = (Obj.iMaxShield + Obj.iAddShield) * 5 / 100;	// #formula
			int iHPGage = (int)((Obj.MaxLife + Obj.AddLife) * 10.0f / 100.0f);	// #formula

			if (Obj.iShield < 0)
				Obj.iShield = 0;

			if (Obj.FillLife > 0.0f)
			{
				Obj.Life += Obj.FillLife;

				if (Obj.Life > (Obj.MaxLife + Obj.AddLife))
					Obj.Life = Obj.MaxLife + Obj.AddLife;
			}

			if (Obj.iFillShield > 0)
			{
				Obj.iShield += Obj.iFillShield;

				if (Obj.iShield > (Obj.iMaxShield + Obj.iAddShield))
					Obj.iShield = Obj.iMaxShield + Obj.iAddShield;
			}

			Obj.FillLife = (float)iHPGage;
			Obj.FillLifeMax = (float)iHPGage;
			Obj.FillLifeCount = 4;
			Obj.iFillShield = iShieldGage;
			Obj.iFillShieldMax = iShieldGage;
			Obj.iFillShieldCount = 4;

			GCSendEffectInfo(Obj, 3);

			if (!gObjSearchItemMinus(Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}

			sLog->outBasic("[%s][%s]Use Compound Potion Lv1 - SD[%d] HP[%f] -> SD[%d] HP[%f]",
				Obj.AccountID, Obj.Name, Obj.iShield, Obj.Life,
				Obj.iShield + iShieldGage, (double)(Obj.Life + (float)iHPGage));
		}
		else if (lpItem->m_Type == ITEMGET(14, 39))	// Compound Potion 
		{
			int iShieldGage = (Obj.iMaxShield + Obj.iAddShield) * 10 / 100;	// #formula
			int iHPGage = (int)((Obj.MaxLife + Obj.AddLife) * 25.0f / 100.0f);	// #formula

			if (Obj.iShield < 0)
				Obj.iShield = 0;

			if (Obj.FillLife > 0.0f)
			{
				Obj.Life += Obj.FillLife;

				if (Obj.Life > (Obj.MaxLife + Obj.AddLife))
					Obj.Life = Obj.MaxLife + Obj.AddLife;
			}

			if (Obj.iFillShield > 0)
			{
				Obj.iShield += Obj.iFillShield;

				if (Obj.iShield > (Obj.iMaxShield + Obj.iAddShield))
					Obj.iShield = Obj.iMaxShield + Obj.iAddShield;
			}

			Obj.FillLife = (float)iHPGage;
			Obj.FillLifeMax = (float)iHPGage;
			Obj.FillLifeCount = 4;
			Obj.iFillShield = iShieldGage;
			Obj.iFillShieldMax = iShieldGage;
			Obj.iFillShieldCount = 4;

			GCSendEffectInfo(Obj, 3);

			if (!gObjSearchItemMinus(Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}

			sLog->outBasic("[%s][%s]Use Compound Potion Lv2 - SD[%d] HP[%f] -> SD[%d] HP[%f]",
				Obj.AccountID, Obj.Name, Obj.iShield, Obj.Life,
				Obj.iShield + iShieldGage, (double)(Obj.Life + (float)iHPGage));
		}
		else if (lpItem->m_Type == ITEMGET(14, 40))	// Large Compound Potion 
		{
			int iShieldGage = (Obj.iMaxShield + Obj.iAddShield) * 20 / 100;	// #formula
			int iHPGage = (int)((Obj.MaxLife + Obj.AddLife) * 45.0f / 100.0f);	// #formula

			if (Obj.iShield < 0)
				Obj.iShield = 0;

			if (Obj.FillLife > 0.0f)
			{
				Obj.Life += Obj.FillLife;

				if (Obj.Life > (Obj.MaxLife + Obj.AddLife))
					Obj.Life = Obj.MaxLife + Obj.AddLife;
			}

			if (Obj.iFillShield > 0)
			{
				Obj.iShield += Obj.iFillShield;

				if (Obj.iShield > (Obj.iMaxShield + Obj.iAddShield))
					Obj.iShield = Obj.iMaxShield + Obj.iAddShield;
			}

			Obj.FillLife = (float)iHPGage;
			Obj.FillLifeMax = (float)iHPGage;
			Obj.FillLifeCount = 4;
			Obj.iFillShield = iShieldGage;
			Obj.iFillShieldMax = iShieldGage;
			Obj.iFillShieldCount = 4;

			GCSendEffectInfo(Obj, 3);

			if (!gObjSearchItemMinus(Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}

			sLog->outBasic("[%s][%s]Use Compound Potion Lv3 - SD[%d] HP[%f] -> SD[%d] HP[%f]",
				Obj.AccountID, Obj.Name,
				Obj.iShield, Obj.Life,
				Obj.iShield + iShieldGage, (double)(Obj.Life + (float)iHPGage));
		}
		else if (lpItem->m_Type == ITEMGET(14, 8)) // Antidote
		{
			if (!gObjSearchItemMinus(Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}

			if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_POISON) == TRUE)
			{
				gObjRemoveBuffEffect(Obj, BUFFTYPE_POISON);
				Obj.lpAttackObj = 0;
				GCMagicCancelSend(Obj, 1);
			}

			if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_FREEZE) == TRUE)
			{
				gObjRemoveBuffEffect(Obj, BUFFTYPE_FREEZE);
				Obj.DelayActionTime = 0;
				Obj.DelayLevel = 0;
				Obj.lpAttackObj = 0;
				GCMagicCancelSend(Obj, 7);
			}
		}
		else if (lpItem->m_Type == ITEMGET(14, 46))
		{
			if (!gObjSearchItemMinus(Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}

			gObjAddBuffEffect(&Obj, BUFFTYPE_HALLOWEEN_BLESS);
		}
		else if (lpItem->m_Type == ITEMGET(14, 47))
		{
			if (!gObjSearchItemMinus(Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}

			gObjAddBuffEffect(Obj, BUFFTYPE_HALLOWEEN_ANGER);
		}
		else if (lpItem->m_Type == ITEMGET(14, 48))
		{
			if (!gObjSearchItemMinus(Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}

			gObjAddBuffEffect(Obj, BUFFTYPE_HALLOWEEN_CRY);
		}
		else if (lpItem->m_Type == ITEMGET(14, 49))
		{
			if (!gObjSearchItemMinus(Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}

			gObjAddBuffEffect(&Obj, BUFFTYPE_HALLOWEEN_FOOD);
		}
		else if (lpItem->m_Type == ITEMGET(14, 50))
		{
			if (!gObjSearchItemMinus(Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}

			gObjAddBuffEffect(&Obj, BUFFTYPE_HALLOWEEN_DRINK);
		}
		else if (lpItem->m_Type == ITEMGET(14, 85))
		{
			if (!gObjSearchItemMinus(Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}

			gObjAddBuffEffect(&Obj, BUFFTYPE_CHERRYBLOSSOM_DRINK);
		}
		else if (lpItem->m_Type == ITEMGET(14, 86))
		{
			if (!gObjSearchItemMinus(Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}

			gObjAddBuffEffect(&Obj, BUFFTYPE_CHERRYBLOSSOM_DUMPLING);
		}
		else if (lpItem->m_Type == ITEMGET(14, 87))
		{
			if (!gObjSearchItemMinus(Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}

			gObjAddBuffEffect(&Obj, BUFFTYPE_CHERRYBLOSSOM_PETAL);
		}
		else if (lpItem->m_Type == ITEMGET(13, 81))
		{
			if (!gObjSearchItemMinus(Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}

			gObjAddBuffEffect(Obj, BUFFTYPE_CHARM_GUARDIAN);
		}
		else if (lpItem->m_Type == ITEMGET(13, 82))
		{
			if (!gObjSearchItemMinus(Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}

			gObjAddBuffEffect(Obj, BUFFTYPE_CHARM_PROTECTITEM);
		}

		else if (lpItem->m_Type == ITEMGET(13, 43))
		{
			if (!gObjSearchItemMinus(Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}

			gObjAddBuffEffect(Obj, BUFFTYPE_PCS_MARK1);
		}

		else if (lpItem->m_Type == ITEMGET(13, 44))
		{
			if (!gObjSearchItemMinus(&Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}

			gObjAddBuffEffect(Obj, BUFFTYPE_PCS_MARK2);
		}

		else if (lpItem->m_Type == ITEMGET(13, 45))
		{
			if (!gObjSearchItemMinus(Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}

			gObjAddBuffEffect(Obj, BUFFTYPE_PCS_MARK3);
		}
		else if (g_kJewelOfHarmonySystem.IsJewelOfHarmonySmeltingItems(lpItem->m_Type) == TRUE)
		{
			if (g_kJewelOfHarmonySystem.SmeltItemBySmeltingStone(Obj, lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE)
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemOneSend(Obj, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}
			else
			{
				GCReFillSend(Obj, Obj.Life, 0xFD, 1, Obj.iShield);
			}
		}
		else if (lpItem->m_Type == ITEMGET(12, 30))
		{
			if (UseBundleOfBlessJewel(Obj, lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE)
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemOneSend(Obj, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}
		}

#if( ENABLETEST_ITEMUPGRADEJEWELS == 1 )
		else if (g_ItemUpgradeJewels.IsJewel(lpItem->m_Type))
		{
			g_ItemUpgradeJewels.ProcInsert(Obj, lpMsg->inventoryPos, lpMsg->invenrotyTarget);
		}
#else
		else if (g_kJewelOfHarmonySystem.IsJewelOfHarmonyPurity(lpItem->m_Type) == TRUE && gSocketItemType.CheckSocketItemType(&Obj.pntInventory[lpMsg->invenrotyTarget]) == FALSE) //season4 add-on)
		{
			if (g_kJewelOfHarmonySystem.StrengthenItemByJewelOfHarmony(&Obj, lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE)
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemOneSend(Obj, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}
			else
			{
				GCReFillSend(Obj, Obj.Life, 0xFD, 1, Obj.iShield);
			}
		}
		else if (lpItem->m_Type == ITEMGET(14, 13)) // Jewel Of Bless
		{
			if (gObjItemLevelUp(&Obj, lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE)
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemOneSend(Obj, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}
		}
		else if (lpItem->m_Type == ITEMGET(14, 14)) // Jewel Of Soul
		{
			if (gObjItemRandomLevelUp(&Obj, lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE)
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemOneSend(Obj, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}
		}
		else if (lpItem->m_Type == ITEMGET(14, 16)) // Jewel Of Life
		{
			if (gObjItemRandomOption3Up(&Obj, lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE)
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemOneSend(Obj, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}
			else
			{
				GCReFillSend(Obj, Obj.Life, 0xFD, 1, Obj.iShield);
			}
		}
#endif
		else if (lpItem->m_Type == ITEMGET(14, 160))	//1.01.00 update
		{
			if (g_LuckyItemManager.LuckyItemRepaire(Obj, lpMsg->inventoryPos, lpMsg->invenrotyTarget))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemOneSend(Obj, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}
			else
			{
				GCReFillSend(Obj, Obj.Life, 0xFD, 1, Obj.iShield);
			}
		}

		else if (lpItem->m_Type == ITEMGET(14, 161))	//1.01.00 update
		{
			if (g_kJewelOfHarmonySystem.StrengthenItemByJewelOfRise(Obj, lpMsg->inventoryPos, lpMsg->invenrotyTarget))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemOneSend(Obj, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}
			else
			{
				GCReFillSend(Obj, Obj.Life, 0xFD, 1, Obj.iShield);
			}
		}
		else if (lpItem->m_Type == ITEMGET(14, 209)) // Tradeable Seal Season 8
		{
			if (g_PentagramSystem.AddTradeCount(&Obj, lpMsg->inventoryPos, lpMsg->invenrotyTarget) == TRUE)
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemOneSend(Obj, lpMsg->invenrotyTarget);
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}

			else
			{
				GCReFillSend(Obj, Obj.Life, 0xFD, 1, Obj.iShield);
			}
		}

		else if (lpItem->m_Type == ITEMGET(14, 224))
		{
			if (!gObjSearchItemMinus(Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}

			gObjAddBuffEffect(&Obj, BUFFTYPE_LIGHT_BLESSING);
			//[K2]Light
		}

		else if (lpItem->m_Type == ITEMGET(14, 263))
		{
			if (!gObjSearchItemMinus(Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
				//	gObjInventoryDeleteItem(Obj, pos);

			}
			gObjAddBuffEffect(&Obj, BUFFTYPE_LIGHT_BLESSING_LOW);
		}

		else if (lpItem->m_Type == ITEMGET(14, 264))
		{
			if (!gObjSearchItemMinus(Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}

			gObjAddBuffEffect(&Obj, BUFFTYPE_LIGHT_BLESSING_MIDDLE);
		}

		else if (lpItem->m_Type >= ITEMGET(15, 0) ||	// Group 15 - Scrolls
			lpItem->m_Type == ITEMGET(12, 7) || // Orb of Twisting Slash
			lpItem->m_Type == ITEMGET(12, 289) || // 
			lpItem->m_Type == ITEMGET(12, 290) || // 
			lpItem->m_Type == ITEMGET(12, 291) || // 
			lpItem->m_Type == ITEMGET(12, 292) || // 
			lpItem->m_Type == ITEMGET(12, 293) || // 
			lpItem->m_Type == ITEMGET(12, 295) || // 
			lpItem->m_Type == ITEMGET(12, 296) || // 
			lpItem->m_Type == ITEMGET(12, 297) || // 
			lpItem->m_Type == ITEMGET(12, 298) || // 
			lpItem->m_Type == ITEMGET(12, 299) || // 
			lpItem->m_Type == ITEMGET(12, 300) || // 
			lpItem->m_Type == ITEMGET(12, 303) || // 
			lpItem->m_Type == ITEMGET(12, 305) || // 
			(lpItem->m_Type >= ITEMGET(12, 8) && lpItem->m_Type <= ITEMGET(12, 24)) ||	// Orbs
			lpItem->m_Type == ITEMGET(12, 35) ||
			(lpItem->m_Type >= ITEMGET(12, 44) && lpItem->m_Type <= ITEMGET(12, 48)) ||	// Scroll of Fire Scream
			(lpItem->m_Type >= ITEMGET(12, 271) && lpItem->m_Type <= ITEMGET(12, 277))) // Grow Lancer Skills
		{

			if ((Obj.m_PlayerData->Strength + Obj.AddStrength) < lpItem->m_RequireStrength)
			{
				GCReFillSend(Obj, Obj.Life, 0xFD, 1, Obj.iShield);
				return;
			}

			if ((Obj.m_PlayerData->Dexterity + Obj.AddDexterity) < lpItem->m_RequireDexterity)
			{
				GCReFillSend(Obj, Obj.Life, 0xFD, 1, Obj.iShield);
				return;
			}

			if (lpItem->m_Type == ITEMGET(15, 18))	// Scroll of HellBurst
			{
				if (g_QuestInfo.GetQuestState(Obj, 2) != 2)
				{
					GCReFillSend(Obj, Obj.Life, 0xFD, 1, Obj.iShield);
					return;
				}
			}

			if (lpItem->m_Type >= ITEMGET(12, 8) && lpItem->m_Type <= ITEMGET(12, 24))
			{
				if (Obj.Level < lpItem->m_RequireLevel)	// Orbs
				{
					GCReFillSend(Obj, Obj.Life, 0xFD, 1, Obj.iShield);
					return;
				}
			}

			WORD skillnumber;

			if (lpItem->IsClass(Obj.Class, Obj.m_PlayerData->ChangeUP) == FALSE)
			{
				GCInventoryItemDeleteSend(Obj, -1, 1);

			}
			else
			{
				int addskill = gObjMagicAdd(&Obj, ITEM_GET_TYPE(lpItem->m_Type), ITEM_GET_INDEX(lpItem->m_Type), lpItem->m_Level, skillnumber);

				if (addskill >= 0)
				{
					gObjInventoryItemSet(Obj, pos, -1);
					GCMagicListOneSend(Obj, addskill, skillnumber, lpItem->m_Level, 0, 0);
					Obj.pntInventory[pos]->Clear();
					GCInventoryItemDeleteSend(Obj, pos, 1);
				}
				else
				{
					GCInventoryItemDeleteSend(Obj, -1, 1);
				}
			}
		}
		else if (lpItem->m_Type == ITEMGET(14, 10)) // Town Portal Scroll
		{
			if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
			{
				sLog->outBasic("[UBF][Return Scroll] [%s][%s][%s] - No Available",
					Obj.AccountID, Obj.Name, Obj.m_PlayerData->m_RealNameOfUBF);

				return;
			}

			sLog->outBasic("[Using Item] [Return Scroll] [%s][%s] - Current Map:[%d]",
				Obj.AccountID, Obj.Name, Obj.MapNumber);

			if (BC_MAP_RANGE(Obj.MapNumber))
			{
				int iBridgeIndex = g_BloodCastle.GetBridgeIndex(Obj.MapNumber);
				if (g_BloodCastle.GetCurrentState(iBridgeIndex) == 2)
				{
					g_BloodCastle.SearchUserDropQuestItem(Obj);
				}
				else
				{
					g_BloodCastle.SearchUserDeleteQuestItem(Obj);
				}
			}

			if (IT_MAP_RANGE(Obj.MapNumber))
			{
				g_IT_Event.DropRelicsItem(Obj.MapNumber, Obj);
			}

			if (Obj.m_IfState->use && Obj.m_IfState->type == 3)
			{
				Obj.TargetNpcNumber = -1;
				Obj.m_IfState->type = 0;
				Obj.m_IfState->use = 0;
			}

			gObjInventoryItemSet(Obj, pos, -1);
			Obj.pntInventory[pos]->Clear();
			GCInventoryItemDeleteSend(Obj, pos, 1);

			if (Obj.MapNumber == MAP_INDEX_DEVIAS)
			{
				gObjMoveGate(Obj, 22);
			}
			else if (Obj.MapNumber == MAP_INDEX_NORIA)
			{
				gObjMoveGate(Obj, 27);
			}
			else if (Obj.MapNumber == MAP_INDEX_ELBELAND)
			{
				gObjMoveGate(Obj, 267);
			}
			else if (Obj.MapNumber == MAP_INDEX_LOSTTOWER)
			{
				gObjMoveGate(Obj, 42);
			}
			else if (Obj.MapNumber == MAP_INDEX_ATHLANSE)
			{
				gObjMoveGate(Obj, 49);
			}
			else if (Obj.MapNumber == MAP_INDEX_TARKAN)
			{
				gObjMoveGate(Obj, 57);
			}
			else if (BC_MAP_RANGE(Obj.MapNumber) != FALSE)
			{
				gObjMoveGate(Obj, 22);
			}
			else if (CC_MAP_RANGE(Obj.MapNumber) != FALSE)
			{
				gObjMoveGate(Obj, 22);
			}
			else if (Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
			{
				gObjMoveGate(Obj, 333);
			}
			else if (KALIMA_MAP_RANGE(Obj.MapNumber) != FALSE)
			{
				gObjMoveGate(Obj, 22);
			}
			else if (IT_MAP_RANGE(Obj.MapNumber) != FALSE)
			{
				gObjMoveGate(Obj, 267);
			}
			else if (Obj.MapNumber == MAP_INDEX_AIDA)
			{
				gObjMoveGate(Obj, 27);
			}
			else if (Obj.MapNumber == MAP_INDEX_CRYWOLF_FIRSTZONE)
			{
				gObjMoveGate(Obj, 27);
			}
			else if (Obj.MapNumber == MAP_INDEX_BARRACKS)
			{
				gObjMoveGate(Obj, 114);
			}
			else if (Obj.MapNumber == MAP_INDEX_REFUGEE)
			{
				gObjMoveGate(Obj, 114);
			}
			else if (Obj.MapNumber == MAP_INDEX_CALMNESS)
			{
				gObjMoveGate(Obj, 273);
			}
			else if (Obj.MapNumber == MAP_INDEX_RAKLION || Obj.MapNumber == MAP_INDEX_HATCHERY)
			{
				gObjMoveGate(Obj, 287);
			}
			else if (Obj.MapNumber == MAP_INDEX_SANTAVILLAGE)
			{
				gObjMoveGate(Obj, 22);
			}
			else if (DG_MAP_RANGE(Obj.MapNumber) != FALSE)
			{
				gObjMoveGate(Obj, 267);
			}
			else if (IMPERIAL_MAP_RANGE(Obj.MapNumber) != FALSE)
			{
				gObjMoveGate(Obj, 22);
			}
			else if (Obj.MapNumber == MAP_INDEX_LORENMARKET)
			{
				gObjMoveGate(Obj, 333);
			}
			else if (Obj.MapNumber == MAP_INDEX_FEREA)
			{
				gObjMoveGate(Obj, 509);
			}
			else if (Obj.MapNumber == MAP_INDEX_NIXIE_LAKE)
			{
				gObjMoveGate(Obj, 522);
			}

			else if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE && Obj.MapNumber == MAP_INDEX_CASTLESIEGE)
			{
				if (g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					if (Obj.m_btCsJoinSide == 1)
					{
						gObjMoveGate(Obj, 101);
					}
					else
					{
						gObjMoveGate(Obj, 100);
					}
				}
				else
				{
					if (g_CastleSiege.CheckCastleOwnerMember(Obj) == TRUE || g_CastleSiege.CheckCastleOwnerUnionMember(Obj) == TRUE)
					{
						gObjMoveGate(Obj, 101);
					}
					else
					{
						gObjMoveGate(Obj, 100);
					}
				}
			}

			else if (g_NewPVP.IsVulcanusMap(Obj.MapNumber))
			{
				gObjMoveGate(Obj, 294);
			}

			else if (g_NewPVP.IsPKFieldMap(Obj.MapNumber))
			{
				g_NewPVP.Reset(Obj);
				gObjMoveGate(Obj, 17);
			}

			else if (Obj.MapNumber == MAP_INDEX_ARENA)
			{
			}

			else
			{
				gObjMoveGate(Obj, 17);
			}

		}
		else if (lpItem->m_Type == ITEMGET(13, 66))
		{
			if (DG_MAP_RANGE(Obj.MapNumber) == TRUE)
			{
				g_DoppelGanger.LeaveDoppelganger(Obj);

				if (g_DoppelGanger.GetDoppelgangerState() == 2)
				{
					g_DoppelGanger.SendDoppelgangerResult(&Obj, 1);
				}
			}

			gObjTeleport(Obj, MAP_INDEX_SANTAVILLAGE, 220, 20);

			if (!gObjSearchItemMinus(Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}
		}

		else if (lpItem->m_Type == ITEMGET(14, 162))
		{
			if (Obj.m_PlayerData->m_InventoryExpansion < 2)
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
				Obj.m_PlayerData->m_InventoryExpansion++;
				GDSetExpWare(Obj, 1, Obj.m_PlayerData->m_InventoryExpansion);

				PMSG_USEEXPANSIONITEM pMsg;
				PHeadSetB((BYTE*)&pMsg, 0x2B, sizeof(pMsg));
				pMsg.Result = 1;
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
			}
		}

		else if (lpItem->m_Type == ITEMGET(14, 163))
		{
			if (Obj.m_PlayerData->m_WarehouseExpansion < 1)
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
				GDSetExpWare(Obj, 2, 1);
				Obj.m_PlayerData->m_WarehouseExpansion = 1;

				PMSG_USEEXPANSIONITEM pMsg;
				PHeadSetB((BYTE*)&pMsg, 0x2B, sizeof(pMsg));
				pMsg.Result = 1;
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
			}
		}

		else if (lpItem->m_Type == ITEMGET(13, 152) || lpItem->m_Type == ITEMGET(13, 156))
		{
			if (g_MasterLevelSkillTreeSystem.CheckMasterSkillPoint(Obj, 1) == TRUE)
			{
				g_MasterLevelSkillTreeSystem.ResetMasterSkill(Obj, 1);
				sLog->outBasic("[MasterLevelSkill Reset] - Passive (%d)", Obj);
				gObjCloseSet(Obj, 1);
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}

			else
			{
				this->GCServerMsgStringSend(Lang.GetText(0, 526), Obj, 1);
			}
		}

		else if (lpItem->m_Type == ITEMGET(13, 153) || lpItem->m_Type == ITEMGET(13, 157))
		{
			if (g_MasterLevelSkillTreeSystem.CheckMasterSkillPoint(Obj, 2) == TRUE)
			{
				g_MasterLevelSkillTreeSystem.ResetMasterSkill(Obj, 2);
				sLog->outBasic("[MasterLevelSkill Reset] - Strengthen Skill (%d)", Obj);
				gObjCloseSet(Obj, 1);
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}

			else
			{
				this->GCServerMsgStringSend(Lang.GetText(0, 526), Obj, 1);
			}
		}

		else if (lpItem->m_Type == ITEMGET(13, 154) || lpItem->m_Type == ITEMGET(13, 158))
		{
			if (g_MasterLevelSkillTreeSystem.CheckMasterSkillPoint(Obj, 3) == TRUE)
			{
				g_MasterLevelSkillTreeSystem.ResetMasterSkill(Obj, 3);
				sLog->outBasic("[MasterLevelSkill Reset] - Enhance Attack/Defense (%d)", Obj);
				gObjCloseSet(Obj, 1);
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}

			else
			{
				this->GCServerMsgStringSend(Lang.GetText(0, 526), Obj, 1);
			}
		}

		else if (lpItem->m_Type == ITEMGET(13, 155) || lpItem->m_Type == ITEMGET(13, 159))
		{
			if (g_MasterLevelSkillTreeSystem.CheckMasterSkillPoint(Obj, 0) == TRUE)
			{
				g_MasterLevelSkillTreeSystem.ResetMasterSkill(Obj, 0);
				sLog->outBasic("[MasterLevelSkill Reset] - All (%d)", Obj);
				gObjCloseSet(Obj, 1);
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}

			else
			{
				this->GCServerMsgStringSend(Lang.GetText(0, 526), Obj, 1);
			}
		}

		else if (lpItem->m_Type == ITEMGET(14, 9)) // Ale
		{
			int level = lpItem->m_Level;

			gObjInventoryItemSet(Obj, pos, -1);
			Obj.pntInventory[pos]->Clear();
			GCInventoryItemDeleteSend(Obj, pos, 1);
			gObjUseDrink(Obj, level);
		}
		else if (lpItem->m_Type == ITEMGET(14, 20)) // Remedy of Love
		{
			if (lpItem->m_Level == 0)
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
				gObjUseDrink(Obj, 2);
			}
		}
		else if (lpItem->m_Type == ITEMGET(13, 15)) // Fruits
		{
			if (iItemUseType == 0)
			{
				gObjUsePlusStatFruit(Obj, pos);
			}
			else if (iItemUseType == 1)
			{
				gObjUseMinusStatFruit(Obj, pos);
			}
		}
		else if (lpItem->m_Type == ITEMGET(13, 70))
		{
			if (Obj.SaveMapNumber != -1)
			{
				bool bMove = gMoveCommand.CheckEquipmentToMove(Obj, Obj.SaveMapNumber);

				if (bMove == true && MapNumberCheck(Obj.SaveMapNumber))
				{
					gObjTeleport(Obj, Obj.SaveMapNumber, Obj.SaveX, Obj.SaveY);
					gObjInventoryItemSet(Obj, pos, -1);
					Obj.pntInventory[pos]->Clear();
					GCInventoryItemDeleteSend(Obj, pos, 1);
					Obj.SaveMapNumber = -1;
					GCServerCmd(Obj, 61, 0, 0);
				}
			}
			else
			{
				if (gObjSearchItemMinus(Obj, pos, 1))
				{
					Obj.SaveMapNumber = Obj.MapNumber;
					Obj.SaveX = Obj.X;
					Obj.SaveY = Obj.Y;
				}
				else
				{
					gObjInventoryItemSet(Obj, pos, -1);
					Obj.pntInventory[pos]->Clear();
					GCInventoryItemDeleteSend(Obj, pos, 1);
				}
			}
		}
		else if (lpItem->m_Type == ITEMGET(13, 69))
		{
			if (Obj.DieMapNumber != -1)
			{
				bool bMove = gMoveCommand.CheckEquipmentToMove(Obj, Obj.DieMapNumber);

				bool bGens = g_GensSystem.IsMapBattleZone(Obj.MapNumber);

				if (bMove == true && bGens == false && MapNumberCheck(Obj.DieMapNumber))
				{
					gObjTeleport(Obj, Obj.DieMapNumber, Obj.DieX, Obj.DieY);
					gObjInventoryItemSet(Obj, pos, -1);
					Obj.pntInventory[pos]->Clear();
					GCInventoryItemDeleteSend(Obj, pos, 1);
					Obj.DieMapNumber = -1;
					GCServerCmd(Obj, 60, 0, 0);
				}
			}
		}
		else if (lpItem->m_Type >= ITEMGET(13, 54) && lpItem->m_Type <= ITEMGET(13, 58))
		{
			CashShopExMinusStatFruit(Obj, pos);
		}
		else if (lpItem->m_Type == ITEMGET(14, 70)) // CSHOP HP Potions
		{
			int tLife = (lpItem->m_Value * 10) - (Obj.Level);	// #formula

			if (tLife < 0)
			{
				tLife = 0;
			}

			tLife += ((int)(Obj.MaxLife + Obj.AddLife));	// #formula
			Obj.Life += tLife;

			if (Obj.Life > (Obj.MaxLife + Obj.AddLife - 1.0f))
				Obj.Life = Obj.MaxLife + Obj.AddLife;

			GCReFillSend(Obj, Obj.Life, 0xFD, 1, Obj.iShield);

			Obj.FillLifeMax = tLife;
			Obj.FillLife = tLife;

			if (!gObjSearchItemMinus(&Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}
		}
		else if (lpItem->m_Type == ITEMGET(14, 71)) // CSHOP MP Potions
		{
			int tMana = (lpItem->m_Value * 10) - (Obj.Level);	// #formula

			if (tMana < 0)
			{
				tMana = 0;
			}

			tMana += ((int)(Obj.MaxMana + Obj.AddMana));
			Obj.Mana += tMana;

			if (Obj.Mana > (Obj.MaxMana + Obj.AddMana - 1.0f))
				Obj.Mana = Obj.MaxMana + Obj.AddMana;

			GCManaSend(Obj, Obj.Mana, 0xFF, 0, Obj.BP);

			if (!gObjSearchItemMinus(&Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}
		}
		else if (lpItem->m_Type == ITEMGET(14, 133))
		{
			gObjUseRecoveryPotion(Obj, pos, (double)0.65);
		}
		else if (lpItem->m_Type == ITEMGET(14, 78)) // CSHOP Arcanum For?a
		{
			LPITEMEFFECT lpEffect = g_ItemAddOption.GetItemEffectData(lpItem->m_Type);

			if (lpEffect == NULL)
			{
				GCReFillSend(Obj, Obj.Life, 0xFD, 1, Obj.iShield);
				return;
			}

			g_PeriodItemEx.SetPeriodItemInfo(Obj, lpItem->m_Type, 0, lpEffect->iEffectValidTime);
			if (!gObjSearchItemMinus(Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}
		}
		else if (lpItem->m_Type == ITEMGET(14, 79)) // CSHOP Arcanum Agilidade
		{
			LPITEMEFFECT lpEffect = g_ItemAddOption.GetItemEffectData(lpItem->m_Type);

			if (lpEffect == NULL)
			{
				GCReFillSend(Obj, Obj.Life, 0xFD, 1, Obj.iShield);
				return;
			}

			g_PeriodItemEx.SetPeriodItemInfo(Obj, lpItem->m_Type, 0, lpEffect->iEffectValidTime);
			if (!gObjSearchItemMinus(Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}
		}
		else if (lpItem->m_Type == ITEMGET(14, 80)) // CSHOP Arcanum Vitalidade
		{
			LPITEMEFFECT lpEffect = g_ItemAddOption.GetItemEffectData(lpItem->m_Type);

			if (lpEffect == NULL)
			{
				GCReFillSend(Obj, Obj.Life, 0xFD, 1, Obj.iShield);
				return;
			}

			g_PeriodItemEx.SetPeriodItemInfo(Obj, lpItem->m_Type, 0, lpEffect->iEffectValidTime);
			if (!gObjSearchItemMinus(Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}
		}
		else if (lpItem->m_Type == ITEMGET(14, 81)) // CSHOP Arcanum Energia
		{
			LPITEMEFFECT lpEffect = g_ItemAddOption.GetItemEffectData(lpItem->m_Type);

			if (lpEffect == NULL)
			{
				GCReFillSend(Obj, Obj.Life, 0xFD, 1, Obj.iShield);
				return;
			}

			g_PeriodItemEx.SetPeriodItemInfo(Obj, lpItem->m_Type, 0, lpEffect->iEffectValidTime);
			if (!gObjSearchItemMinus(Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}
		}
		else if (lpItem->m_Type == ITEMGET(14, 82)) // CSHOP Arcanum Comando
		{
			LPITEMEFFECT lpEffect = g_ItemAddOption.GetItemEffectData(lpItem->m_Type);

			if (lpEffect == NULL)
			{
				GCReFillSend(Obj, Obj.Life, 0xFD, 1, Obj.iShield);
				return;
			}

			g_PeriodItemEx.SetPeriodItemInfo(Obj, lpItem->m_Type, 0, lpEffect->iEffectValidTime);
			if (!gObjSearchItemMinus(Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}
		}
		else if (lpItem->m_Type == ITEMGET(13, 60)) // CSHOP PK Free Ticket
		{
			if (Obj.m_PK_Level == 6)
			{
				Obj.m_PK_Level = 5;
			}
			else if (Obj.m_PK_Level == 5)
			{
				Obj.m_PK_Level = 4;
			}
			else if (Obj.m_PK_Level == 4)
			{
				Obj.m_PK_Level = 3;
			}
			else if (Obj.m_PK_Level <= 3)
			{
				Obj.m_PK_Level = Obj.m_PK_Level;
			}

			GCPkLevelSend(Obj, Obj.m_PK_Level);

			if (!gObjSearchItemMinus(Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}
			sLog->outBasic("[PCS] [%s][%s] Use PK Clear Item", Obj.AccountID, Obj.Name);
		}
		else if (lpItem->m_Type == ITEMGET(14, 7))	// Siege Potion
		{
			if (!gObjSearchItemMinus(Obj, pos, 1))
			{
				gObjInventoryItemSet(Obj, pos, -1);
				Obj.pntInventory[pos]->Clear();
				GCInventoryItemDeleteSend(Obj, pos, 1);
			}

			int iLevel = lpItem->m_Level;
			int iSerial = lpItem->GetNumber();
			int iDur = lpItem->m_Durability;

			if (Obj.m_PlayerData->lpGuild)
			{
				sLog->outBasic("[CastleSiege] Using CastleSiege Potion Lv[%d]/Si[%d]/Dur[%d] - [%s][%s] - (Guild : %s)",
					iLevel, iSerial, iDur, Obj.AccountID, Obj.Name, Obj.m_PlayerData->lpGuild->Name);
			}
			else
			{
				sLog->outBasic("[CastleSiege] Using CastleSiege Potion Lv[%d]/Si[%d]/Dur[%d] - [%s][%s]",
					iLevel, iSerial, iDur, Obj.AccountID, Obj.Name);
			}

			if (iLevel == 0)
			{
				gObjUseBlessAndSoulPotion(Obj, iLevel);
			}
			else if (iLevel == 1)
			{
				gObjUseBlessAndSoulPotion(Obj, iLevel);
			}
		}
		else if (lpItem->m_Type == ITEMGET(13, 48))
		{
			if ((Obj.MapNumber < MAP_INDEX_KALIMA1 || Obj.MapNumber > MAP_INDEX_KALIMA6) && Obj.MapNumber != MAP_INDEX_KALIMA7)
			{
				int iLevel = g_KalimaGate.GetKalimaGateLevel2(Obj);
				int iSerial = lpItem->GetNumber();
				int iDuration = lpItem->m_Durability;
				BOOL bKalimaGateCreateResult = FALSE;
				int iKalimaGateX = 0;
				int iKalimaGateY = 0;

				iLevel++;
				iKalimaGateX = Obj.X + rand() % 6 - 2;
				iKalimaGateY = Obj.Y + rand() % 6 - 2;

				bKalimaGateCreateResult = g_KalimaGate.CreateKalimaGate(Obj, iLevel, iKalimaGateX, iKalimaGateY);

				if (bKalimaGateCreateResult == TRUE)
				{
					if (!gObjSearchItemMinus(Obj, pos, 1))
					{
						gObjInventoryItemSet(Obj, pos, -1);
						Obj.pntInventory[pos]->Clear();
						GCInventoryItemDeleteSend(Obj, pos, 1);
					}

					sLog->outBasic("[PCS] Use Free Kalima Ticket Success");
				}
				else
				{
					sLog->outBasic("[PCS] Use Free Kalima Ticket Failed");
				}
			}
		}

		else if (g_QuestExpManager.IsQuestItemAtt(lpItem->m_Type, 64) == true)
		{
			int Ep = g_QuestExpManager.GetQuestItemEp(lpItem->m_Type);
			int iResult = 0;
			lua_State *L = g_MuLuaQuestExp.GetLua();

			if (!L)
			{
				sLog->outBasic("[QuestExp] - Error - [%s] [%d]", __FILE__, __LINE__);
			}

			else
			{
				if (Ep)
				{
					g_Generic_Call(L, "ItemUseQuest", "ii>i", Obj, Ep, &iResult);

					if (iResult)
					{
						gObjInventoryItemSet(Obj, pos, -1);
						Obj.pntInventory[pos]->Clear();
						GCInventoryItemDeleteSend(Obj, pos, 1);
					}
				}
			}
		}
		else
		{
			sLog->outBasic("error-L3 : %s return %s %d %d %s",
				Obj.Name, __FILE__, __LINE__, pos, Obj.pntInventory[pos]->GetName());

			GCReFillSend(Obj, Obj.Life, 0xFD, 1, Obj.iShield);
		}
	}
	else
	{
		GCReFillSend(Obj, Obj.Life, 0xFD, 1, Obj.iShield);
		sLog->outBasic("error-L3 : %s return %s %d %d",
			Obj.Name, __FILE__, __LINE__, pos);
	}
}


struct PMSG_REFILL
{
	PBMSG_HEAD2 h;	// C1:26
	BYTE IPos;	// 3
	BYTE LifeH;	// 4
	BYTE LifeL;	// 5
	BYTE Flag;	// 6
	BYTE btShieldH;	// 7
	BYTE btShieldL;	// 8
	int IGCLife;
	int IGCPenta;
};



void GameProtocol::GCReFillSend(CGameObject &Obj, int Life, BYTE Ipos, BYTE flag, WORD wShield)
{
	PMSG_REFILL pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x26, sizeof(pMsg));
	pMsg.IPos = Ipos;
	pMsg.LifeH = SET_NUMBERH(Life);
	pMsg.LifeL = SET_NUMBERL(Life);
	pMsg.btShieldH = SET_NUMBERH(wShield);
	pMsg.btShieldL = SET_NUMBERL(wShield);
	pMsg.Flag = flag;	// #error Flag is Disabled
	pMsg.IGCLife = Life;
	pMsg.IGCPenta = wShield;
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}


struct PMSG_MANASEND
{
	PBMSG_HEAD2 h;	// C1:27
	BYTE IPos;	// 3
	BYTE ManaH;	// 4
	BYTE ManaL;	// 5
	BYTE BPH;	// 6
	BYTE BPL;	// 7
	int IGCMana;
};

void GameProtocol::GCManaSend(CGameObject &Obj, int Mana, BYTE Ipos, BYTE flag, WORD BP)
{
	if (Obj < 0 || Obj > g_ConfigRead.server.GetObjectMax() - 1)
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	PMSG_MANASEND pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x27, sizeof(pMsg));
	pMsg.IPos = Ipos;
	pMsg.ManaH = SET_NUMBERH(Mana);
	pMsg.ManaL = SET_NUMBERL(Mana);
	pMsg.BPH = SET_NUMBERH(BP);
	pMsg.BPL = SET_NUMBERL(BP);
	pMsg.IGCMana = Mana;
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}


struct PMSG_INVENTORYDELETE
{
	PBMSG_HEAD2 h;	// C1:28
	BYTE IPos;	// 3
	BYTE Flag;	// 4
};

void GameProtocol::GCInventoryItemDeleteSend(CGameObject &Obj, BYTE pos, BYTE flag)
{
	PMSG_INVENTORYDELETE pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x28, sizeof(pMsg));
	pMsg.IPos = pos;
	pMsg.Flag = flag;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}

struct PMSG_ITEMUSESPECIALTIME
{
	PBMSG_HEAD2 h;	// C1:29
	BYTE Number;	// 3
	WORD Time;	// 4
};

void GameProtocol::GCItemObjectUseSpecialTimeSend(CGameObject &Obj, BYTE number, int time)
{
	if (time > 65535)
	{
		time = 65535;
	}

	PMSG_ITEMUSESPECIALTIME pMsg;

	PHeadSetBE((BYTE*)&pMsg, 0x29, sizeof(pMsg));
	pMsg.Number = number;
	pMsg.Time = time;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}

struct PMSG_ITEMDUR
{
	PBMSG_HEAD2 h;	// C1:2A
	BYTE IPos;	// 3
	BYTE Dur;	// 4
	BYTE Flag;	// 5
};


void GameProtocol::GCItemObjectDurSend(CGameObject &Obj, BYTE pos, BYTE dur, BYTE flag)
{
	PMSG_ITEMDUR pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x2A, sizeof(pMsg));
	pMsg.IPos = pos;
	pMsg.Dur = dur;
	pMsg.Flag = flag;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}



void GameProtocol::GCItemObjectDurSend2(CGameObject &Obj, BYTE pos, BYTE dur, BYTE flag)
{
	PMSG_ITEMDUR pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x2A, sizeof(pMsg));
	pMsg.IPos = pos;
	pMsg.Dur = dur;
	pMsg.Flag = flag;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}


struct PMSG_WEATHER
{
	PBMSG_HEAD2 h;	// C1:0F
	BYTE Weather;	// 3
};

void GameProtocol::CGWeatherSend(CGameObject &Obj, BYTE weather)
{
	PMSG_WEATHER pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x0F, sizeof(pMsg));
	pMsg.Weather = weather;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}



void GameProtocol::GCServerCmd(CGameObject &Obj, BYTE type, BYTE Cmd1, BYTE Cmd2)
{
	PMSG_SERVERCMD ServerCmd;

	PHeadSubSetB((BYTE*)&ServerCmd, 0xF3, 0x40, sizeof(ServerCmd));
	ServerCmd.CmdType = type;
	ServerCmd.X = Cmd1;
	ServerCmd.Y = Cmd2;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&ServerCmd, ServerCmd.h.size);
}




void GameProtocol::GCReqmoveDevilSquare(PMSG_REQ_MOVEDEVILSQUARE* lpMsg, CGameObject &Obj)
{
	if (Obj < 0 || Obj > g_ConfigRead.server.GetObjectMax() - 1)
	{
		return;
	}

	BYTE cSquareNumber = lpMsg->SquareNumber;
	BYTE cInvitationItemPos = lpMsg->InvitationItemPos - INVENTORY_BAG_START;

	if (Obj.m_IfState->use && Obj.m_IfState->type != 12)
		return;

	PMSG_RESULT_MOVEDEVILSQUARE pResult;

	PHeadSetB((BYTE*)&pResult, 0x90, sizeof(pResult));
	pResult.Result = 0;

	

	BOOL bPlayerKiller = FALSE; //season 2.5 add-on

	if (Obj.PartyNumber >= 0) //season 2.5 add-on
	{
		if ((gParty.GetPKPartyPenalty(Obj.PartyNumber)) > 4)
		{
			bPlayerKiller = TRUE;
		}
	}

	else
	{
		if (Obj.m_PK_Level > 4)
		{
			bPlayerKiller = TRUE;
		}
	}

	if (g_ConfigRead.data.common.PKEnterEventAllow == true)
	{
		bPlayerKiller = FALSE;
	}

	if (bPlayerKiller == TRUE) //season 2.5 changed
	{
		pResult.Result = 6;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (MAIN_INVENTORY_RANGE(cInvitationItemPos) == FALSE)
	{
		pResult.Result = 1;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if (DS_LEVEL_RANGE(cSquareNumber) == FALSE)
	{
		pResult.Result = 1;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if (g_DevilSquare.GetState() != DevilSquare_OPEN)
	{
		pResult.Result = 2;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if (g_DevilSquare.GetObjCount(cSquareNumber) >= g_DevilSquare.GetMaxUserInSquare())
	{
		pResult.Result = 5;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	CItemObject * sitem = Obj.pntInventory[cInvitationItemPos];

	if (sitem->m_Type != ITEMGET(14, 19) && (sitem->m_Type != ITEMGET(13, 46) || sitem->m_Durability <= 0.0f))	// Devil's Invitation
	{
		pResult.Result = 1;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	int ItemLevel = sitem->m_Level;

	if (sitem->m_Type == ITEMGET(13, 46))
		ItemLevel = 10;

	if (ItemLevel != 0)
	{
		if (ItemLevel != 10)
		{
			if (ItemLevel != (cSquareNumber + 1))
			{
				pResult.Result = 1;
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

				return;
			}
		}
	}

	BOOL bEnterCheck = FALSE;
	WORD movegateindex = 61;	// #gate
	int iENTER_LEVEL = g_DevilSquare.GetUserLevelToEnter(Obj, movegateindex);

	if (DS_LEVEL_RANGE(iENTER_LEVEL) == FALSE)
	{
		return;
	}

	if (ItemLevel == 10)
		cSquareNumber = iENTER_LEVEL;

	if (cSquareNumber != iENTER_LEVEL)
	{
		if (cSquareNumber > iENTER_LEVEL)
		{
			bEnterCheck = 2;
		}
		else
		{
			bEnterCheck = 1;
		}
	}

	if (bEnterCheck == 1)
	{
		pResult.Result = 3;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if (bEnterCheck == 2)
	{
		pResult.Result = 4;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if (sitem->m_Type == ITEMGET(14, 19) || (sitem->m_Type == ITEMGET(13, 46) && sitem->m_Durability == 1.0f))
	{
		gObjInventoryDeleteItem(Obj, cInvitationItemPos);
		GCInventoryItemDeleteSend(Obj, cInvitationItemPos, 1);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
	}
	else if (sitem->m_Type == ITEMGET(13, 46) && sitem->m_Durability > 1.0f)
	{
		sitem->m_Durability -= 1.0f;
		GCItemObjectDurSend2(Obj, cInvitationItemPos, sitem->m_Durability, 0);
	}

	Obj.m_nEventExp = 0;
	Obj.m_nEventScore = 0;
	Obj.m_nEventMoney = 0;
	Obj.m_bDevilSquareIndex = g_DevilSquare.GetDevilSquareIndex(movegateindex);
	Obj.m_bDevilSquareAuth = true;
	Obj.m_IfState->state = 0;
	Obj.m_IfState->type = 0;
	Obj.m_IfState->use = 0;

	gObjMoveGate(Obj, movegateindex);
	g_DevilSquare.IncObjCount(cSquareNumber);
}



struct PMSG_RESULT_DEVILSQUARE_REMAINTIME
{
	PBMSG_HEAD2 h;	// C1:91
	BYTE hEventType;	// 3
	BYTE RemainTime;	// 4
	BYTE EnteredUser;	// 5
	BYTE RemainTime_LOW;	// 6
};


void GameProtocol::GCReqDevilSquareRemainTime(PMSG_REQ_DEVILSQUARE_REMAINTIME* lpMsg, CGameObject &Obj)
{
	PMSG_RESULT_DEVILSQUARE_REMAINTIME pResult;

	PHeadSetB((BYTE*)&pResult, 0x91, sizeof(pResult));
	pResult.RemainTime = 0;
	int iENTER_LEVEL;

	switch (lpMsg->hEventType)
	{
	case 1:
		if (Obj.m_PK_Level >= 4 && g_ConfigRead.data.common.PKEnterEventAllow == false)
		{
			GCServerCmd(Obj, 0x37, 0, 0);

			return;
		}

		pResult.hEventType = 1;

		if (g_DevilSquare.GetState() == DevilSquare_CLOSE)
		{
			pResult.RemainTime = g_DevilSquare.GetRemainTime();
		}
		else if (g_DevilSquare.GetState() == DevilSquare_OPEN)
		{
			pResult.RemainTime = 0;
		}
		else
		{
			pResult.RemainTime = g_DevilSquare.GetRemainTime() + g_DevilSquare.GetCloseTime();
		}
		break;

	case 2:
		if (Obj.m_PK_Level >= 4 && g_ConfigRead.data.common.PKEnterEventAllow == false)
		{
			GCServerCmd(Obj, 0x38, 0, 0);

			return;
		}

		pResult.hEventType = 2;

		if (g_BloodCastle.GetCurrentState(lpMsg->btItemLevel) == 1)
		{
			if (g_BloodCastle.CheckCanEnter(lpMsg->btItemLevel) != false)
			{
				pResult.RemainTime = 0;
			}
			else
			{
				pResult.RemainTime = g_BloodCastle.GetRemainTime(lpMsg->btItemLevel);
			}
		}
		else
		{
			pResult.RemainTime = g_BloodCastle.GetRemainTime(lpMsg->btItemLevel);
		}
		break;

	case 4:
		if (Obj.m_PK_Level >= 4 && g_ConfigRead.data.common.PKEnterEventAllow == false)
		{
			GCServerCmd(Obj, 0x39, 0, 0);

			return;
		}

		iENTER_LEVEL = g_ChaosCastle.GetUserLevelToEnter(Obj);

		if (iENTER_LEVEL == -1)
			return;

		pResult.hEventType = 4;

		if (g_ChaosCastle.GetCurrentState(iENTER_LEVEL) == 1)
		{
			if (g_ChaosCastle.CheckCanEnter(iENTER_LEVEL) != false)
			{
				pResult.RemainTime = 0;
				pResult.RemainTime_LOW = 0;
				pResult.EnteredUser = g_ChaosCastle.GetCurEnteredUser(iENTER_LEVEL);
			}
			else
			{
				WORD wREMAIN_TIME = g_ChaosCastle.GetRemainTime(iENTER_LEVEL);
				pResult.RemainTime = SET_NUMBERH(wREMAIN_TIME);
				pResult.RemainTime_LOW = SET_NUMBERL(wREMAIN_TIME);
				pResult.EnteredUser = g_ChaosCastle.GetCurEnteredUser(iENTER_LEVEL);
			}
		}
		else
		{
			WORD wREMAIN_TIME = g_ChaosCastle.GetRemainTime(iENTER_LEVEL);
			pResult.RemainTime = SET_NUMBERH(wREMAIN_TIME);
			pResult.RemainTime_LOW = SET_NUMBERL(wREMAIN_TIME);
			pResult.EnteredUser = g_ChaosCastle.GetCurEnteredUser(iENTER_LEVEL);
		}
		break;
	case 5:
		pResult.hEventType = 5;
		pResult.RemainTime = g_IT_Event.GetRemainTime();
		pResult.RemainTime_LOW = 0;
		pResult.EnteredUser = 0;
		break;
	case 6:
		if (Obj.m_PK_Level < 4)
		{
			pResult.hEventType = 6;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		}
		else
		{
			GCServerCmd(Obj, 0x39, 0, 0);
			return;
		}
	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
}



void GameProtocol::AllSendMsg(BYTE* Msg, int size)
{
	for (int n = g_ConfigRead.server.GetObjectStartUserIndex(); n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		if (getGameObject(n)->Connected == PLAYER_PLAYING)
		{
			if (getGameObject(n)->Type == OBJ_USER)
			{
				IOCP.DataSend(n, Msg, size);
			}
		}
	}
}



void GameProtocol::AllSendSameMapMsg(UCHAR * Msg, int size, BYTE mapnumber)
{
	for (int n = 0; n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		if (getGameObject(n)->Connected == PLAYER_PLAYING)
		{
			if (getGameObject(n)->Type == OBJ_USER)
			{
				if (getGameObject(n)->MapNumber == mapnumber)
				{
					IOCP.DataSend(n, Msg, size);
				}
			}
		}
	}
}


struct PMSG_PING
{
	PBMSG_HEAD2 h;	// C1:71
};


//#unused
void GameProtocol::GCSendPing(CGameObject &Obj)
{
	PMSG_PING pMsgPing;

	PHeadSetB((BYTE*)&pMsgPing, 0x71, sizeof(pMsgPing));
	Obj.iPingTime = GetTickCount();

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsgPing, pMsgPing.h.size);
}


//#unused
void GameProtocol::GCPingSendRecv(PMSG_PING_RESULT* aRecv, CGameObject &Obj)
{
	int PingTime = GetTickCount() - Obj.iPingTime;

	MsgOutput(Obj, "Ping: %d ms", PingTime);
}



struct PMSG_REQ_REGISTER_EVENTCHIP
{
	PBMSG_HEAD2 h;	// C1:02
	int iINDEX;	// 4
	BYTE Pos;	// 8
	char szUID[11];	// 9
};

struct PMSG_REQ_REGISTER_STONES
{
	PBMSG_HEAD2 h;	// C1:06
	int iINDEX;	// 4
	BYTE iPosition;	// 8
	char szUID[11];	// 9
};


void GameProtocol::GCRegEventChipRecv(PMSG_REGEVENTCHIP* lpMsg, CGameObject &Obj)
{
	if (!EVENCHIP_TYPE_RANGE(lpMsg->Type))
		return;

	

	if (Obj.m_IfState->type == 1)
	{
		return;
	}

	if (Obj.m_IfState->type == 7)
	{
		return;
	}

	if (Obj.UseEventServer != FALSE)
	{
		PMSG_REGEVENTCHIP_RESULT Result;

		PHeadSetB((BYTE*)&Result, 0x95, sizeof(Result));
		Result.ChipCount = -1;
		Result.Type = lpMsg->Type;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&Result, Result.h.size);

		return;
	}

	int Pos = lpMsg->ChipPos + INVENTORY_BAG_START;
	CItemObject* sitem = Obj.pntInventory[Pos];

	if (!sitem->IsItem())
	{
		PMSG_REGEVENTCHIP_RESULT Result;

		PHeadSetB((BYTE*)&Result, 0x95, sizeof(Result));
		Result.ChipCount = -1;
		Result.Type = lpMsg->Type;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&Result, Result.h.size);

		return;
	}

	Obj.UseEventServer = TRUE;

	switch (lpMsg->Type)
	{
	case 0x00:
		if (sitem->m_Type == ITEMGET(14, 21) && sitem->m_Level == 0) // Rena
		{
			PMSG_REQ_REGISTER_EVENTCHIP pMsg;

			PHeadSubSetB((BYTE*)&pMsg, 0xBE, 0x02, sizeof(pMsg));
			pMsg.iINDEX = Obj.m_Index;
			pMsg.Pos = Pos;
			std::strcpy(pMsg.szUID, Obj.AccountID);

			wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg));
		}
		else
		{
			PMSG_REGEVENTCHIP_RESULT Result;

			PHeadSetB((BYTE*)&Result, 0x95, sizeof(Result));
			Result.ChipCount = -1;
			Result.Type = 0x00;

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&Result, Result.h.size);
			Obj.UseEventServer = FALSE;
		}
		break;
	case 0x01:
		if (sitem->m_Type == ITEMGET(14, 21) && sitem->m_Level == 1) // Stone
		{
			PMSG_REQ_REGISTER_STONES pMsg;

			PHeadSubSetB((BYTE*)&pMsg, 0xBE, 0x06, sizeof(pMsg));
			pMsg.iINDEX = Obj.m_Index;
			pMsg.iPosition = Pos;
			std::strcpy(pMsg.szUID, Obj.AccountID);

			wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg));

		}
		else
		{
			PMSG_REGEVENTCHIP_RESULT Result;

			PHeadSetB((BYTE*)&Result, 0x95, sizeof(Result));
			Result.ChipCount = -1;
			Result.Type = 0x01;

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&Result, Result.h.size);
			Obj.UseEventServer = FALSE;
		}
		break;
	default:
		Obj.UseEventServer = FALSE;
		break;
	}
}

struct PMSG_REQ_REGISTER_MUTONUM
{
	PBMSG_HEAD2 h;	// C1:03
	int iINDEX;	// 4
	char szUID[11];	// 8
};

void GameProtocol::GCGetMutoNumRecv(PMSG_GETMUTONUMBER* lpMsg, CGameObject &Obj)
{
	if (Obj.MutoNumber != 0)
	{
		char msg[255];
		wsprintf(msg, Lang.GetText(0, 527));
		GCServerMsgStringSend(msg, Obj, 1);
		return;
	}

	if (Obj.UseEventServer != FALSE)
		return;

	Obj.UseEventServer = TRUE;

	if (!gObjFind10EventChip(Obj))
	{
		PMSG_GETMUTONUMBER_RESULT Result;

		PHeadSetB((BYTE*)&Result, 0x96, sizeof(Result));
		Result.MutoNum[0] = -1;
		Result.MutoNum[1] = 0;
		Result.MutoNum[2] = 0;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&Result, Result.h.size);
		Obj.UseEventServer = FALSE;

		return;
	}

	PMSG_REQ_REGISTER_MUTONUM pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xBE, 0x03, sizeof(pMsg));
	pMsg.iINDEX = Obj.m_Index;
	std::strcpy(pMsg.szUID, Obj.AccountID);

	wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg));

}



void GameProtocol::GCUseEndEventChipRescv(CGameObject &Obj)
{
	if (!gObjIsConnectedGP(Obj))
	{
		return;
	}

	if (Obj.m_IfState->use && Obj.m_IfState->type == 9)
	{
		Obj.m_IfState->state = 0;
		Obj.m_IfState->type = 0;
		Obj.m_IfState->use = 0;
	}
}

struct PMSG_REQ_RESET_EVENTCHIP
{
	PBMSG_HEAD2 h;	// C1:[04:Rena] [09:Stone]
	int iINDEX;	// 4
	char szUID[11];	// 8
};

void GameProtocol::GCUseRenaChangeZenRecv(PMSG_EXCHANGE_EVENTCHIP* lpMsg, CGameObject &Obj)
{
	if (Obj.UseEventServer)
		return;

	Obj.UseEventServer = TRUE;

	PMSG_REQ_RESET_EVENTCHIP pMsg;

	if (lpMsg->btType == 1)	// Stone?
		PHeadSubSetB((BYTE*)&pMsg, 0xBE, 0x09, sizeof(pMsg));
	else
		PHeadSubSetB((BYTE*)&pMsg, 0xBE, 0x04, sizeof(pMsg));

	pMsg.iINDEX = Obj.m_Index;
	std::strcpy(pMsg.szUID, Obj.AccountID);

	wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg));

	if (lpMsg->btType == 0x01)
	{

	}
	else
	{

	}
}

struct PMSG_SEND_QEUSTINFO
{
	PBMSG_HEAD2 h;	// C1:A0
	BYTE Count;	// 3
	BYTE State[50];	// 4
};

void GameProtocol::CGRequestQuestInfo(CGameObject &Obj)
{
	

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.m_PlayerData->m_SendQuestInfo != 0)
		return;

	int questcount = g_QuestInfo.GetQeustCount();
	int foundquest = 0;

	PMSG_SEND_QEUSTINFO pMsg;
	LPQUEST_INFO lpQuestInfo;

	std::memcpy(pMsg.State, Obj.m_PlayerData->m_Quest, sizeof(pMsg.State));
	int i;
	for (i = 0; i < MAX_QUEST_INFO; i++)
	{
		lpQuestInfo = g_QuestInfo.GetQuestInfo(i);

		if (lpQuestInfo)
		{
			foundquest++;

			if (foundquest == questcount)
			{
				break;
			}
		}
	}

	int iSize = i / 4 + 5;

	PHeadSetB((BYTE*)&pMsg, 0xA0, iSize);
	pMsg.Count = i;
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, iSize);
	Obj.m_PlayerData->m_SendQuestInfo = true;
}

void GameProtocol::GCSendQuestInfo(CGameObject &Obj, int QuestIndex) // Third Quest
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	CGRequestQuestInfo(Obj);
	
	PMSG_SETQUEST pMsg;

	PHeadSetB((BYTE*)&pMsg, 0xA1, sizeof(pMsg));
	pMsg.QuestIndex = QuestIndex;
	pMsg.State = g_QuestInfo.GetQuestStateBYTE(lpObj, QuestIndex);

	if (pMsg.State != 0)
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, sizeof(pMsg));
}


struct PMSG_SETQUEST_RESULT
{
	PBMSG_HEAD2 h;	// C1:A2
	BYTE QuestIndex;	// 3
	BYTE Result;	// 4
	BYTE State;	// 5
};


void GameProtocol::CGSetQuestState(PMSG_SETQUEST* lpMsg, CGameObject &Obj)
{
	
	PMSG_SETQUEST_RESULT pMsg;

	PHeadSetB((BYTE*)&pMsg, 0xA2, sizeof(pMsg));
	pMsg.QuestIndex = lpMsg->QuestIndex;
	pMsg.Result = g_QuestInfo.SetQuestState(lpObj, lpMsg->QuestIndex, lpMsg->State);
	pMsg.State = g_QuestInfo.GetQuestStateBYTE(lpObj, lpMsg->QuestIndex);

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, sizeof(pMsg));
}


struct PMSG_SETQUEST_PRIZE
{
	PBMSG_HEAD2 h;	// C1:A3
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	BYTE Type;	// 5
	BYTE Count;	// 6
};


void GameProtocol::GCSendQuestPrize(CGameObject &Obj, BYTE Type, BYTE Count)
{
	CGRequestQuestInfo(Obj);
	

	PMSG_SETQUEST_PRIZE pMsg;

	PHeadSetB((BYTE*)&pMsg, 0xA3, sizeof(pMsg));
	pMsg.NumberH = SET_NUMBERH(Obj.m_Index);
	pMsg.NumberL = SET_NUMBERL(Obj.m_Index);
	pMsg.Type = Type;
	pMsg.Count = Count;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
	MsgSendV2(lpObj, (BYTE*)&pMsg, sizeof(pMsg));
}



void GameProtocol::CGCloseWindow(CGameObject &Obj)
{
	if (Obj.m_IfState->use && Obj.m_IfState->type)
	{
		if (Obj.m_IfState->type == 7)
			return;

		if (Obj.m_IfState->type == 10)	// Marlon
		{
			if (Obj.TargetNpcNumber != -1 && getGameObject(Obj.TargetNpcNumber)->Class == 229)
			{
				gQeustNpcTeleport.TalkRefDel();
				Obj.TargetNpcNumber = -1;
			}
		}

		if (Obj.TargetNpcNumber != -1 && getGameObject(Obj.TargetNpcNumber)->Class == 568)
		{
			gQuestExpNpcTeleport.TalkRefDel();
			Obj.TargetNpcNumber = -1;
		}

		if (Obj.m_IfState->type == 3)
		{
			Obj.TargetNpcNumber = -1;
		}

		if (Obj.m_IfState->type == 1)
		{
			CGTradeCancelButtonRecv(Obj);
		}

		if (Obj.m_IfState->type == 6)
		{
			CGWarehouseUseEnd(Obj);
		}

		if (Obj.m_IfState->type == 13)
		{
			g_MixSystem.ChaosBoxInit(&Obj);
			gObjInventoryCommit(Obj);
		}

		Obj.m_IfState->state = 0;
		Obj.m_IfState->type = 0;
		Obj.m_IfState->use = 0;
	}
}



void GameProtocol::CGRequestEnterBloodCastle(PMSG_REQ_MOVEBLOODCASTLE* lpMsg, CGameObject &Obj)
{
	//BYTE btBridgeNumber = lpMsg->iBridgeNumber;
	//BYTE btInvisibleCourtItemPos = lpMsg->iItemPos;
	BYTE btInvisibleCourtItemPos;
	int iITEM_LEVEL = 0;
	UINT64 dwITEM_SERIAL = 0;
	PMSG_RESULT_MOVEBLOODCASTLE pResult;

	PHeadSetB((BYTE*)&pResult, 0x9A, sizeof(pResult));
	pResult.Result = 0;

	if (Obj.Type != OBJ_USER || Obj.Connected <= PLAYER_LOGGED)
		return;

	if (Obj.m_IfState->use && Obj.m_IfState->type != 12)
		return;

	BYTE btBridgeNumber;
	BYTE btGateNumber;

	int CurrLvl = Obj.Level + Obj.m_PlayerData->MasterLevel;

	if ((CurrLvl >= 1) && (CurrLvl <= 49))
	{
		pResult.Result = 4;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}
	if ((CurrLvl >= 50) && (CurrLvl <= 179))
	{
		btBridgeNumber = 0;
		btGateNumber = 66;
	}
	else if ((CurrLvl >= 180) && (CurrLvl <= 269))
	{
		btBridgeNumber = 1;
		btGateNumber = 67;
	}
	else if ((CurrLvl >= 270) && (CurrLvl <= 349))
	{
		btBridgeNumber = 2;
		btGateNumber = 68;
	}
	else if ((CurrLvl >= 350) && (CurrLvl <= 399))
	{
		btBridgeNumber = 3;
		btGateNumber = 69;
	}
	else if ((CurrLvl >= 400) && (CurrLvl <= 500))
	{
		btBridgeNumber = 4;
		btGateNumber = 70;
	}
	else if ((CurrLvl >= 501) && (CurrLvl <= 650))
	{
		btBridgeNumber = 5;
		btGateNumber = 71;
	}
	else if ((CurrLvl >= 651) && (CurrLvl <= 850))
	{
		btBridgeNumber = 6;
		btGateNumber = 80;
	}

	if (Obj.Type != OBJ_USER || Obj.Connected <= PLAYER_LOGGED)
		return;

	if (Obj.m_IfState->use && Obj.m_IfState->type != 12)
		return;

	BOOL bPlayerKiller = FALSE; //Season 2.5 add-on

	if (Obj.PartyNumber >= 0) //Season 2.5 add-on
	{
		if (gParty.GetPKPartyPenalty(Obj.PartyNumber) >= 5)
		{
			bPlayerKiller = TRUE;
		}
	}
	else if (Obj.m_PK_Level >= 4)
	{
		bPlayerKiller = TRUE;
	}

	if (g_ConfigRead.data.common.PKEnterEventAllow == true)
	{
		bPlayerKiller = FALSE;
	}

	if (bPlayerKiller == TRUE)
	{
		pResult.Result = 7;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (g_BloodCastle.CheckCanEnter(btBridgeNumber) == false)
	{
		pResult.Result = 2;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	int Err = 0;

	for (int btInvisibleCourtItemPos = 12; btInvisibleCourtItemPos < 76; btInvisibleCourtItemPos++)
	{

		if (Obj.pntInventory[btInvisibleCourtItemPos]->m_Type == ITEMGET(13, 18))	// Cloack's Invitation
		{
			if (Obj.pntInventory[btInvisibleCourtItemPos]->m_Level == btBridgeNumber + 1)
			{

				gObjInventoryDeleteItem(iIndex, btInvisibleCourtItemPos);
				GCInventoryItemDeleteSend(iIndex, btInvisibleCourtItemPos, 1);
				Err++;
				break;
			}

		}
		else if (Obj.pntInventory[btInvisibleCourtItemPos]->m_Type == ITEMGET(13, 47)) //Ticket BC
		{
			if (Obj.pntInventory[btInvisibleCourtItemPos]->m_Durability > 0.0f)
			{
				Obj.pntInventory[btInvisibleCourtItemPos]->m_Durability -= 1.0f;
				if (Obj.pntInventory[btInvisibleCourtItemPos]->m_Durability == 0.0f)
				{
					gObjInventoryDeleteItem(iIndex, btInvisibleCourtItemPos);
					GCInventoryItemDeleteSend(iIndex, btInvisibleCourtItemPos, 1);
				}
				else
					GCItemObjectDurSend2(Obj, btInvisibleCourtItemPos, Obj.pntInventory[btInvisibleCourtItemPos]->m_Durability, 0);
				Err++;
				break;
			}
		}

	}

	if (Err == 0)
	{
		pResult.Result = 1;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	int iBC_INDEX = g_BloodCastle.EnterUserBridge(btBridgeNumber, iIndex);

	if (iBC_INDEX == -1)
	{
		pResult.Result = 5;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	Obj.m_cBloodCastleIndex = btBridgeNumber;
	Obj.m_cBloodCastleSubIndex = iBC_INDEX;
	Obj.m_iBloodCastleEXP = 0;
	Obj.m_IfState->state = 0;
	Obj.m_IfState->type = 0;
	Obj.m_IfState->use = 0;

	g_BloodCastle.SearchUserDeleteQuestItem(Obj);
	char szTemp[256];
	wsprintf(szTemp, Lang.GetText(0, 74), btBridgeNumber + 1);
	GCServerMsgStringSend(szTemp, iIndex, 1);

	gObjMoveGate(iIndex, btGateNumber);
}

struct PMSG_RESULT_MOVECHAOSCASTLE
{
	PBMSG_HEAD2 h;	// C1:AF:01
	BYTE Result;	// 4
};

void GameProtocol::CGRequestEnterChaosCastle(PMSG_REQ_MOVECHAOSCASTLE* lpMsg, CGameObject &Obj)
{
	BYTE btChaosCastleNumber = lpMsg->iChaosCastleNumber - 1;
	BYTE btGuardMailSetItemPos = lpMsg->iItemPos;
	int iITEM_LEVEL = 0;
	UINT64 dwITEM_SERIAL = 0;
	PMSG_RESULT_MOVECHAOSCASTLE pResult;

	PHeadSubSetB((BYTE*)&pResult, 0xAF, 0x01, sizeof(pResult));
	pResult.Result = 0;

	if (Obj.Type != OBJ_USER || Obj.Connected <= PLAYER_LOGGED)
		return;

	if (BC_MAP_RANGE(Obj.MapNumber) || CC_MAP_RANGE(Obj.MapNumber) || DS_MAP_RANGE(Obj.MapNumber) ||
		IT_MAP_RANGE(Obj.MapNumber) || DG_MAP_RANGE(Obj.MapNumber) || IMPERIAL_MAP_RANGE(Obj.MapNumber) ||
		Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL || g_ConfigRead.server.GetServerType() == SERVER_ARCA)
	{
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 118));
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pNotice, pNotice.h.size);
		return;
	}

	if (Obj.m_IfState->use && Obj.m_IfState->type != 12)
		return;

	if (Obj.m_PlayerData->m_bUsePartyMatching)
	{
		PMSG_ANS_PARTYMATCHING_ERROR pMsg;
		PHeadSubSetB((BYTE*)&pMsg, 0xEF, 0x09, sizeof(pMsg));
		pMsg.iResult = -1;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	if (g_ConfigRead.pk.bPkPenaltyDisable == FALSE)
	{
		BOOL bPlayerKiller = FALSE; //Season 2.5 add-on

		if (Obj.PartyNumber >= 0) //Season 2.5 add-on
		{
			if (gParty.GetPKPartyPenalty(Obj.PartyNumber) >= 6)
			{
				bPlayerKiller = TRUE;
			}
		}
		else if (Obj.m_PK_Level >= 6)
		{
			bPlayerKiller = TRUE;
		}

		if (bPlayerKiller == TRUE)
		{
			PMSG_NOTICE pNotice;
			TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 119));
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pNotice, pNotice.h.size);
			return;
		}
	}

	if (gObjDuelCheck(Obj) == TRUE)
	{
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 120));
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pNotice, pNotice.h.size);
		return;
	}

	if (Obj.m_PlayerData->lpGuild)
	{
		if (Obj.m_PlayerData->lpGuild->WarState == 1)
		{
			PMSG_NOTICE pNotice;
			TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 121));
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pNotice, pNotice.h.size);
			return;
		}
	}

	if (Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 3) ||	// Dino
		Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 2) ||	// Uniria
		Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 37))	// Fenrir
	{
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 122));
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pNotice, pNotice.h.size);
		return;
	}

	BOOL bPlayerKiller = FALSE; //Season 2.5 add-on

	if (Obj.PartyNumber >= 0) //Season 2.5 add-on
	{
		if (gParty.GetPKPartyPenalty(Obj.PartyNumber) >= 5)
		{
			bPlayerKiller = TRUE;
		}
	}
	else if (Obj.m_PK_Level >= 4)
	{
		bPlayerKiller = TRUE;
	}

	if (bPlayerKiller == TRUE)
	{
		pResult.Result = 8;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	int iENTER_LEVEL = g_ChaosCastle.GetUserLevelToEnter(Obj);

	if (iENTER_LEVEL == -1)
	{
		pResult.Result = 1;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (g_ChaosCastle.CheckCanEnter(iENTER_LEVEL) == false)
	{
		pResult.Result = 2;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (Obj.pntInventory[btGuardMailSetItemPos]->IsItem() == TRUE)
	{
		if (Obj.pntInventory[btGuardMailSetItemPos]->m_Type == ITEMGET(13, 29)) // Armor of Guardman
		{
			iITEM_LEVEL = Obj.pntInventory[btGuardMailSetItemPos]->m_Level;
			dwITEM_SERIAL = Obj.pntInventory[btGuardMailSetItemPos]->m_Number;
		}
		else
		{
			pResult.Result = 1;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}
	}
	else
	{
		pResult.Result = 1;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (g_ChaosCastle.CheckWearingMOPH(Obj) == TRUE)
	{
		GCServerMsgStringSend(Lang.GetText(0, 123), iIndex, 1);
		return;
	}

	int iBC_INDEX = -1;

	if (g_ChaosCastle.CheckUserEnterMoney(iIndex, iENTER_LEVEL))
	{
		iBC_INDEX = g_ChaosCastle.EnterUserChaosCastle(iENTER_LEVEL, iIndex);

		if (iBC_INDEX == -1)
		{
			pResult.Result = 5;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}
	}
	else
	{
		pResult.Result = 7;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	BOOL bRES = g_ChaosCastle.PayUserEnterMoney(iIndex, iENTER_LEVEL);

	if (bRES == FALSE)
	{
		g_ChaosCastle.LeaveUserChaosCastle(iENTER_LEVEL, iIndex);
		pResult.Result = 5;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	gObjInventoryDeleteItem(iIndex, btGuardMailSetItemPos);
	GCInventoryItemDeleteSend(iIndex, btGuardMailSetItemPos, 1);

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

	Obj.m_cChaosCastleIndex = iENTER_LEVEL;
	Obj.m_cChaosCastleSubIndex = iBC_INDEX;
	Obj.m_IfState->state = 0;
	Obj.m_IfState->type = 0;
	Obj.m_IfState->use = 0;

	if (Obj.m_bPShopOpen == true)
		g_PersonalStore.CGPShopReqClose(Obj);

	if (Obj.PartyNumber >= 0)
	{
		PMSG_PARTYDELUSER pMsg;
		int pindex = gParty.GetIndex(Obj.PartyNumber, iIndex, Obj.DBNumber);

		if (pindex >= 0)
		{
			pMsg.Number = pindex;
			CGPartyDelUser(&pMsg, iIndex, 0);
		}
	}

	if (Obj.Class == CLASS_ELF && Obj.m_RecallMon >= 0)
	{
		GCRecallMonLife(iIndex, 60, 0);
		gObjMonsterCallKill(Obj);
	}

	char szTemp[256];
	wsprintf(szTemp, Lang.GetText(0, 124), iENTER_LEVEL + 1);
	GCServerMsgStringSend(szTemp, iIndex, 1);

	switch (iENTER_LEVEL + 1)
	{
	case 1:
		gObjMoveGate(iIndex, 82);
		break;
	case 2:
		gObjMoveGate(iIndex, 83);
		break;
	case 3:
		gObjMoveGate(iIndex, 84);
		break;
	case 4:
		gObjMoveGate(iIndex, 85);
		break;
	case 5:
		gObjMoveGate(iIndex, 86);
		break;
	case 6:
		gObjMoveGate(iIndex, 87);
		break;
	case 7:
		gObjMoveGate(iIndex, 272);
		break;
	}

}

void GameProtocol::CGRequestRepositionUserInChaosCastle(PMSG_REQ_REPOSUSER_IN_CC * lpMsg, CGameObject &Obj)
{
	if (!gObjIsConnected(Obj))
		return;

	if (!CC_MAP_RANGE(Obj.MapNumber))
		return;

	if (g_ChaosCastle.ObjSetPosition(Obj, Obj.X, Obj.Y) == TRUE)
		Obj.m_iChaosCastleBlowTime = GetTickCount();
}



void GameProtocol::CGRequestEventEnterCount(PMSG_REQ_CL_ENTERCOUNT* lpMsg, CGameObject &Obj)
{
	if (!lpMsg)
		return;

	switch (lpMsg->btEventType)
	{
	case 0x02:
		EGReqBloodCastleEnterCount(Obj);
		break;
	}
}

struct PMSG_REQ_2ANIV_SERIAL
{
	PBMSG_HEAD2 h;	// C1:08
	int iINDEX;	// 8
	char szUID[MAX_ACCOUNT_LEN + 1];	// 8
	char SERIAL1[5];	// 13
	char SERIAL2[5];	// 18
	char SERIAL3[5];	// 1D
	int iMEMB_GUID;	// 24
};

void GameProtocol::CGRequestLottoRegister(PMSG_REQ_2ANV_LOTTO_EVENT* lpMsg, CGameObject &Obj)
{
	PMSG_REQ_2ANIV_SERIAL pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xBE, 0x08, sizeof(pMsg));

	if (Obj.UseEventServer)
		return;

	Obj.UseEventServer = TRUE;
	pMsg.iINDEX = Obj.m_Index;
	pMsg.iMEMB_GUID = Obj.DBNumber;
	std::memcpy(pMsg.szUID, Obj.AccountID, MAX_ACCOUNT_LEN);
	pMsg.szUID[MAX_ACCOUNT_LEN] = 0;
	std::memcpy(pMsg.SERIAL1, lpMsg->SERIAL1, 4);
	pMsg.SERIAL1[4] = 0;
	std::memcpy(pMsg.SERIAL2, lpMsg->SERIAL2, 4);
	pMsg.SERIAL2[4] = 0;
	std::memcpy(pMsg.SERIAL3, lpMsg->SERIAL3, 4);
	pMsg.SERIAL3[4] = 0;

	wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg));

	sLog->outBasic("[Mu_2Anv_Event] [%s][%s] Register Lotto Number (Serial: %s-%s-%s)",
		Obj.AccountID, Obj.Name, pMsg.SERIAL1, pMsg.SERIAL2, pMsg.SERIAL3);
}

struct SDHP_CHARACTER_TRANSFER
{
	PBMSG_HEAD2 h;	// C1:CF
	char Account[10];	// 3
	short Number;	// E
};


void GameProtocol::CGReqMoveOtherServer(PMSG_REQ_MOVE_OTHERSERVER * lpMsg, CGameObject &Obj)
{
	if (!gEnableServerDivision)
		return;

	if (!gObjIsConnectedGP(Obj))
	{
		sLog->outError( "error-L3 [%s][%d]", __FILE__, __LINE__);
		return;
	}

	

	if (Obj.m_MoveOtherServer)
		return;

	Obj.m_MoveOtherServer = true;

	PMSG_DEFRESULT pResult;
	PHeadSetB((BYTE*)&pResult, 0x99, sizeof(pResult));

	if (!gObjPasswordCheck(Obj, lpMsg->LastJoominNumber))
	{
		pResult.result = 0;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		Obj.m_MoveOtherServer = false;

		sLog->outBasic("[CharTrasfer] Fail (JoominNumber) [%s][%s]",
			Obj.AccountID, Obj.Name);

		GCServerMsgStringSend(Lang.GetText(0, 528), Obj, 1);

		return;
	}

	SDHP_CHARACTER_TRANSFER pCharTransfer;

	GJSetCharacterInfo(Obj, FALSE);
	PHeadSetB((BYTE*)&pCharTransfer, 0xCF, sizeof(pCharTransfer));
	pCharTransfer.Number = Obj.m_Index;
	std::memcpy(pCharTransfer.Account, Obj.AccountID, sizeof(pCharTransfer.Account));

	wsDataCli.DataSend((PCHAR)&pCharTransfer, sizeof(pCharTransfer));

	sLog->outBasic("[CharTrasfer] Request Character Trasfer [%s][%s]",
		Obj.AccountID, Obj.Name);
}



void GameProtocol::GCPacketCheckSumRecv(PMSG_PACKETCHECKSUM * aRecv, CGameObject &Obj)
{
	gPacketCheckSum.AddCheckSum(Obj, aRecv->funcindex, aRecv->CheckSum);
}

struct PMSG_GETITEMINFO_FOR_PARTY
{
	PBMSG_HEAD2 h;	// C1:47
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	UINT ItemInfo;	// 8
	BYTE Level;	// C
};

void GameProtocol::GCSendGetItemInfoForParty(CGameObject &Obj, CMapItem * lpItem)
{
	

	if (!lpItem)
		return;

	if (Obj.PartyNumber >= 0)
	{
		PMSG_GETITEMINFO_FOR_PARTY pMsg;

		pMsg.h.c = 0xC1;
		pMsg.h.headcode = 0x47;
		pMsg.h.size = sizeof(pMsg);
		pMsg.NumberH = SET_NUMBERH(Obj.m_Index);
		pMsg.NumberL = SET_NUMBERL(Obj.m_Index);
		pMsg.ItemInfo = lpItem->m_Type & (ITEMGET(MAX_TYPE_ITEMS - 1, MAX_SUBTYPE_ITEMS - 1));

		if (lpItem->m_Option1)
			pMsg.ItemInfo |= 0x2000;

		if (lpItem->m_Option2)
			pMsg.ItemInfo |= 0x4000;

		if (lpItem->m_Option3)
			pMsg.ItemInfo |= 0x8000;

		if (lpItem->m_NewOption)
			pMsg.ItemInfo |= 0x10000;

		if (lpItem->m_SetOption)
			pMsg.ItemInfo |= 0x20000;

		if (g_kItemSystemFor380.Is380OptionItem((lpItem != NULL) ? (CItemObject*)&lpItem->m_Number : NULL))
			pMsg.ItemInfo |= 0x40000;

		pMsg.Level = lpItem->m_Level;

		int partycount = gParty.GetPartyCount(Obj.PartyNumber);

		if (partycount >= 0)
		{
			for (int n = 0; n < MAX_USER_IN_PARTY; n++)
			{
				int number = gParty.m_PartyS[Obj.PartyNumber].Number[n];

				if (number >= 0)
				{
					IOCP.DataSend(number, (BYTE*)&pMsg, pMsg.h.size);
				}
			}
		}
	}
}


struct PMSG_EFFECTINFO
{
	PBMSG_HEAD2 h;	// C1:48
	BYTE NumberH;	// 3
	BYTE NumberL;	// 4
	BYTE btType;	// 5
};

void GameProtocol::GCSendEffectInfo(CGameObject &Obj, BYTE btType)
{
	PMSG_EFFECTINFO pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x48;
	pMsg.h.size = sizeof(pMsg);
	pMsg.NumberH = SET_NUMBERH(Obj.m_Index);
	pMsg.NumberL = SET_NUMBERL(Obj.m_Index);
	pMsg.btType = btType;

	if (btType == 17)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	MsgSendV2(lpObj, (BYTE*)&pMsg, pMsg.h.size);
}



void GameProtocol::CGRequestPetItemCommand(PMSG_REQUEST_PET_ITEM_COMMAND * lpMsg, CGameObject &Obj)
{
	if (!gObjIsConnectedGP(Obj))
	{
		sLog->outBasic("error-L2 : Index %s %d", __FILE__, __LINE__);
		return;
	}

	if (lpMsg->PetType)
		return;

	int iTargetIndex = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);
	gDarkSpirit[Obj - g_ConfigRead.server.GetObjectStartUserIndex()].ChangeCommand(lpMsg->Command, iTargetIndex);
}


// DONE YAYA!!!
void GameProtocol::CGRequestPetItemInfo(CGameObject &Obj, PMSG_SEND_PET_ITEMINFO &pMsg)
{
	if (!gObjIsConnectedGP(lpObj))
	{
		sLog->outBasic("error-L2 : Index %s %d", __FILE__, __LINE__);
		return;
	}

	if (pMsg.PetType != 0 && pMsg.PetType != 1)
		return;

	if (pMsg.InvenType == 0)	// Inventory
	{
		if (!INVENTORY_RANGE(pMsg.nPos))
		{
			sLog->outError("[PetItem] [%s][%s] RequestPetItem Invalid Iventory nPos %d",
				Obj.AccountID, Obj.Name, pMsg.nPos);
			return;
		}

		if (Obj.pntInventory[pMsg.nPos]->IsItem())
		{
			pMsg.Level = Obj.pntInventory[pMsg.nPos]->m_PetItem_Level;
			pMsg.Exp = Obj.pntInventory[pMsg.nPos]->m_PetItem_Exp - gPetItemExp.m_DarkSpiritExpTable[pMsg.Level];
			pMsg.Dur = (BYTE)Obj.pntInventory[pMsg.nPos]->m_Durability;

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
		}
	}
	else if (pMsg.InvenType == 1)	// Warehouse
	{
		if (Obj.LoadWareHouseInfo != false)
		{
			if (!WAREHOUSE_RANGE(pMsg.nPos))
			{
				sLog->outError("[PetItem] [%s][%s] RequestPetItem Invalid WareHouse nPos %d",
					Obj.AccountID, Obj.Name, pMsg.nPos);
				return;
			}

			if (Obj.pntWarehouse[pMsg.nPos]->IsItem())
			{
				pMsg.Level = Obj.pntWarehouse[pMsg.nPos]->m_PetItem_Level;
				pMsg.Exp = Obj.pntWarehouse[pMsg.nPos]->m_PetItem_Exp - gPetItemExp.m_DarkSpiritExpTable[pMsg.Level];
				pMsg.Dur = (BYTE)Obj.pntInventory[pMsg.nPos]->m_Durability;

				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
			}
		}
	}
	else if (pMsg.InvenType == 2)	// Trade
	{
		if (Obj.m_IfState->use < 1 || Obj.m_IfState->type != 1)
			return;

		if (!TRADE_BOX_RANGE(pMsg.nPos))
		{
			sLog->outError("[PetItem] [%s][%s] RequestPetItem Invalid Trade nPos %d",
				Obj.AccountID, Obj.Name, pMsg.nPos);
			return;
		}

		if (Obj.pntTrade[pMsg.nPos]->IsItem())
		{
			pMsg.Level = Obj.pntTrade[pMsg.nPos]->m_PetItem_Level;
			pMsg.Exp = Obj.pntTrade[pMsg.nPos]->m_PetItem_Exp - gPetItemExp.m_DarkSpiritExpTable[pMsg.Level];
			pMsg.Dur = (BYTE)Obj.pntInventory[pMsg.nPos]->m_Durability;

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
		}
	}
	else if (pMsg.InvenType == 3)	// Target Trade
	{
		if (Obj.m_IfState->use < 1 || Obj.m_IfState->type != 1)
			return;

		int iTargetIndex = Obj.TargetNumber;

		if (iTargetIndex < 0)
			return;

		if (!ObjectMaxRange(iTargetIndex))
			return;

		if (getGameObject(iTargetIndex)->m_IfState->use < 1 || getGameObject(iTargetIndex)->m_IfState->type != 1)
			return;

		if (!TRADE_BOX_RANGE(pMsg.nPos))
		{
			sLog->outError("[PetItem] [%s][%s] RequestPetItem Invalid TargetTrade nPos %d",
				Obj.AccountID, Obj.Name, pMsg.nPos);
			return;
		}

		if (getGameObject(iTargetIndex)->pntTrade[pMsg.nPos]->IsItem())
		{
			pMsg.Level = getGameObject(iTargetIndex)->pntTrade[pMsg.nPos]->m_PetItem_Level;
			pMsg.Exp = getGameObject(iTargetIndex)->pntTrade[pMsg.nPos]->m_PetItem_Exp - gPetItemExp.m_DarkSpiritExpTable[pMsg.Level];
			pMsg.Dur = Obj.pntInventory[pMsg.nPos]->m_Durability;

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
		}
	}
	else if (pMsg.InvenType == 4)	// Chaos
	{
		if (!CHAOS_BOX_RANGE(pMsg.nPos))
		{
			sLog->outError("[PetItem] [%s][%s] RequestPetItem Invalid Chaos nPos %d",
				Obj.AccountID, Obj.Name, pMsg.nPos);
			return;
		}

		if (Obj.pntChaosBox[pMsg.nPos]->IsItem())
		{
			pMsg.Level = Obj.pntChaosBox[pMsg.nPos]->m_PetItem_Level;
			pMsg.Exp = Obj.pntChaosBox[pMsg.nPos]->m_PetItem_Exp - gPetItemExp.m_DarkSpiritExpTable[pMsg.Level];
			pMsg.Dur = (BYTE)Obj.pntInventory[pMsg.nPos]->m_Durability;

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
		}
	}
	else if (pMsg.InvenType == 5)	// Personal Shop
	{
		if (!INVENTORY_RANGE(pMsg.nPos))
		{
			sLog->outError("[PetItem] [%s][%s] RequestPetItem Invalid Iventory nPos %d",
				Obj.AccountID, Obj.Name, pMsg.nPos);
			return;
		}

		if (Obj.m_iPShopDealerIndex != -1)
		{
			if (ObjectMaxRange(Obj.m_iPShopDealerIndex))
			{
				if (getGameObject(Obj.m_iPShopDealerIndex)->pntInventory[pMsg.nPos]->IsItem())
				{
					pMsg.Level = getGameObject(Obj.m_iPShopDealerIndex)->pntInventory[pMsg.nPos]->m_PetItem_Level;
					pMsg.Exp = getGameObject(Obj.m_iPShopDealerIndex)->pntInventory[pMsg.nPos]->m_PetItem_Exp - gPetItemExp.m_DarkSpiritExpTable[pMsg.Level];
					pMsg.Dur = (BYTE)Obj.pntInventory[pMsg.nPos]->m_Durability;

					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
				}
			}
		}
	}
}





struct PMSG_ANS_GUILDVIEWPORT
{
	PBMSG_HEAD2 h;	// C1:66
	int GuildNumber;	// 4
	BYTE btGuildType;	// 8
	BYTE UnionName[8];	// 9
	char GuildName[8];	// 11
	BYTE Mark[32];	// 19
};

void GameProtocol::GCGuildViewportInfo(PMSG_REQ_GUILDVIEWPORT * aRecv, CGameObject &Obj)
{
	if (!gObjIsConnectedGP(Obj))
	{
		sLog->outBasic("error-L2 : Index %s %d", __FILE__, __LINE__);
		return;
	}
	
	DWORD dwGuildNumber = aRecv->GuildNumber;
	GUILD_INFO_STRUCT * lpGuildInfo = Guild.SearchGuild_Number(dwGuildNumber);

	if (lpGuildInfo)
	{
		PMSG_ANS_GUILDVIEWPORT pMsg;

		pMsg.h.c = 0xC1;
		pMsg.h.headcode = 0x66;
		pMsg.h.size = sizeof(pMsg);
		pMsg.GuildNumber = lpGuildInfo->Number;
		pMsg.btGuildType = lpGuildInfo->btGuildType;

		TUnionInfo * pUnionInfo = UnionManager.SearchUnion(lpGuildInfo->iGuildUnion);

		if (pUnionInfo)
			std::memcpy(pMsg.UnionName, pUnionInfo->m_szMasterGuild, MAX_GUILD_LEN);
		else
			pMsg.UnionName[0] = '\0';

		std::memcpy(pMsg.GuildName, lpGuildInfo->Name, MAX_GUILD_LEN);
		std::memcpy(pMsg.Mark, lpGuildInfo->Mark, sizeof(pMsg.Mark));

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
	}
	else
	{
		sLog->outBasic("GuildViewportInfo Fail : [%s] GuildNumber : %d",
			Obj.Name, dwGuildNumber);
	}
}





void GameProtocol::CGGuildAssignStatus(PMSG_GUILD_ASSIGN_STATUS_REQ * aRecv, CGameObject &Obj)
{
	if (!gObjIsConnectedGP(Obj))
	{
		sLog->outBasic("error-L2 : Index %s %d", __FILE__, __LINE__);
		return;
	}

	PMSG_GUILD_ASSIGN_STATUS_RESULT pMsg = { 0 };

	pMsg.h.set((BYTE*)&pMsg, 0xE1, 0x00, sizeof(pMsg));
	pMsg.btType = aRecv->btType;
	std::memcpy(pMsg.szTagetName, aRecv->szTagetName, sizeof(pMsg.szTagetName));

	if (Obj.m_PlayerData->GuildNumber <= 0 || Obj.m_PlayerData->lpGuild == NULL)
	{
		pMsg.btResult = GUILD_ANS_NOTEXIST_GUILD;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
		return;
	}

	if (Obj.m_PlayerData->GuildStatus != G_MASTER)
	{
		pMsg.btResult = GUILD_ANS_NOTEXIST_PERMISSION;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
		return;
	}

	char szTargetName[MAX_ACCOUNT_LEN + 1] = { 0 };
	std::memcpy(szTargetName, aRecv->szTagetName, MAX_ACCOUNT_LEN);

	int iArcaBattleState = g_ArcaBattle.GetState();

	if (iArcaBattleState > 2 && iArcaBattleState < 9)
	{
		pMsg.btResult = GUILD_ANS_ARCA_BATTLE_ONGOING;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
		return;
	}

	if (!strcmp(Obj.Name, szTargetName))
	{
		pMsg.btResult = GUILD_ANS_NOTEXIST_PERMISSION;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
		return;
	}

	if (Obj.m_PlayerData->lpGuild == NULL)
		return;

	if (aRecv->btType == 1 || aRecv->btType == 2)
	{
		int iSubMasterCount = 0;
		int iBattleMasterCount = 0;

		for (int n = 0; n < MAX_USER_GUILD; n++)
		{
			if (Obj.m_PlayerData->lpGuild->Use[n] > 0)
			{
				if (Obj.m_PlayerData->lpGuild->GuildStatus[n] == G_SUB_MASTER)
				{
					iSubMasterCount++;
				}
				else if (Obj.m_PlayerData->lpGuild->GuildStatus[n] == G_BATTLE_MASTER)
				{
					iBattleMasterCount++;
				}
			}
		}

		if (aRecv->btGuildStatus == G_SUB_MASTER)
		{
			if (iSubMasterCount == 0)
			{
				GDGuildReqAssignStatus(Obj, aRecv->btType, aRecv->szTagetName, aRecv->btGuildStatus);
				return;
			}

			pMsg.btResult = GUILD_ANS_NOTEXIST_EXTRA_STATUS;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
			return;
		}
		else if (aRecv->btGuildStatus == G_BATTLE_MASTER)
		{
			if (iBattleMasterCount < (((Obj.Level + Obj.m_PlayerData->MasterLevel) / 200) + 1))
			{
				GDGuildReqAssignStatus(Obj, aRecv->btType, aRecv->szTagetName, aRecv->btGuildStatus);
				return;
			}

			pMsg.btResult = GUILD_ANS_NOTEXIST_EXTRA_STATUS;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
			return;
		}
		else if (aRecv->btGuildStatus == G_PERSON)
		{
			GDGuildReqAssignStatus(Obj, aRecv->btType, aRecv->szTagetName, aRecv->btGuildStatus);
			return;
		}
		else
		{
			return;
		}
	}

	if (aRecv->btType == 3)
	{
		GDGuildReqAssignStatus(Obj, aRecv->btType, aRecv->szTagetName, aRecv->btGuildStatus);
	}
}



void GameProtocol::CGGuildAssignType(PMSG_GUILD_ASSIGN_TYPE_REQ * aRecv, CGameObject &Obj)
{
	if (!gObjIsConnectedGP(Obj))
	{
		sLog->outBasic("error-L2 : Index %s %d", __FILE__, __LINE__);
		return;
	}

	PMSG_GUILD_ASSIGN_TYPE_RESULT pMsg = { 0 };

	pMsg.h.set((BYTE*)&pMsg, 0xE2, sizeof(pMsg));
	pMsg.btGuildType = aRecv->btGuildType;

	if (Obj.m_PlayerData->GuildNumber <= 0 || Obj.m_PlayerData->lpGuild == NULL)
	{
		pMsg.btResult = GUILD_ANS_NOTEXIST_GUILD;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
		return;
	}

	if (Obj.m_PlayerData->GuildStatus != G_MASTER)
	{
		pMsg.btResult = GUILD_ANS_NOTEXIST_PERMISSION;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
		return;
	}

	GDGuildReqAssignType(Obj, aRecv->btGuildType);
}



void GameProtocol::CGRelationShipReqJoinBreakOff(PMSG_RELATIONSHIP_JOIN_BREAKOFF_REQ * aRecv, CGameObject &Obj)
{
	if (g_CastleSiegeSync.GetCastleState() >= 5 && g_CastleSiegeSync.GetCastleState() <= 7)
	{
		MsgOutput(Obj, Lang.GetText(0, 197));
		return;
	}

	int iTargetUserIndex = MAKE_NUMBERW(aRecv->btTargetUserIndexH, aRecv->btTargetUserIndexL);
	if (!ObjectMaxRange(iTargetUserIndex))
		return;
	CGameObject* lpObjTarget = getGameObject(iTargetUserIndex);

	if (!PacketCheckTime(Obj))
		return;

	if (gObjIsConnectedGP(*lpObjTarget) == FALSE)
	{
		GCResultSend(Obj, 0x51, 3);
		return;
	}

	int iArcaBattleState = g_ArcaBattle.GetState();

	if (iArcaBattleState > 2 && iArcaBattleState < 9)
	{
		//TRACE_LOG("Obj, 0x51, 0xA5");
		GCResultSend(Obj, 0x51, 0xA4);
		return;
	}

	if (g_NewPVP.IsDuel(Obj) || g_NewPVP.IsDuel(*lpObjTarget))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 320), Obj, 1);
		return;
	}

	if (g_NewPVP.IsObserver(Obj) || g_NewPVP.IsObserver(*lpObjTarget))
	{
		this->GCServerMsgStringSend(Lang.GetText(0, 321), Obj, 1);
		return;
	}

	if (!Obj.m_PlayerData->lpGuild || !lpObjTarget->m_PlayerData->lpGuild)
	{
		GCResultSend(Obj, 0x51, 5);
		return;
	}

	if (Obj.m_PlayerData->GuildStatus != G_MASTER && lpObjTarget->m_PlayerData->GuildStatus != G_MASTER)
	{
		GCResultSend(Obj, 0x51, 4);
		return;
	}

	if (Obj.m_IfState->use > 0)
	{
		GCResultSend(Obj, 0x51, 6);
		return;
	}

	if (lpObjTarget->m_IfState->use > 0)
	{
		GCResultSend(Obj, 0x51, 6);
		return;
	}

	PMSG_RELATIONSHIP_JOIN_BREAKOFF_ANS ErrMsg = { 0 };

	PHeadSetB((BYTE*)&ErrMsg, 0xE6, sizeof(ErrMsg));
	ErrMsg.btResult = 0;
	ErrMsg.btRequestType = aRecv->btRequestType;
	ErrMsg.btRelationShipType = aRecv->btRelationShipType;
	ErrMsg.btTargetUserIndexH = aRecv->btTargetUserIndexH;
	ErrMsg.btTargetUserIndexL = aRecv->btTargetUserIndexL;

	if (Obj.m_PlayerData->GuildStatus != G_MASTER || lpTargetObj->m_PlayerData->GuildStatus != G_MASTER)
	{
		ErrMsg.btResult = GUILD_ANS_NOTEXIST_PERMISSION;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&ErrMsg, ErrMsg.h.size);
		return;
	}

	GUILD_INFO_STRUCT * lpGuildInfo = Obj.m_PlayerData->lpGuild;
	GUILD_INFO_STRUCT * lpTargetGuildInfo = getGameObject(iTargetUserIndex)->m_PlayerData->lpGuild;
	BYTE btRelationShip = gObjGetRelationShip(Obj, *lpTargetObj);

	if (aRecv->btRequestType == 1)
	{
		if (aRecv->btRelationShipType == 1)
		{
			if (lpTargetGuildInfo->Count < g_ConfigRead.data.common.AllianceMinGuildMember)
			{
				ErrMsg.btResult = GUILD_ANS_CANNOT_BE_UNION_MASTER;
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&ErrMsg, ErrMsg.h.size);
				return;
			}

			if (lpTargetGuildInfo->iGuildUnion != 0 && lpTargetGuildInfo->iGuildUnion == lpTargetGuildInfo->Number)	// #error
			{
				if (UnionManager.GetGuildRelationShipCount(lpTargetGuildInfo->iGuildUnion, 1) >= g_ConfigRead.data.common.AllianceMaxGuilds)
				{
					ErrMsg.btResult = GUILD_ANS_EXCEED_MAX_UNION_MEMBER;
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&ErrMsg, ErrMsg.h.size);
					return;
				}
			}

			char szCastleOwnerGuildName[MAX_GUILD_LEN + 1] = { 0 };
			std::memcpy(szCastleOwnerGuildName, g_CastleSiegeSync.GetCastleOwnerGuild(), MAX_GUILD_LEN);

			if (!strcmp(lpGuildInfo->Name, szCastleOwnerGuildName))
			{
				ErrMsg.btResult = GUILD_ANS_EXIST_UNION;
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&ErrMsg, ErrMsg.h.size);
				return;
			}

			if (lpGuildInfo->iGuildUnion)
			{
				ErrMsg.btResult = GUILD_ANS_EXIST_UNION;
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&ErrMsg, ErrMsg.h.size);
				return;
			}

			if (btRelationShip == 2)
			{
				ErrMsg.btResult = GUILD_ANS_EXIST_RELATIONSHIP_RIVAL;
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&ErrMsg, ErrMsg.h.size);
				return;
			}

			if (lpTargetGuildInfo->iGuildUnion && lpTargetGuildInfo->Number != lpTargetGuildInfo->iGuildUnion)	// #error???
			{
				ErrMsg.btResult = GUIDL_ANS_NOT_UNION_MASTER;
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&ErrMsg, ErrMsg.h.size);
				return;
			}

			int UnionMasterInfluence = g_GensSystem.GetGensInfluence(*lpTargetObj);
			int GuildMasterInfluence = g_GensSystem.GetGensInfluence(Obj);

			if (UnionMasterInfluence == NONE_INFLUENCE)
			{
				ErrMsg.btResult = GUILD_ANS_NOT_UNION_MASTER_GENS;
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&ErrMsg, ErrMsg.h.size);
				return;
			}

			if (GuildMasterInfluence == NONE_INFLUENCE)
			{
				ErrMsg.btResult = GUILD_ANS_NOT_GENS;
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&ErrMsg, ErrMsg.h.size);
				return;
			}

			if (UnionMasterInfluence != GuildMasterInfluence && g_GensSystem.CanGensJoinUnionWhileOppositeGens() == FALSE)
			{
				ErrMsg.btResult = GUILD_ANS_WRONG_GENS;
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&ErrMsg, ErrMsg.h.size);
				return;
			}

		}
		else if (aRecv->btRelationShipType == 2)
		{
			/*
			if ( lpGuildInfo->iGuildRival || lpTargetGuildInfo->iGuildRival )
			{
				ErrMsg.btResult = GUILD_ANS_EXIST_RIVAL;
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&ErrMsg, ErrMsg.h.size);
				return;
			}*/

			if (btRelationShip == 1)
			{
				ErrMsg.btResult = GUILD_ANS_EXIST_RELATIONSHIP_UNION;
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&ErrMsg, ErrMsg.h.size);
				return;
			}
		}
	}
	else if (aRecv->btRequestType == 2)
	{
		if (aRecv->btRelationShipType == 1)
		{
			if (lpGuildInfo->iGuildUnion == 0)
			{
				ErrMsg.btResult = GUILD_ANS_NOTEXIST_UNION;
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&ErrMsg, ErrMsg.h.size);
				return;
			}
		}
		else if (aRecv->btRelationShipType == 2)
		{
			if (!lpGuildInfo->iGuildRival || !lpTargetGuildInfo->iGuildRival)
			{
				ErrMsg.btResult = GUILD_ANS_NOTEXIST_UNION;
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&ErrMsg, ErrMsg.h.size);
				return;
			}

			if (lpGuildInfo->iGuildRival != lpTargetGuildInfo->Number || lpGuildInfo->Number != lpTargetGuildInfo->iGuildRival)
			{
				ErrMsg.btResult = GUILD_ANS_NOT_GUILD_RIVAL;
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&ErrMsg, ErrMsg.h.size);
				return;
			}
		}
	}

	if (aRecv->btRequestType == 2)
	{
		if (aRecv->btRelationShipType == 1)
		{
			GDUnionBreakOff(Obj, lpGuildInfo->iGuildUnion);
			return;
		}
	}

	Obj.m_IfState->use = 1;
	Obj.m_IfState->type = 14;
	Obj.m_IfState->state = 0;
	Obj.m_InterfaceTime = GetTickCount();

	lpTargetObj.m_IfState->use = 1;
	lpTargetObj.m_IfState->type = 14;
	lpTargetObj.m_IfState->state = 0;
	lpTargetObj.m_InterfaceTime = GetTickCount();

	PMSG_RELATIONSHIP_JOIN_BREAKOFF_REQ pMsg = { 0 };

	PHeadSetB((BYTE*)&pMsg, 0xE5, sizeof(pMsg));
	pMsg.btTargetUserIndexL = SET_NUMBERL(Obj.m_Index);
	pMsg.btTargetUserIndexH = SET_NUMBERH(Obj.m_Index);
	pMsg.btRequestType = aRecv->btRequestType;
	pMsg.btRelationShipType = aRecv->btRelationShipType;

	IOCP.DataSend(iTargetUserIndex, (BYTE*)&pMsg, pMsg.h.size);
}



void GameProtocol::CGRelationShipAnsJoinBreakOff(PMSG_RELATIONSHIP_JOIN_BREAKOFF_ANS * aRecv, CGameObject &Obj)
{
	int iTargetUserIndex = MAKE_NUMBERW(aRecv->btTargetUserIndexH, aRecv->btTargetUserIndexL);

	if (!ObjectMaxRange(iTargetUserIndex))
		return;

	if (!PacketCheckTime(Obj))
		return;

	if (gObjIsConnected(*getGameObject(iTargetUserIndex)) == FALSE)
	{
		GCResultSend(Obj, 0x51, 3);
		return;
	}

	if (aRecv->btResult == 1)
	{
		if (aRecv->btRequestType == 1)
		{
			GDRelationShipReqJoin(iTargetUserIndex, Obj, aRecv->btRelationShipType);
		}
		else if (aRecv->btRequestType == 2)
		{
			GDRelationShipReqBreakOff(Obj, iTargetUserIndex, aRecv->btRelationShipType);
		}
	}
	else
	{
		PMSG_RELATIONSHIP_JOIN_BREAKOFF_ANS ErrMsg = { 0 };

		PHeadSetB((BYTE*)&ErrMsg, 0xE6, sizeof(ErrMsg));
		ErrMsg.btResult = 0;
		ErrMsg.btRequestType = aRecv->btRequestType;
		ErrMsg.btRelationShipType = aRecv->btRelationShipType;
		ErrMsg.btTargetUserIndexH = aRecv->btTargetUserIndexH;
		ErrMsg.btTargetUserIndexL = aRecv->btTargetUserIndexL;
		ErrMsg.btResult = GUILD_ANS_CANCLE_REQUEST;

		IOCP.DataSend(iTargetUserIndex, (BYTE*)&ErrMsg, ErrMsg.h.size);
	}

	if (Obj.m_IfState->use)
	{
		if (Obj.m_IfState->type == 14)
		{
			Obj.m_IfState->use = 0;
		}
	}

	if (getGameObject(iTargetUserIndex)->m_IfState->use)
	{
		if (getGameObject(iTargetUserIndex)->m_IfState->type == 14)
		{
			getGameObject(iTargetUserIndex)->m_IfState->use = 0;
		}
	}
}



void GameProtocol::CGUnionList(PMSG_UNIONLIST_REQ * aRecv, CGameObject &Obj)
{
	if (!PacketCheckTime(Obj))
		return;

	if (gObjIsConnected(Obj) == FALSE)
	{
		GCResultSend(Obj, 0x51, 3);
		return;
	}

	if (Obj.m_PlayerData->lpGuild == NULL)
	{
		GCResultSend(Obj, 0x51, 3);
		return;
	}

	if (Obj.m_PlayerData->lpGuild->iGuildUnion == 0)
	{
		GCResultSend(Obj, 0x51, 3);
		return;
	}

	GDUnionListSend(Obj, Obj.m_PlayerData->lpGuild->iGuildUnion);

}


void GameProtocol::CGRelationShipReqKickOutUnionMember(PMSG_KICKOUT_UNIONMEMBER_REQ* aRecv, CGameObject &Obj)
{
	if (this->PacketCheckTime(lpObj) == FALSE)
		return;

	if (lpObj == NULL)
		return;

	if (gObjIsConnected(lpObj) == FALSE)
	{
		GCResultSend(lpObj, 0x51, 3);
		MsgOutput(lpObj, Lang.GetText(0, 529));
		return;
	}

	if (Obj.m_PlayerData->lpGuild == NULL)
	{
		GCResultSend(lpObj, 0x51, 3);
		MsgOutput(Obj, Lang.GetText(0, 530));
		return;
	}

	PMSG_RELATIONSHIP_JOIN_BREAKOFF_ANS ErrMsg = { 0 };

	PHeadSetB((BYTE*)&ErrMsg, 0xE6, sizeof(ErrMsg));
	ErrMsg.btResult = 0;
	ErrMsg.btRequestType = 2;
	ErrMsg.btRelationShipType = 1;
	ErrMsg.btTargetUserIndexH = 0;
	ErrMsg.btTargetUserIndexL = 0;

	if (Obj.m_PlayerData->lpGuild->iGuildUnion == 0)
	{
		ErrMsg.btResult = GUILD_ANS_NOTEXIST_UNION;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&ErrMsg, ErrMsg.h.size);

		return;
	}

	int iArcaBattleState = g_ArcaBattle.GetState();

	if (iArcaBattleState > 2 && iArcaBattleState < 9)
	{
		ErrMsg.btResult = GUILD_ANS_ARCA_BATTLE_ONGOING;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&ErrMsg, sizeof(ErrMsg));
		return;
	}

	if (Obj.m_PlayerData->GuildStatus != GUILD_MASTER || Obj.m_PlayerData->lpGuild->Number != Obj.m_PlayerData->lpGuild->iGuildUnion)
	{
		ErrMsg.btResult = GUILD_ANS_NOTEXIST_PERMISSION;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&ErrMsg, ErrMsg.h.size);

		return;
	}

	char szUnionMemberGuildName[MAX_GUILD_LEN + 1] = { 0 };
	std::memcpy(szUnionMemberGuildName, aRecv->szTargetGuildName, MAX_GUILD_LEN);

	if (!strcmp(Obj.m_PlayerData->lpGuild->Name, szUnionMemberGuildName))
	{
		ErrMsg.btResult = GUILD_ANS_NOTEXIST_PERMISSION;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&ErrMsg, ErrMsg.h.size);

		return;
	}

	GDRelationShipReqKickOutUnionMember(Obj, szUnionMemberGuildName);
}



void GameProtocol::CGReqMapSvrAuth(PMSG_REQ_MAPSERVERAUTH * lpMsg, CGameObject &Obj)
{
	if (lpMsg->btCliVersion[0] != szClientVersion[0] ||
		lpMsg->btCliVersion[1] != szClientVersion[1] ||
		lpMsg->btCliVersion[2] != szClientVersion[2] ||
		lpMsg->btCliVersion[3] != szClientVersion[3] ||
		lpMsg->btCliVersion[4] != szClientVersion[4])
	{
		//GCAnsMapSvrAuth(iIndex, 6);
		//IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);

		//return;
	}

	char id[MAX_ACCOUNT_LEN + 1];
	id[MAX_ACCOUNT_LEN] = 0;
	std::memcpy(id, lpMsg->szAccountID, MAX_ACCOUNT_LEN);
	BuxConvert(id, MAX_ACCOUNT_LEN);

	/*
		BYTE btSerial[17];

		btSerial[16]=0;
		std::memcpy(btSerial, lpMsg->btCliSerial, sizeof(lpMsg->btCliSerial));

		if ( strcmp((char*)btSerial, szGameServerExeSerial) )
		{
			sLog->outError(  "error-L1: Serial error [%s] [%s]", id, btSerial);
			GCAnsMapSvrAuth(iIndex, 6);
			IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);

			return;
		}*/

	if (g_ConfigRead.server.IsConnectMemberUse() == TRUE)
	{
		if (ConMember.IsMember(id) == FALSE)
		{
			GCAnsMapSvrAuth(iIndex, 2);

			return;
		}
	}

	

	if (PacketCheckTime(Obj) == FALSE)
	{
		sLog->outBasic("error-L2: Login time is over [%d][%s]", iIndex, id);
		IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);

		return;
	}

	if (Obj.Connected != PLAYER_CONNECTED)
	{
		sLog->outBasic("error-L2: Authentication is allowed after connected [%d][%s]", iIndex, id);
		IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);

		return;
	}

	if (Obj.LoginMsgSnd != 0)
	{
		if (Obj.LoginMsgSnd == 1)
		{
			sLog->outBasic("error: It is after authentication information is sended [%d][%s][%s]", iIndex, id, Obj.m_PlayerData->ConnectUser->IP);
		}
		else
		{
			sLog->outBasic("error: %d %s %d", Obj.LoginMsgSnd, __FILE__, __LINE__);
		}

		return;
	}

	Obj.CheckTick = lpMsg->iTickCount;
	Obj.CheckTick2 = GetTickCount();
	Obj.ConnectCheckTime = GetTickCount();
	Obj.CheckSpeedHack = true;
	Obj.ConnectCheckTime = GetTickCount();
	Obj.LoginMsgSnd = 1;
	Obj.LoginMsgCount++;
	Obj.AccountID[MAX_ACCOUNT_LEN] = 0;
	std::memcpy(Obj.AccountID, id, MAX_ACCOUNT_LEN);
	std::memset(Obj.Name, 0, MAX_ACCOUNT_LEN);
	std::memcpy(Obj.m_MapServerAuthInfo.szCharName, lpMsg->szCharName, sizeof(lpMsg->szCharName));
	Obj.m_MapServerAuthInfo.iJA1 = lpMsg->iJoinAuthCode1;
	Obj.m_MapServerAuthInfo.iJA2 = lpMsg->iJoinAuthCode2;
	Obj.m_MapServerAuthInfo.iJA3 = lpMsg->iJoinAuthCode3;
	Obj.m_MapServerAuthInfo.iJA4 = lpMsg->iJoinAuthCode4;
	Obj.m_cAccountItemBlock = 0;
	Obj.m_PlayerData->VipType = 0;
	Obj.m_PlayerData->VipEffect = VIP_EFFECT_NONE;
	Obj.m_PlayerData->AntiHackCheckTime = GetTickCount();
	Obj.m_PlayerData->dwLastHitHackTick = GetTickCount64();
	gObjAddMsgSendDelay(lpObj, 1001, iIndex, 5000, 0);
}

struct PMSG_ANS_MAPSERVERAUTH
{
	PBMSG_HEAD2 h;	// C1:B1:01
	BYTE iResult;	// 4
};

void GameProtocol::GCAnsMapSvrAuth(CGameObject &Obj, int iResult)
{
	PMSG_ANS_MAPSERVERAUTH pMsgResult;

	PHeadSubSetB((BYTE*)&pMsgResult, 0xB1, 0x01, sizeof(pMsgResult));
	pMsgResult.iResult = iResult;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::CGReqCastleSiegeState(PMSG_REQ_CASTLESIEGESTATE * lpMsg, CGameObject &Obj)
{
	GS_GDReqOwnerGuildMaster(g_MapServerManager.GetMapSvrGroup(), iIndex);
}

void GameProtocol::GCAnsCastleSiegeState(CGameObject &Obj, int iResult, LPSTR lpszGuildName, LPSTR lpszGuildMaster)
{
	if ((lpszGuildName == NULL) || (lpszGuildMaster == NULL))
	{
		return;
	}

	PMSG_ANS_CASTLESIEGESTATE pMsgResult;

	pMsgResult.h.set((BYTE*)&pMsgResult.h, 0xB2, 0x00, sizeof(pMsgResult));

	pMsgResult.btResult = iResult;

	std::memcpy(pMsgResult.cOwnerGuild, lpszGuildName, sizeof(pMsgResult.cOwnerGuild));
	std::memcpy(pMsgResult.cOwnerGuildMaster, lpszGuildMaster, sizeof(pMsgResult.cOwnerGuildMaster));

	SYSTEMTIME tmStateStartDate = { 0 };
	SYSTEMTIME tmStateEndDate = { 0 };
	SYSTEMTIME tmSiegeStartDate = { 0 };

	int iCastleState = g_CastleSiege.GetCastleState();
	int bRETVAL = g_CastleSiege.GetCastleStateTerm(&tmStateStartDate, &tmStateEndDate);
	tmSiegeStartDate = g_CastleSiege.GetCastleLeftSiegeDate();

	if (bRETVAL == FALSE)
	{
		iCastleState = -1;
	}

	int iStateLeftSec = g_CastleSiege.GetCurRemainSec();

	// Create Packet
	pMsgResult.cCastleSiegeState = iCastleState;
	// Start Date
	pMsgResult.btStartYearH = SET_NUMBERH(tmStateStartDate.wYear);
	pMsgResult.btStartYearL = SET_NUMBERL(tmStateStartDate.wYear);
	pMsgResult.btStartMonth = tmStateStartDate.wMonth;
	pMsgResult.btStartDay = tmStateStartDate.wDay;
	pMsgResult.btStartHour = tmStateStartDate.wHour;
	pMsgResult.btStartMinute = tmStateStartDate.wMinute;
	// End Date
	pMsgResult.btEndYearH = SET_NUMBERH(tmStateEndDate.wYear);
	pMsgResult.btEndYearL = SET_NUMBERL(tmStateEndDate.wYear);
	pMsgResult.btEndMonth = tmStateEndDate.wMonth;
	pMsgResult.btEndDay = tmStateEndDate.wDay;
	pMsgResult.btEndHour = tmStateEndDate.wHour;
	pMsgResult.btEndMinute = tmStateEndDate.wMinute;
	// Siege Start Date
	pMsgResult.btSiegeStartYearH = SET_NUMBERH(tmSiegeStartDate.wYear);
	pMsgResult.btSiegeStartYearL = SET_NUMBERL(tmSiegeStartDate.wYear);
	pMsgResult.btSiegeStartMonth = tmSiegeStartDate.wMonth;
	pMsgResult.btSiegeStartDay = tmSiegeStartDate.wDay;
	pMsgResult.btSiegeStartHour = tmSiegeStartDate.wHour;
	pMsgResult.btSiegeStartMinute = tmSiegeStartDate.wMinute;
	// Time Left
	pMsgResult.btStateLeftSec1 = SET_NUMBERH(SET_NUMBERHW(iStateLeftSec));
	pMsgResult.btStateLeftSec2 = SET_NUMBERL(SET_NUMBERHW(iStateLeftSec));
	pMsgResult.btStateLeftSec3 = SET_NUMBERH(SET_NUMBERLW(iStateLeftSec));
	pMsgResult.btStateLeftSec4 = SET_NUMBERL(SET_NUMBERLW(iStateLeftSec));

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::CGReqRegCastleSiege(PMSG_REQ_REGCASTLESIEGE * lpMsg, CGameObject &Obj)
{
	if (g_CastleSiege.GetCastleState() != CASTLESIEGE_STATE_REGSIEGE)
	{
		GCAnsRegCastleSiege(iIndex, 7, "");
		return;
	}
	if (strcmp(Obj.m_PlayerData->GuildName, "") == 0)
	{
		GCAnsRegCastleSiege(iIndex, 6, "");
		return;
	}
	if (g_CastleSiege.CheckUnionGuildMaster(Obj) == FALSE)
	{
		GCAnsRegCastleSiege(iIndex, 0, "");
	}
	else if ((g_CastleSiege.CheckCastleOwnerMember(Obj) != FALSE) ||
		(g_CastleSiege.CheckCastleOwnerUnionMember(Obj) != FALSE))
	{
		GCAnsRegCastleSiege(iIndex, 3, "");
	}
	else
	{
		GS_GDReqRegAttackGuild(g_MapServerManager.GetMapSvrGroup(), iIndex);
	}
}

void GameProtocol::GCAnsRegCastleSiege(CGameObject &Obj, int iResult, LPSTR lpszGuildName)
{
	PMSG_ANS_REGCASTLESIEGE pMsgResult;

	if (lpszGuildName == NULL)
	{
		return;
	}

	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x01, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	std::memcpy(&pMsgResult.szGuildName, lpszGuildName, sizeof(pMsgResult.szGuildName));

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsgResult, pMsgResult.h.size);

	if (::gObjIsConnected(Obj))
	{
		sLog->outBasic("[CastleSiege] [%s][%s] Registered Castle Siege (GUILD:%s) - Result:%d", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, iResult);
	}
}

void GameProtocol::CGReqGiveUpCastleSiege(PMSG_REQ_GIVEUPCASTLESIEGE * lpMsg, CGameObject &Obj)
{
	if ((g_CastleSiege.GetCastleState() < CASTLESIEGE_STATE_REGSIEGE) || (g_CastleSiege.GetCastleState() > CASTLESIEGE_STATE_REGMARK)) //Fixed
	{
		GCAnsGiveUpCastleSiege(iIndex, 3, 0, 0, "");
		return;
	}

	if (strcmp(Obj.m_PlayerData->GuildName, "") == 0)
	{
		return;
	}

	if (g_CastleSiege.CheckUnionGuildMaster(Obj) == FALSE)
	{
		return;
	}

	GS_GDReqGuildSetGiveUp(g_MapServerManager.GetMapSvrGroup(), iIndex, lpMsg->btGiveUp);
}

void GameProtocol::GCAnsGiveUpCastleSiege(CGameObject &Obj, int iResult, int bGiveUp, int iMarkCount, LPSTR lpszGuildName)
{
	PMSG_ANS_GIVEUPCASTLESIEGE pMsgResult;

	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x02, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.btIsGiveUp = bGiveUp;

	std::memcpy(&pMsgResult.szGuildName, lpszGuildName, sizeof(pMsgResult.szGuildName));

	if ((pMsgResult.btResult == 1) && (iMarkCount > 0))
	{
		Obj.m_PlayerData->Money += (iMarkCount * 3000);

		if (Obj.m_PlayerData->Money > MAX_ZEN)
		{
			Obj.m_PlayerData->Money = MAX_ZEN;
		}
		GCMoneySend(iIndex, Obj.m_PlayerData->Money);
	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsgResult, pMsgResult.h.size);

	if (gObjIsConnected(Obj))
	{
		sLog->outBasic("[CastleSiege] [%s][%s] GiveUp Castle Siege (GUILD:%s) - Result:%d", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, iResult);
	}
}

void GameProtocol::CGReqGuildRegInfo(PMSG_REQ_GUILDREGINFO * lpMsg, CGameObject &Obj)
{
	if (lpMsg == NULL)
	{
		return;
	}

	if (strcmp(Obj.m_PlayerData->GuildName, "") == 0)
	{
		return;
	}

	GS_GDReqGuildMarkRegInfo(g_MapServerManager.GetMapSvrGroup(), iIndex);
}

void GameProtocol::GCAnsGuildRegInfo(CGameObject &Obj, int iResult, CSP_ANS_GUILDREGINFO* lpMsgResult)
{
	PMSG_ANS_GUILDREGINFO pMsgResult;

	if (lpMsgResult == NULL)
	{
		return;
	}

	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 3, sizeof(pMsgResult));

	pMsgResult.btResult = iResult;
	pMsgResult.btGuildMark1 = SET_NUMBERH(SET_NUMBERHW(lpMsgResult->iRegMarkCount));
	pMsgResult.btGuildMark2 = SET_NUMBERL(SET_NUMBERHW(lpMsgResult->iRegMarkCount));
	pMsgResult.btGuildMark3 = SET_NUMBERH(SET_NUMBERLW(lpMsgResult->iRegMarkCount));
	pMsgResult.btGuildMark4 = SET_NUMBERL(SET_NUMBERLW(lpMsgResult->iRegMarkCount));
	pMsgResult.btRegRank = lpMsgResult->btRegRank;
	pMsgResult.btIsGiveUp = lpMsgResult->bIsGiveUp;
	std::memcpy(&pMsgResult.szGuildName, lpMsgResult->szGuildName, sizeof(pMsgResult.szGuildName));

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::CGReqRegGuildMark(PMSG_REQ_REGGUILDMARK * lpMsg, CGameObject &Obj)
{
	if (lpMsg == NULL)
	{
		return;
	}

	if (Obj.UseEventServer == TRUE)
	{
		return;
	}

	Obj.UseEventServer = TRUE;

	if (g_CastleSiege.GetCastleState() != CASTLESIEGE_STATE_REGMARK)
	{
		Obj.UseEventServer = FALSE;
		return;
	}
	if (strcmp(Obj.m_PlayerData->GuildName, "") == 0)
	{
		Obj.UseEventServer = FALSE;
		return;
	}

	if (CHECK_LIMIT(lpMsg->btItemPos, 64) == FALSE)
	{
		PMSG_ANS_REGGUILDMARK pMsgResult = { 0 };
		pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x04, sizeof(pMsgResult));
		pMsgResult.btResult = 3;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
		Obj.UseEventServer = FALSE;
	}
	else
	{
		int iItemPos = (lpMsg->btItemPos + 12);

		if (Obj.pntInventory[iItemPos]->IsItem() == FALSE)
		{
			PMSG_ANS_REGGUILDMARK pMsgResult = { 0 };
			pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x04, sizeof(pMsgResult));
			pMsgResult.btResult = 3;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
			Obj.UseEventServer = FALSE;
		}
		else
		{
			if (Obj.pntInventory[iItemPos]->m_Type != ITEMGET(14, 21) ||
				Obj.pntInventory[iItemPos]->m_Level != 3 ||
				Obj.pntInventory[iItemPos]->m_Durability <= 0.0f)
			{
				PMSG_ANS_REGGUILDMARK pMsgResult = { 0 };
				pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x04, sizeof(pMsgResult));
				pMsgResult.btResult = 3;
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
				Obj.UseEventServer = FALSE;
			}
			else
			{
				GS_GDReqRegGuildMark(g_MapServerManager.GetMapSvrGroup(), iIndex, iItemPos);
			}
		}
	}
}

void GameProtocol::GCAnsRegGuildMark(CGameObject &Obj, int iResult, CSP_ANS_GUILDREGMARK* lpMsgResult)
{
	if (Obj.pntInventory[lpMsgResult->iItemPos]->IsItem() == FALSE)
	{
		PMSG_ANS_REGGUILDMARK pMsgResult = { 0 };
		pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x04, sizeof(pMsgResult));
		pMsgResult.btResult = 3;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
		Obj.UseEventServer = 0;
	}
	else
	{
		if (Obj.pntInventory[lpMsgResult->iItemPos]->m_Type != ITEMGET(14, 21) &&
			Obj.pntInventory[lpMsgResult->iItemPos]->m_Level == 3 ||
			Obj.pntInventory[lpMsgResult->iItemPos]->m_Durability <= 0.0f)
		{
			PMSG_ANS_REGGUILDMARK pMsgResult = { 0 };
			pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x04, sizeof(pMsgResult));
			pMsgResult.btResult = 3;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
			Obj.UseEventServer = FALSE;
		}
		else
		{
			if ((Obj.pntInventory[lpMsgResult->iItemPos]->m_Durability -= 1.0) > 0.0f)
			{
				GCItemObjectDurSend(iIndex, lpMsgResult->iItemPos, Obj.pntInventory[lpMsgResult->iItemPos]->m_Durability, 0);
			}
			else
			{
				gObjInventoryDeleteItem(iIndex, lpMsgResult->iItemPos);
				GCInventoryItemDeleteSend(iIndex, lpMsgResult->iItemPos, 1);
			}

			PMSG_ANS_REGGUILDMARK pMsgResult;
			pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x04, sizeof(pMsgResult));

			pMsgResult.btResult = iResult;
			pMsgResult.btGuildMark1 = SET_NUMBERH(SET_NUMBERHW(lpMsgResult->iRegMarkCount));
			pMsgResult.btGuildMark2 = SET_NUMBERL(SET_NUMBERHW(lpMsgResult->iRegMarkCount));
			pMsgResult.btGuildMark3 = SET_NUMBERH(SET_NUMBERLW(lpMsgResult->iRegMarkCount));
			pMsgResult.btGuildMark4 = SET_NUMBERL(SET_NUMBERLW(lpMsgResult->iRegMarkCount));
			std::memcpy(&pMsgResult.szGuildName, lpMsgResult->szGuildName, sizeof(pMsgResult.szGuildName));
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
			Obj.UseEventServer = 0;

		}
	}
}

void GameProtocol::CGReqNpcBuy(PMSG_REQ_NPCBUY * lpMsg, CGameObject &Obj)
{
	if (lpMsg == NULL)
	{
		return;
	}

	if (::gObjIsConnected(Obj) == FALSE)
	{
		return;
	}

	if (strcmp(Obj.m_PlayerData->GuildName, "") == 0)
	{
		return;
	}

	if (g_CastleSiege.CheckGuildOwnCastle(Obj.m_PlayerData->GuildName) == FALSE || (Obj.m_PlayerData->GuildStatus != G_MASTER && Obj.m_PlayerData->GuildStatus != G_SUB_MASTER))
	{
		sLog->outError( "[CastleSiege] CGReqNpcBuy() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus, lpMsg->iNpcNumber, lpMsg->iNpcIndex);
	}
	else
	{
		BYTE btResult = 0;

		BOOL bRETVAL = g_CastleSiege.CheckAddDbNPC(iIndex, lpMsg->iNpcNumber, lpMsg->iNpcIndex, btResult);

		if (bRETVAL == FALSE)
		{
			GCAnsNpcBuy(iIndex, btResult, lpMsg->iNpcNumber, lpMsg->iNpcIndex);
			sLog->outError( "[CastleSiege] CGReqNpcBuy() ERROR - CCastleSiege::CheckAddDbNPC() [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus, lpMsg->iNpcNumber, lpMsg->iNpcIndex);
		}
		else
		{
			sLog->outError( "[CastleSiege] CGReqNpcBuy() OK - [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus, lpMsg->iNpcNumber, lpMsg->iNpcIndex);
		}
	}
}

void GameProtocol::GCAnsNpcBuy(CGameObject &Obj, int iResult, int iNpcNumber, int iNpcIndex)
{
	PMSG_ANS_NPCBUY pMsgResult;

	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x05, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.iNpcNumber = iNpcNumber;
	pMsgResult.iNpcIndex = iNpcIndex;
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::CGReqNpcRepair(PMSG_REQ_NPCREPAIR * lpMsg, CGameObject &Obj)
{
	if (lpMsg == NULL)
	{
		return;
	}

	if (::gObjIsConnected(Obj) == FALSE)
	{
		return;
	}

	if (strcmp(Obj.m_PlayerData->GuildName, "") == 0)
	{
		return;
	}

	if ((g_CastleSiege.CheckGuildOwnCastle(Obj.m_PlayerData->GuildName) == FALSE) || (Obj.m_PlayerData->GuildStatus != G_MASTER) && (Obj.m_PlayerData->GuildStatus != G_SUB_MASTER))
	{
		sLog->outError( "[CastleSiege] CGReqNpcRepair() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus);
		return;
	}

	_CS_NPC_DATA pNpcData;
	BOOL bRETVAL;
	int iNpcIndex;
	int iRepairCost;

	bRETVAL = g_CastleSiege.GetNpcData(lpMsg->iNpcNumber, lpMsg->iNpcIndex, pNpcData);

	if (bRETVAL == FALSE)
	{
		GCAnsNpcRepair(iIndex, 0, lpMsg->iNpcNumber, lpMsg->iNpcIndex, 0, 0);
		sLog->outError( "[CastleSiege] CGReqNpcRepair() ERROR - CL Request Fail [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus, lpMsg->iNpcNumber, lpMsg->iNpcIndex);
		return;
	}
	else
	{
		iNpcIndex = pNpcData.m_iNPC_OBJINDEX;

		if (gObjIsConnected(iNpcIndex) == FALSE)
		{
			GCAnsNpcRepair(iIndex, 0, lpMsg->iNpcNumber, lpMsg->iNpcIndex, 0, 0);
			sLog->outError( "[CastleSiege] CGReqNpcRepair() ERROR - Npc Alive Data Mismatch [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus, pNpcData.m_iNPC_NUM, pNpcData.m_iNPC_INDEX);
			return;
		}
		else if (getGameObject(iNpcIndex)->Live == 0)
		{
			GCAnsNpcRepair(iIndex, 0, lpMsg->iNpcNumber, lpMsg->iNpcIndex, 0, 0);
			sLog->outError( "[CastleSiege] CGReqNpcRepair() ERROR - Npc Alive Data Mismatch [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus, pNpcData.m_iNPC_NUM, pNpcData.m_iNPC_INDEX);
			return;
		}
		else
		{
			if ((getGameObject(iNpcIndex)->MaxLife == getGameObject(iNpcIndex)->Life) || (getGameObject(iNpcIndex)->MaxLife < getGameObject(iNpcIndex)->Life))
			{
				getGameObject(iNpcIndex)->Life = getGameObject(iNpcIndex)->MaxLife;
				GCAnsNpcRepair(iIndex, 0, lpMsg->iNpcNumber, lpMsg->iNpcIndex, 0, 0);
				return;
			}
			else
			{
				iRepairCost = 0;

				switch (lpMsg->iNpcNumber)
				{
				case 277:
					iRepairCost = float((getGameObject(iNpcIndex)->MaxLife - getGameObject(iNpcIndex)->Life) * 0.5f) + float(getGameObject(iNpcIndex)->m_btCsNpcDfLevel * 1000000);
					break;
				case 283:
					iRepairCost = (getGameObject(iNpcIndex)->MaxLife - getGameObject(iNpcIndex)->Life)* 0.3f + float((getGameObject(iNpcIndex)->m_btCsNpcDfLevel + getGameObject(iNpcIndex)->m_btCsNpcRgLevel) * 1000000);
					break;
				default:
					GCAnsNpcRepair(iIndex, 0, lpMsg->iNpcNumber, lpMsg->iNpcIndex, 0, 0);
					return;
				}

				if (Obj.m_PlayerData->Money < iRepairCost)
				{
					GCAnsNpcRepair(iIndex, 3, lpMsg->iNpcNumber, lpMsg->iNpcIndex, 0, 0);
					sLog->outError( "[CastleSiege] CGReqNpcRepair() ERROR - Money isn't enough [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus, pNpcData.m_iNPC_NUM, pNpcData.m_iNPC_INDEX);
					return;
				}
				else
				{
					sLog->outError( "[CastleSiege] CGReqNpcRepair() OK - [%s][%s], Guild:(%s)(%d), Npc:(CLS:%d, IDX:%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus, pNpcData.m_iNPC_NUM, pNpcData.m_iNPC_INDEX);
					GS_GDReqCastleNpcRepair(g_MapServerManager.GetMapSvrGroup(), iIndex, lpMsg->iNpcNumber, lpMsg->iNpcIndex, iRepairCost);
					//return;
				}
			}
		}
	}
}

void GameProtocol::GCAnsNpcRepair(CGameObject &Obj, int iResult, int iNpcNumber, int iNpcIndex, int iNpcHP, int iNpcMaxHP)
{
	PMSG_ANS_NPCREPAIR pMsgResult;

	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x06, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.iNpcNumber = iNpcNumber;
	pMsgResult.iNpcIndex = iNpcIndex;
	pMsgResult.iNpcHP = iNpcHP;
	pMsgResult.iNpcMaxHP = iNpcMaxHP;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}



void GameProtocol::CGReqNpcUpgrade(PMSG_REQ_NPCUPGRADE * lpMsg, CGameObject &Obj)
{
	if (lpMsg == NULL)
	{
		return;
	}

	if (::gObjIsConnected(Obj) == FALSE)
	{
		return;
	}

	int iNEED_GEMOFDEFEND = 0;
	int	iNEED_MONEY = 0;
	int	iCUR_UPLVL = 0;
	int	iNXT_UPLVL = 0;
	int	iNXT_UPVALUE = 0;
	int	bENABLE_UPGRADE = FALSE;

	if (strcmp(Obj.m_PlayerData->GuildName, "") == 0)
		return;

	if ((g_CastleSiege.CheckGuildOwnCastle(Obj.m_PlayerData->GuildName) == FALSE) || (Obj.m_PlayerData->GuildStatus != G_MASTER && Obj.m_PlayerData->GuildStatus != G_SUB_MASTER))
	{
		sLog->outBasic("[CastleSiege] CGReqNpcUpgrade() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus);
		return;
	}

	switch (lpMsg->iNpcNumber)
	{
	case 277://Castle Gate
	{
		_CS_NPC_DATA m_NpcData;
		BOOL bResult = g_CastleSiege.GetNpcData(lpMsg->iNpcNumber, lpMsg->iNpcIndex, m_NpcData);

		if (bResult)
		{
			switch (lpMsg->iNpcUpType)
			{
			case 1:
			{
				iCUR_UPLVL = m_NpcData.m_iNPC_DF_LEVEL;
				iNXT_UPLVL = lpMsg->iNpcUpValue;

				if ((iNXT_UPLVL >= 1) && (iNXT_UPLVL <= 3) && (iNXT_UPLVL == (iCUR_UPLVL + 1)))
				{
					bENABLE_UPGRADE = TRUE;
					iNXT_UPVALUE = iNXT_UPLVL;
					iNEED_GEMOFDEFEND = g_iNpcUpDfLevel_CGATE[(iNXT_UPLVL - 1)][1];
					iNEED_MONEY = g_iNpcUpDfLevel_CGATE[(iNXT_UPLVL - 1)][2];
				}
			}
			break;

			case 3:
			{
				iCUR_UPLVL = 0;

				for (int i = 0; i < 3; i++)
				{
					if (m_NpcData.m_iNPC_MAXHP >= *g_iNpcUpMaxHP_CGATE[i])
					{
						iCUR_UPLVL = (i + 1);
					}
				}
				iNXT_UPLVL = 0;

				for (int i = 0; i < 3; i++)
				{
					if (lpMsg->iNpcUpValue >= *g_iNpcUpMaxHP_CGATE[i])
					{
						iNXT_UPLVL = (i + 1);
					}
				}

				if ((iNXT_UPLVL >= 1) && (iNXT_UPLVL <= 3) && (iNXT_UPLVL == (iCUR_UPLVL + 1)))
				{
					bENABLE_UPGRADE = TRUE;
					iNXT_UPVALUE = g_iNpcUpMaxHP_CGATE[(iNXT_UPLVL - 1)][0];
					iNEED_GEMOFDEFEND = g_iNpcUpMaxHP_CGATE[(iNXT_UPLVL - 1)][1];
					iNEED_MONEY = g_iNpcUpMaxHP_CGATE[(iNXT_UPLVL - 1)][2];
				}
			}
			break;
			default:
			{
				GCAnsNpcUpgrade(iIndex, 5, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
				return;
			}
			break;
			}
		}
		else
		{
			GCAnsNpcUpgrade(iIndex, 7, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
			return;
		}
	}
	break;
	case 283://Statue
	{
		_CS_NPC_DATA m_NpcData;

		BOOL bResult;

		bResult = g_CastleSiege.GetNpcData(lpMsg->iNpcNumber, lpMsg->iNpcIndex, m_NpcData);

		if (bResult)
		{
			switch (lpMsg->iNpcUpType)
			{
			case 1:
			{
				iCUR_UPLVL = m_NpcData.m_iNPC_DF_LEVEL;
				iNXT_UPLVL = lpMsg->iNpcUpValue;

				if ((iNXT_UPLVL >= 1) && (iNXT_UPLVL <= 3) && (iNXT_UPLVL == (iCUR_UPLVL + 1))) //Fixed
				{
					bENABLE_UPGRADE = TRUE;
					iNXT_UPVALUE = iNXT_UPLVL;
					iNEED_GEMOFDEFEND = g_iNpcUpDfLevel_CSTATUE[(iNXT_UPLVL - 1)][1];
					iNEED_MONEY = g_iNpcUpDfLevel_CSTATUE[(iNXT_UPLVL - 1)][2];
				}
			}
			break;
			case 2:
			{
				iCUR_UPLVL = m_NpcData.m_iNPC_RG_LEVEL;
				iNXT_UPLVL = lpMsg->iNpcUpValue;

				if ((iNXT_UPLVL >= 1) && (iNXT_UPLVL <= 3) && (iNXT_UPLVL == (iCUR_UPLVL + 1)))
				{
					bENABLE_UPGRADE = TRUE;
					iNXT_UPVALUE = iNXT_UPLVL;
					iNEED_GEMOFDEFEND = g_iNpcUpRgLevel_CSTATUE[(iNXT_UPLVL - 1)][1];
					iNEED_MONEY = g_iNpcUpRgLevel_CSTATUE[(iNXT_UPLVL - 1)][2];
				}
			}
			break;
			case 3:
			{
				iCUR_UPLVL = 0;

				for (int i = 0; i < 3; i++)
				{
					if (m_NpcData.m_iNPC_MAXHP >= *g_iNpcUpMaxHP_CSTATUE[i])
					{
						iCUR_UPLVL = (i + 1);
					}
				}

				iNXT_UPLVL = 0;

				for (int i = 0; i < 3; i++)
				{
					if (lpMsg->iNpcUpValue >= *g_iNpcUpMaxHP_CSTATUE[i])
					{
						iNXT_UPLVL = (i + 1);
					}
				}

				if ((iNXT_UPLVL >= 1) && (iNXT_UPLVL <= 3) && (iNXT_UPLVL == (iCUR_UPLVL + 1)))
				{
					bENABLE_UPGRADE = TRUE;
					iNXT_UPVALUE = g_iNpcUpMaxHP_CSTATUE[(iNXT_UPLVL - 1)][0];
					iNEED_GEMOFDEFEND = g_iNpcUpMaxHP_CSTATUE[(iNXT_UPLVL - 1)][1];
					iNEED_MONEY = g_iNpcUpMaxHP_CSTATUE[(iNXT_UPLVL - 1)][2];
				}
			}
			break;
			default:
			{
				GCAnsNpcUpgrade(iIndex, 5, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
				sLog->outBasic("[CastleSiege] CGReqNpcUpgrade() ERROR - UpType doesn't Exist [%s][%s], Guild:(%s)(%d), NPC CLS:%d, IDX:%d, UPTYPE:%d, UPVAL:%d", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildNumber, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
				return;
			}
			break;
			}
		}
		else
		{
			GCAnsNpcUpgrade(iIndex, 7, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
			return;
		}
	}
	break;
	}

	int iGEMOFDEFEND_COUNT;
	int x;
	int bRETVAL;

	if (bENABLE_UPGRADE == FALSE)
	{
		GCAnsNpcUpgrade(iIndex, 6, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
	}
	else
	{
		if (Obj.m_PlayerData->Money < iNEED_MONEY)
		{
			GCAnsNpcUpgrade(iIndex, 3, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
			sLog->outBasic("[CastleSiege] CGReqNpcUpgrade() ERROR - Money is Low [%s][%s], Guild:(%s)(%d), NPC CLS:%d, IDX:%d, UPTYPE:%d, UPVAL:%d, NXTLV:%d, Money:%d, Need Money:%d", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildNumber, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue, iNXT_UPLVL, Obj.m_PlayerData->Money, iNEED_MONEY);
		}
		else
		{
			for (iGEMOFDEFEND_COUNT = 0, x = 0; x < INVENTORY_SIZE; x++)
			{
				if (Obj.pntInventory[x]->IsItem() == TRUE)
				{
					if (Obj.pntInventory[x]->m_Type == ITEMGET(14, 31))
					{
						iGEMOFDEFEND_COUNT += 1;
					}
				}
			}

			if (iGEMOFDEFEND_COUNT < iNEED_GEMOFDEFEND)
			{
				GCAnsNpcUpgrade(iIndex, 4, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
				sLog->outBasic("[CastleSiege] CGReqNpcUpgrade() ERROR - Gem is Low [%s][%s], Guild:(%s)(%d), NPC CLS:%d, IDX:%d, UPTYPE:%d, UPVAL:%d, NXTLV:%d, Gem:%d, Need Gem:%d", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildNumber, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue, iNXT_UPLVL, iGEMOFDEFEND_COUNT, iNEED_GEMOFDEFEND);
			}
			else
			{
				bRETVAL = g_CastleSiege.PayForUpgradeDbNpc(iIndex, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue, (iNXT_UPLVL - 1));

				if (bRETVAL == FALSE)
				{
				}
				else
				{
					Obj.m_bIsCastleNPCUpgradeCompleted = TRUE;
					sLog->outBasic("[CastleSiege] [0xB2][0x07] CGReqNpcUpgrade() - Pay For Npc Upgrade (CLS:%d, IDX:%d, UPTYPE:%d, UPVAL:%d)", lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, iNXT_UPVALUE);
					GS_GDReqCastleNpcUpgrade(g_MapServerManager.GetMapSvrGroup(), iIndex, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, iNXT_UPVALUE, (iNXT_UPLVL - 1));
					sLog->outBasic("[CastleSiege] CGReqNpcUpgrade() REQ OK - [%s][%s], Guild:(%s)(%d), NPC CLS:%d, IDX:%d, UPTYPE:%d, UPVAL:%d, NXTLV:%d", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildNumber, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue, iNXT_UPLVL);
				}
			}
		}

	}
}

void GameProtocol::GCAnsNpcUpgrade(CGameObject &Obj, int iResult, int iNpcNumber, int iNpcIndex, int iNpcUpType, int iNpcUpValue)
{
	PMSG_ANS_NPCUPGRADE pMsgResult;

	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x07, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.iNpcNumber = iNpcNumber;
	pMsgResult.iNpcIndex = iNpcIndex;
	pMsgResult.iNpcUpType = iNpcUpType;
	pMsgResult.iNpcUpValue = iNpcUpValue;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::CGReqTaxMoneyInfo(PMSG_REQ_TAXMONEYINFO * lpMsg, CGameObject &Obj)
{
	if (lpMsg == NULL)
	{
		return;
	}

	if (strcmp(Obj.m_PlayerData->GuildName, "") == 0)
	{
		return;
	}

	if ((g_CastleSiege.CheckGuildOwnCastle(Obj.m_PlayerData->GuildName) == FALSE) || (Obj.m_PlayerData->GuildStatus != G_MASTER))
	{
		sLog->outError( "[CastleSiege] CGReqTaxMoneyInfo() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus);
	}
	else
	{
		GS_GDReqTaxInfo(g_MapServerManager.GetMapSvrGroup(), iIndex);
		sLog->outBasic("[CastleSiege] CGReqTaxMoneyInfo() REQ OK - [%s][%s], Guild:(%s)(%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus);
	}
}

void GameProtocol::GCAnsTaxMoneyInfo(CGameObject &Obj, int iResult, BYTE btTaxRateChaos, BYTE btTaxRateStore, __int64 i64Money)
{
	PMSG_ANS_TAXMONEYINFO pMsgResult;

	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x08, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.btTaxRateChaos = btTaxRateChaos;
	pMsgResult.btTaxRateStore = btTaxRateStore;
	pMsgResult.btMoney1 = SET_NUMBERH(SET_NUMBERHW(HIDWORD(i64Money)));
	pMsgResult.btMoney2 = SET_NUMBERL(SET_NUMBERHW(HIDWORD(i64Money)));
	pMsgResult.btMoney3 = SET_NUMBERH(SET_NUMBERLW(HIDWORD(i64Money)));
	pMsgResult.btMoney4 = SET_NUMBERL(SET_NUMBERLW(HIDWORD(i64Money)));
	pMsgResult.btMoney5 = SET_NUMBERH(SET_NUMBERHW(LODWORD(i64Money)));
	pMsgResult.btMoney6 = SET_NUMBERL(SET_NUMBERHW(LODWORD(i64Money)));
	pMsgResult.btMoney7 = SET_NUMBERH(SET_NUMBERLW(LODWORD(i64Money)));
	pMsgResult.btMoney8 = SET_NUMBERL(SET_NUMBERLW(LODWORD(i64Money)));

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::CGReqTaxRateChange(PMSG_REQ_TAXRATECHANGE * lpMsg, CGameObject &Obj)
{
	int iMaxTaxRate;
	int iTaxRate;

	if (lpMsg == NULL)
	{
		return;
	}

	if (strcmp(Obj.m_PlayerData->GuildName, "") == 0)
	{
		return;
	}

	if ((g_CastleSiege.CheckGuildOwnCastle(Obj.m_PlayerData->GuildName) == FALSE) || (Obj.m_PlayerData->GuildStatus != G_MASTER))
	{
		sLog->outError( "[CastleSiege] CGReqTaxRateChange() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus);
	}
	else
	{
		iMaxTaxRate = 0;

		switch (lpMsg->btTaxType)
		{
		case 1:
			iMaxTaxRate = 3;
			break;
		case 2:
			iMaxTaxRate = 3;
			break;
		case 3:
			iMaxTaxRate = 300000;
			break;
		}

		iTaxRate = MAKE_NUMBERDW(MAKE_NUMBERW(lpMsg->btTaxRate1, lpMsg->btTaxRate2), MAKE_NUMBERW(lpMsg->btTaxRate3, lpMsg->btTaxRate4));

		if ((iTaxRate < 0) || (iTaxRate > iMaxTaxRate))
		{
			GCAnsTaxRateChange(iIndex, 0, 0, 0);
			sLog->outBasic("[CastleSiege] CGReqTaxRateChange() ERROR - Tax Rate Out of Range [%s][%s], Guild:(%s)(%d), TaxType:%d, TaxRate:%d", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus, lpMsg->btTaxType, iTaxRate);
		}
		else
		{
			GS_GDReqTaxRateChange(g_MapServerManager.GetMapSvrGroup(), iIndex, lpMsg->btTaxType, iTaxRate);
			sLog->outBasic("[CastleSiege] CGReqTaxRateChange() REQ OK - [%s][%s], Guild:(%s)(%d), TaxType:%d, TaxRate:%d", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus, lpMsg->btTaxType, iTaxRate);
		}
	}
}

void GameProtocol::GCAnsTaxRateChange(CGameObject &Obj, int iResult, BYTE btTaxType, int iTaxRate)
{
	PMSG_ANS_TAXRATECHANGE pMsgResult;
	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x09, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.btTaxType = btTaxType;
	pMsgResult.btTaxRate1 = SET_NUMBERH(SET_NUMBERHW(iTaxRate));
	pMsgResult.btTaxRate2 = SET_NUMBERL(SET_NUMBERHW(iTaxRate));
	pMsgResult.btTaxRate3 = SET_NUMBERH(SET_NUMBERLW(iTaxRate));
	pMsgResult.btTaxRate4 = SET_NUMBERL(SET_NUMBERLW(iTaxRate));
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}



void GameProtocol::CGReqMoneyDrawOut(PMSG_REQ_MONEYDRAWOUT * lpMsg, CGameObject &Obj)
{
	int iMoneyChange;
	BOOL bRETVAL;

	if (lpMsg == NULL)
	{
		return;
	}

	if (strcmp(Obj.m_PlayerData->GuildName, "") == 0)
	{
		return;
	}

	if ((g_CastleSiege.CheckGuildOwnCastle(Obj.m_PlayerData->GuildName) == FALSE) || (Obj.m_PlayerData->GuildStatus != G_MASTER))
	{
		sLog->outError( "[CastleSiege] CGReqMoneyDrawOut() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus);
	}
	else
	{
		iMoneyChange = MAKE_NUMBERDW(MAKE_NUMBERW(lpMsg->btMoney4, lpMsg->btMoney3), MAKE_NUMBERW(lpMsg->btMoney2, lpMsg->btMoney1));

		if (iMoneyChange <= 0)
		{
			GCAnsMoneyDrawOut(iIndex, 0, 0);
			sLog->outBasic("[CastleSiege] CGReqMoneyDrawOut() ERROR - Req Money < 0 [%s][%s], Guild:(%s)(%d), Money:%d, TotMoney:%d", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus, iMoneyChange, Obj.m_PlayerData->Money);
		}
		else
		{
			bRETVAL = g_CastleSiege.CheckCastleHasMoney(iMoneyChange);

			if (bRETVAL == FALSE)
			{
				GCAnsMoneyDrawOut(iIndex, 0, 0);
				sLog->outBasic("[CastleSiege] CGReqMoneyDrawOut() ERROR - Castle Money is Low [%s][%s], Guild:(%s)(%d), Money:%d, TotMoney:%d", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus, iMoneyChange, Obj.m_PlayerData->Money);
			}
			else
			{
				if ((Obj.m_PlayerData->Money + iMoneyChange) > MAX_ZEN)
				{
					GCAnsMoneyDrawOut(iIndex, 0, 0);
				}
				else
				{
					GS_GDReqCastleMoneyChange(g_MapServerManager.GetMapSvrGroup(), iIndex, -iMoneyChange);
					sLog->outBasic("[CastleSiege] CGReqMoneyDrawOut() REQ OK - [%s][%s], Guild:(%s)(%d), Money:%d, TotMoney:%d", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus, iMoneyChange, Obj.m_PlayerData->Money);
				}
			}
		}
	}
}

void GameProtocol::GCAnsMoneyDrawOut(CGameObject &Obj, int iResult, __int64 i64Money)
{
	PMSG_ANS_MONEYDRAWOUT pMsgResult;
	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x10, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.btMoney1 = SET_NUMBERH(SET_NUMBERHW(HIDWORD(i64Money)));
	pMsgResult.btMoney2 = SET_NUMBERL(SET_NUMBERHW(HIDWORD(i64Money)));
	pMsgResult.btMoney3 = SET_NUMBERH(SET_NUMBERLW(HIDWORD(i64Money)));
	pMsgResult.btMoney4 = SET_NUMBERL(SET_NUMBERLW(HIDWORD(i64Money)));
	pMsgResult.btMoney5 = SET_NUMBERH(SET_NUMBERHW(LODWORD(i64Money)));
	pMsgResult.btMoney6 = SET_NUMBERL(SET_NUMBERHW(LODWORD(i64Money)));
	pMsgResult.btMoney7 = SET_NUMBERH(SET_NUMBERLW(LODWORD(i64Money)));
	pMsgResult.btMoney8 = SET_NUMBERL(SET_NUMBERLW(LODWORD(i64Money)));

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::GCAnsCsGateState(CGameObject &Obj, int iResult, int iGateIndex)
{
	PMSG_ANS_CSGATESTATE pMsgResult;
	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x11, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.btIndex1 = SET_NUMBERH((iGateIndex & 0xffff));
	pMsgResult.btIndex2 = SET_NUMBERL((iGateIndex & 0xffff));
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::CGReqCsGateOperate(PMSG_REQ_CSGATEOPERATE * lpMsg, CGameObject &Obj)
{
	int iGateIndex;

	if (strcmp(Obj.m_PlayerData->GuildName, "") == 0)
	{
		return;
	}

	if (g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
	{
		if (Obj.m_btCsJoinSide != 1)
		{
			sLog->outBasic("[CastleSiege] CGReqCsGateOperate() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus);
			return;
		}
	}
	else
	{
		if ((g_CastleSiege.CheckCastleOwnerMember(Obj) == FALSE) && (g_CastleSiege.CheckCastleOwnerUnionMember(Obj) == FALSE))
		{
			sLog->outBasic("[CastleSiege] CGReqCsGateOperate() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus);
			return;
		}
	}

	iGateIndex = MAKE_NUMBERW(lpMsg->btIndex1, lpMsg->btIndex2);

	if (g_CastleSiege.CheckCsGateAlive(iGateIndex) == TRUE)
	{
		int iOperateGate = g_CastleSiege.OperateGate(iGateIndex, getGameObject(iGateIndex)->m_iCsNpcExistVal, lpMsg->btOperate);

		if (iOperateGate == 0)
		{
			GCAnsCsGateOperate(iIndex, 0, -1, 0);
			sLog->outBasic("[CastleSiege] CGReqCsGateOperate() ERROR - Operate Gate Failed [%s][%s], Guild:(%s)(%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus);
		}
		else
		{
			GCAnsCsGateOperate(iIndex, 1, iGateIndex, lpMsg->btOperate);
			g_CastleSiege.SendCsGateStateViewPort(iGateIndex, lpMsg->btOperate);
			sLog->outBasic("[CastleSiege] CGReqCsGateOperate() OK - [%s][%s], Guild:(%s)(%d), DOOR:(%d)(X:%d,Y:%d)(STATUE:%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildNumber, iGateIndex, getGameObject(iGateIndex)->X, getGameObject(iGateIndex)->Y, getGameObject(iGateIndex)->m_btCsGateOpen);
		}
	}
	else
	{
		GCAnsCsGateOperate(iIndex, 2, -1, 0);
		sLog->outBasic("[CastleSiege] CGReqCsGateOperate() ERROR - Gate Doesn't Exist [%s][%s], Guild:(%s)(%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus);
	}
}

void GameProtocol::GCAnsCsGateOperate(CGameObject &Obj, int iResult, int iGateIndex, int iGateOperate)
{
	PMSG_ANS_CSGATEOPERATE pMsgResult;

	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x12, sizeof(pMsgResult));
	pMsgResult.btResult = iResult;
	pMsgResult.btOperate = iGateOperate;
	pMsgResult.btIndex1 = SET_NUMBERH(iGateIndex & 0xffff); //??
	pMsgResult.btIndex2 = SET_NUMBERL(iGateIndex & 0xffff); //??
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::GCAnsCsGateCurState(CGameObject &Obj, int iGateIndex, int iGateOperate)
{
	PMSG_ANS_CSGATECURSTATE pMsgResult;
	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x13, sizeof(pMsgResult));
	pMsgResult.btOperate = iGateOperate;
	pMsgResult.btIndex1 = SET_NUMBERH(iGateIndex & 0xffff); //??
	pMsgResult.btIndex2 = SET_NUMBERL(iGateIndex & 0xffff); //??
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::GCAnsCsAccessSwitchState(CGameObject &Obj, int iSwitchIndex, int iSwitchUserIndex, BYTE btSwitchState)
{
	PMSG_ANS_NOTIFYSWITCHPROC pMsgResult;
	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x14, sizeof(pMsgResult));
	pMsgResult.btIndex1 = SET_NUMBERH(iSwitchIndex & 0xffff); //??
	pMsgResult.btIndex2 = SET_NUMBERL(iSwitchIndex & 0xffff); //??
	pMsgResult.btUserIndex1 = SET_NUMBERH(iSwitchUserIndex & 0xffff); //??
	pMsgResult.btUserIndex2 = SET_NUMBERL(iSwitchUserIndex & 0xffff); //??
	pMsgResult.btSwitchState = btSwitchState;
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::GCAnsCsAccessCrownState(CGameObject &Obj, BYTE btCrownState)
{
	PMSG_ANS_NOTIFYCROWNPROC pMsgResult;
	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x15, sizeof(pMsgResult));
	pMsgResult.btCrownState = btCrownState;

	if (btCrownState == 0)
	{
		if (Obj.m_iAccumulatedCrownAccessTime < 0 || Obj.m_iAccumulatedCrownAccessTime > 61000) //season4.0 changed
		{
			Obj.m_iAccumulatedCrownAccessTime = 0;
		}

		sLog->outBasic("[CastleSiege] [Reg. Attempt] Accumulated Crown AccessTime : %d [%s](%s)(%s)", Obj.m_iAccumulatedCrownAccessTime, Obj.m_PlayerData->GuildName, Obj.AccountID, Obj.Name);
	}

	if (btCrownState == 1)
	{
		sLog->outBasic("[CastleSiege] [Reg. Success] Accumulated Crown AccessTime : %d [%s](%s)(%s)", Obj.m_iAccumulatedCrownAccessTime, Obj.m_PlayerData->GuildName, Obj.AccountID, Obj.Name);
		Obj.m_iAccumulatedCrownAccessTime = 0;
	}

	if (btCrownState == 2)
	{
		Obj.m_iAccumulatedCrownAccessTime += GetTickCount() - g_CastleSiege.GetCrownAccessTickCount();

		if (Obj.m_iAccumulatedCrownAccessTime < 0 || Obj.m_iAccumulatedCrownAccessTime > 61000) //season4.0 changed
		{
			Obj.m_iAccumulatedCrownAccessTime = 0;
		}

		if (Obj.Live == FALSE)
		{
			if (g_CastleSiege.CastleSiegeResetAccumulationAfterKill == TRUE)
			{
				Obj.m_iAccumulatedCrownAccessTime = 0;
			}
		}

		sLog->outBasic("[CastleSiege] [Reg. Fail] Accumulated Crown AccessTime : %d [%s](%s)(%s)", Obj.m_iAccumulatedCrownAccessTime, Obj.m_PlayerData->GuildName, Obj.AccountID, Obj.Name);
	}

	pMsgResult.dwAccumulatedCrownAccessTime = Obj.m_iAccumulatedCrownAccessTime; //??

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}
void GameProtocol::GCAnsCsNotifyStart(CGameObject &Obj, BYTE btStartState)
{
	PMSG_ANS_NOTIFYCSSTART pMsgResult;
	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x17, sizeof(pMsgResult));
	pMsgResult.btStartState = btStartState;
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::GCAnsCsNotifyProgress(CGameObject &Obj, BYTE btCastleSiegeState, LPCSTR lpszGuildName)
{
	if (lpszGuildName == NULL)
	{
		return;
	}

	PMSG_ANS_NOTIFYCSPROGRESS pMsgResult;

	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x18, sizeof(pMsgResult));
	pMsgResult.btCastleSiegeState = btCastleSiegeState;
	std::memset(pMsgResult.szGuildName, 0, sizeof(pMsgResult.szGuildName));
	std::memcpy(pMsgResult.szGuildName, lpszGuildName, sizeof(pMsgResult.szGuildName));

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::GCAnsCsMapSvrTaxInfo(CGameObject &Obj, BYTE btTaxType, BYTE btTaxRate)
{
	PMSG_ANS_MAPSVRTAXINFO pMsgResult;

	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x1A, sizeof(pMsgResult));
	pMsgResult.btTaxType = btTaxType;
	pMsgResult.btTaxRate = btTaxRate;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsgResult, pMsgResult.h.size);

}

void GameProtocol::CGReqCsMiniMapData(PMSG_REQ_MINIMAPDATA * lpMsg, CGameObject &Obj)
{
	if (gObjIsConnected(Obj) == FALSE)
	{
		return;
	}

	if ((Obj.m_btCsJoinSide <= 0) || (g_CastleSiege.CheckUnionGuildMaster(Obj) == FALSE))
	{
		GCAnsCsMiniMapData(iIndex, 3);
	}
	else if (g_CastleSiege.GetCastleState() != CASTLESIEGE_STATE_STARTSIEGE)
	{
		GCAnsCsMiniMapData(iIndex, 2);
	}
	else
	{
		g_CastleSiege.AddMiniMapDataReqUser(Obj);
		GCAnsCsMiniMapData(iIndex, 1);
	}
}

void GameProtocol::GCAnsCsMiniMapData(CGameObject &Obj, BYTE btResult)
{
	PMSG_ANS_MINIMAPDATA pMsgResult;

	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x1B, sizeof(pMsgResult));
	pMsgResult.btResult = btResult;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}


void GameProtocol::CGReqStopCsMiniMapData(PMSG_REQ_STOPMINIMAPDATA * lpMsg, CGameObject &Obj)
{
	if (gObjIsConnected(Obj) == FALSE)
	{
		return;
	}

}


void GameProtocol::CGReqCsSendCommand(PMSG_REQ_CSCOMMAND * lpMsg, CGameObject &Obj)
{
	if (gObjIsConnected(Obj) == FALSE)
	{
		return;
	}

	if (Obj.m_btCsJoinSide <= 0 || g_CastleSiege.CheckUnionGuildMaster(Obj) == FALSE)
	{
		return;
	}

	if (g_CastleSiege.GetCastleState() != CASTLESIEGE_STATE_STARTSIEGE)
	{
		return;
	}

	GCAnsCsSendCommand(Obj.m_btCsJoinSide, lpMsg->btTeam, lpMsg->btX, lpMsg->btY, lpMsg->btCommand);
}

void GameProtocol::GCAnsCsSendCommand(int iCsJoinSize, BYTE btTeam, BYTE btX, BYTE btY, BYTE btCommand)
{
	PMSG_ANS_CSCOMMAND pMsgResult;

	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x1D, sizeof(pMsgResult));

	pMsgResult.btTeam = btTeam;
	pMsgResult.btX = btX;
	pMsgResult.btY = btY;
	pMsgResult.btCommand = btCommand;

	for (int iIndex = g_ConfigRead.server.GetObjectStartUserIndex(); iIndex < g_ConfigRead.server.GetObjectMax(); iIndex++)
	{
		if (gObjIsConnected(Obj) == FALSE) continue;

		if ((Obj.m_btCsJoinSide == iCsJoinSize) && (Obj.MapNumber == MAP_INDEX_CASTLESIEGE))
		{
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
		}
	}
}

void GameProtocol::GCAnsCsLeftTimeAlarm(BYTE btHour, BYTE btMinute)
{
	PMSG_ANS_CSLEFTTIMEALARM pMsgResult;

	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x1E, sizeof(pMsgResult));
	pMsgResult.btHour = btHour;
	pMsgResult.btMinute = btMinute;

	for (int iIndex = g_ConfigRead.server.GetObjectStartUserIndex(); iIndex < g_ConfigRead.server.GetObjectMax(); iIndex++)
	{
		if (!gObjIsConnected(Obj))
		{
			continue;
		}

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
	}

}

void GameProtocol::GCAnsSelfCsLeftTimeAlarm(CGameObject &Obj, BYTE btHour, BYTE btMinute)
{
	PMSG_ANS_CSLEFTTIMEALARM pMsgResult;

	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x1E, sizeof(pMsgResult));
	pMsgResult.btHour = btHour;
	pMsgResult.btMinute = btMinute;

	if (!gObjIsConnected(Obj))
	{
		return;
	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::CGReqCsSetEnterHuntZone(PMSG_REQ_CSHUNTZONEENTER * lpMsg, CGameObject &Obj)
{
	if (gObjIsConnected(Obj) == FALSE)
	{
		return;
	}

	if (g_CastleSiege.CheckCastleOwnerMember(Obj) == FALSE)
	{
		GCAnsCsSetEnterHuntZone(iIndex, 2, lpMsg->btHuntZoneEnter);
	}
	else if (g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
	{
		GCAnsCsSetEnterHuntZone(iIndex, 0, lpMsg->btHuntZoneEnter);
	}
	else
	{
		g_CastleSiege.SetHuntZoneEnter(lpMsg->btHuntZoneEnter);
		GCAnsCsSetEnterHuntZone(iIndex, 1, lpMsg->btHuntZoneEnter);
	}
}

void GameProtocol::GCAnsCsSetEnterHuntZone(CGameObject &Obj, BYTE btResult, BYTE btEnterHuntZone)
{
	PMSG_ANS_CSHUNTZONEENTER pMsgResult;

	pMsgResult.h.set((BYTE*)&pMsgResult, 0xB2, 0x1F, sizeof(pMsgResult));
	pMsgResult.btResult = btResult;
	pMsgResult.btHuntZoneEnter = btEnterHuntZone;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsgResult, pMsgResult.h.size);
}

void GameProtocol::CGReqNpcDbList(PMSG_REQ_NPCDBLIST * lpMsg, CGameObject &Obj)
{
	PMSG_ANS_NPCDBLIST pResult;

	if (lpMsg == NULL)
	{
		return;
	}

	if (strcmp(Obj.m_PlayerData->GuildName, "") == 0)
	{
		return;
	}

	if (g_CastleSiege.CheckGuildOwnCastle(Obj.m_PlayerData->GuildName) == FALSE || ((Obj.m_PlayerData->GuildStatus != 128) && (Obj.m_PlayerData->GuildStatus != 64)))
	{
		sLog->outError( "[CastleSiege] CGReqNpcDbList() ERROR - Authority Fail [%s][%s], Guild:(%s)(%d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildName, Obj.m_PlayerData->GuildStatus);
		pResult.h.set((BYTE*)&pResult, 0xB3, sizeof(pResult));
		pResult.iCount = 0;
		pResult.btResult = 2;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, sizeof(pResult));
	}
	else
	{
		switch (lpMsg->btMonsterCode)
		{
		case 1:
			g_CastleSiege.SendNpcStateList(iIndex, 277);
			break;
		case 2:
			g_CastleSiege.SendNpcStateList(iIndex, 283);
			break;
		}
	}
}

void GameProtocol::CGReqCsRegGuildList(PMSG_REQ_CSREGGUILDLIST * lpMsg, CGameObject &Obj)
{
	if (lpMsg == NULL) return;

	GS_GDReqAllGuildMarkRegInfo(g_MapServerManager.GetMapSvrGroup(), iIndex);
}

void GameProtocol::CGReqCsAttkGuildList(PMSG_REQ_CSATTKGUILDLIST * lpMsg, CGameObject &Obj)
{
	char cBUFFER[1625]; //season 2.5 changed old -> 1022
	PMSG_ANS_CSATTKGUILDLIST* lpMsgSend;
	PMSG_CSATTKGUILDLIST* lpMsgSendBody;
	int iCount;

	if (lpMsg == NULL)	return;

	lpMsgSend = (PMSG_ANS_CSATTKGUILDLIST*)(cBUFFER);
	lpMsgSendBody = (PMSG_CSATTKGUILDLIST*)(cBUFFER + sizeof(PMSG_ANS_CSATTKGUILDLIST));

	iCount = 0;

	lpMsgSend->btResult = g_CastleSiege.GetCsAttkGuildList(lpMsgSendBody, iCount);
	lpMsgSend->iCount = iCount;

	if (lpMsgSend->iCount < 0)	 lpMsgSend->iCount = 0;
	if (lpMsgSend->iCount > 100)lpMsgSend->iCount = 100;

	lpMsgSend->h.set((BYTE*)lpMsgSend, 0xB5, ((lpMsgSend->iCount * sizeof(PMSG_CSATTKGUILDLIST)) + sizeof(PMSG_ANS_CSATTKGUILDLIST)));
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)cBUFFER, ((lpMsgSend->iCount * sizeof(PMSG_CSATTKGUILDLIST)) + sizeof(PMSG_ANS_CSATTKGUILDLIST)));
}

void GameProtocol::CGReqWeaponUse(PMSG_REQ_USEWEAPON * aRecv, CGameObject &Obj)
{
	WORD wObjIndex = MAKE_NUMBERW(aRecv->btObjIndexH, aRecv->btObjIndexL);

	if (!ObjectMaxRange(wObjIndex))
	{
		return;
	}

	CGameObject lpOwnerObj = Obj;
	CGameObject lpWeaponObj = *getGameObject(wObjIndex);

	if (lpOwnerObj.Type != OBJ_USER)
	{
		return;
	}

	if (lpWeaponObj.Type != OBJ_NPC)
	{
		return;
	}

	PMSG_ANS_USEWEAPON pMsg = { 0 };

	PHeadSubSetB((BYTE*)&pMsg, 0xB7, 0x01, sizeof(pMsg));
	pMsg.btResult = 1;

	if (lpWeaponObj.Class == 221)	// Slingshot attack
	{
		pMsg.btWeaponType = 1;
		pMsg.btObjIndexH = SET_NUMBERH(wObjIndex);
		pMsg.btObjIndexL = SET_NUMBERL(wObjIndex);
		g_CsNPC_Weapon.GetTargetPointXY(lpWeaponObj.Class, aRecv->btTargetIndex - 1, pMsg.btPointX, pMsg.btPointY, TRUE);
	}
	else if (lpWeaponObj.Class == 222)	// Slingshot defense
	{
		pMsg.btWeaponType = 2;
		pMsg.btObjIndexH = SET_NUMBERH(wObjIndex);
		pMsg.btObjIndexL = SET_NUMBERL(wObjIndex);
		g_CsNPC_Weapon.GetTargetPointXY(lpWeaponObj.Class, aRecv->btTargetIndex - 1, pMsg.btPointX, pMsg.btPointY, TRUE);
	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));

	pMsg.btResult = 2;

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpWeaponObj.VpPlayer2[n].type == OBJ_USER)
		{
			if (lpWeaponObj.VpPlayer2[n].state)
			{
				if (iIndex != lpWeaponObj.VpPlayer2[n].number)
				{
					IOCP.DataSend(lpWeaponObj.VpPlayer2[n].number, (BYTE*)&pMsg, sizeof(pMsg));
				}
			}
		}
	}

	gObjNotifyUseWeaponV1(lpOwnerObj, lpWeaponObj, pMsg.btPointX, pMsg.btPointY);
	g_CsNPC_Weapon.SetWeaponCalDamageInfo(wObjIndex, pMsg.btPointX, pMsg.btPointY, 10000);
}

void GameProtocol::CGReqWeaponDamageValue(PMSG_REQ_WEAPON_DAMAGE_VALUE * aRecv, CGameObject &Obj)
{
	BYTE btTargetX = 0;
	BYTE btTargetY = 0;
	WORD wObjIndex = MAKE_NUMBERW(aRecv->btObjIndexH, aRecv->btObjIndexL);

	if (g_CsNPC_Weapon.GetWeaponCalDamageInfo(wObjIndex, btTargetX, btTargetY) == TRUE)
	{
		gObjNotifyUseWeaponDamage(*getGameObject(wObjIndex), btTargetX, btTargetY);
	}
	else
	{
		MsgOutput(Obj, Lang.GetText(0, 181));
	}
}

void GameProtocol::GCSendObjectCreationState(int iObjectIndex)
{
	if (!ObjectMaxRange(iObjectIndex))
	{
		return;
	}

	CGameObject* lpObj = getGameObject(iObjectIndex);
	PMSG_NOTIFY_OBJECT_CREATION_STATE	pMsg = { 0 };

	if (Obj.Class == 278)
	{
		PHeadSubSetB((BYTE*)&pMsg, 0xB9, 0x01, sizeof(pMsg));
		pMsg.btObjIndexH = SET_NUMBERH(iObjectIndex);
		pMsg.btObjIndexL = SET_NUMBERL(iObjectIndex);
		pMsg.btCreationState = Obj.m_btCreationState;
		MsgSendV2(*lpObj, (BYTE*)&pMsg, sizeof(pMsg));
	}
}

void GameProtocol::CGReqGuildMarkOfCastleOwner(PMSG_REQ_GUILDMARK_OF_CASTLEOWNER * aRecv, CGameObject &Obj)
{	
	char szGuildName[MAX_GUILD_LEN + 1] = { 0 };

	std::memcpy(szGuildName, g_CastleSiegeSync.GetCastleOwnerGuild(), MAX_GUILD_LEN);

	if (szGuildName)
	{
		GUILD_INFO_STRUCT * lpGuild = Guild.SearchGuild(szGuildName);

		if (lpGuild != NULL)
		{
			PMSG_ANS_GUILDMARK_OF_CASTLEOWNER pMsg = { 0 };

			PHeadSubSetB((BYTE*)&pMsg, 0xB9, 0x02, sizeof(pMsg));
			std::memcpy(pMsg.GuildMarkOfCastleOwner, lpGuild->Mark, sizeof(pMsg.GuildMarkOfCastleOwner));

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
		}
	}
}

void GameProtocol::CGReqCastleHuntZoneEntrance(PMSG_REQ_MOVE_TO_CASTLE_HUNTZONE * aRecv, CGameObject &Obj)
{
	PMSG_ANS_MOVE_TO_CASTLE_HUNTZONE pMsg = { 0 };

	PHeadSubSetB((BYTE*)&pMsg, 0xB9, 0x05, sizeof(pMsg));

	pMsg.btResult = 0;

	int iEnterTaxMoney = 0;

	iEnterTaxMoney = g_CastleSiegeSync.GetTaxHuntZone(Obj, TRUE);

	if (iEnterTaxMoney < 0)
	{
		iEnterTaxMoney = 0;
	}

	BOOL bPermission = FALSE;

	if (g_CastleSiege.GetHuntZoneEnter())
	{
		bPermission = TRUE;
	}

	if (Obj.m_PlayerData->lpGuild)
	{
		if (g_CastleSiege.CheckCastleOwnerMember(Obj))
		{
			bPermission = TRUE;
		}

		if (g_CastleSiege.CheckCastleOwnerUnionMember(Obj))
		{
			bPermission = TRUE;
		}
	}
	else
	{
		if (g_CastleSiege.GetHuntZoneEnter())
		{
			bPermission = TRUE;
		}
	}

	if (bPermission == TRUE)
	{
		if (Obj.m_PlayerData->Money >= iEnterTaxMoney)
		{
			if (Obj.MapNumber == MAP_INDEX_CASTLESIEGE)
			{
				if (gObjMoveGate(Obj, 95) == TRUE)
				{
					pMsg.btResult = 1;
					int iOldMoney = Obj.m_PlayerData->Money;

					Obj.m_PlayerData->Money -= iEnterTaxMoney;

					g_CastleSiegeSync.AddTributeMoney(iEnterTaxMoney);

					if (Obj.m_PlayerData->Money < 0)
					{
						Obj.m_PlayerData->Money = 0;
					}

					GCMoneySend(Obj, Obj.m_PlayerData->Money);

					sLog->outBasic("[Castle HuntZone] [%s][%s] - Entrance TAX : %d - %d = %d", Obj.AccountID, Obj.Name, iOldMoney, iEnterTaxMoney, Obj.m_PlayerData->Money);
				}
			}
		}
	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
}

void GameProtocol::CGReqJewelMix(PMSG_REQ_JEWEL_MIX * lpMsg, CGameObject &Obj)
{
	if (lpMsg == NULL)
		return;

	bool bCheck = false;
	Check_SameSerialItem(iIndex, 3, bCheck);

	if (bCheck == true)
		return;
	CJewelMixSystem::MixJewel(iIndex, lpMsg->btJewelType, lpMsg->btJewelMix);
}

void GameProtocol::GCAnsJewelMix(CGameObject &Obj, int iResult)
{
	PMSG_ANS_JEWEL_UNMIX pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xBC, 0x00, sizeof(pMsg));
	pMsg.btResult = iResult;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
}


void GameProtocol::CGReqJewelUnMix(PMSG_REQ_JEWEL_UNMIX * lpMsg, CGameObject &Obj)
{
	if (lpMsg == NULL)
		return;

	bool bCheck = false;
	Check_SameSerialItem(iIndex, 3, bCheck);

	if (bCheck == true)
		return;
	CJewelMixSystem::UnMixJewel(iIndex, lpMsg->btJewelType, lpMsg->btJewelLevel, lpMsg->btJewelPos);
}

void GameProtocol::GCAnsJewelUnMix(CGameObject &Obj, int iResult)
{
	PMSG_ANS_JEWEL_UNMIX pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xBC, 0x01, sizeof(pMsg));
	pMsg.btResult = iResult;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
}

void GameProtocol::CGReqCrywolfInfo(PMSG_REQ_CRYWOLF_INFO* lpMsg, CGameObject &Obj)
{

	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		GCAnsCrywolfInfo(iIndex, g_Crywolf.GetOccupationState(), g_Crywolf.GetCrywolfState());
	}

	else
	{
		GCAnsCrywolfInfo(iIndex, g_CrywolfSync.GetOccupationState(), g_CrywolfSync.GetCrywolfState());
	}
}

void GameProtocol::GCAnsCrywolfInfo(CGameObject &Obj, BYTE btOccupationState, BYTE btCrywolfState)
{
	PMSG_ANS_CRYWOLF_INFO pMsg = { 0 };

	PHeadSubSetB((BYTE*)&pMsg, 0xBD, 0x00, sizeof(pMsg));
	pMsg.btOccupationState = btOccupationState;
	pMsg.btCrywolfState = btCrywolfState;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
}


struct PMSG_ANS_CRYWOLF_ALTAR_CONTRACT
{
	PBMSG_HEAD2 h;	// C1:BD:03
	BYTE btResult;	// 4
	BYTE btAltarState;	// 5
	BYTE btObjIndexH;	// 6
	BYTE btObjIndexL;	// 7
};

void GameProtocol::CGReqAlatrContract(PMSG_REQ_CRYWOLF_ALTAR_CONTRACT* lpMsg, CGameObject &Obj)
{
	int iAltarIndex = MAKE_NUMBERW(lpMsg->btObjIndexH, lpMsg->btObjIndexL);

	if (!ObjectMaxRange(iAltarIndex))
		return;

	CGameObject lpAltarObj = *getGameObject(iAltarIndex);

	if (!CRYWOLF_ALTAR_CLASS_RANGE(lpAltarObj.Class))
		return;

	if (!gObjIsConnected(Obj))
		return;

	PMSG_ANS_CRYWOLF_ALTAR_CONTRACT pMsg = { 0 };

	PHeadSubSetB((BYTE*)&pMsg, 0xBD, 0x03, sizeof(pMsg));
	pMsg.btResult = 0;
	pMsg.btAltarState = g_CrywolfNPC_Altar.GetAltarState(lpAltarObj.Class);
	pMsg.btObjIndexH = lpMsg->btObjIndexH;
	pMsg.btObjIndexL = lpMsg->btObjIndexL;

	if (pMsg.btAltarState == 0 &&
		Obj.Type == OBJ_USER &&
		Obj.Class == CLASS_ELF &&
		Obj.Level >= MIN_ELF_LEVEL_ALTAR)
	{
		if (g_CrywolfNPC_Altar.SetAltarUserIndex(iAltarIndex, lpAltarObj.Class, Obj) != FALSE)
		{
			pMsg.btResult = 1;
		}
	}
	else
	{
		MsgOutput(Obj, Lang.GetText(0, 256), (int)MIN_ELF_LEVEL_ALTAR);
	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
}

struct PMSG_ANS_CRYWOLF_BENEFIT_PLUS_CHAOSRATE
{
	PBMSG_HEAD2 h;	// C1:BD:09
	BYTE btPlusChaosRate;	// 4
};


void GameProtocol::CGReqPlusChaosRate(PMSG_REQ_CRYWOLF_BENEFIT_PLUS_CHAOSRATE* lpMsg, CGameObject &Obj)
{
	PMSG_ANS_CRYWOLF_BENEFIT_PLUS_CHAOSRATE pMsg = { 0 };

	PHeadSubSetB((BYTE*)&pMsg, 0xBD, 0x09, sizeof(pMsg));
	pMsg.btPlusChaosRate = g_CrywolfSync.GetPlusChaosRate();

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
}

void GameProtocol::CGReqKanturuStateInfo(PMSG_REQ_KANTURU_STATE_INFO* lpMsg, CGameObject &Obj)
{

	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		g_KanturuEntranceNPC.NotifyEntranceInfo(Obj);
	}
}


void GameProtocol::GCReqEnterKanturuBossMap(PMSG_REQ_ENTER_KANTURU_BOSS_MAP* lpMsg, CGameObject &Obj)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		g_KanturuEntranceNPC.NotifyResultEnterKanturuBossMap(Obj);
	}
}

void GameProtocol::WereWolfQuarrelCheck(CGameObject &Obj)
{

	if (Obj.Type != OBJ_USER || Obj.Connected <= PLAYER_LOGGED)
	{
		return;
	}

	g_QuestInfo.CheckQuestMapEnterOnWerwolf(Obj);
}

void GameProtocol::GateKeeperCheck(CGameObject &Obj)
{
	if (Obj.Type != OBJ_USER || Obj.Connected <= PLAYER_LOGGED)
	{
		return;
	}

	g_QuestInfo.CheckQuestMapEnterOnGateKeeper(Obj);
}

struct PMSG_ILLUSIONTEMPLE_KILLPOINT
{
	PBMSG_HEAD2 h;
	BYTE btKillPoint;
};

void GameProtocol::GCSendIllusionTempleKillPoint(CGameObject &Obj, BYTE KillPoint)
{
	PMSG_ILLUSIONTEMPLE_KILLPOINT pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xBF, 0x06, sizeof(pMsg));
	pMsg.btKillPoint = KillPoint;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::CGReqIllusionTempleUseSkill(PMSG_REQ_USEILLUSIONTEMPLESKILL * aRecv, CGameObject &Obj)
{
	g_IT_Event.ITR_USeSkill(Obj, MAKE_NUMBERW(aRecv->MagicNumberH, aRecv->MagicNumberL), MAKE_NUMBERW(aRecv->btTargetObjIndexH, aRecv->btTargetObjIndexL), aRecv->btDis);
}

void GameProtocol::GCIllusionTempleSendReward(CGameObject &Obj)
{

	g_IT_Event.ReqEventReward(Obj);
}

void GameProtocol::GCMasterLevelUpMsgSend(CGameObject &Obj)
{

	PMSG_MASTER_LEVEL_UP_SEND pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x51, sizeof(pMsg));

	pMsg.MasterLevel = Obj.m_PlayerData->MasterLevel;
	pMsg.MLPoint = Obj.m_PlayerData->MasterPoint;
	pMsg.GainPoint = g_MaxStatsInfo.GetClass.MLPointPerLevel;
	pMsg.MaxPoint = g_MaxStatsInfo.GetClass.MLUserMaxLevel;
	pMsg.MaxLife = Obj.MaxLife + Obj.AddLife;
	pMsg.MaxMana = Obj.MaxMana + Obj.AddMana;
	pMsg.MaxBP = Obj.MaxBP + Obj.AddBP;
	pMsg.MaxShield = Obj.iMaxShield + Obj.iAddShield;
	pMsg.IGCMaxLife = Obj.MaxLife + Obj.AddLife;
	pMsg.IGCMaxMana = Obj.MaxMana + Obj.AddMana;
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

	GCSendEffectInfo(Obj, 16);
}

struct PMSG_REQ_REGISTER_LUCKYCOIN
{
	PBMSG_HEAD2 h;
	int iIndex;
	char szAccountID[11];
	BYTE btPos;
};

void GameProtocol::CGReqLuckyCoinInfo(CGameObject &Obj)
{
	if (Obj.UseEventServer == FALSE)
	{
		PMSG_REQ_LUCKYCOIN pMsg = { 0 };
		PHeadSubSetB((BYTE*)&pMsg, 0xBE, 0x19, sizeof(pMsg));

		std::memcpy(pMsg.szUID, Obj.AccountID, 10);
		pMsg.szUID[10] = 0;
		pMsg.iIndex = Obj.m_Index;

		wsDataCli.DataSend((PCHAR)&pMsg, pMsg.h.size);

		Obj.UseEventServer = TRUE;
	}
}

void GameProtocol::CGReqLuckyCoinRegister(PMSG_REQ_LUCKYCOIN_REGISTER *aRecv, CGameObject &Obj)
{

	if (gObjIsConnectedGP(Obj) == false)
	{
		return;
	}

	if (Obj.m_IfState->type == 1)
	{
		return;
	}

	if (Obj.m_IfState->type != 7)
	{
		return;
	}

	if (Obj.UseEventServer == FALSE)
	{
		BYTE btPos = 0xFF;

		for (int i = INVENTORY_BAG_START; i < MAIN_INVENTORY_SIZE; i++)
		{
			if (Obj.pntInventory[i]->IsItem() == TRUE)
			{
				if (Obj.pntInventory[i]->m_Type == ITEMGET(14, 100))
				{
					btPos = i;
					break;
				}
			}
		}

		if (btPos == 0xFF)
		{
			PMSG_ANS_LUCKYCOIN_REGISTER pMsg;
			PHeadSubSetB((BYTE*)&pMsg, 0xBF, 0x0C, sizeof(pMsg));

			pMsg.btResult = 0;
			pMsg.iLuckyCoin = 0;

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

			return;
		}

		Obj.UseEventServer = TRUE;

		PMSG_REQ_REGISTER_LUCKYCOIN pMsg = { 0 };
		PHeadSubSetB((BYTE*)&pMsg, 0xBE, 0x18, sizeof(pMsg));

		pMsg.btPos = btPos;
		pMsg.iIndex = Obj.m_Index;
		std::memcpy(pMsg.szAccountID, Obj.AccountID, 10);

		wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);

	}
	return;
}

void GameProtocol::CGReqLuckyCoinTrade(PMSG_REQ_LUCKYCOIN_TRADE * aRecv, CGameObject &Obj)
{

	if (gObjIsConnectedGP(Obj) == false)
	{
		return;
	}

	if (Obj.m_IfState->type == 1)
	{
		return;
	}

	if (Obj.m_IfState->type != 7)
	{
		return;
	}

	int Count = 0;
	CItemObject Item;
	time_t Duration;
	int iBagResult = 0;

	for (int i = INVENTORY_BAG_START; i < MAIN_INVENTORY_SIZE; ++i)
	{
		if (Obj.pntInventory[i]->IsItem() == TRUE && Obj.pntInventory[i]->m_Type == ITEMGET(14, 100))
			Count += Obj.pntInventory[i]->m_Durability;
	}

	if (Count < aRecv->iLuckyCoinTradeCount)
	{
		GCAnsLuckyCoinTrade(Obj, 0);
		return;
	}

	if (!CheckInventoryEmptySpace(lpObj, 4, 2))
	{
		GCAnsLuckyCoinTrade(Obj, 2);
		return;
	}

	Count = aRecv->iLuckyCoinTradeCount;

	for (int i = INVENTORY_BAG_START; i < MAIN_INVENTORY_SIZE; ++i)
	{
		if (Obj.pntInventory[i]->IsItem() == TRUE && Obj.pntInventory[i]->m_Type == ITEMGET(14, 100))
		{
			if (Obj.pntInventory[i]->m_Durability > Count)
			{
				Obj.pntInventory[i]->m_Durability -= Count;
				GCItemObjectDurSend2(Obj, i, Obj.pntInventory[i]->m_Durability, 0);
				Count = 0;
			}
			else
			{
				Count -= Obj.pntInventory[i]->m_Durability;
				gObjInventoryDeleteItem(Obj, i);
				GCInventoryItemDeleteSend(Obj, i, 1);
			}
			if (Count == 0)
				break;
		}
	}

	switch (aRecv->iLuckyCoinTradeCount)
	{
	case 10:
		iBagResult = g_BagManager.GetItemFromBag(Obj, BAG_EVENT, EVENTBAG_LUCKYCOIN10, Obj.m_Index, Item, Duration);
		break;
	case 20:
		iBagResult = g_BagManager.GetItemFromBag(Obj, BAG_EVENT, EVENTBAG_LUCKYCOIN20, Obj.m_Index, Item, Duration);
		break;
	case 30:
		iBagResult = g_BagManager.GetItemFromBag(Obj, BAG_EVENT, EVENTBAG_LUCKYCOIN30, Obj.m_Index, Item, Duration);
		break;
	default:
		GCAnsLuckyCoinTrade(Obj, 4);
		return;
	}

	GJSetCharacterInfo(Obj, FALSE);

	if (iBagResult == 0)
	{
		GCAnsLuckyCoinTrade(Obj, 2);
		return;
	}

	else if (iBagResult == 2 || iBagResult == 3)
	{
		GCAnsLuckyCoinTrade(Obj, 1);
		return;
	}

	ItemCreate(Obj, 235, 0, 0, Item.m_Type, Item.m_Level, Item.m_Durability, Item.m_Option1, Item.m_Option2, Item.m_Option3, Obj, Item.m_NewOption, Item.m_SetOption, Duration, 0, 0);
	//sLog->outBasic("[Lucky Coin] [%s][%s] Trade Lucky Coin [%d]", Obj.AccountID, Obj.Name, aRecv->iLuckyCoinTradeCount);
	Item.Convert(Item.m_Type, Item.m_Option1, Item.m_Option2, Item.m_Option3, Item.m_NewOption, Item.m_SetOption, Item.m_ItemOptionEx, 0, 0, Duration, CURRENT_DB_VERSION);
}

void GameProtocol::GCAnsLuckyCoinTrade(CGameObject &Obj, BYTE Result)
{
	PMSG_ANS_LUCKYCOIN_TRADE pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xBF, 0x0D, sizeof(pMsg));

	pMsg.btResult = Result;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCSendObjectHP(CGameObject &Obj, int aTargetIndex)
{
	PMSG_TARGET_HP pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xFA, 0x05, sizeof(pMsg));

	pMsg.m_Index = aTargetIndex;
	pMsg.m_Hp = getGameObject(aTargetIndex)->Life;
	pMsg.m_MaxHp = getGameObject(aTargetIndex)->MaxLife + getGameObject(aTargetIndex)->AddLife;
	pMsg.m_Level = getGameObject(aTargetIndex)->Level;
	pMsg.m_bIceEffect = gObjCheckUsedBuffEffect(*getGameObject(aTargetIndex), BUFFTYPE_FREEZE);
	pMsg.m_bPoisonEffect = gObjCheckUsedBuffEffect(*getGameObject(aTargetIndex), BUFFTYPE_POISON);

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	this->MsgSendV2(*getGameObject(aTargetIndex), (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::CGReqAntiCheatRecv(CGameObject &Obj, PMSG_SEND_AH_INFO *aRecv)
{
	if (!ObjectMaxRange(Obj))
	{
		return;
	}

	if (!gObjIsConnected(Obj))
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.Connected != PLAYER_PLAYING)
	{
		return;
	}

	if (Obj.m_PlayerData->LivePacketTime == 0 || g_ConfigRead.antihack.EnablePacketTimeCheck == FALSE)
	{
		Obj.m_PlayerData->LivePacketTime = GetTickCount();
	}

	else
	{
		DWORD tick = GetTickCount() - Obj.m_PlayerData->LivePacketTime;

		if (tick < g_ConfigRead.antihack.PacketTimeMinTime)
		{
			sLog->outError( "[ANTI-HACK] (%s)(%s)(%s) (Map:%d)(X:%d)(Y:%d) SpeedHack detected -> Tick (Current:%d/Minimum:%d)",
				Obj.AccountID, Obj.Name, Obj.m_PlayerData->ConnectUser->IP, Obj.MapNumber, Obj.X, Obj.Y, tick, g_ConfigRead.antihack.PacketTimeMinTime);

			AntiHackLog->Output("[ANTI-HACK] (%s)(%s)(%s) (Map:%d)(X:%d)(Y:%d) SpeedHack detected -> Tick (Current:%d/Minimum:%d)",
				Obj.AccountID, Obj.Name, Obj.m_PlayerData->ConnectUser->IP, Obj.MapNumber, Obj.X, Obj.Y, tick, g_ConfigRead.antihack.PacketTimeMinTime);

			Obj.m_PlayerData->LiveCheckDetectCount++;

			if (Obj.m_PlayerData->LiveCheckDetectCount >= 3)
			{

				if (g_ConfigRead.antihack.EnableHackDetectMessage == TRUE)
				{
					MsgOutput(Obj, (char *)g_ConfigRead.antihack.HackDetectMessage.c_str());
				}

				if (g_ConfigRead.EnableAutoBanUserAccount == TRUE)
				{
					GDReqBanUser(Obj, 1, 1);
				}

				//this->GCSendDisableReconnect(Obj);
				IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);

				return;
			}
		}

		else
		{
			Obj.m_PlayerData->LivePacketTime = GetTickCount();
		}
	}

	if (g_ConfigRead.antihack.EnabledAgiCheck == false)
	{
		Obj.AgiCheckTime = GetTickCount();
		return;
	}

	if (Obj.m_PlayerData->BlessSkillCast == true)
	{
		Obj.AgiCheckTime = GetTickCount();
		return;
	}

	WORD Agility = Obj.m_PlayerData->Dexterity + Obj.AddDexterity;

	int AgilityDiff = (Obj.m_PlayerData->Dexterity + Obj.AddDexterity) - aRecv->Agi;

	if (aRecv->type == 1)
	{
		if (AgilityDiff <= -30 || AgilityDiff >= 30)
		{
			sLog->outError( "[ANTI-HACK] (%s)(%s)(%s) Dexterity check error: %d/%d", Obj.AccountID, Obj.Name, Obj.m_PlayerData->ConnectUser->IP,
				Agility, aRecv->Agi);

			AntiHackLog->Output("[ANTI-HACK] (%s)(%s)(%s) Dexterity check error: %d/%d", Obj.AccountID, Obj.Name, Obj.m_PlayerData->ConnectUser->IP,
				Agility, aRecv->Agi);

			if (g_ConfigRead.antihack.EnableHackDetectMessage == TRUE)
			{
				MsgOutput(Obj, (char *)g_ConfigRead.antihack.HackDetectMessage.c_str());
			}

			if (g_ConfigRead.EnableAutoBanUserAccount == TRUE)
			{
				GDReqBanUser(Obj, 1, 1);
			}

			//this->GCSendDisableReconnect(Obj);
			IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
		}

		else
		{
			Obj.AgiCheckTime = GetTickCount();
		}
	}
	else
	{
		sLog->outError( "[ANTI-HACK] (%s)(%s)(%s) Cheat detected: %s", Obj.AccountID, Obj.Name, Obj.m_PlayerData->ConnectUser->IP,
			aRecv->Agi == 0 ? "SpeedHack" : "Proxy");

		AntiHackLog->Output("[ANTI-HACK] (%s)(%s)(%s) Cheat detected: %s", Obj.AccountID, Obj.Name, Obj.m_PlayerData->ConnectUser->IP,
			aRecv->Agi == 0 ? "SpeedHack" : "Proxy");

		if (g_ConfigRead.antihack.EnableHackDetectMessage == TRUE)
		{
			MsgOutput(Obj, (char *)g_ConfigRead.antihack.HackDetectMessage.c_str());
		}

		if (g_ConfigRead.EnableAutoBanUserAccount == TRUE)
		{
			GDReqBanUser(Obj, 1, 1);
		}

		//this->GCSendDisableReconnect(Obj);
		IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);

	}
}

void GameProtocol::CGReqFileCrcRecv(CGameObject &Obj, PMSG_REQ_CRCDATA *aRecv)
{
	if (!gObjIsConnectedGP(Obj))
	{
		return;
	}

	

	if (g_ConfigRead.MainCRC == 0)
	{
		Obj.CrcCheckTime = 0;
		return;
	}

	if (aRecv->MainExe == g_ConfigRead.MainCRC &&
		aRecv->IGCDLL == g_ConfigRead.DLLCRC &&
		aRecv->PlayerBMD == g_ConfigRead.PlayerCRC &&
		aRecv->SkillCRC == g_ConfigRead.SkillCRC
		)
	{
		Obj.CrcCheckTime = 0;
	}

	else
	{
		sLog->outBasic("[ANTI-HACK] (%s)(%s)(%s) Invalid Game-Client files (Recv/Valid) (Main:%X/%X) (IGC:%X/%X) (Player:%X/%X) (SkillCRC: %X/%X)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->ConnectUser->IP, aRecv->MainExe, g_ConfigRead.MainCRC,
			aRecv->IGCDLL, g_ConfigRead.DLLCRC, aRecv->PlayerBMD, g_ConfigRead.PlayerCRC, aRecv->SkillCRC, g_ConfigRead.SkillCRC);

		AntiHackLog->Output("[ANTI-HACK] (%s)(%s)(%s) Invalid Game-Client files (Recv/Valid) (Main:%X/%X) (IGC:%X/%X) (Player:%X/%X) (SkillCRC: %X/%X)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->ConnectUser->IP, aRecv->MainExe, g_ConfigRead.MainCRC,
			aRecv->IGCDLL, g_ConfigRead.DLLCRC, aRecv->PlayerBMD, g_ConfigRead.PlayerCRC, aRecv->SkillCRC, g_ConfigRead.SkillCRC);

		if (g_ConfigRead.antihack.EnableHackDetectMessage == TRUE)
		{
			MsgOutput(Obj, (char *)g_ConfigRead.antihack.HackDetectMessage.c_str());
		}

		if (g_ConfigRead.EnableAutoBanUserAccount == TRUE)
		{
			GDReqBanUser(Obj, 1, 1);
		}

		//this->GCSendDisableReconnect(Obj);
		IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);

	}
}

void GameProtocol::GCElementalDamageSend(CGameObject &Obj, int TargetIndex, int Damage, int Elemental)
{
	PMSG_ELEMENTALDAMAGE pResult;
	PHeadSetB((BYTE*)&pResult, 0xD8, sizeof(pResult));

	pResult.NumberH = SET_NUMBERH(TargetIndex);
	pResult.NumberL = SET_NUMBERL(TargetIndex);
	pResult.Damage = Damage;
	pResult.btElement = Elemental;
	pResult.New = 0; ///NEW
	pResult.New1 = getGameObject(TargetIndex)->Life; ///[k2)
	pResult.New2 = 512; ///NEW
	pResult.btTargetH = SET_NUMBERH(Obj.m_Index);
	pResult.btTargetL = SET_NUMBERL(Obj.m_Index);

	if (getGameObject(TargetIndex)->Type == OBJ_USER)
	{
		IOCP.DataSend(TargetIndex, (BYTE*)&pResult, pResult.h.size);
	}

	if (cManager.WatchTargetIndex == TargetIndex || cManager.WatchTargetIndex == Obj)
	{
		cManager.DataSend((BYTE*)&pResult, pResult.h.size);
	}

	if (Obj.Type == OBJ_USER)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
	}
}

void GameProtocol::CGReqEnterAcheron(CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj))
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	int iTargetNumber = Obj.TargetNumber;

	if (iTargetNumber == -1)
	{
		return;
	}

	if (getGameObject(iTargetNumber)->Class != 580)
	{
		return;
	}

	BYTE pos = 0xFF;

	for (int i = 12; i < MAIN_INVENTORY_SIZE; i++)
	{
		if (Obj.pntInventory[i]->m_Type == ITEMGET(13, 146))
		{
			pos = i;
			break;
		}
	}

	if (pos == 0xFF)
	{
		PMSG_RESULT pMsg;
		PHeadSubSetB((BYTE*)&pMsg, 0xF8, 0x21, sizeof(pMsg));

		pMsg.result = 0x01;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	gObjMoveGate(Obj, 417);
	sLog->outBasic("[%s][%s] Entered to Acheron Map (Spirit Map serial: %I64d)", Obj.AccountID, Obj.Name,
		Obj.pntInventory[pos]->m_Number);

	gObjInventoryDeleteItem(Obj, pos);
	this->GCInventoryItemDeleteSend(Obj, pos, 1);
}

void GameProtocol::GCObjectLifeInfo(CGameObject &Obj, int sObjNum, int iMaxLife, int iCurLife)
{
	PMSG_OBJECT_LIFE_INFO pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xEC, 0x10, sizeof(pMsg));

	pMsg.ObjNumberH = HIBYTE(sObjNum);
	pMsg.ObjNumberL = LOBYTE(sObjNum);

	pMsg.MaxLifeHH = iMaxLife >> 24;
	pMsg.MaxLifeHL = iMaxLife >> 8;
	pMsg.MaxLifeLH = iMaxLife >> 16;
	pMsg.MaxLifeLL = iMaxLife;

	pMsg.CurLifeHH = iCurLife >> 24;
	pMsg.CurLifeHL = iCurLife >> 8;
	pMsg.CurLifeLH = iCurLife >> 16;
	pMsg.CurLifeLL = iCurLife;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
}

void GameProtocol::GCChaosMachinePriceSend(CGameObject &Obj)
{
	if (Obj.Type != OBJ_USER)
	{
		sLog->outError( "[ERROR] Index %d not USER", Obj);
		return;
	}

	BYTE SendData[65535];
	PMSG_PRICE_INFO pMsg;
	PMSG_PRICE_ITEM_LIST pList;
	pMsg.h.c = 0xC2;
	pMsg.h.headcode = 0xFA;
	pMsg.subc = 0x96;
	int lOfs = 0;
	lOfs += sizeof(pMsg);

	pMsg.count = 0;
	pMsg.Type = 1;
	for (short i = 0; i < CHAOS_BOX_SIZE; ++i)
	{
		if (Obj.pntChaosBox[i]->IsItem())
		{
			Obj.pntChaosBox[i]->Value();
			pList.BuyPrice = Obj.pntChaosBox[i]->m_BuyMoney;
			pList.SellPrice = Obj.pntChaosBox[i]->m_SellMoney;
			pList.Pos = i;
			std::memcpy(&SendData[lOfs], &pList, sizeof(pList));
			lOfs += sizeof(pList);
			pMsg.count++;
		}
	}
	int size = lOfs + pMsg.count * sizeof(PMSG_PRICE_ITEM_LIST);
	pMsg.h.sizeL = SET_NUMBERL(size);
	pMsg.h.sizeH = SET_NUMBERH(size);
	std::memcpy(&SendData[0], &pMsg, sizeof(pMsg));
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, SendData, size);

}
void GameProtocol::GCPriceSend(CGameObject &Obj, BYTE type, SHOP_DATA *lpShopData)
{
	if (Obj.Type != OBJ_USER)
	{
		sLog->outError( "[ERROR] Index %d not USER", Obj);
		return;
	}

	BYTE SendData[65535];
	PMSG_PRICE_INFO pMsg;
	PMSG_PRICE_ITEM_LIST pList;
	pMsg.h.c = 0xC2;
	pMsg.h.headcode = 0xFA;
	pMsg.subc = 0x95;
	int lOfs = 0;
	lOfs += sizeof(pMsg);

	if (type == 0) // shop
	{
		if (!lpShopData)
		{
			return;
		}

		pMsg.count = lpShopData->m_ShopData.ItemCount;
		int size = lOfs + pMsg.count * sizeof(PMSG_PRICE_ITEM_LIST);
		pMsg.h.sizeL = SET_NUMBERL(size);
		pMsg.h.sizeH = SET_NUMBERH(size);
		pMsg.Type = 0;
		std::memcpy(SendData, &pMsg, sizeof(pMsg));

		for (int i = 0; i < 120; i++)
		{
			if (lpShopData->m_ShopData.m_item[i].IsItem())
			{
				lpShopData->m_ShopData.m_item[i].Value();
				pList.BuyPrice = lpShopData->m_ShopData.m_item[i].m_BuyMoney;
				pList.SellPrice = lpShopData->m_ShopData.m_item[i].m_SellMoney;
				pList.Pos = i;
				std::memcpy(&SendData[lOfs], &pList, sizeof(pList));
				lOfs += sizeof(pList);
			}
		}

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, SendData, size);
	}

	if (type == 1) // inventory
	{
		pMsg.count = 0;
		pMsg.Type = 1;

		for (int i = 0; i < INVENTORY_SIZE; ++i)
		{
			if (Obj.pntInventory[i]->IsItem())
			{
				Obj.pntInventory[i]->Value();
				pList.BuyPrice = Obj.pntInventory[i]->m_BuyMoney;
				pList.SellPrice = Obj.pntInventory[i]->m_SellMoney;
				pList.Pos = i;
				std::memcpy(&SendData[lOfs], &pList, sizeof(pList));
				lOfs += sizeof(pList);
				pMsg.count++;
			}
		}

		int size = lOfs + pMsg.count * sizeof(PMSG_PRICE_ITEM_LIST);
		pMsg.h.sizeL = SET_NUMBERL(size);
		pMsg.h.sizeH = SET_NUMBERH(size);
		std::memcpy(&SendData[0], &pMsg, sizeof(pMsg));
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, SendData, size);
	}
}

int GameProtocol::OnCGInviteDuel(PMSG_REQ_DUEL_INVITE* lpMsg, CGameObject &Obj)
{
	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		return ENEWPVP::E_NOT_DUEL_CSSERVER;
	}

	int nResponsor = MAKEWORD(lpMsg->NumberL, lpMsg->NumberH);

	CGameObject* lpResponsor = (CGameObject*) getGameObject(nResponsor);

	if (!PacketCheckTime(Obj)) return ENEWPVP::E_LIMIT_PACKETTIME;
	if (lpResponsor->m_Index == Obj.m_Index)			return ENEWPVP::E_INVITE_MYSELF;
	if (g_MaxStatsInfo.GetClass.IsNonPvP[Obj.MapNumber])
	{
		GCServerMsgStringSend(Lang.GetText(0, 97), Obj, 1);
		return ENEWPVP::E_NON_PKSERVER;
	}
	if (Obj.CloseType != -1)	return ENEWPVP::E_CONNECTION_CLOSING;
	if (g_NewPVP.CheckPKPenalty(Obj))
	{
		GCServerMsgStringSend(Lang.GetText(0, 98), Obj, 1);
		return ENEWPVP::E_NOT_DUEL_MURDERER;
	}
	if (!gObjIsConnectedEx(*lpResponsor))
	{
		sLog->outBasic("error: %s %d (%d)", __FILE__, __LINE__, nResponsor);
		return ENEWPVP::E_DISCONNECT;
	}
	CGameObject* lpResponsor = (CGameObject*)*getGameObject(nResponsor);
	if (g_NewPVP.CheckPKPenalty(*lpResponsor))
	{
		GCServerMsgStringSend(Lang.GetText(0, 99), Obj, 1);
		return ENEWPVP::E_NOT_DUEL_MURDERER;
	}

	char szDuelName[MAX_ACCOUNT_LEN + 1] = { 0 };
	char szDuelName2[MAX_ACCOUNT_LEN + 1] = { 0 };

	std::memcpy(szDuelName, lpResponsor->Name, MAX_ACCOUNT_LEN);
	szDuelName[MAX_ACCOUNT_LEN] = '\0';

	std::memcpy(szDuelName2, lpMsg->szName, MAX_ACCOUNT_LEN);
	szDuelName2[MAX_ACCOUNT_LEN] = '\0';

	if (strcmp(szDuelName, szDuelName2) != 0)
	{
		GCServerMsgStringSend(Lang.GetText(0, 85), Obj, 1);
		return ENEWPVP::E_FAILED_RESULT;
	}
	if (!IsDuelEnable(*lpResponsor))
	{
		GCServerMsgStringSend(Lang.GetText(0, 86), Obj, 1);
		return ENEWPVP::E_FAILED_RESULT;
	}
	if (g_NewPVP.IsGuildWar(Obj))
	{
		GCServerMsgStringSend(Lang.GetText(0, 87), Obj, 1);
		return ENEWPVP::E_GUILDWAR;
	}
	if (g_NewPVP.IsGuildWar(*lpResponsor))
	{
		GCServerMsgStringSend(Lang.GetText(0, 88), Obj, 1);
		return ENEWPVP::E_GUILDWAR;
	}
	if (g_NewPVP.IsSelfDefense(Obj) || g_NewPVP.IsSelfDefense(*lpResponsor))
	{
		GCServerMsgStringSend(Lang.GetText(0, 108), Obj, 1);
		return ENEWPVP::E_SELFDEFENSE;
	}
	if (CC_MAP_RANGE(Obj.MapNumber) || CC_MAP_RANGE(lpResponsor->MapNumber))
	{
		GCServerMsgStringSend(Lang.GetText(0, 125), Obj, 1);
		return ENEWPVP::E_INVALID_MAP;
	}
	if (DS_MAP_RANGE(Obj.MapNumber) || DS_MAP_RANGE(lpResponsor->MapNumber))
	{
		GCServerMsgStringSend(Lang.GetText(0, 100), Obj, 1);
		return ENEWPVP::E_INVALID_MAP;
	}
	if (BC_MAP_RANGE(Obj.MapNumber) || BC_MAP_RANGE(lpResponsor->MapNumber))
	{
		GCServerMsgStringSend(Lang.GetText(0, 89), Obj, 1);
		return ENEWPVP::E_INVALID_MAP;
	}
	if (IT_MAP_RANGE(Obj.MapNumber) || IT_MAP_RANGE(lpResponsor->MapNumber))
	{
		GCServerMsgStringSend(Lang.GetText(0, 300), Obj, 1);
		return ENEWPVP::E_INVALID_MAP;
	}
	if (DG_MAP_RANGE(Obj.MapNumber) || DG_MAP_RANGE(lpResponsor->MapNumber))
	{
		GCServerMsgStringSend(Lang.GetText(0, 301), Obj, 1);
		return ENEWPVP::E_INVALID_MAP;
	}
	if (IMPERIAL_MAP_RANGE(Obj.MapNumber) || IMPERIAL_MAP_RANGE(lpResponsor->MapNumber))
	{
		GCServerMsgStringSend(Lang.GetText(0, 302), Obj, 1);
		return ENEWPVP::E_INVALID_MAP;
	}
	if (Obj.m_IfState->use > 0)
	{
		GCServerMsgStringSend(Lang.GetText(0, 90), Obj, 1);
		return ENEWPVP::E_FAILED_RESULT;
	}
	if (lpResponsor->m_IfState->use > 0)
	{
		GCServerMsgStringSendEx(Obj, 1, Lang.GetText(0, 91), lpResponsor->Name);
		return ENEWPVP::E_FAILED_RESULT;
	}
	int nRet = g_NewPVP.Reserve(Obj, *lpResponsor);
	if (nRet != ENEWPVP::E_NO_ERROR)
	{
		switch (nRet)
		{
		case ENEWPVP::E_ALREADY_DUELRESERVED:
			GCServerMsgStringSendEx(Obj, 1, Lang.GetText(0, 93), lpResponsor->Name);
			GCServerMsgStringSend(Lang.GetText(0, 84), Obj, 1);
			break;
		case ENEWPVP::E_ALREADY_DUELLING:
			GCServerMsgStringSend(Lang.GetText(0, 83), Obj, 1);
			GCServerMsgStringSend(Lang.GetText(0, 84), Obj, 1);
			break;
		case ENEWPVP::E_ALREADY_DUELREQUESTED_1:
			GCServerMsgStringSendEx(Obj, 1, Lang.GetText(0, 92), lpResponsor->Name);
			break;
		case ENEWPVP::E_ALREADY_DUELRESERVED_1:
			GCServerMsgStringSendEx(Obj, 1, Lang.GetText(0, 93), lpResponsor->Name);
			break;
		case ENEWPVP::E_ALREADY_DUELLING_1:
			GCServerMsgStringSendEx(Obj, 1, Lang.GetText(0, 94), lpResponsor->Name);
			break;
		}
		return nRet;
	}

	GCServerMsgStringSendEx(Obj, 1, Lang.GetText(0, 95), lpResponsor->Name);
	sLog->outBasic("[NewPVP] [%s][%s] Requested to Start Duel to [%s][%s]", Obj.AccountID, Obj.Name, lpResponsor->AccountID, lpResponsor->Name);
	return ENEWPVP::E_NO_ERROR;
}

int GameProtocol::OnCGAnswerDuel(PMSG_ANS_DUEL_ANSWER* lpMsg, CGameObject &Obj)
{
	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		return ENEWPVP::E_NOT_DUEL_CSSERVER;
	}

	CGameObject* lpRequester = &Obj;
	CGameObject* lpResponsor = (CGameObject*) Obj;
	int nRequester = MAKEWORD(lpMsg->NumberL, lpMsg->NumberH);

	if (!gObjIsConnectedEx(Obj))	return ENEWPVP::E_DISCONNECT;
	lpRequester = (CGameObject*) getGameObject(nRequester);

	PMSG_ANS_DUEL_INVITE res = { 0 };
	res.h.c = 0xC1;
	res.h.size = sizeof(PMSG_ANS_DUEL_INVITE);
	res.h.headcode = 0xAA;
	res.h.subcode = 0x01;
	std::memcpy(res.szName, lpResponsor->Name, MAX_ACCOUNT_LEN);
	if (lpResponsor->CloseType != -1)
	{
		g_NewPVP.Cancel(*lpRequester, *lpResponsor, FALSE);
		res.nResult = ENEWPVP::E_CONNECTION_CLOSING;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&res, res.h.size);
		return ENEWPVP::E_CONNECTION_CLOSING;
	}
	if (!lpMsg->bDuelOK)
	{
		g_NewPVP.Cancel(*lpRequester, *lpResponsor, FALSE);
		res.nResult = ENEWPVP::E_REFUSE_INVATE;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&res, res.h.size);
		return ENEWPVP::E_REFUSE_INVATE;
	}
	if (BC_MAP_RANGE(lpRequester->MapNumber) ||
		CC_MAP_RANGE(lpRequester->MapNumber) ||
		DS_MAP_RANGE(lpRequester->MapNumber) ||
		IT_MAP_RANGE(lpRequester->MapNumber) ||
		DG_MAP_RANGE(lpRequester->MapNumber) ||
		IMPERIAL_MAP_RANGE(lpRequester->MapNumber) ||
		lpRequester->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		g_NewPVP.Cancel(*lpRequester, *lpResponsor, FALSE);
		GCServerMsgStringSend(Lang.GetText(0, 125), *lpResponsor, 1);
		GCServerMsgStringSend(Lang.GetText(0, 125), *lpRequester, 1);
		res.nResult = ENEWPVP::E_INVALID_MAP;
		IOCP.DataSend(lpResponsor->m_Index, (BYTE*)&res, res.h.size);
		IOCP.DataSend(lpRequester->m_Index, (BYTE*)&res, res.h.size);
		return ENEWPVP::E_INVALID_MAP;
	}
	int nRet = g_NewPVP.Join(*lpRequester, *lpResponsor);
	if (nRet != ENEWPVP::E_NO_ERROR)
	{
		switch (nRet)
		{
		case ENEWPVP::E_ALREADY_DUELRESERVED_1:
			GCServerMsgStringSend(Lang.GetText(0, 96), Obj, 1);
			break;
		case ENEWPVP::E_ALREADY_DUELLING_1:
			GCServerMsgStringSend(Lang.GetText(0, 83), Obj, 1);
			GCServerMsgStringSend(Lang.GetText(0, 84), Obj, 1);
			break;
		}
		res.nResult = nRet;
		IOCP.DataSend(lpRequester->m_Index, (BYTE*)&res, res.h.size);
		IOCP.DataSend(lpResponsor->m_Index, (BYTE*)&res, res.h.size);
		g_NewPVP.Cancel(*lpRequester, *lpResponsor, FALSE);
		return nRet;
	}
	return ENEWPVP::E_NO_ERROR;
}

int GameProtocol::OnCGLeaveDuel(PMSG_REQ_DUEL_EXIT* lpMsg, CGameObject &Obj)
{
	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		return ENEWPVP::E_NOT_DUEL_CSSERVER;
	}

	if (Obj.CloseType != -1)	return ENEWPVP::E_DISCONNECT;
	int nRet = g_NewPVP.Leave(Obj);
	if (nRet != ENEWPVP::E_NO_ERROR)	return nRet;
	return ENEWPVP::E_NO_ERROR;
}

int GameProtocol::OnDuelChannelJoin(PMSG_REQ_DUEL_JOINCNANNEL* lpMsg, CGameObject &Obj)
{
	if (Obj.CloseType != -1)	return ENEWPVP::E_DISCONNECT;
	int nRet = g_NewPVP.JoinChannel(lpMsg->nChannelId, Obj);
	if (nRet != ENEWPVP::E_NO_ERROR)	return nRet;
	return ENEWPVP::E_NO_ERROR;
}

int GameProtocol::OnDuelChannelLeave(PMSG_REQ_DUEL_LEAVECNANNEL* lpMsg, CGameObject &Obj)
{
	if (Obj.CloseType != -1)	return ENEWPVP::E_DISCONNECT;
	int nRet = g_NewPVP.LeaveChannel(lpMsg->nChannelId, Obj);
	if (nRet != ENEWPVP::E_NO_ERROR)	return nRet;
	return ENEWPVP::E_NO_ERROR;
}

void GameProtocol::CGReqRegGensMember(PMSG_REQ_REG_GENS_MEMBER *lpMsg, CGameObject &Obj)
{
	BYTE bRet;

	

	GUILD_INFO_STRUCT *lpGuildInfo = Obj.m_PlayerData->lpGuild;

	//if (!ObjectMaxRange(Obj)) return;

	if (gObjIsConnectedGP(Obj))
	{
		PMSG_ANS_REG_GENS_MEMBER_EXDB pMsg;

		pMsg.bInfluence = lpMsg->bResult;

		//[GensSystem] [%s][%s] Gens Request - Join [%s]
		if (!g_GensSystem.GetGensInfluence(lpObj))
		{
			pMsg.bResult = 0; bRet = 1;
		}

		//GensSystem] [%s][%s] Gens Joining Failed to Gens be Same : [%s]
		if (g_GensSystem.GetGensInfluence(lpObj) > 0)
		{
			pMsg.bResult = 1; bRet = 0;
		}

		//[GensSystem] [%s][%s] Gens Joining Failed to Below Gens leaving Time
		//if (g_GensAbusingResetTime < 60)
		//{
		//	pMsg.bResult = 2; bRet = 1;
		//}

		//[GensSystem] [%s][%s] Gens Joining Failed to Below 50Lv
		if (Obj.Level < 50)
		{
			pMsg.bResult = 3; bRet = 0;
		}

		//[GensSystem] [%s][%s] Gens Joining Failed to Gens be Different Guild Leader [%s]
		if (Obj.m_PlayerData->GuildStatus >= 0)
		{
			pMsg.bResult = 4; bRet = 0;
		}

		//[GensSystem] [%s][%s] Gens Joining Failed to Not Register Guild Leader GuildName [%s]
		if (Obj.m_PlayerData->GuildStatus == GUILD_MASTER)
		{
			pMsg.bResult = 5; bRet = 0;
		}

		//[GensSystem] [%s][%s] Gens Joining Failed to Already Partymember
		if (Obj.PartyNumber >= 0)
		{
			pMsg.bResult = 6; bRet = 0;
		}

		//[GensSystem] [%s][%s] Gens Joining Failed to Union GuildMaster
		if (lpGuildInfo != NULL && lpGuildInfo->iGuildUnion)
		{
			pMsg.bResult = 7; bRet = 0;
		}

		if (Obj.m_PlayerData->m_bUsePartyMatching)
		{
			pMsg.bResult = 8; bRet = 0;
		}

		if (bRet)
		{
			g_GensSystem.ReqRegGensMember(lpObj, lpMsg->bResult);
		}
		else
		{
			pMsg.bIndexH = SET_NUMBERH(Obj.m_Index);
			pMsg.bIndexL = SET_NUMBERL(Obj.m_Index);
			DGAnsRegGensMember((PMSG_ANS_REG_GENS_MEMBER_EXDB*)&pMsg);
		}
	}
}

//-> Completed
void GameProtocol::CGReqSecedeGensMember(PMSG_REQ_SEGEDE_GENS_MEMBER *lpMsg, CGameObject &Obj)
{
	

	BYTE bRet = 1;

	if (gObjIsConnectedGP(Obj))
	{
		PMSG_ANS_SECEDE_GENS_MEMBER_EXDB pMsg;

		if (!g_GensSystem.IsInfluenceNPC(lpObj))
		{
			pMsg.bResult = 3; bRet = 0;
		}

		else if (Obj.m_PlayerData->GuildStatus == GUILD_MASTER)
		{
			pMsg.bResult = 2; bRet = 0;
		}

		//[GensSystem] [%s][%s] Gens Leaving Failed to Not Register Gens
		else if (!g_GensSystem.GetGensInfluence(lpObj))
		{
			pMsg.bResult = 1; bRet = 0;
		}

		else if (Obj.m_PlayerData->m_bUsePartyMatching)
		{
			pMsg.bResult = 4; bRet = 0;
		}

		else
		{
			pMsg.bResult = 0; bRet = 1;
		}

		if (bRet)
		{
			g_GensSystem.ReqSecedeGensMember(lpObj);
		}

		else
		{
			pMsg.bIndexH = SET_NUMBERH(Obj.m_Index);
			pMsg.bIndexL = SET_NUMBERL(Obj.m_Index);
			DGAnsSecedeGensMember((PMSG_ANS_SECEDE_GENS_MEMBER_EXDB*)&pMsg);
		}
	}
}

//-> Completed
void GameProtocol::CGReqGensReward(PMSG_GENS_REWARD_CODE *lpMsg, CGameObject &Obj)
{
	BYTE bRet = 0; //Not Used

	

	if (gObjIsConnectedGP(Obj))
	{
		g_GensSystem.ReqExDBGensRewardCheck(lpObj, lpMsg->bReward);
	}
}

//-> Completed
void GameProtocol::CGReqGensMemberInfo(PMSG_REQ_GENS_INFO *lpMsg, CGameObject &Obj)
{
	//if (!ObjectMaxRange(Obj)) return;

	

	if (gObjIsConnectedGP(Obj))
	{
		if (g_GensSystem.IsRegGensInfluence(lpObj))
		{
			g_GensSystem.ReqExDBGensInfo(lpObj);
		}
	}
}
void GameProtocol::CGInventoryEquipment(PMSG_REQ_INVENTORY_EQUIPMENT_ITEM *lpMsg, CGameObject &Obj)
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (!gObjIsConnected(Obj))
	{
		return;
	}

	

	PMSG_ANS_INVENTORY_EQUIPMENT_ITEM pMsg = { 0 };

	pMsg.btResult = 0;
	PHeadSubSetB((BYTE*)&pMsg, 0xBF, 0x20, sizeof(pMsg));

	pMsg.btItemPos = lpMsg->btItemPos;

	if (!IsInvenPet(Obj.pntInventory[lpMsg->btItemPos]->m_Type) &&
		Obj.pntInventory[lpMsg->btItemPos]->m_Durability == 0.0)
	{
		pMsg.btResult = -1;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	switch (Obj.pntInventory[lpMsg->btItemPos]->m_Type)
	{
	case ITEMGET(13, 128):
	case ITEMGET(13, 129):
		if (Obj.m_PlayerData->m_btSculptPos == 0)
		{
			if (lpMsg->btValue == (BYTE)-2 && Obj.pntInventory[lpMsg->btItemPos]->m_Durability == 255.0)
			{
				Obj.m_PlayerData->m_btSculptPos = lpMsg->btItemPos;
				Obj.pntInventory[lpMsg->btItemPos]->m_Durability = 254.0;
				pMsg.btResult = -2;
			}

			goto DEF_SWITCH;
		}

		if (Obj.m_PlayerData->m_btSculptPos < INVETORY_WEAR_SIZE || Obj.m_PlayerData->m_btSculptPos >= MAIN_INVENTORY_SIZE)
		{
			goto DEF_SWITCH;
		}

		if (lpMsg->btValue == (BYTE)-1 && Obj.m_PlayerData->m_btSculptPos == lpMsg->btItemPos)
		{
			if (Obj.pntInventory[lpMsg->btItemPos]->m_Durability == 254.0)
			{
				Obj.pntInventory[lpMsg->btItemPos]->m_Durability = 255.0;
				pMsg.btResult = -1;
			}

			goto DEF_SWITCH;
		}

		this->GCServerMsgStringSend(Lang.GetText(0, 548), Obj, 1);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		break;
	case ITEMGET(13, 130):
	case ITEMGET(13, 131):
	case ITEMGET(13, 132):
	case ITEMGET(13, 133):
		if (Obj.m_PlayerData->m_btCharmPos == 0)
		{
			if (lpMsg->btValue == (BYTE)-2 && Obj.pntInventory[lpMsg->btItemPos]->m_Durability == 255.0)
			{
				Obj.m_PlayerData->m_btCharmPos = lpMsg->btItemPos;
				Obj.pntInventory[lpMsg->btItemPos]->m_Durability = 254.0;
				pMsg.btResult = -2;
			}

			goto DEF_SWITCH;
		}

		if (Obj.m_PlayerData->m_btCharmPos < INVETORY_WEAR_SIZE || Obj.m_PlayerData->m_btCharmPos >= MAIN_INVENTORY_SIZE)
		{
			goto DEF_SWITCH;
		}

		if (lpMsg->btValue == (BYTE)-1 && Obj.m_PlayerData->m_btCharmPos == lpMsg->btItemPos)
		{
			if (Obj.pntInventory[lpMsg->btItemPos]->m_Durability == 254.0)
			{
				Obj.pntInventory[lpMsg->btItemPos]->m_Durability = 255.0;
				pMsg.btResult = -1;
			}

			goto DEF_SWITCH;
		}

		this->GCServerMsgStringSend(Lang.GetText(0, 548), Obj, 1);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		break;
	case ITEMGET(13, 134):
		if (Obj.m_PlayerData->m_btArtifactPos == 0)
		{
			if (lpMsg->btValue == (BYTE)-2 && Obj.pntInventory[lpMsg->btItemPos]->m_Durability == 255.0)
			{
				Obj.m_PlayerData->m_btArtifactPos = lpMsg->btItemPos;
				Obj.pntInventory[lpMsg->btItemPos]->m_Durability = 254.0;
				pMsg.btResult = -2;
			}

			goto DEF_SWITCH;
		}

		if (Obj.m_PlayerData->m_btArtifactPos < INVETORY_WEAR_SIZE || Obj.m_PlayerData->m_btArtifactPos >= MAIN_INVENTORY_SIZE)
		{
			goto DEF_SWITCH;
		}

		if (lpMsg->btValue == (BYTE)-1 && Obj.m_PlayerData->m_btArtifactPos == lpMsg->btItemPos)
		{
			if (Obj.pntInventory[lpMsg->btItemPos]->m_Durability == 254.0)
			{
				Obj.pntInventory[lpMsg->btItemPos]->m_Durability = 255.0;
				pMsg.btResult = -1;
			}

			goto DEF_SWITCH;
		}

		this->GCServerMsgStringSend(Lang.GetText(0, 548), Obj, 1);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		break;
	case ITEMGET(13, 177):
	case ITEMGET(13, 178):
	case ITEMGET(13, 179):
		if (Obj.m_PlayerData->m_btExpUpCharmPos == 0)
		{
			if (lpMsg->btValue == (BYTE)-2 && Obj.pntInventory[lpMsg->btItemPos]->m_Durability == 255.0)
			{
				Obj.m_PlayerData->m_btExpUpCharmPos = lpMsg->btItemPos;
				Obj.pntInventory[lpMsg->btItemPos]->m_Durability = 254.0;
				pMsg.btResult = -2;
			}

			goto DEF_SWITCH;
		}

		if (Obj.m_PlayerData->m_btExpUpCharmPos < INVETORY_WEAR_SIZE || Obj.m_PlayerData->m_btExpUpCharmPos >= MAIN_INVENTORY_SIZE)
		{
			goto DEF_SWITCH;
		}

		if (lpMsg->btValue == (BYTE)-1 && Obj.m_PlayerData->m_btExpUpCharmPos == lpMsg->btItemPos)
		{
			if (Obj.pntInventory[lpMsg->btItemPos]->m_Durability == 254.0)
			{
				Obj.pntInventory[lpMsg->btItemPos]->m_Durability = 255.0;
				pMsg.btResult = -1;
			}

			goto DEF_SWITCH;
		}

		this->GCServerMsgStringSend(Lang.GetText(0, 548), iIndex, 1);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		break;
	case ITEMGET(13, 2):
	case ITEMGET(13, 3):
	case ITEMGET(13, 4):
	case ITEMGET(13, 5):
	case ITEMGET(13, 37):
		if (Obj.m_btInvenPetPos != 0)
		{
			if (Obj.m_btInvenPetPos >= INVETORY_WEAR_SIZE && Obj.m_btInvenPetPos < MAIN_INVENTORY_SIZE)
			{
				if (lpMsg->btValue != (BYTE)-1 || Obj.m_btInvenPetPos != lpMsg->btItemPos)
				{
					this->GCServerMsgStringSend(Lang.GetText(0, 615), iIndex, 1);
					return;
				}

				if (gObjUseInvenPetCheck(lpObj, Obj.pntInventory[lpMsg->btItemPos], 0) == FALSE)
				{
					this->GCServerMsgStringSend(Lang.GetText(0, 616), iIndex, 1);
					return;
				}

				if (Obj.pntInventory[lpMsg->btItemPos]->m_JewelOfHarmonyOption == 1)
				{

					Obj.pntInventory[lpMsg->btItemPos]->m_JewelOfHarmonyOption = 0;
					Obj.m_wInvenPet = -1;
					pMsg.btResult = -1;
				}
			}
		}

		else if (lpMsg->btValue == (BYTE)-2)
		{

			if (Obj.m_wInvenPet != (WORD)-1 || Obj.pntInventory[lpMsg->btItemPos]->m_JewelOfHarmonyOption == 1)
			{

				this->GCServerMsgStringSend(Lang.GetText(0, 615), iIndex, 1);
				return;
			}

			if (gObjUseInvenPetCheck(lpObj, Obj.pntInventory[lpMsg->btItemPos], 1) == FALSE)
			{
				this->GCServerMsgStringSend(Lang.GetText(0, 617), iIndex, 1);
				return;
			}

			Obj.m_btInvenPetPos = lpMsg->btItemPos;
			Obj.pntInventory[lpMsg->btItemPos]->m_JewelOfHarmonyOption = 1;
			pMsg.btResult = (BYTE)-2;

		}

		gObjMakePreviewCharSet(Obj);
		this->GCEquipmentChange(iIndex, lpMsg->btItemPos);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		break;
	default:
	DEF_SWITCH:
		gObjCalCharacter.CalcCharacter(Obj);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		break;
	}
}

void GameProtocol::CGReqUseTransformationRing(PMSG_REQ_USETRANSFORMATIONRING * lpMsg, CGameObject &Obj)
{

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (lpMsg->btState == 1)
	{
		if (IsTransformCharacterSkin(Obj.m_Change) == TRUE)
		{
			Obj.m_PlayerData->m_EnableUseChangeSkin = 0;
		}
	}

	else
	{
		Obj.m_PlayerData->m_EnableUseChangeSkin = 1;
	}

	PMSG_MOVE pMove;

	pMove.h.c = 0xC1;
	pMove.h.headcode = 0xD7;
	pMove.h.size = sizeof(pMove);
	pMove.X = Obj.X;
	pMove.Y = Obj.Y;

	PMoveProc(&pMove, Obj);
	gObjViewportListProtocolCreate(Obj);
}

void GameProtocol::GCSendAttackSpeed(CGameObject &Obj)
{
	PMSG_ATTACKSPEEDSEND pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0xEC, 0x30, sizeof(pMsg));

	pMsg.AttackSpeed = Obj.m_AttackSpeed;

	pMsg.MagicSpeed = Obj.m_MagicSpeed;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::CGReqArcaBattleGuildMasterJoin(PMSG_REQ_ARCA_BATTLE_JOIN *lpMsg, CGameObject &Obj)
{
	if (gObjIsConnected(Obj) == false)
	{
		return;
	}

	GUILD_INFO_STRUCT * lpGuildInfo = Obj.m_PlayerData->lpGuild;
	PMSG_ANS_ARCA_BATTLE_GUILD_JOIN_DS pMsg;

	pMsg.wNumber = Obj.m_Index;

	if (!lpGuildInfo)
	{
		pMsg.btResult = 1;
		DGAnsArcaBattleGuildJoinSelect((PMSG_ANS_ARCA_BATTLE_GUILD_JOIN_DS *)&pMsg);
		return;
	}

	int iArcaBattleState = g_ArcaBattle.GetState();

	if (iArcaBattleState != 3)
	{
		pMsg.btResult = 5;
		DGAnsArcaBattleGuildJoinSelect((PMSG_ANS_ARCA_BATTLE_GUILD_JOIN_DS *)&pMsg);
		return;
	}

	if (Obj.m_PlayerData->GuildStatus != GUILD_MASTER)
	{
		pMsg.btResult = 1;
		DGAnsArcaBattleGuildJoinSelect((PMSG_ANS_ARCA_BATTLE_GUILD_JOIN_DS *)&pMsg);
		return;
	}

	if (lpGuildInfo->Count < g_ArcaBattle.GetJoinMemberCnt())
	{
		pMsg.btResult = 2;
		DGAnsArcaBattleGuildJoinSelect((PMSG_ANS_ARCA_BATTLE_GUILD_JOIN_DS *)&pMsg);
		return;
	}

	g_ArcaBattle.GDReqArcaBattleIsTopRank(Obj, Obj.m_PlayerData->GuildNumber);
}

void GameProtocol::CGReqArcaBattleGuildMemberJoin(PMSG_REQ_ARCA_BATTLE_JOIN *lpMsg, CGameObject &Obj)
{
	PMSG_ANS_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS pMsg;

	

	if (gObjIsConnected(Obj) == false)
	{
		return;
	}

	pMsg.wNumber = Obj.m_Index;

	if (Obj.m_PlayerData->lpGuild == NULL)
	{
		pMsg.btResult = 7;
		DGAnsArcaBattleGuildMemberJoin((PMSG_ANS_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS *)&pMsg);
		return;
	}

	if (Obj.m_PlayerData->GuildStatus == GUILD_MASTER)
	{
		pMsg.btResult = 13;
		DGAnsArcaBattleGuildMemberJoin((PMSG_ANS_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS *)&pMsg);
		return;
	}

	int iArcaBattleState = g_ArcaBattle.GetState();

	if (iArcaBattleState < 4)
	{
		pMsg.btResult = 14;
		DGAnsArcaBattleGuildMemberJoin((PMSG_ANS_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS *)&pMsg);
		return;
	}

	if (iArcaBattleState >= 7)
	{
		pMsg.btResult = 10;
		DGAnsArcaBattleGuildMemberJoin((PMSG_ANS_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS *)&pMsg);
		return;
	}

	GDReqArcaBattleGuildMemberJoin(lpObj);
}

void GameProtocol::CGReqArcaBattleEnter(PMSG_REQ_ARCA_BATTLE_ENTER *lpMsg, CGameObject &Obj)
{
	PMSG_ANS_ARCA_BATTLE_ENTER_DS pMsg;

	

	if (gObjIsConnected(Obj))
	{
		int iArcaBattleState = g_ArcaBattle.GetState();

		if (iArcaBattleState == 6 || iArcaBattleState == 7)
		{
			if (Obj.m_PlayerData->m_bUsePartyMatching)
			{
				PMSG_ANS_PARTYMATCHING_ERROR pMsg;
				PHeadSubSetB((BYTE*)&pMsg, 0xEF, 0x09, sizeof(pMsg));
				pMsg.iResult = -1;

				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
				return;
			}

			if (Obj.Teleport)
			{
				return;
			}

			BYTE btAttr = MapC[Obj.MapNumber].GetAttr(Obj.X, Obj.Y);

			if (btAttr & 1)
				GDReqArcaBattleEnter(lpObj, lpMsg->btEnterSeq);
		}

		else
		{
			pMsg.wNumber = Obj.m_Index;
			pMsg.btResult = 12;
			DGAnsArcaBattleEnter((PMSG_ANS_ARCA_BATTLE_ENTER_DS *)&pMsg);
		}
	}
}

void GameProtocol::CGReqAcheronEnter(PMSG_REQ_ACHERON_ENTER *lpMsg, CGameObject &Obj)
{
	PMSG_ANS_ACHERON_ENTER pMsg;

	if (gObjIsConnected(Obj) == false)
	{
		return;
	}

	for (int pos = INVETORY_WEAR_SIZE; pos < MAIN_INVENTORY_SIZE; pos++)
	{
		if (Obj.pntInventory[pos]->IsItem() && Obj.pntInventory[pos]->m_Type == ITEMGET(13, 146))
		{
			gObjInventoryDeleteItem(Obj, pos);
			this->GCInventoryItemDeleteSend(Obj, pos, 1);
			gObjMoveGate(Obj, 417);
			return;
		}
	}

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xF8;
	pMsg.h.subcode = 0x21;
	pMsg.btResult = 1;
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
}

void GameProtocol::CGReqArcaBattleBootyExchange(CGameObject &Obj)
{
	if (gObjIsConnected(Obj) == false)
	{
		return;
	}

	int bCanChaosBox = 1; // stub.
	if (bCanChaosBox == TRUE && !Obj.m_IfState->use)
	{
		if (Obj.m_bPShopOpen == TRUE)
		{
			sLog->outBasic("[%s][%s] is Already Opening PShop, ChaosBox Failed", Obj.AccountID, Obj.Name);
			this->GCServerMsgStringSend(Lang.GetText(0, 531), Obj, 1);
			return;
		}

		Obj.m_IfState->use = 1;
		Obj.m_IfState->type = 7;
		gObjInventoryTrans(Obj);
		sLog->outBasic("[ArcaBattle][BootyItemMix] [%s][%s]  Booty Item Mix Start", Obj.AccountID, Obj.Name);
		gObjItemTextSave(Obj);
		gObjWarehouseTextSave(Obj);
	}
}

void GameProtocol::CGReqSpritemapExchange(CGameObject &Obj)
{
	if (gObjIsConnected(Obj))
	{
		int bCanChaosBox = 1; // stub;
		if (bCanChaosBox == 1 && !(Obj.m_IfState->type & 3))
		{
			if (Obj.m_bPShopOpen == 1)
			{
				sLog->outBasic("[%s][%s] is Already Opening PShop, ChaosBox Failed", Obj.AccountID, Obj.Name);
				this->GCServerMsgStringSend(Lang.GetText(0, 531), Obj, 1);
			}

			else
			{
				Obj.m_IfState->use = 1;
				Obj.m_IfState->type = 7;

				gObjInventoryTrans(Obj);
				sLog->outBasic("[ArcaBattle][BootyItemMix] [%s][%s]  Booty Item Mix Start", Obj.AccountID, Obj.Name);
				gObjItemTextSave(Obj);
				gObjWarehouseTextSave(Obj);
			}
		}
	}
}

void GameProtocol::CGReqRegisteredMemberCnt(CGameObject &Obj)
{
	PMSG_REQ_AB_REG_MEMBER_CNT_DS pMsg;
	PMSG_ANS_AB_REG_MEMBER_CNT pAnsMsg;

	if (gObjIsConnected(Obj))
	{
		pAnsMsg.h.c = 0xC1;
		pAnsMsg.h.size = sizeof(pMsg);
		pAnsMsg.h.headcode = 0xF8;
		pAnsMsg.h.subcode = 0x42;
		pAnsMsg.btRegMemCnt = 0;

		if (Obj.m_PlayerData->lpGuild)
		{
			pMsg.h.c = 0xC1;
			pMsg.h.size = sizeof(pMsg);
			pMsg.h.headcode = 0xF8;
			pMsg.h.subcode = 0x45;
			pMsg.iIndex = Obj.m_Index;
			pMsg.iGuildNumber = Obj.m_PlayerData->GuildNumber;
			wsDataCli.DataSend((char*)&pMsg, sizeof(pMsg));
		}

		else
		{
			pAnsMsg.btResult = 1;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pAnsMsg, sizeof(pMsg));
		}
	}

}

void GameProtocol::CGReqEnterDoppelGanger(PMSG_REQ_ENTER_DOPPELGANGER *lpMsg, CGameObject &Obj)
{
	g_DoppelGanger.EnterDoppelgangerEvent(Obj, lpMsg->btPos);
}

void GameProtocol::CGReqEnterZone(PMSG_REQ_ENTER_ZONE * lpMsg, CGameObject &Obj)
{
	if (lpMsg->btResult == TRUE)
	{
		g_ImperialGuardian.CGEnterPortal(Obj, 0);
	}
}

void GameProtocol::GCSendRecvCheck(CGameObject &Obj)
{
	PMSG_RECV_TRAP pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xFA, 0x9F, sizeof(pMsg));

	pMsg.wUserIndex = Obj.m_Index;
	pMsg.CurrTime = time(NULL);

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::CGReqAntihackBreach(CGameObject &Obj, PMSG_ANTIHACK_BREACH *lpMsg)
{
	

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	sLog->outError( "[%s][%s][%s][%s] AntiHack breach -> Code:%d",
		Obj.AccountID, Obj.Name, Obj.m_PlayerData->ConnectUser->IP, Obj.m_PlayerData->HWID, lpMsg->dwErrorCode);

	AntiHackLog->Output("[%s][%s][%s][%s] AntiHack breach -> Code: %d",
		Obj.AccountID, Obj.Name, Obj.m_PlayerData->ConnectUser->IP, Obj.m_PlayerData->HWID, lpMsg->dwErrorCode);

	if (g_ConfigRead.antihack.AntiHackBreachDisconnectUser == true)
	{
		//this->GCSendDisableReconnect(Obj);
		IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
	}
}

void GameProtocol::CGReqAntihackCheck(CGameObject &Obj, PMSG_ANTIHACK_CHECK *lpMsg)
{
	

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	BYTE MainRecvHeader[] = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC };

	if (memcmp(lpMsg->checkdata, MainRecvHeader, 5) != 0 && g_ConfigRead.antihack.AntiHackRecvHookProtect == true) // data is wrong, recv is probably hooked
	{
		sLog->outError( "[%s][%s][%s][%s] AntiHack breach -> Recv header data is wrong",
			Obj.AccountID, Obj.Name, Obj.m_PlayerData->ConnectUser->IP, Obj.m_PlayerData->HWID);

		AntiHackLog->Output("[%s][%s][%s][%s] AntiHack breach -> Recv header data is wrong",
			Obj.AccountID, Obj.Name, Obj.m_PlayerData->ConnectUser->IP, Obj.m_PlayerData->HWID);
		this->GCSendDisableReconnect(Obj);
		//this->GCServerMsgStringSendEx(Obj, 1, Lang.GetText(0, 547));
		gObjCloseSet(Obj, 0);
	}

	else
	{
		Obj.m_PlayerData->AntiHackCheckTime = GetTickCount();
	}
}

void GameProtocol::CGReqInJewelPentagramItem(PMSG_REQ_IN_PENTAGRAM_JEWEL *lpMsg, CGameObject &Obj)
{
	if (!lpMsg)
	{
		return;
	}

	if (!g_PentagramSystem.PentagramJewel_IN(Obj, lpMsg->iPentagramPos, lpMsg->iJewelPos))
		this->GCAnsInJewelPentagramItem(Obj, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

void GameProtocol::GCAnsInJewelPentagramItem(CGameObject &Obj, int iResult, BYTE btJewelPos, BYTE btJewelIndex, BYTE btItemType, WORD wItemIndex, BYTE btMainAttribute, BYTE btLevel, BYTE btRank1OptionNum, BYTE btRank1Level, BYTE btRank2OptionNum, BYTE btRank2Level, BYTE btRank3OptionNum, BYTE btRank3Level, BYTE btRank4OptionNum, BYTE btRank4Level, BYTE btRank5OptionNum, BYTE btRank5Level)
{
	PMSG_ANS_IN_PENTAGRAM_JEWEL pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xEC, 0x00, sizeof(pMsg));

	pMsg.btResult = iResult;
	pMsg.btJewelPos = btJewelPos;
	pMsg.btJewelIndex = btJewelIndex;
	pMsg.btItemType = btItemType;
	pMsg.wItemIndex = wItemIndex;
	pMsg.btMainAttribute = btMainAttribute;
	pMsg.btLevel = btLevel;
	pMsg.btRank1OptionNum = btRank1OptionNum;
	pMsg.btRank1Level = btRank1Level;
	pMsg.btRank2OptionNum = btRank2OptionNum;
	pMsg.btRank2Level = btRank2Level;
	pMsg.btRank3OptionNum = btRank3OptionNum;
	pMsg.btRank3Level = btRank3Level;
	pMsg.btRank4OptionNum = btRank4OptionNum;
	pMsg.btRank4Level = btRank4Level;
	pMsg.btRank5OptionNum = btRank5OptionNum;
	pMsg.btRank5Level = btRank5Level;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
}

void GameProtocol::CGReqOutJewelPentagramItem(PMSG_REQ_OUT_PENTAGRAM_JEWEL *lpMsg, CGameObject &Obj)
{
	if (!lpMsg)
	{
		return;
	}

	BYTE btJewelPos;
	BYTE btJewelDBIndex;

	int iResult = g_PentagramSystem.PentagramJewel_OUT(Obj, lpMsg->iPentagramPos, lpMsg->btSocketIndex, &btJewelPos, &btJewelDBIndex);
	this->GCAnsOutJewelPentagramItem(Obj, iResult, btJewelPos, btJewelDBIndex);

	if (iResult != 1)
	{
		this->GCAnsPentagramJewelInOut(Obj, iResult);
	}
}

void GameProtocol::GCAnsOutJewelPentagramItem(CGameObject &Obj, int iResult, BYTE btJewelPos, BYTE btJewelDBIndex)
{
	PMSG_ANS_OUT_PENTAGRAM_JEWEL pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xEC, 0x01, sizeof(pMsg));

	pMsg.Result = iResult;
	pMsg.btJewelPos = btJewelPos;
	pMsg.btJewelDBIndex = btJewelDBIndex;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::CGReqRefinePentagramJewel(PMSG_REQ_REFINE_PENTAGRAM_JEWEL *lpMsg, CGameObject &Obj)
{
	if (!lpMsg)
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	Obj.m_PlayerData->m_bIsPentagramMixCompleted = true;
	BYTE btRefineResult = g_PentagramMixSystem.PentagramJewelRefine(Obj, lpMsg->btRefineKind);

	if (btRefineResult != 1)
	{
		this->GCAnsRefinePentagramJewel(Obj, btRefineResult);
	}
}

void GameProtocol::GCAnsRefinePentagramJewel(CGameObject &Obj, BYTE btResult)
{
	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));

	pMsg.Result = btResult;
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::CGReqUpgradePentagramJewel(PMSG_REQ_UPGRADE_PENTAGRAM_JEWEL *lpMsg, CGameObject &Obj)
{
	if (!lpMsg)
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	Obj.m_PlayerData->m_bIsPentagramMixCompleted = true;
	BYTE btUpgradeResult = g_PentagramMixSystem.PentagramJewel_Upgrade(Obj, lpMsg->btUpgradeType, lpMsg->btTargetValue);

	if (btUpgradeResult != 1)
	{
		this->GCAnsUpgradePentagramJewel(Obj, btUpgradeResult);
	}
}

void GameProtocol::GCAnsUpgradePentagramJewel(CGameObject &Obj, BYTE btResult)
{
	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));

	pMsg.Result = btResult;
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCAnsPentagramJewelInOut(CGameObject &Obj, BYTE btResult)
{
	PMSG_PENTAGRAM_JEWEL_INOUT pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xEC, 0x04, sizeof(pMsg));

	pMsg.btResult = btResult;
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::CGReqGuildMatchingList(PMSG_REQ_GUILDMATCHINGLIST *lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	GDReqGuildMatchingList(nUserIndex, lpMsg->nPage);
}

void GameProtocol::CGReqGuildMatchingListSearchWord(PMSG_REQ_GUILDMATCHINGLISTSEARCHWORD *lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	int nPage = lpMsg->nPage;
	char szSearchWord[11];

	std::memcpy(szSearchWord, lpMsg->szSearchWord, 11);
	GDReqGuildMatchingListSearchWord(nUserIndex, nPage, szSearchWord);
}

void GameProtocol::CGReqRegGuildMatchingList(PMSG_REQ_REGGUILDMATCHINGDATA *lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	CGameObject lpObj = *getGameObject(nUserIndex);

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	int nGuildNumber = Obj.m_PlayerData->GuildNumber;
	GUILD_INFO_STRUCT * lpstGuildInfo = Guild.SearchGuild_Number(nGuildNumber);
	_stGuildMatchingList stGuildMatchingList;

	PMSG_ANS_REGGUILDMATCHINGDATA pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xED, 0x02, sizeof(pMsg));

	if (!lpstGuildInfo || Guild.GetGuildMemberStatus(lpstGuildInfo->Name, Obj.Name) != GUILD_MASTER)
	{
		pMsg.nResult = -4;
		IOCP.DataSend(nUserIndex, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (Obj.m_PK_Level > 3)
	{
		pMsg.nResult = -2;
		IOCP.DataSend(nUserIndex, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	std::memcpy(stGuildMatchingList.szMemo, lpMsg->szMemo, 41);
	stGuildMatchingList.btInterestType = lpMsg->btInterestType;
	stGuildMatchingList.btLevelRange = lpMsg->btLevelRange;
	stGuildMatchingList.btClassType = lpMsg->btClassType;
	stGuildMatchingList.btGuildMasterClass = Obj.Class;

	if (Obj.m_PlayerData->ChangeUP == 1)
	{
		stGuildMatchingList.btGuildMasterClass |= 0x08;
	}

	if (Obj.m_PlayerData->ChangeUP == 2)
	{
		stGuildMatchingList.btGuildMasterClass |= CS_SET_CHANGEUP_SX;
		stGuildMatchingList.btGuildMasterClass |= CS_SET_CHANGEUP2_SX;
		//stAllowList.btApplicantClass |= 0x10;
	}

	std::memcpy(stGuildMatchingList.szGuildName, Obj.m_PlayerData->GuildName, MAX_GUILD_LEN + 1);
	std::memcpy(stGuildMatchingList.szRegistrant, Obj.Name, MAX_ACCOUNT_LEN + 1);
	stGuildMatchingList.btGuildMemberCnt = lpstGuildInfo->Count;
	stGuildMatchingList.nGuildNumber = lpstGuildInfo->Number;
	stGuildMatchingList.nGuildMasterLevel = Obj.Level + Obj.m_PlayerData->MasterLevel;
	stGuildMatchingList.btGensType = g_GensSystem.GetGensInfluence(lpObj);

	GDReqRegGuildMatchingList(nUserIndex, stGuildMatchingList);
}

void GameProtocol::CGReqCancelGuildMatchingList(PMSG_REQ_CANCEL_GUILDMATCHINGLIST * lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	CGameObject lpObj = *getGameObject(nUserIndex);

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	int nGuildNumber = Obj.m_PlayerData->GuildNumber;
	GUILD_INFO_STRUCT * lpstGuildInfo = Guild.SearchGuild_Number(nGuildNumber);

	PMSG_ANS_CANCEL_GUILDMATCHINGLIST pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xED, 0x03, sizeof(pMsg));

	if (!lpstGuildInfo || Guild.GetGuildMemberStatus(lpstGuildInfo->Name, Obj.Name) != GUILD_MASTER)
	{
		pMsg.nResult = -2;
		IOCP.DataSend(nUserIndex, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	GDReqDelMatchingList(nUserIndex, nGuildNumber);
}

void GameProtocol::CGReqJoinGuildMatching(PMSG_REQ_JOIN_GUILDMATCHING *lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	CGameObject lpObj = *getGameObject(nUserIndex);

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	int nGuildNumber = Obj.m_PlayerData->GuildNumber;
	int nTargetGuildNumber = lpMsg->nGuildNumber;

	PMSG_ANS_JOIN_GUILDMATCHING pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xED, 0x04, sizeof(pMsg));

	if (nGuildNumber > 0)
	{
		pMsg.nResult = -3;
		IOCP.DataSend(nUserIndex, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (getGameObject(nUserIndex)->m_PK_Level > 3)
	{
		pMsg.nResult = -2;
		IOCP.DataSend(nUserIndex, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	_stGuildMatchingAllowListDB stAllowList;

	stAllowList.nGuildNumber = nTargetGuildNumber;
	std::memcpy(stAllowList.szApplicantName, Obj.Name, MAX_ACCOUNT_LEN + 1);
	stAllowList.btApplicantClass = Obj.Class;
	stAllowList.btState = 0;
	stAllowList.nApplicantLevel = Obj.Level + Obj.m_PlayerData->MasterLevel;

	if (Obj.m_PlayerData->ChangeUP == 1)
	{
		stAllowList.btApplicantClass |= 0x08;
	}

	else if (Obj.m_PlayerData->ChangeUP == 2)
	{
		stAllowList.btApplicantClass |= CS_SET_CHANGEUP_SX;
		stAllowList.btApplicantClass |= CS_SET_CHANGEUP2_SX;
		//stAllowList.btApplicantClass |= 0x10;
	}

	GDReqJoinGuildMatchingList(nUserIndex, stAllowList);
}

void GameProtocol::CGReqCancelJoinGuildMatching(int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	CGameObject lpObj = *getGameObject(nUserIndex);

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	GDReqCancelJoinGuildMatching(nUserIndex, Obj.Name, 0);
}

void GameProtocol::CGReqAllowJoinGuildMatching(PMSG_REQ_ALLOW_JOIN_GUILDMATCHING *lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	CGameObject lpObj = *getGameObject(nUserIndex);

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	char szMemberName[MAX_ACCOUNT_LEN + 1];
	std::memcpy(szMemberName, lpMsg->szName, MAX_ACCOUNT_LEN + 1);

	if (gObjIsConnected(lpObj))
	{
		GDReqAllowJoinGuildMatching(nUserIndex, lpMsg->nAllowType, szMemberName, Obj.m_PlayerData->GuildName);
	}
}

void GameProtocol::CGReqGetAllowListJoinGuildMatching(PMSG_REQ_ALLOWLIST_GUILDMATCHING *lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	CGameObject lpObj = *getGameObject(nUserIndex);

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	int nGuildNumber = Obj.m_PlayerData->GuildNumber;
	GUILD_INFO_STRUCT * lpstGuildInfo = Guild.SearchGuild_Number(nGuildNumber);

	PMSG_ANS_WAIT_GUILDMATCHING pMsg;
	PHeadSubSetW((BYTE*)&pMsg, 0xED, 0x07, sizeof(pMsg));

	if (!lpstGuildInfo || Guild.GetGuildMemberStatus(lpstGuildInfo->Name, Obj.Name) != GUILD_MASTER)
	{
		pMsg.nResult = -2;
		IOCP.DataSend(nUserIndex, (BYTE*)&pMsg, sizeof(pMsg));
		return;
	}

	GDReqWaitGuildMatchingList(nGuildNumber, nUserIndex);
}

void GameProtocol::CGReqGetWaitStateListGuildMatching(int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	CGameObject lpObj = *getGameObject(nUserIndex);

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	char szName[MAX_ACCOUNT_LEN + 1];
	std::memcpy(szName, Obj.Name, MAX_ACCOUNT_LEN + 1);

	GDReqGetWaitStateListGuildMatching(nUserIndex, szName);
}

void GameProtocol::CGReqRegWantedPartyMember(PMSG_REQ_REG_WANTED_PARTYMEMBER *lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	CGameObject lpObj = *getGameObject(nUserIndex);

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	BOOL bAlreadyParty = FALSE;
	int nPartyNumber = -1;

	if (Obj.PartyNumber != -1)
	{
		bAlreadyParty = TRUE;
		nPartyNumber = Obj.PartyNumber;
	}

	PARTY_INFO_LISTDB stList;

	if (bAlreadyParty)
	{
		stList.btCurPartyMemberCnt = gParty.GetCount(nPartyNumber);
	}

	else
	{
		stList.btCurPartyMemberCnt = 1;
	}

	stList.btUsePassword = lpMsg->btUsePassword;
	stList.btWantedClass = lpMsg->btWantedClass;
	stList.nHuntingGround = lpMsg->nHuntingGround;
	stList.nMaxLevel = lpMsg->nMaxLevel;
	stList.nMinLevel = lpMsg->nMinLevel;
	std::memcpy(stList.szLeaderName, Obj.Name, MAX_ACCOUNT_LEN + 1);
	std::memcpy(stList.szPassword, lpMsg->szPassword, 5);
	std::memcpy(stList.szTitle, lpMsg->szTitle, 41);
	std::memcpy(stList.btWantedClassDetailInfo, lpMsg->btWantedClassDetailInfo, 7);
	stList.btGensType = g_GensSystem.GetGensInfluence(*getGameObject(nUserIndex));
	stList.nLeaderLevel = getGameObject(nUserIndex)->Level + getGameObject(nUserIndex)->m_PlayerData->MasterLevel;
	stList.btLeaderClass = getGameObject(nUserIndex)->Class;
	stList.btApprovalType = lpMsg->btApprovalType;

	if (getGameObject(nUserIndex)->m_PlayerData->ChangeUP == 1)
	{
		stList.btLeaderClass |= 0x08;
	}

	else if (getGameObject(nUserIndex)->m_PlayerData->ChangeUP == 2)
	{
		stList.btLeaderClass |= CS_SET_CHANGEUP_SX;
		stList.btLeaderClass |= CS_SET_CHANGEUP2_SX;
		//		stList.btLeaderClass |= 0x10;
	}

	if (getGameObject(nUserIndex)->m_IfState->use != 0 && getGameObject(nUserIndex)->m_IfState->type == 2)
	{
		PMSG_ANS_REG_WANTED_PARTYMEMBER pMsg;
		PHeadSubSetB((BYTE*)&pMsg, 0xEF, 0x00, sizeof(pMsg));
		pMsg.nResult = -2;

		IOCP.DataSend(nUserIndex, (BYTE*)&pMsg, sizeof(pMsg));
	}

	else
	{
		GDReqRegWantedPartyMember(stList, nUserIndex);
	}
}

void GameProtocol::CGReqGetPartyMatchingList(PMSG_REQ_GET_PARTYMATCHINGLIST * lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	CGameObject lpObj = *getGameObject(nUserIndex);

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	char szSearchWord[11];
	std::memset(szSearchWord, 0x00, sizeof(szSearchWord));

	if (lpMsg->btUseSearchWord == 0)
	{
		GDReqGetPartyMatchingList(lpMsg->nPage, szSearchWord, 0, nUserIndex);
	}

	else if (lpMsg->btUseSearchWord == 1)
	{
		std::memcpy(szSearchWord, lpMsg->szSearchWord, 10);
		GDReqGetPartyMatchingList(lpMsg->nPage, szSearchWord, 1, nUserIndex);
	}

	else if (lpMsg->btUseSearchWord == 2)
	{
		GDReqGetPartyMatchingList(lpMsg->nPage, szSearchWord, 2, nUserIndex);
	}
}

void GameProtocol::CGReqJoinMemberPartyMatching(PMSG_REQ_MEMBERJOIN_PARTYMATCHINGLIST *lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	CGameObject lpObj = *getGameObject(nUserIndex);

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	char szMemberName[MAX_ACCOUNT_LEN + 1];
	char szLeaderName[MAX_ACCOUNT_LEN + 1];
	char szPassword[5];

	std::memset(szMemberName, 0x00, MAX_ACCOUNT_LEN + 1);
	std::memset(szLeaderName, 0x00, MAX_ACCOUNT_LEN + 1);
	std::memset(szPassword, 0x00, 5);

	std::memcpy(szMemberName, Obj.Name, MAX_ACCOUNT_LEN);
	std::memcpy(szLeaderName, lpMsg->szLeaderName, MAX_ACCOUNT_LEN);
	std::memcpy(szPassword, lpMsg->szPassword, 4);

	int nTotalLevel = Obj.Level + Obj.m_PlayerData->MasterLevel;
	BYTE btChangeUpClass = Obj.Class;

	if (Obj.m_PlayerData->ChangeUP == 1)
	{
		btChangeUpClass |= 0x08;
	}

	if (Obj.m_PlayerData->ChangeUP == 2)
	{
		btChangeUpClass |= CS_SET_CHANGEUP_SX;
		btChangeUpClass |= CS_SET_CHANGEUP2_SX;
		//btChangeUpClass |= 0x08;
		//btChangeUpClass |= 0x18;
	}

	BYTE btClass = Obj.Class;

	switch (Obj.Class)
	{
	case CLASS_WIZARD:
		btClass = 4;
		break;
	case CLASS_KNIGHT:
		btClass = 1;
		break;
	case CLASS_ELF:
		btClass = 2;
		break;
	case CLASS_MAGUMSA:
		btClass = 8;
		break;
	case CLASS_DARKLORD:
		btClass = 16;
		break;
	case CLASS_SUMMONER:
		btClass = 32;
		break;
	case CLASS_RAGEFIGHTER:
		btClass = 64;
		break;
	case CLASS_GROWLANCER:
		btClass = 128;
		break;
	default:
		btClass = 0;
		break;
	}

	if (getGameObject(nUserIndex)->m_IfState->use != 0 && getGameObject(nUserIndex)->m_IfState->type == 2)
	{
		PMSG_ANS_MEMBERJOIN_PARTYMATCHINGLIST pMsg;
		PHeadSubSetB((BYTE*)&pMsg, 0xEF, 0x02, sizeof(pMsg));
		pMsg.nResult = -4;

		IOCP.DataSend(nUserIndex, (BYTE*)&pMsg, sizeof(pMsg));
	}

	else
	{
		BYTE btGensType = g_GensSystem.GetGensInfluence(*getGameObject(nUserIndex));
		GDReqJoinMemberPartyMatching(szMemberName, szLeaderName,
			szPassword, nUserIndex, Obj.DBNumber,
			nTotalLevel, btClass, lpMsg->btUseRandomParty,
			btGensType, btChangeUpClass);
	}
}

void GameProtocol::CGReqMemberJoinStateList(PMSG_REQ_MEMBERJOIN_STATELIST_PARTYMATCHING *lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	CGameObject lpObj = *getGameObject(nUserIndex);

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	GDReqMemberJoinStateList(Obj.Name, nUserIndex);
}

void GameProtocol::CGReqMemberJoinStateListLeader(PMSG_REQ_WAITLIST_PARTYMATCHING *lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	CGameObject lpObj = *getGameObject(nUserIndex);

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	GDReqMemberJoinStateListLeader(Obj.Name, nUserIndex);
}

void GameProtocol::CGReqAcceptJoinMemberPartyMatching(PMSG_REQ_ACCEPTMEMBER_PARTYMATCHING *lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	CGameObject lpObj = *getGameObject(nUserIndex);

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	char szMemberName[MAX_ACCOUNT_LEN + 1];
	char szLeaderName[MAX_ACCOUNT_LEN + 1];

	std::memset(szMemberName, 0x00, MAX_ACCOUNT_LEN + 1);
	std::memset(szLeaderName, 0x00, MAX_ACCOUNT_LEN + 1);

	std::memcpy(szMemberName, lpMsg->szMemberName, MAX_ACCOUNT_LEN);
	std::memcpy(szLeaderName, Obj.Name, MAX_ACCOUNT_LEN);

	GDReqAcceptMemberJoin(nUserIndex, lpMsg->btType, szLeaderName, szMemberName, 0);
}

void GameProtocol::CGReqCancelPartyMatching(PMSG_REQ_CANCEL_JOIN_PARTYMATCHING *lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	CGameObject lpObj = *getGameObject(nUserIndex);

	if (gObjIsConnected(nUserIndex) == false)
	{
		return;
	}

	GDReqCancelPartyMatching(nUserIndex, lpMsg->btType);
}

#pragma pack (1)
struct stBuffList
{
	stBuffList()
	{
		this->btBuffIndex = 0;
		this->nBuffDuration = 0;
	}

	BYTE btBuffIndex;
	int nBuffDuration;
};

struct PMSG_BUFFLIST_SEND_PARTY
{
	PBMSG_HEAD2 h;
	char Name[MAX_ACCOUNT_LEN + 1];
	BYTE btBuffCount;
	stBuffList stBuffList[32];
};
#pragma pack ()

void GameProtocol::GCDisplayBuffeffectPartyMember(CGameObject &Obj)
{
	if (gObjIsConnected(Obj) == false)
	{
		return;
	}

	if (Obj.PartyNumber < 0)
	{
		return;
	}

	PMSG_BUFFLIST_SEND_PARTY pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x2E, sizeof(pMsg));

	pMsg.btBuffCount = Obj.m_BuffEffectCount;

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		std::memcpy(pMsg.Name, Obj.m_PlayerData->m_RealNameOfUBF, MAX_ACCOUNT_LEN + 1);
	}

	else
	{
		std::memcpy(pMsg.Name, Obj.Name, MAX_ACCOUNT_LEN + 1);
	}

	int nPartyIndex = Obj.PartyNumber;

	if (pMsg.btBuffCount > MAX_BUFFEFFECT)
	{
		pMsg.btBuffCount = MAX_BUFFEFFECT;
	}

	for (int nBuffCnt = 0; nBuffCnt < pMsg.btBuffCount; nBuffCnt++)
	{
		pMsg.stBuffList[nBuffCnt].btBuffIndex = Obj.pntBuffEffectList[nBuffCnt]->BuffIndex;
		pMsg.stBuffList[nBuffCnt].nBuffDuration = Obj.pntBuffEffectList[nBuffCnt]->EffectDuration;
	}

	for (int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		if (gObjIsConnected(gParty.m_PartyS[nPartyIndex].Number[i]))
		{
			IOCP.DataSend(gParty.m_PartyS[nPartyIndex].Number[i], (BYTE*)&pMsg, pMsg.h.size);
		}
	}
}

void GameProtocol::GCPartyMemberPosition(int nIndex)
{
	if (ObjectMaxRange(nIndex) == false)
	{
		return;
	}

	if (!gObjIsConnected(nIndex))
	{
		return;
	}

	if (getGameObject(nIndex)->m_PlayerData->m_bSendMemberPos == FALSE)
	{
		return;
	}

	if (getGameObject(nIndex)->PartyNumber < 0)
	{
		return;
	}

	PMSG_MEMBER_POS_INFO_SEND pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xE7, 0x01, sizeof(pMsg));

	int nPartyNumber = getGameObject(nIndex)->PartyNumber;

	if (ObjectMaxRange(nPartyNumber))
	{
		pMsg.btCount = 0;

		for (int i = 0; i < 5; i++)
		{
			int nTargetIndex = gParty.m_PartyS[nPartyNumber].Number[i];

			if (ObjectMaxRange(nTargetIndex) == false)
			{
				continue;
			}

			if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
			{
				std::memcpy(pMsg.stPosInfo[pMsg.btCount].szName, getGameObject(nTargetIndex)->m_PlayerData->m_RealNameOfUBF, MAX_ACCOUNT_LEN + 1);
			}

			else
			{
				std::memcpy(pMsg.stPosInfo[pMsg.btCount].szName, getGameObject(nTargetIndex)->Name, MAX_ACCOUNT_LEN + 1);
			}

			pMsg.stPosInfo[pMsg.btCount].btMapNumber = getGameObject(nTargetIndex)->MapNumber;
			pMsg.stPosInfo[pMsg.btCount].btPosX = getGameObject(nTargetIndex)->X;
			pMsg.stPosInfo[pMsg.btCount].btPosY = getGameObject(nTargetIndex)->Y;

			pMsg.btCount++;
		}

		IOCP.DataSend(nIndex, (BYTE*)&pMsg, pMsg.h.size);
	}
}

void GameProtocol::CGReqSendMemberPosInfoStart(int nIndex)
{
	if (ObjectMaxRange(nIndex) == false)
	{
		return;
	}

	if (!gObjIsConnected(nIndex))
	{
		return;
	}

	if (getGameObject(nIndex)->Type != OBJ_USER)
	{
		return;
	}

	if (getGameObject(nIndex)->PartyNumber < 0)
	{
		return;
	}

	getGameObject(nIndex)->m_PlayerData->m_bSendMemberPos = TRUE;
}

void GameProtocol::CGReqSendMemberPosInfoStop(int nIndex)
{
	if (ObjectMaxRange(nIndex) == false)
	{
		return;
	}

	if (!gObjIsConnected(nIndex))
	{
		return;
	}

	if (getGameObject(nIndex)->Type != OBJ_USER)
	{
		return;
	}

	if (getGameObject(nIndex)->PartyNumber < 0)
	{
		return;
	}

	getGameObject(nIndex)->m_PlayerData->m_bSendMemberPos = FALSE;
}

void GameProtocol::GCReqSendNpcPosInfo(PMSG_REQ_NPC_POSITION *lpMsg, int nIndex)
{
	if (ObjectMaxRange(nIndex) == false)
	{
		return;
	}

	if (!gObjIsConnected(nIndex))
	{
		return;
	}

	if (getGameObject(nIndex)->Type != OBJ_USER)
	{
		return;
	}

	if (getGameObject(nIndex)->PartyNumber < 0)
	{
		return;
	}

	if (lpMsg->btType)
	{
		getGameObject(nIndex)->m_PlayerData->m_bSendNpcPos = FALSE;
	}

	else
	{
		getGameObject(nIndex)->m_PlayerData->m_bSendNpcPos = TRUE;
	}
}

void GameProtocol::CGReqCCF_DayTime(CGameObject &Obj)
{
	if (gObjIsConnected(Obj) == false)
	{
		return;
	}

	if (g_ConfigRead.server.GetServerType() != SERVER_BATTLECORE)
	{
		return;
	}

	Obj.m_bCCF_UI_Using = true;
	g_ChaosCastleFinal.Send_CCF_Schedule(Obj);
}

void GameProtocol::CGReqCCF_EnterCheck(CGameObject &Obj)
{
	if (ObjectMaxRange(Obj) == false)
	{
		return;
	}

	if (Obj.Type != OBJ_USER || Obj.Connected <= PLAYER_LOGGED || g_ConfigRead.server.GetServerType() != SERVER_BATTLECORE)
	{
		return;
	}

	PMSG_RESULT_CCF_ENTERCHECK pResult;
	PHeadSubSetB((BYTE*)&pResult, 0xAF, 0x05, sizeof(pResult));

	if (Obj.MapNumber != MAP_INDEX_LORENMARKET)
	{
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 593));
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pNotice, pNotice.h.size);

		pResult.byResult = 11;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (!Obj.m_bCCF_UI_Using)
	{
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 593));
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pNotice, pNotice.h.size);

		pResult.byResult = 6;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (Obj.m_IfState->use && Obj.m_IfState->type != 12)
	{
		pResult.byResult = 12;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, &pResult.h.c, pResult.h.size);
		return;
	}

	if (Obj.m_byCCF_CanEnter)
	{
		sLog->outBasic("[Chaos Castle Survival] [ CGReqCCF_EnterCheck ] ACC:%s, NAME:%s m_byCCF_CanEnter :%d ", Obj.AccountID, Obj.Name, Obj.m_byCCF_CanEnter);
		Obj.m_byCCF_CanEnter = 0;

		pResult.byResult = 13;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (!g_ConfigRead.pk.bPkPenaltyDisable)
	{
		BOOL bPlayerKiller = FALSE; //Season 2.5 add-on

		if (Obj.PartyNumber >= 0) //Season 2.5 add-on
		{
			if (gParty.GetPKPartyPenalty(Obj.PartyNumber) >= 6)
			{
				bPlayerKiller = TRUE;
			}
		}
		else if (Obj.m_PK_Level >= 6)
		{
			bPlayerKiller = TRUE;
		}

		if (bPlayerKiller == TRUE)
		{
			PMSG_NOTICE pNotice;
			TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 593));
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pNotice, pNotice.h.size);

			pResult.byResult = 14;
			pResult.byRemainTime = 0;
			pResult.byRemainTime_LOW = 0;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}
	}

	if (gObjDuelCheck(Obj) == TRUE)
	{
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 594));
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pNotice, pNotice.h.size);

		pResult.byResult = 15;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (Obj.m_PlayerData->lpGuild && Obj.m_PlayerData->lpGuild->WarState == 1)
	{
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 595));
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pNotice, pNotice.h.size);

		pResult.byResult = 16;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 3) ||
		Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 2) ||
		Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 37))
	{
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx(&pNotice, 1, Lang.GetText(0, 596));
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pNotice, pNotice.h.size);

		pResult.byResult = 17;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	BOOL bPlayerKiller = FALSE; //Season 2.5 add-on

	if (Obj.PartyNumber >= 0) //Season 2.5 add-on
	{
		if (gParty.GetPKPartyPenalty(Obj.PartyNumber) >= 6)
		{
			bPlayerKiller = TRUE;
		}
	}
	else if (Obj.m_PK_Level >= 6)
	{
		bPlayerKiller = TRUE;
	}

	if (bPlayerKiller == TRUE)
	{
		pResult.byResult = 8;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, &pResult.h.c, pResult.h.size);
		return;
	}

	if (g_ChaosCastleFinal.m_nCCF_Level > Obj.Level)
	{
		pResult.byResult = 1;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, &pResult.h.c, pResult.h.size);
		return;
	}

	if (g_ChaosCastleFinal.CheckWearingMOPH(Obj) == TRUE)
	{
		GCServerMsgStringSend(Lang.GetText(0, 576), iIndex, 1);

		pResult.byResult = 18;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, &pResult.h.c, pResult.h.size);
		return;
	}
	if ((MapC[Obj.MapNumber].GetAttr(Obj.X, Obj.Y) & 1) != 1)
	{
		pResult.byResult = 19;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, &pResult.h.c, pResult.h.size);
		return;
	}

	int nBattleType = g_ChaosCastleFinal.Check_CCF_DayType();

	if (!nBattleType)
	{
		sLog->outBasic("[Chaos Castle Survival][CGReqCCF_EnterCheck] No OpenDay , Index:%d,Name:%s", iIndex, Obj.Name);

		pResult.byResult = 10;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, &pResult.h.c, pResult.h.size);
		return;
	}
	if (nBattleType == 3 && g_ChaosCastleFinal.CheckOpenOrNotOpenCCF_Final(3) == TRUE)
	{
		sLog->outBasic("[Chaos Castle Survival][CGReqCCF_EnterCheck] Not Final Time, Index:%d,Name:%s", iIndex, Obj.Name);

		pResult.byResult = 2;
		int nRemainTime = g_ChaosCastleFinal.GetFinalRemainTime(Obj);
		pResult.byRemainTime = HIBYTE(nRemainTime);
		pResult.byRemainTime_LOW = LOBYTE(nRemainTime);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, &pResult.h.c, pResult.h.size);
		return;
	}
	if (g_ChaosCastleFinal.CheckCanEnter() == FALSE)
	{
		pResult.byResult = 2;
		int nRemainTime = g_ChaosCastleFinal.GetRemainTime(Obj);
		pResult.byRemainTime = HIBYTE(nRemainTime);
		pResult.byRemainTime_LOW = LOBYTE(nRemainTime);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, &pResult.h.c, pResult.h.size);
		return;
	}

	int nCheckIndex = g_ChaosCastleFinal.EnterCheck_CCF_User(Obj);

	if (nCheckIndex == -1)
	{
		pResult.byResult = 5;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, &pResult.h.c, pResult.h.size);
		return;
	}

	if (nCheckIndex == -2)
	{
		pResult.byResult = 9;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, &pResult.h.c, pResult.h.size);
		return;
	}

	if (!nBattleType)
	{
		pResult.byResult = 10;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, &pResult.h.c, pResult.h.size);
		return;
	}

	if (nBattleType == 1)
	{
		if (!g_ChaosCastleFinal.CheckUserEnterMoney(iIndex, 1))
		{
			pResult.byResult = 7;
			pResult.byRemainTime = 0;
			pResult.byRemainTime_LOW = 0;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, &pResult.h.c, pResult.h.size);
			return;
		}

		Obj.m_byCCF_CanEnter = 1;

		int nReturn = this->CGReqEnterChaosCastleFinal(Obj);

		if (nReturn)
		{
			sLog->outBasic("[Chaos Castle Survival][gObj_EnterChaosCastleFinal] ACC :%s , NAME :%s return :%d ", Obj.AccountID, Obj.Name, nReturn);
		}

		return;
	}

	if (nBattleType == 2)
	{
		if (g_ChaosCastleFinal.CheckUserEnterMoney(iIndex, 2))
		{
			Obj.m_byCCF_CanEnter = -1;
			g_ChaosCastleFinal.GD_Req_Get_Permission(Obj);
			Obj.m_nCCF_CertiTick = GetTickCount();
		}
		else
		{
			pResult.byResult = 7;
			pResult.byRemainTime = 0;
			pResult.byRemainTime_LOW = 0;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, &pResult.h.c, pResult.h.size);
		}
	}
	else
	{
		if (g_ChaosCastleFinal.CheckUserEnterMoney(iIndex, 3))
		{
			Obj.m_byCCF_CanEnter = -1;
			g_ChaosCastleFinal.GD_Req_Get_Permission(Obj);
			Obj.m_nCCF_CertiTick = GetTickCount();
		}
		else
		{
			pResult.byResult = 7;
			pResult.byRemainTime = 0;
			pResult.byRemainTime_LOW = 0;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, &pResult.h.c, pResult.h.size);
		}
	}
}

int GameProtocol::CGReqEnterChaosCastleFinal(CGameObject &Obj)
{
	if (Obj.Type == OBJ_USER && Obj.Connected <= PLAYER_LOGGED)
	{
		return -5;
	}

	PMSG_RESULT_CCF_ENTERCHECK pResult;
	PHeadSubSetB((BYTE*)&pResult, 0xAF, 0x05, sizeof(pResult));

	if (g_ChaosCastleFinal.PayUserEnterMoney(iIndex, Obj.m_byCCF_CanEnter) == FALSE)
	{
		pResult.byResult = 7;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return -3;
	}

	int nCCFUserIndex = 0;
	int nEnterCheck = g_ChaosCastleFinal.EnterCCF(iIndex, nCCFUserIndex);

	if (nEnterCheck)
	{
		sLog->outBasic("[Chaos Castle Survival] (ACC:%s,NAME:%s) EnterCCF return ERROR :%d ", Obj.AccountID, Obj.Name, nEnterCheck);

		pResult.byResult = 20;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return nEnterCheck;
	}

	else
	{
		pResult.byResult = 0;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		Obj.m_nCCF_UserIndex = nCCFUserIndex;
		Obj.m_byCCF_CanEnter = 0;
		Obj.m_IfState->state = 0;
		Obj.m_IfState->type = 0;
		Obj.m_IfState->use = 0;
		Obj.m_byKillMobCount = 0;
		Obj.m_byKillUserCount = 0;
		Obj.m_bCCF_Quit_Msg = 0;

		if (Obj.m_bPShopOpen == true)
			g_PersonalStore.CGPShopReqClose(Obj);

		if (Obj.PartyNumber >= 0)
		{
			PMSG_PARTYDELUSER pMsg;

			int pindex = gParty.GetIndex(Obj.PartyNumber, iIndex, Obj.DBNumber);

			if (pindex >= 0)
			{
				pMsg.Number = pindex;
				CGPartyDelUser(&pMsg, iIndex, 0);
			}
		}

		if (Obj.Class == CLASS_ELF)
		{
			if (Obj.m_RecallMon >= 0)
			{
				GCRecallMonLife(iIndex, 60, 0);
				gObjMonsterCallKill(Obj);
			}
		}

		GCServerMsgStringSend(Lang.GetText(0, 577), iIndex, 1);
		Obj.MapNumber = MAP_INDEX_CHAOSCASTLE_SURVIVAL;
		gObjMoveGate(iIndex, 443);

		sLog->outBasic("[Chaos Castle Survival] (ACC:%s,NAME:%s) Enter CCF_Tryout", Obj.AccountID, Obj.Name);
		return 0;
	}
}

void GameProtocol::CGRequestRepositionUserInCCF(PMSG_REQ_REPOSUSER_IN_CCF *lpMsg, CGameObject &Obj)
{
	if (Obj.MapNumber != MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		return;
	}

	if (!gObjIsConnected(Obj))
	{
		return;
	}

	if (g_ChaosCastleFinal.ObjSetPosition(Obj, Obj.X, Obj.Y) == TRUE)
	{
		Obj.m_nCCF_BlowTime = GetTickCount();
	}
}

void GameProtocol::CG_CCF_UI_OnOff(PMSG_CCF_UI_ONOFF *lpMsg, int nUserIndex)
{
	if (ObjectMaxRange(nUserIndex) == false)
	{
		return;
	}

	if (getGameObject(nUserIndex)->Type != OBJ_USER || getGameObject(nUserIndex)->Connected <= PLAYER_LOGGED)
	{
		return;
	}

	if (getGameObject(nUserIndex)->m_byCCF_CanEnter == 0xFF)
		getGameObject(nUserIndex)->m_byCCF_CanEnter = 0;

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
		getGameObject(nUserIndex)->m_bCCF_UI_Using = 0;
}

void GameProtocol::CGReq_CCF_Ranking(PMSG_REQ_CCF_RANKING *lpMsg, CGameObject &Obj)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_BATTLECORE)
	{
		return;
	}

	

	if (Obj.m_bCCF_UI_Using == false)
	{
		return;
	}

	if (Obj.MapNumber != MAP_INDEX_LORENMARKET)
	{
		return;
	}

	if (g_ChaosCastleFinal.Check_CCF_DayType() == FALSE)
	{
		_stPMSG_CCF_RANK_INFO pMsg;

		PHeadSubSetW((BYTE*)&pMsg, 0xAF, 0x07, sizeof(pMsg));
		pMsg.btResult = 2;
		pMsg.btCnt = 0;
		pMsg.h.sizeH = HIBYTE(sizeof(pMsg));
		pMsg.h.sizeL = LOBYTE(sizeof(pMsg));
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
	}

	if (g_ChaosCastleFinal.m_FirstRankingInfoLoad)
	{
		g_ChaosCastleFinal.Send_RankingInfo(lpMsg->byRankingType, Obj);
	}

	else
	{
		g_ChaosCastleFinal.GD_Load_CCF_RankingList(g_ChaosCastleFinal.Check_CCF_DayType());

		_stPMSG_CCF_RANK_INFO pMsg;

		PHeadSubSetW((BYTE*)&pMsg, 0xAF, 0x07, sizeof(pMsg));
		pMsg.btResult = 3;
		pMsg.btCnt = 0;
		pMsg.h.sizeH = HIBYTE(sizeof(pMsg));
		pMsg.h.sizeL = LOBYTE(sizeof(pMsg));
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));

		sLog->outBasic("[Chaos Castle Survival][CGReq_CCF_Ranking] GD_Load_CCF_RankingList Send ");
	}
}
void GameProtocol::GCMuunItemDurSend(CGameObject &Obj, BYTE pos, BYTE dur)
{
	PMSG_MUUNITEM_DUR pMsg;

	PHeadSubSetB(&pMsg.h.c, 0x4E, 0x03, sizeof(pMsg));
	pMsg.IPos = pos;
	pMsg.Dur = dur;
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCMuunEquipmentChange(CGameObject &Obj, int pos)
{
	CGameObject lpObj;
	PMSG_USER_MUUN_EQUIPMENT_CHANGED pChange;

	lpObj = &Obj;
	PHeadSubSetB((BYTE*)&pChange, 0x4E, 0x06, sizeof(pChange));

	pChange.NumberH = SET_NUMBERH(Obj.m_Index);
	pChange.NumberL = SET_NUMBERL(Obj.m_Index);
	ItemByteConvert(pChange.ItemInfo, Obj.pntMuunInventory[pos]);
	pChange.ItemInfo[1] = 16 * pos;
	pChange.ItemInfo[1] = Obj.pntMuunInventory[pos]->m_Level & 0xF | 16 * pos;
	this->MsgSendV2(lpObj, (BYTE*)&pChange, (unsigned __int8)pChange.h.size);
}

void GameProtocol::GCMuunInventoryItemDeleteSend(CGameObject &Obj, BYTE pos, BYTE flag)
{
	PMSG_PETINVENTORY_DELETE pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0x4E, 0x04, sizeof(pMsg));
	pMsg.IPos = pos;
	pMsg.Flag = flag;
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCMuunInventoryItemOneSend(CGameObject &Obj, int pos)
{
	PMSG_MUUN_INVEN_ITEM_MOD pMsg;

	if (Obj.pntMuunInventory[pos]->IsItem())
	{
		PHeadSubSetB((BYTE*)&pMsg, 0x4E, 0x05, sizeof(pMsg));
		pMsg.Pos = pos;
		ItemByteConvert(pMsg.ItemInfo, Obj.pntMuunInventory[pos]);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}
}

void GameProtocol::CGMuunItemGetRequest(PMSG_MUUNITEM_GETREQUEST *lpMsg, CGameObject &Obj)
{
	int item_num, map_num;
	PMSG_MUUNITEM_GETRESULT pResult;
	int type;
	int level;
	int special;
	int NOption;
	char szItemName[50];
	CMapItem * lpItem;

	pResult.h.c = 0xC3;
	pResult.h.headcode = 0x4E;
	pResult.h.subcode = 0x00;
	pResult.h.size = sizeof(pResult);
	pResult.result = -1;

	if (!gObjIsConnected(Obj))
	{
		//this->GCSendDisableReconnect(Obj);
		IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.CloseType != -1)
		return;

	if (Obj.m_PlayerData->m_bSecurityCheck == false)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	if (Obj.DieRegen != 0)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, &pResult.h.c, (unsigned __int8)pResult.h.size);

		return;
	}

	if (g_NewPVP.IsObserver(Obj))
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if (Obj.m_IfState->use == 1)
	{
		GCServerMsgStringSend(Lang.GetText(0, 597), Obj, 1);

		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, (pResult.h.size - 12));

		return;
	}

	if (!::gObjFixMuunInventoryPointer(Obj))

		if (Obj.pTransaction == TRUE)
		{
			return;
		}

	item_num = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	if (MaxItemTypeRange(item_num) == FALSE)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	map_num = Obj.MapNumber;

	if (MAX_MAP_RANGE(map_num) == FALSE)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	lpItem = &MapC[map_num].m_CItemObject[item_num];

	if (lpItem->IsItem() == TRUE && lpItem->Give == false && lpItem->live == true)
	{
		type = lpItem->m_Type;
		level = lpItem->m_Level;
		special = lpItem->m_Special[0];
		NOption = lpItem->m_NewOption;
		LPITEM_ATTRIBUTE pItemAttribute = GetItemAttr(type);

		if (!pItemAttribute)
		{
			return;
		}

		if (pItemAttribute->ItemKindA != 12)
		{
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

			return;
		}

		pResult.result = gObjChkMuunInventoryEmpty(Obj);

		if (pResult.result == 255)
		{
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

			return;
		}

		if (g_CMuunSystem.IsMuunExpireDate(lpItem->m_Type) == TRUE)
		{
			lpItem->SetMuunItemPeriodExpire();
		}

		ItemByteConvert((BYTE*)&pResult.Data[0], lpItem->m_Type, lpItem->m_Option1, lpItem->m_Option2,
			lpItem->m_Option3, lpItem->m_Level, lpItem->m_Durability, lpItem->m_NewOption,
			lpItem->m_SetOption, lpItem->m_JewelOfHarmonyOption, lpItem->m_ItemOptionEx, lpItem->m_SocketOption, lpItem->m_BonusSocketOption, 0);

		if (MapC[map_num].ItemGive(Obj, item_num, 0) == 1)
		{
			BYTE Ret = ::gObjMuunInventoryInsertItem(Obj, lpItem);

			if (Ret == 0xFF)
				pResult.result = -1;

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

			if (Ret == 0xFF)
			{
				GCMuunInventoryItemListSend(Obj);
			}

			else
			{
				BYTE NewOption[MAX_EXOPTION_SIZE];

				::ItemIsBufExOption(NewOption, (lpItem != NULL) ? (CItemObject*)&lpItem->m_Number : NULL);

				if (g_CMuunSystem.IsStoneofEvolution(lpItem))
				{
					WORD wPetItemNumber = (lpItem->m_SocketOption[0] << 8) | lpItem->m_SocketOption[1];
					LPITEM_ATTRIBUTE PetItemAttr = GetItemAttr(wPetItemNumber);

					if (!PetItemAttr)
					{
						this->GCSendGetItemInfoForParty(Obj, lpItem);

						return;
					}

				}

				else
				{

				}

				this->GCSendGetItemInfoForParty(Obj, lpItem);
			}
		}

		else
		{
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}
	}

	else
	{

		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}
}

void GameProtocol::CGMuunItemSellRequestRecv(PMSG_MUUN_ITEM_SELLREQUEST *lpMsg, CGameObject &Obj)
{
	PMSG_MUUN_ITEM_SELLRESULT pResult;
	

	PHeadSubSetB((BYTE*)&pResult, 0x4E, 0x09, sizeof(pResult));

	pResult.Result = 0;
	pResult.Money = 0;

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.CloseType != -1)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (Obj.m_IfState->use > 0 && Obj.m_IfState->type != 3)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (!PacketCheckTime(Obj))
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (!::gObjFixMuunInventoryPointer(Obj))
	{

	}

	if (Obj.pTransaction == 1)
	{
		return;
	}

	if (Obj.TargetNpcNumber == -1)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	CGameObject lpShopObj = *getGameObject(Obj.TargetNpcNumber);

	SHOP_DATA * lpShopData = g_ShopMng.GetShop(lpObj, lpShopObj);

	if (!lpShopData)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (!gObjCanItemTouch(lpObj, 3))
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (lpMsg->Pos > MUUN_INVENTORY_SIZE - 1)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (gObjCheckSerial0ItemList(Obj.pntMuunInventory[lpMsg->Pos]) != FALSE)
	{
		MsgOutput(Obj, Lang.GetText(0, 259));
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		return;
	}

	if (!IsSellToNPCItemObject(Obj.pntMuunInventory[lpMsg->Pos]->m_Type))
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (Obj.pntMuunInventory[lpMsg->Pos]->IsItem() == FALSE)
	{
		pResult.Result = false;
		pResult.Money = Obj.m_PlayerData->Money;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

	}
	else
	{
		int sellmoney = 0;

		if (g_CMuunSystem.IsMuunItem(Obj.pntMuunInventory[lpMsg->Pos]->m_Type) == TRUE)
		{
			sellmoney = 500000;
		}

		else if (g_CMuunSystem.IsStoneofEvolution(Obj.pntMuunInventory[lpMsg->Pos]->m_Type) == TRUE)
		{
			sellmoney = 200000;
		}

		else if (Obj.pntMuunInventory[lpMsg->Pos]->m_Type == ITEMGET(13, 239))
		{
			sellmoney = 300000;
		}

		else if (Obj.pntMuunInventory[lpMsg->Pos]->m_Type == ITEMGET(13, 240))
		{
			sellmoney = 1000000;
		}

		else
		{
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}

		if (gObjCheckMaxZen(Obj, sellmoney) == FALSE)
		{
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}

		Obj.m_PlayerData->Money += sellmoney;
		pResult.Result = 1;
		pResult.Money = Obj.m_PlayerData->Money;

		BYTE NewOption[MAX_EXOPTION_SIZE];
		ItemIsBufExOption(NewOption, Obj.pntMuunInventory[lpMsg->Pos]);

		if (lpMsg->Pos == 0 || lpMsg->Pos == 1)
		{
			WORD nItemCode = Obj.pntMuunInventory[lpMsg->Pos]->m_Type;
			UINT64 nSerial = Obj.pntMuunInventory[lpMsg->Pos]->m_Number;

			if (lpMsg->Pos == 0)
			{
				Obj.m_wMuunItem = -1;
			}

			else if (lpMsg->Pos == 1)
			{
				Obj.m_wMuunSubItem = -1;
			}

			g_CMuunSystem.RemoveUserMuunEffect(lpObj, lpMsg->Pos);
			this->GCMuunEquipmentChange(Obj, lpMsg->Pos);
		}

		gObjMuunInventoryDeleteItem(Obj, lpMsg->Pos);
		gObjMakePreviewCharSet(Obj);

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
	}
}

void GameProtocol::CGReqEventInvenOpen(BYTE* lpRecv, CGameObject &Obj)
{
	if (g_CMuRummyMng.IsMuRummyEventOn() == true)
	{
		g_CMuRummyMng.CGReqMuRummyEventOpen((PMSG_REQ_MURUMMY_EVENT_OPEN *)lpRecv, Obj);
		return;
	}

	if (g_ConfigRead.data.common.EventInventoryOpen)
	{
		PMSG_ANS_MURUMMY_EVENT_OPEN pMsg;
		PHeadSubSetB((BYTE*)&pMsg, 0x4D, 0x0F, sizeof(pMsg));

		pMsg.btResult = 3;

		time_t tmEndTime;
		DWORD dw32EndTime;
		SystemTimeToUnixTime(&g_ConfigRead.data.common.EventEndDate, &tmEndTime);
		dw32EndTime = (DWORD)tmEndTime;

		pMsg.btEventTime1 = SET_NUMBERH(SET_NUMBERHW(dw32EndTime));
		pMsg.btEventTime2 = SET_NUMBERL(SET_NUMBERHW(dw32EndTime));
		pMsg.btEventTime3 = SET_NUMBERH(SET_NUMBERLW(dw32EndTime));
		pMsg.btEventTime4 = SET_NUMBERL(SET_NUMBERLW(dw32EndTime));

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}
}

void GameProtocol::CGEventItemGetRequest(PMSG_EVENTITEM_GETREQUEST *lpMsg, CGameObject &Obj)
{

	int item_num, map_num;
	PMSG_EVENTITEM_GETRESULT pResult;
	int type;
	int level;
	int special;
	int NOption;
	char szItemName[50];
	CMapItem * lpItem;

	pResult.h.c = 0xC3;
	pResult.h.headcode = 0x4D;
	pResult.h.subcode = 0x00;
	pResult.h.size = sizeof(pResult);
	pResult.result = -1;

	pResult.index[0] = lpMsg->NumberH;
	pResult.index[1] = lpMsg->NumberL;

	if (!gObjIsConnected(Obj))
	{
		//this->GCSendDisableReconnect(Obj);
		IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (Obj.CloseType != -1)
		return;

	if (g_CMuRummyMng.IsMuRummyEventOn() == false && g_ConfigRead.data.common.EventInventoryOpen == false)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	if (Obj.m_PlayerData->m_bSecurityCheck == false)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	if (Obj.DieRegen != 0)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	if (g_NewPVP.IsObserver(Obj) == TRUE)
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	if (!gObjFixEventInventoryPointer(Obj))
		sLog->outBasic("[Fix EventInv.Ptr] False Location - %s, %d", __FILE__, __LINE__);

	if (Obj.pTransaction == TRUE)
	{
		sLog->outBasic("[%s][%s] CGEventItemGetRequest() Failed : Transaction == 1, IF_TYPE : %d",
			Obj.AccountID, Obj.Name, Obj.m_IfState->type);
		return;

	}

	item_num = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	if (MaxItemTypeRange(item_num) == FALSE)
	{
		sLog->outBasic("error-L3 : %s %d", __FILE__, __LINE__);
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	map_num = Obj.MapNumber;

	if (MAX_MAP_RANGE(map_num) == FALSE)
	{
		sLog->outBasic("error-L3 : %s %d", __FILE__, __LINE__);
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

		return;
	}

	lpItem = &MapC[map_num].m_CItemObject[item_num];

	if (lpItem->IsItem() == TRUE && lpItem->Give == false && lpItem->live == true)
	{
		type = lpItem->m_Type;
		level = lpItem->m_Level;
		special = lpItem->m_Special[0];
		NOption = lpItem->m_NewOption;
		LPITEM_ATTRIBUTE pItemAttribute = GetItemAttr(type);

		if (!pItemAttribute)
		{
			return;
		}

		if (pItemAttribute->ItemKindA != 11)
		{
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);

			return;
		}

		if (lpItem->m_Type == ITEMGET(14, 278))
		{
			int iInventoryIndex;

			for (int iLoop = 0; iLoop < 32; iLoop++)
			{
				iInventoryIndex = -1;

				if (Obj.pntEventInventory[iLoop]->m_Type == ITEMGET(14, 278) &&
					Obj.pntEventInventory[iLoop]->m_Durability < 25.0)
				{
					iInventoryIndex = iLoop;
				}

				if (iInventoryIndex < 0 || iInventoryIndex > EVENT_INVENTORY_SIZE - 1)
				{
					continue;
				}

				if (MapC[map_num].ItemGive(Obj, item_num, 1) == TRUE)
				{
					BYTE NewOption[MAX_EXOPTION_SIZE];

					::ItemIsBufExOption(NewOption, (lpItem != NULL) ? (CItemObject*)&lpItem->m_Number : NULL);

					pResult.result = -3;
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

					Obj.pntEventInventory[iInventoryIndex]->m_Durability += lpItem->m_Durability;

					if (Obj.pntEventInventory[iInventoryIndex]->m_Durability < 25.0)
					{
						this->GCEventItemDurSend(Obj, iInventoryIndex, Obj.pntEventInventory[iInventoryIndex]->m_Durability);
						return;
					}

					int iDurGap = Obj.pntEventInventory[iInventoryIndex]->m_Durability - 25.0;
					BYTE btRet = gObjEventInventoryInsertItemTemp(&Obj, lpItem);
					gObjEventInventoryItemSet(Obj, iInventoryIndex, -1);
					Obj.pntEventInventory[iInventoryIndex]->Clear();
					this->GCEventInventoryItemDeleteSend(Obj, iInventoryIndex, 1);

					ItemCreate(Obj, 226, Obj.X, Obj.Y, ITEMGET(14, 244), lpItem->m_Level, 0, 0, 0, 0, Obj, 0, 0, 0, 0, 0);

					if (iDurGap <= 0)
					{
						return;
					}

					if (btRet == 255)
					{
						ItemCreate(Obj, Obj.MapNumber, Obj.X, Obj.Y, ItemGetNumberMake(14, 278), lpItem->m_Level, 0, 0, 0, 0, Obj, 0, 0, 0, 0, 0);
						return;
					}

					lpItem->m_State = 2;
					lpItem->Give = 0;
					lpItem->live = 1;
					lpItem->m_Durability = iDurGap;
				}
			}
		}

		if (lpItem->m_Type == ITEMGET(14, 215))
		{
			int iInventoryIndex;

			for (int iLoop = 0; iLoop < 32; iLoop++)
			{
				iInventoryIndex = -1;

				if (Obj.pntEventInventory[iLoop]->m_Type == ITEMGET(14, 215) &&
					Obj.pntEventInventory[iLoop]->m_Durability < 24.0)
				{
					iInventoryIndex = iLoop;
				}

				if (iInventoryIndex < 0 || iInventoryIndex > EVENT_INVENTORY_SIZE - 1)
				{
					continue;
				}

				if (MapC[map_num].ItemGive(Obj, item_num, 1) == TRUE)
				{
					BYTE NewOption[MAX_EXOPTION_SIZE];

					::ItemIsBufExOption(NewOption, (lpItem != NULL) ? (CItemObject*)&lpItem->m_Number : NULL);

					pResult.result = -3;
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

					Obj.pntEventInventory[iInventoryIndex]->m_Durability += lpItem->m_Durability;

					if (Obj.pntEventInventory[iInventoryIndex]->m_Durability < 24.0)
					{
						this->GCEventItemDurSend(Obj, iInventoryIndex, Obj.pntEventInventory[iInventoryIndex]->m_Durability);
						return;
					}

					int iDurGap = Obj.pntEventInventory[iInventoryIndex]->m_Durability - 24.0;
					BYTE btRet = gObjEventInventoryInsertItemTemp(&Obj, lpItem);
					gObjEventInventoryItemSet(Obj, iInventoryIndex, -1);
					Obj.pntEventInventory[iInventoryIndex]->Clear();
					this->GCEventInventoryItemDeleteSend(Obj, iInventoryIndex, 1);

					ItemCreate(Obj, 226, Obj.X, Obj.Y, ITEMGET(14, 216), lpItem->m_Level, 0, 0, 0, 0, Obj, 0, 0, 0, 0, 0);

					if (iDurGap <= 0)
					{
						return;
					}

					if (btRet == 255)
					{
						ItemCreate(Obj, Obj.MapNumber, Obj.X, Obj.Y, ItemGetNumberMake(14, 215), lpItem->m_Level, 0, 0, 0, 0, Obj, 0, 0, 0, 0, 0);
						return;
					}

					lpItem->m_State = 2;
					lpItem->Give = 0;
					lpItem->live = 1;
					lpItem->m_Durability = iDurGap;
				}
			}
		}

		pResult.result = gObjEventInventoryInsertItemTemp(&Obj, lpItem);

		if (pResult.result == 255)
		{
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}

		ItemByteConvert((BYTE*)&pResult.Data[0], lpItem->m_Type, lpItem->m_Option1, lpItem->m_Option2,
			lpItem->m_Option3, lpItem->m_Level, lpItem->m_Durability, lpItem->m_NewOption,
			lpItem->m_SetOption, lpItem->m_JewelOfHarmonyOption, lpItem->m_ItemOptionEx, lpItem->m_SocketOption, lpItem->m_BonusSocketOption, 0);

		if (MapC[map_num].ItemGive(Obj, item_num, false) == TRUE)
		{
			BYTE Ret = ::gObjEventInventoryInsertItem(Obj, lpItem);

			if (Ret == 0xFF)
				pResult.result = -1;

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

			if (Ret == 0xFF)
			{
				this->GCEventInventoryItemListSend(Obj);
			}
			else
			{
				BYTE NewOption[MAX_EXOPTION_SIZE];

				::ItemIsBufExOption(NewOption, (lpItem != NULL) ? (CItemObject*)&lpItem->m_Number : NULL);

				this->GCSendGetItemInfoForParty(Obj, lpItem);
			}
		}

		else
		{
			pResult.result = -1;
			pResult.h.size -= sizeof(pResult.Data);

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}
	}

	else
	{
		pResult.result = -1;
		pResult.h.size -= sizeof(pResult.Data);

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}
}

bool GameProtocol::CGEventItemDropRequest(PMSG_EVENTITEM_THROW *lpMsg, CGameObject &Obj)
{
	BOOL CopyItem = FALSE;
	BYTE NewOption[MAX_EXOPTION_SIZE];

	if (!PacketCheckTime(Obj))
		return FALSE;

	if (Obj.Type != OBJ_USER)
	{
		return FALSE;
	}

	PMSG_EVENTITEM_THROW_RESULT pResult;

	PHeadSubSetB((BYTE*)&pResult, 0x4D, 0x01, sizeof(pResult));
	pResult.Result = true;
	pResult.Ipos = lpMsg->Ipos;

	if (Obj.m_IfState->use != 0)
	{
		if (Obj.m_IfState->type != 3)
		{
			pResult.Result = false;
			return FALSE;
		}
	}

	if (Obj.m_PlayerData->m_bSecurityCheck == false)
	{
		MsgOutput(Obj, Lang.GetText(0, 394));
		pResult.Result = false;
	}

	if (Obj.CloseType != -1)
		pResult.Result = false;

	if (Obj.m_bMapSvrMoveQuit == true)
	{
		sLog->outBasic("[ANTI-HACK][Item Duplication] - Item Drop during MapServer Move [%s][%s]",
			Obj.AccountID, Obj.Name);
		pResult.Result = false;
	}

	if (!::gObjFixEventInventoryPointer(Obj))
		sLog->outBasic("[Fix EventInv.Ptr] False Location - %s, %d", __FILE__, __LINE__);

	if (Obj.pTransaction == 1)
	{
		sLog->outBasic("[%s][%s] CGEventItemDropRequest() Failed : Transaction == 1, IF_TYPE : %d",
			Obj.AccountID, Obj.Name, Obj.m_IfState->type);
		pResult.Result = false;

		return false;
	}

	

	if (lpMsg->Ipos == 0xFF)
	{
		sLog->outBasic("error:%s %d", __FILE__, __LINE__);
		pResult.Result = false;
	}

	if (lpMsg->Ipos < 0 || lpMsg->Ipos > MAIN_INVENTORY_SIZE - 1)
		pResult.Result = false;

	if (!gObjCanItemTouch(Obj, 0))
		pResult.Result = false;

	if (!IsDumpItem(Obj.pntEventInventory[lpMsg->Ipos]->m_Type))
		pResult.Result = false;

	LPITEM_ATTRIBUTE pItemAttribute = GetItemAttr(Obj.pntEventInventory[lpMsg->Ipos]->m_Type);

	if (!pItemAttribute)
	{
		return false;
	}

	if (pItemAttribute->ItemKindA != 11)
		pResult.Result = false;

	if (pResult.Result == 1)
	{
		::ItemIsBufExOption(NewOption, Obj.pntEventInventory[lpMsg->Ipos]);

		int map = Obj.MapNumber;
		int type = Obj.pntEventInventory[lpMsg->Ipos]->m_Type;
		int level = Obj.pntEventInventory[lpMsg->Ipos]->m_Level;
		float dur = Obj.pntEventInventory[lpMsg->Ipos]->m_Durability;
		BOOL bIsItem = Obj.pntEventInventory[lpMsg->Ipos]->IsItem();
		BYTE Option1 = Obj.pntEventInventory[lpMsg->Ipos]->m_Option1;
		BYTE Option2 = Obj.pntEventInventory[lpMsg->Ipos]->m_Option2;
		BYTE Option3 = Obj.pntEventInventory[lpMsg->Ipos]->m_Option3;
		BYTE NOption = Obj.pntEventInventory[lpMsg->Ipos]->m_NewOption;
		UINT64 serial = Obj.pntEventInventory[lpMsg->Ipos]->m_Number;
		int PetLevel = Obj.pntEventInventory[lpMsg->Ipos]->m_PetItem_Level;
		UINT64 PetExp = Obj.pntEventInventory[lpMsg->Ipos]->m_PetItem_Exp;
		BYTE SOption = Obj.pntEventInventory[lpMsg->Ipos]->m_SetOption;
		BYTE btItemEffectEx = Obj.pntEventInventory[lpMsg->Ipos]->m_ItemOptionEx;
		UINT64 serial2 = Obj.pntEventInventory[lpMsg->Ipos]->m_Number;
		char szItemName[50] = "Item";
		BYTE JOHOption = g_kJewelOfHarmonySystem.GetItemStrengthenOption(Obj.pntEventInventory[lpMsg->Ipos]);
		BYTE JOHOptionLevel = g_kJewelOfHarmonySystem.GetItemOptionLevel(Obj.pntEventInventory[lpMsg->Ipos]);

		//Season 4 add-on
		BYTE SocketOption[5];
		BYTE SocketIndex;
		g_SocketOptionSystem.GetSocketOption(Obj.pntEventInventory[lpMsg->Ipos], SocketOption, SocketIndex);

		for (int iOption = 0; iOption < 5; iOption++)
		{
			SocketOption[iOption] = Obj.pntEventInventory[lpMsg->Ipos]->m_SocketOption[iOption];
		}

		if (gObjCheckSerial0ItemList(Obj.pntEventInventory[lpMsg->Ipos]) != FALSE)
		{
			MsgOutput(Obj, Lang.GetText(0, 259));
			sLog->outBasic("[ANTI-HACK][Serial 0 Item] [ItemDrop] (%s)(%s) Item(%s) Pos(%d)",
				Obj.AccountID, Obj.Name, Obj.pntEventInventory[lpMsg->Ipos]->GetName(),
				lpMsg->Ipos);

			CopyItem = TRUE;
			pResult.Result = false;
			bIsItem = FALSE;
		}

		if (!::gObjEventInventorySearchSerialNum(Obj, serial2))
		{
			CopyItem = TRUE;
			pResult.Result = false;
			bIsItem = FALSE;
		}

		if (bIsItem == TRUE)
		{
			std::strcpy(szItemName, Obj.pntEventInventory[lpMsg->Ipos]->GetName());

			if (g_BagManager.SearchAndUseBag(Obj, BAG_COMMON, type, level) == true)
			{
				gObjEventInventoryDeleteItem(Obj, lpMsg->Ipos);
			}
			else if (type == ITEMGET(13, 210) && level == 0)
			{
				gObjEventInventoryDeleteItem(Obj, lpMsg->Ipos);
				g_BagManager.SearchAndUseBag(Obj, BAG_EVENT, EVENTBAG_EGG_PETITEM, Obj.m_Index);
			}
			else if (type == ITEMGET(13, 220) && level == 0)
			{
				gObjEventInventoryDeleteItem(Obj, lpMsg->Ipos);
				g_BagManager.SearchAndUseBag(Obj, BAG_EVENT, EVENTBAG_EGG_LUCKYBAG, Obj.m_Index);

			}

			else if (type == ITEMGET(13, 221) && level == 0)
			{
				gObjEventInventoryDeleteItem(Obj, lpMsg->Ipos);
				g_BagManager.SearchAndUseBag(Obj, BAG_EVENT, EVENTBAG_EGG_FIREGHOST, Obj.m_Index);

			}

			else if (type == ITEMGET(13, 222) && level == 0)
			{
				gObjEventInventoryDeleteItem(Obj, lpMsg->Ipos);
				g_BagManager.SearchAndUseBag(Obj, BAG_EVENT, EVENTBAG_EGG_GOLDGOBLIN, Obj.m_Index);

			}

			else if (type == ITEMGET(14, 244))
			{
				if (g_EvoMonMng.UseSummonScroll(Obj) == FALSE)
				{
					pResult.Result = false;
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

					return FALSE;
				}

				gObjEventInventoryDeleteItem(Obj, lpMsg->Ipos);
			}

			else
			{
				if (MapC[map].ItemDrop(type, level, dur, lpMsg->px, lpMsg->py, Option1, Option2, Option3,
					NOption, SOption, serial2, Obj.m_index, PetLevel, PetExp, btItemEffectEx, SocketOption, SocketIndex, 0) == TRUE)
				{
					gObjEventInventoryDeleteItem(Obj, lpMsg->Ipos);
					pResult.Result = true;

				}

				else
				{
					pResult.Result = false;
				}
			}
		}

		else
		{
			pResult.Result = false;
		}
	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

	if (CopyItem != FALSE)
	{
		this->GCEventInventoryItemListSend(Obj);
	}
}

void GameProtocol::GCEventInventoryItemListSend(CGameObject &Obj)
{
	PMSG_INVENTORYLISTCOUNT pMsgILC;
	PMSG_INVENTORYLIST pMsgIL;
	int sOfs = sizeof(PMSG_INVENTORYLISTCOUNT);
	int pMsgILSize = sizeof(PMSG_INVENTORYLIST);
	BYTE sendBuf[4096];
	
	int itemcount = 0;

	for (int n = 0; n < EVENT_INVENTORY_SIZE; n++)
	{
		if (Obj.pntEventInventory[n]->IsItem() == TRUE)
		{
			if (Obj.pntEventInventory[n]->m_bItemExist == false)
			{
				continue;
			}

			pMsgIL.Pos = n;
			ItemByteConvert(pMsgIL.ItemInfo, Obj.pntEventInventory[n]);
			std::memcpy(&sendBuf[sOfs], &pMsgIL, pMsgILSize);
			itemcount++;
			sOfs += pMsgILSize;
		}
	}

	pMsgILC.h.c = 0xC4;
	pMsgILC.h.headcode = 0x4D;
	pMsgILC.subcode = 0x02;
	pMsgILC.h.sizeH = SET_NUMBERH(sOfs);
	pMsgILC.h.sizeL = SET_NUMBERL(sOfs);
	pMsgILC.Count = itemcount;
	std::memcpy(sendBuf, &pMsgILC, sizeof(PMSG_INVENTORYLISTCOUNT));

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, sendBuf, sOfs);
}

void GameProtocol::GCEventItemDurSend(CGameObject &Obj, BYTE pos, BYTE dur)
{
	PMSG_EVENTITEM_DUR pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0x4D, 0x03, sizeof(pMsg));
	pMsg.IPos = pos;
	pMsg.Dur = dur;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCEventInvenItemOneSend(CGameObject &Obj, int pos)
{
	if (!Obj.pntEventInventory[pos]->IsItem())
		return;

	PMSG_EVENT_INVEN_ITEM_MOD pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0x4D, 0x05, sizeof(pMsg));
	pMsg.Pos = pos;
	ItemByteConvert(pMsg.ItemInfo, Obj.pntEventInventory[pos]);

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR *)&pMsg, pMsg.h.size);
}

void GameProtocol::GCEventInventoryItemDeleteSend(CGameObject &Obj, BYTE pos, BYTE flag)
{
	PMSG_EVENTINVENTORY_DELETE pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0x4D, 0x04, sizeof(pMsg));
	pMsg.IPos = pos;
	pMsg.Flag = flag;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCResSendExpEventInfo(CGameObject &Obj)
{
	PMSG_RES_EXPEVENTINFO pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xBF, 0x52, sizeof(pMsg));

	pMsg.wPCBangRate = 0;
	pMsg.wExpRate = g_ExpManager.GetEventExp() / g_ExpManager.GetStaticExp() * 100.0;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::CGReq_ITL_Relics(PMSG_REQ_ITL_RELATE_RELICS *lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj))
	{
		return;
	}

	if (!gObjIsConnected(Obj))
	{
		return;
	}

	if (!lpMsg)
	{
		return;
	}

	WORD NpcNumber = MAKE_NUMBERW(lpMsg->NumberH, lpMsg->NumberL);

	if (NpcNumber >= g_ConfigRead.server.GetObjectMax())
	{
		return;
	}

	

	if (!lpObj)
	{
		return;
	}

	if (Obj.MapNumber != getGameObject(NpcNumber)->MapNumber)
	{
		return;
	}

	sLog->outBasic("[ ITL ][ ITR ][ActRelicsGetOrRegister] NpcIndex:%d,byPrePacket:%d,byAct:%d,wNpcType:%d",
		NpcNumber, lpMsg->byPrePacket, lpMsg->byAct, lpMsg->wNpcType);

	if (IT_MAP_RANGE(Obj.MapNumber))
	{
		g_IT_Event.ActRelicsGetOrRegister(*getGameObject(NpcNumber), lpObj, Obj.MapNumber, lpMsg->byPrePacket, lpMsg->byAct, lpMsg->wNpcType);
	}
}

void GameProtocol::CG_Req_Enter_ITR(PMSG_REQ_ENTER_ITR *lpMsg, CGameObject &Obj)
{
	_ITL_AnswerEnter pResult;
	PHeadSubSetB((BYTE*)&pResult, 0xBF, 0x70, sizeof(pResult));
	std::memset(pResult.Name, 0x00, sizeof(pResult.Name));

	if (g_IT_Event.IsCanEnterTiming() == false)
	{
		pResult.bReason = 8;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (Obj.PartyNumber < 0)
	{
		pResult.bReason = 1;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (gParty.Isleader(Obj.PartyNumber, iIndex, Obj.DBNumber) == false)
	{
		pResult.bReason = 3;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (gParty.GetCount(Obj.PartyNumber) < g_IT_Event.m_iITR_MinUserInParty)
	{
		pResult.bReason = 4;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (gParty.GetPKPartyPenalty(Obj.PartyNumber) >= 5)
	{
		pResult.bReason = 12;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	int PartyUsersIndex = 0;
	BOOL bResult = this->EnterITRCheckUser(iIndex, PartyUsersIndex);

	if (bResult != 0)
	{
		pResult.bReason = bResult;
		std::memcpy(pResult.Name, getGameObject(PartyUsersIndex)->Name, MAX_ACCOUNT_LEN + 1);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		if (iIndex != PartyUsersIndex)
		{
			IOCP.DataSend(PartyUsersIndex, (BYTE*)&pResult, pResult.h.size);
		}

		return;
	}

	BYTE bySlotNum = -1;
	int nEnterIllusionIndex = g_IT_Event.CheckEnterLevel(Obj);
	BYTE byMapNumber = g_IT_Event.Find_EmptySlot(Obj.PartyNumber, bySlotNum, nEnterIllusionIndex);

	if (byMapNumber > 6)
	{
		pResult.bReason = 14;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	BOOL bUpdateOk = g_IT_Event.Update_PartyInfo(byMapNumber, bySlotNum, Obj.PartyNumber, iIndex);

	if (bUpdateOk != 0)
	{
		sLog->outBasic("[ITR][Update_PartyInfo] ERRORcode:%d,NAME:%s,partyindex:%d",
			bUpdateOk, Obj.Name, Obj.PartyNumber);

		pResult.bReason = -1;
		std::memcpy(pResult.Name, Obj.Name, MAX_ACCOUNT_LEN + 1);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	int dbnumber = 0;
	int useridx;

	for (int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		if (gParty.GetIndexUser(Obj.PartyNumber, i, useridx, dbnumber) == FALSE)
		{
			pResult.bReason = 13;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}

		if (useridx > 0)
		{
			pResult.bReason = 0;
			std::memcpy(pResult.Name, Obj.Name, MAX_ACCOUNT_LEN + 1);
			IOCP.DataSend(useridx, (BYTE*)&pResult, pResult.h.size);
		}
	}
}

void GameProtocol::CG_ReqAcceptEnterITR(PMSG_REQ_ACCEPTENTER_ITR *lpMsg, CGameObject &Obj)
{
	if (!gObjIsConnected(Obj))
	{
		return;
	}

	_ITR_AcceptEnter pResult;
	PHeadSubSetB((BYTE*)&pResult, 0xBF, 0x71, sizeof(pResult));
	std::memset(pResult.Name, 0x00, sizeof(pResult.Name));

	int byOkCount = 0;
	int UserIndex;
	int dbnumber;
	BYTE byEnterOK = lpMsg->byEnterOK;

	if (gParty.IsParty(Obj.PartyNumber) == FALSE)
	{
		sLog->outBasic("[ ITR ][CGReqAcceptEnterITR] reqman is not party Error Name :%s,", Obj.Name);
		g_IT_Event.Delete_PartyInfo(Obj.PartyNumber);
		gParty.ClearPartyAuthITR(Obj.PartyNumber);
		return;
	}

	if (byEnterOK == TRUE)
	{
		sLog->outBasic("[ ITR ][CGReqAcceptEnterITR] reqman opposite Name :%s,", Obj.Name);
		g_IT_Event.Delete_PartyInfo(Obj.PartyNumber);
		gParty.ClearPartyAuthITR(Obj.PartyNumber);

		pResult.byError = -1;
		std::memcpy(pResult.Name, Obj.Name, MAX_ACCOUNT_LEN + 1);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (gParty.EnterITR_PartyAuth(Obj.PartyNumber, Obj) == FALSE)
	{
		sLog->outBasic("[ ITR ][CGReqAcceptEnterITR][EnterITR_PartyAuth] Internal Error Name :%s", Obj.Name);
		g_IT_Event.Delete_PartyInfo(Obj.PartyNumber);
		gParty.ClearPartyAuthITR(Obj.PartyNumber);

		pResult.byError = -1;
		std::memcpy(pResult.Name, Obj.Name, MAX_ACCOUNT_LEN + 1);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (gParty.AllAgreeEnterITR(Obj.PartyNumber) == FALSE)
	{
		sLog->outBasic("[ ITR ] [CGReqAcceptEnterITR] Enter Agree NAME:%s, G.NAME:%s,AgreeCnt:%d",
			Obj.Name, Obj.m_PlayerData->GuildName, gParty.ReturnOKCountITR(Obj.PartyNumber));
		return;
	}

	int TroubleUserIndex = -1;
	BOOL bResult = EnterITRCheckUser(Obj, TroubleUserIndex);

	if (bResult != 0)
	{
		pResult.byError = bResult;
		std::memcpy(pResult.Name, getGameObject(TroubleUserIndex)->Name, MAX_ACCOUNT_LEN + 1);
		int nAllPartyUserIndex;

		for (int j = 0; j < MAX_USER_IN_PARTY; j++)
		{
			gParty.GetIndexUser(Obj.PartyNumber, j, nAllPartyUserIndex, dbnumber);
			IOCP.DataSend(nAllPartyUserIndex, (BYTE*)&pResult, pResult.h.size);
		}

		g_IT_Event.Delete_PartyInfo(Obj.PartyNumber);
		gParty.ClearPartyAuthITR(Obj.PartyNumber);
		return;
	}

	for (int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		int pIndex = gParty.GetIndexUser(Obj.PartyNumber, i, UserIndex, dbnumber);
		CGameObject* lpObjPartyUser = getGameObject(pIndex);
		if (pIndex != -1)
		{
			if (gParty.m_PartyS[Obj.PartyNumber].bITRWithYouOk[i] == FALSE)
			{
				continue;
			}

			BYTE byITRMapIndex = -1;
			BYTE bySlotNum = -1;
			BOOL bReturn = g_IT_Event.Find_ExistingPartyInfo(Obj.PartyNumber, byITRMapIndex, bySlotNum);

			if (bReturn)
			{
				sLog->outBasic("[ ITR ] [Find_ExistingPartyInfo]ERROR return:%d Name:%s, mapnumber:%d, ITLMapIndex:%d,bySlotNum:%d ",
					bReturn, Obj.Name, Obj.MapNumber, byITRMapIndex, bySlotNum);

				return;
			}

			if (g_IT_Event.Enter_ITR(*lpObjPartyUser, byITRMapIndex, bySlotNum) == TRUE)
			{
				pResult.byError = 0;
				std::memcpy(pResult.Name, lpObjPartyUser->Name, MAX_ACCOUNT_LEN + 1);
				IOCP.DataSend(UserIndex, (BYTE*)&pResult, pResult.h.size);

				lpObjPartyUser->MapNumber = byITRMapIndex + 45;
				gObjMoveGate(*lpObjPartyUser, byITRMapIndex + 142);
			}
		}
	}
}

BYTE GameProtocol::EnterITRCheckUser(CGameObject &Obj, int &TroubleUserIndex)
{
	int PartyLeaderITRIndex = g_IT_Event.CheckEnterLevel(Obj);

	if (PartyLeaderITRIndex == -1)
	{
		return 5;
	}

	int useridx = 0;
	int dbnumber = 0;

	for (int i = 0; i < MAX_USER_IN_PARTY; i++)
	{
		BYTE bReturn = gParty.GetIndexUser(Obj.PartyNumber, i, useridx, dbnumber);
		TroubleUserIndex = useridx;

		if (bReturn == FALSE)
		{
			return 13;
		}

		if (useridx > 0)
		{
			if (getGameObject(useridx)->MapNumber != MAP_INDEX_LORENMARKET &&
				IT_MAP_RANGE(getGameObject(useridx)->MapNumber) == FALSE)
			{
				return 10;
			}

			if (getGameObject(useridx)->PartyNumber != Obj.PartyNumber)
			{
				return 13;
			}

			bReturn = g_IT_Event.CheckEnterLevel(useridx);

			if (PartyLeaderITRIndex != bReturn)
			{
				return 5;
			}

			if (g_IT_Event.CheckWearingMOPH(useridx) == TRUE)
			{
				return 7;
			}

			if (getGameObject(useridx)->m_IfState->use && getGameObject(useridx)->m_IfState->type != 12)
			{
				return 11;
			}

			if (getGameObject(useridx)->m_PK_Level >= 4)
			{
				return 12;
			}

			if (gObjDuelCheck(*getGameObject(useridx)) == TRUE)
			{
				return 15;
			}

			if (getGameObject(useridx)->m_PlayerData->lpGuild != NULL &&
				getGameObject(useridx)->m_PlayerData->lpGuild->WarState == TRUE)
			{
				return 16;
			}

			if (getGameObject(useridx)->pntInventory[getGameObject(useridx)->m_btInvenPetPos]->m_Type == ITEMGET(13, 3) ||
				getGameObject(useridx)->pntInventory[getGameObject(useridx)->m_btInvenPetPos]->m_Type == ITEMGET(13, 2) ||
				getGameObject(useridx)->pntInventory[getGameObject(useridx)->m_btInvenPetPos]->m_Type == ITEMGET(13, 37))
			{
				return 17;
			}
		}
	}

	return 0;
}

void GameProtocol::GCSendRareItemPriceInfo(CGameObject &Obj)
{
	PMSG_RAREITEMPIRCESEND pMsg;
	pMsg.h.set((BYTE*)&pMsg, 0xFA, 0xA4, sizeof(pMsg));

	pMsg.btItemSellPriceDivisor = g_ConfigRead.itemPrices.btItemSellPriceDivisor;
	pMsg.iJewelOfBlessPrice = g_ConfigRead.itemPrices.iJewelOfBlessPrice;
	pMsg.iJewelOfSoulPrice = g_ConfigRead.itemPrices.iJewelOfSoulPrice;;
	pMsg.iJewelOfChaosPrice = g_ConfigRead.itemPrices.iJewelOfChaosPrice;
	pMsg.iCrestOfMonarchPrice = g_ConfigRead.itemPrices.iCrestOfMonarchPrice;
	pMsg.iJewelOfCreationPrice = g_ConfigRead.itemPrices.iJewelOfCreationPrice;
	pMsg.iLochFeatherPrice = g_ConfigRead.itemPrices.iLochFeatherPrice;
	pMsg.iWereRabbitEggPrice = g_ConfigRead.itemPrices.iWereRabbitEggPrice;
	pMsg.iJewelOfGuardianPrice = g_ConfigRead.itemPrices.iJewelOfGuardianPrice;
	pMsg.iJewelOfLifePrice = g_ConfigRead.itemPrices.iJewelOfLifePrice;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
}

void GameProtocol::GCSendDisableReconnect(CGameObject &Obj)
{
	PMSG_DISABLE_RECONNECT pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0xFA, sizeof(pMsg));
	pMsg.subcode = 0xA5;

	for (int i = 0; i < 100; i++)
	{
		pMsg.Trash[i] = rand() % 0xFF;
	}

	Obj.m_dwDCTimer = GetTickCount64();

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));

}
void GameProtocol::GCSendDisableReconnectSystem(CGameObject &Obj)
{
	PMSG_DISABLE_RECONNECT pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0xFA, sizeof(pMsg));
	pMsg.subcode = 0xA7;

	for (int i = 0; i < 100; i++)
	{
		pMsg.Trash[i] = rand() % 0xFF;
	}

	//Obj.m_dwDCTimer = GetTickCount64();

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));

}
void GameProtocol::CGReqUBFMyCharacterInfo(PMSG_REQ_UBF_INFO *lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj))
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (lpMsg->btUBFKind == 0)
	{
		g_UnityBattleField.GDReqCheckJoinedUnityBattleField(iIndex, g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE, 0);
	}
}

void GameProtocol::CGReqUBFJoin(PMSG_REQ_UBF_JOIN *lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj))
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (lpMsg->btUBFKind == 0)
	{
		g_UnityBattleField.GDReqJoinUnityBattleField(Obj);
	}
}

void GameProtocol::CGReqUBFCancel(PMSG_REQ_UBF_CANCEL *lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj))
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (lpMsg->btCanceled == 1)
	{
		g_UnityBattleField.GDReqCancelUnityBattleField(iIndex, 1);
	}
}

void GameProtocol::CGReqUBFGetRealName(PMSG_REQ_UBF_REAL_NAME *lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj))
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (lpMsg->btRequest == 1)
	{
		g_UnityBattleField.GDReqGetRealNameAndServerCode(Obj);
	}
}

void GameProtocol::CGReqUBFGetReward(PMSG_REQ_GET_REWARD *lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj))
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	g_UnityBattleField.GDReqUBFGetReward(iIndex, lpMsg->btBattleKind);
}

void GameProtocol::CGReqDSFSchedule(CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj))
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (!gObjIsConnected(Obj))
	{
		return;
	}

	if (g_ConfigRead.server.GetServerType() != SERVER_BATTLECORE)
	{
		return;
	}

	if (Obj.MapNumber != MAP_INDEX_LORENMARKET)
	{
		return;
	}

	g_DevilSquareFinal.ReqDSFSchedule(Obj);
}

void GameProtocol::CGReqDSFEnter(CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj))
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	PMSG_ANS_DSF_ENTER pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xDB, 0x01, sizeof(pMsg));

	std::memset(pMsg.Name, 0x00, sizeof(pMsg.Name));
	pMsg.btDSFType = g_DevilSquareFinal.m_nDSF_TYPE;

	if (g_DevilSquareFinal.m_btDSFCurState != 0)
	{
		pMsg.btResult = 8;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (g_DevilSquareFinal.m_bEntrance == false)
	{
		pMsg.btResult = 8;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (gParty.GetLorenMarketUserCount(Obj.PartyNumber) != 2)
	{
		sLog->outBasic("[DSF][CGReqDSFEnter][%s][%s][%s][%d] EnterError : Party User is not in RorenMarket",
			Obj.AccountID, Obj.m_PlayerData->m_RealNameOfUBF, Obj.Name, Obj.m_PlayerData->m_nServerCodeOfHomeWorld);

		pMsg.btResult = 10;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (gParty.GetPKPartyPenalty(Obj.PartyNumber) >= 5)
	{
		pMsg.btResult = 12;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	int PartyUsersIndex = 0;
	BOOL bResult = this->EnterDSFCheckUser(iIndex, PartyUsersIndex);

	if (bResult != 0)
	{
		pMsg.btResult = bResult;
		std::memcpy(pMsg.Name, getGameObject(PartyUsersIndex)->Name, MAX_ACCOUNT_LEN + 1);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

		if (PartyUsersIndex != iIndex)
		{
			IOCP.DataSend(PartyUsersIndex, (BYTE*)&pMsg, pMsg.h.size);
		}

		return;
	}

	BOOL bExistAlready = g_DevilSquareFinal.IsAlreadyExistUserInDSF(Obj);

	if (bExistAlready != FALSE)
	{
		g_DevilSquareFinal.DeletePartyByIndex(Obj);
	}

	if (g_DevilSquareFinal.m_nDSF_TYPE == 1)
	{
		BYTE btSlotNum = -1;
		BYTE btRet = g_DevilSquareFinal.Find_EmptyPartySlot(Obj.PartyNumber, btSlotNum);

		if (btRet >= 0xFE)
		{
			pMsg.btResult = 14;
			std::memcpy(pMsg.Name, Obj.Name, sizeof(pMsg.Name));
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

			return;
		}

		BOOL bUpdateOk = g_DevilSquareFinal.SetParty(btSlotNum, 1, Obj.PartyNumber, iIndex);

		if (bUpdateOk != FALSE)
		{
			sLog->outBasic("[DSF][CGReqDSFEnter][%s][%s][%s][%d] ERROR code:%d",
				Obj.AccountID, Obj.m_PlayerData->m_RealNameOfUBF, Obj.Name, Obj.m_PlayerData->m_nServerCodeOfHomeWorld, bUpdateOk);

			pMsg.btResult = -1;
			std::memcpy(pMsg.Name, Obj.Name, sizeof(pMsg.Name));
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

			return;
		}

		int dbnumber = 0, useridx = -1;

		for (int i = 0; i < 2; i++)
		{
			BOOL bError = gParty.GetIndexUser(Obj.PartyNumber, i, useridx, dbnumber);

			if (bError == FALSE)
			{
				pMsg.btResult = 13;
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
				return;
			}

			if (useridx > 0)
			{
				pMsg.btResult = 0;
				std::memcpy(pMsg.Name, Obj.Name, sizeof(pMsg.Name));
				IOCP.DataSend(useridx, (BYTE*)&pMsg, pMsg.h.size);

				sLog->outBasic("[DSF][CGReqDSFEnter][%s][%s][%s][%d] Request DSF Enter Success And Ask To Party Member, DSFType : %d",
					Obj.AccountID, Obj.m_PlayerData->m_RealNameOfUBF,
					Obj.Name, Obj.m_PlayerData->m_nServerCodeOfHomeWorld,
					g_DevilSquareFinal.m_nDSF_TYPE);
			}
		}
	}

	else if (g_DevilSquareFinal.m_nDSF_TYPE == 2)
	{
		int dbnumber = 0;
		int useridx = -1;
		WORD wServerCode1 = -1;
		WORD wServerCode2 = -1;
		char szUserName1[MAX_ACCOUNT_LEN + 1];
		char szUserName2[MAX_ACCOUNT_LEN + 1];

		for (int i = 0; i < 2; i++)
		{
			BOOL bError = gParty.GetIndexUser(Obj.PartyNumber, i, *getGameObject(useridx), dbnumber);

			if (bError == FALSE)
			{
				pMsg.btResult = 13;
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
				return;
			}

			if (useridx > 0)
			{
				if (i == 0)
				{
					std::memcpy(szUserName1, getGameObject(useridx)->m_PlayerData->m_NameConvertOfUBF.ConvertNameToRealName(getGameObject(useridx)->Name), MAX_ACCOUNT_LEN + 1);
					wServerCode1 = getGameObject(useridx)->m_PlayerData->m_NameConvertOfUBF.FindServerCodeAtCharacterName(getGameObject(useridx)->Name);
				}

				else if (i == 1)
				{
					std::memcpy(szUserName2, getGameObject(useridx)->m_PlayerData->m_NameConvertOfUBF.ConvertNameToRealName(getGameObject(useridx)->Name), MAX_ACCOUNT_LEN + 1);
					wServerCode2 = getGameObject(useridx)->m_PlayerData->m_NameConvertOfUBF.FindServerCodeAtCharacterName(getGameObject(useridx)->Name);
				}
			}
		}

		BOOL bGoFinal = g_DevilSquareFinal.IsGoSemiFinalParty(szUserName1, szUserName2, wServerCode1, wServerCode2);

		if (bGoFinal == FALSE)
		{
			pMsg.btResult = 18;
			std::memcpy(pMsg.Name, Obj.Name, sizeof(pMsg.Name));
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

			return;
		}

		BYTE btSlotNum = -1;
		BYTE btRet = g_DevilSquareFinal.Find_EmptyPartySlot(Obj.PartyNumber, btSlotNum);

		if (btRet >= 0xFE)
		{
			pMsg.btResult = 14;
			std::memcpy(pMsg.Name, Obj.Name, sizeof(pMsg.Name));
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

			return;
		}

		BOOL bUpdateOk = g_DevilSquareFinal.SetParty(btSlotNum, 2, Obj.PartyNumber, iIndex);

		if (bUpdateOk != FALSE)
		{
			sLog->outBasic("[DSF][CGReqDSFEnter][%s][%s][%s][%d] ERROR code:%d",
				Obj.AccountID, Obj.m_PlayerData->m_RealNameOfUBF, Obj.Name, Obj.m_PlayerData->m_nServerCodeOfHomeWorld, bUpdateOk);

			pMsg.btResult = -1;
			std::memcpy(pMsg.Name, Obj.Name, sizeof(pMsg.Name));
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

			return;
		}

		for (int i = 0; i < 2; i++)
		{
			BOOL bError = gParty.GetIndexUser(Obj.PartyNumber, i, *getGameObject(useridx), dbnumber);

			if (bError == FALSE)
			{
				pMsg.btResult = 13;
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
				return;
			}

			if (useridx > 0)
			{
				pMsg.btResult = 0;
				std::memcpy(pMsg.Name, Obj.Name, sizeof(pMsg.Name));
				IOCP.DataSend(useridx, (BYTE*)&pMsg, pMsg.h.size);

				sLog->outBasic("[DSF][CGReqDSFEnter][%s][%s][%s][%d] Request DSF Enter Success And Ask To Party Member, DSFType : %d",
					Obj.AccountID, Obj.m_PlayerData->m_RealNameOfUBF,
					Obj.Name, Obj.m_PlayerData->m_nServerCodeOfHomeWorld,
					g_DevilSquareFinal.m_nDSF_TYPE);
			}
		}
	}

	else if (g_DevilSquareFinal.m_nDSF_TYPE == 3)
	{
		int dbnumber = 0;
		int useridx = -1;
		WORD wServerCode1 = -1;
		WORD wServerCode2 = -1;
		char szUserName1[MAX_ACCOUNT_LEN + 1];
		char szUserName2[MAX_ACCOUNT_LEN + 1];

		for (int i = 0; i < 2; i++)
		{
			BOOL bError = gParty.GetIndexUser(Obj.PartyNumber, i, getGameObject(useridx), dbnumber);

			if (bError == FALSE)
			{
				pMsg.btResult = 13;
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
				return;
			}

			if (useridx > 0)
			{
				if (i == 0)
				{
					std::memcpy(szUserName1, getGameObject(useridx)->m_PlayerData->m_NameConvertOfUBF.ConvertNameToRealName(getGameObject(useridx)->Name), MAX_ACCOUNT_LEN + 1);
					wServerCode1 = getGameObject(useridx)->m_PlayerData->m_NameConvertOfUBF.FindServerCodeAtCharacterName(getGameObject(useridx)->Name);
				}

				else if (i == 1)
				{
					std::memcpy(szUserName2, getGameObject(useridx)->m_PlayerData->m_NameConvertOfUBF.ConvertNameToRealName(getGameObject(useridx)->Name), MAX_ACCOUNT_LEN + 1);
					wServerCode2 = getGameObject(useridx)->m_PlayerData->m_NameConvertOfUBF.FindServerCodeAtCharacterName(getGameObject(useridx)->Name);
				}
			}
		}

		BOOL bGoFinal = g_DevilSquareFinal.IsGoFinalParty(szUserName1, szUserName2, wServerCode1, wServerCode2);

		if (bGoFinal == FALSE)
		{
			pMsg.btResult = 18;
			std::memcpy(pMsg.Name, Obj.Name, sizeof(pMsg.Name));
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

			return;
		}

		BYTE btSlotNum = -1;
		BYTE btRet = g_DevilSquareFinal.Find_EmptyPartySlot(Obj.PartyNumber, btSlotNum);

		if (btRet >= 0xFE)
		{
			pMsg.btResult = 14;
			std::memcpy(pMsg.Name, Obj.Name, sizeof(pMsg.Name));
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

			return;
		}

		BOOL bUpdateOk = g_DevilSquareFinal.SetParty(btSlotNum, 3, Obj.PartyNumber, iIndex);

		if (bUpdateOk != FALSE)
		{
			sLog->outBasic("[DSF][CGReqDSFEnter][%s][%s][%s][%d] ERROR code:%d",
				Obj.AccountID, Obj.m_PlayerData->m_RealNameOfUBF, Obj.Name, Obj.m_PlayerData->m_nServerCodeOfHomeWorld, bUpdateOk);

			pMsg.btResult = -1;
			std::memcpy(pMsg.Name, Obj.Name, sizeof(pMsg.Name));
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

			return;
		}

		for (int i = 0; i < 2; i++)
		{
			BOOL bError = gParty.GetIndexUser(Obj.PartyNumber, i, useridx, dbnumber);

			if (bError == FALSE)
			{
				pMsg.btResult = 13;
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
				return;
			}

			if (useridx > 0)
			{
				pMsg.btResult = 0;
				std::memcpy(pMsg.Name, Obj.Name, sizeof(pMsg.Name));
				IOCP.DataSend(useridx, (BYTE*)&pMsg, pMsg.h.size);

				sLog->outBasic("[DSF][CGReqDSFEnter][%s][%s][%s][%d] Request DSF Enter Success And Ask To Party Member, DSFType : %d",
					Obj.AccountID, Obj.m_PlayerData->m_RealNameOfUBF,
					Obj.Name, Obj.m_PlayerData->m_nServerCodeOfHomeWorld,
					g_DevilSquareFinal.m_nDSF_TYPE);
			}
		}
	}
}

int GameProtocol::EnterDSFCheckUser(CGameObject &Obj, int & TroubleUserIndex)
{
	int useridx = -1;
	int dbnumber = 0;
	int nLorenMarketUserCount = 0;

	for (int i = 0; i < 2; i++)
	{
		BOOL bError = gParty.GetIndexUser(Obj.PartyNumber, i, useridx, dbnumber);
		TroubleUserIndex = useridx;

		if (bError == FALSE)
		{
			return 13;
		}

		if (useridx > 0)
		{
			if (getGameObject(useridx)->MapNumber == MAP_INDEX_LORENMARKET || getGameObject(useridx)->MapNumber == MAP_INDEX_DEVILSQUARE_FINAL)
			{
				nLorenMarketUserCount++;
			}

			if (getGameObject(useridx)->PartyNumber != Obj.PartyNumber)
			{
				return 13;
			}

			if (getGameObject(useridx)->Level < g_DevilSquareFinal.m_DSFUnitInfo.DSFMinLevel)
			{
				return 5;
			}

			if (g_DevilSquareFinal.CheckWearingMOPH(*getGameObject(useridx)) == TRUE)
			{
				return 7;
			}

			if (getGameObject(useridx)->m_IfState->use && getGameObject(useridx)->m_IfState->type != 12)
			{
				return 11;
			}

			if (getGameObject(useridx)->m_PK_Level >= 4)
			{
				return 12;
			}

			if (gObjDuelCheck(*getGameObject(useridx)) == TRUE)
			{
				return 15;
			}

			if (getGameObject(useridx)->m_PlayerData->lpGuild != NULL &&
				getGameObject(useridx)->m_PlayerData->lpGuild->WarState == TRUE)
			{
				return 16;
			}
		}
	}

	return 0;
}

void GameProtocol::CGReqAcceptEnterDSF(PMSG_REQ_DSF_ACCEPTENTER *lpMsg, CGameObject &Obj)
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (!gObjIsConnected(Obj))
	{
		return;
	}

	PMSG_ANS_DSF_ACCEPTENTER pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xDB, 0x02, sizeof(pMsg));
	std::memset(pMsg.Name, 0x00, sizeof(pMsg.Name));

	int useridx = -1, dbnumber = 0;

	if (gParty.IsParty(Obj.PartyNumber) == FALSE)
	{
		sLog->outBasic("[DSF][CGReqAcceptEnterDSF][%s][%s][%s][%d] reqman is not party Error",
			Obj.AccountID, Obj.m_PlayerData->m_RealNameOfUBF,
			Obj.Name, Obj.m_PlayerData->m_nServerCodeOfHomeWorld);

		g_DevilSquareFinal.DeleteParty(Obj.PartyNumber);
		gParty.ClearPartyAuthDSF(Obj.PartyNumber);
		return;
	}

	if (lpMsg->btEnterOK == TRUE)
	{
		sLog->outBasic("[DSF][CGReqAcceptEnterDSF][%s][%s][%s][%d] Enter Cancel...",
			Obj.AccountID, Obj.m_PlayerData->m_RealNameOfUBF,
			Obj.Name, Obj.m_PlayerData->m_nServerCodeOfHomeWorld);

		g_DevilSquareFinal.DeleteParty(Obj.PartyNumber);
		gParty.ClearPartyAuthDSF(Obj.PartyNumber);

		pMsg.btError = 24;
		std::memcpy(pMsg.Name, Obj.Name, MAX_ACCOUNT_LEN + 1);

		for (int i = 0; i < 2; i++)
		{
			gParty.GetIndexUser(Obj.PartyNumber, i, useridx, dbnumber);

			if (useridx > 0)
			{
				IOCP.DataSend(useridx, (BYTE*)&pMsg, pMsg.h.size);
			}
		}

		return;
	}

	sLog->outBasic("[DSF][CGReqAcceptEnterDSF][%s][%s][%s][%d] Enter OK...",
		Obj.AccountID, Obj.m_PlayerData->m_RealNameOfUBF,
		Obj.Name, Obj.m_PlayerData->m_nServerCodeOfHomeWorld);

	if (gParty.EnterDSF_PartyAuth(Obj.PartyNumber, iIndex) == FALSE)
	{
		sLog->outBasic("[DSF][CGReqAcceptEnterDSF][%s][%s][%s][%d] Internal Error!!!",
			Obj.AccountID, Obj.m_PlayerData->m_RealNameOfUBF,
			Obj.Name, Obj.m_PlayerData->m_nServerCodeOfHomeWorld);

		pMsg.btError = -1;
		std::memcpy(pMsg.Name, Obj.Name, sizeof(pMsg.Name));
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (gParty.AllAgreeEnterDSF(Obj.PartyNumber) == FALSE)
	{
		int AgreeCnt = gParty.DSFReturnOKCount(Obj.PartyNumber);

		sLog->outBasic("[DSF[CGReqAcceptEnterDSF][%s][%s][%s][%d] Party Enter Agree AgreeCnt:%d",
			Obj.AccountID, Obj.m_PlayerData->m_RealNameOfUBF,
			Obj.Name, Obj.m_PlayerData->m_nServerCodeOfHomeWorld, AgreeCnt);

		return;
	}

	int TroubleUserIndex = -1;
	BOOL bResult = this->EnterDSFCheckUser(iIndex, TroubleUserIndex);

	if (bResult != FALSE)
	{
		pMsg.btError = bResult;
		std::memcpy(pMsg.Name, getGameObject(TroubleUserIndex)->Name, sizeof(pMsg.Name));

		int nAllPartyUserIndex;

		for (int i = 0; i < 2; i++)
		{
			gParty.GetIndexUser(Obj.PartyNumber, i, nAllPartyUserIndex, dbnumber);
			IOCP.DataSend(nAllPartyUserIndex, (BYTE*)&pMsg, pMsg.h.size);
		}

		g_DevilSquareFinal.DeleteParty(Obj.PartyNumber);
		gParty.ClearPartyAuthDSF(Obj.PartyNumber);
		return;
	}

	for (int i = 0; i < 2; i++)
	{
		gParty.GetIndexUser(Obj.PartyNumber, i, useridx, dbnumber);
		CGameObject* lpObjPartyUser = getGameObject(useridx);
		if (useridx != -1)
		{
			BYTE btSlotNum = -1;
			BOOL bReturn = g_DevilSquareFinal.Find_PartySlot(Obj.PartyNumber, btSlotNum);

			if (bReturn != FALSE)
			{
				sLog->outBasic("[DSF][Find_ExistingPartyInfo][%s][%s][%s][%d] ERROR bReturn:%d, mapnumber:%d, btSlotNum:%d ",
					Obj.AccountID, Obj.m_PlayerData->m_RealNameOfUBF, Obj.Name, Obj.m_PlayerData->m_nServerCodeOfHomeWorld,
					bReturn, Obj.MapNumber, btSlotNum);

				return;
			}

			BOOL bEnterOK = g_DevilSquareFinal.Enter_DSF(*lpObjPartyUser, btSlotNum);

			if (bEnterOK == FALSE)
			{
				g_DevilSquareFinal.DeleteParty(Obj.PartyNumber);

				sLog->outBasic("[DSF][CGReqAcceptEnterDSF][%s][%s][%s][%d] Enter Error bReturn:%d, mapnumber:%d, btSlotNum:%d ",
					Obj.AccountID, Obj.m_PlayerData->m_RealNameOfUBF, Obj.Name, Obj.m_PlayerData->m_nServerCodeOfHomeWorld,
					bReturn, Obj.MapNumber, btSlotNum);
			}

			else
			{
				pMsg.btError = 0;
				std::memcpy(pMsg.Name, lpObjPartyUser->Name, sizeof(pMsg.Name));
				IOCP.DataSend(*lpObjPartyUser, (BYTE*)&pMsg, pMsg.h.size);

				lpObjPartyUser->MapNumber = MAP_INDEX_DEVILSQUARE_FINAL;
				gObjMoveGate(*lpObjPartyUser, 474);

				sLog->outBasic("[DSF][CGReqAcceptEnterDSF][%s][%s][%s][%d] Enter DSF - PartyNo:%d, btSlotNum:%d ",
					lpObjPartyUser->AccountID, lpObjPartyUser->m_PlayerData->m_RealNameOfUBF,
					lpObjPartyUser->Name, lpObjPartyUser->m_PlayerData->m_nServerCodeOfHomeWorld,
					lpObjPartyUser->PartyNumber, btSlotNum);
			}
		}
	}
}

void GameProtocol::CGReqDSFGoFinalParty(PMSG_REQ_DSF_GO_FINAL_PARTY_INFO *lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj))
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (!gObjIsConnected(Obj))
	{
		return;
	}

	if (g_ConfigRead.server.GetServerType() != SERVER_BATTLECORE)
	{
		return;
	}

	BYTE sendBuf[4096];
	std::memset(sendBuf, 0x00, sizeof(sendBuf));

	PMSG_ANS_DSF_GO_FINAL_PARTY_INFO pMsg;
	DSF_GO_FINAL_PARTY pParty;

	int iSize = sizeof(PMSG_ANS_DSF_GO_FINAL_PARTY_INFO);
	int iCount = 0;

	if (lpMsg->btDSFType == 2)
	{
		for (int i = 0; i < 40; i++)
		{
			if (g_DevilSquareFinal.m_GoSemiFinalParty[i].btRank > 0)
			{
				std::memcpy(pParty.szUserName1, g_DevilSquareFinal.m_GoSemiFinalParty[i].szUserName1, MAX_ACCOUNT_LEN + 1);
				std::memcpy(pParty.szUserName2, g_DevilSquareFinal.m_GoSemiFinalParty[i].szUserName2, MAX_ACCOUNT_LEN + 1);
				pParty.btRank = g_DevilSquareFinal.m_GoSemiFinalParty[i].btRank;
				pParty.btDSFType = g_DevilSquareFinal.m_GoSemiFinalParty[i].btDSFType;
				pParty.iPoint = g_DevilSquareFinal.m_GoSemiFinalParty[i].iPoint;

				std::memcpy(&sendBuf[iSize], &pParty, sizeof(pParty));
				iSize += sizeof(pParty);
				iCount++;
			}
		}
	}

	if (lpMsg->btDSFType == 3)
	{
		for (int i = 0; i < 10; i++)
		{
			if (g_DevilSquareFinal.m_GoFinalParty[i].btRank > 0)
			{
				std::memcpy(pParty.szUserName1, g_DevilSquareFinal.m_GoFinalParty[i].szUserName1, MAX_ACCOUNT_LEN + 1);
				std::memcpy(pParty.szUserName2, g_DevilSquareFinal.m_GoFinalParty[i].szUserName2, MAX_ACCOUNT_LEN + 1);
				pParty.btRank = g_DevilSquareFinal.m_GoFinalParty[i].btRank;
				pParty.btDSFType = g_DevilSquareFinal.m_GoFinalParty[i].btDSFType;
				pParty.iPoint = g_DevilSquareFinal.m_GoFinalParty[i].iPoint;

				std::memcpy(&sendBuf[iSize], &pParty, sizeof(pParty));
				iSize += sizeof(pParty);
				iCount++;
			}
		}
	}

	if (lpMsg->btDSFType == 4)
	{
		if (g_DevilSquareFinal.m_WinnerParty.btRank > 0)
		{
			std::memcpy(pParty.szUserName1, g_DevilSquareFinal.m_WinnerParty.szUserName1, MAX_ACCOUNT_LEN + 1);
			std::memcpy(pParty.szUserName2, g_DevilSquareFinal.m_WinnerParty.szUserName2, MAX_ACCOUNT_LEN + 1);
			pParty.btRank = g_DevilSquareFinal.m_WinnerParty.btRank;
			pParty.btDSFType = g_DevilSquareFinal.m_WinnerParty.btDSFType;
			pParty.iPoint = g_DevilSquareFinal.m_WinnerParty.iPoint;

			std::memcpy(&sendBuf[iSize], &pParty, sizeof(pParty));
			iSize += sizeof(pParty);
			iCount++;
		}
	}

	PHeadSubSetW((BYTE*)&pMsg, 0xDB, 0x03, iSize);
	pMsg.btCnt = iCount;
	pMsg.btDSFType = lpMsg->btDSFType;

	if (iCount == 0)
	{
		pMsg.btResult = 100;
	}

	else
	{
		pMsg.btResult = 0;
	}

	std::memcpy(&sendBuf, &pMsg, sizeof(pMsg));
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, sendBuf, iSize);

	sLog->outBasic("[DSF][CGReqDSFGoFinalParty][%s][%s][%s][%d] ReqType:%d, ResultCount:%d",
		Obj.AccountID, Obj.m_PlayerData->m_RealNameOfUBF,
		Obj.Name, Obj.m_PlayerData->m_nServerCodeOfHomeWorld,
		lpMsg->btDSFType, iCount);
}

void GameProtocol::CGReqDSFGetReward(PMSG_REQ_DSF_GET_REWARD *lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj))
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (!gObjIsConnected(Obj))
	{
		return;
	}

	if (g_ConfigRead.server.GetServerType() != SERVER_BATTLECORE)
	{
		return;
	}

	if (lpMsg->btDSFType == 4)
	{
		g_DevilSquareFinal.GDReqGetReward(Obj);
	}
}

void GameProtocol::CGReqDoUsePopupType(CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj))
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (!gObjIsConnected(&Obj))
	{
		return;
	}

	PMSG_ANS_USE_POPUP pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x26, sizeof(pMsg));

	pMsg.nPopupType = g_UnityBattleField.GetUBFPopupType();
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCSendEventBanner(CGameObject &Obj, int iBannerType)
{
	PMSG_SEND_EVENT_BANNER pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0x4D, 0x18, sizeof(pMsg));

	pMsg.iBannerType = iBannerType;
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCAlterPShopVault(CGameObject &Obj, BYTE btValue)
{
	PMSG_ALTERPSHOPVAULT pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xFA, 0xA8, sizeof(pMsg));
	pMsg.type = btValue;
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}
void GameProtocol::GCSetCharColors(CGameObject &Obj)
{
	PMSG_SET_CHAT_COLOR pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xFA, 0x12, sizeof(pMsg));

	std::memcpy(pMsg.btInfoMsg, g_ConfigRead.data.chatcolor.btInfoMsg, sizeof(pMsg.btInfoMsg));
	std::memcpy(pMsg.btErrorMsg, g_ConfigRead.data.chatcolor.btErrorMsg, sizeof(pMsg.btErrorMsg));
	std::memcpy(pMsg.btChatMsg, g_ConfigRead.data.chatcolor.btChatMsg, sizeof(pMsg.btChatMsg));
	std::memcpy(pMsg.btWhisperMsg, g_ConfigRead.data.chatcolor.btWhisperMsg, sizeof(pMsg.btWhisperMsg));
	std::memcpy(pMsg.btPartyMsg, g_ConfigRead.data.chatcolor.btPartyMsg, sizeof(pMsg.btPartyMsg));
	std::memcpy(pMsg.btGuildMsg, g_ConfigRead.data.chatcolor.btGuildMsg, sizeof(pMsg.btGuildMsg));
	std::memcpy(pMsg.btAllianceMsg, g_ConfigRead.data.chatcolor.btAllianceMsg, sizeof(pMsg.btAllianceMsg));
	std::memcpy(pMsg.btGensMsg, g_ConfigRead.data.chatcolor.btGensMsg, sizeof(pMsg.btGensMsg));
	std::memcpy(pMsg.btGMChatMsg, g_ConfigRead.data.chatcolor.btGMChatMsg, sizeof(pMsg.btGMChatMsg));

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::GCSendRuud(CGameObject &Obj, int iRuud, int iObtainedRuud, bool bIsObtainedRuud)
{
	PMSG_RUUD pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xD0, 0xF1, sizeof(pMsg));

	pMsg.iTotalRuud = iRuud;
	pMsg.iObtainedRuud = iObtainedRuud;
	pMsg.btObtainNotiSend = bIsObtainedRuud;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}

void GameProtocol::CGReqUseBoxInInventory(CGameObject &Obj, PMSG_REQ_USE_BOX * aRecv)
{
	if (!ObjectMaxRange(Obj))
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (!gObjIsConnected(&Obj))
	{
		return;
	}

	if (Obj.m_IfState->use != 0)
	{
		return;
	}

	if (Obj.m_PlayerData->m_bSecurityCheck == false)
	{
		return;
	}

	if (Obj.CloseType != -1)
	{
		return;
	}

	if (Obj.m_bMapSvrMoveQuit == true)
	{
		sLog->outBasic("[ANTI-HACK][Item Duplication] - Use Box during MapServer Move [%s][%s]", Obj.AccountID, Obj.Name);
		return;
	}

	PMSG_ANS_USE_BOX pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xD0, 0xF2, sizeof(pMsg));

	if (CheckInventoryEmptySpace(&Obj, 4, 4) == FALSE)
	{
		pMsg.iResult = -2;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (Obj.m_PlayerData->Money >= 1900000000)
	{
		pMsg.iResult = -3;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	CItemObject Item;
	time_t Duration;
	int iResult = 0;
	int iBoxType = Obj.pntInventory[aRecv->btPos]->m_Type;

	switch (iBoxType)
	{
	case ITEMGET(14, 282):
		iResult = g_BagManager.GetItemFromBag(Obj, BAG_EVENT, EVENTBAG_MASTERYBOX_MINOR, Obj, Item, Duration);
		break;
	case ITEMGET(14, 283):
		iResult = g_BagManager.GetItemFromBag(Obj, BAG_EVENT, EVENTBAG_MASTERYBOX_STANDARD, Obj, Item, Duration);
		break;
	case ITEMGET(14, 284):
		iResult = g_BagManager.GetItemFromBag(Obj, BAG_EVENT, EVENTBAG_MASTERYBOX_GREATER, Obj, Item, Duration);
		break;
	case ITEMGET(14, 287):
		iResult = g_BagManager.GetItemFromBag(Obj, BAG_EVENT, EVENTBAG_MASTERYCHEST_2000, Obj, Item, Duration);
		break;
	case ITEMGET(14, 288):
		iResult = g_BagManager.GetItemFromBag(Obj, BAG_EVENT, EVENTBAG_MASTERYCHEST_1000, Obj, Item, Duration);
		break;
	case ITEMGET(14, 289):
		iResult = g_BagManager.GetItemFromBag(Obj, BAG_EVENT, EVENTBAG_MASTERYCHEST_500, Obj, Item, Duration);
		break;
	case ITEMGET(14, 336):
		iResult = g_BagManager.GetItemFromBag(Obj, BAG_EVENT, EVENTBAG_MASTERYCHEST_100, Obj, Item, Duration);
		break;
	case ITEMGET(14, 293):
		iResult = g_BagManager.GetItemFromBag(Obj, BAG_EVENT, EVENTBAG_ELEMENTALCAPSULE_NORMAL, Obj, Item, Duration);
		break;
	case ITEMGET(14, 384):
		iResult = g_BagManager.GetItemFromBag(Obj, BAG_EVENT, EVENTBAG_RUNECAPSULE, Obj, Item, Duration);
		break;/*
	case ITEMGET(14, 367):
		iResult = g_BagManager.GetItemFromBag(Obj, BAG_EVENT, EVENTBAG_ROOMY_CARD_LOWER, Obj, Item, Duration);
		break;
	case ITEMGET(14, 368):
		iResult = g_BagManager.GetItemFromBag(Obj, BAG_EVENT, EVENTBAG_ROOMY_CARD_MEDIUM, Obj, Item, Duration);
		break;
	case ITEMGET(14, 369):
		iResult = g_BagManager.GetItemFromBag(Obj, BAG_EVENT, EVENTBAG_ROOMY_CARD_HIGHER, Obj, Item, Duration);
		break;
	case ITEMGET(14, 296):
		iResult = g_BagManager.GetItemFromBag(Obj, BAG_EVENT, EVENTBAG_ELEMENTALCAPSULE_NORMAL, Obj, Item, Duration);
		break;
	case ITEMGET(14, 338):
		iResult = g_BagManager.GetItemFromBag(Obj, BAG_EVENT, EVENTBAG_RUNECAPSULE, Obj, Item, Duration);
		break;
	case ITEMGET(14, 337):
		iResult = g_BagManager.GetItemFromBag(Obj, BAG_EVENT, EVENTBAG_ROOMY_CARD_LOWER, Obj, Item, Duration);
		break;
	case ITEMGET(14, 339):
		iResult = g_BagManager.GetItemFromBag(Obj, BAG_EVENT, EVENTBAG_ROOMY_CARD_MEDIUM, Obj, Item, Duration);
		break;
	case ITEMGET(14, 340):
		iResult = g_BagManager.GetItemFromBag(Obj, BAG_EVENT, EVENTBAG_ROOMY_CARD_HIGHER, Obj, Item, Duration);
		break;*/
	case ITEMGET(13, 350):
		iResult = g_BagManager.GetItemFromBag(Obj, BAG_EVENT, EVENTBAG_MUUNEGG, Obj, Item, Duration);
		break;		
	default:
		iResult = 0;
		break;
	}

	if (iResult == 0)
	{
		pMsg.iResult = -1;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	gObjInventoryDeleteItem(Obj, aRecv->btPos);
	GCInventoryItemDeleteSend(Obj, aRecv->btPos, 0);

	if (iResult == 2 || iResult == 3)
	{
		pMsg.iResult = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	pMsg.iResult = 2;
	pMsg.iItemType = Item.m_Type;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	ItemCreate(Obj, 235, 0, 0, Item.m_Type, Item.m_Level, Item.m_Durability, Item.m_Option1, Item.m_Option2, Item.m_Option3, Obj, Item.m_NewOption, Item.m_SetOption, Duration, Item.m_SocketOption, Item.m_BonusSocketOption);
}

void GameProtocol::CGRecvHitHackValues(CGameObject &Obj, PMSG_SEND_HITHACK_INFO * lpMsg)
{
	if (!g_ConfigRead.data.common.bHitHackDetection)
	{
		return;
	}

	if (!ObjectMaxRange(Obj))
	{
		return;
	}

	if (!gObjIsConnectedGP(Obj))
	{
		return;
	}

	bool bMismatchFound = false;
	bool bIsAllValues1Equal = false;;
	bool bIsAllValues2Equal = false;;
	

	if (Obj.m_PlayerData->Dexterity > g_ConfigRead.data.common.iHitHackMaxAgility)
	{
		Obj.m_PlayerData->dwLastHitHackTick = GetTickCount64();
		return;
	}

	DWORD dwCount = Obj.m_PlayerData->dwHitHackCounter1;

	if ((dwCount + 1) % 7 != 0)
	{
		Obj.m_PlayerData->dwLastHitHackValue1[dwCount] = lpMsg->dwHitHackVal1;
		Obj.m_PlayerData->dwLastHitHackValue2[dwCount] = lpMsg->dwHitHackVal2;
		Obj.m_PlayerData->dwHitHackCounter1++;
	}
	else
	{
		Obj.m_PlayerData->dwHitHackCounter1 = 0;
		dwCount = 0;

		Obj.m_PlayerData->dwLastHitHackValue1[6] = lpMsg->dwHitHackVal1;
		Obj.m_PlayerData->dwLastHitHackValue2[6] = lpMsg->dwHitHackVal2;

	}

	for (int i = 0; i < 7 - 1; i++)
	{
		if (Obj.m_PlayerData->dwLastHitHackValue1[i] != Obj.m_PlayerData->dwLastHitHackValue1[i + 1])
		{
			bMismatchFound = true;
		}
	}

	if (!bMismatchFound)
	{
		if (Obj.m_PlayerData->dwLastHitHackValue1[0] != 0)
		{
			bIsAllValues1Equal = true;
		}
	}

	bMismatchFound = false;

	for (int i = 0; i < 7 - 1; i++)
	{
		if (Obj.m_PlayerData->dwLastHitHackValue2[i] != Obj.m_PlayerData->dwLastHitHackValue2[i + 1])
		{
			bMismatchFound = true;
		}
	}

	if (!bMismatchFound)
	{
		if (Obj.m_PlayerData->dwLastHitHackValue2[0] = !0)
		{
			bIsAllValues2Equal = true;
		}
	}

	if (bIsAllValues1Equal || bIsAllValues2Equal)
	{
		sLog->outError( "[AntiHack][%s][%s][%s] Hit Hack detected", Obj.m_PlayerData->ConnectUser->IP, Obj.AccountID, Obj.Name);
		AntiHackLog->Output("[AntiHack][%s][%s][%s] Hit Hack detected", Obj.m_PlayerData->ConnectUser->IP, Obj.AccountID, Obj.Name);
		GCSendDisableReconnect(Obj);
	}

	Obj.m_PlayerData->dwLastHitHackTick = GetTickCount64();

}

// event entry notify

void GameProtocol::CGEventEntryNotice(int iEventType, BYTE state)
{
	if (iEventType >= EVENT_NOTIFICATION::EVENT_NOTIFICATION_MAX)
	{
		return;
	}

	PMSG_EVENTENTRY_NOTICE pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xF1, 0xFE, sizeof(PMSG_EVENTENTRY_NOTICE));

	pMsg.btEventType = iEventType;
	pMsg.btNoticeState = state;

	for (int n = g_ConfigRead.server.GetObjectStartUserIndex(); n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		if (getGameObject(n)->Connected == PLAYER_PLAYING)
		{
			if (getGameObject(n)->Type == OBJ_USER && getGameObject(n)->EventNotification[iEventType] != state)
			{
				getGameObject(n)->EventNotification[iEventType] = state;
				IOCP.DataSend(n, (BYTE*)&pMsg, pMsg.h.size);
			}
		}
	}
}

void GameProtocol::CGReqNixieBossEnter(PMSG_REQ_NIXIELAKE_ENTER *lpMsg, CGameObject &Obj)
{
	

	if (ObjectMaxRange(Obj) == false)
	{
		return;
	}

	if (g_ConfigRead.NixieLakeEntrance == false)
	{
		MsgOutput(Obj, "Nix Event is Closed");
		return;
	}

	if (gObjIsConnected(Obj) == false)
	{
		return;
	}

	PMSG_ANS_NIXIELAKE_ENTER pMsg;

	for (int pos = INVETORY_WEAR_SIZE; pos < MAIN_INVENTORY_SIZE; pos++)
	{
		if (Obj.pntInventory[pos]->IsItem() && Obj.pntInventory[pos]->m_Type == ITEMGET(14, 390) && Obj.pntInventory[pos]->m_Durability == 5.0f)
		{
			gObjInventoryDeleteItem(iIndex, pos);
			this->GCInventoryItemDeleteSend(iIndex, pos, 1);
			gObjMoveGate(iIndex, 531);
			sLog->outBasic("[%s][%s] Entered to Nixie Boss Map", Obj.AccountID, Obj.Name);
			return;
		}
	}

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x76;
	pMsg.h.subcode = 0x02;
	pMsg.btResult = 1;
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));

}

void GameProtocol::GCPlayerStatsPanelNew(CGameObject &Obj)
{
	PMSG_ADDSTATS_RESULT pStatsInfo;
	PHeadSubSetB((BYTE*)&pStatsInfo, 0xEC, 0x25, sizeof(pStatsInfo));
	pStatsInfo.str = Obj.m_PlayerData->Strength;
	pStatsInfo.stradd = Obj.AddStrength;
	pStatsInfo.agi = Obj.m_PlayerData->Dexterity;
	pStatsInfo.agiadd = Obj.AddDexterity;
	pStatsInfo.vit = Obj.m_PlayerData->Vitality;
	pStatsInfo.vitadd = Obj.AddVitality;
	pStatsInfo.ene = Obj.m_PlayerData->Energy;
	pStatsInfo.eneadd = Obj.AddEnergy;
	pStatsInfo.cmd = Obj.Leadership;
	pStatsInfo.cmdadd = Obj.AddLeadership;
	pStatsInfo.mPrec = Obj.m_PlayerData->m_MPSkillOpt->iMpsAutoRecoverMana + Obj.m_PlayerData->m_WingExcOption->iWingOpRecoveryMana + Obj.m_PlayerData->m_JewelOfHarmonyEffect->HJOpAddRefillMP + Obj.m_PlayerData->m_btRefillMPSocketOption;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pStatsInfo, sizeof(pStatsInfo));
}

// NUEVO STATS PANEL POR %
struct SendPlayerInfoNewPanel
{
	PBMSG_HEAD2 h;
	DWORD unk;
	DWORD unk1;
	WORD Strength;
	WORD AddSetAccStrength;
	WORD Agility;
	WORD AddSetAccAgility;
	WORD Stamina;
	WORD AddSetAccStamina;
	WORD Energy;
	WORD AddSetAccEnergy;
	WORD Command;
	WORD AddSetAccCommand;
	WORD CritDMGInc;
	WORD ExcDMBInc;
	WORD SkillAtkDMGInc;
	WORD BasicDef;
	WORD SDWhenAtckd;
	WORD SDIgnoreRate;
	WORD SDWhenAtk;
	WORD ZenObincRate;
	float DefIgnRate;
	float HPRecRate;
	float MPRecRate;
	float StunRate;
	float StunResRate;
	BYTE Desconosido;
	float ShieldAbsorb;
	float MobAtkHPRec;
	float MobAtkMPRec;
	float MobAtkSDRec;
	float SDRecRate;
	float ResAllMPRate;
	float ResAllHPRate;
	float RessAllSDRate;
	float AGRecRate;
	float DMGAbsorbRate;
	float ShieldBlockRate;
	float WeaponBlockRate;
	float HPAbsorb;
	float SDAbsorb;
	float ReturnDmgRate;
	float DmgRedcRate;
	float MPUseRedcRate;
	float CritDMGRate;
	float ExcDMGRate;
	float DoubleDmgRate;
	BYTE DMGRedcRate;
	BYTE AGUsageRedRate;
	BYTE DMGReflcRate;
};

void GameProtocol::GCPlayerStatsPanelRates(CGameObject &Obj)
{

	PMSG_STATS_RESULT pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xEC, 0x29, sizeof(pMsg));
	pMsg.CriticalDMGInc = Obj.m_PlayerData->m_MPSkillOpt->iMpsIncCriticalDamageRate + Obj.m_PlayerData->SetOpAddCriticalDamageSuccessRate + Obj.m_PlayerData->SetOpAddCriticalDamage + Obj.m_PlayerData->m_JewelOfHarmonyEffect->HJOpAddCriticalDamage; // crit dmg inc
	pMsg.unk1 = Obj.m_PlayerData->m_MPSkillOpt->iMpsAddPhysicDamage + Obj.m_PlayerData->m_MPSkillOpt->iMpsElfAddPhysicDamage + Obj.m_PlayerData->m_JewelOfHarmonyEffect->HJOpAddSkillAttack + Obj.m_PlayerData->SetOpAddSkillAttack; // skill attack dmg inc
	pMsg.str = Obj.m_PlayerData->Strength;
	pMsg.stradd = Obj.AddStrength;
	pMsg.agi = Obj.m_PlayerData->Dexterity;
	pMsg.agiadd = Obj.AddDexterity;
	pMsg.vit = Obj.m_PlayerData->Vitality;
	pMsg.vitadd = Obj.AddVitality;
	pMsg.ene = Obj.m_PlayerData->Energy;
	pMsg.eneadd = Obj.AddEnergy;
	pMsg.cmd = Obj.Leadership;
	pMsg.cmdadd = Obj.AddLeadership;
	pMsg.ExcellentDMGInc = 27; // 
	pMsg.ExcellentDMGInc2 = 37; // 
	pMsg.SDRation = Obj.m_PlayerData->m_JewelOfHarmonyEffect->HJOpAddSDRate; // sd(%) when attack
	pMsg.SDBypass = Obj.m_PlayerData->m_JewelOfHarmonyEffect->HJOpAddIgnoreSDRate; // sd % when attack
	pMsg.unk4 = Obj.m_PlayerData->m_MPSkillOpt->iMpsIncIgnoreEnemyBlock + Obj.m_PlayerData->SetOpIgnoreDefense + Obj.m_PlayerData->m_WingExcOption->iWingOpIgnoreEnemyDefense; // def ign rate
	pMsg.unk5 = Obj.AutoHPRecovery + Obj.m_PlayerData->m_MPSkillOpt->iMpsAutoRecoverLife + Obj.m_PlayerData->m_btRefillHPSocketOption + Obj.m_PlayerData->m_JewelOfHarmonyEffect->HJOpAddRefillHP + Obj.m_PlayerData->m_WingExcOption->iWingOpRecoveryHP; // hp rec rate
	pMsg.ZenObincRate = 12;
	pMsg.unk7 = Obj.m_PlayerData->m_MPSkillOpt->iMpsAddSturn; // stun rate
	pMsg.unk8 = Obj.m_PlayerData->m_Resistance_Stun; // stun res rate
	pMsg.unk9 = 45678;
	pMsg.unk10 = 0; // shield anbsorb
	pMsg.unk11 = Obj.m_PlayerData->m_MPSkillOpt->iMpsMonsterDieGetLife; // mob attack hp rec
	pMsg.unk12 = Obj.m_PlayerData->m_MPSkillOpt->iMpsMonsterDieGetMana;; // mob attack mp rec 
	pMsg.unk13 = Obj.m_PlayerData->m_MPSkillOpt->iMpsMonsterDieGetSD; // mob attack sd rec
	pMsg.unk14 = Obj.m_PlayerData->m_MPSkillOpt->iMpsSDSpeed; // sd rec rate
	pMsg.unk15 = Obj.m_PlayerData->m_MPSkillOpt->iMpsRecoverManaRate; // res all mp rate
	pMsg.unk16 = Obj.m_PlayerData->m_MPSkillOpt->iMpsRecoverHPRate; // res all hp rate
	pMsg.unk17 = Obj.m_PlayerData->m_MPSkillOpt->iMpsRecoverSDRate; // res all sd rate
	pMsg.unk18 = Obj.m_PlayerData->m_MPSkillOpt->iMpsAutoRecoverAG + Obj.m_PlayerData->SetOpIncAGValue; // ag rec rate
	pMsg.unk19 = 0; // dmg absorb rate
	pMsg.unk20 = Obj.m_PlayerData->m_MPSkillOpt->iMpsShieldBlockRate + Obj.m_PlayerData->m_MPSkillOpt->iMpsElfShieldBlockRate + Obj.m_PlayerData->m_MPSkillOpt->iMpsAddShieldBlockingRage; // shield block rate
	pMsg.unk21 = 0; // weapon block rate
	pMsg.unk22 = 0; // hp absorb
	pMsg.unk23 = Obj.m_PlayerData->m_JewelOfHarmonyEffect->HJOpDecreaseSDRate; // sd absorb
	pMsg.unk24 = Obj.m_PlayerData->m_MPSkillOpt->iMpsReturnEnemyAttack + Obj.m_PlayerData->m_WingExcOption->iWingOpReturnEnemyDamage; // return dmg rate
	pMsg.IncreaseDamageRate = 0; // dmg inc rate
	pMsg.unk26 = Obj.m_SkillInfo.SoulBarrierManaRate; // MP use redc rate
	pMsg.CritDMGRate = Obj.m_CriticalDamage + Obj.m_PlayerData->SetOpAddCriticalDamage; // Critical Dmg Rate
	pMsg.ExcDMGRate = Obj.m_ExcelentDamage + Obj.m_PlayerData->SetOpAddExDamageSuccessRate; // Excellent Dmg Rate
	pMsg.DoubleDmgRate = Obj.m_PlayerData->m_MPSkillOpt->iMpsIncDoubleDamageRate + Obj.m_PlayerData->m_MPSkillOpt->iMpsIncDarkSpiritDoubleDamageRate + Obj.m_PlayerData->m_MPSkillOpt->iMpsAddDoubleDamage + Obj.m_PlayerData->SetOpDoubleDamage + Obj.m_PlayerData->m_MPSkillOpt->iMpsMonkDoubleDamage + Obj.m_PlayerData->m_WingExcOption->iWingOpAddDoubleDamage; // 
#if(CUSTOM_DAMAGES12 == 1)
	pMsg.unk28 = Obj.DamageDecrease + Obj.S12DamageDecrease + Obj.m_PlayerData->m_JewelOfHarmonyEffect->HJOpAddDamageDecrease; // Obj.DamageMinus
#else	
	pMsg.unk28 = Obj.DamageDecrease + Obj.m_PlayerData->m_JewelOfHarmonyEffect->HJOpAddDamageDecrease; // Obj.DamageMinus
#endif
	pMsg.unk29 = Obj.m_PlayerData->m_btAGReduceRate; // ag usage rec rate
	pMsg.unk30 = Obj.DamageReflect + Obj.m_PlayerData->SetOpReflectionDamage; // damage reflect

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}


void GameProtocol::CGReqHuntingLog(CGameObject &Obj)
{
	PMSG_ANS_HUNTING_LOG pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xEC, 0x50, sizeof(pMsg));
	pMsg.unk1 = 1;
	pMsg.unk2 = 125;
	pMsg.unk3 = 300;
	pMsg.unk4 = 400;

	sLog->outBasic("Hunting Log in Test");

}

void GameProtocol::CGSaveMuBotSettings(PMSG_MUBOT_SETTINGS_RECV* lpMsg, CGameObject &Obj)
{
	if (!gObjIsConnectedGP(Obj))
		return;

	DGMuBotSettingsSend(Obj, Obj.Name, &lpMsg->btDATA[0]);

	sLog->outBasic("[MuHelper] Option Save [%s][%s]",Obj.AccountID, Obj.Name);
}

void GameProtocol::GCSendMuBotSettings(CGameObject &Obj, BYTE* lpData)
{
	if (!gObjIsConnected(Obj))
		return;

	PMSG_MUBOT_SETTINGS_RECV pMsg;
	PHeadSetW((BYTE*)&pMsg, 0xAE, sizeof(pMsg));

	pMsg.btType = 0;
	std::memcpy(pMsg.btDATA, lpData, sizeof(pMsg.btDATA));

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));

	sLog->outBasic("[MuHelper] Option Send [%s][%s]",Obj.AccountID, Obj.Name);
}

void GameProtocol::CGRequestStartMuBot(PMSG_MUBOT_REQ_START* lpMsg, CGameObject &Obj)
{
	if (g_ConfigRead.data.mubot.enable == FALSE) 
	{
		return;
	}

	if (!gObjIsConnectedGP(Obj)) 
	{
		return;
	}

	PMSG_MUBOT_USE pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xBF, 0x51, sizeof(pMsg));
	pMsg.Money = 0;
	pMsg.Time = 0;
	pMsg.Status = 1;

	

	if (Obj.m_IfState->use == TRUE) 
	{
		return;
	}

	if (Obj.Level < g_ConfigRead.data.mubot.minlevel) 
	{
		MsgOutput(Obj, "Only characters above %d level can run MU Helper", g_ConfigRead.data.mubot.minlevel);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (Obj.MapNumber == MAP_INDEX_CASTLESIEGE || Obj.MapNumber == MAP_INDEX_VULCANROOM)
	{
		MsgOutput(Obj, "MU Helper cant run on this location");
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.m_bMUBOT = FALSE;
		return;
	}

	if (lpMsg->btType == 0) 
	{
		if (Obj.m_PlayerData->Money < 10000)
		{
			pMsg.Status = 2;
		}
		else 
		{
			Obj.m_PlayerData->Money -= 10000;
			GCMoneySend(Obj, Obj.m_PlayerData->Money);
			pMsg.Status = 0;
			pMsg.Time = 0;
			pMsg.Money = 10000;
			Obj.m_bMUBOT = TRUE;
		}
	}
	else 
	{
		Obj.m_bMUBOT = FALSE;
	}

	Obj.m_iMUBOT_TIME = 0;
	Obj.m_iMUBOT_STAGE = 0;
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}

std::vector<Recv_PetItem_Info> GameProtocol::gObjRequestPetItemInfoDS(CGameObject &Obj, int inventype, std::vector<int> serials)
{
	int serverCode = g_ConfigRead.server.GetGameServerCode() / 20;
	int founditemcount = 0;
	int serverType = g_ConfigRead.server.GetServerType(); // stub server type.

	std::vector<Recv_PetItem_Info> tData;
	if (serverType == 0) // normal GS
	{
		for (int i = 0; i < serials.size(); i++)
		{
			Recv_PetItem_Info pRecvPetInfo;

			QueryResult* res = this->m_PetDB->Fetch("SELECT Pet_Level, Pet_Exp FROM T_PetItem_Info WHERE ItemSerial=%I64d",
				serials[i]);

			if (res == NULL)
			{
				this->m_PetDB->ExecQuery("INSERT INTO T_PetItem_Info (ItemSerial, Pet_Level, Pet_Exp) VALUES (%I64d, %d, %I64d)",
					serials[i], 1, 0);

				pRecvPetInfo.Level = 1;
				pRecvPetInfo.Exp = 0;
				pRecvPetInfo.nPos = i;
				pRecvPetInfo.nSerial = serials[i];

			}
			else
			{
				pRecvPetInfo.Level = this->m_PetDB->GetAsInteger(0);
				pRecvPetInfo.Exp = this->m_PetDB->GetAsInteger64(1);
				pRecvPetInfo.nPos = i;
				pRecvPetInfo.nSerial = serials[i];
			}
			tData.push_back(pRecvPetInfo);
		}
	}
	else // battlecore GS
	{
		for (int i = 0; i < serials.size(); i++)
		{
			Recv_PetItem_Info pRecvPetInfo;

			QueryResult* res = this->m_PetDB->Fetch("CALL IGC_BattleCore_LoadPetItemInfo (%I64d, %d);",
				serials[i], serverCode);

			if (res == NULL)
			{
				this->m_PetDB->ExecQuery("CALL IGC_BattleCore_SavePetItemInfo (%I64d, %d, %d, %I64d);",
					serials[i], serverCode, i, 0);

				pRecvPetInfo.Level = 1;
				pRecvPetInfo.Exp = 0;
				pRecvPetInfo.nPos = i;
				pRecvPetInfo.nSerial = serials[i];
			}
			else
			{
				pRecvPetInfo.Level = this->m_PetDB->GetAsInteger(0);
				pRecvPetInfo.Exp = this->m_PetDB->GetAsInteger64(1);
				pRecvPetInfo.nPos = this->m_PetDB->GetAsInteger64(2);
				pRecvPetInfo.nSerial = this->m_PetDB->GetAsInteger64(3);
			}
			tData.push_back(pRecvPetInfo);
		}
	}
	return tData;
}


void GameProtocol::gObjRequestPetItemInfo(CGameObject &Obj, int inventype)
{
	std::vector<int> serials;
	int founditemcount = 0;
	if (inventype == 0)
	{
		for (int n = 0; n < INVENTORY_SIZE; n++)
		{
			if (Obj.pntInventory[n]->IsItem())
			{
				if (Obj.pntInventory[n]->m_Type == ITEMGET(13, 4) || Obj.pntInventory[n]->m_Type == ITEMGET(13, 5))
				{
					founditemcount++;
					serials.push_back(Obj.pntInventory[n]->m_Number);
				}
			}
		}
	}
	else if (inventype == 1)
	{
		for (int n = 0; n < WAREHOUSE_SIZE; n++)
		{
			if (Obj.pntWarehouse[n]->IsItem())
			{
				if (Obj.pntWarehouse[n]->m_Type == ITEMGET(13, 4) || Obj.pntWarehouse[n]->m_Type == ITEMGET(13, 5))
				{
					founditemcount++;
					serials.push_back(Obj.pntWarehouse[n]->m_Number);
				}
			}
		}
	}

	std::vector<Recv_PetItem_Info> data = gObjRequestPetItemInfoDS(lpObj, inventype, serials);

	PMSG_SEND_PET_ITEMINFO pMsg;
	pMsg.h.set((BYTE*)&pMsg, 0xA9, sizeof(pMsg));
	pMsg.InvenType = inventype;
	pMsg.PetType = g_ConfigRead.server.GetServerType();

	if (inventype == 0)
	{
		BOOL ReCalc = 0;

		for (int n = 0; n < data.size(); n++)
		{
			if (data[n].nPos < INVETORY_WEAR_SIZE)
			{
				ReCalc = 1;
			}

			if (Obj.pntInventory[data[n]->nPos]->IsItem())
			{
				if (Obj.pntInventory[data[n]->nPos]->m_Number == data[n]->nSerial)
				{
					Obj.pntInventory[data[n]->nPos]->SetPetItemInfo(data[n]->Level, data[n]->Exp);
				}
			}
			this->CGRequestPetItemInfo(lpObj, pMsg);
		}

		if (ReCalc != FALSE)
		{
			gObjCalCharacter.CalcCharacter(lpObj);
		}
	}
	else if (inventype == 1)
	{
		for (int n = 0; n < data.size(); n++)
		{

			if (Obj.pntWarehouse[data[n]->nPos]->IsItem())
			{
				if (Obj.pntWarehouse[data[n]->nPos]->m_Number == data[n]->nSerial)
				{
					Obj.pntWarehouse[data[n]->nPos]->SetPetItemInfo(data[n]->Level, data[n]->Exp);
				}
			}
			this->CGRequestPetItemInfo(lpObj, pMsg);
		}
	}
}

void GameProtocol::GJPUserClose(LPSTR szAccountID)
{
	SDHP_USERCLOSE_ID pClose;

	pClose.h.c = 0xC1;
	pClose.h.size = sizeof(SDHP_USERCLOSE_ID);
	pClose.h.headcode = 0x05;
	std::memcpy(pClose.szId, szAccountID, 10);

	wsJServerCli.DataSend((char*)&pClose, pClose.h.size);
}

void GameProtocol::GJPUserClose(CGameObject &Obj)
{
	SDHP_USERCLOSE_ID pClose;

	pClose.h.c = 0xC1;
	pClose.h.size = sizeof(SDHP_USERCLOSE_ID);
	pClose.h.headcode = 0x05;
	std::memcpy(pClose.szId, Obj.AccountID, 10);
	std::memcpy(pClose.szName, Obj.BackName, 10);
	pClose.Level = Obj.Level;
	pClose.DbClass = Obj.m_PlayerData->DbClass;

	wsJServerCli.DataSend((char*)&pClose, pClose.h.size);
}

//////////////////////////////////////////////// DS PROTOCOL /////////////////////////////////////////////////////////////////////


int gMapSrvGroupOnlineServer = 0;

//void(*ItemCreate)(CGameObject &Obj, BYTE MapNumber, BYTE x, BYTE y, int type, BYTE level, BYTE dur, BYTE Op1, BYTE Op2, BYTE Op3, int LootIndex, BYTE NewOption, BYTE SetOption, time_t lDuration, BYTE *SocketOption, BYTE MainAttribute);

void DataServerProtocolCore(BYTE protoNum, BYTE* aRecv, int aLen)
{

#if(TRACE_PROTOCOL==1)
	LogAddHeadHex("DATA_SERVER", aRecv, aLen);
#endif

	switch (protoNum)
	{
	case 0x00:
		DataServerLoginResult((SDHP_RESULT *)aRecv);
		break;

	case 0x01:
		JGPGetCharList(aRecv);
		break;

	case 0x02:
		DCInfo.DGAnsClassDefData((PMSG_ANS_CLASSDEF *)aRecv);
		break;

	case 0x03:
		DGAnsSwitchWare((PMSG_ANS_SWITCHWARE *)aRecv);
		break;

	case 0x04:
		JGCharacterCreateRequest((SDHP_CREATECHARRESULT *)aRecv);
		break;

	case 0x05:
		JGCharDelRequest((SDHP_CHARDELETERESULT *)aRecv);
		break;

	case 0x06:
		JGGetCharacterInfo((SDHP_DBCHAR_INFORESULT *)aRecv);
		break;

	case 0x08:
		DGGetWarehouseList((SDHP_GETWAREHOUSEDB_SAVE *)aRecv);
		break;

	case 0x09:
		DGSetWarehouseList((PMSG_ANS_WARESAVE *)aRecv);
		break;

	case 0x10:
		GDGetWarehouseNoItem((SDHP_GETWAREHOUSEDB_RESULT *)aRecv);
		break;

	case 0x4C:
		if (aRecv[3] == 0x00)
			g_MineSystem.GDAnsModifyMineSystemUPTUserInfo((SDHP_ANS_MINESYSTEM_UPT_USERINFO *)aRecv);
		else if (aRecv[3] == 0x01)
			g_MineSystem.GDAnsLoadMineSystemUPTUserInfo((SDHP_ANS_LOAD_MINESYSTEM_UPT_USERINFO *)aRecv);
		break;

	case 0x3E:
		if (aRecv[3] == 0x00)
			g_EvoMonMng.DGAnsEvoMonMaxScore(aRecv);
		break;

	case 0x4F:
	{
		PMSG_DEFAULT2 * lpDef1 = (PMSG_DEFAULT2*)aRecv;
		PWMSG_DEFAULT2 * lpDef2 = (PWMSG_DEFAULT2*)aRecv;

		if (aRecv[0] == 0xC1)
		{
			switch (lpDef1->subcode)
			{
			case 0x01:
				g_GremoryCase.DGAnsAddItemToGremoryCase((_stAnsAddItemToGremoryCase *)aRecv);
				break;
			case 0x02:
				g_GremoryCase.DGAnsCheckItemUseGremoryCase((_stAnsCheckUseItemGremoryCase *)aRecv);
				break;
			}
		}

		else if (aRecv[0] == 0xC2)
		{
			switch (lpDef2->subcode)
			{
			case 0x00:
				g_GremoryCase.DGAnsStorageItemList(aRecv);
				break;
			}
		}
	}
	break;

	case 0x52:
	{
		ItemSerialCreateRecv((SDHP_ITEMCREATERECV *)aRecv);
		break;
	}
	case 0x56:
		DGRecvPetItemInfo(aRecv);
		break;

	case 0x60:
		DGOptionDataRecv((SDHP_SKILLKEYDATA_SEND *)aRecv);
		break;

	case 0x6F:
	{
		PMSG_DEFAULT2 * lpDef1 = (PMSG_DEFAULT2*)aRecv;
		PWMSG_DEFAULT2 * lpDef2 = (PWMSG_DEFAULT2*)aRecv;

		if (aRecv[0] == 0xC1)
		{
			switch (lpDef1->subcode)
			{
			case 0x02:
				g_CancelItemSale.DGAnsGetReBuyItem((SDHP_ANS_SHOP_REBUY_GET_ITEM *)aRecv);
				break;
			}
		}

		else if (aRecv[0] == 0xC2)
		{
			switch (lpDef2->subcode)
			{
			case 0x00:
				g_CancelItemSale.DGAnsSoldItemList(aRecv);
				break;
			}
		}
	}
	break;

	case 0x80:
	{
		PMSG_DEFAULT2 * lpDef1 = (PMSG_DEFAULT2 *)aRecv;

		switch (lpDef1->subcode)
		{
		case 0x00:
			GS_DGAnsCastleTotalInfo(aRecv);
			break;

		case 0x01:
			GS_DGAnsOwnerGuildMaster(aRecv);
			break;

		case 0x03:
			GS_DGAnsCastleNpcBuy(aRecv);
			break;

		case 0x04:
			GS_DGAnsCastleNpcRepair(aRecv);
			break;

		case 0x05:
			GS_DGAnsCastleNpcUpgrade(aRecv);
			break;

		case 0x06:
			GS_DGAnsTaxInfo(aRecv);
			break;

		case 0x07:
			GS_DGAnsTaxRateChange(aRecv);
			break;

		case 0x08:
			GS_DGAnsCastleMoneyChange(aRecv);
			break;

		case 0x09:
			GS_DGAnsSiegeDateChange(aRecv);
			break;

		case 0x0A:
			GS_DGAnsGuildMarkRegInfo(aRecv);
			break;

		case 0x0B:
			GS_DGAnsSiegeEndedChange(aRecv);
			break;

		case 0x0C:
			GS_DGAnsCastleOwnerChange(aRecv);
			break;

		case 0x0D:
			GS_DGAnsRegAttackGuild(aRecv);
			break;

		case 0x0E:
			GS_DGAnsRestartCastleState(aRecv);
			break;

		case 0x0F:
			GS_DGAnsMapSvrMsgMultiCast(aRecv);
			break;

		case 0x10:
			GS_DGAnsRegGuildMark(aRecv);
			break;

		case 0x11:
			GS_DGAnsGuildMarkReset(aRecv);
			break;

		case 0x12:
			GS_DGAnsGuildSetGiveUp(aRecv);
			break;

		case 0x16:
			GS_DGAnsNpcRemove(aRecv);
			break;

		case 0x17:
			GS_DGAnsCastleStateSync(aRecv);
			break;

		case 0x18:
			GS_DGAnsCastleTributeMoney(aRecv);
			break;

		case 0x19:
			GS_DGAnsResetCastleTaxInfo(aRecv);
			break;

		case 0x1A:
			GS_DGAnsResetSiegeGuildInfo(aRecv);
			break;

		case 0x1B:
			GS_DGAnsResetRegSiegeInfo(aRecv);
			break;

		case 0x1F:
			GS_DGAnsGlobalPostMultiCast(aRecv);
			break;
		}
	}
	break;

	case 0x81:
		GS_DGAnsCastleInitData(aRecv);
		break;

	case 0x82:
		GS_DGAnsCastleNpcInfo(aRecv);
		break;

	case 0x83:
		GS_DGAnsAllGuildMarkRegInfo(aRecv);
		break;

	case 0x84:
		GS_DGAnsFirstCreateNPC(aRecv);
		break;

	case 0x85:
		GS_DGAnsCalcREgGuildList(aRecv);
		break;

	case 0x86:
		GS_DGAnsCsGulidUnionInfo(aRecv);
		break;

	case 0x87:
		GS_DGAnsCsSaveTotalGuildInfo(aRecv);
		break;

	case 0x88:
		GS_DGAnsCsLoadTotalGuildInfo(aRecv);
		break;

	case 0x89:
		GS_DGAnsCastleNpcUpdate(aRecv);
		break;

	case 0x61:
		DGMuBotOptionRecv((LPMUBOT_SETTINGS_SEND*) aRecv);
		break;

	case 0xB0:
		DGAnsCrywolfSync(aRecv);
		break;

	case 0xB1:
		DGAnsCrywolfInfoLoad(aRecv);
		break;

	case 0xB2:
		DGAnsCrywolfInfoSave(aRecv);
		break;

	case 0xBD:
	{
		PMSG_DEFAULT2 * lpDef1 = (PMSG_DEFAULT2 *)aRecv;

		switch (lpDef1->subcode)
		{
		case 0x02:
			EGAnsBloodCastleEnterCount((PMSG_ANS_BLOODCASTLE_ENTERCOUNT *)aRecv);
			break;
		}
	}
	break;

	case 0xBE:
		EventChipEventProtocolCore(aRecv[3], aRecv, aLen);
		break;

	case 0xCF:
		DGMoveOtherServer((SDHP_CHARACTER_TRANSFER_RESULT *)aRecv);
		break;

	case 0xC0:
	{
		//PMSG_DEFAULT2 * lpDef1 = (PMSG_DEFAULT2 *)aRecv;

		//switch ( lpDef1->subcode )
		//{
		//case 0x01 :
		//STAT_REQ_ANS_SAVE_LOAD_DATA *lpMsg = reinterpret_cast<STAT_REQ_ANS_SAVE_LOAD_DATA *>(aRecv);
		//if(lpObj->UserStatistics != 0)
		//{
		//lpObj->UserStatistics->LoadData(&lpMsg->UserData);
		//}
		//break;
		//}
	}
	break;

	case 0xD0:
	{
		PMSG_DEFAULT2 * lpDef1 = (PMSG_DEFAULT2 *)aRecv;
		PWMSG_DEFAULT2 * lpDef2 = (PWMSG_DEFAULT2*)aRecv;

		if (aRecv[0] == 0xC1)
		{
			switch (lpDef1->subcode)
			{
			case 0x01:
				g_PeriodItemEx.DGPeriodItemExInsert((PMSG_ANS_PERIODITEMEX_INSERT *)aRecv);
				break;

			case 0x02:
				g_PeriodItemEx.DGPeriodItemExDelete((PMSG_ANS_PERIODITEMEX_DELETE *)aRecv);
				break;

			case 0x04:
				g_PeriodItemEx.DGPeriodItemExListCount((PMSG_ANS_PERIODITEMEX_LIST_COUNT *)aRecv);
				break;

			case 0x05:
				g_PeriodItemEx.DGPeriodItemExList((PMSG_ANS_PERIODITEMEX_LIST *)aRecv);
				break;
			}
		}

		else if (aRecv[0] == 0xC2)
		{
			switch (lpDef2->subcode)
			{
			case 0x03:
				g_PeriodItemEx.DGPeriodItemExExpiredItemList((PMSG_ANS_PERIODITEMEX_EXPIRED_ITEMLIST *)aRecv);
				break;
			}
		}
	}

	break;

	case 0xD1:
		DGAnsInGameShopPoint((ISHOP_ANS_POINT *)aRecv);
		break;
	case 0xD2:
		DGAnsInGameShopItemList(aRecv);
		break;
	case 0xD4:
		g_LuckyItemManager.DGAnsLuckyItemList((PMSG_ANS_LUCKYITEM_SELECT *)aRecv);
		break;
	case 0xD5:
		DGAnsInGameShopItemBuy(aRecv);
		break;
	case 0xD6:
		DGAnsInGameShopItemGift(aRecv);
		break;
	case 0xD9:
		DGAnsInGameShopItemUse((ISHOP_ITEM_USEANS *)aRecv);
		break;
	case 0xE0:
		g_PentagramSystem.DBANS_GetPentagramJewel(aRecv);
		break;

	case 0xE4:
	{
		PMSG_DEFAULT2 * lpDef1 = (PMSG_DEFAULT2 *)aRecv;

		switch (lpDef1->subcode)
		{
		case 0x03:
			g_BuffEffect.DGPeriodItemExSelect((PMSG_ANS_PERIODBUFF_SELECT *)aRecv);
			break;
		}
	}
	break;

	case 0xE5:
		g_ArcaBattle.DGAnsArcaBattleAllJoinUser((PMSG_ANS_AB_ALL_JOIN_USER_DS *)aRecv);
		break;

	case 0xE6:
		DGLoadEventInvenItem((SDHP_ANS_DBEVENT_INVEN_LOAD *)aRecv);
		break;

	case 0xE8:
		g_CMuRummyMng.GDAnsCardInfo((PMSG_ANS_MURUMMY_SELECT_DS *)aRecv);
		break;

	case 0xE9:
		g_PersonalStore.GDAnsPShopItemValue((PMSG_ANS_PSHOPITEMVALUE_INFO *)aRecv);
		break;

	case 0xF5:
		g_ChaosCastleFinal.DG_Ans_CCF_Rank((PMSG_ANS_CCF_RANK *)aRecv);
		break;

	case 0xF9:
		if (aRecv[3] == 0x01)
		{
			DGAns_ChaosCastle_KillPoint_Result((SDHP_ANS_KILLPOINT_RESULT_CC_UBF *)aRecv);
		}
		else if (aRecv[3] == 0xA3)
		{
			DGAns_GetCCFPermission((SDHP_ANS_CCF_GETPERMISSION *)aRecv);
		}
		else if (aRecv[3] == 0xA4)
		{
			DGAns_CCF_Info_Notify((SDHP_ANS_CCF_INFO_NOTIFY *)aRecv);
		}
		break;

	case 0xFD:
		if (aRecv[0] == 0xC1)
		{
			if (aRecv[3] == 0x00)
			{
				DG_DSF_CanPartyEnter((PMSG_ANS_DSF_CAN_PARTY_ENTER *)aRecv);
			}

			else if (aRecv[3] == 0x03)
			{
				DG_DSF_PartyRankRenew((PMSG_ANS_DSF_PARTYRANKRENEW *)aRecv);
			}

			else if (aRecv[3] == 0x07)
			{
				DG_DSF_GetReward((PMSG_ANS_GET_DSF_REWARD *)aRecv);
			}
		}

		else if (aRecv[0] == 0xC2)
		{
			if (aRecv[4] == 0x05)
			{
				DG_DSF_GoFinalParty(aRecv);
			}
		}
		break;

	case 0xF6:
		g_QuestExpUserMng.UserQuestInfoLoad((PMSG_ANS_QUESTEXP_INFO *)aRecv);
		break;

	case 0xF8:
	{
		PMSG_DEFAULT2 * lpDef1 = (PMSG_DEFAULT2*)aRecv;
		PWMSG_DEFAULT2 * lpDef2 = (PWMSG_DEFAULT2*)aRecv;

		if (aRecv[0] == 0xC1)
		{
			switch (lpDef1->subcode)
			{
			case 0x2B:
				DGAnsArcaBattleGuildJoinSelect((PMSG_ANS_ARCA_BATTLE_GUILD_JOIN_DS *)aRecv);
				break;
			case 0x31:
				DGAnsArcaBattleGuildJoin((PMSG_ANS_ARCA_BATTLE_GUILD_JOIN_DS *)aRecv);
				break;
			case 0x33:
				DGAnsArcaBattleGuildMemberJoin((PMSG_ANS_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS *)aRecv);
				break;
			case 0x35:
				DGAnsArcaBattleEnter((PMSG_ANS_ARCA_BATTLE_ENTER_DS *)aRecv);
				break;
			case 0x3A:
				DGAnsArcaBattleWinGuildInfo((PMSG_ANS_AB_WIN_GUILD_INFO_DS *)aRecv);
				break;
			case 0x40:
				DGReqArcaBattleProcMultiCast((PMSG_ANS_AB_PROC_STATE_DS *)aRecv);
				break;
			case 0x42:
				GDAnsJoinMemberUnder((PMSG_ANS_AB_JOIN_MEMBER_UNDER_DS *)aRecv);
				break;
			case 0x44:
				g_ArcaBattle.DGAnsArcaBattleJoinMemberUnderReq((PMSG_ANS_AB_JOIN_CANCEL_DS *)aRecv);
				break;
			case 0x46:
				g_ArcaBattle.DGAnsRegisteredMemberCnt((PMSG_ANS_AB_REG_MEMBER_CNT_DS *)aRecv);
				break;
			case 0x48:
				g_ArcaBattle.DGAnsRemoveAllRewardBuff();
				break;
			case 0x4C:
				g_ArcaBattle.DGAnsRemoveRewardBuff((PMSG_ANS_REMOVE_GUILD_BUFF_DS *)aRecv);
				break;
			case 0x4E:
				g_ArcaBattle.DGAnsMarkCnt((PMSG_ANS_ARCA_BATTLE_MARK_CNT_DS *)aRecv);
				break;
			case 0x50:
				g_ArcaBattle.DGAnsMarkReg((PMSG_ANS_ARCA_BATTLE_MARK_REG_DS *)aRecv);
				break;
			case 0x52:
				g_ArcaBattle.DGAnsMarkRank((PMSG_ANS_ARCA_BATTLE_MARK_RANK_DS *)aRecv);
				break;
			case 0xF1:
				g_AcheronGuardianEvent.DGAnsAcheronEventProcMultiCast((PMSG_ANS_AE_PLAY_DS *)aRecv);
				break;
			default:
				if (aRecv[4] == 0xFC)
					g_ArcaBattle.DGAnsAllGuildMarkCnt((PMSG_ANS_ALL_GUILD_MARK_CNT_DS *)aRecv);
				break;
			}
		}
	}
	break;
	case 0xF3:
	{
		PMSG_DEFAULT2 * lpDef1 = (PMSG_DEFAULT2 *)aRecv;

		switch (lpDef1->subcode)
		{
		case 0x01:
			DGAnsUBFAccountUserInfo((PMSG_ANS_UBF_ACCOUNT_USERINFO *)aRecv);
			break;
		case 0x02:
			DGUBFRegisterAccountUserResult((PMSG_UBF_REGISTER_ACCOUNT_USER_RESULT *)aRecv);
			break;
		case 0x03:
			DGUBFAccountUserCopyResult((PMSG_UBF_ACCOUNT_USER_COPY_RESULT *)aRecv);
			break;
		case 0x05:
			DGAnsSetReceivedWinnerItem((PMSG_ANS_UBF_SET_RECEIVED_REWARD *)aRecv);
			break;
		case 0x06:
			DGAnsUBFGetReward((PMSG_ANS_UBF_GET_REWARD *)aRecv);
			break;
		case 0x07:
			DGUBFAnsCancelToJoinResult((PMSG_UBF_ANS_CANCEL_REGISTER_USER *)aRecv);
			break;
		case 0x08:
			DGUBFAnsGetRealNameAndServerCode((PMSG_ANS_GET_UBF_REAL_NAME *)aRecv);
			break;
		}
	}
	break;
	case 0xF1:
		g_CMuunSystem.DGLoadMuunInvenItem((SDHP_ANS_DBMUUN_INVEN_LOAD *)aRecv);
		break;
	case 0xFF:	// Test Packet
	{
		PMSG_TEST * pMsg = (PMSG_TEST *)aRecv;

		sLog->outError("error-L2DataServer RECV : %d (%d)", aLen, pMsg->count);
	}
	break;
	case 0xC2:// JoinMu
		DGReqOtherChannelWishper((PMSG_RECV_CHATDATA_WHISPER*)aRecv);
		break;
	case 0xC3:
	{
		PMSG_DEFAULT2 * lpDef1 = (PMSG_DEFAULT2 *)aRecv;
		switch (lpDef1->subcode)
		{
		case 0x01:
		{
			DSMSG_ANS_SUBSERVER_COUNT * Ans = (DSMSG_ANS_SUBSERVER_COUNT*)aRecv;
			gMapSrvGroupOnlineServer = Ans->wSvrCount;
		}
		break;
		case 0x03:
		{
			DGWhisperResponseRecv((DSMSG_GS_WHISPER_RESULT*)aRecv);
		}
		break;
		case 0x04:
		{
			DGDisconnectOtherChannel((PMSG_RECV_DC_OTHER_CHANNEL*)aRecv);
		}
		break;
		}
	}
	break;
	}
}

void DataServerLogin()
{
	SDHP_SERVERINFO pInfo;

	pInfo.h.c = 0xC1;
	pInfo.h.size = sizeof(SDHP_SERVERINFO);	// Size
	pInfo.h.headcode = 0x00;
	pInfo.Port = g_ConfigRead.server.GetGameServerPort();
	pInfo.Type = 1;
	pInfo.ServerCode = g_ConfigRead.server.GetGameServerCode();
	std::strcpy(pInfo.ServerName, g_ConfigRead.server.GetServerName());

	wsDataCli.DataSend((char *)&pInfo, sizeof(pInfo));
}

void DataServerLoginResult(SDHP_RESULT * lpMsg)
{
	if (lpMsg->Result == false)
	{
		g_Log.MsgBox("Authentication error from authentication server");
		return;
	}

	DataServerConnected = TRUE;
	gServerReady++;
	SendMessage(ghWnd, WM_START_SERVER, 0, 0); // Review the Message WM_USER +5

	DCInfo.GDReqClassDefData();
}





/* * * * * * * * * * * * * * * * * * * * *
*	Mu Char List Count Packet
*	Direction : GameServer -> Client
*  Code     : 0xC1
*	HeadCode : 0xF3
*	SubCode  : 0x00
*	BASE PACKET - Complete with PMSG_CHARLIST
*/
#pragma pack (1)

#pragma pack ()

struct PMSG_CHARLIST
{
	BYTE Index;	// 0
	char Name[10];	// 1
	WORD Level;	// C
	BYTE CtlCode;	// E
	BYTE CharSet[CHAR_SET_SIZE];	// F
	BYTE btGuildStatus;	// 21
};

struct PMSG_CHARLIST_S9
{
	BYTE Index;	// 0
	char Name[10];	// 1
	WORD Level;	// C
	BYTE CtlCode;	// E
	BYTE CharSet[CHAR_SET_SIZE];	// F
	BYTE btGuildStatus;	// 21
	BYTE btPkLevel;
};

struct PMSG_CHARLIST_ENABLE_CREATION
{
	PBMSG_HEAD2 h; // C1:DE:00
	BYTE EnableClass;// 4
};

struct PMSG_RESET_INFO_CHARLIST
{
	PBMSG_HEAD2 h;
	WORD Reset[5];
};

void JGPGetCharList(BYTE * lpRecv)
{
	SDHP_CHARLISTCOUNT * lpCount = (SDHP_CHARLISTCOUNT *)lpRecv;
	SDHP_CHARLIST * lpCL;
	char szId[MAX_ACCOUNT_LEN + 1];
	BYTE sendbuf[256];
	int lOfs = 0;
	int lsOfs = sizeof(SDHP_CHARLISTCOUNT);
	CGameObject* lpObj = getGameObject(lpCount->Number);
	BYTE index;
	PMSG_CHARLISTCOUNT pCLCount;	// Packet Char List Count
	PMSG_CHARLIST_S9 pCList;
	WORD TempntInventory[12];

	PMSG_RESET_INFO_CHARLIST pRMsg = { -1 };
	pRMsg.h.c = 0xC1;
	pRMsg.h.headcode = 0xFA;
	pRMsg.h.subcode = 0x0A;
	pRMsg.h.size = sizeof(pRMsg);

	pCLCount.h.c = 0xC1;
	pCLCount.h.headcode = 0xF3;
	pCLCount.subcode = 0x00;//K2
	pCLCount.Count = lpCount->Count;

	lpObj->CharsCount = pCLCount.Count;


	szId[MAX_ACCOUNT_LEN] = 0;
	std::memcpy(szId, lpCount->AccountId, MAX_ACCOUNT_LEN);

	if (::gObjIsAccontConnect(Obj, szId) == FALSE)
	{
		sLog->outError("Requested character list doesn't match the user (%s)%s", lpObj->AccountID, szId);
		IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);

		return;
	}

	lpObj->EnableCharacterCreate = lpCount->EnableCharacterCreate;
	lpObj->m_PlayerData->m_WarehouseExpansion = lpCount->WhExpansion;
	lpObj->m_PlayerData->m_iSecurityCode = lpCount->SecurityCode;

	if (lpObj->m_PlayerData->m_iSecurityCode != 0)
	{
		lpObj->m_PlayerData->m_bSecurityCheck = false;
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 534), Obj, 0);
	}

	else
	{
		lpObj->m_PlayerData->m_bSecurityCheck = true;
	}

	pCLCount.MaxClass = 0x1F;//lpCount->EnableCharacterCreate;
	pCLCount.MoveCnt = lpCount->MoveCnt;
	pCLCount.WhExpansion = lpCount->WhExpansion;
	pCLCount.CharacterSlotCount = 5;
	PMSG_CHARLIST_ENABLE_CREATION pMsgC;
	PHeadSubSetB((BYTE*)&pMsgC, 0xDE, 0x00, sizeof(pMsgC));

	pMsgC.EnableClass = lpCount->EnableCharacterCreate;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR *)&pMsgC, sizeof(PMSG_CHARLIST_ENABLE_CREATION));

	std::memset(sendbuf, 0, sizeof(sendbuf));
	lOfs += sizeof(PMSG_CHARLISTCOUNT);

	if (pCLCount.Count > 0)
	{
		lpObj->m_PlayerData->m_NameConvertOfUBF.InitData(); // UBF Name Conversion

		for (int n = 0; n < pCLCount.Count; n++)
		{
			std::memset(&pCList, 0, sizeof(pCList));
			lpCL = (SDHP_CHARLIST *)&lpRecv[lsOfs];
			pRMsg.Reset[n] = lpCL->Resets;
			pCList.Index = lpCL->Index;
			pCList.Level = lpCL->Level;
			pCList.CtlCode = lpCL->CtlCode;
			pCList.btGuildStatus = lpCL->btGuildStatus;

			//lpObj->Name1[n] := 
			if (lpObj->m_cAccountItemBlock != 0)
				pCList.CtlCode |= 0x10;	// Set Block Item

			if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
			{
				std::string tmpKeyName;
				std::string tmpRealName;

				tmpKeyName.insert(0, lpCL->Name, MAX_ACCOUNT_LEN);
				tmpRealName.insert(0, lpCL->UnityBFRealName, MAX_ACCOUNT_LEN);

				lpObj->m_PlayerData->m_NameConvertOfUBF.InputData(n, tmpKeyName.c_str(), tmpRealName.c_str(), lpCL->ServerCodeOfHomeWorld, MAX_ACCOUNT_LEN);
				sLog->outBasic("[UBF][ConvertInputData][%d][%s][KeyName: %s][RealName: %s] ServerCode: %d",
					Obj, lpObj->AccountID, tmpKeyName.c_str(), tmpRealName.c_str(), lpCL->ServerCodeOfHomeWorld);

				std::memcpy(pCList.Name, lpCL->UnityBFRealName, MAX_ACCOUNT_LEN);
			}

			else
			{
				std::memcpy(pCList.Name, lpCL->Name, MAX_ACCOUNT_LEN);
			}

			pCList.CharSet[CS_CLASS] = CS_GET_CLASS_SX(lpCL->Class);
			int changeup = CS_GET_CHANGEUP_SX(lpCL->Class);

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
			std::memcpy(lpObj->Name1[n], pCList.Name, MAX_ACCOUNT_LEN);

			if (lpCL->dbInventory[0] == (BYTE)-1 && (lpCL->dbInventory[2] & 0x80) == 0x80 && (lpCL->dbInventory[3] & 0xF0) == 0xF0)
			{
				TempntInventory[0] = -1;
			}
			else
			{
				TempntInventory[0] = (lpCL->dbInventory[0] + (lpCL->dbInventory[2] & 0x80) * 2) + (lpCL->dbInventory[3] & 0xF0) * 32;
			}

			if (lpCL->dbInventory[4] == (BYTE)-1 && (lpCL->dbInventory[6] & 0x80) == 0x80 && (lpCL->dbInventory[7] & 0xF0) == 0xF0)
			{
				TempntInventory[1] = -1;
			}
			else
			{
				TempntInventory[1] = (lpCL->dbInventory[4] + (lpCL->dbInventory[6] & 0x80) * 2) + (lpCL->dbInventory[7] & 0xF0) * 32;
			}

			if (lpCL->dbInventory[8] == (BYTE)-1 && (lpCL->dbInventory[10] & 0x80) == 0x80 && (lpCL->dbInventory[11] & 0xF0) == 0xF0)
			{
				TempntInventory[2] = 0x1FF;
			}
			else
			{
				TempntInventory[2] = ((lpCL->dbInventory[8] + (lpCL->dbInventory[10] & 0x80) * 2) + (lpCL->dbInventory[11] & 0xF0) * 32) % MAX_SUBTYPE_ITEMS;
			}

			if (lpCL->dbInventory[12] == (BYTE)-1 && (lpCL->dbInventory[14] & 0x80) == 0x80 && (lpCL->dbInventory[15] & 0xF0) == 0xF0)
			{
				TempntInventory[3] = 0x1FF;
			}
			else
			{
				TempntInventory[3] = ((lpCL->dbInventory[12] + (lpCL->dbInventory[14] & 0x80) * 2) + (lpCL->dbInventory[15] & 0xF0) * 32) % MAX_SUBTYPE_ITEMS;
			}

			if (lpCL->dbInventory[16] == (BYTE)-1 && (lpCL->dbInventory[18] & 0x80) == 0x80 && (lpCL->dbInventory[19] & 0xF0) == 0xF0)
			{
				TempntInventory[4] = 0x1FF;
			}
			else
			{
				TempntInventory[4] = ((lpCL->dbInventory[16] + (lpCL->dbInventory[18] & 0x80) * 2) + (lpCL->dbInventory[19] & 0xF0) * 32) % MAX_SUBTYPE_ITEMS;
			}

			if (lpCL->dbInventory[20] == (BYTE)-1 && (lpCL->dbInventory[22] & 0x80) == 0x80 && (lpCL->dbInventory[23] & 0xF0) == 0xF0)
			{
				TempntInventory[5] = 0x1FF;
			}
			else
			{
				TempntInventory[5] = ((lpCL->dbInventory[20] + (lpCL->dbInventory[22] & 0x80) * 2) + (lpCL->dbInventory[23] & 0xF0) * 32) % MAX_SUBTYPE_ITEMS;
			}

			if (lpCL->dbInventory[24] == (BYTE)-1 && (lpCL->dbInventory[26] & 0x80) == 0x80 && (lpCL->dbInventory[27] & 0xF0) == 0xF0)
			{
				TempntInventory[6] = 0x1FF;
			}
			else
			{
				TempntInventory[6] = ((lpCL->dbInventory[24] + (lpCL->dbInventory[26] & 0x80) * 2) + (lpCL->dbInventory[27] & 0xF0) * 32) % MAX_SUBTYPE_ITEMS;
			}

			if (lpCL->dbInventory[28] == (BYTE)-1 && (lpCL->dbInventory[30] & 0x80) == 0x80 && (lpCL->dbInventory[31] & 0xF0) == 0xF0)
			{
				TempntInventory[7] = 0x1FF;
			}
			else
			{
				TempntInventory[7] = ((lpCL->dbInventory[28] + (lpCL->dbInventory[30] & 0x80) * 2) + (lpCL->dbInventory[31] & 0xF0) * 32) % MAX_SUBTYPE_ITEMS;
			}

			if (lpCL->dbInventory[32] == (BYTE)-1 && (lpCL->dbInventory[34] & 0x80) == 0x80 && (lpCL->dbInventory[35] & 0xF0) == 0xF0)
			{
				TempntInventory[8] = 0x1FF;
			}
			else
			{
				TempntInventory[8] = ((lpCL->dbInventory[32] + (lpCL->dbInventory[34] & 0x80) * 2) + (lpCL->dbInventory[35] & 0xF0) * 32) % MAX_SUBTYPE_ITEMS;
			}

			pCList.CharSet[12] |= DBI_GET_TYPE(TempntInventory[0]);
			pCList.CharSet[1] = TempntInventory[0] % 256;

			pCList.CharSet[13] |= DBI_GET_TYPE(TempntInventory[1]);
			pCList.CharSet[2] = TempntInventory[1] % 256;

			pCList.CharSet[13] |= (int)(TempntInventory[2] & 0x1E0) >> 5;
			pCList.CharSet[9] |= (int)(TempntInventory[2] & 0x10) << 3;
			pCList.CharSet[3] |= (int)(TempntInventory[2] & 0x0F) << 4;

			pCList.CharSet[14] |= (int)(TempntInventory[3] & 0x1E0) >> 1;
			pCList.CharSet[9] |= (int)(TempntInventory[3] & 0x10) << 2;
			pCList.CharSet[3] |= (int)(TempntInventory[3] & 0x0F);

			pCList.CharSet[14] |= (int)(TempntInventory[4] & 0x1E0) >> 5;
			pCList.CharSet[9] |= (int)(TempntInventory[4] & 0x10) << 1;
			pCList.CharSet[4] |= (int)(TempntInventory[4] & 0x0F) << 4;

			pCList.CharSet[15] |= (int)(TempntInventory[5] & 0x1E0) >> 1;
			pCList.CharSet[9] |= (int)(TempntInventory[5] & 0x10);
			pCList.CharSet[4] |= (int)(TempntInventory[5] & 0x0F);

			pCList.CharSet[15] |= (int)(TempntInventory[6] & 0x1E0) >> 5;
			pCList.CharSet[9] |= (int)(TempntInventory[6] & 0x10) >> 1;
			pCList.CharSet[5] |= (int)(TempntInventory[6] & 0x0F) << 4;

			index = 0;

			if ((TempntInventory[8] & 0x04) == 0)
			{
				if (TempntInventory[8] == 0x1FF)
				{
					index |= 0x03;
				}
				else
				{
					index |= (TempntInventory[8]) & 0x03;
				}
			}
			else
			{
				index |= 0x03;
			}

			pCList.CharSet[5] |= index;

			int levelindex = 0;

			if (TempntInventory[0] != (WORD)-1)
			{
				levelindex = LevelSmallConvert(DBI_GET_LEVEL(lpCL->dbInventory[1]));
			}

			if (TempntInventory[1] != (WORD)-1)
			{
				levelindex |= (int)LevelSmallConvert(DBI_GET_LEVEL(lpCL->dbInventory[5])) << 3;
			}

			if (TempntInventory[2] < 0x1FF)
			{
				levelindex |= (int)LevelSmallConvert(DBI_GET_LEVEL(lpCL->dbInventory[9])) << 6;
			}

			if (TempntInventory[3] < 0x1FF)
			{
				levelindex |= (int)LevelSmallConvert(DBI_GET_LEVEL(lpCL->dbInventory[13])) << 9;
			}

			if (TempntInventory[4] < 0x1FF)
			{
				levelindex |= (int)LevelSmallConvert(DBI_GET_LEVEL(lpCL->dbInventory[17])) << 12;
			}

			if (TempntInventory[5] < 0x1FF)
			{
				levelindex |= (int)LevelSmallConvert(DBI_GET_LEVEL(lpCL->dbInventory[21])) << 15;
			}

			if (TempntInventory[6] < 0x1FF)
			{
				levelindex |= (int)LevelSmallConvert(DBI_GET_LEVEL(lpCL->dbInventory[25])) << 18;
			}

			pCList.CharSet[6] = ((int)levelindex >> 0x10) & 0xFF;
			pCList.CharSet[7] = ((int)levelindex >> 0x08) & 0xFF;
			pCList.CharSet[8] = ((int)levelindex) & 0xFF;

			pCList.CharSet[10] = 0;

			if ((TempntInventory[8] & 0x03) != 0 && TempntInventory[8] != 0x1FF)
			{
				pCList.CharSet[10] |= 0x01;
			}

			pCList.CharSet[11] = 0;

			if ((TempntInventory[8] & 0x04) != 0 && TempntInventory[8] != 0x1FF)
			{
				pCList.CharSet[12] |= 0x01;
			}

			if ((TempntInventory[8] & 0x05) != 0 && TempntInventory[8] != 0x1FF)
			{
				pCList.CharSet[12] |= 0x01;
			}

			pCList.CharSet[16] = 0;
			pCList.CharSet[17] = 0;

			if (TempntInventory[8] == 0x25 && TempntInventory[8] != 0x1FF)
			{
				pCList.CharSet[10] &= 0xFE;
				pCList.CharSet[12] &= 0xFE;
				pCList.CharSet[12] |= 0x04;
				BYTE btExcellentOption = lpCL->dbInventory[34] & 0x3F;

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

			if (TempntInventory[7] >= 0 && TempntInventory[7] <= 6)
			{
				pCList.CharSet[16] |= (0x04 * (TempntInventory[7] + 1));
			}

			else if (TempntInventory[7] >= 36 && TempntInventory[7] <= 43)
			{
				pCList.CharSet[9] |= 0x01;
				pCList.CharSet[16] |= (0x04 * (TempntInventory[7] - 36));
			}

			else if ((TempntInventory[7] >= 49 && TempntInventory[7] <= 50) || (TempntInventory[7] >= 130 && TempntInventory[7] <= 135))
			{
				pCList.CharSet[9] |= 0x02;

				if (TempntInventory[7] == 49 || TempntInventory[7] == 50)
				{
					pCList.CharSet[16] |= (0x04 * (TempntInventory[7] - 49));
				}

				else if (TempntInventory[7] >= 130 && TempntInventory[7] <= 135)
				{
					pCList.CharSet[16] |= (0x04 * (0x02 + (TempntInventory[7] - 130)));
				}
			}

			else if (TempntInventory[7] >= 262 && TempntInventory[7] <= 269)
			{
				pCList.CharSet[9] |= 0x03;

				if (TempntInventory[7] >= 262 && TempntInventory[7] <= 265)
				{
					pCList.CharSet[16] |= (0x04 * (TempntInventory[7] - 262));
				}

				if (TempntInventory[7] == 266 || TempntInventory[7] == 268)
				{
					pCList.CharSet[16] |= 0x10;
				}

				if (TempntInventory[7] == 267)
				{
					pCList.CharSet[16] |= 0x14;
				}

				if (TempntInventory[7] == 269)
				{
					pCList.CharSet[16] |= 0x1C;
				}
			}

			else if (TempntInventory[7] == 30)
			{
				pCList.CharSet[9] |= 0x03;
				pCList.CharSet[16] |= 0x18;
			}

			else if (TempntInventory[7] == 270)
			{
				pCList.CharSet[9] |= 0x04;
			}

			else if (TempntInventory[7] == 278)
			{
				pCList.CharSet[9] |= 0x04;
				pCList.CharSet[16] |= 0x04;
			}

			int itemindex = TempntInventory[8];
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
			std::memcpy(&sendbuf[lOfs], &pCList, sizeof(pCList));
			lsOfs += sizeof(SDHP_CHARLIST);
			lOfs += sizeof(pCList);
		}
	}
	pCLCount.h.size = lOfs;
	std::memcpy(sendbuf, &pCLCount, sizeof(PMSG_CHARLISTCOUNT));
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pRMsg, sizeof(pRMsg));
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, sendbuf, lOfs);

	if (gObjUseSkill.m_SkillData.EnableSiegeOnAllMaps)
	{
		BYTE EnableSiege[4] = { 0xC1, 0x04, 0xFA, 0xA0 };
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, EnableSiege, sizeof(EnableSiege));
	}

	g_GensSystem.SendBattleZoneData(&gObj[Obj]);
	GSProtocol.GCSetCharColors(Obj);

	BYTE EnableDMG[4] = { 0xC1, 0x04, 0xFA, 0xA9 }; //DMG over 65k
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, EnableDMG, sizeof(EnableDMG));

	BYTE DisableCheat[4] = { 0xC1, 0x04, 0xF3, 0xFA }; //Disable Cheat
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, DisableCheat, sizeof(DisableCheat));

}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Get Char List Request Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  : 0x01
*/
struct SDHP_GETCHARLIST
{
	PBMSG_HEAD h;	// C1:01
	char Id[10];	// 3
	short Number;	// E
	int IsUnityBattleFieldServer;
};



void DataServerGetCharListRequest(CGameObject &Obj)
{
	SDHP_GETCHARLIST pMsg;
	char TempId[11];
	char* AccountId;

	std::memset(TempId, 0, sizeof(TempId));
	AccountId = gObjGetAccountId(Obj);

	if (AccountId == 0)
	{
		sLog->outBasic("error-L2 : User who is not authenticated requested character list?? %s %d", __FILE__, __LINE__);
		IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
		return;
	}

	// When Len is Less than 1
	if (strlen(AccountId) < 1)
	{
		// Error Message
		//
		sLog->outBasic("error-L2 : ID doesn't exist %s %d", __FILE__, __LINE__);
		IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
		return;
	}

	std::strcpy((char*)TempId, AccountId);

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(SDHP_GETCHARLIST);
	pMsg.h.headcode = 0x01;
	pMsg.Number = Obj.m_Index;
	pMsg.IsUnityBattleFieldServer = g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE ? TRUE : FALSE;
	std::memcpy(pMsg.Id, TempId, sizeof(TempId) - 1);

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);

}




/* * * * * * * * * * * * * * * * * * * * *
*	Mu Get Char List Result Packet
*	Direction : GameServer -> Client
*  Code     : 0xC1
*	HeadCode : 0xF3
*	SubCode  : 0x01
*/
struct PMSG_CHARCREATERESULT
{
	PBMSG_HEAD h;	// C1:F3:01
	BYTE subcode;	// 3
	BYTE Result;	// 4
	BYTE Name[10];	// 5
	BYTE pos;	// F
	WORD Level;	// 10
	BYTE Class;	// 12
	BYTE Equipment[24];	// 13
};



void JGCharacterCreateRequest(SDHP_CREATECHARRESULT* lpMsg)
{
	PMSG_CHARCREATERESULT pResult;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	CGameObject* lpObj = getGameObject(lpMsg->Number);

	szAccountID[MAX_ACCOUNT_LEN] = 0;
	std::memcpy(szAccountID, lpMsg->AccountId, sizeof(lpMsg->AccountId));

	if (gObjIsAccontConnect(*lpObj, szAccountID) == FALSE)
	{
		sLog->outError("Request to create character doesn't match the user %s", szAccountID);
		IOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
		return;
	}

	pResult.h.c = 0xC1;
	pResult.h.size = sizeof(PMSG_CHARCREATERESULT);
	pResult.h.headcode = 0xF3;
	pResult.subcode = 0x01;
	pResult.Result = lpMsg->Result;
	pResult.pos = lpMsg->Pos;

	pResult.Class = CS_GET_CLASS_SX(lpMsg->ClassSkin);
	int changeup = CS_GET_CHANGEUP_SX(lpMsg->ClassSkin);

	if (changeup == 1)
	{
		pResult.Class |= CS_SET_CHANGEUP_SX;
	}

	if (changeup == 2)
	{
		pResult.Class |= CS_SET_CHANGEUP_SX;
		pResult.Class |= CS_SET_CHANGEUP2_SX;
	}

	pResult.Level = lpMsg->Level;
	std::memcpy(pResult.Name, lpMsg->Name, sizeof(pResult.Name));

	if (pResult.Result == 1)
	{
		std::memcpy(pResult.Equipment, lpMsg->Equipment, sizeof(pResult.Equipment));
	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
}


void JGCharacterCreateFailSend(CGameObject &Obj, char* id)
{
	PMSG_CHARCREATERESULT  pResult;

	pResult.h.c = 0xC1;
	pResult.h.size = sizeof(PMSG_CHARCREATERESULT);
	pResult.h.headcode = 0xF3;
	pResult.subcode = 0x01;
	pResult.Result = false;
	pResult.pos = 0;
	std::memcpy(pResult.Name, id, sizeof(pResult.Name));

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pResult, pResult.h.size);
}





void JGCharDelRequest(SDHP_CHARDELETERESULT * lpMsg)
{
	PMSG_RESULT pResult;
	char szAccountId[MAX_ACCOUNT_LEN + 1];
	CGameObject *lbObj = getGameObject(lpMsg->Number);

	szAccountId[MAX_ACCOUNT_LEN] = 0;
	std::memcpy(szAccountId, lpMsg->AccountID, sizeof(lpMsg->AccountID));

	if (::gObjIsAccontConnect(*lpObj, szAccountId) == FALSE)
	{
		sLog->outError("Request to delete character doesn't match the user %s", szAccountId);
		IOCP.CloseClient(*lpObj);

		return;
	}

	pResult.h.c = 0xC1;
	pResult.h.size = sizeof(PMSG_RESULT);
	pResult.h.headcode = 0xF3;
	pResult.subcode = 0x02;
	pResult.result = lpMsg->Result;

	IOCP.DataSend(lpMsg->Number, (UCHAR*)&pResult, pResult.h.size);
}

void JGGetCharacterInfo(SDHP_DBCHAR_INFORESULT * lpMsg)
{
	PMSG_CHARMAPJOINRESULT pjMsg;

	char szAccountId[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
	CGameObject* lpObj = getGameObject(lpMsg->Number);

	szAccountId[MAX_ACCOUNT_LEN] = 0;
	std::memcpy(szAccountId, lpMsg->AccountID, MAX_ACCOUNT_LEN);

	if (gObjIsAccontConnect(*lpObj, szAccountId) == FALSE)
	{
		sLog->outError("Request to receive character information doesn't match the user %s", szAccountId);
		IOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);

		return;
	}

	szName[MAX_ACCOUNT_LEN] = 0;
	std::memcpy(szName, lpMsg->Name, MAX_ACCOUNT_LEN);

	if (lpMsg->result == false)
	{
		sLog->outError("error : %s Character data setting fail %s", szName, lpObj->AccountID);
		IOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);

		return;
	}

	if ((lpMsg->CtlCode & 1) != 0)
	{
		sLog->outError("error-L1 : Load Block Character infomation", szName, lpObj->AccountID);	// Deathway Fix HERE
		IOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);

		return;
	}

	for each (std::pair<int,CGameObject*> ObjEntry in gGameObjects)
	{
		CGameObject* tObj = ObjEntry.second;
		if (tObj->Connected == PLAYER_PLAYING)
		{
			if (tObj->Type == OBJ_USER)
			{
				if (!strncmp(szName, tObj->Name, MAX_ACCOUNT_LEN) || !strncmp(szAccountId, tObj->AccountID, MAX_ACCOUNT_LEN))
				{
					sLog->outBasic("[Anti-HACK][JGGetCharacterInfo] Attempted Character-Copy by double logging [%s][%s]",
						szName, tObj->AccountID);
					GSProtocol.GCSendDisableReconnect(*tObj);
					IOCP.CloseClient(tObj->m_PlayerData->ConnectUser->Index);
					return;
				}
			}
		}
	}

	if (gObjSetCharacter((UCHAR*)lpMsg, *lpObj) == false)
	{
		sLog->outError("error : %s Character data setting fail", szName);
		sLog->outError("CL:%d LV:%d MLV:%d IDX:%d CS:%d", lpMsg->Class, lpMsg->Level, lpMsg->mLevel, lpObj, lpObj->Connected);
		IOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
		return;
	}

	lpObj->iShield = lpObj->iMaxShield + lpObj->iAddShield;

	/*if (lpObj->m_Index != ) // TODO remnant figure out if needed.
	{
		IOCP.CloseClient(Obj.m_PlayerData->ConnectUser->Index);
		return;
	}*/

	if (lpObj->m_PlayerData->Money < 0)
	{
		sLog->outError("ERROR: MONEY < 0 [%s][%s] %d -> 0", szName, lpObj->AccountID, lpObj->m_PlayerData->Money);
		lpObj->m_PlayerData->Money = 2000000000;
	}

	if (lpObj->m_bMapSvrMoveReq == false)
	{
		if (MapNumberCheck(lpObj->MapNumber) == FALSE)
		{
			IOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);
			return;
		}

		if (lpObj->m_PK_Level > 4)
		{
			lpObj->MapNumber = MAP_INDEX_RORENCIA;
			lpObj->X = 143;
			lpObj->Y = 135;
			lpObj->TX = 143;
			lpObj->TY = 135;

			sLog->outBasic("[CRenewal][PKSetting][%s][%s] PkLevel:[%d],KillCount:[%d], PkPoint:[%d] PK Character move to LORENCIA",
				lpObj->AccountID, lpObj->Name, lpObj->m_PK_Level, lpObj->m_PK_Count, lpObj->m_PK_Time);
		}

		short wGameServerCode = g_MapServerManager.CheckMoveMapSvr(*lpObj, lpObj->MapNumber, lpObj->m_sPrevMapSvrCode);

		if (wGameServerCode != g_ConfigRead.server.GetGameServerCode())
		{
			if (wGameServerCode == -1)
			{
				sLog->outError("[MapServerMng] Map Server Move Fail : CheckMoveMapSvr() == -1 [%s][%s] (%d)",
					lpObj->AccountID, lpObj->Name, lpObj->m_Index);
				IOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);

				return;
			}

			GJReqMapSvrMove(*lpObj, wGameServerCode, lpObj->MapNumber, lpObj->X, lpObj->Y);
			sLog->outBasic("[MapServerMng] Request to Move Map Server : (%d) - [%s][%s] (%d)",
				wGameServerCode, lpObj->AccountID, lpObj->Name, lpObj->m_Index);

			return;
		}
	}

	lpObj->m_bMapSvrMoveReq = false;
	lpObj->m_bMapSvrMoveReq_2 = false;

	pjMsg.h.c = 0xC3;
	pjMsg.h.headcode = 0xF3;
	pjMsg.h.size = sizeof(PMSG_CHARMAPJOINRESULT);
	pjMsg.subcode = 0x03;
	pjMsg.MapX = lpObj->X;
	pjMsg.MapY = lpObj->Y;
	pjMsg.MapNumber = lpObj->MapNumber;
	pjMsg.Dir = lpObj->Dir;

	UINT64 Experience = lpObj->m_PlayerData->Experience;

	if (g_MasterLevelSkillTreeSystem.IsMasterLevelUser(*lpObj) != FALSE)
	{
		Experience = lpObj->m_PlayerData->MasterExperience;
	}

	pjMsg.ExpHHH = SET_NUMBERH(SET_NUMBERHW(HIDWORD(Experience)));
	pjMsg.ExpHHL = SET_NUMBERL(SET_NUMBERHW(HIDWORD(Experience)));
	pjMsg.ExpHLH = SET_NUMBERH(SET_NUMBERLW(HIDWORD(Experience)));
	pjMsg.ExpHLL = SET_NUMBERL(SET_NUMBERLW(HIDWORD(Experience)));
	pjMsg.ExpLHH = SET_NUMBERH(SET_NUMBERHW(LODWORD(Experience)));
	pjMsg.ExpLHL = SET_NUMBERL(SET_NUMBERHW(LODWORD(Experience)));
	pjMsg.ExpLLH = SET_NUMBERH(SET_NUMBERLW(LODWORD(Experience)));
	pjMsg.ExpLLL = SET_NUMBERL(SET_NUMBERLW(LODWORD(Experience)));

	UINT64 NextExperience = lpObj->m_PlayerData->NextExp;

	if (g_MasterLevelSkillTreeSystem.IsMasterLevelUser(*lpObj) != FALSE)
	{
		NextExperience = lpObj->m_PlayerData->MasterNextExp;
	}

	pjMsg.NextExpHHH = SET_NUMBERH(SET_NUMBERHW(HIDWORD(NextExperience)));
	pjMsg.NextExpHHL = SET_NUMBERL(SET_NUMBERHW(HIDWORD(NextExperience)));
	pjMsg.NextExpHLH = SET_NUMBERH(SET_NUMBERLW(HIDWORD(NextExperience)));
	pjMsg.NextExpHLL = SET_NUMBERL(SET_NUMBERLW(HIDWORD(NextExperience)));
	pjMsg.NextExpLHH = SET_NUMBERH(SET_NUMBERHW(LODWORD(NextExperience)));
	pjMsg.NextExpLHL = SET_NUMBERL(SET_NUMBERHW(LODWORD(NextExperience)));
	pjMsg.NextExpLLH = SET_NUMBERH(SET_NUMBERLW(LODWORD(NextExperience)));
	pjMsg.NextExpLLL = SET_NUMBERL(SET_NUMBERLW(LODWORD(NextExperience)));

	pjMsg.LevelUpPoint = lpObj->m_PlayerData->LevelUpPoint;
	pjMsg.Str = lpObj->m_PlayerData->Strength;
	pjMsg.Dex = lpObj->m_PlayerData->Dexterity;
	pjMsg.Vit = lpObj->m_PlayerData->Vitality;
	pjMsg.Energy = lpObj->m_PlayerData->Energy;
	pjMsg.Money = lpObj->m_PlayerData->Money;
	pjMsg.PkLevel = lpObj->m_PK_Level;
	pjMsg.Life = lpObj->Life;
	pjMsg.MaxLife = lpObj->AddLife + lpObj->MaxLife;
	pjMsg.Mana = lpObj->Mana;
	pjMsg.MaxMana = lpObj->AddMana + lpObj->MaxMana;
	pjMsg.wShield = lpObj->iShield;
	pjMsg.wMaxShield = lpObj->iMaxShield + lpObj->iAddShield;
	pjMsg.CtlCode = lpMsg->CtlCode;
	pjMsg.BP = lpObj->BP;
	pjMsg.MaxBP = lpObj->MaxBP + lpObj->AddBP;
	pjMsg.Leadership = lpObj->Leadership;
	pjMsg.InvExpansion = lpObj->m_PlayerData->m_InventoryExpansion;
	pjMsg.IGCLife = lpObj->Life;
	pjMsg.IGCMaxLife = lpObj->AddLife + lpObj->MaxLife;
	pjMsg.IGCMana = lpObj->Mana;
	pjMsg.IGCMaxMana = lpObj->AddMana + lpObj->MaxMana;
	pjMsg.Ruud = lpObj->m_PlayerData->Ruud;
	pjMsg.GetTick = GetTickCount();
	short AddPoint = 0;
	short MaxAddPoint = 0;
	short MinusPoint = 0;
	short MaxMinusPoint = 0;

	gObjGetStatPointState(*lpObj, AddPoint, MaxAddPoint, MinusPoint, MaxMinusPoint);
	pjMsg.AddPoint = AddPoint;
	pjMsg.MaxAddPoint = MaxAddPoint;
	pjMsg.wMinusPoint = MinusPoint;
	pjMsg.wMaxMinusPoint = MaxMinusPoint;

	if (AddPoint < 0 || AddPoint > MaxAddPoint || MinusPoint < 0 || MinusPoint > MaxMinusPoint)
	{
	}

	if (g_ArcaBattle.IsArcaBattleServer() == TRUE && g_AcheronGuardianEvent.IsPlayStart() == FALSE)
	{
		g_ArcaBattle.SendArcaBattlePlayInfo(*lpObj, 1);
		g_ArcaBattle.SendArcaBattleCurrentState(*lpObj);
	}

	if (g_ArcaBattle.IsArcaBattleServer() == TRUE && g_AcheronGuardianEvent.IsPlayStart() == TRUE)
	{
		g_ArcaBattle.SendArcaBattleCurrentState(*lpObj);
	}

	if (g_ArcaBattle.IsArcaBattleServer() == TRUE && g_AcheronGuardianEvent.IsPlayStart() == FALSE)
	{
		g_ArcaBattle.EnterArcaBattleEvent(*lpObj);
	}

	g_QuestInfo.QuestInfoSave(*lpObj);

	PMSG_RESET_INGAME_SEND pRSMsg = { 0 };
	pRSMsg.h.c = 0xC1;
	pRSMsg.h.headcode = 0xFA;
	pRSMsg.h.size = sizeof(pRSMsg);
	pRSMsg.h.subcode = 0x0B;
	pRSMsg.Resets = lpObj->m_PlayerData->m_iResets;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pRSMsg, pRSMsg.h.size);
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pjMsg, pjMsg.h.size);

	GCItemObjectListSend(Obj);
	FriendListRequest(Obj);

	g_CMuRummyMng.GDReqCardInfo(*lpObj);
	g_CMuunSystem.GDReqLoadMuunInvenItem(*lpObj);
	g_MasterLevelSkillTreeSystem.SendMLData(*lpObj);
	g_MasterLevelSkillTreeSystem.CGReqGetMasterLevelSkillTree(Obj);
	GSProtocol.GCMagicListMultiSend(lpObj, 0);
	GDReqLoadEventInvenItem(*lpObj);
	GSProtocol.CGRequestQuestInfo(Obj);
	g_PeriodItemEx.RequestPeriodItemInfo(*lpObj);
	g_LuckyItemManager.GDReqLuckyItemSelect(*lpObj);
	g_GensSystem.ReqExDBGensInfo(*lpObj);
	g_GensSystem.GDReqAbusingInfo(*lpObj);
	g_GensSystem.ReqGensRewardDay(*lpObj);
	g_BuffEffect.RequestPeriodBuffSelect(lpObj);
	g_PentagramSystem.DBREQ_GetPentagramJewel(*lpObj, lpMsg->AccountID, 0);
	g_PentagramSystem.DBREQ_GetPentagramJewel(*lpObj, lpMsg->AccountID, 1);
	g_PersonalStore.GDRequestPShopItemValue(*lpObj, lpMsg->AccountID);
	g_MineSystem.CheckIsUPTUserWhenConnect(Obj);
	g_GremoryCase.GDReqStorageItemList(Obj);
	g_SendNPCInfo.SendNpcCoordinate(Obj, lpObj->MapNumber, 0);
	g_SendNPCInfo.SendPortalCoordinate(Obj, lpObj->MapNumber);
	//[K2]
	g_UnityBattleField.GDReqCheckJoinedUnityBattleField(Obj, g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE ? TRUE : FALSE, 0);
	g_UnityBattleField.SendUBFNotice(Obj);
	g_EvoMonMng.GCSendEvoMonNotice(Obj);
	g_EvoMonMng.GDReqEvoMonMaxScore(Obj);
	DGGuildMemberInfoRequest(Obj);

	lpObj->m_PlayerData->m_dwMapMoveKeyValue = g_KeyGenerater.GenerateSeedValue();
	GSProtocol.GCSendMapMoveCheckSum(lpObj);

	GSProtocol.GCPlayerStatsPanelRates(lpObj->m_Index);
	GSProtocol.GCPlayerStatsPanelNew(lpObj->m_Index);

	if (g_ConfigRead.data.common.EnterGameMessage == 1)
	{
		PMSG_CHATDATA_WHISPER pMsg;
		PHeadSetB((BYTE*)&pMsg, 0x02, sizeof(pMsg));

		std::memset(pMsg.chatmsg, 0x00, sizeof(pMsg.chatmsg));
		std::memcpy(pMsg.id, lpObj->Name, sizeof(pMsg.id));
		std::strcpy(pMsg.chatmsg, Lang.GetText(0, 544));

		pMsg.h.size -= sizeof(pMsg.chatmsg);
		pMsg.h.size += strlen(Lang.GetText(0, 544)) + 1;

		GSProtocol.DataSendAll((BYTE*)&pMsg, pMsg.h.size);
	}

	if (lpObj->m_PlayerData->VipType > 0)
	{
		MsgOutput(Obj, Lang.GetText(0, 493), g_VipSystem.GetVipName(lpObj->m_PlayerData->VipType));
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_NORMAL)
	{
		GSProtocol.GCServerMsgStringSend((char *)g_ConfigRead.data.common.WelcomeMessage.c_str(), lpObj->m_Index, 0);
	}

	gObjUseSkill.SkillChangeUse(*lpObj);

	GSProtocol.GCReFillSend(Obj, lpObj->Life, 0xFF, 0, lpObj->iShield);
	GSProtocol.GCReFillSend(Obj, lpObj->AddLife + lpObj->MaxLife, 0xFE, 0, lpObj->iAddShield + lpObj->iMaxShield);

	GSProtocol.GCManaSend(Obj, lpObj->Mana, 0xFF, 0, lpObj->BP);
	GSProtocol.GCManaSend(Obj, lpObj->MaxMana, 0xFE, 0, lpObj->MaxBP);

	GSProtocol.GCResSendExpEventInfo(Obj);
	g_QuestExpUserMng.DB_ReqUserQuestInfo(Obj);

	PMSG_SETAGILITYBUG pAgilityBug;
	pAgilityBug.h.c = 0xC1;
	pAgilityBug.h.headcode = 0xFA;
	pAgilityBug.h.size = sizeof(pAgilityBug);
	pAgilityBug.subcode = 0xA2;

	if (lpObj->Class == CLASS_RAGEFIGHTER || lpObj->Class == CLASS_ELF)
	{
		pAgilityBug.value = 0x0F;
	}
	else
	{
		pAgilityBug.value = 0x02; ///[K2]
	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pAgilityBug, sizeof(pAgilityBug));

	//if (g_ConfigRead.data.common.LuckyItemAllowUseJewels == true)
	{
		BYTE dataenable[] = { 0xC1, 0x04, 0xFA, 0xA4 };
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, dataenable, sizeof(dataenable));
	}

	if (g_ConfigRead.data.common.joinmuRemoveItemSellDropRestriction == true)
	{
		BYTE dataenable[] = { 0xC1, 0x04, 0xFA, 0xB0 };
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, dataenable, sizeof(dataenable));
	}

	//BYTE TEST[] = { 0xC1, 0x06, 0x6D, 0x00, 0x01, 0x00 }; //Help Icon
	//IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, TEST, sizeof(TEST));

	GSProtocol.GCSendRareItemPriceInfo(Obj);
	g_StatSpec.SendOptionList(lpObj);

	if (g_ConfigRead.data.common.bReconnectSystem == 0)
	{
		GSProtocol.GCSendDisableReconnectSystem(Obj);
	}

	for (int n = 0; n < INVENTORY_SIZE; n++)
	{
		if (lpObj->pntInventory[n]->IsPeriodItemExpire() == 1)
		{
			lpObj->pntInventory[n]->SetPeriodItemExpire();
			g_PeriodItemEx.RemovePeriodItemDataByForce(*lpObj, lpObj->pntInventory[n]->m_Type, lpObj->pntInventory[n]->m_Number);
			lpObj->pntInventory[n]->Clear();
		}
	}
}

void GCItemObjectListSend(CGameObject &Obj)
{
	PMSG_INVENTORYLISTCOUNT pMsgILC;	// Packet Message Item List Count
	PMSG_INVENTORYLIST pMsgIL;	// Packet Message Item List
	int sOfs = sizeof(PMSG_INVENTORYLISTCOUNT);
	int pMsgILSize = sizeof(PMSG_INVENTORYLIST);
	BYTE sendBuf[4096];
	LPOBJ lpObj = &gObj[Obj];
	int itemcount = 0;

	for (int n = 0; n < INVENTORY_SIZE; n++)
	{
		if (lpObj->pntInventory[n]->IsItem() == TRUE)
		{
			if (lpObj->pntInventory[n]->m_bItemExist == false)
			{
				continue;
			}

			pMsgIL.Pos = n;
			ItemByteConvert(pMsgIL.ItemInfo, lpObj->pntInventory[n]);
			std::memcpy(&sendBuf[sOfs], &pMsgIL, pMsgILSize);
			itemcount++;
			sOfs += pMsgILSize;
		}
	}

	pMsgILC.h.c = 0xC4;
	pMsgILC.h.headcode = 0xF3;
	pMsgILC.subcode = 0x10;
	pMsgILC.h.sizeH = SET_NUMBERH(sOfs);
	pMsgILC.h.sizeL = SET_NUMBERL(sOfs);
	pMsgILC.Count = itemcount;
	std::memcpy(sendBuf, &pMsgILC, sizeof(PMSG_INVENTORYLISTCOUNT));

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, sendBuf, sOfs);
}

void GJSetCharacterInfo(CGameObject &Obj, BOOL bMapServerMove)
{
	if (Obj.m_bMapSvrMoveQuit == true)
	{
		sLog->outError("[MapServerMng] GJSetCharacterInfo() - Inventory Already Saved [%s][%s] (%d)",
			Obj.AccountID, Obj.Name, Obj.m_Index);

		return;
	}

	if (bMapServerMove == TRUE)
	{
		Obj.m_bMapSvrMoveQuit = TRUE;

		sLog->outBasic("[MapServerMng][%s][%s] GJSetCharacterInfo() - MapServerMove Character Save Start (%d)",
			Obj.AccountID, Obj.Name, Obj.m_Index);
	}

	if (Obj.Type == OBJ_USER && Obj.m_btOpenWarehouse == TRUE)
	{
		GDSetWarehouseList(Obj, FALSE);
	}

	SDHP_DBCHAR_INFOSAVE pCSave;	// Packet Character Save
	pCSave.h.c = 0xC2;
	pCSave.h.headcode = 0x07;
	pCSave.h.sizeH = sizeof(SDHP_DBCHAR_INFOSAVE) >> 8;
	pCSave.h.sizeL = sizeof(SDHP_DBCHAR_INFOSAVE) & 0xFF;
	pCSave.Name[MAX_ACCOUNT_LEN] = 0;
	std::memcpy(pCSave.AccountID, Obj.AccountID, sizeof(pCSave.AccountID) - 1);
	std::memcpy(pCSave.Name, Obj.Name, sizeof(pCSave.Name) - 1);
	pCSave.Class = Obj.m_PlayerData->DbClass;
	pCSave.Level = Obj.Level;
	pCSave.mLevel = Obj.m_PlayerData->MasterLevel;
	pCSave.LevelUpPoint = Obj.m_PlayerData->LevelUpPoint;
	pCSave.mlPoint = Obj.m_PlayerData->MasterPoint;
	pCSave.Exp = Obj.m_PlayerData->Experience;
	pCSave.mlExp = Obj.m_PlayerData->MasterExperience;
	pCSave.NextExp = Obj.m_PlayerData->NextExp;
	pCSave.mlNextExp = Obj.m_PlayerData->MasterNextExp;
	pCSave.Money = Obj.m_PlayerData->Money;
	pCSave.Str = Obj.m_PlayerData->Strength;
	pCSave.Dex = Obj.m_PlayerData->Dexterity;
	pCSave.Vit = Obj.m_PlayerData->Vitality;
	pCSave.Energy = Obj.m_PlayerData->Energy;
	pCSave.Life = Obj.Life;
	pCSave.MaxLife = Obj.MaxLife;
	pCSave.Mana = Obj.Mana;
	pCSave.MaxMana = Obj.MaxMana;
	ItemByteConvert32((BYTE*)pCSave.dbInventory, *Obj.pInventory, INVENTORY_SIZE);
	MagicByteConvert(pCSave.dbMagicList, Obj.Magic, MAGIC_SIZE);
	pCSave.MapNumber = Obj.MapNumber;
	pCSave.MapX = Obj.X;
	pCSave.MapY = Obj.Y;
	pCSave.Dir = Obj.Dir;
	pCSave.PkCount = Obj.m_PK_Count;
	pCSave.PkLevel = Obj.m_PK_Level;
	pCSave.PkTime = Obj.m_PK_Time;
	pCSave.Leadership = Obj.Leadership;
	pCSave.ChatLitmitTime = Obj.ChatLimitTime;
	pCSave.iFruitPoint = Obj.m_PlayerData->iFruitPoint;
	pCSave.CharInfoSave = 0;
	pCSave.resets = Obj.m_PlayerData->m_iResets;
	std::memcpy(pCSave.dbQuest, Obj.m_PlayerData->m_Quest, sizeof(pCSave.dbQuest));
	std::memcpy(pCSave.MarryName, Obj.MarryName, sizeof(pCSave.MarryName));
	pCSave.Married = Obj.Married;
	pCSave.WinDuels = Obj.WinDuels;
	pCSave.LoseDuels = Obj.LoseDuels;
	pCSave.ChatBlockTime = Obj.ChatBlockTime;
	pCSave.PenaltyMask = Obj.PenaltyMask;
	//pCSave.Ruud = Obj.EventMap;
	pCSave.Ruud = Obj.m_PlayerData->Ruud;
	pCSave.EventMap = Obj.EventMap;
	if (wsDataCli.DataSend((char*)&pCSave, sizeof(SDHP_DBCHAR_INFOSAVE)) == FALSE)
	{
		sLog->outError("%s Character data save fail", Obj.Name);
		return;
	}

	gObjItemTextSave(Obj);
	gObjStatTextSave(Obj);
	gObjSavePetItemInfo(Obj, 0);
	g_LuckyItemManager.GDReqLuckyItemInsert2nd(Obj);
	g_QuestExpUserMng.UserAllQuestInfoSave(Obj);
	g_GensSystem.GDReqSaveContributePoint(Obj);
	g_GensSystem.DBSaveAbusingKillUserName(Obj);
	g_PentagramSystem.DBREQ_SetPentagramJewel(Obj, 0);
	g_PentagramSystem.DBREQ_SetPentagramJewel(Obj, 1);
	GDReqSaveEventInvenItem(Obj);
	g_CMuunSystem.GDReqSaveMuunInvenItem(Obj);

}



/* * * * * * * * * * * * * * * * * * * * *
*	Mu Get Warehouse Info Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  : 0x08
*/
struct SDHP_GETWAREHOUSEDB
{
	PBMSG_HEAD h;	// C1:08
	char AccountID[11];	// 3
	int iUserIndex;	// E
};

void GDGetWarehouseList(CGameObject &Obj, char* AccountID)
{
	if (ObjectMaxRange(Obj) == FALSE)
	{
		return;
	}

	if (strlen(AccountID) < 1)
	{
		return;
	}

	if (lpObj->m_bMapSvrMoveReq == true)
	{
		sLog->outBasic("[GetWarehouseList] MapServerMove User. Can't Open WhareHouse. return!! [%s], IP [%s] ",
			AccountID, lpObj->m_PlayerData->ConnectUser->IP);
		return;
	}

	if (lpObj->m_State == 32)
	{
		sLog->outBasic("[GetWarehouseList] MapServerMove User. Can't Open WhareHouse. return!! [%s], IP [%s] ",
			AccountID, lpObj->m_PlayerData->ConnectUser->IP);
		return;
	}

	if (lpObj->m_bMapSvrMoveQuit == true)
	{
		sLog->outBasic("[GetWarehouseList] MapServerMove User. Can't Open WhareHouse. return!! [%s], IP [%s] ",
			AccountID, lpObj->m_PlayerData->ConnectUser->IP);
		return;
	}

	SDHP_GETWAREHOUSEDB	pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x08;
	pMsg.h.size = sizeof(SDHP_GETWAREHOUSEDB);
	pMsg.iUserIndex = Obj.m_Index;
	std::memcpy(pMsg.AccountID, AccountID, sizeof(pMsg.AccountID));

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}




void DGGetWarehouseList(SDHP_GETWAREHOUSEDB_SAVE * lpMsg)
{
	char szId[11];
	CGameObject* lpObj = getGameObject(lpMsg->aIndex);
	PMSG_TALKRESULT pResult;

	szId[MAX_ACCOUNT_LEN] = 0;
	std::memcpy(szId, lpMsg->AccountID, sizeof(lpMsg->AccountID));

	if (lpObj->m_bMapSvrMoveReq == true)
	{
		sLog->outBasic("[DGGetWarehouseList] MapServerMove User. Can't Open WhareHouse. return!! [%s], IP [%s] ",
			szId, lpObj->m_PlayerData->ConnectUser->IP);
		return;
	}

	if (lpObj->m_State == 32)
	{
		sLog->outBasic("[DGGetWarehouseList] MapServerMove User. Can't Open WhareHouse. return!! [%s], IP [%s] ",
			szId, lpObj->m_PlayerData->ConnectUser->IP);
		return;
	}

	if (lpObj->m_bMapSvrMoveQuit == true)
	{
		sLog->outBasic("[DGGetWarehouseList] MapServerMove User. Can't Open WhareHouse. return!! [%s], IP [%s] ",
			szId, lpObj->m_PlayerData->ConnectUser->IP);
		return;
	}

	if (!gObjIsAccontConnect(Obj, szId))
	{
		sLog->outError("Error-L1 : Request to receive Warehouse information doesn't match the user [%s][%d]", szId, Obj);
		return;
	}

	int itype;
	int _type;
	BYTE OptionData;
	CItemObject* item;

	std::memset(lpObj->WarehouseMap, (BYTE)-1, WAREHOUSE_SIZE);
	lpObj->WarehouseMoney = lpMsg->Money;
	lpObj->WarehousePW = lpMsg->pw;

	int count = 0;
	for (int n = 0; n < WAREHOUSE_SIZE; n++)
	{
		itype = lpMsg->dbItems[n*MAX_DBITEM_INFO];

		itype |= (lpMsg->dbItems[n*MAX_DBITEM_INFO + 9] & 0xF0) << 5;
		itype |= (lpMsg->dbItems[n*MAX_DBITEM_INFO + 7] & 0x80) << 1;
		_type = itype;

		lpObj->pntWarehouse[n]->Clear();

		if (lpMsg->dbItems[n*MAX_DBITEM_INFO] == (BYTE)-1 && (lpMsg->dbItems[n*MAX_DBITEM_INFO + 9] & 0xF0) == 0xF0 && (lpMsg->dbItems[n*MAX_DBITEM_INFO + 7] & 0x80) == 0x80)
		{
			itype = -1;
		}

		if (IsItem(_type) == FALSE)
		{
			itype = -1;
		}

		if (_type == ITEMGET(14, 17) || _type == ITEMGET(14, 18))
		{
			if (lpMsg->dbItems[n*MAX_DBITEM_INFO + 2] == 0)	// Level 0
			{
				_type = -1;	// #error maybe this is itype
			}
		}

		if (_type == ITEMGET(14, 19))
		{
			if (lpMsg->dbItems[n*MAX_DBITEM_INFO + 2] == (BYTE)-1)
			{
				itype = -1;
			}
		}

		if (itype != -1)
		{
			item->m_Level = DBI_GET_LEVEL(lpMsg->dbItems[n*MAX_DBITEM_INFO + 1]);

			if (_type != ITEMGET(14, 11))	// Box Of Luck
			{
				if (item->m_Level > 15)
				{
					item->m_Level = 0;
				}
			}

			OptionData = lpMsg->dbItems[n*MAX_DBITEM_INFO + 1];
			item->m_Option1 = DBI_GET_SKILL(OptionData);
			item->m_Option2 = DBI_GET_LUCK(OptionData);
			item->m_Option3 = DBI_GET_OPTION(OptionData);
			item->m_Option3 |= DBI_GET_DINO_OPTION(lpMsg->dbItems[n*MAX_DBITEM_INFO + 7]);
			item->m_Durability = lpMsg->dbItems[n*MAX_DBITEM_INFO + 2];
			item->m_JewelOfHarmonyOption = lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_JOH_DATA];
			item->m_ItemOptionEx = DBI_GET_380OPTION(lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_OPTION380_DATA]);

			if (item->m_ItemOptionEx != 0)
			{
				item->m_Type = itype;
				if (g_kItemSystemFor380.Is380Item(&item) == FALSE)
				{
					item->m_ItemOptionEx = 0;
				}
			}

			BYTE SocketOption[5];
			BYTE SocketOptionIndex = 0xFF;
			int i = 0;
			std::memset(&SocketOption, 0xFF, 5);

			if (gSocketItemType.CheckSocketItemType(itype) == TRUE)
			{
				SocketOptionIndex = lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_JOH_DATA];

				for (i = 0; i < 5; i++)
				{
					SocketOption[i] = lpMsg->dbItems[((n*MAX_DBITEM_INFO) + DBI_SOCKET_1) + i];
				}
			}

			else if (g_PentagramSystem.IsPentagramItem(itype) == TRUE ||
				g_PentagramSystem.IsPentagramJewel(itype) == TRUE)
			{
				SocketOptionIndex = lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_JOH_DATA];

				for (i = 0; i < 5; i++)
				{
					SocketOption[i] = lpMsg->dbItems[((n*MAX_DBITEM_INFO) + DBI_SOCKET_1) + i];
				}
			}

			else
			{
				SocketOptionIndex = 0;

				for (i = 0; i < 5; i++)
				{
					SocketOption[i] = 0xFF;
				}
			}

			item->m_PeriodItemOption = (lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_OPTION380_DATA] & 6) >> 1;

			item->Convert(itype, item->m_Option1, item->m_Option2, item->m_Option3, DBI_GET_NOPTION(lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_NOPTION_DATA]), lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SOPTION_DATA], item->m_ItemOptionEx, SocketOption, SocketOptionIndex, item->m_PeriodItemOption, CURRENT_DB_VERSION);

			if (_type == ITEMGET(14, 7)) // Siege Potion
			{

			}

			else if ((_type >= ITEMGET(14, 0) && _type <= ITEMGET(14, 8)) || (_type >= ITEMGET(14, 35) && _type <= ITEMGET(14, 40)))
			{
				if (item->m_Durability == 0.0f)
				{
					item->m_Durability = 1.0f;
				}
			}
			else if (_type != ITEMGET(13, 10) && _type != ITEMGET(14, 29) && _type != ITEMGET(14, 21) && item->m_Level == 3)
			{
				if (item->m_Durability > item->m_BaseDurability)
				{
					item->m_Durability = item->m_BaseDurability;
				}
			}

			int iMaxOverlap = IsOverlapItem(item->m_Type);

			if (iMaxOverlap != 0)
			{
				if (item->m_Durability > iMaxOverlap)
				{
					item->m_Durability = iMaxOverlap;
				}
			}

			lpObj->pntWarehouse[n]->m_Option1 = item->m_Option1;
			lpObj->pntWarehouse[n]->m_Option2 = item->m_Option2;
			lpObj->pntWarehouse[n]->m_Option3 = item->m_Option3;
			lpObj->pntWarehouse[n]->m_JewelOfHarmonyOption = item->m_JewelOfHarmonyOption;
			lpObj->pntWarehouse[n]->m_ItemOptionEx = item->m_ItemOptionEx;

			DWORD hidword = MAKE_NUMBERDW(MAKE_NUMBERW(lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL1], lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL2]), MAKE_NUMBERW(lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL3], lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL4]));
			DWORD lodword = MAKE_NUMBERDW(MAKE_NUMBERW(lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL5], lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL6]), MAKE_NUMBERW(lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL7], lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL8]));
			item->m_Number = MAKEQWORD(hidword, lodword);

			gObjWarehouseInsertItemPos(Obj, *item, n, -1);
			gObjWarehouseItemSet(Obj, n, 1);

			count++;
		}
	}

	pResult.h.c = 0xC3;
	pResult.h.headcode = 0x30;
	pResult.h.size = sizeof(PMSG_TALKRESULT);
	pResult.result = 2;

	IOCP.DataSend(lpObj->m_Index, (BYTE*)&pResult, pResult.h.size);
	GSProtocol.GCUserWarehouseSend(&gObj[Obj]);

	if (bCanWarehouseLock == TRUE)
	{
		if (lpObj->WarehousePW > 0)
		{
			if (lpObj->WarehouseLock == 0)
			{
				GSProtocol.GCWarehouseStateSend(Obj, 12);
			}
			else
			{
				GSProtocol.GCWarehouseStateSend(Obj, 1);
			}
		}
		else
		{
			GSProtocol.GCWarehouseStateSend(Obj, 0);
		}
	}

	lpObj->LoadWareHouseInfo = true;
	lpObj->m_PlayerData->m_ReqWarehouseOpen = false;
	gObjRequestPetItemInfo(*lpObj, 1);
	bool bCheck = false;
	Check_SameSerialItem(Obj, 1, bCheck);
}

void GDGetWarehouseNoItem(SDHP_GETWAREHOUSEDB_RESULT * lpMsg)
{
	char szId[11];
	CGameObject* lpObj = getGameObject(lpMsg->aIndex);
	PMSG_TALKRESULT pResult;

	szId[MAX_ACCOUNT_LEN] = 0;
	std::memcpy(szId, lpMsg->AccountID, sizeof(lpMsg->AccountID));

	if (!gObjIsAccontConnect(Obj, szId))
	{
		sLog->outError("Error-L1 : Request to receive Warehouse information doesn't match the user [%s][%d]", szId, Obj);
		return;
	}

	LPOBJ lpObj = &gObj[Obj];

	if (lpObj->m_IfState->type == 6)
	{
		if (lpObj->m_IfState->state == 0)
		{
			lpObj->m_IfState->state = 1;
			pResult.h.c = 0xC3;
			pResult.h.headcode = 0x30;
			pResult.h.size = sizeof(pResult);
			pResult.result = 2;

			IOCP.DataSend(lpObj->m_Index, (BYTE*)&pResult, pResult.h.size);
			sLog->outError("Warehouse item not found [%s]", lpObj->Name);

			PMSG_MONEY pMsg; // Season 4.5 fix

			PHeadSetB((BYTE*)&pMsg, 0x81, sizeof(pMsg));
			pMsg.Result = 1;
			pMsg.iMoney = lpObj->m_PlayerData->Money;
			pMsg.wMoney = lpObj->WarehouseMoney;

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		}
	}

	lpObj->LoadWareHouseInfo = true;
	lpObj->m_PlayerData->m_ReqWarehouseOpen = false;
	lpObj->m_btOpenWarehouse = TRUE;
}

void GDSetWarehouseList(CGameObject &Obj, BOOL bCloseWindow)
{
	SDHP_GETWAREHOUSEDB_SAVE pMsg;

	if (lpObj->LoadWareHouseInfo == false)
	{
		sLog->outBasic("[%s][%s] WareHouse Save Fail : Not Open",
			lpObj->AccountID, lpObj->Name);

		return;
	}

	std::memcpy(pMsg.AccountID, lpObj->AccountID, sizeof(pMsg.AccountID));
	pMsg.aIndex = Obj.m_Index;
	pMsg.h.c = 0xC2;
	pMsg.h.headcode = 0x09;
	pMsg.h.sizeH = HIBYTE(sizeof(pMsg));
	pMsg.h.sizeL = LOBYTE(sizeof(pMsg));
	pMsg.Money = lpObj->WarehouseMoney;
	pMsg.pw = lpObj->WarehousePW;
	pMsg.WarehouseID = lpObj->WarehouseID;
	pMsg.CloseWindow = bCloseWindow;

	ItemByteConvert32(pMsg.dbItems, lpObj->pWarehouse, WAREHOUSE_SIZE);

	wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg));

	lpObj->WarehouseSaveLock = TRUE;

	gObjSavePetItemInfo(Obj, 1);
	gObjWarehouseTextSave(&gObj[Obj]);
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu DB Char Item Save Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC2
*	HeadCode  : 0x11
*/
struct SDHP_DBCHAR_ITEMSAVE
{
	PWMSG_HEAD h;	// C2:11
	char Name[11];	// 4
	BYTE dbInventory[7584];	// 34
};

void GDUserItemSave(LPOBJ lpObj)
{
	SDHP_DBCHAR_ITEMSAVE pMsg;

	pMsg.h.c = 0xC2;
	pMsg.h.headcode = 0x11;
	pMsg.h.sizeH = HIBYTE(sizeof(pMsg));
	pMsg.h.sizeL = LOBYTE(sizeof(pMsg));
	std::memcpy(pMsg.Name, lpObj->Name, sizeof(pMsg.Name));
	ItemByteConvert32(pMsg.dbInventory, lpObj->pInventory, INVENTORY_SIZE);

	wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg));
}



/* * * * * * * * * * * * * * * * * * * * *
*	Mu Warehouse Money Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  : 0x12
*/
struct SDHP_WAREHOUSEMONEY_SAVE
{
	PBMSG_HEAD h;	// C1:12
	char AccountID[11];	// 3
	int iUserIndex;	// E
	int Money;	// 10
};

void GDSetWarehouseMoney(CGameObject &Obj)
{
	SDHP_WAREHOUSEMONEY_SAVE pMsg;

	std::memcpy(pMsg.AccountID, Obj.AccountID, sizeof(pMsg.AccountID));
	pMsg.iUserIndex = Obj.m_Index;
	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x12;
	pMsg.h.size = sizeof(pMsg);
	pMsg.Money = Obj.WarehouseMoney;

	wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg));
}

void GDGameServerInfoSave()
{
	return;		// GHere goes a MACRO
}



/* * * * * * * * * * * * * * * * * * * * *
*	Mu Item Serial Create Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  : 0x52, 0x55 (in Case of PET )
*/
struct SDHP_ITEMCREATE
{
	PBMSG_HEAD h;	// C1:XX
	BYTE x;	// 3
	BYTE y;	// 4
	BYTE MapNumber;	// 5 [235:Mu_2nd_Aniv], [236:CashShop]
	short Type;	// 6
	BYTE Level;	// 8
	BYTE Dur;	// 9
	BYTE Op1;	// A
	BYTE Op2;	// B
	BYTE Op3;	// C
	BYTE NewOption;	// D
	int iUserIndex;	// 10
	int lootindex;	// 14
	BYTE SetOption;	// 16
	time_t lDuration;
	BYTE SocketOption[5];
	BYTE MainAttribute;
};

// MapNumber [235:Mu_2nd_Aniv], [236:CashShop]
void ItemCreate(CGameObject &Obj, BYTE MapNumber, BYTE x, BYTE y, int type, BYTE level, BYTE dur, BYTE Op1, BYTE Op2, BYTE Op3, int LootIndex, BYTE NewOption, BYTE SetOption, time_t lDuration, BYTE *SocketOption, BYTE MainAttribute)
{
	SDHP_ITEMCREATE  pMsg;
	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x52;
	pMsg.h.size = sizeof(pMsg);
	pMsg.MapNumber = MapNumber;
	pMsg.x = x;
	pMsg.y = y;
	pMsg.Type = type;
	pMsg.Level = level;

#if (ENABLE_CUSTOM_ITEMDROPEX == 1)
	if (g_DropEx.IsBlockItem(type))
	{
		if (MapNumber >= 0 && MapNumber <= MAX_NUMBER_MAP)
		{
			return;
		}
	}
#endif

	if (dur == 0)
	{
		dur = ItemGetDurability(type, level, NewOption, SetOption);
	}

	if (IsItem(type) == 0)
	{
		return;
	}

	pMsg.Dur = dur;
	pMsg.Op1 = Op1;
	pMsg.Op2 = Op2;
	pMsg.Op3 = Op3;
	pMsg.NewOption = NewOption;
	pMsg.iUserIndex = Obj.m_Index;
	pMsg.lootindex = LootIndex;
	pMsg.SetOption = SetOption;
	pMsg.lDuration = lDuration;

	if (SocketOption)
	{
		for (int i = 0; i < MAX_SOCKET_OPTION; i++)
		{
			pMsg.SocketOption[i] = SocketOption[i];
		}
	}

	else
	{
		for (int i = 0; i < MAX_SOCKET_OPTION; i++)
		{
			pMsg.SocketOption[i] = 0xFF;
		}
	}

	pMsg.MainAttribute = MainAttribute;

	if (LootIndex != -1 && gObj[LootIndex].Type == OBJ_USER)
	{
		if (g_ConfigRead.data.common.joinmuDropItemUnderCharacter[gObj[LootIndex].Class] == true)
		{
			pMsg.x = gObj[LootIndex].X;
			pMsg.y = gObj[LootIndex].Y;
		}
	}

	if (ObjectMaxRange(Obj) != FALSE && lpObj->Type != OBJ_USER)
	{
		BYTE NewOption[MAX_EXOPTION_SIZE];

		for (int i = 0; i < MAX_EXOPTION_SIZE; i++)
		{
			NewOption[i] = 0;
		}

		if ((pMsg.NewOption & 0x20) != 0)
		{
			NewOption[0] = TRUE;
		}
		if ((pMsg.NewOption & 0x10) != 0)
		{
			NewOption[1] = TRUE;
		}

		if ((pMsg.NewOption & 0x8) != 0)
		{
			NewOption[2] = TRUE;
		}
		if ((pMsg.NewOption & 0x4) != 0)
		{
			NewOption[3] = TRUE;
		}
		if ((pMsg.NewOption & 0x2) != 0)
		{
			NewOption[4] = TRUE;
		}
		if ((pMsg.NewOption & 0x1) != 0)
		{
			NewOption[5] = TRUE;
		}

	}

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

void PetItemCreate(CGameObject &Obj, BYTE MapNumber, BYTE x, BYTE y, int type, BYTE level, BYTE dur, BYTE Op1, BYTE Op2, BYTE Op3, int LootIndex, BYTE NewOption, BYTE SetOption)
{
	SDHP_ITEMCREATE  pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x55;
	pMsg.h.size = sizeof(pMsg);
	pMsg.MapNumber = MapNumber;
	pMsg.x = x;
	pMsg.y = y;
	pMsg.Type = type;
	pMsg.Level = level;

	if (dur == 0)
	{
		dur = ItemGetDurability(type, level, NewOption, SetOption);
	}

	pMsg.Dur = dur;
	pMsg.Op1 = Op1;
	pMsg.Op2 = Op2;
	pMsg.Op3 = Op3;
	pMsg.NewOption = NewOption;
	pMsg.iUserIndex = Obj.m_Index;
	pMsg.lootindex = LootIndex;
	pMsg.SetOption = SetOption;
	pMsg.lDuration = 0;
	pMsg.MainAttribute = 0;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

void ItemSerialCreateRecv(SDHP_ITEMCREATERECV * lpMsg)
{
	int mapnumber = lpMsg->MapNumber;
	CGameObject* lpObj = getGameObject(lpMsg->iUserIndex);
	int lootindex = lpMsg->lootindex;

	if (mapnumber != (BYTE)-1)
	{
		if (mapnumber < 0 || (mapnumber > MAX_NUMBER_MAP - 1 && mapnumber < 217))
		{
			return;
		}
	}

	if (lpMsg->MapNumber == (BYTE)-1 || lpMsg->MapNumber == (BYTE)-2)
	{
		if (gObjIsConnectedGP(Obj) == FALSE)
		{
			return;
		}

		lpObj = &gObj[Obj];

		if (lpMsg->MapNumber == (BYTE)-2)
		{
			if (lpObj->m_IfState->type != 13)
			{
				return;
			}
		}
		else if (lpObj->m_IfState->type != 7)
		{
			return;
		}

		PMSG_CHAOSMIXRESULT pMsg;

		pMsg.h.c = 0xC1;
		pMsg.h.headcode = 0x86;
		pMsg.h.size = sizeof(PMSG_CHAOSMIXRESULT);

		if (lpMsg->MapNumber == (BYTE)-2)
		{
			pMsg.Result = 100;
		}
		else
		{
			pMsg.Result = 1;
		}

		CItemObject NewItem;

		NewItem.m_Type = lpMsg->Type;
		NewItem.m_Level = lpMsg->Level;
		NewItem.m_Durability = ItemGetDurability(lpMsg->Type, lpMsg->Level, lpMsg->NewOption, lpMsg->SetOption);

		if (lpMsg->Type == ITEMGET(14, 7)) // Siege Potion
		{
			NewItem.m_Durability = lpMsg->Dur;
		}

		if (lpMsg->Type == ITEMGET(13, 37))
		{
			NewItem.m_Durability = lpMsg->Dur;
		}

		BYTE SocketOption[5];
		int SocketSlotCount = 0;
		int SocketBonusOption = 0;

		if (gSocketItemType.CheckSocketItemType((int)&NewItem) == true)
		{
			SocketSlotCount = g_SocketOptionSystem.GetMakeSocketSlotCount(lpMsg->Type);
			SocketBonusOption = -1;

			for (int i = 0; i < 5; i++)
			{
				if (i < SocketSlotCount)
				{
					SocketOption[i] = 0xFE;
				}
				else
				{
					SocketOption[i] = 0xFF;
				}
			}
		}

		else
		{
			SocketBonusOption = 0;

			if (memcmp(lpMsg->SocketOption, SocketOption, 5) == 0)
			{
				for (int i = 0; i < 5; i++)
				{
					SocketOption[i] = 0xFF;
				}
			}

			else
			{
				for (int i = 0; i < 5; i++)
				{
					SocketOption[i] = 0xFF;
				}
			}
		}

		NewItem.Convert(lpMsg->Type, lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, SocketOption, SocketBonusOption, 0, CURRENT_DB_VERSION);
		ItemByteConvert(pMsg.ItemInfo, NewItem);
		NewItem.m_Number = lpMsg->m_Number;

		if (lpObj->ChaosMassMixCurrItem == 0)
		{
			g_MixSystem.ChaosBoxInit(*lpObj);
		}

		BYTE btPos = gObjChaosBoxInsertItemTemp(*lpObj, &NewItem);

		if (btPos == 0xFF)
		{
			pMsg.Result = CB_ERROR;

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
			lpObj->ChaosLock = FALSE;

			return;
		}
		;
		gObjChaosBoxInsertItemPos(Obj, NewItem, btPos, -1);
		gObjChaosItemSet(Obj, btPos, 1);
		BYTE ExOption[MAX_EXOPTION_SIZE];
		ItemIsBufExOption(ExOption, &NewItem);

		pMsg.Pos = btPos;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

		lpObj->ChaosMassMixCurrItem++;

		if (lpObj->ChaosMassMixCurrItem >= lpObj->ChaosMassMixSuccessCount)
		{
			lpObj->ChaosMassMixCurrItem = 0;
			lpObj->ChaosMassMixSuccessCount = 0;

			GJSetCharacterInfo(Obj, 0);
			lpObj->ChaosLock = FALSE;
		}

		return;
	}

	if (lpMsg->MapNumber == 230)
	{
		CItemObject Item;
		int iType = ITEM_GET_TYPE(lpMsg->Type);
		int iTypeIndex = ITEM_GET_INDEX(lpMsg->Type);
		Item.m_Level = lpMsg->Level;
		Item.m_Durability = lpMsg->Dur;
		Item.m_Number = lpMsg->m_Number;
		Item.m_Type = lpMsg->Type;

		Item.Convert(ITEMGET(iType, iTypeIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, lpMsg->MainAttribute, 0, CURRENT_DB_VERSION);

		PMSG_CHAOSMIXRESULT pMsg;
		PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
		pMsg.Result = CB_ERROR;
		ItemByteConvert(pMsg.ItemInfo, Item);

		BYTE btPos = gObjPentagramMixBoxInsertItem(Obj, Item);

		if (btPos == 0xFF)
		{
			pMsg.Result = CB_ERROR;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
			lpObj->m_PlayerData->m_PentagramMixLock = FALSE;
		}

		else
		{
			pMsg.Result = CB_SUCCESS;
			gObjPentagramMixItemSet(Obj, btPos, 1);
			BYTE ExOption[MAX_EXOPTION_SIZE];
			ItemIsBufExOption(ExOption, &Item);

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
			lpObj->m_PlayerData->m_PentagramMixLock = FALSE;
		}
	}

	else if (lpMsg->MapNumber == 231)
	{
		CItemObject Item;
		int iType = ITEM_GET_TYPE(lpMsg->Type);
		int iTypeIndex = ITEM_GET_INDEX(lpMsg->Type);
		Item.m_Level = lpMsg->Level;
		Item.m_Durability = lpMsg->Dur;
		Item.m_Number = lpMsg->m_Number;
		Item.m_Type = lpMsg->Type;

		Item.Convert(ITEMGET(iType, iTypeIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, lpMsg->MainAttribute, 0, CURRENT_DB_VERSION);
		BYTE btInOutResult = FALSE;

		BYTE btRes = gObjInventoryInsertItem(Obj, Item);

		if (btRes == 0xFF)
		{
			GSProtocol.GCAnsPentagramJewelInOut(Obj, btInOutResult);
		}
		else
		{
			GSProtocol.GCInventoryItemOneSend(Obj, btRes);
			GSProtocol.GCAnsPentagramJewelInOut(Obj, 1);
			BYTE ExOption[MAX_EXOPTION_SIZE];
			ItemIsBufExOption(ExOption, &Item);

		}
	}

	else if (lpMsg->MapNumber == 233)
	{
		if (lpObj->Connected > PLAYER_CONNECTED)
		{
			CItemObject Item;
			int iType = ITEM_GET_TYPE(lpMsg->Type);
			int iTypeIndex = ITEM_GET_INDEX(lpMsg->Type);
			Item.m_Level = lpMsg->Level;
			Item.m_Durability = lpMsg->Dur;
			Item.m_Number = lpMsg->m_Number;
			Item.m_Type = lpMsg->Type;

			Item.Convert(ITEMGET(iType, iTypeIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, lpMsg->MainAttribute, 0, CURRENT_DB_VERSION);

			BYTE iItemPos = gObjInventoryInsertItem(*lpObj, Item);

			if (iItemPos == (BYTE)-1)
			{

			}
			else
			{
				GSProtocol.GCInventoryItemOneSend(*lpObj, iItemPos);
				GSProtocol.GCServerMsgStringSendEx(*lpObj, 1, Lang.GetText(0, 535), ItemAttribute[Item.m_Type].Name);
			}
		}
	}

	else if (lpMsg->MapNumber == 234)
	{
		if (lpObj->Connected > PLAYER_CONNECTED)
		{
			CItemObject Item;
			int iType = ITEM_GET_TYPE(lpMsg->Type);
			int iTypeIndex = ITEM_GET_INDEX(lpMsg->Type);
			Item.m_Level = lpMsg->Level;
			Item.m_Number = lpMsg->m_Number;
			Item.m_Type = lpMsg->Type;

			Item.Convert(ITEMGET(iType, iTypeIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, lpMsg->MainAttribute, 0, CURRENT_DB_VERSION);

			Item.m_Durability = ItemGetDurability(ITEMGET(iType, iTypeIndex), Item.m_Level, Item.m_NewOption, Item.m_SetOption);

			BYTE iItemPos = gObjInventoryInsertItem(*lpObj, Item);

			if (iItemPos == (BYTE)-1)
			{

				GSProtocol.GCAnsLuckyCoinTrade(lpObj, 2);
			}
			else
			{
				if (lpMsg->lDuration > 0)
				{
					g_PeriodItemEx.SetPeriodItemInfo(*lpObj, lpMsg->Type, lpMsg->m_Number, lpMsg->lDuration);
				}

				GSProtocol.GCInventoryItemOneSend(lpObj, iItemPos);

				if (lpMsg->lDuration > 0)
				{
					g_PeriodItemEx.SendPeriodItemInfoOnce(lpObj, *Item);
				}

				GSProtocol.GCAnsLuckyCoinTrade(lpObj, 1);
			}

			GJSetCharacterInfo(*lpObj, Obj, FALSE);
		}
	}

	else if (lpMsg->MapNumber == 235)
	{
		if (lpObj->Connected > PLAYER_CONNECTED)
		{
			CItemObject Item;
			int iType = ITEM_GET_TYPE(lpMsg->Type);
			int iTypeIndex = ITEM_GET_INDEX(lpMsg->Type);
			Item.m_Level = lpMsg->Level;
			Item.m_Durability = lpMsg->Dur;
			Item.m_Number = lpMsg->m_Number;
			Item.m_Type = lpMsg->Type;

			Item.Convert(ITEMGET(iType, iTypeIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, lpMsg->MainAttribute, 0, CURRENT_DB_VERSION);

			BYTE iItemPos = gObjInventoryInsertItem(*lpObj, Item);

			if (iItemPos == (BYTE)-1)
			{

			}
			else
			{
				if (lpMsg->lDuration > 0)
				{
					g_PeriodItemEx.SetPeriodItemInfo(*lpObj, lpMsg->Type, lpMsg->m_Number, lpMsg->lDuration);
				}

				GSProtocol.GCInventoryItemOneSend(lpObj, iItemPos);

				if (lpMsg->lDuration > 0)
				{
					g_PeriodItemEx.SendPeriodItemInfoOnce(lpObj, *Item);
				}
			}

			GJSetCharacterInfo(Obj, FALSE);
		}
	}

	else if (lpMsg->MapNumber == 236)
	{
		if (lpObj->Connected > PLAYER_CONNECTED)
		{
			CItemObject pCreateItem;
			time_t curtime = time(NULL);
			int iItemType = ITEM_GET_TYPE(lpMsg->Type);
			int iItemIndex = ITEM_GET_INDEX(lpMsg->Type);

			pCreateItem.m_Type = lpMsg->Type;

			pCreateItem.Convert(ITEMGET(iItemType, iItemIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, lpMsg->MainAttribute, 0, CURRENT_DB_VERSION);
			pCreateItem.m_Level = lpMsg->Level;
			pCreateItem.m_Durability = 255.0;
			pCreateItem.m_Number = lpMsg->m_Number;

			BYTE btItemPos = gObjInventoryInsertItem(lpObj, pCreateItem);

			if (btItemPos == (BYTE)-1)
			{

			}
			else
			{
				if (lpMsg->lDuration > 0)
				{
					g_PeriodItemEx.SetPeriodItemInfo(*lpObj, lpMsg->Type, lpMsg->m_Number, lpMsg->lDuration);
				}

				GSProtocol.GCInventoryItemOneSend(lpObj, btItemPos);

				if (lpMsg->lDuration > 0)
				{
					g_PeriodItemEx.SendPeriodItemInfoOnce(lpObj, *pCreateItem);
				}
			}
		}
	}

	else if (lpMsg->MapNumber == 229)
	{
		if (lpObj->Connected > PLAYER_LOGGED)
		{
			PMSG_BUYRESULT pResult;
			CItemObject pCreateItem;
			int iItemType = ITEM_GET_TYPE(lpMsg->Type);
			int iItemIndex = ITEM_GET_INDEX(lpMsg->Type);

			pCreateItem.m_Level = lpMsg->Level;
			pCreateItem.m_Durability = lpMsg->Dur;
			pCreateItem.m_Number = lpMsg->m_Number;
			pCreateItem.Convert(ITEMGET(iItemType, iItemIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, lpMsg->MainAttribute, 0, CURRENT_DB_VERSION);
			pCreateItem.m_Level = lpMsg->Level;

			BOOL bIsBlank = gObjGamblingInventoryCheck(*lpObj, 2, 4);
			if (bIsBlank == TRUE)
			{
				BYTE btItemPos = gObjInventoryInsertItem(*lpObj, pCreateItem);
				if (btItemPos == 0xFF)
				{
					PHeadSetB((BYTE*)&pResult, 0x32, sizeof(pResult));
					pResult.Result = 0xFF;
					IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
					return;
				}

				int iBuyTaxMoney = (g_GamblingItemBag.GetGamblingValue() * (__int64)g_CastleSiegeSync.GetTaxRateChaos(*lpObj) / (__int64)100);
				g_CastleSiegeSync.AddTributeMoney(iBuyTaxMoney);

				lpObj->m_PlayerData->Money -= iBuyTaxMoney + g_GamblingItemBag.GetGamblingValue();

				if (lpObj->m_PlayerData->Money < 0)
					lpObj->m_PlayerData->Money = 0;

				GSProtocol.GCMoneySend(Obj, lpObj->m_PlayerData->Money);
				GSProtocol.GCInventoryItemOneSend(*lpObj, btItemPos);

				BYTE ExOption[6];
				ItemIsBufExOption(ExOption, &pCreateItem);

				if (lpMsg->Type == ITEMGET(3, 11) ||
					lpMsg->Type == ITEMGET(5, 33) ||
					lpMsg->Type == ITEMGET(4, 24) ||
					lpMsg->Type == ITEMGET(2, 18) ||
					lpMsg->Type == ITEMGET(5, 34))
				{
					char szBuffer[256] = { 0 };
					sprintf(szBuffer, Lang.GetText(0, 589), pCreateItem.GetName());

					PMSG_NOTICE pNotice;
					TNotice::MakeNoticeMsg(&pNotice, 0, szBuffer);
					TNotice::SendNoticeToAllUser(&pNotice);

					PMSG_SERVERCMD ServerCmd;
					PHeadSubSetB((BYTE*)&ServerCmd, 0xF3, 0x40, sizeof(ServerCmd));
					ServerCmd.CmdType = 0;
					ServerCmd.X = lpObj->X;
					ServerCmd.Y = lpObj->Y;
					GSProtocol.MsgSendV2(*lpObj, (BYTE*)&ServerCmd, sizeof(ServerCmd));
					IOCP.DataSend(*lpObj, (BYTE*)&ServerCmd, sizeof(ServerCmd));

				}
				else
				{

				}
			}
			else
			{
				GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 536), *lpObj, 1);

				PHeadSetB((BYTE*)&pResult, 0x32, sizeof(pResult));
				pResult.Result = 0xFF;
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
				lpObj->ChaosLock = FALSE;
				return;
			}
		}
	}

	else if (lpMsg->MapNumber == 228)
	{
		if (lpObj->Connected > PLAYER_CONNECTED)
		{
			CItemObject Item;
			int iType = ITEM_GET_TYPE(lpMsg->Type);
			int iTypeIndex = ITEM_GET_INDEX(lpMsg->Type);
			Item.m_Level = lpMsg->Level;
			Item.m_Number = lpMsg->m_Number;
			Item.m_Type = lpMsg->Type;

			Item.Convert(ITEMGET(iType, iTypeIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, lpMsg->MainAttribute, 0, CURRENT_DB_VERSION);

			Item.m_Durability = ItemGetDurability(ITEMGET(iType, iTypeIndex), Item.m_Level, Item.m_NewOption, Item.m_SetOption);

			BYTE iItemPos = gObjInventoryInsertItem(*lpObj, Item);

			if (iItemPos == (BYTE)-1)
			{

			}
			else
			{
				GSProtocol.GCInventoryItemOneSend(*lpObj, iItemPos);
			}
		}
	}

	else if (lpMsg->MapNumber == 227)
	{
		CItemObject Item;
		int iType = ITEM_GET_TYPE(lpMsg->Type);
		int iTypeIndex = ITEM_GET_INDEX(lpMsg->Type);
		Item.m_Level = lpMsg->Level;
		Item.m_Durability = lpMsg->Dur;
		Item.m_Number = lpMsg->m_Number;
		Item.m_Type = lpMsg->Type;
		if (g_CMuunSystem.IsMuunItem(lpMsg->Type) == TRUE)
		{
			int nMuunRank = g_CMuunSystem.GetMuunRankOfMuunInfo(lpMsg->Type);
			BYTE SocketOptionIndex = 0;
			SocketOptionIndex |= nMuunRank;

			Item.Convert(ITEMGET(iType, iTypeIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, SocketOptionIndex, 0, CURRENT_DB_VERSION);

		}
		else
		{
			Item.Convert(ITEMGET(iType, iTypeIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, lpMsg->MainAttribute, 0, CURRENT_DB_VERSION);

		}

		int iItemCount = 0;

		if (g_CMuunSystem.IsMuunItem(lpMsg->Type) == TRUE)
		{
			int nMuunRank = g_CMuunSystem.GetMuunRankOfMuunInfo(lpMsg->Type);
			int iItemCount = MapC[lpObj->MapNumber].MonsterItemDrop(lpMsg->Type, lpMsg->Level, lpMsg->Dur, lpMsg->x,
				lpMsg->y, lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption,
				lootindex, lpMsg->m_Number, 0, lpMsg->SocketOption, nMuunRank, lpMsg->lDuration);
		}
		else
		{
			int iItemCount = MapC[lpObj->MapNumber].MonsterItemDrop(lpMsg->Type, lpMsg->Level, lpMsg->Dur, lpMsg->x,
				lpMsg->y, lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption,
				lootindex, lpMsg->m_Number, 0, lpMsg->SocketOption, lpMsg->MainAttribute, lpMsg->lDuration);
		}

		BYTE NewOption[MAX_EXOPTION_SIZE];

		for (int i = 0; i < MAX_EXOPTION_SIZE; i++)
		{
			NewOption[i] = FALSE;
		}

		if ((lpMsg->NewOption & 0x20) != 0)
		{
			NewOption[0] = TRUE;
		}
		if ((lpMsg->NewOption & 0x10) != 0)
		{
			NewOption[1] = TRUE;
		}

		if ((lpMsg->NewOption & 0x8) != 0)
		{
			NewOption[2] = TRUE;
		}
		if ((lpMsg->NewOption & 0x4) != 0)
		{
			NewOption[3] = TRUE;
		}

		if ((lpMsg->NewOption & 0x2) != 0)
		{
			NewOption[4] = TRUE;
		}
		if ((lpMsg->NewOption & 0x1) != 0)
		{
			NewOption[5] = TRUE;
		}

	}

	else if (lpMsg->MapNumber == 226)
	{
		if (lpObj->Connected > 1)
		{
			CItemObject Item;
			int iType = ITEM_GET_TYPE(lpMsg->Type);
			int iTypeIndex = ITEM_GET_INDEX(lpMsg->Type);
			Item.m_Level = lpMsg->Level;
			Item.m_Durability = lpMsg->Dur;
			Item.m_Number = lpMsg->m_Number;
			Item.m_Type = lpMsg->Type;

			Item.Convert(ITEMGET(iType, iTypeIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, lpMsg->MainAttribute, 0, CURRENT_DB_VERSION);

			BYTE btItemPos = gObjEventInventoryInsertItem(*lpObj, Item);

			if (btItemPos == (BYTE)-1)
			{

			}

			else
			{
				GSProtocol.GCEventInvenItemOneSend(*lpObj, btItemPos);
			}
		}
	}

	else if (lpMsg->MapNumber == 225)
	{
		if (lpObj->Connected > 1)
		{
			LPOBJ lpObj = *lpObj;
			CMuRummyInfo * pCMuRummyInfo = lpObj->m_PlayerData->m_pCMuRummyInfo;

			if (!pCMuRummyInfo)
			{
				return;
			}

			CItemObject Item;
			int iType = ITEM_GET_TYPE(lpMsg->Type);
			int iTypeIndex = ITEM_GET_INDEX(lpMsg->Type);
			Item.m_Level = lpMsg->Level;
			Item.m_Durability = lpMsg->Dur;
			Item.m_Number = lpMsg->m_Number;
			Item.m_Type = lpMsg->Type;

			if (g_PentagramSystem.IsPentagramItem(lpMsg->Type) == TRUE)
			{
				BYTE btEnableSlot[5];
				BYTE btSocketSlotCount[5];
				BYTE btSlotCount = g_PentagramSystem.GetMakePentagramSlotCountNKind(btEnableSlot, lpMsg->Type);

				for (int i = 0; i < 5; i++)
				{
					if (btEnableSlot[i])
					{
						btSocketSlotCount[i] = 0xFE;
					}

					else
					{
						btSocketSlotCount[i] = 0xFF;
					}
				}

				Item.Convert(ITEMGET(iType, iTypeIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, btSocketSlotCount, lpMsg->MainAttribute, 0, CURRENT_DB_VERSION);
			}

			else
			{
				Item.Convert(ITEMGET(iType, iTypeIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, lpMsg->MainAttribute, 0, CURRENT_DB_VERSION);
			}

			BYTE btItemPos = gObjInventoryInsertItem(*lpObj, Item);

			if (btItemPos == (BYTE)-1)
			{
				pCMuRummyInfo->SetWaitReward(0);
				g_CMuRummyMng.GCSendMsg(*lpObj, 7, 0);

			}

			else
			{
				_tagPMSG_ANS_MURUMMY_END pMsg;

				PHeadSubSetB((BYTE*)&pMsg, 0x4D, 0x15, sizeof(pMsg));
				pMsg.btResult = TRUE;
				IOCP.DataSend(*lpObj, (BYTE*)&pMsg, pMsg.h.size);

				g_CMuRummyMng.GCSendMsg(*lpObj, 9, ITEMGET(iType, iTypeIndex));

				if (lpMsg->lDuration > 0)
				{
					g_PeriodItemEx.SetPeriodItemInfo(lpObj, lpMsg->Type, lpMsg->m_Number, lpMsg->lDuration);
				}

				GSProtocol.GCInventoryItemOneSend(*lpObj, btItemPos);

				if (lpMsg->lDuration > 0)
				{
					g_PeriodItemEx.SendPeriodItemInfoOnce(lpObj, &Item);
				}

				g_CMuRummyMng.GCSendMsg(*lpObj, 8, 0);

				BYTE NewOption[MAX_EXOPTION_SIZE];
				ItemIsBufExOption(NewOption, &Item);

				g_CMuRummyMng.GDReqMuRummyDBLog(lpObj, pCMuRummyInfo->GetScore());
				pCMuRummyInfo->Clear();
				g_CMuRummyMng.GDReqScoreDelete(lpObj);
			}
		}
	}

	else if (lpMsg->MapNumber == 224)
	{
		if (lpObj->Connected > 1)
		{
			LPOBJ lpObj = *lpObj;

			CItemObject MuunItem;
			int ItemType = ITEM_GET_TYPE(lpMsg->Type);
			int ItemIndex = ITEM_GET_INDEX(lpMsg->Type);
			int nMuunRank = g_CMuunSystem.GetMuunRankOfMuunInfo(lpMsg->Type);
			BYTE SocketOption[5];
			int SocketSlotCount = 0;

			for (int k = 0; k < 5; k++)
			{
				SocketOption[k] = lpMsg->SocketOption[k];
			}

			MuunItem.m_Level = lpMsg->Level;
			MuunItem.m_Durability = lpMsg->Dur;
			MuunItem.m_Number = lpMsg->m_Number;
			MuunItem.Convert(lpMsg->Type, lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, SocketOption, nMuunRank, 0, 3);

			if (g_CMuunSystem.IsMuunExpireDate(lpMsg->Type) == TRUE)
			{
				MuunItem.SetMuunItemPeriodExpire();
			}

			BYTE btItemPos = gObjMuunInventoryInsertItem(*lpObj, MuunItem);

			if (btItemPos == (BYTE)-1)
			{

			}
			else
			{
				GSProtocol.GCMuunInventoryItemOneSend(*lpObj, btItemPos);
			}
		}
	}

	else if (lpMsg->MapNumber == 223)
	{
		if (lpObj->Connected > PLAYER_CONNECTED)
		{
			CItemObject Item;
			int iType = ITEM_GET_TYPE(lpMsg->Type);
			int iTypeIndex = ITEM_GET_INDEX(lpMsg->Type);
			Item.m_Level = lpMsg->Level;
			Item.m_Durability = lpMsg->Dur;
			Item.m_Number = lpMsg->m_Number;
			Item.m_Type = lpMsg->Type;

			Item.Convert(ITEMGET(iType, iTypeIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, 0, -1, 0, CURRENT_DB_VERSION);

			BYTE iItemPos = gObjInventoryInsertItem(*lpObj, Item);

			if (iItemPos == (BYTE)-1)
			{

			}
			else
			{
				::GSProtocol.GCInventoryItemOneSend(*lpObj, iItemPos);
			}

		}
	}

	else if (lpMsg->MapNumber == 222)
	{
		if (lpObj->Connected > PLAYER_CONNECTED)
		{
			CItemObject Item;
			int iType = ITEM_GET_TYPE(lpMsg->Type);
			int iTypeIndex = ITEM_GET_INDEX(lpMsg->Type);
			Item.m_Level = lpMsg->Level;
			Item.m_Durability = lpMsg->Dur;
			Item.m_Number = lpMsg->m_Number;
			Item.m_Type = lpMsg->Type;

			Item.Convert(ITEMGET(iType, iTypeIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, lpMsg->MainAttribute, 0, CURRENT_DB_VERSION);

			BYTE iItemPos = gObjInventoryInsertItem(*lpObj, Item);

			PMSG_BUYRESULT pResult;
			PHeadSetB((BYTE*)&pResult, 0x32, sizeof(pResult));

			if (iItemPos == (BYTE)-1)
			{
				pResult.Result = -1;
			}
			else
			{
				pResult.Result = iItemPos;
				ItemByteConvert(pResult.ItemInfo, Item);
			}

			IOCP.DataSend(*lpObj, (BYTE*)&pResult, sizeof(pResult));
		}
	}

	else if (lpMsg->MapNumber == 221)
	{
		if (lpObj->Connected > PLAYER_CONNECTED)
		{
			CItemObject Item;

			int iType = ITEM_GET_TYPE(lpMsg->Type);
			int iTypeIndex = ITEM_GET_INDEX(lpMsg->Type);
			Item.m_Level = lpMsg->Level;
			Item.m_Durability = lpMsg->Dur;
			Item.m_Number = lpMsg->m_Number;
			Item.m_Type = lpMsg->Type;

			Item.Convert(ITEMGET(iType, iTypeIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, lpMsg->MainAttribute, 0, CURRENT_DB_VERSION);

			BYTE iItemPos = gObjInventoryInsertItem(*lpObj, Item);

			PMSG_BUYRESULT pResult;
			PHeadSetB((BYTE*)&pResult, 0x32, sizeof(pResult));

			if (iItemPos == (BYTE)-1)
			{
				pResult.Result = -1;
			}
			else
			{
				pResult.Result = iItemPos;
				ItemByteConvert(pResult.ItemInfo, Item);
			}

			IOCP.DataSend(*lpObj, (BYTE*)&pResult, sizeof(pResult));
		}
	}

	else if (lpMsg->MapNumber == 220)
	{
		if (lpObj->Connected > 1)
		{
			LPOBJ lpObj = *lpObj;
			CMuRummyInfo * pCMuRummyInfo = lpObj->m_PlayerData->m_pCMuRummyInfo;

			if (!pCMuRummyInfo)
			{
				return;
			}

			CItemObject MuunItem;
			int ItemType = ITEM_GET_TYPE(lpMsg->Type);
			int ItemIndex = ITEM_GET_INDEX(lpMsg->Type);
			int nMuunRank = g_CMuunSystem.GetMuunRankOfMuunInfo(lpMsg->Type);
			BYTE SocketOption[5];
			int SocketSlotCount = 0;

			for (int k = 0; k < 5; k++)
			{
				SocketOption[k] = lpMsg->SocketOption[k];
			}

			MuunItem.m_Level = lpMsg->Level;
			MuunItem.m_Durability = lpMsg->Dur;
			MuunItem.m_Number = lpMsg->m_Number;
			MuunItem.Convert(lpMsg->Type, lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, SocketOption, nMuunRank, 0, 3);

			if (g_CMuunSystem.IsMuunExpireDate(lpMsg->Type) == TRUE)
			{
				MuunItem.SetMuunItemPeriodExpire();
			}

			BYTE btItemPos = gObjMuunInventoryInsertItem(*lpObj, MuunItem);

			if (btItemPos == (BYTE)-1)
			{
				pCMuRummyInfo->SetWaitReward(0);
				g_CMuRummyMng.GCSendMsg(*lpObj, 7, 0);

			}

			else
			{
				_tagPMSG_ANS_MURUMMY_END pMsg;

				PHeadSubSetB((BYTE*)&pMsg, 0x4D, 0x15, sizeof(pMsg));
				pMsg.btResult = TRUE;
				IOCP.DataSend(*lpObj, (BYTE*)&pMsg, pMsg.h.size);

				g_CMuRummyMng.GCSendMsg(*lpObj, 9, ITEMGET(ItemType, ItemIndex));
				GSProtocol.GCMuunInventoryItemOneSend(*lpObj, btItemPos);
				g_CMuRummyMng.GCSendMsg(*lpObj, 8, 0);

				BYTE NewOption[MAX_EXOPTION_SIZE];
				ItemIsBufExOption(NewOption, &MuunItem);

				g_CMuRummyMng.GDReqMuRummyDBLog(lpObj, pCMuRummyInfo->GetScore());
				pCMuRummyInfo->Clear();
				g_CMuRummyMng.GDReqScoreDelete(lpObj);
			}
		}
	}

	else if (lpMsg->MapNumber == 219)
	{
		if (lpObj->Connected > 1)
		{
			CItemObject Item;
			int ItemType = ITEM_GET_TYPE(lpMsg->Type);
			int ItemIndex = ITEM_GET_INDEX(lpMsg->Type);
			Item.m_Level = lpMsg->Level;
			Item.m_Durability = lpMsg->Dur;
			Item.m_Number = lpMsg->m_Number;

			Item.Convert(lpMsg->Type, lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, 0, 0, 3);

			BYTE btItemPos = gObjInventoryInsertItem(*lpObj, Item);

			if (btItemPos == (BYTE)-1)
			{

			}
			else
			{
				GSProtocol.GCInventoryItemOneSend(*lpObj, btItemPos);

				BYTE NewOption[MAX_EXOPTION_SIZE];
				ItemIsBufExOption(NewOption, &Item);

				lpObj->ChaosLock = FALSE;
			}
		}

		else
		{
			lpObj->ChaosLock = FALSE;
		}
	}

	else if (lpMsg->MapNumber == 218)
	{
		if (lpObj->Connected > 1)
		{
			LPOBJ lpObj = *lpObj;

			int nRank = 0;
			BYTE SocketOption[5];

			if (g_CMuunSystem.IsMuunItem(lpMsg->Type) == TRUE)
			{
				nRank = g_CMuunSystem.GetMuunRankOfMuunInfo(lpMsg->Type);
			}

			else if (g_CMuunSystem.IsStoneofEvolution(lpMsg->Type) == TRUE)
			{
				nRank = 0;
			}

			for (int n = 0; n < 5; n++)
			{
				SocketOption[n] = lpMsg->SocketOption[n];
			}

			CItemObject Item;
			int ItemType = ITEM_GET_TYPE(lpMsg->Type);
			int ItemIndex = ITEM_GET_INDEX(lpMsg->Type);
			Item.m_Level = lpMsg->Level;
			Item.m_Durability = lpMsg->Dur;
			Item.m_Number = lpMsg->m_Number;

			Item.Convert(lpMsg->Type, lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, SocketOption, nRank, 0, 3);

			BYTE btRet = g_CMuunSystem.DGMuunExchangeInsertInven(lpObj, Item, lpMsg->x);

			if (btRet == 0xFF)
			{

			}
			else
			{
				BYTE NewOption[MAX_EXOPTION_SIZE];
				ItemIsBufExOption(NewOption, &Item);

				if (g_CMuunSystem.IsStoneofEvolution(lpMsg->Type) == TRUE)
				{
					WORD wPetItemNumber = (Item.m_SocketOption[0] << 8) | Item.m_SocketOption[1];
					LPITEM_ATTRIBUTE lpItemAttribute = GetItemAttr(wPetItemNumber);

					if (lpItemAttribute != NULL)
					{

					}
					else
					{

					}
				}
				else
				{

				}
			}
		}
	}

	else if (lpMsg->MapNumber == 217)
	{
		if (lpObj->Connected > PLAYER_CONNECTED)
		{
			CItemObject Item;

			int iType = ITEM_GET_TYPE(lpMsg->Type);
			int iTypeIndex = ITEM_GET_INDEX(lpMsg->Type);
			Item.m_Level = lpMsg->Level;
			Item.m_Durability = lpMsg->Dur;
			Item.m_Number = lpMsg->m_Number;
			Item.m_Type = lpMsg->Type;

			Item.Convert(ITEMGET(iType, iTypeIndex), lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption, 0, lpMsg->SocketOption, lpMsg->MainAttribute, 0, CURRENT_DB_VERSION);

			BYTE iItemPos = gObjInventoryInsertItem(*lpObj, Item);

			PMSG_ANS_RUUD_STORE_BUYITEM pMsg;
			PHeadSubSetB((BYTE*)&pMsg, 0xD0, 0xF0, sizeof(pMsg));

			if (iItemPos == (BYTE)-1)
			{
				pMsg.btResult = 0xFA;
			}
			else
			{
				pMsg.btResult = 0;
				pMsg.btPos = iItemPos;
				ItemByteConvert(pMsg.btItemInfo, Item);

			}
			GSProtocol.GCInventoryItemOneSend(Obj, iItemPos);
			//IOCP.DataSend(*lpObj, (BYTE*)&pMsg, sizeof(pMsg));
		}
	}

	else
	{
		int iRetMapNumber = lpMsg->MapNumber;

		if (lpMsg->MapNumber >= 238 && lpMsg->MapNumber <= 245)
		{
			mapnumber = g_BloodCastle.GetItemMapNumberFirst(iRetMapNumber);
		}

		if (lpMsg->MapNumber >= 246 && lpMsg->MapNumber <= 253)
		{
			mapnumber = g_BloodCastle.GetItemMapNumberSecond(iRetMapNumber);
		}

		if (mapnumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL && g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
		{
			if (lpObj->MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
			{
				if (gObjIsConnected(*lpObj))
				{
					lpMsg->x = lpObj->X;
					lpMsg->y = lpObj->Y;
				}
			}

			BYTE btAttr = MapC[MAP_INDEX_CHAOSCASTLE_SURVIVAL].GetAttr(lpMsg->x, lpMsg->y);

			if ((btAttr & 8) == 8)
			{
				g_ChaosCastleFinal.GetRandomDropPos(lpMsg->x, lpMsg->y);
			}
		}

		CItemObject Item;

		Item.m_Type = lpMsg->Type;

		BYTE SocketSlotCount = 0;
		BYTE SocketOption[5] = { 0, 0, 0, 0, 0 };
		BYTE SocketOptionIndex = 0;

		if (gSocketItemType.CheckSocketItemType(lpMsg->Type) == 1)
		{
			if (memcmp(lpMsg->SocketOption, SocketOption, 5) != 0)
			{
				for (int i = 0; i < 5; i++)
				{
					if (lpMsg->SocketOption[i] == 0xFE)
					{
						SocketSlotCount++;
					}
				}
			}

			else
			{
				SocketSlotCount = g_SocketOptionSystem.GetMakeSocketSlotCount(lpMsg->Type);
			}

			SocketOptionIndex = 0xFF;

			for (int i = 0; i < 5; i++)
			{
				if (i < SocketSlotCount)
				{
					SocketOption[i] = 0xFE;
				}
				else
				{
					SocketOption[i] = 0xFF;
				}
			}

		}

		else if (g_PentagramSystem.IsPentagramItem(lpMsg->Type) == TRUE)
		{
			BYTE btEnableSlot[5];

			if (memcmp(lpMsg->SocketOption, SocketOption, 5) != 0)
			{
				for (int i = 0; i < 5; i++)
				{
					if (lpMsg->SocketOption[i] == 0xFE)
					{
						btEnableSlot[i] = TRUE;
						SocketSlotCount++;
					}

					else
					{
						btEnableSlot[i] = FALSE;
					}
				}
			}

			else
			{
				SocketSlotCount = g_PentagramSystem.GetMakePentagramSlotCountNKind(btEnableSlot, lpMsg->Type);
			}

			SocketOptionIndex = lpMsg->MainAttribute;

			for (int k = 0; k < 5; k++)
			{
				if (btEnableSlot[k])
				{
					SocketOption[k] = 0xFE;
				}

				else
				{
					SocketOption[k] = 0xFF;
				}
			}
		}

		else if (g_CMuunSystem.IsMuunItem(lpMsg->Type) == TRUE)
		{
			int nMuunRank = g_CMuunSystem.GetMuunRankOfMuunInfo(lpMsg->Type);
			SocketOptionIndex |= nMuunRank;

			for (int k = 0; k < 5; k++)
			{
				SocketOption[k] = lpMsg->SocketOption[k];
			}

		}

		else if (g_CMuunSystem.IsStoneofEvolution(lpMsg->Type) == TRUE)
		{
			for (int k = 0; k < 5; k++)
			{
				SocketOption[k] = lpMsg->SocketOption[k];
			}

		}

		else if (g_PentagramSystem.IsPentagramJewel(lpMsg->Type) == TRUE)
		{
			SocketOptionIndex = lpMsg->MainAttribute;

			for (int l = 0; l < 5; l++)
			{
				SocketOption[l] = lpMsg->SocketOption[l];
			}
		}

		else
		{
			SocketOptionIndex = 0;

			if (memcmp(lpMsg->SocketOption, SocketOption, 5) == 0)
			{
				for (int i = 0; i < 5; i++)
				{
					SocketOption[i] = 0xFF;
				}
			}

			else
			{
				for (int i = 0; i < 5; i++)
				{
					SocketOption[i] = lpMsg->SocketOption[i];
				}
			}
		}

		int iItemCount = MapC[mapnumber].MonsterItemDrop(lpMsg->Type, lpMsg->Level, lpMsg->Dur, lpMsg->x,
			lpMsg->y, lpMsg->Op1, lpMsg->Op2, lpMsg->Op3, lpMsg->NewOption, lpMsg->SetOption,
			lootindex, lpMsg->m_Number, 0, SocketOption, SocketOptionIndex, lpMsg->lDuration);

		if (iItemCount != -1)
		{
			if (iRetMapNumber >= 238 && iRetMapNumber <= 245)
			{
				MapC[mapnumber].m_CItemObject[iItemCount].m_Time = GetTickCount() + 300000;
				MapC[mapnumber].m_CItemObject[iItemCount].m_LootTime = GetTickCount() + 20000;

			}

			if (iRetMapNumber >= 246 && iRetMapNumber <= 253)
			{
				MapC[mapnumber].m_CItemObject[iItemCount].m_Time = GetTickCount() + 900000;
				MapC[mapnumber].m_CItemObject[iItemCount].m_LootTime = GetTickCount() + 10000;

				int iBridgeIndex = g_BloodCastle.GetBridgeIndex(mapnumber); //s3 add-on (loc108)

				g_BloodCastle.SetQuestItemSerialNumber(iBridgeIndex, lpMsg->m_Number);
			}

			if (((mapnumber) < MAP_INDEX_ILLUSIONTEMPLE1) ? FALSE : ((mapnumber) > MAP_INDEX_ILLUSIONTEMPLE6) ? FALSE : TRUE) //Season 2.5 add-on
			{
				if (lpMsg->Type == ITEMGET(14, 64))
				{
					MapC[mapnumber].m_CItemObject[iItemCount].m_Time = GetTickCount() + 15000;
					MapC[mapnumber].m_CItemObject[iItemCount].m_LootTime = GetTickCount() + 5000;
				}

				if (lpMsg->Type == ITEMGET(12, 15))
				{
					MapC[mapnumber].m_CItemObject[iItemCount].m_Time = GetTickCount() + 300000;
					MapC[mapnumber].m_CItemObject[iItemCount].m_LootTime = GetTickCount() + 10000;
				}
			}
		}

		//int iItemCount;
		BYTE NewOption[MAX_EXOPTION_SIZE];

		for (int i = 0; i < MAX_EXOPTION_SIZE; i++)
		{
			NewOption[i] = FALSE;
		}

		if ((lpMsg->NewOption & 0x20) != 0)
		{
			NewOption[0] = TRUE;
		}
		if ((lpMsg->NewOption & 0x10) != 0)
		{
			NewOption[1] = TRUE;
		}

		if ((lpMsg->NewOption & 0x8) != 0)
		{
			NewOption[2] = TRUE;
		}
		if ((lpMsg->NewOption & 0x4) != 0)
		{
			NewOption[3] = TRUE;
		}

		if ((lpMsg->NewOption & 0x2) != 0)
		{
			NewOption[4] = TRUE;
		}
		if ((lpMsg->NewOption & 0x1) != 0)
		{
			NewOption[5] = TRUE;
		}
	}
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Item Move Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  : 0x53
*/
struct SDHP_ITEMMOVESAVE
{
	PBMSG_HEAD h;	// C1:53
	DWORD Serial;	// 4
	char ServerName[20];	// 8
	char Account[11];	// 1C
	char Name[11];	// 26
	char ItemName[21];	// 30
	BYTE X;	// 45
	BYTE Y;	// 46
	BYTE ItemLevel;	// 47
	BYTE ItemOp1;	// 48
	BYTE ItemOp2;	// 49
	BYTE ItemOp3;	// 4A
};


void ItemMovePathSave(char* ActID, char* Name, BYTE level, BYTE mapnumber, BYTE x, BYTE y, char* Item, BYTE op1, BYTE op2, BYTE op3, DWORD serial)
{
	SDHP_ITEMMOVESAVE pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x53;
	pMsg.h.size = sizeof(pMsg);
	pMsg.ItemLevel = level;
	pMsg.Serial = serial;
	pMsg.ItemOp1 = op1;
	pMsg.ItemOp2 = op2;
	pMsg.ItemOp3 = op3;
	pMsg.X = x;
	pMsg.Y = y;
	std::memcpy(pMsg.Account, ActID, sizeof(pMsg.Account));
	std::strcpy(pMsg.ItemName, Item);
	std::memcpy(pMsg.Name, Name, sizeof(pMsg.Name));
	std::strcpy(pMsg.ServerName, g_ConfigRead.server.GetServerName());

	wsDataCli.DataSend((PCHAR)&pMsg, pMsg.h.size);
}



/* * * * * * * * * * * * * * * * * * * * *
*	Mu Skill Key Data Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  : 0x60
*/
struct SDHP_SKILLKEYDATA
{
	PBMSG_HEAD h;	// C1:60
	char Name[11];	// 3
	BYTE SkillKeyBuffer[20];	// D
	BYTE GameOption;	// 17
	BYTE QkeyDefine;	// 18
	BYTE WkeyDefine;	// 19
	BYTE EkeyDefine;	// 1A
	BYTE ChatWindow;	// 1B
	BYTE RkeyDefine;
	int QWERLevelDefine;
	BYTE EnableTransformMode;
};


void DGOptionDataSend(CGameObject &Obj, char* szName, BYTE * KeyBuffer, BYTE GO, BYTE Qk, BYTE Wk, BYTE Ek, BYTE ChatWnd, BYTE Rk, int QWER, BYTE Transform)
{
	SDHP_SKILLKEYDATA pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0x60;
	pMsg.h.size = sizeof(pMsg);
	std::memcpy(pMsg.Name, szName, sizeof(pMsg.Name));
	std::memcpy(pMsg.SkillKeyBuffer, KeyBuffer, sizeof(pMsg.SkillKeyBuffer));
	pMsg.GameOption = GO;
	pMsg.QkeyDefine = Qk;
	pMsg.WkeyDefine = Wk;
	pMsg.EkeyDefine = Ek;
	pMsg.ChatWindow = ChatWnd;
	pMsg.RkeyDefine = Rk;
	pMsg.QWERLevelDefine = QWER;
	pMsg.EnableTransformMode = Transform;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}



void DGOptionDataRecv(SDHP_SKILLKEYDATA_SEND * lpMsg)
{
	CGameObject* lpObj = getGameObject(lpMsg->iUserIndex);
	char szName[MAX_ACCOUNT_LEN + 1];

	if (Obj < 0 || Obj >= g_ConfigRead.server.GetObjectMax())
	{
		return;
	}

	std::memset(szName, 0, sizeof(szName));
	std::memcpy(szName, lpMsg->Name, sizeof(lpMsg->Name));

	if (strcmp(lpObj->Name, szName) != 0)
	{
		return;
	}

	::GSProtocol.GCSkillKeySend(Obj, lpMsg->SkillKeyBuffer, lpMsg->GameOption, lpMsg->QkeyDefine, lpMsg->WkeyDefine, lpMsg->EkeyDefine, lpMsg->ChatWindow, lpMsg->RkeyDefine, lpMsg->QWERLevelDefine);
	lpObj->m_PlayerData->m_EnableUseChangeSkin = lpMsg->EnableTransformMode;
	gObjViewportListProtocolCreate(&gObj[Obj]);
}

void DGMoveOtherServer(SDHP_CHARACTER_TRANSFER_RESULT * lpMsg)
{
	if (!gObjIsConnectedGP(lpMsg->Number))
	{
		sLog->outError("error-L3 [%s][%d]", __FILE__, __LINE__);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->Number];

	if (lpMsg->Result != 0)
	{
		PMSG_DEFRESULT pResult;

		PHeadSetB((BYTE*)&pResult, 0x99, sizeof(pResult));
		pResult.result = 1;

		IOCP.DataSend(lpMsg->Number, (BYTE*)&pResult, pResult.h.size);

		sLog->outBasic("[CharTrasfer] Fail [%s][%s] (%d)",
			lpObj->AccountID, lpObj->Name, lpMsg->Result);

		lpObj->m_MoveOtherServer = 0;

		GSProtocol.GCServerMsgStringSend("???? ????? change@webzen.co.kr?? ?????? ??ñ??????", lpObj->m_Index, 1);
		// Deathway translation here
		return;
	}

	sLog->outBasic("[CharTrasfer] Success [%s][%s] (%d)",
		lpObj->AccountID, lpObj->Name, lpMsg->Result);

	GSProtocol.GCServerMsgStringSend("?????? ???????.", lpObj->m_Index, 1);// Deathway translation here
	GSProtocol.GCServerMsgStringSend("???? ?????? ????????ñ? ??????.", lpObj->m_Index, 1);// Deathway translation here
	GJSetCharacterInfo(*lpObj, 0);
	lpObj->LoadWareHouseInfo = false;
	gObjCloseSet(*getGameObject(lpMsg->Number), 2);
	lpObj->m_MoveOtherServer = 0;
}

struct Request_PetItem_Info
{
	BYTE nPos;	// 0
	UINT64 nSerial;	// 4
};


/* * * * * * * * * * * * * * * * * * * * *
*	Mu Request Pet Info Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC2
*	HeadCode  : 0x56
*	BASE PACKET - Complete with Request_PetItem_Info
*/
struct SDHP_REQUEST_PETITEM_INFO
{
	PWMSG_HEAD h;
	char AccountID[11];	// 4
	WORD Number;	// 10
	BYTE InvenType;	// 12
	BYTE nCount;	// 13
	WORD ServerType;
	WORD ServerCode;
};

void gObjRequestPetItemInfo(CGameObject &Obj, int inventype)
{
	LPOBJ lpObj = &gObj[Obj];
	char pbuffer[4096];
	int lofs = sizeof(SDHP_REQUEST_PETITEM_INFO);
	int founditemcount = 0;
	Request_PetItem_Info petIteminfo;

	if (inventype == 0)
	{
		for (int n = 0; n < INVENTORY_SIZE; n++)
		{
			if (lpObj->pntInventory[n]->IsItem())
			{
				if (lpObj->pntInventory[n]->m_Type == ITEMGET(13, 4) || lpObj->pntInventory[n]->m_Type == ITEMGET(13, 5))
				{
					founditemcount++;
					petIteminfo.nPos = n;
					petIteminfo.nSerial = lpObj->pntInventory[n]->m_Number;
					std::memcpy(&pbuffer[lofs], &petIteminfo, sizeof(petIteminfo));
					lofs += sizeof(petIteminfo);
				}
			}
		}
	}
	else if (inventype == 1)
	{
		for (int n = 0; n < WAREHOUSE_SIZE; n++)
		{
			if (lpObj->pntWarehouse[n]->IsItem())
			{
				if (lpObj->pntWarehouse[n]->m_Type == ITEMGET(13, 4) || lpObj->pntWarehouse[n]->m_Type == ITEMGET(13, 5))
				{
					founditemcount++;
					petIteminfo.nPos = n;
					petIteminfo.nSerial = lpObj->pntWarehouse[n]->m_Number;
					std::memcpy(&pbuffer[lofs], &petIteminfo, sizeof(petIteminfo));
					lofs += sizeof(petIteminfo);
				}
			}
		}
	}

	if (founditemcount)
	{
		SDHP_REQUEST_PETITEM_INFO pMsg;

		pMsg.h.set((BYTE*)&pMsg, 0x56, lofs + sizeof(pMsg));
		std::memcpy(pMsg.AccountID, lpObj->AccountID, MAX_ACCOUNT_LEN);
		pMsg.Number = Obj.m_Index;
		pMsg.InvenType = inventype;
		pMsg.nCount = founditemcount;
		pMsg.ServerType = g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE ? TRUE : FALSE;
		pMsg.ServerCode = g_ConfigRead.server.GetGameServerCode() / 20;

		std::memcpy(pbuffer, &pMsg, sizeof(pMsg));
		wsDataCli.DataSend((PCHAR)pbuffer, lofs + sizeof(pMsg));
	}
}

struct Recv_PetItem_Info
{
	BYTE nPos;	// 0
	UINT64 nSerial;	// 4
	BYTE Level;	// 8
	UINT64 Exp;	// C
};

struct SDHP_RECV_PETITEM_INFO
{
	PWMSG_HEAD h;
	char AccountID[11];	// 4
	WORD Number;	// 10
	BYTE InvenType;	// 12
	BYTE nCount;	// 13
};

void DGRecvPetItemInfo(BYTE* lpData)
{
	SDHP_RECV_PETITEM_INFO* lpMsg = (SDHP_RECV_PETITEM_INFO *)lpData;
	Recv_PetItem_Info* pPetItemInfo = (Recv_PetItem_Info *)(lpData + sizeof(SDHP_RECV_PETITEM_INFO));
	CGameObject* lpObj = getGameObject(lpMsg->Number);
	char szAccountId[MAX_ACCOUNT_LEN + 1];
	szAccountId[MAX_ACCOUNT_LEN] = 0;

	std::memcpy(szAccountId, lpMsg->AccountID, MAX_ACCOUNT_LEN);

	if (!gObjIsAccontConnect(*lpObj, szAccountId))
	{
		sLog->outError("Request to receive petitem infomation doesn't match the user %s", szAccountId);
		IOCP.CloseClient(lpObj->m_PlayerData->ConnectUser->Index);

		return;
	}

	if (lpMsg->InvenType == 0)
	{
		BOOL ReCalc = 0;

		for (int n = 0; n < lpMsg->nCount; n++)
		{
			if (pPetItemInfo[n].nPos < INVETORY_WEAR_SIZE)
			{
				ReCalc = 1;
			}

			if (lpObj->pntInventory[pPetItemInfo[n].nPos]->IsItem())
			{
				if (lpObj->pntInventory[pPetItemInfo[n].nPos]->m_Number == pPetItemInfo[n].nSerial)
				{
					lpObj->pntInventory[pPetItemInfo[n].nPos]->SetPetItemInfo(pPetItemInfo[n].Level, pPetItemInfo[n].Exp);
				}
			}
		}

		if (ReCalc != FALSE)
		{
			gObjCalCharacter.CalcCharacter(*lpObj);
		}
	}
	else if (lpMsg->InvenType == 1)
	{
		for (int n = 0; n < lpMsg->nCount; n++)
		{

			if (lpObj->pntWarehouse[pPetItemInfo[n].nPos]->IsItem())
			{
				if (lpObj->pntWarehouse[pPetItemInfo[n].nPos]->m_Number == pPetItemInfo[n].nSerial)
				{
					lpObj->pntWarehouse[pPetItemInfo[n].nPos]->SetPetItemInfo(pPetItemInfo[n].Level, pPetItemInfo[n].Exp);
				}
			}
		}
	}
}

struct Save_PetItem_Info
{
	UINT64 nSerial;	// 0
	BYTE Level;	// 3
	UINT64 Exp;	// 8
};


/* * * * * * * * * * * * * * * * * * * * *
*	Mu PetInfo Save Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC2
*	HeadCode  : 0x55
*	BASE PACKET - Complete with Save_PetItem_Info
*/
struct SDHP_SAVE_PETITEM_INFO
{
	PWMSG_HEAD h;
	BYTE nCount;	// 4
	WORD ServerType;
	WORD ServerCode;
};

void gObjSavePetItemInfo(CGameObject &Obj, int inventype)
{
	char pbuffer[4096];
	int lofs = sizeof(SDHP_SAVE_PETITEM_INFO);
	int founditemcount = 0;
	Save_PetItem_Info petIteminfo;

	if (inventype == 0)
	{
		for (int n = 0; n < INVENTORY_SIZE; n++)
		{
			if (Obj.pntInventory[n]->IsItem())
			{
				if (Obj.pntInventory[n]->m_Type == ITEMGET(13, 4) || Obj.pntInventory[n]->m_Type == ITEMGET(13, 5))
				{
					if (Obj.pntInventory[n]->m_IsLoadPetItemInfo != FALSE)
					{
						founditemcount++;
						petIteminfo.nSerial = Obj.pntInventory[n]->m_Number;
						petIteminfo.Level = Obj.pntInventory[n]->m_PetItem_Level;
						petIteminfo.Exp = Obj.pntInventory[n]->m_PetItem_Exp;

						std::memcpy(&pbuffer[lofs], &petIteminfo, sizeof(petIteminfo));
						lofs += sizeof(petIteminfo);
					}
				}
			}
		}
	}
	else if (inventype == 1)
	{
		for (int n = 0; n < WAREHOUSE_SIZE; n++)
		{
			if (Obj.pntWarehouse[n]->IsItem())
			{
				if (Obj.pntWarehouse[n]->m_Type == ITEMGET(13, 4) || Obj.pntWarehouse[n]->m_Type == ITEMGET(13, 5))
				{
					if (Obj.pntWarehouse[n]->m_IsLoadPetItemInfo != FALSE)
					{
						founditemcount++;
						petIteminfo.nSerial = Obj.pntWarehouse[n]->m_Number;
						petIteminfo.Level = Obj.pntWarehouse[n]->m_PetItem_Level;
						petIteminfo.Exp = Obj.pntWarehouse[n]->m_PetItem_Exp;

						std::memcpy(&pbuffer[lofs], &petIteminfo, sizeof(petIteminfo));
						lofs += sizeof(petIteminfo);
					}
				}
			}
		}
	}

	if (founditemcount)
	{
		SDHP_SAVE_PETITEM_INFO pMsg;

		pMsg.h.set((BYTE*)&pMsg, 0x57, lofs + sizeof(pMsg));
		pMsg.nCount = founditemcount;
		pMsg.ServerType = g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE ? TRUE : FALSE;

		if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
		{
			pMsg.ServerCode = Obj.m_PlayerData->m_nServerCodeOfHomeWorld;
		}

		else
		{
			pMsg.ServerCode = g_ConfigRead.server.GetGameServerCode() / 20;
		}

		std::memcpy(pbuffer, &pMsg, sizeof(pMsg));

		wsDataCli.DataSend(pbuffer, lofs + sizeof(pMsg));
	}
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  :
*	SubCode   :
*/
struct CSP_REQ_CASTLEDATA
{
	PBMSG_HEAD2 h;	// C1:80:00
	WORD wMapSvrNum;	// 4
	int iCastleEventCycle;	// 8
};

//004340C0 - identical
void GS_GDReqCastleTotalInfo(int iMapSvrGroup, int iCastleEventCycle)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	CSP_REQ_CASTLEDATA pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0x80, 0x00, sizeof(CSP_REQ_CASTLEDATA));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iCastleEventCycle = iCastleEventCycle;

	::wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  :
*	SubCode   :
*/
struct CSP_REQ_OWNERGUILDMASTER
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
};

//00434180 - identical
void GS_GDReqOwnerGuildMaster(int iMapSvrGroup, CGameObject &Obj)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (ObjectMaxRange(Obj) == FALSE)
	{
		return;
	}

	CSP_REQ_OWNERGUILDMASTER pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0x80, 0x01, sizeof(CSP_REQ_OWNERGUILDMASTER));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = Obj;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  :
*	SubCode   :
*/
struct CSP_REQ_NPCBUY
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	int iNpcNumber;	// C
	int iNpcIndex;	// 10
	int iNpcDfLevel;	// 14
	int iNpcRgLevel;	// 18
	int iNpcMaxHp;	// 1C
	int iNpcHp;	// 20
	BYTE btNpcX;	// 24
	BYTE btNpcY;	// 25
	BYTE btNpcDIR;	// 26
	int iBuyCost;	// 28
};

//00434220 - identical
void GS_GDReqCastleNpcBuy(int iMapSvrGroup, CGameObject &Obj, int iNpcNumber, int iNpcIndex, int iNpcDfLevel, int iNpcRgLevel, int iNpcMaxHP, int iNpcHP, BYTE btNpcX, BYTE btNpcY, BYTE btNpcDIR, int iBuyCost)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (ObjectMaxRange(Obj) == FALSE)
	{
		return;
	}

	CSP_REQ_NPCBUY pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0x80, 0x03, sizeof(CSP_REQ_NPCBUY));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = Obj;
	pMsg.iNpcNumber = iNpcNumber;
	pMsg.iNpcIndex = iNpcIndex;
	pMsg.iNpcDfLevel = iNpcDfLevel;
	pMsg.iNpcRgLevel = iNpcRgLevel;
	pMsg.iNpcMaxHp = iNpcMaxHP;
	pMsg.iNpcHp = iNpcHP;
	pMsg.btNpcX = btNpcX;
	pMsg.btNpcY = btNpcY;
	pMsg.btNpcDIR = btNpcDIR;
	pMsg.iBuyCost = iBuyCost;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  :
*	SubCode   :
*/
struct CSP_REQ_NPCREPAIR
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	int iNpcNumber;	// C
	int iNpcIndex;	// 10
	int iRepairCost;	// 14
};

//00434310 - identical
void GS_GDReqCastleNpcRepair(int iMapSvrGroup, CGameObject &Obj, int iNpcNumber, int iNpcIndex, int iRepairCost)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (ObjectMaxRange(Obj) == FALSE)
	{
		return;
	}

	CSP_REQ_NPCREPAIR pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0x80, 0x04, sizeof(CSP_REQ_NPCREPAIR));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = Obj;
	pMsg.iNpcNumber = iNpcNumber;
	pMsg.iNpcIndex = iNpcIndex;
	pMsg.iRepairCost = iRepairCost;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  :
*	SubCode   :
*/
struct CSP_REQ_NPCUPGRADE
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	int iNpcNumber;	// C
	int iNpcIndex;	// 10
	int iNpcUpType;	// 14
	int iNpcUpValue;	// 18
	int iNpcUpIndex;	// 1C
};

void GS_GDReqCastleNpcUpgrade(int iMapSvrGroup, CGameObject &Obj, int iNpcNumber, int iNpcIndex, int iNpcUpType, int iNpcUpValue, int iNpcUpIndex)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (ObjectMaxRange(Obj) == FALSE)
	{
		return;
	}

	CSP_REQ_NPCUPGRADE pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0x80, 0x05, sizeof(CSP_REQ_NPCUPGRADE));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = Obj;
	pMsg.iNpcNumber = iNpcNumber;
	pMsg.iNpcIndex = iNpcIndex;
	pMsg.iNpcUpType = iNpcUpType;
	pMsg.iNpcUpValue = iNpcUpValue;
	pMsg.iNpcUpIndex = iNpcUpIndex;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  :
*	SubCode   :
*/
struct CSP_REQ_TAXINFO
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 3
	int iIndex;	// 8
};

void GS_GDReqTaxInfo(int iMapSvrGroup, CGameObject &Obj)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (ObjectMaxRange(Obj) == FALSE)
	{
		return;
	}

	CSP_REQ_TAXINFO pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0x80, 0x06, sizeof(CSP_REQ_TAXINFO));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = Obj;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}


/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  :
*	SubCode   :
*/
struct CSP_REQ_TAXRATECHANGE
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	int iTaxKind;	// C
	int iTaxRate;	// 10
};

void GS_GDReqTaxRateChange(int iMapSvrGroup, CGameObject &Obj, int iTaxType, int iTaxRate)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (ObjectMaxRange(Obj) == FALSE)
	{
		return;
	}

	CSP_REQ_TAXRATECHANGE pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0x80, 0x07, sizeof(CSP_REQ_TAXRATECHANGE));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = Obj;
	pMsg.iTaxRate = iTaxRate;
	pMsg.iTaxKind = iTaxType;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  :
*	SubCode   :
*/
struct CSP_REQ_MONEYCHANGE
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	int iMoneyChanged;	// C
};

void GS_GDReqCastleMoneyChange(int iMapSvrGroup, CGameObject &Obj, int iMoneyChange)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (ObjectMaxRange(Obj) == FALSE)
	{
		return;
	}

	CSP_REQ_MONEYCHANGE pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0x80, 0x08, sizeof(CSP_REQ_MONEYCHANGE));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = Obj;
	pMsg.iMoneyChanged = iMoneyChange;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  :
*	SubCode   :
*/
struct CSP_REQ_SDEDCHANGE
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	WORD wStartYear;	// C
	BYTE btStartMonth;	// E
	BYTE btStartDay;	// F
	WORD wEndYear;	// 10
	BYTE btEndMonth;	// 12
	BYTE btEndDay;	// 13
};

void GS_GDReqSiegeDateChange(int iMapSvrGroup, CGameObject &Obj, WORD wStartYear, BYTE btStartMonth, BYTE btStartDay, WORD wEndYear, BYTE btEndMonth, BYTE btEndDay)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	CSP_REQ_SDEDCHANGE pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0x80, 0x09, sizeof(CSP_REQ_SDEDCHANGE));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = Obj;
	pMsg.wStartYear = wStartYear;
	pMsg.btStartMonth = btStartMonth;
	pMsg.btStartDay = btStartDay;
	pMsg.wEndYear = wEndYear;
	pMsg.btEndMonth = btEndMonth;
	pMsg.btEndDay = btEndDay;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  :
*	SubCode   :
*/
struct CSP_REQ_GUILDREGINFO
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	char szGuildName[8];	// C
};

void GS_GDReqGuildMarkRegInfo(int iMapSvrGroup, CGameObject &Obj)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (ObjectMaxRange(Obj) == FALSE)
	{
		return;
	}

	CSP_REQ_GUILDREGINFO pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0x80, 0x0A, sizeof(CSP_REQ_GUILDREGINFO));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = Obj;
	std::memcpy(pMsg.szGuildName, lpObj->m_PlayerData->GuildName, 8);

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  :
*	SubCode   :
*/
struct CSP_REQ_SIEGEENDCHANGE
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	BOOL bIsSiegeEnded;	// 8
};

void GS_GDReqSiegeEndedChange(int iMapSvrGroup, BOOL bIsSiegeEnded)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	CSP_REQ_SIEGEENDCHANGE pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0x80, 0x0B, sizeof(CSP_REQ_SIEGEENDCHANGE));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.bIsSiegeEnded = bIsSiegeEnded;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  :
*	SubCode   :
*/
struct CSP_REQ_CASTLEOWNERCHANGE
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	BOOL bIsCastleOccupied;	// 8
	char szOwnerGuildName[8];	// C
};

void GS_GDReqCastleOwnerChange(int iMapSvrGroup, BOOL bIsCastleOccupied, char* lpszGuildName)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (lpszGuildName == NULL)
	{
		return;
	}

	CSP_REQ_CASTLEOWNERCHANGE pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0x80, 0x0C, sizeof(CSP_REQ_CASTLEOWNERCHANGE));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.bIsCastleOccupied = bIsCastleOccupied;
	std::memcpy(pMsg.szOwnerGuildName, lpszGuildName, 8);

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  :
*	SubCode   :
*/
struct CSP_REQ_REGATTACKGUILD
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	char szEnemyGuildName[8];	// C
};

void GS_GDReqRegAttackGuild(int iMapSvrGroup, CGameObject &Obj)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (ObjectMaxRange(Obj) == FALSE)
	{
		return;
	}

	CSP_REQ_REGATTACKGUILD pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0x80, 0x0D, sizeof(CSP_REQ_REGATTACKGUILD));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = Obj;
	std::memcpy(pMsg.szEnemyGuildName, lpObj->m_PlayerData->GuildName, 8);

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  :
*	SubCode   :
*/
struct CSP_REQ_CASTLESIEGEEND
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
};

void GS_GDReqRestartCastleState(int iMapSvrGroup)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	CSP_REQ_CASTLESIEGEEND pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0x80, 0x0E, sizeof(CSP_REQ_CASTLESIEGEEND));
	pMsg.wMapSvrNum = iMapSvrGroup;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  :
*	SubCode   :
*/
struct CSP_REQ_MAPSVRMULTICAST
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	char szMsgText[128];	// 6
};

void GS_GDReqMapSvrMsgMultiCast(int iMapSvrGroup, char * lpszMsgText)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (lpszMsgText == NULL)
	{
		return;
	}

	CSP_REQ_MAPSVRMULTICAST pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0x80, 0x0F, sizeof(CSP_REQ_MAPSVRMULTICAST));
	pMsg.wMapSvrNum = iMapSvrGroup;
	std::strcpy(pMsg.szMsgText, lpszMsgText);
	pMsg.szMsgText[127] = 0;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

struct CSP_GLOBALPOST_MULTICAST
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	char szSender[MAX_ACCOUNT_LEN];
	char szMessage[MAX_CHAT_LEN];
	char szServerName[50];
	BYTE btColorRGB[3];
};

void GS_GDReqGlobalPostMultiCast(int iMapSvrGroup, CGameObject &Obj, char * szMessage)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (!ObjectMaxRange(Obj))
	{
		return;
	}

	if (szMessage == NULL)
	{
		return;
	}

	CSP_GLOBALPOST_MULTICAST pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0x80, 0x1F, sizeof(CSP_GLOBALPOST_MULTICAST));
	pMsg.wMapSvrNum = iMapSvrGroup;
	std::memcpy(pMsg.szSender, lpObj->Name, MAX_ACCOUNT_LEN);
	std::strcpy(pMsg.szMessage, szMessage);
	std::memcpy(pMsg.szServerName, g_ConfigRead.server.GetServerName(), 50);

	pMsg.btColorRGB[0] = g_ConfigRead.data.post.btGPostColorRGB[0];
	pMsg.btColorRGB[1] = g_ConfigRead.data.post.btGPostColorRGB[1];
	pMsg.btColorRGB[2] = g_ConfigRead.data.post.btGPostColorRGB[2];

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  :
*	SubCode   :
*/
struct CSP_REQ_GUILDREGMARK
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	char szGuildName[8];	// C
	int iItemPos;	// 14
};

void GS_GDReqRegGuildMark(int iMapSvrGroup, CGameObject &Obj, int iItemPos)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (ObjectMaxRange(Obj) == FALSE)
	{
		return;
	}

	CSP_REQ_GUILDREGMARK pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0x80, 0x10, sizeof(CSP_REQ_GUILDREGMARK));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = Obj;
	pMsg.iItemPos = iItemPos;
	std::memcpy(&pMsg.szGuildName, lpObj->m_PlayerData->GuildName, sizeof(pMsg.szGuildName));

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  :
*	SubCode   :
*/
struct CSP_REQ_GUILDRESETMARK
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	char szGuildName[8];	// C
};

void GS_GDReqGuildMarkReset(int iMapSvrGroup, CGameObject &Obj, char* lpszGuildName)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (ObjectMaxRange(Obj) == FALSE)
	{
		return;
	}

	if (lpszGuildName == NULL)
	{
		return;
	}

	CSP_REQ_GUILDRESETMARK pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0x80, 0x11, sizeof(CSP_REQ_GUILDRESETMARK));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = Obj;
	std::memcpy(pMsg.szGuildName, lpszGuildName, 8);

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  :
*	SubCode   :
*/
struct CSP_REQ_GUILDSETGIVEUP
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
	char szGuildName[8];	// C
	BOOL bIsGiveUp;	// 14
};

void GS_GDReqGuildSetGiveUp(int iMapSvrGroup, CGameObject &Obj, BOOL bIsGiveUp)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (ObjectMaxRange(Obj) == FALSE)
	{
		return;
	}

	CSP_REQ_GUILDSETGIVEUP pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0x80, 0x12, sizeof(CSP_REQ_GUILDSETGIVEUP));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = Obj;
	pMsg.bIsGiveUp = bIsGiveUp;
	std::memcpy(pMsg.szGuildName, lpObj->m_PlayerData->GuildName, 8);

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  :
*	SubCode   :
*/
struct CSP_REQ_NPCREMOVE
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iNpcNumber;	// 8
	int iNpcIndex;	// C
};

void GS_GDReqNpcRemove(int iMapSvrGroup, int iNpcNumber, int iNpcIndex)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	CSP_REQ_NPCREMOVE pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0x80, 0x16, sizeof(CSP_REQ_NPCREMOVE));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iNpcNumber = iNpcNumber;
	pMsg.iNpcIndex = iNpcIndex;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  :
*	SubCode   :
*/
struct CSP_REQ_CASTLESTATESYNC
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iCastleState;	// 8
	int iTaxRateChaos;	// C
	int iTaxRateStore;	// 10
	int iTaxHuntZone;	// 14
	char szOwnerGuildName[8];	// 18
};

void GS_GDReqCastleStateSync(int iMapSvrGroup, int iCastleState, int iTaxRateChaos, int iTaxRateStore, int iTaxHuntZone, char* lpszOwnerGuild)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	CSP_REQ_CASTLESTATESYNC pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0x80, 0x17, sizeof(CSP_REQ_CASTLESTATESYNC));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iCastleState = iCastleState;
	pMsg.iTaxRateChaos = iTaxRateChaos;
	pMsg.iTaxRateStore = iTaxRateStore;
	pMsg.iTaxHuntZone = iTaxHuntZone;
	std::memcpy(pMsg.szOwnerGuildName, lpszOwnerGuild, 8);

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  :
*	SubCode   :
*/
struct CSP_REQ_CASTLETRIBUTEMONEY
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iCastleTributeMoney;	// 8
};

void GS_GDReqCastleTributeMoney(int iMapSvrGroup, int iCastleTributeMoney)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	if (iCastleTributeMoney < 0)
	{
		return;
	}

	CSP_REQ_CASTLETRIBUTEMONEY pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0x80, 0x18, sizeof(CSP_REQ_CASTLETRIBUTEMONEY));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iCastleTributeMoney = iCastleTributeMoney;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  :
*	SubCode   :
*/
struct CSP_REQ_RESETCASTLETAXINFO
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
};

void GS_GDReqResetCastleTaxInfo(int iMapSvrGroup)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	CSP_REQ_RESETCASTLETAXINFO pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0x80, 0x19, sizeof(CSP_REQ_RESETCASTLETAXINFO));
	pMsg.wMapSvrNum = iMapSvrGroup;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  :
*	SubCode   :
*/
struct CSP_REQ_RESETSIEGEGUILDINFO
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
};

void GS_GDReqResetSiegeGuildInfo(int iMapSvrGroup)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	CSP_REQ_RESETSIEGEGUILDINFO pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0x80, 0x1A, sizeof(CSP_REQ_RESETSIEGEGUILDINFO));
	pMsg.wMapSvrNum = iMapSvrGroup;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  :
*	SubCode   :
*/
struct CSP_REQ_RESETREGSIEGEINFO
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 2
};

void GS_GDReqResetRegSiegeInfo(int iMapSvrGroup)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	CSP_REQ_RESETREGSIEGEINFO pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0x80, 0x1B, sizeof(CSP_REQ_RESETREGSIEGEINFO));
	pMsg.wMapSvrNum = iMapSvrGroup;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  :
*/
struct CSP_REQ_CSINITDATA
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
	int iCastleEventCycle;	// 8
};

void GS_GDReqCastleInitData(int iMapSvrGroup, int iCastleEventCycle)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	CSP_REQ_CSINITDATA pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0x81, sizeof(CSP_REQ_CSINITDATA));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iCastleEventCycle = iCastleEventCycle;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  :
*/
struct CSP_REQ_NPCDATA
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
};

void GS_GDReqCastleNpcInfo(int iMapSvrGroup, CGameObject &Obj)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	CSP_REQ_NPCDATA pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0x82, sizeof(CSP_REQ_NPCDATA));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = Obj;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  :
*/
struct CSP_REQ_ALLGUILDREGINFO
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
	int iIndex;	// 8
};

void GS_GDReqAllGuildMarkRegInfo(int iMapSvrGroup, CGameObject &Obj)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	CSP_REQ_ALLGUILDREGINFO pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0x83, sizeof(CSP_REQ_ALLGUILDREGINFO));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iIndex = Obj;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  :
*/
struct CSP_REQ_CALCREGGUILDLIST
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
};

void GS_GDReqCalcRegGuildList(int iMapSvrGroup)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	CSP_REQ_CALCREGGUILDLIST pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0x85, sizeof(CSP_REQ_CALCREGGUILDLIST));
	pMsg.wMapSvrNum = iMapSvrGroup;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : GameServer -> DataServer
*  Code      : 0xC1
*	HeadCode  :
*/
struct CSP_REQ_CSLOADTOTALGUILDINFO
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
};

void GS_GDReqCsLoadTotalGuildInfo(int iMapSvrGroup)
{
	if (iMapSvrGroup < 0)
	{
		return;
	}

	CSP_REQ_CSLOADTOTALGUILDINFO pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0x88, sizeof(CSP_REQ_CSLOADTOTALGUILDINFO));
	pMsg.wMapSvrNum = iMapSvrGroup;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

struct CSP_ANS_CASTLEDATA
{

	// static data ------------------------------------

	// non-static data --------------------------------
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xa>*/ /*|0x2|*/ WORD wStartYear;
	/*<thisrel this+0xc>*/ /*|0x1|*/ BYTE btStartMonth;
	/*<thisrel this+0xd>*/ /*|0x1|*/ BYTE btStartDay;
	/*<thisrel this+0xe>*/ /*|0x2|*/ WORD wEndYear;
	/*<thisrel this+0x10>*/ /*|0x1|*/ BYTE btEndMonth;
	/*<thisrel this+0x11>*/ /*|0x1|*/ BYTE btEndDay;
	/*<thisrel this+0x12>*/ /*|0x1|*/ BYTE btIsSiegeGuildList;
	/*<thisrel this+0x13>*/ /*|0x1|*/ BYTE btIsSiegeEnded;
	/*<thisrel this+0x14>*/ /*|0x1|*/ BYTE btIsCastleOccupied;
	/*<thisrel this+0x15>*/ /*|0x8|*/ char szCastleOwnGuild[8];
	/*<thisrel this+0x20>*/ /*|0x8|*/ __int64 i64CastleMoney;
	/*<thisrel this+0x28>*/ /*|0x4|*/ int iTaxRateChaos;
	/*<thisrel this+0x2c>*/ /*|0x4|*/ int iTaxRateStore;
	/*<thisrel this+0x30>*/ /*|0x4|*/ int iTaxHuntZone;
	/*<thisrel this+0x34>*/ /*|0x4|*/ int iFirstCreate;
};

void GS_DGAnsCastleTotalInfo(BYTE* lpRecv)
{
	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		CSP_ANS_CASTLEDATA* lpMsg = (CSP_ANS_CASTLEDATA*)lpRecv;

		if (lpMsg == NULL)
			return;

		if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
			sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x00] GS_DGAnsCastleTotalInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
	}

	else
	{
		return;
	}
}

struct CSP_ANS_OWNERGUILDMASTER {

	// static data ------------------------------------

	// non-static data --------------------------------
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iIndex;
	/*<thisrel this+0x10>*/ /*|0x8|*/ char szCastleOwnGuild[8];
	/*<thisrel this+0x18>*/ /*|0xa|*/ char szCastleOwnGuildMaster[10];

};

void GS_DGAnsOwnerGuildMaster(BYTE* lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_OWNERGUILDMASTER* lpMsg = (CSP_ANS_OWNERGUILDMASTER*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x00] GS_DGAnsCastleTotalInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	GSProtocol.GCAnsCastleSiegeState(lpMsg->iIndex, lpMsg->iResult, lpMsg->szCastleOwnGuild, lpMsg->szCastleOwnGuildMaster);
}

struct CSP_ANS_NPCBUY {

	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iIndex;
	/*<thisrel this+0x10>*/ /*|0x4|*/ int iNpcNumber;
	/*<thisrel this+0x14>*/ /*|0x4|*/ int iNpcIndex;
	/*<thisrel this+0x18>*/ /*|0x4|*/ int iBuyCost;
};

void GS_DGAnsCastleNpcBuy(BYTE* lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_NPCBUY* lpMsg = (CSP_ANS_NPCBUY*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x03] GS_DGAnsCastleNpcBuy() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if (lpMsg->iResult == 1)
	{
		BOOL bAddResult = g_CastleSiege.AddDbNPC(lpMsg->iNpcNumber, lpMsg->iNpcIndex);

		if (bAddResult == TRUE)
		{
			if (gObjIsConnected(lpMsg->iIndex))
			{
				gObj[lpMsg->iIndex].m_PlayerData->Money -= lpMsg->iBuyCost;

				if (gObj[lpMsg->iIndex].m_PlayerData->Money < 0)
				{
					gObj[lpMsg->iIndex].m_PlayerData->Money = 0;
				}

				GSProtocol.GCMoneySend(lpMsg->iIndex, gObj[lpMsg->iIndex].m_PlayerData->Money);
			}

			sLog->outBasic("[CastleSiege] GS_DGAnsCastleNpcBuy() - CCastleSiege::AddDbNPC() OK - Npc:(CLS:%d, IDX:%d)", lpMsg->iNpcNumber, lpMsg->iNpcIndex);
		}
		else
		{
			sLog->outBasic("[CastleSiege] GS_DGAnsCastleNpcBuy() - CCastleSiege::AddDbNPC() FAILED - Npc:(CLS:%d, IDX:%d)", lpMsg->iNpcNumber, lpMsg->iNpcIndex);
		}
	}
	else
	{
		sLog->outBasic("[CastleSiege] GS_DGAnsCastleNpcBuy() FAILED - Result:(%d), Npc:(CLS:%d, IDX:%d)", lpMsg->iResult, lpMsg->iNpcNumber, lpMsg->iNpcIndex);
	}

	GSProtocol.GCAnsNpcBuy(lpMsg->iIndex, lpMsg->iResult, lpMsg->iNpcNumber, lpMsg->iNpcIndex);
}

struct CSP_ANS_NPCREPAIR {
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iIndex;
	/*<thisrel this+0x10>*/ /*|0x4|*/ int iNpcNumber;
	/*<thisrel this+0x14>*/ /*|0x4|*/ int iNpcIndex;
	/*<thisrel this+0x18>*/ /*|0x4|*/ int iNpcMaxHp;
	/*<thisrel this+0x1c>*/ /*|0x4|*/ int iNpcHp;
	/*<thisrel this+0x20>*/ /*|0x4|*/ int iRepairCost;
};

void GS_DGAnsCastleNpcRepair(BYTE* lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_NPCREPAIR* lpMsg = (CSP_ANS_NPCREPAIR*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x04] GS_DGAnsCastleNpcRepair() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if (lpMsg->iResult == 1)
	{
		BOOL bRepairResult = g_CastleSiege.RepairDbNPC(lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcHp, lpMsg->iNpcMaxHp);
		if (bRepairResult == TRUE)
		{
			if (gObjIsConnected(lpMsg->iIndex))
			{
				gObj[lpMsg->iIndex].m_PlayerData->Money -= lpMsg->iRepairCost;

				if (gObj[lpMsg->iIndex].m_PlayerData->Money < 0)
				{
					gObj[lpMsg->iIndex].m_PlayerData->Money = 0;
				}

				GSProtocol.GCMoneySend(lpMsg->iIndex, gObj[lpMsg->iIndex].m_PlayerData->Money);
			}

			sLog->outBasic("[CastleSiege] GS_DGAnsCastleNpcRepair() - CCastleSiege::RepairDbNPC() OK - Npc:(CLS:%d, IDX:%d)", lpMsg->iNpcNumber, lpMsg->iNpcIndex);
		}
		else
		{
			sLog->outBasic("[CastleSiege] GS_DGAnsCastleNpcRepair() - CCastleSiege::RepairDbNPC() FAILED - Npc:(CLS:%d, IDX:%d)", lpMsg->iNpcNumber, lpMsg->iNpcIndex);
		}
	}
	else
	{
		sLog->outBasic("[CastleSiege] GS_DGAnsCastleNpcRepair() FAILED - Result:(%d), Npc:(CLS:%d, IDX:%d)", lpMsg->iResult, lpMsg->iNpcNumber, lpMsg->iNpcIndex);
	}

	GSProtocol.GCAnsNpcRepair(lpMsg->iIndex, lpMsg->iResult, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcHp, lpMsg->iNpcMaxHp);
}

struct CSP_ANS_NPCUPGRADE {
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iIndex;
	/*<thisrel this+0x10>*/ /*|0x4|*/ int iNpcNumber;
	/*<thisrel this+0x14>*/ /*|0x4|*/ int iNpcIndex;
	/*<thisrel this+0x18>*/ /*|0x4|*/ int iNpcUpType;
	/*<thisrel this+0x1c>*/ /*|0x4|*/ int iNpcUpValue;
	/*<thisrel this+0x20>*/ /*|0x4|*/ int iNpcUpIndex;
};

void GS_DGAnsCastleNpcUpgrade(BYTE* lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_NPCUPGRADE* lpMsg = (CSP_ANS_NPCUPGRADE*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x05] GS_DGAnsCastleNpcUpgrade() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if (lpMsg->iResult == 0)
	{
		sLog->outError("[CastleSiege] ERROR - Castle NPC Upgrade Fail() (CLS:%d, IDX:%d, UPTYPE:%d, UPVAL:%d)", lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
	}
	else
	{
		g_CastleSiege.UpgradeDbNPC(*getGameObject(lpMsg->iIndex), lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue, lpMsg->iNpcUpIndex);
		sLog->outBasic("[CastleSiege] [0x80][0x06] GS_DGAnsTaxInfo() - Npc Upgrade OK (CLS:%d, IDX:%d, UPTYPE:%d, UPVAL:%d)", lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
	}

	GSProtocol.GCAnsNpcUpgrade(lpMsg->iIndex, lpMsg->iResult, lpMsg->iNpcNumber, lpMsg->iNpcIndex, lpMsg->iNpcUpType, lpMsg->iNpcUpValue);
}

struct CSP_ANS_TAXINFO {
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iIndex;
	/*<thisrel this+0x10>*/ /*|0x8|*/ __int64 i64CastleMoney;
	/*<thisrel this+0x18>*/ /*|0x4|*/ int iTaxRateChaos;
	/*<thisrel this+0x1c>*/ /*|0x4|*/ int iTaxRateStore;
	/*<thisrel this+0x20>*/ /*|0x4|*/ int iTaxHuntZone;
};

void GS_DGAnsTaxInfo(BYTE* lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_TAXINFO* lpMsg = (CSP_ANS_TAXINFO*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x06] GS_DGAnsTaxInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if (lpMsg->iResult == TRUE)
	{
		GSProtocol.GCAnsTaxMoneyInfo(lpMsg->iIndex, lpMsg->iResult, lpMsg->iTaxRateChaos, lpMsg->iTaxRateStore, lpMsg->i64CastleMoney);
		g_CastleSiege.SetCastleMoney(lpMsg->i64CastleMoney);
	}
}

struct CSP_ANS_TAXRATECHANGE {
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iIndex;
	/*<thisrel this+0x10>*/ /*|0x4|*/ int iTaxKind;
	/*<thisrel this+0x14>*/ /*|0x4|*/ int iTaxRate;
};

void GS_DGAnsTaxRateChange(BYTE* lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_TAXRATECHANGE* lpMsg = (CSP_ANS_TAXRATECHANGE*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x07] GS_DGAnsTaxRateChange() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if (lpMsg->iResult == 1)
	{
		g_CastleSiege.SetTaxRate(lpMsg->iTaxKind, lpMsg->iTaxRate);
		GSProtocol.GCAnsTaxRateChange(lpMsg->iIndex, lpMsg->iResult, lpMsg->iTaxKind, lpMsg->iTaxRate);
	}
}

struct CSP_ANS_MONEYCHANGE {
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iIndex;
	/*<thisrel this+0x10>*/ /*|0x4|*/ int iMoneyChanged;
	/*<thisrel this+0x18>*/ /*|0x8|*/ __int64 i64CastleMoney;
};

void GS_DGAnsCastleMoneyChange(BYTE* lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_MONEYCHANGE* lpMsg = (CSP_ANS_MONEYCHANGE*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x08] GS_DGAnsCastleMoneyChange() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if (lpMsg->iResult == TRUE)
	{
		if (gObjIsConnected(lpMsg->iIndex))
		{
			if (lpMsg->iMoneyChanged < 0)
			{
				gObj[lpMsg->iIndex].m_PlayerData->Money -= lpMsg->iMoneyChanged;

				if (gObj[lpMsg->iIndex].m_PlayerData->Money > MAX_ZEN)
				{
					gObj[lpMsg->iIndex].m_PlayerData->Money = MAX_ZEN;
				}

				GSProtocol.GCMoneySend(lpMsg->iIndex, gObj[lpMsg->iIndex].m_PlayerData->Money);

				sLog->outBasic("[CastleSiege] [0x80][0x08] GS_DGAnsCastleMoneyChange() - Withdraw Request OK [%s][%s] (ReqMoney:%d, TotMoney:%I64d)", gObj[lpMsg->iIndex].AccountID,
					gObj[lpMsg->iIndex].Name, lpMsg->iMoneyChanged, lpMsg->i64CastleMoney);
			}
		}
		g_CastleSiege.SetCastleMoney(lpMsg->i64CastleMoney);
	}

	GSProtocol.GCAnsMoneyDrawOut(lpMsg->iIndex, lpMsg->iResult, lpMsg->i64CastleMoney);
}

struct CSP_ANS_SDEDCHANGE {
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iIndex;
	/*<thisrel this+0x10>*/ /*|0x2|*/ WORD wStartYear;
	/*<thisrel this+0x12>*/ /*|0x1|*/ BYTE btStartMonth;
	/*<thisrel this+0x13>*/ /*|0x1|*/ BYTE btStartDay;
	/*<thisrel this+0x14>*/ /*|0x2|*/ WORD wEndYear;
	/*<thisrel this+0x16>*/ /*|0x1|*/ BYTE btEndMonth;
	/*<thisrel this+0x17>*/ /*|0x1|*/ BYTE btEndDay;
};

void GS_DGAnsSiegeDateChange(BYTE* lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_SDEDCHANGE* lpMsg = (CSP_ANS_SDEDCHANGE*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x09] GS_DGAnsSiegeDateChange() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}
}

void GS_DGAnsGuildMarkRegInfo(BYTE* lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_GUILDREGINFO* lpMsg = (CSP_ANS_GUILDREGINFO*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x0A] GS_DGAnsGuildMarkRegInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	GSProtocol.GCAnsGuildRegInfo(lpMsg->iIndex, lpMsg->iResult, lpMsg);
}

struct CSP_ANS_SIEGEENDCHANGE {
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int bIsSiegeEnded;
};

void GS_DGAnsSiegeEndedChange(BYTE* lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_SIEGEENDCHANGE* lpMsg = (CSP_ANS_SIEGEENDCHANGE*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x0B] GS_DGAnsSiegeEndedChange() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}
}

struct CSP_ANS_CASTLEOWNERCHANGE
{
	PBMSG_HEAD2 h;	// C1:80:0C
	int iResult;	// 4
	WORD wMapSvrNum;	// 8
	int bIsCastleOccupied;	// C
	char szOwnerGuildName[0x8];	// 10
};

void GS_DGAnsCastleOwnerChange(BYTE* lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_CASTLEOWNERCHANGE* lpMsg = (CSP_ANS_CASTLEOWNERCHANGE*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x0C] GS_DGAnsCastleOwnerChange() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}
}

struct CSP_ANS_REGATTACKGUILD {

	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iIndex;
	/*<thisrel this+0x10>*/ /*|0x8|*/ char szEnemyGuildName[8];
};

void GS_DGAnsRegAttackGuild(BYTE* lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_REGATTACKGUILD* lpMsg = (CSP_ANS_REGATTACKGUILD*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x0D] GS_DGAnsRegAttackGuild() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	GSProtocol.GCAnsRegCastleSiege(lpMsg->iIndex, lpMsg->iResult, lpMsg->szEnemyGuildName);
}

struct CSP_ANS_CASTLESIEGEEND {
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
};

void GS_DGAnsRestartCastleState(BYTE* lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_CASTLESIEGEEND* lpMsg = (CSP_ANS_CASTLESIEGEEND*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x0E] GS_DGAnsRestartCastleState() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}
	else
	{
		g_CastleSiege.ResetCastleCycle();
	}
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : DataServer -> GameServer
*  Code      : 0xC1
*	HeadCode  :
*	SubCode	  :
*/
struct CSP_ANS_MAPSVRMULTICAST
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	char szMsgText[128];	// 6
};

void GS_DGAnsMapSvrMsgMultiCast(BYTE* lpRecv)
{
	CSP_ANS_MAPSVRMULTICAST * lpMsg = (CSP_ANS_MAPSVRMULTICAST *)lpRecv;

	if (lpMsg == NULL)
	{
		return;
	}

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x0F] GS_DGAnsMapSvrMsgMultiCast() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	char szNotice[144] = { 0 };
	std::memcpy(szNotice, lpMsg->szMsgText, 128);

	if (strlen(szNotice) > 1)
	{
		PMSG_NOTICE pNotice;

		TNotice::MakeNoticeMsg((TNotice *)&pNotice, 0, szNotice);
		TNotice::SetNoticeProperty((TNotice *)&pNotice, 0, _ARGB(255, 255, 200, 80), 1, 0, 20);
		TNotice::SendNoticeToAllUser((TNotice *)&pNotice);
	}

}

void GS_DGAnsGlobalPostMultiCast(BYTE* lpRecv)
{
	CSP_GLOBALPOST_MULTICAST * lpMsg = (CSP_GLOBALPOST_MULTICAST *)lpRecv;

	if (lpMsg == NULL)
	{
		return;
	}

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("GS_DGAnsGlobalPostMultiCast() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	PMSG_POST_DATA pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xFA, 0x11, sizeof(pMsg));

	pMsg.btColorRGB[0] = lpMsg->btColorRGB[0];
	pMsg.btColorRGB[1] = lpMsg->btColorRGB[1];
	pMsg.btColorRGB[2] = lpMsg->btColorRGB[2];

	std::memcpy(pMsg.szName, lpMsg->szSender, MAX_ACCOUNT_LEN);
	std::memcpy(pMsg.szServerName, lpMsg->szServerName, 50);

	std::strcpy(pMsg.szMessage, lpMsg->szMessage);
	std::strcpy(pMsg.szTag, Lang.GetText(0, 433));

	GSProtocol.DataSendAll((BYTE*)&pMsg, sizeof(pMsg));
}

void GS_DGAnsRegGuildMark(BYTE* lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_GUILDREGMARK* lpMsg = (CSP_ANS_GUILDREGMARK*)lpRecv;

	if (lpMsg == NULL)
	{
		return;
	}

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x10] GS_DGAnsRegGuildMark() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}
	else
	{
		GSProtocol.GCAnsRegGuildMark(lpMsg->iIndex, lpMsg->iResult, lpMsg);
	}
}

struct CSP_ANS_GUILDRESETMARK {
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iIndex;
	/*<thisrel this+0x10>*/ /*|0x8|*/ char szGuildName[8];
	/*<thisrel this+0x18>*/ /*|0x4|*/ int iRegMarkCount;
};

void GS_DGAnsGuildMarkReset(BYTE* lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_GUILDRESETMARK* lpMsg = (CSP_ANS_GUILDRESETMARK*)lpRecv;

	if (lpMsg == NULL)
	{
		return;
	}

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x11] GS_DGAnsGuildMarkReset() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}
}

struct CSP_ANS_GUILDSETGIVEUP {
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iIndex;
	/*<thisrel this+0x10>*/ /*|0x8|*/ char szGuildName[8];
	/*<thisrel this+0x18>*/ /*|0x4|*/ int bIsGiveUp;
	/*<thisrel this+0x1c>*/ /*|0x4|*/ int iRegMarkCount;
};

void GS_DGAnsGuildSetGiveUp(BYTE* lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_GUILDSETGIVEUP* lpMsg = (CSP_ANS_GUILDSETGIVEUP*)lpRecv;

	if (lpMsg == NULL)
	{
		return;
	}

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x12] GS_DGAnsGuildSetGiveUp() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	GSProtocol.GCAnsGiveUpCastleSiege(lpMsg->iIndex, lpMsg->iResult, lpMsg->bIsGiveUp, lpMsg->iRegMarkCount, lpMsg->szGuildName);
}

struct CSP_ANS_NPCREMOVE {
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iNpcNumber;
	/*<thisrel this+0x10>*/ /*|0x4|*/ int iNpcIndex;
};

void GS_DGAnsNpcRemove(BYTE* lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_NPCREMOVE* lpMsg = (CSP_ANS_NPCREMOVE*)lpRecv;

	if (lpMsg == NULL)
	{
		return;
	}

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x16] GS_DGAnsNpcRemove() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : DataServer -> GameServer
*  Code      : 0xC1
*	HeadCode  :
*	SubCode	  :
*/
struct CSP_ANS_CASTLESTATESYNC
{
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;	// 4
	int iCastleState;	// 8
	int iTaxRateChaos;	// C
	int iTaxRateStore;	// 10
	int iTaxHuntZone;	// 14
	char szOwnerGuildName[8];	// 18
};

void GS_DGAnsCastleStateSync(BYTE* lpRecv) //Identical gs-cs 56
{
	CSP_ANS_CASTLESTATESYNC * lpMsg = (CSP_ANS_CASTLESTATESYNC *)lpRecv;

	if (lpMsg == NULL)
	{
		return;
	}

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		return;
	}

	g_CastleSiegeSync.SetCastleState(lpMsg->iCastleState);
	g_CastleSiegeSync.SetTaxRateChaos(lpMsg->iTaxRateChaos);
	g_CastleSiegeSync.SetTaxRateStore(lpMsg->iTaxRateStore);
	g_CastleSiegeSync.SetTaxHuntZone(lpMsg->iTaxHuntZone);
	g_CastleSiegeSync.SetCastleOwnerGuild(lpMsg->szOwnerGuildName);
}

/* * * * * * * * * * * * * * * * * * * * *
*	Mu Castle Siege          Packet
*	Direction : DataServer -> GameServer
*  Code      : 0xC1
*	HeadCode  :
*	SubCode	  :
*/
struct CSP_ANS_CASTLETRIBUTEMONEY
{
	PBMSG_HEAD2 h;
	int iResult;	// 4
	WORD wMapSvrNum;	// 8
};

void GS_DGAnsCastleTributeMoney(BYTE* lpRecv)
{
	CSP_ANS_CASTLETRIBUTEMONEY * lpMsg = (CSP_ANS_CASTLETRIBUTEMONEY *)lpRecv;

	if (lpMsg == NULL)
	{
		return;
	}

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x18] GS_DGAnsCastleTributeMoney() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	sLog->outBasic("[CastleSiege] [0x80][0x18] GS_DGAnsCastleTributeMoney() - Money Tribute OK (%d)", g_CastleSiegeSync.GetTributeMoney());
	g_CastleSiegeSync.ResetTributeMoney();

}

struct CSP_ANS_RESETCASTLETAXINFO {
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;

};

void GS_DGAnsResetCastleTaxInfo(BYTE* lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_RESETCASTLETAXINFO* lpMsg = (CSP_ANS_RESETCASTLETAXINFO*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x19] GS_DGAnsResetCastleTaxInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}
	else
	{
		if (lpMsg->iResult == 1)
		{
			g_CastleSiege.ResetCastleTaxInfo();
		}
	}
}

struct CSP_ANS_RESETSIEGEGUILDINFO {
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
};

void GS_DGAnsResetSiegeGuildInfo(BYTE* lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_RESETSIEGEGUILDINFO* lpMsg = (CSP_ANS_RESETSIEGEGUILDINFO*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x1A] GS_DGAnsResetSiegeGuildInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}
}

struct CSP_ANS_RESETREGSIEGEINFO {
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PBMSG_HEAD2 h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
};

void GS_DGAnsResetRegSiegeInfo(BYTE* lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_RESETREGSIEGEINFO* lpMsg = (CSP_ANS_RESETREGSIEGEINFO*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x80][0x1B] GS_DGAnsResetRegSiegeInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}
}

void GS_DGAnsCastleInitData(BYTE* lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_CSINITDATA* lpMsg = (CSP_ANS_CSINITDATA*)lpRecv;
	CSP_CSINITDATA* lpMsgBody = (CSP_CSINITDATA*)(lpRecv + sizeof(CSP_ANS_CSINITDATA));

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x81] GS_DGAnsCastleInitData() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if (g_CastleSiege.GetDataLoadState() != 2)
	{
		sLog->outError("[CastleSiege] CASTLE SIEGE DATA SETTING FAILED [0x81] - m_iCastleDataLoadState != CASTLESIEGE_DATALOAD_2 (%d)", g_CastleSiege.GetDataLoadState());
		return;
	}

	g_CastleSiege.SetDataLoadState(3);

	if (lpMsg->iResult == FALSE)
	{
		sLog->outError("[CastleSiege] CASTLE SIEGE DATA SETTING FAILED [0x81] - lpMsg->iResult == 0");
		g_Log.MsgBox("[CastleSiege] CASTLE SIEGE DATA SETTING FAILED [0x81] - lpMsg->iResult == 0");
		return;
	}

	BOOL bRET_VAL = FALSE;

	bRET_VAL = g_CastleSiege.SetCastleInitData(lpMsg);

	if (bRET_VAL == FALSE)
	{
		sLog->outError("[CastleSiege] CASTLE SIEGE DATA SETTING FAILED [0x81] - g_CastleSiege.SetCastleInitData() == FALSE");
		return;
	}

	bRET_VAL = g_CastleSiege.SetCastleNpcData(lpMsgBody, lpMsg->iCount);

	if (bRET_VAL == FALSE)
	{
		sLog->outError("[CastleSiege] CASTLE SIEGE DATA SETTING FAILED [0x81] - g_CastleSiege.SetCastleNpcData() == FALSE");
		return;
	}

	g_CastleSiege.SetDbDataLoadOK(TRUE);

	g_CastleSiege.SetDataLoadState(4);

	if (lpMsg->iFirstCreate == 1)
	{
		g_CastleSiege.FirstCreateDbNPC();
	}

	g_CastleSiege.Init();
}

struct CSP_NPCDATA {
	/*<thisrel this+0x0>*/ /*|0x4|*/ int iNpcNumber;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iNpcIndex;
	/*<thisrel this+0x8>*/ /*|0x4|*/ int iNpcDfLevel;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iNpcRgLevel;
	/*<thisrel this+0x10>*/ /*|0x4|*/ int iNpcMaxHp;
	/*<thisrel this+0x14>*/ /*|0x4|*/ int iNpcHp;
	/*<thisrel this+0x18>*/ /*|0x1|*/ BYTE btNpcX;
	/*<thisrel this+0x19>*/ /*|0x1|*/ BYTE btNpcY;
	/*<thisrel this+0x1a>*/ /*|0x1|*/ BYTE btNpcDIR;
};

void GS_DGAnsCastleNpcInfo(BYTE* lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_NPCDATA* lpMsg = (CSP_ANS_NPCDATA*)lpRecv;
	CSP_NPCDATA* lpMsgBody = (CSP_NPCDATA*)(lpRecv + sizeof(CSP_ANS_NPCDATA));

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x82] GS_DGAnsCastleNpcInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}
}

struct CSP_ANS_ALLGUILDREGINFO { // <size 0x14>
	/*<thisrel this+0x0>*/ /*|0x4|*/ PWMSG_HEAD h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iIndex;
	/*<thisrel this+0x10>*/ /*|0x4|*/ int iCount;
};
struct CSP_GUILDREGINFO {// <size 0x10>
	/*<thisrel this+0x0>*/ /*|0x8|*/ char szGuildName[8];
	/*<thisrel this+0x8>*/ /*|0x4|*/ int iRegMarkCount;
	/*<thisrel this+0xc>*/ /*|0x1|*/ BYTE bIsGiveUp;
	/*<thisrel this+0xd>*/ /*|0x1|*/ BYTE btRegRank;
};
struct PMSG_ANS_CSREGGUILDLIST {// <size 0xc>
	/*<thisrel this+0x0>*/ /*|0x4|*/ PWMSG_HEAD h;
	/*<thisrel this+0x4>*/ /*|0x1|*/ BYTE btResult;
	/*<thisrel this+0x8>*/ /*|0x4|*/ int iCount;
};
struct PMSG_CSREGGUILDLIST {// <size 0xe>
	/*<thisrel this+0x0>*/ /*|0x8|*/ char szGuildName[8];
	/*<thisrel this+0x8>*/ /*|0x1|*/ BYTE btRegMarks1;
	/*<thisrel this+0x9>*/ /*|0x1|*/ BYTE btRegMarks2;
	/*<thisrel this+0xa>*/ /*|0x1|*/ BYTE btRegMarks3;
	/*<thisrel this+0xb>*/ /*|0x1|*/ BYTE btRegMarks4;
	/*<thisrel this+0xc>*/ /*|0x1|*/ BYTE btIsGiveUp;
	/*<thisrel this+0xd>*/ /*|0x1|*/ BYTE btSeqNum;
};

void GS_DGAnsAllGuildMarkRegInfo(BYTE* lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_ALLGUILDREGINFO* lpMsg = (CSP_ANS_ALLGUILDREGINFO*)lpRecv;
	CSP_GUILDREGINFO* lpMsgBody = (CSP_GUILDREGINFO*)(lpRecv + sizeof(CSP_ANS_ALLGUILDREGINFO));
	char cBUFFER[1668];
	PMSG_ANS_CSREGGUILDLIST* lpMsgSend;
	PMSG_CSREGGUILDLIST* lpMsgSendBody;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x83] GS_DGAnsAllGuildMarkRegInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if (!gObjIsConnected(lpMsg->iIndex))
		return;

	lpMsgSend = (PMSG_ANS_CSREGGUILDLIST*)cBUFFER;
	lpMsgSendBody = (PMSG_CSREGGUILDLIST*)&cBUFFER[12];

	lpMsgSend->iCount = 0;
	lpMsgSend->btResult = lpMsg->iResult;

	if (lpMsg->iResult == TRUE)
	{
		lpMsgSend->iCount = lpMsg->iCount;

		for (int i = 0; i < lpMsg->iCount; i++)
		{
			lpMsgSendBody[i].btSeqNum = lpMsgBody[i].btRegRank;
			lpMsgSendBody[i].btIsGiveUp = lpMsgBody[i].bIsGiveUp;
			lpMsgSendBody[i].btRegMarks1 = SET_NUMBERH(SET_NUMBERHW(lpMsgBody[i].iRegMarkCount));
			lpMsgSendBody[i].btRegMarks2 = SET_NUMBERL(SET_NUMBERHW(lpMsgBody[i].iRegMarkCount));
			lpMsgSendBody[i].btRegMarks3 = SET_NUMBERH(SET_NUMBERLW(lpMsgBody[i].iRegMarkCount));
			lpMsgSendBody[i].btRegMarks4 = SET_NUMBERL(SET_NUMBERLW(lpMsgBody[i].iRegMarkCount));
			std::memcpy(&lpMsgSendBody[i].szGuildName, lpMsgBody[i].szGuildName, sizeof(lpMsgSendBody[i].szGuildName));
		}
	}

	lpMsgSend->h.set((BYTE*)&cBUFFER, 0xB4, (lpMsgSend->iCount * sizeof(PMSG_CSREGGUILDLIST) + sizeof(PMSG_ANS_CSREGGUILDLIST)));

	IOCP.DataSend(lpMsg->iIndex, (BYTE*)&cBUFFER, (lpMsgSend->iCount * sizeof(PMSG_CSREGGUILDLIST) + sizeof(PMSG_ANS_CSREGGUILDLIST)));
}

struct CSP_ANS_NPCSAVEDATA {

	// static data ------------------------------------

	// non-static data --------------------------------
	/*<thisrel this+0x0>*/ /*|0x3|*/ struct PBMSG_HEAD h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
};

void GS_DGAnsFirstCreateNPC(BYTE* lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_NPCSAVEDATA* lpMsg = (CSP_ANS_NPCSAVEDATA*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x84] GS_DGAnsFirstCreateNPC() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if (lpMsg->iResult == TRUE)
	{
		if (g_CastleSiege.GetDbNpcCreated() == FALSE)
		{
			g_CastleSiege.CreateDbNPC();
			g_CastleSiege.SetDbNpcCreated(1);
		}
	}
}
struct CSP_ANS_CALCREGGUILDLIST {// <size 0x10>
	/*<thisrel this+0x0>*/ /*|0x4|*/ PWMSG_HEAD h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iCount;
};

void GS_DGAnsCalcREgGuildList(BYTE* lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_CALCREGGUILDLIST* lpMsg = (CSP_ANS_CALCREGGUILDLIST*)lpRecv;
	CSP_CALCREGGUILDLIST* lpMsgBody = (CSP_CALCREGGUILDLIST*)(lpRecv + sizeof(CSP_ANS_CALCREGGUILDLIST));

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x85] GS_DGAnsCalcREgGuildList() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if (lpMsg->iResult == TRUE)
	{
		g_CastleSiege.SetCalcRegGuildList(lpMsgBody, lpMsg->iCount);
		return;
	}

	sLog->outError("[CastleSiege] PACKET-ERROR [0x85] GS_DGAnsCalcREgGuildList() - lpMsg->iResult != 1 (%d)", lpMsg->iResult);
}

struct CSP_ANS_CSGUILDUNIONINFO {// <size 0x10>
	/*<thisrel this+0x0>*/ /*|0x4|*/ struct PWMSG_HEAD h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iCount;
};

void GS_DGAnsCsGulidUnionInfo(BYTE* lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_CSGUILDUNIONINFO* lpMsg = (CSP_ANS_CSGUILDUNIONINFO*)lpRecv;
	CSP_CSGUILDUNIONINFO* lpMsgBody = (CSP_CSGUILDUNIONINFO*)(lpRecv + sizeof(CSP_ANS_CSGUILDUNIONINFO));

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x86] GS_DGAnsCsGulidUnionInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if (lpMsg->iResult == TRUE)
	{
		g_CastleSiege.MakeCsTotalGuildInfo(lpMsgBody, lpMsg->iCount);
		return;
	}
}

struct CSP_ANS_CSSAVETOTALGUILDINFO {// <size 0xc>
	/*<thisrel this+0x0>*/ /*|0x3|*/ PBMSG_HEAD h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
};

void GS_DGAnsCsSaveTotalGuildInfo(BYTE* lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_CSSAVETOTALGUILDINFO* lpMsg = (CSP_ANS_CSSAVETOTALGUILDINFO*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x87] GS_DGAnsCsSaveTotalGuildInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if (lpMsg->iResult == TRUE)
	{
		g_CastleSiege.SetIsSiegeGuildList(TRUE);
		return;
	}
	g_CastleSiege.SetIsSiegeGuildList(FALSE);
}

struct CSP_ANS_CSLOADTOTALGUILDINFO {// <size 0x10>
	/*<thisrel this+0x0>*/ /*|0x4|*/ PWMSG_HEAD h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
	/*<thisrel this+0xc>*/ /*|0x4|*/ int iCount;
};

void GS_DGAnsCsLoadTotalGuildInfo(BYTE* lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_CSLOADTOTALGUILDINFO* lpMsg = (CSP_ANS_CSLOADTOTALGUILDINFO*)lpRecv;
	CSP_CSLOADTOTALGUILDINFO* lpMsgBody = (CSP_CSLOADTOTALGUILDINFO*)(lpRecv + sizeof(CSP_ANS_CSLOADTOTALGUILDINFO));

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x88] GS_DGAnsCsLoadTotalGuildInfo() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	if (lpMsg->iResult == TRUE)
	{
		g_CastleSiege.SetCsTotalGuildInfo(lpMsgBody, lpMsg->iCount);
		return;
	}
}

struct CSP_ANS_NPCUPDATEDATA {// <size 0xc>
	/*<thisrel this+0x0>*/ /*|0x3|*/ PBMSG_HEAD h;
	/*<thisrel this+0x4>*/ /*|0x4|*/ int iResult;
	/*<thisrel this+0x8>*/ /*|0x2|*/ WORD wMapSvrNum;
};

void GS_DGAnsCastleNpcUpdate(BYTE* lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CSP_ANS_NPCUPDATEDATA* lpMsg = (CSP_ANS_NPCUPDATEDATA*)lpRecv;

	if (lpMsg == NULL)
		return;

	if (lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup())
	{
		sLog->outError("[CastleSiege] PACKET-ERROR [0x89] GS_DGAnsCastleNpcUpdate() - lpMsg->wMapSvrNum != g_MapServerManager.GetMapSvrGroup()");
		return;
	}

	char* szResult;

	if (lpMsg->iResult != FALSE)
	{
		szResult = "SUCCEED";
	}
	else
	{
		szResult = "FAIL";
	}

	sLog->outBasic("[CastleSiege] [0x89] GS_DGAnsCastleNpcUpdate() - Npc Data Update Result : '%s'", szResult);
}



struct CWP_REQ_CRYWOLFSYNC
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
	int iCrywolfState;	// 8
	int iOccupationState;	// C
	int iApplyBenefit;
	int iApplyPenalty;
	int iPlusChaosRate;
	int iGemDropPenaltyRate;
	int iGettingExpPenaltyRate;
	int iMinusMonHPBenefitRate;
	int iKundunHPRefillState;
};

void GDReqCrywolfSync(int iMapSvrGroup, int iCrywolfState, int iOccupationState)
{
	if (iMapSvrGroup < 0)
		return;

	CWP_REQ_CRYWOLFSYNC pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0xB0, sizeof(CWP_REQ_CRYWOLFSYNC));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iCrywolfState = iCrywolfState;
	pMsg.iOccupationState = iOccupationState;
	pMsg.iApplyBenefit = g_CrywolfSync.GetApplyBenefit();
	pMsg.iApplyPenalty = g_CrywolfSync.GetApplyPenalty();
	pMsg.iPlusChaosRate = g_CrywolfSync.GetPlusChaosRate();
	pMsg.iGemDropPenaltyRate = g_CrywolfSync.GetGemDropPenaltiyRate(); // zapisz, bo mi dzis prad wylaczaja czesto k
	pMsg.iGettingExpPenaltyRate = g_CrywolfSync.GetGettingExpPenaltyRate();
	pMsg.iMinusMonHPBenefitRate = g_CrywolfSync.GetMonHPBenefitRate();
	pMsg.iKundunHPRefillState = g_CrywolfSync.GetKundunHPRefillState();

	wsDataCli.DataSend((PCHAR)&pMsg, pMsg.h.size);
}



struct CWP_ANS_CRYWOLFSYNC
{
	PBMSG_HEAD h;
	BYTE btResult;	// 3
	int iCrywolfState;	// 4
	int iOccupationState;	// 8
	int iApplyBenefit;
	int iApplyPenalty;
	int iPlusChaosRate;
	int iGemDropPenaltyRate;
	int iGettingExpPenaltyRate;
	int iMinusMonHPBenefitRate;
	int iKundunHPRefillState;
};


DWORD dwCrywolfSyncLogTickCount = 0;

void DGAnsCrywolfSync(BYTE* lpRecv)
{
	CWP_ANS_CRYWOLFSYNC * lpMsg = (CWP_ANS_CRYWOLFSYNC *)lpRecv;

	if (lpMsg == NULL)
		return;

	g_CrywolfSync.SetApplyBenefit(lpMsg->iApplyBenefit);
	g_CrywolfSync.SetApplyPenalty(lpMsg->iApplyPenalty);
	g_CrywolfSync.SetPlusChaosRate(lpMsg->iPlusChaosRate);
	g_CrywolfSync.SetGemDropPenaltiyRate(lpMsg->iGemDropPenaltyRate);
	g_CrywolfSync.SetGettingExpPenaltyRate(lpMsg->iGettingExpPenaltyRate);
	g_CrywolfSync.SetMonHPBenefitRate(lpMsg->iMinusMonHPBenefitRate);
	g_CrywolfSync.SetKundunHPRefillState(lpMsg->iKundunHPRefillState);

	int iOldOccupationState = g_CrywolfSync.GetOccupationState();
	g_CrywolfSync.SetCrywolfState(lpMsg->iCrywolfState);
	g_CrywolfSync.SetOccupationState(lpMsg->iOccupationState);

	if (GetTickCount() > (dwCrywolfSyncLogTickCount + 60000))
	{
		sLog->outBasic("[Crywolf Sync] Occupation: %d, State: %d, Benefit: %d, Penalty: %d, PlusChaosRate: %d, GemDropPenaltyRate: %d, ExpPenaltyRate: %d, MinusMonHPBenefitRate: %d, KundunHPRefillState: %d",
			lpMsg->iOccupationState, lpMsg->iCrywolfState, lpMsg->iApplyBenefit, lpMsg->iApplyPenalty, lpMsg->iPlusChaosRate, lpMsg->iGemDropPenaltyRate, lpMsg->iGettingExpPenaltyRate, lpMsg->iMinusMonHPBenefitRate, lpMsg->iKundunHPRefillState);
		dwCrywolfSyncLogTickCount = GetTickCount();
	}
}

struct CWP_REQ_CRYWOLFINFOLOAD
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
};

void GDReqCrywolfInfoLoad(int iMapSvrGroup)
{
	if (iMapSvrGroup < 0)
		return;

	CWP_REQ_CRYWOLFINFOLOAD pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0xB1, sizeof(CWP_REQ_CRYWOLFINFOLOAD));
	pMsg.wMapSvrNum = iMapSvrGroup;

	wsDataCli.DataSend((PCHAR)&pMsg, pMsg.h.size);
}


struct CWP_ANS_CRYWOLFINFOLOAD
{
	PBMSG_HEAD h;
	BYTE btResult;	// 3
	int iCrywolfState;	// 4
	int iOccupationState;	// 8
};

void DGAnsCrywolfInfoLoad(BYTE* lpRecv)
{
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		return;
	}

	CWP_ANS_CRYWOLFINFOLOAD * lpMsg = (CWP_ANS_CRYWOLFINFOLOAD *)lpRecv;

	if (lpMsg == NULL)
	{
		return;
	}

	int iResult;

	if (true != FALSE && lpMsg->iOccupationState >= 0 && lpMsg->iOccupationState <= 2)
	{
		iResult = 1;
	}
	else
	{
		iResult = 0;
	}

	lpMsg->btResult = iResult;

	if ((iResult & 0xFF) != 0)
	{
		g_Crywolf.ApplyCrywolfDBInfo(lpMsg->iCrywolfState, lpMsg->iOccupationState);
		return;
	}
}

struct CWP_REQ_CRYWOLFINFOSAVE
{
	PBMSG_HEAD h;
	WORD wMapSvrNum;	// 4
	int iCrywolfState;	// 8
	int iOccupationState;	// C
};


void GDReqCrywolfInfoSave(int iMapSvrGroup, int iCrywolfState, int iOccupationState)
{
	if (iMapSvrGroup < 0)
		return;

	CWP_REQ_CRYWOLFINFOSAVE pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0xB2, sizeof(CWP_REQ_CRYWOLFINFOSAVE));
	pMsg.wMapSvrNum = iMapSvrGroup;
	pMsg.iCrywolfState = iCrywolfState;
	pMsg.iOccupationState = iOccupationState;

	wsDataCli.DataSend((PCHAR)&pMsg, pMsg.h.size);
}



struct CWP_ANS_CRYWOLFINFOSAVE
{
	PBMSG_HEAD h;
	BYTE btResult;	// 3
};

void DGAnsCrywolfInfoSave(BYTE* lpRecv)
{
	CWP_ANS_CRYWOLFINFOSAVE * lpMsg = (CWP_ANS_CRYWOLFINFOSAVE *)lpRecv;
}

void GDReqInGameShopItemList(CGameObject &Obj, BYTE InventoryType, BYTE InventoryNum)
{
	ISHOP_REQ_ITEMLIST pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xD2, 0x02, sizeof(pMsg));

	std::memcpy(pMsg.AccountID, Obj.AccountID, 11);
	pMsg.aIndex = Obj.m_Index;
	pMsg.InvType = InventoryType;
	pMsg.InvNum = InventoryNum;

	wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg));
}

void DGAnsInGameShopItemList(BYTE* lpRecv)
{
	ISHOP_ANS_ITEMLIST * lpMsg = (ISHOP_ANS_ITEMLIST *)(lpRecv);
	ISHOP_ITEMLIST * lpMsgBody = (ISHOP_ITEMLIST *)(lpRecv + sizeof(ISHOP_ANS_ITEMLIST));

	CGameObject* lpObj = getGameObject(lpMsg->aIndex);

	g_CashShop.GCCashInventoryItemCount(*lpObj, lpRecv);
}

void GDReqInGameShopPoint(CGameObject &Obj)
{
	ISHOP_REQ_POINT pMsg = { 0 };

	PHeadSetB((BYTE*)&pMsg, 0xD1, sizeof(pMsg));
	std::memcpy(pMsg.AccountID, Obj.AccountID, 11);
	pMsg.aIndex = Obj.m_Index;

	wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg));
}

void DGAnsInGameShopPoint(ISHOP_ANS_POINT *lpMsg)
{
	CGameObject* lpObj = getGameObject(lpMsg->aIndex);

	lpObj->m_PlayerData->m_WCoinC = lpMsg->CoinC;
	lpObj->m_PlayerData->m_WCoinP = lpMsg->CoinP;
	lpObj->m_PlayerData->m_GoblinPoint = lpMsg->Goblin;

	g_CashShop.GCCashPoint(*lpObj);
}

void GDReqInGameShopPointAdd(CGameObject &Obj, BYTE Type, float Coin)
{
	ISHOP_POINT_ADD pMsg = { 0 };

	PHeadSetB((BYTE*)&pMsg, 0xD7, sizeof(pMsg));

	pMsg.aIndex = Obj.m_Index;
	std::memcpy(pMsg.AccountID, Obj.AccountID, 11);
	pMsg.Type = Type;
	pMsg.Coin = Coin;

	wsDataCli.DataSend((PCHAR)&pMsg, pMsg.h.size);
}

void DGAnsInGameShopItemBuy(BYTE* lpRecv)
{
	ISHOP_ITEM_BUYANS * pItem = (ISHOP_ITEM_BUYANS *)(lpRecv);

	g_CashShop.GCCashItemBuy(*getGameObject(pItem->aIndex), pItem->ID1, pItem->ID2, pItem->ID3, pItem->Result);
}

void GDReqInGameShopItemBuy(CGameObject &Obj, int ID1, int ID2, int ID3, int Price, BYTE CoinType)
{
	ISHOP_ITEM_BUY pMsg;
	PHeadSetB((BYTE*)&pMsg, 0xD5, sizeof(pMsg));

	pMsg.aIndex = Obj.m_Index;
	std::memcpy(pMsg.AccountID, lpObj->AccountID, 11);
	pMsg.ID1 = ID1;
	pMsg.ID2 = ID2;
	pMsg.ID3 = ID3;
	pMsg.Price = Price;
	pMsg.CoinType = CoinType;

	wsDataCli.DataSend((PCHAR)&pMsg, pMsg.h.size);
}

void GDReqInGameShopItemUse(CGameObject &Obj, int UniqueCode, int AuthCode)
{
	ISHOP_ITEM_USE pMsg;
	PHeadSetB((BYTE*)&pMsg, 0xD9, sizeof(pMsg));

	pMsg.aIndex = Obj.m_Index;
	std::memcpy(pMsg.AccountID, lpObj->AccountID, 11);
	pMsg.UniqueCode = UniqueCode;
	pMsg.AuthCode = AuthCode;

	wsDataCli.DataSend((PCHAR)&pMsg, pMsg.h.size);
}

void DGAnsInGameShopItemUse(ISHOP_ITEM_USEANS * lpMsg)
{
	if (!ObjectMaxRange(*lpObj))
		return;

	g_CashShop.GCCashInventoryItemUse(*lpObj, lpMsg->Result, lpMsg->UniqueCode, lpMsg->AuthCode, lpMsg->ID1, lpMsg->ID2, lpMsg->ID3);
}

void GDReqInGameShopItemGift(CGameObject &Obj, int ID1, int ID2, int ID3, int Price, BYTE CoinType, char Target[11], char Message[200])
{
	ISHOP_ITEM_GIFT pMsg;
	PHeadSetW((BYTE*)&pMsg, 0xD6, sizeof(pMsg));

	pMsg.aIndex = Obj.m_Index;
	std::memcpy(pMsg.AccountID, lpObj->AccountID, 11);
	pMsg.ID1 = ID1;
	pMsg.ID2 = ID2;
	pMsg.ID3 = ID3;
	pMsg.Price = Price;
	pMsg.CoinType = CoinType;
	std::memcpy(pMsg.TargetName, Target, 11);
	std::memcpy(pMsg.Name, lpObj->Name, 11);
	std::memcpy(pMsg.Message, Message, 200);

	wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg));
}

void DGAnsInGameShopItemGift(BYTE* lpRecv)
{
	ISHOP_ITEM_GIFTANS* pItem = (ISHOP_ITEM_GIFTANS *)(lpRecv);
	CGameObject* lpObj = getGameObject(pItem->aIndex);
	g_CashShop.GCCashItemGift(*lpObj, pItem->ID1, pItem->ID2, pItem->ID3, pItem->Result);
}

void GDReqInGameShopItemDelete(CGameObject &Obj, int UniqueCode, int AuthCode)
{
	ISHOP_ITEM_DELETE pMsg;
	PHeadSetB((BYTE*)&pMsg, 0xDB, sizeof(pMsg));

	std::memcpy(pMsg.AccountID, Obj.AccountID, MAX_ACCOUNT_LEN + 1);
	pMsg.UniqueCode = UniqueCode;
	pMsg.AuthCode = AuthCode;

	wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg));
}

void GDReqInGameShopItemRollbackUse(CGameObject &Obj, int UniqueCode, int AuthCode)
{
	ISHOP_ITEM_ROLLBACK pMsg;
	PHeadSetB((BYTE*)&pMsg, 0xDC, sizeof(pMsg));

	std::memcpy(pMsg.AccountID, lpObj->AccountID, MAX_ACCOUNT_LEN + 1);
	pMsg.UniqueCode = UniqueCode;
	pMsg.AuthCode = AuthCode;

	wsDataCli.DataSend((PCHAR)&pMsg, sizeof(pMsg));
}

void GDReqBanUser(CGameObject &Obj, BYTE Type, BYTE Ban)
{
	BAN_REQ_USER pBan;

	PHeadSetB((BYTE*)&pBan, 0xFA, sizeof(pBan));

	pBan.Type = Type;
	pBan.Ban = Ban;

	if (pBan.Type == 1)
	{
		std::memcpy(pBan.AccName, lpObj->AccountID, 11);
	}
	else
	{
		std::memcpy(pBan.AccName, lpObj->Name, 11);
	}

	wsDataCli.DataSend((PCHAR)&pBan, pBan.h.size);
}

struct SDHP_EXPANDEDWAREHOUSE_SET
{
	PBMSG_HEAD h;
	char AccountID[11];
	BYTE ExpansionType;
	BYTE ExpansionLevel;
};

void GDSetExpWare(CGameObject &Obj, BYTE ExpansionType, BYTE ExpansionLevel)
{
	SDHP_EXPANDEDWAREHOUSE_SET pMsg;
	PHeadSetB((BYTE*)&pMsg, 0xDA, sizeof(pMsg));

	pMsg.ExpansionType = ExpansionType;

	if (pMsg.ExpansionType == 2)
	{
		std::memcpy(pMsg.AccountID, lpObj->AccountID, 11);
	}
	else
	{
		std::memcpy(pMsg.AccountID, lpObj->Name, 11);
	}

	pMsg.ExpansionLevel = ExpansionLevel;

	wsDataCli.DataSend((PCHAR)&pMsg, pMsg.h.size);
}

void GDCharCardBuy(char AccountID[11], BYTE Type)
{
	ISHOP_CHARCARD_BUY pCard;

	PHeadSetB((BYTE*)&pCard, 0xD4, sizeof(pCard));

	std::memcpy(pCard.AccountID, AccountID, 11);

	pCard.Type = Type;

	wsDataCli.DataSend((PCHAR)&pCard, pCard.h.size);
}

void DGSetWarehouseList(PMSG_ANS_WARESAVE *lpMsg)
{
	if (!ObjectMaxRange(lpMsg->iIndex))
	{
		return;
	}

	if (gObj[lpMsg->iIndex].Type != OBJ_USER)
	{
		return;
	}

	gObj[lpMsg->iIndex].WarehouseSaveLock = FALSE;

	if (lpMsg->CloseWindow == TRUE)
	{
		gObj[lpMsg->iIndex].m_btOpenWarehouse = FALSE;
		gObj[lpMsg->iIndex].WarehouseTick = GetTickCount();

		if (gObj[lpMsg->iIndex].m_IfState->use == 1 && gObj[lpMsg->iIndex].m_IfState->type == 6)
		{
			gObj[lpMsg->iIndex].m_IfState->use = 0;
			gObj[lpMsg->iIndex].m_IfState->type = 0;
			gObj[lpMsg->iIndex].m_IfState->state = 0;
		}
	}

}

void GDReqSecLock(CGameObject &Obj, int Code)
{
	SECLOCK_REQ_SAVE pLock;

	PHeadSetB((BYTE*)&pLock, 0xFB, sizeof(pLock));

	std::memcpy(pLock.AccountID, lpObj->AccountID, 11);

	pLock.Code = Code;

	wsDataCli.DataSend((char *)&pLock, pLock.h.size);
}

void ReqMonsterCount(CGameObject &Obj)
{
	DS_GET_MONSTERCNT pMsg;
	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xFD;
	pMsg.h.size = sizeof(pMsg);
	pMsg.aIndex = Obj.m_Index;
	std::memcpy(pMsg.character, lpObj->Name, 10);
	pMsg.character[10] = 0;
	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);

}

void ReqMonsterCountAns(DS_GET_MONSTERCNT * aRecv)
{
	CGameObject* lpObj = getGameObject(aRecv->aIndex);
	if (lpObj->Connected == PLAYER_PLAYING && lpObj->Type == OBJ_USER)
	{
		lpObj->m_PlayerData->IGC_KillCount = aRecv->cnt;
	}
}

void ReqSaveMonsterCount(CGameObject &Obj, int monsterid)
{
	DS_SAVE_MONSTERCNT pMsg;
	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xFC;
	pMsg.h.size = sizeof(pMsg);
	pMsg.type = monsterid;
	std::memcpy(pMsg.character, lpObj->Name, 10);
	pMsg.character[10] = 0;
	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
}

void ReqSavePlayerKiller(short kIndex, short vIndex)
{
	DS_SAVE_PLAYERKILLER pMsg;
	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xFE;
	pMsg.h.size = sizeof(pMsg);
	std::memcpy(pMsg.Victim, gObj[vIndex].Name, 10);
	pMsg.Victim[10] = 0;
	std::memcpy(pMsg.Killer, gObj[kIndex].Name, 10);
	pMsg.Killer[10] = 0;
	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
}

void GDReqArcaBattleGuildJoin(CGameObject &Obj)
{
	PMSG_REQ_ARCA_BATTLE_GUILD_JOIN_DS pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xF8;
	pMsg.h.subcode = 0x30;
	std::memcpy(pMsg.szGuildMaster, obj->Name, MAX_ACCOUNT_LEN + 1);
	std::memcpy(pMsg.szGuildName, obj->m_PlayerData->lpGuild->Name, MAX_GUILD_LEN + 1);
	pMsg.wNumber = obj->m_Index;
	pMsg.dwGuild = obj->m_PlayerData->GuildNumber;
	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

void DGAnsArcaBattleGuildJoin(PMSG_ANS_ARCA_BATTLE_GUILD_JOIN_DS *lpMsg)
{
	PMSG_ANS_ARCA_BATTLE_JOIN pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xF8;
	pMsg.h.subcode = 0x31;
	pMsg.btResult = lpMsg->btResult;
	IOCP.DataSend(lpMsg->wNumber, (BYTE*)&pMsg, sizeof(pMsg));
}

void DGAnsArcaBattleGuildJoinSelect(PMSG_ANS_ARCA_BATTLE_GUILD_JOIN_DS *lpMsg)
{
	PMSG_ANS_ARCA_BATTLE_JOIN pMsg;
	CGameObject *lpObj = getGameObject(lpMsg->wNumber);

	if (lpObj->Type != OBJ_USER)
		return;

	if (gObjIsConnected(lpObj))
	{
		switch (lpMsg->btResult)
		{
		case 0:
			sLog->outBasic("[ArcaBattle] [%s][%s][%s] ArcaBattle Guild Join", lpObj->AccountID, lpObj->Name, lpObj->m_PlayerData->GuildName);
			GDReqArcaBattleGuildJoin(*lpObj);
			return;

		case 1:
			sLog->outBasic("[ArcaBattle] [%s][%s] ArcaBattle Join Fail to Not Guild Master", lpObj->AccountID, lpObj->Name);
			break;

		case 2:
			sLog->outBasic("[ArcaBattle] [%s][%s] ArcaBattle Join Fail to Below Guild Member", lpObj->AccountID, lpObj->Name);
			break;

		case 3:
			sLog->outBasic("[ArcaBattle] [%s][%s] ArcaBattle Join Fail to Guild Over", lpObj->AccountID, lpObj->Name);
			break;

		case 4:
			sLog->outBasic("[ArcaBattle] [%s][%s] ArcaBattle Join Fail to be Guild Name Same", lpObj->AccountID, lpObj->Name);
			break;

		case 5:
			sLog->outBasic("[ArcaBattle] [%s][%s] (Guild Master) ArcaBattle Join Fail to Time Over", lpObj->AccountID, lpObj->Name);
			break;

		case 6:
			sLog->outBasic("[ArcaBattle] [%s][%s] ArcaBattle Join Fail to not enough Guild Rank", lpObj->AccountID, lpObj->Name);
			break;

		default:
			sLog->outBasic("[ArcaBattle] [%s][%s] GuildJoin Error Result : %d", lpObj->AccountID, lpObj->Name, lpMsg->btResult);
			break;
		}

		pMsg.h.c = 0xC1;
		pMsg.h.size = sizeof(pMsg);
		pMsg.h.headcode = 0xF8;
		pMsg.h.subcode = 0x31;
		pMsg.btResult = lpMsg->btResult;
		IOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
	}
}

void GDReqArcaBattleGuildMemberJoin(CGameObject &Obj)
{
	_tagPMSG_REQ_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xF8;
	pMsg.h.subcode = 0x32;
	std::memcpy(pMsg.szCharName, obj->Name, MAX_ACCOUNT_LEN + 1);
	std::memcpy(pMsg.szGuildName, obj->m_PlayerData->lpGuild->Name, MAX_GUILD_LEN + 1);
	pMsg.dwGuild = obj->m_PlayerData->GuildNumber;
	pMsg.wNumber = obj->m_Index;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

void DGAnsArcaBattleGuildMemberJoin(PMSG_ANS_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS *lpMsg)
{
	PMSG_ANS_ARCA_BATTLE_JOIN pMsg;

	CGameObject* lpObj = getGameObject(pMsg->aIndex);

	if (lpObj->Type != OBJ_USER)
		return;

	if (gObjIsConnected(Obj))
	{
		switch (lpMsg->btResult)
		{
		case 0:
			sLog->outBasic("[ArcaBattle] [%s][%s][%s] ArcaBattle Guild Member Join", lpObj->AccountID, lpObj->Name, lpObj->m_PlayerData->GuildName);
			return;

		case 7:
			sLog->outBasic("[ArcaBattle] [%s][%s] ArcaBattle Join Fail to Not Guild Member", lpObj->AccountID, lpObj->Name);
			break;

		case 8:
			sLog->outBasic("[ArcaBattle] [%s][%s] ArcaBattle Join Fail to be Name Same", lpObj->AccountID, lpObj->Name);
			break;

		case 9:
			sLog->outBasic("[ArcaBattle] [%s][%s] (Guild Member) ArcaBattle Join Fail to Guild Member Over", lpObj->AccountID, lpObj->Name);
			break;

		case 10:
			sLog->outBasic("[ArcaBattle] [%s][%s] (Guild Member) ArcaBattle Join Fail to Time Over", lpObj->AccountID, lpObj->Name);
			break;

		case 13:
			sLog->outBasic("[ArcaBattle] [%s][%s] (Guild Master) ArcaBattle Join Fail GuildMaster Will be Registered Automatically", lpObj->AccountID, lpObj->Name);
			break;

		case 14:
			sLog->outBasic("[ArcaBattle] [%s][%s] ArcaBattle Join Fail to Guild Master Join Time ", lpObj->AccountID, lpObj->Name);
			break;

		default:
			sLog->outBasic("[ArcaBattle] [%s][%s] GuildMemberJoin Error Result : %d", lpObj->AccountID, lpObj->Name, lpMsg->btResult);
			break;
		}

		pMsg.h.c = 0xC1;
		pMsg.h.size = sizeof(pMsg);
		pMsg.h.headcode = 0xF8;
		pMsg.h.subcode = 0x33;
		pMsg.btResult = lpMsg->btResult;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
	}
}

void GDReqArcaBattleEnter(CGameObject &Obj, BYTE btEnterSeq)
{
	PMSG_REQ_ARCA_BATTLE_ENTER_DS pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xF8;
	pMsg.h.subcode = 0x34;
	pMsg.h.size = sizeof(pMsg);
	std::memcpy(pMsg.szCharName, obj->Name, MAX_ACCOUNT_LEN + 1);
	pMsg.wNumber = obj->m_Index;
	pMsg.btEnterSeq = btEnterSeq;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

void DGAnsArcaBattleEnter(PMSG_ANS_ARCA_BATTLE_ENTER_DS *lpMsg)
{
	CGameObject* lpObj = getGameObject(lpMsg->wNumber);

	if (gObjIsConnected(*lpObj) == FALSE)
	{
		return;
	}

	if (lpObj->Teleport != FALSE)
	{
		return;
	}

	if (lpMsg->btResult == 11)
	{
		sLog->outBasic("[ArcaBattle] [%s][%s] ArcaBattle Enter Fail to Not Join Guild Master", lpObj->AccountID, lpObj->Name);
	}

	else if (lpMsg->btResult == 12)
	{
		sLog->outBasic("[ArcaBattle] [%s][%s] ArcaBattle Enter Fail to Not Play Time", lpObj->AccountID, lpObj->Name);
	}

	else if (lpMsg->btResult == 0)
	{
		if (lpMsg->btEnterSeq == TRUE)
		{
			gObjMoveGate(*lpObj, 426);
			sLog->outBasic("[ArcaBattle] [%s][%s][%s] ArcaBattle Enter", lpObj->AccountID, lpObj->Name, lpObj->m_PlayerData->GuildName);
			return;
		}
	}

	else
	{
		sLog->outBasic("[ArcaBattle] [%s][%s] Enter Error Result : %d", lpObj->AccountID, lpObj->Name, lpMsg->btResult);
		return;
	}

	PMSG_ANS_ARCA_BATTLE_JOIN pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xF8;
	pMsg.h.subcode = 0x35;
	pMsg.btResult = lpMsg->btResult;
	IOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
}

void DGAnsArcaBattleWinGuildInfo(PMSG_ANS_AB_WIN_GUILD_INFO_DS *lpMsg)
{
	g_ArcaBattle.AddArcaBattleWinGuildInfo(lpMsg->m_stABWinGuildInfoDS, lpMsg->btGuildCnt);
}

void DGReqArcaBattleProcMultiCast(PMSG_ANS_AB_PROC_STATE_DS *lpMsg)
{
	if (!g_ArcaBattle.IsArcaBattleServer())
	{
		g_ArcaBattle.SetState(lpMsg->btProcState);
	}

	sLog->outBasic("[ArcaBattle] ArcaBattle Proc State [%d]", lpMsg->btProcState);
}

void GDAnsJoinMemberUnder(PMSG_ANS_AB_JOIN_MEMBER_UNDER_DS *lpMsg)
{
	_tagPMSG_ANS_AB_JOIN_MEMBER_UNDER pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xF8;
	pMsg.h.subcode = 0x3E;
	pMsg.btMinGuildMember = g_ArcaBattle.GetJoinMemberCnt();

	for (int i = 0; i < lpMsg->btGuildCnt; ++i)
	{
		GUILD_INFO_STRUCT * lpGuild = Guild.SearchGuild(lpMsg->GuildMemberCnt[i].szGuildNames);

		if (lpMsg->GuildMemberCnt[i].btGuildMemberCnt < g_ArcaBattle.GetJoinMemberCnt() && lpGuild != NULL)
		{
			for (int j = 0; j < 80; j++)
			{
				if (lpGuild->GuildStatus[j] == GUILD_MASTER)
				{
					if (gObjIsConnected(lpGuild->Index[j]) == 1)
					{
						pMsg.btGuildMemberCnt = lpMsg->GuildMemberCnt[i].btGuildMemberCnt;
						IOCP.DataSend(lpGuild->Index[j], (BYTE*)&pMsg, pMsg.h.size);
					}

					break;
				}
			}
		}
	}
}

void DGAns_CCF_Info_Notify(SDHP_ANS_CCF_INFO_NOTIFY *lpMsg)
{
	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		return;
	}

	char *szTemp;

	switch (lpMsg->nType)
	{
	case 1:
		szTemp = "Preliminary Battle";
		break;
	case 2:
		szTemp = "Semi Final Battle";
		break;
	case 3:
		szTemp = "The final battle";
		break;
	}

	char szMsg[256];

	for (int nUser = g_ConfigRead.server.GetObjectStartUserIndex(); nUser < g_ConfigRead.server.GetObjectMax(); nUser++)
	{
		LPOBJ lpObj = &gObj[nUser];

		if (gObjIsConnected(nUser) && lpObj->Type == OBJ_USER)
		{
			std::memset(szMsg, 0x00, sizeof(szMsg));
			sprintf(szMsg, Lang.GetText(0, 567), szTemp, lpMsg->nMin);

			GSProtocol.GCServerMsgStringSend(szMsg, nUser, 0);
		}
	}
}

void DGAns_GetCCFPermission(SDHP_ANS_CCF_GETPERMISSION *lpMsg)
{
	if (gObjIsConnected(lpMsg->nIndex) == false)
	{
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->nIndex];

	BYTE byCCF_Enter_Result = -1;

	if (lpMsg->nResult)
	{
		byCCF_Enter_Result = 3;
	}

	else
	{
		if (lpObj->m_byCCF_CanEnter != 0xFF || lpObj->m_bCCF_UI_Using != TRUE)
		{
			byCCF_Enter_Result = 6;
		}

		else
		{
			DWORD nNowTick = GetTickCount();
			DWORD nUserTick = lpObj->m_nCCF_CertiTick;
			DWORD nTickGap = nNowTick - nUserTick;
			if (nTickGap >= 60000)
				byCCF_Enter_Result = 4;
			else
				byCCF_Enter_Result = 0;
		}
	}

	PMSG_RESULT_CCF_ENTERCHECK pResult;

	PHeadSubSetB((BYTE*)&pResult, 0xAF, 0x05, sizeof(pResult));

	if (byCCF_Enter_Result)
	{
		pResult.byResult = 3;
		pResult.byRemainTime = 0;
		pResult.byRemainTime_LOW = 0;

		IOCP.DataSend(lpMsg->nIndex, (BYTE*)&pResult, pResult.h.size);
		lpObj->m_byCCF_CanEnter = 0;
		lpObj->m_bCCF_UI_Using = false;
	}

	else
	{
		int nResult = GSProtocol.CGReqEnterChaosCastleFinal(lpMsg->nIndex);

		if (nResult)
		{
			sLog->outBasic("[CGReqCCF_EnterCheck][CGReqEnterChaosCastleFinal] ACC :%s , NAME :%s return :%d ", lpObj->AccountID, lpObj->Name, nResult);
		}
	}
}

void GCMuunInventoryItemListSend(CGameObject &Obj)
{
	int itemcount;
	int iUserIndex;
	BYTE sendBuf[4096];
	unsigned int pMsgILSize;
	unsigned int sOfs;
	PMSG_INVENTORYLIST pMsgIL;
	PMSG_INVENTORYLISTCOUNT pMsgILC;

	sOfs = 6;
	pMsgILSize = 13;
	itemcount = 0;
	for (int n = 0; n < MUUN_INVENTORY_SIZE; ++n)
	{
		if (Obj.pntMuunInventory[n]->IsItem() == 1)
		{
			if (Obj.pntMuunInventory[n]->m_bItemExist)
			{
				/*			pMsgIL.Pos = n;
				ItemByteConvert(pMsgIL.ItemInfo,lpObj->pntInventory[n]);
				std::memcpy(&sendBuf[sOfs], &pMsgIL, pMsgILSize);
				itemcount++;
				sOfs += pMsgILSize;*/
				pMsgIL.Pos = n;
				//std::memcpy(&sendBuf[3920], &lpObj->pntMuunInventory[n], 0xC8u);
				ItemByteConvert(pMsgIL.ItemInfo, *Obj.pntMuunInventory[n]);
				std::memcpy(&sendBuf[sOfs], &pMsgIL, pMsgILSize);
				++itemcount;
				sOfs += pMsgILSize;
			}
		}
	}

	pMsgILC.h.c = 0xC4;
	pMsgILC.h.headcode = 0x4E;
	pMsgILC.subcode = 0x02;
	pMsgILC.h.sizeH = SET_NUMBERH(sOfs);
	pMsgILC.h.sizeL = SET_NUMBERL(sOfs);
	pMsgILC.Count = itemcount;
	std::memcpy(sendBuf, &pMsgILC, sizeof(PMSG_INVENTORYLISTCOUNT));
	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, sendBuf, sOfs);
}

void GDReqSaveEventInvenItem(CGameObject &Obj)
{
	if (g_CMuRummyMng.IsMuRummyEventOn() == false && g_ConfigRead.data.common.EventInventoryOpen == false)
	{
		return;
	}

	if (Obj.EventInventoryLoad == false)
	{
		return;
	}

	if (!gObjIsConnected(Obj))
	{
		return;
	}

	BYTE NewOption[MAX_EXOPTION_SIZE];

	for (int n = 0; n < EVENT_INVENTORY_SIZE; n++)
	{
		if (Obj.pntEventInventory[n]->IsItem() == TRUE)
		{
			ItemIsBufExOption(NewOption, Obj.pntEventInventory[n]);

		}
	}

	SDHP_REQ_DBEVENT_INVEN_SAVE pMsg;
	std::memcpy(pMsg.AccountID, Obj.AccountID, MAX_ACCOUNT_LEN + 1);
	std::memcpy(pMsg.Name, Obj.Name, MAX_ACCOUNT_LEN + 1);

	pMsg.h.c = 0xC2;
	pMsg.h.sizeH = HIBYTE(sizeof(pMsg));
	pMsg.h.sizeL = LOBYTE(sizeof(pMsg));
	pMsg.h.headcode = 0xE7;
	ItemByteConvert32(pMsg.dbInventory, Obj.pntEventInventory, 32);

	wsDataCli.DataSend((char *)&pMsg, sizeof(pMsg));
}

void GDReqLoadEventInvenItem(CGameObject &Obj)
{
	if (g_CMuRummyMng.IsMuRummyEventOn() == false && g_ConfigRead.data.common.EventInventoryOpen == false)
	{
		return;
	}

	if (!gObjIsConnected(Obj))
	{
		return;
	}

	SDHP_REQ_DBEVENT_INVEN_LOAD pMsg;
	std::memcpy(pMsg.AccountID, Obj.AccountID, MAX_ACCOUNT_LEN + 1);
	std::memcpy(pMsg.Name, Obj.Name, MAX_ACCOUNT_LEN + 1);

	pMsg.aIndex = Obj.m_Index;
	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xE6;

	wsDataCli.DataSend((char *)&pMsg, sizeof(pMsg));
}

void DGLoadEventInvenItem(SDHP_ANS_DBEVENT_INVEN_LOAD *lpMsg)
{
	if (g_CMuRummyMng.IsMuRummyEventOn() == false && g_ConfigRead.data.common.EventInventoryOpen == false)
	{
		return;
	}

	char szId[11];
	CGameObject* lpObj = getGameObject(lpMsg->aIndex);

	szId[MAX_ACCOUNT_LEN] = 0;
	std::memcpy(szId, lpObj->AccountID, MAX_ACCOUNT_LEN);

	if (lpObj->m_bMapSvrMoveReq == true)
	{
		return;
	}

	if (lpObj->m_State == 32)
	{
		return;
	}

	if (lpObj->m_bMapSvrMoveQuit == true)
	{
		return;
	}

	if (!gObjIsAccontConnect(*lpObj, szId))
	{
		return;
	}

	lpObj->EventInventoryLoad = true;

	int itype;
	int _type;
	BYTE OptionData;
	CItemObject* lpItem;

	std::memset(lpObj->EventInventoryMap, (BYTE)-1, EVENT_INVENTORY_MAP_SIZE);

	int count = 0;
	for (int n = 0; n < EVENT_INVENTORY_SIZE; n++)
	{
		itype = lpMsg->dbItems[n*MAX_DBITEM_INFO];

		itype |= (lpMsg->dbItems[n*MAX_DBITEM_INFO + 9] & 0xF0) << 5;
		itype |= (lpMsg->dbItems[n*MAX_DBITEM_INFO + 7] & 0x80) << 1;
		_type = itype;

		lpObj->pntEventInventory[n]->Clear();

		if (lpMsg->dbItems[n*MAX_DBITEM_INFO] == (BYTE)-1 && (lpMsg->dbItems[n*MAX_DBITEM_INFO + 9] & 0xF0) == 0xF0 && (lpMsg->dbItems[n*MAX_DBITEM_INFO + 7] & 0x80) == 0x80)
		{
			itype = -1;
		}

		if (IsItem(_type) == FALSE)
		{
			itype = -1;
		}

		if (_type == ITEMGET(14, 17) || _type == ITEMGET(14, 18))
		{
			if (lpMsg->dbItems[n*MAX_DBITEM_INFO + 2] == 0)
			{
				_type = -1;
			}
		}

		if (_type == ITEMGET(14, 19))
		{
			if (lpMsg->dbItems[n*MAX_DBITEM_INFO + 2] == (BYTE)-1)
			{
				itype = -1;
			}
		}

		if (itype != -1)
		{
			lpItem->m_Level = DBI_GET_LEVEL(lpMsg->dbItems[n*MAX_DBITEM_INFO + 1]);

			if (_type != ITEMGET(14, 11))	// Box Of Luck
			{
				if (lpItem->m_Level > 15)
				{
					lpItem->m_Level = 0;
				}
			}

			OptionData = lpMsg->dbItems[n*MAX_DBITEM_INFO + 1];
			lpItem->m_Option1 = DBI_GET_SKILL(OptionData);
			lpItem->m_Option2 = DBI_GET_LUCK(OptionData);
			lpItem->m_Option3 = DBI_GET_OPTION(OptionData);
			lpItem->m_Option3 |= DBI_GET_DINO_OPTION(lpMsg->dbItems[n*MAX_DBITEM_INFO + 7]);
			lpItem->m_Durability = lpMsg->dbItems[n*MAX_DBITEM_INFO + 2];
			lpItem->m_JewelOfHarmonyOption = lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_JOH_DATA];
			lpItem->m_ItemOptionEx = DBI_GET_380OPTION(lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_OPTION380_DATA]);

			if (lpItem->m_ItemOptionEx != 0)
			{
				lpItem->m_Type = itype;
				if (g_kItemSystemFor380.Is380Item(&item) == FALSE)
				{
					lpItem->m_ItemOptionEx = 0;

				}
			}

			BYTE SocketOption[5];
			BYTE SocketOptionIndex = 0xFF;
			int i = 0;
			std::memset(&SocketOption, 0xFF, 5);

			if (gSocketItemType.CheckSocketItemType(itype) == TRUE)
			{
				SocketOptionIndex = lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_JOH_DATA];

				for (i = 0; i < 5; i++)
				{
					SocketOption[i] = lpMsg->dbItems[((n*MAX_DBITEM_INFO) + DBI_SOCKET_1) + i];
				}
			}

			else if (g_PentagramSystem.IsPentagramItem(itype) == TRUE ||
				g_PentagramSystem.IsPentagramJewel(itype) == TRUE)
			{
				SocketOptionIndex = lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_JOH_DATA];

				for (i = 0; i < 5; i++)
				{
					SocketOption[i] = lpMsg->dbItems[((n*MAX_DBITEM_INFO) + DBI_SOCKET_1) + i];
				}
			}

			else
			{
				SocketOptionIndex = 0;

				for (i = 0; i < 5; i++)
				{
					SocketOption[i] = 0xFF;
				}
			}

			lpItem->m_PeriodItemOption = (lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_OPTION380_DATA] & 6) >> 1;

			lpItem->Convert(itype, lpItem->m_Option1, lpItem->m_Option2, lpItem->m_Option3, DBI_GET_NOPTION(lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_NOPTION_DATA]), lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SOPTION_DATA], lpItem->m_ItemOptionEx, SocketOption, SocketOptionIndex, lpItem->m_PeriodItemOption, CURRENT_DB_VERSION);

			if (_type == ITEMGET(14, 7)) // Siege Potion
			{

			}

			else if ((_type >= ITEMGET(14, 0) && _type <= ITEMGET(14, 8)) || (_type >= ITEMGET(14, 35) && _type <= ITEMGET(14, 40)))
			{
				if (lpItem->m_Durability == 0.0f)
				{
					lpItem->m_Durability = 1.0f;
				}
			}

			else if (_type != ITEMGET(13, 10) && _type != ITEMGET(14, 29) && _type != ITEMGET(14, 21) && lpItem->m_Level == 3)
			{
				if (lpItem->m_Durability > lpItem->m_BaseDurability)
				{
					lpItem->m_Durability = lpItem->m_BaseDurability;
				}
			}

			lpObj->pntEventInventory[n]->m_Option1 = lpItem->m_Option1;
			lpObj->pntEventInventory[n]->m_Option2 = lpItem->m_Option2;
			lpObj->pntEventInventory[n]->m_Option3 = lpItem->m_Option3;
			lpObj->pntEventInventory[n]->m_JewelOfHarmonyOption = lpItem->m_JewelOfHarmonyOption;
			lpObj->pntEventInventory[n]->m_ItemOptionEx = lpItem->m_ItemOptionEx;

			DWORD hidword = MAKE_NUMBERDW(MAKE_NUMBERW(lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL1], lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL2]), MAKE_NUMBERW(lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL3], lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL4]));
			DWORD lodword = MAKE_NUMBERDW(MAKE_NUMBERW(lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL5], lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL6]), MAKE_NUMBERW(lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL7], lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL8]));
			lpItem->m_Number = MAKEQWORD(hidword, lodword);

			gObjEventInventoryInsertItemPos(*lpObj, item, n, -1);
			gObjEventInventoryItemSet(*lpObj, *getGameObject(n), 1);
			count++;
		}
	}

	BYTE NewOption[MAX_EXOPTION_SIZE];

	for (int n = 0; n < EVENT_INVENTORY_SIZE; n++)
	{
		if (lpObj->pntEventInventory[n]->IsItem() == TRUE)
		{
			ItemIsBufExOption(NewOption, lpObj->pntEventInventory[n]);

		}
	}

	GSProtocol.GCEventInventoryItemListSend(Obj);

	if (g_ConfigRead.data.common.EventInventoryOpen == true)
	{
		GSProtocol.GCSendEventBanner(Obj, 1); //[K2]
	}

	GSProtocol.GCPlayerStatsPanelRates(lpObj->m_Index);
	GSProtocol.GCPlayerStatsPanelNew(lpObj->m_Index);

	for (int n = 0; n < INVENTORY_SIZE; n++)
	{
		if (lpObj->pntInventory[n]->IsPeriodItemExpire() == 1)
		{
			lpObj->pntInventory[n]->SetPeriodItemExpire();
			g_PeriodItemEx.RemovePeriodItemDataByForce(*lpObj, lpObj->pntInventory[n]->m_Type, lpObj->pntInventory[n]->m_Number);
			lpObj->pntInventory[n]->Clear();
		}
	}
}

void GDReqSwitchWare(CGameObject &Obj, int WareID)
{
	PMSG_REQ_SWITCHWARE pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x03, sizeof(pMsg));

	pMsg.iIndex = Obj;
	std::memcpy(pMsg.szAccountID, lpObj->AccountID, MAX_ACCOUNT_LEN + 1);
	pMsg.WarehouseID = WareID;

	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
}

void DGAnsSwitchWare(PMSG_ANS_SWITCHWARE *lpMsg)
{
	int iIndex = lpMsg->iIndex;
	CGameObject* lpObj = getGameObject(iIndex);
	if (!gObjIsAccontConnect(*lpObj, lpMsg->szAccountID))
	{
		return;
	}

	if (lpMsg->Result == 0)
	{
		MsgOutput(*lpObj, Lang.GetText(0, 452), lpMsg->WarehouseID);
	}

	else if (lpMsg->Result == 1)
	{
		MsgOutput(*lpObj, "Internal error");
	}

	else if (lpMsg->Result == 2)
	{
		MsgOutput(*lpObj, "You cannot switch warehouse now, please wait.");
	}

	else if (lpMsg->Result == 3)
	{
		MsgOutput(*lpObj, "You cannot switch warehouse now, warehouse is open or still not saved");
	}

	else if (lpMsg->Result == 4)
	{
		MsgOutput(*lpObj, "This is already used warehouse (%d)", lpMsg->WarehouseID);
	}
}

void DGAnsUBFAccountUserInfo(PMSG_ANS_UBF_ACCOUNT_USERINFO *lpMsg)
{
	CGameObject* lpObj = getGameObject(lpMsg->iUserIndex);

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	if (lpMsg->btResult == TRUE)
	{
		g_UnityBattleField.DGAnsCheckJoinedUnityBattleField(*lpObj, lpMsg->btRegisterState);
	}

	else
	{
		g_UnityBattleField.DGAnsCheckJoinedUnityBattleField(*lpObj, 0);
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE && lpMsg->btObserverMode == TRUE && (lpMsg->btRegisterState == 0 || lpMsg->btResult == 0))
	{
		g_UnityBattleField.DGAnsCheckJoinedUnityBattleField(*lpObj, lpMsg->btRegisterState);

		sLog->outBasic("[DGAnsUBFAccountUserInfo][%s][%s] LogOut, Because Not Resister State or Request To Cancel", lpObj->AccountID, lpObj->Name);
		gObjCloseSet(*lpObj, 0);
	}

	if (lpMsg->btObserverMode == FALSE)
	{
		PMSG_ANS_UBF_INFO pResult;
		PHeadSubSetB((BYTE*)&pResult, 0xDC, 0x01, sizeof(pResult));
		pResult.btResult = lpMsg->btRegisterState;
		IOCP.DataSend(*getGameObject(lpMsg->iUserIndex), (BYTE*)&pResult, pResult.h.size);
	}
}

void DGUBFRegisterAccountUserResult(PMSG_UBF_REGISTER_ACCOUNT_USER_RESULT *lpMsg)
{
	CGameObject* lpObj = getGameObject(*lpObj);
	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	if (lpMsg->btResult != TRUE)
	{
		sLog->outBasic("[UBFRegisterAccountUserResult][%s][%s] UBFRegisterAccountUserResult Error :%d (0:DB fail/3: Unexpired Day)",
			lpObj->AccountID, lpObj->Name, lpMsg->btResult);
	}

	g_UnityBattleField.DGAnsJoinUnityBattleField(lpMsg->iUserIndex, lpMsg->btResult, lpMsg->nLeftSec);
}

void DGUBFAccountUserCopyResult(PMSG_UBF_ACCOUNT_USER_COPY_RESULT *lpMsg)
{
	if (!ObjectMaxRange(lpMsg->iUserIndex))
	{
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->iUserIndex];

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	g_UnityBattleField.DGAnsCopyCharacterInfo(lpMsg->iUserIndex, lpMsg->btResult, lpMsg->btSubResult);
}

void DGAnsUBFGetReward(PMSG_ANS_UBF_GET_REWARD *lpMsg)
{
	if (!ObjectMaxRange(lpMsg->iUserIndex))
	{
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->iUserIndex];

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	PMSG_ANS_GET_REWARD pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xCD, 0x06, sizeof(pMsg));

	pMsg.btResult = lpMsg->btReturn;
	IOCP.DataSend(lpMsg->iUserIndex, (BYTE*)&pMsg, pMsg.h.size);

	if (lpMsg->btReturn == 0)
	{
		sLog->outBasic("[UBF][DGAnsUBFGetReward][%d][%s][%s] is not eligible for WinnerItem. return : %d",
			lpMsg->iUserIndex, lpObj->AccountID, lpObj->Name, lpMsg->btReturn);
		return;
	}

	sLog->outBasic("[UBF][DGAnsUBFGetReward][%d][%s][%s] Received Reward Of UnityBattleField - ChaosCastleFinal : %d",
		lpMsg->iUserIndex, lpObj->AccountID, lpObj->Name, lpMsg->btReturn);

	if (lpMsg->btBattleKind == 1) // CCF
	{
		if (lpMsg->btStage1 == TRUE)
		{
			for (int i = 0; i < lpMsg->btRewardCount1[0]; i++)
			{
				g_ChaosCastleFinal.GiveCCFWinnerItem(lpObj->m_Index, 1, 0);
			}

			for (int i = 0; i < lpMsg->btRewardCount1[1]; i++)
			{
				g_ChaosCastleFinal.GiveCCFWinnerItem(lpObj->m_Index, 1, 1);
			}
		}

		if (lpMsg->btStage2 == TRUE)
		{
			for (int i = 0; i < lpMsg->btRewardCount2[0]; i++)
			{
				g_ChaosCastleFinal.GiveCCFWinnerItem(lpObj->m_Index, 2, 0);
			}

			for (int i = 0; i < lpMsg->btRewardCount2[1]; i++)
			{
				g_ChaosCastleFinal.GiveCCFWinnerItem(lpObj->m_Index, 2, 1);
			}
		}

		if (lpMsg->btStage3 == TRUE)
		{
			for (int i = 0; i < lpMsg->btRewardCount3[0]; i++)
			{
				g_ChaosCastleFinal.GiveCCFWinnerItem(lpObj->m_Index, 3, 0);
			}

			for (int i = 0; i < lpMsg->btRewardCount3[1]; i++)
			{
				g_ChaosCastleFinal.GiveCCFWinnerItem(lpObj->m_Index, 3, 1);
			}
		}
	}

	else if (lpMsg->btBattleKind == 2) // Tormented Square
	{
		if (lpMsg->btStage1 == TRUE)
		{
			for (int i = 0; i < lpMsg->btRewardCount1[0]; i++)
			{
				g_DevilSquareFinal.GiveDSFWinnerItem(lpObj->m_Index, 1, 0);
			}

			for (int i = 0; i < lpMsg->btRewardCount1[1]; i++)
			{
				g_DevilSquareFinal.GiveDSFWinnerItem(lpObj->m_Index, 1, 1);
			}
		}

		if (lpMsg->btStage2 == TRUE)
		{
			for (int i = 0; i < lpMsg->btRewardCount2[0]; i++)
			{
				g_DevilSquareFinal.GiveDSFWinnerItem(lpObj->m_Index, 2, 0);
			}

			for (int i = 0; i < lpMsg->btRewardCount2[1]; i++)
			{
				g_DevilSquareFinal.GiveDSFWinnerItem(lpObj->m_Index, 2, 1);
			}
		}

		if (lpMsg->btStage3 == TRUE)
		{
			for (int i = 0; i < lpMsg->btRewardCount3[0]; i++)
			{
				g_DevilSquareFinal.GiveDSFWinnerItem(lpObj->m_Index, 3, 0);
			}

			for (int i = 0; i < lpMsg->btRewardCount3[1]; i++)
			{
				g_DevilSquareFinal.GiveDSFWinnerItem(lpObj->m_Index, 3, 1);
			}
		}
	}

	else if (lpMsg->btBattleKind == 3) // CC Battle
	{
		if (lpMsg->btStage1 == TRUE)
		{
			for (int i = 0; i < lpMsg->btRewardCount1[0]; i++)
			{
				g_ChaosCastle.GiveUBFReward(lpObj->m_Index, 0);
			}

			for (int i = 0; i < lpMsg->btRewardCount1[1]; i++)
			{
				g_ChaosCastle.GiveUBFReward(lpObj->m_Index, 1);
			}
		}
	}

	g_UnityBattleField.GDReqSetReceivedWinnerItem(lpMsg->iUserIndex, lpMsg->btBattleKind);
}

void DGAnsSetReceivedWinnerItem(PMSG_ANS_UBF_SET_RECEIVED_REWARD *lpMsg)
{
	if (!ObjectMaxRange(lpMsg->iUserIndex))
	{
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->iUserIndex];

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	g_UnityBattleField.DGAnsSetReceivedWinnerItem(lpMsg->iUserIndex, lpMsg->btReturn);
}

void DGUBFAnsCancelToJoinResult(PMSG_UBF_ANS_CANCEL_REGISTER_USER *lpMsg)
{
	g_UnityBattleField.DGAnsCancelUnityBattleField(lpMsg->iUserIndex, lpMsg->btCanceledResult, lpMsg->btDeletedResult);
}

void DGUBFAnsGetRealNameAndServerCode(PMSG_ANS_GET_UBF_REAL_NAME *lpMsg)
{
	if (!ObjectMaxRange(lpMsg->iUserIndex))
	{
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->iUserIndex];

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	g_UnityBattleField.DGAnsGetRealNameAndServerCode(lpMsg->iUserIndex, lpMsg->iServerCode);
}

void GDReqCopyPetItemInfo(CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj))
	{
		return;
	}

	LPOBJ lpObj = &gObj[Obj];

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	if (!gObjIsConnected(Obj))
	{
		return;
	}

	char Buffer[4096];
	int lOfs = sizeof(PMSG_UBF_ACCOUNT_USER_COPY_PETITEM);

	int Count = 0;
	PMSG_UBF_ACCOUNT_USER_COPY_PETITEM pMsg;
	PMSG_UBF_COPY_PETITEM pPetItem;

	for (int i = 0; i < INVENTORY_SIZE; i++)
	{
		if (lpObj->pntInventory[i]->IsItem() == TRUE &&
			(lpObj->pntInventory[i]->m_Type == ITEMGET(13, 4) || lpObj->pntInventory[i]->m_Type == ITEMGET(13, 5)))
		{
			pPetItem.btItemPos = i;
			pPetItem.i64ItemSerial = lpObj->pntInventory[i]->m_Number;

			std::memcpy(&Buffer[lOfs], &pPetItem, sizeof(pPetItem));
			lOfs += sizeof(pPetItem);
			Count++;
		}
	}

	if (Count > 0)
	{
		pMsg.h.set((BYTE*)&pMsg, 0xF4, lOfs);
		std::memcpy(pMsg.szAccountID, lpObj->AccountID, MAX_ACCOUNT_LEN + 1);

		pMsg.iUserIndex = Obj;
		pMsg.btCount = Count;
		pMsg.IsUnityBattleFieldServer = g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE ? TRUE : FALSE;
		pMsg.ServerCode = g_ConfigRead.server.GetGameServerCode() / 20;
		std::memcpy(&Buffer, &pMsg, sizeof(pMsg));

		wsDataCli.DataSend(Buffer, lOfs);
	}

	sLog->outBasic("[UBF][GDReqCopyPetItemInfo][%s][%s] Move(Copy) the PetInfo Into UnityBattleField",
		lpObj->AccountID, lpObj->Name);
}

void DGAns_ChaosCastle_KillPoint_Result(SDHP_ANS_KILLPOINT_RESULT_CC_UBF *lpMsg)
{
	CGameObject* lpObj = getGameObject(lpMsg->nIndex);

	if (lpObj->Type != OBJ_USER)
	{
		return;
	}

	if (!gObjIsConnected(lpObj))
	{
		return;
	}

	sLog->outBasic("[CChaosCastle][DGAns_ChaosCastle_KillPoint_Result][%s][%s][%s] Ans Result-Return:%d / KillPoint:%d/ TotalPoint:%d",
		lpObj->AccountID, lpObj->Name, lpObj->m_PlayerData->m_RealNameOfUBF, lpMsg->nResult, lpMsg->nCurrentPoint, lpMsg->nTotalPoint);
}

void GDReqDSFCanPartyEnter(CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj))
	{
		return;
	}

	if (!gObjIsConnected(Obj))
	{
		return;
	}

	sLog->outBasic("[DSF][GDReqCanPartyEnter][%s][%s][%s][%d] DSF Enter Request",
		lpObj->AccountID, lpObj->m_PlayerData->m_RealNameOfUBF,
		lpObj->Name, lpObj->m_PlayerData->m_nServerCodeOfHomeWorld);

	PMSG_ANS_DSF_ENTER pResult;
	PHeadSubSetB((BYTE*)&pResult, 0xDB, 0x01, sizeof(pResult));

	std::memset(pResult.Name, 0x00, sizeof(pResult.Name));

	if (lpObj->PartyNumber < 0)
	{
		pResult.btResult = 1;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (gParty.Isleader(lpObj->PartyNumber, Obj, lpObj->DBNumber) == FALSE)
	{
		pResult.btResult = 3;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (gParty.GetCount(lpObj->PartyNumber) != 2)
	{
		pResult.btResult = 4;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	LPOBJ lpObj = &gObj[Obj];
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	int dbnumber = 0, useridx = -1;

	PMSG_REQ_DSF_CAN_PARTY_ENTER pMsg;

	for (int i = 0; i < 2; i++)
	{
		if (gParty.GetIndexUser(lpObj->PartyNumber, i, useridx, dbnumber) == FALSE)
		{
			pResult.btResult = 13;

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);
			return;
		}

		if (useridx > 0)
		{
			if (i == 0)
			{
				std::memcpy(pMsg.szAccountID1, gObj[useridx].AccountID, MAX_ACCOUNT_LEN + 1);
				std::memcpy(pMsg.szUserName1, gObj[useridx].Name, MAX_ACCOUNT_LEN + 1);
			}

			else if (i == 1)
			{
				std::memcpy(pMsg.szAccountID2, gObj[useridx].AccountID, MAX_ACCOUNT_LEN + 1);
				std::memcpy(pMsg.szUserName2, gObj[useridx].Name, MAX_ACCOUNT_LEN + 1);
			}
		}
	}

	pMsg.iUserIndex = Obj;
	pMsg.btDSFType = g_DevilSquareFinal.WhatIsTodayTypeOfDSF();
	pMsg.iEnterYear = sysTime.wYear;
	pMsg.btEnterMonth = sysTime.wMonth;
	pMsg.btEnterDay = sysTime.wDay;

	PHeadSubSetB((BYTE*)&pMsg, 0xFD, 0x00, sizeof(pMsg));
	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
}

void DG_DSF_CanPartyEnter(PMSG_ANS_DSF_CAN_PARTY_ENTER *lpMsg)
{
	CGameObject* lpObj = getGameObject(lpMsg->iUserIndex);

	if (!gObjIsConnected(*lpObj))
	{
		return;
	}

	PMSG_ANS_DSF_ENTER pResult;
	PHeadSubSetB((BYTE*)&pResult, 0xDB, 0x01, sizeof(pResult));

	std::memset(pResult.Name, 0x00, sizeof(pResult.Name));

	if (lpMsg->btResult == 3)
	{
		if (g_DevilSquareFinal.WhatIsTodayTypeOfDSF() == 1)
		{
			pResult.btResult = 22;
		}

		else if (g_DevilSquareFinal.WhatIsTodayTypeOfDSF() == 2)
		{
			pResult.btResult = 23;
		}

		IOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		sLog->outBasic("[DSF][DG_DSF_CanPartyEnter] [%s][%s][%s][%d] Can't Enter : Already Pass Party",
			lpObj->AccountID, lpObj->m_PlayerData->m_RealNameOfUBF,
			lpObj->Name, lpObj->m_PlayerData->m_nServerCodeOfHomeWorld);
	}

	else if (lpMsg->btResult == 0 || lpMsg->btResult == 4)
	{
		GSProtocol.CGReqDSFEnter(*lpObj);

		sLog->outBasic("[DSF][DG_DSF_CanPartyEnter] [%s][%s][%s][%d] Can Enter : EnterCnt:%d",
			lpObj->AccountID, lpObj->m_PlayerData->m_RealNameOfUBF,
			lpObj->Name, lpObj->m_PlayerData->m_nServerCodeOfHomeWorld,
			lpMsg->btCount);
	}

	else if (lpMsg->btResult == 1)
	{
		if (g_DevilSquareFinal.WhatIsTodayTypeOfDSF() == 2)
		{
			pResult.btResult = 18;
		}

		else if (g_DevilSquareFinal.WhatIsTodayTypeOfDSF() == 3)
		{
			pResult.btResult = 19;
		}

		IOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		sLog->outBasic("[DSF][DG_DSF_CanPartyEnter] [%s][%s][%s][%d] Can't Enter : Not Tournament User",
			lpObj->AccountID, lpObj->m_PlayerData->m_RealNameOfUBF,
			lpObj->Name, lpObj->m_PlayerData->m_nServerCodeOfHomeWorld);
	}

	else if (lpMsg->btResult == 2 && g_DevilSquareFinal.WhatIsTodayTypeOfDSF() == 1)
	{
		pResult.btResult = 22;
		IOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		sLog->outBasic("[DSF][DG_DSF_CanPartyEnter] [%s][%s][%s][%d] Can't Enter : Already Exist Pass User",
			lpObj->AccountID, lpObj->m_PlayerData->m_RealNameOfUBF,
			lpObj->Name, lpObj->m_PlayerData->m_nServerCodeOfHomeWorld);
	}

	else if (lpMsg->btResult == 5)
	{
		pResult.btResult = 21;
		IOCP.DataSend(lpObj->m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		sLog->outBasic("[DSF][DG_DSF_CanPartyEnter] [%s][%s][%s][%d] Can't Enter : Enter With Different User",
			lpObj->AccountID, lpObj->m_PlayerData->m_RealNameOfUBF,
			lpObj->Name, lpObj->m_PlayerData->m_nServerCodeOfHomeWorld);
	}
}

void DG_DSF_PartyRankRenew(PMSG_ANS_DSF_PARTYRANKRENEW *lpMsg)
{
	if (!lpMsg)
	{
		return;
	}

	if (g_ConfigRead.server.GetServerType() != SERVER_BATTLECORE)
	{
		return;
	}

	if (lpMsg->btResult == 1)
	{
		g_DevilSquareFinal.GDReqDSFGoFinalParty(2);
		g_DevilSquareFinal.GDReqDSFGoFinalParty(3);
		g_DevilSquareFinal.GDReqDSFGoFinalParty(4);
	}
}

void DG_DSF_GoFinalParty(BYTE* lpRecv)
{
	PMSG_ANS_DSF_GO_FINAL_PARTY * lpMsg = (PMSG_ANS_DSF_GO_FINAL_PARTY *)(lpRecv);

	if (!lpMsg)
	{
		return;
	}

	if (g_ConfigRead.server.GetServerType() != SERVER_BATTLECORE)
	{
		return;
	}

	int nPartyCnt = lpMsg->btPartyCnt;

	switch (lpMsg->btDSFType)
	{
	case 2:
		if (nPartyCnt > 40)
		{
			nPartyCnt = 40;
		}

		for (int i = 0; i < 40; i++)
		{
			g_DevilSquareFinal.m_GoSemiFinalParty[i].Clear();
		}

		for (int j = 0; j < nPartyCnt; j++)
		{
			DSF_GO_FINAL_PARTY * lpData = (DSF_GO_FINAL_PARTY *)(lpRecv + sizeof(PMSG_ANS_DSF_GO_FINAL_PARTY) + (j * sizeof(DSF_GO_FINAL_PARTY)));

			if (lpData->btRank <= 0)
			{
				std::memset(g_DevilSquareFinal.m_GoSemiFinalParty[j].szUserName1, 0x00, MAX_ACCOUNT_LEN + 1);
				std::memset(g_DevilSquareFinal.m_GoSemiFinalParty[j].szUserName2, 0x00, MAX_ACCOUNT_LEN + 1);
				g_DevilSquareFinal.m_GoSemiFinalParty[j].wServerCode1 = -1;
				g_DevilSquareFinal.m_GoSemiFinalParty[j].wServerCode2 = -1;
				g_DevilSquareFinal.m_GoSemiFinalParty[j].btRank = 0;
				g_DevilSquareFinal.m_GoSemiFinalParty[j].btDSFType = 0;
				g_DevilSquareFinal.m_GoSemiFinalParty[j].iPoint = 0;
			}

			else
			{
				std::memcpy(g_DevilSquareFinal.m_GoSemiFinalParty[j].szUserName1, lpData->szUserName1, MAX_ACCOUNT_LEN + 1);
				std::memcpy(g_DevilSquareFinal.m_GoSemiFinalParty[j].szUserName2, lpData->szUserName2, MAX_ACCOUNT_LEN + 1);
				g_DevilSquareFinal.m_GoSemiFinalParty[j].wServerCode1 = lpData->wServerCode1;
				g_DevilSquareFinal.m_GoSemiFinalParty[j].wServerCode2 = lpData->wServerCode2;
				g_DevilSquareFinal.m_GoSemiFinalParty[j].btRank = lpData->btRank;
				g_DevilSquareFinal.m_GoSemiFinalParty[j].btDSFType = lpData->btDSFType;
				g_DevilSquareFinal.m_GoSemiFinalParty[j].iPoint = lpData->iPoint;
			}
		}
		break;
	case 3:
		if (nPartyCnt > 10)
		{
			nPartyCnt = 10;
		}

		for (int i = 0; i < 10; i++)
		{
			g_DevilSquareFinal.m_GoFinalParty[i].Clear();
		}

		for (int j = 0; j < nPartyCnt; j++)
		{
			DSF_GO_FINAL_PARTY * lpData = (DSF_GO_FINAL_PARTY *)(lpRecv + sizeof(PMSG_ANS_DSF_GO_FINAL_PARTY) + (j * sizeof(DSF_GO_FINAL_PARTY)));

			if (lpData->btRank <= 0)
			{
				std::memset(g_DevilSquareFinal.m_GoFinalParty[j].szUserName1, 0x00, MAX_ACCOUNT_LEN + 1);
				std::memset(g_DevilSquareFinal.m_GoFinalParty[j].szUserName2, 0x00, MAX_ACCOUNT_LEN + 1);
				g_DevilSquareFinal.m_GoFinalParty[j].wServerCode1 = -1;
				g_DevilSquareFinal.m_GoFinalParty[j].wServerCode2 = -1;
				g_DevilSquareFinal.m_GoFinalParty[j].btRank = 0;
				g_DevilSquareFinal.m_GoFinalParty[j].btDSFType = 0;
				g_DevilSquareFinal.m_GoFinalParty[j].iPoint = 0;
			}

			else
			{
				std::memcpy(g_DevilSquareFinal.m_GoFinalParty[j].szUserName1, lpData->szUserName1, MAX_ACCOUNT_LEN + 1);
				std::memcpy(g_DevilSquareFinal.m_GoFinalParty[j].szUserName2, lpData->szUserName2, MAX_ACCOUNT_LEN + 1);
				g_DevilSquareFinal.m_GoFinalParty[j].wServerCode1 = lpData->wServerCode1;
				g_DevilSquareFinal.m_GoFinalParty[j].wServerCode2 = lpData->wServerCode2;
				g_DevilSquareFinal.m_GoFinalParty[j].btRank = lpData->btRank;
				g_DevilSquareFinal.m_GoFinalParty[j].btDSFType = lpData->btDSFType;
				g_DevilSquareFinal.m_GoFinalParty[j].iPoint = lpData->iPoint;
			}
		}
		break;
	case 4:

		if (nPartyCnt <= 0)
		{
			break;
		}

		g_DevilSquareFinal.m_WinnerParty.Clear();

		DSF_GO_FINAL_PARTY * lpData = (DSF_GO_FINAL_PARTY *)(lpRecv + sizeof(PMSG_ANS_DSF_GO_FINAL_PARTY));

		if (lpData->btRank <= 0)
		{
			std::memset(g_DevilSquareFinal.m_WinnerParty.szUserName1, 0x00, MAX_ACCOUNT_LEN + 1);
			std::memset(g_DevilSquareFinal.m_WinnerParty.szUserName2, 0x00, MAX_ACCOUNT_LEN + 1);
			g_DevilSquareFinal.m_WinnerParty.wServerCode1 = -1;
			g_DevilSquareFinal.m_WinnerParty.wServerCode2 = -1;
			g_DevilSquareFinal.m_WinnerParty.btRank = 0;
			g_DevilSquareFinal.m_WinnerParty.btDSFType = 0;
			g_DevilSquareFinal.m_WinnerParty.iPoint = 0;
		}

		else
		{
			std::memcpy(g_DevilSquareFinal.m_WinnerParty.szUserName1, lpData->szUserName1, MAX_ACCOUNT_LEN + 1);
			std::memcpy(g_DevilSquareFinal.m_WinnerParty.szUserName2, lpData->szUserName2, MAX_ACCOUNT_LEN + 1);
			g_DevilSquareFinal.m_WinnerParty.wServerCode1 = lpData->wServerCode1;
			g_DevilSquareFinal.m_WinnerParty.wServerCode2 = lpData->wServerCode2;
			g_DevilSquareFinal.m_WinnerParty.btRank = lpData->btRank;
			g_DevilSquareFinal.m_WinnerParty.btDSFType = lpData->btDSFType;
			g_DevilSquareFinal.m_WinnerParty.iPoint = lpData->iPoint;
		}
		break;

	}
}

void DG_DSF_GetReward(PMSG_ANS_GET_DSF_REWARD *lpMsg)
{
	if (!lpMsg)
	{
		return;
	}

	PMSG_ANS_DSF_GET_REWARD pResult;
	PHeadSubSetB((BYTE*)&pResult, 0xDB, 0x09, sizeof(pResult));

	pResult.btResult = lpMsg->btResult;
	IOCP.DataSend(lpMsg->nUserIndex, (BYTE*)&pResult, pResult.h.size);

	if (lpMsg->btResult == 1)
	{
		sLog->outBasic("[DSF][DG_DSF_GetReward] [%s][%s][%s][%d] Can't Get Reward : Not League Winner!",
			gObj[lpMsg->nUserIndex].AccountID, gObj[lpMsg->nUserIndex].m_PlayerData->m_RealNameOfUBF,
			gObj[lpMsg->nUserIndex].Name, gObj[lpMsg->nUserIndex].m_PlayerData->m_nServerCodeOfHomeWorld);
	}

	else if (lpMsg->btResult == 2)
	{
		sLog->outBasic("[DSF][DG_DSF_GetReward] [%s][%s][%s][%d] Can't Get Reward : Get Reward Already!",
			gObj[lpMsg->nUserIndex].AccountID, gObj[lpMsg->nUserIndex].m_PlayerData->m_RealNameOfUBF,
			gObj[lpMsg->nUserIndex].Name, gObj[lpMsg->nUserIndex].m_PlayerData->m_nServerCodeOfHomeWorld);
	}

	else
	{
		sLog->outBasic("[DSF][DG_DSF_GetReward] [%s][%s][%s][%d] Can Get Reward Success!!! ",
			gObj[lpMsg->nUserIndex].AccountID, gObj[lpMsg->nUserIndex].m_PlayerData->m_RealNameOfUBF,
			gObj[lpMsg->nUserIndex].Name, gObj[lpMsg->nUserIndex].m_PlayerData->m_nServerCodeOfHomeWorld);
	}
}

void DGReqOtherChannelWishper(PMSG_RECV_CHATDATA_WHISPER *lpMsg)
{
	int userIndex = gObjGetIndex(lpMsg->id);

	if (userIndex == -1)
	{
		GDWhisperResultSend(0, lpMsg->OriginPlayerIndex, lpMsg->OriginGSIndex);
		return;
	}
	else
	{
		GDWhisperResultSend(1, lpMsg->OriginPlayerIndex, lpMsg->OriginGSIndex);
	}
	char tid[11];
	PMSG_CHATDATA_WHISPER pWhisper;
	int len;
	int index;


	if (g_ConfigRead.data.common.SavePrivateChat == true)
	{
		sLog->outBasic(Lang.GetText(0, 517), lpMsg->fromId, lpMsg->id, lpMsg->chatmsg);
	}

	tid[10] = 0;
	std::memcpy(tid, lpMsg->id, sizeof(lpMsg->id));
	index = WhisperCash.AddCash(tid);

	PHeadSetB((BYTE*)&pWhisper, 0x02, sizeof(pWhisper));
	std::strcpy(pWhisper.id, lpMsg->fromId);
	std::memcpy(pWhisper.chatmsg, lpMsg->chatmsg, sizeof(lpMsg->chatmsg));

	//pWhisper.szChatMsg[MAX_CHAT_LEN-2] = 0;
	//pWhisper.chatmsg[MAX_CHAT_LEN-1] = 0;
	len = strlen(pWhisper.chatmsg);

	if (len > 0)
	{
		pWhisper.h.size -= sizeof(pWhisper.chatmsg);
		pWhisper.h.size += len + 1;
		IOCP.DataSend(index, (UCHAR*)&pWhisper, pWhisper.h.size);
	}

}

void GDReqMapSrvGroupServerCount()
{
	DSMSG_REQ_SUBSERVER_COUNT pMsg;
	pMsg.h.set((BYTE*)&pMsg, 0xC3, 0x00, sizeof(pMsg));
	pMsg.wMapSvrGroup = g_MapServerManager.GetMapSvrGroup();
	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

void DGWhisperResponseRecv(DSMSG_GS_WHISPER_RESULT * aRecv)
{
	LPOBJ lpObj = &gObj[aRecv->OriginPlayerIndex];

	if (!lpObj)
	{
		sLog->outBasic("[%d] Is no longer online LINE %d", aRecv->OriginPlayerIndex, __LINE__);
		return;
	}

	if (lpObj->Connected != PLAYER_PLAYING)
	{
		sLog->outBasic("[%d] Is no longer online LINE %d", aRecv->OriginPlayerIndex, __LINE__);
		return;
	}

	if (lpObj->m_Whispering.iWhisperSent == true)
	{
		if (aRecv->btResult == 1)
			lpObj->m_Whispering.iWhisperSent = false;

		lpObj->m_Whispering.wResponseCount++;
		if (lpObj->m_Whispering.wResponseCount >= gMapSrvGroupOnlineServer && lpObj->m_Whispering.iWhisperSent == true) // if all gs return false , player is not found
		{
			GSProtocol.GCServerMsgSend(0, lpObj->m_Index);
		}
	}
}

void GDWhisperResultSend(BYTE Result, int OriginPlayerIndex, int OriginGS)
{
	DSMSG_ANS_WHISPER_RESULT pMsg;
	pMsg.h.set((BYTE*)&pMsg, 0xC3, 0x02, sizeof(pMsg));

	pMsg.btResult = Result;
	pMsg.OriginGSIndex = OriginGS;
	pMsg.OriginPlayerIndex = OriginPlayerIndex;

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}
void GDReqDisconnectOtherChannel(char * charname)
{
	PMSG_RECV_DC_OTHER_CHANNEL pMsg;
	pMsg.h.set((BYTE*)&pMsg, 0xC3, 0x04, sizeof(pMsg));

	pMsg.wMapSrvGroup = g_MapServerManager.GetMapSvrGroup();
	std::memcpy(pMsg.szName, charname, MAX_ACCOUNT_LEN + 1);

	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);

}
void DGDisconnectOtherChannel(PMSG_RECV_DC_OTHER_CHANNEL * aRecv)
{
	int iTargetIndex = gObjGetIndex(aRecv->szName);

	if (iTargetIndex >= 0)
	{
		CGameObject* lpObj = getGameObject(iTargetIndex);

		if (lpObj->m_bOff != false)
		{
			GJSetCharacterInfo(*lpObj, FALSE);
			lpObj->m_bOff = false;
			gObjDel(*lpObj);

		}

		GSProtocol.GCSendDisableReconnect(*lpObj);
		sLog->outBasic("Multi-Cast Disconnect: %s", aRecv->szName);
	}
}

struct MUBOT_SETTINGS_REQ_SAVE
{
	PWMSG_HEAD h;
	char szName[11];
	BYTE btDATA[512];
};

void DGMuBotSettingsSend(CGameObject &Obj, char* szName, BYTE* lpSettings)
{
	MUBOT_SETTINGS_REQ_SAVE pMsg = { 0 };
	PHeadSetW((BYTE*)&pMsg, 0x61, sizeof(pMsg));

	std::memcpy(pMsg.szName, szName, 10);
	std::memcpy(pMsg.btDATA, lpSettings, sizeof(pMsg.btDATA));

	wsDataCli.DataSend((char*)&pMsg, sizeof(pMsg));
}

void DGMuBotOptionRecv(LPMUBOT_SETTINGS_SEND lpMsg)
{
	CGameObject* lpObj = getGameObject(lpMsg->userIndex);
	char szName[MAX_ACCOUNT_LEN + 1];

	std::memset(szName, 0, sizeof(szName));
	std::memcpy(szName, lpMsg->szName, MAX_ACCOUNT_LEN);

	if (strcmp(lpObj->Name, szName) != 0)
	{
		return;
	}

	GSProtocol.GCSendMuBotSettings(*lpObj, lpMsg->btDATA);
}

