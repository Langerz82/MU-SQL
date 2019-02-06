////////////////////////////////////////////////////////////////////////////////
// FloodSystem.cpp
#include "stdafx.h"

#if FILE_FLOOD_SYSTEMSWITCH
#include "TLog.h"
#include "User.h"
#include "LogToFile.h"
#include "readscript.h"
#include "FloodSystem.h"
#include "configread.h"

cAntiFlood AntiFlood;

void cAntiFlood::Init()
{
	CIniReader ReadFloodSystem(g_ConfigRead.GetPath("\\Custom\\FloodSystem\\FloodSystem.ini"));

	this->Enabled = ReadFloodSystem.ReadInt("FloodSystem", "FloodSystemSwitch", 1);
	this->IsDebugEnabled = ReadFloodSystem.ReadInt("FloodSystem", "FloodSystemDebugSwitch", 1);
	this->IsAutoAdd = ReadFloodSystem.ReadInt("FloodSystem", "AutoAddToBlackList", 0);
	this->MaxIPConnection = ReadFloodSystem.ReadInt("FloodSystem", "MaxIPConnection", 20);
	this->AutoCloseFloodCon = ReadFloodSystem.ReadInt("FloodSystem", "AutoCloseAllFloodConnection", 0);
	this->ReadBlackList(g_ConfigRead.GetPath(FILE_FLOOD_SYSTEM));
}

void cAntiFlood::BlackListClean()
{
	for (int i = 0; i <= MAXIPINBLACKLIST; i++)
	{
		memset(this->BlackListIP[i], 0, sizeof(this->BlackListIP[i]) - 1);
	}
}

void cAntiFlood::ReadBlackList(LPSTR filename)
{
	this->BlackListClean();

	int Token;
	this->BLSize = 0;

	SMDFile = fopen(filename, "r");

	if (SMDFile == NULL)
	{

		g_Log.MsgBox("Error reading file %s", filename);
		exit(1);
	}

	while (true)
	{
		Token = GetToken();

		if (Token == 2)
		{
			break;
		}

		//if ( Token == 1 )
		//{
		if (this->AddToBlackList(TokenString) == false)
		{

			g_Log.MsgBox("error-L3 : Max IP count in BlackList reached!");
			exit(1);
		}
		//}
	}

	fclose(SMDFile);

	if (this->IsDebugEnabled == 1)
	{
		g_Log.AddC(TColor::Green, "[BlackList] Loaded Successfull!");
	}
}

int cAntiFlood::GetIPCount(char * IP)
{
	int Count = 0;

	for (int n = g_ConfigRead.server.GetObjectStartUserIndex(); n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		if (gObj[n].Connected >= PLAYER_CONNECTED)
		{
			if (strcmp(&gObj[n].m_PlayerData->Ip_addr[0], IP) == 0)
			{
				Count++;
			}
		}
	}
	return Count;
}

void cAntiFlood::AutoClose(char * IP)
{
	for (int n = g_ConfigRead.server.GetObjectStartUserIndex(); n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		if (gObj[n].Connected >= PLAYER_CONNECTED)
		{
			if (strcmp(&gObj[n].m_PlayerData->Ip_addr[0], IP) == 0)
			{
				if (this->IsDebugEnabled == 1)
				{
					g_Log.AddC(TColor::Red, "[AutoClose] Index: %d", n);
				}
				IOCP.CloseClient(n);
			}
		}
	}
}

bool cAntiFlood::Check(char * IP)
{
	if (this->Enabled == TRUE)
	{
		if (this->BL_IPCheck(IP) == false)
		{

			if (this->IsDebugEnabled == 1)
			{
				g_Log.AddC(TColor::Red, "[BlackList] Rejected IP %s", IP);
			}
			return false;
		}

		if (this->MaxIPConnection < this->GetIPCount(IP))
		{
			if (this->IsAutoAdd == 1)
			{
				if (this->AddToBlackList(IP) == true)
				{
					WriteTxt(g_ConfigRead.GetPath(FILE_FLOOD_SYSTEM), IP);

					if (this->IsDebugEnabled == 1)
					{
						g_Log.AddC(TColor::Red, "[BlackList] IP: %s Added to Black List - Flood Attempt: %d", IP, this->MaxIPConnection);
					}
				}
				else
				{

					if (this->IsDebugEnabled == 1)
					{
						g_Log.AddC(TColor::Red, "[BlackList][BLACKLIST FULL] IP: %s Fail Add to Black List - Flood Attempt: %d", IP, this->MaxIPConnection);
					}
				}
				if (this->AutoCloseFloodCon == 1)
				{
					this->AutoClose(IP);
				}
			}
			return false;
		}
	}
	return true;
}

bool cAntiFlood::BL_IPCheck(char * IP)
{
	for (int i = 0; i<this->BLSize; i++)
		if (this->BlackListIP[i][0] != 0)
			if (!stricmp(IP, this->BlackListIP[i]))
				return false;
	return true;
}

bool cAntiFlood::AddToBlackList(LPSTR IP)
{
	if (this->BLSize >= MAXIPINBLACKLIST)
	{
		return false;
	}
	strcpy(this->BlackListIP[this->BLSize], IP);
	this->BLSize++;
	return true;
}
#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

