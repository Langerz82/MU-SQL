////////////////////////////////////////////////////////////////////////////////
// ObjCalCharacter.h
#ifndef OBJCALCHARACTER_H
#define OBJCALCHARACTER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "User/CUserData.h"
#include "ItemManagement/Item.h"

class CObjCalCharacter
{
public:

	CObjCalCharacter();
	virtual ~CObjCalCharacter();
	
	void Init();

	void CalcCharacter(int aIndex);
	void CalcSetItemStat(LPGameObject &lpObj);
	void GetSetItemOption(LPGameObject &lpObj, LPBYTE pSetOptionTable, LPBYTE pSetOptionCountTable, int * pSetOptionCount);
	void CalcSetItemOption(LPGameObject &lpObj);
	void SetItemStatPlusSpecial(LPGameObject &lpObj, int option, int ivalue);
	void SetItemPlusSpecial(LPGameObject &lpObj, int option, int ivalue);
	void SetItemApply(LPGameObject &lpObj);
	void CalcMLSkillItemOption(LPGameObject &lpObj);
	BOOL ValidItem(LPGameObject &lpObj, CItem * lpItem, int pos);
	void CalcShieldPoint(LPGameObject &lpObj);
	void PremiumItemApply(LPGameObject &lpObj);

	MULua m_Lua;

private:
};

extern CObjCalCharacter gObjCalCharacter;
#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

