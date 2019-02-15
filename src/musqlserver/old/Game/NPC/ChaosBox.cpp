////////////////////////////////////////////////////////////////////////////////
// ChaosBox.cpp
#include "StdAfx.h"
#include "ChaosBox.h"
#include "Logging/Log.h"
#include "GameMain.h"
#include "util.h"

#include "CastleSiegeSync.h"
#include "Event/BloodCastle/BloodCastle.h"
#include "DevilSquare.h"
#include "SetItemOption.h"
#include "JewelOfHarmonySystem.h"
#include "IOCP.h"
#include "CrywolfSync.h"
#include "CashLotterySystem.h"
#include "configread.h"
#include "ImperialGuardian.h"
#include "BagManager.h"
#include "LuckyItemManager.h"
#include "ItemSocketOptionSystem.h"
#include "PentagramSystem.h"
#include "VipSys.h"
#include "RandomManager.h"
#include "LargeRand.h"
#include "ItemOptionRate.h"
#include "SocketItemType.h"

CMixSystem g_MixSystem;

CMixSystem::CMixSystem()
{
	this->CHAOS_GEM_ITEMINDEX = ITEMGET(12, 15);
	this->GUARDIAN_GEM_ITEMINDEX = ITEMGET(14, 31);
	this->BLESS_GEM_ITEMINDEX = ITEMGET(14, 13);
	this->SOUL_GEM_ITEMINDEX = ITEMGET(14, 14);
	this->CREATURE_GEM_ITEMINDEX = ITEMGET(14, 22);
	this->BLESS_GEM_BUNDLE_ITEMINDEX = ITEMGET(12, 30);
	this->SOUL_GEM_BULDLE_ITEMINDEX = ITEMGET(12, 31);
	this->CONDOR_FEATHER_ITEMINDEX = ITEMGET(13, 53);
	this->CONDOR_FLAME_ITEMINDEX = ITEMGET(13, 52);
	this->WING_OF_STORM_ITEMINDEX = ITEMGET(12, 36);
	this->WING_OF_ETERNAL_ITEMINDEX = ITEMGET(12, 37);
	this->WING_OF_ILLUSION_ITEMINDEX = ITEMGET(12, 38);
	this->WING_OF_RUIN_ITEMINDEX = ITEMGET(12, 39);
	this->CAPE_OF_EMPEROR_ITEMINDEX = ITEMGET(12, 40);
	this->WING_OF_DIMENSION_ITEMINDEX = ITEMGET(12, 43);
	this->CAPE_OF_OVERRULE_ITEMINDEX = ITEMGET(12, 50);
	this->CLOAK_OF_TRANSCEDENCE_ITEMINDEX = ITEMGET(12, 270);
	this->FRUIT_ITEMINDEX = ITEMGET(13, 15);
	this->DINORANT_ITEMINDEX = ITEMGET(13, 3);
	this->BLOODCASTLE_INVITATION_ITEMINDEX = ITEMGET(13, 38);
	this->DEVILSQUARE_INVITATION_ITEMINDEX = ITEMGET(14, 19);
	this->SMALL_SD_POTION_ITEMINDEX = ITEMGET(14, 35);
	this->MEDIUM_SD_POTION_ITEMINDEX = ITEMGET(14, 36);
	this->LARGE_SD_POTION_ITEMINDEX = ITEMGET(14, 37);
	this->BLESS_POTION_ITEMINDEX = ITEMGET(14, 7);
	this->SOUL_POTION_ITEMINDEX = ITEMGET(14, 7);
	this->GOLDEN_BOX_ITEMINDEX = ITEMGET(14, 123);
	this->SILVER_BOX_ITEMINDEX = ITEMGET(14, 124);
	this->HIGH_REFINE_STONE_ITEMINDEX = ITEMGET(14, 44);
	this->LOW_REFINE_STONE_ITEMINDEX = ITEMGET(14, 43);
	this->HARMONY_JEWEL_ITEMINDEX = ITEMGET(14, 42);
	this->m_i3rdWingOpt_LuckRate = 50;
}

CMixSystem::~CMixSystem()
{

}

BOOL CMixSystem::ChaosBoxCheck(CGameObject &Obj)
{
	if (Obj.pChaosBox == NULL)
	{
		return false;
	}

	return true;
}

void CMixSystem::GCChaosMixSend(CGameObject &Obj, BYTE result, CItemObject* lpItem) // OK
{
	PMSG_CHAOSMIXRESULT pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0x86, sizeof(pMsg));

	pMsg.Result = result;

	if (lpItem == 0)
	{
		memset(pMsg.ItemInfo, 0xFF, sizeof(pMsg.ItemInfo));
	}
	else
	{
		ItemByteConvert(pMsg.ItemInfo, (*lpItem));
	}

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

	Obj.ChaosLock = 0;
}

bool CMixSystem::GetElementalTalismanOfLuckRate(CGameObject &Obj, int* rate) // OK
{
	int count = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 189)) // Elemental Talisman of Luck
		{
			count += (int)Obj.pChaosBox[n].m_Durability;
		}
	}

	(*rate) += count;

	return ((count > MAX_TALISMAN_OF_LUCK) ? 0 : 1);
}

bool CMixSystem::GetTalismanOfLuckRate(CGameObject &Obj, int* rate) // OK
{
	int count = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 53)) // Talisman of Luck
		{
			count += (int)Obj.pChaosBox[n].m_Durability;
		}
	}

	(*rate) += count;

	return ((count > MAX_TALISMAN_OF_LUCK) ? 0 : 1);
}
BOOL CMixSystem::ChaosBoxInit(CGameObject &Obj)
{
	if (Obj.pChaosBox == NULL)
	{
		return false;
	}

	if (Obj.pChaosBoxMap == NULL)
	{
		return false;
	}

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		Obj.pChaosBox[n].Clear();
	}

	memset(Obj.pChaosBoxMap, (BYTE)-1, CHAOS_BOX_MAP_SIZE);

	return true;
}


BOOL CMixSystem::ChaosBoxItemDown(CGameObject &Obj)
{
	if (Obj.pChaosBox == NULL)
	{
		return FALSE;
	}

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 15) || Obj.pChaosBox[n].m_Type == ITEMGET(14, 13) || Obj.pChaosBox[n].m_Type == ITEMGET(14, 14) ||
			Obj.pChaosBox[n].m_Type == ITEMGET(13, 83) || Obj.pChaosBox[n].m_Type == ITEMGET(13, 84) || Obj.pChaosBox[n].m_Type == ITEMGET(13, 85) ||
			Obj.pChaosBox[n].m_Type == ITEMGET(13, 86) || Obj.pChaosBox[n].m_Type == ITEMGET(13, 87) || Obj.pChaosBox[n].m_Type == ITEMGET(13, 88) ||
			Obj.pChaosBox[n].m_Type == ITEMGET(13, 89) || Obj.pChaosBox[n].m_Type == ITEMGET(13, 90) || Obj.pChaosBox[n].m_Type == ITEMGET(13, 91) ||
			Obj.pChaosBox[n].m_Type == ITEMGET(13, 92) || Obj.pChaosBox[n].m_Type == ITEMGET(14, 53) || Obj.pChaosBox[n].m_Type == ITEMGET(14, 103) ||
			Obj.pChaosBox[n].m_Type == ITEMGET(14, 103) || Obj.pChaosBox[n].m_Type == ITEMGET(14, 104) || Obj.pChaosBox[n].m_Type == ITEMGET(14, 105) ||
			Obj.pChaosBox[n].m_Type == ITEMGET(14, 106) || Obj.pChaosBox[n].m_Type == ITEMGET(14, 107) || Obj.pChaosBox[n].m_Type == ITEMGET(14, 108))
		{
			Obj.pChaosBox[n].Clear();
		}
		else
		{
			if (Obj.pChaosBox[n].IsItem() == FALSE)
				continue;

			int op = Obj.pChaosBox[n].m_Option1;

			if (op > 0)
			{
				if ((rand() % 2) == 0)
				{
					op--;
				}
			}

			Obj.pChaosBox[n].m_Option1 = op;
			op = Obj.pChaosBox[n].m_Option3;

			if (op > 0)
			{
				if ((rand() % 2) == 0)
				{
					op--;
				}
			}

			Obj.pChaosBox[n].m_Option3 = op;

			if (Obj.pChaosBox[n].m_Level > 0)
			{
				Obj.pChaosBox[n].m_Level = rand() % Obj.pChaosBox[n].m_Level;
			}

			float dur = ItemGetDurability(Obj.pChaosBox[n].m_Type, Obj.pChaosBox[n].m_Level, Obj.pChaosBox[n].IsExtItem(), Obj.pChaosBox[n].IsSetItem());

			Obj.pChaosBox[n].m_Durability = dur * Obj.pChaosBox[n].m_Durability / Obj.pChaosBox[n].m_BaseDurability;
			Obj.pChaosBox[n].Convert(Obj.pChaosBox[n].m_Type, Obj.pChaosBox[n].m_Option1,
				Obj.pChaosBox[n].m_Option2, Obj.pChaosBox[n].m_Option3, Obj.pChaosBox[n].m_NewOption,
				Obj.pChaosBox[n].m_SetOption, Obj.pChaosBox[n].m_ItemOptionEx, 0, -1, 0, CURRENT_DB_VERSION);
		}
	}

	return TRUE;
}

int CMixSystem::ChaosBoxMix(CGameObject &Obj, int & Result2, int & WingNum)
{
	BYTE ExOption[8];
	int ChaosDiamond = 0;
	int ChaosItems = 0;

	if (Obj.pChaosBox == NULL)
	{
		return 0;
	}

	int value = 0;
	int add = 0;
	int nv = 0;
	Result2 = 0;
	int WingCharmItemNum = -1; //season4.5 add-on
	Obj.ChaosSuccessRate = 0;
	Obj.ChaosMoney = 0;
	int iCharmOfLuckCount = 0;
	int iCharmOfWing = 0;
	WingNum = -1;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			Obj.pChaosBox[n].OldValue();
			add = 0;
			nv = 0;

			if (g_LuckyItemManager.IsLuckyItemEquipment(Obj.pChaosBox[n].m_Type))	//1.01.00
			{
				return false;
			}

			if (Obj.pChaosBox[n].m_Level >= MIN_CHAOS_ITEM_LEVEL && (Obj.pChaosBox[n].m_Option3 * 4) >= MIN_CHAOS_ITEM_LEVEL)
			{
				nv = Obj.pChaosBox[n].m_OldBuyMoney;
				value += Obj.pChaosBox[n].m_OldBuyMoney;
				add = 1;

				if (Obj.pChaosBox[n].m_Type == ITEMGET(2, 6) || Obj.pChaosBox[n].m_Type == ITEMGET(4, 6) || Obj.pChaosBox[n].m_Type == ITEMGET(5, 7))	// Chaos Items
				{
					Result2 = 1;
				}
			}

			if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 15))
			{
				value += Obj.pChaosBox[n].m_OldBuyMoney;
				nv = Obj.pChaosBox[n].m_OldBuyMoney;
				ChaosDiamond++;
				add = 1;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 13) || Obj.pChaosBox[n].m_Type == ITEMGET(14, 14))	// Jewel of Bless, Jewel of Soul
			{
				value += Obj.pChaosBox[n].m_OldBuyMoney;
				nv = Obj.pChaosBox[n].m_OldBuyMoney;
				add = 1;
			}

			//season4.5 add-on
			if (Obj.pChaosBox[n].m_Type == ITEMGET(13, 83) ||
				Obj.pChaosBox[n].m_Type == ITEMGET(13, 84) ||
				Obj.pChaosBox[n].m_Type == ITEMGET(13, 85) ||
				Obj.pChaosBox[n].m_Type == ITEMGET(13, 86) ||
				Obj.pChaosBox[n].m_Type == ITEMGET(13, 87))
			{
				WingCharmItemNum = Obj.pChaosBox[n].m_Type;
			}

			if (Obj.pChaosBox[n].m_Type == ITEMGET(13, 14)) //updated 1.01.00
			{
				return false;
			}

			if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 53))	// Charm Of Luck
			{
				iCharmOfLuckCount += Obj.pChaosBox[n].m_Durability;
			}

			if (add != 0)
			{
				ChaosItems++;
			}

			ItemIsBufExOption(ExOption, &Obj.pChaosBox[n]);
		}
	}

	if (ChaosDiamond == 0)
	{
		value = 0;	// 0% of success
	}

	if (ChaosItems < 2)
	{
		value = 0;// 0% of success
	}

	if (iCharmOfLuckCount > 10)
		return FALSE;

	Obj.ChaosSuccessRate = value / 20000;
	Obj.ChaosSuccessRate += iCharmOfLuckCount;

	if (Obj.ChaosSuccessRate > 100)
	{
		Obj.ChaosSuccessRate = 100;
	}

	Obj.ChaosMoney = Obj.ChaosSuccessRate * 10000;	// Required Money to MIX anc createe a Chaos Item

	if (WingCharmItemNum > -1) //season4.5 add-on
	{
		int WingCharmIndex = ITEM_GET_TYPE(WingCharmItemNum);

		switch (WingCharmIndex)
		{
		case 83:
			WingNum = 2;
			break;
		case 84:
			WingNum = 1;
			break;
		case 85:
			WingNum = 0;
			break;
		case 86:
			WingNum = 41;
			break;
		case 87:
			WingNum = 30;
			break;
		default:
			break;
		}
	}

	return value;
}

void CMixSystem::DefaultChaosMix(CGameObject &Obj)
{
	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;
	CGameObject &Obj = Obj.m_Index;
	BOOL fail = TRUE;
	int MixResult2;
	int WingNum;
	Obj.ChaosLock = TRUE;
	int WingCharmIndex = -1;

	if (this->ChaosBoxMix(lpObj, MixResult2, WingCharmIndex) == 0)
	{
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	int iChaosTaxMoney = (int)((__int64)Obj.ChaosMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	Obj.ChaosMoney += iChaosTaxMoney;

	if (Obj.ChaosMoney < 0)
	{
		Obj.ChaosMoney = 0;
	}

	if (Obj.m_PlayerData->Money < Obj.ChaosMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		IOCP.DataSend(Obj.m_Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
	}
	else
	{
		if (Obj.ChaosSuccessRate > 0)
		{
			if (Obj.ChaosSuccessRate >= 100)
			{
				Obj.ChaosSuccessRate = 100;
			}

			if ((rand() % 100) <= (Obj.ChaosSuccessRate - 1))
			{
				fail = FALSE;
				pMsg.Result = CB_SUCCESS;

				int Level = rand() % 5;
				int Option1 = 0;
				int Option2 = 0;
				int Option3 = 0;

				if ((rand() % 100) < (Obj.ChaosSuccessRate / 5 + 6))
				{
					Option1 = 1;
				}

				if ((rand() % 100) < (Obj.ChaosSuccessRate / 5 + 4))
				{
					Option2 = 1;
				}

				int OpProb;
				int OpType = rand() % 3;
				OpProb = rand() % 100;

				switch (OpType)
				{
				case 0:
					if (OpProb < (Obj.ChaosSuccessRate / 5 + 4))
					{
						Option3 = 3;
					}
					break;

				case 1:
					if (OpProb < (Obj.ChaosSuccessRate / 5 + 8))
					{
						Option3 = 2;
					}
					break;

				case 2:
					if (OpProb < (Obj.ChaosSuccessRate / 5 + 12))
					{
						Option3 = 1;
					}
					break;
				}

				if (MixResult2 == TRUE)
				{
					int WingType = rand() % 4;
					int WingNum = 0;

					if (WingCharmIndex > -1) //season4.5 add-on
					{
						WingNum = WingCharmIndex + ITEMGET(12, 0);
					}
					else if (WingType == 0)
					{
						WingNum = ITEMGET(12, 0);
					}
					else if (WingType == 1)
					{
						WingNum = ITEMGET(12, 1);
					}
					else if (WingType == 2)
					{
						WingNum = ITEMGET(12, 2);
					}
					else if (WingType == 3) //season3 add-on
					{
						WingNum = ITEMGET(12, 41);
					}

					::ItemCreate(Obj.m_Index, -1, 0, 0, WingNum, 0, 255, Option1, Option2, Option3, -1, 0, 0, 0, 0, 0);
				}
				else
				{
					int ChaosItemType = rand() % 3;
					int ChaosItemNum = 0;

					if (ChaosItemType == 0)	// Chaos Dragon Axe
					{
						ChaosItemNum = ITEMGET(2, 6);
					}
					else if (ChaosItemType == 1)	// Chaos Nature Bow
					{
						ChaosItemNum = ITEMGET(4, 6);
					}
					else if (ChaosItemType == 2)	// Chaos Lighting Staff
					{
						ChaosItemNum = ITEMGET(5, 7);
					}

					::ItemCreate(Obj.m_Index, -1, 0, 0, ChaosItemNum, Level, 255, Option1, Option2, Option3, -1, 0, 0, 0, 0, 0);
				}
			}
		}

		Obj.m_PlayerData->Money -= Obj.ChaosMoney;
		g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
		gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

		if (fail == TRUE)
		{
			ChaosBoxItemDown(lpObj);
			gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
			IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
			sLog->outBasic("[%s][%s] CBMix Fail %d Money : %d-%d",
				Obj.AccountID, Obj.Name, Obj.ChaosSuccessRate,
				Obj.m_PlayerData->Money, Obj.ChaosMoney);
			Obj.ChaosLock = FALSE;
		}
		else
		{
			sLog->outBasic("[%s][%s] CBMix Success Rate:%d Money : %d",
				Obj.AccountID, Obj.Name, Obj.ChaosSuccessRate,
				Obj.ChaosMoney);
		}
	}

	::gObjInventoryCommit(Obj.m_Index);
}

int CMixSystem::CheckDevilSquareItem(CGameObject &Obj, int & eventitemcount, int & itemlevel)
{
	BOOL bChaoseGem = FALSE;
	BOOL bEyeOfDevil = FALSE;
	BOOL bKeyOfDevil = FALSE;
	eventitemcount = 0;
	BOOL FoundOtherItem = FALSE;
	int level = -1;
	BOOL bLevelCheck = FALSE;
	int iCharmOfLuckCount = 0;
	BOOL bCharmOfLuckOver = FALSE;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 15))	// Chaos
			{
				bChaoseGem = TRUE;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 17))	// Eye
			{
				eventitemcount += 1;
				bEyeOfDevil = TRUE;

				if (level != Obj.pChaosBox[n].m_Level)
				{
					if (level == -1)
					{
						level = Obj.pChaosBox[n].m_Level;
					}
					else
					{
						bLevelCheck = TRUE;
					}
				}
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 18))	// Key
			{
				eventitemcount += 1;
				bKeyOfDevil = TRUE;

				if (level != Obj.pChaosBox[n].m_Level)
				{
					if (level == -1)
					{
						level = Obj.pChaosBox[n].m_Level;
					}
					else
					{
						bLevelCheck = TRUE;
					}
				}
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 53))	// Charm Of Luck
			{
				iCharmOfLuckCount += Obj.pChaosBox[n].m_Durability;

				if (iCharmOfLuckCount > 10)
				{
					bCharmOfLuckOver = TRUE;
				}
			}
			else
			{
				FoundOtherItem = TRUE;
			}
		}
	}

	itemlevel = level;
	Obj.ChaosSuccessRate = iCharmOfLuckCount;

	if (FoundOtherItem != FALSE)
	{
		return 0;
	}

	if (bLevelCheck != FALSE)
	{
		return 3;
	}

	if (bCharmOfLuckOver == TRUE)
	{
		return 4;
	}

	if (bChaoseGem != FALSE && bEyeOfDevil != FALSE && bKeyOfDevil != FALSE)
	{
		LogDQChaosItem(lpObj);
		return 1;
	}

	if (bEyeOfDevil != FALSE && bKeyOfDevil != FALSE)
	{
		LogDQChaosItem(lpObj);
		return 2;
	}

	return 0;
}

void CMixSystem::LogDQChaosItem(CGameObject &Obj)
{
	BYTE ExOption[MAX_EXOPTION_SIZE];

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			::ItemIsBufExOption(ExOption, &Obj.pChaosBox[n]);

		}
	}
}




BOOL CMixSystem::DevilSquareEventChaosMix(CGameObject &Obj, BOOL bCheckType, int iItemLevel)
{
	BOOL fail = TRUE;
	CGameObject &Obj = Obj.m_Index;
	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;
	Obj.ChaosLock = TRUE;

	INT nChaosNeedMoney = 0;
	int iCharmOfLuckCount = 0;
	iCharmOfLuckCount = Obj.ChaosSuccessRate;

	if (iCharmOfLuckCount > 10)
	{
		pMsg.Result = 0xF0;
		Obj.ChaosLock = FALSE;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	switch (iItemLevel)
	{
	case 0:
		Obj.ChaosSuccessRate = 60;
		nChaosNeedMoney = 100000;
		break;

	case 1:
		Obj.ChaosSuccessRate = gDQChaosSuccessRateLevel1;
		nChaosNeedMoney = 100000;
		break;

	case 2:
		Obj.ChaosSuccessRate = gDQChaosSuccessRateLevel2;
		nChaosNeedMoney = 200000;
		break;

	case 3:
		Obj.ChaosSuccessRate = gDQChaosSuccessRateLevel3;
		nChaosNeedMoney = 400000;
		break;

	case 4:
		Obj.ChaosSuccessRate = gDQChaosSuccessRateLevel4;
		nChaosNeedMoney = 700000;
		break;

	case 5:
		Obj.ChaosSuccessRate = gDQChaosSuccessRateLevel5;
		nChaosNeedMoney = 1100000;
		break;

	case 6:
		Obj.ChaosSuccessRate = gDQChaosSuccessRateLevel6;
		nChaosNeedMoney = 1600000;
		break;

	case 7:
		Obj.ChaosSuccessRate = gDQChaosSuccessRateLevel7;
		nChaosNeedMoney = 2000000;
		break;

	default:

		pMsg.Result = CB_INVALID_ITEM_LEVEL;
		IOCP.DataSend(Obj.m_Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return 1;
		break;
	}

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if (nChaosNeedMoney < 0)
	{
		nChaosNeedMoney = 0;
	}

	if ((Obj.m_PlayerData->Money - nChaosNeedMoney) < 0)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		IOCP.DataSend(Obj.m_Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return 1;

	}

	if (g_CrywolfSync.GetOccupationState() == 0 && g_CrywolfSync.GetApplyBenefit() == TRUE)
	{
		Obj.ChaosSuccessRate += g_CrywolfSync.GetPlusChaosRate();
	}

	Obj.ChaosSuccessRate += iCharmOfLuckCount;

	int iRate = rand() % 100;

	if (bCheckType == TRUE)
	{
		if (iRate < Obj.ChaosSuccessRate)
		{
			int DevilInv = ITEMGET(14, 19);	// Devil Ticket
			ItemCreate(Obj.m_Index, 0xFF, 0, 0, DevilInv, iItemLevel, 0, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
			fail = FALSE;
		}
	}

	Obj.m_PlayerData->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	if (fail == TRUE)
	{
		ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_Index, (BYTE *)&pMsg, pMsg.h.size);

		Obj.ChaosLock = FALSE;
	}
	else
	{

	}

	::gObjInventoryCommit(Obj.m_Index);

	return TRUE;
}

void CMixSystem::LogChaosItem(CGameObject &Obj, LPSTR sLogType)
{
	BYTE ExOption[MAX_EXOPTION_SIZE];

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			::ItemIsBufExOption(ExOption, &Obj.pChaosBox[n]);

		}
	}
}

BOOL CMixSystem::PlusItemLevelChaosMix(CGameObject &Obj, int mixType)
{
	int ChaosGemCount = 0;
	int BlessGemCount = 0;
	int SoulGemCount = 0;
	int Plus9ItemCount = 0;
	int Plus10ItemCount = 0;
	int PlusItemPos = -1;
	int OtherItemFound = 0;
	int Plus11ItemCount = 0;
	int Plus12ItemCount = 0;
	int Plus13ItemCount = 0;
	int Plus14ItemCount = 0;
	int ExtraBlessGemCount = 0;
	int ExtraSoulGemCount = 0;
	int iCharmOfLuckCount = 0;
	int iElementalCharmOfLuckCount = 0;
	int iCharmOfAssembly = 0;
	int iElementalCharmOfAssembly = 0;
	BYTE btItemLuckValue = 0;

	for (int n = 0; n<CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 15))
			{
				ChaosGemCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 13))
			{
				BlessGemCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 14))
			{
				SoulGemCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 53))	// Charm Of Luck
			{
				iCharmOfLuckCount += Obj.pChaosBox[n].m_Durability;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 96))
			{
				iCharmOfAssembly++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 189)) // Elemental Charm of Luck
			{
				iElementalCharmOfLuckCount += 10; // Always 10%
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 190))
			{
				iElementalCharmOfAssembly++;
			}
			else if (Obj.pChaosBox[n].m_Level == 9)
			{
				Plus9ItemCount++;
				PlusItemPos = n;
			}
			else if (Obj.pChaosBox[n].m_Level == 10)
			{
				Plus10ItemCount++;
				PlusItemPos = n;
			}
			else if (Obj.pChaosBox[n].m_Level == 11)
			{
				Plus11ItemCount++;
				PlusItemPos = n;
			}
			else if (Obj.pChaosBox[n].m_Level == 12)
			{
				Plus12ItemCount++;
				PlusItemPos = n;
			}
			else if (Obj.pChaosBox[n].m_Level == 13)
			{
				Plus13ItemCount++;
				PlusItemPos = n;
			}
			else if (Obj.pChaosBox[n].m_Level == 14)
			{
				Plus14ItemCount++;
				PlusItemPos = n;
			}
			else
			{
				OtherItemFound++;
			}
		}
	}

	if (iCharmOfLuckCount > 10)
	{
		PMSG_CHAOSMIXRESULT pResult;

		PHeadSetB((BYTE*)&pResult, 0x86, sizeof(pResult));

		sLog->outBasic("[PlusItemLevel] [%s][%s] CBMix Charm of luck over 10 (%d)",
			Obj.AccountID, Obj.Name, iCharmOfLuckCount);

		pResult.Result = 0xF0;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		Obj.ChaosLock = FALSE;

		return 0;
	}

	if (iElementalCharmOfLuckCount > 10)
	{
		PMSG_CHAOSMIXRESULT pResult;

		PHeadSetB((BYTE*)&pResult, 0x86, sizeof(pResult));

		sLog->outBasic("[PlusItemLevel] [%s][%s] CBMix Charm of Luck over 10 (%d)",
			Obj.AccountID, Obj.Name, iCharmOfLuckCount);

		pResult.Result = 0xF0;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

		Obj.ChaosLock = FALSE;

		return 0;
	}

	if (g_PentagramSystem.IsPentagramItem(Obj.pChaosBox[PlusItemPos].m_Type) == true)
	{
		if (iCharmOfLuckCount != 0 || iCharmOfAssembly != 0)
		{
			PMSG_CHAOSMIXRESULT pResult;

			PHeadSetB((BYTE*)&pResult, 0x86, sizeof(pResult));

			sLog->outBasic("[PlusItemLevel] [%s][%s] CBMix Wrong Charm of luck/Assembly",
				Obj.AccountID, Obj.Name, iCharmOfLuckCount);

			pResult.Result = 0xF0;

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

			Obj.ChaosLock = FALSE;

			return 0;
		}
	}

	else
	{
		if (iElementalCharmOfLuckCount != 0 || iElementalCharmOfAssembly != 0)
		{
			PMSG_CHAOSMIXRESULT pResult;

			PHeadSetB((BYTE*)&pResult, 0x86, sizeof(pResult));

			sLog->outBasic("[PlusItemLevel] [%s][%s] CBMix Wrong Charm of luck/Assembly",
				Obj.AccountID, Obj.Name, iCharmOfLuckCount);

			pResult.Result = 0xF0;

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pResult, pResult.h.size);

			Obj.ChaosLock = FALSE;

			return 0;
		}
	}

	if (OtherItemFound != FALSE)
	{
		return FALSE;
	}

	Obj.ChaosMoney = 0;
	int MixType = 0;

	if (Plus9ItemCount == 1 && ChaosGemCount == 1 && BlessGemCount == 1 && SoulGemCount == 1 && Plus10ItemCount == 0 && Plus11ItemCount == 0 && Plus12ItemCount == 0)
	{
		MixType = 3;
		Obj.ChaosMoney = 2000000;
		ExtraBlessGemCount = BlessGemCount - 1;
		ExtraSoulGemCount = SoulGemCount - 1;
	}
	else if (Plus10ItemCount == 1 && ChaosGemCount == 1 && BlessGemCount == 2 && SoulGemCount == 2 && Plus9ItemCount == 0 && Plus11ItemCount == 0 && Plus12ItemCount == 0)
	{
		MixType = 4;
		Obj.ChaosMoney = 4000000;
		ExtraBlessGemCount = BlessGemCount - 2;
		ExtraSoulGemCount = SoulGemCount - 2;
	}
	else if (Plus11ItemCount == 1 && ChaosGemCount == 1 && BlessGemCount == 3 && SoulGemCount == 3 && Plus9ItemCount == 0 && Plus10ItemCount == 0 && Plus12ItemCount == 0)
	{
		MixType = 22;
		Obj.ChaosMoney = 6000000;
		ExtraBlessGemCount = BlessGemCount - 3;
		ExtraSoulGemCount = SoulGemCount - 3;
	}
	else if (Plus12ItemCount == 1 && ChaosGemCount == 1 && BlessGemCount == 4 && SoulGemCount == 4 && Plus9ItemCount == 0 && Plus10ItemCount == 0 && Plus11ItemCount == 0)
	{
		MixType = 23;
		Obj.ChaosMoney = 8000000;
		ExtraBlessGemCount = BlessGemCount - 4;
		ExtraSoulGemCount = SoulGemCount - 4;
	}

	else if (Plus13ItemCount == 1 && ChaosGemCount == 1 && BlessGemCount == 5 && SoulGemCount == 5 && Plus9ItemCount == 0 && Plus10ItemCount == 0 && Plus11ItemCount == 0)
	{
		MixType = 49;
		Obj.ChaosMoney = 10000000;
		ExtraBlessGemCount = BlessGemCount - 5;
		ExtraSoulGemCount = SoulGemCount - 5;
	}

	else if (Plus14ItemCount == 1 && ChaosGemCount == 1 && BlessGemCount == 6 && SoulGemCount == 6 && Plus9ItemCount == 0 && Plus10ItemCount == 0 && Plus11ItemCount == 0)
	{
		MixType = 50;
		Obj.ChaosMoney = 12000000;
		ExtraBlessGemCount = BlessGemCount - 6;
		ExtraSoulGemCount = SoulGemCount - 6;
	}

	if (MixType != mixType)
	{
		MixType = 0;
	}

	if (MixType == 0)
	{
		return FALSE;
	}

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;
	Obj.ChaosLock = TRUE;

	sLog->outBasic("[PlusItemLevel] Chaos Mix Start");

	int iChaosTaxMoney = (int)((__int64)Obj.ChaosMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	Obj.ChaosMoney += iChaosTaxMoney;

	if (Obj.ChaosMoney < 0)
	{
		Obj.ChaosMoney = 0;
	}

	if (Obj.m_PlayerData->Money < Obj.ChaosMoney)
	{
		sLog->outBasic("[PlusItemLevel] [%s][%s] CBMix Not Enough Money [%d] need zen [%d]",
			Obj.AccountID, Obj.Name, Obj.m_PlayerData->Money, Obj.ChaosMoney);
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return TRUE;
	}

	Obj.m_PlayerData->Money -= Obj.ChaosMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	if (Obj.m_PlayerData->VipType != 0)
	{
		Obj.ChaosSuccessRate = g_VipSystem.GetPlusItemMixRate(lpObj, mixType);

		if (Obj.pChaosBox[PlusItemPos].m_Option2 != FALSE)	// if Have Luck
		{
			Obj.ChaosSuccessRate += g_VipSystem.GetPlusItemAddLuckRate(lpObj);
		}
	}

	else
	{
		if (Obj.pChaosBox[PlusItemPos].m_Level == 9)
		{
			Obj.ChaosSuccessRate = g_ConfigRead.mix.PlusItemLevel10Rate;
		}
		else if (Obj.pChaosBox[PlusItemPos].m_Level == 10)
		{
			Obj.ChaosSuccessRate = g_ConfigRead.mix.PlusItemLevel11Rate;
		}
		else if (Obj.pChaosBox[PlusItemPos].m_Level == 11)
		{
			Obj.ChaosSuccessRate = g_ConfigRead.mix.PlusItemLevel12Rate;
		}
		else if (Obj.pChaosBox[PlusItemPos].m_Level == 12)
		{
			Obj.ChaosSuccessRate = g_ConfigRead.mix.PlusItemLevel12Rate;
		}
		else if (Obj.pChaosBox[PlusItemPos].m_Level == 13)
		{
			Obj.ChaosSuccessRate = g_ConfigRead.mix.PlusItemLevel12Rate;
		}
		else if (Obj.pChaosBox[PlusItemPos].m_Level == 14)
		{
			Obj.ChaosSuccessRate = g_ConfigRead.mix.PlusItemLevel12Rate;
		}

		if (Obj.pChaosBox[PlusItemPos].m_Option2 != FALSE)	// if Have Luck
		{
			Obj.ChaosSuccessRate += g_ConfigRead.mix.PlusItemAddLuckRate;
		}
	}

	Obj.ChaosSuccessRate += iCharmOfLuckCount;
	Obj.ChaosSuccessRate += iElementalCharmOfLuckCount;

	if (Obj.ChaosSuccessRate > 100)
	{
		Obj.ChaosSuccessRate = 100;
	}

	btItemLuckValue = Obj.pChaosBox[PlusItemPos].m_Option2;

	int Result = rand() % 100;

	if (Result < Obj.ChaosSuccessRate)
	{
		Obj.pChaosBox[PlusItemPos].m_Level++;
		pMsg.Result = CB_SUCCESS;

		CItemObject Item;

		Item.Convert(Obj.pChaosBox[PlusItemPos].m_Type, Obj.pChaosBox[PlusItemPos].m_Option1, Obj.pChaosBox[PlusItemPos].m_Option2, Obj.pChaosBox[PlusItemPos].m_Option3,
			Obj.pChaosBox[PlusItemPos].m_NewOption, Obj.pChaosBox[PlusItemPos].m_SetOption, Obj.pChaosBox[PlusItemPos].m_ItemOptionEx, Obj.pChaosBox[PlusItemPos].m_SocketOption
		, Obj.pChaosBox[PlusItemPos].m_BonusSocketOption, 0, 3);

		float Dur = ItemGetDurability(Obj.pChaosBox[PlusItemPos].m_Type, Obj.pChaosBox[PlusItemPos].m_Level,
			Obj.pChaosBox[PlusItemPos].IsExtItem(), Obj.pChaosBox[PlusItemPos].IsSetItem());

		Item.m_Type = Obj.pChaosBox[PlusItemPos].m_Type;
		Item.m_Level = Obj.pChaosBox[PlusItemPos].m_Level;
		Item.m_Durability = Dur * Obj.pChaosBox[PlusItemPos].m_Durability / Obj.pChaosBox[PlusItemPos].m_BaseDurability;
		Item.m_Option1 = Obj.pChaosBox[PlusItemPos].m_Option1;
		Item.m_Option2 = Obj.pChaosBox[PlusItemPos].m_Option2;
		Item.m_Option3 = Obj.pChaosBox[PlusItemPos].m_Option3;
		Item.m_NewOption = Obj.pChaosBox[PlusItemPos].m_NewOption;
		Item.m_ItemOptionEx = Obj.pChaosBox[PlusItemPos].m_ItemOptionEx;
		Item.m_JewelOfHarmonyOption = Obj.pChaosBox[PlusItemPos].m_JewelOfHarmonyOption;
		Item.m_SocketOption[0] = Obj.pChaosBox[PlusItemPos].m_SocketOption[0];
		Item.m_SocketOption[1] = Obj.pChaosBox[PlusItemPos].m_SocketOption[1];
		Item.m_SocketOption[2] = Obj.pChaosBox[PlusItemPos].m_SocketOption[2];
		Item.m_SocketOption[3] = Obj.pChaosBox[PlusItemPos].m_SocketOption[3];
		Item.m_SocketOption[4] = Obj.pChaosBox[PlusItemPos].m_SocketOption[4];
		Item.m_BonusSocketOption = Obj.pChaosBox[PlusItemPos].m_BonusSocketOption;

		ItemByteConvert(pMsg.ItemInfo, Item);
		ChaosBoxInit(lpObj);
		::gObjChaosBoxInsertItemPos(Obj.m_Index, Item, 0, -1);
		gObjChaosItemSet(Obj.m_Index, 0, 1);
		pMsg.Pos = 0; //[K2]
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);

		if (g_ConfigRead.mix.PlusItemMixResultNotice != 0 && MixType == 50)
		{
			gGameProtocol.GCServerMsgStringSendAllEx(0, Lang.GetText(0, 611), Obj.Name, ItemAttribute[Item.m_Type].Name);
		}
	}
	else if (iCharmOfAssembly == 0 && iElementalCharmOfAssembly == 0)
	{
		if (g_PentagramSystem.IsPentagramItem(&Obj.pChaosBox[PlusItemPos]) == true)
		{
			g_PentagramSystem.DelPentagramJewelInfo(Obj.m_Index, &Obj.pChaosBox[PlusItemPos]);
		}

		int FailLevel = Obj.pChaosBox[PlusItemPos].m_Level + 1;
		ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
	}
	else
	{
		int FailLevel = Obj.pChaosBox[PlusItemPos].m_Level + 1;
		this->ChaosMixCharmItemUsed(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
	return TRUE;
}
// TEST  SRC  FUNC  EMU S8
void CMixSystem::PlusItemLevelMixTest(CGameObject &Obj, int type) // OK
{
	int ChaosCount = 0;
	int BlessCount = 0;
	int SoulCount = 0;
	int ItemCount = 0;
	int ItemSlot = 0;
	int ChaosAmulet = 0;
	int ElementalChaosAmulet = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 15))
		{
			ChaosCount++;
		}
		else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 13))
		{
			BlessCount++;
		}
		else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 14))
		{
			SoulCount++;
		}
		else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 96))
		{
			ChaosAmulet++;
		}
		else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 190))
		{
			ElementalChaosAmulet++;
		}
		else if (Obj.pChaosBox[n].m_Level == (9 + type))
		{
			ItemCount++;
			ItemSlot = n;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (ChaosCount != 1 || SoulCount < (type + 1) || BlessCount < (type + 1) || ItemCount != 1)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (Obj.m_PlayerData->VipType != 0)
	{
		Obj.ChaosSuccessRate = g_VipSystem.GetPlusItemMixRate(lpObj, type);

		if (Obj.pChaosBox[ItemSlot].m_Option2 != FALSE)	// if Have Luck
		{
			Obj.ChaosSuccessRate += g_VipSystem.GetPlusItemAddLuckRate(lpObj);
		}
	}
	else
	{
		switch (type)
		{
		case 0:
			Obj.ChaosSuccessRate = g_ConfigRead.mix.PlusItemLevel10Rate;
			break;
		case 1:
			Obj.ChaosSuccessRate = g_ConfigRead.mix.PlusItemLevel11Rate;
			break;
		case 2:
			Obj.ChaosSuccessRate = g_ConfigRead.mix.PlusItemLevel12Rate;
			break;
		case 3:
			Obj.ChaosSuccessRate = g_ConfigRead.mix.PlusItemLevel13Rate;
			break;
		case 4:
			Obj.ChaosSuccessRate = g_ConfigRead.mix.PlusItemLevel14Rate;
			break;
		case 5:
			Obj.ChaosSuccessRate = g_ConfigRead.mix.PlusItemLevel15Rate;
			break;
		}

		if (Obj.pChaosBox[ItemSlot].m_Option2 != FALSE)	// if Have Luck
		{
			Obj.ChaosSuccessRate += g_ConfigRead.mix.PlusItemAddLuckRate;
		}
	}

	if (Obj.ChaosSuccessRate > 100)
	{
		Obj.ChaosSuccessRate = 100;
	}

	int TaxMoney = (Obj.ChaosMoney*g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index)) / 100;

	Obj.ChaosMoney += TaxMoney;

	if (Obj.m_PlayerData->Money < ((DWORD)Obj.ChaosMoney))
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	Obj.m_PlayerData->Money -= Obj.ChaosMoney;

	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	g_CastleSiegeSync.AddTributeMoney(TaxMoney);

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		pMsg.Result = CB_SUCCESS;

		CItemObject &item = Obj.pChaosBox[ItemSlot];

		item.m_Level++;

		item.m_Durability = (float)ItemGetDurability(item.m_Type, item.m_Level, item.IsExtItem(), item.IsSetItem());

		item.m_Durability = (item.m_Durability*Obj.pChaosBox[ItemSlot].m_Durability) / item.m_BaseDurability;

		this->ChaosBoxInit(lpObj);

		BYTE SocketOption[5];
		BYTE SocketIndex;
		g_SocketOptionSystem.GetSocketOption(&Obj.pChaosBox[ItemSlot], SocketOption, SocketIndex);

		if (g_PentagramSystem.IsPentagramItem(&Obj.pChaosBox[ItemSlot]))
		{
			for (int i = 0; i < 5; i++)
			{
				SocketOption[i] = Obj.pChaosBox[ItemSlot].m_SocketOption[i];
			}

			SocketIndex = Obj.pChaosBox[ItemSlot].m_BonusSocketOption;
		}

		::gObjChaosBoxInsertItemPos(Obj.m_Index, item, 0, -1);

		gObjChaosItemSet(Obj.m_Index, 0, 1);

		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);

		if (g_ConfigRead.mix.PlusItemMixResultNotice != 0 && type == 5)
		{
			gGameProtocol.GCServerMsgStringSendAllEx(0, Lang.GetText(0, 611), Obj.Name, ItemAttribute[item.m_Type].Name);
		}

	}
	else
	{
		pMsg.Result = CB_ERROR;

		if (ChaosAmulet == 0 && ElementalChaosAmulet == 0)
		{
			if (g_PentagramSystem.IsPentagramItem(&Obj.pChaosBox[ItemSlot]) == true)
			{
				g_PentagramSystem.DelPentagramJewelInfo(Obj.m_Index, &Obj.pChaosBox[ItemSlot]);
			}

			int FailLevel = Obj.pChaosBox[ItemSlot].m_Level + 1;

			ChaosBoxInit(lpObj);

			gGameProtocol.GCUserChaosBoxSend(lpObj, 0);

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);

		}
		else
		{
			this->ChaosBoxItemDown(lpObj);

			this->ChaosBoxInit(lpObj);

		}
	}
}

