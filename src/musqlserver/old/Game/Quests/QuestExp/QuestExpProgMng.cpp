////////////////////////////////////////////////////////////////////////////////
// QuestExpProgMng.cpp
#include "StdAfx.h"
#include "QuestExpProgMng.h"
#include "QuestExpInfo.h"
#include "QuestExpUserInfo.h"
#include "QuestExpUserMng.h"
#include "QuestExpLuaBind.h"
#include "User/CUserData.h"
#include "Logging/Log.h"
#include "GameMain.h"
#include "util.h"
#include "gObjMonster.h"
#include "BuffEffectSlot.h"

#include "GensSystem.h"
#include "configread.h"

QuestExpProgMng g_QuestExpProgMng;

QuestExpProgMng::QuestExpProgMng(void)
{
	this->m_iChangeDay = -1;
}

QuestExpProgMng::~QuestExpProgMng(void)
{
}

void QuestExpProgMng::ReqQuestAskComplete(DWORD dwQuestInfoIndexID, int iObjIndex)
{
	int iEpisode = GetQuestEpisodeFromInfoIndexId(dwQuestInfoIndexID);
	int iQS = GetQuestSwitchFromInfoIndexId(dwQuestInfoIndexID);

	if (getGameObject(iObjIndex)->m_PlayerData->m_UserQuestInfo[iEpisode]->m_UserQuestAskInfo[0)->GetQuestType() == QUESTEXP_ASK_TUTORIAL_KEY)
	{
		getGameObject(iObjIndex)->m_PlayerData->m_UserQuestInfo[iEpisode]->m_UserQuestAskInfo[0)->SetComplete(true);
		getGameObject(iObjIndex)->m_PlayerData->m_UserQuestInfo[iEpisode]->m_UserQuestAskInfo[0)->SetValue(1);

		this->SendQuestAskInfoUpdate(iEpisode, iQS, QUESTEXP_ASK_TUTORIAL_KEY, getGameObject(iObjIndex)->m_PlayerData->m_UserQuestInfo[iEpisode]->m_UserQuestAskInfo[0)->GetValue(), iObjIndex);
	}
}

struct PMSG_QUEST_GIVEUP_ANS
{
    PBMSG_HEAD2 h;
    DWORD dwQuestInfoIndexID;
};

void QuestExpProgMng::QuestExpGiveUpBtnClick(DWORD dwQuestInfoIndexID, int iObjIndex)
{
	int iEpisode = GetQuestEpisodeFromInfoIndexId(dwQuestInfoIndexID);
	int iQS = GetQuestSwitchFromInfoIndexId(dwQuestInfoIndexID);

	//sLog->outBasic("[QuestExp] Give Up [%s][%s] Ep[%d] QS[%d]", getGameObject(iObjIndex)->AccountID, getGameObject(iObjIndex)->Name, iEpisode, iQS);

	int iUserQuestSwitch = getGameObject(iObjIndex)->m_PlayerData->m_UserQuestInfo[iEpisode)->GetQuestSwitch();

	lua_State *L = g_MuLuaQuestExp.GetLua();

	if (!L)
	{
		sLog->outBasic("[QuestExp] - Error - [%s] [%d]", __FILE__, __LINE__);
		return;
	}

	g_MuLuaQuestExp.Generic_Call("QuestGiveUp", "iii>", iObjIndex, iEpisode, iUserQuestSwitch);

	getGameObject(iObjIndex)->m_PlayerData->m_UserQuestInfo[iEpisode)->QuestAskInfoClear();

	int iSendQS = getGameObject(iObjIndex)->m_PlayerData->m_UserQuestInfo[iEpisode)->GetQuestSwitch();

	//sLog->outBasic("[QuestExp] Give Up - SetQuestSwitch [%s][%s] Ep[%d] QS[%d]", getGameObject(iObjIndex)->AccountID, getGameObject(iObjIndex)->Name, iEpisode, iSendQS);

	PMSG_QUEST_GIVEUP_ANS pMsg;
	pMsg.dwQuestInfoIndexID = GetQuestInfoIndexId(iEpisode, iSendQS);

	PHeadSubSetB(reinterpret_cast<BYTE*>(&pMsg), 0xF6, 0x0F, sizeof(pMsg));
	IOCP.DataSend(iObjIndex, reinterpret_cast<BYTE*>(&pMsg), pMsg.h.size);
}

#pragma pack (1)
struct PMSG_NPC_QUESTEXP_INFO
{
	PBMSG_HEAD2 h;
	DWORD dwQuestInfoIndexID;
	BYTE btAskCnt;
	BYTE btRewardCnt;
	BYTE btRandRewardCnt;

	PMSG_NPC_QUESTEXP_INFO()
	{
		this->dwQuestInfoIndexID = 0;
		this->btAskCnt = 0;
		this->btRewardCnt = 0;
		this->btRandRewardCnt = 0;
	}
};
#pragma pack ()

void QuestExpProgMng::SendQuestProgress(DWORD dwQuestInfoIndexID, int iObjIndex)
{
	PMSG_NPC_QUESTEXP_INFO pMsg;

	pMsg.dwQuestInfoIndexID = dwQuestInfoIndexID;
	pMsg.btAskCnt = 0;
	pMsg.btRewardCnt = 0;
	PHeadSubSetB((BYTE*)&pMsg, 0xF6, 0x0B, sizeof(pMsg));
	IOCP.DataSend(iObjIndex, (BYTE*)&pMsg, pMsg.h.size);

	//sLog->outBasic("[QuestExp] - Ans Send User : Selection Statements Choose One [%s][%s] QuestIndexID[0x%x]", getGameObject(iObjIndex)->AccountID, getGameObject(iObjIndex)->Name, dwQuestInfoIndexID);
}

#pragma pack (1)
struct NPC_QUESTEXP_ASK_INFO
{
	DWORD dwAskType;
	WORD wIndex;
	DWORD dwValue;
	DWORD dwCurrentValue;
	BYTE btItemInfo[MAX_ITEM_INFO];

	NPC_QUESTEXP_ASK_INFO()
	{
		this->dwAskType = 0;
		this->wIndex = 0;
		this->dwValue = 0;
		this->dwCurrentValue = 0;
		memset(this->btItemInfo, 0, sizeof(this->btItemInfo));
	}
};

struct NPC_QUESTEXP_REWARD_INFO
{
	DWORD dwAskType;
	WORD wIndex;
	DWORD dwValue;
	BYTE btItemInfo[MAX_ITEM_INFO];

	NPC_QUESTEXP_REWARD_INFO()
	{
		this->dwAskType = 0;
		this->wIndex = 0;
		this->dwValue = 0;
		memset(this->btItemInfo, 0, sizeof(this->btItemInfo));
	}
};

struct NPC_QUESTEXP_INFO
{
	NPC_QUESTEXP_ASK_INFO NpcQuestAskInfo[MAX_QUESTEXP_ASK_INFO];
	NPC_QUESTEXP_REWARD_INFO NpcQuestRewardInfo[MAX_QUESTEXP_REWARD_INFO];
};
#pragma pack ()

void QuestExpProgMng::SendQuestProgress(QuestExpInfo* pQuestExpInfo, DWORD dwQuestInfoIndexID, int iObjIndex)
{
	BYTE sendBuff[2048] = { 0 };
	PMSG_NPC_QUESTEXP_INFO pMsg;
	int lOfs = sizeof(pMsg);

	NPC_QUESTEXP_INFO NpcQuestExpInfo;

	bool bGetSendQuestInfo = this->GetSendQuestInfo(pQuestExpInfo, dwQuestInfoIndexID, iObjIndex, &NpcQuestExpInfo);

	if (!bGetSendQuestInfo)
	{
		sLog->outBasic("[QuestExp] - Error - SendQuestProgress() [%s] [%d]", __FILE__, __LINE__);
		return;
	}

	memcpy(&sendBuff[lOfs], &NpcQuestExpInfo, sizeof(NpcQuestExpInfo));
	lOfs += sizeof(NpcQuestExpInfo);

	auto MapQuestAsk = pQuestExpInfo->GetQuestExpAsk();
	auto MapQuestReward = pQuestExpInfo->GetQuestReward();

	int iAskCnt = MapQuestAsk.size();

	QuestExpRewardKind* pRewardKind = g_QuestExpManager.GetQuestExpRewardKind(dwQuestInfoIndexID);

	if (pRewardKind)
	{
		if (pRewardKind->GetRewardKind() == true)
		{
			QuestExpRewardKindRandom* pRewardKindRandom = static_cast<QuestExpRewardKindRandom*>(pRewardKind);
			pMsg.btRandRewardCnt = pRewardKindRandom->GetRandRewardCnt();
		}
	}

	pMsg.dwQuestInfoIndexID = dwQuestInfoIndexID;
	pMsg.btAskCnt = iAskCnt;
	pMsg.btRewardCnt = MapQuestReward.size();

	PHeadSubSetB((BYTE*)&pMsg, 0xF6, 0x0C, lOfs);
	memcpy(sendBuff, &pMsg, sizeof(pMsg));
	IOCP.DataSend(iObjIndex, sendBuff, lOfs);

	//sLog->outBasic("[QuestExp] Send QuestInfo From NPC To User : [%s][%s] Ep[%d] QS[%d]", getGameObject(iObjIndex)->AccountID, getGameObject(iObjIndex)->Name, GetQuestEpisodeFromInfoIndexId(dwQuestInfoIndexID), GetQuestSwitchFromInfoIndexId(dwQuestInfoIndexID));
}

void QuestExpProgMng::SendQuestProgress(int iEpisode, int iQS, int iObjIndex)
{
	DWORD dwQuestInfoIndexID = GetQuestInfoIndexId(iEpisode, iQS);

	if (this->ChkQuestIndexIDToEpAndQSLimit(dwQuestInfoIndexID, iObjIndex) == false)
	{
		sLog->outBasic("[QuestExp] - Error - [%s] [%d]", __FILE__, __LINE__);
		return;
	}

	QuestExpInfo* pQuestExpInfo = g_QuestExpManager.GetQuestExpInfo(dwQuestInfoIndexID);

	if (pQuestExpInfo)
	{
		this->SendQuestProgress(pQuestExpInfo, dwQuestInfoIndexID, iObjIndex);
	}

	else
	{
		this->SendQuestProgress(dwQuestInfoIndexID, iObjIndex);
	}
}

