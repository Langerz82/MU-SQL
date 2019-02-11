// LifeStone.cpp: implementation of the CLifeStone class.
//	GS-CS	1.00.19	JPN	0x00562650	-	Completed
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "LifeStone.h"
#include "CastleSiege.h"
#include "CastleSiegeSync.h"
#include "Gamemain.h"
#include "User/CUserData.h"
#include "MapClass.h"
#include "Logging/Log.h"
#include "configread.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLifeStone g_CsNPC_LifeStone;

CLifeStone::CLifeStone()
{
	return;
}

CLifeStone::~CLifeStone()
{
	return;
}

//////////////////////////////////////////////////////////////////////
// Member Functions
//////////////////////////////////////////////////////////////////////

int CLifeStone::CreateLifeStone(int iIndex)
{
	CGameObject lpObj = &gGameObjects[iIndex];
	int iMonsterIndex = -1;
	BYTE cX = lpObj.X;
	BYTE cY = lpObj.Y;

	if ( g_CastleSiegeSync.GetCastleState() != 7 )
	{
		MsgOutput(iIndex, Lang.GetText(0,179));
		return FALSE;
	}

	if ( lpObj.m_PlayerData->GuildStatus != 0x80 )
		return FALSE;

	if ( lpObj.m_btCsJoinSide < 2 )
	{
		MsgOutput(iIndex, Lang.GetText(0,176));
		return FALSE;
	}

	if ( lpObj.m_PlayerData->lpGuild->lpLifeStone  )
	{
		MsgOutput(iIndex, Lang.GetText(0,177));
		return FALSE;
	}

	if ( cX > 150 && cX < 210 && cY > 175 && cY < 230 )
	{
		MsgOutput(iIndex, Lang.GetText(0,178));
		return FALSE;
	}

	BYTE btMapAttr = MapC[lpObj.MapNumber].GetAttr(cX, cY);

	if ( gGameObjects[iIndex]->MapNumber != MAP_INDEX_CASTLESIEGE )
	{
		MsgOutput(iIndex, Lang.GetText(0,170));
		return FALSE;
	}

	iMonsterIndex = gObjAddMonster(lpObj.MapNumber);

	if ( iMonsterIndex >= 0 )
	{
		MONSTER_ATTRIBUTE * MAttr = gMAttr.GetAttr(278);

		if ( MAttr == NULL )
		{
			gObjDel(iMonsterIndex);
			return FALSE;
		}

		gObjSetMonster(iMonsterIndex, 278);
		
		gGameObjects[iMonsterIndex]->m_PlayerData = new CUserData(iMonsterIndex);
		gGameObjects[iMonsterIndex]->Live = TRUE;
		gGameObjects[iMonsterIndex]->Life = MAttr->m_Hp;
		gGameObjects[iMonsterIndex]->MaxLife = MAttr->m_Hp;
		gGameObjects[iMonsterIndex]->m_PosNum = -1;
		gGameObjects[iMonsterIndex]->X = cX;
		gGameObjects[iMonsterIndex]->Y = cY;
		gGameObjects[iMonsterIndex]->MTX = cX;
		gGameObjects[iMonsterIndex]->MTY = cY;
		gGameObjects[iMonsterIndex]->TX = cX;
		gGameObjects[iMonsterIndex]->TY = cY;
		gGameObjects[iMonsterIndex]->m_OldX = cX;
		gGameObjects[iMonsterIndex]->m_OldY = cY;
		gGameObjects[iMonsterIndex]->StartX = cX;
		gGameObjects[iMonsterIndex]->StartY = cY;
		gGameObjects[iMonsterIndex]->MapNumber = lpObj.MapNumber;
		gGameObjects[iMonsterIndex]->m_MoveRange = 0;
		gGameObjects[iMonsterIndex]->Level = MAttr->m_Level;
		gGameObjects[iMonsterIndex]->Type = OBJ_MONSTER;
		gGameObjects[iMonsterIndex]->MaxRegenTime = 1000;
		gGameObjects[iMonsterIndex]->Dir = 1;
		gGameObjects[iMonsterIndex]->RegenTime = GetTickCount();
		gGameObjects[iMonsterIndex]->m_Attribute = 0;
		gGameObjects[iMonsterIndex]->DieRegen = 0;
		gGameObjects[iMonsterIndex]->m_btCsNpcType = OBJ_NPC;
		gGameObjects[iMonsterIndex]->m_btCsJoinSide = lpObj.m_btCsJoinSide;
		gGameObjects[iMonsterIndex]->m_PlayerData->lpGuild = lpObj.m_PlayerData->lpGuild;
		gGameObjects[iMonsterIndex]->m_btCreationState = 0;
		lpObj.m_PlayerData->lpGuild->lpLifeStone = &gGameObjects[iMonsterIndex];

		MsgOutput(iIndex, Lang.GetText(0,180));

		sLog->outBasic("[CastleSiege] LifeStone is created - [%s] [%s][%s] (Map:%d)(X:%d, Y:%d)",
			lpObj.m_PlayerData->lpGuild->Name, lpObj.AccountID, lpObj.Name, lpObj.MapNumber, cX, cY);

		lpObj.m_btLifeStoneCount++;

	}
	else
	{
		MsgOutput(iIndex, Lang.GetText(0,181));
		return FALSE;
	}

	return TRUE;
}

