
#include "JewelMixSystem.h"
#include "PeriodItemEx.h"
#include "Gameprotocol.h"
#include "User/CUserData.h"

int CJewelMixSystem::UnMixJewelPrice;

CJewelMixSystem::CJewelMixSystem()
{
	return;
}

CJewelMixSystem::~CJewelMixSystem()
{
	return;
}

int CJewelMixSystem::GetJewelCount(CGameObject &Obj, int iJewelType)
{
	if ( !gObjIsConnected(Obj.m_Index) )
		return -1;

	int iItemType;

	switch ( iJewelType )
	{
		case 0:
			iItemType = ITEMGET(14,13);
			break;
		case 1:
			iItemType = ITEMGET(14,14);
			break;
		default:
			return -1;
	}	
	
	int iItemCount = gObjGetItemCountInIventory(iIndex, iItemType);
	return iItemCount;
}


struct JEWELMIX_INFO
{
	int m_iJewelCount;	// 0
	int m_iMixMoney;	// 4
	int m_iSourceType;	// 8
	int m_iChangeType;	// C
};

static JEWELMIX_INFO g_JewelMixInfo[10][3] = { 10, 500000, ITEMGET(14,13), ITEMGET(12,30),
												   20, 1000000, ITEMGET(14,13), ITEMGET(12,30),
												   30, 1500000, ITEMGET(14,13), ITEMGET(12,30), // bless
												   10, 500000, ITEMGET(14,14), ITEMGET(12,31),
												   20, 1000000, ITEMGET(14,14), ITEMGET(12,31),
												   30, 1500000, ITEMGET(14,14), ITEMGET(12,31), // soul
												   10, 500000, ITEMGET(14,16), ITEMGET(12,136),
												   20, 1000000, ITEMGET(14,16), ITEMGET(12,136),
												   30, 1500000, ITEMGET(14,16), ITEMGET(12,136), // life
												   10, 500000, ITEMGET(14,22), ITEMGET(12,137),
												   20, 1000000, ITEMGET(14,22), ITEMGET(12,137),
												   30, 1500000, ITEMGET(14,22), ITEMGET(12,137), //creation
												   10, 500000, ITEMGET(14,31), ITEMGET(12,138),
												   20, 1000000, ITEMGET(14,31), ITEMGET(12,138),
												   30, 1500000, ITEMGET(14,31), ITEMGET(12,138), // guardian
												   10, 500000, ITEMGET(14,41), ITEMGET(12,139),
												   20, 1000000, ITEMGET(14,41), ITEMGET(12,139),
												   30, 1500000, ITEMGET(14,41), ITEMGET(12,139), // gemstone
												   10, 500000, ITEMGET(14,42), ITEMGET(12,140),
												   20, 1000000, ITEMGET(14,42), ITEMGET(12,140),
												   30, 1500000, ITEMGET(14,42), ITEMGET(12,140), // harmony
												   10, 500000, ITEMGET(12,15), ITEMGET(12,141),
												   20, 1000000, ITEMGET(12,15), ITEMGET(12,141),
												   30, 1500000, ITEMGET(12,15), ITEMGET(12,141), // chaos
												   10, 500000, ITEMGET(14,43), ITEMGET(12,142),
												   20, 1000000, ITEMGET(14,43), ITEMGET(12,142),
												   30, 1500000, ITEMGET(14,43), ITEMGET(12,142), // lower refine stone
												   10, 500000, ITEMGET(14,44), ITEMGET(12,143),
												   20, 1000000, ITEMGET(14,44), ITEMGET(12,143),
												   30, 1500000, ITEMGET(14,44), ITEMGET(12,143), // Higher refining stone

};

