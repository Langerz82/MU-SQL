#include "StdAfx.h"
#include "ConnectEngine.h"
#include "Logging/Log.h"
#include "Main.h"

std::vector<STR_CS_USER*> gConnUsers;
std::map<int, STR_CS_USER*> gCSUsers;

int SCount=0;

void UserInit()
{
	SCount = 0;
}

int iReturnIpCount(char *IP)
{
	int count = 0;

	for (int i = 0; i < MAX_USER; i++)
	{
		if (!strcmp(IP, gConnUsers[i]->IP))
		{
			count++;
		}
	}

	return count;

}

STR_CS_USER* UserAdd(char* SocketKey, char* IP)
{
	int count = SCount;
	int totalcount = 0;

	while (true)
	{
		if (iReturnIpCount(IP) > g_MaxConnectionsPerIP /*&& strcmpi(IP, g_WhiteListIP) != 0*/)
		{
			sLog->outError("IP: [%s] Reached Maximum Allowed Connections", IP);
			return nullptr;
		}

		if (count < MAX_USER)
		{
			STR_CS_USER* connUser = new STR_CS_USER();
			connUser->ConnectionState = 1;
			strcpy(connUser->IP, IP);
			std::memcpy(connUser->SocketKey, SocketKey, 32);
			connUser->News = false;
			connUser->PacketCount = 0;
			connUser->i64PacketTime = GetTickCount64();
			connUser->Index = count;
			insertCSUser(connUser);
			SCount++;
			sLog->outBasic("Connection Accept: %s", IP);
			return connUser;
		}

		count++;

		if (count >= MAX_USER)
		{
			count = 0;
		}

		totalcount++;

		if (totalcount >= MAX_USER)
		{
			break;
		}
	}

	return nullptr;
}

short UserDelete(int index)
{
	if (gConnUsers[index]->ConnectionState < 1)
	{
		return -1;
	}
	sLog->outBasic("Disconnected: %s (%d)", gConnUsers[index]->IP, index);

	eraseCSUser(index);
	SCount--;

	return 1;
}

bool UserSet(int index, unsigned int Port, int type, char* Name)
{
	if (gConnUsers[index]->ConnectionState < 1)
	{
		return FALSE;
	}

	if (gConnUsers[index]->ConnectionState == 2)
	{
		return FALSE;
	}

	gConnUsers[index]->ConnectionState = 2;
	gConnUsers[index]->Port = Port;
	gConnUsers[index]->Type = type;

	return TRUE;
}

void UserRun()
{
	for (int i = 0; i < MAX_USER; i++)
	{
		if (gConnUsers[i]->ConnectionState != 0)
		{
			if (GetTickCount64() - gConnUsers[i]->ConnectionState >= 1000)
			{
				gConnUsers[i]->PacketCount = 0;
				gConnUsers[i]->i64PacketTime = GetTickCount64();
			}
		}
	}
}
