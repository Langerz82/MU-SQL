////////////////////////////////////////////////////////////////////////////////
// user.cpp
// ------------------------------
// Decompiled by Deathway
// Date : 2007-05-09
// -------------------------------
// GS-N 0.99.60T 0x004B1200
/*	GS-N	1.00.18	JPN	0x004CF4C0	-	Completed
	void gObjSpriteDamage(CGameObject &lpObj, int damage)	-	Wierd Compilation - Status OK
	void gObjTradeCancel(CGameObject &lpObj)	-	Wierd Compilation - Status OK
	BOOL gObjJoominCheck(CGameObject &lpObj, char* szInJN)	-	strcmpi comflict symbols
	void SkillFrustrum()	-	Wrong Symbol NAmes of the zzmath functions
*/

#include "StdAfx.h"
#include "CUserData.h"

#include "AcheronGuardianEvent.h"
#include "AntiSpeed.h"
#include "Authorization.h"
#include "BattleSoccer.h"
#include "BattleSoccerManager.h"
#include "BonusEvent.h"
#include "BotSystem.h"
#include "BuffEffect.h"
#include "BuffEffectSlot.h"
#include "CannonTower.h"
#include "CashShop.h"
#include "CastleCrown.h"
#include "CastleCrownSwitch.h"
#include "CastleSiegeSync.h"
#include "ChaosBox.h"
#include "ChaosCastle.h"
#include "ChaosCastleFinal.h"// directions
#include "CItemDrop.h"
#include "ComboAttack.h"
#include "configread.h"
#include "Crywolf.h"
#include "CrywolfAltar.h"
#include "CrywolfStatue.h"
#include "CrywolfSync.h"
#include "CustomMaxStats.h"
#include "DarkSpirit.h"
#include "DevilSquare.h"
#include "DevilSquareFinal.h"
#include "DoppelGanger.h"
#include "EledoradoEvent.h"
#include "EvolutionMonsterInfo.h"
#include "EvolutionMonsterMng.h"
#include "ExpManager.h"
#include "GameProtocol.h"
#include "GameServer.h"
#include "Gate.h"
#include "generalStructs.h"
#include "GensSystem.h"
#include "GMMng.h"
#include "gObjMonster.h"
#include "GremoryCase.h"
#include "Guardian.h"
#include "GuardianStatue.h"
#include "GuildClass.h"
#include "IllusionTempleEvent_Renewal.h"
#include "ImperialGuardian.h"
#include "IOCP.h"
#include "ItemManagement/Item.h"
#include "ItemOptionTypeMng.h"
#include "itemsocketoptiondefine.h"
#include "ItemSocketOptionSystem.h"

#include "JewelOfHarmonySystem.h"
#include "KalimaGate.h"
#include "Kanturu.h"
#include "KanturuBattleUserMng.h"
#include "LargeRand.h"
#include "LastManStanding.h"
#include "LifeStone.h"
#include "Lua/MuLua.h"
#include "LuckyItemManager.h"
#include "MagicInf.h"
#include "Main.h"
#include "MapAttribute.h"
#include "MapServerManager.h"
#include "MasterLevelSkillTreeSystem.h"
#include "MasterSkillSystem.h"
#include "Mercenary.h"
#include "MineSystem.h"
#include "MonsterAttr.h"
#include "MoveCommand.h"
#include "MultiAttackHAckCheck.h"
#include "MuRummyInfo.h"
#include "MuRummyMng.h"
#include "MuunInfo.h"
#include "MuunSystem.h"
#include "NewPVP.h"
#include "ObjAttack.h"
#include "ObjCalCharacter.h"
#include "ObjUseSkill.h"
#include "OfflineLevelling.h"
#include "PacketChecksum.h"
#include "PentagramMixSystem.h"
#include "PentagramSystem.h"
#include "PeriodItemEx.h"
#include "PersonalStore.h"
#include "QuestExpInfo.h"
#include "QuestExpProgMng.h"
#include "QuestExpUserInfo.h"
#include "QuestInfo.h"
#include "Raklion.h"
#include "RaklionBattleUserMng.h"
#include "RaklionMonsterMng.h"
#include "RaklionUtil.h"
#include "SendNPCInfo.h"
#include "SetItemOption.h"
#include "Shop.h"
#include "SkillAdditionInfo.h"
#include "SkillDelay.h"
#include "SocketItemType.h"
#include "SProtocol.h"
#include "StatMng.h"
#include "TDurMagicKeyChecker.h"
#include "TMonsterAIAgro.h"
#include "TMonsterAIElement.h"
#include "TMonsterSkillElementInfo.h"
#include "TMonsterSkillManager.h"
#include "TNotice.h"
#include "TUnion.h"
#include "UnityBattleField.h"
#include "UserChatBlockMng.hpp"
#include "UserDieItemDrop.h"
#include "util.h"
#include "VipSys.h"
#include "Weapon.h"
#include "zzzmathlib.h"


