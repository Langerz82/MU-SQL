#include "Warehouse.h"
#include "IOCP.h"
#include "ItemSerial.h"
#include "ServerEngine.h"
#include "MapServerManager.h"
#include "Utility/util.h"
#include "Database/Database/MySQLConnection.h"

using namespace std;

CWarehouse::CWarehouse(DatabaseWorkerPool<MySQLConnection>* db): m_WareDB(db)
{
	InitializeCriticalSection(&this->m_WareDataCriti);
	m_vtWarehouseData.clear();
}

CWarehouse::~CWarehouse()
{
	DeleteCriticalSection(&this->m_WareDataCriti);
}

void CWarehouse::AddUserData(char * szAccountID)
{
	EnterCriticalSection(&this->m_WareDataCriti);

	for (std::vector<USERWAREHOUSE_DATA*>::iterator it = this->m_vtWarehouseData.begin(); it != this->m_vtWarehouseData.end(); it++)
	{
		if (strcmp(szAccountID, (*it)->szAccountID) == 0)
		{
			this->m_vtWarehouseData.erase(it);
			break;
		}
	}

	USERWAREHOUSE_DATA* m_UserWareData = new USERWAREHOUSE_DATA();

	std::memcpy(m_UserWareData->szAccountID, szAccountID, MAX_ACCOUNT_LEN + 1);
	m_UserWareData->WarehouseID = 0;
	m_UserWareData->ChangeIDEnableState = TRUE;
	m_UserWareData->WarehouseOpenState = false;
	m_UserWareData->LastChangeTick = GetTickCount();

	m_vtWarehouseData.push_back(m_UserWareData);

	LeaveCriticalSection(&this->m_WareDataCriti);
}

void CWarehouse::DelUserData(char * szAccountID)
{
	for (std::vector<USERWAREHOUSE_DATA*>::iterator it = this->m_vtWarehouseData.begin(); it != this->m_vtWarehouseData.end(); it++)
	{
		if (strcmp(szAccountID, (*it)->szAccountID) == 0)
		{
			this->m_vtWarehouseData.erase(it);
			break;
		}
	}
}

void CWarehouse::SetChangeEnableState(char * szAccountID, int State)
{
	for (std::vector<USERWAREHOUSE_DATA*>::iterator it = this->m_vtWarehouseData.begin(); it != this->m_vtWarehouseData.end(); it++)
	{
		if (strcmp(szAccountID, (*it)->szAccountID) == 0)
		{
			(*it)->ChangeIDEnableState = State;
			(*it)->LastChangeTick = GetTickCount();
			break;
		}
	}
}

