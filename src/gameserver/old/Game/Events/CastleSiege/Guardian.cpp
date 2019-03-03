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
		if (!gObjIsConnected(Obj.m_Index))
			return;

		CGameObject* lpObj = Obj;

		if (Obj.VPCount < 1)
			return;

		int tObjNum = -1;

		for (int i = 0; i<MAX_VIEWPORT; i++)
		{
			tObjNum = Obj.VpPlayer[i].number;

			if (tObjNum >= 0)
			{
				if (getGameObject(tObjNum)->Type == OBJ_USER && getGameObject(tObjNum)->Live)
				{
					if (getGameObject(tObjNum)->m_btCsJoinSide == Obj.m_btCsJoinSide)
					{
						if (abs(Obj.Y - getGameObject(tObjNum)->Y) <= 3 &&
							abs(Obj.X - getGameObject(tObjNum)->X) <= 3)
						{
							getGameObject(tObjNum)->Life += 100.0f;
							getGameObject(tObjNum)->Mana += 100.0f;
							getGameObject(tObjNum)->BP += 100;

							if (getGameObject(tObjNum)->Life > getGameObject(tObjNum)->MaxLife)
								getGameObject(tObjNum)->Life = getGameObject(tObjNum)->MaxLife;

							if (getGameObject(tObjNum)->Mana > getGameObject(tObjNum)->MaxMana)
								getGameObject(tObjNum)->Mana = getGameObject(tObjNum)->MaxMana;

							if (getGameObject(tObjNum)->BP > getGameObject(tObjNum)->MaxBP)
								getGameObject(tObjNum)->BP = getGameObject(tObjNum)->MaxBP;

							GSProtocol.GCReFillSend(tObjNum, getGameObject(tObjNum)->Life, 0xFF, 1, getGameObject(tObjNum)->iShield);
							GSProtocol.GCManaSend(tObjNum, getGameObject(tObjNum)->Mana, 0xFF, 0, getGameObject(tObjNum)->BP);
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

