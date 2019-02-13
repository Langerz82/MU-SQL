////////////////////////////////////////////////////////////////////////////////
// Event.h
#ifndef ___EVENT_H
#define ___EVENT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"

#include "User/CUserData.h"

struct PBMSG_HEAD;
struct PMSG_ANS_CL_ENTERCOUNT;
struct PMSG_ANS_LUCKYCOININFO;
struct PMSG_ANS_LUCKYCOIN_REGISTER;
struct PMSG_EVENTCHIPINFO;
struct PMSG_GETMUTONUMBER_RESULT;
struct PMSG_REGEVENTCHIP_RESULT;
struct PMSG_REQ_BLOODCASTLE_ENTERCOUNT;
struct PMSG_REQ_REG_CC_OFFLINE_GIFT;
struct PMSG_REQ_REG_DL_OFFLINE_GIFT;
struct PMSG_REQ_REG_HT_OFFLINE_GIFT;
struct PMSG_REQ_SANTAGIFT;
struct PMSG_SERVERCMD;

/*
struct PMSG_REGEVENTCHIP_RESULT
{
	PBMSG_HEAD2 h;	// C1:95
	BYTE Type;	// 3
	int ChipCount;	// 4
};

struct PMSG_GETMUTONUMBER_RESULT
{
	PBMSG_HEAD2 h;	// C1:96
	short MutoNum[3];	// 4
};

struct PMSG_EVENTCHIPINFO
{
	PBMSG_HEAD2 h;	// C1:94
	BYTE Type;	// 3
	int ChipCount;	// 4
	short MutoNum[3];	// 6
};

struct PMSG_LUCKYCOININFO
{
	PBMSG_HEAD2 h;
	int LuckyCoin;
};

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

struct PMSG_ANS_VIEW_EC_MN
{
	PBMSG_HEAD2 h;
	int iINDEX;	// 4
	char szUID[11];	// 8
	char bSUCCESS;	// 13
	short nEVENT_CHIPS;	// 14
	int iMUTO_NUM;	// 18
};

struct PMSG_ANS_REGISTER_EVENTCHIP
{
	PBMSG_HEAD2 h;
	int iINDEX;	// 4
	BYTE Pos;	// 8
	char szUID[11];	// 9
	char bSUCCESS;	// 14
	short nEVENT_CHIPS;	// 16
};


struct PMSG_ANS_REGISTER_MUTONUM
{
	PBMSG_HEAD2 h;
	int iINDEX;	// 4
	char szUID[11];	// 8
	char bSUCCESS;	// 13
	int iMUTO_NUM;	// 14
};


struct PMSG_ANS_RESET_EVENTCHIP
{
	PBMSG_HEAD2 h;
	int iINDEX;	// 4
	char szUID[11];	// 8
	char bSUCCESS;	// 13
};

struct PMSG_ANS_VIEW_STONES
{
	PBMSG_HEAD2 h;
	int iINDEX;	// 4
	char szUID[11];	// 8
	char bSUCCESS;	// 13
	int iStoneCount;	// 14
};

struct PMSG_ANS_REGISTER_STONES
{
	PBMSG_HEAD2 h;
	int iINDEX;	// 4
	BYTE iPosition;	// 8
	char szUID[11];	// 9
	char bSUCCESS;	// 14
	int iStoneCount;	// 18
};

struct PMSG_ANS_DELETE_STONES
{
	PBMSG_HEAD2 h;
	int iINDEX;	// 4
	char szUID[11];	// 8
	char bSUCCESS;	// 13
	int iStoneCount;	// 14
};

struct PMSG_ANS_2ANIV_SERIAL
{
	PBMSG_HEAD2 h;
	int iINDEX;	// 4
	char szUID[11];	// 8
	BYTE btIsRegistered;	// 13
	int iGiftNumber;	// 14
};

struct PMSG_ANS_REG_RINGGIFT
{
	PBMSG_HEAD2 h;
	int iINDEX;	// 4
	char szUID[11];	// 8
	BYTE btIsRegistered;	// 13
	BYTE btGiftSection;	// 14
	BYTE btGiftKind;	// 15
};

struct PMSG_ANS_BLOODCASTLE_ENTERCOUNT
{
	PBMSG_HEAD2 h;
	char AccountID[10];	// 3
	char GameID[10];	// D
	int ServerCode;	// 18
	int iObjIndex;	// 1C
	int iLeftCount;	// 20
};

struct PMSG_ANS_REG_CC_OFFLINE_GIFT
{
	PBMSG_HEAD2 h;
	int iINDEX;	// 4
	char szUID[11];	// 8
	WORD wServerCode;	// 14
	char szNAME[11];	// 16
	int iResultCode;	// 24
	char szGIFT_NAME[50];	/// 28
};

struct PMSG_ANS_REG_DL_OFFLINE_GIFT
{
	PBMSG_HEAD2 h;
	int iINDEX;	// 4
	char szUID[11];	// 8
	WORD wServerCode;	// 14
	char szNAME[11];	// 16
	int iResultCode;	// 24
	char szGIFT_NAME[50];	/// 28
};

struct PMSG_ANS_REG_HT_OFFLINE_GIFT
{
	PBMSG_HEAD2 h;
	int iINDEX;	// 4
	char szUID[11];	// 8
	WORD wServerCode;	// 14
	char szNAME[11];	// 16
	int iResultCode;	// 24
	char szGIFT_NAME[50];	/// 28

};

struct PMSG_ANS_REG_LUCKYCOIN
{
	PBMSG_HEAD2 h;
	int iIndex;
	char szUID[11];
	BYTE Result;
	BYTE Pos;
	int LuckyCoins;
};

struct PMSG_REQ_LUCKYCOIN
{
	PBMSG_HEAD2 h;
	int iIndex;
	char szUID[11];
};

struct PMSG_ANS_LUCKYCOIN
{
	PBMSG_HEAD2 h;
	int iIndex;
	char szUID[11];
	int LuckyCoins;
};

struct PMSG_ANS_SANTACHECK
{
	PBMSG_HEAD2 h;
	char AccountID[11];
	short gGameServerCode;
	CGameObject &lpObj;
	WORD Result;
	WORD UseCount;
};

struct PMSG_REQ_SANTACHECK
{
	PBMSG_HEAD2 h;
	char AccountID[11];
	short gGameServerCode;
	CGameObject &lpObj;
};

struct PMSG_REQ_SANTAGIFT
{
	PBMSG_HEAD2 h;
	char AccountID[11];
	short gGameServerCode;
	CGameObject &lpObj;
};

struct PMSG_ANS_SANTAGIFT
{
	PBMSG_HEAD2 h;
	char AccountID[11];
	short gGameServerCode;
	CGameObject &lpObj;
	WORD Result;
	WORD UseCount;
};
*/

