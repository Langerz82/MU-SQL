// SkillDelay.cpp: implementation of the CSkillDelay class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SkillDelay.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSkillDelay::CSkillDelay()
{
	Init();
}

CSkillDelay::~CSkillDelay()
{

}

void CSkillDelay::Init()
{
	memset(LastSkillUseTime, 0, sizeof(LastSkillUseTime));
}

BOOL CSkillDelay::Check(WORD skill)
{
	int skilldelaytime = MagicDamageC.GetDelayTime(skill);

	if (skilldelaytime == 0)
	{
		return TRUE;
	}

	ULONGLONG dwtime = GetTickCount64();

	if (skilldelaytime + this->LastSkillUseTime[skill] >= dwtime)
	{
		return FALSE;
	}

	else
	{
		this->LastSkillUseTime[skill] = dwtime;
		return TRUE;
	}
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

