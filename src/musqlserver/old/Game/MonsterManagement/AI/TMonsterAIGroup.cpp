// TMonsterAIGroup.cpp: implementation of the TMonsterAIGroup class.
//	GS-N	1.00.18	JPN	0x0055FB90	-	Completed
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "TMonsterAIGroup.h"
#include "User/CUserData.h"
#include "Gamemain.h"
#include "Logging/Log.h"

bool TMonsterAIGroup::s_bDataLoad = FALSE;
TMonsterAIGroupMember TMonsterAIGroup::s_MonsterAIGroupMemberArray[MAX_MONSTER_AI_GROUP][MAX_MONSTER_AI_GROUP_MEMBER];
int TMonsterAIGroup::s_iMonsterAIGroupMemberCount[MAX_MONSTER_AI_GROUP];

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TMonsterAIGroup::TMonsterAIGroup()
{
	this->DelAllGroupInfo();
}

TMonsterAIGroup::~TMonsterAIGroup()
{
	return;
}


bool TMonsterAIGroup::LoadData(LPSTR lpszFileName)
{
	TMonsterAIGroup::s_bDataLoad = FALSE;

	if ( !lpszFileName || !strcmp(lpszFileName, ""))
	{
		sLog->outError("[Monster AI Group] - File load error : File Name Error");
		return FALSE;
	}

	try
	{
		pugi::xml_document file;
		pugi::xml_parse_result res = file.load_file(lpszFileName);

		if ( res.status != pugi::status_ok )
		{
			sLog->outError("[Monster AI Group] - Can't Load %s (%s)", lpszFileName, res.description());
			return FALSE;
		}

		pugi::xml_node mainXML = file.child("MonsterAI");

		for (pugi::xml_node group = mainXML.child("Group"); group; group = group.next_sibling())
		{
			int iGroupNumber = group.attribute("Number").as_int();
			int iGuid = group.attribute("GUID").as_int();
			int iClass = group.attribute("MonsterIndex").as_int();
			int iRank = group.attribute("Rank").as_int();
			int iStartAI = group.attribute("StartAI").as_int();
			int iAI01 = group.attribute("AI01").as_int();
			int iAI02 = group.attribute("AI02").as_int();
			int iAI03 = group.attribute("AI03").as_int();
			int iCreateType = group.attribute("CreateType").as_int();
			int iMapNumber = group.attribute("MapNumber").as_int();
			int iStartX = group.attribute("StartX").as_int();
			int iStartY = group.attribute("StartY").as_int();
			int iStartDir = group.attribute("Dir").as_int();
			int iRegenType = group.attribute("RegenType").as_int();

			if ( iGroupNumber > MAX_MONSTER_AI_GROUP )
			{
				sLog->outError("[Monster AI Group] Exceed Max GroupNumber %d", iGroupNumber);
				continue;
			}

			if ( iGuid > MAX_MONSTER_AI_GROUP_MEMBER)
			{
				sLog->outError("[Monster AI Group] Exceed Max GroupMember %d, %d ", iGroupNumber, iGuid);
				continue;
			}

			TMonsterAIGroup::s_MonsterAIGroupMemberArray[iGroupNumber][iGuid].m_iGroupNumber = iGroupNumber;
			TMonsterAIGroup::s_MonsterAIGroupMemberArray[iGroupNumber][iGuid].m_iGuid = iGuid;
			TMonsterAIGroup::s_MonsterAIGroupMemberArray[iGroupNumber][iGuid].m_iClass = iClass;
			TMonsterAIGroup::s_MonsterAIGroupMemberArray[iGroupNumber][iGuid].m_iRank = iRank;
			TMonsterAIGroup::s_MonsterAIGroupMemberArray[iGroupNumber][iGuid].m_iStartAI = iStartAI;
			TMonsterAIGroup::s_MonsterAIGroupMemberArray[iGroupNumber][iGuid].m_iAI01 = iAI01;
			TMonsterAIGroup::s_MonsterAIGroupMemberArray[iGroupNumber][iGuid].m_iAI02 = iAI02;
			TMonsterAIGroup::s_MonsterAIGroupMemberArray[iGroupNumber][iGuid].m_iAI03 = iAI03;
			TMonsterAIGroup::s_MonsterAIGroupMemberArray[iGroupNumber][iGuid].m_iCreateType = iCreateType;
			TMonsterAIGroup::s_MonsterAIGroupMemberArray[iGroupNumber][iGuid].m_iMapNumber = iMapNumber;
			TMonsterAIGroup::s_MonsterAIGroupMemberArray[iGroupNumber][iGuid].m_iStartX = iStartX;
			TMonsterAIGroup::s_MonsterAIGroupMemberArray[iGroupNumber][iGuid].m_iStartY = iStartY;
			TMonsterAIGroup::s_MonsterAIGroupMemberArray[iGroupNumber][iGuid].m_iStartDir = iStartDir;
			TMonsterAIGroup::s_MonsterAIGroupMemberArray[iGroupNumber][iGuid].m_iRegenType = iRegenType;		
			TMonsterAIGroup::s_iMonsterAIGroupMemberCount[iGroupNumber]++;
		}

		TMonsterAIGroup::s_bDataLoad = TRUE;
	}

	catch ( DWORD )
	{
		sLog->outError("[Monster AI Group] - Loading Exception Error (%s) File. ", lpszFileName);
	}

	return FALSE;
}



