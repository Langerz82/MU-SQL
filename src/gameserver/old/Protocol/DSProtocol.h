////////////////////////////////////////////////////////////////////////////////
// DSProtocol.h
// ------------------------------
// Decompiled by Deathway
// Date : 2007-05-09
// ------------------------------
#ifndef DSPROTOCOL_H
#define DSPROTOCOL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "prodef.h"
#include "CGameObject.h"

#include "ArcaBattle.h"
#include "DevilSquareFinal.h"

extern void(*ItemCreate)(CGameObject* lpObj, BYTE MapNumber, BYTE x, BYTE y, int type, BYTE level, BYTE dur, BYTE Op1, BYTE Op2, BYTE Op3, int LootIndex, BYTE NewOption, BYTE SetOption, time_t lDuration, BYTE *SocketOption, BYTE MainAttribute);


void DataServerProtocolCore(BYTE protoNum, LPBYTE aRecv, int aLen);
void DataServerLogin();
void DataServerLoginResult( SDHP_RESULT* lpMsg);
void DataServerGetCharListRequest( short aIndex);
void JGCharacterCreateRequest( SDHP_CREATECHARRESULT* lpMsg);
void JGCharacterCreateFailSend(CGameObject* lpObj, char* id);
void JGCharDelRequest( SDHP_CHARDELETERESULT* lpMsg);
void JGGetCharacterInfo( SDHP_DBCHAR_INFORESULT* lpMsg);
void GCItemListSend(CGameObject* lpObj);
void GJSetCharacterInfo(CGameObject* lpObj, BOOL bMapServerMove);
void GDGetWarehouseList(CGameObject* lpObj, char* AccountID);
void DGGetWarehouseList( SDHP_GETWAREHOUSEDB_SAVE* lpMsg);
void GDGetWarehouseNoItem( SDHP_GETWAREHOUSEDB_RESULT* lpMsg);
void GDSetWarehouseList(CGameObject* lpObj, BOOL bCloseWindow);
void DGSetWarehouseList(PMSG_ANS_WARESAVE* lpMsg);
void GDUserItemSave( CGameObject* lpObj);
void GDSetWarehouseMoney(CGameObject* lpObj);
void GDGameServerInfoSave();
//void ItemCreate(CGameObject* lpObj, BYTE MapNumber, BYTE x, BYTE y, int type, BYTE level, BYTE dur, BYTE Op1, BYTE Op2, BYTE Op3, int LootIndex, BYTE NewOption, BYTE SetOption, time_t lDuration, BYTE *SocketOption, BYTE MainAttribute);
void PetItemSerialCreateSend(CGameObject* lpObj, BYTE MapNumber, BYTE x, BYTE y, int type, BYTE level,BYTE dur, BYTE Op1, BYTE Op2, BYTE Op3, int LootIndex, BYTE NewOption, BYTE SetOption);
void ExpItemSerialCreateSend(CGameObject* lpObj, BYTE MapNumber, BYTE x, BYTE y, int type, BYTE level, BYTE dur, BYTE Op1, BYTE Op2, BYTE Op3, int LootIndex, BYTE NewOption, BYTE SetOption, time_t ExpTime);
void ItemSerialCreateRecv( SDHP_ITEMCREATERECV* lpMsg);
void ItemMovePathSave(char* ActID, char* Name, BYTE level, BYTE mapnumber, BYTE x, BYTE y, char* Item, BYTE op1, BYTE op2, BYTE op3, DWORD serial);
void DGOptionDataSend(CGameObject* lpObj, char* szName, LPBYTE KeyBuffer, BYTE GO,BYTE Qk, BYTE Wk, BYTE Ek, BYTE ChatWnd, BYTE Rk, int QWER, BYTE Transform);
void DGOptionDataRecv( SDHP_SKILLKEYDATA_SEND* lpMsg);
void DGDeleteTempUserInfo(SDHP_DELETE_TEMPUSERINFO_RESULT * aRecv);
void DGMoveOtherServer( SDHP_CHARACTER_TRANSFER_RESULT* lpMsg);
void gObjRequestPetItemInfo(CGameObject* lpObj, int inventype);
void DGRecvPetItemInfo(LPBYTE lpData);
void gObjSavePetItemInfo(CGameObject* lpObj, int inventype);
void GS_GDReqCastleTotalInfo(int iMapSvrGroup, int iCastleEventCycle);
void GS_GDReqOwnerGuildMaster(int iMapSvrGroup, CGameObject* lpObj);
void GS_GDReqCastleNpcBuy(int iMapSvrGroup, CGameObject* lpObj, int iNpcNumber, int iNpcIndex, int iNpcDfLevel, int iNpcRgLevel, int iNpcMaxHP, int iNpcHP, BYTE btNpcX, BYTE btNpcY, BYTE btNpcDIR,  int iBuyCost);
void GS_GDReqCastleNpcRepair(int iMapSvrGroup, CGameObject* lpObj, int iNpcNumber, int iNpcIndex, int iRepairCost);
void GS_GDReqCastleNpcUpgrade(int iMapSvrGroup, CGameObject* lpObj, int iNpcNumber, int iNpcIndex, int iNpcUpType, int iNpcUpValue, int iNpcUpIndex);
void GS_GDReqTaxInfo(int iMapSvrGroup, CGameObject* lpObj);
void GS_GDReqTaxRateChange(int iMapSvrGroup, CGameObject* lpObj, int iTaxType, int iTaxRate);
void GS_GDReqCastleMoneyChange(int iMapSvrGroup, CGameObject* lpObj, int iMoneyChange);
//void GS_GDReqCastleDateChange(int iMapSvrGroup, CGameObject* lpObj,WORD wStartYear, BYTE btStartMonth, BYTE btStartDay, WORD wEndYear, BYTE btEndMonth, BYTE btEndDay);
void GS_GDReqSiegeDateChange(int iMapSvrGroup, CGameObject* lpObj, WORD wStartYear, BYTE btStartMonth, BYTE btStartDay, WORD wEndYear, BYTE btEndMonth, BYTE btEndDay);
void GS_GDReqGuildMarkRegInfo(int iMapSvrGroup, CGameObject* lpObj);
void GS_GDReqSiegeEndedChange(int iMapSvrGroup, int bIsSiegeEnded);
void GS_GDReqCastleOwnerChange(int iMapSvrGroup, int bIsCastleOccupied, char* lpszGuildName);
void GS_GDReqRegAttackGuild(int iMapSvrGroup, CGameObject* lpObj);
void GS_GDReqRestartCastleState(int iMapSvrGroup);
void GS_GDReqMapSvrMsgMultiCast(int iMapSvrGroup, char* lpszMsgText);
void GS_GDReqGlobalPostMultiCast(int iMapSvrGroup, CGameObject* lpObj, char * szMessage);
void GS_GDReqRegGuildMark(int iMapSvrGroup, CGameObject* lpObj, int iItemPos);
void GS_GDReqGuildMarkReset(int iMapSvrGroup, CGameObject* lpObj, char* lpszGuildName);
void GS_GDReqGuildSetGiveUp(int iMapSvrGroup, CGameObject* lpObj, int bIsGiveUp);
void GS_GDReqNpcRemove(int iMapSvrGroup, int iNpcNumber, int iNpcIndex);
void GS_GDReqCastleStateSync(int iMapSvrGroup, int iCastleState, int iTaxRateChaos, int iTaxRateStore, int iTaxHuntZone, char* lpszOwnerGuild);
void GS_GDReqCastleTributeMoney(int iMapSvrGroup, int iCastleTributeMoney);
void GS_GDReqResetCastleTaxInfo(int iMapSvrGroup);
void GS_GDReqResetSiegeGuildInfo(int iMapSvrGroup);
void GS_GDReqResetRegSiegeInfo(int iMapSvrGroup);
void GS_GDReqCastleInitData(int iMapSvrGroup, int iCastleEventCycle);
void GS_GDReqCastleNpcInfo(int iMapSvrGroup, CGameObject* lpObj);
void GS_GDReqAllGuildMarkRegInfo(int iMapSvrGroup, CGameObject* lpObj);
void GS_GDReqCalcRegGuildList(int iMapSvrGroup);
void GS_GDReqCsLoadTotalGuildInfo(int iMapSvrGroup);
void GS_DGAnsCastleTotalInfo(LPBYTE lpRecv);
void GS_DGAnsOwnerGuildMaster(LPBYTE lpRecv);
void GS_DGAnsCastleNpcBuy(LPBYTE lpRecv);
void GS_DGAnsCastleNpcRepair(LPBYTE lpRecv);
void GS_DGAnsCastleNpcUpgrade(LPBYTE lpRecv);
void GS_DGAnsTaxInfo(LPBYTE lpRecv);
void GS_DGAnsTaxRateChange(LPBYTE lpRecv);
void GS_DGAnsCastleMoneyChange(LPBYTE lpRecv);
void GS_DGAnsSiegeDateChange(LPBYTE lpRecv);
void GS_DGAnsGuildMarkRegInfo(LPBYTE lpRecv);
void GS_DGAnsSiegeEndedChange(LPBYTE lpRecv);
void GS_DGAnsCastleOwnerChange(LPBYTE lpRecv);
void GS_DGAnsRegAttackGuild(LPBYTE lpRecv);
void GS_DGAnsRestartCastleState(LPBYTE lpRecv);
void GS_DGAnsMapSvrMsgMultiCast(LPBYTE lpRecv);
void GS_DGAnsGlobalPostMultiCast(LPBYTE lpRecv);
void GS_DGAnsRegGuildMark(LPBYTE lpRecv);
void GS_DGAnsGuildMarkReset(LPBYTE lpRecv);
void GS_DGAnsGuildSetGiveUp(LPBYTE lpRecv);
void GS_DGAnsNpcRemove(LPBYTE lpRecv);
void GS_DGAnsCastleStateSync(LPBYTE lpRecv);
void GS_DGAnsCastleTributeMoney(LPBYTE lpRecv);
void GS_DGAnsResetCastleTaxInfo(LPBYTE lpRecv);
void GS_DGAnsResetSiegeGuildInfo(LPBYTE lpRecv);
void GS_DGAnsResetRegSiegeInfo(LPBYTE lpRecv);
void GS_DGAnsCastleInitData(LPBYTE lpRecv);
void GS_DGAnsCastleNpcInfo(LPBYTE lpRecv);
void GS_DGAnsAllGuildMarkRegInfo(LPBYTE lpRecv);
void GS_DGAnsFirstCreateNPC(LPBYTE lpRecv);
void GS_DGAnsCalcREgGuildList(LPBYTE lpRecv);
void GS_DGAnsCsGulidUnionInfo(LPBYTE lpRecv);
void GS_DGAnsCsSaveTotalGuildInfo(LPBYTE lpRecv);
void GS_DGAnsCsLoadTotalGuildInfo(LPBYTE lpRecv);
void GS_DGAnsCastleNpcUpdate(LPBYTE lpRecv);
void GDReqCrywolfSync(int iMapSvrGroup, int iCrywolfState, int iOccupationState);
void DGAnsCrywolfSync(LPBYTE lpRecv);
void GDReqCrywolfInfoLoad(int iMapSvrGroup);
void DGAnsCrywolfInfoLoad(LPBYTE lpRecv);
void GDReqCrywolfInfoSave(int iMapSvrGroup, int iCrywolfState, int iOccupationState);
void DGAnsCrywolfInfoSave(LPBYTE lpRecv);
void GDReqGensJoin(CGameObject* lpObj, int GensID);
void DGAnsGensJoin(GSP_ANS_GENS_JOIN *lpMsg);
void GDReqGensInfo(CGameObject* lpObj);
void DGAnsGensInfo(GSP_ANS_GENS_INFO *lpMsg);
void GDReqGensQuit(CGameObject* lpObj);
void DGAnsGensQuit(GSP_ANS_GENS_QUIT *lpMsg);
void GDReqGensRank(CGameObject* lpObj);
void DGAnsGensRank(GSP_ANS_GENS_RANK *lpMsg);
void GDReqGensSave(CGameObject* lpObj);

