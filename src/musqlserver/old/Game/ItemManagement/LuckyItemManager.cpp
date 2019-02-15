////////////////////////////////////////////////////////////////////////////////
// LuckyItemManager.cpp
//#include "StdAfx.h"
#include "LargeRand.h"
#include "Logging/Log.h"
#include "ChaosBox.h"
#include "util.h"
#include "GameMain.h"
#include "SetItemOption.h"

#include "LuckyItemManager.h"
#include "configread.h"
#include "ItemOptionTypeMng.h"

LuckyItemManager	g_LuckyItemManager;

LuckyItemManager::LuckyItemManager()
{
	this->m_iTicketCnt = 0;
}

LuckyItemManager::~LuckyItemManager()
{

}

int	LuckyItemManager::LoadLuckyItemEquipment(int iType, _LUCKY_ITEM_INFO * pLuckyItemInfo, char * lpszFileName)
{
	int counter	= 0;
	_LUCKY_ITEM_EQUIPMENT * pLuckyItemEquipment = NULL;

	for( int i = 0; i < this->m_iTicketCnt; i++ )
	{
		if( pLuckyItemInfo[i].iItemKind == iType )
		{
			pLuckyItemEquipment = pLuckyItemInfo[i].LuckyItemEquipment;
		}
	}

	if( !pLuckyItemEquipment )
	{
		sLog->outError("Load LuckyItem Script Error ItemType %d", iType);
		return false;
	}

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(lpszFileName);

	if (res.status != pugi::status_ok)
	{
		sLog->outError("%s File Load Error (%s)", lpszFileName, res.description());
		return false;
	}

	pugi::xml_node mainXML = file.child("LuckyItem");
	pugi::xml_node ticket_list = mainXML.child("TicketItemList");

	for (pugi::xml_node list = ticket_list.child("List"); list; list = list.next_sibling())
	{
		int iListID = list.attribute("ID").as_int();

		if (iListID == iType)
		{
			for (pugi::xml_node item = list.child("Item"); item; item = item.next_sibling())
			{
				int iType = item.attribute("Cat").as_int();
				int iIndex = item.attribute("Index").as_int();

				pLuckyItemEquipment[counter].iItemNum = ITEMGET(iType, iIndex);
				pLuckyItemEquipment[counter].iSkill = item.attribute("Skill").as_int();
				pLuckyItemEquipment[counter].iLuckOption = item.attribute("Luck").as_int();
				pLuckyItemEquipment[counter].iAddOption = item.attribute("Option").as_int();
				pLuckyItemEquipment[counter].iClass = item.attribute("CharClass").as_int();

				counter++;
			}
		}
	}

	return true;
}

