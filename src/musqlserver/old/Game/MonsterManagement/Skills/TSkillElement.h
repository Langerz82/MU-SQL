// TSkillElement.h: interface for the TMonsterSkillElementInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TSKILLELEMENT_H__2344436E_2B9F_409A_B2AA_70024491BEE5__INCLUDED_)
#define AFX_TSKILLELEMENT_H__2344436E_2B9F_409A_B2AA_70024491BEE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "CGameObject.h"

class CGameObject;

class TSkillElement
{

public:

	//TSkillElement();
	virtual void ForceSkillElement(CGameObject &Obj, int iTargetIndex){return;}	// 0
};


#endif // !defined(AFX_TSKILLELEMENT_H__2344436E_2B9F_409A_B2AA_70024491BEE5__INCLUDED_)


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