void QuestExpProgMng::SendQuestProgressInfo(DWORD dwQuestInfoIndexID, int iObjIndex)
{
	QuestExpInfo* pQuestExpInfo = g_QuestExpManager.GetQuestExpInfo(dwQuestInfoIndexID);

	if (pQuestExpInfo)
	{
		this->SendQuestProgressInfo(pQuestExpInfo, dwQuestInfoIndexID, iObjIndex);
	}
}

void QuestExpProgMng::SendQuestProgressInfo(QuestExpInfo* pQuestExpInfo, DWORD dwQuestInfoIndexID, int iObjIndex)
{
	BYTE sendBuff[2048] = { 0 };
	PMSG_NPC_QUESTEXP_INFO pMsg;
	int lOfs = sizeof(pMsg);

	NPC_QUESTEXP_INFO NpcQuestExpInfo;

	bool bGetSendQuestInfo = GetSendQuestInfo(pQuestExpInfo, dwQuestInfoIndexID, iObjIndex, &NpcQuestExpInfo);

	if (!bGetSendQuestInfo)
	{
		sLog->outBasic("[QuestExp] - Error - SendQuestProgressInfo() [%s] [%d]", __FILE__, __LINE__);
	}

	memcpy(&sendBuff[lOfs], &NpcQuestExpInfo, sizeof(NpcQuestExpInfo));
	lOfs += sizeof(NpcQuestExpInfo);

	auto MapQuestAsk = pQuestExpInfo->GetQuestExpAsk();
	auto MapQuestReward = pQuestExpInfo->GetQuestReward();
	int iAskCnt = MapQuestAsk.size();

	QuestExpRewardKind* pRewardKind = g_QuestExpManager.GetQuestExpRewardKind(dwQuestInfoIndexID);

	if (pRewardKind)
	{
		if (pRewardKind->GetRewardKind() == true)
		{
			QuestExpRewardKindRandom* pRewardKindRandom = static_cast<QuestExpRewardKindRandom*>(pRewardKind);
			pMsg.btRandRewardCnt = pRewardKindRandom->GetRandRewardCnt();
		}
	}

	pMsg.dwQuestInfoIndexID = dwQuestInfoIndexID;
	pMsg.btAskCnt = iAskCnt;
	pMsg.btRewardCnt = MapQuestReward.size();

	PHeadSubSetB((BYTE*)&pMsg, 0xF6, 0x1B, lOfs);

	memcpy(sendBuff, &pMsg, sizeof(pMsg));
	IOCP.DataSend(iObjIndex, sendBuff, lOfs);

	//sLog->outBasic("[QuestExp] Send Quest Info To QuestList : [%s][%s] Ep[%d] QS[%d]",
	//	getGameObject(iObjIndex)->AccountID, getGameObject(iObjIndex)->Name, GetQuestEpisodeFromInfoIndexId(dwQuestInfoIndexID), GetQuestSwitchFromInfoIndexId(dwQuestInfoIndexID));


}

bool QuestExpProgMng::ChkQuestAsk(int iEpisode, int iObjIndex)
{
	int iAskCnt = 0;
	UserQuestInfo* pUserQuestInfo = g_QuestExpUserMng.GetUserQuestInfo(iEpisode, iObjIndex);

	if (!pUserQuestInfo)
	{
		sLog->outBasic("[QuestExp] - Error - [%s] [%d]", __FILE__, __LINE__);
		return false;
	}

	int iQS = pUserQuestInfo->GetQuestSwitch();
	DWORD dwQuestInfoIndexID = GetQuestInfoIndexId(iEpisode, iQS);
	QuestExpInfo* pQuestExpInfo = g_QuestExpManager.GetQuestExpInfo(dwQuestInfoIndexID);

	if (!pQuestExpInfo)
	{
		sLog->outBasic("[QuestExp] - Error - ChkQuestAsk : [%s][%s] QuestIndexID[0x%x]", getGameObject(iObjIndex)->AccountID, getGameObject(iObjIndex)->Name, dwQuestInfoIndexID);
		return false;
	}

	auto MapQuestAsk = pQuestExpInfo->GetQuestExpAsk();

	auto map_it = MapQuestAsk.begin();
	auto map_end = MapQuestAsk.end();

	while (true)
	{
		if (map_it == map_end)
			break;

		QuestExpAsk* pQuestExpAsk = map_it->second;

		if (!pQuestExpAsk)
		{
			sLog->outBasic("[QuestExp] - Error - ChkQuestAsk *** : [%s][%s] QuestIndexID[0x%x]", getGameObject(iObjIndex)->AccountID, getGameObject(iObjIndex)->Name, dwQuestInfoIndexID);
			return false;
		}

		if (IsQuestAskInfo(iAskCnt) == false)
		{
			sLog->outBasic("[QuestExp] - Error - iAskCnt [%d] [%s] [%d]", iAskCnt, __FILE__, __LINE__);
			return false;
		}

		int iQuestType = pQuestExpAsk->GetQuestType();

		switch (iQuestType)
		{
		case QUESTEXP_ASK_NONE:
		case QUESTEXP_ASK_MONSTER:
		case QUESTEXP_ASK_TUTORIAL_KEY:
			break;

		case QUESTEXP_ASK_GET_ITEM:
		{
			QuestGetItem* pQuestGetItem = static_cast<QuestGetItem*>(pQuestExpAsk);
			ChkUserQuestTypeItem(pQuestGetItem, &pUserQuestInfo->m_UserQuestAskInfo[iAskCnt], iObjIndex);
		}
		break;

		case QUESTEXP_ASK_SKILL_LEARN:
		{
			QuestSkillLearn* pQuestSkillLearn = static_cast<QuestSkillLearn*>(pQuestExpAsk);
			ChkUserQuestTypeSkillLearn(pQuestSkillLearn, &pUserQuestInfo->m_UserQuestAskInfo[iAskCnt], iObjIndex);
		}
		break;

		case QUESTEXP_ASK_LEVEL_UP:
		{
			QuestLevelUp* pQuestLevelUp = static_cast<QuestLevelUp*>(pQuestExpAsk);
			ChkUserQuestTypeLevelUp(pQuestLevelUp, &pUserQuestInfo->m_UserQuestAskInfo[iAskCnt], iObjIndex);
		}
		break;

		case QUESTEXP_ASK_BUFF:
		{
			QuestBuff* pQuestBuff = static_cast<QuestBuff*>(pQuestExpAsk);
			ChkUserQuestTypeBuff(pQuestBuff, &pUserQuestInfo->m_UserQuestAskInfo[iAskCnt], iObjIndex);
		}
		break;

		case QUESTEXP_ASK_CHAOSCASTLE_USER_KILL:
		case QUESTEXP_ASK_CHAOSCASTLE_MON_KILL:
		case QUESTEXP_ASK_BLOODCASTLE_DOOR_KILL:
		case QUESTEXP_ASK_BLOODCASTLE_CLEAR:
		case QUESTEXP_ASK_CHAOSCASTLE_CLEAR:
		case QUESTEXP_ASK_DEVILSQUARE_CLEAR:
		case QUESTEXP_ASK_ILLUSIONTEMPLE_CLEAR:
		case QUESTEXP_ASK_DEVILSQUARE_POINT_GAIN:
		case QUESTEXP_ASK_CONTRIBUTION_POINT:
		case QUESTEXP_ASK_NPC_TALK:
			break;

		case QUESTEXP_ASK_NEED_ZEN:
		{
			QuestNeedZen* pQuestNeedZen = static_cast<QuestNeedZen*>(pQuestExpAsk);
			ChkUserQuestTypeNeedZen(pQuestNeedZen, &pUserQuestInfo->m_UserQuestAskInfo[iAskCnt], iObjIndex);
		}

		break;

		default:
			sLog->outBasic("[QuestExp] - Error - Unknown Ask Type [%d] [%s] [%d]", iQuestType, __FILE__, __LINE__);
			return false;
		}

		iAskCnt++;
		map_it++;
	}

	return true;
}

