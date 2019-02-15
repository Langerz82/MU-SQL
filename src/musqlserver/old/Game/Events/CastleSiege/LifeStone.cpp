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

int CLifeStone::CreateLifeStone(CGameObject &Obj)
{
	CGameObject* lpObj = Obj;
	int iMonsterIndex = -1;
	BYTE cX = Obj.X;
	BYTE cY = Obj.Y;

	if ( g_CastleSiegeSync.GetCastleState() != 7 )
	{
		MsgOutput(iIndex, Lang.GetText(0,179));
		return FALSE;
	}

	if ( Obj.m_PlayerData->GuildStatus != 0x80 )
		return FALSE;

	if ( Obj.m_btCsJoinSide < 2 )
	{
		MsgOutput(iIndex, Lang.GetText(0,176));
		return FALSE;
	}

	if ( Obj.m_PlayerData->lpGuild->lpLifeStone  )
	{
		MsgOutput(iIndex, Lang.GetText(0,177));
		return FALSE;
	}

	if ( cX > 150 && cX < 210 && cY > 175 && cY < 230 )
	{
		MsgOutput(iIndex, Lang.GetText(0,178));
		return FALSE;
	}

	BYTE btMapAttr = MapC[Obj.MapNumber].GetAttr(cX, cY);

	if ( Obj.MapNumber != MAP_INDEX_CASTLESIEGE )
	{
		MsgOutput(iIndex, Lang.GetText(0,170));
		return FALSE;
	}

	iMonsterIndex = gObjAddMonster(Obj.MapNumber);

	if ( iMonsterIndex >= 0 )
	{
		MONSTER_ATTRIBUTE * MAttr = gMAttr.GetAttr(278);

		if ( MAttr == NULL )
		{
			gObjDel(iMonsterIndex);
			return FALSE;
		}

		gObjSetMonster(iMonsterIndex, 278);
		
		getGameObject(iMonsterIndex)->m_PlayerData = new CUserData(iMonsterIndex);
		getGameObject(iMonsterIndex)->Live = TRUE;
		getGameObject(iMonsterIndex)->Life = MAttr->m_Hp;
		getGameObject(iMonsterIndex)->MaxLife = MAttr->m_Hp;
		getGameObject(iMonsterIndex)->m_PosNum = -1;
		getGameObject(iMonsterIndex)->X = cX;
		getGameObject(iMonsterIndex)->Y = cY;
		getGameObject(iMonsterIndex)->MTX = cX;
		getGameObject(iMonsterIndex)->MTY = cY;
		getGameObject(iMonsterIndex)->TX = cX;
		getGameObject(iMonsterIndex)->TY = cY;
		getGameObject(iMonsterIndex)->m_OldX = cX;
		getGameObject(iMonsterIndex)->m_OldY = cY;
		getGameObject(iMonsterIndex)->StartX = cX;
		getGameObject(iMonsterIndex)->StartY = cY;
		getGameObject(iMonsterIndex)->MapNumber = Obj.MapNumber;
		getGameObject(iMonsterIndex)->m_MoveRange = 0;
		getGameObject(iMonsterIndex)->Level = MAttr->m_Level;
		getGameObject(iMonsterIndex)->Type = OBJ_MONSTER;
		getGameObject(iMonsterIndex)->MaxRegenTime = 1000;
		getGameObject(iMonsterIndex)->Dir = 1;
		getGameObject(iMonsterIndex)->RegenTime = GetTickCount();
		getGameObject(iMonsterIndex)->m_Attribute = 0;
		getGameObject(iMonsterIndex)->DieRegen = 0;
		getGameObject(iMonsterIndex)->m_btCsNpcType = OBJ_NPC;
		getGameObject(iMonsterIndex)->m_btCsJoinSide = Obj.m_btCsJoinSide;
		getGameObject(iMonsterIndex)->m_PlayerData->lpGuild = Obj.m_PlayerData->lpGuild;
		getGameObject(iMonsterIndex)->m_btCreationState = 0;
		Obj.m_PlayerData->lpGuild->lpLifeStone = &getGameObject(iMonsterIndex);

		MsgOutput(iIndex, Lang.GetText(0,180));

		sLog->outBasic("[CastleSiege] LifeStone is created - [%s] [%s][%s] (Map:%d)(X:%d, Y:%d)",
			Obj.m_PlayerData->lpGuild->Name, Obj.AccountID, Obj.Name, Obj.MapNumber, cX, cY);

		Obj.m_btLifeStoneCount++;

	}
	else
	{
		MsgOutput(iIndex, Lang.GetText(0,181));
		return FALSE;
	}

	return TRUE;
}

