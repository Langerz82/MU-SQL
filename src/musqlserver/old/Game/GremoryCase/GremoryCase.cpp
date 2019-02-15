////////////////////////////////////////////////////////////////////////////////
// GremoryCase.cpp
#include "GremoryCase.h"
#include "Main.h"
#include "GameProtocol.h"
#include "ItemSocketOptionSystem.h"
#include "PentagramSystem.h"
#include "MuunSystem.h"
#include "SocketItemType.h"

CGremoryCase g_GremoryCase;

CGremoryCase::CGremoryCase(void)
{
}

CGremoryCase::~CGremoryCase(void)
{
}

void CGremoryCase::CheckStorageExpiredItems()
{
	time_t CurrTime = time(NULL);

	for each (std::pair<int,CGameObject*> ObjEntry in gGameObjects)
	{
		CGameObject* lpObj = ObjEntry.second;
		if (gObjIsConnected(i) == TRUE)
		{
			if (getGameObject(i)->Type == OBJ_USER)
			{
				if (getGameObject(i)->m_PlayerData->m_bGremoryCaseDBInfo == true)
				{
					for (int j = 0; j < MAX_GREMORYCASE_STORAGE_TYPES; j++)
					{
						for (int k = 0; k < MAX_GREMORYCASE_STORAGE_ITEMS; k++)
						{
							if (getGameObject(i)->m_PlayerData->m_GremoryCaseData[j][k)->btRewardInventory != 0)
							{
								if (getGameObject(i)->m_PlayerData->m_GremoryCaseData[j][k)->dwExpireTime < CurrTime)
								{
									this->GDReqDeleteItemFromGremoryCase(i, getGameObject(i)->m_PlayerData->m_GremoryCaseData[j][k]->ItemInfo.m_Type, getGameObject(i)->m_PlayerData->m_GremoryCaseData[j][k]->dwItemGUID, getGameObject(i)->m_PlayerData->m_GremoryCaseData[j)[k)->dwAuthCode);
									this->GCSendDeleteItemFromGremoryCase(i, j, k);

									getGameObject(i)->m_PlayerData->m_GremoryCaseData[j][k)->Clear();

								}
							}
						}
					}
				}
			}
		}
	}
}

void CGremoryCase::GDReqStorageItemList(CGameObject &Obj)
{
	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		return;
	}

	Obj.m_PlayerData->m_bGremoryCaseDBInfo = false;

	_stReqGremoryCaseItemList pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0x4F, 0x00, sizeof(pMsg));

	pMsg.iIndex = iIndex;
	std::memcpy(pMsg.szAccountID, Obj.AccountID, MAX_ACCOUNT_LEN+1);
	std::memcpy(pMsg.szName, Obj.Name, MAX_ACCOUNT_LEN+1);

	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);

}