bool QuestExpProgMng::GetSendQuestInfo(QuestExpInfo* pQuestExpInfo, DWORD dwQuestInfoIndexID, int iObjIndex, NPC_QUESTEXP_INFO* pNpcQuestExpInfo)
{
	int iEpisode = GetQuestEpisodeFromInfoIndexId(dwQuestInfoIndexID);
	UserQuestInfo* pUserQuestInfo = g_QuestExpUserMng.GetUserQuestInfo(iEpisode, iObjIndex);

	if (!pUserQuestInfo)
	{
		sLog->outBasic("[QuestExp] - Error - GetSendQuestInfo()  pUserQuestInfo == NULL : [%s][%s]", getGameObject(iObjIndex)->AccountID, getGameObject(iObjIndex)->Name);
		return false;
	}

	auto MapQuestAsk = pQuestExpInfo->GetQuestExpAsk();
	int iAskCnt = 0;

	auto map_it = MapQuestAsk.begin();
	auto map_end = MapQuestAsk.end();

	while (true)
	{
		if (map_it == map_end)
			break;

		QuestExpAsk* pQuestExpAsk = map_it->second;

		if (!pQuestExpAsk)
		{
			sLog->outBasic("[QuestExp] - Error - GetSendQuestInfo *** : [%s][%s] QuestIndexID[0x%x]", getGameObject(iObjIndex)->AccountID, getGameObject(iObjIndex)->Name, dwQuestInfoIndexID);
			return false;
		}

		if (IsQuestAskInfo(iAskCnt) == false)
		{
			sLog->outBasic("[QuestExp] - Error - iAskCnt [%d] [%s] [%d]", iAskCnt, __FILE__, __LINE__);
			return false;
		}

		int iQuestType = pQuestExpAsk->GetQuestType();

		switch (iQuestType)
		{
		case QUESTEXP_ASK_NONE:
			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].dwAskType = 0;
			break;

		case QUESTEXP_ASK_TUTORIAL_KEY:
			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].dwAskType = QUESTEXP_ASK_TUTORIAL_KEY;
			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].dwCurrentValue = pUserQuestInfo->m_UserQuestAskInfo[iAskCnt].GetValue();
			break;

		case QUESTEXP_ASK_MONSTER:
		{
			QuestMonsterKill* pQuestMonsterKill = static_cast<QuestMonsterKill*>(pQuestExpAsk);

			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].dwAskType = QUESTEXP_ASK_MONSTER;
			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].wIndex = pQuestMonsterKill->GetMonsterIndex();
			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].dwValue = pQuestMonsterKill->GetMonsterKillCnt();
			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].dwCurrentValue = pUserQuestInfo->m_UserQuestAskInfo[iAskCnt].GetValue();
		}
		break;

		case QUESTEXP_ASK_GET_ITEM:
		{
			QuestGetItem* pQuestGetItem = static_cast<QuestGetItem*>(pQuestExpAsk);
			ChkUserQuestTypeItem(pQuestGetItem, &pUserQuestInfo->m_UserQuestAskInfo[iAskCnt], iObjIndex);

			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].dwAskType = QUESTEXP_ASK_GET_ITEM;
			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].wIndex = pQuestGetItem->GetQuestItemNum();
			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].dwValue = pQuestGetItem->GetQuestItemCnt();
			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].dwCurrentValue = pUserQuestInfo->m_UserQuestAskInfo[iAskCnt].GetValue();

			CItemObject Item;
			Item.m_Type = pQuestGetItem->GetQuestItemNum();
			Item.m_Level = pQuestGetItem->GetQuestItemLevel();
			Item.m_Option1 = pQuestGetItem->GetQuestItemSkill();
			Item.m_Option3 = pQuestGetItem->GetQuestItemOpt();
			Item.m_Durability = 0;
			Item.m_NewOption = pQuestGetItem->GetQuestItemExOpt();

			ItemByteConvert(pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].btItemInfo, Item);
		}
		break;

		case QUESTEXP_ASK_SKILL_LEARN:
		{
			QuestSkillLearn* pQuestSkillLearn = static_cast<QuestSkillLearn*>(pQuestExpAsk);
			ChkUserQuestTypeSkillLearn(pQuestSkillLearn, &pUserQuestInfo->m_UserQuestAskInfo[iAskCnt], iObjIndex);

			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].dwAskType = QUESTEXP_ASK_SKILL_LEARN;
			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].wIndex = pQuestSkillLearn->GetQuestSkillIndex();
			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].dwCurrentValue = pUserQuestInfo->m_UserQuestAskInfo[iAskCnt].GetValue();
		}
		break;

		case QUESTEXP_ASK_LEVEL_UP:
		{
			QuestLevelUp* pQuestLevelUp = static_cast<QuestLevelUp*>(pQuestExpAsk);
			ChkUserQuestTypeLevelUp(pQuestLevelUp, &pUserQuestInfo->m_UserQuestAskInfo[iAskCnt], iObjIndex);

			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].dwAskType = QUESTEXP_ASK_LEVEL_UP;
			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].wIndex = pQuestLevelUp->GetQuestLevelVal();
			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].dwCurrentValue = pUserQuestInfo->m_UserQuestAskInfo[iAskCnt].GetValue();
		}
		break;

		case QUESTEXP_ASK_BUFF:
		{
			QuestBuff* pQuestBuff = static_cast<QuestBuff*>(pQuestExpAsk);
			ChkUserQuestTypeBuff(pQuestBuff, &pUserQuestInfo->m_UserQuestAskInfo[iAskCnt], iObjIndex);

			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].dwAskType = QUESTEXP_ASK_BUFF;
			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].dwValue = pQuestBuff->GetQuestBuffIndex();
			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].dwCurrentValue = pUserQuestInfo->m_UserQuestAskInfo[iAskCnt].GetValue();
		}

		break;

		case QUESTEXP_ASK_CHAOSCASTLE_USER_KILL:
		case QUESTEXP_ASK_CHAOSCASTLE_MON_KILL:
		case QUESTEXP_ASK_BLOODCASTLE_DOOR_KILL:
		{
			QuestEventMapKillPoint* pQuestEventMapKillPoint = static_cast<QuestEventMapKillPoint*>(pQuestExpAsk);

			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].dwAskType = iQuestType;
			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].wIndex = pQuestEventMapKillPoint->GetQuestEventMapLevel();
			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].dwValue = pQuestEventMapKillPoint->GetQuestEventMapKillCnt();
			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].dwCurrentValue = pUserQuestInfo->m_UserQuestAskInfo[iAskCnt].GetValue();
		}
		break;

		case QUESTEXP_ASK_BLOODCASTLE_CLEAR:
		case QUESTEXP_ASK_CHAOSCASTLE_CLEAR:
		case QUESTEXP_ASK_DEVILSQUARE_CLEAR:
		case QUESTEXP_ASK_ILLUSIONTEMPLE_CLEAR:
		{
			QuestEventMapClear* pQuestEventMapClear = static_cast<QuestEventMapClear*>(pQuestExpAsk);

			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].dwAskType = iQuestType;
			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].wIndex = pQuestEventMapClear->GetQuestEventMapLevel();
			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].dwCurrentValue = pUserQuestInfo->m_UserQuestAskInfo[iAskCnt].GetValue();
		}
		break;

		case QUESTEXP_ASK_DEVILSQUARE_POINT_GAIN:
		{
			QuestEventMapDevilPoint* pQuestEventMapDevilPoint = static_cast<QuestEventMapDevilPoint*>(pQuestExpAsk);

			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].dwAskType = iQuestType;
			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].wIndex = pQuestEventMapDevilPoint->GetQuestEventMapLevel();
			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].dwValue = pQuestEventMapDevilPoint->GetQuestEventMapDevilPoint();
			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].dwCurrentValue = pUserQuestInfo->m_UserQuestAskInfo[iAskCnt].GetValue();
		}
		break;

		case QUESTEXP_ASK_NEED_ZEN:
		{
			QuestNeedZen* pQuestNeedZen = static_cast<QuestNeedZen*>(pQuestExpAsk);
			ChkUserQuestTypeNeedZen(pQuestNeedZen, &pUserQuestInfo->m_UserQuestAskInfo[iAskCnt], iObjIndex);

			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].dwAskType = iQuestType;
			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].wIndex = pQuestNeedZen->GetQuestNeedZen();
			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].dwCurrentValue = pUserQuestInfo->m_UserQuestAskInfo[iAskCnt].GetValue();
		}
		break;

		case QUESTEXP_ASK_CONTRIBUTION_POINT:
		{
			QuestPVP_Point* pQuestPVP_Point = static_cast<QuestPVP_Point*>(pQuestExpAsk);

			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].dwAskType = iQuestType;
			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].wIndex = pQuestPVP_Point->GetQuestPVP_Point();
			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].dwCurrentValue = pUserQuestInfo->m_UserQuestAskInfo[iAskCnt].GetValue();
		}

		break;

		case QUESTEXP_ASK_NPC_TALK:
		{
			QuestNpcTalk* pQuestNpcTalk = static_cast<QuestNpcTalk*>(pQuestExpAsk);

			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].dwAskType = iQuestType;
			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].wIndex = pQuestNpcTalk->GetQuestNPCTalk();
			pNpcQuestExpInfo->NpcQuestAskInfo[iAskCnt].dwCurrentValue = pUserQuestInfo->m_UserQuestAskInfo[iAskCnt].GetValue();
		}
		break;

		default:

			sLog->outBasic("[QuestExp] - Error - Unknown Ask Type [%d] [%s] [%d]", iQuestType, __FILE__, __LINE__);
			return false;
		}

		map_it++;
		iAskCnt++;
	}

	QuestExpRewardKindRandom* pRewardKindRandom = nullptr;
	int iArrRewardIndex[MAX_QUESTEXP_REWARDKIND_RANDOM] = { 0, 0, 0, 0, 0 };

	QuestExpRewardKind* pRewardKind = g_QuestExpManager.GetQuestExpRewardKind(dwQuestInfoIndexID);
	if (pRewardKind)
	{
		if (pRewardKind->GetRewardKind() == true)
		{
			pRewardKindRandom = static_cast<QuestExpRewardKindRandom*>(pRewardKind);
			memcpy(iArrRewardIndex, pRewardKindRandom->GetRewardIndex(), sizeof(iArrRewardIndex));
		}
	}

	auto MapQuestReward = pQuestExpInfo->GetQuestReward();

	auto quest_reward_it = MapQuestReward.begin();
	auto quest_reward_end = MapQuestReward.end();

	int iReward = 0;

	while (true)
	{
		if (quest_reward_it == quest_reward_end)
		{
			break;
		}

		QuestExpReward* pQuestExpReward = quest_reward_it->second;

		if (!pQuestExpReward)
		{
			sLog->outBasic("[QuestExp] - Error - GetSendQuestInfo *** : [%s][%s] QuestIndexID[0x%x]", getGameObject(iObjIndex)->AccountID, getGameObject(iObjIndex)->Name, dwQuestInfoIndexID);
			return false;
		}

		if (IsQuestRewardInfo(iReward) == false)
		{
			sLog->outBasic("[QuestExp] - Error - iReward [%d] [%s] [%d]", iReward, __FILE__, __LINE__);
			return false;
		}

		int iRewardType = pQuestExpReward->GetRewardType();

		if (pRewardKind)
		{
			for (int i = 0; i < MAX_QUESTEXP_REWARDKIND_RANDOM; ++i)
			{
				if (iArrRewardIndex[i] == pQuestExpReward->GetIndexID())
				{
					iRewardType |= 0x20;
				}
			}
		}

		switch (iRewardType & 0xFFFFFFDF)
		{
		case QUESTEXP_REWARD_EXP:
		{
			RewardExp* reward_exp = static_cast<RewardExp*>(pQuestExpReward);

			pNpcQuestExpInfo->NpcQuestRewardInfo[iReward].dwAskType = iRewardType;
			pNpcQuestExpInfo->NpcQuestRewardInfo[iReward].wIndex = 0;
			pNpcQuestExpInfo->NpcQuestRewardInfo[iReward].dwValue = reward_exp->GetRewardExp();
		}
		break;

		case QUESTEXP_REWARD_ZEN:
		{
			RewardZen* pRewardZen = static_cast<RewardZen*>(pQuestExpReward);

			pNpcQuestExpInfo->NpcQuestRewardInfo[iReward].dwAskType = iRewardType;
			pNpcQuestExpInfo->NpcQuestRewardInfo[iReward].wIndex = 0;
			pNpcQuestExpInfo->NpcQuestRewardInfo[iReward].dwValue = pRewardZen->GetRewardZen();
		}
		break;

		case QUESTEXP_REWARD_ITEM:
		{
			RewardItem* pRewardItem = static_cast<RewardItem*>(pQuestExpReward);

			pNpcQuestExpInfo->NpcQuestRewardInfo[iReward].dwAskType = iRewardType;
			pNpcQuestExpInfo->NpcQuestRewardInfo[iReward].wIndex = pRewardItem->GetRewardItemNum();
			pNpcQuestExpInfo->NpcQuestRewardInfo[iReward].dwValue = pRewardItem->GetRewardItemCnt();

			CItemObject Item;
			Item.m_Type = pRewardItem->GetRewardItemNum();
			Item.m_Level = pRewardItem->GetRewardItemLevel();
			Item.m_Option1 = pRewardItem->GetRewardItemSkill();
			Item.m_Durability = pRewardItem->GetRewardItemDur();
			Item.m_NewOption = pRewardItem->GetRewardItemExOpt();
			Item.m_Option3 = pRewardItem->GetRewardItemOpt();

			ItemByteConvert(pNpcQuestExpInfo->NpcQuestRewardInfo[iReward].btItemInfo, Item);

			if (pRewardItem->GetRewardItemOverlap() == true)
			{
				int iItemCnt = pRewardItem->GetRewardItemCnt();
				int iItemDur = pRewardItem->GetRewardItemDur();
				pNpcQuestExpInfo->NpcQuestRewardInfo[iReward].dwValue = iItemDur * iItemCnt;
			}

		}
		break;

		case QUESTEXP_REWARD_CONTRIBUTION_POINT:
		{
			RewardContribute* pRewardContribute = static_cast<RewardContribute*>(pQuestExpReward);

			pNpcQuestExpInfo->NpcQuestRewardInfo[iReward].dwAskType = iRewardType;
			pNpcQuestExpInfo->NpcQuestRewardInfo[iReward].wIndex = 0;
			pNpcQuestExpInfo->NpcQuestRewardInfo[iReward].dwValue = pRewardContribute->GetRewardContributePoint();
		}
		break;

		default:
			sLog->outBasic("[QuestExp] - Error - Unknown Reward Type %d %s %d", iRewardType, __FILE__, __LINE__);
			return false;
		}

		++quest_reward_it;
		iReward++;
	}

	return true;
}

