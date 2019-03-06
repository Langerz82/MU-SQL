#include "Marry.h"
#include "GameProtocol.h"
#include "User/CUserData.h"

#include "Main.h"
CMarry Marry;

CMarry::CMarry()
{
	InitializeCriticalSection(&this->m_criti);
}

CMarry::~CMarry()
{
	DeleteCriticalSection(&this->m_criti);
}

void CMarry::LoadFile(char * filename)
{
	EnterCriticalSection(&this->m_criti);

	this->m_bFileLoad = false;

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(filename);

	if (res.status != pugi::status_ok)
	{
		sLog->outError("Error loading %s file (%s)", filename, res.description());

		LeaveCriticalSection(&this->m_criti);
		return;
	}

	pugi::xml_node mainXML = file.child("MarrySystem");

	this->m_bMarryEnable = mainXML.attribute("Enable").as_bool();
	this->m_iMarryMinUserLevel = mainXML.attribute("ReqLevel").as_int();
	this->m_iMarryNeedMoney = mainXML.attribute("ReqMoney").as_int();

	pugi::xml_node location = mainXML.child("Location");

	this->m_wMarryMapNumber = location.attribute("MapNumber").as_int();
	this->m_btMarryStartX = location.attribute("StartX").as_int();
	this->m_btMarryStartY = location.attribute("StartY").as_int();
	this->m_btMarryEndX = location.attribute("EndX").as_int();
	this->m_btMarryEndY = location.attribute("EndY").as_int();

	pugi::xml_node wedding = mainXML.child("WeddingSettings");

	this->m_bHomoSexualAllow = wedding.attribute("Homosexual").as_bool();
	this->m_bNeedSpecialItemForMarry = wedding.attribute("ReqSpecialItem").as_bool();

	this->m_vtMarryItem.clear();
	
	for (pugi::xml_node item = wedding.child("Item"); item; item = item.next_sibling())
	{
		MARRY_ITEM m_Item;

		m_Item.m_iSide = item.attribute("Side").as_int();
		int iItemID = ItemGetNumberMake(item.attribute("ItemCat").as_int(), item.attribute("ItemIndex").as_int());

		if (iItemID == -1)
		{
			sLog->outError("ERROR - Wrong item in %s file (%d %d)", filename, item.attribute("ItemCat").as_int(), item.attribute("ItemIndex").as_int());
			continue;
		}

		m_Item.m_wItemID = iItemID;
		m_Item.m_iItemCount = item.attribute("ItemCount").as_int();

		this->m_vtMarryItem.push_back(m_Item);
	}

	pugi::xml_node gift = mainXML.child("WeddingGift");

	this->m_bGiftEnable = gift.attribute("Enable").as_bool();

	this->m_vtGiftItem.clear();

	for (pugi::xml_node item = gift.child("Item"); item; item = item.next_sibling())
	{
		MARRY_ITEM m_Item;

		m_Item.m_iSide = item.attribute("Side").as_int();
		int iItemID = ItemGetNumberMake(item.attribute("ItemCat").as_int(), item.attribute("ItemIndex").as_int());

		if (iItemID == -1)
		{
			sLog->outError("ERROR - Wrong item in %s file (%d %d)", filename, item.attribute("ItemCat").as_int(), item.attribute("ItemIndex").as_int());
			continue;
		}

		m_Item.m_wItemID = iItemID;
		m_Item.m_iItemCount = item.attribute("ItemCount").as_int();

		this->m_vtGiftItem.push_back(m_Item);
	}

	pugi::xml_node divorce = mainXML.child("DivorceSettings");

	this->m_bDivorceAllow = divorce.attribute("Allow").as_bool();
	this->m_bNeedSpecialItemForDivorce = divorce.attribute("ReqSpecialItem").as_bool();

	this->m_vtDivorceItem.clear();

	for (pugi::xml_node item = divorce.child("Item"); item; item = item.next_sibling())
	{
		MARRY_ITEM m_Item;

		m_Item.m_iSide = 0;
		int iItemID = ItemGetNumberMake(item.attribute("ItemCat").as_int(), item.attribute("ItemIndex").as_int());

		if (iItemID == -1)
		{
			sLog->outError("ERROR - Wrong item in %s file (%d %d)", filename, item.attribute("ItemCat").as_int(), item.attribute("ItemIndex").as_int());
			continue;
		}

		m_Item.m_wItemID = iItemID;
		m_Item.m_iItemCount = item.attribute("ItemCount").as_int();

		this->m_vtDivorceItem.push_back(m_Item);
	}

	this->m_bFileLoad = true;
	LeaveCriticalSection(&this->m_criti);
}