void GDReqInGameShopItemList(CGameObject* lpObj, BYTE InventoryType, BYTE InventoryNum);
void DGAnsInGameShopItemList(LPBYTE lpRecv);
void GDReqInGameShopPoint(CGameObject* lpObj);
void DGAnsInGameShopPoint(ISHOP_ANS_POINT *lpMsg);
void GDReqInGameShopPointAdd(CGameObject* lpObj, BYTE Type, float Coin);
void GDReqInGameShopItemBuy(CGameObject* lpObj, int ID1, int ID2, int ID3, int Price, BYTE CoinType);
void DGAnsInGameShopItemBuy(LPBYTE lpRecv);
void GDReqInGameShopItemGift(CGameObject* lpObj, int ID1, int ID2, int ID3, int Price, BYTE CoinType, char Target[11], char Message[200]);
void DGAnsInGameShopItemGift(LPBYTE lpRecv);
void GDReqInGameShopItemUse(CGameObject* lpObj, int UniqueCode, int AuthCode);
void DGAnsInGameShopItemUse(ISHOP_ITEM_USEANS *lpMsg);
void GDReqInGameShopItemDelete(CGameObject* lpObj, int UniqueCode, int AuthCode);
void GDReqInGameShopItemRollbackUse(CGameObject* lpObj, int UniqueCode, int AuthCode);

