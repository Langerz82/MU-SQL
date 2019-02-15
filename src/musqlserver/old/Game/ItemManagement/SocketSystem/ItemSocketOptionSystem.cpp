#include "ItemSocketOptionSystem.h"
#include "LargeRand.h"
#include "CUserData.h"
#include "GameProtocol.h"

CItemObjectSocketOptionSystem	g_SocketOptionSystem;

CItemObjectSocketOptionSystem::CItemObjectSocketOptionSystem()
{

}

CItemObjectSocketOptionSystem::~CItemObjectSocketOptionSystem()
{

}

void CItemObjectSocketOptionSystem::Initialize()
{
	int iClearIndex = 0;

	for (iClearIndex = 0; iClearIndex < 50; iClearIndex++)
	{
		m_SeedData[iClearIndex].Clear();
	}

	for (iClearIndex = 0; iClearIndex < 5; iClearIndex++)
	{
		m_SphereData[iClearIndex].Clear();
	}

	for (iClearIndex = 0; iClearIndex < 12; iClearIndex++)
	{
		m_BonusSocketOptionData[iClearIndex].Clear();
	}

	m_SeedExtractPool.InitPool();

	for (iClearIndex = 0; iClearIndex < 5; iClearIndex++)
	{
		m_SphereDropRate[iClearIndex] = 0;
		m_SphereDropLevel[iClearIndex] = 0;
	}
}

bool CItemObjectSocketOptionSystem::LoadOptionScript(char *pchFileName)
{
	Initialize();

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(pchFileName);

	if (res.status != pugi::status_ok)
	{
		sLog->outError("[SocketOption] SocketOption Script file Load failed. (%s) (%s)", pchFileName, res.description());
		return false;
	}

	int iCategoryIndex = 0;
	int iSeedOptionDataCount = 0;
	int iSphereOptionDataCount = 0;
	int iBonusSocketOptionDataCount = 0;
	int iSetSocketOptionDataCount = 0;

	pugi::xml_node mainXML = file.child("SocketSystem");
	pugi::xml_node item_option = mainXML.child("SocketItemOptionSettings");

	for (pugi::xml_node option = item_option.child("Option"); option; option = option.next_sibling())
	{
		BYTE btSeedIndex = 0;
		BYTE btSeedType = 0;
		BYTE btItemLevel = 0;
		BYTE btOptionValueType = 0;
		WORD wOptionValue[5] = { 0 };

		int iExtractRate = 0;
		int iItemCode = 0;

		btSeedIndex = option.attribute("Index").as_int();
		btSeedType = option.attribute("ElementType").as_int();
		btItemLevel = option.attribute("Level").as_int();
		btOptionValueType = option.attribute("BonusType").as_int();
		wOptionValue[0] = option.attribute("BonusValue1").as_int();
		wOptionValue[1] = option.attribute("BonusValue2").as_int();
		wOptionValue[2] = option.attribute("BonusValue3").as_int();
		wOptionValue[3] = option.attribute("BonusValue4").as_int();
		wOptionValue[4] = option.attribute("BonusValue5").as_int();
		iExtractRate = option.attribute("PoolValue").as_int();

		m_SeedExtractPool.AddValue(btSeedIndex, iExtractRate);
		m_SeedData[iSeedOptionDataCount].SetOption(btSeedIndex, btSeedType, btItemLevel, btOptionValueType, wOptionValue, iExtractRate);
		iSeedOptionDataCount++;

		if (iSeedOptionDataCount > 50)
		{
			sLog->outError("[ERROR] MAX SeedSphere Option Data Overflow! [%d]", iSeedOptionDataCount);
			break;
		}
	}

	pugi::xml_node bonus_option = mainXML.child("SocketBonusSettings");

	for (pugi::xml_node option = bonus_option.child("Option"); option; option = option.next_sibling())
	{
		BONUS_SOCKET_OPTION BonusOptionData;
		memset(&BonusOptionData, 0, sizeof(BONUS_SOCKET_OPTION));

		BonusOptionData.OptionIndex = option.attribute("Index").as_int();
		BonusOptionData.ItemTypeBegin = option.attribute("MinItemType").as_int();
		BonusOptionData.ItemTypeEnd = option.attribute("MaxItemType").as_int();

		BonusOptionData.OptionValueType = option.attribute("BonusType").as_int();
		BonusOptionData.OptionValue = option.attribute("BonusValue").as_int();
		BonusOptionData.ExtractRate = option.attribute("BonusRate").as_int();
		BonusOptionData.SocketSlotType[0] = option.attribute("ElementType1").as_int();
		BonusOptionData.SocketSlotType[1] = option.attribute("ElementType2").as_int();
		BonusOptionData.SocketSlotType[2] = option.attribute("ElementType3").as_int();
		BonusOptionData.SocketSlotType[3] = option.attribute("ElementType4").as_int();
		BonusOptionData.SocketSlotType[4] = option.attribute("ElementType5").as_int();

		for (int i = 0; i < 5; i++)
		{
			if (BonusOptionData.SocketSlotType[i] != NULL)
			{
				BonusOptionData.SocketSlotCount++;
			}
		}

		switch (BonusOptionData.OptionIndex)
		{
		case BONUS_OPTION_WEAPON_ATTACK_POWER:
		case BONUS_OPTION_WEAPON_ATTACK_POWER_4TH:
			BonusOptionData.EffectType = SEED_EFFECT_TYPE_FIRE_BEGIN;			
			break;
		case BONUS_OPTION_WEAPON_ATTACK_SKILL_POWER:
		case BONUS_OPTION_WEAPON_ATTACK_SKILL_POWER_4TH:
			BonusOptionData.EffectType = SEED_EFFECT_TYPE_ATTACK_SKILL_POWER;	
			break;
		case BONUS_OPTION_STAFF_ATTACK_POWER:
		case BONUS_OPTION_STAFF_ATTACK_POWER_4TH:
			BonusOptionData.EffectType = SEED_EFFECT_TYPE_FIRE_BEGIN;			
			break;
		case BONUS_OPTION_STAFF_ATTACK_SKILL_POWER:
		case BONUS_OPTION_STAFF_ATTACK_SKILL_POWER_4TH:
			BonusOptionData.EffectType = SEED_EFFECT_TYPE_ATTACK_SKILL_POWER;	
			break;
		case BONUS_OPTION_PROTECT_BLOCK_POWER:
		case BONUS_OPTION_PROTECT_BLOCK_POWER_4TH:
			BonusOptionData.EffectType = SEED_EFFECT_TYPE_BLOCK_POWER;			
			break;
		case BONUS_OPTION_PROTECT_MAX_HP_INC:
		case BONUS_OPTION_PROTECT_MAX_HP_INC_4TH:
			BonusOptionData.EffectType = SEED_EFFECT_TYPE_MAX_HP_INC;			
			break;
		}

		std::memcpy(&m_BonusSocketOptionData[iBonusSocketOptionDataCount], &BonusOptionData, sizeof(BONUS_SOCKET_OPTION));
		iBonusSocketOptionDataCount++;
	}

	pugi::xml_node set_option = mainXML.child("SpecialOptionSettings");

	for (pugi::xml_node option = set_option.child("Option"); option; option = option.next_sibling())
	{
		int iSetSocketOptionIndex = 0;

		BYTE btSetSocketType = 0;
		BYTE btSetSocketOptionValueType = 0;
		BYTE btSetSocketOptionValue = 0;
		int iExtractRate = 0;
		BYTE btSetSocketTypeIndex[6] = { 0 };

		iSetSocketOptionIndex = option.attribute("Index").as_int();
		btSetSocketType = option.attribute("Type").as_int();
		btSetSocketOptionValueType = option.attribute("BonusType").as_int();
		btSetSocketOptionValue = option.attribute("BonusValue").as_int();
		iExtractRate = option.attribute("BonusRate").as_int();
		btSetSocketTypeIndex[0] = option.attribute("SeedCountFire").as_int();
		btSetSocketTypeIndex[1] = option.attribute("SeedCountWater").as_int();
		btSetSocketTypeIndex[2] = option.attribute("SeedCountIce").as_int();
		btSetSocketTypeIndex[3] = option.attribute("SeedCountWind").as_int();
		btSetSocketTypeIndex[4] = option.attribute("SeedCountLight").as_int();
		btSetSocketTypeIndex[5] = option.attribute("SeedCountEarth").as_int();

		m_SetSocketOptionData[iSetSocketOptionDataCount].OptionIndex = iSetSocketOptionIndex;

		m_SetSocketOptionData[iSetSocketOptionDataCount].OptionValueType = btSetSocketOptionValueType;
		m_SetSocketOptionData[iSetSocketOptionDataCount].OptionValue = btSetSocketOptionValue;

		m_SetSocketOptionData[iSetSocketOptionDataCount].ExtractRate = iExtractRate;

		m_SetSocketOptionData[iSetSocketOptionDataCount].SeedTypeTable[0] = btSetSocketTypeIndex[0];
		m_SetSocketOptionData[iSetSocketOptionDataCount].SeedTypeTable[1] = btSetSocketTypeIndex[1];
		m_SetSocketOptionData[iSetSocketOptionDataCount].SeedTypeTable[2] = btSetSocketTypeIndex[2];
		m_SetSocketOptionData[iSetSocketOptionDataCount].SeedTypeTable[3] = btSetSocketTypeIndex[3];
		m_SetSocketOptionData[iSetSocketOptionDataCount].SeedTypeTable[4] = btSetSocketTypeIndex[4];
		m_SetSocketOptionData[iSetSocketOptionDataCount].SeedTypeTable[5] = btSetSocketTypeIndex[5];

		switch (iSetSocketOptionIndex)
		{
		case SET_OPTION_DOUBLE_DAMAGE_RATE_INC:	
			m_SetSocketOptionData[iSetSocketOptionDataCount].EffectType = SEED_EFFECT_TYPE_SET_DOUBLE_DAMAGE_RATE_INC;	
			break;
		case SET_OPTION_IGNORE_ENEMY_DEFENSE:	
			m_SetSocketOptionData[iSetSocketOptionDataCount].EffectType = SEED_EFFECT_TYPE_SET_IGNORE_ENERMY_DEFENSE;	
			break;
		}

		iSetSocketOptionDataCount++;
	}

	LoadScript();

	return true;
}

