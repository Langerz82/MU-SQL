////////////////////////////////////////////////////////////////////////////////
// QuestInfo.cpp
// ------------------------------
// Decompiled by Deathway
// Date : 2007-05-09
// ------------------------------
#include "StdAfx.h"
#include "QuestInfo.h"
#include "Logging/Log.h"
#include "Main.h"
#include "GameProtocol.h"
#include "gObjMonster.h"

#include "configread.h"
#include "MasterLevelSkillTreeSystem.h"
#include "util.h"

// GS-N 0.99.60T 0x0046EBF0
//	GS-N	1.00.18	JPN	0x00485BA0	-	Completed

//CQuestInfo::QuestClear Weird type of compilation
BYTE QuestBitMask[8];
CQuestInfo g_QuestInfo;



char sQuestString[4][50] = { "None", "Accept", "Clear", "None"};	// THIS IS NOT THE PLACE OF THIS VARIABLE



CQuestInfo::CQuestInfo()
{
	QuestBitMask[0]=0xFC;
	QuestBitMask[1]=0xFC;
	QuestBitMask[2]=0xF3;
	QuestBitMask[3]=0xF3;
	QuestBitMask[4]=0xCF;
	QuestBitMask[5]=0xCF;
	QuestBitMask[6]=0x3F;
	QuestBitMask[7]=0x3F;

	this->Init();
}

CQuestInfo::~CQuestInfo()
{
	return;
}

void CQuestInfo::Init()
{
	this->m_QuestCount = 0;
	memset(this->QuestInfo, -1, sizeof(this->QuestInfo));

	for ( int i=0;i<MAX_QUEST_INFO;i++)
	{
		this->QuestInfo[i].QuestConditionCount=0;
		this->QuestInfo[i].QuestSubInfoCount= 0;
	}
}

BOOL CQuestInfo::LoadQuestInfo(char * filename)
{
	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(filename);

	if (res.status != pugi::status_ok)
	{
		sLog->outError("load error %s (%s)", filename, res.description());
		return false;
	}

	this->Init();

	pugi::xml_node mainXML = file.child("ClassQuest");

	for (pugi::xml_node quest_info = mainXML.child("QuestInfo"); quest_info; quest_info = quest_info.next_sibling())
	{
		int iQuestIndex = quest_info.attribute("Index").as_int();

		if (iQuestIndex < 0 || iQuestIndex >= MAX_QUEST_INFO)
		{
			sLog->outError("Error in %s - iQuestIndex out of range (%d)", __FUNCTION__, iQuestIndex);
			continue;
		}

		this->QuestInfo[iQuestIndex].QuestIndex = iQuestIndex;
		this->QuestInfo[iQuestIndex].QuestStartType = quest_info.attribute("Type").as_int();
		this->QuestInfo[iQuestIndex].QuestStartSubType = quest_info.attribute("SubType").as_int();
		strcpy(this->QuestInfo[iQuestIndex].Name, quest_info.attribute("Name").as_string());

		this->QuestInfo[iQuestIndex].QuestSubInfoCount = 0;
		this->QuestInfo[iQuestIndex].QuestConditionCount = 0;

		pugi::xml_node quest_job = quest_info.child("QuestJob");

		for (pugi::xml_node job = quest_job.child("Job"); job; job = job.next_sibling())
		{
			int iSubInfoCount = this->QuestInfo[iQuestIndex].QuestSubInfoCount;

			this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].QuestType				= job.attribute("Type").as_int();
			this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].NeedType				= job.attribute("ReqItemCat").as_int();
			this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].NeedSubType			= job.attribute("ReqItemIndex").as_int();
			this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].ItemLevel				= job.attribute("ItemLevel").as_int();
			this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].NeedNumber				= job.attribute("ItemCount").as_int();
			this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].NeedTargetMinLevel		= job.attribute("MonsterMinLevel").as_int();
			this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].NeedTargetMaxLevel		= job.attribute("MonsterMaxLevel").as_int();
			this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].NeedDropRate			= job.attribute("DropRate").as_int();
			this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RewardType				= job.attribute("RewardType").as_int();
			this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RewardSubType			= job.attribute("RewardSubType").as_int();
			this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RewardCount			= job.attribute("RewardCount").as_int();
			this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].LinkConditionIndex		= job.attribute("LinkConditionIndex").as_int();
			this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RequireClass[0]		= job.attribute("DarkWizard").as_int();
			this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RequireClass[1]		= job.attribute("DarkKnight").as_int();
			this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RequireClass[2]		= job.attribute("FairyElf").as_int();
			this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RequireClass[3]		= job.attribute("MagicGladiator").as_int();
			this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RequireClass[4]		= job.attribute("DarkLord").as_int();
			this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RequireClass[5]		= job.attribute("Summoner").as_int();
			this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RequireClass[6]		= job.attribute("RageFighter").as_int();
			this->QuestInfo[iQuestIndex].QuestSubInfo[iSubInfoCount].RequireClass[7]		= job.attribute("GrowLancer").as_int();
			this->QuestInfo[iQuestIndex].QuestSubInfoCount++;
		}

		pugi::xml_node quest_condition = quest_info.child("QuestCondition");

		for (pugi::xml_node condition = quest_condition.child("Condition"); condition; condition = condition.next_sibling())
		{
			int iConditionCount = this->QuestInfo[iQuestIndex].QuestConditionCount;

			this->QuestInfo[iQuestIndex].QuestCondition[iConditionCount].Index				= condition.attribute("Type").as_int();
			this->QuestInfo[iQuestIndex].QuestCondition[iConditionCount].NeedQuestIndex		= condition.attribute("Index").as_int();
			this->QuestInfo[iQuestIndex].QuestCondition[iConditionCount].MinLevel			= condition.attribute("MinLevel").as_int();
			this->QuestInfo[iQuestIndex].QuestCondition[iConditionCount].MaxLevel			= condition.attribute("MaxLevel").as_int();
			this->QuestInfo[iQuestIndex].QuestCondition[iConditionCount].ReqStr				= condition.attribute("ReqStrength").as_int();
			this->QuestInfo[iQuestIndex].QuestCondition[iConditionCount].NeedZen			= condition.attribute("ReqMoney").as_int();
			this->QuestInfo[iQuestIndex].QuestCondition[iConditionCount].StartContext		= condition.attribute("StartContext").as_int();
			this->QuestInfo[iQuestIndex].QuestConditionCount++;
		}

		//sLog->outBasic("[Quest] (%s) Load : Index %d, SubInfoCount %d, ConditionCount %d",
		//	this->QuestInfo[iQuestIndex].Name, iQuestIndex, this->QuestInfo[iQuestIndex].QuestSubInfoCount, this->QuestInfo[iQuestIndex].QuestConditionCount);

		this->m_QuestCount++;
	}
					
	this->InitQuestItem();
	return true;
}

