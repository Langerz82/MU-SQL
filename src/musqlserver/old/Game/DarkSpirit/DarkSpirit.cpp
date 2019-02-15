////////////////////////////////////////////////////////////////////////////////
// DarkSpirit.cpp
#include "StdAfx.h"
#include "DarkSpirit.h"
#include "MapClass.h"
#include "GameMain.h"
#include "GameProtocol.h"
#include "BattleSoccerManager.h"
#include "gObjMonster.h"
#include "ObjAttack.h"
#include "ChaosCastle.h"
#include "BuffEffectSlot.h"
#include "IllusionTempleEvent_Renewal.h"
#include "configread.h"
#include "ImperialGuardian.h"
#include "GensSystem.h"
#include "AcheronGuardianEvent.h"
#include "ItemObject.h"
#include "NewPVP.h"
#include "ArcaBattle.h"
#include "EvolutionMonsterMng.h"

// GS-N 0.99.60T 0x004AA110
//	GS-N	1.00.18	JPN	0x004C7E00	-	Completed

CDarkSpirit * gDarkSpirit;



CDarkSpirit::CDarkSpirit() : m_ActionMode(PetItem_Mode_Normal)
{
	InitializeCriticalSection(&this->m_SpiritCriti);
	this->Init();
}



CDarkSpirit::~CDarkSpirit()
{
	DeleteCriticalSection(&this->m_SpiritCriti);
	return;
}



void CDarkSpirit::Init()
{
	this->m_AttackDamageMin = 0;
	this->m_AttackDamageMax = 0;
	this->m_AttackSpeed = 0;
	this->m_SuccessAttackRate = 0;
	this->m_dwLastAttackTime = 0;
	this->m_iMasterIndex = -1;
	this->m_iTargetIndex = -1;
	this->m_pPetItem = NULL;
}



void CDarkSpirit::Run()
{
	EnterCriticalSection(&this->m_SpiritCriti);

	int masteridx = this->m_iMasterIndex;

	if ( masteridx < g_ConfigRead.server.GetObjectStartUserIndex() || masteridx > g_ConfigRead.server.GetObjectMax()-1 )
	{
		LeaveCriticalSection(&this->m_SpiritCriti);
		return;
	}

	if(!gObjIsConnected(masteridx))
	{
		LeaveCriticalSection(&this->m_SpiritCriti);
		return;
	}

	CGameObject lpObj = &getGameObject(masteridx);

	if ( Obj.Class != 4 ) // dñl
	{
		LeaveCriticalSection(&this->m_SpiritCriti);
		return;
	}

	if ( this->m_pPetItem == NULL )
	{
		LeaveCriticalSection(&this->m_SpiritCriti);
		return;
	}

	if ( this->m_pPetItem->m_Durability <= 0.0f )
	{
		LeaveCriticalSection(&this->m_SpiritCriti);
		return;
	}

	if (!MAX_MAP_RANGE(Obj.MapNumber))
	{
		LeaveCriticalSection(&this->m_SpiritCriti);
		return;
	}

	BYTE attr = MapC[Obj.MapNumber].GetAttr(Obj.X, Obj.Y);

	if ( (attr&1) != 0 )
	{
		LeaveCriticalSection(&this->m_SpiritCriti);
		return;
	}

	if ( this->m_dwLastAttackTime > GetTickCount() )
	{
		LeaveCriticalSection(&this->m_SpiritCriti);
		return;
	}

	int nAttackSpeed = this->m_AttackSpeed;

	if (Obj.m_PlayerData->m_MPSkillOpt.iMpsIncDarkSpiritAttackSpeed > 0.0)
	{
		nAttackSpeed += Obj.m_PlayerData->m_MPSkillOpt.iMpsIncDarkSpiritAttackSpeed;
	}

	this->m_dwLastAttackTime = ( GetTickCount() + 1500 ) - ( nAttackSpeed * 10 );

	int iActionMode = this->m_ActionMode;
	LeaveCriticalSection(&this->m_SpiritCriti);

	switch ( iActionMode )
	{
		case PetItem_Mode_Normal:
			this->ModeNormal();
			break;

		case PetItem_Mode_Attack_Random:
			this->ModeAttackRandom();
			break;

		case PetItem_Mode_Attack_WithMaster:
			this->ModeAttackWithMaster();
			break;

		case PetItem_Mode_Attack_Target:
			this->ModeAttakTarget();
			break;
	}
}



void CDarkSpirit::ModeNormal()
{
	EnterCriticalSection(&this->m_SpiritCriti);

	if ( this->m_iMasterIndex < g_ConfigRead.server.GetObjectStartUserIndex() || this->m_iMasterIndex > g_ConfigRead.server.GetObjectMax()-1 )
	{
		return;
	}

	CGameObject lpObj = &getGameObject(this->m_iMasterIndex);

	LeaveCriticalSection(&this->m_SpiritCriti);
}



void CDarkSpirit::ModeAttackRandom()
{
	EnterCriticalSection(&this->m_SpiritCriti);

	if ( this->m_iMasterIndex < g_ConfigRead.server.GetObjectStartUserIndex() || this->m_iMasterIndex > g_ConfigRead.server.GetObjectMax()-1 )
	{
		LeaveCriticalSection(&this->m_SpiritCriti);
		return;
	}

	CGameObject lpObj = &getGameObject(this->m_iMasterIndex);

	int tObjNum;
	int count = 0;
	int FindObj[MAX_VIEWPORT];
	int FindObjCount = 0;
	int dis;
	int DuelIndex = Obj.m_iDuelUser;
	BOOL EnableAttack;
	int iDamageType = 0;
	int iActionType = 0;
	int iCriticalDamage = this->m_CriticalDamage + Obj.m_PlayerData->m_MPSkillOpt.iMpsAddCriticalDamageRate;
	int iExcellentDamage = this->m_ExcellentDamage + Obj.m_PlayerData->m_MPSkillOpt.iMpsAddExcellentDamageRate;
	int iDoubleDamage = this->m_DoubleDamage + Obj.m_PlayerData->m_MPSkillOpt.iMpsIncDarkSpiritDoubleDamageRate;

	LeaveCriticalSection(&this->m_SpiritCriti);

	if ((rand() % 100) < iCriticalDamage)
	{
		iDamageType = 1;
		iActionType = 1;
	}

	if ((rand() % 100) < iExcellentDamage)
	{
		iDamageType = 2;
		iActionType = 1;
	}

	if ((rand() % 100) < iDoubleDamage)
	{
		iDamageType = 3;
		iActionType = 1;
	}

	while ( true )
	{
		if ( Obj.VpPlayer2[count].state != 0 )
		{
			tObjNum = Obj.VpPlayer2[count].number;

			if ( tObjNum >= 0 )
			{
				EnableAttack = FALSE;
				
				if ( getGameObject(tObjNum)->Life > 0.0f && (getGameObject(tObjNum)->Class < 100 || getGameObject(tObjNum)->Class  > 110 ) )
				{
					if ( Obj.VpPlayer2[count].type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon < 0)
					{
						EnableAttack = TRUE;
					}

					else if ( DuelIndex == tObjNum )
					{
						EnableAttack = TRUE;
					}

					else
					{
						int lc85 = getGameObject(tObjNum)->m_Index;
						
						if ( getGameObject(tObjNum)->Type == OBJ_MONSTER )
						{
							if ( getGameObject(tObjNum)->m_RecallMon >= 0 )
							{
								lc85 = getGameObject(getGameObject(tObjNum)->m_RecallMon)->m_Index; 
							}
						}

						if ( gObjTargetGuildWarCheck(lpObj, &getGameObject(lc85)) == TRUE )
						{
							EnableAttack = TRUE;
						}

						if ( getGameObject(tObjNum)->Type == OBJ_USER && g_GensSystem.IsMapBattleZone(Obj.MapNumber) == TRUE && g_GensSystem.IsPkEnable(lpObj, &getGameObject(lc85)) == TRUE )
						{
							EnableAttack = TRUE;
						}

						if(getGameObject(tObjNum)->Class >= 678 && getGameObject(tObjNum)->Class <= 680)
						{
							EnableAttack = TRUE;
						}

						if ( IMPERIAL_MAP_RANGE(getGameObject(tObjNum)->MapNumber) == TRUE )
						{
							EnableAttack = g_ImperialGuardian.IsAttackAbleMonster(tObjNum);
						}
					}

					if ( EnableAttack != FALSE )
					{
						if ( Obj.MapNumber == getGameObject(tObjNum)->MapNumber )
						{
							dis = gObjCalDistance(lpObj, &getGameObject(tObjNum));
							
							if ( dis < RAVEN_ATTACK_DISTANCE-2 )
							{
								FindObj[FindObjCount] = tObjNum;
								FindObjCount++;
							}
						}
					}
				}
			}
		}

		count++;	
		if ( count > MAX_VIEWPORT-1 )
		{
			break;
		}
	}

	if ( FindObjCount != 0 )
	{
		if ( iDamageType != 0 )
		{
			int target = FindObj[rand()%FindObjCount];
			this->SendAttackMsg( lpObj, target, iDamageType, iActionType);
		}
		else
		{
			int target = FindObj[rand()%FindObjCount];
			this->RangeAttack( lpObj, target);
		}
	}
}




