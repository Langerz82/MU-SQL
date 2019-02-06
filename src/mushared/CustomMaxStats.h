////////////////////////////////////////////////////////////////////////////////
// CustomMaxStats.h
#pragma once

#include "classdef.h"
#include "MapClass.h"

#define FILE_CUSTOM_MAXSTATS "IGC_PlayerInfo.xml"

#define FILE_CUSTOM_GENERALDROP "IGC_GeneralDrop.xml"

#define FILE_CUSTOM_MAPSNONPK "IGC_MapNonPK.xml"

struct MaxStatsInfo
{
	int MaxStats[8][5];
	int LevelUpPointNormal;
	int LevelUpPointMGDL;
	int Max_Party_Level_Dif;
	int MLUserMaxLevel;
	int MLPointPerLevel;
	int MLMonsterMinLevel;
	int m_MinReset;
	bool IsNonPvP[MAX_NUMBER_MAP];
};

struct ExcellentOptCount
{
	int MonsterMinOptionCount;
	int MonsterMaxOptionCount;
};

struct MaxGeneralDrop
{
	int ItemDropPer;
	int ExcItemDropPer;
	int ItemLuckDrop;
	int SkillDropRate;
	int ExcLuckDropRate;
	int ExcSkillDropRate;
};

class CMaxStatsInfo
{
public:
	CMaxStatsInfo();
	~CMaxStatsInfo();

	void ReadDrop(const char* File);
	void ReadMapNonPK(const char* File);
	void Read(const char* File);
	MaxStatsInfo GetClass;
	MaxGeneralDrop GetGeneralDrop;
	ExcellentOptCount GetExcellentDrop;
};

extern CMaxStatsInfo g_MaxStatsInfo;

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

