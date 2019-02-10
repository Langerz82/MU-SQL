////////////////////////////////////////////////////////////////////////////////
// MapItem.h
// ------------------------------
// Decompiled by Deathway
// Date : 2007-05-09
// ------------------------------
#ifndef MAPITEM_H
#define MAPITEM_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ItemManagement/Item.h"
#include "User/CUserData.h"

class CMapItem : public CItem
{

public:

	CMapItem();
	virtual ~CMapItem();

	void Init();
	void CreateItem(int type, int level, int x, int y, float dur, BYTE Option1, BYTE Option2, BYTE Option3, BYTE NOption, BYTE SOption, UINT64 number, BYTE ItemEffectEx, BYTE *SocketOption, BYTE SocketBonusOption);
	void DropCreateItem(int type, int level, int x, int y, float dur, BYTE Option1, BYTE Option2, BYTE Option3, BYTE NOption, BYTE SOption, UINT64 number, int PetLevel, UINT64 PetExp, BYTE ItemEffectEx, BYTE *SocketOption, BYTE SocketBonusOption);

public:
	
	BYTE px;	// B4
	BYTE py;	// B5
	bool live;	// B6
	bool Give;	// B7
	DWORD m_State;	// B8
	DWORD m_Time;	// BC
	int m_UserIndex;	// C0
	DWORD m_LootTime;	// C4
	DWORD m_PeriodItemDuration;
	VIEWPORT_PLAYER_STRUCT VpPlayer[MAX_VIEWPORT];	// C8
	int VPCount;	// 44C
};

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

