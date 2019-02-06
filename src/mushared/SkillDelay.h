// SkillDelay.h: interface for the CSkillDelay class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKILLDELAY_H__B49397E0_3335_4B10_A115_29A2ACE442D5__INCLUDED_)
#define AFX_SKILLDELAY_H__B49397E0_3335_4B10_A115_29A2ACE442D5__INCLUDED_

#include "MagicDamage.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSkillDelay  
{
	ULONGLONG LastSkillUseTime[MAX_SKILL];
public:
	CSkillDelay();
	virtual ~CSkillDelay();

	void Init();
	int Check(WORD skill);
};

#endif // !defined(AFX_SKILLDELAY_H__B49397E0_3335_4B10_A115_29A2ACE442D5__INCLUDED_)


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

