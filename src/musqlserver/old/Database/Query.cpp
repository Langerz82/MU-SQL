
#include "Query.h"
#include "DelayHandler.h"

#define szModule "CQuery"

CQuery::CQuery(): m_Database()
{
}

CQuery::~CQuery()
{
	Close();
}

BOOL CQuery::Connect(LPTSTR lpszDNS, LPSTR lpszPort, LPTSTR lpszUser, LPTSTR lpszPassword, LPSTR lpszServerName)
{
	lstrcpy(this->m_szDNS, lpszDNS);
	lstrcpy(this->m_szUser, lpszUser);
	lstrcpy(this->m_szPassword, lpszPassword);

	char* InCon[256]				= {0};

	wsprintf((char *)InCon, "%s;%s;%s;%s;%s;", lpszDNS, lpszPort,
		lpszUser, lpszPassword, lpszServerName);

	bool Result = m_Database.Initialize((const char*)InCon, 1); // TODO add configurable connections.

	if (!Result)
	{
		sLog.outError("Can not connect to database %s", InCon);
		return FALSE;
	}

	m_Database.ThreadStart();
	m_Database.AllowAsyncTransactions();

	return TRUE;
}

BOOL CQuery::ReConnect()
{
	return this->Connect(this->m_szDNS, this->m_szPort, this->m_szUser, this->m_szPassword, this->m_szDatabase);
}

BOOL CQuery::ExecQuery(TCHAR* lpszStatement, ...)
{
	TCHAR szStatement[1024];

	va_list pArguments;
	va_start(pArguments, lpszStatement);
		vsprintf(szStatement, lpszStatement, pArguments);
	va_end(pArguments);

	return Execute(szStatement);
}

BOOL CQuery::Execute(TCHAR* lpszStatement)
{
	while (true)
	{
		bool Result = m_Database.Execute(lpszStatement);

		if (!Result)
		{
			sLog.outError("[SQL Error] Error executing: %s", lpszStatement);

			bool bReconnect = false;
			this->Diagnosis(bReconnect);

			if (bReconnect == true)
			{
				Sleep(1);
				continue;
			}

			return FALSE;
		}
	}
	return TRUE;
}

QueryResult* CQuery::Fetch(TCHAR* lpszStatement, ...)
{
	TCHAR szStatement[2048];

	va_list pArguments;
	va_start(pArguments, lpszStatement);
	vsprintf(szStatement, lpszStatement, pArguments);
	va_end(pArguments);

	if (m_Result != NULL)
		m_Result == NULL;
	if (m_Fields != NULL)
		delete m_Fields;

	while (true)
	{
		m_Result = m_Database.Query(szStatement);

		if (m_Result == NULL)
		{
			sLog.outError("[SQL Error] Error querying: %s", lpszStatement);

			bool bReconnect = false;
			this->Diagnosis(bReconnect);

			if (bReconnect == true)
			{
				Sleep(1);
				continue;
			}
			m_Fields = NULL;
			return NULL;
		}
	}
	Field* m_Fields = new Field();
	m_Fields = m_Result->Fetch();
	return m_Result;
}

bool CQuery::HasFields()
{
	return (m_Fields != NULL);
}

int CQuery::GetAsBinary(LPSTR lpszStatement, LPBYTE OUT lpszReturnBuffer, int size)
{
	QueryResult* res = Fetch(lpszStatement);
	Field* field = res->Fetch();
	lpszReturnBuffer = field[0].GetByteArray(size);
	return sizeof(lpszReturnBuffer);
}

void CQuery::SetAsBinary(LPTSTR lpszStatement, LPBYTE lpBinaryBuffer, UINT32 BinaryBufferSize)
{
	// TODO.
}


/*int CQuery::FindIndex(LPTSTR ColName)
{
	for (short i = 0; i<this->m_ColCount; i++)
	{
		if (this->m_SQLColName[i][0] == ColName[0])
		{
			if (lstrcmp((TCHAR *)this->m_SQLColName[i], (TCHAR *)ColName) == 0)
			{
				return i;
			}
		}
	}

	return -1;
}

void CQuery::GetAsString(LPTSTR ColName, LPTSTR pOutBuffer, int size)
{
	int iIndex = this->FindIndex(ColName);

	if (iIndex != -1)
	{
		this->GetAsString(iIndex, pOutBuffer, size);
		return;
	}
	else
	{
		pOutBuffer[0] = 0;
	}
}*/

void CQuery::GetAsString(int iIndex, LPTSTR pOutBuffer, int size)
{
	if (iIndex == -1)
	{
		pOutBuffer[0] = 0;
	}

	LPTSTR str = this->m_Fields->GetCppString[iIndex];
	if (str == NULL)
	{
		pOutBuffer[0] = 0;
	}
	else
	{
		strncpy(pOutBuffer, this->m_Fields->GetCppString[iIndex], size);
	}
}

/*DWORD CQuery::GetAsInteger(LPTSTR ColName)
{
	int iIndex = this->FindIndex(ColName);

	if (iIndex != -1)
	{
		return this->GetAsInteger(iIndex);
	}

	return -1;
}*/

DWORD CQuery::GetAsInteger(int iIndex)
{
	if (iIndex == -1)
	{
		return -1;
	}

	return this->m_Fields->GetInt32[iIndex];
}

/*INT64 CQuery::GetAsInteger64(LPTSTR ColName)
{
	int iIndex = this->FindIndex(ColName);

	if (iIndex != -1)
	{
		return this->GetAsInteger64(iIndex);
	}

	return -1LL;
}*/

INT64 CQuery::GetAsInteger64(int iIndex)
{
	if (iIndex == -1)
	{
		return -1LL;
	}
	return this->m_Fields->GetInt64[iIndex];
}

/*float CQuery::GetAsFloat(LPTSTR ColName)
{
	int iIndex = this->FindIndex(ColName);

	if (iIndex != -1)
	{
		return this->GetAsFloat(iIndex);
	}

	return -1;
}*/

float CQuery::GetAsFloat(int iIndex)
{
	if (iIndex == -1)
	{
		return -1;
	}
	return this->m_Fields->GetFloat[iIndex];
}


void CQuery::Close()
{
	if (m_Result)
		m_Result = NULL;
	if (m_Fields)
		delete m_Fields;

	m_Database.HaltDelayThread();
}

void CQuery::Diagnosis(bool &bReconnect)
{
	m_Database.Ping();

	if(this->ReConnect() == TRUE)
	{
		g_DelayHandler.IncreaseQuerySessionId();
	}

	bReconnect = true;
}

void CQuery::Disconnect()
{
	m_Database.HaltDelayThread();
}