void CQuestInfo::InitQuestItem()
{
	int questcount = this->GetQeustCount();
	int foundquest = 0;
	LPQUEST_INFO lpQuestInfo;
	LPQUEST_SUB_INFO lpSubInfo;

	for (int i=0;i<MAX_QUEST_INFO;i++)
	{
		lpQuestInfo = this->GetQuestInfo(i);

		if ( lpQuestInfo == NULL )
		{
			continue;
		}

		for ( int n=0;n<lpQuestInfo->QuestSubInfoCount;n++)
		{
			lpSubInfo = &lpQuestInfo->QuestSubInfo[n];

			if ( lpSubInfo != NULL )
			{
				if ( lpSubInfo->QuestType == 1 )
				{
					LPITEM_ATTRIBUTE p = &ItemAttribute[ITEMGET(lpSubInfo->NeedType, lpSubInfo->NeedSubType) ];
					p->QuestItem = TRUE;
					//sLog->outBasic("[Quest] [SetQuestItem] %s", p->Name);
				}
			}
		}
		foundquest++;
		if ( foundquest == questcount )
		{
			return;
		}
	}
}

BOOL CQuestInfo::IsQuest(int QuestIndex)
{
	if ( QuestIndex < 0 || QuestIndex > MAX_QUEST_INFO )
	{
		return false;
	}

	if ( this->QuestInfo[QuestIndex].QuestIndex == -1 )
	{
		return false;
	}

	return true;
}

					
int CQuestInfo::GetQuestState(CGameObject &Obj, int QuestIndex)
{
	if ( QuestIndex < 0 || QuestIndex > MAX_QUEST_INFO )
	{
		return false;
	}

	if(Obj.Type != OBJ_USER)
	{
		return false;
	}

	int index = (QuestIndex) /4;
	int shift = (QuestIndex % 4) *2;
	return (Obj.m_PlayerData->m_Quest[index] >> shift) &3;
}

BYTE CQuestInfo::GetQuestStateBYTE(CGameObject &Obj, int QuestIndex)
{
	if ( QuestIndex < 0 || QuestIndex > MAX_QUEST_INFO )
	{
		return false;
	}

	if(Obj.Type != OBJ_USER)
	{
		return false;
	}
	
	int index = (QuestIndex / 4);
	return Obj.m_PlayerData->m_Quest[index];
}

BYTE CQuestInfo::SetQuestState(CGameObject &Obj, int QuestIndex, int State)
{
	if ( QuestIndex < 0 || QuestIndex > MAX_QUEST_INFO )
	{
		return -1;
	}

	if ( Obj.Type != OBJ_USER )
	{
		return -1;
	}

	int QuestState = this->GetQuestState(lpObj, QuestIndex);

	if ( QuestState == 3 || QuestState == 0)
	{
		BYTE btCond = this->QuestRunConditionCheck(lpObj, QuestIndex);

		if ( btCond != 0 )
		{
			return btCond;
		}

		this->QuestAccept(lpObj, QuestIndex);
		//sLog->outBasic("[Quest] AcceptQuest [%s][%s] (%s)", Obj.AccountID, Obj.Name,this->GetQuestInfo(QuestIndex)->Name);
		State = 1;
	}
	else if ( QuestState == 1 )
	{
		BYTE btCond = this->QuestClearConditionCheck(lpObj, QuestIndex);

		if ( btCond != 0 )
		{
			return btCond;
		}

		this->QuestClear(lpObj, QuestIndex);
		State = 2;
		//sLog->outBasic("[Quest] ClearQuest [%s][%s] (%s)", Obj.AccountID, Obj.Name,this->GetQuestInfo(QuestIndex)->Name);
	}
	else
	{
		return -1;
	}

	int index = (QuestIndex / 4);
	int shift =  (QuestIndex % 4) *2;
	Obj.m_PlayerData->m_Quest[index] &= QuestBitMask[shift];
	Obj.m_PlayerData->m_Quest[index] |= (State&3) << shift;
	return 0;
}

BYTE CQuestInfo::ReSetQuestState(CGameObject &Obj, int QuestIndex)
{
	if(Obj.Type != OBJ_USER)
	{
		return -1;
	}

	int State = 3;
	int index = (QuestIndex/4);
	int shift = (QuestIndex % 4)*2;
	Obj.m_PlayerData->m_Quest[index] &= QuestBitMask[shift];
	Obj.m_PlayerData->m_Quest[index] |= (State&3) << shift;

	return 0;
}

LPQUEST_INFO CQuestInfo::GetQuestInfo(int QuestIndex)
{
	if ( QuestIndex < 0 || QuestIndex > MAX_QUEST_INFO )
	{
		return false;
	}
	
	if ( this->QuestInfo[QuestIndex].QuestIndex == -1 )
	{
		return NULL;
	}

	return &this->QuestInfo[QuestIndex];
}

LPQUEST_SUB_INFO CQuestInfo::GetSubquestInfo(CGameObject &Obj, LPQUEST_INFO lpQuestInfo, int subquestindex)
{
	if ( lpQuestInfo == NULL )
	{
		return NULL;
	}

	if ( subquestindex < 0 || subquestindex > MAX_SUBINFO_QUEST )
	{
		return NULL;
	}

	if ( Obj.Type != OBJ_USER )
	{
		return NULL;
	}

	LPQUEST_SUB_INFO lpSubInfo = &lpQuestInfo->QuestSubInfo[subquestindex];
	int Class = Obj.Class;
	int ChangeUP = Obj.m_PlayerData->ChangeUP;
	int requireclass = lpSubInfo->RequireClass[Class];

	if ( requireclass == 0 )
	{
		return NULL;
	}

	if ( requireclass > 1 )
	{
		if ( requireclass > (ChangeUP + 1) )
		{
			return NULL;
		}
	}

	return lpSubInfo;
}

LPQUEST_CONDITION CQuestInfo::GetQuestCondition(LPQUEST_INFO lpQuestInfo, int conditionindex)
{
	if ( lpQuestInfo == NULL )
	{
		return NULL;
	}

	if ( conditionindex < 0 || conditionindex > MAX_CONDITION_QUEST )
	{
		return NULL;
	}

	return &lpQuestInfo->QuestCondition[conditionindex];
}

