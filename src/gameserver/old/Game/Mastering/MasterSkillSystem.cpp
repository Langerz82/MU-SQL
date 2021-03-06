////////////////////////////////////////////////////////////////////////////////
// MasterSkillSystem.cpp
#include "MasterSkillSystem.h"
#include "GameProtocol.h"
#include "User/CUserData.h"
#include "Main.h"
#include "ObjCalCharacter.h"
#include "VipSys.h"
#include "CustomMaxStats.h"


CMasterLevelSystem::CMasterLevelSystem()
{
	this->gMasterExperience = NULL;
}
CMasterLevelSystem::~CMasterLevelSystem()
{
	delete [] this->gMasterExperience;
}

bool CMasterLevelSystem::MasterLevelUp(CGameObject &Obj, UINT64 addexp, int iMonsterType, const char * szEventType)
{
	if(Obj.Type != OBJ_USER)
	{
		return false;
	}

	if(Obj.m_PlayerData->ChangeUP != 2)
	{
		return false;
	}

	if (Obj.m_PlayerData->MasterLevel >= g_MaxStatsInfo.GetClass.MLUserMaxLevel)
	{
		Obj.m_PlayerData->MasterExperience = this->gMasterExperience[Obj.m_PlayerData->MasterLevel];
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0,45), Obj.m_Index, 1);
		return false;
	}
	
	gObjSetExpPetItem(Obj.m_Index, addexp);

	if ( Obj.m_PlayerData->MasterExperience < Obj.m_PlayerData->MasterNextExp )
	{
		return true;
	}

	Obj.m_PlayerData->MasterLevel++;

	if ( g_ConfigRead.data.reset.iBlockMLPointAfterResets == -1 || Obj.m_PlayerData->m_iResets < g_ConfigRead.data.reset.iBlockMLPointAfterResets )
	{
		Obj.m_PlayerData->MasterPoint += g_MaxStatsInfo.GetClass.MLPointPerLevel;
	}

	Obj.m_PlayerData->MasterExperience = Obj.m_PlayerData->MasterNextExp;

	gObjCalCharacter.CalcCharacter(Obj.m_Index);

	Obj.MaxLife += DCInfo.DefClass[ Obj.Class ].LevelLife;
	Obj.MaxMana += DCInfo.DefClass[ Obj.Class ].LevelMana;
	Obj.Life = Obj.MaxLife;
	Obj.Mana = Obj.MaxMana;

	Obj.Life = Obj.MaxLife + Obj.AddLife;
	Obj.Mana = Obj.MaxMana + Obj.AddMana;
	
	gObjCalCharacter.CalcShieldPoint(lpObj);
	Obj.iShield = Obj.iMaxShield + Obj.iAddShield;
	GSProtocol.GCReFillSend(Obj.m_Index,Obj.Life,0xFF,0,Obj.iShield);
	Obj.m_PlayerData->MasterNextExp = this->gObjNextMLExpCal(lpObj);
	gObjSetBP(Obj.m_Index);
	gObjCalcMaxLifePower(Obj.m_Index);
	GSProtocol.GCMasterLevelUpMsgSend(Obj.m_Index);

	return false;

}
UINT64 CMasterLevelSystem::gObjNextMLExpCal(CGameObject &Obj)
{
	if(Obj.Type != OBJ_USER)
		return 0;

	return this->gMasterExperience[Obj.m_PlayerData->MasterLevel+1];
}

bool CMasterLevelSystem::IsMasterLevelUser(CGameObject &Obj)
{
	if(Obj.Type != OBJ_USER)
		return false;

	if(Obj.m_PlayerData->ChangeUP == 2 && Obj.Level >= g_ConfigRead.data.common.UserMaxLevel 
		&& Obj.m_PlayerData->m_iResets >= g_MaxStatsInfo.GetClass.m_MinReset)
		return true;

	return false;
}

void CMasterLevelSystem::SetExpTable()
{
	if ( this->gMasterExperience != NULL )
	{
		delete [] this->gMasterExperience;
	}

	this->gMasterExperience = new UINT64[g_MaxStatsInfo.GetClass.MLUserMaxLevel + 1];

	if ( this->gMasterExperience == NULL )
	{
		sLog->outError("error - memory allocation failed");
		return;
	}

	this->gMasterExperience[0] = 0;
	MULua * TempLua = new MULua(false);
	TempLua->DoFile(g_ConfigRead.GetPath("\\Scripts\\Misc\\ExpCalc.lua"));
	double exp = 0.0;

	for (int n = 1; n <= g_MaxStatsInfo.GetClass.MLUserMaxLevel; n++)
	{
		TempLua->Generic_Call("SetExpTable_Master", "ii>d", n, g_ConfigRead.data.common.UserMaxLevel, &exp);
		this->gMasterExperience[n] = exp;
	}

	delete TempLua;

}

