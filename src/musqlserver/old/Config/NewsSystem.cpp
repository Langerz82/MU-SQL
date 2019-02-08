////////////////////////////////////////////////////////////////////////////////
// NewsSystem.cpp
#include "StdAfx.h"
#include "NewsSystem.h"
#include "TNotice.h"
#include "Log/Log.h"

CNewsSystem g_NewsSystem;

CNewsSystem::CNewsSystem(void)
{

}

CNewsSystem::~CNewsSystem(void)
{

}

bool CNewsSystem::LoadFile(LPSTR lpFile)
{
	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(lpFile);

	if(res.status != pugi::status_ok)
	{
		sLog->outError("%s load fail (%s)", lpFile, res.description());
		return false;
	}
	
	this->m_i64Tick = GetTickCount64();
	this->m_iMaxGUID = 0;
	this->m_NewsData.clear();

	NEWS_DATA m_News;

	pugi::xml_node mainXML = file.child("NewsSystem");

	this->m_bEnable = mainXML.attribute("Enable").as_int();
	this->m_iMinuteDelay = mainXML.attribute("DisplayInterval").as_int();

	for (pugi::xml_node news = mainXML.child("News"); news; news = news.next_sibling())
	{
		m_News.GUID = news.attribute("ID").as_int();

		for (pugi::xml_node line = news.child("Line"); line; line = line.next_sibling())
		{
			int iLine = line.attribute("Number").as_int();

			if (iLine < 0 || iLine > 2)
			{
				sLog->outError("NewsSystem: NewsID is WRONG - must be 0,1 or 2");
				continue;
			}

			m_News.News[iLine] = line.attribute("Text").as_string();
		}
		
		this->m_NewsData.push_back(m_News);
	}

	sLog->outBasic("%d news load!", m_NewsData.size());
	return true;
}

void CNewsSystem::Run()
{
	if (this->m_bEnable == false)
	{
		return;
	}

	if((GetTickCount64() - m_i64Tick) < (this->m_iMinuteDelay*60000))
	{
		return;
	}

	PMSG_NOTICE pNotice;

	for(std::vector<NEWS_DATA>::iterator It = this->m_NewsData.begin(); It != this->m_NewsData.end(); It++)
	{
		if(It->GUID == this->m_iMaxGUID)
		{
			TNotice::MakeNoticeMsg(&pNotice, 0, (char *)It->News[0].c_str());
			TNotice::SendNoticeToAllUser(&pNotice);

			TNotice::MakeNoticeMsg(&pNotice, 0, (char *)It->News[1].c_str());
			TNotice::SendNoticeToAllUser(&pNotice);

			TNotice::MakeNoticeMsg(&pNotice, 0, (char *)It->News[2].c_str());
			TNotice::SendNoticeToAllUser(&pNotice);

			this->m_iMaxGUID++;
			this->m_i64Tick = GetTickCount64();
			
			if(this->m_iMaxGUID == this->m_NewsData.size())
				this->m_iMaxGUID = 0;

			return;
		}
	}

	this->m_iMaxGUID = 0;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

