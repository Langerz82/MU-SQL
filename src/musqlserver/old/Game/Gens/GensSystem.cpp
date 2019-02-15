////////////////////////////////////////////////////////////////////////////////
// GensSystem.cpp

#include "GameMain.h"
#include "GuildClass.h"
#include "GameProtocol.h"
#include "IOCP.h"
#include "User/CUserData.h"
#include "BuffEffectSlot.h"
#include "configread.h"
#include "PentagramSystem.h"
#include "PeriodItemEx.h"
#include "MoveCommand.h"
#include "util.h"
#include "GensSystem.h"
#include "GensSystemProtocol.h"
#include "BuffEffect.h"
#include "MasterLevelSkillTreeSystem.h"
#include "CustomMaxStats.h"

char *szInfluence[] = { "NONE_INFLUENCE", "Duprian", "Vanert" };

GensSystem g_GensSystem;

GensSystem::GensSystem() 
{
	InitializeCriticalSection(&criti);
	this->m_iDuprianMemberCount = 0;
	this->m_iVanertMemberCount = 0;
	this->m_bFileLoad = FALSE;

	this->m_mapBattleZoneData.clear();
	memset(this->m_wBattleZoneWarp, -1, sizeof(this->m_wBattleZoneWarp));
};

GensSystem::~GensSystem() 
{
	DeleteCriticalSection(&criti);
};

//-> Completed
int GensSystem::GDReqAbusingInfo(CGameObject &Obj)  // done
{
	if (!ObjectMaxRange(Obj.m_Index))
		return 0;

	if (gObjIsConnected(lpObj))
	{
		PMSG_REQ_ABUSING_INFO pMsg;		
		
		PHeadSubSetB((BYTE*)&pMsg, 0xF8, 0x09, sizeof(pMsg));		
		std::memcpy(pMsg.Name, Obj.Name, 10);
		pMsg.Name[10] = 0;
		pMsg.bIndexH = SET_NUMBERH(Obj.m_Index);
		pMsg.bIndexL = SET_NUMBERL(Obj.m_Index);

		if (wsExDbCli.DataSend((char*)&pMsg, pMsg.h.size))
			return true;
	}
	return false;
}; 

//-> Completed
int GensSystem::ReqExDBGensInfo(CGameObject &Obj) // done
{
	if (!ObjectMaxRange(Obj.m_Index)) return 0;

	if (gObjIsConnected(lpObj))
	{
		PMSG_REQ_ABUSING_INFO pMsg;			

		PHeadSubSetB((BYTE*)&pMsg, 0xF8, 0x01, sizeof(pMsg));	
		std::memcpy(pMsg.Name, Obj.Name, 10);
		pMsg.Name[10] = 0;
		pMsg.bIndexH = SET_NUMBERH(Obj.m_Index);
		pMsg.bIndexL = SET_NUMBERL(Obj.m_Index);

		if (wsExDbCli.DataSend((char*)&pMsg, pMsg.h.size)) return true;
	}
	return false;
};

//-> Completed
void GensSystem::ReqRegGensMember(CGameObject &Obj, BYTE btInfluence) // done
{
	if (gObjIsConnected(Obj.m_Index))
	{
		if ( btInfluence != VANERT_INFLUENCE && btInfluence != DUPRIAN_INFLUENCE )
		{
			sLog->outBasic("[GensSystem] - Error - %s %s Line: %d", "btInfluence", __FILE__, __LINE__);   
			return;
		}

		PMSG_REQ_REG_GENS_MEMBER_EXDB pMsg;	
		memset(&pMsg, 0x00, sizeof(pMsg));

		pMsg.AccountID[10] = 0;
		pMsg.Name[10] = 0;

		std::memcpy(&pMsg.AccountID[0], Obj.AccountID, 10);
		std::memcpy(&pMsg.Name[0], Obj.Name, 10);

		if (Obj.m_PlayerData->lpGuild)
		{
			pMsg.wNumberH = SET_NUMBERHW(Obj.m_PlayerData->lpGuild->Number);
			pMsg.wNumberL = SET_NUMBERLW(Obj.m_PlayerData->lpGuild->Number);
		}

		pMsg.bInfluence = btInfluence;
		pMsg.bIndexH = SET_NUMBERH(Obj.m_Index);
		pMsg.bIndexL = SET_NUMBERL(Obj.m_Index);
	
		PHeadSubSetB((BYTE*)&pMsg, 0xF8, 0x03, sizeof(pMsg));

		wsExDbCli.DataSend((char*)&pMsg, pMsg.h.size);
	}
};

//-> Completed
int GensSystem::AnsRegGensMember(int iObjIndex, BYTE btResult)
{
	return 0;
};
 
//-> Completed
bool GensSystem::IsInfluenceNPC(CGameObject &Obj)
{
	if(Obj.Type != OBJ_USER)
	{
		return false;
	}

	if (Obj.TargetNpcNumber == -1)
	{
		return false;
	}

	int iNpcClass = getGameObject(Obj.TargetNpcNumber)->Class;

	return (iNpcClass == 544 && this->GetGensInfluence(lpObj) == VANERT_INFLUENCE 
		    || iNpcClass == 543 && this->GetGensInfluence(lpObj) == DUPRIAN_INFLUENCE); 
};

//-> Completed
int GensSystem::ReqSecedeGensMember(CGameObject &Obj) // done
{
	GUILD_INFO_STRUCT *lpGuildInfo = Obj.m_PlayerData->lpGuild;

	if (gObjIsConnected(Obj.m_Index))
	{
		PMSG_REQ_SECEDE_GENS_MEMBER_EXDB pMsg;

		PHeadSubSetBE((BYTE*)&pMsg, 0xF8, 0x05, sizeof(pMsg)); 

		pMsg.bIndexH = SET_NUMBERH(Obj.m_Index);
		pMsg.bIndexL = SET_NUMBERL(Obj.m_Index);

		if (Obj.m_PlayerData->lpGuild)
		{
			pMsg.wNumberH = SET_NUMBERHW(Obj.m_PlayerData->lpGuild->Number);
			pMsg.wNumberL = SET_NUMBERLW(Obj.m_PlayerData->lpGuild->Number);
		}

		std::memcpy(pMsg.Name, Obj.Name, 10);
		pMsg.Name[10] = 0;

		if (wsExDbCli.DataSend((char*)&pMsg, sizeof(pMsg))) return true;
	}

	return false;
};

//-> Completed
int GensSystem::SendGensInfo(CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
	 return 0;

	if (gObjIsConnected(lpObj))
	{
		PMSG_SEND_GENS_INFO pMsg;

		PHeadSubSetB((BYTE*)&pMsg, 0xF8, 0x7, sizeof(pMsg));
		
		pMsg.bInfluence = this->GetGensInfluence(lpObj);
		pMsg.iGensRanking = Obj.m_PlayerData->m_GensRanking;
		pMsg.iGensClass = Obj.m_PlayerData->m_GensClass;
		pMsg.iContributePoint = Obj.m_PlayerData->m_ContributePoint;
		pMsg.iNextContributePoint = this->GetNextContributePoint(lpObj);

		if (IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size))
			return true;
	}
	return false;
};

