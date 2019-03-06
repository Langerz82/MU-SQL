////////////////////////////////////////////////////////////////////////////////
// PeriodItemEx.cpp
// Decompiled by Dudas [IGC] July 2013
// Status: 100%

#include "StdAfx.h"
#include "PeriodItemEx.h"
#include "BuffEffectSlot.h"
#include "ItemAddOption.h"
#include "User/CUserData.h"
#include "ObjUseSkill.h"
#include "ObjCalCharacter.h"

#include "Main.h"
#include "Logging/Log.h"
#include "GameProtocol.h"
#include "configread.h"

CPeriodItemEx g_PeriodItemEx;

CPeriodItemEx::CPeriodItemEx(void)
{
}

CPeriodItemEx::~CPeriodItemEx(void)
{
	if ( this->m_PeriodData != NULL )
	{
		delete [] this->m_PeriodData;
	}
}

void CPeriodItemEx::Initialize()
{
	if ( this->m_PeriodData != NULL )
	{
		delete [] this->m_PeriodData;
	}

	this->m_PeriodData = new PERIOD_DATA[g_ConfigRead.server.GetObjectMaxUser()];

	for (int i=0; i<g_ConfigRead.server.GetObjectMaxUser(); i++)
	{
		this->m_PeriodData[i].Clear();
	}
}

int CPeriodItemEx::AddPeriodInfo(CGameObject &Obj)
{
	if ( Obj.Type != OBJ_USER )
	{
		return -1;
	}

	for ( int i=0; i<g_ConfigRead.server.GetObjectMaxUser(); i++ )
	{
		if ( this->m_PeriodData[i].btUsedInfo == FALSE )
		{
			this->m_PeriodData[i].Clear();

			this->m_PeriodData[i].btUsedInfo = TRUE;
			this->m_PeriodData[i].m_Obj = lpObj;
			this->m_PeriodData[i].dwUserGuid = Obj.DBNumber;
			this->m_PeriodData[i].wUserIndex = Obj.m_Index;
			this->m_PeriodData[i].btItemCount = 0;
			std::memcpy(this->m_PeriodData[i].szAccountID, Obj.AccountID, MAX_ACCOUNT_LEN+1);
			std::memcpy(this->m_PeriodData[i].szCharacterName, Obj.Name, MAX_ACCOUNT_LEN+1);
			
			Obj.m_PlayerData->m_iPeriodItemEffectIndex = i;

			return i;
		}
	}

	return -1;
}

BOOL CPeriodItemEx::RemovePeriodInfo(CGameObject &Obj)
{
	if ( Obj.Type != OBJ_USER )
	{
		return FALSE;
	}

	int PeriodItemIndex = Obj.m_PlayerData->m_iPeriodItemEffectIndex;

	if ( this->IsCorrectUser(lpObj) == FALSE )
	{
		return FALSE;
	}

	if ( PeriodItemIndex < 0 || PeriodItemIndex >= g_ConfigRead.server.GetObjectMaxUser())
	{
		return FALSE;
	}

	if ( this->m_PeriodData[PeriodItemIndex].btUsedInfo == FALSE )
	{
		return FALSE;
	}

	this->m_PeriodData[PeriodItemIndex].Clear();

	return TRUE;
}

BOOL CPeriodItemEx::IsCorrectUser(CGameObject &Obj)
{
	if ( Obj.Type != OBJ_USER )
	{
		return FALSE;
	}

	int PeriodItemIndex = Obj.m_PlayerData->m_iPeriodItemEffectIndex;

	if ( PeriodItemIndex >= 0 )
	{
		if ( this->m_PeriodData[PeriodItemIndex].btUsedInfo == TRUE && this->m_PeriodData[PeriodItemIndex].dwUserGuid == Obj.DBNumber )
		{
			return TRUE;
		}
	}

	return FALSE;
}

