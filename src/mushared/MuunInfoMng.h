////////////////////////////////////////////////////////////////////////////////
// MuunInfoMng.h
#ifndef MUUNINFOMNG_H
#define MUUNINFOMNG_H

#include "MuunInfo.h"

#define MAX_MUUN_ITEM 300

class CMuunInfoMng
{
public:
	CMuunInfoMng();
	~CMuunInfoMng();

	bool LoadScriptMuunSystemInfo(char *lpszFileName);
	bool LoadScriptMuunSystemOption(const char *lpszFileName);

	bool InsertMuunOptionInfo(CMuunOpt *pCMuunOpt);
	time_t ConvertStringToTime(const char *pchTime);

	CMuunInfo * GetMuunInfo(int iIndex);
	CMuunInfo * GetMuunItemNumToMuunInfo(int iMuunItemNum);

	int GetBeforeEvolutionMuunItemIndex(int iItemNum);

private:
	CMuunInfo m_CMuunInfo[MAX_MUUN_ITEM];
};
#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

