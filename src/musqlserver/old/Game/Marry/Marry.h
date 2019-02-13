////////////////////////////////////////////////////////////////////////////////
// Marry.h
#ifndef _MU_CMARRY_H_
#define _MU_CMARRY_H_

#include "StdAfx.h"
#include "User/CUserData.h"

class CMarry
{
public:
	CMarry();
	virtual ~CMarry();

	void LoadFile(char *filename);

	void Propose(CGameObject &Obj, int uIndex);
	bool Accept(CGameObject &Obj);
	void Divorce(CGameObject &Obj);

private:
	bool CheckPosition(CGameObject &Obj, int uIndex);
	bool CheckRequiredItem(CGameObject &Obj, int iSide);
	bool CheckDivorceItem(CGameObject &Obj);
	void GiveGiftItem(CGameObject &Obj, int iSide);
	void DeleteRequiredItem(CGameObject &Obj, int iSide);
	void DeleteDivorceItem(CGameObject &Obj);

	bool m_bFileLoad;
	bool m_bMarryEnable;
	int m_iMarryMinUserLevel;
	int m_iMarryNeedMoney;

	WORD m_wMarryMapNumber;
	BYTE m_btMarryStartX;
	BYTE m_btMarryStartY;
	BYTE m_btMarryEndX;
	BYTE m_btMarryEndY;

	bool m_bHomoSexualAllow;
	bool m_bNeedSpecialItemForMarry;

	bool m_bGiftEnable;

	bool m_bDivorceAllow;
	bool m_bNeedSpecialItemForDivorce;

	std::vector<MARRY_ITEM> m_vtMarryItem;
	std::vector<MARRY_ITEM> m_vtGiftItem;
	std::vector<MARRY_ITEM> m_vtDivorceItem;

	CRITICAL_SECTION m_criti;
};

inline bool isFemale(CGameObject &Obj)
{
	return (lpObj.Class == CLASS_ELF || lpObj.Class == CLASS_SUMMONER || lpObj.Class == CLASS_GROWLANCER) ? true : false;
}

inline bool isMale(CGameObject &Obj)
{
	return (lpObj.Class == CLASS_WIZARD || lpObj.Class == CLASS_KNIGHT || lpObj.Class == CLASS_MAGUMSA ||
			lpObj.Class == CLASS_DARKLORD || lpObj.Class == CLASS_RAGEFIGHTER) ? true : false; 
}
extern CMarry Marry;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

