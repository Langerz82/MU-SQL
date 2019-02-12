////////////////////////////////////////////////////////////////////////////////
// EvolutionMonsterMng.cpp
#include "StdAfx.h"
#include "EvolutionMonsterMng.h"
#include "GameProtocol.h"
#include "util.h"
#include "BloodCastle.h"
#include "DevilSquare.h"
#include "ChaosCastle.h"
#include "KalimaGate.h"
#include "MonsterAttr.h"
#include "GameMain.h"
#include "BuffEffectSlot.h"
#include "GremoryCase.h"
#include "TNotice.h"
#include "gObjMonster.h"
#include "EvolutionMonsterMng.h"

CEvolutionMonsterMng g_EvoMonMng;

CEvolutionMonsterMng::CEvolutionMonsterMng(void)
{
	this->Init();
}

CEvolutionMonsterMng::~CEvolutionMonsterMng(void)
{
}

void CEvolutionMonsterMng::Init()
{
	this->m_bEnable = false;
	this->m_iSummonScrollDropLevel = 0;
	this->m_iSummonScrollDropRate = 0;

	this->m_mapEvoMonData.clear();
	this->m_vtSpecialEvoMon.clear();
	this->m_vtRewardRange.clear();
}

void CEvolutionMonsterMng::LoadFile(char *szFile)
{
	this->m_bLoadFile = false;
	this->Init();

	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(szFile);

	if (res.status != pugi::status_ok)
	{
		sLog->outError("Error - cannot load %s file (%s)", szFile, res.description());
		return;
	}

	pugi::xml_node mainXML = file.child("EvolutionMonster");

	this->m_bEnable = mainXML.attribute("Enable").as_bool();
	this->m_iEvoMonTimeLimit = mainXML.attribute("TimeLimit").as_int();

	pugi::xml_node summon_scroll = mainXML.child("SummonScroll");

	this->m_iSummonScrollDropRate = summon_scroll.attribute("DropRate").as_int();
	this->m_iSummonScrollDropLevel = summon_scroll.attribute("DropLevel").as_int();

	pugi::xml_node evo_data = mainXML.child("EvolutionData");

	for (pugi::xml_node evolution = evo_data.child("Evolution"); evolution; evolution = evolution.next_sibling())
	{
		EVOMON_MONSTER_DATA m_Data;

		m_Data.m_wLevel = evolution.attribute("Level").as_int();
		m_Data.m_fHp = evolution.attribute("Hp").as_float();
		m_Data.m_fDamage = evolution.attribute("Damage").as_float();
		m_Data.m_fDefense = evolution.attribute("Defense").as_float();
		m_Data.m_fAtkRate = evolution.attribute("AtkRate").as_float();
		m_Data.m_fDefRate = evolution.attribute("DefRate").as_float();
		m_Data.m_iSuccessRate = evolution.attribute("Rate").as_int();

		this->m_mapEvoMonData.insert(std::pair<int, EVOMON_MONSTER_DATA>(m_Data.m_wLevel, m_Data));
	}

	pugi::xml_node reward_range = mainXML.child("RewardRange");

	for (pugi::xml_node reward = reward_range.child("Reward"); reward; reward = reward.next_sibling())
	{
		EVOMON_REWARD_RANGE m_Reward;

		m_Reward.m_wMinLevel = reward.attribute("EvoLevelMin").as_int();
		m_Reward.m_wMaxLevel = reward.attribute("EvoLevelMax").as_int();
		m_Reward.m_btRewardType = reward.attribute("Type").as_int();

		this->m_vtRewardRange.push_back(m_Reward);
	}

	pugi::xml_node special_evomon = mainXML.child("SpecialEvolution");

	this->m_btSpecialEvoMonRewardType = special_evomon.attribute("RewardType").as_int();

	for (pugi::xml_node special = special_evomon.child("Special"); special; special = special.next_sibling())
	{
		EVOMON_SPECIAL_MONSTER m_SpecialMon;

		m_SpecialMon.m_wMinLevel = special.attribute("EvoLevelMin").as_int();
		m_SpecialMon.m_wMaxLevel = special.attribute("EvoLevelMax").as_int();
		m_SpecialMon.m_iSuccessRate = special.attribute("Rate").as_int();

		this->m_vtSpecialEvoMon.push_back(m_SpecialMon);
	}

	this->m_bLoadFile = true;
}

