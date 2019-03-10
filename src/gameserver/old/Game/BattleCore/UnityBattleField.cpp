////////////////////////////////////////////////////////////////////////////////
// UnityBattleField.cpp
#include "StdAfx.h"
#include "UnityBattleField.h"
#include "Main.h"
#include "GOFunctions.h"
#include "util.h"
#include "PersonalStore.h"
#include "iniReader.h"
#include "configread.h"
#include "GameProtocol.h"
#include "GOFunctions.h"


CUnityBattleField g_UnityBattleField;

CUnityBattleField::CUnityBattleField(void)
{
}

CUnityBattleField::~CUnityBattleField(void)
{
}

void CUnityBattleField::LoadData(char *FileName)
{
	CIniReader ReadBattleCore(FileName);

	this->m_bUBFEnable = ReadBattleCore.ReadInt("BattleCore", "IsBattleCoreEvent", 1);
	this->EventBannerOff = 0;//ReadBattleCore.ReadInt("BattleCore", "IsBattleCoreBanner", 1);
	this->m_iUBFPopupType = ReadBattleCore.ReadInt("BattleCore", "IsNoticeWindow", 1);
	this->m_bUBFCharacterPromotion = ReadBattleCore.ReadInt("BattleCore", "IsCharacterPromotion", 0);
}

void CUnityBattleField::SendUBFNotice(CGameObject &Obj)
{
	if (this->m_bUBFEnable == false)
	{
		return;
	}

	if (this->EventBannerOff == 1)
	{
		GSProtocol.GCSendEventBanner(&Obj, 3);
	}
}

void CUnityBattleField::GDReqJoinUnityBattleField(CGameObject &Obj)
{
	if (this->m_bUBFEnable == false)
	{
		sLog->outBasic("[UBF][GDReqJoinUnityBattleField][%s][%s][%s][ServerCode:%d] UBF not available.",
			Obj.m_PlayerData->ConnectUser->AccountID, Obj.m_PlayerData->m_RealNameOfUBF, Obj.Name, Obj.m_PlayerData->m_nServerCodeOfHomeWorld);

		return;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		sLog->outBasic("[UBF][GDReqJoinUnityBattleField][%s][%s][%s][ServerCode:%d] In UBF, Can not be Requested Join",
			Obj.m_PlayerData->ConnectUser->AccountID, Obj.m_PlayerData->m_RealNameOfUBF, Obj.Name, Obj.m_PlayerData->m_nServerCodeOfHomeWorld);

		return;
	}

	if (Obj.m_PlayerData->m_bMapSvrMoveQuit == true || Obj.m_PlayerData->m_bMapSvrMoveReq == true || Obj.m_PlayerData->m_bMapSvrMoveReq_2 == true)
	{
		sLog->outBasic("[UBF][GDReqJoinUnityBattleField][%s][%s][%s][ServerCode:%d] Map Of Move request was trying to Join UnityBattleField.",
			Obj.m_PlayerData->ConnectUser->AccountID, Obj.m_PlayerData->m_RealNameOfUBF, Obj.Name, Obj.m_PlayerData->m_nServerCodeOfHomeWorld);

		return;
	}

	if (Obj.m_IfState.use && Obj.m_IfState.type == 1)
	{
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 634), &Obj, 1);
		sLog->outBasic("[UBF][GDReqJoinUnityBattleField][%s][%s][%s][ServerCode:%d] Trading can't be to Join UnityBattleField.",
			Obj.m_PlayerData->ConnectUser->AccountID, Obj.m_PlayerData->m_RealNameOfUBF, Obj.Name, Obj.m_PlayerData->m_nServerCodeOfHomeWorld);

		return;
	}

	if (Obj.m_bPShopOpen == true)
	{
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 633), &Obj, 1);
		sLog->outBasic("[UBF][GDReqJoinUnityBattleField][%s][%s][%s][ServerCode:%d] Opened PShop can't be to Join UnityBattleField.",
			Obj.m_PlayerData->ConnectUser->AccountID, Obj.m_PlayerData->m_RealNameOfUBF, Obj.Name, Obj.m_PlayerData->m_nServerCodeOfHomeWorld);

		return;
	}

	sLog->outBasic("[UBF][GDReqJoinUnityBattleField][%s][%s] Request To Join UnityBattleField",
		Obj.m_PlayerData->ConnectUser->AccountID, Obj.Name);
	//GJSetCharacterInfo(Obj, Obj.m_Index, 0); // TODO

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	PMSG_UBF_REGISTER_ACCOUNT_USER pMsg;
	std::memcpy(pMsg.szAccountID, Obj.m_PlayerData->ConnectUser->AccountID, MAX_ACCOUNT_LEN + 1);
	std::memcpy(pMsg.szName, Obj.Name, MAX_ACCOUNT_LEN + 1);
	std::memcpy(pMsg.szBattleFieldName, Obj.Name, MAX_ACCOUNT_LEN + 1);

	pMsg.iUserIndex = Obj.m_Index;
	pMsg.ServerCode = g_ConfigRead.server.GetGameServerCode() / 20;
	pMsg.btRegisterState = 1;
	pMsg.btRegisterMonth = sysTime.wMonth;
	pMsg.btRegisterDay = sysTime.wDay;

	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x02, sizeof(pMsg));
	//wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
}