//-> Completed
int GensSystem::AnsSecedeGensMember(int iObjIndex, BYTE btResult)
{
	//int iRet;
	return 0;
};

//-> Completed
int GensSystem::SendPkPenaltyMapMove(CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
 	return 0;

	if (gObjIsConnected(lpObj))
	{
		BYTE pMsg[4] = { 0xC1, 0x04, 0xF8, 0x08 };
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg[1]);
		return true;
	}
	return false;
};

//-> Completed
int GensSystem::GDReqSaveContributePoint(CGameObject &Obj)  // done
{
	if (!ObjectMaxRange(Obj.m_Index))
		return false;

	if (gObjIsConnected(lpObj))
	{
		if (this->IsRegGensInfluence(lpObj))
		{
			PMSG_REQ_SAVE_CONTRIBUTE_POINT_EXDB pMsg;

			PHeadSubSetB((BYTE*)&pMsg, 0xF8, 0x07, sizeof(pMsg));
			pMsg.Name[10] = 0;
			std::memcpy(pMsg.Name, Obj.Name, 10);
			pMsg.iContributePoint = this->GetContributePoint(lpObj);
			pMsg.iGensClass = this->GetGensClass(lpObj);

			if (wsExDbCli.DataSend((char*)&pMsg, pMsg.h.size))
				return true;
		}
	}
	return false;
};

//-> Completed
int GensSystem::DBSaveAbusingKillUserName(CGameObject &Obj) // done
{
	if (!ObjectMaxRange(Obj.m_Index))
		return false;

	if(Obj.Type != OBJ_USER)
	{
		return false;
	}

	if (gObjIsConnected(Obj.m_Index))
	{
		if (this->IsRegGensInfluence(lpObj))
		{
			EnterCriticalSection(&criti);

			PMSG_REQ_SAVE_ABUSING_KILLUSER_EXDB pMsg;

			pMsg.AccountID[10] = 0;
			pMsg.Name[10] = 0;

			for (int i = 0; i < 10; ++i)
			{
				pMsg.KillName[i * 11 + MAX_ACCOUNT_LEN] = 0;

				pMsg.KillCount[i] = Obj.m_PlayerData->m_KillUserCnt[i];

				std::memcpy(&pMsg.KillName[i * 11], &Obj.m_PlayerData->KillUserName[i * 11], 10); 
			}

			std::memcpy(pMsg.AccountID, Obj.AccountID, 0x0A);
			
			std::memcpy(pMsg.Name, Obj.Name, 0x0A);

			PHeadSubSetB((BYTE*)&pMsg, 0xF8, 0x08, sizeof(pMsg));

			LeaveCriticalSection(&criti);
			
			if (wsExDbCli.DataSend((char*)&pMsg, pMsg.h.size))
				return true;
		}
	}
	return false;
};

//-> Completed
void GensSystem::BattleZoneChatMsgSend(CGameObject &Obj, BYTE* Msg, int size)
{
	int iTarInfluence = 0;

	if (this->IsMapBattleZone(Obj.MapNumber))
	{
		iTarInfluence = this->GetGensInfluence(lpObj);

		for (int n = 0; n < MAX_VIEWPORT; ++n)
		{
			if (Obj.VpPlayer[n].type == OBJ_USER && Obj.VpPlayer[n].state != 0 && (this->GetGensInfluence(&getGameObject(Obj.VpPlayer[n]->number]) == iTarInfluence || CheckAuthorityCondition(98, lpObj) || CheckAuthorityCondition(98, &getGameObject(Obj.VpPlayer[n)->number)) || this->CanGensSeeOppositeChat() == TRUE))
			{				
				IOCP.DataSend(Obj.VpPlayer[n].number, Msg, size);
			}
		}
	}
};

BOOL GensSystem::IsMoveMapBattleZone(int iMapIndex)
{
	for (int i = 0; i < MAX_MOVE_COMMAND; i++)
	{
		if (this->m_wBattleZoneWarp[i] == iMapIndex)
		{
			return TRUE;
		}
	}

	return FALSE;
};

void GensSystem::GensViewportListProtocol(CGameObject &Obj)
{
	if(Obj.Type != OBJ_USER)
	{
		return;
	}

	PMSG_GENS_MEMBER_VIEWPORT_INFO pGensMsg;
	PMSG_SEND_GENS_MEMBER_VIEWPORT pGensCount;

	BYTE GensBuf[2048];
	int GensCount = 0;
	
	int GensInfoOfs = sizeof(PMSG_SEND_GENS_MEMBER_VIEWPORT);

	memset(GensBuf, 0, sizeof(GensBuf));

	if ((Obj.Authority & 0x20) != 0x20 || !gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_INVISABLE))
	{
		pGensMsg.btInfluence = this->GetGensInfluence(lpObj);
		pGensMsg.NumberH = HIBYTE(Obj.m_Index);
		pGensMsg.NumberL = LOBYTE(Obj.m_Index);
		pGensMsg.iGensRanking = Obj.m_PlayerData->m_GensRanking;
		pGensMsg.iGensClass = this->GetGensClass(lpObj);
		pGensMsg.iContributePoint = this->GetContributePoint(lpObj); 
		std::memcpy(&GensBuf[GensInfoOfs], &pGensMsg, sizeof(PMSG_GENS_MEMBER_VIEWPORT_INFO));
	
		GensInfoOfs += sizeof(PMSG_GENS_MEMBER_VIEWPORT_INFO);

		PHeadSubSetW((BYTE*)&pGensCount, 0xF8, 0x05, GensInfoOfs); 	
		pGensCount.Count = GensCount;
		std::memcpy(&GensBuf, &pGensCount, sizeof(PMSG_SEND_GENS_MEMBER_VIEWPORT));

		for (int n = 0; n < MAX_VIEWPORT; n++)
		{
			if (Obj.VpPlayer2[n].type == OBJ_USER && Obj.VpPlayer2[n].state != 0)
			{
				int tObjNum = Obj.VpPlayer[n].number;

				if (tObjNum > 0)
				{
					IOCP.DataSend(tObjNum, GensBuf, GensInfoOfs);
				}				
			}
		}
	}
}

int GensSystem::IsMapBattleZone(int iMapIndex)
{
	if (g_MaxStatsInfo.GetClass.IsNonPvP[gObj.MapNumber] == TRUE)
	{
		return FALSE;
	}

	if (iMapIndex < 0 || iMapIndex > MAX_NUMBER_MAP-1)
	{
		return FALSE;
	}

	std::map<int, BATTLEZONE_MAP_DATA>::iterator It = this->m_mapBattleZoneData.find(iMapIndex);

	if (It == this->m_mapBattleZoneData.end())
	{
		return FALSE;
	}

	return TRUE;
};