void QuestExpProgMng::ChkUserQuestTypeBuff(QuestBuff* pQuestBuff, UserQuestAskInfo* pUserQuestAskInfo, int iObjIndex)
{
	int iBuffIndex = pQuestBuff->GetQuestBuffIndex();

	if (::gObjCheckUsedBuffEffect(&getGameObject(iObjIndex), iBuffIndex))
	{
		pUserQuestAskInfo->SetValue(1);
		pUserQuestAskInfo->SetComplete(true);
	}
	else
	{
		pUserQuestAskInfo->SetValue(0);
		pUserQuestAskInfo->SetComplete(false);
	}
}

void QuestExpProgMng::ChkUserQuestTypeNeedZen(QuestNeedZen* pQuestNeedZen, UserQuestAskInfo* pUserQuestAskInfo, int iObjIndex)
{
	int iZen = getGameObject(iObjIndex)->m_PlayerData->Money;

	if (iZen < pQuestNeedZen->GetQuestNeedZen())
	{
		pUserQuestAskInfo->SetValue(iZen);
		pUserQuestAskInfo->SetComplete(false);
	}
	else
	{
		pUserQuestAskInfo->SetValue(iZen);
		pUserQuestAskInfo->SetComplete(true);
	}
}

void QuestExpProgMng::ChkUserQuestTypeSkillLearn(QuestSkillLearn* pQuestSkillLearn, UserQuestAskInfo* pUserQuestAskInfo, int iObjIndex)
{
	int iSkillIndex = pQuestSkillLearn->GetQuestSkillIndex();

	for (int n = 0; n < MAX_MAGIC; n++)
	{
		if (getGameObject(iObjIndex)->Magic[n)->IsMagic())
		{
			if (getGameObject(iObjIndex)->Magic[n)->m_Skill == iSkillIndex)
			{
				if (!pUserQuestAskInfo->IsComplete())
				{
					pUserQuestAskInfo->SetComplete(true);
					pUserQuestAskInfo->SetValue(1);
				}
			}
		}
	}
}

void QuestExpProgMng::ChkUserQuestTypeLevelUp(QuestLevelUp* pQuestLevelUp, UserQuestAskInfo* pUserQuestAskInfo, int iObjIndex)
{
	int iUserLevel = getGameObject(iObjIndex)->Level;

	if (pQuestLevelUp->GetQuestLevelVal() > iUserLevel)
	{
		pUserQuestAskInfo->SetComplete(false);
		pUserQuestAskInfo->SetValue(iUserLevel);
	}
	else
	{
		pUserQuestAskInfo->SetComplete(true);
		pUserQuestAskInfo->SetValue(iUserLevel);
	}
}

void QuestExpProgMng::DeleteInventoryItem(QuestGetItem* pQuestGetItem, int iObjIndex)
{
	if (ObjectMaxRange(iObjIndex) == false || !gObjIsConnected(iObjIndex))
	{
		return;
	}

	int iItemCnt = pQuestGetItem->GetQuestItemCnt();
	int iItemNum = pQuestGetItem->GetQuestItemNum();
	int iItemLevel = pQuestGetItem->GetQuestItemLevel();
	int iItemOpt = pQuestGetItem->GetQuestItemOpt();
	int iItemExOpt = pQuestGetItem->GetQuestItemExOpt();
	int iItemSkill = pQuestGetItem->GetQuestItemSkill();

	for (int x = 0; x < INVENTORY_SIZE; ++x)
	{
		if (!getGameObject(iObjIndex)->pInventory[x)->IsItem())
		{
			continue;
		}

		if (iItemCnt <= 0)
			return;

		if (getGameObject(iObjIndex)->pInventory[x)->m_Type != iItemNum)
		{
			continue;
		}

		if (getGameObject(iObjIndex)->pInventory[x)->m_Level != iItemLevel)
		{
			continue;
		}

		if ((getGameObject(iObjIndex)->pInventory[x)->m_Type < ITEMGET(14, 0)
			|| getGameObject(iObjIndex)->pInventory[x)->m_Type > ITEMGET(14, 8))
			&& (getGameObject(iObjIndex)->pInventory[x)->m_Type < ITEMGET(14, 35)
				|| getGameObject(iObjIndex)->pInventory[x)->m_Type > ITEMGET(14, 40))
			&& g_QuestExpManager.IsQuestItemAtt(getGameObject(iObjIndex)->pInventory[x)->m_Type, QUESTEXP_ITEM_OVERLAP) != true)
		{
			if (getGameObject(iObjIndex)->pInventory[x)->m_Option1 != iItemSkill)
			{
				continue;
			}

			if (getGameObject(iObjIndex)->pInventory[x)->m_Option3 != iItemOpt)
			{
				continue;
			}

			if (getGameObject(iObjIndex)->pInventory[x)->m_NewOption != iItemExOpt)
			{
				continue;
			}

			if (getGameObject(iObjIndex)->pInventory[x)->m_Option2 != 0)
			{
				continue;
			}

			if (getGameObject(iObjIndex)->pInventory[x)->m_SetOption != 0)
			{
				continue;
			}

			BYTE NewOption[MAX_EXOPTION_SIZE] = { 0 };
			ItemIsBufExOption(NewOption, &getGameObject(iObjIndex)->pInventory[x));

			/*sLog->outBasic("[QuestExp] DeleteInvenItem [%s][%s] Delete Item Info - Item:[%s,%d,%d,%d,%d] serial:[%I64d][%d] Ex:[%d,%d,%d,%d,%d,%d,%d] Set[%d] 380:[%d] HO:[%d,%d] SC[%d,%d,%d,%d,%d] BonusOption[%d]",
				getGameObject(iObjIndex)->AccountID, getGameObject(iObjIndex)->Name, getGameObject(iObjIndex)->pInventory[x]->GetName(), getGameObject(iObjIndex)->pInventory[x)->m_Level, getGameObject(iObjIndex)->pInventory[x)->m_Option1,
				getGameObject(iObjIndex)->pInventory[x]->m_Option2, getGameObject(iObjIndex)->pInventory[x]->m_Option3, getGameObject(iObjIndex)->pInventory[x)->m_Number, (int)getGameObject(iObjIndex)->pInventory[x)->m_Durability,
				NewOption[0], NewOption[1], NewOption[2], NewOption[3], NewOption[4], NewOption[5], NewOption[6], getGameObject(iObjIndex)->pInventory[x)->m_SetOption,
				getGameObject(iObjIndex)->pInventory[x]->m_ItemOptionEx >> 7, g_kJewelOfHarmonySystem.GetItemStrengthenOption(&getGameObject(iObjIndex)->pInventory[x)),
				g_kJewelOfHarmonySystem.GetItemOptionLevel(&getGameObject(iObjIndex)->pInventory[x]), getGameObject(iObjIndex)->pInventory[x)->m_SocketOption[0),
				getGameObject(iObjIndex)->pInventory[x]->m_SocketOption[1], getGameObject(iObjIndex)->pInventory[x]->m_SocketOption[2], getGameObject(iObjIndex)->pInventory[x)->m_SocketOption[3),
				getGameObject(iObjIndex)->pInventory[x]->m_SocketOption[4], getGameObject(iObjIndex)->pInventory[x)->m_BonusSocketOption);*/

			gObjInventoryItemSet(iObjIndex, x, -1);
			getGameObject(iObjIndex)->pInventory[x)->Clear();
			gGameProtocol.GCInventoryItemDeleteSend(iObjIndex, x, 1);

			iItemCnt--;
		}
		else
		{
			if (gObjSearchItemMinus(&getGameObject(iObjIndex), x, 1))
			{
				x--;
			}

			else
			{
				BYTE NewOption[MAX_EXOPTION_SIZE] = { 0 };
				ItemIsBufExOption(NewOption, &getGameObject(iObjIndex)->pInventory[x));

				//sLog->outBasic("[QuestExp] DeleteInvenItem [%s][%s] Delete Item Info - Item:[%s,%d,%d,%d,%d] serial:[%I64d][%d] Ex:[%d,%d,%d,%d,%d,%d,%d] Set[%d] 380:[%d] HO:[%d,%d] SC[%d,%d,%d,%d,%d] BonusOption[%d]",
				//	getGameObject(iObjIndex)->AccountID, getGameObject(iObjIndex)->Name, getGameObject(iObjIndex)->pInventory[x]->GetName(), getGameObject(iObjIndex)->pInventory[x)->m_Level, getGameObject(iObjIndex)->pInventory[x)->m_Option1,
				//	getGameObject(iObjIndex)->pInventory[x]->m_Option2, getGameObject(iObjIndex)->pInventory[x]->m_Option3, getGameObject(iObjIndex)->pInventory[x)->m_Number, (int)getGameObject(iObjIndex)->pInventory[x)->m_Durability,
				//	NewOption[0], NewOption[1], NewOption[2], NewOption[3], NewOption[4], NewOption[5], NewOption[6], getGameObject(iObjIndex)->pInventory[x)->m_SetOption,
				//	getGameObject(iObjIndex)->pInventory[x]->m_ItemOptionEx >> 7, g_kJewelOfHarmonySystem.GetItemStrengthenOption(&getGameObject(iObjIndex)->pInventory[x)),
				//	g_kJewelOfHarmonySystem.GetItemOptionLevel(&getGameObject(iObjIndex)->pInventory[x]), getGameObject(iObjIndex)->pInventory[x)->m_SocketOption[0),
				//	getGameObject(iObjIndex)->pInventory[x]->m_SocketOption[1], getGameObject(iObjIndex)->pInventory[x]->m_SocketOption[2], getGameObject(iObjIndex)->pInventory[x)->m_SocketOption[3),
				//	getGameObject(iObjIndex)->pInventory[x]->m_SocketOption[4], getGameObject(iObjIndex)->pInventory[x)->m_BonusSocketOption);

				gObjInventoryItemSet(iObjIndex, x, -1);
				getGameObject(iObjIndex)->pInventory[x)->Clear();
				gGameProtocol.GCInventoryItemDeleteSend(iObjIndex, x, 1);
			}

			iItemCnt--;
		}
	}
}

