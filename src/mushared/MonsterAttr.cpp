////////////////////////////////////////////////////////////////////////////////
// MonsterAttr.cpp
#include "stdafx.h"
#include "MonsterAttr.h"
#include "TLog.h"
#include "user.h"

// GS-N 0.99.60T 0x00418C10 - Completed - Just few disorders with same result int LoadAttr
//	GS-N	1.00.18	JPN	0x0041F300	-	Completed

CMonsterAttr::CMonsterAttr()
{
	return;
}

CMonsterAttr::~CMonsterAttr()
{
	return;
}

LPMONSTER_ATTRIBUTE CMonsterAttr::GetAttr(int aClass)
{
	for ( int n = 0; n<MAX_MONSTER_TYPE;n++)
	{
		if ( this->m_MonsterAttr[n].m_Index == aClass )
		{
			return &this->m_MonsterAttr[n];
		}
	}

	return NULL;
}

void CMonsterAttr::LoadAttr(char* filename)
{
	pugi::xml_document file;
	pugi::xml_parse_result result = file.load_file(filename);

	if (result.status != pugi::status_ok)
	{
		g_Log.MsgBox("load error %s (%s)", filename, result.description());
		return;
	}

	pugi::xml_node main_section = file.child("MonsterList");
	int nCount = 0;

	for (pugi::xml_node monster = main_section.child("Monster"); monster; monster = monster.next_sibling())
	{
		this->m_MonsterAttr[nCount].m_Index = monster.attribute("Index").as_int(-1);
		this->m_MonsterAttr[nCount].m_btExpType = monster.attribute("ExpType").as_int(0);
		strcpy(this->m_MonsterAttr[nCount].m_Name, monster.attribute("Name").as_string(""));
		this->m_MonsterAttr[nCount].m_Level = monster.attribute("Level").as_int(0);
		this->m_MonsterAttr[nCount].m_Hp = monster.attribute("HP").as_int(0);
		this->m_MonsterAttr[nCount].m_Mp = monster.attribute("MP").as_int(0);
		this->m_MonsterAttr[nCount].m_DamageMin = monster.attribute("DamageMin").as_int(0);
		this->m_MonsterAttr[nCount].m_DamageMax = monster.attribute("DamageMax").as_int(0);
		this->m_MonsterAttr[nCount].m_Defense = monster.attribute("Defense").as_int(0);
		this->m_MonsterAttr[nCount].m_MagicDefense = monster.attribute("MagicDefense").as_int(0);
		this->m_MonsterAttr[nCount].m_AttackRating = monster.attribute("AttackRate").as_int(0);
		this->m_MonsterAttr[nCount].m_Successfulblocking = monster.attribute("BlockRate").as_int(0);
		this->m_MonsterAttr[nCount].m_MoveRange = monster.attribute("MoveRange").as_int(0);
		this->m_MonsterAttr[nCount].m_AttackType = monster.attribute("AttackType").as_int(0);
		this->m_MonsterAttr[nCount].m_AttackRange = monster.attribute("AttackRange").as_int(0);
		this->m_MonsterAttr[nCount].m_ViewRange = monster.attribute("ViewRange").as_int(0);
		this->m_MonsterAttr[nCount].m_MoveSpeed = monster.attribute("MoveSpeed").as_int(0);
		this->m_MonsterAttr[nCount].m_AttackSpeed = monster.attribute("AttackSpeed").as_int(0);
		this->m_MonsterAttr[nCount].m_RegenTime = monster.attribute("RegenTime").as_int(0);
		this->m_MonsterAttr[nCount].m_Attribute = monster.attribute("Attribute").as_int(0);
		this->m_MonsterAttr[nCount].m_ItemRate = monster.attribute("ItemDropRate").as_int(0);
		this->m_MonsterAttr[nCount].m_MoneyRate = monster.attribute("MoneyDropRate").as_int(0);
		this->m_MonsterAttr[nCount].m_MaxItemLevel = monster.attribute("MaxItemLevel").as_int(0);
		this->m_MonsterAttr[nCount].m_MonsterSkill = monster.attribute("MonsterSkill").as_int(0);
		this->m_MonsterAttr[nCount].m_Resistance[R_ICE] = monster.attribute("IceRes").as_int(0);
		this->m_MonsterAttr[nCount].m_Resistance[R_POISON] = monster.attribute("PoisonRes").as_int(0);
		this->m_MonsterAttr[nCount].m_Resistance[R_LIGHTNING] = monster.attribute("LightRes").as_int(0);
		this->m_MonsterAttr[nCount].m_Resistance[R_FIRE] = monster.attribute("FireRes").as_int(0);
		this->m_MonsterAttr[nCount].m_PentagramMainAttribute = monster.attribute("PentagramMainAttrib").as_int(0);
		this->m_MonsterAttr[nCount].m_PentagramAttributePattern = monster.attribute("PentagramAttribPattern").as_int(0);
		this->m_MonsterAttr[nCount].m_PentagramAttackMin = monster.attribute("PentagramDamageMin").as_int(0);
		this->m_MonsterAttr[nCount].m_PentagramAttackMax = monster.attribute("PentagramDamageMax").as_int(0);
		this->m_MonsterAttr[nCount].m_PentagramAttackRating = monster.attribute("PentagramAttackRate").as_int(0);
		this->m_MonsterAttr[nCount].m_PentagramDefenseRating = monster.attribute("PentagramDefenseRate").as_int(0);
		this->m_MonsterAttr[nCount].m_PentagramDefense = monster.attribute("PentagramDefense").as_int(0);

		if (this->m_MonsterAttr[nCount].m_PentagramMainAttribute > 0)
		{
			this->m_MonsterAttr[nCount].m_Resistance[R_ICE] = 0;
			this->m_MonsterAttr[nCount].m_Resistance[R_POISON] = 0;
			this->m_MonsterAttr[nCount].m_Resistance[R_LIGHTNING] = 0;
			this->m_MonsterAttr[nCount].m_Resistance[R_FIRE] = 0;
		}

		this->m_MonsterAttr[nCount].m_iScriptHP = this->m_MonsterAttr[nCount].m_Hp;

		nCount++;
	}
}

