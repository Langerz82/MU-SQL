////////////////////////////////////////////////////////////////////////////////
// LuckyItemManager.h
#pragma once

#include "StdAfx.h"
#include "Protocol/protocolStructs.h"

#define	MAX_LUCKYINFO_LEVELRATE			16
#define MAX_LUCKYINFO_ITEMEQUIPMENT		8
#define	MAX_LUCKYINFO_SMELTRATE			10

struct LUCKY_ITEM_INFO;
struct LUCKY_ITEM_EQUIPMENT;


struct LuckyItemManager
{
public:
	LuckyItemManager();
	~LuckyItemManager();

	int		LoadLuckyItemEquipment(int iType, LUCKY_ITEM_INFO * pLuckyItemInfo, char * lpszFileName);
	int		LoadLuckyItemInfo(char * lpszFileName);
	void	AddLuckyItemInfo(LUCKY_ITEM_INFO * pLuckyItemInfo, int iItemCnt);

	LUCKY_ITEM_INFO * GetLuckyItemInfo(int iItemNum);
	LUCKY_ITEM_INFO * GetCharClassLuckyItemInfo(int iItemNum, int iCharClass);
	LUCKY_ITEM_EQUIPMENT * GetLuckyItemEquipment(LUCKY_ITEM_INFO *const pLuckyItemInfo, int iCharClass);

	int		IsLuckyItemTicket(int iItemNum);
	int		IsLuckyItemEquipment(int iItemNum);

	SMELT_RATE * GetSmeltingRateInfo(int iItemNum);

	void	LuckyItemTicketExchange(OBJECTSTRUCT * lpObj);
	BYTE	GambleGetLevel(ITEM_LEVEL_RATE * pItemLevelRandRate, int iIndex);
	void	LuckyItemSmelting(OBJECTSTRUCT * lpObj);
	int		GambleLuckyItemSmelting(int iItemNum, int iDur);
	int		GetLuckyItemDurabilityTime();
	BYTE	GetLevel(int iMinLv, int iMaxLv);
	int		LuckyItemRepaire(OBJECTSTRUCT * lpObj, int source, int target);

	int		GDReqLuckyItemInsert(CItem * pItem, int iObjIndex);
	int		GDReqLuckyItemInsert(short wItemCode, UINT64 dwSerial, short wDurabilitySmall, int iObjIndex);
	void	GDReqLuckyItemInsert2nd(int iObjIndex);
	void	GDReqLuckyItemDelete(short wItemCode, UINT64 dwSerial, int iObjIndex);
	void	DGAnsLuckyItemList(PMSG_ANS_LUCKYITEM_SELECT * lpRecv);
	void	GDReqLuckyItemSelect(OBJECTSTRUCT * lpObj);

private:
	int									m_iTicketCnt;
	std::map<int, _LUCKY_ITEM_INFO>		m_Map_LuckyItemInfo;

}; extern LuckyItemManager g_LuckyItemManager;


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

