// BuffEffect.cpp: implementation of the CBuffEffect class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "User/CUserData.h"
#include "BuffEffect.h"
#include "BuffEffectSlot.h"
#include "Prodef.H"
#include "util.h"
#include "GameProtocol.h"
#include "GameMain.h"
#include "PeriodItemEx.h"
#include "Logging/Log.h"
#include "ArcaBattle.h"
#include "configread.h"
#include "ObjUseSkill.h"

CBuffEffect	g_BuffEffect;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBuffEffect::CBuffEffect()
{

}

CBuffEffect::~CBuffEffect()
{

}

void CBuffEffect::SetBuffEffect(CGameObject &Obj, BYTE EffectType, int EffectValue)
{
	if(lpObj == NULL || EffectType < EFFECTTYPE_NONE)	return;

	if(Obj.Connected < PLAYER_PLAYING)	return;

	switch(EffectType)
	{
	case EFFECTTYPE_ATTACKSPEED:
		Obj.m_AttackSpeed += EffectValue;
		Obj.m_MagicSpeed += EffectValue;
		break;
	case EFFECTTYPE_IMPROVE_DEFENSE:
		Obj.m_Defense += EffectValue;
		Obj.m_MagicDefense += EffectValue;
		break;
	case EFFECTTYPE_HP:
		Obj.AddLife += EffectValue;	
		gGameProtocol.GCReFillSend(Obj.m_Index, Obj.AddLife+Obj.MaxLife, 0xFE, 0, Obj.iAddShield+Obj.iMaxShield);
		gGameProtocol.GCReFillSend(Obj.m_Index, Obj.Life, 0xFF, 0, Obj.iShield);
		break;
	case EFFECTTYPE_MANA:
		Obj.AddMana += EffectValue;	
		gGameProtocol.GCManaSend(Obj.m_Index, (Obj.MaxMana + Obj.AddMana), 0xFE, 0, (Obj.MaxBP+Obj.AddBP));
		gGameProtocol.GCManaSend(Obj.m_Index, Obj.Mana, 0xFF, 0, Obj.BP);
		break;
	case EFFECTTYPE_STRENGTH:
		Obj.AddStrength += EffectValue;
		break;
	case EFFECTTYPE_DEXTERITY:
		EnterCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);
		Obj.m_PlayerData->AgilityCheckDelay = GetTickCount();
		Obj.AddDexterity += EffectValue;
		LeaveCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);
		break;
	case EFFECTTYPE_VITALITY:
		Obj.AddVitality += EffectValue;
		break;
	case EFFECTTYPE_ENERGY:
		Obj.AddEnergy += EffectValue;
		break;
	case EFFECTTYPE_LEADERSHIP:
		Obj.AddLeadership += EffectValue;
		break;
	case EFFECTTYPE_MELEEDAMAGE:
		Obj.m_AttackDamageMaxLeft += EffectValue;
		Obj.m_AttackDamageMinLeft += EffectValue;
		Obj.m_AttackDamageMaxRight += EffectValue;
		Obj.m_AttackDamageMinRight += EffectValue;
		break;
	case EFFECTTYPE_MAGICDAMAGE:
		Obj.m_MagicDamageMin += EffectValue;
		Obj.m_MagicDamageMax += EffectValue;
		break;
	case EFFECTTYPE_IMPROVE_MELEE_DEFENSE:
		Obj.m_SkillInfo.SoulBarrierDefence = EffectValue;
		break;
	case EFFECTTYPE_IMPROVE_MAGIC_DEFENSE:
		Obj.m_MagicDefense += EffectValue;
		break;
	case EFFECTTYPE_DAMAGEREFLECT:
		Obj.DamageReflect += EffectValue;
		break;
	case EFFECTTYPE_REDUCE_ATTACK_RATE:
		Obj.m_AttackRating -= EffectValue;
		break;
	case EFFECTTYPE_MELEE_DEFENSE_DOWN_MANA:
		Obj.m_SkillInfo.SoulBarrierManaRate = EffectValue;
		break;
	case EFFECTTYPE_BERSERKER_UP:
		Obj.AddMana += (int)( (((float)(EffectValue) * (float)(Obj.MaxMana))) / 100.0f);		
		gGameProtocol.GCManaSend(Obj.m_Index, (Obj.MaxMana + Obj.AddMana), 0xFE, 0, (Obj.MaxBP+Obj.AddBP));
		gGameProtocol.GCManaSend(Obj.m_Index, Obj.Mana, 0xFF, 0, Obj.BP);
		break;
	case EFFECTTYPE_BERSERKER_DOWN:
		{
			float fAddLife = 0;
			float fPer = (((float)(40.0f) - (float)(EffectValue))/100.0f);

			fPer = (fPer > 0.1f)?fPer:0.1f;
			fAddLife = fPer * Obj.MaxLife;

			Obj.AddLife -= (int)((float)(fAddLife));
			Obj.Life = ( (float)(Obj.Life) < ( (float)(Obj.AddLife) + (float)(Obj.MaxLife) ) )?( (float)(Obj.Life) ):( ( (float)(Obj.AddLife) + (float)(Obj.MaxLife) ) );

			gGameProtocol.GCReFillSend(Obj.m_Index, Obj.AddLife+Obj.MaxLife, 0xFE, 0, Obj.iAddShield+Obj.iMaxShield);
			gGameProtocol.GCReFillSend(Obj.m_Index, Obj.Life, 0xFF, 0, Obj.iShield);
		}
		break;
	case EFFECTTYPE_MAGICPOWER_INC:
		Obj.m_MagicDamageMin += EffectValue;
		break;
	case EFFECTTYPE_MAGICPOWERMAX_INC:
		Obj.m_MagicDamageMax += EffectValue;
		break;
	case EFFECTTYPE_POWER_UP:
		Obj.m_AttackDamageMaxLeft += EffectValue;
		Obj.m_AttackDamageMinLeft += EffectValue;
		Obj.m_AttackDamageMaxRight += EffectValue;
		Obj.m_AttackDamageMinRight += EffectValue;

		Obj.m_MagicDamageMin += EffectValue;
		Obj.m_MagicDamageMax += EffectValue;

		Obj.m_CurseDamageMin += EffectValue;
		Obj.m_CurseDamageMax += EffectValue;
		break;
	case EFFECTTYPE_GUARD_UP:
		Obj.m_Defense += EffectValue;
		break;
	case EFFECTTYPE_AG_UP:
		Obj.AddBP += EffectValue * (Obj.Level + Obj.m_PlayerData->MasterLevel);
		gGameProtocol.GCManaSend(Obj.m_Index, (WORD)(Obj.MaxMana + Obj.AddMana), 0xFE, 0, (WORD)(Obj.MaxBP+Obj.AddBP));
		gGameProtocol.GCManaSend(Obj.m_Index, (WORD)Obj.Mana, 0xFF, 0, (WORD)Obj.BP);
		break;
	case EFFECTTYPE_SD_UP:
		Obj.iAddShield += EffectValue * (Obj.Level + Obj.m_PlayerData->MasterLevel);
		gGameProtocol.GCReFillSend(Obj.m_Index, Obj.AddLife+Obj.MaxLife, 0xFE, 0, Obj.iAddShield+Obj.iMaxShield);
		gGameProtocol.GCReFillSend(Obj.m_Index, Obj.Life, 0xFF, 0, Obj.iShield);
		break;
	case EFFECTTYPE_SD_UP_VALUE:
		Obj.iAddShield += EffectValue;
		gGameProtocol.GCReFillSend(Obj.m_Index, Obj.AddLife+Obj.MaxLife, 0xFE, 0, Obj.iAddShield+Obj.iMaxShield);
		gGameProtocol.GCReFillSend(Obj.m_Index, Obj.Life, 0xFF, 0, Obj.iShield);
		break;
	case EFFECTTYPE_AG_UP_VALUE:
		Obj.AddBP += EffectValue;
		gGameProtocol.GCManaSend(Obj.m_Index, (WORD)(Obj.MaxMana + Obj.AddMana), 0xFE, 0, (WORD)(Obj.MaxBP+Obj.AddBP));
		gGameProtocol.GCManaSend(Obj.m_Index, (WORD)Obj.Mana, 0xFF, 0, (WORD)Obj.BP);
		break;
	case EFFECTTYPE_IMPROVE_DEFENSE_RATE:
		Obj.m_SuccessfulBlocking += EffectValue;
		break;
	case EFFECTTYPE_DECREASE_DEFENSE_RATE:
		Obj.m_SuccessfulBlocking -= EffectValue;
		if(Obj.m_SuccessfulBlocking < 0)
			Obj.m_SuccessfulBlocking = 0;
		break;
	case EFFECTTYPE_BLIND:
		Obj.m_bBlind = true;
		break;
	case EFFECTTYPE_WRATH_INC_DAMAGE:
		Obj.m_AttackDamageMinLeft += Obj.m_AttackDamageMinLeft * EffectValue / 100;
		Obj.m_AttackDamageMaxLeft += Obj.m_AttackDamageMaxLeft * EffectValue / 100;
		Obj.m_AttackDamageMinRight += Obj.m_AttackDamageMinRight * EffectValue / 100;
		Obj.m_AttackDamageMaxRight += Obj.m_AttackDamageMaxRight * EffectValue / 100;
		break;
	}
}