bool CEvolutionMonsterMng::UseSummonScroll(CGameObject &lpObj)
{
	PMSG_EVOMON_SUMMON_RESULT pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0x3E, 0x03, sizeof(pMsg));

	if (this->IsEvoMonEnable() == false)
	{
		return false;
	}

	if (ObjectMaxRange(aIndex) == false)
	{
		return false;
	}

	if (!gObjIsConnected(aIndex))
	{
		return false;
	}

	

	if (!lpObj)
	{
		return false;
	}

	if (lpObj.CloseType != -1)
	{
		return false;
	}

	CEvolutionMonsterInfo * pEvoMonInfo = lpObj.m_PlayerData->m_pCEvoMonInfo;

	if (!pEvoMonInfo)
	{
		return false;
	}

	int iEvoMonIndex = pEvoMonInfo->GetEvoMonIndex();

	if (pEvoMonInfo->IsEvoMonActive() == true && ObjectMaxRange(iEvoMonIndex) && gGameObjects[iEvoMonIndex]->Live == TRUE)
	{
		if (gGameObjects[iEvoMonIndex]->Live == TRUE)
		{
			pMsg.btResult = 1;
			pMsg.btMapNumber = gGameObjects[iEvoMonIndex]->MapNumber;
			pMsg.btX = gGameObjects[iEvoMonIndex]->X;
			pMsg.btY = gGameObjects[iEvoMonIndex]->Y;
			pMsg.iTimeLeft = (gGameObjects[iEvoMonIndex]->m_Disappear_Monster + (this->m_iEvoMonTimeLimit * 60 * 1000) - GetTickCount()) / 60 / 1000;

			IOCP.DataSend(lpObj.m_Index, (LPBYTE)&pMsg, pMsg.h.size);
			return false;
		}

		else
		{
			pMsg.btResult = 2;

			IOCP.DataSend(lpObj.m_Index, (LPBYTE)&pMsg, pMsg.h.size);
			return false;
		}
	}

	int nPartyNumber = lpObj.PartyNumber;

	if (nPartyNumber != -1)
	{
		for (int i = 0; i < MAX_USER_IN_PARTY; i++)
		{
			int iPartyIndex = gParty.m_PartyS[nPartyNumber].Number[i];

			if (iPartyIndex != -1 && gGameObjects[iPartyIndex]->Type == OBJ_USER)
			{
				CEvolutionMonsterInfo * pPartyEvoMonInfo = gGameObjects[iPartyIndex]->m_PlayerData->m_pCEvoMonInfo;

				if (pPartyEvoMonInfo == NULL)
				{
					continue;
				}

				if (pEvoMonInfo->IsEvoMonActive() == true && ObjectMaxRange(iEvoMonIndex))
				{
					pMsg.btResult = 3;

					IOCP.DataSend(lpObj.m_Index, (LPBYTE)&pMsg, pMsg.h.size);
					return false;
				}
			}
		}
	}

	if (this->IsEventMap(lpObj.MapNumber, lpObj.X, lpObj.Y) == true)
	{
		
		pMsg.btResult = 4;

		IOCP.DataSend(lpObj.m_Index, (LPBYTE)&pMsg, pMsg.h.size);
		return false;
	}

	pEvoMonInfo->Clear();
	int iEvoMon = this->SummonEvoMon(lpObj);

	if (iEvoMon == -1)
	{
		MsgOutput(aIndex, "Internal Error - cannot summon EvoMon");
		return false;
	}

	pEvoMonInfo->SetState(EVOMON_ACTIVE);
	pEvoMonInfo->SetEvoMonIndex(iEvoMon);
	pEvoMonInfo->SetEvoMonLevel(1);
	pEvoMonInfo->SetScore(pEvoMonInfo->GetEvoMonLevel());

	pMsg.btResult = 0;
	IOCP.DataSend(lpObj.m_Index, (LPBYTE)&pMsg, pMsg.h.size);

	
	return true;
}

