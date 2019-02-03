////////////////////////////////////////////////////////////////////////////////
// classdef.h
// ------------------------------
// Decompiled by Deathway
// Date : 2007-05-09
// ------------------------------
#ifndef CLASSDEF_H
#define CLASSDEF_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000.

#include "zzzitem.h"
#include "prodef.h"

#define MAX_PLAYER_EQUIPMENT 12	// need to redo agin aññ with this macro

enum DBClassNumber
{
	CLASS_WIZARDEVO = 2,
	CLASS_KNIGHTEVO = 18,
	CLASS_ELFEVO = 34,
	CLASS_MAGICEVO = 50,
	CLASS_DARKLORDEVO = 66,
	CLASS_SUMMONEREVO = 82,
	CLASS_RAGEFIGHTEREVO = 98,
	CLASS_GROWLANCEREVO = 114
};

enum ClassNumber
{
	CLASS_WIZARD = 0x0,
	CLASS_KNIGHT = 0x1,
	CLASS_ELF = 0x2,
	CLASS_MAGUMSA = 0x3,
	CLASS_DARKLORD = 0x4,
	CLASS_SUMMONER = 0x5,
	CLASS_RAGEFIGHTER = 0x6,
	CLASS_GROWLANCER = 0x7
};

struct DEFAULTCLASSTYPE
{
	int Experience;	// 0
	WORD Strength;	// 4
	WORD Dexterity;	// 6
	WORD Vitality;	// 8
	WORD Energy;	// A
	float LevelLife;	// C
	float Life;	// 10
	float MaxLife;	// 14
	float LevelMana;	// 18
	float Mana;	// 1C
	float MaxMana;	// 20
	float VitalityToLife;	// 24
	float EnergyToMana;	// 28
	CItem Equipment[MAX_PLAYER_EQUIPMENT+2];	// 2C
	WORD Leadership;	// 9CC
};

struct PMSG_REQ_CLASSDEF
{
	PBMSG_HEAD h;
};

struct CLASSDEF_DATA
{
	int Class;
	int Str;
	int Dex;
	int Vit;
	int Energy;
	float Life;
	float Mana;
	float LevelLife;
	float LevelMana;
	float VitalityToLife;
	float EnergyToMana;
	int Leadership;
};

struct PMSG_ANS_CLASSDEF
{
	PWMSG_HEAD h;
	int iCount;
	CLASSDEF_DATA m_ClassData[MAX_TYPE_PLAYER];
};

class classdef
{
public:

	classdef();
	virtual ~classdef();

	void GDReqClassDefData();
	void DGAnsClassDefData(PMSG_ANS_CLASSDEF *lpMsg);

	void SetCharacter(int Class, int Str, int Dex, int Vit, int Energy, float Life, float Mana, float LevelLife, float LevelMana, float VitalityToLife, float EnergyToMana, int Leadership);
	void SetEquipment(int Class);
	int GetDefPoint(int char_class);

public:

	DEFAULTCLASSTYPE DefClass[MAX_TYPE_PLAYER];	// 4
	
};

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

