////////////////////////////////////////////////////////////////////////////////
// LuckyItemManager.h
#pragma once

#include "StdAfx.h"
#include "Protocol/protocolStructs.h"
#include "generalStructs.h"

struct STR_LUCKY_ITEM_INFO;
struct STR_LUCKY_ITEM_EQUIPMENT;


struct LuckyItemManager
{
public:
	LuckyItemManager();
	~LuckyItemManager();

	int		LoadLuckyItemEquipment(int iType, STR_LUCKY_ITEM_INFO * pLuckyItemInfo, char * lpszFileName);
	int		LoadLuckyItemInfo(char * lpszFileName);
	void	AddLuckyItemInfo(STR_LUCKY_ITEM_INFO * pLuckyItemInfo, int iItemCnt);

	STR_LUCKY_ITEM_INFO * GetLuckyItemInfo(int iItemNum);
	STR_LUCKY_ITEM_INFO * GetCharClassLuckyItemInfo(int iItemNum, int iCharClass);
	STR_LUCKY_ITEM_EQUIPMENT * GetLuckyItemEquipment(STR_LUCKY_ITEM_INFO *const pLuckyItemInfo, int iCharClass);

	int		IsLuckyItemTicket(int iItemNum);
	int		IsLuckyItemEquipment(int iItemNum);

	STR_SMELT_RATE * GetSmeltingRateInfo(int iItemNum);

	void	LuckyItemTicketExchange(CGameObject &lpObj);
	BYTE	GambleGetLevel(STR_ITEM_LEVEL_RATE * pItemLevelRandRate, CGameObject &Obj);
	void	LuckyItemSmelting(CGameObject &lpObj);
	int		GambleLuckyItemSmelting(int iItemNum, int iDur);
	int		GetLuckyItemDurabilityTime();
	BYTE	GetLevel(int iMinLv, int iMaxLv);
	int		LuckyItemRepaire(CGameObject &lpObj, int source, int target);

	int		GDReqLuckyItemInsert(CItem * pItem, int iObjIndex);
	int		GDReqLuckyItemInsert(short wItemCode, UINT64 dwSerial, short wDurabilitySmall, int iObjIndex);
	void	GDReqLuckyItemInsert2nd(int iObjIndex);
	void	GDReqLuckyItemDelete(short wItemCode, UINT64 dwSerial, int iObjIndex);
	void	DGAnsLuckyItemList(PMSG_ANS_LUCKYITEM_SELECT * lpRecv);
	void	GDReqLuckyItemSelect(CGameObject &lpObj);

private:
	int									m_iTicketCnt;
	std::map<int, STR_LUCKY_ITEM_INFO>		m_Map_LuckyItemInfo;

}; extern LuckyItemManager g_LuckyItemManager;


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