int CEvolutionMonsterMng::SummonEvoMon(CGameObject &lpObj)
{
	LPMONSTER_ATTRIBUTE lpMonsterAttr = gMAttr.GetAttr(681);
	
	if (!lpMonsterAttr)
	{
		return -1;
	}

	int nResult = gObjAddMonster(lpObj.MapNumber);

	if (nResult < 0)
	{
		return -1;
	}

	gGameObjects[nResult]->m_Disappear_Monster = GetTickCount();
	gGameObjects[nResult]->m_nEvoMonOwnerIndex = lpObj.m_Index;
	gObjSetMonster(nResult, lpMonsterAttr->m_Index);
	gGameObjects[nResult]->DieRegen = 0;
	gGameObjects[nResult]->m_PosNum = -1;
	gGameObjects[nResult]->X = lpObj.X;
	gGameObjects[nResult]->Y = lpObj.Y;
	gGameObjects[nResult]->MapNumber = lpObj.MapNumber;
	gGameObjects[nResult]->TX = gGameObjects[nResult]->X;
	gGameObjects[nResult]->TY = gGameObjects[nResult]->Y;
	gGameObjects[nResult]->m_OldX = gGameObjects[nResult]->X;
	gGameObjects[nResult]->m_OldY = gGameObjects[nResult]->Y;
	gGameObjects[nResult]->StartX = LOBYTE(gGameObjects[nResult]->X);
	gGameObjects[nResult]->StartY = LOBYTE(gGameObjects[nResult]->Y);
	gGameObjects[nResult]->Level = LOWORD(lpMonsterAttr->m_Level);
	gGameObjects[nResult]->MaxRegenTime = 3000;
	gGameObjects[nResult]->Dir = rand() % 8;

	return nResult;
}

int CEvolutionMonsterMng::SetField(int iEvoMonIndex, int nFieldIndex, int nOwnerIndex, int iFieldClass)
{
	LPMONSTER_ATTRIBUTE lpMonsterAttr = gMAttr.GetAttr(iFieldClass);

	if (!lpMonsterAttr)
	{
		return -1;
	}

	gGameObjects[nFieldIndex]->m_Disappear_Monster = GetTickCount();
	gGameObjects[nFieldIndex]->m_nEvoMonOwnerIndex = nOwnerIndex;
	gObjSetMonster(nFieldIndex, lpMonsterAttr->m_Index);
	gGameObjects[nFieldIndex]->DieRegen = 0;
	gGameObjects[nFieldIndex]->m_PosNum = -1;
	gGameObjects[nFieldIndex]->X = gGameObjects[iEvoMonIndex]->X;
	gGameObjects[nFieldIndex]->Y = gGameObjects[iEvoMonIndex]->Y;
	gGameObjects[nFieldIndex]->MapNumber = gGameObjects[iEvoMonIndex]->MapNumber;
	gGameObjects[nFieldIndex]->TX = gGameObjects[iEvoMonIndex]->X;
	gGameObjects[nFieldIndex]->TY = gGameObjects[iEvoMonIndex]->Y;
	gGameObjects[nFieldIndex]->m_OldX = gGameObjects[iEvoMonIndex]->X;
	gGameObjects[nFieldIndex]->m_OldY = gGameObjects[iEvoMonIndex]->Y;
	gGameObjects[nFieldIndex]->StartX = gGameObjects[iEvoMonIndex]->X;
	gGameObjects[nFieldIndex]->StartY = gGameObjects[iEvoMonIndex]->Y;
	gGameObjects[nFieldIndex]->Level = lpMonsterAttr->m_Level;
	gGameObjects[nFieldIndex]->MaxRegenTime = 60000;
	gGameObjects[nFieldIndex]->Dir = rand() % 8;

	return nFieldIndex;
}

