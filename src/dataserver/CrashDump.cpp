// CrashDump.cpp: implementation of the CCrashDump class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define szModule "CCrashDump"

CCrashDump g_CrashDump;

void (*CCrashDump::m_fn)();
MINIDUMPWRITEDUMP CCrashDump::m_fnMiniDump;

CCrashDump::CCrashDump():
m_hDll(NULL)
{


	TCHAR szDbgHelpPath[MAX_PATH]={0};

	// ������� ��������� ��������� ���������� ����� � EXE, �.�.
	// � System32 ����� ���� ������ ������.

	m_hDll = ::LoadLibrary( ".\\DBGHELP.DLL" );

	if (m_hDll==NULL)
	{
		// ���� �������� �� ������� - ������� ��������� �����
		// ��������� ������
		m_hDll = ::LoadLibrary( "DBGHELP.DLL" );
	}

	//.//MessageBoxA(0,"DRAKONS",0,MB_OK|MB_TOPMOST);

	if(m_hDll)
	{
		//���� ���������� ��������� - �������� ����� MiniDumpWriteDump()
		m_fnMiniDump = (MINIDUMPWRITEDUMP)::GetProcAddress( m_hDll, "MiniDumpWriteDump" );
		
		//���� �� �� - ��������� �����������
		if(m_fnMiniDump)
		{
			::SetUnhandledExceptionFilter( TopLevelFilter );
		}
	}
}

CCrashDump::~CCrashDump()
{

	if(m_hDll)
		FreeLibrary(m_hDll);
}

void CCrashDump::SetCrashCallFun(void (*fn)())
{
	m_fn = fn;
}

LONG __stdcall CCrashDump::TopLevelFilter( struct _EXCEPTION_POINTERS *pExceptionInfo )
{
	//�� ��������� �������� ������� ����������� �������
	LONG retval = EXCEPTION_CONTINUE_SEARCH;
 
 
	LPCTSTR szResult = NULL;
 
	TCHAR        DumpPath[MAX_PATH] = { 0, };
	SYSTEMTIME    SystemTime;

	GetLocalTime(&SystemTime);

	sprintf_s(DumpPath, MAX_PATH, _T("%d-%d-%d_%dh%dm%ds-DS.dmp"),
		SystemTime.wYear,
		SystemTime.wMonth,
		SystemTime.wDay,
		SystemTime.wHour,
		SystemTime.wMinute,
		SystemTime.wSecond); 

	// ������� ����
	HANDLE hFile = ::CreateFile( DumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, 
	                            CREATE_ALWAYS,
								FILE_ATTRIBUTE_NORMAL, NULL );


	if (hFile != INVALID_HANDLE_VALUE)
	{
		_MINIDUMP_EXCEPTION_INFORMATION ExInfo;
 
		ExInfo.ThreadId = ::GetCurrentThreadId();
		ExInfo.ExceptionPointers = pExceptionInfo;
		ExInfo.ClientPointers = NULL;

		// � �������� � ���� ����
		BOOL bOK = m_fnMiniDump( GetCurrentProcess(), GetCurrentProcessId(), 
								hFile, (MINIDUMP_TYPE)262, &ExInfo, NULL, NULL );

		retval = EXCEPTION_EXECUTE_HANDLER;
		::CloseHandle(hFile);

	}

	if(m_fn)
		m_fn();
 
	return retval;
}


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

