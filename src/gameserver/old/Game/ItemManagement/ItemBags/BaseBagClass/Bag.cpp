////////////////////////////////////////////////////////////////////////////////
// Bag.cpp
#include "Bag.h"
#include "User/CUserData.h"
#include "LuaBag.h"
#include "Main.h"
#include "GameProtocol.h"
#include "LargeRand.h"
#include "MasterLevelSkillTreeSystem.h"
#include "CustomMaxStats.h"

const char * szFileClassName [] = { "Wizard", "Knight", "FairyElf", "MagicGladiator", "DarkLord", "Summoner", "RageFighter", "GrowLancer" };

CBag::CBag()
{
	this->InitBag();
}

CBag::~CBag()
{
	this->m_BagData.m_vtSectionDrop.clear();
}

bool CBag::LoadBag(const char* lpFileName)
{
	this->InitBag();

	pugi::xml_document file;
	pugi::xml_parse_result result = file.load_file(lpFileName);

	if (result.status != pugi::status_ok)
	{
		sLog->outError("Failed to load %s file!", lpFileName);
		return false;
	}

	pugi::xml_node mainXML_section = file.child("ItemBag");
	pugi::xml_node config_section = mainXML_section.child("BagConfig");
	pugi::xml_node coin_section = mainXML_section.child("AddCoin");
	pugi::xml_node summon_section = mainXML_section.child("SummonBook");
	pugi::xml_node ruud = mainXML_section.child("Ruud");

	strcpy(this->m_BagData.szBagName, config_section.attribute("Name").as_string());
	this->m_BagData.dwItemDropRate = config_section.attribute("ItemRate").as_int(0);
	this->m_BagData.dwRandomSetItemDropRate = config_section.attribute("SetItemRate").as_int(0);
	this->m_BagData.dwDropMoney = config_section.attribute("MoneyDrop").as_int(0);
	this->m_BagData.iBagUseEffect = config_section.attribute("BagUseEffect").as_int(-1);
	this->m_BagData.dwBagUseRate = config_section.attribute("BagUseRate").as_int(0);

	this->m_BagData.bAddCoinEnable = coin_section.attribute("Enable").as_bool(false);
	this->m_BagData.btAddCoinType = coin_section.attribute("CoinType").as_int(0);
	this->m_BagData.dwAddCoinValue = coin_section.attribute("CoinValue").as_int(0);

	this->m_BagData.bSummonBookEnable = summon_section.attribute("Enable").as_bool(false);
	this->m_BagData.dwSummonBookDropRate = summon_section.attribute("DropRate").as_int(0);
	this->m_BagData.btSummonBookItemType = summon_section.attribute("ItemCat").as_int(0);
	this->m_BagData.wSummonBookItemIndex = summon_section.attribute("ItemIndex").as_int(0);

	this->m_BagData.dwGainRuudRate = ruud.attribute("GainRate").as_int();
	this->m_BagData.dwMinGainRuud = ruud.attribute("MinValue").as_int();
	this->m_BagData.dwMaxGainRuud = ruud.attribute("MaxValue").as_int();

	if(this->m_BagData.dwItemDropRate < 10000 && this->m_BagData.dwDropMoney == 0)
	{
		sLog->outError("Error - %s bag has ItemDropRate lower than maximum and DropMoney is zero", lpFileName);
	}

	int SectionNumber;
	BAG_SECTION_DROP m_DropSection;
	BAG_SECTION_ITEMS m_ItemSection;
	BAG_ITEM m_Item;

	for (pugi::xml_node section = mainXML_section.child("DropAllow"); section; section = section.next_sibling())
	{
		SectionNumber = 1;

		for (int i = 0; i < MAX_TYPE_PLAYER; i++)
		{
			m_DropSection.btUseSectionAllowedClass[i] = section.attribute(szFileClassName[i]).as_int();
		}

		m_DropSection.wUseSectionMinLevel = section.attribute("PlayerMinLevel").as_int(1);

		if (strcmp("MAX", section.attribute("PlayerMaxLevel").as_string()) == 0)
		{
			m_DropSection.wUseSectionMaxLevel = g_ConfigRead.data.common.UserMaxLevel + g_MaxStatsInfo.GetClass.MLUserMaxLevel;
		}

		else
		{
			m_DropSection.wUseSectionMaxLevel = section.attribute("PlayerMaxLevel").as_int(820);
		}

		m_DropSection.btUseSectionAllowedMapNum = section.attribute("MapNumber").as_int(-1);

		if ( m_DropSection.btUseSectionAllowedMapNum != 0xFF && MAX_MAP_RANGE(m_DropSection.btUseSectionAllowedMapNum) == FALSE )
		{
			sLog->outError("Error: Bag (%s), DropAllow Section:%d, btUseSectionAllowedMapNum value is wrong", lpFileName, m_DropSection.btUseSectionAllowedMapNum);
			continue;
		}

		for (pugi::xml_node dropsec = section.child("Drop"); dropsec; dropsec = dropsec.next_sibling())
		{
			m_ItemSection.btSectionID = SectionNumber;
			m_ItemSection.dwSectionUseRate = dropsec.attribute("Rate").as_int();
			m_ItemSection.btItemDropCount = dropsec.attribute("Count").as_int();
			m_ItemSection.m_vtItems.clear();

			for (pugi::xml_node item = dropsec.child("Item"); item; item = item.next_sibling())
			{
				m_Item.btItemType = item.attribute("Cat").as_int(-1);
				m_Item.wItemIndex = item.attribute("Index").as_int(-1);
				m_Item.btItemMinLevel = item.attribute("ItemMinLevel").as_int(0);
				m_Item.btItemMaxLevel = item.attribute("ItemMaxLevel").as_int(0);
				m_Item.btItemDur = item.attribute("Durability").as_int(0);
				m_Item.btIsSkill = item.attribute("Skill").as_int(0);
				m_Item.btIsLuck = item.attribute("Luck").as_int(0);
				m_Item.btIsOption = item.attribute("Option").as_int(0);

				std::string szExcOptions = item.attribute("Exc").as_string();
				std::deque<int> exc_opt_list;
				strtk::parse(szExcOptions, ";", exc_opt_list);

				memset(m_Item.btIsExItem, -1, sizeof(m_Item.btIsExItem));

				for (int i = 0; i < exc_opt_list.size(); i++)
				{
					m_Item.btIsExItem[i] = exc_opt_list.at(i);
				}

				m_Item.btIsSetItem = item.attribute("SetItem").as_int(0);
				m_Item.btIsSocketItem = item.attribute("SocketCount").as_int(0);
				m_Item.btIsElementalItem = item.attribute("ElementalItem").as_int(0);
				m_Item.btMuunEvolutionItemType = item.attribute("MuunEvolutionItemCat").as_int(-1);
				m_Item.wMuunEvolutionItemIndex = item.attribute("MuunEvolutionItemIndex").as_int(-1);
				m_Item.dwDuration = item.attribute("Duration").as_int(0);

				if (ItemGetNumberMake(m_Item.btItemType, m_Item.wItemIndex) == -1)
				{
					sLog->outError("Error: Wrong Item in %s bag (Cat: %d Index: %d)", this->m_BagData.szBagName, m_Item.btItemType, m_Item.wItemIndex);
					continue;
				}

				m_ItemSection.m_vtItems.push_back(m_Item);
			}

			m_DropSection.m_mapSectionItems.insert(std::pair<int, BAG_SECTION_ITEMS>(SectionNumber, m_ItemSection));
			SectionNumber++;
		}

		this->m_BagData.m_vtSectionDrop.push_back(m_DropSection);
	}

	this->m_BagData.m_bLoadBag = true;
	return true;
}

