////////////////////////////////////////////////////////////////////////////////
// ItemSerial.h
#ifndef ITEMSERIAL_H
#define ITEMSERIAL_H

#include "StdAfx.h"

class CItemObjectSerial
{
public:
	CItemObjectSerial(void);
	virtual ~CItemObjectSerial(void);

	bool Init();
	UINT64 GenerateSerial();
	UINT64 GetSerialCount();

private:
	UINT64 MakeSerial();
	BOOL GetSerialFromDB(UINT64 & serial, int MakeSerialCount);

	CRITICAL_SECTION m_ItemSerialCriti;
	UINT64 m_dwItemSerial;
	UINT64 m_dwLastSerial;
};

extern CItemObjectSerial g_ItemSerial;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

