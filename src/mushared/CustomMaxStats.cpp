////////////////////////////////////////////////////////////////////////////////
// CustomMaxStats.cpp
#include "stdafx.h"
#include "CustomMaxStats.h"
#include "GameMain.h"
#include "TLog.h"
#include "configread.h"

CMaxStatsInfo g_MaxStatsInfo;

CMaxStatsInfo::CMaxStatsInfo() {}
CMaxStatsInfo::~CMaxStatsInfo() {}

void CMaxStatsInfo::Read(const char* File)
{
	using namespace pugi;
	xml_document Document;
	xml_parse_result Result = Document.load_file(File);

	if (Result.status != status_ok)
	{
		g_Log.MsgBox("[IGC_PlayerInfo] File %s not found!", File);
		return;
	}

	xml_node nodeStart = Document.child("PlayerInfo");

	xml_node PartyLevel = nodeStart.child("PartyReqLevel");
	GetClass.Max_Party_Level_Dif = PartyLevel.child("MaxPartyLevelDif").text().as_int(230);

	xml_node Mastering = nodeStart.child("MasterLevel");
	GetClass.m_MinReset = Mastering.child("MinReset").text().as_int(0);
	GetClass.MLUserMaxLevel = Mastering.child("MaxLevel").text().as_int(420);
	GetClass.MLPointPerLevel = Mastering.child("PointPerLevel").text().as_int(1);
	GetClass.MLMonsterMinLevel = Mastering.child("MonsterMinLevel").text().as_int(95);

	xml_node Common = nodeStart.child("LevelUpPoint");
	GetClass.LevelUpPointNormal = Common.child("PointNormal").text().as_int(5);
	GetClass.LevelUpPointMGDL = Common.child("PointMGDLRFGL").text().as_int(7);

	xml_node StatsForClassDW = nodeStart.child("StatsDW");
	GetClass.MaxStats[0][0] = StatsForClassDW.child("MaxStrenghtDW").text().as_int(0);
	GetClass.MaxStats[0][1] = StatsForClassDW.child("MaxAgilityDW").text().as_int(0);
	GetClass.MaxStats[0][2] = StatsForClassDW.child("MaxVitalityDW").text().as_int(0);
	GetClass.MaxStats[0][3] = StatsForClassDW.child("MaxEnergyDW").text().as_int(0);
	GetClass.MaxStats[0][4] = StatsForClassDW.child("MaxCommandDW").text().as_int(0);

	xml_node StatsForClassDK = nodeStart.child("StatsDK");
	GetClass.MaxStats[1][0] = StatsForClassDK.child("MaxStrenghtDK").text().as_int(0);
	GetClass.MaxStats[1][1] = StatsForClassDK.child("MaxAgilityDK").text().as_int(0);
	GetClass.MaxStats[1][2] = StatsForClassDK.child("MaxVitalityDK").text().as_int(0);
	GetClass.MaxStats[1][3] = StatsForClassDK.child("MaxEnergyDK").text().as_int(0);
	GetClass.MaxStats[1][4] = StatsForClassDK.child("MaxCommandDK").text().as_int(0);

	xml_node StatsForClassELF = nodeStart.child("StatsELF");
	GetClass.MaxStats[2][0] = StatsForClassELF.child("MaxStrenghtELF").text().as_int(0);
	GetClass.MaxStats[2][1] = StatsForClassELF.child("MaxAgilityELF").text().as_int(0);
	GetClass.MaxStats[2][2] = StatsForClassELF.child("MaxVitalityELF").text().as_int(0);
	GetClass.MaxStats[2][3] = StatsForClassELF.child("MaxEnergyELF").text().as_int(0);
	GetClass.MaxStats[2][4] = StatsForClassELF.child("MaxCommandELF").text().as_int(0);

	xml_node StatsForClassMG = nodeStart.child("StatsMG");
	GetClass.MaxStats[3][0] = StatsForClassMG.child("MaxStrenghtMG").text().as_int(0);
	GetClass.MaxStats[3][1] = StatsForClassMG.child("MaxAgilityMG").text().as_int(0);
	GetClass.MaxStats[3][2] = StatsForClassMG.child("MaxVitalityMG").text().as_int(0);
	GetClass.MaxStats[3][3] = StatsForClassMG.child("MaxEnergyMG").text().as_int(0);
	GetClass.MaxStats[3][4] = StatsForClassMG.child("MaxCommandMG").text().as_int(0);

	xml_node StatsForClassDL = nodeStart.child("StatsDL");
	GetClass.MaxStats[4][0] = StatsForClassDL.child("MaxStrenghtDL").text().as_int(0);
	GetClass.MaxStats[4][1] = StatsForClassDL.child("MaxAgilityDL").text().as_int(0);
	GetClass.MaxStats[4][2] = StatsForClassDL.child("MaxVitalityDL").text().as_int(0);
	GetClass.MaxStats[4][3] = StatsForClassDL.child("MaxEnergyDL").text().as_int(0);
	GetClass.MaxStats[4][4] = StatsForClassDL.child("MaxCommandDL").text().as_int(0);

	xml_node StatsForClassSUMM = nodeStart.child("StatsSUMM");
	GetClass.MaxStats[5][0] = StatsForClassSUMM.child("MaxStrenghtSUMM").text().as_int(0);
	GetClass.MaxStats[5][1] = StatsForClassSUMM.child("MaxAgilitySUMM").text().as_int(0);
	GetClass.MaxStats[5][2] = StatsForClassSUMM.child("MaxVitalitySUMM").text().as_int(0);
	GetClass.MaxStats[5][3] = StatsForClassSUMM.child("MaxEnergySUMM").text().as_int(0);
	GetClass.MaxStats[5][4] = StatsForClassSUMM.child("MaxCommandSUMM").text().as_int(0);

	xml_node StatsForClassRF = nodeStart.child("StatsRF");
	GetClass.MaxStats[6][0] = StatsForClassRF.child("MaxStrenghtRF").text().as_int(0);
	GetClass.MaxStats[6][1] = StatsForClassRF.child("MaxAgilityRF").text().as_int(0);
	GetClass.MaxStats[6][2] = StatsForClassRF.child("MaxVitalityRF").text().as_int(0);
	GetClass.MaxStats[6][3] = StatsForClassRF.child("MaxEnergyRF").text().as_int(0);
	GetClass.MaxStats[6][4] = StatsForClassRF.child("MaxCommandRF").text().as_int(0);

	xml_node StatsForClassGL = nodeStart.child("StatsGL");
	GetClass.MaxStats[7][0] = StatsForClassGL.child("MaxStrenghtGL").text().as_int(0);
	GetClass.MaxStats[7][1] = StatsForClassGL.child("MaxAgilityGL").text().as_int(0);
	GetClass.MaxStats[7][2] = StatsForClassGL.child("MaxVitalityGL").text().as_int(0);
	GetClass.MaxStats[7][3] = StatsForClassGL.child("MaxEnergyGL").text().as_int(0);
	GetClass.MaxStats[7][4] = StatsForClassGL.child("MaxCommandGL").text().as_int(0);

	
}