int CPeriodItemEx::AddPeriodItemData(CGameObject &Obj, WORD ItemType, UINT64 Serial, DWORD Duration)
{
	if ( Obj.Type != OBJ_USER) 
	{
		return -1;
	}

	int PeriodItemIndex = Obj.m_PlayerData->m_iPeriodItemEffectIndex;

	if ( this->IsCorrectUser(lpObj) == FALSE )
	{
		return -1;
	}

	for ( int i = 0; i < MAX_PERIOD_ITEMS; i++ )
	{
		if ( this->m_PeriodData[PeriodItemIndex].m_ItemInfo[i].btUsedInfo == FALSE )
		{
			time_t ExpireDate = this->GetExpireDate(Duration);
			this->m_PeriodData[PeriodItemIndex].m_ItemInfo[i].btUsedInfo = TRUE;
			this->m_PeriodData[PeriodItemIndex].m_ItemInfo[i].btItemType = PERIODITEM_NORMAL;
			this->m_PeriodData[PeriodItemIndex].m_ItemInfo[i].wItemCode = ItemType;
			this->m_PeriodData[PeriodItemIndex].m_ItemInfo[i].Serial = Serial;
			this->m_PeriodData[PeriodItemIndex].m_ItemInfo[i].lItemBuyDate = this->GetCurrentDate();
			this->m_PeriodData[PeriodItemIndex].m_ItemInfo[i].lItemExpireDate = ExpireDate;
			this->m_PeriodData[PeriodItemIndex].m_ItemInfo[i].dwDuration = Duration;
			this->m_PeriodData[PeriodItemIndex].btItemCount++;

			int itempos = this->GetItemFromInventory(lpObj, ItemType, Serial);
			
			if ( itempos == -1 )
			{
				this->m_PeriodData[PeriodItemIndex].m_ItemInfo[i].Clear();
				this->m_PeriodData[PeriodItemIndex].btItemCount--;	
				return -2;
			}
			
			else
			{
				if ( Obj.pntInventory[itempos]->IsPeriodItem() == FALSE )
				{
					Obj.pntInventory[itempos]->SetPeriodItem();
				}

				return i;
			}		
		}
	}

	return -1;
}

int CPeriodItemEx::AddPeriodItemData(CGameObject &Obj, WORD ItemType, UINT64 Serial, DWORD Duration, time_t ExpireDate)
{
	if ( Obj.Type != OBJ_USER) 
	{
		return -1;
	}

	int PeriodItemIndex = Obj.m_PlayerData->m_iPeriodItemEffectIndex;

	if ( this->IsCorrectUser(lpObj) == FALSE )
	{
		return -1;
	}

	for ( int i=0; i<30 ; i++ )
	{
		if ( this->m_PeriodData[PeriodItemIndex].m_ItemInfo[i].btUsedInfo == FALSE )
		{
			this->m_PeriodData[PeriodItemIndex].m_ItemInfo[i].btUsedInfo = TRUE;
			this->m_PeriodData[PeriodItemIndex].m_ItemInfo[i].btItemType = PERIODITEM_NORMAL;
			this->m_PeriodData[PeriodItemIndex].m_ItemInfo[i].wItemCode = ItemType;
			this->m_PeriodData[PeriodItemIndex].m_ItemInfo[i].Serial = Serial;
			this->m_PeriodData[PeriodItemIndex].m_ItemInfo[i].lItemExpireDate = ExpireDate;
			this->m_PeriodData[PeriodItemIndex].m_ItemInfo[i].dwDuration = Duration;
			this->m_PeriodData[PeriodItemIndex].btItemCount++;

			int itempos = this->GetItemFromInventory(lpObj, ItemType, Serial);

			if ( itempos != -1 )
			{
				Obj.pntInventory[itempos]->SetPeriodItem();
			}

			return i;
		}
	}

	return -1;
}