void GensSystem::SetGensInfluence(CGameObject &Obj, int iInfluence)
{
	if(Obj.Type != OBJ_USER)
	{
		return;
	}

	if(Obj.m_PlayerData->ISBOT == true)
	{
		return;
	}

	if (iInfluence != VANERT_INFLUENCE && iInfluence != DUPRIAN_INFLUENCE && iInfluence != LEAVED_INFLUENCE && iInfluence != NONE_INFLUENCE)
	{
		sLog->outBasic("[GensSystem] - Error - [%s][%s] Influence[%d] %s %d",	
			Obj.AccountID, Obj.Name, iInfluence, __FILE__, __LINE__);
	}
	else
	{
		if (iInfluence == LEAVED_INFLUENCE) iInfluence = NONE_INFLUENCE;

		Obj.m_PlayerData->m_GensInfluence = iInfluence;
	}
};

//-> Completed
int GensSystem::IsPkEnable(CGameObject &Obj, CGameObject lpTargetObj)
{
	if (this->IsMapBattleZone(Obj.MapNumber))
	{
	 if (Obj.Type != 1 || lpTargetObj.Type != 2)
	 {
			return this->GetGensInfluence(lpObj) != this->GetGensInfluence(lpTargetObj);
		}
		else
		{
			return 1;
		}
	}
	return 1;
};

//-> Completed
int GensSystem::GetGensInfluence(CGameObject &Obj)
{
	if(Obj.Type != OBJ_USER)
	{
		return NONE_INFLUENCE;
	}

	if(Obj.m_PlayerData->ISBOT == true)
	{
		return NONE_INFLUENCE;
	}

	if (Obj.m_PlayerData->m_GensInfluence == LEAVED_INFLUENCE || Obj.m_PlayerData->m_GensInfluence < NONE_INFLUENCE || Obj.m_PlayerData->m_GensInfluence > VANERT_INFLUENCE) 
	{
		sLog->outBasic("[GensSystem] Error [%s][%s] GensInfluence [%d]", Obj.AccountID, Obj.Name, Obj.m_PlayerData->m_GensInfluence); 
		return 0;
	}

	return Obj.m_PlayerData->m_GensInfluence;

};

//-> Completed
char *GensSystem::GetGensInfluenceName(CGameObject &Obj)
{
	if(Obj.Type != OBJ_USER)
	{
		return NULL;
	}	
	if (Obj.m_PlayerData->m_GensInfluence == LEAVED_INFLUENCE || Obj.m_PlayerData->m_GensInfluence < NONE_INFLUENCE || Obj.m_PlayerData->m_GensInfluence > VANERT_INFLUENCE) 
	{
		sLog->outBasic("[GensSystem] Error [%s][%s] GensInfluence [%d]", Obj.AccountID, Obj.Name, Obj.m_PlayerData->m_GensInfluence);
		return NULL;
	}
	return szInfluence[Obj.m_PlayerData->m_GensInfluence];
};

//-> Completed
bool GensSystem::IsRegGensInfluence(CGameObject &Obj)
{
	if(Obj.Type != OBJ_USER)
	{
		return false;
	}

	if (this->GetGensInfluence(lpObj) == VANERT_INFLUENCE || this->GetGensInfluence(lpObj) == DUPRIAN_INFLUENCE)
	{
		return true;
	}
	return false;
};

//-> Completed
int GensSystem::SetUserBattleZoneEnable(CGameObject &Obj, int bBattleZoneEnable)
{
	if(Obj.Type != OBJ_USER)
	{
		return 0;
	}	
	
	return Obj.m_PlayerData->m_UserBattleZoneEnable = bBattleZoneEnable;
};

//-> Completed
int GensSystem::IsUserBattleZoneEnable(CGameObject &Obj)
{
	if(Obj.Type != OBJ_USER)
	{
		return 0;
	}	
	
	return Obj.m_PlayerData->m_UserBattleZoneEnable;
};

//-> Completed
int GensSystem::SetContributePoint(CGameObject &Obj, int iContributePoint)
{
	if(Obj.Type != OBJ_USER)
	{
		return 0;
	}

	return Obj.m_PlayerData->m_ContributePoint = iContributePoint;
};

//-> Completed
int GensSystem::AddContributePoint(CGameObject &Obj, int iContributePoint)
{
	if(Obj.Type != OBJ_USER)
	{
		return 0;
	}	
	
	Obj.m_PlayerData->m_ContributePoint += iContributePoint;

	return this->GDReqSaveContributePoint(lpObj);
};

//-> Completed
int GensSystem::SubContributePoint(CGameObject &Obj, int iContributePoint)
{
	if(Obj.Type != OBJ_USER)
	{
		return 0;
	}

	Obj.m_PlayerData->m_ContributePoint -= iContributePoint;
	
	if (Obj.m_PlayerData->m_ContributePoint < 0)
	{
		Obj.m_PlayerData->m_ContributePoint = 0;
	}

	return this->GDReqSaveContributePoint(lpObj);
};

//-> Completed
int GensSystem::GetContributePoint(CGameObject &Obj)
{
	if(Obj.Type != OBJ_USER)
	{
		return 0;
	}

	return Obj.m_PlayerData->m_ContributePoint;
};

int GensSystem::PkPenaltyAddNeedZenMapMove(CGameObject &Obj)
{
	return g_ConfigRead.pk.GetWarpCostMultiplier(Obj.m_PK_Level);
};

int GensSystem::GetPKPartyLevel(CGameObject &Obj)
{
	if (Obj.PartyNumber == -1)
	{
		return Obj.m_PK_Level;
	}

	else
	{
		return gParty.GetPKPartyPenalty(Obj.PartyNumber);
	}
}

void GensSystem::PkPenaltyDropInvenItem(CGameObject &Obj)
{
	if (this->IsGensMuBlueMode() == false)
	{
		return;
	}

	if (this->IsMapBattleZone(Obj.MapNumber) == false)
	{
		if (this->GetPKPartyLevel(lpObj) > 3)
		{
			for (int x = INVETORY_WEAR_SIZE; x < MAIN_INVENTORY_SIZE; x++)
			{
				if (Obj.pntInventory[x]->IsItem() == TRUE &&
					g_PentagramSystem.IsPentagramItem(Obj.pntInventory[x]->m_Type) == FALSE &&
					g_PentagramSystem.IsPentagramJewel(Obj.pntInventory[x]->m_Type) == FALSE)
				{
					PMSG_ITEMTHROW pDropItem;

					pDropItem.Ipos = x;
					pDropItem.px = Obj.X;
					pDropItem.py = Obj.Y;

					if (gGameProtocol.CGItemDropRequest(&pDropItem, Obj.m_Index, 1) == true)
					{
					}

					break;
				}
			}
		}
	}
};

