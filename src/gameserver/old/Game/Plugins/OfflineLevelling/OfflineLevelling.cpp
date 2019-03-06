////////////////////////////////////////////////////////////////////////////////
// OfflineLevelling.cpp
#include "StdAfx.h"
#include "User/CUserData.h"
#include "ObjUseSkill.h"
#include "OfflineLevelling.h"
#include "Logging/Log.h"
#include "configread.h"
#include "GameProtocol.h"
#include "Main.h"
#include "util.h"

#include "ItemSocketOptionSystem.h"
#include "CashShop.h"
#include "LuckyItemManager.h"
#include "SocketItemType.h"

COfflineLevelling g_OffLevel;

void OffLevelThread()
{
	while(true)
	{
		for(int i=g_ConfigRead.server.GetObjectStartUserIndex();i<g_ConfigRead.server.GetObjectMax();i++)
		{
			if(!gObjIsConnectedGP(i))
				continue;
			if(g_OffLevel.FindUser(i) != -1)
			{
				g_OffLevel.FindAndAttack(getGameObject(i));
				g_OffLevel.CheckUseTime(i);
				if(g_OffLevel.m_General.AutoRepairItems == 1)
				{
					g_OffLevel.CheckRepairItems(i);
				}
			}
		}
		Sleep(g_OffLevel.m_General.HitInterval);
	}
}

COfflineLevelling::COfflineLevelling()
{

}

COfflineLevelling::~COfflineLevelling()
{

}

void COfflineLevelling::Initiate()
{
	InitializeCriticalSection(&m_OfflevelCriti);
	this->m_OffPlayerData.clear();
	memset(this->m_SkillDef,0,sizeof(this->m_SkillDef));
	this->LoadFile(g_ConfigRead.GetPath("\\Plugins\\IGC_OffLevelling.xml"));
	this->LoadSkillDefinitions(g_ConfigRead.GetPath("\\Skills\\IGC_SkillCategory.xml"));
}

BOOL COfflineLevelling::LoadSkillDefinitions(LPCSTR szFile)
{
	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(szFile);

	if(res.status != pugi::status_ok)
	{
		sLog->outError("%s file load fail (%s)", szFile, res.description());
		return 0;
	}

	pugi::xml_node skillcat = file.child("SkillCategory");
	for (pugi::xml_node skill = skillcat.child("Skill"); skill; skill = skill.next_sibling())
	{
		int skillid = skill.attribute("Index").as_int();
		this->m_SkillCategories[skillid].SkillType = skill.attribute("Type").as_int();
	}
}