void CMaxStatsInfo::ReadDrop(const char* File)
{
	using namespace pugi;
	xml_document Document;
	xml_parse_result Result = Document.load_file(File);

	if (Result.status != status_ok)
	{
		g_Log.MsgBox("[IGC_GeneralDrop] File %s not found!", File);
		return;
	}

	xml_node nodeStart = Document.child("GeneralDropRate");

	xml_node CommonDropRate = nodeStart.child("Rate");
	GetGeneralDrop.ItemDropPer = CommonDropRate.child("ItemDropPer").text().as_int(0);
	GetGeneralDrop.ItemLuckDrop = CommonDropRate.child("ItemLuckDrop").text().as_int(0);
	GetGeneralDrop.SkillDropRate = CommonDropRate.child("SkillDropRate").text().as_int(0);
	GetGeneralDrop.ExcItemDropPer = CommonDropRate.child("ExcItemDropPer").text().as_int(0);
	GetGeneralDrop.ExcLuckDropRate = CommonDropRate.child("ExcLuckDropRate").text().as_int(0);
	GetGeneralDrop.ExcSkillDropRate = CommonDropRate.child("ExcSkillDropRate").text().as_int(0);

	xml_node OptionCount = nodeStart.child("ExcMonsterOptCount");
	GetExcellentDrop.MonsterMinOptionCount = OptionCount.child("ExcMinOption").text().as_int(1);
	GetExcellentDrop.MonsterMaxOptionCount = OptionCount.child("ExcMaxOption").text().as_int(6);
}

void CMaxStatsInfo::ReadMapNonPK(const char* File)
{
	using namespace pugi;
	xml_document Document;
	xml_parse_result Result = Document.load_file(File);

	if (Result.status != status_ok)
	{
		g_Log.MsgBox("[IGC_MapNonPK] File %s not found!", File);
		return;
	}

	xml_node CommonMapNonPK = Document.child("MapNonPK");

	for (pugi::xml_node map = CommonMapNonPK.child("Map"); map; map = map.next_sibling())
	{
		int MapNumber = map.attribute("MapID").as_int(-1);

		int PvPType = map.attribute("EnableNonPK").as_int(0);

		GetClass.IsNonPvP[MapNumber] = PvPType;
	}
}



////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

