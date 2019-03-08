// BuffEffectSlot.cpp: implementation of the CBuffEffectSlot class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "User/CUserData.h"
#include "MasterLevelSkillTreeSystem.h"
#include "BuffEffectSlot.h"
#include "BuffEffect.h"
#include "ItemAddOption.h"
#include "ObjCalCharacter.h"
#include "util.h"
#include "ItemObject.h"
#include "ProDef.h"
#include "GameProtocol.h"
#include "Main.h"
#include "NewPVP.h"
#include "GensSystem.h"
#include "ChaosCastle.h"
#include "OfflineLevelling.h"
#include "GOFunctions.h"

CBuffEffectSlot	g_BuffEffectSlot;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBuffEffectSlot::CBuffEffectSlot()
{

}

CBuffEffectSlot::~CBuffEffectSlot()
{

}

int CBuffEffectSlot::SetEffect(CGameObject &Obj, int iBuffIndex, BYTE btEffectType1, BYTE btEffectType2, int iEffectValue1, int iEffectValue2, int iDuration)
{
	LPBUFF_EFFECT_DATE lpBuffData = NULL;
	LPBUFF_EFFECT_DATE lpPrevBuffData = NULL;
	//sLog->outBasic("[K2] EFFECT VALUE %d VALUE %d btEffectType1 %d btEffectType2 %d", iEffectValue1, iEffectValue2, btEffectType1, btEffectType2);
	int BuffCount = 0;
	int DebuffCount = 0;
	int iRemovedBuffIndex = 0;


	if(g_BuffScript.CheckVaildBuffEffect(iBuffIndex) == false)	return -1;

	lpBuffData = g_BuffScript.GetBuffData(iBuffIndex);

	if(lpBuffData == NULL)	return -1;

	int i;
	for( i = 0; i < MAX_BUFFEFFECT; i++)
	{
		if( Obj.pntBuffEffectList[i]->BuffIndex != BUFFTYPE_NONE )
		{
			lpPrevBuffData = g_BuffScript.GetBuffData(Obj.pntBuffEffectList[i]->BuffIndex);

			if(lpPrevBuffData == NULL) continue;

			if(lpPrevBuffData->BuffEffectType == lpBuffData->BuffEffectType)
			{
				RemoveEffect(Obj, Obj.pntBuffEffectList[i]->BuffIndex);
				iRemovedBuffIndex = lpPrevBuffData->BuffIndex;
			}

			if(lpPrevBuffData->BuffType == BUFFEFFECT_TYPE_POSITIVE)
			{
				BuffCount++;
			}
			else if(lpPrevBuffData->BuffType == BUFFEFFECT_TYPE_NEGATIVE)
			{
				DebuffCount++;
			}

			if(i > Obj.m_BuffEffectCount) break;
		}
	}

	for( i = 0; i < MAX_BUFFEFFECT; i++)
	{
		if(Obj.pntBuffEffectList[i]->BuffIndex == BUFFTYPE_NONE)
		{
			Obj.pntBuffEffectList[i]->BuffIndex = iBuffIndex;
			Obj.pntBuffEffectList[i]->EffectCategory = lpBuffData->BuffEffectType;
			Obj.pntBuffEffectList[i]->EffectType1 = btEffectType1;
			Obj.pntBuffEffectList[i]->EffectType2 = btEffectType2;
			Obj.pntBuffEffectList[i]->EffectValue1 = iEffectValue1;
			Obj.pntBuffEffectList[i]->EffectValue2 = iEffectValue2;
			Obj.pntBuffEffectList[i]->EffectDuration = iDuration;
			Obj.pntBuffEffectList[i]->EffectSetTime = GetTickCount64();

			g_BuffEffect.SetBuffEffect(Obj,btEffectType1,iEffectValue1); //first
			g_BuffEffect.SetBuffEffect(Obj,btEffectType2,iEffectValue2); //second
			break;
		}
	}

	return iRemovedBuffIndex;
}

bool CBuffEffectSlot::RemoveEffect(CGameObject &Obj, int iBuffIndex)
{
	bool bIsRemoveEffect = false;

	if(g_BuffScript.CheckVaildBuffEffect(iBuffIndex) == false)	return false;

	for(int i = 0; i < MAX_BUFFEFFECT; i++)
	{
		if(bIsRemoveEffect == true)
		{
			if( (Obj.pntBuffEffectList[i]->BuffIndex != BUFFTYPE_NONE) && (i > 0) )
			{
				std::memcpy(&Obj.pntBuffEffectList[i-1],&Obj.pntBuffEffectList[i],sizeof(Obj.pntBuffEffectList[i])); //Change the Buff Slot to the removed one...
				memset(&Obj.pntBuffEffectList[i],0,sizeof(Obj.pntBuffEffectList[i]));
			}
		}
		
		if(Obj.pntBuffEffectList[i]->BuffIndex == iBuffIndex ) //Will Clear the Effect
		{
			g_BuffEffect.ClearBuffEffect(Obj,Obj.pntBuffEffectList[i]->EffectType1,Obj.pntBuffEffectList[i]->EffectValue1);
			g_BuffEffect.ClearBuffEffect(Obj,Obj.pntBuffEffectList[i]->EffectType2,Obj.pntBuffEffectList[i]->EffectValue2);
			memset(&Obj.pntBuffEffectList[i],0,sizeof(Obj.pntBuffEffectList[i]));
			bIsRemoveEffect = true;
		}
	}

	return bIsRemoveEffect;
}

bool CBuffEffectSlot::CheckUsedEffect(CGameObject &Obj, int iBuffIndex)
{
	if( g_BuffScript.CheckVaildBuffEffect(iBuffIndex) == false )	return false;

	for(int i = 0; i < MAX_BUFFEFFECT; i++)
	{
		if(Obj.pntBuffEffectList[i]->BuffIndex == iBuffIndex)	return true;
	}

	return false;
}

void CBuffEffectSlot::RemoveBuffVariable(CGameObject &Obj, int iBuffIndex)
{
	switch(iBuffIndex)
	{
	case BUFFTYPE_FREEZE: //56	28	255	255	Ice	1	1	1	Diminui velocidade de mobilidade
		{
			Obj.DelayActionTime = 0;
			Obj.DelayLevel = 0;
			Obj.m_SkillInfo->IceType = 0;
		}
		break;
	}
}