void CEvolutionMonsterMng::EvolutionMonsterDie(CGameObject & lpTargetObj, CGameObject &lpObj)
{
	if (!lpTargetObj)
	{
		return;
	}

	int nEvoMonOwnerIndex = lpTargetObj.m_nEvoMonOwnerIndex;

	if (!ObjectMaxRange(nEvoMonOwnerIndex))
	{
		return;
	}

	CGameObject lpOwnerObj = &gGameObjects[nEvoMonOwnerIndex];

	if (lpOwnerObj.Type != OBJ_USER)
	{
		return;
	}

	if (lpOwnerObj.CloseType != -1)
	{
		return;
	}

	CEvolutionMonsterInfo * pEvoMonInfo = lpOwnerObj.m_PlayerData->m_pCEvoMonInfo;

	if (!pEvoMonInfo)
	{
		return;
	}

	if (pEvoMonInfo->GetState() != EVOMON_ACTIVE)
	{
		return;
	}

	if (pEvoMonInfo->GetMaxScore() >= 30)
	{
		if (pEvoMonInfo->GetEvoMonLevel() >= 69)
		{
			pEvoMonInfo->SetEvoMonLevel(pEvoMonInfo->GetEvoMonLevel() + 1);
		}

		else
		{
			pEvoMonInfo->SetEvoMonLevel(pEvoMonInfo->GetEvoMonLevel() + 4);
		}
	}

	else
	{
		pEvoMonInfo->SetEvoMonLevel(pEvoMonInfo->GetEvoMonLevel() + 1);
	}

	pEvoMonInfo->SetScore(pEvoMonInfo->GetEvoMonLevel());

	int nOwnerTotalDamage = gObjMonsterGetDamageUser(lpTargetObj, lpOwnerObj);
	pEvoMonInfo->SetTotalDamage(pEvoMonInfo->GetTotalDamage() + nOwnerTotalDamage);

	this->GCSendUserScore(lpOwnerObj, pEvoMonInfo->GetEvoMonLevel());

	if (lpTargetObj.Class == 690)
	{
		PMSG_NOTICE pNotice;
		TNotice::MakeNoticeMsgEx(&pNotice, 0, "%s has killed Special EvoMon", lpObj.Name);
		TNotice::SendNoticeToAllUser(&pNotice);

		if (gObjCalDistance(lpOwnerObj, lpTargetObj) < 6)
		{
			int nPartyNumber = lpOwnerObj.PartyNumber;

			if (nPartyNumber < 0)
			{
				this->GiveReward(lpOwnerObj, true);
			}

			else
			{
				for (int i = 0; i < MAX_USER_IN_PARTY; i++)
				{
					int nPartyIndex = gParty.m_PartyS[nPartyNumber].Number[i];

					if (nPartyIndex != -1)
					{
						CGameObject lpPartyObj = &gGameObjects[nPartyIndex];
						this->GiveReward(lpPartyObj, true);
					}
				}
			}
		}
	}

	int nFieldIndex = pEvoMonInfo->GetFieldIndex();

	if (nFieldIndex != -1)
	{
		gObjDel(nFieldIndex);
		pEvoMonInfo->SetFieldIndex(-1);
	}

	nFieldIndex = gObjAddMonster(lpOwnerObj.MapNumber);

	if (nFieldIndex < 0)
	{
		return;
	}

	pEvoMonInfo->SetFieldIndex(nFieldIndex);

	std::map<int, EVOMON_MONSTER_DATA>::iterator It = this->m_mapEvoMonData.find(pEvoMonInfo->GetEvoMonLevel());

	if (It == this->m_mapEvoMonData.end())
	{
		MsgOutput(lpOwnerObj.m_Index, "You have reached maximum level of Evomon");

		this->GiveReward(lpOwnerObj, false);
		this->GDReqSaveEvoMonScore(lpOwnerObj.m_Index, pEvoMonInfo->GetScore(), pEvoMonInfo->GetTotalDamage());

		pEvoMonInfo->SetState(EVOMON_NOT_ACTIVE);
		return;
	}

	int nRand = rand() % 10000;
	bool bSuccess = false;

	if (nRand < It->second.m_iSuccessRate)
	{
		bSuccess = true;
	}

	int nFieldClass;

	if (bSuccess == false)
	{
		nFieldClass = 705;
	}

	else
	{
		nFieldClass = 704;
	}

	this->SetField(lpTargetObj.m_Index, nFieldIndex, nEvoMonOwnerIndex, nFieldClass);
	pEvoMonInfo->SetState(EVOMON_KILLED);
}

