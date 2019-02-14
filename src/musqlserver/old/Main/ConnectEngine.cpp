
#include "ConnectEngine.h"
#include "Logging/Log.h"
#include "ConnectServer.h"

std::vector<STR_CS_USER*> gConnUsers;

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

short UserAdd(SOCKET s, char *IP)
{
	int count = SCount;
	int totalcount = 0;

	while (true)
	{
		if (iReturnIpCount(IP) > g_MaxConnectionsPerIP && strcmpi(IP, g_WhiteListIP) != 0)
		{
			sLog->outError("IP: [%s] Reached Maximum Allowed Connections", IP);
			return -1;
		}

		if (count < MAX_USER)
		{
			STR_CS_USER* connUser = new STR_CS_USER();
			connUser->ConnectionState = 1;
			strcpy(connUser->IP, IP);
			connUser->socket = s;
			connUser->News = false;
			connUser->PacketCount = 0;
			connUser->i64PacketTime = GetTickCount64();
			connUser->Index = count;
			insertUser(connUser);
			SCount++;
			sLog->outBasic("Connection Accept: %s", IP);
			return (short)connUser->Index;
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

	return -1;
}

short UserDelete(int index)
{
	if (gConnUsers[index]->ConnectionState < 1)
	{
		return -1;
	}
	sLog->outBasic("Disconnected: %s (%d)", gConnUsers[index]->IP, index);

	eraseUser(index);
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
