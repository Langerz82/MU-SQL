// RaklionSelupan.cpp: implementation of the CRaklionSelupan class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "RaklionSelupan.h"
#include "RaklionMonsterMng.h"
#include "User/CUserData.h"
#include "gObjMonster.h"
#include "TMonsterSkillManager.h"
#include "Logging/Log.h"
#include "configread.h"
#include "MagicInf.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRaklionSelupan::CRaklionSelupan()
{
	Init();
}

CRaklionSelupan::~CRaklionSelupan()
{

}

void CRaklionSelupan::Init()
{
	m_iSelupanObjIndex = -1;
	m_iNowBerserkLevel = 0;
	m_iSelupanMinDamage = 0;
	m_iSelupanMaxDamage = 0;
}

void CRaklionSelupan::RaklionSelupanAct_FirstSkill()
{
	int iSelupanIndex = GetSelupanObjIndex();
	int iTargetIndex = GetTargetOfSelupan();
	CGameObject lpObj = &getGameObject(iSelupanIndex);

	if( iTargetIndex == -1 )
	{
		iTargetIndex = iSelupanIndex;
	}
	else
	{
		CMagicInf cMagicInf;
		memset(&cMagicInf, 0, sizeof(cMagicInf));
		cMagicInf.m_Skill = 253;
		TMonsterSkillManager::UseMonsterSkill(iSelupanIndex, iTargetIndex, 31, 0, &cMagicInf);
	}

	sLog->outBasic("[ RAKLION ][ FirstSkill ] Skill Using : Index(%d) %s", iSelupanIndex, Obj.Name);
}

void CRaklionSelupan::RaklionSelupanAct_PoisonAttack()
{
	int iSelupanIndex = GetSelupanObjIndex();
	int iTargetIndex = GetTargetOfSelupan();
	CGameObject lpObj = &getGameObject(iSelupanIndex);

	if( iTargetIndex == -1 )
	{
		iTargetIndex = iSelupanIndex;
		TMonsterSkillManager::UseMonsterSkill(iSelupanIndex, iTargetIndex, 1, 7, NULL);
	}
	else
	{
		CMagicInf cMagicInf;
		memset(&cMagicInf, 0, sizeof(cMagicInf));
		cMagicInf.m_Skill = 250;
		TMonsterSkillManager::UseMonsterSkill(iSelupanIndex, iTargetIndex, 1, 1, &cMagicInf);
	}

	sLog->outBasic("[ RAKLION ][ PoisonAttack ] Skill Using : Index(%d) %s", iSelupanIndex, Obj.Name);
}

void CRaklionSelupan::RaklionSelupanAct_IceStorm()
{
	int iSelupanIndex = GetSelupanObjIndex();
	int iTargetIndex = GetTargetOfSelupan();
	CGameObject lpObj = &getGameObject(iSelupanIndex);

	if( iTargetIndex == -1 )
	{
		iTargetIndex = iSelupanIndex;
		TMonsterSkillManager::UseMonsterSkill(iSelupanIndex, iTargetIndex, 1, 7, NULL);
	}
	else
	{
		CMagicInf cMagicInf;
		memset(&cMagicInf, 0, sizeof(cMagicInf));
		cMagicInf.m_Skill = 251;
		TMonsterSkillManager::UseMonsterSkill(iSelupanIndex, iTargetIndex, 1, 2, &cMagicInf);
	}

	sLog->outBasic("[ RAKLION ][ IceStorm ] Skill Using : Index(%d) %s", iSelupanIndex, Obj.Name);
}

void CRaklionSelupan::RaklionSelupanAct_IceStrike()
{
	int iSelupanIndex = GetSelupanObjIndex();
	int iTargetIndex = GetTargetOfSelupan();
	CGameObject lpObj = &getGameObject(iSelupanIndex);

	if( iTargetIndex == -1 )
	{
		iTargetIndex = iSelupanIndex;
		TMonsterSkillManager::UseMonsterSkill(iSelupanIndex, iTargetIndex, 1, 7, NULL);
	}
	else
	{
		CMagicInf cMagicInf;
		memset(&cMagicInf, 0, sizeof(cMagicInf));
		cMagicInf.m_Skill = 252;
		TMonsterSkillManager::UseMonsterSkill(iSelupanIndex, iTargetIndex, 1, 3, &cMagicInf);
	}

	sLog->outBasic("[ RAKLION ][ IceStrike ] Skill Using : Index(%d) %s", iSelupanIndex, Obj.Name);
}