void CMarry::Propose(CGameObject &Obj, int uIndex)
{
	if (ObjectMaxRange(Obj.m_Index) == 0 || ObjectMaxRange(uIndex) == 0)
	{
		return;
	}

	if (getGameObject(uIndex)->Type != OBJ_USER)
	{
		return;
	}

	if (Obj.Name == getGameObject(uIndex)->Name)
	{
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0,393), Obj.m_Index, 1);
		return;
	}

	if (!this->m_bMarryEnable)
	{
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0,373), Obj.m_Index, 1);
		return;
	}

	if (Obj.Married != 0 || getGameObject(uIndex)->Married != 0)
	{
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0,374), Obj.m_Index, 1);
		return;
	}

	if (isFemale(Obj.m_Index) == isFemale(uIndex) && !this->m_bHomoSexualAllow)
	{
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0,375), Obj.m_Index, 1);
		return;
	}

	if (isMale(Obj.m_Index) == isMale(uIndex) && !this->m_bHomoSexualAllow)
	{
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0,375), Obj.m_Index, 1);
		return;
	}

	if (this->m_iMarryNeedMoney > Obj.m_PlayerData->Money || this->m_iMarryNeedMoney > getGameObject(uIndex)->m_PlayerData->Money)
	{
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0,376), Obj.m_Index, 1);
		return;
	}

	if (this->m_iMarryMinUserLevel > Obj.Level || this->m_iMarryMinUserLevel > getGameObject(uIndex)->Level)
	{
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0,377), Obj.m_Index, 1);
		return;
	}

	if (!this->CheckPosition(Obj.m_Index, uIndex))
	{
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0,378), Obj.m_Index, 1);
		return;
	}

	bool bCheck1 = this->CheckRequiredItem(Obj.m_Index, 1);
	bool bCheck2 = this->CheckRequiredItem(uIndex, 2);

	if (bCheck1 == false || bCheck2 == false)
	{
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0,379), Obj.m_Index, 1);
		return;
	}

	getGameObject(uIndex)->MarryRequested = 1;
	getGameObject(uIndex)->MarryRequestIndex = Obj.m_Index;
	getGameObject(uIndex)->MarryRequestTime = GetTickCount();

	char msg[128];
	wsprintf(msg, Lang.GetText(0,380), Obj.Name);
	GSProtocol.GCServerMsgStringSend(msg, uIndex, 1);
	GSProtocol.GCServerMsgStringSend(Lang.GetText(0,381), Obj.m_Index, 1);

	sLog->outBasic("[Marry][%s][%s] Request to marry with [%s][%s]", Obj.AccountID, Obj.Name, getGameObject(uIndex)->AccountID, getGameObject(uIndex)->Name);
}

