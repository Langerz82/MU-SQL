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
		if ( Obj.VpPlayer2[i].type == OBJ_USER )
		{
			if ( Obj.VpPlayer2[i].state != FALSE )
			{
				if ( Obj.Connected > PLAYER_CONNECTED && Obj.Live != FALSE)
				{
					IOCP.DataSend(Obj.VpPlayer2[i].number, lpMsg, size);
				}
				else
				{
					Obj.VpPlayer2[i].number = -1;
					Obj.VpPlayer2[i].state = FALSE;
					Obj.VPCount2--;
				}
			}
		}
	}
}


BOOL TMonsterAIUtil::FindPathToMoveMonster(CGameObject &Obj, int iTargetX, int iTargetY, int iMaxPathCount, BOOL bPreventOverMoving)
{
	if ( TMonsterAIUtil::CheckMovingCondition(lpObj)==FALSE)
		return FALSE;

	if ( MAX_MAP_RANGE(Obj.MapNumber)==FALSE)
		return FALSE;

	PATH_t Path;
	BOOL bPathFound = FALSE;

	if ( bPreventOverMoving == 1 )
		bPathFound = MapC[Obj.MapNumber].PathFinding2(Obj.X, Obj.Y, iTargetX, iTargetY, &Path);
	else
		bPathFound = MapC[Obj.MapNumber].PathFinding4(Obj.X, Obj.Y, iTargetX, iTargetY, &Path);

	if (bPathFound )
	{
		Obj.m_LastMoveTime = GetTickCount();

		int iTargetX;
		int iTargetY;
		int iStartX;
		int iStartY;
		int iResultX;
		int iResultY;
		BYTE btTargetDir = 0;

		iStartX = Obj.X;
		iStartY = Obj.Y;
		iResultX = Obj.X;
		iResultY = Obj.Y;
		Obj.PathCount = Path.PathNum;
		Obj.PathCur = 1;
		Obj.PathStartEnd = 1;

		if ( Obj.PathCount > iMaxPathCount )
			Obj.PathCount = iMaxPathCount;

		Obj.PathX[0] = Obj.X;
		Obj.PathY[0] = Obj.Y;
		Obj.PathDir[0] = Obj.Dir;

		for(int n=1;n<Obj.PathCount;n++)
		{
			iTargetX = Path.PathX[n];
			iTargetY = Path.PathY[n];
			btTargetDir = GetPathPacketDirPos(iTargetX-iStartX, iTargetY-iStartY);
			Obj.PathX[n] = iTargetX;
			Obj.PathY[n] = iTargetY;
			Obj.PathDir[n] = btTargetDir;
			iResultX += RoadPathTable[btTargetDir*2];
			iResultY += RoadPathTable[btTargetDir*2+1];
		}

		Obj.MTX = iResultX;
		Obj.MTY = iResultY;
		Obj.TX = iResultX;
		Obj.TY = iResultY;

		if ( Obj.PathCount > 0 )
		{
			MapC[Obj.MapNumber].ClearStandAttr(iStartX, iStartY);
			MapC[Obj.MapNumber].SetStandAttr(iResultX, iResultY);
			Obj.m_OldX = iStartX;
			Obj.m_OldY = iStartY;
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
	if ( !Obj.Live )
		return FALSE;

	if ( Obj.RegenOk > 0 )
		return FALSE;

	if ( Obj.Teleport )
		return FALSE;

	if ( Obj.m_State != 2 )
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
	int maxmoverange = Obj.m_MoveRange*2+1;
	int searchc=10;
	Obj.NextActionTime = 1000;
	BYTE tpx;
	BYTE tpy;

	while ( searchc-- != 0 )
	{
		__try
		{
			tpx = (Obj.X - Obj.m_MoveRange) + (rand()%maxmoverange);
			tpy = (Obj.Y - Obj.m_MoveRange) + (rand()%maxmoverange);
		}
		__except(maxmoverange=1, 1)
		{

		}

		BYTE attr = MapC[Obj.MapNumber].GetAttr(tpx, tpy);

		if ( (attr&1)!=1 && (attr&2)!=2 && (attr&4)!=4 && (attr&8)!= 8 )
		{
			Obj.MTX = tpx;
			Obj.MTY = tpy;
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

	int iTargetIndex = Obj.TargetNumber;

	if ( ObjectMaxRange(iTargetIndex) == FALSE )
	{
		return FALSE;
	}

	lpTargetObj = &getGameObject(iTargetIndex);
	tpx = lpTargetObj.X;
	mtx = tpx;
	tpy = lpTargetObj.Y;
	mty = tpy;
	tx = Obj.X;
	ty = Obj.Y;
	int dis = Obj.m_AttackRange / sqrt(2.0);

	if ( Obj.X < mtx )
	{
		tx -= dis;
	}

	if ( Obj.X > mtx )
	{
		tx += dis;
	}

	if ( Obj.Y < mty )
	{
		ty -= dis;
	}

	if ( Obj.Y > mty )
	{
		ty += dis;
	}

	searchp = GetPathPacketDirPos( (Obj.X - tx), (Obj.Y - ty) ) * 2;

	if ( MapC[Obj.MapNumber].GetStandAttr(tx, ty) == 0 )
	{
		while ( searchcount-- )
		{
			mtx = Obj.X + RoadPathTable[searchp];
			mty = Obj.Y + RoadPathTable[1+searchp];
			attr = MapC[Obj.MapNumber].GetAttr(mtx, mty);
			
			if ( (attr&1) != 1 && (attr&2) != 2 && (attr&4) != 4 && (attr&8) != 8  )
			{
				Obj.MTX = mtx;
				Obj.MTY = mty;
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

	attr = MapC[Obj.MapNumber].GetAttr(tx, ty);

	if ( (attr&1) != 1 && (attr&2) != 2 && (attr&4) != 4 && (attr&8) != 8  )
	{
		Obj.MTX = tx;
		Obj.MTY = ty;
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

	int iTargetIndex = Obj.TargetNumber;

	if ( ObjectMaxRange(iTargetIndex) == FALSE )
	{
		return FALSE;
	}

	lpTargetObj = &getGameObject(iTargetIndex);
	tpx = lpTargetObj.X;
	mtx = tpx;
	tpy = lpTargetObj.Y;
	mty = tpy;
	int dis = Obj.m_AttackRange / sqrt(2.0);

	if ( Obj.X < mtx )
	{
		tpx -= dis;
	}

	if ( Obj.X > mtx )
	{
		tpx += dis;
	}

	if ( Obj.Y < mty )
	{
		tpy -= dis;
	}

	if ( Obj.Y > mty )
	{
		tpy += dis;
	}

	searchp = GetPathPacketDirPos( (lpTargetObj.X - tpx), (lpTargetObj.Y - tpy) ) * 2;

	if ( MapC[Obj.MapNumber].GetStandAttr(tpx, tpy) == 0 )
	{
		while ( searchcount-- )
		{
			mtx = lpTargetObj.X + RoadPathTable[searchp];
			mty = lpTargetObj.Y + RoadPathTable[1+searchp];
			attr = MapC[Obj.MapNumber].GetAttr(mtx, mty);
			
			if ( (attr&1) != 1 && (attr&2) != 2 && (attr&4) != 4 && (attr&8) != 8  )
			{
				Obj.MTX = mtx;
				Obj.MTY = mty;
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

	attr = MapC[Obj.MapNumber].GetAttr(tpx, tpy);

	if ( (attr&1) != 1 && (attr&2) != 2 && (attr&4) != 4 && (attr&8) != 8  )
	{
		Obj.MTX = tpx;
		Obj.MTY = tpy;
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

	pMove.NumberH = SET_NUMBERH(Obj.m_Index);
	pMove.NumberL = SET_NUMBERL(Obj.m_Index);
	pMove.X = Obj.MTX;
	pMove.Y = Obj.MTY;
	pMove.Path = Obj.Dir << 4;

	if ( Obj.Class == 541 )
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
	std::memcpy(szChat, szBuffer, sizeof(szChat)-1);

	for(int i=0;i<MaxViewportMonster;i++)
	{
		if ( Obj.VpPlayer2[i].state )
		{
			int tObjNum = Obj.VpPlayer2[i].number;

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

