#ifndef SRSA_H
#define SRSA_H

#include "RNG.h"

class IgcInt;

#pragma pack(push, 1)
typedef struct t_RSA_DATA_HEADER{
	DWORD crc;
	WORD size;
	DWORD old_crc;
	WORD old_size;
} RSA_DATA_HEADER, *PRSA_DATA_HEADER, *LPRSA_DATA_HEADER;
#pragma pack(pop)

class CPublicRsaKey{
public:
	//	CPublicRsaKey();
	int m_Size;

	mpz_t m_N;
	BYTE* m_NBuff;
	int m_NSize;

	mpz_t m_E;
	BYTE* m_EBuff;
	int m_ESize;

	//	CPrivateRsaKey();
	__fastcall CPublicRsaKey(int KeySize, BYTE* NKey, int NSize, BYTE* EKey, int ESize);
	__fastcall CPublicRsaKey(int KeySize, IgcInt NKey, IgcInt EKey);

	~CPublicRsaKey();


	void __fastcall EncBuffer(BYTE* buff, int size, BYTE* outBuff, int& outSize);
};

class CPrivateRsaKey{
public:
	int m_Size;

	mpz_t m_N;
	BYTE* m_NBuff;
	int m_NSize;

	mpz_t m_D;
	BYTE* m_DBuff;
	int m_DSize;

	//	CPrivateRsaKey();
	~CPrivateRsaKey();
	__fastcall CPrivateRsaKey(int KeySize, BYTE* NKey, int NSize, BYTE* DKey, int DSize);
	__fastcall CPrivateRsaKey(int KeySize, IgcInt NKey, IgcInt DKey);


	int __fastcall DecBuffer(BYTE* buff, int size, BYTE* outBuff, int& outSize);
};

class CRsa{
private:
	CPublicRsaKey* m_Pub;
	CPrivateRsaKey* m_Priv;

	int m_Bits;
	int m_Size;

public:
	// Default constructors
	// Generate Random RSA Key
	__fastcall CRsa(int bits);
	__fastcall CRsa(int bits, RNG rng);
	~CRsa();
	// Insert RSA Key
	//CRsa(CPublicRsaKey* pKey);
	//CRsa(CPrivateRsaKey* pKey);



	CPublicRsaKey* GetPublicRsaKey();
	CPrivateRsaKey* GetPrivateRsaKey();
};

#endif