#include "StdAfx.h"
#include "RNG.h"
#include "IgcInt.h"

#pragma optimize("g", on)
RNG g_Rng;

RNG::RNG()
{

}

void RNG::SetSeed(int Seed)
{
	gmp_randinit_default(m_State);
	gmp_randseed_ui(m_State, Seed);
}

void RNG::SetSeed(mpz_t Seed)
{
	gmp_randinit_default(m_State);
	gmp_randseed(m_State, Seed);
}

void RNG::SetSeed(IgcInt & Seed)
{
	gmp_randseed(m_State, Seed.value);
}

void RNG::Clear()
{
	gmp_randclear(m_State);
}
#pragma optimize("g", off)