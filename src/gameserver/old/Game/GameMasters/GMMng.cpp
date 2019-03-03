////////////////////////////////////////////////////////////////////////////////
// GMMng.cpp
#include "GMMng.h"
#include "Main.h"
#include "GameProtocol.h"
#include "util.h"
#include "MoveCommand.h"
#include "GuildClass.h"
#include "TNotice.h"
#include "ObjCalCharacter.h"
#include "BattleSoccerManager.h"
#include "IOCP.h"
#include "Kanturu.h"
#include "SkillAdditionInfo.h"
#include "RaklionBattleUserMng.h"
#include "BuffEffectSlot.h"
#include "AntiSwear.h"
#include "Marry.h"
#include "MapServerManager.h"
#include "Crywolf.h"
#include "BloodCastle.h"
#include "BagManager.h"
#include "DevilSquare.h"
#include "ChaosCastle.h"
#include "configread.h"
#include "PentagramSystem.h"
#include "GensSystem.h"
#include "ImperialGuardian.h"
#include "MasterLevelSkillTreeSystem.h"
#include "ItemSocketOptionSystem.h"
#include "SendNPCInfo.h"
#include "VipSys.h"
#include "SProtocol.h"
#include "MuunSystem.h"
#include "ItemOptionTypeMng.h"
#include "OfflineLevelling.h"
#include "ChaosCastleFinal.h"
#include "QuestExpProgMng.h"
#include "ChangeCmd.h"
#include "UpgradeCmd.h"
#include "gObjMonster.h"
#include "ResetTable.h"
#include "Bag.h"
#include "CustomMaxStats.h"
#include "StatSpecialize.h"
#include "SocketItemType.h"

CGMMng cManager;

CGMMng::CGMMng()
{
	this->ManagerInit();
}

CGMMng::~CGMMng()
{
	delete CommandLog;
	return;
}

void CGMMng::LoadCommandFile(LPSTR file)
{
	GetPrivateProfileString("commands", "PostCMD", "/post", m_CommandNames.PostCMD, 20, file);
	GetPrivateProfileString("commands", "ItemCMD", "/item", m_CommandNames.ItemCMD, 20, file);
	GetPrivateProfileString("commands", "AddSTRCMD", "/addstr", m_CommandNames.AddSTRCMD, 20, file);
	GetPrivateProfileString("commands", "AddAGICMD", "/addagi", m_CommandNames.AddAGICMD, 20, file);
	GetPrivateProfileString("commands", "AddVITCMD", "/addvit", m_CommandNames.AddVITCMD, 20, file);
	GetPrivateProfileString("commands", "AddENECMD", "/addene", m_CommandNames.ADDENECMD, 20, file);
	GetPrivateProfileString("commands", "AddCMDCMD", "/addcmd", m_CommandNames.ADDCMDCMD, 20, file);
	GetPrivateProfileString("commands", "OnlineCMD", "/online", m_CommandNames.ONLINECMD, 20, file);
	GetPrivateProfileString("commands", "PKClearCMD", "/pkclear", m_CommandNames.PKCLEARCMD, 20, file);
	GetPrivateProfileString("commands", "WareCMD", "/ware", m_CommandNames.WARECMD, 20, file);
	GetPrivateProfileString("commands", "MarryProposeCMD", "/prop", m_CommandNames.PROPOSECMD, 20, file);
	GetPrivateProfileString("commands", "MarryAcceptCMD", "/accept", m_CommandNames.ACCEPTCMD, 20, file);
	GetPrivateProfileString("commands", "MarryTeleportCMD", "/huwi", m_CommandNames.TRACEMARRYCMD, 20, file);
	GetPrivateProfileString("commands", "SkinCMD", "/skin", m_CommandNames.SKINCMD, 20, file);
	GetPrivateProfileString("commands", "ForceBCStart", "/bcstart", m_CommandNames.BCStart, 20, file);
	GetPrivateProfileString("commands", "ForceDSStart", "/DSstart", m_CommandNames.DSStart, 20, file);
	GetPrivateProfileString("commands", "ForceCCStart", "/CCstart", m_CommandNames.CCStart, 20, file);
	GetPrivateProfileString("commands", "ForceITStart", "/ITstart", m_CommandNames.ITStart, 20, file);
	GetPrivateProfileString("commands", "WARCMD", "/war", m_CommandNames.WARCMD, 20, file);
	GetPrivateProfileString("commands", "FireWorks", "/fw", m_CommandNames.FireWorks, 20, file);
	GetPrivateProfileString("commands", "WatchCMD", "/watch", m_CommandNames.Watch, 20, file);
	GetPrivateProfileString("commands", "TraceCMD", "/trace", m_CommandNames.Trace, 20, file);
	GetPrivateProfileString("commands", "DisconnectCMD", "/dc", m_CommandNames.DC, 20, file);
	GetPrivateProfileString("commands", "GuildDCCMD", "/gdc", m_CommandNames.GuildDC, 20, file);
	GetPrivateProfileString("commands", "MoveCMD", "/move", m_CommandNames.MoveCMD, 20, file);
	GetPrivateProfileString("commands", "MoveGuild", "/moveguild", m_CommandNames.gMoveCMD, 20, file);
	GetPrivateProfileString("commands", "StartSoccer", "/startsoccer", m_CommandNames.StartSoccer, 20, file);
	GetPrivateProfileString("commands", "StopSoccer", "/stopsoccer", m_CommandNames.EndSoccer, 20, file);
	GetPrivateProfileString("commands", "EndWar", "/endwar", m_CommandNames.EndWar, 20, file);
	GetPrivateProfileString("commands", "ChatBan", "/cb", m_CommandNames.ChatBan, 20, file);
	GetPrivateProfileString("commands", "UnbanChat", "/ucb", m_CommandNames.ChatUnban, 20, file);
	GetPrivateProfileString("commands", "SetGW", "/setgw", m_CommandNames.SetGW, 20, file);
	GetPrivateProfileString("commands", "SoccerStart", "/battlesoccer", m_CommandNames.SoccerStart, 20, file);
	GetPrivateProfileString("commands", "SetRequest", "/requests", m_CommandNames.Requests, 20, file);
	GetPrivateProfileString("commands", "SendKundunInfo", "/kundunhp", m_CommandNames.KundunInfo, 20, file);
	GetPrivateProfileString("commands", "KundunSetLife", "/kundunsetlife", m_CommandNames.SetKundunHP, 20, file);
	GetPrivateProfileString("commands", "ResetCMD", "/reset", m_CommandNames.ResetCMD, 20, file);
	GetPrivateProfileString("commands", "SvInfoCMD", "/svinfo", m_CommandNames.SvInfoCMD, 20, file);
	GetPrivateProfileString("commands", "CharInfoCMD", "/charinfo", m_CommandNames.CharInfoCMD, 20, file);
	GetPrivateProfileString("commands", "HideCMD", "/hide", m_CommandNames.HideCMD, 20, file);
	GetPrivateProfileString("commands", "UnHideCMD", "/unhide", m_CommandNames.UnHideCMD, 20, file);
	GetPrivateProfileString("commands", "ClearInvCMD", "/clearinv", m_CommandNames.ClearInvCMD, 20, file);
	GetPrivateProfileString("commands", "MarryDivorceCMD", "/divorce", m_CommandNames.DIVORCECMD, 20, file);

	GetPrivateProfileString("commands", "CrywolfControlCMD", "/crywolfset", m_CommandNames.CWSetStateCMD, 20, file);
	GetPrivateProfileString("commands", "CastleSiegeSetOwnerCMD", "/cssetowner", m_CommandNames.CSSetOwnerCMD, 20, file);
	GetPrivateProfileString("commands", "CastleSiegeSetRegisterCMD", "/cssetregister", m_CommandNames.CSSetRegCMD, 20, file);
	GetPrivateProfileString("commands", "CastleSiegeSetRegMarkCMD", "/cssetregmark", m_CommandNames.CSSetMarkCMD, 20, file);
	GetPrivateProfileString("commands", "CastleSiegeSetNotifyCMD", "/cssetnotify", m_CommandNames.CSSetNotifyCMD, 20, file);
	GetPrivateProfileString("commands", "CastleSiegeSetReadyCMD", "/cssetready", m_CommandNames.CSSetReadyCMD, 20, file);
	GetPrivateProfileString("commands", "CastleSiegeSetEndCMD", "/cssetend", m_CommandNames.CSSetEndCMD, 20, file);
	GetPrivateProfileString("commands", "CastleSiegeGetOwnerCMD", "/csgetowner", m_CommandNames.CSGetOwnerCMD, 20, file);
	GetPrivateProfileString("commands", "CastleSiegeGetStateCMD", "/csgetstate", m_CommandNames.CSGetStateCMD, 20, file);
	GetPrivateProfileString("commands", "CastleSiegeSetStartCMD", "/cssetstart", m_CommandNames.CSSetStartCMD, 20, file);
	GetPrivateProfileString("commands", "CastleSiegeChangeSideCMD", "/cschangeside", m_CommandNames.CSSetChangeSideCMD, 20, file);

	GetPrivateProfileString("commands", "KanturuSetStandbyCMD", "/ktstandby", m_CommandNames.KTSetStandbyCMD, 20, file);
	GetPrivateProfileString("commands", "KanturuSetMayaCMD", "/ktmaya", m_CommandNames.KTSetMayaCMD, 20, file);
	GetPrivateProfileString("commands", "KanturuSetTowerCMD", "/kttower", m_CommandNames.KTSetTowerCMD, 20, file);

	GetPrivateProfileString("commands", "BanCharCMD", "/banchar", m_CommandNames.BanCharCMD, 20, file);
	GetPrivateProfileString("commands", "UnBanCharCMD", "/unbanchar", m_CommandNames.UnBanCharCMD, 20, file);
	GetPrivateProfileString("commands", "BanAccCMD", "/banacc", m_CommandNames.BanAccCMD, 20, file);
	GetPrivateProfileString("commands", "UnBanAccCMD", "/unbanacc", m_CommandNames.UnBanAccCMD, 20, file);

	GetPrivateProfileString("commands", "CrewCMD", "/crew", m_CommandNames.CrewCMD, 20, file);
	GetPrivateProfileString("commands", "GlobalPostCMD", "/gpost", m_CommandNames.GPostCMD, 20, file);
	GetPrivateProfileString("commands", "SetPKCMD", "/setpk", m_CommandNames.PKSETCMD, 20, file);
	GetPrivateProfileString("commands", "OffTradeCMD", "/off", m_CommandNames.offTrade, 20, file);
	GetPrivateProfileString("commands", "DCFriendCMD", "/dcfriend", m_CommandNames.dcFriend, 20, file);
	GetPrivateProfileString("commands", "SetPartyCMD", "/setparty", m_CommandNames.setparty, 20, file);
	GetPrivateProfileString("commands", "OffLevelCMD", "/offlevel", m_CommandNames.offlevel, 20, file);
	GetPrivateProfileString("commands", "GremoryGiftCMD", "/addgift", m_CommandNames.GremoryGiftCMD, 20, file);

	GetPrivateProfileString("joinmu", "ChangeItemCMD", "/changeitem", m_CommandNames.JoinMuChangeCMD, 20, file);
	GetPrivateProfileString("joinmu", "UpgradeItemCMD", "/upgradeitem", m_CommandNames.JoinMuUpgradeCMD, 20, file);

}

void CGMMng::NewLoadCommandFile(LPSTR file)
{
	GetPrivateProfileString("commands", "PartyLeader", "/PartyLeader", m_CommandNames.partyleader, 20, file);
	GetPrivateProfileString("commands", "ResetTable", "/TableReset", m_CommandNames.resettabulado, 20, file);
	GetPrivateProfileString("commands", "NewMakeItem", "/MakeItems", m_CommandNames.MakeItems, 20, file);
	GetPrivateProfileString("commands", "ClearInventory", "/clearinv", m_CommandNames.ClearInventory, 20, file);
	GetPrivateProfileString("commands", "MakeRandomSet", "/makerandomset", m_CommandNames.MakeRandomSet, 20, file);

}
void CGMMng::Init()
{
	if (CommandLog == NULL)
	{
		CommandLog = new CLogToFile("COMMANDS_LOG", g_ConfigRead.server.GetCommandLogPath(), 256);
	}

	this->cCommand.Init();
	this->LoadCommandFile(g_ConfigRead.GetPath("IGC_Commands.ini"));
	this->NewLoadCommandFile(g_ConfigRead.GetPath("Custom\\IGC_NewCommands.ini"));

	this->cCommand.Add(m_CommandNames.FireWorks, 217);
	this->cCommand.Add(m_CommandNames.Watch, 216);
	this->cCommand.Add(m_CommandNames.Trace, 215);
	this->cCommand.Add(m_CommandNames.DC, 100);
	this->cCommand.Add(m_CommandNames.GuildDC, 112);
	this->cCommand.Add(m_CommandNames.MoveCMD, 101);
	this->cCommand.Add(m_CommandNames.gMoveCMD, 108);
	this->cCommand.Add(m_CommandNames.StartSoccer, 109);
	this->cCommand.Add(m_CommandNames.EndSoccer, 110);
	this->cCommand.Add(m_CommandNames.EndWar, 111);
	this->cCommand.Add(m_CommandNames.ChatBan, 104);
	this->cCommand.Add(m_CommandNames.ChatUnban, 106);
	this->cCommand.Add("/remaintime", 201);
	this->cCommand.Add(m_CommandNames.SetGW, 202);
	this->cCommand.Add(m_CommandNames.Requests, 203);
	this->cCommand.Add(m_CommandNames.SoccerStart, 204);
	this->cCommand.Add(m_CommandNames.KundunInfo, 320);
	this->cCommand.Add(m_CommandNames.SetKundunHP, 321);
	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		this->cCommand.Add(m_CommandNames.CSSetOwnerCMD, 331);
		this->cCommand.Add(m_CommandNames.CSSetRegCMD, 332);
		this->cCommand.Add(m_CommandNames.CSSetMarkCMD, 333);
		this->cCommand.Add(m_CommandNames.CSSetNotifyCMD, 334);
		this->cCommand.Add(m_CommandNames.CSSetReadyCMD, 335);
		this->cCommand.Add(m_CommandNames.CSSetEndCMD, 336);
		this->cCommand.Add(m_CommandNames.CSGetOwnerCMD, 337);
		this->cCommand.Add(m_CommandNames.CSGetStateCMD, 338);
		this->cCommand.Add(m_CommandNames.CSSetStartCMD, 339);
		this->cCommand.Add(m_CommandNames.CSSetChangeSideCMD, 340);
		this->cCommand.Add(m_CommandNames.CWSetStateCMD, 398);
	}
	this->cCommand.Add(m_CommandNames.WARCMD, 200);
	this->cCommand.Add(m_CommandNames.PostCMD, 391);	// PostMsg
	this->cCommand.Add(m_CommandNames.PKCLEARCMD, 392);	// ClearPkCommand
	this->cCommand.Add(m_CommandNames.AddSTRCMD, 393);
	this->cCommand.Add(m_CommandNames.AddAGICMD, 394);
	this->cCommand.Add(m_CommandNames.AddVITCMD, 395);
	this->cCommand.Add(m_CommandNames.ADDENECMD, 396);
	this->cCommand.Add(m_CommandNames.ADDCMDCMD, 397);
	this->cCommand.Add(m_CommandNames.ItemCMD, 390);
	this->cCommand.Add(m_CommandNames.ONLINECMD, 400);
	this->cCommand.Add("/info", 401);
	this->cCommand.Add("/i", 402);
	this->cCommand.Add(m_CommandNames.WARECMD, 403);
	this->cCommand.Add(m_CommandNames.PROPOSECMD, 404);
	this->cCommand.Add(m_CommandNames.ACCEPTCMD, 405);
	this->cCommand.Add(m_CommandNames.TRACEMARRYCMD, 406);
	this->cCommand.Add(m_CommandNames.SKINCMD, 420);
	this->cCommand.Add(m_CommandNames.BCStart, 408);
	this->cCommand.Add(m_CommandNames.DSStart, 409);
	this->cCommand.Add(m_CommandNames.CCStart, 410);
	this->cCommand.Add(m_CommandNames.ITStart, 411);
	this->cCommand.Add(m_CommandNames.ResetCMD, 412);
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		this->cCommand.Add(m_CommandNames.KTSetStandbyCMD, 413);
		this->cCommand.Add(m_CommandNames.KTSetMayaCMD, 414);
		this->cCommand.Add(m_CommandNames.KTSetTowerCMD, 415);
	}
	this->cCommand.Add(m_CommandNames.HideCMD, 416);
	this->cCommand.Add(m_CommandNames.UnHideCMD, 417);
	this->cCommand.Add(m_CommandNames.ClearInvCMD, 418);
	this->cCommand.Add(m_CommandNames.SvInfoCMD, 423);
	this->cCommand.Add(m_CommandNames.CharInfoCMD, 399);
	this->cCommand.Add(m_CommandNames.PKSETCMD, 421);
	this->cCommand.Add(m_CommandNames.DIVORCECMD, 422);
	this->cCommand.Add(m_CommandNames.BanAccCMD, 424);
	this->cCommand.Add(m_CommandNames.UnBanAccCMD, 425);
	this->cCommand.Add(m_CommandNames.BanCharCMD, 426);
	this->cCommand.Add(m_CommandNames.UnBanCharCMD, 427);
	this->cCommand.Add(m_CommandNames.CrewCMD, 428);
	this->cCommand.Add(m_CommandNames.GPostCMD, 429);
	this->cCommand.Add("/zxcgetaccinformationforuserzxc", 430);
	this->cCommand.Add("/unlock", 431);
	this->cCommand.Add("/lock", 432);
	this->cCommand.Add("/idle", 434);
	this->cCommand.Add(m_CommandNames.offTrade, 433);
	this->cCommand.Add(m_CommandNames.dcFriend, 440);
	this->cCommand.Add("/ifsetcheat", 441);
	this->cCommand.Add("/ifsetday", 442);
	this->cCommand.Add("/ifwarpzone", 443);
	this->cCommand.Add("/mlsreset", 444);
	this->cCommand.Add("/gethp", 445);
	this->cCommand.Add("/trans", 446);
	this->cCommand.Add(m_CommandNames.setparty, 447);
	this->cCommand.Add("/test", 448);
	this->cCommand.Add(m_CommandNames.offlevel, 449);
	this->cCommand.Add(m_CommandNames.GremoryGiftCMD, 450);
	this->cCommand.Add("/offdebug", 451);
	this->cCommand.Add("/ccsblow", 452);
	this->cCommand.Add("/ccstrap", 453);
	this->cCommand.Add("/questchangeday", 454);
	this->cCommand.Add(m_CommandNames.JoinMuChangeCMD, 455);
	this->cCommand.Add(m_CommandNames.JoinMuUpgradeCMD, 456);
	this->cCommand.Add(m_CommandNames.partyleader, 460);
	this->cCommand.Add(m_CommandNames.resettabulado, 461);
	this->cCommand.Add(m_CommandNames.MakeItems, 462);
	this->cCommand.Add(m_CommandNames.ClearInventory, 463);
	this->cCommand.Add(m_CommandNames.MakeRandomSet, 464);
	this->WatchTargetIndex = -1;
}

