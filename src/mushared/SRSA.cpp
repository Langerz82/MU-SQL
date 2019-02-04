#include "StdAfx.h"
#include "SRSA.h"
#include "CRC.h"
#include "IgcInt.h"
#include "IgcMath.h"

#pragma optimize("gpty", on)
/*************************************************************************************************/
/********* RSA Public key class	******************************************************************/
/*************************************************************************************************/
__fastcall CPublicRsaKey::CPublicRsaKey( int KeySize, BYTE* NKey, int NSize, BYTE* EKey, int ESize )
{
	VM_TIGER_BLACK_START
	m_Size = KeySize;

	mpz_init(m_N);
	mpz_init(m_E);

	mpz_import(m_N, NSize, 1, sizeof(BYTE), 1, 0, NKey);
	mpz_import(m_E, ESize, 1, sizeof(BYTE), 1, 0, EKey);

	m_NBuff = new BYTE[NSize];
	memcpy(m_NBuff, NKey, NSize);

	m_EBuff = new BYTE[ESize];
	memcpy(m_EBuff, EKey, ESize);

	m_NSize = NSize;
	m_ESize = ESize;
	VM_TIGER_BLACK_END
}

__fastcall CPublicRsaKey::CPublicRsaKey( int KeySize, IgcInt NKey, IgcInt EKey )
{
	VM_TIGER_BLACK_START
	m_Size = KeySize;

	mpz_init(m_N);
	mpz_set(m_N, NKey.value);

	size_t count;
	m_NSize = mpz_size(m_N)*sizeof(mp_limb_t);
	m_NBuff = new BYTE[m_NSize];
	mpz_export(m_NBuff, &count, 1, m_NSize, 1, 0, m_N);
/*	size_t count;
	m_NSize = mpz_size(NKey.value)*4;
	m_NBuff = new BYTE[m_NSize];
	mpz_export(m_NBuff, &count, 1, m_NSize, 1, 0, m_N);
	*/

	mpz_init(m_E);
	mpz_set(m_E, EKey.value);

	m_ESize = mpz_size(m_E)*sizeof(mp_limb_t);
	m_EBuff = new BYTE[m_ESize];
	mpz_export(m_EBuff, &count, 1, m_ESize, 1, 0, m_E);
/*	m_ESize = mpz_size(EKey.value)*4;
	m_EBuff = new BYTE[m_ESize];
	mpz_export(m_EBuff, &count, 1, m_ESize, 1, 0, m_E);*/
	VM_TIGER_BLACK_END
}

void __fastcall CPublicRsaKey::EncBuffer(BYTE* buff, int size, BYTE* outBuff, int& outSize )
{
	VM_TIGER_RED_START
	int BlockSize = m_Size-1;
	int MemBlocks = size/BlockSize;

	if (size%BlockSize != 0)
	{
		MemBlocks++;
	}

	DWORD crc = ComputeCRC((BYTE*)buff, size);

	RSA_DATA_HEADER h;
	h.old_size = size;
	h.old_crc = crc;

	int TempDataSize = MemBlocks*BlockSize;
	int EncDataSize = MemBlocks*m_Size;
	BYTE tempData[10000];
	BYTE encData[10000];

	memset(tempData, 0, TempDataSize);
	memcpy(tempData, buff, size);
	memset(encData, 0, EncDataSize);

	int EncBlockSize = m_Size;

	for (int i = 0; i < MemBlocks; i++)
	{
		mpz_t toEnc;
		mpz_init(toEnc);
		mpz_import(toEnc, BlockSize, 1, sizeof(BYTE), 1, 0, &tempData[i*BlockSize]);

		mpz_t enc;
		mpz_init(enc);
		mpz_powm(enc, toEnc, m_E, m_N);

		size_t count;
		mpz_export(&encData[i*EncBlockSize], &count, 1, EncBlockSize, 1, 0, enc);

		mpz_clear(toEnc);
		mpz_clear(enc);
	}

	h.crc = ComputeCRC(encData, EncDataSize);
	h.size = EncDataSize+sizeof(RSA_DATA_HEADER);

	BYTE finalData[10000];

	memcpy(finalData, &h, sizeof(RSA_DATA_HEADER));
	memcpy(&finalData[sizeof(RSA_DATA_HEADER)], encData, EncDataSize);
	memcpy(outBuff, finalData, sizeof(finalData));

	outSize = EncDataSize+sizeof(RSA_DATA_HEADER);
	VM_TIGER_RED_END
}

