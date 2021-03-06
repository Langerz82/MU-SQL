////////////////////////////////////////////////////////////////////////////////
// CastleSiegeSync.cpp
#include "StdAfx.h"
#include "CastleSiegeSync.h"
#include "TUnion.h"
#include "GameProtocol.h"
#include "MapServerManager.h"

// GS-N 0.99.60T 0x0051B7C0 - Completed
//	GS-N	1.00.18	JPN	0x0054D150	-	Completed


CCastleSiegeSync::CCastleSiegeSync()
{
	this->Clear();
}


CCastleSiegeSync::~CCastleSiegeSync()
{
	return;
}
	

void CCastleSiegeSync::Clear()
{
	this->m_iCurCastleState = -1;
	this->m_iCurTaxRateChaos = 0;
	this->m_iCurTaxRateStore  = 0;
	this->m_lCastleTributeMoney = 0;
	this->m_iCsTributeMoneyTimer = 0;
	memset(this->m_szCastleOwnerGuild, 0, sizeof(this->m_szCastleOwnerGuild));
}



void CCastleSiegeSync::SetCastleOwnerGuild(char * lpszGuildName)
{
	if ( lpszGuildName == NULL )
	{
		return;
	}

	memset(this->m_szCastleOwnerGuild, 0, sizeof(this->m_szCastleOwnerGuild));
	std::memcpy(this->m_szCastleOwnerGuild, lpszGuildName, sizeof(this->m_szCastleOwnerGuild)/2);
}



int CCastleSiegeSync::GetTaxRateChaos(CGameObject &Obj)
{
	int iCurTaxRateChaos = this->m_iCurTaxRateChaos;

	if ( this->CheckCastleOwnerMember(Obj.m_Index) == TRUE || this->CheckCastleOwnerUnionMember(Obj.m_Index) == TRUE )
	{
		iCurTaxRateChaos = 0;
	}

	return iCurTaxRateChaos;
}



int CCastleSiegeSync::GetTaxRateStore(CGameObject &Obj)
{
	int iCurTaxRateStore = this->m_iCurTaxRateStore;

	if ( this->CheckCastleOwnerMember(Obj.m_Index) == TRUE || this->CheckCastleOwnerUnionMember(Obj.m_Index) == TRUE )
	{
		iCurTaxRateStore = 0;
	}

	return iCurTaxRateStore;
}



int CCastleSiegeSync::GetTaxHuntZone(CGameObject &Obj, BOOL bCheckOwnerGuild)
{
	int iCurTaxHuntZone = this->m_iCurTaxHuntZone;

	if ( bCheckOwnerGuild == TRUE )
	{
		if ( this->CheckCastleOwnerMember(Obj.m_Index) == TRUE || this->CheckCastleOwnerUnionMember(Obj.m_Index) == TRUE )
		{
			iCurTaxHuntZone = 0;
		}
	}

	return iCurTaxHuntZone;
}



void CCastleSiegeSync::AddTributeMoney(int iMoney)
{
	if ( this->m_lCastleTributeMoney < 0 )
	{
		InterlockedExchange((LPLONG)&this->m_lCastleTributeMoney, 0);
	}

	if ( iMoney <= 0 )
	{
		return;
	}

	if ( (this->m_lCastleTributeMoney + iMoney) > MAX_TRIBUTE_MONEY )
	{
		return;
	}

	InterlockedExchangeAdd((LPLONG)&this->m_lCastleTributeMoney, iMoney);
}



void CCastleSiegeSync::ResetTributeMoney()
{
	InterlockedExchange((LPLONG)&this->m_lCastleTributeMoney, 0);
}



void CCastleSiegeSync::AdjustTributeMoney()
{
	if ( this->m_lCastleTributeMoney < 0 )
	{
		InterlockedExchange((LPLONG)&this->m_lCastleTributeMoney, 0);
	}

	if ( this->m_lCastleTributeMoney == 0 )
	{
		return;
	}

	this->m_iCsTributeMoneyTimer++;

	this->m_iCsTributeMoneyTimer %= 180;

	if ( this->m_iCsTributeMoneyTimer != 0 )
	{
		return;
	}

	GS_GDReqCastleTributeMoney(g_MapServerManager.GetMapSvrGroup(), this->m_lCastleTributeMoney);
}



BOOL CCastleSiegeSync::CheckCastleOwnerMember(CGameObject &Obj)
{
	if ( gObjIsConnected(Obj.m_Index) == FALSE )
	{
		return FALSE;
	}

	if ( strcmp(this->m_szCastleOwnerGuild, "") == 0 )
	{
		return FALSE;
	}

	if ( strcmp(Obj.m_PlayerData->GuildName, this->m_szCastleOwnerGuild) != 0 )
	{
		return FALSE;
	}

	return TRUE;
}



BOOL CCastleSiegeSync::CheckCastleOwnerUnionMember(CGameObject &Obj)
{
	if ( gObjIsConnected(Obj.m_Index) == FALSE )
	{
		return FALSE;
	}

	if ( strcmp(this->m_szCastleOwnerGuild, "") == 0 )
	{
		return FALSE;
	}

	GUILD_INFO_STRUCT * lpGuildInfo = Obj.m_PlayerData->lpGuild;
	
	if ( lpGuildInfo == NULL )
	{
		return FALSE;
	}
	
	TUnionInfo * pUnionInfo = UnionManager.SearchUnion(lpGuildInfo->iGuildUnion);

	if ( pUnionInfo == NULL )
	{
		return FALSE;
	}

	if ( strcmp( pUnionInfo->m_szMasterGuild, this->m_szCastleOwnerGuild) == 0 )
	{
		return TRUE;
	}

	return FALSE;
}



int CCastleSiegeSync::CheckOverlapCsMarks(CGameObject &Obj)
{
	for ( int x=INVENTORY_BAG_START;x<MAIN_INVENTORY_SIZE;x++)
	{
		if ( Obj.pntInventory[x]->IsItem() == TRUE )
		{
			if ( Obj.pntInventory[x]->m_Type == ITEMGET(14,21) )
			{
				if ( Obj.pntInventory[x]->m_Level == 3 )
				{
					int op = Obj.pntInventory[x]->m_Durability;

					if (op >= 0 && op < IsOverlapItem(Obj.pntInventory[x]->m_Type))
					{
						return x;
					}
				}
			}
		}
	}

	return -1;
}

CCastleSiegeSync g_CastleSiegeSync;

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

