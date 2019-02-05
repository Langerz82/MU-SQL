////////////////////////////////////////////////////////////////////////////////
// ExpManager.cpp
#include "stdafx.h"
#include "ExpManager.h"
#include "Log/Log.h"
#include "user.h"
#include "MasterLevelSkillTreeSystem.h"

CExpManager g_ExpManager;

CExpManager::CExpManager()
{
	this->m_bDebugMode = false;
	this->m_iExpCalcType = EXP_CALC_STATIC_ONLY;
	this->m_fStaticExp = 1.0;
	this->m_fStaticMLExp = 1.0;
	this->m_fEventExp = 0.0; 
	this->m_vtExpRanges.clear();
}

CExpManager::~CExpManager()
{

}

void CExpManager::LoadScript(LPSTR szFileName)
{
	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(szFileName);

	if (res.status != pugi::status_ok)
	{
		sLog.outError("[Exp Manager] Cannot load %s file (%s)", szFileName, res.description());
		return;
	}

	pugi::xml_node mainXML = file.child("ExpSystem");

	this->m_iExpCalcType = main.attribute("CalcType").as_int();
	this->m_bDebugMode = main.attribute("DebugMode").as_bool();

	pugi::xml_node static_exp = mainXML.child("StaticExp");

	this->m_fStaticExp = static_exp.attribute("Normal").as_float();
	this->m_fStaticMLExp = static_exp.attribute("Master").as_float();
	this->m_fEventExp = static_exp.attribute("Event").as_float();
	this->m_fQuestExp = static_exp.attribute("Quest").as_float();

	pugi::xml_node dynamic_exp = mainXML.child("DynamicExpRangeList");
	this->m_vtExpRanges.clear();

	for (pugi::xml_node range = dynamic_exp.child("Range"); range; range = range.next_sibling())
	{
		EXP_RANGES m_Range;

		m_Range.iMinReset = range.attribute("MinReset").as_int();
		m_Range.iMaxReset = range.attribute("MaxReset").as_int();
		m_Range.iMinLevel = range.attribute("MinLevel").as_int();
		m_Range.iMaxLevel = range.attribute("MaxLevel").as_int();
		m_Range.fExp = range.attribute("NormalExp").as_float();
		m_Range.fMLExp = range.attribute("MasterExp").as_float();

		this->m_vtExpRanges.push_back(m_Range);
	}

}

float CExpManager::GetExpMultiplier(int iLevel, int iReset, bool bIsMasterLevel)
{
	float fExp = 0.0;

	if (this->m_iExpCalcType == EXP_CALC_STATIC_ONLY)
	{
		if (bIsMasterLevel)
		{
			fExp = this->m_fStaticMLExp;
		}

		else
		{
			fExp = this->m_fStaticExp;
		}

		fExp += this->m_fEventExp;
	}

	else
	{
		for (std::vector<EXP_RANGES>::iterator It = this->m_vtExpRanges.begin(); It != this->m_vtExpRanges.end(); It++)
		{
			if (iReset >= It->iMinReset && iReset <= It->iMaxReset && iLevel >= It->iMinLevel && iLevel <= It->iMaxLevel)
			{
				if (bIsMasterLevel)
				{
					fExp = It->fMLExp;
				}

				else
				{
					fExp = It->fExp;
				}

				break;
			}
		}

		if (this->m_iExpCalcType == EXP_CALC_STATIC_PLUS_DYNAMIC)
		{
			if (bIsMasterLevel)
			{
				fExp += this->m_fStaticMLExp;
			}

			else
			{
				fExp += this->m_fStaticExp;
			}

			fExp += this->m_fEventExp;
		}

		else if (this->m_iExpCalcType == EXP_CALC_STATIC_MULTIPLE_DYNAMIC)
		{
			if (bIsMasterLevel)
			{
				fExp *= (this->m_fStaticMLExp + this->m_fEventExp);
			}

			else
			{
				fExp *= (this->m_fStaticExp + this->m_fEventExp);
			}
		}
	}

	if (this->m_bDebugMode == true)
	{
		g_Log.AddC(TColor::Yellow, "[EXP DEBUG] Level:%d Reset:%d ML:%d ExpResult:%f (CalcType:%d)", iLevel, iReset, bIsMasterLevel, fExp, this->m_iExpCalcType);
	}

	return fExp;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