void CGremoryCase::DGAnsStorageItemList(BYTE* lpRecv)
{
	_stAnsGremoryCaseItemList * lpMsg = (_stAnsGremoryCaseItemList *)(lpRecv);
	_stGremoryCaseItem * lpItem;

	int iIndex = lpMsg->iIndex;

	if ( !ObjectMaxRange(Obj.m_Index) )
	{
		return;
	}

	CGameObject* lpObj = Obj;

	if ( Obj.Type != OBJ_USER )
	{
		return;
	}

	int iCharacterStorageCount = 0;
	int iServerStorageCount = 0;

	BYTE SocketOption[5] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	BYTE SocketOptionIndex = 0xFF;

	for (int i = 0; i < lpMsg->btCount; i++ )
	{
		lpItem = (_stGremoryCaseItem *)(lpRecv + sizeof(_stAnsGremoryCaseItemList) + (i * sizeof(_stGremoryCaseItem)));

		for (int j = 0; j < MAX_GREMORYCASE_STORAGE_ITEMS; j++)
		{
			if ( Obj.m_PlayerData->m_GremoryCaseData[lpItem->btStorageType-1][j].btRewardInventory == 0)
			{
				Obj.m_PlayerData->m_GremoryCaseData[lpItem->btStorageType-1][j].Clear();
				Obj.m_PlayerData->m_GremoryCaseData[lpItem->btStorageType-1][j].ItemInfo.Clear();
				Obj.m_PlayerData->m_GremoryCaseData[lpItem->btStorageType-1][j].btRewardInventory = lpItem->btStorageType;
				Obj.m_PlayerData->m_GremoryCaseData[lpItem->btStorageType-1][j].btRewardSource = lpItem->btRewardSource;
				Obj.m_PlayerData->m_GremoryCaseData[lpItem->btStorageType-1][j].dwItemGUID = lpItem->btItemGUID;
				Obj.m_PlayerData->m_GremoryCaseData[lpItem->btStorageType-1][j].dwAuthCode = lpItem->dwAuthCode;
				Obj.m_PlayerData->m_GremoryCaseData[lpItem->btStorageType-1][j].dwReceiveDate = lpItem->iReceiveDate;
				Obj.m_PlayerData->m_GremoryCaseData[lpItem->btStorageType-1][j].dwExpireTime = lpItem->iExpireDate;

				Obj.m_PlayerData->m_GremoryCaseData[lpItem->btStorageType-1][j].ItemInfo.m_Type = lpItem->wItemID;
				Obj.m_PlayerData->m_GremoryCaseData[lpItem->btStorageType-1][j].ItemInfo.m_Level = lpItem->btItemLevel;
				Obj.m_PlayerData->m_GremoryCaseData[lpItem->btStorageType-1][j].ItemInfo.m_Durability = ItemGetDurability(lpItem->wItemID, lpItem->btItemLevel, lpItem->btItemExcOption, lpItem->btItemSetOption);

				if (gSocketItemType.CheckSocketItemType((int)&Obj.m_PlayerData->m_GremoryCaseData[lpItem->btStorageType-1][j].ItemInfo) == true)
				{
					//BYTE SocketOption[5] = { -1,-1,-1,-1,-1 };
					//BYTE SocketOption[5] = { -1,-1,-1,-1,-1 };
					int SocketSlotCount = 0;
					
					if (memcmp(lpItem->btItemSocketOption, SocketOption, 5) == 0)
					{
						SocketSlotCount = g_SocketOptionSystem.GetMakeSocketSlotCount(lpItem->wItemID);
					}

					else
					{
						for (int k = 0; k < 5; k++)
						{
							if (lpItem->btItemSocketOption[k] == 0xFE)
							{
								SocketSlotCount++;
							}
						}
					}

					SocketOptionIndex = -1;

					for (int k = 0; k < 5; k++)
					{
						if (k < SocketSlotCount)
						{
							SocketOption[k] = 0xFE;
						}
						else
						{
							SocketOption[k] = 0x00;
						}
					}
				}

				else if (g_PentagramSystem.IsPentagramItem(Obj.m_PlayerData->m_GremoryCaseData[lpItem->btStorageType-1][j].ItemInfo.m_Type) == true)
				{
					BYTE SocketOption[5] = { -1,-1,-1,-1,-1 };
					BYTE btEnableSlot[5];
					int SocketSlotCount = 0;

					if (memcmp(lpItem->btItemSocketOption, SocketOption, 5) == 0)
					{
						SocketSlotCount = g_PentagramSystem.GetMakePentagramSlotCountNKind(btEnableSlot, lpItem->wItemID);
					}

					else
					{
						for (int k = 0; k < 5; k++)
						{
							if (lpItem->btItemSocketOption[k] == 0xFE)
							{
								btEnableSlot[k] = TRUE;
								SocketSlotCount++;
							}

							else
							{
								btEnableSlot[k] = FALSE;
							}
						}
					}

					SocketOptionIndex = lpItem->btItemMainAttribute;

					for (int k = 0; k < 5; k++)
					{
						if (btEnableSlot[k])
						{
							SocketOption[k] = 0xFE;
						}

						else
						{
							SocketOption[k] = 0x00;
						}
					}
				}

				else if (g_CMuunSystem.IsStoneofEvolution(Obj.m_PlayerData->m_GremoryCaseData[lpItem->btStorageType-1][j].ItemInfo.m_Type) == TRUE)
				{
					int iMuunEvolutionItemID = ITEMGET(lpItem->wMuunEvoItemType, lpItem->btMuunEvoItemIndex);
				
					SocketOption[0] = iMuunEvolutionItemID >> 8;
					SocketOption[1] = ITEM_GET_INDEX(iMuunEvolutionItemID);
				}

				else
				{
					SocketOptionIndex = -1;

					for (int k = 0; k < 5; k++)
					{
						SocketOption[k] = lpItem->btItemSocketOption[k];
					}
				}

				Obj.m_PlayerData->m_GremoryCaseData[lpItem->btStorageType-1][j].ItemInfo.Convert(lpItem->wItemID, lpItem->btItemSkill,
					lpItem->btItemLuck, lpItem->btItemOption, lpItem->btItemExcOption, lpItem->btItemSetOption, 0, SocketOption, SocketOptionIndex, 0, 3);

				if ( lpItem->btStorageType == GC_STORAGE_SERVER )
				{
					iServerStorageCount++;
				}

				else if ( lpItem->btStorageType == GC_STORAGE_CHARACTER )
				{
					iCharacterStorageCount++;
				}

				break;
			}		
		}
	}

	Obj.m_PlayerData->m_bGremoryCaseDBInfo = true;
	
	this->GCSendStorageItemList(Obj.m_Index);
	this->CheckIsInStorageItemAboutToExpire(Obj.m_Index);
	this->CheckIsStorageFull(Obj.m_Index);
}

