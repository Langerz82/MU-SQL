////////////////////////////////////////////////////////////////////////////////
// AppointItemDrop.cpp
#include "StdAfx.h"
#include "Logging/Log.h"
#include "LargeRand.h"
#include "User/CUserData.h"
#include "DSProtocol.h"
#include "gObjMonster.h"
#include "ItemOptionTypeMng.h"
#include "configread.h"
#include "AppointItemDrop.h"

CAppointItemDrop g_CAppointItemDrop;

CAppointItemDrop::CAppointItemDrop()
{
	this->m_mapItemDropInfo.clear();
	this->m_mapMonsterItemGroupLink.clear();
	this->m_bIsSuccessReadScript = false;
}

CAppointItemDrop::~CAppointItemDrop()
{

}

void CAppointItemDrop::LoadAppointItemDropScript()
{
	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(g_ConfigRead.GetPath("IGC_AppointItemDrop.xml"));

	if(res.status != pugi::status_ok)
	{
		sLog->outError("Error loading %s file (%s)", g_ConfigRead.GetPath("IGC_AppointItemDrop.xml"), res.description());
		return;
	}

	_ST_APP_DROP_ITEM_GROUP tmpDropItemGroup;
	_ST_APPOINTITEMDROP_DROP_ITEM tmpDropItem;
	_ST_APP_MONSTER_ITEMGROUP_LINK tmpMonsterItemGroupLink;

	this->m_bIsSuccessReadScript = false;
	this->m_mapItemDropInfo.clear();
	this->m_mapMonsterItemGroupLink.clear();

	pugi::xml_node mainXML = file.child("AppointItemDrop");
	pugi::xml_node monsterlist = mainXML.child("MonsterList");

	for (pugi::xml_node monster = monsterlist.child("Monster"); monster; monster = monster.next_sibling())
	{
		tmpMonsterItemGroupLink.iMonsterIndex = monster.attribute("ID").as_int();
		tmpMonsterItemGroupLink.iDropItemGroup = monster.attribute("DropGroup").as_int();

		std::map<int, _ST_APP_MONSTER_ITEMGROUP_LINK>::iterator It = this->m_mapMonsterItemGroupLink.find(tmpMonsterItemGroupLink.iMonsterIndex);

		if (It == this->m_mapMonsterItemGroupLink.end())
		{
			m_mapMonsterItemGroupLink.insert(std::pair<int, _ST_APP_MONSTER_ITEMGROUP_LINK>(tmpMonsterItemGroupLink.iMonsterIndex, tmpMonsterItemGroupLink));	
		}

		else	
		{
			sLog->outError("[IGC_AppointItemDrop.xml] Error - link already exists (%d) (%d)", tmpMonsterItemGroupLink.iMonsterIndex, tmpMonsterItemGroupLink.iDropItemGroup);
		}
	}

	pugi::xml_node itemdrop = mainXML.child("ItemDrop");

	for (pugi::xml_node dropgroup = itemdrop.child("DropGroup"); dropgroup; dropgroup = dropgroup.next_sibling())
	{
		tmpDropItemGroup.iDropItemGroup = dropgroup.attribute("ID").as_int();
		tmpDropItemGroup.iDropRateDenominator = dropgroup.attribute("Denominator").as_int();

		if (tmpDropItemGroup.iDropRateDenominator <= 0)
		{
			sLog->outError("[IGC_AppointItemDrop.xml] Error - Denominator is ZERO (Group:%d)", tmpDropItemGroup.iDropItemGroup);
			return;
		}

		tmpDropItemGroup.iTotalDropRate = 0;
		tmpDropItemGroup.vecDropItem.clear();

		for (pugi::xml_node item = dropgroup.child("Item"); item; item = item.next_sibling())
		{
			tmpDropItem.iDropItemGroup = tmpDropItemGroup.iDropItemGroup;
			tmpDropItem.iType = item.attribute("Cat").as_int();
			tmpDropItem.iIndex = item.attribute("Index").as_int();
			tmpDropItem.iMinLevel = item.attribute("MinLevel").as_int();
			tmpDropItem.iMaxLevel = item.attribute("MaxLevel").as_int();
			tmpDropItem.iSkill = item.attribute("Skill").as_int();
			tmpDropItem.iLuck = item.attribute("Luck").as_int();
			tmpDropItem.iAddoption = item.attribute("Option").as_int();
			tmpDropItem.iExcelent = item.attribute("Exc").as_int();
			tmpDropItem.iDropRate = item.attribute("DropRate").as_int();

			if (ItemGetNumberMake(tmpDropItem.iType, tmpDropItem.iIndex) == -1)
			{
				sLog->outError("[ !! ERROR !! ]  at 'IGC_AppointItemDrop.xml' Wrong Item Index Found Type:%d, Index:%d", tmpDropItem.iType, tmpDropItem.iIndex);
			}

			tmpDropItemGroup.vecDropItem.push_back(tmpDropItem);
			tmpDropItemGroup.iTotalDropRate += tmpDropItem.iDropRate;
		}

		this->m_mapItemDropInfo.insert(std::pair<int, _ST_APP_DROP_ITEM_GROUP>(tmpDropItemGroup.iDropItemGroup, tmpDropItemGroup));
	}	

	this->m_bIsSuccessReadScript = true;
}