int LuckyItemManager::LoadLuckyItemInfo(char * lpszFileName)
{
	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(lpszFileName);

	if (res.status != pugi::status_ok)
	{
		sLog->outError("%s File Load Error (%s)", lpszFileName, res.description());
		return false;
	}
	
	this->m_iTicketCnt					= 0;
	int iTicketCount					= 0;
	_LUCKY_ITEM_INFO LuckyItemInfo[5];

	pugi::xml_node mainXML = file.child("LuckyItem");
	pugi::xml_node ticket = mainXML.child("TicketSettings");

	for (pugi::xml_node item = ticket.child("Item"); item; item = item.next_sibling())
	{
		int iType = item.attribute("Cat").as_int();
		int iIndex = item.attribute("Index").as_int();

		if (ItemGetNumberMake(iType, iIndex) == -1)
		{
			sLog->outError("Error: Wrong Item in %s file (Cat: %d Index: %d)", lpszFileName, iType, iIndex);
			continue;
		}

		LuckyItemInfo[iTicketCount].iItemNum = ITEMGET(iType, iIndex);
		LuckyItemInfo[iTicketCount].iItemKind = item.attribute("ListID").as_int();

		iTicketCount++;
		this->m_iTicketCnt = iTicketCount;
	}

	pugi::xml_node ticket_list = mainXML.child("TicketItemList");

	for (pugi::xml_node list = ticket_list.child("List"); list; list = list.next_sibling())
	{
		int iListID = list.attribute("ID").as_int();

		if (!this->LoadLuckyItemEquipment(iListID, LuckyItemInfo, lpszFileName))
		{
			sLog->outError("Error - (%s) LoadLuckyItemEquipment fail (ListID: %d)", lpszFileName, iListID);
			return false;
		}
	}

	pugi::xml_node item_level = mainXML.child("ItemLevelDrawSettings");

	for (pugi::xml_node list = item_level.child("List"); list; list = list.next_sibling())
	{
		int iListID = list.attribute("ID").as_int() - 1;

		if (iListID < 0 || iListID >= 5)
		{
			sLog->outError("Error: %s file - wrong List ID (%d)", lpszFileName, list.attribute("ID").as_int());
			continue;
		}

		int iCount = 0;

		for (pugi::xml_node item = list.child("Item"); item; item = item.next_sibling())
		{
			LuckyItemInfo[iListID].ItemLevelRate[iCount].iLevel = item.attribute("Level").as_int();
			LuckyItemInfo[iListID].ItemLevelRate[iCount].iRate = item.attribute("SuccessRate").as_int();
			iCount++;
		}
	}

	pugi::xml_node item_option = mainXML.child("ItemOptionDrawSettings");

	for (pugi::xml_node list = item_option.child("List"); list; list = list.next_sibling())
	{
		int iListID = list.attribute("ID").as_int() - 1;

		if (iListID < 0 || iListID >= 5)
		{
			sLog->outError("Error: %s file - wrong List ID (%d)", lpszFileName, list.attribute("ID").as_int());
			continue;
		}

		int iCount = 0;

		for (pugi::xml_node option = list.child("Option"); option; option = option.next_sibling())
		{
			LuckyItemInfo[iListID].iAddOpt3Rate[iCount] = option.attribute("SuccessRate").as_int();
			iCount++;
		}
	}

	pugi::xml_node set_option = mainXML.child("SetItemDrawSettings");

	for (pugi::xml_node list = set_option.child("List"); list; list = list.next_sibling())
	{
		int iListID = list.attribute("ID").as_int() - 1;

		if (iListID < 0 || iListID >= 5)
		{
			sLog->outError("Error: %s file - wrong List ID (%d)", lpszFileName, list.attribute("ID").as_int());
			continue;
		}

		LuckyItemInfo[iListID].iAddSetOptRate[0] = list.attribute("TierI").as_int();
		LuckyItemInfo[iListID].iAddSetOptRate[1] = list.attribute("TierII").as_int();
	}

	pugi::xml_node smelt_option = mainXML.child("SmeltDrawSettings");

	for (pugi::xml_node list = smelt_option.child("List"); list; list = list.next_sibling())
	{
		int iListID = list.attribute("ID").as_int() - 1;

		if (iListID < 0 || iListID >= 5)
		{
			sLog->outError("Error: %s file - wrong List ID (%d)", lpszFileName, list.attribute("ID").as_int());
			continue;
		}

		int iCount = 0;

		for (pugi::xml_node option = list.child("Range"); option; option = option.next_sibling())
		{
			LuckyItemInfo[iListID].SmeltRate[iCount].iMinDurab = option.attribute("MinDurability").as_int();
			LuckyItemInfo[iListID].SmeltRate[iCount].iMaxDurab = option.attribute("MaxDurability").as_int();
			LuckyItemInfo[iListID].SmeltRate[iCount].iSmeltRate = option.attribute("SmeltRate").as_int();
			iCount++;
		}
	}

	this->AddLuckyItemInfo(LuckyItemInfo, iTicketCount);
	return true;
}

void LuckyItemManager::AddLuckyItemInfo(_LUCKY_ITEM_INFO * pLuckyItemInfo, int iItemCnt)
{
	for( int i = 0; i < iItemCnt; i++ )
	{
		int iIndexID = pLuckyItemInfo[i].iItemNum;
		this->m_Map_LuckyItemInfo[iIndexID] = pLuckyItemInfo[i];
	}
}

