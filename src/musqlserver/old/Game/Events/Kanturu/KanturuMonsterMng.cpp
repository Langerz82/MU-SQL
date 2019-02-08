// KanturuMonsterMng.cpp: implementation of the CKanturuMonsterMng class.
//	GS-N	1.00.18	JPN	0x00578BB0	-	Completed
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KanturuMonsterMng.h"
#include "Gamemain.h"
#include "Logging/Log.h"
#include "configread.h"

CKanturuMonsterMng g_KanturuMonsterMng;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CKanturuMonsterMng::CKanturuMonsterMng()
{
	this->m_iMayaObjIndex = -1;
	this->ResetLoadData();
	this->ResetRegenMonsterObjData();
}

CKanturuMonsterMng::~CKanturuMonsterMng()
{
	return;
}

void CKanturuMonsterMng::ResetLoadData()
{
	for ( int iCount=0;iCount<MAX_KANTURU_MONSTER;iCount++)
	{
		memset(&this->m_SetBaseInfo[iCount], -1, sizeof(this->m_SetBaseInfo[iCount]));
	}

	this->m_iMaxMonsterCount = 0;
}


void CKanturuMonsterMng::ResetRegenMonsterObjData()
{
	for ( int iCount=0;iCount<this->m_KanturuMonster.GetCount();iCount++)
	{
		if ( this->m_KanturuMonster.m_iObjIndex[iCount] != -1 )
		{
			gObjDel(this->m_KanturuMonster.m_iObjIndex[iCount]);
		}
	}

	if ( this->m_iMayaObjIndex != -1 )
	{
		gObjDel(this->m_iMayaObjIndex);
	}

	this->m_KanturuMonster.Reset();
	this->m_iMaxRegenMonsterCount = 0;
	this->m_iAliveMonsterCount = 0;
	this->m_iMayaObjIndex = -1;
}


void CKanturuMonsterMng::MonsterDie(int iIndex)
{
	LPOBJ lpObj = &gObj[iIndex];

	for ( int iCount=0;iCount<this->m_KanturuMonster.GetCount();iCount++)
	{
		if ( this->m_KanturuMonster.m_iObjIndex[iCount] == iIndex )
		{
			this->m_iAliveMonsterCount--;
			return;
		}
	}
}

BOOL CKanturuMonsterMng::LoadData(LPSTR lpszFileName)
{
	if (g_ConfigRead.server.GetStateFromEventTable(g_ConfigRead.server.GetServerType(), EV_TABLE_KANTURU) == false)
	{
		return FALSE;
	}

	this->m_bFileDataLoad = FALSE;

	if ( !lpszFileName || !strcmp(lpszFileName , "") )
	{
		sLog->outError("[Kanturu][MonsterSetBase] - File load error : File Name Error");
		return FALSE;
	}

	try
	{
		pugi::xml_document file;
		pugi::xml_parse_result res = file.load_file(lpszFileName);

		if ( res.status != pugi::status_ok )
		{
			sLog->outError("[Kanturu][MonsterSetBase] - Can't Load %s (%s)", lpszFileName, res.description());
			return FALSE;
		}

		this->ResetLoadData();

		pugi::xml_node mainXML = file.child("KanturuEvent");

		for (pugi::xml_node monster = mainXML.child("Monster"); monster; monster = monster.next_sibling())
		{
			BYTE btGroup = monster.attribute("Group").as_int();
			WORD wType = monster.attribute("Index").as_int();
			BYTE btMapNumber = monster.attribute("MapNumber").as_int();
			BYTE btDistance = monster.attribute("Distance").as_int();
			BYTE btX = monster.attribute("StartX").as_int();
			BYTE btY = monster.attribute("StartY").as_int();
			BYTE btDir = monster.attribute("Dir").as_int();

			if ( this->m_iMaxMonsterCount < 0 || this->m_iMaxMonsterCount >= MAX_KANTURU_MONSTER )
			{
				sLog->outError("[Kanturu][MonsterSetBase] - Exceed Max Info Count (%d)", this->m_iMaxMonsterCount);
				break;
			}

			this->SetMonsterSetBaseInfo(this->m_iMaxMonsterCount, btGroup, wType, btMapNumber, btDistance, btX, btY, btDir);
			this->m_iMaxMonsterCount++;
		}

		this->m_bFileDataLoad = TRUE;
	}

	catch ( DWORD )
	{
		sLog->outError("[Kanturu][MonsterSetBase] Loading Exception Error (%s) File. ", lpszFileName);
	}

	return this->m_bFileDataLoad;
}