BOOL CAppointItemDrop::AppointItemDrop(LPGameObject lpObj, OBJECTSTRUCT *lpMonsterObj)
{
	if (this->m_bIsSuccessReadScript == false)
	{
		return FALSE;
	}
		
	std::map<int, _ST_APP_MONSTER_ITEMGROUP_LINK>::iterator iterMonsterItemGroupLink = this->m_mapMonsterItemGroupLink.find(lpMonsterObj->Class);

	if (iterMonsterItemGroupLink == m_mapMonsterItemGroupLink.end())
	{
		return FALSE;
	}

	std::map<int, _ST_APP_DROP_ITEM_GROUP>::iterator iterMonsterItemDropInfo = m_mapItemDropInfo.find(iterMonsterItemGroupLink->second.iDropItemGroup);

	if (iterMonsterItemDropInfo == m_mapItemDropInfo.end())
	{
		return FALSE;
	}

	DWORD iSelectedDropRate = GetLargeRand() % iterMonsterItemDropInfo->second.iDropRateDenominator;
	
	if (iSelectedDropRate > iterMonsterItemDropInfo->second.iTotalDropRate)
	{
		return FALSE;
	}
	
	int iLeft = iterMonsterItemDropInfo->second.iTotalDropRate;
	int iRight = iterMonsterItemDropInfo->second.iTotalDropRate;
	
	for (std::vector<_ST_APPOINTITEMDROP_DROP_ITEM>::reverse_iterator iterDropItem = iterMonsterItemDropInfo->second.vecDropItem.rbegin(); iterDropItem != iterMonsterItemDropInfo->second.vecDropItem.rend(); iterDropItem++)
	{
		iLeft -= iterDropItem->iDropRate;
		if (iLeft <= iSelectedDropRate && iSelectedDropRate < iRight)
		{
			int iType = ItemGetNumberMake(iterDropItem->iType, iterDropItem->iIndex);
			int iLevel = rand() % (iterDropItem->iMaxLevel + 1 - iterDropItem->iMinLevel);
			iLevel += iterDropItem->iMinLevel;
			BYTE iX = lpObj->X;
			BYTE iY = lpObj->Y;
			int iDur = ItemGetDurability(iType, iLevel, iterDropItem->iExcelent, 0);
			int iSkill = iterDropItem->iSkill;
			int iLuck = iterDropItem->iLuck;
			int iOption3 = 0;
			BYTE btSocketOption[5];
			memset(btSocketOption, -1, sizeof(btSocketOption));

			if (iterDropItem->iAddoption)
			{
				if (rand() % 5 >= 1)
				{
					iOption3 = rand() % 3;
				}

				else
				{
					iOption3 = 3;
				}
			}

			BYTE byExOption = 0;
			LPITEM_ATTRIBUTE p = &ItemAttribute[iType];

			if (iterDropItem->iExcelent)
			{
				if (p->ItemKindA == ITEM_KIND_A_WING)
				{
					byExOption = g_ItemOptionTypeMng.WingExcOptionRand(p->ItemKindA, p->ItemKindB, btSocketOption);
				}

				else
				{
					byExOption = g_ItemOptionTypeMng.CommonExcOptionRand(p->ItemKindA, btSocketOption);
				}
			}

			else
			{
				byExOption = 0;
			}

			int iMaxHitUser = gObjMonsterTopHitDamageUser(lpMonsterObj);
			ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, iX, iY, iType, iLevel, iDur, iSkill, iLuck, iOption3, iMaxHitUser, byExOption, 0, 0, btSocketOption, 0);
			
			return TRUE;
		}
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

