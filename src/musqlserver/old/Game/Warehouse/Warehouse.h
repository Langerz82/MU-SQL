
#ifndef _MU_WAREHOUSE_
#define _MU_WAREHOUSE_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "MuDefines.h"
#include "Database/Query.h"

class CWarehouse
{
public:

	CWarehouse(DatabaseWorkerPool<MySQLConnection>* db);
	~CWarehouse();

	void Init();

	void AddUserData(char * szAccountID);
	void DelUserData(char * szAccountID);

	int SwitchWarehouse(char * szAccountID, int WarehouseID);
	void SetChangeEnableState(char * szAccountID, int State);
	void SetWarehouseOpenState(char * szAccountID, bool bState);

	int GetWarehouseID(char * szAccountID);
	bool GetWarehouseOpenState(char * szAccountID);

	void DGGetWarehouseList(CGameObject &Obj, SDHP_GETWAREHOUSEDB * aRecv);
	void GDSetWarehouseList(CGameObject &Obj, SDHP_GETWAREHOUSEDB_SAVE * aRecv);
	void GDSetWarehouseMoney(CGameObject &Obj, SDHP_WAREHOUSEMONEY_SAVE * aRecv);
	void GDReqSwitchWarehouse(CGameObject &Obj, PMSG_REQ_SWITCHWARE * aRecv);

private:
	std::vector<USERWAREHOUSE_DATA*> m_vtWarehouseData;
	CRITICAL_SECTION m_WareDataCriti;
	CQuery m_WareDB;
};

#endif