void CGremoryCase::GDReqAddItemToGremoryCase(CGameObject &Obj, _stGremoryCaseItem stItem, int iDaysToExpire)
{
	CGameObject* lpObj = Obj;

	if ( Obj.Type != OBJ_USER ) 
	{
		return;
	}

	if ( Obj.m_PlayerData->m_bGremoryCaseDBInfo == false )
	{
		return;
	}

	if (stItem.btStorageType != GC_STORAGE_SERVER && stItem.btStorageType != GC_STORAGE_CHARACTER)
	{
		return;
	}

	_stReqAddItemToGremoryCase pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0x4F, 0x01, sizeof(pMsg));

	pMsg.iIndex = iIndex;
	std::memcpy(pMsg.szAccountID, Obj.AccountID, MAX_ACCOUNT_LEN+1);
	std::memcpy(pMsg.szName, Obj.Name, MAX_ACCOUNT_LEN+1);

	pMsg.m_GremoryCaseItem.btStorageType = stItem.btStorageType;
	pMsg.m_GremoryCaseItem.btRewardSource = stItem.btRewardSource;
	pMsg.m_GremoryCaseItem.btItemGUID = 0;
	pMsg.m_GremoryCaseItem.dwAuthCode = 0;
	pMsg.m_GremoryCaseItem.wItemID = stItem.wItemID;
	pMsg.m_GremoryCaseItem.btItemLevel = stItem.btItemLevel;
	pMsg.m_GremoryCaseItem.btItemDurability = stItem.btItemDurability;
	pMsg.m_GremoryCaseItem.btItemSkill = stItem.btItemSkill;
	pMsg.m_GremoryCaseItem.btItemLuck = stItem.btItemLuck;
	pMsg.m_GremoryCaseItem.btItemOption = stItem.btItemOption;
	pMsg.m_GremoryCaseItem.btItemExcOption = stItem.btItemExcOption;
	pMsg.m_GremoryCaseItem.btItemSetOption = stItem.btItemSetOption;
	std::memcpy(pMsg.m_GremoryCaseItem.btItemSocketOption, stItem.btItemSocketOption, 5);
	pMsg.m_GremoryCaseItem.btItemMainAttribute = stItem.btItemMainAttribute;
	pMsg.m_GremoryCaseItem.wMuunEvoItemType = stItem.wMuunEvoItemType;
	pMsg.m_GremoryCaseItem.btMuunEvoItemIndex = stItem.btMuunEvoItemIndex;
	pMsg.m_GremoryCaseItem.iReceiveDate = time(NULL);
	pMsg.m_GremoryCaseItem.iExpireDate = time(NULL) + (iDaysToExpire * 24 * 60 * 60);

	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
}

