#include "StdAfx.h"
#include "IgcInt.h"

/****************************************************************************/
/* IgcInt constructors
*/
#pragma optimize("g", on)

IgcInt::~IgcInt()
{
	mpz_clear(value);
}

IgcInt::IgcInt()
{
	mpz_init(value);
}

IgcInt::IgcInt(IgcInt& pSet)
{
	mpz_init(value);
	mpz_set(value, pSet.value);
}

IgcInt::IgcInt(mpz_t val)
{
	mpz_init(value);
	mpz_set(value, val);
}

IgcInt::IgcInt(long val)
{
	mpz_init(value);
	if(val >= 0)
		mpz_set_ui(value, val);
	else
		mpz_set_si(value, val);
}

/*IgcInt::IgcInt(unsigned long val)
{
	mpz_init(value);
	mpz_set_ui(value, val);
}
*/
IgcInt::IgcInt(char* str)
{
	mpz_init(value);
	mpz_set_str(value, str, 10);
}

IgcInt::IgcInt(char* str, int base)
{
	mpz_init(value);
	mpz_set_str(value, str, base);
}

/*************************************************************************************/
/* IgcInt Operators
*/

void IgcInt::operator=(IgcInt &pSet)
{
//	mpz_t temp;
//	mpz_init(temp);

	mpz_set(value, pSet.value);
}

void IgcInt::operator=(mpz_t pSet)
{
	mpz_set(value, pSet);
}

void IgcInt::operator=(char* pSet)
{
	mpz_set_str(value, pSet, 10);
}

void IgcInt::operator=(long pSet)
{
	if(pSet >= 0)
		mpz_set_ui(value, pSet);
	else
		mpz_set_si(value, pSet);
}

// Compare operators
////////////////////////////////////////////////////////
bool IgcInt::operator==(IgcInt & pInt)
{
	if(!mpz_cmp(this->value, pInt.value))
		return true;
	else
		return false;
}

bool IgcInt::operator<=(IgcInt &pInt)
{
	if(mpz_cmp(this->value, pInt.value) <= 0)
		return true;
	else
		return false;
}

bool IgcInt::operator>=(IgcInt & pInt)
{
	if(mpz_cmp(this->value, pInt.value) >= 0)
		return true;
	else
		return false;
}

bool IgcInt::operator<(IgcInt & pInt)
{
	if(mpz_cmp(this->value, pInt.value) < 0)
		return true;
	else
		return false;
}

bool IgcInt::operator>(IgcInt & pInt)
{
	if(mpz_cmp(this->value, pInt.value) > 0)
		return true;
	else
		return false;
}

///////////////////////////////////

bool IgcInt::operator==(long pInt)
{
	if(!mpz_cmp_ui(this->value, pInt))
		return true;
	else
		return false;
}

bool IgcInt::operator<=(long pInt)
{
	if(mpz_cmp_ui(this->value, pInt) <= 0)
		return true;
	else
		return false;
}

bool IgcInt::operator>=(long pInt)
{
	if(mpz_cmp_ui(this->value, pInt) >= 0)
		return true;
	else
		return false;
}

bool IgcInt::operator<(long pInt)
{
	if(mpz_cmp_ui(this->value, pInt) < 0)
		return true;
	else
		return false;
}

bool IgcInt::operator>(long pInt)
{
	if(mpz_cmp_ui(this->value, pInt) > 0)
		return true;
	else
		return false;
}

// Arithmetic operators
////////////////////////////////////////////////////////////
IgcInt IgcInt::operator+(IgcInt & pAdd)
{
	mpz_t temp;
	mpz_init(temp);

	mpz_add(temp, this->value, pAdd.value);

	IgcInt res = temp;

	mpz_clear(temp);
	return res;
}

IgcInt IgcInt::operator-(IgcInt & pSub)
{
	mpz_t temp;
	mpz_init(temp);

	mpz_sub(temp, this->value, pSub.value);

	IgcInt res = temp;

	mpz_clear(temp);
	return res;
}

