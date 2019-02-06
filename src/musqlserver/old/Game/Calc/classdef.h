
#ifndef CLASSDEF_H
#define CLASSDEF_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000.

#include "StdAfx.h"
#include "ItemManagement/Item.h"
#include "Protocol/ProtocolStructs.h"

#define TRACE_PACKET 0	
#define CURRENT_DB_VERSION 3
#define MAX_TYPE_PLAYER 8
#define MAX_NUMBER_MAP 101
#define CHECK_LIMIT(value, limit) (((value)< 0)?FALSE:((value)> limit-1)?FALSE:TRUE)
#define AUTO_REG 0
#define DATASERVER_VERSION "13.0.0.0"

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

#define MAX_ITEM_SPECIAL_ATTRIBUTE 7
#define MAX_SOCKET_OPTION 5

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