_LUCKY_ITEM_INFO * LuckyItemManager::GetLuckyItemInfo(int iItemNum)
{
	std::map<int, _LUCKY_ITEM_INFO>::iterator Iter = this->m_Map_LuckyItemInfo.find(iItemNum);
	
	if( Iter == this->m_Map_LuckyItemInfo.end() )
	{
		return NULL;
	}
	
	return &Iter->second;
}

_LUCKY_ITEM_INFO * LuckyItemManager::GetCharClassLuckyItemInfo(int iItemNum, int iCharClass)
{
	if( iItemNum < 0 )
	{
		return NULL;
	}
	
	_LUCKY_ITEM_INFO * pLuckyItemInfo = this->GetLuckyItemInfo(iItemNum);
	
	if( !pLuckyItemInfo )
	{
		return NULL;
	}
	
	for( int i = 0; i < MAX_LUCKYINFO_ITEMEQUIPMENT; i++ )
	{
		if( pLuckyItemInfo->LuckyItemEquipment[i].iClass == iCharClass )
		{
			return pLuckyItemInfo;
		}
	}
	
	return NULL;
}

_LUCKY_ITEM_EQUIPMENT * LuckyItemManager::GetLuckyItemEquipment(_LUCKY_ITEM_INFO *const pLuckyItemInfo, int iCharClass)
{
	if( !pLuckyItemInfo )
	{
		return NULL;
	}
	
	for( int i = 0; i < MAX_LUCKYINFO_ITEMEQUIPMENT; i++ )
	{
		if( pLuckyItemInfo->LuckyItemEquipment[i].iClass == iCharClass )
		{
			return &pLuckyItemInfo->LuckyItemEquipment[i];
		}
	}
	
	return NULL;
}

int LuckyItemManager::IsLuckyItemTicket(int iItemNum)
{
	if( iItemNum < 0 )
	{
		return false;
	}
	
	std::map<int, _LUCKY_ITEM_INFO>::iterator Iter = this->m_Map_LuckyItemInfo.find(iItemNum);
	
	if( Iter == this->m_Map_LuckyItemInfo.end() )
	{
		return false;
	}
	
	return true;
}

int LuckyItemManager::IsLuckyItemEquipment(int iItemNum)
{
	if( iItemNum < 0 )
	{
		return false;
	}
	
	for( std::map<int, _LUCKY_ITEM_INFO>::iterator Iter = this->m_Map_LuckyItemInfo.begin(); Iter != this->m_Map_LuckyItemInfo.end(); Iter++ )
	{
		_LUCKY_ITEM_INFO * pLuckyItemInfo = &Iter->second;
		
		for( int i = 0; i < MAX_LUCKYINFO_ITEMEQUIPMENT; i++ )
		{
			if( pLuckyItemInfo->LuckyItemEquipment[i].iItemNum == iItemNum )
			{
				return true;
			}
		}
	}
	
	return false;
}

_SMELT_RATE * LuckyItemManager::GetSmeltingRateInfo(int iItemNum)
{
	if( iItemNum < 0 )
	{
		return false;
	}
	
	for( std::map<int, _LUCKY_ITEM_INFO>::iterator Iter = this->m_Map_LuckyItemInfo.begin(); Iter != this->m_Map_LuckyItemInfo.end(); Iter++ )
	{
		_LUCKY_ITEM_INFO * pLuckyItemInfo = &Iter->second;
		
		for( int i = 0; i < MAX_LUCKYINFO_ITEMEQUIPMENT; i++ )
		{
			if( pLuckyItemInfo->LuckyItemEquipment[i].iItemNum == iItemNum )
			{
				return pLuckyItemInfo->SmeltRate;
			}
		}
	}
	
	return false;
}

