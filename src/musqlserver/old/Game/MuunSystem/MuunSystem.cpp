////////////////////////////////////////////////////////////////////////////////
// MuunSystem.cpp
//#include "StdAfx.h"
#include "GameProtocol.h"
#include "Main.h"
#include "ItemSocketOptionSystem.h"
#include "PentagramSystem.h"
#include "NewPVP.h"
#include "ObjCalCharacter.h"
#include "MuunSystem.h"
#include "gObjMonster.h"
#include "ItemOptionTypeMng.h"
#include "GensSystem.h"
#include "LargeRand.h"
#include "BuffEffectSlot.h"
#include "ImperialGuardian.h"
#include "BattleSoccerManager.h"
#include "Event/AcheronGuardianEvent/AcheronGuardianEvent.h"
#include "ObjAttack.h"
#include "ChaosCastle.h"
#include "OfflineLevelling.h"
#include "LastManStanding.h"
#include "EvolutionMonsterMng.h"
#include "SocketItemType.h"

CMuunAttack::CMuunAttack()
{

}

CMuunAttack::~CMuunAttack()
{

}

void CMuunAttack::SendAttackMsg(CGameObject &Obj, int aTargetIndex, int SubCode, int SubCode2)
{
	if (SubCode == MUUN_DEC_ENEMY_ATTACK_SKILL)
	{
		aTargetIndex = Obj.m_Index;
	}

	if (SubCode == MUUN_ATTACK_SKILL || SubCode == MUUN_ATTACK_SKILL_2 || SubCode == MUUN_ATTACK_SKILL_NONPVP)
	{
		if (gObjCalDistance(getGameObject(Obj.m_Index), getGameObject(aTargetIndex)) > 4)
		{
			g_CMuunSystem.ReSetTarget(getGameObject(Obj.m_Index), aTargetIndex);
			return;
		}

		if (getGameObject(aTargetIndex)->Life <= 0.0)
		{
			g_CMuunSystem.ReSetTarget(getGameObject(Obj.m_Index), aTargetIndex);
			return;
		}

		if (SubCode == MUUN_ATTACK_SKILL_NONPVP)
		{
			if (getGameObject(aTargetIndex)->Type == OBJ_USER)
			{
				g_CMuunSystem.ReSetTarget(getGameObject(Obj.m_Index), aTargetIndex);
				return;
			}
		}
	}

	if (SubCode != MUUN_DEC_ENEMY_ATTACK_SKILL)
	{
		PMSG_MUUN_ATTACK_COMMAND pMsg;

		pMsg.Slot = SubCode2;
		pMsg.SkillType = SubCode;
		pMsg.NumberH = HIBYTE(Obj.m_Index);
		pMsg.NumberL = LOBYTE(Obj.m_Index);
		pMsg.TargetNumberH = HIBYTE(aTargetIndex);
		pMsg.TargetNumberL = LOBYTE(aTargetIndex);

		pMsg.h.set((BYTE*)&pMsg, 0x4E, 0x12, sizeof(pMsg));

		IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
		gGameProtocol.MsgSendV2(getGameObject(Obj.m_Index), (BYTE*)&pMsg, pMsg.h.size);
	}

	gObjAddAttackProcMsgSendDelay(getGameObject(Obj.m_Index), 63, aTargetIndex, 600, SubCode, SubCode2);
}

void CMuunAttack::SkillProc(CGameObject &Obj)
{
	for (int i = 0; i < MAX_MUUN_EFFECT_LIST; i++)
	{
		if (Obj.pntMuunInventory[i]->m_Durability > 0.0 &&
			Obj.pntMuunEffectList[i]->bOptEnable == true &&
			(Obj.pntMuunEffectList[i]->nOptType == MUUN_ATTACK_SKILL ||
			Obj.pntMuunEffectList[i]->nOptType == MUUN_DEC_ENEMY_ATTACK_SKILL ||
			Obj.pntMuunEffectList[i]->nOptType == MUUN_ATTACK_SKILL_2 ||
			Obj.pntMuunEffectList[i]->nOptType == MUUN_ATTACK_SKILL_NONPVP))
		{
			DWORD dwSkillDelayTime = Obj.pntMuunEffectList[i]->nSkillDelayTime;
			int nTargetIndex = Obj.pntMuunEffectList[i]->nTargetIndex;

			if (nTargetIndex >= 0 || (Obj.pntMuunEffectList[i]->nOptType != MUUN_ATTACK_SKILL && Obj.pntMuunEffectList[i]->nOptType != MUUN_ATTACK_SKILL_2 && Obj.pntMuunEffectList[i]->nOptType != MUUN_ATTACK_SKILL_NONPVP))
			{
				if (g_GensSystem.IsPkEnable(lpObj, getGameObject(nTargetIndex)) == FALSE)
				{
					return;
				}

				if ((MapC[getGameObject(nTargetIndex)->MapNumber]->GetAttr(getGameObject(nTargetIndex)->X, getGameObject(nTargetIndex)->Y) & 1) == 1)
				{
					g_CMuunSystem.ReSetTarget(lpObj, nTargetIndex);
					return;
				}

				if ((MapC[Obj.MapNumber].GetAttr(Obj.X, Obj.Y) & 1) == 1)
				{
					g_CMuunSystem.ReSetTarget(lpObj, nTargetIndex);
					return;
				}

				if (Obj.pntMuunInventory[i]->IsMuunItemPeriodExpire() == FALSE && Obj.pntMuunEffectList[i]->nAddOptType == 3)
				{
					dwSkillDelayTime /= Obj.pntMuunEffectList[i]->nAddOptValue;
				}

				if (GetTickCount() - Obj.pntMuunEffectList[i]->nTickTime > dwSkillDelayTime)
				{
					Obj.pntMuunEffectList[i]->nTickTime = GetTickCount();
					this->SendAttackMsg(Obj.m_Index, nTargetIndex, Obj.pntMuunEffectList[i]->nOptType, i);
				}
			}
		}
	}
}

bool CMuunAttack::DamageAbsorb(CGameObject &Obj, CGameObject lpTargetObj, CMagicInf *lpMagic, int SubCode2)
{
	Obj.pntMuunEffectList[SubCode2]->bSkillUsed = false;
	return true;
}

bool CMuunAttack::Stun(CGameObject &Obj, CGameObject lpTargetObj, CMagicInf *lpMagic, int SubCode2)
{
	if (g_ConfigRead.EnableStunEffect == 1)
	{
		int iValue = Obj.pntMuunEffectList[SubCode2]->nOptValue;
		int iStunRate = iValue;

		if (lpTargetObj.Type == OBJ_USER)
		{
			iStunRate -= lpTargetObj.m_PlayerData->m_Resistance_Stun;
		}

		if (GetLargeRand() % 1000000 <= iStunRate)
		{
			gObjAddBuffEffect(lpTargetObj, BUFFTYPE_STUN, 0, 0, 0, 0, 2);
			gObjSetPosition(lpTargetObj.m_Index, lpTargetObj.X, lpTargetObj.Y);
		}
	}
	return true;
}

