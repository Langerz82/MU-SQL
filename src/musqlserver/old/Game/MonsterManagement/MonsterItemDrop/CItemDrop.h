////////////////////////////////////////////////////////////////////////////////
// CItemObjectDrop.h
#ifndef _MU_ITEMDROP_H
#define _MU_ITEMDROP_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "User/CUserData.h"


class CItemObjectDrop
{
public:
	CItemObjectDrop(void);
	virtual ~CItemObjectDrop(void);

	bool LoadFile(const char* szFile);
	bool LoadZenDropFile(const char* szFile);
	bool DropItem(CGameObject &lpUser, CGameObject &lpMonster);
	bool IsZenDropActive(BYTE btMapNumber);
	UINT64 GetZenAmount(BYTE btMapNumber, WORD wMonsterLevel);

private:

	inline int GetItemLevel(int min, int max)
	{
		int diff;
		__try
		{
			diff = max-min;
			int level = rand()%(diff+1);
	
			return level += min;
		}

		__except(diff=1, 1)
		{
			return min;
		}
	}

	ITEMDROP_ITEM ** GetItem(CGameObject &lpUser, CGameObject &lpMonster, int & ItemCount);

	boost::shared_ptr<ITEMDROP_MONSTER> GetMonsterData(CGameObject &lpUser, CGameObject &lpMonster);
	boost::shared_ptr<ITEMDROP_ITEM> GetItemFromMonster(boost::shared_ptr<ITEMDROP_MONSTER> itemdrop_monster_ptr);

	void AddCoinReward(boost::shared_ptr<ITEMDROP_MONSTER> itemdrop_monster_ptr, CGameObject &lpUser);

	std::vector<boost::shared_ptr<ITEMDROP_MONSTER>> m_vtMonsterList;
	std::map<int, ZEN_DROP> m_mapZenDrop;
	DWORD m_dwDropUseRate;

	bool m_bZenDropEnable;
	bool m_bMultiplyZenDropByMonLevel;
	DWORD m_dwMultiplyZenChanceRate;
};

extern CItemObjectDrop ItemDrop;

#endif
