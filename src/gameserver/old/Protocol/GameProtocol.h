// ------------------------------
// Decompiled by Deathway
// Date : 2007-05-09
// ------------------------------
#ifndef PROTOCOL_H
#define PROTOCOL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "prodef.h"
#include "GuildClass.h"
#include "CUserData.h"
#include "CGameObject.h"
#include "MapClass.h"
#include "GensSystemProtocol.h"
#include "Shop.h"



class GameProtocol
{
public:

	GameProtocol();
	~GameProtocol();

	void ProtocolCore(BYTE protoNum, LPBYTE aRecv, int aLen, CGameObject* lpObj, BOOL Encrypt);
	void MsgSendV2(CGameObject* lpObj, unsigned char* Msg, int size);
	void ChatSendV2(CGameObject* lpObj, unsigned char* Msg, int size);
	void CGLiveClient(PMSG_CLIENTTIME* lpClientTime, CGameObject* lpObj);
	void GCCheckMainExeKeySend(CGameObject* lpObj);
	WORD EncryptCheckSumKey(WORD wSource);
	void CGCheckMainRecv(PMSG_CHECK_MAINEXE_RESULT* lpMsg, CGameObject* lpObj);
	void PEchoProc(unsigned char* aMsg, int aLen, short aIndex);
	void GCResultSend(CGameObject* lpObj, unsigned char headcode, unsigned char result);
	void ChatSend(CGameObject* lpObj, char* szChat);
	void vChatSend(CGameObject* lpObj, char* szChat, ...);
	void AllSendServerMsg(char* chatmsg);
	void DataSendAll(unsigned char* lpMsg, int iMsgSize);
	void ChatTargetSend(CGameObject* lpObj, char* szChat, int senduser);
	void PChatProc(PMSG_CHATDATA* lpChat, CGameObject* lpObj);
	void CGChatRecv(PMSG_CHATDATA_NUMBER* lpMsg, CGameObject* lpObj);
	void GCServerMsgSend(unsigned char msg, CGameObject* lpObj);
	void GCServerMsgStringSend(char* szMsg, CGameObject* lpObj, BYTE type);
	void GCServerMsgStringSendEx(CGameObject* lpObj, BYTE type, LPSTR szMsg, ...);
	void GCServerMsgStringSendAll(LPSTR  szMsg, BYTE type);
	void GCServerMsgStringSendAllEx(BYTE type, LPSTR szMsg, ...);
	void GCServerMsgStringSendGuild(_GUILD_INFO_STRUCT* lpNode, char* szMsg, unsigned char type);
	void GCEventStateSend(CGameObject* lpObj, unsigned char state, unsigned char event);
	void GCMapEventStateSend(int map, unsigned char state, unsigned char event);
	void CGChatWhisperRecv(PMSG_CHATDATA_WHISPER* lpMsg, CGameObject* lpObj);
	void SCPJoinResultSend(CGameObject* lpObj, unsigned char result);
	void CSPJoinIdPassRequest(PMSG_IDPASS* lpMsg, CGameObject* lpObj);
	void GCJoinBillCheckSend(char* AccountId, CGameObject* lpObj);
	void GCJoinResult(unsigned char result, CGameObject* lpObj);
	void GCJoinSocketResult(unsigned char result, SOCKET Socket);
	void CGClientCloseMsg(PMSG_CLIENTCLOSE* lpMsg, CGameObject* lpObj);
	void GCCloseMsgSend(CGameObject* lpObj, unsigned char result);
	void CGClientMsg(PMSG_CLIENTMSG* lpMsg, CGameObject* lpObj);
	void CGPCharacterCreate(PMSG_CHARCREATE* lpMsg, CGameObject* lpObj);
	void CGPCharDel(PMSG_CHARDELETE* lpMsg, CGameObject* lpObj);
	void CGPCharacterMapJoinRequest(PMSG_CHARMAPJOIN* lpMsg, CGameObject* lpObj);
	void GCLevelUpMsgSend(CGameObject* lpObj, int iSendEffect);
	void CGLevelUpPointAdd(PMSG_LVPOINTADD* lpMsg, CGameObject* lpObj);
	void GCInventoryItemOneSend(CGameObject* lpObj, int pos);
	void GCPkLevelSend(CGameObject* lpObj, unsigned char pklevel);
	void GCMagicListOneSend(CGameObject* lpObj, char Pos, unsigned short type, unsigned char level, unsigned short skill, BYTE btListType);
	void GCMagicListOneDelSend(CGameObject* lpObj, char Pos, unsigned short type, BYTE level, unsigned short skill, BYTE btListType);
	void GCMagicListMultiSend(CGameObject* lpObj, BYTE btListType);
	void GCEquipmentSend(CGameObject* lpObj);
	void GCRecallMonLife(CGameObject* lpObj, int maxlife, int life);
	void GCTimeViewSend(CGameObject* lpObj, int second);
	void GCGoalSend(CGameObject* lpObj, char* Name1, unsigned char score1, char* Name2, unsigned char score2);
	void GCSkillKeyRecv(PMSG_SKILLKEY* lpMsg, CGameObject* lpObj);
	void GCSkillKeySend(CGameObject* lpObj, unsigned char* keybuffer, unsigned char GO, unsigned char Qk, unsigned char Wk, unsigned char Ek, /*<regrel ebp+0x20>*/ /*|0x1|*/ unsigned char ChatWnd, unsigned char Rk, int QWER);
	void GCMoneySend(CGameObject* lpObj, unsigned long money);
	void GCItemInventoryPutSend(CGameObject* lpObj, unsigned char result, unsigned char iteminfo1, unsigned char iteminfo2);
	void CGItemGetRequest(PMSG_ITEMGETREQUEST* lpMsg, CGameObject* lpObj, BYTE ID, BYTE ID1);
	bool CGItemDropRequest(PMSG_ITEMTHROW* lpMsg, CGameObject* lpObj, int drop_type);
	void GCItemMoveResultSend(CGameObject* lpObj, unsigned char result, unsigned char pos, unsigned char* const ItemInfo);
	void CGInventoryItemMove(PMSG_INVENTORYITEMMOVE* lpMsg, CGameObject* lpObj);
	void GCEquipmentChange(CGameObject* lpObj, int pos);
	void CGTalkRequestRecv(PMSG_TALKREQUEST* lpMsg, CGameObject* lpObj);
	void GCUserWarehouseSend(CGameObject* lpObj);
	void CGBuyRequestRecv(PMSG_BUYREQUEST* lpMsg, CGameObject* lpObj);
	void CGSellRequestRecv(PMSG_SELLREQUEST* lpMsg, CGameObject* lpObj);
	int  GetNeedMoneyItemDurRepaire(/*CGameObject* lpObj, */class CItemObject* DurItem, int RequestPos);
	void ItemDurRepaire(CGameObject* lpObj, /*CGameObject* lpObj, */class CItemObject* DurItem, int pos, int RequestPos);
	void CGModifyRequestItem(PMSG_ITEMDURREPAIR* lpMsg, CGameObject* lpObj);
	void CGTradeRequestSend(PMSG_TRADE_REQUEST* lpMsg, CGameObject* lpObj);
	void CGTradeResponseRecv(PMSG_TRADE_RESPONSE* lpMsg, CGameObject* lpObj);
	bool GCTradeResponseSend(BYTE response, CGameObject* lpObj, LPSTR id, WORD level, int GuildNumber);
	int GCTradeOtherDel(CGameObject* lpObj, BYTE tradeindex);
	int GCTradeOtherAdd(CGameObject* lpObj, BYTE tradeindex, LPBYTE iteminfo);
	void CGTradeMoneyRecv(PMSG_TRADE_GOLD* lpMsg, CGameObject* lpObj);
	bool GCTradeMoneyOther(CGameObject* lpObj, DWORD money);
	bool GCTradeOkButtonSend(CGameObject* lpObj, unsigned char flag);
	void CGTradeOkButtonRecv(PMSG_TRADE_OKBUTTON* lpMsg, CGameObject* lpObj);
	void CGTradeCancelButtonRecv(CGameObject* lpObj);
	void CGTradeResult(CGameObject* lpObj, unsigned char result);
	
