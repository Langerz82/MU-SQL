#include "PentagramSystem.h"
#include "PentagramMixSystem.h"
#include "CUserData.h"
#include "Logging/Log.h"
#include "gObjMonster.h"
#include "Main.h"
#include "GameProtocol.h"

CPentagramSystem g_PentagramSystem;

CPentagramSystem::CPentagramSystem(void)
{
	this->Penta_DropRate = new TEST_ITEMSDROP[50];
	this->m_ServerAttributeDefine = new SERVER_ATTRIBUTE_DEFINE[14];
	this->m_PentagramItemOpenSocketRate = new PENTAGRAM_ITEM_OPEN_SOCKET_RATE[13];
	this->m_MonsterDropItemRate = new MONSTER_DROP_ITEM_RATE[30];
	this->m_JewelOutRate = new JEWEL_OUT_RATE[55];
	this->m_AttackSetEffect = new PENTAGRAM_SET_EFFECT[6];
	this->m_RelationShipSetEffect = new PENTAGRAM_SET_EFFECT[20];
	this->m_PentagramHaveSetOption = new PENTAGRAM_HAVE_SET_OPTION[20];
	this->m_PentagramItemOption = new PENTAGRAM_ITEM_OPTION[20];
	this->m_PentagramItemOptionEnableNeed = new PENTAGRAM_ITEM_OPTION_ENABLE_NEED[10];
	this->m_PentagramSocketRateByGrade = new PENTAGRAM_SOCKET_RATE_BY_GRADE[3];
}

CPentagramSystem::~CPentagramSystem(void)
{
	delete[] this->Penta_DropRate;
	delete[] this->m_ServerAttributeDefine;
	delete[] this->m_PentagramItemOpenSocketRate;
	delete[] this->m_MonsterDropItemRate;
	delete[] this->m_JewelOutRate;
	delete[] this->m_AttackSetEffect;
	delete[] this->m_RelationShipSetEffect;
	delete[] this->m_PentagramHaveSetOption;
	delete[] this->m_PentagramItemOption;
	delete[] this->m_PentagramItemOptionEnableNeed;
	delete[] this->m_PentagramSocketRateByGrade;
}

void CPentagramSystem::Initialize_Drop()
{
	for (int iClearIndex = 0; iClearIndex < 14; iClearIndex++)
	{
		this->m_ServerAttributeDefine[iClearIndex].Clear();
	}

	for (int iClearIndex = 0; iClearIndex < 13; iClearIndex++)
	{
		this->m_PentagramItemOpenSocketRate[iClearIndex].Clear();
	}

	for (int iClearIndex = 0; iClearIndex < 30; iClearIndex++)
	{
		this->m_MonsterDropItemRate[iClearIndex].Clear();
	}
}

bool CPentagramSystem::LoadDropScript(char *pchFileName)
{
	this->Initialize_Drop();

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(pchFileName);

	if (res.status != pugi::status_ok)
	{
		sLog->outError("[PentagramSystem] PentagramSystem Drop Script file Load failed. (%s) (%s)", pchFileName, res.description());
		return false;
	}

	pugi::xml_node pentagramitemdrop = file.child("PentagramItemDrop");
	pugi::xml_node serverelement = pentagramitemdrop.child("ServerElementRate");
	pugi::xml_node socketrate = pentagramitemdrop.child("SocketRate");
	pugi::xml_node itemdroprate = pentagramitemdrop.child("ItemDropRate");

	int count = 0;

	for (pugi::xml_node server = serverelement.child("Server"); server; server = server.next_sibling())
	{
		this->m_ServerAttributeDefine[count].ListIndex = server.attribute("Index").as_int();
		this->m_ServerAttributeDefine[count].ServerIndex = server.attribute("Code").as_int();
		this->m_ServerAttributeDefine[count].FireRate = server.attribute("FireDrop").as_int();
		this->m_ServerAttributeDefine[count].WaterRate = server.attribute("WaterDrop").as_int();
		this->m_ServerAttributeDefine[count].WindRate = server.attribute("WindDrop").as_int();
		this->m_ServerAttributeDefine[count].EarthRate = server.attribute("EarthDrop").as_int();
		this->m_ServerAttributeDefine[count].DarkRate = server.attribute("DarknessDrop").as_int();

		count++;
	}

	count = 0;

	for (pugi::xml_node socket = socketrate.child("Socket"); socket; socket = socket.next_sibling())
	{
		this->m_PentagramItemOpenSocketRate[count].SocketCount = socket.attribute("Count").as_int();
		this->m_PentagramItemOpenSocketRate[count].SocketOpenRate = socket.attribute("OpenRate").as_int();
		this->m_PentagramItemOpenSocketRate[count].SocketOpenSet = socket.attribute("OpenSet").as_int();
		this->m_PentagramItemOpenSocketRate[count].Slot_1 = socket.attribute("SlotRate1").as_int();
		this->m_PentagramItemOpenSocketRate[count].Slot_2 = socket.attribute("SlotRate2").as_int();
		this->m_PentagramItemOpenSocketRate[count].Slot_3 = socket.attribute("SlotRate3").as_int();
		this->m_PentagramItemOpenSocketRate[count].Slot_4 = socket.attribute("SlotRate4").as_int();
		this->m_PentagramItemOpenSocketRate[count].Slot_5 = socket.attribute("SlotRate5").as_int();

		count++;
	}

	count = 0;

	for (pugi::xml_node monster = itemdroprate.child("Monster"); monster; monster = monster.next_sibling())
	{
		this->m_MonsterDropItemRate[count].MonsterClass = monster.attribute("Index").as_int();

		char szTemp[256];

		for (int i = 0; i < 10; i++)
		{
			sprintf(szTemp, "ItemCat%d", i + 1);
			this->m_MonsterDropItemRate[count].DropItemType[i] = monster.attribute(szTemp).as_int();

			sprintf(szTemp, "ItemIndex%d", i + 1);
			this->m_MonsterDropItemRate[count].DropItemIndex[i] = monster.attribute(szTemp).as_int();

			sprintf(szTemp, "DropRate%d", i + 1);
			this->m_MonsterDropItemRate[count].DropItemRate[i] = monster.attribute(szTemp).as_int();
		}

		count++;
	}

	return true;
}

void CPentagramSystem::Initialize_JewelOutRate()
{
	for (int i = 0; i<55; i++)
	{
		this->m_JewelOutRate[i].Clear();
	}
}

bool CPentagramSystem::LoadJewelOutRate(char *pchFileName)
{
	this->Initialize_JewelOutRate();

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(pchFileName);

	if (res.status != pugi::status_ok)
	{
		sLog->outError("[PentagramSystem] PentagramSystem JewelOutRate Script file Load failed. (%s) (%s)", pchFileName, res.description());
		return false;
	}

	pugi::xml_node pentagramsys = file.child("PentagramSystem");
	int count = 0;

	for (pugi::xml_node errtel = pentagramsys.child("Errtel"); errtel; errtel = errtel.next_sibling())
	{
		this->m_JewelOutRate[count].JewelOutIndex = errtel.attribute("ID").as_int();
		this->m_JewelOutRate[count].JewelRank = errtel.attribute("Rank").as_int();
		this->m_JewelOutRate[count].JewelLevel = errtel.attribute("Level").as_int();
		this->m_JewelOutRate[count].OutRate = errtel.attribute("Rate").as_int();

		count++;
	}

	return true;
}

void CPentagramSystem::Initialize_SetOption()
{
	for (int iClearIndex = 0; iClearIndex < 6; iClearIndex++)
	{
		this->m_AttackSetEffect[iClearIndex].Clear();
	}

	for (int iClearIndex = 0; iClearIndex < 20; iClearIndex++)
	{
		this->m_RelationShipSetEffect[iClearIndex].Clear();
	}
}

bool CPentagramSystem::LoadPentagramSetOptionScript(char *pchFileName)
{
	this->Initialize_SetOption();

	/*
	SMDFile = fopen(pchFileName, "r");

	if (SMDFile == NULL)
	{
	sLog->outError("[PentagramSystem] PentagramSystem PentagramSetOption Script file Load failed. (%s)", pchFileName);
	return false;
	}

	int iCategoryIndex = 0;
	int iValueCount = 0;
	int Token;

	while (true)
	{
	Token = GetToken();

	if (Token == END)
	{
	break;
	}

	if (strcmp("end", TokenString) == 0)
	{
	break;
	}

	if (Token == NUMBER)
	{
	iCategoryIndex = TokenNumber;

	if (iCategoryIndex == 0)
	{
	iValueCount = 0;
	while (true)
	{
	Token = GetToken();

	if (strcmp("end", TokenString) == 0)
	{
	break;
	}
	}
	}

	else if (iCategoryIndex == 1)
	{
	iValueCount = 0;
	PENTAGRAM_SET_EFFECT AttackSetEffectData;
	memset(&AttackSetEffectData, 0x00, sizeof(PENTAGRAM_SET_EFFECT));

	while (true)
	{
	Token = GetToken();

	if (strcmp("end", TokenString) == 0)
	{
	break;
	}

	AttackSetEffectData.SetIndex = TokenNumber;

	for (int i = 0; i < 6; i++)
	{
	Token = GetToken();
	AttackSetEffectData.RON[i] = TokenNumber;
	}

	Token = GetToken();
	Token = GetToken();
	Token = GetToken();
	AttackSetEffectData.Value1 = TokenNumber;

	Token = GetToken();
	Token = GetToken();
	Token = GetToken();
	AttackSetEffectData.Value2 = TokenNumber;

	Token = GetToken();
	Token = GetToken();
	Token = GetToken();
	AttackSetEffectData.Value3 = TokenNumber;

	Token = GetToken();
	Token = GetToken();
	Token = GetToken();
	AttackSetEffectData.Value4 = TokenNumber;

	std::memcpy(&this->m_AttackSetEffect[iValueCount], &AttackSetEffectData, sizeof(PENTAGRAM_SET_EFFECT));
	iValueCount++;
	}
	}

	else if (iCategoryIndex == 2)
	{
	iValueCount = 0;
	PENTAGRAM_SET_EFFECT RelationshipSetEffectData;
	memset(&RelationshipSetEffectData, 0x00, sizeof(PENTAGRAM_SET_EFFECT));

	while (true)
	{
	Token = GetToken();

	if (strcmp("end", TokenString) == 0)
	{
	break;
	}

	RelationshipSetEffectData.SetIndex = TokenNumber;

	for (int i = 0; i < 6; i++)
	{
	Token = GetToken();
	RelationshipSetEffectData.RON[i] = TokenNumber;
	}

	Token = GetToken();
	Token = GetToken();
	Token = GetToken();
	RelationshipSetEffectData.Value1 = TokenNumber;

	Token = GetToken();
	Token = GetToken();
	Token = GetToken();
	RelationshipSetEffectData.Value2 = TokenNumber;

	Token = GetToken();
	Token = GetToken();
	Token = GetToken();
	RelationshipSetEffectData.Value3 = TokenNumber;

	Token = GetToken();
	Token = GetToken();
	Token = GetToken();
	RelationshipSetEffectData.Value4 = TokenNumber;

	std::memcpy(&this->m_RelationShipSetEffect[iValueCount], &RelationshipSetEffectData, sizeof(PENTAGRAM_SET_EFFECT));
	iValueCount++;
	}
	}

	else if (iCategoryIndex == 3)
	{
	iValueCount = 0;
	PENTAGRAM_HAVE_SET_OPTION PentagramHaveSetOptionData;
	memset(&PentagramHaveSetOptionData, 0x00, sizeof(PENTAGRAM_HAVE_SET_OPTION));

	while (true)
	{
	Token = GetToken();

	if (strcmp("end", TokenString) == 0)
	{
	break;
	}

	PentagramHaveSetOptionData.BundleIndex = TokenNumber;

	Token = GetToken();
	PentagramHaveSetOptionData.ItemType = TokenNumber;

	Token = GetToken();
	PentagramHaveSetOptionData.ItemIndex = TokenNumber;

	for (int i = 0; i < 26; i++)
	{
	Token = GetToken();
	PentagramHaveSetOptionData.SetOptionIndex[i] = TokenNumber;
	}

	std::memcpy(&this->m_PentagramHaveSetOption[iValueCount], &PentagramHaveSetOptionData, sizeof(PENTAGRAM_HAVE_SET_OPTION));
	iValueCount++;
	}
	}
	}
	}

	fclose(SMDFile);
	*/
	return true;
}

void CPentagramSystem::Initialize_PentagramItemOption()
{
	for (int iClearIndex = 0; iClearIndex < 20; iClearIndex++)
	{
		this->m_PentagramItemOption[iClearIndex].Clear();
	}

	for (int iClearIndex = 0; iClearIndex < 10; iClearIndex++)
	{
		this->m_PentagramItemOptionEnableNeed[iClearIndex].Clear();
	}

	for (int iClearIndex = 0; iClearIndex < 3; iClearIndex++)
	{
		this->m_PentagramSocketRateByGrade[iClearIndex].Clear();
	}
}

bool CPentagramSystem::LoadPentagramOptionScript(char *pchFileName)
{
	this->Initialize_PentagramItemOption();

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(pchFileName);

	if (res.status != pugi::status_ok)
	{
		sLog->outError("[PentagramSystem] PentagramSystem PentagramOption Script file Load failed. (%s) (%s)", pchFileName, res.description());
		return false;
	}

	int iValueCount = 0;

	PENTAGRAM_ITEM_OPTION PentagramItemOption;
	PENTAGRAM_ITEM_OPTION_ENABLE_NEED PentagramItemOptionEnableNeed;
	PENTAGRAM_SOCKET_RATE_BY_GRADE PentagramSocketRateByGrade;

	pugi::xml_node pentagramsystem = file.child("PentagramSystem");
	pugi::xml_node optionlist = pentagramsystem.child("OptionList");
	pugi::xml_node optionsetting = pentagramsystem.child("OptionSetting");
	pugi::xml_node socketprobabilityrate = pentagramsystem.child("SocketProbabilityRate");

	for (pugi::xml_node pentagram = optionlist.child("Pentagram"); pentagram; pentagram = pentagram.next_sibling())
	{
		memset(&PentagramItemOption, 0x00, sizeof(PENTAGRAM_ITEM_OPTION));

		PentagramItemOption.ItemType = pentagram.attribute("ItemCat").as_int();
		PentagramItemOption.ItemIndex = pentagram.attribute("ItemIndex").as_int();
		strcpy(PentagramItemOption.ItemName, pentagram.attribute("Name").as_string());
		PentagramItemOption.Grade = pentagram.attribute("Grade").as_int();

		for (int i = 0; i < 7; i++)
		{
			char szTemp[256];
			sprintf(szTemp, "OptionNum%d", i + 1);

			PentagramItemOption.OptionNum[i] = pentagram.attribute(szTemp).as_int();
		}

		std::memcpy(&this->m_PentagramItemOption[iValueCount], &PentagramItemOption, sizeof(PentagramItemOption));
		iValueCount++;
	}

	iValueCount = 0;

	for (pugi::xml_node errtel = optionsetting.child("Errtel"); errtel; errtel = errtel.next_sibling())
	{
		memset(&PentagramItemOptionEnableNeed, 0x00, sizeof(PENTAGRAM_ITEM_OPTION_ENABLE_NEED));

		PentagramItemOptionEnableNeed.OptionNum = errtel.attribute("OptionNum").as_int();
		strcpy(PentagramItemOptionEnableNeed.OptionName, errtel.attribute("Name").as_string());

		for (int i = 0; i < 3; i++)
		{
			char szTemp[256];

			sprintf(szTemp, "Type%d", i + 1);
			PentagramItemOptionEnableNeed.Need_ErrtelKind[i] = errtel.attribute(szTemp).as_int();

			sprintf(szTemp, "Rank%d", i + 1);
			PentagramItemOptionEnableNeed.Need_ErrtelRank[i] = errtel.attribute(szTemp).as_int();

			sprintf(szTemp, "Level%d", i + 1);
			PentagramItemOptionEnableNeed.Need_ErrtelLevel[i] = errtel.attribute(szTemp).as_int();
		}

		std::memcpy(&this->m_PentagramItemOptionEnableNeed[iValueCount], &PentagramItemOptionEnableNeed, sizeof(PentagramItemOptionEnableNeed));
		iValueCount++;
	}

	iValueCount = 0;

	for (pugi::xml_node socket = socketprobabilityrate.child("Socket"); socket; socket = socket.next_sibling())
	{
		memset(&PentagramSocketRateByGrade, 0x00, sizeof(PENTAGRAM_SOCKET_RATE_BY_GRADE));

		PentagramSocketRateByGrade.Grade = socket.attribute("Grade").as_int();

		for (int i = 0; i < 5; i++)
		{
			char szTemp[256];

			sprintf(szTemp, "Rate%d", i + 1);
			PentagramSocketRateByGrade.SocketRate[i] = socket.attribute(szTemp).as_int();
		}

		std::memcpy(&this->m_PentagramSocketRateByGrade[iValueCount], &PentagramSocketRateByGrade, sizeof(PentagramSocketRateByGrade));
		iValueCount++;
	}

	return true;
}

