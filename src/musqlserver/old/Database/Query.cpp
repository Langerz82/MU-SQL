
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

QueryResult* CQuery::Fetch(TCHAR* lpszStatement)
{
	while (true)
	{
		m_Result = m_Database.Query(lpszStatement);

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

			return NULL;
		}
	}
	return m_Result;
}

void CQuery::Close()
{
	if (m_Result)
		delete m_Result;

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