BYTE CQuestInfo::QuestClearConditionCheck(CGameObject &Obj, int QuestIndex)
{
	LPQUEST_INFO lpQuestInfo = this->GetQuestInfo(QuestIndex);

	if ( lpQuestInfo == NULL )
	{
		return -1;
	}

	int subquestcount = lpQuestInfo->QuestSubInfoCount;
	int concount = lpQuestInfo->QuestConditionCount;
	LPQUEST_SUB_INFO lpSubInfo;
	BOOL bFoundSubQuest = FALSE;
	

	for ( int subquest=0;subquest<subquestcount;subquest++)
	{
		lpSubInfo = this->GetSubquestInfo(lpObj, lpQuestInfo, subquest);

		if ( lpSubInfo != NULL )
		{
			bFoundSubQuest = TRUE;

			if ( lpSubInfo->QuestType == 1 )
			{
				int NumberItemQuestFound = gObjGetItemCountInIventory(Obj.m_Index, lpSubInfo->NeedType, lpSubInfo->NeedSubType, lpSubInfo->ItemLevel);

				if ( NumberItemQuestFound < lpSubInfo->NeedNumber)
				{
					return 1;
				}
			}
			else if ( lpSubInfo->QuestType == 2)
			{
				int NumberMonsterKillCount = this->GetQuestKillCount(lpObj, lpSubInfo->NeedType);

				if ( NumberMonsterKillCount < lpSubInfo->NeedNumber)
				{
					return 1;
				}
			}
		}
	}

	if ( bFoundSubQuest == FALSE )
	{
		return -1;
	}

	return 0;
}

int CQuestInfo::GetQuestKillCount(CGameObject &Obj, int uClass)
{
	if(Obj.Type != OBJ_USER)
	{
		return 0;
	}

	int iQuestState_1 = this->GetQuestState(lpObj, 5);
	int iQuestState_2 = this->GetQuestState(lpObj, 6);

	if (iQuestState_1 == 1 || iQuestState_2 == 1)
	{
		return Obj.m_PlayerData->m_Quest[(uClass%100)-5];
	}
	else
	{
		return 0;
	}
}

BYTE CQuestInfo::QuestRunConditionCheck(CGameObject &Obj, int QuestIndex)
{
	LPQUEST_INFO lpQuestInfo = this->GetQuestInfo(QuestIndex);

	if ( lpQuestInfo == NULL )
	{
		return -1;
	}

	int subquestcount = lpQuestInfo->QuestSubInfoCount;
	int concount = lpQuestInfo->QuestConditionCount;
	LPQUEST_SUB_INFO lpSubInfo;
	LPQUEST_CONDITION lpCondition;
	BOOL bFoundSubQuest = FALSE;

	for ( int subquest=0;subquest<subquestcount;subquest++)
	{
		lpSubInfo = this->GetSubquestInfo(lpObj, lpQuestInfo, subquest);

		if ( lpSubInfo != NULL )
		{
			bFoundSubQuest = TRUE;

			for ( int i=0;i<concount;i++)
			{
				lpCondition = this->GetQuestCondition(lpQuestInfo, i);

				if ( lpCondition != NULL && lpCondition->Index == -1)
				{
					if ( this->CompareCondition(lpObj, lpCondition) == FALSE )
					{
						return lpCondition->StartContext;
					}
				}
				else if ( lpCondition != NULL && lpCondition->Index == lpSubInfo->LinkConditionIndex )
				{
					if ( this->CompareCondition(lpObj, lpCondition) == 0)
					{
						return lpCondition->StartContext;
					}
				}
				else if ( lpCondition == NULL )
				{
					return -1;
				}
			}

		}
	}

	if ( bFoundSubQuest == FALSE )
	{
		return -1;
	}

	return 0;
}

BOOL CQuestInfo::CompareCondition(CGameObject &Obj, LPQUEST_CONDITION lpCondition)
{
	if ( Obj.Type != OBJ_USER )
	{
		return FALSE;
	}

	if ( lpCondition->NeedQuestIndex != -1 )
	{
		if ( this->GetQuestState(lpObj, lpCondition->NeedQuestIndex) != 2 )
		{
			return FALSE;
		}
	}

	if ( lpCondition->MinLevel != 0 )
	{
		if ( lpCondition->MinLevel > Obj.Level )
		{
			return FALSE;
		}
	}

	if ( lpCondition->MaxLevel != 0 )
	{
		if ( lpCondition->MaxLevel < Obj.Level )
		{
			return FALSE;
		}
	}

	if ( lpCondition->NeedZen > Obj.m_PlayerData->Money )
	{
		return FALSE;
	}

	if ( lpCondition->ReqStr > Obj.m_PlayerData->Strength + Obj.AddStrength)
	{
		return FALSE;
	}

	return TRUE;

}



BOOL CQuestInfo::NpcTalk(CGameObject &Npc, CGameObject lpObj)
{
	int questcount = this->GetQeustCount();
	int foundquest = 0;
	LPQUEST_INFO lpQuestInfo;
	int queststate = -1;
	int questindex = -1;

	for ( int i = 0; i<MAX_QUEST_INFO;i++)
	{
		lpQuestInfo = this->GetQuestInfo(i);

		if ( lpQuestInfo == NULL )
		{
			continue;
		}

		if ( lpQuestInfo->QuestStartType == 2 ) // NPC Click
		{
			if ( lpNpc->Class == lpQuestInfo->QuestStartSubType)
			{
				if ( queststate == -1 )
				{
					questindex = lpQuestInfo->QuestIndex;
					queststate = this->GetQuestState(lpObj, lpQuestInfo->QuestIndex);
				}
				else if ( queststate == 2 )
				{
					for ( int n=0;n<lpQuestInfo->QuestSubInfoCount ;n++)
					{
						LPQUEST_SUB_INFO lpSubInfo = this->GetSubquestInfo(lpObj, lpQuestInfo, n);

						if ( lpSubInfo != NULL )
						{
							questindex = lpQuestInfo->QuestIndex;
							queststate = this->GetQuestState(lpObj, lpQuestInfo->QuestIndex);
						}
					}
				}
			}
		}

		foundquest++;

		if ( foundquest == questcount )
		{
			break;
		}
	}

	if ( queststate != -1 )
	{
		this->SendQuestMonsterKill(lpObj, questindex);
		GSProtocol.GCSendQuestInfo(Obj.m_Index, questindex);
		Obj.m_IfState->type = 10;
		Obj.m_IfState->state = 0;
		Obj.m_IfState->use = 1;
		return true;
	}

	return false;
}


