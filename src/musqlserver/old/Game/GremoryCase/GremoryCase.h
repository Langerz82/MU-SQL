////////////////////////////////////////////////////////////////////////////////
// GremoryCase.h
#ifndef GREMORYCASE_H
#define GREMORYCASE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "ItemManagement/Item.h"
#include "protocolStructs.h"
#include "generalStructs.h"

struct _stGremoryCaseItem;
struct _stAnsAddItemToGremoryCase;
struct PMSG_ADD_GREMORYCASE_ITEM_TO_INVENTORY_REQ;

class CGremoryCase
{
public:
	CGremoryCase(void);
	virtual ~CGremoryCase(void);

	void CheckStorageExpiredItems();

	void GDReqStorageItemList(int iIndex);
	void DGAnsStorageItemList(LPBYTE lpRecv);
	void GDReqAddItemToGremoryCase(int iIndex, _stGremoryCaseItem stItem, int iDaysToExpire);
	void DGAnsAddItemToGremoryCase(_stAnsAddItemToGremoryCase * lpMsg);
	void GDReqCheckItemUseGremoryCase(int iIndex, WORD wItemID, DWORD dwItemGUID, DWORD dwAuthCode);
	void DGAnsCheckItemUseGremoryCase(_stAnsCheckUseItemGremoryCase * lpMsg);
	void GDReqDeleteItemFromGremoryCase(int iIndex, WORD wItemID, DWORD dwItemGUID, DWORD dwAuthCode);

	void GCSendStorageItemList(int iIndex);
	void GCSendAddItemToGremoryCase(int iIndex, BYTE btStorageType, BYTE btItemArrayIndex);
	void CGReqGetItemFromGremoryCase(PMSG_ADD_GREMORYCASE_ITEM_TO_INVENTORY_REQ * lpMsg, int iIndex);
	void GCSendDeleteItemFromGremoryCase(int iIndex, BYTE btStorageType, int iItemArrayIndex);
	void CGReqOpenGremoryCase(int iIndex);

private:
	void CheckIsInStorageItemAboutToExpire(int iIndex);
	void CheckIsStorageFull(int iIndex);
};

extern CGremoryCase g_GremoryCase;

#endif