bool CPentagramSystem::IsPentagramItem(CItemObject &Item)
{
	if (lpItemData->IsItem() == FALSE)
	{
		return false;
	}

	return this->IsPentagramItem(lpItemData->m_Type);
}

bool CPentagramSystem::IsPentagramItem(int ItemCode)
{
	switch (ItemCode)
	{
	case ITEMGET(12, 200):
	case ITEMGET(12, 201):
	case ITEMGET(12, 202):
	case ITEMGET(12, 203):
	case ITEMGET(12, 204):
	case ITEMGET(12, 205):
	case ITEMGET(12, 206):
	case ITEMGET(12, 207):
	case ITEMGET(12, 208):
	case ITEMGET(12, 209):
	case ITEMGET(12, 210):
	case ITEMGET(12, 211):
	case ITEMGET(12, 212):
	case ITEMGET(12, 213):
	case ITEMGET(12, 214):
	case ITEMGET(12, 215):
	case ITEMGET(12, 216):
	case ITEMGET(12, 217):
	case ITEMGET(12, 218):
	case ITEMGET(12, 306):
	case ITEMGET(12, 307):
	case ITEMGET(12, 308):
		return true;
	default:
		return false;
	}
}

bool CPentagramSystem::IsPentagramMithril(int ItemIndex) // OK
{
	if (ItemIndex == ITEMGET(12, 144) || ItemIndex == ITEMGET(12, 145) || ItemIndex == ITEMGET(12, 148))
	{
		return 1;
	}

	return 0;
}

bool CPentagramSystem::IsPentagramJewel(CItemObject &Item)
{
	if (Item.IsItem() == FALSE)
	{
		return false;
	}

	return this->IsPentagramJewel(Item.m_Type);
}

bool CPentagramSystem::IsPentagramJewel(int ItemCode)
{
	switch (ItemCode)
	{
	case ITEMGET(12, 144):
	case ITEMGET(12, 145):
	case ITEMGET(12, 148):
	case ITEMGET(12, 200):
	case ITEMGET(12, 201):
	case ITEMGET(12, 202):
	case ITEMGET(12, 203):
	case ITEMGET(12, 204):
	case ITEMGET(12, 205):
	case ITEMGET(12, 206):
	case ITEMGET(12, 207):
	case ITEMGET(12, 208):
	case ITEMGET(12, 209):
	case ITEMGET(12, 210):
	case ITEMGET(12, 211):
	case ITEMGET(12, 212):
	case ITEMGET(12, 213):
	case ITEMGET(12, 214):
	case ITEMGET(12, 215):
	case ITEMGET(12, 216):
	case ITEMGET(12, 217):
	case ITEMGET(12, 218):
	case ITEMGET(12, 306):
	case ITEMGET(12, 307):
	case ITEMGET(12, 308):
	case ITEMGET(12, 221):
	case ITEMGET(12, 222):
	case ITEMGET(12, 231):
	case ITEMGET(12, 232):
	case ITEMGET(12, 241):
	case ITEMGET(12, 242):
	case ITEMGET(12, 251):
	case ITEMGET(12, 252):
	case ITEMGET(12, 261):
		return true;
	default:
		return false;
	}
}

void CPentagramSystem::ClearPentagramItem(CGameObject &Obj)
{
	

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	Obj.m_iPentagramMainAttribute = 0;
	memset(&Obj.m_PlayerData->m_PentagramOptions, 0x00, sizeof(PENTAGRAM_OPTION));
}