void CBuffEffect::ClearBuffEffect(CGameObject &Obj, BYTE EffectType, int EffectValue)
{
	if(lpObj == NULL || EffectType < EFFECTTYPE_NONE)	return;

	if(Obj.Connected < PLAYER_PLAYING)	return;

	switch(EffectType)
	{
	case EFFECTTYPE_ATTACKSPEED:
		Obj.m_AttackSpeed -= EffectValue;
		Obj.m_MagicSpeed -= EffectValue;
		break;
	case EFFECTTYPE_IMPROVE_DEFENSE:
		Obj.m_Defense -= EffectValue;
		Obj.m_MagicDefense -= EffectValue;
		break;
	case EFFECTTYPE_HP:
		Obj.AddLife -= EffectValue;
		if(Obj.AddLife <= 0.0)
			Obj.AddLife = 0.0;
		gGameProtocol.GCReFillSend(Obj.m_Index, Obj.AddLife+Obj.MaxLife, 0xFE, 0, Obj.iAddShield+Obj.iMaxShield);
		gGameProtocol.GCReFillSend(Obj.m_Index, Obj.Life, 0xFF, 0, Obj.iShield);
		break;
	case EFFECTTYPE_MANA:
		Obj.AddMana -= EffectValue;
		if(Obj.AddMana <= 0.0)
			Obj.AddMana = 0.0;
		gGameProtocol.GCManaSend(Obj.m_Index, (WORD)(Obj.MaxMana + Obj.AddMana), 0xFE, 0, (WORD)(Obj.MaxBP+Obj.AddBP));
		gGameProtocol.GCManaSend(Obj.m_Index, (WORD)Obj.Mana, 0xFF, 0, (WORD)Obj.BP);
		break;
	case EFFECTTYPE_STRENGTH:
		Obj.AddStrength -= EffectValue;
		break;
	case EFFECTTYPE_DEXTERITY:
		EnterCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);
		Obj.m_PlayerData->AgilityCheckDelay = GetTickCount();
		Obj.AddDexterity -= EffectValue;
		LeaveCriticalSection(&Obj.m_PlayerData->AgiCheckCriti);
		break;
	case EFFECTTYPE_VITALITY:
		Obj.AddVitality -= EffectValue;
		break;
	case EFFECTTYPE_ENERGY:
		Obj.AddEnergy -= EffectValue;
		break;
	case EFFECTTYPE_LEADERSHIP:
		Obj.AddLeadership -= EffectValue;
		break;
	case EFFECTTYPE_MELEEDAMAGE:
		Obj.m_AttackDamageMaxLeft -= EffectValue;
		Obj.m_AttackDamageMinLeft -= EffectValue;
		Obj.m_AttackDamageMaxRight -= EffectValue;
		Obj.m_AttackDamageMinRight -= EffectValue;
		break;
	case EFFECTTYPE_MAGICDAMAGE:
		Obj.m_MagicDamageMin -= EffectValue;
		Obj.m_MagicDamageMax -= EffectValue;
		break;
	case EFFECTTYPE_IMPROVE_MELEE_DEFENSE:
		Obj.m_SkillInfo.SoulBarrierDefence -= EffectValue;
		break;
	case EFFECTTYPE_IMPROVE_MAGIC_DEFENSE:
		Obj.m_MagicDefense -= EffectValue;
		break;
	case EFFECTTYPE_DAMAGEREFLECT:
		Obj.DamageReflect -= EffectValue;
		break;
	case EFFECTTYPE_REDUCE_ATTACK_RATE:
		Obj.m_AttackRating += EffectValue;
		break;
	case EFFECTTYPE_MELEE_DEFENSE_DOWN_MANA:
		Obj.m_SkillInfo.SoulBarrierManaRate = 0;
		break;
	case EFFECTTYPE_BERSERKER_UP:
		Obj.AddMana -= (int)( (((float)(EffectValue) * (float)(Obj.MaxMana))) / 100.0f);
		if(Obj.AddMana <= 0.0)
			Obj.AddMana = 0.0;
		Obj.Mana = ( (float)(Obj.Mana) < ( (float)(Obj.AddMana) + (float)(Obj.MaxMana) ) )?( (float)(Obj.Mana) ):( ( (float)(Obj.AddMana) + (float)(Obj.MaxMana) ) );
		
		gGameProtocol.GCManaSend(Obj.m_Index, (WORD)(Obj.MaxMana + Obj.AddMana), 0xFE, 0, (WORD)(Obj.MaxBP+Obj.AddBP));
		gGameProtocol.GCManaSend(Obj.m_Index, (WORD)Obj.Mana, 0xFF, 0, (WORD)Obj.BP);
		break;
	case EFFECTTYPE_BERSERKER_DOWN:
		{
			float fAddLife = 0;
			float fPer = (((float)(40.0f) - (float)(EffectValue))/100.0f);

			fPer = (fPer > 0.1f)?fPer:0.1f;
			fAddLife = fPer * Obj.MaxLife;

			Obj.AddLife += (int)(fAddLife);

			gGameProtocol.GCReFillSend(Obj.m_Index, Obj.AddLife+Obj.MaxLife, 0xFE, 0, Obj.iAddShield+Obj.iMaxShield);
			gGameProtocol.GCReFillSend(Obj.m_Index, Obj.Life, 0xFF, 0, Obj.iShield);
		}
		break;
	case EFFECTTYPE_MAGICPOWER_INC:
		Obj.m_MagicDamageMin -= EffectValue;
		break;
	case EFFECTTYPE_MAGICPOWERMAX_INC:
		Obj.m_MagicDamageMax -= EffectValue;
		Obj.m_PlayerData->m_MPSkillOpt.iMpsCriticalRateInc = 0.0;
		break;
	case EFFECTTYPE_POWER_UP:
		Obj.m_AttackDamageMaxLeft -= EffectValue;
		Obj.m_AttackDamageMinLeft -= EffectValue;
		Obj.m_AttackDamageMaxRight -= EffectValue;
		Obj.m_AttackDamageMinRight -= EffectValue;

		Obj.m_MagicDamageMin -= EffectValue;
		Obj.m_MagicDamageMax -= EffectValue;

		Obj.m_CurseDamageMin -= EffectValue;
		Obj.m_CurseDamageMax -= EffectValue;
		break;
	case EFFECTTYPE_GUARD_UP:
		Obj.m_Defense -= EffectValue;
		break;
	case EFFECTTYPE_AG_UP:
		Obj.AddBP -= EffectValue * (Obj.Level + Obj.m_PlayerData->MasterLevel);
		gGameProtocol.GCManaSend(Obj.m_Index, (WORD)(Obj.MaxMana + Obj.AddMana), 0xFE, 0, (WORD)(Obj.MaxBP+Obj.AddBP));
		gGameProtocol.GCManaSend(Obj.m_Index, (WORD)Obj.Mana, 0xFF, 0, (WORD)Obj.BP);
		break;
	case EFFECTTYPE_SD_UP:
		Obj.iAddShield -= EffectValue * (Obj.Level + Obj.m_PlayerData->MasterLevel);
		gGameProtocol.GCReFillSend(Obj.m_Index, Obj.AddLife+Obj.MaxLife, 0xFE, 0, Obj.iAddShield+Obj.iMaxShield);
		gGameProtocol.GCReFillSend(Obj.m_Index, Obj.Life, 0xFF, 0, Obj.iShield);
		break;
	case EFFECTTYPE_SD_UP_VALUE:
		Obj.iAddShield -= EffectValue;
		gGameProtocol.GCReFillSend(Obj.m_Index, Obj.AddLife+Obj.MaxLife, 0xFE, 0, Obj.iAddShield+Obj.iMaxShield);
		gGameProtocol.GCReFillSend(Obj.m_Index, Obj.Life, 0xFF, 0, Obj.iShield);
		break;
	case EFFECTTYPE_AG_UP_VALUE:
		Obj.AddBP -= EffectValue;
		gGameProtocol.GCManaSend(Obj.m_Index, (WORD)(Obj.MaxMana + Obj.AddMana), 0xFE, 0, (WORD)(Obj.MaxBP+Obj.AddBP));
		gGameProtocol.GCManaSend(Obj.m_Index, (WORD)Obj.Mana, 0xFF, 0, (WORD)Obj.BP);
		break;
	case EFFECTTYPE_IMPROVE_DEFENSE_RATE:
		Obj.m_SuccessfulBlocking -= EffectValue;
		break;
	case EFFECTTYPE_DECREASE_DEFENSE_RATE:
		Obj.m_SuccessfulBlocking += EffectValue;
		break;
	case EFFECTTYPE_ELF_BLESS:
		{
			PMSG_USE_STAT_FRUIT pMsg;
			PHeadSetB((BYTE*)&pMsg, 0x2C, sizeof(pMsg));
			pMsg.result = 0x12;
			pMsg.btStatValue = EffectValue;
			pMsg.btFruitType = 0x07;
			IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
		}
		break;
	case EFFECTTYPE_BLIND:
		Obj.m_bBlind = false;
		break;
	case EFFECTTYPE_WRATH_INC_DAMAGE:
		Obj.m_AttackDamageMinLeft -= Obj.m_AttackDamageMinLeft * EffectValue / 100;
		Obj.m_AttackDamageMaxLeft -= Obj.m_AttackDamageMaxLeft * EffectValue / 100;
		Obj.m_AttackDamageMinRight -= Obj.m_AttackDamageMinRight * EffectValue / 100;
		Obj.m_AttackDamageMaxRight -= Obj.m_AttackDamageMaxRight * EffectValue / 100;
		break;
	}
}

