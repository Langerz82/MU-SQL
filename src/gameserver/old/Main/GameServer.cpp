//------------------------------------------
// GameServer.cpp
// Webzen Mu Game Server
// Date: 02-02-2007
// Decompilation by: Deathway
// Description : Main entry of the Program, 
//				 Start all servers
//------------------------------------------
//	GS-N	1.00.18	JPN	0x0052D520	-	Completed
#include "StdAfx.h"
#include "User/CUserData.h"
#include "Lang.h"
#include "resource.h"
#include "GameServer.h"
#include "GameMain.h"
#include "IOCP.h"
#include "Logging/Log.h"
#include "RingAttackEvent.h"

#include "Authorization.h"
#include "SProtocol.h"
#include "ChaosCastle.h"
#include "Protocol.h"
#include "MapServerManager.h"
#include "Shop.h"
#include "EledoradoEvent.h"
#include "EventManagement.h"
#include "CashShop.h"
#include "configread.h"
#include "TServerInfoDisplayer.h"
#include "DevilSquare.h"
#include "CItemObjectDrop.h"
//Added by HermeX
#include "mdump.h"
#include "NewsSystem.h"
#include "ItemSocketOptionSystem.h"
#include "ItemAddOption.h"
#include "SetItemOption.h"
#include "MoveCommand.h"
#include "QuestInfo.h"
#include "AntiSwear.h"
#include "SetItemDrop.h"
#include "PentagramSystem.h"
#include "PentagramMixSystem.h"
#include "VipSys.h"
#include "QuestExpLuaBind.h"
#include "LuaFun.h"
#include "MapAttribute.h"
#include "BagManager.h"
#include "MathAuthenticator.h"
#include "ProhibitedSymbols.h"
#include "ExpManager.h"
#include "IllusionTempleEvent_Renewal.h"
#include "OfflineLevelling.h"
#include "MuTimer.h"
#include "LastManStanding.h"
#include "PvPBalance.h"
#include "ItemDropEx.h"
#include "./Custom/CustomEventDrop/CustomEventDrop.h"
#include "FloodSystem.h"
#include "UserDieItemDrop.h"
#include "Item_Bags.h"
#include "ObjUseSkill.h"
#include "MasterLevelSkillTreeSystem.h"
#include "ObjCalCharacter.h"
#include "StatSpecialize.h"
#include "./Custom/CustomItemValue/CustomItemValue.h"
#include "ItemOptionRate.h"
#include "CustomMaxStats.h"
#include "PVPZone.h"
#include "ItemUpgradeJewel.h"
#include "CustomMichi.h"

int gServerReady;
int gCloseMsg;
int gCloseMsgTime;
int gCurPaintType;
int gCurPaintMapNumber;

CRingAttackEvent g_RingAttackEvent;
CXMasAttackEvent g_XMasAttackEvent;

HINSTANCE hInst; // Current Instance
HWND ghWnd, ghStatusBar;
HANDLE ghConnServThread;

char szTitle[100];
char szWindowClass[64];

BOOL GameServerStart(void);
int InitInstance(HINSTANCE, int);	// Init GameServer
ATOM MyRegisterClass(HINSTANCE);	// Register Window Class
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);// Brain of the GameServer
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK GSDisconnect(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK AuthWindow(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void UpdateInfoBar(HWND hWnd);
//-------------------------------------------------------------

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;
	CMiniDump::Begin();
	hInst=hInstance;
	
	CLEAR_START

	//Load Title and WindowClass Name
	LoadString(hInstance,IDS_APP_TITLE, szTitle, 100);
	LoadString(hInstance,IDS_GAMESERVER, szWindowClass, 100);

	MyRegisterClass(hInstance);

	InitCommonControls();

	CLEAR_END
	
	// Check if error ocurs
	if (!InitInstance(hInstance, nCmdShow))
	{
		return 0;
	}

	CLEAR_START

	ShowWindow(ghWnd, SW_NORMAL);// Showm GameServer Window

	hAccelTable=LoadAccelerators(hInstance,(LPCSTR)IDA_GAMESERVER);// Load Accelerators
	IOCP.GiocpInit();
	GameMainInit(ghWnd);

	lua_State * L = g_MuLuaQuestExp.GetLua();

	CLEAR_END

	if (!L)
	{
		MessageBox(0, "Lua initalization failed.", "Lua Error", MB_OK | MB_ICONERROR|MB_TOPMOST);
		ExitProcess(1);
	}

	Debug_AddDebugPrintTemp(L);

#ifdef GS_AUTHORITY
	CLEAR_START
	g_AuthSys.hConnThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)CAuthSystem::AuthConnectionProcess, &g_AuthSys, NULL, &g_AuthSys.dwConnThreadID);
	CLEAR_END
