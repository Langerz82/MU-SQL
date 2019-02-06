////////////////////////////////////////////////////////////////////////////////
// PacketEncrypt.h
#ifndef PACKETENCRYPT_H
#define PACKETENCRYPT_H

#include <rijndael.h>
#include <rsa.h>

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

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