void CBuffEffect::SetActiveBuffEffect(class CGameObject* lpObj, BYTE EffectType, int EffectValue)
{
	if(EffectType <= EFFECTTYPE_NONE)	return;

	switch(EffectType)
	{
		case EFFECTTYPE_GIVE_DMG_TICK:		GiveDamageEffect(lpObj, EffectValue);			break;
		case EFFECTTYPE_POISON_DMG_TICK:	PoisonEffect(lpObj, EffectValue);				break;
		case EFFECTTYPE_FILLHP:				GiveDamageFillHPEffect(lpObj, EffectValue);		break;
	}
}

void CBuffEffect::GiveDamageEffect(class CGameObject* lpObj, int Damage)
{
	int DecreaseHealthPoint = 0;
	int DecreaseShiledPoint = 0;

	DecreaseHealthPoint = Damage;
	DecreaseShiledPoint = DecreaseHealthPoint;

	if(Obj.Live == 0)	return;

	if( Obj.lpAttackObj != NULL && 
		Obj.Type == OBJ_USER && Obj.m_bOffLevel == FALSE && Obj.lpAttackObj.Type == OBJ_USER)
	{
		DecreaseShiledPoint = 90 * DecreaseHealthPoint / 100;
		DecreaseHealthPoint -= DecreaseShiledPoint;

		if(Obj.iShield-DecreaseShiledPoint > 0)
		{
			Obj.iShield -= DecreaseShiledPoint;
			Obj.Life -= DecreaseHealthPoint;
		}
		else
		{
			DecreaseHealthPoint += DecreaseShiledPoint - Obj.iShield;
			DecreaseShiledPoint = Obj.iShield;
			Obj.Life -= DecreaseHealthPoint;
			Obj.iShield = 0;
		}
	}
	else
	{
		DecreaseShiledPoint = 0;
		Obj.Life -= DecreaseHealthPoint;
	}

	if(Obj.Life < 0.0f)
	{
		Obj.Life = 0.0f;
	}

	if(Obj.lpAttackObj != NULL)
	{
		gObjLifeCheck(lpObj, Obj.lpAttackObj, DecreaseHealthPoint, 3, 0, 0, 0, DecreaseShiledPoint, 0);
	}
}