	// PARTY FUNCTIONS.
	void CGPartyRequestRecv(PMSG_PARTYREQUEST* lpMsg, CGameObject* lpObj);
	void CGPartyRequestResultRecv(PMSG_PARTYREQUESTRESULT* lpMsg, CGameObject* lpObj);
	void CGPartyList(CGameObject* lpObj);
	void CGPartyListAll(int pnumber);
	void CGPartyDelUser(PMSG_PARTYDELUSER* lpMsg, CGameObject* lpObj, int nType);
	void GCPartyDelUserSend(CGameObject* lpObj);
	void GCPartyDelUserSendNoMessage(CGameObject* lpObj);

	// GUILD FUNCTIONS.
	void CGGuildRequestRecv(PMSG_GUILDJOINQ* lpMsg, CGameObject* lpObj);
	void CGGuildRequestResultRecv(PMSG_GUILDQRESULT* lpMsg, CGameObject* lpObj);
	void CGGuildListAll(int pnumber);
	void CGGuildDelUser(PMSG_GUILDDELUSER* lpMsg, CGameObject* lpObj);
	void GCGuildDelUserResult(CGameObject* lpObj, unsigned char Result);
	void GCGuildMasterQuestionSend(CGameObject* lpObj);
	void CGGuildMasterAnswerRecv(PMSG_GUILDMASTERANSWER* lpMsg, CGameObject* lpObj);
	void GCGuildMasterManagerRun(CGameObject* lpObj);
	void CGGuildMasterInfoSave(CGameObject* lpObj,PMSG_GUILDINFOSAVE* lpMsg);
	void CGGuildMasterCreateCancel(CGameObject* lpObj);
	void GCGuildViewportNowPaint(CGameObject* lpObj, char* guildname, unsigned char* Mark, int isGuildMaster);
	void GCGuildViewportDelNow(CGameObject* lpObj, int isGuildMaster);
	void GCManagerGuildWarEnd(char* GuildName);
	void GCManagerGuildWarSet(char* GuildName1, char* GuildName2, int type);
	void GCGuildWarRequestResult(char* GuildName, CGameObject* lpObj, int type);
	void GCGuildWarRequestSend(char* GuildName, CGameObject* lpObj, int type);
	void GCGuildWarRequestSendRecv(PMSG_GUILDWARSEND_RESULT* lpMsg, CGameObject* lpObj);
	void GCGuildWarDeclare(CGameObject* lpObj, char* _guildname);
	void GCGuildWarEnd(CGameObject* lpObj, unsigned char result, char* _guildname);
	void GCGuildWarScore(CGameObject* lpObj);