BOOL CPeriodItemEx::RemovePeriodItemData(CGameObject &Obj, BYTE ItemType, WORD ItemIndex, UINT64 Serial)
{
	if ( !lpObj )
	{
		return FALSE;
	}

	if ( Obj.Connected < PLAYER_LOGGED )
	{
		return FALSE;
	}

	if ( Obj.Type != OBJ_USER )
	{
		return FALSE;
	}

	if ( ItemType == PERIODITEM_BUFF )
	{	
		LPBUFF_EFFECT_DATE lpEffect = g_BuffEffectSlot.GetEffectDataFromItemCode(ItemIndex);

		if ( lpEffect == NULL )
		{
			return FALSE;
		}

		gObjRemoveBuffEffect(lpObj, lpEffect->BuffIndex);
	}

	else if ( ItemType == PERIODITEM_NORMAL )
	{
		if ( Serial == 0 )
		{
			return FALSE;
		}

		int pos = this->GetItemFromInventory(lpObj, ItemIndex, Serial);

		if ( pos == -1 )
		{
			return FALSE;
		}

		this->SetDisableItemToExpire(lpObj, pos);
		//auto-deleting
		gObjInventoryDeleteItem(Obj.m_Index, pos);
		GSProtocol.GCInventoryItemDeleteSend(Obj.m_Index, pos, 1);
		gObjCalCharacter.CalcCharacter(Obj.m_Index);
		// ----
		if (pos < 12)
		{
			if (pos == 10 || pos == 11)
			{
				gObjUseSkill.SkillChangeUse(Obj.m_Index);
			}
			// ----
			gObjMakePreviewCharSet(Obj.m_Index);
			PMSG_USEREQUIPMENTCHANGED pChange;
			PHeadSetB((BYTE*)&pChange, 0x25, sizeof(pChange));
			pChange.NumberH = SET_NUMBERH(Obj.m_Index);
			pChange.NumberL = SET_NUMBERL(Obj.m_Index);
			ItemByteConvert(pChange.ItemInfo, Obj.pntInventory[pos]);
			pChange.ItemInfo[I_OPTION] = pos << 4;
			pChange.ItemInfo[I_OPTION] |= LevelSmallConvert(Obj.m_Index, pos) & MAX_ITEM_LEVEL;
			pChange.Element = Obj.m_iPentagramMainAttribute;
			if (pChange.ItemInfo[5] == 0xF0)
				pChange.ItemInfo[5] = 0xF1;

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pChange, pChange.h.size);
			GSProtocol.MsgSendV2(lpObj, (BYTE*)&pChange, pChange.h.size);
		}
	}

	ITEM_DATA m_ItemData;
	
	m_ItemData.btItemType = ItemType;
	m_ItemData.wItemCode = ItemIndex;
	m_ItemData.Serial = Serial;

	this->RequestPeriodItemDelete(lpObj, &m_ItemData);

	return TRUE;
}

BOOL CPeriodItemEx::RemovePeriodItemDataByForce(CGameObject &Obj, WORD ItemType, UINT64 Serial)
{
	if ( !lpObj )
	{
		return FALSE;
	}

	if ( Obj.Connected < PLAYER_LOGGED )
	{
		return FALSE;
	}

	if ( Obj.Type != OBJ_USER )
	{
		return FALSE;
	}

	if ( Serial == 0 )
	{
		return FALSE;
	}

	int iInventoryPosition = this->GetItemFromInventory(lpObj, ItemType, Serial);

	if ( iInventoryPosition == -1 )
	{
		return FALSE;
	}

	for (int i = 0; i < g_ConfigRead.server.GetObjectMaxUser(); i++)
	{
		if ( this->m_PeriodData[i].btUsedInfo == TRUE )
		{
			if ( this->m_PeriodData[i].m_Obj.DBNumber == Obj.DBNumber )
			{
				for ( int iItemSlotIndex = 0; iItemSlotIndex < 30; iItemSlotIndex++ )
				{
					if ( this->m_PeriodData[i].m_ItemInfo[iItemSlotIndex].Serial == Serial )
					{
						
						ITEM_DATA ItemData;

						ItemData.btItemType = 2;
						ItemData.wItemCode = ItemType;
						ItemData.Serial = Serial;
						this->RequestPeriodItemDelete(lpObj, &ItemData);
						this->m_PeriodData[i].m_ItemInfo[iItemSlotIndex].Clear();
						gObjCalCharacter.CalcCharacter(Obj.m_Index);
						return TRUE;
					}
				}
			}
		}
	}

	return FALSE;
}

