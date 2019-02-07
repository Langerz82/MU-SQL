// DSprotocol.cpp: implementation of the DSprotocol class.
//
//////////////////////////////////////////////////////////////////////

#include "DSprotocol.h"
#include "giocp.h"
#include "ItemSerial.h"
#include "ServerEngine.h"
#include "MapServerManager.h"
#include "Main.h"
#include "Utility/util.h"

#define szModule "DSProtocol"

using namespace std;

CWarehouseUserData::CWarehouseUserData()
{
	InitializeCriticalSection(&this->m_WareDataCriti);
	this->m_vtWarehouseData.clear();
}

CWarehouseUserData::~CWarehouseUserData()
{
	DeleteCriticalSection(&this->m_WareDataCriti);
}

void CWarehouseUserData::Init()
{
	if(FALSE)
	{
		sLog.outError("[ERROR] - DATA SERVER CANNOT CONNECT TO MSSQL");
		return;
	}
}

void CWarehouseUserData::AddUserData(char * szAccountID)
{
	EnterCriticalSection(&this->m_WareDataCriti);

	for (std::vector<USERWAREHOUSE_DATA*>::iterator it = this->m_vtWarehouseData.begin(); it != this->m_vtWarehouseData.end(); it++)
	{
		if ( strcmp(szAccountID, it->szAccountID) == 0 )
		{
			this->m_vtWarehouseData.erase(it);
			break;
		}
	}

	USERWAREHOUSE_DATA m_UserWareData;

	std::memcpy(m_UserWareData.szAccountID, szAccountID, MAX_ACCOUNT_LEN+1);
	m_UserWareData.WarehouseID = 0;
	m_UserWareData.ChangeIDEnableState = TRUE;
	m_UserWareData.WarehouseOpenState = false;
	m_UserWareData.LastChangeTick = GetTickCount();

	this->m_vtWarehouseData.push_back(m_UserWareData);

	LeaveCriticalSection(&this->m_WareDataCriti);
}

void CWarehouseUserData::DelUserData(char * szAccountID)
{
	for (std::vector<USERWAREHOUSE_DATA*>::iterator it = this->m_vtWarehouseData.begin(); it != this->m_vtWarehouseData.end(); it++)
	{
		if ( strcmp(szAccountID, it->szAccountID) == 0 )
		{
			this->m_vtWarehouseData.erase(it);
			break;
		}
	}
}

void CWarehouseUserData::SetChangeEnableState(char * szAccountID, int State)
{
	for (std::vector<USERWAREHOUSE_DATA*>::iterator it = this->m_vtWarehouseData.begin(); it != this->m_vtWarehouseData.end(); it++)
	{
		if ( strcmp(szAccountID, it->szAccountID) == 0 )
		{
			it->ChangeIDEnableState = State;
			it->LastChangeTick = GetTickCount();
			break;
		}
	}
}

int CWarehouseUserData::SwitchWarehouse(char *szAccountID, int WarehouseID)
{
	USERWAREHOUSE_DATA* pData = NULL;

	for (std::vector<USERWAREHOUSE_DATA*>::iterator it = this->m_vtWarehouseData.begin(); it != this->m_vtWarehouseData.end(); it++)
	{
		if ( strcmp(szAccountID, it->szAccountID) == 0 )
		{
			pData = &*it;
			break;
		}
	}

	if (pData == NULL)
	{
		return 1;
	}

	if (pData->WarehouseOpenState == true)
	{
		return 5;
	}

	if (GetTickCount() - pData->LastChangeTick < 30000)
	{
		return 2;
	}

	if (pData->ChangeIDEnableState == FALSE)
	{
		return 3;
	}

	if (pData->WarehouseID == WarehouseID)
	{
		return 4;
	}

	pData->WarehouseID = WarehouseID;
	pData->LastChangeTick = GetTickCount();
	return 0;
}
	
int CWarehouseUserData::GetWarehouseID(char * szAccountID)
{
	int WareID = 0;

	for (std::vector<USERWAREHOUSE_DATA*>::iterator it = this->m_vtWarehouseData.begin(); it != this->m_vtWarehouseData.end(); it++)
	{
		if ( strcmp(szAccountID, it->szAccountID) == 0 )
		{
			WareID = it->WarehouseID;
			break;
		}
	}

	return WareID;
}

bool CWarehouseUserData::GetWarehouseOpenState(char * szAccountID)
{
	for (std::vector<USERWAREHOUSE_DATA*>::iterator it = this->m_vtWarehouseData.begin(); it != this->m_vtWarehouseData.end(); it++)
	{
		if (strcmp(szAccountID, it->szAccountID) == 0)
		{
			return it->WarehouseOpenState;
		}
	}

	return false;
}

void CWarehouseUserData::SetWarehouseOpenState(char * szAccountID, bool bState)
{
	for (std::vector<USERWAREHOUSE_DATA*>::iterator it = this->m_vtWarehouseData.begin(); it != this->m_vtWarehouseData.end(); it++)
	{
		if (strcmp(szAccountID, it->szAccountID) == 0)
		{
			it->WarehouseOpenState = bState;
		}
	}
}

void CWarehouseUserData::GDReqSwitchWarehouse(int aIndex, PMSG_REQ_SWITCHWARE * aRecv)
{
	EnterCriticalSection(&this->m_WareDataCriti);

	PMSG_ANS_SWITCHWARE pMsg;
	PHeadSetB((LPBYTE)&pMsg, 0x03, sizeof(pMsg));

	pMsg.iIndex = aRecv->iIndex;
	pMsg.WarehouseID = aRecv->WarehouseID;
	std::memcpy(pMsg.szAccountID, aRecv->szAccountID, MAX_ACCOUNT_LEN+1);
	pMsg.Result = this->SwitchWarehouse(aRecv->szAccountID, aRecv->WarehouseID);

	LeaveCriticalSection(&this->m_WareDataCriti);

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
	sLog.outBasic("[Warehouse][%s] Switch to Warehouse:%d Result:%d", pMsg.szAccountID, pMsg.WarehouseID, pMsg.Result);
}

void CWarehouseUserData::DGGetWarehouseList(int aIndex, SDHP_GETWAREHOUSEDB * aRecv)
{
	EnterCriticalSection(&this->m_WareDataCriti);

	char szAccountID[11]={0};
	szAccountID[10] = 0;
	std::memcpy(szAccountID, aRecv->AccountID, 10);

	SDHP_GETWAREHOUSEDB_SAVE pResult;

	PHeadSetW((LPBYTE)&pResult, 0x08, sizeof(pResult));
	std::memcpy(pResult.AccountID, szAccountID, 11);
	pResult.aIndex = aRecv->aIndex;

	if (this->GetWarehouseOpenState(szAccountID) == true)
	{
		sLog.outError("[Warehouse] (%s) - trying to open already opened warehouse.", szAccountID);

		LeaveCriticalSection(&this->m_WareDataCriti);
		return;
	}

	
	if (FALSE)
	{

		this->SetChangeEnableState(szAccountID, FALSE);
		this->SetWarehouseOpenState(szAccountID, true);
		
		SDHP_GETWAREHOUSEDB_RESULT pMsg;
		PHeadSetB((LPBYTE)&pMsg, 0x10, sizeof(pMsg));
		pMsg.aIndex = aRecv->aIndex;
		std::memcpy(pMsg.AccountID, szAccountID, 11);
		
		LeaveCriticalSection(&this->m_WareDataCriti);
		DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
		return;
	}


	char szTemp[128];

	this->SetChangeEnableState(szAccountID, FALSE);
	this->SetWarehouseOpenState(szAccountID, true);
	int WarehouseID = this->GetWarehouseID(szAccountID);

	switch(WarehouseID)
	{
		case 0:
			wsprintf(szTemp, "SELECT Items FROM warehouse WHERE AccountID='%s'", szAccountID);
			break;

		default:
			wsprintf(szTemp, "SELECT Items%d FROM warehouse WHERE AccountID='%s'",WarehouseID+1,szAccountID);
			break;
	};
	
	int ret = 0;

	if (ret < 0)
	{
		this->SetChangeEnableState(szAccountID, TRUE);
		this->SetWarehouseOpenState(szAccountID, false);

		LeaveCriticalSection(&this->m_WareDataCriti);
		return;
	}

	if (ret == 0)
	{
		std::memset(pResult.dbItems, -1, sizeof(pResult.dbItems));
	}

	else if (ret > 0 && ret < sizeof(pResult.dbItems))
	{
		for (int i = ret; i < sizeof(pResult.dbItems); i++)
		{
			pResult.dbItems[i] = -1;
		}
	}

	LeaveCriticalSection(&this->m_WareDataCriti);
	DataSend(aIndex, (LPBYTE)&pResult, sizeof(pResult), __FUNCTION__);
}

