////////////////////////////////////////////////////////////////////////////////
// LogToFile.cpp
#include "stdafx.h"
#include "LogToFile.h"
#include "winutil.h"
#include "TLog.h"
#include "GameMain.h"
#include "configread.h"

void WriteTxt(LPSTR FileName, LPSTR String)
{
	FILE *f;
	f = fopen(FileName, "a+");
	char Print[255] = { 0 };
	wsprintf(Print, "\n%s", String);
	if (fputs(Print, f) == EOF)
	{
		g_Log.AddC(TColor::Red, "Error: Cant Write File: %s", FileName);
	}
	fclose(f);
}

CLogToFile::CLogToFile(const LPSTR LogDataName,const LPSTR LogDirectoryName, DWORD dwMaxLogFileSize)
{
	CreateDirectory(LogDirectoryName, NULL);

	strcpy(this->m_szLogDataName, LogDataName);
	strcpy(this->m_szLogDirectoryName, LogDirectoryName);

	this->m_dwMaxLogFileSize = dwMaxLogFileSize * 1024 * 1024;
	this->m_fLogFile = nullptr;

	memset(this->m_cBUFFER, 0x00, sizeof(this->m_cBUFFER));
	InitializeCriticalSection(&this->m_critLogToFile);

	this->DataSet_Start();
}

CLogToFile::~CLogToFile()
{
	DeleteCriticalSection(&this->m_critLogToFile );
}

void CLogToFile::DataSet_Start()
{
	char szTemp[255];
	memset(szTemp, 0, sizeof(szTemp));

	tm * today;
	time_t ltime;

	time(&ltime);
	today = localtime(&ltime);

	today->tm_year = today->tm_year + 1900;

	this->m_CurDate.Day = today->tm_mday;
	this->m_CurDate.Month = today->tm_mon + 1;
	this->m_CurDate.Year = today->tm_year;

	this->m_StartDate.Day = this->m_CurDate.Day;
	this->m_StartDate.Month = this->m_CurDate.Month;
	this->m_StartDate.Year = this->m_CurDate.Year;

	wsprintf(szTemp, "%s\\%02d.%02d.%04d_%s_%s.log", this->m_szLogDirectoryName,
		this->m_StartDate.Day, this->m_StartDate.Month, this->m_StartDate.Year, this->m_szLogDataName, g_ConfigRead.server.GetServerName());

	if (IsFile(szTemp) == TRUE)
	{
		for (int i = 2; ; i++)
		{
			memset(szTemp, 0, sizeof(szTemp));

			wsprintf(szTemp, "%s\\%02d.%02d.%04d_%s_%s_start-%d.log", this->m_szLogDirectoryName,
				this->m_StartDate.Day, this->m_StartDate.Month, this->m_StartDate.Year, this->m_szLogDataName, g_ConfigRead.server.GetServerName(), i);

			if (IsFile(szTemp) == false)
			{
				break;
			}
		}
	}

	memset(this->m_szLogFileName_Orig, 0, sizeof(this->m_szLogFileName_Orig));
	memcpy(this->m_szLogFileName_Orig, szTemp, strlen(szTemp) - 4);
	strcpy(this->m_szLogFileName, szTemp);

	if (this->m_fLogFile == nullptr)
	{
		this->m_fLogFile = fopen(this->m_szLogFileName, "a+t");

		if (this->m_fLogFile == nullptr)
		{
			g_Log.MsgBox("Log file create error.");
		}
	}
}