void CPeriodItemEx::CheckPeriodData()
{
	for(int i=0;i<g_ConfigRead.server.GetObjectMaxUser();i++)
	{
		if ( this->m_PeriodData[i].btUsedInfo == TRUE )
		{
			if ( this->m_PeriodData[i].m_Obj )
			{
				CGameObject lpObj = this->m_PeriodData[i].m_Obj;

				if ( Obj.Type == OBJ_USER )
				{
					if ( Obj.Connected >= PLAYER_LOGGED )
					{
						for ( int j=0; j<30; j++ )
						{
							if ( this->m_PeriodData[i].m_ItemInfo[j].btUsedInfo == TRUE )
							{
								if ( this->CheckExpireDate(this->m_PeriodData[i].m_ItemInfo[j].lItemExpireDate) == TRUE )
								{
									
									this->RemovePeriodItemData(lpObj, this->m_PeriodData[i].m_ItemInfo[j].btItemType, this->m_PeriodData[i].m_ItemInfo[j].wItemCode, this->m_PeriodData[i].m_ItemInfo[j].Serial);
									this->m_PeriodData[i].m_ItemInfo[j].Clear();
									this->m_PeriodData[i].btItemCount--;
									gObjUseSkill.SkillChangeUse(Obj.m_Index);
									gObjCalCharacter.CalcCharacter(Obj.m_Index);
								}
							}
						}
					}
				}
			}
		}
	}
}

time_t CPeriodItemEx::GetCurrentDate()
{
	return time(NULL);
}

time_t CPeriodItemEx::GetExpireDate(DWORD Duration)
{
	time_t expiretime = time(NULL);

	expiretime += Duration;

	return expiretime;
}

BOOL CPeriodItemEx::CheckExpireDate(time_t ExpireDate)
{
	return this->GetCurrentDate() > ExpireDate;
}

time_t CPeriodItemEx::GetLeftDate(time_t ExpireDate)
{
	time_t currtime = time(NULL);
	time_t difftime = ExpireDate - currtime;

	return difftime;
}

BOOL CPeriodItemEx::SetPeriodItemInfo(CGameObject &Obj, WORD wItemCode, UINT64 Serial, DWORD dwDuration)
{
	if ( lpObj == NULL )
	{
		return FALSE;
	}

	if ( Obj.Connected < PLAYER_LOGGED )
	{
		return FALSE;
	}

	if ( Obj.Type != OBJ_USER )
	{
		return FALSE;
	}

	ITEM_DATA m_ItemData;

	//VM_TIGER_LONDON_START

	if ( Serial != 0 )
	{
		int pos = this->GetItemFromInventory(lpObj, wItemCode, Serial);

		if ( pos == -1 )
		{
			return FALSE;
		}

		Obj.pntInventory[pos]->SetPeriodItem();
		this->AddPeriodItemData(lpObj, wItemCode, Serial, dwDuration);
		m_ItemData.Clear();
		m_ItemData.wItemCode = wItemCode;
		m_ItemData.btItemType = PERIODITEM_NORMAL;
		m_ItemData.Serial = Obj.pntInventory[pos]->GetNumber();		
		m_ItemData.btEffectCategory = 0;
		m_ItemData.btEffectType1 = 0;
		m_ItemData.btEffectType2 = 0;
		m_ItemData.dwDuration = dwDuration;
		m_ItemData.lItemBuyDate = this->GetCurrentDate();
		m_ItemData.lItemExpireDate = this->GetExpireDate(dwDuration);
	}

	else
	{
		LPBUFF_EFFECT_DATE lpBuffEffect = g_BuffEffectSlot.GetEffectDataFromItemCode(wItemCode);
		LPITEMEFFECT lpItemEffect = g_ItemAddOption.GetItemEffectData(wItemCode);

		if ( !lpBuffEffect || !lpItemEffect )
		{
			return FALSE;
		}

		if ( !gObjAddBuffEffectForInGameShop(lpObj, wItemCode, dwDuration) )
		{
			return FALSE;
		}

		m_ItemData.Clear();
		m_ItemData.wItemCode = wItemCode;
		m_ItemData.btItemType = PERIODITEM_BUFF;
		m_ItemData.Serial = 0;
		m_ItemData.btEffectCategory = lpBuffEffect->BuffEffectType;
		m_ItemData.btEffectType1 = (BYTE)lpItemEffect->wEffectType1;
		m_ItemData.btEffectType2 = (BYTE)lpItemEffect->wEffectType2;
		m_ItemData.dwDuration = dwDuration;
		m_ItemData.lItemBuyDate = this->GetCurrentDate();
		m_ItemData.lItemExpireDate = this->GetExpireDate(dwDuration);
	}

	//VM_TIGER_LONDON_END

	this->RequestPeriodItemInsert(lpObj, &m_ItemData, dwDuration);
	return TRUE;
}

