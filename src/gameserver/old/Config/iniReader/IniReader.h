////////////////////////////////////////////////////////////////////////////////
// IniReader.h - Ported.
#ifndef INIREADER_H
#define INIREADER_H

#include "StdAfx.h"
#include <string>
#include "cpp/INIReader.h"
#include "Logging/Log.h"

class CIniReader
{
public:
	CIniReader(LPSTR szFile)
	{
		m_iniReader = new INIReader(szFile);
		if (m_iniReader->ParseError() < 0) {
			std::cout << StringFormat("Can't load '%s', Error code: %d\n", szFile, m_iniReader->ParseError());
			return;
		}
	}

	~CIniReader()
	{
		delete m_iniReader;
		m_iniReader = NULL;
	}

	int ReadInt(LPSTR szSection, LPSTR szKey, int Default)
	{
		return m_iniReader->GetInteger(szSection, szKey, Default);
	}

	DWORD ReadDword(LPSTR szSection, LPSTR szKey, DWORD Default)
	{
		return m_iniReader->GetInteger(szSection, szKey, Default);
	}

	std::string ReadString(LPSTR szSection, LPSTR szKey, std::string Default)
	{
		const char* szReturnStr = m_iniReader->GetString(szSection, szKey, Default).c_str();

		return szReturnStr;
	}

	float ReadFloat(LPSTR szSection, LPSTR szKey, float Default)
	{
		return m_iniReader->GetReal(szSection, szKey, Default);
	}

private:
	INIReader* m_iniReader;
}; 

#endif