void GensSystem::PkPenaltyDropZen(CGameObject &Obj)
{
	if (this->IsGensMuBlueMode() == false)
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (this->IsMapBattleZone(Obj.MapNumber) == true)
	{
		return;
	}

	if (this->GetPKPartyLevel(lpObj) <= 4)
	{
		return;
	}

	if (Obj.m_PlayerData->Money <= 0)
	{
		return;
	}

	if ((rand()%30) == 0)
	{
		int iDropMoney = 3 * Obj.m_PlayerData->Money / 100;

		MapC[Obj.MapNumber].MoneyItemDrop(iDropMoney, Obj.X, Obj.Y);

		Obj.m_PlayerData->Money -= iDropMoney;

		if (Obj.m_PlayerData->Money < 0)
		{
			Obj.m_PlayerData->Money = 0;
		}

	}
};

void GensSystem::SendPKPenaltyDebuff(CGameObject &Obj)
{
	if (this->IsGensMuBlueMode() == false)
	{
		return;
	}

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (this->IsMapBattleZone(Obj.MapNumber) == true)
	{
		return;
	}

	if (Obj.m_PK_Level <= 4)
	{
		return;
	}

	PeriodBuffInfo BuffInfo;
	LPPERIOD_BUFF_EFFECT_INFO lpPeriBuff = g_BuffEffectSlot.GetPeriodBuffInfo(106);

	if (!lpPeriBuff)
		return;

	if (gObjAddPeriodBuffEffect(lpObj, lpPeriBuff, lpPeriBuff->dwDuration) == TRUE)
	{
		BuffInfo.wBuffIndex = 106;
		BuffInfo.btEffectType1 = 0;
		BuffInfo.btEffectType2 = 0;
		BuffInfo.lDuration = lpPeriBuff->dwDuration;

		g_BuffEffect.RequestPeriodBuffInsert(lpObj, &BuffInfo);
	}
}

void GensSystem::CalcContributePoint(CGameObject &Obj, CGameObject lpTargetObj)
{
	if(Obj.Type != OBJ_USER)
	{
		return;
	}

	int iInfluence;
	int iTarInfluence;
	int iContributePoint;
	int iKillerContributePoint;
	int iCalcLv;
	int iAddValue;
	int iSubValue;
	int iAddBonusValue;
	int iGensClassGap;
	
	char szName[MAX_ACCOUNT_LEN+1];

	if (Obj.Type && lpTargetObj.Type && this->IsMapBattleZone(Obj.MapNumber) && this->IsMapBattleZone(lpTargetObj.MapNumber))
	{
		iInfluence = this->GetGensInfluence(lpObj);
		iTarInfluence = this->GetGensInfluence(lpTargetObj);

		if (iInfluence && iTarInfluence)
		{
			if (iInfluence != iTarInfluence)
			{
				std::memcpy(szName, lpTargetObj.Name, sizeof(lpTargetObj.Name));		

				if (this->ChkKillUserName(lpObj, szName) == 0)
				{
					iKillerContributePoint = this->GetContributePoint(lpTargetObj); 
					
					iContributePoint = this->GetContributePoint(lpObj);

					if (iKillerContributePoint > 0 || iContributePoint <= 0)
					{
						iAddValue = 0; iSubValue = 0;

						if (Obj.Level == lpTargetObj.Level)
						{
							iAddValue = 5; iSubValue = 3;
						}
						else
						{							
							if (Obj.Level >= lpTargetObj.Level)
							{					
								if (Obj.Level > lpTargetObj.Level)
								{
									iCalcLv = Obj.Level - lpTargetObj.Level;

									if (iCalcLv < 51)
							 		{
									 if (iCalcLv < 31 || iCalcLv > 50)
								 	{
								 		if (iCalcLv < 11 || iCalcLv > 30)
									 	{
									 		if (iCalcLv > 0 && iCalcLv <= 10)
											 {
										 		iAddValue = 5; iSubValue = 3;
										 	}
									 	}
									 	else
										 {
										 	iAddValue = 3; iSubValue = 3;
										 }
								 	}
								 	else 
								 	{
										 iAddValue = 2; iSubValue = 1;
									 }
								 }
							 	else
							 	{
							 		iAddValue = 1; iSubValue = 1;
							 	}
							 }
						 }
							else
							{
								iCalcLv = lpTargetObj.Level - Obj.Level;

								if (iCalcLv < 51)
								{
									if (iCalcLv < 31 || iCalcLv > 50)
									{
										if (iCalcLv < 11 || iCalcLv > 30)
										{
											if (iCalcLv > 0 && iCalcLv <= 10)
											{
												iAddValue = 5; iSubValue = 3;
											}
										}
										else
										{
											iAddValue = 6; iSubValue = 3;
										}
									}
									else
									{
										iAddValue = 6; iSubValue = 3;
									}
								}
								else
								{
									iAddValue = 7; iSubValue = 3;
								}
							}
						}

						iAddBonusValue = 0;

						iGensClassGap = Obj.m_PlayerData->m_GensClass - lpTargetObj.m_PlayerData->m_GensClass;
					
						if (iGensClassGap == 1)
						{
							if (Obj.m_PlayerData->m_GensClass >= 9)
							{
								iAddBonusValue = 1;
							}
							else
							{
								iAddBonusValue = 3;
							}
						}
						else
						{
							if (iGensClassGap == 2)
							{
								if (Obj.m_PlayerData->m_GensClass >= 9)
								{
									iAddBonusValue = 2;
								}
								else
								{
									iAddBonusValue = 4;
								}
							}
							else
							{
								if (iGensClassGap >= 3)
								{
									if (Obj.m_PlayerData->m_GensClass >= 9)
									{
										iAddBonusValue = 3;
									}
									else
									{
										iAddBonusValue = 5;
									}
								}
							}
					
							if (Obj.m_PlayerData->m_GensClass > 9 && iAddBonusValue + iAddValue >= this->GetNextContributePoint(lpObj))
							{
								gGameProtocol.GCServerMsgStringSend(Lang.GetText(0,369), Obj.m_Index, 1);
							}

							this->AddContributePoint(lpObj, iAddValue);				
								
							this->SubContributePoint(lpTargetObj, iSubValue);
						
							if (iAddBonusValue > 0)
							{
								this->AddContributePoint(lpObj, iAddBonusValue);

								gGameProtocol.GCServerMsgStringSend(Lang.GetText(0,370), Obj.m_Index, 1);

							}
						}
				 }
		 	}
	 	}

		else
		{
			sLog->outBasic("[GensSystem] - Error NONE_INFLUENCE - %s Line: %d", __FILE__, __LINE__);
		}
	 }
}
};

int GensSystem::ChkKillUserName(CGameObject &Obj, char *szCharName)
{
	if(Obj.Type != OBJ_USER)
	{
		return false;
	}

	int i = 0;
	if(Obj.Type != OBJ_USER)
	{
		return 0;
	}
	while (true)
	{
		if (i >= 10)
		{
			EnterCriticalSection((LPCRITICAL_SECTION)&criti);
			this->InsertKillUserName(lpObj, szCharName);
			LeaveCriticalSection((LPCRITICAL_SECTION)&criti);
			return 0;
		}

		if (!strcmp(&Obj.m_PlayerData->KillUserName[i*11], szCharName))
	 	break;
	
		i++;
	}
	
	EnterCriticalSection((LPCRITICAL_SECTION)&criti);
	++Obj.m_PlayerData->m_KillUserCnt[i];
	LeaveCriticalSection((LPCRITICAL_SECTION)&criti);

	if (this->AbusingPenalty(lpObj, i))
		return true;

	return false;
};

