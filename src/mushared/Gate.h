// ------------------------------
// Decompiled by Deathway
// Date : 2007-03-09
// ------------------------------
// gate.h

#ifndef GATE_H
#define GATE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_GATES 1024

class CGate
{
public:

	CGate();
	virtual ~CGate();

	void Init();
	void Load(LPSTR filename);
	int IsGate(int GateNumber);
	int GetGate(int mgt, short& x, short& y, BYTE& MapNumber, BYTE& dir, short& Level);
	int GetLevel(int GateNumber);
	int IsInGate(int aIndex, int GateNumber);
	int CheckGateLevel(int aIndex, int GateNumber);

private:

	WORD m_This[MAX_GATES];	// 4
	BYTE m_Flag[MAX_GATES];	// 104
	BYTE m_MapNumber[MAX_GATES];	// 204
	BYTE m_Sx[MAX_GATES];	// 304
	BYTE m_Sy[MAX_GATES];	// 404
	BYTE m_Ex[MAX_GATES];	// 504
	BYTE m_Ey[MAX_GATES];	// 604
	WORD m_TargetGate[MAX_GATES];	// 704
	BYTE m_Dir[MAX_GATES];	// 804
	WORD m_MinLevel[MAX_GATES];	// 904
};

extern CGate gGateC;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

