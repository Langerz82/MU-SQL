// Eprotocol.h: interface for the Eprotocol class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EPROTOCOL_H__25F5CC2C_9902_45AA_904D_F6DC2D9B01EA__INCLUDED_)
#define AFX_EPROTOCOL_H__25F5CC2C_9902_45AA_904D_F6DC2D9B01EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "ProtocolStructs.h"

void EProtocolCore(CGameObject &lpObj, BYTE HeadCode, LPBYTE aRecv, int iSize);
void InitEventDB();

void EGAnsEventChipInfo(CGameObject &lpObj,PMSG_REQ_VIEW_EC_MN * lpMsg);
void EGAnsRegEventChipInfo(CGameObject &lpObj,PMSG_REQ_REGISTER_EVENTCHIP * lpMsg);
void EGAnsResetEventChip(CGameObject &lpObj,PMSG_REQ_RESET_EVENTCHIP * lpMsg);
void EGAnsEventStoneInfo(CGameObject &lpObj,PMSG_REQ_VIEW_EC_MN * lpMsg);
void EGAnsRegEventStoneInfo(CGameObject &lpObj,PMSG_REQ_REGISTER_STONES * lpMsg);
void EGAnsResetStoneInfo(CGameObject &lpObj,PMSG_REQ_RESET_EVENTCHIP * lpMsg);
void EGAns2AnivRegSerial(CGameObject &lpObj,PMSG_REQ_2ANIV_SERIAL * lpMsg);
void EGAnsDeleteStones(CGameObject &lpObj,PMSG_REQ_DELETE_STONES * lpMsg);
void EGAnsRegCCOfflineGift(CGameObject &lpObj,PMSG_REQ_REG_CC_OFFLINE_GIFT * lpMsg);
void EGAnsRegDLOfflineGift(CGameObject &lpObj,PMSG_REQ_REG_DL_OFFLINE_GIFT * lpMsg);
void EGAnsRegHTOfflineGift(CGameObject &lpObj,PMSG_REQ_REG_HT_OFFLINE_GIFT * lpMsg);
void EGAnsLuckyCoinInfo(CGameObject &lpObj,PMSG_REQ_LUCKYCOIN * lpMsg);
void EGAnsRegLuckyCoin(CGameObject &lpObj,PMSG_REQ_REGISTER_LUCKYCOIN * lpMsg);

#endif // !defined(AFX_EPROTOCOL_H__25F5CC2C_9902_45AA_904D_F6DC2D9B01EA__INCLUDED_)


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