void QuestExpProgMng::ChkUserQuestTypeItem(QuestGetItem* pQuestGetItem, UserQuestAskInfo* pUserQuestAskInfo, int iObjIndex)
{
	int iItemCnt = 0;
	int iItemDur = 0;
	int iItemNum = pQuestGetItem->GetQuestItemNum();
	int iItemLevel = pQuestGetItem->GetQuestItemLevel();
	int iItemOpt = pQuestGetItem->GetQuestItemOpt();
	int iItemExOpt = pQuestGetItem->GetQuestItemExOpt();
	int iItemSkill = pQuestGetItem->GetQuestItemSkill();

	for (int x = 0; x < INVENTORY_SIZE; ++x)
	{
		if (!getGameObject(iObjIndex)->pInventory[x)->IsItem())
		{
			continue;
		}

		if (getGameObject(iObjIndex)->pInventory[x)->m_Type != iItemNum)
		{
			continue;
		}

		if (getGameObject(iObjIndex)->pInventory[x)->m_Level != iItemLevel)
		{
			continue;
		}

		if ((getGameObject(iObjIndex)->pInventory[x]->m_Type < ITEMGET(14, 0) || getGameObject(iObjIndex)->pInventory[x)->m_Type > ITEMGET(14, 8))
			&& (getGameObject(iObjIndex)->pInventory[x]->m_Type < ITEMGET(14, 35) || getGameObject(iObjIndex)->pInventory[x)->m_Type > ITEMGET(14, 40))
			&& g_QuestExpManager.IsQuestItemAtt(getGameObject(iObjIndex)->pInventory[x)->m_Type, QUESTEXP_ITEM_OVERLAP) != true)
		{
			if (getGameObject(iObjIndex)->pInventory[x)->m_Option1 == iItemSkill)
			{
				if (getGameObject(iObjIndex)->pInventory[x)->m_Option3 == iItemOpt)
				{
					if (getGameObject(iObjIndex)->pInventory[x)->m_NewOption == iItemExOpt)
					{
						if (!getGameObject(iObjIndex)->pInventory[x)->m_Option2)
						{
							if (!getGameObject(iObjIndex)->pInventory[x)->m_SetOption)
								iItemCnt++;
						}
					}
				}
			}
		}
		else
		{
			iItemDur += getGameObject(iObjIndex)->pInventory[x)->m_Durability;
		}
	}

	if (iItemDur > 0)
	{
		iItemCnt = iItemDur;
	}

	pUserQuestAskInfo->SetValue(iItemCnt);

	if (pQuestGetItem->GetQuestItemCnt() > iItemCnt)
	{
		pUserQuestAskInfo->SetComplete(false);
	}

	else
	{
		if (!pUserQuestAskInfo->IsComplete())
		{
			pUserQuestAskInfo->SetComplete(true);
		}
	}
}

void QuestExpProgMng::ChkMonsterKillPartyPlay(DWORD dwQuestInfoIndexID, int iAskIndex, QuestMonsterKill* pQuestMonsterKill, CGameObject &Obj, CGameObject &MonsterObj)
{
	if (!pQuestMonsterKill)
	{
		return;
	}

	int iEpisode = GetQuestEpisodeFromInfoIndexId(dwQuestInfoIndexID);
	int iQS = GetQuestSwitchFromInfoIndexId(dwQuestInfoIndexID);

	if (lpObj.m_PlayerData->m_UserQuestInfo[iEpisode].GetEpisode() != iEpisode)
	{
		return;
	}

	if (lpObj.m_PlayerData->m_UserQuestInfo[iEpisode].GetQuestSwitch() != iQS)
	{
		return;
	}

	UserQuestAskInfo* pUserQuestAskInfo = &lpObj.m_PlayerData->m_UserQuestInfo[iEpisode].m_UserQuestAskInfo[iAskIndex];

	if (pUserQuestAskInfo->GetQuestType() != QUESTEXP_ASK_MONSTER)
	{
		return;
	}

	if (lpMonsterObj.Class != pQuestMonsterKill->GetMonsterIndex())
	{
		return;
	}

	int iKillCnt = pQuestMonsterKill->GetMonsterKillCnt();

	if (iKillCnt > pUserQuestAskInfo->GetValue())
	{
		pUserQuestAskInfo->SetValue(pUserQuestAskInfo->GetValue() + 1);

		//sLog->outBasic("[QuestExp] Party Play Ask Kill Monster [%s][%s] Ep[%d] QS[%d] MonName[%s] AskKillCnt[%d] KillCnt[%d] ",
		//	lpObj.AccountID, lpObj.Name, iEpisode, iQS, lpMonsterObj.Name, pUserQuestAskInfo->GetValue(), pQuestMonsterKill->GetMonsterKillCnt());
	}

	if (pQuestMonsterKill->GetMonsterKillCnt() <= pUserQuestAskInfo->GetValue())
	{
		if (!pUserQuestAskInfo->IsComplete())
		{
			pUserQuestAskInfo->SetComplete(true);
		}
	}
}

void QuestExpProgMng::ChkUserQuestTypeMonsterKill(CGameObject lpObj, CGameObject lpMonsterObj)
{
	if (lpObj.PartyNumber == -1)
	{
		ChkUserQuestType(QUESTEXP_ASK_MONSTER, lpObj, lpMonsterObj, 0);
		return;
	}

	int iPartyNumber = lpObj.PartyNumber;
	int iKillerObjIndex = lpObj.m_Index;

	for (int iPartyUserCount = 0; iPartyUserCount < MAX_USER_IN_PARTY; ++iPartyUserCount)
	{
		int iPartyUserIndex = gParty.m_PartyS[iPartyNumber].Number[iPartyUserCount];
		if (iPartyUserIndex >= 0)
		{
			CGameObject* lpPartyObj = &getGameObject(iPartyUserIndex);

			if (getGameObject(iPartyUserIndex)->Connected >= PLAYER_PLAYING || !lpPartyObj.Live)
			{
				if (lpObj.MapNumber == lpPartyObj.MapNumber)
				{
					if (gObjCalDistance(lpObj, lpPartyObj) <= 9)
					{
						ChkUserQuestType(QUESTEXP_ASK_MONSTER, lpPartyObj, lpMonsterObj, iKillerObjIndex);
					}
				}
			}
		}
	}
}