BOOL CPeriodItemEx::SetDisableItemToExpire(CGameObject &Obj, int pos)
{
	if ( !lpObj )
	{
		return FALSE;
	}

	if ( Obj.Connected < PLAYER_LOGGED )
	{
		return FALSE;
	}

	if ( Obj.Type != OBJ_USER )
	{
		return FALSE;
	}

	if ( pos == -1 )
	{
		return FALSE;
	}

	if ( Obj.pntInventory[pos]->IsPeriodItem() == FALSE )
	{
		return FALSE;
	}

	Obj.pntInventory[pos]->SetPeriodItemExpire();
	GCItemObjectListSend(Obj.m_Index);
	this->OnRequestPeriodItemList(lpObj);

	return TRUE;
}

void CPeriodItemEx::SetExpireNotMatchedData(CGameObject &Obj)
{
	if ( Obj.Type != OBJ_USER )
	{
		return;
	}

	if ( Obj.m_PlayerData->m_iPeriodItemEffectIndex == -1 )
	{
		return;
	}

	int ItemCount = 0;

	for (int i=0;i<INVENTORY_SIZE;i++)
	{
		if ( Obj.pntInventory[i]->IsItem() == TRUE &&
			Obj.pntInventory[i]->IsPeriodItem() == TRUE &&
			Obj.pntInventory[i]->IsPeriodItemExpire() == FALSE )
		{
			BOOL bItemFound = FALSE;

			for (int j=0;j<30;j++)
			{
				if ( this->m_PeriodData[Obj.m_PlayerData->m_iPeriodItemEffectIndex].m_ItemInfo[j].btUsedInfo == TRUE &&
					this->m_PeriodData[Obj.m_PlayerData->m_iPeriodItemEffectIndex].m_ItemInfo[j].wItemCode == Obj.pntInventory[i]->m_Type &&
					this->m_PeriodData[Obj.m_PlayerData->m_iPeriodItemEffectIndex].m_ItemInfo[j].Serial == Obj.pntInventory[i]->m_Number )
				{
					bItemFound = TRUE;
				}
			}

			if ( bItemFound == FALSE )
			{
				Obj.pntInventory[i]->SetPeriodItemExpire();
				ItemCount++;
			}
		}
	}

	if ( ItemCount > 0 )
	{
		GCItemObjectListSend(Obj.m_Index);
	}
}

void CPeriodItemEx::RequestPeriodItemInfo(CGameObject &Obj)
{
	PMSG_REQ_PERIODITEMEX_SELECT pMsg;

	pMsg.dwUserGuid = Obj.DBNumber;
	pMsg.wUserIndex = Obj.m_Index;
	std::memcpy(pMsg.chCharacterName, Obj.Name, MAX_ACCOUNT_LEN+1);
	PHeadSubSetB((BYTE*)&pMsg, 0xD0, 0x03, sizeof(pMsg));

	wsDataCli.DataSend((char *)&pMsg, sizeof(pMsg));
}