void CDarkSpirit::ModeAttackWithMaster()
{
	EnterCriticalSection(&this->m_SpiritCriti);

	if ( this->m_iMasterIndex < g_ConfigRead.server.GetObjectStartUserIndex() || this->m_iMasterIndex > g_ConfigRead.server.GetObjectMax()-1 )
	{
		LeaveCriticalSection(&this->m_SpiritCriti);
		return;
	}

	if ( this->m_iTargetIndex == -1 )
	{
		LeaveCriticalSection(&this->m_SpiritCriti);
		return;
	}

	CGameObject lpObj = &getGameObject(this->m_iMasterIndex);
	CGameObject lpTargetObj = &getGameObject(this->m_iTargetIndex);

	int iCriticalDamage = this->m_CriticalDamage + Obj.m_PlayerData->m_MPSkillOpt.iMpsAddCriticalDamageRate;
	int iExcellentDamage = this->m_ExcellentDamage + Obj.m_PlayerData->m_MPSkillOpt.iMpsAddExcellentDamageRate;
	int iDoubleDamage = this->m_DoubleDamage + Obj.m_PlayerData->m_MPSkillOpt.iMpsIncDarkSpiritDoubleDamageRate;
	int dis;

	LeaveCriticalSection(&this->m_SpiritCriti);

	if ( Obj.MapNumber == lpTargetObj.MapNumber )
	{
		if ( lpTargetObj.Life > 0.0f )
		{
			dis = gObjCalDistance(lpObj, lpTargetObj);

			if ( dis < RAVEN_ATTACK_DISTANCE )
			{
				int iDamageType = 0;
				int iActionType = 0;

				if ((rand() % 100) < iCriticalDamage)
				{
					iDamageType = 1;
					iActionType = 1;
				}

				if ((rand() % 100) < iExcellentDamage)
				{
					iDamageType = 2;
					iActionType = 1;
				}

				if ((rand() % 100) < iDoubleDamage)
				{
					iDamageType = 3;
					iActionType = 1;
				}

				if ( iDamageType != FALSE )
				{
					this->SendAttackMsg(Obj.m_Index, lpTargetObj.m_Index, iDamageType, iActionType);
				}
				else
				{
					this->RangeAttack(Obj.m_Index, lpTargetObj.m_Index);
				}
			}
		}
		else
		{
			this->ReSetTarget(lpTargetObj.m_Index);
		}
	}
}




void CDarkSpirit::ModeAttakTarget()
{
	EnterCriticalSection(&this->m_SpiritCriti);

	if ( this->m_iMasterIndex < g_ConfigRead.server.GetObjectStartUserIndex() || this->m_iMasterIndex > g_ConfigRead.server.GetObjectMax()-1 )
	{
		LeaveCriticalSection(&this->m_SpiritCriti);
		return;
	}

	if ( this->m_iTargetIndex == -1 )
	{
		LeaveCriticalSection(&this->m_SpiritCriti);
		return;
	}

	CGameObject lpObj = &getGameObject(this->m_iMasterIndex);
	CGameObject lpTargetObj = &getGameObject(this->m_iTargetIndex);
	int count = 0;
	int dis;

	LeaveCriticalSection(&this->m_SpiritCriti);

	if (g_GensSystem.IsMapBattleZone(Obj.MapNumber) == TRUE && g_GensSystem.IsPkEnable(lpObj, lpTargetObj) == FALSE)
	{
		return;
	}

	if ( Obj.MapNumber == lpTargetObj.MapNumber )
	{
		if ( lpTargetObj.Life > 0.0f )
		{
			dis = gObjCalDistance(lpObj, lpTargetObj);

			if ( dis < RAVEN_ATTACK_DISTANCE )
			{
				this->SendAttackMsg(Obj.m_Index, lpTargetObj.m_Index, 1, 1);
			}
		}
		else
		{
			this->ReSetTarget(lpTargetObj.m_Index);
		}
	}
}




void CDarkSpirit::RangeAttack(CGameObject &Obj, int aTargetIndex)
{
	EnterCriticalSection(&this->m_SpiritCriti);

	if ( this->m_iMasterIndex < g_ConfigRead.server.GetObjectStartUserIndex() || this->m_iMasterIndex > g_ConfigRead.server.GetObjectMax()-1 )
	{
		LeaveCriticalSection(&this->m_SpiritCriti);
		return;
	}

	LeaveCriticalSection(&this->m_SpiritCriti);

	
	int StartDis = 1;
	int tObjNum;
	int count = 0;
	int loopcount = 0;
	int attackcheck;
	int EnableAttack;
	int HitCount = 0;

	this->SendAttackMsg(Obj.m_Index, aTargetIndex, 0, 0);

	while ( true )
	{
		if ( Obj.VpPlayer2[count].state != 0 )
		{
			tObjNum = Obj.VpPlayer2[count].number;

			if ( tObjNum >= 0 && aTargetIndex != tObjNum )
			{
				EnableAttack = FALSE;
				
				if ( Obj.VpPlayer2[count].type == OBJ_MONSTER && getGameObject(tObjNum)->m_RecallMon < 0)
				{
					EnableAttack = TRUE;
				}

				else if ( tObjNum == aTargetIndex )
				{
					EnableAttack = TRUE;
				}

				else
				{
					int lc10 = getGameObject(tObjNum)->m_Index;

					if ( getGameObject(tObjNum)->Type == OBJ_MONSTER )
					{
						if ( getGameObject(tObjNum)->m_RecallMon >= 0 )
						{
							lc10 = getGameObject(getGameObject(tObjNum)->m_RecallMon)->m_Index;
						}
					}

					if ( gObjTargetGuildWarCheck(lpObj, &getGameObject(lc10)) == TRUE )
					{
						EnableAttack = TRUE;
					}

					if ( getGameObject(tObjNum)->Type == OBJ_USER && g_GensSystem.IsMapBattleZone(Obj.MapNumber) == TRUE && g_GensSystem.IsPkEnable(lpObj, &getGameObject(tObjNum)) == TRUE )
					{
						EnableAttack = TRUE;
					}

					if(getGameObject(tObjNum)->Class >= 678 && getGameObject(tObjNum)->Class <= 680)
					{
						EnableAttack = TRUE;
					}
				}

				if ( EnableAttack != FALSE )
				{
					attackcheck = FALSE;

					if ( loopcount == 0 )
					{
						attackcheck = TRUE;
					}
					else if ( (rand()%3) == 0 )
					{
						attackcheck = TRUE;
					}

					if ( attackcheck != FALSE )
					{
						if ( gObjCalDistance(&getGameObject(aTargetIndex), &getGameObject(tObjNum)) < RAVEN_ATTACK_DISTANCE-3 )
						{
							this->SendAttackMsg(Obj.m_Index, tObjNum, 2, 0);
							HitCount++;

							if ( HitCount > 3 )
							{
								break;
							}
						}
					}
				}
			}				
		}

		count++;

		if ( count > MAX_VIEWPORT-1 )
		{
			break;
		}
	}
}


struct PMSG_PET_ITEM_ATTACK_COMMAND
{
	PBMSG_HEAD2 h;	// C1:A8
	BYTE PetType;	//	3
	BYTE SkillType;	// 4
	BYTE NumberH;	// 5
	BYTE NumberL;	// 6
	BYTE TargetNumberH;	// 7
	BYTE TargetNumberL;	// 8
};

