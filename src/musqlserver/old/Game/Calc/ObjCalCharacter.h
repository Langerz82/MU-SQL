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
	void CalcSetItemStat(CGameObject* lpObj);
	void GetSetItemOption(CGameObject* lpObj, LPBYTE pSetOptionTable, LPBYTE pSetOptionCountTable, int * pSetOptionCount);
	void CalcSetItemOption(CGameObject* lpObj);
	void SetItemStatPlusSpecial(CGameObject* lpObj, int option, int ivalue);
	void SetItemPlusSpecial(CGameObject* lpObj, int option, int ivalue);
	void SetItemApply(CGameObject* lpObj);
	void CalcMLSkillItemOption(CGameObject* lpObj);
	BOOL ValidItem(CGameObject* lpObj, CItem * lpItem, int pos);
	void CalcShieldPoint(CGameObject* lpObj);
	void PremiumItemApply(CGameObject* lpObj);

	MULua m_Lua;

private:
};

extern CObjCalCharacter gObjCalCharacter;
#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