void CPeriodItemEx::RequestPeriodItemInsert(CGameObject &Obj, ITEM_DATA * lpItemData, DWORD dwDuration)
{
	PMSG_REQ_PERIODITEMEX_INSERT pMsg;

	pMsg.dwUserGuid = Obj.DBNumber;
	pMsg.wUserIndex = Obj.m_Index;
	std::memcpy(pMsg.chCharacterName, Obj.Name, MAX_ACCOUNT_LEN+1);

	pMsg.btItemType = lpItemData->btItemType;
	pMsg.wItemCode = lpItemData->wItemCode;
	pMsg.btEffectCategory = lpItemData->btEffectCategory;
	pMsg.btEffectType1 = lpItemData->btEffectType1;
	pMsg.btEffectType2 = lpItemData->btEffectType2;
	pMsg.Serial = lpItemData->Serial;
	pMsg.dwDuration = dwDuration;
	pMsg.lBuyDate = this->GetCurrentDate();
	pMsg.lExpireDate = this->GetExpireDate(dwDuration);

	PHeadSubSetB((BYTE*)&pMsg, 0xD0, 0x01, sizeof(pMsg));
	
	wsDataCli.DataSend((char *)&pMsg, sizeof(pMsg));
}

void CPeriodItemEx::RequestPeriodItemDelete(CGameObject &Obj, ITEM_DATA * lpItemData)
{
	PMSG_REQ_PERIODITEMEX_DELETE pMsg;

	pMsg.dwUserGuid = Obj.DBNumber;
	pMsg.wUserIndex = Obj.m_Index;
	std::memcpy(pMsg.chCharacterName, Obj.Name, MAX_ACCOUNT_LEN+1);

	pMsg.btItemType = lpItemData->btItemType;
	pMsg.wItemCode = lpItemData->wItemCode;
	pMsg.Serial = lpItemData->Serial;

	PHeadSubSetB((BYTE*)&pMsg, 0xD0, 0x02, sizeof(pMsg));
	sLog->outBasic("[PeriodItemEx][Delete PeriodItem] Request Delete PeriodItem Data. User Id : %s(%d), Name : %s, Item : %s(%d), Type : %d, Serial : %I64d",
		Obj.AccountID, Obj.DBNumber, Obj.Name, ItemAttribute[lpItemData->wItemCode].Name, lpItemData->wItemCode, lpItemData->btItemType, lpItemData->Serial);
	
	wsDataCli.DataSend((char *)&pMsg, sizeof(pMsg));
}

void CPeriodItemEx::DGPeriodItemExInsert(PMSG_ANS_PERIODITEMEX_INSERT * lpMsg)
{
	int UserIndex = lpMsg->wUserIndex;
	int pos = 0;

	if ( ObjectMaxRange(UserIndex) == FALSE )
	{
		return;
	}

	CGameObject lpObj = getGameObject(UserIndex);

	if ( Obj.Connected < PLAYER_LOGGED )
	{
		return;
	}

	if ( Obj.Type != OBJ_USER )
	{
		return;
	}

	if ( lpMsg->btResultCode == 0 )
	{
		
		if (lpMsg->btItemtype == 2)
		{
			this->SendPeriodItemInfoOnce(lpObj, lpMsg->Serial);
		}
	}
}

void CPeriodItemEx::DGPeriodItemExDelete(PMSG_ANS_PERIODITEMEX_DELETE * lpMsg)
{
	int UserIndex = lpMsg->wUserIndex;

	if ( ObjectMaxRange(UserIndex) == FALSE )
	{
		return;
	}

	CGameObject lpObj = getGameObject(UserIndex);

	if ( Obj.Connected < PLAYER_LOGGED )
	{
		return;
	}

	if ( Obj.Type != OBJ_USER )
	{
		return;
	}

	if (lpMsg->btResultCode)
	{

	}
	else
	{
		sLog->outBasic("[PeriodItemEx][Delete PeriodItem] OnRequest Delete PeriodItem Data. User Id : %s(%d), Name : %s, Result : %d",
			Obj.AccountID, Obj.DBNumber, Obj.Name, lpMsg->btResultCode);
	}
}

