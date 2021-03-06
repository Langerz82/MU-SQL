// DSprotocol.cpp: implementation of the DSprotocol class.
//
//////////////////////////////////////////////////////////////////////

#include "CoreDSprotocol.h"
#include "giocp.h"
#include "ItemSerial.h"
#include "ServerEngine.h"
#include "MapServerManager.h"
#include "Main.h"
#include "Utility/util.h"

#define szModule "CoreDSProtocol"

using namespace std;

CDataServerProtocol::CDataServerProtocol()
{
	DatabaseWorkerPool<MySQLConnection>* pDB = (DatabaseWorkerPool<MySQLConnection>*) &ConnectDatabase;

	m_CharDB = new CQuery(pDB);
	m_AccDB = new CQuery(pDB);
	m_GSDB = new CQuery(pDB);
	m_PetDB = new CQuery(pDB);
	m_CastleDB = new CQuery(pDB);
	m_CrywolfDB = new CQuery(pDB);
	m_PentagramDB = new CQuery(pDB);
	m_EventDB = new CQuery(pDB);
	m_RankingDB = new CQuery(pDB);
	m_ItemShopDB = new CQuery(pDB);
	m_PeriodItemDB = new CQuery(pDB);
	m_CharMiscDB = new CQuery(pDB);
	m_ArcaDB = new CQuery(pDB);
	m_QuestExpDB = new CQuery(pDB);
	m_LuckyItemDB = new CQuery(pDB);
	m_CCFinalDB = new CQuery(pDB);
	m_RummyDB = new CQuery(pDB);
	m_MineDB = new CQuery(pDB);
	m_PShopDB = new CQuery(pDB);
	m_EventInvDB = new CQuery(pDB);
	m_MuunDB = new CQuery(pDB);
	m_OptionDataDB = new CQuery(pDB);
	m_ReBuyDB = new CQuery(pDB);
	m_GremoryCaseDB = new CQuery(pDB);
	m_BattleCoreDB = new CQuery(pDB);
	m_DSFinalDB = new CQuery(pDB);
	m_EvoMonDB = new CQuery(pDB);
	m_Survivor = new CQuery(pDB);

	this->m_iConnectionCount = 0;
	this->m_iItemCount = 0;

	InitializeCriticalSection(&this->m_QuestExpDBCriti);
}

CDataServerProtocol::~CDataServerProtocol()
{
	delete m_CharDB;
	delete m_AccDB;
	delete m_GSDB;
	delete m_PetDB;
	delete m_CastleDB;
	delete m_CrywolfDB;
	delete m_PentagramDB;
	delete m_EventDB;
	delete m_RankingDB;
	delete m_ItemShopDB;
	delete m_PeriodItemDB;
	delete m_CharMiscDB;
	delete m_ArcaDB;
	delete m_QuestExpDB;
	delete m_LuckyItemDB;
	delete m_CCFinalDB;
	delete m_RummyDB;
	delete m_MineDB;
	delete m_PShopDB;
	delete m_EventInvDB;
	delete m_MuunDB;
	delete m_OptionDataDB;
	delete m_ReBuyDB;
	delete m_GremoryCaseDB;
	delete m_BattleCoreDB;
	delete m_DSFinalDB;
	delete m_EvoMonDB;
	delete m_Survivor;

	DeleteCriticalSection(&this->m_QuestExpDBCriti);
}

BOOL CDataServerProtocol::Init()
{
	this->m_WareUserData->Init();

	QueryResult* res = this->m_GSDB->Fetch("SELECT ItemCount FROM GameServerInfo WHERE Number = 0");

	this->m_iItemCount = this->m_GSDB->GetAsInteger64(0);

	sLog->outBasic("[DataServer] ItemCount: %d", this->m_iItemCount);

	return TRUE;
}

void CDataServerProtocol::DataServerLogin(int aIndex, SDHP_SERVERINFO * lpMsg)
{
	SDHP_RESULT pResult = { 0 };

	PHeadSetB((LPBYTE)&pResult, 0x00, sizeof(pResult));
	pResult.Result = 1;
	pResult.ItemCount = this->m_iItemCount;

	g_Server[aIndex].m_ServerCode = lpMsg->ServerCode;
	g_Server[aIndex].m_ServerGroup = g_MapServerManager.GetMapSvrGroup(lpMsg->ServerCode);

	sLog->outBasic("[DataServer] GameServer(%d)(%d) connect", lpMsg->ServerCode, g_MapServerManager.GetMapSvrGroup(lpMsg->ServerCode));

	//DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size, __FUNCTION__);
}

void CDataServerProtocol::JGPGetCharList(int aIndex, SDHP_GETCHARLIST * aRecv)
{
	char cBUFFER[10000] = { 0 };

	SDHP_CHARLISTCOUNT * pCount = (SDHP_CHARLISTCOUNT *)(cBUFFER);
	SDHP_CHARLIST * pCL = (SDHP_CHARLIST *)(cBUFFER + sizeof(SDHP_CHARLISTCOUNT));

	char szAccountID[11];
	szAccountID[10] = 0;
	memcpy(szAccountID, aRecv->Id, 10);

	pCount->Number = aRecv->Number;
	memcpy(pCount->AccountId, szAccountID, MAX_ACCOUNT_LEN);

	QueryResult* res = this->m_AccDB->Fetch("SELECT Id FROM AccountCharacter WHERE Id='%s'", szAccountID);
	if (res == NULL)
	{
		return;
	}

	if (this->m_AccDB.HasFields())
	{
		this->m_AccDB.ExecQuery("INSERT INTO AccountCharacter (Id) VALUES ('%s')", szAccountID);
		sLog->outBasic("[%s] - no characters", szAccountID);
	}

	QueryResult* res = this->m_AccDB.Fetch("SELECT * FROM AccountCharacter WHERE Id='%s'", szAccountID);
	this->m_AccDB.Fetch();

	TCHAR CharName[5][11] = { 0 };
	memset(CharName, 0x00, sizeof(CharName));

	pCount->MoveCnt = (BYTE)this->m_AccDB.GetAsInteger("MoveCnt");

	this->m_AccDB.GetAsString("GameID1", CharName[0], MAX_ACCOUNT_LEN);
	this->m_AccDB.GetAsString("GameID2", CharName[1], MAX_ACCOUNT_LEN);
	this->m_AccDB.GetAsString("GameID3", CharName[2], MAX_ACCOUNT_LEN);
	this->m_AccDB.GetAsString("GameID4", CharName[3], MAX_ACCOUNT_LEN);
	this->m_AccDB.GetAsString("GameID5", CharName[4], MAX_ACCOUNT_LEN);

	this->m_AccDB.Close();

	g_Log.Add("[%s] - characters: [%s][%s][%s][%s][%s]", szAccountID, CharName[0], CharName[1], CharName[2], CharName[3], CharName[4]);

	QueryResult* res = this->m_AccDB.Fetch("SELECT WarehouseExpansion, Summoner, RageFighter, SecCode FROM AccountCharacter WHERE Id='%s'", szAccountID);
	this->m_AccDB.Fetch();

	pCount->WhExpansion = this->m_AccDB.GetAsInteger("WarehouseExpansion");
	pCount->SecurityCode = this->m_AccDB.GetAsInteger("SecCode");
	pCount->CharacterSlotCount = this->m_AccDB.GetAsInteger("SlotCount");

	BOOL Summoner = this->m_AccDB.GetAsInteger("Summoner");
	BOOL RF = this->m_AccDB.GetAsInteger("RageFighter");

	if (Summoner == TRUE)
		pCount->EnableCharacterCreate |= 1;
	if (RF == TRUE)
		pCount->EnableCharacterCreate |= 8;

	this->m_AccDB.Close();

	g_Log.Add("[%s] - Account Data: [SU:%d][RF:%d][Expanded WH:%d]", szAccountID, Summoner, RF, pCount->WhExpansion);

	int iCharCount = 0;
	int lOfs = sizeof(SDHP_CHARLISTCOUNT);

	for (int i = 0; i<5; i++)
	{
		if (CharName[i][0] != 0)
		{
			if (lstrlen(CharName[i]) >= 4)
			{
				if (QueryResult* res = this->m_AccDB.Fetch("SELECT cLevel, Class, CtlCode FROM vCharacterPreview WHERE Name='%s'", CharName[i]) == TRUE)
				{
					char szTemp[200];
					pCL = (SDHP_CHARLIST *)(cBUFFER + lOfs);

					this->m_AccDB.Fetch();
					pCL->Level = (WORD)this->m_AccDB.GetAsInteger("cLevel");
					pCL->Class = (BYTE)this->m_AccDB.GetAsInteger("Class");
					pCL->Resets = (WORD)this->m_AccDB.GetAsInteger("RESETS");
					if (pCL->Level >= g_MagumsaCreateMinLevel && (pCount->EnableCharacterCreate & 4) != 4)
						pCount->EnableCharacterCreate |= 4;

					if (pCL->Level >= g_DarkLordCreateMinLevel && (pCount->EnableCharacterCreate & 2) != 2)
						pCount->EnableCharacterCreate |= 2;

					if (pCL->Level >= g_GrowLancerCreateMinLevel && (pCount->EnableCharacterCreate & 0x10) != 0x10)
						pCount->EnableCharacterCreate |= 0x10;

					pCL->CtlCode = (BYTE)this->m_AccDB.GetAsInteger("CtlCode");
					this->m_AccDB.Close();

					BYTE btInvetory[INVENTORY_BINARY_SIZE];
					wsprintf(szTemp, "SELECT Inventory FROM Character WHERE Name='%s'", CharName[i]);
					int ret = this->m_AccDB.GetAsBinary(szTemp, btInvetory, sizeof(btInvetory));

					int dbsize = 32;

					memset(pCL->dbInventory, -1, sizeof(pCL->dbInventory));

					for (int j = 0; j<12; j++)
					{
						if (btInvetory[0 + j * dbsize] == 0xFF && (btInvetory[7 + j * dbsize] & 0x80) == 0x80 && (btInvetory[10 + j * dbsize] & 0xF0) == 0xF0)
						{
							pCL->dbInventory[j * 4] = -1;
							pCL->dbInventory[j * 4 + 1] = -1;
							pCL->dbInventory[j * 4 + 2] = -1;
							pCL->dbInventory[j * 4 + 3] = -1;
						}
						else
						{
							pCL->dbInventory[j * 4] = (btInvetory[0 + j * dbsize]);	// 0..7 bits of Item
							pCL->dbInventory[j * 4 + 1] = (btInvetory[1 + j * dbsize]);	// Get Level of Item
							pCL->dbInventory[j * 4 + 2] = (btInvetory[7 + j * dbsize]);	// 8 bit     of Item
							pCL->dbInventory[j * 4 + 3] = (btInvetory[9 + j * dbsize]);	// 9..12 bit of Item
						}
					}

					QueryResult* res = this->m_AccDB.Fetch("SELECT RESETS FROM Character WHERE Name='%s'", CharName[i]);

					if (this->m_AccDB.Fetch() != SQL_NO_DATA)
					{
						pCL->Resets = (WORD)this->m_AccDB.GetAsInteger("RESETS");
					}

					this->m_AccDB.Close();

					if (aRecv->IsUnityBattleFieldServer == FALSE)
					{
						QueryResult* res = this->m_AccDB.Fetch("SELECT G_Status FROM GuildMember WHERE Name='%s'", CharName[i]);

						if (this->m_AccDB.Fetch() != SQL_NO_DATA)
						{
							pCL->btGuildStatus = (BYTE)this->m_AccDB.GetAsInteger("G_Status");
						}

						else
						{
							pCL->btGuildStatus = 0xFF;
						}
					}

					else
					{
						pCL->btGuildStatus = 0xFF;
					}

					if (aRecv->IsUnityBattleFieldServer == TRUE)
					{
						this->m_BattleCoreDB.ExecQuery("EXEC IGC_BattleCore_GetRealName '%s'", CharName[i]);
						this->m_BattleCoreDB.Fetch();

						this->m_BattleCoreDB.GetAsString("RealName", pCL->UnityBFRealName, MAX_ACCOUNT_LEN + 1);
						pCL->ServerCodeOfHomeWorld = this->m_BattleCoreDB.GetAsInteger("ServerCode");

						this->m_BattleCoreDB.Close();
					}

					else
					{
						strcpy(pCL->UnityBFRealName, CharName[i]);
						pCL->ServerCodeOfHomeWorld = 0;
					}

					pCL->Index = i;
					memcpy(pCL->Name, CharName[i], MAX_ACCOUNT_LEN);
					//g_MuOnlineDB.Close();
					iCharCount++;
					lOfs += sizeof(SDHP_CHARLIST);
				}

				this->m_AccDB.Close();
			}
		}
	}

	if (g_MagumsaCreateMinLevel == 0 && (pCount->EnableCharacterCreate & 4) != 4)
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
	}

	PHeadSetW((LPBYTE)pCount, 0x01, sizeof(SDHP_CHARLISTCOUNT) + sizeof(SDHP_CHARLIST) * iCharCount);
	pCount->Count = iCharCount;

	//DataSend(aIndex, (LPBYTE)cBUFFER, sizeof(SDHP_CHARLISTCOUNT) + sizeof(SDHP_CHARLIST) * iCharCount, __FUNCTION__);
}

void CDataServerProtocol::JGCharacterCreateRequest(int aIndex, SDHP_CREATECHAR * aRecv)
{
	SDHP_CREATECHARRESULT pResult = { 0 };
	PHeadSetB((LPBYTE)&pResult, 0x04, sizeof(pResult));
	memcpy(pResult.AccountId, aRecv->AccountId, 10);
	pResult.ClassSkin = aRecv->ClassSkin;
	pResult.Number = aRecv->Number;
	int iIndex = 0;

	char szAccountID[11] = { 0 };
	char szName[11] = { 0 };
	szAccountID[10] = 0;
	szName[10] = 0;
	memcpy(szAccountID, aRecv->AccountId, 10);
	memcpy(szName, aRecv->Name, 10);

	if (SpaceSyntexCheck(szName) == FALSE || QuoteSpaceSyntexCheck(szName) == FALSE || PercentSyntaxCheck(szName) == FALSE)
	{
		pResult.Result = 0;
		//DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size, __FUNCTION__);
		return;
	}

	QueryResult* res = this->m_AccDB.Fetch("SELECT * FROM AccountCharacter WHERE Id='%s'", szAccountID);
	this->m_AccDB.Fetch();

	TCHAR CharName[5][11] = { 0 };
	memset(CharName, 0x00, sizeof(CharName));

	this->m_AccDB.GetAsString("GameID1", CharName[0], sizeof(CharName[0]) - 1);
	this->m_AccDB.GetAsString("GameID2", CharName[1], sizeof(CharName[0]) - 1);
	this->m_AccDB.GetAsString("GameID3", CharName[2], sizeof(CharName[0]) - 1);
	this->m_AccDB.GetAsString("GameID4", CharName[3], sizeof(CharName[0]) - 1);
	this->m_AccDB.GetAsString("GameID5", CharName[4], sizeof(CharName[0]) - 1);

	this->m_AccDB.Close();

	for (iIndex = 0; iIndex<5; iIndex++)
	{
		if (CharName[iIndex][0] == 0)
			break;
	}

	if (iIndex == 5)
	{
		pResult.Result = 2;
		g_Log.Add("[%s] cannot create character - no empty slot", szAccountID);
	}
	else
	{
		memcpy(pResult.Name, aRecv->Name, 10);
		pResult.Pos = iIndex;

		if ((pResult.ClassSkin >> 4) < 0 || (pResult.ClassSkin >> 4) > MAX_TYPE_PLAYER - 1)
		{
			pResult.Result = 0;
			g_Log.Add("[%s] error - wrong type of character", szAccountID);
		}

		this->m_AccDB.ExecQuery("WZ_CreateCharacter '%s', '%s', '%d'", szAccountID, szName, pResult.ClassSkin);
		this->m_AccDB.Fetch();
		pResult.Result = this->m_AccDB.GetResult(0);
		this->m_AccDB.Close();

		if (pResult.Result == 1)
		{
			this->m_AccDB.ExecQuery("UPDATE AccountCharacter SET GameID%d='%s' WHERE Id='%s'",
				iIndex + 1, szName, szAccountID);
			this->m_AccDB.Close();

			if (QueryResult* res = this->m_AccDB.Fetch("SELECT cLevel FROM vCharacterPreview WHERE Name='%s'", szName) == TRUE && this->m_AccDB.Fetch() != SQL_NO_DATA)
			{
				pResult.Level = this->m_AccDB.GetAsInteger("cLevel");
			}

			this->m_AccDB.Close();

			g_Log.Add("[%s] created character -> [%s]", szAccountID, szName);
		}
	}

	//DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size, __FUNCTION__);
}

void CDataServerProtocol::JGCharDelRequest(int aIndex, SDHP_CHARDELETE * aRecv)
{
	SDHP_CHARDELETERESULT pResult = { 0 };

	PHeadSetB((LPBYTE)&pResult, 0x05, sizeof(pResult));
	memcpy(pResult.AccountID, aRecv->AccountID, 10);
	pResult.Number = aRecv->Number;
	pResult.Result = 1;

	char szAccountID[11] = { 0 };
	char szName[11] = { 0 };
	szAccountID[10] = 0;
	szName[10] = 0;
	memcpy(szAccountID, aRecv->AccountID, 10);
	memcpy(szName, aRecv->Name, 10);

	if (lstrlen(aRecv->AccountID) < 1)
		pResult.Result = 0;

	if (lstrlen(aRecv->Name) < 1)
		pResult.Result = 0;

	if (pResult.Result == 1)
	{
		this->m_AccDB.ExecQuery("EXEC IGC_DeleteCharacter '%s'", szName);
		this->m_AccDB.Fetch();

		int iQueryResult = this->m_AccDB.GetAsInteger("QueryResult");
		this->m_AccDB.Close();

		if (iQueryResult == TRUE)
		{
			QueryResult* res = this->m_AccDB.Fetch("SELECT * FROM AccountCharacter WHERE Id='%s'", szAccountID);
			this->m_AccDB.Fetch();
			TCHAR CharName[5][11] = { 0 };
			memset(CharName, 0x00, sizeof(CharName));
			this->m_AccDB.GetAsString("GameID1", CharName[0], sizeof(CharName[0]) - 1);
			this->m_AccDB.GetAsString("GameID2", CharName[1], sizeof(CharName[0]) - 1);
			this->m_AccDB.GetAsString("GameID3", CharName[2], sizeof(CharName[0]) - 1);
			this->m_AccDB.GetAsString("GameID4", CharName[3], sizeof(CharName[0]) - 1);
			this->m_AccDB.GetAsString("GameID5", CharName[4], sizeof(CharName[0]) - 1);
			int iIndex;
			for (iIndex = 0; iIndex < 5; iIndex++)
			{
				if (lstrcmp(CharName[iIndex], szName) == 0)
				{
					break;
				}
			}
			this->m_AccDB.Close();

			this->m_AccDB.ExecQuery("UPDATE AccountCharacter SET GameID%d=NULL WHERE Id='%s'", iIndex + 1, szAccountID);
			this->m_AccDB.Close();

			g_Log.Add("[%s] deleted character -> [%s]", szAccountID, szName);
		}

		else
		{
			pResult.Result = 0;
		}
	}

	//DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size, __FUNCTION__);
}

void CDataServerProtocol::JGGetCharacterInfo(int aIndex, SDHP_DBCHARINFOREQUEST * aRecv)
{
	SDHP_DBCHAR_INFORESULT pResult = { 0 };

	PHeadSetW((LPBYTE)&pResult, 0x06, sizeof(pResult));
	char szAccountID[11];
	char szName[11];
	szAccountID[10] = 0;
	szName[10] = 0;
	memcpy(szAccountID, aRecv->AccountID, 10);
	memcpy(szName, aRecv->Name, 10);
	memcpy(pResult.AccountID, aRecv->AccountID, 10);
	memcpy(pResult.Name, aRecv->Name, 10);
	pResult.Number = aRecv->Number;
	pResult.result = 0;
	char AccountCharacter[5][11];
	memset(AccountCharacter, 0x00, sizeof(AccountCharacter));

	QueryResult* res = this->m_AccDB.Fetch("SELECT * FROM AccountCharacter WHERE Id = '%s'", szAccountID);

	if (this->m_AccDB.Fetch() != SQL_NO_DATA)
	{
		this->m_AccDB.GetAsString("GameID1", AccountCharacter[0], sizeof(AccountCharacter[0]) - 1);
		this->m_AccDB.GetAsString("GameID2", AccountCharacter[1], sizeof(AccountCharacter[1]) - 1);
		this->m_AccDB.GetAsString("GameID3", AccountCharacter[2], sizeof(AccountCharacter[2]) - 1);
		this->m_AccDB.GetAsString("GameID4", AccountCharacter[3], sizeof(AccountCharacter[3]) - 1);
		this->m_AccDB.GetAsString("GameID5", AccountCharacter[4], sizeof(AccountCharacter[4]) - 1);

	}

	this->m_AccDB.Close();
	bool charexist = false;

	for (int i = 0; i < 5; i++)
	{
		if (!strcmp(AccountCharacter[i], szName))
		{
			charexist = true;
			break;
		}
	}

	if (!charexist)
	{
		sLog->outError("[ERROR] Char NOT EXIST (Name:%s),", szName);
		return;
	}

	char WHExpansion = 0;

	QueryResult* res = this->m_AccDB.Fetch("SELECT WarehouseExpansion FROM AccountCharacter WHERE Id = '%s'", szAccountID);
	this->m_AccDB.Fetch();

	WHExpansion = this->m_AccDB.GetAsInteger("WarehouseExpansion");

	this->m_AccDB.Close();

	if (g_DSBattleCoreEnable == 1)
	{
		if (QueryResult* res = this->m_CharDB.Fetch("SELECT cLevel, mLevel, Class, LevelUpPoint, mlPoint, Experience, mlExperience, mlNextExp, Strength, Dexterity, Vitality, Energy, Money, Life, MaxLife, Mana, MaxMana, MapNumber, MapPosX, MapPosY, MapDir, PkCount, PkLevel, PkTime, CtlCode, Leadership, ChatLimitTime, FruitPoint, RESETS, Married, MarryName, InventoryExpansion, WinDuels, LoseDuels, BlockChatTime, PenaltyMask FROM Character WHERE Name='%s' AND AccountID='%s'", szName, szAccountID) == FALSE)
		{
			this->m_CharDB.Close();
			return;
		}
	}
	else
	{
		if (QueryResult* res = this->m_CharDB.Fetch("SELECT cLevel, mLevel, Class, LevelUpPoint, mlPoint, Experience, mlExperience, mlNextExp, Strength, Dexterity, Vitality, Energy, Money, Life, MaxLife, Mana, MaxMana, MapNumber, MapPosX, MapPosY, MapDir, PkCount, PkLevel, PkTime, CtlCode, Leadership, ChatLimitTime, FruitPoint, RESETS, Married, MarryName, InventoryExpansion, WinDuels, LoseDuels, BlockChatTime, PenaltyMask, Ruud FROM Character WHERE Name='%s' AND AccountID='%s'", szName, szAccountID) == FALSE)
		{
			this->m_CharDB.Close();
			return;
		}
	}

	if (this->m_CharDB.Fetch() == SQL_NO_DATA)
	{
		this->m_CharDB.Close();
		return;
	}

	pResult.Level = (short)this->m_CharDB.GetAsInteger("cLevel");
	pResult.mLevel = this->m_CharDB.GetAsInteger("mLevel");
	pResult.Class = (BYTE)this->m_CharDB.GetAsInteger("Class");
	pResult.LevelUpPoint = this->m_CharDB.GetAsInteger("LevelUpPoint");
	pResult.mlPoint = this->m_CharDB.GetAsInteger("mlPoint");
	pResult.Exp = this->m_CharDB.GetAsInteger64("Experience");
	pResult.mlExp = this->m_CharDB.GetAsInteger64("mlExperience");
	pResult.mlNextExp = this->m_CharDB.GetAsInteger64("mlNextExp");
	pResult.Str = (WORD)this->m_CharDB.GetAsInteger("Strength");
	pResult.Dex = (WORD)this->m_CharDB.GetAsInteger("Dexterity");
	pResult.Vit = (WORD)this->m_CharDB.GetAsInteger("Vitality");
	pResult.Energy = (WORD)this->m_CharDB.GetAsInteger("Energy");
	pResult.Money = this->m_CharDB.GetAsInteger("Money");
	pResult.Life = (DWORD)this->m_CharDB.GetAsFloat("Life");
	pResult.MaxLife = (DWORD)this->m_CharDB.GetAsFloat("MaxLife");
	pResult.Mana = (DWORD)this->m_CharDB.GetAsFloat("Mana");
	pResult.MaxMana = (DWORD)this->m_CharDB.GetAsFloat("MaxMana");
	pResult.MapNumber = (BYTE)this->m_CharDB.GetAsInteger("MapNumber");
	pResult.MapX = (BYTE)this->m_CharDB.GetAsInteger("MapPosX");
	pResult.MapY = (BYTE)this->m_CharDB.GetAsInteger("MapPosY");
	pResult.Dir = (BYTE)this->m_CharDB.GetAsInteger("MapDir");
	pResult.PkCount = this->m_CharDB.GetAsInteger("PkCount");
	pResult.PkLevel = this->m_CharDB.GetAsInteger("PkLevel");
	pResult.PkTime = this->m_CharDB.GetAsInteger("PkTime");
	pResult.CtlCode = (BYTE)this->m_CharDB.GetAsInteger("CtlCode");
	pResult.Leadership = (WORD)this->m_CharDB.GetAsInteger("Leadership");
	pResult.ChatLitmitTime = (WORD)this->m_CharDB.GetAsInteger("ChatLimitTime");
	pResult.iFruitPoint = this->m_CharDB.GetAsInteger("FruitPoint");
	pResult.resets = this->m_CharDB.GetAsInteger("RESETS");
	pResult.InventoryExpansion = this->m_CharDB.GetAsInteger("InventoryExpansion");
	pResult.WarehouseExpansion = WHExpansion;
	pResult.WinDuels = this->m_CharDB.GetAsInteger("WinDuels");
	pResult.LoseDuels = this->m_CharDB.GetAsInteger("LoseDuels");
	pResult.ChatBlockTime = this->m_CharDB.GetAsInteger64("BlockChatTime");
	pResult.PenaltyMask = this->m_CharDB.GetAsInteger("PenaltyMask");
	pResult.Married = this->m_CharDB.GetAsInteger("Married");

	if (g_DSBattleCoreEnable == 1)
	{
		//pResult.Ruud = this->m_CharDB.GetAsInteger("Ruud");
	}
	else
	{
		pResult.Ruud = this->m_CharDB.GetAsInteger("Ruud");
	}

	this->m_CharDB.GetAsString("MarryName", pResult.MarryName, 10);
	this->m_CharDB.Close();

	char szTemp[256];
	BYTE btTemp[INVENTORY_BINARY_SIZE] = { 0 };
	memset(btTemp, 0xFF, sizeof(btTemp));

	wsprintf(szTemp, "SELECT Inventory from Character where Name='%s'", szName);
	int ret = this->m_CharDB.GetAsBinary(szTemp, btTemp, sizeof(btTemp));
	memcpy(pResult.dbInventory, btTemp, sizeof(pResult.dbInventory));
	memset(btTemp, 0, sizeof(btTemp));

	wsprintf(szTemp, "SELECT MagicList from Character where Name='%s'", szName);
	this->m_CharDB.GetAsBinary(szTemp, btTemp, sizeof(btTemp));
	memcpy(pResult.dbMagicList, btTemp, sizeof(pResult.dbMagicList));
	memset(btTemp, 0, sizeof(btTemp));

	wsprintf(szTemp, "SELECT Quest from Character where Name='%s'", szName);
	this->m_CharDB.GetAsBinary(szTemp, btTemp, sizeof(btTemp));
	memcpy(pResult.dbQuest, btTemp, sizeof(pResult.dbQuest));
	memset(btTemp, 0, sizeof(btTemp));

	QueryResult* res = this->m_CharDB.Fetch("SELECT AuthorityMask, DATEDIFF(DAY, GETDATE(), Expiry) AS ExpirationTime FROM T_GMSystem WHERE Name='%s'", szName);

	if (this->m_CharDB.Fetch() != SQL_NO_DATA)
	{
		int Days = this->m_CharDB.GetAsInteger("ExpirationTime");

		if (Days > 0)
		{
			pResult.GmCode = this->m_CharDB.GetAsInteger("AuthorityMask");
			pResult.GmExpDays = Days;
		}
		else
		{
			pResult.GmCode = 0;
			pResult.GmExpDays = 0;
			pResult.CtlCode = 0;
		}
	}
	else
	{
		pResult.GmCode = 0;
		pResult.GmExpDays = 0;
		pResult.CtlCode = 0;
	}

	this->m_CharDB.Close();
	pResult.result = 1;

	this->m_AccDB.ExecQuery("UPDATE AccountCharacter SET GameIDC='%s' WHERE Id='%s'", szName, szAccountID);

	this->m_WareUserData.AddUserData(szAccountID);
	//DataSend(aIndex, (LPBYTE)&pResult, sizeof(pResult), __FUNCTION__);

	if (pResult.result == 1)
	{
		SDHP_SKILLKEYDATA_SEND pSkillData = { 0 };

		PHeadSetB((LPBYTE)&pSkillData, 0x60, sizeof(pSkillData));
		pSkillData.aIndex = aRecv->Number;

		QueryResult* res = this->m_OptionDataDB.Fetch("SELECT * FROM OptionData WHERE Name='%s'", szName);

		if (this->m_OptionDataDB.Fetch() != SQL_NO_DATA)
		{
			memcpy(pSkillData.Name, szName, 10);
			pSkillData.GameOption = (BYTE)this->m_OptionDataDB.GetAsInteger("GameOption");
			pSkillData.QkeyDefine = (BYTE)this->m_OptionDataDB.GetAsInteger("Qkey");
			pSkillData.WkeyDefine = (BYTE)this->m_OptionDataDB.GetAsInteger("Wkey");
			pSkillData.EkeyDefine = (BYTE)this->m_OptionDataDB.GetAsInteger("Ekey");
			pSkillData.RkeyDefine = (BYTE)this->m_OptionDataDB.GetAsInteger("Rkey");
			pSkillData.ChatWindow = (BYTE)this->m_OptionDataDB.GetAsInteger("ChatWindow");
			pSkillData.QWERLevelDefine = this->m_OptionDataDB.GetAsInteger("QWERLevel");
			pSkillData.EnableTransformMode = this->m_OptionDataDB.GetAsInteger("EnableChangeMode");
			this->m_OptionDataDB.Close();

			BYTE btTemp[sizeof(pSkillData.SkillKeyBuffer)] = { 0 };
			char szTemp[256];
			wsprintf(szTemp, "SELECT SkillKey from OptionData where Name='%s'", szName);
			this->m_OptionDataDB.GetAsBinary(szTemp, btTemp, sizeof(btTemp));
			memcpy(pSkillData.SkillKeyBuffer, btTemp, 20);
		}

		else
		{
			this->m_OptionDataDB.Close();
		}

		//DataSend(aIndex, (LPBYTE)&pSkillData, sizeof(pSkillData), __FUNCTION__);
	}

	MUBOT_SETTINGS_SEND pMuBot = { 0 };
	pMuBot.h.c = PMHC_WORD;
	pMuBot.h.sizeH = SET_NUMBERH(sizeof(pMuBot));
	pMuBot.h.sizeL = SET_NUMBERL(sizeof(pMuBot));
	pMuBot.h.headcode = 0x61;

	strcpy(pMuBot.szName, szName);
	pMuBot.aIndex = aRecv->Number;
	GetMuBotData(szName, &pMuBot);
	//DataSend(aIndex, (LPBYTE)&pMuBot, sizeof(pMuBot), __FUNCTION__);
}

