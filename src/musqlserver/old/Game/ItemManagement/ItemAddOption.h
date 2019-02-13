// ItemAddOption.h: interface for the CItemObjectAddOption class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMADDOPTION_H__96FEC421_39C3_40DD_9D47_EE1D86DD8A21__INCLUDED_)
#define AFX_ITEMADDOPTION_H__96FEC421_39C3_40DD_9D47_EE1D86DD8A21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "generalStructs.h"

struct ITEMEFFECT;

class CItemObjectAddOption  
{
public:

	CItemObjectAddOption();
	virtual ~CItemObjectAddOption();

	void Load(PCHAR chFileName);
	void Initialize();

	bool IsValidEffect(int iItemNumber);
	LPITEMEFFECT GetItemEffectData(int iItemNumber);

private:

	LPITEMEFFECT SearchItemEffect(int iItemNumber);

	ITEMEFFECT m_ItemAddOption[100];
};

extern CItemObjectAddOption	g_ItemAddOption;

#endif // !defined(AFX_ITEMADDOPTION_H__96FEC421_39C3_40DD_9D47_EE1D86DD8A21__INCLUDED_)