CNameConvert::CNameConvert()
{
	this->m_perCharacterInfo = new CharacterNameOfUBF[5];
}

CNameConvert::~CNameConvert()
{
	delete this->m_perCharacterInfo;
}

void CNameConvert::InitData()
{
	for (int i = 0; i < 5; i++)
	{
		this->m_perCharacterInfo[i].Clear();
	}
}

void CNameConvert::InputData(int cnt, const char *name, const char *realNameOfUbf, WORD serverCode, int Length)
{
	if (cnt > -1 && cnt < 5)
	{
		memcpy(this->m_perCharacterInfo[cnt].szCharacterName, name, Length);
		this->m_perCharacterInfo[cnt].szCharacterName[Length] = 0;
		memcpy(this->m_perCharacterInfo[cnt].szCharacterNameOfUBF, realNameOfUbf, Length);
		this->m_perCharacterInfo[cnt].szCharacterNameOfUBF[Length] = 0;
		this->m_perCharacterInfo[cnt].nServerCodeOfHomeWorld = serverCode;
	}
}

char * CNameConvert::ConvertNameToRealName(const char *name)
{
	for (int i = 0; i < 5; i++)
	{
		if (!strcmp(this->m_perCharacterInfo[i].szCharacterName, name))
		{
			return this->m_perCharacterInfo[i].szCharacterNameOfUBF;
		}
	}

	return this->m_perCharacterInfo[0].szCharacterNameOfUBF;
}

char * CNameConvert::ConvertRealNameToName(const char *realName)
{
	for (int i = 0; i < 5; i++)
	{
		if (!strcmp(this->m_perCharacterInfo[i].szCharacterNameOfUBF, realName))
		{
			return this->m_perCharacterInfo[i].szCharacterName;
		}
	}

	return this->m_perCharacterInfo[0].szCharacterName;
}

WORD CNameConvert::FindServerCodeAtRealName(const char *realName)
{
	for (int i = 0; i < 5; i++)
	{
		if (!strcmp(this->m_perCharacterInfo[i].szCharacterNameOfUBF, realName))
		{
			return this->m_perCharacterInfo[i].nServerCodeOfHomeWorld;
		}
	}

	return 0;
}

WORD CNameConvert::FindServerCodeAtCharacterName(const char *name)
{
	for (int i = 0; i < 5; i++)
	{
		if (!strcmp(this->m_perCharacterInfo[i].szCharacterName, name))
		{
			return this->m_perCharacterInfo[i].nServerCodeOfHomeWorld;
		}
	}

	return 0;
}

char * CNameConvert::ConvertSlotIndexToName(int slotindex)
{
	int SlotNumber = slotindex;

	if (slotindex > 2 && slotindex > -1)
	{
		SlotNumber = 0;
		sLog->outBasic("[UBF][CNAMECONVERT][ConvertSlotIndexToName] Map Join Request Fail, Wrong SlotIndex :%d. Default (ChName,RealName): (%s,%s)",
			slotindex, this->m_perCharacterInfo[0].szCharacterName, this->m_perCharacterInfo[0].szCharacterNameOfUBF);
	}

	return this->m_perCharacterInfo[SlotNumber].szCharacterName;
}