void CMixSystem::ChaosMixCharmItemUsed(CGameObject &Obj)
{
	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			Obj.pChaosBox[n].m_Level = 0;

			int iItemCode = Obj.pChaosBox[n].m_Type;

			if (this->IsDeleteItem(iItemCode) == TRUE)
			{
				Obj.pChaosBox[n].Clear();
				Obj.pChaosBoxMap[n] = -1;
			}
		}
	}
}

BOOL CMixSystem::IsDeleteItem(int iItemCode)
{
	switch (iItemCode)
	{
	case ITEMGET(14, 53):
	case ITEMGET(12, 15):
	case ITEMGET(14, 13):
	case ITEMGET(14, 14):
	case ITEMGET(14, 96):
	case ITEMGET(14, 189):
	case ITEMGET(14, 190):
		return TRUE;
	}

	return FALSE;
}

BOOL CMixSystem::PegasiaChaosMix(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;
	int UniriaCount = 0;
	int ChoasGemCount = 0;
	int iCharmOfLuckCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(13, 2) && Obj.pChaosBox[n].m_Durability == 255.0f)	// Uniria
			{
				UniriaCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 15)) // Chaos Gem
			{
				ChoasGemCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 53))	// Charm Of Luck
			{
				iCharmOfLuckCount += Obj.pChaosBox[n].m_Durability;
			}
		}
	}

	if (UniriaCount != 10 || ChoasGemCount != 1)
	{
		Obj.ChaosLock = FALSE;
		return FALSE;
	}

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	if (iCharmOfLuckCount > 10)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = 0xF0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

		return 0;
	}

	int nChaosNeedMoney = 500000;
	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if (nChaosNeedMoney < 0)
	{
		nChaosNeedMoney = 0;
	}

	if (Obj.m_PlayerData->Money < nChaosNeedMoney)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_NOT_ENOUGH_ZEN;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return TRUE;
	}

	Obj.ChaosSuccessRate = 70;	// Succes Rate for Dinorant
	Obj.ChaosSuccessRate += iCharmOfLuckCount;
	Obj.m_PlayerData->Money -= nChaosNeedMoney;

	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	if ((rand() % 100) < Obj.ChaosSuccessRate)
	{
		int Dinorant = ITEMGET(13, 3);
		int Option3 = 0;

		if ((rand() % 100) < 30)
		{
			Option3 = 1 << ((rand() % 3));

			if ((rand() % 5) == 0)
			{
				Option3 |= 1 << (rand() % 3);
			}
		}

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, Dinorant, 0, 255, 1, 0, Option3, Obj.m_Index, 0, 0, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

		return TRUE;
	}
	else
	{
		ChaosBoxInit(lpObj);	// Errase Chaos Box
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);

		Obj.ChaosLock = FALSE;
		return FALSE;
	}
}

BOOL CMixSystem::CircleChaosMix(CGameObject &Obj)	// Fruits
{
	Obj.ChaosLock = TRUE;

	int CreatureGemCount = 0;
	int ChoasGemCount = 0;
	int iCharmOfLuckCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 22))	// Jewel of Creation
			{
				CreatureGemCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 15)) // Chaos Gem
			{
				ChoasGemCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 53))	// Charm Of Luck
			{
				iCharmOfLuckCount += Obj.pChaosBox[n].m_Durability;
			}
		}
	}

	if (CreatureGemCount != 1 || ChoasGemCount != 1)
	{
		return FALSE;
	}

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	if (iCharmOfLuckCount > 10)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = 0xF0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

		return 1;
	}

	if (Obj.Level < 10)
	{
		pMsg.Result = CB_LOW_LEVEL_USER;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return TRUE;
	}

	int nChaosNeedMoney = 3000000;
	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if (nChaosNeedMoney < 0)
	{
		nChaosNeedMoney = 0;
	}

	if (Obj.m_PlayerData->Money < nChaosNeedMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return TRUE;
	}

	Obj.ChaosSuccessRate = 90;	// Succes Rate for Fruit
	Obj.ChaosSuccessRate += iCharmOfLuckCount;
	Obj.m_PlayerData->Money -= nChaosNeedMoney;

	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	if ((rand() % 100) < Obj.ChaosSuccessRate)
	{
		int Fruit = ITEMGET(13, 15);	// Fruit
		int FruitType;
		int RandonValue = rand() % 100;

		if (RandonValue < 30)
		{
			FruitType = 0;
		}
		else if (RandonValue < 55)
		{
			FruitType = 1;
		}
		else if (RandonValue < 75)
		{
			FruitType = 2;
		}
		else if (RandonValue < 95)
		{
			FruitType = 3;
		}
		else
		{
			FruitType = 4;
		}

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, Fruit, FruitType, 255, 1, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

		return TRUE;
	}
	else
	{
		ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return FALSE;
	}
}

BOOL CMixSystem::WingChaosMix(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int WingCount = 0;
	int ChoasGemCount = 0;
	int LokesFeathersCount = 0;
	int WingIndex = -1;
	DWORD iChaosMoney = 0;
	int iWingChaosMoney = 0;
	int iSleeveOfLord = 0;
	int iCharmOfLuckCount = 0;
	int WingCharmItemNum = -1; //season4.5 add-on

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if ((Obj.pChaosBox[n].m_Type >= ITEMGET(12, 3) && Obj.pChaosBox[n].m_Type <= ITEMGET(12, 6)) || Obj.pChaosBox[n].m_Type == ITEMGET(13, 30))
			{
				Obj.ChaosLock = FALSE;

				return FALSE;
			}
			if ((Obj.pChaosBox[n].m_Type >= ITEMGET(12, 0) && Obj.pChaosBox[n].m_Type <= ITEMGET(12, 2)) || Obj.pChaosBox[n].m_Type == ITEMGET(12, 41))
			{
				WingCount++;
				WingIndex = n;
				iWingChaosMoney = Obj.pChaosBox[n].m_BuyMoney;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 15)) // Chaos
			{
				ChoasGemCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(13, 14)) // Feather
			{
				if (Obj.pChaosBox[n].m_Level == 0) // Feather
				{
					LokesFeathersCount++;
				}
				else	// Crst of Monarch
				{
					iSleeveOfLord++;
				}
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 53))	// Charm Of Luck
			{
				iCharmOfLuckCount += Obj.pChaosBox[n].m_Durability;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(13, 88) || //season4.5 add-on
				Obj.pChaosBox[n].m_Type == ITEMGET(13, 89) ||
				Obj.pChaosBox[n].m_Type == ITEMGET(13, 90) ||
				Obj.pChaosBox[n].m_Type == ITEMGET(13, 91) ||
				Obj.pChaosBox[n].m_Type == ITEMGET(13, 92))
			{
				WingCharmItemNum = Obj.pChaosBox[n].m_Type;
			}

			else if (Obj.pChaosBox[n].IsExtItem() != FALSE)
			{
				if (Obj.pChaosBox[n].m_Level >= 4)
				{
					iChaosMoney += Obj.pChaosBox[n].m_BuyMoney;
				}
			}
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;


	if (iSleeveOfLord == 1)
	{
		if (WingCount != 1 || ChoasGemCount != 1 || LokesFeathersCount != 0)
		{
			Obj.ChaosLock = FALSE;
			pMsg.Result = CB_INCORRECT_MIX_ITEMS;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
			return FALSE;
		}
	}
	else if (LokesFeathersCount == 1)
	{
		if (WingCount != 1 || ChoasGemCount != 1 || iSleeveOfLord != 0)
		{
			pMsg.Result = CB_INCORRECT_MIX_ITEMS;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
			Obj.ChaosLock = FALSE;

			return FALSE;
		}
	}
	else
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);

		return FALSE;
	}

	if (iCharmOfLuckCount > 10)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = 0xF0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

		return FALSE;
	}

	int nChaosNeedMoney = 5000000;
	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if (nChaosNeedMoney < 0)
	{
		nChaosNeedMoney = 0;
	}

	if (Obj.m_PlayerData->Money < nChaosNeedMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return TRUE;
	}

	CItemObject * pWing = &Obj.pChaosBox[WingIndex];
	Obj.ChaosSuccessRate = (DWORD)((DWORD)iWingChaosMoney / (DWORD)4000000);
	Obj.ChaosSuccessRate += iChaosMoney / 40000;

	if (Obj.ChaosSuccessRate == 0)
	{
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return FALSE;
	}

	LogChaosItem(lpObj, "WingMix,2");
	sLog->outBasic("[WingMix,2] Chaos Mix Start");

	if (Obj.m_PlayerData->VipType != 0)
	{
		if (iSleeveOfLord == 1)
		{
			if (Obj.ChaosSuccessRate > g_VipSystem.GetCapeOfLordRate(lpObj))
			{
				Obj.ChaosSuccessRate = g_VipSystem.GetCapeOfLordRate(lpObj);
			}
		}
		else if (LokesFeathersCount == 1)
		{
			if (Obj.ChaosSuccessRate > g_VipSystem.GetWing2ndRate(lpObj))
			{
				Obj.ChaosSuccessRate = g_VipSystem.GetWing2ndRate(lpObj);
			}
		}

		else if (Obj.ChaosSuccessRate > g_VipSystem.GetWing2ndRate(lpObj))
		{
			Obj.ChaosSuccessRate = g_VipSystem.GetWing2ndRate(lpObj);
		}
	}

	else
	{
		if (iSleeveOfLord == 1)
		{
			if (Obj.ChaosSuccessRate > g_ConfigRead.mix.CapeLordMaxRate)
			{
				Obj.ChaosSuccessRate = g_ConfigRead.mix.CapeLordMaxRate;
			}
		}
		else if (LokesFeathersCount == 1)
		{
			if (Obj.ChaosSuccessRate > g_ConfigRead.mix.Wing2MaxRate)
			{
				Obj.ChaosSuccessRate = g_ConfigRead.mix.Wing2MaxRate;
			}
		}
		else if (Obj.ChaosSuccessRate > g_ConfigRead.mix.Wing2MaxRate)
		{
			Obj.ChaosSuccessRate = g_ConfigRead.mix.Wing2MaxRate;
		}
	}

	Obj.ChaosSuccessRate += iCharmOfLuckCount;
	Obj.m_PlayerData->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	if ((rand() % 100) < Obj.ChaosSuccessRate)
	{
		int iWingLevel = 0;
		int iWingType;
		int iItemType;
		int iItemSubType;

		if (iSleeveOfLord != 0)	//1.01.00 updated
		{
			int iWing2rdType = rand() % 3;

			if (iWing2rdType == 0)
			{
				iItemType = 13;
				iItemSubType = 30;
			}
			else if (iWing2rdType == 1)
			{
				iItemType = 12;
				iItemSubType = 49;
			}
			else if (iWing2rdType == 2)
			{
				iItemType = 12;
				iItemSubType = 269;
			}

		}
		else
		{
			iItemType = 12;

			int iRand = rand() % 5;

			if (iRand == 4)
			{
				iItemSubType = 42;
			}
			else
			{
				iItemSubType = iRand + 3;
			}
		}

		if (WingCharmItemNum > -1) //season4.5 add-on
		{
			int WingCharmIndex = WingCharmItemNum ^ ITEMGET(13, 0);

			switch (WingCharmIndex)
			{
			case 88:
				iItemSubType = 5;
				break;
			case 89:
				iItemSubType = 4;
				break;
			case 90:
				iItemSubType = 3;
				break;
			case 91:
				iItemSubType = 42;
				break;
			case 92:
				iItemSubType = 6;
				break;
			default:
				sLog->outBasic("[MixSystem][WingChaosMix] WingCharmItemNum => %d , WingCharmIndex => %d", WingCharmItemNum, WingCharmIndex);
				break;
			}
		}

		int iWingNum = ITEMGET(iItemType, iItemSubType);
		int iOption2 = 0;
		int iOption3 = 0;

		int iRandomValue = rand() % 100;
		int iRandomValue2 = rand() % 3;

		switch (iRandomValue2)
		{
		case 0:
			if (iRandomValue < 4)
			{
				iOption2 = 3;	// +12
			}
			break;

		case 1:
			if (iRandomValue < 10)
			{
				iOption2 = 2;	// +8
			}
			break;

		case 2:
			if (iRandomValue < 20)
			{
				iOption2 = 1;	// +4;
			}
		}

		BYTE btNewExcOption[5];
		memset(btNewExcOption, -1, sizeof(btNewExcOption));

		if ((rand() % 5) == 0)
		{
			iOption3 = g_ItemOptionTypeMng.WingExcOptionRand(ItemAttribute[iWingNum].ItemKindA, ItemAttribute[iWingNum].ItemKindB, btNewExcOption);

			if (ItemAttribute[iWingNum].ItemKindB == ITEM_KIND_B_LORD_CAPE)
			{
				iOption3 |= 0x20;
			}

			else if ((rand() % 2) != 0)
			{
				iOption3 |= 0x20;
			}
		}

		int iOption1 = 0;

		if ((rand() % 100) <= g_ConfigRead.mix.Wing2MixRateLuck)
		{
			iOption1 = 1;
		}

		::ItemCreate(Obj.m_Index, -1, 0, 0, iWingNum, iWingLevel, 0, 0, iOption1, iOption2, -1, iOption3, 0, 0, btNewExcOption, 0);
		::gObjInventoryCommit(Obj.m_Index);
		::sLog->outBasic("[WingMix,2] [%s][%s] CBMix Success %d Money : %d-%d, CharmRate : %d",
			Obj.AccountID, Obj.Name, Obj.ChaosSuccessRate, Obj.m_PlayerData->Money, nChaosNeedMoney, iCharmOfLuckCount);
		return TRUE;
	}
	else
	{
		for (int n = 0; n < CHAOS_BOX_SIZE; n++)
		{
			Obj.pChaosBox[n].Clear();
		}

		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		::sLog->outBasic("[WingMix,2] [%s][%s] CBMix Fail %d Money : %d-%d, CharmRate : %d",
			Obj.AccountID, Obj.Name, Obj.ChaosSuccessRate, Obj.m_PlayerData->Money, nChaosNeedMoney, iCharmOfLuckCount);
		Obj.ChaosLock = FALSE;
		return FALSE;
	}
}

BOOL CMixSystem::IllusionTempleItemChaosMix(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;
	int ChoasGemCount = 0;
	int CovenantCount = 0;
	int OldScrollCount = 0;
	int CovenantLevel = 0;
	int OldScrollLevel = 0;
	int iCharmOfLuckCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 15)) // Chaos
			{
				ChoasGemCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(13, 49)) // Old Scroll
			{
				OldScrollCount++;
				OldScrollLevel = Obj.pChaosBox[n].m_Level;
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(13, 50)) // Covenant
			{
				CovenantCount++;
				CovenantLevel = Obj.pChaosBox[n].m_Level;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 53))	// Charm Of Luck
			{
				iCharmOfLuckCount += Obj.pChaosBox[n].m_Durability;
			}

		}
	}

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	if (ChoasGemCount == 1)
	{
		if (ChoasGemCount != 1 || OldScrollCount != 1 || CovenantCount != 1 || OldScrollLevel != CovenantLevel)
		{
			pMsg.Result = CB_INCORRECT_MIX_ITEMS;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
			Obj.ChaosLock = FALSE;

			return FALSE;
		}
	}
	else if (OldScrollCount == 1)
	{
		if (ChoasGemCount != 1 || OldScrollCount != 1 || CovenantCount != 1 || OldScrollLevel != CovenantLevel)
		{
			pMsg.Result = CB_INCORRECT_MIX_ITEMS;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
			Obj.ChaosLock = FALSE;

			return FALSE;
		}
	}
	else if (CovenantCount == 1)
	{
		if (ChoasGemCount != 1 || OldScrollCount != 1 || CovenantCount != 1 || OldScrollLevel != CovenantLevel)
		{
			pMsg.Result = CB_INCORRECT_MIX_ITEMS;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
			Obj.ChaosLock = FALSE;

			return FALSE;
		}
	}
	else
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);

		return FALSE;
	}

	if (iCharmOfLuckCount > 10)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = 0xF0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

		return FALSE;
	}

	int nChaosNeedMoney; //Fine ;D

	switch (OldScrollLevel)
	{
	case 1:
		nChaosNeedMoney = 3000000;
		break;
	case 2:
		nChaosNeedMoney = 5000000;
		break;
	case 3:
		nChaosNeedMoney = 7000000;
		break;
	case 4:
		nChaosNeedMoney = 9000000;
		break;
	case 5:
		nChaosNeedMoney = 11000000;
		break;
	case 6:
		nChaosNeedMoney = 13000000;
		break;
	default:
		nChaosNeedMoney = 3000000;
		break;
	}

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if (nChaosNeedMoney < 0)
	{
		nChaosNeedMoney = 0;
	}

	if (Obj.m_PlayerData->Money < nChaosNeedMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return TRUE;
	}

	Obj.ChaosSuccessRate = 70;

	if (g_CrywolfSync.GetOccupationState() == 0 && g_CrywolfSync.GetApplyBenefit() == TRUE)
	{
		Obj.ChaosSuccessRate += g_CrywolfSync.GetPlusChaosRate();
	}

	if (Obj.ChaosSuccessRate == 0 && ChoasGemCount != 1 && OldScrollCount != 1 && CovenantCount != 1 && OldScrollLevel != CovenantLevel)
	{
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return FALSE;
	}

	if (ChoasGemCount == 1 && OldScrollCount == 1 && CovenantCount == 1 && OldScrollLevel == CovenantLevel)
	{
		if (Obj.ChaosSuccessRate > 80)
		{
			Obj.ChaosSuccessRate = 80;
		}
	}
	else if (Obj.ChaosSuccessRate > 100)
	{
		Obj.ChaosSuccessRate = 100;
	}

	Obj.ChaosSuccessRate += iCharmOfLuckCount;
	Obj.m_PlayerData->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);

	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	if ((rand() % 100) < Obj.ChaosSuccessRate)
	{
		int iBloodScroll = ITEMGET(13, 51);
		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iBloodScroll, OldScrollLevel, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);
		return TRUE;
	}
	else
	{
		for (int n = 0; n < CHAOS_BOX_SIZE; n++)
		{
			Obj.pChaosBox[n].Clear();
		}

		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);

		Obj.ChaosLock = FALSE;

		return FALSE;
	}
}

BOOL CMixSystem::IsMixPossibleItem(int iItemCode)
{
	if (iItemCode == this->CONDOR_FEATHER_ITEMINDEX || iItemCode == this->CONDOR_FLAME_ITEMINDEX)
	{
		return true;
	}

	return false;
}

BOOL CMixSystem::CheckWingItem(int iItemCode)
{
	if (iItemCode == ITEMGET(12, 0) ||
		iItemCode == ITEMGET(12, 1) ||
		iItemCode == ITEMGET(12, 2) ||
		iItemCode == ITEMGET(12, 41))
	{
		return true;
	}

	return false;
}

BOOL CMixSystem::Check2ndWingItem(int iItemCode)
{
	if (iItemCode == ITEMGET(12, 3) ||
		iItemCode == ITEMGET(12, 4) ||
		iItemCode == ITEMGET(12, 5) ||
		iItemCode == ITEMGET(12, 6) ||
		iItemCode == ITEMGET(13, 30) ||
		iItemCode == ITEMGET(12, 42) ||
		iItemCode == ITEMGET(12, 49) ||
		iItemCode == ITEMGET(12, 269))
	{
		return true;
	}
	return false;
}

BOOL CMixSystem::Check3rdWingItem(int iItemCode)
{
	if (iItemCode == this->WING_OF_STORM_ITEMINDEX ||
		iItemCode == this->WING_OF_ETERNAL_ITEMINDEX ||
		iItemCode == this->WING_OF_ILLUSION_ITEMINDEX ||
		iItemCode == this->WING_OF_RUIN_ITEMINDEX ||
		iItemCode == this->CAPE_OF_EMPEROR_ITEMINDEX ||
		iItemCode == this->WING_OF_DIMENSION_ITEMINDEX ||
		iItemCode == this->CAPE_OF_OVERRULE_ITEMINDEX ||
		iItemCode == this->CLOAK_OF_TRANSCEDENCE_ITEMINDEX)
	{
		return true;
	}

	return false;
}

BOOL CMixSystem::CheckLevelCondition(CItemObject * lpItem, short Level, BYTE Op1, BYTE Op2, BYTE Op3, BYTE SetOption, BYTE NewOption)
{
	if (lpItem == NULL) { return FALSE; }
	if (Level != FALSE) { if (lpItem->m_Level < Level) { return FALSE; } }
	if (Op1 != FALSE) { if (lpItem->m_Option1 < Op1) { return FALSE; } }
	if (Op2 != FALSE) { if (lpItem->m_Option2 < Op2) { return FALSE; } }
	if (Op3 != FALSE) { if (lpItem->m_Option3 < Op3) { return FALSE; } }
	if (SetOption != FALSE) { if (!lpItem->m_SetOption) { return FALSE; } }
	if (NewOption != FALSE) { if (!lpItem->m_NewOption) { return FALSE; } }
	return TRUE;
}

BOOL CMixSystem::FeatherOfCondorMix(CGameObject &Obj)
{
	int ChaosCount = 0;
	int CreationCount = 0;
	int SoulPack10 = 0;
	int WingCount = 0;
	int ItemCount = 0;
	int ItemMoney = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 15))
		{
			ChaosCount++;
		}
		else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 22))
		{
			CreationCount++;
		}
		else if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 31) && Obj.pChaosBox[n].m_Level == 0)
		{
			SoulPack10++;
		}
		else if (((Obj.pChaosBox[n].m_Type >= ITEMGET(12, 3) && Obj.pChaosBox[n].m_Type <= ITEMGET(12, 6)) || Obj.pChaosBox[n].m_Type == ITEMGET(12, 42) || Obj.pChaosBox[n].m_Type == ITEMGET(12, 49) || Obj.pChaosBox[n].m_Type == ITEMGET(13, 30) || Obj.pChaosBox[n].m_Type == ITEMGET(12, 269)) && (Obj.pChaosBox[n].m_Level >= 9 && Obj.pChaosBox[n].m_Option3 >= 1))
		{
			WingCount++;
		}
		else if (Obj.pChaosBox[n].IsSetItem() != 0 && Obj.pChaosBox[n].m_Level >= 7 && Obj.pChaosBox[n].m_Option3 >= 1)
		{
			ItemCount++;
			ItemMoney += Obj.pChaosBox[n].m_BuyMoney;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	if (ChaosCount != 1 || CreationCount != 1 || SoulPack10 != 1 || WingCount != 1 || ItemCount == 0)
	{
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return FALSE;
	}

	Obj.ChaosSuccessRate = g_ConfigRead.mix.FeatherCondorMaxRate;

	if (this->GetTalismanOfLuckRate(lpObj, &Obj.ChaosSuccessRate) == 0)
	{
		pMsg.Result = CB_NOT_ENOUGH_EMPTY_SPACEX;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return 0;
	}

	Obj.ChaosSuccessRate = ((Obj.ChaosSuccessRate > 100) ? 100 : Obj.ChaosSuccessRate);

	Obj.ChaosMoney = Obj.ChaosSuccessRate * 200000;

	int TaxMoney = (Obj.ChaosMoney*g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index)) / 100;

	Obj.ChaosMoney += TaxMoney;

	if (Obj.m_PlayerData->Money < ((DWORD)Obj.ChaosMoney))
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return 0;
	}

	Obj.m_PlayerData->Money -= Obj.ChaosMoney;

	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	g_CastleSiegeSync.AddTributeMoney(TaxMoney);

	if ((GetLargeRand() % 100) < Obj.ChaosSuccessRate)
	{
		ItemCreate(Obj.m_Index, 0xFF, 0, 0, ITEMGET(13, 53), 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0xFF);
		::gObjInventoryCommit(Obj.m_Index);
		return TRUE;
	}
	else
	{
		this->ThirdWingMixFail(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		Obj.ChaosLock = FALSE;
		return FALSE;
	}
}

BOOL CMixSystem::AdvancedWingMix(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int ChoasGemCount = 0;
	int BundleOfSoulCount = 0;
	int BundleOfBlessCount = 0;
	int BundleOfCreationCount = 0;
	int BundleOfHarmonyCount = 0;
	int ExCItemObjectCount = 0;
	DWORD iChaosMoney = 0;
	int iCharmOfLuckCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if ((Obj.pChaosBox[n].m_Type >= ITEMGET(12, 0) && Obj.pChaosBox[n].m_Type <= ITEMGET(12, 6)) || Obj.pChaosBox[n].m_Type == ITEMGET(13, 30))
			{
				Obj.ChaosLock = FALSE;

				return FALSE;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 15)) // Chaos
			{
				ChoasGemCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 31)) // Bundle of Soul
			{
				BundleOfSoulCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 30)) // Bundle of Bless
			{
				BundleOfBlessCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 137)) // Jewel of Creation Bundle
			{
				BundleOfCreationCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 140)) // Harmony Bundle
			{
				BundleOfHarmonyCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 53)) // Charm Of Luck
			{
				iCharmOfLuckCount += Obj.pChaosBox[n].m_Durability;
			}
			/*	else if ( Obj.pChaosBox[n].IsExtItem() != FALSE) //Exc Item
			{
			if ( Obj.pChaosBox[n].m_Level >= 9 && Obj.pChaosBox[n].m_Option3 >= 1 )
			{
			ExCItemObjectCount++;
			iChaosMoney += Obj.pChaosBox[n].m_BuyMoney;
			}
			}*/
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	if (BundleOfHarmonyCount != 0 && BundleOfCreationCount != 0)
	{
		if (ChoasGemCount != 1 || BundleOfSoulCount != 1 || BundleOfBlessCount != 1 || BundleOfCreationCount != 1 || BundleOfHarmonyCount < 1)
		{
			pMsg.Result = CB_INCORRECT_MIX_ITEMS;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
			Obj.ChaosLock = FALSE;

			return FALSE;
		}
	}
	else
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);

		return FALSE;
	}

	if (iCharmOfLuckCount > 10)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = 0xF0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosSuccessRate = 0;
	Obj.ChaosSuccessRate += iChaosMoney / 3000000; //Good 40%

	if (Obj.ChaosSuccessRate < 1)
	{
		Obj.ChaosSuccessRate = 1;
	}

	else if (Obj.ChaosSuccessRate > g_ConfigRead.mix.Wing3MaxRate)
	{
		Obj.ChaosSuccessRate = g_ConfigRead.mix.Wing3MaxRate;
	}

	Obj.ChaosSuccessRate += iCharmOfLuckCount;

	int nChaosNeedMoney = Obj.ChaosSuccessRate * 200000;
	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	Obj.ChaosSuccessRate = 100;
	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if (nChaosNeedMoney < 0)
	{
		nChaosNeedMoney = 0;
	}

	if (Obj.m_PlayerData->Money < nChaosNeedMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return TRUE;
	}

	Obj.m_PlayerData->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	if ((rand() % 100) < Obj.ChaosSuccessRate)
	{
		int iWingLevel = 0;
		int iItemType;
		int iItemSubType;
		int iWingType;

		iItemType = 12;
		iWingType = rand() % 8;
		/*if(iWingType == 5)
		{
		iItemSubType = 43;
		}
		else if(iWingType == 6)
		{
		iItemSubType = 50;
		}
		else*/
		//{
		iItemSubType = iWingType + 51;
		//}
		//iItemSubType = rand()%5 + 36;

		int iWingNum = ITEMGET(iItemType, iItemSubType);
		int iOption1 = 0;
		int iOption2 = 0;
		int iOption3 = 0;

		if ((rand() % 5) == 0)
		{
			iOption1 = 1;
		}

		int iRandomValue = rand() % 100;
		int iRandomValue2 = rand() % 3;

		switch (iRandomValue2)
		{
		case 0:
			if (iRandomValue < 4)
			{
				iOption2 = 3;	// +12
			}
			break;

		case 1:
			if (iRandomValue < 10)
			{
				iOption2 = 2;	// +8
			}
			break;

		case 2:
			if (iRandomValue < 20)
			{
				iOption2 = 1;	// +4;
			}
		}

		//int ExOption;

		/*		if ( CondorFeatherCount != 0 && CondorFlameCount != 0)
		{
		if ( (rand()%5) == 0 )
		{
		iOption3 = 1 << (rand()%3);
		}

		if ( (rand()%2) != 0 )
		{
		iOption3 |= 0x20;
		}
		}*/

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iWingNum, iWingLevel, 0, 0, iOption1, iOption2, -1, iOption3, 0, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);
		return TRUE;
	}
	else
	{
		this->ThirdWingMixFail(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return FALSE;
	}
}

BOOL CMixSystem::ThirdWingLevel2ChaosMix(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int ChoasGemCount = 0;
	int BundleOfSoulCount = 0;
	int BundleOfBlessCount = 0;
	int JewelOfCreationCount = 0;
	int CondorFlameCount = 0;
	int CondorFeatherCount = 0;
	int ExCItemObjectCount = 0;
	DWORD iChaosMoney = 0;
	int iCharmOfLuckCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == this->CHAOS_GEM_ITEMINDEX) // Chaos
			{
				ChoasGemCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == this->SOUL_GEM_BULDLE_ITEMINDEX) // Bundle of Soul
			{
				BundleOfSoulCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == this->BLESS_GEM_BUNDLE_ITEMINDEX) // Bundle of Bless
			{
				BundleOfBlessCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == this->CREATURE_GEM_ITEMINDEX) // Jewel of Creation
			{
				JewelOfCreationCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == this->CONDOR_FLAME_ITEMINDEX) // Condor Flame
			{
				CondorFlameCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == this->CONDOR_FEATHER_ITEMINDEX) // Condor Feather
			{
				CondorFeatherCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 53)) // Charm Of Luck
			{
				iCharmOfLuckCount += Obj.pChaosBox[n].m_Durability;
			}

			else if (this->CheckLevelCondition(&Obj.pChaosBox[n], 7, 0, 0, 1, 0, 1) != FALSE)
			{
				ExCItemObjectCount++;
				iChaosMoney += Obj.pChaosBox[n].m_BuyMoney;
			}
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	if (CondorFeatherCount != 0 && CondorFlameCount != 0 && JewelOfCreationCount != 0)
	{
		if (ChoasGemCount != 1 || BundleOfSoulCount != 1 || BundleOfBlessCount != 1 || JewelOfCreationCount != 1 || ExCItemObjectCount < 1)
		{
			pMsg.Result = CB_INCORRECT_MIX_ITEMS;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
			Obj.ChaosLock = FALSE;

			return FALSE;
		}
	}
	else
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);

		return FALSE;
	}

	if (iCharmOfLuckCount > 10)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = 0xF0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosSuccessRate = 0;
	Obj.ChaosSuccessRate += iChaosMoney / 3000000; //Good 40%

	if (Obj.ChaosSuccessRate < 1)
	{
		Obj.ChaosSuccessRate = 1;
	}

	if (Obj.m_PlayerData->VipType != 0)
	{
		if (Obj.ChaosSuccessRate > g_VipSystem.GetWing3rdRate(lpObj))
		{
			Obj.ChaosSuccessRate = g_VipSystem.GetWing3rdRate(lpObj);
		}
	}

	else
	{
		if (Obj.ChaosSuccessRate > g_ConfigRead.mix.Wing3MaxRate)
		{
			Obj.ChaosSuccessRate = g_ConfigRead.mix.Wing3MaxRate;
		}
	}

	Obj.ChaosSuccessRate += iCharmOfLuckCount;

	int nChaosNeedMoney = Obj.ChaosSuccessRate * 200000;
	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if (nChaosNeedMoney < 0)
	{
		nChaosNeedMoney = 0;
	}

	if (Obj.m_PlayerData->Money < nChaosNeedMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return TRUE;
	}

	LogChaosItem(lpObj, "ThirdWingLevel2_Mix");
	sLog->outBasic("[ThirdWing Mix][Level 02] Chaos Mix Start");
	Obj.m_PlayerData->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	if ((rand() % 100) < Obj.ChaosSuccessRate)
	{
		int iWingType = rand() % 8;	//1.01.00 updated
		int iWingNum = 0;

		if (iWingType == 0)
		{
			iWingNum = this->WING_OF_STORM_ITEMINDEX;
		}
		else if (iWingType == 1)
		{
			iWingNum = this->WING_OF_ETERNAL_ITEMINDEX;
		}
		else if (iWingType == 2)
		{
			iWingNum = this->WING_OF_ILLUSION_ITEMINDEX;
		}
		else if (iWingType == 3)
		{
			iWingNum = this->WING_OF_RUIN_ITEMINDEX;
		}
		else if (iWingType == 4)
		{
			iWingNum = this->CAPE_OF_EMPEROR_ITEMINDEX;
		}
		else if (iWingType == 5)
		{
			iWingNum = this->WING_OF_DIMENSION_ITEMINDEX;
		}
		else if (iWingType == 6)	//1.01.00 updated
		{
			iWingNum = this->CAPE_OF_OVERRULE_ITEMINDEX;
		}
		else if (iWingType == 7)
		{
			iWingNum = this->CLOAK_OF_TRANSCEDENCE_ITEMINDEX;
		}

		int iOption2 = 0;
		int iExcOpt = 0;
		BYTE btNewExcOption[5];
		memset(btNewExcOption, -1, sizeof(btNewExcOption));

		iExcOpt = g_ItemOptionTypeMng.WingExcOptionRand(ItemAttribute[iWingNum].ItemKindA, ItemAttribute[iWingNum].ItemKindB, btNewExcOption);

		int iOptTypeRand = rand() % 2;
		int iOptRateRand;

		switch (iOptTypeRand)
		{
		case 0:
			iOptRateRand = rand() % 1000;
			if (iOptRateRand < 400)
			{
				iExcOpt |= 0x10;
			}
			break;

		case 1:
			iOptRateRand = rand() % 1000;
			if (iOptRateRand < 300)
			{
				iExcOpt |= 0x20;
			}
			break;
		}

		int iOpt3TypeLevel = rand() % 4;
		int iOpt3TypeLevelRate = rand() % 1000;

		switch (iOpt3TypeLevel)
		{
		case 1:
		{
			if (iOpt3TypeLevelRate < 120)
			{
				iOption2 = 1;
			}
		}
		break;

		case 2:
		{
			if (iOpt3TypeLevelRate < 60)
			{
				iOption2 = 2;
			}
		}
		break;

		case 3:
		{
			if (iOpt3TypeLevelRate < 30)
			{
				iOption2 = 3;
			}
		}
		break;
		}

		int iOption1 = 0;

		if ((rand() % 100) <= g_ConfigRead.mix.Wing3MixRateLuck)
		{
			iOption1 = 1;
		}

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iWingNum, 0, 0, 0, iOption1, iOption2, -1, iExcOpt, 0, 0, btNewExcOption, 0);
		::gObjInventoryCommit(Obj.m_Index);
		::sLog->outBasic("[ThirdWing Mix][Level 02] [%s][%s] CBMix Success %d Money : %d-%d, CharmRate : %d",
			Obj.AccountID, Obj.Name, Obj.ChaosSuccessRate, Obj.m_PlayerData->Money, nChaosNeedMoney, iCharmOfLuckCount);
		return TRUE;
	}
	else
	{
		this->ThirdWingMixFail(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		::sLog->outBasic("[ThirdWing Mix][Level 02] [%s][%s] CBMix Fail %d Money : %d-%d, CharmRate : %d",
			Obj.AccountID, Obj.Name, Obj.ChaosSuccessRate, Obj.m_PlayerData->Money, nChaosNeedMoney, iCharmOfLuckCount);

		Obj.ChaosLock = FALSE;

		return FALSE;
	}
}

