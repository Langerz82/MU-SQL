////////////////////////////////////////////////////////////////////////////////
// ItemSerial.h
#ifndef ITEMSERIAL_H
#define ITEMSERIAL_H

class CItemSerial
{
public:
	CItemSerial(void);
	virtual ~CItemSerial(void);

	bool Init();
	UINT64 GenerateSerial();
	UINT64 GetSerialCount();

private:
	UINT64 MakeSerial();
	BOOL GetSerialFromDB(UINT64 & serial, int MakeSerialCount);

	//CRITICAL_SECTION m_ItemSerialCriti;
	UINT64 m_dwItemSerial;
	UINT64 m_dwLastSerial;
};

extern CItemSerial g_ItemSerial;
#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