BOOL COfflineLevelling::LoadFile(LPCSTR szFile)
{
	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(szFile);

	if(res.status != pugi::status_ok)
	{
		sLog->outError("%s file load fail (%s)", szFile, res.description());
		return 0;
	}

	pugi::xml_node offlevelsettings = file.child("OffLevellingSystem");
	this->b_Enabled = offlevelsettings.attribute("Enable").as_bool(0);

	pugi::xml_node general = offlevelsettings.child("General");

	m_General.VipType = general.attribute("VipType").as_int();
	m_General.CoinType = general.attribute("CoinType").as_int();
	m_General.HitInterval = general.attribute("HitInterval").as_int(1000);
	m_General.ChargeInterval = general.attribute("ChargeInterval").as_int();
	m_General.CoinValue = general.attribute("CoinValue").as_int();
	m_General.Immortal = general.attribute("Immortal").as_int(0);
	m_General.MaxDuration = general.attribute("MaxDuration").as_int(0);
	m_General.MaxVipDuration = general.attribute("MaxVipDuration").as_int(0);
	m_General.AutoRepairItems = general.attribute("AutoRepairItems").as_int(0);
	m_PickItems.clear();

	pugi::xml_node itemlist = offlevelsettings.child("PickItems");

	m_PickItemSettings.bPickUpItems = itemlist.attribute("Enable").as_bool(0);
	m_PickItemSettings.PickUpZen = itemlist.attribute("PickUpZen").as_int(0);
	m_PickItemSettings.PickUpExc = itemlist.attribute("PickUpAllExc").as_int(0);
	m_PickItemSettings.PickUpAncient = itemlist.attribute("PickUpAllAncient").as_int(0);
	m_PickItemSettings.PickUpSocket = itemlist.attribute("PickUpAllSocket").as_int(0);

	for(pugi::xml_node item = itemlist.child("Item"); item; item = item.next_sibling())
	{
		OFF_LEVEL_ITEM_PICK_LIST pickItem; 
		WORD wItemCat = item.attribute("Cat").as_int();
		WORD wItemIndex = item.attribute("Index").as_int();
		pickItem.ItemId = ITEMGET(wItemCat,wItemIndex);
		this->m_PickItems.insert(std::pair<int, OFF_LEVEL_ITEM_PICK_LIST>(pickItem.ItemId, pickItem));
		
	}

	pugi::xml_node permapadjust = offlevelsettings.child("PerMapAdjustments");

	for (pugi::xml_node map = permapadjust.child("Map"); map; map = map.next_sibling())
	{
		int MapNumber = map.attribute("Number").as_int();
		this->m_PerMapAttr[MapNumber].CoinType = map.attribute("CoinType").as_int();
		this->m_PerMapAttr[MapNumber].CoinValue = map.attribute("CoinValue").as_int();
		this->m_PerMapAttr[MapNumber].Disable = map.attribute("Disable").as_int();
	}

	pugi::xml_node skilldef = offlevelsettings.child("ClassSkillSettings");

	for(pugi::xml_node classes = skilldef.child("Class"); classes; classes = classes.next_sibling())
	{
		int ClassType = classes.attribute("Type").as_int();
		for(pugi::xml_node skills = classes.child("Skill"); skills; skills = skills.next_sibling())
		{
			int idx = skills.attribute("ID").as_int();
			this->m_SkillDef[ClassType].SkillNumber[idx] = skills.attribute("Index").as_int();
		}
	}
}
int COfflineLevelling::FindUser(CGameObject &Obj)
{
	EnterCriticalSection(&m_OfflevelCriti);
	std::map<int,OFF_LEVEL_PLAYERS>::iterator iter = m_OffPlayerData.find(Obj.m_Index);
	
	if(iter == m_OffPlayerData.end())
	{
		//..sLog->outError("User %d not found LINE: %d",Obj.m_Index,__LINE__);
		LeaveCriticalSection(&m_OfflevelCriti);
		return -1;
	}

	LeaveCriticalSection(&m_OfflevelCriti);
	return Obj.m_Index;
}

BOOL COfflineLevelling::AddUser(CGameObject &Obj, WORD wSkillId)
{
	EnterCriticalSection(&m_OfflevelCriti);

	std::map<int,OFF_LEVEL_PLAYERS>::iterator iter = m_OffPlayerData.find(Obj.m_Index);

	if(iter != m_OffPlayerData.end())
	{
		sLog->outBasic("[OfflineLevelling] User Already exist %d",Obj.m_Index);
		LeaveCriticalSection(&m_OfflevelCriti);
		return false;
	}
	OFF_LEVEL_PLAYERS obj;
	obj.Obj.m_Index = Obj.m_Index;
	obj.wSkillNumber = wSkillId;
	obj.dwOffTime = GetTickCount();
	m_OffPlayerData.insert(std::pair<int, OFF_LEVEL_PLAYERS>(Obj.m_Index,obj));

	sLog->outBasic("[OfflineLevelling] Add user %d Skill Id %d",Obj.m_Index, wSkillId);

	LeaveCriticalSection(&m_OfflevelCriti);
	return true;
}

BOOL COfflineLevelling::DeleteUser(CGameObject &Obj)
{
	EnterCriticalSection(&m_OfflevelCriti);

	std::map<int,OFF_LEVEL_PLAYERS>::iterator iter = m_OffPlayerData.find(Obj.m_Index);

	if(iter == m_OffPlayerData.end())
	{
		sLog->outBasic("[OfflineLevelling] User does not exist %d",Obj.m_Index);
		return false;
	}

	m_OffPlayerData.erase(iter);
	sLog->outBasic("[OfflineLevelling] User %d removed from Offlevelling system",Obj.m_Index);
	LeaveCriticalSection(&m_OfflevelCriti);
	return true;
}