BOOL CQuestInfo::MonsterItemDrop(CGameObject &Obj)
{
	int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

	if ( MaxHitUser == -1 )
	{
		return false;
	}

	CGameObject lpTarget = getGameObject(MaxHitUser);
	int questcount = this->GetQeustCount();
	int foundquest = 0;
	LPQUEST_INFO lpQuestInfo;
	LPQUEST_SUB_INFO lpSubInfo;
	int type;
	int level;
	int x;
	int y;
	float dur = 0;
	int Option1 = 0;
	int Option2 = 0;
	int Option3 = 0;

	if(gParty.GetPartyCount(lpTarget->PartyNumber) > 0)
	{ 
		return this->MonsterItemDropParty(lpObj, lpTarget);
	}

	for ( int i=0;i<MAX_QUEST_INFO;i++)
	{
		lpQuestInfo = this->GetQuestInfo(i);

		if ( lpQuestInfo == NULL )
		{
			continue;
		}

		for ( int n =0;n<lpQuestInfo->QuestSubInfoCount;n++)
		{
			lpSubInfo = this->GetSubquestInfo(lpTarget, lpQuestInfo, n);
			
			if ( lpSubInfo != NULL )
			{
				if ( lpSubInfo->QuestType == 1 )
				{
					if ( (Obj.Level >= lpSubInfo->NeedTargetMinLevel && Obj.Level <= lpSubInfo->NeedTargetMaxLevel && lpSubInfo->NeedTargetMinLevel != -1) || (lpSubInfo->NeedTargetMinLevel == -1 && lpSubInfo->NeedTargetMaxLevel == Obj.Class) )
					{
						if ( this->GetQuestState(lpTarget, lpQuestInfo->QuestIndex) == TRUE )
						{
							if ( (rand() % ITEM_QUEST_DROP_PROBABILITY) < lpSubInfo->NeedDropRate)
							{
								int itemcount = gObjGetItemCountInIventory(MaxHitUser, lpSubInfo->NeedType,
									lpSubInfo->NeedSubType, lpSubInfo->ItemLevel);
																		
								if ( itemcount >= lpSubInfo->NeedNumber)
								{
									continue;
								}
																												
								dur = 0;
								x = Obj.X;
								y = Obj.Y;
								level = lpSubInfo->ItemLevel;
								type = ItemGetNumberMake(lpSubInfo->NeedType, lpSubInfo->NeedSubType);
								ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, level, dur, Option1,
									Option2, Option3, lpTarget->m_Index, 0, 0, 0, 0, 0);
								//sLog->outBasic("[Quest] Quest Item Drop [%s]: [%s][%s] (%s) (%d,%d)", Obj.Name,
								//	lpTarget->AccountID, lpTarget->Name, lpQuestInfo->Name, lpSubInfo->NeedType,	lpSubInfo->NeedSubType);
								return true;
							}				
						}
					}
				}
			}
		}

		foundquest++;

		if ( foundquest == questcount )
		{
			break;
		}
	}

	return false;
}

BOOL CQuestInfo::MonsterItemDropParty(CGameObject &Obj, CGameObject lpTargetObj)
{
	int questcount = this->GetQeustCount();
	int foundquest = 0;
	LPQUEST_INFO lpQuestInfo;
	LPQUEST_SUB_INFO lpSubInfo;
	int type;
	int level;
	int x;
	int y;
	float dur = 0;
	int Option1 = 0;
	int Option2 = 0;
	int Option3 = 0;

	for(int i=0;i<MAX_USER_IN_PARTY;i++)
	{
		CGameObject &Obj = gParty.m_PartyS[lpTargetObj.PartyNumber].Number[i];

		if(Obj.m_Index >= 0 && Obj.MapNumber == lpTargetObj.MapNumber)
		{
			CGameObject lpPartyObj = getGameObject(Obj.m_Index);

			for ( int j=0;j<MAX_QUEST_INFO;j++)
			{
				lpQuestInfo = this->GetQuestInfo(j);

				if ( lpQuestInfo == NULL )
				{
					continue;
				}

				for ( int n =0;n<lpQuestInfo->QuestSubInfoCount;n++)
				{
					lpSubInfo = this->GetSubquestInfo(lpPartyObj, lpQuestInfo, n);
			
					if ( lpSubInfo != NULL )
					{
						if ( lpSubInfo->QuestType == 1 )
						{
							if ( (Obj.Level >= lpSubInfo->NeedTargetMinLevel && Obj.Level <= lpSubInfo->NeedTargetMaxLevel && lpSubInfo->NeedTargetMinLevel != -1) || (lpSubInfo->NeedTargetMinLevel == -1 && lpSubInfo->NeedTargetMaxLevel == Obj.Class) )
							{
								if ( this->GetQuestState(lpPartyObj, lpQuestInfo->QuestIndex) == TRUE )
								{
									if ( (rand() % ITEM_QUEST_DROP_PROBABILITY) < lpSubInfo->NeedDropRate)
									{
										int itemcount = gObjGetItemCountInIventory(Obj.m_Index, lpSubInfo->NeedType,
											lpSubInfo->NeedSubType, lpSubInfo->ItemLevel);
																		
										if ( itemcount >= lpSubInfo->NeedNumber)
										{
											continue;
										}
																												
										dur = 0;
										x = Obj.X;
										y = Obj.Y;
										level = lpSubInfo->ItemLevel;
										type = ItemGetNumberMake(lpSubInfo->NeedType, lpSubInfo->NeedSubType);
										ItemCreate(Obj.m_Index, Obj.MapNumber, x, y, type, level, dur, Option1,
											Option2, Option3, Obj.m_Index, 0, 0, 0, 0, 0);
										//sLog->outBasic("[Quest] Quest Item Drop(Party) [%s]: [%s][%s] (%s) (%d,%d)", Obj.Name,
										//	lpPartyObj.AccountID, lpPartyObj.Name, lpQuestInfo->Name, lpSubInfo->NeedType,lpSubInfo->NeedSubType);
										return true;
									}
								}
							}
						}
					}
				}

				foundquest++;

				if( foundquest == questcount )
				{
					break;
				}
			}
		}
	}

	return false;
}