void CKanturuMonsterMng::SetMonsterSetBaseInfo(int iIndex, BYTE btGroup, WORD wType, BYTE btMapNumber, BYTE btDistance, BYTE btX, BYTE btY, BYTE btDir)
{
	this->m_SetBaseInfo[iIndex].btGroup = btGroup;
	this->m_SetBaseInfo[iIndex].wType = wType;
	this->m_SetBaseInfo[iIndex].btMapNumber = btMapNumber;
	this->m_SetBaseInfo[iIndex].btDistance = btDistance;
	this->m_SetBaseInfo[iIndex].btX = btX;
	this->m_SetBaseInfo[iIndex].btY = btY;
	this->m_SetBaseInfo[iIndex].btDir = btDir;
}


int CKanturuMonsterMng::SetKanturuMonster(int iGroupNumber)
{
	int iMapNumber = 0;
	WORD wType = 0;
	LPOBJ lpObj;
	int iResult;
	int iRegenCount = 0;
	
	for ( int iCount=0;iCount<MAX_KANTURU_MONSTER;iCount++)
	{
		if ( this->m_SetBaseInfo[iCount].btGroup == iGroupNumber &&
			 this->m_SetBaseInfo[iCount].wType != 0 )
		{
			iResult  = gObjAddMonster(this->m_SetBaseInfo[iCount].btMapNumber);

			if ( iResult < 0 )
			{
			}
			else
			{
				lpObj = &gObj[iResult];

				lpObj->m_PosNum = -1;
				lpObj->X = this->m_SetBaseInfo[iCount].btX;
				lpObj->Y = this->m_SetBaseInfo[iCount].btY;
				lpObj->MapNumber = this->m_SetBaseInfo[iCount].btMapNumber;

				if ( this->GetPosition(iCount, lpObj->MapNumber, lpObj->X, lpObj->Y) == FALSE )
				{
					gObjDel(iResult);
					continue;
				}

				lpObj->TX = lpObj->X;
				lpObj->TY = lpObj->Y;
				lpObj->m_OldX = lpObj->X;
				lpObj->m_OldY = lpObj->Y;
				lpObj->StartX = lpObj->X;
				lpObj->StartY = lpObj->Y;
				lpObj->m_MoveRange = this->m_SetBaseInfo[iCount].btDistance;
				lpObj->DieRegen = FALSE;

				if ( this->m_SetBaseInfo[iCount].btDir == 0xFF )
				{
					lpObj->Dir = rand()%8;
				}
				else
				{
					lpObj->Dir = this->m_SetBaseInfo[iCount].btDir;
				}

				if ( gObjSetMonster(iResult, this->m_SetBaseInfo[iCount].wType) == FALSE )
				{
					gObjDel(iResult);
					continue;
				}

				if ( this->m_SetBaseInfo[iCount].wType == 364 )	/// Maya Hand
				{
					this->m_iMayaObjIndex = iResult;

					continue;
				}

				if ( gObj[iResult].Type == OBJ_MONSTER )
				{
					this->m_KanturuMonster.AddObj(iResult);
					this->m_iMaxRegenMonsterCount++;
					this->m_iAliveMonsterCount++;

				}

				iRegenCount++;
			}
		}
	}

	return iRegenCount;
}


BOOL CKanturuMonsterMng::GetPosition(int TableNum, short MapNumber, short & x, short & y)
{
	if ( TableNum < 0 || TableNum > MAX_KANTURU_MONSTER-1 )
	{
		sLog->outBasic("ERROR : %s %d", __FILE__, __LINE__);
		return FALSE;
	}

	int iOrx = this->m_SetBaseInfo[TableNum].btX;
	int iOry = this->m_SetBaseInfo[TableNum].btY;
	BYTE btAttr = MapC[MapNumber].GetAttr(iOrx, iOry);

	if ( (btAttr&1)!=1 && (btAttr&4)!=4 && (btAttr&8)!=8 )
	{
		x = iOrx;
		y = iOry;

		return TRUE;
	}

	int iCount = 100;	// Number of Tries

	while ( iCount-- != 0 )
	{
		iOrx = this->m_SetBaseInfo[TableNum].btX - 3;
		iOry = this->m_SetBaseInfo[TableNum].btY - 3;
		iOrx += rand() % 7;
		iOry += rand() % 7;

		btAttr = MapC[MapNumber].GetAttr(iOrx, iOry);

		if ( (btAttr&1)!=1 && (btAttr&4)!=4 && (btAttr&8)!=8 )
		{
			x = iOrx;
			y = iOry;

			return TRUE;
		}
	}

	return FALSE;
}


BOOL CKanturuMonsterMng::IsExistAliveMonster()
{
	if ( this->m_iAliveMonsterCount <=  0 )
	{
		return FALSE;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