	void CGWarehouseMoneyInOut(CGameObject* lpObj,PMSG_WAREHOUSEMONEYINOUT* lpMsg);
	void GCWarehouseInventoryMoneySend(CGameObject* lpObj, unsigned char result, int money, int wmoney);
	void CGWarehouseUseEnd(CGameObject* lpObj);
	void GCWarehouseStateSend(CGameObject* lpObj, unsigned char state);
	void GCWarehouseRecivePassword(CGameObject* lpObj,PMSG_WAREHOUSEPASSSEND* lpMsg);
	void GCUserChaosBoxSend(CGameObject* lpObj, int iChaosBoxType);
	void CGChaosBoxItemMixButtonClick(PMSG_CHAOSMIX* aRecv, CGameObject* lpObj);
	void CGChaosBoxUseEnd(CGameObject* lpObj);
	void PMoveProc(PMSG_MOVE* lpMove, CGameObject* lpObj);
	void RecvPositionSetProc(PMSG_POSISTION_SET* lpMove, CGameObject* lpObj);
	void CGAttack(PMSG_ATTACK* lpMsg, CGameObject* lpObj);
	void GCDamageSendPoison(CGameObject* lpObj, int damage, int iShieldDamage);
	void GCDamageSend(CGameObject* lpObj, CGameObject* lpTargetObj, int AttackDamage, int MSBFlag, int MSBDamage, int iShieldDamage);
	void GCKillPlayerExpSend(CGameObject* lpObj, CGameObject* lpTargetObj, int exp, int AttackDamage, int MSBFlag);
	void GCKillPlayerMasterExpSend(CGameObject* lpObj, CGameObject* lpTargetObj, int exp, int AttackDamage, BOOL MSBFlag);
	void GCDiePlayerSend(CGameObject* lpObj, CGameObject* lpTargetObj, BYTE skill, int KillerIndex);
	void GCActionSend(CGameObject* lpObj, BYTE ActionNumber, CGameObject* lpObjTarget, int aTargetIndex);
	void CGActionRecv(PMSG_ACTION* lpMsg, CGameObject* lpObj);
	void CGMagicAttack(LPBYTE lpRecv, CGameObject* lpObj);
	void CGRageAttack(LPBYTE lpRecv, CGameObject* lpObj);
	void CGRageAttackRange(PMSG_RAGE_ATTACK_RANGE* lpMsg, CGameObject* lpObj);
	void GCMagicAttackNumberSend(CGameObject* lpObj, WORD MagicNumber, int usernumber, unsigned char skillsuccess);
	void GCMonkMagicAttack(CGameObject* lpObj, WORD MagicNumber, int usernumber,  unsigned char skillsuccess);
	void GCMagicCancelSend(CGameObject* lpObj, WORD MagicNumber);
	void GCMagicCancel(PMSG_MAGICCANCEL * lpMsg, CGameObject* lpObj);
	void GCUseEffectItem(CGameObject* lpObj, int iEffectUseOption, int iOptionType, int iEffectType, int iLeftTime);
	void GCUseEffectItem2(CGameObject* lpObj, int iEffectUseOption, int iOptionType, int iEffectType, int iLeftTime);
	void CGMagicCancel(PMSG_MAGICCANCEL* lpMsg, CGameObject* lpObj);
	void GCUseMonsterSkillSend(CGameObject* lpObj, CGameObject* lpTargetObj, int iSkillNumber);
	void GCStateInfoSend(CGameObject* lpObj, unsigned char state, int BuffViewport);
	void CGTeleportRecv(PMSG_TELEPORT* lpMsg, CGameObject* lpObj);
	void CGReqMapMove(PMSG_REQ_MAPMOVE *lpMsg, CGameObject* lpObj);
	void GCSendMapMoveCheckSum(CGameObject* lpObj);
	void CGTargetTeleportRecv(PMSG_TARGET_TELEPORT* lpMsg, CGameObject* lpObj);
	void GCTeleportSend(CGameObject* lpObj, unsigned short MoveNumber, BYTE MapNumber, BYTE MapX, BYTE MapY, BYTE Dir);
	void CGBeattackRecv(unsigned char* lpRecv, CGameObject* lpObj, int magic_send);
	void CGDurationMagicRecv(LPBYTE lpRecv, CGameObject* lpObj);
	void ObjectMapJoinPositionSend(short aIndex);
	void CGUseItemRecv(PMSG_USEITEM* lpMsg, CGameObject* lpObj);
	void GCReFillSend(CGameObject* lpObj, int Life, BYTE Ipos, unsigned char flag,  WORD wShield);
	void GCManaSend(CGameObject* lpObj, int Mana, BYTE Ipos, unsigned char flag,  WORD BP);
	void GCInventoryItemDeleteSend(CGameObject* lpObj, BYTE pos, unsigned char flag);
	void GCItemUseSpecialTimeSend(CGameObject* lpObj, unsigned char number, int time);
	void GCItemDurSend(CGameObject* lpObj, BYTE pos, unsigned char dur, unsigned char flag);
	void GCItemDurSend2(CGameObject* lpObj, BYTE pos, unsigned char dur, unsigned char flag);
	void CGWeatherSend(CGameObject* lpObj, BYTE weather);
	void GCServerCmd(CGameObject* lpObj, BYTE type, unsigned char Cmd1, unsigned char Cmd2);
	void GCReqmoveDevilSquare(PMSG_REQ_MOVEDEVILSQUARE* lpMsg, CGameObject* lpObj);
	void GCReqDevilSquareRemainTime(PMSG_REQ_DEVILSQUARE_REMAINTIME* lpMsg, CGameObject* lpObj);
	void AllSendMsg(unsigned char* Msg, int size);
	void AllSendSameMapMsg(unsigned char* Msg, int size, unsigned char mapnumber);
	void GCSendPing(CGameObject* lpObj);
	void GCPingSendRecv(PMSG_PING_RESULT* aRecv, CGameObject* lpObj);
	void GCRegEventChipRecv(PMSG_REGEVENTCHIP* lpMsg, CGameObject* lpObj);
	void GCGetMutoNumRecv(PMSG_GETMUTONUMBER* lpMsg, CGameObject* lpObj);
	void GCUseEndEventChipRescv(CGameObject* lpObj);
	void GCUseRenaChangeZenRecv(PMSG_EXCHANGE_EVENTCHIP* lpMsg, CGameObject* lpObj);
	
