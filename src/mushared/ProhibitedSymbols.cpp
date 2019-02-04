////////////////////////////////////////////////////////////////////////////////
// ProhibitedSymbols.cpp
#include "StdAfx.h"
#include "ProhibitedSymbols.h"
#include "TLog.h"
#include <regex>

CProhibitedSymbols g_prohibitedSymbols;

void CProhibitedSymbols::LoadSymbolFile(LPSTR szFile)
{
	this->m_ProhibitedSymbols.clear();
	_PROSYM_STRUCT data;

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(szFile);

	if (res.status != pugi::status_ok)
	{
		g_Log.MsgBox("%s file loading error - %s", szFile, res.description());
		return;
	}

	pugi::xml_node main_section = file.child("ProhibitedSymbols");
	this->m_nMode = main_section.attribute("Mode").as_int();
	pugi::xml_node symbol_section = main_section.child("Symbols");

	for (pugi::xml_node restrictsymbol = symbol_section.child("Restrict"); restrictsymbol; restrictsymbol = restrictsymbol.next_sibling())
	{
		DWORD dwNum = MultiByteToWideChar(CP_UTF8, 0, restrictsymbol.attribute("Symbol").as_string(), -1, NULL, 0);
		wchar_t* pwText;
		pwText = new wchar_t[dwNum];
		MultiByteToWideChar(CP_UTF8, 0, restrictsymbol.attribute("Symbol").as_string(), -1, pwText, dwNum);
		std::wstring wStr = pwText;

		data.system[0] = restrictsymbol.attribute("Chat").as_int();
		data.system[1] = restrictsymbol.attribute("CharName").as_int();
		data.system[2] = restrictsymbol.attribute("GuildName").as_int();

		this->m_ProhibitedSymbols.insert(std::pair<wchar_t, _PROSYM_STRUCT>(*wStr.c_str(), data));

		delete[] pwText;
	}

	pugi::xml_node regex_section = main_section.child("Regex");

	for (pugi::xml_node regextype = regex_section.child("Type"); regextype; regextype = regextype.next_sibling())
	{
		int nSystem = regextype.attribute("System").as_int();
		this->regex[nSystem] = regextype.attribute("Regex").as_string();
	}

	g_Log.Add("Loaded %d prohibited symbols", this->m_ProhibitedSymbols.size());
}

bool CProhibitedSymbols::Validate(char* string, int len, BYTE Type)
{
	DWORD dwNum = MultiByteToWideChar(CP_UTF8, 0, string, -1, NULL, 0);
	char *psText;
	wchar_t *pwText;

	if (!dwNum)
	{

	}

	else
	{
		pwText = new wchar_t[dwNum];
		MultiByteToWideChar(CP_UTF8, 0, string, -1, pwText, dwNum);

		dwNum = WideCharToMultiByte(CP_UTF8, 0, pwText, -1, NULL, 0, NULL, NULL);

		psText = new char[dwNum];
		WideCharToMultiByte(CP_UTF8, 0, pwText, -1, psText, dwNum, NULL, NULL);
	}

	std::wstring wstr = pwText;
	bool bCheckResult = true;

	if (this->m_nMode == 0)
	{
		for (int i = 0; i < wstr.length(); i++)
		{
			std::map<wchar_t, _PROSYM_STRUCT>::iterator it = this->m_ProhibitedSymbols.find(wstr.at(i));
			if (it != this->m_ProhibitedSymbols.end() && it->second.system[Type] == 1)
			{
				bCheckResult = false;
				break;
			}
		}
	}
	else
	{
		dwNum = MultiByteToWideChar(CP_UTF8, 0, this->regex[Type].c_str(), -1, NULL, 0);
		wchar_t *pwRegex;
		pwRegex = new wchar_t[dwNum];
		MultiByteToWideChar(CP_UTF8, 0, this->regex[Type].c_str(), -1, pwRegex, dwNum);
		std::wregex r(pwRegex);
		bool match = std::regex_match(wstr.c_str(), r);

		if (match)
		{
			bCheckResult = true;
		}

		else
		{
			bCheckResult = false;
		}

		delete[] pwRegex;
	}

	delete[] psText;
	delete[] pwText;
	return bCheckResult;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