void CWarehouseUserData::GDSetWarehouseList(int aIndex, SDHP_GETWAREHOUSEDB_SAVE * aRecv)
{
	EnterCriticalSection(&this->m_WareDataCriti);

	char szAccountID[11]={0};
	szAccountID[10] = 0;
	std::memcpy(szAccountID, aRecv->AccountID, 10);

	if (this->GetWarehouseOpenState(szAccountID) == false)
	{
		sLog.outError("[Warehouse] (%s) - trying to close already closed warehouse. (Item)", szAccountID);

		//LeaveCriticalSection(&this->m_WareDataCriti);
		return;
	}

	char it[15];
	std::sprintf(it,"Items%d",aRecv->WarehouseID+1);
	char szTemp[128];

	int WarehouseID = this->GetWarehouseID(szAccountID);

	switch(WarehouseID)
	{
		case 0:
			wsprintf(szTemp, "UPDATE warehouse SET Items=? WHERE AccountID='%s'", szAccountID);
			break;

		default:
			wsprintf(szTemp, "UPDATE warehouse SET Items%d=? WHERE AccountID='%s'",WarehouseID+1,szAccountID);
			break;
	};


	if (aRecv->CloseWindow == TRUE)
	{

		this->SetChangeEnableState(szAccountID, TRUE);
		this->SetWarehouseOpenState(szAccountID, false);
	}

	PMSG_ANS_WARESAVE pMsg;
	PHeadSetB((LPBYTE)&pMsg, 0x09, sizeof(pMsg));

	pMsg.Result = TRUE;
	pMsg.CloseWindow = aRecv->CloseWindow;
	pMsg.iIndex = aRecv->aIndex;

	LeaveCriticalSection(&this->m_WareDataCriti);
	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CWarehouseUserData::GDSetWarehouseMoney(int aIndex, SDHP_WAREHOUSEMONEY_SAVE * aRecv)
{
	EnterCriticalSection(&this->m_WareDataCriti);

	char szAccountID[11] = { 0 };
	szAccountID[10] = 0;
	std::memcpy(szAccountID, aRecv->AccountID, 10);
	

	LeaveCriticalSection(&this->m_WareDataCriti);
}

CDataServerProtocol::CDataServerProtocol()
{
	this->m_iConnectionCount = 0;
	this->m_iItemCount = 0;

}

CDataServerProtocol::~CDataServerProtocol()
{

}

BOOL CDataServerProtocol::Init()
{
	if (!g_UseDataServer)
	{
		return FALSE;
	}

	if(FALSE)
	{
		sLog.outError("[ERROR] - DATA SERVER CANNOT CONNECT TO MSSQL");
		return FALSE;
	}

	this->m_iConnectionCount++;

	if(FALSE)
	{
		sLog.outError("[ERROR] - DATA SERVER CANNOT CONNECT TO MSSQL");
		return FALSE;
	}

	this->m_iConnectionCount++;

	if(FALSE)
	{
		sLog.outError("[ERROR] - DATA SERVER CANNOT CONNECT TO MSSQL");
		return FALSE;
	}

	this->m_iConnectionCount++;

	if(FALSE)
	{
		sLog.outError("[ERROR] - DATA SERVER CANNOT CONNECT TO MSSQL");
		return FALSE;
	}

	this->m_iConnectionCount++;

	if(FALSE)
	{
		sLog.outError("[ERROR] - DATA SERVER CANNOT CONNECT TO MSSQL");
		return FALSE;
	}

	this->m_iConnectionCount++;

	if(FALSE)
	{
		sLog.outError("[ERROR] - DATA SERVER CANNOT CONNECT TO MSSQL");
		return FALSE;
	}

	this->m_iConnectionCount++;

	if(FALSE)
	{
		sLog.outError("[ERROR] - DATA SERVER CANNOT CONNECT TO MSSQL");
		return FALSE;
	}

	this->m_iConnectionCount++;

	if(FALSE)
	{
		sLog.outError("[ERROR] - DATA SERVER CANNOT CONNECT TO MSSQL");
		return FALSE;
	}

	this->m_iConnectionCount++;

	if(FALSE)
	{
		sLog.outError("[ERROR] - DATA SERVER CANNOT CONNECT TO MSSQL");
		return FALSE;
	}

	this->m_iConnectionCount++;

	if(FALSE)
	{
		sLog.outError("[ERROR] - DATA SERVER CANNOT CONNECT TO MSSQL");
		return FALSE;
	}

	this->m_iConnectionCount++;

	if(FALSE)
	{
		sLog.outError("[ERROR] - DATA SERVER CANNOT CONNECT TO MSSQL");
		return FALSE;
	}

	this->m_iConnectionCount++;

	if(FALSE)
	{
		sLog.outError("[ERROR] - DATA SERVER CANNOT CONNECT TO MSSQL");
		return FALSE;
	}

	this->m_iConnectionCount++;

	if(FALSE)
	{
		sLog.outError("[ERROR] - DATA SERVER CANNOT CONNECT TO MSSQL");
		return FALSE;
	}

	this->m_iConnectionCount++;

	if (FALSE)
	{
		sLog.outError("[ERROR] - DATA SERVER CANNOT CONNECT TO MSSQL");
		return FALSE;
	}

	this->m_iConnectionCount++;

	if (FALSE)
	{
		sLog.outError("[ERROR] - DATA SERVER CANNOT CONNECT TO MSSQL");
		return FALSE;
	}

	this->m_iConnectionCount++;

	if (FALSE)
	{
		sLog.outError("[ERROR] - DATA SERVER CANNOT CONNECT TO MSSQL");
		return FALSE;
	}

	this->m_iConnectionCount++;

	if (FALSE)
	{
		sLog.outError("[ERROR] - DATA SERVER CANNOT CONNECT TO MSSQL");
		return FALSE;
	}

	this->m_iConnectionCount++;

	if (FALSE)
	{
		sLog.outError("[ERROR] - DATA SERVER CANNOT CONNECT TO MSSQL");
		return FALSE;
	}

	this->m_iConnectionCount++;

	if (FALSE)
	{
		sLog.outError("[ERROR] - DATA SERVER CANNOT CONNECT TO MSSQL");
		return FALSE;
	}

	this->m_iConnectionCount++;

	if (FALSE)
	{
		sLog.outError("[ERROR] - DATA SERVER CANNOT CONNECT TO MSSQL");
		return FALSE;
	}

	this->m_iConnectionCount++;

	if (FALSE)
	{
		sLog.outError("[ERROR] - DATA SERVER CANNOT CONNECT TO MSSQL");
		return FALSE;
	}

	this->m_iConnectionCount++;

	if (FALSE)
	{
		sLog.outError("[ERROR] - DATA SERVER CANNOT CONNECT TO MSSQL");
		return FALSE;
	}

	this->m_iConnectionCount++;

	if (FALSE)
	{
		sLog.outError("[ERROR] - DATA SERVER CANNOT CONNECT TO MSSQL");
		return FALSE;
	}

	this->m_iConnectionCount++;

	if (FALSE)
	{
		sLog.outError("[ERROR] - DATA SERVER CANNOT CONNECT TO MSSQL");
		return FALSE;
	}

	this->m_iConnectionCount++;

	if (FALSE)
	{
		sLog.outError("[ERROR] - DATA SERVER CANNOT CONNECT TO MSSQL");
		return FALSE;
	}

	this->m_iConnectionCount++;

	if (FALSE)
	{
		sLog.outError("[ERROR] - DATA SERVER CANNOT CONNECT TO MSSQL");
		return FALSE;
	}

	this->m_iConnectionCount++;

	if (FALSE)
	{
		sLog.outError("[ERROR] - DATA SERVER CANNOT CONNECT TO MSSQL");
		return FALSE;
	}

	this->m_iConnectionCount++;

	if (FALSE)
	{
		sLog.outError("[ERROR] - DATA SERVER CANNOT CONNECT TO MSSQL");
		return FALSE;
	}

	this->m_iConnectionCount++;

	this->m_WareUserData.Init();

	sLog.outBasic("[SUCCESS] - DATA SERVER CONNECT MSSQL SUCCESS (COUNT: %d)", this->m_iConnectionCount);

	sLog.outBasic("[DataServer] ItemCount: %d", this->m_iItemCount);

	return TRUE;
}

void CDataServerProtocol::ProtocolCore(int aIndex, BYTE HeadCode, LPBYTE aRecv, int iSize)
{
#if (TRACE_PACKET == 1 )
	LogAddHeadHex("DATA_SERVER", aRecv, iSize);
#endif

	switch ( HeadCode )
	{
		case 0x00:
			m_DSProtocol.DataServerLogin(aIndex, (SDHP_SERVERINFO *)aRecv);
			break;
		case 0x01:
			m_DSProtocol.JGPGetCharList(aIndex, (SDHP_GETCHARLIST *)aRecv);
			break;
		case 0x02:
			m_DSProtocol.GDReqClassDefData(aIndex);
			break;
		case 0x03:
			m_DSProtocol.m_WareUserData.GDReqSwitchWarehouse(aIndex, (PMSG_REQ_SWITCHWARE *)aRecv);
			break;
		case 0x04:
			m_DSProtocol.JGCharacterCreateRequest(aIndex, (SDHP_CREATECHAR *)aRecv);
			break;
		case 0x05:
			m_DSProtocol.JGCharDelRequest(aIndex, (SDHP_CHARDELETE *)aRecv);
			break;
		case 0x06:
			// TODO
			break;
		case 0x07:
			
			break;
		case 0x08:

			break;
		case 0x09:

			break;
		case 0x11:

			break;
		case 0x12:
			m_DSProtocol.m_WareUserData.GDSetWarehouseMoney(aIndex, (SDHP_WAREHOUSEMONEY_SAVE *) aRecv);
			break;
		case 0x3E:
			{
				PMSG_DEFAULT2 * lpDef1 = (PMSG_DEFAULT2 *)aRecv;

				switch (lpDef1->subcode)
					{
					case 0x00:
						m_DSProtocol.GDReqEvoMonMaxScore(aIndex, (PMSG_REQ_EVOMON_MAXSCORE *)aRecv);
						break;
					case 0x01:
						m_DSProtocol.GDReqEvoMonSaveScore(aIndex, (PMSG_REQ_SAVE_EVOMON_RESULT *)aRecv);
						break;
					}
			}
			break;
		case 0x4C:
			if (aRecv[3] == 0x00)
				m_DSProtocol.GDReqMineModifyUPTUserInfo(aIndex, (SDHP_REQ_MINESYSTEM_UPT_USERINFO *)aRecv);
			else if (aRecv[3] == 0x01)
				m_DSProtocol.GDReqMineCheckIsUPTWhenUserConnect(aIndex, (SDHP_REQ_LOAD_MINESYSTEM_UPT_USERINFO *)aRecv);
			break;
		case 0x4F:
			{
				PMSG_DEFAULT2 * lpDef1 = (PMSG_DEFAULT2 *)aRecv;

				switch(lpDef1->subcode)
				{
					case 0x00:
						m_DSProtocol.GDReqGremoryCaseItemList(aIndex, (_stReqGremoryCaseItemList *) aRecv);
						break;
					case 0x01:
						m_DSProtocol.GDReqGremoryCaseAddItem(aIndex, (_stReqAddItemToGremoryCase *) aRecv);
						break;
					case 0x02:
						m_DSProtocol.GDReqCheckUseItemGremoryCase(aIndex, (_stReqCheckUseItemGremoryCase *) aRecv);
						break;
					case 0x03:
						m_DSProtocol.GDReqGremoryCaseDeleteItem(aIndex, (_stReqDeleteItemFromGremoryCase *) aRecv);
						break;
				}
			}
			break;
		case 0x52:
			m_DSProtocol.ItemSerialCreateRecv(aIndex, (SDHP_ITEMCREATE *) aRecv);
			break;
		case 0x55:
			m_DSProtocol.PetItemSerialCreateRecv(aIndex, (SDHP_ITEMCREATE *) aRecv);
			break;
		case 0x56:
			m_DSProtocol.DGRecvPetItemInfo(aIndex, (SDHP_REQUEST_PETITEM_INFO *) aRecv);
			break;
		case 0x57:
			m_DSProtocol.GDSavePetItemInfo(aIndex, (SDHP_SAVE_PETITEM_INFO *) aRecv);
			break;
		case 0x60:
			m_DSProtocol.DGOptionDataRecv(aIndex, (SDHP_SKILLKEYDATA *) aRecv);
			break;
		case 0x61:
			m_DSProtocol.DGMuBotOptionRecv((MUBOT_SETTINGS_REQ_SAVE*)aRecv, aIndex);
			break;
		case 0x6F:
			{
				PMSG_DEFAULT2 * lpDef1 = (PMSG_DEFAULT2 *)aRecv;

				switch(lpDef1->subcode)
				{
					case 0x00:
						m_DSProtocol.GDReqReBuyItemList(aIndex, (SDHP_REQ_SHOP_REBUY_LIST*) aRecv);
						break;
					case 0x01:
						m_DSProtocol.GDReqReBuyAddItem(aIndex, (SDHP_REQ_SHOP_REBUY_ADD_ITEM*) aRecv);
						break;
					case 0x02:
						m_DSProtocol.GDReqReBuyGetItem(aIndex, (SDHP_REQ_SHOP_REBUY_GET_ITEM*) aRecv);
						break;
					case 0x03:
						m_DSProtocol.GDReqDeleteSoldItem(aIndex, (SDHP_REQ_SHOP_REBUY_DELETE_ITEM*) aRecv);
						break;
				}
			}
			break;
		case 0x80:
			{
				PMSG_DEFAULT2 * lpDef1 = (PMSG_DEFAULT2 *)aRecv;

				switch(lpDef1->subcode)
				{
				case 0x00:
					//	NOT Called by GameServer
					break;
				case 0x01:
					m_DSProtocol.GS_DGAnsOwnerGuildMaster(aIndex,(CSP_REQ_OWNERGUILDMASTER*)aRecv);
					break;
				case 0x03:
					m_DSProtocol.GS_DGAnsCastleNpcBuy(aIndex,(CSP_REQ_NPCBUY *)aRecv);
					break;
				case 0x04:
					m_DSProtocol.GS_DGAnsCastleNpcRepair(aIndex,(CSP_REQ_NPCREPAIR*)aRecv);
					break;
				case 0x05:
					m_DSProtocol.GS_DGAnsCastleNpcUpgrade(aIndex,(CSP_REQ_NPCUPGRADE*)aRecv);
					break;
				case 0x06:
					m_DSProtocol.GS_DGAnsTaxInfo(aIndex,(CSP_REQ_TAXINFO*)aRecv);
					break;
				case 0x07:
					m_DSProtocol.GS_DGAnsTaxRateChange(aIndex,(CSP_REQ_TAXRATECHANGE*)aRecv);
					break;
				case 0x08:
					m_DSProtocol.GS_DGAnsCastleMoneyChange(aIndex,(CSP_REQ_MONEYCHANGE*)aRecv);
					break;
				case 0x09:
					m_DSProtocol.GS_DGAnsSiegeDateChange(aIndex,(CSP_REQ_SDEDCHANGE*)aRecv);
					break;
				case 0x0A:
					m_DSProtocol.GS_DGAnsGuildMarkRegInfo(aIndex,(CSP_REQ_GUILDREGINFO*)aRecv);
					break;
				case 0x0B:
					m_DSProtocol.GS_DGAnsSiegeEndedChange(aIndex,(CSP_REQ_SIEGEENDCHANGE*)aRecv);
					break;
				case 0x0C:
					m_DSProtocol.GS_DGAnsCastleOwnerChange(aIndex,(CSP_REQ_CASTLEOWNERCHANGE*)aRecv);
					break;
				case 0x0D:
					m_DSProtocol.GS_DGAnsRegAttackGuild(aIndex,(CSP_REQ_REGATTACKGUILD*)aRecv);
					break;
				case 0x0E:
					m_DSProtocol.GS_DGAnsRestartCastleState(aIndex,(CSP_REQ_CASTLESIEGEEND*)aRecv);
					break;
				case 0x0F:
					m_DSProtocol.GS_DGAnsMapSvrMsgMultiCast(aIndex,(CSP_REQ_MAPSVRMULTICAST*)aRecv);
					break;
				case 0x10:
					m_DSProtocol.GS_DGAnsRegGuildMark(aIndex,(CSP_REQ_GUILDREGMARK*)aRecv);
					break;
				case 0x11:
					m_DSProtocol.GS_DGAnsGuildMarkReset(aIndex,(CSP_REQ_GUILDRESETMARK*)aRecv);
					break;
				case 0x12:
					m_DSProtocol.GS_DGAnsGuildSetGiveUp(aIndex,(CSP_REQ_GUILDSETGIVEUP*)aRecv);
					break;
				case 0x16:
					m_DSProtocol.GS_DGAnsNpcRemove(aIndex,(CSP_REQ_NPCREMOVE*)aRecv);
					break;
				case 0x17:
					m_DSProtocol.GS_DGAnsCastleStateSync(aIndex,(CSP_REQ_CASTLESTATESYNC*)aRecv);
					break;
				case 0x18:
					m_DSProtocol.GS_DGAnsCastleTributeMoney(aIndex,(CSP_REQ_CASTLETRIBUTEMONEY *) aRecv);
					break;
				case 0x19:
					m_DSProtocol.GS_DGAnsResetCastleTaxInfo(aIndex,(CSP_REQ_RESETCASTLETAXINFO*)aRecv);
					break;
				case 0x1A:
					m_DSProtocol.GS_DGAnsResetSiegeGuildInfo(aIndex,(CSP_REQ_RESETSIEGEGUILDINFO*)aRecv);
					break;
				case 0x1B:
					m_DSProtocol.GS_DGAnsResetRegSiegeInfo(aIndex,(CSP_REQ_RESETREGSIEGEINFO *) aRecv);
					break;
				case 0x1F:
					m_DSProtocol.GS_DGAnsGlobalPostMultiCast(aIndex,(CSP_GLOBALPOST_MULTICAST *) aRecv);
					break;
				}
			}
			break;
		case 0x81:
			m_DSProtocol.GS_DGAnsCastleInitData(aIndex,(CSP_REQ_CSINITDATA *)aRecv);
			break;
		case 0x82:
			//	NOT Called by GameServer
			break;
		case 0x83:
			m_DSProtocol.GS_DGAnsAllGuildMarkRegInfo(aIndex,(CSP_REQ_ALLGUILDREGINFO*)aRecv);
			break;
		case 0x84:
			m_DSProtocol.GS_DGAnsFirstCreateNPC(aIndex,(CSP_REQ_NPCSAVEDATA*)aRecv);
			break;
		case 0x85:
			m_DSProtocol.GS_DGAnsCalcRegGuildList(aIndex,(CSP_REQ_CALCREGGUILDLIST*)aRecv);
			break;
		case 0x86:
			m_DSProtocol.GS_DGAnsCsGulidUnionInfo(aIndex,(CSP_REQ_CSGUILDUNIONINFO *)aRecv);
			break;
		case 0x87:
			m_DSProtocol.GS_DGAnsCsSaveTotalGuildInfo(aIndex,(CSP_REQ_CSSAVETOTALGUILDINFO*)aRecv);
			break;
		case 0x88:
			m_DSProtocol.GS_DGAnsCsLoadTotalGuildInfo(aIndex,(CSP_REQ_CSLOADTOTALGUILDINFO*)aRecv);
			break;
		case 0x89:
			m_DSProtocol.GS_DGAnsCastleNpcUpdate(aIndex,(CSP_REQ_NPCUPDATEDATA*)aRecv);
			break;
		case 0xB0:
			m_DSProtocol.DGAnsCrywolfSync(aIndex,(CWP_REQ_CRYWOLFSYNC*)aRecv);
			break;
		case 0xB1:
			m_DSProtocol.DGAnsCrywolfInfoLoad(aIndex,(CWP_REQ_CRYWOLFINFOLOAD*)aRecv);
			break;
		case 0xB2:
			m_DSProtocol.DGAnsCrywolfInfoSave(aIndex,(CWP_REQ_CRYWOLFINFOSAVE*)aRecv);
			break;
		case 0xD0:
			{
				PMSG_DEFAULT2 * lpMsg = (PMSG_DEFAULT2 *)aRecv;

				switch ( lpMsg->subcode )
				{
					case 0x01:
						m_DSProtocol.DGAnsPeriodItemExInsert(aIndex,(PMSG_REQ_PERIODITEMEX_INSERT *)aRecv);
						break;					
					
					case 0x02:
						m_DSProtocol.DGAnsPeriodItemExDelete(aIndex,(PMSG_REQ_PERIODITEMEX_DELETE*)aRecv);
						break;

					case 0x03:
						m_DSProtocol.DGAnsPeriodItemExSelect(aIndex,(PMSG_REQ_PERIODITEMEX_SELECT *)aRecv);
						break;

					case 0x20:
						m_DSProtocol.GDReqLuckyItemSelect(aIndex, (PMSG_REQ_LUCKYITEM_SELECT *)aRecv);
						break;

					case 0x22:
						m_DSProtocol.GDReqLuckyItemInsert(aIndex, (PMSG_REQ_LUCKYITEM_INSERT *)aRecv);
						break;

					case 0x23:
						m_DSProtocol.GDReqLuckyItemDelete(aIndex, (PMSG_REQ_LUCKYITEM_DELETE *)aRecv);
						break;

					case 0x24:
						m_DSProtocol.GDReqLuckyItemInsert2nd(aIndex, (PMSG_REQ_LUCKYITEM_INSERT_2ND *)aRecv);
						break;

				}
			}
			break;

		case 0xBD:
			{
				PMSG_DEFAULT2 * lpMsg = reinterpret_cast<PMSG_DEFAULT2 *>(aRecv);

				switch ( lpMsg->subcode )
				{
					case 0x01:
						m_DSProtocol.DevilSqureScore(reinterpret_cast<PMSG_ANS_EVENTUSERSCORE *>(aRecv));
						break;
					case 0x02:
						m_DSProtocol.GDReqBloodCastleEnterCount(aIndex, reinterpret_cast<PMSG_REQ_BLOODCASTLE_ENTERCOUNT *>(aRecv));
						break;
					case 0x03:
						m_DSProtocol.BloodCastleScore_5TH(reinterpret_cast<PMSG_ANS_BLOODCASTLESCORE_5TH *>(aRecv));
						break;
					case 0x04:
						m_DSProtocol.IllusionTempleScore(reinterpret_cast<PMSG_ANS_ILLUSIONTEMPLE_RANKING *>(aRecv));
						break;
					case 0x05:
						m_DSProtocol.ChaosCastleScore(reinterpret_cast<PMSG_ANS_CHAOSCASTLE_RANKING *>(aRecv));
						break;
				}
			}
			break;
		case 0xBE:
			{
				PMSG_DEFAULT2 * lpMsg = reinterpret_cast<PMSG_DEFAULT2 *>(aRecv);
				
				switch ( lpMsg->subcode )
				{
					case 0x01:
						m_DSProtocol.EGAnsEventChipInfo(aIndex,(PMSG_REQ_VIEW_EC_MN *)aRecv);
						break;
					case 0x02:
						m_DSProtocol.EGAnsRegEventChipInfo(aIndex,(PMSG_REQ_REGISTER_EVENTCHIP *)aRecv);
						break;
					case 0x04:
						m_DSProtocol.EGAnsResetEventChip(aIndex,(PMSG_REQ_RESET_EVENTCHIP *)aRecv);
						break;
					case 0x05:
						m_DSProtocol.EGAnsEventStoneInfo(aIndex,(PMSG_REQ_VIEW_EC_MN *)aRecv);
						break;
					case 0x06:
						m_DSProtocol.EGAnsRegEventStoneInfo(aIndex,(PMSG_REQ_REGISTER_STONES *)aRecv);
						break;
					case 0x07:
						m_DSProtocol.EGAnsDeleteStones(aIndex,(PMSG_REQ_DELETE_STONES *)aRecv);
						break;
					case 0x08:
						m_DSProtocol.EGAns2AnivRegSerial(aIndex,(PMSG_REQ_2ANIV_SERIAL *)aRecv);
						break;
					case 0x09:
						m_DSProtocol.EGAnsResetStoneInfo(aIndex,(PMSG_REQ_RESET_EVENTCHIP *)aRecv);
						break;
					case 0x15:
						m_DSProtocol.EGAnsRegCCOfflineGift(aIndex,(PMSG_REQ_REG_CC_OFFLINE_GIFT *)aRecv);
						break;
					case 0x16:
						m_DSProtocol.EGAnsRegDLOfflineGift(aIndex,(PMSG_REQ_REG_DL_OFFLINE_GIFT *)aRecv);
						break;
					case 0x17:
						m_DSProtocol.EGAnsRegHTOfflineGift(aIndex,(PMSG_REQ_REG_HT_OFFLINE_GIFT *)aRecv);
						break;
					case 0x18:
						m_DSProtocol.EGAnsRegLuckyCoin(aIndex,(PMSG_REQ_REGISTER_LUCKYCOIN *)aRecv);
						break;
					case 0x19:
						m_DSProtocol.EGAnsLuckyCoinInfo(aIndex,(PMSG_REQ_LUCKYCOIN *)aRecv);
						break;
					case 0x20:
						m_DSProtocol.EGReqSantaCheck(aIndex,(PMSG_REQ_SANTACHECK *)aRecv);
						break;
					case 0x21:
						m_DSProtocol.EGReqSantaGift(aIndex,(PMSG_REQ_SANTAGIFT *)aRecv);
						break;
				}
			}
			break;
		case 0xCE:
			m_DSProtocol.GDDeleteTempUserInfo(aIndex, (SDHP_DELETE_TEMPUSERINFO *)aRecv);
			break;
		case 0xCF:
			m_DSProtocol.DGMoveOtherServer(aIndex, (SDHP_CHARACTER_TRANSFER *) aRecv);
			break;
		case 0xD1:
			m_DSProtocol.ReqInGameShopPoint(aIndex, (ISHOP_REQ_POINT *)aRecv);
			break;
		case 0xD2:
			m_DSProtocol.ReqInGameShopItemList(aIndex, (ISHOP_REQ_ITEMLIST *)aRecv);
			break;
		case 0xD4:
			m_DSProtocol.ReqInGameShopCharacterCardBuy(aIndex, (ISHOP_CHARCARD_BUY *)aRecv);
			break;
		case 0xD5:
			m_DSProtocol.ReqInGameShopItemBuy(aIndex, (ISHOP_ITEM_BUY *)aRecv);
			break;
		case 0xD6:
			m_DSProtocol.ReqInGameShopItemGift(aIndex, (ISHOP_ITEM_GIFT *)aRecv);
			break;
		case 0xD7:
			m_DSProtocol.ReqInGameShopAddPoint(aIndex, (ISHOP_POINT_ADD *)aRecv);
			break;
		case 0xD8:
			{
				PWMSG_DEFAULT2 * lpMsg = reinterpret_cast<PWMSG_DEFAULT2 *>(aRecv);
				
				switch ( lpMsg->subcode )
				{
					case 0x01:
						m_DSProtocol.ReqInGameShopPackageBuy(aIndex, aRecv);
						break;
					case 0x02:
						m_DSProtocol.ReqInGameShopPackageGift(aIndex, aRecv);
						break;
				}
			}
			break;
		case 0xD9:
			m_DSProtocol.ReqInGameShopItemUse(aIndex, (ISHOP_ITEM_USE *)aRecv);
			break;
		case 0xDA:
			m_DSProtocol.ReqSetExpandedWarehouse(aIndex, (SDHP_EXPANDEDWAREHOUSE_SET *)aRecv);
			break;
		case 0xDB:
			m_DSProtocol.ReqInGameShopItemDelete(aIndex, (ISHOP_ITEM_DELETE *)aRecv);
			break;
		case 0xDC:
			m_DSProtocol.ReqInGameShopItemRollbackUse(aIndex, (ISHOP_ITEM_ROLLBACK *)aRecv);
			break;
		case 0xE0:
			m_DSProtocol.GDReqGetPentagramJewel(aIndex, (PMSG_REQ_PENTAGRAMJEWEL *)aRecv);
			break;
		case 0xE1:
			m_DSProtocol.GDReqSetPentagramJewel(aIndex, aRecv);
			break;
		case 0xE2:
			m_DSProtocol.GDReqDelPentagramJewel(aIndex, (PMSG_DEL_PENTAGRAMJEWEL *)aRecv);
			break;
		case 0xE3:
			m_DSProtocol.GDReqInsertPentagramJewel(aIndex, (PMSG_INSERT_PENTAGRAMJEWEL *)aRecv);
			break;
		case 0xE4:
			{
				PMSG_DEFAULT2 * lpMsg = reinterpret_cast<PMSG_DEFAULT2 *>(aRecv);
				
				switch ( lpMsg->subcode )
				{
					case 0x01:

						break;
					case 0x02:

						break;
					case 0x03:

						break;
				}
			}
			break;
		case 0xE5:
			m_DSProtocol.GDReqArcaBattleAllJoinUser(aIndex, (PMSG_REQ_AB_ALL_JOIN_USER_DS *)aRecv);
			break;
		case 0xE6:

			break;
		case 0xE7:

			break;
		case 0xE8:
			{
				PMSG_DEFAULT2 * lpMsg = reinterpret_cast<PMSG_DEFAULT2 *>(aRecv);

				switch (lpMsg->subcode)
				{
					case 0x00:
						m_DSProtocol.GDReqCardInfo(aIndex, (PMSG_REQ_MURUMMY_SELECT_DS *)aRecv);
						break;
					case 0x01:
						m_DSProtocol.GDReqCardInfoInsert(aIndex, (PMSG_REQ_MURUMMY_INSERT_DS *)aRecv);
						break;
					case 0x02:
						m_DSProtocol.GDReqScoreUpdate(aIndex, (PMSG_REQ_MURUMMY_SCORE_UPDATE_DS *)aRecv);
						break;
					case 0x03:
						m_DSProtocol.GDReqCardInfoUpdate(aIndex, (PMSG_REQ_MURUMMY_UPDATE_DS *)aRecv);
						break;
					case 0x04:
						m_DSProtocol.GDReqScoreDelete(aIndex, (PMSG_REQ_MURUMMY_DELETE_DS *)aRecv);
						break;
					case 0x05:
						m_DSProtocol.GDReqSlotInfoUpdate(aIndex, (PMSG_REQ_MURUMMY_SLOTUPDATE_DS *)aRecv);
						break;
					case 0x06:
						m_DSProtocol.GDReqMuRummyInfoUpdate(aIndex, (PMSG_REQ_MURUMMY_INFO_UPDATE_DS *)aRecv);
						break;
					case 0x07:

						break;
				}
			}
			break;
		case 0xE9:
			m_DSProtocol.GDReqPShopItemValue(aIndex, (PMSG_REQ_PSHOPITEMVALUE_INFO *)aRecv);
			break;
		case 0xEB:
			m_DSProtocol.GDAllSavePShopItemValue(aIndex, (PMSG_UPDATE_PSHOPITEMVALUE_INFO *)aRecv);
			break;
		case 0xEC:
			m_DSProtocol.GDDelPShopItemValue(aIndex, (PMSG_DEL_PSHOPITEM *)aRecv);
			break;
		case 0xED:
			m_DSProtocol.GDMovePShopItem(aIndex, (PMSG_MOVE_PSHOPITEM *)aRecv);
			break;
		case 0xEE:
			{
				PMSG_DEFAULT2 * lpMsg = reinterpret_cast<PMSG_DEFAULT2 *>(aRecv);

				switch (lpMsg->subcode)
				{
					case 0x00:
						m_DSProtocol.GDReqEventEntryCount(aIndex, (PMSG_REQ_EVENTENTRY_INFO *)aRecv);
						break;
				}
			}
			break;
		case 0xF6:
			m_DSProtocol.GDReqQuestExpInfoSave(aIndex, aRecv);
			break;
		case 0xF7:
			m_DSProtocol.GDReqQuestExpInfoLoad(aIndex, (PMSG_REQ_QUESTEXP_INFO *)aRecv);
			break;
		case 0xF8:
			{
				PMSG_DEFAULT2 * lpMsg = reinterpret_cast<PMSG_DEFAULT2 *>(aRecv);
				
				switch ( lpMsg->subcode )
				{
					case 0x30:
						m_DSProtocol.GDReqArcaBattleGuildJoin(aIndex, (PMSG_REQ_ARCA_BATTLE_GUILD_JOIN_DS *)(aRecv));
						break;
					case 0x32:
						m_DSProtocol.GDReqArcaBattleGuildMemberJoin(aIndex, (PMSG_REQ_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS *)(aRecv));
						break;
					case 0x34:
						m_DSProtocol.GDReqArcaBattleEnter(aIndex, (PMSG_REQ_ARCA_BATTLE_ENTER_DS *)(aRecv));
						break;
					case 0x38:
						m_DSProtocol.GDReqArcaBattleWinGuildInfoInsert(aIndex, (PMSG_REQ_AB_WIN_GUILD_INFO_INSERT_DS *)(aRecv));
						break;
					case 0x39:
						m_DSProtocol.GDReqArcaBattleWinGuildInfo(aIndex, (PMSG_REQ_AB_WIN_GUILD_INFO_DS *)(aRecv));
						break;
					case 0x3D:
						m_DSProtocol.GDReqDeleteArcaBattleInfo(aIndex);
						break;
					case 0x3E:
						m_DSProtocol.GDReqArcaBattleProcMultiCast(aIndex, (PMSG_REQ_AB_PROC_INSERT_DS *)(aRecv));
						break;
					case 0x3F:
						m_DSProtocol.GDReqArcaBattleProcState(aIndex, (PMSG_REQ_AB_PROC_STATE_DS *)(aRecv));
						break;
					case 0x41:
						m_DSProtocol.GDReqArcaBattleJoinMemberUnder(aIndex, (PMSG_REQ_AB_JOIN_MEMBER_UNDER_DS *)(aRecv));
						break;
					case 0x43:
						m_DSProtocol.GDReqArcaBattleJoinMemberUnderReq(aIndex, (PMSG_REQ_AB_JOIN_CANCEL_DS *)(aRecv));
						break;
					case 0x45:
						m_DSProtocol.GDReqArcaBattleRegisteredMemberCnt(aIndex, (PMSG_REQ_AB_REG_MEMBER_CNT_DS *)(aRecv));
						break;
					case 0x47:
						m_DSProtocol.GDReqRemoveAllGuildBuffMultiCast(aIndex, (PMSG_REQ_REMOVE_ALL_GUILD_BUFF_DS *)(aRecv));
						break;
					case 0x4D:
						m_DSProtocol.GDReqArcaBattleMarkCnt(aIndex, (PMSG_REQ_ARCA_BATTLE_MARK_CNT_DS *)aRecv);
						break;
					case 0x4F:
						m_DSProtocol.GDReqArcaBattleMarkReg(aIndex, (PMSG_REQ_ARCA_BATTLE_MARK_REG_DS *)aRecv);
						break;
					case 0x51:
						m_DSProtocol.GDReqArcaBattleMarkRank(aIndex, (PMSG_REQ_ARCA_BATTLE_MARK_RANK_DS *)aRecv);
						break;
					case 0x53:
						m_DSProtocol.GDReqArcaBattleMarkRegDel(aIndex, (PMSG_REQ_ARCA_BATTLE_MARK_REG_DEL_DS *)aRecv);
						break;
					case 0x54:
						m_DSProtocol.GDReqArcaBattleIsTopRank(aIndex, (PMSG_REQ_ARCA_BATTLE_IS_TOP_RANK *)aRecv);
						break;
					case 0x55:
						m_DSProtocol.GDReqArcaBattleMarkRegAllDel(aIndex);
						break;
					case 0xF0:
						m_DSProtocol.GDReqAcheronGuardianProcMultiCast(aIndex, (PMSG_REQ_AE_PLAY_DS *)aRecv);
						break;
					case 0xFD:
						m_DSProtocol.GDReqArcaBattleMarkRegSet(aIndex, (PMSG_REQ_AB_MARK_REG_UPDATE_DS *)aRecv);
						break;
					case 0xFE:
						m_DSProtocol.GDReqArcaBattleGuildRegInit(aIndex, (PMSG_REQ_GUILD_REG_INIT *)aRecv);
						break;
					default:
						if (aRecv[4] == 0xFB)
						{
							m_DSProtocol.GDReqArcaBattleAllGuildMarkCnt(aIndex);
						}
						break;
				}

			}
			break;
		case 0xF9:
			{
				PMSG_DEFAULT2 * lpMsg = reinterpret_cast<PMSG_DEFAULT2 *>(aRecv);

				switch (lpMsg->subcode)
				{
					case 0x02:
						m_DSProtocol.GDReqUBFSetCCBattleReward(aIndex, (SDHP_REQ_SET_CC_WINNER_INFO_UBF *)aRecv);
						break;
					case 0xA1:
						m_DSProtocol.GDReqChaosCastleFinalSave(aIndex, (PMSG_REQ_SAVE_CCF_RESULT *)aRecv);
						break;
					case 0xA2:
						m_DSProtocol.GDReqChaosCastleFinalPermission(aIndex, (PMSG_REQ_CCF_PERMISSION *)aRecv);
						break;
					case 0xA3:
						m_DSProtocol.GDReqChaosCastleFinalLoad(aIndex, (SDHP_REQ_CCF_RANKING *)aRecv);
						break;
					case 0xA4:
						m_DSProtocol.GDReqChaosCastleFinalRenew(aIndex, (SDHP_RENEW_RANKING *)aRecv);
						break;
					case 0xA5:
						m_DSProtocol.GDReqChaosCastleFinalSendMsgAllSvr(aIndex, (PMSG_SEND_CCF_INFO_ALL_SVR *)aRecv);
						break;
					case 0xB0:
						m_DSProtocol.GDReqUBFSetCCFReward(aIndex, (SDHP_REQ_SET_CCF_WINNER_INFO *)aRecv);
						break;
				}
			}
			break;
		case 0xFA:
			m_DSProtocol.ReqBanUser(aIndex,(BAN_REQ_USER*)aRecv);
			break;
		case 0xFC:
			m_DSProtocol.ReqSaveMonsterCount(aIndex,(DS_SAVE_MONSTERCNT*)aRecv);
			break;
		case 0xFB:
			m_DSProtocol.ReqSecLock(aIndex,(SECLOCK_REQ_SAVE*)aRecv);
			break;
		case 0xFD:
			{
				PMSG_DEFAULT2 * lpMsg = reinterpret_cast<PMSG_DEFAULT2 *>(aRecv);
				
				switch ( lpMsg->subcode )
				{
					case 0x00:
						m_DSProtocol.GDReqDSFCanPartyEnter(aIndex, (PMSG_REQ_DSF_CAN_PARTY_ENTER *)aRecv);
						break;
					case 0x02:
						m_DSProtocol.GDReqDSFSavePartyPoint(aIndex, (PMSG_REQ_SAVE_DSF_PARTYPOINT *)aRecv);
						break;
					case 0x03:
						m_DSProtocol.GDReqDSFPartyRankRenew(aIndex, (PMSG_REQ_DSF_PARTYRANKRENEW *)aRecv);
						break;
					case 0x05:
						m_DSProtocol.GDReqDSFGoFinalParty(aIndex, (PMSG_REQ_DSF_GO_FINAL_PARTY *)aRecv);
						break;
					case 0x06:
						m_DSProtocol.GDReqDSFInsertRewardUser(aIndex, (PMSG_REQ_SAVE_DSF_REWARD_USER *)aRecv);
						break;
					case 0x07:
						m_DSProtocol.GDReqDSFGetReward(aIndex, (PMSG_REQ_GET_DSF_REWARD *)aRecv);
						break;
					case 0x08:
						m_DSProtocol.GDReqUBFSetDSFReward(aIndex, (PMSG_REQ_SET_DSF_WINNER_INFO *)aRecv);
						break;
				}
			}
			break;
		case 0xFE:
			m_DSProtocol.ReqSavePlayerKiller(aIndex,(DS_SAVE_PLAYERKILLER*)aRecv);
			break;
		case 0xF1:

			break;
		case 0xF2:

			break;
		case 0xF3:
			{
				PMSG_DEFAULT2 * lpMsg = reinterpret_cast<PMSG_DEFAULT2 *>(aRecv);

				switch (lpMsg->subcode)
				{
					case 0x01:
						m_DSProtocol.GDReqUBFCheckIsJoinedUser(aIndex, (PMSG_REQ_UBF_ACCOUNT_USERINFO *)aRecv);
						break;
					case 0x02:
						m_DSProtocol.GDReqUBFJoinUser(aIndex, (PMSG_UBF_REGISTER_ACCOUNT_USER *)aRecv);
						break;
					case 0x03:
						m_DSProtocol.GDReqUBFCopyCharacter(aIndex, (PMSG_UBF_ACCOUNT_USER_COPY *)aRecv);
						break;
					case 0x05:
						m_DSProtocol.GDReqUBFSetGainReward(aIndex, (PMSG_REQ_UBF_SET_RECEIVED_REWARD *)aRecv);
						break;
					case 0x06:
						m_DSProtocol.GDReqUBFGetReward(aIndex, (PMSG_REQ_UBF_GET_REWARD *)aRecv);
						break;
					case 0x07:
						m_DSProtocol.GDReqUBFCancelUser(aIndex, (PMSG_UBF_REQ_CANCEL_REGISTER_USER *)aRecv);
						break;
					case 0x08:
						m_DSProtocol.GDReqUBFGetRealName(aIndex, (PMSG_REQ_GET_UBF_REAL_NAME *)aRecv);
						break;
				}
			}
			break;
		case 0xF4:
			m_DSProtocol.GDReqUBFCopyPetItem(aIndex, aRecv);
			break;
		case 0xC2:
			m_DSProtocol.GDReqWishperOtherChannel(aIndex,(PMSG_RECV_CHATDATA_WHISPER*)aRecv);
			break;
		case 0xC3:
			{
				PMSG_DEFAULT2 * lpMsg = reinterpret_cast<PMSG_DEFAULT2 *>(aRecv);
				switch(lpMsg->subcode)
				{
					case 0x00:
						m_DSProtocol.GDReqMapSrvGroupServerCount(aIndex,(DSMSG_REQ_SUBSERVER_COUNT*)aRecv);
						break;
					case 0x02:
						m_DSProtocol.DGWhisperResponseRecv(aIndex,(DSMSG_ANS_WHISPER_RESULT*)aRecv);
						break;
					case 0x04:
						m_DSProtocol.GDDisconnectOtherChannel(aIndex,(PMSG_RECV_DC_OTHER_CHANNEL*)aRecv);
						break;
				}
			}
			break;
	}
}

void CDataServerProtocol::DataServerLogin(int aIndex, SDHP_SERVERINFO * lpMsg)
{
	SDHP_RESULT pResult = {0};

	PHeadSetB((LPBYTE)&pResult, 0x00, sizeof(pResult));
	pResult.Result = 1;
	pResult.ItemCount = this->m_iItemCount;
	
	g_Server[aIndex].m_ServerCode = lpMsg->ServerCode;
	g_Server[aIndex].m_ServerGroup = g_MapServerManager.GetMapSvrGroup(lpMsg->ServerCode);

	sLog.outBasic("[DataServer] GameServer(%d)(%d) connect",lpMsg->ServerCode, g_MapServerManager.GetMapSvrGroup(lpMsg->ServerCode));
	
	DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size, __FUNCTION__);
}

