// CustomEventDrop.cpp: implementation of the CCustomEventDrop class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "./Custom/CustomEventDrop/CustomEventDrop.h"
#include "DSProtocol.h"
#include "MapClass.h"
#include "MapServerManager.h"
#include "MemScript.h"
#include "TNotice.h"
#include "ScheduleManager.h"
#include "User.h"
#include "TLog.h"
#include "protocol.h"
#include "configread.h"
#include "MapAttribute.h"
#include "LargeRand.h"
#include "GameMain.h"
#include "gObjMonster.h"
#include "SetItemOption.h"
#include "Event.h"
#include "winutil.h"
#include "Notice.h"
#include "CustomMichi.h"

CCustomEventDrop gCustomEventDrop;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCustomEventDrop::CCustomEventDrop() // OK
{
	for (int n = 0; n < MAX_CUSTOM_EVENT_DROP; n++)
	{
		CUSTOM_EVENT_DROP_INFO* lpInfo = &this->m_CustomEventDropInfo[n];

		lpInfo->Index = n;
		lpInfo->State = CUSTOM_EVENT_DROP_STATE_BLANK;
		lpInfo->RemainTime = 0;
		lpInfo->TargetTime = 0;
		lpInfo->TickCount = GetTickCount();
		lpInfo->AlarmMinSave = -1;
		lpInfo->AlarmMinLeft = -1;
	}
}

CCustomEventDrop::~CCustomEventDrop() // OK
{

}

void CCustomEventDrop::Init() // OK
{
	for (int n = 0; n < MAX_CUSTOM_EVENT_DROP; n++)
	{
		if (this->m_CustomEventDropSwitch == 0)
		{
			this->SetState(&this->m_CustomEventDropInfo[n], CUSTOM_EVENT_DROP_STATE_BLANK);
		}
		else
		{
			this->SetState(&this->m_CustomEventDropInfo[n], CUSTOM_EVENT_DROP_STATE_EMPTY);
		}
	}
}

void CCustomEventDrop::Load(char* path) // OK
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

	for (int n = 0; n < MAX_CUSTOM_EVENT_DROP; n++)
	{
		this->m_CustomEventDropInfo[n].StartTime.clear();
		this->m_CustomEventDropInfo[n].RuleInfo.DropItem.clear();
	}

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

					this->m_CustomEventDropSwitch = lpMemScript->GetAsNumber();
				}
				else if (section == 1)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					CUSTOM_EVENT_DROP_START_TIME info;

					int index = lpMemScript->GetNumber();

					info.Year = lpMemScript->GetAsNumber();

					info.Month = lpMemScript->GetAsNumber();

					info.Day = lpMemScript->GetAsNumber();

					info.DayOfWeek = lpMemScript->GetAsNumber();

					info.Hour = lpMemScript->GetAsNumber();

					info.Minute = lpMemScript->GetAsNumber();

					info.Second = lpMemScript->GetAsNumber();

					this->m_CustomEventDropInfo[index].StartTime.push_back(info);
				}
				else if (section == 2)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					int index = lpMemScript->GetNumber();

					strcpy(this->m_CustomEventDropInfo[index].RuleInfo.Name, lpMemScript->GetAsString());

					this->m_CustomEventDropInfo[index].RuleInfo.Map = lpMemScript->GetAsNumber();

					this->m_CustomEventDropInfo[index].RuleInfo.X = lpMemScript->GetAsNumber();

					this->m_CustomEventDropInfo[index].RuleInfo.Y = lpMemScript->GetAsNumber();

					this->m_CustomEventDropInfo[index].RuleInfo.Range = lpMemScript->GetAsNumber();

					this->m_CustomEventDropInfo[index].RuleInfo.AlarmTime = lpMemScript->GetAsNumber();

					this->m_CustomEventDropInfo[index].RuleInfo.EventTime = lpMemScript->GetAsNumber();
				}
				else if (section == 3)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					CUSTOM_EVENT_DROP_ITEM_INFO info;

					int index = lpMemScript->GetNumber();

					info.ItemIndex = lpMemScript->GetAsNumber();

					info.ItemLevel = lpMemScript->GetAsNumber();

					info.Durability = lpMemScript->GetAsNumber();

					info.Skill = lpMemScript->GetAsNumber();

					info.Luck = lpMemScript->GetAsNumber();

					info.Option = lpMemScript->GetAsNumber();

					info.Excellent = lpMemScript->GetAsNumber();

					info.Ancient = lpMemScript->GetAsNumber();

					info.DropCount = lpMemScript->GetAsNumber();

					info.DropDelay = lpMemScript->GetAsNumber();

					info.DropState = 0;

					this->m_CustomEventDropInfo[index].RuleInfo.DropItem.push_back(info);
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

