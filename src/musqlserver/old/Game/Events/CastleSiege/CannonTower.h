// CannonTower.h: interface for the CCannonTower class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CANNONTOWER_H__DD8C3ABB_D613_4E50_B4F1_0127230C40BC__INCLUDED_)
#define AFX_CANNONTOWER_H__DD8C3ABB_D613_4E50_B4F1_0127230C40BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCannonTower  
{
public:
	CCannonTower();
	virtual ~CCannonTower();
	void CannonTowerAct(LPGameObject &lpObj);
};

extern CCannonTower g_CsNPC_CannonTower;

#endif // !defined(AFX_CANNONTOWER_H__DD8C3ABB_D613_4E50_B4F1_0127230C40BC__INCLUDED_)


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

