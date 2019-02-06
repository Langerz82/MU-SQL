////////////////////////////////////////////////////////////////////////////////
// PacketEncrypt.cpp
// SimpleModulus.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "PacketEncrypt.h"
//bp on 66 0FEF 07
static BYTE btKey[32] = { 0x44, 0x9D, 0x0F, 0xD0, 0x37, 0x22, 0x8F, 0xCB, 0xED, 0x0D, 0x37, 0x04, 0xDE, 0x78, 0x00, 0xE4, 0x33, 0x86, 0x20, 0xC2, 0x79, 0x35, 0x92, 0x26, 0xD4, 0x37, 0x37, 0x30, 0x98, 0xEF, 0xA4, 0xDE };
//static BYTE btKey[32] = { 0x7A, 0x45C, 0x74, 0x6D, 0xB5, 0x43, 0xF7, 0x12, 0x4A, 0x18, 0x8D, 0x94, 0x7A, 0x56, 0x71, 0x01, 0x44, 0x19, 0xE6, 0x83, 0x78, 0x46, 0x86, 0xDB, 0xBE, 0x7D, 0x69, 0x56, 0x45, 0x34, 0x23, 0x12 };

CPacketEncrypt g_PacketEncrypt;

CPacketEncrypt::CPacketEncrypt()
{ 
	this->enc.SetKey(btKey, 32);
	this->dec.SetKey(btKey, 32);
}

CPacketEncrypt::~CPacketEncrypt()
{
	return;
}

int CPacketEncrypt::Decrypt(BYTE * lpDest, BYTE * lpSource, int iSize)
{
	if (lpSource == NULL || lpDest == NULL || iSize == NULL)
	{
		return -1;
	}

	int remainder = iSize % this->dec.BLOCKSIZE;
	int outLen = 0;

	if (remainder == 1)
	{
		int paddingSize = lpSource[iSize - 1];
		outLen = iSize - paddingSize - 1;

		if (lpDest != NULL)
		{
			if (outLen > 0 && outLen <= 16384)
			{
				BYTE obuf[16384];
				BYTE ibuf[16384];

				memset(ibuf, 0, iSize);
				memset(obuf, 0, iSize);

				memcpy(ibuf, lpSource, iSize);

				for (int i = 0; i < outLen; i += this->dec.BLOCKSIZE)
				{
					this->dec.ProcessBlock(&ibuf[i], &obuf[i]);
				}

				memcpy(lpDest, obuf, outLen);
			}
		}

		return outLen;
	}
       
	return -1;
}

int CPacketEncrypt::Encrypt(BYTE * lpDest, BYTE * lpSource, int iSize)
{
	if (lpSource == NULL || lpDest == NULL || iSize <= 0 || iSize > 16384)
	{
		return iSize;
	}

	int paddingSize = 0;
	int remainder = iSize % this->enc.BLOCKSIZE;

	if (remainder >= 0)
	{
		paddingSize = this->enc.BLOCKSIZE - remainder;
	}
	else
	{
		return iSize;
	}

	int outLen = iSize + paddingSize + 1;

	if (lpDest != NULL)
	{
		BYTE padding[64];
		BYTE obuf[16384];
		BYTE tbuf[16384];

		memset(padding, 0, sizeof(padding));
		memset(tbuf, 0, outLen);
		memset(obuf, 0, outLen);

		memcpy(tbuf, lpSource, iSize);
		memcpy(&tbuf[iSize], padding, paddingSize);

		for (int i = 0; i < outLen; i += this->enc.BLOCKSIZE)
		{
			this->enc.ProcessBlock(&tbuf[i], &obuf[i]);
		}

		obuf[outLen - 1] = paddingSize;
		memcpy(lpDest, obuf, outLen);
	}

	return outLen;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

