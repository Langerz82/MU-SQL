////////////////////////////////////////////////////////////////////////////////
// IniReader.h - Ported.
#ifndef INIREADER_H
#define INIREADER_H

#include "custTypedef.h"
#include <string>
#include "Log/Log.h"
#include "iniReader/INIReaderImp.h"

class CIniReader
{
public:
	CIniReader(LPSTR szFile)
	{
		m_iniReader = new INIReaderImp(szFile);
		if (m_iniReader->ParseError())
			sLog.outError("Error - cannot load %s file", szFile);
	}

	~CIniReader()
	{
		//iniparser_freedict(this->m_IniDictionary);
		//this->m_IniDictionary = NULL;
		delete m_iniReader;
		m_iniReader = NULL;
	}

	int ReadInt(LPSTR szSection, LPSTR szKey, int Default)
	{
		//char szFormatKey[128];
		//sprintf(szFormatKey, "%s:%s", szSection, szKey);

		//return iniparser_getint(this->m_IniDictionary, szFormatKey, Default);
		return m_iniReader->GetInteger(szSection, szKey, Default);
	}

	DWORD ReadDword(LPSTR szSection, LPSTR szKey, DWORD Default)
	{
		//char szFormatKey[128];
		//sprintf(szFormatKey, "%s:%s", szSection, szKey);

		//return iniparser_getuint(this->m_IniDictionary, szFormatKey, Default);
		return m_iniReader->GetInteger(szSection, szKey, Default);
	}

	std::string ReadString(LPSTR szSection, LPSTR szKey, std::string Default)
	{
		//char szFormatKey[128];
		//sprintf(szFormatKey, "%s:%s", szSection, szKey);

		//char* szReturnStr = iniparser_getstring(this->m_IniDictionary, szFormatKey, (char *)Default.c_str());

		const char* szReturnStr = m_iniReader->GetString(szSection, szKey, Default).c_str();

		return szReturnStr;
	}

	float ReadFloat(LPSTR szSection, LPSTR szKey, float Default)
	{
		//char szFormatKey[128];
		//sprintf(szFormatKey, "%s:%s", szSection, szKey);

		//return (float)iniparser_getdouble(this->m_IniDictionary, szFormatKey, Default);
		return m_iniReader->GetReal(szSection, szKey, Default);
	}

private:

	//dictionary * m_IniDictionary;
	INIReaderImp* m_iniReader;
}; 

#endif