void CGMMng::ManagerInit()
{
	for (int n = 0; n < MAX_GAME_MASTER; n++)
	{
		this->ManagerIndex[n] = -1;
	}
}

int CGMMng::ManagerAdd(char* name, CGameObject &Obj)
{
	for (int n = 0; n < MAX_GAME_MASTER; n++)
	{
		if (this->ManagerIndex[n] == -1)
		{
			strcpy(this->szManagerName[n], name);
			this->ManagerIndex[n] = Obj.m_Index;
			return n;
		}
	}

	return -1;
}

void CGMMng::ManagerDel(char* name)
{
	for (int n = 0; n < MAX_GAME_MASTER; n++)
	{
		if (this->ManagerIndex[n] != -1)
		{
			if (strcmp(this->szManagerName[n], name) == 0)
			{
				this->ManagerIndex[n] = -1;
				break;
			}
		}
	}
}

void CGMMng::ManagerSendData(LPSTR szMsg, int size)
{
	for (int n = 0; n < MAX_GAME_MASTER; n++)
	{
		if (this->ManagerIndex[n] != -1)
		{
			GSProtocol.GCServerMsgStringSend(szMsg, this->ManagerIndex[n], 0);
		}
	}
}

void CGMMng::DataSend(BYTE* szMsg, int size)
{
	for (int n = 0; n < MAX_GAME_MASTER; n++)
	{
		if (this->ManagerIndex[n] != -1)
		{
			::IOCP.DataSend(this->ManagerIndex[n], szMsg, size);
		}
	}
}

void CGMMng::BattleInfoSend(char* Name1, BYTE score1, char* Name2, BYTE score2)
{
	for (int n = 0; n < MAX_GAME_MASTER; n++)
	{
		if (this->ManagerIndex[n] != -1)
		{
			GSProtocol.GCGoalSend(this->ManagerIndex[n], Name1, score1, Name2, score2);
		}
	}
}

int CGMMng::GetCmd(char* szCmd)
{
	for (int n = 0; n < MAX_GM_COMMAND; n++)
	{
		if (this->cCommand.nCmdCode[n] > 0)
		{
			if (stricmp(szCmd, this->cCommand.szCmd[n]) == 0)
			{
				return this->cCommand.nCmdCode[n];
			}
		}
	}

	return 0;
}


char* CGMMng::GetTokenString()
{
	char seps[2] = " ";
	return strtok(0, seps);
}
// -------------------------------------------------------------------------------

CGameObject CGMMng::GetUserInfo(CGameObject &User, LPSTR UserName)
{
	for each (std::pair<int,CGameObject*> ObjEntry in gGameObjects)
	{
		if (getGameObject(i)->Connected >= PLAYER_PLAYING)
		{
			if (getGameObject(i)->Name[0] == *UserName)
			{
				if (strcmp(getGameObject(i)->Name, UserName) == 0)
				{
					return getGameObject(i);
				}
			}
		}
	}
	// ----
	MsgOutput(lpUser.m_Index, "Target user not found");
	return NULL;
}
// -------------------------------------------------------------------------------

int CGMMng::GetTokenNumber()
{
	char seps[2] = " ";
	char * szToken = strtok(0, seps);

	if (szToken != NULL)
	{
		return atoi(szToken);
	}

	return 0;
}

void PostSend(CGameObject &Obj, char * szMessage)
{
	PMSG_POST_DATA pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xFA, 0x10, sizeof(pMsg));

	pMsg.btColorRGB[0] = g_ConfigRead.data.post.btPostColorRGB[0];
	pMsg.btColorRGB[1] = g_ConfigRead.data.post.btPostColorRGB[1];
	pMsg.btColorRGB[2] = g_ConfigRead.data.post.btPostColorRGB[2];

	std::memcpy(pMsg.szName, Obj.Name, MAX_ACCOUNT_LEN);
	std::memcpy(pMsg.szServerName, g_ConfigRead.server.GetServerName(), 50);

	strcpy(pMsg.szTag, Lang.GetText(0, 334));
	strcpy(pMsg.szMessage, szMessage);

	GSProtocol.DataSendAll((BYTE*)&pMsg, sizeof(pMsg));
}

