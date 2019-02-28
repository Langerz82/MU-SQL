////////////////////////////////////////////////////////////////////////////////
// QuestExpProtocol.h
#ifndef QUESTEXPPROTOCOL_H
#define QUESTEXPPROTOCOL_H

#pragma once
#include "prodef.h"

struct PMSG_REQ_QUESTEXP
{
    PBMSG_HEAD2 h;
    DWORD dwQuestInfoIndexID;
    BYTE btResult;
};

struct PMSG_ANS_QUESTEXP
{
    PBMSG_HEAD2 h;
    BYTE btResult;
};

struct PMSG_REQ_QUESTEXP_COMPLETE
{
    PBMSG_HEAD2 h;
    DWORD dwQuestInfoIndexID;
};

struct PMSG_REQ_QUESTEXP_GIVEUP
{
    PBMSG_HEAD2 h;
    DWORD dwQuestInfoIndexID;
};

struct PMSG_REQ_QUESTEXP_ASK_COMPLETE
{
    PBMSG_HEAD2 h;
    DWORD dwQuestInfoIndexID;
};

struct PMSG_REQ_QUESTEXP_PROGRESS_LIST
{
    PBMSG_HEAD2 h;
    DWORD dwQuestInfoIndexID;
};

struct PMSG_REQ_QUESTEXP_PROGRESS_INFO
{
    PBMSG_HEAD2 h;
    DWORD dwQuestInfoIndexID;
};

struct PMSG_REQ_EVENT_ITEM_EP_LIST
{
    PBMSG_HEAD2 h;
};

struct PMSG_REQ_NPC_QUESTEXP
{
    PBMSG_HEAD2 h;
};

struct PMSG_REQ_ATTDEF_POWER_INC
{
    PBMSG_HEAD2 h;
};

void CGReqQuestSwitch(PMSG_REQ_QUESTEXP *pMsg, CGameObject &Obj);
void CGReqQuestProgress(PMSG_REQ_QUESTEXP *pMsg, CGameObject &Obj);
void CGReqQuestComplete(PMSG_REQ_QUESTEXP_COMPLETE *pMsg, CGameObject &Obj);
void CGReqQuestGiveUp(PMSG_REQ_QUESTEXP_GIVEUP *pMsg, CGameObject &Obj);
void CGReqTutorialKeyComplete(PMSG_REQ_QUESTEXP_ASK_COMPLETE *pMsg, CGameObject &Obj);
void CGReqProgressQuestList(PMSG_REQ_QUESTEXP_PROGRESS_LIST *pMsg, CGameObject &Obj);
void CGReqProgressQuestInfo(PMSG_REQ_QUESTEXP_PROGRESS_INFO *pMsg, CGameObject &Obj);
void CGReqEventItemQuestList(PMSG_REQ_EVENT_ITEM_EP_LIST *pMsg, CGameObject &Obj);

void CGReqQuestExp(PMSG_REQ_NPC_QUESTEXP *pMsg, CGameObject &Obj);
void CGReqAttDefPowerInc(PMSG_REQ_ATTDEF_POWER_INC *pMsg, CGameObject &Obj);

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