void CGremoryCase::DGAnsAddItemToGremoryCase(_stAnsAddItemToGremoryCase * lpMsg)
{
	int iIndex = lpMsg->iIndex;

	if ( !ObjectMaxRange(Obj.m_Index) )
	{
		return;
	}

	CGameObject* lpObj = Obj;

	if ( Obj.Type != OBJ_USER )
	{
		return;
	}

	if ( lpMsg->btResult == 1 )
	{
		return;
	}

	int iItemArrayIndex = -1;
	BYTE SocketOption[5] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	BYTE SocketOptionIndex = 0xFF;

	for (int i = 0; i < MAX_GREMORYCASE_STORAGE_ITEMS; i++)
	{
		if (Obj.m_PlayerData->m_GremoryCaseData[lpMsg->m_GremoryCaseItem.btStorageType-1][i].btRewardInventory == 0)
		{
			iItemArrayIndex = i;
			Obj.m_PlayerData->m_GremoryCaseData[lpMsg->m_GremoryCaseItem.btStorageType-1][i].Clear();
			Obj.m_PlayerData->m_GremoryCaseData[lpMsg->m_GremoryCaseItem.btStorageType-1][i].ItemInfo.Clear();
			Obj.m_PlayerData->m_GremoryCaseData[lpMsg->m_GremoryCaseItem.btStorageType-1][i].btRewardInventory = lpMsg->m_GremoryCaseItem.btStorageType;
			Obj.m_PlayerData->m_GremoryCaseData[lpMsg->m_GremoryCaseItem.btStorageType-1][i].btRewardSource = lpMsg->m_GremoryCaseItem.btRewardSource;
			Obj.m_PlayerData->m_GremoryCaseData[lpMsg->m_GremoryCaseItem.btStorageType-1][i].dwItemGUID = lpMsg->m_GremoryCaseItem.btItemGUID;
			Obj.m_PlayerData->m_GremoryCaseData[lpMsg->m_GremoryCaseItem.btStorageType-1][i].dwAuthCode = lpMsg->m_GremoryCaseItem.dwAuthCode;
			Obj.m_PlayerData->m_GremoryCaseData[lpMsg->m_GremoryCaseItem.btStorageType-1][i].dwReceiveDate = lpMsg->m_GremoryCaseItem.iReceiveDate;
			Obj.m_PlayerData->m_GremoryCaseData[lpMsg->m_GremoryCaseItem.btStorageType-1][i].dwExpireTime = lpMsg->m_GremoryCaseItem.iExpireDate;

			Obj.m_PlayerData->m_GremoryCaseData[lpMsg->m_GremoryCaseItem.btStorageType-1][i].ItemInfo.m_Type = lpMsg->m_GremoryCaseItem.wItemID;
			Obj.m_PlayerData->m_GremoryCaseData[lpMsg->m_GremoryCaseItem.btStorageType-1][i].ItemInfo.m_Level = lpMsg->m_GremoryCaseItem.btItemLevel;
			Obj.m_PlayerData->m_GremoryCaseData[lpMsg->m_GremoryCaseItem.btStorageType-1][i].ItemInfo.m_Durability = ItemGetDurability(lpMsg->m_GremoryCaseItem.wItemID, lpMsg->m_GremoryCaseItem.btItemLevel, lpMsg->m_GremoryCaseItem.btItemExcOption, lpMsg->m_GremoryCaseItem.btItemSetOption);

			if (gSocketItemType.CheckSocketItemType((int)&Obj.m_PlayerData->m_GremoryCaseData[lpMsg->m_GremoryCaseItem.btStorageType - 1][i].ItemInfo) == true)
			{
				BYTE SocketOption[5] = { -1,-1,-1,-1,-1 };
				int SocketSlotCount = 0;

				if (memcmp(lpMsg->m_GremoryCaseItem.btItemSocketOption, SocketOption, 5) == 0)
				{
					SocketSlotCount = g_SocketOptionSystem.GetMakeSocketSlotCount(lpMsg->m_GremoryCaseItem.wItemID);
				}

				else
				{
					for (int k = 0; k < 5; k++)
					{
						if (lpMsg->m_GremoryCaseItem.btItemSocketOption[k] == 0xFE)
						{
							SocketSlotCount++;
						}
					}
				}

				SocketOptionIndex = -1;

				for (int k = 0; k < 5; k++)
				{
					if (k < SocketSlotCount)
					{
						SocketOption[k] = 0xFE;
					}
					else
					{
						SocketOption[k] = 0xFF;
					}
				}
			}

			else if (g_PentagramSystem.IsPentagramItem(Obj.m_PlayerData->m_GremoryCaseData[lpMsg->m_GremoryCaseItem.btStorageType - 1][i].ItemInfo.m_Type) == true)
			{
				BYTE SocketOption[5] = { -1,-1,-1,-1,-1 };
				BYTE btEnableSlot[5];
				int SocketSlotCount = 0;

				if (memcmp(lpMsg->m_GremoryCaseItem.btItemSocketOption, SocketOption, 5) == 0)
				{
					SocketSlotCount = g_PentagramSystem.GetMakePentagramSlotCountNKind(btEnableSlot, lpMsg->m_GremoryCaseItem.wItemID);
				}

				else
				{
					for (int k = 0; k < 5; k++)
					{
						if (lpMsg->m_GremoryCaseItem.btItemSocketOption[k] == 0xFE)
						{
							btEnableSlot[k] = TRUE;
							SocketSlotCount++;
						}

						else
						{
							btEnableSlot[k] = FALSE;
						}
					}
				}

				SocketOptionIndex = lpMsg->m_GremoryCaseItem.btItemMainAttribute;

				for (int k = 0; k < 5; k++)
				{
					if (btEnableSlot[k])
					{
						SocketOption[k] = 0xFE;
					}

					else
					{
						SocketOption[k] = 0xFF;
					}
				}
			}

			else if (g_CMuunSystem.IsStoneofEvolution(Obj.m_PlayerData->m_GremoryCaseData[lpMsg->m_GremoryCaseItem.btStorageType-1][i].ItemInfo.m_Type) == TRUE)
			{
				int iMuunEvolutionItemID = ITEMGET(lpMsg->m_GremoryCaseItem.wMuunEvoItemType, lpMsg->m_GremoryCaseItem.btMuunEvoItemIndex);
				
				SocketOption[0] = iMuunEvolutionItemID >> 8;
				SocketOption[1] = ITEM_GET_INDEX(iMuunEvolutionItemID);
			}

			else
			{
				SocketOptionIndex = -1;

				for (int k = 0; k < 5; k++)
				{
					SocketOption[k] = lpMsg->m_GremoryCaseItem.btItemSocketOption[k];
				}
			}

			Obj.m_PlayerData->m_GremoryCaseData[lpMsg->m_GremoryCaseItem.btStorageType-1][i].ItemInfo.Convert(lpMsg->m_GremoryCaseItem.wItemID, lpMsg->m_GremoryCaseItem.btItemSkill,
				lpMsg->m_GremoryCaseItem.btItemLuck, lpMsg->m_GremoryCaseItem.btItemOption, lpMsg->m_GremoryCaseItem.btItemExcOption, lpMsg->m_GremoryCaseItem.btItemSetOption, 0, SocketOption, SocketOptionIndex, 0, 3);
			break;
		}
	}
	
	if (iItemArrayIndex == -1)
	{
		return;
	}

	
	this->GCSendAddItemToGremoryCase(iIndex, lpMsg->m_GremoryCaseItem.btStorageType-1, iItemArrayIndex);
}