int CGMMng::ManagementProc(CGameObject &Obj, char* szCmd, CGameObject &ObjTarget)
{
	char seps[2] = " ";
	char * szCmdToken;
	char string[256];
	char szId[20];
	char * pId = szId;
	int len = strlen(szCmd);
	int command_number;

	if (len < 1 || len > 250)
	{
		return 0;
	}
	memset(szId, 0, sizeof(szId));
	strcpy(string, szCmd);
	szCmdToken = strtok(string, seps);

	command_number = this->GetCmd(szCmdToken);

	if (command_number != 392 && command_number != 412)
	{
		//CommandLog->Output("[%s][%s][%s] Used command: %s [AUTH: %d]", 
		//	Obj.AccountID, Obj.Name, Obj.m_PlayerData->ConnectUser->IP, szCmdToken,F
		//	Obj.Authority);
	}
	//sLog->outBasic("Use GM Command -> [ %d ]", command_number);
	//sLog->outBasic("Use GM Command -> [ %d ]", command_number);
	//return 0;
	switch (command_number)
	{
	case 331:
	{
		pId = this->GetTokenString();
		g_CastleSiege.OperateGmCommand(Obj.m_Index, 0, pId);
	}
	break;
	case 332:
	{
		pId = this->GetTokenString();
		g_CastleSiege.OperateGmCommand(Obj.m_Index, 1, pId);
	}
	break;
	case 333:
	{
		pId = this->GetTokenString();
		g_CastleSiege.OperateGmCommand(Obj.m_Index, 2, pId);
	}
	break;
	case 334:
	{
		pId = this->GetTokenString();
		g_CastleSiege.OperateGmCommand(Obj.m_Index, 3, pId);
	}
	break;
	case 335:
	{
		pId = this->GetTokenString();
		g_CastleSiege.OperateGmCommand(Obj.m_Index, 4, pId);
	}
	break;
	case 336:
	{
		pId = this->GetTokenString();
		g_CastleSiege.OperateGmCommand(Obj.m_Index, 5, pId);
	}
	break;
	case 337:
	{
		pId = this->GetTokenString();
		g_CastleSiege.OperateGmCommand(Obj.m_Index, 6, pId);
	}
	break;
	case 338:
	{
		pId = this->GetTokenString();
		g_CastleSiege.OperateGmCommand(Obj.m_Index, 7, pId);
	}
	break;
	case 339:
	{
		pId = this->GetTokenString();
		g_CastleSiege.OperateGmCommand(Obj.m_Index, 9, pId);
	}
	break;
	case 340:
	{
		pId = this->GetTokenString();
		g_CastleSiege.OperateGmCommand(Obj.m_Index, 8, pId);
	}
	break;
	case 217:	//116:
	{
		if ((Obj.Authority & 2) != 2 && (Obj.Authority & 0x20) != 0x20)
		{
			return 0;
		}

		if ((Obj.GameMaster & GM_COMMAND_FIRECRACKER) != GM_COMMAND_FIRECRACKER)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 424));
			return 0;
		}

		CommandLog->Output("Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s",
			Obj.m_PlayerData->ConnectUser->IP, Obj.AccountID, Obj.Name, "FIRECRACK.");

		int x = Obj.X;
		int y = Obj.Y;
		PMSG_SERVERCMD pMsg;

		PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x40, sizeof(pMsg));
		pMsg.CmdType = 0;

		for (int i = 0; i < 15; i++)
		{
			pMsg.X = x + (rand() % 5) * 2 - 4;
			pMsg.Y = y + (rand() % 5) * 2 - 4;
			GSProtocol.MsgSendV2(lpObj, (UCHAR*)&pMsg, sizeof(pMsg));
			::IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, sizeof(pMsg));
		}
	}
	break;

	case 216:	//115:
	{
		if ((Obj.Authority & 2) != 2 && (Obj.Authority & 0x20) != 0x20)
		{
			return 0;
		}

		if ((Obj.GameMaster & GM_COMMAND_DC_MOVE) != GM_COMMAND_DC_MOVE)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 425));
			return 0;
		}

		pId = this->GetTokenString();

		if (pId == NULL)
		{
			return 0;
		}

		CGameObject lpTargetObj = gObjFind(pId);

		if (lpTargetObj == NULL)
		{
			return 0;
		}

		/*LogAddTDC(1,"Use GM Command -> [ %s ]	[ %s ]	[ %s ] / Target : [%s][%s] : %s ",
			Obj.m_PlayerData->ConnectUser->IP, Obj.AccountID, Obj.Name, lpTargetObj.AccountID,
			lpTargetObj.Name, "User Watching");*/

		char szTemp[256];

		if (this->WatchTargetIndex == lpTargetObj.m_Index)
		{
			this->WatchTargetIndex = -1;

			wsprintf(szTemp, "%s - invalid target", lpTargetObj.Name);
			GSProtocol.GCServerMsgStringSend(szTemp, Obj.m_Index, 1);
		}
		else
		{
			wsprintf(szTemp, "%s - monitoring set", lpTargetObj.Name);
			GSProtocol.GCServerMsgStringSend(szTemp, Obj.m_Index, 1);
			this->WatchTargetIndex = lpTargetObj.m_Index;
		}
	}
	break;

	case 215:	//114:
	{
		if ((Obj.Authority & 2) != 2 && (Obj.Authority & 0x20) != 0x20)
		{
			return 0;
		}

		if ((Obj.GameMaster & GM_COMMAND_DC_MOVE) != GM_COMMAND_DC_MOVE)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 425));
			return 0;
		}

		pId = this->GetTokenString();

		if (pId == NULL)
		{
			return 0;
		}

		int map;
		int iX;
		int iY;
		CGameObject lpTargetObj = gObjFind(pId);
		int iIndex;

		if (lpTargetObj == NULL)
		{
			return 0;
		}

		GMLog->Output("Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] / Target : [%s][%s] : %s",
			Obj.m_PlayerData->ConnectUser->IP, Obj.AccountID, Obj.Name, lpTargetObj.AccountID,
			lpTargetObj.Name, "User Tracking");

		map = lpTargetObj.MapNumber;
		iX = lpTargetObj.X;
		iY = lpTargetObj.Y;
		iIndex = Obj.m_Index;

		if (iIndex >= 0)
		{
			gObjTeleport(iIndex, map, iX, iY);
		}

		if (map == MAP_INDEX_HATCHERY)
		{
			g_RaklionBattleUserMng.AddUserData(Obj.m_Index);
		}

		if (map == MAP_INDEX_URUK_MOUNTAIN)
		{
			SendLordSilvesterBlockInfo(iIndex, g_bSilvesterEntranceBlock == true ? 0 : 1);
		}
	}
	break;
	case 214:	//113:
	{
		if ((Obj.Authority & 2) != 2)
		{
			return 0;
		}

		if ((Obj.GameMaster & GM_COMMAND_DC_MOVE) != GM_COMMAND_DC_MOVE)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 425));
			return 0;
		}

		sLog->outBasic("Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s",
			Obj.m_PlayerData->ConnectUser->IP, Obj.AccountID, Obj.Name,
			"User Stat (connection)");
		int lc151 = 0;
		int lc152 = 400;
		int iTokenNumber = this->GetTokenNumber();

		if (iTokenNumber > 0)
		{
			lc151 = iTokenNumber;
		}

		int iTokenNumber2 = this->GetTokenNumber();

		if (iTokenNumber2 > 0)
		{
			lc152 = iTokenNumber2;
		}

		gObjSendUserStatistic(Obj.m_Index, lc151, lc152);
	}
	break;
	case Command::Disconnect:
	{
		if ((Obj.AuthorityCode & 4) != 4)
		{
			return 0;
		}

		LPSTR UserName = this->GetTokenString();
		// ----
		if (UserName == 0)
		{
			MsgOutput(Obj.m_Index, "Syntax error in command");
			return 0;
		}
		// ----
		CGameObject lpTarget = this->GetUserInfo(lpObj, UserName);
		// ----
		if (!lpTarget)
		{
			return 0;
		}
		// ----
		IOCP.CloseClient(lpTarget->m_Index);
	}
	break;

	case 112:	//108:
	{
		if ((Obj.AuthorityCode & 4) != 4)
		{
			return 0;
		}

		if ((Obj.GameMaster & GM_COMMAND_GUILD) != GM_COMMAND_GUILD)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 426));
			return 0;
		}

		sLog->outBasic("Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s",
			Obj.m_PlayerData->ConnectUser->IP, Obj.AccountID, Obj.Name,
			"Guild Disconnect");

		GMLog->Output("Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s",
			Obj.m_PlayerData->ConnectUser->IP, Obj.AccountID, Obj.Name,
			"Guild Disconnect");

		pId = this->GetTokenString();

		if (pId == NULL)
		{
			return 0;
		}

		GUILD_INFO_STRUCT * lpGuild = Guild.SearchGuild(pId);
		int iIndex;

		if (lpGuild != NULL)
		{
			for (int i = 0; i < MAX_USER_GUILD; i++)
			{
				if (lpGuild->Index[i] >= 0)
				{
					iIndex = lpGuild->Index[i];

					if (iIndex >= 0)
					{
						sLog->outBasic("%s is forced to disconnect", pId);
						IOCP.CloseClient(Obj.m_Index);
					}
				}
			}
		}
	}
	break;

	case Command::Move:	//101:
	{
		if ((Obj.GameMaster & GM_COMMAND_GUILD) == GM_COMMAND_GUILD && (Obj.GameMaster & GM_COMMAND_DC_MOVE) == GM_COMMAND_DC_MOVE)
		{
			pId = this->GetTokenString();

			if (pId == NULL)
			{
				return 0;
			}

			int iTokenNumber1 = this->GetTokenNumber();
			int iTokenNumber2 = this->GetTokenNumber();
			int iTokenNumber3 = this->GetTokenNumber();
			int iIndex = gObjGetIndex(pId);
			CGameObject lpTargetObj;

			if (iIndex >= 0)
			{
				lpTargetObj = gObjFind(pId);

				if (lpTargetObj == NULL)
				{
					return 0;
				}

				int nCurrentMap = lpTargetObj.MapNumber;

				GMLog->Output("Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] / Target : [%s][%s] : %s",
					Obj.m_PlayerData->ConnectUser->IP, Obj.AccountID, Obj.Name, lpTargetObj.AccountID,
					lpTargetObj.Name, "User SetPosition");

				gObjTeleport(iIndex, iTokenNumber1, iTokenNumber2, iTokenNumber3);

				if (nCurrentMap != iTokenNumber1)
				{
					g_SendNPCInfo.SendNpcCoordinate(lpTargetObj.m_Index, lpTargetObj.MapNumber, 0);
					g_SendNPCInfo.SendPortalCoordinate(lpTargetObj.m_Index, lpTargetObj.MapNumber);
				}

				if (iTokenNumber1 == MAP_INDEX_HATCHERY)
				{
					g_RaklionBattleUserMng.AddUserData(Obj.m_Index);
				}

				if (iTokenNumber1 == MAP_INDEX_URUK_MOUNTAIN)
				{
					SendLordSilvesterBlockInfo(iIndex, g_bSilvesterEntranceBlock == true ? 0 : 1);
				}
			}
		}
		else
		{
			pId = this->GetTokenString();

			if (pId != NULL)
			{
				int lc165 = -1;
				int lc166 = 0;
				int lc167 = 0;

				if (Obj.Teleport != 0)
				{
					GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 153), Obj.m_Index, 1);
					return 0;
				}

				if ((Obj.m_IfState->use) != 0)
				{
					if (Obj.m_IfState->type == 3)
					{
						Obj.TargetNpcNumber = -1;
						Obj.m_IfState->type = 0;
						Obj.m_IfState->use = 0;
					}
				}

				if (Obj.m_IfState->use > 0)
				{
					GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 153), Obj.m_Index, 1);
					return 0;
				}

				if (Obj.IsInBattleGround != false)
				{
					GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 153), Obj.m_Index, 1);
					return 0;
				}
			}
		}
	}
	break;

	case 108:	//104:
	{
		if ((Obj.AuthorityCode & 8) != 8)
		{
			return 0;
		}

		if ((Obj.GameMaster & GM_COMMAND_GUILD) != GM_COMMAND_GUILD)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 426));
			return 0;
		}

		GMLog->Output("Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s",
			Obj.m_PlayerData->ConnectUser->IP, Obj.AccountID, Obj.Name,
			"Guild SetPosition");

		pId = this->GetTokenString();

		if (pId == NULL)
		{
			return 0;
		}

		int iTokenNumber1 = this->GetTokenNumber();
		int iTokenNumber2 = this->GetTokenNumber();
		int iTokenNumber3 = this->GetTokenNumber();
		GUILD_INFO_STRUCT &lpGuild = Guild.SearchGuild(pId);
		int iIndex;

		if (lpGuild != NULL)
		{
			for (int i = 0; i < MAX_USER_GUILD; i++)
			{
				if (lpGuild->Index[i] >= 0)
				{
					iIndex = lpGuild->Index[i];
					gObjTeleport(iIndex, iTokenNumber1, iTokenNumber2++, iTokenNumber3);
				}
			}
		}
	}
	break;

	case 109:	//105:
	{
		if ((Obj.Authority & 2) == 2)
		{
			if ((Obj.GameMaster & GM_COMMAND_BATTLESOCCER) != GM_COMMAND_BATTLESOCCER)
			{
				MsgOutput(Obj.m_Index, Lang.GetText(0, 427));
				return 0;
			}

			GMLog->Output("Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s",
				Obj.m_PlayerData->ConnectUser->IP, Obj.AccountID, Obj.Name,
				"Start BattleSoccer");

			BattleSoccerGoalStart(0);
		}
	}
	break;

	case 110:	//106:
	{
		if ((Obj.Authority & 2) == 2)
		{
			if ((Obj.GameMaster & GM_COMMAND_BATTLESOCCER) != GM_COMMAND_BATTLESOCCER)
			{
				MsgOutput(Obj.m_Index, Lang.GetText(0, 856));
				return 0;
			}

			GMLog->Output("Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s",
				Obj.m_PlayerData->ConnectUser->IP, Obj.AccountID, Obj.Name,
				"Stop BattleSoccer");

			BattleSoccerGoalEnd(0);
		}
		else
		{
			if (Obj.m_PlayerData->lpGuild != NULL)
			{
				if (Obj.m_PlayerData->lpGuild->WarType == 1)
				{
					strcmp(Obj.Name, Obj.m_PlayerData->lpGuild->Names[0]);
				}
			}
		}
	}

	break;

	case 111:	//107:
	{
		if ((Obj.Authority & 2) == 2)
		{
			if ((Obj.GameMaster & GM_COMMAND_GUILD) != GM_COMMAND_GUILD)
			{
				MsgOutput(Obj.m_Index, Lang.GetText(0, 426));
				return 0;
			}

			
			GMLog->Output("Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s",
				Obj.m_PlayerData->ConnectUser->IP, Obj.AccountID, Obj.Name, "End GuildWar");

			char * szGuild = this->GetTokenString();

			if (szGuild != NULL)
			{
				GSProtocol.GCManagerGuildWarEnd(szGuild);
			}
		}
		else
		{
			if (Obj.m_PlayerData->lpGuild != NULL && Obj.m_PlayerData->lpGuild->lpTargetGuildNode != NULL)
			{
				if (strcmp(Obj.Name, Obj.m_PlayerData->lpGuild->Names[0]) == 0)
				{
					if (Obj.m_PlayerData->lpGuild->BattleGroundIndex >= 0 && Obj.m_PlayerData->lpGuild->WarType == 1)
					{
						::gObjAddMsgSendDelay(lpObj2, 7, lpObj2.m_Index, 10000, 0);

						char szTemp[100];

						GUILD_INFO_STRUCT* guild = Obj.m_PlayerData->lpGuild;
						wsprintf(szTemp, Lang.GetText(0, 61), Obj.m_PlayerData->lpGuild->Names[0]);
						GSProtocol.GCServerMsgStringSendGuild(*guild, szTemp, 1);
						GSProtocol.GCServerMsgStringSendGuild(*guild, szTemp, 1);
					}
				}
			}
		}
	}
	break;

	case 104:	//102:
	{
		if ((Obj.AuthorityCode & 0x20) != 0x20)
		{
			return FALSE;
		}

		if ((Obj.GameMaster & GM_COMMAND_DC_MOVE) != GM_COMMAND_DC_MOVE)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 425));
			return 0;
		}

		GMLog->Output("Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s", Obj.m_PlayerData->ConnectUser->IP, Obj.AccountID,
			Obj.Name, "Ban Chatting");

		pId = this->GetTokenString();
		int BanTime = this->GetTokenNumber();

		if (pId == NULL)
		{
			return FALSE;
		}

		time_t bantime = time(NULL) + (BanTime * 60);

		int Index = ::gObjGetIndex(pId);

		if (Index >= 0)
		{
			getGameObject(Index)->PenaltyMask |= 1;
			getGameObject(Index)->Penalty |= 2;
			getGameObject(Index)->ChatBlockTime = bantime;
			MsgOutput(Obj.m_Index, Lang.GetText(0, 428));
		}

	}
	break;

	case 106:	//103:
	{
		if ((Obj.AuthorityCode & 32) != 32)
		{
			return FALSE;
		}

		if ((Obj.GameMaster & GM_COMMAND_DC_MOVE) != GM_COMMAND_DC_MOVE)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 425));
			return 0;
		}

		GMLog->Output("Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s", Obj.m_PlayerData->ConnectUser->IP, Obj.AccountID,
			Obj.Name, "Free Ban-Chatting");

		pId = this->GetTokenString();

		if (pId == NULL)
		{
			return FALSE;
		}

		int Index = ::gObjGetIndex(pId);

		if (Index >= 0)
		{
			getGameObject(Index)->PenaltyMask &= ~1;
			getGameObject(Index)->Penalty &= ~2;
			getGameObject(Index)->ChatBlockTime = 0;
			MsgOutput(Obj.m_Index, Lang.GetText(0, 428));
		}
	}
	break;

	case 200:	//109:
	{
		pId = this->GetTokenString();

		if (pId != NULL)
		{
			if (strlen(pId) >= 1)
			{
				GSProtocol.GCGuildWarRequestResult(pId, Obj.m_Index, 0);
			}
		}
	}

	break;

	case 202:	//111:
	{
		if ((Obj.Authority & 2) == 2)
		{
			if ((Obj.GameMaster & GM_COMMAND_GUILD) != GM_COMMAND_GUILD)
			{
				MsgOutput(Obj.m_Index, Lang.GetText(0, 426));
				return 0;
			}

			GMLog->Output("Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s", Obj.m_PlayerData->ConnectUser->IP, Obj.AccountID,
				Obj.Name, "Set GuildWar");

			pId = this->GetTokenString();

			if (pId != NULL)
			{
				char * Rival = this->GetTokenString();

				if (Rival != NULL)
				{
					if (strlen(pId) >= 1)
					{
						if (strlen(Rival) >= 1)
						{
							GSProtocol.GCManagerGuildWarSet(pId, Rival, 1);
						}
					}
				}
			}
		}
	}

	break;

	case 204:
	{
		if (gEnableBattleSoccer != FALSE)
		{
			pId = this->GetTokenString();

			if (pId != NULL)
			{
				if (strlen(pId) >= 1)
				{
					GSProtocol.GCGuildWarRequestResult(pId, Obj.m_Index, 1);
				}
			}
		}
	}
	break;

	case 201:	//110:
	{
		gObjBillRequest(getGameObject(Obj.m_Index));
	}
	break;
	// Commands by [IGC] drakelv
	case 391:
	{
		if (Obj.ChatLimitTime > 0)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 139), Obj.ChatLimitTime);
			return FALSE;
		}

		if (Obj.Level < g_ConfigRead.data.post.wPostLevel)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 429), g_ConfigRead.data.post.wPostLevel);
			return FALSE;
		}

		if (Obj.m_PlayerData->Money < g_ConfigRead.data.post.iPostPrice)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 430), g_ConfigRead.data.post.iPostPrice);
			return FALSE;
		}

		if (strlen((char*)szCmd + strlen(this->m_CommandNames.PostCMD)) >= MAX_CHAT_LEN)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 642));
			return FALSE;
		}

		if (SwearFilter.CompareText(szCmd + strlen(this->m_CommandNames.PostCMD)) == true)
		{
			Obj.m_PlayerData->SwearWarning++;

			if (Obj.m_PlayerData->SwearWarning >= 3)
			{
				GSProtocol.GCSendDisableReconnect(Obj.m_Index);
				MsgOutput(Obj.m_Index, Lang.GetText(0, 431));
			}

			else
			{
				MsgOutput(Obj.m_Index, Lang.GetText(0, 432), Obj.m_PlayerData->SwearWarning);
				Obj.m_PlayerData->Money -= g_ConfigRead.data.post.iPostPrice * 2;
				GSProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);
			}

			return 0;
		}

		if (GetTickCount() - Obj.m_PostCMDLastUseTick < (g_ConfigRead.data.post.dwPostCooldown * 1000))
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 601), g_ConfigRead.data.post.dwPostCooldown);
			return 0;
		}

		Obj.m_PostCMDLastUseTick = GetTickCount();
		Obj.m_PlayerData->Money -= g_ConfigRead.data.post.iPostPrice;
		GSProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

		PostSend(lpObj, (char*)szCmd + strlen(this->m_CommandNames.PostCMD));
		break;
	}

	case 429: // MultiCast Post
	{
		if (Obj.ChatLimitTime > 0)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 139), Obj.ChatLimitTime);
			return FALSE;
		}

		if (Obj.Level < g_ConfigRead.data.post.wGPostLevel)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 429), g_ConfigRead.data.post.wPostLevel);
			return FALSE;
		}

		if (Obj.m_PlayerData->Money < g_ConfigRead.data.post.iGPostPrice)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 430), g_ConfigRead.data.post.iPostPrice);
			return FALSE;
		}

		if (SwearFilter.CompareText(szCmd + strlen(m_CommandNames.GPostCMD)) == true)
		{
			Obj.m_PlayerData->SwearWarning++;

			if (Obj.m_PlayerData->SwearWarning >= 3)
			{
				GSProtocol.GCSendDisableReconnect(Obj.m_Index);
				MsgOutput(Obj.m_Index, Lang.GetText(0, 431));
				//IOCP.CloseClient(Obj.m_Index);
			}

			else
			{
				MsgOutput(Obj.m_Index, Lang.GetText(0, 432), Obj.m_PlayerData->SwearWarning);
				Obj.m_PlayerData->Money -= g_ConfigRead.data.post.iPostPrice * 2;
				GSProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);
			}

			return 0;
		}

		if (GetTickCount() - Obj.m_PostCMDLastUseTick < (g_ConfigRead.data.post.dwGPostCooldown * 1000))
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 601), g_ConfigRead.data.post.dwGPostCooldown);
			return 0;
		}

		Obj.m_PostCMDLastUseTick = GetTickCount();
		Obj.m_PlayerData->Money -= g_ConfigRead.data.post.iPostPrice;
		GSProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

		GS_GDReqGlobalPostMultiCast(g_MapServerManager.GetMapSvrGroup(), Obj.m_Index, (char*)szCmd + strlen(m_CommandNames.GPostCMD));
		break;
	}
	break;
	case 430:
	{
		/* pId = GetTokenString();
		int index = gObjGetIndex(pId);

		if(index == -1)
			break;

		MsgOutput(Obj.m_Index, "Name:%s Login:%s", pId, getGameObject(index)->AccountID); */
	}
	break;
	case Command::AddStr:
	{
		int Pontos;
		Pontos = GetTokenNumber();
		if (Pontos == NULL)
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 434), Obj.m_Index, 1);
			return FALSE;
		}
		if (Obj.m_PlayerData->LevelUpPoint < Pontos || Pontos < 1)
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 435), Obj.m_Index, 1);
			return FALSE;
		}
		if ((Pontos + Obj.m_PlayerData->Strength) > g_MaxStatsInfo.GetClass.MaxStats[Obj.Class][0])
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 436), Obj.m_Index, 1);
			return FALSE;
		}
		Obj.m_PlayerData->LevelUpPoint -= Pontos;
		Obj.m_PlayerData->Strength += Pontos;

		PMSG_ADDSTATS pStats;
		pStats.h.c = 0xC1;
		pStats.h.headcode = 0xFA;
		pStats.h.size = sizeof(pStats);
		pStats.subcode = 0x90;
		pStats.Points = Obj.m_PlayerData->Strength;
		pStats.statstype = 0;
		pStats.LUP = Obj.m_PlayerData->LevelUpPoint;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pStats, sizeof(pStats));

		gObjCalCharacter.CalcCharacter(Obj.m_Index);
		gObjSetBP(Obj.m_Index);
		GSProtocol.GCLevelUpMsgSend(Obj.m_Index, 1);
		gObjCalcMaxLifePower(Obj.m_Index);
		return TRUE;
	}
	break;
	case Command::AddAgi:
	{
		int Pontos;
		Pontos = GetTokenNumber();

		if (Pontos == NULL)
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 434), Obj.m_Index, 1);
			return FALSE;
		}
		if (Obj.m_PlayerData->LevelUpPoint < Pontos || Pontos < 1)
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 435), Obj.m_Index, 1);
			return FALSE;
		}
		if ((Pontos + Obj.m_PlayerData->Dexterity) > g_MaxStatsInfo.GetClass.MaxStats[Obj.Class][1])
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 436), Obj.m_Index, 1);
			return FALSE;
		}

		EnterCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);
		Obj.m_PlayerData->AgilityCheckDelay = GetTickCount();
		Obj.m_PlayerData->Dexterity += Pontos;
		Obj.m_PlayerData->LevelUpPoint -= Pontos;
		LeaveCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);

		PMSG_ADDSTATS pStats;
		pStats.h.c = 0xC1;
		pStats.h.headcode = 0xFA;
		pStats.h.size = sizeof(pStats);
		pStats.subcode = 0x90;
		pStats.Points = Obj.m_PlayerData->Dexterity;
		pStats.statstype = 1;
		pStats.LUP = Obj.m_PlayerData->LevelUpPoint;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pStats, sizeof(pStats));

		gObjCalCharacter.CalcCharacter(Obj.m_Index);
		GSProtocol.GCLevelUpMsgSend(Obj.m_Index, 1);
		return TRUE;
	}
	break;
	case Command::AddVit:
	{
		int Pontos;
		Pontos = GetTokenNumber();
		if (Pontos == NULL)
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 434), Obj.m_Index, 1);
			return FALSE;
		}
		if (Obj.m_PlayerData->LevelUpPoint < Pontos || Pontos < 1)
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 435), Obj.m_Index, 1);
			return FALSE;
		}
		if ((Pontos + Obj.m_PlayerData->Vitality) > g_MaxStatsInfo.GetClass.MaxStats[Obj.Class][2])
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 436), Obj.m_Index, 1);
			return FALSE;
		}

		Obj.m_PlayerData->LevelUpPoint -= Pontos;
		Obj.m_PlayerData->Vitality += Pontos;
		Obj.MaxLife += Obj.VitalityToLife * Pontos;
		gObjCalcMaxLifePower(Obj.m_Index);

		PMSG_ADDSTATS pStats;
		pStats.h.c = 0xC1;
		pStats.h.headcode = 0xFA;
		pStats.h.size = sizeof(pStats);
		pStats.subcode = 0x90;
		pStats.Points = Obj.m_PlayerData->Vitality;
		pStats.statstype = 2;
		pStats.LUP = Obj.m_PlayerData->LevelUpPoint;
		pStats.MaxLifeAndMana = Obj.MaxLife + Obj.AddLife;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pStats, sizeof(pStats));

		gObjCalCharacter.CalcCharacter(Obj.m_Index);
		GSProtocol.GCLevelUpMsgSend(Obj.m_Index, 1);
		return TRUE;
	}
	case Command::AddEne:
	{
		int Pontos;
		Pontos = GetTokenNumber();
		if (Pontos == NULL)
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 434), Obj.m_Index, 1);
			return FALSE;
		}
		if (Obj.m_PlayerData->LevelUpPoint < Pontos || Pontos < 1)
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 435), Obj.m_Index, 1);
			return FALSE;
		}
		if ((Pontos + Obj.m_PlayerData->Energy) > g_MaxStatsInfo.GetClass.MaxStats[Obj.Class][3])
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 436), Obj.m_Index, 1);
			return FALSE;
		}

		Obj.m_PlayerData->Energy += Pontos;
		Obj.m_PlayerData->LevelUpPoint -= Pontos;
		Obj.MaxMana += Obj.EnergyToMana * Pontos;

		PMSG_ADDSTATS pStats;
		pStats.h.c = 0xC1;
		pStats.h.headcode = 0xFA;
		pStats.h.size = sizeof(pStats);
		pStats.subcode = 0x90;
		pStats.Points = Obj.m_PlayerData->Energy;
		pStats.statstype = 3;
		pStats.LUP = Obj.m_PlayerData->LevelUpPoint;
		pStats.MaxLifeAndMana = Obj.MaxMana + Obj.AddMana;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pStats, sizeof(pStats));

		gObjCalCharacter.CalcCharacter(Obj.m_Index);
		GSProtocol.GCLevelUpMsgSend(Obj.m_Index, 1);
		return TRUE;
		break;
	}
	case Command::AddCmd:
	{
		if (Obj.Class != 4)
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 441), Obj.m_Index, 1);
			return FALSE;
		}

		int Pontos;
		Pontos = GetTokenNumber();
		if (Pontos == NULL)
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 434), Obj.m_Index, 1);
			return FALSE;
		}
		if (Obj.m_PlayerData->LevelUpPoint < Pontos || Pontos < 1)
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 435), Obj.m_Index, 1);
			return FALSE;
		}
		if ((Pontos + Obj.Leadership) > g_MaxStatsInfo.GetClass.MaxStats[Obj.Class][4])
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 436), Obj.m_Index, 1);
			return FALSE;
		}

		Obj.Leadership += Pontos;
		Obj.m_PlayerData->LevelUpPoint -= Pontos;

		PMSG_ADDSTATS pStats;
		pStats.h.c = 0xC1;
		pStats.h.headcode = 0xFA;
		pStats.h.size = sizeof(pStats);
		pStats.subcode = 0x90;
		pStats.Points = Obj.Leadership;
		pStats.statstype = 4;
		pStats.LUP = Obj.m_PlayerData->LevelUpPoint;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pStats, sizeof(pStats));

		gObjCalCharacter.CalcCharacter(Obj.m_Index);
		GSProtocol.GCLevelUpMsgSend(Obj.m_Index, 1);
		return TRUE;
	}
	case 435:
	{
		int Pontos;
		Pontos = GetTokenNumber();
		if (Pontos == NULL)
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 434), Obj.m_Index, 1);
			return FALSE;
		}
		if (Obj.m_PlayerData->LevelUpPoint < Pontos || Pontos < 1)
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 435), Obj.m_Index, 1);
			return FALSE;
		}
		if ((Pontos + Obj.m_PlayerData->Strength) > g_MaxStatsInfo.GetClass.MaxStats[Obj.Class][0])
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 436), Obj.m_Index, 1);
			return FALSE;
		}
		Obj.m_PlayerData->LevelUpPoint -= Pontos;
		Obj.m_PlayerData->Strength += Pontos;
		char Msg[100];
		sprintf(Msg, Lang.GetText(0, 437), Pontos);
		GSProtocol.GCServerMsgStringSend(Msg, Obj.m_Index, 1);
		return TRUE;
	}
	break;
	case 436:
	{
		int Pontos = GetTokenNumber();

		if (Pontos == NULL)
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 434), Obj.m_Index, 1);
			return FALSE;
		}
		if (Obj.m_PlayerData->LevelUpPoint < Pontos || Pontos < 1)
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 435), Obj.m_Index, 1);
			return FALSE;
		}
		if ((Pontos + Obj.m_PlayerData->Dexterity) > g_MaxStatsInfo.GetClass.MaxStats[Obj.Class][1])
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 436), Obj.m_Index, 1);
			return FALSE;
		}

		EnterCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);
		Obj.m_PlayerData->AgilityCheckDelay = GetTickCount();
		Obj.m_PlayerData->Dexterity += Pontos;
		Obj.m_PlayerData->LevelUpPoint -= Pontos;
		LeaveCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);

		char Msg[100];
		sprintf(Msg, Lang.GetText(0, 438), Pontos);
		GSProtocol.GCServerMsgStringSend(Msg, Obj.m_Index, 1);

		return TRUE;
	}
	break;
	case 437:
	{
		int Pontos;
		Pontos = GetTokenNumber();
		if (Pontos == NULL)
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 434), Obj.m_Index, 1);
			return FALSE;
		}
		if (Obj.m_PlayerData->LevelUpPoint < Pontos || Pontos < 1)
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 435), Obj.m_Index, 1);
			return FALSE;
		}
		if ((Pontos + Obj.m_PlayerData->Vitality) > g_MaxStatsInfo.GetClass.MaxStats[Obj.Class][2])
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 407), Obj.m_Index, 1);
			return FALSE;
		}

		Obj.m_PlayerData->LevelUpPoint -= Pontos;
		Obj.m_PlayerData->Vitality += Pontos;
		char Msg[100];
		sprintf(Msg, Lang.GetText(0, 439), Pontos);
		GSProtocol.GCServerMsgStringSend(Msg, Obj.m_Index, 1);
		gObjCalCharacter.CalcCharacter(Obj.m_Index);
		return TRUE;
	}
	case 438:
	{
		int Pontos;
		Pontos = GetTokenNumber();
		if (Pontos == NULL)
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 434), Obj.m_Index, 1);
			return FALSE;
		}
		if (Obj.m_PlayerData->LevelUpPoint < Pontos || Pontos < 1)
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 435), Obj.m_Index, 1);
			return FALSE;
		}
		if ((Pontos + Obj.m_PlayerData->Energy) > g_MaxStatsInfo.GetClass.MaxStats[Obj.Class][3])
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 436), Obj.m_Index, 1);
			return FALSE;
		}

		Obj.m_PlayerData->Energy += Pontos;
		Obj.m_PlayerData->LevelUpPoint -= Pontos;
		char Msg[100];
		sprintf(Msg, Lang.GetText(0, 440), Pontos);
		GSProtocol.GCServerMsgStringSend(Msg, Obj.m_Index, 1);
		gObjCalCharacter.CalcCharacter(Obj.m_Index);
		return TRUE;
		break;
	}
	case 439:
	{
		if (Obj.Class != 4)
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 441), Obj.m_Index, 1);
			return FALSE;
		}

		int Pontos;
		Pontos = GetTokenNumber();
		if (Pontos == NULL)
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 434), Obj.m_Index, 1);
			return FALSE;
		}
		if (Obj.m_PlayerData->LevelUpPoint < Pontos || Pontos < 1)
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 435), Obj.m_Index, 1);
			return FALSE;
		}
		if ((Pontos + Obj.Leadership) > g_MaxStatsInfo.GetClass.MaxStats[Obj.Class][4])
		{
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 436), Obj.m_Index, 1);
			return FALSE;
		}

		Obj.Leadership += Pontos;
		Obj.m_PlayerData->LevelUpPoint -= Pontos;
		char Msg[100];
		sprintf(Msg, Lang.GetText(0, 442), Pontos);
		GSProtocol.GCServerMsgStringSend(Msg, Obj.m_Index, 1);
		return TRUE;
	}

	case 423:
	{
		SYSTEMTIME t;
		GetLocalTime(&t);
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx(&pNotice, 0, Lang.GetText(0, 443), g_ConfigRead.server.GetServerName(), g_ConfigRead.server.GetGameServerCode(), gObjTotalUser, g_ConfigRead.server.GetObjectMaxUser());
		TNotice::SendNoticeToUser(Obj.m_Index, &pNotice);
		TNotice::MakeNoticeMsgEx(&pNotice, 0, Lang.GetText(0, 444), t.wHour, t.wMinute, t.wSecond, t.wDay, t.wMonth, t.wYear);
		TNotice::SendNoticeToUser(Obj.m_Index, &pNotice);
		break;
	}

	case 390:
	{
		if (Obj.Authority != 0x20)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 417));
			return FALSE;
		}

		if ((Obj.GameMaster & GM_COMMAND_ITEM_CREATE) != GM_COMMAND_ITEM_CREATE)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 418));
			return 0;
		}

		if (gIsDropCmd == 0)
		{
			return 0;
		}

		int type, index, ItemLevel, ItemSkill, ItemLuck, ItemOpt, ItemAncient, ItemExpireTime, ItemSocketCount, MainAttribute, MuunEvoItemType, MuunEvoItemIndex;
		std::string szItemExc;

		type = GetTokenNumber();
		index = GetTokenNumber();
		ItemLevel = GetTokenNumber();
		ItemSkill = GetTokenNumber();
		ItemLuck = GetTokenNumber();
		ItemOpt = GetTokenNumber();

		char *szString = GetTokenString();

		if (szString == nullptr)
		{
			szItemExc = "-1;-1;-1;-1;-1;-1;-1;-1;-1";
		}

		else
		{
			szItemExc = szString;
		}

		ItemAncient = GetTokenNumber();
		ItemExpireTime = GetTokenNumber();
		ItemSocketCount = GetTokenNumber();
		MainAttribute = GetTokenNumber();
		MuunEvoItemType = GetTokenNumber();
		MuunEvoItemIndex = GetTokenNumber();

		if ((type >= 0 && type <= 17))
		{
			CItemObject Item;
			Item.m_Type = ItemGetNumberMake(type, index);

			if (Item.m_Type == -1)
			{
				sLog->outError( "[Game Master][Create Item][%s][%s][%s] - Wrong ItemType (%d)",
					Obj.AccountID, Obj.Name, Obj.m_PlayerData->ConnectUser->IP, Item.m_Type);

				return 0;
			}

			ITEM_ATTRIBUTE * p = &ItemAttribute[Item.m_Type];

			BYTE btExcOptionList[9];
			memset(btExcOptionList, -1, sizeof(btExcOptionList));

			std::deque<int> exc_opt_list;
			strtk::parse(szItemExc, ";", exc_opt_list);

			for (int i = 0; i < exc_opt_list.size(); i++)
			{
				btExcOptionList[i] = exc_opt_list.at(i);
			}

			if (btExcOptionList[0] == (BYTE)-2)
			{
				if (p->ItemKindA == ITEM_KIND_A_WING)
				{
					Item.m_NewOption = g_ItemOptionTypeMng.WingExcOptionRand(p->ItemKindA, p->ItemKindB, Item.m_SocketOption);
				}

				else
				{
					Item.m_NewOption = g_ItemOptionTypeMng.CommonExcOptionRand(p->ItemKindA, Item.m_SocketOption);
				}
			}

			else if (btExcOptionList[0] == (BYTE)-3)
			{
				if (p->ItemKindA == ITEM_KIND_A_WING)
				{
					Item.m_NewOption = g_ItemOptionTypeMng.WingExcOptionGetCount(p->ItemKindA, p->ItemKindB, Item.m_SocketOption, btExcOptionList[1]);
				}

				else
				{
					Item.m_NewOption = g_ItemOptionTypeMng.CommonExcOptionGetCount(p->ItemKindA, Item.m_SocketOption, btExcOptionList[1]);
				}
			}

			else
			{
				for (int i = 0; i < 9; i++)
				{
					if (btExcOptionList[i] == -1)
					{
						continue;
					}

					if (btExcOptionList[i] >= 6)
					{
						bool bOptionFound = false;

						for (int j = 0; j < 5; j++)
						{
							if (Item.m_SocketOption[j] == btExcOptionList[i])
							{
								bOptionFound = true;
								break;
							}
						}

						if (bOptionFound == true)
						{
							continue;
						}

						for (int j = 0; j < 5; j++)
						{
							if (Item.m_SocketOption[j] == 0xFF)
							{
								Item.m_SocketOption[j] = btExcOptionList[i];
								break;
							}
						}
					}

					else
					{
						Item.m_NewOption |= (1 << (5 - btExcOptionList[i]));
					}
				}
			}

			if (gSocketItemType.CheckSocketItemType((int)&Item) == true)
			{
				if (ItemSocketCount == 0)
				{
					g_SocketOptionSystem.MakeSocketSlot(&Item, rand() % 5 + 1);
				}

				else
				{
					g_SocketOptionSystem.MakeSocketSlot(&Item, ItemSocketCount);
				}

				MainAttribute = -1;
			}

			else if (g_PentagramSystem.IsPentagramItem(Item.m_Type) == true)
			{
				if (ItemSocketCount == 0)
				{
					g_PentagramSystem.GetMakePentagramSlotCountNKind(Item.m_SocketOption, Item.m_Type);
				}

				else
				{
					BYTE Slot[5] = { 0, 0, 0, 0, 0 };

					for (int i = 0; i < ItemSocketCount; i++)
					{
						Slot[i] = 1;
					}

					g_PentagramSystem.MakePentagramSocketSlot(&Item, Slot[0], Slot[1], Slot[2], Slot[3], Slot[4]);
				}
			}

			else if (g_CMuunSystem.IsStoneofEvolution(Item.m_Type) == TRUE)
			{
				int iMuunEvolutionItemID = ITEMGET(MuunEvoItemType, MuunEvoItemIndex);

				Item.m_SocketOption[0] = iMuunEvolutionItemID >> 8;
				Item.m_SocketOption[1] = ITEM_GET_INDEX(iMuunEvolutionItemID);
				sLog->outBasic("ITEM (%d) %d %d", type, Item.m_SocketOption[0], Item.m_SocketOption[1]); //[K2]
			}

			if (type == 13 && (index == 4 || index == 5))
			{
				PetItemCreate(Obj.m_Index, Obj.MapNumber, Obj.X, Obj.Y, Item.m_Type, ItemLevel, 0, ItemSkill, ItemLuck, ItemOpt, -1, Item.m_NewOption, ItemAncient);
			}
			else
			{
				ItemCreate(Obj.m_Index, 227, Obj.X, Obj.Y, Item.m_Type, ItemLevel, 0, ItemSkill, ItemLuck, ItemOpt, -1, Item.m_NewOption, ItemAncient, ItemExpireTime, Item.m_SocketOption, MainAttribute);
			}

			GMLog->Output("[%s][%s][%s] Created Item using Admin Command (%s/%d/%d) Auth: %d", Obj.AccountID, Obj.Name, Obj.m_PlayerData->ConnectUser->IP, Lang.GetMap(0, Obj.MapNumber), Obj.X, Obj.Y, Obj.Authority);
		}
		break;
	}

	case 399:
	{
		MsgOutput(Obj.m_Index, Lang.GetText(0, 445));
		MsgOutput(Obj.m_Index, Lang.GetText(0, 446), Obj.AccountID, Obj.Name, Obj.Level, Obj.m_PlayerData->MasterLevel, Obj.Class);
		MsgOutput(Obj.m_Index, Lang.GetText(0, 447), Obj.m_PlayerData->SwearWarning, Lang.GetMap(0, Obj.MapNumber), Obj.X, Obj.Y);
		MsgOutput(Obj.m_Index, Lang.GetText(0, 448), gObjIsConnectedGP(gObjGetIndex(Obj.MarryName)));
		break;
	}

	case 404:
	{
		pId = this->GetTokenString();

		if (pId == 0)
		{
			return 0;
		}

		Marry.Propose(Obj.m_Index, gObjGetIndex(pId));
		break;
	}
	case 405:
	{
		Marry.Accept(Obj.m_Index);
		break;
	}

	case 421:
	{
		pId = this->GetTokenString();
		int pklevel = this->GetTokenNumber();
		int pkcount = this->GetTokenNumber();

		if (pId == 0)
		{
			return 0;
		}

		if ((Obj.GameMaster & GM_COMMAND_PK_MODIFY) != GM_COMMAND_PK_MODIFY)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 449));
			return 0;
		}

		short index = gObjGetIndex(pId);

		if (index == -1) break;
		getGameObject(index)->m_PK_Level = pklevel;
		getGameObject(index)->m_PK_Count = pkcount;
		GSProtocol.GCPkLevelSend(index, getGameObject(index)->m_PK_Level);
		break;
	}

	case 422:
	{
		Marry.Divorce(Obj.m_Index);
		break;
	}

	case 406:
	{
		if (Obj.Married == 0)
			return 0;

		if (strlen(Obj.MarryName) <= 0)
			return 0;

		int uindex = gObjGetIndex(Obj.MarryName);

		if (uindex == -1)
			return 0;

		if (this->CheckTraceMarryCondition(getGameObject(Obj.m_Index), getGameObject(uindex)) == FALSE)
		{
			return 0;
		}

		int nSrcMapNumber = Obj.MapNumber;
		int nTarMapNumber = getGameObject(uindex)->MapNumber;

		gObjTeleport(Obj.m_Index, getGameObject(uindex)->MapNumber, getGameObject(uindex)->X, getGameObject(uindex)->Y);
		if (nSrcMapNumber != nTarMapNumber)
		{
			g_SendNPCInfo.SendNpcCoordinate(Obj.m_Index, nTarMapNumber, 0);
			g_SendNPCInfo.SendPortalCoordinate(Obj.m_Index, nTarMapNumber);
		}				break;
	}

	case 392:
	{
		if (!g_ConfigRead.pk.bPkClearEnable)
			break;

		if (Obj.m_PK_Level <= 3)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 501));
			return 0;
		}

		UINT64 Price;

		if (g_ConfigRead.pk.bIsCostMultiplyByKillCount)
		{
			Price = g_ConfigRead.pk.iPkClearCost * Obj.m_PK_Count;
		}
		else
		{
			Price = g_ConfigRead.pk.iPkClearCost;
		}
		if (Price > MAX_ZEN)
		{
			Price = MAX_ZEN;
		}

		if (Obj.m_PlayerData->Money < Price)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 450), Price);
		}
		else
		{
			Obj.m_PlayerData->Money -= Price;
			GSProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);
			Obj.m_PK_Level = 3;
			Obj.m_PK_Count = 0;
			GSProtocol.GCPkLevelSend(Obj.m_Index, Obj.m_PK_Level);
		}
		break;
	}
	case 403: // multi warehouse by drakelv
	{
		if (!IsMultiWareHouse)
		{
			return 0;
		}

		int WareID = this->GetTokenNumber();

		if (Obj.m_IfState->use == 1 && Obj.m_IfState->type == 6)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 111));
			return 0;
		}

		if (Obj.m_btOpenWarehouse == TRUE)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 111));
			return 0;
		}

		if (WareID < 0 || WareID > g_WareHouseCount)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 110));
			return 0;
		}

		int Time = (GetTickCount() - Obj.WarehouseTick) / 1000;

		if (Time < 30)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 451), 30 - Time);
			return 0;
		}

		if (Obj.WarehouseSaveLock == TRUE)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 598));
			return 0;
		}

		GDReqSwitchWare(Obj.m_Index, WareID);
	}
	break;

	case 203:	//112:
	{
		pId = this->GetTokenString();

		if (pId != NULL)
		{
			BOOL bState;

			if (strcmp(pId, "on") == 0)
			{
				bState = TRUE;
			}
			else if (strcmp(pId, "off") == 0)
			{
				bState = FALSE;
			}

			if (bState >= FALSE && bState <= TRUE)
			{
				gObjSetTradeOption(Obj.m_Index, bState);
				gObjSetDuelOption(Obj.m_Index, bState);
			}
		}
	}
	break;

	case 320:	//117:
	{
		if ((Obj.Authority & 2) != 2 && (Obj.Authority & 0x20) != 0x20)
		{
			return FALSE;
		}

		if ((Obj.GameMaster & GM_EVENT_MODIFY) != GM_EVENT_MODIFY)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 453));
			return 0;
		}

		sLog->outBasic("[KUNDUN] Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s", Obj.m_PlayerData->ConnectUser->IP, Obj.AccountID,
			Obj.Name, "Kundun Event Start");

		for (int n = 0; n < MAX_VIEWPORT; n++)
		{
			if (Obj.VpPlayer[n].state != 0)
			{
				if (Obj.VpPlayer[n].type == OBJ_MONSTER)
				{
					if (Obj.VpPlayer[n].number >= 0)
					{
						CGameObject lpTarget = getGameObject(Obj.VpPlayer[n]->number);

						if (lpTarget->Class == 275)
						{
							TNotice pNotice(1);

							pNotice.SendToUser(Obj.m_Index, "KUNDUN HP = %7.0f / %7.0f", lpTarget->Life, lpTarget->MaxLife);
							pNotice.SendToUser(Obj.m_Index, "HP Refill Sec = %d HP Refill Value = %d HP Refill Time = %d", giKundunRefillHPSec, giKundunRefillHP, giKundunRefillHPTime);
						}
					}
				}
			}
		}

	}
	break;

	case 321:	//118:
	{
		if ((Obj.Authority & 2) != 2 && (Obj.Authority & 0x20) != 0x20)
		{
			return FALSE;
		}

		if ((Obj.GameMaster & GM_EVENT_MODIFY) != GM_EVENT_MODIFY)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 453));
			return 0;
		}

		sLog->outBasic("[KUNDUN] Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s", Obj.m_PlayerData->ConnectUser->IP, Obj.AccountID,
			Obj.Name, "Kundun SetHP");

		int iLife = this->GetTokenNumber();

		for (int n = 0; n < MAX_VIEWPORT; n++)
		{
			if (Obj.VpPlayer[n].state != FALSE)
			{
				if (Obj.VpPlayer[n].type == OBJ_MONSTER)
				{
					if (Obj.VpPlayer[n].number >= 0)
					{
						CGameObject lpTarget = getGameObject(Obj.VpPlayer[n]->number);

						if (lpTarget->Class == 275)
						{
							if (iLife <= 5000)
							{
								iLife = 5000;
							}

							if (iLife > lpTarget->MaxLife)
							{
								iLife = lpTarget->MaxLife;
							}

							lpTarget->Life = iLife;

							TNotice pNotice(1);

							pNotice.SendToUser(Obj.m_Index, "Kundun HP = %7.0f / %7.0f", lpTarget->Life, lpTarget->MaxLife);
						}
					}
				}
			}
		}
	}
	break;

	case 322:	//119:
	{
		if ((Obj.Authority & 2) != 2 && (Obj.Authority & 0x20) != 0x20)
		{
			return FALSE;
		}

		if ((Obj.GameMaster & GM_EVENT_MODIFY) != GM_EVENT_MODIFY)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 453));
			return 0;
		}

		sLog->outBasic("[KUNDUN] Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s", Obj.m_PlayerData->ConnectUser->IP, Obj.AccountID,
			Obj.Name, "Kundun SetHPRefill");

		int RefillHP = this->GetTokenNumber();

		if (RefillHP <= 0 || RefillHP > 5000000)
		{
			return 0;
		}

		giKundunRefillHP = RefillHP;

		TNotice pNotice(0);

		pNotice.SendToUser(Obj.m_Index, "Kundun HP Refill Sec = %d Refill HP = %d Refill HP Time = %d", giKundunRefillHPSec, giKundunRefillHP, giKundunRefillHPTime);

	}

	break;

	case 323:	//120:
	{
		if ((Obj.Authority & 2) != 2 && (Obj.Authority & 0x20) != 0x20)
		{
			return FALSE;
		}

		if ((Obj.GameMaster & GM_EVENT_MODIFY) != GM_EVENT_MODIFY)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 453));
			return 0;
		}

		sLog->outBasic("[KUNDUN] Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s", Obj.m_PlayerData->ConnectUser->IP, Obj.AccountID,
			Obj.Name, "Kundun SetHPRefillSec");

		int RefillHPSec = this->GetTokenNumber();

		if (RefillHPSec <= 0 || RefillHPSec > 10000)
		{
			return 0;
		}

		giKundunRefillHPSec = RefillHPSec;

		TNotice pNotice(0);

		pNotice.SendToUser(Obj.m_Index, "Kundun HP Refill Sec = %d Refill HP = %d Refill HP Time = %d", giKundunRefillHPSec, giKundunRefillHP, giKundunRefillHPTime);
	}

	break;

	case 324:	//121:
	{
		if ((Obj.Authority & 2) != 2 && (Obj.Authority & 0x20) != 0x20)
		{
			return FALSE;
		}

		if ((Obj.GameMaster & GM_EVENT_MODIFY) != GM_EVENT_MODIFY)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 453));
			return 0;
		}

		sLog->outBasic("[KUNDUN] Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] : %s", Obj.m_PlayerData->ConnectUser->IP, Obj.AccountID,
			Obj.Name, "Kundun SetHPRefillTime");

		int RefillHPTime = this->GetTokenNumber();

		if (RefillHPTime < 0 || RefillHPTime > 60000)
		{
			return 0;
		}

		giKundunRefillHPTime = RefillHPTime;

		TNotice pNotice(0);

		pNotice.SendToUser(Obj.m_Index, "Kundun HP Refill Sec = %d Refill HP = %d Refill HP Time = %d", giKundunRefillHPSec, giKundunRefillHP, giKundunRefillHPTime);

	}
	break;
	case 400:
	{
		int oncount = 0;
		int gmoncount = 0;
		for (int g = g_ConfigRead.server.GetObjectStartUserIndex(); g < g_ConfigRead.server.GetObjectMax(); g++)
		{
			if (getGameObject(g)->Connected == 3)
			{
				oncount++;
			}
			if (getGameObject(g)->Connected == 3 && getGameObject(g)->Authority == 32)
			{
				gmoncount++;
			}
		}
		MsgOutput(Obj.m_Index, Lang.GetText(0, 454), oncount, gmoncount);
		return TRUE;
	}
	break;
	case 401:
	{
		if ((Obj.Authority & 2) != 2 && (Obj.Authority & 0x20) != 0x20)
		{
			return FALSE;
		}

		pId = this->GetTokenString();

		if (pId == NULL)
		{
			return 0;
		}

		int iTargetIndex = gObjGetIndex(pId);

		if (!ObjectMaxRange(iTargetIndex))
		{
			return 0;
		}

		if (getGameObject(iTargetIndex)->Type != OBJ_USER)
		{
			return 0;
		}

		MsgOutput(Obj.m_Index, "Character: %s", getGameObject(iTargetIndex)->Name);
		MsgOutput(Obj.m_Index, "AccountID: %s, Level:%d MasterLevel:%d", getGameObject(iTargetIndex)->AccountID, getGameObject(iTargetIndex)->Level, getGameObject(iTargetIndex)->m_PlayerData->MasterLevel);
		MsgOutput(Obj.m_Index, "Map: %s, X:%d Y:%d", Lang.GetMap(0, getGameObject(iTargetIndex)->MapNumber), getGameObject(iTargetIndex)->X, getGameObject(iTargetIndex)->Y);

	}
	break;
	case 398:
	{
		int value;
		value = GetTokenNumber();

		g_Crywolf.OperateGmCommand(Obj.m_Index, value);
	}
	break;
	case 408:
	{


		if ((Obj.Authority & 2) != 2 && (Obj.Authority & 0x20) != 0x20) /*Check if user is GM*/
			return 0;

		if ((Obj.GameMaster & GM_EVENT_MODIFY) != GM_EVENT_MODIFY)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 453));
			return 0;
		}

		g_BloodCastle.SetCheatEventOpen(350000);
		MsgOutput(Obj.m_Index, Lang.GetText(0, 455));
	}
	break;
	case 409:
	{
		if ((Obj.Authority & 2) != 2 && (Obj.Authority & 0x20) != 0x20) /*Check if user is GM*/
			return 0;

		if ((Obj.GameMaster & GM_EVENT_MODIFY) != GM_EVENT_MODIFY)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 453));
			return 0;
		}

		g_DevilSquare.SetCheatOpenTime(1);
		MsgOutput(Obj.m_Index, Lang.GetText(0, 456)); //[K2]
			//g_BagManager.UseBag_GremoryCase(Obj.m_Index, 2, 10, Obj.m_Index, 2, 2, 30);
	}
	break;
	case 410:
	{
		if ((Obj.Authority & 2) != 2 && (Obj.Authority & 0x20) != 0x20) /*Check if user is GM*/
			return 0;

		if ((Obj.GameMaster & GM_EVENT_MODIFY) != GM_EVENT_MODIFY)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 453));
			return 0;
		}

		for (int i = 0; i < MAX_CHAOSCASTLE_LEVEL; i++)
		{
			g_ChaosCastle.m_stChaosCastleData[i].m_iCC_REMAIN_MSEC = 350000;
		}
		MsgOutput(Obj.m_Index, Lang.GetText(0, 457));
	}
	break;
	case 411:
	{
		if ((Obj.Authority & 2) != 2 && (Obj.Authority & 0x20) != 0x20) /*Check if user is GM*/
			return 0;

		if ((Obj.GameMaster & GM_EVENT_MODIFY) != GM_EVENT_MODIFY)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 453));
			return 0;
		}

		for (int i = 0; i < MAX_CHAOSCASTLE_LEVEL; i++)
		{
			g_ChaosCastle.m_stChaosCastleData[i].m_iCC_REMAIN_MSEC = 350000;
		}
		MsgOutput(Obj.m_Index, Lang.GetText(0, 457));
	}
	break;
	case 412:
	{
		if (!g_ConfigRead.data.reset.enable)
			return 0;

		if (Obj.Level < g_ConfigRead.data.reset.iMinLevel)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 458), g_ConfigRead.data.reset.iMinLevel);
			return 0;
		}
		if (Obj.m_PlayerData->Money < g_ConfigRead.data.reset.iCost)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 459), g_ConfigRead.data.reset.iCost);
			return 0;
		}

		if (Obj.m_PlayerData->m_iResets >= g_ConfigRead.data.reset.iResetLimit)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 460));
			return 0;
		}

		if (BC_MAP_RANGE(Obj.MapNumber) || DS_MAP_RANGE(Obj.MapNumber) || CC_MAP_RANGE(Obj.MapNumber) || IT_MAP_RANGE(Obj.MapNumber) ||
			DG_MAP_RANGE(Obj.MapNumber) || IMPERIAL_MAP_RANGE(Obj.MapNumber) || CRYWOLF_MAP_RANGE(Obj.MapNumber) || Obj.MapNumber == MAP_INDEX_CASTLESIEGE ||
			Obj.MapNumber == MAP_INDEX_CASTLEHUNTZONE || g_GensSystem.IsMapBattleZone(Obj.MapNumber) || Obj.MapNumber == MAP_INDEX_KANTURU_BOSS || Obj.MapNumber == MAP_INDEX_KALIMA1
			|| Obj.MapNumber == MAP_INDEX_KALIMA2 || Obj.MapNumber == MAP_INDEX_KALIMA3 || Obj.MapNumber == MAP_INDEX_KALIMA4 || Obj.MapNumber == MAP_INDEX_KALIMA5 || Obj.MapNumber == MAP_INDEX_KALIMA6
			|| Obj.MapNumber == MAP_INDEX_KALIMA7 || Obj.MapNumber == MAP_INDEX_ARCA_WAR || Obj.MapNumber == MAP_INDEX_DEBENTER_ARCA_WAR)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 510));
			return FALSE;
		}

		if (g_ConfigRead.data.reset.RemoveEquipment)
		{
			for (int i = 0; i < 12; i++)
			{
				if (Obj.pntInventory[i]->IsItem() == true)
				{
					MsgOutput(Obj.m_Index, Lang.GetText(0, 461));
					return 0;
				}
			}
		}

		if (Obj.PartyNumber >= 0)
		{
			PMSG_PARTYDELUSER pMsg;
			int pindex = gParty.GetIndex(Obj.PartyNumber, Obj.m_Index, Obj.DBNumber);

			if (pindex >= 0)
			{
				pMsg.Number = pindex;
				GSProtocol.CGPartyDelUser(&pMsg, Obj.m_Index, 0);
			}
		}

		if (Obj.m_SkillInfo.LordSummonTime != 0)
		{
			Obj.m_SkillInfo.LordSummonTime = 0;
			GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 146), Obj.m_Index, 1);
		}

		gObjClearBuffEffect(lpObj, CLEAR_TYPE_NON_PCS_ITEM_EFFECT);

		Obj.Level = 1;
		Obj.m_PlayerData->Experience = 0;
		Obj.m_PlayerData->NextExp = 0;
		bool CharQuit = false;

		if (g_ConfigRead.data.reset.ResetML == true)
		{
			Obj.m_PlayerData->MasterLevel = 0;
			Obj.m_PlayerData->MasterExperience = 0;
			Obj.m_PlayerData->MasterNextExp = g_MasterLevelSkillTreeSystem.gObjNextMLExpCal(lpObj);
		}

		if (g_ConfigRead.data.reset.ResetStats == true)
		{
			if (g_ConfigRead.data.reset.SaveOldPoint == FALSE)
			{
				Obj.m_PlayerData->LevelUpPoint = 0;
			}

			EnterCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);
			Obj.m_PlayerData->AgilityCheckDelay = GetTickCount();
			Obj.m_PlayerData->Strength = DCInfo.DefClass[Obj.Class].Strength;
			Obj.m_PlayerData->Dexterity = DCInfo.DefClass[Obj.Class].Dexterity;
			Obj.m_PlayerData->Vitality = DCInfo.DefClass[Obj.Class].Vitality;
			Obj.m_PlayerData->Energy = DCInfo.DefClass[Obj.Class].Energy;
			Obj.Leadership = DCInfo.DefClass[Obj.Class].Leadership;
			LeaveCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);

			if (Obj.m_PlayerData->VipType == 0)
			{
				Obj.m_PlayerData->LevelUpPoint += (Obj.m_PlayerData->m_iResets + 1) * g_ConfigRead.data.reset.NormalPointPerReset;
			}

			else if (Obj.m_PlayerData->VipType > 0)
			{
				Obj.m_PlayerData->LevelUpPoint += (Obj.m_PlayerData->m_iResets + 1) * g_VipSystem.GetPointPerReset(lpObj);
			}

			CharQuit = true;
		}

		Obj.MaxLife = DCInfo.DefClass[Obj.Class].Life + ((Obj.Level + Obj.m_PlayerData->MasterLevel) - 1) * DCInfo.DefClass[Obj.Class].LevelLife + ((Obj.m_PlayerData->Vitality - DCInfo.DefClass[Obj.Class].Vitality) * DCInfo.DefClass[Obj.Class].VitalityToLife);
		Obj.MaxMana = DCInfo.DefClass[Obj.Class].Mana + ((Obj.Level + Obj.m_PlayerData->MasterLevel) - 1) * DCInfo.DefClass[Obj.Class].LevelMana + ((Obj.m_PlayerData->Energy - DCInfo.DefClass[Obj.Class].Energy) * DCInfo.DefClass[Obj.Class].EnergyToMana);

		Obj.m_PlayerData->m_iResets += 1;
		gObjNextExpCal(lpObj);
		Obj.m_PlayerData->Money -= g_ConfigRead.data.reset.iCost;
		GSProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);
		gObjCalCharacter.CalcCharacter(Obj.m_Index);
		GJSetCharacterInfo(lpObj, Obj.m_Index, 0);
		GSProtocol.GCLevelUpMsgSend(Obj.m_Index, 1);

		if (g_ConfigRead.data.reset.ResetML == true)
		{
			GSProtocol.GCMasterLevelUpMsgSend(Obj.m_Index);
		}

		if (Obj.Class == CLASS_ELF)
		{
			gObjTeleport(Obj.m_Index, 3, 171, 108);
		}
		else if (Obj.Class == CLASS_SUMMONER)
		{
			gObjTeleport(Obj.m_Index, 51, 51, 224);
		}
		else
		{
			gObjTeleport(Obj.m_Index, 0, 128, 135);
		}

		MsgOutput(Obj.m_Index, Lang.GetText(0, 462), Obj.m_PlayerData->m_iResets);

		if (CharQuit == true || g_ConfigRead.data.reset.MoveToCharacterSelect == true)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 463));
			gObjCloseSet(Obj.m_Index, 1);
		}
	}
	break;
	break;
	case 413:
		g_Kanturu.OperateGmCommand(Obj.m_Index, 0);
		break;
	case 414:
		g_Kanturu.OperateGmCommand(Obj.m_Index, 1);
		break;
	case 415:
		g_Kanturu.OperateGmCommand(Obj.m_Index, 2);
		break;
	case 416:
		if ((Obj.Authority & 2) == 2 || (Obj.Authority & 0x20) == 0x20)
		{
			if ((Obj.GameMaster & GM_COMMAND_INVISIBLE) != GM_COMMAND_INVISIBLE)
			{
				MsgOutput(Obj.m_Index, Lang.GetText(0, 464));
				return 0;
			}

			gObjAddBuffEffect(lpObj, BUFFTYPE_INVISABLE, 0, 0, 0, 0, -10);
			gObjViewportListProtocolDestroy(lpObj);
		}
		break;
	case 417:
		if ((Obj.Authority & 2) == 2 || (Obj.Authority & 0x20) == 0x20)
		{
			if ((Obj.GameMaster & GM_COMMAND_INVISIBLE) != GM_COMMAND_INVISIBLE)
			{
				MsgOutput(Obj.m_Index, Lang.GetText(0, 464));
				return 0;
			}

			gObjRemoveBuffEffect(lpObj, BUFFTYPE_INVISABLE);
			gObjViewportListProtocolCreate(lpObj);
		}
		break;
	case 418:
		for (int i = 0; i < INVENTORY_SIZE; ++i)
		{
			if (Obj.pntInventory[i]->IsItem() == TRUE)
			{
				if (g_PentagramSystem.IsPentagramItem(&Obj.pntInventory[i]) == true)
				{
					for (int j = 0; j < 5; j++)
					{
						if (Obj.pntInventory[i]->m_SocketOption[j] < 0xFE)
						{
							g_PentagramSystem.DelPentagramJewelInfo(Obj.m_Index, 0, Obj.pntInventory[i]->m_SocketOption[j]);
						}
					}

					g_PentagramSystem.GCPentagramJewelInfo(Obj.m_Index, 0);
				}

				gObjInventoryDeleteItem(Obj.m_Index, i);
				GSProtocol.GCInventoryItemDeleteSend(Obj.m_Index, i, 1);
			}

		}
		Obj.m_PlayerData->Money = 0;
		GSProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);
		sLog->outError( "[%s][%s] Cleared inventory", Obj.AccountID, Obj.Name);
		MsgOutput(Obj.m_Index, Lang.GetText(0, 465));
		break;
	case 420:
	{
		if ((Obj.Authority & 2) != 2 && (Obj.Authority & 0x20) != 0x20)
			break;

		if ((Obj.GameMaster & GM_COMMAND_SKIN) != GM_COMMAND_SKIN)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 466));
			return 0;
		}

		int skin = GetTokenNumber();
		if (skin <= 0 && skin > 1024)
		{
			Obj.m_Change = -1;
			gObjViewportListProtocolCreate(lpObj);
			break;
		}


		Obj.m_Change = skin;
		gObjViewportListProtocolCreate(lpObj);
	}
	break;
	case 424:
	{
		if ((Obj.Authority & 2) != 2 && (Obj.Authority & 0x20) != 0x20)
			break;

		if ((Obj.GameMaster & GM_COMMAND_BAN) != GM_COMMAND_BAN)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 467));
			return 0;
		}

		pId = this->GetTokenString();

		if (pId == NULL)
		{
			return 0;
		}

		for each (std::pair<int,CGameObject*> ObjEntry in gGameObjects)
		{
			if (getGameObject(i)->Connected >= PLAYER_LOGGED)
			{
				if (getGameObject(i)->AccountID[0] == pId[0])
				{
					if (getGameObject(i)->AccountID[1] == pId[1])
					{
						if (!strcmp(getGameObject(i)->AccountID, pId))
						{
							GDReqBanUser(i, 1, 1);
							MsgOutput(Obj.m_Index, Lang.GetText(0, 468), pId);
							return 1;
						}
					}
				}
			}
		}
		if (g_ConfigRead.data.common.IsJoinMu)
		{
			BAN_REQ_USER pBan;
			PHeadSetB((BYTE*)&pBan, 0xFA, sizeof(pBan));

			std::memcpy(pBan.AccName, pId, sizeof(pBan.AccName));
			pBan.Ban = 1;
			pBan.Type = 1;

			wsDataCli.DataSend((char *)&pBan, pBan.h.size);
		}
		else
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 469), pId);
		}
	}
	break;
	case 425:
	{
		if ((Obj.Authority & 2) != 2 && (Obj.Authority & 0x20) != 0x20)
			break;

		if ((Obj.GameMaster & GM_COMMAND_BAN) != GM_COMMAND_BAN)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 467));
			return 0;
		}

		pId = this->GetTokenString();

		if (pId == NULL)
		{
			return 0;
		}

		BAN_REQ_USER pBan;
		PHeadSetB((BYTE*)&pBan, 0xFA, sizeof(pBan));

		std::memcpy(pBan.AccName, pId, sizeof(pBan.AccName));
		pBan.Ban = 0;
		pBan.Type = 1;

		wsDataCli.DataSend((char *)&pBan, pBan.h.size);

		MsgOutput(Obj.m_Index, Lang.GetText(0, 470), pId);
	}
	break;
	case 426:
	{
		if ((Obj.Authority & 2) != 2 && (Obj.Authority & 0x20) != 0x20)
			break;

		if ((Obj.GameMaster & GM_COMMAND_BAN) != GM_COMMAND_BAN)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 467));
			return 0;
		}

		pId = this->GetTokenString();

		if (pId == NULL)
		{
			return 0;
		}

		CGameObject lpBan = gObjFind(pId);

		if (lpBan == NULL && !g_ConfigRead.data.common.IsJoinMu)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 471), pId);
			return 0;
		}
		else if (g_ConfigRead.data.common.IsJoinMu && lpBan == NULL)
		{
			BAN_REQ_USER pBan;
			PHeadSetB((BYTE*)&pBan, 0xFA, sizeof(pBan));

			std::memcpy(pBan.AccName, pId, sizeof(pBan.AccName));
			pBan.Ban = 1;
			pBan.Type = 0;

			wsDataCli.DataSend((char *)&pBan, pBan.h.size);
			return 1;
		}

		GDReqBanUser(lpBan->m_Index, 0, 1);
		MsgOutput(Obj.m_Index, Lang.GetText(0, 472), pId);
	}
	break;
	case 427:
	{
		if ((Obj.Authority & 2) != 2 && (Obj.Authority & 0x20) != 0x20)
			break;

		if ((Obj.GameMaster & GM_COMMAND_BAN) != GM_COMMAND_BAN)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 467));
			return 0;
		}

		pId = this->GetTokenString();

		if (pId == NULL)
		{
			return 0;
		}

		BAN_REQ_USER pBan;
		PHeadSetB((BYTE*)&pBan, 0xFA, sizeof(pBan));

		std::memcpy(pBan.AccName, pId, sizeof(pBan.AccName));
		pBan.Ban = 0;
		pBan.Type = 0;

		wsDataCli.DataSend((char *)&pBan, pBan.h.size);

		MsgOutput(Obj.m_Index, Lang.GetText(0, 473), pId);
	}
	break;
	case 428:

		MsgOutput(Obj.m_Index, Lang.GetText(0, 474));

		for each (std::pair<int,CGameObject*> ObjEntry in gGameObjects)
		{
			if (getGameObject(i)->Connected == PLAYER_PLAYING)
			{
				if ((getGameObject(i)->Authority & 2) == 2 || (getGameObject(i)->Authority & 0x20) == 0x20)
				{
					if (getGameObject(i)->GameMaster != 0)
					{
						MsgOutput(Obj.m_Index, "%s", getGameObject(i)->Name);
					}
				}
			}
		}
		break;

	case 431:
	{
		int code = GetTokenNumber();

		if (Obj.m_PlayerData->m_iSecurityCode == code)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 475));
			Obj.m_PlayerData->m_bSecurityCheck = true;
		}

		else
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 476));
		}
	}

	break;

	case 432:
	{
		int code = GetTokenNumber();

		GDReqSecLock(Obj.m_Index, code);
		Obj.m_PlayerData->m_iSecurityCode = code;

		MsgOutput(Obj.m_Index, Lang.GetText(0, 477), code);
	}

	break;
	case 433:
	{
		if (!g_ConfigRead.offtrade.enabled)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 478));
			return 0;
		}

		if (!g_ConfigRead.offtrade.Maps[Obj.MapNumber])
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 479));
			return 0;
		}

		if (g_ConfigRead.server.GetServerType() != SERVER_NORMAL && Obj.MapNumber != MAP_INDEX_LORENMARKET)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 479));
			return 0;
		}

		if (Obj.m_bPShopOpen)
		{
			Obj.m_bOff = true;
			GJReqSetOffTrade(getGameObject(Obj.m_Index));
			MsgOutput(Obj.m_Index, Lang.GetText(0, 480));
			BYTE p[4] = { 0xC1, 0x04, 0xFA, 0x0D };
			IOCP.DataSend(Obj.m_Index, p, sizeof(p));
		}
		else
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 481));
		}

	}
	break;
	case 440:
	{
		char *password;
		pId = this->GetTokenString();
		password = this->GetTokenString();

		if (pId == NULL)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 538));
			return 0;

		}

		CGameObject lpDcOBJ = gObjFind(pId);
		if (lpDcOBJ == NULL)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 482));
			return 0;
		}

		if (password == NULL)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 483));
			return 0;
		}
		if (gObjPasswordCheck(lpDcOBJ->m_Index, password))
		{
			//gObjCloseSet(lpDcOBJ->m_Index,0);
			GSProtocol.GCSendDisableReconnect(lpDcOBJ->m_Index);
			//IOCP.CloseClient(lpDcOBJ->m_Index);
			MsgOutput(Obj.m_Index, Lang.GetText(0, 485));
		}
		else
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 484));
		}

	}
	break;
	case 441:
	{
		if ((Obj.Authority & 2) != 2 && (Obj.Authority & 0x20) != 0x20)
		{
			return 0;
		}

		WORD wCheatMode = this->GetTokenNumber();

		if (wCheatMode < 0)
			wCheatMode = 0;

		g_ImperialGuardian.SetCheatModeGM(wCheatMode);

		switch (wCheatMode)
		{
		case 1:
			GSProtocol.GCServerMsgStringSend("[EventDungeon] CheatMode On", Obj.m_Index, 1);
			break;
		case 2:
			GSProtocol.GCServerMsgStringSend("[EventDungeon] CheatMode On", Obj.m_Index, 1);
			break;
		case 3:
			GSProtocol.GCServerMsgStringSend("[EventDungeon] CheatMode On - LOG START", Obj.m_Index, 1);
			break;
		default:
			GSProtocol.GCServerMsgStringSend("[EventDungeon] CheatMode Off", Obj.m_Index, 1);
			break;
		}
	}
	break;
	case 442:
	{
		if ((Obj.Authority & 2) != 2 && (Obj.Authority & 0x20) != 0x20)
		{
			return 0;
		}

		int nDayOfWeeks = this->GetTokenNumber();

		if (nDayOfWeeks < 0 || nDayOfWeeks > 6)
		{
			break;
		}

		else
		{
			g_ImperialGuardian.SetDayOfWeekGM(nDayOfWeeks);
			GSProtocol.GCServerMsgStringSend("[EventDungeon] SetDayOfWeek Success", Obj.m_Index, 1);
		}
	}
	break;
	case 443:
	{
		if ((Obj.Authority & 2) != 2 && (Obj.Authority & 0x20) != 0x20)
		{
			return 0;
		}

		int nZoneIndex = this->GetTokenNumber();

		if (nZoneIndex < 0 || nZoneIndex > 3)
		{
			nZoneIndex = 0;
		}

		g_ImperialGuardian.WarpZoneGM(Obj.m_Index, nZoneIndex);
	}
	break;
	case 444:
	{
		if ((Obj.Authority & 2) != 2 && (Obj.Authority & 0x20) != 0x20)
		{
			return 0;
		}

		int nTreeType = this->GetTokenNumber();
		g_MasterLevelSkillTreeSystem.ResetMasterSkill(Obj.m_Index, nTreeType);
	}
	break;
	case 445:
	{
		MsgOutput(Obj.m_Index, "HP/MaxHP (%d/%d)", (int)Obj.Life, (int)(Obj.AddLife + Obj.MaxLife));
	}
	break;
	case 446:
	{
		if ((Obj.Authority & 2) != 2 && (Obj.Authority & 0x20) != 0x20)
		{
			return 0;
		}

		if ((Obj.GameMaster & GM_COMMAND_DC_MOVE) != GM_COMMAND_DC_MOVE)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 425));
			return 0;
		}

		pId = this->GetTokenString();

		if (pId == NULL)
		{
			return 0;
		}

		int map;
		int iX;
		int iY;
		CGameObject lpTargetObj = gObjFind(pId);
		int iIndex;

		if (lpTargetObj == NULL)
		{
			return 0;
		}

		sLog->outBasic("Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] / Target : [%s][%s] : %s",
			Obj.m_PlayerData->ConnectUser->IP, Obj.AccountID, Obj.Name, lpTargetObj.AccountID,
			lpTargetObj.Name, "User Trans");

		GMLog->Output("Use GM Command -> [ %s ]\t[ %s ]\t[ %s ] / Target : [%s][%s] : %s",
			Obj.m_PlayerData->ConnectUser->IP, Obj.AccountID, Obj.Name, lpTargetObj.AccountID,
			lpTargetObj.Name, "User Trans");

		map = Obj.MapNumber;
		iX = Obj.X;
		iY = Obj.Y;
		iIndex = lpTargetObj.m_Index;

		int nGMMapNumber = map;
		int nUserMapNumber = lpTargetObj.MapNumber;

		if (iIndex >= 0)
		{
			gObjTeleport(iIndex, map, iX, iY);

			if (nGMMapNumber != nUserMapNumber)
			{
				g_SendNPCInfo.SendNpcCoordinate(iIndex, map, 0);
				g_SendNPCInfo.SendPortalCoordinate(iIndex, map);
			}
		}

		if (map == MAP_INDEX_HATCHERY)
		{
			g_RaklionBattleUserMng.AddUserData(Obj.m_Index);
		}

		if (map == MAP_INDEX_URUK_MOUNTAIN)
		{
			SendLordSilvesterBlockInfo(Obj.m_Index, g_bSilvesterEntranceBlock == true ? 0 : 1);
		}
	}
	break;
	case Command::AutoParty:
	{
		if (g_ConfigRead.data.common.AutoPartySet == 0) 
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 551), m_CommandNames.setparty);
			return 0;
		}
		pId = this->GetTokenString();

		if (pId == NULL)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 552), m_CommandNames.setparty);
			return 0;
		}
		if (strlen(pId) < 4)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 553));
			return 0;
		}
		if (strlen(pId) > 10)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 554));
			return 0;
		}
		if (g_GensSystem.IsMapBattleZone(Obj.MapNumber) == TRUE)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 533));
			return 0;
		}
		std::memcpy(Obj.m_PlayerData->PartyPassword, pId, 10);

		MsgOutput(Obj.m_Index, Lang.GetText(0, 555), Obj.m_PlayerData->PartyPassword);

		Obj.m_PlayerData->bActiveSetParty = true;
	}
	break;
	case 448:
	{
		BYTE TEST[] = { 0xC1, 0x05, 0x49, 0x00 , 0x00 };
		BYTE TEST1[] = { 0xC1, 0x08, 0x4D, 0x18, 0x02, 0x00, 0x00, 0x00 };
		BYTE TEST2[] = { 0xC1, 0x08, 0x3E, 0x03, 0x00, 0x00, 0x00, 0x00 };
		BYTE TEST3[] = { 0xC1, 0x06, 0x3E, 0x11, 0x01, 0x02 };
		IOCP.DataSend(Obj.m_Index, TEST, 5);
		IOCP.DataSend(Obj.m_Index, TEST1, 8);
		IOCP.DataSend(Obj.m_Index, TEST2, 8);
		IOCP.DataSend(Obj.m_Index, TEST3, 6);
	}
	break;
	case 449:
	{
		if (g_OffLevel.b_Enabled == true)
		{
			if (Obj.m_PlayerData->VipType < g_OffLevel.m_General.VipType)
			{
				MsgOutput(Obj.m_Index, Lang.GetText(0, 602), g_OffLevel.m_General.VipType);
				return 0;
			}
			if (g_OffLevel.m_PerMapAttr[Obj.MapNumber].Disable != 1)
			{
				for (int i = 0; i < 5; i++)
				{
					CMagicInf* lpMagic = gObjGetMagicSearch(lpObj, g_OffLevel.m_SkillDef[Obj.Class].SkillNumber[i]);

					if (lpMagic != 0)
					{
						if (!g_OffLevel.ChargePlayer(Obj.m_Index))
						{
							MsgOutput(Obj.m_Index, Lang.GetText(0, 289));
							return 0;
						}

						g_OffLevel.AddUser(Obj.m_Index, lpMagic->m_Skill);
						MsgOutput(Obj.m_Index, Lang.GetText(0, 603), MagicDamageC.m_Name[g_OffLevel.m_SkillDef[Obj.Class].SkillNumber[i]], lpMagic->m_Skill);
						Obj.m_bOffLevel = true;
						Obj.m_bOff = true;
						Obj.m_bPShopOpen = true;
						Obj.m_dwOffLevelTime = GetTickCount64();
						sprintf(Obj.m_szPShopText, Lang.GetText(0, 610));
						GJReqSetOffTrade(getGameObject(Obj.m_Index));
						MsgOutput(Obj.m_Index, Lang.GetText(0, 480));

						BYTE p[4] = { 0xC1, 0x04, 0xFA, 0x0D };
						IOCP.DataSend(Obj.m_Index, p, sizeof(p));
						return 1;
					}
					else
					{
						if (g_OffLevel.m_SkillDef[Obj.Class].SkillNumber[i] > 0)
						{
							MsgOutput(Obj.m_Index, Lang.GetText(0, 604), MagicDamageC.m_Name[g_OffLevel.m_SkillDef[Obj.Class].SkillNumber[i]], g_OffLevel.m_SkillDef[Obj.Class].SkillNumber[i]);
						}
					}
				}
			}
		}
	}
	break;
	case 450: // GM Gift by Gremory Case
	{
		if (Obj.Authority != 0x20)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 417));
			return FALSE;
		}

		if ((Obj.GameMaster & GM_COMMAND_ITEM_CREATE) != GM_COMMAND_ITEM_CREATE)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 418));
			return 0;
		}

		pId = this->GetTokenString();

		if (pId == NULL)
		{
			return 0;
		}

		CGameObject lpTargetObj = gObjFind(pId);

		if (lpTargetObj == NULL)
		{
			return 0;
		}

		int StorageType, ItemType, ItemIndex, ItemLevel, ItemDurability, ItemSkill, ItemLuck, ItemOption, ItemExcOption, ItemSetOption, ItemSocketCount, ItemMainAttribute, MuunEvoItemType, MuunEvoItemIndex, DaysToExpire;

		StorageType = this->GetTokenNumber();
		ItemType = this->GetTokenNumber();
		ItemIndex = this->GetTokenNumber();
		ItemLevel = this->GetTokenNumber();
		ItemDurability = this->GetTokenNumber();
		ItemSkill = this->GetTokenNumber();
		ItemLuck = this->GetTokenNumber();
		ItemOption = this->GetTokenNumber();

		std::string szItemExc;
		char *szString = GetTokenString();

		if (szString == nullptr)
		{
			szItemExc = "-1;-1;-1;-1;-1;-1;-1;-1;-1";
		}

		else
		{
			szItemExc = szString;
		}

		ItemSetOption = this->GetTokenNumber();
		ItemSocketCount = this->GetTokenNumber();
		ItemMainAttribute = this->GetTokenNumber();
		MuunEvoItemType = this->GetTokenNumber();
		MuunEvoItemIndex = this->GetTokenNumber();
		DaysToExpire = this->GetTokenNumber();

		if (StorageType != GC_STORAGE_SERVER && StorageType != GC_STORAGE_CHARACTER)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 641));
			return 0;
		}

		int iType = ItemGetNumberMake(ItemType, ItemIndex);

		if (ItemGetNumberMake(ItemType, ItemIndex) == -1)
		{
			return 0;
		}

		CItemObject Item;
		Item.m_Type = iType;

		ITEM_ATTRIBUTE * p = &ItemAttribute[iType];

		BYTE btExcOptionList[9];
		memset(btExcOptionList, -1, sizeof(btExcOptionList));

		std::deque<int> exc_opt_list;
		strtk::parse(szItemExc, ";", exc_opt_list);

		for (int i = 0; i < exc_opt_list.size(); i++)
		{
			btExcOptionList[i] = exc_opt_list.at(i);
		}

		int iExcOption = 0;

		if (btExcOptionList[0] == (BYTE)-2)
		{
			if (p->ItemKindA == ITEM_KIND_A_WING)
			{
				iExcOption = g_ItemOptionTypeMng.WingExcOptionRand(p->ItemKindA, p->ItemKindB, Item.m_SocketOption);
			}

			else
			{
				iExcOption = g_ItemOptionTypeMng.CommonExcOptionRand(p->ItemKindA, Item.m_SocketOption);
			}
		}

		else if (btExcOptionList[0] == (BYTE)-3)
		{
			if (p->ItemKindA == ITEM_KIND_A_WING)
			{
				iExcOption = g_ItemOptionTypeMng.WingExcOptionGetCount(p->ItemKindA, p->ItemKindB, Item.m_SocketOption, btExcOptionList[1]);
			}

			else
			{
				iExcOption = g_ItemOptionTypeMng.CommonExcOptionGetCount(p->ItemKindA, Item.m_SocketOption, btExcOptionList[1]);
			}
		}

		else
		{
			for (int i = 0; i < 9; i++)
			{
				if (btExcOptionList[i] == -1)
				{
					continue;
				}

				if (btExcOptionList[i] >= 6)
				{
					bool bOptionFound = false;

					for (int j = 0; j < 5; j++)
					{
						if (Item.m_SocketOption[j] == btExcOptionList[i])
						{
							bOptionFound = true;
							break;
						}
					}

					if (bOptionFound == true)
					{
						continue;
					}

					for (int j = 0; j < 5; j++)
					{
						if (Item.m_SocketOption[j] == 0xFF)
						{
							Item.m_SocketOption[j] = btExcOptionList[i];
							break;
						}
					}
				}

				else
				{
					iExcOption |= (1 << (5 - btExcOptionList[i]));
				}
			}
		}

		if (gSocketItemType.CheckSocketItemType(iType) == true)
		{
			if (ItemSocketCount == 0)
			{
				g_SocketOptionSystem.MakeSocketSlot(&Item, rand() % 5 + 1);
			}

			else
			{
				g_SocketOptionSystem.MakeSocketSlot(&Item, ItemSocketCount);
			}

			ItemMainAttribute = -1;
		}

		else if (g_PentagramSystem.IsPentagramItem(Item.m_Type) == true)
		{
			if (ItemSocketCount == 0)
			{
				g_PentagramSystem.GetMakePentagramSlotCountNKind(Item.m_SocketOption, Item.m_Type);
			}

			else
			{
				BYTE Slot[5] = { 0, 0, 0, 0, 0 };

				for (int i = 0; i < ItemSocketCount; i++)
				{
					Slot[i] = 1;
				}

				g_PentagramSystem.MakePentagramSocketSlot(&Item, Slot[0], Slot[1], Slot[2], Slot[3], Slot[4]);
			}
		}

		else if (g_CMuunSystem.IsStoneofEvolution(Item.m_Type) == TRUE)
		{
			int iMuunEvolutionItemID = ITEMGET(MuunEvoItemType, MuunEvoItemIndex);

			Item.m_SocketOption[0] = iMuunEvolutionItemID >> 8;
			Item.m_SocketOption[1] = ITEM_GET_INDEX(iMuunEvolutionItemID);
		}

		if (DaysToExpire < 1)
		{
			DaysToExpire = 1;
		}

		_stGremoryCaseItem stItem;
		stItem.btStorageType = StorageType;
		stItem.btRewardSource = GC_REWARD_GM_REWARD;
		stItem.wItemID = ItemGetNumberMake(ItemType, ItemIndex);
		stItem.btItemLevel = ItemLevel;
		stItem.btItemDurability = ItemDurability;
		stItem.btItemSkill = ItemSkill;
		stItem.btItemLuck = ItemLuck;
		stItem.btItemOption = ItemOption;
		stItem.btItemExcOption = iExcOption;
		stItem.btItemSetOption = ItemSetOption;
		std::memcpy(stItem.btItemSocketOption, Item.m_SocketOption, 5);
		stItem.btItemMainAttribute = ItemMainAttribute;
		stItem.wMuunEvoItemType = MuunEvoItemType;
		stItem.btMuunEvoItemIndex = MuunEvoItemIndex;

		g_GremoryCase.GDReqAddItemToGremoryCase(lpTargetObj.m_Index, stItem, DaysToExpire);

		sLog->outBasic("[%s][%s][%s] Added GM Gift to Gremory Case using Admin Command (%s/%d/%d) Auth: %d Target: %s", Obj.AccountID, Obj.Name, Obj.m_PlayerData->ConnectUser->IP, Lang.GetMap(0, Obj.MapNumber), Obj.X, Obj.Y, Obj.Authority, lpTargetObj.Name);
		GMLog->Output("[%s][%s][%s] Created Added GM Gift to Gremory Case using Admin Command (%s/%d/%d) Auth: %d Target: %s", Obj.AccountID, Obj.Name, Obj.m_PlayerData->ConnectUser->IP, Lang.GetMap(0, Obj.MapNumber), Obj.X, Obj.Y, Obj.Authority, lpTargetObj.Name);
	}
	break;
	case 451:
	{
		if ((Obj.Authority & 2) != 2 && (Obj.Authority & 0x20) != 0x20)
		{
			return 0;
		}

		if (g_OffLevel.b_Enabled == true)
		{
			if (Obj.m_PlayerData->VipType < g_OffLevel.m_General.VipType)
			{
				MsgOutput(Obj.m_Index, Lang.GetText(0, 602), g_OffLevel.m_General.VipType);
				return 0;
			}

			if (g_OffLevel.m_PerMapAttr[Obj.MapNumber].Disable != 1)
			{
				for (int i = 0; i < 5; i++)
				{
					CMagicInf* lpMagic = gObjGetMagicSearch(lpObj, g_OffLevel.m_SkillDef[Obj.Class].SkillNumber[i]);

					// Check SafeZone
					BYTE btAttr = MapC[Obj.MapNumber].GetAttr(Obj.X, Obj.Y);

					if ((btAttr & 1) == 1)
					{
						MsgOutput(Obj.m_Index, Lang.GetText(0, 635));
						return FALSE;
					}

					// Check Magic
					if (lpMagic != 0)
					{
						g_OffLevel.AddUser(Obj.m_Index, lpMagic->m_Skill);
						MsgOutput(Obj.m_Index, Lang.GetText(0, 603), MagicDamageC.m_Name[g_OffLevel.m_SkillDef[Obj.Class].SkillNumber[i]], lpMagic->m_Skill);
						Obj.m_bOffLevel = true;
						Obj.m_bOff = true;
						Obj.m_bPShopOpen = true;
						sprintf(Obj.m_szPShopText, Lang.GetText(0, 610));
						GJReqSetOffTrade(getGameObject(Obj.m_Index));
						MsgOutput(Obj.m_Index, Lang.GetText(0, 480));
						return 1;
					}
					else
					{
						if (g_OffLevel.m_SkillDef[Obj.Class].SkillNumber[i] > 0)
						{
							MsgOutput(Obj.m_Index, Lang.GetText(0, 604), MagicDamageC.m_Name[g_OffLevel.m_SkillDef[Obj.Class].SkillNumber[i]], g_OffLevel.m_SkillDef[Obj.Class].SkillNumber[i]);
						}
					}
				}
			}
		}
	}
	break;
	case 452:
	{
		if ((Obj.Authority & 2) != 2 && (Obj.Authority & 0x20) != 0x20)
		{
			return 0;
		}

		int iBlow = this->GetTokenNumber();

		if (iBlow != 0 && iBlow != 1)
		{
			MsgOutput(Obj.m_Index, "[CCS-Debug] Blow Switch values: 0/1");
			return 0;
		}

		g_ChaosCastleFinal.m_bCCF_CHEAT_BLOW = iBlow;
		sLog->outBasic("[ChaosCastleFinal][CHEAT][CCF BLOW] [%s]Changed m_bCCF_CHEAT_BLOW:%d ", Obj.Name, g_ChaosCastleFinal.m_bCCF_CHEAT_BLOW);
	}
	break;
	case 453:
	{
		if ((Obj.Authority & 2) != 2 && (Obj.Authority & 0x20) != 0x20)
		{
			return 0;
		}

		int iTrap = this->GetTokenNumber();

		if (iTrap != 0 && iTrap != 1)
		{
			MsgOutput(Obj.m_Index, "[CCS-Debug] Trap Switch values: 0/1");
			return 0;
		}

		g_ChaosCastleFinal.m_bCCF_CHEAT_TRAP = iTrap;
		sLog->outBasic("[ChaosCastleFinal][CHEAT][CCF BLOW] [%s]Changed m_bCCF_CHEAT_TRAP:%d ", Obj.Name, g_ChaosCastleFinal.m_bCCF_CHEAT_TRAP);
	}
	break;
	case 454:
	{
		if ((Obj.Authority & 2) != 2 && (Obj.Authority & 0x20) != 0x20)
		{
			return 0;
		}

		int iDay = this->GetTokenNumber();

		g_QuestExpProgMng.SetCheatChangeDay(iDay);
		MsgOutput(Obj.m_Index, "QuestExp Cheat Day:%d", g_QuestExpProgMng.GetCheatChangeDay());
	}
	break;

	case 455:
	{
		int iResult = g_ChangeCmd.DoChange(Obj.m_Index);

		if (iResult == -1)
		{
			return 0;
		}

		if (iResult == 0)
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 639));
		}

		else
		{
			MsgOutput(Obj.m_Index, Lang.GetText(0, 640), iResult);
		}
	}
	break;

	case 456:
	{
		int iResult = g_UpgradeCmd.DoUpgrade(Obj.m_Index);

		if (iResult == -1)
		{
			return 0;
		}

		if (iResult == 0)
		{
			MsgOutput(Obj.m_Index, "No upgrade has been made!");
		}

		else
		{
			MsgOutput(Obj.m_Index, "Item Successfully upgraded!");
		}
	}
	break;
	case Command::PartyLeader:
	{
		char * TargetName = this->GetTokenString();
		int TargetIndex = gObjGetIndex(TargetName);
		// ----
		if (!gParty.SetLeader(Obj.m_Index, TargetIndex))
		{
			return false;
		}
	}
	break;
	case Command::ResetTable:
		this->CommandReset(lpObj);
		break;
	case Command::NewMakeItem:
	{
		if ((Obj.Authority & 2) != 2 && (Obj.Authority & 0x20) != 0x20)
		{
			return 0;
		}

		int section = this->GetTokenNumber();
		int type = this->GetTokenNumber();
		int level = this->GetTokenNumber();
		int skill = this->GetTokenNumber();
		int luck = this->GetTokenNumber();
		int option = this->GetTokenNumber();
		int exc = this->GetTokenNumber();
		int qnt = this->GetTokenNumber();
		int setopt = this->GetTokenNumber();

		if (qnt <= 0) qnt = 1;

		CommandMake(lpObj, section, type, level, skill, luck, option, exc, qnt, setopt);
	}
	break;
	case Command::ClearInventory:
		this->CommandClearInventory(lpObj);
	break;
	case Command::MakeRandomSet:
		this->CommandMakeRandomSet(lpObj);
	break;

	}
	return 0;
}