void CJewelMixSystem::LoadMixJewelPrice() // PRIVATE CONFIG FOR JOINMU / NEWYORKER
{
	CIniReader ReadJewelMix(g_ConfigRead.GetPath("IGC_Common.ini"));

	g_JewelMixInfo[0][0].m_iMixMoney = ReadJewelMix.ReadInt("joinmu", "Bless10MixPrice", 500000);
	g_JewelMixInfo[0][1].m_iMixMoney = ReadJewelMix.ReadInt("joinmu", "Bless20MixPrice", 1000000);
	g_JewelMixInfo[0][2].m_iMixMoney = ReadJewelMix.ReadInt("joinmu", "Bless30MixPrice", 1500000);

	g_JewelMixInfo[1][0].m_iMixMoney = ReadJewelMix.ReadInt("joinmu", "Soul10MixPrice", 500000);
	g_JewelMixInfo[1][1].m_iMixMoney = ReadJewelMix.ReadInt("joinmu", "Soul20MixPrice", 1000000);
	g_JewelMixInfo[1][2].m_iMixMoney = ReadJewelMix.ReadInt("joinmu", "Soul30MixPrice", 1500000);

	g_JewelMixInfo[2][0].m_iMixMoney = ReadJewelMix.ReadInt("joinmu", "Life10MixPrice", 500000);
	g_JewelMixInfo[2][1].m_iMixMoney = ReadJewelMix.ReadInt("joinmu", "Life20MixPrice", 1000000);
	g_JewelMixInfo[2][2].m_iMixMoney = ReadJewelMix.ReadInt("joinmu", "Life30MixPrice", 1500000);

	g_JewelMixInfo[3][0].m_iMixMoney = ReadJewelMix.ReadInt("joinmu", "Creation10MixPrice", 500000);
	g_JewelMixInfo[3][1].m_iMixMoney = ReadJewelMix.ReadInt("joinmu", "Creation20MixPrice", 1000000);
	g_JewelMixInfo[3][2].m_iMixMoney = ReadJewelMix.ReadInt("joinmu", "Creation30MixPrice", 1500000);

	g_JewelMixInfo[4][0].m_iMixMoney = ReadJewelMix.ReadInt("joinmu", "Guardian10MixPrice", 500000);
	g_JewelMixInfo[4][1].m_iMixMoney = ReadJewelMix.ReadInt("joinmu", "Guardian20MixPrice", 1000000);
	g_JewelMixInfo[4][2].m_iMixMoney = ReadJewelMix.ReadInt("joinmu", "Guardian30MixPrice", 1500000);

	g_JewelMixInfo[5][0].m_iMixMoney = ReadJewelMix.ReadInt("joinmu", "Gemstone10MixPrice", 500000);
	g_JewelMixInfo[5][1].m_iMixMoney = ReadJewelMix.ReadInt("joinmu", "Gemstone20MixPrice", 1000000);
	g_JewelMixInfo[5][2].m_iMixMoney = ReadJewelMix.ReadInt("joinmu", "Gemstone30MixPrice", 1500000);

	g_JewelMixInfo[6][0].m_iMixMoney = ReadJewelMix.ReadInt("joinmu", "Harmony10MixPrice", 500000);
	g_JewelMixInfo[6][1].m_iMixMoney = ReadJewelMix.ReadInt("joinmu", "Harmony20MixPrice", 1000000);
	g_JewelMixInfo[6][2].m_iMixMoney = ReadJewelMix.ReadInt("joinmu", "Harmony30MixPrice", 1500000);

	g_JewelMixInfo[7][0].m_iMixMoney = ReadJewelMix.ReadInt("joinmu", "Chaos10MixPrice", 500000);
	g_JewelMixInfo[7][1].m_iMixMoney = ReadJewelMix.ReadInt("joinmu", "Chaos20MixPrice", 1000000);
	g_JewelMixInfo[7][2].m_iMixMoney = ReadJewelMix.ReadInt("joinmu", "Chaos30MixPrice", 1500000);

	g_JewelMixInfo[8][0].m_iMixMoney = ReadJewelMix.ReadInt("joinmu", "LowerRefineStone10MixPrice", 500000);
	g_JewelMixInfo[8][1].m_iMixMoney = ReadJewelMix.ReadInt("joinmu", "LowerRefineStone20MixPrice", 1000000);
	g_JewelMixInfo[8][2].m_iMixMoney = ReadJewelMix.ReadInt("joinmu", "LowerRefineStone30MixPrice", 1500000);

	g_JewelMixInfo[9][0].m_iMixMoney = ReadJewelMix.ReadInt("joinmu", "HigherRefineStone10MixPrice", 500000);
	g_JewelMixInfo[9][1].m_iMixMoney = ReadJewelMix.ReadInt("joinmu", "HigherRefineStone20MixPrice", 1000000);
	g_JewelMixInfo[9][2].m_iMixMoney = ReadJewelMix.ReadInt("joinmu", "HigherRefineStone30MixPrice", 1500000);

	CJewelMixSystem::UnMixJewelPrice = ReadJewelMix.ReadInt("joinmu", "UnMixJewelPrice", 1000000);
}
									 