BOOL CQuestInfo::AddMonsterKillCount(CGameObject &Obj)
{
	int MaxHitUser = gObjMonsterTopHitDamageUser(lpObj);

	if ( MaxHitUser == -1 )
	{
		return false;
	}

	CGameObject lpTarget = getGameObject(MaxHitUser);
	int questcount = this->GetQeustCount();
	int foundquest = 0;
	LPQUEST_INFO lpQuestInfo;
	LPQUEST_SUB_INFO lpSubInfo;
	int type;
	int level;
	int x;
	int y;
	float dur = 0;
	int Option1 = 0;
	int Option2 = 0;
	int Option3 = 0;

	if(lpTarget->Type != OBJ_USER)
	{
		return false;
	}

	if(gParty.GetPartyCount(lpTarget->PartyNumber) > 0)
	{
		for(int i=0;i<MAX_USER_IN_PARTY;i++)
		{
			CGameObject &Obj = gParty.m_PartyS[lpTarget->PartyNumber].Number[i];

			if(Obj.m_Index >= 0 && Obj.MapNumber == lpTarget->MapNumber)
			{
				CGameObject lpPartyObj = getGameObject(Obj.m_Index);

				if(lpPartyObj.Type != OBJ_USER)
				{
					continue;
				}

				for ( int j=0;j<MAX_QUEST_INFO;j++)
				{
					lpQuestInfo = this->GetQuestInfo(j);

					if ( lpQuestInfo == NULL )
					{	
						continue;
					}

					for ( int n =0;n<lpQuestInfo->QuestSubInfoCount;n++)
					{
						lpSubInfo = this->GetSubquestInfo(lpPartyObj, lpQuestInfo, n);
			
						if ( lpSubInfo != NULL )
						{
							if ( lpSubInfo->QuestType == 2 )
							{
								if ( Obj.Class == lpSubInfo->NeedType && this->GetQuestState(lpPartyObj, lpQuestInfo->QuestIndex) == TRUE )
								{
									switch ( lpSubInfo->NeedType )
									{
										case 409:
											if(lpPartyObj.m_PlayerData->m_Quest[20] < lpSubInfo->NeedNumber)
											{
												lpPartyObj.m_PlayerData->m_Quest[20]++;
												if(g_ConfigRead.data.common.ThirdQuestCounter)
												{
													MsgOutput(lpPartyObj.m_Index, Lang.GetText(0,540), lpPartyObj.m_PlayerData->m_Quest[20], lpSubInfo->NeedNumber);
												}
											}
											break;
										case 410:
											if(lpPartyObj.m_PlayerData->m_Quest[21] < lpSubInfo->NeedNumber)
											{
												lpPartyObj.m_PlayerData->m_Quest[21]++;
												if(g_ConfigRead.data.common.ThirdQuestCounter)
												{
													MsgOutput(lpPartyObj.m_Index, Lang.GetText(0,541), lpPartyObj.m_PlayerData->m_Quest[21], lpSubInfo->NeedNumber);
												}
											}
											break;
										case 411:
											if(lpPartyObj.m_PlayerData->m_Quest[22] < lpSubInfo->NeedNumber)
											{
												lpPartyObj.m_PlayerData->m_Quest[22]++;
												if(g_ConfigRead.data.common.ThirdQuestCounter)
												{
													MsgOutput(lpPartyObj.m_Index, Lang.GetText(0,542), lpPartyObj.m_PlayerData->m_Quest[22], lpSubInfo->NeedNumber);
												}
											}
											break;
										case 412:
											if(lpPartyObj.m_PlayerData->m_Quest[24] < lpSubInfo->NeedNumber)
											{
												lpPartyObj.m_PlayerData->m_Quest[24]++;
												if(g_ConfigRead.data.common.ThirdQuestCounter)
												{
													MsgOutput(lpPartyObj.m_Index, Lang.GetText(0,543), lpPartyObj.m_PlayerData->m_Quest[24], lpSubInfo->NeedNumber);
												}
											}
											break;
									}

								}
							}
						}
					}

					foundquest++;
					if(foundquest == questcount)
					{
						break;
					}
				}
			}
		}
	}

	else
	{
		for ( int i=0;i<MAX_QUEST_INFO;i++)
		{
			lpQuestInfo = this->GetQuestInfo(i);

			if ( lpQuestInfo == NULL )
			{
				continue;
			}

			for ( int n =0;n<lpQuestInfo->QuestSubInfoCount;n++)
			{
				lpSubInfo = this->GetSubquestInfo(lpTarget, lpQuestInfo, n);
			
				if ( lpSubInfo != NULL )
				{
					if ( lpSubInfo->QuestType == 2 )
					{

						if ( Obj.Class == lpSubInfo->NeedType && this->GetQuestState(lpTarget, lpQuestInfo->QuestIndex) == TRUE )
						{
							switch ( lpSubInfo->NeedType )
							{
								case 409:
									if(lpTarget->m_PlayerData->m_Quest[20] < lpSubInfo->NeedNumber)
									{
										lpTarget->m_PlayerData->m_Quest[20]++;
										if(g_ConfigRead.data.common.ThirdQuestCounter)
										{
											MsgOutput(lpTarget->m_Index, Lang.GetText(0,540), lpTarget->m_PlayerData->m_Quest[20], lpSubInfo->NeedNumber);
										}
										
										return true;
									}
									break;
									case 410:
										if(lpTarget->m_PlayerData->m_Quest[21] < lpSubInfo->NeedNumber)
										{
											lpTarget->m_PlayerData->m_Quest[21]++;
											if(g_ConfigRead.data.common.ThirdQuestCounter)
											{
												MsgOutput(lpTarget->m_Index, Lang.GetText(0,541), lpTarget->m_PlayerData->m_Quest[21], lpSubInfo->NeedNumber);
											}

											return true;
										}
										break;
									case 411:
										if(lpTarget->m_PlayerData->m_Quest[22] < lpSubInfo->NeedNumber)
										{
											lpTarget->m_PlayerData->m_Quest[22]++;
											if(g_ConfigRead.data.common.ThirdQuestCounter)
											{
												MsgOutput(lpTarget->m_Index, Lang.GetText(0,542), lpTarget->m_PlayerData->m_Quest[22], lpSubInfo->NeedNumber);
											}
											
											return true;
										}
										break;
									case 412:
										if(lpTarget->m_PlayerData->m_Quest[24] < lpSubInfo->NeedNumber)
										{
											lpTarget->m_PlayerData->m_Quest[24]++;
											if(g_ConfigRead.data.common.ThirdQuestCounter)
											{
												MsgOutput(lpTarget->m_Index, Lang.GetText(0,543), lpTarget->m_PlayerData->m_Quest[24], lpSubInfo->NeedNumber);
											}

											return true;
										}
										break;
							}
						}
					}
				}
			}

			foundquest++;
			if(foundquest == questcount)
			{
				break;
			}
		}
	}

	return false;
}