void CEvolutionMonsterMng::FieldProcess(CGameObject & lpFieldObj)
{
	if (!lpFieldObj)
	{
		return;
	}

	int nEvoMonOwnerIndex = lpFieldObj.m_nEvoMonOwnerIndex;

	if (!ObjectMaxRange(nEvoMonOwnerIndex))
	{
		gObjDel(lpFieldObj.m_Index);
		return;
	}

	CGameObject lpOwnerObj = &gGameObjects[nEvoMonOwnerIndex];

	if (lpOwnerObj.Type != OBJ_USER)
	{
		gObjDel(lpFieldObj.m_Index);
		return;
	}

	if (lpOwnerObj.CloseType != -1)
	{
		gObjDel(lpFieldObj.m_Index);
		return;
	}

	CEvolutionMonsterInfo * pEvoMonInfo = lpOwnerObj.m_PlayerData->m_pCEvoMonInfo;

	if (!pEvoMonInfo)
	{
		gObjDel(lpFieldObj.m_Index);
		return;
	}

	if (pEvoMonInfo->GetState() == EVOMON_KILLED)
	{
		if (lpFieldObj.Class == 704)
		{
			gObjAddBuffEffect(lpFieldObj, BUFFTYPE_EVOMON_SUCCESS, 0, 0, 0, 0, -10);
			MsgOutput(nEvoMonOwnerIndex, "Awakening succeeded. After a moment, awakened monster appear.");

			pEvoMonInfo->SetState(EVOMON_AWAKENING);

			lpFieldObj.m_Disappear_Monster = GetTickCount() + 2000;
		}

		else if (lpFieldObj.Class == 705)
		{
			gObjAddBuffEffect(lpFieldObj, BUFFTYPE_EVOMON_FAILED, 0, 0, 0, 0, -10);
			MsgOutput(nEvoMonOwnerIndex, "Awakening failed. Awakening has failed due to low chance.");
			
			pEvoMonInfo->SetState(EVOMON_AWAKE_FAILED);
			this->GiveReward(lpOwnerObj, false);

			lpFieldObj.m_Disappear_Monster = GetTickCount();
		}
	}

	else if (pEvoMonInfo->GetState() == EVOMON_AWAKENING)
	{
		this->EvoluteMonster(lpOwnerObj);
		pEvoMonInfo->SetState(EVOMON_ACTIVE);
	}

	else if (pEvoMonInfo->GetState() == EVOMON_AWAKE_FAILED)
	{
		this->EndEvoMon(lpOwnerObj);
	}
}

void CEvolutionMonsterMng::EndTimeProcess(CGameObject & lpMonsterObj)
{
	if (!lpMonsterObj)
	{
		return;
	}

	int nEvoMonOwnerIndex = lpMonsterObj.m_nEvoMonOwnerIndex;

	if (!ObjectMaxRange(nEvoMonOwnerIndex))
	{
		return;
	}

	CGameObject lpOwnerObj = &gGameObjects[nEvoMonOwnerIndex];

	if (lpOwnerObj.Type != OBJ_USER)
	{
		return;
	}

	if (lpOwnerObj.CloseType != -1)
	{
		return;
	}

	CEvolutionMonsterInfo * pEvoMonInfo = lpOwnerObj.m_PlayerData->m_pCEvoMonInfo;

	if (!pEvoMonInfo)
	{
		return;
	}

	MsgOutput(lpOwnerObj.m_Index, "You have reached a maximum time, EvoMon disappeared");
	
	this->EndEvoMon(lpOwnerObj);
}

