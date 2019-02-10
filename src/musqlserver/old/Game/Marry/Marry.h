////////////////////////////////////////////////////////////////////////////////
// Marry.h
#ifndef _CMARRY_H_
#define _CMARRY_H_

#include "User/CUserData.h"

struct MARRY_ITEM
{
	int m_iSide;
	WORD m_wItemID;
	int m_iItemCount;
};

class CMarry
{
public:
	CMarry();
	virtual ~CMarry();

	void LoadFile(char *filename);

	void Propose(LPGameObject &lpObj, int uIndex);
	bool Accept(int aIndex);
	void Divorce(int aIndex);

private:
	bool CheckPosition(LPGameObject &lpObj, int uIndex);
	bool CheckRequiredItem(LPGameObject &lpObj, int iSide);
	bool CheckDivorceItem(int aIndex);
	void GiveGiftItem(LPGameObject &lpObj, int iSide);
	void DeleteRequiredItem(LPGameObject &lpObj, int iSide);
	void DeleteDivorceItem(int aIndex);

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

inline bool isFemale(int aIndex)
{
	return (lpObj->Class == CLASS_ELF || lpObj->Class == CLASS_SUMMONER || lpObj->Class == CLASS_GROWLANCER) ? true : false;
}

inline bool isMale(int aIndex)
{
	return (lpObj->Class == CLASS_WIZARD || lpObj->Class == CLASS_KNIGHT || lpObj->Class == CLASS_MAGUMSA ||
			lpObj->Class == CLASS_DARKLORD || lpObj->Class == CLASS_RAGEFIGHTER) ? true : false; 
}
extern CMarry Marry;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