__fastcall CPublicRsaKey::~CPublicRsaKey()
{
	mpz_clear(m_N);
	mpz_clear(m_E);

	delete [] m_NBuff;
	delete [] m_EBuff;
}

/*************************************************************************************************/
/********* RSA Private key class	******************************************************************/
/*************************************************************************************************/

__fastcall CPrivateRsaKey::CPrivateRsaKey(int KeySize, BYTE* NKey, int NSize, BYTE* DKey, int DSize)
{
	VM_TIGER_BLACK_START
	m_Size = KeySize;

	mpz_init(m_N);
	mpz_init(m_D);

	mpz_import(m_N, NSize, 1, sizeof(BYTE), 1, 0, NKey);
	mpz_import(m_D, DSize, 1, sizeof(BYTE), 1, 0, DKey);

	m_NBuff = new BYTE[NSize];
	memcpy(m_NBuff, NKey, NSize);

	m_DBuff = new BYTE[DSize];
	memcpy(m_DBuff, DKey, DSize);

	m_NSize = NSize;
	m_DSize = DSize;
	VM_TIGER_BLACK_END
}

__fastcall CPrivateRsaKey::CPrivateRsaKey(int KeySize, IgcInt NKey, IgcInt DKey)
{
	VM_TIGER_BLACK_START
	m_Size = KeySize;

	size_t count;

	mpz_init(m_N);
	mpz_set(m_N, NKey.value);

	m_NSize = mpz_size(m_N)*sizeof(mp_limb_t);
	m_NBuff = new BYTE[m_NSize];
	mpz_export(m_NBuff, &count, 1, m_NSize, 1, 0, m_N);
	
	mpz_init(m_D);
	mpz_set(m_D, DKey.value);

	m_DSize = mpz_size(m_D)*sizeof(mp_limb_t);
	m_DBuff = new BYTE[m_DSize];
	mpz_export(m_DBuff, &count, 1, m_DSize, 1, 0, m_D);
	VM_TIGER_BLACK_END
}

int __fastcall CPrivateRsaKey::DecBuffer(BYTE* buff, int size, BYTE* outBuff, int& outSize)
{
	VM_TIGER_RED_START
	BYTE* pBuff = (BYTE*)buff;
	if (size < sizeof(RSA_DATA_HEADER))
		return 3;

	PRSA_DATA_HEADER h = (PRSA_DATA_HEADER)buff;

	if(h->size !=  size)
		return 1;							// Dlugosc w naglowku nie moze byc inna niz przyjeta dlugosc danych
	if(h->size < h->old_size)
		return 2;	//fail here						// Dlugosc wiadomosci zaszyfrowanej nie moze byc krotsza niz wiadomosc odszyfrowana

	BYTE* pData = &pBuff[sizeof(RSA_DATA_HEADER)];
	int dataSize = size - sizeof(RSA_DATA_HEADER);

	int encCRC = ComputeCRC(pData, dataSize);
	if (encCRC != h->crc)
		return 4;							// Suma kontrolna w naglowku musi byc rowna sumie kontrolnej wyliczonej na podstawie danych

	int encBlockSize = m_Size;
	if (dataSize % encBlockSize != 0)
		return 5;							// Dlugosc danych musi byc wyrownana do dlugosci klucza

	int MemBlocks = dataSize/encBlockSize;
	/*
	if((MemBlocks-1)*m_Size > h->old_size)
		return 6;							// Odkodowane dane nie mogą byc krotsze niz (ilosc_blokow-1)*dlugosc_klucza
		*/

//	BYTE* decData = new BYTE[h->old_size];
	int DecBlockSize = m_Size-1;
	BYTE decData[10000];
	VM_TIGER_RED_END

	for (int i = 0; i < MemBlocks; i++)
	{
		VM_TIGER_RED_START
		mpz_t enc;
		mpz_init(enc);
		mpz_import(enc, encBlockSize, 1, sizeof(BYTE), 1, 0, &pData[i*encBlockSize]);

		mpz_t dec;
		mpz_init(dec);
		mpz_powm(dec, enc, m_D, m_N);

		size_t count;
		//BYTE* decoded = new BYTE[DecBlockSize];
		//mpz_export(decoded, &count, 1, DecBlockSize, 1, 0, dec);
		//memcpy(&decData[i*DecBlockSize], decoded, DecBlockSize);
		//delete [] decoded;
		mpz_export(&decData[i*DecBlockSize], &count, 1, DecBlockSize, 1, 0, dec);

		mpz_clear(enc);
		mpz_clear(dec);
		VM_TIGER_RED_END
	}

//	delete [] decData;

	memcpy(outBuff, decData, sizeof(decData));
	outSize = h->old_size;
	return 0;
}

