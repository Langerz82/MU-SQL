#ifndef _MU_JEWELMIXSYSTEM_H
#define _MU_JEWELMIXSYSTEM_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"

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
