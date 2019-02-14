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

#include "prodef.h"
#include "CUserData.h"
#include "ArcaBattle.h"
#include "DevilSquareFinal.h"

/* * * * * * * * * * * * * * * * * * * * * 
 *	Mu Char List Count Packet
 *	Direction : DataServer -> GameServer
 *  Code : 0xC2
 *	HeadCode : 0x01
 *	BASE PACKET - Complete with SDHP_CHARLIST
 */

void DataServerProtocolCore(BYTE protoNum, LPBYTE aRecv, int aLen);
void DataServerLogin();
void DataServerLoginResult( SDHP_RESULT* lpMsg);
void JGPGetCharList(LPBYTE lpRecv);
void DataServerGetCharListRequest( short aIndex);
void JGCharacterCreateRequest( SDHP_CREATECHARRESULT* lpMsg);
void JGCharacterCreateFailSend(CGameObject &Obj, char* id);
void JGCharDelRequest( SDHP_CHARDELETERESULT* lpMsg);
void JGGetCharacterInfo( SDHP_DBCHAR_INFORESULT* lpMsg);
void GCItemObjectListSend(CGameObject &Obj);
void GJSetCharacterInfo(CGameObject &Obj, CGameObject &ObjTarget, BOOL bMapServerMove);
void GDGetWarehouseList(CGameObject &Obj, char* AccountID);
void DGGetWarehouseList( SDHP_GETWAREHOUSEDB_SAVE* lpMsg);
void GDGetWarehouseNoItem( SDHP_GETWAREHOUSEDB_RESULT* lpMsg);
void GDSetWarehouseList(CGameObject &Obj, BOOL bCloseWindow);
void DGSetWarehouseList(PMSG_ANS_WARESAVE* lpMsg);
void GDUserItemSave(CGameObject &Obj);
void GDSetWarehouseMoney(CGameObject &Obj);
void GDGameServerInfoSave();
void ItemCreate(CGameObject &Obj, BYTE MapNumber, BYTE x, BYTE y, int type, BYTE level, BYTE dur, BYTE Op1, BYTE Op2, BYTE Op3, int LootIndex, BYTE NewOption, BYTE SetOption, time_t lDuration, BYTE *SocketOption, BYTE MainAttribute);
void PetItemSerialCreateSend(CGameObject &Obj, BYTE MapNumber, BYTE x, BYTE y, int type, BYTE level,BYTE dur, BYTE Op1, BYTE Op2, BYTE Op3, int LootIndex, BYTE NewOption, BYTE SetOption);
void ExpItemSerialCreateSend(CGameObject &Obj, BYTE MapNumber, BYTE x, BYTE y, int type, BYTE level, BYTE dur, BYTE Op1, BYTE Op2, BYTE Op3, int LootIndex, BYTE NewOption, BYTE SetOption, time_t ExpTime);
void ItemSerialCreateRecv( SDHP_ITEMCREATERECV* lpMsg);
void ItemMovePathSave(char* ActID, char* Name, BYTE level, BYTE mapnumber, BYTE x, BYTE y, char* Item, BYTE op1, BYTE op2, BYTE op3, DWORD serial);
void DGOptionDataSend(CGameObject &Obj, char* szName, LPBYTE KeyBuffer, BYTE GO,BYTE Qk, BYTE Wk, BYTE Ek, BYTE ChatWnd, BYTE Rk, int QWER, BYTE Transform);
void DGOptionDataRecv( SDHP_SKILLKEYDATA_SEND* lpMsg);
void DGDeleteTempUserInfo(SDHP_DELETE_TEMPUSERINFO_RESULT * aRecv);
void DGMoveOtherServer( SDHP_CHARACTER_TRANSFER_RESULT* lpMsg);
void gObjRequestPetItemInfo(CGameObject &Obj, int inventype);
void DGRecvPetItemInfo(LPBYTE lpData);
void gObjSavePetItemInfo(CGameObject &Obj, int inventype);
void GS_GDReqCastleTotalInfo(int iMapSvrGroup, int iCastleEventCycle);
void GS_GDReqOwnerGuildMaster(int iMapSvrGroup, CGameObject &Obj);
void GS_GDReqCastleNpcBuy(int iMapSvrGroup, CGameObject &Obj, int iNpcNumber, int iNpcIndex, int iNpcDfLevel, int iNpcRgLevel, int iNpcMaxHP, int iNpcHP, BYTE btNpcX, BYTE btNpcY, BYTE btNpcDIR,  int iBuyCost);
void GS_GDReqCastleNpcRepair(int iMapSvrGroup, CGameObject &Obj, int iNpcNumber, int iNpcIndex, int iRepairCost);
void GS_GDReqCastleNpcUpgrade(int iMapSvrGroup, CGameObject &Obj, int iNpcNumber, int iNpcIndex, int iNpcUpType, int iNpcUpValue, int iNpcUpIndex);
void GS_GDReqTaxInfo(int iMapSvrGroup, CGameObject &Obj);
void GS_GDReqTaxRateChange(int iMapSvrGroup, CGameObject &Obj, int iTaxType, int iTaxRate);
void GS_GDReqCastleMoneyChange(int iMapSvrGroup, CGameObject &Obj, int iMoneyChange);
//void GS_GDReqCastleDateChange(int iMapSvrGroup, CGameObject &Obj,WORD wStartYear, BYTE btStartMonth, BYTE btStartDay, WORD wEndYear, BYTE btEndMonth, BYTE btEndDay);
void GS_GDReqSiegeDateChange(int iMapSvrGroup, CGameObject &Obj, WORD wStartYear, BYTE btStartMonth, BYTE btStartDay, WORD wEndYear, BYTE btEndMonth, BYTE btEndDay);
void GS_GDReqGuildMarkRegInfo(int iMapSvrGroup, CGameObject &Obj);
void GS_GDReqSiegeEndedChange(int iMapSvrGroup, int bIsSiegeEnded);
void GS_GDReqCastleOwnerChange(int iMapSvrGroup, int bIsCastleOccupied, char* lpszGuildName);
void GS_GDReqRegAttackGuild(int iMapSvrGroup, CGameObject &Obj);
void GS_GDReqRestartCastleState(int iMapSvrGroup);
void GS_GDReqMapSvrMsgMultiCast(int iMapSvrGroup, char* lpszMsgText);
void GS_GDReqGlobalPostMultiCast(int iMapSvrGroup, CGameObject &Obj, char * szMessage);
void GS_GDReqRegGuildMark(int iMapSvrGroup, CGameObject &Obj, int iItemPos);
void GS_GDReqGuildMarkReset(int iMapSvrGroup, CGameObject &Obj, char* lpszGuildName);
void GS_GDReqGuildSetGiveUp(int iMapSvrGroup, CGameObject &Obj, int bIsGiveUp);
void GS_GDReqNpcRemove(int iMapSvrGroup, int iNpcNumber, int iNpcIndex);
void GS_GDReqCastleStateSync(int iMapSvrGroup, int iCastleState, int iTaxRateChaos, int iTaxRateStore, int iTaxHuntZone, char* lpszOwnerGuild);
void GS_GDReqCastleTributeMoney(int iMapSvrGroup, int iCastleTributeMoney);
void GS_GDReqResetCastleTaxInfo(int iMapSvrGroup);
void GS_GDReqResetSiegeGuildInfo(int iMapSvrGroup);
void GS_GDReqResetRegSiegeInfo(int iMapSvrGroup);
void GS_GDReqCastleInitData(int iMapSvrGroup, int iCastleEventCycle);
void GS_GDReqCastleNpcInfo(int iMapSvrGroup, CGameObject &Obj);
void GS_GDReqAllGuildMarkRegInfo(int iMapSvrGroup, CGameObject &Obj);
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
void GDReqGensJoin(CGameObject &Obj, int GensID);
void DGAnsGensJoin(GSP_ANS_GENS_JOIN *lpMsg);
void GDReqGensInfo(CGameObject &Obj);
void DGAnsGensInfo(GSP_ANS_GENS_INFO *lpMsg);
void GDReqGensQuit(CGameObject &Obj);
void DGAnsGensQuit(GSP_ANS_GENS_QUIT *lpMsg);
void GDReqGensRank(CGameObject &Obj);
void DGAnsGensRank(GSP_ANS_GENS_RANK *lpMsg);
void GDReqGensSave(CGameObject &Obj);