void COfflineLevelling::FindAndAttack(CGameObject &user)
{
	CGameObject tObj;
	CMagicInf* lpMagic;
	OFF_LEVEL_PLAYERS obj;
	std::map<int,OFF_LEVEL_PLAYERS>::iterator iter = m_OffPlayerData.find(user->m_Index);
	obj = iter->second;
	switch(this->GetSkillAttackType(obj.wSkillNumber))
	{
		case MAGIC_ATTACK:
			{
				for (int i=0;i<MAX_VIEWPORT;i++)
				{		
					if(user->VpPlayer2[i].type == OBJ_MONSTER && user->VpPlayer2[i].state != 0)
					{
						lpMagic = gObjGetMagicSearch(user,obj.wSkillNumber);
						if(lpMagic == NULL)
						{
							sLog->outBasic("[OfflineLevelling] [%s][%s] Skill %d not found", user->AccountID, user->Name,obj.wSkillNumber);
							return;
						}
						
						int tObjNum = user->VpPlayer2[i].number;

						if (!ObjectMaxRange(tObjNum))
						{
							continue;
						}

						tObj = getGameObject(tObjNum);

						if(!tObj)
							continue;

						if(tObj.Live == 0 || tObj.DieRegen != FALSE)
							continue;

						if(SkillDistanceCheck(user->m_Index,tObj.m_Index, lpMagic->m_Skill))
						{
							gObjUseSkill.UseSkill(user->m_Index, tObj.m_Index, lpMagic);
							return;
						}
					}
				}
			}
			break;
		case DURATION_MAGIC:
			{
			for (int i=0;i<MAX_VIEWPORT;i++)
				{
					if(user->VpPlayer2[i].type == OBJ_MONSTER && user->VpPlayer2[i].state != 0)
					{
						int tObjNum = user->VpPlayer2[i].number;

						if (!ObjectMaxRange(tObjNum))
						{
							continue;
						}

						tObj = getGameObject(tObjNum);

						if(!tObj)
							continue;

						if(tObj.Live == 0 || tObj.DieRegen != FALSE)
							continue;

						lpMagic = gObjGetMagicSearch(user,obj.wSkillNumber);

						if(lpMagic == NULL)
						{
								sLog->outBasic("[OfflineLevelling] [%s][%s] Skill %d not found", user->AccountID, user->Name,obj.wSkillNumber);
								return;
						}

						if(SkillDistanceCheck(user->m_Index,tObj.m_Index, lpMagic->m_Skill) && tObj.Live != 0)
						{
							BYTE Dir = GetPathPacketDirPos(tObj.X - user->X,tObj.Y - user->Y);
							user->Dir = Dir;
							PMSG_ACTIONRESULT pActionResult;
				
							PHeadSetB((BYTE*)&pActionResult, 0x18, sizeof(pActionResult));
							pActionResult.NumberH = SET_NUMBERH(user->m_Index);
							pActionResult.NumberL = SET_NUMBERL(user->m_Index);
							pActionResult.ActionNumber = 0x7A;
							pActionResult.Dir = user->Dir;
							pActionResult.TargetNumberH =SET_NUMBERH(tObj.m_Index);
							pActionResult.TargetNumberL = SET_NUMBERL(tObj.m_Index);

							IOCP.DataSend(user->m_Index, (BYTE*)&pActionResult, sizeof(pActionResult));
	
							gObjUseSkill.UseSkill(user->m_Index, lpMagic,user->X, user->Y, Dir, 0, tObj.m_Index);
							return;
						}
					}
				}
			}
			break;
		case BE_ATTACK:
			{
				int hitCount = 0;
				for (int i=0;i<MAX_VIEWPORT;i++)
				{
					if(user->VpPlayer2[i].type == OBJ_MONSTER && user->VpPlayer2[i].state != 0)
					{	
						int tObjNum = user->VpPlayer2[i].number;

						if (!ObjectMaxRange(tObjNum))
						{
							continue;
						}

						tObj = getGameObject(tObjNum);

						if(!tObj)
							continue;

						if(tObj.Live == 0 || tObj.DieRegen != FALSE)
							continue;

						int DistanceCheck = gObjCalDistance(user, tObj);

						if(DistanceCheck > 13){
							continue;
						}
							
						hitCount++;

						if(hitCount > 5)
						{
							BYTE Dir = GetPathPacketDirPos(tObj.X - user->X,tObj.Y - user->Y);
							gObjUseSkill.UseSkill(user->m_Index, lpMagic,user->X, user->Y, Dir, 0, tObj.m_Index);
							break;
						}
						lpMagic = gObjGetMagicSearch(user,obj.wSkillNumber);
						if(lpMagic == NULL)
						{
							sLog->outBasic("[OfflineLevelling] [%s][%s] Skill %d not found", user->AccountID, user->Name,obj.wSkillNumber);
							return;
						}
						gObjAttack(user, tObj, lpMagic, TRUE, 1, 0, FALSE, 0, 0);

					}
				}
			}
			break;
	}
}
bool COfflineLevelling::ChargePlayer(CGameObject &Obj)
{
	
	
	if(lpObj == NULL)
		return false;

	if(Obj.Type != OBJ_USER)
		return false;
	if(this->m_PerMapAttr[Obj.MapNumber].CoinType == -1)
	{
		switch (this->m_General.CoinType)
		{
			case 0:
			{
				if (Obj.m_PlayerData->Money < this->m_General.CoinValue)
				{
					return false;
				}
				else
				{
					Obj.m_PlayerData->Money -= this->m_General.CoinValue;
					return true;
				}
			}
			break;

			case 1:
			{
				if (Obj.m_PlayerData->m_WCoinC < this->m_General.CoinValue)
				{
					return false;
				}
				else
				{
						Obj.m_PlayerData->m_WCoinC -= this->m_General.CoinValue;
						GDReqInGameShopPointAdd(Obj.m_Index, 0, 0.00 - this->m_General.CoinValue);
						return true;
				}
			}
			break;

			case 2:
			{
				if (Obj.m_PlayerData->m_WCoinP < this->m_General.CoinValue )
				{
					return false;
				}
				else
				{
						Obj.m_PlayerData->m_WCoinP -= this->m_General.CoinValue;
						GDReqInGameShopPointAdd(Obj.m_Index, 1, 0.00 - this->m_General.CoinValue);
						return true;
				}
			}
			break;

			case 3:
			{
				if (Obj.m_PlayerData->m_GoblinPoint < this->m_General.CoinValue)
				{
					return false;
				}
				else
				{
					Obj.m_PlayerData->m_GoblinPoint -= this->m_General.CoinValue;
					GDReqInGameShopPointAdd(Obj.m_Index, 2, 0.00 - this->m_General.CoinValue);
					return true;
				}
			}
			break;
		}
	}
	else
	{
		switch (this->m_PerMapAttr[Obj.MapNumber].CoinType)
		{
			case 0:
			{
				if (Obj.m_PlayerData->Money < this->m_PerMapAttr[Obj.MapNumber].CoinValue)
				{
					return false;
				}
				else
				{
					Obj.m_PlayerData->Money -= this->m_PerMapAttr[Obj.MapNumber].CoinValue;
					return true;
				}
			}
			break;

			case 1:
			{
				if (Obj.m_PlayerData->m_WCoinC < this->m_PerMapAttr[Obj.MapNumber].CoinValue)
				{
					return false;
				}
				else
				{
						Obj.m_PlayerData->m_WCoinC -= this->m_PerMapAttr[Obj.MapNumber].CoinValue;
						GDReqInGameShopPointAdd(Obj.m_Index, 0, 0.00 - this->m_PerMapAttr[Obj.MapNumber].CoinValue);
						return true;
				}
			}
			break;

			case 2:
			{
				if (Obj.m_PlayerData->m_WCoinP < this->m_PerMapAttr[Obj.MapNumber].CoinValue )
				{
					return false;
				}
				else
				{
						Obj.m_PlayerData->m_WCoinP -= this->m_PerMapAttr[Obj.MapNumber].CoinValue;
						GDReqInGameShopPointAdd(Obj.m_Index, 1, 0.00 - this->m_PerMapAttr[Obj.MapNumber].CoinValue);
						return true;
				}
			}
			break;

			case 3:
			{
				if (Obj.m_PlayerData->m_GoblinPoint < this->m_PerMapAttr[Obj.MapNumber].CoinValue)
				{
					return false;
				}
				else
				{
					Obj.m_PlayerData->m_GoblinPoint -= this->m_PerMapAttr[Obj.MapNumber].CoinValue;
					GDReqInGameShopPointAdd(Obj.m_Index, 2, 0.00 - this->m_PerMapAttr[Obj.MapNumber].CoinValue);
					return true;
				}
			}
			break;
		}
	}
	return false;
}
void COfflineLevelling::Run()
{
	CGameObject lpObj;

	if (this->b_Enabled == false)
	{
		return;
	}

	EnterCriticalSection(&this->m_OfflevelCriti);
	for(std::map<int, OFF_LEVEL_PLAYERS>::iterator iter = m_OffPlayerData.begin(); iter != m_OffPlayerData.end(); iter++)
	{
		lpObj = getGameObject(iter->second.Obj.m_Index);
		if(!lpObj){
			this->DeleteUser(iter->second.Obj.m_Index);
			continue;
		}
		if(!Obj.m_bOffLevel){
			this->DeleteUser(Obj.m_Index);
			continue;
		}

		if(!Obj.m_bOff){
			this->DeleteUser(Obj.m_Index);
			continue;
		}

		if(GetTickCount() - iter->second.dwOffTime > this->m_General.ChargeInterval*1000)
		{
			iter->second.dwOffTime = GetTickCount();

			if(!this->ChargePlayer(Obj.m_Index))
			{
				Obj.m_bOff = false;
				Obj.m_bOffLevel = false;
			}
		}
	}
	LeaveCriticalSection(&this->m_OfflevelCriti);
}