bool CMarry::Accept(CGameObject &Obj)
{
	if (ObjectMaxRange(Obj.m_Index) == 0)
	{
		return false;
	}

	char info[255];
	int uIndex = Obj.MarryRequestIndex;

	if (Obj.MarryRequested != 1)
	{
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0,382), Obj.m_Index, 1);
		return false;
	}

	if (GetTickCount() - Obj.MarryRequestTime > 60000)
	{
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0,383), Obj.m_Index, 1);
		Obj.MarryRequested = 0;
		return false;
	}

	if (!this->CheckPosition(Obj.m_Index, uIndex))
	{
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0,384), Obj.m_Index, 1);
		return false;
	}

	if (getGameObject(uIndex)->m_PlayerData->Money < getGameObject(uIndex)->m_PlayerData->Money - this->m_iMarryNeedMoney || Obj.m_PlayerData->Money < Obj.m_PlayerData->Money - this->m_iMarryNeedMoney)
	{
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0,385), Obj.m_Index, 1);
		return false;
	}

	bool bCheck1 = this->CheckRequiredItem(Obj.m_Index, 1);
	bool bCheck2 = this->CheckRequiredItem(uIndex, 2);

	if (bCheck1 == false || bCheck2 == false)
	{
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0,386), Obj.m_Index, 1);
		return false;
	}

	this->DeleteRequiredItem(Obj.m_Index, 1);
	this->DeleteRequiredItem(uIndex, 2);

	Obj.Married = 1;
	Obj.MarryRequestIndex = 0;
	Obj.MarryRequestTime = 0;
	strcpy(Obj.MarryName, getGameObject(uIndex)->Name);

	Obj.m_PlayerData->Money -= this->m_iMarryNeedMoney;
	GSProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	GSProtocol.GCServerMsgStringSend(Lang.GetText(0,387), Obj.m_Index, 1);

	getGameObject(uIndex)->Married = 1;
	getGameObject(uIndex)->MarryRequestIndex = 0;
	getGameObject(uIndex)->MarryRequestTime = 0;
	strcpy(getGameObject(uIndex)->MarryName, Obj.Name);

	getGameObject(uIndex)->m_PlayerData->Money -= this->m_iMarryNeedMoney;
	GSProtocol.GCMoneySend(uIndex, getGameObject(uIndex)->m_PlayerData->Money);

	GSProtocol.GCServerMsgStringSend(Lang.GetText(0,388), uIndex, 1);
		
	PMSG_SERVERCMD ServerCmd;

	PHeadSubSetB((BYTE*)&ServerCmd, 0xF3, 0x40, sizeof(ServerCmd));
	ServerCmd.CmdType = 0;
	ServerCmd.X = getGameObject(uIndex)->X;
	ServerCmd.Y = getGameObject(uIndex)->Y;

	GSProtocol.MsgSendV2(gObj, (BYTE*)&ServerCmd, sizeof(ServerCmd));
	IOCP.DataSend(uIndex, (BYTE*)&ServerCmd, sizeof(ServerCmd));

	ServerCmd.X = Obj.X;
	ServerCmd.Y = Obj.Y;

	GSProtocol.MsgSendV2(gObj, (BYTE*)&ServerCmd, sizeof(ServerCmd));
	IOCP.DataSend(Obj.m_Index, (BYTE*)&ServerCmd, sizeof(ServerCmd));

	if (this->m_bGiftEnable == true)
	{
		this->GiveGiftItem(Obj.m_Index, 1);
		this->GiveGiftItem(uIndex, 2);
	}

	wsprintf(info, Lang.GetText(0,389), getGameObject(uIndex)->Name, Obj.Name);
	GSProtocol.AllSendServerMsg(info);

	sLog->outBasic("[Marry] New Marriage: %s [%d]  %s [%d]", getGameObject(uIndex)->Name, Obj.Married, Obj.Name, getGameObject(uIndex)->Married);
	return true;
}

void CMarry::Divorce(CGameObject &Obj)
{
	if (Obj.Married == 0)
	{
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0,390), Obj.m_Index, 1);
		return;
	}

	if (this->m_bDivorceAllow == false)
	{
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0,614), Obj.m_Index, 1);
		return;
	}

	if (!this->CheckDivorceItem(Obj.m_Index))
	{
		MsgOutput(Obj.m_Index, Lang.GetText(0,391));
		return;
	}

	GSProtocol.GCServerMsgStringSend(Lang.GetText(0,392), Obj.m_Index, 1);

	int uIndex = gObjGetIndex(Obj.MarryName);

	if (uIndex != -1)
	{
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0,392), uIndex, 1);
		getGameObject(uIndex)->Married = 0;
		strcpy(getGameObject(uIndex)->MarryName, "");
	}

	Obj.Married = 0;
	strcpy(Obj.MarryName, "");

	this->DeleteDivorceItem(Obj.m_Index);

	sLog->outBasic("[Marry] Divorce: [%s] [%d]", Obj.Name, getGameObject(uIndex)->Name);
}

bool CMarry::CheckPosition(CGameObject &Obj, int uIndex)
{
	int aX = Obj.X, aY = Obj.Y, aM = Obj.MapNumber, uX = getGameObject(uIndex)->X, uY = getGameObject(uIndex)->Y, uM = getGameObject(uIndex)->MapNumber;

	if((aX >= this->m_btMarryStartX && aX <= this->m_btMarryEndX) && (aY >= this->m_btMarryStartY && aY <= this->m_btMarryEndY) && (uX >= this->m_btMarryStartX && uX <= this->m_btMarryEndX) && (uY >= this->m_btMarryStartY && uY <= this->m_btMarryEndY))
	{
		return true;
	}

	return false;
}