void CBag::InitBag()
{
	memset(this->m_BagData.szBagName, 0x00, sizeof(this->m_BagData.szBagName));
	this->m_BagData.dwDropMoney = 0;
	this->m_BagData.dwItemDropRate = 0;
	this->m_BagData.iBagUseEffect = -1;
	this->m_BagData.m_vtSectionDrop.clear();
	this->m_BagData.m_bLoadBag = false;
}

int CBag::GetDropSection(CGameObject &Obj, BAG_SECTION_DROP &pRetDrop)
{
	if (this->m_BagData.m_bLoadBag == false)
	{
		return FALSE;
	}

	if (Obj.Type != OBJ_USER)
	{
		return FALSE;
	}

	if (this->m_BagData.m_vtSectionDrop.size() <= 0)
	{
		return FALSE;
	}

	
	TRandomPoolMgr m_RandomPoolSection;

	m_RandomPoolSection.InitPool();
	int SectionID = 1;

	for (std::vector<BAG_SECTION_DROP>::iterator It = this->m_BagData.m_vtSectionDrop.begin(); It != this->m_BagData.m_vtSectionDrop.end(); It++, SectionID++)
	{
		if (Obj.MapNumber != It->btUseSectionAllowedMapNum && It->btUseSectionAllowedMapNum != 0xFF)
		{
			continue;
		}

		if (It->btUseSectionAllowedClass[Obj.Class] == 0)
		{
			continue;
		}

		if (It->btUseSectionAllowedClass[Obj.Class] > 1)
		{
			if (It->btUseSectionAllowedClass[Obj.Class] > (Obj.m_PlayerData->ChangeUP + 1))
			{
				continue;
			}
		}

		if ((Obj.Level + Obj.m_PlayerData->MasterLevel) < It->wUseSectionMinLevel)
		{
			continue;
		}

		if ((Obj.Level + Obj.m_PlayerData->MasterLevel) > It->wUseSectionMaxLevel)
		{
			continue;
		}

		m_RandomPoolSection.AddValue(SectionID, 0);
	}

	int RetSection = m_RandomPoolSection.GetRandomValue(eRANDOMPOOL_BY_RANDOM);

	if (RetSection == 0)
	{
		return FALSE;
	}

	pRetDrop = this->m_BagData.m_vtSectionDrop[RetSection-1];

	return TRUE;
}