int CBuffEffectSlot::ClearEffect(CGameObject &Obj, enum eBuffClearType ClearType)
{
	LPBUFF_EFFECT_DATE lpBuffData = NULL;

	int iRemoveEffectCount = 0;

	for(int i = 0; i < MAX_BUFFEFFECT; i++)
	{
		if( CheckUsedEffect(Obj,Obj.pntBuffEffectList[i]->BuffIndex) == true )
		{
			lpBuffData = g_BuffScript.GetBuffData(Obj.pntBuffEffectList[i]->BuffIndex); //Get Buff Info

			if(lpBuffData == NULL)	continue;

			if(lpBuffData->ClearType == ClearType)
			{
				if( RemoveEffect(Obj,Obj.pntBuffEffectList[i]->BuffIndex) == true)
				{
					iRemoveEffectCount++;
					i--;
				}
			}

			if(ClearType == CLEAR_TYPE_LOGOUT)
			{
				if( RemoveEffect(Obj,Obj.pntBuffEffectList[i]->BuffIndex) == true)
				{
					iRemoveEffectCount++;
					i--;
				}
			}

			if(ClearType == CLEAR_TYPE_DIEREGEN)
			{
				if( RemoveEffect(Obj,Obj.pntBuffEffectList[i]->BuffIndex) == true)
				{
					iRemoveEffectCount++;
					i--;
				}
			}
		}
	}

	return iRemoveEffectCount;
}

LPBUFF_EFFECT_DATE CBuffEffectSlot::GetEffectData(int iBuffIndex)
{
	LPBUFF_EFFECT_DATE lpBuffData = NULL;

	if(g_BuffScript.CheckVaildBuffEffect(iBuffIndex) == false)	return NULL;
	
	lpBuffData = g_BuffScript.GetBuffData(iBuffIndex);

	return lpBuffData;
}

LPBUFF_EFFECT_DATE CBuffEffectSlot::GetEffectDataFromItemCode(WORD wItemCode)
{
	return g_BuffScript.GetBuffDataFromItemCode(wItemCode);
}

int CBuffEffectSlot::GetBuffClearType(int iBuffIndex)
{
	LPBUFF_EFFECT_DATE lpBuffData = NULL;

	if(g_BuffScript.CheckVaildBuffEffect(iBuffIndex) == false)	return -1;

	lpBuffData = g_BuffScript.GetBuffData(iBuffIndex);

	return lpBuffData->ClearType;
}

void gObjCheckBuffEffectList(CGameObject &Obj)
{
	if( Obj.Type == OBJ_USER || Obj.Type == OBJ_MONSTER) //Connected Users or Monsters General
	{
		for(int i = 0; i < MAX_BUFFEFFECT; i++)
		{
			if(Obj.pntBuffEffectList[i]->EffectDuration == -10)	continue;
			if(Obj.pntBuffEffectList[i]->BuffIndex == BUFFTYPE_NONE)	continue;

			switch(Obj.pntBuffEffectList[i]->BuffIndex)
			{
			case BUFFTYPE_POISON: //55	27	255	255	Poison	1	1	1	Poisoned status of condition
				{
					if((Obj.pntBuffEffectList[i]->EffectDuration % 3) == 0) //Rand type??
					{
						g_BuffEffect.SetActiveBuffEffect(Obj, Obj.pntBuffEffectList[i]->EffectType1, Obj.pntBuffEffectList[i]->EffectValue1);
						g_BuffEffect.SetActiveBuffEffect(Obj, Obj.pntBuffEffectList[i]->EffectType2, Obj.pntBuffEffectList[i]->EffectValue2);
					}
				}
				break;
			default:
				{
					g_BuffEffect.SetActiveBuffEffect(Obj, Obj.pntBuffEffectList[i]->EffectType1, Obj.pntBuffEffectList[i]->EffectValue1);
					g_BuffEffect.SetActiveBuffEffect(Obj, Obj.pntBuffEffectList[i]->EffectType2, Obj.pntBuffEffectList[i]->EffectValue2);
				}
				break;
			}
			
			Obj.pntBuffEffectList[i]->EffectDuration--;

			if(Obj.pntBuffEffectList[i]->EffectDuration <= 0)
			{
				gObjRemoveBuffEffect(Obj, Obj.pntBuffEffectList[i]->BuffIndex);
			}
		}
	}
}

bool gObjAddBuffEffect(CGameObject &Obj, int iBuffIndex)
{
	if(Obj.m_bOffLevel == true && g_OffLevel.m_General.Immortal == 1) return false;

	int iItemCode = 0;
	LPITEMEFFECT lpItemEffectInfo = NULL;
	LPBUFF_EFFECT_DATE lpBuffData = NULL;

	lpBuffData = g_BuffEffectSlot.GetEffectData(iBuffIndex);

	if(lpBuffData == NULL)	return false;

	iItemCode = (lpBuffData->ItemType << 9) + lpBuffData->ItemIndex;

	if(g_ItemAddOption.IsValidEffect(iItemCode) == false)	return false;

	lpItemEffectInfo = g_ItemAddOption.GetItemEffectData(iItemCode);

	int iRemoveBuffIndex = 0;

	iRemoveBuffIndex = g_BuffEffectSlot.SetEffect(Obj, iBuffIndex, lpItemEffectInfo->wEffectType1, lpItemEffectInfo->wEffectType2, lpItemEffectInfo->iEffectValue1, lpItemEffectInfo->iEffectValue2, lpItemEffectInfo->iEffectValidTime);

	if(iRemoveBuffIndex != -1)
	{
		if(iRemoveBuffIndex > 0)
		{
			GCUseBuffEffect(Obj, iRemoveBuffIndex, BUFFSTATE_BUFF_NEW, lpItemEffectInfo->wOptionType, 0, 0, 0);
		}
		else Obj.m_BuffEffectCount++;
		GCUseBuffEffect(Obj, iBuffIndex, BUFFSTATE_BUFF_DELETE, lpItemEffectInfo->wOptionType, lpItemEffectInfo->wEffectType1, lpItemEffectInfo->iEffectValidTime, lpItemEffectInfo->iEffectValue1);
		return true;
	}

	return false;
}

bool gObjAddBuffEffect(CGameObject &Obj, int iBuffIndex, BYTE EffectType1, int EffectValue1, BYTE EffectType2, int EffectValue2, int Duration)
{

	if(Obj.m_bOffLevel == true && g_OffLevel.m_General.Immortal == 1) return false;

	LPBUFF_EFFECT_DATE lpBuffData = NULL;

	lpBuffData = g_BuffEffectSlot.GetEffectData(iBuffIndex);

	int iRemoveBuffIndex = 0;

	iRemoveBuffIndex = g_BuffEffectSlot.SetEffect(Obj, iBuffIndex, EffectType1, EffectType2, EffectValue1, EffectValue2, Duration);

	if(iRemoveBuffIndex != -1)
	{
		if(iRemoveBuffIndex > 0)
		{
			GCUseBuffEffect(Obj, iRemoveBuffIndex, BUFFSTATE_BUFF_NEW, lpBuffData->BuffEffectType, 0, 0, 0);
		}
		else Obj.m_BuffEffectCount++;
		GCUseBuffEffect(Obj, iBuffIndex, BUFFSTATE_BUFF_DELETE, lpBuffData->BuffEffectType, EffectType1, Duration, EffectValue1);
		return true;
	}

	return false;
}