int CLifeStone::DeleteLifeStone(int iIndex)
{
	if ( iIndex < 0 || iIndex > g_ConfigRead.server.GetObjectMax()-1 )
		return FALSE;

	CGameObject lpLifeStone = &gGameObjects[iIndex];

	if ( lpLifeStone->m_PlayerData->lpGuild )
	{
		sLog->outBasic("[CastleSiege] LifeStone is broken - [%s]", lpLifeStone->m_PlayerData->lpGuild->Name);
		lpLifeStone->m_PlayerData->lpGuild->lpLifeStone = NULL;
		delete lpLifeStone->m_PlayerData;
	}

	return TRUE;
}

int CLifeStone::SetReSpawnUserXY(int iUserIndex)
{
	if ( !gObjIsConnected(iUserIndex) )
		return FALSE;

	CGameObject lpUser = &gGameObjects[iUserIndex];

	if ( lpUser->MapNumber != MAP_INDEX_CASTLESIEGE )
		return FALSE;

	if ( lpUser->m_PlayerData->lpGuild == NULL )
		return FALSE;

	BYTE btCsJoinSide = lpUser->m_btCsJoinSide;

	if ( lpUser->m_PlayerData->lpGuild->lpLifeStone == NULL )
		return FALSE;

	CGameObject lpLifeStone = lpUser->m_PlayerData->lpGuild->lpLifeStone;

	if ( lpLifeStone->m_btCreationState != 5 )
		return FALSE;

	lpUser->MapNumber = MAP_INDEX_CASTLESIEGE;
	lpUser->X = lpLifeStone->X;
	lpUser->Y = lpLifeStone->Y;

	return TRUE;
}

void CLifeStone::LifeStoneAct(int iIndex)
{
	if ( !gObjIsConnected(iIndex))
		return;

	CGameObject lpObj = &gGameObjects[iIndex];

	lpObj.m_iCreatedActivationTime++;
	BYTE btCreationState = lpObj.m_btCreationState;

	if ( lpObj.m_iCreatedActivationTime < 60 )
		lpObj.m_btCreationState = lpObj.m_iCreatedActivationTime / 12;
	else
		lpObj.m_btCreationState = 5;

	if ( btCreationState != lpObj.m_btCreationState )
		GSProtocol.GCSendObjectCreationState(iIndex);

	if ( lpObj.m_btCreationState < 5 )
		return;

	if ( lpObj.VPCount < 1 ) 
		return;

	int tObjNum = -1;

	for (int i=0;i<MAX_VIEWPORT;i++)
	{
		tObjNum = lpObj.VpPlayer[i].number;

		if ( tObjNum >= 0 )
		{
			if ( gGameObjects[tObjNum]->Type == OBJ_USER && gGameObjects[tObjNum]->Live )
			{
				if ( gGameObjects[tObjNum]->m_btCsJoinSide == lpObj.m_btCsJoinSide )
				{
					if ( abs(lpObj.Y - gGameObjects[tObjNum]->Y) <= 3 &&
						 abs(lpObj.X - gGameObjects[tObjNum]->X) <= 3 )
					{
						BOOL bLifeChange = FALSE;
						BOOL bManaChange = FALSE;
						BOOL bBpChange = FALSE;

						/*gGameObjects[tObjNum]->Life += 100.0f;
						gGameObjects[tObjNum]->Mana += 100.0f;
						gGameObjects[tObjNum]->BP += 100;*/

						if ( gGameObjects[tObjNum]->Life < (gGameObjects[tObjNum]->MaxLife + gGameObjects[tObjNum]->AddLife))
						{
							gGameObjects[tObjNum]->Life += ( gGameObjects[tObjNum]->MaxLife + gGameObjects[tObjNum]->AddLife ) / 100.0f;

							if ( gGameObjects[tObjNum]->Life > (gGameObjects[tObjNum]->MaxLife + gGameObjects[tObjNum]->AddLife))
								gGameObjects[tObjNum]->Life = gGameObjects[tObjNum]->MaxLife + gGameObjects[tObjNum]->AddLife;

							bLifeChange = TRUE;
						}

						if ( gGameObjects[tObjNum]->Mana < (gGameObjects[tObjNum]->MaxMana + gGameObjects[tObjNum]->AddMana))
						{
							gGameObjects[tObjNum]->Mana += ( gGameObjects[tObjNum]->MaxMana + gGameObjects[tObjNum]->AddMana ) / 100.0f;

							if ( gGameObjects[tObjNum]->Mana > (gGameObjects[tObjNum]->MaxMana + gGameObjects[tObjNum]->AddMana))
								gGameObjects[tObjNum]->Mana = gGameObjects[tObjNum]->MaxMana + gGameObjects[tObjNum]->AddMana;

							bManaChange = TRUE;
						}

						if ( gGameObjects[tObjNum]->BP < (gGameObjects[tObjNum]->MaxBP + gGameObjects[tObjNum]->AddBP))
						{
							gGameObjects[tObjNum]->BP += ( gGameObjects[tObjNum]->MaxBP + gGameObjects[tObjNum]->AddBP ) / 100;

							if ( gGameObjects[tObjNum]->BP > (gGameObjects[tObjNum]->MaxBP + gGameObjects[tObjNum]->AddBP))
								gGameObjects[tObjNum]->BP = gGameObjects[tObjNum]->MaxBP + gGameObjects[tObjNum]->AddBP;

							bBpChange = TRUE;
						}

						if (bLifeChange )
							GSProtocol.GCReFillSend(tObjNum, gGameObjects[tObjNum]->Life, 0xFF, 1, gGameObjects[tObjNum]->iShield);

						if (bManaChange ||bBpChange ) 
							GSProtocol.GCManaSend(tObjNum, gGameObjects[tObjNum]->Mana, 0xFF, 0, gGameObjects[tObjNum]->BP);
					}
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

