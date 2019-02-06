////////////////////////////////////////////////////////////////////////////////
// CustomMichi.cpp
#include "StdAfx.h"
#include "CustomMichi.h"
#include "TLog.h"
#include "MuRummyMng.h"
#include "JewelMixSystem.h"
#include "ChangeCmd.h"
#include "UpgradeCmd.h"
#include "MasterLevelSkillTreeSystem.h"
#include "readscript.h"
#include "winutil.h"

CConfigMichi g_Michi;

CConfigMichi::CConfigMichi()
{
}

CConfigMichi::~CConfigMichi()
{
}

void CConfigMichi::LoadPotionHack()
{
	CIniReader PotionHackConfig(g_ConfigRead.GetPath(FILE_CUSTOM_HACKPOTION));
	
	// AUTO POTION  AND  POTION RATES
	this->FixHackPotions.m_CheckHealingAutoPotionHack = PotionHackConfig.ReadDword("HealingPotionHackCheck", "AutoPotionHack", 0);
	this->FixHackPotions.m_CheckAutoHealingPotionHackTolerance = PotionHackConfig.ReadDword("HealingPotionHackCheck", "AutoPotionHackTolerance", 250);
	this->FixHackPotions.m_LifePotionMultiplyCount = PotionHackConfig.ReadDword("HealingPotionHackCheck", "LifePotionMultiplyCount", 1);
	this->FixHackPotions.m_LifePotionMultiplyRate = PotionHackConfig.ReadDword("HealingPotionHackCheck", "LifePotionMultiplyRate", 100);
	this->FixHackPotions.Potion0 = PotionHackConfig.ReadDword("HealingPotionHackCheck", "LifePotionRate1", 10);
	this->FixHackPotions.Potion1 = PotionHackConfig.ReadDword("HealingPotionHackCheck", "LifePotionRate2", 20);
	this->FixHackPotions.Potion2 = PotionHackConfig.ReadDword("HealingPotionHackCheck", "LifePotionRate3", 30);
	this->FixHackPotions.Potion3 = PotionHackConfig.ReadDword("HealingPotionHackCheck", "LifePotionRate4", 40);

	g_Log.AddC(TColor::Red, "Healing: AutoPotionHack:(On: %s), AutoPotionHackTolerance: %d ", this->FixHackPotions.m_CheckHealingAutoPotionHack == true ? "Yes" : "No", this->FixHackPotions.m_CheckAutoHealingPotionHackTolerance);

}

void CConfigMichi::GCFireworksSend(LPOBJ lpObj, int x, int y) // OK
{
	PMSG_SERVERCMD ServerCmd;

	PHeadSubSetB((LPBYTE)&ServerCmd, 0xF3, 0x40, sizeof(ServerCmd));
	ServerCmd.CmdType = 0;
	ServerCmd.X = x;
	ServerCmd.Y = y;

	IOCP.DataSend(lpObj->m_Index, (BYTE*)&ServerCmd, ServerCmd.h.size);
	GSProtocol.MsgSendV2(lpObj, (BYTE*)&ServerCmd, ServerCmd.h.size);
}


void CConfigMichi::UseHealingPotion(CItem * citem, int pos, int aIndex)
{
	if (this->FixHackPotions.m_CheckHealingAutoPotionHack == 1)
	{
		if (GetTickCount() - gObj[aIndex].m_PlayerData->PotionTime < this->FixHackPotions.m_CheckAutoHealingPotionHackTolerance)
		{
			GSProtocol.GCReFillSend(aIndex, gObj[aIndex].Life, 0xFD, 1, gObj[aIndex].iShield);
			return;
		}
	}
	
	gObj[aIndex].m_PlayerData->PotionTime = GetTickCount();

	int tLife = (citem->m_Value * 10) - (gObj[aIndex].Level * 2);

	if (tLife <  0)
	{
		tLife = 0;
	}

	int nAddRate = 0;

	switch (citem->m_Type)
	{
	case ITEMGET(14, 0):
		nAddRate = this->FixHackPotions.Potion0;
		break;
	case ITEMGET(14, 1):
		nAddRate = this->FixHackPotions.Potion1;
		break;
	case ITEMGET(14, 2):
		nAddRate = this->FixHackPotions.Potion2;
		break;
	case ITEMGET(14, 3):
		nAddRate = this->FixHackPotions.Potion3;
		break;
	}

	if (citem->m_Level == 1)
	{
		nAddRate += 5;
	}

	tLife += ((int)gObj[aIndex].MaxLife * nAddRate) / 100;

	if (citem->m_Type == ITEMGET(14, 0))
	{
		if (citem->m_Level < 2)
		{
			gObj[aIndex].FillLife += tLife;
			tLife = 0;
		}
	}

	if (gObj[aIndex].FillLife > 0.0f)
	{
		gObj[aIndex].Life += gObj[aIndex].FillLife;

		if (gObj[aIndex].Life > (gObj[aIndex].MaxLife + gObj[aIndex].AddLife))
		{
			gObj[aIndex].Life = gObj[aIndex].MaxLife + gObj[aIndex].AddLife;
			gObj[aIndex].FillLife = 0;
		}

		GSProtocol.GCReFillSend(gObj[aIndex].m_Index, gObj[aIndex].Life, 0xFF, FALSE, gObj[aIndex].iShield);
	}

	gObj[aIndex].FillLifeMax = tLife;
	gObj[aIndex].FillLife = tLife;

	if (citem->m_Type == ITEMGET(14, 0) && citem->m_Level < 2)	//ok
	{
		gObj[aIndex].FillLifeCount = 0;
	}
	else if (citem->m_Level == 1)
	{
		gObj[aIndex].FillLifeCount = 2;
	}
	else
	{
		gObj[aIndex].FillLifeCount = 3;
	}

	if (!gObjSearchItemMinus(&gObj[aIndex], pos, 1))
	{
		gObjInventoryItemSet(aIndex, pos, -1);
		gObj[aIndex].pInventory[pos].Clear();
		GSProtocol.GCInventoryItemDeleteSend(aIndex, pos, 1);
	}
}