void GDReqBanUser(CGameObject* lpObj, BYTE Type, BYTE Ban);
void GDSetExpWare(CGameObject* lpObj, BYTE ExpansionType, BYTE ExpansionLevel);
void GDCharCardBuy(char AccountID[11], BYTE Type);
void GDReqSecLock(CGameObject* lpObj, int Code);
void ReqMonsterCountAns(DS_GET_MONSTERCNT * aRecv);
void ReqMonsterCount(short aIndex);
void ReqSaveMonsterCount(short aIndex, int monsterid);
void ReqSavePlayerKiller(short kIndex, short vIndex);

void GDReqArcaBattleGuildJoin(CGameObject* lpObj);
void DGAnsArcaBattleGuildJoin(PMSG_ANS_ARCA_BATTLE_GUILD_JOIN_DS *lpMsg);

void DGAnsArcaBattleGuildJoinSelect(PMSG_ANS_ARCA_BATTLE_GUILD_JOIN_DS *lpMsg);

void GDReqArcaBattleGuildMemberJoin(CGameObject* lpObj);
void DGAnsArcaBattleGuildMemberJoin(PMSG_ANS_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS *lpMsg);

void GDReqArcaBattleEnter(CGameObject* lpObj, BYTE btEnterSeq);
void DGAnsArcaBattleEnter(PMSG_ANS_ARCA_BATTLE_ENTER_DS *lpMsg);