int COfflineLevelling::GetSkillAttackType(WORD wSkillID)
{
/*	switch(wSkillID)
	{
	case 24:
	case 41:
		return DURATION_MAGIC;
	case 43:
		return MAGIC_ATTACK;
	}
	*/
	return this->m_SkillCategories[wSkillID].SkillType;
}

BOOL COfflineLevelling::SkillDistanceCheck(CGameObject &Obj, int aTargetIndex, int iSkillNum)
{

	if ( iSkillNum == 40 )
		return TRUE;

	if ( iSkillNum == 392 )
		return TRUE;

	int iSkillDistance = MagicDamageC.GetSkillDistance(iSkillNum);

	if ( iSkillDistance == -1 )
		return FALSE;

	iSkillDistance += 1;
	
	CGameObject lpTargetObj = getGameObject(aTargetIndex);

	if ( abs(Obj.X - lpTargetObj.X) > iSkillDistance ||
		 abs(Obj.Y - lpTargetObj.Y) > iSkillDistance )
	{
		return FALSE;
	}

	return TRUE;
}

int COfflineLevelling::GetOffLevelerCount()
{
	int counter = 0;
	for(int i=g_ConfigRead.server.GetObjectStartUserIndex();i<g_ConfigRead.server.GetObjectMax();i++)
	{
		if(getGameObject(i)->Connected == 3 && getGameObject(i)->Type == OBJ_USER && getGameObject(i)->m_bOffLevel )
		{
			counter++;
		}
	}
	return counter;
}