void CGMMng::CommandMake(CGameObject &Obj, int qnt, int section, int type, int level, int skill, int luck, int option, int exc, int SetOpt)
{

	for (int i = 0; i < qnt; i++)
	{
		ItemCreate(Obj.m_Index, 235, 0, 0, ((section * 512) + type), level, 0, skill, luck, option, -1, exc, SetOpt, 0, 0, 0);
	}

	sLog->outBasic("[GM][%s]MakeItem:[Cat:%d Index:%d Level:%d Skill:%d Luck:%d Option:%d Excellent:%d Ancient:%d]", Obj.Name, section, type, level, skill, luck, option, exc, SetOpt);
}

bool CGMMng::CommandMakeRandomSet(CGameObject &Obj)
{
	if ((Obj.Authority & 2) != 2 && (Obj.Authority & 0x20) != 0x20)
	{
		return 0;
	}

	int Count = this->GetTokenNumber();
	// ----
	if (Count == -1)
	{
		MsgOutput(Obj.m_Index, "Syntax error in command");
		return 0;
	}
	// ----
	for (int i = 0; i < Count; i++)
	{
		MakeRandomSetItem(Obj.m_Index);
	}
}

bool CGMMng::CommandClearInventory(CGameObject &Obj)
{
	if ((Obj.Authority & 2) != 2 && (Obj.Authority & 0x20) != 0x20)
	{
		return 0;
	}

	LPSTR UserName = this->GetTokenString();
	// ----
	if (UserName == 0)
	{
		MsgOutput(Obj.m_Index, "Syntax error in command");
		return 0;
	}
	// ----
	CGameObject lpTarget = this->GetUserInfo(lpObj, UserName);
	// ----
	if (!lpTarget)
	{
		return 0;
	}
	// ----
	for (int i = INVETORY_WEAR_SIZE; i < MAIN_INVENTORY_SIZE; i++)
	{
		if (lpTarget->pntInventory[i]->IsItem())
		{
			gObjInventoryDeleteItem(lpTarget->m_Index, i);
		}
	}
	// ----
	GCItemObjectListSend(lpTarget->m_Index);
}

