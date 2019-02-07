////////////////////////////////////////////////////////////////////////////////
// SetItemDrop.h
#include "User/user.h"

#pragma once

struct SETITEM_DROP_ITEM
{
	BYTE	ItemCat;
	BYTE	MinLevel;
	BYTE	MaxLevel;
	BYTE	ItemCount;
	BYTE	Skill;
	BYTE	Luck;
	BYTE	Option;
	BYTE	ExcOption[9];
	DWORD	DropRate;
};

struct SETITEM_DROP_RULE
{
	WORD MonsterIndex;
	WORD MonsterMinLevel;
	WORD MonsterMaxLevel;
	WORD PlayerMinLevel;
	WORD PlayerMaxLevel;
	BYTE MapNumber;
	std::vector<SETITEM_DROP_ITEM> m_vtSetItems;
};

class CSetItemDrop
{
public:
	CSetItemDrop(void);
	virtual ~CSetItemDrop(void);

	void LoadFile(LPSTR lpFile);
	bool DropItem(LPOBJ lpMonsterObj, LPOBJ lpObj);

	inline int GetItemLevel(int min, int max)
	{
		int diff = max-min;
		int level = rand()%(diff+1);
	
		return level += min;
	}

private:

	SETITEM_DROP_RULE * GetDropRule(OBJECTSTRUCT *lpUser, OBJECTSTRUCT *lpMonster);
	SETITEM_DROP_ITEM * GetItemData(SETITEM_DROP_RULE *lpRule);

	std::vector<SETITEM_DROP_RULE> m_vtDropRules;
	DWORD m_dwDropUseRate;
};

extern CSetItemDrop SetItemDrop;


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