void CPentagramSystem::CalcPentagramItem(CGameObject &Obj, CItemObject* lpItemData)
{
	if (!lpItemData)
	{
		return;
	}

	
	BYTE btJewelPos = 0;
	BYTE btJewelIndex = 0;
	BYTE btJewelLevel = 0;
	BYTE btRank1OptionNum = 0;
	BYTE btRank1Level = 0;
	BYTE btRank2OptionNum = 0;
	BYTE btRank2Level = 0;
	BYTE btRank3OptionNum = 0;
	BYTE btRank3Level = 0;
	BYTE btRank4OptionNum = 0;
	BYTE btRank4Level = 0;
	BYTE btRank5OptionNum = 0;
	BYTE btRank5Level = 0;
	int iSocketCount = 0;
	int iRON[25];
	memset(&iRON, 0x00, sizeof(iRON));
	int iRONCount = 0;
	int iRuby_CurRank = 0;
	int iSapph_CurRank = 0;
	int iEme_CurRank = 0;
	int iTopa_CurRank = 0;
	int iOnyx_CurRank = 0;
	int iAttackSetSum = 0;
	int iRelationshipSetSum = 0;

	Obj.m_iPentagramMainAttribute = lpItemData->m_BonusSocketOption & 0x0F;
	memset(&Obj.m_PlayerData->m_PentagramOptions, 0x00, sizeof(PENTAGRAM_OPTION));

	for (int i = 0; i < 5; i++)
	{
		if (lpItemData->m_SocketOption[i] < 0xFF && lpItemData->m_SocketOption[i] != 0xFE)
		{
			for (int j = 0; j < 254; j++)
			{
				if (Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btJewelIndex == lpItemData->m_SocketOption[i])
				{
					btJewelPos = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btJewelPos;
					btJewelIndex = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btJewelIndex;
					btJewelLevel = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btLevel;
					btRank1OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank1OptionNum;
					btRank1Level = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank1Level;
					btRank2OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank2OptionNum;
					btRank2Level = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank2Level;
					btRank3OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank3OptionNum;
					btRank3Level = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank3Level;
					btRank4OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank4OptionNum;
					btRank4Level = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank4Level;
					btRank5OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank5OptionNum;
					btRank5Level = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank5Level;
					iSocketCount++;

					switch (i)
					{
					case 0: // Errtel of Anger
						if (btRank1OptionNum != 0x0F)
						{
							for (int k = 0; k < 254; k++)
							{
								if (g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex >= 0 && g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex < 10 &&
									g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex == 250 && btRank1OptionNum == g_PentagramMixSystem.m_JewelItemOptionData[k].RankOptionNum)
								{
									Obj.m_PlayerData->m_PentagramOptions->m_iRuby_1RankAddDamage = g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank1Level];
									iRON[iRONCount] = g_PentagramMixSystem.m_JewelItemOptionData[k].RON;
									iRONCount++;
									iRuby_CurRank++;
									break;
								}
							}
						}

						if (btRank2OptionNum != 0x0F)
						{
							for (int k = 0; k < 250; k++)
							{
								if (g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex >= 10 &&
									g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex < 20 &&
									btRank2OptionNum == g_PentagramMixSystem.m_JewelItemOptionData[k].RankOptionNum)
								{
									Obj.m_PlayerData->m_PentagramOptions->m_iRuby_2RankOptionNum = btRank2OptionNum;
									Obj.m_PlayerData->m_PentagramOptions->m_iRuby_2RankAddAttackRelationshipRate = g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank2Level];
									iRON[iRONCount] = g_PentagramMixSystem.m_JewelItemOptionData[k].RON;
									iRONCount++;
									iRuby_CurRank++;
									break;
								}
							}
						}

						if (btRank3OptionNum != 0x0F)
						{
							for (int k = 0; k < 250; k++)
							{
								if (g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex >= 20 &&
									g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex < 30 &&
									btRank3OptionNum == g_PentagramMixSystem.m_JewelItemOptionData[k].RankOptionNum)
								{
									Obj.m_PlayerData->m_PentagramOptions->m_iRuby_3RankOptionNum = btRank3OptionNum;
									Obj.m_PlayerData->m_PentagramOptions->m_iRuby_3RankAddDamage = g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank3Level];
									iRON[iRONCount] = g_PentagramMixSystem.m_JewelItemOptionData[k].RON;
									iRONCount++;
									iRuby_CurRank++;
									break;
								}
							}
						}

						if (btRank4OptionNum != 0x0F)
						{
							for (int k = 0; k < 250; k++)
							{
								if (g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex >= 30 &&
									g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex < 40 &&
									btRank4OptionNum == g_PentagramMixSystem.m_JewelItemOptionData[k].RankOptionNum)
								{
									Obj.m_PlayerData->m_PentagramOptions->m_iRuby_4RankOptionNum = btRank4OptionNum;
									Obj.m_PlayerData->m_PentagramOptions->m_iRuby_4RankAddDamage = g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank4Level];
									iRON[iRONCount] = g_PentagramMixSystem.m_JewelItemOptionData[k].RON;
									iRONCount++;
									iRuby_CurRank++;
									break;
								}
							}
						}

						if (btRank5OptionNum != 0x0F)
						{
							for (int k = 0; k < 250; k++)
							{
								if (g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex >= 40 &&
									g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex < 50 &&
									btRank5OptionNum == g_PentagramMixSystem.m_JewelItemOptionData[k].RankOptionNum)
								{
									Obj.m_PlayerData->m_PentagramOptions->m_iRuby_5RankOptionNum = btRank5OptionNum;
									Obj.m_PlayerData->m_PentagramOptions->m_iRuby_5RankCriticalDamageRate = g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank5Level];
									iRON[iRONCount] = g_PentagramMixSystem.m_JewelItemOptionData[k].RON;
									iRONCount++;
									iRuby_CurRank++;
									break;
								}
							}
						}
						break;
					case 1:
						if (btRank1OptionNum != 0x0F)
						{
							for (int k = 0; k < 254; k++)
							{
								if (g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex >= 50 && g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex < 60 &&
									g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex == 251 && btRank1OptionNum == g_PentagramMixSystem.m_JewelItemOptionData[k].RankOptionNum)
								{
									Obj.m_PlayerData->m_PentagramOptions->m_iSapph_1RankAddDefense = g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank1Level];
									iRON[iRONCount] = g_PentagramMixSystem.m_JewelItemOptionData[k].RON;
									iRONCount++;
									iSapph_CurRank++;
									break;
								}
							}
						}

						if (btRank2OptionNum != 0x0F)
						{
							for (int k = 0; k < 250; k++)
							{
								if (g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex >= 60 &&
									g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex < 70 &&
									btRank2OptionNum == g_PentagramMixSystem.m_JewelItemOptionData[k].RankOptionNum)
								{
									Obj.m_PlayerData->m_PentagramOptions->m_iSapph_2RankOptionNum = btRank2OptionNum;
									Obj.m_PlayerData->m_PentagramOptions->m_iSapph_2RankAddDefenseRelationshipRate = g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank2Level];
									iRON[iRONCount] = g_PentagramMixSystem.m_JewelItemOptionData[k].RON;
									iRONCount++;
									iSapph_CurRank++;
									break;
								}
							}
						}

						if (btRank3OptionNum != 0x0F)
						{
							for (int k = 0; k < 250; k++)
							{
								if (g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex >= 70 &&
									g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex < 80 &&
									btRank3OptionNum == g_PentagramMixSystem.m_JewelItemOptionData[k].RankOptionNum)
								{
									Obj.m_PlayerData->m_PentagramOptions->m_iSapph_3RankOptionNum = btRank3OptionNum;
									Obj.m_PlayerData->m_PentagramOptions->m_iSapph_3RankAddDefense = g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank3Level];
									iRON[iRONCount] = g_PentagramMixSystem.m_JewelItemOptionData[k].RON;
									iRONCount++;
									iSapph_CurRank++;
									break;
								}
							}
						}

						if (btRank4OptionNum != 0x0F)
						{
							for (int k = 0; k < 250; k++)
							{
								if (g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex >= 80 &&
									g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex < 90 &&
									btRank4OptionNum == g_PentagramMixSystem.m_JewelItemOptionData[k].RankOptionNum)
								{
									Obj.m_PlayerData->m_PentagramOptions->m_iSapph_4RankOptionNum = btRank4OptionNum;
									Obj.m_PlayerData->m_PentagramOptions->m_iSapph_4RankAddDefense = g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank4Level];
									iRON[iRONCount] = g_PentagramMixSystem.m_JewelItemOptionData[k].RON;
									iRONCount++;
									iSapph_CurRank++;
									break;
								}
							}
						}

						if (btRank5OptionNum != 0x0F)
						{
							for (int k = 0; k < 250; k++)
							{
								if (g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex >= 90 &&
									g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex < 100 &&
									btRank5OptionNum == g_PentagramMixSystem.m_JewelItemOptionData[k].RankOptionNum)
								{
									Obj.m_PlayerData->m_PentagramOptions->m_iSapph_5RankOptionNum = btRank5OptionNum;
									Obj.m_PlayerData->m_PentagramOptions->m_iSapph_5RankMinusTargetDamageRate = g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank5Level];
									iRON[iRONCount] = g_PentagramMixSystem.m_JewelItemOptionData[k].RON;
									iRONCount++;
									iSapph_CurRank++;
									break;
								}
							}
						}
						break;
					case 2:
						if (btRank1OptionNum != 0x0F)
						{
							for (int k = 0; k < 254; k++)
							{
								if (g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex >= 100 &&	g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex < 110 &&
									g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex == 252 && btRank1OptionNum == g_PentagramMixSystem.m_JewelItemOptionData[k].RankOptionNum)
								{
									Obj.m_PlayerData->m_PentagramOptions->m_iEme_1RankAddAttackRate = g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank1Level];
									iRON[iRONCount] = g_PentagramMixSystem.m_JewelItemOptionData[k].RON;
									iRONCount++;
									iEme_CurRank++;
									break;
								}
							}
						}

						if (btRank2OptionNum != 0x0F)
						{
							for (int k = 0; k < 250; k++)
							{
								if (g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex >= 110 &&
									g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex < 120 &&
									btRank2OptionNum == g_PentagramMixSystem.m_JewelItemOptionData[k].RankOptionNum)
								{
									Obj.m_PlayerData->m_PentagramOptions->m_iEme_2RankOptionNum = btRank2OptionNum;
									Obj.m_PlayerData->m_PentagramOptions->m_iEme_2RankAddAttackRelationshipRate = g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank2Level];
									iRON[iRONCount] = g_PentagramMixSystem.m_JewelItemOptionData[k].RON;
									iRONCount++;
									iEme_CurRank++;
									break;
								}
							}
						}

						if (btRank3OptionNum != 0x0F)
						{
							for (int k = 0; k < 250; k++)
							{
								if (g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex >= 120 &&
									g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex < 130 &&
									btRank3OptionNum == g_PentagramMixSystem.m_JewelItemOptionData[k].RankOptionNum)
								{
									Obj.m_PlayerData->m_PentagramOptions->m_iEme_3RankOptionNum = btRank3OptionNum;
									Obj.m_PlayerData->m_PentagramOptions->m_iEme_3RankAddDamage = g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank3Level];
									iRON[iRONCount] = g_PentagramMixSystem.m_JewelItemOptionData[k].RON;
									iRONCount++;
									iEme_CurRank++;
									break;
								}
							}
						}

						if (btRank4OptionNum != 0x0F)
						{
							for (int k = 0; k < 250; k++)
							{
								if (g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex >= 130 &&
									g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex < 140 &&
									btRank4OptionNum == g_PentagramMixSystem.m_JewelItemOptionData[k].RankOptionNum)
								{
									Obj.m_PlayerData->m_PentagramOptions->m_iEme_4RankOptionNum = btRank4OptionNum;
									Obj.m_PlayerData->m_PentagramOptions->m_iEme_4RankAddDamage = g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank4Level];
									iRON[iRONCount] = g_PentagramMixSystem.m_JewelItemOptionData[k].RON;
									iRONCount++;
									iEme_CurRank++;
									break;
								}
							}
						}

						if (btRank5OptionNum != 0x0F)
						{
							for (int k = 0; k < 250; k++)
							{
								if (g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex >= 140 &&
									g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex < 150 &&
									btRank5OptionNum == g_PentagramMixSystem.m_JewelItemOptionData[k].RankOptionNum)
								{
									Obj.m_PlayerData->m_PentagramOptions->m_iEme_5RankOptionNum = btRank5OptionNum;
									Obj.m_PlayerData->m_PentagramOptions->m_iEme_5RankAddDamageRate = g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank5Level];
									iRON[iRONCount] = g_PentagramMixSystem.m_JewelItemOptionData[k].RON;
									iRONCount++;
									iEme_CurRank++;
									break;
								}
							}
						}
						break;
					case 3:
						if (btRank1OptionNum != 0x0F)
						{
							for (int k = 0; k < 254; k++)
							{
								if (g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex >= 150 &&	g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex < 160 &&
									g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex == 253 && btRank1OptionNum == g_PentagramMixSystem.m_JewelItemOptionData[k].RankOptionNum)
								{
									Obj.m_PlayerData->m_PentagramOptions->m_iTopa_1RankOptionNum = btRank1OptionNum;
									Obj.m_PlayerData->m_PentagramOptions->m_iTopa_1RankAddDefenseSuccessRate = g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank1Level];
									iRON[iRONCount] = g_PentagramMixSystem.m_JewelItemOptionData[k].RON;
									iRONCount++;
									iTopa_CurRank++;
									break;
								}
							}
						}

						if (btRank2OptionNum != 0x0F)
						{
							for (int k = 0; k < 250; k++)
							{
								if (g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex >= 160 &&
									g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex < 170 &&
									btRank2OptionNum == g_PentagramMixSystem.m_JewelItemOptionData[k].RankOptionNum)
								{
									Obj.m_PlayerData->m_PentagramOptions->m_iTopa_2RankOptionNum = btRank2OptionNum;
									Obj.m_PlayerData->m_PentagramOptions->m_iTopa_2RankAddDefenseRelationshipRate = g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank2Level];
									iRON[iRONCount] = g_PentagramMixSystem.m_JewelItemOptionData[k].RON;
									iRONCount++;
									iTopa_CurRank++;
									break;
								}
							}
						}

						if (btRank3OptionNum != 0x0F)
						{
							for (int k = 0; k < 250; k++)
							{
								if (g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex >= 170 &&
									g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex < 180 &&
									btRank3OptionNum == g_PentagramMixSystem.m_JewelItemOptionData[k].RankOptionNum)
								{
									Obj.m_PlayerData->m_PentagramOptions->m_iTopa_3RankOptionNum = btRank3OptionNum;
									Obj.m_PlayerData->m_PentagramOptions->m_iTopa_3RankAddDefense = g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank3Level];
									iRON[iRONCount] = g_PentagramMixSystem.m_JewelItemOptionData[k].RON;
									iRONCount++;
									iTopa_CurRank++;
									break;
								}
							}
						}

						if (btRank4OptionNum != 0x0F)
						{
							for (int k = 0; k < 250; k++)
							{
								if (g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex >= 180 &&	
									g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex < 190 &&
									btRank4OptionNum == g_PentagramMixSystem.m_JewelItemOptionData[k].RankOptionNum)
								{
									Obj.m_PlayerData->m_PentagramOptions->m_iTopa_4RankOptionNum = btRank4OptionNum;
									Obj.m_PlayerData->m_PentagramOptions->m_iTopa_4RankAddDefense = g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank4Level];
									iRON[iRONCount] = g_PentagramMixSystem.m_JewelItemOptionData[k].RON;
									iRONCount++;
									iTopa_CurRank++;
									break;
								}
							}
						}

						if (btRank5OptionNum != 0x0F)
						{
							for (int k = 0; k < 250; k++)
							{
								if (g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex >= 190 && 
									g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex < 200 &&
									btRank5OptionNum == g_PentagramMixSystem.m_JewelItemOptionData[k].RankOptionNum)
								{
									Obj.m_PlayerData->m_PentagramOptions->m_iTopa_5RankOptionNum = btRank5OptionNum;
									Obj.m_PlayerData->m_PentagramOptions->m_iTopa_5RankDamageAbsorbRate = g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank5Level];
									iRON[iRONCount] = g_PentagramMixSystem.m_JewelItemOptionData[k].RON;
									iRONCount++;
									iTopa_CurRank++;
									break;
								}
							}
						}
						break;
					case 4:
						if (btRank1OptionNum != 0x0F)
						{
							for (int k = 0; k < 250; k++)
							{
								if (g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex >= 200 && g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex < 210 &&
									btRank1OptionNum == g_PentagramMixSystem.m_JewelItemOptionData[k].RankOptionNum)
								{
									Obj.m_PlayerData->m_PentagramOptions->m_iOnyx_1RankSlowMoveSkillRate = g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank1Level];
									iRON[iRONCount] = g_PentagramMixSystem.m_JewelItemOptionData[k].RON;
									iRONCount++;
									iOnyx_CurRank++;
									break;
								}
							}
						}

						if (btRank2OptionNum != 0x0F)
						{
							for (int k = 0; k < 250; k++)
							{
								if (g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex >= 210 &&
									g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex < 220 &&
									btRank2OptionNum == g_PentagramMixSystem.m_JewelItemOptionData[k].RankOptionNum)
								{
									switch (btRank2OptionNum)
									{
									case 1:
										Obj.m_PlayerData->m_PentagramOptions->m_iOnyx_2RankAddStrength = g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank2Level];
										Obj.AddStrength += g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank2Level];
										break;
									case 2:
										EnterCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);
										Obj.m_PlayerData->AgilityCheckDelay = GetTickCount();
										Obj.m_PlayerData->m_PentagramOptions->m_iOnyx_2RankAddDexterity = g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank2Level];
										Obj.AddDexterity += g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank2Level];
										LeaveCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);
										break;
									case 3:
										Obj.m_PlayerData->m_PentagramOptions->m_iOnyx_2RankAddEnergy = g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank2Level];
										Obj.AddEnergy += g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank2Level];
										break;
									case 4:
										Obj.m_PlayerData->m_PentagramOptions->m_iOnyx_2RankAddVitality = g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank2Level];
										Obj.AddVitality += g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank2Level];
										break;
									}

									iRON[iRONCount] = g_PentagramMixSystem.m_JewelItemOptionData[k].RON;
									iRONCount++;
									iOnyx_CurRank++;
									break;
								}
							}
						}

						if (btRank3OptionNum != 0x0F)
						{
							for (int k = 0; k < 250; k++)
							{
								if (g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex >= 220 &&
									g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex < 230 &&
									btRank3OptionNum == g_PentagramMixSystem.m_JewelItemOptionData[k].RankOptionNum)
								{
									switch (btRank3OptionNum)
									{
									case 1:
										Obj.m_PlayerData->m_PentagramOptions->m_iOnyx_3RankAddMaxLife = g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank3Level];
										Obj.AddLife += g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank3Level];
										break;
									case 2:
										Obj.m_PlayerData->m_PentagramOptions->m_iOnyx_3RankAddMaxMana = g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank3Level];
										Obj.AddMana += g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank3Level];
										break;
									case 3:
										Obj.m_PlayerData->m_PentagramOptions->m_iOnyx_3RankAddMaxAG = g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank3Level];
										Obj.AddBP += g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank3Level];
										break;
									case 4:
										Obj.m_PlayerData->m_PentagramOptions->m_iOnyx_3RankAddMaxSD = g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank3Level];
										Obj.iAddShield += g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank3Level];
										break;
									}

									iRON[iRONCount] = g_PentagramMixSystem.m_JewelItemOptionData[k].RON;
									iRONCount++;
									iOnyx_CurRank++;
									break;
								}
							}
						}

						if (btRank4OptionNum != 0x0F)
						{
							for (int k = 0; k < 250; k++)
							{
								if (g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex >= 230 &&
									g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex < 240 &&
									btRank4OptionNum == g_PentagramMixSystem.m_JewelItemOptionData[k].RankOptionNum)
								{
									Obj.m_PlayerData->m_PentagramOptions->m_iOnyx_4RankOptionNum = btRank4OptionNum;
									Obj.m_PlayerData->m_PentagramOptions->m_iOnyx_4RankAddExllentDamageRate = g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank4Level];
									iRON[iRONCount] = g_PentagramMixSystem.m_JewelItemOptionData[k].RON;
									iRONCount++;
									iOnyx_CurRank++;
									break;
								}
							}
						}

						if (btRank5OptionNum != 0x0F)
						{
							for (int k = 0; k < 250; k++)
							{
								if (g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex >= 240 &&
									g_PentagramMixSystem.m_JewelItemOptionData[k].JewelIndex < 250 &&
									btRank5OptionNum == g_PentagramMixSystem.m_JewelItemOptionData[k].RankOptionNum)
								{
									Obj.m_PlayerData->m_PentagramOptions->m_iOnyx_5RankHalfValueSkillRate = g_PentagramMixSystem.m_JewelItemOptionData[k].LevelValue[btRank5Level];
									iRON[iRONCount] = g_PentagramMixSystem.m_JewelItemOptionData[k].RON;
									iRONCount++;
									iOnyx_CurRank++;
									break;
								}
							}
						}
						break;
					}
				}
			}
		}
	}

	bool bEnableNeed = false;

	for (int iPentaItem = 0; iPentaItem < 20; iPentaItem++)
	{
		if (this->m_PentagramItemOption[iPentaItem].ItemType == ITEM_GET_TYPE(lpItemData->m_Type) &&
			this->m_PentagramItemOption[iPentaItem].ItemIndex == ITEM_GET_INDEX(lpItemData->m_Type))
		{
			for (int iOption = 0; iOption < 7; iOption++)
			{
				int iOptionNum = this->m_PentagramItemOption[iPentaItem].OptionNum[iOption];

				if (iOptionNum == 0)
					break;

				for (int iOptionCount = 0; iOptionCount < 10; iOptionCount++)
				{
					if (this->m_PentagramItemOptionEnableNeed[iOptionCount].OptionNum == iOptionNum)
					{
						bool bNotMatch = false;

						for (int iEnableNeedCount = 0; iEnableNeedCount < 3 && this->m_PentagramItemOptionEnableNeed[iOptionCount].Need_ErrtelKind[iEnableNeedCount] != -1; iEnableNeedCount++)
						{
							if (lpItemData->m_SocketOption[this->m_PentagramItemOptionEnableNeed[iOptionCount].Need_ErrtelKind[iEnableNeedCount]] >= 0xFE)
							{
								bNotMatch = true;
								break;
							}

							for (int l = 0; l < 254; l++)
							{
								if (Obj.m_PlayerData->m_PentagramJewelInfo_Inven[l].btJewelIndex == lpItemData->m_SocketOption[this->m_PentagramItemOptionEnableNeed[iOptionCount].Need_ErrtelKind[iEnableNeedCount]])
								{
									if (this->m_PentagramItemOptionEnableNeed[iOptionCount].Need_ErrtelRank[iEnableNeedCount] == 1)
									{
										if (Obj.m_PlayerData->m_PentagramJewelInfo_Inven[l].btRank1Level >= 15 ||
											Obj.m_PlayerData->m_PentagramJewelInfo_Inven[l].btRank1Level < this->m_PentagramItemOptionEnableNeed[iOptionCount].Need_ErrtelLevel[iEnableNeedCount])
										{
											bEnableNeed = false;
											bNotMatch = true;
										}

										else
										{
											bEnableNeed = true;
										}
										break;
									}

									if (this->m_PentagramItemOptionEnableNeed[iOptionCount].Need_ErrtelRank[iEnableNeedCount] == 2)
									{
										if (Obj.m_PlayerData->m_PentagramJewelInfo_Inven[l].btRank2Level >= 15 ||
											Obj.m_PlayerData->m_PentagramJewelInfo_Inven[l].btRank2Level < this->m_PentagramItemOptionEnableNeed[iOptionCount].Need_ErrtelLevel[iEnableNeedCount])
										{
											bEnableNeed = false;
											bNotMatch = true;
										}

										else
										{
											bEnableNeed = true;
										}
										break;
									}

									if (this->m_PentagramItemOptionEnableNeed[iOptionCount].Need_ErrtelRank[iEnableNeedCount] == 3)
									{
										if (Obj.m_PlayerData->m_PentagramJewelInfo_Inven[l].btRank3Level >= 15 ||
											Obj.m_PlayerData->m_PentagramJewelInfo_Inven[l].btRank3Level < this->m_PentagramItemOptionEnableNeed[iOptionCount].Need_ErrtelLevel[iEnableNeedCount])
										{
											bEnableNeed = false;
											bNotMatch = true;
										}

										else
										{
											bEnableNeed = true;
										}
										break;
									}

									if (this->m_PentagramItemOptionEnableNeed[iOptionCount].Need_ErrtelRank[iEnableNeedCount] == 4)
									{
										if (Obj.m_PlayerData->m_PentagramJewelInfo_Inven[l].btRank4Level >= 15 ||
											Obj.m_PlayerData->m_PentagramJewelInfo_Inven[l].btRank4Level < this->m_PentagramItemOptionEnableNeed[iOptionCount].Need_ErrtelLevel[iEnableNeedCount])
										{
											bEnableNeed = false;
											bNotMatch = true;
										}

										else
										{
											bEnableNeed = true;
										}
										break;
									}

									if (this->m_PentagramItemOptionEnableNeed[iOptionCount].Need_ErrtelRank[iEnableNeedCount] == 5)
									{
										if (Obj.m_PlayerData->m_PentagramJewelInfo_Inven[l].btRank5Level >= 15 ||
											Obj.m_PlayerData->m_PentagramJewelInfo_Inven[l].btRank5Level < this->m_PentagramItemOptionEnableNeed[iOptionCount].Need_ErrtelLevel[iEnableNeedCount])
										{
											bEnableNeed = false;
											bNotMatch = true;
										}

										else
										{
											bEnableNeed = true;
										}
										break;
									}
								}
							}
						}

						if (bEnableNeed == true && bNotMatch == false)
						{
							switch (this->m_PentagramItemOptionEnableNeed[iOptionCount].OptionNum)
							{
							case 1:
								Obj.m_PlayerData->m_PentagramOptions->m_isAddPentaAttack = true;
								break;
							case 2:
								Obj.m_PlayerData->m_PentagramOptions->m_isAddPentaDefense = true;
								break;
							case 3:
								Obj.m_PlayerData->m_PentagramOptions->m_isAddCriPentaDamage = true;
								break;
							case 4:
								Obj.m_PlayerData->m_PentagramOptions->m_isChangePentaDefense = true;
								break;
							case 5:
								Obj.m_PlayerData->m_PentagramOptions->m_isChangePentaAttack = true;
								break;
							case 6:
								Obj.m_PlayerData->m_PentagramOptions->m_isAddResistByStrongRelationShip = true;
								break;
							case 7:
								Obj.m_PlayerData->m_PentagramOptions->m_isAddResistByPentaAttack = true;
								break;
							}
						}

						break;
					}
				}
			}
			break;
		}
	}
}
bool CPentagramSystem::IsEnableDropPentagramItemMap(int iMapIndex)
{
	switch (iMapIndex)
	{
	case 1:
	case 2:
	case 4:
	case 7:
	case 8:
	case 10:
		return true;
	default:
		return false;
	}
}

