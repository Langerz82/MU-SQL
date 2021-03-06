////////////////////////////////////////////////////////////////////////////////
// MuRummyMng.h
#ifndef MURUMMYMNG_H
#define MURUMMYMNG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MuRummyInfo.h"
#include "MuRummyCardInfo.h"

struct PMSG_REQ_MURUMMY_EVENT_OPEN;
struct PMSG_REQ_MURUMMY_INFO;
struct PMSG_REQ_REVEAL_CARD;
struct PMSG_REQ_CARD_MOVE;
struct PMSG_REQ_CARD_REMOVE;
struct PMSG_REQ_CARD_MATCH;
struct PMSG_REQ_MURUMMY_END;
struct MuRummyCardInfo;

class CMuRummyMng
{
public:
	CMuRummyMng();
	virtual ~CMuRummyMng();

	bool IsMuRummyEventOn() { return this->m_bMuRummyEventOn; }
	int GetMuRummyEventItemDropRate() { return this->m_iMuRummyEventItemDropRate; }

	void LoadPreFixData(char *lpszFileName);

	int GetRewardScore1() { return this->m_iRewardScore1; }
	int GetRewardScore2() { return this->m_iRewardScore2; }

	void SetCardDeck(CMuRummyInfo *pMuRummyInfo);
	void SetSpecialCard(CMuRummyInfo *pMuRummyInfo);
	void SetSlotStatus(CMuRummyInfo *pMuRummyInfo);
	void CardShuffle(CMuRummyCardInfo *pCCardInfo);
	bool SetPlayCardInfo(CMuRummyInfo *pMuRummyInfo, MuRummyCardInfo *pOutCardInfo);
	bool FillEmptySlotCard(CMuRummyInfo *pMuRummyInfo, MuRummyCardInfo *pOutCardInfo, CGameObject lpObj);
	BYTE GetTempCardShowSlot(BYTE *pSlotCardIndex);
	BYTE CardSlotMove(CMuRummyInfo *pCMuRummyInfo, int sSlot, int tSlot, CMuRummyCardInfo *pCOutCardInfo, CGameObject lpObj);
	BYTE CardSlotReMove(CMuRummyInfo *pCMuRummyInfo, int sSlot);
	BYTE CardMatchCheck(CMuRummyInfo *pCMuRummyInfo, int *iOutScore,MuRummyCardUpdateDS *pOutCardUpdateDS, CGameObject lpObj);
	void CardSortAscending(CMuRummyCardInfo *pCCardInfo);
	int IsCardSameColor(CMuRummyCardInfo *pCCardInfo);
	int IsCardNumber(CMuRummyCardInfo *pCCardInfo);
	BYTE GetColorName(int iColorNum);

	void CGReqMuRummyEventOpen(PMSG_REQ_MURUMMY_EVENT_OPEN *lpMsg, CGameObject &Obj);
	void CGReqMuRummyStart(PMSG_REQ_MURUMMY_INFO *lpMsg, CGameObject &Obj);
	void CGReqCardReveal(PMSG_REQ_REVEAL_CARD *lpMsg, CGameObject &Obj);
	void CGReqCardMove(PMSG_REQ_CARD_MOVE *lpMsg, CGameObject &Obj);
	void CGReqCardReMove(PMSG_REQ_CARD_REMOVE *lpMsg, CGameObject &Obj);
	void CGReqCardMatch(PMSG_REQ_CARD_MATCH *lpMsg, CGameObject &Obj);
	void CGReqMuRummyEnd(PMSG_REQ_MURUMMY_END *lpMsg, CGameObject &Obj);

	void GCSendCardList(CGameObject &Obj);
	void GCSendMsg(CGameObject &Obj, BYTE btNotiIndex, int iValue);

	void CheatSetCardColorAsc(CMuRummyInfo *pMuRummyInfo, CGameObject &Obj);
	void CheatSetCardNumAsc(CMuRummyInfo *pMuRummyInfo, CGameObject &Obj);
	void CheatSetScore(CMuRummyInfo *pMuRummyInfo, int iScore, CGameObject &Obj);

	void GDReqCardInfo(CGameObject &Obj);
	void GDAnsCardInfo(PMSG_ANS_MURUMMY_SELECT_DS *lpMsg);
	void GDReqCardInfoInsert(CGameObject &Obj);
	void GDReqScoreUpdate(CGameObject lpObj, WORD wScore,MuRummyCardUpdateDS *pCardUpdateDS);
	void GDReqCardInfoUpdate(CGameObject lpObj, CMuRummyCardInfo *pCCardInfo, int iSeq);
	void GDReqScoreDelete(CGameObject &Obj);
	void GDReqSlotInfoUpdate(CGameObject lpObj, BYTE btSeq, BYTE btSlotNum, BYTE btStatus);
	void GDReqMuRummyInfoUpdate(CGameObject &Obj);
	void GDReqMuRummyDBLog(CGameObject lpObj, int iScore);

	void Slot3Log(CGameObject lpObj, int iLogType);
	void Slot5Log(CGameObject lpObj, int iLogType);

private:
	int m_iRewardScore1;
	int m_iRewardScore2;
	bool m_bMuRummyEventOn;
	int m_iMuRummyEventItemDropRate;
	bool m_bMuRummyEventDBLog;
};

extern CMuRummyMng g_CMuRummyMng;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

