////////////////////////////////////////////////////////////////////////////////
// UnityBattleField.h
#ifndef UNITY_BATTLE_FIELD_H
#define UNITY_BATTLE_FIELD_H

#pragma once

class CUnityBattleField
{
public:
	CUnityBattleField(void);
	virtual ~CUnityBattleField(void);

	void LoadData(char *FileName);
	void SendUBFNotice(CGameObject &lpObj);

	void GDReqJoinUnityBattleField(CGameObject &lpObj);
	void DGAnsJoinUnityBattleField(CGameObject &lpObj, BYTE Result, WORD LeftSecond);
	void GDReqCopyCharacterInfo(CGameObject &lpObj, BYTE CharacterSlot);
	void DGAnsCopyCharacterInfo(CGameObject &lpObj, BYTE result, BYTE subResult);
	void GDReqCheckJoinedUnityBattleField(CGameObject &lpObj, int IsUnityBattleFieldServer, BYTE ObServerMode);
	void DGAnsCheckJoinedUnityBattleField(CGameObject &lpObj, BYTE btRegisterState);
	void GDObserverLogoutManager();
	void GDReqCancelUnityBattleField(CGameObject &lpObj, BYTE btCancelType);
	void GDReqCancelUnityBattleField(CGameObject &lpObj, BYTE btCancelType, const char *name);
	void DGAnsCancelUnityBattleField(CGameObject &lpObj, BYTE aCanceledResult, BYTE deletedResult);
	void GDReqGetRealNameAndServerCode(CGameObject &lpObj);
	void DGAnsGetRealNameAndServerCode(CGameObject &lpObj, int nServerCodeOfHomeWorld);
	void GDReqUBFGetReward(CGameObject &lpObj, BYTE btBattleKind);
	void GDReqSetReceivedWinnerItem(CGameObject &lpObj, BYTE btBattleKind);
	void DGAnsSetReceivedWinnerItem(CGameObject &lpObj, BYTE btReturn);

	int GetUBFPopupType() { return this->m_iUBFPopupType; }

private:

	bool EventBannerOff;
	bool m_bUBFEnable;
	int  m_iUBFPopupType;
	bool m_bUBFCharacterPromotion;
};

extern CUnityBattleField g_UnityBattleField;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

