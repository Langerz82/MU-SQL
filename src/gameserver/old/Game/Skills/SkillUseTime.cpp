////////////////////////////////////////////////////////////////////////////////
// SkillUseTime.cpp
#include "StdAfx.h"
#include "SkillUseTime.h"
#include "Logging/Log.h"
#include "User/CUserData.h"

CSkillUseTime g_SkillUseTime;

CSkillUseTime::CSkillUseTime(void)
{
	this->m_bFileLoad = false;
	this->m_bEnabled = false;
}

CSkillUseTime::~CSkillUseTime(void)
{
}

bool CSkillUseTime::LoadConfig(LPSTR lpFile)
{
	CIniReader ReadSkillTime(lpFile);

	this->m_bEnabled					= ReadSkillTime.ReadInt("SkillTimeCheck", "IsEnabled", 0);
	this->m_iIsDC						= ReadSkillTime.ReadInt("SkillTimeCheck", "IsDisconnectUser", 0);
	this->m_iNumberOfBadSkillUseDC		= ReadSkillTime.ReadInt("SkillTimeCheck", "NumberOfCastsToDisconnect", 0);
	this->m_bDebugMode					= ReadSkillTime.ReadInt("SkillTimeCheck", "DebugMode", 0);

	return true;
}

bool CSkillUseTime::LoadFile(LPSTR lpFile)
{
	if ( this->m_bEnabled == false )
	{
		return false;
	}

	this->m_bFileLoad = false;
	this->m_vtSkillTimeInfo.clear();

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(lpFile);

	if (res.status != pugi::status_ok)
	{
		sLog->outError("%s File Load Error (%s)", lpFile, res.description());
		return false;
	}

	SKILL_TIME_INFO m_SkillInfo;
	
	pugi::xml_node mainXML = file.child("SkillUseTime");

	for (pugi::xml_node skill = mainXML.child("Skill"); skill; skill = skill.next_sibling())
	{
		m_SkillInfo.iSkill = skill.attribute("Number").as_int();
		m_SkillInfo.iMinMagicSpeed = skill.attribute("MinMagicSpeed").as_int();
		m_SkillInfo.iMaxMagicSpeed = skill.attribute("MaxMagicSpeed").as_int();
		m_SkillInfo.iUseTime = skill.attribute("UseTime").as_int();

		this->m_vtSkillTimeInfo.push_back(m_SkillInfo);
	}

	this->m_bFileLoad = true;
	sLog->outBasic("Loaded file %s (count:%d)", lpFile, this->m_vtSkillTimeInfo.size());

	return true;
}

bool CSkillUseTime::CheckSkillTime(CGameObject &Obj, int iSkill)
{
	if ( this->m_bEnabled == false )
	{
		return true;
	}

	if ( this->m_bFileLoad == false )
	{
		return true;
	}

	if ( Obj.Type != OBJ_USER )
	{
		return true;
	}

	ULONGLONG iTimeDiff = GetTickCount64() - Obj.m_PlayerData->LastSkillUseTick;

	if ( this->m_bDebugMode )
	{
		sLog->outBasic("[DEBUG] UseSkill (%d) Time(%d) MagicSpeed(%d)", iSkill, iTimeDiff, Obj.m_MagicSpeed);
	}

	for(std::vector<SKILL_TIME_INFO>::iterator It = this->m_vtSkillTimeInfo.begin(); It != this->m_vtSkillTimeInfo.end(); It++)
	{
		if ( Obj.m_MagicSpeed >= It->iMinMagicSpeed && Obj.m_MagicSpeed <= It->iMaxMagicSpeed )
		{
			if ( iSkill == It->iSkill )
			{
				if ( iTimeDiff < It->iUseTime )
				{
					Obj.m_PlayerData->LastSkillUseCount++;

					if ( this->m_iIsDC )
					{
						if ( Obj.m_PlayerData->LastSkillUseCount >= this->m_iNumberOfBadSkillUseDC )
						{
							sLog->outError( "[ANTI-HACK] [%s][%s][%s] Used skill too fast %d times -> Disconnect", Obj.AccountID, Obj.Name, Obj.m_PlayerData->ConnectUser->IP, Obj.m_PlayerData->LastSkillUseCount);
							GSProtocol.GCSendDisableReconnect(Obj.m_Index);
							//IOCP.CloseClient(Obj.m_Index);
						}
					}

					return false;
				}
			}
		}
	}

	Obj.m_PlayerData->LastSkillUseCount = 0;
	Obj.m_PlayerData->LastSkillUseNumber = iSkill;
	Obj.m_PlayerData->LastSkillUseTick = GetTickCount64();

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

