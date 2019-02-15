////////////////////////////////////////////////////////////////////////////////
// MonsterStatCalc.cpp
#include "StdAfx.h"
#include "MonsterStatCalc.h"
#include "Logging/Log.h"
#include "MonsterAttr.h"
#include "GameMain.h"

CMonsterStatCalc g_MonsterStatCalc;

CMonsterStatCalc::CMonsterStatCalc(void)
{
}

CMonsterStatCalc::~CMonsterStatCalc(void)
{
}

void CMonsterStatCalc::LoadScript(LPSTR Filename)
{
	this->m_bLoad = false;

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(Filename);

	if ( res.status != pugi::status_ok )
	{
		return;
	}

	pugi::xml_node mainXML = file.child("ImperialGuardian");
	_stMonsterStatRef monsterStatRef;

	for (pugi::xml_node monster = mainXML.child("Monster"); monster; monster = monster.next_sibling())
	{
		monsterStatRef.m_nMinLevel = monster.attribute("PlayerMinLevel").as_int();
		monsterStatRef.m_nMaxLevel = monster.attribute("PlayerMaxLevel").as_int();
		monsterStatRef.m_nLevel = monster.attribute("Level").as_int();
		monsterStatRef.m_nHp = monster.attribute("HealthPoint").as_int();
		monsterStatRef.m_nMp = monster.attribute("ManaPoint").as_int();
		monsterStatRef.m_nDamageMin = monster.attribute("DamageMin").as_int();
		monsterStatRef.m_nDamageMax = monster.attribute("DamageMax").as_int();
		monsterStatRef.m_nDefense = monster.attribute("Defense").as_int();
		monsterStatRef.m_nMagicDefense = monster.attribute("MagicDefense").as_int();
		monsterStatRef.m_nAttackRating = monster.attribute("AttackRate").as_int();
		monsterStatRef.m_nSuccessfulblocking = monster.attribute("SuccessfulBlocking").as_int();

		this->m_vtMonsterStatRefAttr.push_back(monsterStatRef);
	}

	this->m_bLoad = true;

}

BOOL CMonsterStatCalc::ConvertMonsterStat(CGameObject &Obj, int nUserMaxLevel)
{
	if ( this->m_bLoad == FALSE )
	{
		return FALSE;
	}

	if ( lpObj == NULL )
	{
		return FALSE;
	}

	LPMONSTER_ATTRIBUTE lpAttr = gMAttr.GetAttr(Obj.Class);

	if ( lpAttr == NULL )
	{
		return FALSE;
	}

	if ( lpAttr->m_Level == 0 )
	{
		return FALSE;
	}

	int nSize = this->m_vtMonsterStatRefAttr.size();
	_stMonsterStatRef monsterStatRef;

	for ( int i=0; i<nSize; i++ )
	{
		monsterStatRef = this->m_vtMonsterStatRefAttr.at(i);

		if ( nUserMaxLevel >= monsterStatRef.m_nMinLevel && nUserMaxLevel <= monsterStatRef.m_nMaxLevel )
		{
			strncpy(Obj.Name, lpAttr->m_Name, MAX_ACCOUNT_LEN);
			Obj.Level = lpAttr->m_Level * monsterStatRef.m_nLevel;
			Obj.Life = lpAttr->m_Hp * monsterStatRef.m_nHp;
			Obj.MaxLife = lpAttr->m_Hp * monsterStatRef.m_nHp;
			Obj.Mana = lpAttr->m_Mp * monsterStatRef.m_nMp;
			Obj.MaxMana = lpAttr->m_Mp * monsterStatRef.m_nMp;
			Obj.m_AttackDamageMin = lpAttr->m_DamageMin * monsterStatRef.m_nDamageMin;
			Obj.m_AttackDamageMax = lpAttr->m_DamageMax * monsterStatRef.m_nDamageMax;
			Obj.m_Defense = lpAttr->m_Defense * monsterStatRef.m_nDefense;
			Obj.m_MagicDefense = lpAttr->m_MagicDefense * monsterStatRef.m_nMagicDefense;
			Obj.m_AttackRating = lpAttr->m_AttackRating * monsterStatRef.m_nAttackRating;
			Obj.m_SuccessfulBlocking = lpAttr->m_Successfulblocking * monsterStatRef.m_nSuccessfulblocking;
			Obj.m_MoveRange = lpAttr->m_MoveRange;
			Obj.m_AttackSpeed = lpAttr->m_AttackSpeed;
			Obj.m_MoveSpeed = lpAttr->m_MoveSpeed;
			Obj.MaxRegenTime = 1000 * lpAttr->m_RegenTime;
			Obj.m_AttackRange = lpAttr->m_AttackRange;
			Obj.m_ViewRange = lpAttr->m_ViewRange;
			Obj.m_Attribute = lpAttr->m_Attribute;
			Obj.m_AttackType = lpAttr->m_AttackType;
			Obj.m_ItemRate = lpAttr->m_ItemRate;
			Obj.m_MoneyRate = lpAttr->m_MoneyRate;
			Obj.m_Resistance[R_ICE] = lpAttr->m_Resistance[R_ICE];
			Obj.m_Resistance[R_POISON] = lpAttr->m_Resistance[R_POISON];
			Obj.m_Resistance[R_LIGHTNING] = lpAttr->m_Resistance[R_LIGHTNING];
			Obj.m_Resistance[R_FIRE] = lpAttr->m_Resistance[R_FIRE];
			Obj.m_iScriptMaxLife = lpAttr->m_iScriptHP;
			Obj.m_iPentagramMainAttribute = lpAttr->m_PentagramMainAttribute;
			Obj.m_iPentagramAttributePattern = lpAttr->m_PentagramAttributePattern;
			Obj.m_iPentagramDefense = lpAttr->m_PentagramDefense;
			Obj.m_iPentagramAttackMin = lpAttr->m_PentagramAttackMin;
			Obj.m_iPentagramAttackMax = lpAttr->m_PentagramAttackMax;
			Obj.m_iPentagramAttackRating = lpAttr->m_PentagramAttackRating;
			Obj.m_iPentagramDefenseRating = lpAttr->m_PentagramDefenseRating;
			Obj.m_iBasicAI = lpAttr->m_iAINumber;
			Obj.m_iCurrentAI = lpAttr->m_iAINumber;
			Obj.m_iCurrentAIState = 0;
			Obj.m_iLastAIRunTime = 0;
			Obj.m_iGroupNumber = 0;
			Obj.m_iSubGroupNumber = 0;
			Obj.m_iGroupMemberGuid = -1;
			Obj.m_iRegenType = 0;

			Obj.m_Agro->ResetAll();

			Obj.m_iLastAutomataRuntime = 0;
			Obj.m_iLastAutomataDelay = 0;

			memcpy(Obj.m_Resistance, lpAttr->m_Resistance, MAX_RESISTENCE_TYPE );

			return TRUE;
		}
	}
	
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

