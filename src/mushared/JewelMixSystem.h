////////////////////////////////////////////////////////////////////////////////
// JewelMixSystem.h
// ------------------------------
// Decompiled by Deathway
// Date : 2007-05-09
// ------------------------------
#ifndef JEWELMIXSYSTEM_H
#define JEWELMIXSYSTEM_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CJewelMixSystem
{

public:

	CJewelMixSystem();
	virtual ~CJewelMixSystem();

	static void LoadMixJewelPrice();

	static int GetJewelCount(int iIndex, int iJewelType);
	static int GetJewelCountPerLevel(int iJewelType, int iJewelLevel);
	static int MixJewel(int iIndex, int iJewelType, int iMixType);
	static int UnMixJewel(int iIndex, int iJewelType, int iJewelLevel, int iInventoryPos);

private:
	static int UnMixJewelPrice;
};

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

