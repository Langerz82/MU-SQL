// ------------------------------
// Decompiled by Deathway
// Date : 2007-03-09
// ------------------------------
// WinUtil.cpp
//	GS-N	1.00.18	JPN	0x004A4E40	-	Completed

//#include "stdafx.h"
#include "WinUtil.h"
//#include <Iphlpapi.h>
//#include <Assert.h>
//#pragma comment(lib, "iphlpapi.lib")
//#include <rpc.h>
//#include <rpcdce.h>
//#pragma comment(lib, "rpcrt4.lib")
//#include "../stdafx.h"

static unsigned char bBuxCode[3]	=	{0xED, 0xBE, 0x9A};	

int INT_MULTIPLER = 3;

void BuxConvert(char* buf, int size)
{
	int n = 0;
	// ----
	for (n=0;n<size;n++)
	{
		buf[n]^=bBuxCode[n%3] ;		// Nice trick from WebZen
	}
}


void PHeadSetB(LPBYTE lpBuf, BYTE head, int size)
{
	lpBuf[0] =0xC1;		// Packets
	lpBuf[1] =size;
	lpBuf[2] =head;
}
	

void PHeadSubSetB(LPBYTE lpBuf, BYTE head, BYTE sub, int size)
{
	lpBuf[0] =0xC1;	// Packets
	lpBuf[1] =size;
	lpBuf[2] =head;
	lpBuf[3] =sub;
}

void PHeadSetW(LPBYTE lpBuf, BYTE head,  int size) 
{
	lpBuf[0] = 0xC2;	// Packets Header
	lpBuf[1]= SET_NUMBERH(size);
	lpBuf[2]= SET_NUMBERL(size);
	lpBuf[3]= head;
}

void PHeadSubSetW(LPBYTE lpBuf, BYTE head, BYTE sub,  int size)
{
	lpBuf[0]=0xC2;	// packet header
	lpBuf[1]=SET_NUMBERH(size);
	lpBuf[2]=SET_NUMBERL(size);
	lpBuf[3]=head;
	lpBuf[4]=sub;
}


void PHeadSetBE( LPBYTE lpBuf, BYTE head,int size)
{
	lpBuf[0] =0xC3;
	lpBuf[1] =size;
	lpBuf[2] =head;
}

void PHeadSubSetBE(LPBYTE lpBuf, BYTE head,BYTE sub, int size)
{
	lpBuf[0] =0xC3;
	lpBuf[1] =size;
	lpBuf[2] =head;
	lpBuf[3] =sub;
}