	// QUEST FUNCTIONS.
	void CGRequestQuestInfo(CGameObject* lpObj);
	void GCSendQuestInfo(CGameObject* lpObj, int QuestIndex);
	void CGSetQuestState(PMSG_SETQUEST* lpMsg, CGameObject* lpObj);
	void GCSendQuestPrize(CGameObject* lpObj, unsigned char Type, unsigned char Count);
	
	void CGCloseWindow(CGameObject* lpObj);
	void CGRequestEnterBloodCastle(PMSG_REQ_MOVEBLOODCASTLE* lpMsg, CGameObject* lpObj);
	void CGRequestEnterChaosCastle(PMSG_REQ_MOVECHAOSCASTLE* lpMsg, CGameObject* lpObj);
	void CGRequestRepositionUserInChaosCastle(PMSG_REQ_REPOSUSER_IN_CC* lpMsg, CGameObject* lpObj);
	void CGRequestEventEnterCount(PMSG_REQ_CL_ENTERCOUNT* lpMsg, CGameObject* lpObj);
	void CGRequestLottoRegister(PMSG_REQ_2ANV_LOTTO_EVENT* lpMsg, CGameObject* lpObj);
	void CGReqMoveOtherServer(PMSG_REQ_MOVE_OTHERSERVER* lpMsg, CGameObject* lpObj);
	void GCPacketCheckSumRecv(PMSG_PACKETCHECKSUM* aRecv, CGameObject* lpObj);
	void CGDuelStartRequestRecv(PMSG_REQ_START_DUEL* lpMsg, CGameObject* lpObj);
	void CGDuelEndRequestRecv(PMSG_REQ_END_DUEL* lpMsg, CGameObject* lpObj);
	void CGDuelOkRequestRecv(PMSG_ANS_DUEL_OK* lpMsg, CGameObject* lpObj);
	void GCSendDuelScore(CGameObject* lpObj1, CGameObject* lpObj2);
	void GCSendGetItemInfoForParty(CGameObject* lpObj, class CMapItem* lpItem);
	void GCSendEffectInfo(CGameObject* lpObj, BYTE btType);
	void CGRequestPetItemCommand(PMSG_REQUEST_PET_ITEM_COMMAND* lpMsg, CGameObject* lpObj);
	void CGRequestPetItemInfo(PMSG_REQUEST_PET_ITEMINFO* lpMsg, CGameObject* lpObj);
	
	void GCGuildViewportInfo(PMSG_REQ_GUILDVIEWPORT* aRecv, CGameObject* lpObj);
	void CGGuildAssignStatus(PMSG_GUILD_ASSIGN_STATUS_REQ* aRecv, CGameObject* lpObj);
	void CGGuildAssignType(PMSG_GUILD_ASSIGN_TYPE_REQ* aRecv, CGameObject* lpObj);
	
	void CGRelationShipReqJoinBreakOff(PMSG_RELATIONSHIP_JOIN_BREAKOFF_REQ* aRecv, CGameObject* lpObj);
	void CGRelationShipAnsJoinBreakOff(PMSG_RELATIONSHIP_JOIN_BREAKOFF_ANS* aRecv, CGameObject* lpObj);
	
	void CGUnionList(PMSG_UNIONLIST_REQ* aRecv, CGameObject* lpObj);
	void CGRelationShipReqKickOutUnionMember(PMSG_KICKOUT_UNIONMEMBER_REQ* aRecv, CGameObject* lpObj);
	
	void CGReqMapSvrAuth(PMSG_REQ_MAPSERVERAUTH* lpMsg, CGameObject *lpObj);
	void GCAnsMapSvrAuth(CGameObject *lpObj, int iResult);
	
	void CGReqCastleSiegeState(PMSG_REQ_CASTLESIEGESTATE* lpMsg, CGameObject *lpObj);
	void GCAnsCastleSiegeState(CGameObject *lpObj, int iResult, char* lpszGuildName, char* lpszGuildMaster);
	void CGReqRegCastleSiege(PMSG_REQ_REGCASTLESIEGE* lpMsg, CGameObject *lpObj);
	void GCAnsRegCastleSiege(CGameObject *lpObj, int iResult, char* lpszGuildName);
	void CGReqGiveUpCastleSiege(PMSG_REQ_GIVEUPCASTLESIEGE* lpMsg, CGameObject *lpObj);
	void GCAnsGiveUpCastleSiege(CGameObject *lpObj, int iResult, int bGiveUp, int iMarkCount, char* lpszGuildName);