void QuestExpProgMng::ChkUserQuestType(int iQuestType, CGameObject lpObj, CGameObject lpMonsterObj, int iKillerObjIndex)
{
	if (!lpMonsterObj)
	{
		return;
	}

	for (int i = 0; i < MAX_QUESTEXP_USER_INFO; i++)
	{
		if (lpObj.m_PlayerData->m_UserQuestInfo[i].GetEpisode() == 0)
		{
			continue;
		}

		UserQuestInfo* pUserQuestInfo = &lpObj.m_PlayerData->m_UserQuestInfo[i];
		int iAskCnt = pUserQuestInfo->GetAskCnt();

		for (int j = 0; j < iAskCnt; j++)
		{
			if (iQuestType != pUserQuestInfo->m_UserQuestAskInfo[j].GetQuestType())
			{
				continue;
			}

			UserQuestAskInfo* pUserQuestAskInfo = &pUserQuestInfo->m_UserQuestAskInfo[j];
			int iIndexID = pUserQuestAskInfo->GetIndexID();
			int iEpisode = pUserQuestInfo->GetEpisode();
			int iQS = pUserQuestInfo->GetQuestSwitch();

			QuestExpAsk* pQuestExpAsk = g_QuestExpManager.GetQuestExpAsk(iEpisode, iQS, iIndexID);

			if (!pQuestExpAsk)
				return;

			if (iQuestType == QUESTEXP_ASK_MONSTER)
			{
				QuestMonsterKill* pQuestMonsterKill = static_cast<QuestMonsterKill*>(pQuestExpAsk);

				if (lpMonsterObj.Class != pQuestMonsterKill->GetMonsterIndex())
				{
					continue;
				}

				if (pQuestMonsterKill->GetMonsterKillCnt() > pUserQuestAskInfo->GetValue())
				{
					pUserQuestAskInfo->SetValue(pUserQuestAskInfo->GetValue() + 1);

					if (!pQuestMonsterKill->IsPartyPlay() || lpObj.PartyNumber == -1)
					{
						//sLog->outBasic("[QuestExp] Ask Kill Monster [%s][%s] Ep[%d] QS[%d] MonName[%s] AskKillCnt[%d] KillCnt[%d] ",
						//	lpObj.AccountID, lpObj.Name, iEpisode, iQS, lpMonsterObj.Name, pQuestMonsterKill->GetMonsterKillCnt(), pUserQuestAskInfo->GetValue());
					}

					else
					{
						//sLog->outBasic("[QuestExp] Party Play Ask Kill Monster [%s][%s] Ep[%d] QS[%d] MonName[%s] AskKillCnt[%d] KillCnt[%d] MonsterKiller[%s][%s]",
						//	lpObj.AccountID, lpObj.Name, iEpisode, iQS, lpMonsterObj.Name, pQuestMonsterKill->GetMonsterKillCnt(), pUserQuestAskInfo->GetValue(),
						//	getGameObject(iKillerObjIndex)->AccountID, getGameObject(iKillerObjIndex)->Name);
					}
				}

				if (pQuestMonsterKill->GetMonsterKillCnt() <= pUserQuestAskInfo->GetValue())
				{
					if (!pUserQuestAskInfo->IsComplete())
					{
						pUserQuestAskInfo->SetComplete(true);
					}
				}
			}
			else if (iQuestType == QUESTEXP_ASK_NPC_TALK)
			{
				QuestNpcTalk* pQuestNpcTalk = static_cast<QuestNpcTalk*>(pQuestExpAsk);

				if (pQuestNpcTalk->GetQuestNPCTalk())
				{
					if (lpMonsterObj.Class == pQuestNpcTalk->GetQuestNPCTalk())
					{
						if (!pUserQuestAskInfo->IsComplete())
						{
							pUserQuestAskInfo->SetValue(1);
							pUserQuestAskInfo->SetComplete(true);
						}
					}
				}
				else
				{
					if (lpMonsterObj.Class == 543 && g_GensSystem.GetGensInfluence(lpObj) == DUPRIAN_INFLUENCE
						|| lpMonsterObj.Class == 544 && g_GensSystem.GetGensInfluence(lpObj) == VANERT_INFLUENCE)
					{
						if (!pUserQuestAskInfo->IsComplete())
						{
							pUserQuestAskInfo->SetValue(1);
							pUserQuestAskInfo->SetComplete(true);
						}
					}
				}
			}

			this->SendQuestAskInfoUpdate(iEpisode, iQS, iQuestType, pUserQuestAskInfo->GetValue(), lpObj.m_Index);
		}
	}
}

void QuestExpProgMng::ChkUserQuestTypeEventMap(int iQuestType, CGameObject &Obj, int iMapLevel, int iValue)
{
	if (lpObj.Type != OBJ_USER)
	{
		return;
	}

	for (int i = 0; i < MAX_QUESTEXP_USER_INFO; i++)
	{
		if (lpObj.m_PlayerData->m_UserQuestInfo[i].GetEpisode() == 0)
		{
			continue;
		}

		UserQuestInfo* pUserQuestInfo = &lpObj.m_PlayerData->m_UserQuestInfo[i];
		int iAskCnt = pUserQuestInfo->GetAskCnt();

		for (int j = 0; j < iAskCnt; j++)
		{
			if (iQuestType != pUserQuestInfo->m_UserQuestAskInfo[j].GetQuestType())
			{
				continue;
			}

			UserQuestAskInfo* pUserQuestAskInfo = &pUserQuestInfo->m_UserQuestAskInfo[j];
			int iIndexID = pUserQuestAskInfo->GetIndexID();
			int iEpisode = pUserQuestInfo->GetEpisode();
			int iQS = pUserQuestInfo->GetQuestSwitch();

			QuestExpAsk* pQuestExpAsk = g_QuestExpManager.GetQuestExpAsk(iEpisode, iQS, iIndexID);

			switch (iQuestType)
			{
			case QUESTEXP_ASK_CHAOSCASTLE_USER_KILL:
			case QUESTEXP_ASK_CHAOSCASTLE_MON_KILL:
			case QUESTEXP_ASK_BLOODCASTLE_DOOR_KILL:
			{
				QuestEventMapKillPoint* pQuestEventMapKillPoint = static_cast<QuestEventMapKillPoint*>(pQuestExpAsk);

				if (pQuestEventMapKillPoint->GetQuestEventMapLevel() - 1 == iMapLevel)
				{
					int iKillCnt = pQuestEventMapKillPoint->GetQuestEventMapKillCnt();

					if (iKillCnt > pUserQuestAskInfo->GetValue())
					{
						pUserQuestAskInfo->SetValue(pUserQuestAskInfo->GetValue() + 1);

						//sLog->outBasic("[QuestExp] Ask Kill User [%s][%s] Ep[%d] QS[%d] RequestType[0x08%X] MapLevel[%d] AskKillCnt[%d] KillCnt[%d]",
						//	lpObj.AccountID, lpObj.Name, iEpisode, iQS, iQuestType, iMapLevel, pQuestEventMapKillPoint->GetQuestEventMapKillCnt(), pUserQuestAskInfo->GetValue());
					}
				}

				if (pQuestEventMapKillPoint->GetQuestEventMapKillCnt() <= pUserQuestAskInfo->GetValue())
				{
					if (!pUserQuestAskInfo->IsComplete())
					{
						pUserQuestAskInfo->SetComplete(true);
					}
				}
			}
			break;

			case QUESTEXP_ASK_BLOODCASTLE_CLEAR:
			case QUESTEXP_ASK_CHAOSCASTLE_CLEAR:
			case QUESTEXP_ASK_DEVILSQUARE_CLEAR:
			case QUESTEXP_ASK_ILLUSIONTEMPLE_CLEAR:
			{
				QuestEventMapClear* pQuestEventMapClear = static_cast<QuestEventMapClear*>(pQuestExpAsk);

				if (pQuestEventMapClear->GetQuestEventMapLevel() - 1 == iMapLevel)
				{
					if (!pUserQuestAskInfo->IsComplete())
					{
						pUserQuestAskInfo->SetComplete(true);
						pUserQuestAskInfo->SetValue(1);

						//sLog->outBasic("[QuestExp] Ask Event Map Clear - Complete - [%s][%s] Ep[%d] QS[%d] QuestType[0x%08X]",
						//	lpObj.AccountID, lpObj.Name, iEpisode, iQS, iQuestType);
					}
				}
			}
			break;

			case QUESTEXP_ASK_DEVILSQUARE_POINT_GAIN:
			{
				QuestEventMapDevilPoint* pQuestEventMapDevilPoint = static_cast<QuestEventMapDevilPoint*>(pQuestExpAsk);

				if (pQuestEventMapDevilPoint->GetQuestEventMapLevel() - 1 == iMapLevel)
				{
					int iDevilPoint = pQuestEventMapDevilPoint->GetQuestEventMapDevilPoint();

					if (iDevilPoint > pUserQuestAskInfo->GetValue())
					{
						pUserQuestAskInfo->SetValue(iValue + pUserQuestAskInfo->GetValue());

						//sLog->outBasic("[QuestExp] Ask Devil Point [%s][%s] Ep[%d] QS[%d] RequestType[0x%08X] MapLevel[%d] AskDevilPoint[%d] CurDevilPoint[%d]",
						//	lpObj.AccountID, lpObj.Name, iEpisode, iQS, iQuestType, iMapLevel, pQuestEventMapDevilPoint->GetQuestEventMapDevilPoint(), pUserQuestAskInfo->GetValue());
					}
				}

				if (pQuestEventMapDevilPoint->GetQuestEventMapDevilPoint() <= pUserQuestAskInfo->GetValue())
				{
					if (!pUserQuestAskInfo->IsComplete())
					{
						pUserQuestAskInfo->SetComplete(true);
					}
				}
			}
			break;

			case QUESTEXP_ASK_CONTRIBUTION_POINT:
			{
				QuestPVP_Point* iDevilPoint = static_cast<QuestPVP_Point*>(pQuestExpAsk);

				if (iDevilPoint->GetQuestMapNum() == iMapLevel)
				{
					if (iDevilPoint->GetQuestPVP_Point() > pUserQuestAskInfo->GetValue())
					{
						pUserQuestAskInfo->SetValue(pUserQuestAskInfo->GetValue() + 1);

						//sLog->outBasic("[QuestExp] Ask PVP [%s][%s] Ep[%d] QS[%d] RequestType[0x08%X] MapNum[%d] AskKillCnt[%d] KillCnt[%d]",
						//	lpObj.AccountID, lpObj.Name, iEpisode, iQS, iQuestType, iMapLevel, iDevilPoint->GetQuestPVP_Point(), pUserQuestAskInfo->GetValue());
					}
				}

				if (iDevilPoint->GetQuestPVP_Point() <= pUserQuestAskInfo->GetValue())
				{
					if (!pUserQuestAskInfo->IsComplete())
					{
						pUserQuestAskInfo->SetComplete(true);
					}
				}
			}
			break;

			}

			this->SendQuestAskInfoUpdate(iEpisode, iQS, iQuestType, pUserQuestAskInfo->GetValue(), lpObj.m_Index);
		}
	}
}

