// Query.h: interface for the CQuery class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUERY_H__8A3116E5_E735_4FF9_A61D_5566130C3610__INCLUDED_)
#define AFX_QUERY_H__8A3116E5_E735_4FF9_A61D_5566130C3610__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "Common\Common.h"
#include "database/Database/DatabaseEnv.h"
#include "database/Database/MySQLConnection.h"
#include "database/Database/Field.h"
#include "Database/CSDatabase.h"
//#include "Database/GSDatabase.h"

class Field;
class MySQLConnection;

class CQuery  
{
public:
	CQuery() {};
	CQuery(DatabaseWorkerPool<MySQLConnection>* db);
	//CQuery(DatabaseWorkerPool<GameDatabaseConnection>* db);
	virtual ~CQuery();

	//BOOL ReConnect();
	//void Disconnect();

	BOOL ExecQuery(TCHAR* lpszStatement, ...);
	BOOL Execute(TCHAR* lpszStatement);
	QueryResult* Fetch(TCHAR* lpszStatement, ...);
	int Fetch();
	//bool HasFields();

	int GetAsBinary(LPSTR lpszStatement, LPBYTE OUT lpszReturnBuffer, int size);
	void SetAsBinary(LPTSTR lpszStatement, LPBYTE lpBinaryBuffer, UINT32 BinaryBufferSize);

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

	DatabaseWorkerPool<MySQLConnection>* m_Database;
	QueryResult* m_Result;

};

#endif // !defined(AFX_QUERY_H__8A3116E5_E735_4FF9_A61D_5566130C3610__INCLUDED_)


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