void CPeriodItemEx::DGPeriodItemExExpiredItemList(PMSG_ANS_PERIODITEMEX_EXPIRED_ITEMLIST * lpMsg)
{
	int UserIndex = lpMsg->wUserIndex;

	if ( ObjectMaxRange(UserIndex) == FALSE )
	{
		return;
	}

	CGameObject lpObj = getGameObject(UserIndex);

	if ( Obj.Connected < PLAYER_LOGGED )
	{
		return;
	}

	if ( Obj.Type != OBJ_USER )
	{
		return;
	}

	int pos = 0;

	for ( int i = 0; i < lpMsg->btExpiredItemCount; i++ )
	{
		pos = this->GetItemFromInventory(lpObj, lpMsg->wItemCode[i], lpMsg->Serial[i]);

		if ( pos != -1 )
		{
			sLog->outBasic("[PeriodItemEx][SetExpiredItemList] OnRequest Expired Item Set Disable. User Id : %s(%d), Name : %s, Item : %s(%d), Serial : %I64d",
				Obj.AccountID, Obj.DBNumber, Obj.Name, ItemAttribute[lpMsg->wItemCode[i]].Name, lpMsg->wItemCode[i], lpMsg->Serial[i]);

			this->SetDisableItemToExpire(lpObj, pos);
		}
	}
}

void CPeriodItemEx::DGPeriodItemExListCount(PMSG_ANS_PERIODITEMEX_LIST_COUNT * lpMsg)
{
	int UserIndex = lpMsg->wUserIndex;

	if ( ObjectMaxRange(UserIndex) == FALSE )
	{
		return;
	}

	CGameObject lpObj = getGameObject(UserIndex);

	if ( Obj.Connected < PLAYER_LOGGED )
	{
		return;
	}

	if ( Obj.Type != OBJ_USER )
	{
		return;
	}

	if ( lpMsg->dwUserGuid == Obj.DBNumber )
	{
		this->SetExpireNotMatchedData(lpObj);
		this->OnRequestPeriodItemList(lpObj);
	}
}

void CPeriodItemEx::DGPeriodItemExList(PMSG_ANS_PERIODITEMEX_LIST * lpMsg)
{
	int UserIndex = lpMsg->wUserIndex;

	if ( ObjectMaxRange(UserIndex) == FALSE )
	{
		return;
	}

	CGameObject lpObj = getGameObject(UserIndex);

	if ( Obj.Connected < PLAYER_LOGGED )
	{
		return;
	}

	if ( Obj.Type != OBJ_USER )
	{
		return;
	}

	time_t LeftDate = this->GetLeftDate(lpMsg->lItemExpireDate);

	if ( lpMsg->btItemType == PERIODITEM_BUFF )
	{
		if ( gObjAddBuffEffectForInGameShop(lpObj, lpMsg->wItemCode, LeftDate) == true )
		{
		
		}

		else
		{
		
		}
	}

	else if ( lpMsg->btItemType == PERIODITEM_NORMAL )
	{
		if ( this->AddPeriodItemData(lpObj, lpMsg->wItemCode, lpMsg->Serial, LeftDate) != -1 )
		{
		
		}

		else
		{
		
		}
	}
}

int CPeriodItemEx::GetItemFromInventory(CGameObject &Obj, WORD wItemCode, UINT64 Serial)
{
	int pos = -1;

	if ( lpObj == NULL ) return -1;
	if ( Obj.Connected < PLAYER_LOGGED ) return -1;
	if ( Obj.Type != OBJ_USER ) return -1;

	for ( int i=0;i<INVENTORY_SIZE;i++ )
	{
		if (Obj.pntInventory[i]->IsItem())
		{
			if (Obj.pntInventory[i]->m_Type == wItemCode)
			{
				if (Obj.pntInventory[i]->GetNumber() == Serial)
				{
					pos = i;
					break;
				}
			}
		}
	}

	return pos;
}