	void CGReqGuildRegInfo(PMSG_REQ_GUILDREGINFO* lpMsg, CGameObject *lpObj);
	void GCAnsGuildRegInfo(CGameObject *lpObj, int iResult,CSP_ANS_GUILDREGINFO* lpMsgResult);
	void CGReqRegGuildMark(PMSG_REQ_REGGUILDMARK* lpMsg, CGameObject *lpObj);
	void GCAnsRegGuildMark(CGameObject *lpObj, int iResult,CSP_ANS_GUILDREGMARK* lpMsgResult);

	void CGReqNpcBuy(PMSG_REQ_NPCBUY* lpMsg, CGameObject *lpObj);
	void GCAnsNpcBuy(CGameObject *lpObj, int iResult, int iNpcNumber, int iNpcIndex);
	void CGReqNpcRepair(PMSG_REQ_NPCREPAIR* lpMsg, CGameObject *lpObj);
	void GCAnsNpcRepair(CGameObject *lpObj, int iResult, int iNpcNumber, int iNpcIndex, int iNpcHP, int iNpcMaxHP);
	void CGReqNpcUpgrade(PMSG_REQ_NPCUPGRADE* lpMsg, CGameObject *lpObj);
	void GCAnsNpcUpgrade(CGameObject *lpObj, int iResult, int iNpcNumber, int iNpcIndex, int iNpcUpType, int iNpcUpValue);
	void CGReqTaxMoneyInfo(PMSG_REQ_TAXMONEYINFO* lpMsg, CGameObject *lpObj);
	void GCAnsTaxMoneyInfo(CGameObject *lpObj, int iResult, BYTE btTaxRateChaos, BYTE btTaxRateStore, __int64 i64Money);
	void CGReqTaxRateChange(PMSG_REQ_TAXRATECHANGE* lpMsg, CGameObject *lpObj);
	void GCAnsTaxRateChange(CGameObject *lpObj, int iResult, BYTE btTaxType, int iTaxRate);
	void CGReqMoneyDrawOut(PMSG_REQ_MONEYDRAWOUT* lpMsg, CGameObject *lpObj);
	void GCAnsMoneyDrawOut(CGameObject *lpObj, int iResult, __int64 i64Money);
	void GCAnsCsGateState(CGameObject *lpObj, int iResult, int iGateIndex);
	void CGReqCsGateOperate(PMSG_REQ_CSGATEOPERATE* lpMsg, CGameObject *lpObj);
	void GCAnsCsGateOperate(CGameObject *lpObj, int iResult, int iGateIndex, int iGateOperate);
	void GCAnsCsGateCurState(CGameObject *lpObj, int iGateIndex, int iGateOperate);
	void GCAnsCsAccessSwitchState(CGameObject *lpObj, int iSwitchIndex, int iSwitchUserIndex, BYTE btSwitchState);
	void GCAnsCsAccessCrownState(CGameObject *lpObj, BYTE btCrownState);
	void GCAnsCsNotifyStart(CGameObject *lpObj, BYTE btStartState);
	void GCAnsCsNotifyProgress(CGameObject *lpObj, BYTE btCastleSiegeState, LPCSTR lpszGuildName);
	void GCAnsCsMapSvrTaxInfo(CGameObject *lpObj, BYTE btTaxType, BYTE btTaxRate);
	void CGReqCsMiniMapData(PMSG_REQ_MINIMAPDATA* lpMsg, CGameObject *lpObj);
	void GCAnsCsMiniMapData(CGameObject *lpObj, BYTE btResult);
	void CGReqStopCsMiniMapData(PMSG_REQ_STOPMINIMAPDATA* lpMsg, CGameObject *lpObj);
	void CGReqCsSendCommand(PMSG_REQ_CSCOMMAND* lpMsg, CGameObject *lpObj);
	void GCAnsCsSendCommand(int iCsJoinSide, BYTE btTeam, BYTE btX, BYTE btY, BYTE btCommand);
	void GCAnsCsLeftTimeAlarm(BYTE btHour, BYTE btMinute);
	void GCAnsSelfCsLeftTimeAlarm(CGameObject *lpObj, BYTE btHour, BYTE btMinute);
	void CGReqCsSetEnterHuntZone(PMSG_REQ_CSHUNTZONEENTER* lpMsg, CGameObject *lpObj);
	void GCAnsCsSetEnterHuntZone(CGameObject *lpObj, BYTE btResult, BYTE btEnterHuntZone);
	void CGReqNpcDbList(PMSG_REQ_NPCDBLIST* lpMsg, CGameObject *lpObj);
	
	void CGReqCsRegGuildList(PMSG_REQ_CSREGGUILDLIST* lpMsg, CGameObject *lpObj);
	void CGReqCsAttkGuildList(PMSG_REQ_CSATTKGUILDLIST* lpMsg, CGameObject *lpObj);

