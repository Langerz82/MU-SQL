////////////////////////////////////////////////////////////////////////////////
// PVPZone.cpp
#include "StdAfx.h"
#include "PVPZone.h"
#include "GameMain.h"
#include "TLog.h"
#include "readscript.h"
#include "configread.h"
// -------------------------------------------------------------------------------

PVPZone g_PVPZone;
// -------------------------------------------------------------------------------

PVPZone::PVPZone()
{
	this->Init();
}
// -------------------------------------------------------------------------------

PVPZone::~PVPZone()
{

}
// -------------------------------------------------------------------------------

void PVPZone::Init()
{
	ZeroMemory(this->m_Data, sizeof(this->m_Data));
}
// -------------------------------------------------------------------------------

void PVPZone::Load()
{
	this->Init();
	this->ReadData(g_ConfigRead.GetPath("IGC_PVPZone.xml"));
}
// -------------------------------------------------------------------------------

void PVPZone::ReadData(char * File)
{
	using namespace pugi;
	xml_document Document;
	xml_parse_result Result = Document.load_file(File);

	if (Result.status != status_ok)
	{
		g_Log.MsgBox("[IGC_PVPZone] File %s not found!", File);
		return;
	}

	xml_node CommonMapNonPK = Document.child("PVPZone");

	for (pugi::xml_node map = CommonMapNonPK.child("PVP"); map; map = map.next_sibling())
	{
		int CountID = map.attribute("ID").as_int(0);

		int PvPType = map.attribute("MapID").as_int(-1);
		int PKX1 = map.attribute("CoordX1").as_int(0);
		int PKY1 = map.attribute("CoordY1").as_int(0);
		int PKX2 = map.attribute("CoordX2").as_int(0);
		int PKY2 = map.attribute("CoordY2").as_int(0);

		g_PVPZone.m_Data[CountID].MapNumber = PvPType;
		g_PVPZone.m_Data[CountID].PlayerKiller_PKX1 = PKX1;
		g_PVPZone.m_Data[CountID].PlayerKiller_PKY1 = PKY1;
		g_PVPZone.m_Data[CountID].PlayerKiller_PKX2 = PKX2;
		g_PVPZone.m_Data[CountID].PlayerKiller_PKY2 = PKY2;
	}
}
// -------------------------------------------------------------------------------
/*
bool PVPZone::IsPVPZone(LPOBJ lpUser)
{
	for (int i = 0; i < this->m_LoadedCount; i++)
	{
		if (lpUser->MapNumber == this->m_Data[i].MapNumber)
		{
			if ((lpUser->X >= this->m_Data[i].X1 && lpUser->X <= this->m_Data[i].X2)// <= 131)	//Fix
				&& (lpUser->Y >= this->m_Data[i].Y1 && lpUser->Y <= this->m_Data[i].Y2))
			{
				return true;
			}
		}
	}
	// ----
	return false;
}*/
// -------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