void CUnityBattleField::DGAnsJoinUnityBattleField(CGameObject &Obj, BYTE Result, WORD LeftSecond)
{

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	sLog->outBasic("[UBF][DGAnsJoinUnityBattleField][%s][%s] Answer is To Join UBF ResultCode: %d (0:NewJoin Succ/1:ReJoin Succ/2:Joined State/3:Already apply/4:2 PersonUpper/5:Server Limit)",
		Obj.m_PlayerData->ConnectUser->AccountID, Obj.Name, Result);

	PMSG_ANS_UBF_JOIN pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xCD, 0x02, sizeof(pMsg));

	pMsg.btResult = Result;
	pMsg.nLeftSec = LeftSecond;
	GIOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);

	if (Result != 1 && Result != 0)
	{
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 632), &Obj, 1);
		return;
	}

	GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 631), &Obj, 1);
	Obj.m_PlayerData->m_JoinUnityBattle = true;

	if (Obj.m_bPShopOpen == true)
	{
		g_PersonalStore.CGPShopReqClose(Obj);
	}

	this->GDReqCopyCharacterInfo(Obj, 0);
}

void CUnityBattleField::GDReqCopyCharacterInfo(CGameObject &Obj, BYTE CharacterSlot)
{

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		sLog->outBasic("[UBF][GDReqCopyCharcterInfo][%s][%s] Move(Copy Off) Fail, Don't Move because UBFServer",
			Obj.m_PlayerData->ConnectUser->AccountID, Obj.Name);
		return;
	}

	PMSG_UBF_ACCOUNT_USER_COPY pMsg;
	std::memcpy(pMsg.szAccountID, Obj.m_PlayerData->ConnectUser->AccountID, MAX_ACCOUNT_LEN + 1);
	std::memcpy(pMsg.szName, Obj.Name, MAX_ACCOUNT_LEN + 1);
	pMsg.iUserIndex = Obj.m_Index;
	pMsg.ServerCode = g_ConfigRead.server.GetGameServerCode() / 20;
	pMsg.btPromotionCode = this->m_bUBFCharacterPromotion;

	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x03, sizeof(pMsg));
	//wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);

	sLog->outBasic("[UBF][GDReqCopyCharcterInfo][%s][%s] Move(Copy On) the Character Into UnityBattleField ",
		Obj.m_PlayerData->ConnectUser->AccountID, Obj.Name);

	GDReqCopyPetItemInfo(Obj);
}