void TMonsterAIGroup::ChangeAIOrder(int iGroupNumber, int iAIOrder)
{
	if ( iGroupNumber < 0 || iGroupNumber >= MAX_MONSTER_AI_GROUP )
	{
		sLog->outBasic("[Monster AI Group] ChangeAIOrder() Error - (GroupNumber=%d)", iGroupNumber);
		return;
	}

	TMonsterAIGroupMember * lpMemb = NULL;

	for ( int j=0;j<TMonsterAIGroup::s_iMonsterAIGroupMemberCount[iGroupNumber];j++)
	{
		lpMemb = &TMonsterAIGroup::s_MonsterAIGroupMemberArray[iGroupNumber][j];

		if ( lpMemb->m_iGroupNumber == iGroupNumber )
		{
			if ( iAIOrder == 0 )
			{
				getGameObject(lpMemb->m_iObjIndex)->m_iBasicAI = lpMemb->m_iStartAI;
				getGameObject(lpMemb->m_iObjIndex)->m_iCurrentAI = lpMemb->m_iStartAI;
			}
			else if (iAIOrder == 1 )
			{
				getGameObject(lpMemb->m_iObjIndex)->m_iBasicAI = lpMemb->m_iAI01;
				getGameObject(lpMemb->m_iObjIndex)->m_iCurrentAI = lpMemb->m_iAI01;
			}
			else if (iAIOrder == 2 )
			{
				getGameObject(lpMemb->m_iObjIndex)->m_iBasicAI = lpMemb->m_iAI02;
				getGameObject(lpMemb->m_iObjIndex)->m_iCurrentAI = lpMemb->m_iAI02;
			}
			else if (iAIOrder == 3 )
			{
				getGameObject(lpMemb->m_iObjIndex)->m_iBasicAI = lpMemb->m_iAI03;
				getGameObject(lpMemb->m_iObjIndex)->m_iCurrentAI = lpMemb->m_iAI03;
			}
		}		
	}
}



bool TMonsterAIGroup::DelGroupInstance(int iGroupNumber)
{
	if ( iGroupNumber < 0 || iGroupNumber >= MAX_MONSTER_AI_GROUP )
	{
		sLog->outBasic("[Monster AI Group] DelGroup() Error - (GroupNumber=%d)", iGroupNumber);
		return FALSE;
	}

	TMonsterAIGroupMember * lpMemb = NULL;

	for ( int j=0;j<TMonsterAIGroup::s_iMonsterAIGroupMemberCount[iGroupNumber];j++)
	{
		lpMemb = &TMonsterAIGroup::s_MonsterAIGroupMemberArray[iGroupNumber][j];

		if ( lpMemb->m_iGroupNumber == iGroupNumber )
		{
			if ( lpMemb->m_iObjIndex != -1 )
			{
				gObjDel(lpMemb->m_iObjIndex);
			}
		}
	}

	return FALSE;
}



bool TMonsterAIGroup::DelAllGroupInfo()
{
	for ( int i=0;i<MAX_MONSTER_AI_GROUP;i++)
	{
		for ( int j=0;j<MAX_MONSTER_AI_GROUP_MEMBER;j++)
		{
			TMonsterAIGroup::s_MonsterAIGroupMemberArray[i][j].Reset();
		}

		TMonsterAIGroup::s_iMonsterAIGroupMemberCount[i] = 0;
	}

	TMonsterAIGroup::s_bDataLoad = FALSE;
	return FALSE;
}

int TMonsterAIGroup::FindGroupLeader(int iGroupNumber)
{
	if ( iGroupNumber < 0 || iGroupNumber >= MAX_MONSTER_AI_GROUP )
	{
		sLog->outBasic("[Monster AI Group] FindGroupLeader() Error - (GroupNumber=%d)", iGroupNumber);
		return -1;
	}

	for ( int i=0;i<MAX_MONSTER_AI_GROUP_MEMBER;i++)
	{
		if ( TMonsterAIGroup::s_MonsterAIGroupMemberArray[iGroupNumber][i].m_iRank == 0 )
		{
			return TMonsterAIGroup::s_MonsterAIGroupMemberArray[iGroupNumber][i].m_iObjIndex;
		}
	}

	return -1;
}



