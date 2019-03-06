////////////////////////////////////////////////////////////////////////////////
// QuestExpProtocol.cpp
#include "StdAfx.h"
#include "Logging/Log.h"
#include "QuestExpProtocol.h"
#include "QuestExpUserMng.h"
#include "QuestExpLuaBind.h"
#include "User/CUserData.h"
#include "QuestExpProgMng.h"
#include "NpcTalk.h"
#include "util.h"
#include "Main.h"

void CGReqQuestSwitch(PMSG_REQ_QUESTEXP *pMsg, CGameObject &Obj)
{
    if( !ObjectMaxRange(Obj.m_Index) )
        return;

    CGameObject* lpObj = getGameObject(Obj.m_Index);

    if( !gObjIsConnected(Obj.m_Index) )
        return;

    if( !g_QuestExpProgMng.ChkQuestIndexIDToEpLimit(pMsg->dwQuestInfoIndexID, Obj.m_Index) )
    {
        sLog->outBasic("[QuestExp] - Error - [%s] [%d]", __FILE__, __LINE__);
        return;
    }

	sLog->outBasic("[QuestExp] Selection Episode List Choose One [%s][%s] QuestInfoIndexID[0x%x] Choose[%d]",
		Obj.AccountID, Obj.Name, pMsg->dwQuestInfoIndexID, pMsg->btResult);

	lua_State* L = g_MuLuaQuestExp.GetLua();

	if (!L)
	{
		sLog->outBasic("[QuestExp] - Error - [%s] [%d]", __FILE__, __LINE__);
		return;
	}

	if (g_QuestExpUserMng.IsQuestAccept(pMsg->dwQuestInfoIndexID, Obj.m_Index))
	{
		g_MuLuaQuestExp.Generic_Call("CGReqQuestSwitch", "iii>", (int)pMsg->dwQuestInfoIndexID, (int)pMsg->btResult, Obj.m_Index);
	}

    else
    {
        PMSG_ANS_QUESTEXP pAnsMsg;
        pAnsMsg.btResult = 1;

        PHeadSubSetB((BYTE*)&pAnsMsg, 0xF6, 0x00, sizeof(pAnsMsg));
        IOCP.DataSend(Obj.m_PlayerData->ConnectUser->Index, (BYTE*)&pAnsMsg, sizeof(pAnsMsg));
    }
}

void CGReqQuestProgress(PMSG_REQ_QUESTEXP *pMsg, CGameObject &Obj)
{
    if( !ObjectMaxRange(Obj.m_Index) )
        return;

    CGameObject* lpObj = getGameObject(Obj.m_Index);

    if( !gObjIsConnected(Obj.m_Index) )
        return;

    if( !g_QuestExpProgMng.ChkQuestIndexIDToEpLimit(pMsg->dwQuestInfoIndexID, Obj.m_Index) )
    {
        sLog->outBasic("[QuestExp] - Error - [%s] [%d]", __FILE__, __LINE__);
        return;
    }

    sLog->outBasic("[QuestExp] Selection Statements Choose One - User NPC Talking [%s][%s] QuestInfoIndexID[0x%x] Choose[%d]",
		Obj.AccountID, Obj.Name, pMsg->dwQuestInfoIndexID,pMsg->btResult);

	lua_State* L = g_MuLuaQuestExp.GetLua();

	if (!L)
	{
		sLog->outBasic("[QuestExp] - Error - [%s] [%d]", __FILE__, __LINE__);
		return;
	}

	g_MuLuaQuestExp.Generic_Call("CGReqQuestProgress", "iii>", (int)pMsg->dwQuestInfoIndexID, (int)pMsg->btResult, Obj.m_Index);
}

void CGReqQuestComplete(PMSG_REQ_QUESTEXP_COMPLETE *pMsg, CGameObject &Obj)
{
    if( !ObjectMaxRange(Obj.m_Index) )
        return;

    CGameObject* lpObj = getGameObject(Obj.m_Index);

    if( !gObjIsConnected(Obj.m_Index) )
        return;

    if( !g_QuestExpProgMng.ChkQuestIndexIDToEpLimit(pMsg->dwQuestInfoIndexID, Obj.m_Index) )
    {
        sLog->outBasic("[QuestExp] - Error - [%s] [%d]", __FILE__, __LINE__);
        return;
    }

    sLog->outBasic("[QuestExp] ReqQuestComplete [%s][%s] QuestInfoIndexID[0x%x]", Obj.AccountID, Obj.Name, pMsg->dwQuestInfoIndexID);

	lua_State* L = g_MuLuaQuestExp.GetLua();

	if (!L)
	{
		sLog->outBasic("[QuestExp] - Error - [%s] [%d]", __FILE__, __LINE__);
		return;
	}

	g_MuLuaQuestExp.Generic_Call("CGReqQuestComplete", "ii>", (int)pMsg->dwQuestInfoIndexID, Obj.m_Index);
}

