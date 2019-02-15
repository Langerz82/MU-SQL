////////////////////////////////////////////////////////////////////////////////
// ItemOptionTypeMng.cpp
#include "ItemOptionTypeMng.h"
#include "Logging/Log.h"
#include "FormulaParse.h"
#include "MasterLevelSkillTreeSystem.h"

CItemObjectOptionTypeMng g_ItemOptionTypeMng;

void CItemObjectOptionTypeMng::Initialize()
{
	for (int iClearIndex = 0; iClearIndex < MAX_EXCELLENT_COMMON_OPTIONS; iClearIndex++)
	{
		this->m_CommonExtOptionType[iClearIndex].Clear();
	}

	for (int iClearIndex = 0; iClearIndex < MAX_EXCELLENT_WING_OPTIONS; iClearIndex++)
	{
		this->m_WingExtOptionType[iClearIndex].Clear();
	}

	memset(this->m_ExcellentOptionRate.m_CommonOptionRate, 0, sizeof(this->m_ExcellentOptionRate));
	this->m_ExcellentOptionRate.m_vtWingOptionRate.clear();
}

bool CItemObjectOptionTypeMng::LoadScript(char *pchFileName)
{
	this->Initialize();

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(pchFileName);

	if (res.status != pugi::status_ok)
	{
		sLog->outError("Failed to load %s file (%s)", pchFileName, res.description());
		return false;
	}

	pugi::xml_node mainXML = file.child("ExcellentOptions");
	pugi::xml_node common = mainXML.child("Common");
	int OptionCount = 0;

	for (pugi::xml_node option = common.child("Option"); option; option = option.next_sibling())
	{
		this->m_CommonExtOptionType[OptionCount].OptionID = option.attribute("ID").as_int();
		this->m_CommonExtOptionType[OptionCount].OptionNumber = option.attribute("Number").as_int();
		this->m_CommonExtOptionType[OptionCount].Operator = option.attribute("Operator").as_int();
		this->m_CommonExtOptionType[OptionCount].OptionValue = option.attribute("Value").as_int();
		this->m_CommonExtOptionType[OptionCount].FormulaID = option.attribute("FormulaID").as_int();
		this->m_CommonExtOptionType[OptionCount].ItemKindA_1 = option.attribute("ItemKindA_1").as_int();
		this->m_CommonExtOptionType[OptionCount].ItemKindA_2 = option.attribute("ItemKindA_2").as_int();
		this->m_CommonExtOptionType[OptionCount].ItemKindA_3 = option.attribute("ItemKindA_3").as_int();
		this->m_CommonExtOptionType[OptionCount].OptionRate = option.attribute("Rate").as_int();
		strcpy(this->m_CommonExtOptionType[OptionCount].OptionName, option.attribute("Name").as_string());

		OptionCount++;
	}

	pugi::xml_node common_effect = mainXML.child("CommonOptionEffect");

	for (pugi::xml_node option = common_effect.child("Option"); option; option = option.next_sibling())
	{
		int iOptionID = option.attribute("ID").as_int();

		if (iOptionID < 0 || iOptionID >= MAX_EXCELLENT_COMMON_OPTIONS)
		{
			continue;
		}

		this->m_CommonExtOptionType[iOptionID].OptionEffect[0] = option.attribute("Etc").as_int();
		this->m_CommonExtOptionType[iOptionID].OptionEffect[1] = option.attribute("Rate").as_int();
		this->m_CommonExtOptionType[iOptionID].OptionEffect[2] = option.attribute("Attack").as_int();
		this->m_CommonExtOptionType[iOptionID].OptionEffect[3] = option.attribute("Defense").as_int();
		this->m_CommonExtOptionType[iOptionID].OptionEffect[4] = option.attribute("Life").as_int();
		this->m_CommonExtOptionType[iOptionID].OptionEffect[5] = option.attribute("Mana").as_int();
		this->m_CommonExtOptionType[iOptionID].OptionEffect[6] = option.attribute("Skill").as_int();
	}

	pugi::xml_node wings = mainXML.child("Wings");
	OptionCount = 0;

	for (pugi::xml_node option = wings.child("Option"); option; option = option.next_sibling())
	{
		this->m_WingExtOptionType[OptionCount].OptionID = option.attribute("ID").as_int();
		this->m_WingExtOptionType[OptionCount].ItemKindA = option.attribute("ItemKindA").as_int();
		this->m_WingExtOptionType[OptionCount].ItemKindB = option.attribute("ItemKindB").as_int();
		this->m_WingExtOptionType[OptionCount].OptionNumber = option.attribute("Number").as_int();
		this->m_WingExtOptionType[OptionCount].Operator = option.attribute("Operator").as_int();
		this->m_WingExtOptionType[OptionCount].FormulaID = option.attribute("FormulaID").as_int();
		this->m_WingExtOptionType[OptionCount].OptionRate = option.attribute("Rate").as_int();
		this->m_WingExtOptionType[OptionCount].OptionValue = option.attribute("Value").as_int();
		strcpy(this->m_WingExtOptionType[OptionCount].OptionName, option.attribute("Name").as_string());

		OptionCount++;
	}

	pugi::xml_node wing_effect = mainXML.child("WingOptionEffect");

	for (pugi::xml_node option = wing_effect.child("Option"); option; option = option.next_sibling())
	{
		int iOptionID = option.attribute("ID").as_int();

		if (iOptionID < 0 || iOptionID >= MAX_EXCELLENT_WING_OPTIONS)
		{
			continue;
		}

		this->m_WingExtOptionType[iOptionID].OptionEffect[0] = option.attribute("Etc").as_int();
		this->m_WingExtOptionType[iOptionID].OptionEffect[1] = option.attribute("Rate").as_int();
		this->m_WingExtOptionType[iOptionID].OptionEffect[2] = option.attribute("Attack").as_int();
		this->m_WingExtOptionType[iOptionID].OptionEffect[3] = option.attribute("Defense").as_int();
		this->m_WingExtOptionType[iOptionID].OptionEffect[4] = option.attribute("Life").as_int();
		this->m_WingExtOptionType[iOptionID].OptionEffect[5] = option.attribute("Mana").as_int();
		this->m_WingExtOptionType[iOptionID].OptionEffect[6] = option.attribute("Skill").as_int();
	}

	pugi::xml_node optionrate = mainXML.child("OptionDropRate");
	pugi::xml_node commonrate = optionrate.child("Common");

	this->m_ExcellentOptionRate.m_CommonOptionRate[0] = commonrate.attribute("One").as_int();
	this->m_ExcellentOptionRate.m_CommonOptionRate[1] = commonrate.attribute("Two").as_int();
	this->m_ExcellentOptionRate.m_CommonOptionRate[2] = commonrate.attribute("Three").as_int();
	this->m_ExcellentOptionRate.m_CommonOptionRate[3] = commonrate.attribute("Four").as_int();
	this->m_ExcellentOptionRate.m_CommonOptionRate[4] = commonrate.attribute("Five").as_int();
	this->m_ExcellentOptionRate.m_CommonOptionRate[5] = commonrate.attribute("Six").as_int();
	this->m_ExcellentOptionRate.m_CommonOptionRate[6] = commonrate.attribute("Seven").as_int();
	this->m_ExcellentOptionRate.m_CommonOptionRate[7] = commonrate.attribute("Eight").as_int();
	this->m_ExcellentOptionRate.m_CommonOptionRate[8] = commonrate.attribute("Nine").as_int();

	pugi::xml_node wing = optionrate.child("Wing");

	for (pugi::xml_node kind = wing.child("Kind"); kind; kind = kind.next_sibling())
	{
		WING_EXC_OPTION_RATE m_WingRate;

		m_WingRate.ItemKindA = kind.attribute("A").as_int();
		m_WingRate.ItemKindB = kind.attribute("B").as_int();

		m_WingRate.WingOptionRate[0] = kind.attribute("One").as_int();
		m_WingRate.WingOptionRate[1] = kind.attribute("Two").as_int();
		m_WingRate.WingOptionRate[2] = kind.attribute("Three").as_int();
		m_WingRate.WingOptionRate[3] = kind.attribute("Four").as_int();
		m_WingRate.WingOptionRate[4] = kind.attribute("Five").as_int();
		m_WingRate.WingOptionRate[5] = kind.attribute("Six").as_int();
		m_WingRate.WingOptionRate[6] = kind.attribute("Seven").as_int();
		m_WingRate.WingOptionRate[7] = kind.attribute("Eight").as_int();
		m_WingRate.WingOptionRate[8] = kind.attribute("Nine").as_int();

		this->m_ExcellentOptionRate.m_vtWingOptionRate.push_back(m_WingRate);
	}
}