//-> Completed
int GensSystem::AbusingPenalty(CGameObject &Obj, int iKillUserIndex)
{
	if(Obj.Type != OBJ_USER)
	{
		return 0;
	}	
	
	int iKillUserCnt = Obj.m_PlayerData->m_KillUserCnt[iKillUserIndex];

	bool bDBSave = false;

	if (Obj.m_PlayerData->m_ContributePoint > 0)
	{
		bDBSave = true;
	}

	if (iKillUserCnt >= this->m_iGensAbusingPenaltyWarning)
	{
		if (iKillUserCnt < this->m_iGensAbusingPenaltyLimit)
		{
			char szMsg[128];
			wsprintf(szMsg, Lang.GetText(0,372), m_iGensAbusingPenaltyWarning, m_iGensAbusingPenaltyLimit);
			gGameProtocol.GCServerMsgStringSend(szMsg, Obj.m_Index, 1);
			return false;
		}

		else if (iKillUserCnt >= this->m_iGensAbusingPenaltyLimit)
		{
			gGameProtocol.GCServerMsgStringSend(Lang.GetText(0,371), Obj.m_Index, 1);

			EnterCriticalSection((LPCRITICAL_SECTION)&criti);
		
			if (Obj.m_PlayerData->m_ContributePoint < 0)
			{
				Obj.m_PlayerData->m_ContributePoint = 0;
			}
		
			LeaveCriticalSection((LPCRITICAL_SECTION)&criti);

			if (bDBSave)
			{
				this->GDReqSaveContributePoint(lpObj);
			}

			return true;
		}
	}

	return false;
};

//-> Completed
int GensSystem::InsertKillUserName(CGameObject &Obj, char *szCharName)
{
	if(Obj.Type != OBJ_USER)
	{
		return 0;
	}	
	
	int iKillUserToCnt = Obj.m_PlayerData->m_KillUserTotCnt;

	if (iKillUserToCnt >= 10)
	{
		Obj.m_PlayerData->m_KillUserTotCnt = 0; 
		iKillUserToCnt = 0; 
	}

	memset(&Obj.m_PlayerData->KillUserName[iKillUserToCnt*11], 0, 11);
	strcpy(&Obj.m_PlayerData->KillUserName[iKillUserToCnt*11], szCharName);

	++Obj.m_PlayerData->m_KillUserCnt[iKillUserToCnt];
	++Obj.m_PlayerData->m_KillUserTotCnt;

	for (int i = 0; i < Obj.m_PlayerData->m_KillUserTotCnt; ++i)
	{
		sLog->outBasic("[GensSystem] Abusing List [%s][%s] KillUserName[%s] Cnt[%d]", Obj.AccountID, Obj.Name, &Obj.m_PlayerData->KillUserName[iKillUserToCnt*11], Obj.m_PlayerData->m_KillUserCnt[iKillUserToCnt]);
		iKillUserToCnt = i + 1;
	}

	return iKillUserToCnt;
};

//-> Completed
void GensSystem::MoveInBattleZonePartySplit(CGameObject &Obj)
{
	PMSG_PARTYDELUSER pMsg;

	if (this->CanGensCreatePartyOnBattleZone() == TRUE)
	{
		return;
	}

	if (Obj.m_PlayerData->m_bUsePartyMatching == false && Obj.PartyNumber >= 0)
	{
		int pindex = gParty.GetIndex(Obj.PartyNumber, Obj.m_Index, Obj.DBNumber);

		if (pindex >= 0)
		{
			pMsg.Number = pindex;
			gGameProtocol.CGPartyDelUser(&pMsg, Obj.m_Index, 0); 
			gGameProtocol.GCResultSend(Obj.m_Index, 0x41, 0x7);
		}
	}
};

//-> Completed
void GensSystem::AbusingInfoReset(CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
		return;

	if(Obj.Type != OBJ_USER)
	{
		return;
	}

	if (gObjIsConnected(Obj.m_Index))
	{
		if (this->IsRegGensInfluence(lpObj))
		{
			if ( this->GetAbusingResetTime() > 0 && GetTickCount64() - Obj.m_PlayerData->m_GensAbusingTick >= 60000 * this->GetAbusingResetTime())
			{
				EnterCriticalSection((LPCRITICAL_SECTION)&criti);

				Obj.m_PlayerData->m_GensAbusingTick = GetTickCount64();

				for (int i = 0; i < 10; ++i)
				{
					Obj.m_PlayerData->m_KillUserCnt[i] = 0;
					memset(&Obj.m_PlayerData->KillUserName[i*11], 0, 11);
				}

				Obj.m_PlayerData->m_KillUserTotCnt = 0;

				LeaveCriticalSection((LPCRITICAL_SECTION)&criti);
			}
		}
		else
		{
			Obj.m_PlayerData->m_GensAbusingTick = GetTickCount64();
		}
	}
};

//-> Completed
int GensSystem::CalGensClass(CGameObject &Obj)
{
	if(Obj.Type != OBJ_USER)
	{
		return 0;
	}

	if (Obj.m_PlayerData->m_ContributePoint < this->m_GensRanking[8].ContributePoint)
	{
		if (Obj.m_PlayerData->m_ContributePoint < this->m_GensRanking[9].ContributePoint)
		{
			if (Obj.m_PlayerData->m_ContributePoint < this->m_GensRanking[10].ContributePoint)
			{
				if (Obj.m_PlayerData->m_ContributePoint < this->m_GensRanking[11].ContributePoint)
				{
					if (Obj.m_PlayerData->m_ContributePoint < this->m_GensRanking[12].ContributePoint)
					{
						if (Obj.m_PlayerData->m_ContributePoint < this->m_GensRanking[13].ContributePoint)
						{
							return 0;
						}
						else 
						{
							return 14;
						}
					}
					else
					{
						return 13;
					}
				}
				else 
				{
					return 12;
				}
			}
			else
			{
				return 11;
			}
		}
		else
		{
			return 10;
		}
	}
	else
	{
		if (Obj.m_PlayerData->m_GensRanking > this->m_GensRanking[0].RankingPosition)
		{
			if (Obj.m_PlayerData->m_GensRanking > this->m_GensRanking[1].RankingPosition)
			{
				if (Obj.m_PlayerData->m_GensRanking > this->m_GensRanking[2].RankingPosition)
				{
					if (Obj.m_PlayerData->m_GensRanking > this->m_GensRanking[3].RankingPosition)
					{
						if (Obj.m_PlayerData->m_GensRanking > this->m_GensRanking[4].RankingPosition)
						{
							if (Obj.m_PlayerData->m_GensRanking > this->m_GensRanking[5].RankingPosition)
							{
								if (Obj.m_PlayerData->m_GensRanking > this->m_GensRanking[6].RankingPosition)
								{
									if (Obj.m_PlayerData->m_GensRanking > this->m_GensRanking[7].RankingPosition)
									{
										return 9;
									}
									else
									{
										return 8;
									}
								}
								else
								{
									return 7;
								}
							}
							else 
							{
								return 6;
							}
						}
						else 
						{
							return 5;
						}
					}
					else
					{
						return 4;
					}
				}
				else
				{
					return 3;
				}
			}
			else 
			{
				return 2;
			}
		}
		else
		{
			return 1;
		}
	}
	return 0;
};

