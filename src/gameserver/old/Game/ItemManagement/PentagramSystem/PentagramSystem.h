#ifndef _MU_PENTAGRAMSYSTEM_H
#define _MU_PENTAGRAMSYSTEM_H

#include "StdAfx.h"
#include "ItemObject.h"
#include "CGameObject.h"
#include "TRandomPoolMgr.h"


struct SERVER_ATTRIBUTE_DEFINE
{
	void Clear()
	{
		this->ListIndex = 0;
		this->ServerIndex = 0;
		this->FireRate = 0;
		this->WaterRate = 0;
		this->WindRate = 0;
		this->EarthRate = 0;
		this->DarkRate = 0;
	}

	int ListIndex;
	int ServerIndex;
	int FireRate;
	int WaterRate;
	int WindRate;
	int EarthRate;
	int DarkRate;
};

struct PENTAGRAM_ITEM_OPEN_SOCKET_RATE
{
	void Clear()
	{
		this->SocketCount = 0;
		this->SocketOpenRate = 0;
		this->SocketOpenSet = 0;
		this->Slot_1 = 0;
		this->Slot_2 = 0;
		this->Slot_3 = 0;
		this->Slot_4 = 0;
		this->Slot_5 = 0;
	}

	int SocketCount;
	int SocketOpenRate;
	int SocketOpenSet;
	int Slot_1;
	int Slot_2;
	int Slot_3;
	int Slot_4;
	int Slot_5;
};

struct MONSTER_DROP_ITEM_RATE
{
	void Clear()
	{
		this->MonsterClass = 0;

		for (int i = 0; i<5; i++)
		{
			this->DropItemType[i] = 0;
			this->DropItemIndex[i] = 0;
			this->DropItemRate[i] = 0;
		}
	}

	int MonsterClass;
	int DropItemType[6];
	int DropItemIndex[6];
	int DropItemRate[6];
};

struct JEWEL_OUT_RATE
{
	void Clear()
	{
		this->JewelOutIndex = 0;
		this->JewelRank = 0;
		this->JewelLevel = 0;
		this->OutRate = 0;
	}

	int JewelOutIndex;
	int JewelRank;
	int JewelLevel;
	int OutRate;
};

struct PENTAGRAM_SET_EFFECT
{
	void Clear()
	{
		this->SetIndex = 0;

		for (int i = 0; i < 6; i++)
		{
			this->RON[i] = 0;
		}

		this->Value1 = 0;
		this->Value2 = 0;
		this->Value3 = 0;
		this->Value4 = 0;
	}

	int SetIndex;
	int RON[6];
	int Value1;
	int Value2;
	int Value3;
	int Value4;
};

struct PENTAGRAM_HAVE_SET_OPTION
{
	int BundleIndex;
	int ItemType;
	int ItemIndex;
	int SetOptionIndex[26];
};

struct PENTAGRAM_ITEM_OPTION
{
	void Clear()
	{
		this->ItemType = 0;
		this->ItemIndex = 0;
		memset(this->ItemName, 0, sizeof(this->ItemName));
		this->Grade = 0;

		for (int i = 0; i < 7; i++)
		{
			this->OptionNum[i] = -1;
		}
	}

	int ItemType;
	int ItemIndex;
	char ItemName[64];
	int Grade;
	int OptionNum[7];
};

struct PENTAGRAM_ITEM_OPTION_ENABLE_NEED
{
	void Clear()
	{
		this->OptionNum = 0;
		memset(this->OptionName, 0, sizeof(this->OptionName));

		for (int i = 0; i < 3; i++)
		{
			this->Need_ErrtelKind[i] = -1;
			this->Need_ErrtelRank[i] = -1;
			this->Need_ErrtelLevel[i] = -1;
		}
	}

	int OptionNum;
	char OptionName[64];
	int Need_ErrtelKind[3];
	int Need_ErrtelRank[3];
	int Need_ErrtelLevel[3];
};

struct PENTAGRAM_SOCKET_RATE_BY_GRADE
{
	void Clear()
	{
		this->Grade = 0;

		for (int i = 0; i < 5; i++)
		{
			this->SocketRate[i] = 0;
		}
	}

	int Grade;
	int SocketRate[5];
};

#pragma pack (1)
struct PMSG_ANS_PENTAGRAMJEWEL
{
	PWMSG_HEAD h;
	BYTE btJewelCnt;
	int iUserIndex;
	int iAnsType;
};
#pragma pack ()

enum
{
	ELEMENT_NONE = 0,
	ELEMENT_FIRE = 1,
	ELEMENT_WATER = 2,
	ELEMENT_EARTH = 3,
	ELEMENT_WIND = 4,
	ELEMENT_DARK = 5,
};

struct TEST_ITEMSDROP
{
	int ItemDropIndex;
	int ItemDropLevel;
	int ItemDropRate;
};