int CPentagramSystem::AttributeMonsterItemDrop(CGameObject &Obj)
{
	int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

	if (MaxHitUser == -1)
	{
		return false;
	}

	CGameObject lpUser = getGameObject(MaxHitUser);

	bool bFindServer = false;
	int iMainAttribute = -1;
	bool bMakeItem = false;
	int DropItemType = 0;
	int DropItemIndex = 0;

	for (int i = 0; i<14; i++)
	{
		if (this->m_ServerAttributeDefine[i].ServerIndex == g_ConfigRead.server.GetGameServerCode())
		{
			bFindServer = true;
			int iFireRate = this->m_ServerAttributeDefine[i].FireRate;
			int iWaterRate = this->m_ServerAttributeDefine[i].WaterRate;
			int iEarthRate = this->m_ServerAttributeDefine[i].EarthRate;
			int iWindRate = this->m_ServerAttributeDefine[i].WindRate;
			int iDarkRate = this->m_ServerAttributeDefine[i].DarkRate;
			int iRandNum = rand() % 10000;

			if (iRandNum >= this->m_ServerAttributeDefine[i].FireRate)
			{
				if (iRandNum < iFireRate || iRandNum >= iWaterRate + iFireRate)
				{
					if (iRandNum < iWaterRate + iFireRate || iRandNum >= iEarthRate + iWaterRate + iFireRate)
					{
						if (iRandNum < iEarthRate + iWaterRate + iFireRate || iRandNum >= iWindRate + iEarthRate + iWaterRate + iFireRate)
						{
							if (iRandNum >= iWindRate + iEarthRate + iWaterRate + iFireRate && iRandNum < iDarkRate + iWindRate + iEarthRate + iWaterRate + iFireRate)
							{
								iMainAttribute = 5;
							}
						}
						else
						{
							iMainAttribute = 4;
						}
					}
					else
					{
						iMainAttribute = 3;
					}
				}
				else
				{
					iMainAttribute = 2;
				}
			}
			else
			{
				iMainAttribute = 1;
			}
			break;
		}
	}

	if (!bFindServer)
	{
		int iRandNum = rand() % 5;

		if (!iRandNum)
			iMainAttribute = 1;
		if (iRandNum == 1)
			iMainAttribute = 2;
		if (iRandNum == 2)
			iMainAttribute = 3;
		if (iRandNum == 3)
			iMainAttribute = 4;
		if (iRandNum == 4)
			iMainAttribute = 5;
	}

	for (int j = 0; j < 30; ++j)
	{
		if (Obj.Class == this->m_MonsterDropItemRate[j].MonsterClass)
		{
			int iItem1Rate = this->m_MonsterDropItemRate[j].DropItemRate[0];
			int iItem2Rate = this->m_MonsterDropItemRate[j].DropItemRate[1];
			int iItem3Rate = this->m_MonsterDropItemRate[j].DropItemRate[2];
			int iItem4Rate = this->m_MonsterDropItemRate[j].DropItemRate[3];
			int iItem5Rate = this->m_MonsterDropItemRate[j].DropItemRate[4];
			int iItem6Rate = this->m_MonsterDropItemRate[j].DropItemRate[5];
			int iRandNum = rand() % 10000;

			if (iRandNum >= iItem1Rate)
			{
				if (iRandNum < iItem1Rate || iRandNum >= iItem2Rate + iItem1Rate)
				{
					if (iRandNum < iItem2Rate + iItem1Rate || iRandNum >= iItem3Rate + iItem2Rate + iItem1Rate)
					{
						if (iRandNum < iItem3Rate + iItem2Rate + iItem1Rate || iRandNum >= iItem4Rate + iItem3Rate + iItem2Rate + iItem1Rate)
						{
							if (iRandNum < iItem4Rate + iItem3Rate + iItem2Rate + iItem1Rate || iRandNum >= iItem5Rate + iItem4Rate + iItem3Rate + iItem2Rate + iItem1Rate)
							{
								if (iRandNum >= iItem5Rate + iItem4Rate + iItem3Rate + iItem2Rate + iItem1Rate && iRandNum < iItem6Rate + iItem5Rate + iItem4Rate + iItem3Rate + iItem2Rate + iItem1Rate)
								{
									bMakeItem = true;
									DropItemType = this->m_MonsterDropItemRate[j].DropItemType[5];
									DropItemIndex = this->m_MonsterDropItemRate[j].DropItemIndex[5];
								}
							}

							else
							{
								bMakeItem = true;
								DropItemType = this->m_MonsterDropItemRate[j].DropItemType[4];
								DropItemIndex = this->m_MonsterDropItemRate[j].DropItemIndex[4];
							}
						}

						else
						{
							bMakeItem = true;
							DropItemType = this->m_MonsterDropItemRate[j].DropItemType[3];
							DropItemIndex = this->m_MonsterDropItemRate[j].DropItemIndex[3];
						}
					}

					else
					{
						bMakeItem = true;
						DropItemType = this->m_MonsterDropItemRate[j].DropItemType[2];
						DropItemIndex = this->m_MonsterDropItemRate[j].DropItemIndex[2];
					}
				}
				else
				{
					bMakeItem = true;
					DropItemType = this->m_MonsterDropItemRate[j].DropItemType[1];
					DropItemIndex = this->m_MonsterDropItemRate[j].DropItemIndex[1];
				}
			}
			else
			{
				bMakeItem = true;
				DropItemType = this->m_MonsterDropItemRate[j].DropItemType[0];
				DropItemIndex = this->m_MonsterDropItemRate[j].DropItemIndex[0];
			}
			break;
		}
	}

	if (bMakeItem == true)
	{
		int ItemNumber = ItemGetNumberMake(DropItemType, DropItemIndex);
		ItemCreate(Obj.m_Index, Obj.MapNumber, Obj.X, Obj.Y, ItemNumber, 0, ItemGetDurability(ItemNumber, 0, 0, 0), 0, 0, 0, MaxHitUser, 0, 0, 0, 0, iMainAttribute | 0x10);
		//sLog->outBasic("[PentagramSystem] Pentagram Item Drop [%s]: [%s][%s] (type:%d)", Obj.Name, lpUser->AccountID, lpUser->Name, ItemNumber);

		return true;
	}

	return false;
}

BYTE CPentagramSystem::GetMakePentagramSlotCountNKind(BYTE *btEnableSlot, int iType)
{
	int iSocketSlotCount = 0;
	int iSocketAddRate = 0;
	int iSetAddRate = 0;
	int iSlotNumAddRate = 0;
	int iSlotSetRand = 0;
	int bSlotOpen = 0;
	int iSocketCount = 0;
	int iSlot_1 = 0;
	int iSlot_2 = 0;
	int iSlot_3 = 0;
	int iSlot_4 = 0;
	int iSlot_5 = 0;
	int iSlotNoRand = 0;
	int iGrade = 0;
	int iItemType = ITEM_GET_TYPE(iType);
	int iItemIndex = ITEM_GET_INDEX(iType);

	for (int iPentaItem = 0; iPentaItem < 20; iPentaItem++)
	{
		if (this->m_PentagramItemOption[iPentaItem].ItemType == iItemType &&
			this->m_PentagramItemOption[iPentaItem].ItemIndex == iItemIndex)
		{
			iGrade = this->m_PentagramItemOption[iPentaItem].Grade;
			break;
		}
	}

	for (int iTempGrade = 0; iTempGrade < 3; iTempGrade++)
	{
		if (this->m_PentagramSocketRateByGrade[iTempGrade].Grade == iGrade)
		{
			int iRate = 0;
			int iSocketRateRand = rand() % 10000;

			for (int iSocket = 0; iSocket < 5; iSocket++)
			{
				iRate += this->m_PentagramSocketRateByGrade[iTempGrade].SocketRate[iSocket];

				if (iSocketRateRand < iRate + 1)
				{

					iSocketCount = iSocket + 1;
					goto END_LOOP;
				}
			}

			break;
		}
	}

END_LOOP:
	if (iGrade <= 1)
	{
		for (int i = 0; i < 5; i++)
			btEnableSlot[i] = 0;

		int iSlotCountRand = rand() % 10000;

		for (int i = 0; i < 13; i++)
		{
			if (this->m_PentagramItemOpenSocketRate[i].SocketCount != iSocketCount)
			{
				iSocketCount = this->m_PentagramItemOpenSocketRate[i].SocketCount;

				if (iSlotCountRand < this->m_PentagramItemOpenSocketRate[i].SocketOpenRate + iSocketAddRate)
				{
					iSocketSlotCount = this->m_PentagramItemOpenSocketRate[i].SocketCount;
					iSlotSetRand = rand() % 10000;

					for (int j = 0; j < 13; ++j)
					{
						if (iSocketSlotCount == this->m_PentagramItemOpenSocketRate[j].SocketCount && iSlotSetRand < this->m_PentagramItemOpenSocketRate[j].SocketOpenSet + iSetAddRate)
						{
							iSlotNoRand = rand() % 10000;
							iSlot_1 = 1;

							if (this->m_PentagramItemOpenSocketRate[j].Slot_2 == 10000)
							{
								iSlot_2 = 1;
							}

							else
							{
								if (iSlotNoRand >= this->m_PentagramItemOpenSocketRate[j].Slot_2)
								{
									iSlot_2 = 0;
									iSlotNumAddRate = this->m_PentagramItemOpenSocketRate[j].Slot_2;
								}

								else
								{
									iSlot_2 = 1;
									bSlotOpen = 1;
								}
							}

							if (this->m_PentagramItemOpenSocketRate[j].Slot_3 == 10000)
							{
								iSlot_3 = 1;
							}
							else
							{
								if (iSlotNoRand >= this->m_PentagramItemOpenSocketRate[j].Slot_3 + iSlotNumAddRate || bSlotOpen)
								{
									iSlot_3 = 0;
									iSlotNumAddRate += this->m_PentagramItemOpenSocketRate[j].Slot_3;
								}
								else
								{
									iSlot_3 = 1;
									bSlotOpen = 1;
								}
							}
							if (this->m_PentagramItemOpenSocketRate[j].Slot_4 == 10000)
							{
								iSlot_4 = 1;
							}
							else
							{
								if (iSlotNoRand >= this->m_PentagramItemOpenSocketRate[j].Slot_4 + iSlotNumAddRate || bSlotOpen)
								{
									iSlot_4 = 0;
									iSlotNumAddRate += this->m_PentagramItemOpenSocketRate[j].Slot_4;
								}
								else
								{
									iSlot_4 = 1;
									bSlotOpen = 1;
								}
							}

							if (this->m_PentagramItemOpenSocketRate[j].Slot_5 == 10000)
								iSlot_5 = 1;
							else
								iSlot_5 = iSlotNoRand < this->m_PentagramItemOpenSocketRate[j].Slot_5 + iSlotNumAddRate && !bSlotOpen;
							goto END_LOOP2;
						}
						iSetAddRate += this->m_PentagramItemOpenSocketRate[j].SocketOpenSet;
					}
					break;
				}
				iSocketAddRate += this->m_PentagramItemOpenSocketRate[i].SocketOpenRate;
			}
		}
	}

	else
	{
		for (int i = 0; i < 5; i++)
		{
			btEnableSlot[i] = 0;
		}

		int iSocketAddCount = 0;
		int iRemainCount = 0;

		for (int k = 0; k < 20; k++)
		{
			if (this->m_PentagramItemOption[k].ItemType == iItemType &&
				this->m_PentagramItemOption[k].ItemIndex == iItemIndex)
			{
				for (int iOption = 0; iOption < 7; iOption++)
				{
					int iOptionNum = this->m_PentagramItemOption[k].OptionNum[iOption];

					if (iOptionNum == 0)
					{
						break;
					}

					for (int iOptionCount = 0; iOptionCount < 10; iOptionCount++)
					{
						if (this->m_PentagramItemOptionEnableNeed[iOptionCount].OptionNum == iOptionNum)
						{
							iSlot_1 = TRUE;

							for (int iEnableNeedCount = 0; iEnableNeedCount < 3 && this->m_PentagramItemOptionEnableNeed[iOptionCount].Need_ErrtelKind[iEnableNeedCount] != -1; iEnableNeedCount++)
							{
								switch (this->m_PentagramItemOptionEnableNeed[iOptionCount].Need_ErrtelKind[iEnableNeedCount])
								{
								case 1:
									iSlot_2 = TRUE;
									break;
								case 2:
									iSlot_3 = TRUE;
									break;
								case 3:
									iSlot_4 = TRUE;
									break;
								case 4:
									iSlot_5 = TRUE;
									break;
								default:
									continue;
								}
							}
							break;
						}
					}
				}
				break;
			}
		}

		if (iSlot_1 == TRUE)
			iSocketAddCount = 1;
		else
			iRemainCount = 1;
		if (iSlot_2 == TRUE)
			iSocketAddCount++;
		else
			iRemainCount++;
		if (iSlot_3 == TRUE)
			iSocketAddCount++;
		else
			iRemainCount++;
		if (iSlot_4 == TRUE)
			iSocketAddCount++;
		else
			iRemainCount++;
		if (iSlot_5 == TRUE)
			iSocketAddCount++;
		else
			iRemainCount++;

		if (iSocketAddCount < iSocketCount)
		{
			for (int s = 0; s < iSocketCount - iSocketAddCount; s++)
			{
				int iRand = rand() % 10000;
				if (iRand % iRemainCount)
				{
					if (iRand % iRemainCount == 1)
					{
						if (iSlot_2)
						{
							if (iSlot_3)
							{
								if (!iSlot_4 && !iSlot_5)
								{
									iSlot_5 = 1;
									--iRemainCount;
								}
							}
							else
							{
								if (iSlot_4)
								{
									if (!iSlot_5)
									{
										iSlot_5 = 1;
										--iRemainCount;
									}
								}
								else
								{
									iSlot_4 = 1;
									--iRemainCount;
								}
							}
						}
						else
						{
							if (iSlot_3)
							{
								if (iSlot_4)
								{
									if (!iSlot_5)
									{
										iSlot_5 = 1;
										--iRemainCount;
									}
								}
								else
								{
									iSlot_4 = 1;
									--iRemainCount;
								}
							}
							else
							{
								iSlot_3 = 1;
								--iRemainCount;
							}
						}
					}
					else
					{
						if (iRand % iRemainCount == 2)
						{
							if (iSlot_2)
							{
								if (!iSlot_3 && !iSlot_4 && !iSlot_5)
								{
									iSlot_5 = 1;
									--iRemainCount;
								}
							}
							else
							{
								if (!iSlot_3)
								{
									if (iSlot_4)
									{
										if (!iSlot_5)
										{
											iSlot_5 = 1;
											--iRemainCount;
										}
									}
									else
									{
										iSlot_4 = 1;
										--iRemainCount;
									}
								}
							}
						}
						else
						{
							if (iRand % iRemainCount == 3 && !iSlot_2 && !iSlot_3 && !iSlot_4 && !iSlot_5)
							{
								iSlot_5 = 1;
								--iRemainCount;
							}
						}
					}
				}
				else
				{
					if (iSlot_2)
					{
						if (iSlot_3)
						{
							if (iSlot_4)
							{
								if (!iSlot_5)
								{
									iSlot_5 = 1;
									--iRemainCount;
								}
							}
							else
							{
								iSlot_4 = 1;
								--iRemainCount;
							}
						}
						else
						{
							iSlot_3 = 1;
							--iRemainCount;
						}
					}
					else
					{
						iSlot_2 = 1;
						--iRemainCount;
					}
				}
			}
		}
	}

END_LOOP2:
	if (iSlot_1)
		btEnableSlot[0] = 0xFE;
	if (iSlot_2)
		btEnableSlot[1] = 0xFE;
	if (iSlot_3)
		btEnableSlot[2] = 0xFE;
	if (iSlot_4)
		btEnableSlot[3] = 0xFE;
	if (iSlot_5)
		btEnableSlot[4] = 0xFE;
	//sLog->outBasic("[K2] Pentagram iSlot_ %d %d %d %d %d", iSlot_1, iSlot_2, iSlot_3, iSlot_4, iSlot_5);
	return iSlot_1 + iSlot_2 + iSlot_3 + iSlot_4 + iSlot_5;
}