void CItemObjectSocketOptionSystem::LoadScript()
{
	m_mapSocketSlotCountPool.clear();
	m_TwoHandWeaponMakeSocketSlotCountPool.InitPool();

	LPSTR pchFileName = g_ConfigRead.GetPath("\\SocketSystem\\IGC_SocketSystem.xml");

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(pchFileName);

	if (res.status != pugi::status_ok)
	{
		sLog->outError("%s load fail (%s)", pchFileName, res.description());
		return;
	}

	pugi::xml_node mainXML = file.child("SocketSystem");

	m_SphereDropOn = mainXML.attribute("SphereDrop").as_int();
	m_TetraDropOn = mainXML.attribute("TetraDrop").as_int();

	pugi::xml_node sphere_drop = mainXML.child("SphereDrop");
	pugi::xml_node sphere_rate = sphere_drop.child("Rate");
	pugi::xml_node sphere_level = sphere_drop.child("Level");

	int iCount = 0;
	for (pugi::xml_attribute rate = sphere_rate.first_attribute(); rate; rate = rate.next_attribute())
	{
		m_SphereDropRate[iCount++] = rate.as_int();
	}

	iCount = 0;
	for (pugi::xml_attribute level = sphere_level.first_attribute(); level; level = level.next_attribute())
	{
		m_SphereDropLevel[iCount++] = level.as_int();
	}

	pugi::xml_node tetra_drop = mainXML.child("TetraDrop");
	pugi::xml_node tetra_bronze = tetra_drop.child("Bronze");
	pugi::xml_node tetra_silver = tetra_drop.child("Silver");
	pugi::xml_node tetra_gold = tetra_drop.child("Gold");

	m_TetraDropRate[0] = tetra_bronze.attribute("Rate").as_int();
	m_TetraDropRate[1] = tetra_silver.attribute("Rate").as_int();
	m_TetraDropRate[2] = tetra_gold.attribute("Rate").as_int();

	m_TetraDropMinLevel[0] = tetra_bronze.attribute("LevelMin").as_int();
	m_TetraDropMinLevel[1] = tetra_silver.attribute("LevelMin").as_int();
	m_TetraDropMinLevel[2] = tetra_gold.attribute("LevelMin").as_int();

	m_TetraDropMaxLevel[0] = tetra_bronze.attribute("LevelMax").as_int();
	m_TetraDropMaxLevel[1] = tetra_silver.attribute("LevelMax").as_int();
	m_TetraDropMaxLevel[2] = tetra_gold.attribute("LevelMax").as_int();

	pugi::xml_node seed_mixes = mainXML.child("Seed");
	pugi::xml_node seedsphere_mixes = seed_mixes.child("Sphere");

	m_RequireMoneyForSeedExtract = seed_mixes.attribute("ExtractMoney").as_int();
	m_RequireMoneyForSeedSphereComposite = seedsphere_mixes.attribute("CompositeMoney").as_int();
	m_RequireMoneyForSeedSphereSet = seedsphere_mixes.attribute("SetMoney").as_int();
	m_RequireMoneyForSeedSphereRemove = seedsphere_mixes.attribute("RemoveMoney").as_int();

	pugi::xml_node socketrate = mainXML.child("SocketRateSettings");

	int ItemCat;
	int SlotRate[5];
	TRandomPoolMgr Pool;

	for (pugi::xml_node socket = socketrate.child("Socket"); socket; socket = socket.next_sibling())
	{
		Pool.InitPool();

		ItemCat = socket.attribute("CatID").as_int();
		SlotRate[0] = socket.attribute("Count1").as_int();
		SlotRate[1] = socket.attribute("Count2").as_int();
		SlotRate[2] = socket.attribute("Count3").as_int();
		SlotRate[3] = socket.attribute("Count4").as_int();
		SlotRate[4] = socket.attribute("Count5").as_int();

		for (int i = 0; i < 5; i++)
		{
			Pool.AddValue(i + 1, SlotRate[i]);
		}

		m_mapSocketSlotCountPool.insert(std::pair<int, TRandomPoolMgr>(ItemCat, Pool));
	}

	pugi::xml_node twohand = socketrate.child("TwoHandWeapons");

	SlotRate[0] = twohand.attribute("Count1").as_int();
	SlotRate[1] = twohand.attribute("Count2").as_int();
	SlotRate[2] = twohand.attribute("Count3").as_int();
	SlotRate[3] = twohand.attribute("Count4").as_int();
	SlotRate[4] = twohand.attribute("Count5").as_int();

	for (int i = 0; i < 5; i++)
	{
		m_TwoHandWeaponMakeSocketSlotCountPool.AddValue(i + 1, SlotRate[i]);
	}
}

bool CItemObjectSocketOptionSystem::LoadSocketSlotRateFile(char *pchFileName)
{
	this->m_mapSocketSlotCountPool.clear();
	this->m_TwoHandWeaponMakeSocketSlotCountPool.InitPool();

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(pchFileName);

	if (res.status != pugi::status_ok)
	{
		sLog->outError("%s load fail (%s)", pchFileName, res.description());
		return false;
	}

	int ItemCat;
	int SlotRate[5];
	TRandomPoolMgr Pool;

	pugi::xml_node mainXML = file.child("SocketRateSettings");

	for (pugi::xml_node socket = mainXML.child("Socket"); socket; socket = socket.next_sibling())
	{
		Pool.InitPool();

		ItemCat = socket.attribute("CatID").as_int();
		SlotRate[0] = socket.attribute("Count1").as_int();
		SlotRate[1] = socket.attribute("Count2").as_int();
		SlotRate[2] = socket.attribute("Count3").as_int();
		SlotRate[3] = socket.attribute("Count4").as_int();
		SlotRate[4] = socket.attribute("Count5").as_int();

		for (int i = 0; i < 5; i++)
		{
			Pool.AddValue(i + 1, SlotRate[i]);
		}

		this->m_mapSocketSlotCountPool.insert(std::pair<int, TRandomPoolMgr>(ItemCat, Pool));
	}

	pugi::xml_node twohand = mainXML.child("TwoHandWeapons");

	SlotRate[0] = twohand.attribute("Count1").as_int();
	SlotRate[1] = twohand.attribute("Count2").as_int();
	SlotRate[2] = twohand.attribute("Count3").as_int();
	SlotRate[3] = twohand.attribute("Count4").as_int();
	SlotRate[4] = twohand.attribute("Count5").as_int();

	for (int i = 0; i < 5; i++)
	{
		this->m_TwoHandWeaponMakeSocketSlotCountPool.AddValue(i + 1, SlotRate[i]);
	}
}

bool CItemObjectSocketOptionSystem::IsEnableSocketItem(CItemObject *lpItemData)
{
	if (lpItemData == NULL)	return false;

	if (lpItemData->IsItem() == FALSE)	return false;

	return IsEnableSocketItem(lpItemData->m_Type);
}

bool CItemObjectSocketOptionSystem::IsEnableSocketItem(int iItemCode)
{
	return GetItemGroup(iItemCode) == ITEMTYPE_SOCKET;
}

