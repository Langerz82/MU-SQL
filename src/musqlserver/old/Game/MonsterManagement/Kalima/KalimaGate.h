////////////////////////////////////////////////////////////////////////////////
// KalimaGate.h
// ------------------------------
#ifndef KALIMAGATE_H
#define KALIMAGATE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"

static int g_iKalimaGateGateIndex[KALIMA_FLOORS] =
{
	0x58, 0x59,
	0x5A, 0x5B,
	0x5C, 0x5D,
	0x74
};


class CKalimaGate
{

private:

	CRITICAL_SECTION m_critKalimaGate;	// 4

public:

	CKalimaGate();
	virtual ~CKalimaGate();

	int CreateKalimaGate(CGameObject &Obj, BYTE btLevel, BYTE cTX,  BYTE cTY);
	void KalimaGateAct(CGameObject &Obj);
	int CheckOverlapKundunMark(CGameObject &Obj, BYTE btLevel);
	int DeleteKalimaGate(int iCallOwnerIndex);
	int DeleteKalimaGate(int iKalimaGateIndex, int iCallOwnerIndex);
	int CreateKalimaGate2(CGameObject &Obj, int iMonMapNumber, BYTE cTX, BYTE cTY);
	void KalimaGateAct2(CGameObject &Obj);
	int GetKalimaGateLevel2(CGameObject &Obj);

private:

	int GetRandomLocation(int iMapNumber, BYTE& cX, BYTE& cY);
	int GetKalimaGateLevel(CGameObject &Obj);
};

extern CKalimaGate g_KalimaGate;

#endif