//void DGAnsArcaBattleWinGuildInfo(PMSG_ANS_AB_WIN_GUILD_INFO_DS *lpMsg);

void DGReqArcaBattleProcMultiCast(PMSG_ANS_AB_PROC_STATE_DS *lpMsg);

void GDAnsJoinMemberUnder(PMSG_ANS_AB_JOIN_MEMBER_UNDER_DS *lpMsg);

void DGAns_ChaosCastle_KillPoint_Result(SDHP_ANS_KILLPOINT_RESULT_CC_UBF *lpMsg);
void DGAns_CCF_Info_Notify(SDHP_ANS_CCF_INFO_NOTIFY *lpMsg);
void DGAns_GetCCFPermission(SDHP_ANS_CCF_GETPERMISSION *lpMsg);

void GCMuunInventoryItemListSend(CGameObject* lpObj);

void GDReqSaveEventInvenItem(CGameObject* lpObj);
void GDReqLoadEventInvenItem(CGameObject* lpObj);
void DGLoadEventInvenItem(_tagSDHP_ANS_DBEVENT_INVEN_LOAD *lpMsg);

void GDReqSwitchWare(CGameObject* lpObj, int WareID);
void DGAnsSwitchWare(PMSG_ANS_SWITCHWARE *lpMsg);

void DGAnsUBFAccountUserInfo(PMSG_ANS_UBF_ACCOUNT_USERINFO *lpMsg);
void DGUBFRegisterAccountUserResult(PMSG_UBF_REGISTER_ACCOUNT_USER_RESULT *lpMsg);
void DGUBFAccountUserCopyResult(PMSG_UBF_ACCOUNT_USER_COPY_RESULT *lpMsg);
void DGAnsUBFGetReward(PMSG_ANS_UBF_GET_REWARD *lpMsg);
void DGAnsSetReceivedWinnerItem(PMSG_ANS_UBF_SET_RECEIVED_REWARD *lpMsg);
void DGUBFAnsCancelToJoinResult(PMSG_UBF_ANS_CANCEL_REGISTER_USER *lpMsg);
void DGUBFAnsGetRealNameAndServerCode(PMSG_ANS_GET_UBF_REAL_NAME *lpMsg);
void GDReqCopyPetItemInfo(CGameObject* lpObj);

void GDReqDSFCanPartyEnter(CGameObject* lpObj);
void DG_DSF_CanPartyEnter(PMSG_ANS_DSF_CAN_PARTY_ENTER *lpMsg);
void DG_DSF_PartyRankRenew(PMSG_ANS_DSF_PARTYRANKRENEW *lpMsg);
void DG_DSF_GoFinalParty(LPBYTE lpRecv);
void DG_DSF_GetReward(PMSG_ANS_GET_DSF_REWARD *lpMsg);
void DGReqOtherChannelWishper(PMSG_RECV_CHATDATA_WHISPER *lpMsg);
void GDReqMapSrvGroupServerCount();
void DGWhisperResponseRecv(DSMSG_GS_WHISPER_RESULT * aRecv);
void GDWhisperResultSend(BYTE Result, int OriginPlayerIndex, int OriginGS);
void GDDisconnectOtherChannel(PMSG_RECV_DC_OTHER_CHANNEL * aRecv);
void GDReqDisconnectOtherChannel(char * charname);
void DGDisconnectOtherChannel(PMSG_RECV_DC_OTHER_CHANNEL * aRecv);
void DGMuBotSettingsSend(CGameObject* lpObj, char* szName, BYTE* lpSettings);
void DGMuBotOptionRecv(LPMUBOT_SETTINGS_SEND lpMsg);

extern int gMapSrvGroupOnlineServer;
#endif /*DSPROTOCOL_H*/

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

