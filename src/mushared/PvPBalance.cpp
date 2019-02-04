////////////////////////////////////////////////////////////////////////////////
// PvPBalance.cpp
#include "stdafx.h"
#include "PvPBalance.h"
#include "GameMain.h"
#include "TLog.h"
#include "configread.h"

PvPBalance g_PvPBalance;

PvPBalance::PvPBalance() {}
PvPBalance::~PvPBalance() {}

void PvPBalance::Load()
{
	Init();
	Read(g_ConfigRead.GetPath(FILE_CUSTOM_PVPBALANCE));
}

void PvPBalance::Init()
{
	m_isLoaded = false;
	m_ClassInfo.clear();
}

void PvPBalance::Read(const char* File)
{
	using namespace pugi;
	xml_document Document;
	xml_parse_result Result = Document.load_file(File);

	if (Result.status != status_ok)
	{
		g_Log.MsgBox("[PvPBalance] File %s not found!", File);
		return;
	}

	xml_node nodeStart = Document.child("pvpbalance");

	for (xml_node nodeIt = nodeStart.child("rule"); nodeIt; nodeIt = nodeIt.next_sibling()) {
		PvPBalanceInfo newInfo = { 0 };
		newInfo.SourceClassType = (BYTE)nodeIt.attribute("source").as_int(-1);
		newInfo.TargetClassType = (BYTE)nodeIt.attribute("target").as_int(-1);
		newInfo.IncreaseDamagePercent = nodeIt.child("damage").attribute("increase").as_int(0);
		newInfo.DecreaseDamagePercent = nodeIt.child("damage").attribute("decrease").as_int(0);
		newInfo.IncreaseAttackRatePercent = nodeIt.child("attackrate").attribute("increase").as_int(0);
		newInfo.DecreaseAttackRatePercent = nodeIt.child("attackrate").attribute("decrease").as_int(0);
		newInfo.IncreaseDefenseRatePercent = nodeIt.child("defenserate").attribute("increase").as_int(0);
		newInfo.DecreaseDefenseRatePercent = nodeIt.child("defenserate").attribute("decrease").as_int(0);

		m_ClassInfo.push_back(newInfo);
	}
	m_isLoaded = true;
}

PvPBalanceInfo* PvPBalance::getInfo(BYTE SourceClassType, BYTE TargetClassType)
{
	if (!m_isLoaded)
	{
		return NULL;
	}

	for (size_t i = 0; i < m_ClassInfo.size(); i++)
	{

		if (m_ClassInfo[i].SourceClassType == SourceClassType
			&& m_ClassInfo[i].TargetClassType == TargetClassType)
		{
			return &m_ClassInfo[i];
		}
	}
	return NULL;
}

void PvPBalance::modifyDamage(short UserIndex, short TargetIndex, int& Value)
{
	LPOBJ objSource = &gObj[UserIndex];
	LPOBJ objTarget = &gObj[TargetIndex];
	PvPBalanceInfo* modifyInfo = getInfo(objSource->Class, objTarget->Class);

	if (modifyInfo == NULL)
	{
		return;
	}

	int tmpValue = 0;

	if (modifyInfo->IncreaseDamagePercent > 0)
	{
		tmpValue = (Value * modifyInfo->IncreaseDamagePercent) / 100;

		if (tmpValue > 0)
		{
			Value += tmpValue;
		}
	}

	if (modifyInfo->DecreaseDamagePercent > 0)
	{
		tmpValue = (Value * modifyInfo->DecreaseDamagePercent) / 100;

		if (tmpValue > 0)
		{
			Value -= tmpValue;
		}
	}
}

void PvPBalance::modifyAttackRate(short UserIndex, short TargetIndex, float& Value)
{
	LPOBJ objSource = &gObj[UserIndex];
	LPOBJ objTarget = &gObj[TargetIndex];
	PvPBalanceInfo* modifyInfo = getInfo(objSource->Class, objTarget->Class);

	if (modifyInfo == NULL) {
		return;
	}

	float tmpValue = 0;

	if (modifyInfo->IncreaseAttackRatePercent > 0)
	{
		tmpValue = (Value * modifyInfo->IncreaseAttackRatePercent) / 100;

		if (tmpValue > 0)
		{

			Value += tmpValue;
		}
	}

	if (modifyInfo->DecreaseAttackRatePercent > 0)
	{
		tmpValue = (Value * modifyInfo->DecreaseAttackRatePercent) / 100;

		if (tmpValue > 0)
		{
			Value -= tmpValue;
		}
	}
}

void PvPBalance::modifyDefenseRate(short UserIndex, short TargetIndex, float& Value)
{
	LPOBJ objSource = &gObj[UserIndex];
	LPOBJ objTarget = &gObj[TargetIndex];
	PvPBalanceInfo* modifyInfo = getInfo(objSource->Class, objTarget->Class);

	if (modifyInfo == NULL) {
		return;
	}

	float tmpValue = 0;

	if (modifyInfo->IncreaseDefenseRatePercent > 0)
	{
		tmpValue = (Value * modifyInfo->IncreaseDefenseRatePercent) / 100;

		if (tmpValue > 0)
		{
			Value += tmpValue;
		}
	}

	if (modifyInfo->DecreaseDefenseRatePercent > 0)
	{
		tmpValue = (Value * modifyInfo->DecreaseDefenseRatePercent) / 100;

		if (tmpValue > 0)
		{
			Value -= tmpValue;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