BOOL CQuestInfo::QuestAccept(CGameObject &Obj, int QuestIndex)
{
	LPQUEST_INFO lpQuestInfo = this->GetQuestInfo(QuestIndex);

	if ( lpQuestInfo == NULL )
	{
		return 0xFF;
	}

	if ( Obj.Type != OBJ_USER )
	{
		return 0xFF;
	}

	int subquestcount = lpQuestInfo->QuestSubInfoCount;
	int concount = lpQuestInfo->QuestConditionCount;
	LPQUEST_SUB_INFO lpSubInfo;
	LPQUEST_CONDITION lpCondition;
	int NeedZen = 0;

	if ( QuestIndex > 3 && QuestIndex < 6 ) //Third Quest Fix
	{
		//Begin
		if( lpQuestInfo->QuestSubInfoCount > 0 )
		{
			lpSubInfo = lpQuestInfo->QuestSubInfo;

			if ( lpSubInfo != NULL )
			{
				for ( int n = 0;n<concount;n++)
				{
					lpCondition = this->GetQuestCondition(lpQuestInfo, n);
					NeedZen += lpCondition->NeedZen;
				}
			}
		}

		if ( QuestIndex > 4 && QuestIndex < 7 ) //Third Quest Fix
		{
			Obj.m_PlayerData->m_Quest[19] = 0;
			Obj.m_PlayerData->m_Quest[20] = 0;
			Obj.m_PlayerData->m_Quest[21] = 0;
			Obj.m_PlayerData->m_Quest[22] = 0;
			Obj.m_PlayerData->m_Quest[23] = 0;
			Obj.m_PlayerData->m_Quest[24] = 0;
			
			//sLog->outBasic("[Quest] Set MonsterKillInfo [%s][%s]", Obj.AccountID, Obj.Name);

		}
		//End
	}
	else
	{
		//Begin
		for ( int subquest=0;subquest<subquestcount;subquest++)
		{
			lpSubInfo = this->GetSubquestInfo(lpObj, lpQuestInfo, subquest);

			if ( lpSubInfo != NULL )
			{
				for ( int n = 0;n<concount;n++)
				{
					lpCondition = this->GetQuestCondition(lpQuestInfo, n);
					NeedZen += lpCondition->NeedZen;
				}
			}
		}
		//End
	}
	
	Obj.m_PlayerData->Money -= NeedZen;
	GSProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);
	return true;
}

BOOL CQuestInfo::QuestClear(CGameObject &Obj, int QuestIndex)
{
	LPQUEST_INFO lpQuestInfo = this->GetQuestInfo(QuestIndex);

	if ( lpQuestInfo == NULL )
	{
		return 0xFF;
	}

	if ( Obj.Type != OBJ_USER )
	{
		return 0xFF;
	}

	int subquestcount = lpQuestInfo->QuestSubInfoCount;
	int concount = lpQuestInfo->QuestConditionCount;
	LPQUEST_SUB_INFO lpSubInfo;

	for ( int subquest=0;subquest<subquestcount;subquest++)
	{
		lpSubInfo = this->GetSubquestInfo(lpObj, lpQuestInfo, subquest);

		if ( lpSubInfo != NULL )
		{
			if ( lpSubInfo->RewardType == QUEST_COMPENSATION_CHANGEUP )
			{
				Obj.m_PlayerData->LevelUpPoint += lpSubInfo->RewardCount;
				GSProtocol.GCSendQuestPrize(Obj.m_Index, QUEST_COMPENSATION_STATUP, lpSubInfo->RewardCount );
				Obj.m_PlayerData->ChangeUP = true;
				Obj.m_PlayerData->DbClass |= 1;
				gObjMakePreviewCharSet(Obj.m_Index);
			//	BYTE btClass = (Obj.Class * 32) & 224;
				BYTE btClass = (Obj.Class << 5) & 0xE0;
		
			//	sLog->outBasic("[K2] btClass %d Class %d", btClass, Obj.Class);
				btClass |= (Obj.m_PlayerData->ChangeUP << 4) & 0x10;
				btClass = btClass / 2;
			//	btClass |= (Obj.m_PlayerData->ChangeUP * 16) & 16;
			//	sLog->outBasic("[K2] btClass %d ChangeUP %d", btClass, Obj.m_PlayerData->ChangeUP);
				GSProtocol.GCSendQuestPrize(Obj.m_Index, QUEST_COMPENSATION_CHANGEUP, btClass);
			//	sLog->outBasic("[Quest] Quest Clear (%s) : [%s][%s] Stat(%d,%d), ChangeUp",
			//		lpQuestInfo->Name, Obj.AccountID, Obj.Name, Obj.m_PlayerData->LevelUpPoint,lpSubInfo->RewardCount );
			}
			else if ( lpSubInfo->RewardType == QUEST_COMPENSATION_STATUP )
			{
				Obj.m_PlayerData->LevelUpPoint += lpSubInfo->RewardCount;
				GSProtocol.GCSendQuestPrize(Obj.m_Index, QUEST_COMPENSATION_STATUP, lpSubInfo->RewardCount );
				//sLog->outBasic("[Quest] Quest Clear (%s) : [%s][%s] Stat(%d,%d)",lpQuestInfo->Name, Obj.AccountID, Obj.Name, Obj.m_PlayerData->LevelUpPoint,
				//	lpSubInfo->RewardCount );
			}
			else if ( lpSubInfo->RewardType == QUEST_COMPENSATION_PLUSSSTAT )
			{
				int level = Obj.Level - QUEST_MINLEVEL_PLUSSTAT;

				if ( level <= 0 )
				{
					level = 0;
				}

				Obj.m_PlayerData->LevelUpPoint += level;
				Obj.m_PlayerData->PlusStatQuestClear = true;
				GSProtocol.GCSendQuestPrize(Obj.m_Index, QUEST_COMPENSATION_PLUSSSTAT, level );
				//sLog->outBasic("[Quest] Quest Clear (%s) : [%s][%s] Stat(%d,%d), PlusStat",
				//	lpQuestInfo->Name, Obj.AccountID, Obj.Name, Obj.m_PlayerData->LevelUpPoint,level );
			}
			else if ( lpSubInfo->RewardType == QUEST_COMPENSATION_COMBOSKILL )
			{
				GSProtocol.GCSendQuestPrize(Obj.m_Index, QUEST_COMPENSATION_COMBOSKILL, 0 );
				Obj.m_PlayerData->ComboSkillquestClear = true;
				//sLog->outBasic("[Quest] Quest Clear (%s) : [%s][%s] ComboSkill",	lpQuestInfo->Name, Obj.AccountID, Obj.Name);
			}
			else if ( lpSubInfo->RewardType == QUEST_COMPENSATION_CHANGEUP2 )
			{
				Obj.m_PlayerData->LevelUpPoint += lpSubInfo->RewardCount;
				GSProtocol.GCSendQuestPrize(Obj.m_Index, QUEST_COMPENSATION_STATUP, lpSubInfo->RewardCount);
				Obj.m_PlayerData->ChangeUP = 2;
				g_MasterLevelSkillTreeSystem.InitData(lpObj);

//				BYTE btClass = (Obj.Class * 32) + 24;
//				btClass = btClass / 2;
				BYTE btClass = CS_SET_CLASS_SX(Obj.Class);
				btClass |= CS_SET_CHANGEUP_SX;
				btClass |= CS_SET_CHANGEUP2_SX;
				//BYTE btClass = (Obj.Class * 32) + 24;

				if (Obj.m_PlayerData->DbClass != 2 && Obj.m_PlayerData->DbClass != 18 && Obj.m_PlayerData->DbClass != 34 && Obj.m_PlayerData->DbClass != 50 && Obj.m_PlayerData->DbClass != 66 && Obj.m_PlayerData->DbClass != 82 && Obj.m_PlayerData->DbClass != 98 && Obj.m_PlayerData->DbClass != 114)
				{
					if (Obj.m_PlayerData->DbClass == 48 || Obj.m_PlayerData->DbClass == 64 || Obj.m_PlayerData->DbClass == 96 || Obj.m_PlayerData->DbClass == 112)
					{
						Obj.m_PlayerData->DbClass += 2;
					}
					else
					{
						Obj.m_PlayerData->DbClass += 1;
					}
				}

				gObjMakePreviewCharSet(Obj.m_Index);
				GSProtocol.GCSendQuestPrize(Obj.m_Index, QUEST_COMPENSATION_CHANGEUP2, btClass);
				//sLog->outBasic("[Quest] Quest Clear (%s) : [%s][%s] Class:%d(%d), 3rd ChangeUp",	lpQuestInfo->Name, Obj.AccountID, Obj.Name, Obj.m_PlayerData->DbClass, Obj.m_PlayerData->DbClass-1 );

				Obj.m_PlayerData->m_Quest[20] = 0;
				Obj.m_PlayerData->m_Quest[21] = 0;
				Obj.m_PlayerData->m_Quest[22] = 0;
				Obj.m_PlayerData->m_Quest[24] = 0;

				//sLog->outBasic("[Quest] Quest Clear - MonsterKillCount Reset (%s) : [%s][%s]",lpQuestInfo->Name, Obj.AccountID, Obj.Name );

				//sLog->outBasic("[Quest] Quest Clear (%s) : [%s][%s] Stat(%d,%d), Class:%d PlusStat",lpQuestInfo->Name, Obj.AccountID, Obj.Name, Obj.m_PlayerData->LevelUpPoint, lpSubInfo->RewardCount, Obj.m_PlayerData->DbClass );
			}

			if ( lpSubInfo->QuestType == 1 )
			{
				gObjDelteItemCountInInventory(Obj.m_Index, lpSubInfo->NeedType, lpSubInfo->NeedSubType,
					lpSubInfo->NeedNumber);
			}
		}
	}
	return true;
}

