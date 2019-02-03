// CrashDump.h: interface for the CCrashDump class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CRASHDUMP_H__4B7112C7_4DEF_415D_8A62_9E167CBD5662__INCLUDED_)
#define AFX_CRASHDUMP_H__4B7112C7_4DEF_415D_8A62_9E167CBD5662__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


typedef BOOL (__stdcall *MINIDUMPWRITEDUMP)(
  HANDLE hProcess,
  DWORD ProcessId,
  HANDLE hFile,
  MINIDUMP_TYPE DumpType,
  PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
  PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
  PMINIDUMP_CALLBACK_INFORMATION CallbackParam
);

class CCrashDump  
{
public:
	CCrashDump();
	virtual ~CCrashDump();

	void SetCrashCallFun(void (*fn)());

	static void (*m_fn)();
	static MINIDUMPWRITEDUMP m_fnMiniDump;

private:
	HMODULE m_hDll;

	static LONG __stdcall TopLevelFilter( struct _EXCEPTION_POINTERS *pExceptionInfo );

};

extern CCrashDump g_CrashDump;

#endif // !defined(AFX_CRASHDUMP_H__4B7112C7_4DEF_415D_8A62_9E167CBD5662__INCLUDED_)


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