int CLifeStone::DeleteLifeStone(CGameObject &Obj)
{
	if ( iIndex < 0 || iIndex > g_ConfigRead.server.GetObjectMax()-1 )
		return FALSE;

	CGameObject lpLifeStone = Obj;

	if ( lpLifeStone->m_PlayerData->lpGuild )
	{
		sLog->outBasic("[CastleSiege] LifeStone is broken - [%s]", lpLifeStone->m_PlayerData->lpGuild->Name);
		lpLifeStone->m_PlayerData->lpGuild->lpLifeStone = NULL;
		delete lpLifeStone->m_PlayerData;
	}

	return TRUE;
}

int CLifeStone::SetReSpawnUserXY(CGameObject &Obj)
{
	if ( !gObjIsConnected(iUserIndex) )
		return FALSE;

	CGameObject lpUser = &getGameObject(iUserIndex);

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

void CLifeStone::LifeStoneAct(CGameObject &Obj)
{
	if ( !gObjIsConnected(Obj.m_Index))
		return;

	CGameObject* lpObj = Obj;

	Obj.m_iCreatedActivationTime++;
	BYTE btCreationState = Obj.m_btCreationState;

	if ( Obj.m_iCreatedActivationTime < 60 )
		Obj.m_btCreationState = Obj.m_iCreatedActivationTime / 12;
	else
		Obj.m_btCreationState = 5;

	if ( btCreationState != Obj.m_btCreationState )
		gGameProtocol.GCSendObjectCreationState(Obj.m_Index);

	if ( Obj.m_btCreationState < 5 )
		return;

	if ( Obj.VPCount < 1 ) 
		return;

	int tObjNum = -1;

	for (int i=0;i<MAX_VIEWPORT;i++)
	{
		tObjNum = Obj.VpPlayer[i].number;

		if ( tObjNum >= 0 )
		{
			if ( getGameObject(tObjNum)->Type == OBJ_USER && getGameObject(tObjNum)->Live )
			{
				if ( getGameObject(tObjNum)->m_btCsJoinSide == Obj.m_btCsJoinSide )
				{
					if ( abs(Obj.Y - getGameObject(tObjNum)->Y) <= 3 &&
						 abs(Obj.X - getGameObject(tObjNum)->X) <= 3 )
					{
						BOOL bLifeChange = FALSE;
						BOOL bManaChange = FALSE;
						BOOL bBpChange = FALSE;

						/*getGameObject(tObjNum)->Life += 100.0f;
						getGameObject(tObjNum)->Mana += 100.0f;
						getGameObject(tObjNum)->BP += 100;*/

						if ( getGameObject(tObjNum)->Life < (getGameObject(tObjNum)->MaxLife + getGameObject(tObjNum)->AddLife))
						{
							getGameObject(tObjNum)->Life += ( getGameObject(tObjNum)->MaxLife + getGameObject(tObjNum)->AddLife ) / 100.0f;

							if ( getGameObject(tObjNum)->Life > (getGameObject(tObjNum)->MaxLife + getGameObject(tObjNum)->AddLife))
								getGameObject(tObjNum)->Life = getGameObject(tObjNum)->MaxLife + getGameObject(tObjNum)->AddLife;

							bLifeChange = TRUE;
						}

						if ( getGameObject(tObjNum)->Mana < (getGameObject(tObjNum)->MaxMana + getGameObject(tObjNum)->AddMana))
						{
							getGameObject(tObjNum)->Mana += ( getGameObject(tObjNum)->MaxMana + getGameObject(tObjNum)->AddMana ) / 100.0f;

							if ( getGameObject(tObjNum)->Mana > (getGameObject(tObjNum)->MaxMana + getGameObject(tObjNum)->AddMana))
								getGameObject(tObjNum)->Mana = getGameObject(tObjNum)->MaxMana + getGameObject(tObjNum)->AddMana;

							bManaChange = TRUE;
						}

						if ( getGameObject(tObjNum)->BP < (getGameObject(tObjNum)->MaxBP + getGameObject(tObjNum)->AddBP))
						{
							getGameObject(tObjNum)->BP += ( getGameObject(tObjNum)->MaxBP + getGameObject(tObjNum)->AddBP ) / 100;

							if ( getGameObject(tObjNum)->BP > (getGameObject(tObjNum)->MaxBP + getGameObject(tObjNum)->AddBP))
								getGameObject(tObjNum)->BP = getGameObject(tObjNum)->MaxBP + getGameObject(tObjNum)->AddBP;

							bBpChange = TRUE;
						}

						if (bLifeChange )
							gGameProtocol.GCReFillSend(tObjNum, getGameObject(tObjNum)->Life, 0xFF, 1, getGameObject(tObjNum)->iShield);

						if (bManaChange ||bBpChange ) 
							gGameProtocol.GCManaSend(tObjNum, getGameObject(tObjNum)->Mana, 0xFF, 0, getGameObject(tObjNum)->BP);
					}
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