	void CGReqWeaponUse(PMSG_REQ_USEWEAPON* aRecv, CGameObject *lpObj);
	void CGReqWeaponDamageValue(PMSG_REQ_WEAPON_DAMAGE_VALUE* aRecv, CGameObject *lpObj);
	void CGReqGuildMarkOfCastleOwner(PMSG_REQ_GUILDMARK_OF_CASTLEOWNER* aRecv, CGameObject *lpObj);
	void CGReqJewelMix(PMSG_REQ_JEWEL_MIX* lpMsg, CGameObject *lpObj);
	void GCAnsJewelMix(CGameObject *lpObj, int iResult);
	void CGReqJewelUnMix(PMSG_REQ_JEWEL_UNMIX* lpMsg, CGameObject *lpObj);
	void GCAnsJewelUnMix(CGameObject *lpObj, int iResult);
	void CGReqCrywolfInfo(PMSG_REQ_CRYWOLF_INFO* lpMsg, CGameObject *lpObj);
	void GCAnsCrywolfInfo(CGameObject *lpObj, BYTE btOccupationState, BYTE btCrywolfState);
	void CGReqAlatrContract(PMSG_REQ_CRYWOLF_ALTAR_CONTRACT* lpMsg, CGameObject *lpObj);
	void CGReqPlusChaosRate(PMSG_REQ_CRYWOLF_BENEFIT_PLUS_CHAOSRATE* lpMsg, CGameObject *lpObj);
	void CGReqKanturuStateInfo(PMSG_REQ_KANTURU_STATE_INFO* lpMsg, CGameObject *lpObj);
	void GCReqEnterKanturuBossMap(PMSG_REQ_ENTER_KANTURU_BOSS_MAP* lpMsg, CGameObject *lpObj);
	void GCIllusionTempleSendReward(CGameObject* lpObj);
	void WereWolfQuarrelCheck(CGameObject* lpObj);
	void GateKeeperCheck(CGameObject* lpObj);
	void GCMasterLevelUpMsgSend(CGameObject* lpObj);
	void CGReqIllusionTempleUseSkill(PMSG_REQ_USEILLUSIONTEMPLESKILL * aRecv, CGameObject* lpObj);
	void GCSendIllusionTempleKillPoint(CGameObject* lpObj, BYTE KillPoint);
	void CGReqLuckyCoinRegister(PMSG_REQ_LUCKYCOIN_REGISTER * aRecv, CGameObject* lpObj);
	void CGReqLuckyCoinInfo(CGameObject* lpObj);
	void CGReqLuckyCoinTrade(PMSG_REQ_LUCKYCOIN_TRADE * aRecv, CGameObject* lpObj);
	void GCAnsLuckyCoinTrade(CGameObject* lpObj, BYTE Result);
	void CGReqCastleHuntZoneEntrance(PMSG_REQ_MOVE_TO_CASTLE_HUNTZONE* aRecv, CGameObject* lpObj);
	void GCSendObjectCreationState(CGameObject* lpObj);
	void GCSendObjectHP(CGameObject* lpObj, CGameObject* lpTargetObj);
	void CGReqAntiCheatRecv(CGameObject* lpObj, PMSG_SEND_AH_INFO * aRecv);
	void CGReqFileCrcRecv(CGameObject* lpObj, PMSG_REQ_CRCDATA * aRecv);
	void GCElementalDamageSend(CGameObject* lpObj, int TargetIndex, int Damage, int Elemental);
	void GCSendErtelInfo(CGameObject* lpObj);
	void CGReqEnterAcheron(CGameObject* lpObj);
	void GCObjectLifeInfo(CGameObject *lpObj, short sObjNum, int iMaxLife, int iCurLife);
	void CGAgilityRecv(CGameObject* lpObj, PMSG_AGILITYSEND * aRecv);
	void GCPriceSend(CGameObject* lpObj, BYTE type, SHOP_DATA *lpShopData);
	int OnCGInviteDuel(LPPMSG_REQ_DUEL_INVITE lpMsg, CGameObject* lpObj);
	int OnCGAnswerDuel(LPPMSG_ANS_DUEL_ANSWER lpMsg, CGameObject* lpObj);
	int OnCGLeaveDuel(LPPMSG_REQ_DUEL_EXIT lpMsg, CGameObject* lpObj);
	int OnDuelChannelJoin(LPPMSG_REQ_DUEL_JOINCNANNEL lpMsg, CGameObject* lpObj);
	int OnDuelChannelLeave(LPPMSG_REQ_DUEL_LEAVECNANNEL lpMsg, CGameObject* lpObj);
	void CGReqRegGensMember(PMSG_REQ_REG_GENS_MEMBER *lpMsg, CGameObject *lpObj);
	void CGReqSecedeGensMember(PMSG_REQ_SEGEDE_GENS_MEMBER *lpMsg, CGameObject *lpObj);
	void CGReqGensReward(PMSG_GENS_REWARD_CODE *lpMsg, CGameObject *lpObj);
	void CGReqGensMemberInfo(PMSG_REQ_GENS_INFO *lpMsg, CGameObject *lpObj);
	void CGInventoryEquipment(PMSG_REQ_INVENTORY_EQUIPMENT_ITEM *lpMsg, CGameObject* lpObj);
	void CGReqUseTransformationRing(PMSG_REQ_USETRANSFORMATIONRING *lpMsg, CGameObject *lpObj);
	void GCSendAttackSpeed(CGameObject *lpObj);
	void GCChaosMachinePriceSend(CGameObject* lpObj);
	void CGReqEnterDoppelGanger(PMSG_REQ_ENTER_DOPPELGANGER *lpMsg, CGameObject *lpObj);
	void CGReqEnterZone(PMSG_REQ_ENTER_ZONE *lpMsg, CGameObject *lpObj);	
	void CGReqInJewelPentagramItem(PMSG_REQ_IN_PENTAGRAM_JEWEL *lpMsg, CGameObject* lpObj);
	void GCAnsInJewelPentagramItem(CGameObject* lpObj, int iResult, BYTE btJewelPos, BYTE btJewelIndex, BYTE btItemType, WORD wItemIndex, BYTE btMainAttribute, BYTE btLevel, BYTE btRank1OptionNum, BYTE btRank1Level, BYTE btRank2OptionNum, BYTE btRank2Level, BYTE btRank3OptionNum, BYTE btRank3Level, BYTE btRank4OptionNum, BYTE btRank4Level, BYTE btRank5OptionNum, BYTE btRank5Level);
	void CGReqOutJewelPentagramItem(PMSG_REQ_OUT_PENTAGRAM_JEWEL *lpMsg, CGameObject* lpObj);
	void GCAnsOutJewelPentagramItem(CGameObject *lpObj, int iResult, BYTE btJewelPos, BYTE btJewelDBIndex);
	void CGReqRefinePentagramJewel(PMSG_REQ_REFINE_PENTAGRAM_JEWEL *lpMsg, CGameObject* lpObj);
	void GCAnsRefinePentagramJewel(CGameObject *lpObj, BYTE btResult);
	void CGReqUpgradePentagramJewel(PMSG_REQ_UPGRADE_PENTAGRAM_JEWEL *lpMsg, CGameObject* lpObj);
	void GCAnsUpgradePentagramJewel(CGameObject *lpObj, BYTE btResult);
	void GCAnsPentagramJewelInOut(CGameObject *lpObj, BYTE btResult);
	void CGReqArcaBattleGuildMasterJoin(PMSG_REQ_ARCA_BATTLE_JOIN *lpMsg, CGameObject *lpObj);
	void CGReqArcaBattleGuildMemberJoin(PMSG_REQ_ARCA_BATTLE_JOIN *lpMsg, CGameObject *lpObj);
	void CGReqArcaBattleEnter(PMSG_REQ_ARCA_BATTLE_ENTER *lpMsg, CGameObject *lpObj);
	void CGReqAcheronEnter(PMSG_REQ_ACHERON_ENTER *lpMsg, CGameObject *lpObj);
	void CGReqArcaBattleBootyExchange(CGameObject *lpObj);
	void CGReqSpritemapExchange(CGameObject *lpObj);
	void CGReqRegisteredMemberCnt(CGameObject *lpObj);
	void GCSendRecvCheck(CGameObject* lpObj);
	void CGReqAntihackBreach(CGameObject* lpObj, PMSG_ANTIHACK_BREACH *lpMsg);
	void CGReqAntihackCheck(CGameObject* lpObj, PMSG_ANTIHACK_CHECK *lpMsg);
	void GCMuunItemDurSend(CGameObject* lpObj, BYTE pos, BYTE dur);
	void GCMuunEquipmentChange(CGameObject* lpObj, int pos);
	void GCMuunInventoryItemDeleteSend(CGameObject* lpObj, BYTE pos, BYTE flag);
	void GCMuunInventoryItemOneSend(CGameObject* lpObj, int pos); 
	void CGMuunItemGetRequest(PMSG_MUUNITEM_GETREQUEST *lpMsg, CGameObject* lpObj);
	void CGMuunItemSellRequestRecv(PMSG_MUUN_ITEM_SELLREQUEST *lpMsg, CGameObject* lpObj);
	void CGReqGuildMatchingList(PMSG_REQ_GUILDMATCHINGLIST *lpMsg, CGameObject* lpObj);
	void CGReqGuildMatchingListSearchWord(PMSG_REQ_GUILDMATCHINGLISTSEARCHWORD *lpMsg, CGameObject* lpObj);
	void CGReqRegGuildMatchingList(PMSG_REQ_REGGUILDMATCHINGDATA *lpMsg, CGameObject* lpObj);
	void CGReqCancelGuildMatchingList(PMSG_REQ_CANCEL_GUILDMATCHINGLIST *lpMsg, CGameObject* lpObj);
	void CGReqJoinGuildMatching(PMSG_REQ_JOIN_GUILDMATCHING *lpMsg, CGameObject* lpObj);
	void CGReqCancelJoinGuildMatching(CGameObject* lpObj);
	void CGReqAllowJoinGuildMatching(PMSG_REQ_ALLOW_JOIN_GUILDMATCHING *lpMsg, CGameObject* lpObj);
	void CGReqGetAllowListJoinGuildMatching(PMSG_REQ_ALLOWLIST_GUILDMATCHING *lpMsg, CGameObject* lpObj);
	void CGReqGetWaitStateListGuildMatching(CGameObject* lpObj);

