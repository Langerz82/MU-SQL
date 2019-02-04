////////////////////////////////////////////////////////////////////////////////
// LogToFile.h
// ------------------------------
// Decompiled by Deathway
// Date : 2007-03-09
// ------------------------------
#ifndef LOGTOFILE_H
#define LOGTOFILE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct
{
	int Day;
	int Month;
	int Year;
} LOGTOFILEDATE;

void WriteTxt(LPSTR FileName, LPSTR String);

class CLogToFile
{

public:

	void Output(LPSTR fmt, ...);
	CLogToFile(const LPSTR LogDataName,const LPSTR LogDirectoryName, DWORD dwMaxLogFileSize = 256);
	~CLogToFile();

private:

	void DataSet_Start();
	void DataSet_Check();

	FILE* m_fLogFile;	// 0
	char m_szLogDataName[260];
	char m_szLogDirectoryName[260];
	char m_szLogFileName[260];
	char m_szLogFileName_Orig[260];
	char m_cBUFFER[65536];	// 20C
	DWORD m_dwMaxLogFileSize;
	CRITICAL_SECTION m_critLogToFile; 
	LOGTOFILEDATE m_StartDate;
	LOGTOFILEDATE m_CurDate;
};

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