BYTE CItemObjectSocketOptionSystem::GetEmptySlotCount(CItemObject *lpItemData)
{
	BYTE btEmptySlotCount = 0;

	for (int i = 0; i < 5; i++)
	{
		if (lpItemData->m_SocketOption[i] == 0xFE)
		{
			btEmptySlotCount++;
		}
	}

	return btEmptySlotCount;
}

bool CItemObjectSocketOptionSystem::GetSeedSphereEffect(BYTE btSeedSphereIndex, BYTE& pbSeedType, BYTE& pbEffectValueType, WORD& pwEffectValue)
{
	BYTE SeedIndex = 0;
	BYTE SphereLevel = 0;

	CSeedItem* lpSeedData = NULL;
	CSphereItem *lpSphereData = NULL;

	CSeedSphereItem SeedSphere;

	SeedIndex = btSeedSphereIndex % 50;
	SphereLevel = (btSeedSphereIndex - SeedIndex) / 50 + 1;

	lpSeedData = GetSeedData(SeedIndex);
	lpSphereData = GetSphereData(SphereLevel);

	if (lpSeedData == NULL)	return false;

	SeedSphere.SetOption(lpSeedData, SphereLevel);

	pbSeedType = SeedSphere.m_SeedIndex;
	pbEffectValueType = SeedSphere.m_SeedOptionValueType;
	pwEffectValue = SeedSphere.m_SeedOptionValue;
	return true;
}

void CItemObjectSocketOptionSystem::GetSocketOption(CItemObject *lpItemData, BYTE *pbSocketOption, BYTE& pbSocketBonusOption)
{
	if (lpItemData == NULL)	return;

	if (lpItemData->IsItem() == FALSE ||
		IsEnableSocketItem(lpItemData) == false) return;

	for (int i = 0; i < 5; i++)
	{
		pbSocketOption[i] = lpItemData->m_SocketOption[i];
	}
	pbSocketBonusOption = lpItemData->m_BonusSocketOption;
}

bool CItemObjectSocketOptionSystem::IsSocketItem(CItemObject &Item)
{
	if (lpItemData == NULL)
	{
		return false;
	}

	if (lpItemData->IsItem() == FALSE)
	{
		return false;
	}

	return IsEnableSocketItem(lpItemData->m_Type);
}

bool CItemObjectSocketOptionSystem::IsSeedItem(int ItemCode)
{
	switch (ItemCode)
	{
	case ITEMGET(12, 60):
	case ITEMGET(12, 61):
	case ITEMGET(12, 62):
	case ITEMGET(12, 63):
	case ITEMGET(12, 64):
	case ITEMGET(12, 65):
		return true;
	}

	return false;
}

bool CItemObjectSocketOptionSystem::IsSphereItem(int ItemCode)
{
	switch (ItemCode)
	{
	case ITEMGET(12, 70):
	case ITEMGET(12, 71):
	case ITEMGET(12, 72):
	case ITEMGET(12, 73):
	case ITEMGET(12, 74):
		return true;
	}

	return false;
}

bool CItemObjectSocketOptionSystem::IsSeedSphereItem(int ItemCode)
{
	switch (ItemCode)
	{
	case ITEMGET(12, 100):
	case ITEMGET(12, 101):
	case ITEMGET(12, 102):
	case ITEMGET(12, 103):
	case ITEMGET(12, 104):
	case ITEMGET(12, 105):
	case ITEMGET(12, 106):
	case ITEMGET(12, 107):
	case ITEMGET(12, 108):
	case ITEMGET(12, 109):
	case ITEMGET(12, 110):
	case ITEMGET(12, 111):
	case ITEMGET(12, 112):
	case ITEMGET(12, 113):
	case ITEMGET(12, 114):
	case ITEMGET(12, 115):
	case ITEMGET(12, 116):
	case ITEMGET(12, 117):
	case ITEMGET(12, 118):
	case ITEMGET(12, 119):
	case ITEMGET(12, 120):
	case ITEMGET(12, 121):
	case ITEMGET(12, 122):
	case ITEMGET(12, 123):
	case ITEMGET(12, 124):
	case ITEMGET(12, 125):
	case ITEMGET(12, 126):
	case ITEMGET(12, 127):
	case ITEMGET(12, 128):
	case ITEMGET(12, 129):
		// new seed sphere
	case ITEMGET(12, 311):
	case ITEMGET(12, 312):
	case ITEMGET(12, 313):
	case ITEMGET(12, 314):
	case ITEMGET(12, 315):
	case ITEMGET(12, 316):
	case ITEMGET(12, 317):
	case ITEMGET(12, 318):
	case ITEMGET(12, 319):
	case ITEMGET(12, 320):
	case ITEMGET(12, 321):
	case ITEMGET(12, 322):
	case ITEMGET(12, 323):
	case ITEMGET(12, 324):
	case ITEMGET(12, 325):
	case ITEMGET(12, 326):
	case ITEMGET(12, 327):
	case ITEMGET(12, 328):
	case ITEMGET(12, 329):
	case ITEMGET(12, 330):
	case ITEMGET(12, 331):
	case ITEMGET(12, 332):
	case ITEMGET(12, 333):
	case ITEMGET(12, 334):
	case ITEMGET(12, 335):
	case ITEMGET(12, 336):
	case ITEMGET(12, 337):
	case ITEMGET(12, 338):
	case ITEMGET(12, 339):
	case ITEMGET(12, 340):
		return true;
	default:
		return false;
	}
}

BYTE CItemObjectSocketOptionSystem::GetSeedIndex(int ItemCode, BYTE ItemLevel)
{
	for (int i = 0; i < 50; i++)
	{
		if (m_SeedData[i].m_ItemCode == ItemCode &&
			m_SeedData[i].m_ItemLevel == ItemLevel)
		{
			return m_SeedData[i].m_SeedIndex;
		}
	}
	return 0xFF;
}

BYTE CItemObjectSocketOptionSystem::GetSphereLevel(int ItemCode)
{
	switch (ItemCode)
	{
	case ITEMGET(12, 70):	return 1;	break;
	case ITEMGET(12, 71):	return 2;	break;
	case ITEMGET(12, 72):	return 3;   break;
	case ITEMGET(12, 73):	return 4;	break;
	case ITEMGET(12, 74):	return 5;   break;
	}

	return 0;
}

bool CItemObjectSocketOptionSystem::CheckItemForSameSeedType(CItemObject *lpItemData, WORD SeedSphereItemCode, BYTE ItemLevel)
{
	BYTE btSeedIndex = 0;
	BYTE btSeedSphereIndex = 0;

	if (!IsEnableSocketItem(lpItemData))	return false;

	btSeedSphereIndex = GetSeedSphereIndexForItemCode(SeedSphereItemCode, ItemLevel);
	btSeedIndex = btSeedSphereIndex % 50;

	for (int i = 0; i < 5; i++)
	{
		if (lpItemData->m_SocketOption[i] == 0xFF ||
			lpItemData->m_SocketOption[i] == 0xFE)	continue;

		if ((lpItemData->m_SocketOption[i] % 50) == btSeedIndex)	return false;
	}

	return true;
}

BYTE CItemObjectSocketOptionSystem::GetSeedSphereIndexForItemCode(WORD ItemCode, BYTE ItemLevel)
{
	BYTE btSeedType = 0;
	BYTE btSphereLevel = 0;
	BYTE btSeedSphereIndex = 0;

	btSphereLevel = (ITEM_GET_INDEX(ItemCode) - 100) / 6;
	btSeedType = (ITEM_GET_INDEX(ItemCode) - 100) % 6 + 1;

	for (int i = 0; i < 50; i++)
	{
		if (m_SeedData[i].m_SeedType == btSeedType &&
			m_SeedData[i].m_ItemLevel == ItemLevel)
		{
			btSeedSphereIndex = btSphereLevel * 50 + m_SeedData[i].m_SeedIndex;
			return btSeedSphereIndex;
		}
	}

	return 0xFF;
}

DWORD CItemObjectSocketOptionSystem::GetSeedSphereValue(BYTE btSeedSphereIndex)
{
	BYTE btSeedIndex = 0;
	BYTE btSphereLevel = 0;

	CSeedItem *lpSeedData = NULL;
	CSphereItem* lpSphereData = NULL;

	CSeedSphereItem SeedSphere;

	DWORD dwSeedSphereValue = 0;

	btSeedIndex = btSeedSphereIndex % 50;
	btSphereLevel = (btSeedSphereIndex - btSeedIndex) / 50 + 1;

	lpSeedData = GetSeedData(btSeedIndex);

	if (lpSeedData == NULL)	return 0;

	SeedSphere.SetOption(lpSeedData, btSphereLevel);

	dwSeedSphereValue = ItemAttribute[SeedSphere.GetItemCode()].BuyMoney;
	return dwSeedSphereValue;
}