bool CPentagramSystem::SetPentagramMainAttribute(CItemObject *lpItemData, BYTE btAttributeNumber)
{
	if (!lpItemData)
	{
		return false;
	}

	if (this->IsPentagramItem(lpItemData) == false && this->IsPentagramJewel(lpItemData) == false)
	{
		return false;
	}

	lpItemData->m_BonusSocketOption = btAttributeNumber | 0x10 * ((lpItemData->m_BonusSocketOption & 0x0F) >> 4);
	return true;
}

bool CPentagramSystem::MakePentagramSocketSlot(CItemObject *lpItemData, BYTE btSocketSlot1, BYTE btSocketSlot2, BYTE btSocketSlot3, BYTE btSocketSlot4, BYTE btSocketSlot5)
{
	if (!lpItemData)
	{
		return false;
	}

	if (this->IsPentagramItem(lpItemData) == false)
	{
		return false;
	}

	if (btSocketSlot1 == TRUE)
		lpItemData->m_SocketOption[0] = 0xFE;
	else
		lpItemData->m_SocketOption[0] = 0xFF;

	if (btSocketSlot2 == TRUE)
		lpItemData->m_SocketOption[1] = 0xFE;
	else
		lpItemData->m_SocketOption[1] = 0xFF;

	if (btSocketSlot3 == TRUE)
		lpItemData->m_SocketOption[2] = 0xFE;
	else
		lpItemData->m_SocketOption[2] = 0xFF;

	if (btSocketSlot4 == TRUE)
		lpItemData->m_SocketOption[3] = 0xFE;
	else
		lpItemData->m_SocketOption[3] = 0xFF;

	if (btSocketSlot5 == TRUE)
		lpItemData->m_SocketOption[4] = 0xFE;
	else
		lpItemData->m_SocketOption[4] = 0xFF;

	return true;

}

bool CPentagramSystem::ClearPentagramSocketSlot(CGameObject &Obj, int iInventoryPos, CItemObject *lpTargetItem, BYTE btSocketSlotIndex)
{
	if (!lpTargetItem)
	{
		return false;
	}

	if (this->IsPentagramItem(lpTargetItem) == false)
	{
		return false;
	}

	if (btSocketSlotIndex <= 5)
	{
		BYTE btJewelDBIndex;
		BYTE btJewelPos;

		this->PentagramJewel_OUT(Obj.m_Index, iInventoryPos, btSocketSlotIndex, &btJewelPos, &btJewelDBIndex);
		return true;
	}

	else
	{
		return false;
	}
}
bool CPentagramSystem::AddRadianceSlot(BYTE TargetSlot)
{
	if (TargetSlot == 0)
		return false;
	//m_SocketOption[0] = 0xFE;
}
bool CPentagramSystem::SetPentagramSocketSlot(CItemObject *lpTargetItem, BYTE bt1RankOptionNum, BYTE bt1RankLevel, BYTE bt2RankOptionNum, BYTE bt2RankLevel, BYTE bt3RankOptionNum, BYTE bt3RankLevel, BYTE bt4RankOptionNum, BYTE bt4RankLevel, BYTE bt5RankOptionNum, BYTE bt5RankLevel, BYTE curRank)
{
	if (!lpTargetItem)
	{
		return false;
	}

	if (this->IsPentagramJewel(lpTargetItem) == false)
	{
		return false;
	}

	if (bt1RankOptionNum <= 0)
		lpTargetItem->m_SocketOption[0] = 0xFF;
	else
		lpTargetItem->m_SocketOption[0] = bt1RankOptionNum | 0x10 * bt1RankLevel;

	if (bt2RankOptionNum <= 0)
		lpTargetItem->m_SocketOption[1] = 0xFF;
	else
		lpTargetItem->m_SocketOption[1] = bt2RankOptionNum | 0x10 * bt2RankLevel;

	if (bt3RankOptionNum <= 0)
		lpTargetItem->m_SocketOption[2] = 0xFF;
	else
		lpTargetItem->m_SocketOption[2] = bt3RankOptionNum | 0x10 * bt3RankLevel;

	if (bt4RankOptionNum <= 0)
		lpTargetItem->m_SocketOption[3] = 0xFF;
	else
		lpTargetItem->m_SocketOption[3] = bt4RankOptionNum | 0x10 * bt4RankLevel;

	if (bt5RankOptionNum <= 0)
		lpTargetItem->m_SocketOption[4] = 0xFF;
	else
		lpTargetItem->m_SocketOption[4] = bt5RankOptionNum | 0x10 * bt5RankLevel;

	if (!curRank)
		curRank = 1;

	BYTE btMainAttribute = lpTargetItem->m_BonusSocketOption & 0x0F;

	switch (curRank)
	{
	case 1:
		lpTargetItem->m_Level = bt1RankLevel;
		break;
	case 2:
		lpTargetItem->m_Level = bt2RankLevel;
		break;
	case 3:
		lpTargetItem->m_Level = bt3RankLevel;
		break;
	case 4:
		lpTargetItem->m_Level = bt4RankLevel;
		break;
	case 5:
		lpTargetItem->m_Level = bt5RankLevel;
		break;
	}

	lpTargetItem->m_BonusSocketOption = btMainAttribute | 0x10 * curRank;

	return true;
}

bool CPentagramSystem::SwitchPentagramJewel(CGameObject &Obj, CItemObject *lpSourceItem, int iSwitchType)
{
	

	if (Obj.Type != OBJ_USER)
	{
		return false;
	}

	bool bReturnValue = true;
	BYTE iOriginJewelIndex = 0xFF;
	BYTE iItemType = 0xFF;
	WORD iItemIndex = 0xFFFF;
	BYTE iMainAttribute = -1;
	BYTE iJewelLevel = -1;
	BYTE iRank1OptionNum = -1;
	BYTE iRank1Level = -1;
	BYTE iRank2OptionNum = -1;
	BYTE iRank2Level = -1;
	BYTE iRank3OptionNum = -1;
	BYTE iRank3Level = -1;
	BYTE iRank4OptionNum = -1;
	BYTE iRank4Level = -1;
	BYTE iRank5OptionNum = -1;
	BYTE iRank5Level = -1;

	if (iSwitchType == 1)
	{
		for (int i = 0; i < 5; i++)
		{
			if (lpSourceItem->m_SocketOption[i] < 0xFE)
			{
				for (int j = 0; j < 254; j++)
				{
					if (Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btJewelIndex == lpSourceItem->m_SocketOption[i])
					{
						iOriginJewelIndex = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btJewelIndex;
						iItemType = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btItemType;
						iItemIndex = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].wItemIndex;
						iMainAttribute = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btMainAttribute;
						iJewelLevel = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btLevel;
						iRank1OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank1OptionNum;
						iRank1Level = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank1Level;
						iRank2OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank2OptionNum;
						iRank2Level = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank2Level;
						iRank3OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank3OptionNum;
						iRank3Level = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank3Level;
						iRank4OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank4OptionNum;
						iRank4Level = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank4Level;
						iRank5OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank5OptionNum;
						iRank5Level = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank5Level;
						break;
					}
				}

				for (int iJewelIndex = 0; iJewelIndex < 254; iJewelIndex++)
				{
					BOOL bIndexExist = FALSE;

					for (int m = 0; m < 254; m++)
					{
						if (Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[m].btJewelIndex == iJewelIndex)
						{
							bIndexExist = TRUE;
							break;
						}
					}

					if (!bIndexExist)
					{
						lpSourceItem->m_BonusSocketOption = iMainAttribute;
						lpSourceItem->m_SocketOption[i] = iJewelIndex;
						this->AddPentagramJewelInfo(Obj.m_Index, 1, iJewelIndex, iItemType, iItemIndex, iMainAttribute, iJewelLevel,
							iRank1OptionNum, iRank1Level, iRank2OptionNum, iRank2Level, iRank3OptionNum, iRank3Level,
							iRank4OptionNum, iRank4Level, iRank5OptionNum, iRank5Level);
						this->DelPentagramJewelInfo(Obj.m_Index, 0, iOriginJewelIndex);
						break;
					}

					if (iJewelIndex == 254)
					{
						bReturnValue = FALSE;
						break;
					}
				}
			}
		}
	}

	else if (iSwitchType == 0)
	{
		for (int i = 0; i < 5; i++)
		{
			if (lpSourceItem->m_SocketOption[i] < 0xFE)
			{
				for (int j = 0; j < 254; j++)
				{
					if (Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[j].btJewelIndex == lpSourceItem->m_SocketOption[i])
					{
						iOriginJewelIndex = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[j].btJewelIndex;
						iItemType = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[j].btItemType;
						iItemIndex = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[j].wItemIndex;
						iMainAttribute = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[j].btMainAttribute;
						iJewelLevel = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[j].btLevel;
						iRank1OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[j].btRank1OptionNum;
						iRank1Level = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[j].btRank1Level;
						iRank2OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[j].btRank2OptionNum;
						iRank2Level = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[j].btRank2Level;
						iRank3OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[j].btRank3OptionNum;
						iRank3Level = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[j].btRank3Level;
						iRank4OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[j].btRank4OptionNum;
						iRank4Level = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[j].btRank4Level;
						iRank5OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[j].btRank5OptionNum;
						iRank5Level = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[j].btRank5Level;
						break;
					}
				}

				for (int iJewelIndex = 0; iJewelIndex < 254; iJewelIndex++)
				{
					BOOL bIndexExist = FALSE;

					for (int m = 0; m < 254; m++)
					{
						if (Obj.m_PlayerData->m_PentagramJewelInfo_Inven[m].btJewelIndex == iJewelIndex)
						{
							bIndexExist = TRUE;
							break;
						}
					}

					if (!bIndexExist)
					{
						lpSourceItem->m_BonusSocketOption = iMainAttribute;
						lpSourceItem->m_SocketOption[i] = iJewelIndex;
						this->AddPentagramJewelInfo(Obj.m_Index, 0, iJewelIndex, iItemType, iItemIndex, iMainAttribute, iJewelLevel,
							iRank1OptionNum, iRank1Level, iRank2OptionNum, iRank2Level, iRank3OptionNum, iRank3Level,
							iRank4OptionNum, iRank4Level, iRank5OptionNum, iRank5Level);
						this->DelPentagramJewelInfo(Obj.m_Index, 1, iOriginJewelIndex);
						break;
					}

					if (iJewelIndex == 254)
					{
						bReturnValue = FALSE;
						break;
					}
				}
			}
		}
	}

	return bReturnValue;
}

bool CPentagramSystem::AddPentagramJewelInfo(CGameObject &Obj, int iJewelPos, int iJewelIndex, int iItemType, int iItemIndex, int iMainAttribute, int iJewelLevel, BYTE btRank1OptionNum, BYTE btRank1Level, BYTE btRank2OptionNum, BYTE btRank2Level, BYTE btRank3OptionNum, BYTE btRank3Level, BYTE btRank4OptionNum, BYTE btRank4Level, BYTE btRank5OptionNum, BYTE btRank5Level)
{
	
	bool iAddSuccess = false;

	if (Obj.Type != OBJ_USER)
	{
		return false;
	}

	if (iJewelPos == 1)
	{
		for (int i = 0; i < 254; i++)
		{
			if (Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btJewelIndex == 0xFF)
			{
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btJewelPos = TRUE;
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btJewelIndex = iJewelIndex;
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btItemType = iItemType;
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].wItemIndex = iItemIndex;
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btMainAttribute = iMainAttribute;
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btLevel = iJewelLevel;
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank1OptionNum = btRank1OptionNum;
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank1Level = btRank1Level;
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank2OptionNum = btRank2OptionNum;
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank2Level = btRank2Level;
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank3OptionNum = btRank3OptionNum;
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank3Level = btRank3Level;
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank4OptionNum = btRank4OptionNum;
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank4Level = btRank4Level;
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank5OptionNum = btRank5OptionNum;
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank5Level = btRank5Level;
				iAddSuccess = true;
				break;
			}
		}
	}

	else if (iJewelPos == 0)
	{
		for (int i = 0; i < 254; i++)
		{
			if (Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btJewelIndex == 0xFF)
			{
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btJewelPos = 0;
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btJewelIndex = iJewelIndex;
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btItemType = iItemType;
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].wItemIndex = iItemIndex;
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btMainAttribute = iMainAttribute;
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btLevel = iJewelLevel;
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank1OptionNum = btRank1OptionNum;
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank1Level = btRank1Level;
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank2OptionNum = btRank2OptionNum;
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank2Level = btRank2Level;
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank3OptionNum = btRank3OptionNum;
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank3Level = btRank3Level;
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank4OptionNum = btRank4OptionNum;
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank4Level = btRank4Level;
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank5OptionNum = btRank5OptionNum;
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank5Level = btRank5Level;
				iAddSuccess = true;
				break;
			}
		}
	}

	if (iAddSuccess == false)
	{
		sLog->outBasic("[AddPentagramJewelInfo] - Error - [%s] [%d]", __FILE__, __LINE__);
		return iAddSuccess;
	}

	this->DBREQ_InsertPentagramJewel(Obj.m_Index, iJewelPos, iJewelIndex, iItemType, iItemIndex, iMainAttribute,
		iJewelLevel, btRank1OptionNum, btRank1Level, btRank2OptionNum, btRank2Level,
		btRank3OptionNum, btRank3Level, btRank4OptionNum, btRank4Level, btRank5OptionNum, btRank5Level);

	return iAddSuccess;
}

bool CPentagramSystem::DelPentagramJewelInfo(CGameObject &Obj, CItemObject *lpItemData)
{
	if (!lpItemData)
	{
		return false;
	}

	if (lpItemData->IsItem() == FALSE)
	{
		return false;
	}

	for (int i = 0; i < 5; i++)
	{
		if (lpItemData->m_SocketOption[i] < 0xFE)
		{
			this->DelPentagramJewelInfo(Obj.m_Index, 0, lpItemData->m_SocketOption[i]);
		}
	}

	return true;
}

