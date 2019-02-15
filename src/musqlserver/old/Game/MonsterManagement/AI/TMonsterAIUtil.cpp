// TMonsterAIUtil.cpp: implementation of the TMonsterAIUtil class.
//	GS-N	1.00.18	JPN	0x00564930	-	Completed
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "TMonsterAIUtil.h"
#include "BuffEffectSlot.h"
#include "Logging/Log.h"
#include "Gamemain.h"
#include "util.h"
#include "configread.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TMonsterAIUtil::TMonsterAIUtil()
{
	return;
}

TMonsterAIUtil::~TMonsterAIUtil()
{
	return;
}

BOOL TMonsterAIUtil::FindMonViewportObj(int iObjIndex, int iTargetObjIndex)
{
	if ( OBJMON_RANGE(iObjIndex) ==  FALSE || ObjectMaxRange(iTargetObjIndex) == FALSE )
		return FALSE;

	for ( int i=0;i<MaxViewportMonster;i++)
	{
		if ( getGameObject(iObjIndex)->VpPlayer[i)->state != FALSE )
		{
			if ( iTargetObjIndex == getGameObject(iObjIndex)->VpPlayer[i)->number )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL TMonsterAIUtil::FindMonViewportObj2(int iObjIndex, int iTargetObjIndex)
{
	if ( OBJMON_RANGE(iObjIndex) ==  FALSE || ObjectMaxRange(iTargetObjIndex) == FALSE )
		return FALSE;

	for ( int i=0;i<MaxViewportMonster;i++)
	{
		if ( getGameObject(iObjIndex)->VpPlayer2[i)->state != FALSE )
		{
			if ( iTargetObjIndex == getGameObject(iObjIndex)->VpPlayer2[i)->number )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}



void TMonsterAIUtil::SendMonsterV2Msg(CGameObject &Obj, BYTE* lpMsg, int size)
{
	for ( int i=0;i<MaxViewportMonster;i++)
	{
		if ( lpObj.VpPlayer2[i].type == OBJ_USER )
		{
			if ( lpObj.VpPlayer2[i].state != FALSE )
			{
				if ( lpObj.Connected > PLAYER_CONNECTED && lpObj.Live != FALSE)
				{
					IOCP.DataSend(lpObj.VpPlayer2[i].number, lpMsg, size);
				}
				else
				{
					lpObj.VpPlayer2[i].number = -1;
					lpObj.VpPlayer2[i].state = FALSE;
					lpObj.VPCount2--;
				}
			}
		}
	}
}


BOOL TMonsterAIUtil::FindPathToMoveMonster(CGameObject &Obj, int iTargetX, int iTargetY, int iMaxPathCount, BOOL bPreventOverMoving)
{
	if ( TMonsterAIUtil::CheckMovingCondition(lpObj)==FALSE)
		return FALSE;

	if ( MAX_MAP_RANGE(lpObj.MapNumber)==FALSE)
		return FALSE;

	PATH_t Path;
	BOOL bPathFound = FALSE;

	if ( bPreventOverMoving == 1 )
		bPathFound = MapC[lpObj.MapNumber].PathFinding2(lpObj.X, lpObj.Y, iTargetX, iTargetY, &Path);
	else
		bPathFound = MapC[lpObj.MapNumber].PathFinding4(lpObj.X, lpObj.Y, iTargetX, iTargetY, &Path);

	if (bPathFound )
	{
		lpObj.m_LastMoveTime = GetTickCount();

		int iTargetX;
		int iTargetY;
		int iStartX;
		int iStartY;
		int iResultX;
		int iResultY;
		BYTE btTargetDir = 0;

		iStartX = lpObj.X;
		iStartY = lpObj.Y;
		iResultX = lpObj.X;
		iResultY = lpObj.Y;
		lpObj.PathCount = Path.PathNum;
		lpObj.PathCur = 1;
		lpObj.PathStartEnd = 1;

		if ( lpObj.PathCount > iMaxPathCount )
			lpObj.PathCount = iMaxPathCount;

		lpObj.PathX[0] = lpObj.X;
		lpObj.PathY[0] = lpObj.Y;
		lpObj.PathDir[0] = lpObj.Dir;

		for(int n=1;n<lpObj.PathCount;n++)
		{
			iTargetX = Path.PathX[n];
			iTargetY = Path.PathY[n];
			btTargetDir = GetPathPacketDirPos(iTargetX-iStartX, iTargetY-iStartY);
			lpObj.PathX[n] = iTargetX;
			lpObj.PathY[n] = iTargetY;
			lpObj.PathDir[n] = btTargetDir;
			iResultX += RoadPathTable[btTargetDir*2];
			iResultY += RoadPathTable[btTargetDir*2+1];
		}

		lpObj.MTX = iResultX;
		lpObj.MTY = iResultY;
		lpObj.TX = iResultX;
		lpObj.TY = iResultY;

		if ( lpObj.PathCount > 0 )
		{
			MapC[lpObj.MapNumber].ClearStandAttr(iStartX, iStartY);
			MapC[lpObj.MapNumber].SetStandAttr(iResultX, iResultY);
			lpObj.m_OldX = iStartX;
			lpObj.m_OldY = iStartY;
		}
	}

	if ( bPathFound )
	{
		this->SendMonsterMoveMsg(lpObj);
		return TRUE;
	}

	return FALSE;
}



BOOL TMonsterAIUtil::CheckMovingCondition(CGameObject &Obj)
{
	if ( !lpObj.Live )
		return FALSE;

	if ( lpObj.RegenOk > 0 )
		return FALSE;

	if ( lpObj.Teleport )
		return FALSE;

	if ( lpObj.m_State != 2 )
		return FALSE;

	if (gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_STONE) || gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_STUN) || gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_SLEEP)
		|| gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_FREEZE_2) || gObjCheckUsedBuffEffect(lpObj, BUFFTYPE_EARTH_BINDS))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL TMonsterAIUtil::CheckMoveRange(CGameObject &Obj, int iTargetX, int iTargetY)
{
	return FALSE;
}


BOOL TMonsterAIUtil::GetXYToPatrol(CGameObject &Obj)
{
	int maxmoverange = lpObj.m_MoveRange*2+1;
	int searchc=10;
	lpObj.NextActionTime = 1000;
	BYTE tpx;
	BYTE tpy;

	while ( searchc-- != 0 )
	{
		__try
		{
			tpx = (lpObj.X - lpObj.m_MoveRange) + (rand()%maxmoverange);
			tpy = (lpObj.Y - lpObj.m_MoveRange) + (rand()%maxmoverange);
		}
		__except(maxmoverange=1, 1)
		{

		}

		BYTE attr = MapC[lpObj.MapNumber].GetAttr(tpx, tpy);

		if ( (attr&1)!=1 && (attr&2)!=2 && (attr&4)!=4 && (attr&8)!= 8 )
		{
			lpObj.MTX = tpx;
			lpObj.MTY = tpy;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL TMonsterAIUtil::GetXYToEascape(CGameObject &Obj)
{
	int tpx;	// Target Player X
	int tpy;
	int mtx;	// Monster Target X
	int mty;
	int tx;
	int ty;
	int searchp = 0;
	int sn = 0;
	int searchcount = MAX_ROAD_PATH_TABLE/2-1;
	BYTE attr;
	BOOL result = 0;
	CGameObject lpTargetObj;

	int iTargetIndex = lpObj.TargetNumber;

	if ( ObjectMaxRange(iTargetIndex) == FALSE )
	{
		return FALSE;
	}

	lpTargetObj = &getGameObject(iTargetIndex);
	tpx = lpTargetObj.X;
	mtx = tpx;
	tpy = lpTargetObj.Y;
	mty = tpy;
	tx = lpObj.X;
	ty = lpObj.Y;
	int dis = lpObj.m_AttackRange / sqrt(2.0);

	if ( lpObj.X < mtx )
	{
		tx -= dis;
	}

	if ( lpObj.X > mtx )
	{
		tx += dis;
	}

	if ( lpObj.Y < mty )
	{
		ty -= dis;
	}

	if ( lpObj.Y > mty )
	{
		ty += dis;
	}

	searchp = GetPathPacketDirPos( (lpObj.X - tx), (lpObj.Y - ty) ) * 2;

	if ( MapC[lpObj.MapNumber].GetStandAttr(tx, ty) == 0 )
	{
		while ( searchcount-- )
		{
			mtx = lpObj.X + RoadPathTable[searchp];
			mty = lpObj.Y + RoadPathTable[1+searchp];
			attr = MapC[lpObj.MapNumber].GetAttr(mtx, mty);
			
			if ( (attr&1) != 1 && (attr&2) != 2 && (attr&4) != 4 && (attr&8) != 8  )
			{
				lpObj.MTX = mtx;
				lpObj.MTY = mty;
				return TRUE;
			}

			searchp += 2;

			if ( searchp > MAX_ROAD_PATH_TABLE-1 )
			{
				searchp = 0;
			}
		}

		return FALSE;
	}

	attr = MapC[lpObj.MapNumber].GetAttr(tx, ty);

	if ( (attr&1) != 1 && (attr&2) != 2 && (attr&4) != 4 && (attr&8) != 8  )
	{
		lpObj.MTX = tx;
		lpObj.MTY = ty;
		return TRUE;
	}

	return FALSE;
}



BOOL TMonsterAIUtil::GetXYToChase(CGameObject &Obj)
{
	int tpx;	// Target Player X
	int tpy;
	int mtx;	// Monster Target X
	int mty;
	int searchp = 0;
	int sn = 0;
	int searchcount = MAX_ROAD_PATH_TABLE/2-1;
	BYTE attr;
	BOOL result = 0;
	CGameObject lpTargetObj;

	int iTargetIndex = lpObj.TargetNumber;

	if ( ObjectMaxRange(iTargetIndex) == FALSE )
	{
		return FALSE;
	}

	lpTargetObj = &getGameObject(iTargetIndex);
	tpx = lpTargetObj.X;
	mtx = tpx;
	tpy = lpTargetObj.Y;
	mty = tpy;
	int dis = lpObj.m_AttackRange / sqrt(2.0);

	if ( lpObj.X < mtx )
	{
		tpx -= dis;
	}

	if ( lpObj.X > mtx )
	{
		tpx += dis;
	}

	if ( lpObj.Y < mty )
	{
		tpy -= dis;
	}

	if ( lpObj.Y > mty )
	{
		tpy += dis;
	}

	searchp = GetPathPacketDirPos( (lpTargetObj.X - tpx), (lpTargetObj.Y - tpy) ) * 2;

	if ( MapC[lpObj.MapNumber].GetStandAttr(tpx, tpy) == 0 )
	{
		while ( searchcount-- )
		{
			mtx = lpTargetObj.X + RoadPathTable[searchp];
			mty = lpTargetObj.Y + RoadPathTable[1+searchp];
			attr = MapC[lpObj.MapNumber].GetAttr(mtx, mty);
			
			if ( (attr&1) != 1 && (attr&2) != 2 && (attr&4) != 4 && (attr&8) != 8  )
			{
				lpObj.MTX = mtx;
				lpObj.MTY = mty;
				return TRUE;
			}

			searchp += 2;

			if ( searchp > MAX_ROAD_PATH_TABLE-1 )
			{
				searchp = 0;
			}
		}

		return FALSE;
	}

	attr = MapC[lpObj.MapNumber].GetAttr(tpx, tpy);

	if ( (attr&1) != 1 && (attr&2) != 2 && (attr&4) != 4 && (attr&8) != 8  )
	{
		lpObj.MTX = tpx;
		lpObj.MTY = tpy;
		return TRUE;
	}

	return FALSE;
}



BOOL TMonsterAIUtil::SendMonsterMoveMsg(CGameObject &Obj)
{
	PMSG_RECVMOVE pMove;

#if(ENABLE_SEASON12 == 1)
	PHeadSetB((BYTE*)&pMove, HEADCODE_MOVE, sizeof(pMove));
#else
	PHeadSetB((BYTE*)&pMove, 0xD4, sizeof(pMove));
#endif

	pMove.NumberH = SET_NUMBERH(lpObj.m_Index);
	pMove.NumberL = SET_NUMBERL(lpObj.m_Index);
	pMove.X = lpObj.MTX;
	pMove.Y = lpObj.MTY;
	pMove.Path = lpObj.Dir << 4;

	if ( lpObj.Class == 541 )
	{
		sLog->outBasic("CHUJ!");
		return FALSE;
	}

	this->SendMonsterV2Msg(lpObj, (BYTE*)&pMove, pMove.h.size);
	return TRUE;
}


void TMonsterAIUtil::SendChattingMsg(int iObjIndex, char* lpszMsg, ...)
{
	if ( !lpszMsg )
		return;

	if ( !ObjectMaxRange(iObjIndex))
		return;

	CGameObject lpObj = &getGameObject(iObjIndex);
	char szBuffer[512] = "";
	va_list pArguments;

	va_start(pArguments, lpszMsg);
	vsprintf(szBuffer, lpszMsg, pArguments);
	va_end(pArguments);

	char szChat[60] = {0};
	memcpy(szChat, szBuffer, sizeof(szChat)-1);

	for(int i=0;i<MaxViewportMonster;i++)
	{
		if ( lpObj.VpPlayer2[i].state )
		{
			int tObjNum = lpObj.VpPlayer2[i].number;

			if ( ObjectMaxRange(tObjNum) )
			{
				gGameProtocol.ChatTargetSend(lpObj, szChat, tObjNum);
			}
		}
	}
}



////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

