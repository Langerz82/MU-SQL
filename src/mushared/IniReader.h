////////////////////////////////////////////////////////////////////////////////
// IniReader.h
#ifndef INIREADER_H
#define INIREADER_H

#include <iniparser.h>
#include "TLog.h"
class CIniReader
{
public:
	CIniReader(LPSTR szFile)
	{
		this->m_IniDictionary = iniparser_load(szFile);

		if (this->m_IniDictionary == NULL)
		{
			g_Log.MsgBox("Error - cannot load %s file", szFile);
		}
	}

	~CIniReader()
	{
		iniparser_freedict(this->m_IniDictionary);
		this->m_IniDictionary = NULL;
	}

	int ReadInt(LPSTR szSection, LPSTR szKey, int Default)
	{
		char szFormatKey[128];
		sprintf(szFormatKey, "%s:%s", szSection, szKey);

		return iniparser_getint(this->m_IniDictionary, szFormatKey, Default);
	}

	DWORD ReadDword(LPSTR szSection, LPSTR szKey, DWORD Default)
	{
		char szFormatKey[128];
		sprintf(szFormatKey, "%s:%s", szSection, szKey);

		return iniparser_getuint(this->m_IniDictionary, szFormatKey, Default);
	}

	std::string ReadString(LPSTR szSection, LPSTR szKey, std::string Default)
	{
		char szFormatKey[128];
		sprintf(szFormatKey, "%s:%s", szSection, szKey);

		char* szReturnStr = iniparser_getstring(this->m_IniDictionary, szFormatKey, (char *)Default.c_str());

		return szReturnStr;
	}

	float ReadFloat(LPSTR szSection, LPSTR szKey, float Default)
	{
		char szFormatKey[128];
		sprintf(szFormatKey, "%s:%s", szSection, szKey);

		return (float)iniparser_getdouble(this->m_IniDictionary, szFormatKey, Default);
	}

private:

	dictionary * m_IniDictionary;
}; 

#endif



////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