// UPTO

void CUnityBattleField::DGAnsCopyCharacterInfo(CGameObject &Obj, BYTE result, BYTE subResult)
{

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (result != TRUE)
	{
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 630), Obj.m_Index, 1);
		sLog->outBasic("[UBF][DGAnsCopyCharcterInfo][%s][%s] Character Move(Copy Fail) Result: %d (2,3:Fail), ErrCode: %d (2:No UserInfo / 3: No CharSlot)",
			Obj.m_PlayerData->ConnectUser->AccountID, Obj.Name, result, subResult);
		return;
	}

	GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 629), Obj.m_Index, 1);
	sLog->outBasic("[UBF][DGAnsCopyCharcterInfo][%s][%s] Character Move(Copy) Result: %d (1:Succes) ",
		Obj.m_PlayerData->ConnectUser->AccountID, Obj.Name, result);

	sLog->outBasic("[UBF][DGAnsCopyCharcterInfo][Copy Complete][%s][%s] CharInfoSave : Class=%d Level=%d LVPoint=%d Exp=%I64d Str=%d Dex=%d Vit=%d Energy=%d Leadership:%d Map=%d Pk=%d",
		Obj.m_PlayerData->ConnectUser->AccountID, Obj.Name, Obj.m_PlayerData->DbClass, Obj.Level, Obj.m_PlayerData->LevelUpPoint, Obj.m_PlayerData->Experience, Obj.m_PlayerData->Strength, Obj.m_PlayerData->Dexterity,
		Obj.m_PlayerData->Vitality, Obj.m_PlayerData->Energy, Obj.Leadership, Obj.MapNumber, Obj.m_PK_Level);
	gObjItemTextSave(lpObj);
	gObjMagicTextSave(lpObj);

	sLog->outBasic("[UBF][DGAnsCopyCharacterInfo][%s][%s] Copy Infomation End", Obj.m_PlayerData->ConnectUser->AccountID, Obj.Name);
}

void CUnityBattleField::GDReqCheckJoinedUnityBattleField(CGameObject &Obj, int IsUnityBattleFieldServer, BYTE ObServerMode)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	PMSG_REQ_UBF_ACCOUNT_USERINFO pMsg;
	std::memcpy(pMsg.szAccountID, Obj.m_PlayerData->ConnectUser->AccountID, MAX_ACCOUNT_LEN + 1);
	std::memcpy(pMsg.szName, Obj.Name, MAX_ACCOUNT_LEN + 1);

	pMsg.iUserIndex = Obj.m_Index;
	pMsg.iServerCode = g_ConfigRead.server.GetGameServerCode() / 20;
	pMsg.IsUnityBattleFieldServer = g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE ? TRUE : FALSE;
	pMsg.btObserverMode = ObServerMode;

	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x01, sizeof(pMsg));
	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
}
void CUnityBattleField::DGAnsCheckJoinedUnityBattleField(CGameObject &Obj, BYTE btRegisterState)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (btRegisterState == TRUE)
	{
		Obj.m_PlayerData->m_JoinUnityBattle = true;
		if (Obj.m_bPShopOpen == true)
		{
			g_PersonalStore.CGPShopReqClose(Obj.m_Index);
		}
	}

	else
	{
		Obj.m_PlayerData->m_JoinUnityBattle = false;
	}

	PMSG_ANS_UBF_INFO pMsg;
	PHeadSubSetB((BYTE*)&pMsg, 0xCD, 0x01, sizeof(pMsg));
	pMsg.btResult = btRegisterState;

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pMsg, pMsg.h.size);
}

void CUnityBattleField::GDObserverLogoutManager()
{
	for (int n = g_ConfigRead.server.GetObjectStartUserIndex(); n < g_ConfigRead.server.GetObjectMax(); n++)
	{
		CGameObject* lpObj = getGameObject(n);

		if (gObjIsConnected(*lpObj) == TRUE && lpObj->Type == OBJ_USER)
		{
			this->GDReqCheckJoinedUnityBattleField(n, g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE ? TRUE : FALSE, TRUE);
		}
	}
}