void GDReqInGameShopItemList(CGameObject &Obj, BYTE InventoryType, BYTE InventoryNum);
void DGAnsInGameShopItemList(LPBYTE lpRecv);
void GDReqInGameShopPoint(CGameObject &Obj);
void DGAnsInGameShopPoint(ISHOP_ANS_POINT *lpMsg);
void GDReqInGameShopPointAdd(CGameObject &Obj, BYTE Type, float Coin);
void GDReqInGameShopItemBuy(CGameObject &Obj, int ID1, int ID2, int ID3, int Price, BYTE CoinType);
void DGAnsInGameShopItemBuy(LPBYTE lpRecv);
void GDReqInGameShopItemGift(CGameObject &Obj, int ID1, int ID2, int ID3, int Price, BYTE CoinType, char Target[11], char Message[200]);
void DGAnsInGameShopItemGift(LPBYTE lpRecv);
void GDReqInGameShopItemUse(CGameObject &Obj, int UniqueCode, int AuthCode);
void DGAnsInGameShopItemUse(ISHOP_ITEM_USEANS *lpMsg);
void GDReqInGameShopItemDelete(CGameObject &Obj, int UniqueCode, int AuthCode);
void GDReqInGameShopItemRollbackUse(CGameObject &Obj, int UniqueCode, int AuthCode);

