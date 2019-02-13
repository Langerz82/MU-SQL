////////////////////////////////////////////////////////////////////////////////
// MuunSystem.h
#ifndef MUUNSYSTEM_H
#define MUUNSYSTEM_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MuunInfoMng.h"
//
#include "ObjBaseAttack.h"

#define MAX_MUUN_ENERGY_GENERATOR_POINT		8
#define MAX_MUUN_EXCHANGE_INFO				10
#define MAX_MUUN_EXCHANGE_ITEMBAG			20

struct EnergyGeneratorPoint
{
	EnergyGeneratorPoint()
	{
		this->iRank = -1;
		this->iRankPt = 0;
		this->iMuunLv = -1;
		this->iMuunLvPt = 0;
	}

	int iRank;
	int iRankPt;
	int iMuunLv;
	int iMuunLvPt;
};

struct MuunExchangeInfo
{
	MuunExchangeInfo()
	{
		this->iItemType = 0;
		this->iItemIndex = 0;
		this->iInvenChk = 0;
		this->iItemCnt = 0;
		this->iItemBagIndex = 0;
	}

	int iItemType;
	int iItemIndex;
	int iInvenChk;
	int iItemCnt;
	int iItemBagIndex;
};

struct MuunExchangeItembag
{
	MuunExchangeItembag()
	{
		this->iIndex = 0;
		this->iItemType = 0;
		this->iItemIndex = 0;
		this->iMinLv = 0;
		this->iMaxLv = 0;
		this->iSkill = 0;
		this->iLuck = 0;
		this->iAddOpt = 0;
		this->iExItem = 0;
		this->iStoneMuunItemType = 0;
		this->iStoneMuunItemIndex = 0;
		this->iInvenChk = 0;
	}

	int iIndex;
	int iItemType;
	int iItemIndex;
	int iMinLv;
	int iMaxLv;
	int iSkill;
	int iLuck;
	int iAddOpt;
	int iExItem;
	int iStoneMuunItemType;
	int iStoneMuunItemIndex;
	int iInvenChk;
};

struct MUUNITEMDATA
{
	void Clear()
	{
		this->bIsUsed = 0;
		this->dwSerial = 0;
		this->pCMuunInfo = 0;
		this->wItemCode = 0;
		this->btOptionType = 0;
	}

	bool bIsUsed;
	WORD wItemCode;
	BYTE btOptionType;
	UINT64 dwSerial;
	CMuunInfo *pCMuunInfo;
};

struct MUUN_ITEM_PERIOD_DATA
{
	void Clear()
	{
		this->bIsUsed = false;

		for (int i = 0; i < MUUN_INVENTORY_SIZE; i++)
		{
			this->ItemData[i].Clear();
		}
	}

	bool bIsUsed;
	char btUsedDataCount;
	WORD wUserIndex;
	DWORD dwUserGUID;
	CGameObject lpUserObj;
	char chAccountID[MAX_ACCOUNT_LEN+1];
	char chCharacterName[MAX_ACCOUNT_LEN+1];
	MUUNITEMDATA ItemData[MUUN_INVENTORY_SIZE];

};

class CMuunAttack : public CObjBaseAttack
{
public:
	CMuunAttack();
	virtual ~CMuunAttack();

	void SendAttackMsg(CGameObject &lpObj, int aTargetIndex, int SubCode, int SubCode2);
	void SkillProc(CGameObject &lpObj);

	bool DamageAbsorb(CGameObject &lpObj, CGameObject lpTargetObj, CMagicInf *lpMagic, int SubCode2);
	bool Stun(CGameObject &lpObj, CGameObject lpTargetObj, CMagicInf *lpMagic, int SubCode2);
	bool Attack(CGameObject &lpObj, CGameObject lpTargetObj, CMagicInf *lpMagic, int SubCode2);

	int GetAttackDamage(CGameObject &lpObj, int targetDefense, int SubCode2, CGameObject lpTargetObj);
	int GetShieldDamage(CGameObject &lpObj, CGameObject lpTargetObj, int iAttackDamage);
};

class CMuunSystem
{
public:

	CMuunSystem();
	virtual ~CMuunSystem();

	void Initialize();

	bool LoadScriptMuunSystemInfo(char *lpszFileName);
	bool LoadScriptMuunSystemOption(char *lpszFileName);
	bool LoadScriptMuunExchange(char *lpszFileName);

	void MuunItemDamage(CGameObject lpObj, int damage);
	int MuunItemEquipment(CGameObject &lpObj, int iPos, int iSource);

	void SetMuunItemAddPeriodData(CGameObject lpObj, int iMuunItemNum, UINT64 dwSerial);
	bool SetUserMuunEffect(CGameObject lpObj, int iMuunItmeNum, int iMuunLv, int iEquipPos);
	void SetAddOptTypeValue(MUUN_EFFECT_LIST *pUserMuunEffect);
	bool RemoveUserMuunEffect(CGameObject lpObj, int iEquipPos);

	bool GetMuunItemValueOfOptType(CGameObject lpObj, int iMuunOtpIndex, int *EffectValue1, int *EffectValue2);

	void GDReqLoadMuunInvenItem(CGameObject &obj);
	void DGLoadMuunInvenItem(SDHP_ANS_DBMUUN_INVEN_LOAD *lpMsg);
	void GDReqSaveMuunInvenItem(CGameObject &obj);

	void GCSendConditionStatus(CGameObject &lpObj, int iPos, int iStatus);

	bool IsMuunItem(CItemObject *pCItemObject);
	bool IsMuunUtil(int ItemIndex);
	bool IsMuunItem(int iItemNum);

	bool IsStoneofEvolution(CItemObject *pCItemObject);
	bool IsStoneofEvolution(int iItemNum);