void CQuestInfo::QuestInfoSave(CGameObject &Obj)
{
	int questcount = this->GetQeustCount();
	int foundquest = 0;
	LPQUEST_INFO lpQuestInfo;

	for ( int i = 0 ; i<MAX_QUEST_INFO;i++)
	{
		lpQuestInfo = this->GetQuestInfo(i);

		if ( lpQuestInfo == NULL )
		{
			continue;
		}

		//sLog->outBasic("[Quest] QuestSave [%s][%s] index(%d) name(%s) state(%s)",
		//	Obj.AccountID, Obj.Name, i, lpQuestInfo->Name,sQuestString[this->GetQuestState(lpObj, i)]);
		foundquest++;

		if ( foundquest == questcount )
		{
			break;
		}
	}

	int iQuestState_1 = this->GetQuestState(lpObj, 5);
	int iQuestState_2 = this->GetQuestState(lpObj, 6);

	if (iQuestState_1 == 1 || iQuestState_2 == 1)
	{
		//sLog->outBasic("[Quest] Save MonsterKillInfo [%s][%s] (%d, %d, %d, %d)", Obj.AccountID, Obj.Name, Obj.m_PlayerData->m_Quest[20], Obj.m_PlayerData->m_Quest[21], Obj.m_PlayerData->m_Quest[22], Obj.m_PlayerData->m_Quest[24]);
	}
}

BOOL CQuestInfo::GetQuestItem(CGameObject &Obj, short nType, short nLevel)
{
	CGameObject* lpObj = Obj;

	LPQUEST_INFO lpQuestInfo;
	LPQUEST_SUB_INFO lpSubInfo;

	int iSubquestcount;
	int iQuestState = QS_NOTHING;

	for (int iQuestIndex = 0; iQuestIndex < m_QuestCount; iQuestIndex++)
	{
		iQuestState = GetQuestState(lpObj, iQuestIndex);
		if (iQuestState == QS_ACCEPT)
		{
			lpQuestInfo = GetQuestInfo(iQuestIndex);
			if (!lpQuestInfo)	return FALSE;

			iSubquestcount = lpQuestInfo->QuestSubInfoCount;
			for (int subquest = 0; subquest < iSubquestcount; subquest++)
			{
				lpSubInfo = GetSubquestInfo(lpObj, lpQuestInfo, subquest);
				if (!lpSubInfo)	continue;
				if (lpSubInfo->QuestType != 1)	return FALSE;

				if (nType == (lpSubInfo->NeedType*512) + lpSubInfo->NeedSubType)
				{
					if (nLevel == lpSubInfo->ItemLevel)
					{
						int iItemCount = gObjGetItemCountInIventory(iIndex, nType);
						if (iItemCount >= lpSubInfo->NeedNumber)
						{
							//sLog->outBasic("[Quest] Too many has quest items [%s][%s] (%d/%d)", Obj.AccountID, Obj.Name, nType, iItemCount);
							return FALSE;
						}
						return TRUE;
					}
				}
			}
			return FALSE;
		}
	}
	return FALSE;
}

struct PMSG_ANS_QUEST_MONKILL_INFO
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btQuestIndex;
	int iKillCountInfo[10];
};