void QuestExpProgMng::AddQuestSwitchList(int iEpisode, int iQS)
{
	if (IsEpisode(iEpisode) == false)
	{
		sLog->outBasic("[QuestExp] - Error - [%s] [%d]", __FILE__, __LINE__);
		return;
	}

	if (iQS >= MAX_QUESTEXP_SWITCH - 1)
	{
		return;
	}

	DWORD dwQuestInfoIndexID = GetQuestInfoIndexId(iEpisode, iQS);
	this->m_vtQuestListNpcTalk.push_back(dwQuestInfoIndexID);
}

struct PMSG_QUESTSWITCH_LIST
{
	PBMSG_HEAD2 h;
	WORD wNPCIndex;
	WORD wQuestCnt;
};

void QuestExpProgMng::SendQuestSwitchList(int iObjIndex, int iSendType)
{
	BYTE sendBuff[1024];
	PMSG_QUESTSWITCH_LIST QuestList;
	int lOfs = sizeof(QuestList);
	QuestList.wQuestCnt = this->m_vtQuestListNpcTalk.size();
	int iIndex = iObjIndex;

	auto it = this->m_vtQuestListNpcTalk.begin();
	auto end = this->m_vtQuestListNpcTalk.end();

	for (; it != end; it++)
	{
		DWORD dwQuestInfoIndexID = *it;

		memcpy(&sendBuff[lOfs], &dwQuestInfoIndexID, sizeof(dwQuestInfoIndexID));
		lOfs += sizeof(dwQuestInfoIndexID);

		int iEpisode = GetQuestEpisodeFromInfoIndexId(dwQuestInfoIndexID);
		int iQS = GetQuestSwitchFromInfoIndexId(dwQuestInfoIndexID);

		/*if (iSendType == QUESTEXP_SEND_NPC)
		{
			sLog->outBasic("[QuestExp] Send Quest List To NPC: [%s][%s] Ep[%d] QS[%d] QuestListCnt[%d]",	Obj.AccountID, Obj.Name, iEpisode, iQS, QuestList.wQuestCnt);
		}
		else if (iSendType == QUESTEXP_SEND_EVENT)
		{
			sLog->outBasic("[QuestExp] Send Quest List To Event: [%s][%s] Ep[%d] QS[%d] QuestListCnt[%d]",Obj.AccountID, Obj.Name, iEpisode, iQS, QuestList.wQuestCnt);
		}
		else if (iSendType == QUESTEXP_SEND_ITEM)
		{
			sLog->outBasic("[QuestExp] Send Quest List Item Used: [%s][%s] Ep[%d] QS[%d] QuestListCnt[%d]",Obj.AccountID, Obj.Name, iEpisode, iQS, QuestList.wQuestCnt);
		}*/
	}

	QuestList.wNPCIndex = 0;

	if (iSendType == QUESTEXP_SEND_NPC)
	{
		PHeadSubSetB(reinterpret_cast<BYTE *>(&QuestList), 0xF6, 0x0A, 0);
		QuestList.wNPCIndex = getGameObject(Obj.TargetNpcNumber)->Class;
	}

	else if (iSendType == QUESTEXP_SEND_EVENT)
	{
		PHeadSubSetB(reinterpret_cast<BYTE *>(&QuestList), 0xF6, 0x03, 0);
	}

	else if (iSendType == QUESTEXP_SEND_ITEM)
	{
		PHeadSubSetB(reinterpret_cast<BYTE *>(&QuestList), 0xF6, 0x04, 0);
	}

	QuestList.h.size = lOfs;
	memcpy(sendBuff, &QuestList, sizeof(QuestList));
	IOCP.DataSend(Obj.m_Index, static_cast<BYTE*>(sendBuff), lOfs);

	this->m_vtQuestListNpcTalk.erase(this->m_vtQuestListNpcTalk.begin(), this->m_vtQuestListNpcTalk.end());
}

struct PMSG_QUEST_ASK_STATE
{
	PBMSG_HEAD2 h;
	DWORD dwQuestInfoIndexID;
	BYTE btAskIndex;
	WORD wCurrentValue;
};

void QuestExpProgMng::SendQuestAskState(PMSG_QUEST_ASK_STATE* pMsg, int iObjIndex)
{
	PHeadSubSetB((BYTE *)pMsg, 0xF6, 0x0E, sizeof(pMsg));
	IOCP.DataSend(iObjIndex, (BYTE*)pMsg, pMsg->h.size);
}

struct PMSG_ANS_QUESTEXP_COMPLETE
{
    PBMSG_HEAD2 h;
	DWORD dwQuestInfoIndexID;
	BYTE btResult;
};

void QuestExpProgMng::GCANSQuestCompleteBtnClick(int iObjIndex, DWORD dwQuestInfoIndexID, BYTE btResult)
{
	if (ChkQuestIndexIDToEpAndQSLimit(dwQuestInfoIndexID, iObjIndex) == false)
	{
		sLog->outBasic("[QuestExp] - Error - [%s] [%d]", __FILE__, __LINE__);
		return;
	}

	PMSG_ANS_QUESTEXP_COMPLETE pMsg;

	pMsg.dwQuestInfoIndexID = dwQuestInfoIndexID;
	pMsg.btResult = btResult;
	PHeadSubSetB((BYTE *)&pMsg, 0xF6, 0x0D, sizeof(pMsg));

	//sLog->outBasic("[QuestExp] Ans Complete [%s][%s] Ep [%d] QS [%d] Result [%d]", getGameObject(iObjIndex)->Name, getGameObject(iObjIndex)->AccountID, GetQuestEpisodeFromInfoIndexId(dwQuestInfoIndexID), GetQuestSwitchFromInfoIndexId(dwQuestInfoIndexID), btResult);
	IOCP.DataSend(iObjIndex, (BYTE*)&pMsg, pMsg.h.size);
}

#pragma pack(push, 1)
struct PMSG_QUESTPROGRESS_LIST
{
    PBMSG_HEAD2 h;
	BYTE btQuestCnt;
};
#pragma pack(pop)

void QuestExpProgMng::SendProgressQuestList(int iObjIndex)
{
	BYTE sendBuff[1024] = { 0 };
	BYTE btQuestCnt = 0;
	PMSG_QUESTPROGRESS_LIST pMsg;
	int lOfs = sizeof(pMsg);

	for (int i = 0; i < MAX_QUESTEXP_USER_INFO; ++i)
	{
		int iEpisode = getGameObject(iObjIndex)->m_PlayerData->m_UserQuestInfo[i)->GetEpisode();
		int iQS = getGameObject(iObjIndex)->m_PlayerData->m_UserQuestInfo[i)->GetQuestSwitch();
		WORD wProgState = getGameObject(iObjIndex)->m_PlayerData->m_UserQuestInfo[i)->GetQuestProgState();

		if (wProgState == 0)
			continue;

		if (iEpisode == 0)
			continue;

		if (iQS == 0)
			continue;

		if (iQS == 0xFE || iQS == 0xFF)
			continue;

		DWORD dwQuestInfoIndexID = GetQuestInfoIndexId(iEpisode, iQS);

		memcpy(&sendBuff[lOfs], &dwQuestInfoIndexID, sizeof(dwQuestInfoIndexID));
		lOfs += sizeof(dwQuestInfoIndexID);
		btQuestCnt++;

		//sLog->outBasic("[QuestExp] Send QuestProg List : [%s][%s] Ep[%d] QS[%d]",getGameObject(iObjIndex)->AccountID, getGameObject(iObjIndex)->Name, iEpisode, iQS);
	}

	pMsg.btQuestCnt = btQuestCnt;
	PHeadSubSetB((BYTE *)&pMsg, 0xF6, 0x1A, lOfs);
	memcpy(sendBuff, &pMsg, sizeof(pMsg));

	IOCP.DataSend(iObjIndex, (BYTE *)sendBuff, lOfs);


}

void QuestExpProgMng::SetQuestProg(int iEpisode, int iObjIndex, int iState)
{
	if (IsEpisode(iEpisode) == false)
	{
		sLog->outBasic("[QuestExp] - Error - SetQuestProg : [%s][%s] Ep[%d]", getGameObject(iObjIndex)->AccountID, getGameObject(iObjIndex)->Name, iEpisode);
		return;
	}

	if (getGameObject(iObjIndex)->m_PlayerData->m_UserQuestInfo[iEpisode)->GetQuestProgState() != QUESTEXP_PROG_STATE_TIME_LIMIT)
	{
		getGameObject(iObjIndex)->m_PlayerData->m_UserQuestInfo[iEpisode)->SetQuestProgState(iState);
	}
}

bool QuestExpProgMng::ChkQuestIndexIDToEpLimit(DWORD dwQuestInfoIndexID, int iObjIndex)
{
	int iEpisode = GetQuestEpisodeFromInfoIndexId(dwQuestInfoIndexID);

	if (IsEpisode(iEpisode) == false)
	{
		sLog->outBasic("[QuestExp] - Error - ChkEpisodeLimit : [%s][%s] Ep[%d] QuestIndexID[0x%x]", getGameObject(iObjIndex)->AccountID, getGameObject(iObjIndex)->Name, iEpisode, dwQuestInfoIndexID);
		return false;
	}

	return true;
}

