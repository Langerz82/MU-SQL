// TMonsterSkillElementInfo.cpp: implementation of the TMonsterSkillElementInfo class.
//	GS-N	1.00.18	JPN	0x00557A00	-	Completed
//////////////////////////////////////////////////////////////////////
#include "TMonsterSkillElementInfo.h"
#include "GameProtocol.h"
#include "User/CUserData.h"
#include "BuffEffectSlot.h"
//////////////////////////////////////////////////////////////////////
// Construction
//////////////////////////////////////////////////////////////////////

void TMonsterSkillElementInfo::CheckSkillElementInfoProc(CGameObject &Obj)
{
	if ( Obj.m_MonsterSkillElementInfo.CheckAttackTime() )
	{
		if ( Obj.m_MonsterSkillElementInfo.DecAttackTime() )
		{
			Obj.m_MonsterSkillElementInfo.ResetAttack();
		}
	}

	if ( Obj.m_MonsterSkillElementInfo.CheckDefenseTime() )
	{
		if ( Obj.m_MonsterSkillElementInfo.DecDefenseTime() )
		{
			Obj.m_MonsterSkillElementInfo.ResetDefense();
		}
	}
	
	if ( Obj.m_MonsterSkillElementInfo.CheckAutoAGTime() )
	{
		if ( Obj.m_MonsterSkillElementInfo.DecAutoAGTime() )
		{
			Obj.m_MonsterSkillElementInfo.ResetAutoAG();
		}
	}

	if ( Obj.m_MonsterSkillElementInfo.CheckAutoHPTime() )
	{
		if ( Obj.m_MonsterSkillElementInfo.DecAutoHPTime() )
		{
			Obj.m_MonsterSkillElementInfo.ResetAutoHP();
		}
		else 
		{
			if ( Obj.m_MonsterSkillElementInfo.m_iSkillElementAutoHPCycle == 110 )
				Obj.Life += Obj.m_MonsterSkillElementInfo.m_iSkillElementAutoHP;
			else
				Obj.Life += Obj.Life * Obj.m_MonsterSkillElementInfo.m_iSkillElementAutoHP / 100.0f;

			if ( Obj.Life <0 )
			{
				Obj.Life = 0;
				gGameProtocol.GCReFillSend(lpObj, Obj.Life, 0xFF, 0, Obj.iShield);
			}
			else if ( (Obj.MaxLife + Obj.AddLife) < Obj.Life )
			{
				Obj.Life = Obj.MaxLife + Obj.AddLife;
				gGameProtocol.GCReFillSend(lpObj, Obj.Life, 0xFF, 0, Obj.iShield);
			}
			else
			{
				gGameProtocol.GCReFillSend(lpObj, Obj.Life, 0xFF, 0, Obj.iShield);
			}
		}
	}

	if ( Obj.m_MonsterSkillElementInfo.CheckAutoMPTime() )
	{
		if ( Obj.m_MonsterSkillElementInfo.DecAutoMPTime() )
		{
			Obj.m_MonsterSkillElementInfo.ResetAutoMP();
		}
	}

	if ( Obj.m_MonsterSkillElementInfo.CheckImmuneTime() )
	{
		if ( Obj.m_MonsterSkillElementInfo.DecImmuneTime() )
		{
			Obj.m_MonsterSkillElementInfo.ResetImmune();
			gObjRemoveBuffEffect(lpObj, BUFFTYPE_MONSTER_MAGIC_IMMUNE);
			gObjRemoveBuffEffect(lpObj, BUFFTYPE_MONSTER_MELEE_IMMUNE);
		}
	}

	if ( Obj.m_MonsterSkillElementInfo.CheckResistTime() )
	{
		if ( Obj.m_MonsterSkillElementInfo.DecResistTime() )
		{
			Obj.m_MonsterSkillElementInfo.ResetResist();
		}
	}

	if ( Obj.m_MonsterSkillElementInfo.CheckModifyStatTime() )
	{
		if ( Obj.m_MonsterSkillElementInfo.DecModifyStatTime() )
		{
			Obj.m_MonsterSkillElementInfo.ResetModifyStat();
		}
	}

	if (Obj.m_MonsterSkillElementInfo.CheckImmuneAllTime())
	{
		if (Obj.m_MonsterSkillElementInfo.DecImmuneAllTime())
		{
			Obj.m_MonsterSkillElementInfo.ResetImmuneAll();
		}
	}
}

void TMonsterSkillElementInfo::Reset()
{
	this->ResetDefense();
	this->ResetAttack();
	this->ResetAutoHP();
	this->ResetAutoMP();
	this->ResetAutoAG();
	this->ResetImmune();
	this->ResetResist();
	this->ResetModifyStat();
	this->ResetImmuneAll();
}

void  TMonsterSkillElementInfo::ResetDefense()
{
	this->m_iSkillElementDefense = 0;
	this->m_iSkillElementDefenseTime = 0;
}

void  TMonsterSkillElementInfo::ResetAttack()
{
	this->m_iSkillElementAttack = 0;
	this->m_iSkillElementAttackTime = 0;
}

