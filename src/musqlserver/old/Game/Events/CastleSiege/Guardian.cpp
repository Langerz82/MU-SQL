////////////////////////////////////////////////////////////////////////////////
// Guardian.cpp
#include "StdAfx.h"
#include "Guardian.h"
#include "User/CUserData.h"
#include "configread.h"

// GS_N 0.99.60T 0x00519F60
//	GS-N	1.00.18	JPN	0x0054B8D0	-	Completed

CGuardian g_CsNPC_Guardian;

CGuardian::CGuardian()
{
	return;
}

CGuardian::~CGuardian()
{
	return;
}

BOOL CGuardian::CreateGuardian(CGameObject &Obj)
{	
	MsgOutput(iIndex, Lang.GetText(0,175));
	return true;
}

void CGuardian::GuardianAct(CGameObject &Obj)
{
	if (g_ConfigRead.server.GetServerType() == SERVER_CASTLE)
	{
		if (!gObjIsConnected(iIndex))
			return;

		CGameObject lpObj = Obj;

		if (lpObj.VPCount < 1)
			return;

		int tObjNum = -1;

		for (int i = 0; i<MAX_VIEWPORT; i++)
		{
			tObjNum = lpObj.VpPlayer[i].number;

			if (tObjNum >= 0)
			{
				if (gGameObjects[tObjNum]->Type == OBJ_USER && gGameObjects[tObjNum]->Live)
				{
					if (gGameObjects[tObjNum]->m_btCsJoinSide == lpObj.m_btCsJoinSide)
					{
						if (abs(lpObj.Y - gGameObjects[tObjNum]->Y) <= 3 &&
							abs(lpObj.X - gGameObjects[tObjNum]->X) <= 3)
						{
							gGameObjects[tObjNum]->Life += 100.0f;
							gGameObjects[tObjNum]->Mana += 100.0f;
							gGameObjects[tObjNum]->BP += 100;

							if (gGameObjects[tObjNum]->Life > gGameObjects[tObjNum]->MaxLife)
								gGameObjects[tObjNum]->Life = gGameObjects[tObjNum]->MaxLife;

							if (gGameObjects[tObjNum]->Mana > gGameObjects[tObjNum]->MaxMana)
								gGameObjects[tObjNum]->Mana = gGameObjects[tObjNum]->MaxMana;

							if (gGameObjects[tObjNum]->BP > gGameObjects[tObjNum]->MaxBP)
								gGameObjects[tObjNum]->BP = gGameObjects[tObjNum]->MaxBP;

							gGameProtocol.GCReFillSend(tObjNum, gGameObjects[tObjNum]->Life, 0xFF, 1, gGameObjects[tObjNum]->iShield);
							gGameProtocol.GCManaSend(tObjNum, gGameObjects[tObjNum]->Mana, 0xFF, 0, gGameObjects[tObjNum]->BP);
						}
					}
				}
			}
		}
	}
}



////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