void CBuffEffect::PoisonEffect(CGameObject &Obj, BYTE PoisonRate)
{
	int DecreaseHealthPoint = 0;
	int DecreaseShiledPoint = 0;

	if (g_ConfigRead.data.common.IsJoinMu == 1)
	{
		gObjUseSkill.m_Lua.Generic_Call("PoisonEffectCalc", "d>i", Obj.Life, &DecreaseHealthPoint);
	}
	else
	{
		DecreaseHealthPoint = (((int)(PoisonRate)* Obj.Life) / 100);
	}

	DecreaseShiledPoint = DecreaseHealthPoint;

	if(Obj.lpAttackObj != NULL && 
		Obj.Type == OBJ_USER && Obj.m_bOffLevel == FALSE && Obj.lpAttackObj.Type == OBJ_USER)
	{
		if(Obj.iShield-DecreaseShiledPoint > 0)
		{
			Obj.iShield -= DecreaseShiledPoint;
			DecreaseHealthPoint = 0;
		}
		else
		{
			DecreaseHealthPoint = DecreaseShiledPoint - Obj.iShield;
			DecreaseShiledPoint = Obj.iShield;
			Obj.Life -= DecreaseHealthPoint;
			Obj.iShield = 0;
		}
	}
	else
	{
		Obj.Life -= DecreaseHealthPoint;
		DecreaseShiledPoint = 0;
	}
	
	if(Obj.Life < 0.0f)
	{
		Obj.Life = 0.0f;
	}

	if(Obj.lpAttackObj != NULL)
	{
		gObjLifeCheck(lpObj, *Obj.lpAttackObj, DecreaseHealthPoint, 2, 0, 0, 1, DecreaseShiledPoint, 0);
	}
}