bool CItemObjectSocketOptionSystem::CheckSetSeedSphereItemType(CItemObject *lpItemData, BYTE btSeedSphereIndex)
{
	if (!IsEnableSocketItem(lpItemData))	return false;

	BYTE btItemType = 0;
	BYTE btItemIndex = 0;
	BYTE btSeedSphereType = 0;

	btItemIndex = ITEM_GET_INDEX(lpItemData->m_Type);
	btItemType = ITEM_GET_TYPE(lpItemData->m_Type);
	btSeedSphereType = GetSocketType(btSeedSphereIndex);

	if (btItemType >= 0 && btItemType <= 5)
	{
		switch (btSeedSphereType)
		{
		case SEED_TYPE_FIRE:
		case SEED_TYPE_ICE:
		case SEED_TYPE_LIGHTNING:
			return true;
		}
	}
	else if (btItemType >= 6 && btItemType <= 11)
	{
		switch (btSeedSphereType)
		{
		case SEED_TYPE_WATER:
		case SEED_TYPE_WIND:
		case SEED_TYPE_EARTH:
			return true;
		}
	}
	else
	{
		return false;
	}

	return false;
}

bool CItemObjectSocketOptionSystem::IsEnableDropSocketItemMap(int iMapIndex)
{
	switch (iMapIndex)
	{
	case MAP_INDEX_RAKLION:
	case MAP_INDEX_HATCHERY:
		return true;
	}

	return false;
}

int CItemObjectSocketOptionSystem::GetSphereDropInfo(int iMonsterLevel)
{
	int iItemCode = -1;
	int iSphereLevel = 0;

	if (m_SphereDropOn != TRUE)	return iItemCode;

	if (m_SphereDropLevel[4] != 0 && m_SphereDropLevel[4] <= iMonsterLevel)
	{
		iSphereLevel = 5;
	}
	else if (m_SphereDropLevel[3] != 0 && m_SphereDropLevel[3] <= iMonsterLevel)
	{
		iSphereLevel = 4;
	}
	else if (m_SphereDropLevel[2] != 0 && m_SphereDropLevel[2] <= iMonsterLevel)
	{
		iSphereLevel = 3;
	}
	else if (m_SphereDropLevel[1] != 0 && m_SphereDropLevel[1] <= iMonsterLevel)
	{
		iSphereLevel = 2;
	}
	else if (m_SphereDropLevel[0] != 0 && m_SphereDropLevel[0] <= iMonsterLevel)
	{
		iSphereLevel = 1;
	}
	else return iItemCode;

	for (int i = 0; i < iSphereLevel; i++)
	{
		if (m_SphereDropRate[i] >= GetLargeRand() % 10000)
		{
			iItemCode = i + ITEMGET(12, 70);
		}
	}

	return iItemCode;
}

int CItemObjectSocketOptionSystem::GetTetraDropInfo(int iMonsterLevel)
{
	int iItemCode = -1;
	int iTetraLevel = 0;

	if (m_TetraDropOn != TRUE)	return iItemCode;

	if (iMonsterLevel >= m_TetraDropMinLevel[2] && iMonsterLevel <= m_TetraDropMaxLevel[2])
	{
		iTetraLevel = 3;
	}

	else if (iMonsterLevel >= m_TetraDropMinLevel[1] && iMonsterLevel <= m_TetraDropMaxLevel[1])
	{
		iTetraLevel = 2;
	}

	else if (iMonsterLevel >= m_TetraDropMinLevel[0] && iMonsterLevel <= m_TetraDropMaxLevel[0])
	{
		iTetraLevel = 1;
	}

	else return iItemCode;

	for (int i = 0; i < iTetraLevel; i++)
	{
		if (m_TetraDropRate[i] >= GetLargeRand() % 10000)
		{
			iItemCode = ITEMGET(14, 247) - i;
		}
	}

	return iItemCode;
}

BYTE CItemObjectSocketOptionSystem::GetSeedIndexForSeedSpearItemCode(WORD ItemCode, BYTE ItemLevel)
{
	for (int i = 0; i < 50; i++)
	{
		if (this->m_SeedData[i].m_SeedType == ((ITEM_GET_INDEX(ItemCode) - 100) % 6 + 1) &&
			this->m_SeedData[i].m_ItemLevel == ItemLevel)
		{
			return this->m_SeedData[i].m_SeedIndex;
		}
	}

	return -1;
}

BYTE CItemObjectSocketOptionSystem::GetSeedSphereIndex(BYTE btSeedIndex, BYTE btSphereLevel)
{
	return btSphereLevel * 50 + btSeedIndex;
}

bool CItemObjectSocketOptionSystem::MakeSeedSphere(CSeedSphereItem *lpSeedSphere, BYTE btSeedIndex, BYTE btSphereLevel)
{
	CSeedItem* lpSeedData = NULL;
	CSphereItem* lpSphereData = NULL;

	lpSeedData = GetSeedData(btSeedIndex);

	if (lpSeedData == NULL)	return false;

	if (!lpSeedSphere->SetOption(lpSeedData, btSphereLevel))	return false;

	return true;
}

CSeedItem * CItemObjectSocketOptionSystem::GetSeedData(BYTE btSeedIndex)
{
	for (int i = 0; i < 50; i++)
	{
		if (m_SeedData[i].m_SeedIndex == btSeedIndex)
		{
			return &m_SeedData[i];
		}
	}

	return NULL;
}

CSphereItem * CItemObjectSocketOptionSystem::GetSphereData(BYTE btSphereLevel)
{
	for (int i = 0; i < 5; i++)
	{
		if (m_SphereData[i].m_SphereLevel == btSphereLevel)
		{
			return &m_SphereData[i];
		}
	}

	return NULL;
}

BYTE CItemObjectSocketOptionSystem::GetExtractSeed()
{
	int iSeedDatObj.m_Index = 0;

	iSeedDatObj.m_Index = m_SeedExtractPool.GetRandomValue(eRANDOMPOOL_BY_WEIGHT);

	return iSeedDatObj.m_Index;
}

BYTE CItemObjectSocketOptionSystem::GetSocketType(BYTE btSeedSphereIndex)
{
	BYTE btSeedIndex = 0;
	CSeedItem* lpSeedData = NULL;

	btSeedIndex = btSeedSphereIndex % 50;

	lpSeedData = GetSeedData(btSeedIndex);

	if (lpSeedData == NULL)	return 0;
	return lpSeedData->m_SeedType;
}

int CItemObjectSocketOptionSystem::GetSeedSphereLevel(BYTE btSeedSphereIndex)
{
	BYTE SeedIndex = btSeedSphereIndex % 50;
	BYTE SphereLevel = (btSeedSphereIndex - SeedIndex) / 50;

	return SphereLevel;
}

BONUS_SOCKET_OPTION * CItemObjectSocketOptionSystem::GetBonusSocketOption(BYTE btBonusSocketOptionIndex)
{
	for (int i = 0; i < 12; i++)
	{
		if (m_BonusSocketOptionData[i].OptionIndex == btBonusSocketOptionIndex)
		{
			return &m_BonusSocketOptionData[i];
		}
	}

	return NULL;
}

int CItemObjectSocketOptionSystem::ApplySeedSphereEffect(CGameObject &Obj)
{
	int iTotalSeedSphereCount = 0;
	BYTE SeedSphereIndex = 0;
	BYTE SeedIndex = 0;
	BYTE SphereLevel = 0;

	CSeedSphereItem SeedSphere;

	for (int iEquip = 0; iEquip < INVETORY_WEAR_SIZE; iEquip++)
	{
		if (!IsEnableSocketItem(&Obj.pInventory[iEquip]))	continue;

		for (int i = 0; i < 5; i++)
		{
			SeedSphereIndex = Obj.pInventory[iEquip]->m_SocketOption[i];

			if (SeedSphereIndex == 0xFF ||
				SeedSphereIndex == 0xFE)
			{
				continue;
			}

			SeedIndex = SeedSphereIndex % 50;
			SphereLevel = (SeedSphereIndex - SeedIndex) / 50 + 1;

			if (!MakeSeedSphere(&SeedSphere, SeedIndex, SphereLevel))	continue;

			SetEffect(lpObj, SOCKET_OPTION_TYPE_SEED, SeedIndex, SeedSphere.m_SeedOptionValueType, SeedSphere.m_SeedOptionValue);
			iTotalSeedSphereCount++;
		}

	}
	return iTotalSeedSphereCount;
}