#include "BuffEffect.h"

CCustomMonsterAttr gCMAttr;

CCustomMonsterAttr::CCustomMonsterAttr()
{

}

CCustomMonsterAttr::~CCustomMonsterAttr()
{
	if (!m_MonsterData.empty())
	{
		std::map<int, CUSTOM_MONSTER_ATTR*>::iterator _It;

		for (_It = m_MonsterData.begin(); _It != m_MonsterData.end(); _It++) {
			if (_It->second)
				delete _It->second;
		}
	}
}

void CCustomMonsterAttr::Load(char* szFileName)
{
	pugi::xml_document Document;
	pugi::xml_parse_result Result = Document.load_file(szFileName);
	// ----
	if (Result.status != pugi::status_ok)
	{
		g_Log.MsgBox("[MonsterStatInfo] File %s not found!", szFileName);
		return;
	}
	// ----
	if (!m_MonsterData.empty())
	{
		std::map<int, CUSTOM_MONSTER_ATTR*>::iterator _It;

		for (_It = m_MonsterData.begin(); _It != m_MonsterData.end(); _It++) {
			if (_It->second)
				delete _It->second;
		}
	}
	m_MonsterData.clear();
	// ----
	pugi::xml_node MonsterStatInfo = Document.child("monsterstatinfo");
	// ----
	this->m_DefaultData.HPMul = MonsterStatInfo.child("default").attribute("hp").as_float();
	this->m_DefaultData.DmgMul = MonsterStatInfo.child("default").attribute("damage").as_float();
	this->m_DefaultData.DefMul = MonsterStatInfo.child("default").attribute("defense").as_float();
	this->m_DefaultData.ARateMul = MonsterStatInfo.child("default").attribute("attackrate").as_float();
	this->m_DefaultData.DRateMul = MonsterStatInfo.child("default").attribute("defenserate").as_float();
	// ----
	for (pugi::xml_node Node = MonsterStatInfo.child("specific").child("monster"); Node; Node = Node.next_sibling())
	{
		CUSTOM_MONSTER_ATTR* pTmp = new CUSTOM_MONSTER_ATTR;
		int MonIndex = Node.attribute("id").as_int();
		pTmp->HPMul = Node.attribute("hp").as_float();
		pTmp->DmgMul = Node.attribute("damage").as_float();
		pTmp->DefMul = Node.attribute("defense").as_float();
		pTmp->ARateMul = Node.attribute("attackrate").as_float();
		pTmp->DRateMul = Node.attribute("defenserate").as_float();
		m_MonsterData.insert(std::pair<int, CUSTOM_MONSTER_ATTR*>(MonIndex, pTmp));
	}
}


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

