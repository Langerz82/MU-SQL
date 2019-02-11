#ifndef _MU_PENTAGRAMSYSTEM_H
#define _MU_PENTAGRAMSYSTEM_H

#include "StdAfx.h"
#include "ItemManagement/Item.h"
#include "User/CUserData.h"

class CPentagramSystem
{
public:
	CPentagramSystem(void);
	virtual ~CPentagramSystem(void);

	void Initialize_Drop();
	bool LoadDropScript(char* pchFileName);
	void Initialize_JewelOutRate();
	bool LoadJewelOutRate(char* pchFileName);
	void Initialize_SetOption();
	bool LoadPentagramSetOptionScript(char* pchFileName);
	void Initialize_PentagramItemOption();
	bool LoadPentagramOptionScript(char* pchFileName);

	bool IsPentagramItem(CItem *lpItemData);
	bool IsPentagramItem(int ItemCode);

	bool IsPentagramMithril(int ItemIndex);
	bool IsPentagramJewel(CItem *lpItemData);
	bool IsPentagramJewel(int ItemCode);

	void ClearPentagramItem(LPGameObject &lpObj);
	void CalcPentagramItem(LPGameObject &lpObj, CItem *lpItemData);

	bool IsEnableDropPentagramItemMap(int iMapIndex);
	int AttributeMonsterItemDrop(LPGameObject &lpObj);

	BYTE GetMakePentagramSlotCountNKind(BYTE *btEnableSlot, int iType);
	bool SetPentagramMainAttribute(CItem *lpItemData, BYTE btAttributeNumber);
	bool MakePentagramSocketSlot(CItem *lpItemData, BYTE btSocketSlot1, BYTE btSocketSlot2, BYTE btSocketSlot3, BYTE btSocketSlot4, BYTE btSocketSlot5);
	bool ClearPentagramSocketSlot(LPGameObject &lpObj, int iInventoryPos, CItem *lpTargetItem, BYTE btSocketSlotIndex);
	bool SetPentagramSocketSlot(CItem *lpTargetItem, BYTE bt1RankOptionNum, BYTE bt1RankLevel, BYTE bt2RankOptionNum, BYTE bt2RankLevel, BYTE bt3RankOptionNum, BYTE bt3RankLevel, BYTE bt4RankOptionNum, BYTE bt4RankLevel, BYTE bt5RankOptionNum, BYTE bt5RankLevel, BYTE curRank);

	bool SwitchPentagramJewel(LPGameObject &lpObj, CItem *lpSourceItem, int iSwitchType);
	bool AddPentagramJewelInfo(LPGameObject &lpObj, int iJewelPos, int iJewelIndex, int iItemType, int iItemIndex, int iMainAttribute, int iJewelLevel, BYTE btRank1OptionNum, BYTE btRank1Level, BYTE btRank2OptionNum, BYTE btRank2Level, BYTE btRank3OptionNum, BYTE btRank3Level, BYTE btRank4OptionNum, BYTE btRank4Level, BYTE btRank5OptionNum, BYTE btRank5Level);
	bool DelPentagramJewelInfo(LPGameObject &lpObj, CItem *lpItemData);
	bool DelPentagramJewelInfo(LPGameObject &lpObj, int iJewelPos, int iJewelIndex);

	void DBREQ_GetPentagramJewel(LPGameObject lpObj, char *szAccountId, int iJewelPos);
	void DBANS_GetPentagramJewel(LPBYTE lpRecv);
	void GCPentagramJewelInfo(LPGameObject &lpObj, int iJewelPos);
	void DBREQ_SetPentagramJewel(LPGameObject &lpObj, int iJewelPos);
	void DBREQ_DelPentagramJewel(LPGameObject &lpObj, int iJewelPos, int iJewelIndex);
	void DBREQ_InsertPentagramJewel(LPGameObject &lpObj, int iJewelPos, int iJewelIndex, int iItemType, int iItemIndex, int iMainAttribute, int iJewelLevel, BYTE btRank1, BYTE btRank1Level, BYTE btRank2, BYTE btRank2Level, BYTE btRank3, BYTE btRank3Level, BYTE btRank4, BYTE btRank4Level, BYTE btRank5, BYTE btRank5Level);

	int PentagramJewel_IN(LPGameObject &lpObj, int iPentagramItemPos, int iJewelItemPos);
	int PentagramJewel_OUT(LPGameObject &lpObj, int iPentagramItemPos, BYTE btSocketIndex, BYTE *btJewelPos, BYTE *btJewelDBIndex);

	bool GCTransPentagramJewelViewInfo(LPGameObject &lpObj, CItem *lpItemData);
	bool GCPShopPentagramJewelViewInfo(LPGameObject &lpObj, int aSourceIndex);
	bool AddRadianceSlot(BYTE TargetSlot);
	int IsEnableToTradePentagramItem(LPGameObject &lpObj);
	int IsEnableTransPentagramJewelInfo(LPGameObject &lpObj, int targetIndex);

	int CheckOverlapMythrilPiece(int iIndex, int iItemType, int iMainAttribute);

	int AddTradeCount(LPGameObject lpObj, int source, int target);

	// fix drop slot pentagrams
	void LoadOptionMaps(const char* File);
	void LoadOptionNews();
	BOOL ElementDrop(LPGameObject &lpObj, CGameObject* lpTargetObj);
	TRandomPoolMgr m_SlotCountRate;

	TEST_ITEMSDROP Penta_DropRate[50];

	int m_iSpiritMap_DropLevel;
	int m_iSpiritMap_DropRate;

	int m_iMithril_DropRate;

	int m_iMurenBook_DropLevel;
	int m_iMurenBook_DropRate;

	int m_iEtramuScroll_DropLevel;
	int m_iEtramuScroll_DropRate;
private:

	SERVER_ATTRIBUTE_DEFINE m_ServerAttributeDefine[14];
	PENTAGRAM_ITEM_OPEN_SOCKET_RATE m_PentagramItemOpenSocketRate[13];
	MONSTER_DROP_ITEM_RATE m_MonsterDropItemRate[30];
	JEWEL_OUT_RATE m_JewelOutRate[55];
	PENTAGRAM_SET_EFFECT m_AttackSetEffect[6];
	PENTAGRAM_SET_EFFECT m_RelationShipSetEffect[20];
	PENTAGRAM_HAVE_SET_OPTION m_PentagramHaveSetOption[20];
	PENTAGRAM_ITEM_OPTION m_PentagramItemOption[20];
	PENTAGRAM_ITEM_OPTION_ENABLE_NEED m_PentagramItemOptionEnableNeed[10];
	PENTAGRAM_SOCKET_RATE_BY_GRADE m_PentagramSocketRateByGrade[3];
};

extern CPentagramSystem g_PentagramSystem;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