class CItemObject;
class TRandomPoolMgr;


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

	bool IsPentagramItem(CItemObject &Item);
	bool IsPentagramItem(int ItemCode);

	bool IsPentagramMithril(int ItemIndex);
	bool IsPentagramJewel(CItemObject &Item);
	bool IsPentagramJewel(int ItemCode);

	void ClearPentagramItem(CGameObject &Obj);
	void CalcPentagramItem(CGameObject &Obj, CItemObject *lpItemData);

	bool IsEnableDropPentagramItemMap(int iMapIndex);
	int AttributeMonsterItemDrop(CGameObject &Obj);

	BYTE GetMakePentagramSlotCountNKind(BYTE *btEnableSlot, int iType);
	bool SetPentagramMainAttribute(CItemObject *lpItemData, BYTE btAttributeNumber);
	bool MakePentagramSocketSlot(CItemObject *lpItemData, BYTE btSocketSlot1, BYTE btSocketSlot2, BYTE btSocketSlot3, BYTE btSocketSlot4, BYTE btSocketSlot5);
	bool ClearPentagramSocketSlot(CGameObject &Obj, int iInventoryPos, CItemObject *lpTargetItem, BYTE btSocketSlotIndex);
	bool SetPentagramSocketSlot(CItemObject *lpTargetItem, BYTE bt1RankOptionNum, BYTE bt1RankLevel, BYTE bt2RankOptionNum, BYTE bt2RankLevel, BYTE bt3RankOptionNum, BYTE bt3RankLevel, BYTE bt4RankOptionNum, BYTE bt4RankLevel, BYTE bt5RankOptionNum, BYTE bt5RankLevel, BYTE curRank);

	bool SwitchPentagramJewel(CGameObject &Obj, CItemObject *lpSourceItem, int iSwitchType);
	bool AddPentagramJewelInfo(CGameObject &Obj, int iJewelPos, int iJewelIndex, int iItemType, int iItemIndex, int iMainAttribute, int iJewelLevel, BYTE btRank1OptionNum, BYTE btRank1Level, BYTE btRank2OptionNum, BYTE btRank2Level, BYTE btRank3OptionNum, BYTE btRank3Level, BYTE btRank4OptionNum, BYTE btRank4Level, BYTE btRank5OptionNum, BYTE btRank5Level);
	bool DelPentagramJewelInfo(CGameObject &Obj, CItemObject *lpItemData);
	bool DelPentagramJewelInfo(CGameObject &Obj, int iJewelPos, int iJewelIndex);

	void DBREQ_GetPentagramJewel(CGameObject &Obj, char *szAccountId, int iJewelPos);
	void DBANS_GetPentagramJewel(LPBYTE lpRecv);
	void GCPentagramJewelInfo(CGameObject &Obj, int iJewelPos);
	void DBREQ_SetPentagramJewel(CGameObject &Obj, int iJewelPos);
	void DBREQ_DelPentagramJewel(CGameObject &Obj, int iJewelPos, int iJewelIndex);
	void DBREQ_InsertPentagramJewel(CGameObject &Obj, int iJewelPos, int iJewelIndex, int iItemType, int iItemIndex, int iMainAttribute, int iJewelLevel, BYTE btRank1, BYTE btRank1Level, BYTE btRank2, BYTE btRank2Level, BYTE btRank3, BYTE btRank3Level, BYTE btRank4, BYTE btRank4Level, BYTE btRank5, BYTE btRank5Level);

	int PentagramJewel_IN(CGameObject &Obj, int iPentagramItemPos, int iJewelItemPos);
	int PentagramJewel_OUT(CGameObject &Obj, int iPentagramItemPos, BYTE btSocketIndex, BYTE *btJewelPos, BYTE *btJewelDBIndex);

	bool GCTransPentagramJewelViewInfo(CGameObject &Obj, CItemObject *lpItemData);
	bool GCPShopPentagramJewelViewInfo(CGameObject &Obj, int aSourceIndex);
	bool AddRadianceSlot(BYTE TargetSlot);
	int IsEnableToTradePentagramItem(CGameObject &Obj);
	int IsEnableTransPentagramJewelInfo(CGameObject &Obj, int targetIndex);

	int CheckOverlapMythrilPiece(CGameObject &Obj, int iItemType, int iMainAttribute);

	int AddTradeCount(CGameObject &Obj, int source, int target);

	// fix drop slot pentagrams
	void LoadOptionMaps(const char* File);
	void LoadOptionNews();
	BOOL ElementDrop(CGameObject &Obj, CGameObject lpTargetObj);
	TRandomPoolMgr m_SlotCountRate;

	TEST_ITEMSDROP* Penta_DropRate;

	int m_iSpiritMap_DropLevel;
	int m_iSpiritMap_DropRate;

	int m_iMithril_DropRate;

	int m_iMurenBook_DropLevel;
	int m_iMurenBook_DropRate;

	int m_iEtramuScroll_DropLevel;
	int m_iEtramuScroll_DropRate;
private:

	SERVER_ATTRIBUTE_DEFINE* m_ServerAttributeDefine;
	PENTAGRAM_ITEM_OPEN_SOCKET_RATE* m_PentagramItemOpenSocketRate;
	MONSTER_DROP_ITEM_RATE* m_MonsterDropItemRate;
	JEWEL_OUT_RATE* m_JewelOutRate;
	PENTAGRAM_SET_EFFECT* m_AttackSetEffect;
	PENTAGRAM_SET_EFFECT* m_RelationShipSetEffect;
	PENTAGRAM_HAVE_SET_OPTION* m_PentagramHaveSetOption;
	PENTAGRAM_ITEM_OPTION* m_PentagramItemOption;
	PENTAGRAM_ITEM_OPTION_ENABLE_NEED* m_PentagramItemOptionEnableNeed;
	PENTAGRAM_SOCKET_RATE_BY_GRADE* m_PentagramSocketRateByGrade;
};

extern CPentagramSystem g_PentagramSystem;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