bool COfflineLevelling::CheckUseTime(CGameObject &Obj)
{
	

	if(!lpObj)
		return false;

	if(this->m_General.MaxDuration == 0 && Obj.m_PlayerData->VipType == 0 )
	{
		return false;
	}
	else if (this->m_General.MaxVipDuration == 0 && Obj.m_PlayerData->VipType != 0)
	{
		return false;
	}

	DWORD64 currentTick = GetTickCount64();

	DWORD64 useTime = (currentTick - Obj.m_dwOffLevelTime);

	if(Obj.m_PlayerData->VipType == 0)
	{
		if(useTime > (this->m_General.MaxDuration * 1000 * 3600))
		{
			Obj.m_bOff = false;
			Obj.m_bOffLevel = false;
			this->DeleteUser(Obj.m_Index);
			sLog->outBasic("[OffLevel][%d][%s][%s] Exceed max use time [Hours:%d] [Vip:%d]",Obj.m_Index,Obj.AccountID,Obj.Name,(useTime * 1000 *3600), Obj.m_PlayerData->VipType);
		}
	}
	else
	{
		if(useTime > (this->m_General.MaxVipDuration * 1000 * 3600))
		{
			Obj.m_bOff = false;
			Obj.m_bOffLevel = false;
			this->DeleteUser(Obj.m_Index);
			sLog->outBasic("[OffLevel][%d][%s][%s] Exceed max use time [Hours:%d] [Vip:%d]",Obj.m_Index,Obj.AccountID,Obj.Name,(useTime * 1000 *3600), Obj.m_PlayerData->VipType);
		}
	}

	return true;
	
	//
	
}

void COfflineLevelling::CheckAndPickUpItem(CGameObject &Obj, CMapItem* cMapItem, short tObjNum)
{
	bool bTakeItem = false;

	std::map<int, OFF_LEVEL_ITEM_PICK_LIST>::iterator It = this->m_PickItems.find(cMapItem->m_Type);

	if (It != this->m_PickItems.end())
	{
		bTakeItem = true;
	}

	else if(cMapItem->m_Type == ITEMGET(14,15) && this->m_PickItemSettings.PickUpZen == 1)
	{
		bTakeItem = true;
	}

	else if(cMapItem->m_NewOption > 0 && this->m_PickItemSettings.PickUpExc == 1)
	{
		bTakeItem = true;
	}

	else if(cMapItem->IsSetItem() && this->m_PickItemSettings.PickUpAncient == 1)
	{
		bTakeItem = true;
	}
	else if(gSocketItemType.CheckSocketItemType((int)&cMapItem) && m_PickItemSettings.PickUpSocket == 1)
	{
		bTakeItem = true;
	}
	else
	{
		bTakeItem = false;
	}

	if(bTakeItem)
	{
		PMSG_ITEMGETREQUEST pItemGetReq;
		pItemGetReq.NumberH = SET_NUMBERH(tObjNum);
		pItemGetReq.NumberL = SET_NUMBERL(tObjNum);
		GSProtocol.CGItemGetRequest(&pItemGetReq,Obj.m_Index, pItemGetReq.NumberH, pItemGetReq.NumberL);
	}
}

