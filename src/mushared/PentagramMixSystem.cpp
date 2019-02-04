////////////////////////////////////////////////////////////////////////////////
// PentagramMixSystem.cpp
#include "StdAfx.h"
#include "PentagramMixSystem.h"
#include "TLog.h"
#include "CastleSiegeSync.h"
#include "ChaosBox.h"
#include "protocol.h"
#include "winutil.h"
#include "GameMain.h"
#include "DSProtocol.h"
#include "RandomManager.h"
#include "LargeRand.h"

CPentagramMixSystem g_PentagramMixSystem;

CPentagramMixSystem::CPentagramMixSystem(void)
{

}

CPentagramMixSystem::~CPentagramMixSystem(void)
{

}

void CPentagramMixSystem::Initialize_MixNeedSource()
{
	for (int iClearIndex = 0; iClearIndex < 18; iClearIndex++)
	{
		this->m_RefineOptionData[iClearIndex].Clear();
	}

	for (int iClearIndex = 0; iClearIndex < 18; iClearIndex++)
	{
		this->m_RefineSuccessOptionData[iClearIndex].Clear();
	}

	for (int iClearIndex = 0; iClearIndex < 11; iClearIndex++)
	{
		this->m_JewelLevelUpgradeNeedItemData[iClearIndex].Clear();
	}

	for (int iClearIndex = 0; iClearIndex < 4; iClearIndex++)
	{
		this->m_JewelRankUpgradeNeedItemData[iClearIndex].Clear();
	}

	for (int iClearIndex = 0; iClearIndex < 4; iClearIndex++)
	{
		this->m_JewelRankUpgradeDetermineRate[iClearIndex].Clear();
	}
}

void CPentagramMixSystem::Initialize_JewelOption()
{
	for (int iClearIndex = 0; iClearIndex < 254; iClearIndex++)
	{
		this->m_JewelItemOptionData[iClearIndex].Clear();
	}
}

bool CPentagramMixSystem::LoadMixNeedSourceScript(char* pchFileName)
{
	this->Initialize_MixNeedSource();

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(pchFileName);

	if (res.status != pugi::status_ok)
	{
		g_Log.MsgBox("[PentagramMixSystem] PentagramSystem NeedSource Script file Load failed. (%s) (%s)", pchFileName, res.description());
		return false;
	}

	pugi::xml_node pentagramsystem = file.child("PentagramSystem");
	pugi::xml_node combinationmaterials = pentagramsystem.child("CombinationMaterials");
	pugi::xml_node combinationresult = pentagramsystem.child("CombinationResult");
	pugi::xml_node levelupsettings = pentagramsystem.child("LevelUpgradeSettings");
	pugi::xml_node rankupsettings = pentagramsystem.child("RankUpgradeSettings");
	pugi::xml_node rankupsocketrate = pentagramsystem.child("RankUpgradeSocketRate");

	int iValueCount = 0;

	for (pugi::xml_node combination = combinationmaterials.child("Combination"); combination; combination = combination.next_sibling())
	{
		REFINE_OPTION RefineOptionData;
		memset(&RefineOptionData, 0x00, sizeof(RefineOptionData));

		RefineOptionData.RefineIndex = combination.attribute("Index").as_int();
		RefineOptionData.RefineSuccessRate = combination.attribute("SuccessRate").as_int();

		for (int i = 0; i < 4; i++)
		{
			char szTemp[256];

			sprintf(szTemp, "ItemCat%d", i + 1);
			RefineOptionData.NeedItem_Type[i] = combination.attribute(szTemp).as_int();

			sprintf(szTemp, "ItemIndex%d", i + 1);
			RefineOptionData.NeedItem_Index[i] = combination.attribute(szTemp).as_int();

			sprintf(szTemp, "ItemCount%d", i + 1);
			RefineOptionData.NeedItem_Num[i] = combination.attribute(szTemp).as_int();
		}

		RefineOptionData.NeedMoney = combination.attribute("ReqMoney").as_int();

		memcpy(&this->m_RefineOptionData[iValueCount], &RefineOptionData, sizeof(REFINE_OPTION));
		iValueCount++;
	}

	iValueCount = 0;

	for (pugi::xml_node result = combinationresult.child("Result"); result; result = result.next_sibling())
	{

		REFINE_SUCCESS_OPTION RefineSuccessOptionData;
		memset(&RefineSuccessOptionData, 0x00, sizeof(REFINE_SUCCESS_OPTION));

		RefineSuccessOptionData.SuccessRefineIndex = result.attribute("Index").as_int();

		for (int i = 0; i < 5; i++)
		{
			char szTemp[256];

			sprintf(szTemp, "SuccessRate%d", i + 1);
			RefineSuccessOptionData.Success_AcquisitionRate[i] = result.attribute(szTemp).as_int();

			sprintf(szTemp, "ItemCat%d", i + 1);
			RefineSuccessOptionData.Success_AcquisitionItem_Type[i] = result.attribute(szTemp).as_int();

			sprintf(szTemp, "ItemIndex%d", i + 1);
			RefineSuccessOptionData.Success_AcquisitionItem_Index[i] = result.attribute(szTemp).as_int();

			sprintf(szTemp, "ItemCount%d", i + 1);
			RefineSuccessOptionData.Success_AcquisitionItem_Num[i] = result.attribute(szTemp).as_int();
		}

		memcpy(&this->m_RefineSuccessOptionData[iValueCount], &RefineSuccessOptionData, sizeof(REFINE_SUCCESS_OPTION));
		iValueCount++;
	}

	iValueCount = 0;

	for (pugi::xml_node errtel = levelupsettings.child("Errtel"); errtel; errtel = errtel.next_sibling())
	{
		JEWEL_LEVEL_UPGRADE_NEED_ITEM LevelUpgradeNeedItemData;
		memset(&LevelUpgradeNeedItemData, 0x00, sizeof(JEWEL_LEVEL_UPGRADE_NEED_ITEM));

		LevelUpgradeNeedItemData.TargetItemLevel = errtel.attribute("ExpectedLevel").as_int();
		LevelUpgradeNeedItemData.MainSourceItemLevel = errtel.attribute("InitialLevel").as_int();
		LevelUpgradeNeedItemData.MainSourceItemNum = errtel.attribute("Count").as_int();

		for (int i = 0; i < 4; i++)
		{
			char szTemp[256];

			sprintf(szTemp, "ItemCat%d", i + 1);
			LevelUpgradeNeedItemData.NeedSourceItem_Type[i] = errtel.attribute(szTemp).as_int();

			sprintf(szTemp, "ItemIndex%d", i + 1);
			LevelUpgradeNeedItemData.NeedSourceItem_Index[i] = errtel.attribute(szTemp).as_int();

			sprintf(szTemp, "ItemCount%d", i + 1);
			LevelUpgradeNeedItemData.NeedSourceItem_Num[i] = errtel.attribute(szTemp).as_int();
		}

		LevelUpgradeNeedItemData.NeedMoney = errtel.attribute("ReqMoney").as_int();

		memcpy(&this->m_JewelLevelUpgradeNeedItemData[iValueCount], &LevelUpgradeNeedItemData, sizeof(JEWEL_LEVEL_UPGRADE_NEED_ITEM));
		iValueCount++;
	}

	iValueCount = 0;

	for (pugi::xml_node errtel = rankupsettings.child("Errtel"); errtel; errtel = errtel.next_sibling())
	{

		JEWEL_RANK_UPGRADE_NEED_ITEM RankUpgradeNeedItemData;
		memset(&RankUpgradeNeedItemData, 0x00, sizeof(JEWEL_RANK_UPGRADE_NEED_ITEM));

		RankUpgradeNeedItemData.TargetItemRank = errtel.attribute("ExpectedRank").as_int();
		RankUpgradeNeedItemData.MainSourceItemRank = errtel.attribute("InitialRank").as_int();
		RankUpgradeNeedItemData.MainSourceItemNum = errtel.attribute("Count").as_int();

		for (int i = 0; i < 4; i++)
		{
			char szTemp[256];

			sprintf(szTemp, "ItemCat%d", i + 1);
			RankUpgradeNeedItemData.NeedSourceItem_Type[i] = errtel.attribute(szTemp).as_int();

			sprintf(szTemp, "ItemIndex%d", i + 1);
			RankUpgradeNeedItemData.NeedSourceItem_Index[i] = errtel.attribute(szTemp).as_int();

			sprintf(szTemp, "ItemCount%d", i + 1);
			RankUpgradeNeedItemData.NeedSourceItem_Num[i] = errtel.attribute(szTemp).as_int();
		}

		RankUpgradeNeedItemData.NeedMoney = errtel.attribute("ReqMoney").as_int();

		memcpy(&this->m_JewelRankUpgradeNeedItemData[iValueCount], &RankUpgradeNeedItemData, sizeof(JEWEL_RANK_UPGRADE_NEED_ITEM));
		iValueCount++;
	}

	iValueCount = 0;

	for (pugi::xml_node errtel = rankupsocketrate.child("Errtel"); errtel; errtel = errtel.next_sibling())
	{
		JEWEL_RANK_UPGRADE_DETERMINE_RATE JewelRankUpgradeDetermineRate;
		memset(&JewelRankUpgradeDetermineRate, 0x00, sizeof(JEWEL_RANK_UPGRADE_DETERMINE_RATE));

		JewelRankUpgradeDetermineRate.RankStep = errtel.attribute("Rank").as_int();

		for (int i = 0; i < 4; i++)
		{
			char szTemp[256];

			sprintf(szTemp, "Socket%dRate", i + 2);
			JewelRankUpgradeDetermineRate.RankRate[i] = errtel.attribute(szTemp).as_int();
		}

		JewelRankUpgradeDetermineRate.RankUpgradeSuccessRate = errtel.attribute("RankUpgradeSuccessRate").as_int();

		memcpy(&this->m_JewelRankUpgradeDetermineRate[iValueCount], &JewelRankUpgradeDetermineRate, sizeof(JEWEL_RANK_UPGRADE_DETERMINE_RATE));
		iValueCount++;
	}

	return true;
}