void CMixSystem::ThirdWingMixFail(CGameObject &Obj)
{
	if (Obj.pChaosBox == NULL)
	{
		return;
	}

	for (int n = 0; n < CHAOS_BOX_SIZE; n++) //loc2
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Check2ndWingItem(Obj.pChaosBox[n].m_Type) != FALSE)
			{
				ThirdWingMixFailItemPanalty(&Obj.pChaosBox[n]);
			}
			else if (CheckLevelCondition(&Obj.pChaosBox[n], 0, 0, 0, 0, 1, 0) != FALSE)
			{
				ThirdWingMixFailItemPanalty(&Obj.pChaosBox[n]);
			}
			else if (CheckLevelCondition(&Obj.pChaosBox[n], 0, 0, 0, 0, 0, 1) != FALSE)
			{
				ThirdWingMixFailItemPanalty(&Obj.pChaosBox[n]);
			}
			else
			{
				Obj.pChaosBox[n].Clear();
			}
		}
	}
}

void CMixSystem::ThirdWingMixFailItemPanalty(CItemObject * lpItem)
{
	if (lpItem == NULL)
	{
		return;
	}

	if ((rand() % 2) < 1)
	{
		lpItem->m_Level -= 2;
	}
	else
	{
		lpItem->m_Level -= 3;
	}

	lpItem->m_Option3 = 0;
	lpItem->Convert(lpItem->m_Type, lpItem->m_Option1, lpItem->m_Option2, lpItem->m_Option3, lpItem->m_NewOption, lpItem->m_SetOption, lpItem->m_ItemOptionEx, lpItem->m_SocketOption, lpItem->m_BonusSocketOption, 0, CURRENT_DB_VERSION);
}

void CMixSystem::DevilSquareItemChaosMix(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	if (g_DevilSquare.IsEventEnable() == true)
	{
		PMSG_CHAOSMIXRESULT pMsg;
		PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
		pMsg.Result = CB_ERROR;
		CGameObject &Obj = Obj.m_Index;
		int eventitemcount;
		int itemlevel;
		int Ret = CheckDevilSquareItem(lpObj, eventitemcount, itemlevel);

		if (Ret != FALSE)
		{
			if (Ret == 3)
			{
				pMsg.Result = CB_INCORRECT_MIX_ITEMS;
				IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
				Obj.ChaosLock = FALSE;

				return;
			}

			if (Ret == 2)
			{
				pMsg.Result = CB_INCORRECT_MIX_ITEMS;
				IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
				Obj.ChaosLock = FALSE;

				return;
			}

			if (Ret == 4)
			{
				pMsg.Result = 0xF0;
				IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
				Obj.ChaosLock = FALSE;

				return;
			}

			if (eventitemcount > 2)
			{
				pMsg.Result = CB_TOO_MANY_ITEMS;
				IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
				Obj.ChaosLock = FALSE;

				return;
			}

			if (Obj.Level < 10)
			{
				pMsg.Result = CB_LOW_LEVEL_USER;
				IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
				Obj.ChaosLock = FALSE;

				return;
			}

			DevilSquareEventChaosMix(lpObj, Ret, itemlevel);
			return;
		}

		if (eventitemcount > 1)
		{
			pMsg.Result = CB_LACKING_MIX_ITEMS;
			IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
			Obj.ChaosLock = FALSE;

			return;
		}
	}

	Obj.ChaosLock = FALSE;
}

void CMixSystem::BloodCastleItemChaosMix(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	CGameObject &Obj = Obj.m_Index;

	int iRET_VAL = g_BloodCastle.CheckChoasMixItem(Obj.m_Index);

	int cMapNum = g_BloodCastle.GetBridgeMapNumber(iRET_VAL - 1);

	if (BC_MAP_RANGE(cMapNum) != FALSE)
	{
		if (g_BloodCastle.BloodCastleChaosMix(Obj.m_Index, iRET_VAL) == false)
		{
			Obj.ChaosLock = FALSE;
		}

		return;
	}

	switch (iRET_VAL)
	{
	case 9:
		pMsg.Result = CB_NO_BC_CORRECT_ITEMS;
		IOCP.DataSend(Obj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		break;

	case 10:
		pMsg.Result = CB_NO_BC_CORRECT_ITEMS;
		IOCP.DataSend(Obj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		break;

	case 11:
		pMsg.Result = CB_NO_BC_CORRECT_ITEMS;
		IOCP.DataSend(Obj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		break;

	case 12:
		pMsg.Result = CB_INVALID_ITEM_LEVEL;
		IOCP.DataSend(Obj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		break;

	case 13:
		pMsg.Result = CB_BC_NOT_ENOUGH_ZEN;
		IOCP.DataSend(Obj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		break;

	case 14:
		pMsg.Result = CB_USER_CLASS_LOW_LEVEL;
		IOCP.DataSend(Obj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		break;
	case 15:
		pMsg.Result = 0xF0;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		break;
	default:
		Obj.ChaosLock = FALSE;
	}

}

void CMixSystem::SetItemChaosMix(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int ChoasGemCount = 0;
	int BlessGemCount = 0;
	int SoulGemCount = 0;
	int MetalOfAncientCount = 0;
	int MixItemCount = 0;
	int MixSetItemIndex = 0;
	int iMixItemChaosMoney = 0;
	DWORD iChaosMoney = 0;
	int iMetalOfAncientItemLevel = -1;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 13))
			{
				BlessGemCount++;

				if (BlessGemCount > 3)
				{
					iChaosMoney += Obj.pChaosBox[n].m_BuyMoney;
				}
			}

			if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 14))
			{
				SoulGemCount++;

				if (SoulGemCount > 3)
				{
					iChaosMoney += Obj.pChaosBox[n].m_BuyMoney;
				}
			}

			if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 16))
			{
				iChaosMoney += Obj.pChaosBox[n].m_BuyMoney;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 15))
			{
				ChoasGemCount++;
			}
			else if (Obj.pChaosBox[n].m_Level >= 6 && Obj.pChaosBox[n].IsExtItem() != FALSE)
			{
				if (Obj.pChaosBox[n].IsSetItem() != FALSE)
				{
					MixItemCount++;
					MixSetItemIndex = n;
					iMixItemChaosMoney += Obj.pChaosBox[n].m_BuyMoney;
				}
			}
			else if (((Obj.pChaosBox[n].m_Type >= ITEMGET(13, 8) && Obj.pChaosBox[n].m_Type < ITEMGET(13, 14)) || (Obj.pChaosBox[n].m_Type >= ITEMGET(13, 20) && Obj.pChaosBox[n].m_Type < ITEMGET(13, 28))) && (Obj.pChaosBox[n].m_Option3 >= 2 && Obj.pChaosBox[n].m_Level >= 3))
			{
				if (Obj.pChaosBox[n].IsSetItem() != FALSE)
				{
					MixItemCount++;
					MixSetItemIndex = n;
					iMixItemChaosMoney += Obj.pChaosBox[n].m_BuyMoney;
				}
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 27)) // Ancien Metal, Apply Deathway Fix here
			{
				MetalOfAncientCount++;
				iMetalOfAncientItemLevel = Obj.pChaosBox[n].m_Level;
			}
			else
			{
				IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
				Obj.ChaosLock = FALSE;
			}
		}
	}

	if (ChoasGemCount < 1 || BlessGemCount < 3 || SoulGemCount < 3 || MetalOfAncientCount != 1 || MixItemCount != 1)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	if (gSetItemOption.CheckMixContition(Obj.pChaosBox[MixSetItemIndex].m_Type, iMetalOfAncientItemLevel) == FALSE)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	Obj.ChaosSuccessRate = iMixItemChaosMoney / 40000;
	Obj.ChaosSuccessRate = iChaosMoney / 400000;

	if (Obj.ChaosSuccessRate > 80)
	{
		Obj.ChaosSuccessRate = 80;
	}

	int nChaosNeedMoney = (Obj.ChaosSuccessRate - 50) * 1000000;

	if (Obj.m_PlayerData->Money < nChaosNeedMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	int setoption = gSetItemOption.MakeSetOption(Obj.pChaosBox[MixSetItemIndex].m_Type, iMetalOfAncientItemLevel);

	if (setoption == 0)
	{
		pMsg.Result = CB_ERROR;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	Obj.m_PlayerData->Money -= nChaosNeedMoney;
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	if ((rand() % 100) < Obj.ChaosSuccessRate)
	{
		Obj.pChaosBox[MixSetItemIndex].m_SetOption = setoption;

		if ((rand() % 100) < 80)
		{
			Obj.pChaosBox[MixSetItemIndex].m_SetOption |= 4;
		}
		else
		{
			Obj.pChaosBox[MixSetItemIndex].m_SetOption |= 8;
		}

		pMsg.Result = CB_SUCCESS;
		CItemObject Item;

		float Dur = ItemGetDurability(Obj.pChaosBox[MixSetItemIndex].m_Type, Obj.pChaosBox[MixSetItemIndex].m_Level,
			Obj.pChaosBox[MixSetItemIndex].IsExtItem(), gSetItemOption.IsSetItem(Obj.pChaosBox[MixSetItemIndex].m_Type));

		Item.m_Level = Obj.pChaosBox[MixSetItemIndex].m_Level;
		Item.m_Durability = Dur * Obj.pChaosBox[MixSetItemIndex].m_Durability / Obj.pChaosBox[MixSetItemIndex].m_BaseDurability;
		Item.m_JewelOfHarmonyOption = Obj.pChaosBox[MixSetItemIndex].m_JewelOfHarmonyOption;

		Item.Convert(Obj.pChaosBox[MixSetItemIndex].m_Type, Obj.pChaosBox[MixSetItemIndex].m_Option1, Obj.pChaosBox[MixSetItemIndex].m_Option2,
			Obj.pChaosBox[MixSetItemIndex].m_Option3, Obj.pChaosBox[MixSetItemIndex].m_NewOption,
			Obj.pChaosBox[MixSetItemIndex].m_SetOption, Obj.pChaosBox[MixSetItemIndex].m_ItemOptionEx, Obj.pChaosBox[MixSetItemIndex].m_SocketOption, Obj.pChaosBox[MixSetItemIndex].m_BonusSocketOption, 0, CURRENT_DB_VERSION);

		Item.m_Number = Obj.pChaosBox[MixSetItemIndex].m_Number;

		ItemByteConvert(pMsg.ItemInfo, Item);
		ChaosBoxInit(lpObj);
		::gObjChaosBoxInsertItemPos(Obj.m_Index, Item, 0, -1);
		gObjChaosItemSet(Obj.m_Index, 0, 1);

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);

	}
	else
	{
		ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);

		Obj.ChaosLock = FALSE;
	}
}


void CMixSystem::DarkHorseChaosMix(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;
	int ChoasGemCount = 0;
	int BlessGemCount = 0;
	int SoulGemCount = 0;
	int CreatureGemCount = 0;
	int SoulOfDarkHorse = 0;
	DWORD iChaosMoney = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;
	int iCharmOfLuckCount = 0;
	int iInvalidItemCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 13))
			{
				BlessGemCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 14))
			{
				SoulGemCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 22))	// Creation
			{
				CreatureGemCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 15))	// SUPE CHOAS HAHAHA
			{
				ChoasGemCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(13, 31) && Obj.pChaosBox[n].m_Level == 0)	// Spirit
			{
				SoulOfDarkHorse++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 53))	// Charm Of Luck
			{
				iCharmOfLuckCount += Obj.pChaosBox[n].m_Durability;
			}
			else
			{
				iInvalidItemCount++;
			}

		}
	}

	if (ChoasGemCount != 1 || BlessGemCount != 5 || SoulGemCount != 5 || CreatureGemCount != 1 || SoulOfDarkHorse != 1 || iInvalidItemCount > 0)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	if (iCharmOfLuckCount > 10)
	{
		pMsg.Result = 0xF0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	Obj.ChaosSuccessRate = 60;
	Obj.ChaosSuccessRate += iCharmOfLuckCount;

	int nChaosNeedMoney = 5000000;

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if (nChaosNeedMoney < 0)
	{
		nChaosNeedMoney = 0;
	}

	if (Obj.m_PlayerData->Money < nChaosNeedMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	Obj.m_PlayerData->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	if ((rand() % 100) < Obj.ChaosSuccessRate)
	{
		int Level = 0;
		int ItemType = 13;
		int ItemSubType = 4;
		int ItemNum = ITEMGET(ItemType, ItemSubType);
		int Option1 = 0;
		int Option2 = 0;
		int Option3 = 0;

		int Add = 0;

		if ((rand() % 100) < (Obj.ChaosSuccessRate / 5 + 6))
		{
			Add = 1;
		}

		if ((rand() % 5) == 0)
		{
			Option1 = 1;
		}

		int lc22 = rand() % 100;
		int lc23 = rand() % 3;
		//int lc24 = lc23;

		switch (lc23)
		{
		case 0:
			if (lc22 < 4)
			{
				Option2 = 3;
			}
			break;

		case 1:
			if (lc22 < 10)
			{
				Option2 = 2;
			}
			break;

		case 2:
			if (lc22 < 20)
			{
				Option2 = 1;
			}
			break;
		}

		::PetItemCreate(Obj.m_Index, 0xFE, 0, 0, ItemNum, Level, 0, Add, Option1, Option2, -1, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);
	}
	else
	{
		ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 1);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);

		Obj.ChaosLock = FALSE;
	}
}

void CMixSystem::DarkSpiritChaosMix(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;
	int ChoasGemCount = 0;
	int BlessGemCount = 0;
	int SoulGemCount = 0;
	int CreatureGemCount = 0;
	int SoulOfSpirit = 0;
	DWORD iChaosMoney = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;
	int iCharmOfLuckCount = 0;
	int iInvalidItemCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 13))
			{
				BlessGemCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 14))
			{
				SoulGemCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 22))	// Creation
			{
				CreatureGemCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 15))	// SUPE CHOAS HAHAHA
			{
				ChoasGemCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(13, 31) && Obj.pChaosBox[n].m_Level == 1)	// Spirit
			{
				SoulOfSpirit++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 53))	// Charm Of Luck
			{
				iCharmOfLuckCount += Obj.pChaosBox[n].m_Durability;
			}
			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if (ChoasGemCount != 1 || BlessGemCount != 2 || SoulGemCount != 2 || CreatureGemCount != 1 || SoulOfSpirit != 1 || iInvalidItemCount > 0)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	if (iCharmOfLuckCount > 10)
	{
		pMsg.Result = 0xF0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	Obj.ChaosSuccessRate = 60;
	Obj.ChaosSuccessRate += iCharmOfLuckCount;

	int nChaosNeedMoney = 1000000;

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if (nChaosNeedMoney < 0)
	{
		nChaosNeedMoney = 0;
	}

	if (Obj.m_PlayerData->Money < nChaosNeedMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	Obj.m_PlayerData->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	if ((rand() % 100) < Obj.ChaosSuccessRate)
	{
		int Level = 0;
		int ItemType = 13;
		int ItemSubType = 5;
		int ItemNum = ITEMGET(ItemType, ItemSubType);
		int Option1 = 0;
		int Option2 = 0;
		int Option3 = 0;

		int Add = 0;

		if ((rand() % 100) < (Obj.ChaosSuccessRate / 5 + 6))
		{
			Add = 1;
		}

		if ((rand() % 5) == 0)
		{
			Option1 = 1;
		}

		int lc22 = rand() % 100;
		int lc23 = rand() % 3;
		//		int lc24 = lc23;

		switch (lc23)
		{
		case 0:
			if (lc22 < 4)
			{
				Option2 = 3;
			}
			break;

		case 1:
			if (lc22 < 10)
			{
				Option2 = 2;
			}
			break;

		case 2:
			if (lc22 < 20)
			{
				Option2 = 1;
			}
			break;
		}

		::PetItemCreate(Obj.m_Index, 0xFE, 0, 0, ItemNum, Level, 0, Add, Option1, Option2, -1, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);
	}
	else
	{
		ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 1);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);

		Obj.ChaosLock = FALSE;
	}
}

void CMixSystem::BlessPotionChaosMix(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;
	int iBlessGemCount = 0;
	DWORD iChaosMoney = 0;
	int iInvalidItemCount = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 13))
			{
				iBlessGemCount++;
			}
			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if (iBlessGemCount == 0)
		return;

	if (iInvalidItemCount > 0)
		return;

	if (iBlessGemCount > 25)
	{
		MsgOutput(Obj.m_Index, Lang.GetText(0, 182));
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	Obj.ChaosSuccessRate = 100;

	int nChaosNeedMoney = 100000;

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if (nChaosNeedMoney < 0)
	{
		nChaosNeedMoney = 0;
	}

	if (Obj.m_PlayerData->Money < nChaosNeedMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	Obj.m_PlayerData->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	if ((rand() % 100) < Obj.ChaosSuccessRate)
	{
		int ItemNum = ITEMGET(14, 7);
		int Level = 0;
		int Dur = iBlessGemCount * 10;

		ItemCreate(Obj.m_Index, 0xFF, 0, 0, ItemNum, Level, Dur, 0, 0, 0, -1, 0, 0, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		ChaosBoxInit(lpObj);
		::gGameProtocol.GCUserChaosBoxSend(lpObj, 1);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);

		Obj.ChaosLock = FALSE;
	}
}

void CMixSystem::SoulPotionChaosMix(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;
	int iSoulGemCount = 0;
	DWORD iChaosMoney = 0;
	int iInvalidItemCount = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 14))
			{
				iSoulGemCount++;
			}
			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if (iSoulGemCount == 0)
		return;

	if (iInvalidItemCount > 0)
		return;

	if (iSoulGemCount > 25)
	{
		MsgOutput(Obj.m_Index, Lang.GetText(0, 182));
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	Obj.ChaosSuccessRate = 100;

	int nChaosNeedMoney = 50000;

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if (nChaosNeedMoney < 0)
	{
		nChaosNeedMoney = 0;
	}

	if (Obj.m_PlayerData->Money < nChaosNeedMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	Obj.m_PlayerData->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	if ((rand() % 100) < Obj.ChaosSuccessRate)
	{
		int ItemNum = ITEMGET(14, 7);
		int Level = 1;
		int Dur = iSoulGemCount * 10;

		ItemCreate(Obj.m_Index, 0xFF, 0, 0, ItemNum, Level, Dur, 0, 0, 0, -1, 0, 0, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		ChaosBoxInit(lpObj);
		::gGameProtocol.GCUserChaosBoxSend(lpObj, 1);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);

		Obj.ChaosLock = FALSE;
	}
}



void CMixSystem::LifeStoneChaosMix(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;
	int iChoasGemCount = 0;
	int iBlessGemCount = 0;
	int iSoulGemCount = 0;
	int iProtectionGemCount = 0;
	int iInvalidItemCount = 0;
	DWORD iChaosMoney = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 13))
			{
				iBlessGemCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 14))
			{
				iSoulGemCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 31))	// Guardian Jewel
			{
				iProtectionGemCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 15))	// Habla pe super CHOAS
			{
				iChoasGemCount++;
			}
			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if (iChoasGemCount != 1 || iBlessGemCount != 5 || iSoulGemCount != 5 || iProtectionGemCount != 1 || iInvalidItemCount > 0)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	Obj.ChaosSuccessRate = 100;

	int nChaosNeedMoney = 5000000;

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if (nChaosNeedMoney < 0)
	{
		nChaosNeedMoney = 0;
	}

	if (Obj.m_PlayerData->Money < nChaosNeedMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	Obj.m_PlayerData->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	if ((rand() % 100) < Obj.ChaosSuccessRate)
	{
		ItemCreate(Obj.m_Index, 0xFF, 0, 0, ITEMGET(13, 11), 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		ChaosBoxInit(lpObj);
		::gGameProtocol.GCUserChaosBoxSend(lpObj, 1);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);

		Obj.ChaosLock = FALSE;
	}
}

void CMixSystem::CastleSpecialItemMix(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int iDefendGemCount = 0;
	int iBlessGemMixCount = 0;
	int iSoulGemMixCount = 0;
	int iInvalidItemCount = 0;
	DWORD iChaosMoney = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	if (g_CastleSiegeSync.CheckCastleOwnerMember(Obj.m_Index))
	{
		if (Obj.m_PlayerData->GuildStatus != GUILD_MASTER)
		{

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

			Obj.ChaosLock = FALSE;

			return;
		}
	}
	else
	{

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

		Obj.ChaosLock = FALSE;

		return;
	}

	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

		Obj.ChaosLock = FALSE;

		return;
	}

	if (g_CastleSiege.GetLordMixLimit() <= 0)
	{

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

		Obj.ChaosLock = FALSE;

		return;
	}

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 30))
			{
				iBlessGemMixCount += Obj.pChaosBox[n].m_Level + 1;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 31))
			{
				iSoulGemMixCount += Obj.pChaosBox[n].m_Level + 1;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 31))
			{
				iDefendGemCount++;
			}
			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if (iBlessGemMixCount != 3 || iSoulGemMixCount != 3 || iDefendGemCount != 30 || iInvalidItemCount > 0)
	{

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

		Obj.ChaosLock = FALSE;

		return;
	}

	Obj.ChaosSuccessRate = 100;

	int nChaosNeedMoney = 1000000000;

	if (Obj.m_PlayerData->Money < nChaosNeedMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

		Obj.ChaosLock = FALSE;

		return;
	}

	Obj.m_PlayerData->Money -= nChaosNeedMoney;

	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	if ((rand() % 100) < Obj.ChaosSuccessRate)
	{
		CItemObject Item;
		time_t Duration;

		if (g_BagManager.GetItemFromBag(Obj.m_Index, BAG_EVENT, EVENTBAG_LORDMIX, Obj.m_Index, Item, Duration) != TRUE)
		{
			ChaosBoxInit(lpObj);
			gGameProtocol.GCUserChaosBoxSend(lpObj, 1);

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

			Obj.ChaosLock = FALSE;
			return;
		}

		ItemCreate(Obj.m_Index, 255, 0, 0, Item.m_Type, Item.m_Level, Item.m_Durability, Item.m_Option1, Item.m_Option2, Item.m_Option3, Obj.m_Index, Item.m_NewOption, Item.m_SetOption, Duration, Item.m_SocketOption, Item.m_BonusSocketOption);
		gObjInventoryCommit(Obj.m_Index);

		sLog->outBasic("[CastleSpecialMix] [%s][%s] CBMix Success %d Money : %d-%d",
			Obj.AccountID, Obj.Name, Obj.ChaosSuccessRate, Obj.m_PlayerData->Money, nChaosNeedMoney);

		g_CastleSiege.DecreaseLordMixLimit();

	}
	else
	{
		ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 1);

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

		sLog->outBasic("[CastleSpecialMix] [%s][%s] CBMix Fail %d Money : %d-%d",
			Obj.AccountID, Obj.Name, Obj.ChaosSuccessRate, Obj.m_PlayerData->Money, nChaosNeedMoney);
	}

	Obj.ChaosLock = FALSE;
}

void CMixSystem::HiddenTreasureBoxItemMix(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;
	int iBlueCrystal = 0;
	int iRedCrystal = 0;
	int iBlackCrystal = 0;
	int iInvalidItemCount = 0;
	DWORD iChaosMoney = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 26))
			{
				switch (Obj.pChaosBox[n].m_Level)
				{
				case 1:
					iRedCrystal++;
					break;
				case 2:
					iBlueCrystal++;
					break;
				case 3:
					iBlackCrystal++;
					break;
				}
			}
			else
			{
				iInvalidItemCount++;
			}
		}
	}

	int iCrystalMixType = -1;

	if (iInvalidItemCount == 0)
	{
		if (iBlueCrystal == 7)
		{
			if (iRedCrystal == 0 && iBlackCrystal == 0)
			{
				iCrystalMixType = 0;
				Obj.ChaosSuccessRate = 100;
			}
		}
		else if (iRedCrystal == 5)
		{
			if (iBlueCrystal == 0 && iBlackCrystal == 0)
			{
				iCrystalMixType = 1;
				Obj.ChaosSuccessRate = 100;
			}
		}
		else if (iBlackCrystal == 3)
		{
			if (iRedCrystal == 0 && iBlueCrystal == 0)
			{
				iCrystalMixType = 2;
				Obj.ChaosSuccessRate = 100;
			}
		}
		else if (iBlueCrystal == 1 && iRedCrystal == 1 && iBlackCrystal == 1)
		{
			{
				iCrystalMixType = 3;
				Obj.ChaosSuccessRate = 100;
			}
		}
		else
		{
			sLog->outBasic("[Hidden TreasureBox Event] [%s][%s] Item Mix Failed",
				Obj.AccountID, Obj.Name);

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
			Obj.ChaosLock = FALSE;
		}
	}
	else
	{
		sLog->outBasic("[Hidden TreasureBox Event] [%s][%s] Item Mix Failed",
			Obj.AccountID, Obj.Name);

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	LogChaosItem(lpObj, "Hidden TreasureBox Mix");
	sLog->outBasic("[Hidden TreasureBox Event] [%s][%s] Chaos Mix Start",
		Obj.AccountID, Obj.Name);

	if ((rand() % 100) < Obj.ChaosSuccessRate)
	{
		switch (iCrystalMixType)
		{
		case 0:	case 1:	case 2:
			ItemCreate(Obj.m_Index, 0xFF, 0, 0, ITEMGET(12, 26), 4, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0);
			break;
		case 3:
			ItemCreate(Obj.m_Index, 0xFF, 0, 0, ITEMGET(12, 26), 5, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0);
			break;
		default:
			sLog->outBasic("[Hidden TreasureBox Event] [%s][%s] CBMix Failed - iCrystalMixType is wrong : %d",
				Obj.AccountID, Obj.Name, iCrystalMixType);
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
			Obj.ChaosLock = FALSE;
			return;
		}

		::gObjInventoryCommit(Obj.m_Index);
		sLog->outBasic("[Hidden TreasureBox Event] [%s][%s] CBMix Success:%d Type:%d",
			Obj.AccountID, Obj.Name, Obj.ChaosSuccessRate, iCrystalMixType);
	}
	else
	{
		sLog->outBasic("[Hidden TreasureBox Event] [%s][%s] CBMix Fail Rate:%d",
			Obj.AccountID, Obj.Name, Obj.ChaosSuccessRate);

		ChaosBoxInit(lpObj);
		::gGameProtocol.GCUserChaosBoxSend(lpObj, 1);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);

		Obj.ChaosLock = FALSE;
	}

	Obj.ChaosLock = FALSE;
}



void CMixSystem::Fenrir_01Level_Mix(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;
	int iStuffCount_01 = 0;
	int iStuffCount_02 = 0;
	int iChaosGemCount = 0;
	int iInvalidItemCount = 0;
	DWORD iChaosMoney = 0;
	int iCharmOfLuckCount = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(13, 32))
			{
				iStuffCount_01 += Obj.pChaosBox[n].m_Durability;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(13, 33))
			{
				iStuffCount_02 += Obj.pChaosBox[n].m_Durability;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 15))
			{
				iChaosGemCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 53))	// Charm Of Luck
			{
				iCharmOfLuckCount += Obj.pChaosBox[n].m_Durability;
			}
			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if (iStuffCount_01 != 20 || iStuffCount_02 != 20 || iChaosGemCount != 1 || iInvalidItemCount > 0)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	if (iCharmOfLuckCount > 10)
	{
		pMsg.Result = 0xF0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	LogChaosItem(lpObj, "Fenrir_01Level_Mix");
	sLog->outBasic("[Fenrir Mix][Level 01] Chaos Mix Start");

	Obj.ChaosSuccessRate = g_iFenrir_01Level_MixRate;
	Obj.ChaosSuccessRate += iCharmOfLuckCount;
	int nChaosNeedMoney = 0;	// #error Set the Correct price before call the CstleSyegeTributes

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if (nChaosNeedMoney < 0)
	{
		nChaosNeedMoney = 0;
	}

	if (Obj.m_PlayerData->Money < nChaosNeedMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	Obj.m_PlayerData->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	if ((rand() % 100) < Obj.ChaosSuccessRate)
	{
		int type = ITEMGET(13, 35);
		int level = 0;
		int dur = 1;

		ItemCreate(Obj.m_Index, 0xFF, 0, 0, type, level, dur, 0, 0, 0, -1, 0, 0, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

		sLog->outBasic("[Fenrir Mix][Level 01] [%s][%s] CBMix Success %d Money : %d-%d, CharmRate : %d",
			Obj.AccountID, Obj.Name, Obj.ChaosSuccessRate, Obj.m_PlayerData->Money, nChaosNeedMoney,
			iCharmOfLuckCount);

	}
	else
	{
		ChaosBoxInit(lpObj);
		::gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);

		sLog->outBasic("[Fenrir Mix][Level 01] [%s][%s] CBMix Fail %d Money : %d-%d, CharmRate : %d",
			Obj.AccountID, Obj.Name, Obj.ChaosSuccessRate, Obj.m_PlayerData->Money, nChaosNeedMoney,
			iCharmOfLuckCount);

		Obj.ChaosLock = FALSE;
	}
}

void CMixSystem::Fenrir_02Level_Mix(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;
	int iStuffCount_01 = 0;
	int iStuffCount_02 = 0;
	int iChaosGemCount = 0;
	int iInvalidItemCount = 0;
	DWORD iChaosMoney = 0;
	int iCharmOfLuckCount = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(13, 34))
			{
				iStuffCount_01 += Obj.pChaosBox[n].m_Durability;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(13, 35))
			{
				iStuffCount_02++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 15))
			{
				iChaosGemCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 53))	// Charm Of Luck
			{
				iCharmOfLuckCount += Obj.pChaosBox[n].m_Durability;
			}
			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if (iStuffCount_01 != 10 || iStuffCount_02 != 5 || iChaosGemCount != 1 || iInvalidItemCount > 0)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	if (iCharmOfLuckCount > 10)
	{
		pMsg.Result = 0xF0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	LogChaosItem(lpObj, "Fenrir_02Level_Mix");
	sLog->outBasic("[Fenrir Mix][Level 02] Chaos Mix Start");

	Obj.ChaosSuccessRate = g_iFenrir_02Level_MixRate;
	Obj.ChaosSuccessRate += iCharmOfLuckCount;
	int nChaosNeedMoney = 0;	// #error Set the Correct price before call the CstleSyegeTributes

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if (nChaosNeedMoney < 0)
	{
		nChaosNeedMoney = 0;
	}

	if (Obj.m_PlayerData->Money < nChaosNeedMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	Obj.m_PlayerData->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	if ((rand() % 100) < Obj.ChaosSuccessRate)
	{
		int type = ITEMGET(13, 36);
		int level = 0;
		int dur = 1;

		ItemCreate(Obj.m_Index, 0xFF, 0, 0, type, level, dur, 0, 0, 0, -1, 0, 0, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

		sLog->outBasic("[Fenrir Mix][Level 02] [%s][%s] CBMix Success %d Money : %d-%d, CharmRate : %d",
			Obj.AccountID, Obj.Name, Obj.ChaosSuccessRate, Obj.m_PlayerData->Money, nChaosNeedMoney,
			iCharmOfLuckCount);

	}
	else
	{
		ChaosBoxInit(lpObj);
		::gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);

		sLog->outBasic("[Fenrir Mix][Level 02] [%s][%s] CBMix Fail %d Money : %d-%d, CharmRate : %d",
			Obj.AccountID, Obj.Name, Obj.ChaosSuccessRate, Obj.m_PlayerData->Money, nChaosNeedMoney,
			iCharmOfLuckCount);

		Obj.ChaosLock = FALSE;
	}
}



void CMixSystem::Fenrir_03Level_Mix(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;
	int iStuffCount_01 = 0;
	int iLifeGemCount = 0;
	int iChaosGemCount = 0;
	int iInvalidItemCount = 0;
	DWORD iChaosMoney = 0;
	int iCharmOfLuckCount = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(13, 36))
			{
				iStuffCount_01++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 16))
			{
				iLifeGemCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 15))
			{
				iChaosGemCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 53))	// Charm Of Luck
			{
				iCharmOfLuckCount += Obj.pChaosBox[n].m_Durability;
			}
			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if (iStuffCount_01 != 1 || iLifeGemCount != 3 || iChaosGemCount != 1 || iInvalidItemCount > 0)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	if (iCharmOfLuckCount > 10)
	{
		pMsg.Result = 0xF0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	LogChaosItem(lpObj, "Fenrir_03Level_Mix");
	sLog->outBasic("[Fenrir Mix][Level 03] Chaos Mix Start");

	Obj.ChaosSuccessRate = g_iFenrir_03Level_MixRate;
	Obj.ChaosSuccessRate += iCharmOfLuckCount;
	int nChaosNeedMoney = 10000000;

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if (nChaosNeedMoney < 0)
	{
		nChaosNeedMoney = 0;
	}

	if (Obj.m_PlayerData->Money < nChaosNeedMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	Obj.m_PlayerData->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	if ((rand() % 100) < Obj.ChaosSuccessRate)
	{
		int type = ITEMGET(13, 37);
		int level = 0;
		int dur = 255;
		int op1 = 1;

		ItemCreate(Obj.m_Index, 0xFF, 0, 0, type, level, dur, op1, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

		sLog->outBasic("[Fenrir Mix][Level 03] [%s][%s] CBMix Success %d Money : %d-%d, CharmRate : %d",
			Obj.AccountID, Obj.Name, Obj.ChaosSuccessRate, Obj.m_PlayerData->Money, nChaosNeedMoney,
			iCharmOfLuckCount);

	}
	else
	{
		ChaosBoxInit(lpObj);
		::gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);

		sLog->outBasic("[Fenrir Mix][Level 03] [%s][%s] CBMix Fail %d Money : %d-%d, CharmRate : %d",
			Obj.AccountID, Obj.Name, Obj.ChaosSuccessRate, Obj.m_PlayerData->Money, nChaosNeedMoney,
			iCharmOfLuckCount);

		Obj.ChaosLock = FALSE;
	}
}

void CMixSystem::Fenrir_04Upgrade_Mix(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;
	int iStuffCount_01 = 0;
	int iLifeGemCount = 0;
	int iChaosGemCount = 0;
	int iAttackStuffCount = 0;
	int iDefendStuffCount = 0;
	int iAttackStuffPrice = 0;
	int iDefendStuffPrice = 0;
	int iInvalidItemCount = 0;
	DWORD iChaosMoney = 0;
	BOOL bFenrirDamageInc = FALSE;
	BOOL bFenrirDamageDec = FALSE;
	int iCharmOfLuckCount = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(13, 37) && Obj.pChaosBox[n].IsPeriodItem() == FALSE)
			{
				iStuffCount_01++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 16))
			{
				iLifeGemCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 15))
			{
				iChaosGemCount++;
			}
			else if (Obj.pChaosBox[n].m_Type >= ITEMGET(0, 0) &&
				Obj.pChaosBox[n].m_Type < ITEMGET(6, 0) &&
				Obj.pChaosBox[n].m_Level >= 4 &&
				Obj.pChaosBox[n].m_Option3 >= 1)
			{
				iAttackStuffCount++;
				iAttackStuffPrice += Obj.pChaosBox[n].m_BuyMoney;
			}
			else if (Obj.pChaosBox[n].m_Type >= ITEMGET(6, 0) &&
				Obj.pChaosBox[n].m_Type < ITEMGET(12, 0) &&
				Obj.pChaosBox[n].m_Level >= 4 &&
				Obj.pChaosBox[n].m_Option3 >= 1)
			{
				iDefendStuffCount++;
				iDefendStuffPrice += Obj.pChaosBox[n].m_BuyMoney;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 53))	// Charm Of Luck
			{
				iCharmOfLuckCount += Obj.pChaosBox[n].m_Durability;
			}
			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if (iStuffCount_01 != 1 || iLifeGemCount != 5 || iChaosGemCount != 1 || (iAttackStuffCount == 0 && iDefendStuffCount == 0) || iInvalidItemCount > 0)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	if (iCharmOfLuckCount > 10)
	{
		pMsg.Result = 0xF0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	if (iAttackStuffCount > 0)
	{
		bFenrirDamageInc = TRUE;
		iChaosMoney = iAttackStuffPrice;
	}

	if (iDefendStuffCount > 0)
	{
		bFenrirDamageDec = TRUE;
		iChaosMoney = iDefendStuffPrice;
	}

	if (bFenrirDamageInc && bFenrirDamageDec)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	if (!bFenrirDamageInc && !bFenrirDamageDec)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	LogChaosItem(lpObj, "Fenrir_04Level_Mix");
	sLog->outBasic("[Fenrir Mix][Level 04] Chaos Mix Start");

	int nChaosNeedMoney = 10000000;

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if (nChaosNeedMoney < 0)
	{
		nChaosNeedMoney = 0;
	}

	if (Obj.m_PlayerData->Money < nChaosNeedMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	Obj.m_PlayerData->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	if (iChaosMoney > 1000000)
		iChaosMoney = 1000000;

	Obj.ChaosSuccessRate = iChaosMoney * 100 / 1000000;

	if (Obj.ChaosSuccessRate > 79)
		Obj.ChaosSuccessRate = 79;

	Obj.ChaosSuccessRate += iCharmOfLuckCount;

	if ((rand() % 100) < Obj.ChaosSuccessRate)
	{
		int type = ITEMGET(13, 37);
		int level = 0;
		int dur = 255;
		int op1 = 1;
		int nop = 0;

		if (bFenrirDamageInc)
		{
			nop |= 1;
		}

		if (bFenrirDamageDec)
		{
			nop |= 2;
		}

		ItemCreate(Obj.m_Index, 0xFF, 0, 0, type, level, dur, op1, 0, 0, Obj.m_Index, nop, 0, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

		sLog->outBasic("[Fenrir Mix][Level 04] [%s][%s] CBMix Success %d Money : %d-%d, CharmRate : %d",
			Obj.AccountID, Obj.Name, Obj.ChaosSuccessRate, Obj.m_PlayerData->Money, nChaosNeedMoney,
			iCharmOfLuckCount);

	}
	else
	{
		ChaosBoxInit(lpObj);
		::gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);

		sLog->outBasic("[Fenrir Mix][Level 04] [%s][%s] CBMix Fail %d Money : %d-%d, CharmRate : %d",
			Obj.AccountID, Obj.Name, Obj.ChaosSuccessRate, Obj.m_PlayerData->Money, nChaosNeedMoney,
			iCharmOfLuckCount);

		Obj.ChaosLock = FALSE;
	}
}



void CMixSystem::ShieldPotionLv1_Mix(CGameObject &Obj)
{
	int iHealthPotionCount = 0;
	int iInvalidItemCount = 0;
	int iChaosMixPrice = 0;
	int iCharmOfLuckCount = 0;

	if (g_ConfigRead.g_ShieldSystemOn == FALSE)
		return;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 3))
			{
				iHealthPotionCount += Obj.pChaosBox[n].m_Durability;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 53))	// Charm Of Luck
			{
				iCharmOfLuckCount += Obj.pChaosBox[n].m_Durability;
			}
			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if (iInvalidItemCount > 0 || iHealthPotionCount != 3)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	if (iCharmOfLuckCount > 10)
	{
		pMsg.Result = 0xF0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	iChaosMixPrice = g_ConfigRead.g_iShieldPotionLv1MixMoney;
	int iChaosTaxMoney = iChaosMixPrice * g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / 100;

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	iChaosMixPrice += iChaosTaxMoney;

	if (iChaosMixPrice < 0)
	{
		iChaosMixPrice = 0;
	}

	if (Obj.m_PlayerData->Money < iChaosMixPrice)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	Obj.m_PlayerData->Money -= iChaosMixPrice;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	LogChaosItem(lpObj, "PotionMix][ShieldPotion Lv1 Mix");
	sLog->outBasic("[PotionMix][ShieldPotion Lv1 Mix] - Mix Start");

	int iRate = rand() % 100;
	iRate -= iCharmOfLuckCount;

	if (iRate < g_ConfigRead.g_iShieldPotionLv1MixSuccessRate)
	{
		int ItemNum = ITEMGET(14, 35);

		ItemCreate(Obj.m_Index, 0xFF, 0, 0, ItemNum, 0, 1, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

		sLog->outBasic("[ChaosMix][Shield Potion] Lv1 Potion Mix Success [%s][%s], Money(%d-%d), CharmRate : %d",
			Obj.AccountID, Obj.Name, Obj.m_PlayerData->Money, iChaosMixPrice, iCharmOfLuckCount);
	}
	else
	{
		ChaosBoxInit(lpObj);
		::gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);

		sLog->outBasic("[ChaosMix][Shield Potion] Lv1 Potion Mix [%s][%s] CBMix Fail %d Money : %d-%d, CharmRate : %d",
			Obj.AccountID, Obj.Name, g_ConfigRead.g_iShieldPotionLv1MixSuccessRate,
			Obj.m_PlayerData->Money, iChaosMixPrice, iCharmOfLuckCount);
	}

	Obj.ChaosLock = FALSE;
}