void CBuffEffect::GiveDamageFillHPEffect(CGameObject &Obj, int Damage)
{
	int DecreaseHealthPoint = 0;
	int DecreaseShiledPoint = 0;

	DecreaseHealthPoint = Damage;
	DecreaseShiledPoint = DecreaseHealthPoint;

	if (Obj.Live == 0)	return;

	if (Obj.lpAttackObj != NULL &&
		Obj.Type == OBJ_USER && Obj.lpAttackObj.Type == OBJ_USER)
	{
		DecreaseShiledPoint = 90 * DecreaseHealthPoint / 100;
		DecreaseHealthPoint -= DecreaseShiledPoint;

		if (Obj.iShield - DecreaseShiledPoint > 0)
		{
			Obj.iShield -= DecreaseShiledPoint;
			Obj.Life -= DecreaseHealthPoint;
		}
		else
		{
			DecreaseHealthPoint += DecreaseShiledPoint - Obj.iShield;
			DecreaseShiledPoint = Obj.iShield;
			Obj.Life -= DecreaseHealthPoint;
			Obj.iShield = 0;
		}
	}
	else
	{
		DecreaseShiledPoint = 0;
		Obj.Life -= DecreaseHealthPoint;
	}

	if (Obj.Life < 0.0f)
	{
		Obj.Life = 0.0f;
	}

	if (Obj.lpAttackObj != NULL)
	{
		gObjLifeCheck(lpObj, *Obj.lpAttackObj, DecreaseHealthPoint, 3, 0, 0, 0, DecreaseShiledPoint, 0);
	}

	if (gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_BLEEDING) == true)
	{
		int value1 = 0, value2 = 0;
		gObjGetValueOfBuffIndex(lpObj, BUFFTYPE_BLEEDING, &value1, &value2);

		if (ObjectMaxRange(value2) == false)	return;

		CGameObject lpTarget = &getGameObject(value2);

		if ((Obj.AddLife + Obj.MaxLife) >= (Obj.Life+DecreaseHealthPoint))
		{
			Obj.Life += DecreaseHealthPoint;
		}

		else
		{
			Obj.Life = Obj.AddLife + Obj.MaxLife;
		}

		gGameProtocol.GCReFillSend(Obj.m_Index, Obj.Life, 0xFF, 0, Obj.iShield);
	}
}