int CNameConvert::IsDuplicationOfName()
{
	for (int pivotIndex = 0; pivotIndex < 5; pivotIndex++)
	{
		for (int searchIndex = 0; searchIndex < 5; searchIndex++)
		{
			if (pivotIndex != searchIndex)
			{
				if (!strcmp(this->m_perCharacterInfo[pivotIndex].szCharacterNameOfUBF, this->m_perCharacterInfo[searchIndex].szCharacterNameOfUBF))
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}


CUserData::CUserData(int IDNumber)
{
	this->IDNumber;
	InitializeCriticalSection(&this->m_MasterSkillCriti);
	InitializeCriticalSection(&this->m_DarkSideRFCriti);
	InitializeCriticalSection(&this->AgiCheckCriti);
	this->m_pCMuRummyInfo = new CMuRummyInfo;
	this->m_pCEvoMonInfo = new CEvolutionMonsterInfo;
	this->pPentagramMixBox = new CItem[CHAOS_BOX_SIZE];
	this->pPentagramMixBoxMap = new BYTE[CHAOS_BOX_MAP_SIZE];
	this->m_SeedOptionList = new SOCKET_OPTION_LIST[35];
	this->m_BonusOptionList = new SOCKET_OPTION_LIST[7];
	this->m_SetOptionList = new SOCKET_OPTION_LIST[2];
	this->m_CancelItemSaleList = new STR_USER_SHOP_REBUY_ITEM[MAX_CANCEL_ITEMS_SALE];
	this->m_GremoryCaseData = new GREMORYCASE_ITEM_DATA[MAX_GREMORYCASE_STORAGE_TYPES * MAX_GREMORYCASE_STORAGE_ITEMS];
	this->m_StatSpecOption = new STAT_USER_OPTION[12];

	this->Init();
}

CUserData::~CUserData()
{
	DeleteCriticalSection(&this->m_MasterSkillCriti);
	DeleteCriticalSection(&this->m_DarkSideRFCriti);
	DeleteCriticalSection(&this->AgiCheckCriti);

	if (this->m_pCMuRummyInfo != NULL)
	{
		delete this->m_pCMuRummyInfo;
		this->m_pCMuRummyInfo = NULL;
	}

	if (this->m_pCEvoMonInfo != NULL)
	{
		delete this->m_pCEvoMonInfo;
		this->m_pCEvoMonInfo = NULL;
	}

	if (this->pPentagramMixBox != NULL)
	{
		delete[] this->pPentagramMixBox;
		this->pPentagramMixBox = NULL;
	}

	if (this->pPentagramMixBoxMap != NULL)
	{
		delete[] this->pPentagramMixBoxMap;
		this->pPentagramMixBoxMap = NULL;
	}

	delete[] this->m_SeedOptionList;
	delete[] this->m_BonusOptionList;
	delete[] this->m_SetOptionList;
	delete[] this->m_CancelItemSaleList;
	delete[] this->m_GremoryCaseData;
	delete[] this->m_StatSpecOption;
}

void CUserData::Init(bool VipReset)
{
	memset(this->m_RealNameOfUBF, 0, sizeof(this->m_RealNameOfUBF));
	memset(this->GuildName, 0, sizeof(this->GuildName));
	this->GuildNumber = 0;
	this->iGuildUnionTimeStamp = 0;
	this->lpGuild = NULL;
	this->GuildStatus = -1;
	this->SantaCount = 0;
	this->RegisterdLMS = 0;
	this->RegisteredLMSRoom = -1;
	this->MoveGate = -1;
	g_kJewelOfHarmonySystem.InitEffectValue(this->m_JewelOfHarmonyEffect);
	g_kItemSystemFor380.InitEffectValue(this->m_ItemOptionExFor380);
	this->m_iResets = 0;

	this->LastAuthTime = 0;
	if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
	{
		this->m_bKanturuEntranceByNPC = FALSE;
	}

	this->SetOpAddMaxAttackDamage = 0;
	this->SetOpAddMinAttackDamage = 0;
	this->SetOpAddDamage = 0;
	this->SetOpIncAGValue = 0;
	this->SetOpAddCriticalDamageSuccessRate = 0;
	this->SetOpAddCriticalDamage = 0;
	this->SetOpAddExDamageSuccessRate = 0;
	this->SetOpAddExDamage = 0;
	this->SetOpAddSkillAttack = 0;
	this->SetOpAddAttackDamage = 0;
	this->SetOpAddDefence = 0;
	this->SetOpAddMagicPower = 0;
	this->SetOpAddDefenceRate = 0;
	this->SetOpIgnoreDefense = 0;
	this->SetOpDoubleDamage = 0;
	this->SetOpAddMinMagicDamage = 0;
	this->SetOpAddMaxMagicDamage = 0;
	this->SetOpTwoHandSwordImproveDamage = 0;
	this->SetOpImproveSuccessAttackRate = 0;
	this->SetOpReflectionDamage = 0;
	this->SetOpImproveSheldDefence = 0;
	this->SetOpDecreaseAG = 0;
	this->SetOpImproveItemDropRate = 0;
	this->IsFullSetItem = false;
	this->m_ReqWarehouseOpen = 0;
	this->m_WCoinC = 0.0;
	this->m_WCoinP = 0.0;
	this->m_GoblinPoint = 0.0;
	this->m_iGensInfoLoad = false;
	this->m_iQuestInfoLoad = false;
	this->m_iPeriodItemEffectIndex = -1;
	this->SkillHellFire2State = 0;
	this->SkillHellFire2Count = 0;
	this->SkillStrengthenHellFire2State = 0;
	this->SkillStrengthenHellFire2Count = 0;
	this->PacketsPerSecond = 0;
	this->SwearWarning = 0;
	this->PlusStatQuestClear = false;
	this->ComboSkillquestClear = false;
	memset(this->m_Quest, (BYTE)-1, sizeof(this->m_Quest));
	this->m_SendQuestInfo = 0;
	this->BlessSkillCast = false;
	this->RFSkillCount = 0;
	this->DarkSideCount = 0;
	this->m_btSculptPos = 0;
	this->m_btCharmPos = 0;
	this->m_btArtifactPos = 0;
	this->m_btExpUpCharmPos = 0;
	this->MasterLevel = 0;
	this->MasterExperience = 0;
	this->MasterNextExp = 0;
	this->MasterPoint = 0;
	this->m_GensInfluence = NONE_INFLUENCE;
	this->m_GensClass = 0;
	this->m_GensRanking = 0;
	this->m_GensAbusingTick = 0;
	this->m_ContributePoint = 0;
	this->m_NextContributePoint = 0;
	this->PentagramInfoLoad = 0;
	this->AgilityCheckDelay = GetTickCount();
	this->AgilityCheckCount = 0;
	this->ISBOT = false;
	this->wBotIndex = -1;
	for (int i = 0; i < 5; i++)
	{
		this->DarkSideTarget[i] = 10000;
	}

	for (int i = 0; i < 254; i++)
	{
		this->m_PentagramJewelInfo_Inven[i].Clear();
	}

	for (int i = 0; i < 254; i++)
	{
		this->m_PentagramJewelInfo_Warehouse[i].Clear();
	}

	this->m_bUserQuestInfoSent = FALSE;

	for (int i = 0; i < MAX_QUESTEXP_USER_INFO; i++)
	{
		this->m_UserQuestInfo[i].Clear();
	}

	this->PotionTime = 0;
	this->m_dwMSBFlagAttackDisable = GetTickCount();
	this->m_bUsePolymorphSkin = false;
	this->LastSkillUseCount = 0;
	this->LastSkillUseNumber = 0;
	this->LastSkillUseTick = GetTickCount64();
	this->PentagramMixMoney = 0;
	this->PentagramMixSuccessRate = 0;
	this->m_PentagramMixLock = FALSE;
	this->m_bIsPentagramMixCompleted = false;
	this->m_bUsePartyMatching = false;
	this->m_bPartyMatchingLeader = false;
	this->m_nServerChannel = -1;
	this->m_nPartyMatchingIndex = -1;
	this->m_nPartyMatchingMemberCount = 0;
	this->m_bUseGuildMatching = false;
	this->m_bSendNpcPos = FALSE;
	this->m_bSendMemberPos = FALSE;
	this->m_Resistance_Perfect = 0;
	this->m_Resistance_SD = 0;
	this->m_Resistance_Stun = 0;
	this->m_Resistance_Double = 0;
	this->m_Resistance_Critical = 0;
	this->m_Resistance_Excellent = 0;
	this->m_EnableUseChangeSkin = 0;
	this->AntiHackCheckTime = GetTickCount();
	this->m_dwMapMoveKeyValue = 0;
	this->m_bIsCancelItemSale = false;
	this->m_bGremoryCaseDBInfo = false;
	this->m_JoinUnityBattle = false;
	this->m_nServerCodeOfHomeWorld = -1;
	this->SkillDelay.Init();
	this->m_dwGensHitTeleportDelay = 0;
	this->bActiveSetParty = false;
	this->dwOpenWareCnt = 0;
	this->dwCloseWareCnt = 0;
	this->bHasSameSerialItem = false;
	this->Ruud = 0;
	memset(this->EventEntryCount, 0x00, sizeof(this->EventEntryCount));

	this->dwHitHackCounter1 = 0;
	this->dwHitHackCounter2 = 0;

	memset(this->dwLastHitHackValue1, 0, sizeof(this->dwLastHitHackValue1));
	memset(this->dwLastHitHackValue2, 0, sizeof(this->dwLastHitHackValue2));

	this->dwLastHitHackTick = GetTickCount64();

	memset(this->PartyPassword, 0x00, sizeof(this->PartyPassword));

	for (int i = 0; i < MAX_CANCEL_ITEMS_SALE; i++)
	{
		this->m_CancelItemSaleList[i].Clear();
	}

	for (int i = 0; i < MAX_GREMORYCASE_STORAGE_TYPES; i++)
	{
		for (int j = 0; j < MAX_GREMORYCASE_STORAGE_ITEMS; j++)
		{
			this->m_GremoryCaseData[i * MAX_GREMORYCASE_STORAGE_TYPES + j].Clear();
		}
	}

	if (VipReset == true)
	{
		this->VipType = 0;
		this->VipEffect = VIP_EFFECT_NONE;
	}

	this->MovingDistance = 0;
	this->MovingIgnore = 0;
	this->RageDMG = 0;

	this->m_AttackQueue = new CAttackQueue(this);
}