void CMixSystem::ShieldPotionLv2_Mix(CGameObject &Obj)
{
	int iHealthPotionCount = 0;
	int iInvalidItemCount = 0;
	int iChaosMixPrice = 0;
	int iCharmOfLuckCount = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 38))
			{
				iHealthPotionCount += Obj.pChaosBox[n].m_Durability;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 53))	// Charm Of Luck
			{
				iCharmOfLuckCount += Obj.pChaosBox[n].m_Durability;
			}
			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if (iInvalidItemCount > 0 || iHealthPotionCount != 3)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	if (iCharmOfLuckCount > 10)
	{
		pMsg.Result = 0xF0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	iChaosMixPrice = g_ConfigRead.g_iShieldPotionLv2MixMoney;
	int iChaosTaxMoney = iChaosMixPrice * g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / 100;

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	iChaosMixPrice += iChaosTaxMoney;

	if (iChaosMixPrice < 0)
	{
		iChaosMixPrice = 0;
	}

	if (Obj.m_PlayerData->Money < iChaosMixPrice)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	Obj.m_PlayerData->Money -= iChaosMixPrice;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);

	LogChaosItem(lpObj, "PotionMix][ShieldPotion Lv2 Mix");
	sLog->outBasic("[PotionMix][ShieldPotion Lv2 Mix] - Mix Start");

	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);
	int iRate = rand() % 100;
	iRate -= iCharmOfLuckCount;

	if (iRate < g_ConfigRead.g_iShieldPotionLv2MixSuccessRate)
	{
		int ItemNum = ITEMGET(14, 36);

		ItemCreate(Obj.m_Index, 0xFF, 0, 0, ItemNum, 0, 1, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

		sLog->outBasic("[ChaosMix][Shield Potion] Lv2 Potion Mix Success [%s][%s], Money(%d-%d), CharmRate : %d",
			Obj.AccountID, Obj.Name, Obj.m_PlayerData->Money, iChaosMixPrice, iCharmOfLuckCount);
	}
	else
	{
		ChaosBoxInit(lpObj);
		::gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);

		sLog->outBasic("[ChaosMix][Shield Potion] Lv2 Potion Mix [%s][%s] CBMix Fail %d Money : %d-%d, CharmRate : %d",
			Obj.AccountID, Obj.Name, g_ConfigRead.g_iShieldPotionLv2MixSuccessRate,
			Obj.m_PlayerData->Money, iChaosMixPrice, iCharmOfLuckCount);
	}

	Obj.ChaosLock = FALSE;
}

void CMixSystem::ShieldPotionLv3_Mix(CGameObject &Obj)
{
	int iHealthPotionCount = 0;
	int iInvalidItemCount = 0;
	int iChaosMixPrice = 0;
	int iCharmOfLuckCount = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 39))
			{
				iHealthPotionCount += Obj.pChaosBox[n].m_Durability;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 53))	// Charm Of Luck
			{
				iCharmOfLuckCount += Obj.pChaosBox[n].m_Durability;
			}
			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if (iInvalidItemCount > 0 || iHealthPotionCount != 3)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	if (iCharmOfLuckCount > 10)
	{
		pMsg.Result = 0xF0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	iChaosMixPrice = g_ConfigRead.g_iShieldPotionLv3MixMoney;
	int iChaosTaxMoney = iChaosMixPrice * g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / 100;

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	iChaosMixPrice += iChaosTaxMoney;

	if (iChaosMixPrice < 0)
	{
		iChaosMixPrice = 0;
	}

	if (Obj.m_PlayerData->Money < iChaosMixPrice)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	Obj.m_PlayerData->Money -= iChaosMixPrice;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	LogChaosItem(lpObj, "PotionMix][ShieldPotion Lv3 Mix");
	sLog->outBasic("[PotionMix][ShieldPotion Lv3 Mix] - Mix Start");

	int iRate = rand() % 100;
	iRate -= iCharmOfLuckCount;

	if (iRate < g_ConfigRead.g_iShieldPotionLv3MixSuccessRate)
	{
		int ItemNum = ITEMGET(14, 37);

		ItemCreate(Obj.m_Index, 0xFF, 0, 0, ItemNum, 0, 1, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

		sLog->outBasic("[ChaosMix][Shield Potion] Lv3 Potion Mix Success [%s][%s], Money(%d-%d), CharmRate : %d",
			Obj.AccountID, Obj.Name, Obj.m_PlayerData->Money, iChaosMixPrice, iCharmOfLuckCount);
	}
	else
	{
		ChaosBoxInit(lpObj);
		::gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);

		sLog->outBasic("[ChaosMix][Shield Potion] Lv3 Potion Mix [%s][%s] CBMix Fail %d Money : %d-%d, CharmRate : %d",
			Obj.AccountID, Obj.Name, g_ConfigRead.g_iShieldPotionLv3MixSuccessRate,
			Obj.m_PlayerData->Money, iChaosMixPrice, iCharmOfLuckCount);
	}

	Obj.ChaosLock = FALSE;
}



void CMixSystem::LotteryItemMix(CGameObject &Obj, int type)
{
	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	int ChaosCardCount = 0;
	int ChaosCardGoldCount = 0;
	int ChaosCardRareCount = 0;
	int ChaosCardMiniCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 54))
		{
			type = 1;
			ChaosCardCount++;
		}
		else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 92))
		{
			type = 2;
			ChaosCardGoldCount++;
		}
		else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 93))
		{
			type = 3;
			ChaosCardRareCount++;
		}
		else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 95))
		{
			type = 4;
			ChaosCardMiniCount++;
		}
	}

	if (type == 0 || (type == 1 && ChaosCardCount != 1) || (type == 2 && ChaosCardGoldCount != 1) || (type == 3 && ChaosCardRareCount != 1) || (type == 4 && ChaosCardMiniCount != 1))
	{
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	CItemObject LotteryItem;

	if (type == 1)
	{
		g_Lottery.GetItem("ChaosCard", &LotteryItem);
	}
	else if (type == 2)
	{
		g_Lottery.GetItem("ChaosCardGold", &LotteryItem);
	}
	else if (type == 3)
	{
		g_Lottery.GetItem("ChaosCardRare", &LotteryItem);
	}
	else if (type == 4)
	{
		g_Lottery.GetItem("ChaosCardMini", &LotteryItem);
	}

	/*if (type == 1)
	{
		if (g_ChaosCard.GetItem(&LotteryItem) == -1)
		{
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
			Obj.ChaosLock = FALSE;
			return;
		}
	}
	else if (type == 2)
	{
		if (g_ChaosCardGold.GetItem(&LotteryItem) == -1)
		{
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
			Obj.ChaosLock = FALSE;
			return;
		}
	}
	else if (type == 3)
	{
		if (g_ChaosCardRare.GetItem(&LotteryItem) == -1)
		{
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
			Obj.ChaosLock = FALSE;
			return;
		}
	}
	else if (type == 4)
	{
		if (g_ChaosCardMini.GetItem(&LotteryItem) == -1)
		{
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
			Obj.ChaosLock = FALSE;
			return;
		}
	}*/

	ItemCreate(Obj.m_Index, 255, 0, 0, LotteryItem.m_Type, LotteryItem.m_Level, LotteryItem.m_Durability,
		LotteryItem.m_Option1, LotteryItem.m_Option2, LotteryItem.m_Option3, Obj.m_Index, LotteryItem.m_NewOption, 0, 0, 0, 0);

	gObjInventoryCommit(Obj.m_Index);

	BYTE btExOption[MAX_EXOPTION_SIZE];

	ItemIsBufExOption(btExOption, &LotteryItem);
}

BOOL CMixSystem::SeedExtractMixS12(CGameObject &Obj, BYTE AncientPos, BYTE ExePos, BYTE JOHPos, BYTE JOCPos, BYTE ChaosPos)
{
	//
	CItemObject * lpAncientItem = NULL;
	CItemObject * lpExeItem = NULL;
	CItemObject * lpJohItem = NULL;
	CItemObject * lpJoCItemObject = NULL;
	CItemObject * lpChaosItem = NULL;
	int isChaos = 0;
	int isSeed = 0;
	int isSphere = 0;
	int isJoc = 0;
	int isJoh = 0;

	int isFire = 0;
	int isWater = 0;
	int isIce = 0;
	int isLighting = 0;
	int isWind = 0;
	int isEarth = 0;
	BOOL cont = false;
	lpAncientItem = &Obj.pInventory[AncientPos];
	lpExeItem = &Obj.pInventory[ExePos];
	lpJohItem = &Obj.pInventory[JOHPos];
	lpJoCItemObject = &Obj.pInventory[JOCPos];
	lpChaosItem = &Obj.pInventory[ChaosPos];

	PMSG_SEEDRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg.h, 0x78, sizeof(PMSG_SEEDRESULT));
	pMsg.Result = 0x01;
	pMsg.Result1 = 0x00; //Fail
	if (lpChaosItem->m_Type == ITEMGET(12, 15))
	{
		isChaos++;
	}
	if (lpJoCItemObject->m_Type == ITEMGET(14, 22))
	{
		isJoc++;
	}
	if (lpJohItem->m_Type == ITEMGET(14, 42))
	{
		isJoh++;
	}

	if ((lpAncientItem->IsSetItem() != FALSE) && (!lpAncientItem->m_Level >= 4))
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	if ((lpAncientItem->IsExtItem() != FALSE) && (!lpAncientItem->m_Level >= 4))
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	if (isChaos != 1 && isJoc != 1 && isJoh != 1)
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	int iChaosTaxMoney = (int)((__int64)g_SocketOptionSystem.m_RequireMoneyForSeedExtract * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	int iMixMoney = g_SocketOptionSystem.m_RequireMoneyForSeedExtract + iChaosTaxMoney;

	if (Obj.m_PlayerData->Money < iMixMoney)
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.m_PlayerData->Money -= iMixMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);
	BYTE SocketOptionIndex = 0;
	SocketOptionIndex = g_SocketOptionSystem.GetExtractSeed();
	CSeedItem * lpSeedData = g_SocketOptionSystem.GetSeedData(SocketOptionIndex);

	if (lpSeedData != NULL)
	{
		::gObjInventoryItemSet(Obj.m_Index, AncientPos, -1);
		::Obj.pInventory[AncientPos]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, AncientPos, 1);

		::gObjInventoryItemSet(Obj.m_Index, ExePos, -1);
		::Obj.pInventory[ExePos]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, ExePos, 1);

		::gObjInventoryItemSet(Obj.m_Index, JOHPos, -1);
		::Obj.pInventory[JOHPos]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JOHPos, 1);

		::gObjInventoryItemSet(Obj.m_Index, JOCPos, -1);
		::Obj.pInventory[JOCPos]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JOCPos, 1);

		::gObjInventoryItemSet(Obj.m_Index, ChaosPos, -1);
		::Obj.pInventory[ChaosPos]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, ChaosPos, 1);

		ItemCreate(Obj.m_Index, 235, 0, 0, lpSeedData->m_ItemCode, lpSeedData->m_ItemLevel, 0, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
		gObjInventoryCommit(Obj.m_Index);
		pMsg.Result1 = 0x01;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return 1;
		sLog->outBasic("[%s][%s] Seed Extract Mix Success - SeedInfo ( Index : %d, Type : %d, Level : %d, ItemCode : %d )",
			Obj.AccountID, Obj.Name, lpSeedData->m_SeedIndex, lpSeedData->m_SeedType, lpSeedData->m_ItemLevel, lpSeedData->m_ItemCode);
	}
}

void CMixSystem::SeedExtractMix(CGameObject &Obj)
{
	int iExCItemObjectCount = 0;
	int iSetItemCount = 0;
	int iHarmonyCount = 0;
	int iChoasGemCount = 0;
	int iCreationGemCount = 0;
	int iInvalidItemCount = 0;
	int iSeedExtractRate = 0;
	DWORD iChaosMoney = 0;
	int iMixMoney = 0;
	int iChaosTaxMoney = 0;
	int iSetItemNum = 0;
	int iExCItemObjectNum = 0;
	int iHarmonyItemNum = 0;
	int iChoasGemItemNum = 0;
	int iCreationItemNum = 0;

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;
	Obj.ChaosLock = TRUE;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].IsExtItem() == TRUE && Obj.pChaosBox[n].m_Level >= 4)
			{
				iExCItemObjectCount++;
				iChaosMoney += Obj.pChaosBox[n].m_BuyMoney;
				iExCItemObjectNum = n;
			}
			else if (Obj.pChaosBox[n].IsSetItem() != FALSE && Obj.pChaosBox[n].m_Level >= 4)
			{
				iSetItemCount++;
				iChaosMoney += Obj.pChaosBox[n].m_BuyMoney;
				iSetItemNum = n;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 42))
			{
				iHarmonyCount++;
				iHarmonyItemNum = n;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 15))
			{
				iChoasGemCount++;
				iChoasGemItemNum = n;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 22))
			{
				iCreationGemCount++;
				iCreationItemNum = n;
			}
			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if (iInvalidItemCount > 0)
	{
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	if (iExCItemObjectCount != 1 || iSetItemCount != 1 || iHarmonyCount != 1 || iChoasGemCount != 1 || iCreationGemCount != 1)
	{
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	iChaosTaxMoney = (int)((__int64)g_SocketOptionSystem.m_RequireMoneyForSeedExtract * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	iMixMoney = g_SocketOptionSystem.m_RequireMoneyForSeedExtract + iChaosTaxMoney;

	if (Obj.m_PlayerData->Money < iMixMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	LogChaosItem(lpObj, "SeedExtract");
	Obj.m_PlayerData->Money -= iMixMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	iSeedExtractRate = 80 + (iChaosMoney / 2000000);

	if (iSeedExtractRate > 90)
	{
		iSeedExtractRate = 90;
	}

	if (rand() % 100 > iSeedExtractRate)
	{
		Obj.pChaosBox[iSetItemNum].m_Level -= rand() % 3;
		Obj.pChaosBox[iExCItemObjectNum].m_Level -= rand() % 3;
		Obj.pChaosBox[iHarmonyItemNum].Clear();
		Obj.pChaosBox[iChoasGemItemNum].Clear();
		Obj.pChaosBox[iCreationItemNum].Clear();
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		sLog->outBasic("[%s][%s] Seed Extract Mix Failed.", Obj.AccountID, Obj.Name);
		return;
	}

	BYTE SocketOptionIndex = 0;
	SocketOptionIndex = g_SocketOptionSystem.GetExtractSeed();
	CSeedItem * lpSeedData = g_SocketOptionSystem.GetSeedData(SocketOptionIndex);

	if (lpSeedData != NULL)
	{
		pMsg.Result = 1;
		ItemCreate(Obj.m_Index, 0xFF, 0, 0, lpSeedData->m_ItemCode, lpSeedData->m_ItemLevel, 1, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
		gObjInventoryCommit(Obj.m_Index);
		sLog->outBasic("[%s][%s] Seed Extract Mix Success - SeedInfo ( Index : %d, Type : %d, Level : %d, ItemCode : %d )",
			Obj.AccountID, Obj.Name, lpSeedData->m_SeedIndex, lpSeedData->m_SeedType, lpSeedData->m_ItemLevel, lpSeedData->m_ItemCode);
	}
	else
	{
		Obj.pChaosBox[iSetItemNum].m_Level -= rand() % 3;
		Obj.pChaosBox[iExCItemObjectNum].m_Level -= rand() % 3;
		Obj.pChaosBox[iHarmonyItemNum].Clear();
		Obj.pChaosBox[iChoasGemItemNum].Clear();
		Obj.pChaosBox[iCreationItemNum].Clear();
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		sLog->outBasic("[%s][%s] Seed Extract Mix Failed - SeedData is NULL", Obj.AccountID, Obj.Name);
		Obj.ChaosLock = FALSE;
	}
}

BOOL CMixSystem::SeedSphereEnhance(CGameObject &Obj, BYTE Sphere1, BYTE Sphere2, BYTE Rune)
{
	CItemObject * lpSphere1 = NULL;
	CItemObject * lpSphere2 = NULL;
	CItemObject lpNewSphere;
	CItemObject * lpRune = NULL;
	int Item = 0;
	int ReqZen = 0;
	int ReqRune = 0;
	lpSphere1 = &Obj.pInventory[Sphere1];
	lpSphere2 = &Obj.pInventory[Sphere2];
	lpRune = &Obj.pInventory[Rune];
	CopyMemory(&lpNewSphere, &lpSphere1, sizeof(lpSphere1));

	lpNewSphere.m_Type = lpSphere1->m_Type + 6;
	Item = LOBYTE(lpNewSphere.m_Type);
	Item = Item - 6;

	if ((Item >= 124) && (Item <= 129))
	{
		lpNewSphere.m_Type = lpSphere1->m_Type + 187;
	}

	if ((Item >= 100) && (Item <= 105))
	{
		ReqZen = 10000000;
		ReqRune = 4;
	}
	else if ((Item >= 106) && (Item <= 111))
	{
		ReqZen = 13000000;
		ReqRune = 5;
	}
	else if ((Item >= 112) && (Item <= 117))
	{
		ReqZen = 16000000;
		ReqRune = 5;
	}
	else if ((Item >= 118) && (Item <= 123))
	{
		ReqZen = 19000000;
		ReqRune = 5;
	}
	else if ((Item >= 124) && (Item <= 129))
	{
		ReqZen = 22000000;
		ReqRune = 6;
	}
	else if ((Item >= 311) && (Item <= 316))
	{
		ReqZen = 25000000;
		ReqRune = 7;
	}
	else if ((Item >= 317) && (Item <= 322))
	{
		ReqZen = 28000000;
		ReqRune = 8;
	}
	else if ((Item >= 323) && (Item <= 328))
	{
		ReqZen = 31000000;
		ReqRune = 9;
	}
	else if ((Item >= 329) && (Item <= 334))
	{
		ReqZen = 34000000;
		ReqRune = 10;
	}

	if (!lpRune->m_Type == ITEMGET(12, 401))
	{
		return FALSE;
	}

	if (!gObjSearchItemMinus(&getGameObject(Obj.m_Index), Rune, ReqRune))
	{
		::gObjInventoryItemSet(Obj.m_Index, Rune, -1);
		::Obj.pInventory[Rune]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, Rune, 1);
	}

	if (Obj.m_PlayerData->Money <= ReqZen)
	{
		return FALSE; //No Zen
	}
	//if ((Item > 129) && (Item < 310))
	//lpNewSphere.m_Type = lpNewSphere.m_Type + 181;

	lpNewSphere.m_Level = lpSphere1->m_Level;
	lpNewSphere.m_Option1 = lpSphere1->m_Option1;
	lpNewSphere.m_Option2 = lpSphere1->m_Option2;
	lpNewSphere.m_Option3 = lpSphere1->m_Option3;
	lpNewSphere.m_NewOption = lpSphere1->m_NewOption;
	BYTE SocketOption[5];

	if ((rand() % 100) < g_ConfigRead.SeedSphereEnhance)
	{
		for (int i = 0; i < 5; i++)
		{
			SocketOption[i] = 0xFF;
		}
		Obj.m_PlayerData->Money -= ReqZen;
		gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);
		ItemCreate(Obj.m_Index, 235, 0, 0, lpNewSphere.m_Type, lpNewSphere.m_Level, 0, lpNewSphere.m_Option1, lpNewSphere.m_Option2, lpNewSphere.m_Option3, Obj.m_Index, lpNewSphere.m_NewOption, 0, 0, SocketOption, 0);

		gObjInventoryCommit(Obj.m_Index);
		::gObjInventoryItemSet(Obj.m_Index, Sphere1, -1);
		::Obj.pInventory[Sphere1]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, Sphere1, 1);
		::gObjInventoryItemSet(Obj.m_Index, Sphere2, -1);
		::Obj.pInventory[Sphere2]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, Sphere2, 1);
		return TRUE;
	}
	else
	{
		Obj.m_PlayerData->Money -= ReqZen;
		gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);
		gObjInventoryCommit(Obj.m_Index);
		::gObjInventoryItemSet(Obj.m_Index, Sphere2, -1);
		::Obj.pInventory[Sphere2]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, Sphere2, 1);
		return FALSE;
	}
}

void CMixSystem::SeedSphereRemoveMixS12(CGameObject &Obj, BYTE ItemPos, BYTE Slot, BYTE JogPos, BYTE JohPos1, BYTE JohPos2, BYTE JohPos3, BYTE JohPos4, BYTE JohPos5, BYTE ChaosPos1, BYTE ChaosPos2, BYTE ChaosPos3, BYTE ChaosPos4, BYTE ChaosPos5)
{
	CItemObject * lpSocketItem = NULL;
	CItemObject * lpJog = NULL;
	CItemObject * lpJoh1 = NULL;
	CItemObject * lpJoh2 = NULL;
	CItemObject * lpJoh3 = NULL;
	CItemObject * lpJoh4 = NULL;
	CItemObject * lpJoh5 = NULL;

	CItemObject * lpChaos1 = NULL;
	CItemObject * lpChaos2 = NULL;
	CItemObject * lpChaos3 = NULL;
	CItemObject * lpChaos4 = NULL;
	CItemObject * lpChaos5 = NULL;

	int Item = 0;
	int ReqZen = 0;
	int ReqRune = 0;
	lpSocketItem = &Obj.pInventory[ItemPos];
	lpJog = &Obj.pInventory[JogPos];

	lpJoh1 = &Obj.pInventory[JohPos1];
	lpJoh2 = &Obj.pInventory[JohPos2];
	lpJoh3 = &Obj.pInventory[JohPos3];
	lpJoh4 = &Obj.pInventory[JohPos4];
	lpJoh5 = &Obj.pInventory[JohPos5];

	lpChaos1 = &Obj.pInventory[ChaosPos1];
	lpChaos2 = &Obj.pInventory[ChaosPos2];
	lpChaos3 = &Obj.pInventory[ChaosPos3];
	lpChaos4 = &Obj.pInventory[ChaosPos4];
	lpChaos5 = &Obj.pInventory[ChaosPos5];

	PMSG_SEEDRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg.h, 0x78, sizeof(PMSG_SEEDRESULT));
	pMsg.Result = 0x01;
	pMsg.Result1 = 0x00; //Fail

	int iSocketItemCount = 0; //2
	int iInvalidItemCount = 0; //3
	int iSeedSphereItemCount = 0;
	int iSocketItemPosition = 0;
	int iSeedSpherePosition = 0;

	int iMixMoney = 0; //7
	int iChaosTaxMoney = 0; //8

	int iChaosJewelCount = 0;
	int iJewelOfHarmonyCount = 0;
	int iGemOfDefendCount = 0;

	int iChaosJewelPosition[5];
	int iJewelOfHarmonyPosition[5];

	int iGemOfDefendPosition = -1;

	iChaosJewelPosition[0] = -1;
	iChaosJewelPosition[1] = -1;
	iChaosJewelPosition[2] = -1;
	iChaosJewelPosition[3] = -1;
	iChaosJewelPosition[4] = -1;

	iJewelOfHarmonyPosition[0] = -1;
	iJewelOfHarmonyPosition[1] = -1;
	iJewelOfHarmonyPosition[2] = -1;
	iJewelOfHarmonyPosition[3] = -1;
	iJewelOfHarmonyPosition[4] = -1;


	if (Slot >= 5)
	{
		return;
	}

	if (lpSocketItem->IsItem() == TRUE)
	{
		if (gSocketItemType.CheckSocketItemType(lpSocketItem->m_Type) == 1)
		{
			if (lpSocketItem->m_SocketOption[Slot] != 0xFE && lpSocketItem->m_SocketOption[Slot] != 0xFF) //Slot with OPTION
			{
				iSocketItemCount++; //
			}
		}

		if (lpChaos1->m_Type == ITEMGET(12, 15))
			iChaosJewelCount++;
		if (lpChaos2->m_Type == ITEMGET(12, 15))
			iChaosJewelCount++;
		if (lpChaos3->m_Type == ITEMGET(12, 15))
			iChaosJewelCount++;
		if (lpChaos4->m_Type == ITEMGET(12, 15))
			iChaosJewelCount++;
		if (lpChaos5->m_Type == ITEMGET(12, 15))
			iChaosJewelCount++;

		if (lpJog->m_Type == ITEMGET(14, 31))
			iGemOfDefendCount++;

		if (lpJoh1->m_Type == ITEMGET(14, 42))
			iJewelOfHarmonyCount++;
		if (lpJoh2->m_Type == ITEMGET(14, 42))
			iJewelOfHarmonyCount++;
		if (lpJoh3->m_Type == ITEMGET(14, 42))
			iJewelOfHarmonyCount++;
		if (lpJoh4->m_Type == ITEMGET(14, 42))
			iJewelOfHarmonyCount++;
		if (lpJoh5->m_Type == ITEMGET(14, 42))
			iJewelOfHarmonyCount++;
	}

	if (iSocketItemCount != 1)
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (iChaosJewelCount != 5 || iJewelOfHarmonyCount != 5 || iGemOfDefendCount != 1)
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	iChaosTaxMoney = (int)((__int64)g_SocketOptionSystem.m_RequireMoneyForSeedSphereRemove * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	iMixMoney = g_SocketOptionSystem.m_RequireMoneyForSeedSphereRemove + iChaosTaxMoney;

	if (Obj.m_PlayerData->Money < iMixMoney)
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	BYTE btSocketOption = lpSocketItem->m_SocketOption[Slot];
	BYTE btSeedIndex = btSocketOption % 50;
	BYTE btSphereLevel = (btSocketOption - btSeedIndex) / 50 + 1;
	CSeedSphereItem SeedSphereItem;
	BOOL MakeSeedSphere = g_SocketOptionSystem.MakeSeedSphere(&SeedSphereItem, btSeedIndex, btSphereLevel);
	if (MakeSeedSphere != TRUE)
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}
	Obj.m_PlayerData->Money -= iMixMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	if (rand() % 100 < g_ConfigRead.SeedSphereRemove)
	{
		g_SocketOptionSystem.ClearSocketSlot(lpSocketItem, Slot);
		ItemCreate(Obj.m_Index, 235, 0, 0, SeedSphereItem.m_ItemCode, SeedSphereItem.m_ItemLevel, 0, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
	}
	else
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	int levelitemdur = ItemGetDurability(lpSocketItem->m_Type, lpSocketItem->m_Level, lpSocketItem->IsExtItem(), lpSocketItem->IsSetItem());

	CItemObject NewItem;
	NewItem.m_Number = lpSocketItem->m_Number;
	NewItem.m_Level = lpSocketItem->m_Level;
	NewItem.m_Durability = levelitemdur * lpSocketItem->m_Durability / lpSocketItem->m_BaseDurability;
	NewItem.m_JewelOfHarmonyOption = lpSocketItem->m_JewelOfHarmonyOption;

	NewItem.Convert(lpSocketItem->m_Type, lpSocketItem->m_Option1, lpSocketItem->m_Option2, lpSocketItem->m_Option3,
		lpSocketItem->m_NewOption, lpSocketItem->m_SetOption, lpSocketItem->m_ItemOptionEx,
		lpSocketItem->m_SocketOption, lpSocketItem->m_BonusSocketOption, 0, 3);

	::gObjInventoryItemSet(Obj.m_Index, ItemPos, -1);
	::Obj.pInventory[ItemPos]->Clear();
	::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, ItemPos, 1);

	::gObjInventoryItemSet(Obj.m_Index, JogPos, -1);
	::Obj.pInventory[JogPos]->Clear();
	::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JogPos, 1);

	::gObjInventoryItemSet(Obj.m_Index, JohPos1, -1);
	::Obj.pInventory[JohPos1]->Clear();
	::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JohPos1, 1);
	::gObjInventoryItemSet(Obj.m_Index, JohPos2, -1);
	::Obj.pInventory[JohPos2]->Clear();
	::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JohPos2, 1);
	::gObjInventoryItemSet(Obj.m_Index, JohPos3, -1);
	::Obj.pInventory[JohPos3]->Clear();
	::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JohPos3, 1);
	::gObjInventoryItemSet(Obj.m_Index, JohPos4, -1);
	::Obj.pInventory[JohPos4]->Clear();
	::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JohPos4, 1);
	::gObjInventoryItemSet(Obj.m_Index, JohPos5, -1);
	::Obj.pInventory[JohPos5]->Clear();
	::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JohPos5, 1);

	::gObjInventoryItemSet(Obj.m_Index, ChaosPos1, -1);
	::Obj.pInventory[ChaosPos1]->Clear();
	::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, ChaosPos1, 1);
	::gObjInventoryItemSet(Obj.m_Index, ChaosPos2, -1);
	::Obj.pInventory[ChaosPos2]->Clear();
	::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, ChaosPos2, 1);
	::gObjInventoryItemSet(Obj.m_Index, ChaosPos3, -1);
	::Obj.pInventory[ChaosPos3]->Clear();
	::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, ChaosPos3, 1);
	::gObjInventoryItemSet(Obj.m_Index, ChaosPos4, -1);
	::Obj.pInventory[ChaosPos4]->Clear();
	::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, ChaosPos4, 1);
	::gObjInventoryItemSet(Obj.m_Index, ChaosPos5, -1);
	::Obj.pInventory[ChaosPos5]->Clear();
	::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, ChaosPos5, 1);
	ItemCreate(Obj.m_Index, 235, 0, 0, NewItem.m_Type, NewItem.m_Level, 0, NewItem.m_Option1, NewItem.m_Option2, NewItem.m_Option3, Obj.m_Index, NewItem.m_NewOption, 0, 0, NewItem.m_SocketOption, NewItem.m_BonusSocketOption);
	pMsg.Result1 = 1;
	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
	sLog->outBasic("[%s][%s] SeedSphere Remove Success - ItemInfo ( Name : %s, ItemCode : %d, Level : %d, SocketOption[%d,%d,%d,%d,%d], BonusOption : %d )",
		Obj.AccountID, Obj.Name, ItemAttribute[lpSocketItem->m_Type].Name, lpSocketItem->m_Type,
		lpSocketItem->m_Level, lpSocketItem->m_SocketOption[0], lpSocketItem->m_SocketOption[1],
		lpSocketItem->m_SocketOption[2], lpSocketItem->m_SocketOption[3], lpSocketItem->m_SocketOption[4], lpSocketItem->m_BonusSocketOption);

	//IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}

