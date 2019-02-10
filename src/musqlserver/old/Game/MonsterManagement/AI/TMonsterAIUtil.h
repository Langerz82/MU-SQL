// TMonsterAIUtil.h: interface for the TMonsterAIUtil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TMONSTERAIUTIL_H__D2E082C2_05F0_49E5_9177_3592AAF616E7__INCLUDED_)
#define AFX_TMONSTERAIUTIL_H__D2E082C2_05F0_49E5_9177_3592AAF616E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "User/CUserData.h"

class TMonsterAIUtil  
{

public:

	TMonsterAIUtil();
	virtual ~TMonsterAIUtil();

	BOOL FindPathToMoveMonster(LPGameObject &lpObj, int iTargetX, int iTargetY, int iMaxPathCount, BOOL bPreventOverMoving);
	BOOL SendMonsterMoveMsg(LPGameObject &lpObj);
	BOOL CheckMovingCondition(LPGameObject &lpObj);
	void SendMonsterV2Msg(LPGameObject &lpObj, unsigned char* lpMsg, int size);
	BOOL CheckMoveRange(LPGameObject &lpObj, int iTargetX, int iTargetY);
	BOOL GetXYToPatrol(LPGameObject &lpObj);
	BOOL GetXYToEascape(LPGameObject &lpObj);
	BOOL GetXYToChase(LPGameObject &lpObj);
	BOOL FindMonViewportObj(int iObjIndex, int iTargetObjIndex);
	BOOL FindMonViewportObj2(int iObjIndex, int iTargetObjIndex);
	void __cdecl SendChattingMsg(int iObjIndex, char* lpszMsg, ...);

};

#endif // !defined(AFX_TMONSTERAIUTIL_H__D2E082C2_05F0_49E5_9177_3592AAF616E7__INCLUDED_)


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