void LuckyItemManager::LuckyItemTicketExchange(CGameObject &Obj)
{
	int iValidItemCount		= 0;
	int iInvalidItemCount	= 0;
	CItemObject NewItem;
	
	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;
	
	Obj.ChaosLock = true;
	
	_LUCKY_ITEM_INFO * pLuckyItemInfo				= 0;
	_ITEM_LEVEL_RATE * pItemLevelRandRate			= 0;
	_LUCKY_ITEM_EQUIPMENT * pLuckyItemEquipment		= 0;
	int * piAddSetOptRate							= NULL;
	int iItemPos									= -1;
	int iItemHeight									= 4;
	int iItemWidth									= 4;
	
	if( !CheckInventoryEmptySpace(lpObj, iItemHeight, iItemWidth) )
	{
		sLog->outBasic("[LuckyItem] - Fail - Not Empty Inventory [%s][%s] CharClass[%d]", Obj.AccountID, Obj.Name, Obj.Class);
		pMsg.Result = 0xF1;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = false;
		return;
	}
	
	for( int i = 0; i < CHAOS_BOX_SIZE; i++ )
	{
		if( Obj.pntChaosBox[i]->IsItem() )
		{
			pLuckyItemInfo = this->GetCharClassLuckyItemInfo(Obj.pntChaosBox[i]->m_Type, Obj.Class);

			if( pLuckyItemInfo == NULL )
			{
				iInvalidItemCount++;
				iItemPos = i;
			}
			else
			{
				iValidItemCount++;
				iItemPos = i;
			}
		}
	}
	
	if( iValidItemCount != 1 || iInvalidItemCount != 0 || iItemPos == -1 )
	{
		sLog->outBasic("[LuckyItem] - Can Not be Exchanged [%s][%s] CharClass[%d] ItemNum[%d] ItemName[%s]", Obj.AccountID, Obj.Name, Obj.Class,
			Obj.pntChaosBox[iItemPos]->m_Type, ItemAttribute[Obj.pntChaosBox[iItemPos]->m_Type].Name);
		pMsg.Result = 0x20;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = false;
		return;
	}
	
	g_MixSystem.LogChaosItem(lpObj, "[LuckyItem] Item Exchange");
	sLog->outBasic("[LuckyItem] - Exchange Start");
	
	int	iCharType		= 0;
	int Type			= 0;
	BYTE level			= 0;
	BYTE dur			= 0xFF;
	BYTE Op1			= 0;
	BYTE Op2			= 0;
	BYTE Op3			= 0;
	int iMinLv			= 0;
	int iMaxLv			= 0;
	int SetOption		= 0;
	pItemLevelRandRate	= pLuckyItemInfo->ItemLevelRate;
	pLuckyItemEquipment = this->GetLuckyItemEquipment(pLuckyItemInfo, Obj.Class);
	piAddSetOptRate	= pLuckyItemInfo->iAddSetOptRate;
	
	if( pLuckyItemEquipment == 0 || pItemLevelRandRate == 0 || piAddSetOptRate == 0)
	{
		sLog->outBasic("[LuckyItem] --- Error --- [%s][%s] CharClass[%d] ItemNum[%d] ItemName[%s]", Obj.AccountID, Obj.Name, Obj.Class,
			Obj.pntChaosBox[iItemPos]->m_Type, ItemAttribute[Obj.pntChaosBox[iItemPos]->m_Type].Name);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = false;
		return;
	}
	
	Type		= pLuckyItemEquipment->iItemNum;
	SetOption	= gSetItemOption.LuckyItemGenSetOption(Type, piAddSetOptRate);
	level		= this->GambleGetLevel(pItemLevelRandRate, Obj.m_Index);
	
	if( pLuckyItemEquipment->iSkill )
	{
		Op1 = 1;
	}
	
	if( pLuckyItemEquipment->iLuckOption )
	{
		Op2 = 0;
		
		if( !(rand() % 2) )
		{
			Op2 = 1;
		}
	}

	LPITEM_ATTRIBUTE p = &ItemAttribute[Type];
		
	DWORD dwOptionTemp = GetLargeRand() % 1000000;
	
	if( pLuckyItemEquipment->iAddOption )
	{
		if( dwOptionTemp <= pLuckyItemInfo->iAddOpt3Rate[0] )
		{
			Op3 = 0;
		}
		
		else if( dwOptionTemp <= (pLuckyItemInfo->iAddOpt3Rate[0] + pLuckyItemInfo->iAddOpt3Rate[1]) )
		{
			Op3 = 1;
		}
		
		else if (dwOptionTemp <= (pLuckyItemInfo->iAddOpt3Rate[0] + pLuckyItemInfo->iAddOpt3Rate[1] + pLuckyItemInfo->iAddOpt3Rate[2]))
		{
			Op3 = 2;
		}
		
		else if (dwOptionTemp <= (pLuckyItemInfo->iAddOpt3Rate[0] + pLuckyItemInfo->iAddOpt3Rate[1] + pLuckyItemInfo->iAddOpt3Rate[2] + pLuckyItemInfo->iAddOpt3Rate[3]))
		{
			Op3 = 3;
		}

		else if (dwOptionTemp <= (pLuckyItemInfo->iAddOpt3Rate[0] + pLuckyItemInfo->iAddOpt3Rate[1] + pLuckyItemInfo->iAddOpt3Rate[2] + pLuckyItemInfo->iAddOpt3Rate[3] + pLuckyItemInfo->iAddOpt3Rate[4]))
		{
			Op3 = 4;
		}

		else if (dwOptionTemp <= (pLuckyItemInfo->iAddOpt3Rate[0] + pLuckyItemInfo->iAddOpt3Rate[1] + pLuckyItemInfo->iAddOpt3Rate[2] + pLuckyItemInfo->iAddOpt3Rate[3] + pLuckyItemInfo->iAddOpt3Rate[4] + pLuckyItemInfo->iAddOpt3Rate[5]))
		{
			Op3 = 5;
		}

		else if (dwOptionTemp <= (pLuckyItemInfo->iAddOpt3Rate[0] + pLuckyItemInfo->iAddOpt3Rate[1] + pLuckyItemInfo->iAddOpt3Rate[2] + pLuckyItemInfo->iAddOpt3Rate[3] + pLuckyItemInfo->iAddOpt3Rate[4] + pLuckyItemInfo->iAddOpt3Rate[5] + pLuckyItemInfo->iAddOpt3Rate[6]))
		{
			Op3 = 6;
		}

		else
		{
			Op3 = 7;
		}
	}

	if (g_ConfigRead.data.common.Is28Opt == false)
		Op3 = 4;
	
	ItemCreate(Obj.m_Index, -1, 0, 0, Type, level, dur, Op1, Op2, Op3, Obj.m_Index, 0, SetOption, 0, 0, 0);
	
	int	tmpSetOption = 0;
	
	if( (SetOption & 0x01) != 0 )
	{
		tmpSetOption = 1;
	}
	else if( (SetOption & 0x02) != 0 )
	{
		tmpSetOption = 2;
	}
	
	char szSetItemName[48] = { 0 };
	strcpy(szSetItemName, gSetItemOption.GetSetOptionName(Type, tmpSetOption));
	strcat(szSetItemName, ItemAttribute[pLuckyItemEquipment->iItemNum].Name);
	gObjInventoryCommit(Obj.m_Index);
}

