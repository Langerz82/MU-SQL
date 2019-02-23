////////////////////////////////////////////////////////////////////////////////
// PacketEncrypt.h
#ifndef PACKETENCRYPT_H
#define PACKETENCRYPT_H

#include "StdAfx.h"
#include <cryptopp/rijndael.h>
#include <cryptopp/rsa.h>

class CPacketEncrypt
{

public:

	CPacketEncrypt();
	virtual ~CPacketEncrypt();

	int Encrypt(BYTE * lpDest, BYTE * lpSource, int iSize);
	int Decrypt(BYTE * lpDest, BYTE * lpSource, int iSize);

private:
	CryptoPP::Rijndael::Encryption enc;
	CryptoPP::Rijndael::Decryption dec;
};

extern CPacketEncrypt g_PacketEncrypt;

#endif