//-> Completed
int GensSystem::SetGensRanking(CGameObject &Obj, int iGensRanking)
{
	if(Obj.Type != OBJ_USER)
	{
		return 0;
	}

	return Obj.m_PlayerData->m_GensRanking = iGensRanking;
};

//-> Completed
int GensSystem::SetGensClass(CGameObject &Obj, int iGensClass)
{
	if(Obj.Type != OBJ_USER)
	{
		return 0;
	}

	return Obj.m_PlayerData->m_GensClass = iGensClass;
};

//-> Completed
int GensSystem::GetGensClass(CGameObject &Obj)
{
	if(Obj.Type != OBJ_USER)
	{
		return 0;
	}

	return Obj.m_PlayerData->m_GensClass;
};

//-> Completed
int GensSystem::GetNextContributePoint(CGameObject &Obj)
{
	if(Obj.Type != OBJ_USER)
	{
		return -2;
	}

	if (this->IsRegGensInfluence(lpObj))
	{
		if (Obj.m_PlayerData->m_ContributePoint > 499)
		{
			if (Obj.m_PlayerData->m_ContributePoint > 1499)
			{
				if (Obj.m_PlayerData->m_ContributePoint > 2999)
				{
					if (Obj.m_PlayerData->m_ContributePoint > 5999)
					{
						if (Obj.m_PlayerData->m_ContributePoint > 9999)
						{
							return -2;
						}
						else
						{
							return 10000 - Obj.m_PlayerData->m_ContributePoint;
						}
					}
					else
					{
						return 6000 - Obj.m_PlayerData->m_ContributePoint;
					}
				}
				else
				{
					return 3000 - Obj.m_PlayerData->m_ContributePoint;
				}
			}
			else
			{
		 	return 1500 - Obj.m_PlayerData->m_ContributePoint;
			}
		}
		else
		{
			return 500 - Obj.m_PlayerData->m_ContributePoint;
		}
	}
	else
	{
		return -1;
	}

	return 0;
};

//-> Completed
int GensSystem::SetGensMemberCount(int iInfluence, int iMemberCount)
{
	if (iInfluence == VANERT_INFLUENCE)
	{
		this->m_iVanertMemberCount= iMemberCount;
	}
	else 
	{
		if (iInfluence == DUPRIAN_INFLUENCE)
		 this->m_iDuprianMemberCount = iMemberCount;
	}

	return NONE_INFLUENCE;
};

//-> Completed
int GensSystem::GetGensMemberCount(int iInfluence)
{
	if (iInfluence == VANERT_INFLUENCE)
	{
		return this->m_iVanertMemberCount;
	}

	else if (iInfluence == DUPRIAN_INFLUENCE)
	{
		return this->m_iDuprianMemberCount;
	}

	else
	{
		return -1;
	}
};

//-> Completed
int GensSystem::ReqExDBGensRewardCheck(CGameObject &Obj, int iInfluence) // done
{
	if (!ObjectMaxRange(Obj.m_Index))
		return false;

	if (gObjIsConnected(lpObj))
	{
		PMSG_REQ_GENS_REWARD_CHECK_EXDB pMsg;	

		PHeadSubSetB((BYTE*)&pMsg, 0xF8, 0x0C, sizeof(pMsg));
	
		pMsg.Name[10] = 0;
		std::memcpy(pMsg.Name, Obj.Name, MAX_ACCOUNT_LEN);
		pMsg.wIndexH = SET_NUMBERH(Obj.m_Index);
		pMsg.wIndexL = SET_NUMBERL(Obj.m_Index);
		pMsg.iInfluence = iInfluence;
			
		if (wsExDbCli.DataSend((char*)&pMsg, pMsg.h.size))
			return true;
	}
	return false;
};
 
//-> Completed
int GensSystem::ReqExDBGensRewardComplete(CGameObject &Obj) // done
{
	if (!ObjectMaxRange(Obj.m_Index))
		return false;

	if (gObjIsConnected(lpObj))
	{
		PMSG_REQ_GENS_REWARD_COMPLETE_EXDB pMsg;

		PHeadSubSetB((BYTE*)&pMsg, 0xF8, 0x0E, sizeof(pMsg));

		pMsg.Name[10] = 0;
		std::memcpy(pMsg.Name, Obj.Name, 10);
		pMsg.wIndexH = SET_NUMBERHW(Obj.m_Index);
		pMsg.wIndexL = SET_NUMBERLW(Obj.m_Index);

		if (wsExDbCli.DataSend((char*)&pMsg, pMsg.h.size))
			return true;
	}
	return false;
};

//-> Completed
int GensSystem::SendGensReward(CGameObject &Obj, BYTE btResult)
{
	if (!ObjectMaxRange(Obj.m_Index))
		return false;

	if (gObjIsConnected(lpObj))
	{
		PMSG_SEND_GENS_REWARD pMsg;

		PHeadSubSetB((BYTE*)&pMsg, 0xF8, 0x0A, sizeof(pMsg));

		pMsg.bItemType = btResult;

		if (IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size))
		 return true;
	}
	return false;
};