bool CMuunAttack::Attack(CGameObject &Obj, CGameObject lpTargetObj, CMagicInf *lpMagic, int SubCode2)
{
	int skillSuccess = 0;
	CGameObject lpCallObj;
	CGameObject lpCallTargetObj;
	int MsgDamage = 0;
	int ManaChange = 0;
	int iTempShieldDamage = 0;
	int iTotalShieldDamage = 0;

	if ((Obj.Authority & 2) == 2 || (lpTargetObj.Authority & 2) == 2) //s4 add-on
	{
		return FALSE;
	}

	if ((Obj.Authority & 32) == 32 || (lpTargetObj.Authority & 32) == 32) //s4 add-on
	{
		if (gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_INVISABLE) == TRUE)
		{
			return FALSE;
		}

		if (gObjCheckUsedBuffEffect(lpTargetObj, BUFFTYPE_INVISABLE) == TRUE)
		{
			return FALSE;
		}
	}

	if ( Obj.MapNumber != lpTargetObj.MapNumber )
	{
		return FALSE;
	}

	if( lpTargetObj.m_bOffLevel == true && g_OffLevel.m_General.Immortal == 1)
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

	if (g_ConfigRead.EnableAttackBlockInSafeZone == TRUE)
	{
		BYTE btAttr = MapC[Obj.MapNumber].GetAttr(Obj.X, Obj.Y);

		if ((btAttr & 1) == 1)
		{
			return FALSE;
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
			lpCallTargetObj = getGameObject(lpTargetObj.m_RecallMon);
		}
	}

	if ( this->PkCheck(lpCallObj, lpTargetObj) == FALSE )
	{
		return FALSE;
	}

	BOOL bIsOnDuel = gObjDuelCheck(lpObj, lpTargetObj);

	if ( bIsOnDuel )
	{
		Obj.m_iDuelTickCount = GetTickCount();
		lpTargetObj.m_iDuelTickCount = GetTickCount();
	}

	int iOriginTargetDefense = 0;
	int targetdefense = this->GetTargetDefense(lpObj, lpTargetObj, MsgDamage, iOriginTargetDefense);

	if (iOriginTargetDefense > 0)
	{
		targetdefense = iOriginTargetDefense;
	}

	int AttackDamage = this->GetAttackDamage(lpObj, targetdefense, SubCode2, lpTargetObj);

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
		CItemObject * Wing = &lpTargetObj.pntInventory[7];

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
		CItemObject * Dinorant = &lpTargetObj.pntInventory[8];
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
		CItemObject * Darkhorse = &lpTargetObj.pntInventory[8];
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

	if (AttackDamage > 1)
	{
		int nMuunItemEffectValue = 0;

		if (g_CMuunSystem.GetMuunItemValueOfOptType(lpTargetObj, MUUN_DEC_ENEMY_ATTACK_SKILL, &nMuunItemEffectValue, 0) == TRUE)
		{
			AttackDamage = AttackDamage * (100 - nMuunItemEffectValue) / 100;
		}
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
			AttackDamage = AttackDamage * g_ConfigRead.calc.PvPDamageRate[Obj.Class][lpTargetObj.Class] / 100;
		}

		else if (Obj.Type == OBJ_USER && lpTargetObj.Type == OBJ_MONSTER)
		{
			AttackDamage = AttackDamage * g_ConfigRead.calc.PvMDamageRate[Obj.Class] / 100;
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

	BOOL selfdefense = 0;
	lpCallObj = lpTargetObj;
	
	if ( lpTargetObj.Type == OBJ_MONSTER )
	{
		if ( lpTargetObj.m_RecallMon >= 0 )
		{
			lpCallObj = getGameObject(lpTargetObj.m_RecallMon);
		}
	}

	if ( AttackDamage >= 1 )
	{
		if ( Obj.Type == OBJ_USER && lpTargetObj.Type == OBJ_USER )
		{
			if (gObjDuelCheck(lpObj, lpTargetObj))
			{
				selfdefense = 0;
			}

			else if (Obj.m_PlayerData->RegisterdLMS == 1 && lpTargetObj.m_PlayerData->RegisterdLMS == 1)
			{
				if (Obj.MapNumber == g_LastManStanding.m_Cfg.iPVPMap)
				{
					if (g_LastManStanding.m_Rooms[Obj.m_PlayerData->RegisteredLMSRoom].bState == 3)
					{
						selfdefense = 0;
					}
				}
			}

			else if (CC_MAP_RANGE(Obj.MapNumber) || CC_MAP_RANGE(lpTargetObj.MapNumber))
			{
				selfdefense = 0;
			}

			else if (Obj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL || lpTargetObj.MapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
			{
				selfdefense = 0;
			}

			else if (IT_MAP_RANGE(Obj.MapNumber) || IT_MAP_RANGE(lpTargetObj.MapNumber))
			{
				selfdefense = 0;
			}

			else if (g_GensSystem.IsMapBattleZone(Obj.MapNumber) && g_GensSystem.IsMapBattleZone(lpTargetObj.MapNumber))
			{
				selfdefense = 0;
			}

			else if (g_ArcaBattle.IsArcaBattleServer() == TRUE)
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
			if(lpCallObj.PartyNumber >= 0)
			{
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

		gObjLifeCheck(lpTargetObj, lpObj, AttackDamage, 0, 0, MsgDamage, skill, iTotalShieldDamage, 0);
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

int CMuunAttack::GetAttackDamage(CGameObject &Obj, int targetDefense, int SubCode2, CGameObject lpTargetObj)
{
	int ad = 0;
	int AttackDamage = Obj.pntMuunEffectList[SubCode2]->nOptValue * (Obj.m_PlayerData->MasterLevel + Obj.Level) + 10;

	if (lpTargetObj.Type == OBJ_USER)
	{
		ad = AttackDamage / 2 - targetDefense;
	}

	else
	{
		ad = AttackDamage - targetDefense;
	}

	return ad;
}

int CMuunAttack::GetShieldDamage(CGameObject &Obj, CGameObject lpTargetObj, int iAttackDamage)
{
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

CMuunSystem g_CMuunSystem;

CMuunSystem::CMuunSystem()
{

}

CMuunSystem::~CMuunSystem()
{
	if (this->m_MuunItemPeriodData != NULL)
	{
		delete[] this->m_MuunItemPeriodData;
	}
}

void CMuunSystem::Initialize()
{
	if (this->m_MuunItemPeriodData != NULL)
	{
		delete[] this->m_MuunItemPeriodData;
	}

	this->m_MuunItemPeriodData = new MUUN_ITEM_PERIOD_DATA[g_ConfigRead.server.GetObjectMaxUser()];

	for (int i = 0; i<g_ConfigRead.server.GetObjectMaxUser(); i++)
	{
		this->m_MuunItemPeriodData[i].Clear();
	}
}

bool CMuunSystem::LoadScriptMuunSystemInfo(char *lpszFileName)
{
	return this->m_MuunInfoMng.LoadScriptMuunSystemInfo(lpszFileName);
}

bool CMuunSystem::LoadScriptMuunSystemOption(char *lpszFileName)
{
	return this->m_MuunInfoMng.LoadScriptMuunSystemOption(lpszFileName);
}

void CMuunSystem::MuunItemDamage(CGameObject lpObj, int damage)
{
	if (g_ConfigRead.pet.DamageDisableForPet[DAMAGE_OFF_MUUN] == true)
	{
		return;
	}

	if (Obj.Type != OBJ_USER && (double)damage > 0.0)
	{
		return;
	}

	if (damage <= 0)
	{
		return;
	}

	for (int i = 0; i < 2; i++)
	{
		float fdamage = (float)damage;

		if (Obj.pntMuunInventory[i]->IsItem() == TRUE && Obj.pntMuunInventory[i]->m_Durability > 0.0)
		{
			fdamage = fdamage + fdamage;
			fdamage = fdamage / 400.0;
			fdamage = fdamage / 400.0;

			Obj.pntMuunInventory[i]->m_Durability -= fdamage;

			if (Obj.pntMuunInventory[i]->m_Durability != 0.0)
			{
				gGameProtocol.GCMuunItemDurSend(Obj.m_Index, i, Obj.pntMuunInventory[i]->m_Durability);
			}

			if (Obj.pntMuunInventory[i]->m_Durability < 1.0)
			{
				Obj.pntMuunInventory[i]->m_Durability = 0.0;
				this->CalCharacterStat(Obj.m_Index, Obj.pntMuunEffectList[i]->nOptType);
			}
		}
	}
}

BOOL CMuunSystem::MuunItemEquipment(CGameObject &Obj, int iPos, int iSource)
{
	

	if (Obj.Type != OBJ_USER)
	{
		return FALSE;
	}

	if (iPos >= 2)
	{
		if (iSource < 2 && iSource != -1 && Obj.pntMuunInventory[iSource]->IsItem() == FALSE)
		{
			if (!iSource)
			{
				Obj.m_wMuunItem = -1;
			}

			else if (iSource == 1)
			{
				Obj.m_wMuunSubItem = -1;
			}

			this->RemoveUserMuunEffect(lpObj, iSource);
			this->GCSendConditionStatus(Obj.m_Index, iSource, 0);
			gGameProtocol.GCMuunEquipmentChange(Obj.m_Index, iSource);
		}
	}
	else
	{
		if (Obj.pntMuunInventory[iPos]->IsItem() == TRUE)
		{
			WORD nItemNum = Obj.pntMuunInventory[iPos]->m_Type;

			if (iPos == 0)
			{
				Obj.m_wMuunItem = ITEM_GET_INDEX(Obj.pntMuunInventory[iPos]->m_Type) + (((ITEM_GET_TYPE(Obj.pntMuunInventory[iPos]->m_Type)) | (0x10 * Obj.pntMuunInventory[iPos]->m_Level)) << 9);
			}

			else if (iPos == 1)
			{
				Obj.m_wMuunSubItem = ITEM_GET_INDEX(Obj.pntMuunInventory[iPos]->m_Type) + (((ITEM_GET_TYPE(Obj.pntMuunInventory[iPos]->m_Type)) | (0x10 * Obj.pntMuunInventory[iPos]->m_Level)) << 9);
			}

			this->SetUserMuunEffect(lpObj, nItemNum, Obj.pntMuunInventory[iPos]->m_Level, iPos);
			this->CheckEquipMuunItemCondition(lpObj);
		}
	}

	return FALSE;
}

void CMuunSystem::SetMuunItemAddPeriodData(CGameObject lpObj, int iMuunItemNum, UINT64 dwSerial)
{
	CMuunInfo *pCMuunInfo = this->m_MuunInfoMng.GetMuunItemNumToMuunInfo(iMuunItemNum);

	if (!pCMuunInfo)
	{
		sLog->outBasic("[MuunSystem][Error] pCMuunInfo is NULL [%s][%s]", Obj.AccountID, Obj.Name);
		return;
	}

	pCMuunInfo->GetIndex();
	pCMuunInfo->GetAddOptStart();
	pCMuunInfo->GetAddOptEnd();
	pCMuunInfo->GetConditionType();
	pCMuunInfo->GetConditionVal1();
	pCMuunInfo->GetConditionVal2();
	this->AddMuunItmePeriodData(lpObj, iMuunItemNum, dwSerial, 0, pCMuunInfo);
}

bool CMuunSystem::SetUserMuunEffect(CGameObject lpObj, int iMuunItemNum, int iMuunLv, int iEquipPos)
{
	CMuunInfo *pCMuunInfo = m_MuunInfoMng.GetMuunItemNumToMuunInfo(iMuunItemNum);

	if (!pCMuunInfo)
	{
		sLog->outBasic("[MuunSystem][Error] pCMuunInfo is NULL [%s][%s]", Obj.AccountID, Obj.Name);
		return FALSE;
	}

	if (iEquipPos == 0)
	{
		Obj.pntMuunEffectList[0]->nOptType = pCMuunInfo->GetOptType();
		Obj.pntMuunEffectList[0]->nOptValue = pCMuunInfo->GetMuunLvVal(iMuunLv);
		Obj.pntMuunEffectList[0]->nAddOptType = pCMuunInfo->GetAddOptType();
		Obj.pntMuunEffectList[0]->nAddOptValue = pCMuunInfo->GetAddOptVal();
		Obj.pntMuunEffectList[0]->nIndex = pCMuunInfo->GetIndex();
		Obj.pntMuunEffectList[0]->nMuunItemNum = iMuunItemNum;
		Obj.pntMuunEffectList[0]->pCMuunInfo = pCMuunInfo;
		Obj.pntMuunEffectList[0]->bSkillUsed = false;
		Obj.pntMuunEffectList[0]->nSkillDelayTime = pCMuunInfo->GetDelayTime();
		Obj.pntMuunEffectList[0]->nTickTime = pCMuunInfo->GetDelayTime() + GetTickCount();

		this->SetAddOptTypeValue(&Obj.pntMuunEffectList[0]);
	}

	else if (iEquipPos == 1)
	{
		Obj.pntMuunEffectList[1]->nOptType = pCMuunInfo->GetOptType();
		Obj.pntMuunEffectList[1]->nOptValue = pCMuunInfo->GetMuunLvVal(iMuunLv) / 2;
		Obj.pntMuunEffectList[1]->nAddOptType = pCMuunInfo->GetAddOptType();
		Obj.pntMuunEffectList[1]->nAddOptValue = pCMuunInfo->GetAddOptVal();
		Obj.pntMuunEffectList[1]->nIndex = pCMuunInfo->GetIndex();
		Obj.pntMuunEffectList[1]->nMuunItemNum = iMuunItemNum;
		Obj.pntMuunEffectList[1]->pCMuunInfo = pCMuunInfo;
		Obj.pntMuunEffectList[1]->bSkillUsed = false;
		Obj.pntMuunEffectList[1]->nSkillDelayTime = pCMuunInfo->GetDelayTime();
		Obj.pntMuunEffectList[1]->nTickTime = pCMuunInfo->GetDelayTime() + GetTickCount();

		this->SetAddOptTypeValue(&Obj.pntMuunEffectList[1]);
	}

	return TRUE;
}

void CMuunSystem::SetAddOptTypeValue(MUUN_EFFECT_LIST *pUserMuunEffect)
{
	if (pUserMuunEffect->nAddOptType == 1)
	{
		pUserMuunEffect->nTotalVal = pUserMuunEffect->nOptValue * pUserMuunEffect->nAddOptValue;
	}

	else if (pUserMuunEffect->nAddOptType == 2)
	{
		pUserMuunEffect->nTotalVal = pUserMuunEffect->nOptValue + pUserMuunEffect->nAddOptValue;
	}

	else if (pUserMuunEffect->nAddOptType == 3)
	{
		pUserMuunEffect->nTotalVal = pUserMuunEffect->nOptValue;
	}
}

bool CMuunSystem::RemoveUserMuunEffect(CGameObject lpObj, int iEquipPos)
{
	if (iEquipPos >= 2)
	{
		return TRUE;
	}

	CMuunInfo *pCMuunInfo = Obj.pntMuunEffectList[iEquipPos]->pCMuunInfo;

	if (!pCMuunInfo)
	{
		sLog->outBasic("[MuunSystem][Error] pCMuunInfo is NULL [%s][%s]", Obj.AccountID, Obj.Name);
		return FALSE;
	}

	int nOptType = pCMuunInfo->GetOptType();

	Obj.pntMuunEffectList[iEquipPos]->Clear();
	this->CalCharacterStat(Obj.m_Index, nOptType);
}

bool CMuunSystem::GetMuunItemValueOfOptType(CGameObject lpObj, int iMuunOptIndex, int *EffectValue1, int *EffectValue2)
{
	if (Obj.Type != OBJ_USER)
	{
		return false;
	}

	*EffectValue1 = 0;

	for (int i = 0; i < MAX_MUUN_EFFECT_LIST; i++)
	{
		if (Obj.pntMuunInventory[i]->IsItem() == FALSE)
		{
			continue;
		}

		if (iMuunOptIndex != Obj.pntMuunEffectList[i]->nOptType)
		{
			continue;
		}

		if (Obj.pntMuunEffectList[i]->bOptEnable == FALSE)
		{
			continue;
		}

		if (Obj.pntMuunInventory[i]->m_Durability <= 0.0)
		{
			continue;
		}

		if (Obj.pntMuunInventory[i]->IsMuunItemPeriodExpire())
		{
			*EffectValue1 += Obj.pntMuunEffectList[i]->nOptValue;
		}

		else
		{
			*EffectValue1 += Obj.pntMuunEffectList[i]->nTotalVal;
		}

		if (iMuunOptIndex == 51)
		{
			if (Obj.pntMuunEffectList[i]->bSkillUsed == true)
			{
				return false;
			}

			Obj.pntMuunEffectList[i]->bSkillUsed = true;

			PMSG_MUUN_ATTACK_COMMAND pMsg;

			pMsg.Slot = i;
			pMsg.SkillType = 51;
			pMsg.NumberH = HIBYTE(Obj.m_Index);
			pMsg.NumberL = LOBYTE(Obj.m_Index);
			pMsg.TargetNumberH = HIBYTE(Obj.m_Index);
			pMsg.TargetNumberL = LOBYTE(Obj.m_Index);

			PHeadSubSetB((BYTE*)&pMsg, 0x4E, 0x12, sizeof(pMsg));

			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
			gGameProtocol.MsgSendV2(lpObj, (BYTE*)&pMsg, pMsg.h.size);
		}

		return true;
	}

	return false;
}

void CMuunSystem::GDReqLoadMuunInvenItem(CGameObject &obj)
{
	SDHP_REQ_DBMUUN_INVEN_LOAD pMsg;

	if (!ObjectMaxRange(obj->m_Index))
	{
		return;
	}

	if (!gObjIsConnected(obj->m_Index))
	{
		return;
	}
			
	std::memcpy(pMsg.AccountID, obj->AccountID, MAX_ACCOUNT_LEN + 1);
	std::memcpy(pMsg.Name, obj->Name, MAX_ACCOUNT_LEN + 1);
	pMsg.Obj.m_Index = obj->m_Index;

	pMsg.h.c = 0xC1;
	pMsg.h.size = sizeof(pMsg);
	pMsg.h.headcode = 0xF1;
	
	wsDataCli.DataSend((char*)&pMsg, pMsg.h.size);
}

void CMuunSystem::DGLoadMuunInvenItem(SDHP_ANS_DBMUUN_INVEN_LOAD *lpMsg)
{
	CGameObject &Obj = lpMsg->Obj.m_Index;
	char szId[11];
	szId[MAX_ACCOUNT_LEN] = 0;
	std::memcpy(szId, Obj.AccountID, MAX_ACCOUNT_LEN);

	if (ObjectMaxRange(Obj.m_Index) == false)
	{
		sLog->outBasic("[DEBUG MUUN] Wrong Obj.m_Index (RECV)");
		return;
	}

	if (Obj.m_bMapSvrMoveReq == true)
	{
		sLog->outBasic("[DGLoadMuunInvenItem] MapServerMove User.Can't Open Event Inven. return!! [%s], IP [%s] ", Obj.AccountID, Obj.m_PlayerData->ConnectUser->IP);
		return;
	}

	if (Obj.m_State == 32)
	{
		sLog->outBasic("[DGLoadMuunInvenItem] MapServerMove User.Can't Open Event Inven. return!! [%s], IP [%s] ", Obj.AccountID, Obj.m_PlayerData->ConnectUser->IP);
		return;
	}

	if (Obj.m_bMapSvrMoveQuit == 1)
	{
		sLog->outBasic("[DGLoadMuunInvenItem] MapServerMove User.Can't Open Event Inven. return!! [%s], IP [%s] ", Obj.AccountID, Obj.m_PlayerData->ConnectUser->IP);
		return;
	}

	if (!gObjIsAccontConnect(Obj.m_Index, szId))
	{
		sLog->outBasic("error-L1 : Request to receive Warehouse information doesn't match the user. [%s][%d]", szId, Obj.m_Index);
		return;
	}

	Obj.bMuunInventoryLoad = true;
	

	int itype;
	int _type;
	BYTE OptionData;
	CItemObject &item;
	int nMuunInvenItemCount = 0;

	for (int n = 0; n < MUUN_INVENTORY_SIZE; n++)
	{
		itype = lpMsg->dbItems[MAX_DBITEM_INFO * n];
		itype |= (lpMsg->dbItems[MAX_DBITEM_INFO * n + 9] & 0xF0) << 5;
		itype |= (lpMsg->dbItems[MAX_DBITEM_INFO * n + 7] & 0x80) << 1;
		_type = itype;

		Obj.pntMuunInventory[n]->Clear();

		if (lpMsg->dbItems[MAX_DBITEM_INFO * n] == (BYTE)-1 && (lpMsg->dbItems[MAX_DBITEM_INFO * n + 9] & 0xF0) == 0xF0 && lpMsg->dbItems[MAX_DBITEM_INFO * n + 7] & 0x80)
		{
			itype = -1;
		}

		if (IsItem(_type) == FALSE)
		{
			itype = -1;
		}

		if (itype != -1)
		{
			item.m_Level = DBI_GET_LEVEL(lpMsg->dbItems[MAX_DBITEM_INFO * n + 1]);
			OptionData = lpMsg->dbItems[n*MAX_DBITEM_INFO + 1];
			item.m_Option1 = DBI_GET_SKILL(OptionData);
			item.m_Option2 = DBI_GET_LUCK(OptionData);
			item.m_Option3 = DBI_GET_OPTION(OptionData);

			if (_type == ITEMGET(13, 3))
			{
				item.m_Option3 |= (lpMsg->dbItems[MAX_DBITEM_INFO * n + DBI_NOPTION_DATA] & 0x40) >> 4;
			}

			item.m_Durability = lpMsg->dbItems[MAX_DBITEM_INFO * n + DBI_DUR];
			item.m_JewelOfHarmonyOption = lpMsg->dbItems[MAX_DBITEM_INFO * n + DBI_JOH_DATA];
			item.m_ItemOptionEx = DBI_GET_380OPTION(lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_OPTION380_DATA]);

			if (item.m_ItemOptionEx != 0)
			{
				item.m_Type = itype;
				if (!g_kItemSystemFor380.Is380Item(&item))
				{
					item.m_ItemOptionEx = 0;
					sLog->outBasic("[380Item][%s][%s] Invalid 380 Item Option in Muun Inventory pos[%d]", Obj.AccountID, Obj.Name, n);
				}
			}

			BYTE SocketOption[5];
			BYTE SocketOptionIndex = 0xFF;
			memset(&SocketOption, 0xFF, 5);

			if (gSocketItemType.CheckSocketItemType(itype) == 1)
			{
				SocketOptionIndex = lpMsg->dbItems[MAX_DBITEM_INFO * n + DBI_JOH_DATA];

				for (int iSocketSlotIndex = 0; iSocketSlotIndex < 5; iSocketSlotIndex++)
				{
					SocketOption[iSocketSlotIndex] = lpMsg->dbItems[((n*MAX_DBITEM_INFO) + DBI_SOCKET_1) + iSocketSlotIndex];
				}
			}

			else if (g_PentagramSystem.IsPentagramItem(itype) == TRUE ||
				g_PentagramSystem.IsPentagramJewel(itype) == TRUE)
			{
				SocketOptionIndex = lpMsg->dbItems[MAX_DBITEM_INFO * n + DBI_JOH_DATA];

				for (int iSocketSlotIndex = 0; iSocketSlotIndex < 5; iSocketSlotIndex++)
				{
					SocketOption[iSocketSlotIndex] = lpMsg->dbItems[((n*MAX_DBITEM_INFO) + DBI_SOCKET_1) + iSocketSlotIndex];
				}
			}

			else if (this->IsMuunItem(itype) == TRUE)
			{
				SocketOptionIndex = lpMsg->dbItems[MAX_DBITEM_INFO * n + DBI_JOH_DATA];

				for (int iSocketSlotIndex = 0; iSocketSlotIndex < 5; iSocketSlotIndex++)
				{
					SocketOption[iSocketSlotIndex] = lpMsg->dbItems[((n*MAX_DBITEM_INFO) + DBI_SOCKET_1) + iSocketSlotIndex];
				}
			}

			else if (this->IsStoneofEvolution(itype) == TRUE)
			{
				SocketOptionIndex = 0;

				for (int iSocketSlotIndex = 0; iSocketSlotIndex < 5; iSocketSlotIndex++)
				{
					SocketOption[iSocketSlotIndex] = lpMsg->dbItems[((n*MAX_DBITEM_INFO) + DBI_SOCKET_1) + iSocketSlotIndex];
				}
			}

			else
			{
				SocketOptionIndex = 0;

				for (int iSocketSlotIndex = 0; iSocketSlotIndex < 5; ++iSocketSlotIndex)
				{
					SocketOption[iSocketSlotIndex] = lpMsg->dbItems[((n*MAX_DBITEM_INFO) + DBI_SOCKET_1) + iSocketSlotIndex];

					if (SocketOption[iSocketSlotIndex] == 0)
					{
						SocketOption[iSocketSlotIndex] = 0xFF;
					}
				}
			}

			item.m_PeriodItemOption = (lpMsg->dbItems[MAX_DBITEM_INFO * n + 9] & 6) >> 1;
			item.Convert(itype, item.m_Option1, item.m_Option2, item.m_Option3, DBI_GET_NOPTION(lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_NOPTION_DATA]), lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SOPTION_DATA], item.m_ItemOptionEx, SocketOption, SocketOptionIndex, item.m_PeriodItemOption, 3);

			Obj.pntMuunInventory[n]->m_Option1 = item.m_Option1;
			Obj.pntMuunInventory[n]->m_Option2 = item.m_Option2;
			Obj.pntMuunInventory[n]->m_Option3 = item.m_Option3;
			Obj.pntMuunInventory[n]->m_JewelOfHarmonyOption = item.m_JewelOfHarmonyOption;
			Obj.pntMuunInventory[n]->m_ItemOptionEx = item.m_ItemOptionEx;

			DWORD hidword = MAKE_NUMBERDW(MAKE_NUMBERW(lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL1], lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL2]), MAKE_NUMBERW(lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL3], lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL4]));
			DWORD lodword = MAKE_NUMBERDW(MAKE_NUMBERW(lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL5], lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL6]), MAKE_NUMBERW(lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL7], lpMsg->dbItems[n*MAX_DBITEM_INFO + DBI_SERIAL8]));
			item.m_Number = MAKEQWORD(hidword, lodword);

			if (this->IsMuunItem(&item) == TRUE)
			{
				if (item.IsMuunItemPeriodExpire() == FALSE)
				{
					CMuunInfo *	pCMuunInfo = this->m_MuunInfoMng.GetMuunItemNumToMuunInfo(itype);

					if (pCMuunInfo == NULL)
					{
						sLog->outBasic("[MuunSystem][Error] [%s][%s] DGLoadMuunInvenItem() pCMuunInfo is NULL %d", Obj.AccountID, Obj.Name, __LINE__);
					}

					else
					{
						if (this->CheckAddOptionExpireDate(pCMuunInfo->GetAddOptStart(), pCMuunInfo->GetAddOptEnd()) == TRUE)
						{
							item.SetMuunItemPeriodExpire();
						}

						else
						{
							this->SetMuunItemAddPeriodData(lpObj, itype, item.m_Number);
						}
					}
				}

				else
				{
					CMuunInfo *	pCMuunInfo = this->m_MuunInfoMng.GetMuunItemNumToMuunInfo(itype);

					if (pCMuunInfo == NULL)
					{
						sLog->outBasic("[MuunSystem][Error] [%s][%s] DGLoadMuunInvenItem() pCMuunInfo is NULL %d", Obj.AccountID, Obj.Name, __LINE__);
					}

					else
					{
						if (this->CheckAddOptionExpireDate(pCMuunInfo->GetAddOptStart(), pCMuunInfo->GetAddOptEnd()) == FALSE)
						{
							item.SetMuunItemPeriodReset();
							this->SetMuunItemAddPeriodData(lpObj, itype, item.m_Number);

							sLog->outBasic("[MuunSystem][MuunItemPeriodReset] [%s][%s] MuunItemPeriodReset() Type:[%d] serial:[%I64d]",
								Obj.AccountID, Obj.Name, itype, item.m_Number);
						}
					}
				}
			}

			gObjMuunInventoryInsertItemPos(Obj.m_Index, item, n);

			if (n < 2)
			{
				this->MuunItemEquipment(Obj.m_Index, n, -1);

				if (!n)
				{
					gGameProtocol.GCMuunEquipmentChange(Obj.m_Index, 0);
				}
			}

			nMuunInvenItemCount++;
		}
	}

	for (int i = 0; i < MUUN_INVENTORY_SIZE; i++)
	{
		if (Obj.pntMuunInventory[i]->IsItem() == TRUE)
		{
			if (this->IsStoneofEvolution(Obj.pntMuunInventory[i]->m_Type) == TRUE)
			{
				WORD wPetItemNumber = (Obj.pntMuunInventory[i]->m_SocketOption[0] << 8) | Obj.pntMuunInventory[i]->m_SocketOption[1];
				LPITEM_ATTRIBUTE lpItemAttribute = GetItemAttr(wPetItemNumber);

				if (lpItemAttribute != NULL)
				{
					BYTE NewOption[MAX_EXOPTION_SIZE];
					ItemIsBufExOption(NewOption, &Obj.pntMuunInventory[i]);

				}

				else
				{
					sLog->outBasic("[MuunSystem][Error] [%s][%s] DGLoadMuunInvenItem() pCMuunInfo is NULL %d", Obj.AccountID, Obj.Name, __LINE__);
				}
			}

			else
			{
				BYTE NewOption[MAX_EXOPTION_SIZE];
				ItemIsBufExOption(NewOption, &Obj.pntMuunInventory[i]);
			}
		}
	}

	GCMuunInventoryItemListSend(Obj.m_Index);

	Obj.dwCheckMuunItemTime = GetTickCount();
}
std::string hexStr(BYTE *data, int len)
{
	std::stringstream ss;
	ss << std::hex;
	for (int i(0); i<len; ++i)
		ss << (int)data[i];
	return ss.str();
}void CMuunSystem::GDReqSaveMuunInvenItem(CGameObject &Obj)
{
	if (Obj.bMuunInventoryLoad == false)
	{
		return;
	}

	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	if (!gObjIsConnected(Obj.m_Index))
	{
		return;
	}

	for (int n = 0; n < MUUN_INVENTORY_SIZE; n++)
	{
		if (Obj.pntMuunInventory[n]->IsItem() == FALSE)
		{
			continue;
		}

		if (this->IsStoneofEvolution(Obj.pntMuunInventory[n]->m_Type) == TRUE)
		{
			WORD wPetItemNumber = (Obj.pntMuunInventory[n]->m_SocketOption[0] << 8) | Obj.pntMuunInventory[n]->m_SocketOption[1];

			LPITEM_ATTRIBUTE pItemAttribute = GetItemAttr(wPetItemNumber);

			if (!pItemAttribute)
			{
				sLog->outBasic("[MuunSystem][Error][%s][%s] GDReqSaveMuunInvenItem() pItemAttribute is NULL", Obj.AccountID, Obj.Name);
			}

			else
			{
				BYTE NewOption[MAX_EXOPTION_SIZE];
				ItemIsBufExOption(NewOption, &Obj.pntMuunInventory[n]);

			}
		}

		else
		{
			BYTE NewOption[MAX_EXOPTION_SIZE];
			ItemIsBufExOption(NewOption, &Obj.pntMuunInventory[n]);
	
		}
	}

	SDHP_REQ_DBMUUN_INVEN_SAVE pMsg;
	std::memcpy(pMsg.AccountID, Obj.AccountID, MAX_ACCOUNT_LEN+1);
	std::memcpy(pMsg.Name, Obj.Name, MAX_ACCOUNT_LEN+1);

	pMsg.h.c = 0xC2;
	pMsg.h.sizeH = SET_NUMBERH(sizeof(pMsg));
	pMsg.h.sizeL = SET_NUMBERL(sizeof(pMsg));
	pMsg.h.headcode = 0xF2;
	ItemByteConvert32((BYTE*)pMsg.dbInventory, Obj.pMuunInventory, MUUN_INVENTORY_SIZE);

	int i;
	char* buf_str = (char*)malloc(2 * MUUN_INVENTORY_SIZE + 1);
	char* buf_ptr = buf_str;
	for (i = 0; i < MUUN_INVENTORY_SIZE; i++)
	{
		buf_ptr += sprintf(buf_ptr, "%02X", (BYTE)pMsg.dbInventory[i]);
	}
	//sLog->outBasic("[K2] Muun Obj.pntMuunInventory[i]->m_Type %s", buf_ptr);
	wsDataCli.DataSend((char*)&pMsg, sizeof(pMsg));
}