bool CPentagramMixSystem::LoadJewelOptionScript(char *pchFileName)
{
	this->Initialize_JewelOption();

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(pchFileName);

	if (res.status != pugi::status_ok)
	{
		g_Log.MsgBox("[PentagramMixSystem] PentagramSystem Jewel Upgrade Script file Load failed. (%s) (%s)", pchFileName, res.description());
		return false;
	}

	pugi::xml_node pentagramsystem = file.child("PentagramSystem");
	int iValueCount = 0;

	for (pugi::xml_node errtel = pentagramsystem.child("Errtel"); errtel; errtel = errtel.next_sibling())
	{
		PENTAGRAM_JEWEL_ITEM_OPTION JewelItemOptionData;
		memset(&JewelItemOptionData, 0x00, sizeof(JewelItemOptionData));

		JewelItemOptionData.JewelIndex = errtel.attribute("Index").as_int();
		JewelItemOptionData.PentagramJewel_Type = errtel.attribute("ItemCat").as_int();
		JewelItemOptionData.PentagramJewel_Index = errtel.attribute("ItemIndex").as_int();
		JewelItemOptionData.ItemRank = errtel.attribute("Rank").as_int();
		JewelItemOptionData.RankOptionNum = errtel.attribute("OptionNum").as_int();
		JewelItemOptionData.RankOptionNumRate = errtel.attribute("OptionRate").as_int();
		JewelItemOptionData.RON = errtel.attribute("RON").as_int();

		for (int i = 0; i < 11; i++)
		{
			char szTemp[256];

			sprintf(szTemp, "OptionValue%d", i);
			JewelItemOptionData.LevelValue[i] = errtel.attribute(szTemp).as_int();

			sprintf(szTemp, "OptionRate%d", i);
			JewelItemOptionData.LevelUpgradeValue[i] = errtel.attribute(szTemp).as_int();
		}

		memcpy(&this->m_JewelItemOptionData[iValueCount], &JewelItemOptionData, sizeof(PENTAGRAM_JEWEL_ITEM_OPTION));
		iValueCount++;
	}

	return true;
}

BOOL CPentagramMixSystem::PentagramMixBoxInit(LPOBJ lpObj)
{
	if (lpObj->Type != OBJ_USER)
	{
		return FALSE;
	}

	if (lpObj->m_PlayerData->pPentagramMixBox != NULL)
	{
		for (int n = 0; n < CHAOS_BOX_SIZE; n++)
		{
			lpObj->m_PlayerData->pPentagramMixBox[n].Clear();
		}

		for (int n = 0; n < CHAOS_BOX_SIZE; n++)
		{
			lpObj->m_PlayerData->pPentagramMixBoxMap[n] = -1;
		}

		return TRUE;
	}

	lpObj->m_PlayerData->pPentagramMixBox = new CItem[CHAOS_BOX_SIZE];

	if (lpObj->m_PlayerData->pPentagramMixBox == NULL)
	{
		return FALSE;
	}

	lpObj->m_PlayerData->pPentagramMixBoxMap = new BYTE[CHAOS_BOX_SIZE];

	if (lpObj->m_PlayerData->pPentagramMixBoxMap == NULL)
	{
		delete[] lpObj->m_PlayerData->pPentagramMixBox;
		return false;
	}

	for (int n = 0; n < CHAOS_BOX_SIZE; n++)
	{
		lpObj->m_PlayerData->pPentagramMixBoxMap[n] = -1;
	}

	return true;
}