void CPeriodItemEx::OnRequestPeriodItemList(CGameObject &Obj)
{
	PMSG_PROPERTYITEMCOUNT pMsg1;
	PMSG_PROPERTYITEMINFO pMsg2;

	if ( Obj.Type != OBJ_USER )
		return;

	int PeriodIndex = Obj.m_PlayerData->m_iPeriodItemEffectIndex;

	if (PeriodIndex == -1)
		return;

	PHeadSubSetB((BYTE*)&pMsg1, 0xD2, 0x11, sizeof(pMsg1));
	PHeadSubSetB((BYTE*)&pMsg2, 0xD2, 0x12, sizeof(pMsg2));
	
	pMsg1.btCount = this->m_PeriodData[PeriodIndex].btItemCount;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg1, sizeof(pMsg1));

	if ( this->m_PeriodData[PeriodIndex].btUsedInfo == TRUE )
	{
		for ( int i = 0; i < 30; i++ )
		{
			if ( this->m_PeriodData[PeriodIndex].m_ItemInfo[i].btUsedInfo == TRUE )
			{
				pMsg2.ItemID = this->m_PeriodData[PeriodIndex].m_ItemInfo[i].wItemCode;
				pMsg2.Pos = this->GetItemFromInventory(lpObj, this->m_PeriodData[PeriodIndex].m_ItemInfo[i].wItemCode, this->m_PeriodData[PeriodIndex].m_ItemInfo[i].Serial);
				pMsg2.Time = this->m_PeriodData[PeriodIndex].m_ItemInfo[i].lItemExpireDate;

				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg2, sizeof(pMsg2));
			}
		}
	}
}

void CPeriodItemEx::OnRequestPeriodItemListOnce(CGameObject &Obj, ITEM_DATA * lpItemData)
{
	if ( lpObj == NULL ) return;
	if ( lpItemData == NULL ) return;

	PMSG_PROPERTYITEMINFO pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xD2, 0x12, sizeof(pMsg));

	pMsg.ItemID = lpItemData->wItemCode;
	pMsg.Pos = this->GetItemFromInventory(lpObj, lpItemData->wItemCode, lpItemData->Serial);
	pMsg.Time = lpItemData->lItemExpireDate;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}

BOOL CPeriodItemEx::SendPeriodItemInfoOnce(CGameObject &Obj, CItemObject * Item)
{
	if ( Obj.Type != OBJ_USER )
	{
		return FALSE;
	}

	if ( Item == NULL )
	{
		return FALSE;
	}

	int PeriodIndex = Obj.m_PlayerData->m_iPeriodItemEffectIndex;

	if ( PeriodIndex == -1 )
	{
		return FALSE;
	}

	if ( this->m_PeriodData[PeriodIndex].btUsedInfo == TRUE && this->m_PeriodData[PeriodIndex].dwUserGuid == Obj.DBNumber )
	{
		for ( int i=0; i<30; i++ )
		{
			if ( this->m_PeriodData[PeriodIndex].m_ItemInfo[i].btUsedInfo == TRUE && this->m_PeriodData[PeriodIndex].m_ItemInfo[i].Serial == Item->GetNumber() )
			{
				this->OnRequestPeriodItemListOnce(lpObj, &this->m_PeriodData[PeriodIndex].m_ItemInfo[i]);
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CPeriodItemEx::SendPeriodItemInfoOnce(CGameObject &Obj, UINT64 Serial)
{
	if ( Obj.Type != OBJ_USER )
	{
		return FALSE;
	}

	if ( Serial == 0 )
	{
		return FALSE;
	}

	int PeriodIndex = Obj.m_PlayerData->m_iPeriodItemEffectIndex;

	if ( PeriodIndex == -1 )
	{
		return FALSE;
	}

	if ( this->m_PeriodData[PeriodIndex].btUsedInfo == TRUE && this->m_PeriodData[PeriodIndex].dwUserGuid == Obj.DBNumber )
	{
		for ( int i=0; i<30; i++ )
		{
			if ( this->m_PeriodData[PeriodIndex].m_ItemInfo[i].btUsedInfo == TRUE && this->m_PeriodData[PeriodIndex].m_ItemInfo[i].Serial == Serial )
			{
				this->OnRequestPeriodItemListOnce(lpObj, &this->m_PeriodData[PeriodIndex].m_ItemInfo[i]);
				return TRUE;
			}
		}
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