void CMuunSystem::GCSendConditionStatus(CGameObject &Obj, int iPos, int iStatus)
{
	PMSG_MUUNITEM_CONDITION_STATUS pMsg;

	pMsg.btStatus = iStatus;
	pMsg.btIPos = iPos;
	PHeadSubSetB((BYTE*)&pMsg, 0x4E, 0x07, sizeof(pMsg));

	this->MsgIsMuunItemActive(getGameObject(Obj.m_Index), iPos);
	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}

bool CMuunSystem::IsMuunItem(CItemObject *pCItemObject)
{
	if (!pCItemObject)
	{
		return false;
	}

	LPITEM_ATTRIBUTE pItemAttribute = GetItemAttr(pCItemObject->m_Type);

	if (!pItemAttribute)
	{
		return false;
	}
	
	return pItemAttribute->ItemKindA == 12 && (pItemAttribute->ItemKindB >= 63 && pItemAttribute->ItemKindB <= 66);
}

bool CMuunSystem::IsMuunUtil(int ItemIndex) // OK
{
	if (ItemIndex == ITEMGET(13, 211) || ItemIndex == ITEMGET(13, 239) || ItemIndex == ITEMGET(13, 240))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool CMuunSystem::IsMuunItem(int iItemNum)
{
	LPITEM_ATTRIBUTE pItemAttribute = GetItemAttr(iItemNum);

	if (!pItemAttribute)
	{
		return false;
	}

	return pItemAttribute->ItemKindA == 12 && (pItemAttribute->ItemKindB >= 63 && pItemAttribute->ItemKindB <= 66);
}

bool CMuunSystem::IsStoneofEvolution(CItemObject *pCItemObject)
{
	if (!pCItemObject)
	{
		return false;
	}

	LPITEM_ATTRIBUTE pItemAttribute = GetItemAttr(pCItemObject->m_Type);

	if (!pItemAttribute)
	{
		return false;
	}

	return pItemAttribute->ItemKindA == 12 && pItemAttribute->ItemKindB == 0;
}

bool CMuunSystem::IsStoneofEvolution(int iItemNum)
{
	LPITEM_ATTRIBUTE pItemAttribute = GetItemAttr(iItemNum);

	if (!pItemAttribute)
	{
		return false;
	}

	return pItemAttribute->ItemKindA == 12 && pItemAttribute->ItemKindB == 0;
}

int CMuunSystem::GetEvolutionMuunItemIndex(int iItemNum)
{
	CMuunInfo *pCMuunInfo = this->m_MuunInfoMng.GetMuunItemNumToMuunInfo(iItemNum);

	if (!pCMuunInfo)
	{
		sLog->outBasic("[MuunSystem][Error]GetEvolutionMuunItemIndex() pItemAttribute is NULL %d", __LINE__);
		return 0;
	}

	return pCMuunInfo->GetEvolutionMuunNum();
}

int CMuunSystem::GetBeforeEvolutionMuunItemIndex(int iItemNum)
{
	return this->m_MuunInfoMng.GetBeforeEvolutionMuunItemIndex(iItemNum);
}

int CMuunSystem::GetMuunRankOfMuunInfo(int iItemNum)
{
	CMuunInfo *pCMuunInfo = this->m_MuunInfoMng.GetMuunItemNumToMuunInfo(iItemNum);

	if (!pCMuunInfo)
	{
		sLog->outBasic("[MuunSystem][Error] GetMuunRankOfMuunInfo() pCMuunInfo is NULL %d", __LINE__);
		return 0;
	}

	return pCMuunInfo->GetMuunRank();
}

void CMuunSystem::CGMuunInventoryUseItemRecv(PMSG_USEITEM_MUUN_INVEN *lpMsg, CGameObject &Obj)
{
	int iItemUseType = lpMsg->btItemUseType;

	if (Obj.m_IfState->use == 1)
	{
		sLog->outBasic("[%s][%s]_If return  %d", Obj.AccountID, Obj.Name, __LINE__);
		this->GCMuunInventoryUseItemResult(Obj.m_Index, iItemUseType, 1);

		return;
	}

	if (Obj.CloseType != -1)
	{
		sLog->outBasic("[%s][%s] CGMuunInventoryUseItemRecv()_CloseType return %d", Obj.AccountID, Obj.Name, __LINE__);
		this->GCMuunInventoryUseItemResult(Obj.m_Index, iItemUseType, 1);

		return;
	}

	if (!::gObjFixMuunInventoryPointer(Obj.m_Index))
	{
		sLog->outBasic("[Fix Inv.Ptr] False Location - %d", __LINE__);
	}

	if (Obj.pTransaction == 1)
	{
		sLog->outBasic("[%s][%s] CGUseItemRecv() Failed : Transaction == 1, IF_TYPE : %d", Obj.AccountID, Obj.Name, Obj.m_IfState->type);
		this->GCMuunInventoryUseItemResult(Obj.m_Index, iItemUseType, 1);

		return;
	}

	if (g_NewPVP.IsObserver(getGameObject(Obj.m_Index)))
	{
		gGameProtocol.GCServerMsgStringSend(Lang.GetText(0,562), Obj.m_Index, 1);
		this->GCMuunInventoryUseItemResult(Obj.m_Index, iItemUseType, 1);

		return;
	}

	if (lpMsg->inventoryPos == lpMsg->invenrotyTarget)
	{
		sLog->outBasic("error-L1 : [%s][%s] CGMuunInventoryUseItemRecv()_Pos return %d", Obj.AccountID, Obj.Name, __LINE__);
		this->GCMuunInventoryUseItemResult(Obj.m_Index, iItemUseType, 1);

		return;
	}

	switch (iItemUseType)
	{
		case 1:
			if (this->MuunItemLevelUp(getGameObject(Obj.m_Index), lpMsg->inventoryPos, lpMsg->invenrotyTarget) == FALSE)
			{
				sLog->outBasic("[MuunSystem][LevelUp] [%s][%s] [Fail]", Obj.AccountID, Obj.Name);
				this->GCMuunInventoryUseItemResult(Obj.m_Index, iItemUseType, 1);

				return;
			}

			this->ClearPeriodMuunItemData(getGameObject(Obj.m_Index), Obj.pntMuunInventory[lpMsg->inventoryPos]->m_Type, Obj.pntMuunInventory[lpMsg->inventoryPos)->m_Number);
			Obj.pntMuunInventory[lpMsg->inventoryPos].Clear();
			gGameProtocol.GCMuunInventoryItemDeleteSend(Obj.m_Index, lpMsg->inventoryPos, 1);
			gGameProtocol.GCMuunInventoryItemOneSend(Obj.m_Index, lpMsg->invenrotyTarget);
			break;

		case 2:
			if (this->MuunItemEvolution(getGameObject(Obj.m_Index), lpMsg->inventoryPos, lpMsg->invenrotyTarget) == FALSE)
			{
				sLog->outBasic("[MuunSystem] [%s][%s]Item Use Muun Item Evolution Fail", Obj.AccountID, Obj.Name);
				this->GCMuunInventoryUseItemResult(Obj.m_Index, iItemUseType, 1);

				return;
			}

			Obj.pntMuunInventory[lpMsg->inventoryPos].Clear();
			gGameProtocol.GCMuunInventoryItemDeleteSend(Obj.m_Index, lpMsg->inventoryPos, 1);
			break;

		case 3:
			if (this->MuunItemLifeGem(getGameObject(Obj.m_Index), lpMsg->inventoryPos, lpMsg->invenrotyTarget) == FALSE)
			{
				sLog->outBasic("[MuunSystem] [%s][%s]Item Use Muun Item LifeGem Fail", Obj.AccountID, Obj.Name);
				this->GCMuunInventoryUseItemResult(Obj.m_Index, iItemUseType, 1);

				return;
			}

			gObjInventoryItemSet(Obj.m_Index, lpMsg->inventoryPos, -1);
			Obj.pntInventory[lpMsg->inventoryPos]->Clear();
			gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, lpMsg->inventoryPos, 1);
			gGameProtocol.GCMuunItemDurSend(Obj.m_Index, lpMsg->invenrotyTarget, -1);
			break;

		case 4:
			if (this->MuunItemEnergyGenerator(getGameObject(Obj.m_Index), lpMsg->inventoryPos, lpMsg->invenrotyTarget) == FALSE)
			{
				sLog->outBasic("[MuunSystem] [%s][%s]Item Use Muun Item Energy Generator Fail", Obj.AccountID, Obj.Name);
				this->GCMuunInventoryUseItemResult(Obj.m_Index, iItemUseType, 1);

				return;
			}

			this->ClearPeriodMuunItemData(getGameObject(Obj.m_Index), Obj.pntMuunInventory[lpMsg->inventoryPos]->m_Type, Obj.pntMuunInventory[lpMsg->inventoryPos)->m_Number);
			Obj.pntMuunInventory[lpMsg->inventoryPos].Clear();
			gGameProtocol.GCMuunInventoryItemDeleteSend(Obj.m_Index, lpMsg->inventoryPos, 1);
			gGameProtocol.GCMuunInventoryItemOneSend(Obj.m_Index, lpMsg->invenrotyTarget);
			break;
	}

	this->GCMuunInventoryUseItemResult(Obj.m_Index, iItemUseType, 0);
}
			