bool gObjAddBuffEffect(CGameObject &Obj, int iBuffIndex, int Duration)
{
	if(Obj.m_bOffLevel == true && g_OffLevel.m_General.Immortal == 1) return false;

	int iItemCode = 0;
	LPITEMEFFECT lpItemEffectInfo = NULL;
	LPBUFF_EFFECT_DATE lpBuffData = NULL;

	lpBuffData = g_BuffEffectSlot.GetEffectData(iBuffIndex);

	if(lpBuffData == NULL)	return false;

	iItemCode = (lpBuffData->ItemType << 9) + lpBuffData->ItemIndex;

	if(g_ItemAddOption.IsValidEffect(iItemCode) == false)	return false;

	lpItemEffectInfo = g_ItemAddOption.GetItemEffectData(iItemCode);

	int iRemoveBuffIndex = 0;

	iRemoveBuffIndex = g_BuffEffectSlot.SetEffect(Obj, iBuffIndex, lpItemEffectInfo->wEffectType1, lpItemEffectInfo->wEffectType2, lpItemEffectInfo->iEffectValue1, lpItemEffectInfo->iEffectValue2, Duration);

	if(iRemoveBuffIndex != -1)
	{
		if(iRemoveBuffIndex > 0)
		{
			GCUseBuffEffect(Obj, iRemoveBuffIndex, BUFFSTATE_BUFF_NEW, lpItemEffectInfo->wOptionType, 0, 0, 0);
		}

		else Obj.m_BuffEffectCount++;
		GCUseBuffEffect(Obj, iBuffIndex, BUFFSTATE_BUFF_DELETE, lpItemEffectInfo->wOptionType, lpItemEffectInfo->wEffectType1, Duration, lpItemEffectInfo->iEffectValue1);

		return true;
	}

	return false;
}

bool gObjAddPeriodBuffEffect(CGameObject &Obj,PeriodBuffEffectInfo *lpPeriBuff, unsigned int dwDuration)
{

	LPBUFF_EFFECT_DATE lpBuffData = NULL;

	lpBuffData = g_BuffEffectSlot.GetEffectData(lpPeriBuff->wBuffIndex);

	if(lpBuffData == NULL)	return false;

	int iRemoveBuffIndex = 0;

	iRemoveBuffIndex = g_BuffEffectSlot.SetEffect(Obj, lpPeriBuff->wBuffIndex, lpPeriBuff->btEffectType1, lpPeriBuff->btEffectType2, lpPeriBuff->wEffectValue1, lpPeriBuff->wEffectValue2, dwDuration);
       
	if(iRemoveBuffIndex != -1)
	{
		if(iRemoveBuffIndex > 0)
		{
			GCUseBuffEffect(Obj, iRemoveBuffIndex, BUFFSTATE_BUFF_NEW, lpBuffData->BuffEffectType, 0, 0, 0);
		}

		else Obj.m_BuffEffectCount++;

		gObjCalCharacter.CalcCharacter(Obj);
		GCUseBuffEffect(Obj, lpPeriBuff->wBuffIndex, BUFFSTATE_BUFF_DELETE, lpBuffData->BuffEffectType, lpPeriBuff->btEffectType1, dwDuration, lpPeriBuff->wEffectValue1);
		return true;
	}

	return false;
}

bool gObjAddBuffEffectForInGameShop(CGameObject &Obj, WORD wItemCode, int Duration)
{

	if(Duration <= 0) return false;

	LPBUFF_EFFECT_DATE lpBuffData = NULL;
	LPITEMEFFECT lpItemEffectInfo = NULL;

	lpBuffData = g_BuffEffectSlot.GetEffectDataFromItemCode(wItemCode);

	if (lpBuffData == NULL) return false;

	if (g_ItemAddOption.IsValidEffect(wItemCode) == false) return false;

	lpItemEffectInfo = g_ItemAddOption.GetItemEffectData(wItemCode);

	int iRemoveBuffIndex = g_BuffEffectSlot.SetEffect(Obj, lpBuffData->BuffIndex, lpItemEffectInfo->wEffectType1, lpItemEffectInfo->wEffectType2, lpItemEffectInfo->iEffectValue1, lpItemEffectInfo->iEffectValue2, Duration);

	if(iRemoveBuffIndex == -1)
	{
		return false;
	}

	else if(iRemoveBuffIndex > 0)
	{
		GCUseBuffEffect(Obj, iRemoveBuffIndex, BUFFSTATE_BUFF_NEW, lpItemEffectInfo->wOptionType, 0, 0, 0);
	}

	else
	{
		Obj.m_BuffEffectCount++;
		gObjCalCharacter.CalcCharacter(Obj);
	}

	GCUseBuffEffect(Obj, lpBuffData->BuffIndex, BUFFSTATE_BUFF_DELETE, lpItemEffectInfo->wOptionType, lpItemEffectInfo->wEffectType1, Duration, lpItemEffectInfo->iEffectValue1);
	return true;
}

bool gObjRemoveBuffEffect(CGameObject &Obj, int iBuffIndex)
{

	LPBUFF_EFFECT_DATE lpBuffData = NULL;

	lpBuffData = g_BuffEffectSlot.GetEffectData(iBuffIndex);

	if(lpBuffData == NULL)	return false;

	if(g_BuffEffectSlot.RemoveEffect(Obj, iBuffIndex) == true)
	{
		g_BuffEffectSlot.RemoveBuffVariable(Obj, iBuffIndex);
		Obj.m_BuffEffectCount--;
		GCUseBuffEffect(Obj, iBuffIndex, BUFFSTATE_BUFF_ADD, 0, lpBuffData->BuffEffectType, 0, 0);
		
		if ( iBuffIndex == BUFFTYPE_MONK_INCREASE_HEALTH || iBuffIndex == BUFFTYPE_MONK_INCREASE_HEALTH_STR || iBuffIndex == BUFFTYPE_BLESS ||
			iBuffIndex == BUFFTYPE_ACHERON_FIRE || iBuffIndex == BUFFTYPE_ACHERON_FROST || iBuffIndex == BUFFTYPE_ACHERON_TORNADO ||
			iBuffIndex == BUFFTYPE_ACHERON_BIND || iBuffIndex == BUFFTYPE_ACHERON_DARKNESS )
		{
			gObjCalCharacter.CalcCharacter(Obj);
		}

		return true;
	}

	return false;
}

