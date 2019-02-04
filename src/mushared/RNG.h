#ifndef _RNG_H
#define _RNG_H

class IgcInt;

typedef class RNG{
public:
	gmp_randstate_t m_State;

	RNG();

	void SetSeed(int  Seed);
	void SetSeed(mpz_t Seed);
	void SetSeed(IgcInt & Seed);

	void Clear();

} *PRNG, *LPRNG;

extern RNG g_Rng;

#endif