#ifndef _IGCINT_H
#define _IGCINT_H

#include "RNG.h"

/************************************************************************************/
/*	Class IgcInt type
*/

typedef class IgcInt{
public:
	mpz_t value;

	//	Constructors
	~IgcInt();
	IgcInt();
	IgcInt(IgcInt& pSet);
	IgcInt(mpz_t val);
	IgcInt(long val);
	//	IgcInt(unsigned long val);
	IgcInt(char* str);
	IgcInt(char* str, int base);

	// Set Operators
	void operator=(IgcInt & pSet);
	void operator=(mpz_t pSet);
	void operator=(char* pSet);
	void operator=(long pSet);

	// Compare Operators
	bool operator==(IgcInt & pInt);
	bool operator>=(IgcInt & pInt);
	bool operator<=(IgcInt & pInt);
	bool operator>(IgcInt & pInt);
	bool operator<(IgcInt & pInt);

	bool operator==(long pInt);
	bool operator>=(long pInt);
	bool operator<=(long pInt);
	bool operator>(long pInt);
	bool operator<(long pInt);


	// Arithmetic operators
	IgcInt operator+(IgcInt & pAdd);
	IgcInt operator-(IgcInt & pSub);
	IgcInt operator/(IgcInt & pDiv);
	IgcInt operator*(IgcInt & pMul);
	IgcInt operator%(IgcInt & pMod);

	void operator+=(IgcInt & pInc);
	void operator-=(IgcInt & pDec);
	void operator*=(IgcInt & pMul);
	void operator/=(IgcInt & pDiv);
	void operator%=(IgcInt & pMod);

	void operator++();
	void operator--();

	unsigned int Get();
	unsigned int GetSize();

	/**************************************************************************************/
	/* Some functions
	*/
	// Rand / primes
	void Randomize(RNG rng);
	void Random(int bits, RNG rng);
	void Random(IgcInt bits, RNG rng);
	void Random(int min, int max, RNG rng);
	void Random(IgcInt min, IgcInt max, RNG rng);

	void NextPrime();
	void RandPrime(int bits, RNG rng);

	IgcInt Abs();
	IgcInt Pow(long exp);
	IgcInt PowMod(IgcInt & pExp, IgcInt & pMod);
} *PIgcInt, *pIgcInt, *LPIgcInt, *lpIgcInt, tIgcInt;

#endif