void CRaklionSelupan::RaklionSelupanAct_SummonMonster()
{
	int iSelupanIndex = GetSelupanObjIndex();
	int iTargetIndex = iSelupanIndex;
	CGameObject lpObj = &getGameObject(iSelupanIndex);

	TMonsterSkillManager::UseMonsterSkill(iSelupanIndex, iTargetIndex, 30, 4, NULL);
	
	RegenSummonMonster();
	
	sLog->outBasic("[ RAKLION ][ SummonMonster ] Skill Using : Index(%d) %s", iSelupanIndex, Obj.Name);
}

void CRaklionSelupan::RaklionSelupanAct_Heal()
{
	int iSelupanIndex = GetSelupanObjIndex();
	int iTargetIndex = iSelupanIndex;
	CGameObject lpObj = &getGameObject(iSelupanIndex);

	TMonsterSkillManager::UseMonsterSkill(iSelupanIndex, iTargetIndex, 20, 5, NULL);

	sLog->outBasic("[ RAKLION ][ Heal ] Skill Using : Index(%d) %s", iSelupanIndex, Obj.Name);
}

void CRaklionSelupan::RaklionSelupanAct_Freeze()
{
	int iSelupanIndex = GetSelupanObjIndex();
	int iTargetIndex = GetTargetOfSelupan();
	CGameObject lpObj = &getGameObject(iSelupanIndex);

	if( iTargetIndex == -1 )
	{
		iTargetIndex = iSelupanIndex;
		TMonsterSkillManager::UseMonsterSkill(iSelupanIndex, iTargetIndex, 1, 7, NULL);
	}
	else TMonsterSkillManager::UseMonsterSkill(iSelupanIndex, iTargetIndex, 1, 6, NULL);

	sLog->outBasic("[ RAKLION ][ Freeze ] Skill Using : Index(%d) %s", iSelupanIndex, Obj.Name);
}

void CRaklionSelupan::RaklionSelupanAct_Teleport()
{
	int iSelupanIndex = GetSelupanObjIndex();
	int iTargetIndex = GetTargetOfSelupan();
	CGameObject lpObj = &getGameObject(iSelupanIndex);
	CGameObject  lpObjTarget = &getGameObject(iTargetIndex);

	if( iTargetIndex < 0 )	iTargetIndex = iSelupanIndex;

	TMonsterSkillManager::UseMonsterSkill(iSelupanIndex, iTargetIndex, 1, 7, NULL);
	sLog->outBasic("[ RAKLION ][ Teleport ] Skill Using : Index(%d) %s", iSelupanIndex, Obj.Name);
}

void CRaklionSelupan::RaklionSelupanAct_Invincibility()
{
	int iSelupanIndex = GetSelupanObjIndex();
	int iTargetIndex = iSelupanIndex;
	CGameObject lpObj = &getGameObject(iSelupanIndex);

	TMonsterSkillManager::UseMonsterSkill(iSelupanIndex, iTargetIndex, 10, 8, NULL);
	
	sLog->outBasic("[ RAKLION ][ Invincibility ] Skill Using : Index(%d) %s", iSelupanIndex, Obj.Name);
}

void CRaklionSelupan::RaklionSelupanAct_Berserk_Cancel()
{
	int iSelupanIndex = GetSelupanObjIndex();
	CGameObject lpObj = &getGameObject(iSelupanIndex);

	Obj.m_AttackDamageMin = m_iSelupanMinDamage;
	Obj.m_AttackDamageMax = m_iSelupanMaxDamage;
	
	sLog->outBasic("[ RAKLION ][ Berserk Cancel ] MinDamage(%d) Damage(%d~%d)", Obj.m_AttackDamageMin, Obj.m_AttackDamageMin, Obj.m_AttackDamageMax);
}