BOOL CMixSystem::SocketItemUpgradeMixS12(CGameObject &Obj, BYTE ItemPos, BYTE SocketUpgradeNotePos, BYTE JoBlessPos1, BYTE JoBlessPos2, BYTE JoBlessPos3, BYTE JoBlessPos4, BYTE JoBlessPos5, BYTE JoSoulPos1, BYTE JoSoulPos2, BYTE JoSoulPos3, BYTE JoSoulPos4, BYTE JoSoulPos5, BYTE JoChaosPos)
{
	CItemObject * IsItem = NULL;
	CItemObject * lpUpgradeNoteItem = NULL;
	CItemObject * JewelOfBless1 = NULL;
	CItemObject * JewelOfBless2 = NULL;
	CItemObject * JewelOfBless3 = NULL;
	CItemObject * JewelOfBless4 = NULL;
	CItemObject * JewelOfBless5 = NULL;
	CItemObject * JewelOfSoul1 = NULL;
	CItemObject * JewelOfSoul2 = NULL;
	CItemObject * JewelOfSoul3 = NULL;
	CItemObject * JewelOfSoul4 = NULL;
	CItemObject * JewelOfSoul5 = NULL;
	CItemObject * lpChaosItem = NULL;

	int isChaos = 0;
	int isJoSoul = 0;
	int isJoBless = 0;
	int HelmType = 0;
	int SocketUpgNote = 0;

	IsItem = &Obj.pInventory[ItemPos];
	lpUpgradeNoteItem = &Obj.pInventory[SocketUpgradeNotePos];
	JewelOfBless1 = &Obj.pInventory[JoBlessPos1];
	JewelOfBless2 = &Obj.pInventory[JoBlessPos2];
	JewelOfBless3 = &Obj.pInventory[JoBlessPos3];
	JewelOfBless4 = &Obj.pInventory[JoBlessPos4];
	JewelOfBless5 = &Obj.pInventory[JoBlessPos5];

	JewelOfSoul1 = &Obj.pInventory[JoSoulPos1];
	JewelOfSoul2 = &Obj.pInventory[JoSoulPos2];
	JewelOfSoul3 = &Obj.pInventory[JoSoulPos3];
	JewelOfSoul4 = &Obj.pInventory[JoSoulPos4];
	JewelOfSoul5 = &Obj.pInventory[JoSoulPos5];

	lpChaosItem = &Obj.pInventory[JoChaosPos];

	PMSG_SEEDRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg.h, 0x78, sizeof(PMSG_SEEDRESULT));
	pMsg.Result = 0x01;
	pMsg.Result1 = 0x00; //Fail

	if (lpChaosItem->m_Type == ITEMGET(12, 15))
		isChaos++;

	if (JewelOfBless1->m_Type == ITEMGET(14, 13))
		isJoBless++;
	if (JewelOfBless2->m_Type == ITEMGET(14, 13))
		isJoBless++;
	if (JewelOfBless3->m_Type == ITEMGET(14, 13))
		isJoBless++;
	if (JewelOfBless4->m_Type == ITEMGET(14, 13))
		isJoBless++;
	if (JewelOfBless5->m_Type == ITEMGET(14, 13))
		isJoBless++;

	if (JewelOfSoul1->m_Type == ITEMGET(14, 14))
		isJoSoul++;
	if (JewelOfSoul2->m_Type == ITEMGET(14, 14))
		isJoSoul++;
	if (JewelOfSoul3->m_Type == ITEMGET(14, 14))
		isJoSoul++;
	if (JewelOfSoul4->m_Type == ITEMGET(14, 14))
		isJoSoul++;
	if (JewelOfSoul5->m_Type == ITEMGET(14, 14))
		isJoSoul++;

	if (lpUpgradeNoteItem->m_Type == ITEMGET(14, 364))
		SocketUpgNote++;

	if(gSocketItemType.CheckSocketItemType(IsItem->m_Type) == TRUE);
	HelmType++;
	

	if ((SocketUpgNote != 1) && (isJoSoul != 5) && (isJoBless != 5) && (isChaos != 1))
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	if (Obj.m_PlayerData->Money <= g_ConfigRead.IsSocketItemUpgradeMixZen)
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0; //No Zen
	}

	Obj.m_PlayerData->Money -= g_ConfigRead.IsSocketItemUpgradeMixZen;
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	if (rand() % 100 < g_ConfigRead.IsSocketItemUpgradeMix)
	{
		int NewItemType = 0;
		// start set
		if (IsItem->m_Type == ITEMGET(7, 45))
		{
			NewItemType = IsItem->m_Type + 29;
		}
		else if (IsItem->m_Type == ITEMGET(8, 45))
		{
			NewItemType = IsItem->m_Type + 29;
		}
		else if (IsItem->m_Type == ITEMGET(9, 45))
		{
			NewItemType = IsItem->m_Type + 29;
		}
		else if (IsItem->m_Type == ITEMGET(10, 45))
		{
			NewItemType = IsItem->m_Type + 29;
		}
		else if (IsItem->m_Type == ITEMGET(11, 45))
		{
			NewItemType = IsItem->m_Type + 29;
		}
		else if (IsItem->m_Type == ITEMGET(7, 46))
		{
			NewItemType = IsItem->m_Type + 8;
		}
		else if (IsItem->m_Type == ITEMGET(8, 46))
		{
			NewItemType = IsItem->m_Type + 8;
		}
		else if (IsItem->m_Type == ITEMGET(9, 46))
		{
			NewItemType = IsItem->m_Type + 8;
		}
		else if (IsItem->m_Type == ITEMGET(10, 46))
		{
			NewItemType = IsItem->m_Type + 8;
		}
		else if (IsItem->m_Type == ITEMGET(11, 46))
		{
			NewItemType = IsItem->m_Type + 8;
		}
		else if (IsItem->m_Type == ITEMGET(8, 47))
		{
			NewItemType = IsItem->m_Type + 10;
		}
		else if (IsItem->m_Type == ITEMGET(9, 47))
		{
			NewItemType = IsItem->m_Type + 10;
		}
		else if (IsItem->m_Type == ITEMGET(10, 47))
		{
			NewItemType = IsItem->m_Type + 10;
		}
		else if (IsItem->m_Type == ITEMGET(11, 47))
		{
			NewItemType = IsItem->m_Type + 10;
		}
		else if (IsItem->m_Type == ITEMGET(8, 48))
		{
			NewItemType = IsItem->m_Type + 28;
		}
		else if (IsItem->m_Type == ITEMGET(9, 48))
		{
			NewItemType = IsItem->m_Type + 28;
		}
		else if (IsItem->m_Type == ITEMGET(10, 48))
		{
			NewItemType = IsItem->m_Type + 28;
		}
		else if (IsItem->m_Type == ITEMGET(11, 48))
		{
			NewItemType = IsItem->m_Type + 28;
		}
		else if (IsItem->m_Type == ITEMGET(7, 49))
		{
			NewItemType = IsItem->m_Type + 7;
		}
		else if (IsItem->m_Type == ITEMGET(8, 49))
		{
			NewItemType = IsItem->m_Type + 7;
		}
		else if (IsItem->m_Type == ITEMGET(9, 49))
		{
			NewItemType = IsItem->m_Type + 7;
		}
		else if (IsItem->m_Type == ITEMGET(10, 49))
		{
			NewItemType = IsItem->m_Type + 7;
		}
		else if (IsItem->m_Type == ITEMGET(11, 49))
		{
			NewItemType = IsItem->m_Type + 7;
		}
		else if (IsItem->m_Type == ITEMGET(7, 50))
		{
			NewItemType = IsItem->m_Type + 25;
		}
		else if (IsItem->m_Type == ITEMGET(8, 50))
		{
			NewItemType = IsItem->m_Type + 25;
		}
		else if (IsItem->m_Type == ITEMGET(9, 50))
		{
			NewItemType = IsItem->m_Type + 25;
		}
		else if (IsItem->m_Type == ITEMGET(10, 50))
		{
			NewItemType = IsItem->m_Type + 25;
		}
		else if (IsItem->m_Type == ITEMGET(11, 50))
		{
			NewItemType = IsItem->m_Type + 25;
		}
		else if (IsItem->m_Type == ITEMGET(7, 51))
		{
			NewItemType = IsItem->m_Type + 7;
		}
		else if (IsItem->m_Type == ITEMGET(8, 51))
		{
			NewItemType = IsItem->m_Type + 7;
		}
		else if (IsItem->m_Type == ITEMGET(9, 51))
		{
			NewItemType = IsItem->m_Type + 7;
		}
		else if (IsItem->m_Type == ITEMGET(10, 51))
		{
			NewItemType = IsItem->m_Type + 7;
		}
		else if (IsItem->m_Type == ITEMGET(11, 51))
		{
			NewItemType = IsItem->m_Type + 7;
		}
		else if (IsItem->m_Type == ITEMGET(7, 52))
		{
			NewItemType = IsItem->m_Type + 3;
		}
		else if (IsItem->m_Type == ITEMGET(8, 52))
		{
			NewItemType = IsItem->m_Type + 3;
		}
		else if (IsItem->m_Type == ITEMGET(9, 52))
		{
			NewItemType = IsItem->m_Type + 3;
		}
		else if (IsItem->m_Type == ITEMGET(10, 52))
		{
			NewItemType = IsItem->m_Type + 3;
		}
		else if (IsItem->m_Type == ITEMGET(11, 52))
		{
			NewItemType = IsItem->m_Type + 3;
		}
		else if (IsItem->m_Type == ITEMGET(7, 146))
		{
			NewItemType = IsItem->m_Type + 1;
		}
		else if (IsItem->m_Type == ITEMGET(8, 146))
		{
			NewItemType = IsItem->m_Type + 1;
		}
		else if (IsItem->m_Type == ITEMGET(9, 146))
		{
			NewItemType = IsItem->m_Type + 1;
		}
		else if (IsItem->m_Type == ITEMGET(11, 146))
		{
			NewItemType = IsItem->m_Type + 1;
		}
		else if (IsItem->m_Type == ITEMGET(7, 148))
		{
			NewItemType = IsItem->m_Type + 1;
		}
		else if (IsItem->m_Type == ITEMGET(8, 148))
		{
			NewItemType = IsItem->m_Type + 1;
		}
		else if (IsItem->m_Type == ITEMGET(9, 148))
		{
			NewItemType = IsItem->m_Type + 1;
		}
		else if (IsItem->m_Type == ITEMGET(10, 148))
		{
			NewItemType = IsItem->m_Type + 1;
		}
		else if (IsItem->m_Type == ITEMGET(11, 148))
		{
			NewItemType = IsItem->m_Type + 1;
		}
		else if (IsItem->m_Type == ITEMGET(5, 31))
		{
			NewItemType = IsItem->m_Type + 4;
		}
		else if (IsItem->m_Type == ITEMGET(6, 20))
		{
			NewItemType = IsItem->m_Type + 4;
		}
		else if (IsItem->m_Type == ITEMGET(0, 26))
		{
			NewItemType = IsItem->m_Type + 10;
		}
		else if (IsItem->m_Type == ITEMGET(0, 27))
		{
			NewItemType = IsItem->m_Type + 2;
		}
		else if (IsItem->m_Type == ITEMGET(4, 23))
		{
			NewItemType = IsItem->m_Type + 2;
		}
		else if (IsItem->m_Type == ITEMGET(6, 19))
		{
			NewItemType = IsItem->m_Type + 4;
		}
		else if (IsItem->m_Type == ITEMGET(0, 28))
		{
			NewItemType = IsItem->m_Type + 2;
		}
		else if (IsItem->m_Type == ITEMGET(5, 30))
		{
			NewItemType = IsItem->m_Type + 5;
		}
		else if (IsItem->m_Type == ITEMGET(6, 18))
		{
			NewItemType = IsItem->m_Type + 7;
		}
		else if (IsItem->m_Type == ITEMGET(2, 17))
		{
			NewItemType = IsItem->m_Type + 2;
		}
		else if (IsItem->m_Type == ITEMGET(5, 20))
		{
			NewItemType = IsItem->m_Type + 12;
		}
		else if (IsItem->m_Type == ITEMGET(0, 52))
		{
			NewItemType = IsItem->m_Type + 1;
		}
		else if (IsItem->m_Type == ITEMGET(3, 22))
		{
			NewItemType = IsItem->m_Type + 1;
		}
		else if (IsItem->m_Type == ITEMGET(6, 40))
		{
			NewItemType = IsItem->m_Type + 1;
		}
		else if (IsItem->m_Type == ITEMGET(6, 17))
		{
			NewItemType = IsItem->m_Type + 5;
		}

		CItemObject NewItem;
		NewItem.m_Type = NewItemType;
		NewItem.m_Option1 = IsItem->m_Option1;
		NewItem.m_Option2 = IsItem->m_Option2;
		NewItem.m_SocketOption[0] = IsItem->m_SocketOption[0];
		NewItem.m_SocketOption[1] = IsItem->m_SocketOption[1];
		NewItem.m_SocketOption[2] = IsItem->m_SocketOption[2];
		NewItem.m_SocketOption[3] = IsItem->m_SocketOption[3];
		NewItem.m_SocketOption[4] = IsItem->m_SocketOption[4];
		NewItem.m_BonusSocketOption = IsItem->m_BonusSocketOption;
		//NewItem.Convert(NewItemType, IsItem->m_Option1, IsItem->m_Option2, IsItem->m_Option3, 0, 0, 0, IsItem->m_SocketOption, IsItem->m_BonusSocketOption, 0, 3);

		::gObjInventoryItemSet(Obj.m_Index, ItemPos, -1);
		::Obj.pInventory[ItemPos]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, ItemPos, 1);

		::gObjInventoryItemSet(Obj.m_Index, SocketUpgradeNotePos, -1);
		::Obj.pInventory[SocketUpgradeNotePos]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, SocketUpgradeNotePos, 1);

		::gObjInventoryItemSet(Obj.m_Index, JoBlessPos1, -1);
		::Obj.pInventory[JoBlessPos1]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JoBlessPos1, 1);

		::gObjInventoryItemSet(Obj.m_Index, JoBlessPos2, -1);
		::Obj.pInventory[JoBlessPos2]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JoBlessPos2, 1);

		::gObjInventoryItemSet(Obj.m_Index, JoBlessPos3, -1);
		::Obj.pInventory[JoBlessPos3]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JoBlessPos3, 1);

		::gObjInventoryItemSet(Obj.m_Index, JoBlessPos4, -1);
		::Obj.pInventory[JoBlessPos4]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JoBlessPos4, 1);

		::gObjInventoryItemSet(Obj.m_Index, JoBlessPos5, -1);
		::Obj.pInventory[JoBlessPos5]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JoBlessPos5, 1);

		::gObjInventoryItemSet(Obj.m_Index, JoSoulPos1, -1);
		::Obj.pInventory[JoSoulPos1]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JoSoulPos1, 1);

		::gObjInventoryItemSet(Obj.m_Index, JoSoulPos2, -1);
		::Obj.pInventory[JoSoulPos2]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JoSoulPos2, 1);

		::gObjInventoryItemSet(Obj.m_Index, JoSoulPos3, -1);
		::Obj.pInventory[JoSoulPos3]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JoSoulPos3, 1);

		::gObjInventoryItemSet(Obj.m_Index, JoSoulPos4, -1);
		::Obj.pInventory[JoSoulPos4]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JoSoulPos4, 1);

		::gObjInventoryItemSet(Obj.m_Index, JoSoulPos5, -1);
		::Obj.pInventory[JoSoulPos5]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JoSoulPos5, 1);

		::gObjInventoryItemSet(Obj.m_Index, JoChaosPos, -1);
		::Obj.pInventory[JoChaosPos]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JoChaosPos, 1);

		ItemCreate(Obj.m_Index, 235, 0, 0, NewItem.m_Type, 0, 0, NewItem.m_Option1, NewItem.m_Option2, NewItem.m_Option3, Obj.m_Index, 0, 0, 0, NewItem.m_SocketOption, NewItem.m_BonusSocketOption);
		gObjInventoryCommit(Obj.m_Index);

		//sLog->outBasic(" Socket Item Upgrade Mix Successfull");

		pMsg.Result1 = 1;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return TRUE;
	}
	else
	{
		::gObjInventoryItemSet(Obj.m_Index, SocketUpgradeNotePos, -1);
		::Obj.pInventory[SocketUpgradeNotePos]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, SocketUpgradeNotePos, 1);

		::gObjInventoryItemSet(Obj.m_Index, JoBlessPos1, -1);
		::Obj.pInventory[JoBlessPos1]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JoBlessPos1, 1);

		::gObjInventoryItemSet(Obj.m_Index, JoBlessPos2, -1);
		::Obj.pInventory[JoBlessPos2]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JoBlessPos2, 1);

		::gObjInventoryItemSet(Obj.m_Index, JoBlessPos3, -1);
		::Obj.pInventory[JoBlessPos3]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JoBlessPos3, 1);

		::gObjInventoryItemSet(Obj.m_Index, JoBlessPos4, -1);
		::Obj.pInventory[JoBlessPos4]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JoBlessPos4, 1);

		::gObjInventoryItemSet(Obj.m_Index, JoBlessPos5, -1);
		::Obj.pInventory[JoBlessPos5]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JoBlessPos5, 1);

		::gObjInventoryItemSet(Obj.m_Index, JoSoulPos1, -1);
		::Obj.pInventory[JoSoulPos1]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JoSoulPos1, 1);

		::gObjInventoryItemSet(Obj.m_Index, JoSoulPos2, -1);
		::Obj.pInventory[JoSoulPos2]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JoSoulPos2, 1);

		::gObjInventoryItemSet(Obj.m_Index, JoSoulPos3, -1);
		::Obj.pInventory[JoSoulPos3]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JoSoulPos3, 1);

		::gObjInventoryItemSet(Obj.m_Index, JoSoulPos4, -1);
		::Obj.pInventory[JoSoulPos4]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JoSoulPos4, 1);

		::gObjInventoryItemSet(Obj.m_Index, JoSoulPos5, -1);
		::Obj.pInventory[JoSoulPos5]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JoSoulPos5, 1);

		::gObjInventoryItemSet(Obj.m_Index, JoChaosPos, -1);
		::Obj.pInventory[JoChaosPos]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JoChaosPos, 1);

		pMsg.Result1 = 0;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

		//sLog->outError( " Socket Item Upgrade Mix Failed");
		return FALSE;
	}
}

void CMixSystem::SeedSphereCompositeMixS12(CGameObject &Obj, BYTE SeedPos, BYTE SpherePos, BYTE JocPos, BYTE ChaosPos)
{
	CItemObject * lpSeedItem = NULL;
	CItemObject * lpSphereItem = NULL;
	CItemObject * lpJoCItemObject = NULL;
	CItemObject * lpChaosItem = NULL;
	int isChaos = 0;
	int isSeed = 0;
	int isSphere = 0;
	int isJoc = 0;

	int isFire = 0;
	int isWater = 0;
	int isIce = 0;
	int isLighting = 0;
	int isWind = 0;
	int isEarth = 0;

	lpSeedItem = &Obj.pInventory[SeedPos];
	lpSphereItem = &Obj.pInventory[SpherePos];
	lpJoCItemObject = &Obj.pInventory[JocPos];
	lpChaosItem = &Obj.pInventory[ChaosPos];
	PMSG_SEEDRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg.h, 0x78, sizeof(PMSG_SEEDRESULT));
	pMsg.Result = 0x01;
	pMsg.Result1 = 0x00; //Fail

	if (lpChaosItem->m_Type == ITEMGET(12, 15))
	{
		isChaos++;
	}
	if (lpJoCItemObject->m_Type == ITEMGET(14, 22))
	{
		isJoc++;
	}
	if (lpSphereItem->m_Type == ITEMGET(12, 70))
	{
		isSphere++;
	}
	if (lpSeedItem->m_Type == ITEMGET(12, 60)) //Fire
	{
		isFire++;
	}
	else if (lpSeedItem->m_Type == ITEMGET(12, 61)) //Water
	{
		isWater++;
	}
	else if (lpSeedItem->m_Type == ITEMGET(12, 62)) //Ice
	{
		isIce++;
	}
	else if (lpSeedItem->m_Type == ITEMGET(12, 63)) //Wind
	{
		isWind++;
	}
	else if (lpSeedItem->m_Type == ITEMGET(12, 64)) //Lighting
	{
		isLighting++;
	}
	else if (lpSeedItem->m_Type == ITEMGET(12, 65)) //Earth
	{
		isEarth++;
	}

	if ((isChaos != 1) && (isSphere != 1) && (isJoc != 1))
	{

		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (Obj.m_PlayerData->Money <= 1000000)
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return; //No Zen
	}

	BYTE OptionIndex = 0;
	BYTE SphereLevel = 0; //58
	CSeedSphereItem SeedSphereItem;

	OptionIndex = g_SocketOptionSystem.GetSeedIndex(lpSeedItem->m_Type, lpSeedItem->m_Level);
	SphereLevel = g_SocketOptionSystem.GetSphereLevel(lpSphereItem->m_Type);
	g_SocketOptionSystem.MakeSeedSphere(&SeedSphereItem, OptionIndex, SphereLevel);


	Obj.m_PlayerData->Money -= 1000000;
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	if (SeedSphereItem.m_ItemCode != 0)
	{
		pMsg.Result1 = 1;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		::gObjInventoryItemSet(Obj.m_Index, SeedPos, -1);
		::Obj.pInventory[SeedPos]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, SeedPos, 1);

		::gObjInventoryItemSet(Obj.m_Index, SpherePos, -1);
		::Obj.pInventory[SpherePos]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, SpherePos, 1);

		::gObjInventoryItemSet(Obj.m_Index, JocPos, -1);
		::Obj.pInventory[JocPos]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JocPos, 1);

		::gObjInventoryItemSet(Obj.m_Index, ChaosPos, -1);
		::Obj.pInventory[ChaosPos]->Clear();
		::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, ChaosPos, 1);
		ItemCreate(Obj.m_Index, 235, 0, 0, SeedSphereItem.m_ItemCode, SeedSphereItem.m_ItemLevel, 0, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
		gObjInventoryCommit(Obj.m_Index);
		sLog->outBasic("[%s][%s] SeedSphere Composite Mix Success - SeedSphere Info ( Index : %d, Type : %d, Level : %d, ItemCode : %d )",
			Obj.AccountID, Obj.Name, SeedSphereItem.m_SeedSphereIndex, SeedSphereItem.m_SeedType, SeedSphereItem.m_SphereLevel, SeedSphereItem.m_ItemCode);
	}
}