void CEvolutionMonsterMng::EvoluteMonster(CGameObject &lpObj)
{
	if (!lpObj)
	{
		return;
	}

	if (lpObj.CloseType != -1)
	{
		return;
	}

	CEvolutionMonsterInfo * pEvoMonInfo = lpObj.m_PlayerData->m_pCEvoMonInfo;

	if (!pEvoMonInfo)
	{
		return;
	}

	if (pEvoMonInfo->GetState() != EVOMON_AWAKENING)
	{
		return;
	}

	int nEvoMonIndex = pEvoMonInfo->GetEvoMonIndex();
	int nEvoMonLevel = pEvoMonInfo->GetEvoMonLevel();
	int nFieldIndex = pEvoMonInfo->GetFieldIndex();

	if (!ObjectMaxRange(nEvoMonIndex))
	{
		return;
	}

	if (!ObjectMaxRange(nFieldIndex))
	{
		return;
	}

	std::map<int, EVOMON_MONSTER_DATA>::iterator It = this->m_mapEvoMonData.find(nEvoMonLevel);

	if (It == this->m_mapEvoMonData.end())
	{
		return;
	}

	std::vector<EVOMON_SPECIAL_MONSTER>::iterator It2 = this->m_vtSpecialEvoMon.end();

	for (It2 = this->m_vtSpecialEvoMon.begin(); It2 != this->m_vtSpecialEvoMon.end(); It2++)
	{
		if (nEvoMonLevel >= It2->m_wMinLevel && nEvoMonLevel <= It2->m_wMaxLevel)
		{
			break;
		}
	}

	int nMonsterClass = 681;

	if (It2 != this->m_vtSpecialEvoMon.end())
	{
		if (rand() % 10000 < It2->m_iSuccessRate)
		{
			nMonsterClass = 690;

			PMSG_NOTICE pNotice;
			TNotice::MakeNoticeMsgEx(&pNotice, 0, "%s has summoned Special EvoMon", lpObj.Name);
			TNotice::SendNoticeToAllUser(&pNotice);
		}
	}

	gGameObjects[nEvoMonIndex]->m_Disappear_Monster = GetTickCount();
	gGameObjects[nEvoMonIndex]->m_nEvoMonOwnerIndex = lpObj.m_Index;
	gObjSetMonster(nEvoMonIndex, nMonsterClass);
	gGameObjects[nEvoMonIndex]->Level = pEvoMonInfo->GetEvoMonLevel();
	this->SetMonsterStats(&gGameObjects[nEvoMonIndex], It->second);
	gGameObjects[nEvoMonIndex]->DieRegen = 0;
	gGameObjects[nEvoMonIndex]->m_PosNum = -1;
	gGameObjects[nEvoMonIndex]->X = gGameObjects[nFieldIndex]->X;
	gGameObjects[nEvoMonIndex]->Y = gGameObjects[nFieldIndex]->Y;
	gGameObjects[nEvoMonIndex]->MapNumber = gGameObjects[nFieldIndex]->MapNumber;
	gGameObjects[nEvoMonIndex]->TX = gGameObjects[nEvoMonIndex]->X;
	gGameObjects[nEvoMonIndex]->TY = gGameObjects[nEvoMonIndex]->Y;
	gGameObjects[nEvoMonIndex]->m_OldX = gGameObjects[nEvoMonIndex]->X;
	gGameObjects[nEvoMonIndex]->m_OldY = gGameObjects[nEvoMonIndex]->Y;
	gGameObjects[nEvoMonIndex]->StartX = gGameObjects[nEvoMonIndex]->X;
	gGameObjects[nEvoMonIndex]->StartY = gGameObjects[nEvoMonIndex]->Y;
	gGameObjects[nEvoMonIndex]->MaxRegenTime = 3000;
	gGameObjects[nEvoMonIndex]->Dir = rand() % 8;

}

void CEvolutionMonsterMng::SetMonsterStats(CGameObject & lpMonsterObj, EVOMON_MONSTER_DATA MonsterData)
{
	if (lpMonsterObj.Class == 690)
	{
		return;
	}

	LPMONSTER_ATTRIBUTE lpAttr = gMAttr.GetAttr(lpMonsterObj.Class);

	if (lpAttr == NULL)
	{
		return;
	}

	lpMonsterObj.Life = lpAttr->m_Hp * MonsterData.m_fHp;
	lpMonsterObj.MaxLife = lpAttr->m_Hp * MonsterData.m_fHp;
	lpMonsterObj.m_AttackDamageMin = lpAttr->m_DamageMin * MonsterData.m_fDamage;
	lpMonsterObj.m_AttackDamageMax = lpAttr->m_DamageMax * MonsterData.m_fDamage;
	lpMonsterObj.m_Defense = lpAttr->m_Defense * MonsterData.m_fDefense;
	lpMonsterObj.m_MagicDefense = lpAttr->m_MagicDefense * MonsterData.m_fDefense;
	lpMonsterObj.m_AttackRating = lpAttr->m_AttackRating * MonsterData.m_fAtkRate;
	lpMonsterObj.m_SuccessfulBlocking = lpAttr->m_Successfulblocking * MonsterData.m_fDefRate;
}