	void CGReqRegWantedPartyMember(PMSG_REQ_REG_WANTED_PARTYMEMBER *lpMsg, CGameObject* lpObj);
	void CGReqGetPartyMatchingList(PMSG_REQ_GET_PARTYMATCHINGLIST *lpMsg, CGameObject* lpObj);
	void CGReqJoinMemberPartyMatching(PMSG_REQ_MEMBERJOIN_PARTYMATCHINGLIST *lpMsg, CGameObject* lpObj);
	void CGReqMemberJoinStateList(PMSG_REQ_MEMBERJOIN_STATELIST_PARTYMATCHING *lpMsg, CGameObject* lpObj);
	void CGReqMemberJoinStateListLeader(PMSG_REQ_WAITLIST_PARTYMATCHING *lpMsg, CGameObject* lpObj);
	void CGReqAcceptJoinMemberPartyMatching(PMSG_REQ_ACCEPTMEMBER_PARTYMATCHING *lpMsg, CGameObject* lpObj);
	void CGReqCancelPartyMatching(PMSG_REQ_CANCEL_JOIN_PARTYMATCHING *lpMsg, CGameObject* lpObj);

	void GCDisplayBuffeffectPartyMember(CGameObject* lpObj);
	void GCPartyMemberPosition(CGameObject* lpObj);
	void CGReqSendMemberPosInfoStart(CGameObject* lpObj);
	void CGReqSendMemberPosInfoStop(CGameObject* lpObj);
	void GCReqSendNpcPosInfo(PMSG_REQ_NPC_POSITION *lpMsg, CGameObject* lpObj);