void CItemObjectOptionTypeMng::Initialize_AccessoryItemOption()
{
	for (int iClearIndex = 0; iClearIndex < 6; iClearIndex++)
	{
		this->m_AccessoryItemOptionValue[iClearIndex].Clear();
	}
}

bool CItemObjectOptionTypeMng::LoadAccessoryItemOptionScript(char *pchFileName)
{
	ACCESSORY_ITEM_OPTION_VALUE AccessoryItemOptionValue; 
	int iValueCount = 0; 

	this->Initialize_AccessoryItemOption();

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(pchFileName);

	if (res.status != pugi::status_ok)
	{
		sLog->outError("[ItemOptionTypeMng] AccessoryItem Option Script file Load failed. (%s) (%s)", pchFileName, res.description());
		return false;
	}

	pugi::xml_node mainXML = file.child("AccessoryItems");

	for (pugi::xml_node item = mainXML.child("Item"); item; item = item.next_sibling())
	{
		memset((char *)&AccessoryItemOptionValue, 0, sizeof(AccessoryItemOptionValue));
			
		AccessoryItemOptionValue.ItemType = item.attribute("Cat").as_int();
		AccessoryItemOptionValue.ItemIndex = item.attribute("Index").as_int();

		for (int i = 0; i < 16; i++)
		{
			char szTemp[256];
			sprintf(szTemp, "OptValue%d", i);

			AccessoryItemOptionValue.LevelValue[i] = item.attribute(szTemp).as_int();
		}

		std::memcpy(&this->m_AccessoryItemOptionValue[iValueCount], &AccessoryItemOptionValue, sizeof(AccessoryItemOptionValue));
		iValueCount++;
	}

	return true;
}