extern CGameObject pEventObj;

void EventChipEventProtocolCore(BYTE protoNum, LPBYTE aRecv, int aLen);


void FireworksOpenEven(CGameObject &lpObj);
void ChristmasFireCrackDrop(CGameObject &lpObj);

void HallowinDayEventItemBoxOpen(CGameObject &lpObj);
void KanturuMayaHandItemBagOpen(CGameObject &lpObj, BYTE btMapNumber, BYTE cX, BYTE cY);
void KanturuNightmareItemBagOpen(CGameObject &lpObj, BYTE btMapNumber, BYTE cX, BYTE cY);
void EGRecvEventChipInfo(PMSG_ANS_VIEW_EC_MN * aRecv);
void EGResultRegEventChip(PMSG_ANS_REGISTER_EVENTCHIP * aRecv);
void EGRecvRegMutoNum( PMSG_ANS_REGISTER_MUTONUM* aRecv);
void EGRecvChangeRena( PMSG_ANS_RESET_EVENTCHIP* aRecv);
void EGRecvStoneInfo( PMSG_ANS_VIEW_STONES* aRecv);
void EGRecvRegStone( PMSG_ANS_REGISTER_STONES* aRecv);
void EGRecvDeleteStone( PMSG_ANS_DELETE_STONES* aRecv);
void EGRecvChangeStones( PMSG_ANS_RESET_EVENTCHIP* aRecv);
void EGRecv2AnvRegSerial( PMSG_ANS_2ANIV_SERIAL* aRecv);
void EGRecvRegRingGift( PMSG_ANS_REG_RINGGIFT* aRecv);
void EGReqBloodCastleEnterCount(CGameObject &Obj);
void EGAnsBloodCastleEnterCount( PMSG_ANS_BLOODCASTLE_ENTERCOUNT* lpMsg);
void EGReqRegCCOfflineGift(CGameObject &Obj);
void EGAnsRegCCOfflineGift( PMSG_ANS_REG_CC_OFFLINE_GIFT* lpMsg);
void EGReqRegDLOfflineGift(CGameObject &Obj);
void EGAnsRegDLOfflineGift( PMSG_ANS_REG_DL_OFFLINE_GIFT* lpMsg);
void EGReqRegHTOfflineGift(CGameObject &Obj);
void EGAnsRegHTOfflineGift( PMSG_ANS_REG_HT_OFFLINE_GIFT* lpMsg);
void EGAnsRegLuckyCoin(PMSG_ANS_REG_LUCKYCOIN * lpMsg);
void EGAnsLuckyCoinInfo(PMSG_ANS_LUCKYCOIN * lpMsg);
void EGReqSantaGift(CGameObject &lpObj);
void EGAnsSantaCheck(PMSG_ANS_SANTACHECK *lpMsg);
void EGAnsSantaGift(PMSG_ANS_SANTAGIFT *lpMsg);

extern CGameObject pEventObj;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