void CDarkSpirit::SendAttackMsg(CGameObject &Obj, CGameObject &ObjTarget, int iDamageType, int iActionType)
{
	PMSG_PET_ITEM_ATTACK_COMMAND pMsg;

	if ( iDamageType == 3 )
	{
		iDamageType = 0;
	}

	pMsg.PetType = 0;
	pMsg.SkillType = (iDamageType)? 0 : 1;
	pMsg.NumberH = SET_NUMBERH(Obj.m_Index);
	pMsg.NumberL = SET_NUMBERL(Obj.m_Index);
	pMsg.TargetNumberH = SET_NUMBERH(aTargetIndex);
	pMsg.TargetNumberL = SET_NUMBERL(aTargetIndex);
	pMsg.h.set((BYTE*)&pMsg, 0xA8, sizeof(pMsg));
	
	IOCP.DataSend(Obj.m_Index, (UCHAR*)&pMsg, pMsg.h.size);
	gGameProtocol.MsgSendV2(&getGameObject(Obj.m_Index), (UCHAR*)&pMsg, pMsg.h.size);

	if (iActionType == 1)
	{
		EnterCriticalSection(&this->m_SpiritCriti);
		
		CGameObject* lpObj = getGameObject(this->m_iMasterIndex);
		int iDamageType = 0;
		int iCriticalDamage = this->m_CriticalDamage + lpObj->m_PlayerData->m_MPSkillOpt.iMpsAddCriticalDamageRate;
		int iExcellentDamage = this->m_ExcellentDamage + lpObj->m_PlayerData->m_MPSkillOpt.iMpsAddExcellentDamageRate;
		int iDoubleDamage = this->m_DoubleDamage + lpObj->m_PlayerData->m_MPSkillOpt.iMpsIncDarkSpiritDoubleDamageRate;

		LeaveCriticalSection(&this->m_SpiritCriti);

		if (iCriticalDamage > rand() % 100)
		{
			iDamageType = 1;
		}

		if (iExcellentDamage > rand() % 100)
		{
			iDamageType = 2;
		}

		if (iDoubleDamage > rand() % 100)
		{
			iDamageType = 3;
		}
	}

	gObjAddAttackProcMsgSendDelay(&getGameObject(Obj.m_Index), 51, aTargetIndex, 600, iDamageType, iActionType);
}



void CDarkSpirit::SetTarget(CGameObject &Obj)
{
	EnterCriticalSection(&this->m_SpiritCriti);

	if ( this->m_iMasterIndex == -1 )
	{
		LeaveCriticalSection(&this->m_SpiritCriti);
		return;
	}

	if ( this->m_iMasterIndex < g_ConfigRead.server.GetObjectStartUserIndex() || this->m_iMasterIndex > g_ConfigRead.server.GetObjectMax()-1 )
	{
		LeaveCriticalSection(&this->m_SpiritCriti);
		return;
	}

	CGameObject lpObj = &getGameObject(this->m_iMasterIndex);

	if ( Obj.Class != 4 )	// DL
	{
		LeaveCriticalSection(&this->m_SpiritCriti);
		return;
	}

	this->m_iTargetIndex = aTargetIndex;
	LeaveCriticalSection(&this->m_SpiritCriti);
}



void CDarkSpirit::ReSetTarget(CGameObject &Obj)
{
	EnterCriticalSection(&this->m_SpiritCriti);

	if ( this->m_iMasterIndex == -1 )
	{
		LeaveCriticalSection(&this->m_SpiritCriti);
		return;
	}

	if ( this->m_iMasterIndex < g_ConfigRead.server.GetObjectStartUserIndex() || this->m_iMasterIndex > g_ConfigRead.server.GetObjectMax()-1 )
	{
		LeaveCriticalSection(&this->m_SpiritCriti);
		return;
	}

	CGameObject lpObj = &getGameObject(this->m_iMasterIndex);

	if ( Obj.Class != 4 )	// DL
	{
		LeaveCriticalSection(&this->m_SpiritCriti);
		return;
	}

	if ( aTargetIndex != this->m_iTargetIndex )
	{
		LeaveCriticalSection(&this->m_SpiritCriti);
		return;
	}

	this->m_iTargetIndex = -1;
	int iActionMode = this->m_ActionMode;

	LeaveCriticalSection(&this->m_SpiritCriti);

	if ( iActionMode == PetItem_Mode_Attack_Target )
	{
		this->SetMode(PetItem_Mode_Normal, -1);
	}
}



struct PMSG_SEND_PET_ITEM_COMMAND
{
	PBMSG_HEAD2 h;	// C1:A7
	BYTE PetType;	// 3
	BYTE Command;	// 4
	BYTE NumberH;	// 5
	BYTE NumberL;	// 6
};

void CDarkSpirit::SetMode(ePetItem_Mode mode, int iTargetindex)
{
	EnterCriticalSection(&this->m_SpiritCriti);

	if ( this->m_iMasterIndex == -1 )
	{
		LeaveCriticalSection(&this->m_SpiritCriti);
		return;
	}

	if ( this->m_iMasterIndex < g_ConfigRead.server.GetObjectStartUserIndex() || this->m_iMasterIndex > g_ConfigRead.server.GetObjectMax()-1 )
	{
		LeaveCriticalSection(&this->m_SpiritCriti);
		return;
	}

	CGameObject lpObj = &getGameObject(this->m_iMasterIndex);

	if ( Obj.Class != CLASS_DARKLORD )	// DL
	{
		LeaveCriticalSection(&this->m_SpiritCriti);
		return;
	}

	if ( mode == PetItem_Mode_Attack_Target || this->m_ActionMode != mode)
	{
		if ( mode == PetItem_Mode_Attack_Target )
		{
			if ( ObjectMaxRange(iTargetindex) != FALSE )
			{
				this->m_iTargetIndex = iTargetindex;
			}

			else
			{
				LeaveCriticalSection(&this->m_SpiritCriti);
				return;
			}
		}

		this->m_ActionMode = mode;
		int iTargetIndex = this->m_iTargetIndex;

		LeaveCriticalSection(&this->m_SpiritCriti);

		PMSG_SEND_PET_ITEM_COMMAND pMsg;

		pMsg.h.set((BYTE*)&pMsg, 0xA7, sizeof(pMsg));
		pMsg.PetType = 0;
		pMsg.Command = mode;
		pMsg.NumberH = SET_NUMBERH(iTargetIndex);
		pMsg.NumberL = SET_NUMBERL(iTargetIndex);
		IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (UCHAR*)&pMsg, sizeof(pMsg));
		return;
	}

	LeaveCriticalSection(&this->m_SpiritCriti);
}




void CDarkSpirit::Set(CGameObject &Obj, CItemObject * pPetItem)
{
	EnterCriticalSection(&this->m_SpiritCriti);
	

	if ( Obj.Class != 4 )	// DL
	{
		LeaveCriticalSection(&this->m_SpiritCriti);
		return;
	}

	BOOL bChange = FALSE;

	if ( this->m_iMasterIndex == -1 )
	{
		bChange = TRUE;
	}

	this->Init();

	if ( pPetItem->m_Type != ITEMGET(13,5) ) // Talon of Dark Raven
	{
		LeaveCriticalSection(&this->m_SpiritCriti);
		return;
	}

	if ( pPetItem->m_Durability <= 0.0f )
	{
		LeaveCriticalSection(&this->m_SpiritCriti);
		return;
	}

	int petitemlevel = pPetItem->m_PetItem_Level;
	int leadership = Obj.Leadership + Obj.AddLeadership;

	g_ConfigRead.m_ItemCalcLua.Generic_Call("DarkSpirit_CalcValues", "ii>iiddiiii", leadership, petitemlevel,
		&this->m_AttackDamageMin, &this->m_AttackDamageMax, &this->m_CriticalDamage, &this->m_ExcellentDamage,
		&this->m_DoubleDamage, &this->m_IgnoreEnemy, &this->m_AttackSpeed, &this->m_SuccessAttackRate);

	this->m_AttackSpeed += Obj.m_PlayerData->m_MPSkillOpt.iMpsAddDarkSpiritDamage;
	this->m_ExcellentDamage = 0;
	this->m_iMasterIndex = Obj.m_Index;
	this->m_pPetItem = pPetItem;

	this->m_iMasterIndex = Obj.m_Index;
	this->m_pPetItem = pPetItem;

	LeaveCriticalSection(&this->m_SpiritCriti);

	if ( bChange != FALSE )
	{
		this->SetMode(PetItem_Mode_Normal, -1);
	}
}



