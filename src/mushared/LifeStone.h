// LifeStone.h: interface for the CLifeStone class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIFESTONE_H__9E5B6740_42C2_41D9_AB3E_74F5FB0DE015__INCLUDED_)
#define AFX_LIFESTONE_H__9E5B6740_42C2_41D9_AB3E_74F5FB0DE015__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLifeStone  
{
public:
	CLifeStone();
	virtual ~CLifeStone();

	int CreateLifeStone(int iIndex);
	int DeleteLifeStone(int iIndex);
	int SetReSpawnUserXY(int iUserIndex);
	void LifeStoneAct(int iIndex);
};

extern CLifeStone g_CsNPC_LifeStone;

#endif // !defined(AFX_LIFESTONE_H__9E5B6740_42C2_41D9_AB3E_74F5FB0DE015__INCLUDED_)


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