void CRaklionSelupan::RaklionSelupanAct_Berserk1()
{
	int iSelupanIndex = GetSelupanObjIndex();
	int iTargetIndex = iSelupanIndex;
	CGameObject lpObj = &getGameObject(iSelupanIndex);
	
	TMonsterSkillManager::UseMonsterSkill(iSelupanIndex, iTargetIndex, 1, 9, NULL);
	
	m_iNowBerserkLevel = 1;

	int iIncreaseAttackValue = 0;
	iIncreaseAttackValue = m_iNowBerserkLevel * Obj.m_MonsterSkillElementInfo.m_iSkillElementAttack;

	Obj.m_AttackDamageMin = m_iSelupanMinDamage + iIncreaseAttackValue;
	Obj.m_AttackDamageMax = m_iSelupanMaxDamage + iIncreaseAttackValue;

	sLog->outBasic("[ RAKLION ][ Berserk1 ] Skill Using : Index(%d) Damage(%d~%d)", iSelupanIndex, Obj.m_AttackDamageMin, Obj.m_AttackDamageMax);
}

void CRaklionSelupan::RaklionSelupanAct_Berserk2()
{
	int iSelupanIndex = GetSelupanObjIndex();
	int iTargetIndex = iSelupanIndex;
	CGameObject lpObj = &getGameObject(iSelupanIndex);
	
	TMonsterSkillManager::UseMonsterSkill(iSelupanIndex, iTargetIndex, 1, 9, NULL);
	
	m_iNowBerserkLevel = 2;

	int iIncreaseAttackValue = 0;
	iIncreaseAttackValue = m_iNowBerserkLevel * Obj.m_MonsterSkillElementInfo.m_iSkillElementAttack;

	Obj.m_AttackDamageMin = m_iSelupanMinDamage + iIncreaseAttackValue;
	Obj.m_AttackDamageMax = m_iSelupanMaxDamage + iIncreaseAttackValue;

	sLog->outBasic("[ RAKLION ][ Berserk2 ] Skill Using : Index(%d) Damage(%d~%d)", iSelupanIndex, Obj.m_AttackDamageMin, Obj.m_AttackDamageMax);
}

void CRaklionSelupan::RaklionSelupanAct_Berserk3()
{
	int iSelupanIndex = GetSelupanObjIndex();
	int iTargetIndex = iSelupanIndex;
	CGameObject lpObj = &getGameObject(iSelupanIndex);
	
	TMonsterSkillManager::UseMonsterSkill(iSelupanIndex, iTargetIndex, 1, 9, NULL);
	
	m_iNowBerserkLevel = 3;

	int iIncreaseAttackValue = 0;
	iIncreaseAttackValue = m_iNowBerserkLevel * Obj.m_MonsterSkillElementInfo.m_iSkillElementAttack;

	Obj.m_AttackDamageMin = m_iSelupanMinDamage + iIncreaseAttackValue;
	Obj.m_AttackDamageMax = m_iSelupanMaxDamage + iIncreaseAttackValue;

	sLog->outBasic("[ RAKLION ][ Berserk3 ] Skill Using : Index(%d) Damage(%d~%d)", iSelupanIndex, Obj.m_AttackDamageMin, Obj.m_AttackDamageMax);
}

void CRaklionSelupan::RaklionSelupanAct_Berserk4()
{
	int iSelupanIndex = GetSelupanObjIndex();
	int iTargetIndex = iSelupanIndex;
	CGameObject lpObj = &getGameObject(iSelupanIndex);
	
	TMonsterSkillManager::UseMonsterSkill(iSelupanIndex, iTargetIndex, 1, 9, NULL);
	
	m_iNowBerserkLevel = 4;

	int iIncreaseAttackValue = 0;
	iIncreaseAttackValue = m_iNowBerserkLevel * Obj.m_MonsterSkillElementInfo.m_iSkillElementAttack;

	Obj.m_AttackDamageMin = m_iSelupanMinDamage + iIncreaseAttackValue;
	Obj.m_AttackDamageMax = m_iSelupanMaxDamage + iIncreaseAttackValue;

	sLog->outBasic("[ RAKLION ][ Berserk4 ] Skill Using : Index(%d) Damage(%d~%d)", iSelupanIndex, Obj.m_AttackDamageMin, Obj.m_AttackDamageMax);
}

