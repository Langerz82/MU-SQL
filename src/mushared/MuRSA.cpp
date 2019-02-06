#include "StdAfx.h"
#include "SRSA.h"
#include "MuRSA.h"

int CMuRSA::EncPacket(CPublicRsaKey* key, BYTE* p, int len, BYTE* outp, int& outlen, BYTE protocoltype)
{
	if(protocoltype == 0)
	{
		if(p[0] == 0xC1 || p[0] == 0xC3)
		{
			int buffLen;
			BYTE outBuff[10000];

			key->EncBuffer(&p[3], len-3, outBuff, buffLen);

			int newLen = buffLen+3;

			memcpy(outp, p, 3);
			memcpy(&outp[3], outBuff, buffLen);
			outp[1] = newLen;

			outlen = buffLen+3;

			return 0;
		}

		else if(p[0] == 0xC2 || p[0] == 0xC4)
		{
			int buffLen;
			BYTE outBuff[10000];

			key->EncBuffer(&p[4], len-4, outBuff, buffLen);
	
			int newLen = buffLen+4;

			memcpy(outp, p, 4);
			memcpy(&outp[4], outBuff, buffLen);
			BYTE* nlenH = (BYTE*)&outp[1];
			*nlenH = HIBYTE(newLen);
			BYTE* nlenL = (BYTE*)&outp[2];
			*nlenL = LOBYTE(newLen);

			outlen = buffLen+4;

			return 0;
		}

		else
		{
			return 1;
		}
	}
	
	else
	{
		int buffLen;
		BYTE outBuff[10000];

		key->EncBuffer(&p[3], len-3, outBuff, buffLen);

		int newLen = buffLen+3;
		memcpy(outp, p, 3);
		memcpy(&outp[3], outBuff, buffLen);

		*(WORD*)&outp[1] = (WORD)newLen;

		outlen = buffLen+3;

		return 0;
	}
}

int CMuRSA::DecPacket(CPrivateRsaKey* key, BYTE* p, int len, BYTE* outp, int& outlen, BYTE protocoltype)
{
	if(protocoltype == 0)
	{
		if(p[0] == 0xC1 || p[0] == 0xC3)
		{
			int buffLen;
			BYTE outBuff[10000];

			if(key->DecBuffer(&p[3], len-3, outBuff, buffLen) != 0)
			{
				return 1;
			}

			int newLen = buffLen+3;

			memcpy(outp, p, 3);
			memcpy(&outp[3], outBuff, buffLen);
			outp[1] = newLen;

			outlen = buffLen+3;

			return 0;
		}

		else if(p[0] == 0xC2 || p[0] == 0xC4)
		{
			int buffLen;
			BYTE outBuff[10000];

			if(key->DecBuffer(&p[4], len-4, outBuff, buffLen) != 0)
			{
				return 1;
			}

			int newLen = buffLen+4;

			memcpy(outp, p, 4);
			memcpy(&outp[4], outBuff, buffLen);
			BYTE* nlenH = (BYTE*)&outp[1];
			*nlenH = HIBYTE(newLen);
			BYTE* nlenL = (BYTE*)&outp[2];
			*nlenL = LOBYTE(newLen);

			outlen = buffLen+4;

			return 0;
		}

		else
		{
			return 1;
		}
	}
	
	else
	{
		int buffLen;
		BYTE outBuff[10000];

		int iResult = key->DecBuffer(&p[3], len-3, outBuff, buffLen);

		if(iResult != 0)
		{
			return 1;
		}

		int newLen = buffLen+3;

		memcpy(outp, p, 3);
		memcpy(&outp[3], outBuff, buffLen);
		*(WORD*)&outp[1] = (WORD)newLen;

		outlen = buffLen+3;

		return 0;
	}

}