////////////////////////////////////////////////////////////////////////////////
// IpCache.h
// ------------------------------
// Decompiled by Deathway
// Date : 2007-03-09
// ------------------------------
#ifndef IPCACHE_H
#define IPCACHE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_IP_CACHE	2000

struct IPINFO 
{
	char IpAddress[16];	// 0
	BOOL IpUsed;	// 10
	DWORD Time;	// 14
};


class CIpCache
{
public:

	void DelIp(char* ip);
	BOOL AddIp(char* ip);
	void Init();
	
	CIpCache();
	virtual ~CIpCache();

private:

	IPINFO m_IpInfo[MAX_IP_CACHE];
};



extern CIpCache IpCache;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