void CDataServerProtocol::GJSetCharacterInfo(int aIndex, SDHP_DBCHAR_INFOSAVE * aRecv)
{
	char szName[11] = { 0 };
	szName[10] = 0;
	memcpy(szName, aRecv->Name, 10);

	char szAccountID[11] = { 0 };
	szAccountID[10] = 0;
	memcpy(szAccountID, aRecv->AccountID, 10);

	if (g_DSBattleCoreEnable == 1)
	{
		this->m_CharDB.ExecQuery("UPDATE Character SET cLevel=%d, mLevel=%d, Class=%d,LevelUpPoint=%d, mlPoint=%d, Experience=%I64d, mlExperience=%I64d, mlNextExp=%I64d, Strength=%d, Dexterity=%d, Vitality=%d, Energy=%d, Money=%d, Life=%f, MaxLife=%f, Mana=%f, MaxMana=%f, MapNumber=%d, MapPosX=%d, MapPosY=%d, MapDir=%d, PkCount=%d, PkLevel=%d, PkTime=%d, Leadership=%d, ChatLimitTime=%d, FruitPoint=%d, RESETS = %d, Married = %d, MarryName = '%s', WinDuels=%d, LoseDuels=%d, BlockChatTime=%I64d, PenaltyMask=%d WHERE Name = '%s'",
			aRecv->Level, aRecv->mLevel, aRecv->Class, aRecv->LevelUpPoint, aRecv->mlPoint, aRecv->Exp, aRecv->mlExp, aRecv->mlNextExp, aRecv->Str, aRecv->Dex, aRecv->Vit, aRecv->Energy, aRecv->Money, float(aRecv->Life), float(aRecv->MaxLife), float(aRecv->Mana), float(aRecv->MaxMana), aRecv->MapNumber, aRecv->MapX, aRecv->MapY, aRecv->Dir, aRecv->PkCount, aRecv->PkLevel, aRecv->PkTime, aRecv->Leadership, aRecv->ChatLitmitTime, aRecv->iFruitPoint, aRecv->resets, aRecv->Married, aRecv->MarryName, aRecv->WinDuels, aRecv->LoseDuels, aRecv->ChatBlockTime, aRecv->PenaltyMask, szName);
	}
	else
	{
		this->m_CharDB.ExecQuery("UPDATE Character SET cLevel=%d, mLevel=%d, Class=%d,LevelUpPoint=%d, mlPoint=%d, Experience=%I64d, mlExperience=%I64d, mlNextExp=%I64d, Strength=%d, Dexterity=%d, Vitality=%d, Energy=%d, Money=%d, Life=%f, MaxLife=%f, Mana=%f, MaxMana=%f, MapNumber=%d, MapPosX=%d, MapPosY=%d, MapDir=%d, PkCount=%d, PkLevel=%d, PkTime=%d, Leadership=%d, ChatLimitTime=%d, FruitPoint=%d, RESETS = %d, Married = %d, MarryName = '%s', WinDuels=%d, LoseDuels=%d, BlockChatTime=%I64d, PenaltyMask=%d, Ruud=%d WHERE Name = '%s'",
			aRecv->Level, aRecv->mLevel, aRecv->Class, aRecv->LevelUpPoint, aRecv->mlPoint, aRecv->Exp, aRecv->mlExp, aRecv->mlNextExp, aRecv->Str, aRecv->Dex, aRecv->Vit, aRecv->Energy, aRecv->Money, float(aRecv->Life), float(aRecv->MaxLife), float(aRecv->Mana), float(aRecv->MaxMana), aRecv->MapNumber, aRecv->MapX, aRecv->MapY, aRecv->Dir, aRecv->PkCount, aRecv->PkLevel, aRecv->PkTime, aRecv->Leadership, aRecv->ChatLitmitTime, aRecv->iFruitPoint, aRecv->resets, aRecv->Married, aRecv->MarryName, aRecv->WinDuels, aRecv->LoseDuels, aRecv->ChatBlockTime, aRecv->PenaltyMask, aRecv->Ruud, szName);
	}

	char szTemp[128];
	wsprintf(szTemp, "UPDATE Character SET Inventory=? WHERE Name='%s'", szName);
	this->m_CharDB.SetAsBinary(szTemp, aRecv->dbInventory, sizeof(aRecv->dbInventory));

	wsprintf(szTemp, "UPDATE Character SET MagicList=? WHERE Name='%s'", szName);
	this->m_CharDB.SetAsBinary(szTemp, aRecv->dbMagicList, sizeof(aRecv->dbMagicList));

	wsprintf(szTemp, "UPDATE Character SET Quest=? WHERE Name='%s'", szName);
	this->m_CharDB.SetAsBinary(szTemp, aRecv->dbQuest, sizeof(aRecv->dbQuest));

	if (aRecv->CharInfoSave == 1)
	{
		this->m_CharDB.ExecQuery("INSERT INTO T_CurCharName (Name) VALUES('%s')", szName);
		this->m_CharDB.Close();
	}
}

void CDataServerProtocol::GDUserItemSave(int aIndex, SDHP_DBCHAR_ITEMSAVE * aRecv)
{
	char szName[11] = { 0 };
	szName[10] = 0;
	memcpy(szName, aRecv->Name, 10);

	char szTemp[128];
	wsprintf(szTemp, "UPDATE Character SET Inventory=? WHERE Name='%s'", szName);
	this->m_CharDB.SetAsBinary(szTemp, aRecv->dbInventory, sizeof(aRecv->dbInventory));
}

void CDataServerProtocol::ItemSerialCreateRecv(int aIndex, SDHP_ITEMCREATE * aRecv)
{
	SDHP_ITEMCREATERECV pResult;

	PHeadSetB((LPBYTE)&pResult, 0x52, sizeof(pResult));
	pResult.aIndex = aRecv->aIndex;
	pResult.Dur = aRecv->Dur;
	pResult.Level = aRecv->Level;
	pResult.lootindex = aRecv->lootindex;
	pResult.m_Number = g_ItemSerial.GenerateSerial();
	pResult.MapNumber = aRecv->MapNumber;
	pResult.NewOption = aRecv->NewOption;
	pResult.Op1 = aRecv->Op1;
	pResult.Op2 = aRecv->Op2;
	pResult.Op3 = aRecv->Op3;
	pResult.SetOption = aRecv->SetOption;
	pResult.Type = aRecv->Type;
	pResult.x = aRecv->x;
	pResult.y = aRecv->y;
	pResult.lDuration = aRecv->lDuration;
	memcpy(&pResult.SocketOption, aRecv->SocketOption, sizeof(pResult.SocketOption));
	pResult.MainAttribute = aRecv->MainAttribute;

	//DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size, __FUNCTION__);
}


void CDataServerProtocol::PetItemSerialCreateRecv(int aIndex, SDHP_ITEMCREATE * aRecv)
{
	SDHP_ITEMCREATERECV pResult;

	PHeadSetB((LPBYTE)&pResult, 0x52, sizeof(pResult));
	pResult.aIndex = aRecv->aIndex;
	pResult.Dur = aRecv->Dur;
	pResult.Level = aRecv->Level;
	pResult.lootindex = aRecv->lootindex;
	pResult.m_Number = g_ItemSerial.GenerateSerial();

	this->m_PetDB.ExecQuery("INSERT INTO T_PetItem_Info (ItemSerial, Pet_Level, Pet_Exp) VALUES (%I64d, %d, %I64d)", pResult.m_Number, 1, 0);
	this->m_PetDB.Close();

	pResult.MapNumber = aRecv->MapNumber;
	pResult.NewOption = aRecv->NewOption;
	pResult.Op1 = aRecv->Op1;
	pResult.Op2 = aRecv->Op2;
	pResult.Op3 = aRecv->Op3;
	pResult.SetOption = aRecv->SetOption;
	pResult.Type = aRecv->Type;
	pResult.x = aRecv->x;
	pResult.y = aRecv->y;
	memcpy(&pResult.SocketOption, aRecv->SocketOption, sizeof(pResult.SocketOption));
	pResult.lDuration = aRecv->lDuration;
	pResult.MainAttribute = aRecv->MainAttribute;

	//DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size, __FUNCTION__);
}

void CDataServerProtocol::DGRecvPetItemInfo(int aIndex, SDHP_REQUEST_PETITEM_INFO * aRecv)
{
	char szAccountID[11] = { 0 };
	szAccountID[10] = 0;
	memcpy(szAccountID, aRecv->AccountID, 10);
	int lOfs1 = sizeof(SDHP_REQUEST_PETITEM_INFO);
	int lOfs2 = sizeof(SDHP_RECV_PETITEM_INFO);
	char cBUFFER[4092] = { 0 };
	SDHP_RECV_PETITEM_INFO * pRecvPetInfoCount = (SDHP_RECV_PETITEM_INFO *)(cBUFFER);
	Request_PetItem_Info * pRequestPetInfo;
	Recv_PetItem_Info * pRecvPetInfo;

	if (aRecv->ServerType == 0) // normal GS
	{
		for (int i = 0; i<aRecv->nCount; i++)
		{
			pRequestPetInfo = (Request_PetItem_Info *)((LPBYTE)aRecv + lOfs1);
			pRecvPetInfo = (Recv_PetItem_Info *)((LPBYTE)cBUFFER + lOfs2);

			QueryResult* res = this->m_PetDB.Fetch("SELECT Pet_Level, Pet_Exp FROM T_PetItem_Info WHERE ItemSerial=%I64d",
				pRequestPetInfo->nSerial);

			if (this->m_PetDB.Fetch() == SQL_NO_DATA)
			{
				this->m_PetDB.Close();
				this->m_PetDB.ExecQuery("INSERT INTO T_PetItem_Info (ItemSerial, Pet_Level, Pet_Exp) VALUES (%I64d, %d, %I64d)",
					pRequestPetInfo->nSerial, 1, 0);
				this->m_PetDB.Close();

				QueryResult* res = this->m_PetDB.Fetch("SELECT Pet_Level, Pet_Exp FROM T_PetItem_Info WHERE ItemSerial=%I64d",
					pRequestPetInfo->nSerial);
				this->m_PetDB.Fetch();
			}

			pRecvPetInfo->Level = this->m_PetDB.GetAsInteger("Pet_Level");
			pRecvPetInfo->Exp = this->m_PetDB.GetAsInteger64("Pet_Exp");
			pRecvPetInfo->nPos = pRequestPetInfo->nPos;
			pRecvPetInfo->nSerial = pRequestPetInfo->nSerial;

			this->m_PetDB.Close();
			pRecvPetInfoCount->nCount++;

			lOfs1 += sizeof(Request_PetItem_Info);
			lOfs2 += sizeof(Recv_PetItem_Info);
		}
	}

	else // battlecore GS
	{
		for (int i = 0; i<aRecv->nCount; i++)
		{
			pRequestPetInfo = (Request_PetItem_Info *)((LPBYTE)aRecv + lOfs1);
			pRecvPetInfo = (Recv_PetItem_Info *)((LPBYTE)cBUFFER + lOfs2);

			this->m_PetDB.ExecQuery("EXEC IGC_BattleCore_LoadPetItemInfo %I64d, %d",
				pRequestPetInfo->nSerial, aRecv->ServerCode);

			if (this->m_PetDB.Fetch() == SQL_NO_DATA)
			{
				this->m_PetDB.Close();
				this->m_PetDB.ExecQuery("EXEC IGC_BattleCore_SavePetItemInfo %I64d, %d, %d, %I64d",
					pRequestPetInfo->nSerial, aRecv->ServerCode, 1, 0);
				this->m_PetDB.Close();

				this->m_PetDB.ExecQuery("EXEC IGC_BattleCore_LoadPetItemInfo %I64d, %d",
					pRequestPetInfo->nSerial, aRecv->ServerCode);
				this->m_PetDB.Fetch();
			}

			pRecvPetInfo->Level = this->m_PetDB.GetAsInteger("Pet_Level");
			pRecvPetInfo->Exp = this->m_PetDB.GetAsInteger64("Pet_Exp");
			pRecvPetInfo->nPos = pRequestPetInfo->nPos;
			pRecvPetInfo->nSerial = pRequestPetInfo->nSerial;

			this->m_PetDB.Close();
			pRecvPetInfoCount->nCount++;

			lOfs1 += sizeof(Request_PetItem_Info);
			lOfs2 += sizeof(Recv_PetItem_Info);
		}
	}

	PHeadSetW((LPBYTE)pRecvPetInfoCount, 0x56, sizeof(SDHP_RECV_PETITEM_INFO) + pRecvPetInfoCount->nCount * sizeof(Recv_PetItem_Info));
	//pRecvPetInfoCount->nCount = aRecv->nCount;
	pRecvPetInfoCount->InvenType = aRecv->InvenType;
	pRecvPetInfoCount->Number = aRecv->Number;
	memcpy(pRecvPetInfoCount->AccountID, szAccountID, 11);

	//DataSend(aIndex, (LPBYTE)cBUFFER, sizeof(SDHP_RECV_PETITEM_INFO) + pRecvPetInfoCount->nCount * sizeof(Recv_PetItem_Info), __FUNCTION__);
}

void CDataServerProtocol::GDSavePetItemInfo(int aIndex, SDHP_SAVE_PETITEM_INFO * aRecv)
{
	int lOfs = sizeof(SDHP_SAVE_PETITEM_INFO);

	Save_PetItem_Info * pSavePetInfo = (Save_PetItem_Info *)((LPBYTE)aRecv + lOfs);

	if (aRecv->ServerType == 0) // normal gs
	{
		for (int i = 0; i<aRecv->nCount; i++)
		{
			this->m_PetDB.ExecQuery("UPDATE T_PetItem_Info SET Pet_Level=%d, Pet_Exp=%I64d WHERE ItemSerial=%I64d",
				pSavePetInfo[i].Level, pSavePetInfo[i].Exp, pSavePetInfo[i].nSerial);
			this->m_PetDB.Close();
		}
	}

	else // battlecore gs
	{
		for (int i = 0; i<aRecv->nCount; i++)
		{
			this->m_PetDB.ExecQuery("EXEC IGC_BattleCore_SavePetItemInfo %I64d, %d, %d, %I64d",
				pSavePetInfo[i].nSerial, aRecv->ServerCode, pSavePetInfo[i].Level, pSavePetInfo[i].Exp);
			this->m_PetDB.Close();
		}
	}
}

void CDataServerProtocol::DGOptionDataRecv(int aIndex, SDHP_SKILLKEYDATA * aRecv)
{
	char szName[11] = { 0 };
	szName[10] = 0;
	memcpy(szName, aRecv->Name, 11);

	QueryResult* res = this->m_OptionDataDB.Fetch("SELECT Name FROM OptionData WHERE Name='%s'", szName);

	if (this->m_OptionDataDB.Fetch() == SQL_NO_DATA)
	{
		this->m_OptionDataDB.Close();
		this->m_OptionDataDB.ExecQuery("INSERT INTO OptionData (Name, GameOption, Qkey, Wkey, Ekey, ChatWindow, Rkey, QWERLevel, EnableChangeMode) VALUES ('%s', %d, %d, %d, %d, %d, %d, %d, %d)",
			szName, aRecv->GameOption, aRecv->QkeyDefine, aRecv->WkeyDefine, aRecv->EkeyDefine, aRecv->ChatWindow, aRecv->RkeyDefine, aRecv->QWERLevelDefine, aRecv->EnableTransformMode);
	}
	else
	{
		this->m_OptionDataDB.Close();
		this->m_OptionDataDB.ExecQuery("UPDATE OptionData SET GameOption=%d, Qkey=%d, Wkey=%d, Ekey=%d, ChatWindow=%d, Rkey=%d, QWERLevel=%d, EnableChangeMode=%d WHERE Name='%s'",
			aRecv->GameOption, aRecv->QkeyDefine, aRecv->WkeyDefine, aRecv->EkeyDefine, aRecv->ChatWindow, aRecv->RkeyDefine, aRecv->QWERLevelDefine, aRecv->EnableTransformMode, szName);
	}

	char szTemp[100];
	wsprintf(szTemp, "UPDATE OptionData SET SkillKey=? where Name='%s'", szName);
	this->m_OptionDataDB.SetAsBinary(szTemp, aRecv->SkillKeyBuffer, sizeof(aRecv->SkillKeyBuffer));
}

void CDataServerProtocol::DGMoveOtherServer(int aIndex, SDHP_CHARACTER_TRANSFER * aRecv)
{
	sLog->outError("[MuOnlineDB] Error Character Transfer is not longer avaible");
}

void CDataServerProtocol::GDDeleteTempUserInfo(int aIndex, SDHP_DELETE_TEMPUSERINFO * aRecv)
{
	SDHP_DELETE_TEMPUSERINFO_RESULT pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xCE;
	pMsg.h.size = sizeof(pMsg);

	memcpy(pMsg.CharName, aRecv->CharName, MAX_ACCOUNT_LEN + 1);

	for (int i = 0; i<g_dwMaxServerGroups; i++)
	{
		if (g_Server[i].m_Index != -1
			&& g_Server[i].m_Type == g_Server[aIndex].m_Type
			&& g_Server[i].m_ServerCode == aRecv->ServerCode)
		{
			//DataSend(i, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
			return;
		}
	}
}