int CBag::GetItemsSection(BAG_SECTION_DROP &pDropSec, BAG_SECTION_ITEMS &pRetItems)
{
	TRandomPoolMgr m_RandomPoolSection;

	m_RandomPoolSection.InitPool();

	for (std::map<int, BAG_SECTION_ITEMS>::iterator It = pDropSec.m_mapSectionItems.begin(); It != pDropSec.m_mapSectionItems.end(); It++)
	{
		m_RandomPoolSection.AddValue(It->second.btSectionID, It->second.dwSectionUseRate);
	}

	int SectionID = m_RandomPoolSection.GetRandomValue(eRANDOMPOOL_BY_WEIGHT);

	if (SectionID == 0)
	{
		return FALSE;
	}

	pRetItems = pDropSec.m_mapSectionItems[SectionID];

	return TRUE;
}

int CBag::GetItem(BAG_SECTION_ITEMS &pItemsSec, BAG_ITEM & pRetItem)
{
	if (pItemsSec.m_vtItems.size() == 0)
	{
		return FALSE;
	}

	int BagSize = pItemsSec.m_vtItems.size();
	int ItemNum = rand() % BagSize;

	pRetItem = pItemsSec.m_vtItems[ItemNum];

	return TRUE;
}

int CBag::GetReadyItemToUse(CGameObject &Obj, CItemObject &pItem, time_t & DurationItem)
{
	

	BAG_ITEM m_Item;
	BAG_SECTION_ITEMS m_ItemSection;
	BAG_SECTION_DROP m_DropSection;

	if (rand() % 10000 >= this->m_BagData.dwItemDropRate)
	{
		Obj.m_PlayerData->Money += this->m_BagData.dwDropMoney;
		GSProtocol.GCMoneySend(&Obj, Obj.m_PlayerData->Money);
		MsgOutput(&Obj, "Obtained %d Zen", this->m_BagData.dwDropMoney);

		return 2;
	}

	if (rand() % 10000 < this->m_BagData.dwGainRuudRate)
	{
		int iRuudValue = this->GetValueMinMax(this->m_BagData.dwMinGainRuud, this->m_BagData.dwMaxGainRuud);
		Obj.m_PlayerData->Ruud += iRuudValue;

		GSProtocol.GCSendRuud(&Obj, Obj.m_PlayerData->Ruud, iRuudValue, true);
		return 3;
	}

	int iResult = this->GetDropSection(Obj, m_DropSection);

	if (iResult == FALSE)
	{
		return 0;
	}

	iResult = this->GetItemsSection(m_DropSection, m_ItemSection);

	if (iResult == FALSE)
	{
		return 0;
	}

	if (this->GetItem(m_ItemSection, m_Item) == FALSE)
	{
		return 0;
	}
	bool bResult = gLuaBag.MakeItemFromBag(Obj, &m_Item, pItem, DurationItem);

	if (bResult == false)
	{
		return 0;
	}

	return 1;
}