bool CGMMng::CommandReset(CGameObject &Obj)
{
	sLog->outBasic("TABLE_RESET", ".\\TABLE_RESET", 1);

	int point = 0;

	TNotice pNotice(1);

	if (g_ConfigRead.ResetSwitch == 0)
	{
		pNotice.SendToUser(Obj.m_Index, g_ConfigRead.ResetMsg[0]);
		return false;
	}

	if (Obj.Level < g_ConfigRead.ResetLevel)
	{
		pNotice.SendToUser(Obj.m_Index, g_ConfigRead.ResetMsg[1], g_ConfigRead.ResetLevel);
		return false;
	}

	int ResetFee = g_ConfigRead.ResetMoney;
	if (g_ConfigRead.ResetMoneyType == 1)
	{
		ResetFee *= Obj.m_PlayerData->m_iResets + g_ConfigRead.ResetMultiply;

		if (ResetFee > MAX_ZEN)
			ResetFee = MAX_ZEN;
	}

	if (ResetFee > 0)
	{
		if (Obj.m_PlayerData->Money < ResetFee)
		{
			pNotice.SendToUser(Obj.m_Index, g_ConfigRead.ResetMsg[2], ResetFee);
			return false;
		}
	}

	if (Obj.m_PlayerData->m_iResets >= g_ConfigRead.ResetLimit)
	{
		pNotice.SendToUser(Obj.m_Index, g_ConfigRead.ResetMsg[3], g_ConfigRead.ResetLimit);
		return false;
	}

	if (g_ConfigRead.ResetCheckItem != 0)
	{
		for (int n = 0; n < INVETORY_WEAR_SIZE; n++)
		{
			if (Obj.pntInventory[n]->IsItem() != 0)
			{
				pNotice.SendToUser(Obj.m_Index, g_ConfigRead.ResetMsg[4]);
				return false;
			}
		}
	}

	if (g_ConfigRead.ResetMultiply > 0)
		Obj.m_PlayerData->m_iResets += g_ConfigRead.ResetMultiply;

	if (ResetFee > 0)
	{
		Obj.m_PlayerData->Money -= ResetFee;
		GSProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);
	}

	Obj.Level = g_ConfigRead.ResetStartLevel;
	Obj.m_PlayerData->Experience = gLevelExperience[Obj.Level - 1];

	if (g_ConfigRead.ResetType == 2 || g_ConfigRead.ResetType == 0)
	{
		point = gResetTable.GetResetPoint(lpObj);
	}
	else if (g_ConfigRead.ResetType == 1)
	{
		point = g_ConfigRead.ResetPoint * Obj.m_PlayerData->m_iResets;
	}

	if (g_ConfigRead.ResetPointRate[Obj.Class] != 100)
	{
		point = (point*g_ConfigRead.ResetPointRate[Obj.Class]) / 100;
	}
	
	if (g_ConfigRead.ResetType == 0)
		Obj.m_PlayerData->LevelUpPoint += point;
	else
		Obj.m_PlayerData->LevelUpPoint = point;
	
	if (g_ConfigRead.ResetType == 1 || g_ConfigRead.ResetType == 2)
	{
		Obj.m_PlayerData->Strength = DCInfo.DefClass[Obj.Class].Strength;
		Obj.m_PlayerData->Dexterity = DCInfo.DefClass[Obj.Class].Dexterity;
		Obj.m_PlayerData->Vitality = DCInfo.DefClass[Obj.Class].Vitality;
		Obj.m_PlayerData->Energy = DCInfo.DefClass[Obj.Class].Energy;

		if (g_ConfigRead.ResetLeadership == 1)
			Obj.Leadership = DCInfo.DefClass[Obj.Class].Leadership;
	}

	char BonusText[80];
	strcpy(BonusText, g_ConfigRead.ResetMsg[7]);

	char BonusTemp[50];

	if (g_ConfigRead.ResetBonusGoblinPoint > 0)
	{
		sprintf(BonusTemp, g_ConfigRead.ResetMsg[9], g_ConfigRead.ResetBonusGoblinPoint);
		strcat(BonusText, BonusTemp);

		//Obj.m_PlayerData->m_GoblinPoint += g_ConfigRead.ResetBonusGoblinPoint;
		GDReqInGameShopPointAdd(Obj.m_Index, 2, g_ConfigRead.ResetBonusGoblinPoint);
	}

	if (g_ConfigRead.ResetBonusWCoinC > 0)
	{
		sprintf(BonusTemp, g_ConfigRead.ResetMsg[11], g_ConfigRead.ResetBonusWCoinC);
		strcat(BonusText, BonusTemp);

		//Obj.m_PlayerData->m_WCoinC += g_ConfigRead.ResetBonusWCoinC;
		GDReqInGameShopPointAdd(Obj.m_Index, 0, g_ConfigRead.ResetBonusWCoinC);
	}

	if (g_ConfigRead.ResetSwitchReward == 1)
	{
		g_BagManager.UseBag_GremoryCase(Obj.m_Index, BAG_EVENT, EVENTBAG_RESETREWARD, Obj.m_Index, GC_STORAGE_CHARACTER, GC_REWARD_GM_REWARD, 30);
	}

	Obj.MaxLife = DCInfo.DefClass[Obj.Class].Life + ((Obj.Level - 1) * DCInfo.DefClass[Obj.Class].LevelLife) + ((Obj.m_PlayerData->Vitality - DCInfo.DefClass[Obj.Class].Vitality) * DCInfo.DefClass[Obj.Class].VitalityToLife);

	Obj.MaxMana = DCInfo.DefClass[Obj.Class].Mana + ((Obj.Level - 1) * DCInfo.DefClass[Obj.Class].LevelMana) + ((Obj.m_PlayerData->Energy - DCInfo.DefClass[Obj.Class].Energy) * DCInfo.DefClass[Obj.Class].EnergyToMana);

	gObjCalCharacter.CalcCharacter(Obj.m_Index);

	GJSetCharacterInfo(lpObj, Obj.m_Index, 0);
	GSProtocol.GCLevelUpMsgSend(Obj.m_Index, 1);
	g_StatSpec.CalcStatOption(lpObj, STAT_OPTION_INC_ATTACK_POWER); 
	g_StatSpec.CalcStatOption(lpObj, STAT_OPTION_INC_MAGIC_DAMAGE);
	g_StatSpec.CalcStatOption(lpObj, STAT_OPTION_INC_CURSE_DAMAGE);
	g_StatSpec.CalcStatOption(lpObj, STAT_OPTION_INC_ATTACK_SPEED);
	g_StatSpec.CalcStatOption(lpObj, STAT_OPTION_INC_ATTACK_RATE);
	g_StatSpec.CalcStatOption(lpObj, STAT_OPTION_INC_ATTACK_RATE_PVP);
	g_StatSpec.CalcStatOption(lpObj, STAT_OPTION_INC_DEFENSE_RATE);
	g_StatSpec.CalcStatOption(lpObj, STAT_OPTION_INC_DEFENSE_RATE_PVP);
	g_StatSpec.CalcStatOption(lpObj, STAT_OPTION_INC_DEFENSE);

	pNotice.SendToUser(Obj.m_Index, g_ConfigRead.ResetMsg[5], Obj.m_PlayerData->m_iResets, point);

	if (strlen(BonusText) > strlen(g_ConfigRead.ResetMsg[7]))
		pNotice.SendToUser(Obj.m_Index, BonusText);

	if (g_ConfigRead.WriteResetLog == 1)
		g_ResetLog.Output("[%s][%s] VIP: %d | Type: %d | Resets: %d | Points: %d", Obj.AccountID, Obj.Name, Obj.m_PlayerData->VipType, g_ConfigRead.ResetType, Obj.m_PlayerData->m_iResets, point);

	if (g_ConfigRead.ResetMove != 0)
	{
		Obj.MapNumber = g_ConfigRead.StartingMap[Obj.Class];
		MapC[Obj.MapNumber].GetMapPos(Obj.MapNumber, Obj.X, Obj.Y);
		gObjTeleport(Obj.m_Index, Obj.MapNumber, Obj.X, Obj.Y);
	}

	return true;
}

