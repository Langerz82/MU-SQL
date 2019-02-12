// TMonsterSkillUnit.cpp: implementation of the TMonsterSkillUnit class.
//	GS-N	1.00.18	JPN	0x00559910	-	Completed
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "TMonsterSkillUnit.h"
#include "Logging/Log.h"

TMonsterSkillUnit TMonsterSkillUnit::s_MonsterSkillUnitArray[MAX_MONSTER_SKILL_UNIT_ARRAY];
BOOL TMonsterSkillUnit::s_bDataLoad = FALSE;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TMonsterSkillUnit::TMonsterSkillUnit()
{
	this->Reset();
}

TMonsterSkillUnit::~TMonsterSkillUnit()
{
	return;
}

void TMonsterSkillUnit::Reset()
{
	this->m_iUnitTargetType = -1;
	this->m_iUnitScopeType = -1;
	this->m_iUnitScopeValue = -1;
	this->m_iDelay = -1;
	this->m_iUnitNumber = -1;
	memset(this->m_szUnitName, 0, sizeof(this->m_szUnitName));
	this->m_iElementsCount = -1;

	for (int i = 0; i < MAX_MONSTER_SKILL_ELEMENT_INFO; i++)
	{
		this->m_lpElementsSlot[i] = NULL;
	}
}



BOOL TMonsterSkillUnit::LoadData(LPSTR lpszFileName)
{
	TMonsterSkillUnit::s_bDataLoad = FALSE;

	if ( lpszFileName == NULL || strcmp(lpszFileName, "") == 0 )
	{
		sLog->outError("[Monster Skill Unit] - File load error : File Name Error");
		return FALSE;
	}

	try
	{
		pugi::xml_document file;
		pugi::xml_parse_result res = file.load_file(lpszFileName);

		if ( res.status != pugi::status_ok )
		{
			sLog->outError("[Monster Skill Unit] - Can't Load %s (%s)", lpszFileName, res.description());
			return FALSE;
		}

		TMonsterSkillUnit::DelAllSkillUnit();

		pugi::xml_node mainXML = file.child("MonsterSkill");

		for (pugi::xml_node unit = mainXML.child("Unit"); unit; unit = unit.next_sibling())
		{
			char szUnitName[20] = {0};
			memcpy(szUnitName, unit.attribute("Name").as_string(), sizeof(szUnitName));

			int iUnitNumber = unit.attribute("Number").as_int();
			int iUnitTargetType = unit.attribute("TargetType").as_int();
			int iUnitScopeType = unit.attribute("ScopeType").as_int();
			int iUnitScopeValue = unit.attribute("ScopeValue").as_int();
			int iDelay = unit.attribute("Delay").as_int();
			int iElementCount = 0;
			int iElementsSlot[MAX_MONSTER_SKILL_ELEMENT_INFO] = { -1};

			char szTemp[128];
			memset(szTemp, 0x00, sizeof(szTemp));

			for ( int i = 0; i < MAX_MONSTER_SKILL_ELEMENT_INFO; i++)
			{
				sprintf(szTemp, "ElementSlot%d", i);
				iElementsSlot[i] = unit.attribute(szTemp).as_int();
			}

			if ( iUnitNumber < 0 || iUnitNumber >= MAX_MONSTER_SKILL_UNIT_ARRAY )
			{
				sLog->outError("[Monster SkillUnit] - Unit(%d) Error (%s) File. ", iUnitNumber , lpszFileName);
				continue;
			}

			TMonsterSkillUnit::s_MonsterSkillUnitArray[iUnitNumber].m_iUnitNumber = iUnitNumber;
			TMonsterSkillUnit::s_MonsterSkillUnitArray[iUnitNumber].m_iUnitTargetType = iUnitTargetType;
			TMonsterSkillUnit::s_MonsterSkillUnitArray[iUnitNumber].m_iUnitScopeType = iUnitScopeType;
			TMonsterSkillUnit::s_MonsterSkillUnitArray[iUnitNumber].m_iUnitScopeValue = iUnitScopeValue;
			TMonsterSkillUnit::s_MonsterSkillUnitArray[iUnitNumber].m_iDelay = iDelay;
			memcpy(TMonsterSkillUnit::s_MonsterSkillUnitArray[iUnitNumber].m_szUnitName, szUnitName, sizeof(TMonsterSkillUnit::s_MonsterSkillUnitArray[iUnitNumber].m_szUnitName));

			for(int i = 0; i < MAX_MONSTER_SKILL_ELEMENT_INFO; i++)
			{
				int iElement = iElementsSlot[i];

				if ( iElement != -1 )
				{
					TMonsterSkillElement * pMonsterSkillElement = TMonsterSkillElement::FindSkillElement(iElement);

					if ( pMonsterSkillElement )
					{
						iElementCount++;
						TMonsterSkillUnit::s_MonsterSkillUnitArray[iUnitNumber].m_lpElementsSlot[i] = pMonsterSkillElement;
					}

					else
					{
					
					}
				}
			}

			TMonsterSkillUnit::s_MonsterSkillUnitArray[iUnitNumber].m_iElementsCount = iElementCount;
		}

		TMonsterSkillUnit::s_bDataLoad = TRUE;
	}

	catch(DWORD)
	{
		sLog->outError("[Monster Skill Unit] - Loading Exception Error (%s) File. ", lpszFileName);
	}

	return FALSE;
}


BOOL TMonsterSkillUnit::DelAllSkillUnit()
{
	for(int i=0;i<MAX_MONSTER_SKILL_UNIT_ARRAY;i++)
	{
		TMonsterSkillUnit::s_MonsterSkillUnitArray[i].Reset();
	}

	return FALSE;
}


TMonsterSkillUnit * TMonsterSkillUnit::FindSkillUnit(int iUnitNumber)
{
	if ( iUnitNumber < 0 || iUnitNumber >= MAX_MONSTER_SKILL_UNIT_ARRAY )
	{
		sLog->outBasic("[Monster SkillUnit] FindSkillUnit() Error - (UnitNumber=%d) ",
			iUnitNumber);

		return NULL;
	}

	if ( TMonsterSkillUnit::s_MonsterSkillUnitArray[iUnitNumber].m_iUnitNumber == iUnitNumber )
	{
		return &TMonsterSkillUnit::s_MonsterSkillUnitArray[iUnitNumber];
	}

	sLog->outBasic("[Monster SkillUnit] FindSkillUnit() Error - (UnitNumber=%d) ",
		iUnitNumber);

	return FALSE;
}

void TMonsterSkillUnit::RunSkill(CGameObject &Obj, int iTargetIndex)
{
	char szElementsName[5][20] = {0};

	for ( int i=0;i<this->m_iElementsCount;i++)
	{
		if ( this->m_lpElementsSlot[i] == NULL )
		{
			sLog->outError(  "[Monster SkillUnit] SkillElement is NULL");
			continue;
		}

		this->m_lpElementsSlot[i]->ForceSkillElement(iIndex, iTargetIndex);
	}
}


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