int CWarehouse::SwitchWarehouse(char *szAccountID, int WarehouseID)
{
	USERWAREHOUSE_DATA* pData = NULL;

	for (std::vector<USERWAREHOUSE_DATA*>::iterator it = this->m_vtWarehouseData.begin(); it != this->m_vtWarehouseData.end(); it++)
	{
		if (strcmp(szAccountID, (*it)->szAccountID) == 0)
		{
			pData = *it;
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

int CWarehouse::GetWarehouseID(char * szAccountID)
{
	int WareID = 0;

	for (std::vector<USERWAREHOUSE_DATA*>::iterator it = this->m_vtWarehouseData.begin(); it != this->m_vtWarehouseData.end(); it++)
	{
		if (strcmp(szAccountID, (*it)->szAccountID) == 0)
		{
			WareID = (*it)->WarehouseID;
			break;
		}
	}

	return WareID;
}

bool CWarehouse::GetWarehouseOpenState(char * szAccountID)
{
	for (std::vector<USERWAREHOUSE_DATA*>::iterator it = this->m_vtWarehouseData.begin(); it != this->m_vtWarehouseData.end(); it++)
	{
		if (strcmp(szAccountID, (*it)->szAccountID) == 0)
		{
			return (*it)->WarehouseOpenState;
		}
	}

	return false;
}

void CWarehouse::SetWarehouseOpenState(char * szAccountID, bool bState)
{
	for (std::vector<USERWAREHOUSE_DATA*>::iterator it = this->m_vtWarehouseData.begin(); it != this->m_vtWarehouseData.end(); it++)
	{
		if (strcmp(szAccountID, (*it)->szAccountID) == 0)
		{
			(*it)->WarehouseOpenState = bState;
		}
	}
}

void CWarehouse::GDReqSwitchWarehouse(CGameObject &Obj, PMSG_REQ_SWITCHWARE * aRecv)
{
	EnterCriticalSection(&this->m_WareDataCriti);

	PMSG_ANS_SWITCHWARE pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x03, sizeof(pMsg));

	pMsg.iIndex = aRecv->iIndex;
	pMsg.WarehouseID = aRecv->WarehouseID;
	std::memcpy(pMsg.szAccountID, aRecv->szAccountID, MAX_ACCOUNT_LEN + 1);
	pMsg.Result = this->SwitchWarehouse(aRecv->szAccountID, aRecv->WarehouseID);

	LeaveCriticalSection(&this->m_WareDataCriti);

	DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size, __FUNCTION__);
	sLog->outBasic("[Warehouse][%s] Switch to Warehouse:%d Result:%d", pMsg.szAccountID, pMsg.WarehouseID, pMsg.Result);
}

void CWarehouse::DGGetWarehouseList(CGameObject &Obj, SDHP_GETWAREHOUSEDB * aRecv)
{
	EnterCriticalSection(&this->m_WareDataCriti);

	char szAccountID[11] = { 0 };
	szAccountID[10] = 0;
	memcpy(szAccountID, aRecv->AccountID, 10);

	SDHP_GETWAREHOUSEDB_SAVE pResult;

	PHeadSetW((BYTE*)&pResult, 0x08, sizeof(pResult));
	memcpy(pResult.AccountID, szAccountID, 11);
	pResult.aIndex = aRecv->aIndex;

	if (this->GetWarehouseOpenState(szAccountID) == true)
	{
		sLog->outError("[Warehouse] (%s) - trying to open already opened warehouse.", szAccountID);

		LeaveCriticalSection(&this->m_WareDataCriti);
		return;
	}

	QueryResult* result = this->m_WareDB.Fetch("SELECT AccountID FROM warehouse WHERE AccountID='%s'", szAccountID);
	if (result == NULL)
	{
		//this->m_WareDB.Close();
		this->m_WareDB.ExecQuery("INSERT INTO warehouse (AccountID, Money, EndUseDate, WHOpen) VALUES ('%s',0, getdate(),1)", szAccountID);

		this->SetChangeEnableState(szAccountID, FALSE);
		this->SetWarehouseOpenState(szAccountID, true);

		SDHP_GETWAREHOUSEDB_RESULT pMsg;
		PHeadSetB((BYTE*)&pMsg, 0x10, sizeof(pMsg));
		pMsg.aIndex = aRecv->aIndex;
		memcpy(pMsg.AccountID, szAccountID, 11);

		LeaveCriticalSection(&this->m_WareDataCriti);
		DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size, __FUNCTION__);
		return;
	}

	// This is the new method calls to DB API.
	this->m_WareDB.ExecQuery("UPDATE warehouse set WHOpen = 1 WHERE AccountID='%s'", szAccountID);
	QueryResult* result2 = this->m_WareDB.Fetch("SELECT Money, pw FROM warehouse WHERE AccountID='%s'", szAccountID);

	pResult.Money = this->m_WareDB.GetAsInteger(0);
	pResult.pw = this->m_WareDB.GetAsInteger(1);

	char szTemp[128];

	this->SetChangeEnableState(szAccountID, FALSE);
	this->SetWarehouseOpenState(szAccountID, true);
	int WarehouseID = this->GetWarehouseID(szAccountID);

	switch (WarehouseID)
	{
	case 0:
		wsprintf(szTemp, "SELECT Items FROM warehouse WHERE AccountID='%s'", szAccountID);
		break;

	default:
		wsprintf(szTemp, "SELECT Items%d FROM warehouse WHERE AccountID='%s'", WarehouseID + 1, szAccountID);
		break;
	};

	int ret = this->m_WareDB.GetAsBinary(szTemp, pResult.dbItems, sizeof(pResult.dbItems));
	if (ret < 0)
	{
		this->SetChangeEnableState(szAccountID, TRUE);
		this->SetWarehouseOpenState(szAccountID, false);

		LeaveCriticalSection(&this->m_WareDataCriti);
		return;
	}

	if (ret == 0)
	{
		memset(pResult.dbItems, -1, sizeof(pResult.dbItems));
	}

	else if (ret > 0 && ret < sizeof(pResult.dbItems))
	{
		for (int i = ret; i < sizeof(pResult.dbItems); i++)
		{
			pResult.dbItems[i] = -1;
		}
	}

	LeaveCriticalSection(&this->m_WareDataCriti);
	DataSend(lpObj.m_Index, (BYTE*)&pResult, sizeof(pResult), __FUNCTION__);
}