bool QuestExpProgMng::ChkQuestIndexIDToQSLimit(DWORD dwQuestInfoIndexID, int iObjIndex)
{
	int iQS = GetQuestSwitchFromInfoIndexId(dwQuestInfoIndexID);

	if (IsQuestSwitch(iQS) == false)
	{
		sLog->outBasic("[QuestExp] - Error - ChkQuestSwitchLimit : [%s][%s] QS[%d] QuestIndexID[0x%x]", getGameObject(iObjIndex)->AccountID, getGameObject(iObjIndex)->Name, iQS, dwQuestInfoIndexID);
		return false;
	}

	return true;
}

bool QuestExpProgMng::ChkQuestIndexIDToEpAndQSLimit(DWORD dwQuestInfoIndexID, int iObjIndex)
{
	int iQS = GetQuestSwitchFromInfoIndexId(dwQuestInfoIndexID);
	int iEpisode = GetQuestEpisodeFromInfoIndexId(dwQuestInfoIndexID);

	if (!ObjectMaxRange(iObjIndex))
	{
		sLog->outBasic("[QuestExp] - Error - [%s] [%d]", __FILE__, __LINE__);

		return false;
	}

	if (!IsEpisode(iEpisode))
	{
		sLog->outBasic("[QuestExp] - Error - ChkEpAndQSLimit : [%s][%s] Ep[%d] QuestIndexID[0x%x]", getGameObject(iObjIndex)->AccountID, getGameObject(iObjIndex)->Name, iEpisode, dwQuestInfoIndexID);
		return false;
	}

	if (!IsQuestSwitch(iQS))
	{
		sLog->outBasic("[QuestExp] - Error - ChkEpAndQSLimit : [%s][%s] QS[%d] QuestIndexID[0x%x]", getGameObject(iObjIndex)->AccountID, getGameObject(iObjIndex)->Name, iQS, dwQuestInfoIndexID);
		return false;
	}

	return true;
}

void QuestExpProgMng::QuestMonsterItemDrop(DWORD dwQuestInfoIndexID, CGameObject lpObj, CGameObject lpMonsterObj)
{
	int iMaxHitUserIndex = gObjMonsterTopHitDamageUser(lpMonsterObj);

	if (iMaxHitUserIndex == -1)
		return;

	for (int i = 0; i < MAX_QUESTEXP_USER_INFO; i++)
	{
		int iEpisode = lpObj.m_PlayerData->m_UserQuestInfo[i].GetEpisode();
		int iQS = lpObj.m_PlayerData->m_UserQuestInfo[i].GetQuestSwitch();

		if (iEpisode == 0)
			continue;

		if (iQS == 0)
			continue;

		QuestDropItemInfo* pQuestDropItemInfo = g_QuestExpManager.GetQuestDropItemInfo(GetQuestInfoIndexId(iEpisode, iQS));

		if (!pQuestDropItemInfo)
			continue;

		if (pQuestDropItemInfo->GetMonsterIndex() != lpMonsterObj.Class)
			continue;

		if (ChkQuestMonsterItemDrop(pQuestDropItemInfo))
		{
			pQuestDropItemInfo->GetAskIndex();

			int iItemNum = pQuestDropItemInfo->GetItemNum();
			int iItemLevel = pQuestDropItemInfo->GetItemLevel();

			ItemCreate(lpObj.m_Index, lpObj.MapNumber, lpObj.X, lpObj.Y, iItemNum, iItemLevel, 0, 0, 0, 0, iMaxHitUserIndex, 0, 0, 0, 0, 0);
			//sLog->outBasic("[QuestExp] Monster Kill Quest Item Drop - MonsterName[%s]: [%s][%s] ItemNum[%d]", lpMonsterObj.Name, lpObj.AccountID, lpObj.Name, iItemNum);
		}
	}
}

bool QuestExpProgMng::ChkQuestMonsterItemDrop(QuestDropItemInfo* pQuestDropItemInfo)
{
	if (pQuestDropItemInfo == nullptr)
	{
		return false;
	}

	int iRandRate = pQuestDropItemInfo->GetDropRate();

	if ((rand() % 10000) < iRandRate)
	{
		return true;
	}

	else
	{
		return false;
	}
}

bool QuestExpProgMng::ChkQuestMonsterItemDrop(DWORD dwQuestInfoIndexID)
{
	QuestDropItemInfo* pQuestDropItemInfo = g_QuestExpManager.GetQuestDropItemInfo(dwQuestInfoIndexID);

	if (pQuestDropItemInfo == nullptr)
	{
		return false;
	}

	int iRandRate = pQuestDropItemInfo->GetDropRate();

	if ((rand() % 10000) < iRandRate)
	{
		return true;
	}

	else
	{
		return false;
	}
}

bool QuestExpProgMng::IsQuestDropItem(CGameObject &Obj, WORD nType, WORD nLevel)
{
	CGameObject* lpObj = Obj;

	for (int i = 0; i < MAX_QUESTEXP_USER_INFO; ++i)
	{
		int iEpisode = lpObj.m_PlayerData->m_UserQuestInfo[i].GetEpisode();
		int iQS = lpObj.m_PlayerData->m_UserQuestInfo[i].GetQuestSwitch();

		if (iEpisode == 0)
			continue;

		if (iQS == 0)
			continue;

		QuestDropItemInfo* pQuestDropItemInfo = g_QuestExpManager.GetQuestDropItemInfo(GetQuestInfoIndexId(iEpisode, iQS));

		if (!pQuestDropItemInfo)
			continue;

		UserQuestAskInfo* pUserQuestAskInfo = &lpObj.m_PlayerData->m_UserQuestInfo[i].m_UserQuestAskInfo[pQuestDropItemInfo->GetAskIndex() - 1];

		if (pQuestDropItemInfo->GetItemNum() == nType)
		{
			if (!ChkQuestAsk(iEpisode, iIndex))
			{
				sLog->outBasic("[QuestExp] - Error - [%s] [%d]", __FILE__, __LINE__);
				return false;
			}

			if (!pUserQuestAskInfo->IsComplete())
			{
				return true;
			}
		}
	}

	return false;
}

void QuestExpProgMng::SetQuestTimeLimit(int iObjIndex, DWORD dwQuestIndexID, int iDuration)
{
	CGameObject* lpObj = &getGameObject(iObjIndex);

	for (int i = 0; i < MAX_QUESTEXP_USER_INFO; i++)
	{
		int iEpisode = lpObj.m_PlayerData->m_UserQuestInfo[i].GetEpisode();
		int iQS = lpObj.m_PlayerData->m_UserQuestInfo[i].GetQuestSwitch();
		DWORD dwQuestInfoIndexID = GetQuestInfoIndexId(iEpisode, iQS);

		if (iEpisode && iQS && dwQuestInfoIndexID == dwQuestInfoIndexID)
		{
			lpObj.m_PlayerData->m_UserQuestInfo[i].SetStartDate(GetCurrentDate());
			lpObj.m_PlayerData->m_UserQuestInfo[i].SetEndDate(GetExpireDate(iDuration));
			lpObj.m_PlayerData->m_UserQuestInfo[i].SetQuestProgState(QUESTEXP_PROG_STATE_TIME_LIMIT);

			SYSTEMTIME tStartTime;
			SYSTEMTIME tEndTime;

			UnixTimeToSystemTime(lpObj.m_PlayerData->m_UserQuestInfo[i].GetStartDate(), &tStartTime);
			UnixTimeToSystemTime(lpObj.m_PlayerData->m_UserQuestInfo[i].GetEndDate(), &tEndTime);

			//sLog->outBasic("[QuestExp] SetQuestTimeLimit [%s][%s] Ep[%d] QS[%d] StartDate : %d-%.2d-%.2d %.2d:%.2d:%.2d EndDate : %d-%.2d-%.2d %.2d:%.2d:%.2d",
			//	lpObj.AccountID, lpObj.Name, iEpisode, iQS,tStartTime.wYear, tStartTime.wMonth, tStartTime.wDay, tStartTime.wHour, tStartTime.wMinute, tStartTime.wSecond,
			//	tEndTime.wYear, tEndTime.wMonth, tEndTime.wDay, tEndTime.wHour, tEndTime.wMinute, tEndTime.wSecond);

			return;
		}
	}
}

#pragma pack (1)
struct PMSG_ANS_QUESTEXP_ASK_UPDATE
{
	PBMSG_HEAD2 h;
	WORD wQS;
	WORD wEp;
	BYTE btAskType;
	int iAskCnt;
};
#pragma pack ()

void QuestExpProgMng::SendQuestAskInfoUpdate(int iEpisode, int iQS, int iAskType, int iAskCount, int iObjIndex)
{
	PMSG_ANS_QUESTEXP_ASK_UPDATE pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xF6, 0x06, sizeof(pMsg));

	pMsg.wEp = iEpisode;
	pMsg.wQS = iQS;
	pMsg.btAskType = iAskType;
	pMsg.iAskCnt = iAskCount;

	IOCP.DataSend(iObjIndex, (BYTE*)&pMsg, pMsg.h.size);
}

time_t QuestExpProgMng::GetCurrentDate()
{
	return time(NULL);
}

time_t QuestExpProgMng::GetExpireDate(int iDuration)
{
	time_t lExpireTime = time(NULL);
	lExpireTime += iDuration;

	return lExpireTime;
}

bool QuestExpProgMng::CheckExpireDate(time_t lQuestExpireDate)
{
	return this->GetCurrentDate() > lQuestExpireDate;
}

time_t QuestExpProgMng::GetLeftDate(time_t lExpireDate)
{
	time_t currtime = time(NULL);
	time_t difftime = lExpireDate - currtime;

	return difftime;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