	void CGReqCCF_DayTime(CGameObject *lpObj);
	void CGReqCCF_EnterCheck(CGameObject *lpObj);
	void CG_CCF_UI_OnOff(PMSG_CCF_UI_ONOFF *lpMsg, CGameObject* lpObj);
	int	 CGReqEnterChaosCastleFinal(CGameObject *lpObj);
	void CGRequestRepositionUserInCCF(PMSG_REQ_REPOSUSER_IN_CCF *lpMsg, CGameObject* lpObj);
	void CGReq_CCF_Ranking(PMSG_REQ_CCF_RANKING *lpMsg, CGameObject* lpObj);

	/*void CGReqEventInvenOpen(LPBYTE lpRecv, CGameObject* lpObj);
	void CGEventItemGetRequest(PMSG_EVENTITEM_GETREQUEST *lpMsg, CGameObject* lpObj);
	bool CGEventItemDropRequest(PMSG_EVENTITEM_THROW *lpMsg, CGameObject* lpObj);
	void GCEventInventoryItemListSend(CGameObject* lpObj);
	void GCEventItemDurSend(CGameObject* lpObj, BYTE pos, BYTE dur);
	void GCEventInvenItemOneSend(CGameObject* lpObj, int pos);
	void GCEventInventoryItemDeleteSend(CGameObject* lpObj, BYTE pos, unsigned char flag);

	void GCResSendExpEventInfo(CGameObject* lpObj);*/

	void CGReq_ITL_Relics(PMSG_REQ_ITL_RELATE_RELICS *lpMsg, CGameObject *lpObj);
	void CG_Req_Enter_ITR(PMSG_REQ_ENTER_ITR *lpMsg, CGameObject *lpObj);
	void CG_ReqAcceptEnterITR(PMSG_REQ_ACCEPTENTER_ITR *lpMsg, CGameObject *lpObj);
	BYTE EnterITRCheckUser(CGameObject *lpObj, int &TroubleUserIndex);

	void CGReqUBFMyCharacterInfo(PMSG_REQ_UBF_INFO *lpMsg, CGameObject *lpObj);
	void CGReqUBFJoin(PMSG_REQ_UBF_JOIN *lpMsg, CGameObject *lpObj);
	void CGReqUBFCancel(PMSG_REQ_UBF_CANCEL *lpMsg, CGameObject *lpObj);
	void CGReqUBFGetRealName(PMSG_REQ_UBF_REAL_NAME *lpMsg, CGameObject *lpObj);
	void CGReqUBFGetReward(PMSG_REQ_GET_REWARD *lpMsg, CGameObject *lpObj);

	void CGReqDSFSchedule(CGameObject *lpObj);
	void CGReqDSFEnter(CGameObject *lpObj);
	int EnterDSFCheckUser(CGameObject *lpObj, int & TroubleUserIndex);
	void CGReqAcceptEnterDSF(PMSG_REQ_DSF_ACCEPTENTER *lpMsg, CGameObject *lpObj);
	void CGReqDSFGoFinalParty(PMSG_REQ_DSF_GO_FINAL_PARTY_INFO *lpMsg, CGameObject *lpObj);
	void CGReqDSFGetReward(PMSG_REQ_DSF_GET_REWARD *lpMsg, CGameObject *lpObj);

	void CGReqDoUsePopupType(CGameObject* lpObj);
	void GCSendEventBanner(CGameObject* lpObj, int iBannerType);

	void GCSendRareItemPriceInfo(CGameObject* lpObj);
	void GCSendDisableReconnect(CGameObject* lpObj);
	bool PacketCheckTime(CGameObject* lpObj);
	void GCSendDisableReconnectSystem(CGameObject* lpObj);
	void GCAlterPShopVault(CGameObject* lpObj, BYTE type);
	void GCSetCharColors(CGameObject* lpObj);

	void GCSendRuud(CGameObject* lpObj, int iRuud, int iObtainedRuud, bool bIsObtainedRuud);
	void CGReqUseBoxInInventory(CGameObject* lpObj, PMSG_REQ_USE_BOX * aRecv);

	void CGEventEntryNotice(int EventType, BYTE state); // event entry notify
	void CGReqNixieBossEnter(PMSG_REQ_NIXIELAKE_ENTER *lpMsg, CGameObject *lpObj);
	void CGReqHuntingLog(CGameObject *lpObj);

	void GCPlayerStatsPanelRates(CGameObject* lpObj);
	void GCPlayerStatsPanelNew(CGameObject* lpObj);

	void CGRequestStartMuBot(PMSG_MUBOT_REQ_START* lpMsg, CGameObject* lpObj);
	void CGSaveMuBotSettings(PMSG_MUBOT_SETTINGS_RECV* lpMsg, CGameObject* lpObj);
	void GCSendMuBotSettings(CGameObject* lpObj, BYTE* lpData);

	void CGRecvHitHackValues(CGameObject* lpObj, PMSG_SEND_HITHACK_INFO * lpMsg);
	bool First = true;
private:
	bool DataEncryptCheck(CGameObject* lpObj, BYTE protoNum, BOOL Encrypt);
};

extern GameProtocol GSProtocol;


#endif
