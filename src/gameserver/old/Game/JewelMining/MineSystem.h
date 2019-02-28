
#ifndef _MU_MINESYSTEM_H
#define _MU_MINESYSTEM_H

#include "StdAfx.h"
#include "User/CUserData.h"


class CMineSystem
{
public:
	CMineSystem();
	virtual ~CMineSystem();

	void LoadMineingDropScript();

	void CheckIsUPTUserWhenDisconnected(CGameObject &Obj);
	void CheckIsUPTUserWhenConnect(CGameObject &Obj);
	void GiveRewardItemToUPTUser(CGameObject &Obj, WORD wTwinkleType, int iStage);
	
	void MineTwinkle(PMSG_REQ_MINETWINKLE *lpMsg, CGameObject &Obj);
	void RewardMineTwinkle(PMSG_REQ_MINETWINKLE_REWARD *lpMsg, CGameObject &Obj);
	void FailMineTwinkle(PMSG_ANS_MINETWINKLE_END_ANIMATION *lpMsg, CGameObject &Obj, bool bError);

	void RequestDBToModifyUPTUserInfo(CGameObject &Obj, char *szCharName, WORD wTwinkleType, int iCurrentStage, BYTE byRequestType);
	void GDAnsModifyMineSystemUPTUserInfo(SDHP_ANS_MINESYSTEM_UPT_USERINFO *lpMsg);
	void GDAnsLoadMineSystemUPTUserInfo(SDHP_ANS_LOAD_MINESYSTEM_UPT_USERINFO *lpMsg);

	BOOL CheckMoveMapWhileMining(CGameObject &Obj);
	BOOL IsTwinkle(WORD wClass);
	BOOL IsEquipPickax(CGameObject &Obj);
	BOOL IsPickax(WORD wItemType);

	void SetTwinklesInfo();
	void SetTwinkleInfo(CGameObject &Obj);
	void ResetTwinkleInfo(CGameObject &Obj, WORD wTwinkleIndex, bool bFailMining);

	BOOL SuccessOrFailure(WORD wType, int iStage);
	BOOL MiracleSuccessOrFailure(WORD wType, int iStage);
	BOOL CheckValidationMineState(CGameObject &Obj, WORD wTwinkleIndex, WORD wTwinkleType, BYTE byMapNumber, WORD wUserIndex, int *iResult, bool bRewardCheck);
	BOOL GiveRewardItem(CGameObject &Obj, WORD wTwinkleType, int iStage, int bNotify);

	void ChangeUnconditionalSuccess(int iIsSuccess);

	short GetRepairValueOfJewel(WORD sItemType, WORD sLevel);

private:
	int m_iChanceStartValue;
	int m_iChanceStartValue_Copy;
	std::map<int, _ST_MINESYSTEM_MINE_SUCCESS_INFO> m_mapMineSuccessInfo;
	std::vector<_ST_MINESYSTEM_MINE_MIRACLE_SUCCESS_INFO> m_vecMiracleSuccessInfo;
	std::vector<_ST_MINESYSTEM_REWARD_VALUE> m_vecRewardValue;
	std::vector<_ST_MINESYSTEM_REWARD_VALUE> m_vecMiracleRewardValue;
	std::vector<_ST_MINESYSTEM_REPAIR_VALUE> m_vecRepairValue;
	std::map<int, _ST_MINESYSTEM_TWINKLE_SPAWN> m_mapTwinkleSpawn;
	std::map<int, std::string> m_mapMapName;
	std::map<int, _ST_MINESYSTEM_TWINKLE> m_mapTwinkle;
};

extern CMineSystem g_MineSystem;

#endif