int TMonsterAIGroup::FindGroupMemberObjectIndex(int iGroupNumber, int iGuid)
{
	if ( iGroupNumber < 0 || iGroupNumber >= MAX_MONSTER_AI_GROUP )
	{
		sLog->outBasic("[Monster AI Group] FindGroupMemberObjectIndex() Error - (GroupNumber=%d Guid=%d)", iGroupNumber, iGuid);
		return -1;
	}

	for ( int i=0;i<MAX_MONSTER_AI_GROUP_MEMBER;i++)
	{
		if ( TMonsterAIGroup::s_MonsterAIGroupMemberArray[iGroupNumber][i].m_iGuid == iGuid )
		{
			return TMonsterAIGroup::s_MonsterAIGroupMemberArray[iGroupNumber][i].m_iObjIndex;
		}
	}

	return -1;
}



TMonsterAIGroupMember * TMonsterAIGroup::FindGroupMember(int iGroupNumber, int iGuid)
{
	if ( iGroupNumber < 0 || iGroupNumber >= MAX_MONSTER_AI_GROUP )
	{
		sLog->outBasic("[Monster AI Group] FindGroupMember() Error - (GroupNumber=%d)(Guid=%d) ", iGroupNumber, iGuid);
		return NULL;
	}

	if ( TMonsterAIGroup::s_MonsterAIGroupMemberArray[iGroupNumber][iGuid].m_iGroupNumber == iGroupNumber &&
		 TMonsterAIGroup::s_MonsterAIGroupMemberArray[iGroupNumber][iGuid].m_iGuid == iGuid )
	{
		return &TMonsterAIGroup::s_MonsterAIGroupMemberArray[iGroupNumber][iGuid];
	}

	sLog->outBasic("[Monster AI Group] FindGroupMember() Error - (GroupNumber=%d)(Guid=%d) ", iGroupNumber, iGuid);

	return NULL;
}


TMonsterAIGroupMember * TMonsterAIGroup::FindGroupMemberToHeal(int iObjIndex, int iGroupNumber, int iGuid, int iDistance)
{
	if ( TMonsterAIGroup::s_iMonsterAIGroupMemberCount[iGroupNumber] == 0 )
		return NULL;

	int iMinHPGuid = -1;
	int iMinHP = 100000000;
	int iCount = TMonsterAIGroup::s_iMonsterAIGroupMemberCount[iGroupNumber];

	for ( int i=0;i<iCount;i++)
	{
		if ( TMonsterAIGroup::s_MonsterAIGroupMemberArray[iGroupNumber][i].m_iGroupNumber == iGroupNumber )
		{
			CGameObject lpTargetObj = &getGameObject(TMonsterAIGroup::s_MonsterAIGroupMemberArray[iGroupNumber][i]->m_iObjIndex);

			if ( lpTargetObj == NULL )
				continue;

			if ( lpTargetObj.Live == false )
				continue;

			if ( gObjCalDistance(&getGameObject(iObjIndex), lpTargetObj) > iDistance )
				continue;

			int iMaxLife = lpTargetObj.MaxLife + 1.0f;
			int iPercentHP = lpTargetObj.Life * 100.0f / iMaxLife;

			if ( iPercentHP < 60 )
			{
				if ( lpTargetObj.m_Index == iObjIndex )
					iPercentHP = 1;

				if ( iPercentHP < iMinHP )
				{
					iMinHP = iPercentHP;
					iMinHPGuid = lpTargetObj.m_iGroupMemberGuid;
				}
			}
		}
	}

	if ( iMinHPGuid != -1 )
		return &TMonsterAIGroup::s_MonsterAIGroupMemberArray[iGroupNumber][iMinHPGuid];
	
	return NULL;
}


TMonsterAIGroupMember * TMonsterAIGroup::FindGroupMemberToSommon(int iObjIndex, int iGroupNumber, int iGuid)
{
	if ( TMonsterAIGroup::s_iMonsterAIGroupMemberCount[iGroupNumber] == 0 )
		return NULL;

	int iCount = TMonsterAIGroup::s_iMonsterAIGroupMemberCount[iGroupNumber];

	for ( int i=0;i<iCount;i++)
	{
		if ( TMonsterAIGroup::s_MonsterAIGroupMemberArray[iGroupNumber][i].m_iGroupNumber == iGroupNumber )
		{
			CGameObject lpTargetObj = &getGameObject(TMonsterAIGroup::s_MonsterAIGroupMemberArray[iGroupNumber][i]->m_iObjIndex);

			if ( lpTargetObj.m_Index == iObjIndex )
				continue;

			if ( lpTargetObj == NULL )
				continue;

			if ( lpTargetObj.m_iRegenType == -1 )
				continue;

			if ( lpTargetObj.Live == FALSE )
			{
				return &TMonsterAIGroup::s_MonsterAIGroupMemberArray[iGroupNumber][i];
			}
		}
	}

	return NULL;
}