bool gObjClearBuffEffect(CGameObject &Obj, enum eBuffClearType ClearType)
{

	int iRemoveBuffEffectCount = 0;
	int iBuffIndex = 0;
	
	switch(ClearType)
	{
	case CLEAR_TYPE_CASTLESIEGE:
		{
			for(iBuffIndex = 0; iBuffIndex < MAX_BUFFEFFECT; iBuffIndex++)
			{
				switch(Obj.pntBuffEffectList[iBuffIndex]->BuffIndex)
				{
				case BUFFTYPE_CASTLE_DEFENSE_MARK:
				case BUFFTYPE_CASTLE_ATTACK_MARK:
				case BUFFTYPE_CASTLE_ATTACK_MARK_2:
				case BUFFTYPE_CASTLE_ATTACK_MARK_3:
					{
						if(gObjRemoveBuffEffect(Obj, Obj.pntBuffEffectList[iBuffIndex]->BuffIndex) == true)
						{
							iRemoveBuffEffectCount++;
							iBuffIndex--;
						}
					}
					break;
				default:
					{
						break;
					}
					break;
				}
			}
		}
		break;
	case CLEAR_TYPE_PCBANG_SEAL_FINISH:
		{
			for(iBuffIndex = 0; iBuffIndex < MAX_BUFFEFFECT; iBuffIndex++)
			{
				switch(Obj.pntBuffEffectList[iBuffIndex]->BuffIndex)
				{
				case BUFFTYPE_PCBANG_POINT_MARK1:
				case BUFFTYPE_PCBANG_POINT_MARK2:
				case BUFFTYPE_PCBANG_POINT_MARK3:
				case BUFFTYPE_PCBANG_POINT_MARK4:
					{
						if(gObjRemoveBuffEffect(Obj, Obj.pntBuffEffectList[iBuffIndex]->BuffIndex) == true)
						{
							iRemoveBuffEffectCount++;
							iBuffIndex--;
						}
					}
					break;
				default:
					{
						break;
					}
					break;
				}
			}
		}
		break;
	case CLEAR_TYPE_NON_PCS_ITEM_EFFECT:
		{
			for(iBuffIndex = 0; iBuffIndex < MAX_BUFFEFFECT; iBuffIndex++)
			{
				switch(Obj.pntBuffEffectList[iBuffIndex]->BuffIndex)
				{
				case BUFFTYPE_PCBANG_POINT_MARK1:
				case BUFFTYPE_PCBANG_POINT_MARK2:
				case BUFFTYPE_PCBANG_POINT_MARK3:

				case BUFFTYPE_PCS_MARK1:
				case BUFFTYPE_PCS_MARK2:
				case BUFFTYPE_PCS_MARK3:
				case BUFFTYPE_PCS_MARK4:

				case BUFFTYPE_PCS_SCROLL_HASTE:
				case BUFFTYPE_PCS_SCROLL_DEFENSE:
				case BUFFTYPE_PCS_SCROLL_ANGER:
				case BUFFTYPE_PCS_SCROLL_MAGIC:
				case BUFFTYPE_PCS_SCROLL_HEALTH:
				case BUFFTYPE_PCS_SCROLL_MANA:

				case BUFFTYPE_PCS_MEDICINE_STRENGTH:
				case BUFFTYPE_PCS_MEDICINE_DEXTERITY:
				case BUFFTYPE_PCS_MEDICINE_VITALITY:
				case BUFFTYPE_PCS_MEDICINE_ENERGY:
				case BUFFTYPE_PCS_MEDICINE_LEADERSHIP:

				case BUFFTYPE_PCS_MARK5:
				case BUFFTYPE_PCS_MARK6:

				case BUFFTYPE_PCS_SCROLL_CRITICAL:
				case BUFFTYPE_PCS_SCROLL_EXCELLENT:

				case BUFFTYPE_CHARM_GUARDIAN:
				case BUFFTYPE_CHARM_PROTECTITEM:
				case BUFFTYPE_PCS_MARK1_FOR_MASTER:
				case BUFFTYPE_PCS_MARK2_FOR_MASTER:
				case BUFFTYPE_PCBANG_POINT_MARK4:

				case BUFFTYPE_PARTYITEM_EXP:
				case BUFFTYPE_PCS_AG:
				case BUFFTYPE_PCS_SD:
				case BUFFTYPE_PCS_MARK7:
				case BUFFTYPE_HACKTOOL_PENALTY:
				case BUFFTYPE_PCS_MARK8:
				case BUFFTYPE_HAWK_FIGURINE:
				case BUFFTYPE_GOAT_FIGURINE:
				case BUFFTYPE_OAK_CHARM:
				case BUFFTYPE_MAPLE_CHARM:
				case BUFFTYPE_GOLDEN_OAK_CHARM:
				case BUFFTYPE_GOLDEN_MAPLE_CHARM:
				case BUFFTYPE_WORN_HORSESHOE:

				case BUFFTYPE_ARCA_FIRETOWER:
				case BUFFTYPE_ARCA_WATERTOWER:
				case BUFFTYPE_ARCA_EARTHTOWER:
				case BUFFTYPE_ARCA_WINDTOWER:
				case BUFFTYPE_ARCA_DARKNESSTOWER:
				case BUFFTYPE_ARCA_DEATHPENALTY:

				case BUFFTYPE_ASCENSION_MARK1:
				case BUFFTYPE_ASCENSION_MARK2:
				case BUFFTYPE_ASCENSION_MARK3:
				case BUFFTYPE_ASCENSION_SEAL1:
				case BUFFTYPE_ASCENSION_SEAL2:

				case BUFFTYPE_PCS_SCROLL_DEFENSE_MASTER:
				case BUFFTYPE_PCS_SCROLL_WIZARD_MASTER:
				case BUFFTYPE_PCS_SCROLL_HEALTH_MASTER:
				case BUFFTYPE_PCS_SCROLL_MANA_MASTER:
				case BUFFTYPE_PCS_SCROLL_WRATH_MASTER:
				case BUFFTYPE_PCS_SCROLL_RECOVERY_MASTER:
				case BUFFTYPE_PCS_SCROLL_BATTLE_MASTER:
				case BUFFTYPE_PCS_SCROLL_STRENGTH_MASTER:
				case BUFFTYPE_PCS_SCROLL_QUICKNESS_MASTER:

				case BUFFTYPE_USE_MOUNT_UNIRIA:
				case BUFFTYPE_USE_MOUNT_DINORANT:
				case BUFFTYPE_USE_MOUNT_DLHORSE:
				case BUFFTYPE_USE_MOUNT_DLSPIRIT:
				case BUFFTYPE_USE_MOUNT_FENRIR:
					break;
				default:
					{
						if(gObjRemoveBuffEffect(Obj, Obj.pntBuffEffectList[iBuffIndex]->BuffIndex) == true)
						{
							iRemoveBuffEffectCount++;
							iBuffIndex--;
						}
					}
					break;
				}
			}
		}
		break;
	case CLEAR_TYPE_LOGOUT:
		{
			for(iBuffIndex = 0; iBuffIndex < MAX_BUFFEFFECT; iBuffIndex++)
			{
				if(gObjRemoveBuffEffect(Obj, Obj.pntBuffEffectList[iBuffIndex]->BuffIndex) == true)
				{
					iRemoveBuffEffectCount++;
					iBuffIndex--;
				}
			}
			Obj.m_BuffEffectCount = 0;
		}
		break;
	default:
		iRemoveBuffEffectCount = g_BuffEffectSlot.ClearEffect(Obj, ClearType);
		break;
	}

	if(Obj.m_BuffEffectCount < 0 || Obj.m_BuffEffectCount > MAX_BUFFEFFECT)	Obj.m_BuffEffectCount = 0;
	gObjSendBuffList(Obj);
	return true;
}

