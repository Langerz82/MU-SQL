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
	void SendUBFNotice(CGameObject &Obj);

	void GDReqJoinUnityBattleField(CGameObject &Obj);
	void DGAnsJoinUnityBattleField(CGameObject &Obj, BYTE Result, WORD LeftSecond);
	void GDReqCopyCharacterInfo(CGameObject &Obj, BYTE CharacterSlot);
	void DGAnsCopyCharacterInfo(CGameObject &Obj, BYTE result, BYTE subResult);
	void GDReqCheckJoinedUnityBattleField(CGameObject &Obj, int IsUnityBattleFieldServer, BYTE ObServerMode);
	void DGAnsCheckJoinedUnityBattleField(CGameObject &Obj, BYTE btRegisterState);
	void GDObserverLogoutManager();
	void GDReqCancelUnityBattleField(CGameObject &Obj, BYTE btCancelType);
	void GDReqCancelUnityBattleField(CGameObject &Obj, BYTE btCancelType, const char *name);
	void DGAnsCancelUnityBattleField(CGameObject &Obj, BYTE aCanceledResult, BYTE deletedResult);
	void GDReqGetRealNameAndServerCode(CGameObject &Obj);
	void DGAnsGetRealNameAndServerCode(CGameObject &Obj, int nServerCodeOfHomeWorld);
	void GDReqUBFGetReward(CGameObject &Obj, BYTE btBattleKind);
	void GDReqSetReceivedWinnerItem(CGameObject &Obj, BYTE btBattleKind);
	void DGAnsSetReceivedWinnerItem(CGameObject &Obj, BYTE btReturn);

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