int CJewelMixSystem::GetJewelCountPerLevel(int iJewelType, int iJewelLevel)
{
	if ( !CHECK_LIMIT(iJewelLevel, 3) )
		return -1;

	int iJewelCount = 0;

	switch ( iJewelType )
	{
		case 0:
			iJewelCount = g_JewelMixInfo[0][iJewelLevel].m_iJewelCount;
			break;
		case 1:
			iJewelCount = g_JewelMixInfo[1][iJewelLevel].m_iJewelCount;
			break;
		case 2:
			iJewelCount = g_JewelMixInfo[2][iJewelLevel].m_iJewelCount;
			break;
		case 3:
			iJewelCount = g_JewelMixInfo[3][iJewelLevel].m_iJewelCount;
			break;
		case 4:
			iJewelCount = g_JewelMixInfo[4][iJewelLevel].m_iJewelCount;
			break;
		case 5:
			iJewelCount = g_JewelMixInfo[5][iJewelLevel].m_iJewelCount;
			break;
		case 6:
			iJewelCount = g_JewelMixInfo[6][iJewelLevel].m_iJewelCount;
			break;
		case 7:
			iJewelCount = g_JewelMixInfo[7][iJewelLevel].m_iJewelCount;
			break;
		case 8:
			iJewelCount = g_JewelMixInfo[8][iJewelLevel].m_iJewelCount;
			break;
		case 9:
			iJewelCount = g_JewelMixInfo[9][iJewelLevel].m_iJewelCount;
			break;
		/*case 10: // array overrun
			iJewelCount = g_JewelMixInfo[10][iJewelLevel].m_iJewelCount;
			break;*/
		default:
			return -1;
	}

	return iJewelCount;
}