void CBuffEffect::SetPrevEffect(CGameObject &Obj)
{
	if(lpObj == NULL)	return;

	int BuffCount = 0;

	for(int i = 0; i < MAX_BUFFEFFECT; i++)
	{
		if(Obj.m_BuffEffectList[i].BuffIndex == BUFFTYPE_NONE) continue;

		switch(Obj.m_BuffEffectList[i].EffectType1)
		{
		case EFFECTTYPE_HP:
		case EFFECTTYPE_MANA:
		case EFFECTTYPE_STRENGTH:
		case EFFECTTYPE_DEXTERITY:
		case EFFECTTYPE_VITALITY:
		case EFFECTTYPE_ENERGY:
		case EFFECTTYPE_LEADERSHIP:
		case EFFECTTYPE_DAMAGEREFLECT:
		case EFFECTTYPE_SD_UP:
		case EFFECTTYPE_AG_UP:
		case EFFECTTYPE_SD_UP_VALUE:
		case EFFECTTYPE_AG_UP_VALUE:
			BuffCount++;
			SetBuffEffect(lpObj, Obj.m_BuffEffectList[i].EffectType1, Obj.m_BuffEffectList[i].EffectValue1);
			break;
		default:
			break;
		}

		switch(Obj.m_BuffEffectList[i].EffectType2)
		{
		case EFFECTTYPE_HP:
		case EFFECTTYPE_MANA:
		case EFFECTTYPE_STRENGTH:
		case EFFECTTYPE_DEXTERITY:
		case EFFECTTYPE_VITALITY:
		case EFFECTTYPE_ENERGY:
		case EFFECTTYPE_LEADERSHIP:
		case EFFECTTYPE_DAMAGEREFLECT:
		case EFFECTTYPE_SD_UP:
		case EFFECTTYPE_AG_UP:
		case EFFECTTYPE_SD_UP_VALUE:
		case EFFECTTYPE_AG_UP_VALUE:
			BuffCount++;
			SetBuffEffect(lpObj, Obj.m_BuffEffectList[i].EffectType2, Obj.m_BuffEffectList[i].EffectValue2);
			break;
		default:
			break;
		}
	}
}

void CBuffEffect::SetNextEffect(CGameObject &Obj)
{
	if(lpObj == NULL)	return;

	int BuffCount = 0;

	for(int i = 0; i < MAX_BUFFEFFECT; i++)
	{
		if(Obj.m_BuffEffectList[i].BuffIndex == BUFFTYPE_NONE) continue;

		switch(Obj.m_BuffEffectList[i].EffectType1)
		{
		case EFFECTTYPE_HP:
		case EFFECTTYPE_MANA:
		case EFFECTTYPE_STRENGTH:
		case EFFECTTYPE_DEXTERITY:
		case EFFECTTYPE_VITALITY:
		case EFFECTTYPE_ENERGY:
		case EFFECTTYPE_LEADERSHIP:
		case EFFECTTYPE_DAMAGEREFLECT:
		case EFFECTTYPE_SD_UP:
		case EFFECTTYPE_AG_UP:
		case EFFECTTYPE_SD_UP_VALUE:
		case EFFECTTYPE_AG_UP_VALUE:
			break;
		default:
			BuffCount++;
			SetBuffEffect(lpObj, Obj.m_BuffEffectList[i].EffectType1, Obj.m_BuffEffectList[i].EffectValue1);
			break;
		}

		switch(Obj.m_BuffEffectList[i].EffectType2)
		{
		case EFFECTTYPE_HP:
		case EFFECTTYPE_MANA:
		case EFFECTTYPE_STRENGTH:
		case EFFECTTYPE_DEXTERITY:
		case EFFECTTYPE_VITALITY:
		case EFFECTTYPE_ENERGY:
		case EFFECTTYPE_LEADERSHIP:
		case EFFECTTYPE_DAMAGEREFLECT:
		case EFFECTTYPE_SD_UP:
		case EFFECTTYPE_AG_UP:
		case EFFECTTYPE_SD_UP_VALUE:
		case EFFECTTYPE_AG_UP_VALUE:
			break;
		default:
			BuffCount++;
			SetBuffEffect(lpObj, Obj.m_BuffEffectList[i].EffectType2, Obj.m_BuffEffectList[i].EffectValue2);
			break;
		}
	}
}

