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
	void SendUBFNotice(LPGameObject &lpObj);

	void GDReqJoinUnityBattleField(LPGameObject &lpObj);
	void DGAnsJoinUnityBattleField(LPGameObject &lpObj, BYTE Result, WORD LeftSecond);
	void GDReqCopyCharacterInfo(LPGameObject &lpObj, BYTE CharacterSlot);
	void DGAnsCopyCharacterInfo(LPGameObject &lpObj, BYTE result, BYTE subResult);
	void GDReqCheckJoinedUnityBattleField(LPGameObject &lpObj, int IsUnityBattleFieldServer, BYTE ObServerMode);
	void DGAnsCheckJoinedUnityBattleField(LPGameObject &lpObj, BYTE btRegisterState);
	void GDObserverLogoutManager();
	void GDReqCancelUnityBattleField(LPGameObject &lpObj, BYTE btCancelType);
	void GDReqCancelUnityBattleField(LPGameObject &lpObj, BYTE btCancelType, const char *name);
	void DGAnsCancelUnityBattleField(LPGameObject &lpObj, BYTE aCanceledResult, BYTE deletedResult);
	void GDReqGetRealNameAndServerCode(LPGameObject &lpObj);
	void DGAnsGetRealNameAndServerCode(LPGameObject &lpObj, int nServerCodeOfHomeWorld);
	void GDReqUBFGetReward(LPGameObject &lpObj, BYTE btBattleKind);
	void GDReqSetReceivedWinnerItem(LPGameObject &lpObj, BYTE btBattleKind);
	void DGAnsSetReceivedWinnerItem(LPGameObject &lpObj, BYTE btReturn);

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