#else
	//g_AuthSys.SetAddons(1);
	AllServerStart();
#endif
		
	// Main Message Loop
	while(GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd,hAccelTable,&msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	CMiniDump::End();
	return msg.wParam;
}

		
BOOL AllServerStart(void){
	
	if (JoinServerConnected==0)
	{
		if (GMJoinServerConnect(g_ConfigRead.server.GetJoinServerIP(), WM_GM_JOIN_CLIENT_MSG_PROC) == 0)
		{	
			sLog->outError("JoinServer Connect fail");
			return 0;
		}
	}
	
	if (DataServerConnected==0)
	{
		if (GMDataServerConnect(g_ConfigRead.server.GetDataServerIP(), WM_GM_DATA_CLIENT_MSG_PROC) == 0)
		{
			sLog->outError("DataServer connect fail");
			return 0;
		}

		if (ExDataServerConnect(g_ConfigRead.server.GetExDBIP(), WM_GM_EXDATA_CLIENT_MSG_PROC) == 0)
		{
			sLog->outError("DataServer connect fail");
			return 0;
		}
	}

	return TRUE;
}

BOOL GameServerStart(void) 
{
	//VM_TIGER_LONDON_START

	if (GameMainServerCreate(WM_GM_SERVER_MSG_PROC, WM_GM_CLIENT_MSG_PROC) == FALSE )
	{
		sLog->outError("GameServer failed to run");
		return FALSE;
	}

	IOCP.CreateGIocp(g_ConfigRead.server.GetGameServerPort());

	sLog->outBasic("Server version: %s Compiled at: %s %s", GAMESERVER_VERSION, __DATE__, __TIME__);
	sLog->outBasic("Created and developed by IGC-Network, released by MuEMU. ");
	sLog->outBasic("// Powered by vnDev.games - Trong.LIVE - Dao Van Trong //");

	SetTimer(ghWnd, WM_LOG_PAINT, 1000, NULL);
	SetTimer(ghWnd, WM_CONNECT_DATASERVER, 10000, NULL);
	
	g_MuTimer.SetMuTimer();

	GSInfoSendFlag = TRUE;

	//VM_TIGER_LONDON_END

	return TRUE;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_NORMAL_V1);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDR_MENU;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_NORMAL_V1);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

	HWND hWnd;

	CLEAR_START

    hWnd = CreateWindowExA(
        0,
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
		NULL, NULL, hInstance, NULL);
	/*
		hWnd = CreateWindowExA(0,szWindowClass,szTitle,WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, 0, GAMESERVER_WIDTH, GAMESERVER_HEIGHT,
			NULL, NULL, hInstance, NULL);
	*/

	if(hWnd == NULL) return FALSE;
	
	INITCOMMONCONTROLSEX icc;
	icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icc.dwICC = ICC_BAR_CLASSES;
	InitCommonControlsEx(&icc);
	
	ghWnd=hWnd;

	HWND StatusBar = CreateWindowExA(0,STATUSCLASSNAME,NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0,ghWnd,(HMENU)130,hInstance,NULL);
	ghStatusBar = StatusBar;

	int iQueueBarWidths[] = {130, 340, 470, 600, 730, 860, 990, 1120, 1250, 1630, 1760, 1890, -1};
	SendMessage(StatusBar, SB_SETPARTS, 11, (LPARAM)iQueueBarWidths);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	CLEAR_END

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	INT wmId, wmEvent;
	
	switch (message) 
	{
		case WM_CREATE:
			return 0;
		case WM_SIZE:
			SendMessage(ghStatusBar, WM_SIZE, 0, 0);
			break;
		case WM_COMMAND:
			wmId    = wParam & 0xFFFF & 0xFFFF; 
			wmEvent = (wParam >> 16) & 0xFFFF; 
					
			// Parse the menu selections:
			switch (wmId)
			{
				//About
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUT_BOX, hWnd, (DLGPROC)About);
				   break;
				  // Players
				case ID_DISCONNECT_WITHCLOSE:
					gObjAllLogOut();
					break;
				case ID_DISCONNECT_WITHOUTCLOSE:
					gObjAllDisconnect();
					break;
				case ID_PLAYERS_DISCONNECTOFFTRADER:
					gObjDisconnectOffTraders();
					break;
				case ID_PLAYERS_DISCONNECTOFFLEVEL:
					gObjDisconnectOffLevelers();
					break;
				case ID_PLAYERS_DISCONNECTSELECTEDPLAYER:
					DialogBox(hInst, (LPCTSTR)IDD_DCMENU, hWnd, (DLGPROC)GSDisconnect);
					break;
				case ID_DISCONNECTWITHMESSAGE_5MINUTES:
					if ( gCloseMsg == 0 )
					{
						gCloseMsgTime = 300;
						gCloseMsg=1;
						gGameProtocol.AllSendServerMsg( Lang.GetText(0,38) );
					}
					break;
				case ID_DISCONNECTWITHMESSAGE_3MINUTES:
					if ( gCloseMsg == 0 )
					{
						gCloseMsgTime = 180;
						gCloseMsg=2;
						gGameProtocol.AllSendServerMsg(Lang.GetText(0,39) );
					}
					break;
				case ID_DISCONNECTWITHMESSAGE_1MINUTE:
					if ( gCloseMsg == 0 )
					{
						gCloseMsgTime = 60;
						gCloseMsg=3;
						gGameProtocol.AllSendServerMsg(Lang.GetText(0,40) );
					}
					break;
				case ID_EVENTS_RELOADBC:
					ReloadEvent(EVENT_BC);
					break;
				case ID_EVENTS_RELOADDS:
					ReloadEvent(EVENT_DS);
					break;
				case ID_EVENTS_RELOADCC:
					ReloadEvent(EVENT_CC);
					break;
				case ID_EVENTS_RELOADIT:
					ReloadEvent(EVENT_IT);
					break;
				case ID_CASTLESIEGE_RELOAD:
					ReloadEvent(EVENT_CS);
					break;
				case ID_EVENTS_RELOADDEEPEVENT:
					ReloadEvent(EVENT_DEEP);
					break;
				case ID_EVENTS_RELOADCRYWOLF:
					ReloadEvent(EVENT_CW);
					break;
				case ID_KANTURU_RELOAD:
					ReloadEvent(EVENT_KANTURU);
					break;
				case ID_EVENTS_RELOADRAKLION:
					ReloadEvent(EVENT_RAKLION);
					break;
				case ID_EVENTS_RELOADDG:
					ReloadEvent(EVENT_DG);
					break;
				case ID_EVENTS_RELOADIF:
					ReloadEvent(EVENT_IF);
					break;
				case ID_EVENTS_RELOADARCA:
					ReloadEvent(EVENT_ARCA);
					break;
				case ID_EVENTS_RELOADACHERONGUARDIAN:
					ReloadEvent(EVENT_ACHERON);
					break;
#if (ENABLE_CUSTOM_PVPBALANCE == 1)
				case ID_RELOADMAINDATA_PVPBALANCE:
					g_PvPBalance.Load();
					sLog->outBasic("[PvPBalance] Load Complete");
					break; 
#endif
				case ID_RELOADMAINDATA_FLOODSYSTEM1:
#if FILE_FLOOD_SYSTEMSWITCH
					AntiFlood.Init();
					sLog->outBasic("[FloodSystem] Load Complete");
#else
					sLog->outBasic("[FloodSystem] Disable");
#endif
					break;
				case ID_RELOADMAINDATA_USERDIEITEMDROP:
					g_UserDieItemDrop.Load();
					sLog->outBasic("[UserDieItemDrop] Load Complete");
					break;
				case ID_RELOADMAINDATA_CUSTEVENTDROP:
					gCustomEventDrop.Init();
					gCustomEventDrop.Load(g_ConfigRead.GetPath(FILE_CUSTOM_EVENTDROP));
					sLog->outBasic("[CustomEventDrop] Load Complete");
					break;
				case ID_EVENTS_RELOADLASTMANSTANDING:
					g_LastManStanding.LoadConfig(g_ConfigRead.GetPath("\\Events\\IGC_LastManStanding.xml"));
					g_LastManStanding.SpawnRegNPC();
					break;
				case ID_RELOADMAINDATA_ITEMS:
					OpenItemScript(g_ConfigRead.GetPath("\\Items\\IGC_ItemList.xml"));
					g_PentagramMixSystem.LoadMixNeedSourceScript(g_ConfigRead.GetPath("\\PentagramSystem\\IGC_PentagramMixNeedSource.xml"));
					g_PentagramMixSystem.LoadJewelOptionScript(g_ConfigRead.GetPath("\\PentagramSystem\\IGC_PentagramJewelOptionValue.xml"));
					g_PentagramSystem.LoadJewelOutRate(g_ConfigRead.GetPath("\\PentagramSystem\\IGC_PentagramJewelOutRate.xml"));
					g_PentagramSystem.LoadPentagramOptionScript(g_ConfigRead.GetPath("\\PentagramSystem\\IGC_PentagramOption.xml"));
					LoadResetItemList(g_ConfigRead.GetPath("\\Items\\IGC_ResetItem.xml"));
					gSetItemOption.LoadOptionInfo(g_ConfigRead.GetPath("\\Items\\IGC_ItemSetOption.xml"));
					gSetItemOption.LoadTypeInfo(g_ConfigRead.GetPath("\\Items\\IGC_ItemSetType.xml"));
					g_SocketOptionSystem.LoadOptionScript(g_ConfigRead.GetPath("\\SocketSystem\\IGC_SocketOption.xml"));
					g_kJewelOfHarmonySystem.LoadScript(g_ConfigRead.GetPath("\\Items\\IGC_HarmonyItem_Option.xml"));
					g_kJewelOfHarmonySystem.LoadScriptOfSmelt(g_ConfigRead.GetPath("\\Items\\IGC_HarmonyItem_Smelt.xml"));
					g_kItemSystemFor380.Load380ItemOptionInfo(g_ConfigRead.GetPath("\\Items\\IGC_Item380Option.xml"));
					g_ItemAddOption.Load(g_ConfigRead.GetPath("\\Items\\IGC_ItemOptionManager.xml"));
					break;
				case ID_RELOADMAINDATA_VIPSETTINGS:
					g_VipSystem.ReadFile("IGC_VipSettings.xml");
					break;
				case ID_RELOADMAINDATA_SKILL_LUA:
					gObjUseSkill.m_Lua.DoFile(g_ConfigRead.GetPath("\\Scripts\\Skills\\SkillRuudCalc.lua"));
					sLog->outBasic("[SkillRuudCalc] Load Complete");
					break;
				case ID_RELOADMAINDATA_SKILL_LUA1:
					gObjUseSkill.m_Lua.DoFile(g_ConfigRead.GetPath("\\Scripts\\Skills\\RegularSkillCalc.lua"));
					sLog->outBasic("[RegularSkillCalc] Load Complete");
					break;
				case ID_RELOADMAINDATA_SKILL_LUA2:
					g_MasterLevelSkillTreeSystem.m_Lua.DoFile(g_ConfigRead.GetPath("\\Scripts\\Skills\\MasterSkillPoint.lua"));
					sLog->outBasic("[MasterSkillPoint] Load Complete");
					break;
				case ID_RELOADMAINDATA_SKILL_LUA3:
					g_MasterLevelSkillTreeSystem.m_Lua.DoFile(g_ConfigRead.GetPath("\\Scripts\\Skills\\MasterSkillCalc.lua"));
					sLog->outBasic("[MasterSkillCalc] Load Complete");
					break;
				case ID_RELOADMAINDATA_SKILL_LUA4:
					gObjCalCharacter.m_Lua.DoFile(g_ConfigRead.GetPath("\\Scripts\\Character\\CalcCharacter.lua"));
					sLog->outBasic("[CalcCharacter] Load Complete");
					break;
				case ID_RELOADMAINDATA_SKILL_LUA5:
					g_StatSpec.m_Lua.DoFile(g_ConfigRead.GetPath("\\Scripts\\Specialization\\StatSpec.lua"));
					sLog->outBasic("[StatSpec] Load Complete");
					break;
				case ID_RELOADMAINDATA_AUTOPOTHACK:
					g_Michi.LoadPotionHack();
					sLog->outBasic("[AutoPotionHack] Load Complete");
					break;
				case ID_RELOADMAINDATA_NEWEVENTBAG:
					g_EventItemBagManager.Load();
					sLog->outBasic("[GoldenInvasion] Load Complete");
					break;
				case ID_RELOADMAINDATA_IGCMONMNG:
					g_DropEx.ReadData(g_ConfigRead.GetPath("Custom\\ItemDropRate.ini"));
					sLog->outBasic("[ItemDropRate] Load Complete");
					break;
				case ID_RELOADMAINDATA_ITEMVALUE:
					gItemValue.Load(g_ConfigRead.GetPath(FILE_CUSTOM_ITEMVAL));
					sLog->outBasic("[IGC_ItemValue] Load Complete");
					break;
				case ID_RELOADMAINDATA_ITEMOPTRATE:
					gItemOptionRate.Load(g_ConfigRead.GetPath(FILE_CUSTOM_ITEMOPTION));
					sLog->outBasic("[IGC_ItemOptionRate] Load Complete");
					break;
				case ID_RELOADMAINDATA_ADDMOBSTATS:
					gCMAttr.Load(g_ConfigRead.GetPath(FILE_CUSTOM_MONSTERSTATINFO));
					sLog->outBasic("[IGC_AddMonsterStats] Load Complete");
					break;
				case ID_RELOADMAINDATA_MUHELPERBOT:
					g_ConfigRead.ReadMuHelperSettings();
					sLog->outBasic("[IGC_MuHelper] Load Complete");
					break;
				case ID_RELOADMAINDATA_SHIELDGAGESYS:
					g_ConfigRead.ReadShieldGageSettings();
					sLog->outBasic("[IGC_ShieldGageSystem] Load Complete");
					break;
				case ID_RELOADMAINDATA_ANTIHACKS:
					g_ConfigRead.LoadAntiHackConfig();
					sLog->outBasic("[IGC_AntiHack] Load Complete");
					break;
				case ID_RELOADMAINDATA_PLAYERINFO:
					g_MaxStatsInfo.Read(g_ConfigRead.GetPath(FILE_CUSTOM_MAXSTATS));
					sLog->outBasic("[IGC_PlayerInfo] Load Complete");
					break;
				case ID_RELOADMAINDATA_GENERALDROP:
					g_MaxStatsInfo.ReadDrop(g_ConfigRead.GetPath(FILE_CUSTOM_GENERALDROP));
					sLog->outBasic("[IGC_GeneralDrop] Load Complete");
					break;
				case ID_RELOADMAINDATA_MAPNONPK:
					g_MaxStatsInfo.ReadMapNonPK(g_ConfigRead.GetPath(FILE_CUSTOM_MAPSNONPK));
					sLog->outBasic("[IGC_MapNonPK] Load Complete");
					break;
				case ID_RELOADMAINDATA_PENTADROPRATE:
					g_PentagramSystem.LoadOptionNews();
					g_PentagramSystem.LoadOptionMaps(g_ConfigRead.GetPath("PentagramSystem\\IGC_PentagramDropMaps.xml"));
					sLog->outBasic("[IGC_PentagramRateDrop] Load Complete");
					break;
				case ID_RELOADMAINDATA_PVPZONE:
					g_PVPZone.Load();
					sLog->outBasic("[IGC_PVPZone] Load Complete");
					break;
				case ID_RELOADMAINDATA_ITEMUPGRADEJEWEL:
					g_ItemUpgradeJewels.Load();
					sLog->outBasic("[IGC_ItemUpgradeJewel] Load Complete");
					break;
				case ID_RELOADMAINDATA_SOCKETUPGRADE:
					g_ConfigRead.ReadSocketMixSettings();
					sLog->outBasic("[IGC_SocketMix] Load Complete");
					break;
				case ID_RELOADMAINDATA_SKILLSTUNS:
					g_ConfigRead.ReadSkillStunsSettings();
					sLog->outBasic("[IGC_SkillStun] Load Complete");
					break;
				case ID_RELOADMAINDATA_PARTYBONUS:
					gParty.ReadBonusInfo(g_ConfigRead.GetPath(FILE_PARTYBONUS));
					sLog->outBasic("[IGC_PartyBonus] Load Complete");
					break;
				case ID_RELOADMAINDATA_ITEMDROPBLOCK:
					g_DropEx.ReadBlockData(g_ConfigRead.GetPath("Custom\\ItemDropBlocked.ini"));
					sLog->outBasic("[ItemDropBlocked] Load Complete");
					break;
				case ID_RELOADMAINDATA_SKILLS:
					MagicDamageC.LogSkillList(g_ConfigRead.GetPath("\\Skills\\IGC_SkillList.xml"));
					break;
				case ID_RELOADMAINDATA_WARPS:
					gMoveCommand.Load(g_ConfigRead.GetPath("\\Warps\\IGC_MoveReq.xml"));
					gMoveCommand.LoadMoveLevel(g_ConfigRead.GetPath("\\Warps\\IGC_MoveLevel.xml"));
					break;
				case ID_RELOADMAINDATA_EVENTBAGS:
					g_BagManager.InitBagManager();
					break;
				case ID_RELOADMAINDATA_EVENTMANAGER:
					g_EventManager.Load(g_ConfigRead.GetPath("\\Events\\IGC_InvasionManager.xml"));
					g_EventManager.RegEvent(0, DragonEvent);
					g_EventManager.RegEvent(1, AttackEvent);
					g_EventManager.RegEvent(2, &gEledoradoEvent);
					g_EventManager.RegEvent(3, &g_RingAttackEvent);
					g_EventManager.RegEvent(4, &g_XMasAttackEvent);
					g_EventManager.Init();
					break;
				case ID_RELOADMAINDATA_QUESTS:
					g_QuestInfo.LoadQuestInfo(g_ConfigRead.GetPath("IGC_ClassQuest.xml"));
					break;
				case ID_RELOADMAINDATA_MONSTERS:
					GameMonsterAllCloseAndReLoad();
					g_LastManStanding.LoadConfig(g_ConfigRead.GetPath("\\Events\\IGC_LastManStanding.xml"));
					g_LastManStanding.SpawnRegNPC();
					break;
				case ID_RELOADMAINDATA_ITEMSHOP:
					g_CashShop.CashShopOptioNReload();
					break;
				case ID_RELOADMAINDATA_SHOPS:
					g_ShopMng.LoadShopList(g_ConfigRead.GetPath("IGC_ShopList.xml"));
					break;
				case ID_RELOADMISCDATA_LANGUAGE:
					Lang.Init();
					break;
				case ID_RELOADMISCDATA_MAPSERVER:
					g_MapServerManager.LoadData(g_ConfigRead.GetPath("IGC_MapServerInfo.xml") );
					break;
				case ID_RELOADMISCDATA_CHECKSUM:
					CheckSumFileLoad(g_ConfigRead.GetPath("CheckSum.dat"));
					break;
				case ID_RELOADMISCDATA_ANTISWEAR:
					g_prohibitedSymbols.LoadSymbolFile(g_ConfigRead.GetPath("IGC_ProhibitedSymbols.xml"));
					SwearFilter.LoadFile(g_ConfigRead.GetPath("IGC_ProhibitedWords.xml"));
					break;
				case ID_RELOADMAINDATA_ITEMDROP:
					ItemDrop.LoadFile(g_ConfigRead.GetPath("IGC_DropManager.xml"));
					SetItemDrop.LoadFile(g_ConfigRead.GetPath("IGC_SetItemDropManager.xml"));
					break;
				case ID_RELOADMISCDATA_EXPMANAGER:
					g_ExpManager.LoadScript(g_ConfigRead.GetPath("IGC_ExpSystem.xml"));
					break;
				case ID_RELOADMISCDATA_MAPATTRIBUTE:
					g_MapAttr.LoadFile(g_ConfigRead.GetPath("IGC_MapAttribute.xml"));
					break;
				case ID_RELOADMISCDATA_COMMANDS:
					cManager.Init();
					break;
				case ID_RELOADMISCDATA_NEWSSYSTEM:
					g_NewsSystem.LoadFile(g_ConfigRead.GetPath("IGC_NewsSystem.xml"));
					break;
				case ID_RELOADMISCDATA_BOTS:
					GameMonsterAllCloseAndReLoad();
					//g_BotSystem.Init();
					//sLog->outBasic( "[GameServer] Please reload Monsters");
					break;
				case ID_DATA_ALLDATARELOAD:
					ReadCommonServerInfo();
					break;
				case ID_RELOADMAINDATA_OFFLEVELLING:
					/*if(g_ConfigRead.OffLevel == true)
					{*/
						g_OffLevel.LoadFile(g_ConfigRead.GetPath("\\Plugins\\IGC_OffLevelling.xml"));
						g_OffLevel.LoadSkillDefinitions(g_ConfigRead.GetPath("\\Skills\\IGC_SkillCategory.xml"));	
					/*}
					else {
						sLog->outError( "Your license does not have Offlevel plugin purchased, please disable Off-Level system");
					}*/
					break;
				case ID_LICENSE_SHOWINFO:
					g_AuthSys.GetLicenseInfo();
					break;
				case ID_LICENSE_DELETELICENSE:
					{
						if(MessageBox(hWnd, "It will delete license data from your computer! After that, GameServer will be closed. Proceed?", "WARNING", MB_ICONWARNING|MB_YESNO|MB_TOPMOST) == IDYES)
						{
							HKEY hKey;

							if(RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\MUEMU", 0, KEY_READ, &hKey) != ERROR_SUCCESS)
							{
								g_AuthSys.SendMessage(NO_LICENSE_IN_REGISTRY);
								g_AuthSys.MessageHandle(NO_LICENSE_IN_REGISTRY);
							}
						
							RegDeleteKeyEx(HKEY_CURRENT_USER, "Software\\MUEMU", KEY_WOW64_64KEY, 0);

							ExitProcess(0);
						}
					}
					break;
				default:
					return DefWindowProc(hWnd, message, wParam, lParam);
					break;
			}
			break;
		case WM_GM_SERVER_MSG_PROC:
			GMServerMsgProc(wParam, lParam);
			break;
		case WM_GM_CLIENT_MSG_PROC:
			GMClientMsgProc(wParam, lParam);
			break;
		case WM_GM_JOIN_CLIENT_MSG_PROC:
			GMJoinClientMsgProc(wParam, lParam);
			break;
		case WM_GM_DATA_CLIENT_MSG_PROC:
			GMDataClientMsgProc(wParam, lParam);
			break;
		case WM_GM_EXDATA_CLIENT_MSG_PROC:
			ExDataClientMsgProc(wParam, lParam);
			break;
		case WM_GM_AUTH_SERVER_MSG_PROC:
			AuthClientMsgProc(wParam,lParam);
			break;
		case WM_START_SERVER:
			if ( gServerReady == 2 )
				GameServerStart();
			break;
		case WM_TIMER:
			switch ( wParam )
			{
				case WM_LOG_PAINT:
					{
						RECT rect;
						HDC hdc = GetDC(hWnd);
						GetClientRect(hWnd, &rect);
						FillRect(hdc, &rect, (HBRUSH)GetStockObject(0));
						ReleaseDC(hWnd, hdc);

						if ( gCurPaintType == 0)
						{
							g_Log.LogTextPaint(hWnd);
						}
						
						gObjViewportPaint(hWnd);
						g_ServerInfoDisplayer.Run(hWnd);
						GJNotifyMaxUserCount();
						UpdateInfoBar(ghStatusBar);
						CheckAuthThread();
					}
					break;
				case WM_CONNECT_DATASERVER:
					GMJoinServerConnect(g_ConfigRead.server.GetJoinServerIP(), WM_GM_JOIN_CLIENT_MSG_PROC);
					GMDataServerConnect(g_ConfigRead.server.GetDataServerIP(), WM_GM_DATA_CLIENT_MSG_PROC);
					ExDataServerConnect(g_ConfigRead.server.GetExDBIP(), WM_GM_EXDATA_CLIENT_MSG_PROC);
					break;
			}
			break;
		case WM_CLOSE:
			if (MessageBox(ghWnd, "Do you want to close eGameServer? -(o_0)'--", "Close", MB_YESNO|MB_APPLMODAL|MB_TOPMOST) == IDYES)
			{
				DestroyWindow(hWnd);
			}
			break;
		case WM_DESTROY:
			g_MuTimer.SetTimerEnd();
			GameMainFree();
			IOCP.GiocpDelete();
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// Message handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if ( ( wParam & 0xFFFF & 0xFFFF ) == IDOK || (wParam & 0xFFFF & 0xFFFF ) == IDCANCEL) 
			{
				EndDialog(hDlg, (wParam& 0xFFFF & 0xFFFF ));
				return TRUE;
			}
			break;
	}
    return FALSE;
}