void GDReqBanUser(CGameObject &Obj, BYTE Type, BYTE Ban);
void GDSetExpWare(CGameObject &Obj, BYTE ExpansionType, BYTE ExpansionLevel);
void GDCharCardBuy(char AccountID[11], BYTE Type);
void GDReqSecLock(CGameObject &Obj, int Code);
void ReqMonsterCountAns(DS_GET_MONSTERCNT * aRecv);
void ReqMonsterCount(short aIndex);
void ReqSaveMonsterCount(short aIndex, int monsterid);
void ReqSavePlayerKiller(short kIndex, short vIndex);

void GDReqArcaBattleGuildJoin(CGameObject &Obj);
void DGAnsArcaBattleGuildJoin(PMSG_ANS_ARCA_BATTLE_GUILD_JOIN_DS *lpMsg);

void DGAnsArcaBattleGuildJoinSelect(PMSG_ANS_ARCA_BATTLE_GUILD_JOIN_DS *lpMsg);

void GDReqArcaBattleGuildMemberJoin(CGameObject &Obj);
void DGAnsArcaBattleGuildMemberJoin(PMSG_ANS_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS *lpMsg);

void GDReqArcaBattleEnter(CGameObject &Obj, BYTE btEnterSeq);
void DGAnsArcaBattleEnter(PMSG_ANS_ARCA_BATTLE_ENTER_DS *lpMsg);

void DGAnsArcaBattleWinGuildInfo(PMSG_ANS_AB_WIN_GUILD_INFO_DS *lpMsg);

void DGReqArcaBattleProcMultiCast(PMSG_ANS_AB_PROC_STATE_DS *lpMsg);

void GDAnsJoinMemberUnder(PMSG_ANS_AB_JOIN_MEMBER_UNDER_DS *lpMsg);

void DGAns_ChaosCastle_KillPoint_Result(SDHP_ANS_KILLPOINT_RESULT_CC_UBF *lpMsg);
void DGAns_CCF_Info_Notify(SDHP_ANS_CCF_INFO_NOTIFY *lpMsg);
void DGAns_GetCCFPermission(SDHP_ANS_CCF_GETPERMISSION *lpMsg);

void GCMuunInventoryItemListSend(CGameObject &Obj);

void GDReqSaveEventInvenItem(CGameObject &Obj);
void GDReqLoadEventInvenItem(CGameObject &Obj);
void DGLoadEventInvenItem(SDHP_ANS_DBEVENT_INVEN_LOAD *lpMsg);

void GDReqSwitchWare(CGameObject &Obj, int WareID);
void DGAnsSwitchWare(PMSG_ANS_SWITCHWARE *lpMsg);

void DGAnsUBFAccountUserInfo(PMSG_ANS_UBF_ACCOUNT_USERINFO *lpMsg);
void DGUBFRegisterAccountUserResult(PMSG_UBF_REGISTER_ACCOUNT_USER_RESULT *lpMsg);
void DGUBFAccountUserCopyResult(PMSG_UBF_ACCOUNT_USER_COPY_RESULT *lpMsg);
void DGAnsUBFGetReward(PMSG_ANS_UBF_GET_REWARD *lpMsg);
void DGAnsSetReceivedWinnerItem(PMSG_ANS_UBF_SET_RECEIVED_REWARD *lpMsg);
void DGUBFAnsCancelToJoinResult(PMSG_UBF_ANS_CANCEL_REGISTER_USER *lpMsg);
void DGUBFAnsGetRealNameAndServerCode(PMSG_ANS_GET_UBF_REAL_NAME *lpMsg);
void GDReqCopyPetItemInfo(CGameObject &Obj);

void GDReqDSFCanPartyEnter(CGameObject &Obj);
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
void DGMuBotSettingsSend(CGameObject &Obj, char* szName, BYTE* lpSettings);
void DGMuBotOptionRecv(LPMUBOT_SETTINGS_SEND lpMsg);

extern int gMapSrvGroupOnlineServer;
#endif /*DSPROTOCOL_H*/

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

