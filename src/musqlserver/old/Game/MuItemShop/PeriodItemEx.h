////////////////////////////////////////////////////////////////////////////////
// PeriodItemEx.h
#ifndef PERIODITEMEX_H
#define PERIODITEMEX_H

#include "StdAfx.h"


#define MAX_PERIOD_ITEMS 30

enum PERIODITEM_TYPE
{
	PERIODITEM_BUFF = 1,
	PERIODITEM_NORMAL = 2
};

struct ITEM_DATA
{
	BYTE btUsedInfo;
	BYTE btItemType; // used
	WORD wItemCode; // used
	BYTE btEffectCategory;
	BYTE btEffectType1;
	BYTE btEffectType2;
	UINT64 Serial; // used
	time_t lItemBuyDate; // used
	time_t lItemExpireDate; // used
	DWORD dwDuration; // used

	void Clear()
	{
		this->btUsedInfo = FALSE;
		this->btItemType = FALSE;
		this->wItemCode = 0;
		this->Serial = 0;
	}
};

struct PERIOD_DATA
{
	BYTE btUsedInfo;
	BYTE btItemCount;
	WORD wUserIndex;
	DWORD dwUserGuid;
	CGameObject* m_Obj; // lol
	char szAccountID[MAX_ACCOUNT_LEN+1];
	char szCharacterName[MAX_ACCOUNT_LEN+1];
	ITEM_DATA* m_ItemInfo[30];
	void Clear()
	{
		this->btUsedInfo = FALSE;
		this->btItemCount = FALSE;
		this->wUserIndex = 0;
		this->dwUserGuid = 0;
		this->m_Obj = nullptr;
		this->szAccountID[0] = 0;
		this->szCharacterName[0] = 0;

		for(int i=0;i<30;i++)
		{
			this->m_ItemInfo[i]->Clear();
		}
	}
};

class CPeriodItemEx
{
public:
	CPeriodItemEx(void);
	virtual ~CPeriodItemEx(void);

	void Initialize();

	int AddPeriodInfo(CGameObject &Obj);
	BOOL RemovePeriodInfo(CGameObject &Obj);
	BOOL IsCorrectUser(CGameObject &Obj);

	int AddPeriodItemData(CGameObject &Obj, WORD ItemType, UINT64 Serial, DWORD Duration, time_t ExpireDate);
	int AddPeriodItemData(CGameObject &Obj, WORD ItemType, UINT64 Serial, DWORD Duration);
	int RemovePeriodItemData(CGameObject &Obj, BYTE ItemUseType, WORD ItemType, UINT64 Serial);
	int RemovePeriodItemDataByForce(CGameObject &Obj, WORD ItemType, UINT64 Serial);

	BOOL CheckExpireDate(time_t ExpireDate);
	void CheckPeriodData();
	time_t GetExpireDate(DWORD Duration);
	time_t GetLeftDate(time_t ExpireDate);
	int GetItemFromInventory(CGameObject &Obj, WORD ItemType, UINT64 Serial);

	time_t GetCurrentDate();

	void OnRequestPeriodItemList(CGameObject &Obj);
	void OnRequestPeriodItemListOnce(CGameObject &Obj, ITEM_DATA * lpItemData);

	void RequestPeriodItemInfo(CGameObject &Obj);
	void RequestPeriodItemInsert(CGameObject &Obj, ITEM_DATA * lpItemData, DWORD dwDuration);
	void RequestPeriodItemDelete(CGameObject &Obj, ITEM_DATA * lpItemData);

	void DGPeriodItemExInsert(PMSG_ANS_PERIODITEMEX_INSERT * lpMsg);
	void DGPeriodItemExDelete(PMSG_ANS_PERIODITEMEX_DELETE * lpMsg);
	void DGPeriodItemExExpiredItemList(PMSG_ANS_PERIODITEMEX_EXPIRED_ITEMLIST * lpMsg);
	void DGPeriodItemExListCount(PMSG_ANS_PERIODITEMEX_LIST_COUNT * lpMsg);
	void DGPeriodItemExList(PMSG_ANS_PERIODITEMEX_LIST * lpMsg);
	
	BOOL SendPeriodItemInfoOnce(CGameObject &Obj, CItemObject * Item);
	BOOL SendPeriodItemInfoOnce(CGameObject &Obj, UINT64 Serial);

	BOOL SetPeriodItemInfo(CGameObject &Obj, WORD wItemCode, UINT64 Serial, DWORD dwDuration);
	BOOL SetDisableItemToExpire(CGameObject &Obj, int pos);
	void SetExpireNotMatchedData(CGameObject &Obj);

	PERIOD_DATA * m_PeriodData;

};

extern CPeriodItemEx g_PeriodItemEx;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