bool CPentagramSystem::DelPentagramJewelInfo(CGameObject &Obj, int iJewelPos, int iJewelIndex)
{
	
	bool iDelSuccess = false;

	if (Obj.Type != OBJ_USER)
	{
		return false;
	}

	if (iJewelPos == 1)
	{
		for (int i = 0; i < 254; i++)
		{
			if (Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btJewelIndex == iJewelIndex)
			{
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btJewelPos = 1;
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btJewelIndex = -1;
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btItemType = -1;
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].wItemIndex = -1;
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btMainAttribute = -1;
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btLevel = 0;
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank1OptionNum = -1;
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank1Level = -1;
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank2OptionNum = -1;
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank2Level = -1;
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank3OptionNum = -1;
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank3Level = -1;
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank4OptionNum = -1;
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank4Level = -1;
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank5OptionNum = -1;
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank5Level = -1;
				iDelSuccess = true;
				break;
			}
		}
	}

	else if (iJewelPos == 0)
	{
		for (int i = 0; i < 254; i++)
		{
			if (Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btJewelIndex == iJewelIndex)
			{
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btJewelPos = 0;
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btJewelIndex = -1;
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btItemType = -1;
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].wItemIndex = -1;
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btMainAttribute = -1;
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btLevel = 0;
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank1OptionNum = -1;
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank1Level = -1;
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank2OptionNum = -1;
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank2Level = -1;
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank3OptionNum = -1;
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank3Level = -1;
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank4OptionNum = -1;
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank4Level = -1;
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank5OptionNum = -1;
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank5Level = -1;
				iDelSuccess = true;
				break;
			}
		}
	}

	if (iDelSuccess == false)
	{
		sLog->outBasic("[DelPentagramJewelInfo] - Error - [%s] [%d]", __FILE__, __LINE__);
		return iDelSuccess;
	}

	this->DBREQ_DelPentagramJewel(Obj.m_Index, iJewelPos, iJewelIndex);
	return iDelSuccess;
}

struct PMSG_REQ_PENTAGRAMJEWEL
{
	PBMSG_HEAD2 h;
	CGameObject &Obj;
	BYTE btJewelPos;
	int iUserGuid;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
};

void CPentagramSystem::DBREQ_GetPentagramJewel(CGameObject lpObj, char *szAccountId, int iJewelPos)
{
	PMSG_REQ_PENTAGRAMJEWEL pMsg;

	pMsg.iUserIndex = Obj.m_Index;
	pMsg.btJewelPos = iJewelPos;
	pMsg.iUserGuid = Obj.DBNumber;
	std::memcpy(&pMsg.szAccountID, szAccountId, MAX_ACCOUNT_LEN);
	pMsg.szAccountID[10] = 0;
	std::memcpy(&pMsg.szName, Obj.Name, MAX_ACCOUNT_LEN);
	pMsg.szName[10] = 0;

	PHeadSetB((BYTE*)&pMsg, 0xE0, sizeof(pMsg));
	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

void CPentagramSystem::DBANS_GetPentagramJewel(BYTE* lpRecv)
{
	PMSG_ANS_PENTAGRAMJEWEL * lpMsg = (PMSG_ANS_PENTAGRAMJEWEL*)lpRecv;

	CGameObject &Obj = lpMsg->iUserIndex;

	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	if (lpMsg->btJewelCnt < 0 || lpMsg->btJewelCnt > 249)
	{
		sLog->outBasic("[PentagramJewel] - Error - [%s] [%d]", __FILE__, __LINE__);
		return;
	}

	

	if (lpMsg->iAnsType == 1)
	{
		for (int i = 0; i < lpMsg->btJewelCnt; i++)
		{
			PENTAGRAMJEWEL_INFO *lpPentagramInfo = (PENTAGRAMJEWEL_INFO *)(lpRecv + sizeof(PMSG_ANS_PENTAGRAMJEWEL) + (i * sizeof(PENTAGRAMJEWEL_INFO)));

			Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btJewelPos = lpPentagramInfo->btJewelPos;
			Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btJewelIndex = lpPentagramInfo->btJewelIndex;
			Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btItemType = lpPentagramInfo->btItemType;
			Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].wItemIndex = lpPentagramInfo->wItemIndex;
			Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btMainAttribute = lpPentagramInfo->btMainAttribute;
			Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btLevel = lpPentagramInfo->btLevel;
			Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank1OptionNum = lpPentagramInfo->btRank1OptionNum;
			Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank1Level = lpPentagramInfo->btRank1Level;
			Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank2OptionNum = lpPentagramInfo->btRank2OptionNum;
			Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank2Level = lpPentagramInfo->btRank2Level;
			Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank3OptionNum = lpPentagramInfo->btRank3OptionNum;
			Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank3Level = lpPentagramInfo->btRank3Level;
			Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank4OptionNum = lpPentagramInfo->btRank4OptionNum;
			Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank4Level = lpPentagramInfo->btRank4Level;
			Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank5OptionNum = lpPentagramInfo->btRank5OptionNum;
			Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank5Level = lpPentagramInfo->btRank5Level;
		}
	}

	else if (lpMsg->iAnsType == 0)
	{
		for (int i = 0; i < lpMsg->btJewelCnt; i++)
		{
			PENTAGRAMJEWEL_INFO *lpPentagramInfo = (PENTAGRAMJEWEL_INFO *)(lpRecv + sizeof(PMSG_ANS_PENTAGRAMJEWEL) + (i * sizeof(PENTAGRAMJEWEL_INFO)));

			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btJewelPos = lpPentagramInfo->btJewelPos;
			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btJewelIndex = lpPentagramInfo->btJewelIndex;
			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btItemType = lpPentagramInfo->btItemType;
			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].wItemIndex = lpPentagramInfo->wItemIndex;
			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btMainAttribute = lpPentagramInfo->btMainAttribute;
			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btLevel = lpPentagramInfo->btLevel;
			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank1OptionNum = lpPentagramInfo->btRank1OptionNum;
			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank1Level = lpPentagramInfo->btRank1Level;
			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank2OptionNum = lpPentagramInfo->btRank2OptionNum;
			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank2Level = lpPentagramInfo->btRank2Level;
			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank3OptionNum = lpPentagramInfo->btRank3OptionNum;
			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank3Level = lpPentagramInfo->btRank3Level;
			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank4OptionNum = lpPentagramInfo->btRank4OptionNum;
			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank4Level = lpPentagramInfo->btRank4Level;
			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank5OptionNum = lpPentagramInfo->btRank5OptionNum;
			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank5Level = lpPentagramInfo->btRank5Level;
		}

		if (Obj.pntInventory[236]->IsItem() == TRUE)
		{
			this->CalcPentagramItem(Obj.m_Index, &Obj.pntInventory[236]);
			gGameProtocol.GCReFillSend(Obj.m_Index, Obj.MaxLife + Obj.AddLife, 0xFE, 0, Obj.iAddShield + Obj.iMaxShield);
			gGameProtocol.GCManaSend(Obj.m_Index, Obj.MaxMana + Obj.AddMana, 0xFE, 0, Obj.AddBP + Obj.MaxBP);
		}
	}

	this->GCPentagramJewelInfo(Obj.m_Index, lpMsg->iAnsType);
}

void CPentagramSystem::GCPentagramJewelInfo(CGameObject &Obj, int iJewelPos)
{
	

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	int dwSize = sizeof(PMSG_PENTAGRAMJEWEL_INFO);
	int dwJewelCnt = 0;

	BYTE Buff[4498]; // Fixed Size
	memset(&Buff, 0x00, sizeof(Buff));
	PMSG_PENTAGRAMJEWEL_INFO pMsg;
	PENTAGRAMJEWEL_INFO m_PentagramJewelInfo;

	if (iJewelPos == 1)
	{
		for (int i = 0; i < 254; i++)
		{
			if (Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btJewelPos == 1 &&
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btJewelIndex < 0xFF)
			{
				m_PentagramJewelInfo.btJewelPos = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btJewelPos;
				m_PentagramJewelInfo.btJewelIndex = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btJewelIndex;
				m_PentagramJewelInfo.btItemType = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btItemType;
				m_PentagramJewelInfo.wItemIndex = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].wItemIndex;
				m_PentagramJewelInfo.btMainAttribute = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btMainAttribute;
				m_PentagramJewelInfo.btLevel = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btLevel;
				m_PentagramJewelInfo.btRank1OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank1OptionNum;
				m_PentagramJewelInfo.btRank1Level = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank1Level;
				m_PentagramJewelInfo.btRank2OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank2OptionNum;
				m_PentagramJewelInfo.btRank2Level = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank2Level;
				m_PentagramJewelInfo.btRank3OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank3OptionNum;
				m_PentagramJewelInfo.btRank3Level = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank3Level;
				m_PentagramJewelInfo.btRank4OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank4OptionNum;
				m_PentagramJewelInfo.btRank4Level = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank4Level;
				m_PentagramJewelInfo.btRank5OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank5OptionNum;
				m_PentagramJewelInfo.btRank5Level = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank5Level;

				std::memcpy(&Buff[dwSize], &m_PentagramJewelInfo, sizeof(PENTAGRAMJEWEL_INFO));
				dwSize += sizeof(PENTAGRAMJEWEL_INFO);
				dwJewelCnt++;
			}
		}
	}

	else if (iJewelPos == 0)
	{
		for (int i = 0; i < 254; i++)
		{
			if (Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btJewelPos == 0 &&
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btJewelIndex < 0xFF)
			{
				m_PentagramJewelInfo.btJewelPos = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btJewelPos;
				m_PentagramJewelInfo.btJewelIndex = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btJewelIndex;
				m_PentagramJewelInfo.btItemType = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btItemType;
				m_PentagramJewelInfo.wItemIndex = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].wItemIndex;
				m_PentagramJewelInfo.btMainAttribute = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btMainAttribute;
				m_PentagramJewelInfo.btLevel = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btLevel;
				m_PentagramJewelInfo.btRank1OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank1OptionNum;
				m_PentagramJewelInfo.btRank1Level = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank1Level;
				m_PentagramJewelInfo.btRank2OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank2OptionNum;
				m_PentagramJewelInfo.btRank2Level = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank2Level;
				m_PentagramJewelInfo.btRank3OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank3OptionNum;
				m_PentagramJewelInfo.btRank3Level = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank3Level;
				m_PentagramJewelInfo.btRank4OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank4OptionNum;
				m_PentagramJewelInfo.btRank4Level = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank4Level;
				m_PentagramJewelInfo.btRank5OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank5OptionNum;
				m_PentagramJewelInfo.btRank5Level = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank5Level;

				std::memcpy(&Buff[dwSize], &m_PentagramJewelInfo, sizeof(PENTAGRAMJEWEL_INFO));
				dwSize += sizeof(PENTAGRAMJEWEL_INFO);
				dwJewelCnt++;
			}
		}
	}

	pMsg.btResult = dwJewelCnt > 0;
	pMsg.btJewelCnt = dwJewelCnt;
	pMsg.btJewelPos = iJewelPos;
	PHeadSubSetW((BYTE*)&pMsg, 0xEE, 0x01, dwSize);
	std::memcpy(&Buff, &pMsg, sizeof(pMsg));

	IOCP.DataSend(Obj.m_Index, Buff, dwSize);
}

#pragma pack(1)
struct PMSG_REQ_SETPENTAGRAMJEWEL
{
	PWMSG_HEAD h; // 4
	CGameObject &Obj; // 4
	int iUserGuid; // 4
	char szAccountID[MAX_ACCOUNT_LEN + 1]; // 11
	char szName[MAX_ACCOUNT_LEN + 1]; // 11
	BYTE btJewelPos; // 1
	BYTE btJewelCnt; // 1
};
#pragma pack ()

void CPentagramSystem::DBREQ_SetPentagramJewel(CGameObject &Obj, int iJewelPos)
{
	

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	PMSG_REQ_SETPENTAGRAMJEWEL pMsg;
	PENTAGRAMJEWEL_INFO m_PentagramJewelInfo;

	pMsg.iUserIndex = Obj.m_Index;
	pMsg.iUserGuid = Obj.DBNumber;

	std::memcpy(pMsg.szAccountID, Obj.AccountID, MAX_ACCOUNT_LEN + 1);
	std::memcpy(pMsg.szName, Obj.Name, MAX_ACCOUNT_LEN + 1);

	BYTE Buff[4448]; // Fixed size
	memset(&Buff, 0x00, sizeof(Buff));

	int dwSize = sizeof(PMSG_REQ_SETPENTAGRAMJEWEL);
	int JewelCnt = 0;

	if (iJewelPos == 1)
	{
		for (int i = 0; i < 254; i++)
		{
			if (Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btJewelPos == 1 &&
				Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btJewelIndex < 0xFF)
			{
				m_PentagramJewelInfo.btJewelPos = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btJewelPos;
				m_PentagramJewelInfo.btJewelIndex = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btJewelIndex;
				m_PentagramJewelInfo.btItemType = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btItemType;
				m_PentagramJewelInfo.wItemIndex = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].wItemIndex;
				m_PentagramJewelInfo.btMainAttribute = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btMainAttribute;
				m_PentagramJewelInfo.btLevel = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btLevel;
				m_PentagramJewelInfo.btRank1OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank1OptionNum;
				m_PentagramJewelInfo.btRank1Level = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank1Level;
				m_PentagramJewelInfo.btRank2OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank2OptionNum;
				m_PentagramJewelInfo.btRank2Level = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank2Level;
				m_PentagramJewelInfo.btRank3OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank3OptionNum;
				m_PentagramJewelInfo.btRank3Level = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank3Level;
				m_PentagramJewelInfo.btRank4OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank4OptionNum;
				m_PentagramJewelInfo.btRank4Level = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank4Level;
				m_PentagramJewelInfo.btRank5OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank5OptionNum;
				m_PentagramJewelInfo.btRank5Level = Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[i].btRank5Level;

				std::memcpy(&Buff[dwSize], &m_PentagramJewelInfo, sizeof(PENTAGRAMJEWEL_INFO));
				dwSize += sizeof(PENTAGRAMJEWEL_INFO);
				JewelCnt++;
			}
		}

		for (int i = 0; i < WAREHOUSE_SIZE; i++)
		{
			if (Obj.pntWarehouse[i]->IsItem() == TRUE &&
				this->IsPentagramItem(&Obj.pntWarehouse[i]))
			{
				BYTE ExOption[MAX_EXOPTION_SIZE];
				ItemIsBufExOption(ExOption, &Obj.pntWarehouse[i]);

				for (int j = 0; j < 5; j++)
				{
					if (Obj.pntWarehouse[i]->m_SocketOption[j] < 0xFE)
					{
						for (int k = 0; k < 254; k++)
						{
							if (Obj.m_PlayerData->m_PentagramJewelInfo_Warehouse[k].btJewelIndex == Obj.pntWarehouse[i]->m_SocketOption[j])
							{

							}
						}
					}
				}
			}
		}
	}

	else if (iJewelPos == 0)
	{
		for (int i = 0; i < 254; i++)
		{
			if (Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btJewelPos == 0 &&
				Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btJewelIndex < 0xFF)
			{
				m_PentagramJewelInfo.btJewelPos = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btJewelPos;
				m_PentagramJewelInfo.btJewelIndex = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btJewelIndex;
				m_PentagramJewelInfo.btItemType = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btItemType;
				m_PentagramJewelInfo.wItemIndex = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].wItemIndex;
				m_PentagramJewelInfo.btMainAttribute = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btMainAttribute;
				m_PentagramJewelInfo.btLevel = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btLevel;
				m_PentagramJewelInfo.btRank1OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank1OptionNum;
				m_PentagramJewelInfo.btRank1Level = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank1Level;
				m_PentagramJewelInfo.btRank2OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank2OptionNum;
				m_PentagramJewelInfo.btRank2Level = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank2Level;
				m_PentagramJewelInfo.btRank3OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank3OptionNum;
				m_PentagramJewelInfo.btRank3Level = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank3Level;
				m_PentagramJewelInfo.btRank4OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank4OptionNum;
				m_PentagramJewelInfo.btRank4Level = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank4Level;
				m_PentagramJewelInfo.btRank5OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank5OptionNum;
				m_PentagramJewelInfo.btRank5Level = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank5Level;

				std::memcpy(&Buff[dwSize], &m_PentagramJewelInfo, sizeof(PENTAGRAMJEWEL_INFO));
				dwSize += sizeof(PENTAGRAMJEWEL_INFO);
				JewelCnt++;
			}
		}

		for (int i = 0; i < INVENTORY_SIZE; i++)
		{
			if (Obj.pntInventory[i]->IsItem() == TRUE &&
				this->IsPentagramItem(&Obj.pntInventory[i]))
			{
				BYTE ExOption[MAX_EXOPTION_SIZE];
				ItemIsBufExOption(ExOption, &Obj.pntInventory[i]);

				for (int j = 0; j < 5; j++)
				{
					if (Obj.pntInventory[i]->m_SocketOption[j] < 0xFE)
					{
						for (int k = 0; k < 254; k++)
						{
							if (Obj.m_PlayerData->m_PentagramJewelInfo_Inven[k].btJewelIndex == Obj.pntInventory[i]->m_SocketOption[j])
							{

							}
						}
					}
				}
			}
		}
	}

	if (JewelCnt > 0)
	{
		pMsg.btJewelCnt = JewelCnt;
		pMsg.btJewelPos = iJewelPos;
		PHeadSetW((BYTE*)&pMsg, 0xE1, dwSize);
		std::memcpy(&Buff, &pMsg, sizeof(pMsg));
		wsDataCli.DataSend((char *)Buff, dwSize);
	}
}