bool gObjCheckUsedBuffEffect(CGameObject &Obj, int iBuffIndex)
{

	return g_BuffEffectSlot.CheckUsedEffect(Obj, iBuffIndex);
}

void gObjRemoveOneDebuffEffect(CGameObject &Obj)
{
	ULONGLONG iTime = 0;
	int iIndex = -1;

	for(int i = 0; i < MAX_BUFFEFFECT && i <= Obj.m_BuffEffectCount; i++)
	{
		LPBUFF_EFFECT_DATE lpBuffData = g_BuffScript.GetBuffData(Obj.pntBuffEffectList[i]->BuffIndex);

		if ( lpBuffData && lpBuffData->BuffType == BUFFEFFECT_TYPE_NEGATIVE )
		{
			if ( iTime )
			{
				if ( iTime > Obj.pntBuffEffectList[i]->EffectSetTime )
				{
					iTime = Obj.pntBuffEffectList[i]->EffectSetTime;
					iIndex = i;
				}
			}

			else
			{
				iTime = Obj.pntBuffEffectList[i]->EffectSetTime;
				iIndex = i;
			}
		}
	}

	if ( iTime )
	{
		gObjRemoveBuffEffect(Obj, Obj.pntBuffEffectList[iIndex]->BuffIndex);
	}
}

bool gObjChangeBuffValidTime(CGameObject &Obj, int iBuffIndex, int iTime)
{

	for(int i = 0; i < MAX_BUFFEFFECT; i++)
	{
		if(i > Obj.m_BuffEffectCount)	return false;

		if(Obj.pntBuffEffectList[i]->BuffIndex == iBuffIndex)
		{
			Obj.pntBuffEffectList[i]->EffectDuration += iTime;

			if(Obj.pntBuffEffectList[i]->EffectDuration < 0)	gObjRemoveBuffEffect(Obj, iBuffIndex);
			return true;
		}
	}

	return false;
}

void gObjSetActiveEffectAtTick(CGameObject &Obj)
{
	int EffectValue = 0;

	for(int i = 0; i < MAX_BUFFEFFECT; i++)
	{
		if(Obj.pntBuffEffectList[i]->BuffIndex != BUFFTYPE_NONE)
		{
			g_BuffEffect.SetActiveBuffEffect(Obj, Obj.pntBuffEffectList[i]->EffectType1, Obj.pntBuffEffectList[i]->EffectValue1);
			g_BuffEffect.SetActiveBuffEffect(Obj, Obj.pntBuffEffectList[i]->EffectType2, Obj.pntBuffEffectList[i]->EffectValue2);
		}
	}
}

int gObjMakeViewportState(CGameObject &Obj, BYTE *lpBuffer)
{

	int iBuffCount = 0;

	for(int i = 0; i < MAX_BUFFEFFECT; i++)
	{
		if( iBuffCount >= MAX_BUFFEFFECT || iBuffCount >= Obj.m_BuffEffectCount)
		{
			break;
		}

		if(Obj.pntBuffEffectList[i]->BuffIndex == BUFFTYPE_NONE)	continue;

		switch(Obj.pntBuffEffectList[i]->BuffIndex)
		{
		case BUFFTYPE_NONE:
			break;
		default:
			lpBuffer[iBuffCount] = Obj.pntBuffEffectList[i]->BuffIndex;
			iBuffCount++;
			break;
		}
	}

	return iBuffCount;
}

bool gObjCheckPowerfulEffect(CGameObject &Obj, int iBuffIndex, int EffectValue1, int EffectValue2)
{

	for(int i = 0; i < MAX_BUFFEFFECT; i++)
	{
		if(Obj.pntBuffEffectList[i]->BuffIndex == iBuffIndex)
		{
			if(Obj.pntBuffEffectList[i]->EffectValue1 > EffectValue1)	return true;
			if(Obj.pntBuffEffectList[i]->EffectValue2 > EffectValue2)	return true;
		}
	}

	return false;
}

int gObjGetTotalValueOfEffect(CGameObject &Obj, int EffectType)
{

	int iTotalValue = 0;

	for(int i = 0; i < MAX_BUFFEFFECT; i++)
	{
		if(Obj.pntBuffEffectList[i]->BuffIndex == BUFFTYPE_NONE)	continue;

		if(Obj.pntBuffEffectList[i]->EffectType1 == EffectType)
		{
			iTotalValue += Obj.pntBuffEffectList[i]->EffectValue1;
		}

		if(Obj.pntBuffEffectList[i]->EffectType2 == EffectType)
		{
			iTotalValue += Obj.pntBuffEffectList[i]->EffectValue2;
		}
	}

	return iTotalValue;
}