int CItemObjectOptionTypeMng::CommonExcOptionRand(int ItemKindA, BYTE *btSocketExcOption)
{
	BYTE btExcOptions[9];
	DWORD dwExcOptionRate[9];
	int iExcOption = 0;
	for (int i = 0; i < 20; i++)
	{
		//	sLog->outError("[K2]1# ItemKindA %d %d %d [%d]", this->m_CommonExtOptionType[i].ItemKindA_1, this->m_CommonExtOptionType[i].ItemKindA_2, this->m_CommonExtOptionType[i].ItemKindA_3, ItemKindA);
		if (this->m_CommonExtOptionType[i].ItemKindA_1 == ItemKindA ||	this->m_CommonExtOptionType[i].ItemKindA_2 == ItemKindA ||
			this->m_CommonExtOptionType[i].ItemKindA_3 == ItemKindA)
		{
			btExcOptions[iExcOption] = this->m_CommonExtOptionType[i].OptionNumber;
			dwExcOptionRate[iExcOption] = this->m_CommonExtOptionType[i].OptionRate;
			iExcOption++;

			if (iExcOption >= 9)
			{
				break;
			}
		}
	}

	int OneOp = this->m_ExcellentOptionRate.m_CommonOptionRate[0];
	int TwoOp = this->m_ExcellentOptionRate.m_CommonOptionRate[1];
	int ThreeOp = this->m_ExcellentOptionRate.m_CommonOptionRate[2];
	int FourOp = this->m_ExcellentOptionRate.m_CommonOptionRate[3];
	int FiveOp = this->m_ExcellentOptionRate.m_CommonOptionRate[4];
	int SixOp = this->m_ExcellentOptionRate.m_CommonOptionRate[5];
	int SevenOp = this->m_ExcellentOptionRate.m_CommonOptionRate[6];
	int EightOp = this->m_ExcellentOptionRate.m_CommonOptionRate[7];
	int NineOp = this->m_ExcellentOptionRate.m_CommonOptionRate[8];
	int iRand = rand() % 10000;
	int iOptionCount = 0;

	if (iRand <= OneOp)
	{
		iOptionCount = 1;
	}

	else if (iRand <= (OneOp + TwoOp))
	{
		iOptionCount = 2;
	}

	else if (iRand <= (OneOp + TwoOp + ThreeOp))
	{
		iOptionCount = 3;
	}

	else if (iRand <= (OneOp + TwoOp + ThreeOp + FourOp))
	{
		iOptionCount = 4;
	}

	else if (iRand <= (OneOp + TwoOp + ThreeOp + FourOp + FiveOp))
	{
		iOptionCount = 5;
	}

	else if (iRand <= (OneOp + TwoOp + ThreeOp + FourOp + FiveOp + SixOp))
	{
		iOptionCount = 6;
	}

	else if (iRand <= (OneOp + TwoOp + ThreeOp + FourOp + FiveOp + SixOp + SevenOp))
	{
		iOptionCount = 7;
	}

	else if (iRand <= (OneOp + TwoOp + ThreeOp + FourOp + FiveOp + SixOp + SevenOp + EightOp))
	{
		iOptionCount = 8;
	}

	else if (iRand <= (OneOp + TwoOp + ThreeOp + FourOp + FiveOp + SixOp + SevenOp + EightOp + NineOp))
	{
		iOptionCount = 9;
	}

	else
	{
		iOptionCount = 9;
	}

	if (iOptionCount == 0)
	{
		return 0;
	}

	if (iOptionCount > iExcOption)
	{
		iOptionCount = iExcOption;
	}
	//sLog->outError("[K2]1# RuudShop %d", iOptionCount);
	//sLog->outBasic("[K2] iOptionCount %d", iOptionCount);
	BYTE ExcOption = 0;

	for (int i = 0; i < 5; i++)
	{
		btSocketExcOption[i] = 0xFF;
	}

	bool bOptionTaken;
	while (iOptionCount != 0)
	{
		BYTE btOptionIndex = rand() % iExcOption;
		BYTE Option = btExcOptions[btOptionIndex];
		DWORD dwRate = dwExcOptionRate[btOptionIndex];

		bOptionTaken = false;

		if (Option >= 6)
		{
			for (int i = 0; i < 5; i++)
			{
				if (btSocketExcOption[i] == Option)
				{
					bOptionTaken = true;
					//sLog->outBasic("[K2] bOptionTaken %d", Option);
					break;
				}
			}
		}

		else
		{
			if ((ExcOption & (1 << (5 - Option))) == Option)
			{
				bOptionTaken = true;
				//sLog->outBasic("[K2] bOptionTaken %d", bOptionTaken);
			}
		}

		if (bOptionTaken == false && (rand() % 10000) < dwRate)
		{
			if (Option >= 6)
			{
				for (int i = 0; i < 5; i++)
				{
					if (btSocketExcOption[i] == 0xFF)
					{
						btSocketExcOption[i] = Option;
						break;
					}
				}
			}

			else
			{
				ExcOption |= (1 << (5 - Option));
			}

			iOptionCount--;
			//sLog->outBasic("[K2] iOptionCount %d", iOptionCount);
		}
	}
	//sLog->outBasic("[K2] bOptionTaken %d", ExcOption);
	return ExcOption;
}