void CDataServerProtocol::GS_DGAnsCastleInitData(int aIndex, CSP_REQ_CSINITDATA * aRecv)
{
	BYTE SendData[sizeof(CSP_ANS_CSINITDATA) + (sizeof(CSP_CSINITDATA) * MAX_CS_NPC)] = { 0 };

	CSP_ANS_CSINITDATA * lpMsg = (CSP_ANS_CSINITDATA *)SendData;
	CSP_CSINITDATA * lpMsgBody = (CSP_CSINITDATA *)&SendData[sizeof(CSP_ANS_CSINITDATA)];

	lpMsg->h.c = 0xC2;
	lpMsg->h.headcode = 0x81;
	lpMsg->wMapSvrNum = aRecv->wMapSvrNum;
	lpMsg->iCount = 0;
	lpMsg->iResult = 0;

	int iCount = 0;

	if (this->m_CastleDB.ExecQuery("EXEC WZ_CS_GetCastleTotalInfo %d, %d", aRecv->wMapSvrNum, aRecv->iCastleEventCycle) == TRUE && this->m_CastleDB.Fetch() != SQL_NO_DATA) {

		lpMsg->iResult = TRUE;

		lpMsg->wStartYear = (WORD)this->m_CastleDB.GetAsInteger("SYEAR");
		lpMsg->btStartMonth = (BYTE)this->m_CastleDB.GetAsInteger("SMONTH");
		lpMsg->btStartDay = (BYTE)this->m_CastleDB.GetAsInteger("SDAY");
		lpMsg->wEndYear = (WORD)this->m_CastleDB.GetAsInteger("EYEAR");
		lpMsg->btEndMonth = (BYTE)this->m_CastleDB.GetAsInteger("EMONTH");
		lpMsg->btEndDay = (BYTE)this->m_CastleDB.GetAsInteger("EDAY");
		lpMsg->btIsSiegeGuildList = (BYTE)this->m_CastleDB.GetAsInteger("SIEGE_GUILDLIST_SETTED");
		lpMsg->btIsSiegeEnded = (BYTE)this->m_CastleDB.GetAsInteger("SIEGE_ENDED");
		lpMsg->btIsCastleOccupied = (BYTE)this->m_CastleDB.GetAsInteger("CASTLE_OCCUPY");
		lpMsg->i64CastleMoney = this->m_CastleDB.GetAsInteger64("MONEY");
		lpMsg->iTaxRateChaos = this->m_CastleDB.GetAsInteger("TAX_RATE_CHAOS");
		lpMsg->iTaxRateStore = this->m_CastleDB.GetAsInteger("TAX_RATE_STORE");
		lpMsg->iTaxHuntZone = this->m_CastleDB.GetAsInteger("TAX_HUNT_ZONE");
		lpMsg->iFirstCreate = this->m_CastleDB.GetAsInteger("FIRST_CREATE");
		this->m_CastleDB.GetAsString("OWNER_GUILD", lpMsg->szCastleOwnGuild, sizeof(lpMsg->szCastleOwnGuild));

		this->m_CastleDB.Close();

		if (this->m_CastleDB.ExecQuery("EXEC WZ_CS_GetCastleNpcInfo %d", aRecv->wMapSvrNum) == TRUE && this->m_CastleDB.Fetch() != SQL_NO_DATA) {
			do
			{
				lpMsgBody[iCount].iNpcNumber = this->m_CastleDB.GetAsInteger("NPC_NUMBER");
				lpMsgBody[iCount].iNpcIndex = this->m_CastleDB.GetAsInteger("NPC_INDEX");
				lpMsgBody[iCount].iNpcDfLevel = this->m_CastleDB.GetAsInteger("NPC_DF_LEVEL");
				lpMsgBody[iCount].iNpcRgLevel = this->m_CastleDB.GetAsInteger("NPC_RG_LEVEL");
				lpMsgBody[iCount].iNpcMaxHp = this->m_CastleDB.GetAsInteger("NPC_MAXHP");
				lpMsgBody[iCount].iNpcHp = this->m_CastleDB.GetAsInteger("NPC_HP");
				lpMsgBody[iCount].btNpcX = this->m_CastleDB.GetAsInteger("NPC_X");
				lpMsgBody[iCount].btNpcY = this->m_CastleDB.GetAsInteger("NPC_Y");
				lpMsgBody[iCount].btNpcDIR = this->m_CastleDB.GetAsInteger("NPC_DIR");
			} while ((iCount++) < MAX_CS_NPC && this->m_CastleDB.Fetch() != SQL_NO_DATA);

			lpMsg->iCount = iCount;
		}
	}

	this->m_CastleDB.Close();

	int size = sizeof(CSP_ANS_CSINITDATA) + (sizeof(CSP_CSINITDATA)*iCount);

	lpMsg->h.sizeH = SET_NUMBERH(size);
	lpMsg->h.sizeL = SET_NUMBERL(size);

	//DataSend(aIndex, (PBYTE)SendData, size, __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsOwnerGuildMaster(int aIndex, CSP_REQ_OWNERGUILDMASTER * aRecv)
{
	CSP_ANS_OWNERGUILDMASTER pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x01;
	pMsg.iResult = 0;

	pMsg.wMapSvrNum = aRecv->wMapSvrNum;
	pMsg.iIndex = aRecv->iIndex;

	if (this->m_CastleDB.ExecQuery("EXEC WZ_CS_GetOwnerGuildMaster %d", aRecv->wMapSvrNum) == TRUE && this->m_CastleDB.Fetch() != SQL_NO_DATA)
	{
		pMsg.iResult = this->m_CastleDB.GetAsInteger("QueryResult");
		this->m_CastleDB.GetAsString("OwnerGuild", pMsg.szCastleOwnGuild, sizeof(pMsg.szCastleOwnGuild));
		this->m_CastleDB.GetAsString("OwnerGuildMaster", pMsg.szCastleOwnGuildMaster, sizeof(pMsg.szCastleOwnGuildMaster));
	}

	this->m_CastleDB.Close();
	//DataSend(aIndex, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsCastleNpcBuy(int aIndex, CSP_REQ_NPCBUY * aRecv)
{
	CSP_ANS_NPCBUY pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x03;
	pMsg.iResult = 0;

	pMsg.iIndex = aRecv->iIndex;
	pMsg.wMapSvrNum = aRecv->wMapSvrNum;
	pMsg.iBuyCost = aRecv->iBuyCost;
	pMsg.iNpcIndex = aRecv->iNpcIndex;
	pMsg.iNpcNumber = aRecv->iNpcNumber;

	if (this->m_CastleDB.ExecQuery("EXEC WZ_CS_ReqNpcBuy %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
		aRecv->wMapSvrNum, aRecv->iNpcNumber, aRecv->iNpcIndex, aRecv->iNpcDfLevel, aRecv->iNpcRgLevel, aRecv->iNpcMaxHp, aRecv->iNpcHp, aRecv->btNpcX, aRecv->btNpcY, aRecv->btNpcDIR) == TRUE && this->m_CastleDB.Fetch() != SQL_NO_DATA)
	{
		pMsg.iResult = this->m_CastleDB.GetAsInteger("QueryResult");
	}

	this->m_CastleDB.Close();
	//DataSend(aIndex, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsCastleNpcRepair(int aIndex, CSP_REQ_NPCREPAIR * aRecv)
{
	CSP_ANS_NPCREPAIR pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x04;
	pMsg.iResult = 0;

	pMsg.iIndex = aRecv->iIndex;
	pMsg.wMapSvrNum = aRecv->wMapSvrNum;
	pMsg.iNpcIndex = aRecv->iNpcIndex;
	pMsg.iNpcNumber = aRecv->iNpcNumber;
	pMsg.iRepairCost = aRecv->iRepairCost;

	if (this->m_CastleDB.ExecQuery("EXEC WZ_CS_ReqNpcRepair %d, %d, %d", aRecv->wMapSvrNum, aRecv->iNpcNumber, aRecv->iNpcIndex) == TRUE && this->m_CastleDB.Fetch() != SQL_NO_DATA)
	{
		pMsg.iResult = this->m_CastleDB.GetAsInteger("QueryResult");
		pMsg.iNpcHpl = this->m_CastleDB.GetAsInteger("NPC_HP");
		pMsg.iNpcMaxHp = this->m_CastleDB.GetAsInteger("NPC_MAXHP");
	}

	this->m_CastleDB.Close();
	//DataSend(aIndex, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsCastleNpcUpgrade(int aIndex, CSP_REQ_NPCUPGRADE * aRecv)
{
	CSP_ANS_NPCUPGRADE pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x05;
	pMsg.iResult = 0;

	pMsg.iIndex = aRecv->iIndex;
	pMsg.wMapSvrNum = aRecv->wMapSvrNum;

	pMsg.iNpcIndex = aRecv->iNpcIndex;
	pMsg.iNpcNumber = aRecv->iNpcNumber;
	pMsg.iNpcUpIndex = aRecv->iNpcUpIndex;
	pMsg.iNpcUpType = aRecv->iNpcUpType;
	pMsg.iNpcUpValue = aRecv->iNpcUpValue;

	if (this->m_CastleDB.ExecQuery("EXEC WZ_CS_ReqNpcUpgrade %d, %d, %d, %d, %d",
		aRecv->wMapSvrNum, aRecv->iNpcNumber, aRecv->iNpcIndex, aRecv->iNpcUpType, aRecv->iNpcUpValue) == TRUE && this->m_CastleDB.Fetch() != SQL_NO_DATA)
	{
		pMsg.iResult = this->m_CastleDB.GetAsInteger("QueryResult");
	}

	this->m_CastleDB.Close();
	//DataSend(aIndex, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsTaxInfo(int aIndex, CSP_REQ_TAXINFO * aRecv)
{
	CSP_ANS_TAXINFO pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x06;
	pMsg.iResult = 0;

	pMsg.iIndex = aRecv->iIndex;
	pMsg.wMapSvrNum = aRecv->wMapSvrNum;

	if (this->m_CastleDB.ExecQuery("EXEC WZ_CS_GetCastleTaxInfo %d", aRecv->wMapSvrNum) == TRUE && this->m_CastleDB.Fetch() != SQL_NO_DATA) {
		pMsg.iResult = 1;

		pMsg.i64CastleMoney = this->m_CastleDB.GetAsInteger64("MONEY");
		pMsg.iTaxRateChaos = this->m_CastleDB.GetAsInteger("TAX_RATE_CHAOS");
		pMsg.iTaxRateStore = this->m_CastleDB.GetAsInteger("TAX_RATE_STORE");
		pMsg.iTaxHuntZone = this->m_CastleDB.GetAsInteger("TAX_HUNT_ZONE");
	}

	this->m_CastleDB.Close();
	//DataSend(aIndex, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsTaxRateChange(int aIndex, CSP_REQ_TAXRATECHANGE * aRecv)
{
	CSP_ANS_TAXRATECHANGE pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x07;
	pMsg.iResult = 0;

	pMsg.iIndex = aRecv->iIndex;
	pMsg.wMapSvrNum = aRecv->wMapSvrNum;

	if (this->m_CastleDB.ExecQuery("EXEC WZ_CS_ModifyTaxRate %d, %d, %d",
		aRecv->wMapSvrNum, aRecv->iTaxKind, aRecv->iTaxRate) == TRUE && this->m_CastleDB.Fetch() != SQL_NO_DATA)
	{
		pMsg.iResult = this->m_CastleDB.GetAsInteger("QueryResult");
		pMsg.iTaxKind = this->m_CastleDB.GetAsInteger("TaxKind");
		pMsg.iTaxRate = this->m_CastleDB.GetAsInteger("TaxRate");
	}

	this->m_CastleDB.Close();
	//DataSend(aIndex, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsCastleMoneyChange(int aIndex, CSP_REQ_MONEYCHANGE * aRecv)
{
	CSP_ANS_MONEYCHANGE pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x08;
	pMsg.iResult = 0;

	pMsg.iIndex = aRecv->iIndex;
	pMsg.wMapSvrNum = aRecv->wMapSvrNum;
	pMsg.iMoneyChanged = aRecv->iMoneyChanged;

	if (this->m_CastleDB.ExecQuery("EXEC WZ_CS_ModifyMoney %d, %d", aRecv->wMapSvrNum, aRecv->iMoneyChanged) == TRUE && this->m_CastleDB.Fetch() != SQL_NO_DATA)
	{
		pMsg.iResult = this->m_CastleDB.GetAsInteger("QueryResult");
		pMsg.i64CastleMoney = this->m_CastleDB.GetAsInteger64("MONEY");
	}

	this->m_CastleDB.Close();
	//DataSend(aIndex, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsSiegeDateChange(int aIndex, CSP_REQ_SDEDCHANGE * aRecv)
{
	CSP_ANS_SDEDCHANGE pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x09;
	pMsg.iResult = 0;

	pMsg.iIndex = aRecv->iIndex;
	pMsg.wMapSvrNum = aRecv->wMapSvrNum;

	pMsg.wStartYear = aRecv->wStartYear;
	pMsg.btStartMonth = aRecv->btStartMonth;
	pMsg.btStartDay = aRecv->btStartDay;
	pMsg.wEndYear = aRecv->wEndYear;
	pMsg.btEndMonth = aRecv->btEndMonth;
	pMsg.btEndDay = aRecv->btEndDay;

	if (this->m_CastleDB.ExecQuery("EXEC WZ_CS_ModifyCastleSchedule %d, '%d-%d-%d 00:00:00', '%d-%d-%d 00:00:00'",
		aRecv->wMapSvrNum, aRecv->wStartYear, aRecv->btStartMonth, aRecv->btStartDay,
		aRecv->wEndYear, aRecv->btEndMonth, aRecv->btEndDay) == TRUE && this->m_CastleDB.Fetch() != SQL_NO_DATA)
	{
		pMsg.iResult = this->m_CastleDB.GetAsInteger("QueryResult");
	}

	this->m_CastleDB.Close();
	//DataSend(aIndex, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsGuildMarkRegInfo(int aIndex, CSP_REQ_GUILDREGINFO * aRecv)
{
	CSP_ANS_GUILDREGINFO pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x0A;
	pMsg.iResult = 0;

	pMsg.iIndex = aRecv->iIndex;
	pMsg.wMapSvrNum = aRecv->wMapSvrNum;

	char szGuildName[9] = { 0 };
	memcpy(szGuildName, aRecv->szGuildName, 8);

	if (this->m_CastleDB.ExecQuery("EXEC WZ_CS_GetGuildMarkRegInfo %d, '%s'", aRecv->wMapSvrNum, szGuildName) == TRUE)
	{
		if (this->m_CastleDB.Fetch() == SQL_NO_DATA)
		{
			pMsg.iRegMarkCount = 0;
			pMsg.iResult = 2;
		}
		else
		{
			pMsg.iResult = 1;
			pMsg.iRegMarkCount = this->m_CastleDB.GetAsInteger("REG_MARKS");
			g_Log.Add("%d regmarks", pMsg.iRegMarkCount);
			pMsg.btRegRank = this->m_CastleDB.GetAsInteger("SEQ_NUM");
			pMsg.bIsGiveUp = this->m_CastleDB.GetAsInteger("IS_GIVEUP");
		}
	}

	this->m_CastleDB.Close();
	//DataSend(aIndex, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsSiegeEndedChange(int aIndex, CSP_REQ_SIEGEENDCHANGE * aRecv)
{
	CSP_ANS_SIEGEENDCHANGE pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x0B;
	pMsg.iResult = 0;

	pMsg.wMapSvrNum = aRecv->wMapSvrNum;
	pMsg.bIsSiegeEnded = aRecv->bIsSiegeEnded;

	if (this->m_CastleDB.ExecQuery("EXEC WZ_CS_ModifySiegeEnd %d, %d", aRecv->wMapSvrNum, aRecv->bIsSiegeEnded) == TRUE && this->m_CastleDB.Fetch())
	{
		pMsg.iResult = this->m_CastleDB.GetAsInteger("QueryResult");
	}

	this->m_CastleDB.Close();
	//DataSend(aIndex, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsCastleOwnerChange(int aIndex, CSP_REQ_CASTLEOWNERCHANGE * aRecv)
{
	CSP_ANS_CASTLEOWNERCHANGE pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x0C;
	pMsg.iResult = 0;

	pMsg.wMapSvrNum = aRecv->wMapSvrNum;
	pMsg.bIsCastleOccupied = aRecv->bIsCastleOccupied;

	char szOwnerGuildName[9];
	szOwnerGuildName[8] = 0;
	memcpy(szOwnerGuildName, aRecv->szOwnerGuildName, 0x08);
	memcpy(pMsg.szOwnerGuildName, aRecv->szOwnerGuildName, 0x08);

	if (this->m_CastleDB.ExecQuery("EXEC WZ_CS_ModifyCastleOwnerInfo %d, %d, '%s'", aRecv->wMapSvrNum, aRecv->bIsCastleOccupied, szOwnerGuildName) == TRUE && this->m_CastleDB.Fetch() != SQL_NO_DATA)
	{
		pMsg.iResult = this->m_CastleDB.GetAsInteger("QueryResult");
	}

	this->m_CastleDB.Close();
	//DataSend(aIndex, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsRegAttackGuild(int aIndex, CSP_REQ_REGATTACKGUILD * aRecv)
{
	CSP_ANS_REGATTACKGUILD pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x0D;
	pMsg.iResult = 0;

	pMsg.iIndex = aRecv->iIndex;
	pMsg.wMapSvrNum = aRecv->wMapSvrNum;

	char szEnemyGuildName[9] = { 0 };
	szEnemyGuildName[8] = 0;
	memcpy(szEnemyGuildName, aRecv->szEnemyGuildName, 0x08);
	memcpy(pMsg.szEnemyGuildName, aRecv->szEnemyGuildName, 0x08);

	if (this->m_CastleDB.ExecQuery("EXEC WZ_CS_ReqRegAttackGuild %d, '%s'", aRecv->wMapSvrNum, szEnemyGuildName) == TRUE && this->m_CastleDB.Fetch() != SQL_NO_DATA)
	{
		pMsg.iResult = this->m_CastleDB.GetAsInteger("QueryResult");
	}

	this->m_CastleDB.Close();
	//DataSend(aIndex, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsRestartCastleState(int aIndex, CSP_REQ_CASTLESIEGEEND * aRecv)
{
	CSP_ANS_CASTLESIEGEEND pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x0E;
	pMsg.iResult = 0;

	pMsg.wMapSvrNum = aRecv->wMapSvrNum;

	if (this->m_CastleDB.ExecQuery("EXEC WZ_CS_ResetCastleSiege %d", aRecv->wMapSvrNum) == TRUE && this->m_CastleDB.Fetch() != SQL_NO_DATA)
	{
		pMsg.iResult = this->m_CastleDB.GetAsInteger("QueryResult");
	}

	this->m_CastleDB.Close();
	//DataSend(aIndex, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsMapSvrMsgMultiCast(int aIndex, CSP_REQ_MAPSVRMULTICAST * aRecv)
{
	CSP_ANS_MAPSVRMULTICAST pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x0F;

	pMsg.wMapSvrNum = aRecv->wMapSvrNum;

	memcpy(pMsg.szMsgText, aRecv->szMsgText, 0x80);

	for (int i = 0; i<g_dwMaxServerGroups; i++)
	{
		if (g_Server[i].m_Index != -1
			&& g_Server[i].m_Type == g_Server[aIndex].m_Type
			&& g_Server[i].m_ServerGroup == aRecv->wMapSvrNum)
		{
			//DataSend(i, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
		}
	}
}

void CDataServerProtocol::GS_DGAnsGlobalPostMultiCast(int aIndex, CSP_GLOBALPOST_MULTICAST * aRecv)
{
	CSP_GLOBALPOST_MULTICAST pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x1F;

	pMsg.wMapSvrNum = aRecv->wMapSvrNum;

	memcpy(pMsg.szSender, aRecv->szSender, sizeof(pMsg.szSender));
	memcpy(pMsg.szMessage, aRecv->szMessage, sizeof(pMsg.szMessage));
	memcpy(pMsg.szServerName, aRecv->szServerName, sizeof(pMsg.szServerName));
	memcpy(pMsg.btColorRGB, aRecv->btColorRGB, sizeof(pMsg.btColorRGB));

	for (int i = 0; i < g_dwMaxServerGroups; i++)
	{
		if (g_Server[i].m_Index != -1
			&& g_Server[i].m_Type == g_Server[aIndex].m_Type
			&& g_Server[i].m_ServerGroup == aRecv->wMapSvrNum)
		{
			//DataSend(i, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
		}
	}
}

void CDataServerProtocol::GS_DGAnsRegGuildMark(int aIndex, CSP_REQ_GUILDREGMARK * aRecv)
{
	CSP_ANS_GUILDREGMARK pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x10;
	pMsg.iResult = 0;

	pMsg.iIndex = aRecv->iIndex;
	pMsg.wMapSvrNum = aRecv->wMapSvrNum;
	pMsg.iItemPos = aRecv->iItemPos;

	char szGuildName[9] = { 0 };
	szGuildName[8] = 0;

	memcpy(szGuildName, aRecv->szGuildName, 0x08);
	memcpy(pMsg.szGuildName, aRecv->szGuildName, 0x08);

	if (this->m_CastleDB.ExecQuery("EXEC WZ_CS_ReqRegGuildMark %d, '%s'", aRecv->wMapSvrNum, szGuildName) == TRUE && this->m_CastleDB.Fetch() != SQL_NO_DATA)
	{
		pMsg.iResult = this->m_CastleDB.GetAsInteger("QueryResult");
		pMsg.iRegMarkCount = this->m_CastleDB.GetAsInteger("REG_MARKS");
	}

	this->m_CastleDB.Close();
	//DataSend(aIndex, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsGuildMarkReset(int aIndex, CSP_REQ_GUILDRESETMARK * aRecv)
{
	CSP_ANS_GUILDRESETMARK pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x11;
	pMsg.iResult = 0;

	pMsg.iIndex = aRecv->iIndex;
	pMsg.wMapSvrNum = aRecv->wMapSvrNum;

	char szGuildName[9] = { 0 };
	szGuildName[8] = 0;

	memcpy(szGuildName, aRecv->szGuildName, 0x08);
	memcpy(pMsg.szGuildName, aRecv->szGuildName, 0x08);

	if (this->m_CastleDB.ExecQuery("EXEC WZ_CS_ModifyGuildMarkReset %d, '%s'", aRecv->wMapSvrNum, szGuildName) == TRUE && this->m_CastleDB.Fetch() != SQL_NO_DATA)
	{
		pMsg.iResult = this->m_CastleDB.GetAsInteger("QueryResult");
		pMsg.iRegMarkCount = this->m_CastleDB.GetAsInteger("DEL_MARKS");
	}

	this->m_CastleDB.Close();
	//DataSend(aIndex, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsGuildSetGiveUp(int aIndex, CSP_REQ_GUILDSETGIVEUP * aRecv)
{
	CSP_ANS_GUILDSETGIVEUP pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x12;
	pMsg.iResult = 0;

	pMsg.iIndex = aRecv->iIndex;
	pMsg.wMapSvrNum = aRecv->wMapSvrNum;
	pMsg.bIsGiveUp = aRecv->bIsGiveUp;

	char szGuildName[9] = { 0 };
	szGuildName[8] = 0;

	memcpy(szGuildName, aRecv->szGuildName, 0x08);
	memcpy(pMsg.szGuildName, aRecv->szGuildName, 0x08);

	if (this->m_CastleDB.ExecQuery("EXEC WZ_CS_ModifyGuildGiveUp %d, '%s', %d", aRecv->wMapSvrNum, szGuildName, aRecv->bIsGiveUp) == TRUE && this->m_CastleDB.Fetch() != SQL_NO_DATA)
	{
		pMsg.iResult = this->m_CastleDB.GetAsInteger("QeuryResult");
		pMsg.iRegMarkCount = this->m_CastleDB.GetAsInteger("DEL_MARKS");
	}

	this->m_CastleDB.Close();
	//DataSend(aIndex, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsNpcRemove(int aIndex, CSP_REQ_NPCREMOVE * aRecv)
{
	CSP_ANS_NPCREMOVE pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x16;
	pMsg.iResult = 0;

	pMsg.wMapSvrNum = aRecv->wMapSvrNum;
	pMsg.iNpcIndex = aRecv->iNpcIndex;
	pMsg.iNpcNumber = aRecv->iNpcNumber;

	if (this->m_CastleDB.ExecQuery("EXEC WZ_CS_ReqNpcRemove %d, %d, %d", aRecv->wMapSvrNum, aRecv->iNpcNumber, aRecv->iNpcIndex) == TRUE && this->m_CastleDB.Fetch() != SQL_NO_DATA)
	{
		pMsg.iResult = this->m_CastleDB.GetAsInteger("QueryResult");
	}

	this->m_CastleDB.Close();
	//DataSend(aIndex, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsCastleStateSync(int aIndex, CSP_REQ_CASTLESTATESYNC * aRecv)
{
	CSP_ANS_CASTLESTATESYNC pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x17;

	pMsg.wMapSvrNum = aRecv->wMapSvrNum;
	pMsg.iCastleState = aRecv->iCastleState;
	pMsg.iTaxHuntZone = aRecv->iTaxHuntZone;
	pMsg.iTaxRateChaos = aRecv->iTaxRateChaos;
	pMsg.iTaxRateStore = aRecv->iTaxRateStore;

	memcpy(pMsg.szOwnerGuildName, aRecv->szOwnerGuildName, 0x08);

	for (int i = 0; i<g_dwMaxServerGroups; i++)
	{
		if (g_Server[i].m_Index != -1
			&& g_Server[i].m_Type == g_Server[aIndex].m_Type
			&& g_Server[i].m_ServerGroup == aRecv->wMapSvrNum)
		{
			//DataSend(i, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
		}
	}
}

void CDataServerProtocol::GS_DGAnsCastleTributeMoney(int aIndex, CSP_REQ_CASTLETRIBUTEMONEY * aRecv)
{
	/***********************************
	NEED TESTINGS HERE
	***********************************/
	CSP_ANS_MONEYCHANGE pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(CSP_ANS_CASTLETRIBUTEMONEY);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x18;

	pMsg.wMapSvrNum = aRecv->wMapSvrNum;

	if (aRecv->iCastleTributeMoney > 0)
	{
		pMsg.h.size = sizeof(CSP_ANS_MONEYCHANGE);

		if (this->m_CastleDB.ExecQuery("EXEC WZ_CS_ModifyMoney %d, %d", aRecv->wMapSvrNum, aRecv->iCastleTributeMoney) == TRUE && this->m_CastleDB.Fetch() != SQL_NO_DATA)
		{
			pMsg.iResult = this->m_CastleDB.GetAsInteger("QueryResult");
			pMsg.i64CastleMoney = this->m_CastleDB.GetAsInteger64("MONEY");
		}
	}

	this->m_CastleDB.Close();
	//DataSend(aIndex, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsResetCastleTaxInfo(int aIndex, CSP_REQ_RESETCASTLETAXINFO * aRecv)
{
	CSP_ANS_RESETCASTLETAXINFO pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x19;
	pMsg.iResult = 0;

	pMsg.wMapSvrNum = aRecv->wMapSvrNum;

	if (this->m_CastleDB.ExecQuery("EXEC WZ_CS_ResetCastleTaxInfo %d", aRecv->wMapSvrNum) == TRUE && this->m_CastleDB.Fetch() != SQL_NO_DATA)
	{
		pMsg.iResult = this->m_CastleDB.GetAsInteger("QueryResult");
	}

	this->m_CastleDB.Close();
	//DataSend(aIndex, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsResetSiegeGuildInfo(int aIndex, CSP_REQ_RESETSIEGEGUILDINFO * aRecv)
{
	CSP_ANS_RESETSIEGEGUILDINFO pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x1A;
	pMsg.iResult = 0;

	pMsg.wMapSvrNum = aRecv->wMapSvrNum;

	if (this->m_CastleDB.ExecQuery("EXEC WZ_CS_ResetSiegeGuildInfo %d", aRecv->wMapSvrNum) == TRUE && this->m_CastleDB.Fetch() != SQL_NO_DATA)
	{
		pMsg.iResult = this->m_CastleDB.GetAsInteger("QueryResult");
	}

	this->m_CastleDB.Close();
	//DataSend(aIndex, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsResetRegSiegeInfo(int aIndex, CSP_REQ_RESETREGSIEGEINFO * aRecv)
{
	CSP_ANS_RESETREGSIEGEINFO pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x1B;
	pMsg.iResult = 0;

	pMsg.wMapSvrNum = aRecv->wMapSvrNum;

	if (this->m_CastleDB.ExecQuery("EXEC WZ_CS_ResetRegSiegeInfo %d", aRecv->wMapSvrNum) == TRUE && this->m_CastleDB.Fetch() != SQL_NO_DATA)
	{
		pMsg.iResult = this->m_CastleDB.GetAsInteger("QueryResult");
	}

	this->m_CastleDB.Close();
	//DataSend(aIndex, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsAllGuildMarkRegInfo(int aIndex, CSP_REQ_ALLGUILDREGINFO * aRecv)
{
	BYTE SendData[sizeof(CSP_ANS_ALLGUILDREGINFO) + sizeof(CSP_GUILDREGINFO) * MAX_CS_GUILDLIST] = { 0 };

	CSP_ANS_ALLGUILDREGINFO * lpMsg = (CSP_ANS_ALLGUILDREGINFO *)SendData;
	CSP_GUILDREGINFO * lpMsgBody = (CSP_GUILDREGINFO *)&SendData[sizeof(CSP_ANS_ALLGUILDREGINFO)];

	lpMsg->h.c = 0xC2;
	lpMsg->h.headcode = 0x83;
	lpMsg->iResult = TRUE;

	lpMsg->iIndex = aRecv->iIndex;
	lpMsg->wMapSvrNum = aRecv->wMapSvrNum;
	lpMsg->iCount = 0;
	int iCount = 0;
	if (this->m_CastleDB.ExecQuery("EXEC WZ_CS_GetAllGuildMarkRegInfo %d", aRecv->wMapSvrNum) == TRUE && this->m_CastleDB.Fetch() != SQL_NO_DATA)
	{
		do
		{
			this->m_CastleDB.GetAsString("REG_SIEGE_GUILD", lpMsgBody[iCount].szGuildName, sizeof(lpMsgBody->szGuildName));
			lpMsgBody[iCount].iRegMarkCount = this->m_CastleDB.GetAsInteger("REG_MARKS");
			lpMsgBody[iCount].bIsGiveUp = this->m_CastleDB.GetAsInteger("IS_GIVEUP");
			lpMsgBody[iCount].btRegRank = this->m_CastleDB.GetAsInteger("SEQ_NUM");
		} while ((iCount++) < MAX_CS_GUILDLIST && this->m_CastleDB.Fetch() != SQL_NO_DATA);
	}

	lpMsg->iCount = iCount;
	int size = sizeof(CSP_ANS_ALLGUILDREGINFO) + sizeof(CSP_GUILDREGINFO) * lpMsg->iCount;

	lpMsg->h.sizeH = SET_NUMBERH(size);
	lpMsg->h.sizeL = SET_NUMBERL(size);

	this->m_CastleDB.Close();
	//DataSend(aIndex, (PBYTE)SendData, size, __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsFirstCreateNPC(int aIndex, CSP_REQ_NPCSAVEDATA * aRecv)
{
	CSP_ANS_NPCSAVEDATA pMsg;

	CSP_NPCSAVEDATA * lpMsgBody = (CSP_NPCSAVEDATA *)((PBYTE)aRecv + sizeof(CSP_REQ_NPCSAVEDATA));

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x84;
	pMsg.iResult = 0;

	pMsg.wMapSvrNum = aRecv->wMapSvrNum;

	if (this->m_CastleDB.ExecQuery("DELETE FROM MuCastle_NPC WHERE MAP_SVR_GROUP = %d", aRecv->wMapSvrNum) == TRUE)
	{
		int iCount = aRecv->iCount;
		for (int i = 0; i < iCount; i++)
		{
			pMsg.iResult = 1;
			this->m_CastleDB.Close();

			if (this->m_CastleDB.ExecQuery("EXEC WZ_CS_ReqNpcBuy %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
				aRecv->wMapSvrNum, lpMsgBody[i].iNpcNumber, lpMsgBody[i].iNpcIndex,
				lpMsgBody[i].iNpcDfLevel, lpMsgBody[i].iNpcRgLevel, lpMsgBody[i].iNpcMaxHp,
				lpMsgBody[i].iNpcHp, lpMsgBody[i].btNpcX, lpMsgBody[i].btNpcY, lpMsgBody[i].btNpcDIR) != TRUE)
			{
				pMsg.iResult = 0;
				break;
			}
		}
	}

	this->m_CastleDB.Close();
	//DataSend(aIndex, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsCalcRegGuildList(int aIndex, CSP_REQ_CALCREGGUILDLIST * aRecv)
{
	BYTE SendData[sizeof(CSP_ANS_CALCREGGUILDLIST) + sizeof(CSP_CALCREGGUILDLIST) * MAX_CS_GUILDLIST] = { 0 };

	CSP_ANS_CALCREGGUILDLIST * lpMsg = (CSP_ANS_CALCREGGUILDLIST *)SendData;
	CSP_CALCREGGUILDLIST * lpMsgBody = (CSP_CALCREGGUILDLIST *)&SendData[sizeof(CSP_ANS_CALCREGGUILDLIST)];

	lpMsg->h.c = 0xC2;
	lpMsg->h.headcode = 0x85;
	lpMsg->wMapSvrNum = aRecv->wMapSvrNum;
	lpMsg->iResult = 0;

	int iCount = 0;

	if (this->m_CastleDB.ExecQuery("EXEC WZ_CS_GetCalcRegGuildList %d", aRecv->wMapSvrNum) == TRUE)
	{
		lpMsg->iResult = TRUE;

		if (this->m_CastleDB.Fetch() != SQL_NO_DATA)
		{
			do
			{
				this->m_CastleDB.GetAsString("REG_SIEGE_GUILD", lpMsgBody[iCount].szGuildName, sizeof(lpMsgBody->szGuildName));
				lpMsgBody[iCount].iRegMarkCount = this->m_CastleDB.GetAsInteger("REG_MARKS");
				lpMsgBody[iCount].iGuildMemberCount = this->m_CastleDB.GetAsInteger("GUILD_MEMBER");
				lpMsgBody[iCount].iGuildMasterLevel = this->m_CastleDB.GetAsInteger("GM_LEVEL");
				lpMsgBody[iCount].iSeqNum = this->m_CastleDB.GetAsInteger("SEQ_NUM");
			} while ((iCount++) < MAX_CS_GUILDLIST && this->m_CastleDB.Fetch() != SQL_NO_DATA);
		}
	}

	int size = sizeof(CSP_ANS_CALCREGGUILDLIST) + sizeof(CSP_CALCREGGUILDLIST) * iCount;

	lpMsg->iCount = iCount;
	lpMsg->h.sizeH = SET_NUMBERH(size);
	lpMsg->h.sizeL = SET_NUMBERL(size);

	this->m_CastleDB.Close();
	//DataSend(aIndex, (PBYTE)SendData, size, __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsCsGulidUnionInfo(int aIndex, CSP_REQ_CSGUILDUNIONINFO * aRecv)
{
	BYTE SendData[sizeof(CSP_ANS_CSGUILDUNIONINFO) + sizeof(CSP_CSGUILDUNIONINFO) * MAX_CS_GUILDLIST * 3] = { 0 };

	CSP_CSGUILDUNIONINFO * aRecvBody = (CSP_CSGUILDUNIONINFO*)((PBYTE)aRecv + sizeof(CSP_REQ_CSGUILDUNIONINFO));

	CSP_ANS_CSGUILDUNIONINFO * lpMsg = (CSP_ANS_CSGUILDUNIONINFO*)SendData;
	CSP_CSGUILDUNIONINFO * lpMsgBody = (CSP_CSGUILDUNIONINFO*)&SendData[sizeof(CSP_ANS_CSGUILDUNIONINFO)];

	lpMsg->h.c = 0xC2;
	lpMsg->h.headcode = 0x86;
	lpMsg->iResult = TRUE;
	lpMsg->wMapSvrNum = aRecv->wMapSvrNum;

	int iCount = 0;

	for (int i = 0; i < aRecv->iCount; i++)
	{
		char szGuildName[9] = { 0 };
		szGuildName[8] = 0;
		memcpy(szGuildName, aRecvBody[i].szGuildName, 8);

		if (this->m_CastleDB.ExecQuery("EXEC WZ_CS_GetCsGuildUnionInfo '%s'", szGuildName) == TRUE && this->m_CastleDB.Fetch() != SQL_NO_DATA)
		{
			do
			{
				this->m_CastleDB.GetAsString("GUILD_NAME", lpMsgBody[iCount].szGuildName, sizeof(lpMsgBody->szGuildName));
				lpMsgBody[iCount].iCsGuildID = aRecvBody[i].iCsGuildID;
			} while ((iCount++) < MAX_CS_GUILDLIST && this->m_CastleDB.Fetch() != SQL_NO_DATA);
		}
		else
		{
			lpMsg->iResult = 0;
			break;
		}
		this->m_CastleDB.Close();
	}

	int size = sizeof(CSP_ANS_CSGUILDUNIONINFO) + sizeof(CSP_CSGUILDUNIONINFO) * iCount;

	lpMsg->iCount = iCount;

	lpMsg->h.sizeH = SET_NUMBERH(size);
	lpMsg->h.sizeL = SET_NUMBERL(size);

	//DataSend(aIndex, (PBYTE)SendData, size, __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsCsSaveTotalGuildInfo(int aIndex, CSP_REQ_CSSAVETOTALGUILDINFO * aRecv)
{
	CSP_ANS_CSSAVETOTALGUILDINFO pMsg;
	CSP_CSSAVETOTALGUILDINFO * lpMsgBody = (CSP_CSSAVETOTALGUILDINFO*)((PBYTE)aRecv + sizeof(CSP_REQ_CSSAVETOTALGUILDINFO));

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x87;

	pMsg.iResult = TRUE;
	pMsg.wMapSvrNum = aRecv->wMapSvrNum;

	for (int i = 0; i < aRecv->iCount; i++)
	{
		char szGuildName[9] = { 0 };
		szGuildName[8] = 0;
		memcpy(szGuildName, lpMsgBody[i].szGuildName, 8);

		if (this->m_CastleDB.ExecQuery("EXEC WZ_CS_SetSiegeGuildInfo %d, '%s', %d, %d, %d",
			aRecv->wMapSvrNum, szGuildName, lpMsgBody[i].iCsGuildID, lpMsgBody[i].iGuildInvolved, lpMsgBody[i].iGuildScore) == TRUE)
		{

		}
		else
		{
			pMsg.iResult = FALSE;
			break;
		}
	}
	//DataSend(aIndex, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsCsLoadTotalGuildInfo(int aIndex, CSP_REQ_CSLOADTOTALGUILDINFO * aRecv)
{
	BYTE SendData[sizeof(CSP_ANS_CSLOADTOTALGUILDINFO) + sizeof(CSP_CSLOADTOTALGUILDINFO) * MAX_CS_GUILDLIST] = { 0 };

	CSP_ANS_CSLOADTOTALGUILDINFO * lpMsg = (CSP_ANS_CSLOADTOTALGUILDINFO*)SendData;
	CSP_CSLOADTOTALGUILDINFO * lpMsgBody = (CSP_CSLOADTOTALGUILDINFO*)&SendData[sizeof(CSP_ANS_CSLOADTOTALGUILDINFO)];

	lpMsg->h.c = 0xC2;
	lpMsg->h.headcode = 0x88;
	lpMsg->wMapSvrNum = aRecv->wMapSvrNum;
	lpMsg->iResult = FALSE;

	int iCount = 0;

	if (this->m_CastleDB.ExecQuery("EXEC WZ_CS_GetSiegeGuildInfo %d", aRecv->wMapSvrNum) == TRUE && this->m_CastleDB.Fetch() != SQL_NO_DATA)
	{
		lpMsg->iResult = TRUE;
		do
		{
			this->m_CastleDB.GetAsString("GUILD_NAME", lpMsgBody[iCount].szGuildName, sizeof(lpMsgBody->szGuildName));
			lpMsgBody[iCount].iCsGuildID = this->m_CastleDB.GetAsInteger("GUILD_ID");
			lpMsgBody[iCount].iGuildInvolved = this->m_CastleDB.GetAsInteger("GUILD_INVOLVED");
			lpMsgBody[iCount].iGuildScore = this->m_CastleDB.GetAsInteger("GUILD_SCORE");
		} while ((iCount++) < MAX_CS_GUILDLIST && this->m_CastleDB.Fetch() != SQL_NO_DATA);
	}

	lpMsg->iCount = iCount;

	int size = sizeof(CSP_ANS_CSLOADTOTALGUILDINFO) + sizeof(CSP_CSLOADTOTALGUILDINFO) * iCount;

	lpMsg->h.sizeH = SET_NUMBERH(size);
	lpMsg->h.sizeL = SET_NUMBERL(size);

	this->m_CastleDB.Close();
	//DataSend(aIndex, (PBYTE)SendData, size, __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsCastleNpcUpdate(int aIndex, CSP_REQ_NPCUPDATEDATA * aRecv)
{
	CSP_ANS_NPCUPDATEDATA pMsg;

	CSP_NPCUPDATEDATA * lpMsgBody = (CSP_NPCUPDATEDATA *)((PBYTE)aRecv + sizeof(CSP_REQ_NPCUPDATEDATA));

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x89;
	pMsg.iResult = FALSE;
	pMsg.wMapSvrNum = aRecv->wMapSvrNum;

	if (this->m_CastleDB.ExecQuery("DELETE FROM MuCastle_NPC WHERE MAP_SVR_GROUP = %d", aRecv->wMapSvrNum) == TRUE)
	{
		pMsg.iResult = TRUE;

		for (int i = 0; i < aRecv->iCount; i++)
		{
			this->m_CastleDB.Close();

			if (this->m_CastleDB.ExecQuery("EXEC WZ_CS_ReqNpcUpdate %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
				aRecv->wMapSvrNum, lpMsgBody[i].iNpcNumber, lpMsgBody[i].iNpcIndex,
				lpMsgBody[i].iNpcDfLevel, lpMsgBody[i].iNpcRgLevel, lpMsgBody[i].iNpcMaxHp,
				lpMsgBody[i].iNpcHp, lpMsgBody[i].btNpcX, lpMsgBody[i].btNpcY, lpMsgBody[i].btNpcDIR) == TRUE)
			{

			}
			else
			{
				pMsg.iResult = FALSE;
				break;
			}
		}
	}

	this->m_CastleDB.Close();
	//DataSend(aIndex, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::DGAnsCrywolfSync(int aIndex, CWP_REQ_CRYWOLFSYNC * aRecv)
{
	CWP_ANS_CRYWOLFSYNC pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xB0;
	pMsg.btResult = TRUE;

	pMsg.iCrywolfState = aRecv->iCrywolfState;
	pMsg.iOccupationState = aRecv->iOccupationState;
	pMsg.iApplyBenefit = aRecv->iApplyBenefit;
	pMsg.iApplyPenalty = aRecv->iApplyPenalty;
	pMsg.iPlusChaosRate = aRecv->iPlusChaosRate;
	pMsg.iGemDropPenaltyRate = aRecv->iGemDropPenaltyRate;
	pMsg.iGettingExpPenaltyRate = aRecv->iGettingExpPenaltyRate;
	pMsg.iMinusMonHPBenefitRate = aRecv->iMinusMonHPBenefitRate;
	pMsg.iKundunHPRefillState = aRecv->iKundunHPRefillState;

	for (int i = 0; i<g_dwMaxServerGroups; i++)
	{
		if (g_Server[i].m_Index != -1
			&& g_Server[i].m_Type == g_Server[aIndex].m_Type
			&& g_Server[i].m_ServerGroup == aRecv->wMapSvrNum)
		{
			//DataSend(i, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
		}
	}
}

void CDataServerProtocol::DGAnsCrywolfInfoLoad(int aIndex, CWP_REQ_CRYWOLFINFOLOAD * aRecv)
{
	CWP_ANS_CRYWOLFINFOLOAD pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xB1;
	pMsg.btResult = FALSE;

	if (this->m_CrywolfDB.ExecQuery("EXEC WZ_CW_InfoLoad %d", aRecv->wMapSvrNum) == TRUE && this->m_CrywolfDB.Fetch() != SQL_NO_DATA)
	{
		pMsg.btResult = TRUE;
		pMsg.iOccupationState = this->m_CrywolfDB.GetAsInteger("CRYWOLF_OCCUFY");
		pMsg.iCrywolfState = this->m_CrywolfDB.GetAsInteger("CRYWOLF_STATE");
	}

	this->m_CrywolfDB.Close();
	//DataSend(aIndex, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::DGAnsCrywolfInfoSave(int aIndex, CWP_REQ_CRYWOLFINFOSAVE * aRecv)
{
	CWP_ANS_CRYWOLFINFOSAVE pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xB2;
	pMsg.btResult = FALSE;

	if (this->m_CrywolfDB.ExecQuery("EXEC WZ_CW_InfoSave %d, %d, %d", aRecv->wMapSvrNum, aRecv->iCrywolfState, aRecv->iOccupationState) == TRUE)
	{
		pMsg.btResult = TRUE;
	}

	this->m_CrywolfDB.Close();
	//DataSend(aIndex, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::DGAnsPeriodItemExInsert(int aIndex, PMSG_REQ_PERIODITEMEX_INSERT * aRecv)
{
	PMSG_ANS_PERIODITEMEX_INSERT pMsg;

	pMsg.head.c = 0xC1;
	pMsg.head.headcode = 0xD0;
	pMsg.head.subcode = 0x01;
	pMsg.head.size = sizeof(pMsg);

	pMsg.wUserIndex = aRecv->wUserIndex;
	pMsg.dwUserGuid = aRecv->dwUserGuid;
	pMsg.btItemtype = aRecv->btItemType;
	pMsg.wItemCode = aRecv->wItemCode;
	pMsg.btEffectCategory = aRecv->btEffectCategory;
	pMsg.btEffectType1 = aRecv->btEffectType1;
	pMsg.btEffectType2 = aRecv->btEffectType2;

	if (aRecv->Serial == 0)
		pMsg.Serial = g_ItemSerial.GenerateSerial();
	else
		pMsg.Serial = aRecv->Serial;

	pMsg.dwDuration = aRecv->dwDuration;
	pMsg.lBuyDate = aRecv->lBuyDate;
	pMsg.lExpireDate = aRecv->lExpireDate;
	pMsg.btResultCode = 1;

	if (this->m_PeriodItemDB.ExecQuery("EXEC IGC_PeriodItemInsertEx %d, '%s', %d, %d, %d, %d, %d, %I64d, %d, %I64d, %I64d",
		aRecv->dwUserGuid, aRecv->chCharacterName, aRecv->btItemType, aRecv->wItemCode, aRecv->btEffectCategory,
		aRecv->btEffectType1, aRecv->btEffectType2, pMsg.Serial, aRecv->dwDuration, aRecv->lBuyDate, aRecv->lExpireDate) && this->m_PeriodItemDB.Fetch() != SQL_NO_DATA)
	{
		pMsg.btResultCode = this->m_PeriodItemDB.GetResult(0);
	}

	this->m_PeriodItemDB.Close();
	//DataSend(aIndex, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::DGAnsPeriodItemExDelete(int aIndex, PMSG_REQ_PERIODITEMEX_DELETE * aRecv)
{
	PMSG_ANS_PERIODITEMEX_DELETE pMsg;

	pMsg.head.c = 0xC1;
	pMsg.head.headcode = 0xD0;
	pMsg.head.subcode = 0x02;
	pMsg.head.size = sizeof(pMsg);

	pMsg.dwUserGuid = aRecv->dwUserGuid;
	pMsg.wUserIndex = aRecv->wUserIndex;
	pMsg.btResultCode = 1;

	if (this->m_PeriodItemDB.ExecQuery("EXEC IGC_PeriodItemDeleteEx %d, '%s', %d, %d, %I64d", aRecv->dwUserGuid, aRecv->chCharacterName, aRecv->btItemType, aRecv->wItemCode, aRecv->Serial) == TRUE)
	{
		pMsg.btResultCode = 0;
	}

	this->m_PeriodItemDB.Close();
	//DataSend(aIndex, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::DGAnsPeriodItemExSelect(int aIndex, PMSG_REQ_PERIODITEMEX_SELECT * aRecv)
{
	char szName[11] = { 0 };
	szName[10] = 0;
	memcpy(szName, aRecv->chCharacterName, MAX_ACCOUNT_LEN);

	int PacketSize = 0;

	PMSG_ANS_PERIODITEMEX_LIST_COUNT pListCount;
	PMSG_ANS_PERIODITEMEX_LIST pList;
	PMSG_ANS_PERIODITEMEX_EXPIRED_ITEMLIST pExpiredList;

	pList.head.set((LPBYTE)&pList, 0xD0, 0x05, sizeof(PMSG_ANS_PERIODITEMEX_LIST));
	pListCount.head.set((LPBYTE)&pListCount, 0xD0, 0x04, sizeof(PMSG_ANS_PERIODITEMEX_LIST_COUNT));
	pExpiredList.head.set((LPBYTE)&pExpiredList, 0xD0, 0x03, sizeof(PMSG_ANS_PERIODITEMEX_EXPIRED_ITEMLIST));

	pExpiredList.btExpiredItemCount = 0;
	this->m_PeriodItemDB.ExecQuery("EXEC IGC_PeriodItemExpiredItemSelectEx %d, '%s'", aRecv->dwUserGuid, szName);

	while (this->m_PeriodItemDB.Fetch() == SQL_SUCCESS)
	{
		pExpiredList.wItemCode[pExpiredList.btExpiredItemCount] = this->m_PeriodItemDB.GetAsInteger("ItemCode");
		pExpiredList.Serial[pExpiredList.btExpiredItemCount] = this->m_PeriodItemDB.GetAsInteger64("Serial");
		pExpiredList.btExpiredItemCount++;

		if (pExpiredList.btExpiredItemCount >= 90)
		{
			break;
		}
	}

	pExpiredList.wUserIndex = aRecv->wUserIndex;

	//DataSend(aIndex, (LPBYTE)&pExpiredList, sizeof(pExpiredList), __FUNCTION__);

	this->m_PeriodItemDB.Close();

	pListCount.btListCount = 0;
	pListCount.dwUserGuid = aRecv->dwUserGuid;
	pListCount.wUserIndex = aRecv->wUserIndex;
	this->m_PeriodItemDB.ExecQuery("EXEC IGC_PeriodItemSelectEx %d, '%s'", aRecv->dwUserGuid, szName);

	int itemid = 0;
	time_t currtime = time(NULL);

	while (this->m_PeriodItemDB.Fetch() == SQL_SUCCESS)
	{
		itemid = this->m_PeriodItemDB.GetAsInteger("id");
		pList.dwUserGuid = aRecv->dwUserGuid;
		pList.wUserIndex = aRecv->wUserIndex;
		pList.wItemCode = this->m_PeriodItemDB.GetAsInteger("ItemCode");
		pList.btEffectCategory = this->m_PeriodItemDB.GetAsInteger("EffectCategory");
		pList.btEffectType1 = this->m_PeriodItemDB.GetAsInteger("EffectType1");
		pList.btEffectType2 = this->m_PeriodItemDB.GetAsInteger("EffectType2");
		pList.btItemType = this->m_PeriodItemDB.GetAsInteger("ItemType");
		pList.btUsedInfo = this->m_PeriodItemDB.GetAsInteger("UsedInfo");
		pList.Serial = this->m_PeriodItemDB.GetAsInteger64("Serial");
		pList.lItemBuyDate = this->m_PeriodItemDB.GetAsInteger64("BuyDate");
		pList.lItemExpireDate = this->m_PeriodItemDB.GetAsInteger64("ExpireDate");

		//DataSend(aIndex, (LPBYTE)&pList, sizeof(pList), __FUNCTION__);
		pListCount.btListCount++;
	}

	this->m_PeriodItemDB.Close();

	//DataSend(aIndex, (LPBYTE)&pListCount, sizeof(pListCount), __FUNCTION__);
}

void CDataServerProtocol::ReqInGameShopItemList(short aIndex, ISHOP_REQ_ITEMLIST *aRecv)
{
	char Buff[65535] = { 0 };
	int PacketSize = 0;
	ISHOP_ANS_ITEMLIST * lpMsg = (ISHOP_ANS_ITEMLIST *)(Buff);
	ISHOP_ITEMLIST * lpMsgBody = (ISHOP_ITEMLIST *)(Buff + sizeof(ISHOP_ANS_ITEMLIST));

	this->m_ItemShopDB.ExecQuery("EXEC WZ_IBS_GetItemsList '%s', %d", aRecv->AccountID, aRecv->InvType);

	lpMsg->Count = 0;
	while (this->m_ItemShopDB.Fetch() != SQL_NO_DATA)
	{
		lpMsgBody = (ISHOP_ITEMLIST *)(Buff + sizeof(ISHOP_ANS_ITEMLIST) + lpMsg->Count * sizeof(ISHOP_ITEMLIST));

		lpMsgBody->UniqueCode = this->m_ItemShopDB.GetAsInteger("UniqueCode");
		lpMsgBody->AuthCode = this->m_ItemShopDB.GetAsInteger("AuthCode");
		lpMsgBody->UniqueID1 = this->m_ItemShopDB.GetAsInteger("UniqueID1");
		lpMsgBody->UniqueID2 = this->m_ItemShopDB.GetAsInteger("UniqueID2");
		lpMsgBody->UniqueID3 = this->m_ItemShopDB.GetAsInteger("UniqueID3");
		lpMsgBody->InventoryType = static_cast<BYTE>(this->m_ItemShopDB.GetAsInteger("InventoryType"));

		if (aRecv->InvType == 2)
		{
			this->m_ItemShopDB.GetAsString("GiftName", lpMsgBody->GiftName, 10);
			this->m_ItemShopDB.GetAsString("Message", lpMsgBody->Message, 200);
		}

		++lpMsg->Count;

		if (lpMsg->Count >= 100)
			break;
	}

	lpMsg->Result = 1;

	PacketSize = (sizeof(ISHOP_ANS_ITEMLIST) + (lpMsg->Count * sizeof(ISHOP_ITEMLIST)));

	this->m_ItemShopDB.Close();

	lpMsg->aIndex = aRecv->aIndex;
	lpMsg->InvNum = aRecv->InvNum;
	lpMsg->InvType = aRecv->InvType;
	memcpy(lpMsg->AccountID, aRecv->AccountID, 11);

	lpMsg->h.c = 0xC2;
	lpMsg->h.headcode = 0xD2;
	lpMsg->h.subcode = 0x02;
	lpMsg->h.sizeH = HIBYTE(PacketSize);
	lpMsg->h.sizeL = LOBYTE(PacketSize);

	//DataSend(aIndex, (LPBYTE)&Buff, PacketSize, __FUNCTION__);
}

void CDataServerProtocol::ReqInGameShopCharacterCardBuy(short aIndex, ISHOP_CHARCARD_BUY *aRecv)
{
	if (aRecv->Type == 0)
	{
		this->m_ItemShopDB.ExecQuery("UPDATE AccountCharacter SET Summoner=1 WHERE Id = '%s'", aRecv->AccountID);

	}
	else if (aRecv->Type == 1)
	{
		this->m_ItemShopDB.ExecQuery("UPDATE AccountCharacter SET RageFighter=1 WHERE Id = '%s'", aRecv->AccountID);
	}
	this->m_ItemShopDB.Close();
}

void CDataServerProtocol::ReqInGameShopItemBuy(short aIndex, ISHOP_ITEM_BUY *aRecv)
{
	ISHOP_ITEM_BUYANS pResult;
	PHeadSetB((LPBYTE)&pResult, 0xD5, sizeof(pResult));

	memcpy(&pResult.AccountID, aRecv->AccountID, 11);
	pResult.aIndex = aRecv->aIndex;


	QueryResult* res = this->m_ItemShopDB.Fetch("SELECT %s AS Result FROM T_InGameShop_Point WHERE AccountID = '%s'", aRecv->CoinType == 0 ? "WCoinC" : aRecv->CoinType == 1 ? "WCoinP" : "GoblinPoint", aRecv->AccountID);
	this->m_ItemShopDB.Fetch();

	int Coin = this->m_ItemShopDB.GetAsInteger("Result");

	this->m_ItemShopDB.Close();

	if (Coin < aRecv->Price)
	{
		pResult.Result = 2;
		//DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size, __FUNCTION__);
	}

	else
	{
		Coin -= aRecv->Price;

		this->m_ItemShopDB.ExecQuery("EXEC WZ_IBS_AddItem '%s', %d, %d, %d, %d",
			aRecv->AccountID, aRecv->ID1, aRecv->ID2, aRecv->ID3, 1);

		this->m_ItemShopDB.Fetch();

		if (this->m_ItemShopDB.GetAsInteger("Result") == 1)
		{
			this->m_ItemShopDB.Close();

			this->m_ItemShopDB.ExecQuery("UPDATE T_InGameShop_Point SET %s = %d WHERE AccountID = '%s'", aRecv->CoinType == 0 ? "WCoinC" : aRecv->CoinType == 1 ? "WCoinP" : "GoblinPoint", Coin, aRecv->AccountID);

			pResult.Result = 1;
			pResult.ID1 = aRecv->ID1;
			pResult.ID2 = aRecv->ID2;
			pResult.ID3 = aRecv->ID3;
			//DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size, __FUNCTION__);
			return;
		}
		this->m_ItemShopDB.Close();
		pResult.Result = 3;
		//DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size, __FUNCTION__);
	}
}

void CDataServerProtocol::ReqInGameShopItemGift(short aIndex, ISHOP_ITEM_GIFT *aRecv)
{
	ISHOP_ITEM_GIFTANS pResult;
	PHeadSetB((LPBYTE)&pResult, 0xD6, sizeof(pResult));

	memcpy(&pResult.AccountID, aRecv->AccountID, 11);
	pResult.aIndex = aRecv->aIndex;

	QueryResult* res = this->m_ItemShopDB.Fetch("SELECT %s AS Result FROM T_InGameShop_Point WHERE AccountID = '%s'", aRecv->CoinType == 0 ? "WCoinC" : aRecv->CoinType == 1 ? "WCoinP" : "GoblinPoint", aRecv->AccountID);
	this->m_ItemShopDB.Fetch();

	int Coin = this->m_ItemShopDB.GetAsInteger("Result");

	this->m_ItemShopDB.Close();

	if (Coin < aRecv->Price)
	{
		pResult.Result = 2;
		//DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size, __FUNCTION__);
		return;
	}

	this->m_ItemShopDB.ExecQuery("EXEC IGC_IBS_CheckGiftTarget '%s'", aRecv->TargetName);
	this->m_ItemShopDB.Fetch();

	int iResult = this->m_ItemShopDB.GetAsInteger("Result");

	if (iResult == 3)
	{
		pResult.Result = iResult;
		//DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size, __FUNCTION__);
		return;
	}

	this->m_ItemShopDB.Close();

	Coin -= aRecv->Price;

	this->m_ItemShopDB.ExecQuery("EXEC WZ_IBS_AddGift '%s', %d, %d, %d, %d, '%s', '%s'", aRecv->TargetName, aRecv->ID1, aRecv->ID2, aRecv->ID3, 2, aRecv->Name, aRecv->Message);

	this->m_ItemShopDB.Fetch();

	pResult.Result = this->m_ItemShopDB.GetAsInteger("Result");
	pResult.ID1 = aRecv->ID1;
	pResult.ID2 = aRecv->ID2;
	pResult.ID3 = aRecv->ID3;

	this->m_ItemShopDB.Close();

	if (pResult.Result == 1)
	{
		this->m_ItemShopDB.ExecQuery("UPDATE T_InGameShop_Point SET %s = %d WHERE AccountID = '%s'", aRecv->CoinType == 0 ? "WCoinC" : aRecv->CoinType == 1 ? "WCoinP" : "GoblinPoint", Coin, aRecv->AccountID);
		this->m_ItemShopDB.Close();
	}

	//DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size, __FUNCTION__);
}

void CDataServerProtocol::ReqInGameShopItemUse(short aIndex, ISHOP_ITEM_USE *aRecv)
{
	ISHOP_ITEM_USEANS pMsg = { 0 };
	PHeadSetB((LPBYTE)&pMsg, 0xD9, sizeof(pMsg));
	this->m_ItemShopDB.ExecQuery("EXEC WZ_IBS_GetItemToUse '%s', %d, %d", aRecv->AccountID, aRecv->UniqueCode, aRecv->AuthCode);
	this->m_ItemShopDB.Fetch();

	pMsg.Result = this->m_ItemShopDB.GetAsInteger("Result");

	pMsg.aIndex = aRecv->aIndex;
	pMsg.UniqueCode = aRecv->UniqueCode;
	pMsg.AuthCode = aRecv->AuthCode;

	if (pMsg.Result == 1)
	{
		pMsg.ID1 = this->m_ItemShopDB.GetAsInteger("UniqueID1");
		pMsg.ID2 = this->m_ItemShopDB.GetAsInteger("UniqueID2");
		pMsg.ID3 = this->m_ItemShopDB.GetAsInteger("UniqueID3");
	}

	this->m_ItemShopDB.Close();

	memcpy(pMsg.AccountID, aRecv->AccountID, 11);

	//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::ReqInGameShopPoint(short aIndex, ISHOP_REQ_POINT *aRecv)
{
	ISHOP_ANS_POINT pMsg = { 0 };

	QueryResult* res = this->m_ItemShopDB.Fetch("SELECT WCoinP, WCoinC, GoblinPoint FROM T_InGameShop_Point WHERE AccountID = '%s'", aRecv->AccountID);

	if (this->m_ItemShopDB.Fetch() == SQL_NO_DATA)
	{
		this->m_ItemShopDB.Close();
		this->m_ItemShopDB.ExecQuery("INSERT INTO T_InGameShop_Point VALUES ('%s', 0.00, 0.00, 0.00)", aRecv->AccountID);
		this->m_ItemShopDB.Close();
	}
	else
	{
		pMsg.CoinP = this->m_ItemShopDB.GetAsInteger("WCoinP");
		pMsg.CoinC = this->m_ItemShopDB.GetAsInteger("WCoinC");
		pMsg.Goblin = this->m_ItemShopDB.GetAsInteger("GoblinPoint");
		this->m_ItemShopDB.Close();
	}

	pMsg.aIndex = aRecv->aIndex;
	memcpy(pMsg.AccountID, aRecv->AccountID, 11);

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xD1;
	pMsg.h.size = sizeof(pMsg);

	//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::ReqInGameShopAddPoint(short aIndex, ISHOP_POINT_ADD *aRecv)
{
	this->m_ItemShopDB.ExecQuery("EXEC WZ_IBS_AddCoin '%s', %d, %f", aRecv->AccountID, aRecv->Type, aRecv->Coin);
	this->m_ItemShopDB.Close();
}

void CDataServerProtocol::ReqInGameShopPackageBuy(short aIndex, LPBYTE aRecv)
{
	ISHOP_ITEM_BUY_PACKAGE * lpMsg = (ISHOP_ITEM_BUY_PACKAGE *)(aRecv);
	ISHOP_ITEM_PACKAGE * lpItem = (ISHOP_ITEM_PACKAGE *)(aRecv + sizeof(ISHOP_ITEM_BUY_PACKAGE));

	ISHOP_ITEM_BUYANS pResult;
	PHeadSetB((LPBYTE)&pResult, 0xD5, sizeof(pResult));

	memcpy(&pResult.AccountID, lpMsg->AccountID, 11);
	pResult.aIndex = lpMsg->aIndex;

	QueryResult* res = this->m_ItemShopDB.Fetch("SELECT %s AS Result FROM T_InGameShop_Point WHERE AccountID = '%s'", lpMsg->CoinType == 0 ? "WCoinC" : lpMsg->CoinType == 1 ? "WCoinP" : "GoblinPoint", lpMsg->AccountID);
	this->m_ItemShopDB.Fetch();

	int Coin = this->m_ItemShopDB.GetAsInteger("Result");

	this->m_ItemShopDB.Close();

	if (Coin < lpMsg->Price)
	{
		pResult.Result = 2;
		//DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size, __FUNCTION__);
		return;
	}

	Coin -= lpMsg->Price;

	pResult.Result = 1;
	pResult.ID1 = lpItem->ID1;
	pResult.ID2 = lpItem->ID2;
	pResult.ID3 = lpItem->ID3;

	this->m_ItemShopDB.ExecQuery("UPDATE T_InGameShop_Point SET %s = %d WHERE AccountID = '%s'", lpMsg->CoinType == 0 ? "WCoinC" : lpMsg->CoinType == 1 ? "WCoinP" : "GoblinPoint", Coin, lpMsg->AccountID);
	this->m_ItemShopDB.Close();

	for (int i = 0; i<lpMsg->Count; i++)
	{
		lpItem = (ISHOP_ITEM_PACKAGE *)(aRecv + sizeof(ISHOP_ITEM_BUY_PACKAGE) + i * sizeof(ISHOP_ITEM_PACKAGE));

		this->m_ItemShopDB.ExecQuery("EXEC WZ_IBS_AddItem '%s', %d, %d, %d, %d",
			lpMsg->AccountID, lpItem->ID1, lpItem->ID2, lpItem->ID3, 1);

		this->m_ItemShopDB.Fetch();
		this->m_ItemShopDB.Close();
	}

	//DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size, __FUNCTION__);
}

void CDataServerProtocol::ReqInGameShopPackageGift(short aIndex, LPBYTE aRecv)
{
	ISHOP_ITEM_GIFT_PACKAGE * lpMsg = (ISHOP_ITEM_GIFT_PACKAGE *)(aRecv);
	ISHOP_ITEM_PACKAGE * lpItem = (ISHOP_ITEM_PACKAGE *)(aRecv + sizeof(ISHOP_ITEM_GIFT_PACKAGE));

	ISHOP_ITEM_GIFTANS pResult;
	PHeadSetB((LPBYTE)&pResult, 0xD6, sizeof(pResult));

	memcpy(&pResult.AccountID, lpMsg->AccountID, 11);
	pResult.aIndex = lpMsg->aIndex;

	QueryResult* res = this->m_ItemShopDB.Fetch("SELECT %s AS Result FROM T_InGameShop_Point WHERE AccountID = '%s'", lpMsg->CoinType == 0 ? "WCoinC" : lpMsg->CoinType == 1 ? "WCoinP" : "GoblinPoint", lpMsg->AccountID);
	this->m_ItemShopDB.Fetch();

	int Coin = this->m_ItemShopDB.GetAsInteger("Result");

	this->m_ItemShopDB.Close();

	if (Coin < lpMsg->Price)
	{
		pResult.Result = 2;
		//DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size, __FUNCTION__);
		return;
	}

	this->m_ItemShopDB.ExecQuery("EXEC IGC_IBS_CheckGiftTarget '%s'", lpMsg->TargetName);
	this->m_ItemShopDB.Fetch();

	int iResult = this->m_ItemShopDB.GetAsInteger("Result");

	if (iResult == 3)
	{
		pResult.Result = iResult;
		//DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size, __FUNCTION__);
		return;
	}

	this->m_ItemShopDB.Close();

	Coin -= lpMsg->Price;

	pResult.Result = 1;
	pResult.ID1 = lpItem->ID1;
	pResult.ID2 = lpItem->ID2;
	pResult.ID3 = lpItem->ID3;

	this->m_ItemShopDB.ExecQuery("UPDATE T_InGameShop_Point SET %s = %d WHERE AccountID = '%s'", lpMsg->CoinType == 0 ? "WCoinC" : lpMsg->CoinType == 1 ? "WCoinP" : "GoblinPoint", Coin, lpMsg->AccountID);
	this->m_ItemShopDB.Close();

	for (int i = 0; i<lpMsg->Count; i++)
	{
		lpItem = (ISHOP_ITEM_PACKAGE *)(aRecv + sizeof(ISHOP_ITEM_GIFT_PACKAGE) + i * sizeof(ISHOP_ITEM_PACKAGE));

		this->m_ItemShopDB.ExecQuery("EXEC WZ_IBS_AddGift '%s', %d, %d, %d, %d, '%s', '%s'",
			lpMsg->TargetName, lpItem->ID1, lpItem->ID2, lpItem->ID3,
			2, lpMsg->Name, lpMsg->Message);

		this->m_ItemShopDB.Fetch();
		this->m_ItemShopDB.Close();
	}

	//DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size, __FUNCTION__);
}

void CDataServerProtocol::ReqInGameShopItemDelete(short aIndex, ISHOP_ITEM_DELETE * aRecv)
{
	this->m_ItemShopDB.ExecQuery("EXEC WZ_IBS_DeleteItem '%s', %d, %d", aRecv->AccountID, aRecv->UniqueCode, aRecv->AuthCode);
	this->m_ItemShopDB.Close();
}

void CDataServerProtocol::ReqInGameShopItemRollbackUse(short aIndex, ISHOP_ITEM_ROLLBACK * aRecv)
{
	QueryResult* res = this->m_ItemShopDB.Fetch("SELECT UsedItem FROM T_InGameShop_Items WHERE UniqueCode = %d AND AuthCode = %d AND AccountID = '%s'", aRecv->UniqueCode, aRecv->AuthCode, aRecv->AccountID);
	this->m_ItemShopDB.Fetch();

	int iUsedFlag = this->m_ItemShopDB.GetAsInteger("UsedItem");

	this->m_ItemShopDB.Close();

	if (iUsedFlag == TRUE)
	{
		this->m_ItemShopDB.ExecQuery("UPDATE T_InGameShop_Items SET UsedItem = 0 WHERE UniqueCode = %d AND AuthCode = %d AND AccountID = '%s'", aRecv->UniqueCode, aRecv->AuthCode, aRecv->AccountID);
	}
}

void CDataServerProtocol::ReqLuckyCoinInfo(int aIndex, PMSG_REQ_LUCKYCOIN * lpMsg)
{
	int test = SQL_NO_DATA;
	PMSG_ANS_LUCKYCOIN pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x19;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iIndex = lpMsg->iIndex;
	memcpy(pMsg.szUID, lpMsg->szUID, sizeof(pMsg.szUID));
	pMsg.szUID[10] = 0;

	if (QueryResult* res = this->m_EventDB.Fetch("SELECT LuckyCoin FROM T_LuckyCoin WHERE AccountID='%s'", lpMsg->szUID) == FALSE)
	{
		pMsg.LuckyCoins = 0;
	}
	else
	{
		if (this->m_EventDB.Fetch() != SQL_NO_DATA)
		{
			pMsg.LuckyCoins = (short)this->m_EventDB.GetAsInteger("LuckyCoin");
		}
		else
		{
			this->m_EventDB.Close();
			if (this->m_EventDB.ExecQuery("INSERT INTO T_LuckyCoin (AccountID) VALUES ('%s')", pMsg.szUID) == FALSE)
			{
				pMsg.LuckyCoins = 0;
			}
			else
			{
				pMsg.LuckyCoins = 0;
			}
		}
	}

	this->m_EventDB.Close();
	//DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::ReqRegLuckyCoin(int aIndex, PMSG_REQ_REGISTER_LUCKYCOIN * lpMsg)
{
	PMSG_ANS_REG_LUCKYCOIN pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x18;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iIndex = lpMsg->iIndex;
	pMsg.Pos = lpMsg->btPos;

	strcpy(pMsg.szUID, lpMsg->szAccountID);
	pMsg.szUID[10] = 0x00;
	pMsg.Result = FALSE;

	if (this->m_EventDB.ExecQuery("UPDATE T_LuckyCoin SET LuckyCoin = LuckyCoin + 1 WHERE AccountID = '%s'", pMsg.szUID))
	{
		this->m_EventDB.Close();

		if (QueryResult* res = this->m_EventDB.Fetch("SELECT LuckyCoin FROM T_LuckyCoin WHERE AccountID='%s'", pMsg.szUID))
		{
			if (this->m_EventDB.Fetch() != SQL_NO_DATA)
			{
				pMsg.LuckyCoins = (short)this->m_EventDB.GetAsInteger("LuckyCoin");

				if (pMsg.LuckyCoins != -1)
				{
					pMsg.Result = TRUE;
				}
			}
		}
	}

	this->m_EventDB.Close();

	//DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::DevilSqureScore(PMSG_ANS_EVENTUSERSCORE * lpMsg)
{
	char szAccount[11] = { 0 };
	char szName[11] = { 0 };
	szAccount[10] = 0;
	szName[10] = 0;
	memcpy(szAccount, lpMsg->AccountID, 10);
	memcpy(szName, lpMsg->GameID, 10);

	g_Log.Add("[DEVIL] SERVER:%d-%d, SQUARE:%d, ACCNT:%s, CHAR:%s, CLASS:%d, SCORE:%d",
		lpMsg->ServerCode / 20 + 1, lpMsg->ServerCode % 20 + 1, lpMsg->SquareNum, szAccount, szName, lpMsg->Class, lpMsg->Score);

	this->m_RankingDB.ExecQuery("EXEC SP_POINT_ACCUMULATION '%d', '%d', '%s', '%s', '%d', '%d'",
		lpMsg->ServerCode / 20 + 1, lpMsg->SquareNum, szAccount, szName, lpMsg->Class, lpMsg->Score);
	this->m_RankingDB.Close();
}

void CDataServerProtocol::GDReqBloodCastleEnterCount(int aIndex, PMSG_REQ_BLOODCASTLE_ENTERCOUNT * lpMsg)
{
	PMSG_ANS_BLOODCASTLE_ENTERCOUNT pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBD;
	pMsg.h.subcode = 0x02;
	pMsg.h.size = sizeof(pMsg);

	memcpy(pMsg.AccountID, lpMsg->AccountID, sizeof(pMsg.AccountID));
	memcpy(pMsg.GameID, lpMsg->GameID, sizeof(pMsg.GameID));

	pMsg.ServerCode = lpMsg->ServerCode;
	pMsg.iObjIndex = lpMsg->iObjIndex;

	if (this->m_RankingDB.ExecQuery("EXEC SP_LEFT_ENTERCOUNT_BC '%s', '%s', '%d'", lpMsg->AccountID, lpMsg->GameID, lpMsg->ServerCode) == TRUE)
	{
		pMsg.iLeftCount = this->m_RankingDB.GetAsInteger("LeftEnterCount");
	}
	else
	{
		pMsg.iLeftCount = -1;
	}

	this->m_RankingDB.Close();
	//DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::BloodCastleScore_5TH(PMSG_ANS_BLOODCASTLESCORE_5TH * lpMsg)
{
	char szAccount[11] = { 0 };
	char szName[11] = { 0 };
	szAccount[10] = 0;
	szName[10] = 0;

	memcpy(szAccount, lpMsg->AccountID, 10);
	memcpy(szName, lpMsg->GameID, 10);

	g_Log.Add("[BLOOD] SERVER:%d-%d, BRIDGE:%d, ACCNT:%s, CHAR:%s, CLASS:%d, SCORE:%d, LEFTTIME:%d, ALIVEPARTY:%d",
		lpMsg->ServerCode / 20 + 1, lpMsg->ServerCode % 20 + 1, lpMsg->BridgeNum, szAccount, szName, lpMsg->Class, lpMsg->Score, lpMsg->iLeftTime, lpMsg->iAlivePartyCount);

	this->m_RankingDB.ExecQuery("EXEC SP_POINT_ACCM_BC_5TH '%d', '%d', '%s', '%s', '%d', '%d', '%d', '%d'",
		lpMsg->ServerCode / 20 + 1, lpMsg->BridgeNum, szAccount, szName, lpMsg->Class, lpMsg->Score, lpMsg->iLeftTime, lpMsg->iAlivePartyCount);
}

void CDataServerProtocol::IllusionTempleScore(PMSG_ANS_ILLUSIONTEMPLE_RANKING * lpMsg)
{
	char szAccountID[11] = { 0 };
	char szName[11] = { 0 };
	szAccountID[10] = 0;
	szName[10] = 0;

	memcpy(szAccountID, lpMsg->AccountID, 10);
	memcpy(szName, lpMsg->GameID, 10);

	g_Log.Add("[ILLUSION] SERVER:%d-%d, TEMPLE:%d ACCNT:%s, CHAR:%s, CLASS:%d, SCORE:%d, KILLCOUNT:%d, RELICS:%d, EXPERIENCE:%d, ISWINNER:%d",
		lpMsg->ServerCode / 20 + 1, lpMsg->ServerCode % 20 + 1, lpMsg->TempleNum, szAccountID, szName, lpMsg->Class, lpMsg->TotalScore, lpMsg->KillCount, lpMsg->RelicsGivenCount, lpMsg->Experience, lpMsg->IsWinner);

	this->m_RankingDB.ExecQuery("EXEC SP_POINT_ACCM_IT '%d', '%d', '%s', '%s', '%d', '%d', '%d', '%d', '%d', '%d'",
		lpMsg->ServerCode / 20 + 1, lpMsg->TempleNum, szAccountID, szName, lpMsg->Class, lpMsg->TotalScore, lpMsg->KillCount, lpMsg->RelicsGivenCount, lpMsg->Experience, lpMsg->IsWinner);
}

void CDataServerProtocol::ChaosCastleScore(PMSG_ANS_CHAOSCASTLE_RANKING * lpMsg)
{
	char szAccountID[11] = { 0 };
	char szName[11] = { 0 };
	szAccountID[10] = 0;
	szName[10] = 0;

	memcpy(szAccountID, lpMsg->AccountID, 10);
	memcpy(szName, lpMsg->GameID, 10);

	g_Log.Add("[CHAOS] SERVER:%d-%d, CASTLE:%d, ACCNT:%s CHAR:%s, CLASS:%d, PKILL:%d, MKILL:%d, EXPERIENCE:%d, WINNER:%d",
		lpMsg->ServerCode / 20 + 1, lpMsg->ServerCode % 20 + 1, lpMsg->Castle, szAccountID, szName, lpMsg->Class, lpMsg->PlayerKill, lpMsg->MonsterKill, lpMsg->Experience, lpMsg->IsWinner);

	this->m_RankingDB.ExecQuery("EXEC SP_POINT_ACCM_CC '%d', '%d', '%s', '%s', '%d', '%d', '%d', '%d', '%d'",
		lpMsg->ServerCode / 20 + 1, lpMsg->Castle, szAccountID, szName, lpMsg->Class, lpMsg->PlayerKill, lpMsg->MonsterKill, lpMsg->Experience, lpMsg->IsWinner);
}

void CDataServerProtocol::EGAnsEventChipInfo(int aIndex, PMSG_REQ_VIEW_EC_MN * lpMsg)
{
	int test = SQL_NO_DATA;
	PMSG_ANS_VIEW_EC_MN pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x01;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	memcpy(pMsg.szUID, lpMsg->szUID, sizeof(pMsg.szUID));
	pMsg.szUID[10] = 0;

	if (QueryResult* res = this->m_CharMiscDB.Fetch("SELECT EventChips, Check_Code, MuttoNumber FROM T_MU2003_EVENT WHERE AccountID='%s'", lpMsg->szUID) == FALSE)
	{
		pMsg.bSUCCESS = FALSE;
	}
	else
	{
		if (this->m_CharMiscDB.Fetch() != SQL_NO_DATA)
		{
			if (this->m_CharMiscDB.GetAsInteger("Check_Code") != 0)
			{
				pMsg.nEVENT_CHIPS = 0;
			}
			else
			{
				pMsg.nEVENT_CHIPS = (short)this->m_CharMiscDB.GetAsInteger("EventChips");
			}

			pMsg.iMUTO_NUM = this->m_CharMiscDB.GetAsInteger("MuttonNumber");
			pMsg.bSUCCESS = TRUE;
		}
		else
		{
			this->m_CharMiscDB.Close();
			if (this->m_CharMiscDB.ExecQuery("INSERT INTO T_MU2003_EVENT (AccountID) VALUES ('%s')", pMsg.szUID) == FALSE)
			{
				pMsg.bSUCCESS = FALSE;
			}
			else
			{
				pMsg.nEVENT_CHIPS = 0;
				pMsg.iMUTO_NUM = 0;
				pMsg.bSUCCESS = TRUE;
			}
		}
	}

	this->m_CharMiscDB.Close();
	//DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::EGAnsRegEventChipInfo(int aIndex, PMSG_REQ_REGISTER_EVENTCHIP * lpMsg)
{
	PMSG_ANS_REGISTER_EVENTCHIP pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x02;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	pMsg.Pos = lpMsg->Pos;

	strcpy(pMsg.szUID, lpMsg->szUID);
	pMsg.szUID[10] = 0x00;
	pMsg.bSUCCESS = FALSE;

	if (this->m_CharMiscDB.ExecQuery("UPDATE T_MU2003_EVENT SET EventChips = EventChips + 1 WHERE AccountID = '%s'", pMsg.szUID))
	{
		this->m_CharMiscDB.Close();

		if (QueryResult* res = this->m_CharMiscDB.Fetch("SELECT EventChips FROM T_MU2003_EVENT WHERE AccountID='%s'", pMsg.szUID))
		{
			if (this->m_CharMiscDB.Fetch() != SQL_NO_DATA)
			{
				pMsg.nEVENT_CHIPS = (short)this->m_CharMiscDB.GetAsInteger("EventChips");

				if (pMsg.nEVENT_CHIPS != -1)
				{
					pMsg.bSUCCESS = TRUE;
				}
			}
		}
	}

	this->m_CharMiscDB.Close();

	//DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::EGAnsResetEventChip(int aIndex, PMSG_REQ_RESET_EVENTCHIP * lpMsg)
{
	PMSG_ANS_RESET_EVENTCHIP pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x04;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	strcpy(pMsg.szUID, lpMsg->szUID);
	pMsg.szUID[10] = 0;

	if (this->m_CharMiscDB.ExecQuery("UPDATE T_MU2003_EVENT SET Check_Code = 2 WHERE AccountID = '%s'", pMsg.szUID))
	{
		pMsg.bSUCCESS = TRUE;
	}
	else
	{
		pMsg.bSUCCESS = FALSE;
	}

	this->m_CharMiscDB.Close();

	//DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::EGAnsEventStoneInfo(int aIndex, PMSG_REQ_VIEW_EC_MN * lpMsg)
{
	PMSG_ANS_VIEW_STONES pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x05;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	strcpy(pMsg.szUID, lpMsg->szUID);
	pMsg.szUID[10] = 0x00;

	if (QueryResult* res = this->m_EventDB.Fetch("SELECT StoneCount, Check_Code FROM T_BLOOD_CASTLE WHERE AccountID='%s'", pMsg.szUID) == FALSE)
	{
		pMsg.bSUCCESS = FALSE;
	}
	else
	{
		if (this->m_EventDB.Fetch() != SQL_NO_DATA)
		{
			if (this->m_EventDB.GetAsInteger("Check_Code") != 0)
			{
				pMsg.iStoneCount = 0;
			}
			else
			{
				pMsg.iStoneCount = this->m_EventDB.GetAsInteger("StoneCount");
			}
			pMsg.bSUCCESS = TRUE;
		}
		else
		{
			this->m_EventDB.Close();

			if (this->m_EventDB.ExecQuery("INSERT INTO T_BLOOD_CASTLE (AccountID) VALUES ('%s')", pMsg.szUID))
			{
				pMsg.bSUCCESS = TRUE;
				pMsg.iStoneCount = 0;
			}
		}
	}

	this->m_EventDB.Close();
	//DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::EGAnsRegEventStoneInfo(int aIndex, PMSG_REQ_REGISTER_STONES * lpMsg)
{
	PMSG_ANS_REGISTER_STONES pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x06;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	pMsg.iPosition = lpMsg->iPosition;
	strcpy(pMsg.szUID, lpMsg->szUID);
	pMsg.szUID[10] = 0x00;

	if (this->m_EventDB.ExecQuery("UPDATE T_BLOOD_CASTLE SET StoneCount = StoneCount + 1 WHERE AccountID = '%s'", pMsg.szUID))
	{
		this->m_EventDB.Close();
		if (QueryResult* res = this->m_EventDB.Fetch("SELECT StoneCount FROM T_BLOOD_CASTLE WHERE AccountID='%s'", pMsg.szUID))
		{
			if (this->m_EventDB.Fetch() != SQL_NO_DATA)
			{
				pMsg.iStoneCount = this->m_EventDB.GetAsInteger("StoneCount");

				if (pMsg.iStoneCount != -1)
				{
					pMsg.bSUCCESS = TRUE;
				}
			}
			else
			{
				pMsg.bSUCCESS = FALSE;
			}
		}
	}
	else
	{
		pMsg.bSUCCESS = FALSE;
	}

	this->m_EventDB.Close();
	//DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::EGAnsDeleteStones(int aIndex, PMSG_REQ_DELETE_STONES * lpMsg)
{
	PMSG_ANS_DELETE_STONES pMsg = { 0 };

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x07;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	strcpy(pMsg.szUID, lpMsg->szUID);
	pMsg.szUID[10] = 0x00;

	pMsg.bSUCCESS = FALSE;

	if (QueryResult* res = this->m_EventDB.Fetch("SELECT StoneCount FROM T_BLOOD_CASTLE WHERE AccountID='%s'", pMsg.szUID) == FALSE || this->m_EventDB.Fetch() == SQL_NO_DATA)
	{
		pMsg.bSUCCESS = FALSE;
	}
	else
	{
		int Stones = this->m_EventDB.GetAsInteger("StoneCount");

		this->m_EventDB.Close();

		if (Stones != -1 && Stones >= lpMsg->iStoneCount)
		{
			if (this->m_EventDB.ExecQuery("UPDATE T_BLOOD_CASTLE SET StoneCount = StoneCount - %d WHERE AccountID='%s'", lpMsg->iStoneCount, pMsg.szUID) == FALSE)
			{

			}
			else
			{
				pMsg.bSUCCESS = TRUE;
				pMsg.iStoneCount = Stones - lpMsg->iStoneCount;
			}
		}
	}

	this->m_EventDB.Close();
	//DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::EGAns2AnivRegSerial(int aIndex, PMSG_REQ_2ANIV_SERIAL * lpMsg)
{
	PMSG_ANS_2ANIV_SERIAL pMsg = { 0 };

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x08;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	char szName[11] = { 0 };
	szName[10] = 0;
	memcpy(szName, lpMsg->szUID, 10);
	pMsg.iINDEX = lpMsg->iINDEX;
	memcpy(pMsg.szUID, szName, 11);

	if (SpaceSyntexCheck(lpMsg->SERIAL1) == FALSE || QuoteSpaceSyntexCheck(lpMsg->SERIAL1) == FALSE || PercentSyntaxCheck(lpMsg->SERIAL1) == FALSE)
	{
		pMsg.btIsRegistered = 4;
		//DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
		return;
	}

	if (SpaceSyntexCheck(lpMsg->SERIAL2) == FALSE || QuoteSpaceSyntexCheck(lpMsg->SERIAL2) == FALSE || PercentSyntaxCheck(lpMsg->SERIAL2) == FALSE)
	{
		pMsg.btIsRegistered = 4;
		//DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
		return;
	}

	if (SpaceSyntexCheck(lpMsg->SERIAL3) == FALSE || QuoteSpaceSyntexCheck(lpMsg->SERIAL3) == FALSE || PercentSyntaxCheck(lpMsg->SERIAL3) == FALSE)
	{
		pMsg.btIsRegistered = 4;
		//DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
		return;
	}

	if (this->m_EventDB.ExecQuery("EXEC SP_REG_SERIAL '%s', '%d', '%s', '%s', '%s'", lpMsg->szUID, lpMsg->iMEMB_GUID, lpMsg->SERIAL1, lpMsg->SERIAL2, lpMsg->SERIAL3) && this->m_EventDB.Fetch() != SQL_NO_DATA)
	{
		pMsg.btIsRegistered = (BYTE)this->m_EventDB.GetAsInteger("RegResult");
		pMsg.iGiftNumber = this->m_EventDB.GetAsInteger("F_Register_Section");

		if (pMsg.iGiftNumber == -1)
		{
			pMsg.btIsRegistered = 4;
		}
	}
	else
	{
		pMsg.btIsRegistered = 4;
	}

	this->m_EventDB.Close();
	//DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::EGAnsResetStoneInfo(int aIndex, PMSG_REQ_RESET_EVENTCHIP * lpMsg)
{
	PMSG_ANS_RESET_EVENTCHIP pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x09;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	char szName[11] = { 0 };
	szName[10] = 0;
	memcpy(szName, lpMsg->szUID, 10);
	pMsg.iINDEX = lpMsg->iINDEX;
	memcpy(pMsg.szUID, szName, 11);
	pMsg.szUID[10] = 0;

	if (this->m_EventDB.ExecQuery("UPDATE T_BLOOD_CASTLE SET Check_Code = 2 WHERE AccountID = '%s'", lpMsg->szUID) == FALSE)
	{
		pMsg.bSUCCESS = FALSE;
	}
	else
	{
		pMsg.bSUCCESS = TRUE;
	}

	this->m_EventDB.Close();
	//DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::EGAnsRegCCOfflineGift(int aIndex, PMSG_REQ_REG_CC_OFFLINE_GIFT * lpMsg)
{
	PMSG_ANS_REG_CC_OFFLINE_GIFT pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x15;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	pMsg.wServerCode = lpMsg->wServerCode;
	strcpy(pMsg.szUID, lpMsg->szUID);
	strcpy(pMsg.szNAME, lpMsg->szNAME);
	pMsg.szUID[10] = 0x00;
	pMsg.szNAME[10] = 0x00;

	pMsg.iResultCode = 0;

	if (this->m_EventDB.ExecQuery("EXEC SP_REG_CC_OFFLINE_GIFT '%s', '%s', %d", lpMsg->szUID, lpMsg->szNAME, lpMsg->wServerCode) == FALSE || this->m_EventDB.Fetch() == SQL_NO_DATA)
	{

	}
	else
	{
		pMsg.iResultCode = this->m_EventDB.GetAsInteger("ResultCode");
		if (pMsg.iResultCode != -1)
		{
			this->m_EventDB.GetAsString("GiftName", pMsg.szGIFT_NAME, sizeof(pMsg.szGIFT_NAME) - 1);
		}
	}

	this->m_EventDB.Close();
	//DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::EGAnsRegDLOfflineGift(int aIndex, PMSG_REQ_REG_DL_OFFLINE_GIFT * lpMsg)
{
	PMSG_ANS_REG_DL_OFFLINE_GIFT pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x16;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	pMsg.wServerCode = lpMsg->wServerCode;
	strcpy(pMsg.szUID, lpMsg->szUID);
	strcpy(pMsg.szNAME, lpMsg->szNAME);
	pMsg.szUID[10] = 0x00;
	pMsg.szNAME[10] = 0x00;

	pMsg.iResultCode = 0;

	if (this->m_EventDB.ExecQuery("EXEC SP_REG_DL_OFFLINE_GIFT '%s', '%s', %d", lpMsg->szUID, lpMsg->szNAME, lpMsg->wServerCode) == FALSE || this->m_EventDB.Fetch() == SQL_NO_DATA)
	{

	}
	else
	{
		pMsg.iResultCode = this->m_EventDB.GetAsInteger("ResultCode");
		if (pMsg.iResultCode != -1)
		{
			this->m_EventDB.GetAsString("GiftName", pMsg.szGIFT_NAME, sizeof(pMsg.szGIFT_NAME));
		}
	}

	this->m_EventDB.Close();
	//DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::EGAnsRegHTOfflineGift(int aIndex, PMSG_REQ_REG_HT_OFFLINE_GIFT * lpMsg)
{
	PMSG_ANS_REG_HT_OFFLINE_GIFT pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x17;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	pMsg.wServerCode = lpMsg->wServerCode;
	strcpy(pMsg.szUID, lpMsg->szUID);
	strcpy(pMsg.szNAME, lpMsg->szNAME);
	pMsg.szUID[10] = 0x00;
	pMsg.szNAME[10] = 0x00;

	pMsg.iResultCode = 0;

	if (this->m_EventDB.ExecQuery("EXEC SP_REG_HT_OFFLINE_GIFT '%s', '%s', %d", lpMsg->szUID, lpMsg->szNAME, lpMsg->wServerCode) == FALSE || this->m_EventDB.Fetch() == SQL_NO_DATA)
	{

	}
	else
	{
		pMsg.iResultCode = this->m_EventDB.GetAsInteger("ResultCode");
		if (pMsg.iResultCode != -1)
		{
			this->m_EventDB.GetAsString("GiftName", pMsg.szGIFT_NAME, sizeof(pMsg.szGIFT_NAME));
		}
	}

	this->m_EventDB.Close();
	//DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::EGAnsLuckyCoinInfo(int aIndex, PMSG_REQ_LUCKYCOIN * lpMsg)
{
	int test = SQL_NO_DATA;
	PMSG_ANS_LUCKYCOIN pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x19;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iIndex = lpMsg->iIndex;
	memcpy(pMsg.szUID, lpMsg->szUID, sizeof(pMsg.szUID));
	pMsg.szUID[10] = 0;

	if (QueryResult* res = this->m_EventDB.Fetch("SELECT LuckyCoin FROM T_LuckyCoin WHERE AccountID='%s'", lpMsg->szUID) == FALSE)
	{
		pMsg.LuckyCoins = 0;
	}
	else
	{
		if (this->m_EventDB.Fetch() != SQL_NO_DATA)
		{
			pMsg.LuckyCoins = (short)this->m_EventDB.GetAsInteger("LuckyCoin");
		}
		else
		{
			this->m_EventDB.Close();
			if (this->m_EventDB.ExecQuery("INSERT INTO T_LuckyCoin (AccountID) VALUES ('%s')", pMsg.szUID) == FALSE)
			{
				pMsg.LuckyCoins = 0;
			}
			else
			{
				pMsg.LuckyCoins = 0;
			}
		}
	}

	this->m_EventDB.Close();
	//DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::EGAnsRegLuckyCoin(int aIndex, PMSG_REQ_REGISTER_LUCKYCOIN * lpMsg)
{
	PMSG_ANS_REG_LUCKYCOIN pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x18;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iIndex = lpMsg->iIndex;
	pMsg.Pos = lpMsg->btPos;

	strcpy(pMsg.szUID, lpMsg->szAccountID);
	pMsg.szUID[10] = 0x00;
	pMsg.Result = FALSE;

	if (this->m_EventDB.ExecQuery("UPDATE T_LuckyCoin SET LuckyCoin = LuckyCoin + 1 WHERE AccountID = '%s'", pMsg.szUID))
	{
		this->m_EventDB.Close();

		if (QueryResult* res = this->m_EventDB.Fetch("SELECT LuckyCoin FROM T_LuckyCoin WHERE AccountID='%s'", pMsg.szUID))
		{
			if (this->m_EventDB.Fetch() != SQL_NO_DATA)
			{
				pMsg.LuckyCoins = (short)this->m_EventDB.GetAsInteger("LuckyCoin");

				if (pMsg.LuckyCoins != -1)
				{
					pMsg.Result = TRUE;
				}
			}
		}
	}

	this->m_EventDB.Close();

	//DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::ReqSetExpandedWarehouse(short aIndex, SDHP_EXPANDEDWAREHOUSE_SET * aRecv)
{
	if (aRecv->ExpansionType == 2)
	{
		this->m_AccDB.ExecQuery("UPDATE AccountCharacter SET WarehouseExpansion=%d WHERE Id='%s'", aRecv->ExpansionLevel, aRecv->AccountID);
	}
	else if (aRecv->ExpansionType == 1)
	{
		this->m_AccDB.ExecQuery("UPDATE Character SET InventoryExpansion=%d WHERE Name='%s'", aRecv->ExpansionLevel, aRecv->AccountID);
	}
}

void CDataServerProtocol::EGReqSantaCheck(short aIndex, PMSG_REQ_SANTACHECK * aRecv)
{
	PMSG_ANS_SANTACHECK pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xBE, 0x20, sizeof(pMsg));

	pMsg.aIndex = aRecv->aIndex;
	pMsg.gGameServerCode = aRecv->gGameServerCode;
	memcpy(pMsg.AccountID, aRecv->AccountID, 11);

	this->m_EventDB.ExecQuery("EXEC SP_SANTA_CHECK '%s'", aRecv->AccountID);
	this->m_EventDB.Fetch();

	pMsg.Result = this->m_EventDB.GetAsInteger("Result");
	pMsg.UseCount = this->m_EventDB.GetAsInteger("VisitCount");

	this->m_EventDB.Close();

	//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::EGReqSantaGift(short aIndex, PMSG_REQ_SANTAGIFT * aRecv)
{
	PMSG_ANS_SANTAGIFT pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xBE, 0x21, sizeof(pMsg));

	pMsg.aIndex = aRecv->aIndex;
	pMsg.gGameServerCode = aRecv->gGameServerCode;
	memcpy(pMsg.AccountID, aRecv->AccountID, 11);

	this->m_EventDB.ExecQuery("EXEC SP_SANTA_GIFT '%s', %d", aRecv->AccountID, aRecv->gGameServerCode);
	this->m_EventDB.Fetch();

	pMsg.Result = this->m_EventDB.GetAsInteger("Result");
	pMsg.UseCount = this->m_EventDB.GetAsInteger("VisitCount");

	this->m_EventDB.Close();

	//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::ReqBanUser(short aIndex, BAN_REQ_USER * aRecv)
{
	if (aRecv->Type == 1)
	{
		this->m_AccDB.ExecQuery("UPDATE MEMB_INFO SET bloc_code=%d WHERE memb___id = '%s'", aRecv->Ban, aRecv->AccName);
		this->m_AccDB.Close();
	}
	else
	{
		this->m_AccDB.ExecQuery("UPDATE Character SET CtlCode=%d WHERE Name = '%s'", aRecv->Ban, aRecv->AccName);
		this->m_AccDB.Close();
	}
}

void CDataServerProtocol::ReqSecLock(short aIndex, SECLOCK_REQ_SAVE * aRecv)
{
	char szAccountID[11];
	szAccountID[10] = 0;
	memcpy(szAccountID, aRecv->AccountID, 10);

	this->m_AccDB.ExecQuery("UPDATE AccountCharacter SET SecCode=%d WHERE Id='%s'", aRecv->Code, szAccountID);
	this->m_AccDB.Close();
}

void CDataServerProtocol::ReqSaveMonsterCount(short aIndex, DS_SAVE_MONSTERCNT * aRecv)
{

	this->m_CharMiscDB.ExecQuery("EXEC IGC_Monster_KillCount_Save '%s', %d", aRecv->character, aRecv->type);
	this->m_CharMiscDB.Close();
	/*QueryResult* res = this->m_CharMiscDB.Fetch("SELECT * FROM C_Monster_KillCount WHERE name = '%s' AND MonsterId = %d",aRecv->character,aRecv->type);
	if(this->m_CharMiscDB.Fetch() == SQL_NO_DATA)
	{
	this->m_CharMiscDB.Close();
	this->m_CharMiscDB.ExecQuery("INSERT INTO C_Monster_KillCount (name, MonsterId, count) VALUES ('%s',%d,1)", aRecv->character,aRecv->type);//WHERE name = '%s' AND MonsterId = %d",aRecv->character,aRecv->type);
	}
	else
	{
	this->m_CharMiscDB.Close();
	this->m_CharMiscDB.ExecQuery("UPDATE C_Monster_KillCount SET count = count+1 WHERE name = '%s' AND MonsterId = %d",aRecv->character,aRecv->type);
	}*/
}

void CDataServerProtocol::ReqSavePlayerKiller(short aIndex, DS_SAVE_PLAYERKILLER * aRecv)
{
	this->m_CharMiscDB.ExecQuery("INSERT INTO C_PlayerKiller_Info (Victim, Killer,KillDate) VALUES ('%s','%s',GETDATE() )", aRecv->Victim, aRecv->Killer);
	this->m_CharMiscDB.Close();
}

void CDataServerProtocol::GDReqArcaBattleGuildJoin(int aIndex, PMSG_REQ_ARCA_BATTLE_GUILD_JOIN_DS * aRecv)
{
	PMSG_ANS_ARCA_BATTLE_GUILD_JOIN_DS pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x31, sizeof(pMsg));

	this->m_ArcaDB.ExecQuery("EXEC IGC_ArcaBattleGuildInsert '%s', '%s', %d", aRecv->szGuildName, aRecv->szGuildMaster, aRecv->dwGuild);
	this->m_ArcaDB.Fetch();

	pMsg.btResult = this->m_ArcaDB.GetResult(0);
	pMsg.wNumber = aRecv->wNumber;

	this->m_ArcaDB.Close();
	//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqArcaBattleGuildMemberJoin(int aIndex, PMSG_REQ_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS *aRecv)
{
	PMSG_ANS_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x33, sizeof(pMsg));

	this->m_ArcaDB.ExecQuery("EXEC IGC_ArcaBattleGuildMemberInsert '%s', '%s', %d", aRecv->szGuildName, aRecv->szCharName, aRecv->dwGuild);
	this->m_ArcaDB.Fetch();

	pMsg.btResult = this->m_ArcaDB.GetResult(0);
	pMsg.wNumber = aRecv->wNumber;

	this->m_ArcaDB.Close();
	//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqArcaBattleEnter(int aIndex, PMSG_REQ_ARCA_BATTLE_ENTER_DS *aRecv)
{
	PMSG_ANS_ARCA_BATTLE_ENTER_DS pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x35, sizeof(pMsg));

	this->m_ArcaDB.ExecQuery("EXEC IGC_ArcaBattleMemberSelect '%s'", aRecv->szCharName);
	this->m_ArcaDB.Fetch();

	pMsg.btResult = this->m_ArcaDB.GetResult(0);
	pMsg.wNumber = aRecv->wNumber;
	pMsg.btEnterSeq = aRecv->btEnterSeq;

	this->m_ArcaDB.Close();
	//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqArcaBattleWinGuildInfoInsert(int aIndex, PMSG_REQ_AB_WIN_GUILD_INFO_INSERT_DS *aRecv)
{
	PMSG_ANS_AB_WIN_GUILD_INFO_DS pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x3A, sizeof(pMsg));

	for (int i = 0; i < aRecv->btGuildCnt; i++)
	{
		this->m_ArcaDB.ExecQuery("EXEC IGC_ArcaBattleWinGuildInsert '%s', %d, %d, %d, %I64d",
			aRecv->m_stABWinGuildInfoDS[i].szGuildName, aRecv->m_stABWinGuildInfoDS[i].dwGuild,
			aRecv->m_stABWinGuildInfoDS[i].wOccupyObelisk, aRecv->m_stABWinGuildInfoDS[i].wObeliskGroup, 0);

		this->m_ArcaDB.Fetch();
		this->m_ArcaDB.Close();
	}

	for (int i = 0; i < aRecv->btGuildCnt; i++)
	{
		pMsg.m_stABWinGuildInfoDS[i].dwGuild = aRecv->m_stABWinGuildInfoDS[i].dwGuild;
		pMsg.m_stABWinGuildInfoDS[i].wObeliskGroup = aRecv->m_stABWinGuildInfoDS[i].wObeliskGroup;
		pMsg.m_stABWinGuildInfoDS[i].wOccupyObelisk = aRecv->m_stABWinGuildInfoDS[i].wOccupyObelisk;
		memcpy(pMsg.m_stABWinGuildInfoDS[i].szGuildName, aRecv->m_stABWinGuildInfoDS[i].szGuildName, MAX_GUILD_LEN + 1);
	}

	pMsg.btGuildCnt = aRecv->btGuildCnt;

	for (int i = 0; i<g_dwMaxServerGroups; i++)
	{
		if (g_Server[i].m_Index != -1
			&& g_Server[i].m_Type == g_Server[aIndex].m_Type
			&& g_Server[i].m_ServerGroup == aRecv->wMapSvrNum)
		{
			//DataSend(i, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
		}
	}
}

void CDataServerProtocol::GDReqArcaBattleWinGuildInfo(int aIndex, PMSG_REQ_AB_WIN_GUILD_INFO_DS *aRecv)
{
	PMSG_ANS_AB_WIN_GUILD_INFO_DS pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x3A, sizeof(pMsg));
	pMsg.btGuildCnt = 0;

	this->m_ArcaDB.ExecQuery("EXEC IGC_ArcaBattleWinGuildSelect");

	while (this->m_ArcaDB.Fetch() == SQL_SUCCESS)
	{
		this->m_ArcaDB.GetAsString("G_Name", pMsg.m_stABWinGuildInfoDS[pMsg.btGuildCnt].szGuildName, MAX_GUILD_LEN + 1);
		pMsg.m_stABWinGuildInfoDS[pMsg.btGuildCnt].dwGuild = this->m_ArcaDB.GetAsInteger("G_Number");
		pMsg.m_stABWinGuildInfoDS[pMsg.btGuildCnt].wOccupyObelisk = this->m_ArcaDB.GetAsInteger("OuccupyObelisk");
		pMsg.m_stABWinGuildInfoDS[pMsg.btGuildCnt].wObeliskGroup = this->m_ArcaDB.GetAsInteger("ObeliskGroup");
		pMsg.btGuildCnt++;

		if (pMsg.btGuildCnt >= 5)
		{
			break;
		}
	}

	this->m_ArcaDB.Close();
	//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqDeleteArcaBattleInfo(int aIndex)
{
	this->m_ArcaDB.ExecQuery("EXEC IGC_ArcaBattleInfoDelete");
	this->m_ArcaDB.Fetch();
	this->m_ArcaDB.Close();
}

void CDataServerProtocol::GDReqArcaBattleProcMultiCast(int aIndex, PMSG_REQ_AB_PROC_INSERT_DS *aRecv)
{
	PMSG_ANS_AB_PROC_STATE_DS pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x40, sizeof(pMsg));

	this->m_ArcaDB.ExecQuery("EXEC IGC_ArcaBattleProcInsert %d", aRecv->btProcState);
	this->m_ArcaDB.Fetch();
	this->m_ArcaDB.Close();

	pMsg.btProcState = aRecv->btProcState;

	for (int i = 0; i<g_dwMaxServerGroups; i++)
	{
		if (g_Server[i].m_Index != -1
			&& g_Server[i].m_Type == g_Server[aIndex].m_Type
			&& g_Server[i].m_ServerGroup == aRecv->wMapSvrNum)
		{
			//DataSend(i, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
		}
	}
}

void CDataServerProtocol::GDReqArcaBattleProcState(int aIndex, PMSG_REQ_AB_PROC_STATE_DS *aRecv)
{
	PMSG_ANS_AB_PROC_STATE_DS pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x40, sizeof(pMsg));

	this->m_ArcaDB.ExecQuery("EXEC IGC_ArcaBattleProcSelect");
	this->m_ArcaDB.Fetch();

	pMsg.btProcState = this->m_ArcaDB.GetResult(0);
	this->m_ArcaDB.Close();

	//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqArcaBattleJoinMemberUnder(int aIndex, PMSG_REQ_AB_JOIN_MEMBER_UNDER_DS *aRecv)
{
	PMSG_ANS_AB_JOIN_MEMBER_UNDER_DS pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x42, sizeof(pMsg));
	pMsg.btGuildCnt = 0;

	this->m_ArcaDB.ExecQuery("EXEC IGC_ArcaBattleGuildNamesSelect");
	int iGuildNamesCnt = 0;

	while (this->m_ArcaDB.Fetch() == SQL_SUCCESS)
	{
		this->m_ArcaDB.GetAsString("G_Name", pMsg.GuildMemberCnt[iGuildNamesCnt].szGuildNames, MAX_GUILD_LEN + 1);
		iGuildNamesCnt++;

		if (iGuildNamesCnt >= 6)
		{
			break;
		}
	}

	this->m_ArcaDB.Close();

	for (int i = 0; i < iGuildNamesCnt; i++)
	{
		this->m_ArcaDB.ExecQuery("EXEC IGC_ArcaBattleMemberUnderSelect '%s'", pMsg.GuildMemberCnt[i].szGuildNames);
		this->m_ArcaDB.Fetch();

		pMsg.GuildMemberCnt[i].btGuildMemberCnt = this->m_ArcaDB.GetResult(0);
		this->m_ArcaDB.Close();
	}

	pMsg.btGuildCnt = iGuildNamesCnt;

	for (int i = 0; i<g_dwMaxServerGroups; i++)
	{
		if (g_Server[i].m_Index != -1
			&& g_Server[i].m_Type == g_Server[aIndex].m_Type
			&& g_Server[i].m_ServerGroup == aRecv->wMapSvrNum)
		{
			//DataSend(i, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
		}
	}
}

void CDataServerProtocol::GDReqArcaBattleJoinMemberUnderReq(int aIndex, PMSG_REQ_AB_JOIN_CANCEL_DS *aRecv)
{
	PMSG_ANS_AB_JOIN_CANCEL_DS pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x44, sizeof(pMsg));
	pMsg.btGuildCnt = 0;

	this->m_ArcaDB.ExecQuery("EXEC IGC_ArcaBattleGuildNamesSelect");
	int iGuildNamesCnt = 0;
	char szGuildNames[6][MAX_GUILD_LEN + 1];

	while (this->m_ArcaDB.Fetch() == SQL_SUCCESS)
	{
		this->m_ArcaDB.GetAsString("G_Name", szGuildNames[iGuildNamesCnt], MAX_GUILD_LEN + 1);
		iGuildNamesCnt++;

		if (iGuildNamesCnt >= 6)
		{
			break;
		}
	}

	this->m_ArcaDB.Close();

	for (int i = 0; i < iGuildNamesCnt; i++)
	{
		this->m_ArcaDB.ExecQuery("EXEC IGC_ArcaBattleMinGuildSelect '%s', %d", szGuildNames[i], aRecv->btMinGuildMemNum);
		this->m_ArcaDB.Fetch();

		int iResult = this->m_ArcaDB.GetResult(0);

		if (iResult < 0)
		{
			memcpy(pMsg.CancelGuildNames[pMsg.btGuildCnt].szGuildNames, szGuildNames[i], MAX_GUILD_LEN + 1);
			pMsg.btGuildCnt++;
		}

		this->m_ArcaDB.Close();
	}

	for (int i = 0; i < pMsg.btGuildCnt; i++)
	{
		this->m_ArcaDB.ExecQuery("EXEC IGC_ArcaBattleMinGuildDelete '%s'", pMsg.CancelGuildNames[i].szGuildNames);
		this->m_ArcaDB.Fetch();
		this->m_ArcaDB.Close();
	}

	for (int i = 0; i<g_dwMaxServerGroups; i++)
	{
		if (g_Server[i].m_Index != -1
			&& g_Server[i].m_Type == g_Server[aIndex].m_Type
			&& g_Server[i].m_ServerGroup == aRecv->wMapSvrNum)
		{
			//DataSend(i, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
		}
	}
}

void CDataServerProtocol::GDReqArcaBattleRegisteredMemberCnt(int aIndex, PMSG_REQ_AB_REG_MEMBER_CNT_DS *aRecv)
{
	PMSG_ANS_AB_REG_MEMBER_CNT_DS pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x46, sizeof(pMsg));

	this->m_ArcaDB.ExecQuery("EXEC IGC_ArcaBattleGuildMemberSelect %d", aRecv->iGuildNumber);
	this->m_ArcaDB.Fetch();

	pMsg.btRegMemCnt = this->m_ArcaDB.GetResult(0);
	pMsg.wNumber = aRecv->iIndex;

	this->m_ArcaDB.Close();
	//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqRemoveAllGuildBuffMultiCast(int aIndex, PMSG_REQ_REMOVE_ALL_GUILD_BUFF_DS *aRecv)
{
	PMSG_ANS_REMOVE_ALL_GUILD_BUFF_DS pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x48, sizeof(pMsg));

	for (int i = 0; i<g_dwMaxServerGroups; i++)
	{
		if (g_Server[i].m_Index != -1
			&& g_Server[i].m_Type == g_Server[aIndex].m_Type
			&& g_Server[i].m_ServerGroup == aRecv->wMapSvrNum)
		{
			//DataSend(i, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
		}
	}
}

void CDataServerProtocol::GDReqArcaBattleMarkCnt(int aIndex, PMSG_REQ_ARCA_BATTLE_MARK_CNT_DS *aRecv)
{
	PMSG_ANS_ARCA_BATTLE_MARK_CNT_DS pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x4E, sizeof(pMsg));

	this->m_ArcaDB.ExecQuery("EXEC IGC_ArcaBattleMarkCntSelect %d", aRecv->dwGuildNum);
	this->m_ArcaDB.Fetch();

	int iResult = this->m_ArcaDB.GetResult(0);

	if (iResult < 0)
	{
		pMsg.dwMarkCnt = 0;
		pMsg.btResult = 1;
	}

	else
	{
		pMsg.dwMarkCnt = iResult;
		pMsg.btResult = 0;
	}

	pMsg.wNumber = aRecv->wNumber;
	this->m_ArcaDB.Close();

	//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqArcaBattleMarkReg(int aIndex, PMSG_REQ_ARCA_BATTLE_MARK_REG_DS *aRecv)
{
	PMSG_ANS_ARCA_BATTLE_MARK_REG_DS pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x50, sizeof(pMsg));

	this->m_ArcaDB.ExecQuery("EXEC IGC_ArcaBattleMarkInsert '%s', %d, '%s', %I64d", aRecv->szGuildName, aRecv->dwGuildNum, aRecv->szGuildMaster, (INT64)aRecv->dwMarkCnt);
	this->m_ArcaDB.Fetch();

	int iResult = this->m_ArcaDB.GetResult(0);
	this->m_ArcaDB.Close();

	if (iResult == 0)
	{
		g_Log.Add("[ArcaBattle] Error GDReqArcaBattleMarkReg() GuildName[%s] GuildNum[%d] GuildMaster[%s] MarkCnt[%d]",
			aRecv->szGuildName, aRecv->dwGuildNum, aRecv->szGuildMaster, aRecv->dwMarkCnt);

		pMsg.wNumber = aRecv->wNumber;
		//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);

		return;
	}
}

void CDataServerProtocol::GDReqArcaBattleMarkRank(int aIndex, PMSG_REQ_ARCA_BATTLE_MARK_RANK_DS *aRecv)
{
	PMSG_ANS_ARCA_BATTLE_MARK_RANK_DS pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x52, sizeof(pMsg));

	this->m_ArcaDB.ExecQuery("EXEC IGC_ArcaBattleTopRankSelect");
	pMsg.btGuildCnt = 0;

	while (this->m_ArcaDB.Fetch() == SQL_SUCCESS)
	{
		pMsg.ArcaBattleMarkTopRank[pMsg.btGuildCnt].btRank = pMsg.btGuildCnt + 1;
		this->m_ArcaDB.GetAsString("G_Name", pMsg.ArcaBattleMarkTopRank[pMsg.btGuildCnt].szGuildNames, MAX_GUILD_LEN + 1);
		pMsg.ArcaBattleMarkTopRank[pMsg.btGuildCnt].dwMarkCnt = this->m_ArcaDB.GetAsInteger("MarkCnt");
		pMsg.btGuildCnt++;

		if (pMsg.btGuildCnt >= 6)
		{
			break;
		}
	}

	this->m_ArcaDB.Close();
	this->m_ArcaDB.ExecQuery("EXEC IGC_ArcaBattleMarkRankSelect %d", aRecv->dwGuildNum);
	this->m_ArcaDB.Fetch();

	int iResult = this->m_ArcaDB.GetResult(0);

	if (iResult == 0 || pMsg.btGuildCnt == 0)
	{
		pMsg.btRank = 0;
		pMsg.dwMarkCnt = 0;
		pMsg.btGuildCnt = 0;
		pMsg.wNumber = aRecv->wNumber;

		//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
	}

	else
	{
		pMsg.btRank = iResult;
		pMsg.dwMarkCnt = this->m_ArcaDB.GetResult(1);
		pMsg.wNumber = aRecv->wNumber;

		//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
	}

	this->m_ArcaDB.Close();
}

void CDataServerProtocol::GDReqArcaBattleMarkRegDel(int aIndex, PMSG_REQ_ARCA_BATTLE_MARK_REG_DEL_DS *aRecv)
{
	this->m_ArcaDB.ExecQuery("EXEC IGC_ArcaBattleMarkRegDel %d", aRecv->dwGuildNum);
	this->m_ArcaDB.Fetch();
	this->m_ArcaDB.Close();
}

void CDataServerProtocol::GDReqArcaBattleIsTopRank(int aIndex, PMSG_REQ_ARCA_BATTLE_IS_TOP_RANK *aRecv)
{
	PMSG_ANS_ARCA_BATTLE_GUILD_JOIN_DS pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x2B, sizeof(pMsg));

	this->m_ArcaDB.ExecQuery("EXEC IGC_ArcaBattleMarkRankSelect %d", aRecv->dwGuildNum);
	this->m_ArcaDB.Fetch();
	int iRank = this->m_ArcaDB.GetResult(0);

	if (iRank <= 0 || iRank > 6)
	{
		pMsg.btResult = 6;
	}

	else
	{
		pMsg.btResult = 0;
	}

	this->m_ArcaDB.Close();
	pMsg.wNumber = aRecv->wNumber;

	//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqArcaBattleMarkRegAllDel(int aIndex)
{
	this->m_ArcaDB.ExecQuery("EXEC IGC_ArcaBattleGuildMarkInfoAllDel");
	this->m_ArcaDB.Fetch();
	this->m_ArcaDB.Close();
}

void CDataServerProtocol::GDReqArcaBattleAllGuildMarkCnt(int aIndex)
{
	PMSG_ANS_ALL_GUILD_MARK_CNT_DS pMsg;

	PHeadSubSetW((LPBYTE)&pMsg, 0xF8, 0xFC, sizeof(pMsg));

	this->m_ArcaDB.ExecQuery("EXEC IGC_ArcaBattleAllMarkCntSelect");
	pMsg.wGuildCnt = 0;

	while (this->m_ArcaDB.Fetch() == SQL_SUCCESS)
	{
		this->m_ArcaDB.GetAsString("G_Name", pMsg.ABAllGuildMark[pMsg.wGuildCnt].szGuildName, MAX_GUILD_LEN + 1);
		pMsg.ABAllGuildMark[pMsg.wGuildCnt].dwMarkCnt = this->m_ArcaDB.GetAsInteger("MarkCnt");
		pMsg.wGuildCnt++;

		if (pMsg.wGuildCnt >= 250)
		{
			break;
		}
	}

	this->m_ArcaDB.Close();
	//DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GDReqArcaBattleMarkRegSet(int aIndex, PMSG_REQ_AB_MARK_REG_UPDATE_DS *aRecv)
{
	this->m_ArcaDB.ExecQuery("UPDATE IGC_ARCA_BATTLE_GUILDMARK_REG SET MarkCnt = %d WHERE G_Number = %d", aRecv->dwMarkCnt, aRecv->dwGuildNum);
}

void CDataServerProtocol::GDReqArcaBattleGuildRegInit(int aIndex, PMSG_REQ_GUILD_REG_INIT *aRecv)
{
	this->m_ArcaDB.ExecQuery("DELETE IGC_ARCA_BATTLE_MEMBER_JOIN_INFO");
	this->m_ArcaDB.ExecQuery("DELETE IGC_ARCA_BATTLE_GUILD_JOIN_INFO");
	this->m_ArcaDB.ExecQuery("DELETE IGC_ARCA_BATTLE_GUILDMARK_REG");
}

void CDataServerProtocol::GDReqArcaBattleAllJoinUser(int aIndex, PMSG_REQ_AB_ALL_JOIN_USER_DS *aRecv)
{
	PMSG_ANS_AB_ALL_JOIN_USER_DS pMsg;

	PHeadSetW((LPBYTE)&pMsg, 0xE5, sizeof(pMsg));

	this->m_ArcaDB.ExecQuery("EXEC IGC_ArcaBattleAllJoinUserSelect");
	pMsg.btUserCnt = 0;

	while (this->m_ArcaDB.Fetch() == SQL_SUCCESS)
	{
		this->m_ArcaDB.GetAsString("G_Name", pMsg.stABJoinUserInfo[pMsg.btUserCnt].szGuildName, MAX_GUILD_LEN + 1);
		pMsg.stABJoinUserInfo[pMsg.btUserCnt].dwGuild = this->m_ArcaDB.GetAsInteger("Number");
		this->m_ArcaDB.GetAsString("CharName", pMsg.stABJoinUserInfo[pMsg.btUserCnt].szUserName, MAX_ACCOUNT_LEN + 1);
		pMsg.btUserCnt++;

		if (pMsg.btUserCnt >= 250)
		{
			break;
		}
	}

	this->m_ArcaDB.Close();
	//DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GDReqPeriodBuffInsert(int aIndex, PMSG_REQ_PERIODBUFF_INSERT *aRecv)
{
	this->m_PeriodItemDB.ExecQuery("EXEC IGC_PeriodBuffInsert '%s', %d, %d, %d, %d, %I64d", aRecv->szCharacterName, aRecv->wBuffIndex, aRecv->btEffectType1, aRecv->btEffectType2, aRecv->dwDuration, aRecv->lExpireDate);
}

void CDataServerProtocol::GDReqPeriodBuffDelete(int aIndex, PMSG_REQ_PERIODBUFF_DELETE *aRecv)
{
	this->m_PeriodItemDB.ExecQuery("EXEC IGC_PeriodBuffDelete '%s', %d", aRecv->szCharacterName, aRecv->wBuffIndex);
}

void CDataServerProtocol::GDReqPeriodBuffSelect(int aIndex, PMSG_REQ_PERIODBUFF_SELECT *aRecv)
{
	PMSG_ANS_PERIODBUFF_SELECT pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xE4, 0x03, sizeof(pMsg));
	memcpy(pMsg.szCharacterName, aRecv->szCharacterName, MAX_ACCOUNT_LEN + 1);
	pMsg.btResultCode = 1;
	pMsg.wUserIndex = aRecv->wUserIndex;

	this->m_PeriodItemDB.ExecQuery("EXEC IGC_PeriodBuffSelect '%s'", aRecv->szCharacterName);
	std::vector<int> vtDeleteBuffIndex;
	time_t currtime;

	while (this->m_PeriodItemDB.Fetch() == SQL_SUCCESS)
	{
		pMsg.wBuffIndex = this->m_PeriodItemDB.GetAsInteger("BuffIndex");
		pMsg.btEffectType1 = this->m_PeriodItemDB.GetAsInteger("EffectType1");
		pMsg.btEffectType2 = this->m_PeriodItemDB.GetAsInteger("EffectType2");
		pMsg.lExpireDate = this->m_PeriodItemDB.GetAsInteger("ExpireDate");

		currtime = time(NULL);

		if (currtime > pMsg.lExpireDate)
		{
			vtDeleteBuffIndex.push_back(pMsg.wBuffIndex);
		}

		else
		{
			//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
		}
	}

	this->m_PeriodItemDB.Close();

	for (std::vector<int>::iterator It = vtDeleteBuffIndex.begin(); It != vtDeleteBuffIndex.end(); It++)
	{
		this->m_PeriodItemDB.ExecQuery("EXEC IGC_PeriodBuffDelete '%s', %d", aRecv->szCharacterName, *It);
		this->m_PeriodItemDB.Close();
	}
}

void CDataServerProtocol::GDReqQuestExpInfoLoad(int aIndex, PMSG_REQ_QUESTEXP_INFO *aRecv)
{
	EnterCriticalSection(&this->m_QuestExpDBCriti);

	_QUESTEXP_INFO QuestInfo[25];
	PMSG_ANS_QUESTEXP_INFO pMsg;

	BYTE Buff[4096];
	int PacketSize = 0;
	int iCnt = 0;
	INT64 nLen;

	pMsg.btQuestCnt = 0;
	pMsg.iUserIndex = aRecv->iUserIndex;

	int arrEp[20] = { 0 };

	this->m_QuestExpDB.ExecQuery("EXEC IGC_QuestExpUserInfoLoad_Fir '%s'", aRecv->szCharName);

	while (this->m_QuestExpDB.Fetch() == SQL_SUCCESS)
	{
		bool bLoadCheck = true;
		int iEp = this->m_QuestExpDB.GetAsInteger("EPISODE");
		int iQS = this->m_QuestExpDB.GetAsInteger("QUEST_SWITCH");
		WORD wProgState = this->m_QuestExpDB.GetAsInteger("PROG_STATE");
		time_t lStartDate = this->m_QuestExpDB.GetAsInteger64("StartDateConvert");
		time_t lEndDate = this->m_QuestExpDB.GetAsInteger64("EndDateConvert");

		if (iEp < 0 || iEp > 20)
		{
			bLoadCheck = false;
		}

		if (iQS < 0 || iQS > 255)
		{
			bLoadCheck = false;
		}

		if (bLoadCheck == true)
		{
			QuestInfo[iCnt].dwQuestIndexID = (iEp << 16) + iQS;
			QuestInfo[iCnt].wProgState = wProgState;

			arrEp[iCnt] = iEp;

			QuestInfo[iCnt].lStartDate = lStartDate;
			QuestInfo[iCnt].lEndDate = lEndDate;

			iCnt++;

			if (iCnt >= 20)
			{
				break;
			}
		}
	}

	this->m_QuestExpDB.Close();

	char szTemp[4096];

	for (int i = 0; i < iCnt; i++)
	{
		sprintf(szTemp, "EXEC IGC_QuestExpUserInfoLoad_Sec '%s', %d, ?, ?, ?", aRecv->szCharName, arrEp[i]);

		this->m_QuestExpDB.BindParameterBinaryOutput(1, QuestInfo[i].btAskIndex, 5, &nLen);
		this->m_QuestExpDB.BindParameterBinaryOutput(2, QuestInfo[i].btAskValue, 5, &nLen);
		this->m_QuestExpDB.BindParameterBinaryOutput(3, QuestInfo[i].btAskState, 5, &nLen);

		this->m_QuestExpDB.ExecQuery(szTemp);
	}

	this->m_QuestExpDB.Close();
	PacketSize = sizeof(pMsg) + (iCnt * sizeof(_QUESTEXP_INFO));

	pMsg.btQuestCnt = iCnt;

	pMsg.head.c = 0xC2;
	pMsg.head.headcode = 0xF6;
	pMsg.head.sizeH = HIBYTE(PacketSize);
	pMsg.head.sizeL = LOBYTE(PacketSize);

	memcpy(&Buff, &pMsg, sizeof(pMsg));
	memcpy(&Buff[sizeof(pMsg)], QuestInfo, sizeof(_QUESTEXP_INFO) * pMsg.btQuestCnt);

	//DataSend(aIndex, Buff, PacketSize, __FUNCTION__);

	LeaveCriticalSection(&this->m_QuestExpDBCriti);
}

void CDataServerProtocol::GDReqQuestExpInfoSave(int aIndex, LPBYTE lpRecv)
{
	_PMSG_QUESTEXP_INFO * lpMsg = (_PMSG_QUESTEXP_INFO *)lpRecv;
	_QUESTEXP_INFO * QuestInfo = (_QUESTEXP_INFO *)(lpRecv + sizeof(_PMSG_QUESTEXP_INFO));

	if (lpMsg->btQuestCnt > 25)
	{
		return;
	}

	char szTemp[4096];

	EnterCriticalSection(&this->m_QuestExpDBCriti);

	for (int i = 0; i < lpMsg->btQuestCnt; i++)
	{
		QuestInfo = (_QUESTEXP_INFO *)(lpRecv + sizeof(_PMSG_QUESTEXP_INFO) + (i * sizeof(_QUESTEXP_INFO)));
		DWORD dwQuestInfoIndexID = QuestInfo->dwQuestIndexID;
		int iEpisode = dwQuestInfoIndexID >> 16;
		int iQuestSwitch = dwQuestInfoIndexID % 0x10000;

		if (iEpisode < 0 || iEpisode > 20)
		{
			continue;
		}

		if (iQuestSwitch < 0 || iQuestSwitch > 255)
		{
			continue;
		}

		WORD wProgState = QuestInfo->wProgState;
		time_t lStatrDate = QuestInfo->lStartDate;
		time_t lEndDate = QuestInfo->lEndDate;

		sprintf(szTemp, "EXEC IGC_QuestExpUserInfoSave '%s', %d, %d, %d, %I64d, %I64d, ?, ?, ?", lpMsg->szCharName, iEpisode, iQuestSwitch, wProgState, lStatrDate, lEndDate);

		int iRet = SQLPrepare(this->m_QuestExpDB.m_hStmt, (SQLTCHAR *)szTemp, SQL_NTS);

		if (iRet)
		{
			bool bReconnect = false;

			this->m_QuestExpDB.Diagnosis(bReconnect);
			this->m_QuestExpDB.Close();
			LeaveCriticalSection(&this->m_QuestExpDBCriti);
			return;
		}

		int size = 5;

		INT64 cbBlob;
		SQLBindParameter(this->m_QuestExpDB.m_hStmt, 1, SQL_PARAM_INPUT, -2, -3, size, 0, (SQLPOINTER)1, 0, &cbBlob);
		SQLBindParameter(this->m_QuestExpDB.m_hStmt, 2, SQL_PARAM_INPUT, -2, -3, size, 0, (SQLPOINTER)2, 0, &cbBlob);
		SQLBindParameter(this->m_QuestExpDB.m_hStmt, 3, SQL_PARAM_INPUT, -2, -3, size, 0, (SQLPOINTER)3, 0, &cbBlob);

		cbBlob = -size - 100LL;

		short retcode = SQLExecute(this->m_QuestExpDB.m_hStmt);

		void *pToken;

		if (retcode == SQL_NULL_DATA)
		{
			bool bReconnect = false;

			this->m_QuestExpDB.Diagnosis(bReconnect);
			this->m_QuestExpDB.Close();
			LeaveCriticalSection(&this->m_QuestExpDBCriti);
			return;
		}

		retcode = SQLParamData(this->m_QuestExpDB.m_hStmt, &pToken);

		if (retcode == SQL_NEED_DATA)
		{
			SQLPutData(this->m_QuestExpDB.m_hStmt, QuestInfo->btAskIndex, size);
		}

		retcode = SQLParamData(this->m_QuestExpDB.m_hStmt, &pToken);

		if (retcode == SQL_NEED_DATA)
		{
			SQLPutData(this->m_QuestExpDB.m_hStmt, QuestInfo->btAskValue, size);
		}

		retcode = SQLParamData(this->m_QuestExpDB.m_hStmt, &pToken);

		if (retcode == SQL_NEED_DATA)
		{
			SQLPutData(this->m_QuestExpDB.m_hStmt, QuestInfo->btAskState, size);
		}

		retcode = SQLParamData(this->m_QuestExpDB.m_hStmt, &pToken);

		this->m_QuestExpDB.Close();
		memset(szTemp, 0, 2048);
	}

	LeaveCriticalSection(&this->m_QuestExpDBCriti);
}

void CDataServerProtocol::GDReqLuckyItemInsert(int aIndex, PMSG_REQ_LUCKYITEM_INSERT *lpMsg)
{
	this->m_LuckyItemDB.ExecQuery("EXEC IGC_LuckyItemInsert %d, '%s', %d, %I64d, %d",
		lpMsg->dwUserGuid, lpMsg->szCharName, lpMsg->LuckyItemDBInfo.wItemCode,
		lpMsg->LuckyItemDBInfo.Serial, lpMsg->LuckyItemDBInfo.wDurabilitySmall);

	this->m_LuckyItemDB.Fetch();
	this->m_LuckyItemDB.Close();
}

void CDataServerProtocol::GDReqLuckyItemInsert2nd(int aIndex, PMSG_REQ_LUCKYITEM_INSERT_2ND *lpMsg)
{
	int iItemCnt = lpMsg->btItemCnt;

	if (iItemCnt <= 0)
	{
		return;
	}

	for (int i = 0; i < iItemCnt; i++)
	{
		this->m_LuckyItemDB.ExecQuery("EXEC IGC_LuckyItemInsert %d, '%s', %d, %I64d, %d",
			lpMsg->dwUserGuid, lpMsg->szCharName, lpMsg->LuckyItemDBInfo[i].wItemCode,
			lpMsg->LuckyItemDBInfo[i].Serial, lpMsg->LuckyItemDBInfo[i].wDurabilitySmall);

		this->m_LuckyItemDB.Fetch();
		this->m_LuckyItemDB.Close();
	}
}

void CDataServerProtocol::GDReqLuckyItemDelete(int aIndex, PMSG_REQ_LUCKYITEM_DELETE *lpMsg)
{
	this->m_LuckyItemDB.ExecQuery("EXEC IGC_LuckyItemDelete '%s', %d, %u", lpMsg->szCharName, lpMsg->wItemCode, lpMsg->Serial);
	this->m_LuckyItemDB.Fetch();
	this->m_LuckyItemDB.Close();
}

void CDataServerProtocol::GDReqLuckyItemSelect(int aIndex, PMSG_REQ_LUCKYITEM_SELECT *lpMsg)
{
	char BUFFER[2048];
	memset(BUFFER, 0x00, sizeof(BUFFER));

	int lOfs = sizeof(PMSG_ANS_LUCKYITEM_SELECT);

	PMSG_LUCKYITME_DB_INFO LuckyItemInfo[45] = { 0 };
	int iItemCount = 0;

	this->m_LuckyItemDB.ExecQuery("EXEC IGC_LuckyItemSelect '%s'", lpMsg->chCharacterName);

	while (this->m_LuckyItemDB.Fetch() == SQL_SUCCESS)
	{
		LuckyItemInfo[iItemCount].wItemCode = this->m_LuckyItemDB.GetAsInteger("ItemCode");
		LuckyItemInfo[iItemCount].Serial = this->m_LuckyItemDB.GetAsInteger64("ItemSerial");
		LuckyItemInfo[iItemCount].wDurabilitySmall = this->m_LuckyItemDB.GetAsInteger("DurabilitySmall");

		iItemCount++;

		if (iItemCount >= 45)
			break;
	}

	this->m_LuckyItemDB.Close();

	PMSG_ANS_LUCKYITEM_SELECT pMsg;
	pMsg.btItemCnt = iItemCount;
	pMsg.wUserIndex = lpMsg->wUserIndex;

	memcpy(&BUFFER[lOfs], LuckyItemInfo, pMsg.btItemCnt * sizeof(PMSG_LUCKYITME_DB_INFO));

	lOfs += (pMsg.btItemCnt * sizeof(PMSG_LUCKYITME_DB_INFO));

	pMsg.head.set((LPBYTE)&pMsg, 0xD4, lOfs);
	memcpy(BUFFER, &pMsg, sizeof(pMsg));

	//DataSend(aIndex, (LPBYTE)&BUFFER, lOfs, __FUNCTION__);
}

void CDataServerProtocol::GDReqGetPentagramJewel(int aIndex, PMSG_REQ_PENTAGRAMJEWEL *lpMsg)
{
	BYTE Buffer[4096];
	memset(&Buffer, 0x00, sizeof(Buffer));

	int dwSize = sizeof(PMSG_ANS_PENTAGRAMJEWEL);
	int iCount = 0;

	PMSG_ANS_PENTAGRAMJEWEL pMsg;
	PENTAGRAMJEWEL_INFO m_PentagramJewelInfo;

	this->m_PentagramDB.ExecQuery("EXEC IGC_PentagramInfoSelect %d, '%s', '%s', %d", lpMsg->iUserGuid, lpMsg->szAccountID, lpMsg->szName, lpMsg->btJewelPos);

	while (this->m_PentagramDB.Fetch() == SQL_SUCCESS)
	{
		m_PentagramJewelInfo.btJewelPos = lpMsg->btJewelPos;
		m_PentagramJewelInfo.btJewelIndex = this->m_PentagramDB.GetAsInteger("JewelIndex");
		m_PentagramJewelInfo.btItemType = this->m_PentagramDB.GetAsInteger("ItemType");
		m_PentagramJewelInfo.wItemIndex = this->m_PentagramDB.GetAsInteger("ItemIndex");
		m_PentagramJewelInfo.btMainAttribute = this->m_PentagramDB.GetAsInteger("MainAttribute");
		m_PentagramJewelInfo.btLevel = this->m_PentagramDB.GetAsInteger("JewelLevel");
		m_PentagramJewelInfo.btRank1OptionNum = this->m_PentagramDB.GetAsInteger("Rank1");
		m_PentagramJewelInfo.btRank1Level = this->m_PentagramDB.GetAsInteger("Rank1Level");
		m_PentagramJewelInfo.btRank2OptionNum = this->m_PentagramDB.GetAsInteger("Rank2");
		m_PentagramJewelInfo.btRank2Level = this->m_PentagramDB.GetAsInteger("Rank2Level");
		m_PentagramJewelInfo.btRank3OptionNum = this->m_PentagramDB.GetAsInteger("Rank3");
		m_PentagramJewelInfo.btRank3Level = this->m_PentagramDB.GetAsInteger("Rank3Level");
		m_PentagramJewelInfo.btRank4OptionNum = this->m_PentagramDB.GetAsInteger("Rank4");
		m_PentagramJewelInfo.btRank4Level = this->m_PentagramDB.GetAsInteger("Rank4Level");
		m_PentagramJewelInfo.btRank5OptionNum = this->m_PentagramDB.GetAsInteger("Rank5");
		m_PentagramJewelInfo.btRank5Level = this->m_PentagramDB.GetAsInteger("Rank5Level");

		memcpy(&Buffer[dwSize], &m_PentagramJewelInfo, sizeof(PENTAGRAMJEWEL_INFO));
		dwSize += sizeof(PENTAGRAMJEWEL_INFO);
		iCount++;
	}

	this->m_PentagramDB.Close();

	pMsg.btJewelCnt = iCount;
	pMsg.iAnsType = lpMsg->btJewelPos;
	pMsg.iUserIndex = lpMsg->iUserIndex;
	PHeadSetW((LPBYTE)&pMsg, 0xE0, dwSize);

	memcpy(&Buffer, &pMsg, sizeof(pMsg));

	//DataSend(aIndex, Buffer, dwSize, __FUNCTION__);
}

void CDataServerProtocol::GDReqSetPentagramJewel(int aIndex, LPBYTE lpRecv)
{
	PMSG_REQ_SETPENTAGRAMJEWEL * lpMsg = (PMSG_REQ_SETPENTAGRAMJEWEL *)(lpRecv);

	for (int i = 0; i < lpMsg->btJewelCnt; i++)
	{
		PENTAGRAMJEWEL_INFO * lpMsgBody = (PENTAGRAMJEWEL_INFO *)(lpRecv + sizeof(PMSG_REQ_SETPENTAGRAMJEWEL) + (i * sizeof(PENTAGRAMJEWEL_INFO)));

		this->m_PentagramDB.ExecQuery("EXEC IGC_PentagramInfoUpdate %d, '%s', '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
			lpMsg->iUserGuid, lpMsg->szAccountID, lpMsg->szName, lpMsg->btJewelPos, lpMsgBody->btJewelIndex, lpMsgBody->btItemType, lpMsgBody->wItemIndex,
			lpMsgBody->btMainAttribute, lpMsgBody->btLevel, lpMsgBody->btRank1OptionNum, lpMsgBody->btRank1Level, lpMsgBody->btRank2OptionNum, lpMsgBody->btRank2Level,
			lpMsgBody->btRank3OptionNum, lpMsgBody->btRank3Level, lpMsgBody->btRank4OptionNum, lpMsgBody->btRank4Level, lpMsgBody->btRank5OptionNum, lpMsgBody->btRank5Level);

		this->m_PentagramDB.Close();
	}
}

void CDataServerProtocol::GDReqDelPentagramJewel(int aIndex, PMSG_DEL_PENTAGRAMJEWEL *lpMsg)
{
	this->m_PentagramDB.ExecQuery("EXEC IGC_PentagramInfoDelete %d, '%s', '%s', %d, %d",
		lpMsg->iUserGuid, lpMsg->szAccountID, lpMsg->szName, lpMsg->btJewelPos, lpMsg->btJewelIndex);

	this->m_PentagramDB.Close();
}

void CDataServerProtocol::GDReqInsertPentagramJewel(int aIndex, PMSG_INSERT_PENTAGRAMJEWEL *lpMsg)
{
	this->m_PentagramDB.ExecQuery("EXEC IGC_PentagramInfoUpdate %d, '%s', '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
		lpMsg->iUserGuid, lpMsg->szAccountID, lpMsg->szName, lpMsg->btJewelPos, lpMsg->btJewelPos, lpMsg->btItemType, lpMsg->wItemIndex,
		lpMsg->btMainAttribute, lpMsg->btLevel, lpMsg->btRank1OptionNum, lpMsg->btRank1Level, lpMsg->btRank2OptionNum, lpMsg->btRank2Level,
		lpMsg->btRank3OptionNum, lpMsg->btRank3Level, lpMsg->btRank4OptionNum, lpMsg->btRank4Level, lpMsg->btRank5OptionNum, lpMsg->btRank5Level);

	this->m_PentagramDB.Close();
}

void CDataServerProtocol::GDReqChaosCastleFinalSave(int aIndex, PMSG_REQ_SAVE_CCF_RESULT* lpMsg)
{
	this->m_CCFinalDB.ExecQuery("EXEC IGC_ChaosCastleFinal_Save '%s', %d, %d, %d, %I64d, %d", lpMsg->szCharName, lpMsg->nPoint, lpMsg->nCharClass, lpMsg->nCharLevel, lpMsg->nCharExp, lpMsg->byCCFType);
	this->m_CCFinalDB.Fetch();
	this->m_CCFinalDB.Close();
}

void CDataServerProtocol::GDReqChaosCastleFinalPermission(int aIndex, PMSG_REQ_CCF_PERMISSION* lpMsg)
{
	SDHP_ANS_CCF_GETPERMISSION pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xF9, 0xA3, sizeof(pMsg));

	this->m_CCFinalDB.ExecQuery("EXEC IGC_ChaosCastleFinal_GetPermission '%s', %d", lpMsg->szCharName, lpMsg->byCCFType);

	if (this->m_CCFinalDB.Fetch() == SQL_NO_DATA)
	{
		pMsg.nResult = -1;
	}

	else
	{
		pMsg.nResult = this->m_CCFinalDB.GetResult(0);
	}

	this->m_CCFinalDB.Close();

	pMsg.nIndex = lpMsg->nIndex;

	//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqChaosCastleFinalLoad(int aIndex, SDHP_REQ_CCF_RANKING* lpMsg)
{
	PMSG_ANS_CCF_RANK pMsg;
	PHeadSetW((LPBYTE)&pMsg, 0xF5, sizeof(pMsg));

	if (lpMsg->nServerCategory == 1)
	{
		this->m_CCFinalDB.ExecQuery("EXEC IGC_ChaosCastleFinal_GetRank_r %d", lpMsg->byRankingType);
	}

	else
	{
		this->m_CCFinalDB.ExecQuery("EXEC IGC_ChaosCastleFinal_GetRank %d", lpMsg->byRankingType);
	}

	pMsg.byUserCnt = 0;

	while (this->m_CCFinalDB.Fetch() == SQL_SUCCESS)
	{
		pMsg.RankingInfo[pMsg.byUserCnt].byRank = this->m_CCFinalDB.GetAsInteger("mRank");
		this->m_CCFinalDB.GetAsString("mName", pMsg.RankingInfo[pMsg.byUserCnt].szCharName, MAX_ACCOUNT_LEN + 1);
		pMsg.RankingInfo[pMsg.byUserCnt].nPoint = this->m_CCFinalDB.GetAsInteger("mPoint");

		pMsg.byUserCnt++;

		if (pMsg.byUserCnt >= 50)
			break;
	}

	this->m_CCFinalDB.Close();

	//DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GDReqChaosCastleFinalRenew(int aIndex, SDHP_RENEW_RANKING* lpMsg)
{
	this->m_CCFinalDB.ExecQuery("EXEC IGC_ChaosCastleFinal_Renew %d", lpMsg->byRankingType);
	this->m_CCFinalDB.Fetch();
	this->m_CCFinalDB.Close();
}

void CDataServerProtocol::GDReqChaosCastleFinalSendMsgAllSvr(int aIndex, PMSG_SEND_CCF_INFO_ALL_SVR* lpMsg)
{
	SDHP_ANS_CCF_INFO_NOTIFY pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xF9, 0xA4, sizeof(pMsg));

	pMsg.nMin = lpMsg->nMin;
	pMsg.nType = lpMsg->nType;

	for (int i = 0; i < g_dwMaxServerGroups; i++)
	{
		if (g_Server[i].m_Index != -1
			&& g_Server[i].m_Type == g_Server[aIndex].m_Type
			&& g_Server[i].m_ServerGroup == lpMsg->wMapSvrNum)
		{
			//DataSend(i, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
		}
	}
}

void CDataServerProtocol::GDReqLoadMuunInvenItem(int aIndex, SDHP_REQ_DBMUUN_INVEN_LOAD* lpMsg)
{
	_tagSDHP_ANS_DBMUUN_INVEN_LOAD pMsg;
	PHeadSetW((LPBYTE)&pMsg, 0xF1, sizeof(pMsg));

	char szTemp[250];
	QueryResult* res = this->m_MuunDB.Fetch("SELECT * FROM PetWarehouse WHERE Name= '%s'", lpMsg->Name);

	pMsg.aIndex = lpMsg->aIndex;

	if (this->m_MuunDB.Fetch() == SQL_NO_DATA)
	{
		this->m_MuunDB.Close();
		this->m_MuunDB.ExecQuery("INSERT INTO PetWarehouse (AccountID,Name,DbVersion,SubEquip) VALUES ('%s', '%s','%d','%d')", lpMsg->AccountID, lpMsg->Name, 3, 0);

		pMsg.SubEquip = 0;
		memset(pMsg.dbItems, -1, sizeof(pMsg.dbItems));

		sprintf(szTemp, "UPDATE PetWarehouse SET Items=? WHERE Name='%s'", lpMsg->Name);
		this->m_MuunDB.SetAsBinary(szTemp, pMsg.dbItems, sizeof(pMsg.dbItems));
	}

	else
	{
		pMsg.SubEquip = this->m_MuunDB.GetAsInteger("SubEquip");
		this->m_MuunDB.Close();

		sprintf(szTemp, "SELECT Items from PetWarehouse WHERE Name= '%s'", lpMsg->Name);
		this->m_MuunDB.GetAsBinary(szTemp, pMsg.dbItems, 3264);
	}

	//DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GDReqSaveMuunInvenItem(int aIndex, SDHP_REQ_DBMUUN_INVEN_SAVE * aRecv)
{
	char szName[11] = { 0 };
	memcpy(szName, aRecv->Name, MAX_ACCOUNT_LEN); //
	szName[MAX_ACCOUNT_LEN] = 0; // crash fix

	char szTemp[256];
	wsprintf(szTemp, "UPDATE PetWarehouse SET Items=? WHERE Name='%s'", szName);

	this->m_MuunDB.SetAsBinary(szTemp, aRecv->dbInventory, sizeof(aRecv->dbInventory));
}

void CDataServerProtocol::GDReqLoadEventInvenItem(int aIndex, SDHP_REQ_DBEVENT_INVEN_LOAD * aRecv)
{
	_tagSDHP_ANS_DBEVENT_INVEN_LOAD pMsg;
	PHeadSetW((LPBYTE)&pMsg, 0xE6, sizeof(pMsg));

	char szTemp[1024];
	QueryResult* res = this->m_EventInvDB.Fetch("SELECT * FROM T_Event_Inventory WHERE Name = '%s'", aRecv->Name);

	pMsg.aIndex = aRecv->aIndex;

	if (this->m_EventInvDB.Fetch() == SQL_NO_DATA)
	{
		this->m_EventInvDB.Close();
		this->m_EventInvDB.ExecQuery("INSERT INTO T_Event_Inventory (AccountID, Name) VALUES ('%s', '%s')", aRecv->AccountID, aRecv->Name);

		memset(pMsg.dbItems, -1, sizeof(pMsg.dbItems));

		sprintf(szTemp, "UPDATE T_Event_Inventory SET Inventory=? WHERE Name = '%s'", aRecv->Name);
		this->m_EventInvDB.SetAsBinary(szTemp, pMsg.dbItems, sizeof(pMsg.dbItems));
	}

	else
	{
		this->m_EventInvDB.Close();

		sprintf(szTemp, "SELECT Inventory FROM T_Event_Inventory WHERE Name = '%s'", aRecv->Name);
		this->m_EventInvDB.GetAsBinary(szTemp, pMsg.dbItems, sizeof(pMsg.dbItems));
	}

	//DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GDReqSaveEventInvenItem(int aIndex, SDHP_REQ_DBEVENT_INVEN_SAVE * aRecv)
{
	char szTemp[1024];
	wsprintf(szTemp, "UPDATE T_Event_Inventory SET Inventory=? WHERE Name = '%s'", aRecv->Name);
	this->m_EventInvDB.SetAsBinary(szTemp, aRecv->dbInventory, sizeof(aRecv->dbInventory));
}

void CDataServerProtocol::GDReqCardInfo(int aIndex, PMSG_REQ_MURUMMY_SELECT_DS * aRecv)
{
	PMSG_ANS_MURUMMY_SELECT_DS pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xE8, 0x00, sizeof(pMsg));

	int iCount = 0;

	this->m_RummyDB.ExecQuery("EXEC IGC_SetSelectMuRummy '%s', '%s'", aRecv->AccountID, aRecv->Name);
	while (this->m_RummyDB.Fetch() == SQL_SUCCESS)
	{
		pMsg.wScore = this->m_RummyDB.GetAsInteger("mTotalScore");
		pMsg.stMuRummyCardInfoDS[iCount].btColor = this->m_RummyDB.GetAsInteger("mColor");
		pMsg.stMuRummyCardInfoDS[iCount].btNumber = this->m_RummyDB.GetAsInteger("mNumber");
		pMsg.stMuRummyCardInfoDS[iCount].btSlotNum = this->m_RummyDB.GetAsInteger("mPosition");
		pMsg.stMuRummyCardInfoDS[iCount].btStatus = this->m_RummyDB.GetAsInteger("mStatus");
		pMsg.stMuRummyCardInfoDS[iCount].btSeq = this->m_RummyDB.GetAsInteger("mSequence");

		iCount++;
	}

	this->m_RummyDB.Close();

	if (iCount <= 0)
	{
		pMsg.btResult = 0;
	}

	else
	{
		pMsg.btResult = 1;
	}

	pMsg.aIndex = aRecv->aIndex;

	//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqCardInfoInsert(int aIndex, PMSG_REQ_MURUMMY_INSERT_DS * aRecv)
{
	this->m_RummyDB.ExecQuery("EXEC IGC_SetSaveMuRummyInfo '%s', '%s', %d", aRecv->AccountID, aRecv->Name, 0);
	this->m_RummyDB.Fetch();
	this->m_RummyDB.Close();

	for (int i = 0; i < 24; i++)
	{
		this->m_RummyDB.ExecQuery("EXEC IGC_SetInsertMuRummy '%s', '%s',%d, %d, %d, %d, %d", aRecv->AccountID, aRecv->Name,
			aRecv->stMuRummyCardInfoDS[i].btNumber, aRecv->stMuRummyCardInfoDS[i].btColor, aRecv->stMuRummyCardInfoDS[i].btSlotNum, aRecv->stMuRummyCardInfoDS[i].btStatus, aRecv->stMuRummyCardInfoDS[i].btSeq);

		this->m_RummyDB.Fetch();
		this->m_RummyDB.Close();
	}
}

void CDataServerProtocol::GDReqScoreUpdate(int aIndex, PMSG_REQ_MURUMMY_SCORE_UPDATE_DS * aRecv)
{
	this->m_RummyDB.ExecQuery("EXEC IGC_SetSaveMuRummyInfo '%s', '%s', %d", aRecv->AccountID, aRecv->Name, aRecv->wScore);
	this->m_RummyDB.Fetch();
	this->m_RummyDB.Close();

	for (int i = 0; i < 3; i++)
	{
		if (aRecv->stCardUpdateDS[i].btSeq >= 24)
		{
			return;
		}

		this->m_RummyDB.ExecQuery("EXEC IGC_SetUpdateMuRummy  '%s', '%s', %d, %d, %d", aRecv->AccountID, aRecv->Name, aRecv->stCardUpdateDS[i].btSlotNum, aRecv->stCardUpdateDS[i].btStatus, aRecv->stCardUpdateDS[i].btSeq);
		this->m_RummyDB.Fetch();
		this->m_RummyDB.Close();
	}
}

void CDataServerProtocol::GDReqCardInfoUpdate(int aIndex, PMSG_REQ_MURUMMY_UPDATE_DS * aRecv)
{
	this->m_RummyDB.ExecQuery("EXEC IGC_SetUpdateMuRummy  '%s', '%s', %d, %d, %d", aRecv->AccountID, aRecv->Name, aRecv->btSlotNum, aRecv->btStatus, aRecv->btSequence);
	this->m_RummyDB.Fetch();
	this->m_RummyDB.Close();
}

void CDataServerProtocol::GDReqScoreDelete(int aIndex, PMSG_REQ_MURUMMY_DELETE_DS * aRecv)
{
	this->m_RummyDB.ExecQuery("EXEC IGC_SetDeleteMuRummy '%s', '%s'", aRecv->AccountID, aRecv->Name);
	this->m_RummyDB.Fetch();
	this->m_RummyDB.Close();
}

void CDataServerProtocol::GDReqSlotInfoUpdate(int aIndex, PMSG_REQ_MURUMMY_SLOTUPDATE_DS * aRecv)
{
	if (aRecv->stCardUpdateDS.btSeq == 255 || aRecv->stCardUpdateDS.btSeq >= 8)
	{
		return;
	}

	this->m_RummyDB.ExecQuery("EXEC IGC_SetUpdateMuRummy  '%s', '%s', %d, %d, %d", aRecv->AccountID, aRecv->Name, aRecv->stCardUpdateDS.btSlotNum, aRecv->stCardUpdateDS.btStatus, aRecv->stCardUpdateDS.btSeq);
	this->m_RummyDB.Fetch();
	this->m_RummyDB.Close();
}

void CDataServerProtocol::GDReqMuRummyInfoUpdate(int aIndex, PMSG_REQ_MURUMMY_INFO_UPDATE_DS * aRecv)
{
	this->m_RummyDB.ExecQuery("EXEC IGC_SetSaveMuRummyInfo '%s', '%s', %d", aRecv->AccountID, aRecv->Name, aRecv->wScore);
	this->m_RummyDB.Fetch();
	this->m_RummyDB.Close();

	for (int i = 0; i < 3; i++)
	{
		if (aRecv->stMuRummyCardUpdateDS[i].btSeq >= 24)
		{
			return;
		}

		this->m_RummyDB.ExecQuery("EXEC IGC_SetUpdateMuRummy  '%s', '%s', %d, %d, %d", aRecv->AccountID, aRecv->Name, aRecv->stMuRummyCardUpdateDS[i].btSlotNum, aRecv->stMuRummyCardUpdateDS[i].btStatus, aRecv->stMuRummyCardUpdateDS[i].btSeq);
		this->m_RummyDB.Fetch();
		this->m_RummyDB.Close();
	}
}

void CDataServerProtocol::GDReqMuRummyDBLog(int aIndex, PMSG_REQ_MURUMMY_LOG_INSERT_DS * aRecv)
{
	this->m_RummyDB.ExecQuery("EXEC IGC_SetSaveMuRummyLog '%s', '%s', %d", aRecv->AccountID, aRecv->Name, aRecv->wScore);
	this->m_RummyDB.Fetch();
	this->m_RummyDB.Close();
}

void CDataServerProtocol::GDReqMineModifyUPTUserInfo(int aIndex, SDHP_REQ_MINESYSTEM_UPT_USERINFO * aRecv)
{
	if (aRecv->byRequestType == 0)
	{
		this->m_MineDB.ExecQuery("EXEC IGC_MineSystem_Insert_UPTUserInfo '%s', %d, %d", aRecv->szCharName, aRecv->wTwinkleType, aRecv->iCurrentStage);
		this->m_MineDB.Fetch();
		this->m_MineDB.Close();
	}

	else if (aRecv->byRequestType == 1)
	{
		this->m_MineDB.ExecQuery("EXEC IGC_MineSystem_Delete_UPTUserInfo '%s'", aRecv->szCharName);
		this->m_MineDB.Fetch();
		this->m_MineDB.Close();
	}

	SDHP_ANS_MINESYSTEM_UPT_USERINFO pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0x4C, 0x00, sizeof(pMsg));

	pMsg.byRequestType = aRecv->byRequestType;
	pMsg.wUserIndex = aRecv->wUserIndex;
	pMsg.Result = TRUE;

	//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqMineCheckIsUPTWhenUserConnect(int aIndex, SDHP_REQ_LOAD_MINESYSTEM_UPT_USERINFO * aRecv)
{
	SDHP_ANS_LOAD_MINESYSTEM_UPT_USERINFO pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0x4C, 0x01, sizeof(pMsg));

	this->m_MineDB.ExecQuery("EXEC IGC_MineSystem_Select_UPTUserInfo '%s'", aRecv->szCharName);

	if (this->m_MineDB.Fetch() == SQL_NO_DATA)
	{
		pMsg.byResult = 0;
	}

	else
	{
		pMsg.byResult = 1;
		this->m_MineDB.GetAsString("CharacterName", pMsg.szCharName, MAX_ACCOUNT_LEN + 1);
		pMsg.wTwinkleType = this->m_MineDB.GetAsInteger("TwinkleType");
		pMsg.iCurrentStage = this->m_MineDB.GetAsInteger("CurrentStage");
	}

	this->m_MineDB.Close();

	pMsg.wUserIndex = aRecv->wUserIndex;
	//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqPShopItemValue(int aIndex, PMSG_REQ_PSHOPITEMVALUE_INFO * aRecv)
{
	PMSG_ANS_PSHOPITEMVALUE_INFO pMsg;
	PHeadSetW((LPBYTE)&pMsg, 0xE9, sizeof(pMsg));

	int count = 0;

	this->m_PShopDB.ExecQuery("EXEC IGC_PShopItemValueInfoLoad '%s', '%s'", aRecv->AccountId, aRecv->szName);

	while (this->m_PShopDB.Fetch() == SQL_SUCCESS)
	{
		pMsg.PShopItemValueInfo[count].nPShopItemInvenNum = this->m_PShopDB.GetAsInteger("ItemInvenNum");
		pMsg.PShopItemValueInfo[count].ItemSerial = this->m_PShopDB.GetAsInteger64("ItemSerial");
		pMsg.PShopItemValueInfo[count].nMoney = this->m_PShopDB.GetAsInteger("Money");
		pMsg.PShopItemValueInfo[count].sBlessJewelValue = this->m_PShopDB.GetAsInteger("BlessJewelValue");
		pMsg.PShopItemValueInfo[count].sSoulJewelValue = this->m_PShopDB.GetAsInteger("SoulJewelValue");
		pMsg.PShopItemValueInfo[count].sChaosJewelValue = this->m_PShopDB.GetAsInteger("ChaosJewelValue");

		count++;
	}

	this->m_PShopDB.Close();

	pMsg.btItemCnt = count;
	pMsg.iUserIndex = aRecv->iUserIndex;

	//DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GDAllSavePShopItemValue(int aIndex, PMSG_UPDATE_PSHOPITEMVALUE_INFO * aRecv)
{
	for (int i = 0; i < aRecv->btItemCnt; i++)
	{
		this->m_PShopDB.ExecQuery("EXEC IGC_PShopItemValueInfoSave '%s', '%s', %d, %I64d, %d, %d, %d, %d",
			aRecv->AccountId, aRecv->szName, aRecv->PShopItemValueInfo[i].nPShopItemInvenNum, aRecv->PShopItemValueInfo[i].ItemSerial,
			aRecv->PShopItemValueInfo[i].nMoney, aRecv->PShopItemValueInfo[i].sBlessJewelValue, aRecv->PShopItemValueInfo[i].sSoulJewelValue,
			aRecv->PShopItemValueInfo[i].sChaosJewelValue);

		this->m_PShopDB.Fetch();
		this->m_PShopDB.Close();
	}
}

void CDataServerProtocol::GDDelPShopItemValue(int aIndex, PMSG_DEL_PSHOPITEM * aRecv)
{
	this->m_PShopDB.ExecQuery("EXEC IGC_PShopItemValueInfoDel '%s', '%s', %d", aRecv->AccountId, aRecv->szName, aRecv->nPShopItemInvenNum);
	this->m_PShopDB.Fetch();
	this->m_PShopDB.Close();
}

void CDataServerProtocol::GDMovePShopItem(int aIndex, PMSG_MOVE_PSHOPITEM * aRecv)
{
	this->m_PShopDB.ExecQuery("EXEC IGC_PShopItemMove '%s', '%s', %d, %d", aRecv->AccountId, aRecv->szName, aRecv->nOldPShopItemInvenNum, aRecv->nNewPShopItemInvenNum);
	this->m_PShopDB.Fetch();
	this->m_PShopDB.Close();
}

void CDataServerProtocol::GDReqAcheronGuardianProcMultiCast(int aIndex, PMSG_REQ_AE_PLAY_DS * aRecv)
{
	PMSG_ANS_AE_PLAY_DS pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0xF1, sizeof(pMsg));

	pMsg.btPlay = aRecv->btPlay;

	for (int i = 0; i<g_dwMaxServerGroups; i++)
	{
		if (g_Server[i].m_Index != -1
			&& g_Server[i].m_Type == g_Server[aIndex].m_Type
			&& g_Server[i].m_ServerGroup == aRecv->wMapSvrNum)
		{
			//DataSend(i, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
		}
	}
}

void CDataServerProtocol::GDReqClassDefData(int aIndex)
{
	PMSG_ANS_CLASSDEF pMsg;
	PHeadSetW((LPBYTE)&pMsg, 0x02, sizeof(pMsg));

	QueryResult* res = this->m_GSDB.Fetch("SELECT * FROM DefaultClassType");
	int Count = 0;

	memset(pMsg.m_ClassData, -1, sizeof(pMsg.m_ClassData));

	while (this->m_GSDB.Fetch() != SQL_NO_DATA)
	{
		pMsg.m_ClassData[Count].Class = this->m_GSDB.GetAsInteger("Class") >> 4;
		pMsg.m_ClassData[Count].Str = this->m_GSDB.GetAsInteger("Strength");
		pMsg.m_ClassData[Count].Dex = this->m_GSDB.GetAsInteger("Dexterity");
		pMsg.m_ClassData[Count].Vit = this->m_GSDB.GetAsInteger("Vitality");
		pMsg.m_ClassData[Count].Energy = this->m_GSDB.GetAsInteger("Energy");
		pMsg.m_ClassData[Count].Life = this->m_GSDB.GetAsFloat("Life");
		pMsg.m_ClassData[Count].Mana = this->m_GSDB.GetAsFloat("Mana");
		pMsg.m_ClassData[Count].LevelLife = this->m_GSDB.GetAsFloat("LevelLife");
		pMsg.m_ClassData[Count].LevelMana = this->m_GSDB.GetAsFloat("LevelMana");
		pMsg.m_ClassData[Count].VitalityToLife = this->m_GSDB.GetAsFloat("VitalityToLife");
		pMsg.m_ClassData[Count].EnergyToMana = this->m_GSDB.GetAsFloat("EnergyToMana");
		pMsg.m_ClassData[Count].Leadership = this->m_GSDB.GetAsInteger("Leadership");
		Count++;
	}

	pMsg.iCount = Count;

	this->m_GSDB.Close();

	//DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GDReqReBuyItemList(int aIndex, SDHP_REQ_SHOP_REBUY_LIST * aRecv)
{
	this->m_ReBuyDB.ExecQuery("EXEC IGC_CancelItemSale_ClearExpiredItems '%s', '%s', %I64d", aRecv->szAccountID, aRecv->szName, aRecv->CurrTime);
	this->m_ReBuyDB.Close();

	BYTE BUFFER[2048];
	SDHP_ANS_SHOP_REBUY_LIST * lpMsg = (SDHP_ANS_SHOP_REBUY_LIST *)BUFFER;
	SDHP_SHOP_REBUY_ITEM * lpItem = (SDHP_SHOP_REBUY_ITEM *)BUFFER + sizeof(SDHP_ANS_SHOP_REBUY_LIST);
	INT64 nLen = 0;

	lpMsg->iCount = 0;
	lpMsg->btResult = 1;

	if (this->m_ReBuyDB.ExecQuery("EXEC IGC_CancelItemSale_GetItemList '%s', '%s'", aRecv->szAccountID, aRecv->szName) == TRUE)
	{
		lpMsg->btResult = 0;
	}

	while (this->m_ReBuyDB.Fetch() == SQL_SUCCESS)
	{
		lpItem = (SDHP_SHOP_REBUY_ITEM *)(BUFFER + sizeof(SDHP_ANS_SHOP_REBUY_LIST) + (lpMsg->iCount * sizeof(SDHP_SHOP_REBUY_ITEM)));

		lpItem->btItemNumber = this->m_ReBuyDB.GetAsInteger("ItemNumber");
		lpItem->wItemCount = this->m_ReBuyDB.GetAsInteger("ItemCount");
		lpItem->SellDate = this->m_ReBuyDB.GetAsInteger64("SellDate");
		lpItem->SellExpireDate = this->m_ReBuyDB.GetAsInteger64("SellExpireDate");
		lpItem->dwSellPrice = this->m_ReBuyDB.GetAsInteger("SellPrice");
		lpMsg->iCount++;
	}

	this->m_ReBuyDB.Close();
	char szTemp[256];

	for (int i = 0; i < lpMsg->iCount; i++)
	{
		lpItem = (SDHP_SHOP_REBUY_ITEM *)(BUFFER + sizeof(SDHP_ANS_SHOP_REBUY_LIST) + (i * sizeof(SDHP_SHOP_REBUY_ITEM)));

		sprintf(szTemp, "SELECT ItemData FROM IGC_CancelItemSale WHERE AccountID = '%s' AND Name = '%s' AND ItemNumber = %d AND SellPrice = %d",
			aRecv->szAccountID, aRecv->szName, lpItem->btItemNumber, lpItem->dwSellPrice);

		this->m_ReBuyDB.GetAsBinary(szTemp, lpItem->ItemData, 32);
	}

	this->m_ReBuyDB.Close();

	int PacketSize = sizeof(SDHP_ANS_SHOP_REBUY_LIST) + (lpMsg->iCount * sizeof(SDHP_SHOP_REBUY_ITEM));

	lpMsg->h.c = 0xC2;
	lpMsg->h.sizeH = HIBYTE(PacketSize);
	lpMsg->h.sizeL = LOBYTE(PacketSize);
	lpMsg->h.headcode = 0x6F;
	lpMsg->h.subcode = 0x00;
	lpMsg->iIndex = aRecv->iIndex;
	memcpy(lpMsg->szAccountID, aRecv->szAccountID, MAX_ACCOUNT_LEN + 1);
	memcpy(lpMsg->szName, aRecv->szName, MAX_ACCOUNT_LEN + 1);

	//DataSend(aIndex, BUFFER, PacketSize, __FUNCTION__);
}

void CDataServerProtocol::GDReqReBuyAddItem(int aIndex, SDHP_REQ_SHOP_REBUY_ADD_ITEM * aRecv)
{
	this->m_ReBuyDB.ExecQuery("EXEC IGC_CancelItemSale_ClearExpiredItems '%s', '%s', %I64d", aRecv->szAccountID, aRecv->szName, aRecv->SellDate);
	this->m_ReBuyDB.Close();

	char szTemp[256];
	sprintf(szTemp, "EXEC IGC_CancelItemSale_AddItem '%s', '%s', %I64d, %I64d, %d, ?", aRecv->szAccountID, aRecv->szName, aRecv->SellDate, aRecv->SellExpireDate, aRecv->dwSellPrice);

	int iRet = SQLPrepare(this->m_ReBuyDB.m_hStmt, (SQLTCHAR *)szTemp, SQL_NTS);

	if (iRet)
	{
		bool bReconnect = false;
		this->m_ReBuyDB.Diagnosis(bReconnect);
		this->m_ReBuyDB.Close();
		return;
	}

	int size = 32;
	INT64 cbBlob;

	SQLBindParameter(this->m_ReBuyDB.m_hStmt, 1, SQL_PARAM_INPUT, -2, -3, size, 0, (SQLPOINTER)1, 0, &cbBlob);

	cbBlob = -size - 100LL;
	short retcode = SQLExecute(this->m_ReBuyDB.m_hStmt);

	void *pToken;

	if (retcode == SQL_NULL_DATA)
	{
		bool bReconnect = false;

		this->m_ReBuyDB.Diagnosis(bReconnect);
		this->m_ReBuyDB.Close();
		return;
	}

	retcode = SQLParamData(this->m_ReBuyDB.m_hStmt, &pToken);

	if (retcode == SQL_NEED_DATA)
	{
		SQLPutData(this->m_ReBuyDB.m_hStmt, aRecv->ItemData, size);
	}

	retcode = SQLParamData(this->m_ReBuyDB.m_hStmt, &pToken);

	this->m_ReBuyDB.Close();
	memset(szTemp, 0, 256);
}

void CDataServerProtocol::GDReqReBuyGetItem(int aIndex, SDHP_REQ_SHOP_REBUY_GET_ITEM * aRecv)
{
	SDHP_ANS_SHOP_REBUY_GET_ITEM pMsg;
	char szTemp[256];
	INT64 nLen = 0;

	sprintf(szTemp, "EXEC IGC_CancelItemSale_GetItemToReBuy '%s', '%s', %d, %d", aRecv->szAccountID, aRecv->szName, aRecv->btItemNumber, aRecv->dwItemPrice);

	if (this->m_ReBuyDB.ExecQuery(szTemp) == FALSE)
	{
		pMsg.btResult = 1;
	}

	else if (this->m_ReBuyDB.Fetch() != SQL_SUCCESS)
	{
		pMsg.btResult = 2;
	}

	else
	{
		pMsg.btResult = 0;
		pMsg.btItemNumber = this->m_ReBuyDB.GetAsInteger("ItemNumber");
		pMsg.wItemCount = this->m_ReBuyDB.GetAsInteger("ItemCount");
		pMsg.dwSellPrice = this->m_ReBuyDB.GetAsInteger("SellPrice");
	}

	this->m_ReBuyDB.Close();

	if (pMsg.btResult == 0)
	{
		sprintf(szTemp, "SELECT ItemData FROM IGC_CancelItemSale WHERE AccountID = '%s' AND Name = '%s' AND ItemNumber = %d AND SellPrice = %d",
			aRecv->szAccountID, aRecv->szName, pMsg.btItemNumber, pMsg.dwSellPrice);

		this->m_ReBuyDB.GetAsBinary(szTemp, pMsg.ItemData, sizeof(pMsg.ItemData));
	}

	PHeadSubSetB((LPBYTE)&pMsg, 0x6F, 0x02, sizeof(pMsg));
	pMsg.iIndex = aRecv->iIndex;
	memcpy(pMsg.szAccountID, aRecv->szAccountID, MAX_ACCOUNT_LEN + 1);
	memcpy(pMsg.szName, aRecv->szName, MAX_ACCOUNT_LEN + 1);
	//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqDeleteSoldItem(int aIndex, SDHP_REQ_SHOP_REBUY_DELETE_ITEM * aRecv)
{
	this->m_ReBuyDB.ExecQuery("EXEC IGC_CancelItemSale_DeleteItem '%s', '%s', %d, %d", aRecv->szAccountID, aRecv->szName, aRecv->btItemNumber, aRecv->dwItemPrice);
	this->m_ReBuyDB.Close();
}

void CDataServerProtocol::GDReqGremoryCaseItemList(int aIndex, _stReqGremoryCaseItemList * aRecv)
{
	BYTE BUFFER[8192];

	_stAnsGremoryCaseItemList * lpMsg = (_stAnsGremoryCaseItemList *)(BUFFER);
	_stGremoryCaseItem * lpItem = (_stGremoryCaseItem *)(BUFFER + sizeof(_stAnsGremoryCaseItemList));

	if (this->m_GremoryCaseDB.ExecQuery("EXEC IGC_GremoryCase_GetItemList '%s', '%s'", aRecv->szAccountID, aRecv->szName) == TRUE)
	{
		lpMsg->btResult = 0;
	}

	else
	{
		lpMsg->btCount = 0;
		lpMsg->btResult = 1;
	}

	if (lpMsg->btResult == 0)
	{
		lpMsg->btCount = 0;

		while (this->m_GremoryCaseDB.Fetch() == SQL_SUCCESS)
		{
			lpItem = (_stGremoryCaseItem *)(BUFFER + sizeof(_stAnsGremoryCaseItemList) + (lpMsg->btCount * sizeof(_stGremoryCaseItem)));

			lpItem->btStorageType = this->m_GremoryCaseDB.GetAsInteger("StorageType");
			lpItem->btRewardSource = this->m_GremoryCaseDB.GetAsInteger("RewardSource");
			lpItem->btItemGUID = this->m_GremoryCaseDB.GetAsInteger("ItemGUID");
			lpItem->dwAuthCode = this->m_GremoryCaseDB.GetAsInteger("AuthCode");
			lpItem->wItemID = this->m_GremoryCaseDB.GetAsInteger("ItemID");
			lpItem->btItemLevel = this->m_GremoryCaseDB.GetAsInteger("ItemLevel");
			lpItem->btItemDurability = this->m_GremoryCaseDB.GetAsInteger("ItemDurability");
			lpItem->btItemSkill = this->m_GremoryCaseDB.GetAsInteger("ItemOp1");
			lpItem->btItemLuck = this->m_GremoryCaseDB.GetAsInteger("ItemOp2");
			lpItem->btItemOption = this->m_GremoryCaseDB.GetAsInteger("ItemOp3");
			lpItem->btItemExcOption = this->m_GremoryCaseDB.GetAsInteger("ItemExcOption");
			lpItem->btItemSetOption = this->m_GremoryCaseDB.GetAsInteger("ItemSetOption");
			lpItem->btItemSocketOption[0] = this->m_GremoryCaseDB.GetAsInteger("ItemSocket1");
			lpItem->btItemSocketOption[1] = this->m_GremoryCaseDB.GetAsInteger("ItemSocket2");
			lpItem->btItemSocketOption[2] = this->m_GremoryCaseDB.GetAsInteger("ItemSocket3");
			lpItem->btItemSocketOption[3] = this->m_GremoryCaseDB.GetAsInteger("ItemSocket4");
			lpItem->btItemSocketOption[4] = this->m_GremoryCaseDB.GetAsInteger("ItemSocket5");
			lpItem->btItemMainAttribute = this->m_GremoryCaseDB.GetAsInteger("ItemMainAttribute");
			lpItem->wMuunEvoItemType = this->m_GremoryCaseDB.GetAsInteger("ItemMuunEvoItemType");
			lpItem->btMuunEvoItemIndex = this->m_GremoryCaseDB.GetAsInteger("ItemMuunEvoItemIndex");
			lpItem->iReceiveDate = this->m_GremoryCaseDB.GetAsInteger("ReceiveDate");
			lpItem->iExpireDate = this->m_GremoryCaseDB.GetAsInteger("ExpireDate");

			lpMsg->btCount++;
		}
	}

	this->m_GremoryCaseDB.Close();
	int PacketSize = sizeof(_stAnsGremoryCaseItemList) + (lpMsg->btCount * sizeof(_stGremoryCaseItem));

	lpMsg->h.c = 0xC2;
	lpMsg->h.sizeH = HIBYTE(PacketSize);
	lpMsg->h.sizeL = LOBYTE(PacketSize);
	lpMsg->h.headcode = 0x4F;
	lpMsg->h.subcode = 0x00;
	lpMsg->iIndex = aRecv->iIndex;
	memcpy(lpMsg->szAccountID, aRecv->szAccountID, MAX_ACCOUNT_LEN + 1);
	memcpy(lpMsg->szName, aRecv->szName, MAX_ACCOUNT_LEN + 1);

	//DataSend(aIndex, BUFFER, PacketSize, __FUNCTION__);
}

void CDataServerProtocol::GDReqGremoryCaseAddItem(int aIndex, _stReqAddItemToGremoryCase * aRecv)
{
	_stAnsAddItemToGremoryCase pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0x4F, 0x01, sizeof(pMsg));

	if (this->m_GremoryCaseDB.ExecQuery("EXEC IGC_GremoryCase_AddItem '%s', '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %I64d, %I64d",
		aRecv->szAccountID, aRecv->szName, aRecv->m_GremoryCaseItem.btStorageType, aRecv->m_GremoryCaseItem.btRewardSource,
		aRecv->m_GremoryCaseItem.wItemID, aRecv->m_GremoryCaseItem.btItemLevel, aRecv->m_GremoryCaseItem.btItemDurability,
		aRecv->m_GremoryCaseItem.btItemSkill, aRecv->m_GremoryCaseItem.btItemLuck, aRecv->m_GremoryCaseItem.btItemOption,
		aRecv->m_GremoryCaseItem.btItemExcOption, aRecv->m_GremoryCaseItem.btItemSetOption, aRecv->m_GremoryCaseItem.btItemSocketOption[0],
		aRecv->m_GremoryCaseItem.btItemSocketOption[1], aRecv->m_GremoryCaseItem.btItemSocketOption[2], aRecv->m_GremoryCaseItem.btItemSocketOption[3],
		aRecv->m_GremoryCaseItem.btItemSocketOption[4],
		aRecv->m_GremoryCaseItem.btItemMainAttribute, aRecv->m_GremoryCaseItem.wMuunEvoItemType, aRecv->m_GremoryCaseItem.btMuunEvoItemIndex,
		aRecv->m_GremoryCaseItem.iReceiveDate, aRecv->m_GremoryCaseItem.iExpireDate) == FALSE)
	{
		pMsg.btResult = 1;
	}

	else
	{
		this->m_GremoryCaseDB.Fetch();
		pMsg.m_GremoryCaseItem.btStorageType = aRecv->m_GremoryCaseItem.btStorageType;
		pMsg.m_GremoryCaseItem.btRewardSource = aRecv->m_GremoryCaseItem.btRewardSource;
		pMsg.m_GremoryCaseItem.wItemID = aRecv->m_GremoryCaseItem.wItemID;
		pMsg.m_GremoryCaseItem.btItemLevel = aRecv->m_GremoryCaseItem.btItemLevel;
		pMsg.m_GremoryCaseItem.btItemDurability = aRecv->m_GremoryCaseItem.btItemDurability;
		pMsg.m_GremoryCaseItem.btItemSkill = aRecv->m_GremoryCaseItem.btItemSkill;
		pMsg.m_GremoryCaseItem.btItemLuck = aRecv->m_GremoryCaseItem.btItemLuck;
		pMsg.m_GremoryCaseItem.btItemOption = aRecv->m_GremoryCaseItem.btItemOption;
		pMsg.m_GremoryCaseItem.btItemExcOption = aRecv->m_GremoryCaseItem.btItemExcOption;
		pMsg.m_GremoryCaseItem.btItemSetOption = aRecv->m_GremoryCaseItem.btItemSetOption;
		pMsg.m_GremoryCaseItem.btItemSocketOption[0] = aRecv->m_GremoryCaseItem.btItemSocketOption[0];
		pMsg.m_GremoryCaseItem.btItemSocketOption[1] = aRecv->m_GremoryCaseItem.btItemSocketOption[1];
		pMsg.m_GremoryCaseItem.btItemSocketOption[2] = aRecv->m_GremoryCaseItem.btItemSocketOption[2];
		pMsg.m_GremoryCaseItem.btItemSocketOption[3] = aRecv->m_GremoryCaseItem.btItemSocketOption[3];
		pMsg.m_GremoryCaseItem.btItemSocketOption[4] = aRecv->m_GremoryCaseItem.btItemSocketOption[4];
		pMsg.m_GremoryCaseItem.btItemMainAttribute = aRecv->m_GremoryCaseItem.btItemMainAttribute;
		pMsg.m_GremoryCaseItem.wMuunEvoItemType = aRecv->m_GremoryCaseItem.wMuunEvoItemType;
		pMsg.m_GremoryCaseItem.btMuunEvoItemIndex = aRecv->m_GremoryCaseItem.btMuunEvoItemIndex;
		pMsg.m_GremoryCaseItem.iReceiveDate = aRecv->m_GremoryCaseItem.iReceiveDate;
		pMsg.m_GremoryCaseItem.iExpireDate = aRecv->m_GremoryCaseItem.iExpireDate;
		pMsg.m_GremoryCaseItem.btItemGUID = this->m_GremoryCaseDB.GetAsInteger("ItemGUID");
		pMsg.m_GremoryCaseItem.dwAuthCode = this->m_GremoryCaseDB.GetAsInteger("AuthCode");
		pMsg.btResult = 0;
	}

	this->m_GremoryCaseDB.Close();

	pMsg.iIndex = aRecv->iIndex;
	memcpy(pMsg.szAccountID, aRecv->szAccountID, MAX_ACCOUNT_LEN + 1);
	memcpy(pMsg.szName, aRecv->szName, MAX_ACCOUNT_LEN + 1);

	//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqCheckUseItemGremoryCase(int aIndex, _stReqCheckUseItemGremoryCase * aRecv)
{
	_stAnsCheckUseItemGremoryCase pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0x4F, 0x02, sizeof(pMsg));

	this->m_GremoryCaseDB.ExecQuery("EXEC IGC_GremoryCase_CheckUseItem %d, %d, %d", aRecv->wItemID, aRecv->dwItemGUID, aRecv->dwAuthCode);
	this->m_GremoryCaseDB.Fetch();

	pMsg.btResult = this->m_GremoryCaseDB.GetAsInteger("ResultCode");
	this->m_GremoryCaseDB.Close();

	pMsg.iIndex = aRecv->iIndex;
	memcpy(pMsg.szAccountID, aRecv->szAccountID, MAX_ACCOUNT_LEN + 1);
	memcpy(pMsg.szName, aRecv->szName, MAX_ACCOUNT_LEN + 1);
	pMsg.wItemID = aRecv->wItemID;
	pMsg.dwItemGUID = aRecv->dwItemGUID;
	pMsg.dwAuthCode = aRecv->dwAuthCode;

	//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqGremoryCaseDeleteItem(int aIndex, _stReqDeleteItemFromGremoryCase * aRecv)
{
	this->m_GremoryCaseDB.ExecQuery("EXEC IGC_GremoryCase_DeleteItem %d, %d, %d", aRecv->wItemID, aRecv->dwItemGUID, aRecv->dwAuthCode);
	this->m_GremoryCaseDB.Fetch();
}

void CDataServerProtocol::GDReqUBFCheckIsJoinedUser(int aIndex, PMSG_REQ_UBF_ACCOUNT_USERINFO * aRecv)
{
	PMSG_ANS_UBF_ACCOUNT_USERINFO pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x01, sizeof(pMsg));

	this->m_BattleCoreDB.ExecQuery("EXEC IGC_BattleCore_GetUserInfo '%s', '%s', %d, %d", aRecv->szAccountID, aRecv->szName, aRecv->iServerCode, aRecv->IsUnityBattleFieldServer);
	this->m_BattleCoreDB.Fetch();

	pMsg.btResult = this->m_BattleCoreDB.GetAsInteger("Result");

	if (pMsg.btResult == 0)
	{
		pMsg.btRegisterState = 0;
		pMsg.btRegisterMonth = 0;
		pMsg.btRegisterDay = 0;
	}

	else
	{
		pMsg.btRegisterState = this->m_BattleCoreDB.GetAsInteger("RegisterState");
		pMsg.btRegisterMonth = this->m_BattleCoreDB.GetAsInteger("RegisterMonth");
		pMsg.btRegisterDay = this->m_BattleCoreDB.GetAsInteger("RegisterDay");
	}

	this->m_BattleCoreDB.Close();

	pMsg.btObserverMode = aRecv->btObserverMode;
	pMsg.iUserIndex = aRecv->iUserIndex;

	//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqUBFJoinUser(int aIndex, PMSG_UBF_REGISTER_ACCOUNT_USER * aRecv)
{
	PMSG_UBF_REGISTER_ACCOUNT_USER_RESULT pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x02, sizeof(pMsg));

	this->m_BattleCoreDB.ExecQuery("EXEC IGC_BattleCore_JoinUser '%s', '%s', '%s', %d, %d, %d, %d",
		aRecv->szAccountID, aRecv->szName, aRecv->szBattleFieldName, aRecv->ServerCode,
		aRecv->btRegisterState, aRecv->btRegisterMonth, aRecv->btRegisterDay);

	this->m_BattleCoreDB.Fetch();

	pMsg.btResult = this->m_BattleCoreDB.GetAsInteger("Result");

	if (pMsg.btResult == 3)
	{
		pMsg.nLeftSec = this->m_BattleCoreDB.GetAsInteger("LeftSecond");
	}

	else
	{
		pMsg.nLeftSec = 0;
	}

	this->m_BattleCoreDB.Close();

	pMsg.iUserIndex = aRecv->iUserIndex;

	//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqUBFCopyCharacter(int aIndex, PMSG_UBF_ACCOUNT_USER_COPY * aRecv)
{
	PMSG_UBF_ACCOUNT_USER_COPY_RESULT pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x03, sizeof(pMsg));

	if (aRecv->btPromotionCode == 0)
	{
		this->m_BattleCoreDB.ExecQuery("EXEC IGC_BattleCore_CopyCharacter_Normal '%s', '%s', %d",
			aRecv->szAccountID, aRecv->szName, aRecv->ServerCode);
	}

	else
	{
		this->m_BattleCoreDB.ExecQuery("EXEC IGC_BattleCore_CopyCharacter_Promotion '%s', '%s', %d",
			aRecv->szAccountID, aRecv->szName, aRecv->ServerCode);
	}

	this->m_BattleCoreDB.Fetch();
	pMsg.btResult = this->m_BattleCoreDB.GetAsInteger("Result");
	pMsg.btSubResult = 0;

	this->m_BattleCoreDB.Close();

	pMsg.iUserIndex = aRecv->iUserIndex;
	//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqUBFCancelUser(int aIndex, PMSG_UBF_REQ_CANCEL_REGISTER_USER * aRecv)
{
	PMSG_UBF_ANS_CANCEL_REGISTER_USER pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x07, sizeof(pMsg));

	this->m_BattleCoreDB.ExecQuery("EXEC IGC_BattleCore_CancelUser '%s', '%s', %d", aRecv->szAccountID, aRecv->szName, aRecv->ServerCode);
	this->m_BattleCoreDB.Fetch();

	pMsg.btCanceledResult = this->m_BattleCoreDB.GetAsInteger("Result");

	this->m_BattleCoreDB.Close();

	if (pMsg.btCanceledResult == 1)
	{
		this->m_BattleCoreDB.ExecQuery("EXEC IGC_BattleCore_DeleteCharacter '%s', '%s', %d", aRecv->szAccountID, aRecv->szName, aRecv->ServerCode);
		this->m_BattleCoreDB.Fetch();

		pMsg.btDeletedResult = this->m_BattleCoreDB.GetAsInteger("Result");

		this->m_BattleCoreDB.Close();
	}

	else
	{
		pMsg.btDeletedResult = pMsg.btCanceledResult;
	}

	if (pMsg.btCanceledResult == TRUE && pMsg.btDeletedResult == TRUE)
	{
		QueryResult* res = this->m_BattleCoreDB.Fetch("SELECT * FROM [BattleCore].[dbo].AccountCharacter WHERE Id='%s'", aRecv->szAccountID);
		this->m_BattleCoreDB.Fetch();

		TCHAR CharName[5][11] = { 0 };
		memset(CharName, 0x00, sizeof(CharName));
		this->m_BattleCoreDB.GetAsString("GameID1", CharName[0], sizeof(CharName[0]) - 1);
		this->m_BattleCoreDB.GetAsString("GameID2", CharName[1], sizeof(CharName[0]) - 1);
		this->m_BattleCoreDB.GetAsString("GameID3", CharName[2], sizeof(CharName[0]) - 1);
		this->m_BattleCoreDB.GetAsString("GameID4", CharName[3], sizeof(CharName[0]) - 1);
		this->m_BattleCoreDB.GetAsString("GameID5", CharName[4], sizeof(CharName[0]) - 1);

		int iIndex;
		for (iIndex = 0; iIndex<5; iIndex++)
		{
			if (lstrcmp(CharName[iIndex], aRecv->szName) == 0)
			{
				break;
			}
		}

		this->m_BattleCoreDB.Close();

		if (iIndex != 5)
		{
			this->m_BattleCoreDB.ExecQuery("UPDATE [BattleCore].[dbo].AccountCharacter SET GameID%d=NULL WHERE Id='%s'", iIndex + 1, aRecv->szAccountID);
			this->m_BattleCoreDB.Close();
		}
	}

	pMsg.iUserIndex = aRecv->iUserIndex;
	//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqUBFGetRealName(int aIndex, PMSG_REQ_GET_UBF_REAL_NAME * aRecv)
{
	PMSG_ANS_GET_UBF_REAL_NAME pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x08, sizeof(pMsg));

	this->m_BattleCoreDB.ExecQuery("EXEC IGC_BattleCore_GetRealName '%s'", aRecv->szUBFName);
	this->m_BattleCoreDB.Fetch();

	this->m_BattleCoreDB.GetAsString("RealName", pMsg.szRealName, MAX_ACCOUNT_LEN + 1);
	pMsg.iServerCode = this->m_BattleCoreDB.GetAsInteger("ServerCode");

	this->m_BattleCoreDB.Close();

	strcpy(pMsg.szUBFName, aRecv->szUBFName);
	pMsg.btReturn = 0;
	pMsg.iUserIndex = aRecv->iUserIndex;

	//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqUBFCopyPetItem(int aIndex, LPBYTE lpRecv)
{
	PMSG_UBF_ACCOUNT_USER_COPY_PETITEM * lpMsg = (PMSG_UBF_ACCOUNT_USER_COPY_PETITEM *)lpRecv;
	PMSG_UBF_COPY_PETITEM * lpItem = (PMSG_UBF_COPY_PETITEM *)lpRecv + sizeof(PMSG_UBF_ACCOUNT_USER_COPY_PETITEM);

	for (int i = 0; i < lpMsg->btCount; i++)
	{
		lpItem = (PMSG_UBF_COPY_PETITEM *)(lpRecv + sizeof(PMSG_UBF_ACCOUNT_USER_COPY_PETITEM) + (i * sizeof(PMSG_UBF_COPY_PETITEM)));

		this->m_BattleCoreDB.ExecQuery("EXEC IGC_BattleCore_CopyPetItemInfo %I64d, %d", lpItem->i64ItemSerial, lpMsg->ServerCode);
		this->m_BattleCoreDB.Fetch();
		this->m_BattleCoreDB.Close();
	}
}

void CDataServerProtocol::GDReqUBFSetCCFReward(int aIndex, SDHP_REQ_SET_CCF_WINNER_INFO * aRecv)
{
	this->m_BattleCoreDB.ExecQuery("EXEC IGC_BattleCore_SetCCFReward '%s', %d, %d", aRecv->UBFName, aRecv->btCCFType, aRecv->btRewardType);
	this->m_BattleCoreDB.Fetch();
	this->m_BattleCoreDB.Close();
}

void CDataServerProtocol::GDReqUBFSetCCBattleReward(int aIndex, SDHP_REQ_SET_CC_WINNER_INFO_UBF * aRecv)
{
	this->m_BattleCoreDB.ExecQuery("EXEC IGC_BattleCore_SetCCReward '%s', %d", aRecv->UBFName, aRecv->btRewardType);
	this->m_BattleCoreDB.Fetch();
	this->m_BattleCoreDB.Close();
}

void CDataServerProtocol::GDReqUBFSetDSFReward(int aIndex, PMSG_REQ_SET_DSF_WINNER_INFO * aRecv)
{
	this->m_BattleCoreDB.ExecQuery("EXEC IGC_BattleCore_SetDSFReward '%s', %d, %d", aRecv->UBFName, aRecv->btDSFType, aRecv->btRewardType);
	this->m_BattleCoreDB.Fetch();
	this->m_BattleCoreDB.Close();
}

void CDataServerProtocol::GDReqUBFGetReward(int aIndex, PMSG_REQ_UBF_GET_REWARD * aRecv)
{
	PMSG_ANS_UBF_GET_REWARD pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x06, sizeof(pMsg));

	if (aRecv->btBattleKind == 1) // CCF
	{
		pMsg.btStage1 = FALSE;
		pMsg.btStage2 = FALSE;
		pMsg.btStage3 = FALSE;

		pMsg.btRewardCount1[0] = 0;
		pMsg.btRewardCount1[1] = 0;
		pMsg.btRewardCount2[0] = 0;
		pMsg.btRewardCount2[1] = 0;
		pMsg.btRewardCount3[0] = 0;
		pMsg.btRewardCount3[1] = 0;

		this->m_BattleCoreDB.ExecQuery("EXEC IGC_BattleCore_GetCCFReward '%s', '%s', %d", aRecv->szAccountID, aRecv->szName, aRecv->iServerCode);

		while (this->m_BattleCoreDB.Fetch() == SQL_SUCCESS)
		{
			int iResult = this->m_BattleCoreDB.GetAsInteger("Result");

			if (iResult != 1)
			{
				pMsg.btReturn = 0;
				break;
			}

			int iGainReward = this->m_BattleCoreDB.GetAsInteger("RewardGain");
			int iRewardType = this->m_BattleCoreDB.GetAsInteger("RewardType");
			int iEventStage = this->m_BattleCoreDB.GetAsInteger("Stage");

			if (iGainReward == TRUE)
			{
				continue;
			}

			if (iRewardType != 0 && iRewardType != 1)
			{
				continue;
			}

			if (iEventStage == 0)
			{
				continue;
			}

			if (iEventStage == 1)
			{
				pMsg.btStage1 = TRUE;
				pMsg.btRewardCount1[iRewardType]++;
			}

			else if (iEventStage == 2)
			{
				pMsg.btStage2 = TRUE;
				pMsg.btRewardCount2[iRewardType]++;
			}

			else if (iEventStage == 3)
			{
				pMsg.btStage3 = TRUE;
				pMsg.btRewardCount3[iRewardType]++;
			}
		}

		this->m_BattleCoreDB.Close();

		if (pMsg.btStage1 == FALSE && pMsg.btStage2 == FALSE && pMsg.btStage3 == FALSE)
		{
			pMsg.btReturn = 0;
		}

		else
		{
			pMsg.btReturn = 1;
		}
	}

	else if (aRecv->btBattleKind == 2) // Tormented Square
	{
		pMsg.btStage1 = FALSE;
		pMsg.btStage2 = FALSE;
		pMsg.btStage3 = FALSE;

		pMsg.btRewardCount1[0] = 0;
		pMsg.btRewardCount1[1] = 0;
		pMsg.btRewardCount2[0] = 0;
		pMsg.btRewardCount2[1] = 0;
		pMsg.btRewardCount3[0] = 0;
		pMsg.btRewardCount3[1] = 0;

		this->m_BattleCoreDB.ExecQuery("EXEC IGC_BattleCore_GetDSFReward '%s', '%s', %d", aRecv->szAccountID, aRecv->szName, aRecv->iServerCode);

		while (this->m_BattleCoreDB.Fetch() == SQL_SUCCESS)
		{
			int iResult = this->m_BattleCoreDB.GetAsInteger("Result");

			if (iResult != 1)
			{
				pMsg.btReturn = 0;
				break;
			}

			int iGainReward = this->m_BattleCoreDB.GetAsInteger("RewardGain");
			int iRewardType = this->m_BattleCoreDB.GetAsInteger("RewardType");
			int iEventStage = this->m_BattleCoreDB.GetAsInteger("Stage");

			if (iGainReward == TRUE)
			{
				continue;
			}

			if (iRewardType != 0 && iRewardType != 1)
			{
				continue;
			}

			if (iEventStage == 0)
			{
				continue;
			}

			if (iEventStage == 1)
			{
				pMsg.btStage1 = TRUE;
				pMsg.btRewardCount1[iRewardType]++;
			}

			else if (iEventStage == 2)
			{
				pMsg.btStage2 = TRUE;
				pMsg.btRewardCount2[iRewardType]++;
			}

			else if (iEventStage == 3)
			{
				pMsg.btStage3 = TRUE;
				pMsg.btRewardCount3[iRewardType]++;
			}
		}

		this->m_BattleCoreDB.Close();

		if (pMsg.btStage1 == FALSE && pMsg.btStage2 == FALSE && pMsg.btStage3 == FALSE)
		{
			pMsg.btReturn = 0;
		}

		else
		{
			pMsg.btReturn = 1;
		}
	}

	else if (aRecv->btBattleKind == 3) // CC Battle
	{
		pMsg.btStage1 = FALSE;

		pMsg.btRewardCount1[0] = 0;
		pMsg.btRewardCount1[1] = 0;

		this->m_BattleCoreDB.ExecQuery("EXEC IGC_BattleCore_GetCCReward '%s', '%s', %d", aRecv->szAccountID, aRecv->szName, aRecv->iServerCode);

		while (this->m_BattleCoreDB.Fetch() == SQL_SUCCESS)
		{
			int iResult = this->m_BattleCoreDB.GetAsInteger("Result");

			if (iResult != 1)
			{
				pMsg.btReturn = 0;
				break;
			}

			int iGainReward = this->m_BattleCoreDB.GetAsInteger("RewardGain");
			int iRewardType = this->m_BattleCoreDB.GetAsInteger("RewardType");

			if (iGainReward == TRUE)
			{
				continue;
			}

			if (iRewardType != 0 && iRewardType != 1)
			{
				continue;
			}

			pMsg.btStage1 = TRUE;
			pMsg.btRewardCount1[iRewardType]++;
		}

		this->m_BattleCoreDB.Close();

		if (pMsg.btStage1 == FALSE)
		{
			pMsg.btReturn = 0;
		}

		else
		{
			pMsg.btReturn = 1;
		}
	}

	pMsg.btBattleKind = aRecv->btBattleKind;
	pMsg.iUserIndex = aRecv->iUserIndex;

	//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqUBFSetGainReward(int aIndex, PMSG_REQ_UBF_SET_RECEIVED_REWARD * aRecv)
{
	PMSG_ANS_UBF_SET_RECEIVED_REWARD pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x05, sizeof(pMsg));

	pMsg.iUserIndex = aRecv->iUserIndex;

	if (aRecv->btBattleKind == 1) // CCF
	{
		this->m_BattleCoreDB.ExecQuery("EXEC IGC_BattleCore_SetCCFGainReward '%s', '%s', %d", aRecv->szAccountID, aRecv->szName, aRecv->iServerCode);
		this->m_BattleCoreDB.Fetch();

		pMsg.btReturn = this->m_BattleCoreDB.GetAsInteger("Result");

		this->m_BattleCoreDB.Close();
	}

	else if (aRecv->btBattleKind == 2) // Tormented Square
	{
		this->m_BattleCoreDB.ExecQuery("EXEC IGC_BattleCore_SetDSFGainReward '%s', '%s', %d", aRecv->szAccountID, aRecv->szName, aRecv->iServerCode);
		this->m_BattleCoreDB.Fetch();

		pMsg.btReturn = this->m_BattleCoreDB.GetAsInteger("Result");

		this->m_BattleCoreDB.Close();
	}

	else if (aRecv->btBattleKind == 3) // CC Battle
	{
		this->m_BattleCoreDB.ExecQuery("EXEC IGC_BattleCore_SetCCGainReward '%s', '%s', %d", aRecv->szAccountID, aRecv->szName, aRecv->iServerCode);
		this->m_BattleCoreDB.Fetch();

		pMsg.btReturn = this->m_BattleCoreDB.GetAsInteger("Result");

		this->m_BattleCoreDB.Close();
	}

	//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqDSFCanPartyEnter(int aIndex, PMSG_REQ_DSF_CAN_PARTY_ENTER * aRecv)
{
	PMSG_ANS_DSF_CAN_PARTY_ENTER pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xFD, 0x00, sizeof(pMsg));

	this->m_DSFinalDB.ExecQuery("EXEC IGC_DevilSquareFinal_CanPartyEnter '%s', '%s', '%s', '%s', %d", aRecv->szAccountID1, aRecv->szUserName1, aRecv->szAccountID2, aRecv->szUserName2, aRecv->btDSFType);
	this->m_DSFinalDB.Fetch();

	pMsg.btResult = this->m_DSFinalDB.GetAsInteger("Result");
	pMsg.btCount = this->m_DSFinalDB.GetAsInteger("EnterCount");

	this->m_DSFinalDB.Close();

	pMsg.iUserIndex = aRecv->iUserIndex;
	//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqDSFSavePartyPoint(int aIndex, PMSG_REQ_SAVE_DSF_PARTYPOINT * aRecv)
{
	this->m_DSFinalDB.ExecQuery("EXEC IGC_DevilSquareFinal_Save '%s', '%s', %d, '%s', '%s', %d, %d, %d, %d",
		aRecv->szAccountID1, aRecv->szUserName1, aRecv->nUserLevel1,
		aRecv->szAccountID2, aRecv->szUserName2, aRecv->nUserLevel2,
		aRecv->nPoint, aRecv->nDSFType, aRecv->btEnterCnt);

	this->m_DSFinalDB.Fetch();
	this->m_DSFinalDB.Close();
}

void CDataServerProtocol::GDReqDSFPartyRankRenew(int aIndex, PMSG_REQ_DSF_PARTYRANKRENEW * aRecv)
{
	this->m_DSFinalDB.ExecQuery("EXEC IGC_DevilSquareFinal_Renew %d", aRecv->btDSFType);
	this->m_DSFinalDB.Fetch();
	this->m_DSFinalDB.Close();

	PMSG_ANS_DSF_PARTYRANKRENEW pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xFD, 0x03, sizeof(pMsg));

	pMsg.btResult = 1;
	//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqDSFGoFinalParty(int aIndex, PMSG_REQ_DSF_GO_FINAL_PARTY * aRecv)
{
	PMSG_ANS_DSF_GO_FINAL_PARTY pMsg;
	DSF_GO_FINAL_PARTY pParty;

	BYTE sendBuf[4096];
	int iSize = sizeof(PMSG_ANS_DSF_GO_FINAL_PARTY);
	int iCount = 0;

	this->m_DSFinalDB.ExecQuery("EXEC IGC_DevilSquareFinal_GetRank_r %d", aRecv->btDSFType);

	while (this->m_DSFinalDB.Fetch() == SQL_SUCCESS)
	{
		pParty.btRank = this->m_DSFinalDB.GetAsInteger("mRank");
		this->m_DSFinalDB.GetAsString("mUserName1", pParty.szUserName1, sizeof(pParty.szUserName1));
		this->m_DSFinalDB.GetAsString("mUserName2", pParty.szUserName2, sizeof(pParty.szUserName2));
		pParty.wServerCode1 = this->m_DSFinalDB.GetAsInteger("mServerCode1");
		pParty.wServerCode2 = this->m_DSFinalDB.GetAsInteger("mServerCode2");
		pParty.iPoint = this->m_DSFinalDB.GetAsInteger("mPoint");
		pParty.btDSFType = aRecv->btDSFType;
		pParty.iEnterYear = this->m_DSFinalDB.GetAsInteger("mYear");
		pParty.btEnterMonth = this->m_DSFinalDB.GetAsInteger("mMonth");
		pParty.btEnterDay = this->m_DSFinalDB.GetAsInteger("mDay");

		memcpy(&sendBuf[iSize], &pParty, sizeof(pParty));
		iSize += sizeof(pParty);
		iCount++;
	}

	this->m_DSFinalDB.Close();

	PHeadSubSetW((LPBYTE)&pMsg, 0xFD, 0x05, iSize);
	pMsg.btDSFType = aRecv->btDSFType;
	pMsg.btPartyCnt = iCount;

	memcpy(&sendBuf, &pMsg, sizeof(pMsg));

	//DataSend(aIndex, sendBuf, iSize, __FUNCTION__);
}

void CDataServerProtocol::GDReqDSFInsertRewardUser(int aIndex, PMSG_REQ_SAVE_DSF_REWARD_USER * aRecv)
{
	this->m_DSFinalDB.ExecQuery("EXEC IGC_DevilSquareFinal_SetReward '%s', '%s', %d, %d, %d, %d, %d, %d",
		aRecv->szAccountID, aRecv->szUserName, aRecv->iClass, aRecv->btDSFType,
		aRecv->nRewardYear, aRecv->btRewardMonth, aRecv->btRewardStartDay, aRecv->btRewardEndDay);

	this->m_DSFinalDB.Fetch();
	this->m_DSFinalDB.Close();
}

void CDataServerProtocol::GDReqDSFGetReward(int aIndex, PMSG_REQ_GET_DSF_REWARD * aRecv)
{
	PMSG_ANS_GET_DSF_REWARD pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xFD, 0x07, sizeof(pMsg));

	this->m_DSFinalDB.ExecQuery("EXEC IGC_DevilSquareFinal_GetReward '%s', '%s', %d, %d, %d",
		aRecv->szAccountID, aRecv->szUserName, aRecv->nRewardYear, aRecv->btRewardMonth, aRecv->btRewardDay);

	this->m_DSFinalDB.Fetch();

	pMsg.btResult = this->m_DSFinalDB.GetAsInteger("Result");

	this->m_DSFinalDB.Close();

	pMsg.nUserIndex = aRecv->nUserIndex;
	//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

// JoinMu

void CDataServerProtocol::GDReqWishperOtherChannel(int aIndex, PMSG_RECV_CHATDATA_WHISPER * aRecv)
{
	PMSG_RECV_CHATDATA_WHISPER pMsg;
	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xC2;
	pMsg.h.size = sizeof(pMsg);
	memcpy(pMsg.fromId, aRecv->fromId, sizeof(aRecv->fromId));
	memcpy(pMsg.id, aRecv->id, sizeof(pMsg.id));
	memcpy(pMsg.chatmsg, aRecv->chatmsg, sizeof(pMsg.chatmsg));
	pMsg.wMapSvrNum = aRecv->wMapSvrNum;
	pMsg.OriginGSIndex = aIndex;
	pMsg.OriginPlayerIndex = aRecv->OriginPlayerIndex;
	for (int i = 0; i<g_dwMaxServerGroups; i++)
	{
		if (g_Server[i].m_Index != -1
			&& g_Server[i].m_Type == g_Server[aIndex].m_Type
			&& g_Server[i].m_ServerGroup == aRecv->wMapSvrNum)
		{
			//DataSend(i, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
		}
	}
}

void CDataServerProtocol::GDReqMapSrvGroupServerCount(int aIndex, DSMSG_REQ_SUBSERVER_COUNT * aRecv)
{
	WORD wServerCount = 0;
	DSMSG_ANS_SUBSERVER_COUNT pMsg;
	pMsg.h.set((LPBYTE)&pMsg, 0xC3, 0x01, sizeof(pMsg));

	for (int i = 0; i<g_dwMaxServerGroups; i++)
	{
		if (g_Server[i].m_Index != -1
			&& g_Server[i].m_Type == g_Server[aIndex].m_Type
			&& g_Server[i].m_ServerGroup == aRecv->wMapSvrGroup)
		{
			wServerCount++;
		}
	}

	pMsg.wSvrCount = wServerCount;

	//DataSend(aIndex, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);


}

void CDataServerProtocol::DGWhisperResponseRecv(int aIndex, DSMSG_ANS_WHISPER_RESULT * aRecv)
{
	DSMSG_GS_WHISPER_RESULT pMsg;
	pMsg.h.set((LPBYTE)&pMsg, 0xC3, 0x03, sizeof(pMsg));

	pMsg.btResult = aRecv->btResult;
	pMsg.OriginPlayerIndex = aRecv->OriginPlayerIndex;

	//DataSend(aRecv->OriginGSIndex, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::DGChaosMachineLogLostItems(int aIndex, DSMSG_CHAOSMACHINE_LOSTITEMS * aRecv)
{
	char szTemp[256];
	sprintf(szTemp, "INSERT INTO IGC_ChaosMachine_LostItems (AccountID, Name, LostDate, ?) VALUES ('%s', '%s', GETDATE())", aRecv->szAccountID, aRecv->szName);
}

void CDataServerProtocol::GDDisconnectOtherChannel(int aIndex, PMSG_RECV_DC_OTHER_CHANNEL * aRecv)
{
	PMSG_RECV_DC_OTHER_CHANNEL pMsg;
	pMsg.h.set((LPBYTE)&pMsg, 0xC3, 0x04, sizeof(pMsg));

	memcpy(pMsg.szName, aRecv->szName, sizeof(pMsg.szName));
	pMsg.wMapSrvGroup = aRecv->wMapSrvGroup;
	for (int i = 0; i<g_dwMaxServerGroups; i++)
	{
		if (g_Server[i].m_Index != -1
			&& g_Server[i].m_Type == g_Server[aIndex].m_Type
			&& g_Server[i].m_ServerGroup == aRecv->wMapSrvGroup)
		{
			//DataSend(i, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
		}
	}

}

void CDataServerProtocol::GDReqEventEntryCount(int aIndex, PMSG_REQ_EVENTENTRY_INFO * aRecv)
{
	PMSG_ANS_EVENTENTRY_INFO pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xEE, 0x00, sizeof(pMsg));

	pMsg.nUserIndex = aRecv->nUserIndex;
	memset(pMsg.btEventEntryCount, 0x00, sizeof(pMsg.btEventEntryCount));

	if (g_DSBattleCoreEnable == 0)
	{
		for (int i = 0; i < 6; i++)
		{
			QueryResult* res = this->m_CharMiscDB.Fetch("SELECT COUNT(*) AS EntryCount FROM IGC_EventEntryCount WHERE Name = '%s' AND EventEntryType = %d AND DATEDIFF(DAY, EventEntryDate, GETDATE()) <= 0", aRecv->szName, i);
			this->m_CharMiscDB.Fetch();

			pMsg.btEventEntryCount[i] = this->m_CharMiscDB.GetAsInteger("EntryCount");
			this->m_CharMiscDB.Close();
		}
	}


	//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqEvoMonMaxScore(int aIndex, PMSG_REQ_EVOMON_MAXSCORE * aRecv)
{
	PMSG_ANS_EVOMON_MAXSCORE pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0x3E, 0x00, sizeof(pMsg));

	pMsg.nUserIndex = aRecv->nUserIndex;

	if (g_DSBattleCoreEnable == 1)
	{

	}
	else
	{
		QueryResult* res = this->m_EvoMonDB.Fetch("SELECT TotalScore FROM IGC_EvoMonRanking WHERE Name = '%s'", aRecv->szName);
		this->m_EvoMonDB.Fetch();
	}

	pMsg.nMaxScore = this->m_EvoMonDB.GetAsInteger("TotalScore");

	this->m_EvoMonDB.Close();

	//DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqEvoMonSaveScore(int aIndex, PMSG_REQ_SAVE_EVOMON_RESULT * aRecv)
{
	if (g_DSBattleCoreEnable == 1)
	{

	}
	else
	{
		QueryResult* res = this->m_EvoMonDB.Fetch("SELECT TotalScore, TotalDamage FROM IGC_EvoMonRanking WHERE Name = '%s'", aRecv->szName);

		if (this->m_EvoMonDB.Fetch() == SQL_NO_DATA)
		{
			this->m_EvoMonDB.Close();
			this->m_EvoMonDB.ExecQuery("INSERT INTO IGC_EvoMonRanking (Name, TotalScore, TotalDamage, LastSummonDate) VALUES ('%s', %d, %d, GETDATE())",
				aRecv->szName, aRecv->nScore, aRecv->nTotalDamage);
		}

		else
		{
			int nTotalScore = this->m_EvoMonDB.GetAsInteger("TotalScore");
			int nTotalDamage = this->m_EvoMonDB.GetAsInteger("TotalDamage");

			this->m_EvoMonDB.Close();

			char szTemp[128];
			char szTemp2[64];
			sprintf(szTemp, "UPDATE IGC_EvoMonRanking SET ");

			if (nTotalScore < aRecv->nScore)
			{
				strcat(szTemp, " TotalScore=");
				strcat(szTemp, itoa(aRecv->nScore, szTemp2, 10));
				strcat(szTemp, ", ");
			}

			if (nTotalDamage < aRecv->nTotalDamage)
			{
				strcat(szTemp, " TotalDamage=");
				strcat(szTemp, itoa(aRecv->nTotalDamage, szTemp2, 10));
				strcat(szTemp, ", ");
			}

			strcat(szTemp, "LastSummonDate=GETDATE() WHERE Name = '");
			strcat(szTemp, aRecv->szName);
			strcat(szTemp, "'");

			this->m_EvoMonDB.ExecQuery(szTemp);
		}
	}
}

void CDataServerProtocol::DGMuBotOptionRecv(MUBOT_SETTINGS_REQ_SAVE* lpMsg, int aIndex)
{
	char szName[MAX_IDSTRING + 1];
	memcpy(szName, lpMsg->szName, MAX_IDSTRING);
	szName[MAX_IDSTRING] = '\0';

	MuBotSaveOption(szName, lpMsg);
}

void CDataServerProtocol::MuBotSaveOption(char* szName, MUBOT_SETTINGS_REQ_SAVE* lpMsg)
{
	char szQuery[512];
	sprintf(szQuery, "UPDATE OptionData SET MuBot = ? WHERE Name = '%s'", szName);
	this->m_OptionDataDB.SetAsBinary(szQuery, lpMsg->btDATA, sizeof(lpMsg->btDATA));
	this->m_OptionDataDB.Close();
}

void CDataServerProtocol::GetMuBotData(char* szName, MUBOT_SETTINGS_SEND* lpMsg)
{
	char szQuery[512];
	sprintf(szQuery, "SELECT * FROM OptionData WHERE Name='%s'", szName);

	lpMsg->btDATA[0] = 0xFF;

	if (this->m_OptionDataDB.ExecQuery(szQuery) == TRUE)
	{
		if (this->m_OptionDataDB.Fetch() != SQL_NO_DATA)
		{
			this->m_OptionDataDB.Close();

			BYTE btTemp[sizeof(lpMsg->btDATA)] = { 0 };
			sprintf(szQuery, "SELECT MuBot FROM OptionData WHERE Name='%s'", szName);
			this->m_OptionDataDB.ReadBlob(szQuery, btTemp);
			memcpy(lpMsg->btDATA, btTemp, sizeof(lpMsg->btDATA));
		}
	}

	this->m_OptionDataDB.Close();
}