void CGMMng::GetInfinityArrowMPConsumption(CGameObject &Obj)
{
	MsgOutput(Obj.m_Index, Lang.GetText(0, 486),
		g_SkillAdditionInfo.GetInfinityArrowMPConsumptionPlus0(),
		g_SkillAdditionInfo.GetInfinityArrowMPConsumptionPlus1(),
		g_SkillAdditionInfo.GetInfinityArrowMPConsumptionPlus2());
}



void CGMMng::ControlInfinityArrowMPConsumption0(CGameObject &Obj, int iValue)
{
	g_SkillAdditionInfo.SetInfinityArrowMPConsumptionPlus0(iValue);
	MsgOutput(Obj.m_Index, Lang.GetText(0, 487), iValue);

}

void CGMMng::ControlInfinityArrowMPConsumption1(CGameObject &Obj, int iValue)
{
	g_SkillAdditionInfo.SetInfinityArrowMPConsumptionPlus1(iValue);
	MsgOutput(Obj.m_Index, Lang.GetText(0, 488), iValue);

}

void CGMMng::ControlInfinityArrowMPConsumption2(CGameObject &Obj, int iValue)
{
	g_SkillAdditionInfo.SetInfinityArrowMPConsumptionPlus2(iValue);
	MsgOutput(Obj.m_Index, Lang.GetText(0, 489), iValue);

}

