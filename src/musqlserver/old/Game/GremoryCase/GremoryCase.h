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
struct _stAnsCheckUseItemGremoryCase;



class CGremoryCase
{
public:
	CGremoryCase(void);
	virtual ~CGremoryCase(void);

	void CheckStorageExpiredItems();

	void GDReqStorageItemList(CGameObject &Obj);
	void DGAnsStorageItemList(LPBYTE lpRecv);
	void GDReqAddItemToGremoryCase(CGameObject &Obj, _stGremoryCaseItem stItem, int iDaysToExpire);
	void DGAnsAddItemToGremoryCase(_stAnsAddItemToGremoryCase * lpMsg);
	void GDReqCheckItemUseGremoryCase(CGameObject &Obj, WORD wItemID, DWORD dwItemGUID, DWORD dwAuthCode);
	void DGAnsCheckItemUseGremoryCase(_stAnsCheckUseItemGremoryCase * lpMsg);
	void GDReqDeleteItemFromGremoryCase(CGameObject &Obj, WORD wItemID, DWORD dwItemGUID, DWORD dwAuthCode);

	void GCSendStorageItemList(CGameObject &Obj);
	void GCSendAddItemToGremoryCase(CGameObject &Obj, BYTE btStorageType, BYTE btItemArrayIndex);
	void CGReqGetItemFromGremoryCase(PMSG_ADD_GREMORYCASE_ITEM_TO_INVENTORY_REQ * lpMsg, int iIndex);
	void GCSendDeleteItemFromGremoryCase(CGameObject &Obj, BYTE btStorageType, int iItemArrayIndex);
	void CGReqOpenGremoryCase(CGameObject &Obj);

private:
	void CheckIsInStorageItemAboutToExpire(CGameObject &Obj);
	void CheckIsStorageFull(CGameObject &Obj);
};

extern CGremoryCase g_GremoryCase;

#endif