LRESULT CALLBACK LicenseInfo(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			return TRUE;

		case WM_COMMAND:
			if ( LOWORD(wParam) == IDC_LICENSECLOSE) 
			{
				EndDialog(hDlg, 0);
				return TRUE;
			}
			break;
	}
    return FALSE;
}

LRESULT CALLBACK GSDisconnect(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_COMMAND:
		{
			switch (LOWORD(wParam)) 
			{
			case IDC_DCACCOUNT:
				{
					char szAccount[MAX_ACCOUNT_LEN + 1];
					int sLen = GetDlgItemText(hDlg,IDC_DCTEXT,szAccount,sizeof(szAccount));
					szAccount[MAX_ACCOUNT_LEN] = 0;

					if(sLen <= 0 || sLen > sizeof(szAccount))
					{
						sLog->outError(  "[GameServer] could not read account name");
					}
					else
					{
						for(int i=g_ConfigRead.server.GetObjectStartUserIndex();i<g_ConfigRead.server.GetObjectMax();i++)
						{
							if(gObjIsAccontConnect(i, szAccount) == TRUE)
							{
								if (getGameObject(i)->m_bOff == true)
								{
									GJSetCharacterInfo(getGameObject(i), i, FALSE);
									getGameObject(i)->m_bOff = false;
									gObjDel(i);
								}

								sLog->outBasic( "[GameServer] Account(%s) disconnected",szAccount);

								if(IsDlgButtonChecked(hDlg, IDC_DCBAN) == TRUE)
								{
									GDReqBanUser(i, 1, 1);
								}

								//gGameProtocol.GCSendDisableReconnect(i);
								IOCP.CloseClient(i);
								break;
							}
						}
					}
				}
				EndDialog(hDlg, 0);
				break;
			case IDC_DCCHARACTER:
				{
					char szAccount[MAX_ACCOUNT_LEN + 1];
					int sLen = GetDlgItemText(hDlg,IDC_DCTEXT,szAccount,sizeof(szAccount));
					szAccount[MAX_ACCOUNT_LEN] = 0;

					if(sLen <= 0 || sLen > sizeof(szAccount))
					{
						sLog->outError(  "[GameServer] could not read character name");
					}
					else
					{
						for(int i=g_ConfigRead.server.GetObjectStartUserIndex();i<g_ConfigRead.server.GetObjectMax();i++)
						{
							if(gObjIsConnectedGP(i, szAccount) == TRUE)
							{
								if (getGameObject(i)->m_bOff == true)
								{
									GJSetCharacterInfo(getGameObject(i), i, FALSE);
									getGameObject(i)->m_bOff = false;
									gObjDel(i);
								}

								sLog->outBasic( "[GameServer] Character(%s) disconnected",szAccount);

								if(IsDlgButtonChecked(hDlg, IDC_DCBAN) == TRUE)
								{
									GDReqBanUser(i, 0, 1);
								}

								//gGameProtocol.GCSendDisableReconnect(i);
								IOCP.CloseClient(i);
								break;
							}
						}
					}
				}
				EndDialog(hDlg, 0);
				break;
			}
		}
		return true;
		case WM_CLOSE:
		{
			EndDialog(hDlg, 0);
			return true;
		}
	}
	return false;
}

