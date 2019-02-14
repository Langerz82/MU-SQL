// TMonsterAIAgro.cpp: implementation of the TMonsterAIAgro class.
// GS-N 1.00.18 JPN 0x0055B470	-	Completed
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "GameMain.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TMonsterAIAgro::TMonsterAIAgro()
{
	this->ResetAll();
}

TMonsterAIAgro::~TMonsterAIAgro()
{
	return;
}


void TMonsterAIAgro::ResetAll()
{
	for ( int i=0;i<MAX_MONSTER_AI_AGRO;i++)
	{
		this->m_Agro[i].Reset();
	}
}


BOOL TMonsterAIAgro::SetAgro(CGameObject &ObjTarget, int iValue)
{
	for ( int i=0;i<MAX_MONSTER_AI_AGRO;i++)
	{
		if ( this->m_Agro[i].GetUserIndex() == -1 )
		{
			this->m_Agro[i].SetAgro(iTargetIndex, iValue);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL TMonsterAIAgro::DelAgro(CGameObject &ObjTarget)
{
	for ( int i=0;i<MAX_MONSTER_AI_AGRO;i++)
	{
		if ( this->m_Agro[i].GetUserIndex() == iTargetIndex)
		{
			this->m_Agro[i].Reset();
			return TRUE;
		}
	}

	return FALSE;
}

int TMonsterAIAgro::GetAgro(CGameObject &ObjTarget)
{
	for ( int i=0;i<MAX_MONSTER_AI_AGRO;i++)
	{
		if ( this->m_Agro[i].GetUserIndex() == iTargetIndex )
		{
			return this->m_Agro[i].GetAgroValue();
		}
	}

	return -1;
}


int TMonsterAIAgro::IncAgro(CGameObject &ObjTarget, int iIncValue)
{
	for ( int i=0;i<MAX_MONSTER_AI_AGRO;i++)
	{
		if ( this->m_Agro[i].GetUserIndex() == iTargetIndex )
		{
			return this->m_Agro[i].IncAgro(iIncValue);
		}
	}

	return -1;
}

int TMonsterAIAgro::DecAgro(CGameObject &ObjTarget, int iDecValue)
{
	for ( int i=0;i<MAX_MONSTER_AI_AGRO;i++)
	{
		if ( this->m_Agro[i].GetUserIndex() == iTargetIndex )
		{
			return this->m_Agro[i].DecAgro(iDecValue);
		}
	}

	return -1;
}


void TMonsterAIAgro::DecAllAgro(CGameObject &ObjTarget)
{
	for ( int i=0;i<MAX_MONSTER_AI_AGRO;i++)
	{
		if ( this->m_Agro[i].GetUserIndex() != -1 )
		{
			this->m_Agro[i].DecAgro(iDecValue);
		}
		else
		{
			this->m_Agro[i].Reset();
		}
	}
}



int TMonsterAIAgro::GetMaxAgroUserIndex(int iMonsterIndex)
{
	int iMaxAgroUserIndex = -1;
	int iMaxAgro = -1;

	for ( int i=0;i<MAX_MONSTER_AI_AGRO;i++)
	{
		if ( this->m_Agro[i].GetUserIndex() != -1 )
		{
			CGameObject lpObj = &getGameObject(this->m_Agro[i]->GetUserIndex());

			if ( (lpObj.Authority&2) == 2 )
				continue;

			if ( !gObjIsConnected(lpObj) )
			{
				this->m_Agro[i].Reset();
				continue;
			}

			if ( lpObj.Live == FALSE )
			{
				this->m_Agro[i].Reset();
				continue;
			}

			if ( gObjCalDistance(&getGameObject(iMonsterIndex), lpObj) > 7 )
				continue;

			BYTE btAttr = MapC[lpObj.MapNumber].GetAttr(lpObj.X, lpObj.Y);

			if ( (btAttr&1) != 0 )
				continue;

			if ( this->m_Agro[i].GetAgroValue() > iMaxAgro )
			{
				iMaxAgroUserIndex =	this->m_Agro[i].GetUserIndex();
				iMaxAgro = this->m_Agro[i].GetAgroValue();
			}
		}
	}

	return iMaxAgroUserIndex;
}




////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