struct PMSG_DEL_PENTAGRAMJEWEL
{
	PBMSG_HEAD2 h;
	int iUserGuid;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
	BYTE btJewelPos;
	BYTE btJewelIndex;
};

void CPentagramSystem::DBREQ_DelPentagramJewel(CGameObject &Obj, int iJewelPos, int iJewelIndex)
{
	PMSG_DEL_PENTAGRAMJEWEL pMsg;

	pMsg.iUserGuid = Obj.DBNumber;
	std::memcpy(&pMsg.szAccountID, Obj.AccountID, MAX_ACCOUNT_LEN + 1);
	std::memcpy(&pMsg.szName, Obj.Name, MAX_ACCOUNT_LEN + 1);
	pMsg.btJewelPos = iJewelPos;
	pMsg.btJewelIndex = iJewelIndex;

	PHeadSetB((BYTE*)&pMsg, 0xE2, sizeof(pMsg));
	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

struct PMSG_INSERT_PENTAGRAMJEWEL
{
	PBMSG_HEAD2 h;
	int iUserGuid;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
	BYTE btJewelPos;
	BYTE btJewelIndex;
	BYTE btMainAttribute;
	BYTE btItemType;
	WORD wItemIndex;
	BYTE btLevel;
	BYTE btRank1OptionNum;
	BYTE btRank1Level;
	BYTE btRank2OptionNum;
	BYTE btRank2Level;
	BYTE btRank3OptionNum;
	BYTE btRank3Level;
	BYTE btRank4OptionNum;
	BYTE btRank4Level;
	BYTE btRank5OptionNum;
	BYTE btRank5Level;
};

void CPentagramSystem::DBREQ_InsertPentagramJewel(CGameObject &Obj, int iJewelPos, int iJewelIndex, int iItemType, int iItemIndex, int iMainAttribute, int iJewelLevel, BYTE btRank1, BYTE btRank1Level, BYTE btRank2, BYTE btRank2Level, BYTE btRank3, BYTE btRank3Level, BYTE btRank4, BYTE btRank4Level, BYTE btRank5, BYTE btRank5Level)
{
	PMSG_INSERT_PENTAGRAMJEWEL pMsg;

	pMsg.iUserGuid = Obj.DBNumber;
	std::memcpy(&pMsg.szAccountID, Obj.AccountID, MAX_ACCOUNT_LEN + 1);
	std::memcpy(&pMsg.szName, Obj.Name, MAX_ACCOUNT_LEN + 1);

	pMsg.btJewelPos = iJewelPos;
	pMsg.btJewelIndex = iJewelIndex;
	pMsg.btItemType = iItemType;
	pMsg.wItemIndex = iItemIndex;
	pMsg.btMainAttribute = iMainAttribute;
	pMsg.btLevel = iJewelLevel;
	pMsg.btRank1OptionNum = btRank1;
	pMsg.btRank1Level = btRank1Level;
	pMsg.btRank2OptionNum = btRank2;
	pMsg.btRank2Level = btRank2Level;
	pMsg.btRank3OptionNum = btRank3;
	pMsg.btRank3Level = btRank3Level;
	pMsg.btRank4OptionNum = btRank4;
	pMsg.btRank4Level = btRank4Level;
	pMsg.btRank5OptionNum = btRank5;
	pMsg.btRank5Level = btRank5Level;

	PHeadSetB((BYTE*)&pMsg, 0xE3, sizeof(pMsg));
	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
}

BOOL CPentagramSystem::PentagramJewel_IN(CGameObject &Obj, int iPentagramItemPos, int iJewelItemPos)
{
	if (!gObjIsConnected(Obj.m_Index))
	{
		return FALSE;
	}

	

	if (Obj.Type != OBJ_USER)
	{
		return FALSE;
	}

	BYTE iDbJewelIndex = 0xFF;

	if (Obj.pntInventory[iPentagramItemPos]->IsItem() == FALSE || Obj.pntInventory[iJewelItemPos]->IsItem() == FALSE)
	{
		sLog->outBasic("[PentagramJewel_IN] PentagramItem Is Not Exist [%s][%s]",
			Obj.AccountID, Obj.Name);
		return FALSE;
	}

	if (this->IsPentagramItem(&Obj.pntInventory[iPentagramItemPos]) == FALSE)
	{
		return FALSE;
	}

	if (this->IsPentagramJewel(&Obj.pntInventory[iJewelItemPos]) == FALSE)
	{
		return FALSE;
	}

	BYTE btItemType = ITEM_GET_TYPE(Obj.pntInventory[iJewelItemPos]->m_Type);
	WORD wItemIndex = ITEM_GET_INDEX(Obj.pntInventory[iJewelItemPos]->m_Type);
	BYTE btMainAttribute = Obj.pntInventory[iJewelItemPos]->m_BonusSocketOption;
	BYTE iJewelKind = 0xFF;

	if (wItemIndex < 231)
	{
		iJewelKind = 0;
	}

	else if (wItemIndex >= 231 && wItemIndex < 240)
	{
		iJewelKind = 1;
	}

	else if (wItemIndex >= 241 && wItemIndex < 250)
	{
		iJewelKind = 2;
	}

	else if (wItemIndex >= 251 && wItemIndex < 260)
	{
		iJewelKind = 3;
	}

	else if (wItemIndex >= 261 && wItemIndex < 270)
	{
		iJewelKind = 4;
	}

	if (iJewelKind == 0xFF)
	{
		return FALSE;
	}

	if (Obj.pntInventory[iPentagramItemPos]->m_SocketOption[iJewelKind] != 0xFE)
	{
		sLog->outBasic("[PentagramJewel_IN] PentagramItem Socket is Not Empty [%s][%s]",Obj.AccountID, Obj.Name);
		return FALSE;
	}

	for (int index = 0; index < 254; index++)
	{
		bool bIndexExist = false;

		for (int i = 0; i < 254; i++)
		{
			if (Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btJewelIndex != 0xFF && Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btJewelIndex == index)
			{
				bIndexExist = true;
			}
		}

		if (!bIndexExist)
		{
			iDbJewelIndex = index;
			break;
		}
	}

	if (iDbJewelIndex == 0xFF)
	{
		return FALSE;
	}

	Obj.pntInventory[iPentagramItemPos]->m_SocketOption[iJewelKind] = iDbJewelIndex;

	BYTE btJewelPos = 0;
	BYTE btJewelIndex = iDbJewelIndex;
	BYTE btJewelLevel = Obj.pntInventory[iJewelItemPos]->m_Level;
	BYTE btRank1OptionNum = Obj.pntInventory[iJewelItemPos]->m_SocketOption[0] & 0x0F;
	BYTE btRank1Level = (Obj.pntInventory[iJewelItemPos]->m_SocketOption[0] & 0xF0) >> 4;
	BYTE btRank2OptionNum = Obj.pntInventory[iJewelItemPos]->m_SocketOption[1] & 0x0F;
	BYTE btRank2Level = (Obj.pntInventory[iJewelItemPos]->m_SocketOption[1] & 0xF0) >> 4;
	BYTE btRank3OptionNum = Obj.pntInventory[iJewelItemPos]->m_SocketOption[2] & 0x0F;
	BYTE btRank3Level = (Obj.pntInventory[iJewelItemPos]->m_SocketOption[2] & 0xF0) >> 4;
	BYTE btRank4OptionNum = Obj.pntInventory[iJewelItemPos]->m_SocketOption[3] & 0x0F;
	BYTE btRank4Level = (Obj.pntInventory[iJewelItemPos]->m_SocketOption[3] & 0xF0) >> 4;
	BYTE btRank5OptionNum = Obj.pntInventory[iJewelItemPos]->m_SocketOption[4] & 0x0F;
	BYTE btRank5Level = (Obj.pntInventory[iJewelItemPos]->m_SocketOption[4] & 0xF0) >> 4;

	for (int j = 0; j < 254; j++)
	{
		if (Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btJewelIndex == 0xFF)
		{
			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btJewelPos = 0;
			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btJewelIndex = iDbJewelIndex;
			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btItemType = btItemType;
			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].wItemIndex = wItemIndex;
			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btMainAttribute = btMainAttribute;
			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btLevel = btJewelLevel;
			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank1OptionNum = btRank1OptionNum;
			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank1Level = btRank1Level;
			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank2OptionNum = btRank2OptionNum;
			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank2Level = btRank2Level;
			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank3OptionNum = btRank3OptionNum;
			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank3Level = btRank3Level;
			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank4OptionNum = btRank4OptionNum;
			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank4Level = btRank4Level;
			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank5OptionNum = btRank5OptionNum;
			Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank5Level = btRank5Level;
			break;
		}
	}

	this->DBREQ_InsertPentagramJewel(Obj.m_Index, btJewelPos, btJewelIndex, btItemType, wItemIndex, btMainAttribute, btJewelLevel,
		btRank1OptionNum, btRank1Level, btRank2OptionNum, btRank2Level, btRank3OptionNum, btRank3Level, btRank4OptionNum, btRank4Level, btRank5OptionNum, btRank5Level);

	gObjInventoryDeleteItem(Obj.m_Index, iJewelItemPos);
	gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, iJewelItemPos, 0);
	gGameProtocol.GCAnsInJewelPentagramItem(Obj.m_Index, 1, btJewelPos, btJewelIndex, btItemType, wItemIndex, btMainAttribute, btJewelLevel, btRank1OptionNum, btRank1Level, btRank2OptionNum, btRank2Level, btRank3OptionNum, btRank3Level, btRank4OptionNum, btRank4Level, btRank5OptionNum, btRank5Level);
	gGameProtocol.GCInventoryItemOneSend(Obj.m_Index, iPentagramItemPos);
	BYTE btInOutResult = TRUE;
	gGameProtocol.GCAnsPentagramJewelInOut(Obj.m_Index, 1);

	BYTE ExOption[MAX_EXOPTION_SIZE];
	ItemIsBufExOption(ExOption, &Obj.pntInventory[iPentagramItemPos]);

	return TRUE;
}

BOOL CPentagramSystem::PentagramJewel_OUT(CGameObject &Obj, int iPentagramItemPos, BYTE btSocketIndex, BYTE *btJewelPos, BYTE *btJewelDBIndex)
{
	if (!gObjIsConnected(Obj.m_Index))
	{
		return FALSE;
	}

	

	if (Obj.Type != OBJ_USER)
	{
		return FALSE;
	}

	if (Obj.pntInventory[iPentagramItemPos]->IsItem() == FALSE)
	{
		return FALSE;
	}

	if (this->IsPentagramItem(&Obj.pntInventory[iPentagramItemPos]) == FALSE)
	{
		return FALSE;
	}

	if (btSocketIndex < 0 || btSocketIndex > 4) // additional check
	{
		return FALSE;
	}

	BYTE iJewelDBIndex = Obj.pntInventory[iPentagramItemPos]->m_SocketOption[btSocketIndex];

	if (iJewelDBIndex >= 0xFE)
	{
		return FALSE;
	}

	int iJewelRankCount = 0;
	BYTE iJewelLevel = 0xFF;
	int iOutRate = 0;
	int iReturnValue = 0;
	BYTE iItemType = 0xFF;
	WORD iItemIndex = 0xFFFF;

	for (int i = 0; i < 254; i++)
	{
		if (Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btJewelIndex == iJewelDBIndex)
		{
			if ((Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank1OptionNum & 0x0F) != 0x0F)
				iJewelRankCount++;
			if ((Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank2OptionNum & 0x0F) != 0x0F)
				iJewelRankCount++;
			if ((Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank3OptionNum & 0x0F) != 0x0F)
				iJewelRankCount++;
			if ((Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank4OptionNum & 0x0F) != 0x0F)
				iJewelRankCount++;
			if ((Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank5OptionNum & 0x0F) != 0x0F)
				iJewelRankCount++;

			iJewelLevel = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btLevel;
			iItemType = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btItemType;
			iItemIndex = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].wItemIndex;

			for (int j = 0; j < 55; j++)
			{
				if (this->m_JewelOutRate[j].JewelRank == iJewelRankCount &&
					this->m_JewelOutRate[j].JewelLevel == iJewelLevel)
				{
					iOutRate = this->m_JewelOutRate[j].OutRate;
					break;
				}
			}

			if (!iOutRate)
				return FALSE;

			BYTE btJewelPos = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btJewelPos;
			BYTE btJewelDBIndex = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btJewelIndex;

			if ((rand() % 10000) >= iOutRate)
			{
				sLog->outBasic("[PentagramJewel_OUT][%s][%s] PentagramJewel Out Fail - JewelDBIndex = %d, ItemType = %d, ItemIndex = %d",
					Obj.AccountID, Obj.Name, btJewelDBIndex, iItemType, iItemIndex);

				iReturnValue = 2;
			}

			else
			{
				int iResultItemCode = ITEMGET(Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btItemType, Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].wItemIndex);
				int iItemDurability = ItemGetDurability(iResultItemCode, iJewelLevel, 0, 0);
				if (!CheckInventoryEmptySpace(lpObj, ItemAttribute[iResultItemCode].Height, ItemAttribute[iResultItemCode].Width))
					return 0x64;

				BYTE btSocketOption[5];
				btSocketOption[0] = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank1OptionNum | 0x10 * Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank1Level;
				btSocketOption[1] = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank2OptionNum | 0x10 * Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank2Level;
				btSocketOption[2] = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank3OptionNum | 0x10 * Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank3Level;
				btSocketOption[3] = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank4OptionNum | 0x10 * Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank4Level;
				btSocketOption[4] = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank5OptionNum | 0x10 * Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btRank5Level;

				ItemCreate(Obj.m_Index, 231, 0, 0, iResultItemCode, iJewelLevel, iItemDurability, 0, 0, 0, 0, 0, 0, 0, btSocketOption, Obj.m_PlayerData->m_PentagramJewelInfo_Inven[i].btMainAttribute);
				sLog->outBasic("[PentagramJewel_OUT][%s][%s] PentagramJewel Out Success - JewelDBIndex = %d, ItemType = %d, ItemIndex = %d",
					Obj.AccountID, Obj.Name, iJewelDBIndex, iItemType, iItemIndex);
			}

			Obj.pntInventory[iPentagramItemPos]->m_SocketOption[btSocketIndex] = 0xFE;
			this->DelPentagramJewelInfo(Obj.m_Index, 0, iJewelDBIndex);
			gGameProtocol.GCInventoryItemOneSend(Obj.m_Index, iPentagramItemPos);
			return iReturnValue;
		}
	}

	return iReturnValue;
}