void CDataServerProtocol::JGPGetCharList(int aIndex, SDHP_GETCHARLIST * aRecv)
{
	char cBUFFER[10000]={0};

	SDHP_CHARLISTCOUNT * pCount = (SDHP_CHARLISTCOUNT *)(cBUFFER);
	SDHP_CHARLIST * pCL = (SDHP_CHARLIST *)(cBUFFER + sizeof(SDHP_CHARLISTCOUNT));

	char szAccountID[11];
	szAccountID[10]=0;
	std::memcpy(szAccountID, aRecv->Id, 10);

	pCount->Number = aRecv->Number;
	std::memcpy(pCount->AccountId, szAccountID, MAX_ACCOUNT_LEN);

	TCHAR CharName[5][11]={0};
	memset(CharName, 0x00, sizeof(CharName));
		
	sLog.outBasic("[%s] - characters: [%s][%s][%s][%s][%s]", szAccountID, CharName[0], CharName[1], CharName[2], CharName[3], CharName[4]);


	int iCharCount=0;
	int lOfs=sizeof(SDHP_CHARLISTCOUNT);

	for(int i=0;i<5;i++)
	{
		if ( CharName[i][0] != 0 )
		{
			if ( lstrlen(CharName[i]) >= 4 )
			{
				if (TRUE)
				{
					char szTemp[200];
					pCL = (SDHP_CHARLIST *)(cBUFFER + lOfs);


					BYTE btInvetory[INVENTORY_BINARY_SIZE];
					wsprintf(szTemp, "SELECT Inventory FROM Character WHERE Name='%s'", CharName[i]);


					int dbsize = 32;

					memset(pCL->dbInventory, -1, sizeof(pCL->dbInventory));

					for(int j=0;j<12;j++)
					{
						if ( btInvetory[0+j*dbsize] == 0xFF && (btInvetory[7+j*dbsize] & 0x80 ) == 0x80 && (btInvetory[10+j*dbsize] & 0xF0) == 0xF0 )
						{
							pCL->dbInventory[j*4] = -1;
							pCL->dbInventory[j*4+1] = -1;
							pCL->dbInventory[j*4+2] = -1;
							pCL->dbInventory[j*4+3] = -1;
						}
						else
						{
							pCL->dbInventory[j*4]  = (btInvetory[0+j*dbsize]);	// 0..7 bits of Item
							pCL->dbInventory[j*4+1]  = (btInvetory[1+j*dbsize]);	// Get Level of Item
							pCL->dbInventory[j*4+2]  = (btInvetory[7+j*dbsize]);	// 8 bit     of Item
							pCL->dbInventory[j*4+3]  = (btInvetory[9+j*dbsize]);	// 9..12 bit of Item
						}
					}
					


					if (aRecv->IsUnityBattleFieldServer == FALSE)
					{
						if(false)
						{
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
					}

					else
					{
						strcpy(pCL->UnityBFRealName, CharName[i]);
						pCL->ServerCodeOfHomeWorld = 0;
					}

					pCL->Index = i;
					std::memcpy(pCL->Name, CharName[i], MAX_ACCOUNT_LEN);
					//g_MuOnlineDB.Close();
					iCharCount++;
					lOfs+=sizeof(SDHP_CHARLIST);
				}


			}
		}
	}

	if ( g_MagumsaCreateMinLevel == 0 && (pCount->EnableCharacterCreate & 4) != 4 )
	{
		pCount->EnableCharacterCreate |= 4;
	}
					
	if ( g_DarkLordCreateMinLevel == 0 && (pCount->EnableCharacterCreate & 2) != 2 )
	{
		pCount->EnableCharacterCreate |= 2;
	}

	if ( g_GrowLancerCreateMinLevel == 0 && (pCount->EnableCharacterCreate & 0x10) != 0x10 )
	{
		pCount->EnableCharacterCreate |= 0x10;
	}

	PHeadSetW((LPBYTE)pCount, 0x01, sizeof(SDHP_CHARLISTCOUNT) + sizeof(SDHP_CHARLIST) * iCharCount);
	pCount->Count = iCharCount;
	
	DataSend(aIndex, (LPBYTE)cBUFFER, sizeof(SDHP_CHARLISTCOUNT) + sizeof(SDHP_CHARLIST) * iCharCount, __FUNCTION__);
}

void CDataServerProtocol::JGCharacterCreateRequest(int aIndex, SDHP_CREATECHAR * aRecv)
{
	SDHP_CREATECHARRESULT pResult={0};
	PHeadSetB((LPBYTE)&pResult, 0x04, sizeof(pResult));
	std::memcpy(pResult.AccountId, aRecv->AccountId, 10);
	pResult.ClassSkin = aRecv->ClassSkin;
	pResult.Number = aRecv->Number;
	int iIndex=0;

	char szAccountID[11]={0};
	char szName[11]={0};
	szAccountID[10]=0;
	szName[10]=0;
	std::memcpy(szAccountID, aRecv->AccountId, 10);
	std::memcpy(szName, aRecv->Name, 10);

	if (SpaceSyntexCheck(szName) == FALSE || QuoteSpaceSyntexCheck(szName) == FALSE || PercentSyntaxCheck(szName) == FALSE)
	{
		pResult.Result = 0;
		DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size, __FUNCTION__);
		return;
	}

	TCHAR CharName[5][11]={0};
	std::memset(CharName, 0x00, sizeof(CharName));
	
	for(iIndex=0;iIndex<5;iIndex++)
	{
		if ( CharName[iIndex][0] == 0 )
			break;
	}

	if ( iIndex == 5 )
	{
		pResult.Result = 2;
		sLog.outBasic("[%s] cannot create character - no empty slot", szAccountID);
	}


	DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size, __FUNCTION__);
}

