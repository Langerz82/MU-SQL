////////////////////////////////////////////////////////////////////////////////
// WzMemScript2.h
// ------------------------------
// Decompiled by Deathway
// Date : 2007-03-09
// ------------------------------
#ifndef WZMEMSCRIPT
#define WZMEMSCRIPT

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ReadScript.h"




class CWzMemScript
{
public:

	CWzMemScript();
	virtual ~CWzMemScript();

	int SetBuffer( char* buffer, int buffersize);
	void SetScriptParsingBuffer(char* buffer, int size);
	enum SMDToken GetToken();
	//enum SMDToken __thiscall TokenType();
	int GetNumber();
	char* GetString();

protected:

	int GetC();
	void UnGetC(int ch);

private:

	char* m_pBuffer;	// 4
	int m_iBufferSize;	// 8
	int m_iBufferCount;	// C
	HANDLE m_hFile;	// 10
	char * m_Token;	// 14
	float TokenNumber;	// 18
	char TokenString[100];	// 1C
	enum SMDToken CurrentToken;	// 80
};

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