BYTE CPentagramMixSystem::PentagramJewelRefine(int iIndex, int iRefineType)
{
	if (!gObjIsConnected(iIndex))
	{
		return 0;
	}

	LPOBJ lpObj = &gObj[iIndex];

	if (lpObj->Type != OBJ_USER)
	{
		return 0;
	}

	BYTE btReturnValue = 0;

	if (lpObj->m_PlayerData->m_PentagramMixLock == TRUE)
	{
		return PENTAGRAM_MIX_ALREADY_OPEN;
	}

	if (lpObj->m_bPShopOpen == true)
	{
		return PENTAGRAM_MIX_PSHOP_OPEN;
	}

	lpObj->m_PlayerData->m_PentagramMixLock = TRUE;

	BYTE btMainAttribute = 0xFF;
	bool bPentagramItem = false;
	REFINE_OPTION * lpRefineOptionData = NULL;

	for (int i = 0; i < 18; i++)
	{
		if (this->m_RefineOptionData[i].RefineIndex == iRefineType)
		{
			lpRefineOptionData = &this->m_RefineOptionData[i];
			break;
		}
	}

	if (!lpRefineOptionData)
	{
		lpObj->m_PlayerData->m_PentagramMixLock = FALSE;

		return PENTAGRAM_MIX_REFINE_NOTFOUND;
	}

	int iSourceItemCount[4] = { 0, 0, 0, 0 };
	BYTE btAttributeCount[4];
	WORD LastItemID = 0;
	BYTE btTempMainAttribute;
	BYTE btIsCorrectItem = FALSE;
	BYTE btSocketOption[5];

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (lpObj->m_PlayerData->pPentagramMixBox[i].IsItem() == TRUE)
		{
			for (int iCount = 0; iCount < 4; iCount++)
			{
				if (lpObj->m_PlayerData->pPentagramMixBox[i].m_Type == ITEMGET(lpRefineOptionData->NeedItem_Type[iCount], lpRefineOptionData->NeedItem_Index[iCount]))
				{
					if (iCount == 0)
					{
						btTempMainAttribute = lpObj->m_PlayerData->pPentagramMixBox[i].m_BonusSocketOption & 0x0F;

						if (btMainAttribute == 0xFF)
						{
							btMainAttribute = btTempMainAttribute;
						}

						else
						{
							if (btMainAttribute != btTempMainAttribute)
							{
								lpObj->m_PlayerData->m_PentagramMixLock = FALSE;

								return PENTAGRAM_MIX_ATTRIBUTE_MISMATCH;
							}
						}
					}

					iSourceItemCount[iCount]++;
					btIsCorrectItem = TRUE;
					break;
				}
			}

			if (g_PentagramSystem.IsPentagramItem(lpObj->m_PlayerData->pPentagramMixBox[i].m_Type) == TRUE)
			{
				bPentagramItem = TRUE;
				for (int k = 0; k < 5; k++)
				{
					btSocketOption[k] = lpObj->m_PlayerData->pPentagramMixBox[i].m_SocketOption[k];
				}
			}

			if (btIsCorrectItem == TRUE)
			{
				BYTE ExOption[MAX_EXOPTION_SIZE];
				ItemIsBufExOption(ExOption, &lpObj->m_PlayerData->pPentagramMixBox[i]);

				btIsCorrectItem = FALSE;
			}
		}
	}

	for (int i = 0; i < 4; i++)
	{
		if (lpRefineOptionData->NeedItem_Num[i] != iSourceItemCount[i])
		{
			lpObj->m_PlayerData->m_PentagramMixLock = FALSE;

			return PENTAGRAM_MIX_ITEMS_COUNT_MISMATCH;
		}
	}

	int iPentagramMixNeedMoney = lpRefineOptionData->NeedMoney;
	int iChaosTaxMoney = iPentagramMixNeedMoney * g_CastleSiegeSync.GetTaxRateChaos(iIndex) / 100;
	if (iChaosTaxMoney < 0)
		iChaosTaxMoney = 0;

	iPentagramMixNeedMoney += iChaosTaxMoney;
	if (iPentagramMixNeedMoney < 0)
		iPentagramMixNeedMoney = 0;

	if (lpObj->m_PlayerData->Money < iPentagramMixNeedMoney)
	{
		lpObj->m_PlayerData->m_PentagramMixLock = FALSE;

		return PENTAGRAM_MIX_NOT_ENOUGH_ZEN;
	}

	lpObj->m_PlayerData->Money -= iPentagramMixNeedMoney;
	g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
	GSProtocol.GCMoneySend(iIndex, lpObj->m_PlayerData->Money);

	int iSuccessRate = lpRefineOptionData->RefineSuccessRate;
	int iResultItemType = 0;
	int iResultItemNum = 0;
	int iRandRate = rand() % 10000;

	if (iRandRate >= iSuccessRate)
	{
		if (bPentagramItem == TRUE)
		{
			for (int j = 0; j < 5; j++)
			{
				if (btSocketOption[j] < 0xFE)
				{
					g_PentagramSystem.DelPentagramJewelInfo(iIndex, 0, btSocketOption[j]);
				}
			}
		}

		this->PentagramMixBoxInit(lpObj);
		lpObj->m_PlayerData->m_PentagramMixLock = FALSE;
		btReturnValue = PENTAGRAM_MIX_REFINE_FAILED;
	}

	else
	{
		REFINE_SUCCESS_OPTION * lpRefineSuccessOptionData = NULL;

		for (int i = 0; i < 18; i++)
		{
			if (this->m_RefineSuccessOptionData[i].SuccessRefineIndex == lpRefineOptionData->RefineIndex)
			{
				lpRefineSuccessOptionData = &this->m_RefineSuccessOptionData[i];
				break;
			}
		}

		if (lpRefineSuccessOptionData == NULL)
		{
			lpObj->m_PlayerData->m_PentagramMixLock = FALSE;

			return PENTAGRAM_MIX_REFINE_SUCCESS_NOTFOUND;
		}

		int iFirstAcquisitionItemArrayNo = 0;
		int iAcquisitionItemKind = 0;

		for (int l = 0; l < 5; l++)
		{
			if (lpRefineSuccessOptionData->Success_AcquisitionItem_Num[l] > 0)
				iAcquisitionItemKind++;
		}

		if (bPentagramItem == TRUE)
		{
			for (int j = 0; j < 5; j++)
			{
				if (btSocketOption[j] < 0xFE)
				{
					g_PentagramSystem.DelPentagramJewelInfo(iIndex, 0, btSocketOption[j]);
				}
			}
		}

		this->PentagramMixBoxInit(lpObj);

		int iAcquisitionRate = 0;
		BYTE btBonusSocketOption = 0xFF;
		BYTE btOptionNum = 0;

		for (int i = 0; i < iAcquisitionItemKind; i++)
		{
			iAcquisitionRate += lpRefineSuccessOptionData->Success_AcquisitionRate[i];

			if ((rand() % 10000) < iAcquisitionRate)
			{
				iFirstAcquisitionItemArrayNo = i;
				iResultItemType = ITEMGET(lpRefineSuccessOptionData->Success_AcquisitionItem_Type[iFirstAcquisitionItemArrayNo], lpRefineSuccessOptionData->Success_AcquisitionItem_Index[iFirstAcquisitionItemArrayNo]);
				iResultItemNum = lpRefineSuccessOptionData->Success_AcquisitionItem_Num[iFirstAcquisitionItemArrayNo];
				int iItemDurability = ItemGetDurability(iResultItemType, 0, 0, 0);
				BYTE SocketOption[5];

				if (lpRefineSuccessOptionData->SuccessRefineIndex == 3)
				{
					btOptionNum = 1;

					for (int n = 0; n < 5; n++)
					{
						if (n)
							SocketOption[n] = 0xFF;
						else
							SocketOption[0] = btOptionNum;
					}

					btBonusSocketOption = btMainAttribute | 0x10;
				}

				else if (g_PentagramSystem.IsPentagramItem(iResultItemType) == TRUE)
				{
					BYTE btEnableSlot[5];
					BYTE btSlotCount = g_PentagramSystem.GetMakePentagramSlotCountNKind(btEnableSlot, iResultItemType);

					for (int n = 0; n < 5; n++)
					{
						if (btEnableSlot[n] == TRUE)
						{
							SocketOption[n] = 0xFE;
						}

						else
						{
							SocketOption[n] = 0xFF;
						}
					}

					btBonusSocketOption = btMainAttribute;
				}

				else
				{
					for (int n = 0; n < 5; n++)
					{
						SocketOption[n] = 0xFF;
					}

					if (g_PentagramSystem.IsPentagramJewel(iResultItemType) == TRUE)
					{
						btBonusSocketOption = btMainAttribute;
					}
				}

				for (int j = 0; j < iResultItemNum; j++)
				{
					ItemSerialCreateSend(iIndex, 230, 0, 0, iResultItemType, 0, iItemDurability, 0, 0, 0, 0, 0, 0, 0, SocketOption, btBonusSocketOption);
					btReturnValue = TRUE;
				}

				break;
			}
		}
	}

	lpObj->m_PlayerData->m_PentagramMixLock = FALSE;
	gObjInventoryCommit(iIndex);

	return btReturnValue;
}