/*
int CItemObjectOptionTypeMng::CommonExcOptionRand(int ItemKindA, BYTE *btSocketExcOption)
{
	BYTE btExcOptions[9];
	DWORD dwExcOptionRate[9];
	int iExcOption = 0;
	for (int i = 0; i < 20; i++)
	{
	//	sLog->outError("[K2]1# ItemKindA %d %d %d [%d]", this->m_CommonExtOptionType[i].ItemKindA_1, this->m_CommonExtOptionType[i].ItemKindA_2, this->m_CommonExtOptionType[i].ItemKindA_3, ItemKindA);
		if (this->m_CommonExtOptionType[i].ItemKindA_1 == ItemKindA ||
			this->m_CommonExtOptionType[i].ItemKindA_2 == ItemKindA ||
			this->m_CommonExtOptionType[i].ItemKindA_3 == ItemKindA)
		{
			btExcOptions[iExcOption] = this->m_CommonExtOptionType[i].OptionNumber;
			dwExcOptionRate[iExcOption] = this->m_CommonExtOptionType[i].OptionRate;
			iExcOption++;

			if (iExcOption >= 9)
			{
				break;
			}
		}
	}

	int OneOp = this->m_ExcellentOptionRate.m_CommonOptionRate[0];
	int TwoOp = this->m_ExcellentOptionRate.m_CommonOptionRate[1];
	int ThreeOp = this->m_ExcellentOptionRate.m_CommonOptionRate[2];
	int FourOp = this->m_ExcellentOptionRate.m_CommonOptionRate[3];
	int FiveOp = this->m_ExcellentOptionRate.m_CommonOptionRate[4];
	int SixOp = this->m_ExcellentOptionRate.m_CommonOptionRate[5];
	int SevenOp = this->m_ExcellentOptionRate.m_CommonOptionRate[6];
	int EightOp = this->m_ExcellentOptionRate.m_CommonOptionRate[7];
	int NineOp = this->m_ExcellentOptionRate.m_CommonOptionRate[8];
	int iRand = rand() % 10000;
	int iOptionCount = 0;

	if (iRand <= OneOp)
	{
		iOptionCount = 1;
	}

	else if (iRand <= (OneOp + TwoOp))
	{
		iOptionCount = 2;
	}

	else if (iRand <= (OneOp + TwoOp + ThreeOp))
	{
		iOptionCount = 3;
	}

	else if (iRand <= (OneOp + TwoOp + ThreeOp + FourOp))
	{
		iOptionCount = 4;
	}

	else if (iRand <= (OneOp + TwoOp + ThreeOp + FourOp + FiveOp))
	{
		iOptionCount = 5;
	}

	else if (iRand <= (OneOp + TwoOp + ThreeOp + FourOp + FiveOp + SixOp))
	{
		iOptionCount = 6;
	}

	else if (iRand <= (OneOp + TwoOp + ThreeOp + FourOp + FiveOp + SixOp + SevenOp))
	{
		iOptionCount = 7;
	}

	else if (iRand <= (OneOp + TwoOp + ThreeOp + FourOp + FiveOp + SixOp + SevenOp + EightOp))
	{
		iOptionCount = 8;
	}

	else if (iRand <= (OneOp + TwoOp + ThreeOp + FourOp + FiveOp + SixOp + SevenOp + EightOp + NineOp))
	{
		iOptionCount = 9;
	}

	else
	{
		iOptionCount = 9;
	}

	if (iOptionCount == 0)
	{
		return 0;
	}

	if (iOptionCount > iExcOption)
	{
		iOptionCount = iExcOption;
	}
	//sLog->outError("[K2]1# RuudShop %d", iOptionCount);
	//sLog->outBasic("[K2] iOptionCount %d", iOptionCount);
	BYTE ExcOption = 0;

	for (int i = 0; i < 5; i++)
	{
		btSocketExcOption[i] = 0xFF;
	}

	bool bOptionTaken;
	while (iOptionCount != 0)
	{
		BYTE btOptionIndex = rand() % iExcOption;
		BYTE Option = btExcOptions[btOptionIndex];
		DWORD dwRate = dwExcOptionRate[btOptionIndex];

		bOptionTaken = false;

		if (Option >= 6)
		{
			for (int i = 0; i < 5; i++)
			{
				if (btSocketExcOption[i] == Option)
				{
					bOptionTaken = true;
					//sLog->outBasic("[K2] bOptionTaken %d", Option);
					break;
				}
			}
		}

		else
		{
			if ((ExcOption & (1 << (5 - Option))) == Option)
			{
				bOptionTaken = true;
				//sLog->outBasic("[K2] bOptionTaken %d", bOptionTaken);
			}
		}

		if (bOptionTaken == false && (rand() % 10000) < dwRate)
		{
			if (Option >= 6)
			{
				for (int i = 0; i < 5; i++)
				{
					if (btSocketExcOption[i] == 0xFF)
					{
						btSocketExcOption[i] = Option;
						break;
					}
				}
			}

			else
			{
				ExcOption |= (1 << (5 - Option));
			}

			iOptionCount--;
			//sLog->outBasic("[K2] iOptionCount %d", iOptionCount);
		}
	}
	//sLog->outBasic("[K2] bOptionTaken %d", ExcOption);
	return ExcOption;
}*/

int CItemObjectOptionTypeMng::CommonExcOptionGetCount(int ItemKindA, BYTE * btSocketExcOption, int iOptionCount)
{
	BYTE btExcOptions[9];
	DWORD dwExcOptionRate[9];
	int iExcOption = 0;

	for (int i = 0; i < 20; i++)
	{
		if (this->m_CommonExtOptionType[i].ItemKindA_1 == ItemKindA ||
			this->m_CommonExtOptionType[i].ItemKindA_2 == ItemKindA ||
			this->m_CommonExtOptionType[i].ItemKindA_3 == ItemKindA)
		{
			btExcOptions[iExcOption] = this->m_CommonExtOptionType[i].OptionNumber;
			dwExcOptionRate[iExcOption] = this->m_CommonExtOptionType[i].OptionRate;
			iExcOption++;

			if (iExcOption >= 9)
			{
				break;
			}
		}
	}

	if (iOptionCount == 0)
	{
		return 0;
	}

	if (iOptionCount > iExcOption)
	{
		iOptionCount = iExcOption;
	}

	BYTE ExcOption = 0;

	for (int i = 0; i < 5; i++)
	{
		btSocketExcOption[i] = 0xFF;
	}

	bool bOptionTaken;
	while (iOptionCount != 0)
	{
		BYTE btOptionIndex = rand() % iExcOption;
		BYTE Option = btExcOptions[btOptionIndex];
		DWORD dwRate = dwExcOptionRate[btOptionIndex];
		bOptionTaken = false;

		if (Option >= 6)
		{
			for (int i = 0; i < 5; i++)
			{
				if (btSocketExcOption[i] == Option)
				{
					bOptionTaken = true;
					break;
				}
			}
		}

		else
		{
			if ((ExcOption & (1 << (5 - Option))) == Option)
			{
				bOptionTaken = true;
			}
		}

		if (bOptionTaken == false && (1) < dwRate)
		{
			if (Option >= 6)
			{
				for (int i = 0; i < 5; i++)
				{
					if (btSocketExcOption[i] == 0xFF)
					{
						btSocketExcOption[i] = Option;
						break;
					}
				}
			}

			else
			{
				ExcOption |= (1 << (5 - Option));
			}

			iOptionCount--;
		}
	}

	return ExcOption;
}