void TMonsterAIGroup::InitAll()
{
	for ( int i=0;i<MAX_MONSTER_AI_GROUP;i++)
	{
		if ( TMonsterAIGroup::s_iMonsterAIGroupMemberCount[i] == 0 )
			continue;

		TMonsterAIGroup::Init(i);
	}
}


void TMonsterAIGroup::Init(int iGroupNumber)
{
	if ( TMonsterAIGroup::s_iMonsterAIGroupMemberCount[iGroupNumber] == 0 )
		return;

	TMonsterAIGroup::DelGroupInstance(iGroupNumber);

	for ( int j=0;j<MAX_MONSTER_AI_GROUP_MEMBER;j++)
	{
		TMonsterAIGroupMember & Memb = TMonsterAIGroup::s_MonsterAIGroupMemberArray[iGroupNumber][j];

		if ( Memb.m_iGuid == -1 )
			continue;

		int iResult = gObjAddMonster(Memb.m_iMapNumber);

		if ( iResult >= 0 )
		{
			getGameObject(iResult)->m_PosNum = -1;
			getGameObject(iResult)->MapNumber = Memb.m_iMapNumber;
			getGameObject(iResult)->Live = TRUE;

			gObjViewportListProtocolDestroy(&getGameObject(iResult));
			gObjViewportClose(&getGameObject(iResult));

			BYTE cX;
			BYTE cY;

			if ( Memb.m_iCreateType == 1 )
			{
				int iRadius = 10;
				BOOL bGetPosition = FALSE;
				int iCount = 100;

				while ( iCount-- != 0 )
				{
					cX = ( rand() % (iRadius+1) ) * (((rand()%2==0)?-1:1)) + Memb.m_iStartX;
					cY = ( rand() % (iRadius+1) ) * (((rand()%2==0)?-1:1)) + Memb.m_iStartY;

					BYTE btMapAttr = MapC[Memb.m_iMapNumber].GetAttr(cX, cY);

					if ( btMapAttr == 0 )
					{
						bGetPosition = TRUE;
						break;
					}
				}

				if ( bGetPosition == FALSE )
				{
					getGameObject(iResult)->Live = FALSE;
					getGameObject(iResult)->m_State = 4;
					getGameObject(iResult)->RegenTime = GetTickCount();
					getGameObject(iResult)->DieRegen = 1;

					return;
				}
			}
			else if ( Memb.m_iCreateType == 0 )
			{
				cX = Memb.m_iStartX;
				cY = Memb.m_iStartY;
			}

			getGameObject(iResult)->X = cX;
			getGameObject(iResult)->Y = cY;
			getGameObject(iResult]->MTX = getGameObject(iResult)->X;
			getGameObject(iResult]->MTY = getGameObject(iResult)->Y;
			getGameObject(iResult]->TX = getGameObject(iResult)->X;
			getGameObject(iResult]->TY = getGameObject(iResult)->Y;
			getGameObject(iResult]->StartX = getGameObject(iResult)->X;
			getGameObject(iResult]->StartY = getGameObject(iResult)->Y;

			gObjSetMonster(iResult, Memb.m_iClass);

			getGameObject(iResult)->m_iGroupNumber = Memb.m_iGroupNumber;
			getGameObject(iResult)->m_iGroupMemberGuid = Memb.m_iGuid;
			getGameObject(iResult)->m_iCurrentAI = Memb.m_iStartAI;
			getGameObject(iResult)->m_iBasicAI = Memb.m_iStartAI;
			getGameObject(iResult)->m_iRegenType = Memb.m_iRegenType;
			getGameObject(iResult)->Dir = Memb.m_iStartDir;
			getGameObject(iResult)->m_State = 1;
			getGameObject(iResult)->DieRegen = 0;
			Memb.m_iObjIndex = iResult;

			if ( Memb.m_iCreateType == -1 )
			{
				getGameObject(iResult)->Live = FALSE;
				getGameObject(iResult)->m_State = 4;
				getGameObject(iResult)->RegenTime = GetTickCount();
				getGameObject(iResult)->DieRegen = 1;

				continue;
			}

			sLog->outBasic("[ AI SetMonster ] %s(Index:%d ObjIndex:%d) Map:%d-[%d][%d]",
				getGameObject(iResult]->Name, getGameObject(iResult)->Class, iResult, getGameObject(iResult)->MapNumber,
				getGameObject(iResult]->X, getGameObject(iResult)->Y);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