void CEvolutionMonsterMng::EndEvoMon(CGameObject &lpObj)
{
	if (!lpObj)
	{
		return;
	}

	if (lpObj.CloseType != -1)
	{
		return;
	}

	CEvolutionMonsterInfo * pEvoMonInfo = lpObj.m_PlayerData->m_pCEvoMonInfo;

	if (!pEvoMonInfo)
	{
		return;
	}

	this->GDReqSaveEvoMonScore(lpObj.m_Index, pEvoMonInfo->GetScore(), pEvoMonInfo->GetTotalDamage());
	pEvoMonInfo->Clear();
	pEvoMonInfo->SetState(EVOMON_NOT_ACTIVE);

}

void CEvolutionMonsterMng::UserQuit(CGameObject &lpObj)
{
	if (!lpObj)
	{
		return;
	}

	CEvolutionMonsterInfo * pEvoMonInfo = lpObj.m_PlayerData->m_pCEvoMonInfo;

	if (!pEvoMonInfo)
	{
		return;
	}

	if (pEvoMonInfo->GetState() != EVOMON_NOT_ACTIVE)
	{
	
	}

	this->GDReqSaveEvoMonScore(lpObj.m_Index, pEvoMonInfo->GetScore(), pEvoMonInfo->GetTotalDamage());
	pEvoMonInfo->Clear();
	pEvoMonInfo->SetState(EVOMON_NOT_ACTIVE);
}

void CEvolutionMonsterMng::GCSendUserScore(CGameObject &lpObj, int nScore)
{
	PMSG_EVOMON_SCORE pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0x3E, 0x11, sizeof(pMsg));

	pMsg.wScore = nScore;
	IOCP.DataSend(lpObj.m_PlayerData->IDNumber, (LPBYTE)&pMsg, pMsg.h.size);
}

void CEvolutionMonsterMng::GCSendEvoMonNotice(CGameObject &lpObj)
{
	PMSG_RANKING_SET pMsg;
	PHeadSetB((LPBYTE)&pMsg, 0x49, sizeof(pMsg));

	pMsg.btSetType = 0;
	pMsg.iRankingType = this->IsEvoMonEnable();

	IOCP.DataSend(lpObj.m_Index, (LPBYTE)&pMsg, pMsg.h.size);

	if (this->IsEvoMonEnable() == true)
	{
		gGameProtocol.GCSendEventBanner(aIndex, 2); //[K2]
	}
}

void CEvolutionMonsterMng::GiveReward(CGameObject &lpObj, bool bSpecialEvoMon)
{
	if (!lpObj)
	{
		return;
	}

	if (lpObj.CloseType != -1)
	{
		return;
	}

	CEvolutionMonsterInfo * pEvoMonInfo = lpObj.m_PlayerData->m_pCEvoMonInfo;

	if (!pEvoMonInfo)
	{
		return;
	}

	if (bSpecialEvoMon == true)
	{
		int iItemType = this->GetRewardItemType(this->m_btSpecialEvoMonRewardType);

		if (iItemType == -1)
		{
			return;
		}

		_stGremoryCaseItem stItem;
		stItem.btStorageType = GC_STORAGE_CHARACTER;
		stItem.btRewardSource = GC_REWARD_GM_REWARD;
		stItem.wItemID = iItemType;

		g_GremoryCase.GDReqAddItemToGremoryCase(lpObj.m_Index, stItem, 7);
	}

	else
	{
		int nEvoMonLevel = pEvoMonInfo->GetEvoMonLevel();
		std::vector<EVOMON_REWARD_RANGE>::iterator It = this->m_vtRewardRange.end();

		for (It = this->m_vtRewardRange.begin(); It != this->m_vtRewardRange.end(); It++)
		{
			if (nEvoMonLevel >= It->m_wMinLevel && nEvoMonLevel <= It->m_wMaxLevel)
			{
				break;
			}
		}

		if (It == this->m_vtRewardRange.end())
		{
			return;
		}

		int iItemType = this->GetRewardItemType(It->m_btRewardType);

		if (iItemType == -1)
		{
			return;
		}

		_stGremoryCaseItem stItem;
		stItem.btStorageType = GC_STORAGE_CHARACTER;
		stItem.btRewardSource = GC_REWARD_EVOMON;
		stItem.wItemID = iItemType;

		g_GremoryCase.GDReqAddItemToGremoryCase(lpObj.m_Index, stItem, 7);
	}
}