void CBuffEffect::ClearPrevEffect(CGameObject &Obj)
{
	if(lpObj == NULL)	return;

	int BuffCount = 0;

	for(int i = 0; i < MAX_BUFFEFFECT; i++)
	{
		if(Obj.m_BuffEffectList[i].BuffIndex == BUFFTYPE_NONE) continue;

		switch(Obj.m_BuffEffectList[i].EffectType1)
		{
		case EFFECTTYPE_HP:
		case EFFECTTYPE_MANA:
		case EFFECTTYPE_STRENGTH:
		case EFFECTTYPE_DEXTERITY:
		case EFFECTTYPE_VITALITY:
		case EFFECTTYPE_ENERGY:
		case EFFECTTYPE_LEADERSHIP:
		case EFFECTTYPE_DAMAGEREFLECT:
		case EFFECTTYPE_SD_UP:
		case EFFECTTYPE_AG_UP:
		case EFFECTTYPE_SD_UP_VALUE:
		case EFFECTTYPE_AG_UP_VALUE:
			BuffCount++;
			ClearBuffEffect(lpObj, Obj.m_BuffEffectList[i].EffectType1, Obj.m_BuffEffectList[i].EffectValue1);
			break;
		default:
			break;
		}

		switch(Obj.m_BuffEffectList[i].EffectType2)
		{
		case EFFECTTYPE_HP:
		case EFFECTTYPE_MANA:
		case EFFECTTYPE_STRENGTH:
		case EFFECTTYPE_DEXTERITY:
		case EFFECTTYPE_VITALITY:
		case EFFECTTYPE_ENERGY:
		case EFFECTTYPE_LEADERSHIP:
		case EFFECTTYPE_DAMAGEREFLECT:
		case EFFECTTYPE_SD_UP:
		case EFFECTTYPE_AG_UP:
		case EFFECTTYPE_SD_UP_VALUE:
		case EFFECTTYPE_AG_UP_VALUE:
			BuffCount++;
			ClearBuffEffect(lpObj, Obj.m_BuffEffectList[i].EffectType2, Obj.m_BuffEffectList[i].EffectValue2);
			break;
		default:
			break;
		}
	}
}

struct PMSG_REQ_GUILD_PERIODBUFF_INSERT
{
	PBMSG_HEAD2 head;
	char szGuildName[MAX_GUILD_LEN+1];
	WORD wBuffIndex;
	char btEffectType1;
	char btEffectType2;
	DWORD dwDuration;
	time_t lExpireDate;
};

void CBuffEffect::RequestGuildPeriodBuffInsert(char *szGuildName,PeriodBuffInfo *lpBuffInfo)
{
	PMSG_REQ_GUILD_PERIODBUFF_INSERT pMsg;

	memcpy(pMsg.szGuildName, szGuildName, MAX_GUILD_LEN+1);
	pMsg.wBuffIndex = lpBuffInfo->wBuffIndex;
	pMsg.btEffectType1 = lpBuffInfo->btEffectType1;
	pMsg.btEffectType2 = lpBuffInfo->btEffectType2;
	pMsg.dwDuration = lpBuffInfo->lDuration;
	pMsg.lExpireDate = g_PeriodItemEx.GetExpireDate(lpBuffInfo->lDuration);
	PHeadSubSetB((BYTE*)&pMsg, 0x53, 1, sizeof(pMsg));
 
	wsExDbCli.DataSend((char*)&pMsg, pMsg.head.size);
  
	sLog->outBasic("[PeriodBuff][Insert] Request Insert Guild PeriodBuff. GuildName : %s, BuffIndex : %d, Duration : %d, lExpireDate : %d",
		szGuildName, lpBuffInfo->wBuffIndex, lpBuffInfo->lDuration, pMsg.lExpireDate);
}

struct PMSG_REQ_GUILD_PERIODBUFF_DELETE
{
	PBMSG_HEAD2 head;
	char btGuildCnt;
	WORD wBuffIndex[5];
};

void CBuffEffect::RequestGuildPeriodBuffDelete(WORD *wBuffIndex, char btGuildCnt)
{
	PMSG_REQ_GUILD_PERIODBUFF_DELETE pMsg;

	for ( int i = 0; i < btGuildCnt; ++i )
		pMsg.wBuffIndex[i] = wBuffIndex[i];

	pMsg.btGuildCnt = btGuildCnt;
	PHeadSubSetB((BYTE*)&pMsg, 0x53, 2, sizeof(pMsg));

	wsExDbCli.DataSend((char*)&pMsg, pMsg.head.size);
	sLog->outBasic("[PeriodBuff][Delete] Request All Delete Guild PeriodBuff");
}

struct PMSG_REQ_PERIODBUFF_DELETE
{
	PBMSG_HEAD2 head;
	WORD wUserIndex;
	WORD wBuffIndex;
	char szCharacterName[11];
};

void CBuffEffect::RequestPeriodBuffDelete(CGameObject lpObj, WORD wBuffIndex)
{
	PMSG_REQ_PERIODBUFF_DELETE pMsg;

	pMsg.wUserIndex = Obj.m_Index;
	pMsg.wBuffIndex = wBuffIndex;
	memcpy(pMsg.szCharacterName, Obj.Name, MAX_ACCOUNT_LEN+1);
	PHeadSubSetB((BYTE*)&pMsg, 0xE4, 2, sizeof(pMsg));
  
	wsDataCli.DataSend((char*)&pMsg, pMsg.head.size);
	sLog->outBasic("[PeriodBuff][Delete] Request Delete PeriodBuff. User Id : %s(%d), Name : %s, BuffIndex : %d", Obj.AccountID, Obj.DBNumber, Obj.Name, wBuffIndex);
}