void CItemObjectSocketOptionSystem::ApplyBonusOptionEffect(CGameObject &Obj)
{
	BYTE btBonusSocketIndex = 0;
	LPBONUS_SOCKET_OPTION lpBonusOptionData = NULL;

	for (int iEquip = 0; iEquip < INVETORY_WEAR_SIZE; iEquip++)
	{
		if (!IsEnableSocketItem(&Obj.pInventory[iEquip]))	continue;

		btBonusSocketIndex = Obj.pInventory[iEquip]->m_BonusSocketOption;

		if (btBonusSocketIndex == 0xFF ||
			btBonusSocketIndex == 0xFE)
		{
			continue;
		}

		lpBonusOptionData = GetBonusSocketOption(btBonusSocketIndex);
		if (lpBonusOptionData == NULL)	continue;

		//							EffectType : %d, OptionType : %d, OptionValue : %d
		SetEffect(lpObj, SOCKET_OPTION_TYPE_BONUS, lpBonusOptionData->EffectType, lpBonusOptionData->OptionValueType, lpBonusOptionData->OptionValue);
	}
}

void CItemObjectSocketOptionSystem::ApplySetOptionEffect(CGameObject &Obj)
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	BYTE btTotalSeedTypeList[SEED_TYPE_MAX];
	BYTE btSeedType = SEED_TYPE_NONE;
	BYTE btMatchedCount = 0;

	memset(&btTotalSeedTypeList, 0x00, sizeof(btTotalSeedTypeList));

	for (int iUserSocketIndex = 0; iUserSocketIndex < 35; iUserSocketIndex++)
	{
		btSeedType = GetSocketType(Obj.m_PlayerData->m_SeedOptionList[iUserSocketIndex].m_SocketOptionIndex);
		if (btSeedType == SEED_TYPE_NONE)	break;

		btTotalSeedTypeList[btSeedType - 1] += Obj.m_PlayerData->m_SeedOptionList[iUserSocketIndex].m_SocketOptionCount;
	}

	for (int iSetOptionIndex = 0; iSetOptionIndex < 2; iSetOptionIndex++)
	{
		for (int i = 0; i < 6; i++)
		{
			if (m_SetSocketOptionData[iSetOptionIndex].SeedTypeTable[i] <= btTotalSeedTypeList[i])
			{
				btMatchedCount++;
			}
		}

		if (btMatchedCount == 6)
		{
			SetEffect(lpObj, SOCKET_OPTION_TYPE_SET, m_SetSocketOptionData[iSetOptionIndex].EffectType, m_SetSocketOptionData[iSetOptionIndex].OptionValueType, m_SetSocketOptionData[iSetOptionIndex].OptionValue);
		}
		btMatchedCount = 0;
	}
}

bool CItemObjectSocketOptionSystem::SetApplySocketEffect(CGameObject &Obj)
{
	int iTotalSeedSphereCount = 0;

	if (lpObj == NULL)	return false;

	if (Obj.Type != OBJ_USER ||
		Obj.Connected < PLAYER_PLAYING)	return false;

	SetClearSocketEffect(lpObj);

	iTotalSeedSphereCount = ApplySeedSphereEffect(lpObj);
	if (iTotalSeedSphereCount <= 0)	return false;

	ApplyBonusOptionEffect(lpObj);
	ApplySetOptionEffect(lpObj);
	return true;
}

void CItemObjectSocketOptionSystem::SetClearSocketEffect(CGameObject &Obj)
{
	if (lpObj == NULL)	return;

	if (Obj.Type != OBJ_USER ||
		Obj.Connected < PLAYER_PLAYING)	return;

	int iIndex = 0;

	for (iIndex = 0; iIndex < 35; iIndex++)
	{
		if (Obj.m_PlayerData->m_SeedOptionList[iIndex].m_SocketOptionIndex == 0xFF)	break;
		Obj.m_PlayerData->m_SeedOptionList[iIndex].Clear();
	}

	for (iIndex = 0; iIndex < 7; iIndex++)
	{
		if (Obj.m_PlayerData->m_BonusOptionList[iIndex].m_SocketOptionIndex == 0xFF)	break;
		Obj.m_PlayerData->m_BonusOptionList[iIndex].Clear();
	}

	for (iIndex = 0; iIndex < 2; iIndex++)
	{
		if (Obj.m_PlayerData->m_SetOptionList[iIndex].m_SocketOptionIndex == 0xFF)	break;
		Obj.m_PlayerData->m_SetOptionList[iIndex].Clear();
	}

	Obj.m_PlayerData->m_btRefillHPSocketOption = 0;
	Obj.m_PlayerData->m_btRefillMPSocketOption = 0;
	Obj.m_PlayerData->m_wSocketOptionMonsterDieGetHp = 0;
	Obj.m_PlayerData->m_wSocketOptionMonsterDieGetMana = 0;
	Obj.m_PlayerData->m_btAGReduceRate = 0;
}

void CItemObjectSocketOptionSystem::SetEffect(CGameObject &Obj, BYTE btOptionType, BYTE btEffectType, BYTE btEffectValueType, WORD wEffectValue)
{
	if (btEffectType >= 0 && btEffectType <= 9)
	{
		_SetFireEffect(lpObj, btEffectType, btEffectValueType, wEffectValue);
	}
	else if (btEffectType >= 10 && btEffectType <= 15)
	{
		_SetWaterEffect(lpObj, btEffectType, btEffectValueType, wEffectValue);
	}
	else if (btEffectType >= 16 && btEffectType <= 20)
	{
		_SetIceEffect(lpObj, btEffectType, btEffectValueType, wEffectValue);
	}
	else if (btEffectType >= 21 && btEffectType <= 28)
	{
		_SetWindEffect(lpObj, btEffectType, btEffectValueType, wEffectValue);
	}
	else if (btEffectType >= 29 && btEffectType <= 33)
	{
		_SetThunderEffect(lpObj, btEffectType, btEffectValueType, wEffectValue);
	}
	else if (btEffectType >= 34 && btEffectType <= 40)
	{
		_SetEarthEffect(lpObj, btEffectType, btEffectValueType, wEffectValue);
	}
	else if (btEffectType >= 41 && btEffectType <= 43)
	{
		_SetUniqueEffect(lpObj, btEffectType, btEffectValueType, wEffectValue);
	}
	else if (btEffectType >= 44 && btEffectType <= 45)
	{
		_SetSetEffect(lpObj, btEffectType, btEffectValueType, wEffectValue);
	}

	for (int i = 0; i < 43; i++) //{Nice WebZen =D}
	{
		if (Obj.m_PlayerData->m_SeedOptionList[i].m_SocketOptionIndex == btEffectType &&
			Obj.m_PlayerData->m_SeedOptionList[i].m_SocketOptionValueType == btEffectValueType)
		{
			Obj.m_PlayerData->m_SeedOptionList[i].m_SocketOptionValue += wEffectValue;
			Obj.m_PlayerData->m_SeedOptionList[i].m_SocketOptionCount++;
			break;
		}

		if (Obj.m_PlayerData->m_SeedOptionList[i].m_SocketOptionIndex == 0xFF)
		{
			Obj.m_PlayerData->m_SeedOptionList[i].m_SocketOptionIndex = btEffectType;
			Obj.m_PlayerData->m_SeedOptionList[i].m_SocketOptionValueType = btEffectValueType;
			Obj.m_PlayerData->m_SeedOptionList[i].m_SocketOptionValue = wEffectValue;
			Obj.m_PlayerData->m_SeedOptionList[i].m_SocketOptionCount = 1;
			break;
		}
	}
}

void CItemObjectSocketOptionSystem::ClearEffect(CGameObject &Obj, LPSOCKET_OPTION_LIST lpOptionData)
{
	//empty, possible due to "return;" in the beggining
}