void CUnityBattleField::GDReqCancelUnityBattleField(CGameObject &Obj, BYTE btCancelType)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		sLog->outBasic("[UBF][GDReqCancelUnityBattleField][%s][%s] In UBF, Can not be canceled", Obj.m_PlayerData->ConnectUser->AccountID, Obj.Name);
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 628), Obj.m_Index, 1);
		return;
	}

	PMSG_UBF_REQ_CANCEL_REGISTER_USER pMsg;
	std::memcpy(pMsg.szAccountID, Obj.m_PlayerData->ConnectUser->AccountID, MAX_ACCOUNT_LEN + 1);
	std::memcpy(pMsg.szName, Obj.Name, MAX_ACCOUNT_LEN + 1);
	pMsg.iUserIndex = Obj.m_Index;
	pMsg.ServerCode = g_ConfigRead.server.GetGameServerCode() / 20;
	pMsg.btCanceled = btCancelType;

	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x07, sizeof(pMsg));
	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);;

	sLog->outBasic("[UBF][GDReqCancelUnityBattleField][%s][%s] Request to Cancel UBF",
		Obj.m_PlayerData->ConnectUser->AccountID, Obj.Name);
}

void CUnityBattleField::GDReqCancelUnityBattleField(CGameObject &Obj, BYTE btCancelType, const char *name)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		sLog->outBasic("[UBF][GDReqCancelUnityBattleField][%s][%s] In UBF, Can not be canceled", Obj.m_PlayerData->ConnectUser->AccountID, Obj.Name);
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 628), Obj.m_Index, 1);
		return;
	}

	PMSG_UBF_REQ_CANCEL_REGISTER_USER pMsg;
	std::memcpy(pMsg.szAccountID, Obj.m_PlayerData->ConnectUser->AccountID, MAX_ACCOUNT_LEN + 1);
	std::memcpy(pMsg.szName, name, MAX_ACCOUNT_LEN + 1);
	pMsg.iUserIndex = Obj.m_Index;
	pMsg.ServerCode = g_ConfigRead.server.GetGameServerCode() / 20;
	pMsg.btCanceled = btCancelType;

	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x06, sizeof(pMsg));
	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);;

	sLog->outBasic("[UBF][GDReqCancelUnityBattleField][%s][%s] Request to Cancel UBF",
		Obj.m_PlayerData->ConnectUser->AccountID, Obj.Name);
}

void CUnityBattleField::DGAnsCancelUnityBattleField(CGameObject &Obj, BYTE aCanceledResult, BYTE deletedResult)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (aCanceledResult == TRUE)
	{
		GSProtocol.GCServerMsgStringSend(Lang.GetText(0, 627), Obj.m_Index, 1);
		Obj.m_PlayerData->m_JoinUnityBattle = false;
	}

	sLog->outBasic("[UBF][DGAnsCancelUnityBattleField][%s][%s] Answer CancelUnityBattleField CancelResult:%d (1:Success), deleteResult:%d (1:Success) ",
		Obj.m_PlayerData->ConnectUser->AccountID, Obj.Name, aCanceledResult, deletedResult);

	PMSG_ANS_UBF_CANCEL pResult;
	PHeadSubSetB((BYTE*)&pResult, 0xCD, 0x07, sizeof(pResult));
	pResult.btResult = aCanceledResult;

	IOCP.DataSend(Obj.m_Index, (BYTE*)&pResult, pResult.h.size);
}