int CItemObjectOptionTypeMng::WingExcOptionRand(int ItemKindA, int ItemKindB, BYTE * btSocketExcOption)
{
	BYTE btExcOptions[9];
	DWORD dwExcOptionRate[9];
	int iExcOption = 0;

	for (int i = 0; i < 40; i++)
	{
		if (this->m_WingExtOptionType[i].ItemKindA == ItemKindA && this->m_WingExtOptionType[i].ItemKindB == ItemKindB)
		{
			btExcOptions[iExcOption] = this->m_WingExtOptionType[i].OptionNumber;
			dwExcOptionRate[iExcOption] = this->m_WingExtOptionType[i].OptionRate;
			iExcOption++;

			if (iExcOption >= 9)
			{
				break;
			}
		}
	}

	int OneOp = 0;
	int TwoOp = 0;
	int ThreeOp = 0;
	int FourOp = 0;
	int FiveOp = 0;
	int SixOp = 0;
	int SevenOp = 0;
	int EightOp = 0;
	int NineOp = 0;

	for (std::vector<WING_EXC_OPTION_RATE>::iterator It = this->m_ExcellentOptionRate.m_vtWingOptionRate.begin(); It != this->m_ExcellentOptionRate.m_vtWingOptionRate.end(); It++)
	{
		if (It->ItemKindA == ItemKindA && It->ItemKindB == ItemKindB)
		{
			OneOp = It->WingOptionRate[0];
			TwoOp = It->WingOptionRate[1];
			ThreeOp = It->WingOptionRate[2];
			FourOp = It->WingOptionRate[3];
			FiveOp = It->WingOptionRate[4];
			SixOp = It->WingOptionRate[5];
			SevenOp = It->WingOptionRate[6];
			EightOp = It->WingOptionRate[7];
			NineOp = It->WingOptionRate[8];
			break;
		}
	}

	int iRand = rand() % 10000;
	int iOptionCount = 0;

	if (iRand <= OneOp)
	{
		iOptionCount = 0;
	}

	else if (iRand <= (OneOp + TwoOp))
	{
		iOptionCount = 1;
	}

	else if (iRand <= (OneOp + TwoOp + ThreeOp))
	{
		iOptionCount = 2;
	}

	else if (iRand <= (OneOp + TwoOp + ThreeOp + FourOp))
	{
		iOptionCount = 3;
	}

	else if (iRand <= (OneOp + TwoOp + ThreeOp + FourOp + FiveOp))
	{
		iOptionCount = 4;
	}

	else if (iRand <= (OneOp + TwoOp + ThreeOp + FourOp + FiveOp + SixOp))
	{
		iOptionCount = 5;
	}

	else if (iRand <= (OneOp + TwoOp + ThreeOp + FourOp + FiveOp + SixOp + SevenOp))
	{
		iOptionCount = 6;
	}

	else if (iRand <= (OneOp + TwoOp + ThreeOp + FourOp + FiveOp + SixOp + SevenOp + EightOp))
	{
		iOptionCount = 7;
	}

	else if (iRand <= (OneOp + TwoOp + ThreeOp + FourOp + FiveOp + SixOp + SevenOp + EightOp + NineOp))
	{
		iOptionCount = 8;
	}

	else
	{
		iOptionCount = 9;
	}

	if (iOptionCount == 0)
	{
		return 0;
	}

	if (iOptionCount > iExcOption)
	{
		iOptionCount = iExcOption;
	}

	BYTE ExcOption = 0;

	for (int i = 0; i < 5; i++)
	{
		btSocketExcOption[i] = 0xFF;
	}

	bool bOptionTaken;
	while (iOptionCount != 0)
	{
		BYTE btOptionIndex = rand() % iExcOption;
		BYTE Option = btExcOptions[btOptionIndex];
		DWORD dwRate = dwExcOptionRate[btOptionIndex];

		bOptionTaken = false;

		if (Option >= 6)
		{
			for (int i = 0; i < 5; i++)
			{
				if (btSocketExcOption[i] == Option)
				{
					bOptionTaken = true;
					break;
				}
			}
		}

		else
		{
			if ((ExcOption & (1 << (5 - Option))) == Option)
			{
				bOptionTaken = true;
			}
		}

		if (bOptionTaken == false && (rand() % 10000) < dwRate)
		{
			if (Option >= 6)
			{
				for (int i = 0; i < 5; i++)
				{
					if (btSocketExcOption[i] == 0xFF)
					{
						btSocketExcOption[i] = Option;
						break;
					}
				}
			}

			else
			{
				ExcOption |= (1 << (5 - Option));
			}

			iOptionCount--;
		}
	}

	return ExcOption;
}

int CItemObjectOptionTypeMng::WingExcOptionGetCount(int ItemKindA, int ItemKindB, BYTE * btSocketExcOption, int iOptionCount)
{
	BYTE btExcOptions[9];
	DWORD dwExcOptionRate[9];
	int iExcOption = 0;

	for (int i = 0; i < 40; i++)
	{
		if (this->m_WingExtOptionType[i].ItemKindA == ItemKindA && this->m_WingExtOptionType[i].ItemKindB == ItemKindB)
		{
			btExcOptions[iExcOption] = this->m_WingExtOptionType[i].OptionNumber;
			dwExcOptionRate[iExcOption] = this->m_WingExtOptionType[i].OptionRate;
			iExcOption++;

			if (iExcOption >= 9)
			{
				break;
			}
		}
	}

	if (iOptionCount == 0)
	{
		return 0;
	}

	if (iOptionCount > iExcOption)
	{
		iOptionCount = iExcOption;
	}

	BYTE ExcOption = 0;

	for (int i = 0; i < 5; i++)
	{
		btSocketExcOption[i] = 0xFF;
	}

	bool bOptionTaken;
	while (iOptionCount != 0)
	{
		BYTE btOptionIndex = rand() % iExcOption;
		BYTE Option = btExcOptions[btOptionIndex];
		DWORD dwRate = dwExcOptionRate[btOptionIndex];

		bOptionTaken = false;

		if (Option >= 6)
		{
			for (int i = 0; i < 5; i++)
			{
				if (btSocketExcOption[i] == Option)
				{
					bOptionTaken = true;
					break;
				}
			}
		}

		else
		{
			if ((ExcOption & (1 << (5 - Option))) == Option)
			{
				bOptionTaken = true;
			}
		}

		if (bOptionTaken == false && (rand() % 10000) < dwRate)
		{
			if (Option >= 6)
			{
				for (int i = 0; i < 5; i++)
				{
					if (btSocketExcOption[i] == 0xFF)
					{
						btSocketExcOption[i] = Option;
						break;
					}
				}
			}

			else
			{
				ExcOption |= (1 << (5 - Option));
			}

			iOptionCount--;
		}
	}

	return ExcOption;
}