void CGremoryCase::GDReqCheckItemUseGremoryCase(CGameObject &Obj, WORD wItemID, DWORD dwItemGUID, DWORD dwAuthCode)
{
	_stReqCheckUseItemGremoryCase pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0x4F, 0x02, sizeof(pMsg));

	pMsg.iIndex = iIndex;
	std::memcpy(pMsg.szAccountID, Obj.AccountID, MAX_ACCOUNT_LEN+1);
	std::memcpy(pMsg.szName, Obj.Name, MAX_ACCOUNT_LEN+1);

	pMsg.wItemID = wItemID;
	pMsg.dwItemGUID = dwItemGUID;
	pMsg.dwAuthCode = dwAuthCode;

	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
}

void CGremoryCase::DGAnsCheckItemUseGremoryCase(_stAnsCheckUseItemGremoryCase * lpMsg)
{
	int iIndex = lpMsg->iIndex;

	if ( !ObjectMaxRange(Obj.m_Index) )
	{
		return;
	}

	CGameObject* lpObj = Obj;

	if ( Obj.Type != OBJ_USER )
	{
		return;
	}

	PMSG_ADD_GREMORYCASE_ITEM_TO_INVENTORY_ANS pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0x4F, 0x02, sizeof(pMsg));

	if ( Obj.m_PlayerData->m_bGremoryCaseDBInfo == false )
	{
		pMsg.btResult = 1;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

		return;
	}

	int iStorageType = -1;
	int iItemArrayIndex = -1;

	for (int i = 0; i < MAX_GREMORYCASE_STORAGE_TYPES; i++)
	{
		for (int j = 0; j < MAX_GREMORYCASE_STORAGE_ITEMS; j++)
		{
			if ( Obj.m_PlayerData->m_GremoryCaseData[i][j].btRewardInventory != 0)
			{
				if ( Obj.m_PlayerData->m_GremoryCaseData[i][j].ItemInfo.m_Type == lpMsg->wItemID &&
					Obj.m_PlayerData->m_GremoryCaseData[i][j].dwItemGUID == lpMsg->dwItemGUID &&
					Obj.m_PlayerData->m_GremoryCaseData[i][j].dwAuthCode == lpMsg->dwAuthCode)
				{
					iStorageType = i;
					iItemArrayIndex = j;
					break;
				}
			}
		}
	}

	if ( iStorageType == -1 || iItemArrayIndex == -1 )
	{
		pMsg.btResult = 2;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

		return;
	}

	if ( lpMsg->btResult == 0 )
	{
		pMsg.btResult = 3;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

		return;
	}

	LPITEM_ATTRIBUTE p = GetItemAttr(lpMsg->wItemID);
	bool IsInventoryEmptySpace;

	if ( p->ItemKindA == 11 )
	{
		IsInventoryEmptySpace = CheckEventInventoryEmptySpace(lpObj, p->Height, p->Width);
	}

	else if ( p->ItemKindA == 12 )
	{
		IsInventoryEmptySpace = gObjChkMuunInventoryEmpty(lpObj);
	}

	else
	{
		IsInventoryEmptySpace = CheckInventoryEmptySpace(lpObj, p->Height, p->Width);
	}

	if ( IsInventoryEmptySpace == false )
	{
		pMsg.btResult = 4;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

		return;
	}

	if (Obj.m_IfState->use)
	{
		pMsg.btResult = 5;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

		return;
	}

	if (p->ItemKindA == 11)
	{
			ItemCreate(iIndex, 226, 0, 0, Obj.m_PlayerData->m_GremoryCaseData[iStorageType][iItemArrayIndex].ItemInfo.m_Type, Obj.m_PlayerData->m_GremoryCaseData[iStorageType][iItemArrayIndex].ItemInfo.m_Level,
			Obj.m_PlayerData->m_GremoryCaseData[iStorageType][iItemArrayIndex].ItemInfo.m_Durability, Obj.m_PlayerData->m_GremoryCaseData[iStorageType][iItemArrayIndex].ItemInfo.m_Option1,
			Obj.m_PlayerData->m_GremoryCaseData[iStorageType][iItemArrayIndex].ItemInfo.m_Option2, Obj.m_PlayerData->m_GremoryCaseData[iStorageType][iItemArrayIndex].ItemInfo.m_Option3,
			iIndex, Obj.m_PlayerData->m_GremoryCaseData[iStorageType][iItemArrayIndex].ItemInfo.m_NewOption, Obj.m_PlayerData->m_GremoryCaseData[iStorageType][iItemArrayIndex].ItemInfo.m_SetOption, 0,
			Obj.m_PlayerData->m_GremoryCaseData[iStorageType][iItemArrayIndex].ItemInfo.m_SocketOption, Obj.m_PlayerData->m_GremoryCaseData[iStorageType][iItemArrayIndex].ItemInfo.m_BonusSocketOption);
			
	}

	else if (p->ItemKindA == 12)
	{
	BYTE SocketOption[5] = { -1, -1, -1, -1, -1 };
	
		ItemCreate(iIndex, 224, 0, 0, Obj.m_PlayerData->m_GremoryCaseData[iStorageType][iItemArrayIndex].ItemInfo.m_Type, Obj.m_PlayerData->m_GremoryCaseData[iStorageType][iItemArrayIndex].ItemInfo.m_Level,
			Obj.m_PlayerData->m_GremoryCaseData[iStorageType][iItemArrayIndex].ItemInfo.m_Durability, Obj.m_PlayerData->m_GremoryCaseData[iStorageType][iItemArrayIndex].ItemInfo.m_Option1,
			Obj.m_PlayerData->m_GremoryCaseData[iStorageType][iItemArrayIndex].ItemInfo.m_Option2, Obj.m_PlayerData->m_GremoryCaseData[iStorageType][iItemArrayIndex].ItemInfo.m_Option3,
			iIndex, Obj.m_PlayerData->m_GremoryCaseData[iStorageType][iItemArrayIndex].ItemInfo.m_NewOption, Obj.m_PlayerData->m_GremoryCaseData[iStorageType][iItemArrayIndex].ItemInfo.m_SetOption, 0,
			SocketOption, -1);
	}

	else
	{
		ItemCreate(iIndex, 235, 0, 0, Obj.m_PlayerData->m_GremoryCaseData[iStorageType][iItemArrayIndex].ItemInfo.m_Type, Obj.m_PlayerData->m_GremoryCaseData[iStorageType][iItemArrayIndex].ItemInfo.m_Level,
			Obj.m_PlayerData->m_GremoryCaseData[iStorageType][iItemArrayIndex].ItemInfo.m_Durability, Obj.m_PlayerData->m_GremoryCaseData[iStorageType][iItemArrayIndex].ItemInfo.m_Option1,
			Obj.m_PlayerData->m_GremoryCaseData[iStorageType][iItemArrayIndex].ItemInfo.m_Option2, Obj.m_PlayerData->m_GremoryCaseData[iStorageType][iItemArrayIndex].ItemInfo.m_Option3,
			iIndex, Obj.m_PlayerData->m_GremoryCaseData[iStorageType][iItemArrayIndex].ItemInfo.m_NewOption, Obj.m_PlayerData->m_GremoryCaseData[iStorageType][iItemArrayIndex].ItemInfo.m_SetOption, 0,
			Obj.m_PlayerData->m_GremoryCaseData[iStorageType][iItemArrayIndex].ItemInfo.m_SocketOption, Obj.m_PlayerData->m_GremoryCaseData[iStorageType][iItemArrayIndex].ItemInfo.m_BonusSocketOption);
	}

	pMsg.btResult = 0;
	pMsg.btStorageType = iStorageType+1;
	pMsg.wItemID = lpMsg->wItemID;
	pMsg.dwItemGUID = lpMsg->dwItemGUID;
	pMsg.dwAuthCode = lpMsg->dwAuthCode;

	this->GDReqDeleteItemFromGremoryCase(iIndex, lpMsg->wItemID, lpMsg->dwItemGUID, lpMsg->dwAuthCode);
	Obj.m_PlayerData->m_GremoryCaseData[iStorageType][iItemArrayIndex].Clear();

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}

