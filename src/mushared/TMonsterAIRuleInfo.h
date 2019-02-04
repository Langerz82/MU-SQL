// TMonsterAIRuleInfo.h: interface for the TMonsterAIRuleInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TMONSTERAIRULEINFO_H__0A96A7D0_3DC5_439F_BEB1_940C22757704__INCLUDED_)
#define AFX_TMONSTERAIRULEINFO_H__0A96A7D0_3DC5_439F_BEB1_940C22757704__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class TMonsterAIRuleInfo  
{

public:

	TMonsterAIRuleInfo();
	void Reset();
	int IsValid();
	int CheckConditionSpecificDate();
	int CheckConditionCrywolfStart();
	int CheckConditionCrywolfEnd();

public:

	int m_iRuleNumber;	// 0
	int m_iMonsterClass;	// 4
	int m_iMonsterAIUnit;	// 8
	int m_iRuleCondition;	// C
	int m_iWaitTime;	// 10
	int m_iContinuanceTime;	// 14
	int m_iMonth;	// 18
	int m_iDay;	// 1C
	int m_iWeekDay;	// 20
	int m_iHour;	// 24
	int m_iMinute;	// 28
	int m_iAppliedTime;	// 2C
	int m_iApplyingTime;	// 30
	char m_szRuleDesc[100];	// 34

};

#endif // !defined(AFX_TMONSTERAIRULEINFO_H__0A96A7D0_3DC5_439F_BEB1_940C22757704__INCLUDED_)


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