void CMixSystem::SeedSphereCompositeMix(CGameObject &Obj)
{
	int iSeedItemCount = 0; //2
	int iSphereItemCount = 0; //3
	int iInvalidItemCount = 0; //4
	int iChoasGemCount = 0; //5
	int iCreationGemCount = 0; //6
	int iChaosTaxMoney = 0; //7
	int iMixMoney = 0; //8
	DWORD iChaosMoney = 0; //9
	int iSeedSphereCompositeRate = 0; //10
	CItemObject * lpSeedItem = NULL; //11
	CItemObject * lpSphereItem = NULL; //12
	int iTetraGoldCount = 0;
	int iTetraSilverCount = 0;
	int iTetraBronzeCount = 0;
	int iSeedSpear3rdCount = 0;
	int iChaosJewel10BunchCount = 0;
	int iCreationJewel10BunchCount = 0;
	BOOL bMakeSeedSpear4th = FALSE;

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;
	Obj.ChaosLock = TRUE;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++) //loc21
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (g_SocketOptionSystem.IsSeedItem(Obj.pChaosBox[n].m_Type) == 1)
			{
				iSeedItemCount++;
				iChaosMoney += Obj.pChaosBox[n].m_BuyMoney;
				lpSeedItem = &Obj.pChaosBox[n];
			}

			else if (g_SocketOptionSystem.IsSphereItem(Obj.pChaosBox[n].m_Type) == 1)
			{
				iSphereItemCount++;
				iChaosMoney += Obj.pChaosBox[n].m_BuyMoney;
				lpSphereItem = &Obj.pChaosBox[n];
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 15))
			{
				iChoasGemCount++;
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 22))
			{
				iCreationGemCount++;
			}

			else if (Obj.pChaosBox[n].m_Type >= ITEMGET(12, 112) && Obj.pChaosBox[n].m_Type <= ITEMGET(12, 117))
			{
				iSeedSpear3rdCount++;
				lpSeedItem = &Obj.pChaosBox[n];
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 245))
			{
				iTetraGoldCount++;
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 246))
			{
				iTetraSilverCount++;
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 247))
			{
				iTetraBronzeCount++;
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 141) && Obj.pChaosBox[n].m_Level == 0)
			{
				iChaosJewel10BunchCount++;
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 137) && Obj.pChaosBox[n].m_Level == 0)
			{
				iCreationJewel10BunchCount++;
			}

			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if (iInvalidItemCount > 0)
	{
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	if (iSeedSpear3rdCount != 0)
	{
		if (iSphereItemCount != 1 || iSeedSpear3rdCount != 1 || iChaosJewel10BunchCount != 1 || iCreationJewel10BunchCount != 1)
		{
			pMsg.Result = CB_INCORRECT_MIX_ITEMS;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
			Obj.ChaosLock = FALSE;
			return;
		}

		bMakeSeedSpear4th = TRUE;
	}

	if (bMakeSeedSpear4th == FALSE && (iSphereItemCount != 1 || iSeedItemCount != 1 || iChoasGemCount != 1 || iCreationGemCount != 1))
	{
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	if (iTetraBronzeCount + iTetraSilverCount + iTetraGoldCount > 1)
	{
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	iChaosTaxMoney = (int)((__int64)g_SocketOptionSystem.m_RequireMoneyForSeedSphereComposite * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	iMixMoney = g_SocketOptionSystem.m_RequireMoneyForSeedSphereComposite + iChaosTaxMoney;

	if (Obj.m_PlayerData->Money < iMixMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	LogChaosItem(lpObj, "SeedSphereComposite");

	if (bMakeSeedSpear4th == TRUE)
	{
		if (iTetraGoldCount == 1)
		{
			iSeedSphereCompositeRate = (iChaosMoney / 200000) + 80;
		}

		else if (iTetraSilverCount == 1)
		{
			iSeedSphereCompositeRate = (iChaosMoney / 200000) + 65;
		}

		else if (iTetraBronzeCount == 1)
		{
			iSeedSphereCompositeRate = (iChaosMoney / 200000) + 50;
		}

		else
		{
			iSeedSphereCompositeRate = (iChaosMoney / 200000) + 20;
		}
	}

	else
	{
		iSeedSphereCompositeRate = 80 + (iChaosMoney / 200000);
	}

	Obj.m_PlayerData->Money -= iMixMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	if (rand() % 100 > iSeedSphereCompositeRate) //fail rate
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		sLog->outBasic("[%s][%s] SeedSphere Composite Mix Failed.", Obj.AccountID, Obj.Name);
	}
	else
	{
		BYTE OptionIndex = 0;
		BYTE SphereLevel = 0; //58
		CSeedSphereItem SeedSphereItem;

		if (bMakeSeedSpear4th == TRUE)
		{
			OptionIndex = g_SocketOptionSystem.GetSeedIndexForSeedSpearItemCode(lpSeedItem->m_Type, lpSeedItem->m_Level);
		}

		else
		{
			OptionIndex = g_SocketOptionSystem.GetSeedIndex(lpSeedItem->m_Type, lpSeedItem->m_Level);
		}

		SphereLevel = g_SocketOptionSystem.GetSphereLevel(lpSphereItem->m_Type);

		g_SocketOptionSystem.MakeSeedSphere(&SeedSphereItem, OptionIndex, SphereLevel);

		if (SeedSphereItem.m_ItemCode != 0)
		{
			pMsg.Result = 1;
			ItemCreate(Obj.m_Index, 0xFF, 0, 0, SeedSphereItem.m_ItemCode, SeedSphereItem.m_ItemLevel, 1, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
			gObjInventoryCommit(Obj.m_Index);
			sLog->outBasic("[%s][%s] SeedSphere Composite Mix Success - SeedSphere Info ( Index : %d, Type : %d, Level : %d, ItemCode : %d )",
				Obj.AccountID, Obj.Name, SeedSphereItem.m_SeedSphereIndex, SeedSphereItem.m_SeedType, SeedSphereItem.m_SphereLevel, SeedSphereItem.m_ItemCode);
			return;
		}

		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		sLog->outBasic("[%s][%s] SeedSphere Composite Mix Failed - SeedSphere Data is NULL", Obj.AccountID, Obj.Name);
	}

	Obj.ChaosLock = FALSE;
}
void CMixSystem::SetSeedSphereMixS12(CGameObject &Obj, BYTE ItemPos, BYTE Slot, BYTE SeedPos, BYTE JocPos, BYTE ChaosPos)
{
	CItemObject * pSocketItem = NULL;
	CItemObject * lpSphereItem = NULL;
	CItemObject * lpJoCItemObject = NULL;
	CItemObject * lpChaosItem = NULL;
	int isChaos = 0;
	int isSeed = 0;
	int isSphere = 0;
	int isJoc = 0;

	int iSocketItemCount = 0; //2
	int iInvalidItemCount = 0; //3
	int iSeedSpearItemCount = 0; //4
	int iChoasGemCount = 0; //5
	int iCreationGemCount = 0; //6
							   //	CItemObject * pSocketItem = NULL; //7
	CItemObject * pSeedSpearItem = NULL; //8
	CItemObject Item;
	int iChaosTaxMoney = 0;
	int iMixMoney = 0;

	pSocketItem = &Obj.pInventory[ItemPos];
	pSeedSpearItem = &Obj.pInventory[SeedPos];
	lpJoCItemObject = &Obj.pInventory[JocPos];
	lpChaosItem = &Obj.pInventory[ChaosPos];
	PMSG_SEEDRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg.h, 0x78, sizeof(PMSG_SEEDRESULT));
	pMsg.Result = 0x01;
	pMsg.Result1 = 0x00; //Fail

	if (pSocketItem->IsItem() == TRUE)
	{
		if (gSocketItemType.CheckSocketItemType(pSocketItem->m_Type) == TRUE)
		{
			if (pSocketItem->m_SocketOption[Slot] == 0xFE) //Slot with NO OPTION
			{
				iSocketItemCount++;
			}

			if (g_SocketOptionSystem.IsSeedSphereItem(pSeedSpearItem->m_Type) == TRUE)
			{
				iSeedSpearItemCount++;
			}
			if (lpChaosItem->m_Type == ITEMGET(12, 15))
			{
				iChoasGemCount++;
			}
			if (lpJoCItemObject->m_Type == ITEMGET(14, 22))
			{
				iCreationGemCount++;
			}
		}
	}


	if (iSocketItemCount != 1 && iSeedSpearItemCount != 1 && iChoasGemCount != 1 && iCreationGemCount != 1)
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (g_SocketOptionSystem.CheckItemForSameSeedType(pSocketItem, pSeedSpearItem->m_Type, pSeedSpearItem->m_Level) == 0)
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	iChaosTaxMoney = (int)((__int64)g_SocketOptionSystem.m_RequireMoneyForSeedSphereSet * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	iMixMoney = g_SocketOptionSystem.m_RequireMoneyForSeedSphereSet + iChaosTaxMoney;

	if (Obj.m_PlayerData->Money < iMixMoney)
	{
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	BYTE SocketByteOption = 0;
	SocketByteOption = g_SocketOptionSystem.GetSeedSphereIndexForItemCode(pSeedSpearItem->m_Type, pSeedSpearItem->m_Level);

	if (SocketByteOption == 0xFF)
	{
		sLog->outBasic("[%s][%s] Set Seed Mix Failed - SeedSphere is NULL", Obj.AccountID, Obj.Name);
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (g_SocketOptionSystem.CheckSetSeedSphereItemType(pSocketItem, SocketByteOption) == 0)
	{
		sLog->outBasic("[%s][%s] Set Seed Mix Failed - Wrong Item Type for Set Socket", Obj.AccountID, Obj.Name);
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	Obj.m_PlayerData->Money -= iMixMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	g_SocketOptionSystem.SetSocketSlot(pSocketItem, SocketByteOption, Slot);

	float Dur = ItemGetDurability(pSocketItem->m_Type, pSocketItem->m_Level, pSocketItem->IsExtItem(), pSocketItem->IsSetItem());

	Item.m_Number = pSocketItem->m_Number;
	Item.m_Level = pSocketItem->m_Level;
	Item.m_Durability = Dur * pSocketItem->m_Durability / pSocketItem->m_BaseDurability;
	Item.m_JewelOfHarmonyOption = pSocketItem->m_JewelOfHarmonyOption;

	Item.Convert(pSocketItem->m_Type, pSocketItem->m_Option1, pSocketItem->m_Option2, pSocketItem->m_Option3, pSocketItem->m_NewOption,
		pSocketItem->m_SetOption, pSocketItem->m_ItemOptionEx, pSocketItem->m_SocketOption, pSocketItem->m_BonusSocketOption, 0, CURRENT_DB_VERSION);

	::gObjInventoryItemSet(Obj.m_Index, ItemPos, -1);
	::Obj.pInventory[ItemPos]->Clear();
	::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, ItemPos, 1);

	::gObjInventoryItemSet(Obj.m_Index, SeedPos, -1);
	::Obj.pInventory[SeedPos]->Clear();
	::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, SeedPos, 1);

	::gObjInventoryItemSet(Obj.m_Index, JocPos, -1);
	::Obj.pInventory[JocPos]->Clear();
	::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, JocPos, 1);

	::gObjInventoryItemSet(Obj.m_Index, ChaosPos, -1);
	::Obj.pInventory[ChaosPos]->Clear();
	::gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, ChaosPos, 1);
	pMsg.Result1 = 1;
	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
	ItemCreate(Obj.m_Index, 235, 0, 0, Item.m_Type, Item.m_Level, 0, Item.m_Option1, Item.m_Option2, Item.m_Option3, Obj.m_Index, Item.m_NewOption, 0, 0, Item.m_SocketOption, Item.m_BonusSocketOption);

	sLog->outBasic("[%s][%s] Set SeedSphere Mix Success - ItemInfo ( Name : %s, ItemCode : %d, Level : %d, SocketOption[%d,%d,%d,%d,%d], BonusOption : %d )",
		Obj.AccountID, Obj.Name, ItemAttribute[Item.m_Type].Name, Item.m_Type, Item.m_Level, Item.m_SocketOption[0], Item.m_SocketOption[1],
		Item.m_SocketOption[2], Item.m_SocketOption[3], Item.m_SocketOption[4], Item.m_BonusSocketOption);

}
void CMixSystem::SetSeedSphereMix(CGameObject &Obj, BYTE btPos)
{
	int iSocketItemCount = 0; //2
	int iInvalidItemCount = 0; //3
	int iSeedSpearItemCount = 0; //4
	int iChoasGemCount = 0; //5
	int iCreationGemCount = 0; //6
	CItemObject * pSocketItem = NULL; //7
	CItemObject * pSeedSpearItem = NULL; //8
	CItemObject Item;
	int iChaosTaxMoney = 0;
	int iMixMoney = 0;

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	if (btPos >= 5)
	{
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	Obj.ChaosLock = TRUE;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++) //loc21
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (gSocketItemType.CheckSocketItemType(Obj.pChaosBox[n].m_Type) == TRUE)
			{
				if (Obj.pChaosBox[n].m_SocketOption[btPos] == 0xFE) //Slot with NO OPTION
				{
					iSocketItemCount++; //
					pSocketItem = &Obj.pChaosBox[n];
				}
			}
			else if (g_SocketOptionSystem.IsSeedSphereItem(Obj.pChaosBox[n].m_Type) == TRUE)
			{
				iSeedSpearItemCount++;
				pSeedSpearItem = &Obj.pChaosBox[n];
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 15))
			{
				iChoasGemCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 22))
			{
				iCreationGemCount++;
			}
			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if (iInvalidItemCount > 0)
	{
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	if (iSocketItemCount != 1 && iSeedSpearItemCount != 1 && iChoasGemCount != 1 && iCreationGemCount != 1)
	{
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	if (g_SocketOptionSystem.CheckItemForSameSeedType(pSocketItem, pSeedSpearItem->m_Type, pSeedSpearItem->m_Level) == 0)
	{
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	iChaosTaxMoney = (int)((__int64)g_SocketOptionSystem.m_RequireMoneyForSeedSphereSet * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	iMixMoney = g_SocketOptionSystem.m_RequireMoneyForSeedSphereSet + iChaosTaxMoney;

	if (Obj.m_PlayerData->Money < iMixMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	LogChaosItem(lpObj, "SetSeedSphere");

	BYTE SocketByteOption = 0;
	SocketByteOption = g_SocketOptionSystem.GetSeedSphereIndexForItemCode(pSeedSpearItem->m_Type, pSeedSpearItem->m_Level);

	if (SocketByteOption == 0xFF)
	{
		pMsg.Result = 7;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	if (g_SocketOptionSystem.CheckSetSeedSphereItemType(pSocketItem, SocketByteOption) == 0)
	{
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	Obj.m_PlayerData->Money -= iMixMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	g_SocketOptionSystem.SetSocketSlot(pSocketItem, SocketByteOption, btPos);

	float Dur = ItemGetDurability(pSocketItem->m_Type, pSocketItem->m_Level, pSocketItem->IsExtItem(), pSocketItem->IsSetItem());

	Item.m_Number = pSocketItem->m_Number;
	Item.m_Level = pSocketItem->m_Level;
	Item.m_Durability = Dur * pSocketItem->m_Durability / pSocketItem->m_BaseDurability;
	Item.m_JewelOfHarmonyOption = pSocketItem->m_JewelOfHarmonyOption;

	Item.Convert(pSocketItem->m_Type, pSocketItem->m_Option1, pSocketItem->m_Option2, pSocketItem->m_Option3, pSocketItem->m_NewOption,
		pSocketItem->m_SetOption, pSocketItem->m_ItemOptionEx, pSocketItem->m_SocketOption, pSocketItem->m_BonusSocketOption, 0, CURRENT_DB_VERSION);

	Obj.ChaosLock = FALSE;

	ItemByteConvert(pMsg.ItemInfo, Item);
	this->ChaosBoxInit(lpObj);
	::gObjChaosBoxInsertItemPos(Obj.m_Index, Item, 0, -1);
	gObjChaosItemSet(Obj.m_Index, 0, 1);
	gGameProtocol.GCUserChaosBoxSend(lpObj, 0);

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}

void CMixSystem::SeedSphereRemoveMix(CGameObject &Obj, BYTE btPos)
{
	int iSocketItemCount = 0; //2
	int iInvalidItemCount = 0; //3
	int iSeedSphereItemCount = 0;
	int iSocketItemPosition = 0;
	int iSeedSpherePosition = 0;

	int iMixMoney = 0; //7
	int iChaosTaxMoney = 0; //8

	int iChaosJewelCount = 0;
	int iJewelOfHarmonyCount = 0;
	int iGemOfDefendCount = 0;

	int iChaosJewelPosition[5];
	int iJewelOfHarmonyPosition[5];

	int iGemOfDefendPosition = -1;

	iChaosJewelPosition[0] = -1;
	iChaosJewelPosition[1] = -1;
	iChaosJewelPosition[2] = -1;
	iChaosJewelPosition[3] = -1;
	iChaosJewelPosition[4] = -1;

	iJewelOfHarmonyPosition[0] = -1;
	iJewelOfHarmonyPosition[1] = -1;
	iJewelOfHarmonyPosition[2] = -1;
	iJewelOfHarmonyPosition[3] = -1;
	iJewelOfHarmonyPosition[4] = -1;

	CItemObject * lpSocketItem = NULL;

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	if (btPos >= 5)
	{
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	Obj.ChaosLock = TRUE;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++) //loc21
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (gSocketItemType.CheckSocketItemType(Obj.pChaosBox[n].m_Type) == 1)
			{
				if (Obj.pChaosBox[n].m_SocketOption[btPos] != 0xFE && Obj.pChaosBox[n].m_SocketOption[btPos] != 0xFF) //Slot with OPTION
				{
					iSocketItemCount++; //
					iSocketItemPosition = n;
					lpSocketItem = &Obj.pChaosBox[n];
				}
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 15))
			{
				if (iChaosJewelCount >= 0 && iChaosJewelCount < 5)
				{
					iChaosJewelPosition[iChaosJewelCount] = n;
				}

				iChaosJewelCount++;
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 31))
			{
				iGemOfDefendPosition = n;
				iGemOfDefendCount++;
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 42))
			{
				if (iJewelOfHarmonyCount >= 0 && iJewelOfHarmonyCount < 5)
				{
					iJewelOfHarmonyPosition[iJewelOfHarmonyCount] = n;
				}

				iJewelOfHarmonyCount++;
			}

			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if (iInvalidItemCount > 0)
	{
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	if (iSocketItemCount != 1)
	{
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	if (iChaosJewelCount != 5 || iJewelOfHarmonyCount != 5 || iGemOfDefendCount != 1)
	{
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	iChaosTaxMoney = (int)((__int64)g_SocketOptionSystem.m_RequireMoneyForSeedSphereRemove * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	iMixMoney = g_SocketOptionSystem.m_RequireMoneyForSeedSphereRemove + iChaosTaxMoney;

	if (Obj.m_PlayerData->Money < iMixMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	BYTE btSocketOption = Obj.pChaosBox[iSocketItemPosition].m_SocketOption[btPos];
	BYTE btSeedIndex = btSocketOption % 50;
	BOOL bEmptyPlace = CheckInventoryEmptySpace(lpObj, 1, 1);

	if (bEmptyPlace == FALSE)
	{
		pMsg.Result = CB_LACKING_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	CSeedItem * lpSeedData = g_SocketOptionSystem.GetSeedData(btSeedIndex);

	if (lpSeedData == NULL)
	{
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	Obj.m_PlayerData->Money -= iMixMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	LogChaosItem(lpObj, "SeedSphereRemove");

	if (rand() % 100 < 80)
	{
		g_SocketOptionSystem.ClearSocketSlot(&Obj.pChaosBox[iSocketItemPosition], btPos);
		pMsg.Result = CB_SUCCESS;

		ItemCreate(Obj.m_Index, 219, 0, 0, lpSeedData->m_ItemCode, lpSeedData->m_ItemLevel, 1, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
	}

	else
	{
		pMsg.Result = CB_ERROR;
	}

	int levelitemdur = ItemGetDurability(lpSocketItem->m_Type, lpSocketItem->m_Level, lpSocketItem->IsExtItem(), lpSocketItem->IsSetItem());

	CItemObject NewItem;
	NewItem.m_Number = lpSocketItem->m_Number;
	NewItem.m_Level = lpSocketItem->m_Level;
	NewItem.m_Durability = levelitemdur * lpSocketItem->m_Durability / lpSocketItem->m_BaseDurability;
	NewItem.m_JewelOfHarmonyOption = lpSocketItem->m_JewelOfHarmonyOption;

	NewItem.Convert(lpSocketItem->m_Type, lpSocketItem->m_Option1, lpSocketItem->m_Option2, lpSocketItem->m_Option3,
		lpSocketItem->m_NewOption, lpSocketItem->m_SetOption, lpSocketItem->m_ItemOptionEx,
		lpSocketItem->m_SocketOption, lpSocketItem->m_BonusSocketOption, 0, 3);

	Obj.ChaosLock = FALSE;

	ItemByteConvert(pMsg.ItemInfo, NewItem);

	this->ChaosBoxInit(lpObj);
	gObjChaosBoxInsertItemPos(Obj.m_Index, NewItem, 0, -1);

	gObjChaosItemSet(Obj.m_Index, 0, 1);
	gGameProtocol.GCUserChaosBoxSend(lpObj, 0);

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}

void CMixSystem::SecromiconMix(CGameObject &Obj)
{
	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	int first = 0;
	int second = 0;
	int third = 0;
	int fourth = 0;
	int fifth = 0;
	int sixth = 0;
	int invalid = 0;

	Obj.ChaosLock = TRUE;

	for (int n = 0; n < CHAOS_BOX_SIZE; ++n)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 103))
				first++;
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 104))
				second++;
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 105))
				third++;
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 106))
				fourth++;
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 107))
				fifth++;
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 108))
				sixth++;
			else
				invalid++;
		}
	}

	if (invalid)
	{
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (first == 1 && second == 1 && third == 1 && fourth == 1 && fifth == 1 && sixth == 1)
	{
		int iMixCost = g_ImperialGuardian.GetMixNeedZen();
		int iMixTax = iMixCost * g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / 100;
		if (iMixTax < 0)
			iMixTax = 0;
		iMixCost += iMixTax;
		if (Obj.m_PlayerData->Money < iMixCost)
		{
			pMsg.Result = CB_NOT_ENOUGH_ZEN;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
			Obj.ChaosLock = FALSE;
			return;
		}

		Obj.m_PlayerData->Money -= iMixCost;
		g_CastleSiegeSync.AddTributeMoney(iMixTax);
		gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

		ItemCreate(Obj.m_Index, 255, 0, 0, ITEMGET(14, 109), 0, 0, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
		gObjInventoryCommit(Obj.m_Index);

	}
	Obj.ChaosLock = FALSE;
}

void CMixSystem::PremiumBoxMix(CGameObject &Obj)
{
	PMSG_CHAOSMIXRESULT pMsg = { 0 };
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	char SealedSilver = 0;
	char SealedGold = 0;
	char SilverKey = 0;
	char GoldKey = 0;
	char Invalid = 0;
	Obj.ChaosLock = TRUE;

	for (int n = 0; n < CHAOS_BOX_SIZE; ++n)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 112))
				SilverKey++;
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 113))
				GoldKey++;
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 121))
				SealedGold++;
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 122))
				SealedSilver++;
			else
				Invalid++;
		}
	}

	if (Invalid)
	{
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (SealedSilver == 1 && SilverKey == 1)
	{
		ItemCreate(Obj.m_Index, 255, 0, 0, ITEMGET(14, 124), 0, 0, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
		gObjInventoryCommit(Obj.m_Index);

		Obj.ChaosLock = FALSE;
		return;
	}
	else if (SealedGold == 1 && GoldKey == 1)
	{
		ItemCreate(Obj.m_Index, 255, 0, 0, ITEMGET(14, 123), 0, 0, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
		gObjInventoryCommit(Obj.m_Index);

		Obj.ChaosLock = FALSE;
		return;
	}
	else
	{
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}
}

struct pMixTmp
{
	BYTE bOk;
	BYTE Dur;
	BYTE NeedDur;
	int m_Item;
};

void CMixSystem::CherryBlossomMix(CGameObject &Obj)
{
	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	Obj.ChaosLock = TRUE;

	pMixTmp pTmp = { 0 };

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == FALSE)
			continue;

		if (pTmp.m_Item == FALSE)
		{
			pTmp.m_Item = Obj.pChaosBox[n].m_Type;

			if (pTmp.m_Item == ITEMGET(14, 88))
			{
				pTmp.NeedDur = 10;
			}
			else if (pTmp.m_Item == ITEMGET(14, 89))
			{
				pTmp.NeedDur = 50;
			}
			else if (pTmp.m_Item == ITEMGET(14, 90))
			{
				pTmp.NeedDur = 255;
			}
			else
			{
				pTmp.m_Item = 0;
				continue;
			}

			pTmp.bOk = TRUE;
		}

		if (pTmp.m_Item == Obj.pChaosBox[n].m_Type)
		{
			pTmp.Dur += (int)Obj.pChaosBox[n].m_Durability;
		}
		else
		{
			pTmp.bOk = FALSE;
			break;
		}
	}

	if (pTmp.Dur != pTmp.NeedDur)
	{
		pTmp.bOk = FALSE;
	}

	if (pTmp.m_Item == FALSE || pTmp.bOk == FALSE)
	{
		pMsg.Result = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	int iHeight = 4;
	int iWidth = 2;

	if (!CheckInventoryEmptySpace(lpObj, iHeight, iWidth))
	{
		pMsg.Result = 0xF1;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	int bResult = 0;

	CItemObject Item;
	time_t Duration;

	if (pTmp.m_Item == ITEMGET(14, 88))
	{
		bResult = g_BagManager.GetItemFromBag(Obj.m_Index, BAG_EVENT, EVENTBAG_CHERRYBLOSSOM_WHITE, Obj.m_Index, Item, Duration);
	}
	else if (pTmp.m_Item == ITEMGET(14, 89))
	{
		bResult = g_BagManager.GetItemFromBag(Obj.m_Index, BAG_EVENT, EVENTBAG_CHERRYBLOSSOM_RED, Obj.m_Index, Item, Duration);
	}
	else if (pTmp.m_Item == ITEMGET(14, 90))
	{
		bResult = g_BagManager.GetItemFromBag(Obj.m_Index, BAG_EVENT, EVENTBAG_CHERRYBLOSSOM_GOLD, Obj.m_Index, Item, Duration);
	}

	if (bResult == false)
	{
		ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	gObjInventoryCommit(Obj.m_Index);
	Obj.ChaosLock = FALSE;
}

/*
void CMixSystem::CherryBlossomMix(CGameObject &Obj)
{
PMSG_CHAOSMIXRESULT pMsg;
int iWhiteCherryBlossom = 0;
int iWhiteCherryBlossomDur = 0;
int iRedCherryBlossom = 0;
int iRedCherryBlossomDur = 0;
int iGoldCherryBlossom = 0;
int iGoldCherryBlossomDur = 0;
int iInvalidItemCount = 0;
int iMixType = -1;

PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
pMsg.Result = CB_ERROR;

Obj.ChaosLock = TRUE;

for (int n = 0; n < CHAOS_BOX_SIZE; n++)
{
if (Obj.pChaosBox[n].IsItem() == TRUE)
{
if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 88))
{
iWhiteCherryBlossom++;
iWhiteCherryBlossomDur += Obj.pChaosBox[n].m_Durability;
iMixType = 0;
}

else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 89))
{
iRedCherryBlossom++;
iRedCherryBlossomDur += Obj.pChaosBox[n].m_Durability;
iMixType = 1;
}

else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 90))
{
iGoldCherryBlossom++;
iGoldCherryBlossomDur += Obj.pChaosBox[n].m_Durability;
iMixType = 2;
}

else
{
iInvalidItemCount++;
iMixType = -1;
}
}
}

if (iInvalidItemCount != 0)
{
pMsg.Result = CB_INCORRECT_MIX_ITEMS;
IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
Obj.ChaosLock = FALSE;
return;
}

if (iMixType == 0)
{
if (iWhiteCherryBlossomDur != g_ConfigRead.mix.CherryBlossomWhiteNeedCount)
{
pMsg.Result = CB_INCORRECT_MIX_ITEMS;
IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
Obj.ChaosLock = FALSE;
return;
}
}

else if (iMixType == 1)
{
if (iRedCherryBlossomDur != g_ConfigRead.mix.CherryBlossomRedNeedCount)
{
pMsg.Result = CB_INCORRECT_MIX_ITEMS;
IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
Obj.ChaosLock = FALSE;
return;
}
}

else if (iMixType == 2)
{
if (iGoldCherryBlossomDur != g_ConfigRead.mix.CherryBlossomGoldNeedCount)
{
pMsg.Result = CB_INCORRECT_MIX_ITEMS;
IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
Obj.ChaosLock = FALSE;
return;
}
}

else
{
pMsg.Result = CB_INCORRECT_MIX_ITEMS;
IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
Obj.ChaosLock = FALSE;
return;
}

LogChaosItem(lpObj, "CherryBlosssomMix");

CItemObject Item;
time_t Duration;
int BagType;

if (iMixType == 0)
{
BagType = EVENTBAG_CHERRYBLOSSOM_WHITE;
}

else if (iMixType == 1)
{
BagType = EVENTBAG_CHERRYBLOSSOM_RED;
}

else if (iMixType == 2)
{
BagType = EVENTBAG_CHERRYBLOSSOM_GOLD;
}

if (g_BagManager.GetItemFromBag(Obj.m_Index, BAG_EVENT, BagType, Obj.m_Index, Item, Duration) == false)
{
ChaosBoxInit(lpObj);
gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
Obj.ChaosLock = FALSE;
return;
}

ItemCreate(Obj.m_Index, 255, 0, 0, Item.m_Type, Item.m_Level, Item.m_Durability, Item.m_Option1, Item.m_Option2, Item.m_Option3, Obj.m_Index, Item.m_NewOption, Item.m_SetOption, Duration, Item.m_SocketOption, Item.m_BonusSocketOption);

pMsg.Result = CB_SUCCESS;
IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

sLog->outBasic("[CherryBlossomMix] Item Mix Success [%s][%s]",Obj.AccountID, Obj.Name);

Obj.ChaosLock = FALSE;

}*/

void CMixSystem::ItemRefineMix(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	WORD ItemType = 0xFFFF;

	BYTE SoniCItemObject = 0;
	BYTE CycloneItem = 0;
	BYTE BlastItem = 0;
	BYTE MagmaItem = 0;
	BYTE HornItem = 0;
	BYTE AngeliCItemObject = 0;
	BYTE DevilItem = 0;
	BYTE SpiteItem = 0;
	BYTE AsuraItem = 0;
	BYTE ThunderboltItem = 0;
	BYTE MiracleItem = 0;
	BYTE SummonItem = 0;
	BYTE IceWalkerItem = 0;
	BYTE MammothItem = 0;
	BYTE GiantItem = 0;
	BYTE CoolutinItem = 0;
	BYTE IronKnightItem = 0;
	BYTE DarkMammothItem = 0;
	BYTE DarkGiantItem = 0;
	BYTE DarkCoolutinItem = 0;
	BYTE DarkIronKnightItem = 0;
	BYTE SocketItem = 0;
	int SocketValue = 0;
	BYTE Invalid = 0;
	int iCharmOfLuckCount = 0;

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 191))
			{
				SoniCItemObject++;
				ItemType = ITEMGET(0, 29);
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 192))
			{
				CycloneItem++;
				ItemType = ITEMGET(0, 36);
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 193))
			{
				BlastItem++;
				ItemType = ITEMGET(0, 37);
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 194))
			{
				MagmaItem++;
				ItemType = ITEMGET(3, 12);
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 195))
			{
				HornItem++;
				ItemType = ITEMGET(2, 20);
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 196))
			{
				AngeliCItemObject++;
				ItemType = ITEMGET(4, 25);
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 197))
			{
				DevilItem++;
				ItemType = ITEMGET(4, 26);
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 198))
			{
				MiracleItem++;
				ItemType = ITEMGET(5, 35);
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 199))
			{
				SpiteItem++;
				ItemType = ITEMGET(5, 37);
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 200))
			{
				AsuraItem++;
				ItemType = ITEMGET(0, 30);
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 202))
			{
				SummonItem++;
				ItemType = ITEMGET(5, 32);
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 201))
			{
				ThunderboltItem++;
				ItemType = ITEMGET(2, 19);
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 180))
			{
				IceWalkerItem++;
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 181))
			{
				MammothItem++;
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 182))
			{
				GiantItem++;
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 183))
			{
				CoolutinItem++;
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 184))
			{
				IronKnightItem++;
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 185))
			{
				DarkMammothItem++;
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 186))
			{
				DarkGiantItem++;
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 187))
			{
				DarkCoolutinItem++;
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 188))
			{
				DarkIronKnightItem++;
			}

			else if (gSocketItemType.CheckSocketItemType((int)&Obj.pChaosBox[n]) && Obj.pChaosBox[n].m_Level >= 7 && Obj.pChaosBox[n].m_Option3 >= 1)
			{
				SocketItem++;
				SocketValue += Obj.pChaosBox[n].m_BuyMoney;
			}

			else
			{
				if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 53))
				{
					iCharmOfLuckCount += 10;
				}
				else
				{
					Invalid++;
				}
			}
		}
	}

	if (Invalid)
	{
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	bool Error = false;

	switch (ItemType)
	{
	case ITEMGET(0, 29):
		if (SoniCItemObject != 1 || DarkIronKnightItem != 1 || IceWalkerItem != 2)
		{
			Error = true;
		}
		break;
	case ITEMGET(0, 36):
		if (CycloneItem != 1 || DarkMammothItem != 1 || MammothItem != 2)
		{
			Error = true;
		}
		break;
	case ITEMGET(0, 37):
		if (BlastItem != 1 || DarkIronKnightItem != 1 || GiantItem != 2)
		{
			Error = true;
		}
		break;
	case ITEMGET(3, 12):
		if (MagmaItem != 1 || DarkMammothItem != 1 || CoolutinItem != 2)
		{
			Error = true;
		}
		break;
	case ITEMGET(2, 20):
		if (HornItem != 1 || DarkGiantItem != 1 || GiantItem != 2)
		{
			Error = true;
		}
		break;
	case ITEMGET(4, 25):
		if (AngeliCItemObject != 1 || DarkMammothItem != 1 || IceWalkerItem != 2)
		{
			Error = true;
		}
		break;
	case ITEMGET(4, 26):
		if (DevilItem != 1 || DarkCoolutinItem != 1 || IronKnightItem != 2)
		{
			Error = true;
		}
		break;
	case ITEMGET(5, 35):
		if (MiracleItem != 1 || DarkMammothItem != 1 || GiantItem != 2)
		{
			Error = true;
		}
		break;
	case ITEMGET(5, 37):
		if (SpiteItem != 1 || DarkGiantItem != 1 || IronKnightItem != 2)
		{
			Error = true;
		}
		break;
	case ITEMGET(0, 30):
		if (AsuraItem != 1 || DarkIronKnightItem != 1 || IronKnightItem != 2)
		{
			Error = true;
		}
		break;
	case ITEMGET(5, 32):
		if (SummonItem != 1 || DarkMammothItem != 1 || IronKnightItem != 2)
		{
			Error = true;
		}
		break;
	case ITEMGET(2, 19):
		if (ThunderboltItem != 1 || DarkCoolutinItem != 1 || MammothItem != 2)
		{
			Error = true;
		}
		break;
	}

	if (SocketItem != 3)
	{
		Error = true;
	}

	if (Invalid)
	{
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	LogChaosItem(lpObj, "SocketWeaponMix");
	sLog->outBasic("[SocketWeaponMix] Chaos Mix Start");

	int nChaosNeedMoney = g_ConfigRead.mix.SocketWeaponRequireMoney;

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if (nChaosNeedMoney < 0)
	{
		nChaosNeedMoney = 0;
	}

	if (Obj.m_PlayerData->Money < nChaosNeedMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	Obj.m_PlayerData->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	Obj.ChaosSuccessRate = SocketValue / 360000;
	Obj.ChaosSuccessRate += iCharmOfLuckCount;

	if (Obj.ChaosSuccessRate > g_ConfigRead.mix.SocketWeaponMaxRate)
	{
		Obj.ChaosSuccessRate = g_ConfigRead.mix.SocketWeaponMaxRate;
	}

	Obj.ChaosMoney = g_ConfigRead.mix.SocketWeaponRequireMoney;

	if (rand() % 100 <= Obj.ChaosSuccessRate)
	{
		CItemObject Item;
		Item.m_Type = ItemType;

		Item.m_Level = 0;
		Item.m_Option2 = (rand() % 10000) < 2000 ? TRUE : FALSE;
		Item.m_Option1 = TRUE;
		Item.m_Option3 = 0;

		BYTE SocketOption[5];

		for (int i = 0; i < 5; i++)
		{
			SocketOption[i] = -1;
		}

		ItemCreate(Obj.m_Index, 255, 0, 0, Item.m_Type, Item.m_Level, 0, Item.m_Option1, Item.m_Option2, Item.m_Option3, Obj.m_Index, Item.m_NewOption, 0, 0, SocketOption, 0);
		gObjInventoryCommit(Obj.m_Index);

		sLog->outBasic("[SocketWeaponMix] [%s][%s] Mix Success.", Obj.AccountID, Obj.Name);
	}
	else
	{
		ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);
		sLog->outBasic("[SocketWeaponMix] [%s][%s] Mix Failed.", Obj.AccountID, Obj.Name);
	}

	Obj.ChaosLock = FALSE;
}

void CMixSystem::MonsterWingMix(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int WingCount = 0;
	int ChaosCount = 0;
	int CreationCount = 0;
	int DeathKingMaterialCount = 0;
	int HellMaineMaterialCount = 0;
	int DarkPhoenixMaterialCount = 0;
	int DeathBeamMaterialCount = 0;
	BOOL IsItemCape = FALSE;
	int InvalidItemCount = 0;
	int LuckyHarmAddRate = 0;
	DWORD ChaosMoney = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[i].m_Type == ITEMGET(12, 3) || Obj.pChaosBox[i].m_Type == ITEMGET(12, 4) ||
				Obj.pChaosBox[i].m_Type == ITEMGET(12, 5) || Obj.pChaosBox[i].m_Type == ITEMGET(12, 6) ||
				Obj.pChaosBox[i].m_Type == ITEMGET(13, 30) || Obj.pChaosBox[i].m_Type == ITEMGET(12, 42) ||
				Obj.pChaosBox[i].m_Type == ITEMGET(12, 49))
			{
				WingCount++;
				ChaosMoney = Obj.pChaosBox[i].m_BuyMoney;

				if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 30) || Obj.pChaosBox[i].m_Type == ITEMGET(12, 269) || Obj.pChaosBox[i].m_Type == ITEMGET(12, 49))
				{
					IsItemCape = TRUE;
				}
			}

			else
			{
				switch (Obj.pChaosBox[i].m_Type)
				{
				case ITEMGET(12, 15):
					ChaosCount++;
					break;
				case ITEMGET(14, 22):
					CreationCount++;
					break;
				case ITEMGET(14, 176):
					DeathKingMaterialCount++;
					break;
				case ITEMGET(14, 177):
					HellMaineMaterialCount++;
					break;
				case ITEMGET(14, 178):
					DarkPhoenixMaterialCount++;
					break;
				case ITEMGET(14, 179):
					DeathBeamMaterialCount++;
					break;
				case ITEMGET(14, 53):
					LuckyHarmAddRate = Obj.pChaosBox[i].m_Durability;
					break;
				default:
					InvalidItemCount++;
					break;
				}
			}
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (InvalidItemCount || WingCount != 1 || ChaosCount != 1 || CreationCount != 1 || (DeathKingMaterialCount + HellMaineMaterialCount + DarkPhoenixMaterialCount + DeathBeamMaterialCount) != 1)
	{
		sLog->outBasic("[ERROR][CBMIX][MONSTER_WING][Invalid Recipe Item][%s][%s]", Obj.AccountID, Obj.Name);

		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (LuckyHarmAddRate > 10)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = 0xF0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (IsItemCape == TRUE)
	{
		Obj.ChaosSuccessRate = ChaosMoney / 500000;
	}

	else
	{
		Obj.ChaosSuccessRate = ChaosMoney / 9000000;
	}

	if (Obj.ChaosSuccessRate < 0)
		Obj.ChaosSuccessRate = 0;

	if (Obj.ChaosSuccessRate == 0)
	{
		sLog->outBasic("[ERROR][CBMIX][MONSTER_WING][Success rate is 0][%s][%s]", Obj.AccountID, Obj.Name);

		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	LogChaosItem(lpObj, "MonsterWingMix");

	if (Obj.m_PlayerData->VipType != 0)
	{
		if (Obj.ChaosSuccessRate > g_VipSystem.GetWing25Rate(lpObj))
		{
			Obj.ChaosSuccessRate = g_VipSystem.GetWing25Rate(lpObj);
		}
	}
	else
	{
		if (Obj.ChaosSuccessRate > g_ConfigRead.mix.Wing25MaxRate)
		{
			Obj.ChaosSuccessRate = g_ConfigRead.mix.Wing25MaxRate;
		}
	}

	int nChaosNeedMoney = 100000 * Obj.ChaosSuccessRate;

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if (nChaosNeedMoney < 0)
	{
		nChaosNeedMoney = 0;
	}

	if (Obj.m_PlayerData->Money < nChaosNeedMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	Obj.ChaosSuccessRate += LuckyHarmAddRate;
	Obj.m_PlayerData->Money -= nChaosNeedMoney;

	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int WingType;

		if (DeathKingMaterialCount == 1)
		{
			WingType = 262;
		}

		else if (HellMaineMaterialCount == 1)
		{
			WingType = 263;
		}

		else if (DarkPhoenixMaterialCount == 1)
		{
			WingType = 264;
		}

		else if (DeathBeamMaterialCount == 1)
		{
			WingType = 265;
		}

		else
		{
			sLog->outBasic("[ERROR][CBMIX][MONSTER_WING][NOT MATCH MATERIAL][%s][%s]", Obj.AccountID, Obj.Name);

			Obj.ChaosLock = FALSE;
			pMsg.Result = CB_INCORRECT_MIX_ITEMS;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
			return;
		}

		int Option2 = FALSE;

		if (rand() % 1000 <= 200)
			Option2 = TRUE;

		int Option3 = 0;

		int iRandValue = rand() % 1000;

		if (iRandValue >= 120)
		{
			if (iRandValue >= 180)
			{
				if (iRandValue >= 210)
				{
					Option3 = 0;
				}

				else
				{
					Option3 = 3;
				}
			}

			else
			{
				Option3 = 2;
			}
		}

		else
		{
			Option3 = 1;
		}

		int NOption = 0;
		BYTE btNewExcOption[5];
		memset(btNewExcOption, -1, sizeof(btNewExcOption));

		NOption = g_ItemOptionTypeMng.WingExcOptionRand(ItemAttribute[ITEMGET(12, WingType)].ItemKindA, ItemAttribute[ITEMGET(12, WingType)].ItemKindB, btNewExcOption);

		if (rand() % 2 == 0)
		{
			NOption |= 0x10;
		}

		ItemCreate(Obj.m_Index, 255, 0, 0, ITEMGET(12, WingType), 0, 0, 0, Option2, Option3, -1, NOption, 0, 0, btNewExcOption, 0);
		gObjInventoryCommit(Obj.m_Index);

		sLog->outBasic("[MonsterWing Mix] [%s][%s] CBMix Success %d Money : %d-%d, CharmRate : %d", Obj.AccountID, Obj.Name,
			Obj.ChaosSuccessRate, Obj.m_PlayerData->Money, nChaosNeedMoney, LuckyHarmAddRate);
	}

	else
	{
		for (int i = 0; i < CHAOS_BOX_SIZE; i++)
		{
			Obj.pChaosBox[i].Clear();
		}

		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

		sLog->outBasic("[MonsterWing Mix] [%s][%s] CBMix Fail %d Money : %d-%d, CharmRate : %d",
			Obj.AccountID, Obj.Name, Obj.ChaosSuccessRate, Obj.m_PlayerData->Money, nChaosNeedMoney, LuckyHarmAddRate);
	}

	Obj.ChaosLock = FALSE;
}

void CMixSystem::SummonsMix(CGameObject &Obj, int type)
{
	Obj.ChaosLock = true;

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	int SummonScroll1Count = 0;
	int SummonScroll2Count = 0;
	int SummonScroll3Count = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 210))
		{
			type = 1;
			SummonScroll1Count++;
		}
		else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 211))
		{
			type = 2;
			SummonScroll2Count++;
		}
		else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 212))
		{
			type = 3;
			SummonScroll3Count++;
		}
	}

	if (type == 0 || (type == 1 && SummonScroll1Count != 2) || (type == 2 && (SummonScroll1Count != 1 || SummonScroll2Count != 1)) || (type == 3 && (SummonScroll1Count != 1 || SummonScroll3Count != 1)))
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	switch (type)
	{
	case 1:
		Obj.ChaosSuccessRate = g_ConfigRead.g_iSummonScrollMix1;
		Obj.ChaosMoney = g_ConfigRead.g_iSummonScrollMix1Zen;//100000;
		break;
	case 2:
		Obj.ChaosSuccessRate = g_ConfigRead.g_iSummonScrollMix2;
		Obj.ChaosMoney = g_ConfigRead.g_iSummonScrollMix2Zen;//200000;
		break;
	case 3:
		Obj.ChaosSuccessRate = g_ConfigRead.g_iSummonScrollMix3;
		Obj.ChaosMoney = g_ConfigRead.g_iSummonScrollMix3Zen;//300000;
		break;
	}

	if (this->GetTalismanOfLuckRate(lpObj, &Obj.ChaosSuccessRate) == 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_NOT_ENOUGH_EMPTY_SPACEX;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	Obj.ChaosSuccessRate = ((Obj.ChaosSuccessRate > 100) ? 100 : Obj.ChaosSuccessRate);

	int TaxMoney = (Obj.ChaosMoney*g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index)) / 100;

	Obj.ChaosMoney += TaxMoney;

	if (Obj.m_PlayerData->Money < ((DWORD)Obj.ChaosMoney))
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	Obj.m_PlayerData->Money -= Obj.ChaosMoney;

	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	g_CastleSiegeSync.AddTributeMoney(TaxMoney);

	if ((GetLargeRand() % 100) < Obj.ChaosSuccessRate)
	{
		ItemCreate(Obj.m_Index, 0xFF, 0, 0, (ITEMGET(14, 210) + type), 0, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0xFF);

		Obj.ChaosLock = TRUE;
		sLog->outBasic("[SummonScrollMix][Success][%s][%s] - (Type: %d, ChaosSuccessRate: %d, ChaosMoney: %d)", Obj.AccountID, Obj.Name, type, Obj.ChaosSuccessRate, Obj.ChaosMoney);
	}
	else
	{
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);

		Obj.ChaosLock = FALSE;
		return;
	}
}

void CMixSystem::CCFRewardMix(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int BadgeItemCount = 0;
	int ChaosItemCount = 0;
	int JewelOfSoul10BundleCount = 0;
	int JewelOfBless10BundleCount = 0;
	int JewelOfLife10BundleCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[i].m_Type == ITEMGET(12, 15))
			{
				ChaosItemCount++;
			}

			else if (Obj.pChaosBox[i].m_Type == ITEMGET(12, 136))
			{
				JewelOfLife10BundleCount++;
			}

			else if (Obj.pChaosBox[i].m_Type == ITEMGET(12, 30))
			{
				JewelOfBless10BundleCount++;
			}

			else if (Obj.pChaosBox[i].m_Type == ITEMGET(12, 31))
			{
				JewelOfSoul10BundleCount++;
			}

			else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 276))
			{
				BadgeItemCount++;
			}

			else
			{
				InvalidItemCount++;
			}
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (ChaosItemCount != 1 || JewelOfLife10BundleCount != 1 || JewelOfBless10BundleCount != 1 || JewelOfSoul10BundleCount != 1 || BadgeItemCount != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	int nChaosNeedMoney = 10000000;
	int iChaosTaxMoney = nChaosNeedMoney * g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / 100;

	if (iChaosTaxMoney < 0)
		iChaosTaxMoney = 0;

	nChaosNeedMoney += iChaosTaxMoney;
	Obj.ChaosSuccessRate = 100;

	if (Obj.m_PlayerData->Money < nChaosNeedMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));

		Obj.ChaosLock = FALSE;
		return;
	}

	this->LogChaosItem(lpObj, "Chaos Castle Survival Reward Mix");
	sLog->outBasic("[Chaos Castle Survival Reward Mix] Chaos Mix Start");

	Obj.m_PlayerData->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	int iWingNum = ITEMGET(12, 268);

	int iOption1 = 0;
	int iOption2 = 0;
	int iExcOpt = 0;
	BYTE btNewExcOption[5];
	memset(btNewExcOption, -1, sizeof(btNewExcOption));

	iExcOpt = g_ItemOptionTypeMng.WingExcOptionRand(ItemAttribute[iWingNum].ItemKindA, ItemAttribute[iWingNum].ItemKindB, btNewExcOption);

	int iOptTypeRand = rand() % 2;
	int iOptRateRand;

	switch (iOptTypeRand)
	{
	case 0:
		iOptRateRand = rand() % 1000;
		if (iOptRateRand < 400)
		{
			iExcOpt |= 0x10;
		}
		break;

	case 1:
		iOptRateRand = rand() % 1000;
		if (iOptRateRand < 300)
		{
			iExcOpt |= 0x20;
		}
		break;
	}

	int iOpt3TypeLevel = rand() % 4;
	int iOpt3TypeLevelRate = rand() % 1000;

	switch (iOpt3TypeLevel)
	{
	case 1:
	{
		if (iOpt3TypeLevelRate < 120)
		{
			iOption2 = 1;
		}
	}
	break;

	case 2:
	{
		if (iOpt3TypeLevelRate < 60)
		{
			iOption2 = 2;
		}
	}
	break;

	case 3:
	{
		if (iOpt3TypeLevelRate < 30)
		{
			iOption2 = 3;
		}
	}
	break;
	}

	::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iWingNum, 0, 0, 0, iOption1, iOption2, Obj.m_Index, iExcOpt, 0, 0, btNewExcOption, 0);
	::gObjInventoryCommit(Obj.m_Index);
	::sLog->outBasic("[Chaos Castle Survival Reward Mix] [%s][%s] CBMix Success Money : %d-%d",
		Obj.AccountID, Obj.Name, Obj.m_PlayerData->Money, nChaosNeedMoney);

	Obj.ChaosLock = FALSE;
}

void CMixSystem::DSFRewardMix(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int BadgeItemCount = 0;
	int ChaosItemCount = 0;
	int JewelOfSoul10BundleCount = 0;
	int JewelOfBless10BundleCount = 0;
	int JewelOfLife10BundleCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[i].m_Type == ITEMGET(12, 15))
			{
				ChaosItemCount++;
			}

			else if (Obj.pChaosBox[i].m_Type == ITEMGET(12, 136))
			{
				JewelOfLife10BundleCount++;
			}

			else if (Obj.pChaosBox[i].m_Type == ITEMGET(12, 30))
			{
				JewelOfBless10BundleCount++;
			}

			else if (Obj.pChaosBox[i].m_Type == ITEMGET(12, 31))
			{
				JewelOfSoul10BundleCount++;
			}

			else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 277))
			{
				BadgeItemCount++;
			}

			else
			{
				InvalidItemCount++;
			}
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (ChaosItemCount != 1 || JewelOfLife10BundleCount != 1 || JewelOfBless10BundleCount != 1 || JewelOfSoul10BundleCount != 1 || BadgeItemCount != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	int nChaosNeedMoney = 10000000;
	int iChaosTaxMoney = nChaosNeedMoney * g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / 100;

	if (iChaosTaxMoney < 0)
		iChaosTaxMoney = 0;

	nChaosNeedMoney += iChaosTaxMoney;
	Obj.ChaosSuccessRate = 100;

	if (Obj.m_PlayerData->Money < nChaosNeedMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));

		Obj.ChaosLock = FALSE;
		return;
	}

	this->LogChaosItem(lpObj, "DSF Reward Mix");
	sLog->outBasic("[DSF Reward Mix] Chaos Mix Start");

	Obj.m_PlayerData->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	int iItemNum = ITEMGET(13, 268);

	::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
	::gObjInventoryCommit(Obj.m_Index);
	::sLog->outBasic("[DSF Reward Mix] [%s][%s] CBMix Success Money : %d-%d",
		Obj.AccountID, Obj.Name, Obj.m_PlayerData->Money, nChaosNeedMoney);

	Obj.ChaosLock = FALSE;
}

BOOL CMixSystem::CheckHeroSoulItem(int iItemCode)
{
	if (iItemCode == ITEMGET(7, 1) ||
		iItemCode == ITEMGET(8, 1) ||
		iItemCode == ITEMGET(9, 1) ||
		iItemCode == ITEMGET(10, 1) ||
		iItemCode == ITEMGET(11, 1) ||
		iItemCode == ITEMGET(7, 3) ||
		iItemCode == ITEMGET(8, 3) ||
		iItemCode == ITEMGET(9, 3) ||
		iItemCode == ITEMGET(10, 3) ||
		iItemCode == ITEMGET(11, 3) ||
		iItemCode == ITEMGET(7, 14) ||
		iItemCode == ITEMGET(8, 14) ||
		iItemCode == ITEMGET(9, 14) ||
		iItemCode == ITEMGET(10, 14) ||
		iItemCode == ITEMGET(11, 14) ||
		iItemCode == ITEMGET(7, 15) ||
		iItemCode == ITEMGET(8, 15) ||
		iItemCode == ITEMGET(9, 15) ||
		iItemCode == ITEMGET(10, 15) ||
		iItemCode == ITEMGET(11, 15) ||
		iItemCode == ITEMGET(7, 26) ||
		iItemCode == ITEMGET(8, 26) ||
		iItemCode == ITEMGET(9, 26) ||
		iItemCode == ITEMGET(10, 26) ||
		iItemCode == ITEMGET(11, 26) ||
		iItemCode == ITEMGET(7, 40) ||
		iItemCode == ITEMGET(8, 40) ||
		iItemCode == ITEMGET(9, 40) ||
		iItemCode == ITEMGET(10, 40) ||
		iItemCode == ITEMGET(11, 40) ||
		iItemCode == ITEMGET(7, 59) ||
		iItemCode == ITEMGET(8, 59) ||
		iItemCode == ITEMGET(9, 59) ||
		iItemCode == ITEMGET(0, 32) ||
		iItemCode == ITEMGET(11, 59) ||
		iItemCode == ITEMGET(7, 94) ||
		iItemCode == ITEMGET(8, 94) ||
		iItemCode == ITEMGET(9, 94) ||
		iItemCode == ITEMGET(10, 94) ||
		iItemCode == ITEMGET(11, 94) ||
		iItemCode == ITEMGET(7, 95) ||
		iItemCode == ITEMGET(8, 95) ||
		iItemCode == ITEMGET(9, 95) ||
		iItemCode == ITEMGET(10, 95) ||
		iItemCode == ITEMGET(11, 95))
	{
		return true;
	}
	return false;
}

void CMixSystem::AncientSoulItemMix(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int SetItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == TRUE)
		{
			if (this->CheckHeroSoulItem(Obj.pChaosBox[i].m_Type) != FALSE)
			{
				if (this->CheckLevelCondition(&Obj.pChaosBox[i], 11, 0, 0, 0, 1, 0) != FALSE)
				{
					SetItemCount++;
				}
			}
			else
			{
				InvalidItemCount++;
			}
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SetItemCount != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	Obj.ChaosSuccessRate = 80;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(13, 407);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}

void CMixSystem::BloodAngelUnSealMix(CGameObject &Obj, int iMixType)
{
	Obj.ChaosLock = TRUE;

	int SealedItemCount = 0;
	int AncientHeroSoulCount = 0;
	int InvalidItemCount = 0;

	int iNeedItemType = -1;
	int iResultItemType = -1;
	bool bMixFound = false;

	bool HeroMIX = false;
	int SetItemCount = 0;
	int InvalidItemCount1 = 0;
	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	for (int i = 0; i < BLOODANGEL_MIX_LIST_SIZE; i++)
	{
		if (g_BloodAngelMixList[i].iMixType == iMixType)
		{
			iNeedItemType = g_BloodAngelMixList[i].iNeedItemType;
			iResultItemType = g_BloodAngelMixList[i].iResultItemType;
			bMixFound = true;
			HeroMIX = false;
			break;
		}
		else if (Obj.pChaosBox[i].IsItem() == TRUE)
		{
			if (GetItemKindA(Obj.pChaosBox[i].m_Type) == ITEM_KIND_A_ARMOR &&
				Obj.pChaosBox[i].IsSetItem() == TRUE &&
				g_LuckyItemManager.IsLuckyItemEquipment(Obj.pChaosBox[i].m_Type) == FALSE &&
				IsBloodAngelItem(Obj.pChaosBox[i].m_Type) == FALSE)
			{
				SetItemCount++;
				HeroMIX = true;
				bMixFound = true;
				break;
			}
		}

	}

	if (bMixFound == false)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	if (HeroMIX)
	{
		Obj.ChaosSuccessRate = 80;

		if (rand() % 100 < Obj.ChaosSuccessRate)
		{
			int iItemNum = ITEMGET(13, 407);
			::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
			::gObjInventoryCommit(Obj.m_Index);
		}

		else
		{
			this->ChaosBoxInit(lpObj);
			gGameProtocol.GCUserChaosBoxSend(lpObj, 0);

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

		}

		Obj.ChaosLock = FALSE;
	}
	else
	{
		for (int i = 0; i < CHAOS_BOX_SIZE; i++)
		{
			if (Obj.pChaosBox[i].IsItem() == TRUE)
			{
				if (Obj.pChaosBox[i].m_Type == iNeedItemType)
				{
					SealedItemCount++;
				}

				else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
				{
					AncientHeroSoulCount++;
				}

				else
				{
					InvalidItemCount++;
				}
			}
		}

		if (SealedItemCount != 1 || AncientHeroSoulCount != 1 || InvalidItemCount != 0)
		{
			Obj.ChaosLock = FALSE;
			pMsg.Result = CB_INCORRECT_MIX_ITEMS;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
			return;
		}

		Obj.ChaosSuccessRate = 100;

		int iItemType = iResultItemType;
		int iSetOption = gSetItemOption.GenSetOption(iItemType);
		int iOption2 = 0;

		if ((rand() % 5) == 0)
		{
			iOption2 = 1;
		}

		BYTE Socks[5];
		Socks[0xFF, 0xFF, 0xFF, 0xFF, 0xFF];
		::ItemCreate(Obj.m_Index, 255, 0, 0, iItemType, 0, 0, 0, iOption2, 0, Obj.m_Index, 0, iSetOption, 0, Socks, 0);
		::gObjInventoryCommit(Obj.m_Index);
	}

	Obj.ChaosLock = FALSE;
}