void UpdateInfoBar(HWND hWnd)
{
	HDC hDC;
	hDC = GetDC(hWnd);

	// maybe move this functions to their system file instead write logic here? likely yes
	char szTempText[85];

	wsprintf(szTempText, "Players: %d/%d", gObjTotalUser, g_ConfigRead.server.GetObjectMaxUser());
	SendMessage(ghStatusBar, SB_SETTEXT, 0, (LPARAM)szTempText);

	wsprintf(szTempText, "Monsters: (A/L/M) %d/%d/%d", gObjTotalMonster, gMSetBase.m_Count, g_ConfigRead.server.GetObjectMaxMonster());
	SendMessage(ghStatusBar, SB_SETTEXT, 1, (LPARAM)szTempText);

	wsprintf(szTempText, "Items: %d", gItemLoop);
	SendMessage(ghStatusBar, SB_SETTEXT, 2, (LPARAM)szTempText);

	sprintf(szTempText,"Off-Traders %d", gObjGetOffTradeUsercount());
	SendMessage(ghStatusBar, SB_SETTEXT, 3, (LPARAM)szTempText);

	sprintf(szTempText,"Off-Levelers: %d", g_OffLevel.GetOffLevelerCount());
	SendMessage(ghStatusBar, SB_SETTEXT, 4, (LPARAM)szTempText);

	sprintf(szTempText,"Auto Party: %d", gObjGetAutoPartyUserCount());
	SendMessage(ghStatusBar, SB_SETTEXT, 5, (LPARAM)szTempText);

	//if(g_BloodCastle.CheckCanEnter(0) == true)
	//{
	//	wsprintf(szTempText, "BC: Open");
	//}
	//else
	//{
	//	wsprintf(szTempText, "BC: %d minutes", g_BloodCastle.GetRemainTime(0));
	//}
	
	if(g_ChaosCastle.CheckCanEnter(0) == true)
	{
		wsprintf(szTempText, "CC: Open");
	}
	else
	{
		wsprintf(szTempText, "CC: %d minutes", g_ChaosCastle.GetRemainTime(0));
	}

	SendMessage(ghStatusBar, SB_SETTEXT, 6, (LPARAM)szTempText);

	if(g_IT_Event.GetRemainTime() == 0)
	{
		wsprintf(szTempText, "ITR: Open");
	}
	else
	{
		wsprintf(szTempText, "ITR: %d minutes", g_IT_Event.GetRemainTime());
	}

	SendMessage(ghStatusBar, SB_SETTEXT, 7, (LPARAM)szTempText);

	if(g_DevilSquare.GetState() == DevilSquare_OPEN)
	{
		wsprintf(szTempText, "DS: Open");
	}
	else if(g_DevilSquare.GetState() == DevilSquare_PLAYING)
	{
		wsprintf(szTempText, "DS: Playing");
	}
	else
	{
		wsprintf(szTempText, "DS: %d minutes", g_DevilSquare.GetRemainTime());
	}

	SendMessage(ghStatusBar, SB_SETTEXT, 8, (LPARAM)szTempText);
/*
	int DaysLeft = g_AuthSys.GetExpiryTime();

	if(DaysLeft == LICENSE_PERMANENT)
	{
		wsprintf(szTempText, "License: Permanent");
	}
	
	else
	{
		wsprintf(szTempText, "License: %d days", DaysLeft);
	}
	*/
	wsprintf(szTempText, "License: vndev.Games ");

	SendMessage(ghStatusBar, SB_SETTEXT, 9, (LPARAM)szTempText);

	ReleaseDC(hWnd, hDC);

#ifdef GS_AUTHORITY
	if(WLProtectCheckDebugger() == TRUE)
	{
		exit(1);
	}
#endif

}

LRESULT CALLBACK ActivatingWindow( HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
    switch( Msg )
    {
    case WM_COMMAND:
        break;
        
		default: return DefWindowProc(hwnd, Msg, wParam, lParam);
    }
    
    return TRUE;
}

LRESULT CALLBACK AuthWindow(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hActivation;
    switch (message)
    {

    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        break;

    }

    return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