bool CPentagramSystem::GCTransPentagramJewelViewInfo(CGameObject &Obj, CItemObject *lpItemData)
{
	PMSG_PENTAGRAMJEWEL_INFO pMsg;
	PENTAGRAMJEWEL_INFO m_PentagramJewelInfo;
	int dwSize = 8;
	int iJewelCount = 0;

	BYTE Buff[4096];
	memset(&Buff, 0x00, sizeof(Buff));

	

	if (Obj.Type != OBJ_USER)
	{
		return false;
	}

	int TargetIndex = Obj.TargetNumber;
	CGameObject lpTargetObj = getGameObject(TargetIndex);

	if (lpTargetObj.Type != OBJ_USER)
	{
		return false;
	}

	if (lpItemData->IsItem() == FALSE)
	{
		return false;
	}

	if (this->IsPentagramItem(lpItemData) == false)
	{
		return false;
	}

	for (int i = 0; i < 5; i++)
	{
		if (lpItemData->m_SocketOption[i] < 0xFE)
		{
			for (int j = 0; j < 254; j++)
			{
				if (Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btJewelIndex == lpItemData->m_SocketOption[i])
				{
					m_PentagramJewelInfo.btJewelPos = 2;
					m_PentagramJewelInfo.btJewelIndex = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btJewelIndex;
					m_PentagramJewelInfo.btItemType = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btItemType;
					m_PentagramJewelInfo.wItemIndex = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].wItemIndex;
					m_PentagramJewelInfo.btMainAttribute = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btMainAttribute;
					m_PentagramJewelInfo.btLevel = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btLevel;
					m_PentagramJewelInfo.btRank1OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank1OptionNum;
					m_PentagramJewelInfo.btRank1Level = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank1Level;
					m_PentagramJewelInfo.btRank2OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank2OptionNum;
					m_PentagramJewelInfo.btRank2Level = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank2Level;
					m_PentagramJewelInfo.btRank3OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank3OptionNum;
					m_PentagramJewelInfo.btRank3Level = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank3Level;
					m_PentagramJewelInfo.btRank4OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank4OptionNum;
					m_PentagramJewelInfo.btRank4Level = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank4Level;
					m_PentagramJewelInfo.btRank5OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank5OptionNum;
					m_PentagramJewelInfo.btRank5Level = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btRank5Level;
					std::memcpy(&Buff[dwSize], &m_PentagramJewelInfo, sizeof(m_PentagramJewelInfo));
					dwSize += sizeof(m_PentagramJewelInfo);
					iJewelCount++;
					break;
				}
			}
		}
	}

	if (iJewelCount > 0)
	{
		pMsg.btResult = TRUE;
		pMsg.btJewelCnt = iJewelCount;
		pMsg.btJewelPos = 2;
		PHeadSubSetW((BYTE*)&pMsg, 0xEE, 0x01, dwSize);
		std::memcpy(&Buff, &pMsg, sizeof(pMsg));

		IOCP.DataSend(TargetIndex, Buff, dwSize);

		return true;
	}

	return false;
}

bool CPentagramSystem::GCPShopPentagramJewelViewInfo(CGameObject &Obj, int aSourceIndex)
{
	PMSG_PENTAGRAMJEWEL_INFO pMsg;
	PENTAGRAMJEWEL_INFO m_PentagramJewelInfo;
	int dwSize = 8;
	int iJewelCount = 0;

	BYTE Buff[4096];
	memset(&Buff, 0x00, sizeof(Buff));

	

	if (Obj.Type != OBJ_USER)
	{
		return false;
	}

	CGameObject lpTargetObj = getGameObject(aSourceIndex);

	if (lpTargetObj.Type != OBJ_USER)
	{
		return false;
	}

	for (int i = MAIN_INVENTORY_SIZE; i < INVENTORY_SIZE; i++)
	{
		if (Obj.pntInventory[i]->IsItem() == TRUE && this->IsPentagramItem(&Obj.pntInventory[i]) == true)
		{
			for (int j = 0; j < 5; j++)
			{
				if (Obj.pntInventory[i]->m_SocketOption[j] < 0xFE)
				{
					for (int k = 0; k < 254; k++)
					{
						if (Obj.m_PlayerData->m_PentagramJewelInfo_Inven[k].btJewelIndex == Obj.pntInventory[i]->m_SocketOption[j])
						{
							m_PentagramJewelInfo.btJewelPos = 3;
							m_PentagramJewelInfo.btJewelIndex = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[k].btJewelIndex;
							m_PentagramJewelInfo.btItemType = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[k].btItemType;
							m_PentagramJewelInfo.wItemIndex = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[k].wItemIndex;
							m_PentagramJewelInfo.btMainAttribute = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[k].btMainAttribute;
							m_PentagramJewelInfo.btLevel = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[k].btLevel;
							m_PentagramJewelInfo.btRank1OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[k].btRank1OptionNum;
							m_PentagramJewelInfo.btRank1Level = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[k].btRank1Level;
							m_PentagramJewelInfo.btRank2OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[k].btRank2OptionNum;
							m_PentagramJewelInfo.btRank2Level = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[k].btRank2Level;
							m_PentagramJewelInfo.btRank3OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[k].btRank3OptionNum;
							m_PentagramJewelInfo.btRank3Level = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[k].btRank3Level;
							m_PentagramJewelInfo.btRank4OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[k].btRank4OptionNum;
							m_PentagramJewelInfo.btRank4Level = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[k].btRank4Level;
							m_PentagramJewelInfo.btRank5OptionNum = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[k].btRank5OptionNum;
							m_PentagramJewelInfo.btRank5Level = Obj.m_PlayerData->m_PentagramJewelInfo_Inven[k].btRank5Level;
							std::memcpy(&Buff[dwSize], &m_PentagramJewelInfo, sizeof(m_PentagramJewelInfo));
							dwSize += sizeof(m_PentagramJewelInfo);
							iJewelCount++;
							break;
						}
					}
				}
			}
		}
	}

	if (iJewelCount > 0)
	{
		pMsg.btResult = TRUE;
		pMsg.btJewelCnt = iJewelCount;
		pMsg.btJewelPos = 3;
		PHeadSubSetW((BYTE*)&pMsg, 0xEE, 0x01, dwSize);
		std::memcpy(&Buff, &pMsg, sizeof(pMsg));

		IOCP.DataSend(aSourceIndex, Buff, dwSize);

		return true;
	}

	return false;
}

BOOL CPentagramSystem::IsEnableToTradePentagramItem(CGameObject &Obj)
{
	for (int i = 0; i < TRADE_BOX_SIZE; i++)
	{
		if (Obj.pntTrade[i]->IsItem() == TRUE && this->IsPentagramItem(&Obj.pntTrade[i]) == true)
		{
			if (Obj.pntTrade[i]->m_Durability < 1.0)
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL CPentagramSystem::IsEnableTransPentagramJewelInfo(CGameObject &Obj, int targetIndex)
{
	BOOL bRet = TRUE;
	BYTE iTargetDBJewelIndex = 0xFF;

	
	CGameObject lpTargetObj = getGameObject(targetIndex);

	for (int n = 0; n < TRADE_BOX_SIZE; n++)
	{
		if (Obj.pntTrade[n]->IsItem() == TRUE && this->IsPentagramItem(&Obj.pntTrade[n]) == true)
		{
			for (int i = 0; i < 5; i++)
			{
				if (Obj.pntTrade[n]->m_SocketOption[i] < 0xFE)
				{
					for (int index = 0; index < 254; index++)
					{
						BOOL bIndexExist = FALSE;

						for (int j = 0; j < 254; j++)
						{
							if (lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btJewelIndex != 0xFF)
							{
								if (lpTargetObj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btJewelIndex == index)
								{
									bIndexExist = TRUE;
								}
							}
						}

						if (!bIndexExist)
						{
							iTargetDBJewelIndex = index;
							break;
						}
					}

					if (iTargetDBJewelIndex == 0xFF)
					{
						sLog->outBasic("[IsEnableTransPentagramJewelInfo][%s][%s] PentagramJewelIndex Area Not Exist!!",
							lpTargetObj.AccountID, lpTargetObj.Name);

						bRet = FALSE;
					}
				}
			}
		}

		if (lpTargetObj.pntTrade[n]->IsItem() == TRUE && this->IsPentagramItem(&lpTargetObj.pntTrade[n]) == true)
		{
			for (int i = 0; i < 5; i++)
			{
				if (lpTargetObj.pntTrade[n]->m_SocketOption[i] < 0xFE)
				{
					for (int index = 0; index < 254; index++)
					{
						BOOL bIndexExist = FALSE;

						for (int j = 0; j < 254; j++)
						{
							if (Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btJewelIndex != 0xFF)
							{
								if (Obj.m_PlayerData->m_PentagramJewelInfo_Inven[j].btJewelIndex == index)
								{
									bIndexExist = TRUE;
								}
							}
						}

						if (!bIndexExist)
						{
							iTargetDBJewelIndex = index;
							break;
						}
					}

					if (iTargetDBJewelIndex == 0xFF)
					{
						sLog->outBasic("[IsEnableTransPentagramJewelInfo][%s][%s] PentagramJewelIndex Area Not Exist!!",
							Obj.AccountID, Obj.Name);

						bRet = FALSE;
					}
				}
			}
		}
	}

	return bRet;
}

int CPentagramSystem::CheckOverlapMythrilPiece(CGameObject &Obj, int iItemType, int iMainAttribute)
{
	CGameObject* lpObj = Obj;

	for (int x = 0; x < MAIN_INVENTORY_SIZE; x++)
	{
		if (Obj.pntInventory[x]->IsItem() == TRUE &&
			Obj.pntInventory[x]->m_Type == iItemType &&
			(Obj.pntInventory[x]->m_BonusSocketOption & 0x0F) == iMainAttribute)
		{
			int iITEM_DUR = Obj.pntInventory[x]->m_Durability;

			if (iITEM_DUR >= 0 && iITEM_DUR <= IsOverlapItem(iItemType))
			{
				return x;
			}
		}
	}

	return -1;
}

BOOL CPentagramSystem::AddTradeCount(CGameObject lpObj, int source, int target)
{
	if (source < 0 || source > MAIN_INVENTORY_SIZE - 1)
	{
		return FALSE;
	}

	if (target < 0 || target > MAIN_INVENTORY_SIZE - 1)
	{
		return FALSE;
	}

	if (Obj.pntInventory[source]->IsItem() == FALSE)
	{
		return FALSE;
	}

	if (Obj.pntInventory[target]->IsItem() == FALSE)
	{
		return FALSE;
	}

	if (this->IsPentagramItem(Obj.pntInventory[target]->m_Type) == FALSE)
	{
		return FALSE;
	}

	if (Obj.pntInventory[target]->m_Durability >= 255.0)
	{
		return FALSE;
	}

	Obj.pntInventory[target]->m_Durability += 1.0;

	return TRUE;
}

void CPentagramSystem::LoadOptionMaps(const char* File)
{
	using namespace pugi;
	xml_document Document;
	xml_parse_result Result = Document.load_file(File);

	if (Result.status != status_ok)
	{
		sLog->outError("[IGC_PentagramDropMaps] File %s not found!", File);
		return;
	}

	xml_node CommonMapNonPK = Document.child("PentagramDropMap");

	for (pugi::xml_node map = CommonMapNonPK.child("map"); map; map = map.next_sibling())
	{
		int IndexX = map.attribute("ID").as_int(-1);
		Penta_DropRate[IndexX].ItemDropIndex = map.attribute("ItemIndex").as_int(0);
		Penta_DropRate[IndexX].ItemDropLevel = map.attribute("DropLevel").as_int(0);
		Penta_DropRate[IndexX].ItemDropRate = map.attribute("DropRate").as_int(0);
	}
}

void CPentagramSystem::LoadOptionNews()
{
	CIniReader PentaDropSlot(g_ConfigRead.GetPath("PentagramSystem\\IGC_PentagramRateDrop.ini"));

	this->m_SlotCountRate.InitPool();

	m_iSpiritMap_DropLevel = PentaDropSlot.ReadInt("PentagramSystemDrop", "SpiritMapFragment_DropLevel", 0);
	m_iSpiritMap_DropRate = PentaDropSlot.ReadInt("PentagramSystemDrop", "SpiritMapFragment_DropRate", 0);

	m_iMithril_DropRate = PentaDropSlot.ReadInt("PentagramSystemDrop", "MithrilFragment_DropRate", 0);

	m_iMurenBook_DropLevel = PentaDropSlot.ReadInt("PentagramSystemDrop", "MurenBook_DropLevel", 0);
	m_iMurenBook_DropRate = PentaDropSlot.ReadInt("PentagramSystemDrop", "MurenBook_DropRate", 0);

	m_iEtramuScroll_DropLevel = PentaDropSlot.ReadInt("PentagramSystemDrop", "EtramuScroll_DropLevel", 0);
	m_iEtramuScroll_DropRate = PentaDropSlot.ReadInt("PentagramSystemDrop", "EtramuScroll_DropRate", 0); 

	int tmpRate = PentaDropSlot.ReadInt("PentagramSystemDrop", "ElementSlotCountRate1", 0);
	m_SlotCountRate.AddValue(1, tmpRate);

	tmpRate = PentaDropSlot.ReadInt("PentagramSystemDrop", "ElementSlotCountRate2", 0); 
	m_SlotCountRate.AddValue(2, tmpRate);

	tmpRate = PentaDropSlot.ReadInt("PentagramSystemDrop", "ElementSlotCountRate3", 0);
	m_SlotCountRate.AddValue(3, tmpRate);

	tmpRate = PentaDropSlot.ReadInt("PentagramSystemDrop", "ElementSlotCountRate4", 0);
	m_SlotCountRate.AddValue(4, tmpRate);

	tmpRate = PentaDropSlot.ReadInt("PentagramSystemDrop", "ElementSlotCountRate5", 0); 
	m_SlotCountRate.AddValue(5, tmpRate);
}

BOOL CPentagramSystem::ElementDrop(CGameObject &Obj, CGameObject lpTargetObj)
{

	if (Obj.Level >= m_iSpiritMap_DropLevel && rand() % 10000 < m_iSpiritMap_DropRate)
	{
		int iMaxHitUser = gObjMonsterTopHitDamageUser(lpObj);
		ItemCreate(Obj.m_Index, Obj.MapNumber, Obj.X, Obj.Y, ITEMGET(13, 145), 0, 1,
			0, 0, 0, iMaxHitUser, 0, 0, 0, 0, 0);
		return true;
	}

	if (Obj.MapNumber == MAP_INDEX_ACHERON || Obj.MapNumber == MAP_INDEX_DEBENTER || Obj.MapNumber == MAP_INDEX_NARS || Obj.MapNumber == MAP_INDEX_URUK_MOUNTAIN)
	{
		for (int i = 0; i < 50; i++)
		{
			if (Obj.Level >= Penta_DropRate[i].ItemDropLevel && rand() % 10000 < Penta_DropRate[i].ItemDropRate)
			{
				int iMaxHitUser = gObjMonsterTopHitDamageUser(lpObj);
				BYTE SocketBonus = (Obj.m_iPentagramMainAttribute == ELEMENT_NONE) ? (1 + rand() % 5) : Obj.m_iPentagramMainAttribute;
				BYTE SocketOption[5];

				SocketOption[0] = -1;
				SocketOption[1] = -1;
				SocketOption[2] = -1;
				SocketOption[3] = -1;
				SocketOption[4] = -1;

				int scount = this->m_SlotCountRate.GetRandomValue(eRANDOMPOOL_BY_WEIGHT);
				int count = 0;

				do
				{
					int slot = rand() % 5;

					if (SocketOption[slot] == (BYTE)-1)
					{
						SocketOption[slot] = 0xFE;
						count++;
					}
				} while (scount != count);

				ItemCreate(Obj.m_Index, Obj.MapNumber, Obj.X, Obj.Y, ITEMGET(12, Penta_DropRate[i].ItemDropIndex), 0, 1,
					0, 0, 0, iMaxHitUser, 0, 0, 0, SocketOption, SocketBonus);
				return true;
			}
		}
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

