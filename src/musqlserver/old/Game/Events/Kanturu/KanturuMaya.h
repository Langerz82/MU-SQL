// KanturuMaya.h: interface for the CKanturuMaya class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KANTURUMAYA_H__8768002F_4B22_480A_8F6A_10AE05568C27__INCLUDED_)
#define AFX_KANTURUMAYA_H__8768002F_4B22_480A_8F6A_10AE05568C27__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CKanturuMaya  
{

public:

	CKanturuMaya();
	virtual ~CKanturuMaya();

	void Init();
	void KanturuMayaAct_IceStorm(int iSkillUsingRate);
	void KanturuMayaAct_Hands();
	void SetMayaObjIndex(CGameObject &Obj){this->m_iMayaObjIndex = Obj.m_Index;}
	//int __thiscall GetMayaObjIndex();

private:

	int m_iMayaObjIndex;	// 4
	BOOL m_bMonsterKillCount;	// 8
	int m_iMayaSkillTime;	// C
	int m_iIceStormCount;	// 10

};

#endif // !defined(AFX_KANTURUMAYA_H__8768002F_4B22_480A_8F6A_10AE05568C27__INCLUDED_)


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