bool CMuunSystem::MuunItemEvolution(CGameObject lpObj, int source, int target)
{
	if (source < 2 || source > MUUN_INVENTORY_SIZE - 1)
	{
		return false;
	}

	if (target < 2 || target > MUUN_INVENTORY_SIZE - 1)
	{
		return false;
	}

	if (Obj.pntMuunInventory[source]->IsItem() == FALSE)
	{
		return false;
	}

	if (Obj.pntMuunInventory[target]->IsItem() == FALSE)
	{
		return false;
	}

	if (this->IsStoneofEvolution(Obj.pntMuunInventory[source]->m_Type) == FALSE)
	{
		return false;
	}

	if (this->IsMuunItem(Obj.pntMuunInventory[target]->m_Type) == FALSE)
	{
		return false;
	}

	if (Obj.pntMuunInventory[target]->m_Level == 0)
	{
		return false;
	}

	WORD nEvoMuunItemNum = (Obj.pntMuunInventory[source]->m_SocketOption[0] << 8) | Obj.pntMuunInventory[source]->m_SocketOption[1];

	if (nEvoMuunItemNum != Obj.pntMuunInventory[target]->m_Type)
	{
		return false;
	}

	if (Obj.pntMuunInventory[target]->GetMuunItemRank() + 1 != Obj.pntMuunInventory[target]->m_Level)
	{
		return false;
	}

	if (Obj.pntMuunInventory[target]->GetMuunItemRank() + 1 < Obj.pntMuunInventory[target]->m_Level)
	{
		sLog->outBasic("[MuunSystem][Error][%s][%s] Over Level(%d) %d", Obj.AccountID, Obj.Name, Obj.pntMuunInventory[target]->m_Level, __LINE__);
		return false;
	}

	WORD nEvolutionMuunNum = this->GetEvolutionMuunItemIndex(Obj.pntMuunInventory[target]->m_Type);

	if (nEvolutionMuunNum == 0)
	{
		sLog->outBasic("[MuunSystem][Error][%s][%s] Not Evolution MuunItem Index - target Item Index(%d) %d", Obj.AccountID, Obj.Name, Obj.pntMuunInventory[target]->m_Type, __LINE__);
		return false;
	}

	LPITEM_ATTRIBUTE pSItemAttribute = GetItemAttr(Obj.pntMuunInventory[source]->m_Type);
	LPITEM_ATTRIBUTE pTItemAttribute = GetItemAttr(Obj.pntMuunInventory[target]->m_Type);

	if (pTItemAttribute && pSItemAttribute)
	{
		UINT64 serial = Obj.pntMuunInventory[target]->m_Number;
		WORD Level = Obj.pntMuunInventory[target]->m_Level;
		int iMuunRank = Obj.pntMuunInventory[target]->GetMuunItemRank();

		sLog->outBasic("[MuunSystem][Evolution] [%s][%s] [Success] Source: [%s] Pos[%d] Serial[%I64d] - Target: [%s] Pos[%d] Rank[%d] Level[%d] Serial[%I64d]",
			Obj.AccountID, Obj.Name, pSItemAttribute->Name, source, Obj.pntMuunInventory[source]->m_Number, pTItemAttribute->Name, target, iMuunRank, Level, serial);
	}

	float fDur = Obj.pntMuunInventory[target]->m_Durability;

	this->ClearPeriodMuunItemData(lpObj, Obj.pntMuunInventory[target]->m_Type, Obj.pntMuunInventory[target]->m_Number);
	gGameProtocol.GCMuunInventoryItemDeleteSend(Obj.m_Index, target, 1);

	Obj.pntMuunInventory[target]->Clear();

	BYTE SocketOption[5];
	memset(SocketOption, -1, sizeof(SocketOption));

	ItemCreate(Obj.m_Index, 0xE0, 0, 0, nEvolutionMuunNum, 0, fDur, 0, 0, 0, Obj.m_Index, 0, 0, 0, SocketOption, 0);

	return true;
}

bool CMuunSystem::MuunItemLevelUp(CGameObject lpObj, int source, int target)
{
	if (source < 2 || source > MUUN_INVENTORY_SIZE - 1)
	{
		return false;
	}

	if (target < 2 || target > MUUN_INVENTORY_SIZE - 1)
	{
		return false;
	}

	if (Obj.pntMuunInventory[source]->IsItem() == FALSE)
	{
		return false;
	}

	if (Obj.pntMuunInventory[target]->IsItem() == FALSE)
	{
		return false;
	}

	if (this->IsMuunItem(Obj.pntMuunInventory[target]->m_Type) == FALSE)
	{
		return false;
	}
	
	if (this->IsMuunItem(Obj.pntMuunInventory[source]->m_Type) == FALSE)
	{
		return false;
	}

	if (Obj.pntMuunInventory[source]->m_Level == 0 || Obj.pntMuunInventory[target]->m_Level == 0)
	{
		return false;
	}
							
	if (Obj.pntMuunInventory[source]->m_Type != Obj.pntMuunInventory[target]->m_Type)
	{
		return false;
	}
	
	if (Obj.pntMuunInventory[source]->m_Level != 1)
	{
		return false;
	}

	if (Obj.pntMuunInventory[target]->GetMuunItemRank() + 1 == Obj.pntMuunInventory[target]->m_Level)
	{
		return false;
	}
										
	if (Obj.pntMuunInventory[target]->m_Level >= 5)
	{
		return false;
	}

	if (Obj.pntMuunInventory[target]->GetMuunItemRank() + 1 < Obj.pntMuunInventory[target]->m_Level)
	{
		sLog->outBasic("[MuunSystem][Error][%s][%s] Over Level(%d) %d", Obj.AccountID, Obj.Name, Obj.pntMuunInventory[target]->m_Level, __LINE__);
		return false;
	}

	int nBeforeLv = Obj.pntMuunInventory[target]->m_Level;
	Obj.pntMuunInventory[target]->m_Level++;

	LPITEM_ATTRIBUTE pSItemAttribute = GetItemAttr(Obj.pntMuunInventory[source]->m_Type);
	LPITEM_ATTRIBUTE pTItemAttribute = GetItemAttr(Obj.pntMuunInventory[target]->m_Type);
												
	if (pTItemAttribute && pSItemAttribute)
	{
		sLog->outBasic("[MuunSystem][LevelUp] [%s][%s] [Success] Source: [%s] Pos[%d] Rank[%d] Level[%d] Serial[%I64d] - Target: [%s] Pos[%d] Rank[%d] Level[%d]/[%d] Serial[%I64d]",
			Obj.AccountID, Obj.Name, pSItemAttribute->Name, source, Obj.pntMuunInventory[source]->GetMuunItemRank(), Obj.pntMuunInventory[source]->m_Level, Obj.pntMuunInventory[source]->m_Number,
			pTItemAttribute->Name, target, Obj.pntMuunInventory[target]->GetMuunItemRank(), nBeforeLv, Obj.pntMuunInventory[target]->m_Level, Obj.pntMuunInventory[target]->m_Number);
	}

	return true;
}

