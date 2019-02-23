#ifndef PROTOCOL_H
#define PROTOCOL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "prodef.h"
#include "generalStructs.h"
//#include "GuildClass.h"
//#include "user.h"
//#include "MapClass.h"
#include "GensSystemProtocol.h"
//#include "Shop.h"
#include "Query.h"


class GameProtocol
{
public:

	GameProtocol();
	~GameProtocol();

	void ProtocolCore(BYTE protoNum, LPBYTE aRecv, int aLen, int aIndex, BOOL Encrypt);
	/*void MsgSendV2(LPOBJ lpObj, unsigned char* Msg, int size);
	void ChatSendV2(LPOBJ lpObj, unsigned char* Msg, int size);*/
	void CGLiveClient(PMSG_CLIENTTIME* lpClientTime, short aIndex);
	/*void GCCheckMainExeKeySend(int aIndex);
	WORD EncryptCheckSumKey(WORD wSource);
	void CGCheckMainRecv(PMSG_CHECK_MAINEXE_RESULT* lpMsg, int aIndex);
	void PEchoProc(unsigned char* aMsg, int aLen, short aIndex);
	void GCResultSend(int aIndex, unsigned char headcode, unsigned char result);
	void ChatSend(LPOBJ lpObj, char* szChat);
	void vChatSend(LPOBJ lpObj, char* szChat, ...);
	void AllSendServerMsg(char* chatmsg);
	void DataSendAll(unsigned char* lpMsg, int iMsgSize);
	void ChatTargetSend(LPOBJ lpObj, char* szChat, int senduser);
	void PChatProc(PMSG_CHATDATA* lpChat, short aIndex);
	void CGChatRecv(PMSG_CHATDATA_NUMBER* lpMsg, int aIndex);
	void GCServerMsgSend(unsigned char msg, int aIndex);
	void GCServerMsgStringSend(char* szMsg, int aIndex, BYTE type);
	void GCServerMsgStringSendEx(int aIndex, BYTE type, LPSTR szMsg, ...);
	void GCServerMsgStringSendAll(LPSTR  szMsg, BYTE type);
	void GCServerMsgStringSendAllEx(BYTE type, LPSTR szMsg, ...);
	void GCServerMsgStringSendGuild(_GUILD_INFO_STRUCT* lpNode, char* szMsg, unsigned char type);
	void GCEventStateSend(int aIndex, unsigned char state, unsigned char event);
	void GCMapEventStateSend(int map, unsigned char state, unsigned char event);
	void CGChatWhisperRecv(PMSG_CHATDATA_WHISPER* lpMsg, int aIndex);*/
	void SCPJoinResultSend(int aIndex, BYTE result);
	void CSPJoinIdPassRequest(PMSG_IDPASS* lpMsg, int aIndex);
	//void GCJoinBillCheckSend(char* AccountId, int aIndex);
	void GCJoinResult(BYTE result, int aIndex);
	//void GCJoinSocketResult(unsigned char result, SOCKET Socket);
	void CGClientCloseMsg(PMSG_CLIENTCLOSE* lpMsg, int aIndex);
	void GCCloseMsgSend(int aIndex, unsigned char result);
	void CGClientMsg(PMSG_CLIENTMSG* lpMsg, int aIndex);
	/*void CGPCharacterCreate(PMSG_CHARCREATE* lpMsg, int aIndex);
	void CGPCharDel(PMSG_CHARDELETE* lpMsg, int aIndex);
	void CGPCharacterMapJoinRequest(PMSG_CHARMAPJOIN* lpMsg, int aIndex);
	void GCLevelUpMsgSend(int aIndex, int iSendEffect);
	void CGLevelUpPointAdd(PMSG_LVPOINTADD* lpMsg, int aIndex);
	void GCInventoryItemOneSend(int aIndex, int pos);
	void GCPkLevelSend(int aIndex, unsigned char pklevel);
	void GCMagicListOneSend(int aIndex, char Pos, unsigned short type, unsigned char level, unsigned short skill, BYTE btListType);
	void GCMagicListOneDelSend(int aIndex, char Pos, unsigned short type, BYTE level, unsigned short skill, BYTE btListType);
	void GCMagicListMultiSend(LPOBJ lpObj, BYTE btListType);
	void GCEquipmentSend(int aIndex);
	void GCRecallMonLife(int aIndex, int maxlife, int life);
	void GCTimeViewSend(int aIndex, int second);
	void GCGoalSend(int aIndex, char* Name1, unsigned char score1, char* Name2, unsigned char score2);
	void GCSkillKeyRecv(PMSG_SKILLKEY* lpMsg, int aIndex);
	void GCSkillKeySend(int aIndex, unsigned char* keybuffer, unsigned char GO, unsigned char Qk, unsigned char Wk, unsigned char Ek, unsigned char ChatWnd, unsigned char Rk, int QWER);
	void GCMoneySend(int aIndex, unsigned long money);
	void GCItemInventoryPutSend(int aIndex, unsigned char result, unsigned char iteminfo1, unsigned char iteminfo2);
	void CGItemGetRequest(PMSG_ITEMGETREQUEST* lpMsg, int aIndex, BYTE ID, BYTE ID1);
	bool CGItemDropRequest(PMSG_ITEMTHROW* lpMsg, int aIndex, int drop_type);
	void GCItemMoveResultSend(int aIndex, unsigned char result, unsigned char pos, unsigned char* const ItemInfo);
	void CGInventoryItemMove(PMSG_INVENTORYITEMMOVE* lpMsg, int aIndex);
	void GCEquipmentChange(int aIndex, int pos);
	void CGTalkRequestRecv(PMSG_TALKREQUEST* lpMsg, int aIndex);
	void GCUserWarehouseSend(LPOBJ lpObj);
	void CGBuyRequestRecv(PMSG_BUYREQUEST* lpMsg, int aIndex);
	void CGSellRequestRecv(PMSG_SELLREQUEST* lpMsg, int aIndex);
	int  GetNeedMoneyItemDurRepaire(class CItem* DurItem, int RequestPos);
	void ItemDurRepaire(LPOBJ lpObj, class CItem* DurItem, int pos, int RequestPos);
	void CGModifyRequestItem(PMSG_ITEMDURREPAIR* lpMsg, int aIndex);
	void CGTradeRequestSend(PMSG_TRADE_REQUEST* lpMsg, int aIndex);
	void CGTradeResponseRecv(PMSG_TRADE_RESPONSE* lpMsg, int aIndex);
	bool GCTradeResponseSend(BYTE response, int aIndex, LPSTR id, WORD level, int GuildNumber);
	int GCTradeOtherDel(int aIndex, BYTE tradeindex);
	int GCTradeOtherAdd(int aIndex, BYTE tradeindex, LPBYTE iteminfo);
	void CGTradeMoneyRecv(PMSG_TRADE_GOLD* lpMsg, int aIndex);
	bool GCTradeMoneyOther(int aIndex, DWORD money);
	bool GCTradeOkButtonSend(int aIndex, unsigned char flag);
	void CGTradeOkButtonRecv(PMSG_TRADE_OKBUTTON* lpMsg, int aIndex);
	void CGTradeCancelButtonRecv(int aIndex);
	void CGTradeResult(int aIndex, unsigned char result);
	void CGPartyRequestRecv(PMSG_PARTYREQUEST* lpMsg, int aIndex);
	void CGPartyRequestResultRecv(PMSG_PARTYREQUESTRESULT* lpMsg, int aIndex);
	void CGPartyList(int aIndex);
	void CGPartyListAll(int pnumber);
	void CGPartyDelUser(PMSG_PARTYDELUSER* lpMsg, int aIndex, int nType);
	void GCPartyDelUserSend(int aIndex);
	void GCPartyDelUserSendNoMessage(int aIndex);
	void CGGuildRequestRecv(PMSG_GUILDJOINQ* lpMsg, int aIndex);
	void CGGuildRequestResultRecv(PMSG_GUILDQRESULT* lpMsg, int aIndex);
	void CGGuildListAll(int pnumber);
	void CGGuildDelUser(PMSG_GUILDDELUSER* lpMsg, int aIndex);
	void GCGuildDelUserResult(int aIndex, unsigned char Result);
	void GCGuildMasterQuestionSend(int aIndex);
	void CGGuildMasterAnswerRecv(PMSG_GUILDMASTERANSWER* lpMsg, int aIndex);
	void GCGuildMasterManagerRun(int aIndex);
	void CGGuildMasterInfoSave(int aIndex, PMSG_GUILDINFOSAVE* lpMsg);
	void CGGuildMasterCreateCancel(int aIndex);
	void GCGuildViewportNowPaint(int aIndex, char* guildname, unsigned char* Mark, int isGuildMaster);
	void GCGuildViewportDelNow(int aIndex, int isGuildMaster);
	void GCManagerGuildWarEnd(char* GuildName);
	void GCManagerGuildWarSet(char* GuildName1, char* GuildName2, int type);
	void GCGuildWarRequestResult(char* GuildName, int aIndex, int type);
	void GCGuildWarRequestSend(char* GuildName, int aIndex, int type);
	void GCGuildWarRequestSendRecv(PMSG_GUILDWARSEND_RESULT* lpMsg, int aIndex);
	void GCGuildWarDeclare(int aIndex, char* _guildname);
	void GCGuildWarEnd(int aIndex, unsigned char result, char* _guildname);
	void GCGuildWarScore(int aIndex);
	void CGWarehouseMoneyInOut(int aIndex, PMSG_WAREHOUSEMONEYINOUT* lpMsg);
	void GCWarehouseInventoryMoneySend(int aIndex, unsigned char result, int money, int wmoney);
	void CGWarehouseUseEnd(int aIndex);
	void GCWarehouseStateSend(int aIndex, unsigned char state);
	void GCWarehouseRecivePassword(int aIndex, PMSG_WAREHOUSEPASSSEND* lpMsg);
	void GCUserChaosBoxSend(LPOBJ lpObj, int iChaosBoxType);
	void CGChaosBoxItemMixButtonClick(PMSG_CHAOSMIX* aRecv, int aIndex);
	void CGChaosBoxUseEnd(int aIndex);
	void PMoveProc(PMSG_MOVE* lpMove, int aIndex);
	void RecvPositionSetProc(PMSG_POSISTION_SET* lpMove, int aIndex);
	void CGAttack(PMSG_ATTACK* lpMsg, int aIndex);
	void GCDamageSendPoison(int aIndex, int damage, int iShieldDamage);
	void GCDamageSend(int aIndex, int TargetIndex, int AttackDamage, int MSBFlag, int MSBDamage, int iShieldDamage);
	void GCKillPlayerExpSend(int aIndex, int TargetIndex, int exp, int AttackDamage, int MSBFlag);
	void GCKillPlayerMasterExpSend(int aIndex, int TargetIndex, int exp, int AttackDamage, BOOL MSBFlag);
	void GCDiePlayerSend(LPOBJ lpObj, int TargetIndex, BYTE skill, int KillerIndex);
	void GCActionSend(LPOBJ lpObj, BYTE ActionNumber, int aIndex, int aTargetIndex);
	void CGActionRecv(PMSG_ACTION* lpMsg, int aIndex);
	void CGMagicAttack(LPBYTE lpRecv, int aIndex);
	void CGRageAttack(LPBYTE lpRecv, int aIndex);
	void CGRageAttackRange(PMSG_RAGE_ATTACK_RANGE* lpMsg, int aIndex);
	void GCMagicAttackNumberSend(LPOBJ lpObj, WORD MagicNumber, int usernumber, unsigned char skillsuccess);
	void GCMonkMagicAttack(LPOBJ lpObj, WORD MagicNumber, int usernumber, unsigned char skillsuccess);
	void GCMagicCancelSend(LPOBJ lpObj, WORD MagicNumber);
	void GCMagicCancel(PMSG_MAGICCANCEL * lpMsg, int aIndex);
	void GCUseEffectItem(LPOBJ lpObj, int iEffectUseOption, int iOptionType, int iEffectType, int iLeftTime);
	void GCUseEffectItem2(LPOBJ lpObj, int iEffectUseOption, int iOptionType, int iEffectType, int iLeftTime);
	void CGMagicCancel(PMSG_MAGICCANCEL* lpMsg, int aIndex);
	void GCUseMonsterSkillSend(LPOBJ lpObj, LPOBJ lpTargetObj, int iSkillNumber);
	void GCStateInfoSend(LPOBJ lpObj, unsigned char state, int BuffViewport);
	void CGTeleportRecv(PMSG_TELEPORT* lpMsg, int aIndex);
	void CGReqMapMove(PMSG_REQ_MAPMOVE *lpMsg, int iIndex);
	void GCSendMapMoveCheckSum(LPOBJ lpObj);
	void CGTargetTeleportRecv(PMSG_TARGET_TELEPORT* lpMsg, int aIndex);
	void GCTeleportSend(LPOBJ lpObj, unsigned short MoveNumber, BYTE MapNumber, BYTE MapX, BYTE MapY, BYTE Dir);
	void CGBeattackRecv(unsigned char* lpRecv, int aIndex, int magic_send);
	void CGDurationMagicRecv(LPBYTE lpRecv, int aIndex);
	void ObjectMapJoinPositionSend(short aIndex);
	void CGUseItemRecv(PMSG_USEITEM* lpMsg, int aIndex);
	void GCReFillSend(int aIndex, int Life, BYTE Ipos, unsigned char flag, WORD wShield);
	void GCManaSend(int aIndex, int Mana, BYTE Ipos, unsigned char flag, WORD BP);
	void GCInventoryItemDeleteSend(int aIndex, BYTE pos, unsigned char flag);
	void GCItemUseSpecialTimeSend(int aIndex, unsigned char number, int time);
	void GCItemDurSend(int aIndex, BYTE pos, unsigned char dur, unsigned char flag);
	void GCItemDurSend2(int aIndex, BYTE pos, unsigned char dur, unsigned char flag);
	void CGWeatherSend(int aIndex, BYTE weather);
	void GCServerCmd(int aIndex, BYTE type, unsigned char Cmd1, unsigned char Cmd2);
	void GCReqmoveDevilSquare(PMSG_REQ_MOVEDEVILSQUARE* lpMsg, int aIndex);
	void GCReqDevilSquareRemainTime(PMSG_REQ_DEVILSQUARE_REMAINTIME* lpMsg, int aIndex);
	void AllSendMsg(unsigned char* Msg, int size);
	void AllSendSameMapMsg(unsigned char* Msg, int size, unsigned char mapnumber);
	void GCSendPing(int aIndex);
	void GCPingSendRecv(PMSG_PING_RESULT* aRecv, int aIndex);
	void GCRegEventChipRecv(PMSG_REGEVENTCHIP* lpMsg, int aIndex);
	void GCGetMutoNumRecv(PMSG_GETMUTONUMBER* lpMsg, int aIndex);
	void GCUseEndEventChipRescv(int aIndex);
	void GCUseRenaChangeZenRecv(PMSG_EXCHANGE_EVENTCHIP* lpMsg, int aIndex);
	void CGRequestQuestInfo(int aIndex);
	void GCSendQuestInfo(int aIndex, int QuestIndex);
	void CGSetQuestState(PMSG_SETQUEST* lpMsg, int aIndex);
	void GCSendQuestPrize(int aIndex, unsigned char Type, unsigned char Count);
	void CGCloseWindow(int aIndex);
	void CGRequestEnterBloodCastle(PMSG_REQ_MOVEBLOODCASTLE* lpMsg, int iIndex);
	void CGRequestEnterChaosCastle(PMSG_REQ_MOVECHAOSCASTLE* lpMsg, int iIndex);
	void CGRequestRepositionUserInChaosCastle(PMSG_REQ_REPOSUSER_IN_CC* lpMsg, int aIndex);
	void CGRequestEventEnterCount(PMSG_REQ_CL_ENTERCOUNT* lpMsg, int aIndex);
	void CGRequestLottoRegister(PMSG_REQ_2ANV_LOTTO_EVENT* lpMsg, int aIndex);
	void CGReqMoveOtherServer(PMSG_REQ_MOVE_OTHERSERVER* lpMsg, int aIndex);
	void GCPacketCheckSumRecv(PMSG_PACKETCHECKSUM* aRecv, int aIndex);
	void CGDuelStartRequestRecv(PMSG_REQ_START_DUEL* lpMsg, int aIndex);
	void CGDuelEndRequestRecv(PMSG_REQ_END_DUEL* lpMsg, int aIndex);
	void CGDuelOkRequestRecv(PMSG_ANS_DUEL_OK* lpMsg, int aIndex);
	void GCSendDuelScore(int aIndex1, int aIndex2);
	void GCSendGetItemInfoForParty(int aIndex, class CMapItem* lpItem);
	void GCSendEffectInfo(int aIndex, BYTE btType);
	void CGRequestPetItemCommand(PMSG_REQUEST_PET_ITEM_COMMAND* lpMsg, int aIndex);
	void CGRequestPetItemInfo(PMSG_REQUEST_PET_ITEMINFO* lpMsg, int aIndex);
	void GCGuildViewportInfo(PMSG_REQ_GUILDVIEWPORT* aRecv, int aIndex);
	void CGGuildAssignStatus(PMSG_GUILD_ASSIGN_STATUS_REQ* aRecv, int aIndex);
	void CGGuildAssignType(PMSG_GUILD_ASSIGN_TYPE_REQ* aRecv, int aIndex);
	void CGRelationShipReqJoinBreakOff(PMSG_RELATIONSHIP_JOIN_BREAKOFF_REQ* aRecv, int aIndex);
	void CGRelationShipAnsJoinBreakOff(PMSG_RELATIONSHIP_JOIN_BREAKOFF_ANS* aRecv, int aIndex);
	void CGUnionList(PMSG_UNIONLIST_REQ* aRecv, int aIndex);
	void CGRelationShipReqKickOutUnionMember(PMSG_KICKOUT_UNIONMEMBER_REQ* aRecv, int aIndex);
	void CGReqMapSvrAuth(PMSG_REQ_MAPSERVERAUTH* lpMsg, int iIndex);
	void GCAnsMapSvrAuth(int iIndex, int iResult);
	void CGReqCastleSiegeState(PMSG_REQ_CASTLESIEGESTATE* lpMsg, int iIndex);
	void GCAnsCastleSiegeState(int iIndex, int iResult, char* lpszGuildName, char* lpszGuildMaster);
	void CGReqRegCastleSiege(PMSG_REQ_REGCASTLESIEGE* lpMsg, int iIndex);
	void GCAnsRegCastleSiege(int iIndex, int iResult, char* lpszGuildName);
	void CGReqGiveUpCastleSiege(PMSG_REQ_GIVEUPCASTLESIEGE* lpMsg, int iIndex);
	void GCAnsGiveUpCastleSiege(int iIndex, int iResult, int bGiveUp, int iMarkCount, char* lpszGuildName);
	void CGReqGuildRegInfo(PMSG_REQ_GUILDREGINFO* lpMsg, int iIndex);
	void GCAnsGuildRegInfo(int iIndex, int iResult, CSP_ANS_GUILDREGINFO* lpMsgResult);
	void CGReqRegGuildMark(PMSG_REQ_REGGUILDMARK* lpMsg, int iIndex);
	void GCAnsRegGuildMark(int iIndex, int iResult, CSP_ANS_GUILDREGMARK* lpMsgResult);
	void CGReqNpcBuy(PMSG_REQ_NPCBUY* lpMsg, int iIndex);
	void GCAnsNpcBuy(int iIndex, int iResult, int iNpcNumber, int iNpcIndex);
	void CGReqNpcRepair(PMSG_REQ_NPCREPAIR* lpMsg, int iIndex);
	void GCAnsNpcRepair(int iIndex, int iResult, int iNpcNumber, int iNpcIndex, int iNpcHP, int iNpcMaxHP);
	void CGReqNpcUpgrade(PMSG_REQ_NPCUPGRADE* lpMsg, int iIndex);
	void GCAnsNpcUpgrade(int iIndex, int iResult, int iNpcNumber, int iNpcIndex, int iNpcUpType, int iNpcUpValue);
	void CGReqTaxMoneyInfo(PMSG_REQ_TAXMONEYINFO* lpMsg, int iIndex);
	void GCAnsTaxMoneyInfo(int iIndex, int iResult, BYTE btTaxRateChaos, BYTE btTaxRateStore, __int64 i64Money);
	void CGReqTaxRateChange(PMSG_REQ_TAXRATECHANGE* lpMsg, int iIndex);
	void GCAnsTaxRateChange(int iIndex, int iResult, BYTE btTaxType, int iTaxRate);
	void CGReqMoneyDrawOut(PMSG_REQ_MONEYDRAWOUT* lpMsg, int iIndex);
	void GCAnsMoneyDrawOut(int iIndex, int iResult, __int64 i64Money);
	void GCAnsCsGateState(int iIndex, int iResult, int iGateIndex);
	void CGReqCsGateOperate(PMSG_REQ_CSGATEOPERATE* lpMsg, int iIndex);
	void GCAnsCsGateOperate(int iIndex, int iResult, int iGateIndex, int iGateOperate);
	void GCAnsCsGateCurState(int iIndex, int iGateIndex, int iGateOperate);
	void GCAnsCsAccessSwitchState(int iIndex, int iSwitchIndex, int iSwitchUserIndex, BYTE btSwitchState);
	void GCAnsCsAccessCrownState(int iIndex, BYTE btCrownState);
	void GCAnsCsNotifyStart(int iIndex, BYTE btStartState);
	void GCAnsCsNotifyProgress(int iIndex, BYTE btCastleSiegeState, LPCSTR lpszGuildName);
	void GCAnsCsMapSvrTaxInfo(int iIndex, BYTE btTaxType, BYTE btTaxRate);
	void CGReqCsMiniMapData(PMSG_REQ_MINIMAPDATA* lpMsg, int iIndex);
	void GCAnsCsMiniMapData(int iIndex, BYTE btResult);
	void CGReqStopCsMiniMapData(PMSG_REQ_STOPMINIMAPDATA* lpMsg, int iIndex);
	void CGReqCsSendCommand(PMSG_REQ_CSCOMMAND* lpMsg, int iIndex);
	void GCAnsCsSendCommand(int iCsJoinSide, BYTE btTeam, BYTE btX, BYTE btY, BYTE btCommand);
	void GCAnsCsLeftTimeAlarm(BYTE btHour, BYTE btMinute);
	void GCAnsSelfCsLeftTimeAlarm(int iIndex, BYTE btHour, BYTE btMinute);
	void CGReqCsSetEnterHuntZone(PMSG_REQ_CSHUNTZONEENTER* lpMsg, int iIndex);
	void GCAnsCsSetEnterHuntZone(int iIndex, BYTE btResult, BYTE btEnterHuntZone);
	void CGReqNpcDbList(PMSG_REQ_NPCDBLIST* lpMsg, int iIndex);
	void CGReqCsRegGuildList(PMSG_REQ_CSREGGUILDLIST* lpMsg, int iIndex);
	void CGReqCsAttkGuildList(PMSG_REQ_CSATTKGUILDLIST* lpMsg, int iIndex);
	void CGReqWeaponUse(PMSG_REQ_USEWEAPON* aRecv, int iIndex);
	void CGReqWeaponDamageValue(PMSG_REQ_WEAPON_DAMAGE_VALUE* aRecv, int iIndex);
	void CGReqGuildMarkOfCastleOwner(PMSG_REQ_GUILDMARK_OF_CASTLEOWNER* aRecv, int iIndex);
	void CGReqJewelMix(PMSG_REQ_JEWEL_MIX* lpMsg, int iIndex);
	void GCAnsJewelMix(int iIndex, int iResult);
	void CGReqJewelUnMix(PMSG_REQ_JEWEL_UNMIX* lpMsg, int iIndex);
	void GCAnsJewelUnMix(int iIndex, int iResult);
	void CGReqCrywolfInfo(PMSG_REQ_CRYWOLF_INFO* lpMsg, int iIndex);
	void GCAnsCrywolfInfo(int iIndex, BYTE btOccupationState, BYTE btCrywolfState);
	void CGReqAlatrContract(PMSG_REQ_CRYWOLF_ALTAR_CONTRACT* lpMsg, int iIndex);
	void CGReqPlusChaosRate(PMSG_REQ_CRYWOLF_BENEFIT_PLUS_CHAOSRATE* lpMsg, int iIndex);
	void CGReqKanturuStateInfo(PMSG_REQ_KANTURU_STATE_INFO* lpMsg, int iIndex);
	void GCReqEnterKanturuBossMap(PMSG_REQ_ENTER_KANTURU_BOSS_MAP* lpMsg, int iIndex);
	void GCIllusionTempleSendReward(int aIndex);
	void WereWolfQuarrelCheck(int aIndex);
	void GateKeeperCheck(int aIndex);
	void GCMasterLevelUpMsgSend(int aIndex);
	void CGReqIllusionTempleUseSkill(PMSG_REQ_USEILLUSIONTEMPLESKILL * aRecv, int aIndex);
	void GCSendIllusionTempleKillPoint(int aIndex, BYTE KillPoint);
	void CGReqLuckyCoinRegister(PMSG_REQ_LUCKYCOIN_REGISTER * aRecv, int aIndex);
	void CGReqLuckyCoinInfo(int aIndex);
	void CGReqLuckyCoinTrade(PMSG_REQ_LUCKYCOIN_TRADE * aRecv, int aIndex);
	void GCAnsLuckyCoinTrade(int aIndex, BYTE Result);
	void CGReqCastleHuntZoneEntrance(PMSG_REQ_MOVE_TO_CASTLE_HUNTZONE* aRecv, int aIndex);
	void GCSendObjectCreationState(int iObjectIndex);
	void GCSendObjectHP(int aIndex, int aTargetIndex);
	void CGReqAntiCheatRecv(int aIndex, PMSG_SEND_AH_INFO * aRecv);
	void CGReqFileCrcRecv(int aIndex, PMSG_REQ_CRCDATA * aRecv);
	void GCElementalDamageSend(int aIndex, int TargetIndex, int Damage, int Elemental);
	void GCSendErtelInfo(int aIndex);
	void CGReqEnterAcheron(int aIndex);
	void GCObjectLifeInfo(int iIndex, short sObjNum, int iMaxLife, int iCurLife);
	void CGAgilityRecv(int aIndex, PMSG_AGILITYSEND * aRecv);
	void GCPriceSend(int aIndex, BYTE type, SHOP_DATA *lpShopData);
	int OnCGInviteDuel(LPPMSG_REQ_DUEL_INVITE lpMsg, int aIndex);
	int OnCGAnswerDuel(LPPMSG_ANS_DUEL_ANSWER lpMsg, int aIndex);
	int OnCGLeaveDuel(LPPMSG_REQ_DUEL_EXIT lpMsg, int aIndex);
	int OnDuelChannelJoin(LPPMSG_REQ_DUEL_JOINCNANNEL lpMsg, int aIndex);
	int OnDuelChannelLeave(LPPMSG_REQ_DUEL_LEAVECNANNEL lpMsg, int aIndex);
	void CGReqRegGensMember(_tagPMSG_REQ_REG_GENS_MEMBER *lpMsg, int iIndex);
	void CGReqSecedeGensMember(_tagPMSG_REQ_SEGEDE_GENS_MEMBER *lpMsg, int iIndex);
	void CGReqGensReward(_tagPMSG_GENS_REWARD_CODE *lpMsg, int iIndex);
	void CGReqGensMemberInfo(_tagPMSG_REQ_GENS_INFO *lpMsg, int iIndex);
	void CGInventoryEquipment(_tagPMSG_REQ_INVENTORY_EQUIPMENT_ITEM *lpMsg, int aIndex);
	void CGReqUseTransformationRing(PMSG_REQ_USETRANSFORMATIONRING *lpMsg, int iIndex);
	void GCSendAttackSpeed(int iIndex);
	void GCChaosMachinePriceSend(int aIndex);
	void CGReqEnterDoppelGanger(PMSG_REQ_ENTER_DOPPELGANGER *lpMsg, int iIndex);
	void CGReqEnterZone(PMSG_REQ_ENTER_ZONE *lpMsg, int iIndex);
	void CGReqInJewelPentagramItem(PMSG_REQ_IN_PENTAGRAM_JEWEL *lpMsg, int aIndex);
	void GCAnsInJewelPentagramItem(int aIndex, int iResult, BYTE btJewelPos, BYTE btJewelIndex, BYTE btItemType, WORD wItemIndex, BYTE btMainAttribute, BYTE btLevel, BYTE btRank1OptionNum, BYTE btRank1Level, BYTE btRank2OptionNum, BYTE btRank2Level, BYTE btRank3OptionNum, BYTE btRank3Level, BYTE btRank4OptionNum, BYTE btRank4Level, BYTE btRank5OptionNum, BYTE btRank5Level);
	void CGReqOutJewelPentagramItem(PMSG_REQ_OUT_PENTAGRAM_JEWEL *lpMsg, int aIndex);
	void GCAnsOutJewelPentagramItem(int iIndex, int iResult, BYTE btJewelPos, BYTE btJewelDBIndex);
	void CGReqRefinePentagramJewel(PMSG_REQ_REFINE_PENTAGRAM_JEWEL *lpMsg, int aIndex);
	void GCAnsRefinePentagramJewel(int iIndex, BYTE btResult);
	void CGReqUpgradePentagramJewel(PMSG_REQ_UPGRADE_PENTAGRAM_JEWEL *lpMsg, int aIndex);
	void GCAnsUpgradePentagramJewel(int iIndex, BYTE btResult);
	void GCAnsPentagramJewelInOut(int iIndex, BYTE btResult);
	void CGReqArcaBattleGuildMasterJoin(_tagPMSG_REQ_ARCA_BATTLE_JOIN *lpMsg, int iIndex);
	void CGReqArcaBattleGuildMemberJoin(_tagPMSG_REQ_ARCA_BATTLE_JOIN *lpMsg, int iIndex);
	void CGReqArcaBattleEnter(_tagPMSG_REQ_ARCA_BATTLE_ENTER *lpMsg, int iIndex);
	void CGReqAcheronEnter(_tagPMSG_REQ_ACHERON_ENTER *lpMsg, int iIndex);
	void CGReqArcaBattleBootyExchange(int iIndex);
	void CGReqSpritemapExchange(int iIndex);
	void CGReqRegisteredMemberCnt(int iIndex);
	void GCSendRecvCheck(int aIndex);
	void CGReqAntihackBreach(int aIndex, PMSG_ANTIHACK_BREACH *lpMsg);
	void CGReqAntihackCheck(int aIndex, PMSG_ANTIHACK_CHECK *lpMsg);
	void GCMuunItemDurSend(int aIndex, BYTE pos, BYTE dur);
	void GCMuunEquipmentChange(int aIndex, int pos);
	void GCMuunInventoryItemDeleteSend(int aIndex, BYTE pos, BYTE flag);
	void GCMuunInventoryItemOneSend(int aIndex, int pos);
	void CGMuunItemGetRequest(PMSG_MUUNITEM_GETREQUEST *lpMsg, int aIndex);
	void CGMuunItemSellRequestRecv(PMSG_MUUN_ITEM_SELLREQUEST *lpMsg, int aIndex);
	void CGReqGuildMatchingList(PMSG_REQ_GUILDMATCHINGLIST *lpMsg, int nUserIndex);
	void CGReqGuildMatchingListSearchWord(PMSG_REQ_GUILDMATCHINGLISTSEARCHWORD *lpMsg, int nUserIndex);
	void CGReqRegGuildMatchingList(PMSG_REQ_REGGUILDMATCHINGDATA *lpMsg, int nUserIndex);
	void CGReqCancelGuildMatchingList(PMSG_REQ_CANCEL_GUILDMATCHINGLIST *lpMsg, int nUserIndex);
	void CGReqJoinGuildMatching(PMSG_REQ_JOIN_GUILDMATCHING *lpMsg, int nUserIndex);
	void CGReqCancelJoinGuildMatching(int nUserIndex);
	void CGReqAllowJoinGuildMatching(PMSG_REQ_ALLOW_JOIN_GUILDMATCHING *lpMsg, int nUserIndex);
	void CGReqGetAllowListJoinGuildMatching(PMSG_REQ_ALLOWLIST_GUILDMATCHING *lpMsg, int nUserIndex);
	void CGReqGetWaitStateListGuildMatching(int nUserIndex);

	void CGReqRegWantedPartyMember(PMSG_REQ_REG_WANTED_PARTYMEMBER *lpMsg, int nUserIndex);
	void CGReqGetPartyMatchingList(PMSG_REQ_GET_PARTYMATCHINGLIST *lpMsg, int nUserIndex);
	void CGReqJoinMemberPartyMatching(PMSG_REQ_MEMBERJOIN_PARTYMATCHINGLIST *lpMsg, int nUserIndex);
	void CGReqMemberJoinStateList(PMSG_REQ_MEMBERJOIN_STATELIST_PARTYMATCHING *lpMsg, int nUserIndex);
	void CGReqMemberJoinStateListLeader(PMSG_REQ_WAITLIST_PARTYMATCHING *lpMsg, int nUserIndex);
	void CGReqAcceptJoinMemberPartyMatching(PMSG_REQ_ACCEPTMEMBER_PARTYMATCHING *lpMsg, int nUserIndex);
	void CGReqCancelPartyMatching(PMSG_REQ_CANCEL_JOIN_PARTYMATCHING *lpMsg, int nUserIndex);

	void GCDisplayBuffeffectPartyMember(int aIndex);
	void GCPartyMemberPosition(int nIndex);
	void CGReqSendMemberPosInfoStart(int nIndex);
	void CGReqSendMemberPosInfoStop(int nIndex);
	void GCReqSendNpcPosInfo(PMSG_REQ_NPC_POSITION *lpMsg, int nIndex);

	void CGReqCCF_DayTime(int iIndex);
	void CGReqCCF_EnterCheck(int iIndex);
	void CG_CCF_UI_OnOff(PMSG_CCF_UI_ONOFF *lpMsg, int nUserIndex);
	int	 CGReqEnterChaosCastleFinal(int iIndex);
	void CGRequestRepositionUserInCCF(PMSG_REQ_REPOSUSER_IN_CCF *lpMsg, int aIndex);
	void CGReq_CCF_Ranking(PMSG_REQ_CCF_RANKING *lpMsg, int aIndex);

	void CGReqEventInvenOpen(LPBYTE lpRecv, int aIndex);
	void CGEventItemGetRequest(PMSG_EVENTITEM_GETREQUEST *lpMsg, int aIndex);
	bool CGEventItemDropRequest(PMSG_EVENTITEM_THROW *lpMsg, int aIndex);
	void GCEventInventoryItemListSend(int aIndex);
	void GCEventItemDurSend(int aIndex, BYTE pos, BYTE dur);
	void GCEventInvenItemOneSend(int aIndex, int pos);
	void GCEventInventoryItemDeleteSend(int aIndex, BYTE pos, unsigned char flag);

	void GCResSendExpEventInfo(int aIndex);

	void CGReq_ITL_Relics(PMSG_REQ_ITL_RELATE_RELICS *lpMsg, int iIndex);
	void CG_Req_Enter_ITR(PMSG_REQ_ENTER_ITR *lpMsg, int iIndex);
	void CG_ReqAcceptEnterITR(PMSG_REQ_ACCEPTENTER_ITR *lpMsg, int iIndex);
	BYTE EnterITRCheckUser(int iIndex, int &TroubleUserIndex);

	void CGReqUBFMyCharacterInfo(PMSG_REQ_UBF_INFO *lpMsg, int iIndex);
	void CGReqUBFJoin(PMSG_REQ_UBF_JOIN *lpMsg, int iIndex);
	void CGReqUBFCancel(PMSG_REQ_UBF_CANCEL *lpMsg, int iIndex);
	void CGReqUBFGetRealName(PMSG_REQ_UBF_REAL_NAME *lpMsg, int iIndex);
	void CGReqUBFGetReward(PMSG_REQ_GET_REWARD *lpMsg, int iIndex);

	void CGReqDSFSchedule(int iIndex);
	void CGReqDSFEnter(int iIndex);
	int EnterDSFCheckUser(int iIndex, int & TroubleUserIndex);
	void CGReqAcceptEnterDSF(PMSG_REQ_DSF_ACCEPTENTER *lpMsg, int iIndex);
	void CGReqDSFGoFinalParty(PMSG_REQ_DSF_GO_FINAL_PARTY_INFO *lpMsg, int iIndex);
	void CGReqDSFGetReward(PMSG_REQ_DSF_GET_REWARD *lpMsg, int iIndex);

	void CGReqDoUsePopupType(int aIndex);
	void GCSendEventBanner(int aIndex, int iBannerType);

	void GCSendRareItemPriceInfo(int aIndex);
	void GCSendDisableReconnect(int aIndex);
	bool PacketCheckTime(int aIndex);
	void GCSendDisableReconnectSystem(int aIndex);
	void GCAlterPShopVault(int aIndex, BYTE type);
	void GCSetCharColors(int aIndex);

	void GCSendRuud(int aIndex, int iRuud, int iObtainedRuud, bool bIsObtainedRuud);
	void CGReqUseBoxInInventory(int aIndex, PMSG_REQ_USE_BOX * aRecv);

	void CGEventEntryNotice(int EventType, BYTE state); // event entry notify
	void CGReqNixieBossEnter(_tagPMSG_REQ_NIXIELAKE_ENTER *lpMsg, int iIndex);
	void CGReqHuntingLog(int iIndex);

	void GCPlayerStatsPanelRates(int aIndex);
	void GCPlayerStatsPanelNew(int aIndex);

	void CGRequestStartMuBot(PMSG_MUBOT_REQ_START* lpMsg, int aIndex);
	void CGSaveMuBotSettings(PMSG_MUBOT_SETTINGS_RECV* lpMsg, int aIndex);
	void GCSendMuBotSettings(int aIndex, BYTE* lpData);

	void CGRecvHitHackValues(int aIndex, PMSG_SEND_HITHACK_INFO * lpMsg);*/
	bool First = true;

	CQuery m_CharDB;
	CQuery m_AccDB;
	CQuery m_GSDB;
	CQuery m_PetDB;
	CQuery m_CastleDB;
	CQuery m_CrywolfDB;
	CQuery m_PentagramDB;
	CQuery m_EventDB;
	CQuery m_RankingDB;
	CQuery m_ItemShopDB;
	CQuery m_PeriodItemDB;
	CQuery m_CharMiscDB;
	CQuery m_ArcaDB;
	CQuery m_QuestExpDB;
	CQuery m_LuckyItemDB;
	CQuery m_CCFinalDB;
	CQuery m_RummyDB;
	CQuery m_MineDB;
	CQuery m_PShopDB;
	CQuery m_EventInvDB;
	CQuery m_MuunDB;
	CQuery m_OptionDataDB;
	CQuery m_ReBuyDB;
	CQuery m_GremoryCaseDB;
	CQuery m_BattleCoreDB;
	CQuery m_DSFinalDB;
	CQuery m_EvoMonDB;
	CQuery m_Survivor;

private:
	bool DataEncryptCheck(int aIndex, BYTE protoNum, BOOL Encrypt);
};

extern GameProtocol GSProtocol;

#endif