BOOL CJewelMixSystem::MixJewel( int iIndex, int iJewelType, int iMixType)
{
	if ( !gObjIsConnected(Obj.m_Index))
		return FALSE;

/*	if ( gObjCheckInventorySerial0Item(Obj))
	{
		MsgOutput(iIndex, lMsg.Get(MSGGET(13,26)));
		GCAnsJewelMix(iIndex, 4);

		sLog->outBasic("[ANTI-HACK][Serial 0 Item] [Mix Jewel] (%s)(%s)",
			Obj.AccountID, Obj.Name);

		return FALSE;
	}*/

	if ( Obj.m_IfState.use != 1 || Obj.m_IfState.type != 12 )
	{
		sLog->outBasic("[ANTI-HACK][protocol] [Mix Jewel] (%s)(%s)",
			Obj.AccountID, Obj.Name);

		return FALSE;
	}

	if ( Obj.ChaosLock == TRUE )
	{
		sLog->outBasic("[JewelMix] [%s][%s] Chaos Mix is already working",
			Obj.AccountID, Obj.Name);

		gGameProtocol.GCAnsJewelMix(iIndex, 0);
		return FALSE;
	}

	Obj.ChaosLock = TRUE;

	if ( !CHECK_LIMIT(iJewelType, 10) )
	{
		sLog->outBasic("[JewelMix] [%s][%s] Mix iJewelType is out of bound : %d",
			Obj.AccountID, Obj.Name, iJewelType);

		Obj.ChaosLock = FALSE;
		gGameProtocol.GCAnsJewelMix(iIndex, 2);
		return FALSE;
	}

	if ( !CHECK_LIMIT(iMixType, 10))
	{
		sLog->outBasic("[JewelMix] [%s][%s] iMixType is out of bound : %d",
			Obj.AccountID, Obj.Name, iMixType);

		Obj.ChaosLock = FALSE;
		gGameProtocol.GCAnsJewelMix(iIndex, 3);
		return FALSE;
	}

	int iItemType = g_JewelMixInfo[iJewelType][iMixType].m_iSourceType;
	int iJewelCount = g_JewelMixInfo[iJewelType][iMixType].m_iJewelCount;
	int iMixMoney = g_JewelMixInfo[iJewelType][iMixType].m_iMixMoney;
	int iChangeType = g_JewelMixInfo[iJewelType][iMixType].m_iChangeType;

	if ( iJewelCount <= 0 )
	{
		Obj.ChaosLock = FALSE;
		gGameProtocol.GCAnsJewelMix(iIndex, 0);
		return FALSE;
	}

	int iUserJewelCount = gObjGetItemCountInIventory(iIndex, iItemType);

	if ( iJewelCount > iUserJewelCount )
	{
		sLog->outBasic("[JewelMix] [%s][%s] lack of jewel to mix : %d / %d",
			Obj.AccountID, Obj.Name,
			iUserJewelCount, iJewelCount);

		Obj.ChaosLock = FALSE;
		gGameProtocol.GCAnsJewelMix(iIndex, 4);
		return FALSE;
	}

	if ( iMixMoney > Obj.m_PlayerData->Money )
	{
		sLog->outBasic("[JewelMix] [%s][%s] lack of money to mix : %d / %d",
			Obj.AccountID, Obj.Name,
			Obj.m_PlayerData->Money, iMixMoney);

		Obj.ChaosLock = FALSE;
		gGameProtocol.GCAnsJewelMix(iIndex, 5);
		return FALSE;
	}

	int iDelJewelCount = 0;
	BOOL bItemDelOK = FALSE;

	for ( int x= INVETORY_WEAR_SIZE ; x< MAIN_INVENTORY_SIZE; x++)
	{
		if ( Obj.pInventory[x]->IsItem() == TRUE )
		{
			if ( Obj.pInventory[x]->m_Type == iItemType )
			{
				sLog->outBasic("[JewelMix] [%s][%s] Mix - Delete Jewel, Type:%d, Level:%d, Serial:%I64d",
					Obj.AccountID, Obj.Name,
					Obj.pInventory[x]->m_Type,
					Obj.pInventory[x]->m_Level,
					Obj.pInventory[x]->m_Number);

				gObjInventoryItemSet(iIndex, x, 0xFF);
				Obj.pInventory[x]->Clear();
				iDelJewelCount++;

				if ( iJewelCount <= iDelJewelCount )
				{
					bItemDelOK = TRUE;
					break;
				}
			}
		}
	}

	GCItemListSend(Obj.m_Index);
	g_PeriodItemEx.OnRequestPeriodItemList(Obj);

	if ( bItemDelOK == FALSE )
	{
		sLog->outBasic("[JewelMix] [%s][%s] lack of jewel to mix (in deleting) : %d / %d",
			Obj.AccountID, Obj.Name,
			iDelJewelCount, iJewelCount);

		Obj.ChaosLock = FALSE;
		gGameProtocol.GCAnsJewelMix(iIndex, 0);
		return FALSE;
	}

	ItemSerialCreateSend(iIndex, 235, Obj.X,
						Obj.Y, iChangeType,
						iMixType, 0, 0, 0, 0,
						iIndex, 0, 0, 0, 0, 0);

	Obj.ChaosLock = FALSE;
	Obj.m_PlayerData->Money -= iMixMoney;
	gGameProtocol.GCMoneySend(iIndex, Obj.m_PlayerData->Money);
	gGameProtocol.GCAnsJewelMix(iIndex, 1);

	sLog->outBasic("[JewelMix] [%s][%s] jewel mix succeed : ItemType:%d, JewelCount:%d",
		Obj.AccountID, Obj.Name,
		iItemType, iJewelCount);

	return TRUE;
}