bool CMuunSystem::MuunItemLifeGem(CGameObject lpObj, int source, int target)
{
	if (source < INVETORY_WEAR_SIZE || source > MAIN_INVENTORY_SIZE - 1)
	{
		return false;
	}

	if (target < 2 || target > MUUN_INVENTORY_SIZE - 1)
	{
		return false;
	}

	if (Obj.pntInventory[source]->IsItem() == FALSE)
	{
		return false;
	}

	if (Obj.pntMuunInventory[target]->IsItem() == FALSE)
	{
		return false;
	}

	if (this->IsMuunItem(Obj.pntMuunInventory[target]->m_Type) == FALSE)
	{
		return false;
	}
	
	if (Obj.pntInventory[source]->m_Type != ITEMGET(14, 16))
	{
		return false;
	}

	if (Obj.pntMuunInventory[target]->m_Durability >= 255.0)
	{
		return false;
	}

	int nBeforeDur = Obj.pntMuunInventory[target]->m_Durability;
	Obj.pntMuunInventory[target]->m_Durability = 255.0;

	LPITEM_ATTRIBUTE pItemAttribute = GetItemAttr(Obj.pntInventory[source]->m_Type);
							
	if (pItemAttribute)
	{
		sLog->outBasic("[MuunSystem] [%s][%s] Item Use Muun Item LifeGem Success target[%d] [%s] Dur:[%d]/[%d] Serial:%I64d",
			Obj.AccountID, Obj.Name, target, pItemAttribute->Name, nBeforeDur, (int)Obj.pntMuunInventory[target]->m_Durability, Obj.pntMuunInventory[target]->m_Number);
	}

	return true;
}

bool CMuunSystem::MuunItemEnergyGenerator(CGameObject &Obj, int source, int target)
{
	if (source < 2 || source > MUUN_INVENTORY_SIZE - 1)
	{
		return false;
	}

	if (target < 2 || target > MUUN_INVENTORY_SIZE - 1)
	{
		return false;
	}

	if (Obj.pntMuunInventory[source]->IsItem() == FALSE)
	{
		return false;
	}

	if (Obj.pntMuunInventory[target]->IsItem() == FALSE)
	{
		return false;
	}

	int nEnergy = 0;
	int nRankEnergy = 0;
	int nLvEnergy = 0;

	if (this->IsStoneofEvolution(Obj.pntMuunInventory[source]->m_Type) == FALSE &&
		this->IsMuunItem(Obj.pntMuunInventory[source]->m_Type) == FALSE &&
		Obj.pntMuunInventory[source]->m_Type != ITEMGET(13,239))
	{
		return false;
	}

	if (Obj.pntMuunInventory[target]->m_Type != ITEMGET(13,239))
	{
		return false;
	}

	int nMuunRank = Obj.pntMuunInventory[source]->GetMuunItemRank();
	int nMuunLv = Obj.pntMuunInventory[source]->m_Level;

	if (this->IsMuunItem(Obj.pntMuunInventory[source]->m_Type) == TRUE)
	{
		for (int i = 0; i < 8; i++)
		{
			if ( nMuunRank == this->m_stEnergyGeneratorPoint[i].iRank )
			{
				nRankEnergy = this->m_stEnergyGeneratorPoint[i].iRankPt;
			}

			if ( nMuunLv == this->m_stEnergyGeneratorPoint[i].iMuunLv )
			{
				nLvEnergy = this->m_stEnergyGeneratorPoint[i].iMuunLvPt;
			}
		}

		if (nMuunLv != 0)
		{
			nEnergy = nLvEnergy * nRankEnergy;
		}

		else
		{
			nEnergy = nLvEnergy * (nRankEnergy + nRankEnergy * nRankEnergy);
		}
	}

	else if (this->IsStoneofEvolution(Obj.pntMuunInventory[source]->m_Type) == TRUE)
	{
		nEnergy = this->m_iStoneOfEvolutionPt;
	}

	else if (Obj.pntMuunInventory[source]->m_Type == ITEMGET(13,239))
	{
		nEnergy = Obj.pntMuunInventory[source]->m_Durability;
	}

	int nMuunDurability = nEnergy + Obj.pntMuunInventory[target]->m_Durability;

	if (nMuunDurability < 120)
	{
		LPITEM_ATTRIBUTE pSItemAttribute = GetItemAttr(Obj.pntMuunInventory[source]->m_Type);
		LPITEM_ATTRIBUTE pTItemAttribute = GetItemAttr(Obj.pntMuunInventory[target]->m_Type);
												
		if (pTItemAttribute && pSItemAttribute)
		{
			sLog->outBasic("[MuunSystem][EnergyGenerator] [%s][%s] [Mix Success] Source: [%s] Pos[%d] Rank[%d] Level[%d] Serial[%I64d] - Target: [%s] Pos[%d] Serial[%I64d] Dur[%d/%d/%d]",
				Obj.AccountID, Obj.Name, pSItemAttribute->Name, source, Obj.pntMuunInventory[source]->m_Durability, Obj.pntMuunInventory[source]->m_Level, Obj.pntMuunInventory[source]->m_Number,
				pTItemAttribute->Name, target, Obj.pntInventory[target]->m_Number, Obj.pntMuunInventory[target]->m_Durability, nEnergy, nMuunDurability);
		}

		Obj.pntMuunInventory[target]->m_Durability = nMuunDurability;
		gGameProtocol.GCMuunItemDurSend(Obj.m_Index, target, nMuunDurability);
	}

	else
	{
		LPITEM_ATTRIBUTE pSItemAttribute = GetItemAttr(Obj.pntMuunInventory[source]->m_Type);
		LPITEM_ATTRIBUTE pTItemAttribute = GetItemAttr(Obj.pntMuunInventory[target]->m_Type);
												
		if (pTItemAttribute && pSItemAttribute)
		{
			sLog->outBasic("[MuunSystem][EnergyGenerator] [%s][%s] [Create Success] Source: [%s] Pos[%d] Rank[%d] Level[%d] Serial[%I64d] - Target: [%s] Pos[%d] Serial[%I64d] Dur[%d/%d/%d]",
				Obj.AccountID, Obj.Name, pSItemAttribute->Name, source, Obj.pntMuunInventory[source]->m_Durability, Obj.pntMuunInventory[source]->m_Level, Obj.pntMuunInventory[source]->m_Number,
				pTItemAttribute->Name, target, Obj.pntInventory[target]->m_Number, Obj.pntMuunInventory[target]->m_Durability, nEnergy, nMuunDurability);
		}

		int iType = ITEMGET(13,240);
		int iLevel = 0;
		float fDur = 255.0;

		BYTE SocketOption[5];
		memset(SocketOption, -1, sizeof(SocketOption));

		ItemCreate(Obj.m_Index, 0xE0, 0, 0, iType, iLevel, fDur, 0, 0, 0, Obj.m_Index, 0, 0, 0, SocketOption, 0);

		Obj.pntMuunInventory[target]->Clear();
		gGameProtocol.GCMuunInventoryItemDeleteSend(Obj.m_Index, target, 1);
	}

	return true;
}

void CMuunSystem::GCMuunInventoryUseItemResult(CGameObject &Obj, int iUseType, int iResult)
{
	PMSG_USEITEM_MUUN_INVEN_RESULT pResult; 

	pResult.btItemUseType = iUseType;
	pResult.btResult = iResult;
	PHeadSubSetB((BYTE*)&pResult, 0x4E, 0x08, sizeof(pResult));
	IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
}

int CMuunSystem::AddMuunItemPeriodInfo(CGameObject &Obj)
{
	for (int i = 0; i < g_ConfigRead.server.GetObjectMaxUser(); i++)
	{
		if (this->m_MuunItemPeriodData[i].bIsUsed == false)
		{
			this->m_MuunItemPeriodData[i].Clear();
			this->m_MuunItemPeriodData[i].bIsUsed = true;
			this->m_MuunItemPeriodData[i].lpUserObj = lpObj;
			this->m_MuunItemPeriodData[i].dwUserGUID = Obj.DBNumber;
			this->m_MuunItemPeriodData[i].wUserIndex = Obj.m_Index;
			this->m_MuunItemPeriodData[i].btUsedDataCount = 0;
			std::memcpy(this->m_MuunItemPeriodData[i].chAccountID, Obj.Name, MAX_ACCOUNT_LEN + 1);
			std::memcpy(this->m_MuunItemPeriodData[i].chCharacterName, Obj.AccountID, MAX_ACCOUNT_LEN + 1);

			Obj.m_iMuunItmePeriodDatObj.m_Index = i;
			return i;
		}
	}

	return -1;
}

bool CMuunSystem::RemoveMuunItemPeriodInfo(CGameObject &Obj)
{
	if (this->IsCorrectUser(lpObj) == false)
	{
		return false;
	}

	int iPeriodItemSlotIndex = Obj.m_iMuunItmePeriodDatObj.m_Index;

	if (iPeriodItemSlotIndex < 0 || iPeriodItemSlotIndex >= g_ConfigRead.server.GetObjectMaxUser())
	{
		return false;
	}

	if (this->m_MuunItemPeriodData[iPeriodItemSlotIndex].bIsUsed == false)
	{
		return false;
	}

	this->m_MuunItemPeriodData[iPeriodItemSlotIndex].Clear();

	return true;
}

bool CMuunSystem::IsCorrectUser(CGameObject &Obj)
{
	int iPeriodItemSlotIndex = Obj.m_iMuunItmePeriodDatObj.m_Index;

	if (iPeriodItemSlotIndex < 0 || iPeriodItemSlotIndex >= g_ConfigRead.server.GetObjectMax())
	{
		return false;
	}

	if (this->m_MuunItemPeriodData[iPeriodItemSlotIndex].bIsUsed == FALSE)
	{
		return false;
	}

	if (this->m_MuunItemPeriodData[iPeriodItemSlotIndex].dwUserGUID != Obj.DBNumber)
	{
		return false;
	}

	return true;
}

int CMuunSystem::AddMuunItmePeriodData(CGameObject lpObj, WORD wItemCode, UINT64 dwSerial, int iDuration, CMuunInfo *pCMuunInfo)
{
	int iMuunItemPeriodDatObj.m_Index = Obj.m_iMuunItmePeriodDatObj.m_Index;

	if (CMuunSystem::IsCorrectUser(lpObj) == false)
	{
		return -1;
	}

	for (int i = 0; i < MUUN_INVENTORY_SIZE; i++)
	{
		if (!this->m_MuunItemPeriodData[iMuunItemPeriodDatObj.m_Index].ItemData[i].bIsUsed)
		{
			this->GetExpireDate(iDuration);
			this->m_MuunItemPeriodData[iMuunItemPeriodDatObj.m_Index].ItemData[i].bIsUsed = TRUE;
			this->m_MuunItemPeriodData[iMuunItemPeriodDatObj.m_Index].ItemData[i].wItemCode = wItemCode;
			this->m_MuunItemPeriodData[iMuunItemPeriodDatObj.m_Index].ItemData[i].dwSerial = dwSerial;
			this->m_MuunItemPeriodData[iMuunItemPeriodDatObj.m_Index].ItemData[i].pCMuunInfo = pCMuunInfo;
			return i;
		}
	}

	return -1;
}

void CMuunSystem::CheckMuunItemPeriodData(CGameObject &Obj)
{
	if (lpObj == NULL)
	{
		sLog->outBasic("[MuunSystem][Error] lpObj is NULL [%d]", __LINE__);
		return;
	}

	this->SkillProc(lpObj);

	if (GetTickCount() - Obj.dwCheckMuunItemTime >= 60000)
	{
		Obj.dwCheckMuunItemTime = GetTickCount();
		int i = Obj.m_iMuunItmePeriodDatObj.m_Index;

		if (i < 0 || i >= g_ConfigRead.server.GetObjectMaxUser())
		{
			return;
		}
			
		if (this->m_MuunItemPeriodData[i].bIsUsed == TRUE)
		{
			if (this->m_MuunItemPeriodData[i].lpUserObj != NULL)
			{
				CGameObject lpUserObj = this->m_MuunItemPeriodData[i].lpUserObj;
					
				if (lpUserObj.Type == OBJ_USER)
				{
					if (lpUserObj.Connected == PLAYER_PLAYING)
					{
						this->CheckEquipMuunItemConditionProc(lpUserObj);

						for (int iItemSlotIndex = 0; iItemSlotIndex < MUUN_INVENTORY_SIZE; iItemSlotIndex++)
						{
							if (this->m_MuunItemPeriodData[i].ItemData[iItemSlotIndex].bIsUsed == TRUE)
							{
								CMuunInfo *pCMuunInfo = this->m_MuunItemPeriodData[i].ItemData[iItemSlotIndex].pCMuunInfo;
									
								if (!pCMuunInfo)
								{
									sLog->outBasic("[MuunSystem][Error] pCMuunInfo is NULL [%s][%s] [%d]", lpUserObj.AccountID, lpUserObj.Name, __LINE__);								
									return;
								}
								
								if (this->CheckAddOptionExpireDate(pCMuunInfo->GetAddOptStart(), pCMuunInfo->GetAddOptEnd()) == 1)
								{
									this->RemovePeriodMunnItemData(lpUserObj, this->m_MuunItemPeriodData[i].ItemData[iItemSlotIndex].wItemCode, this->m_MuunItemPeriodData[i].ItemData[iItemSlotIndex].dwSerial);							
									this->m_MuunItemPeriodData[i].ItemData[iItemSlotIndex].Clear();
								}
							}
						}
					}
				}
			}
		}
	}
}

