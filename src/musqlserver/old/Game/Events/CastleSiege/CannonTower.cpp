// CannonTower.cpp: implementation of the CCannonTower class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "GameServer.h"
#include "CannonTower.h"
#include "User/CUserData.h"

//#include "..\include\readscript.h"
#include "util.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCannonTower g_CsNPC_CannonTower;
CCannonTower::CCannonTower()
{

}

CCannonTower::~CCannonTower()
{

}

void CCannonTower::CannonTowerAct(CGameObject &Obj) // 0x00560940  1.00.19
{
	if ((rand() % 2) != 0) 	// review this
	{
		return;
	}
	CGameObject lpObj = Obj;
	int count = 0;
	int ASBOfs = 0;
	PMSG_BEATTACK_COUNT pCount;
	PMSG_BEATTACK pAttack;

	pCount.h.c = 0xC1;

#if(ENABLE_SEASON12 == 1)
	pCount.h.headcode = HEADCODE_ATTACK_MULTI_TARGET;
#else
	pCount.h.headcode = 0xDB;
#endif

	pCount.h.size = 0x00;
	pCount.MagicNumberH = 0x00;
	pCount.MagicNumberL = 0x00;
	pCount.Count = 0x00;
	pCount.X = lpObj.X;
	pCount.Y = lpObj.Y;

	ASBOfs = sizeof(PMSG_BEATTACK_COUNT);

	BYTE AttackSendBuff[256];

	while (true)
	{
		if (lpObj.VpPlayer2[count].state)
		{
			if (lpObj.VpPlayer2[count].type == OBJ_USER)
			{
				int tObjNum = lpObj.VpPlayer2[count].number;

				if (tObjNum >= 0)
				{
					if (getGameObject(tObjNum)->m_btCsJoinSide)
					{
						if (gObjCalDistance(lpObj, &getGameObject(tObjNum)) < 7)
						{
							pAttack.NumberH = SET_NUMBERH(tObjNum);
							pAttack.NumberL = SET_NUMBERL(tObjNum);
							memcpy((AttackSendBuff + ASBOfs), (PBYTE)&pAttack, sizeof(PMSG_BEATTACK));
							ASBOfs += sizeof(PMSG_BEATTACK);
							pCount.Count++;
						}
					}
				}
			}
		}
		count++;

		if (count > MAX_VIEWPORT - 1) break;
	}
	if (pCount.Count > 0)
	{
		pCount.h.size = ASBOfs;

		memcpy(AttackSendBuff, (BYTE*)&pCount, sizeof(PMSG_BEATTACK_COUNT));
		gGameProtocol.CGBeattackRecv(AttackSendBuff, lpObj.m_Index, 1);
		WORD MagicNumber = 50;
		PMSG_DURATION_MAGIC_SEND pSend;
		PHeadSetBE((BYTE*)&pSend, 0x1E, sizeof(pSend));
		pSend.MagicNumberH = SET_NUMBERH(MagicNumber);
		pSend.MagicNumberL = SET_NUMBERL(MagicNumber);
		pSend.X = lpObj.X;
		pSend.Y = lpObj.Y;
		pSend.Dir = 0;
		pSend.NumberH = SET_NUMBERH(Obj.m_Index);
		pSend.NumberL = SET_NUMBERL(Obj.m_Index);

		gGameProtocol.MsgSendV2(lpObj, (PBYTE)&pSend, pSend.h.size);
	}
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

