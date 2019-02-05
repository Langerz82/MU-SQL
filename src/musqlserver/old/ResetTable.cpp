////////////////////////////////////////////////////////////////////////////////
// ResetTable.cpp
#include "stdafx.h"
#include "ResetTable.h"
#include "MemScript.h"

CResetTable gResetTable;

CResetTable::CResetTable() // OK
{
	this->m_count = 0;
}

CResetTable::~CResetTable() // OK
{
}

void CResetTable::Load(char* path) // OK
{
	CMemScript* lpScript = new(std::nothrow) CMemScript;

	if (lpScript == 0)
	{
		return;
	}

	if (lpScript->SetBuffer(path) == 0)
	{
		delete lpScript;
		return;
	}

	this->m_count = 0;

	try
	{
		while (true)
		{
			if (lpScript->GetToken() == 2)
			{
				break;
			}

			RESET_TABLE_INFO info;

			memset(&info, 0, sizeof(info));

			info.MinReset = lpScript->GetNumber();

			info.MaxReset = lpScript->GetAsNumber();

			info.Point = lpScript->GetAsNumber();

			this->SetInfo(info);
		}
	}
	catch (...)
	{
		MessageBox(0, path, "Error", MB_OK|MB_TOPMOST);
	}

	delete lpScript;
}

void CResetTable::SetInfo(RESET_TABLE_INFO info) // OK
{
	if (this->m_count < 0 || this->m_count >= MAX_RESET_TABLE)
	{
		return;
	}

	this->m_ResetTableInfo[this->m_count] = info;
	this->m_count++;
}

int CResetTable::GetResetPoint(LPOBJ lpObj) // OK
{
	int point = 0;

	for (int n = 0; n < this->m_count; n++)
	{
		for (int i = 0; i <= lpObj->m_PlayerData->m_iResets; i++)
		{
			if (i >= this->m_ResetTableInfo[n].MinReset && i <= this->m_ResetTableInfo[n].MaxReset)
			{
				point += this->m_ResetTableInfo[n].Point;
			}
		}
	}

	return point;
}


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