void CItemObjectSocketOptionSystem::_SetFireEffect(CGameObject &Obj, BYTE btEffectType, BYTE btEffectValueType, WORD wEffectValue)
{
	int iCalcEffectValue = 0;

	switch (btEffectType)
	{
	case SEED_EFFECT_TYPE_ATTACK_POWER:
	{
		iCalcEffectValue = CalcEffectValue(Obj.m_AttackDamageMaxLeft, wEffectValue, btEffectValueType);
		Obj.m_AttackDamageMaxLeft += iCalcEffectValue;

		iCalcEffectValue = CalcEffectValue(Obj.m_AttackDamageMinLeft, wEffectValue, btEffectValueType);
		Obj.m_AttackDamageMinLeft += iCalcEffectValue;

		iCalcEffectValue = CalcEffectValue(Obj.m_AttackDamageMaxRight, wEffectValue, btEffectValueType);
		Obj.m_AttackDamageMaxRight += iCalcEffectValue;

		iCalcEffectValue = CalcEffectValue(Obj.m_AttackDamageMinRight, wEffectValue, btEffectValueType);
		Obj.m_AttackDamageMinRight += iCalcEffectValue;

		iCalcEffectValue = CalcEffectValue(Obj.m_MagicDamageMin, wEffectValue, btEffectValueType);
		Obj.m_MagicDamageMin += iCalcEffectValue;

		iCalcEffectValue = CalcEffectValue(Obj.m_MagicDamageMax, wEffectValue, btEffectValueType);
		Obj.m_MagicDamageMax += iCalcEffectValue;
	}
	break;
	case SEED_EFFECT_TYPE_ATTACK_SPEED:
	{
		iCalcEffectValue = CalcEffectValue(Obj.m_AttackSpeed, wEffectValue, btEffectValueType);
		Obj.m_AttackSpeed += iCalcEffectValue;

		iCalcEffectValue = CalcEffectValue(Obj.m_MagicSpeed, wEffectValue, btEffectValueType);
		Obj.m_MagicSpeed += iCalcEffectValue;
	}
	break;
	case SEED_EFFECT_TYPE_MAX_ATTACK_POWER:
	{
		iCalcEffectValue = CalcEffectValue(Obj.m_AttackDamageMaxLeft, wEffectValue, btEffectValueType);
		Obj.m_AttackDamageMaxLeft += iCalcEffectValue;

		iCalcEffectValue = CalcEffectValue(Obj.m_AttackDamageMaxRight, wEffectValue, btEffectValueType);
		Obj.m_AttackDamageMaxRight += iCalcEffectValue;

		iCalcEffectValue = CalcEffectValue(Obj.m_MagicDamageMax, wEffectValue, btEffectValueType);
		Obj.m_MagicDamageMax += iCalcEffectValue;
	}
	break;
	case SEED_EFFECT_TYPE_MIN_ATTACK_POWER:
	{
		iCalcEffectValue = CalcEffectValue(Obj.m_AttackDamageMinLeft, wEffectValue, btEffectValueType);
		Obj.m_AttackDamageMinLeft += iCalcEffectValue;

		iCalcEffectValue = CalcEffectValue(Obj.m_AttackDamageMinRight, wEffectValue, btEffectValueType);
		Obj.m_AttackDamageMinRight += iCalcEffectValue;

		iCalcEffectValue = CalcEffectValue(Obj.m_MagicDamageMin, wEffectValue, btEffectValueType);
		Obj.m_MagicDamageMin += iCalcEffectValue;
	}
	break;
	case SEED_EFFECT_TYPE_MIN_MAX_ATTACK_POWER:
	{
		iCalcEffectValue = CalcEffectValue(Obj.m_AttackDamageMaxLeft, wEffectValue, btEffectValueType);
		Obj.m_AttackDamageMaxLeft += iCalcEffectValue;

		iCalcEffectValue = CalcEffectValue(Obj.m_AttackDamageMinLeft, wEffectValue, btEffectValueType);
		Obj.m_AttackDamageMinLeft += iCalcEffectValue;

		iCalcEffectValue = CalcEffectValue(Obj.m_AttackDamageMaxRight, wEffectValue, btEffectValueType);
		Obj.m_AttackDamageMaxRight += iCalcEffectValue;

		iCalcEffectValue = CalcEffectValue(Obj.m_AttackDamageMinRight, wEffectValue, btEffectValueType);
		Obj.m_AttackDamageMinRight += iCalcEffectValue;

		iCalcEffectValue = CalcEffectValue(Obj.m_MagicDamageMin, wEffectValue, btEffectValueType);
		Obj.m_MagicDamageMin += iCalcEffectValue;

		iCalcEffectValue = CalcEffectValue(Obj.m_MagicDamageMax, wEffectValue, btEffectValueType);
		Obj.m_MagicDamageMax += iCalcEffectValue;
	}
	break;
	case SEED_EFFECT_TYPE_REDUCE_AG_RATE:
	{
		Obj.m_PlayerData->m_btAGReduceRate += wEffectValue;

		if (Obj.m_PlayerData->m_btAGReduceRate > 100)
		{
			Obj.m_PlayerData->m_btAGReduceRate = 100;
		}
	}
	break;
	}
}

void CItemObjectSocketOptionSystem::_SetWaterEffect(CGameObject &Obj, BYTE btEffectType, BYTE btEffectValueType, WORD wEffectValue)
{
	int iCalcEffectValue = 0;

	switch (btEffectType)
	{
	case SEED_EFFECT_TYPE_SUCCESS_BLOCK:
	{
		iCalcEffectValue = CalcEffectValue(Obj.m_SuccessfulBlocking, wEffectValue, btEffectValueType);
		Obj.m_SuccessfulBlocking += iCalcEffectValue;
	}
	break;
	case SEED_EFFECT_TYPE_BLOCK_POWER:
	{
		iCalcEffectValue = CalcEffectValue(Obj.m_Defense, wEffectValue, btEffectValueType);
		Obj.m_Defense += iCalcEffectValue;

		iCalcEffectValue = CalcEffectValue(Obj.m_MagicDefense, wEffectValue, btEffectValueType);
		Obj.m_MagicDefense += iCalcEffectValue;
	}
	break;
	case SEED_EFFECT_TYPE_SHIELD_BLOCK_POWER:
	{
		Obj.m_PlayerData->SetOpImproveSheldDefence += wEffectValue;
	}
	break;
	case SEED_EFFECT_TYPE_REDUCE_DAMAGE:
	{
		Obj.DamageDecrease += wEffectValue;
	}
	break;
	case SEED_EFFECT_TYPE_REFLECT_DAMAGE:
	{
		Obj.DamageReflect += wEffectValue;
	}
	break;
	}
}

void CItemObjectSocketOptionSystem::_SetIceEffect(CGameObject &Obj, BYTE btEffectType, BYTE btEffectValueType, WORD wEffectValue)
{
	int iAddValue = 0;

	switch (btEffectType)
	{
	case SEED_EFFECT_TYPE_MONSTER_KILL_REFILL_HP:
	{
		iAddValue += CalcEffectValue(Obj.MaxLife, wEffectValue, btEffectValueType);
		Obj.m_PlayerData->m_wSocketOptionMonsterDieGetHp += iAddValue;
	}
	break;
	case SEED_EFFECT_TYPE_MONSTER_KILL_REFILL_MP:
	{
		iAddValue += CalcEffectValue(Obj.MaxMana, wEffectValue, btEffectValueType);
		Obj.m_PlayerData->m_wSocketOptionMonsterDieGetMana += iAddValue;
	}
	break;
	case SEED_EFFECT_TYPE_ATTACK_SKILL_POWER:
	{
		iAddValue = CalcEffectValue(Obj.m_PlayerData->SetOpAddSkillAttack, wEffectValue, btEffectValueType);
		Obj.m_PlayerData->SetOpAddSkillAttack += iAddValue;
	}
	break;
	case SEED_EFFECT_TYPE_ATTACK_ACCURACY:
	{
		Obj.m_AttackRating += wEffectValue;
	}
	break;

	}
}

void CItemObjectSocketOptionSystem::_SetWindEffect(CGameObject &Obj, BYTE btEffectType, BYTE btEffectValueType, WORD wEffectValue)
{
	int iAddValue = 0;

	switch (btEffectType)
	{
	case SEED_EFFECT_TYPE_REFILL_HP_INC:
	{
		iAddValue = CalcEffectValue(Obj.MaxLife, wEffectValue, btEffectValueType);
		Obj.m_PlayerData->m_btRefillHPSocketOption += iAddValue;
	}
	break;
	case SEED_EFFECT_TYPE_MAX_HP_INC:
	{
		iAddValue = CalcEffectValue(Obj.MaxLife, wEffectValue, btEffectValueType);
		Obj.AddLife += iAddValue;
		gGameProtocol.GCReFillSend(Obj.m_Index, (Obj.AddLife + Obj.MaxLife), 0xFE, 0, (Obj.iMaxShield + Obj.iAddShield));
		gGameProtocol.GCReFillSend(Obj.m_Index, Obj.Life, 0xFF, 0, Obj.iShield);
	}
	break;
	case SEED_EFFECT_TYPE_MAX_MP_INC:
	{
		iAddValue = CalcEffectValue(Obj.MaxMana, wEffectValue, btEffectValueType);
		Obj.AddMana += iAddValue;
		gGameProtocol.GCManaSend(Obj.m_Index, (Obj.AddMana + Obj.MaxMana), 0xFE, 0, (Obj.MaxBP + Obj.AddBP));
		gGameProtocol.GCManaSend(Obj.m_Index, Obj.Mana, 0xFF, 0, Obj.BP);
	}
	break;
	case SEED_EFFECT_TYPE_REFILL_MP_INC:
	{
		iAddValue = CalcEffectValue(Obj.MaxMana, wEffectValue, btEffectValueType);
		Obj.m_PlayerData->m_btRefillMPSocketOption += iAddValue;
	}
	break;
	case SEED_EFFECT_TYPE_MAX_AG_INC:
	{
		iAddValue = CalcEffectValue(Obj.MaxBP, wEffectValue, btEffectValueType);
		Obj.AddBP += iAddValue;
		gGameProtocol.GCManaSend(Obj.m_Index, (Obj.AddMana + Obj.MaxMana), 0xFE, 0, (Obj.MaxBP + Obj.AddBP));
		gGameProtocol.GCManaSend(Obj.m_Index, Obj.Mana, 0xFF, 0, Obj.BP);
	}
	break;
	case SEED_EFFECT_TYPE_REFILL_AG_INC:
	{
		iAddValue = CalcEffectValue(Obj.m_PlayerData->SetOpIncAGValue, wEffectValue, btEffectValueType);
		Obj.m_PlayerData->SetOpIncAGValue += iAddValue;
	}
	break;
	case SEED_EFFECT_TYPE_MONSTER_KILL_ZEN_INC:
	{
		Obj.MonsterDieGetMoney += wEffectValue;
	}
	break;
	}
}