BOOL CDarkSpirit::Attack(CGameObject &Obj, CGameObject &TargetObj, CMagicInf * lpMagic, int iDamageType, int iActionType)
{
	int skillSuccess = 0;
	CGameObject lpCallObj;
	CGameObject lpCallTargetObj;
	int MsgDamage = 0;
	int ManaChange = 0;
	int iTempShieldDamage = 0;
	int iTotalShieldDamage = 0;

	if ( (Obj.Authority&2) == 2 || (lpTargetObj.Authority&2) == 2 ) //s4 add-on
	{
		return FALSE;
	}

	if ( (Obj.Authority&32) == 32 || (lpTargetObj.Authority&32) == 32) //s4 add-on
	{
		if(gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_INVISABLE) == TRUE)
		{
			return FALSE;
		}

		if(gObjCheckUsedBuffEffect(lpTargetObj, BUFFTYPE_INVISABLE) == TRUE)
		{
			return FALSE;
		}
	}

	if ( Obj.MapNumber != lpTargetObj.MapNumber )
	{
		return FALSE;
	}

	if ( IMPERIAL_MAP_RANGE(Obj.MapNumber) == TRUE && g_ImperialGuardian.IsAttackAbleMonster(lpTargetObj.m_Index) == false )
	{
		return FALSE;
	}

	if (lpTargetObj.Class == 681 || lpTargetObj.Class == 690)
	{
		int nOwnerIndex = g_EvoMonMng.GetOwnerIndex(lpTargetObj.m_Index);

		if (nOwnerIndex != -1)
		{
			if (nOwnerIndex != Obj.m_Index)
			{
				int nPartyNumber = Obj.PartyNumber;

				if (nPartyNumber < 0)
				{
					return FALSE;
				}

				else
				{
					bool bOwnerFound = false;

					for (int i = 0; i < MAX_USER_IN_PARTY; i++)
					{
						int nPartyIndex = gParty.m_PartyS[nPartyNumber].Number[i];

						if (nPartyIndex == nOwnerIndex)
						{
							bOwnerFound = true;
							break;
						}
					}

					if (bOwnerFound == false)
					{
						return FALSE;
					}
				}
			}
		}
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (g_Crywolf.GetCrywolfState() == 3 || g_Crywolf.GetCrywolfState() == 5)
		{
			if (CRYWOLF_MAP_RANGE(lpTargetObj.MapNumber))
			{
				if (lpTargetObj.Type == OBJ_MONSTER)
				{
					return FALSE;
				}
			}
		}
	}

	int skill = 0;
	
	if ( lpMagic )
		skill = lpMagic->m_Skill;

	skillSuccess = TRUE;

	if ( Obj.m_PlayerData->GuildNumber > 0 )
	{
		if ( Obj.m_PlayerData->lpGuild )
		{
			if ( Obj.m_PlayerData->lpGuild->WarState )
			{
				if ( Obj.m_PlayerData->lpGuild->WarType == 1 )
				{
					if ( !GetBattleSoccerGoalMove(0) )
					{
						return TRUE;
					}
				}
			}
		}
	}

	if ( lpTargetObj.Type == OBJ_MONSTER )
	{
		if ( lpTargetObj.Class == 200 )
		{
			if ( skill )
			{
				gObjMonsterStateProc(lpTargetObj, 7, Obj.m_Index, 0);
			}
			else
			{
				gObjMonsterStateProc(lpTargetObj, 6, Obj.m_Index, 0);
			}

			return TRUE;
		}
	}

	if ( Obj.Type == OBJ_USER )
	{
		if ( !gObjIsConnected(lpObj))
		{
			return FALSE;
		}
	}

	if ( lpTargetObj.Type == OBJ_USER )
	{
		if ( !gObjIsConnected(lpTargetObj))
		{
			return FALSE;
		}
	}

	if ( Obj.Type == OBJ_USER && lpTargetObj.Type == OBJ_MONSTER )	// PLAYER VS MONSTER
	{
		if ( Obj.m_RecallMon >= 0 )
		{
			if ( Obj.m_RecallMon == lpTargetObj.m_Index )
			{
				return FALSE;
			}
		}
	}

	if ( !gObjAttackQ(lpTargetObj))
	{
		return FALSE;
	}

	if (g_ArcaBattle.IsArcaBattleServer() == TRUE && g_AcheronGuardianEvent.IsPlayStart() == false && g_ArcaBattle.IsEnableAttackObelisk(lpObj, lpTargetObj.Class) == false)
	{
		return FALSE;
	}

	if (lpTargetObj.Type == OBJ_USER && g_NewPVP.IsDuel(*lpTargetObj) && g_NewPVP.IsSafeState(*lpTargetObj))
	{
		return FALSE;
	}

	if (Obj.Type == OBJ_USER && g_NewPVP.IsDuel(*lpObj) && g_NewPVP.IsSafeState(*lpObj))
	{
		return FALSE;
	}

	if (lpTargetObj.Type == OBJ_USER && g_NewPVP.IsObserver(*lpTargetObj))
	{
		return FALSE;
	}

	if (Obj.Type == OBJ_USER && g_NewPVP.IsObserver(*lpObj))
	{
		return FALSE;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
		{
			if (Obj.m_btCsJoinSide > 0)
			{
				if (Obj.m_btCsJoinSide == lpTargetObj.m_btCsJoinSide)
				{
					return FALSE;
				}
			}
		}
	}

	Obj.m_TotalAttackCount++;

	if ( this->CheckAttackArea(lpObj, lpTargetObj) == FALSE )
	{
		return FALSE;
	}

	lpCallObj = lpObj;
	lpCallTargetObj = lpTargetObj;

	if ( lpTargetObj.Type == OBJ_MONSTER )
	{
		if ( lpTargetObj.m_RecallMon >= 0 )
		{
			lpCallTargetObj = &getGameObject(lpTargetObj.m_RecallMon);
		}
	}

	if ( this->PkCheck(lpCallObj, lpTargetObj) == FALSE )
	{
		return FALSE;
	}

	int Strength = Obj.m_PlayerData->Strength + Obj.AddStrength;
	int Dexterity = Obj.m_PlayerData->Dexterity + Obj.AddDexterity;
	int Vitality = Obj.m_PlayerData->Vitality + Obj.AddVitality;
	int Energy = Obj.m_PlayerData->Energy + Obj.AddEnergy;
	BOOL bIsOnDuel = gObjDuelCheck(lpObj, lpTargetObj);

	if ( bIsOnDuel )
	{
		Obj.m_iDuelTickCount = GetTickCount();
		lpTargetObj.m_iDuelTickCount = GetTickCount();
	}

	int MSBFlag = 0;
	
	if ( iDamageType == 1 )
	{
		iDamageType = 3;
	}

	else if ( iDamageType == 2 )
	{
		MsgDamage = 2;
	}

	else
	{
		MsgDamage = 0;
	}

	BOOL bAllMiss = FALSE;
	int AttackDamage = 0;

	if (g_ConfigRead.g_ShieldSystemOn == TRUE )
	{
		if ( Obj.Type == OBJ_USER && lpTargetObj.Type == OBJ_USER )
		{
			if ( !this->MissCheckPvP(lpObj, lpTargetObj, skill, skillSuccess, bAllMiss) )
			{
				return FALSE;
			}
		}

		else if ( !this->MissCheck(lpObj, lpTargetObj, skill, skillSuccess, bAllMiss) )
		{
			return FALSE;
		}

	}

	else if ( !this->MissCheck(lpObj, lpTargetObj, skill, skillSuccess, bAllMiss) )
	{
		return FALSE;
	}

	int iOriginTargetDefense = 0;
	int targetdefense = this->GetTargetDefense(lpObj, lpTargetObj, MsgDamage, iOriginTargetDefense);

	EnterCriticalSection(&this->m_SpiritCriti);
	float fIgnoreEnemy = this->m_IgnoreEnemy;
	LeaveCriticalSection(&this->m_SpiritCriti);

	if (Obj.Type == OBJ_USER)
	{
		if (Obj.m_PlayerData->m_MPSkillOpt.iMpsDarkSpiritIgnoreEnemyDefense > 0.0)
		{
			fIgnoreEnemy += Obj.m_PlayerData->m_MPSkillOpt.iMpsDarkSpiritIgnoreEnemyDefense;
		}
	}

	if (rand() % 100 < fIgnoreEnemy)
	{
		targetdefense = 0;
		MsgDamage = 1;
	}

	AttackDamage = this->GetAttackDamage(lpObj, targetdefense, iDamageType);

	if (iDamageType == 0 && iActionType == 0)
	{
		AttackDamage /= 1.5;
	}

	if (iDamageType == 2)
	{
		AttackDamage += AttackDamage * 20.0 / 100.0;
	}

	if ( bAllMiss )
	{
		AttackDamage = ( AttackDamage * 30 ) / 100;
	}

	if ( lpTargetObj.DamageDecrease)
	{
		int beforeDamage = AttackDamage;
		AttackDamage -= ( ( AttackDamage * (int)lpTargetObj.DamageDecrease) / 100 );
	}

