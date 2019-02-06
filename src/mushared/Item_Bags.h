////////////////////////////////////////////////////////////////////////////////
// Item_Bags.h
#pragma once
// -------------------------------------------------------------------------------

#include "TRandomPoolMgr.h"
// -------------------------------------------------------------------------------

#define FILE_EVENTITEMBAG	"Custom\\GoldenInvasion.xml"
#define PATH_EVENTITEMBAG	"Custom\\GoldenInvasion\\"

#define EVENTBAG_MAX_RATE			10000
#define EVENTBAG_TYPE_ITEM			0
#define EVENTBAG_TYPE_MONSTER		1
#define EVENTBAG_TYPE_SPECIAL		2
#define EVENTBAG_EFFECT_NONE		0
#define EVENTBAG_EFFECT_FIREWORK	1
#define EVENTBAG_EFFECT_CHERRY		2
// -------------------------------------------------------------------------------

namespace EventBagSpecial
{
	enum Type
	{
		GoldenBudgeDragon = 0, // 43
		GoldenTitan = 1, // 53
		GoldenSoldier = 2, // 54
		GoldenGoblin = 3, // 78
		GoldenDerkon = 4, // 79
		GoldenLizard = 5, // 80
		GoldenVepar = 6, // 81
		GoldenTantalos = 7, // 82
		GoldenWheel = 8, // 83
		GoldenDarkKnight = 9, // 493
		GoldenDevil = 10, // 494
		GoldenStoneGolem = 11, // 495
		GoldenCrust = 12, // 496
		GoldenSatyros = 13, // 497
		GoldenTwinTail = 14, // 498
		GoldenIronKnight = 15, // 499
		GoldenNapin = 16, // 500
		GoldenGreatDragon = 17, // 501
		GoldenRabbit = 18 // 502
	};
};
// -------------------------------------------------------------------------------

struct EventItem
{
	EventItem()
	{
		this->m_Type = 0;
		this->m_Index = 0;
		this->m_MinLevel = 0;
		this->m_MaxLevel = 0;
		this->m_IsSkill = false;
		this->m_IsLuck = false;
		this->m_IsOption = false;
		this->m_MinExcCount = 0;
		this->m_MaxExcCount = 0;
		this->m_IsSet = false;
	}
	// ----
	BYTE m_Type;
	BYTE m_Index;
	BYTE m_MinLevel;
	BYTE m_MaxLevel;
	BYTE m_IsSkill;
	BYTE m_IsLuck;
	BYTE m_IsOption;
	BYTE m_MinExcCount;
	BYTE m_MaxExcCount;
	BYTE m_IsSet;
};
// -------------------------------------------------------------------------------

struct EventBag
{
	BYTE m_ItemCount;
	int m_Rate;
	int m_MaxRate;
	std::vector<EventItem> m_ItemList;
};
// -------------------------------------------------------------------------------

class EventItemBag
{
public:
	EventItemBag();
	virtual ~EventItemBag();
	// ----
	void	Init();
	void	Read(LPSTR File);
	// ----
	LPSTR	GetBagName() { return &this->m_Name[0]; };
	EventItem* GetItem(int Rate);
	bool	Open(int UserIndex, int MapNumber, BYTE X, BYTE Y);
	// ----
	//private:
	bool	m_IsLoaded;
	BYTE	m_Mode;		//-> 0: item drop event, 1: monster die event, 2: special event (mix and etc)
	char	m_Name[255];
	char	m_File[MAX_PATH];
	int		m_MinMoney;
	int		m_MaxMoney;
	WORD	m_ItemType;
	WORD	m_ItemIndex;
	BYTE	m_ItemLevel;
	WORD	m_MinUserLevel;
	BYTE	m_Effect;
	WORD	m_MonsterID;
	BYTE	m_Repeat;
	int		m_SetItemRate;
	BYTE	m_Notify;
	BYTE	m_SpecialType;
	TRandomPoolMgr m_ExcellentPool;
	// ----
	std::vector<EventBag> m_GroupList;
};
// -------------------------------------------------------------------------------

class EventItemBagManager
{
public:
	EventItemBagManager();
	virtual	~EventItemBagManager();
	// ----
	void	Init();
	void	Load();
	void	Read(LPSTR File);
	// ----
	BYTE	OpenBox(int UserIndex, int ItemNumber, BYTE ItemLevel);
	BYTE	OpenMonster(int MonsterIndex);
	BYTE	OpenSpecial(BYTE SpecialType, int UserIndex, int MapNumber, BYTE X, BYTE Y);
	// ----
private:
	std::vector<EventItemBag> m_ItemBagList;
	// ----
}; extern EventItemBagManager g_EventItemBagManager;
// -------------------------------------------------------------------------------


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