bool CMarry::CheckRequiredItem(CGameObject &Obj, int iSide)
{
	if(!this->m_bNeedSpecialItemForMarry) return true;

	EnterCriticalSection(&this->m_criti);

	for (std::vector<MARRY_ITEM>::iterator It = this->m_vtMarryItem.begin(); It != this->m_vtMarryItem.end(); It++)
	{
		if (It->m_iSide != iSide)
		{
			continue;
		}

		if (gObjGetItemCountInIventory(Obj.m_Index, It->m_wItemID) < It->m_iItemCount)
		{
			LeaveCriticalSection(&this->m_criti);
			return false;
		}
	}

	LeaveCriticalSection(&this->m_criti);
	return true;
}

bool CMarry::CheckDivorceItem(CGameObject &Obj)
{
	if(!this->m_bNeedSpecialItemForDivorce) return true;

	EnterCriticalSection(&this->m_criti);

	for (std::vector<MARRY_ITEM>::iterator It = this->m_vtDivorceItem.begin(); It != this->m_vtDivorceItem.end(); It++)
	{
		if (gObjGetItemCountInIventory(Obj.m_Index, It->m_wItemID) < It->m_iItemCount)
		{
			LeaveCriticalSection(&this->m_criti);
			return false;
		}
	}

	LeaveCriticalSection(&this->m_criti);
	return true;
}

void CMarry::GiveGiftItem(CGameObject &Obj, int iSide)
{
	if (!this->m_bGiftEnable) return;

	EnterCriticalSection(&this->m_criti);

	for (std::vector<MARRY_ITEM>::iterator It = this->m_vtGiftItem.begin(); It != this->m_vtGiftItem.end(); It++)
	{
		if (It->m_iSide != iSide)
		{
			continue;
		}

		for (int i = 0; i < It->m_iItemCount; i++)
		{
			ItemCreate(Obj.m_Index, 235, 0, 0, It->m_wItemID, 0, 0, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
		}
	}

	LeaveCriticalSection(&this->m_criti);
}

void CMarry::DeleteRequiredItem(CGameObject &Obj, int iSide)
{
	if(!this->m_bNeedSpecialItemForMarry) return;

	EnterCriticalSection(&this->m_criti);

	for (std::vector<MARRY_ITEM>::iterator It = this->m_vtMarryItem.begin(); It != this->m_vtMarryItem.end(); It++)
	{
		if (It->m_iSide != iSide)
		{
			continue;
		}

		for (int i = 0; i < It->m_iItemCount; i++)
		{
			for(int i=0;i<MAIN_INVENTORY_SIZE;i++)
			{
				if(Obj.pntInventory[i]->IsItem())
				{
					if (Obj.pntInventory[i]->m_Type == It->m_wItemID)
					{
						gObjInventoryDeleteItem(Obj.m_Index, i);
						GSProtocol.GCInventoryItemDeleteSend(Obj.m_Index, i, 1);
						break;
					}
				}	
			}
		}
	}

	LeaveCriticalSection(&this->m_criti);
}

void CMarry::DeleteDivorceItem(CGameObject &Obj)
{
	if(!this->m_bNeedSpecialItemForDivorce) return;

	EnterCriticalSection(&this->m_criti);

	for (std::vector<MARRY_ITEM>::iterator It = this->m_vtDivorceItem.begin(); It != this->m_vtDivorceItem.end(); It++)
	{
		for (int i = 0; i < It->m_iItemCount; i++)
		{
			for (int i = 0; i<MAIN_INVENTORY_SIZE; i++)
			{
				if (Obj.pntInventory[i]->IsItem())
				{
					if (Obj.pntInventory[i]->m_Type == It->m_wItemID)
					{
						gObjInventoryDeleteItem(Obj.m_Index, i);
						GSProtocol.GCInventoryItemDeleteSend(Obj.m_Index, i, 1);
						break;
					}
				}
			}
		}
	}

	LeaveCriticalSection(&this->m_criti);
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