BYTE LuckyItemManager::GambleGetLevel(_ITEM_LEVEL_RATE * pItemLevelRandRate, CGameObject &Obj)
{
	BYTE level			= 0;
	long dwRandRate		= GetLargeRand() % 1000000;
	long dwLevelTemp	= 0;
	
	for( int i = 0; i < MAX_LUCKYINFO_LEVELRATE; i++ )
	{
		dwLevelTemp += pItemLevelRandRate[i].iRate;
		
		if( dwRandRate <= dwLevelTemp )
		{
			level = pItemLevelRandRate[i].iLevel;
			break;
		}
	}
	
	return level;
}

void LuckyItemManager::LuckyItemSmelting(CGameObject &Obj)
{
	int iValidItemCount		= 0;
	BYTE iInvalidItemCount	= 0;
	
	CItemObject NewItem;
	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	
	pMsg.Result			= CB_ERROR;
	Obj.ChaosLock	= true;
	int iItemPos		= -1;
	
	for( int i = 0; i < CHAOS_BOX_SIZE; i++ )
	{
		if( Obj.pntChaosBox[i]->IsItem() )
		{
			int bLuckyItemEquipment = this->IsLuckyItemEquipment(Obj.pntChaosBox[i]->m_Type);
			
			if( !bLuckyItemEquipment )
			{
				iInvalidItemCount++;
			}
			else
			{
				iValidItemCount++;
				iItemPos = i;
			}
		}
	}
	
	if( iValidItemCount != 1 || iInvalidItemCount != 0 || iItemPos == -1 )
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = false;
		return;
	}
	else
	{
		sLog->outBasic("[LuckyItemSmelting] - Smelting Start");
		g_MixSystem.LogChaosItem(lpObj, "[LuckyItem] Item Smelting");
		
		int bGambleLuckyItemSmelting = this->GambleLuckyItemSmelting(Obj.pntChaosBox[iItemPos]->m_Type, (int)Obj.pntChaosBox[iItemPos]->m_Durability);
		this->GDReqLuckyItemDelete(Obj.pntChaosBox[iItemPos]->m_Type, Obj.pntChaosBox[iItemPos]->m_Number, Obj.m_Index);
		
		int iCharType	= 0;
		int Type		= ITEMGET(14, 160);
		BYTE level		= 0;
		BYTE dur		= 0;
		BYTE Op1		= 0;
		BYTE Op2		= 0;
		BYTE Op3		= 0;
		
		if( !bGambleLuckyItemSmelting )
		{
			g_MixSystem.ChaosBoxInit(lpObj);
			gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
			
			sLog->outBasic("[LuckyItem][Smelt Item Mix] Mix Fail [%s][%s] ", Obj.AccountID, Obj.Name);
			
			gGameProtocol.GCServerMsgStringSend(Lang.GetText(0,505), Obj.m_Index, 0x01);
			Obj.ChaosLock = false;
			return;
		}
		else
		{
			ItemCreate(Obj.m_Index, 0xFF, 0, 0, Type, level, 1, Op1, Op2, Op3, Obj.m_Index, 0, 0, 0, 0, 0);
		}
		
		sLog->outBasic("[LuckyItem][Smelt Item Mix] Mix Success [%s][%s] ItemName[%s] ItemNum[%d] Level[%d] Dur[%d] skill[%d] luck[%d] option[%d]",
			Obj.AccountID, Obj.Name, ItemAttribute[Type].Name, Type, level, dur, Op1, Op2, Op3);
		
		gObjInventoryCommit(Obj.m_Index);
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0,504), Obj.m_Index, 0x01);
		return;
	}
}

