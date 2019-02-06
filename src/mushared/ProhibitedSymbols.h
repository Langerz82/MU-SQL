////////////////////////////////////////////////////////////////////////////////
// ProhibitedSymbols.h
#ifndef PROHIBITEDSYMBOLS_H
#define PROHIBITEDSYMBOLS_H
#define TYPE_CHAT 0
#define TYPE_NAME 1
#define TYPE_GUILDNAME 2
struct _PROSYM_STRUCT 
{
	BYTE system[3];
};

class CProhibitedSymbols
{
public:
	void LoadSymbolFile(LPSTR szFile);
	bool Validate(char* string, int len, BYTE Type);
	std::map<wchar_t, _PROSYM_STRUCT> m_ProhibitedSymbols;
	int m_nMode;
	std::string regex[3];
}; extern CProhibitedSymbols g_prohibitedSymbols;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