void COfflineLevelling::CheckRepairItems(CGameObject &Obj)
{
	

	for(int i=0;i<INVETORY_WEAR_SIZE;i++)
	{
		if(Obj.pntInventory[i]->IsItem())
		{
			if(Obj.pntInventory[i]->m_Durability < Obj.pntInventory[i]->m_BaseDurability)
			{
				if ( IsCashItem(Obj.pntInventory[i]->m_Type) == TRUE )
					continue;

				if (IsPremiumItem(Obj.pntInventory[i]->m_Type) == TRUE)
					continue;

				if ( Obj.pntInventory[i]->IsPeriodItem() == TRUE )
					continue;

				if ( Obj.pntInventory[i]->IsPeriodItemExpire() == TRUE )
					continue;

				if (g_LuckyItemManager.IsLuckyItemEquipment(Obj.pntInventory[i]->m_Type))	//season 6.1 add-on
					continue;

				if ( Obj.pntInventory[i]->m_Type == ITEMGET(13,20) && (Obj.pntInventory[i]->m_Level == 0 ||Obj.pntInventory[i]->m_Level == 1 ))
					continue;

				if ( Obj.pntInventory[i]->m_Type == ITEMGET(13,20) && Obj.pntInventory[i]->m_Level == 2 )
					continue;

				if ( Obj.pntInventory[i]->m_Type == ITEMGET(13,4) || Obj.pntInventory[i]->m_Type == ITEMGET(13,5) )
					continue;
				
				if ( Obj.pntInventory[i]->m_Type == ITEMGET(13,37) )
					continue;

				if ( Obj.pntInventory[i]->m_Type == ITEMGET(13,38) )
					continue;

				if ( Obj.pntInventory[i]->m_Type == ITEMGET(13,40) )
					continue;

				if ( Obj.pntInventory[i]->m_Type == ITEMGET(13,41) )
					continue;

				if ( Obj.pntInventory[i]->m_Type == ITEMGET(13,42) )
					continue;

				if ( Obj.pntInventory[i]->m_Type == ITEMGET(13,51) )
					continue;

				if ( Obj.pntInventory[i]->m_Type == ITEMGET(13,64) )
					continue;

				if ( Obj.pntInventory[i]->m_Type == ITEMGET(13,65) )
					continue;

				if ( Obj.pntInventory[i]->m_Type == ITEMGET(13,67) )
					continue;

				if ( Obj.pntInventory[i]->m_Type == ITEMGET(13,70) )
					continue;

				if ( Obj.pntInventory[i]->m_Type == ITEMGET(13,76) )
					continue;

				if ( Obj.pntInventory[i]->m_Type == ITEMGET(13,77) )
					continue;

				if ( Obj.pntInventory[i]->m_Type == ITEMGET(13,78) )
					continue;

				if ( Obj.pntInventory[i]->m_Type == ITEMGET(13,166) )
					continue;

				if ( Obj.pntInventory[i]->m_Type == ITEMGET(13,80) )
					continue;

				if ( Obj.pntInventory[i]->m_Type == ITEMGET(13,106) )
					continue;

				if ( Obj.pntInventory[i]->m_Type == ITEMGET(13,123) )
					continue;

				if ( Obj.pntInventory[i]->m_Type == ITEMGET(13,39) )
					continue;

				if ( Obj.pntInventory[i]->m_Type == ITEMGET(12,26) &&
					(Obj.pntInventory[i]->m_Level == 1 ||
						Obj.pntInventory[i]->m_Level == 2 || 
						Obj.pntInventory[i]->m_Level == 3 || 
						Obj.pntInventory[i]->m_Level == 4 || 
						Obj.pntInventory[i]->m_Level == 5 ))
						continue;

				if (!IsRepairItem(Obj.pntInventory[i]->m_Type))
					continue;

				GSProtocol.ItemDurRepaire(lpObj,&Obj.pntInventory[i],i,1);
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