int LuckyItemManager::GambleLuckyItemSmelting(int iItemNum, int iDur)
{
	int iRandRate			= 0;
	_SMELT_RATE * SmeltRate = this->GetSmeltingRateInfo(iItemNum);
	
	for( int i = 0; i < MAX_LUCKYINFO_SMELTRATE; i++ )
	{
		if( iDur <= SmeltRate[i].iMaxDurab && iDur >= SmeltRate[i].iMinDurab )
		{
			iRandRate = SmeltRate[i].iSmeltRate;
			break;
		}
	}
	
	int iRand = rand() % 100;
	sLog->outBasic("[LuckyItem] - Gamble Smelting iRand %d iRandRate %d Dur %d", iRand, iRandRate, iDur);
	
	if( iRand < iRandRate )
	{
		return true;
	}

	return false;
}

int LuckyItemManager::GetLuckyItemDurabilityTime()
{
	return g_ConfigRead.data.common.LuckyItemDurabilityTime;
}

BYTE LuckyItemManager::GetLevel(int iMinLv, int iMaxLv)
{
	if( iMinLv == iMaxLv )
	{
		return iMinLv;
	}
	
	int sub		= iMaxLv - iMinLv + 1;
	int level	= rand() % sub + iMinLv;
	
	return level;
}