DWORD CRaklionSelupan::GetSelupanHP()
{
	int iSelupanIndex = GetSelupanObjIndex();
	CGameObject lpObj = &getGameObject(iSelupanIndex);
	return (DWORD)Obj.Life;
}

void CRaklionSelupan::SetSelupanHP(int iSelupanHPPercent)
{
	int iSelupanIndex = GetSelupanObjIndex();
	CGameObject lpObj = &getGameObject(iSelupanIndex);
	float iSelupanMAXLife = Obj.AddLife + Obj.MaxLife;
	Obj.Life = iSelupanMAXLife * iSelupanHPPercent / 100;
}

void CRaklionSelupan::InitSelupanIndex()
{
	for (int iCount = 0; iCount < g_ConfigRead.server.GetObjectMaxMonster(); iCount++)
	{
		if (getGameObject(iCount)->Connected == PLAYER_PLAYING && getGameObject(iCount)->Class == 459)
		{
			this->m_iSelupanObjIndex = iCount;
		}
	}

	if (this->m_iSelupanObjIndex == -1)
	{
		sLog->outError( "[Raklion][InitSelupanIndex] error : SelupanObjIndex(%d)", this->m_iSelupanObjIndex);
	}
}

BOOL CRaklionSelupan::RegenSelupan()
{
	int iSelupanPosNum = -1;

	for (int i = 0; i < g_RaklionMonsterMng.m_iMaxMonsterCount; i++)
	{
		if (g_RaklionMonsterMng.m_SetBaseInfo[i].wType == 459)
		{
			iSelupanPosNum = i;
		}
	}

	int result = gObjAddMonster(MAP_INDEX_HATCHERY);

	if (result < 0)
	{
		sLog->outBasic("[RAKLION] CreateSelupan error");
		return FALSE;
	}

	g_RaklionMonsterMng.SetPosition(result, iSelupanPosNum);
	gObjSetMonster(result, 459);

	CGameObject lpObj = &getGameObject(result);

	gObjMonsterHitDamageInit(lpObj);

	getGameObject(result)->DieRegen = 0;
	getGameObject(result)->m_State = 1;

	sLog->outBasic("[RAKLION] Create Selupan : X (%d) / Y(%d)", Obj.X, Obj.Y);

	CGameObject lpObjUser = NULL;

	for (int n = 0; n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		lpObjUser = &getGameObject(n);
		if (lpObjUser->MapNumber == MAP_INDEX_HATCHERY)
		{
			gObjStateSetCreate(n);
		}
	}

	for (int n = 0; n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		lpObjUser = &getGameObject(n);
		if (lpObjUser->MapNumber == MAP_INDEX_HATCHERY)
		{
			gObjViewportListDestroy(n);
		}
	}

	for (int n = 0; n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		lpObjUser = &getGameObject(n);
		if (lpObjUser->MapNumber == MAP_INDEX_HATCHERY)
		{
			gObjViewportListCreate(n);
		}
	}

	for (int n = 0; n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		lpObjUser = &getGameObject(n);
		if (lpObjUser->MapNumber == MAP_INDEX_HATCHERY)
		{
			gObjViewportListProtocol(n);
		}
	}

	gObjSetState();

	m_iSelupanMinDamage = Obj.m_AttackDamageMin;
	m_iSelupanMaxDamage = Obj.m_AttackDamageMax;
	return TRUE;
}

void CRaklionSelupan::HideSelupan()
{
	int iSelupanIndex = GetSelupanObjIndex();

	if (iSelupanIndex != -1)
	{
		gObjViewportListProtocolDestroy(&getGameObject(iSelupanIndex));
		gObjViewportClose(&getGameObject(iSelupanIndex));
		gObjDel(iSelupanIndex);
	}
}