void  TMonsterSkillElementInfo::ResetAutoHP()
{
	this->m_iSkillElementAutoHP = 0;
	this->m_iSkillElementAutoHPCycle = 0;
	this->m_iSkillElementAutoHPTime = 0;
}

void  TMonsterSkillElementInfo::ResetAutoMP()
{
	this->m_iSkillElementAutoMP = 0;
	this->m_iSkillElementAutoMPCycle = 0;
	this->m_iSkillElementAutoMPTime = 0;
}

void  TMonsterSkillElementInfo::ResetAutoAG()
{
	this->m_iSkillElementAutoAG = 0;
	this->m_iSkillElementAutoAGCycle = 0;
	this->m_iSkillElementAutoAGTime = 0;
}

void  TMonsterSkillElementInfo::ResetImmune()
{
	this->m_iSkillElementImmuneNumber = 0;
	this->m_iSkillElementImmuneTime = 0;
}

void  TMonsterSkillElementInfo::ResetResist()
{
	this->m_iSkillElementResistNumber = 0;
	this->m_iSkillElementResistTime = 0;
}

void  TMonsterSkillElementInfo::ResetModifyStat()

{
	this->m_iSkillElementModifyStat = 0;
	this->m_iSkillElementModifyStatType = 0;
	this->m_iSkillElementModifyStatTime = 0;
}

void  TMonsterSkillElementInfo::ResetImmuneAll()
{
	this->m_iSkillElementImmuneAllTime = 0;
}

/////////////////////////////////////////////////////////

BOOL  TMonsterSkillElementInfo::CheckDefenseTime()
{
	return (this->m_iSkillElementDefense <= 0)?FALSE:TRUE;
}

BOOL  TMonsterSkillElementInfo::CheckAttackTime()
{
	return (this->m_iSkillElementAttack <= 0)?FALSE:TRUE;
}

BOOL  TMonsterSkillElementInfo::CheckAutoHPTime()
{
	return (this->m_iSkillElementAutoHPTime <= 0)?FALSE:TRUE;
}

BOOL  TMonsterSkillElementInfo::CheckAutoMPTime()
{
	return (this->m_iSkillElementAutoMPTime <= 0)?FALSE:TRUE;
}

BOOL  TMonsterSkillElementInfo::CheckAutoAGTime()
{
	return (this->m_iSkillElementAutoAGTime <= 0)?FALSE:TRUE;
}

BOOL  TMonsterSkillElementInfo::CheckImmuneTime()
{
	return (this->m_iSkillElementImmuneTime <= 0)?FALSE:TRUE;
}

BOOL  TMonsterSkillElementInfo::CheckResistTime()
{
	return (this->m_iSkillElementResistTime <= 0)?FALSE:TRUE;
}

BOOL  TMonsterSkillElementInfo::CheckModifyStatTime()
{
	return (this->m_iSkillElementModifyStatTime <= 0)?FALSE:TRUE;
}

BOOL  TMonsterSkillElementInfo::CheckImmuneAllTime()
{
	return (this->m_iSkillElementImmuneAllTime <= 0) ? FALSE : TRUE;
}
////////////////////////////////////////////////////////////////

BOOL  TMonsterSkillElementInfo::DecDefenseTime()
{
	this->m_iSkillElementDefense--;
	return (this->m_iSkillElementDefense > 0)?FALSE:TRUE;
}

BOOL  TMonsterSkillElementInfo::DecAttackTime()
{
	this->m_iSkillElementAttack--;
	return (this->m_iSkillElementAttack > 0)?FALSE:TRUE;
}

BOOL  TMonsterSkillElementInfo::DecAutoHPTime()
{
	this->m_iSkillElementAutoHPTime--;
	return (this->m_iSkillElementAutoHPTime > 0)?FALSE:TRUE;
}

BOOL  TMonsterSkillElementInfo::DecAutoMPTime()
{
	this->m_iSkillElementAutoMPTime--;
	return (this->m_iSkillElementAutoMPTime > 0)?FALSE:TRUE;
}

BOOL  TMonsterSkillElementInfo::DecAutoAGTime()
{
	this->m_iSkillElementAutoAGTime--;
	return (this->m_iSkillElementAutoAGTime > 0)?FALSE:TRUE;
}

BOOL  TMonsterSkillElementInfo::DecImmuneTime()
{
	this->m_iSkillElementImmuneTime--;
	return (this->m_iSkillElementImmuneTime > 0)?FALSE:TRUE;
}

BOOL  TMonsterSkillElementInfo::DecResistTime()
{
	this->m_iSkillElementResistTime--;
	return (this->m_iSkillElementResistTime > 0)?FALSE:TRUE;
}

BOOL  TMonsterSkillElementInfo::DecModifyStatTime()
{
	this->m_iSkillElementModifyStatTime--;
	return (this->m_iSkillElementModifyStatTime > 0)?FALSE:TRUE;
}

BOOL  TMonsterSkillElementInfo::DecImmuneAllTime()
{
	this->m_iSkillElementImmuneAllTime--;
	return (this->m_iSkillElementImmuneAllTime > 0) ? FALSE : TRUE;
}



////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

