////////////////////////////////////////////////////////////////////////////////
// MuRummyMng.h
#ifndef MURUMMYMNG_H
#define MURUMMYMNG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MuRummyInfo.h"
#include "MuRummyCardInfo.h"

struct MuRummyCardInfo
{
	MuRummyCardInfo()
	{
		this->btColor = -1;
		this->btNumber = -1;
		this->btSlotNum = -1;
	}

	BYTE btColor;
	BYTE btNumber;
	BYTE btSlotNum;
};

struct PMSG_REQ_MURUMMY_EVENT_OPEN
{
	PBMSG_HEAD2 h;
};

struct PMSG_ANS_MURUMMY_EVENT_OPEN
{
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btEventTime1;
	BYTE btEventTime2;
	BYTE btEventTime3;
	BYTE btEventTime4;
};

struct PMSG_REQ_MURUMMY_INFO
{
	PBMSG_HEAD2 h;
	BYTE btGameType;
};

struct PMSG_ANS_MURUMMY_INFO
{
	PMSG_ANS_MURUMMY_INFO()
	{
		this->btTotScoreH = 0;
		this->btTotScoreL = 0;
		this->btCardDeckCnt = 0;
		this->SpecialCardCnt = 0;
		this->btIsSpecialCard = FALSE;
		memset(this->btSlotStatus, 0x00, sizeof(this->btSlotStatus));
	}

	PBMSG_HEAD2 h;
	BYTE btTotScoreH;
	BYTE btTotScoreL;
	BYTE btCardDeckCnt;
	BYTE SpecialCardCnt;
	BYTE btUnk;
	BYTE btIsSpecialCard;
	BYTE btSlotStatus[10];
	MuRummyCardInfo stMuRummyCardInfo[6];
};

struct PMSG_REQ_REVEAL_CARD
{
	PBMSG_HEAD2 h;
};

struct PMSG_ANS_REVEAL_CARD
{
	PBMSG_HEAD2 h;
	MuRummyCardInfo stMuRummyCardInfo[6];
	BYTE btCardDeckCnt;
};

struct PMSG_REQ_CARD_MOVE
{
	PBMSG_HEAD2 h;
	BYTE sSlot;
	BYTE tSlot;
};

struct PMSG_ANS_CARD_MOVE
{
	PBMSG_HEAD2 h;
	BYTE sSlot;
	BYTE tSlot;
	BYTE btColor;
	BYTE btNumber;
};

struct PMSG_REQ_CARD_REMOVE
{
	PBMSG_HEAD2 h;
	BYTE btSlot;
};

struct PMSG_ANS_CARD_REMOVE
{
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_REQ_CARD_MATCH
{
	PBMSG_HEAD2 h;
};

struct MuRummyCardUpdateDS
{
	MuRummyCardUpdateDS()
	{
		this->btSlotNum = -1;
		this->btSeq = -1;
		this->btStatus = -1;
	}

	BYTE btSlotNum;
	BYTE btSeq;
	BYTE btStatus;
};

struct PMSG_ANS_CARD_MATCH
{
	PBMSG_HEAD2 h;
	BYTE btScoreH;
	BYTE btScoreL;
	BYTE btTotScoreH;
	BYTE btTotScoreL;
	BYTE btResult;
};

struct PMSG_REQ_MURUMMY_END
{
	PBMSG_HEAD2 h;
};

struct PMSG_ANS_MURUMMY_END
{
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_SEND_SLOTCARD_LIST
{
	PBMSG_HEAD2 h;
	MuRummyCardInfo stMuRummyCardInfo[6];
};

struct PMSG_SEND_MURUMMY_MSG
{
	PBMSG_HEAD2 h;
	BYTE btNotiIndex;
	BYTE btValueH;
	BYTE btValueL;
};

struct PMSG_REQ_MURUMMY_SELECT_DS
{
	PBMSG_HEAD2 h;
	char AccountID[11];
	char Name[11];
	WORD aIndex;
};

struct _tagMuRummyCardInfoDS
{
	_tagMuRummyCardInfoDS()
	{
		this->btColor = -1;
		this->btNumber = -1;
		this->btSlotNum = -1;
		this->btSeq = -1;
		this->btStatus = -1;
	}

	BYTE btColor;
	BYTE btNumber;
	BYTE btSlotNum;
	BYTE btSeq;
	BYTE btStatus;
};

struct PMSG_ANS_MURUMMY_SELECT_DS
{
	PBMSG_HEAD2 h;
	WORD aIndex;
	WORD wScore;
	BYTE btResult;
	_tagMuRummyCardInfoDS stMuRummyCardInfoDS[24];
};

struct PMSG_REQ_MURUMMY_INSERT_DS
{
	PBMSG_HEAD2 h;
	char AccountID[11];
	char Name[11];
	WORD aIndex;
	_tagMuRummyCardInfoDS stMuRummyCardInfoDS[24];
};

struct PMSG_REQ_MURUMMY_SCORE_UPDATE_DS
{
	PBMSG_HEAD2 h;
	char AccountID[11];
	char Name[11];
	WORD wScore;
	MuRummyCardUpdateDS stCardUpdateDS[3];
};

struct PMSG_REQ_MURUMMY_UPDATE_DS
{
	PBMSG_HEAD2 h;
	char AccountID[11];
	char Name[11];
	BYTE btSlotNum;
	BYTE btStatus;
	BYTE btSequence;
};

struct PMSG_REQ_MURUMMY_DELETE_DS
{
	PBMSG_HEAD2 h;
	char AccountID[11];
	char Name[11];
};

struct PMSG_REQ_MURUMMY_SLOTUPDATE_DS
{
	PBMSG_HEAD2 h;
	char AccountID[11];
	char Name[11];
	MuRummyCardUpdateDS stCardUpdateDS;
};

struct PMSG_REQ_MURUMMY_INFO_UPDATE_DS
{
	PBMSG_HEAD2 h;
	char AccountID[11];
	char Name[11];
	WORD wScore;
	MuRummyCardUpdateDS stMuRummyCardUpdateDS[24];
};

struct PMSG_REQ_MURUMMY_LOG_INSERT_DS
{
	PBMSG_HEAD2 h;
	char AccountID[11];
	char Name[11];
	WORD wScore;
};

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
	void GDReqScoreUpdate(CGameObject lpObj, WORD wScore, MuRummyCardUpdateDS *pCardUpdateDS);
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