bool CMuunSystem::ClearPeriodMuunItemData(CGameObject lpObj, WORD wItemCode, UINT64 dwSerial)
{
	bool result;
	signed int iItemSlotIndex; 

	if (lpObj)
	{
		if (Obj.Type == OBJ_USER && Obj.Connected >= PLAYER_LOGGED)
		{
			if (this->IsMuunItem(wItemCode))
			{
				int i = Obj.m_iMuunItmePeriodDatObj.m_Index;
				if (this->m_MuunItemPeriodData[i].bIsUsed == TRUE)
				{
					for (iItemSlotIndex = 0; iItemSlotIndex < MUUN_INVENTORY_SIZE; ++iItemSlotIndex)
					{
						if (this->m_MuunItemPeriodData[i].ItemData[iItemSlotIndex].bIsUsed == 1
							&& this->m_MuunItemPeriodData[i].ItemData[iItemSlotIndex].wItemCode == wItemCode
							&& this->m_MuunItemPeriodData[i].ItemData[iItemSlotIndex].dwSerial == dwSerial)
						{
							this->m_MuunItemPeriodData[i].ItemData[iItemSlotIndex].Clear();
							return 1;
						}
					}
				}
				result = 0;
			}
			else
			{
				result = 0;
			}
		}
		else
		{
			result = 0;
		}
	}
	else
	{
		result = 0;
	}
	return result;
}

bool CMuunSystem::RemovePeriodMunnItemData(CGameObject lpObj, WORD wItemCode, UINT64 dwSerial)
{
	bool result; 
	int iInventoryPosition; 

	if (lpObj)
	{
		if (Obj.Type == 1 && Obj.Connected >= 2)
		{
			iInventoryPosition = this->GetItemFromMuunInventory(lpObj, wItemCode, dwSerial);
			if (iInventoryPosition == -1)
			{
				result = 0;
			}
			else
			{
				this->SetDisableMuunItemToExpire(lpObj, iInventoryPosition);
				result = 1;
			}
		}
		else
		{
			result = 0;
		}
	}
	else
	{
		result = 0;
	}
	return result;
}

bool CMuunSystem::SetDisableMuunItemToExpire(CGameObject lpObj, int iInventoryPosition)
{
	bool result; 

	if (lpObj)
	{
		if (Obj.Connected >= 2 || Obj.Type == 1)
		{
			if (iInventoryPosition == -1)
			{
				result = 0;
			}
			else
			{
				Obj.pntMuunInventory[iInventoryPosition]->SetMuunItemPeriodExpire();
				if (iInventoryPosition < 2)
				{
					if (Obj.pntMuunInventory[iInventoryPosition]->IsItem() == 1)
						this->CalCharacterStat(Obj.m_Index,Obj.pntMuunEffectList[iInventoryPosition]->nOptType);
				}
				GCMuunInventoryItemListSend(Obj.m_Index);
				result = 1;
			}
		}
		else
		{
			result = 0;
		}
	}
	else
	{
		result = 0;
	}
	return result;
}

void CMuunSystem::CheckMuunItemConditionLevelUp(CGameObject &Obj)
{
	int nMaxLv; 
	CMuunOpt *pCMuunInfo; 
	int iRet = -1;
	for (int i = 0; i < 2; ++i)
	{
		if (Obj.pntMuunInventory[i]->IsItem())
		{
			pCMuunInfo = Obj.pntMuunEffectList[i]->pCMuunInfo;
			if (!pCMuunInfo)
				return;
			if (pCMuunInfo->GetConditionType() == 2)
			{
				int nMinLv = pCMuunInfo->GetConditionVal1();
				nMaxLv = pCMuunInfo->GetConditionVal2();
				if (nMinLv > Obj.Level || nMaxLv < Obj.Level)
				{
					if (Obj.pntMuunEffectList[i]->bOptEnable == 1)
					{
						Obj.pntMuunEffectList[i]->bOptEnable = 0;
						iRet = 0;
					}
				}
				else
				{
					if (!Obj.pntMuunEffectList[i]->bOptEnable)
					{
						Obj.pntMuunEffectList[i]->bOptEnable = 1;
						iRet = 1;
					}
				}
				if (iRet > -1)
				{
					this->GCSendConditionStatus(Obj.m_Index, i, iRet);
					this->CalCharacterStat(Obj.m_Index,Obj.pntMuunEffectList[i]->pCMuunInfo);
				}
			}
		}
	}
}

void CMuunSystem::CheckMuunItemMoveMapConditionMap(CGameObject lpObj, int iMapNumber)
{
	CMuunOpt *pCMuunInfo; 

	for (int i = 0; i < 2; ++i)
	{
		if (Obj.pntMuunInventory[i]->IsItem() == 1)
		{
			pCMuunInfo = Obj.pntMuunEffectList[i]->pCMuunInfo;
			if (!pCMuunInfo)
				return;
			if (pCMuunInfo->GetConditionType() == 1)
			{
				if (pCMuunInfo->GetConditionVal1() == iMapNumber)
				{
					if (!Obj.pntMuunEffectList[i]->bOptEnable)
					{
						Obj.pntMuunEffectList[i]->bOptEnable = 1;
						this->GCSendConditionStatus(Obj.m_Index, i, 1);
						this->CalCharacterStat(Obj.m_Index,Obj.pntMuunEffectList[i]->pCMuunInfo);
					}
				}
				else
				{
					if (Obj.pntMuunEffectList[i]->bOptEnable == 1)
					{
						Obj.pntMuunEffectList[i]->bOptEnable = 0;
						this->GCSendConditionStatus(Obj.m_Index, i, 0);
						this->CalCharacterStat(Obj.m_Index,Obj.pntMuunEffectList[i]->pCMuunInfo);
					}
				}
			}
		}
	}
}

void CMuunSystem::CheckEquipMuunItemCondition(CGameObject &Obj)
{
	 int iRet; 

	for (int i = 0; i < 2; ++i)
	{
		if (Obj.pntMuunInventory[i]->IsItem() == 1)
		{
			iRet = this->CheckMuunItemCondition(lpObj,&Obj.pntMuunEffectList[i],Obj.pntMuunEffectList[i]->pCMuunInfo);
			if (iRet > -1)
				this->CalCharacterStat(Obj.m_Index,Obj.pntMuunEffectList[i]->pCMuunInfo);
			if (iRet < 0)
				iRet = 0;
			this->GCSendConditionStatus(Obj.m_Index, i, iRet);
		}
	}
}

void CMuunSystem::CheckEquipMuunItemConditionProc(CGameObject &Obj)
{

	signed int iRet; 

	for (int i = 0; i < 2; ++i)
	{
		if (Obj.pntMuunInventory[i]->IsItem() == 1)
		{
			iRet = this->CheckMuunItemConditionProc(&Obj.pntMuunEffectList[i],Obj.pntMuunEffectList[i]->pCMuunInfo);
			if (iRet > -1)
			{
				this->GCSendConditionStatus( Obj.m_Index, i, iRet);
				this->CalCharacterStat(Obj.m_Index,Obj.pntMuunEffectList[i]->pCMuunInfo);
			}
		}
	}
}

void CMuunSystem::CalCharacterStat(CGameObject &Obj, CMuunInfo *pCMuunInfo)
{
	if (!pCMuunInfo)
	{
		sLog->outBasic("[MuunSystem][Error] CalCharacterStat() pCMuunInfo is NULL [%d]", __LINE__);
		return;
	}

	if (pCMuunInfo->GetOptType() == 7 || pCMuunInfo->GetOptType() == 1)
	{
		gObjCalCharacter.CalcCharacter(Obj.m_Index);
	}
}

void CMuunSystem::CalCharacterStat( CGameObject &Obj, int iOptType)
{
	if (iOptType == 7 || iOptType == 1)
	{
		gObjCalCharacter.CalcCharacter(Obj.m_Index);
	}
}

int CMuunSystem::CheckMuunItemCondition(CGameObject &Obj,MUUN_EFFECT_LIST *pUserMuunEffect, CMuunInfo *pCMuunInfo)
{
	if (pCMuunInfo == NULL)
	{
		sLog->outBasic("[MuunSystem][Error] CheckMuunItemCondition() pCMuunInfo is NULL [%s][%s] [%d]", Obj.AccountID, Obj.Name, __LINE__);
		return -1;
	}

	switch (pCMuunInfo->GetConditionType())
	{
		case 3:
			return this->ChkMuunOptConditionTime(pUserMuunEffect, pCMuunInfo);
		case 4:
			return this->ChkMuunOptConditionDay(pUserMuunEffect, pCMuunInfo);
		case 2:
			return this->ChkMuunOptConditionLevel(lpObj, pUserMuunEffect, pCMuunInfo);
		case 1:
			return this->ChkMuunOptConditionMap(lpObj, pUserMuunEffect, pCMuunInfo);
		default:
			return -1;
	}
}

int CMuunSystem::CheckMuunItemConditionProc(MUUN_EFFECT_LIST *pUserMuunEffect, CMuunInfo *pCMuunInfo)
{
	int result; 
	int iRet; 

	if (pCMuunInfo)
	{
		iRet = -1;
		int iConditionType = pCMuunInfo->GetConditionType();
		if (iConditionType == 3)
		{
			iRet = this->ChkMuunOptConditionTime(pUserMuunEffect, pCMuunInfo);
		}
		else
		{
			if (iConditionType == 4)
				iRet = this->ChkMuunOptConditionDay(pUserMuunEffect, pCMuunInfo);
		}
		result = iRet;
	}
	else
	{
		sLog->outBasic(
			"[MuunSystem][Error] CheckMuunItemConditionProc() pCMuunInfo is NULL [%d]",
			__LINE__);
		result = -1;
	}
	return result;
}

int CMuunSystem::ChkMuunOptConditionTime(MUUN_EFFECT_LIST *pUserMuunEffect, CMuunInfo *pCMuunInfo)
{
	if (pCMuunInfo == NULL)
	{
		sLog->outBasic("[MuunSystem][Error] ChkMuunOptConditionTime() pCMuunInfo is NULL [%d]",__LINE__);
		return -1;
	}

	SYSTEMTIME tmToDay;
	int nStartTime = pCMuunInfo->GetConditionVal1();
	int nEndTime = pCMuunInfo->GetConditionVal2();
	GetLocalTime(&tmToDay);

	if (tmToDay.wHour < nStartTime || tmToDay.wHour >= nEndTime)
	{
		if (pUserMuunEffect->bOptEnable == TRUE)
		{
			pUserMuunEffect->bOptEnable = FALSE;
			return FALSE;
		}
	}

	else
	{
		if (pUserMuunEffect->bOptEnable == FALSE)
		{
			pUserMuunEffect->bOptEnable = TRUE;
			return TRUE;
		}
	}

	return -1;
}

int CMuunSystem::ChkMuunOptConditionDay(MUUN_EFFECT_LIST *pUserMuunEffect, CMuunInfo *pCMuunInfo)
{
	SYSTEMTIME tmToDay; 
	char DayOfWeek[7] = { 64, 32, 16, 8, 4, 2, 1 };

	if (!pCMuunInfo)
	{
		sLog->outBasic("[MuunSystem][Error] ChkMuunOptConditionDay() pCMuunInfo is NULL [%d]", __LINE__);
		return -1;
	}

	GetLocalTime(&tmToDay);

	if ((pCMuunInfo->GetConditionVal1() & DayOfWeek[tmToDay.wDayOfWeek]) != DayOfWeek[tmToDay.wDayOfWeek])
	{
		if (pUserMuunEffect->bOptEnable == TRUE)
		{
			pUserMuunEffect->bOptEnable = FALSE;
			return FALSE;
		}
	}

	else
	{
		if (pUserMuunEffect->bOptEnable == FALSE)
		{
			pUserMuunEffect->bOptEnable = TRUE;
			return TRUE;
		}
	}

	return -1;
}

int CMuunSystem::ChkMuunOptConditionLevel(CGameObject lpObj,MUUN_EFFECT_LIST *pUserMuunEffect, CMuunInfo *pCMuunInfo)
{
	if (!pCMuunInfo)
	{
		sLog->outBasic("[MuunSystem][Error] ChkMuunOptConditionLevel() pCMuunInfo is NULL [%d]", __LINE__);
		return -1;
	}

	if (Obj.Type != OBJ_USER)
	{
		sLog->outBasic("[MuunSystem][Error] ChkMuunOptConditionLevel() Obj.Type != OBJ_USER", __LINE__);
		return -1;
	}

	int nMinLv = pCMuunInfo->GetConditionVal1();
	int nMaxLv = pCMuunInfo->GetConditionVal2();

	if (nMinLv > (Obj.Level+Obj.m_PlayerData->MasterLevel) || nMaxLv < (Obj.Level+Obj.m_PlayerData->MasterLevel))
	{
		pUserMuunEffect->bOptEnable = FALSE;
		return FALSE;
	}

	else
	{
		pUserMuunEffect->bOptEnable = TRUE;
		return TRUE;
	}
}

int CMuunSystem::ChkMuunOptConditionMap(CGameObject lpObj,MUUN_EFFECT_LIST *pUserMuunEffect, CMuunInfo *pCMuunInfo)
{
	if (!pCMuunInfo)
	{
		sLog->outBasic("[MuunSystem][Error] ChkMuunOptConditionMap() pCMuunInfo is NULL [%d]", __LINE__);
		return -1;
	}
		
	if (pCMuunInfo->GetConditionVal1() == Obj.MapNumber)
	{
		pUserMuunEffect->bOptEnable = TRUE;
		return TRUE;
	}

	else
	{
		pUserMuunEffect->bOptEnable = FALSE;
		return FALSE;
	}
}

int CMuunSystem::GetItemFromMuunInventory(CGameObject lpObj, WORD wItemCode, UINT64 dwSerial)
{
	if(!lpObj)
	{
		return -1;
	}

	if(Obj.Type != OBJ_USER)
	{
		return -1;
	}

	if(Obj.Connected < PLAYER_LOGGED)
	{
		return -1;
	}

	for (int i = 0; i < MUUN_INVENTORY_SIZE; i++)
	{
		if (Obj.pntMuunInventory[i]->IsItem() == TRUE && Obj.pntMuunInventory[i]->m_Type == wItemCode && Obj.pntMuunInventory[i]->GetNumber() == dwSerial)
		{
			return i;
		}
	}

	return -1;
}

time_t CMuunSystem::GetCurrentDate()
{
	return time(NULL);
}

time_t CMuunSystem::GetExpireDate(int iDuration)
{
	time_t expiretime = time(NULL);

	expiretime += iDuration;

	return expiretime;
}