void CUnityBattleField::GDReqGetRealNameAndServerCode(CGameObject &Obj)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (g_ConfigRead.server.GetServerType() != SERVER_BATTLECORE)
	{
		return;
	}

	PMSG_REQ_GET_UBF_REAL_NAME pMsg;
	pMsg.iUserIndex = Obj.m_Index;
	std::memcpy(pMsg.szUBFName, Obj.Name, MAX_ACCOUNT_LEN + 1);

	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x08, sizeof(pMsg));
	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);
}

void CUnityBattleField::DGAnsGetRealNameAndServerCode(CGameObject &Obj, int nServerCodeOfHomeWorld)
{

}

void CUnityBattleField::GDReqUBFGetReward(CGameObject &Obj, BYTE btBattleKind)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (gObjIsConnected(Obj.m_Index) == FALSE)
	{
		return;
	}

	if (g_ConfigRead.server.GetServerType() == SERVER_BATTLECORE)
	{
		return;
	}

	PMSG_REQ_UBF_GET_REWARD pMsg;
	pMsg.iUserIndex = Obj.m_Index;
	pMsg.iServerCode = g_ConfigRead.server.GetGameServerCode() / 20;
	pMsg.btBattleKind = btBattleKind;
	std::memcpy(pMsg.szAccountID, Obj.m_PlayerData->ConnectUser->AccountID, MAX_ACCOUNT_LEN + 1);
	std::memcpy(pMsg.szName, Obj.Name, MAX_ACCOUNT_LEN + 1);

	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x06, sizeof(pMsg));
	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);

	sLog->outBasic("[UBF][GDReqUBFGetReward][%d][%s][%s] UnityBattleFiled is asking WinnerItem",
		Obj.m_Index, Obj.m_PlayerData->ConnectUser->AccountID, Obj.Name);
}

void CUnityBattleField::GDReqSetReceivedWinnerItem(CGameObject &Obj, BYTE btBattleKind)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (gObjIsConnected(Obj.m_Index) == FALSE)
	{
		return;
	}

	PMSG_REQ_UBF_SET_RECEIVED_REWARD pMsg;
	pMsg.iUserIndex = Obj.m_Index;
	pMsg.iServerCode = g_ConfigRead.server.GetGameServerCode() / 20;
	std::memcpy(pMsg.szAccountID, Obj.m_PlayerData->ConnectUser->AccountID, MAX_ACCOUNT_LEN + 1);
	std::memcpy(pMsg.szName, Obj.Name, MAX_ACCOUNT_LEN + 1);
	pMsg.btReceivedReward = TRUE;
	pMsg.btBattleKind = btBattleKind;

	PHeadSubSetB((BYTE*)&pMsg, 0xF3, 0x05, sizeof(pMsg));
	wsDataCli.DataSend((char *)&pMsg, pMsg.h.size);

	sLog->outBasic("[UBF][GDReqSetReceivedWinnerItem] Index:%d ID:%s Name:%s RequestSetReceived WinnerItem ",
		Obj.m_Index, Obj.m_PlayerData->ConnectUser->AccountID, Obj.Name);
}

void CUnityBattleField::DGAnsSetReceivedWinnerItem(CGameObject &Obj, BYTE btReturn)
{
	if (!ObjectMaxRange(Obj.m_Index))
	{
		return;
	}

	

	if (Obj.Type != OBJ_USER)
	{
		return;
	}

	if (gObjIsConnected(Obj.m_Index) == FALSE)
	{
		return;
	}

	if (btReturn == TRUE)
	{
		sLog->outBasic("[UBF][DGAnsSetReceivedWinnerItem] Index:%d ID:%s Name:%s Received WinnerItem Success",
			Obj.m_Index, Obj.m_PlayerData->ConnectUser->AccountID, Obj.Name);
	}

	else
	{
		sLog->outBasic("[UBF][DGAnsSetReceivedWinnerItem] Index:%d ID:%s Name:%s WinnerItem Failed to complete the process ErrCode :%d",
			Obj.m_Index, Obj.m_PlayerData->ConnectUser->AccountID, Obj.Name, btReturn);
	}
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