void CBuffEffect::RequestPeriodBuffDelete(char *szName, WORD wBuffIndex)
{
	PMSG_REQ_PERIODBUFF_DELETE pMsg;

	pMsg.wUserIndex = 0;
	pMsg.wBuffIndex = wBuffIndex;
	memcpy(pMsg.szCharacterName, szName, MAX_ACCOUNT_LEN + 1);
	PHeadSubSetB((BYTE*)&pMsg, 0xE4, 2, sizeof(pMsg));

	wsDataCli.DataSend((char*)&pMsg, pMsg.head.size);
	sLog->outBasic("[PeriodBuff][Delete] Request Delete PeriodBuff.Name : %s, BuffIndex : %d", szName, wBuffIndex);
}

struct PMSG_REQ_PERIODBUFF_INSERT
{
	PBMSG_HEAD2 head;
	WORD wUserIndex;
	char szCharacterName[MAX_ACCOUNT_LEN+1];
	WORD wBuffIndex;
	char btEffectType1;
	char btEffectType2;
	DWORD dwDuration;
	time_t lExpireDate;
};

void CBuffEffect::RequestPeriodBuffInsert(CGameObject lpObj,PeriodBuffInfo *lpBuffInfo)
{
	PMSG_REQ_PERIODBUFF_INSERT pMsg; 

	pMsg.wUserIndex = Obj.m_Index;
	memcpy(pMsg.szCharacterName, Obj.Name, MAX_ACCOUNT_LEN+1);
	pMsg.wBuffIndex = lpBuffInfo->wBuffIndex;
	pMsg.btEffectType1 = lpBuffInfo->btEffectType1;
	pMsg.btEffectType2 = lpBuffInfo->btEffectType2;
	pMsg.dwDuration = lpBuffInfo->lDuration;
	pMsg.lExpireDate = g_PeriodItemEx.GetExpireDate(lpBuffInfo->lDuration);
	PHeadSubSetB((BYTE*)&pMsg, 0xE4, 1, sizeof(pMsg));

	wsDataCli.DataSend((char*)&pMsg, pMsg.head.size);
	sLog->outBasic("[PeriodBuff][Insert] Request Insert PeriodBuff. User Id : %s(%d), Name : %s, BuffIndex : %d, Duration %d, lExpireDate%d",
	Obj.AccountID, Obj.DBNumber, Obj.Name, lpBuffInfo->wBuffIndex, lpBuffInfo->lDuration, pMsg.lExpireDate);
}

struct PMSG_REQ_PERIODBUFF_SELECT
{
	PBMSG_HEAD2 head;
	WORD wUserIndex;
	char szCharacterName[MAX_ACCOUNT_LEN+1];
};

void CBuffEffect::RequestPeriodBuffSelect(CGameObject &Obj)
{
	PMSG_REQ_PERIODBUFF_SELECT pMsg;

	pMsg.wUserIndex = Obj.m_Index;
	memcpy(pMsg.szCharacterName, Obj.Name, MAX_ACCOUNT_LEN+1);
	PHeadSubSetB((BYTE*)&pMsg, 0xE4, 3, sizeof(pMsg));

	wsDataCli.DataSend((char*)&pMsg, pMsg.head.size);
}

void CBuffEffect::DGPeriodItemExSelect(PMSG_ANS_PERIODBUFF_SELECT *lpMsg)
{
	if (!ObjectMaxRange(lpMsg->wUserIndex))
	{
		return;
	}

	CGameObject lpObj = &getGameObject(lpMsg->wUserIndex);

	if ( Obj.Connected < PLAYER_LOGGED )
	{
		return;
	}

	if ( Obj.Type != OBJ_USER )
	{
		return;
	}

	if ( g_ArcaBattle.IsArcaBattleServer() == FALSE && lpMsg->wBuffIndex == 184 )
	{
		return;
	}

	LPPERIOD_BUFF_EFFECT_INFO lpPeriBuff = g_BuffEffectSlot.GetPeriodBuffInfo(lpMsg->wBuffIndex);

	if ( !lpPeriBuff )
	{
		return;
	}

	time_t lLeftDate = g_PeriodItemEx.GetLeftDate(lpMsg->lExpireDate);

	if ( gObjAddPeriodBuffEffect(lpObj, lpPeriBuff, lLeftDate) == FALSE )
	{
		sLog->outBasic("[PeriodBuff][Error][Select] Answer Select PeriodBuff. User Id : %s(%d), Name : %s, BuffIndex : %d Type1 : %d Type2 : %d ExpireDate : %d ResultCode : %d",
			Obj.AccountID, Obj.DBNumber, Obj.Name, lpMsg->wBuffIndex, lpMsg->btEffectType1, lpMsg->btEffectType2, lpMsg->lExpireDate, lpMsg->btResultCode);
	}

	else
	{
		sLog->outBasic("[PeriodBuff][Select] Answer Select PeriodBuff. User Id : %s(%d), Name : %s, BuffIndex : %d Type1 : %d Type2 : %d ExpireDate : %d ResultCode : %d",
			Obj.AccountID, Obj.DBNumber, Obj.Name, lpMsg->wBuffIndex, lpMsg->btEffectType1, lpMsg->btEffectType2, lpMsg->lExpireDate, lpMsg->btResultCode);
	}
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