BYTE CPentagramMixSystem::PentagramJewel_Upgrade(int iIndex, int iUpgradeType, int iTargetValue)
{
	if (!gObjIsConnected(iIndex))
	{
		return 0;
	}

	LPOBJ lpObj = &gObj[iIndex];

	if (lpObj->Type != OBJ_USER)
	{
		return 0;
	}

	BYTE btReturnValue = 0;

	if (lpObj->m_PlayerData->m_PentagramMixLock == TRUE)
	{
		return PENTAGRAM_MIX_ALREADY_OPEN;
	}

	if (lpObj->m_bPShopOpen == true)
	{
		return PENTAGRAM_MIX_PSHOP_OPEN;
	}

	lpObj->m_PlayerData->m_PentagramMixLock = TRUE;

	if (iUpgradeType == 1)
	{
		int iPentagramJewelID = 0;
		int iPentagramJewelCurRank = 0;
		int iRankCount = 0;
		int iPentagramJewelCount = 0;
		int iSourceJewelLevel = 0;
		int iSetItemCount = 0;
		int iSetItemMoney = 0;
		BYTE btPentagramLuckCharm = FALSE;
		BYTE btPentagramChaosCharm = FALSE;
		int iPentagramLuckCharmDur = 0;
		BYTE bSourceItem = FALSE;
		int SourceItemCount[4] = { 0, 0, 0, 0 };
		BYTE btSocketOption[5];
		int iSuccessRank = 0;
		int iRankOptionNum = 0;

		for (int i = 0; i < CHAOS_BOX_SIZE; i++)
		{
			if (lpObj->m_PlayerData->pPentagramMixBox[i].IsItem() == TRUE)
			{
				for (int n = 0; n < 4; n++)
				{
					if (this->m_JewelRankUpgradeNeedItemData[n].TargetItemRank == iTargetValue)
					{
						for (int k = 0; k < 4; k++)
						{
							if (lpObj->m_PlayerData->pPentagramMixBox[i].m_Type != ITEMGET(this->m_JewelRankUpgradeNeedItemData[n].NeedSourceItem_Type[0], this->m_JewelRankUpgradeNeedItemData[n].NeedSourceItem_Index[0])
								&& lpObj->m_PlayerData->pPentagramMixBox[i].m_Type != ITEMGET(this->m_JewelRankUpgradeNeedItemData[n].NeedSourceItem_Type[1], this->m_JewelRankUpgradeNeedItemData[n].NeedSourceItem_Index[1])
								&& lpObj->m_PlayerData->pPentagramMixBox[i].m_Type != ITEMGET(this->m_JewelRankUpgradeNeedItemData[n].NeedSourceItem_Type[2], this->m_JewelRankUpgradeNeedItemData[n].NeedSourceItem_Index[2]))
							{
								if (lpObj->m_PlayerData->pPentagramMixBox[i].m_Type == ITEMGET(14, 189))
								{
									btPentagramLuckCharm = TRUE;
									iPentagramLuckCharmDur = lpObj->m_PlayerData->pPentagramMixBox[i].m_Durability;
									bSourceItem = TRUE;
								}

								else if (lpObj->m_PlayerData->pPentagramMixBox[i].m_Type == ITEMGET(14, 190))
								{
									btPentagramChaosCharm = TRUE;
									bSourceItem = TRUE;
								}

								else if (iPentagramJewelCount > 1 || iSetItemCount > 1)
								{
									lpObj->m_PlayerData->m_PentagramMixLock = FALSE;

									return PENTAGRAM_MIX_ITEMS_COUNT_MISMATCH;
								}

								else if (this->CheckLevelCondition(&lpObj->m_PlayerData->pPentagramMixBox[i], 7, 0, 0, 1, 0, 0))
								{
									iSetItemCount++;
									iSetItemMoney += lpObj->m_PlayerData->pPentagramMixBox[i].m_BuyMoney;
									bSourceItem = TRUE;
								}

								else if (g_PentagramSystem.IsPentagramJewel(&lpObj->m_PlayerData->pPentagramMixBox[i]) == TRUE)
								{
									iPentagramJewelID = lpObj->m_PlayerData->pPentagramMixBox[i].m_Type;
									iPentagramJewelCurRank = (lpObj->m_PlayerData->pPentagramMixBox[i].m_BonusSocketOption & 0xF0) >> 4;

									for (int l = 0; l < 5; l++)
									{
										if (iPentagramJewelCurRank == l + 1)
										{
											iSourceJewelLevel = (lpObj->m_PlayerData->pPentagramMixBox[i].m_SocketOption[l] & 0xF0) >> 4;
										}

										if (lpObj->m_PlayerData->pPentagramMixBox[i].m_SocketOption[l] >= 0xFE)
										{
											btSocketOption[l] = 0xFF;
										}

										else
										{
											btSocketOption[l] = lpObj->m_PlayerData->pPentagramMixBox[i].m_SocketOption[l];
											iRankCount++;
										}
									}

									iPentagramJewelCount++;
									bSourceItem = TRUE;
								}
								break;
							}

							if (lpObj->m_PlayerData->pPentagramMixBox[i].m_Type == ITEMGET(this->m_JewelRankUpgradeNeedItemData[n].NeedSourceItem_Type[k], this->m_JewelRankUpgradeNeedItemData[n].NeedSourceItem_Index[k]))
							{
								SourceItemCount[k]++;
								bSourceItem = TRUE;
								break;
							}
						}

						if (bSourceItem)
						{
							BYTE ExOption[MAX_EXOPTION_SIZE];
							ItemIsBufExOption(ExOption, &lpObj->m_PlayerData->pPentagramMixBox[i]);

							bSourceItem = FALSE;
						}
					}
				}
			}
		}

		if (iSourceJewelLevel < 7)
		{
			lpObj->m_PlayerData->m_PentagramMixLock = FALSE;

			return PENTAGRAM_MIX_ITEMS_COUNT_MISMATCH;
		}

		for (int m = 0; m < 4; m++)
		{
			if (this->m_JewelRankUpgradeNeedItemData[m].TargetItemRank == iRankCount + 1)
			{
				for (int i = 0; i < 4; i++)
				{
					if (this->m_JewelRankUpgradeNeedItemData[m].NeedSourceItem_Num[i] != SourceItemCount[i] && (i != 3 || this->m_JewelRankUpgradeNeedItemData[m].NeedSourceItem_Num[3] != iSetItemCount))
					{
						lpObj->m_PlayerData->m_PentagramMixLock = FALSE;

						return PENTAGRAM_MIX_ITEMS_COUNT_MISMATCH;
					}
				}
			}
		}

		int iPentagramMixNeedMoney = 0;

		for (int m = 0; m < 4; m++)
		{
			if (this->m_JewelRankUpgradeNeedItemData[m].TargetItemRank == iRankCount + 1)
			{
				iPentagramMixNeedMoney = this->m_JewelRankUpgradeNeedItemData[m].NeedMoney;
			}
		}

		int iChaosTaxMoney = iPentagramMixNeedMoney * g_CastleSiegeSync.GetTaxRateChaos(iIndex) / 100;
		if (iChaosTaxMoney < 0)
			iChaosTaxMoney = 0;

		iPentagramMixNeedMoney += iChaosTaxMoney;
		if (iPentagramMixNeedMoney < 0)
			iPentagramMixNeedMoney = 0;

		if (lpObj->m_PlayerData->Money < iPentagramMixNeedMoney)
		{
			lpObj->m_PlayerData->m_PentagramMixLock = FALSE;

			return PENTAGRAM_MIX_NOT_ENOUGH_ZEN;
		}

		lpObj->m_PlayerData->Money -= iPentagramMixNeedMoney;
		g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
		GSProtocol.GCMoneySend(iIndex, lpObj->m_PlayerData->Money);

		int iPentagramMixSuccessRate = 0;

		for (int m = 0; m < 4; m++)
		{
			if (this->m_JewelRankUpgradeDetermineRate[m].RankStep == iRankCount + 1)
			{
				iPentagramMixSuccessRate = this->m_JewelRankUpgradeDetermineRate[m].RankUpgradeSuccessRate;
			}
		}

		if (btPentagramLuckCharm)
			iPentagramMixSuccessRate += 100 * iPentagramLuckCharmDur;

		int iRandRate = rand() % 10000;

		if (iRandRate >= iPentagramMixSuccessRate)
		{
			for (int i = 0; i < CHAOS_BOX_SIZE; i++)
			{
				if (lpObj->m_PlayerData->pPentagramMixBox[i].IsItem() == TRUE &&
					lpObj->m_PlayerData->pPentagramMixBox[i].m_Type == iPentagramJewelID)
				{
					BYTE ExOption[MAX_EXOPTION_SIZE];
					ItemIsBufExOption(ExOption, &lpObj->m_PlayerData->pPentagramMixBox[i]);

					break;

				}
			}

			if (btPentagramChaosCharm == TRUE)
			{
				CItem Item;
				BYTE ItemType;
				WORD ItemIndex;
				int iLevel;

				for (int i = 0; i < CHAOS_BOX_SIZE; i++)
				{
					if (lpObj->m_PlayerData->pPentagramMixBox[i].IsItem() == TRUE
						&& lpObj->m_PlayerData->pPentagramMixBox[i].m_Type == iPentagramJewelID)
					{
						ItemType = ITEM_GET_TYPE(lpObj->m_PlayerData->pPentagramMixBox[i].m_Type);
						ItemIndex = ITEM_GET_INDEX(lpObj->m_PlayerData->pPentagramMixBox[i].m_Type);
						Item.m_SocketOption[iPentagramJewelCurRank - 1] &= 0x0F;
						Item.m_Level = lpObj->m_PlayerData->pPentagramMixBox[i].m_Level;
						iLevel = Item.m_Level;
						Item.m_Durability = lpObj->m_PlayerData->pPentagramMixBox[i].m_Durability;
						Item.m_Number = lpObj->m_PlayerData->pPentagramMixBox[i].m_Number;

						Item.Convert(lpObj->m_PlayerData->pPentagramMixBox[i].m_Type, lpObj->m_PlayerData->pPentagramMixBox[i].m_Option1,
							lpObj->m_PlayerData->pPentagramMixBox[i].m_Option2, lpObj->m_PlayerData->pPentagramMixBox[i].m_Option3,
							lpObj->m_PlayerData->pPentagramMixBox[i].m_NewOption, lpObj->m_PlayerData->pPentagramMixBox[i].m_SetOption,
							0, lpObj->m_PlayerData->pPentagramMixBox[i].m_SocketOption, lpObj->m_PlayerData->pPentagramMixBox[i].m_BonusSocketOption, 0, CURRENT_DB_VERSION);
						break;
					}
				}

				PMSG_CHAOSMIXRESULT pMsg;
				this->PentagramMixBoxInit(lpObj);

				PHeadSetB((LPBYTE)&pMsg, 0x86, sizeof(pMsg));
				pMsg.Result = CB_ERROR;
				ItemByteConvert(pMsg.ItemInfo, Item);

				BYTE btResult = gObjPentagramMixBoxInsertItem(iIndex, Item);

				if (btResult == 0xFF)
				{
					btReturnValue = PENTAGRAM_MIX_REFINE_NOTFOUND;
				}

				else
				{
					pMsg.Result = PENTAGRAM_MIX_FAILED_WITH_TALISMAN;
					gObjPentagramMixItemSet(iIndex, btResult, 1);
					IOCP.DataSend(iIndex, (LPBYTE)&pMsg, pMsg.h.size);
					btReturnValue = TRUE;
				}
			}

			else
			{
				this->PentagramMixBoxInit(lpObj);
				btReturnValue = PENTAGRAM_MIX_UPGRADE_FAILED;
			}
		}

		else
		{
			CItem Item;
			BYTE ItemType = 0xFF;
			WORD ItemIndex = 0xFFFF;
			int iRand = rand() % 10000;
			int iOptionRate = 0;
			bool bRateFind = false;

			for (int n = 0; n < 254; n++)
			{
				if (ITEMGET(this->m_JewelItemOptionData[n].PentagramJewel_Type, this->m_JewelItemOptionData[n].PentagramJewel_Index) == iPentagramJewelID &&
					this->m_JewelItemOptionData[n].ItemRank == iRankCount + 1)
				{
					if (iRand >= iOptionRate && iRand < this->m_JewelItemOptionData[n].RankOptionNumRate + iOptionRate)
					{
						iRankOptionNum = this->m_JewelItemOptionData[n].RankOptionNum;
						iSuccessRank = this->m_JewelItemOptionData[n].ItemRank;
						bRateFind = true;
						break;
					}

					iOptionRate += this->m_JewelItemOptionData[n].RankOptionNumRate;
				}
			}

			if (bRateFind == false)
			{
				lpObj->m_PlayerData->m_PentagramMixLock = FALSE;

				return PENTAGRAM_MIX_REFINE_NOTFOUND;
			}

			for (int i = 0; i < CHAOS_BOX_SIZE; i++)
			{
				if (lpObj->m_PlayerData->pPentagramMixBox[i].IsItem() == TRUE &&
					lpObj->m_PlayerData->pPentagramMixBox[i].m_Type == iPentagramJewelID)
				{
					lpObj->m_PlayerData->pPentagramMixBox[i].m_SocketOption[iSuccessRank - 1] = iRankOptionNum;
					lpObj->m_PlayerData->pPentagramMixBox[i].m_Level = 0;
					BYTE btMainAttribute = lpObj->m_PlayerData->pPentagramMixBox[i].m_BonusSocketOption & 0x0F;
					lpObj->m_PlayerData->pPentagramMixBox[i].m_BonusSocketOption = btMainAttribute | 0x10 * iSuccessRank;
					Item.m_Level = lpObj->m_PlayerData->pPentagramMixBox[i].m_Level;
					Item.m_Durability = lpObj->m_PlayerData->pPentagramMixBox[i].m_Durability;
					Item.m_Number = lpObj->m_PlayerData->pPentagramMixBox[i].m_Number;

					Item.Convert(lpObj->m_PlayerData->pPentagramMixBox[i].m_Type, lpObj->m_PlayerData->pPentagramMixBox[i].m_Option1,
						lpObj->m_PlayerData->pPentagramMixBox[i].m_Option2, lpObj->m_PlayerData->pPentagramMixBox[i].m_Option3,
						lpObj->m_PlayerData->pPentagramMixBox[i].m_NewOption, lpObj->m_PlayerData->pPentagramMixBox[i].m_SetOption,
						0, lpObj->m_PlayerData->pPentagramMixBox[i].m_SocketOption, lpObj->m_PlayerData->pPentagramMixBox[i].m_BonusSocketOption, 0, CURRENT_DB_VERSION);
					break;
				}
			}

			PMSG_CHAOSMIXRESULT pMsg;
			this->PentagramMixBoxInit(lpObj);

			PHeadSetB((LPBYTE)&pMsg, 0x86, sizeof(pMsg));
			pMsg.Result = CB_ERROR;
			ItemByteConvert(pMsg.ItemInfo, Item);

			BYTE btResult = gObjPentagramMixBoxInsertItem(iIndex, Item);

			if (btResult == 0xFF)
			{
				btReturnValue = PENTAGRAM_MIX_REFINE_NOTFOUND;
			}

			else
			{
				pMsg.Result = CB_SUCCESS;
				gObjPentagramMixItemSet(iIndex, btResult, 1);
				IOCP.DataSend(iIndex, (LPBYTE)&pMsg, pMsg.h.size);

				BYTE ExOption[MAX_EXOPTION_SIZE];
				ItemIsBufExOption(ExOption, &Item);

				btReturnValue = TRUE;
			}
		}
	}

	else if (iUpgradeType == 0)
	{
		int iItemTargetLevel = 0;
		int iUpgradeSuccessRate = 0;
		int iPentagramMixNeedMoney = 0;
		int iSourceJewelType = 0;
		int iJewelRank = 0;
		int iCurRank = 0;
		int iMainAttribute = 0;
		BYTE bPentagramChaosCharm = 0;
		BYTE bPentagramLuckyCharm = 0;
		BYTE iPentagramLuckyCharmRate = 0;
		BYTE bSourceItem = 0;
		int iSourceItemCount[4] = { 0, 0, 0, 0 };
		int iOriginJewelCnt = 0;

		for (int i = 0; i < CHAOS_BOX_SIZE; i++)
		{
			if (lpObj->m_PlayerData->pPentagramMixBox[i].IsItem() == TRUE)
			{
				for (int j = 0; j < 4; j++)
				{
					if (lpObj->m_PlayerData->pPentagramMixBox[i].m_Type != ITEMGET(this->m_JewelLevelUpgradeNeedItemData[iTargetValue - 1].NeedSourceItem_Type[0], this->m_JewelLevelUpgradeNeedItemData[iTargetValue - 1].NeedSourceItem_Index[0])
						&& lpObj->m_PlayerData->pPentagramMixBox[i].m_Type != ITEMGET(this->m_JewelLevelUpgradeNeedItemData[iTargetValue - 1].NeedSourceItem_Type[1], this->m_JewelLevelUpgradeNeedItemData[iTargetValue - 1].NeedSourceItem_Index[1])
						&& lpObj->m_PlayerData->pPentagramMixBox[i].m_Type != ITEMGET(this->m_JewelLevelUpgradeNeedItemData[iTargetValue - 1].NeedSourceItem_Type[2], this->m_JewelLevelUpgradeNeedItemData[iTargetValue - 1].NeedSourceItem_Index[2])
						&& lpObj->m_PlayerData->pPentagramMixBox[i].m_Type != ITEMGET(this->m_JewelLevelUpgradeNeedItemData[iTargetValue - 1].NeedSourceItem_Type[3], this->m_JewelLevelUpgradeNeedItemData[iTargetValue - 1].NeedSourceItem_Index[3]))
					{
						if (lpObj->m_PlayerData->pPentagramMixBox[i].m_Type == ITEMGET(14, 189))
						{
							bPentagramLuckyCharm = TRUE;
							iPentagramLuckyCharmRate = lpObj->m_PlayerData->pPentagramMixBox[i].m_Durability;
							bSourceItem = TRUE;
						}

						else if (lpObj->m_PlayerData->pPentagramMixBox[i].m_Type == ITEMGET(14, 190))
						{
							bPentagramChaosCharm = TRUE;
							bSourceItem = TRUE;
						}

						else if (iOriginJewelCnt > 1)
						{
							lpObj->m_PlayerData->m_PentagramMixLock = FALSE;

							return PENTAGRAM_MIX_ITEMS_COUNT_MISMATCH;
						}

						else
						{
							iItemTargetLevel = lpObj->m_PlayerData->pPentagramMixBox[i].m_Level;
							iItemTargetLevel++;
							iSourceJewelType = lpObj->m_PlayerData->pPentagramMixBox[i].m_Type;
							iCurRank = (lpObj->m_PlayerData->pPentagramMixBox[i].m_BonusSocketOption & 0xF0) >> 4;
							iMainAttribute = lpObj->m_PlayerData->pPentagramMixBox[i].m_BonusSocketOption & 0x0F;
							iOriginJewelCnt++;
							bSourceItem = TRUE;
						}

						break;
					}

					if (lpObj->m_PlayerData->pPentagramMixBox[i].m_Type == ITEMGET(this->m_JewelLevelUpgradeNeedItemData[iTargetValue - 1].NeedSourceItem_Type[j], this->m_JewelLevelUpgradeNeedItemData[iTargetValue - 1].NeedSourceItem_Index[j]))
					{
						iSourceItemCount[j]++;
						bSourceItem = TRUE;
						break;
					}
				}

				if (bSourceItem == TRUE)
				{
					BYTE ExOption[MAX_EXOPTION_SIZE];
					ItemIsBufExOption(ExOption, &lpObj->m_PlayerData->pPentagramMixBox[i]);

					bSourceItem = FALSE;
				}
			}
		}

		if (bPentagramChaosCharm == TRUE && bPentagramLuckyCharm == TRUE)
		{
			lpObj->m_PlayerData->m_PentagramMixLock = FALSE;

			return PENTAGRAM_MIX_ITEMS_COUNT_MISMATCH;
		}

		if (iItemTargetLevel != iTargetValue)
		{
			lpObj->m_PlayerData->m_PentagramMixLock = FALSE;

			return PENTAGRAM_MIX_REFINE_NOTFOUND;
		}

		for (int i = 0; i < 11; i++)
		{
			if (this->m_JewelLevelUpgradeNeedItemData[i].TargetItemLevel == iItemTargetLevel)
			{
				for (int j = 0; j < 4; j++)
				{
					if (this->m_JewelLevelUpgradeNeedItemData[i].NeedSourceItem_Num[j] != iSourceItemCount[j])
					{
						lpObj->m_PlayerData->m_PentagramMixLock = FALSE;

						return PENTAGRAM_MIX_ITEMS_COUNT_MISMATCH;
					}
				}
				break;
			}
		}

		for (int i = 0; i < 254; i++)
		{
			if (ITEMGET(this->m_JewelItemOptionData[i].PentagramJewel_Type, this->m_JewelItemOptionData[i].PentagramJewel_Index) == iSourceJewelType &&
				this->m_JewelItemOptionData[i].ItemRank == iCurRank)
			{
				iUpgradeSuccessRate = this->m_JewelItemOptionData[i].LevelUpgradeValue[iItemTargetLevel];
				break;
			}
		}

		if (bPentagramLuckyCharm)
			iUpgradeSuccessRate += 100 * iPentagramLuckyCharmRate;

		for (int level = 0; level < 10; level++)
		{
			if (this->m_JewelLevelUpgradeNeedItemData[level].TargetItemLevel == iItemTargetLevel)
			{
				iPentagramMixNeedMoney = this->m_JewelLevelUpgradeNeedItemData[level].NeedMoney;
				break;
			}
		}

		int iChaosTaxMoney = iPentagramMixNeedMoney * g_CastleSiegeSync.GetTaxRateChaos(iIndex) / 100;
		if (iChaosTaxMoney < 0)
			iChaosTaxMoney = 0;

		iPentagramMixNeedMoney += iChaosTaxMoney;
		if (iPentagramMixNeedMoney < 0)
			iPentagramMixNeedMoney = 0;

		if (lpObj->m_PlayerData->Money < iPentagramMixNeedMoney)
		{
			lpObj->m_PlayerData->m_PentagramMixLock = FALSE;

			return PENTAGRAM_MIX_NOT_ENOUGH_ZEN;
		}

		lpObj->m_PlayerData->Money -= iPentagramMixNeedMoney;
		g_CastleSiegeSync.AddTributeMoney(iChaosTaxMoney);
		GSProtocol.GCMoneySend(iIndex, lpObj->m_PlayerData->Money);

		int iRandRate = rand() % 10000;

		if (iRandRate >= iUpgradeSuccessRate)
		{
			for (int i = 0; i < CHAOS_BOX_SIZE; i++)
			{
				if (lpObj->m_PlayerData->pPentagramMixBox[i].IsItem() == TRUE && lpObj->m_PlayerData->pPentagramMixBox[i].m_Type == iSourceJewelType)
				{
					BYTE ExOption[MAX_EXOPTION_SIZE];
					ItemIsBufExOption(ExOption, &lpObj->m_PlayerData->pPentagramMixBox[i]);

					break;
				}
			}

			if (bPentagramChaosCharm == TRUE)
			{
				CItem Item;
				BYTE ItemType;
				WORD ItemIndex;
				int iLevel;

				for (int i = 0; i < CHAOS_BOX_SIZE; i++)
				{
					if (lpObj->m_PlayerData->pPentagramMixBox[i].IsItem() == TRUE
						&& lpObj->m_PlayerData->pPentagramMixBox[i].m_Type == iSourceJewelType)
					{
						ItemType = ITEM_GET_TYPE(lpObj->m_PlayerData->pPentagramMixBox[i].m_Type);
						ItemIndex = ITEM_GET_INDEX(lpObj->m_PlayerData->pPentagramMixBox[i].m_Type);
						Item.m_SocketOption[iCurRank - 1] &= 0x0F;
						Item.m_Level = lpObj->m_PlayerData->pPentagramMixBox[i].m_Level;
						iLevel = Item.m_Level;
						Item.m_Durability = lpObj->m_PlayerData->pPentagramMixBox[i].m_Durability;
						Item.m_Number = lpObj->m_PlayerData->pPentagramMixBox[i].m_Number;

						Item.Convert(lpObj->m_PlayerData->pPentagramMixBox[i].m_Type, lpObj->m_PlayerData->pPentagramMixBox[i].m_Option1,
							lpObj->m_PlayerData->pPentagramMixBox[i].m_Option2, lpObj->m_PlayerData->pPentagramMixBox[i].m_Option3,
							lpObj->m_PlayerData->pPentagramMixBox[i].m_NewOption, lpObj->m_PlayerData->pPentagramMixBox[i].m_SetOption,
							0, lpObj->m_PlayerData->pPentagramMixBox[i].m_SocketOption, lpObj->m_PlayerData->pPentagramMixBox[i].m_BonusSocketOption, 0, CURRENT_DB_VERSION);
						break;
					}
				}

				PMSG_CHAOSMIXRESULT pMsg;
				this->PentagramMixBoxInit(lpObj);

				PHeadSetB((LPBYTE)&pMsg, 0x86, sizeof(pMsg));
				pMsg.Result = CB_ERROR;
				ItemByteConvert(pMsg.ItemInfo, Item);

				BYTE btResult = gObjPentagramMixBoxInsertItem(iIndex, Item);

				if (btResult == 0xFF)
				{
					btReturnValue = PENTAGRAM_MIX_REFINE_NOTFOUND;
				}

				else
				{
					pMsg.Result = PENTAGRAM_MIX_FAILED_WITH_TALISMAN;
					gObjPentagramMixItemSet(iIndex, btResult, 1);
					IOCP.DataSend(iIndex, (LPBYTE)&pMsg, pMsg.h.size);
					btReturnValue = TRUE;
				}
			}

			else
			{
				this->PentagramMixBoxInit(lpObj);
				btReturnValue = PENTAGRAM_MIX_UPGRADE_FAILED;
			}
		}

		else
		{
			CItem pModifyItem;

			BYTE iItemType = 0xFF;
			WORD iItemIndex = 0xFFFF;

			for (int i = 0; i < CHAOS_BOX_SIZE; i++)
			{
				if (lpObj->m_PlayerData->pPentagramMixBox[i].IsItem() == TRUE && lpObj->m_PlayerData->pPentagramMixBox[i].m_Type == iSourceJewelType)
				{
					BYTE btRankOptionNum = lpObj->m_PlayerData->pPentagramMixBox[i].m_SocketOption[iCurRank - 1] & 0x0F;
					lpObj->m_PlayerData->pPentagramMixBox[i].m_Level = iItemTargetLevel;
					lpObj->m_PlayerData->pPentagramMixBox[i].m_SocketOption[iCurRank - 1] = btRankOptionNum | 0x10 * iItemTargetLevel;
					iItemType = ITEM_GET_TYPE(lpObj->m_PlayerData->pPentagramMixBox[i].m_Type);
					iItemIndex = ITEM_GET_INDEX(lpObj->m_PlayerData->pPentagramMixBox[i].m_Type);
					pModifyItem.m_Level = lpObj->m_PlayerData->pPentagramMixBox[i].m_Level;
					pModifyItem.m_Durability = lpObj->m_PlayerData->pPentagramMixBox[i].m_Durability;
					pModifyItem.m_Number = lpObj->m_PlayerData->pPentagramMixBox[i].m_Number;

					pModifyItem.Convert(lpObj->m_PlayerData->pPentagramMixBox[i].m_Type, lpObj->m_PlayerData->pPentagramMixBox[i].m_Option1,
						lpObj->m_PlayerData->pPentagramMixBox[i].m_Option2, lpObj->m_PlayerData->pPentagramMixBox[i].m_Option3,
						lpObj->m_PlayerData->pPentagramMixBox[i].m_NewOption, lpObj->m_PlayerData->pPentagramMixBox[i].m_SetOption,
						0, lpObj->m_PlayerData->pPentagramMixBox[i].m_SocketOption, lpObj->m_PlayerData->pPentagramMixBox[i].m_BonusSocketOption, 0, CURRENT_DB_VERSION);
					break;
				}
			}

			PMSG_CHAOSMIXRESULT pMsg;
			this->PentagramMixBoxInit(lpObj);

			PHeadSetB((LPBYTE)&pMsg, 0x86, sizeof(pMsg));
			pMsg.Result = CB_ERROR;
			ItemByteConvert(pMsg.ItemInfo, pModifyItem);

			BYTE btResult = gObjPentagramMixBoxInsertItem(iIndex, pModifyItem);

			if (btResult == 0xFF)
			{
				btReturnValue = PENTAGRAM_MIX_REFINE_NOTFOUND;
			}

			else
			{
				pMsg.Result = CB_SUCCESS;
				gObjPentagramMixItemSet(iIndex, btResult, 1);
				IOCP.DataSend(iIndex, (LPBYTE)&pMsg, pMsg.h.size);

				BYTE ExOption[MAX_EXOPTION_SIZE];
				ItemIsBufExOption(ExOption, &pModifyItem);

				btReturnValue = TRUE;
			}
		}
	}

	lpObj->m_PlayerData->m_PentagramMixLock = FALSE;
	gObjInventoryCommit(iIndex);

	return btReturnValue;
}