void CWarehouse::GDSetWarehouseList(CGameObject &Obj, SDHP_GETWAREHOUSEDB_SAVE * aRecv)
{
	EnterCriticalSection(&this->m_WareDataCriti);

	char szAccountID[11] = { 0 };
	szAccountID[10] = 0;
	memcpy(szAccountID, aRecv->AccountID, 10);

	if (this->GetWarehouseOpenState(szAccountID) == false)
	{
		sLog->outError("[Warehouse] (%s) - trying to close already closed warehouse. (Item)", szAccountID);

		LeaveCriticalSection(&this->m_WareDataCriti);
		return;
	}

	this->m_WareDB.ExecQuery("UPDATE warehouse SET Money=%d, pw=%d WHERE AccountID='%s'",
		aRecv->Money, aRecv->pw, szAccountID);

	char it[15];
	sprintf(it, "Items%d", aRecv->WarehouseID + 1);
	char szTemp[128];

	int WarehouseID = this->GetWarehouseID(szAccountID);

	switch (WarehouseID)
	{
	case 0:
		wsprintf(szTemp, "UPDATE warehouse SET Items=? WHERE AccountID='%s'", szAccountID);
		break;

	default:
		wsprintf(szTemp, "UPDATE warehouse SET Items%d=? WHERE AccountID='%s'", WarehouseID + 1, szAccountID);
		break;
	};

	this->m_WareDB.SetAsBinary(szTemp, aRecv->dbItems, sizeof(aRecv->dbItems));

	if (aRecv->CloseWindow == TRUE)
	{
		this->m_WareDB.ExecQuery("UPDATE warehouse set WHOpen = 0 WHERE AccountID='%s'", szAccountID);

		this->SetChangeEnableState(szAccountID, TRUE);
		this->SetWarehouseOpenState(szAccountID, false);
	}

	PMSG_ANS_WARESAVE pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x09, sizeof(pMsg));

	pMsg.Result = TRUE;
	pMsg.CloseWindow = aRecv->CloseWindow;
	pMsg.iIndex = aRecv->aIndex;

	LeaveCriticalSection(&this->m_WareDataCriti);
	DataSend(lpObj.m_Index, (BYTE*)&pMsg, pMsg.h.size, __FUNCTION__);
}

void CWarehouse::GDSetWarehouseMoney(CGameObject &Obj, SDHP_WAREHOUSEMONEY_SAVE * aRecv)
{
	EnterCriticalSection(&this->m_WareDataCriti);

	char szAccountID[11] = { 0 };
	szAccountID[10] = 0;
	memcpy(szAccountID, aRecv->AccountID, 10);

	this->m_WareDB.ExecQuery("UPDATE warehouse SET Money=%d WHERE AccountID='%s'", aRecv->Money, szAccountID);

	LeaveCriticalSection(&this->m_WareDataCriti);
}