int LuckyItemManager::LuckyItemRepaire(CGameObject &Obj, int source, int target)
{
	int iAddDur = 255;
	
	if( source < 0 || source > MAIN_INVENTORY_SIZE-1 )
	{
		return false;
	}
	
	if( target < 0 || target > MAIN_INVENTORY_SIZE-1 )
	{
		return false;
	}
	
	if( !Obj.pntInventory[source]->IsItem() || !Obj.pntInventory[target]->IsItem() )
	{
		return false;
	}
	
	CItemObject * LuckyItemEquipment	= &Obj.pntInventory[target];
	int	bLuckyItemEquipment		= this->IsLuckyItemEquipment(LuckyItemEquipment->m_Type);
	
	if( !bLuckyItemEquipment )
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0,506), Obj.m_Index, 1);
		sLog->outBasic("[LuckyItem][RepaireItemUsed] - Fail - Not LuckyItem Equipment [%s][%s] Serial[%I64d]",
			Obj.AccountID, Obj.Name, LuckyItemEquipment->m_serial);
		return false;
	}
	
	if( LuckyItemEquipment->m_Durability == 0.0f )
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0,507), Obj.m_Index, 1);
		sLog->outBasic("[LuckyItem][RepaireItemUsed] - Fail - Durability Zero [%s][%s] Serial[%I64d]",
			Obj.AccountID, Obj.Name, LuckyItemEquipment->m_serial);
		return false;
	}
	
	if( ItemAttribute[LuckyItemEquipment->m_Type].Durability < LuckyItemEquipment->m_Durability )
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0,508), Obj.m_Index, 1);
		sLog->outBasic("[LuckyItem][RepaireItemUsed] - Fail - Equipment Full Durability [%s][%s] Serial[%I64d]",
			Obj.AccountID, Obj.Name, LuckyItemEquipment->m_serial);
		return false;
	}
	
	if( ItemAttribute[LuckyItemEquipment->m_Type].Durability >= LuckyItemEquipment->m_Durability + 255.0f )
	{
		LuckyItemEquipment->m_Durability += 255.0f;
	}
	else
	{
		LuckyItemEquipment->m_Durability = ItemAttribute[LuckyItemEquipment->m_Type].Durability;
	}
	
	gGameProtocol.GCServerMsgStringSend(Lang.GetText(0,509), Obj.m_Index, 1);
	sLog->outBasic("[LuckyItem][RepaireItemUsed] - Success - [%s][%s] Serial[%I64d]",
		Obj.AccountID, Obj.Name, LuckyItemEquipment->m_serial);
	
	return true;
}

int LuckyItemManager::GDReqLuckyItemInsert(CItemObject * pItem, int iObjIndex)
{
	WORD wItemCode			= pItem->m_Type;
	UINT64 Serial			= pItem->m_serial;
	WORD wDurabilitySmall	= pItem->m_DurabilitySmall;
	
	return this->GDReqLuckyItemInsert(wItemCode, Serial, wDurabilitySmall, iObjIndex);
}

int LuckyItemManager::GDReqLuckyItemInsert(short wItemCode, UINT64 Serial, short wDurabilitySmall, int iObjIndex)
{
	CGameObject lpObj = getGameObject(iObjIndex);
	PMSG_REQ_LUCKYITEM_INSERT pMsg;
	
	pMsg.dwUserGuid	= Obj.DBNumber;
	std::memcpy(pMsg.szCharName, Obj.Name, MAX_ACCOUNT_LEN+1);
	pMsg.LuckyItemDBInfo.wItemCode			= wItemCode;
	pMsg.LuckyItemDBInfo.Serial			= Serial;
	pMsg.LuckyItemDBInfo.wDurabilitySmall	= wDurabilitySmall;
	
	PHeadSubSetB((BYTE*)&pMsg, 0xD0, 0x22, sizeof(PMSG_REQ_LUCKYITEM_INSERT));
	wsDataCli.DataSend((char *)&pMsg, pMsg.head.size);
	return true;
}

void LuckyItemManager::GDReqLuckyItemInsert2nd(int iObjIndex)
{
	int	iItemType[5];
	
	CGameObject lpObj		= getGameObject(iObjIndex);
	iItemType[0]	= 2;
	iItemType[1]	= 3;
	iItemType[2]	= 4;
	iItemType[3]	= 5;
	iItemType[4]	= 6;
	int iItemCnt	= 0;
	
	PMSG_REQ_LUCKYITEM_INSERT_2ND pMsg;
	
	for( int i = 0; i < 5; i++ )
	{
		CItemObject pItem = Obj.pntInventory[iItemType[i]];
		
		if( this->IsLuckyItemEquipment(pItem.m_Type) )
		{
			pMsg.LuckyItemDBInfo[iItemCnt].Serial			= pItem.m_Number;
			pMsg.LuckyItemDBInfo[iItemCnt].wDurabilitySmall	= pItem.m_DurabilitySmall;
			pMsg.LuckyItemDBInfo[iItemCnt].wItemCode		= pItem.m_Type;
			iItemCnt++;
		}
	}
	
	pMsg.btItemCnt	= iItemCnt;
	pMsg.dwUserGuid	= Obj.DBNumber;
	std::memcpy(pMsg.szCharName, Obj.Name, MAX_ACCOUNT_LEN+1);
	
 	PHeadSubSetB((BYTE*)&pMsg, 0xD0, 0x24, sizeof(PMSG_REQ_LUCKYITEM_INSERT_2ND));
 	wsDataCli.DataSend((LPSTR)&pMsg, pMsg.head.size);
}