bool gObjGetValueOfBuffIndex(CGameObject &Obj, int iBuffIndex, int *EffectValue1, int *EffectValue2)
{

	for(int i = 0; i < MAX_BUFFEFFECT; i++)
	{
		if(Obj.pntBuffEffectList[i]->BuffIndex == iBuffIndex)
		{
			*EffectValue1 = Obj.pntBuffEffectList[i]->EffectValue1;

			if(EffectValue2 != 0)
			{
				*EffectValue2 = Obj.pntBuffEffectList[i]->EffectValue2;
			}

			return true;
		}
	}

	return false;
}

void GCUseBuffEffect(CGameObject &Obj, BYTE BuffEffectIndex, BYTE EffectUseOption, WORD OptionType, WORD EffectType, int LeftTime, DWORD EffectValue)
{
	PMSG_USEEFFECTITEM pMsg;

	PHeadSetB((BYTE*)&pMsg, 0x2D, sizeof(PMSG_USEEFFECTITEM));
	pMsg.btBuffEffectType = BuffEffectIndex;
	pMsg.byEffectOption = EffectUseOption;
	pMsg.wOptionType = OptionType;
	pMsg.wEffectType = EffectType;
	pMsg.iLeftTime = LeftTime;
	pMsg.Unk = 0;
	pMsg.wEffectValue = EffectValue;

	if ( BuffEffectIndex == BUFFTYPE_USE_MOUNT_UNIRIA || BuffEffectIndex == BUFFTYPE_USE_MOUNT_DINORANT || 
		 BuffEffectIndex == BUFFTYPE_USE_MOUNT_DLHORSE || BuffEffectIndex == BUFFTYPE_USE_MOUNT_FENRIR
		|| BuffEffectIndex == BUFFTYPE_USE_MOUNT_DLSPIRIT)
	{
		ItemByteConvert(pMsg.ItemInfo, *Obj.pntInventory[Obj.m_btInvenPetPos]);
	}

	if ( Obj.Type == OBJ_USER )
	{
		GIOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pMsg, pMsg.h.size);
	}

	if(EffectUseOption == 0) {
		GSProtocol.GCStateInfoSend(&Obj, 1, BuffEffectIndex);
	}
	else GSProtocol.GCStateInfoSend(&Obj, 0, BuffEffectIndex);

	GSProtocol.GCSendAttackSpeed(&Obj);
	GSProtocol.GCDisplayBuffeffectPartyMember(&Obj);
}

void gObjSendBuffList(CGameObject &Obj)
{

	if(Obj.Type != OBJ_USER)	return;

	if(Obj.m_BuffEffectCount <= 0)	return;

	for(int i = 0; i < MAX_BUFFEFFECT; i++)
	{
		if(Obj.pntBuffEffectList[i]->BuffIndex != BUFFTYPE_NONE)
		{
			GSProtocol.GCStateInfoSend(&Obj, 1, Obj.pntBuffEffectList[i]->BuffIndex);
		}
	}
}

BOOL gObjAddBuffEffectWideArea(CGameObject* lpTargetObj, int nDistance, int& nAffectedCount, int iBuffIndex, BYTE EffectType1, int EffectValue1, BYTE EffectType2, int EffectValue2, int Duration)
{
	if(lpTargetObj == NULL)	return FALSE;

	for each(std::pair<int, CGameObject*> user in gGameObjects)
	{
		CGameObject* lpObj = user.second;

		if(lpObj->Type == OBJ_USER)
		{
			if(lpObj->MapNumber == lpTargetObj->MapNumber)
			{
				if(gObjCalDistance(*lpTargetObj, *lpObj) < nDistance)
				{
					gObjAddBuffEffect(*lpObj, iBuffIndex, EffectType1, EffectValue1, EffectType2, EffectValue2, Duration);
					nAffectedCount++;
				}
			}
		}
	}
	return TRUE;
}

int GetPremiumExp(int nAddExp)
{
	int nPremiumExp = 100;
	nAddExp -= 100;

	if (nAddExp < 1 || nAddExp > 50)
	{
		if (nAddExp < 51 || nAddExp > 100)
		{
			if (nAddExp < 101 || nAddExp > 150)
			{
				if (nAddExp < 151 || nAddExp > 200)
				{
					if (nAddExp < 201 || nAddExp > 250)
					{
						if (nAddExp < 251 || nAddExp > 300)
						{
							if (nAddExp < 301 || nAddExp > 350)
							{
								if (nAddExp < 351 || nAddExp > 400)
								{
									if (nAddExp < 401 || nAddExp > 450)
									{
										if (nAddExp < 451 || nAddExp > 500)
										{
											if (nAddExp < 501 || nAddExp > 550)
											{
												if (nAddExp < 551 || nAddExp > 600)
												{
													if (nAddExp < 601 || nAddExp > 650)
													{
														if (nAddExp < 651 || nAddExp > 700)
														{
															if (nAddExp >= 701)
																nPremiumExp = 250;
														}
														else
														{
															nPremiumExp = 240;
														}
													}
													else
													{
														nPremiumExp = 230;
													}
												}
												else
												{
													nPremiumExp = 220;
												}
											}
											else
											{
												nPremiumExp = 210;
											}
										}
										else
										{
											nPremiumExp = 200;
										}
									}
									else
									{
										nPremiumExp = 190;
									}
								}
								else
								{
									nPremiumExp = 180;
								}
							}
							else
							{
								nPremiumExp = 170;
							}
						}
						else
						{
							nPremiumExp = 160;
						}
					}
					else
					{
						nPremiumExp = 150;
					}
				}
				else
				{
					nPremiumExp = 140;
				}
			}
			else
			{
				nPremiumExp = 130;
			}
		}
		else
		{
			nPremiumExp = 120;
		}
	}
	else
	{
		nPremiumExp = 110;
	}

	return nPremiumExp;
}