void CDataServerProtocol::JGCharDelRequest(int aIndex, SDHP_CHARDELETE * aRecv)
{
	SDHP_CHARDELETERESULT pResult = { 0 };

	PHeadSetB((LPBYTE)&pResult, 0x05, sizeof(pResult));
	std::memcpy(pResult.AccountID, aRecv->AccountID, 10);
	pResult.Number = aRecv->Number;
	pResult.Result = 1;

	char szAccountID[11] = { 0 };
	char szName[11] = { 0 };
	szAccountID[10] = 0;
	szName[10] = 0;
	std::memcpy(szAccountID, aRecv->AccountID, 10);
	std::memcpy(szName, aRecv->Name, 10);

	if (lstrlen(aRecv->AccountID) < 1)
		pResult.Result = 0;

	if (lstrlen(aRecv->Name) < 1)
		pResult.Result = 0;

	if (pResult.Result == 1)
	{
		int iQueryResult;
		if (iQueryResult == TRUE)
		{
			TCHAR CharName[5][11] = { 0 };
			std::memset(CharName, 0x00, sizeof(CharName));

			int iIndex;
			for (iIndex = 0; iIndex < 5; iIndex++)
			{
				if (lstrcmp(CharName[iIndex], szName) == 0)
				{
					break;
				}
			}

			sLog.outBasic("[%s] deleted character -> [%s]", szAccountID, szName);
		}

		else
		{
			pResult.Result = 0;
		}
	}

	DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size, __FUNCTION__);
}

void CDataServerProtocol::JGGetCharacterInfo(int aIndex, SDHP_DBCHARINFOREQUEST * aRecv)
{
	SDHP_DBCHAR_INFORESULT pResult = { 0 };

	PHeadSetW((LPBYTE)&pResult, 0x06, sizeof(pResult));
	char szAccountID[11];
	char szName[11];
	szAccountID[10] = 0;
	szName[10] = 0;
	std::memcpy(szAccountID, aRecv->AccountID, 10);
	std::memcpy(szName, aRecv->Name, 10);
	std::memcpy(pResult.AccountID, aRecv->AccountID, 10);
	std::memcpy(pResult.Name, aRecv->Name, 10);
	pResult.Number = aRecv->Number;
	pResult.result = 0;
	char AccountCharacter[5][11];
	std::memset(AccountCharacter, 0x00, sizeof(AccountCharacter));


	if (false)
	{

	}


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
		sLog.outError("[ERROR] Char NOT EXIST (Name:%s),", szName);
		return;
	}

	char WHExpansion = 0;



	if (g_DSBattleCoreEnable == 1)
	{
		if (FALSE)
		{
			
			return;
		}
	}
	else
	{
		if (false)
		{
			return;
		}
	}

	if (false)
	{
		return;
	}


	if (g_DSBattleCoreEnable == 1)
	{

	}
	else
	{

	}

	char szTemp[256];
	BYTE btTemp[INVENTORY_BINARY_SIZE] = { 0 };
	std::memset(btTemp, 0xFF, sizeof(btTemp));

	wsprintf(szTemp, "SELECT Inventory from Character where Name='%s'", szName);

	std::memcpy(pResult.dbInventory, btTemp, sizeof(pResult.dbInventory));
	std::memset(btTemp, 0, sizeof(btTemp));

	wsprintf(szTemp, "SELECT MagicList from Character where Name='%s'", szName);
	
	std::memcpy(pResult.dbMagicList, btTemp, sizeof(pResult.dbMagicList));
	std::memset(btTemp, 0, sizeof(btTemp));

	wsprintf(szTemp, "SELECT Quest from Character where Name='%s'", szName);
	
	std::memcpy(pResult.dbQuest, btTemp, sizeof(pResult.dbQuest));
	std::memset(btTemp, 0, sizeof(btTemp));


	if (false)
	{
		int Days = 0;

		if (Days > 0)
		{
			pResult.GmCode = 0;
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

	
	pResult.result = 1;

	

	this->m_WareUserData.AddUserData(szAccountID);
	DataSend(aIndex, (LPBYTE)&pResult, sizeof(pResult), __FUNCTION__);

	if (pResult.result == 1)
	{
		SDHP_SKILLKEYDATA_SEND pSkillData = { 0 };

		PHeadSetB((LPBYTE)&pSkillData, 0x60, sizeof(pSkillData));
		pSkillData.aIndex = aRecv->Number;

	

		if (false)
		{
			std::memcpy(pSkillData.Name, szName, 10);

			BYTE btTemp[sizeof(pSkillData.SkillKeyBuffer)] = { 0 };
			char szTemp[256];
			wsprintf(szTemp, "SELECT SkillKey from OptionData where Name='%s'", szName);

			std::memcpy(pSkillData.SkillKeyBuffer, btTemp, 20);
		}

		else
		{
		}

		DataSend(aIndex, (LPBYTE)&pSkillData, sizeof(pSkillData), __FUNCTION__);
	}

	MUBOT_SETTINGS_SEND pMuBot = { 0 };
	pMuBot.h.c = PMHC_WORD;
	pMuBot.h.sizeH = SET_NUMBERH(sizeof(pMuBot));
	pMuBot.h.sizeL = SET_NUMBERL(sizeof(pMuBot));
	pMuBot.h.headcode = 0x61;

	strcpy(pMuBot.szName, szName);
	pMuBot.aIndex = aRecv->Number;
	GetMuBotData(szName, &pMuBot);
	DataSend(aIndex, (LPBYTE)&pMuBot, sizeof(pMuBot), __FUNCTION__);
}

void CDataServerProtocol::GJSetCharacterInfo(int aIndex, SDHP_DBCHAR_INFOSAVE * aRecv)
{
	char szName[11]={0};
	szName[10] = 0;
	std::memcpy(szName, aRecv->Name, 10);

	char szAccountID[11]={0};
	szAccountID[10] = 0;
	std::memcpy(szAccountID, aRecv->AccountID, 10);
	
	if (g_DSBattleCoreEnable == 1)
	{
	}
	else
	{
	}

	char szTemp[128];
	wsprintf(szTemp, "UPDATE Character SET Inventory=? WHERE Name='%s'", szName);

	wsprintf(szTemp, "UPDATE Character SET MagicList=? WHERE Name='%s'", szName);

	wsprintf(szTemp, "UPDATE Character SET Quest=? WHERE Name='%s'", szName);

	if ( aRecv->CharInfoSave == 1 )
	{
	}
}

void CDataServerProtocol::GDUserItemSave(int aIndex, SDHP_DBCHAR_ITEMSAVE * aRecv)
{
	char szName[11]={0};
	szName[10] = 0;
	std::memcpy(szName, aRecv->Name, 10);

	char szTemp[128];
	wsprintf(szTemp, "UPDATE Character SET Inventory=? WHERE Name='%s'", szName);
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
	std::memcpy(&pResult.SocketOption, aRecv->SocketOption, sizeof(pResult.SocketOption));
	pResult.MainAttribute = aRecv->MainAttribute;

	DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size, __FUNCTION__);
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




	pResult.MapNumber = aRecv->MapNumber;
	pResult.NewOption = aRecv->NewOption;
	pResult.Op1 = aRecv->Op1;
	pResult.Op2 = aRecv->Op2;
	pResult.Op3 = aRecv->Op3;
	pResult.SetOption = aRecv->SetOption;
	pResult.Type = aRecv->Type;
	pResult.x = aRecv->x;
	pResult.y = aRecv->y;
	std::memcpy(&pResult.SocketOption, aRecv->SocketOption, sizeof(pResult.SocketOption));
	pResult.lDuration = aRecv->lDuration;
	pResult.MainAttribute = aRecv->MainAttribute;

	DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size, __FUNCTION__);
}

void CDataServerProtocol::DGRecvPetItemInfo(int aIndex, SDHP_REQUEST_PETITEM_INFO * aRecv)
{
	char szAccountID[11]={0};
	szAccountID[10] = 0;
	std::memcpy(szAccountID, aRecv->AccountID, 10);
	int lOfs1 = sizeof(SDHP_REQUEST_PETITEM_INFO);
	int lOfs2 = sizeof(SDHP_RECV_PETITEM_INFO);
	char cBUFFER[4092]={0};
	SDHP_RECV_PETITEM_INFO * pRecvPetInfoCount = (SDHP_RECV_PETITEM_INFO *)(cBUFFER);
	Request_PetItem_Info * pRequestPetInfo;
	Recv_PetItem_Info * pRecvPetInfo;

	if (aRecv->ServerType == 0) // normal GS
	{
		for(int i=0;i<aRecv->nCount;i++)
		{
			pRequestPetInfo = (Request_PetItem_Info *)((LPBYTE)aRecv + lOfs1);
			pRecvPetInfo = (Recv_PetItem_Info *)((LPBYTE)cBUFFER + lOfs2);

			pRecvPetInfo->nPos = pRequestPetInfo->nPos;
			pRecvPetInfo->nSerial = pRequestPetInfo->nSerial;
	

			pRecvPetInfoCount->nCount++;

			lOfs1+=sizeof(Request_PetItem_Info);
			lOfs2+=sizeof(Recv_PetItem_Info);
		}
	}

	else // battlecore GS
	{
		for(int i=0;i<aRecv->nCount;i++)
		{
			pRequestPetInfo = (Request_PetItem_Info *)((LPBYTE)aRecv + lOfs1);
			pRecvPetInfo = (Recv_PetItem_Info *)((LPBYTE)cBUFFER + lOfs2);

			pRecvPetInfoCount->nCount++;

			lOfs1+=sizeof(Request_PetItem_Info);
			lOfs2+=sizeof(Recv_PetItem_Info);
		}
	}

	PHeadSetW((LPBYTE)pRecvPetInfoCount, 0x56, sizeof(SDHP_RECV_PETITEM_INFO) + pRecvPetInfoCount->nCount * sizeof(Recv_PetItem_Info));
	//pRecvPetInfoCount->nCount = aRecv->nCount;
	pRecvPetInfoCount->InvenType = aRecv->InvenType;
	pRecvPetInfoCount->Number = aRecv->Number;
	std::memcpy(pRecvPetInfoCount->AccountID, szAccountID, 11);

	DataSend(aIndex, (LPBYTE)cBUFFER, sizeof(SDHP_RECV_PETITEM_INFO) + pRecvPetInfoCount->nCount * sizeof(Recv_PetItem_Info), __FUNCTION__);
}

void CDataServerProtocol::GDSavePetItemInfo(int aIndex, SDHP_SAVE_PETITEM_INFO * aRecv)
{
	int lOfs = sizeof(SDHP_SAVE_PETITEM_INFO);

	Save_PetItem_Info * pSavePetInfo = (Save_PetItem_Info *)((LPBYTE)aRecv + lOfs);

}

void CDataServerProtocol::DGOptionDataRecv(int aIndex, SDHP_SKILLKEYDATA * aRecv)
{
	char szName[11]={0};
	szName[10] = 0;
	std::memcpy(szName, aRecv->Name, 11);

	char szTemp[100];
	wsprintf(szTemp, "UPDATE OptionData SET SkillKey=? where Name='%s'", szName);

}

void CDataServerProtocol::DGMoveOtherServer(int aIndex, SDHP_CHARACTER_TRANSFER * aRecv)
{
	sLog.outError("[MuOnlineDB] Error Character Transfer is not longer avaible");
}

