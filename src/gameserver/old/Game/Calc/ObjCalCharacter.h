////////////////////////////////////////////////////////////////////////////////
// ObjCalCharacter.h
#ifndef OBJCALCHARACTER_H
#define OBJCALCHARACTER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "User/CUserData.h"
#include "ItemObject.h"
#include "CGameObject.h"

class CGameObject;

class CObjCalCharacter
{
public:

	CObjCalCharacter();
	virtual ~CObjCalCharacter();
	
	void Init();

	void CalcCharacter(CGameObject &Obj);
	void CalcSetItemStat(CGameObject &Obj);
	void GetSetItemOption(CGameObject &Obj, LPBYTE pSetOptionTable, LPBYTE pSetOptionCountTable, int * pSetOptionCount);
	void CalcSetItemOption(CGameObject &Obj);
	void SetItemStatPlusSpecial(CGameObject &Obj, int option, int ivalue);
	void SetItemPlusSpecial(CGameObject &Obj, int option, int ivalue);
	void SetItemApply(CGameObject &Obj);
	void CalcMLSkillItemOption(CGameObject &Obj);
	BOOL ValidItem(CGameObject &Obj, class CItemObject * lpItem, int pos);
	void CalcShieldPoint(CGameObject &Obj);
	void PremiumItemApply(CGameObject &Obj);

	MULua m_Lua;

private:
};

extern CObjCalCharacter gObjCalCharacter;
#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