void CQuestInfo::SendQuestMonsterKill(CGameObject &Obj, int iQuestIndex)
{
	PMSG_ANS_QUEST_MONKILL_INFO pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xA4, 0x00, sizeof(pMsg));

	pMsg.btResult = 0;
	pMsg.btQuestIndex = iQuestIndex;

	if (this->GetQuestState(lpObj, iQuestIndex) == FALSE)
	{
		return;
	}

	LPQUEST_INFO lpQuestInfo = this->GetQuestInfo(iQuestIndex);

	if (!lpQuestInfo)
	{
		return;
	}

	if( Obj.m_PlayerData->m_Quest[20] == 255)
	{
		Obj.m_PlayerData->m_Quest[20] = 0;
	}

	if( Obj.m_PlayerData->m_Quest[21] == 255 )
	{
		Obj.m_PlayerData->m_Quest[21] = 0;
	}

	if( Obj.m_PlayerData->m_Quest[22] == 255 )
	{
		Obj.m_PlayerData->m_Quest[22] = 0;
	}

	if( Obj.m_PlayerData->m_Quest[24] == 255 )
	{
		Obj.m_PlayerData->m_Quest[24] = 0;
	}

	int subquestcount = lpQuestInfo->QuestSubInfoCount;
	int concount = lpQuestInfo->QuestConditionCount;
	int iSendCount = 0;

	for (int subquest = 0; subquest < subquestcount; subquest++)
	{
		LPQUEST_SUB_INFO lpSubInfo = this->GetSubquestInfo(lpObj, lpQuestInfo, subquest);

		if (lpSubInfo && lpSubInfo->QuestType == 2)
		{
			pMsg.btResult = TRUE;

			if ( lpSubInfo->NeedType == 409 )
			{
				if ( Obj.m_PlayerData->m_Quest[20] > lpSubInfo->NeedNumber )
				{
					Obj.m_PlayerData->m_Quest[20] = lpSubInfo->NeedNumber;
				}

				pMsg.iKillCountInfo[iSendCount] = 409;
				iSendCount++;
				pMsg.iKillCountInfo[iSendCount] = Obj.m_PlayerData->m_Quest[20];
				iSendCount++;
			}

			else if ( lpSubInfo->NeedType == 410 )
			{
				if ( Obj.m_PlayerData->m_Quest[21] > lpSubInfo->NeedNumber )
				{
					Obj.m_PlayerData->m_Quest[21] = lpSubInfo->NeedNumber;
				}

				pMsg.iKillCountInfo[iSendCount] = 410;
				iSendCount++;
				pMsg.iKillCountInfo[iSendCount] = Obj.m_PlayerData->m_Quest[21];
				iSendCount++;
			}

			else if ( lpSubInfo->NeedType == 411 )
			{
				if ( Obj.m_PlayerData->m_Quest[22] > lpSubInfo->NeedNumber )
				{
					Obj.m_PlayerData->m_Quest[22] = lpSubInfo->NeedNumber;
				}

				pMsg.iKillCountInfo[iSendCount] = 411;
				iSendCount++;
				pMsg.iKillCountInfo[iSendCount] = Obj.m_PlayerData->m_Quest[22];
				iSendCount++;
			}

			else if ( lpSubInfo->NeedType == 412 )
			{
				if ( Obj.m_PlayerData->m_Quest[24] > lpSubInfo->NeedNumber )
				{
					Obj.m_PlayerData->m_Quest[24] = lpSubInfo->NeedNumber;
				}

				pMsg.iKillCountInfo[iSendCount] = 412;
				iSendCount++;
				pMsg.iKillCountInfo[iSendCount] = Obj.m_PlayerData->m_Quest[24];
				iSendCount++;
			}
		}
	}

	if (pMsg.btResult == TRUE)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
	}
}

void CQuestInfo::CheckQuestMapEnterOnWerwolf(CGameObject &Obj)
{
	PMSG_ANS_ENTER_ON_QUESTNPC pMsg;
	CGameObject* lpObj = Obj;

	if (!lpObj)
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	PHeadSubSetB((BYTE*)&pMsg, 0xD0, 0x07, sizeof(pMsg));
	pMsg.btResult = 0;

	if (Obj.m_IfState->use && Obj.m_IfState->type != 12)
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (Obj.TargetNpcNumber == -1)
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	CGameObject lpNpc = getGameObject(Obj.TargetNpcNumber);

	if (lpNpc->Class != 407)
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (gObjCalDistance(lpObj, lpNpc) > 5)
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (Obj.Level < 350)
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (Obj.m_PlayerData->Money < 3000000)
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	int iQuestState = this->GetQuestState(lpObj, 5);

	if (iQuestState != 1 && iQuestState != 2)
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	int iPartyNum = Obj.PartyNumber;

	if (iPartyNum >= 0)
	{
		for (int i = 0; i < MAX_USER_IN_PARTY; i++)
		{
			int iPartyUserIndex = gParty.m_PartyS[iPartyNum].Number[i];
			
			if (iPartyUserIndex >= 0 && iPartyUserIndex != iIndex)
			{
				CGameObject lpPartyObj = getGameObject(iPartyUserIndex);

				if ( lpPartyObj.Level >= 350 && 
					Obj.MapNumber == lpPartyObj.MapNumber && 
					gObjCalDistance(lpObj, lpPartyObj) < 10 && 
					gObjMoveGate(iPartyUserIndex, 256) == FALSE )
				{
					IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
				}
			}
		}
	}

	if (gObjMoveGate(iIndex, 256) == TRUE)
	{
		Obj.m_PlayerData->Money -= 3000000;
		GSProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);
	}

	else
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
	}
}

void CQuestInfo::CheckQuestMapEnterOnGateKeeper(CGameObject &Obj)
{
	PMSG_ANS_ENTER_ON_QUESTNPC pMsg;
	CGameObject* lpObj = Obj;

	if (!lpObj)
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	PHeadSubSetB((BYTE*)&pMsg, 0xD0, 0x08, sizeof(pMsg));
	pMsg.btResult = 0;

	if (Obj.m_IfState->use && Obj.m_IfState->type != 12)
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (Obj.TargetNpcNumber == -1)
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	CGameObject lpNpc = getGameObject(Obj.TargetNpcNumber);

	if (lpNpc->Class != 408)
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (gObjCalDistance(lpObj, lpNpc) > 5)
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (Obj.Level < 350)
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	int iQuestState = this->GetQuestState(lpObj, 6);

	if (iQuestState != 1)
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	int iPartyNum = Obj.PartyNumber;

	if (iPartyNum >= 0)
	{
		for (int i = 0; i < MAX_USER_IN_PARTY; i++)
		{
			int iPartyUserIndex = gParty.m_PartyS[iPartyNum].Number[i];
			
			if (iPartyUserIndex >= 0 && iPartyUserIndex != iIndex)
			{
				CGameObject lpPartyObj = getGameObject(iPartyUserIndex);

				if ( lpPartyObj.Level >= 350 && 
					Obj.MapNumber == lpPartyObj.MapNumber && 
					gObjCalDistance(lpObj, lpPartyObj) < 10 && 
					gObjMoveGate(iPartyUserIndex, 257) == FALSE )
				{
					IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
				}
			}
		}
	}

	if (gObjMoveGate(iIndex, 257) == FALSE)
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
	}
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

