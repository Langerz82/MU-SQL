// Query.h: interface for the CQuery class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUERY_H__8A3116E5_E735_4FF9_A61D_5566130C3610__INCLUDED_)
#define AFX_QUERY_H__8A3116E5_E735_4FF9_A61D_5566130C3610__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LogToFile.h"

#define MAX_COLUMNS	100
#define MAX_BINARY_SIZE	10000

class CQuery  
{
public:
	enum 
	{
		MAXCOL = 100, BLOBBATCH = 10000, CQUERYNULL = -100, CQUERYEOF = -101,
		CQUERYNOCOL = -102, CQUERYERROR = -103
	};
public:

	CQuery();
	virtual ~CQuery();

	BOOL Connect(LPTSTR lpszDNS, LPTSTR lpszUser, LPTSTR lpszPassword, LPSTR lpszServerName);
	BOOL ReConnect();
	void Disconnect();

	BOOL ExecQuery(LPTSTR lpszStatement, ...);
	SQLRETURN Fetch();
	void Close();

	int GetResult(int iIndex);
	void GetAsString(LPTSTR ColName, LPTSTR pOutBuffer,int size);
	DWORD GetAsInteger(LPTSTR ColName);
	INT64 GetAsInteger64(LPTSTR ColName);
	float GetAsFloat(LPTSTR ColName);

	int GetAsBinary(LPSTR lpszStatement, LPBYTE OUT lpszReturnBuffer, int size);
	void SetAsBinary(LPTSTR lpszStatement, LPBYTE lpBinaryBuffer, SQLUINTEGER BinaryBufferSize);

	SQLRETURN BindParameterBinaryOutput(int nCol, BYTE *nValue, int iSize, SQLLEN *lLength);

	BOOL SQLSyntexCheck(char* SQLString);
	BOOL SpaceSyntexCheck(char* string);

	int ReadBlob(LPCTSTR szSQL, void *buf);
	void Diagnosis(bool &bReconnect);

	int FindIndex(LPTSTR ColName);
	void GetAsString(int iIndex, LPTSTR pOutBuffer, int size);
	DWORD GetAsInteger(int iIndex);
	INT64 GetAsInteger64(int iIndex);
	float GetAsFloat(int iIndex);

	SQLRETURN ret;
	SQLHANDLE m_hEnviroment;
	SQLHANDLE m_hConnection;
	SQLHANDLE m_hStmt;
	TCHAR m_szDNS[64];
	TCHAR m_szUser[64];
	TCHAR m_szPassword[64];
	SQLSMALLINT m_ColCount;
	SQLTCHAR m_SQLColName[MAX_COLUMNS][30];
	TCHAR m_SQLData[MAX_COLUMNS][256];
	SQLLEN m_SQLDataLen[MAX_COLUMNS];
	static CLogToFile m_LogToFile;
};

#endif // !defined(AFX_QUERY_H__8A3116E5_E735_4FF9_A61D_5566130C3610__INCLUDED_)


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