void CLogToFile::DataSet_Check()
{
	tm * today;
	time_t ltime;

	time(&ltime);
	today = localtime(&ltime);

	today->tm_year = today->tm_year + 1900;

	int iYear = today->tm_year;
	int iMonth = today->tm_mon + 1;
	int iDay = today->tm_mday;

	char szTemp[255];
	memset(szTemp, 0, sizeof(szTemp));

	EnterCriticalSection(&this->m_critLogToFile);

	if ((iDay == this->m_CurDate.Day) && (iMonth == this->m_CurDate.Month) && (iYear == this->m_CurDate.Year))
	{
		DWORD dwFileSize = CalcFileSize(this->m_szLogFileName);

		if (dwFileSize < this->m_dwMaxLogFileSize)
		{
			LeaveCriticalSection(&this->m_critLogToFile);
			return;
		}

		CreateDirectory("Log", NULL);

		if (this->m_fLogFile != nullptr)
		{
			fclose(this->m_fLogFile);
			this->m_fLogFile = nullptr;
		}

		for (int i = 1; ; i++)
		{
			memset(szTemp, 0, sizeof(szTemp));

			wsprintf(szTemp, "%s_part-%d.log", this->m_szLogFileName_Orig, i);

			if (IsFile(szTemp) == false)
			{
				break;
			}
		}
	}
	else
	{
		this->m_CurDate.Day = iDay;
		this->m_CurDate.Month = iMonth;
		this->m_CurDate.Year = iYear;

		if (this->m_fLogFile != nullptr)
		{
			fclose(this->m_fLogFile);
			this->m_fLogFile = nullptr;
		}

		CreateDirectory("Log", NULL);
		wsprintf(szTemp, "%s\\%02d.%02d.%04d_-_%02d.%02d.%04d_%s_%s.log", this->m_szLogDirectoryName,
			this->m_StartDate.Day, this->m_StartDate.Month, this->m_StartDate.Year, 
			this->m_CurDate.Day, this->m_CurDate.Month, this->m_CurDate.Year, this->m_szLogDataName, g_ConfigRead.server.GetServerName());

		char szTemp2[255];
		sprintf(szTemp2, "%s\\%02d.%02d.%04d_%s_%s.log", this->m_szLogDirectoryName,
			this->m_CurDate.Day, this->m_CurDate.Month, this->m_CurDate.Year, this->m_szLogDataName, g_ConfigRead.server.GetServerName());

		memset(this->m_szLogFileName_Orig, 0x00, sizeof(this->m_szLogFileName_Orig));
		memcpy(this->m_szLogFileName_Orig, szTemp2, strlen(szTemp2) - 4);
	}

	strcpy(this->m_szLogFileName, szTemp);

	if (this->m_fLogFile == nullptr)
	{
		this->m_fLogFile = fopen(this->m_szLogFileName, "a+t");

		if (this->m_fLogFile == nullptr)
		{
			g_Log.MsgBox("Log File create error.");
		}
	}

	LeaveCriticalSection(&this->m_critLogToFile);
}

void CLogToFile::Output(LPSTR fmt, ...)
{
	char szArgBuff[2048];
	char szLogBuff[2048];
	memset(szArgBuff, 0, sizeof(szArgBuff));
	memset(szLogBuff, 0, sizeof(szLogBuff));

	std::string data = fmt;
	std::string toSearch = "%n";
	std::string replaceStr = "n";
	// Get the first occurrence
	size_t pos = data.find(toSearch);
	// Repeat till end is reached
	while (pos != std::string::npos)
	{
		// Replace this occurrence of Sub String
		data.replace(pos, toSearch.size(), replaceStr);
		// Get the next occurrence from the current position
		pos = data.find(toSearch, pos + toSearch.size());
	}
	char * outChar = new char[data.length() + 1];
	std::strcpy(outChar, data.c_str());

	va_list lpArguments;

	va_start(lpArguments, fmt);
	vsprintf(szArgBuff, outChar, lpArguments);
	va_end(lpArguments);

	tm * today;
	time_t ltime;

	time(&ltime);
	today = localtime(&ltime);

	sprintf(szLogBuff, "[%.8s] ", asctime(today) + 11);
	strcat(szLogBuff, szArgBuff);

	EnterCriticalSection(&this->m_critLogToFile);

	fprintf(this->m_fLogFile, "%s\n", szLogBuff);
	fflush(this->m_fLogFile);

	LeaveCriticalSection(&this->m_critLogToFile);

	DataSet_Check();
}

	




////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

