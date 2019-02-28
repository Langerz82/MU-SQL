// TMonsterAIAgro.h: interface for the TMonsterAIAgro class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TMONSTERAIAGRO_H__691DFAE8_C8F5_4A75_B0D9_9BA0DCC1BCD4__INCLUDED_)
#define AFX_TMONSTERAIAGRO_H__691DFAE8_C8F5_4A75_B0D9_9BA0DCC1BCD4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_MONSTER_AI_AGRO	100

class TMonsterAIAgroInfo
{

public:

	TMonsterAIAgroInfo()
	{
		// Clean Data
		this->Reset();
	}

	void Reset()
	{
		this->m_iUserIndex = -1;
		this->m_iAgroValue = -1;
	}

	int GetUserIndex()
	{
		return this->m_iUserIndex;
	}

	int GetAgroValue()
	{
		return this->m_iAgroValue;
	}

	void SetAgro(int iTargetIndex, int iValue)
	{
		this->m_iUserIndex = iTargetIndex;
		this->m_iAgroValue = iValue;
	}

	int IncAgro(int iIncValue)
	{
		this->m_iAgroValue += iIncValue;
		return this->m_iAgroValue;
	}

	int DecAgro(int iDecValue)
	{
		this->m_iAgroValue -= iDecValue;

		if ( this->m_iAgroValue < 0 )
			this->m_iAgroValue = 0;

		return this->m_iAgroValue;
	}

private:

	int m_iUserIndex;	// 0
	int m_iAgroValue;	// 4

};

//#pragma auto_inline(on)

class TMonsterAIAgro
{

public:
	
	TMonsterAIAgro();
	virtual ~TMonsterAIAgro();

	void  ResetAll();
	BOOL  SetAgro(CGameObject &ObjTarget, int iValue);
	BOOL  DelAgro(CGameObject &ObjTarget);
	int  GetAgro(CGameObject &ObjTarget);
	int  IncAgro(CGameObject &ObjTarget, int iIncValue);
	int  DecAgro(CGameObject &ObjTarget, int iDecValue);
	void  DecAllAgro(int iDecValue);
	int  GetMaxAgroUserIndex(int iMonsterIndex);

private:

	TMonsterAIAgroInfo m_Agro[MAX_MONSTER_AI_AGRO];	// 4

};


#endif

