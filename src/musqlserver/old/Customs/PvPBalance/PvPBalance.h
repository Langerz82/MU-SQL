#ifndef _MU_PVPBALANCE_H_
#define _MU_PVPBALANCE_H_

#pragma once

#include "StdAfx.h"

#define ENABLE_CUSTOM_PVPBALANCE 1
#define PVPBALANCE_DEBUG 0
#define FILE_CUSTOM_PVPBALANCE "Custom\\PvPBalance.xml"

struct PvPBalanceInfo
{
	BYTE SourceClassType;
	BYTE TargetClassType;
	short IncreaseDamagePercent;
	short DecreaseDamagePercent;
	short IncreaseAttackRatePercent;
	short DecreaseAttackRatePercent;
	short IncreaseDefenseRatePercent;
	short DecreaseDefenseRatePercent;
};

class PvPBalance
{
public:
	PvPBalance();
	~PvPBalance();

	void Load();
	void Init();
	void Read(const char* File);

	PvPBalanceInfo* getInfo(BYTE SourceClassType, BYTE TargetClassType);

	void modifyDamage(CGameObject &objSource, CGameObject &objTarget, int& Value);
	void modifyAttackRate(CGameObject &objSource, CGameObject &objTarget, float& Value);
	void modifyDefenseRate(CGameObject &objSource, CGameObject &objTarget, float& Value);

private:
	bool m_isLoaded;
	std::vector<PvPBalanceInfo> m_ClassInfo;
};

extern PvPBalance g_PvPBalance;

#endif