int CRaklionSelupan::GetTargetOfSelupan()
{
	int iSelupanIndex = GetSelupanObjIndex();
	CGameObject lpObj = &getGameObject(iSelupanIndex);

	if( Obj.TargetNumber >= 0 && Obj.TargetNumber < g_ConfigRead.server.GetObjectMax()) 
	{
		return Obj.TargetNumber;
	}

	return gObjMonsterSearchEnemy(lpObj, Obj.Type);
}

void CRaklionSelupan::SetSelupanSkillDelay(int iDelay)
{
	if( iDelay < 0 || iDelay > (1000*50) )
	{
		sLog->outError( "[ RAKLION ][ SetSelupanSkillDelay ] SkillDelay error : Delay(%d)", m_iSkillDelay);
		return;
	}
	m_iSkillDelay = iDelay;
}

int CRaklionSelupan::GetSelupanSkillDelay()
{
	if( m_iSkillDelay < 0 || m_iSkillDelay > 50000 )
	{
		sLog->outError( "[ RAKLION ][ GetSelupanSkillDelay ] SkillDelay error : Delay(%d)", m_iSkillDelay);
		return -1;
	}
	return m_iSkillDelay;
}

int CRaklionSelupan::GetBerserkLevel()
{
	return m_iNowBerserkLevel;
}

void CRaklionSelupan::InitSelupanDamage(int iMinDamage,int iMaxDamage)
{
	m_iSelupanMinDamage = iMinDamage;
	m_iSelupanMaxDamage = iMaxDamage;
}

void CRaklionSelupan::SetSelupanObjIndex(CGameObject &Obj)
{
	if( iIndex < 0 || iIndex > g_ConfigRead.server.GetObjectMaxMonster() ) 
	{
		sLog->outError( "[ RAKLION ][ SetSelupanObjIndex ] SelupanObjIndex error : Index(%d)", iIndex);
		return;
	}
	m_iSelupanObjIndex = iIndex;
}

int CRaklionSelupan::GetSelupanObjIndex()
{
	InitSelupanIndex();
	return m_iSelupanObjIndex;
}

void CRaklionSelupan::RegenSummonMonster()
{
	CGameObject lpObj = NULL;
	int iPosTableNum = -1;
	int iClass = -1;

	for (int iCount = 0; iCount < g_ConfigRead.server.GetObjectMaxMonster(); iCount++)
	{
		lpObj = &getGameObject(iCount);

		if (Obj.MapNumber != MAP_INDEX_HATCHERY)	continue;

		if (Obj.Class == 457 || Obj.Class == 458)
		{
			if (Obj.Connected == PLAYER_PLAYING &&
				Obj.DieRegen == 0 &&
				Obj.Live == 1)	continue;

			iClass = Obj.Class;
			iPosTableNum = Obj.m_PosNum;

			gObjCharZeroSet(iCount);
			gObjDel(Obj.m_Index);

			Obj.Class = 0;

			int result = gObjAddMonster(MAP_INDEX_HATCHERY);

			lpObj = &getGameObject(result);

			if (result < 0)
			{
				sLog->outBasic("[RAKLION] CreateSelupan error");
				return;
			}

			g_RaklionMonsterMng.SetPosition(result, iPosTableNum);
			gObjSetMonster(result, iClass);
			Obj.Live = 1;
			Obj.DieRegen = 0;
			sLog->outBasic("[RAKLION] Summoned Monster : %s => X-(%d) Y-(%d)", Obj.Name, Obj.X, Obj.Y);
		}
	}
}

void CRaklionSelupan::DeleteSummonMonster()
{
	CGameObject lpObj = NULL;

	for(int iCount=0; iCount<g_ConfigRead.server.GetObjectMaxMonster(); iCount++)
	{
		lpObj = &getGameObject(iCount);

		if( Obj.MapNumber != MAP_INDEX_HATCHERY )	continue;	
		if( Obj.Class == 457 || Obj.Class == 458 )
		{
			gObjViewportListProtocolDestroy(lpObj);
			gObjViewportClose(lpObj);
			gObjDel(Obj.m_Index);
			Obj.Live = 0; 
			Obj.DieRegen = 1;
		}
	}

}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

