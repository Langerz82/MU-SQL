////////////////////////////////////////////////////////////////////////////////
// PVPZone.h
#pragma once
// -------------------------------------------------------------------------------

#include "user.h"
// -------------------------------------------------------------------------------

#define MAX_PVPZONE	50
// -------------------------------------------------------------------------------

struct PVPZONE_DATA
{
	WORD MapNumber;
	BYTE PlayerKiller_PKX1;
	BYTE PlayerKiller_PKY1;
	BYTE PlayerKiller_PKX2;
	BYTE PlayerKiller_PKY2;
};
// -------------------------------------------------------------------------------

class PVPZone
{
public:
	PVPZone();
	~PVPZone();
	// ----
	void	Init();
	void	Load();
	void	ReadData(char * File);
	// ----
	//bool	IsPVPZone(LPOBJ lpUser);
	// ----
	PVPZONE_DATA m_Data[MAX_PVPZONE];
	// ----
}; extern PVPZone g_PVPZone;
// -------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

