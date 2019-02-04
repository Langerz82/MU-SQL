////////////////////////////////////////////////////////////////////////////////
// ObjCalCharacter.h
#ifndef OBJCALCHARACTER_H
#define OBJCALCHARACTER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "user.h"
#include "zzzitem.h"

class CObjCalCharacter
{
public:

	CObjCalCharacter();
	virtual ~CObjCalCharacter();
	
	void Init();

	void CalcCharacter(int aIndex);
	void CalcSetItemStat(LPOBJ lpObj);
	void GetSetItemOption(LPOBJ lpObj, LPBYTE pSetOptionTable, LPBYTE pSetOptionCountTable, int * pSetOptionCount);
	void CalcSetItemOption(LPOBJ lpObj);
	void SetItemStatPlusSpecial(LPOBJ lpObj, int option, int ivalue);
	void SetItemPlusSpecial(LPOBJ lpObj, int option, int ivalue);
	void SetItemApply(LPOBJ lpObj);
	void CalcMLSkillItemOption(LPOBJ lpObj);
	BOOL ValidItem(LPOBJ lpObj, CItem * lpItem, int pos);
	void CalcShieldPoint(LPOBJ lpObj);
	void PremiumItemApply(LPOBJ lpObj);

	MULua m_Lua;

private:
};

extern CObjCalCharacter gObjCalCharacter;
#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