void CItemObjectOptionTypeMng::CalcExcOptionEffect(CGameObject &Obj)
{
	for (int i = 0; i < INVETORY_WEAR_SIZE; i++)
	{
		if (Obj.pInventory[i]->IsItem() == FALSE)
		{
			continue;
		}

		if (Obj.pInventory[i]->m_Durability == 0.0f)
		{
			continue;
		}

		if (Obj.pInventory[i]->m_IsValidItem == false)
		{
			continue;
		}

		if (Obj.pInventory[i]->IsExtItem() == FALSE)
		{
			continue;
		}

		LPITEM_ATTRIBUTE lpAttr = GetItemAttr(Obj.pInventory[i]->m_Type);

		if (lpAttr == NULL)
		{
			continue;
		}

		bool bOptionExist = false;

		for (int j = 0; j < MAX_EXCELLENT_COMMON_OPTIONS; j++)
		{
			if (this->m_CommonExtOptionType[j].OptionID != -1)
			{
				if(this->m_CommonExtOptionType[j].ItemKindA_1 == lpAttr->ItemKindA ||
					this->m_CommonExtOptionType[j].ItemKindA_2 == lpAttr->ItemKindA ||
					this->m_CommonExtOptionType[j].ItemKindA_3 == lpAttr->ItemKindA)
				{
					bOptionExist = false;

					if (this->m_CommonExtOptionType[j].OptionNumber >= 6)
					{
						for (int k = 0; k < 5; k++)
						{
							if (Obj.pInventory[i]->m_SocketOption[k] == this->m_CommonExtOptionType[j].OptionNumber)
							{
								bOptionExist = true;
								break;
							}
						}
					}
					else
					{
						if ((Obj.pInventory[i]->m_NewOption & (1 << (5 - this->m_CommonExtOptionType[j].OptionNumber))) == (1 << (5 - this->m_CommonExtOptionType[j].OptionNumber)))
						{
							bOptionExist = true;
						}
					}

					if (bOptionExist == true)
					{
						this->_SetOptionEffect(lpObj, this->m_CommonExtOptionType[j].OptionEffect, this->m_CommonExtOptionType[j].OptionValue,
							this->m_CommonExtOptionType[j].Operator, this->m_CommonExtOptionType[j].FormulaID, i);
					}
				}
			}
		}
	}
}

COMMON_EXT_OPTION_TYPE * CItemObjectOptionTypeMng::GetCommonExcOption(int ExcOptionID)
{
	for (int i = 0; i < MAX_EXCELLENT_COMMON_OPTIONS; i++)
	{
		if (this->m_CommonExtOptionType[i].OptionID == ExcOptionID)
		{
			return &this->m_CommonExtOptionType[i];
		}
	}

	return NULL;
}

void CItemObjectOptionTypeMng::CalcWingOptionEffect(CGameObject &Obj)
{
	if (Obj.pInventory[7]->IsItem() == FALSE)
	{
		return;
	}

	if (Obj.pInventory[7]->m_Durability == 0.0f)
	{
		return;
	}

	if (Obj.pInventory[7]->m_IsValidItem == false)
	{
		return;
	}

	if (Obj.pInventory[7]->IsExtItem() == FALSE)
	{
		return;
	}

	LPITEM_ATTRIBUTE lpAttr = GetItemAttr(Obj.pInventory[7]->m_Type);

	if (lpAttr == NULL)
	{
		return;
	}

	bool bOptionExist = false;

	for (int i = 0; i < MAX_EXCELLENT_WING_OPTIONS; i++)
	{
		if (this->m_WingExtOptionType[i].OptionID != -1)
		{
			if (this->m_WingExtOptionType[i].ItemKindA == lpAttr->ItemKindA &&
				this->m_WingExtOptionType[i].ItemKindB == lpAttr->ItemKindB)
			{
				bOptionExist = false;

				if (this->m_WingExtOptionType[i].OptionNumber >= 6)
				{
					for (int k = 0; k < 5; k++)
					{
						if (Obj.pInventory[7]->m_SocketOption[k] == this->m_WingExtOptionType[i].OptionNumber)
						{
							bOptionExist = true;
							break;
						}
					}
				}

				else
				{
					if ((Obj.pInventory[7]->m_NewOption & (1 << (5 - this->m_WingExtOptionType[i].OptionNumber))) == (1 << (5 - this->m_WingExtOptionType[i].OptionNumber)))
					{
						bOptionExist = true;
					}
				}

				if (bOptionExist == true)
				{
					this->_SetOptionEffect(lpObj, this->m_WingExtOptionType[i].OptionEffect, this->m_WingExtOptionType[i].OptionValue,
						this->m_WingExtOptionType[i].Operator, this->m_WingExtOptionType[i].FormulaID, 7);
				}
			}
		}
	}
}

WING_EXT_OPTION_TYPE * CItemObjectOptionTypeMng::GetWingOption(int ExcOptionID)
{
	for (int i = 0; i < MAX_EXCELLENT_WING_OPTIONS; i++)
	{
		if (this->m_WingExtOptionType[i].OptionID == ExcOptionID)
		{
			return &this->m_WingExtOptionType[i];
		}
	}

	return NULL;
}