void CGremoryCase::GDReqDeleteItemFromGremoryCase(CGameObject &Obj, WORD wItemID, DWORD dwItemGUID, DWORD dwAuthCode)
{
	_stReqDeleteItemFromGremoryCase pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0x4F, 0x03, sizeof(pMsg));

	std::memcpy(pMsg.szAccountID, Obj.AccountID, MAX_ACCOUNT_LEN+1);
	std::memcpy(pMsg.szName, Obj.Name, MAX_ACCOUNT_LEN+1);

	pMsg.wItemID = wItemID;
	pMsg.dwItemGUID = dwItemGUID;
	pMsg.dwAuthCode = dwAuthCode;

	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
}

void CGremoryCase::GCSendStorageItemList(CGameObject &Obj)
{
	CGameObject* lpObj = Obj;

	if ( Obj.Type != OBJ_USER ) 
	{
		return;
	}

	if ( Obj.m_PlayerData->m_bGremoryCaseDBInfo == false )
	{
		return;
	}

	BYTE BUFFER[8192];
	PMSG_RECEIVE_GREMORYCASE_ITEMLIST * lpMsg = (PMSG_RECEIVE_GREMORYCASE_ITEMLIST *)(BUFFER);
	PMSG_GREMORYCASE_ITEM * lpItem;

	lpMsg->btCount = 0;

	for (int i = 0; i < MAX_GREMORYCASE_STORAGE_TYPES; i++)
	{
		for (int j = 0; j < MAX_GREMORYCASE_STORAGE_ITEMS; j++)
		{
			if (Obj.m_PlayerData->m_GremoryCaseData[i][j].btRewardInventory != 0)
			{
				lpItem = (PMSG_GREMORYCASE_ITEM *)(BUFFER + sizeof(PMSG_RECEIVE_GREMORYCASE_ITEMLIST) + (lpMsg->btCount * sizeof(PMSG_GREMORYCASE_ITEM)));

				lpItem->btRewardInventory = Obj.m_PlayerData->m_GremoryCaseData[i][j].btRewardInventory;
				lpItem->btRewardSource = Obj.m_PlayerData->m_GremoryCaseData[i][j].btRewardSource;
				lpItem->dwItemGUID = Obj.m_PlayerData->m_GremoryCaseData[i][j].dwItemGUID;
				lpItem->dwAuthCode = Obj.m_PlayerData->m_GremoryCaseData[i][j].dwAuthCode;
				lpItem->dwExpireTime = Obj.m_PlayerData->m_GremoryCaseData[i][j].dwExpireTime;		
				ItemByteConvert(lpItem->btItemInfo, Obj.m_PlayerData->m_GremoryCaseData[i][j].ItemInfo);

				lpMsg->btCount++;
			}
		}
	}

	int PacketSize = sizeof(PMSG_RECEIVE_GREMORYCASE_ITEMLIST) + (lpMsg->btCount * sizeof(PMSG_GREMORYCASE_ITEM));
	PHeadSubSetW((BYTE*)lpMsg, 0x4F, 0x00, PacketSize);

	IOCP.DataSend(Obj.m_Index, BUFFER, PacketSize);
}

