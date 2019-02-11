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


BOOL CMercenary::CreateMercenary(int iIndex, int iMercenaryTypeIndex, BYTE cTX, BYTE cTY)
{
	CGameObject lpObj = &gGameObjects[iIndex];
	int iMonsterIndex = -1;
	BYTE cX = cTX;
	BYTE cY = cTY;

	BYTE btMapAttr = MapC[lpObj.MapNumber].GetAttr(cX, cY);

	if ( lpObj.MapNumber != MAP_INDEX_CASTLESIEGE )
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
			if (lpObj.m_btCsJoinSide != 1)
			{
				::MsgOutput(iIndex, Lang.GetText(0,171));

				return FALSE;
			}
		}

		if (lpObj.m_PlayerData->GuildStatus != 0x80 && lpObj.m_PlayerData->GuildStatus != 0x40)
		{
			::MsgOutput(iIndex, Lang.GetText(0,172));

			return FALSE;
		}

		if (this->m_iMercenaryCount > MAX_MERCENARY_COUNT)
		{
			::MsgOutput(iIndex, Lang.GetText(0,174));

			return FALSE;
		}

		iMonsterIndex = gObjAddMonster(lpObj.MapNumber);

		if (iMonsterIndex >= 0)
		{
			MONSTER_ATTRIBUTE * MAttr = gMAttr.GetAttr(iMercenaryTypeIndex);

			if (MAttr == NULL)
			{
				gObjDel(iMonsterIndex);
				return FALSE;
			}

			gObjSetMonster(iMonsterIndex, iMercenaryTypeIndex);

			gGameObjects[iMonsterIndex].Live = TRUE;
			gGameObjects[iMonsterIndex].Life = MAttr->m_Hp;
			gGameObjects[iMonsterIndex].MaxLife = MAttr->m_Hp;
			gGameObjects[iMonsterIndex].m_PosNum = -1;
			gGameObjects[iMonsterIndex].X = cX;
			gGameObjects[iMonsterIndex].Y = cY;
			gGameObjects[iMonsterIndex].MTX = cX;
			gGameObjects[iMonsterIndex].MTY = cY;
			gGameObjects[iMonsterIndex].TX = cX;
			gGameObjects[iMonsterIndex].TY = cY;
			gGameObjects[iMonsterIndex].m_OldX = cX;
			gGameObjects[iMonsterIndex].m_OldY = cY;
			gGameObjects[iMonsterIndex].StartX = cX;
			gGameObjects[iMonsterIndex].StartY = cY;
			gGameObjects[iMonsterIndex].MapNumber = lpObj.MapNumber;
			gGameObjects[iMonsterIndex].m_MoveRange = 0;
			gGameObjects[iMonsterIndex].Level = MAttr->m_Level;
			gGameObjects[iMonsterIndex].Type = OBJ_MONSTER;
			gGameObjects[iMonsterIndex].MaxRegenTime = 1000;
			gGameObjects[iMonsterIndex].Dir = 1;
			gGameObjects[iMonsterIndex].RegenTime = GetTickCount();
			gGameObjects[iMonsterIndex].m_Attribute = 0;
			gGameObjects[iMonsterIndex].DieRegen = 0;
			gGameObjects[iMonsterIndex].m_btCsNpcType = OBJ_MONSTER;
			gGameObjects[iMonsterIndex].m_btCsJoinSide = 1;

			MsgOutput(iIndex, Lang.GetText(0,154));

			this->m_iMercenaryCount++;

			if (lpObj.m_PlayerData->lpGuild)
			{
				sLog->outBasic("[CastleSiege] Mercenary is summoned [%d] - [%d][%d] [%s][%s][%d] - (Guild : %s)",
					iMonsterIndex, iMercenaryTypeIndex, this->m_iMercenaryCount,
					lpObj.AccountID, lpObj.Name, lpObj.m_PlayerData->GuildStatus, lpObj.m_PlayerData->lpGuild->Name);
			}
			else
			{
				sLog->outBasic("[CastleSiege] Mercenary is summoned [%d] - [%d][%d] [%s][%s][%d]",
					iMonsterIndex, iMercenaryTypeIndex, this->m_iMercenaryCount,
					lpObj.AccountID, lpObj.Name, lpObj.m_PlayerData->GuildStatus);
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

BOOL CMercenary::DeleteMercenary(int iIndex)
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



BOOL CMercenary::SearchEnemy(CGameObject &lpObj)
{
	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		int iTargetNumber = -1;
		int iAttackRange = 0;

		lpObj.TargetNumber = -1;

		if (lpObj.Class == 286)
			iAttackRange = 5;
		else if (lpObj.Class == 287)
			iAttackRange = 3;

		for (int i = 0; i < MAX_VIEWPORT; i++)
		{
			iTargetNumber = lpObj.VpPlayer2[i].number;
			if (iTargetNumber >= 0)
			{
				if (gGameObjects[iTargetNumber].Type == OBJ_USER)
				{
					if (gGameObjects[iTargetNumber].Live)
					{
						if (gGameObjects[iTargetNumber].Teleport == 0)
						{
							if (gGameObjects[iTargetNumber].m_btCsJoinSide == lpObj.m_btCsJoinSide)
								continue;

							int x = lpObj.X - gGameObjects[iTargetNumber].X;
							int y = lpObj.Y - gGameObjects[iTargetNumber].Y;
							int iDis = sqrt(static_cast<float>(x*x + y*y));
							lpObj.VpPlayer2[i].dis = iDis;

							if (lpObj.Dir == 1)
							{
								if (abs(x) <= 2)
								{
									int cY = lpObj.Y - iAttackRange;
									if (cY <= gGameObjects[iTargetNumber].Y && lpObj.Y >= gGameObjects[iTargetNumber].Y)
									{
										lpObj.TargetNumber = iTargetNumber;
										return TRUE;
									}
								}
							}

							if (lpObj.Dir == 3)
							{
								if (abs(y) <= 2)
								{
									int cX = lpObj.X - iAttackRange;
									if (cX <= gGameObjects[iTargetNumber].X && lpObj.X >= gGameObjects[iTargetNumber].X)
									{
										lpObj.TargetNumber = iTargetNumber;
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

void CMercenary::MercenaryAct(int iIndex)
{
	if ( ::gObjIsConnected(iIndex) == FALSE )
	{
		return;
	}

	CGameObject lpObj = &gGameObjects[iIndex];

	if ( lpObj.VPCount2 < 1 )
	{
		return;
	}

	if ( this->SearchEnemy(lpObj) != 0 && lpObj.TargetNumber >= 0)
	{
		lpObj.m_ActState.Attack = 1;
		lpObj.NextActionTime = lpObj.m_AttackSpeed;
	}
	else
	{
		lpObj.NextActionTime = lpObj.m_MoveSpeed;
	}
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

