// Query.h: interface for the CQuery class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUERY_H__8A3116E5_E735_4FF9_A61D_5566130C3610__INCLUDED_)
#define AFX_QUERY_H__8A3116E5_E735_4FF9_A61D_5566130C3610__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "Main.h"
//#include "database/Database/DatabaseEnv.h"
#include "database/Database/QueryResult.h"

//class ConnectDatabaseConnection;
//class MySQLConnection;

class CQuery  
{
public:
	CQuery():m_Database(gMUDatabase) {};
	//CQuery(DatabaseWorkerPool<ConnectDatabaseConnection> &db): m_Database(db) {};
	//void Open(DatabaseWorkerPool<ConnectDatabaseConnection> &db);
	//CQuery(DatabaseWorkerPool<GameDatabaseConnection>* db);
	~CQuery();

	//BOOL ReConnect();
	//void Disconnect();

	BOOL ExecQuery(TCHAR* lpszStatement, ...);
	BOOL Execute(TCHAR* lpszStatement);
	QueryResult* Fetch(TCHAR* lpszStatement, ...);
	int Fetch();
	//bool HasFields();

	int GetAsBinary(LPSTR lpszStatement, LPBYTE lpszReturnBuffer, int size);
	//void SetAsBinary(LPSTR lpszStatement, LPBYTE lpBinaryBuffer, UINT32 BinaryBufferSize);

	//int GetResult(int index);
	
	//void GetAsString(LPSTR ColName, LPSTR pOutBuffer, int size);
	//DWORD GetAsInteger(LPSTR ColName);
	//INT64 GetAsInteger64(LPSTR ColName);
	//float GetAsFloat(LPSTR ColName);

	//int FindIndex(LPSTR ColName);
	void GetAsString(int iIndex, LPSTR pOutBuffer, int size);
	DWORD GetAsInteger(int iIndex);
	INT64 GetAsInteger64(int iIndex);
	float GetAsFloat(int iIndex);

	//void Close();
	//void Diagnosis(bool &bReconnect);

	DatabaseWorkerPool<MUDatabase> &m_Database;
	//void* m_Database;
	QueryResult m_Result;

};

#endif