bool CMuunSystem::CheckExpireDate(time_t dwItemExpireDate)
{
	return this->GetCurrentDate() > dwItemExpireDate;
}

time_t CMuunSystem::GetLeftDate(time_t ExpireDate) // unused
{
	time_t currtime = time(NULL);
	time_t difftime = ExpireDate - currtime;

	return difftime;
}

bool CMuunSystem::CheckAddOptionExpireDate(time_t dwStartDate, time_t dwEndDate)
{
	bool result; 
	int dwCurrentDate; 

	dwCurrentDate = this->GetCurrentDate();
	if (dwCurrentDate >= dwStartDate)
		result = dwCurrentDate < dwStartDate || dwCurrentDate >dwEndDate;
	else
		result = 0;
	return result;
}

void CMuunSystem::MsgIsMuunItemActive(CGameObject lpObj, int iPos)
{
	//trash

}

bool CMuunSystem::LoadScriptMuunExchange(char *lpszFileName)
{
	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(lpszFileName);

	if (res.status != pugi::status_ok)
	{
		sLog->outError("[MuunSystem] Cannot load %s file! (%s)", lpszFileName, res.description());
		return false;
	}

	pugi::xml_node mainXML = file.child("MuunSystem");
	pugi::xml_node energy_converter = mainXML.child("EnergyConvertSettings");

	this->m_iStoneOfEvolutionPt = energy_converter.attribute("StoneOfEvolutionPoint").as_int();

	int iCount = 0;

	for (pugi::xml_node muun = energy_converter.child("Muun"); muun; muun = muun.next_sibling())
	{
		if (iCount >= 8)
		{
			sLog->outError("Exceed max entries (StoneOfEvolutionPoint)");
			break;
		}

		this->m_stEnergyGeneratorPoint[iCount].iRank = muun.attribute("Rank").as_int();
		this->m_stEnergyGeneratorPoint[iCount].iRankPt = muun.attribute("RankPoint").as_int();
		this->m_stEnergyGeneratorPoint[iCount].iMuunLv = muun.attribute("Level").as_int();
		this->m_stEnergyGeneratorPoint[iCount].iMuunLvPt = muun.attribute("LevelPoint").as_int();
		iCount++;
	}

	pugi::xml_node required_items = mainXML.child("RequiredItemsExchange");
	iCount = 0;

	for (pugi::xml_node item = required_items.child("Item"); item; item = item.next_sibling())
	{
		if (iCount >= 10)
		{
			sLog->outError("Exceed max entries (RequiredItemsExchange)");
			break;
		}

		this->m_stMuunExchangeInfo[iCount].iItemType = item.attribute("Cat").as_int();
		this->m_stMuunExchangeInfo[iCount].iItemIndex = item.attribute("Index").as_int();
		this->m_stMuunExchangeInfo[iCount].iInvenChk = item.attribute("InventoryCheck").as_int();
		this->m_stMuunExchangeInfo[iCount].iItemCnt = item.attribute("ItemCount").as_int();
		this->m_stMuunExchangeInfo[iCount].iItemBagIndex = item.attribute("ItemBagIndex").as_int();
		iCount++;
	}

	pugi::xml_node bag_list = mainXML.child("BagListSettings");
	iCount = 0;

	for (pugi::xml_node bag = bag_list.child("Bag"); bag; bag = bag.next_sibling())
	{
		if (iCount >= 20)
		{
			sLog->outError("Exceed max entries (BagListSettings)");
			break;
		}

		this->m_stMuunExchangeItembag[iCount].iIndex = bag.attribute("Index").as_int();
		this->m_stMuunExchangeItembag[iCount].iItemType = bag.attribute("ItemCat").as_int();
		this->m_stMuunExchangeItembag[iCount].iItemIndex = bag.attribute("ItemIndex").as_int();
		this->m_stMuunExchangeItembag[iCount].iMinLv = bag.attribute("ItemMinLevel").as_int();
		this->m_stMuunExchangeItembag[iCount].iMaxLv = bag.attribute("ItemMaxLevel").as_int();
		this->m_stMuunExchangeItembag[iCount].iSkill = bag.attribute("Skill").as_int();
		this->m_stMuunExchangeItembag[iCount].iLuck = bag.attribute("Luck").as_int();
		this->m_stMuunExchangeItembag[iCount].iAddOpt = bag.attribute("Option").as_int();
		this->m_stMuunExchangeItembag[iCount].iExItem = bag.attribute("Exc").as_int();
		this->m_stMuunExchangeItembag[iCount].iStoneMuunItemType = bag.attribute("MuunEvoItemCat").as_int();
		this->m_stMuunExchangeItembag[iCount].iStoneMuunItemIndex = bag.attribute("MuunEvoItemIndex").as_int();
		this->m_stMuunExchangeItembag[iCount].iInvenChk = bag.attribute("InventoryCheck").as_int();
		iCount++;
	}

	return true;
}

void CMuunSystem::CGMuunExchangeItem(PMSG_REQ_MUUN_EXCHANGE *lpMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}
    
	if (gObjIsConnected(Obj.m_Index) == FALSE)
	{
		return;
	}

	BYTE nSelect = lpMsg->btSelect;

	if (nSelect >= 10)
	{
		sLog->outBasic("[MuunSystem][MuunExchange][Error][%s][%s] Select Over %d [%s, %d]", Obj.AccountID, Obj.Name, nSelect, __FILE__, __LINE__);
		return;
	}

	if (this->ChkMuunExchangeInvenNeedItem(getGameObject(Obj.m_Index), nSelect, 0) == FALSE)
	{
		sLog->outBasic("[MuunSystem][MuunExchange][FAIL][%s][%s] Lack of Materials", Obj.AccountID, Obj.Name);

		this->SendMsgMuunExchange(Obj.m_Index, 1);
		return;
	}
          
	if (this->ChkMuunExchangeInvenEmpty(getGameObject(Obj.m_Index), nSelect) == FALSE)
	{
		sLog->outBasic("[MuunSystem][MuunExchange][FAIL][%s][%s] Not Empty Space", Obj.AccountID, Obj.Name);

		this->SendMsgMuunExchange(Obj.m_Index, 2);
		return;
	}
            
	this->GDMuunExchangeInsertInven(getGameObject(Obj.m_Index), nSelect);
}

bool CMuunSystem::GDMuunExchangeInsertInven(CGameObject &Obj, int iSelect)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return false;
	}

	if (!gObjIsConnected(Obj.m_Index))
	{
		return false;
	}
	
	float fDur = 0.0;
	int iType = 0;
	int iLevel = 0;
	BYTE Y = 0;
	int iOption1 = 0;
	int iOption2 = 0;
	int iOption3 = 0;
	int iExOption = 0;
	int nSelect = iSelect;

	int nItemBagIndex = this->m_stMuunExchangeInfo[iSelect].iItemBagIndex;
	int level = 0;

	if (this->m_stMuunExchangeItembag[nItemBagIndex].iMinLv == this->m_stMuunExchangeItembag[nItemBagIndex].iMaxLv)
	{
		level = this->m_stMuunExchangeItembag[nItemBagIndex].iMinLv;
	}

	else
	{
		int sub = this->m_stMuunExchangeItembag[nItemBagIndex].iMaxLv- this->m_stMuunExchangeItembag[nItemBagIndex].iMinLv + 1;
		level = this->m_stMuunExchangeItembag[nItemBagIndex].iMinLv + rand() % sub;
	}

	iLevel = level;
	iType = ItemGetNumberMake(this->m_stMuunExchangeItembag[nItemBagIndex].iItemType,this->m_stMuunExchangeItembag[nItemBagIndex].iItemIndex);

	if ( iType == -1 )
	{
		return false;
	}

	BYTE SocketOption[5];
	memset(SocketOption, -1, sizeof(SocketOption));

	if (this->IsStoneofEvolution(iType) == true)
	{
		int nItemIndex = this->m_stMuunExchangeItembag[nItemBagIndex].iStoneMuunItemIndex;
		SocketOption[0] = (nItemIndex + (this->m_stMuunExchangeItembag[nItemBagIndex].iStoneMuunItemType << 9)) >> 8;
		SocketOption[1] = nItemIndex;
	}
		
	if (this->m_stMuunExchangeItembag[nItemBagIndex].iSkill)
	{
		iOption1 = TRUE;
	}

	if (this->m_stMuunExchangeItembag[nItemBagIndex].iLuck)
	{
		iOption2 = FALSE;
		
		if ((rand() % 2) == 0)
		{
			iOption2 = TRUE;
		}
	}
		
	LPITEM_ATTRIBUTE ItemAttribute = GetItemAttr(iType);

	if (ItemAttribute == NULL)
	{
		return false;
	}
			
	if (this->m_stMuunExchangeItembag[nItemBagIndex].iExItem)
	{
		if (ItemAttribute->ItemKindA == ITEM_KIND_A_WING)
		{
			iExOption = g_ItemOptionTypeMng.WingExcOptionRand(ItemAttribute->ItemKindA, ItemAttribute->ItemKindB, SocketOption);
		}

		else
		{
			iExOption = g_ItemOptionTypeMng.CommonExcOptionRand(ItemAttribute->ItemKindA, SocketOption);
		}

		iOption2 = FALSE;
		iOption1 = TRUE;
		iLevel = 0;
	}

	if (this->m_stMuunExchangeItembag[nItemBagIndex].iAddOpt)
	{
		if ((rand() % 5) == 0)
		{
			iOption3 = 3;
		}
		
		else
		{
			iOption3 = rand() % 3;
		}
	}

	if ( iType == ITEMGET(12,15) || iType == ITEMGET(14,13) || iType == ITEMGET(14,14) )
	{
		iOption1 = FALSE;
		iOption2 = FALSE;
		iOption3 = FALSE;
		iLevel = 0;
	}

	if ( iType == ITEMGET(14, 70) || iType == ITEMGET(14,71) )
		fDur = 50.0;

	if ( iType == ITEMGET(14,85) || iType == ITEMGET(14,86) || iType == ITEMGET(14,87) )
		fDur = 10.0;

	if ( iType == ITEMGET(14,53) )
		fDur = 10.0;

	ItemCreate(Obj.m_Index, 218, nSelect, Y, iType, iLevel, fDur, iOption1, iOption2, iOption3, Obj.m_Index, iExOption, 0, 0, SocketOption, 0);

	sLog->outBasic("[MuunSystem][MuunExchange][ItemCreate] [%s][%s] : Item:(%s)%d Level:%d op1:%d op2:%d op3:%d ExOp:%d",
		Obj.AccountID, Obj.Name, ItemAttribute->Name, iType, iLevel, iOption1, iOption2, iOption3, iExOption);
	return true;
}

BYTE CMuunSystem::DGMuunExchangeInsertInven(CGameObject &Obj, CItemObject CreateItem, int iSelect)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return false;
	}

	if (!gObjIsConnected(Obj.m_Index))
	{
		return false;
	}

	BYTE btRet = -1;
	int nItemBagIndex = this->m_stMuunExchangeInfo[iSelect].iItemBagIndex;
	int nChkInven = this->m_stMuunExchangeItembag[nItemBagIndex].iInvenChk;

	if (this->ChkAndDelItemMuunExchange(lpObj, iSelect) == false)
	{
		sLog->outBasic("[MuunSystem][MuunExchange][ItemSerialCreateRecv][ItemDelFail][%s][%s][%d]",
			Obj.AccountID, Obj.Name, iSelect);
		return btRet;
	}

	if (nChkInven == 22)
	{
		btRet = gObjMuunInventoryInsertItem(Obj.m_Index, CreateItem);

		if (btRet != 0xFF)
		{
			gGameProtocol.GCMuunInventoryItemOneSend(Obj.m_Index, btRet);
			this->SendMsgMuunExchange(Obj.m_Index, 0);
			return btRet;
		}
	}

	else if (nChkInven == 0)
	{
		btRet = gObjInventoryInsertItem(Obj.m_Index, CreateItem);

		if (btRet != 0xFF)
		{
			gGameProtocol.GCInventoryItemOneSend(Obj.m_Index, btRet);
			this->SendMsgMuunExchange(Obj.m_Index, 0);
			return btRet;
		}
	}

	else
	{
		sLog->outBasic("[MuunSystem][MuunExchange][Error][%s][%s] DGMuunExchangeInsertInven() ChkInven %d [%s, %d]",
			Obj.AccountID, Obj.Name, nChkInven, __FILE__, __LINE__);

		return btRet;
	}

	this->SendMsgMuunExchange(Obj.m_Index, 2);
	sLog->outBasic("[MuunSystem][MuunExchange][FAIL][%s][%s] Not Empty Space", Obj.AccountID, Obj.Name);
	return btRet;
}