int CItemObjectOptionTypeMng::_CalcEffectValue(CGameObject lpObj, int iInputValue, int iOperator, int iBaseValue, int iFormulaID, CItemObject * Item)
{
	int iEffectValue = 0;
	//sLog->outBasic("[K2] Operator [%d] iBaseValue %d", iOperator, iBaseValue);
	switch (iOperator)
	{
		case EXC_OPERATOR_NORMAL:
			iEffectValue = iBaseValue;
			break;
		case EXC_OPERATOR_PERCENT:
			iEffectValue = iBaseValue;//* iInputValue / 100;
			break;
		case EXC_OPERATOR_DIVIDE_BY_LEVEL_20:
			iEffectValue = iBaseValue * (Obj.Level + Obj.m_PlayerData->MasterLevel) / 20;
			break;
		case EXC_OPERATOR_DIVIDE_BY_LIFE:
			iEffectValue = (Obj.MaxLife + Obj.AddLife) / iBaseValue;
			break;
		case EXC_OPERATOR_DIVIDE_BY_MANA:
			iEffectValue = (Obj.MaxMana + Obj.AddMana) / iBaseValue;
			break;
		case EXC_OPERATOR_INC_PER_LEVEL:
			iEffectValue = iBaseValue + 5 * Item->m_Level;
			break;
		case EXC_OPERATOR_FORMULA:
			{
				if (iFormulaID < 0)
				{
					iEffectValue = iBaseValue;
					return iEffectValue;
				}

				int iItemDropLevel = ItemAttribute[Item->m_Type].Level;

				iEffectValue = g_FormulaParse.UseFormula(FORMULA_TYPE_EXCELLENT, iFormulaID, "x", 1, (double)iItemDropLevel);
			}
			break;
	}

	return iEffectValue;
}

void CItemObjectOptionTypeMng::_SetOptionEffect(CGameObject &Obj, BYTE * OptionEffect, int iEffectValue, int iOperator, int iFormulaID, BYTE btItemPos)
{
	this->_SetEffect_Etc(lpObj, OptionEffect[0], iEffectValue, iOperator, iFormulaID, btItemPos);
	this->_SetEffect_Rate(lpObj, OptionEffect[1], iEffectValue, iOperator, iFormulaID, btItemPos);
	this->_SetEffect_Attack(lpObj, OptionEffect[2], iEffectValue, iOperator, iFormulaID, btItemPos);
	this->_SetEffect_Defense(lpObj, OptionEffect[3], iEffectValue, iOperator, iFormulaID, btItemPos);
	this->_SetEffect_Life(lpObj, OptionEffect[4], iEffectValue, iOperator, iFormulaID, btItemPos);
	this->_SetEffect_Mana(lpObj, OptionEffect[5], iEffectValue, iOperator, iFormulaID, btItemPos);
	this->_SetEffect_Skill(lpObj, OptionEffect[6], iEffectValue, iOperator, iFormulaID, btItemPos);
}

void CItemObjectOptionTypeMng::_SetEffect_Etc(CGameObject lpObj, BYTE OptionEffectID, int iBaseValue, int iOperator, int iFormulaID, BYTE btItemPos)
{
	int iEffectValue = 0;

	switch (OptionEffectID)
	{
	case 0:
		break;
	case 1:
		iEffectValue = this->_CalcEffectValue(lpObj, 1, iOperator, iBaseValue, iFormulaID, &Obj.pInventory[btItemPos]);
		Obj.MonsterDieGetMoney += iEffectValue;
		break;
	}
}

void CItemObjectOptionTypeMng::_SetEffect_Rate(CGameObject lpObj, BYTE OptionEffectID, int iBaseValue, int iOperator, int iFormulaID, BYTE btItemPos)
{
	int iEffectValue = 0;
	switch (OptionEffectID)
	{
	case 0:
		break;
	case 1:
		iEffectValue = this->_CalcEffectValue(lpObj, 1, iOperator, iBaseValue, iFormulaID, &Obj.pInventory[btItemPos]);
		Obj.m_ExcelentDamage += iEffectValue;
		break;
	case 2:
		iEffectValue = this->_CalcEffectValue(lpObj, 1, iOperator, iBaseValue, iFormulaID, &Obj.pInventory[btItemPos]);
		Obj.m_CriticalDamage += iEffectValue;
		break;
	case 4:
		iEffectValue = this->_CalcEffectValue(lpObj, 1, iOperator, iBaseValue, iFormulaID, &Obj.pInventory[btItemPos]);
		Obj.m_PlayerData->m_WingExcOption.iWingOpIgnoreEnemyDefense = iEffectValue;
		break;
	case 5:
		iEffectValue = this->_CalcEffectValue(lpObj, 1, iOperator, iBaseValue, iFormulaID, &Obj.pInventory[btItemPos]);
		Obj.m_PlayerData->m_WingExcOption.iWingOpRecoveryHP = iEffectValue;
		break;
	case 6:
		iEffectValue = this->_CalcEffectValue(lpObj, 1, iOperator, iBaseValue, iFormulaID, &Obj.pInventory[btItemPos]);
		Obj.m_PlayerData->m_WingExcOption.iWingOpRecoveryMana = iEffectValue;
		break;
	case 7:
		iEffectValue = this->_CalcEffectValue(lpObj, 1, iOperator, iBaseValue, iFormulaID, &Obj.pInventory[btItemPos]);
		Obj.m_PlayerData->m_WingExcOption.iWingOpReturnEnemyDamage = iEffectValue;
		break;
	case 8:
		iEffectValue = this->_CalcEffectValue(lpObj, 1, iOperator, iBaseValue, iFormulaID, &Obj.pInventory[btItemPos]);
		Obj.m_PlayerData->m_WingExcOption.iWingOpAddDoubleDamage = iEffectValue;
		break;
	}
}

