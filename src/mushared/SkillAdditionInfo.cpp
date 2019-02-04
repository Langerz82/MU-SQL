// SkillAdditionInfo.cpp: implementation of the CSkillAdditionInfo class.
//	GS-N	1.00.18	JPN	0x00485060	-	Completed
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SkillAdditionInfo.h"
#include "TLog.h"
#include "configread.h"
#include "IniReader.h"
CSkillAdditionInfo g_SkillAdditionInfo;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSkillAdditionInfo::CSkillAdditionInfo()
{
	this->Init();
}

CSkillAdditionInfo::~CSkillAdditionInfo()
{
	return;
}


void CSkillAdditionInfo::Init()
{
	this->m_iInfinityArrowSkillTime = 0;
	this->m_iInfinityArrowUseLevel = 0;
	this->m_iInfinityArrowMPConsumptionPlus0 = 0;
	this->m_iInfinityArrowMPConsumptionPlus1 = 0;
	this->m_iInfinityArrowMPConsumptionPlus2 = 0;
	memset(this->m_nSoulBarrierManaRate_Level, 0x00, sizeof(this->m_nSoulBarrierManaRate_Level));
}


BOOL CSkillAdditionInfo::Load()
{
	CIniReader ReadSkill(g_ConfigRead.GetPath("\\Skills\\IGC_SkillSettings.ini"));

	this->m_iInfinityArrowSkillTime				= ReadSkill.ReadInt("SkillInfo", "InfinityArrowSkillTime", 600); 
	this->m_iInfinityArrowUseLevel				= ReadSkill.ReadInt("SkillInfo", "InfinityArrowUseLevel", 220); 
	this->m_iInfinityArrowMPConsumptionPlus0	= ReadSkill.ReadInt("SkillInfo", "InfinityArraowMPConsumptionPlus0", 0);
	this->m_iInfinityArrowMPConsumptionPlus1	= ReadSkill.ReadInt("SkillInfo", "InfinityArraowMPConsumptionPlus1", 0);
	this->m_iInfinityArrowMPConsumptionPlus2	= ReadSkill.ReadInt("SkillInfo", "InfinityArraowMPConsumptionPlus2", 0);
	this->m_iInfinityArrowMPConsumptionPlus3	= ReadSkill.ReadInt("SkillInfo", "InfinityArraowMPConsumptionPlus3", 0);

	this->m_bFireScreamSkill						= ReadSkill.ReadInt("SkillInfo", "FireScreamSkill", 1);
	this->m_iFireScreamExplosionAttackDistance		= ReadSkill.ReadInt("SkillInfo", "FireScreamExplosionAttackDistance", 1);
	this->m_iFireScreamExplosionRate				= ReadSkill.ReadInt("SkillInfo", "FireScreamExplosionRate", 3000);
	this->m_iFireScreamMaxAttackCountSameSerial		= ReadSkill.ReadInt("SkillInfo", "FireScreamMaxAttackCountSameSerial", 3);
	this->m_iFireScreamSerialInitCount				= ReadSkill.ReadInt("SkillInfo", "FireScreamSerialInitCount", 50);
	this->m_bCheckFireScreamSerial					= ReadSkill.ReadInt("SkillInfo", "CheckFireScreamSerial", 1);

	char szTemp[255];
	memset(szTemp, 0x00, sizeof(szTemp));

	for (int n = 1; n < 21; n++)
	{
		wsprintf(szTemp, "SoulBarrierManaRate_Level%d", n);
		this->m_nSoulBarrierManaRate_Level[n] = ReadSkill.ReadInt("SkillInfo", szTemp, n+20);
	}

	return TRUE;
}

short CSkillAdditionInfo::GetSoulBarrierManaRate_Level(int iLevel)
{
	if (iLevel < 0 || iLevel > MAX_MAGIC_LEVEL)
	{
		return MAX_MAGIC_LEVEL;
	}

	return this->m_nSoulBarrierManaRate_Level[iLevel];
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

