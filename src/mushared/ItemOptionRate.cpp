////////////////////////////////////////////////////////////////////////////////
// ItemOptionRate.cpp
// ItemOptionManager.cpp: implementation of the CItemOptionManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ItemOptionRate.h"
#include "zzzitem.h"
#include "MemScript.h"
#include "PentagramSystem.h"
#include "RandomManager.h"
#include "SetItemOption.h"
#include "ItemSocketOptionSystem.h"
#include "winutil.h"
#include "TLog.h"
#include "LargeRand.h"
#include "SocketItemType.h"

CItemOptionRate gItemOptionRate;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemOptionRate::CItemOptionRate() // OK
{
	this->m_ItemOption0RateInfo.clear();

	this->m_ItemOption1RateInfo.clear();

	this->m_ItemOption2RateInfo.clear();

	this->m_ItemOption3RateInfo.clear();

	this->m_ItemOption4RateInfo.clear();

	this->m_ItemOption5RateInfo.clear();

	this->m_ItemOption6RateInfo.clear();
}

CItemOptionRate::~CItemOptionRate() // OK
{

}

void CItemOptionRate::Load(char* path) // OK
{
	CMemScript* lpMemScript = new CMemScript;

	if (lpMemScript == 0)
	{
		g_Log.MsgBox(MEM_SCRIPT_ALLOC_ERROR, path);
		return;
	}

	if (lpMemScript->SetBuffer(path) == 0)
	{
		g_Log.MsgBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	this->m_ItemOption0RateInfo.clear();

	this->m_ItemOption1RateInfo.clear();

	this->m_ItemOption2RateInfo.clear();

	this->m_ItemOption3RateInfo.clear();

	this->m_ItemOption4RateInfo.clear();

	this->m_ItemOption5RateInfo.clear();

	this->m_ItemOption6RateInfo.clear();

	try
	{
		while (true)
		{
			if (lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			int section = lpMemScript->GetNumber();

			while (true)
			{
				if (section == 0)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					ITEM_OPTION0_RATE_INFO info;

					info.Index = lpMemScript->GetNumber();

					for (int n = 0; n < MAX_ITEM_OPTION0_RATE; n++)
					{
						info.Rate[n] = lpMemScript->GetAsNumber();
					}

					this->m_ItemOption0RateInfo.insert(std::pair<int, ITEM_OPTION0_RATE_INFO>(info.Index, info));
				}
				else if (section == 1)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					ITEM_OPTION1_RATE_INFO info;

					info.Index = lpMemScript->GetNumber();

					for (int n = 0; n < MAX_ITEM_OPTION1_RATE; n++)
					{
						info.Rate[n] = lpMemScript->GetAsNumber();
					}

					this->m_ItemOption1RateInfo.insert(std::pair<int, ITEM_OPTION1_RATE_INFO>(info.Index, info));
				}
				else if (section == 2)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					ITEM_OPTION2_RATE_INFO info;

					info.Index = lpMemScript->GetNumber();

					for (int n = 0; n < MAX_ITEM_OPTION2_RATE; n++)
					{
						info.Rate[n] = lpMemScript->GetAsNumber();
					}

					this->m_ItemOption2RateInfo.insert(std::pair<int, ITEM_OPTION2_RATE_INFO>(info.Index, info));
				}
				else if (section == 3)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					ITEM_OPTION3_RATE_INFO info;

					info.Index = lpMemScript->GetNumber();

					for (int n = 0; n < MAX_ITEM_OPTION3_RATE; n++)
					{
						info.Rate[n] = lpMemScript->GetAsNumber();
					}

					this->m_ItemOption3RateInfo.insert(std::pair<int, ITEM_OPTION3_RATE_INFO>(info.Index, info));
				}
				else if (section == 4)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					ITEM_OPTION4_RATE_INFO info;

					info.Index = lpMemScript->GetNumber();

					for (int n = 0; n < MAX_ITEM_OPTION4_RATE; n++)
					{
						info.Rate[n] = lpMemScript->GetAsNumber();
					}

					this->m_ItemOption4RateInfo.insert(std::pair<int, ITEM_OPTION4_RATE_INFO>(info.Index, info));
				}
				else if (section == 5)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					ITEM_OPTION5_RATE_INFO info;

					info.Index = lpMemScript->GetNumber();

					for (int n = 0; n < MAX_ITEM_OPTION5_RATE; n++)
					{
						info.Rate[n] = lpMemScript->GetAsNumber();
					}

					this->m_ItemOption5RateInfo.insert(std::pair<int, ITEM_OPTION5_RATE_INFO>(info.Index, info));
				}
				else if (section == 6)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					ITEM_OPTION6_RATE_INFO info;

					info.Index = lpMemScript->GetNumber();

					for (int n = 0; n < MAX_ITEM_OPTION6_RATE; n++)
					{
						info.Rate[n] = lpMemScript->GetAsNumber();
					}

					this->m_ItemOption6RateInfo.insert(std::pair<int, ITEM_OPTION6_RATE_INFO>(info.Index, info));
				}
				else
				{
					break;
				}
			}
		}
	}
	catch (...)
	{
		g_Log.MsgBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

bool CItemOptionRate::GetItemOption0(int index, BYTE* option) // OK
{
	std::map<int, ITEM_OPTION0_RATE_INFO>::iterator it = this->m_ItemOption0RateInfo.find(index);

	if (it == this->m_ItemOption0RateInfo.end())
	{
		return 0;
	}

	CRandomManager RandomManager;

	for (int n = 0; n < MAX_ITEM_OPTION0_RATE; n++)
	{
		RandomManager.AddElement(n, it->second.Rate[n]);
	}

	return RandomManager.GetRandomElement(option);
}

bool CItemOptionRate::GetItemOption1(int index, BYTE* option) // OK
{
	std::map<int, ITEM_OPTION1_RATE_INFO>::iterator it = this->m_ItemOption1RateInfo.find(index);

	if (it == this->m_ItemOption1RateInfo.end())
	{
		return 0;
	}

	CRandomManager RandomManager;

	for (int n = 0; n < MAX_ITEM_OPTION1_RATE; n++)
	{
		RandomManager.AddElement(n, it->second.Rate[n]);
	}

	return RandomManager.GetRandomElement(option);
}

bool CItemOptionRate::GetItemOption2(int index, BYTE* option) // OK
{
	std::map<int, ITEM_OPTION2_RATE_INFO>::iterator it = this->m_ItemOption2RateInfo.find(index);

	if (it == this->m_ItemOption2RateInfo.end())
	{
		return 0;
	}

	CRandomManager RandomManager;

	for (int n = 0; n < MAX_ITEM_OPTION2_RATE; n++)
	{
		RandomManager.AddElement(n, it->second.Rate[n]);
	}

	return RandomManager.GetRandomElement(option);
}

bool CItemOptionRate::GetItemOption3(int index, BYTE* option) // OK
{
	std::map<int, ITEM_OPTION3_RATE_INFO>::iterator it = this->m_ItemOption3RateInfo.find(index);

	if (it == this->m_ItemOption3RateInfo.end())
	{
		return 0;
	}

	CRandomManager RandomManager;

	for (int n = 0; n < MAX_ITEM_OPTION3_RATE; n++)
	{
		RandomManager.AddElement(n, it->second.Rate[n]);
	}

	return RandomManager.GetRandomElement(option);
}

bool CItemOptionRate::GetItemOption4(int index, BYTE* option) // OK
{
	std::map<int, ITEM_OPTION4_RATE_INFO>::iterator it = this->m_ItemOption4RateInfo.find(index);

	if (it == this->m_ItemOption4RateInfo.end())
	{
		return 0;
	}

	CRandomManager RandomManager;

	for (int n = 0; n < MAX_ITEM_OPTION4_RATE; n++)
	{
		RandomManager.AddElement(n, it->second.Rate[n]);
	}

	return RandomManager.GetRandomElement(option);
}

bool CItemOptionRate::GetItemOption5(int index, BYTE* option) // OK
{
	std::map<int, ITEM_OPTION5_RATE_INFO>::iterator it = this->m_ItemOption5RateInfo.find(index);

	if (it == this->m_ItemOption5RateInfo.end())
	{
		return 0;
	}

	CRandomManager RandomManager;

	for (int n = 0; n < MAX_ITEM_OPTION5_RATE; n++)
	{
		RandomManager.AddElement(n, it->second.Rate[n]);
	}

	return RandomManager.GetRandomElement(option);
}

bool CItemOptionRate::GetItemOption6(int index, BYTE* option) // OK
{
	std::map<int, ITEM_OPTION6_RATE_INFO>::iterator it = this->m_ItemOption6RateInfo.find(index);

	if (it == this->m_ItemOption6RateInfo.end())
	{
		return 0;
	}

	CRandomManager RandomManager;

	for (int n = 0; n < MAX_ITEM_OPTION6_RATE; n++)
	{
		RandomManager.AddElement(n, it->second.Rate[n]);
	}

	return RandomManager.GetRandomElement(option);
}

bool CItemOptionRate::MakeNewOption(int ItemIndex, int value, BYTE* option) // OK
{
	(*option) = 0;

	int count = 0;

	int MaxNewOption = MAX_ITEM_EXC_OPTION;

	if ((ItemIndex >= ITEMGET(12, 3) && ItemIndex <= ITEMGET(12, 6)) || ItemIndex == ITEMGET(12, 42) || ItemIndex == ITEMGET(12, 49) || ItemIndex == ITEMGET(12, 269) || ItemIndex == ITEMGET(13, 30)) // 2sd Wings
	{
		MaxNewOption = ((ItemIndex == ITEMGET(13, 30)) ? 4 : 3);
	}

	if ((ItemIndex >= ITEMGET(12, 36) && ItemIndex <= ITEMGET(12, 40)) || ItemIndex == ITEMGET(12, 43) || ItemIndex == ITEMGET(12, 270) || ItemIndex == ITEMGET(12, 50)) // 3rd Wings
	{
		MaxNewOption = 4;
	}

	if (ItemIndex >= ITEMGET(12, 262) && ItemIndex <= ITEMGET(12, 265)) // Monster Wings
	{
		MaxNewOption = 2;
	}

	if (ItemIndex == ITEMGET(12, 266)) // Wings of Conqueror
	{
		MaxNewOption = 3;
	}

	if (ItemIndex == ITEMGET(12, 267)) // Wings of Angel and Devil
	{
		MaxNewOption = 3;
	}

	if (ItemIndex == ITEMGET(13, 3)) // Dinorant
	{
		MaxNewOption = 3;
	}

	if (ItemIndex == ITEMGET(13, 37)) // Fenrir
	{
		MaxNewOption = 3;
	}

	while (true)
	{
		if (count >= value || count >= MaxNewOption)
		{
			break;
		}

		int value = 1 << (GetLargeRand() % MaxNewOption);

		if (((*option) & value) == 0)
		{
			(*option) |= value;
			count++;
		}
	}

	return 1;
}

bool CItemOptionRate::MakeSetOption(int ItemIndex, int value, BYTE* option) // OK
{
	if (gSetItemOption.IsSetItem(ItemIndex) == 0 || value == 0)
	{
		(*option) = 0;
		return 0;
	}

	(*option) = 0;

	for (int n = 0; n < MAX_SETITEM_OPTION_INDEX; n++)
	{
		if (gSetItemOption.GetOptionTableIndex(ItemIndex, n) > 0)
		{
			(*option) = (((*option) == 0) ? (n + 1) : (((GetLargeRand() % 2) == 0) ? (n + 1) : (*option)));
		}
	}

	(*option) |= (((*option)>0) ? (4 * value) : 0);

	return 1;
}

bool CItemOptionRate::MakeSocketOption(int ItemIndex, int value, BYTE* option) // OK
{
	if ((gSocketItemType.CheckSocketItemType(ItemIndex) == 0 && g_PentagramSystem.IsPentagramItem(ItemIndex) == 0) || value == 0 || value == 0xFF)
	{
		memset(option, 0xFF, MAX_SOCKET_OPTION);
		return 0;
	}

	if (gSocketItemType.CheckSocketItemType(ItemIndex) == 0)
	{
		for (int n = 0; n < ((value>MAX_SOCKET_OPTION) ? MAX_SOCKET_OPTION : value); n++)
		{
			option[n] = 0xFE;
		}
	}
	else
	{
		for (int n = 0; n < ((value>gSocketItemType.GetSocketItemMaxSocket(ItemIndex)) ? gSocketItemType.GetSocketItemMaxSocket(ItemIndex) : value); n++)
		{
			option[n] = 0xFE;
		}
	}

	return 1;
}


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

