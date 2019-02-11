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

structEnergyGeneratorPoint
{
	_tagEnergyGeneratorPoint()
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

structMuunExchangeInfo
{
	_tagMuunExchangeInfo()
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

structMuunExchangeItembag
{
	_tagMuunExchangeItembag()
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
	OBJECTSTRUCT *lpUserObj;
	char chAccountID[MAX_ACCOUNT_LEN+1];
	char chCharacterName[MAX_ACCOUNT_LEN+1];
	MUUNITEMDATA ItemData[MUUN_INVENTORY_SIZE];

};

class CMuunAttack : public CObjBaseAttack
{
public:
	CMuunAttack();
	virtual ~CMuunAttack();

	void SendAttackMsg(LPGameObject &lpObj, int aTargetIndex, int SubCode, int SubCode2);
	void SkillProc(LPGameObject &lpObj);

	bool DamageAbsorb(LPGameObject &lpObj, CGameObject* lpTargetObj, CMagicInf *lpMagic, int SubCode2);
	bool Stun(LPGameObject &lpObj, CGameObject* lpTargetObj, CMagicInf *lpMagic, int SubCode2);
	bool Attack(LPGameObject &lpObj, CGameObject* lpTargetObj, CMagicInf *lpMagic, int SubCode2);

	int GetAttackDamage(LPGameObject &lpObj, int targetDefense, int SubCode2, CGameObject* lpTargetObj);
	int GetShieldDamage(LPGameObject &lpObj, CGameObject* lpTargetObj, int iAttackDamage);
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

	void MuunItemDamage(LPGameObject lpObj, int damage);
	int MuunItemEquipment(LPGameObject &lpObj, int iPos, int iSource);

	void SetMuunItemAddPeriodData(LPGameObject lpObj, int iMuunItemNum, UINT64 dwSerial);
	bool SetUserMuunEffect(LPGameObject lpObj, int iMuunItmeNum, int iMuunLv, int iEquipPos);
	void SetAddOptTypeValue(_tagMUUN_EFFECT_LIST *pUserMuunEffect);
	bool RemoveUserMuunEffect(LPGameObject lpObj, int iEquipPos);

	bool GetMuunItemValueOfOptType(LPGameObject lpObj, int iMuunOtpIndex, int *EffectValue1, int *EffectValue2);

	void GDReqLoadMuunInvenItem(LPGameObject &obj);
	void DGLoadMuunInvenItem(SDHP_ANS_DBMUUN_INVEN_LOAD *lpMsg);
	void GDReqSaveMuunInvenItem(LPGameObject &obj);

	void GCSendConditionStatus(LPGameObject &lpObj, int iPos, int iStatus);

	bool IsMuunItem(CItem *pCitem);
	bool IsMuunUtil(int ItemIndex);
	bool IsMuunItem(int iItemNum);

	bool IsStoneofEvolution(CItem *pCitem);
	bool IsStoneofEvolution(int iItemNum);

	int GetEvolutionMuunItemIndex(int iItemNum);
	int GetBeforeEvolutionMuunItemIndex(int iItemNum);
	int GetMuunRankOfMuunInfo(int iItemNum);

	void CGMuunInventoryUseItemRecv(PMSG_USEITEM_MUUN_INVEN *lpMsg, int aIndex);

	bool MuunItemEvolution(LPGameObject lpObj, int source, int target);
	bool MuunItemLevelUp(LPGameObject lpObj, int source, int target);
	bool MuunItemLifeGem(LPGameObject lpObj, int source, int target);
	bool MuunItemEnergyGenerator(LPGameObject lpObj, int source, int target);

	void GCMuunInventoryUseItemResult(LPGameObject &lpObj, int iUseType, int iResult);

	bool RemoveMuunItemPeriodInfo(LPGameObject &lpObj);

	bool IsCorrectUser(LPGameObject &lpObj);

	int AddMuunItmePeriodData(LPGameObject lpObj, WORD wItemCode, UINT64 dwSerial, int iDuration, CMuunInfo *pCMuunInfo);
	int AddMuunItemPeriodInfo(LPGameObject &lpObj);
	bool ClearPeriodMuunItemData(LPGameObject lpObj, WORD wItemCode, UINT64 dwSerial);
	bool RemovePeriodMunnItemData(LPGameObject lpObj, WORD wItemCode, UINT64 dwSerial);

	bool SetDisableMuunItemToExpire(LPGameObject lpObj, int iInventoryPosition);

	void CheckMuunItemConditionLevelUp(LPGameObject &lpObj);
	void CheckMuunItemMoveMapConditionMap(LPGameObject lpObj, int iMapNumber);
	void CheckEquipMuunItemCondition(LPGameObject &lpObj);
	void CheckEquipMuunItemConditionProc(LPGameObject &lpObj);

	void CalCharacterStat(LPGameObject &lpObj, CMuunInfo *pCMuunInfo);
	void CalCharacterStat(LPGameObject &lpObj, int iOptType);

	int CheckMuunItemCondition(LPGameObject &obj,MUUN_EFFECT_LIST *pUserMuunEffect, CMuunInfo *pCMuunInfo);
	int CheckMuunItemConditionProc(_tagMUUN_EFFECT_LIST *pUserMuunEffect, CMuunInfo *pCMuunInfo);
	int ChkMuunOptConditionTime(_tagMUUN_EFFECT_LIST *pUserMuunEffect, CMuunInfo *pCMuunInfo);
	int ChkMuunOptConditionDay(_tagMUUN_EFFECT_LIST *pUserMuunEffect, CMuunInfo *pCMuunInfo);
	int ChkMuunOptConditionLevel(LPGameObject &obj,MUUN_EFFECT_LIST *pUserMuunEffect, CMuunInfo *pCMuunInfo);
	int ChkMuunOptConditionMap(LPGameObject &obj,MUUN_EFFECT_LIST *pUserMuunEffect, CMuunInfo *pCMuunInfo);

	int GetItemFromMuunInventory(LPGameObject lpObj, WORD wItemCode, UINT64 dwSerial);

	void CGMuunExchangeItem(PMSG_REQ_MUUN_EXCHANGE *lpMsg, int aIndex);

	bool ChkMuunExchangeInvenNeedItem(int & iItemCnt, int iInvenPos, int iNeedItemCnt, int iInvenItemNum, int iNeedItemNum, int *ItemPos);
	bool ChkMuunExchangeInvenNeedItem(LPGameObject &lpObj, int iSelect, int *ItemPos);

	void SendMsgMuunExchange(LPGameObject &lpObj, int iResult);
	bool ChkMuunExchangeInvenEmpty(LPGameObject &lpObj, int iSelect);

	bool GDMuunExchangeInsertInven(LPGameObject &lpObj, int iSelect);
	BYTE DGMuunExchangeInsertInven(LPGameObject &lpObj, CItem CreateItem, int iSelect);
	bool ChkAndDelItemMuunExchange(LPGameObject &lpObj, int iSelect);

	void SetTarget(LPGameObject &lpObj, int aTargetIndex);
	void ReSetTarget(LPGameObject &lpObj, int aTargetIndex);

	void CGReqRideSelect(PMSG_MUUN_RIDE_SELECT *lpMsg, int aIndex);

	void SkillProc(LPGameObject &lpObj);

	time_t GetCurrentDate();
	time_t GetExpireDate(int iDuration);
	bool CheckExpireDate(time_t dwItemExpireDate);
	time_t GetLeftDate(time_t ExpireDate);
	bool IsMuunExpireDate(int iType);

	void Attack(LPGameObject &lpObj, CGameObject* lpTargetObj, CMagicInf *lpMagic, int SubCode, int SubCode2);

	bool CheckAddOptionExpireDate(time_t StartDate, time_t EndDate);
	void MsgIsMuunItemActive(LPGameObject lpObj, int iPos);

	void CheckMuunItemPeriodData(LPGameObject &lpObj);

	CMuunInfoMng					m_MuunInfoMng;
	MUUN_ITEM_PERIOD_DATA			*m_MuunItemPeriodData;
	CMuunAttack						m_MuunAttack;
	_tagEnergyGeneratorPoint		m_stEnergyGeneratorPoint[MAX_MUUN_ENERGY_GENERATOR_POINT];
	int								m_iStoneOfEvolutionPt;
	_tagMuunExchangeInfo			m_stMuunExchangeInfo[MAX_MUUN_EXCHANGE_INFO];
	_tagMuunExchangeItembag			m_stMuunExchangeItembag[MAX_MUUN_EXCHANGE_ITEMBAG];

}; extern CMuunSystem g_CMuunSystem;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

