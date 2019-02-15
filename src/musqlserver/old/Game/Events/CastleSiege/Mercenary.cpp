////////////////////////////////////////////////////////////////////////////////
// Mercenary.cpp
#include "StdAfx.h"
#include "Mercenary.h"
#include "GameProtocol.h"
#include "GameMain.h"
#include "configread.h"
#include "Logging/Log.h"
#include "CastleSiege.h"
#include "CastleSiegeSync.h"

CMercenary g_CsNPC_Mercenary;

CMercenary::CMercenary()
{
	this->m_iMercenaryCount = 0;
}



CMercenary::~CMercenary()
{
	return;
}


BOOL CMercenary::CreateMercenary(CGameObject &Obj, int iMercenaryTypeIndex, BYTE cTX, BYTE cTY)
{
	CGameObject lpObj = Obj;
	int iMonsterIndex = -1;
	BYTE cX = cTX;
	BYTE cY = cTY;

	BYTE btMapAttr = MapC[Obj.MapNumber].GetAttr(cX, cY);

	if ( Obj.MapNumber != MAP_INDEX_CASTLESIEGE )
	{
		::MsgOutput(iIndex, Lang.GetText(0,170));

		return FALSE;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (g_CastleSiegeSync.GetCastleState() != CASTLESIEGE_STATE_STARTSIEGE)
		{
			::MsgOutput(iIndex, Lang.GetText(0,173));

			return FALSE;
		}

		if (iMercenaryTypeIndex == 286 || iMercenaryTypeIndex == 287)
		{
			if (Obj.m_btCsJoinSide != 1)
			{
				::MsgOutput(iIndex, Lang.GetText(0,171));

				return FALSE;
			}
		}

		if (Obj.m_PlayerData->GuildStatus != 0x80 && Obj.m_PlayerData->GuildStatus != 0x40)
		{
			::MsgOutput(iIndex, Lang.GetText(0,172));

			return FALSE;
		}

		if (this->m_iMercenaryCount > MAX_MERCENARY_COUNT)
		{
			::MsgOutput(iIndex, Lang.GetText(0,174));

			return FALSE;
		}

		iMonsterIndex = gObjAddMonster(Obj.MapNumber);

		if (iMonsterIndex >= 0)
		{
			MONSTER_ATTRIBUTE * MAttr = gMAttr.GetAttr(iMercenaryTypeIndex);

			if (MAttr == NULL)
			{
				gObjDel(iMonsterIndex);
				return FALSE;
			}

			gObjSetMonster(iMonsterIndex, iMercenaryTypeIndex);

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
			getGameObject(iMonsterIndex)->m_btCsNpcType = OBJ_MONSTER;
			getGameObject(iMonsterIndex)->m_btCsJoinSide = 1;

			MsgOutput(iIndex, Lang.GetText(0,154));

			this->m_iMercenaryCount++;

			if (Obj.m_PlayerData->lpGuild)
			{
				sLog->outBasic("[CastleSiege] Mercenary is summoned [%d] - [%d][%d] [%s][%s][%d] - (Guild : %s)",
					iMonsterIndex, iMercenaryTypeIndex, this->m_iMercenaryCount,
					Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildStatus, Obj.m_PlayerData->lpGuild->Name);
			}
			else
			{
				sLog->outBasic("[CastleSiege] Mercenary is summoned [%d] - [%d][%d] [%s][%s][%d]",
					iMonsterIndex, iMercenaryTypeIndex, this->m_iMercenaryCount,
					Obj.AccountID, Obj.Name, Obj.m_PlayerData->GuildStatus);
			}
		}
		else
		{
			MsgOutput(iIndex, Lang.GetText(0,155));
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CMercenary::DeleteMercenary(CGameObject &Obj)
{
	if ( iIndex < 0 || iIndex > g_ConfigRead.server.GetObjectMax()-1 )
	{
		return FALSE;
	}

	this->m_iMercenaryCount--;

	sLog->outBasic("[CastleSiege] Mercenary is broken [%d] - [%d]", iIndex, this->m_iMercenaryCount);

	if ( this->m_iMercenaryCount < 0 )
	{
		this->m_iMercenaryCount = 0;
	}

	return TRUE;
}



BOOL CMercenary::SearchEnemy(CGameObject &Obj)
{
	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		int iTargetNumber = -1;
		int iAttackRange = 0;

		Obj.TargetNumber = -1;

		if (Obj.Class == 286)
			iAttackRange = 5;
		else if (Obj.Class == 287)
			iAttackRange = 3;

		for (int i = 0; i < MAX_VIEWPORT; i++)
		{
			iTargetNumber = Obj.VpPlayer2[i].number;
			if (iTargetNumber >= 0)
			{
				if (getGameObject(iTargetNumber)->Type == OBJ_USER)
				{
					if (getGameObject(iTargetNumber)->Live)
					{
						if (getGameObject(iTargetNumber)->Teleport == 0)
						{
							if (getGameObject(iTargetNumber)->m_btCsJoinSide == Obj.m_btCsJoinSide)
								continue;

							int x = Obj.X - getGameObject(iTargetNumber)->X;
							int y = Obj.Y - getGameObject(iTargetNumber)->Y;
							int iDis = sqrt(static_cast<float>(x*x + y*y));
							Obj.VpPlayer2[i].dis = iDis;

							if (Obj.Dir == 1)
							{
								if (abs(x) <= 2)
								{
									int cY = Obj.Y - iAttackRange;
									if (cY <= getGameObject(iTargetNumber)->Y && Obj.Y >= getGameObject(iTargetNumber)->Y)
									{
										Obj.TargetNumber = iTargetNumber;
										return TRUE;
									}
								}
							}

							if (Obj.Dir == 3)
							{
								if (abs(y) <= 2)
								{
									int cX = Obj.X - iAttackRange;
									if (cX <= getGameObject(iTargetNumber)->X && Obj.X >= getGameObject(iTargetNumber)->X)
									{
										Obj.TargetNumber = iTargetNumber;
										return TRUE;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return FALSE;
}

void CMercenary::MercenaryAct(CGameObject &Obj)
{
	if ( ::gObjIsConnected(Obj.m_Index) == FALSE )
	{
		return;
	}

	CGameObject lpObj = Obj;

	if ( Obj.VPCount2 < 1 )
	{
		return;
	}

	if ( this->SearchEnemy(lpObj) != 0 && Obj.TargetNumber >= 0)
	{
		Obj.m_ActState.Attack = 1;
		Obj.NextActionTime = Obj.m_AttackSpeed;
	}
	else
	{
		Obj.NextActionTime = Obj.m_MoveSpeed;
	}
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