int CBag::GetItemLevel(int MinLevel, int MaxLevel)
{
	if (MinLevel == MaxLevel)	return MinLevel;

	int sub, level = 0;

	__try
	{
		sub = (MaxLevel - MinLevel) + 1;
		level = MinLevel + (rand() % sub);
	}

	__except (sub = 1, 1)
	{
		return 0;
	}

	return level;
}

void CBag::MakeBagEffectUse(CGameObject &Obj, int iMonsterIndex)
{
	PMSG_SERVERCMD ServerCmd;

	PHeadSubSetB((BYTE*)&ServerCmd, 0xF3, 0x40, sizeof(ServerCmd));
	ServerCmd.CmdType = this->m_BagData.iBagUseEffect;

	switch (this->m_BagData.iBagUseEffect)
	{
		case 0:
			ServerCmd.X = getGameObject(iMonsterIndex)->X;
			ServerCmd.Y = getGameObject(iMonsterIndex)->Y;
			break;
		case 2:
			ServerCmd.X = getGameObject(iMonsterIndex)->X;
			ServerCmd.Y = getGameObject(iMonsterIndex)->Y;
			break;
		case 58:
			ServerCmd.X = SET_NUMBERH(Obj.m_Index);
			ServerCmd.Y = SET_NUMBERL(Obj.m_Index);
			break;
	}

	GSProtocol.MsgSendV2(getGameObject(Obj.m_Index), (BYTE*)&ServerCmd, ServerCmd.h.size);
	GIOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&ServerCmd, ServerCmd.h.size);
}

void CBag::AddCoin(CGameObject &Obj)
{

	if (this->m_BagData.bAddCoinEnable == false)
	{
		return;
	}

	

	char *CoinTypes[] = { "C", "P", "G" };
	GDReqInGameShopPointAdd(Obj, this->m_BagData.btAddCoinType, this->m_BagData.dwAddCoinValue);
	MsgOutput(Obj, Lang.GetText(0,502), CoinTypes[this->m_BagData.btAddCoinType], this->m_BagData.dwAddCoinValue);
	sLog->outBasic("[Bag Manager] (%s)(%s) Add Coin: (Type:%d) (Amount:%d) (Bag:%s)", Obj.AccountID, Obj.Name, this->m_BagData.btAddCoinType, this->m_BagData.dwAddCoinValue, this->m_BagData.szBagName);
}

void CBag::DropSummonItem(CGameObject &Obj)
{

	if (this->m_BagData.bSummonBookEnable == false)
	{
		return;
	}

	

	if (Obj.m_bIsHelpMon == true)
	{
		return;
	}

	if (GetLargeRand() % 1000000 <= this->m_BagData.dwSummonBookDropRate)
	{
		int iType = ItemGetNumberMake(this->m_BagData.btSummonBookItemType, this->m_BagData.wSummonBookItemIndex);

		if (iType == -1)
		{
			return;
		}

		ItemCreate(Obj, Obj.MapNumber, Obj.X, Obj.Y, iType, 0, -1, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
	}
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