void LuckyItemManager::GDReqLuckyItemDelete(short wItemCode, UINT64 Serial, int iObjIndex)
{
	CGameObject lpObj = getGameObject(iObjIndex);
	PMSG_REQ_LUCKYITEM_DELETE pMsg;
	
	pMsg.wUserIndex = Obj.m_Index;
	std::memcpy(pMsg.szCharName, Obj.Name, MAX_ACCOUNT_LEN+1);
	pMsg.wItemCode	= wItemCode;
	pMsg.Serial	= Serial;
	
	PHeadSubSetB((BYTE*)&pMsg, 0xD0, 0x23, sizeof(PMSG_REQ_LUCKYITEM_DELETE));
	wsDataCli.DataSend((char *)&pMsg, pMsg.head.size);
	
	sLog->outBasic("[LuckyItem] GDReqLuckyItemDelete [%s][%s] ItemCode[%d] Serial[%I64d]",
		Obj.AccountID, Obj.Name, wItemCode, Serial);
}

void LuckyItemManager::DGAnsLuckyItemList(PMSG_ANS_LUCKYITEM_SELECT * lpRecv)
{
	BYTE iItemCnt	= lpRecv->btItemCnt;
	WORD iObjIndex	= lpRecv->wUserIndex;

	if( iItemCnt <= 0 )
	{
		return;
	}
	
	if( !ObjectMaxRange(iObjIndex) || !gObjIsConnected(iObjIndex) )
	{
		return;
	}
	
	BYTE * iDeleteItemList = new BYTE[iItemCnt];
	memset(iDeleteItemList, 0, iItemCnt);
	PMSG_LUCKYITME_DB_INFO LuckyItemList[45];
	std::memcpy(LuckyItemList, &lpRecv[1], iItemCnt * sizeof(PMSG_LUCKYITME_DB_INFO));
	
	for( int iInven = 0; iInven < MAIN_INVENTORY_SIZE; iInven++ )
	{
		if( getGameObject(iObjIndex)->pntInventory[iInven)->IsItem() )
		{
			CItemObject * pItem = getGameObject(iObjIndex)->pntInventory[iInven);

			for( int i = 0; i < iItemCnt; i++ )
			{
				if( pItem->m_Type == LuckyItemList[i].wItemCode 
					&& pItem->m_Number == LuckyItemList[i].Serial )
				{
					pItem->m_DurabilitySmall = LuckyItemList[i].wDurabilitySmall;
					iDeleteItemList[i]		= 1;
					break;
				}
			}
		}
	}
	
	for( int j = 0; j < iItemCnt; j++ )
	{
		if( iDeleteItemList[j] == 0 )
		{
			this->GDReqLuckyItemDelete(LuckyItemList[j].wItemCode, LuckyItemList[j].Serial, iObjIndex);
		}
	}

	delete[] iDeleteItemList;
}

void LuckyItemManager::GDReqLuckyItemSelect(CGameObject &Obj)
{
	PMSG_REQ_LUCKYITEM_SELECT pMsg;
	
	pMsg.wUserIndex = Obj.m_Index;
	std::memcpy(pMsg.chCharacterName, Obj.Name, MAX_ACCOUNT_LEN+1);
	
	PHeadSubSetB((BYTE*)&pMsg, 0xD0, 0x20, sizeof(PMSG_REQ_LUCKYITEM_SELECT));
	wsDataCli.DataSend((char *)&pMsg, pMsg.head.size);
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

