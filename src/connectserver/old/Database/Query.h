// Query.h: interface for the CQuery class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUERY_H__8A3116E5_E735_4FF9_A61D_5566130C3610__INCLUDED_)
#define AFX_QUERY_H__8A3116E5_E735_4FF9_A61D_5566130C3610__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "Common/Common.h"
#include "database/Database/DatabaseEnv.h"
#include "database/Database/DatabaseWorkerPool.h"
#include "Main.h"
#include "database/Database/Implementation/CSDatabase.h"
#include "database/Database/QueryResult.h"

//class ConnectDatabaseConnection;
//class MySQLConnection;


class CQuery  
{
public:
	CQuery() {};
	CQuery(DatabaseWorkerPool<ConnectDatabaseConnection>* db);
	//CQuery(DatabaseWorkerPool<GameDatabaseConnection>* db);
	virtual ~CQuery();

	//BOOL ReConnect();
	//void Disconnect();

	BOOL ExecQuery(TCHAR* lpszStatement, ...);
	BOOL Execute(TCHAR* lpszStatement);
	QueryResult* Fetch(TCHAR* lpszStatement, ...);
	int Fetch();
	//bool HasFields();

	//int GetAsBinary(LPSTR lpszStatement, LPBYTE OUT lpszReturnBuffer, int size);
	//void SetAsBinary(LPTSTR lpszStatement, LPBYTE lpBinaryBuffer, UINT32 BinaryBufferSize);

	//int GetResult(int index);
	
	//void GetAsString(LPTSTR ColName, LPTSTR pOutBuffer, int size);
	//DWORD GetAsInteger(LPTSTR ColName);
	//INT64 GetAsInteger64(LPTSTR ColName);
	//float GetAsFloat(LPTSTR ColName);

	//int FindIndex(LPTSTR ColName);
	void GetAsString(int iIndex, LPTSTR pOutBuffer, int size);
	DWORD GetAsInteger(int iIndex);
	INT64 GetAsInteger64(int iIndex);
	float GetAsFloat(int iIndex);

	//void Close();
	//void Diagnosis(bool &bReconnect);

	DatabaseWorkerPool<ConnectDatabaseConnection>* m_Database;
	//void* m_Database;
	QueryResult* m_Result;

};

#endif

