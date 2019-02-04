#include "StdAfx.h"
#include "IgcMath.h"

#pragma optimize("g", on)
IgcInt IgcGcd(IgcInt & Int1, IgcInt & Int2)
{
	IgcInt ret;
	mpz_gcd(ret.value, Int1.value, Int2.value);
	return ret;
}

void IgcGcdx(IgcInt &p1, IgcInt &p2, IgcInt &p3, IgcInt &p4, IgcInt &p5)
{
	mpz_gcdext(p1.value, p2.value, p3.value, p4.value, p5.value);
}
#pragma optimize("g", off)