CPrivateRsaKey::~CPrivateRsaKey()
{
	//MUTATE_START
	mpz_clear(m_N);
	mpz_clear(m_D);

	delete [] m_DBuff;
	delete [] m_NBuff;
	//VM_END
}

__fastcall CRsa::CRsa(int bits)
{
	VM_TIGER_BLACK_START
	//	this->m_Priv = new CPrivateRsaKey()

	IgcInt p;
	IgcInt q;
	IgcInt n;
	IgcInt Phi;
	IgcInt d;
	IgcInt e;

	mpz_t max;
	mpz_init(max);
	mpz_t min;
	mpz_init(min);

	mpz_t bit;
	mpz_init(bit);

	mpz_set_ui(bit, 2);
	mpz_pow_ui(max, bit, bits);
	mpz_pow_ui(min, bit, bits-4);

	VM_TIGER_BLACK_END

	while (true)
	{
		CODEREPLACE_START
		p.RandPrime(bits/2, g_Rng);
		q.RandPrime(bits/2, g_Rng);

		while (p==q)
		{
			p.RandPrime(bits/2, g_Rng);
			q.RandPrime(bits/2, g_Rng);
		}

		n = p*q;
		if (n > IgcInt(min) && n < IgcInt(max))
			break;
		CODEREPLACE_END
	}
	Phi = (p-IgcInt(1))*(q-IgcInt(1));

	e = 2;
	while (true)
	{
		if (IgcGcd(e, Phi) == 1)
			break;
		e.NextPrime();
	}

	IgcGcdx(p, d, q, e, Phi);

	if (d < 0)
		d+=Phi;

	m_Bits = bits;
	m_Size = bits/8;


	this->m_Pub = new CPublicRsaKey(m_Size, n, e);
	this->m_Priv = new CPrivateRsaKey(m_Size, n, d);
}

__fastcall CRsa::CRsa( int bits, RNG rng )
{
	IgcInt p;
	IgcInt q;
	IgcInt n;
	IgcInt Phi;
	IgcInt d;
	IgcInt e;

	mpz_t max;
	mpz_init(max);
	mpz_t min;
	mpz_init(min);

	mpz_t bit;
	mpz_init(bit);

	mpz_set_ui(bit, 2);
	mpz_pow_ui(max, bit, bits);
	mpz_pow_ui(min, bit, bits-4);

	while (true)
	{
		CODEREPLACE_START
		p.RandPrime(bits/2, rng);
		q.RandPrime(bits/2, rng);

		while (p==q)
		{
			p.RandPrime(bits/2, rng);
			q.RandPrime(bits/2, rng);
		}

		n = p*q;
		if (n > IgcInt(min) && n < IgcInt(max))
			break;
		CODEREPLACE_END
	}

	Phi = (p-IgcInt(1))*(q-IgcInt(1));

	e = 2;
	while (true)
	{
		if(IgcGcd(e, Phi) == 1)
			break;
		e.NextPrime();
	}

	IgcGcdx(p, d, q, e, Phi);

	if (d < 0)
		d+=Phi;

	m_Bits = bits;
	m_Size = bits/8;


	this->m_Pub = new CPublicRsaKey(m_Size, n, e);
	this->m_Priv = new CPrivateRsaKey(m_Size, n, d);

	mpz_clear(max);
	mpz_clear(min);
	mpz_clear(bit);
}

CPublicRsaKey* CRsa::GetPublicRsaKey()
{
	return m_Pub;
}

CPrivateRsaKey* CRsa::GetPrivateRsaKey()
{
	return m_Priv;
}

__fastcall CRsa::~CRsa()
{
	delete m_Priv;
	delete m_Pub;
}
#pragma optimize("gpty", off)