void CGremoryCase::GCSendAddItemToGremoryCase(CGameObject &Obj, BYTE btStorageType, BYTE btItemArrayIndex)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	if (!gObjIsConnected(Obj.m_Index))
	{
		return;
	}

	CGameObject* lpObj = Obj;

	if ( Obj.Type != OBJ_USER ) 
	{
		return;
	}

	PMSG_RECEIVE_GREMORYCASE_ITEM pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0x4F, 0x01, sizeof(pMsg));

	pMsg.m_ReceivedItem.btRewardInventory = Obj.m_PlayerData->m_GremoryCaseData[btStorageType][btItemArrayIndex].btRewardInventory;
	pMsg.m_ReceivedItem.btRewardSource = Obj.m_PlayerData->m_GremoryCaseData[btStorageType][btItemArrayIndex].btRewardSource;
	pMsg.m_ReceivedItem.dwItemGUID = Obj.m_PlayerData->m_GremoryCaseData[btStorageType][btItemArrayIndex].dwItemGUID;
	pMsg.m_ReceivedItem.dwAuthCode = Obj.m_PlayerData->m_GremoryCaseData[btStorageType][btItemArrayIndex].dwAuthCode;
	pMsg.m_ReceivedItem.dwExpireTime = Obj.m_PlayerData->m_GremoryCaseData[btStorageType][btItemArrayIndex].dwExpireTime;		
	ItemByteConvert(pMsg.m_ReceivedItem.btItemInfo, Obj.m_PlayerData->m_GremoryCaseData[btStorageType][btItemArrayIndex].ItemInfo);

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

	this->GCSendStorageItemList(Obj.m_Index);
}

void CGremoryCase::CGReqGetItemFromGremoryCase(PMSG_ADD_GREMORYCASE_ITEM_TO_INVENTORY_REQ * lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	if (!gObjIsConnected(Obj.m_Index))
	{
		return;
	}

	CGameObject* lpObj = Obj;

	if ( Obj.Type != OBJ_USER ) 
	{
		return;
	}

	this->GDReqCheckItemUseGremoryCase(iIndex, lpMsg->wItemID, lpMsg->dwItemGUID, lpMsg->dwAuthCode);
}