void CGReqQuestGiveUp(PMSG_REQ_QUESTEXP_GIVEUP *pMsg, CGameObject &Obj)
{
    if( !ObjectMaxRange(Obj.m_Index) )
        return;

    if( !gObjIsConnected(Obj.m_Index) )
        return;

    if( !g_QuestExpProgMng.ChkQuestIndexIDToEpLimit(pMsg->dwQuestInfoIndexID, Obj.m_Index) )
    {
        sLog->outBasic("[QuestExp] - Error - [%s] [%d]", __FILE__, __LINE__);
        return;
    }

    g_QuestExpProgMng.QuestExpGiveUpBtnClick(pMsg->dwQuestInfoIndexID, Obj.m_Index);
}

void CGReqTutorialKeyComplete(PMSG_REQ_QUESTEXP_ASK_COMPLETE *pMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
        return;

    if( !gObjIsConnected(Obj.m_Index) )
        return;

    if( !g_QuestExpProgMng.ChkQuestIndexIDToEpLimit(pMsg->dwQuestInfoIndexID, Obj.m_Index) )
    {
        sLog->outBasic("[QuestExp] - Error - [%s] [%d]", __FILE__, __LINE__);
        return;
    }

    g_QuestExpProgMng.ReqQuestAskComplete(pMsg->dwQuestInfoIndexID, Obj.m_Index);
}

void CGReqProgressQuestList(PMSG_REQ_QUESTEXP_PROGRESS_LIST *pMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
        return;

    if( !gObjIsConnected(Obj.m_Index) )
        return;

    g_QuestExpProgMng.SendProgressQuestList(Obj.m_Index);
}

void CGReqProgressQuestInfo(PMSG_REQ_QUESTEXP_PROGRESS_INFO *pMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
        return;

    if( !gObjIsConnected(Obj.m_Index) )
        return;

    if( !g_QuestExpProgMng.ChkQuestIndexIDToEpLimit(pMsg->dwQuestInfoIndexID, Obj.m_Index) )
    {
        sLog->outBasic("[QuestExp] - Error - [%s] [%d]", __FILE__, __LINE__);
        return;

    }    

    g_QuestExpProgMng.SendQuestProgressInfo(pMsg->dwQuestInfoIndexID, Obj.m_Index);
}

void CGReqEventItemQuestList(PMSG_REQ_EVENT_ITEM_EP_LIST *pMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
        return;

    if( !gObjIsConnected(Obj.m_Index) )
        return;

	lua_State* L = g_MuLuaQuestExp.GetLua();

	if (!L)
	{
		sLog->outBasic("[QuestExp] - Error - [%s] [%d]", __FILE__, __LINE__);
		return;
	}

	g_MuLuaQuestExp.Generic_Call("ItemAndEvent", "i>", Obj.m_Index);
}

void CGReqQuestExp(PMSG_REQ_NPC_QUESTEXP *pMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
        return;

    CGameObject* lpObj = getGameObject(Obj.m_Index);

    if( !gObjIsConnected(Obj.m_Index) )
        return;

	if( !ObjectMaxRange(Obj.TargetNpcNumber))
		return;

    if( Obj.m_PlayerData->m_bUserQuestInfoSent == 1 )
    {
		lua_State* L = g_MuLuaQuestExp.GetLua();

		if (!L)
		{
			sLog->outBasic("[QuestExp] - Error - [%s] [%d]", __FILE__, __LINE__);
			return;
		}

		g_MuLuaQuestExp.Generic_Call("NpcTalkClick", "ii>", (int)getGameObject(Obj.TargetNpcNumber)->Class, Obj.m_Index);
    }
}

void CGReqAttDefPowerInc(PMSG_REQ_ATTDEF_POWER_INC *pMsg, CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
        return;

    if( !gObjIsConnected(Obj.m_Index) )
        return;
	if (Obj.TargetNpcNumber == -1)
	{
		//IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
		return;
	}

	if (Obj.CloseType != -1)
	{
		return;
	}
	//[K2] ANTIHACK NPC BUFF TAKE
	if (Obj.m_IfState->use > 0 && Obj.m_IfState->type == 33)
    NpcShadowPhantom(Obj.m_Index);
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