void CItemObjectSocketOptionSystem::_SetThunderEffect(CGameObject &Obj, BYTE btEffectType, BYTE btEffectValueType, WORD wEffectValue)
{
	int iAddValue = 0;

	switch (btEffectType)
	{
	case SEED_EFFECT_TYPE_EXCELLENT_DAMAGE_INC:
	{
		iAddValue = CalcEffectValue(Obj.m_PlayerData->SetOpAddExDamage, wEffectValue, btEffectValueType);
		Obj.m_PlayerData->SetOpAddExDamage += iAddValue;
	}
	break;
	case SEED_EFFECT_TYPE_EXCELLENT_DAMAGE_RATE_INC:
	{
		Obj.m_ExcelentDamage += wEffectValue;
	}
	break;
	case SEED_EFFECT_TYPE_CRITICAL_DAMAGE_INC:
	{
		iAddValue = CalcEffectValue(Obj.m_PlayerData->SetOpAddCriticalDamage, wEffectValue, btEffectValueType);
		Obj.m_PlayerData->SetOpAddCriticalDamage += iAddValue;
	}
	break;
	case SEED_EFFECT_TYPE_CRITICAL_DAMAGE_RATE_INC:
	{
		Obj.m_CriticalDamage += wEffectValue;
	}
	break;
	}
}

void CItemObjectSocketOptionSystem::_SetEarthEffect(CGameObject &Obj, BYTE btEffectType, BYTE btEffectValueType, WORD wEffectValue)
{
	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	int iAddValue = 0;

	switch (btEffectType)
	{
	case SEED_EFFECT_TYPE_STRENGTH_INC:
	{
		iAddValue = CalcEffectValue(Obj.m_PlayerData->Strength + Obj.AddStrength, wEffectValue, btEffectValueType);
		Obj.AddStrength += iAddValue;
	}
	break;
	case SEED_EFFECT_TYPE_DEXTERITY_INC:
	{
		iAddValue = CalcEffectValue(Obj.m_PlayerData->Dexterity + Obj.AddDexterity, wEffectValue, btEffectValueType);

		EnterCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);
		Obj.m_PlayerData->AgilityCheckDelay = GetTickCount();
		Obj.AddDexterity += iAddValue;
		LeaveCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);
	}
	break;
	case SEED_EFFECT_TYPE_VITALITY_INC:
	{
		iAddValue = CalcEffectValue(Obj.m_PlayerData->Vitality + Obj.AddVitality, wEffectValue, btEffectValueType);
		Obj.AddVitality += iAddValue;
	}
	break;
	case SEED_EFFECT_TYPE_ENERGY_INC:
	{
		iAddValue = CalcEffectValue(Obj.m_PlayerData->Energy + Obj.AddEnergy, wEffectValue, btEffectValueType);
		Obj.AddEnergy += iAddValue;
	}
	break;
	}
}

void CItemObjectSocketOptionSystem::_SetUniqueEffect(CGameObject &Obj, BYTE btEffectType, BYTE btEffectValueType, WORD wEffectValue)
{
	//int iAddValue = btEffectType;

	switch (btEffectType)
	{
	case SEED_EFFECT_TYPE_UNIQUE_1:
	{
	}
	break;
	case SEED_EFFECT_TYPE_UNIQUE_2:
	{
	}
	break;
	}
}

void CItemObjectSocketOptionSystem::_SetSetEffect(CGameObject &Obj, BYTE btEffectType, BYTE btEffectValueType, WORD wEffectValue)
{
	int iCalcEffectValue = 0;

	switch (btEffectType)
	{
	case SEED_EFFECT_TYPE_SET_DOUBLE_DAMAGE_RATE_INC:
	{
		Obj.m_PlayerData->SetOpDoubleDamage += wEffectValue;
	}
	break;
	case SEED_EFFECT_TYPE_SET_IGNORE_ENERMY_DEFENSE:
	{
		Obj.m_PlayerData->SetOpIgnoreDefense += wEffectValue;
	}
	break;
	}
}

int CItemObjectSocketOptionSystem::CalcEffectValue(int iInputValue, WORD wEffectValue, BYTE btEffectValueType)
{
	int iReturnValue = 0;

	if (wEffectValue == 0)	return 0;

	switch (btEffectValueType)
	{
	case SEED_OPTION_TYPE_VALUE:		iReturnValue = wEffectValue;						break;
	case SEED_OPTION_TYPE_RATE:			iReturnValue = iInputValue * wEffectValue / 100;	break;
	case SEED_OPTION_TYPE_PER_LEVEL:	iReturnValue = iInputValue / wEffectValue;			break;
	case SEED_OPTION_TYPE_PER_HP:		iReturnValue = iInputValue / wEffectValue;			break;
	case SEED_OPTION_TYPE_PER_MP:		iReturnValue = iInputValue / wEffectValue;			break;
	default:	return -1;	break;
	}

	return iReturnValue;
}

void CItemObjectSocketOptionSystem::SetRequireStatOption(CItemObject *lpItemdata)
{
	if (lpItemdata == NULL)	return;

	if (lpItemdata->IsItem() == FALSE ||
		!IsSocketItem(lpItemdata))	return;

	BYTE SeedIndex = 0;
	BYTE SphereLevel = 0;
	BYTE SeedSphereIndex = 0;

	CSeedSphereItem SeedSphere;

	int iReduceValue = 0;

	for (int i = 0; i < 5; i++)
	{
		if (lpItemdata->m_SocketOption[i] == 0xFF ||
			lpItemdata->m_SocketOption[i] == 0xFE)
		{
			continue;
		}

		SeedSphereIndex = lpItemdata->m_SocketOption[i];
		SeedIndex = SeedSphereIndex % 50;
		SphereLevel = (SeedSphereIndex - SeedIndex) / 50 + 1;

		if (!MakeSeedSphere(&SeedSphere, SeedIndex, SphereLevel))	continue;

		if (SeedIndex == SEED_EFFECT_TYPE_REQUIRE_STRENGTH_DEC)
		{
			iReduceValue = CalcEffectValue(lpItemdata->m_HJOpStrength, SeedSphere.m_SeedOptionValue, SeedSphere.m_SeedOptionValueType);
			lpItemdata->m_HJOpStrength += iReduceValue;
		}
		else if (SeedIndex == SEED_EFFECT_TYPE_REQUIRE_DEXTERITY_DEC)
		{
			iReduceValue = CalcEffectValue(lpItemdata->m_HJOpDexterity, SeedSphere.m_SeedOptionValue, SeedSphere.m_SeedOptionValueType);
			lpItemdata->m_HJOpDexterity += iReduceValue;
		}
		else if (SeedIndex == SEED_EFFECT_TYPE_ITEM_DURABILITY_IMPROVE)
		{
			lpItemdata->m_ImproveDurabilityRate += SeedSphere.m_SeedOptionValue;
		}
	}
}

void CItemObjectSocketOptionSystem::MakeSocketSlot(CItemObject *lpItemData, BYTE btSocketSlotCount)
{
	lpItemData->MakeSocketSlot(btSocketSlotCount);
}

