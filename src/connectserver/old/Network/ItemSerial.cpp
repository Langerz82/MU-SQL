////////////////////////////////////////////////////////////////////////////////
// ItemSerial.cpp

#include "ItemSerial.h"
#include "DSprotocol.h"

CItemObjectSerial g_ItemSerial;

CItemObjectSerial::CItemObjectSerial(void)
{
	//InitializeCriticalSection(&this->m_ItemSerialCriti);
}

CItemObjectSerial::~CItemObjectSerial(void)
{
	//DeleteCriticalSection(&this->m_ItemSerialCriti);
}

bool CItemObjectSerial::Init()
{
	// TODO
	//if (this->m_Query.Connect(g_MuOnlineDNS, g_UserID, g_Password, g_ServerName) == FALSE)
	if (false) // stub
	{
		sLog->outError("[ERROR] - DATA SERVER CANNOT CONNECT TO MSSQL");
		return false;
	}

	this->m_dwItemSerial = 0;
	this->m_dwLastSerial = 0;
	return true;
}

UINT64 CItemObjectSerial::MakeSerial()
{
	if( this->GetSerialFromDB(this->m_dwItemSerial, 3000) == FALSE )
	{
		return FALSE;
	}

	this->m_dwLastSerial = this->m_dwItemSerial + 3000;
	return 1;
}

UINT64 CItemObjectSerial::GenerateSerial()
{
	//EnterCriticalSection(&this->m_ItemSerialCriti);
	
	if ( this->m_dwItemSerial >= this->m_dwLastSerial )
	{
		this->MakeSerial();
	}

	UINT64 retserial = this->m_dwItemSerial;

	this->m_dwItemSerial++;

	//LeaveCriticalSection(&this->m_ItemSerialCriti);

	return retserial;
}

BOOL CItemObjectSerial::GetSerialFromDB(UINT64 & serial, int MakeSerialCount)
{
	// TODO
	return TRUE;
}

UINT64 CItemObjectSerial::GetSerialCount()
{
	return this->m_dwItemSerial;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