//-> Completed
int GensSystem::SendGensRewardItem(CGameObject &Obj, int iGensClass)
{
	int iArrayClass = iGensClass-1;

	if (iArrayClass >= 0)
	{	
		if (this->m_GensRanking[iArrayClass].RewardItemCount > 0)
		{
			int ItemCount = this->m_GensRanking[iArrayClass].RewardItemCount;
			int ItemNumber = ItemGetNumberMake(this->m_GensRanking[iArrayClass].RewardItemGroup, this->m_GensRanking[iArrayClass].RewardItemNumber);
		
			if (this->GensRewardInventoryCheck(lpObj, ItemCount))
			{
				for (int i = 0; i < ItemCount; ++i)
				{
					ItemCreate(Obj.m_Index, 228, 0, 0, ItemNumber, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
				}
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	return false;
};

//-> Completed
bool GensSystem::GensRewardInventoryCheck(CGameObject &Obj, int iItemCount)
{
	BYTE blank = NULL;
	
	int iCount = 0;

	for (int h = 0; h < 8; ++h)
	{
		for (int w = 0; w < 8; ++w)
		{
			if (*(BYTE*)(Obj.pInventoryMap + (h) * 8 + (w)) == 255)
			{
				if (gObjOnlyInventoryRectCheck(Obj.m_Index, w, h, 1, 1))
				{
					++iCount;
				}
			}
		}
	}
	return iItemCount <= iCount;
};

//-> Completed
int GensSystem::ReqExDBGensMemberCount(CGameObject &Obj) // done
{
	if (!ObjectMaxRange(Obj.m_Index))
		return false;

	if (gObjIsConnected(lpObj))
	{
		PMSG_REQ_GENS_MEMBER_COUNT pMsg;

		PHeadSubSetB((BYTE*)&pMsg, 0xF8, 0x0F, sizeof(pMsg));

		pMsg.Name[10] = 0;
		std::memcpy(pMsg.Name, Obj.Name, 10);

		pMsg.wIndexH = SET_NUMBERHW(Obj.m_Index);
		pMsg.wIndexL = SET_NUMBERLW(Obj.m_Index);

		if (wsExDbCli.DataSend((char*)&pMsg, pMsg.h.size))
			return true;
	}
	return false;
};

//-> Completed
int GensSystem::ReqExDBSetGensRewardDay(void) // done
{
	PMSG_SET_GENS_REWARD_DAY_EXDB pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xF8, 0x11, sizeof(pMsg));

	return wsExDbCli.DataSend((char*)&pMsg, pMsg.h.size);
};

//-> Completed
int GensSystem::ReqExDBSetGensRanking(void)
{
	GENS_RANKING pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xF8, 0x12, sizeof(pMsg));

	wsExDbCli.DataSend((char*)&pMsg, pMsg.h.size);

	return true;
};

//-> Completed
int GensSystem::LoadData(char *lpszFileName)
{
	this->m_bFileLoad = FALSE;
	this->m_mapBattleZoneData.clear();

	if (lpszFileName == NULL)
	{
		sLog->outError("[ GensRanking ] - File load error : File Name Error");
		return FALSE;
	}

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(lpszFileName);

	if (res.status != pugi::status_ok)
	{
		sLog->outError("[ GensRanking ] - Can't Load %s (%s)", lpszFileName, res.description());
		return FALSE;
	}

	pugi::xml_node mainXML = file.child("GensSystem");

	this->m_bGensMapFromMuBlue = mainXML.attribute("BlueMuMode").as_bool();
	this->m_iGensInitialContributePoint = mainXML.attribute("InitialContributePoint").as_int();
	this->m_iGensAbusingPenaltyWarning = mainXML.attribute("AbusingPenaltyWarning").as_int();
	this->m_iGensAbusingPenaltyLimit= mainXML.attribute("AbusingPenaltyLimit").as_int();
	this->m_iGensAbusingResetTime = mainXML.attribute("AbusingResetTime").as_int();
	this->m_iGensPkPenaltyAddNeedZenMapMove = mainXML.attribute("PkPenaltyAddNeedZenMapMove").as_int();
	this->m_iGensDamageReduction = mainXML.attribute("DamageReduction").as_int();

	pugi::xml_node general = mainXML.child("General");

	this->m_bGensCanSeeOppositeGensChat = general.attribute("CanSeeOppositeGensChat").as_bool();
	this->m_bGensCanJoinPartyWhileOppositeGens = general.attribute("CanJoinPartyWhileOppositeGens").as_bool();
	this->m_bGensCanJoinGuildWhileOppositeGens = general.attribute("CanJoinGuildWhileOppositeGens").as_bool();
	this->m_bGensCanJoinUnionWhileOppositeGens = general.attribute("CanJoinUnionWhileOppositeGens").as_bool();
	this->m_bGensCanCreatePartyOnBattleZone = general.attribute("CanCreatePartyOnBattleZone").as_bool();
	this->m_iGensAttackWarpDelay = general.attribute("GensAttackWarpDelay").as_int(0); // custom for joinmu

	pugi::xml_node ranking = mainXML.child("RankingSettings");

	for (pugi::xml_node gens = ranking.child("Gens"); gens; gens = gens.next_sibling())
	{
		int iClass = gens.attribute("Class").as_int();

		if (iClass < 1 || iClass > MAX_GENS_CLASS)
		{
			sLog->outError("Wrong Gens Class in %s file (%d)", lpszFileName, iClass);
			continue;
		}

		this->m_GensRanking[iClass - 1].GensClass = iClass;
		this->m_GensRanking[iClass - 1].ContributePoint = gens.attribute("ReqContributePoint").as_int();
		this->m_GensRanking[iClass - 1].RankingPosition = gens.attribute("RankingPosition").as_int();
		this->m_GensRanking[iClass - 1].RewardItemGroup = gens.attribute("RewardItemCat").as_int();
		this->m_GensRanking[iClass - 1].RewardItemNumber = gens.attribute("RewardItemIndex").as_int();
		this->m_GensRanking[iClass - 1].RewardItemCount = gens.attribute("RewardItemCount").as_int();
	}

	pugi::xml_node battlezone = mainXML.child("BattleZoneSettings");
	pugi::xml_node maplist = battlezone.child("MapList");

	for (pugi::xml_node map = maplist.child("Map"); map; map = map.next_sibling())
	{
		int iMapNumber = map.attribute("Number").as_int();

		if (iMapNumber < 0 || iMapNumber > MAX_NUMBER_MAP-1)
		{
			sLog->outError("Wrong Map Number in %s file (%d)", lpszFileName, iMapNumber);
			continue;
		}

		if (map.attribute("Enable").as_bool() == false)
		{
			continue;
		}

		if (this->m_mapBattleZoneData.find(iMapNumber) != this->m_mapBattleZoneData.end())
		{
			sLog->outError("BattleZone Map Number already exists (%s) (Map:%d)", lpszFileName, iMapNumber);
			continue;
		}

		BATTLEZONE_MAP_DATA m_BattleZone;

		m_BattleZone.m_wMapNumber = iMapNumber;
		m_BattleZone.m_btEntryAllowType = map.attribute("FamilyAccessType").as_int();

		pugi::xml_node duprian = map.child("Duprian");

		m_BattleZone.m_fExpBonus[0] = duprian.attribute("ExpBonus").as_float();
		m_BattleZone.m_fMLExpBonus[0] = duprian.attribute("MLExpBonus").as_float();
		m_BattleZone.m_iDropBonus[0] = duprian.attribute("ItemDropBonus").as_int();
		m_BattleZone.m_iMLDropBonus[0] = duprian.attribute("MLItemDropBonus").as_int();
		m_BattleZone.m_iExcDropBonus[0] = duprian.attribute("ExCItemObjectDropBonus").as_int();
		m_BattleZone.m_iMLExcDropBonus[0] = duprian.attribute("MLExCItemObjectDropBonus").as_int();

		pugi::xml_node vanert = map.child("Vanert");

		m_BattleZone.m_fExpBonus[1]		= vanert.attribute("ExpBonus").as_float();
		m_BattleZone.m_fMLExpBonus[1]		= vanert.attribute("MLExpBonus").as_float();
		m_BattleZone.m_iDropBonus[1]		= vanert.attribute("ItemDropBonus").as_int();
		m_BattleZone.m_iMLDropBonus[1]		= vanert.attribute("MLItemDropBonus").as_int();
		m_BattleZone.m_iExcDropBonus[1]	= vanert.attribute("ExCItemObjectDropBonus").as_int();
		m_BattleZone.m_iMLExcDropBonus[1]	= vanert.attribute("MLExCItemObjectDropBonus").as_int();

		this->m_mapBattleZoneData.insert(std::pair<int, BATTLEZONE_MAP_DATA>(iMapNumber, m_BattleZone));
	}

	pugi::xml_node warplist = battlezone.child("WarpList");
	int iWarpCount = 0;

	for (pugi::xml_node warp = warplist.child("Warp"); warp; warp = warp.next_sibling())
	{
		int iWarpNumber = warp.attribute("Index").as_int();

		if (iWarpNumber < 0 || iWarpNumber > MAX_MOVE_COMMAND-1)
		{
			sLog->outError("Wrong Warp Number in %s file (%d)", lpszFileName, iWarpNumber);
			continue;
		}

		if (warp.attribute("Enable").as_int() == 0)
		{
			continue;
		}

		this->m_wBattleZoneWarp[iWarpCount] = iWarpNumber;
		iWarpCount++;
	}

	this->m_bFileLoad = TRUE;

}

int GensSystem::ReqGensRewardDay(CGameObject &Obj)
{
	PMSG_REQ_GENS_REWARD_DAY pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xF8, 0x13, sizeof(pMsg));
	pMsg.bIndexH = SET_NUMBERH(Obj.m_Index);
	pMsg.bIndexL = SET_NUMBERL(Obj.m_Index);

	return wsExDbCli.DataSend((char*)&pMsg, pMsg.h.size);
};

// CUSTOM PACKET (BATTLE ZONE & WARP LIST)

struct PMSG_SEND_GENS_BATTLEZONE_DATA
{
	PWMSG_HEAD2 h;
	WORD iMapList[MAX_NUMBER_MAP];
	int iWarpList[MAX_MOVE_COMMAND];
};

void GensSystem::SendBattleZoneData(CGameObject &Obj)
{
	PMSG_SEND_GENS_BATTLEZONE_DATA pMsg;

	PHeadSubSetW((BYTE*)&pMsg, 0xFA, 0xF8, sizeof(pMsg));

	memset(pMsg.iMapList, 0, sizeof(pMsg.iMapList));
	memset(pMsg.iWarpList, -1, sizeof(pMsg.iWarpList));

	if (g_MaxStatsInfo.GetClass.IsNonPvP[Obj.MapNumber] == FALSE)
	{
		for (std::map<int, BATTLEZONE_MAP_DATA>::iterator It = this->m_mapBattleZoneData.begin(); It != this->m_mapBattleZoneData.end(); It++)
		{
			pMsg.iMapList[It->second.m_wMapNumber] = It->second.m_wMapNumber;
		}

		for (int i = 0; i < MAX_MOVE_COMMAND; i++)
		{
			pMsg.iWarpList[i] = this->m_wBattleZoneWarp[i];
		}
	}

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, sizeof(pMsg));
}

