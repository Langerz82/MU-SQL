
#include "Query.h"
#include "DelayHandler.h"
#include "database/Database/PreparedStatement.h"
#include "database/Database/MySQLConnection.h"

#define szModule "CQuery"

CQuery::CQuery(DatabaseWorkerPool<MySQLConnection>* db): m_Database(db)
{
}
CQuery::CQuery(DatabaseWorkerPool<ConnectDatabaseConnection>* db) : m_Database(db)
{
}
/*CQuery::CQuery(DatabaseWorkerPool<GameDatabaseConnection>* db) : m_Database(db)
{
}*/

CQuery::~CQuery()
{
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
		m_Database->Execute(lpszStatement);

		if (false) // stub TODO
		{
			sLog->outError("[SQL Error] Error executing: %s", lpszStatement);
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
		m_Database->Query(szStatement);

		if (false) // stub TODO
		{
			sLog->outError("[SQL Error] Error querying: %s", lpszStatement);
			m_Fields = NULL;
			return NULL;
		}
	}
	Field* m_Fields = new Field();
	m_Fields = NULL; // TODO
	return m_Result;
}

bool CQuery::HasFields()
{
	return (m_Fields != NULL);
}

int CQuery::GetAsBinary(LPSTR lpszStatement, LPBYTE OUT lpszReturnBuffer, int size)
{
	QueryResult* res = Fetch(lpszStatement);
	Field* field = NULL; // TODO stub
	std::vector<BYTE> vec = field[0].GetBinary();
	int i = 0;
	for (std::vector<BYTE>::const_iterator it = vec.begin(); it != vec.end(); ++it)
	{
		lpszReturnBuffer[i++] = *it;
	}
	return sizeof(lpszReturnBuffer);
}

void CQuery::SetAsBinary(LPTSTR lpszStatement, LPBYTE lpBinaryBuffer, UINT32 BinaryBufferSize)
{
	// TODO.
/* // Old Implementation.
	CQuery::m_LogToFile.Output(lpszStatement);

	if (g_iShowAllQueriesInDS == TRUE)
	{
	g_Log.AddL(TColor::Aqua, lpszStatement);
	}

	SQLLEN cbValueSize = -100LL - BinaryBufferSize;
	SQLPOINTER pToken;
	BYTE cBUFFER[MAX_BINARY_SIZE];
	SQLRETURN Result;

	SQLBindParameter(this->m_hStmt, 1, SQL_PARAM_INPUT, SQL_C_BINARY , SQL_LONGVARBINARY, BinaryBufferSize, 0, (SQLPOINTER)1, 0, &cbValueSize);
	SQLExecDirect(this->m_hStmt, (SQLTCHAR *)lpszStatement, SQL_NTS);

	Result = SQLParamData(this->m_hStmt, &pToken);

	int lOfs=0;

	while ( Result == SQL_NEED_DATA )
	{
	memcpy(cBUFFER, lpBinaryBuffer, BinaryBufferSize);
	Result = SQLPutData(this->m_hStmt, cBUFFER, BinaryBufferSize);
	Result = SQLParamData(this->m_hStmt, &pToken);
	lOfs += BinaryBufferSize;
	}

	SQLParamData(this->m_hStmt, &pToken);
	this->Close();
*/
	// New Implementation.
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

	LPTSTR str = this->m_Fields->GetCString[iIndex];
	if (str == NULL)
	{
		pOutBuffer[0] = 0;
	}
	else
	{
		strncpy(pOutBuffer, this->m_Fields->GetCString[iIndex], size);
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

/*
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
*/