void CMixSystem::BloodAngelUnSealMix2(CGameObject &Obj, int iMixType)
{
	Obj.ChaosLock = TRUE;

	int SealedItemCount = 0;
	int AncientHeroSoulCount = 0;
	int InvalidItemCount = 0;

	int iNeedItemType = -1;
	int iResultItemType = -1;
	bool bMixFound = false;

	bool HeroMIX = false;
	int SetItemCount = 0;
	int InvalidItemCount1 = 0;
	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	for (int i = 0; i < BLOODANGEL_MIX_LIST_SIZE2; i++)
	{
		if (g_BloodAngelMixList2[i].iMixType == iMixType)
		{
			iNeedItemType = g_BloodAngelMixList2[i].iNeedItemType;
			iResultItemType = g_BloodAngelMixList2[i].iResultItemType;
			bMixFound = true;
			HeroMIX = false;
			break;
		}
		else if (Obj.pChaosBox[i].IsItem() == TRUE)
		{
			if (GetItemKindA(Obj.pChaosBox[i].m_Type) == ITEM_KIND_A_ARMOR &&
				Obj.pChaosBox[i].IsSetItem() == TRUE &&
				g_LuckyItemManager.IsLuckyItemEquipment(Obj.pChaosBox[i].m_Type) == FALSE &&
				IsBloodAngelItem(Obj.pChaosBox[i].m_Type) == FALSE)
			{
				SetItemCount++;
				HeroMIX = true;
				bMixFound = true;
				break;
			}
		}

	}

	if (bMixFound == false)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}
	if (HeroMIX)
	{
		Obj.ChaosSuccessRate = 80;

		if (rand() % 100 < Obj.ChaosSuccessRate)
		{
			int iItemNum = ITEMGET(14, 399);
			::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 11, 0, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
			::gObjInventoryCommit(Obj.m_Index);
		}

		else
		{
			this->ChaosBoxInit(lpObj);
			gGameProtocol.GCUserChaosBoxSend(lpObj, 0);

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

		}

		Obj.ChaosLock = FALSE;
	}
	else
	{
		for (int i = 0; i < CHAOS_BOX_SIZE; i++)
		{
			if (Obj.pChaosBox[i].IsItem() == TRUE)
			{
				if (Obj.pChaosBox[i].m_Type == iNeedItemType)
				{
					SealedItemCount++;
				}

				else if (Obj.pChaosBox[i].m_Type == ITEMGET(14, 399))
				{
					AncientHeroSoulCount++;
				}

				else
				{
					InvalidItemCount++;
				}
			}
		}

		if (SealedItemCount != 1 || AncientHeroSoulCount != 1 || InvalidItemCount != 0)
		{
			Obj.ChaosLock = FALSE;
			pMsg.Result = CB_INCORRECT_MIX_ITEMS;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
			return;
		}

		Obj.ChaosSuccessRate = 100;

		int iItemType = iResultItemType;
		int iSetOption = gSetItemOption.GenSetOption(iItemType);
		int iOption2 = 0;

		if ((rand() % 5) == 0)
		{
			iOption2 = 1;
		}

		BYTE Socks[5];
		Socks[0xFF, 0xFF, 0xFF, 0xFF, 0xFF];
		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemType, 11, 0, 0, iOption2, 0, Obj.m_Index, 0, iSetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);
		return;
	}

	Obj.ChaosLock = FALSE;
}

void CMixSystem::DarkAngelMix(CGameObject &Obj, int iMixType)
{
	Obj.ChaosLock = TRUE;

	int SealedItemCount = 0;

	int InvalidItemCount = 0;

	int iNeedItemType = -1;
	int iResultItemType = -1;
	bool bMixFound = false;

	bool HeroMIX = false;
	int SetItemCount = 0;
	int InvalidItemCount1 = 0;

	int ChoasGemCount = 0;
	int StoneShardCount = 0;
	int BundleOfSoulCount = 0;
	int BundleOfBlessCount = 0;
	int JewelOfCreationCount = 0;
	int AncientBACount = 0;
	int m_type = 0;
	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[i].m_Type == this->CHAOS_GEM_ITEMINDEX) // Chaos
			{
				ChoasGemCount++;
			}
			else if (Obj.pChaosBox[i].m_Type == this->SOUL_GEM_BULDLE_ITEMINDEX) // Bundle of Soul
			{
				BundleOfSoulCount++;
			}
			else if (Obj.pChaosBox[i].m_Type == this->BLESS_GEM_BUNDLE_ITEMINDEX) // Bundle of Bless
			{
				BundleOfBlessCount++;
			}
			else if (Obj.pChaosBox[i].m_Type == this->CREATURE_GEM_ITEMINDEX) // Jewel of Creation
			{
				JewelOfCreationCount++;
			}
			else if (Obj.pChaosBox[i].m_Type == ITEMGET(14, 341)) //Stone Shard
			{
				StoneShardCount++;
			}
			else if (this->CheckLevelCondition(&Obj.pChaosBox[i], 11, 0, 0, 0, 1, 0) != FALSE)
			{
				//sLog->outBasic("[CheckLevelCondition %d", Obj.pChaosBox[i].m_Type);
				m_type = Obj.pChaosBox[i].m_Type;
				AncientBACount++;
				//iChaosMoney += Obj.pChaosBox[n].m_BuyMoney;
			}
			else
			{
				InvalidItemCount++;
			}
		}
	}

	if (ChoasGemCount != 1 || BundleOfSoulCount != 1 || BundleOfBlessCount != 1 || StoneShardCount != 1 || JewelOfCreationCount != 1 || AncientBACount < 1)
	{
		pMsg.Result = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	Obj.ChaosSuccessRate = 100;

	int iItemType = m_type + 40;
	int iSetOption = gSetItemOption.GenSetOption(iItemType);
	BYTE ItemOption2 = 0;

	gItemOptionRate.GetItemOption2(61, &ItemOption2);

	BYTE Socks[5];
	Socks[0xFF, 0xFF, 0xFF, 0xFF, 0xFF];
	::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemType, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, iSetOption, 0, Socks, 0);
	::gObjInventoryCommit(Obj.m_Index);

	if (iMixType == 0x8F) //DK DA Armor
	{

	}
}

void CMixSystem::DarkAngelNextMix(CGameObject &Obj, int iMixType)
{
	Obj.ChaosLock = TRUE;

	int SealedItemCount = 0;

	int InvalidItemCount = 0;

	int iNeedItemType = -1;
	int iResultItemType = -1;
	bool bMixFound = false;

	bool HeroMIX = false;
	int SetItemCount = 0;
	int InvalidItemCount1 = 0;

	int ChoasGemCount = 0;
	int StoneShardCount = 0;
	int BundleOfSoulCount = 0;
	int BundleOfBlessCount = 0;
	int JewelOfCreationCount = 0;
	int AncientBACount = 0;
	int m_type = 0;
	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[i].m_Type == this->CHAOS_GEM_ITEMINDEX) // Chaos
			{
				ChoasGemCount++;
			}
			else if (Obj.pChaosBox[i].m_Type == this->SOUL_GEM_BULDLE_ITEMINDEX) // Bundle of Soul
			{
				BundleOfSoulCount++;
			}
			else if (Obj.pChaosBox[i].m_Type == this->BLESS_GEM_BUNDLE_ITEMINDEX) // Bundle of Bless
			{
				BundleOfBlessCount++;
			}
			else if (Obj.pChaosBox[i].m_Type == this->CREATURE_GEM_ITEMINDEX) // Jewel of Creation
			{
				JewelOfCreationCount++;
			}
			else if (Obj.pChaosBox[i].m_Type == ITEMGET(14, 389)) //Stone Shard
			{
				StoneShardCount++;
			}
			else if (this->CheckLevelCondition(&Obj.pChaosBox[i], 11, 0, 0, 0, 1, 0) != FALSE)
			{
				//sLog->outBasic("[CheckLevelCondition %d", Obj.pChaosBox[i].m_Type);
				m_type = Obj.pChaosBox[i].m_Type;
				AncientBACount++;
				//iChaosMoney += Obj.pChaosBox[n].m_BuyMoney;
			}
			else
			{
				InvalidItemCount++;
			}
		}
	}



	if (ChoasGemCount != 1 || BundleOfSoulCount != 1 || BundleOfBlessCount != 1 || StoneShardCount != 1 || JewelOfCreationCount != 1 || AncientBACount < 1)
	{
		pMsg.Result = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	Obj.ChaosSuccessRate = 100;

	int iItemType = m_type + 12;
	int iSetOption = gSetItemOption.GenSetOption(iItemType);
	int iOption2 = 0;

	if ((rand() % 5) == 0)
	{
		iOption2 = 1;
	}

	BYTE Socks[5];
	Socks[0xFF, 0xFF, 0xFF, 0xFF, 0xFF];
	::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemType, 0, 0, 0, iOption2, 0, Obj.m_Index, 0, iSetOption, 0, Socks, 0);
	::gObjInventoryCommit(Obj.m_Index);

	if (iMixType == 0x8F) //DK DA Armor
	{

	}
}

void CMixSystem::ArchangelHammerMix(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int SealedItemCount = 0;

	int InvalidItemCount = 0;

	int iNeedItemType = -1;
	int iResultItemType = -1;
	bool bMixFound = false;

	bool HeroMIX = false;
	int SetItemCount = 0;
	int InvalidItemCount1 = 0;

	int ChoasGemCount = 0;
	int StoneShardCount = 0;
	int BundleOfSoulCount = 0;
	int BundleOfBlessCount = 0;
	int JewelOfCreationCount = 0;
	int JewelOfGuardianCount = 0;
	int SteelCount = 0;
	int m_type = 0;
	int	m_typenew = 0;
	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;
	BYTE btNewOption = 0;
	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[i].m_Type == this->CHAOS_GEM_ITEMINDEX) // Chaos
			{
				ChoasGemCount++;
			}
			else if (Obj.pChaosBox[i].m_Type == this->SOUL_GEM_BULDLE_ITEMINDEX) // Bundle of Soul
			{
				BundleOfSoulCount++;
			}
			else if (Obj.pChaosBox[i].m_Type == this->BLESS_GEM_BUNDLE_ITEMINDEX) // Bundle of Bless
			{
				BundleOfBlessCount++;
			}
			else if (Obj.pChaosBox[i].m_Type == this->CREATURE_GEM_ITEMINDEX) // Jewel of Creation
			{
				JewelOfCreationCount++;
			}
			else if (Obj.pChaosBox[i].m_Type == this->GUARDIAN_GEM_ITEMINDEX) //Jewel of Guardian
			{
				JewelOfGuardianCount++;
			}

			else if ((int)Obj.pChaosBox[i].m_Durability == 10 && Obj.pChaosBox[i].m_Type == ITEMGET(14, 342))
			{

				m_type = Obj.pChaosBox[i].m_Type;
				m_typenew = m_type + 1;
				SteelCount++;
			}
			else
			{
				InvalidItemCount++;
			}
		}
	}

	ITEM_ATTRIBUTE * p = &ItemAttribute[m_typenew];

	if (ChoasGemCount != 1 || BundleOfSoulCount != 1 || BundleOfBlessCount != 1 || JewelOfCreationCount != 1 || JewelOfGuardianCount != 1 || SteelCount < 1)
	{
		pMsg.Result = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	if (InvalidItemCount > 0)
	{
		pMsg.Result = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}
	Obj.ChaosSuccessRate = 100;
	BYTE Socks[5];
	Socks[0xFF, 0xFF, 0xFF, 0xFF, 0xFF];
	BYTE btNewExcOption[5];

	::ItemCreate(Obj.m_Index, 0xFF, 0, 0, m_typenew, 0, 0, 1, 0, 0, Obj.m_Index, 0, 0, 0, Socks, 0);
	::gObjInventoryCommit(Obj.m_Index);
}

void CMixSystem::BlessedAAMix(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int SealedItemCount = 0;

	int InvalidItemCount = 0;

	int iNeedItemType = -1;
	int iResultItemType = -1;
	bool bMixFound = false;

	bool HeroMIX = false;
	int SetItemCount = 0;
	int InvalidItemCount1 = 0;

	int ChoasGemCount = 0;
	int StoneShardCount = 0;
	int BundleOfSoulCount = 0;
	int BundleOfBlessCount = 0;
	int HammerCount = 0;
	int m_type = 0;
	int	m_typenew = 0;
	int AACount = 0;
	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;
	BYTE btNewOption = 0;
	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[i].m_Type == this->CHAOS_GEM_ITEMINDEX) // Chaos
			{
				ChoasGemCount++;
			}
			else if (Obj.pChaosBox[i].m_Type == this->SOUL_GEM_BULDLE_ITEMINDEX) // Bundle of Soul
			{
				BundleOfSoulCount++;
			}
			else if (Obj.pChaosBox[i].m_Type == this->BLESS_GEM_BUNDLE_ITEMINDEX) // Bundle of Bless
			{
				BundleOfBlessCount++;
			}
			else if (Obj.pChaosBox[i].m_Type == ITEMGET(14, 343))
			{
				HammerCount++;
			}
			else if (Obj.pChaosBox[i].m_Type == ITEMGET(14, 343))
			{

				m_type = Obj.pChaosBox[i].m_Type;
				m_typenew = m_type + 1;
				HammerCount++;
			}
			switch (Obj.pChaosBox[i].m_Type)
			{
			case ITEMGET(0, 19): //SWORD
			{
				m_type = Obj.pChaosBox[i].m_Type;
				if (this->CheckLevelCondition(&Obj.pChaosBox[i], 15, 0, 0, 1, 0, 0) != FALSE)
				{
					m_typenew = m_type + 32;
					btNewOption = Obj.pChaosBox[i].m_NewOption;
					AACount++;
				}
				break;
			}
			case ITEMGET(2, 13): //Scepter
			{
				m_type = Obj.pChaosBox[i].m_Type;
				if (this->CheckLevelCondition(&Obj.pChaosBox[i], 15, 0, 0, 1, 0, 0) != FALSE)
				{
					m_typenew = m_type + 12;
					btNewOption = Obj.pChaosBox[i].m_NewOption;
					AACount++;
				}
				break;
			}
			case ITEMGET(4, 18): //Bow
			{
				m_type = Obj.pChaosBox[i].m_Type;
				if (this->CheckLevelCondition(&Obj.pChaosBox[i], 15, 0, 0, 1, 0, 0) != FALSE)
				{
					m_typenew = m_type + 12;
					btNewOption = Obj.pChaosBox[i].m_NewOption;
					AACount++;
				}
				break;
			}
			case ITEMGET(5, 10): //Staff
			{
				m_type = Obj.pChaosBox[i].m_Type;
				if (this->CheckLevelCondition(&Obj.pChaosBox[i], 15, 0, 0, 1, 0, 0) != FALSE)
				{
					m_typenew = m_type + 39;
					btNewOption = Obj.pChaosBox[i].m_NewOption;
					AACount++;
				}
				break;
			}
			case ITEMGET(5, 36): //Stick
			{
				m_type = Obj.pChaosBox[i].m_Type;
				if (this->CheckLevelCondition(&Obj.pChaosBox[i], 15, 0, 0, 1, 0, 0) != FALSE)
				{
					m_typenew = m_type + 14;
					btNewOption = Obj.pChaosBox[i].m_NewOption;
					AACount++;
				}
				break;
			}

			}
		}
	}

	ITEM_ATTRIBUTE * p = &ItemAttribute[m_typenew];

	if (ChoasGemCount != 1 || BundleOfSoulCount != 1 || BundleOfBlessCount != 1 || AACount != 1 || HammerCount < 1)
	{
		pMsg.Result = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		sLog->outBasic("[BlessedWeaponMix] [%s][%s] CBMix Error", Obj.AccountID, Obj.Name);
		Obj.ChaosLock = FALSE;
		return;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		BYTE Socks[5];
		Socks[0xFF, 0xFF, 0xFF, 0xFF, 0xFF];
		BYTE btNewExcOption[5];
		//btNewOption = g_ItemOptionTypeMng.CommonExcOptionRand(p->ItemKindA, btNewExcOption);
		//btNewOption = g_ItemOptionTypeMng.CommonExcOptionGetCount(p->ItemKindA, btNewExcOption, this->m_stCCFFinalReward.m_btNewOption[1]);
		::ItemCreate(Obj.m_Index, -1, 0, 0, m_typenew, 0, 0, 1, 1, 4, Obj.m_Index, btNewOption, 0, 0, Socks, 0);
		::gObjInventoryCommit(Obj.m_Index);
		sLog->outBasic("[BlessedWeaponMix] [%s][%s] CBMix Success %d %d", Obj.AccountID, Obj.Name, Obj.ChaosSuccessRate, btNewOption);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;

}

void CMixSystem::DarkAngelWeaponMix(CGameObject &Obj, int iMixType)
{
	Obj.ChaosLock = TRUE;

	int SealedItemCount = 0;

	int InvalidItemCount = 0;

	int iNeedItemType = -1;
	int iResultItemType = -1;
	bool bMixFound = false;

	bool HeroMIX = false;
	int SetItemCount = 0;
	int InvalidItemCount1 = 0;

	int ChoasGemCount = 0;
	int StoneShardCount = 0;
	int BundleOfSoulCount = 0;
	int BundleOfBlessCount = 0;
	int JewelOfCreationCount = 0;
	int AncientBACount = 0;
	int m_type = 0;
	int	m_typenew = 0;
	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;
	BYTE btNewOption = 0;
	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[i].m_Type == this->CHAOS_GEM_ITEMINDEX) // Chaos
			{
				ChoasGemCount++;
			}
			else if (Obj.pChaosBox[i].m_Type == this->SOUL_GEM_BULDLE_ITEMINDEX) // Bundle of Soul
			{
				BundleOfSoulCount++;
			}
			else if (Obj.pChaosBox[i].m_Type == this->BLESS_GEM_BUNDLE_ITEMINDEX) // Bundle of Bless
			{
				BundleOfBlessCount++;
			}
			else if (Obj.pChaosBox[i].m_Type == this->CREATURE_GEM_ITEMINDEX) // Jewel of Creation
			{
				JewelOfCreationCount++;
			}
			else if (Obj.pChaosBox[i].m_Type == ITEMGET(14, 386)) //Stone Shard
			{
				StoneShardCount++;
			}

			else if (this->CheckLevelCondition(&Obj.pChaosBox[i], 11, 0, 0, 1, 0, 1) != FALSE)
			{
				sLog->outBasic("[CheckLevelCondition %d %d", Obj.pChaosBox[i].m_Option3, Obj.pChaosBox[i].m_NewOption);

				m_type = Obj.pChaosBox[i].m_Type;
				if (m_type == ITEMGET(5, 41)) //Staff ok 0xCE
					m_typenew = m_type + 10;
				if (m_type == ITEMGET(5, 43)) //Stick ok 0xCF
					m_typenew = m_type + 9;

				if (m_type == ITEMGET(0, 42)) //Bloodangel Sword 0xC8
					m_typenew = m_type + 12;
				if (m_type == ITEMGET(0, 44)) //Bloodangel Magic Sword 0xC9
					m_typenew = m_type + 11;
				if (m_type == ITEMGET(0, 46)) //Bloodangel Claw 0xCA
					m_typenew = m_type + 10;

				if (m_type == ITEMGET(3, 19)) //Bloodangel Lance 0xCC
					m_typenew = m_type + 5;
				if (m_type == ITEMGET(2, 22)) //Bloodangel Scepter 	//0xCB
					m_typenew = m_type + 4;
				if (m_type == ITEMGET(4, 28)) //Bloodangel Bow 0xCD
					m_typenew = m_type + 3;
				if (m_type == ITEMGET(4, 34)) //Bloodangel Quiver 0x40
					m_typenew = m_type + 1;
				//sLog->outBasic("[ CheckLevelCondition %d", m_typenew);
				btNewOption = Obj.pChaosBox[i].m_NewOption;
				AncientBACount++;
				//iChaosMoney += Obj.pChaosBox[n].m_BuyMoney;
			}
			else
			{
				sLog->outBasic("[CheckLevelCondition %d %d", Obj.pChaosBox[i].m_Option3, Obj.pChaosBox[i].m_NewOption);
				InvalidItemCount++;
			}
		}
	}

	ITEM_ATTRIBUTE * p = &ItemAttribute[m_typenew];

	if (ChoasGemCount != 1 || BundleOfSoulCount != 1 || BundleOfBlessCount != 1 || StoneShardCount != 1 || JewelOfCreationCount != 1 || AncientBACount < 1)
	{
		pMsg.Result = 0;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		sLog->outBasic("[DarkAngel Weapon] [%s][%s] CBMix Error", Obj.AccountID, Obj.Name);
		Obj.ChaosLock = FALSE;
		return;
	}

	Obj.ChaosSuccessRate = 100;

	//int iItemType = m_typenew;
	int iSetOption = 0;
	int iOption2 = 0;
	int iOption3 = 0;
	if ((rand() % 5) == 0)
	{
		iOption2 = 1;
	}
	if ((rand() % 5) == 0)
	{
		iOption3 = 1;
	}
	BYTE Socks[5];
	Socks[0xFF, 0xFF, 0xFF, 0xFF, 0xFF];
	BYTE btNewExcOption[5];

	::ItemCreate(Obj.m_Index, 0xFF, 0, 0, m_typenew, 0, 0, 1, iOption2, iOption3, Obj.m_Index, btNewOption, 0, 0, Socks, 0);
	::gObjInventoryCommit(Obj.m_Index);
	sLog->outBasic("[DarkAngel Weapon] [%s][%s] CBMix Success %d", Obj.AccountID, Obj.Name, Obj.ChaosSuccessRate);
}

void CMixSystem::CheckEmptySpace_MultiMix(PMSG_REQ_CHAOS_MULTIMIX_CHECK * aRecv, CGameObject &Obj)
{
	if (!gObjIsConnectedGP(Obj.m_Index))
	{
		return;
	}

	CItemObject MixItem;

	switch (aRecv->btMixType)
	{
	case CHAOS_TYPE_FRUIT:
		MixItem.m_Type = this->FRUIT_ITEMINDEX;
		break;
	case CHAOS_TYPE_DINORANT:
		MixItem.m_Type = this->DINORANT_ITEMINDEX;
		break;
	case CHAOS_TYPE_BLOODCATLE:
		MixItem.m_Type = this->BLOODCASTLE_INVITATION_ITEMINDEX;
		break;
	case CHAOS_TYPE_DEVILSQUARE:
		MixItem.m_Type = this->DEVILSQUARE_INVITATION_ITEMINDEX;
		break;
	case CHAOS_TYPE_COMPOUNDPOTION_LV1:
		MixItem.m_Type = this->SMALL_SD_POTION_ITEMINDEX;
		break;
	case CHAOS_TYPE_COMPOUNTPOTION_LV2:
		MixItem.m_Type = this->MEDIUM_SD_POTION_ITEMINDEX;
		break;
	case CHAOS_TYPE_COMPOUNTPOTION_LV3:
		MixItem.m_Type = this->LARGE_SD_POTION_ITEMINDEX;
		break;
	case CHAOS_TYPE_BLESS_POTION:
		MixItem.m_Type = this->BLESS_POTION_ITEMINDEX;
		break;
	case CHAOS_TYPE_SOUL_POTION:
		MixItem.m_Type = this->SOUL_POTION_ITEMINDEX;
		break;
	case CHAOS_TYPE_PREMIUM_BOX:
		MixItem.m_Type = this->GOLDEN_BOX_ITEMINDEX;
		break;
	case CHAOS_TYPE_JEWELOFHARMONY_MIX_SMELTINGITEM:
		MixItem.m_Type = this->HIGH_REFINE_STONE_ITEMINDEX;
		break;
	case CHAOS_TYPE_JEWELOFHARMONY_PURITY:
		MixItem.m_Type = this->HARMONY_JEWEL_ITEMINDEX;
		break;
	}

	bool bResult = gObjChaosBoxPutItemTest(Obj.m_Index, MixItem, aRecv->btMixCount);

	PMSG_ANS_CHAOS_MULTIMIX_CHECK pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x88, sizeof(pMsg));

	pMsg.btResult = bResult;
	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}

void CMixSystem::BlessPotionChaosMix_Multi(CGameObject &Obj, int iMixCount)
{
	Obj.ChaosLock = TRUE;
	int iBlessGemCount = 0;
	DWORD iChaosMoney = 0;
	int iInvalidItemCount = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 13))
			{
				iBlessGemCount++;
			}
			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if (iBlessGemCount == 0)
		return;

	if (iBlessGemCount != iMixCount)
		return;

	if (iInvalidItemCount > 0)
		return;

	if (iBlessGemCount > 25)
	{
		MsgOutput(Obj.m_Index, Lang.GetText(0, 182));
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	LogChaosItem(lpObj, "BlessPotionMix");
	sLog->outBasic("[BlessPotionMix] Chaos Mix Start");
	Obj.ChaosSuccessRate = 100;

	int nChaosNeedMoney = 100000 * iMixCount;

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if (nChaosNeedMoney < 0)
	{
		nChaosNeedMoney = 0;
	}

	if (Obj.m_PlayerData->Money < nChaosNeedMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	Obj.m_PlayerData->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	pMsg.Result = CB_MULTIMIX_RESULT;
	pMsg.Pos = iMixCount;
	memset(pMsg.ItemInfo, 0x00, sizeof(pMsg.ItemInfo));

	Obj.ChaosMassMixCurrItem = 0;
	Obj.ChaosMassMixSuccessCount = 0;

	for (int i = 0; i < iMixCount; i++)
	{
		if ((rand() % 100) < Obj.ChaosSuccessRate)
		{
			BYTE btMixNumber = 1 << (i % 8);
			pMsg.ItemInfo[i / 8] |= btMixNumber;

			Obj.ChaosMassMixSuccessCount++;
			sLog->outBasic("[BlessPotionMix] [%s][%s] Mass Combination ID:%d Success %d Money : %d-%d",
				Obj.AccountID, Obj.Name, i, Obj.ChaosSuccessRate, Obj.m_PlayerData->Money,
				nChaosNeedMoney);
		}

		else
		{
			sLog->outBasic("[BlessPotionMix] [%s][%s] Mass Combination ID:%d Fail %d Money : %d-%d",
				Obj.AccountID, Obj.Name, i, Obj.ChaosSuccessRate, Obj.m_PlayerData->Money,
				nChaosNeedMoney);
		}
	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

	if (Obj.ChaosMassMixSuccessCount == 0)
	{
		ChaosBoxInit(lpObj);
		::gGameProtocol.GCUserChaosBoxSend(lpObj, 1);

		pMsg.Result = CB_ERROR;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);

		sLog->outBasic("[BlessPotionMix] [%s][%s] CBMix Fail %d Money : %d-%d",
			Obj.AccountID, Obj.Name, Obj.ChaosSuccessRate,
			Obj.m_PlayerData->Money, nChaosNeedMoney);

		Obj.ChaosLock = FALSE;
	}

	else
	{
		for (int i = 0; i < Obj.ChaosMassMixSuccessCount; i++)
		{
			int ItemNum = ITEMGET(14, 7);
			int Level = 0;
			int Dur = 10;

			ItemCreate(Obj.m_Index, 0xFF, 0, 0, ItemNum, Level, Dur, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
		}

		gObjInventoryCommit(Obj.m_Index);
	}
}

void CMixSystem::SoulPotionChaosMix_Multi(CGameObject &Obj, int iMixCount)
{
	Obj.ChaosLock = TRUE;
	int iSoulGemCount = 0;
	DWORD iChaosMoney = 0;
	int iInvalidItemCount = 0;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 14))
			{
				iSoulGemCount++;
			}
			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if (iSoulGemCount == 0)
		return;

	if (iSoulGemCount != iMixCount)
		return;

	if (iInvalidItemCount > 0)
		return;

	if (iSoulGemCount > 25)
	{
		MsgOutput(Obj.m_Index, Lang.GetText(0, 182));
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	LogChaosItem(lpObj, "SoulPotionMix");
	sLog->outBasic("[SoulPotionMix] Chaos Mix Start");
	Obj.ChaosSuccessRate = 100;

	int nChaosNeedMoney = 50000 * iMixCount;

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if (nChaosNeedMoney < 0)
	{
		nChaosNeedMoney = 0;
	}

	if (Obj.m_PlayerData->Money < nChaosNeedMoney)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	Obj.m_PlayerData->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	pMsg.Result = CB_MULTIMIX_RESULT;
	pMsg.Pos = iMixCount;
	memset(pMsg.ItemInfo, 0x00, sizeof(pMsg.ItemInfo));

	Obj.ChaosMassMixCurrItem = 0;
	Obj.ChaosMassMixSuccessCount = 0;

	for (int i = 0; i < iMixCount; i++)
	{
		if ((rand() % 100) < Obj.ChaosSuccessRate)
		{
			BYTE btMixNumber = 1 << (i % 8);
			pMsg.ItemInfo[i / 8] |= btMixNumber;

			Obj.ChaosMassMixSuccessCount++;
			sLog->outBasic("[SoulPotionMix] [%s][%s] Mass Combination ID:%d Success %d Money : %d-%d",
				Obj.AccountID, Obj.Name, i, Obj.ChaosSuccessRate, Obj.m_PlayerData->Money,
				nChaosNeedMoney);
		}

		else
		{
			sLog->outBasic("[SoulPotionMix] [%s][%s] Mass Combination ID:%d Fail %d Money : %d-%d",
				Obj.AccountID, Obj.Name, i, Obj.ChaosSuccessRate, Obj.m_PlayerData->Money,
				nChaosNeedMoney);
		}
	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

	if (Obj.ChaosMassMixSuccessCount == 0)
	{
		ChaosBoxInit(lpObj);
		::gGameProtocol.GCUserChaosBoxSend(lpObj, 1);

		pMsg.Result = CB_ERROR;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, pMsg.h.size);

		sLog->outBasic("[SoulPotionMix] [%s][%s] CBMix Fail %d Money : %d-%d",
			Obj.AccountID, Obj.Name, Obj.ChaosSuccessRate,
			Obj.m_PlayerData->Money, nChaosNeedMoney);

		Obj.ChaosLock = FALSE;
	}

	else
	{
		for (int i = 0; i < Obj.ChaosMassMixSuccessCount; i++)
		{
			int ItemNum = ITEMGET(14, 7);
			int Level = 1;
			int Dur = 10;

			ItemCreate(Obj.m_Index, 0xFF, 0, 0, ItemNum, Level, Dur, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
		}

		gObjInventoryCommit(Obj.m_Index);
	}
}

void CMixSystem::CircleChaosMix_Multi(CGameObject &Obj, int iMixCount)	// Fruits Mass Mix
{
	Obj.ChaosLock = TRUE;

	int CreatureGemCount = 0;
	int ChoasGemCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 22))	// Jewel of Creation
			{
				CreatureGemCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 15)) // Chaos Gem
			{
				ChoasGemCount++;
			}
		}
	}

	if (CreatureGemCount != iMixCount || ChoasGemCount != iMixCount)
	{
		return;
	}

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	if (Obj.Level < 10)
	{
		pMsg.Result = CB_LOW_LEVEL_USER;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	int nChaosNeedMoney = 3000000 * iMixCount;
	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if (nChaosNeedMoney < 0)
	{
		nChaosNeedMoney = 0;
	}

	if (Obj.m_PlayerData->Money < nChaosNeedMoney)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_NOT_ENOUGH_ZEN;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	LogChaosItem(lpObj, "CircleMix");
	sLog->outBasic("[CircleMix] Chaos Mix Start");
	Obj.ChaosSuccessRate = 90;	// Succes Rate for Fruit
	Obj.m_PlayerData->Money -= nChaosNeedMoney;

	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	pMsg.Result = CB_MULTIMIX_RESULT;
	pMsg.Pos = iMixCount;

	memset(pMsg.ItemInfo, 0x00, sizeof(pMsg.ItemInfo));

	Obj.ChaosMassMixCurrItem = 0;
	Obj.ChaosMassMixSuccessCount = 0;

	for (int i = 0; i < iMixCount; i++)
	{
		if ((rand() % 100) < Obj.ChaosSuccessRate)
		{
			BYTE btMixNumber = 1 << (i % 8);
			pMsg.ItemInfo[i / 8] |= btMixNumber;

			Obj.ChaosMassMixSuccessCount++;
			sLog->outBasic("[CircleMix] [%s][%s] Mass Combination ID:%d Success %d Money : %d-%d",
				Obj.AccountID, Obj.Name, i, Obj.ChaosSuccessRate, Obj.m_PlayerData->Money,
				nChaosNeedMoney);
		}

		else
		{
			sLog->outBasic("[CircleMix] [%s][%s] Mass Combination ID:%d Fail %d Money : %d-%d",
				Obj.AccountID, Obj.Name, i, Obj.ChaosSuccessRate, Obj.m_PlayerData->Money,
				nChaosNeedMoney);
		}
	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

	if (Obj.ChaosMassMixSuccessCount == 0)
	{
		ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);

		Obj.ChaosLock = FALSE;
		return;
	}

	else
	{
		for (int i = 0; i < Obj.ChaosMassMixSuccessCount; i++)
		{
			int Fruit = ITEMGET(13, 15);	// Fruit
			int FruitType;
			int RandonValue = rand() % 100;

			if (RandonValue < 30)
			{
				FruitType = 0;
			}
			else if (RandonValue < 55)
			{
				FruitType = 1;
			}
			else if (RandonValue < 75)
			{
				FruitType = 2;
			}
			else if (RandonValue < 95)
			{
				FruitType = 3;
			}
			else
			{
				FruitType = 4;
			}

			ItemCreate(Obj.m_Index, 0xFF, 0, 0, Fruit, FruitType, 255, 1, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
		}

		gObjInventoryCommit(Obj.m_Index);
	}
}

void CMixSystem::PegasiaChaosMix_Multi(CGameObject &Obj, int iMixCount)
{
	Obj.ChaosLock = TRUE;
	int UniriaCount = 0;
	int ChoasGemCount = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(13, 2) && Obj.pChaosBox[n].m_Durability == 255.0f)	// Uniria
			{
				UniriaCount++;
			}
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 15)) // Chaos Gem
			{
				ChoasGemCount++;
			}
		}
	}

	if (UniriaCount != (10 * iMixCount) || ChoasGemCount != iMixCount)
	{
		Obj.ChaosLock = FALSE;
		return;
	}

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	int nChaosNeedMoney = 500000 * iMixCount;
	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if (nChaosNeedMoney < 0)
	{
		nChaosNeedMoney = 0;
	}

	if (Obj.m_PlayerData->Money < nChaosNeedMoney)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_NOT_ENOUGH_ZEN;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	LogChaosItem(lpObj, "DinorantMix");
	sLog->outBasic("[DinorantMix] Chaos Mix Start");
	Obj.ChaosSuccessRate = 70;	// Succes Rate for Dinorant
	Obj.m_PlayerData->Money -= nChaosNeedMoney;

	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	pMsg.Result = CB_MULTIMIX_RESULT;
	pMsg.Pos = iMixCount;

	memset(pMsg.ItemInfo, 0x00, sizeof(pMsg.ItemInfo));

	Obj.ChaosMassMixCurrItem = 0;
	Obj.ChaosMassMixSuccessCount = 0;

	for (int i = 0; i < iMixCount; i++)
	{
		if ((rand() % 100) < Obj.ChaosSuccessRate)
		{
			BYTE btMixNumber = 1 << (i % 8);
			pMsg.ItemInfo[i / 8] |= btMixNumber;

			Obj.ChaosMassMixSuccessCount++;
			sLog->outBasic("[DinorantMix] [%s][%s] Mass Combination ID:%d Success %d Money : %d-%d",
				Obj.AccountID, Obj.Name, i, Obj.ChaosSuccessRate, Obj.m_PlayerData->Money,
				nChaosNeedMoney);
		}

		else
		{
			sLog->outBasic("[DinorantMix] [%s][%s] Mass Combination ID:%d Fail %d Money : %d-%d",
				Obj.AccountID, Obj.Name, i, Obj.ChaosSuccessRate, Obj.m_PlayerData->Money,
				nChaosNeedMoney);
		}
	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

	if (Obj.ChaosMassMixSuccessCount == 0)
	{
		ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);

		Obj.ChaosLock = FALSE;
		return;
	}

	else
	{
		for (int i = 0; i < Obj.ChaosMassMixSuccessCount; i++)
		{
			int Dinorant = ITEMGET(13, 3);
			int Option3 = 0;

			if ((rand() % 100) < 30)
			{
				Option3 = 1 << ((rand() % 3));

				if ((rand() % 5) == 0)
				{
					Option3 |= 1 << (rand() % 3);
				}
			}

			ItemCreate(Obj.m_Index, 0xFF, 0, 0, Dinorant, 0, 255, 1, 0, Option3, Obj.m_Index, 0, 0, 0, 0, 0);
		}

		gObjInventoryCommit(Obj.m_Index);
	}
}