void CGremoryCase::GCSendDeleteItemFromGremoryCase(CGameObject &Obj, BYTE btStorageType, int iItemArrayIndex)
{
	PMSG_REMOVE_ITEM_FROM_GREMORYCASE pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0x4F, 0x03, sizeof(pMsg));

	pMsg.btStorageType = btStorageType+1;
	pMsg.wItemID = Obj.m_PlayerData->m_GremoryCaseData[btStorageType][iItemArrayIndex]->ItemInfo.m_Type;
	pMsg.dwItemGUID = Obj.m_PlayerData->m_GremoryCaseData[btStorageType][iItemArrayIndex]->dwItemGUID;
	pMsg.dwAuthCode =  Obj.m_PlayerData->m_GremoryCaseData[btStorageType][iItemArrayIndex]->dwAuthCode;

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}

void CGremoryCase::CGReqOpenGremoryCase(CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	if (!gObjIsConnected(Obj.m_Index))
	{
		return;
	}

	CGameObject* lpObj = Obj;

	if ( Obj.Type != OBJ_USER ) 
	{
		return;
	}

	PMSG_GREMORYCASE_OPEN_ANS pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0x4F, 0x05, sizeof(pMsg));

	if ( Obj.m_PlayerData->m_bGremoryCaseDBInfo == false )
	{
		pMsg.btResult = 1;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

		return;
	}

	pMsg.btResult = 0;
	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

	//this->GDReqStorageItemList(Obj.m_Index);
	//this->GCSendStorageItemList(Obj.m_Index);

}

void CGremoryCase::CheckIsInStorageItemAboutToExpire(CGameObject &Obj)
{
	CGameObject* lpObj = Obj;

	if ( Obj.Type != OBJ_USER ) 
	{
		return;
	}

	if ( Obj.m_PlayerData->m_bGremoryCaseDBInfo == false )
	{
		return;
	}

	bool bItemFound = false;

	time_t TimeCheck = time(NULL) + (7 * 24 * 60 * 60); // add 7 days to date

	for (int i = 0; i < MAX_GREMORYCASE_STORAGE_TYPES; i++)
	{
		for (int j = 0; j < MAX_GREMORYCASE_STORAGE_ITEMS; j++)
		{
			if (Obj.m_PlayerData->m_GremoryCaseData[i][j].btRewardInventory != 0)
			{
				if (Obj.m_PlayerData->m_GremoryCaseData[i][j].dwExpireTime < TimeCheck)
				{
					bItemFound = true;
					break;
				}
			}
		}
	}

	if (bItemFound == false)
	{
		return;
	}

	PMSG_GREMORYCASE_SENT_NOTICE pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0x4F, 0x04, sizeof(pMsg));
	pMsg.btNoticeIndex = 1;

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}

void CGremoryCase::CheckIsStorageFull(CGameObject &Obj)
{
	CGameObject* lpObj = Obj;

	if ( Obj.Type != OBJ_USER ) 
	{
		return;
	}

	if ( Obj.m_PlayerData->m_bGremoryCaseDBInfo == false )
	{
		return;
	}

	BYTE btStorageState = 0;
	int iCharacterStorageCount = 0, iServerStorageCount = 0;

	for (int i = 0; i < MAX_GREMORYCASE_STORAGE_TYPES; i++)
	{
		for (int j = 0; j < MAX_GREMORYCASE_STORAGE_ITEMS; j++)
		{
			if (Obj.m_PlayerData->m_GremoryCaseData[i][j].btRewardInventory != 0)
			{
				if (Obj.m_PlayerData->m_GremoryCaseData[i][j].btRewardInventory == GC_STORAGE_SERVER)
				{
					iServerStorageCount++;
				}

				else if (Obj.m_PlayerData->m_GremoryCaseData[i][j].btRewardInventory == GC_STORAGE_CHARACTER)
				{
					iCharacterStorageCount++;
				}
			}
		}
	}

	if (iServerStorageCount >= MAX_GREMORYCASE_STORAGE_ITEMS || iCharacterStorageCount >= MAX_GREMORYCASE_STORAGE_ITEMS)
	{
		btStorageState = 1;
	}

	else if ((iServerStorageCount >= MAX_GREMORYCASE_STORAGE_ITEMS-5 && iServerStorageCount < MAX_GREMORYCASE_STORAGE_ITEMS) || (iCharacterStorageCount >= MAX_GREMORYCASE_STORAGE_ITEMS-5 && iCharacterStorageCount < MAX_GREMORYCASE_STORAGE_ITEMS))
	{
		btStorageState = 2;
	}

	if ( btStorageState == 0 )
	{
		return;
	}

	PMSG_GREMORYCASE_SENT_NOTICE pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0x4F, 0x04, sizeof(pMsg));
	pMsg.btNoticeIndex = btStorageState == 2 ? 5 : 6;

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

