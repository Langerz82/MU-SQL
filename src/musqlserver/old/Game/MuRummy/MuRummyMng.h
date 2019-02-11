////////////////////////////////////////////////////////////////////////////////
// MuRummyMng.h
#ifndef MURUMMYMNG_H
#define MURUMMYMNG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MuRummyInfo.h"
#include "Protocol/protocolStructs.h"

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
	bool FillEmptySlotCard(CMuRummyInfo *pMuRummyInfo, MuRummyCardInfo *pOutCardInfo, OBJECTSTRUCT *lpObj);
	BYTE GetTempCardShowSlot(BYTE *pSlotCardIndex);
	BYTE CardSlotMove(CMuRummyInfo *pCMuRummyInfo, int sSlot, int tSlot, CMuRummyCardInfo *pCOutCardInfo, OBJECTSTRUCT *lpObj);
	BYTE CardSlotReMove(CMuRummyInfo *pCMuRummyInfo, int sSlot);
	BYTE CardMatchCheck(CMuRummyInfo *pCMuRummyInfo, int *iOutScore, _tagMuRummyCardUpdateDS *pOutCardUpdateDS, OBJECTSTRUCT *lpObj);
	void CardSortAscending(CMuRummyCardInfo *pCCardInfo);
	int IsCardSameColor(CMuRummyCardInfo *pCCardInfo);
	int IsCardNumber(CMuRummyCardInfo *pCCardInfo);
	BYTE GetColorName(int iColorNum);

	void CGReqMuRummyEventOpen(PMSG_REQ_MURUMMY_EVENT_OPEN *lpMsg, int aIndex);
	void CGReqMuRummyStart(PMSG_REQ_MURUMMY_INFO *lpMsg, int aIndex);
	void CGReqCardReveal(PMSG_REQ_REVEAL_CARD *lpMsg, int aIndex);
	void CGReqCardMove(PMSG_REQ_CARD_MOVE *lpMsg, int aIndex);
	void CGReqCardReMove(PMSG_REQ_CARD_REMOVE *lpMsg, int aIndex);
	void CGReqCardMatch(PMSG_REQ_CARD_MATCH *lpMsg, int aIndex);
	void CGReqMuRummyEnd(PMSG_REQ_MURUMMY_END *lpMsg, int aIndex);

	void GCSendCardList(LPGameObject &lpObj);
	void GCSendMsg(LPGameObject &lpObj, BYTE btNotiIndex, int iValue);

	void CheatSetCardColorAsc(CMuRummyInfo *pMuRummyInfo, int aIndex);
	void CheatSetCardNumAsc(CMuRummyInfo *pMuRummyInfo, int aIndex);
	void CheatSetScore(CMuRummyInfo *pMuRummyInfo, int iScore, int aIndex);

	void GDReqCardInfo(LPGameObject &lpObj);
	void GDAnsCardInfo(PMSG_ANS_MURUMMY_SELECT_DS *lpMsg);
	void GDReqCardInfoInsert(LPGameObject &lpObj);
	void GDReqScoreUpdate(LPGameObject lpObj, WORD wScore, _tagMuRummyCardUpdateDS *pCardUpdateDS);
	void GDReqCardInfoUpdate(LPGameObject lpObj, CMuRummyCardInfo *pCCardInfo, int iSeq);
	void GDReqScoreDelete(LPGameObject &lpObj);
	void GDReqSlotInfoUpdate(LPGameObject lpObj, BYTE btSeq, BYTE btSlotNum, BYTE btStatus);
	void GDReqMuRummyInfoUpdate(LPGameObject &lpObj);
	void GDReqMuRummyDBLog(LPGameObject lpObj, int iScore);

	void Slot3Log(LPGameObject lpObj, int iLogType);
	void Slot5Log(LPGameObject lpObj, int iLogType);

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

