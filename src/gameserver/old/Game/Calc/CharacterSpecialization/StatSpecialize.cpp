////////////////////////////////////////////////////////////////////////////////
// StatSpecialize.cpp
#include "StdAfx.h"
#include "StatSpecialize.h"
#include "Logging/Log.h"
#include "configread.h"
#include "util.h"
#include "Main.h"

CStatSpecialize g_StatSpec;

CStatSpecialize::CStatSpecialize(void) : m_Lua(true)
{
	memset(this->m_StatOptions, 0x00, sizeof(this->m_StatOptions));
}

CStatSpecialize::~CStatSpecialize(void)
{
}

bool CStatSpecialize::LoadFile(char *szFile)
{
	this->m_bFileLoad = false;

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(szFile);

	if (res.status != pugi::status_ok)
	{
		sLog->outError("Failed to load %s file (%s)", szFile, res.description());
		return false;
	}

	pugi::xml_node mainXML = file.child("StatSpecialize");
	int Count = 0;

	for(pugi::xml_node option = mainXML.child("Option"); option; option = option.next_sibling())
	{
		this->m_StatOptions[Count].StatOptionID = option.attribute("ID").as_int();
		this->m_StatOptions[Count].StatOptionClass[CLASS_KNIGHT] = option.attribute("DK").as_bool();
		this->m_StatOptions[Count].StatOptionClass[CLASS_WIZARD] = option.attribute("DW").as_bool();
		this->m_StatOptions[Count].StatOptionClass[CLASS_ELF] = option.attribute("Elf").as_bool();
		this->m_StatOptions[Count].StatOptionClass[CLASS_MAGUMSA] = option.attribute("MG").as_bool();
		this->m_StatOptions[Count].StatOptionClass[CLASS_DARKLORD] = option.attribute("DL").as_bool();
		this->m_StatOptions[Count].StatOptionClass[CLASS_SUMMONER] = option.attribute("SU").as_bool();
		this->m_StatOptions[Count].StatOptionClass[CLASS_RAGEFIGHTER] = option.attribute("RF").as_bool();
		this->m_StatOptions[Count].StatOptionClass[CLASS_GROWLANCER] = option.attribute("GL").as_bool();
		Count++;

		if (Count >= MAX_STAT_OPTIONS)
		{
			break;
		}
	}

	this->m_Lua.DoFile(g_ConfigRead.GetPath("\\Scripts\\Specialization\\StatSpec.lua"));

	this->m_bFileLoad = true;
	return true;
}

void CStatSpecialize::ClearUserOption(CGameObject &Obj)
{
	for (int i = 0; i < MAX_USER_STAT_OPTIONS; i++)
	{
		Obj.m_PlayerData->m_StatSpecOption[i].Clear();
	}
}

void CStatSpecialize::CalcStatOption(CGameObject &Obj, int StatOptionID)
{
	if (this->m_bFileLoad == false)
	{
		return;
	}

	double Percent = 0.0;
	this->m_Lua.Generic_Call("StatSpec_GetPercent", "iiiiiii>d", 
		(int)Obj.Class, StatOptionID, (int)Obj.m_PlayerData->Strength, (int)Obj.m_PlayerData->Dexterity,
		(int)Obj.m_PlayerData->Vitality, (int)Obj.m_PlayerData->Energy, (int)Obj.Leadership, &Percent);

	if (Percent == 0.0)
	{
		return;
	}

	this->SetStatOption(Obj, StatOptionID, Percent);
}