BOOL CJewelMixSystem::UnMixJewel(CGameObject &Obj, int iJewelType, int iJewelLevel, int iInventoryPos)
{
	if ( !gObjIsConnected(Obj.m_Index))
		return FALSE;

/*	if ( gObjCheckInventorySerial0Item(Obj))
	{
		MsgOutput(iIndex, lMsg.Get(MSGGET(13,26)));
		GCAnsJewelUnMix(iIndex, 0);

		sLog->outBasic("[ANTI-HACK][Serial 0 Item] [UnMix Jewel] (%s)(%s)",
			Obj.AccountID, Obj.Name);

		return FALSE;
	}*/

	if ( Obj.m_IfState.use != 1 || Obj.m_IfState.type != 12 )
	{
		sLog->outBasic("[ANTI-HACK][protocol] [UnMix Jewel] (%s)(%s)",
			Obj.AccountID, Obj.Name);

		return FALSE;
	}

	if ( Obj.ChaosLock == TRUE )
	{
		sLog->outBasic("[JewelMix] [%s][%s] Chaos Mix is already working",
			Obj.AccountID, Obj.Name);

		gGameProtocol.GCAnsJewelUnMix(iIndex, 0);
		return FALSE;
	}

	Obj.ChaosLock = TRUE;

	if ( !CHECK_LIMIT(iJewelType, 10) )
	{
		sLog->outBasic("[JewelMix] [%s][%s] UnMix iJewelType is out of bound : %d",
			Obj.AccountID, Obj.Name, iJewelType);

		Obj.ChaosLock = FALSE;
		gGameProtocol.GCAnsJewelUnMix(iIndex, 2);
		return FALSE;
	}

	if ( !CHECK_LIMIT(iInventoryPos, MAIN_INVENTORY_SIZE))
	{
		sLog->outBasic("[JewelMix] [%s][%s] iInventoryPos is out of bound : %d",
			Obj.AccountID, Obj.Name, iInventoryPos);

		Obj.ChaosLock = FALSE;
		gGameProtocol.GCAnsJewelUnMix(iIndex, 5);
		return FALSE;
	}

	int iInventoryItemType;
	int iInventoryItemLevel;

	if ( Obj.pInventory[iInventoryPos]->IsItem() == TRUE )
	{
		if ( Obj.pInventory[iInventoryPos]->m_Level == iJewelLevel )
		{
			iInventoryItemType = Obj.pInventory[iInventoryPos]->m_Type;
			iInventoryItemLevel = Obj.pInventory[iInventoryPos]->m_Level;
		}
		else
		{
			sLog->outBasic("[JewelMix] [%s][%s] iJewelLevel is different from request : %d / %d",
				Obj.AccountID, Obj.Name,
				Obj.pInventory[iInventoryPos]->m_Level, iJewelLevel);

			Obj.ChaosLock = FALSE;
			gGameProtocol.GCAnsJewelUnMix(iIndex, 3);
			return FALSE;
		}
	}
	else
	{
		sLog->outBasic("[JewelMix] [%s][%s] Item to unmix is not exist",
			Obj.AccountID, Obj.Name);
	
		Obj.ChaosLock = FALSE;
		gGameProtocol.GCAnsJewelUnMix(iIndex, 4);
		return FALSE;
	}

	if ( !CHECK_LIMIT(iInventoryItemLevel, 3))
	{
		sLog->outBasic("[JewelMix] [%s][%s] iInventoryItemLevel is out of bound : %d",
			Obj.AccountID, Obj.Name, iInventoryItemLevel);

		Obj.ChaosLock = FALSE;
		gGameProtocol.GCAnsJewelUnMix(iIndex, 3);
		return FALSE;
	}

	int iItemType = g_JewelMixInfo[iJewelType][iInventoryItemLevel].m_iChangeType;
	int iJewelCount = g_JewelMixInfo[iJewelType][iInventoryItemLevel].m_iJewelCount;
	int iChangeType = g_JewelMixInfo[iJewelType][iInventoryItemLevel].m_iSourceType;
	int iMixMoney = CJewelMixSystem::UnMixJewelPrice;

	if ( iInventoryItemType != iItemType )
	{
		sLog->outBasic("[JewelMix] [%s][%s] iItemType is different from request : %d / %d",
			Obj.AccountID, Obj.Name,
			iInventoryItemType, iItemType);

		Obj.ChaosLock = FALSE;
		gGameProtocol.GCAnsJewelUnMix(iIndex, 6);
		return FALSE;
	}

	if ( iMixMoney > Obj.m_PlayerData->Money )
	{
		sLog->outBasic("[JewelMix] [%s][%s] lack of money to unmix : %d / %d",
			Obj.AccountID, Obj.Name,
			Obj.m_PlayerData->Money, iMixMoney);

		Obj.ChaosLock = FALSE;
		gGameProtocol.GCAnsJewelUnMix(iIndex, 8);
		return FALSE;
	}

	int iEmptyCount = CheckInventoryEmptySpaceCount(Obj, 1, 1);

	if ( iEmptyCount < iJewelCount )
	{
		sLog->outBasic("[JewelMix] [%s][%s] lack of empty slot to unmix : %d / %d",
			Obj.AccountID, Obj.Name,
			iEmptyCount, iJewelCount);

		Obj.ChaosLock = FALSE;
		gGameProtocol.GCAnsJewelUnMix(iIndex, 7);
		return FALSE;
	}

	sLog->outBasic("[JewelMix] [%s][%s] UnMix - Delete Jewel, Type:%d, Level:%d, Serial:%I64d",
		Obj.AccountID, Obj.Name,
		Obj.pInventory[iInventoryPos]->m_Type,
		Obj.pInventory[iInventoryPos]->m_Level,
		Obj.pInventory[iInventoryPos]->m_Number);

	gObjInventoryItemSet(iIndex, iInventoryPos, 0xFF);
	gObjInventoryDeleteItem(iIndex, iInventoryPos);
	gGameProtocol.GCInventoryItemDeleteSend(iIndex, iInventoryPos, 1);

	int iCrtJewelCount = 0;
	BOOL bItemCrtOK = FALSE;

	for (int x = 0; x < iJewelCount; x++)
	{
		ItemSerialCreateSend(iIndex, 235, Obj.X, Obj.Y, iChangeType, 0, 0, 0, 0, 0, iIndex, 0, 0, 0, 0, 0);
		iCrtJewelCount++;

		if ( iJewelCount <= iCrtJewelCount )
		{
			bItemCrtOK = TRUE;
			break;
		}
	}

	Obj.ChaosLock = FALSE;

	if ( bItemCrtOK == TRUE )
	{
		sLog->outBasic("[JewelMix] [%s][%s] jewel unmix succeed : ItemType:%d, JewelCount:%d",
			Obj.AccountID, Obj.Name,
			iItemType, iJewelCount);

		Obj.m_PlayerData->Money -= iMixMoney;
		gGameProtocol.GCMoneySend(iIndex, Obj.m_PlayerData->Money);
		gGameProtocol.GCAnsJewelUnMix(iIndex, 1);
	}
	else
	{
		sLog->outBasic("[JewelMix] [%s][%s] jewel unmix failed : ItemType:%d, JewelCount:%d",
			Obj.AccountID, Obj.Name,
			iItemType, iJewelCount);

		gGameProtocol.GCAnsJewelUnMix(iIndex, 0);
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