void CMixSystem::BloodCastleItemChaosMix_Multi(CGameObject &Obj, int iMixCount)
{
	Obj.ChaosLock = TRUE;

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	int iIndex = Obj.m_Index;

	int iCHAOS_MIX_LEVEL = 0;
	BOOL bIsChaosGemExist = FALSE;
	BOOL bIsAngelKingPaperExist = FALSE;
	BOOL bIsBloodBoneExist = FALSE;
	BOOL bIsOtherItemExist = FALSE;
	int iEventItemCount = 0;
	int iAngelKingPaperLevel = 0;
	int iBloodBoneLevel = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i]->IsItem() == TRUE)
		{
			if (Obj.pChaosBox[i]->m_Type == ITEMGET(12, 15)) // Chaos
			{
				bIsChaosGemExist++;
			}
			else if (Obj.pChaosBox[i]->m_Type == ITEMGET(13, 16)) //Scroll of Archangel
			{
				int iSCROLL_LEVEL = Obj.pChaosBox[i]->m_Level;
				iEventItemCount++;
				bIsAngelKingPaperExist++;
				iAngelKingPaperLevel = iSCROLL_LEVEL;
			}
			else if (Obj.pChaosBox[i]->m_Type == ITEMGET(13, 17)) //Blood Bone
			{
				int iBLOOD_BONE_LEVEL = Obj.pChaosBox[i]->m_Level;
				iEventItemCount++;
				bIsBloodBoneExist++;
				iBloodBoneLevel = iBLOOD_BONE_LEVEL;
			}
			else
			{
				bIsOtherItemExist = TRUE;
			}
		}
	}

	if (bIsOtherItemExist != FALSE)
	{
		Obj.ChaosLock = FALSE;
		return;
	}

	if (bIsAngelKingPaperExist == FALSE && bIsBloodBoneExist == FALSE)
	{
		Obj.ChaosLock = FALSE;
		return;
	}

	if (bIsAngelKingPaperExist == FALSE || bIsBloodBoneExist == FALSE)
	{
		pMsg.Result = CB_NO_BC_CORRECT_ITEMS;
		IOCP.DataSend(Obj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);

		Obj.ChaosLock = FALSE;
		return;
	}

	if (iEventItemCount > (2 * iMixCount))
	{
		pMsg.Result = CB_INVALID_ITEM_LEVEL;
		IOCP.DataSend(Obj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);

		Obj.ChaosLock = FALSE;
		return;
	}

	if (iAngelKingPaperLevel != iBloodBoneLevel)
	{
		pMsg.Result = CB_NO_BC_CORRECT_ITEMS;
		IOCP.DataSend(Obj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);

		Obj.ChaosLock = FALSE;
		return;
	}

	if (BC_BRIDGE_RANGE(iAngelKingPaperLevel - 1) == FALSE)
	{
		pMsg.Result = CB_NO_BC_CORRECT_ITEMS;
		IOCP.DataSend(Obj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);

		Obj.ChaosLock = FALSE;
		return;
	}

	if (BC_BRIDGE_RANGE(iBloodBoneLevel - 1) == FALSE)
	{
		pMsg.Result = CB_NO_BC_CORRECT_ITEMS;
		IOCP.DataSend(Obj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);

		Obj.ChaosLock = FALSE;
		return;
	}

	if (bIsChaosGemExist == FALSE)
	{
		pMsg.Result = CB_NO_BC_CORRECT_ITEMS;
		IOCP.DataSend(Obj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);

		Obj.ChaosLock = FALSE;
		return;
	}

	if (Obj.Level < g_sttBLOODCASTLE_LEVEL[0]->iLOWER_BOUND)
	{
		pMsg.Result = CB_USER_CLASS_LOW_LEVEL;
		IOCP.DataSend(Obj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);

		Obj.ChaosLock = FALSE;
		return;
	}

	if (bIsChaosGemExist == iMixCount && bIsAngelKingPaperExist == iMixCount && bIsBloodBoneExist == iMixCount)
	{
		int cMapNum = g_BloodCastle.GetBridgeMapNumber(iAngelKingPaperLevel - 1);

		if (BC_MAP_RANGE(cMapNum) == FALSE)
		{
			return;
		}

		char szTemp[64];
		wsprintf(szTemp, "BloodCastle Ticket Mix,%d", iAngelKingPaperLevel); //Season 2.5 add-on

		g_MixSystem.LogChaosItem(Obj, szTemp);

		int iMIX_SUCCESS_RATE = g_iBC_ChoasMixSuccessRate[iAngelKingPaperLevel - 1];

		if (iMIX_SUCCESS_RATE < 0 || iMIX_SUCCESS_RATE > 100)
		{
			IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
			return;
		}

		if (g_CrywolfSync.GetOccupationState() == 0 && g_CrywolfSync.GetApplyBenefit() == TRUE)
		{
			iMIX_SUCCESS_RATE += g_CrywolfSync.GetPlusChaosRate();
		}

		if (iMIX_SUCCESS_RATE > 80)
		{
			iMIX_SUCCESS_RATE = 80;
		}

		int iMIX_NEED_MONEY = g_iBC_ChoasMixMoney[iAngelKingPaperLevel - 1] * iMixCount;
		int iChaosTaxMoney = (int)((__int64)(iMIX_NEED_MONEY)* (__int64)(g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index)) / (__int64)100);

		if (iChaosTaxMoney < 0)
		{
			iChaosTaxMoney = 0;
		}

		iMIX_NEED_MONEY += iChaosTaxMoney;

		if (iMIX_NEED_MONEY < 0)
		{
			IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
			return;
		}

		if ((Obj.m_PlayerData->Money - iMIX_NEED_MONEY) < 0)
		{
			pMsg.Result = CB_BC_NOT_ENOUGH_ZEN;
			IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
			return;
		}

		Obj.m_PlayerData->Money -= iMIX_NEED_MONEY;
		g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
		gGameProtocol.GCMoneySend(iIndex, Obj.m_PlayerData->Money);

		pMsg.Result = CB_MULTIMIX_RESULT;
		pMsg.Pos = iMixCount;

		memset(pMsg.ItemInfo, 0x00, sizeof(pMsg.ItemInfo));

		Obj.ChaosMassMixCurrItem = 0;
		Obj.ChaosMassMixSuccessCount = 0;

		for (int i = 0; i < iMixCount; i++)
		{
			if ((rand() % 100) < iMIX_SUCCESS_RATE)
			{
				BYTE btMixNumber = 1 << (i % 8);
				pMsg.ItemInfo[i / 8] |= btMixNumber;

				Obj.ChaosMassMixSuccessCount++;
			}

			else
			{

			}
		}

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

		if (Obj.ChaosMassMixSuccessCount == 0)
		{
			ChaosBoxInit(lpObj);
			gGameProtocol.GCUserChaosBoxSend(lpObj, 0);

			Obj.ChaosLock = FALSE;
			return;
		}

		for (int i = 0; i < Obj.ChaosMassMixSuccessCount; i++)
		{
			int item_num = ITEMGET(13, 18);
			ItemCreate(iIndex, -1, 0, 0, item_num, iAngelKingPaperLevel, 255, 0, 0, 0, -1, 0, 0, 0, 0, 0);
		}

		gObjInventoryCommit(Obj.m_Index);

		return;
	}
}

void CMixSystem::DevilSquareItemChaosMix_Multi(CGameObject &Obj, int iMixCount)
{
	Obj.ChaosLock = TRUE;

	if (g_DevilSquare.IsEventEnable() == false)
	{
		Obj.ChaosLock = FALSE;
		return;
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;
	CGameObject &Obj = Obj.m_Index;

	BOOL bChaoseGem = FALSE;
	BOOL bEyeOfDevil = FALSE;
	BOOL bKeyOfDevil = FALSE;
	BOOL FoundOtherItem = FALSE;
	int eventitemcount = 0;
	int level = -1;
	BOOL bLevelCheck = FALSE;

	sLog->outBasic("[DevilSquare] Search DevilSquareItem");

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 15))	// Chaos
			{
				bChaoseGem++;
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 17))	// Eye
			{
				eventitemcount++;
				bEyeOfDevil++;

				if (level != Obj.pChaosBox[n].m_Level)
				{
					if (level == -1)
					{
						level = Obj.pChaosBox[n].m_Level;
					}
					else
					{
						bLevelCheck = TRUE;
					}
				}
			}

			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 18))	// Key
			{
				eventitemcount++;
				bKeyOfDevil++;

				if (level != Obj.pChaosBox[n].m_Level)
				{
					if (level == -1)
					{
						level = Obj.pChaosBox[n].m_Level;
					}
					else
					{
						bLevelCheck = TRUE;
					}
				}
			}

			else
			{
				FoundOtherItem = TRUE;
			}
		}
	}

	if (FoundOtherItem != FALSE)
	{
		sLog->outBasic("[DevilSquare] Other DQItems Found");

		Obj.ChaosLock = FALSE;
		return;
	}

	if (bLevelCheck != FALSE)
	{
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		sLog->outBasic("[DevilSquare] DiffLevel Devil's Key or Eyes [%d]", eventitemcount);
		Obj.ChaosLock = FALSE;

		return;
	}

	if (eventitemcount > (2 * iMixCount))
	{
		pMsg.Result = CB_TOO_MANY_ITEMS;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		sLog->outBasic("[DevilSquare] Too many Devil's Key or Eyes [%d]", eventitemcount);
		Obj.ChaosLock = FALSE;

		return;
	}

	if (Obj.Level < 10)
	{
		pMsg.Result = CB_LOW_LEVEL_USER;
		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	sLog->outBasic("[DevilSquare] Chaos Mix Start");
	INT nChaosNeedMoney = 0;

	switch (level)
	{
	case 0:
		Obj.ChaosSuccessRate = 60;
		nChaosNeedMoney = 100000;
		break;

	case 1:
		Obj.ChaosSuccessRate = gDQChaosSuccessRateLevel1;
		nChaosNeedMoney = 100000;
		break;

	case 2:
		Obj.ChaosSuccessRate = gDQChaosSuccessRateLevel2;
		nChaosNeedMoney = 200000;
		break;

	case 3:
		Obj.ChaosSuccessRate = gDQChaosSuccessRateLevel3;
		nChaosNeedMoney = 400000;
		break;

	case 4:
		Obj.ChaosSuccessRate = gDQChaosSuccessRateLevel4;
		nChaosNeedMoney = 700000;
		break;

	case 5:
		Obj.ChaosSuccessRate = gDQChaosSuccessRateLevel5;
		nChaosNeedMoney = 1100000;
		break;

	case 6:
		Obj.ChaosSuccessRate = gDQChaosSuccessRateLevel6;
		nChaosNeedMoney = 1600000;
		break;

	case 7:
		Obj.ChaosSuccessRate = gDQChaosSuccessRateLevel7;
		nChaosNeedMoney = 2000000;
		break;

	default:
		sLog->outBasic("[DevilSquare] [%s][%s] Invalid DQItem Level [%d]",
			Obj.AccountID, Obj.Name, level);

		pMsg.Result = CB_INVALID_ITEM_LEVEL;
		IOCP.DataSend(Obj.m_Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	nChaosNeedMoney *= iMixCount;

	int iChaosTaxMoney = (int)((__int64)nChaosNeedMoney * (__int64)g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / (__int64)100);

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	nChaosNeedMoney += iChaosTaxMoney;

	if (nChaosNeedMoney < 0)
	{
		nChaosNeedMoney = 0;
	}

	if ((Obj.m_PlayerData->Money - nChaosNeedMoney) < 0)
	{
		sLog->outBasic("[DevilSquare] [%s][%s] CBMix Not Enough Money [%d] need zen [%d]",
			Obj.AccountID, Obj.Name, Obj.m_PlayerData->Money, nChaosNeedMoney);
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		IOCP.DataSend(Obj.m_Index, (BYTE *)&pMsg, pMsg.h.size);

		Obj.ChaosLock = FALSE;
		return;

	}

	if (g_CrywolfSync.GetOccupationState() == 0 && g_CrywolfSync.GetApplyBenefit() == TRUE)
	{
		Obj.ChaosSuccessRate += g_CrywolfSync.GetPlusChaosRate();
	}

	Obj.m_PlayerData->Money -= nChaosNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	pMsg.Result = CB_MULTIMIX_RESULT;
	pMsg.Pos = iMixCount;

	memset(pMsg.ItemInfo, 0x00, sizeof(pMsg.ItemInfo));

	Obj.ChaosMassMixCurrItem = 0;
	Obj.ChaosMassMixSuccessCount = 0;

	for (int i = 0; i < iMixCount; i++)
	{
		if ((rand() % 100) < Obj.ChaosSuccessRate)
		{
			BYTE btMixNumber = 1 << (i % 8);
			pMsg.ItemInfo[i / 8] |= btMixNumber;

			Obj.ChaosMassMixSuccessCount++;
			sLog->outBasic("[DevilSquare,%d] [%s][%s] Mass Combination ID:%d Success %d Money : %d-%d", level,
				Obj.AccountID, Obj.Name, i, Obj.ChaosSuccessRate, Obj.m_PlayerData->Money,
				nChaosNeedMoney);
		}

		else
		{
			sLog->outBasic("[DevilSquare,%d] [%s][%s] Mass Combination ID:%d Fail %d Money : %d-%d", level,
				Obj.AccountID, Obj.Name, i, Obj.ChaosSuccessRate, Obj.m_PlayerData->Money,
				nChaosNeedMoney);
		}
	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

	if (Obj.ChaosMassMixSuccessCount == 0)
	{
		ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);

		sLog->outBasic("[DevilSquare,%d] [%s][%s] CBMix Fail %d Money : %d-%d", level,
			Obj.AccountID, Obj.Name, Obj.ChaosSuccessRate,
			Obj.m_PlayerData->Money, nChaosNeedMoney);

		Obj.ChaosLock = FALSE;
		return;
	}

	for (int i = 0; i < Obj.ChaosMassMixSuccessCount; i++)
	{
		int item_num = ITEMGET(14, 19);
		ItemCreate(Obj.m_Index, 0xFF, 0, 0, item_num, level, 255, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
	}

	gObjInventoryCommit(Obj.m_Index);
}

void CMixSystem::ShieldPotionLv1_Mix_Multi(CGameObject &Obj, int iMixCount)
{
	int iHealthPotionCount = 0;
	int iInvalidItemCount = 0;
	int iChaosMixPrice = 0;

	if (g_ConfigRead.g_ShieldSystemOn == FALSE)
		return;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 3))
			{
				iHealthPotionCount += Obj.pChaosBox[n].m_Durability;
			}

			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if (iInvalidItemCount > 0 || iHealthPotionCount != (3 * iMixCount))
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	iChaosMixPrice = g_ConfigRead.g_iShieldPotionLv1MixMoney * iMixCount;
	int iChaosTaxMoney = iChaosMixPrice * g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / 100;

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	iChaosMixPrice += iChaosTaxMoney;

	if (iChaosMixPrice < 0)
	{
		iChaosMixPrice = 0;
	}

	if (Obj.m_PlayerData->Money < iChaosMixPrice)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	Obj.m_PlayerData->Money -= iChaosMixPrice;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	LogChaosItem(lpObj, "PotionMix][ShieldPotion Lv1 Mix");
	sLog->outBasic("[PotionMix][ShieldPotion Lv1 Mix] - Mix Start");

	pMsg.Result = CB_MULTIMIX_RESULT;
	pMsg.Pos = iMixCount;

	memset(pMsg.ItemInfo, 0x00, sizeof(pMsg.ItemInfo));

	Obj.ChaosMassMixCurrItem = 0;
	Obj.ChaosMassMixSuccessCount = 0;

	for (int i = 0; i < iMixCount; i++)
	{
		if ((rand() % 100) < g_ConfigRead.g_iShieldPotionLv1MixSuccessRate)
		{
			BYTE btMixNumber = 1 << (i % 8);
			pMsg.ItemInfo[i / 8] |= btMixNumber;

			Obj.ChaosMassMixSuccessCount++;

			sLog->outBasic("[ChaosMix][Shield Potion] Lv1 Potion Mass Mix (%d) Success [%s][%s], Money(%d-%d)",
				Obj.AccountID, Obj.Name, Obj.m_PlayerData->Money, iChaosMixPrice);
		}

		else
		{
			sLog->outBasic("[ChaosMix][Shield Potion] Lv1 Potion Mass Mix Fail (%d) [%s][%s], Money(%d-%d)",
				Obj.AccountID, Obj.Name, Obj.m_PlayerData->Money, iChaosMixPrice);
		}
	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

	if (Obj.ChaosMassMixSuccessCount == 0)
	{
		ChaosBoxInit(lpObj);
		::gGameProtocol.GCUserChaosBoxSend(lpObj, 0);

		Obj.ChaosLock = FALSE;
		return;
	}

	for (int i = 0; i < Obj.ChaosMassMixSuccessCount; i++)
	{
		int ItemNum = ITEMGET(14, 35);
		ItemCreate(Obj.m_Index, 0xFF, 0, 0, ItemNum, 0, 1, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
	}

	gObjInventoryCommit(Obj.m_Index);
}

void CMixSystem::ShieldPotionLv2_Mix_Multi(CGameObject &Obj, int iMixCount)
{
	int iHealthPotionCount = 0;
	int iInvalidItemCount = 0;
	int iChaosMixPrice = 0;

	if (g_ConfigRead.g_ShieldSystemOn == FALSE)
		return;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 38))
			{
				iHealthPotionCount += Obj.pChaosBox[n].m_Durability;
			}

			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if (iInvalidItemCount > 0 || iHealthPotionCount != (3 * iMixCount))
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	iChaosMixPrice = g_ConfigRead.g_iShieldPotionLv2MixMoney * iMixCount;
	int iChaosTaxMoney = iChaosMixPrice * g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / 100;

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	iChaosMixPrice += iChaosTaxMoney;

	if (iChaosMixPrice < 0)
	{
		iChaosMixPrice = 0;
	}

	if (Obj.m_PlayerData->Money < iChaosMixPrice)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	Obj.m_PlayerData->Money -= iChaosMixPrice;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	LogChaosItem(lpObj, "PotionMix][ShieldPotion Lv2 Mix");
	sLog->outBasic("[PotionMix][ShieldPotion Lv2 Mix] - Mix Start");

	pMsg.Result = CB_MULTIMIX_RESULT;
	pMsg.Pos = iMixCount;

	memset(pMsg.ItemInfo, 0x00, sizeof(pMsg.ItemInfo));

	Obj.ChaosMassMixCurrItem = 0;
	Obj.ChaosMassMixSuccessCount = 0;

	for (int i = 0; i < iMixCount; i++)
	{
		if ((rand() % 100) < g_ConfigRead.g_iShieldPotionLv2MixSuccessRate)
		{
			BYTE btMixNumber = 1 << (i % 8);
			pMsg.ItemInfo[i / 8] |= btMixNumber;

			Obj.ChaosMassMixSuccessCount++;

			sLog->outBasic("[ChaosMix][Shield Potion] Lv2 Potion Mass Mix (%d) Success [%s][%s], Money(%d-%d)",
				Obj.AccountID, Obj.Name, Obj.m_PlayerData->Money, iChaosMixPrice);
		}

		else
		{
			sLog->outBasic("[ChaosMix][Shield Potion] Lv2 Potion Mass Mix Fail (%d) [%s][%s], Money(%d-%d)",
				Obj.AccountID, Obj.Name, Obj.m_PlayerData->Money, iChaosMixPrice);
		}
	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

	if (Obj.ChaosMassMixSuccessCount == 0)
	{
		ChaosBoxInit(lpObj);
		::gGameProtocol.GCUserChaosBoxSend(lpObj, 0);

		Obj.ChaosLock = FALSE;
		return;
	}

	for (int i = 0; i < Obj.ChaosMassMixSuccessCount; i++)
	{
		int ItemNum = ITEMGET(14, 36);
		ItemCreate(Obj.m_Index, 0xFF, 0, 0, ItemNum, 0, 1, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
	}

	gObjInventoryCommit(Obj.m_Index);
}

void CMixSystem::ShieldPotionLv3_Mix_Multi(CGameObject &Obj, int iMixCount)
{
	int iHealthPotionCount = 0;
	int iInvalidItemCount = 0;
	int iChaosMixPrice = 0;

	if (g_ConfigRead.g_ShieldSystemOn == FALSE)
		return;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 39))
			{
				iHealthPotionCount += Obj.pChaosBox[n].m_Durability;
			}

			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if (iInvalidItemCount > 0 || iHealthPotionCount != (3 * iMixCount))
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	iChaosMixPrice = g_ConfigRead.g_iShieldPotionLv3MixMoney * iMixCount;
	int iChaosTaxMoney = iChaosMixPrice * g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / 100;

	if (iChaosTaxMoney < 0)
	{
		iChaosTaxMoney = 0;
	}

	iChaosMixPrice += iChaosTaxMoney;

	if (iChaosMixPrice < 0)
	{
		iChaosMixPrice = 0;
	}

	if (Obj.m_PlayerData->Money < iChaosMixPrice)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;

		return;
	}

	Obj.m_PlayerData->Money -= iChaosMixPrice;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	LogChaosItem(lpObj, "PotionMix][ShieldPotion Lv3 Mix");
	sLog->outBasic("[PotionMix][ShieldPotion Lv3 Mix] - Mix Start");

	pMsg.Result = CB_MULTIMIX_RESULT;
	pMsg.Pos = iMixCount;

	memset(pMsg.ItemInfo, 0x00, sizeof(pMsg.ItemInfo));

	Obj.ChaosMassMixCurrItem = 0;
	Obj.ChaosMassMixSuccessCount = 0;

	for (int i = 0; i < iMixCount; i++)
	{
		if ((rand() % 100) < g_ConfigRead.g_iShieldPotionLv3MixSuccessRate)
		{
			BYTE btMixNumber = 1 << (i % 8);
			pMsg.ItemInfo[i / 8] |= btMixNumber;

			Obj.ChaosMassMixSuccessCount++;

			sLog->outBasic("[ChaosMix][Shield Potion] Lv3 Potion Mass Mix (%d) Success [%s][%s], Money(%d-%d)",
				Obj.AccountID, Obj.Name, Obj.m_PlayerData->Money, iChaosMixPrice);
		}

		else
		{
			sLog->outBasic("[ChaosMix][Shield Potion] Lv3 Potion Mass Mix Fail (%d) [%s][%s], Money(%d-%d)",
				Obj.AccountID, Obj.Name, Obj.m_PlayerData->Money, iChaosMixPrice);
		}
	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

	if (Obj.ChaosMassMixSuccessCount == 0)
	{
		ChaosBoxInit(lpObj);
		::gGameProtocol.GCUserChaosBoxSend(lpObj, 0);

		Obj.ChaosLock = FALSE;
		return;
	}

	for (int i = 0; i < Obj.ChaosMassMixSuccessCount; i++)
	{
		int ItemNum = ITEMGET(14, 37);
		ItemCreate(Obj.m_Index, 0xFF, 0, 0, ItemNum, 0, 1, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
	}

	gObjInventoryCommit(Obj.m_Index);
}

void CMixSystem::PremiumBoxMix_Multi(CGameObject &Obj, int iMixCount)
{
	PMSG_CHAOSMIXRESULT pMsg = { 0 };
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	char SealedSilver = 0;
	char SealedGold = 0;
	char SilverKey = 0;
	char GoldKey = 0;
	char Invalid = 0;

	Obj.ChaosLock = TRUE;

	for (int n = 0; n < CHAOS_BOX_SIZE; ++n)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 112))
				SilverKey++;
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 113))
				GoldKey++;
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 121))
				SealedGold++;
			else if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 122))
				SealedSilver++;
			else
				Invalid++;
		}
	}

	if (Invalid)
	{
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	LogChaosItem(lpObj, "PremiumBoxMix");
	BYTE btMixType = 0xFF;

	if (SealedSilver == iMixCount && SilverKey == iMixCount && SealedGold == 0 && GoldKey == 0)
	{
		btMixType = 0;
	}

	else if (SealedGold == iMixCount && GoldKey == iMixCount && SealedSilver == 0 && SilverKey == 0)
	{
		btMixType = 1;
	}

	if (btMixType == 0xFF)
	{
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return;
	}

	Obj.ChaosSuccessRate = 100;

	pMsg.Result = CB_MULTIMIX_RESULT;
	pMsg.Pos = iMixCount;

	memset(pMsg.ItemInfo, 0x00, sizeof(pMsg.ItemInfo));

	Obj.ChaosMassMixCurrItem = 0;
	Obj.ChaosMassMixSuccessCount = 0;

	for (int i = 0; i < iMixCount; i++)
	{
		if ((rand() % 100) < Obj.ChaosSuccessRate)
		{
			BYTE btMixNumber = 1 << (i % 8);
			pMsg.ItemInfo[i / 8] |= btMixNumber;

			Obj.ChaosMassMixSuccessCount++;

			sLog->outBasic("[PremiumBoxMix] Mass Mix (%d) Success [%s][%s]",
				i, Obj.AccountID, Obj.Name);
		}

		else
		{
			sLog->outBasic("[PremiumBoxMix] Mass Mix Fail (%d) [%s][%s]",
				i, Obj.AccountID, Obj.Name);
		}
	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

	if (Obj.ChaosMassMixSuccessCount == 0)
	{
		ChaosBoxInit(lpObj);
		::gGameProtocol.GCUserChaosBoxSend(lpObj, 0);

		Obj.ChaosLock = FALSE;
		return;
	}

	for (int i = 0; i < Obj.ChaosMassMixSuccessCount; i++)
	{
		int iBoxType = -1;

		if (btMixType == 0)
		{
			iBoxType = ITEMGET(14, 124);
		}

		else if (btMixType == 1)
		{
			iBoxType = ITEMGET(14, 123);
		}

		ItemCreate(Obj.m_Index, 0xFF, 0, 0, iBoxType, 0, 1, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
	}
}

void CMixSystem::JewelOfHarmonyItemPurityMix(CGameObject &Obj) // OK
{
	Obj.ChaosLock = TRUE;
	// Chaos Lock was Enabled

	int iChaosMixPrice = 0;
	int GemStoneCount = 0;

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(PMSG_CHAOSMIXRESULT));

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 41))
		{
			GemStoneCount++;
		}
	}

	if (GemStoneCount != 1)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	iChaosMixPrice = 10000;
	int iChaosTaxMoney = iChaosMixPrice * g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / 100;

	if (iChaosTaxMoney < 0)
		iChaosTaxMoney = 0;

	iChaosMixPrice += iChaosTaxMoney;

	if (iChaosMixPrice < 0)
		iChaosMixPrice = 0;

	if (Obj.m_PlayerData->Money < iChaosMixPrice)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	Obj.m_PlayerData->Money -= iChaosMixPrice;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	if (Obj.m_PlayerData->VipType != 0)
	{
		Obj.ChaosSuccessRate = g_ConfigRead.MixHarmonyVip;
	}
	else
	{
		Obj.ChaosSuccessRate = g_ConfigRead.MixHarmonyFree;
	}

	Obj.ChaosSuccessRate = ((Obj.ChaosSuccessRate > 100) ? 100 : Obj.ChaosSuccessRate);

	if ((rand() % 100) < Obj.ChaosSuccessRate)
	{
		ItemCreate(Obj.m_Index, 0xFF, 0, 0, ITEMGET(14, 42), 0, 1, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
		gObjInventoryCommit(Obj.m_Index);

		sLog->outBasic("[JewelOfHarmonyItemPurityMix][Success][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", Obj.AccountID, Obj.Name, Obj.ChaosSuccessRate, Obj.ChaosMoney);
	}
	else
	{
		this->ChaosBoxInit(lpObj);

		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);

		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

		sLog->outBasic("[JewelOfHarmonyItemPurityMix][Failure][%s][%s] - (ChaosSuccessRate: %d, ChaosMoney: %d)", Obj.AccountID, Obj.Name, Obj.ChaosSuccessRate, Obj.ChaosMoney);

	}
	// Chaos Lock was Disabled
	Obj.ChaosLock = FALSE;
}


void CMixSystem::PurityJewelOfHarmony_MultiMix(CGameObject &Obj, int iMixCount)
{
	if (g_kJewelOfHarmonySystem.m_bSystemPrutiyJewel != TRUE)
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 279), Obj.m_Index, 1);
		return;
	}

	if (g_kJewelOfHarmonySystem.IsEnableToUsePuritySystem() == FALSE)
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0, 279), Obj.m_Index, 1);
		return;
	}

	Obj.ChaosLock = TRUE;
	// Chaos Lock was Enabled

	int iJewelOfHarmonyItemCount = 0;
	int iInvalidItemCount = 0;
	int iChaosMixPrice = 0;
	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(PMSG_CHAOSMIXRESULT));

	pMsg.Result = CB_ERROR;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == TRUE)
		{
			if (Obj.pChaosBox[n].m_Type == ITEMGET(14, 41))
			{
				iJewelOfHarmonyItemCount++;
			}
			else
			{
				iInvalidItemCount++;
			}
		}
	}

	if (iInvalidItemCount > 0 || iJewelOfHarmonyItemCount != iMixCount)
	{
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	iChaosMixPrice = g_kJewelOfHarmonySystem.m_iZenForPurity * iMixCount;
	int iChaosTaxMoney = iChaosMixPrice * g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index) / 100;

	if (iChaosTaxMoney < 0)
		iChaosTaxMoney = 0;

	iChaosMixPrice += iChaosTaxMoney;

	if (iChaosMixPrice < 0)
		iChaosMixPrice = 0;

	if (Obj.m_PlayerData->Money < iChaosMixPrice)
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	Obj.m_PlayerData->Money -= iChaosMixPrice;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);
	g_MixSystem.LogChaosItem(lpObj, "JewelOfHarmony][Purity");

	pMsg.Result = CB_MULTIMIX_RESULT;
	pMsg.Pos = iMixCount;
	memset(pMsg.ItemInfo, 0x00, sizeof(pMsg.ItemInfo));

	Obj.ChaosMassMixCurrItem = 0;
	Obj.ChaosMassMixSuccessCount = 0;

	for (int i = 0; i < iMixCount; i++)
	{
		int iRate = rand() % 100;

		if (iRate < g_kJewelOfHarmonySystem.m_iRatePuritySuccess)
		{
			BYTE btMixNumber = 1 << (i % 8);
			pMsg.ItemInfo[i / 8] |= btMixNumber;

			Obj.ChaosMassMixSuccessCount++;

		}

		else
		{

		}
	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);

	if (Obj.ChaosMassMixSuccessCount == 0)
	{
		g_MixSystem.ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);

		pMsg.Result = CB_ERROR;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	else
	{
		int iItemType = ITEMGET(14, 42);

		for (int i = 0; i < Obj.ChaosMassMixSuccessCount; i++)
		{
			ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemType, 0, 1, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
		}

		gObjInventoryCommit(Obj.m_Index);
	}
	// Chaos Lock was Disabled
	Obj.ChaosLock = FALSE;
}

void CMixSystem::BloodCastleMix(CGameObject &Obj) // OK
{
	Obj.ChaosLock = TRUE;
	// Chaos Lock was Enabled

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((BYTE*)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = CB_ERROR;

	int ChaosCount = 0;
	int ScrollCount = 0;
	int ScrollLevel = 0;
	int BoneCount = 0;
	int BoneLevel = 0;

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		if (Obj.pChaosBox[n].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[n].m_Type == ITEMGET(12, 15))
		{
			ChaosCount++;
		}
		else if (Obj.pChaosBox[n].m_Type == ITEMGET(13, 16))
		{
			ScrollCount++;
			ScrollLevel = Obj.pChaosBox[n].m_Level;
		}
		else if (Obj.pChaosBox[n].m_Type == ITEMGET(13, 17))
		{
			BoneCount++;
			BoneLevel = Obj.pChaosBox[n].m_Level;
		}
	}

	if (ChaosCount != 1 || ScrollCount != 1 || BoneCount != 1 || ScrollLevel != BoneLevel)
	{
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	if (ScrollLevel > 7 || BoneLevel > 7)
	{
		pMsg.Result = CB_INVALID_ITEM_LEVEL;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	switch (ScrollLevel)
	{
	case 1:
		Obj.ChaosSuccessRate = g_ConfigRead.BloodCastleRateCloack[0];
		Obj.ChaosMoney = g_ConfigRead.BloodCastleMoneyCloack[0];
		break;
	case 2:
		Obj.ChaosSuccessRate = g_ConfigRead.BloodCastleRateCloack[1];
		Obj.ChaosMoney = g_ConfigRead.BloodCastleMoneyCloack[1];
		break;
	case 3:
		Obj.ChaosSuccessRate = g_ConfigRead.BloodCastleRateCloack[2];
		Obj.ChaosMoney = g_ConfigRead.BloodCastleMoneyCloack[2];
		break;
	case 4:
		Obj.ChaosSuccessRate = g_ConfigRead.BloodCastleRateCloack[3];
		Obj.ChaosMoney = g_ConfigRead.BloodCastleMoneyCloack[3];
		break;
	case 5:
		Obj.ChaosSuccessRate = g_ConfigRead.BloodCastleRateCloack[4];
		Obj.ChaosMoney = g_ConfigRead.BloodCastleMoneyCloack[4];
		break;
	case 6:
		Obj.ChaosSuccessRate = g_ConfigRead.BloodCastleRateCloack[5];
		Obj.ChaosMoney = g_ConfigRead.BloodCastleMoneyCloack[5];
		break;
	case 7:
		Obj.ChaosSuccessRate = g_ConfigRead.BloodCastleRateCloack[6];
		Obj.ChaosMoney = g_ConfigRead.BloodCastleMoneyCloack[6];
		break;
	}

	Obj.ChaosSuccessRate = ((Obj.ChaosSuccessRate > 100) ? 100 : Obj.ChaosSuccessRate);

	int TaxMoney = (Obj.ChaosMoney*g_CastleSiegeSync.GetTaxRateChaos(Obj.m_Index)) / 100;

	Obj.ChaosMoney += TaxMoney;

	if (Obj.m_PlayerData->Money < ((DWORD)Obj.ChaosMoney))
	{
		pMsg.Result = CB_NOT_ENOUGH_ZEN;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE *)&pMsg, pMsg.h.size);
		Obj.ChaosLock = FALSE;
		return;
	}

	Obj.m_PlayerData->Money -= Obj.ChaosMoney;

	gGameProtocol.GCMoneySend(Obj.m_Index, Obj.m_PlayerData->Money);

	g_CastleSiegeSync.AddTributeMoney(TaxMoney);

	if ((GetLargeRand() % 100) <= Obj.ChaosSuccessRate)
	{
		ItemCreate(Obj.m_Index, 0xFF, 0, 0, ITEMGET(13, 18), ScrollLevel, 1, 0, 0, 0, Obj.m_Index, 0, 0, 0, 0, 0);
		gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);

		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);

	}
	// Chaos Lock was Disabled
	Obj.ChaosLock = FALSE;
}

// start bloodangel mix knight
BOOL CMixSystem::bloodangelmix78(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 351))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(8, 98);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}

BOOL CMixSystem::bloodangelmix79(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 352))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(7, 98);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}

BOOL CMixSystem::bloodangelmix80(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 353))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(9, 98);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}

BOOL CMixSystem::bloodangelmix81(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 354))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(11, 98);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}
// finish mix knight

// start bloodangel mix wizard
BOOL CMixSystem::bloodangelmix86(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 359))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(8, 99);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}

BOOL CMixSystem::bloodangelmix87(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 360))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(7, 99);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}

BOOL CMixSystem::bloodangelmix88(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 361))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(9, 99);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}

BOOL CMixSystem::bloodangelmix89(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 362))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(10, 99);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}
// finish mix wizard

// start bloodangel mix 
//90, ITEMGET(13, 363), ITEMGET(8, 100),
BOOL CMixSystem::bloodangelmix90(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 363))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(8, 100);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}

//91, ITEMGET(13, 364), ITEMGET(7, 100),
BOOL CMixSystem::bloodangelmix91(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 364))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(7, 100);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}

//92, ITEMGET(13, 365), ITEMGET(9, 100),
BOOL CMixSystem::bloodangelmix92(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 365))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(9, 100);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}

//93, ITEMGET(13, 366), ITEMGET(10, 100),
BOOL CMixSystem::bloodangelmix93(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 366))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(10, 100);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}
// finish mix 

// start bloodangel mix magic
//98, ITEMGET(13, 371), ITEMGET(8, 102),
BOOL CMixSystem::bloodangelmix98(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 371))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(8, 102);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}

//99, ITEMGET(13, 372), ITEMGET(11, 102),
BOOL CMixSystem::bloodangelmix99(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 372))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(11, 102);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}

//100, ITEMGET(13, 373), ITEMGET(9, 102),
BOOL CMixSystem::bloodangelmix100(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 373))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(9, 102);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}

//101, ITEMGET(13, 374), ITEMGET(10, 102),
BOOL CMixSystem::bloodangelmix101(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 374))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(10, 102);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}
// finish mix 

// start bloodangel mix darklord
//106, ITEMGET(13, 379), ITEMGET(8, 103),
BOOL CMixSystem::bloodangelmix106(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 379))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(8, 103);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}

//107, ITEMGET(13, 380), ITEMGET(7, 103),
BOOL CMixSystem::bloodangelmix107(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 380))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(7, 103);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}

//108, ITEMGET(13, 381), ITEMGET(9, 103),
BOOL CMixSystem::bloodangelmix108(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 381))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(9, 103);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}

//109, ITEMGET(13, 382), ITEMGET(10, 103),
BOOL CMixSystem::bloodangelmix109(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 382))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(10, 103);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}
// finish mix 

// start bloodangel mix summoner
//114, ITEMGET(13, 387), ITEMGET(8, 101),
BOOL CMixSystem::bloodangelmix114(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 387))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(8, 101);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}

//115, ITEMGET(13, 388), ITEMGET(7, 101),
BOOL CMixSystem::bloodangelmix115(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 388))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(7, 101);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}

//116, ITEMGET(13, 389), ITEMGET(9, 101),
BOOL CMixSystem::bloodangelmix116(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 389))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(9, 101);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}

//117, ITEMGET(13, 390), ITEMGET(10, 101),
BOOL CMixSystem::bloodangelmix117(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 390))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(10, 101);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}
// finish mix 

// start bloodangel mix fighter
//118, ITEMGET(13, 391), ITEMGET(8, 104),
BOOL CMixSystem::bloodangelmix118(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 391))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(8, 104);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}

//119, ITEMGET(13, 392), ITEMGET(7, 104),
BOOL CMixSystem::bloodangelmix119(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 392))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(7, 104);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}

//120, ITEMGET(13, 393), ITEMGET(9, 104),
BOOL CMixSystem::bloodangelmix120(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 393))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(9, 104);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}

//121, ITEMGET(13, 394), ITEMGET(11, 104),
BOOL CMixSystem::bloodangelmix121(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 394))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(11, 104);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}
// finish mix 

// start bloodangel mix fighter
//126, ITEMGET(13, 399), ITEMGET(8, 105),
BOOL CMixSystem::bloodangelmix126(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 399))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(8, 105);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}

//127, ITEMGET(13, 400), ITEMGET(7, 105),
BOOL CMixSystem::bloodangelmix127(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 400))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(7, 105);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}

//128, ITEMGET(13, 401), ITEMGET(9, 105),
BOOL CMixSystem::bloodangelmix128(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 401))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(9, 105);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}

//129, ITEMGET(13, 402), ITEMGET(10, 105)
BOOL CMixSystem::bloodangelmix129(CGameObject &Obj)
{
	Obj.ChaosLock = TRUE;

	int AncientHerosoul = 0;
	int SealedItemCount = 0;
	int InvalidItemCount = 0;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (Obj.pChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 407))
		{
			AncientHerosoul++;
		}
		else if (Obj.pChaosBox[i].m_Type == ITEMGET(13, 402))
		{
			SealedItemCount++;
		}
		else
		{
			InvalidItemCount++;
		}
	}

	PMSG_CHAOSMIXRESULT pMsg;
	PHeadSetB((BYTE*)&pMsg, 0x86, sizeof(pMsg));
	pMsg.Result = CB_ERROR;

	if (SealedItemCount != 1 || AncientHerosoul != 1 || InvalidItemCount != 0)
	{
		Obj.ChaosLock = FALSE;
		pMsg.Result = CB_INCORRECT_MIX_ITEMS;
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		return 0;
	}

	Obj.ChaosSuccessRate = 100;

	if (rand() % 100 < Obj.ChaosSuccessRate)
	{
		int iItemNum = ITEMGET(10, 105);
		int SetOption = gSetItemOption.GenSetOption(iItemNum);
		BYTE ItemOption2 = 0;

		gItemOptionRate.GetItemOption2(60, &ItemOption2);

		::ItemCreate(Obj.m_Index, 0xFF, 0, 0, iItemNum, 0, 0, 0, ItemOption2, 0, Obj.m_Index, 0, SetOption, 0, 0, 0);
		::gObjInventoryCommit(Obj.m_Index);

	}
	else
	{
		this->ChaosBoxInit(lpObj);
		gGameProtocol.GCUserChaosBoxSend(lpObj, 0);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	Obj.ChaosLock = FALSE;
}
// finish mix 



////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

