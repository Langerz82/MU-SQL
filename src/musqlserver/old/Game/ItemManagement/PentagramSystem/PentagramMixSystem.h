////////////////////////////////////////////////////////////////////////////////
// PentagramMixSystem.h
#ifndef PENTAGRAM_MIX_SYSTEM_H
#define PENTAGRAM_MIX_SYSTEM_H

#include "StdAfx.h"
#include "User/CUserData.h"
#include "ItemObject.h"
#include "PentagramSystem.h"

class CPentagramMixSystem
{
public:
	CPentagramMixSystem(void);
	virtual ~CPentagramMixSystem(void);
	friend CPentagramSystem;

	void Initialize_MixNeedSource();
	void Initialize_JewelOption();

	bool LoadMixNeedSourceScript(char *pchFileName);
	bool LoadJewelOptionScript(char *pchFileName);

	int PentagramMixBoxInit(CGameObject* lpObj);
	BYTE PentagramJewelRefine(CGameObject &Obj, int iRefineType);
	BYTE PentagramJewel_Upgrade(CGameObject &Obj, int iUpgradeType, int iTargetValue);
	int CheckLevelCondition(CItemObject *pItem, WORD pLevel, BYTE pOpt1, BYTE pOpt2, BYTE pOpt3, BYTE pSetOpt, BYTE pExOpt);
	void LogPentagramItem(CGameObject lpObj, char *sLogType);

	void CGPentagramJewelRefineRecv(PMSG_PENTAGRAM_JEWEL_REFINE_RECV* lpMsg, CGameObject &lpObj); // OK
	BOOL PentagramJewelMix(CGameObject &lpObj);
	bool IsErrtelType(int ItemCode);
public:
	REFINE_OPTION m_RefineOptionData[18];
	REFINE_SUCCESS_OPTION m_RefineSuccessOptionData[18];
	JEWEL_LEVEL_UPGRADE_NEED_ITEM m_JewelLevelUpgradeNeedItemData[11];
	JEWEL_RANK_UPGRADE_NEED_ITEM m_JewelRankUpgradeNeedItemData[4];
	JEWEL_RANK_UPGRADE_DETERMINE_RATE m_JewelRankUpgradeDetermineRate[4];
	PENTAGRAM_JEWEL_ITEM_OPTION m_JewelItemOptionData[254];
};

extern CPentagramMixSystem g_PentagramMixSystem;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

