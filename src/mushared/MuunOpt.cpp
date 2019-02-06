////////////////////////////////////////////////////////////////////////////////
// MuunOpt.cpp
#include "StdAfx.h"
#include "MuunOpt.h"

CMuunOpt::CMuunOpt()
{

}

CMuunOpt::~CMuunOpt()
{

}

void CMuunOpt::SetOptType(int iOptType)
{
	this->m_iOptType = iOptType;
}

int CMuunOpt::GetOptType()
{
	return this->m_iOptType;
}

void CMuunOpt::SetMuunLvVal(int *iMuunLvVal)
{
	memcpy(this->m_iMuunLvVal, iMuunLvVal, sizeof(m_iMuunLvVal));
}

void CMuunOpt::GetMuunLvVal(int *iOutMuunLvVal)
{
	memcpy(iOutMuunLvVal, this->m_iMuunLvVal, sizeof(this->m_iMuunLvVal));
}

int CMuunOpt::GetMuunLvVal(int iMuunLv)
{
	if (iMuunLv < 0 && iMuunLv > 5)
	{
		return 0;
	}

	return this->m_iMuunLvVal[iMuunLv];
}

void CMuunOpt::SetConditionType(int iConditionType)
{
	this->m_iConditionType = iConditionType;
}

int CMuunOpt::GetConditionType()
{
	return this->m_iConditionType;
}

void CMuunOpt::SetConditionVal1(int iConditionVal1)
{
	this->m_iConditionVal1 = iConditionVal1;
}

int CMuunOpt::GetConditionVal1()
{
	return this->m_iConditionVal1;
}

void CMuunOpt::SetConditionVal2(int iConditionVal2)
{
	this->m_iConditionVal2 = iConditionVal2;
}

int CMuunOpt::GetConditionVal2()
{
	return this->m_iConditionVal2;
}

void CMuunOpt::Clear()
{
	this->m_iOptType = 0;

	for (int i = 0; i < 6; i++)
		this->m_iMuunLvVal[i] = 0;

	this->m_iConditionType = 0;
	this->m_iConditionVal1 = 0;
	this->m_iConditionVal2 = 0;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

