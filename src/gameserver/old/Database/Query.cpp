#include "StdAfx.h"
#include "Query.h"
//#include "DelayHandler.h"
#include "Database/Database/Field.h"

/*void CQuery::Open(DatabaseWorkerPool<ConnectDatabaseConnection> &db)
{
	this->m_Database = db;
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
	this->m_Database.Execute(lpszStatement);

	return TRUE;
}

QueryResult* CQuery::Fetch(TCHAR* lpszStatement, ...)
{
	TCHAR szStatement[2048];

	va_list pArguments;
	va_start(pArguments, lpszStatement);
	vsprintf(szStatement, lpszStatement, pArguments);
	va_end(pArguments);

	this->m_Result = this->m_Database.Query(szStatement);

	if (!this->m_Result)
	{
		sLog->outError("[SQL Error] Error querying: %s", szStatement);
		return nullptr;
	}

	return &this->m_Result;
}

int CQuery::Fetch()
{
	if (this->m_Result->GetRowCount() == 0) 
		return 0;
	return 1;
}

int CQuery::GetAsBinary(LPSTR lpszStatement, BYTE* OUT lpszReturnBuffer, int size)
{
	QueryResult* res = Fetch(lpszStatement);
	std::vector<BYTE> bytes = (*this->m_Result)[0].GetBinary();
	int i = 0;
	for (std::vector<BYTE>::const_iterator it = bytes.begin(); it != bytes.end(); ++it)
	{
		lpszReturnBuffer[i++] = *it;
	}
	return sizeof(lpszReturnBuffer);
}

//void CQuery::SetAsBinary(LPSTR lpszStatement, BYTE* lpBinaryBuffer, UINT32 BinaryBufferSize)
//{
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
	std::memcpy(cBUFFER, lpBinaryBuffer, BinaryBufferSize);
	Result = SQLPutData(this->m_hStmt, cBUFFER, BinaryBufferSize);
	Result = SQLParamData(this->m_hStmt, &pToken);
	lOfs += BinaryBufferSize;
	}

	SQLParamData(this->m_hStmt, &pToken);
	this->Close();
*/
	// New Implementation.
//}


/*int CQuery::FindIndex(LPSTR ColName)
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

void CQuery::GetAsString(LPSTR ColName, LPSTR pOutBuffer, int size)
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

void CQuery::GetAsString(int iIndex, LPSTR pOutBuffer, int size)
{
	if (iIndex == -1)
	{
		pOutBuffer[0] = 0;
	}

	if ((*this->m_Result)[iIndex].GetCString() == NULL)
	{
		pOutBuffer[0] = 0;
	}
	else
	{
		strncpy(pOutBuffer, (*this->m_Result)[iIndex].GetCString(), size);
	}
}

/*DWORD CQuery::GetAsInteger(LPSTR ColName)
{
	int iIndex = this->FindIndex(ColName);

	if (iIndex != -1)
	{
		return this->GetAsInteger(userIndex);
	}

	return -1;
}*/

DWORD CQuery::GetAsInteger(int iIndex)
{
	if (iIndex == -1)
	{
		return -1;
	}

	return (*this->m_Result)[iIndex].GetInt32();
}

/*INT64 CQuery::GetAsInteger64(LPSTR ColName)
{
	int iIndex = this->FindIndex(ColName);

	if (iIndex != -1)
	{
		return this->GetAsInteger64(userIndex);
	}

	return -1LL;
}*/

INT64 CQuery::GetAsInteger64(int iIndex)
{
	if (iIndex == -1)
	{
		return -1LL;
	}
	return (*this->m_Result)[iIndex].GetInt64();
}

/*float CQuery::GetAsFloat(LPSTR ColName)
{
	int iIndex = this->FindIndex(ColName);

	if (iIndex != -1)
	{
		return this->GetAsFloat(userIndex);
	}

	return -1;
}*/

float CQuery::GetAsFloat(int iIndex)
{
	if (iIndex == -1)
	{
		return -1;
	}
	return (*this->m_Result)[iIndex].GetFloat();
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