void CGMMng::SetInfinityArrowTime(CGameObject &Obj, int iValue)
{
	if (Obj.Class == CLASS_ELF && Obj.Type == OBJ_USER && Obj.m_PlayerData->ChangeUP >= 1)
	{
		gObjAddBuffEffect(lpObj, BUFFTYPE_INFINITY_ARROW, 0, 0, 0, 0, iValue);
		MsgOutput(Obj.m_Index, Lang.GetText(0, 490), iValue);
	}
	else
	{
		MsgOutput(Obj.m_Index, Lang.GetText(0, 491));
	}
}


void CGMMng::ControlFireScreamDoubleAttackDistance(CGameObject &Obj, int iValue)
{
	g_SkillAdditionInfo.SetFireScreamExplosionAttackDistance(iValue);
	MsgOutput(Obj.m_Index, Lang.GetText(0, 492), iValue);
}

void ServerMsgSend(CGameObject &Obj, int Type, char Sender[20], const char*Message, ...)
{
	PMSG_CHATDATA_WHISPER pMsg;

	/*char szBuffer[1024];
	va_list pArguments;
	va_start(pArguments,Message);
	vsprintf(szBuffer,Message,pArguments);
	va_end(pArguments);
	BYTE *Packet = new BYTE[200];
	memset(Packet,0x00,200);
	*Packet = 0xC1;
	if(Type)*(Packet+2)=0x02;
	else *(Packet+2)=0x00;
	std::memcpy((Packet+3),Sender,strlen(Sender));
	std::memcpy((Packet+13),szBuffer,strlen(szBuffer));
	int Len = (strlen(szBuffer)+0x13);
	*(Packet+1)=Len;
	if(!lpObj)GSProtocol.DataSendAll(Packet,Len);
	else
		if(Obj.Connected)
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index,Packet,Len);
	delete [] Packet;*/
}

