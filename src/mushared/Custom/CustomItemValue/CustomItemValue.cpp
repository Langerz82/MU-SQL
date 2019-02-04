////////////////////////////////////////////////////////////////////////////////
// CustomItemValue.cpp
// ItemValue.cpp: implementation of the CItemValue class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "./Custom/CustomItemValue/CustomItemValue.h"
#include "zzzitem.h"
#include "ItemOptionTypeMng.h"
#include "MemScript.h"
#include "winutil.h"
#include "TLog.h"

CItemValue gItemValue;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CItemValue::CItemValue() // OK
{
	this->m_ItemValueInfo.clear();
}

CItemValue::~CItemValue() // OK
{

}

void CItemValue::Load(char* path) // OK
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

	this->m_ItemValueInfo.clear();

	try
	{
		while (true)
		{
			if (lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			if (strcmp("end", lpMemScript->GetString()) == 0)
			{
				break;
			}

			ITEM_VALUE_INFO info;

			info.Index = lpMemScript->GetNumber();

			info.Level = lpMemScript->GetAsNumber();

			info.Grade = lpMemScript->GetAsNumber();

			info.Value = lpMemScript->GetAsNumber();

			this->m_ItemValueInfo.push_back(info);
		}
	}
	catch (...)
	{
		g_Log.MsgBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

bool CItemValue::GetItemValue(CItem* lpItem, int* value) // OK
{
	for (std::vector<ITEM_VALUE_INFO>::iterator it = this->m_ItemValueInfo.begin(); it != this->m_ItemValueInfo.end(); it++)
	{
		if (it->Index == lpItem->m_Type)
		{
			if (it->Level == -1 || it->Level == lpItem->m_Level)
			{
				if (it->Grade == -1 || it->Grade == lpItem->m_NewOption)
				{
					(*value) = (int)(it->Value*lpItem->m_Durability);
					return 1;
				}
			}
		}
	}

	return 0;
}


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