bool CMuunSystem::ChkAndDelItemMuunExchange(CGameObject &Obj, int iSelect)
{
	int ItemPos[10];
	memset(ItemPos, -1, sizeof(ItemPos));

	int nNeedItemCnt = this->m_stMuunExchangeInfo[iSelect].iItemCnt;
	int nNeedItemNum = ITEMGET(this->m_stMuunExchangeInfo[iSelect].iItemType, this->m_stMuunExchangeInfo[iSelect].iItemIndex);
	int nChkInven = this->m_stMuunExchangeInfo[iSelect].iInvenChk;

	if (this->ChkMuunExchangeInvenNeedItem(lpObj, iSelect, ItemPos) == false)
	{
		return false;
	}

	if (nChkInven == 22)
	{
		for (int nMuunInven = 0; nMuunInven < nNeedItemCnt; nMuunInven++)
		{
			int nItemPos =  ItemPos[nMuunInven];

			if (nItemPos == -1)
			{
				break;
			}

			if (Obj.pntMuunInventory[nItemPos]->IsItem() == TRUE &&
				Obj.pntMuunInventory[nItemPos]->m_Type == nNeedItemNum)
			{
				BYTE NewOption[MAX_EXOPTION_SIZE];
				ItemIsBufExOption(NewOption, &Obj.pntMuunInventory[nItemPos]);

				sLog->outBasic("[MuunSystem][MuunExchange] Delete MuunInven Item [%s][%s] Delete Item Info - Item:[%s,%d,%d,%d,%d] serial:[%I64d][%d] Ex:[%d,%d,%d,%d,%d,%d,%d] Set[%d] 380:[%d] HO:[%d,%d] SC[%d,%d,%d,%d,%d] BonusOption[%d]",
					Obj.AccountID, Obj.Name, Obj.pntMuunInventory[nItemPos]->GetName(), Obj.pntMuunInventory[nItemPos]->m_Level, Obj.pntMuunInventory[nItemPos]->m_Option1, Obj.pntMuunInventory[nItemPos]->m_Option2, Obj.pntMuunInventory[nItemPos]->m_Option3,
					Obj.pntMuunInventory[nItemPos]->m_Number, (int)Obj.pntMuunInventory[nItemPos]->m_Durability, NewOption[0], NewOption[1], NewOption[2], NewOption[3], NewOption[4], NewOption[5], NewOption[6], Obj.pntMuunInventory[nItemPos]->m_SetOption,
					Obj.pntMuunInventory[nItemPos]->m_ItemOptionEx >> 7, g_kJewelOfHarmonySystem.GetItemStrengthenOption(&Obj.pntMuunInventory[nItemPos]), g_kJewelOfHarmonySystem.GetItemOptionLevel(&Obj.pntMuunInventory[nItemPos]),
					Obj.pntMuunInventory[nItemPos]->m_SocketOption[0], Obj.pntMuunInventory[nItemPos]->m_SocketOption[1], Obj.pntMuunInventory[nItemPos]->m_SocketOption[2], Obj.pntMuunInventory[nItemPos]->m_SocketOption[3], Obj.pntMuunInventory[nItemPos]->m_SocketOption[4],
					Obj.pntMuunInventory[nItemPos]->m_BonusSocketOption);

				this->ClearPeriodMuunItemData(lpObj, Obj.pntMuunInventory[nItemPos]->m_Type, Obj.pntMuunInventory[nItemPos]->m_Number);
				Obj.pntMuunInventory[nItemPos]->Clear();
				gGameProtocol.GCMuunInventoryItemDeleteSend(Obj.m_Index, nItemPos, 1);
			}
		}
	}

	else if (nChkInven == 0)
	{
		for (int nInven = 0; nInven < nNeedItemCnt; nInven++)
		{
			int nItemPos =  ItemPos[nInven];

			if (nItemPos == -1)
			{
				break;
			}

			if (Obj.pntInventory[nItemPos]->IsItem() == TRUE &&
				Obj.pntInventory[nItemPos]->m_Type == nNeedItemNum)
			{
				BYTE NewOption[MAX_EXOPTION_SIZE];
				ItemIsBufExOption(NewOption, &Obj.pntMuunInventory[nItemPos]);

				sLog->outBasic("[MuunSystem][MuunExchange] Delete Inven Item [%s][%s] Delete Item Info - Item:[%s,%d,%d,%d,%d] serial:[%I64d][%d] Ex:[%d,%d,%d,%d,%d,%d,%d] Set[%d] 380:[%d] HO:[%d,%d] SC[%d,%d,%d,%d,%d] BonusOption[%d]",
					Obj.AccountID, Obj.Name, Obj.pntInventory[nItemPos]->GetName(), Obj.pntInventory[nItemPos]->m_Level, Obj.pntInventory[nItemPos]->m_Option1, Obj.pntInventory[nItemPos]->m_Option2, Obj.pntInventory[nItemPos]->m_Option3,
					Obj.pntInventory[nItemPos]->m_Number, (int)Obj.pntInventory[nItemPos]->m_Durability, NewOption[0], NewOption[1], NewOption[2], NewOption[3], NewOption[4], NewOption[5], NewOption[6], Obj.pntInventory[nItemPos]->m_SetOption,
					Obj.pntInventory[nItemPos]->m_ItemOptionEx >> 7, g_kJewelOfHarmonySystem.GetItemStrengthenOption(&Obj.pntInventory[nItemPos]), g_kJewelOfHarmonySystem.GetItemOptionLevel(&Obj.pntInventory[nItemPos]),
					Obj.pntInventory[nItemPos]->m_SocketOption[0], Obj.pntInventory[nItemPos]->m_SocketOption[1], Obj.pntInventory[nItemPos]->m_SocketOption[2], Obj.pntInventory[nItemPos]->m_SocketOption[3], Obj.pntInventory[nItemPos]->m_SocketOption[4],
					Obj.pntInventory[nItemPos]->m_BonusSocketOption);

				gObjInventoryItemSet(Obj.m_Index, nItemPos, -1);
				Obj.pntInventory[nItemPos]->Clear();
				gGameProtocol.GCInventoryItemDeleteSend(Obj.m_Index, nItemPos, 1);
			}
		}
	}

	else
	{
		sLog->outBasic("[MuunSystem][MuunExchange][Error][%s][%s] ChkAndDelItemMuunExchange() ChkInven %d [%s, %d]",
			Obj.AccountID, Obj.Name, nChkInven, __FILE__, __LINE__);

		return false;
	}

	return true;
}

bool CMuunSystem::ChkMuunExchangeInvenNeedItem(int & iItemCnt, int iInvenPos, int iNeedItemCnt, int iInvenItemNum, int iNeedItemNum, int * ItemPos)
{
	if (iInvenItemNum != iNeedItemNum)
	{
		return false;
	}

	if (ItemPos != 0)
	{
		if (iItemCnt < 10)
		{
			ItemPos[iItemCnt] = iInvenPos;
		}
	}

	iItemCnt++;

	if (iItemCnt < iNeedItemCnt)
	{
		return false;
	}

	return true;
}

bool CMuunSystem::ChkMuunExchangeInvenNeedItem(CGameObject &Obj, int iSelect, int *ItemPos)
{
	int nItemCnt = 0;
	int nNeedItemCnt = this->m_stMuunExchangeInfo[iSelect].iItemCnt;
	int nNeedItemNum = ITEMGET(this->m_stMuunExchangeInfo[iSelect].iItemType, this->m_stMuunExchangeInfo[iSelect].iItemIndex);
	int nChkInven = this->m_stMuunExchangeInfo[iSelect].iInvenChk;

	if ( nChkInven == 22 )
	{
		for (int nMuunInven = 2; nMuunInven < MUUN_INVENTORY_SIZE; nMuunInven++ )
		{
			if (Obj.pntMuunInventory[nMuunInven]->IsItem() == TRUE && this->ChkMuunExchangeInvenNeedItem(nItemCnt,nMuunInven, nNeedItemCnt, Obj.pntMuunInventory[nMuunInven]->m_Type, nNeedItemNum,ItemPos) == TRUE)
			{
				return TRUE;
			}
		}
	}

	else if (nChkInven == 0)
	{
		for (int nInven = INVETORY_WEAR_SIZE; nInven < MAIN_INVENTORY_SIZE; nInven++ )
		{
			if (Obj.pntInventory[nInven]->IsItem() == TRUE && this->ChkMuunExchangeInvenNeedItem(nItemCnt, nInven, nNeedItemCnt, Obj.pntInventory[nInven]->m_Type, nNeedItemNum, ItemPos) == TRUE)
			{
				return TRUE;
			}
		}
    }

	else
	{
		sLog->outBasic("[MuunSystem][MuunExchange][Error][%s][%s] ChkMuunExchangeInvenNeedItem() ChkInven %d [%s, %d]",Obj.AccountID,Obj.Name, nChkInven, __FILE__, __LINE__);
		return FALSE;
	}

	sLog->outBasic("[MuunSystem][MuunExchange][FAIL][%s][%s] Lack of Materials", Obj.AccountID, Obj.Name);
	
	this->SendMsgMuunExchange(Obj.m_Index, 1);
	return false;
}

bool CMuunSystem::ChkMuunExchangeInvenEmpty(CGameObject &Obj, int iSelect)
{
	int nChkInven = this->m_stMuunExchangeItembag[this->m_stMuunExchangeInfo[iSelect].iItemBagIndex].iInvenChk;

	if (nChkInven == 22)
	{
		if ( gObjChkMuunInventoryEmpty(lpObj) == 0xFF )
		{
			this->SendMsgMuunExchange(Obj.m_Index, 2);
			return false;
		}
	}

	else if (nChkInven == 0)
	{
		if (CheckInventoryEmptySpace(lpObj, 4, 4) == FALSE)
		{
			this->SendMsgMuunExchange(Obj.m_Index, 2);
			return false;
		}
	}

	else
	{
		sLog->outBasic("[MuunSystem][MuunExchange][Error][%s][%s] ChkMuunExchangeInvenEmpty() ChkInven %d [%s, %d]",  Obj.AccountID, Obj.Name, nChkInven, __FILE__,__LINE__);
		return false;
	}

	return true;
}

void CMuunSystem::SendMsgMuunExchange(CGameObject &Obj, int iResult)
{
	PMSG_ANS_MUUN_EXCHANGE pMsg; 

	pMsg.btResult = iResult;
	PHeadSubSetB((BYTE*)&pMsg, 0x4E, 0x13, sizeof(pMsg));
	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}

void CMuunSystem::SetTarget(CGameObject &Obj, int aTargetIndex)
{
	for (int i = 0; i < 2; i++)
	{
		if (Obj.pntMuunEffectList[i]->nOptType == 50 || Obj.pntMuunEffectList[i]->nOptType == 52)
		{
			Obj.pntMuunEffectList[i]->nTargetIndex = aTargetIndex;
		}

		if (Obj.pntMuunEffectList[i]->nOptType == 53)
		{
			if (getGameObject(aTargetIndex)->Type != OBJ_USER)
			{
				Obj.pntMuunEffectList[i]->nTargetIndex = aTargetIndex;
			}
		}
	}
}

void CMuunSystem::ReSetTarget(CGameObject &Obj, CGameObject &ObjTarget)
{
	for (int i = 0; i < 2; i++)
	{
		if (Obj.pntMuunEffectList[i]->nOptType == 50 || Obj.pntMuunEffectList[i]->nOptType == 52 || Obj.pntMuunEffectList[i]->nOptType == 53)
		{
			Obj.pntMuunEffectList[i]->nTargetIndex = -1;
		}
	}
}

void CMuunSystem::CGReqRideSelect(PMSG_MUUN_RIDE_SELECT *lpMsg, CGameObject &Obj)
{
	if(!ObjectMaxRange(Obj.m_Index))
		return;

	if(!gObjIsConnected(Obj.m_Index))
		return;

	

	Obj.m_wMuunRideItem = -1;

	for (int i = 0; i < 2; i++)
	{
		int nItemNum = Obj.pntMuunInventory[i]->m_Type;

		if ( nItemNum == lpMsg->wItemNum )
		{
			Obj.m_wMuunRideItem = nItemNum;
			break;
		}
	}

	MuunRideViewPortInfo MuunViewPortInfo; 
	BYTE btMuunInfosendBuf[2048]; 

	MuunViewPortInfo.NumberH = SET_NUMBERH(Obj.m_Index);
	MuunViewPortInfo.NumberL = SET_NUMBERL(Obj.m_Index);
	MuunViewPortInfo.MuunRideItemH = SET_NUMBERH(Obj.m_wMuunRideItem);
	MuunViewPortInfo.MuunRideItemL = SET_NUMBERL(Obj.m_wMuunRideItem);

	if ( Obj.m_wMuunRideItem == 0xFFFF )
	{
		MuunViewPortInfo.MuunRideItemH = SET_NUMBERH(Obj.m_wInvenPet);
		MuunViewPortInfo.MuunRideItemL = SET_NUMBERL(Obj.m_wInvenPet);
	}

	PMSG_SEND_MUUN_RIDE_VIEWPORT_INFO pMsgMuun;

	int lOfs = sizeof(pMsgMuun);

	std::memcpy(&btMuunInfosendBuf[lOfs], &MuunViewPortInfo, 4);
	lOfs += sizeof(MuunRideViewPortInfo);

	pMsgMuun.Count = 1;
	pMsgMuun.h.set((BYTE*)&pMsgMuun,0x4E,0x14,lOfs);
	std::memcpy(btMuunInfosendBuf, &pMsgMuun, sizeof(pMsgMuun));

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, btMuunInfosendBuf, lOfs);

	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (Obj.VpPlayer2[n].state != 0)
		{
			int tObjNum = Obj.VpPlayer2[n].number;

			if (tObjNum >= 0 && Obj.VpPlayer2[n].type == OBJ_USER)
			{
				IOCP.DataSend(tObjNum, btMuunInfosendBuf, lOfs);
			}
		}
	}
}

void CMuunSystem::SkillProc(CGameObject &Obj)
{
	this->m_MuunAttack.SkillProc(lpObj);
}

bool CMuunSystem::IsMuunExpireDate(int iType)
{
	if (this->IsMuunItem(iType) == false)
	{
		return false;
	}

	CMuunInfo *pCMuunInfo = this->m_MuunInfoMng.GetMuunItemNumToMuunInfo(iType);

	if (!pCMuunInfo)
	{
		return false;
	}

	if (this->CheckAddOptionExpireDate(pCMuunInfo->GetAddOptStart(), pCMuunInfo->GetAddOptEnd()) == true)
	{
		return true;
	}

	return false;
}

void CMuunSystem::Attack(CGameObject &Obj, CGameObject lpTargetObj, CMagicInf *lpMagic, int SubCode, int SubCode2)
{
	switch (SubCode)
	{
		case 50:
			this->m_MuunAttack.Attack(lpObj, lpTargetObj, lpMagic, SubCode2);
			break;
		case 51:
			this->m_MuunAttack.DamageAbsorb(lpObj, lpTargetObj, lpMagic, SubCode2);
			break;
		case 52:
			this->m_MuunAttack.Stun(lpObj, lpTargetObj, lpMagic, SubCode2);
			break;
		case 53:
			this->m_MuunAttack.Attack(lpObj, lpTargetObj, lpMagic, SubCode2);
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