void CItemObjectOptionTypeMng::_SetEffect_Attack(CGameObject lpObj, BYTE OptionEffectID, int iBaseValue, int iOperator, int iFormulaID, BYTE btItemPos)
{
	int iEffectValue = 0;

	switch (OptionEffectID)
	{
	case 0:
		break;
	case 1:
		if ((Obj.pInventory[btItemPos]->m_Type >= ITEMGET(5, 0) && Obj.pInventory[btItemPos]->m_Type < ITEMGET(6, 0)) ||
			Obj.pInventory[btItemPos]->m_Type == ITEMGET(13, 12) || Obj.pInventory[btItemPos]->m_Type == ITEMGET(13, 25) ||
			Obj.pInventory[btItemPos]->m_Type == ITEMGET(13, 27))
		{
			iEffectValue = this->_CalcEffectValue(lpObj, Obj.m_MagicDamageMax, iOperator, iBaseValue, iFormulaID, &Obj.pInventory[btItemPos]);
			Obj.m_MagicDamageMax += iEffectValue;

			iEffectValue = this->_CalcEffectValue(lpObj, Obj.m_MagicDamageMin, iOperator, iBaseValue, iFormulaID, &Obj.pInventory[btItemPos]);
			Obj.m_MagicDamageMin += iEffectValue;
		}
		else
		{
			if (btItemPos == 0 || btItemPos == 9)
			{
				iEffectValue = this->_CalcEffectValue(lpObj, Obj.m_AttackDamageMinRight, iOperator, iBaseValue, iFormulaID, &Obj.pInventory[btItemPos]);
				Obj.m_AttackDamageMinRight += iEffectValue;

				iEffectValue = this->_CalcEffectValue(lpObj, Obj.m_AttackDamageMaxRight, iOperator, iBaseValue, iFormulaID, &Obj.pInventory[btItemPos]);
				Obj.m_AttackDamageMaxRight += iEffectValue;
			}

			if (btItemPos == 1 || btItemPos == 9)
			{
				iEffectValue = this->_CalcEffectValue(lpObj, Obj.m_AttackDamageMinLeft, iOperator, iBaseValue, iFormulaID, &Obj.pInventory[btItemPos]);
				Obj.m_AttackDamageMinLeft += iEffectValue;

				iEffectValue = this->_CalcEffectValue(lpObj, Obj.m_AttackDamageMaxLeft , iOperator, iBaseValue, iFormulaID, &Obj.pInventory[btItemPos]);
				Obj.m_AttackDamageMaxLeft += iEffectValue;
			}
		}
		break;
	case 2:
		iEffectValue = this->_CalcEffectValue(lpObj, Obj.m_AttackSpeed, iOperator, iBaseValue, iFormulaID, &Obj.pInventory[btItemPos]);
		Obj.m_AttackSpeed += iEffectValue;

		iEffectValue = this->_CalcEffectValue(lpObj, Obj.m_MagicSpeed, iOperator, iBaseValue, iFormulaID, &Obj.pInventory[btItemPos]);
		Obj.m_MagicSpeed += iEffectValue;
		break;
	case 3:
		iEffectValue = this->_CalcEffectValue(lpObj, 1, iOperator, iBaseValue, iFormulaID, &Obj.pInventory[btItemPos]);
		Obj.DamageReflect += iEffectValue;
		break;
	}
}

void CItemObjectOptionTypeMng::_SetEffect_Defense(CGameObject lpObj, BYTE OptionEffectID, int iBaseValue, int iOperator, int iFormulaID, BYTE btItemPos)
{
	int iEffectValue = 0;

	switch (OptionEffectID)
	{
	case 0:
		break;
	case 1:
		iEffectValue = this->_CalcEffectValue(lpObj, 1, iOperator, iBaseValue, iFormulaID, &Obj.pInventory[btItemPos]);
		
		Obj.DamageDecrease += iEffectValue;
		break;
	case 2:
		iEffectValue = this->_CalcEffectValue(lpObj, Obj.m_SuccessfulBlocking, iOperator, iBaseValue, iFormulaID, &Obj.pInventory[btItemPos]);
		Obj.m_SuccessfulBlocking += iEffectValue;
		break;
#if(CUSTOM_DAMAGES12 == 1)
	case 3:
		iEffectValue = this->_CalcEffectValue(lpObj, 1, iOperator, iBaseValue, iFormulaID, &Obj.pInventory[btItemPos]);
		Obj.S12DamageDecrease += iEffectValue;
		break;
#endif
	}
}

void CItemObjectOptionTypeMng::_SetEffect_Life(CGameObject lpObj, BYTE OptionEffectID, int iBaseValue, int iOperator, int iFormulaID, BYTE btItemPos)
{
	int iEffectValue = 0;

	switch (OptionEffectID)
	{
	case 0:
		break;
	case 1:
		Obj.MonsterDieGetLife++;
		break;
	case 2:
		iEffectValue = this->_CalcEffectValue(lpObj, Obj.MaxLife, iOperator, iBaseValue, iFormulaID, &Obj.pInventory[btItemPos]);
		//sLog->outBasic("[K2] Operator 1# [%d] iBaseValue %d iEffectValue %d", iOperator, iBaseValue, iEffectValue);
		Obj.AddLife += ((Obj.MaxLife * iEffectValue) / 100);
	///	Obj.AddLife += iEffectValue;
		break;
	}
}

void CItemObjectOptionTypeMng::_SetEffect_Mana(CGameObject lpObj, BYTE OptionEffectID, int iBaseValue, int iOperator, int iFormulaID, BYTE btItemPos)
{
	int iEffectValue = 0;

	switch (OptionEffectID)
	{
	case 0:
		break;
	case 1:
		Obj.MonsterDieGetMana++;
		break;
	case 2:
		iEffectValue = this->_CalcEffectValue(lpObj, Obj.MaxMana, iOperator, iBaseValue, iFormulaID, &Obj.pInventory[btItemPos]);
		Obj.AddMana += ((Obj.MaxMana * iEffectValue) / 100);
		break;
	}
}

void CItemObjectOptionTypeMng::_SetEffect_Skill(CGameObject lpObj, BYTE OptionEffectID, int iBaseValue, int iOperator, int iFormulaID, BYTE btItemPos)
{
	int iEffectValue = 0;

	switch (OptionEffectID)
	{
	case 0:
		break;
	case 1:
		iEffectValue = this->_CalcEffectValue(lpObj, 1, iOperator, iBaseValue, iFormulaID, &Obj.pInventory[btItemPos]);
		Obj.AddBP += iEffectValue;
		break;
	case 2:
		iEffectValue = this->_CalcEffectValue(lpObj, 1, iOperator, iBaseValue, iFormulaID, &Obj.pInventory[btItemPos]);
		Obj.AddLeadership += iEffectValue;
		break;
	}
}



////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

