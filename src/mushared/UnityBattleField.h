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
	void SendUBFNotice(int aIndex);

	void GDReqJoinUnityBattleField(int aIndex);
	void DGAnsJoinUnityBattleField(int aIndex, BYTE Result, WORD LeftSecond);
	void GDReqCopyCharacterInfo(int aIndex, BYTE CharacterSlot);
	void DGAnsCopyCharacterInfo(int aIndex, BYTE result, BYTE subResult);
	void GDReqCheckJoinedUnityBattleField(int aIndex, int IsUnityBattleFieldServer, BYTE ObServerMode);
	void DGAnsCheckJoinedUnityBattleField(int aIndex, BYTE btRegisterState);
	void GDObserverLogoutManager();
	void GDReqCancelUnityBattleField(int aIndex, BYTE btCancelType);
	void GDReqCancelUnityBattleField(int aIndex, BYTE btCancelType, const char *name);
	void DGAnsCancelUnityBattleField(int aIndex, BYTE aCanceledResult, BYTE deletedResult);
	void GDReqGetRealNameAndServerCode(int aIndex);
	void DGAnsGetRealNameAndServerCode(int aIndex, int nServerCodeOfHomeWorld);
	void GDReqUBFGetReward(int aIndex, BYTE btBattleKind);
	void GDReqSetReceivedWinnerItem(int aIndex, BYTE btBattleKind);
	void DGAnsSetReceivedWinnerItem(int aIndex, BYTE btReturn);

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