IgcInt IgcInt::operator/(IgcInt & pDiv)
{
	mpz_t temp;
	mpz_init(temp);

	mpz_div(temp, this->value, pDiv.value);

	IgcInt res = temp;

	mpz_clear(temp);
	return res;
}

IgcInt IgcInt::operator*(IgcInt & pMul)
{
	mpz_t temp;
	mpz_init(temp);

	mpz_mul(temp, this->value, pMul.value);

	IgcInt res = temp;

	mpz_clear(temp);
	return res;
}

IgcInt IgcInt::operator%(IgcInt & pMod)
{
	IgcInt temp;
	mpz_mod(temp.value, this->value, pMod.value);
	return temp;
}
////////////////////////////////////////////
void IgcInt::operator+=(IgcInt & pInc)
{
	mpz_t temp;
	mpz_init(temp);

	mpz_add(temp, this->value, pInc.value);
	mpz_set(value, temp);
	mpz_clear(temp);
}

void IgcInt::operator-=(IgcInt & pDec)
{
	mpz_t temp;
	mpz_init(temp);

	mpz_sub(temp, this->value, pDec.value);
	mpz_set(value, temp);
	mpz_clear(temp);
}

void IgcInt::operator*=(IgcInt & pMul)
{
	mpz_t temp;
	mpz_init(temp);

	mpz_mul(temp, this->value, pMul.value);
	mpz_set(value, temp);
	mpz_clear(temp);
}

void IgcInt::operator/=(IgcInt & pDiv)
{
	mpz_t temp;
	mpz_init(temp);

	mpz_div(temp, this->value, pDiv.value);
	mpz_set(value, temp);
	mpz_clear(temp);
}

void IgcInt::operator%=(IgcInt & pMod)
{
	IgcInt temp;
	mpz_mod(temp.value, value, pMod.value);
	*this = temp;
}
////////////////////////////////////////////
void IgcInt::operator++()
{
	mpz_add_ui(value, value, 1);
}

void IgcInt::operator--()
{
	mpz_sub_ui(value, value, 1);
}

unsigned int IgcInt::Get()
{
	return mpz_get_ui(value);
}

unsigned int IgcInt::GetSize()
{
	return mpz_size(value);
}


/****************************************************************************************************************************
	Functions
	*/

void IgcInt::Randomize(RNG rng)
{

}

void IgcInt::Random(int bits, RNG rng)
{
	mpz_t rand;
	mpz_init(rand);
	mpz_t max;
	mpz_init(max);
	mpz_t min;
	mpz_init(min);

//	mpz_ui_pow_ui(max, 10, bits+1);
//	mpz_ui_pow_ui(min, 10, bits);

	mpz_t bit;
	mpz_init(bit);
	
	mpz_set_ui(bit, 2);
	mpz_pow_ui(max, bit, bits+1);
	mpz_pow_ui(min, bit, bits);

//	mpz_ui_pow_ui(max, bits+1, 10);
//	mpz_ui_pow_ui(min, bits, 10);


	do{
		mpz_urandomm(rand, rng.m_State, max);
	} while(mpz_cmp(rand, min) > 0);

	mpz_set(value, rand);

	mpz_clear(rand);
	mpz_clear(max);
	mpz_clear(min);
	mpz_clear(bit);
}

void IgcInt::NextPrime()
{
	mpz_nextprime(value, value);
}

void IgcInt::RandPrime(int bits, RNG rng)
{
	Random(bits, rng);
	NextPrime();
}

IgcInt IgcInt::Abs()
{
	IgcInt ret;
	mpz_abs(ret.value, value);
	return ret;
}

IgcInt IgcInt::Pow(long exp)
{
	IgcInt ret;
	mpz_pow_ui(ret.value, value, exp);
	return ret;
}

IgcInt IgcInt::PowMod(IgcInt & pExp, IgcInt & pMod)
{
	IgcInt temp;
	mpz_powm(temp.value, value, pExp.value, pMod.value);
	return temp;
}
#pragma optimize("g", off)