BYTE CItemObjectSocketOptionSystem::GetMakeSocketSlotCount(int iItemCode)
{
	int iSocketSlotCount = 0;

	if (ItemAttribute[iItemCode].HaveItemInfo == 1)
	{
		if (ItemAttribute[iItemCode].TwoHand == TRUE)
		{
			iSocketSlotCount = this->m_TwoHandWeaponMakeSocketSlotCountPool.GetRandomValue(eRANDOMPOOL_BY_WEIGHT);
		}

		else
		{
			int ItemType = ITEM_GET_TYPE(iItemCode);

			std::map<int, TRandomPoolMgr>::iterator It = this->m_mapSocketSlotCountPool.find(ItemType);

			if (It == this->m_mapSocketSlotCountPool.end())
			{
				return iSocketSlotCount;
			}

			iSocketSlotCount = It->second.GetRandomValue(eRANDOMPOOL_BY_WEIGHT);
		}
	}

	return iSocketSlotCount;
}

void CItemObjectSocketOptionSystem::ClearUserData(CGameObject &Obj)
{
	int iIndex = 0;

	if (lpObj == NULL)	return;
	if (Obj.Type != OBJ_USER)	return;

	for (iIndex = 0; iIndex < 35; iIndex++)	Obj.m_PlayerData->m_SeedOptionList[iIndex].Clear();
	for (iIndex = 0; iIndex < 7; iIndex++)	Obj.m_PlayerData->m_BonusOptionList[iIndex].Clear();
	for (iIndex = 0; iIndex < 2; iIndex++)	Obj.m_PlayerData->m_SetOptionList[iIndex].Clear();
}

bool CItemObjectSocketOptionSystem::SetSocketSlot(CItemObject *lpTargetItem, CItemObject *lpSeedSphere, BYTE btSocketSlotIndex)
{
	if (lpTargetItem == NULL || lpSeedSphere == NULL)	return false;
	if (lpTargetItem->IsItem() == FALSE || lpSeedSphere->IsItem() == FALSE)	return false;
	if (!IsEnableSocketItem(lpTargetItem))	return false;
	if (btSocketSlotIndex > 5)	return false;
	if (lpTargetItem->m_SocketOption[btSocketSlotIndex] != 0xFE)	return false;

	BYTE btItemIndex = 0;
	BYTE btSeedIndex = 0;
	BYTE btSphereLevel = 0;
	BYTE btSeedSphereIndex = 0;

	btItemIndex = ITEM_GET_INDEX(lpSeedSphere->m_Type);
	btSeedIndex = btItemIndex - 99;
	btSphereLevel = (btSeedIndex - 1) / 6 + 1;
	btSeedSphereIndex = btSphereLevel * 50 + btSeedIndex;

	lpTargetItem->m_SocketOption[btSocketSlotIndex] = btSeedSphereIndex;
	SetBonusSocketOption(lpTargetItem);
	return true;
}

bool CItemObjectSocketOptionSystem::SetSocketSlot(CItemObject *lpTargetItem, BYTE btSeedIndex, BYTE btSphereLevel, BYTE btSocketSlotIndex)
{
	if (lpTargetItem == NULL)	return false;
	if (lpTargetItem->IsItem() == FALSE)	return false;
	if (IsEnableSocketItem(lpTargetItem) == false)	return false;
	if (btSocketSlotIndex > 5)	return false;
	if (lpTargetItem->m_SocketOption[btSocketSlotIndex] != 0xFE)	return false;

	BYTE btSeedSphereIndex = 0;
	btSeedSphereIndex = 50 * btSphereLevel + btSeedIndex;
	lpTargetItem->m_SocketOption[btSocketSlotIndex] = btSeedSphereIndex;
	SetBonusSocketOption(lpTargetItem);
	return true;
}

bool CItemObjectSocketOptionSystem::SetSocketSlot(CItemObject *lpTargetItem, BYTE btSeedSphereIndex, BYTE btSocketSlotIndex)
{
	if (lpTargetItem == NULL)	return false;
	if (lpTargetItem->IsItem() == FALSE)	return false;
	if (IsEnableSocketItem(lpTargetItem) == false)	return false;
	if (btSocketSlotIndex > 5)	return false;
	if (lpTargetItem->m_SocketOption[btSocketSlotIndex] != 0xFE)	return false;

	lpTargetItem->m_SocketOption[btSocketSlotIndex] = btSeedSphereIndex;
	SetBonusSocketOption(lpTargetItem);
	return true;
}

bool CItemObjectSocketOptionSystem::SetBonusSocketOption(CItemObject *lpItemData)
{
	BYTE SeedSphereType[5];

	if (lpItemData == NULL)	return false;
	if (lpItemData->IsItem() == FALSE || IsEnableSocketItem(lpItemData) == false)	return false;

	int nSeedSphere4thCount = 0;

	for (int iSlotIndex = 0; iSlotIndex < 5; iSlotIndex++)
	{
		if (lpItemData->m_SocketOption[iSlotIndex] != 0xFF &&
			lpItemData->m_SocketOption[iSlotIndex] != 0xFE)
		{
			SeedSphereType[iSlotIndex] = GetSocketType(lpItemData->m_SocketOption[iSlotIndex]);

			if (this->GetSeedSphereLevel(lpItemData->m_SocketOption[iSlotIndex]) == 3 && iSlotIndex < 3)
			{
				nSeedSphere4thCount++;
			}
		}
	}

	BYTE btItemType = 0;
	WORD wItemIndex = 0;
	BYTE btSocketSlotCount = 0;

	wItemIndex = ITEM_GET_INDEX(lpItemData->m_Type);
	btItemType = ITEM_GET_TYPE(lpItemData->m_Type);

	for (int iBonusIndex = 0; iBonusIndex < 6; iBonusIndex++)
	{
		if (btItemType >= m_BonusSocketOptionData[iBonusIndex].ItemTypeBegin &&
			btItemType <= m_BonusSocketOptionData[iBonusIndex].ItemTypeEnd)
		{
			for (int iSlotIndex = 0; iSlotIndex < 5; iSlotIndex++)
			{
				if (m_BonusSocketOptionData[iBonusIndex].SocketSlotType[iSlotIndex] == SeedSphereType[iSlotIndex] && SeedSphereType[iSlotIndex] != NULL)
				{
					btSocketSlotCount++;
				}

				if (btSocketSlotCount >= m_BonusSocketOptionData[iBonusIndex].SocketSlotCount)
				{
					if (rand() % 100 < m_BonusSocketOptionData[iBonusIndex].ExtractRate)
					{
						lpItemData->m_BonusSocketOption = m_BonusSocketOptionData[iBonusIndex].OptionIndex;
						return true;
					}

					lpItemData->m_BonusSocketOption = 0xFF;

					if (nSeedSphere4thCount != 3)
					{
						return false;
					}
				}
			}

			btSocketSlotCount = 0;
		}
	}

	if (nSeedSphere4thCount == 3)
	{
		for (int iBonusIndex = 6; iBonusIndex < 12; iBonusIndex++)
		{
			if (btItemType >= m_BonusSocketOptionData[iBonusIndex].ItemTypeBegin &&
				btItemType <= m_BonusSocketOptionData[iBonusIndex].ItemTypeEnd)
			{
				for (int iSlotIndex = 0; iSlotIndex < 5; iSlotIndex++)
				{
					if (m_BonusSocketOptionData[iBonusIndex].SocketSlotType[iSlotIndex] == SeedSphereType[iSlotIndex] && SeedSphereType[iSlotIndex] != NULL)
					{
						btSocketSlotCount++;
					}

					if (btSocketSlotCount >= m_BonusSocketOptionData[iBonusIndex].SocketSlotCount)
					{
						if (rand() % 100 < m_BonusSocketOptionData[iBonusIndex].ExtractRate)
						{
							lpItemData->m_BonusSocketOption = m_BonusSocketOptionData[iBonusIndex].OptionIndex;
							return true;
						}

						lpItemData->m_BonusSocketOption = 0xFF;
						return false;
					}
				}

				btSocketSlotCount = 0;
			}
		}
	}

	lpItemData->m_BonusSocketOption = 0xFF;
	return false;
}

bool CItemObjectSocketOptionSystem::ClearSocketSlot(CItemObject* lpTargetItem, BYTE btSocketSlotIndex)
{
	if (lpTargetItem == NULL)	return false;
	if (IsEnableSocketItem(lpTargetItem) == false)	return false;
	if (btSocketSlotIndex > 5)	return false;

	if (lpTargetItem->m_SocketOption[btSocketSlotIndex] == 0xFF ||
		lpTargetItem->m_SocketOption[btSocketSlotIndex] == 0xFE)
	{
		return false;
	}

	lpTargetItem->m_SocketOption[btSocketSlotIndex] = 0xFE;
	SetBonusSocketOption(lpTargetItem);
	return true;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