void CMasterLevelSystem::SendMLData(CGameObject &Obj)
{
	if ( Obj.Type != OBJ_USER )
	{
		return;
	}

	PMSG_MASTER_INFO_SEND pMsg;

	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x50, sizeof(pMsg));

	pMsg.MasterLevel = Obj.m_PlayerData->MasterLevel;
	pMsg.MLExpHHH = SET_NUMBERH(SET_NUMBERHW(HIDWORD(Obj.m_PlayerData->MasterExperience)));
	pMsg.MLExpHHL = SET_NUMBERL(SET_NUMBERHW(HIDWORD(Obj.m_PlayerData->MasterExperience)));
	pMsg.MLExpHLH = SET_NUMBERH(SET_NUMBERLW(HIDWORD(Obj.m_PlayerData->MasterExperience)));
	pMsg.MLExpHLL = SET_NUMBERL(SET_NUMBERLW(HIDWORD(Obj.m_PlayerData->MasterExperience)));
	pMsg.MLExpLHH = SET_NUMBERH(SET_NUMBERHW(LODWORD(Obj.m_PlayerData->MasterExperience)));
	pMsg.MLExpLHL = SET_NUMBERL(SET_NUMBERHW(LODWORD(Obj.m_PlayerData->MasterExperience)));
	pMsg.MLExpLLH = SET_NUMBERH(SET_NUMBERLW(LODWORD(Obj.m_PlayerData->MasterExperience)));
	pMsg.MLExpLLL = SET_NUMBERL(SET_NUMBERLW(LODWORD(Obj.m_PlayerData->MasterExperience)));
	pMsg.MLNextExpHHH = SET_NUMBERH(SET_NUMBERHW(HIDWORD(Obj.m_PlayerData->MasterNextExp)));
	pMsg.MLNextExpHHL = SET_NUMBERL(SET_NUMBERHW(HIDWORD(Obj.m_PlayerData->MasterNextExp)));
	pMsg.MLNextExpHLH = SET_NUMBERH(SET_NUMBERLW(HIDWORD(Obj.m_PlayerData->MasterNextExp)));
	pMsg.MLNextExpHLL = SET_NUMBERL(SET_NUMBERLW(HIDWORD(Obj.m_PlayerData->MasterNextExp)));
	pMsg.MLNextExpLHH = SET_NUMBERH(SET_NUMBERHW(LODWORD(Obj.m_PlayerData->MasterNextExp)));
	pMsg.MLNextExpLHL = SET_NUMBERL(SET_NUMBERHW(LODWORD(Obj.m_PlayerData->MasterNextExp)));
	pMsg.MLNextExpLLH = SET_NUMBERH(SET_NUMBERLW(LODWORD(Obj.m_PlayerData->MasterNextExp)));
	pMsg.MLNextExpLLL = SET_NUMBERL(SET_NUMBERLW(LODWORD(Obj.m_PlayerData->MasterNextExp)));
	pMsg.MasterPoint = Obj.m_PlayerData->MasterPoint;
	pMsg.MaxLife = Obj.MaxLife+Obj.AddLife;
	pMsg.MaxMana = Obj.MaxMana+Obj.AddMana;
	pMsg.MaxShield = Obj.iMaxShield+Obj.iAddShield;
	pMsg.MaxBP = Obj.MaxBP+Obj.AddBP;

	IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
}

void CMasterLevelSystem::InitData(CGameObject &Obj)
{
	if ( Obj.Type != OBJ_USER )
	{
		return;
	}

	Obj.m_PlayerData->MasterLevel = 0;
	Obj.m_PlayerData->MasterPoint = 0;
	Obj.m_PlayerData->MasterExperience = 0;
	Obj.m_PlayerData->MasterNextExp = this->gObjNextMLExpCal(lpObj);

	this->SendMLData(lpObj);
}

int CMasterLevelSystem::GetDieDecExpRate(CGameObject &Obj)
{
	if(this->IsMasterLevelUser(lpObj) == FALSE)
	{
		return -1;
	}

	int DecRate = 0;

	if(Obj.m_PK_Level <= 3)
	{
		DecRate = 7;
	}
	else if(Obj.m_PK_Level == 4)
	{
		DecRate = 20;
	}
	else if(Obj.m_PK_Level == 5)
	{
		DecRate = 30;
	}
	else if(Obj.m_PK_Level >= 6)
	{
		DecRate = 40;
	}

	return DecRate;
}

int CMasterLevelSystem::GetDieDecMoneyRate(CGameObject &Obj)
{
	if(this->IsMasterLevelUser(lpObj) == FALSE)
	{
		return -1;
	}

	return 4;
}

bool CMasterLevelSystem::CheckMLGetExp(CGameObject &Obj, CGameObject lpTargetObj)
{
	int iMonsterMinLevel = g_ConfigRead.data.common.MLMonsterMinLevel;

	if (Obj.Type != OBJ_USER)
	{
		return FALSE;
	}

	if (Obj.m_PlayerData->VipType != 0)
	{
		iMonsterMinLevel = g_VipSystem.GetMLMonsterMinLevel(lpObj);
	}

	if (this->IsMasterLevelUser(lpObj))
	{
		if (lpTargetObj.Level < iMonsterMinLevel)	return FALSE;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