int CPentagramMixSystem::CheckLevelCondition(CItem *lpItem, WORD Level, BYTE Op1, BYTE  Op2, BYTE Op3, BYTE SetOption, BYTE NewOption)
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

void CPentagramMixSystem::LogPentagramItem(LPOBJ lpObj, char* szLogType)
{
	return; // Function not used
}

void CPentagramMixSystem::CGPentagramJewelRefineRecv(PMSG_PENTAGRAM_JEWEL_REFINE_RECV* lpMsg, int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if (lpObj->m_bPShopOpen != 0)
	{
		return;
	}

	switch (lpMsg->type)
	{
	case 1:
		//this->PentagramJewelMix(aIndex);
		g_Log.AddC(TColor::Red, "[PentagramJewelMix] in Progress..");
	    break;
	default:
		g_Log.Add("[%s][%s] Undefine PentagramJewelMix type detect %d", lpObj->AccountID, lpObj->Name, lpMsg->type);
		break;
	}
}

bool CPentagramMixSystem::IsErrtelType(int ItemCode)
{
	switch (ItemCode)
	{
	case ITEMGET(12, 221):
	case ITEMGET(12, 231):
	case ITEMGET(12, 241):
	case ITEMGET(12, 251):
	case ITEMGET(12, 261):
		return true;
	default:
		return false;
	}
}