BOOL CGMMng::CheckTraceMarryCondition(CGameObject &Obj, CGameObject lpTargetObj)
{
	if (Obj.Married == 0)
	{
		return FALSE;
	}

	if (strlen(Obj.MarryName) <= 0)
	{
		return FALSE;
	}

	if (Obj.Level + Obj.m_PlayerData->MasterLevel < 200)
	{
		return FALSE;
	}

	if (lpTargetObj.Level + lpTargetObj.m_PlayerData->MasterLevel < 200)
	{
		return FALSE;
	}

	if (BC_MAP_RANGE(lpTargetObj.MapNumber) || DS_MAP_RANGE(lpTargetObj.MapNumber) || CC_MAP_RANGE(lpTargetObj.MapNumber) || IT_MAP_RANGE(lpTargetObj.MapNumber) ||
		DG_MAP_RANGE(lpTargetObj.MapNumber) || IMPERIAL_MAP_RANGE(lpTargetObj.MapNumber) || CRYWOLF_MAP_RANGE(lpTargetObj.MapNumber) || lpTargetObj.MapNumber == MAP_INDEX_CASTLESIEGE ||
		lpTargetObj.MapNumber == MAP_INDEX_CASTLEHUNTZONE || g_GensSystem.IsMapBattleZone(lpTargetObj.MapNumber) || lpTargetObj.MapNumber == MAP_INDEX_KANTURU_BOSS || lpTargetObj.MapNumber == MAP_INDEX_KALIMA1
		|| lpTargetObj.MapNumber == MAP_INDEX_KALIMA2 || lpTargetObj.MapNumber == MAP_INDEX_KALIMA3 || lpTargetObj.MapNumber == MAP_INDEX_KALIMA4 || lpTargetObj.MapNumber == MAP_INDEX_KALIMA5 || lpTargetObj.MapNumber == MAP_INDEX_KALIMA6
		|| lpTargetObj.MapNumber == MAP_INDEX_KALIMA7 || lpTargetObj.MapNumber == MAP_INDEX_ARCA_WAR || lpTargetObj.MapNumber == MAP_INDEX_DEBENTER_ARCA_WAR)
	{
		return FALSE;
	}

	LPMOVE_COMMAND_DATA lpMoveData = gMoveCommand.GetMoveCommandDataByMapNumber(lpTargetObj.MapNumber);

	if (lpMoveData == NULL)
	{
		return FALSE;
	}

	if (lpMoveData->NeedLevel > Obj.Level + Obj.m_PlayerData->MasterLevel)
	{
		MsgOutput(Obj.m_Index, Lang.GetText(0, 612));
		return FALSE;
	}

	if (lpTargetObj.MapNumber == MAP_INDEX_ICARUS)
	{
		if ((Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type != ITEMGET(13, 3) &&
			Obj.pntInventory[7]->m_Type != ITEMGET(13, 30) &&
			Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type != ITEMGET(13, 37) &&
			Obj.pntInventory[7]->m_Type != ITEMGET(12, 36) &&//Season 4.5 addon
			Obj.pntInventory[7]->m_Type != ITEMGET(12, 37) &&//Season 4.5 addon
			Obj.pntInventory[7]->m_Type != ITEMGET(12, 38) &&//Season 4.5 addon
			Obj.pntInventory[7]->m_Type != ITEMGET(12, 39) &&//Season 4.5 addon
			Obj.pntInventory[7]->m_Type != ITEMGET(12, 40) &&//Season 4.5 addon
			Obj.pntInventory[7]->m_Type != ITEMGET(12, 41) &&//Season 4.5 addon
			Obj.pntInventory[7]->m_Type != ITEMGET(12, 42) &&//Season 4.5 addon
			Obj.pntInventory[7]->m_Type != ITEMGET(12, 43) &&//Season 4.5 addon
			Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type != ITEMGET(13, 4) &&//Season 4.5 addon
			!(Obj.pntInventory[7]->m_Type >= ITEMGET(12, 262) && Obj.pntInventory[7]->m_Type <= ITEMGET(12, 265)) &&
			!(Obj.pntInventory[7]->m_Type >= ITEMGET(12, 130) && Obj.pntInventory[7]->m_Type <= ITEMGET(12, 135)) &&
			(Obj.pntInventory[7]->m_Type < ITEMGET(12, 0) || Obj.pntInventory[7]->m_Type > ITEMGET(12, 6))) ||
			Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 2) ||
			Obj.pntInventory[11]->m_Type == ITEMGET(13, 10) || Obj.pntInventory[10]->m_Type == ITEMGET(13, 10))
		{
			return FALSE;
		}
	}

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

	if (bPlayerKiller == TRUE)
	{
		return FALSE;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