void CCustomEventDrop::MainProc() // OK
{
	for (int n = 0; n < MAX_CUSTOM_EVENT_DROP; n++)
	{
		CUSTOM_EVENT_DROP_INFO* lpInfo = &this->m_CustomEventDropInfo[n];

		if ((GetTickCount() - lpInfo->TickCount) >= 1000)
		{
			lpInfo->TickCount = GetTickCount();

			lpInfo->RemainTime = (int)difftime(lpInfo->TargetTime, time(0));

			switch (lpInfo->State)
			{
			case CUSTOM_EVENT_DROP_STATE_BLANK:
				this->ProcState_BLANK(lpInfo);
				break;
			case CUSTOM_EVENT_DROP_STATE_EMPTY:
				this->ProcState_EMPTY(lpInfo);
				break;
			case CUSTOM_EVENT_DROP_STATE_START:
				this->ProcState_START(lpInfo);
				break;
			}
		}
	}
}

void CCustomEventDrop::ProcState_BLANK(CUSTOM_EVENT_DROP_INFO* lpInfo) // OK
{

}

void CCustomEventDrop::ProcState_EMPTY(CUSTOM_EVENT_DROP_INFO* lpInfo) // OK
{
	if (lpInfo->RemainTime > 0 && lpInfo->RemainTime <= (lpInfo->RuleInfo.AlarmTime * 60))
	{
		if ((lpInfo->AlarmMinSave = (((lpInfo->RemainTime % 60) == 0) ? ((lpInfo->RemainTime / 60) - 1) : (lpInfo->RemainTime / 60))) != lpInfo->AlarmMinLeft)
		{
			lpInfo->AlarmMinLeft = lpInfo->AlarmMinSave;

			PMSG_NOTICE pNotice;
			TNotice::MakeNoticeMsgEx(&pNotice, 0, "[%s] Will Start in %d Minute(s)", lpInfo->RuleInfo.Name, (lpInfo->AlarmMinLeft + 1));
			TNotice::SendNoticeToAllUser(&pNotice);
			g_Log.AddC(TColor::Violet, "[%s] Will Start in %d Minute(s)", lpInfo->RuleInfo.Name, (lpInfo->AlarmMinLeft + 1));
		}
	}

	if (lpInfo->RemainTime <= 0)
	{
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx(&pNotice, 0, "[%s] Started", lpInfo->RuleInfo.Name);
		TNotice::SendNoticeToAllUser(&pNotice);
		g_Log.AddC(TColor::Aqua, "[%s] Started", lpInfo->RuleInfo.Name);
		this->SetState(lpInfo, CUSTOM_EVENT_DROP_STATE_START);
	}
}

void CCustomEventDrop::ProcState_START(CUSTOM_EVENT_DROP_INFO* lpInfo) // OK
{
	int ItemNumber = 0;
	int Option1 = 0;
	int Option2 = 0;
	int Option3 = 0;
	BYTE ExcOption = 0;
	BYTE SetOption = 0;

	for (std::vector<CUSTOM_EVENT_DROP_ITEM_INFO>::iterator it = lpInfo->RuleInfo.DropItem.begin(); it != lpInfo->RuleInfo.DropItem.end(); it++)
	{
		if (it->DropState == 0 && ((lpInfo->RuleInfo.EventTime * 60) - lpInfo->RemainTime) >= it->DropDelay)
		{
			for (int n = 0; n < it->DropCount; n++)
			{
				it->DropState = 1;

				// --------------
				if (it->Skill == 1)
				{
					if (rand() % 100 < 50)
					{
						Option1 = 1;
					}
				}
				//if (it->Skill == 1)
				//{
				//	if (rand() % 2)
				//	{
				//		Option1 = 1;
				//	}
				//}
				// ----
				if (it->Luck == 1)
				{
					if (rand() % 100 < 50)
					{
						Option2 = 1;
					}
				}
				//if (it->Luck != 0)
				//{
				//	if (rand() % 2)
				//	{
				//		Option2 = 1;
				//	}
				//}
				// ----
				if (it->Option != 0)
				{
					if (rand() % 5 == 0)
					{
						Option3 = 3;
					}
					else
					{
						Option3 = rand() % 3;
					}
				}
				// ----
				if (it->Excellent != 0)
				{
					ExcOption = g_Michi.IsNewOptionRand(0);
				}
				// ----

				if (it->Ancient != 0)
				{
					SetOption = gSetItemOption.GenSetOption(it->ItemIndex);
				}
				// ---------------
				BYTE X = lpInfo->RuleInfo.X;
				BYTE Y = lpInfo->RuleInfo.Y;
				// ----
				if (!gObjGetRandomItemDropLocation(lpInfo->RuleInfo.Map, X, Y, lpInfo->RuleInfo.Range, lpInfo->RuleInfo.Range, 50))
				{
					X = lpInfo->RuleInfo.X;
					Y = lpInfo->RuleInfo.Y;
				}

				ItemSerialCreateSend(this->GetDummyUserIndex(), lpInfo->RuleInfo.Map, X, Y, it->ItemIndex, it->ItemLevel, it->Durability, Option1, Option2, Option3, -1, ExcOption, SetOption, 0, 0, 0);
			}
		}
	}

	if (lpInfo->RemainTime <= 0)
	{
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, "[%s] Finished", lpInfo->RuleInfo.Name);
		this->SetState(lpInfo, CUSTOM_EVENT_DROP_STATE_EMPTY);
	}
}