BOOL CPentagramMixSystem::PentagramJewelMix(int aIndex) // OK
{
	LPOBJ lpObj = &gObj[aIndex];

	lpObj->ChaosLock = TRUE;

	PMSG_CHAOSMIXRESULT pMsg;

	PHeadSetB((LPBYTE)&pMsg.h, 0x86, sizeof(PMSG_CHAOSMIXRESULT));
	pMsg.Result = 1;

	int iMithrilCount = 0;
	int iMithrilType = -1;
	int iBlessCount = 0;

	if (gObj[aIndex].pInventory->m_Type == ITEMGET(12, 145))
	{
		iMithrilType = gObj[aIndex].pInventory->m_BonusSocketOption;
		iMithrilCount++;
	}
	
	if (gObj[aIndex].pInventory->m_Type == ITEMGET(14, 13))
	{
		iBlessCount++;
	}
		
	if (iMithrilCount != 1 || iBlessCount != 1)
	{
		IOCP.DataSend(aIndex, (UCHAR*)&pMsg, pMsg.h.size);
		return 0;
	}
		

	if (gObj[aIndex].m_PlayerData->Money <= 100000)
	{
		IOCP.DataSend(aIndex, (LPBYTE)&pMsg, pMsg.h.size);
		return 0; //No Zen
	}

	gObj[aIndex].m_PlayerData->Money -= 100000;
	GSProtocol.GCMoneySend(aIndex, gObj[aIndex].m_PlayerData->Money);

	if ((rand() % 100) < 80)
	{
		pMsg.Result = 1;

		int type = rand() % 5;

		if (type == 0)
		{
			ITEMGET(12, 221);
		}
		else if (type == 1)
		{
			ITEMGET(12, 231);
		}
		else if (type == 2)
		{
			ITEMGET(12, 241);
		}
		else if (type == 3)
		{
			ITEMGET(12, 251);
		}
		else if (type == 4)
		{
			ITEMGET(12, 261);
		}
		
		CItem NewItem;
		NewItem.m_Type = type;
		NewItem.m_SocketOption[0] = gObj[aIndex].pInventory->m_SocketOption[0];
		NewItem.m_SocketOption[1] = gObj[aIndex].pInventory->m_SocketOption[1];
		NewItem.m_SocketOption[2] = gObj[aIndex].pInventory->m_SocketOption[2];
		NewItem.m_SocketOption[3] = gObj[aIndex].pInventory->m_SocketOption[3];
		NewItem.m_SocketOption[4] = gObj[aIndex].pInventory->m_SocketOption[4];
		NewItem.m_BonusSocketOption = gObj[aIndex].pInventory->m_BonusSocketOption;
		
		ItemSerialCreateSend(aIndex, 235, 0, 0, NewItem.m_Type, 0, 0, 0, 0, 0, aIndex, 0, 0, 0, NewItem.m_SocketOption, NewItem.m_BonusSocketOption);
		gObjInventoryCommit(aIndex);

		g_Log.AddC(TColor::Red,"[Elemental System][Elixir Mix] [%s][%s] CBMix Success, Money : %d",
			lpObj->AccountID, lpObj->Name, lpObj->m_PlayerData->Money);
	}
	else
	{
		pMsg.Result = 0;
		IOCP.DataSend(aIndex, (UCHAR*)&pMsg, pMsg.h.size);
		g_Log.AddC(TColor::Red, "[Elemental System][Elixir Mix] [%s][%s] CBMix Fail, Money : %d ",
			lpObj->AccountID, lpObj->Name, lpObj->m_PlayerData->Money);
	}
	return 1;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