bool CConfigMichi::IsEventItem(int ItemIndex) // OK
{

	if (ItemIndex == ITEMGET(13, 210) || ItemIndex == ITEMGET(13, 220) || ItemIndex == ITEMGET(13, 221) || ItemIndex == ITEMGET(13, 222) || ItemIndex == ITEMGET(14, 215) || ItemIndex == ITEMGET(14, 216))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


bool CConfigMichi::Ex_IsBadFileLine(char *FileLine, int *Flag)
{
	if (*Flag == 0)
	{
		if (isdigit(FileLine[0]))
		{
			*Flag = FileLine[0] - 48;
			return true;
		}
	}
	else if (*Flag < 0 || *Flag > 9)
	{
		*Flag = 0;
	}

	if (!strncmp(FileLine, "end", 3))
	{
		*Flag = 0;
		return true;
	}

	if (FileLine[0] == '/' || FileLine[0] == '\n')
		return true;

	for (UINT i = 0; i < strlen(FileLine); i++)
	{
		if (isalnum(FileLine[i]))
			return false;
	}
	return true;
}

int CConfigMichi::ExcOpt(int amount)
{
	if (amount > 6) amount = 6;
	if (amount < 1) amount = 1;

	srand(time(NULL));

	amount = rand() % amount + 1;

	int opt_db[6] = { 1, 2, 4, 8, 16, 32 };
	int exc = 0;

	int opt_done[6] = { 0 };

	for (int n = 0; n < amount; n++)
	{
		int SearchExc = 0;

		srand(time(NULL));

		if (n == 0)
		{
			SearchExc = opt_db[rand() % 6];
			opt_done[n] = SearchExc;
		}
		else
		{
			while (true)
			{
				SearchExc = opt_db[rand() % 6];
				bool bOpt = true;

				for (int i = 0; i<amount; i++)
				{
					if (SearchExc == opt_done[i])
					{
						bOpt = false;
					}
				}

				if (bOpt == true)
				{
					opt_done[n] = SearchExc;
					break;
				}
			}
		}

		exc += SearchExc;
	}

	return exc;
}

int CConfigMichi::GetNumberByPercent(int Proc, int Min, int Max)
{
	int Random = rand() % 100;

	if (Proc == 0 || Max == Min)
	{
		return Min;
	}

	if (Random <= Proc)
	{
		return Max;
	}
	if (Proc > 50)
	{
		return GetNumberByPercent(Proc / 2, Min, Max - 1);
	}
	else if (Proc < 30)
	{
		return GetNumberByPercent(Proc * 3 / 2, Min, Max - 1);
	}
	else
	{
		return GetNumberByPercent(Proc, Min, Max - 1);
	}
}


BYTE CConfigMichi::IsNewOptionRand(int level)
{
	BYTE NOption = 0;
	NOption = 1 << (rand() % 6);

	if ((NOption & 2) != 0)
	{
		if ((rand() % 2) != 0)
		{
			NOption = 1 << (rand() % 6);
		}
	}

	if ((rand() % 4) == 0)
	{
		NOption |= 1 << (rand() % 6);
	}

	return NOption;
}


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