void CStatSpecialize::SetStatOption(CGameObject &Obj, int StatOptionID, double StatOptionPercent)
{
	int ArrayIndex = this->GetUserArrayIndex(Obj);

	if (ArrayIndex == -1)
	{
		return;
	}

	Obj.m_PlayerData->m_StatSpecOption[ArrayIndex].StatOptionID = StatOptionID;

	switch ( StatOptionID )
	{
		case STAT_OPTION_INC_ATTACK_POWER:
			{
				int iEffectValueMin = Obj.m_AttackDamageMinLeft * StatOptionPercent / 100.0;
				int iEffectValueMax = Obj.m_AttackDamageMaxLeft * StatOptionPercent / 100.0;
				Obj.m_AttackDamageMinLeft += iEffectValueMin;
				Obj.m_AttackDamageMinRight += iEffectValueMin;
				Obj.m_AttackDamageMaxLeft += iEffectValueMax;
				Obj.m_AttackDamageMaxRight += iEffectValueMax;
				Obj.m_PlayerData->m_StatSpecOption[ArrayIndex].StatOptionValMin = iEffectValueMin;
				Obj.m_PlayerData->m_StatSpecOption[ArrayIndex].StatOptionValMax = iEffectValueMax;
			}
			break;

		case STAT_OPTION_INC_ATTACK_RATE:
			{
				int iEffectValue = Obj.m_PlayerData->m_AttackRatePvM * StatOptionPercent / 100.0;
				Obj.m_PlayerData->m_AttackRatePvM += iEffectValue;
				Obj.m_PlayerData->m_StatSpecOption[ArrayIndex].StatOptionValMin = iEffectValue;
			}
			break;

		case STAT_OPTION_INC_ATTACK_RATE_PVP:
			{
				int iEffectValue = Obj.m_PlayerData->m_AttackRatePvP * StatOptionPercent / 100.0;
				Obj.m_PlayerData->m_AttackRatePvP += iEffectValue;
				Obj.m_PlayerData->m_StatSpecOption[ArrayIndex].StatOptionValMin = iEffectValue;
			}
			break;

		case STAT_OPTION_INC_DEFENSE:
			{
				int iEffectValue = Obj.m_Defense * StatOptionPercent / 100.0;
				Obj.m_Defense += iEffectValue;
				Obj.m_PlayerData->m_StatSpecOption[ArrayIndex].StatOptionValMin = iEffectValue;
			}
			break;

		case STAT_OPTION_INC_ATTACK_SPEED:
			{
				int iEffectValue = Obj.m_AttackSpeed * StatOptionPercent / 100.0;
				Obj.m_AttackSpeed += iEffectValue;
				Obj.m_PlayerData->m_StatSpecOption[ArrayIndex].StatOptionValMin = iEffectValue;
			}
			break;

		case STAT_OPTION_INC_DEFENSE_RATE:
			{
				int iEffectValue = Obj.m_SuccessfulBlocking * StatOptionPercent / 100.0;
				Obj.m_SuccessfulBlocking += iEffectValue;
				Obj.m_PlayerData->m_StatSpecOption[ArrayIndex].StatOptionValMin = iEffectValue;
			}
			break;

		case STAT_OPTION_INC_DEFENSE_RATE_PVP:
			{
				int iEffectValue = Obj.m_PlayerData->m_DefenseRatePvP * StatOptionPercent / 100.0;
				Obj.m_PlayerData->m_DefenseRatePvP += iEffectValue;
				Obj.m_PlayerData->m_StatSpecOption[ArrayIndex].StatOptionValMin = iEffectValue;
			}
			break;

		case STAT_OPTION_INC_MAGIC_DAMAGE:
			{
				int iEffectValueMin = Obj.m_MagicDamageMin * StatOptionPercent / 100.0;
				int iEffectValueMax = Obj.m_MagicDamageMax * StatOptionPercent / 100.0;
				Obj.m_MagicDamageMin += iEffectValueMin;
				Obj.m_MagicDamageMax += iEffectValueMax;
				Obj.m_PlayerData->m_StatSpecOption[ArrayIndex].StatOptionValMin = iEffectValueMin;
				Obj.m_PlayerData->m_StatSpecOption[ArrayIndex].StatOptionValMax = iEffectValueMax;
			}
			break;

		case STAT_OPTION_INC_CURSE_DAMAGE:
			{
				int iEffectValueMin = Obj.m_CurseDamageMin * StatOptionPercent / 100.0;
				int iEffectValueMax = Obj.m_CurseDamageMax * StatOptionPercent / 100.0;
				Obj.m_CurseDamageMin += iEffectValueMin;
				Obj.m_CurseDamageMax += iEffectValueMax;
				Obj.m_PlayerData->m_StatSpecOption[ArrayIndex].StatOptionValMin = iEffectValueMin;
				Obj.m_PlayerData->m_StatSpecOption[ArrayIndex].StatOptionValMax = iEffectValueMax;
			}
			break;
	}
}

int CStatSpecialize::GetUserArrayIndex(CGameObject &Obj)
{
	int ArrayIndex = -1;

	for (int i = 0; i < MAX_USER_STAT_OPTIONS; i++)
	{
		if (Obj.m_PlayerData->m_StatSpecOption[i].StatOptionID == 0)
		{
			ArrayIndex = i;
			break;
		}
	}

	return ArrayIndex;
}

void CStatSpecialize::SendOptionList(CGameObject &Obj)
{
	 PMSG_STAT_SPEC_OPTION pMsg;
	 PHeadSubSetB((BYTE*)&pMsg, 0x59, 0x00, sizeof(pMsg));
	 int OptionCount = 0;

	 for (int i = 0; i < MAX_USER_STAT_OPTIONS; i++)
	 {
		 if(Obj.m_PlayerData->m_StatSpecOption[i].StatOptionID != 0)
		 {
			 pMsg.m_StatOption[OptionCount].StatOptionID = Obj.m_PlayerData->m_StatSpecOption[i].StatOptionID;
			 pMsg.m_StatOption[OptionCount].StatOptionValMin = Obj.m_PlayerData->m_StatSpecOption[i].StatOptionValMin;
			 pMsg.m_StatOption[OptionCount].StatOptionValMax = Obj.m_PlayerData->m_StatSpecOption[i].StatOptionValMax;
			 OptionCount++;
		 }
	 }

	 GIOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}