void CCustomEventDrop::SetState(CUSTOM_EVENT_DROP_INFO* lpInfo, int state) // OK
{
	switch ((lpInfo->State = state))
	{
	case CUSTOM_EVENT_DROP_STATE_BLANK:
		this->SetState_BLANK(lpInfo);
		break;
	case CUSTOM_EVENT_DROP_STATE_EMPTY:
		this->SetState_EMPTY(lpInfo);
		break;
	case CUSTOM_EVENT_DROP_STATE_START:
		this->SetState_START(lpInfo);
		break;
	}
}

void CCustomEventDrop::SetState_BLANK(CUSTOM_EVENT_DROP_INFO* lpInfo) // OK
{

}

void CCustomEventDrop::SetState_EMPTY(CUSTOM_EVENT_DROP_INFO* lpInfo) // OK
{
	lpInfo->AlarmMinSave = -1;
	lpInfo->AlarmMinLeft = -1;

	for (std::vector<CUSTOM_EVENT_DROP_ITEM_INFO>::iterator it = lpInfo->RuleInfo.DropItem.begin(); it != lpInfo->RuleInfo.DropItem.end(); it++)
	{
		it->DropState = 0;
	}

	this->CheckSync(lpInfo);
}

void CCustomEventDrop::SetState_START(CUSTOM_EVENT_DROP_INFO* lpInfo) // OK
{
	lpInfo->AlarmMinSave = -1;
	lpInfo->AlarmMinLeft = -1;

	lpInfo->RemainTime = lpInfo->RuleInfo.EventTime * 60;

	lpInfo->TargetTime = (int)(time(0) + lpInfo->RemainTime);
}

void CCustomEventDrop::CheckSync(CUSTOM_EVENT_DROP_INFO* lpInfo) // OK
{
	if (lpInfo->StartTime.empty() != 0)
	{
		this->SetState(lpInfo, CUSTOM_EVENT_DROP_STATE_BLANK);
		return;
	}

	CTime ScheduleTime;

	CScheduleManager ScheduleManager;

	for (std::vector<CUSTOM_EVENT_DROP_START_TIME>::iterator it = lpInfo->StartTime.begin(); it != lpInfo->StartTime.end(); it++)
	{
		ScheduleManager.AddSchedule(it->Year, it->Month, it->Day, it->Hour, it->Minute, it->Second, it->DayOfWeek);
	}

	if (ScheduleManager.GetSchedule(&ScheduleTime) == 0)
	{
		this->SetState(lpInfo, CUSTOM_EVENT_DROP_STATE_BLANK);
		return;
	}

	lpInfo->RemainTime = (int)difftime(ScheduleTime.GetTime(), time(0));

	lpInfo->TargetTime = (int)ScheduleTime.GetTime();
}

LONG CCustomEventDrop::GetDummyUserIndex() // OK
{
	for (int n = g_ConfigRead.server.GetObjectStartUserIndex(); n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		if (gObjIsConnectedGP(n) != 0)
		{
			return n;
		}
	}

	return g_ConfigRead.server.GetObjectStartUserIndex();
}

bool::CCustomEventDrop::GetRandomItemDropLocation(int map, int* ox, int* oy, int tx, int ty, int count) // OK
{
	int x = (*ox);
	int y = (*oy);

	tx = ((tx<1) ? 1 : tx);
	ty = ((ty<1) ? 1 : ty);

	for (int n = 0; n < count; n++)
	{
		(*ox) = ((GetLargeRand() % (tx + 1))*((GetLargeRand() % 2 == 0) ? -1 : 1)) + x;
		(*oy) = ((GetLargeRand() % (ty + 1))*((GetLargeRand() % 2 == 0) ? -1 : 1)) + y;

		if (MapC[map].GetAttr((*ox), (*oy)) == 0 && MapC[map].GetAttr((*ox), (*oy)) == 0)
		{
			return 1;
		}
	}

	return 0;
}


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