void CheckItemOptForGetExpExRenewal(CGameObject &Obj, UINT64 &iExp, UINT64 iDefaultExp, BOOL bRewardExp, CGameObject &TargetObj /* = ObjNULL */)
{
	UINT64 iRetExp = iExp;
	int nAddExp = 100;

	if (Obj.pntInventory[8]->IsItem() == TRUE && Obj.pntInventory[8]->m_Type == ITEMGET(13, 80) && Obj.pntInventory[8]->m_Durability > 0.0)
	{
		if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_PCS_MARK3) || gObjCheckUsedBuffEffect(Obj, BUFFTYPE_PCBANG_POINT_MARK3))
		{
			iRetExp = 0;
		}

		else if (!bRewardExp)
		{
			nAddExp += 50;
		}
	}

	if ((Obj.pntInventory[11]->m_Type == ITEMGET(13,77) && Obj.pntInventory[11]->m_Durability > 0.0) ||
		(Obj.pntInventory[10]->m_Type == ITEMGET(13,77) && Obj.pntInventory[10]->m_Durability > 0.0))
	{
		if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_PCS_MARK3) || gObjCheckUsedBuffEffect(Obj, BUFFTYPE_PCBANG_POINT_MARK3))
		{
			iRetExp = 0;
		}

		else if (!bRewardExp)
		{
			nAddExp += 30;
		}
	}

	if ((Obj.pntInventory[11]->m_Type == ITEMGET(13,78) && Obj.pntInventory[11]->m_Durability > 0.0) ||
		(Obj.pntInventory[10]->m_Type == ITEMGET(13,78) && Obj.pntInventory[10]->m_Durability > 0.0))
	{
		if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_PCS_MARK3) || gObjCheckUsedBuffEffect(Obj, BUFFTYPE_PCBANG_POINT_MARK3))
		{
			iRetExp = 0;
		}

		else if (!bRewardExp)
		{
			nAddExp += 50;
		}
	}

	if (TargetObj.m_iPentagramMainAttribute > 0)
	{
		nAddExp += 15;
	}

	if (g_GensSystem.IsMapBattleZone(Obj.MapNumber) == TRUE)
	{
		nAddExp += g_GensSystem.GetBattleZoneExpBonus(Obj);
	}

	if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_PCS_MARK3) || gObjCheckUsedBuffEffect(Obj, BUFFTYPE_PCBANG_POINT_MARK3))
	{
		iRetExp = 0;
	}

	else
	{
		if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_PCS_MARK1) ||
			gObjCheckUsedBuffEffect(Obj, BUFFTYPE_PCS_MARK2) ||
			gObjCheckUsedBuffEffect(Obj, BUFFTYPE_PCBANG_POINT_MARK1) ||
			gObjCheckUsedBuffEffect(Obj, BUFFTYPE_PCBANG_POINT_MARK2) ||
			gObjCheckUsedBuffEffect(Obj, BUFFTYPE_PCS_MARK5) ||
			gObjCheckUsedBuffEffect(Obj, BUFFTYPE_PCS_MARK1_FOR_MASTER) ||
			gObjCheckUsedBuffEffect(Obj, BUFFTYPE_PCS_MARK2_FOR_MASTER) ||
			gObjCheckUsedBuffEffect(Obj, BUFFTYPE_ASCENSION_SEAL1) ||
			gObjCheckUsedBuffEffect(Obj, BUFFTYPE_ASCENSION_SEAL2))
		{
			int iEffectExp = gObjGetTotalValueOfEffect(Obj, EFFECTTYPE_EXPERIENCE);
			bool bMasterLevel = g_MasterLevelSkillTreeSystem.IsMasterLevelUser(Obj);
			if (bMasterLevel == true)
			{
				if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_PCS_MARK1_FOR_MASTER) ||
					gObjCheckUsedBuffEffect(Obj, BUFFTYPE_PCS_MARK2_FOR_MASTER) ||
					gObjCheckUsedBuffEffect(Obj, BUFFTYPE_ASCENSION_SEAL2))
				{
					if (bRewardExp)
					{
						iEffectExp = 0;
					}
				}

				else
				{
					iEffectExp = 0;
				}
			}

			else
			{
				if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_PCS_MARK1_FOR_MASTER) ||
					gObjCheckUsedBuffEffect(Obj, BUFFTYPE_PCS_MARK2_FOR_MASTER) ||
					gObjCheckUsedBuffEffect(Obj, BUFFTYPE_ASCENSION_SEAL2))
				{
					iEffectExp = 0;
				}

				else
				{
					if (bRewardExp)
					{
						iEffectExp = 0;
					}
				}
			}

			nAddExp += iEffectExp;
		}
	}

	if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_LIGHT_BLESSING) || 
		gObjCheckUsedBuffEffect(Obj, BUFFTYPE_LIGHT_BLESSING_LOW) ||
		gObjCheckUsedBuffEffect(Obj, BUFFTYPE_LIGHT_BLESSING_MIDDLE))
	{
		int iEffectExp = gObjGetTotalValueOfEffect(Obj, EFFECTTYPE_LIGHT_BLESS_EXP);

		if (bRewardExp)
		{
			iEffectExp = 0;
		}

		nAddExp += iEffectExp;
	}

	if (gObjCheckUsedBuffEffect(Obj, 179) || gObjCheckUsedBuffEffect(Obj, 180) || gObjCheckUsedBuffEffect(Obj, 181) || gObjCheckUsedBuffEffect(Obj, 182) || gObjCheckUsedBuffEffect(Obj, 183))
	{
		nAddExp += 20;
	}

	if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_ASCENSION_MARK1) || gObjCheckUsedBuffEffect(Obj, BUFFTYPE_ASCENSION_MARK2) || gObjCheckUsedBuffEffect(Obj, BUFFTYPE_ASCENSION_MARK3))
	{
		int iEffectExp = gObjGetTotalValueOfEffect(Obj, EFFECTTYPE_ASCENSION_MARK_EXP);

		if (bRewardExp)
		{
			iEffectExp = 0;
		}

		nAddExp += iEffectExp;
	}

	if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_GLORYOFGLADIATOR))
	{
		int iEffectExp = 0;

		if (g_MasterLevelSkillTreeSystem.IsMasterLevelUser(Obj))
		{
			iEffectExp = 0;
		}

		else if (g_NewPVP.IsVulcanusMap(Obj.MapNumber) == TRUE)
		{
			iEffectExp = 20;
		}

		nAddExp += iEffectExp;
	}

	if (Obj.pntInventory[8]->IsItem() == TRUE && Obj.pntInventory[8]->m_Type == ITEMGET(13, 123) && Obj.pntInventory[8]->m_Durability > 0.0)
	{
		if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_PCS_MARK3) || gObjCheckUsedBuffEffect(Obj, BUFFTYPE_PCBANG_POINT_MARK3))
		{
			iRetExp = 0;
		}

		else if (!bRewardExp)
		{
			nAddExp += 30;
		}
	}

	if ((Obj.pntInventory[11]->m_Type == ITEMGET(13, 166) && Obj.pntInventory[11]->m_Durability > 0.0) ||
		(Obj.pntInventory[10]->m_Type == ITEMGET(13, 166) && Obj.pntInventory[10]->m_Durability > 0.0))
	{
		if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_PCS_MARK3) || gObjCheckUsedBuffEffect(Obj, BUFFTYPE_PCBANG_POINT_MARK3))
		{
			iRetExp = 0;
		}

		else if (!bRewardExp)
		{
			nAddExp += 20;
		}
	}

	if (((Obj.pntInventory[8]->IsItem() == TRUE && Obj.pntInventory[8]->m_Type == ITEMGET(13, 123) && Obj.pntInventory[8]->m_Durability > 0.0) ||
		(Obj.pntInventory[8]->IsItem() == TRUE && Obj.pntInventory[8]->m_Type == ITEMGET(13, 80) && Obj.pntInventory[8]->m_Durability > 0.0)) &&
		((Obj.pntInventory[11]->m_Type == ITEMGET(13,122) && Obj.pntInventory[11]->m_Durability > 0.0) ||
		(Obj.pntInventory[10]->m_Type == ITEMGET(13,122) && Obj.pntInventory[10]->m_Durability > 0.0) ||
		(Obj.pntInventory[11]->m_Type == ITEMGET(13,76) && Obj.pntInventory[11]->m_Durability > 0.0) ||
		(Obj.pntInventory[10]->m_Type == ITEMGET(13,76) && Obj.pntInventory[10]->m_Durability > 0.0) ||
		(Obj.pntInventory[11]->m_Type == ITEMGET(13,163) && Obj.pntInventory[11]->m_Durability > 0.0) ||
		(Obj.pntInventory[10]->m_Type == ITEMGET(13,163) && Obj.pntInventory[10]->m_Durability > 0.0) ||
		(Obj.pntInventory[11]->m_Type == ITEMGET(13,164) && Obj.pntInventory[11]->m_Durability > 0.0) ||
		(Obj.pntInventory[10]->m_Type == ITEMGET(13,164) && Obj.pntInventory[10]->m_Durability > 0.0) ||
		(Obj.pntInventory[11]->m_Type == ITEMGET(13,165) && Obj.pntInventory[11]->m_Durability > 0.0) ||
		(Obj.pntInventory[10]->m_Type == ITEMGET(13,165) && Obj.pntInventory[10]->m_Durability > 0.0) ||
		(Obj.pntInventory[11]->m_Type == ITEMGET(13,77) && Obj.pntInventory[11]->m_Durability > 0.0) ||
		(Obj.pntInventory[10]->m_Type == ITEMGET(13,77) && Obj.pntInventory[10]->m_Durability > 0.0) ||
		(Obj.pntInventory[11]->m_Type == ITEMGET(13,78) && Obj.pntInventory[11]->m_Durability > 0.0) ||
		(Obj.pntInventory[10]->m_Type == ITEMGET(13,78) && Obj.pntInventory[10]->m_Durability > 0.0) ||
		(Obj.pntInventory[11]->m_Type == ITEMGET(13,166) && Obj.pntInventory[11]->m_Durability > 0.0) ||
		(Obj.pntInventory[10]->m_Type == ITEMGET(13,166) && Obj.pntInventory[10]->m_Durability > 0.0)))
	{
		if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_PCS_MARK3) || gObjCheckUsedBuffEffect(Obj, BUFFTYPE_PCBANG_POINT_MARK3))
		{
			iRetExp = 0;
		}

		else
		{
			int iEffectExp = 0;

			if (bRewardExp)
			{
				iEffectExp = 0;
			}

			else
			{
				if ((Obj.pntInventory[11]->m_Type == ITEMGET(13,77) && Obj.pntInventory[11]->m_Durability > 0.0) ||
					(Obj.pntInventory[10]->m_Type == ITEMGET(13,77) && Obj.pntInventory[10]->m_Durability > 0.0) ||
					(Obj.pntInventory[11]->m_Type == ITEMGET(13,78) && Obj.pntInventory[10]->m_Durability > 0.0) ||
					(Obj.pntInventory[10]->m_Type == ITEMGET(13,78) && Obj.pntInventory[10]->m_Durability > 0.0))
				{
					iEffectExp = 65;
				}

				else
				{
					iEffectExp = 50;
				}
			}

			nAddExp += iEffectExp;
		}
	}

	if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_PARTYITEM_EXP))
	{
		int iEffectExp = 30;
		int iEffectValue = 0;
		gObjGetValueOfBuffIndex(Obj, BUFFTYPE_PARTYITEM_EXP, &iEffectValue, 0);

		if (Obj.PartyNumber >= 0)
		{
			for (int i = 0; i < MAX_USER_IN_PARTY; i++)
			{
				CGameObject* lpPartyObj = getGameObject(gParty.m_PartyS[Obj.PartyNumber].Number[i]);
				int iPartyDis = gObjCalDistance(Obj, *lpPartyObj);

				if (Obj.m_Index != lpPartyObj->m_Index)
				{
					if (Obj.MapNumber == lpPartyObj->MapNumber)
					{
						if (iPartyDis < 10)
						{
							iEffectExp += iEffectValue;
						}
					}
				}
			}
		}

		nAddExp += iEffectExp;
	}

	int iPremiumExp = GetPremiumExp(nAddExp);

	if (Obj.m_btInvenPetPos != 0 && Obj.pntInventory[Obj.m_btInvenPetPos]->IsItem() == TRUE && 
		Obj.pntInventory[Obj.m_btInvenPetPos]->m_Type == ITEMGET(13, 37) && Obj.pntInventory[Obj.m_btInvenPetPos]->m_Durability > 0.0)
	{
		if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_PCS_MARK3) || gObjCheckUsedBuffEffect(Obj, BUFFTYPE_PCBANG_POINT_MARK3))
		{
			iRetExp = 0;
		}

		else if (!bRewardExp)
		{
			if (Obj.pntInventory[Obj.m_btInvenPetPos]->IsFenrirAddExp() > 0)
			{
				nAddExp += Obj.pntInventory[Obj.m_btInvenPetPos]->IsFenrirAddExp();
			}
		}
	}

	if (CC_MAP_RANGE(Obj.MapNumber) == TRUE)
	{
		if (gObjCheckUsedBuffEffect(Obj, BUFFTYPE_PCS_MARK3) || gObjCheckUsedBuffEffect(Obj, BUFFTYPE_PCBANG_POINT_MARK3))
		{
			iRetExp = 0;
		}

		else if (!bRewardExp)
		{
			nAddExp += g_ChaosCastle.GetExperienceBonus(Obj.m_cChaosCastleIndex);
		}
	}

	iExp = iRetExp * nAddExp / 100.0 * iPremiumExp / 100.0;
}

LPPERIOD_BUFF_EFFECT_INFO  CBuffEffectSlot::GetPeriodBuffInfo(WORD wBuffIndex)
{
	return g_BuffScript.GetPeriodBuffInfo( wBuffIndex);
}