#if(CUSTOM_DAMAGES12 == 1)
	if (lpTargetObj.S12DamageDecrease)
	{
		int beforeDamage = AttackDamage;
		AttackDamage -= (AttackDamage + (int)lpTargetObj.S12DamageDecrease);
	}
#endif

	int tlevel = Obj.Level / 10;

	if ( AttackDamage < tlevel )
	{
		if ( tlevel < 1 )
		{
			tlevel = 1;
		}

		AttackDamage = tlevel;
	}

	if ( lpTargetObj.m_SkillNumber == 18 )
	{
		if ( AttackDamage > 1 )
		{
			AttackDamage >>= 1;
		}
	}

	if (g_ConfigRead.pet.DamageDisableForPet[DAMAGE_OFF_RAVEN] == false)
	{
		int idamage = AttackDamage * 2 / 100;

		Obj.pInventory[Obj.m_btInvenPetPos]->m_DurabilitySmall += idamage - (idamage * Obj.m_PlayerData->m_MPSkillOpt.iMpsPetDurDownSpeed / 100);
		int DurabilityVal = 65000;

		if (Obj.pInventory[Obj.m_btInvenPetPos]->m_DurabilitySmall >= DurabilityVal)
		{
			Obj.pInventory[Obj.m_btInvenPetPos]->m_DurabilitySmall = 0;
			Obj.pInventory[Obj.m_btInvenPetPos]->m_Durability -= 1.0f;

			if (Obj.pInventory[Obj.m_btInvenPetPos]->m_Durability < 1.0f)
			{
				Obj.pInventory[Obj.m_btInvenPetPos]->m_Durability = 0;
				UINT64 iPetExp = Obj.pInventory[Obj.m_btInvenPetPos]->m_PetItem_Exp;

				if (Obj.pInventory[Obj.m_btInvenPetPos]->DecPetItemExp(10))
				{
					this->Set(Obj.m_Index, &Obj.pInventory[Obj.m_btInvenPetPos]);
					this->SendLevelmsg(Obj.m_Index, Obj.m_btInvenPetPos, 0, 0xFF);
				}
			}

			gGameProtocol.GCItemObjectDurSend(Obj.m_Index, Obj.m_btInvenPetPos, Obj.pInventory[Obj.m_btInvenPetPos]->m_Durability, 0);
		}
	}

	if ( gObjAngelSprite(lpTargetObj) == TRUE )
	{
		if ( AttackDamage > 1 )
		{
			float  damage = (AttackDamage * 8) / 10.0f;
			AttackDamage = damage;
		}
	}

	if ( gObjWingSprite(lpTargetObj) == TRUE )
	{
		CItemObject * Wing = &lpTargetObj.pInventory[7];

		if ( AttackDamage > 1 && Wing->m_Type != ITEMGET(13, 30) )
		{
			double WingDamageBlock;

			if (lpTargetObj.m_PlayerData->m_MPSkillOpt.iMpsAddWingDamageBlock <= 0.0)
			{
				WingDamageBlock = 0.0;
			}

			else
			{
				WingDamageBlock = lpTargetObj.m_PlayerData->m_MPSkillOpt.iMpsAddWingDamageBlock;
			}

			g_ConfigRead.m_ItemCalcLua.Generic_Call("Wings_CalcAbsorb", "iiid>i", AttackDamage, Wing->m_Type, Wing->m_Level, WingDamageBlock, &AttackDamage);
		}
	}

	if ( gObjDenorantSprite(lpTargetObj ) )
	{
		CItemObject * Dinorant = &lpTargetObj.pInventory[8];
		int dinorantdecdamage = 90 - Dinorant->IsDinorantReduceAttackDamaege();
		Obj.Life -= 1.0f;

		if ( Obj.Life < 0.0f )
		{
			Obj.Life = 0.0f;
		}
		else
		{
			AttackDamage = AttackDamage * dinorantdecdamage / 100;
		}

		gGameProtocol.GCReFillSend(Obj.m_Index, Obj.Life, 0xFF, 0, Obj.iShield);
	}

	if ( gObjDarkHorse(lpTargetObj ) )
	{
		CItemObject * Darkhorse = &lpTargetObj.pInventory[8];
		int decdamage = 100 - ((Darkhorse->m_PetItem_Level + 30) / 2 );

		lpTargetObj.Life -= 1.0f;

		if ( lpTargetObj.Life < 0.0f )
		{
			lpTargetObj.Life = 0.0f;
		}
		else
		{
			AttackDamage = AttackDamage * decdamage / 100;
		}

		gGameProtocol.GCReFillSend(lpTargetObj.m_Index, lpTargetObj.Life, 0xFF, 0, lpTargetObj.iShield);
	}

	if ( lpTargetObj.Live )
	{
		if ( lpTargetObj.m_SkillInfo.SoulBarrierDefence && AttackDamage > 0)
		{
			int replacemana = (WORD)lpTargetObj.Mana * 2 / 100;

			if ( replacemana < lpTargetObj.Mana )
			{
				lpTargetObj.Mana -= replacemana;
				int decattackdamage = AttackDamage * lpTargetObj.m_SkillInfo.SoulBarrierDefence / 100;
				AttackDamage -= decattackdamage;
				ManaChange = TRUE;

			}
		}

		if (g_ConfigRead.g_ShieldSystemOn == FALSE )
		{
			if ( Obj.Type == OBJ_USER && lpTargetObj.Type == OBJ_USER )
			{
				if ( CC_MAP_RANGE(Obj.MapNumber ) && CC_MAP_RANGE(lpTargetObj.MapNumber) )
				{
					AttackDamage = AttackDamage * 50 / 100;
				}
			}

			if (Obj.Type == OBJ_USER && lpTargetObj.Type == OBJ_USER)
			{
				if (Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL && lpTargetObj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
				{
					AttackDamage = AttackDamage * 50 / 100;
				}
			}
		}

		if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
		{
			if (g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
			{
				if (Obj.Type == OBJ_USER && lpTargetObj.Type == OBJ_USER)
				{
					if (Obj.MapNumber == MAP_INDEX_CASTLESIEGE && lpTargetObj.MapNumber == MAP_INDEX_CASTLESIEGE)
					{
						if (Obj.m_btCsJoinSide == lpTargetObj.m_btCsJoinSide)
						{
							AttackDamage = AttackDamage * g_CastleSiege.CastleSiegeSelfDmgReductionPercent / 100;
						}
						else if (g_ConfigRead.g_ShieldSystemOn == FALSE)
						{
							AttackDamage = AttackDamage * g_CastleSiege.CastleSiegeDmgReductionPercent / 100;
						}
					}
				}
			}
		}

		if ( Obj.Type == OBJ_USER && lpTargetObj.Type == OBJ_MONSTER )
		{
			if ( lpTargetObj.Class == 283 )
			{
				if ( gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_BLESS_POTION) == TRUE || gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_SOUL_POTION) == TRUE )
				{
					AttackDamage += (AttackDamage * 20) / 100;
				}
				else 
				{
					if ( Obj.m_iAccumulatedDamage > 100 )
					{
						gObjWeaponDurDownInCastle(lpObj, lpTargetObj, 1);
						Obj.m_iAccumulatedDamage = 0;
					}
					else
					{
						Obj.m_iAccumulatedDamage += AttackDamage;
					}

					AttackDamage = AttackDamage * 20 / 100;
				}
			}

			if ( lpTargetObj.Class == 277 )
			{
				if ( gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_BLESS_POTION) == TRUE || gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_SOUL_POTION) == TRUE )
				{
					AttackDamage += (AttackDamage * 20) / 100;
				}
				else 
				{
					if ( Obj.m_iAccumulatedDamage > 100 )
					{
						gObjWeaponDurDownInCastle(lpObj, lpTargetObj, 1);
						Obj.m_iAccumulatedDamage = 0;
					}
					else
					{
						Obj.m_iAccumulatedDamage += AttackDamage;
					}

					AttackDamage = AttackDamage * 20 / 100;
				}
			}
		}

		if (bIsOnDuel == TRUE)
		{
			AttackDamage = AttackDamage * g_NewPVP.m_iDuelDamageReduction / 100;
		}

		if (g_GensSystem.IsMapBattleZone(Obj.MapNumber) && g_GensSystem.IsMapBattleZone(lpTargetObj.MapNumber))
		{
			if (Obj.Type == OBJ_USER && lpTargetObj.Type == OBJ_USER)
			{
				if (g_GensSystem.IsPkEnable(lpObj, lpTargetObj) == true)
				{
					AttackDamage = AttackDamage * g_GensSystem.GetDamageReduction() / 100;
				}
			}
		}

		if (Obj.Type == OBJ_USER && lpTargetObj.Type == OBJ_USER)
		{
			AttackDamage = AttackDamage * g_ConfigRead.calc.PvPDamageRate_DarkSpirit[Obj.Class][lpTargetObj.Class] / 100;
		}

		else if (Obj.Type == OBJ_USER && lpTargetObj.Type == OBJ_MONSTER)
		{
			AttackDamage = AttackDamage * g_ConfigRead.calc.PvMDamageRate_DarkSpirit / 100;
		}

		if ( Obj.Type == OBJ_USER && lpTargetObj.Type == OBJ_USER )
		{
			iTempShieldDamage = this->GetShieldDamage(lpObj, lpTargetObj, AttackDamage);
			lpTargetObj.iShield -= iTempShieldDamage;
			lpTargetObj.Life -= AttackDamage - iTempShieldDamage;
			iTotalShieldDamage += iTempShieldDamage;

			if ( lpTargetObj.Life < 0.0f )
			{
				lpTargetObj.Life = 0.0f;
			}
		}
		else
		{
			lpTargetObj.Life -= AttackDamage;

			if ( lpTargetObj.Life < 0.0f )
			{
				lpTargetObj.Life = 0.0f;
			}
		}
	}

	if ( lpTargetObj.Type == OBJ_MONSTER )
	{
		gObjAddMsgSendDelay(lpTargetObj, 0, Obj.m_Index, 100, 0);
		lpTargetObj.LastAttackerID = Obj.m_Index;

		if ( lpTargetObj.m_iCurrentAI )
		{
			lpTargetObj.m_Agro->IncAgro(Obj.m_Index, AttackDamage / 100);
		}
	}

	if ( gObjCheckUsedBuffEffect(lpTargetObj, BUFFTYPE_STONE) == true )
	{
		gObjRemoveBuffEffect(lpTargetObj, BUFFTYPE_STONE);
		gGameProtocol.GCMagicCancelSend(lpTargetObj, 51);
	}

	BOOL selfdefense = 0;
	lpCallObj = lpTargetObj;
	
	if ( lpTargetObj.Type == OBJ_MONSTER )
	{
		if ( lpTargetObj.m_RecallMon >= 0 )
		{
			lpCallObj = &getGameObject(lpTargetObj.m_RecallMon);
		}
	}

	if ( AttackDamage >= 1 )
	{
		if ( Obj.Type == OBJ_USER && lpTargetObj.Type == OBJ_USER )
		{
			if ( gObjDuelCheck(lpObj, lpTargetObj) )
			{
				selfdefense = 0;
			}
			else if ( CC_MAP_RANGE(Obj.MapNumber) || CC_MAP_RANGE(lpTargetObj.MapNumber) )
			{
				selfdefense = 0;
			}
			else if (Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL || lpTargetObj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
			{
				selfdefense = 0;
			}
			else if ( IT_MAP_RANGE(Obj.MapNumber) || IT_MAP_RANGE(lpTargetObj.MapNumber) )
			{
				selfdefense = 0;
			}
			else
			{
				selfdefense = 1;
			}

			if ( gObjGetRelationShip(lpObj, lpTargetObj) == 2 )
			{
				selfdefense = FALSE;
			}

			if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
			{
				if (g_CastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
				{
					if (Obj.m_btCsJoinSide > 0)
					{
						selfdefense = FALSE;
					}
				}
			}
		}
		else if ( lpTargetObj.Type == OBJ_MONSTER && Obj.Type == OBJ_USER )
		{
			if ( lpTargetObj.m_RecallMon >= 0 )
			{
				selfdefense = TRUE;
			}
		}

		if ( lpTargetObj.Type == OBJ_USER )
		{
			gObjArmorRandomDurDown(lpTargetObj, lpObj);
		}
	}

	if ( selfdefense == TRUE )
	{
		if ( !gObjTargetGuildWarCheck(lpObj, lpCallObj) )
		{
			if(lpCallObj.PartyNumber >= 0) //Season 2.5 add-on
			{
				//season3 removed party for LOL
				int number = 0;
				int partynum = lpCallObj.PartyNumber;
				
				if( (gParty.GetPKPartyPenalty(partynum)) < 5)
				{
					gObjCheckSelfDefense(lpObj, lpCallObj.m_Index);
				}	
			}
			else
			{
				gObjCheckSelfDefense(lpObj, lpCallObj.m_Index);
			}
		}
	}

	if ( AttackDamage >= 5 )	// To make strong hit
	{
		if ( lpTargetObj.Type == OBJ_MONSTER )
		{
			if ( (rand()%26) == 0 )
			{
				gObjAddMsgSendDelay(lpTargetObj,4, Obj.m_Index, 100, 0);
			}
		}
		else if ( (rand()%4) == 0 )
		{
			if ( !gObjUniriaSprite(lpTargetObj) )
			{
				MSBFlag = 1;
			}
		}
	}

	if (lpTargetObj.Type == OBJ_USER)
	{
		if (lpTargetObj.pInventory[lpTargetObj.m_btInvenPetPos]->IsItem() == TRUE && lpTargetObj.pInventory[lpTargetObj.m_btInvenPetPos]->m_Type == ITEMGET(13,37))
		{
			if (g_ConfigRead.data.common.DisableMSBEffect[MSB_DISABLE_CHARACTER_FENRIR] == true)
			{
				MSBFlag = FALSE;
			}
		}

		else
		{
			if (g_ConfigRead.data.common.DisableMSBEffect[MSB_DISABLE_CHARACTER] == true)
			{
				MSBFlag = FALSE;
			}
		}
	}

	else if (lpTargetObj.Type == OBJ_MONSTER)
	{
		if (g_ConfigRead.data.common.DisableMSBEffect[MSB_DISABLE_MONSTER] == true)
		{
			MSBFlag = FALSE;
		}
	}

	if ( ManaChange )
	{
		gGameProtocol.GCManaSend(lpTargetObj.m_Index, lpTargetObj.Mana, 0xFF, 0, lpTargetObj.BP);
	}

	if ( Obj.Type == OBJ_USER )
	{
		if ( Obj.m_Change == 9 )
		{
			gGameProtocol.GCMagicAttackNumberSend(lpObj, 3, lpTargetObj.m_Index, 1);
		}
	}

	Obj.m_Rest = 0;

	if ( AttackDamage > 0 )
	{
		int atd_reflect = (int)((float)AttackDamage * lpTargetObj.DamageReflect / 100.0f);

		if(lpTargetObj.DamageReflect > g_ConfigRead.calc.ReflectDamage)
		{
			lpTargetObj.DamageReflect = g_ConfigRead.calc.ReflectDamage;
		}

		if ( atd_reflect )
		{
			gObjAddMsgSendDelay(lpTargetObj, 10, Obj.m_Index, 10, atd_reflect);
		}

		if ( Obj.Type == OBJ_USER && (rand()%100) < Obj.m_PlayerData->SetOpReflectionDamage )
		{
			gObjAddMsgSendDelay(lpTargetObj, 10, Obj.m_Index, 10, AttackDamage);
		}

		if (g_ConfigRead.g_ShieldSystemOn == TRUE )	// #error Remove the //
		{
			AttackDamage -= iTotalShieldDamage;
		}

		gObjLifeCheck(lpTargetObj, lpObj, AttackDamage, 0, MSBFlag, MsgDamage, skill, iTotalShieldDamage, 0);
	}
	else
	{
		gGameProtocol.GCDamageSend(Obj.m_Index, lpTargetObj.m_Index, 0, 0, MsgDamage, 0);
	}

	if ( Obj.Life <= 0.0f && Obj.Type == OBJ_USER )
	{
		if ( Obj.m_CheckLifeTime <= 0 )
		{
			Obj.lpAttackObj = lpTargetObj;

			if ( lpTargetObj.Type == OBJ_USER )
			{
				Obj.m_bAttackerKilled = true;
			}
			else
			{
				Obj.m_bAttackerKilled = false;
			}

			Obj.m_CheckLifeTime = 3;
		}
	}

	return TRUE;
}



int  CDarkSpirit::GetAttackDamage(CGameObject &Obj, int targetDefense, int crititcaldamage)
{
	if (Obj.Type != OBJ_USER)
	{
		return 0;
	}

	EnterCriticalSection(&this->m_SpiritCriti);

	int ad;
	int sub = this->m_AttackDamageMax - this->m_AttackDamageMin;
	int AttackDamage;

	if (sub < 0)
		sub = 0;

	if ( crititcaldamage == 1 )
	{
		AttackDamage = this->m_AttackDamageMin + sub;
	}

	else if ( crititcaldamage == 2 )
	{
		AttackDamage = this->m_AttackDamageMax;
		AttackDamage += ((this->m_AttackDamageMin + sub) * 20) / 100.0f;
	}

	else
	{
		__try
		{
			AttackDamage = this->m_AttackDamageMin + (rand() % (sub + 1));
		}

		__except (sub = 1, 1)
		{

		}
	}

	if (GetItemKindA(Obj.pInventory[0]->m_Type) == ITEM_KIND_A_WEAPON && GetItemKindB(Obj.pInventory[0]->m_Type) == ITEM_KIND_B_SCEPTER)
	{
		int damage = Obj.pInventory[0]->m_Magic / 2;
		damage -= damage * Obj.pInventory[0]->m_CurrentDurabilityState;
		damage = AttackDamage * damage / 100;
		AttackDamage += damage;
	}

	if (Obj.m_PlayerData->m_MPSkillOpt.iMpsAddDarkSpiritDamage > 0.0)
	{
		AttackDamage += Obj.m_PlayerData->m_MPSkillOpt.iMpsAddDarkSpiritDamage;
	}

	if (Obj.m_PlayerData->m_MPSkillOpt.iMpsAddPetAttack > 0.0)
	{
		AttackDamage += Obj.m_PlayerData->m_MPSkillOpt.iMpsAddPetAttack;
	}

	AttackDamage = AttackDamage * g_ConfigRead.calc.DarkRavenDamageMultiplier / 100.0f;
	ad = AttackDamage - targetDefense;

	LeaveCriticalSection(&this->m_SpiritCriti);
	return ad;
}



BOOL CDarkSpirit::MissCheck(CGameObject &Obj, CGameObject &TargetObj, int skill,  int skillSuccess, BOOL& bAllMiss)
{
	EnterCriticalSection(&this->m_SpiritCriti);
	int SuccessAttackRate = this->m_SuccessAttackRate;
	LeaveCriticalSection(&this->m_SpiritCriti);

	int TargetSuccessfulBlocking = TargetObj.m_SuccessfulBlocking;
	int MsgDamage = 0;

	if (IT_MAP_RANGE(TargetObj.MapNumber) && TargetObj.Type == OBJ_USER)
	{
		if (g_IT_Event.GetIllusionTempleState(TargetObj.MapNumber) == 2)
		{
			if (g_IT_Event.CheckSkillProdection(TargetObj.m_nITR_Index, TargetObj.MapNumber) == TRUE)
			{
				gGameProtocol.GCDamageSend(lpObj, TargetObj, 0, 0, 0, 0);
				return 0;
			}

			if (TargetObj.PartyNumber == Obj.PartyNumber)
			{
				gGameProtocol.GCDamageSend(Obj.m_Index, TargetObj.m_Index, 0, 0, 0, 0);
				return 0;
			}
		}
	}

	if (Obj.Type == OBJ_USER)
	{
		if (Obj.m_PlayerData->m_MPSkillOpt.iMpsAttackSuccessRate > 0.0)
		{
			SuccessAttackRate += Obj.m_PlayerData->m_MPSkillOpt.iMpsAttackSuccessRate;
		}

	}

	if ( SuccessAttackRate < TargetSuccessfulBlocking )
	{
		bAllMiss = TRUE;
	}

	if ( bAllMiss != FALSE )
	{
		if ( (rand()%100) >= 5 )
		{
			gGameProtocol.GCDamageSend(Obj, TargetObj, 0, 0, MsgDamage, 0);
			return FALSE;
		}
	}
	else if ( SuccessAttackRate )
	{
		if ( (rand()%SuccessAttackRate) < TargetSuccessfulBlocking)
		{
			gGameProtocol.GCDamageSend(Obj, TargetObj, 0, 0, MsgDamage, 0);
			return FALSE;
		}
	}

	return TRUE;
}


BOOL CDarkSpirit::MissCheckPvP(CGameObject &Obj, CGameObject &TargetObj, int skill,  int skillSuccess, BOOL& bAllMiss)
{
	float iAttackRate = 0;
	float iDefenseRate = 0;
	int iAttackSuccessRate = 0;
	int AttackLevel = 0;
	int DefenseLevel = 0;

	if(IT_MAP_RANGE(lpTargetObj.MapNumber) && lpTargetObj.Type == OBJ_USER)
	{
		if (g_IT_Event.GetIllusionTempleState(lpTargetObj.MapNumber) == 2)
		{
			if (g_IT_Event.CheckSkillProdection(lpTargetObj.m_nITR_Index, lpTargetObj.MapNumber) == TRUE)
			{
				gGameProtocol.GCDamageSend(Obj.m_Index, lpTargetObj.m_Index, 0, 0, 0, 0);
				return 0;
			}
			
			if (lpTargetObj.PartyNumber == Obj.PartyNumber)
			{
				gGameProtocol.GCDamageSend(Obj.m_Index, lpTargetObj.m_Index, 0, 0, 0, 0);
				return 0;
			}
		}
	}

	AttackLevel = Obj.Level + Obj.m_PlayerData->MasterLevel;

	switch (Obj.Class)
	{
	case CLASS_WIZARD:
		iAttackRate = (float)(AttackLevel * 3 + (Obj.m_PlayerData->Dexterity * 4));	// #formula
		break;
	case CLASS_KNIGHT:
		iAttackRate = (float)(AttackLevel * 3 + (Obj.m_PlayerData->Dexterity * 45) / 10);	// #formula
		break;
	case CLASS_ELF:
		iAttackRate = (float)(AttackLevel * 3 + (Obj.m_PlayerData->Dexterity * 6) / 10);	// #formula
		break;
	case CLASS_MAGUMSA:
		iAttackRate = (float)(AttackLevel * 3 + (Obj.m_PlayerData->Dexterity * 35) / 10);	// #formula
		break;
	case CLASS_DARKLORD:
		iAttackRate = (float)(AttackLevel * 3 + (Obj.m_PlayerData->Dexterity * 4));	// #formula
		break;
	case CLASS_SUMMONER:
		iAttackRate = (float)(AttackLevel * 3 + (Obj.m_PlayerData->Dexterity * 35) / 10);	// #formula
		break;
	case CLASS_RAGEFIGHTER:
		iAttackRate = (float)(AttackLevel * 2.6 + (Obj.m_PlayerData->Dexterity * 36) / 10);	// #formula
		break;
	case CLASS_GROWLANCER:
		iAttackRate = (float)(AttackLevel * 3 + (Obj.m_PlayerData->Dexterity * 2));	// #formula
		break;
	}

	DefenseLevel = lpTargetObj.Level + lpTargetObj.m_PlayerData->MasterLevel;

	switch (lpTargetObj.Class)
	{
	case CLASS_WIZARD:
		iDefenseRate = (float)(DefenseLevel * 2 + lpTargetObj.m_PlayerData->Dexterity / 4);	// #formula
		break;
	case CLASS_KNIGHT:
		iDefenseRate = (float)(DefenseLevel * 2 + lpTargetObj.m_PlayerData->Dexterity / 2);	// #formula
		break;
	case CLASS_ELF:
		iDefenseRate = (float)(DefenseLevel * 2 + lpTargetObj.m_PlayerData->Dexterity / 10);	// #formula
		break;
	case CLASS_MAGUMSA:
		iDefenseRate = (float)(DefenseLevel * 2 + lpTargetObj.m_PlayerData->Dexterity / 4);	// #formula
		break;
	case CLASS_DARKLORD:
		iDefenseRate = (float)(DefenseLevel * 2 + lpTargetObj.m_PlayerData->Dexterity / 2);	// #formula
		break;
	case CLASS_SUMMONER:
		iDefenseRate = (float)(DefenseLevel * 2 + lpTargetObj.m_PlayerData->Dexterity / 2);	// #formula
		break;
	case CLASS_RAGEFIGHTER:
		iDefenseRate = (float)(DefenseLevel*1.5f + lpTargetObj.m_PlayerData->Dexterity / 5);	// #formula
		break;
	case CLASS_GROWLANCER:
		iDefenseRate = (float)(DefenseLevel * 2 + lpTargetObj.m_PlayerData->Dexterity / 2);	// #formula
		break;
	}

	if ( iAttackRate <= 0.0f || iDefenseRate <= 0.0f || AttackLevel <= 0 || DefenseLevel <= 0 )
		return FALSE;

	iAttackRate += Obj.m_PlayerData->m_ItemOptionExFor380.OpAddAttackSuccessRatePVP;
	iDefenseRate += lpTargetObj.m_PlayerData->m_ItemOptionExFor380.OpAddDefenseSuccessRatePvP;

	iAttackRate += Obj.m_PlayerData->m_JewelOfHarmonyEffect.HJOpAddAttackSuccessRatePVP;
	iDefenseRate += lpTargetObj.m_PlayerData->m_JewelOfHarmonyEffect.HJOpAddDefenseSuccessRatePvP;

	iAttackRate += Obj.m_PlayerData->m_MPSkillOpt.iMpsIncreasePvPAttackRate;
	iDefenseRate += lpTargetObj.m_PlayerData->m_MPSkillOpt.iMpsIncreasePvPDefenseRate;

	float iExpressionA = ( iAttackRate * 10000.0f ) / ( iAttackRate + iDefenseRate );	// #formula
	float iExpressionB = ( AttackLevel * 10000 ) / ( AttackLevel + DefenseLevel );	// #formula

	iExpressionA /= 10000.0f;
	iExpressionB /= 10000.0f;

	iAttackSuccessRate = 100.0f * iExpressionA * g_ConfigRead.g_fSuccessAttackRateOption * iExpressionB;

	if ( (DefenseLevel - AttackLevel) >= 100 )
	{
		iAttackSuccessRate -= 5;
	}
	else if ( (DefenseLevel - AttackLevel) >= 200 )
	{
		iAttackSuccessRate -= 10;
	}
	else if ( (DefenseLevel - AttackLevel) >= 300 )
	{
		iAttackSuccessRate -= 15;
	}

	DWORD dwRate = rand() % 100;

	if ( dwRate > iAttackSuccessRate )
	{
		gGameProtocol.GCDamageSend(Obj.m_Index, lpTargetObj.m_Index, 0, 0, 0, 0);
		return FALSE;
	}

	return TRUE;
}

	

void CDarkSpirit::ChangeCommand(int command, int targetindex)
{
	CDarkSpirit::ePetItem_Mode setmode = PetItem_Mode_Normal;

	if ( command == PetItem_Mode_Normal )
	{
		setmode = PetItem_Mode_Normal;
	}

	else if ( command == PetItem_Mode_Attack_Random )
	{
		setmode = PetItem_Mode_Attack_Random;
	}

	else if ( command == PetItem_Mode_Attack_WithMaster )
	{
		setmode = PetItem_Mode_Attack_WithMaster;
	}

	else if ( command == PetItem_Mode_Attack_Target )
	{
		setmode = PetItem_Mode_Attack_Target;
	}

	this->SetMode(setmode, targetindex);
}

void __cdecl CDarkSpirit::SendLevelmsg(CGameObject &Obj, int nPos, int PetType, int InvenType)
{
	PMSG_SEND_PET_ITEMINFO pMsg;

	pMsg.h.set((BYTE*)&pMsg, 0xA9, sizeof(pMsg));
	pMsg.PetType = PetType;
	pMsg.InvenType = InvenType;
	pMsg.nPos = nPos;
	pMsg.Level = Obj.pInventory[nPos]->m_PetItem_Level;

	if (PetType == 1)
	{
		pMsg.Exp = Obj.pInventory[nPos]->m_PetItem_Exp - gPetItemExp.m_DarkHorseExpTable[pMsg.Level];
	}

	else
	{
		pMsg.Exp = Obj.pInventory[nPos]->m_PetItem_Exp - gPetItemExp.m_DarkSpiritExpTable[pMsg.Level];
	}

	pMsg.Dur = Obj.pInventory[nPos]->m_Durability;

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}


int CDarkSpirit::GetShieldDamage(CGameObject &Obj, CGameObject &TargetObj, int iAttackDamage)
{
	int iShieldDamage = 0;

	if (g_ConfigRead.g_ShieldSystemOn == FALSE)
		return 0;

	if (iAttackDamage <= 0)
		return 0;

	if (Obj.Type != OBJ_USER)
		return 0;

	if (lpTargetObj.Type != OBJ_USER)
		return 0;

	int iReduceLife = 0;
	int iReduceShield = 0;
	int iReduceLifeForEffect = 0; 
	bool bReduceShieldGage = 0;
	int iDamageDevideToSDRate = g_ConfigRead.g_iDamageDevideToSDRate;
	iDamageDevideToSDRate -= Obj.m_PlayerData->m_JewelOfHarmonyEffect.HJOpDecreaseSDRate;
	iDamageDevideToSDRate += lpTargetObj.m_PlayerData->m_JewelOfHarmonyEffect.HJOpAddSDRate;

	if ( iDamageDevideToSDRate < 0 )
		iDamageDevideToSDRate = 0;

	if ( iDamageDevideToSDRate > 100 )
		iDamageDevideToSDRate = 100;

	if ( Obj.m_PlayerData->m_JewelOfHarmonyEffect.HJOpAddIgnoreSDRate > 0 )
	{
		int iRand = rand() % 100;
		int iIgnoreSDRate = Obj.m_PlayerData->m_JewelOfHarmonyEffect.HJOpAddIgnoreSDRate - lpTargetObj.m_PlayerData->m_Resistance_SD;

		if (iRand < iIgnoreSDRate)
		{
			iDamageDevideToSDRate = 0;
		}
	}

	if ( (Obj.Type == OBJ_USER && lpTargetObj.Type == OBJ_USER) && ( Obj.m_PlayerData->m_JewelOfHarmonyEffect.HJOpDecreaseSDRate || lpTargetObj.m_PlayerData->m_JewelOfHarmonyEffect.HJOpAddSDRate || Obj.m_PlayerData->m_JewelOfHarmonyEffect.HJOpAddIgnoreSDRate ) )
	{
	
	}

	iReduceShield = iAttackDamage * iDamageDevideToSDRate / 100;
	iReduceLife = iAttackDamage - iReduceShield;

	if ( (lpTargetObj.iShield-iReduceShield) <0 )
	{
		iReduceLife += iReduceShield  - lpTargetObj.iShield;
		iReduceShield = lpTargetObj.iShield;

		if ( lpTargetObj.iShield > 0 )
		{
			bReduceShieldGage = true;
		}
	}

	iReduceLifeForEffect = ( lpTargetObj.MaxLife + lpTargetObj.AddLife ) * 20.0f / 100.0f;

	if ( bReduceShieldGage == true && iReduceLife > iReduceLifeForEffect )
	{
		if ( !CC_MAP_RANGE(lpTargetObj.MapNumber) && lpTargetObj.MapNumber != MAP_INDEX_CHAOSCASTLE_SURVIVAL )
		{
			gGameProtocol.GCSendEffectInfo(lpTargetObj.m_Index, 17);
		}
	}

	return iReduceShield;
}


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