	int GetEvolutionMuunItemIndex(int iItemNum);
	int GetBeforeEvolutionMuunItemIndex(int iItemNum);
	int GetMuunRankOfMuunInfo(int iItemNum);

	void CGMuunInventoryUseItemRecv(PMSG_USEITEM_MUUN_INVEN *lpMsg, CGameObject &lpObj);

	bool MuunItemEvolution(CGameObject lpObj, int source, int target);
	bool MuunItemLevelUp(CGameObject lpObj, int source, int target);
	bool MuunItemLifeGem(CGameObject lpObj, int source, int target);
	bool MuunItemEnergyGenerator(CGameObject lpObj, int source, int target);

	void GCMuunInventoryUseItemResult(CGameObject &lpObj, int iUseType, int iResult);

	bool RemoveMuunItemPeriodInfo(CGameObject &lpObj);

	bool IsCorrectUser(CGameObject &lpObj);

	int AddMuunItmePeriodData(CGameObject lpObj, WORD wItemCode, UINT64 dwSerial, int iDuration, CMuunInfo *pCMuunInfo);
	int AddMuunItemPeriodInfo(CGameObject &lpObj);
	bool ClearPeriodMuunItemData(CGameObject lpObj, WORD wItemCode, UINT64 dwSerial);
	bool RemovePeriodMunnItemData(CGameObject lpObj, WORD wItemCode, UINT64 dwSerial);

	bool SetDisableMuunItemToExpire(CGameObject lpObj, int iInventoryPosition);

	void CheckMuunItemConditionLevelUp(CGameObject &lpObj);
	void CheckMuunItemMoveMapConditionMap(CGameObject lpObj, int iMapNumber);
	void CheckEquipMuunItemCondition(CGameObject &lpObj);
	void CheckEquipMuunItemConditionProc(CGameObject &lpObj);

	void CalCharacterStat(CGameObject &lpObj, CMuunInfo *pCMuunInfo);
	void CalCharacterStat(CGameObject &lpObj, int iOptType);

	int CheckMuunItemCondition(CGameObject &obj,MUUN_EFFECT_LIST *pUserMuunEffect, CMuunInfo *pCMuunInfo);
	int CheckMuunItemConditionProc(MUUN_EFFECT_LIST *pUserMuunEffect, CMuunInfo *pCMuunInfo);
	int ChkMuunOptConditionTime(MUUN_EFFECT_LIST *pUserMuunEffect, CMuunInfo *pCMuunInfo);
	int ChkMuunOptConditionDay(MUUN_EFFECT_LIST *pUserMuunEffect, CMuunInfo *pCMuunInfo);
	int ChkMuunOptConditionLevel(CGameObject &obj,MUUN_EFFECT_LIST *pUserMuunEffect, CMuunInfo *pCMuunInfo);
	int ChkMuunOptConditionMap(CGameObject &obj,MUUN_EFFECT_LIST *pUserMuunEffect, CMuunInfo *pCMuunInfo);

	int GetItemFromMuunInventory(CGameObject lpObj, WORD wItemCode, UINT64 dwSerial);

	void CGMuunExchangeItem(PMSG_REQ_MUUN_EXCHANGE *lpMsg, CGameObject &lpObj);

	bool ChkMuunExchangeInvenNeedItem(int & iItemCnt, int iInvenPos, int iNeedItemCnt, int iInvenItemNum, int iNeedItemNum, int *ItemPos);
	bool ChkMuunExchangeInvenNeedItem(CGameObject &lpObj, int iSelect, int *ItemPos);

	void SendMsgMuunExchange(CGameObject &lpObj, int iResult);
	bool ChkMuunExchangeInvenEmpty(CGameObject &lpObj, int iSelect);

	bool GDMuunExchangeInsertInven(CGameObject &lpObj, int iSelect);
	BYTE DGMuunExchangeInsertInven(CGameObject &lpObj, CItemObject CreateItem, int iSelect);
	bool ChkAndDelItemMuunExchange(CGameObject &lpObj, int iSelect);

	void SetTarget(CGameObject &lpObj, int aTargetIndex);
	void ReSetTarget(CGameObject &lpObj, int aTargetIndex);

	void CGReqRideSelect(PMSG_MUUN_RIDE_SELECT *lpMsg, CGameObject &lpObj);

	void SkillProc(CGameObject &lpObj);

	time_t GetCurrentDate();
	time_t GetExpireDate(int iDuration);
	bool CheckExpireDate(time_t dwItemExpireDate);
	time_t GetLeftDate(time_t ExpireDate);
	bool IsMuunExpireDate(int iType);

	void Attack(CGameObject &lpObj, CGameObject lpTargetObj, CMagicInf *lpMagic, int SubCode, int SubCode2);

	bool CheckAddOptionExpireDate(time_t StartDate, time_t EndDate);
	void MsgIsMuunItemActive(CGameObject lpObj, int iPos);

	void CheckMuunItemPeriodData(CGameObject &lpObj);

	CMuunInfoMng					m_MuunInfoMng;
	MUUN_ITEM_PERIOD_DATA			*m_MuunItemPeriodData;
	CMuunAttack						m_MuunAttack;
	EnergyGeneratorPoint		m_stEnergyGeneratorPoint[MAX_MUUN_ENERGY_GENERATOR_POINT];
	int								m_iStoneOfEvolutionPt;
	MuunExchangeInfo			m_stMuunExchangeInfo[MAX_MUUN_EXCHANGE_INFO];
	MuunExchangeItembag			m_stMuunExchangeItembag[MAX_MUUN_EXCHANGE_ITEMBAG];

}; extern CMuunSystem g_CMuunSystem;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

