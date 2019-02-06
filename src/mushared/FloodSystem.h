////////////////////////////////////////////////////////////////////////////////
// FloodSystem.h
#include "stdafx.h"

//#define FILE_FLOOD_SYSTEMSWITCH

#if FILE_FLOOD_SYSTEMSWITCH

#define MAXIPINBLACKLIST	2048

#define FILE_FLOOD_SYSTEM "Custom\\FloodSystem\\BlackList.dat"

class cAntiFlood
{
public:
	void Init();
	bool Check(char * IP);
	bool AddToBlackList(LPSTR IP);

	BOOL Enabled;
private:
	void ReadBlackList(LPSTR filename);
	void BlackListClean();
	void AutoClose(char * IP);

	bool BL_IPCheck(char * IP);
	int GetIPCount(char * IP);

	BOOL IsDebugEnabled;
	BOOL IsAutoAdd;
	BOOL AutoCloseFloodCon;
	int BLSize;
	int MaxIPConnection;
	char BlackListIP[MAXIPINBLACKLIST][16];
};

extern cAntiFlood AntiFlood;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