float GensSystem::GetBattleZoneExpBonus(CGameObject &Obj)
{
	int iInfluence = this->GetGensInfluence(lpObj);

	if (iInfluence == NONE_INFLUENCE || iInfluence == LEAVED_INFLUENCE)
	{
		return 0.0;
	}

	if (this->IsMapBattleZone(Obj.MapNumber) == FALSE)
	{
		return 0.0;
	}

	std::map<int, BATTLEZONE_MAP_DATA>::iterator It = this->m_mapBattleZoneData.find(Obj.MapNumber);

	if (It == this->m_mapBattleZoneData.end())
	{
		return 0.0;
	}

	if (g_MasterLevelSkillTreeSystem.IsMasterLevelUser(lpObj) == true)
	{
		return It->second.m_fMLExpBonus[iInfluence - 1];
	}

	else
	{
		return It->second.m_fExpBonus[iInfluence - 1];
	}
}

int GensSystem::GetBattleZoneDropBonus(CGameObject &Obj)
{
	int iInfluence = this->GetGensInfluence(lpObj);

	if (iInfluence == NONE_INFLUENCE || iInfluence == LEAVED_INFLUENCE)
	{
		return 0;
	}

	if (this->IsMapBattleZone(Obj.MapNumber) == FALSE)
	{
		return 0;
	}

	std::map<int, BATTLEZONE_MAP_DATA>::iterator It = this->m_mapBattleZoneData.find(Obj.MapNumber);

	if (It == this->m_mapBattleZoneData.end())
	{
		return 0;
	}

	if (g_MasterLevelSkillTreeSystem.IsMasterLevelUser(lpObj) == true)
	{
		return It->second.m_iMLDropBonus[iInfluence - 1];
	}

	else
	{
		return It->second.m_iDropBonus[iInfluence - 1];
	}
}

int GensSystem::GetBattleZoneExcDropBonus(CGameObject &Obj)
{
	int iInfluence = this->GetGensInfluence(lpObj);

	if (iInfluence == NONE_INFLUENCE || iInfluence == LEAVED_INFLUENCE)
	{
		return 0;
	}

	if (this->IsMapBattleZone(Obj.MapNumber) == FALSE)
	{
		return 0;
	}

	std::map<int, BATTLEZONE_MAP_DATA>::iterator It = this->m_mapBattleZoneData.find(Obj.MapNumber);

	if (It == this->m_mapBattleZoneData.end())
	{
		return 0;
	}

	if (g_MasterLevelSkillTreeSystem.IsMasterLevelUser(lpObj) == true)
	{
		return It->second.m_iMLExcDropBonus[iInfluence - 1];
	}

	else
	{
		return It->second.m_iExcDropBonus[iInfluence - 1];
	}
}

bool GensSystem::GetEntryAllowType(CGameObject &Obj, BYTE btMapNumber)
{
	if (this->IsUserBattleZoneEnable(lpObj) == FALSE)
	{
		return false;
	}

	int iInfluence = this->GetGensInfluence(lpObj);

	if (iInfluence == NONE_INFLUENCE || iInfluence == LEAVED_INFLUENCE)
	{
		return false;
	}

	std::map<int, BATTLEZONE_MAP_DATA>::iterator It = this->m_mapBattleZoneData.find(btMapNumber);

	if (It == this->m_mapBattleZoneData.end())
	{
		return false;
	}

	if (It->second.m_btEntryAllowType == 0)
	{
		return true;
	}

	else if (It->second.m_btEntryAllowType == 1 && iInfluence == DUPRIAN_INFLUENCE)
	{
		return true;
	}

	else if (It->second.m_btEntryAllowType == 2 && iInfluence == VANERT_INFLUENCE)
	{
		return true;
	}
	
	return false;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