void CDataServerProtocol::GDDeleteTempUserInfo(int aIndex, SDHP_DELETE_TEMPUSERINFO * aRecv)
{
	SDHP_DELETE_TEMPUSERINFO_RESULT pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xCE;
	pMsg.h.size = sizeof(pMsg);

	std::memcpy(pMsg.CharName, aRecv->CharName, MAX_ACCOUNT_LEN+1);

	for(int i=0;i<g_dwMaxServerGroups;i++)
	{
		if(g_Server[i].m_Index != -1 
			&& g_Server[i].m_Type == g_Server[aIndex].m_Type
			&& g_Server[i].m_ServerCode == aRecv->ServerCode)
		{
			DataSend(i, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
			return;
		}
	}
}

void CDataServerProtocol::GS_DGAnsCastleInitData(int aIndex,CSP_REQ_CSINITDATA * aRecv)
{
	BYTE SendData[sizeof(CSP_ANS_CSINITDATA) + (sizeof(CSP_CSINITDATA) * MAX_CS_NPC)] = {0};

	CSP_ANS_CSINITDATA * lpMsg = (CSP_ANS_CSINITDATA *)SendData;
	CSP_CSINITDATA * lpMsgBody = (CSP_CSINITDATA *)&SendData[sizeof(CSP_ANS_CSINITDATA)];

	lpMsg->h.c = 0xC2;
	lpMsg->h.headcode = 0x81;
	lpMsg->wMapSvrNum = aRecv->wMapSvrNum;
	lpMsg->iCount = 0;
	lpMsg->iResult = 0;

	int iCount = 0;



	int size = sizeof(CSP_ANS_CSINITDATA) + (sizeof(CSP_CSINITDATA)*iCount);

	lpMsg->h.sizeH = SET_NUMBERH(size);
	lpMsg->h.sizeL = SET_NUMBERL(size);

	DataSend(aIndex,(PBYTE)SendData,size, __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsOwnerGuildMaster(int aIndex,CSP_REQ_OWNERGUILDMASTER * aRecv)
{
	CSP_ANS_OWNERGUILDMASTER pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x01;
	pMsg.iResult = 0;

	pMsg.wMapSvrNum = aRecv->wMapSvrNum;
	pMsg.iIndex = aRecv->iIndex;

	DataSend(aIndex,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsCastleNpcBuy(int aIndex,CSP_REQ_NPCBUY * aRecv)
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

	DataSend(aIndex,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsCastleNpcRepair(int aIndex,CSP_REQ_NPCREPAIR * aRecv)
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

	DataSend(aIndex,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsCastleNpcUpgrade(int aIndex,CSP_REQ_NPCUPGRADE * aRecv)
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

	DataSend(aIndex,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsTaxInfo(int aIndex,CSP_REQ_TAXINFO * aRecv)
{
	CSP_ANS_TAXINFO pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x06;
	pMsg.iResult = 0;

	pMsg.iIndex = aRecv->iIndex;
	pMsg.wMapSvrNum = aRecv->wMapSvrNum;

	DataSend(aIndex,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsTaxRateChange(int aIndex,CSP_REQ_TAXRATECHANGE * aRecv)
{
	CSP_ANS_TAXRATECHANGE pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x07;
	pMsg.iResult = 0;

	pMsg.iIndex = aRecv->iIndex;
	pMsg.wMapSvrNum = aRecv->wMapSvrNum;

	DataSend(aIndex,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsCastleMoneyChange(int aIndex,CSP_REQ_MONEYCHANGE * aRecv)
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

	DataSend(aIndex,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsSiegeDateChange(int aIndex,CSP_REQ_SDEDCHANGE * aRecv)
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

	DataSend(aIndex,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsGuildMarkRegInfo(int aIndex,CSP_REQ_GUILDREGINFO * aRecv)
{
	CSP_ANS_GUILDREGINFO pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x0A;
	pMsg.iResult = 0;

	pMsg.iIndex = aRecv->iIndex;
	pMsg.wMapSvrNum = aRecv->wMapSvrNum;

	char szGuildName[9] = {0};
	std::memcpy(szGuildName,aRecv->szGuildName,8);


	DataSend(aIndex,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsSiegeEndedChange(int aIndex,CSP_REQ_SIEGEENDCHANGE * aRecv)
{
	CSP_ANS_SIEGEENDCHANGE pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x0B;
	pMsg.iResult = 0;

	pMsg.wMapSvrNum = aRecv->wMapSvrNum;
	pMsg.bIsSiegeEnded = aRecv->bIsSiegeEnded;

	DataSend(aIndex,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsCastleOwnerChange(int aIndex,CSP_REQ_CASTLEOWNERCHANGE * aRecv)
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
	std::memcpy(szOwnerGuildName,aRecv->szOwnerGuildName,0x08);
	std::memcpy(pMsg.szOwnerGuildName,aRecv->szOwnerGuildName,0x08);

	DataSend(aIndex,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsRegAttackGuild(int aIndex,CSP_REQ_REGATTACKGUILD * aRecv)
{
	CSP_ANS_REGATTACKGUILD pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x0D;
	pMsg.iResult = 0;

	pMsg.iIndex = aRecv->iIndex;
	pMsg.wMapSvrNum = aRecv->wMapSvrNum;

	char szEnemyGuildName[9] = {0};
	szEnemyGuildName[8] = 0;
	std::memcpy(szEnemyGuildName,aRecv->szEnemyGuildName,0x08);
	std::memcpy(pMsg.szEnemyGuildName,aRecv->szEnemyGuildName,0x08);

	DataSend(aIndex,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsRestartCastleState(int aIndex,CSP_REQ_CASTLESIEGEEND * aRecv)
{
	CSP_ANS_CASTLESIEGEEND pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x0E;
	pMsg.iResult = 0;

	pMsg.wMapSvrNum = aRecv->wMapSvrNum;

	DataSend(aIndex,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsMapSvrMsgMultiCast(int aIndex,CSP_REQ_MAPSVRMULTICAST * aRecv)
{
	CSP_ANS_MAPSVRMULTICAST pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x0F;

	pMsg.wMapSvrNum = aRecv->wMapSvrNum;

	std::memcpy(pMsg.szMsgText,aRecv->szMsgText,0x80);

	for(int i=0;i<g_dwMaxServerGroups;i++)
	{
		if(g_Server[i].m_Index != -1 
			&& g_Server[i].m_Type == g_Server[aIndex].m_Type
			&& g_Server[i].m_ServerGroup == aRecv->wMapSvrNum)
		{
			DataSend(i,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
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

	std::memcpy(pMsg.szSender, aRecv->szSender, sizeof(pMsg.szSender));
	std::memcpy(pMsg.szMessage, aRecv->szMessage, sizeof(pMsg.szMessage));
	std::memcpy(pMsg.szServerName, aRecv->szServerName, sizeof(pMsg.szServerName));
	std::memcpy(pMsg.btColorRGB, aRecv->btColorRGB, sizeof(pMsg.btColorRGB));

	for (int i = 0; i < g_dwMaxServerGroups; i++)
	{
		if (g_Server[i].m_Index != -1
			&& g_Server[i].m_Type == g_Server[aIndex].m_Type
			&& g_Server[i].m_ServerGroup == aRecv->wMapSvrNum)
		{
			DataSend(i, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
		}
	}
}

void CDataServerProtocol::GS_DGAnsRegGuildMark(int aIndex,CSP_REQ_GUILDREGMARK * aRecv)
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

	char szGuildName[9] = {0};
	szGuildName[8] = 0;

	std::memcpy(szGuildName,aRecv->szGuildName,0x08);
	std::memcpy(pMsg.szGuildName,aRecv->szGuildName,0x08);

	DataSend(aIndex,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsGuildMarkReset(int aIndex,CSP_REQ_GUILDRESETMARK * aRecv)
{
	CSP_ANS_GUILDRESETMARK pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x11;
	pMsg.iResult = 0;

	pMsg.iIndex = aRecv->iIndex;
	pMsg.wMapSvrNum = aRecv->wMapSvrNum;

	char szGuildName[9] = {0};
	szGuildName[8] = 0;

	std::memcpy(szGuildName,aRecv->szGuildName,0x08);
	std::memcpy(pMsg.szGuildName,aRecv->szGuildName,0x08);

	DataSend(aIndex,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsGuildSetGiveUp(int aIndex,CSP_REQ_GUILDSETGIVEUP * aRecv)
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

	char szGuildName[9] = {0};
	szGuildName[8] = 0;

	std::memcpy(szGuildName,aRecv->szGuildName,0x08);
	std::memcpy(pMsg.szGuildName,aRecv->szGuildName,0x08);

	DataSend(aIndex,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsNpcRemove(int aIndex,CSP_REQ_NPCREMOVE * aRecv)
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

	DataSend(aIndex,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsCastleStateSync(int aIndex,CSP_REQ_CASTLESTATESYNC * aRecv)
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

	std::memcpy(pMsg.szOwnerGuildName,aRecv->szOwnerGuildName,0x08);

	for(int i=0;i<g_dwMaxServerGroups;i++)
	{
		if(g_Server[i].m_Index != -1 
			&& g_Server[i].m_Type == g_Server[aIndex].m_Type
			&& g_Server[i].m_ServerGroup == aRecv->wMapSvrNum )
		{
			DataSend(i,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
		}
	}
}

void CDataServerProtocol::GS_DGAnsCastleTributeMoney(int aIndex,CSP_REQ_CASTLETRIBUTEMONEY * aRecv)
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

	DataSend(aIndex,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsResetCastleTaxInfo(int aIndex,CSP_REQ_RESETCASTLETAXINFO * aRecv)
{
	CSP_ANS_RESETCASTLETAXINFO pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x19;
	pMsg.iResult = 0;

	pMsg.wMapSvrNum = aRecv->wMapSvrNum;

	DataSend(aIndex,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsResetSiegeGuildInfo(int aIndex,CSP_REQ_RESETSIEGEGUILDINFO * aRecv)
{
	CSP_ANS_RESETSIEGEGUILDINFO pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x1A;
	pMsg.iResult = 0;

	pMsg.wMapSvrNum = aRecv->wMapSvrNum;

	DataSend(aIndex,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsResetRegSiegeInfo(int aIndex,CSP_REQ_RESETREGSIEGEINFO * aRecv)
{
	CSP_ANS_RESETREGSIEGEINFO pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x80;
	pMsg.h.subcode = 0x1B;
	pMsg.iResult = 0;

	pMsg.wMapSvrNum = aRecv->wMapSvrNum;

	DataSend(aIndex,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsAllGuildMarkRegInfo(int aIndex,CSP_REQ_ALLGUILDREGINFO * aRecv)
{
	BYTE SendData[sizeof(CSP_ANS_ALLGUILDREGINFO) + sizeof(CSP_GUILDREGINFO) * MAX_CS_GUILDLIST] = {0};

	CSP_ANS_ALLGUILDREGINFO * lpMsg = (CSP_ANS_ALLGUILDREGINFO *)SendData;
	CSP_GUILDREGINFO * lpMsgBody = (CSP_GUILDREGINFO *)&SendData[sizeof(CSP_ANS_ALLGUILDREGINFO)];

	lpMsg->h.c = 0xC2;
	lpMsg->h.headcode = 0x83;
	lpMsg->iResult = TRUE;

	lpMsg->iIndex = aRecv->iIndex;
	lpMsg->wMapSvrNum = aRecv->wMapSvrNum;
	lpMsg->iCount = 0;
	int iCount = 0;

	lpMsg->iCount = iCount;
	int size = sizeof(CSP_ANS_ALLGUILDREGINFO) + sizeof(CSP_GUILDREGINFO) * lpMsg->iCount;

	lpMsg->h.sizeH = SET_NUMBERH(size);
	lpMsg->h.sizeL = SET_NUMBERL(size);


	DataSend(aIndex,(PBYTE)SendData,size, __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsFirstCreateNPC(int aIndex,CSP_REQ_NPCSAVEDATA * aRecv)
{
	CSP_ANS_NPCSAVEDATA pMsg;

	CSP_NPCSAVEDATA * lpMsgBody = (CSP_NPCSAVEDATA *)((PBYTE)aRecv + sizeof(CSP_REQ_NPCSAVEDATA));

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x84;
	pMsg.iResult = 0;

	pMsg.wMapSvrNum = aRecv->wMapSvrNum;


	DataSend(aIndex,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsCalcRegGuildList(int aIndex,CSP_REQ_CALCREGGUILDLIST * aRecv)
{
	BYTE SendData[sizeof(CSP_ANS_CALCREGGUILDLIST) + sizeof(CSP_CALCREGGUILDLIST) * MAX_CS_GUILDLIST] = {0};
	
	CSP_ANS_CALCREGGUILDLIST * lpMsg = (CSP_ANS_CALCREGGUILDLIST *)SendData;
	CSP_CALCREGGUILDLIST * lpMsgBody = (CSP_CALCREGGUILDLIST *)&SendData[sizeof(CSP_ANS_CALCREGGUILDLIST)];

	lpMsg->h.c = 0xC2;
	lpMsg->h.headcode = 0x85;
	lpMsg->wMapSvrNum = aRecv->wMapSvrNum;
	lpMsg->iResult = 0;

	int iCount = 0;


	int size = sizeof(CSP_ANS_CALCREGGUILDLIST) + sizeof(CSP_CALCREGGUILDLIST) * iCount;

	lpMsg->iCount = iCount;
	lpMsg->h.sizeH = SET_NUMBERH(size);
	lpMsg->h.sizeL = SET_NUMBERL(size);


	DataSend(aIndex,(PBYTE)SendData,size, __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsCsGulidUnionInfo(int aIndex,CSP_REQ_CSGUILDUNIONINFO * aRecv)
{
	BYTE SendData[sizeof(CSP_ANS_CSGUILDUNIONINFO) + sizeof(CSP_CSGUILDUNIONINFO) * MAX_CS_GUILDLIST * 3] = {0};

	CSP_CSGUILDUNIONINFO * aRecvBody = (CSP_CSGUILDUNIONINFO*)((PBYTE)aRecv + sizeof(CSP_REQ_CSGUILDUNIONINFO));

	CSP_ANS_CSGUILDUNIONINFO * lpMsg = (CSP_ANS_CSGUILDUNIONINFO*)SendData;
	CSP_CSGUILDUNIONINFO * lpMsgBody = (CSP_CSGUILDUNIONINFO*)&SendData[sizeof(CSP_ANS_CSGUILDUNIONINFO)];

	lpMsg->h.c = 0xC2;
	lpMsg->h.headcode = 0x86;
	lpMsg->iResult = TRUE;
	lpMsg->wMapSvrNum = aRecv->wMapSvrNum;

	int iCount = 0;


	int size = sizeof(CSP_ANS_CSGUILDUNIONINFO) + sizeof(CSP_CSGUILDUNIONINFO) * iCount;

	lpMsg->iCount = iCount;

	lpMsg->h.sizeH = SET_NUMBERH(size);
	lpMsg->h.sizeL = SET_NUMBERL(size);

	DataSend(aIndex,(PBYTE)SendData,size, __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsCsSaveTotalGuildInfo(int aIndex,CSP_REQ_CSSAVETOTALGUILDINFO * aRecv)
{
	CSP_ANS_CSSAVETOTALGUILDINFO pMsg;
	CSP_CSSAVETOTALGUILDINFO * lpMsgBody = (CSP_CSSAVETOTALGUILDINFO*)((PBYTE)aRecv+sizeof(CSP_REQ_CSSAVETOTALGUILDINFO));

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x87;

	pMsg.iResult = TRUE;
	pMsg.wMapSvrNum = aRecv->wMapSvrNum;


	DataSend(aIndex,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsCsLoadTotalGuildInfo(int aIndex,CSP_REQ_CSLOADTOTALGUILDINFO * aRecv)
{
	BYTE SendData[sizeof(CSP_ANS_CSLOADTOTALGUILDINFO) + sizeof(CSP_CSLOADTOTALGUILDINFO) * MAX_CS_GUILDLIST] = {0};

	CSP_ANS_CSLOADTOTALGUILDINFO * lpMsg = (CSP_ANS_CSLOADTOTALGUILDINFO*)SendData;
	CSP_CSLOADTOTALGUILDINFO * lpMsgBody = (CSP_CSLOADTOTALGUILDINFO*)&SendData[sizeof(CSP_ANS_CSLOADTOTALGUILDINFO)];

	lpMsg->h.c = 0xC2;
	lpMsg->h.headcode = 0x88;
	lpMsg->wMapSvrNum = aRecv->wMapSvrNum;
	lpMsg->iResult = FALSE;

	int iCount = 0;


	lpMsg->iCount = iCount;

	int size = sizeof(CSP_ANS_CSLOADTOTALGUILDINFO) + sizeof(CSP_CSLOADTOTALGUILDINFO) * iCount;

	lpMsg->h.sizeH = SET_NUMBERH(size);
	lpMsg->h.sizeL = SET_NUMBERL(size);


	DataSend(aIndex,(PBYTE)SendData,size, __FUNCTION__);
}

void CDataServerProtocol::GS_DGAnsCastleNpcUpdate(int aIndex,CSP_REQ_NPCUPDATEDATA * aRecv)
{
	CSP_ANS_NPCUPDATEDATA pMsg;

	CSP_NPCUPDATEDATA * lpMsgBody = (CSP_NPCUPDATEDATA *)((PBYTE)aRecv + sizeof(CSP_REQ_NPCUPDATEDATA));

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0x89;
	pMsg.iResult = FALSE;
	pMsg.wMapSvrNum = aRecv->wMapSvrNum;


	DataSend(aIndex,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::DGAnsCrywolfSync(int aIndex,CWP_REQ_CRYWOLFSYNC * aRecv)
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

	for(int i=0;i<g_dwMaxServerGroups;i++)
	{
		if(g_Server[i].m_Index != -1 
			&& g_Server[i].m_Type == g_Server[aIndex].m_Type
			&& g_Server[i].m_ServerGroup == aRecv->wMapSvrNum)
		{
			DataSend(i,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
		}
	}
}

void CDataServerProtocol::DGAnsCrywolfInfoLoad(int aIndex,CWP_REQ_CRYWOLFINFOLOAD * aRecv)
{
	CWP_ANS_CRYWOLFINFOLOAD pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xB1;
	pMsg.btResult = FALSE;


	DataSend(aIndex,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::DGAnsCrywolfInfoSave(int aIndex,CWP_REQ_CRYWOLFINFOSAVE * aRecv)
{
	CWP_ANS_CRYWOLFINFOSAVE pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xB2;
	pMsg.btResult = FALSE;

	DataSend(aIndex,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::DGAnsPeriodItemExInsert(int aIndex,PMSG_REQ_PERIODITEMEX_INSERT * aRecv)
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

	if(aRecv->Serial == 0)
		pMsg.Serial = g_ItemSerial.GenerateSerial();
	else
		pMsg.Serial = aRecv->Serial;

	pMsg.dwDuration = aRecv->dwDuration;
	pMsg.lBuyDate = aRecv->lBuyDate;
	pMsg.lExpireDate = aRecv->lExpireDate;
	pMsg.btResultCode = 1;


	DataSend(aIndex,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::DGAnsPeriodItemExDelete(int aIndex,PMSG_REQ_PERIODITEMEX_DELETE * aRecv)
{
	PMSG_ANS_PERIODITEMEX_DELETE pMsg;

	pMsg.head.c = 0xC1;
	pMsg.head.headcode = 0xD0;
	pMsg.head.subcode = 0x02;
	pMsg.head.size = sizeof(pMsg);

	pMsg.dwUserGuid = aRecv->dwUserGuid;
	pMsg.wUserIndex = aRecv->wUserIndex;
	pMsg.btResultCode = 1;


	DataSend(aIndex,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::DGAnsPeriodItemExSelect(int aIndex, PMSG_REQ_PERIODITEMEX_SELECT * aRecv)
{
	char szName[11]={0};
	szName[10] = 0;
	std::memcpy(szName, aRecv->chCharacterName, MAX_ACCOUNT_LEN);

	int PacketSize = 0;

	PMSG_ANS_PERIODITEMEX_LIST_COUNT pListCount;
	PMSG_ANS_PERIODITEMEX_LIST pList;
	PMSG_ANS_PERIODITEMEX_EXPIRED_ITEMLIST pExpiredList;

	pList.head.set((LPBYTE)&pList, 0xD0, 0x05, sizeof(PMSG_ANS_PERIODITEMEX_LIST));
	pListCount.head.set((LPBYTE)&pListCount, 0xD0, 0x04, sizeof(PMSG_ANS_PERIODITEMEX_LIST_COUNT));
	pExpiredList.head.set((LPBYTE)&pExpiredList, 0xD0, 0x03, sizeof(PMSG_ANS_PERIODITEMEX_EXPIRED_ITEMLIST));

	pExpiredList.btExpiredItemCount = 0;


	pExpiredList.wUserIndex = aRecv->wUserIndex;

	DataSend(aIndex, (LPBYTE)&pExpiredList, sizeof(pExpiredList), __FUNCTION__);


	
	pListCount.btListCount = 0;
	pListCount.dwUserGuid = aRecv->dwUserGuid;
	pListCount.wUserIndex = aRecv->wUserIndex;


	int itemid = 0;
	time_t currtime = time(NULL);


	DataSend(aIndex, (LPBYTE)&pListCount, sizeof(pListCount), __FUNCTION__);
}

void CDataServerProtocol::ReqInGameShopItemList(short aIndex, ISHOP_REQ_ITEMLIST *aRecv)
{
	char Buff[65535] = {0};
	int PacketSize = 0;
	ISHOP_ANS_ITEMLIST * lpMsg = (ISHOP_ANS_ITEMLIST *)(Buff);
	ISHOP_ITEMLIST * lpMsgBody = (ISHOP_ITEMLIST *)(Buff + sizeof(ISHOP_ANS_ITEMLIST));



	lpMsg->Count = 0;


	lpMsg->Result = 1;

	PacketSize = (sizeof(ISHOP_ANS_ITEMLIST) + (lpMsg->Count * sizeof(ISHOP_ITEMLIST)));



	lpMsg->aIndex = aRecv->aIndex;
	lpMsg->InvNum = aRecv->InvNum;
	lpMsg->InvType = aRecv->InvType;
	std::memcpy(lpMsg->AccountID, aRecv->AccountID, 11);

	lpMsg->h.c = 0xC2;
	lpMsg->h.headcode = 0xD2;
	lpMsg->h.subcode = 0x02;
	lpMsg->h.sizeH = HIBYTE(PacketSize);
	lpMsg->h.sizeL = LOBYTE(PacketSize);

	DataSend(aIndex, (LPBYTE)&Buff, PacketSize, __FUNCTION__);
}

void CDataServerProtocol::ReqInGameShopCharacterCardBuy(short aIndex, ISHOP_CHARCARD_BUY *aRecv)
{
	if(aRecv->Type == 0)
	{

		
	}
	else if(aRecv->Type == 1)
	{

	}

}

void CDataServerProtocol::ReqInGameShopItemBuy(short aIndex, ISHOP_ITEM_BUY *aRecv)
{
	ISHOP_ITEM_BUYANS pResult;
	PHeadSetB((LPBYTE)&pResult, 0xD5, sizeof(pResult));

	std::memcpy(&pResult.AccountID, aRecv->AccountID, 11);
	pResult.aIndex = aRecv->aIndex;


}

void CDataServerProtocol::ReqInGameShopItemGift(short aIndex, ISHOP_ITEM_GIFT *aRecv)
{
	ISHOP_ITEM_GIFTANS pResult;
	PHeadSetB((LPBYTE)&pResult, 0xD6, sizeof(pResult));

	std::memcpy(&pResult.AccountID, aRecv->AccountID, 11);
	pResult.aIndex = aRecv->aIndex;

	DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size, __FUNCTION__);
}

void CDataServerProtocol::ReqInGameShopItemUse(short aIndex, ISHOP_ITEM_USE *aRecv)
{
	ISHOP_ITEM_USEANS pMsg = {0};
	PHeadSetB((LPBYTE)&pMsg, 0xD9, sizeof(pMsg));

	pMsg.aIndex = aRecv->aIndex;
	pMsg.UniqueCode = aRecv->UniqueCode;
	pMsg.AuthCode = aRecv->AuthCode;


	std::memcpy(pMsg.AccountID, aRecv->AccountID, 11);

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::ReqInGameShopPoint(short aIndex, ISHOP_REQ_POINT *aRecv)
{
	ISHOP_ANS_POINT pMsg = {0};

	pMsg.aIndex = aRecv->aIndex;
	std::memcpy(pMsg.AccountID, aRecv->AccountID, 11);

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xD1;
	pMsg.h.size = sizeof(pMsg);

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::ReqInGameShopAddPoint(short aIndex, ISHOP_POINT_ADD *aRecv)
{


}

void CDataServerProtocol::ReqInGameShopPackageBuy(short aIndex, LPBYTE aRecv)
{
	ISHOP_ITEM_BUY_PACKAGE * lpMsg = (ISHOP_ITEM_BUY_PACKAGE *)(aRecv);
	ISHOP_ITEM_PACKAGE * lpItem = (ISHOP_ITEM_PACKAGE *)(aRecv + sizeof(ISHOP_ITEM_BUY_PACKAGE));

	ISHOP_ITEM_BUYANS pResult;
	PHeadSetB((LPBYTE)&pResult, 0xD5, sizeof(pResult));

	std::memcpy(&pResult.AccountID, lpMsg->AccountID, 11);
	pResult.aIndex = lpMsg->aIndex;

	DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size, __FUNCTION__);
}

void CDataServerProtocol::ReqInGameShopPackageGift(short aIndex, LPBYTE aRecv)
{
	ISHOP_ITEM_GIFT_PACKAGE * lpMsg = (ISHOP_ITEM_GIFT_PACKAGE *)(aRecv);
	ISHOP_ITEM_PACKAGE * lpItem = (ISHOP_ITEM_PACKAGE *)(aRecv + sizeof(ISHOP_ITEM_GIFT_PACKAGE));

	ISHOP_ITEM_GIFTANS pResult;
	PHeadSetB((LPBYTE)&pResult, 0xD6, sizeof(pResult));

	std::memcpy(&pResult.AccountID, lpMsg->AccountID, 11);
	pResult.aIndex = lpMsg->aIndex;


	DataSend(aIndex, (LPBYTE)&pResult, pResult.h.size, __FUNCTION__);
}

void CDataServerProtocol::ReqInGameShopItemDelete(short aIndex, ISHOP_ITEM_DELETE * aRecv)
{


}

void CDataServerProtocol::ReqInGameShopItemRollbackUse(short aIndex, ISHOP_ITEM_ROLLBACK * aRecv)
{

}

void CDataServerProtocol::ReqLuckyCoinInfo(int aIndex,PMSG_REQ_LUCKYCOIN * lpMsg)
{
	int test = false;
	PMSG_ANS_LUCKYCOIN pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x19;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iIndex= lpMsg->iIndex;
	std::memcpy(pMsg.szUID,lpMsg->szUID, sizeof(pMsg.szUID));
	pMsg.szUID[10] = 0;


	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::ReqRegLuckyCoin(int aIndex,PMSG_REQ_REGISTER_LUCKYCOIN * lpMsg)
{
	PMSG_ANS_REG_LUCKYCOIN pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x18;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iIndex= lpMsg->iIndex;
	pMsg.Pos = lpMsg->btPos;

	strcpy(pMsg.szUID,lpMsg->szAccountID);
	pMsg.szUID[10] = 0x00;
	pMsg.Result = FALSE;

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::DevilSqureScore(PMSG_ANS_EVENTUSERSCORE * lpMsg)
{
	char szAccount[11] = {0};
	char szName[11] = {0};
	szAccount[10] = 0;
	szName[10] = 0;
	std::memcpy(szAccount,lpMsg->AccountID,10);
	std::memcpy(szName,lpMsg->GameID,10);

}

void CDataServerProtocol::GDReqBloodCastleEnterCount(int aIndex,PMSG_REQ_BLOODCASTLE_ENTERCOUNT * lpMsg)
{
	PMSG_ANS_BLOODCASTLE_ENTERCOUNT pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBD;
	pMsg.h.subcode = 0x02;
	pMsg.h.size = sizeof(pMsg);

	std::memcpy(pMsg.AccountID,lpMsg->AccountID,sizeof(pMsg.AccountID));
	std::memcpy(pMsg.GameID,lpMsg->GameID,sizeof(pMsg.GameID));

	pMsg.ServerCode = lpMsg->ServerCode;
	pMsg.iObjIndex = lpMsg->iObjIndex;

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::BloodCastleScore_5TH(PMSG_ANS_BLOODCASTLESCORE_5TH * lpMsg)
{
	char szAccount[11] = {0};
	char szName[11] = {0};
	szAccount[10] = 0;
	szName[10] = 0;

	std::memcpy(szAccount,lpMsg->AccountID,10);
	std::memcpy(szName,lpMsg->GameID,10);

	
}

void CDataServerProtocol::IllusionTempleScore(PMSG_ANS_ILLUSIONTEMPLE_RANKING * lpMsg)
{
	char szAccountID[11] = {0};
	char szName[11] = {0};
	szAccountID[10] = 0;
	szName[10] = 0;

	std::memcpy(szAccountID, lpMsg->AccountID, 10);
	std::memcpy(szName, lpMsg->GameID, 10);


}

void CDataServerProtocol::ChaosCastleScore(PMSG_ANS_CHAOSCASTLE_RANKING * lpMsg)
{
	char szAccountID[11] = {0};
	char szName[11] = {0};
	szAccountID[10] = 0;
	szName[10] = 0;

	std::memcpy(szAccountID, lpMsg->AccountID, 10);
	std::memcpy(szName, lpMsg->GameID, 10);

}

void CDataServerProtocol::EGAnsEventChipInfo(int aIndex,PMSG_REQ_VIEW_EC_MN * lpMsg)
{
	int test = false;
	PMSG_ANS_VIEW_EC_MN pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x01;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	std::memcpy(pMsg.szUID,lpMsg->szUID,sizeof(pMsg.szUID));
	pMsg.szUID[10] = 0;


	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::EGAnsRegEventChipInfo(int aIndex,PMSG_REQ_REGISTER_EVENTCHIP * lpMsg)
{
	PMSG_ANS_REGISTER_EVENTCHIP pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x02;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	pMsg.Pos = lpMsg->Pos;

	strcpy(pMsg.szUID,lpMsg->szUID);
	pMsg.szUID[10] = 0x00;
	pMsg.bSUCCESS = FALSE;


	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::EGAnsResetEventChip(int aIndex,PMSG_REQ_RESET_EVENTCHIP * lpMsg)
{
	PMSG_ANS_RESET_EVENTCHIP pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x04;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	strcpy(pMsg.szUID,lpMsg->szUID);
	pMsg.szUID[10] = 0;


	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::EGAnsEventStoneInfo(int aIndex,PMSG_REQ_VIEW_EC_MN * lpMsg)
{
	PMSG_ANS_VIEW_STONES pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x05;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	strcpy(pMsg.szUID,lpMsg->szUID);
	pMsg.szUID[10] = 0x00;


	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::EGAnsRegEventStoneInfo(int aIndex,PMSG_REQ_REGISTER_STONES * lpMsg)
{
	PMSG_ANS_REGISTER_STONES pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x06;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	pMsg.iPosition = lpMsg->iPosition;
	strcpy(pMsg.szUID,lpMsg->szUID);
	pMsg.szUID[10] = 0x00;


	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::EGAnsDeleteStones(int aIndex,PMSG_REQ_DELETE_STONES * lpMsg)
{
	PMSG_ANS_DELETE_STONES pMsg = {0};

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x07;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	strcpy(pMsg.szUID,lpMsg->szUID);
	pMsg.szUID[10] = 0x00;

	pMsg.bSUCCESS = FALSE;


	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::EGAns2AnivRegSerial(int aIndex,PMSG_REQ_2ANIV_SERIAL * lpMsg)
{
	PMSG_ANS_2ANIV_SERIAL pMsg = {0};

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x08;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	char szName[11] = {0};
	szName[10] = 0;
	std::memcpy(szName, lpMsg->szUID, 10);
	pMsg.iINDEX = lpMsg->iINDEX;
	std::memcpy(pMsg.szUID, szName, 11);

	if (SpaceSyntexCheck(lpMsg->SERIAL1) == FALSE || QuoteSpaceSyntexCheck(lpMsg->SERIAL1) == FALSE || PercentSyntaxCheck(lpMsg->SERIAL1) == FALSE)
	{
		pMsg.btIsRegistered = 4;
		DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
		return;
	}

	if (SpaceSyntexCheck(lpMsg->SERIAL2) == FALSE || QuoteSpaceSyntexCheck(lpMsg->SERIAL2) == FALSE || PercentSyntaxCheck(lpMsg->SERIAL2) == FALSE)
	{
		pMsg.btIsRegistered = 4;
		DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
		return;
	}

	if (SpaceSyntexCheck(lpMsg->SERIAL3) == FALSE || QuoteSpaceSyntexCheck(lpMsg->SERIAL3) == FALSE || PercentSyntaxCheck(lpMsg->SERIAL3) == FALSE)
	{
		pMsg.btIsRegistered = 4;
		DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
		return;
	}


	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::EGAnsResetStoneInfo(int aIndex,PMSG_REQ_RESET_EVENTCHIP * lpMsg)
{
	PMSG_ANS_RESET_EVENTCHIP pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x09;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	char szName[11] = {0};
	szName[10] = 0;
	std::memcpy(szName, lpMsg->szUID, 10);
	pMsg.iINDEX = lpMsg->iINDEX;
	std::memcpy(pMsg.szUID, szName, 11);
	pMsg.szUID[10] = 0;


	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::EGAnsRegCCOfflineGift(int aIndex,PMSG_REQ_REG_CC_OFFLINE_GIFT * lpMsg)
{
	PMSG_ANS_REG_CC_OFFLINE_GIFT pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x15;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	pMsg.wServerCode = lpMsg->wServerCode;
	strcpy(pMsg.szUID,lpMsg->szUID);
	strcpy(pMsg.szNAME,lpMsg->szNAME);
	pMsg.szUID[10] = 0x00;
	pMsg.szNAME[10] = 0x00;
	
	pMsg.iResultCode = 0;


	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::EGAnsRegDLOfflineGift(int aIndex,PMSG_REQ_REG_DL_OFFLINE_GIFT * lpMsg)
{
	PMSG_ANS_REG_DL_OFFLINE_GIFT pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x16;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	pMsg.wServerCode = lpMsg->wServerCode;
	strcpy(pMsg.szUID,lpMsg->szUID);
	strcpy(pMsg.szNAME,lpMsg->szNAME);
	pMsg.szUID[10] = 0x00;
	pMsg.szNAME[10] = 0x00;
	
	pMsg.iResultCode = 0;


	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::EGAnsRegHTOfflineGift(int aIndex,PMSG_REQ_REG_HT_OFFLINE_GIFT * lpMsg)
{
	PMSG_ANS_REG_HT_OFFLINE_GIFT pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x17;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iINDEX = lpMsg->iINDEX;
	pMsg.wServerCode = lpMsg->wServerCode;
	strcpy(pMsg.szUID,lpMsg->szUID);
	strcpy(pMsg.szNAME,lpMsg->szNAME);
	pMsg.szUID[10] = 0x00;
	pMsg.szNAME[10] = 0x00;
	
	pMsg.iResultCode = 0;

	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::EGAnsLuckyCoinInfo(int aIndex,PMSG_REQ_LUCKYCOIN * lpMsg)
{
	int test = false;
	PMSG_ANS_LUCKYCOIN pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x19;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iIndex= lpMsg->iIndex;
	std::memcpy(pMsg.szUID,lpMsg->szUID, sizeof(pMsg.szUID));
	pMsg.szUID[10] = 0;


	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::EGAnsRegLuckyCoin(int aIndex,PMSG_REQ_REGISTER_LUCKYCOIN * lpMsg)
{
	PMSG_ANS_REG_LUCKYCOIN pMsg;

	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xBE;
	pMsg.h.subcode = 0x18;
	pMsg.h.size = sizeof(pMsg);

	pMsg.iIndex= lpMsg->iIndex;
	pMsg.Pos = lpMsg->btPos;

	strcpy(pMsg.szUID,lpMsg->szAccountID);
	pMsg.szUID[10] = 0x00;
	pMsg.Result = FALSE;



	DataSend(aIndex,(LPBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::ReqSetExpandedWarehouse(short aIndex, SDHP_EXPANDEDWAREHOUSE_SET * aRecv)
{
	if(aRecv->ExpansionType == 2)
	{

	}
	else if(aRecv->ExpansionType == 1)
	{

	}
}

void CDataServerProtocol::EGReqSantaCheck(short aIndex, PMSG_REQ_SANTACHECK * aRecv)
{
	PMSG_ANS_SANTACHECK pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xBE, 0x20, sizeof(pMsg));

	pMsg.aIndex = aRecv->aIndex;
	pMsg.gGameServerCode = aRecv->gGameServerCode;
	std::memcpy(pMsg.AccountID, aRecv->AccountID, 11);









	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::EGReqSantaGift(short aIndex, PMSG_REQ_SANTAGIFT * aRecv)
{
	PMSG_ANS_SANTAGIFT pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xBE, 0x21, sizeof(pMsg));

	pMsg.aIndex = aRecv->aIndex;
	pMsg.gGameServerCode = aRecv->gGameServerCode;
	std::memcpy(pMsg.AccountID, aRecv->AccountID, 11);









	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::ReqBanUser(short aIndex, BAN_REQ_USER * aRecv)
{
	if(aRecv->Type == 1)
	{


	}
	else
	{


	}
}

void CDataServerProtocol::ReqSecLock(short aIndex, SECLOCK_REQ_SAVE * aRecv)
{
	char szAccountID[11];
	szAccountID[10]=0;
	std::memcpy(szAccountID, aRecv->AccountID, 10);



}

void CDataServerProtocol::ReqSaveMonsterCount(short aIndex,DS_SAVE_MONSTERCNT * aRecv)
{


}

void CDataServerProtocol::ReqSavePlayerKiller(short aIndex,DS_SAVE_PLAYERKILLER * aRecv)
{


}

void CDataServerProtocol::GDReqArcaBattleGuildJoin(int aIndex, PMSG_REQ_ARCA_BATTLE_GUILD_JOIN_DS * aRecv)
{
	PMSG_ANS_ARCA_BATTLE_GUILD_JOIN_DS pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x31, sizeof(pMsg));





	pMsg.wNumber = aRecv->wNumber;


	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqArcaBattleGuildMemberJoin(int aIndex, PMSG_REQ_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS *aRecv)
{
	PMSG_ANS_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x33, sizeof(pMsg));





	pMsg.wNumber = aRecv->wNumber;


	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqArcaBattleEnter(int aIndex, PMSG_REQ_ARCA_BATTLE_ENTER_DS *aRecv)
{
	PMSG_ANS_ARCA_BATTLE_ENTER_DS pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x35, sizeof(pMsg));





	pMsg.wNumber = aRecv->wNumber;
	pMsg.btEnterSeq = aRecv->btEnterSeq;


	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqArcaBattleWinGuildInfoInsert(int aIndex, PMSG_REQ_AB_WIN_GUILD_INFO_INSERT_DS *aRecv)
{
	PMSG_ANS_AB_WIN_GUILD_INFO_DS pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x3A, sizeof(pMsg));


	for (int i = 0; i < aRecv->btGuildCnt; i++)
	{
		pMsg.m_stABWinGuildInfoDS[i].dwGuild = aRecv->m_stABWinGuildInfoDS[i].dwGuild;
		pMsg.m_stABWinGuildInfoDS[i].wObeliskGroup = aRecv->m_stABWinGuildInfoDS[i].wObeliskGroup;
		pMsg.m_stABWinGuildInfoDS[i].wOccupyObelisk = aRecv->m_stABWinGuildInfoDS[i].wOccupyObelisk;
		std::memcpy(pMsg.m_stABWinGuildInfoDS[i].szGuildName, aRecv->m_stABWinGuildInfoDS[i].szGuildName, MAX_GUILD_LEN+1);
	}

	pMsg.btGuildCnt = aRecv->btGuildCnt;

	for(int i=0;i<g_dwMaxServerGroups;i++)
	{
		if(g_Server[i].m_Index != -1 
			&& g_Server[i].m_Type == g_Server[aIndex].m_Type
			&& g_Server[i].m_ServerGroup == aRecv->wMapSvrNum)
		{
			DataSend(i,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
		}
	}
}

void CDataServerProtocol::GDReqArcaBattleWinGuildInfo(int aIndex, PMSG_REQ_AB_WIN_GUILD_INFO_DS *aRecv)
{
	PMSG_ANS_AB_WIN_GUILD_INFO_DS pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x3A, sizeof(pMsg));
	pMsg.btGuildCnt = 0;



	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqDeleteArcaBattleInfo(int aIndex)
{



}

void CDataServerProtocol::GDReqArcaBattleProcMultiCast(int aIndex, PMSG_REQ_AB_PROC_INSERT_DS *aRecv)
{
	PMSG_ANS_AB_PROC_STATE_DS pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x40, sizeof(pMsg));





	pMsg.btProcState = aRecv->btProcState;

	for(int i=0;i<g_dwMaxServerGroups;i++)
	{
		if(g_Server[i].m_Index != -1 
			&& g_Server[i].m_Type == g_Server[aIndex].m_Type
			&& g_Server[i].m_ServerGroup == aRecv->wMapSvrNum)
		{
			DataSend(i,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
		}
	}
}

void CDataServerProtocol::GDReqArcaBattleProcState(int aIndex, PMSG_REQ_AB_PROC_STATE_DS *aRecv)
{
	PMSG_ANS_AB_PROC_STATE_DS pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x40, sizeof(pMsg));


	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqArcaBattleJoinMemberUnder(int aIndex, PMSG_REQ_AB_JOIN_MEMBER_UNDER_DS *aRecv)
{
	PMSG_ANS_AB_JOIN_MEMBER_UNDER_DS pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x42, sizeof(pMsg));
	pMsg.btGuildCnt = 0;


	int iGuildNamesCnt = 0;



	pMsg.btGuildCnt = iGuildNamesCnt;

	for(int i=0;i<g_dwMaxServerGroups;i++)
	{
		if(g_Server[i].m_Index != -1 
			&& g_Server[i].m_Type == g_Server[aIndex].m_Type
			&& g_Server[i].m_ServerGroup == aRecv->wMapSvrNum)
		{
			DataSend(i,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
		}
	}
}

void CDataServerProtocol::GDReqArcaBattleJoinMemberUnderReq(int aIndex, PMSG_REQ_AB_JOIN_CANCEL_DS *aRecv)
{
	PMSG_ANS_AB_JOIN_CANCEL_DS pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x44, sizeof(pMsg));
	pMsg.btGuildCnt = 0;


	int iGuildNamesCnt = 0;
	char szGuildNames[6][MAX_GUILD_LEN+1];


	for(int i = 0; i < iGuildNamesCnt; i++)
	{




		int iResult = 0; // stub
		if (iResult < 0)
		{
			std::memcpy(pMsg.CancelGuildNames[pMsg.btGuildCnt].szGuildNames, szGuildNames[i], MAX_GUILD_LEN+1);
			pMsg.btGuildCnt++;
		}


	}

	for(int i = 0; i < pMsg.btGuildCnt; i++)
	{



	}

	for(int i=0;i<g_dwMaxServerGroups;i++)
	{
		if(g_Server[i].m_Index != -1 
			&& g_Server[i].m_Type == g_Server[aIndex].m_Type
			&& g_Server[i].m_ServerGroup == aRecv->wMapSvrNum)
		{
			DataSend(i,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
		}
	}
}

void CDataServerProtocol::GDReqArcaBattleRegisteredMemberCnt(int aIndex, PMSG_REQ_AB_REG_MEMBER_CNT_DS *aRecv)
{
	PMSG_ANS_AB_REG_MEMBER_CNT_DS pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x46, sizeof(pMsg));





	pMsg.wNumber = aRecv->iIndex;


	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqRemoveAllGuildBuffMultiCast(int aIndex, PMSG_REQ_REMOVE_ALL_GUILD_BUFF_DS *aRecv)
{
	PMSG_ANS_REMOVE_ALL_GUILD_BUFF_DS pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x48, sizeof(pMsg));

	for(int i=0;i<g_dwMaxServerGroups;i++)
	{
		if(g_Server[i].m_Index != -1 
			&& g_Server[i].m_Type == g_Server[aIndex].m_Type
			&& g_Server[i].m_ServerGroup == aRecv->wMapSvrNum)
		{
			DataSend(i,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
		}
	}
}

void CDataServerProtocol::GDReqArcaBattleMarkCnt(int aIndex, PMSG_REQ_ARCA_BATTLE_MARK_CNT_DS *aRecv)
{
	PMSG_ANS_ARCA_BATTLE_MARK_CNT_DS pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x4E, sizeof(pMsg));



	pMsg.wNumber = aRecv->wNumber;


	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqArcaBattleMarkReg(int aIndex, PMSG_REQ_ARCA_BATTLE_MARK_REG_DS *aRecv)
{
	PMSG_ANS_ARCA_BATTLE_MARK_REG_DS pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x50, sizeof(pMsg));






	int iResult = 0; // stub
	if (iResult == 0)
	{
		sLog.outBasic("[ArcaBattle] Error GDReqArcaBattleMarkReg() GuildName[%s] GuildNum[%d] GuildMaster[%s] MarkCnt[%d]",
			aRecv->szGuildName, aRecv->dwGuildNum, aRecv->szGuildMaster, aRecv->dwMarkCnt);

		pMsg.wNumber = aRecv->wNumber;
		DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);

		return;
	}
}

void CDataServerProtocol::GDReqArcaBattleMarkRank(int aIndex, PMSG_REQ_ARCA_BATTLE_MARK_RANK_DS *aRecv)
{
	PMSG_ANS_ARCA_BATTLE_MARK_RANK_DS pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x52, sizeof(pMsg));


	pMsg.btGuildCnt = 0;


	int iResult = 0; // stub
	if (iResult == 0 || pMsg.btGuildCnt == 0)
	{
		pMsg.btRank = 0;
		pMsg.dwMarkCnt = 0;
		pMsg.btGuildCnt = 0;
		pMsg.wNumber = aRecv->wNumber;

		DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
	}
	else
	{
		pMsg.btRank = iResult;

		pMsg.wNumber = aRecv->wNumber;

		DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
	}


}

void CDataServerProtocol::GDReqArcaBattleMarkRegDel(int aIndex, PMSG_REQ_ARCA_BATTLE_MARK_REG_DEL_DS *aRecv)
{



}

void CDataServerProtocol::GDReqArcaBattleIsTopRank(int aIndex, PMSG_REQ_ARCA_BATTLE_IS_TOP_RANK *aRecv)
{
	PMSG_ANS_ARCA_BATTLE_GUILD_JOIN_DS pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xF8, 0x2B, sizeof(pMsg));


	pMsg.wNumber = aRecv->wNumber;

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqArcaBattleMarkRegAllDel(int aIndex)
{



}

void CDataServerProtocol::GDReqArcaBattleAllGuildMarkCnt(int aIndex)
{
	PMSG_ANS_ALL_GUILD_MARK_CNT_DS pMsg;

	PHeadSubSetW((LPBYTE)&pMsg, 0xF8, 0xFC, sizeof(pMsg));


	pMsg.wGuildCnt = 0;


	DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GDReqArcaBattleMarkRegSet(int aIndex, PMSG_REQ_AB_MARK_REG_UPDATE_DS *aRecv)
{

}

void CDataServerProtocol::GDReqArcaBattleGuildRegInit(int aIndex, PMSG_REQ_GUILD_REG_INIT *aRecv)
{



}

void CDataServerProtocol::GDReqArcaBattleAllJoinUser(int aIndex, PMSG_REQ_AB_ALL_JOIN_USER_DS *aRecv)
{
	PMSG_ANS_AB_ALL_JOIN_USER_DS pMsg;

	PHeadSetW((LPBYTE)&pMsg, 0xE5, sizeof(pMsg));


	pMsg.btUserCnt = 0;


	DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GDReqPeriodBuffInsert(int aIndex, PMSG_REQ_PERIODBUFF_INSERT *aRecv)
{

}

void CDataServerProtocol::GDReqPeriodBuffDelete(int aIndex, PMSG_REQ_PERIODBUFF_DELETE *aRecv)
{

}

void CDataServerProtocol::GDReqPeriodBuffSelect(int aIndex, PMSG_REQ_PERIODBUFF_SELECT *aRecv)
{
	PMSG_ANS_PERIODBUFF_SELECT pMsg;

	PHeadSubSetB((LPBYTE)&pMsg, 0xE4, 0x03, sizeof(pMsg));
	std::memcpy(pMsg.szCharacterName, aRecv->szCharacterName, MAX_ACCOUNT_LEN+1);
	pMsg.btResultCode = 1;
	pMsg.wUserIndex = aRecv->wUserIndex;


	std::vector<int> vtDeleteBuffIndex;
	time_t currtime;

}

void CDataServerProtocol::GDReqQuestExpInfoLoad(int aIndex, PMSG_REQ_QUESTEXP_INFO *aRecv)
{


	_QUESTEXP_INFO QuestInfo[25];
	PMSG_ANS_QUESTEXP_INFO pMsg;

	BYTE Buff[4096];
	int PacketSize = 0;
	int iCnt = 0;
	INT64 nLen;

	pMsg.btQuestCnt = 0;
	pMsg.iUserIndex = aRecv->iUserIndex;

	int arrEp[20] = { 0 };



	char szTemp[4096];



	PacketSize = sizeof(pMsg) + (iCnt * sizeof(_QUESTEXP_INFO));

	pMsg.btQuestCnt = iCnt;

	pMsg.head.c = 0xC2;
	pMsg.head.headcode = 0xF6;
	pMsg.head.sizeH = HIBYTE(PacketSize);
	pMsg.head.sizeL = LOBYTE(PacketSize);

	std::memcpy(&Buff, &pMsg, sizeof(pMsg));
	std::memcpy(&Buff[sizeof(pMsg)], QuestInfo, sizeof(_QUESTEXP_INFO) * pMsg.btQuestCnt);

	DataSend(aIndex, Buff, PacketSize, __FUNCTION__);


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
	}
}

void CDataServerProtocol::GDReqLuckyItemInsert(int aIndex, PMSG_REQ_LUCKYITEM_INSERT *lpMsg)
{


}

void CDataServerProtocol::GDReqLuckyItemInsert2nd(int aIndex, PMSG_REQ_LUCKYITEM_INSERT_2ND *lpMsg)
{
	int iItemCnt = lpMsg->btItemCnt;

	if (iItemCnt <= 0)
	{
		return;
	}

}

void CDataServerProtocol::GDReqLuckyItemDelete(int aIndex, PMSG_REQ_LUCKYITEM_DELETE *lpMsg)
{



}

void CDataServerProtocol::GDReqLuckyItemSelect(int aIndex, PMSG_REQ_LUCKYITEM_SELECT *lpMsg)
{
	char BUFFER[2048];
	memset(BUFFER, 0x00, sizeof(BUFFER));

	int lOfs = sizeof(PMSG_ANS_LUCKYITEM_SELECT);

	PMSG_LUCKYITME_DB_INFO LuckyItemInfo[45] = { 0 };
	int iItemCount = 0;


	PMSG_ANS_LUCKYITEM_SELECT pMsg;
	pMsg.btItemCnt = iItemCount;
	pMsg.wUserIndex = lpMsg->wUserIndex;

	std::memcpy(&BUFFER[lOfs], LuckyItemInfo, pMsg.btItemCnt * sizeof(PMSG_LUCKYITME_DB_INFO));

	lOfs += (pMsg.btItemCnt * sizeof(PMSG_LUCKYITME_DB_INFO));

	pMsg.head.set((LPBYTE)&pMsg, 0xD4, lOfs);
	std::memcpy(BUFFER, &pMsg, sizeof(pMsg));

	DataSend(aIndex, (LPBYTE)&BUFFER, lOfs, __FUNCTION__);
}

void CDataServerProtocol::GDReqGetPentagramJewel(int aIndex, PMSG_REQ_PENTAGRAMJEWEL *lpMsg)
{
	BYTE Buffer[4096];
	memset(&Buffer, 0x00, sizeof(Buffer));

	int dwSize = sizeof(PMSG_ANS_PENTAGRAMJEWEL);
	int iCount = 0;

	PMSG_ANS_PENTAGRAMJEWEL pMsg;
	PENTAGRAMJEWEL_INFO m_PentagramJewelInfo;

	pMsg.btJewelCnt = iCount;
	pMsg.iAnsType = lpMsg->btJewelPos;
	pMsg.iUserIndex = lpMsg->iUserIndex;
	PHeadSetW((LPBYTE)&pMsg, 0xE0, dwSize);

	std::memcpy(&Buffer, &pMsg, sizeof(pMsg));

	DataSend(aIndex, Buffer, dwSize, __FUNCTION__);
}

void CDataServerProtocol::GDReqSetPentagramJewel(int aIndex, LPBYTE lpRecv)
{
	PMSG_REQ_SETPENTAGRAMJEWEL * lpMsg = (PMSG_REQ_SETPENTAGRAMJEWEL *)(lpRecv);

}

void CDataServerProtocol::GDReqDelPentagramJewel(int aIndex, PMSG_DEL_PENTAGRAMJEWEL *lpMsg)
{

}

void CDataServerProtocol::GDReqInsertPentagramJewel(int aIndex, PMSG_INSERT_PENTAGRAMJEWEL *lpMsg)
{

}

void CDataServerProtocol::GDReqChaosCastleFinalSave(int aIndex, PMSG_REQ_SAVE_CCF_RESULT* lpMsg)
{



}

void CDataServerProtocol::GDReqChaosCastleFinalPermission(int aIndex, PMSG_REQ_CCF_PERMISSION* lpMsg)
{
	SDHP_ANS_CCF_GETPERMISSION pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xF9, 0xA3, sizeof(pMsg));

	pMsg.nIndex = lpMsg->nIndex;

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqChaosCastleFinalLoad(int aIndex, SDHP_REQ_CCF_RANKING* lpMsg)
{
	PMSG_ANS_CCF_RANK pMsg;
	PHeadSetW((LPBYTE)&pMsg, 0xF5, sizeof(pMsg));


	DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GDReqChaosCastleFinalRenew(int aIndex, SDHP_RENEW_RANKING* lpMsg)
{



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
			DataSend(i, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
		}
	}
}

void CDataServerProtocol::GDReqLoadMuunInvenItem(int aIndex, SDHP_REQ_DBMUUN_INVEN_LOAD* lpMsg)
{
	SDHP_ANS_DBMUUN_INVEN_LOAD pMsg;
	PHeadSetW((LPBYTE)&pMsg, 0xF1, sizeof(pMsg));

	char szTemp[250];


	pMsg.aIndex = lpMsg->aIndex;


	DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GDReqSaveMuunInvenItem(int aIndex, SDHP_REQ_DBMUUN_INVEN_SAVE * aRecv)
{
	char szName[11] = { 0 };
	std::memcpy(szName, aRecv->Name, MAX_ACCOUNT_LEN); //
	szName[MAX_ACCOUNT_LEN] = 0; // crash fix

	char szTemp[256];
	wsprintf(szTemp, "UPDATE PetWarehouse SET Items=? WHERE Name='%s'", szName);


}

void CDataServerProtocol::GDReqLoadEventInvenItem(int aIndex, SDHP_REQ_DBEVENT_INVEN_LOAD * aRecv)
{
	SDHP_ANS_DBEVENT_INVEN_LOAD pMsg;
	PHeadSetW((LPBYTE)&pMsg, 0xE6, sizeof(pMsg));

	char szTemp[1024];


	pMsg.aIndex = aRecv->aIndex;


	DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GDReqSaveEventInvenItem(int aIndex, SDHP_REQ_DBEVENT_INVEN_SAVE * aRecv)
{
	char szTemp[1024];
	wsprintf(szTemp, "UPDATE T_Event_Inventory SET Inventory=? WHERE Name = '%s'", aRecv->Name);

}

void CDataServerProtocol::GDReqCardInfo(int aIndex, PMSG_REQ_MURUMMY_SELECT_DS * aRecv)
{
	PMSG_ANS_MURUMMY_SELECT_DS pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xE8, 0x00, sizeof(pMsg));

	int iCount = 0;



	{







		iCount++;
	}



	if (iCount <= 0)
	{
		pMsg.btResult = 0;
	}

	else
	{
		pMsg.btResult = 1;
	}

	pMsg.aIndex = aRecv->aIndex;
	
	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqCardInfoInsert(int aIndex, PMSG_REQ_MURUMMY_INSERT_DS * aRecv)
{



}

void CDataServerProtocol::GDReqScoreUpdate(int aIndex, PMSG_REQ_MURUMMY_SCORE_UPDATE_DS * aRecv)
{



}

void CDataServerProtocol::GDReqCardInfoUpdate(int aIndex, PMSG_REQ_MURUMMY_UPDATE_DS * aRecv)
{



}

void CDataServerProtocol::GDReqScoreDelete(int aIndex, PMSG_REQ_MURUMMY_DELETE_DS * aRecv)
{



}

void CDataServerProtocol::GDReqSlotInfoUpdate(int aIndex, PMSG_REQ_MURUMMY_SLOTUPDATE_DS * aRecv)
{
	if (aRecv->stCardUpdateDS.btSeq == 255 || aRecv->stCardUpdateDS.btSeq >= 8)
	{
		return;
	}




}

void CDataServerProtocol::GDReqMuRummyInfoUpdate(int aIndex, PMSG_REQ_MURUMMY_INFO_UPDATE_DS * aRecv)
{




}

void CDataServerProtocol::GDReqMuRummyDBLog(int aIndex, PMSG_REQ_MURUMMY_LOG_INSERT_DS * aRecv)
{



}

void CDataServerProtocol::GDReqMineModifyUPTUserInfo(int aIndex, SDHP_REQ_MINESYSTEM_UPT_USERINFO * aRecv)
{

	SDHP_ANS_MINESYSTEM_UPT_USERINFO pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0x4C, 0x00, sizeof(pMsg));

	pMsg.byRequestType = aRecv->byRequestType;
	pMsg.wUserIndex = aRecv->wUserIndex;
	pMsg.Result = TRUE;

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqMineCheckIsUPTWhenUserConnect(int aIndex, SDHP_REQ_LOAD_MINESYSTEM_UPT_USERINFO * aRecv)
{
	SDHP_ANS_LOAD_MINESYSTEM_UPT_USERINFO pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0x4C, 0x01, sizeof(pMsg));


	pMsg.wUserIndex = aRecv->wUserIndex;
	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqPShopItemValue(int aIndex, PMSG_REQ_PSHOPITEMVALUE_INFO * aRecv)
{
	PMSG_ANS_PSHOPITEMVALUE_INFO pMsg;
	PHeadSetW((LPBYTE)&pMsg, 0xE9, sizeof(pMsg));

	int count = 0;

	pMsg.btItemCnt = count;
	pMsg.iUserIndex = aRecv->iUserIndex;
	
	DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GDAllSavePShopItemValue(int aIndex, PMSG_UPDATE_PSHOPITEMVALUE_INFO * aRecv)
{

}

void CDataServerProtocol::GDDelPShopItemValue(int aIndex, PMSG_DEL_PSHOPITEM * aRecv)
{



}

void CDataServerProtocol::GDMovePShopItem(int aIndex, PMSG_MOVE_PSHOPITEM * aRecv)
{



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
			DataSend(i, (PBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
		}
	}
}

void CDataServerProtocol::GDReqClassDefData(int aIndex)
{
	PMSG_ANS_CLASSDEF pMsg;
	PHeadSetW((LPBYTE)&pMsg, 0x02, sizeof(pMsg));


	int Count = 0;

	memset(pMsg.m_ClassData, -1, sizeof(pMsg.m_ClassData));

	pMsg.iCount = Count;



	DataSend(aIndex, (LPBYTE)&pMsg, sizeof(pMsg), __FUNCTION__);
}

void CDataServerProtocol::GDReqReBuyItemList(int aIndex, SDHP_REQ_SHOP_REBUY_LIST * aRecv)
{



	BYTE BUFFER[2048];
	SDHP_ANS_SHOP_REBUY_LIST * lpMsg = (SDHP_ANS_SHOP_REBUY_LIST *)BUFFER;
	SDHP_SHOP_REBUY_ITEM * lpItem = (SDHP_SHOP_REBUY_ITEM *)BUFFER + sizeof(SDHP_ANS_SHOP_REBUY_LIST);
	INT64 nLen = 0;

	lpMsg->iCount = 0;
	lpMsg->btResult = 1;

	char szTemp[256];

	for (int i = 0; i < lpMsg->iCount; i++)
	{
		lpItem = (SDHP_SHOP_REBUY_ITEM *)(BUFFER + sizeof(SDHP_ANS_SHOP_REBUY_LIST) + (i * sizeof(SDHP_SHOP_REBUY_ITEM)));

		sprintf(szTemp, "SELECT ItemData FROM IGC_CancelItemSale WHERE AccountID = '%s' AND Name = '%s' AND ItemNumber = %d AND SellPrice = %d",
			aRecv->szAccountID, aRecv->szName, lpItem->btItemNumber, lpItem->dwSellPrice);


	}



	int PacketSize = sizeof(SDHP_ANS_SHOP_REBUY_LIST) + (lpMsg->iCount * sizeof(SDHP_SHOP_REBUY_ITEM));

	lpMsg->h.c = 0xC2;
	lpMsg->h.sizeH = HIBYTE(PacketSize);
	lpMsg->h.sizeL = LOBYTE(PacketSize);
	lpMsg->h.headcode = 0x6F;
	lpMsg->h.subcode = 0x00;
	lpMsg->iIndex = aRecv->iIndex;
	std::memcpy(lpMsg->szAccountID, aRecv->szAccountID, MAX_ACCOUNT_LEN + 1);
	std::memcpy(lpMsg->szName, aRecv->szName, MAX_ACCOUNT_LEN + 1);

	DataSend(aIndex, BUFFER, PacketSize, __FUNCTION__);
}

void CDataServerProtocol::GDReqReBuyAddItem(int aIndex, SDHP_REQ_SHOP_REBUY_ADD_ITEM * aRecv)
{



	char szTemp[256];
	sprintf(szTemp, "EXEC IGC_CancelItemSale_AddItem '%s', '%s', %I64d, %I64d, %d, ?", aRecv->szAccountID, aRecv->szName, aRecv->SellDate, aRecv->SellExpireDate, aRecv->dwSellPrice);



	memset(szTemp, 0, 256);
}

void CDataServerProtocol::GDReqReBuyGetItem(int aIndex, SDHP_REQ_SHOP_REBUY_GET_ITEM * aRecv)
{
	SDHP_ANS_SHOP_REBUY_GET_ITEM pMsg;
	char szTemp[256];
	INT64 nLen = 0;

	sprintf(szTemp, "EXEC IGC_CancelItemSale_GetItemToReBuy '%s', '%s', %d, %d", aRecv->szAccountID, aRecv->szName, aRecv->btItemNumber, aRecv->dwItemPrice);
	


	if (pMsg.btResult == 0)
	{
		sprintf(szTemp, "SELECT ItemData FROM IGC_CancelItemSale WHERE AccountID = '%s' AND Name = '%s' AND ItemNumber = %d AND SellPrice = %d",
			aRecv->szAccountID, aRecv->szName, pMsg.btItemNumber, pMsg.dwSellPrice);


	}

	PHeadSubSetB((LPBYTE)&pMsg, 0x6F, 0x02, sizeof(pMsg));
	pMsg.iIndex = aRecv->iIndex;
	std::memcpy(pMsg.szAccountID, aRecv->szAccountID, MAX_ACCOUNT_LEN + 1);
	std::memcpy(pMsg.szName, aRecv->szName, MAX_ACCOUNT_LEN + 1);
	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqDeleteSoldItem(int aIndex, SDHP_REQ_SHOP_REBUY_DELETE_ITEM * aRecv)
{


}

void CDataServerProtocol::GDReqGremoryCaseItemList(int aIndex, _stReqGremoryCaseItemList * aRecv)
{
	BYTE BUFFER[8192];

	_stAnsGremoryCaseItemList * lpMsg = (_stAnsGremoryCaseItemList *)(BUFFER);
	_stGremoryCaseItem * lpItem = (_stGremoryCaseItem *)(BUFFER + sizeof(_stAnsGremoryCaseItemList));



	int PacketSize = sizeof(_stAnsGremoryCaseItemList) + (lpMsg->btCount * sizeof(_stGremoryCaseItem));

	lpMsg->h.c = 0xC2;
	lpMsg->h.sizeH = HIBYTE(PacketSize);
	lpMsg->h.sizeL = LOBYTE(PacketSize);
	lpMsg->h.headcode = 0x4F;
	lpMsg->h.subcode = 0x00;
	lpMsg->iIndex = aRecv->iIndex;
	std::memcpy(lpMsg->szAccountID, aRecv->szAccountID, MAX_ACCOUNT_LEN+1);
	std::memcpy(lpMsg->szName, aRecv->szName, MAX_ACCOUNT_LEN+1);

	DataSend(aIndex, BUFFER, PacketSize, __FUNCTION__);
}

void CDataServerProtocol::GDReqGremoryCaseAddItem(int aIndex, _stReqAddItemToGremoryCase * aRecv)
{
	_stAnsAddItemToGremoryCase pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0x4F, 0x01, sizeof(pMsg));

}

void CDataServerProtocol::GDReqCheckUseItemGremoryCase(int aIndex, _stReqCheckUseItemGremoryCase * aRecv)
{
	_stAnsCheckUseItemGremoryCase pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0x4F, 0x02, sizeof(pMsg));

	pMsg.iIndex = aRecv->iIndex;
	std::memcpy(pMsg.szAccountID, aRecv->szAccountID, MAX_ACCOUNT_LEN+1);
	std::memcpy(pMsg.szName, aRecv->szName, MAX_ACCOUNT_LEN+1);
	pMsg.wItemID = aRecv->wItemID;
	pMsg.dwItemGUID = aRecv->dwItemGUID;
	pMsg.dwAuthCode = aRecv->dwAuthCode;

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqGremoryCaseDeleteItem(int aIndex, _stReqDeleteItemFromGremoryCase * aRecv)
{


}

void CDataServerProtocol::GDReqUBFCheckIsJoinedUser(int aIndex, PMSG_REQ_UBF_ACCOUNT_USERINFO * aRecv)
{
	PMSG_ANS_UBF_ACCOUNT_USERINFO pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x01, sizeof(pMsg));



	pMsg.btObserverMode = aRecv->btObserverMode;
	pMsg.iUserIndex = aRecv->iUserIndex;
	
	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqUBFJoinUser(int aIndex, PMSG_UBF_REGISTER_ACCOUNT_USER * aRecv)
{
	PMSG_UBF_REGISTER_ACCOUNT_USER_RESULT pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x02, sizeof(pMsg));


	pMsg.iUserIndex = aRecv->iUserIndex;
	
	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqUBFCopyCharacter(int aIndex, PMSG_UBF_ACCOUNT_USER_COPY * aRecv)
{
	PMSG_UBF_ACCOUNT_USER_COPY_RESULT pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x03, sizeof(pMsg));


	pMsg.btSubResult = 0;

	pMsg.iUserIndex = aRecv->iUserIndex;
	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqUBFCancelUser(int aIndex, PMSG_UBF_REQ_CANCEL_REGISTER_USER * aRecv)
{
	PMSG_UBF_ANS_CANCEL_REGISTER_USER pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x07, sizeof(pMsg));

	pMsg.iUserIndex = aRecv->iUserIndex;
	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqUBFGetRealName(int aIndex, PMSG_REQ_GET_UBF_REAL_NAME * aRecv)
{
	PMSG_ANS_GET_UBF_REAL_NAME pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x08, sizeof(pMsg));

	strcpy(pMsg.szUBFName, aRecv->szUBFName);
	pMsg.btReturn = 0;
	pMsg.iUserIndex = aRecv->iUserIndex;

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqUBFCopyPetItem(int aIndex, LPBYTE lpRecv)
{
	PMSG_UBF_ACCOUNT_USER_COPY_PETITEM * lpMsg = (PMSG_UBF_ACCOUNT_USER_COPY_PETITEM *)lpRecv;
	PMSG_UBF_COPY_PETITEM * lpItem = (PMSG_UBF_COPY_PETITEM *)lpRecv + sizeof(PMSG_UBF_ACCOUNT_USER_COPY_PETITEM);


}

void CDataServerProtocol::GDReqUBFSetCCFReward(int aIndex, SDHP_REQ_SET_CCF_WINNER_INFO * aRecv)
{



}

void CDataServerProtocol::GDReqUBFSetCCBattleReward(int aIndex, SDHP_REQ_SET_CC_WINNER_INFO_UBF * aRecv)
{



}

void CDataServerProtocol::GDReqUBFSetDSFReward(int aIndex, PMSG_REQ_SET_DSF_WINNER_INFO * aRecv)
{



}

void CDataServerProtocol::GDReqUBFGetReward(int aIndex, PMSG_REQ_UBF_GET_REWARD * aRecv)
{
	PMSG_ANS_UBF_GET_REWARD pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x06, sizeof(pMsg));


	pMsg.btBattleKind = aRecv->btBattleKind;
	pMsg.iUserIndex = aRecv->iUserIndex;

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqUBFSetGainReward(int aIndex, PMSG_REQ_UBF_SET_RECEIVED_REWARD * aRecv)
{
	PMSG_ANS_UBF_SET_RECEIVED_REWARD pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xF3, 0x05, sizeof(pMsg));

	pMsg.iUserIndex = aRecv->iUserIndex;


	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqDSFCanPartyEnter(int aIndex, PMSG_REQ_DSF_CAN_PARTY_ENTER * aRecv)
{
	PMSG_ANS_DSF_CAN_PARTY_ENTER pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xFD, 0x00, sizeof(pMsg));


	pMsg.iUserIndex = aRecv->iUserIndex;
	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqDSFSavePartyPoint(int aIndex, PMSG_REQ_SAVE_DSF_PARTYPOINT * aRecv)
{

}

void CDataServerProtocol::GDReqDSFPartyRankRenew(int aIndex, PMSG_REQ_DSF_PARTYRANKRENEW * aRecv)
{
	PMSG_ANS_DSF_PARTYRANKRENEW pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xFD, 0x03, sizeof(pMsg));

	pMsg.btResult = 1;
	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqDSFGoFinalParty(int aIndex, PMSG_REQ_DSF_GO_FINAL_PARTY * aRecv)
{
	PMSG_ANS_DSF_GO_FINAL_PARTY pMsg;
	DSF_GO_FINAL_PARTY pParty;

	BYTE sendBuf[4096];
	int iSize = sizeof(PMSG_ANS_DSF_GO_FINAL_PARTY);
	int iCount = 0;


	PHeadSubSetW((LPBYTE)&pMsg, 0xFD, 0x05, iSize);
	pMsg.btDSFType = aRecv->btDSFType;
	pMsg.btPartyCnt = iCount;

	std::memcpy(&sendBuf, &pMsg, sizeof(pMsg));

	DataSend(aIndex, sendBuf, iSize, __FUNCTION__);
}

void CDataServerProtocol::GDReqDSFInsertRewardUser(int aIndex, PMSG_REQ_SAVE_DSF_REWARD_USER * aRecv)
{

}

void CDataServerProtocol::GDReqDSFGetReward(int aIndex, PMSG_REQ_GET_DSF_REWARD * aRecv)
{
	PMSG_ANS_GET_DSF_REWARD pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xFD, 0x07, sizeof(pMsg));


	pMsg.nUserIndex = aRecv->nUserIndex;
	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

// JoinMu

void CDataServerProtocol::GDReqWishperOtherChannel(int aIndex, PMSG_RECV_CHATDATA_WHISPER * aRecv)
{
	PMSG_RECV_CHATDATA_WHISPER pMsg;
	pMsg.h.c = 0xC1;
	pMsg.h.headcode = 0xC2;
	pMsg.h.size = sizeof(pMsg);
	std::memcpy(pMsg.fromId,aRecv->fromId,sizeof(aRecv->fromId));
	std::memcpy(pMsg.id,aRecv->id,sizeof(pMsg.id));
	std::memcpy(pMsg.chatmsg, aRecv->chatmsg,sizeof(pMsg.chatmsg));
	pMsg.wMapSvrNum = aRecv->wMapSvrNum;
	pMsg.OriginGSIndex = aIndex;
	pMsg.OriginPlayerIndex = aRecv->OriginPlayerIndex;
	for(int i=0;i<g_dwMaxServerGroups;i++)
	{
		if(g_Server[i].m_Index != -1 
			&& g_Server[i].m_Type == g_Server[aIndex].m_Type
			&& g_Server[i].m_ServerGroup == aRecv->wMapSvrNum)
		{
			DataSend(i,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
		}
	}
}

void CDataServerProtocol::GDReqMapSrvGroupServerCount(int aIndex, DSMSG_REQ_SUBSERVER_COUNT * aRecv)
{
	WORD wServerCount = 0;
	DSMSG_ANS_SUBSERVER_COUNT pMsg;
	pMsg.h.set((LPBYTE)&pMsg,0xC3,0x01,sizeof(pMsg));
	
	for(int i=0;i<g_dwMaxServerGroups;i++)
	{
		if(g_Server[i].m_Index != -1 
			&& g_Server[i].m_Type == g_Server[aIndex].m_Type
			&& g_Server[i].m_ServerGroup == aRecv->wMapSvrGroup)
		{
			wServerCount++;
		}
	}

	pMsg.wSvrCount = wServerCount;

	DataSend(aIndex,(PBYTE)&pMsg,sizeof(pMsg),__FUNCTION__);

	
}

void CDataServerProtocol::DGWhisperResponseRecv(int aIndex, DSMSG_ANS_WHISPER_RESULT * aRecv)
{
	DSMSG_GS_WHISPER_RESULT pMsg;
	pMsg.h.set((LPBYTE)&pMsg,0xC3,0x03,sizeof(pMsg));

	pMsg.btResult = aRecv->btResult;
	pMsg.OriginPlayerIndex = aRecv->OriginPlayerIndex;
	
	DataSend(aRecv->OriginGSIndex,(PBYTE)&pMsg,sizeof(pMsg),__FUNCTION__);
}

void CDataServerProtocol::DGChaosMachineLogLostItems(int aIndex, DSMSG_CHAOSMACHINE_LOSTITEMS * aRecv)
{
	char szTemp[256];
	sprintf(szTemp, "INSERT INTO IGC_ChaosMachine_LostItems (AccountID, Name, LostDate, ?) VALUES ('%s', '%s', GETDATE())", aRecv->szAccountID, aRecv->szName);
}

void CDataServerProtocol::GDDisconnectOtherChannel(int aIndex, PMSG_RECV_DC_OTHER_CHANNEL * aRecv)
{
	PMSG_RECV_DC_OTHER_CHANNEL pMsg;
	pMsg.h.set((LPBYTE)&pMsg,0xC3,0x04,sizeof(pMsg));

	std::memcpy(pMsg.szName,aRecv->szName,sizeof(pMsg.szName));
	pMsg.wMapSrvGroup = aRecv->wMapSrvGroup;
	for(int i=0;i<g_dwMaxServerGroups;i++)
	{
		if(g_Server[i].m_Index != -1 
			&& g_Server[i].m_Type == g_Server[aIndex].m_Type
			&& g_Server[i].m_ServerGroup == aRecv->wMapSrvGroup)
		{
			DataSend(i,(PBYTE)&pMsg,sizeof(pMsg), __FUNCTION__);
		}
	}

}

void CDataServerProtocol::GDReqEventEntryCount(int aIndex, PMSG_REQ_EVENTENTRY_INFO * aRecv)
{
	PMSG_ANS_EVENTENTRY_INFO pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0xEE, 0x00, sizeof(pMsg));

	pMsg.nUserIndex = aRecv->nUserIndex;
	memset(pMsg.btEventEntryCount, 0x00, sizeof(pMsg.btEventEntryCount));

	if(g_DSBattleCoreEnable == 0)
	{ 
	   for (int i = 0; i < 6; i++)
	   {





	   }
	}
	

	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
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


	}





	DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CDataServerProtocol::GDReqEvoMonSaveScore(int aIndex, PMSG_REQ_SAVE_EVOMON_RESULT* aRecv)
{

}

void CDataServerProtocol::DGMuBotOptionRecv(LPMUBOT_SETTINGS_REQ_SAVE lpMsg, int aIndex)
{
	char szName[MAX_IDSTRING + 1];
	std::memcpy(szName, lpMsg->szName, MAX_IDSTRING);
	szName[MAX_IDSTRING] = '\0';

	MuBotSaveOption(szName, lpMsg);
}

void CDataServerProtocol::MuBotSaveOption(char* szName, MUBOT_SETTINGS_REQ_SAVE* lpMsg)
{
	char szQuery[512];
	sprintf(szQuery, "UPDATE OptionData SET MuBot = ? WHERE Name = '%s'", szName);


}

void CDataServerProtocol::GetMuBotData(char* szName, MUBOT_SETTINGS_SEND* lpMsg)
{
	char szQuery[512];
	sprintf(szQuery, "SELECT * FROM OptionData WHERE Name='%s'", szName);

	lpMsg->btDATA[0] = 0xFF;


	{

		{


			BYTE btTemp[sizeof(lpMsg->btDATA)] = { 0 };
			sprintf(szQuery, "SELECT MuBot FROM OptionData WHERE Name='%s'", szName);

			std::memcpy(lpMsg->btDATA, btTemp, sizeof(lpMsg->btDATA));
		}
	}


}