bool CEvolutionMonsterMng::IsEventMap(int iMapNumber, int iX, int iY)
{
	if (BC_MAP_RANGE(iMapNumber))
	{
		return TRUE;
	}

	if (CC_MAP_RANGE(iMapNumber))
	{
		return TRUE;
	}

	if (DS_MAP_RANGE(iMapNumber))
	{
		return TRUE;
	}

	if (DG_MAP_RANGE(iMapNumber))
	{
		return TRUE;
	}

	if (IMPERIAL_MAP_RANGE(iMapNumber))
	{
		return TRUE;
	}

	if (KALIMA_MAP_RANGE(iMapNumber))
	{
		return TRUE;
	}

	if (iMapNumber == MAP_INDEX_CHAOSCASTLE_SURVIVAL)
	{
		return TRUE;
	}

	if (iMapNumber == MAP_INDEX_HATCHERY)
	{
		return TRUE;
	}

	if (iMapNumber == MAP_INDEX_ICARUS)
	{
		return TRUE;
	}

	if (iMapNumber == MAP_INDEX_VULCANROOM)
	{
		return TRUE;
	}

	if (iMapNumber == MAP_INDEX_ARCA_WAR || iMapNumber == MAP_INDEX_DEBENTER_ARCA_WAR)
	{
		return TRUE;
	}

	if (iMapNumber == MAP_INDEX_BARRACKS || iMapNumber == MAP_INDEX_REFUGEE)
	{
		return TRUE;
	}

	if (iMapNumber == MAP_INDEX_VULCAN)
	{
		return TRUE;
	}

	if (iMapNumber == MAP_INDEX_CASTLESIEGE)
	{
		return TRUE;
	}

	if (ITL_MAP_RANGE(iMapNumber))
	{
		return TRUE;
	}

	BYTE btAttr = MapC[iMapNumber].GetAttr(iX, iY);

	if ((btAttr & 1) == 1)
	{
		return TRUE;
	}

	return FALSE;
}

int CEvolutionMonsterMng::GetRewardItemType(BYTE btType)
{
	if (btType >= 0 && btType <= 5)
	{
		return ITEMGET(14, 248 + btType);
	}

	return -1;
}

bool CEvolutionMonsterMng::IsEvolutedEvoMon(CGameObject &monster)
{
	if (monster->Class == 681 && monster->Level > 1)
	{
		return true;
	}

	return false;
}

int CEvolutionMonsterMng::GetOwnerIndex(int iMonsterIndex)
{
	if (!ObjectMaxRange(iMonsterIndex))
	{
		return -1;
	}

	return gGameObjects[iMonsterIndex]->m_nEvoMonOwnerIndex;
}

void CEvolutionMonsterMng::GDReqEvoMonMaxScore(CGameObject &lpObj)
{
	PMSG_REQ_EVOMON_MAXSCORE pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0x3E, 0x00, sizeof(pMsg));

	pMsg.nUserIndex = aIndex;
	memcpy(pMsg.szName, lpObj.Name, MAX_ACCOUNT_LEN + 1);

	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
}

void CEvolutionMonsterMng::DGAnsEvoMonMaxScore(LPBYTE lpRecv)
{
	PMSG_ANS_EVOMON_MAXSCORE * lpMsg = (PMSG_ANS_EVOMON_MAXSCORE *)lpRecv;

	int nUserIndex = lpMsg->nUserIndex;

	if (!ObjectMaxRange(nUserIndex))
	{
		return;
	}

	CEvolutionMonsterInfo * pEvoMonInfo = gGameObjects[nUserIndex]->m_PlayerData->m_pCEvoMonInfo;

	if (pEvoMonInfo == NULL)
	{
		return;
	}

	pEvoMonInfo->SetMaxScore(lpMsg->nMaxScore);

}

void CEvolutionMonsterMng::GDReqSaveEvoMonScore(CGameObject &lpObj, int nScore, int nTotalDamage)
{
	PMSG_REQ_SAVE_EVOMON_RESULT pMsg;
	PHeadSubSetB((LPBYTE)&pMsg, 0x3E, 0x01, sizeof(pMsg));

	memcpy(pMsg.szName, lpObj.Name, MAX_ACCOUNT_LEN + 1);
	pMsg.nUserIndex = aIndex;
	pMsg.nScore = nScore;
	pMsg.nTotalDamage = nTotalDamage;

	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
}



////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

