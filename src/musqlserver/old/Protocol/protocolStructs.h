
#ifndef _PROTOCOLSTRUCTS_H
#define _PROTOCOLSTRUCTS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "GensSystemProtocol.h"
#include "User/user.h"

#include <string>
#include <map>

#define MAX_CHAT_LEN 90

// Forward Declarations.

struct BAN_REQ_USER;
struct BLOCKCHAT_DATA;
struct CHAT_LOG_DATA;
struct CLASSDEF_DATA;
struct CONNECT_OBJECT;
struct CSP_ANS_ALLGUILDREGINFO;
struct CSP_ANS_CALCREGGUILDLIST;
struct CSP_ANS_CASTLEOWNERCHANGE;
struct CSP_ANS_CASTLESIEGEEND;
struct CSP_ANS_CASTLESTATESYNC;
struct CSP_ANS_CASTLETRIBUTEMONEY;
struct CSP_ANS_CSGUILDUNIONINFO;
struct CSP_ANS_CSINITDATA;
struct CSP_ANS_CSLOADTOTALGUILDINFO;
struct CSP_ANS_CSSAVETOTALGUILDINFO;
struct CSP_ANS_GUILDREGINFO;
struct CSP_ANS_GUILDREGMARK;
struct CSP_ANS_GUILDRESETMARK;
struct CSP_ANS_GUILDSETGIVEUP;
struct CSP_ANS_MAPSVRMULTICAST;
struct CSP_ANS_MONEYCHANGE;
struct CSP_ANS_NPCBUY;
struct CSP_ANS_NPCDATA;
struct CSP_ANS_NPCREMOVE;
struct CSP_ANS_NPCREPAIR;
struct CSP_ANS_NPCSAVEDATA;
struct CSP_ANS_NPCUPDATEDATA;
struct CSP_ANS_NPCUPGRADE;
struct CSP_ANS_OWNERGUILDMASTER;
struct CSP_ANS_REGATTACKGUILD;
struct CSP_ANS_RESETCASTLETAXINFO;
struct CSP_ANS_RESETREGSIEGEINFO;
struct CSP_ANS_RESETSIEGEGUILDINFO;
struct CSP_ANS_SDEDCHANGE;
struct CSP_ANS_SIEGEENDCHANGE;
struct CSP_ANS_TAXINFO;
struct CSP_ANS_TAXRATECHANGE;
struct CSP_CALCREGGUILDLIST;
struct CSP_CSGUILDUNIONINFO;
struct CSP_CSINITDATA;
struct CSP_CSLOADTOTALGUILDINFO;
struct CSP_CSSAVETOTALGUILDINFO;
struct CSP_GLOBALPOST_MULTICAST;
struct CSP_GUILDREGINFO;
struct CSP_NPCSAVEDATA;
struct CSP_NPCUPDATEDATA;
struct CSP_REQ_ALLGUILDREGINFO;
struct CSP_REQ_CALCREGGUILDLIST;
struct CSP_REQ_CASTLEDATA;
struct CSP_REQ_CASTLEOWNERCHANGE;
struct CSP_REQ_CASTLESIEGEEND;
struct CSP_REQ_CASTLESTATESYNC;
struct CSP_REQ_CASTLETRIBUTEMONEY;
struct CSP_REQ_CSGUILDUNIONINFO;
struct CSP_REQ_CSINITDATA;
struct CSP_REQ_CSLOADTOTALGUILDINFO;
struct CSP_REQ_CSSAVETOTALGUILDINFO;
struct CSP_REQ_GUILDREGINFO;
struct CSP_REQ_GUILDREGMARK;
struct CSP_REQ_GUILDRESETMARK;
struct CSP_REQ_GUILDSETGIVEUP;
struct CSP_REQ_MAPSVRMULTICAST;
struct CSP_REQ_MONEYCHANGE;
struct CSP_REQ_NPCBUY;
struct CSP_REQ_NPCDATA;
struct CSP_REQ_NPCREMOVE;
struct CSP_REQ_NPCREPAIR;
struct CSP_REQ_NPCSAVEDATA;
struct CSP_REQ_NPCUPDATEDATA;
struct CSP_REQ_NPCUPGRADE;
struct CSP_REQ_OWNERGUILDMASTER;
struct CSP_REQ_REGATTACKGUILD;
struct CSP_REQ_RESETCASTLETAXINFO;
struct CSP_REQ_RESETREGSIEGEINFO;
struct CSP_REQ_RESETSIEGEGUILDINFO;
struct CSP_REQ_SDEDCHANGE;
struct CSP_REQ_SIEGEENDCHANGE;
struct CSP_REQ_TAXINFO;
struct CSP_REQ_TAXRATECHANGE;
struct CWP_ANS_CRYWOLFINFOLOAD;
struct CWP_ANS_CRYWOLFINFOSAVE;
struct CWP_ANS_CRYWOLFSYNC;
struct CWP_REQ_CRYWOLFINFOLOAD;
struct CWP_REQ_CRYWOLFINFOSAVE;
struct CWP_REQ_CRYWOLFSYNC;
struct DSF_GO_FINAL_PARTY;
struct DSF_TEAM_PARTY_USER_NAME;
struct DSMSG_ANS_SUBSERVER_COUNT;
struct DSMSG_ANS_WHISPER_RESULT;
struct DSMSG_CHAOSMACHINE_LOSTITEMS;
struct DSMSG_GS_WHISPER_RESULT;
struct DSMSG_REQ_SUBSERVER_COUNT;
struct DS_GET_MONSTERCNT;
struct DS_SAVE_MONSTERCNT;
struct DS_SAVE_PLAYERKILLER;
struct EXSDHP_GUILD_ASSIGN_STATUS_REQ;
struct EXSDHP_GUILD_ASSIGN_STATUS_RESULT;
struct EXSDHP_GUILD_ASSIGN_TYPE_REQ;
struct EXSDHP_GUILD_ASSIGN_TYPE_RESULT;
struct EXSDHP_KICKOUT_UNIONMEMBER_REQ;
struct EXSDHP_KICKOUT_UNIONMEMBER_RESULT;
struct EXSDHP_NOTIFICATION_RELATIONSHIP;
struct EXSDHP_RELATIONSHIP_BREAKOFF_REQ;
struct EXSDHP_RELATIONSHIP_BREAKOFF_RESULT;
struct EXSDHP_RELATIONSHIP_JOIN_REQ;
struct EXSDHP_RELATIONSHIP_JOIN_RESULT;
struct EXSDHP_SERVERGROUP_GENS_CHATTING_RECV;
struct EXSDHP_SERVERGROUP_GUILD_CHATTING_RECV;
struct EXSDHP_SERVERGROUP_UNION_CHATTING_RECV;
struct EXSDHP_UNION_LIST;
struct EXSDHP_UNION_LIST_COUNT;
struct EXSDHP_UNION_LIST_REQ;
struct EXSDHP_UNION_RELATIONSHIP_LIST;
struct FCHP_CHATROOM_CREATE_REQ;
struct FCHP_CHATROOM_CREATE_RESULT;
struct FCHP_CHATROOM_INVITATION_REQ;
struct FHP_FRIENDLIST;
struct FHP_FRIENDLIST_COUNT;
struct FHP_FRIENDLIST_REQ;
struct FHP_FRIEND_ADD_REQ;
struct FHP_FRIEND_ADD_RESULT;
struct FHP_FRIEND_CHATROOM_CREATE_REQ;
struct FHP_FRIEND_CHATROOM_CREATE_RESULT;
struct FHP_FRIEND_DEL_RESULT;
struct FHP_FRIEND_INVITATION_REQ;
struct FHP_FRIEND_INVITATION_RET;
struct FHP_FRIEND_MEMO_DEL_REQ;
struct FHP_FRIEND_MEMO_DEL_RESULT;
struct FHP_FRIEND_MEMO_LIST;
struct FHP_FRIEND_MEMO_LIST_REQ;
struct FHP_FRIEND_MEMO_RECV;
struct FHP_FRIEND_MEMO_RECV_OLD;
struct FHP_FRIEND_MEMO_RECV_REQ;
struct FHP_FRIEND_MEMO_SEND;
struct FHP_FRIEND_MEMO_SEND_OLD;
struct FHP_FRIEND_MEMO_SEND_RESULT;
struct FHP_FRIEND_STATE;
struct FHP_FRIEND_STATE_C;
struct FHP_WAITFRIENDLIST_COUNT;
struct FHP_WAITFRIEND_ADD_REQ;
struct FHP_WAITFRIEND_ADD_RESULT;
struct FRIEND_MASTER;
struct GENS_USER;
struct GS_CONNECT_INFO;
struct GUILD_INFO_STRUCT;
struct ISHOP_ANS_ITEMLIST;
struct ISHOP_ANS_POINT;
struct ISHOP_CHARCARD_BUY;
struct ISHOP_ITEMLIST;
struct ISHOP_ITEM_BUY;
struct ISHOP_ITEM_BUYANS;
struct ISHOP_ITEM_BUY_PACKAGE;
struct ISHOP_ITEM_DELETE;
struct ISHOP_ITEM_GIFT;
struct ISHOP_ITEM_GIFTANS;
struct ISHOP_ITEM_GIFT_PACKAGE;
struct ISHOP_ITEM_PACKAGE;
struct ISHOP_ITEM_ROLLBACK;
struct ISHOP_ITEM_USE;
struct ISHOP_ITEM_USEANS;
struct ISHOP_POINT_ADD;
struct ISHOP_REQ_ITEMLIST;
struct ISHOP_REQ_POINT;
struct ISHOP_VIP_BUY;
struct ITEMPERIOD_DATEINFO;
struct JOIN_SERVER_SERVER_DATA;
struct JOIN_SERVER_USER_DATA;
struct MEMO_HEADER;
struct MEMO_READ;
struct MEMO_SEND_HEADER;
struct MUBOT_SETTINGS_REQ_SAVE;
struct MUBOT_SETTINGS_SEND;
struct NAME;
struct PARTYMATCHING_PARTYDATA;
struct PARTYMATCHING_PARTYUSERDATA;
struct PERIOD_EXPIRED_ITEM;
struct PERIOD_EXPIRED_ITEMLIST;
struct PERIOD_ITEMLIST;
struct PMSG_ACTION;
struct PMSG_ACTIONRESULT;
struct PMSG_ADDON_REQUEST;
struct PMSG_ADDSTATS;
struct PMSG_ADDSTATS_RESULT;
struct PMSG_AGILITYSEND;
struct PMSG_AHINFO;
struct PMSG_ALTERPSHOPVAULT;
struct PMSG_ANS_2ANIV_SERIAL;
struct PMSG_ANS_AB_ALL_JOIN_USER_DS;
struct PMSG_ANS_AB_JOIN_CANCEL_DS;
struct PMSG_ANS_AB_JOIN_MEMBER_UNDER_DS;
struct PMSG_ANS_AB_PROC_STATE_DS;
struct PMSG_ANS_AB_REG_MEMBER_CNT_DS;
struct PMSG_ANS_AB_WIN_GUILD_INFO_DS;
struct PMSG_ANS_ACCEPTMEMBER_PARTYMATCHING;
struct PMSG_ANS_ACHERON_ENTER;
struct PMSG_ANS_ACHERON_EVENT_ENTER;
struct PMSG_ANS_AE_PLAY_DS;
struct PMSG_ANS_ALLOWLIST_STATE_GUILDMATCHING;
struct PMSG_ANS_ALLOW_JOIN_GUILDMATCHING;
struct PMSG_ANS_ALL_GUILD_MARK_CNT_DS;
struct PMSG_ANS_ARCA_BATTLE_ENTER_DS;
struct PMSG_ANS_ARCA_BATTLE_GUILD_JOIN_DS;
struct PMSG_ANS_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS;
struct PMSG_ANS_ARCA_BATTLE_MARK_CNT_DS;
struct PMSG_ANS_ARCA_BATTLE_MARK_RANK_DS;
struct PMSG_ANS_ARCA_BATTLE_MARK_REG_DS;
struct PMSG_ANS_BLOODCASTLESCORE_5TH;
struct PMSG_ANS_BLOODCASTLE_ENTERCOUNT;
struct PMSG_ANS_CANCEL_GUILDMATCHINGLIST;
struct PMSG_ANS_CANCEL_JOIN_GUILDMATCHING;
struct PMSG_ANS_CANCEL_JOIN_PARTYMATCHING;
struct PMSG_ANS_CASTLESIEGESTATE;
struct PMSG_ANS_CCF_RANK;
struct PMSG_ANS_CHAOSCASTLE_RANKING;
struct PMSG_ANS_CHAOS_MULTIMIX_CHECK;
struct PMSG_ANS_CLASSDEF;
struct PMSG_ANS_CSATTKGUILDLIST;
struct PMSG_ANS_CSCOMMAND;
struct PMSG_ANS_CSGATECURSTATE;
struct PMSG_ANS_CSGATEOPERATE;
struct PMSG_ANS_CSGATESTATE;
struct PMSG_ANS_CSHUNTZONEENTER;
struct PMSG_ANS_CSLEFTTIMEALARM;
struct PMSG_ANS_DELETE_STONES;
struct PMSG_ANS_DSF_ACCEPTENTER;
struct PMSG_ANS_DSF_CAN_PARTY_ENTER;
struct PMSG_ANS_DSF_ENTER;
struct PMSG_ANS_DSF_GET_REWARD;
struct PMSG_ANS_DSF_GO_FINAL_PARTY;
struct PMSG_ANS_DSF_GO_FINAL_PARTY_INFO;
struct PMSG_ANS_DSF_PARTYRANKRENEW;
struct PMSG_ANS_DSF_SCHEDULE;
struct PMSG_ANS_DUEL_ANSWER;
struct PMSG_ANS_DUEL_OK;
struct PMSG_ANS_END_DUEL;
struct PMSG_ANS_ENTER_ON_QUESTNPC;
struct PMSG_ANS_EVENTENTRY_INFO;
struct PMSG_ANS_EVENTUSERSCORE;
struct PMSG_ANS_EVOMON_MAXSCORE;
struct PMSG_ANS_GET_DSF_REWARD;
struct PMSG_ANS_GET_PARTYMATCHINGLIST;
struct PMSG_ANS_GET_REWARD;
struct PMSG_ANS_GET_UBF_REAL_NAME;
struct PMSG_ANS_GIVEUPCASTLESIEGE;
struct PMSG_ANS_GUILDMARK_OF_CASTLEOWNER;
struct PMSG_ANS_GUILDMATCHINGLIST;
struct PMSG_ANS_GUILDREGINFO;
struct PMSG_ANS_HUNTING_LOG;
struct PMSG_ANS_ILLUSIONTEMPLE_RANKING;
struct PMSG_ANS_INVENTORY_EQUIPMENT_ITEM;
struct PMSG_ANS_IN_PENTAGRAM_JEWEL;
struct PMSG_ANS_ITL_RELATE_RELICS;
struct PMSG_ANS_ITL_SCHEDULE;
struct PMSG_ANS_JEWEL_UNMIX;
struct PMSG_ANS_JOIN_GUILDMATCHING;
struct PMSG_ANS_LUCKYCOIN;
struct PMSG_ANS_LUCKYCOININFO;
struct PMSG_ANS_LUCKYCOIN_REGISTER;
struct PMSG_ANS_LUCKYCOIN_TRADE;
struct PMSG_ANS_LUCKYITEM_SELECT;
struct PMSG_ANS_MAPMOVE;
struct PMSG_ANS_MAPSVRAUTH;
struct PMSG_ANS_MAPSVRMOVE;
struct PMSG_ANS_MAPSVRTAXINFO;
struct PMSG_ANS_MEMBERJOIN_PARTYMATCHINGLIST;
struct PMSG_ANS_MEMBERJOIN_STATELIST_PARTYMATCHING;
struct PMSG_ANS_MINETWINKLE;
struct PMSG_ANS_MINETWINKLE_CANCEL;
struct PMSG_ANS_MINETWINKLE_END_ANIMATION;
struct PMSG_ANS_MINETWINKLE_REWARD;
struct PMSG_ANS_MINIMAPDATA;
struct PMSG_ANS_MONEYDRAWOUT;
struct PMSG_ANS_MOVE_TO_CASTLE_HUNTZONE;
struct PMSG_ANS_MURUMMY_SELECT_DS;
struct PMSG_ANS_MUUN_EXCHANGE;
struct PMSG_ANS_NIXIELAKE_ENTER;
struct PMSG_ANS_NOTIFYCROWNPROC;
struct PMSG_ANS_NOTIFYCSPROGRESS;
struct PMSG_ANS_NOTIFYCSSTART;
struct PMSG_ANS_NOTIFYSWITCHPROC;
struct PMSG_ANS_NOTI_GUILDMATCHING;
struct PMSG_ANS_NOTI_GUILDMATCHING_FORGUILDMASTER;
struct PMSG_ANS_NOTI_JOINPARTYMATCHING;
struct PMSG_ANS_NPCBUY;
struct PMSG_ANS_NPCDBLIST;
struct PMSG_ANS_NPCREPAIR;
struct PMSG_ANS_NPCUPGRADE;
struct PMSG_ANS_OUT_PENTAGRAM_JEWEL;
struct PMSG_ANS_PARTYMATCHING_ERROR;
struct PMSG_ANS_PENTAGRAMJEWEL;
struct PMSG_ANS_PERIODBUFF_SELECT;
struct PMSG_ANS_PERIODITEMEX_DELETE;
struct PMSG_ANS_PERIODITEMEX_EXPIRED_ITEMLIST;
struct PMSG_ANS_PERIODITEMEX_INSERT;
struct PMSG_ANS_PERIODITEMEX_LIST;
struct PMSG_ANS_PERIODITEMEX_LIST_COUNT;
struct PMSG_ANS_PSHOPITEMVALUE_INFO;
struct PMSG_ANS_QUESTEXP_INFO;
struct PMSG_ANS_REGCASTLESIEGE;
struct PMSG_ANS_REGGUILDMARK;
struct PMSG_ANS_REGGUILDMATCHINGDATA;
struct PMSG_ANS_REGISTER_EVENTCHIP;
struct PMSG_ANS_REGISTER_STONES;
struct PMSG_ANS_REG_CC_OFFLINE_GIFT;
struct PMSG_ANS_REG_DL_OFFLINE_GIFT;
struct PMSG_ANS_REG_HT_OFFLINE_GIFT;
struct PMSG_ANS_REG_LUCKYCOIN;
struct PMSG_ANS_REG_RINGGIFT;
struct PMSG_ANS_REG_WANTED_PARTYMEMBER;
struct PMSG_ANS_REMOVE_ALL_GUILD_BUFF_DS;
struct PMSG_ANS_RESET_EVENTCHIP;
struct PMSG_ANS_REWARDLIST;
struct PMSG_ANS_RUUD_STORE_BUYITEM;
struct PMSG_ANS_SANTACHECK;
struct PMSG_ANS_SANTAGIFT;
struct PMSG_ANS_SWITCHWARE;
struct PMSG_ANS_TAXMONEYINFO;
struct PMSG_ANS_TAXRATECHANGE;
struct PMSG_ANS_UBF_ACCOUNT_USERINFO;
struct PMSG_ANS_UBF_CANCEL;
struct PMSG_ANS_UBF_GET_REWARD;
struct PMSG_ANS_UBF_INFO;
struct PMSG_ANS_UBF_JOIN;
struct PMSG_ANS_UBF_SET_RECEIVED_REWARD;
struct PMSG_ANS_USEWEAPON;
struct PMSG_ANS_USE_BOX;
struct PMSG_ANS_USE_POPUP;
struct PMSG_ANS_VIEW_EC_MN;
struct PMSG_ANS_VIEW_STONES;
struct PMSG_ANS_WAITLIST_PARTYMATCHING;
struct PMSG_ANS_WAIT_GUILDMATCHING;
struct PMSG_ANS_WARESAVE;
struct PMSG_ANTIHACK_BREACH;
struct PMSG_ANTIHACK_CHECK;
struct PMSG_ATTACK;
struct PMSG_ATTACKRESULT;
struct PMSG_ATTACKSPEEDSEND;
struct PMSG_BEATTACK;
struct PMSG_BEATTACK_COUNT;
struct PMSG_BLOCKCHAT_LIST;
struct PMSG_BUYREQUEST;
struct PMSG_BUYRESULT;
struct PMSG_CALLMONSTER_VIEWPORTCREATE;
struct PMSG_CCF_UI_ONOFF;
struct PMSG_CHAIN_MAGIC;
struct PMSG_CHAIN_MAGIC_OBJECT;
struct PMSG_CHAOSMIX;
struct PMSG_CHARCHECK;
struct PMSG_CHARCHECK_RESULT;
struct PMSG_CHARCREATE;
struct PMSG_CHARCREATERESULT;
struct PMSG_CHARDELETE;
struct PMSG_CHARLIST;
struct PMSG_CHARLISTCOUNT;
struct PMSG_CHARMAPJOIN;
struct PMSG_CHATDATA;
struct PMSG_CHATDATA_NUMBER;
struct PMSG_CHATDATA_WHISPER;
struct PMSG_CHECK_MAINEXE;
struct PMSG_CHECK_MAINEXE_RESULT;
struct PMSG_CLIENTCLOSE;
struct PMSG_CLIENTMSG;
struct PMSG_CLIENTTIME;
struct PMSG_CSATTKGUILDLIST;
struct PMSG_DEL_PENTAGRAMJEWEL;
struct PMSG_DEL_PSHOPITEM;
struct PMSG_DISABLE_RECONNECT;
struct PMSG_DSF_PLAY_STATE;
struct PMSG_DSF_RESULT;
struct PMSG_DSF_STATE;
struct PMSG_DSF_TEAM_USERNAME;
struct PMSG_DURATION_MAGIC_RECV;
struct PMSG_DURATION_MAGIC_SEND;
struct PMSG_ELEMENTALDAMAGE;
struct PMSG_ENTER_ZONE_RESULT;
struct PMSG_EQUIPMENTLIST;
struct PMSG_EVENT;
struct PMSG_EVENTENTRY_NOTICE;
struct PMSG_EVENTINVENTORY_DELETE;
struct PMSG_EVENTITEM_DUR;
struct PMSG_EVENTITEM_GETREQUEST;
struct PMSG_EVENTITEM_GETRESULT;
struct PMSG_EVENTITEM_THROW;
struct PMSG_EVENTITEM_THROW_RESULT;
struct PMSG_EVENT_INVEN_ITEM_MOD;
struct PMSG_EXCHANGE_EVENTCHIP;
struct PMSG_EX_GAMESERVER_MOVE_RESULT;
struct PMSG_EX_GAMESERVER_MOVE_SEND;
struct PMSG_EX_SKILL_COUNT;
struct PMSG_GETMUTONUMBER;
struct PMSG_GOALSEND;
struct PMSG_GUILDCREATED_RESULT;
struct PMSG_GUILDDELUSER;
struct PMSG_GUILDINFOSAVE;
struct PMSG_GUILDJOINQ;
struct PMSG_GUILDMASTERANSWER;
struct PMSG_GUILDQRESULT;
struct PMSG_GUILDVIEWPORT;
struct PMSG_GUILDVIEWPORT_USER;
struct PMSG_GUILDWARSEND_RESULT;
struct PMSG_GUILD_ASSIGN_STATUS_REQ;
struct PMSG_GUILD_ASSIGN_STATUS_RESULT;
struct PMSG_GUILD_ASSIGN_TYPE_REQ;
struct PMSG_GUILD_ASSIGN_TYPE_RESULT;
struct PMSG_HELLO;
struct PMSG_IDPASS;
struct PMSG_ILLUSION_TEMPLE_USE_MAGIC_RESULT;
struct PMSG_INSERT_PENTAGRAMJEWEL;
struct PMSG_INVENTORYITEMMODIFY;
struct PMSG_INVENTORYITEMMOVE;
struct PMSG_INVENTORYITEMMOVE_RESULT;
struct PMSG_INVENTORYLIST;
struct PMSG_INVENTORYLISTCOUNT;
struct PMSG_ITEMDURREPAIR;
struct PMSG_ITEMGETREQUEST;
struct PMSG_ITEMGETRESULT;
struct PMSG_ITEMTHROW;
struct PMSG_ITEMTHROW_RESULT;
struct PMSG_ITEMVIEWPORTCREATE;
struct PMSG_ITL_GUILD_RANKINFO;
struct PMSG_ITL_PLAY_STATE;
struct PMSG_ITL_RELICS_GET_USER;
struct PMSG_ITL_RESULT;
struct PMSG_ITL_SKILL_END;
struct PMSG_ITL_STATE;
struct PMSG_ITL_STONEINFO;
struct PMSG_ITL_USER_ADD_EXP;
struct PMSG_ITL_USE_MAGIC_RESULT;
struct PMSG_ITR_RESULT;
struct PMSG_ITR_USER_ADD_EXP;
struct PMSG_JOINRESULT;
struct PMSG_KICKOUT_UNIONMEMBER_REQ;
struct PMSG_KILLCOUNT;
struct PMSG_KILLPLAYER_EXT;
struct PMSG_LEVELUP;
struct PMSG_LOGINSEVER_LIVE;
struct PMSG_LUCKYITME_DB_INFO;
struct PMSG_LVPOINTADD;
struct PMSG_LVPOINTADDRESULT;
struct PMSG_MAGICATTACK;
struct PMSG_MAGICATTACK_RESULT;
struct PMSG_MAGICCANCEL;
struct PMSG_MAGICLIST;
struct PMSG_MAGICLISTCOUNT;
struct PMSG_MAPMOVE_CHECKSUM;
struct PMSG_MEMBER_POS_INFO_SEND;
struct PMSG_MONEY;
struct PMSG_MONSTER_VIEWPORTCREATE;
struct PMSG_MOVE;
struct PMSG_MOVE_PSHOPITEM;
struct PMSG_MUBOT_REQ_START;
struct PMSG_MUBOT_SETTINGS_RECV;
struct PMSG_MUBOT_USE;
struct PMSG_MUUNITEM_CONDITION_STATUS;
struct PMSG_MUUNITEM_DUR;
struct PMSG_MUUNITEM_GETREQUEST;
struct PMSG_MUUNITEM_GETRESULT;
struct PMSG_MUUN_ATTACK_COMMAND;
struct PMSG_MUUN_INVEN_ITEM_MOD;
struct PMSG_MUUN_INVEN_SUB_EQUIP_OPEN;
struct PMSG_MUUN_ITEM_SELLREQUEST;
struct PMSG_MUUN_ITEM_SELLRESULT;
struct PMSG_MUUN_RIDE_SELECT;
struct PMSG_NOTIFY_MAXUSER;
struct PMSG_NOTIFY_OBJECT_CREATION_STATE;
struct PMSG_NOTIFY_ZONE_CLEAR;
struct PMSG_NOTIFY_ZONE_TIME;
struct PMSG_OBJECT_LIFE_INFO;
struct PMSG_PACKETCHECKSUM;
struct PMSG_PARTYDELUSER;
struct PMSG_PARTYLIST;
struct PMSG_PARTYLISTCOUNT;
struct PMSG_PARTYREQUEST;
struct PMSG_PARTYREQUESTRESULT;
struct PMSG_PENTAGRAMJEWEL_INFO;
struct PMSG_PENTAGRAM_JEWEL_INOUT;
struct PMSG_PETINVENTORY_DELETE;
struct PMSG_PING_RESULT;
struct PMSG_PKLEVEL;
struct PMSG_POSISTION_SET;
struct PMSG_POST_DATA;
struct PMSG_PRICE_INFO;
struct PMSG_PRICE_ITEM_LIST;
struct PMSG_PROPERTYITEMCOUNT;
struct PMSG_PROPERTYITEMINFO;
struct PMSG_RAGEATTACK;
struct PMSG_RAGEATTACK_REQ;
struct PMSG_RAGE_ATTACK_RANGE;
struct PMSG_RAGE_ATTACK_RANGE_ANS;
struct PMSG_RANKING_SET;
struct PMSG_RAREITEMPIRCESEND;
struct PMSG_RECALLMONLIFE;
struct PMSG_RECVMOVE;
struct PMSG_RECV_CHATDATA_WHISPER;
struct PMSG_RECV_DC_OTHER_CHANNEL;
struct PMSG_RECV_POSISTION_SET;
struct PMSG_RECV_TRAP;
struct PMSG_REGEVENTCHIP;
struct PMSG_RELATIONSHIP_JOIN_BREAKOFF_ANS;
struct PMSG_RELATIONSHIP_JOIN_BREAKOFF_REQ;
struct PMSG_REQUEST_PET_ITEMINFO;
struct PMSG_REQUEST_PET_ITEM_COMMAND;
struct PMSG_REQ_2ANIV_SERIAL;
struct PMSG_REQ_2ANV_LOTTO_EVENT;
struct PMSG_REQ_AB_ALL_JOIN_USER_DS;
struct PMSG_REQ_AB_JOIN_CANCEL_DS;
struct PMSG_REQ_AB_JOIN_MEMBER_UNDER_DS;
struct PMSG_REQ_AB_MARK_REG_ALL_DEL_DS;
struct PMSG_REQ_AB_MARK_REG_UPDATE_DS;
struct PMSG_REQ_AB_PROC_INSERT_DS;
struct PMSG_REQ_AB_PROC_STATE_DS;
struct PMSG_REQ_AB_REG_MEMBER_CNT_DS;
struct PMSG_REQ_AB_WIN_GUILD_INFO_DS;
struct PMSG_REQ_AB_WIN_GUILD_INFO_INSERT_DS;
struct PMSG_REQ_ACCEPTENTER_ITR;
struct PMSG_REQ_ACCEPTMEMBER_PARTYMATCHING;
struct PMSG_REQ_ACHERON_ENTER;
struct PMSG_REQ_ACHERON_EVENT_ENTER;
struct PMSG_REQ_AE_PLAY_DS;
struct PMSG_REQ_ALLOWLIST_GUILDMATCHING;
struct PMSG_REQ_ALLOW_JOIN_GUILDMATCHING;
struct PMSG_REQ_ALL_GUILD_MARK_CNT_DS;
struct PMSG_REQ_ARCA_BATTLE_ENTER;
struct PMSG_REQ_ARCA_BATTLE_ENTER_DS;
struct PMSG_REQ_ARCA_BATTLE_GUILD_JOIN_DS;
struct PMSG_REQ_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS;
struct PMSG_REQ_ARCA_BATTLE_IS_TOP_RANK;
struct PMSG_REQ_ARCA_BATTLE_JOIN;
struct PMSG_REQ_ARCA_BATTLE_MARK_CNT_DS;
struct PMSG_REQ_ARCA_BATTLE_MARK_RANK_DS;
struct PMSG_REQ_ARCA_BATTLE_MARK_REG_DEL_DS;
struct PMSG_REQ_ARCA_BATTLE_MARK_REG_DS;
struct PMSG_REQ_BLOODCASTLE_ENTERCOUNT;
struct PMSG_REQ_BUY_PCPOINTSHOP;
struct PMSG_REQ_CANCEL_GUILDMATCHINGLIST;
struct PMSG_REQ_CANCEL_JOIN_PARTYMATCHING;
struct PMSG_REQ_CASTLESIEGESTATE;
struct PMSG_REQ_CCF_PERMISSION;
struct PMSG_REQ_CCF_RANKING;
struct PMSG_REQ_CHAOS_MULTIMIX_CHECK;
struct PMSG_REQ_CLASSDEF;
struct PMSG_REQ_CL_ENTERCOUNT;
struct PMSG_REQ_CRCDATA;
struct PMSG_REQ_CRYWOLF_ALTAR_CONTRACT;
struct PMSG_REQ_CRYWOLF_BENEFIT_PLUS_CHAOSRATE;
struct PMSG_REQ_CRYWOLF_INFO;
struct PMSG_REQ_CSATTKGUILDLIST;
struct PMSG_REQ_CSCOMMAND;
struct PMSG_REQ_CSGATEOPERATE;
struct PMSG_REQ_CSHUNTZONEENTER;
struct PMSG_REQ_CSREGGUILDLIST;
struct PMSG_REQ_DELETE_STONES;
struct PMSG_REQ_DEVILSQUARE_REMAINTIME;
struct PMSG_REQ_DSF_ACCEPTENTER;
struct PMSG_REQ_DSF_CAN_PARTY_ENTER;
struct PMSG_REQ_DSF_GET_REWARD;
struct PMSG_REQ_DSF_GO_FINAL_PARTY;
struct PMSG_REQ_DSF_GO_FINAL_PARTY_INFO;
struct PMSG_REQ_DSF_PARTYRANKRENEW;
struct PMSG_REQ_DUEL_EXIT;
struct PMSG_REQ_DUEL_INVITE;
struct PMSG_REQ_DUEL_JOINCNANNEL;
struct PMSG_REQ_DUEL_LEAVECNANNEL;
struct PMSG_REQ_END_DUEL;
struct PMSG_REQ_ENTER_DOPPELGANGER;
struct PMSG_REQ_ENTER_ITR;
struct PMSG_REQ_ENTER_KANTURU_BOSS_MAP;
struct PMSG_REQ_ENTER_ZONE;
struct PMSG_REQ_EVENTENTRY_INFO;
struct PMSG_REQ_EVOMON_MAXSCORE;
struct PMSG_REQ_GET_DSF_REWARD;
struct PMSG_REQ_GET_PARTYMATCHINGLIST;
struct PMSG_REQ_GET_REWARD;
struct PMSG_REQ_GET_UBF_REAL_NAME;
struct PMSG_REQ_GIVEUPCASTLESIEGE;
struct PMSG_REQ_GUILDMARK_OF_CASTLEOWNER;
struct PMSG_REQ_GUILDMATCHINGLIST;
struct PMSG_REQ_GUILDMATCHINGLISTSEARCHWORD;
struct PMSG_REQ_GUILDREGINFO;
struct PMSG_REQ_GUILDVIEWPORT;
struct PMSG_REQ_GUILD_PERIODBUFF_DELETE;
struct PMSG_REQ_GUILD_PERIODBUFF_INSERT;
struct PMSG_REQ_GUILD_REG_INIT;
struct PMSG_REQ_HUNTING_LOG;
struct PMSG_REQ_ILLUSIONTEMPLE_ENTERCOUNT;
struct PMSG_REQ_INVENTORY_EQUIPMENT_ITEM;
struct PMSG_REQ_IN_PENTAGRAM_JEWEL;
struct PMSG_REQ_ITL_RELATE_RELICS;
struct PMSG_REQ_JEWEL_MIX;
struct PMSG_REQ_JEWEL_UNMIX;
struct PMSG_REQ_JOIN_GUILDMATCHING;
struct PMSG_REQ_KANTURU_STATE_INFO;
struct PMSG_REQ_LUCKYCOIN;
struct PMSG_REQ_LUCKYCOIN_REGISTER;
struct PMSG_REQ_LUCKYCOIN_TRADE;
struct PMSG_REQ_LUCKYITEM_DELETE;
struct PMSG_REQ_LUCKYITEM_INSERT;
struct PMSG_REQ_LUCKYITEM_INSERT_2ND;
struct PMSG_REQ_LUCKYITEM_SELECT;
struct PMSG_REQ_MAPMOVE;
struct PMSG_REQ_MAPSERVERAUTH;
struct PMSG_REQ_MAPSVRAUTH;
struct PMSG_REQ_MAPSVRMOVE;
struct PMSG_REQ_MASTERLEVEL_SKILL;
struct PMSG_REQ_MEMBERJOIN_PARTYMATCHINGLIST;
struct PMSG_REQ_MEMBERJOIN_STATELIST_PARTYMATCHING;
struct PMSG_REQ_MINETWINKLE;
struct PMSG_REQ_MINETWINKLE_REWARD;
struct PMSG_REQ_MINIMAPDATA;
struct PMSG_REQ_MONEYDRAWOUT;
struct PMSG_REQ_MOVEBLOODCASTLE;
struct PMSG_REQ_MOVECHAOSCASTLE;
struct PMSG_REQ_MOVEDEVILSQUARE;
struct PMSG_REQ_MOVEILLUSIONTEMPLE;
struct PMSG_REQ_MOVE_OTHERSERVER;
struct PMSG_REQ_MOVE_TO_CASTLE_HUNTZONE;
struct PMSG_REQ_MURUMMY_DELETE_DS;
struct PMSG_REQ_MURUMMY_INFO_UPDATE_DS;
struct PMSG_REQ_MURUMMY_INSERT_DS;
struct PMSG_REQ_MURUMMY_LOG_INSERT_DS;
struct PMSG_REQ_MURUMMY_SCORE_UPDATE_DS;
struct PMSG_REQ_MURUMMY_SELECT_DS;
struct PMSG_REQ_MURUMMY_SLOTUPDATE_DS;
struct PMSG_REQ_MURUMMY_UPDATE_DS;
struct PMSG_REQ_MUUN_EXCHANGE;
struct PMSG_REQ_NIXIELAKE_ENTER;
struct PMSG_REQ_NPCBUY;
struct PMSG_REQ_NPCDBLIST;
struct PMSG_REQ_NPCREPAIR;
struct PMSG_REQ_NPCUPGRADE;
struct PMSG_REQ_NPC_POSITION;
struct PMSG_REQ_OPEN_PCPOINTSHOP;
struct PMSG_REQ_OUT_PENTAGRAM_JEWEL;
struct PMSG_REQ_PENTAGRAMJEWEL;
struct PMSG_REQ_PERIODBUFF_DELETE;
struct PMSG_REQ_PERIODBUFF_INSERT;
struct PMSG_REQ_PERIODBUFF_SELECT;
struct PMSG_REQ_PERIODITEMEX_DELETE;
struct PMSG_REQ_PERIODITEMEX_INSERT;
struct PMSG_REQ_PERIODITEMEX_SELECT;
struct PMSG_REQ_PSHOPITEMVALUE_INFO;
struct PMSG_REQ_QUESTEXP_INFO;
struct PMSG_REQ_REFINE_PENTAGRAM_JEWEL;
struct PMSG_REQ_REGCASTLESIEGE;
struct PMSG_REQ_REGGUILDMARK;
struct PMSG_REQ_REGGUILDMATCHINGDATA;
struct PMSG_REQ_REGISTER_EVENTCHIP;
struct PMSG_REQ_REGISTER_LUCKYCOIN;
struct PMSG_REQ_REGISTER_STONES;
struct PMSG_REQ_REG_CC_OFFLINE_GIFT;
struct PMSG_REQ_REG_DL_OFFLINE_GIFT;
struct PMSG_REQ_REG_HT_OFFLINE_GIFT;
struct PMSG_REQ_REG_RINGGIFT;
struct PMSG_REQ_REG_WANTED_PARTYMEMBER;
struct PMSG_REQ_REMOVE_ALL_GUILD_BUFF_DS;
struct PMSG_REQ_REPOSUSER_IN_CC;
struct PMSG_REQ_REPOSUSER_IN_CCF;
struct PMSG_REQ_RESET_EVENTCHIP;
struct PMSG_REQ_RUUD_STORE_BUYITEM;
struct PMSG_REQ_SANTACHECK;
struct PMSG_REQ_SANTAGIFT;
struct PMSG_REQ_SAVE_CCF_RESULT;
struct PMSG_REQ_SAVE_DSF_PARTYPOINT;
struct PMSG_REQ_SAVE_DSF_REWARD_USER;
struct PMSG_REQ_SAVE_EVOMON_RESULT;
struct PMSG_REQ_SETPENTAGRAMJEWEL;
struct PMSG_REQ_SET_DSF_WINNER_INFO;
struct PMSG_REQ_START_DUEL;
struct PMSG_REQ_STOPMINIMAPDATA;
struct PMSG_REQ_SWITCHWARE;
struct PMSG_REQ_TAXMONEYINFO;
struct PMSG_REQ_TAXRATECHANGE;
struct PMSG_REQ_UBF_ACCOUNT_USERINFO;
struct PMSG_REQ_UBF_CANCEL;
struct PMSG_REQ_UBF_GET_REWARD;
struct PMSG_REQ_UBF_INFO;
struct PMSG_REQ_UBF_JOIN;
struct PMSG_REQ_UBF_REAL_NAME;
struct PMSG_REQ_UBF_SET_RECEIVED_REWARD;
struct PMSG_REQ_UPGRADE_PENTAGRAM_JEWEL;
struct PMSG_REQ_USEILLUSIONTEMPLESKILL;
struct PMSG_REQ_USETRANSFORMATIONRING;
struct PMSG_REQ_USEWEAPON;
struct PMSG_REQ_USE_BOX;
struct PMSG_REQ_VIEW_EC_MN;
struct PMSG_REQ_WAITLIST_PARTYMATCHING;
struct PMSG_REQ_WEAPON_DAMAGE_VALUE;
struct PMSG_RESULT;
struct PMSG_RESULT_CCF_ENTERCHECK;
struct PMSG_RESULT_MOVEBLOODCASTLE;
struct PMSG_RES_EXPEVENTINFO;
struct PMSG_RSA_KEY;
struct PMSG_RUUD;
struct PMSG_SELLREQUEST;
struct PMSG_SEND_AH_INFO;
struct PMSG_SEND_CCF_INFO_ALL_SVR;
struct PMSG_SEND_EVENT_BANNER;
struct PMSG_SEND_HITHACK_INFO;
struct PMSG_SEND_MUUN_RIDE_VIEWPORT_INFO;
struct PMSG_SEND_NEWS;
struct PMSG_SEND_PCPOINT;
struct PMSG_SEND_WINDOW_CLOSE;
struct PMSG_SERVERCMD;
struct PMSG_SERVERINFO;
struct PMSG_SERVERLIST_SERVER;
struct PMSG_SERVERMSG;
struct PMSG_SERVERSLIST_COUNT;
struct PMSG_SETAGILITYBUG;
struct PMSG_SETCHARSET;
struct PMSG_SETQUEST;
struct PMSG_SET_CHAT_COLOR;
struct PMSG_SET_DEVILSQUARE;
struct PMSG_SET_OFFTRADE;
struct PMSG_SHOPITEMCOUNT;
struct PMSG_SIMPLE_GUILDVIEWPORT;
struct PMSG_SIMPLE_GUILDVIEWPORT_COUNT;
struct PMSG_SKILLKEY;
struct PMSG_STATEBLOODCASTLE;
struct PMSG_STATS_RESULT;
struct PMSG_STAT_BONUS;
struct PMSG_SUMMONER_INFO;
struct PMSG_TALKREQUEST;
struct PMSG_TALKRESULT;
struct PMSG_TARGET_HP;
struct PMSG_TARGET_HP_DELETE;
struct PMSG_TARGET_HP_INFO;
struct PMSG_TARGET_TELEPORT;
struct PMSG_TELEPORT;
struct PMSG_TELEPORT_RESULT;
struct PMSG_TIMEVIEW;
struct PMSG_TRADE_GOLD;
struct PMSG_TRADE_OKBUTTON;
struct PMSG_TRADE_REQUEST;
struct PMSG_TRADE_RESPONSE;
struct PMSG_UBF_ACCOUNT_USER_COPY;
struct PMSG_UBF_ACCOUNT_USER_COPY_PETITEM;
struct PMSG_UBF_ACCOUNT_USER_COPY_RESULT;
struct PMSG_UBF_ANS_CANCEL_REGISTER_USER;
struct PMSG_UBF_COPY_PETITEM;
struct PMSG_UBF_REGISTER_ACCOUNT_USER;
struct PMSG_UBF_REGISTER_ACCOUNT_USER_RESULT;
struct PMSG_UBF_REQ_CANCEL_REGISTER_USER;
struct PMSG_UNIONLIST_REQ;
struct PMSG_UPDATE_PSHOPITEMVALUE_INFO;
struct PMSG_USEEXPANSIONITEM;
struct PMSG_USEITEM;
struct PMSG_USEITEM_MUUN_INVEN;
struct PMSG_USEITEM_MUUN_INVEN_RESULT;
struct PMSG_USEREQUIPMENTCHANGED;
struct PMSG_USER_MUUN_EQUIPMENT_CHANGED;
struct PMSG_USE_STAT_FRUIT;
struct PMSG_VIEWPORTCREATE;
struct PMSG_VIEWPORTCREATE_CHANGE;
struct PMSG_VIEWPORTDESTROY;
struct PMSG_WAREHOUSEMONEYINOUT;
struct PMSG_WAREHOUSEPASSSEND;
struct PSHOP_ITEMVALUE_INFO_DS;
struct PWMSG_COUNT;
struct P_ADD_NEWS;
struct REQUEST_CMD;
struct Recv_PetItem_Info;
struct Request_PetItem_Info;
struct SDHP_ANS_CCF_GETPERMISSION;
struct SDHP_ANS_CCF_INFO_NOTIFY;
struct SDHP_ANS_DBEVENT_INVEN_LOAD;
struct SDHP_ANS_DBMUUN_INVEN_LOAD;
struct SDHP_ANS_LOAD_MINESYSTEM_UPT_USERINFO;
struct SDHP_ANS_MINESYSTEM_UPT_USERINFO;
struct SDHP_BILLKILLUSER;
struct SDHP_BILLSEARCH;
struct SDHP_BILLSEARCH_RESULT;
struct SDHP_CHARACTER_TRANSFER;
struct SDHP_CHARACTER_TRANSFER_RESULT;
struct SDHP_CHARDELETE;
struct SDHP_CHARDELETERESULT;
struct SDHP_CHARLIST;
struct SDHP_CHARLISTCOUNT;
struct SDHP_COMMAND_BLOCK;
struct SDHP_CREATECHAR;
struct SDHP_CREATECHARRESULT;
struct SDHP_DBCHARINFOREQUEST;
struct SDHP_DBCHAR_INFORESULT;
struct SDHP_DBCHAR_INFOSAVE;
struct SDHP_DBCHAR_ITEMSAVE;
struct SDHP_DELETE_TEMPUSERINFO;
struct SDHP_DELETE_TEMPUSERINFO_RESULT;
struct SDHP_EVENTCONTROL;
struct SDHP_EVENTSTART;
struct SDHP_EXITMSG;
struct SDHP_EXPANDEDWAREHOUSE_SET;
struct SDHP_GETCHARLIST;
struct SDHP_GETWAREHOUSEDB;
struct SDHP_GETWAREHOUSEDB_RESULT;
struct SDHP_GETWAREHOUSEDB_SAVE;
struct SDHP_GUILDALL;
struct SDHP_GUILDALL_COUNT;
struct SDHP_GUILDCREATE;
struct SDHP_GUILDCREATED;
struct SDHP_GUILDCREATE_RESULT;
struct SDHP_GUILDDESTROY;
struct SDHP_GUILDDESTROY_RESULT;
struct SDHP_GUILDLISTSTATE;
struct SDHP_GUILDMEMBERADD;
struct SDHP_GUILDMEMBERADD_RESULT;
struct SDHP_GUILDMEMBERADD_RESULT_WITHOUT_USERINDEX;
struct SDHP_GUILDMEMBERADD_WITHOUT_USERINDEX;
struct SDHP_GUILDMEMBERDEL;
struct SDHP_GUILDMEMBERDEL_RESULT;
struct SDHP_GUILDMEMBER_INFO;
struct SDHP_GUILDMEMBER_INFO_GUILDNAME_REQUEST;
struct SDHP_GUILDMEMBER_INFO_REQUEST;
struct SDHP_GUILDNOTICE;
struct SDHP_GUILDSCOREUPDATE;
struct SDHP_GUILDUPDATE;
struct SDHP_IDPASS;
struct SDHP_IDPASSRESULT;
struct SDHP_ITEMCREATE;
struct SDHP_ITEMCREATERECV;
struct SDHP_JOINFAIL;
struct SDHP_LOVEHEARTCREATE;
struct SDHP_LOVEHEARTEVENT;
struct SDHP_LOVEHEARTEVENT_RESULT;
struct SDHP_NOTICE;
struct SDHP_OPTIONCONTROL;
struct SDHP_OTHERJOINMSG;
struct SDHP_RECV_PETITEM_INFO;
struct SDHP_RENEW_RANKING;
struct SDHP_REQUEST_PETITEM_INFO;
struct SDHP_REQ_CCF_RANKING;
struct SDHP_REQ_DBEVENT_INVEN_LOAD;
struct SDHP_REQ_DBEVENT_INVEN_SAVE;
struct SDHP_REQ_DBMUUN_INVEN_LOAD;
struct SDHP_REQ_DBMUUN_INVEN_SAVE;
struct SDHP_REQ_LOAD_MINESYSTEM_UPT_USERINFO;
struct SDHP_REQ_MINESYSTEM_UPT_USERINFO;
struct SDHP_REQ_SET_CCF_WINNER_INFO;
struct SDHP_REQ_SET_CC_WINNER_INFO_UBF;
struct SDHP_REQ_SET_EXGAMESERVERCODE;
struct SDHP_SAVE_PETITEM_INFO;
struct SDHP_SKILLKEYDATA;
struct SDHP_SKILLKEYDATA_SEND;
struct SDHP_USERCLOSE;
struct SDHP_USERCLOSE_ID;
struct SDHP_USER_NOTICE;
struct SDHP_VIPINFORECV;
struct SDHP_VIPINFORESULT;
struct SDHP_WAREHOUSEMONEY_SAVE;
struct SECLOCK_REQ_SAVE;
struct STAT_BONUS;
struct Save_PetItem_Info;
struct ServerInfo;
struct ServerList;
struct UNION_MEMBER_DATA;
struct USERWAREHOUSE_DATA;
struct USER_CONNECT_DATA;
struct _GUILD_MEMBER;
struct _ITL_AnswerEnter;
struct _ITR_AcceptEnter;
struct _PARTY_INFO_LIST;
struct _PARTY_INFO_LISTDB;
struct _PARTY_MEMBER_WAIT_LIST;
struct _PMSG_QUESTEXP_INFO;
struct _QUESTEXP_INFO;
struct _SEND_PARTYMEMBER_INFO;
struct __ITL_GUILD_RANK;
struct _stABAllGuildMark;
struct _stABJoinUserInfoDS;
struct _stABWinGuildInfoDS;
struct _stAnsAddPartyMember;
struct _stAnsAddRealPartyMember;
struct _stAnsAllowJoinGuildMatching;
struct _stAnsCancelPartyMatching;
struct _stAnsChattingPartyMatching;
struct _stAnsDelGuildMatchingList;
struct _stAnsDelPartyUserPartyMatching;
struct _stAnsDelWaitGuildMatchingList;
struct _stAnsGetPartyMatchingList;
struct _stAnsGuildMatchingData;
struct _stAnsGuildMatchingList;
struct _stAnsJoinMemberPartyMatching;
struct _stAnsJoinMemberStateListPartyMatching;
struct _stAnsNotiGuildMatching;
struct _stAnsNotiGuildMatchingForGuildMaster;
struct _stAnsRegWantedPartyMember;
struct _stAnsRequestJoinPartyMatchingNoti;
struct _stAnsSendPartyMemberList;
struct _stAnsUseGuildMatchingGuild;
struct _stAnsWaitGuildMatching;
struct _stAnsWaitGuildMatchingList;
struct _stAnsWaitListPartyMatching;
struct _stAnsWaitStateListGuildMatching;
struct _stArcaBattleMarkTopRankDS;
struct _stCCFRankingInfo;
struct _stCancelGuildNames;
struct _stGuildMatchingAllowList;
struct _stGuildMatchingAllowListDB;
struct _stGuildMatchingList;
struct _stGuildUnderMember;
struct _stRegWaitGuildMatching;
struct _stReqAddPartyMember;
struct _stReqAllowJoinGuildMatching;
struct _stReqCancelPartyMatching;
struct _stReqChattingPartyMatching;
struct _stReqDelGuildMatchingList;
struct _stReqDelPartyUserPartyMatching;
struct _stReqDelWaitGuildMatchingList;
struct _stReqGetPartyMatchingList;
struct _stReqGuildMatchingData;
struct _stReqGuildMatchingList;
struct _stReqGuildMatchingListSearchWord;
struct _stReqJoinMemberPartyMatching;
struct _stReqJoinMemberStateListPartyMatching;
struct _stReqRegWantedPartyMember;
struct _stReqSendPartyMemberList;
struct _stReqWaitGuildMatchingList;
struct _stReqWaitListPartyMatching;
struct _stReqWaitStateListGuildMatching;
struct _tagITL_REWARDLIST;
struct _tagMuRummyCardInfoDS;
struct _tagMuRummyCardUpdateDS;
struct _tagMuunRideViewPortInfo;
struct buyItemTransaction;
struct itemBranch;
struct sellItem;
struct sellItemPrice;
struct sellPackage;
struct stMemberPosInfo;
struct STR_STRINGCOMPARE;

typedef struct STR_STRINGCOMPARE
{
	bool operator()(const std::string s1, const std::string s2) const
	{
		return strcmp(s1.data(), s2.data()) < 0;
	}
} strCmp;

typedef map<string, GUILD_MEMBER*, strCmp> MAP_GUILD_MEMBER;
typedef map<std::string, GUILD_INFO_STRUCT*, strCmp> MAP_GUILD_INFO;
typedef map<int, UNION_MEMBER_DATA*> MAP_MEMBER_DATA;

struct _ITL_GUILD_RANK {
	char szGuildName[MAX_GUILD_LEN + 1];
	BYTE byRank;
	BYTE byITLType;
	BYTE byWin;
	BYTE byLose;
};

typedef struct _GUILD_MEMBER {
	char m_szMemberName[11];
	BYTE m_btStatus;
	BYTE m_btConnected;
	int m_iUserIndex;
	_GUILD_MEMBER() {
		ZeroMemory(m_szMemberName, sizeof(m_szMemberName));
		m_btStatus = 0;
		m_btConnected = -1;
		m_iUserIndex = -1;
	}
} GUILD_MEMBER, *LPGUILD_MEMBER;

struct _ITL_AnswerEnter {
	PBMSG_HEAD2 h;
	BYTE bReason;
	char Name[MAX_ACCOUNT_LEN + 1];
};

struct PARTY_INFO_LIST {
	char szLeaderName[MAX_ACCOUNT_LEN + 1];
	char szTitle[41];
	WORD nMinLevel;
	WORD nMaxLevel;
	WORD nHuntingGround;
	int nLeaderLevel;
	BYTE btLeaderClass;
	BYTE btWantedClass;
	BYTE btCurPartyMemberCnt;
	BYTE btUsePassword;
	BYTE btWantedClassDetailInfo[7];
	int nServerChannel;
	BYTE btGensType;
};

struct PARTY_INFO_LISTDB {
	char szLeaderName[MAX_ACCOUNT_LEN + 1];
	char szTitle[41];
	char szPassword[5];
	WORD nMinLevel;
	WORD nMaxLevel;
	WORD nHuntingGround;
	int nLeaderLevel;
	BYTE btLeaderClass;
	BYTE btWantedClass;
	BYTE btCurPartyMemberCnt;
	BYTE btUsePassword;
	BYTE btWantedClassDetailInfo[7];
	BYTE btGensType;
	BYTE btApprovalType;
};

struct _PARTY_MEMBER_WAIT_LIST {
	char szName[MAX_ACCOUNT_LEN + 1];
	BYTE btClass;
	int nLevel;
};

struct _PMSG_QUESTEXP_INFO {
	PWMSG_HEAD h;
	BYTE btQuestCnt;
	char szCharName[MAX_ACCOUNT_LEN + 1];
};

struct _QUESTEXP_INFO {
	_QUESTEXP_INFO::_QUESTEXP_INFO() {
		this->dwQuestIndexID = 0;
		this->wProgState = 0;
		this->lStartDate = 0;
		this->lEndDate = 0;
		memset(this->btAskIndex, 0, 5);
		memset(this->btAskValue, 0, 5);
		memset(this->btAskState, 0, 5);
	}
	DWORD dwQuestIndexID;
	WORD wProgState;
	BYTE btAskIndex[5];
	BYTE btAskValue[5];
	BYTE btAskState[5];
	time_t lStartDate;
	time_t lEndDate;
};

struct _SEND_PARTYMEMBER_INFO {
	_SEND_PARTYMEMBER_INFO() {
		this->bUse = FALSE;
	}
	int bUse;
	int bFlag;
	char Name[MAX_ACCOUNT_LEN + 1];
	int nUserIndex;
	int nServerChannel;
};

struct _stABAllGuildMark {
	char szGuildName[MAX_GUILD_LEN + 1];
	DWORD dwMarkCnt;
};

struct _stABJoinUserInfoDS {
	char szGuildName[MAX_GUILD_LEN + 1];
	DWORD dwGuild;
	char szUserName[MAX_ACCOUNT_LEN + 1];
};

struct _stABWinGuildInfoDS {
	char szGuildName[9];
	DWORD dwGuild;
	WORD wOccupyObelisk;
	WORD wObeliskGroup;
};

struct _stAnsAddPartyMember {
	PBMSG_HEAD2 h;
	int nResult;
	int nUserIndex;
	int nMemberIndex;
	BYTE btType;
	BYTE btSendType;
	BYTE btManualJoin;
	char szMemberName[MAX_ACCOUNT_LEN + 1];
};

struct _stAnsAddRealPartyMember {
	PBMSG_HEAD2 h;
	int nLeaderUserIndex;
	int nMemberUserIndex;
};

struct _stAnsAllowJoinGuildMatching {
	PBMSG_HEAD2 h;
	int nUserIndex;
	int nAllowType;
	char szMemberName[MAX_ACCOUNT_LEN + 1];
	char szGuildName[MAX_GUILD_LEN + 1];
	int nResult;
};

struct _stAnsCancelPartyMatching {
	PBMSG_HEAD2 h;
	BYTE btType;
	int nUserIndex;
	int nResult;
};

struct _stAnsChattingPartyMatching {
	PBMSG_HEAD2 h;
	int nRecvUserIndex;
	int nPartyIndex;
	char szChat[63];
	char szSendCharName[MAX_ACCOUNT_LEN + 1];
};

struct _stAnsDelGuildMatchingList {
	PBMSG_HEAD2 h;
	int nUserIndex;
	int nResult;
};

struct _stAnsDelPartyUserPartyMatching {
	PBMSG_HEAD2 h;
	int nUserIndex;
	int nTargetIndex;
	int nResult;
	BYTE btType;
};

struct _stAnsDelWaitGuildMatchingList {
	PBMSG_HEAD2 h;
	int nUserIndex;
	int nType;
	int nResult;
};

struct _stAnsGetPartyMatchingList {
	PWMSG_HEAD2 h;
	int nUserIndex;
	int nListCount;
	int nPage;
	int nTotalPage;
	int nResult;
	PARTY_INFO_LIST stPartyInfoList[6];
};

struct _stAnsGuildMatchingData {
	PBMSG_HEAD2 h;
	int nUserIndex;
	int nResult;
};

struct _stAnsGuildMatchingList {
	PWMSG_HEAD2 h;
	int nUserIndex;
	int nPage;
	int nTotalPage;
	int nListCount;
	int nResult;
	_stGuildMatchingList stGuildMatchingList[9];
};

struct _stAnsJoinMemberPartyMatching {
	PBMSG_HEAD2 h;
	int nUserIndex;
	int nResult;
};

struct _stAnsJoinMemberStateListPartyMatching {
	PBMSG_HEAD2 h;
	int nUserIndex;
	int nResult;
	BYTE btLeaderChannel;
	char szLeaderName[MAX_ACCOUNT_LEN + 1];
};

struct _stAnsNotiGuildMatching {
	PBMSG_HEAD2 h;
	int nUserIndex;
	int nResult;
};

struct _stAnsNotiGuildMatchingForGuildMaster {
	PBMSG_HEAD2 h;
	int nUserIndex;
	int nResult;
};

struct _stAnsRegWantedPartyMember {
	PBMSG_HEAD2 h;
	int nUserIndex;
	int nResult;
};

struct _stAnsRequestJoinPartyMatchingNoti {
	PBMSG_HEAD2 h;
	int nUserIndex;
};

struct _stAnsSendPartyMemberList {
	PBMSG_HEAD2 h;
	int nUserIndex;
	int nMemberCount;
	int nPartyMatchingIndex;
	_SEND_PARTYMEMBER_INFO stList[5];
};

struct _stAnsUseGuildMatchingGuild {
	PBMSG_HEAD2 h;
	int nUserIndex;
};

struct _stAnsWaitGuildMatching {
	PBMSG_HEAD2 h;
	int nUserIndex;
	int nResult;
};

struct _stAnsWaitGuildMatchingList {
	PWMSG_HEAD2 h;
	int nUserIndex;
	int nListCount;
	int nResult;
	_stGuildMatchingAllowListDB stAllowList[80];
};

struct _stAnsWaitListPartyMatching {
	PWMSG_HEAD2 h;
	int nUserIndex;
	int nListCount;
	int nResult;
	_PARTY_MEMBER_WAIT_LIST stList[10];
};

struct _stAnsWaitStateListGuildMatching {
	PBMSG_HEAD2 h;
	char szGuildMasterName[MAX_ACCOUNT_LEN + 1];
	char szGuildName[MAX_GUILD_LEN + 1];
	int nUserIndex;
	int nResult;
};

struct _stArcaBattleMarkTopRankDS {
	BYTE btRank;
	char szGuildNames[MAX_GUILD_LEN + 1];
	DWORD dwMarkCnt;
};

struct _stCancelGuildNames {
	char szGuildNames[9];
};

struct _stCCFRankingInfo {
	_stCCFRankingInfo() {
		memset(this->szCharName, 0x00, sizeof(this->szCharName));
		this->byRank = 0;
		this->nPoint = 0;
	}
	char szCharName[MAX_ACCOUNT_LEN + 1];
	BYTE byRank;
	int nPoint;
};

struct _stGuildMatchingAllowList {
	char szName[MAX_ACCOUNT_LEN + 1];
	BYTE btClass;
	int nLevel;
};

struct _stGuildMatchingAllowListDB {
	int nGuildNumber;
	char szApplicantName[MAX_ACCOUNT_LEN + 1];
	BYTE btApplicantClass;
	int nApplicantLevel;
	BYTE btState;
};

struct _stGuildMatchingList {
	char szMemo[41];
	char szRegistrant[MAX_ACCOUNT_LEN + 1];
	char szGuildName[MAX_GUILD_LEN + 1];
	BYTE btGuildMemberCnt;
	BYTE btGuildMasterClass;
	BYTE btInterestType;
	BYTE btLevelRange;
	BYTE btClassType;
	int nGuildMasterLevel;
	int nBoardNumber;
	int nGuildNumber;
	BYTE btGensType;
};

struct _stGuildUnderMember {
	BYTE btGuildMemberCnt;
	char szGuildNames[9];
};

struct _stRegWaitGuildMatching {
	PBMSG_HEAD2 h;
	int nUserIndex;
	_stGuildMatchingAllowListDB stAllowList;
};

struct _stReqAddPartyMember {
	PBMSG_HEAD2 h;
	int nUserIndex;
	int nMemberIndex;
	BYTE btType;
	BYTE btManualJoin;
	BYTE btAlreadyParty;
	char szLeaderName[MAX_ACCOUNT_LEN + 1];
	char szMemberName[MAX_ACCOUNT_LEN + 1];
};

struct _stReqAllowJoinGuildMatching {
	PBMSG_HEAD2 h;
	int nUserIndex;
	int nAllowType;
	char szName[MAX_ACCOUNT_LEN + 1];
	char szGuildName[MAX_GUILD_LEN + 1];
};

struct _stReqCancelPartyMatching {
	PBMSG_HEAD2 h;
	BYTE btType;
	char szName[MAX_ACCOUNT_LEN + 1];
	int nUserIndex;
};

struct _stReqChattingPartyMatching {
	PBMSG_HEAD2 h;
	int nPartyIndex;
	char szChat[63];
	char szSendCharName[MAX_ACCOUNT_LEN + 1];
};

struct _stReqDelGuildMatchingList {
	PBMSG_HEAD2 h;
	int nUserIndex;
	int nGuildNumber;
};

struct _stReqDelPartyUserPartyMatching {
	PBMSG_HEAD2 h;
	char szTargetName[MAX_ACCOUNT_LEN + 1];
	int nUserIndex;
};

struct _stReqDelWaitGuildMatchingList {
	PBMSG_HEAD2 h;
	int nUserIndex;
	int nType;
	char szName[MAX_ACCOUNT_LEN + 1];
};

struct _stReqGetPartyMatchingList {
	PBMSG_HEAD2 h;
	int nUserIndex;
	int nPage;
	int nType;
	int nLevel;
	BYTE btClass;
	BYTE btGens;
	char szSearchWord[11];
};

struct _stReqGuildMatchingData {
	PBMSG_HEAD2 h;
	int nUserIndex;
	_stGuildMatchingList _stGuildMatchingList;
};

struct _stReqGuildMatchingList {
	PBMSG_HEAD2 h;
	int nUserIndex;
	int nPage;
};

struct _stReqGuildMatchingListSearchWord {
	PBMSG_HEAD2 h;
	int nUserIndex;
	int nPage;
	char szSearchWord[11];
};

struct _stReqJoinMemberPartyMatching {
	PBMSG_HEAD2 h;
	char szMemberName[MAX_ACCOUNT_LEN + 1];
	char szLeaderName[MAX_ACCOUNT_LEN + 1];
	char szPassword[5];
	int nUserIndex;
	int nUserDBNumber;
	int nLevel;
	BYTE btClass;
	BYTE btRandomParty;
	BYTE btGensType;
	BYTE btChangeUpClass;
};

struct _stReqJoinMemberStateListPartyMatching {
	PBMSG_HEAD2 h;
	int nUserIndex;
	char szMemberName[MAX_ACCOUNT_LEN + 1];
};

struct _stReqRegWantedPartyMember {
	PBMSG_HEAD2 h;
	int nUserIndex;
	char szPartyLeaderName[MAX_ACCOUNT_LEN + 1];
	char szTitle[41];
	char szPassword[5];
	WORD nMinLevel;
	WORD nMaxLevel;
	WORD nHuntingGround;
	WORD nLeaderLevel;
	BYTE btWantedClass;
	BYTE btUsePassWord;
	BYTE btCurrentPartyCnt;
	BYTE btApprovalType;
	BYTE btWantedClassDetailInfo[7];
	BYTE btGensType;
	BYTE btLeaderClass;
};

struct _stReqSendPartyMemberList {
	PBMSG_HEAD2 h;
	char szLeaderName[MAX_ACCOUNT_LEN + 1];
};

struct _stReqWaitGuildMatchingList {
	PBMSG_HEAD2 h;
	int nUserIndex;
	int nGuildNumber;
};

struct _stReqWaitListPartyMatching {
	PBMSG_HEAD2 h;
	int nUserIndex;
	char szLeaderName[MAX_ACCOUNT_LEN + 1];
};

struct _stReqWaitStateListGuildMatching {
	PBMSG_HEAD2 h;
	int nUserIndex;
	char szName[MAX_ACCOUNT_LEN + 1];
};

struct _tagITL_REWARDLIST {
	BYTE byRank;
	BYTE byEnteredCnt;
	BYTE byPoint;
	bool bGotReward;
	char szCharName[MAX_ACCOUNT_LEN + 1];
	char szGuildName[MAX_GUILD_LEN + 1];
};

struct _tagMuRummyCardInfoDS {
	_tagMuRummyCardInfoDS() {
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

struct _tagMuRummyCardUpdateDS {
	_tagMuRummyCardUpdateDS() {
		this->btSlotNum = -1;
		this->btSeq = -1;
		this->btStatus = -1;
	}
	BYTE btSlotNum;
	BYTE btSeq;
	BYTE btStatus;
};

struct _tagMuunRideViewPortInfo {
	BYTE NumberH;
	BYTE NumberL;
	BYTE MuunRideItemH;
	BYTE MuunRideItemL;
};

struct PMSG_ANS_AB_ALL_JOIN_USER_DS {
	PWMSG_HEAD h;
	BYTE btUserCnt;
	_stABJoinUserInfoDS stABJoinUserInfo[200];
};

struct PMSG_ANS_AB_JOIN_CANCEL_DS {
	PBMSG_HEAD2 h;
	BYTE btGuildCnt;
	_stCancelGuildNames CancelGuildNames[6];
};

struct PMSG_ANS_AB_JOIN_MEMBER_UNDER_DS {
	PBMSG_HEAD2 h;
	BYTE btGuildCnt;
	_stGuildUnderMember GuildMemberCnt[6];
};

struct PMSG_ANS_AB_PROC_STATE_DS {
	PBMSG_HEAD2 h;
	BYTE btProcState;
};

struct PMSG_ANS_AB_REG_MEMBER_CNT_DS {
	PBMSG_HEAD2 h;
	WORD wNumber;
	BYTE btRegMemCnt;
};

struct PMSG_ANS_AB_WIN_GUILD_INFO_DS {
	PBMSG_HEAD2 h;
	char btGuildCnt;
	_stABWinGuildInfoDS m_stABWinGuildInfoDS[5];
};

struct PMSG_ANS_ACHERON_ENTER {
	PBMSG_HEAD2 h;
	char btResult;
};

struct PMSG_ANS_ACHERON_EVENT_ENTER {
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_ANS_AE_PLAY_DS {
	PBMSG_HEAD2 h;
	BYTE btPlay;
};

struct PMSG_ANS_ALL_GUILD_MARK_CNT_DS {
	PWMSG_HEAD2 h;
	WORD wGuildCnt;
	_stABAllGuildMark ABAllGuildMark[250];
};

struct PMSG_ANS_ARCA_BATTLE_ENTER_DS {
	PBMSG_HEAD2 h;
	char btResult;
	BYTE btEnterSeq;
	WORD wNumber;
};

struct PMSG_ANS_ARCA_BATTLE_GUILD_JOIN_DS {
	PBMSG_HEAD2 h;
	char btResult;
	WORD wNumber;
};

struct PMSG_ANS_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS {
	PBMSG_HEAD2 h;
	char btResult;
	WORD wNumber;
};

struct PMSG_ANS_ARCA_BATTLE_MARK_CNT_DS {
	PBMSG_HEAD2 h;
	BYTE btResult;
	WORD wNumber;
	DWORD dwMarkCnt;
};

struct PMSG_ANS_ARCA_BATTLE_MARK_RANK_DS {
	PBMSG_HEAD2 h;
	WORD wNumber;
	BYTE btRank;
	DWORD dwMarkCnt;
	BYTE btGuildCnt;
	_stArcaBattleMarkTopRankDS ArcaBattleMarkTopRank[6];
};

struct PMSG_ANS_ARCA_BATTLE_MARK_REG_DS {
	PBMSG_HEAD2 h;
	WORD wNumber;
};

struct PMSG_ANS_CCF_RANK {
	PWMSG_HEAD h;
	BYTE byUserCnt;
	_stCCFRankingInfo RankingInfo[50];
};

struct PMSG_ANS_HUNTING_LOG {
	PBMSG_HEAD2 h;
	WORD unk1;
	WORD unk2;
	WORD unk3;
	WORD unk4;
};

struct PMSG_ANS_INVENTORY_EQUIPMENT_ITEM {
	PBMSG_HEAD2 h;
	BYTE btItemPos;
	BYTE btResult;
};

struct PMSG_ANS_MURUMMY_SELECT_DS {
	PBMSG_HEAD2 h;
	WORD aIndex;
	WORD wScore;
	BYTE btResult;
	_tagMuRummyCardInfoDS stMuRummyCardInfoDS[24];
};

struct PMSG_ANS_NIXIELAKE_ENTER {
	PBMSG_HEAD2 h;
	char btResult;
};

struct PMSG_ANS_REMOVE_ALL_GUILD_BUFF_DS {
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_AB_ALL_JOIN_USER_DS {
	PBMSG_HEAD h;
};

struct PMSG_REQ_AB_JOIN_CANCEL_DS {
	PBMSG_HEAD2 h;
	BYTE btMinGuildMemNum;
	WORD wMapSvrNum;
};

struct PMSG_REQ_AB_JOIN_MEMBER_UNDER_DS {
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
};

struct PMSG_REQ_AB_MARK_REG_ALL_DEL_DS {
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_AB_MARK_REG_UPDATE_DS {
	PBMSG_HEAD2 h;
	DWORD dwGuildNum;
	DWORD dwMarkCnt;
};

struct PMSG_REQ_AB_PROC_INSERT_DS {
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
	BYTE btProcState;
};

struct PMSG_REQ_AB_PROC_STATE_DS {
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_AB_REG_MEMBER_CNT_DS {
	PBMSG_HEAD2 h;
	int iIndex;
	int iGuildNumber;
};

struct PMSG_REQ_AB_WIN_GUILD_INFO_DS {
	PBMSG_HEAD2 h;
	WORD wNumber;
};

struct PMSG_REQ_AB_WIN_GUILD_INFO_INSERT_DS {
	PBMSG_HEAD2 h;
	char btGuildCnt;
	unsigned __int16 wMapSvrNum;
	_stABWinGuildInfoDS m_stABWinGuildInfoDS[5];
};

struct PMSG_REQ_ACHERON_ENTER {
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_ACHERON_EVENT_ENTER {
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_AE_PLAY_DS {
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
	BYTE btPlay;
};

struct PMSG_REQ_ALL_GUILD_MARK_CNT_DS {
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_ARCA_BATTLE_ENTER_DS {
	PBMSG_HEAD2 h;
	char szCharName[11];
	BYTE btEnterSeq;
	WORD wNumber;
};

struct PMSG_REQ_ARCA_BATTLE_ENTER {
	PBMSG_HEAD2 h;
	char btEnterSeq;
};

struct PMSG_REQ_ARCA_BATTLE_GUILD_JOIN_DS {
	PBMSG_HEAD2 h;
	char szGuildMaster[11];
	char szGuildName[9];
	DWORD dwGuild;
	WORD wNumber;
};

struct PMSG_REQ_ARCA_BATTLE_GUILD_MEMBER_JOIN_DS {
	PBMSG_HEAD2 h;
	char szCharName[11];
	char szGuildName[9];
	unsigned int dwGuild;
	WORD wNumber;
};

struct PMSG_REQ_ARCA_BATTLE_IS_TOP_RANK {
	PBMSG_HEAD2 h;
	WORD wNumber;
	DWORD dwGuildNum;
};

struct PMSG_REQ_ARCA_BATTLE_MARK_CNT_DS {
	PBMSG_HEAD2 h;
	WORD wNumber;
	DWORD dwGuildNum;
};

struct PMSG_REQ_ARCA_BATTLE_MARK_RANK_DS {
	PBMSG_HEAD2 h;
	WORD wNumber;
	DWORD dwGuildNum;
};

struct PMSG_REQ_ARCA_BATTLE_MARK_REG_DEL_DS {
	PBMSG_HEAD2 h;
	WORD wNumber;
	DWORD dwGuildNum;
};

struct PMSG_REQ_ARCA_BATTLE_MARK_REG_DS {
	PBMSG_HEAD2 h;
	WORD wNumber;
	char szGuildMaster[MAX_ACCOUNT_LEN + 1];
	char szGuildName[MAX_GUILD_LEN + 1];
	DWORD dwMarkCnt;
	DWORD dwGuildNum;
};

struct PMSG_REQ_CCF_PERMISSION {
	PBMSG_HEAD2 h;
	char szCharName[MAX_ACCOUNT_LEN + 1];
	BYTE byCCFType;
	int nIndex;
};

struct PMSG_REQ_GUILD_REG_INIT {
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_HUNTING_LOG {
	PBMSG_HEAD2 h;
	WORD unk1;
	WORD unk2;
	WORD unk3;
	WORD unk4;
};

struct PMSG_REQ_INVENTORY_EQUIPMENT_ITEM {
	PBMSG_HEAD2 h;
	BYTE btItemPos;
	BYTE btValue;
};

struct PMSG_REQ_MURUMMY_DELETE_DS {
	PBMSG_HEAD2 h;
	char AccountID[11];
	char Name[11];
};

struct PMSG_REQ_MURUMMY_INFO_UPDATE_DS {
	PBMSG_HEAD2 h;
	char AccountID[11];
	char Name[11];
	WORD wScore;
	_tagMuRummyCardUpdateDS stMuRummyCardUpdateDS[24];
};

struct PMSG_REQ_MURUMMY_INSERT_DS {
	PBMSG_HEAD2 h;
	char AccountID[11];
	char Name[11];
	WORD aIndex;
	_tagMuRummyCardInfoDS stMuRummyCardInfoDS[24];
};

struct PMSG_REQ_MURUMMY_LOG_INSERT_DS {
	PBMSG_HEAD2 h;
	char AccountID[11];
	char Name[11];
	WORD wScore;
};

struct PMSG_REQ_MURUMMY_SCORE_UPDATE_DS {
	PBMSG_HEAD2 h;
	char AccountID[11];
	char Name[11];
	WORD wScore;
	_tagMuRummyCardUpdateDS stCardUpdateDS[3];
};

struct PMSG_REQ_MURUMMY_SELECT_DS {
	PBMSG_HEAD2 h;
	char AccountID[11];
	char Name[11];
	WORD aIndex;
};

struct PMSG_REQ_MURUMMY_SLOTUPDATE_DS {
	PBMSG_HEAD2 h;
	char AccountID[11];
	char Name[11];
	_tagMuRummyCardUpdateDS stCardUpdateDS;
};

struct PMSG_REQ_MURUMMY_UPDATE_DS {
	PBMSG_HEAD2 h;
	char AccountID[11];
	char Name[11];
	BYTE btSlotNum;
	BYTE btStatus;
	BYTE btSequence;
};

struct PMSG_REQ_NIXIELAKE_ENTER {
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_REMOVE_ALL_GUILD_BUFF_DS {
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
};

struct PMSG_REQ_SAVE_CCF_RESULT {
	PBMSG_HEAD2 h;
	char szCharName[MAX_ACCOUNT_LEN + 1];
	int nPoint;
	int nCharClass;
	int nCharLevel;
	UINT64 nCharExp;
	BYTE byCCFType;
};

struct PMSG_SEND_WINDOW_CLOSE {
	PBMSG_HEAD2 h;
};

struct SDHP_ANS_DBEVENT_INVEN_LOAD {
	PWMSG_HEAD h;
	BYTE dbItems[1024];
	WORD aIndex;
};

struct SDHP_ANS_DBMUUN_INVEN_LOAD {
	PWMSG_HEAD h;
	BYTE dbItems[1984];
	char SubEquip;
	WORD aIndex;
};

struct SDHP_REQ_DBEVENT_INVEN_LOAD {
	PBMSG_HEAD h;
	char AccountID[11];
	char Name[11];
	WORD aIndex;
};

struct SDHP_REQ_DBEVENT_INVEN_SAVE {
	PWMSG_HEAD h;
	char AccountID[11];
	char Name[11];
	BYTE dbInventory[1024];
};

struct SDHP_REQ_DBMUUN_INVEN_SAVE {
	PWMSG_HEAD h;
	char AccountID[11];
	char Name[11];
	BYTE dbInventory[1984];
};

struct BAN_REQ_USER {
	PBMSG_HEAD h;
	BYTE Type;
	BYTE Ban;
	char AccName[11];
};

struct BLOCKCHAT_DATA {
	BYTE btBlockEnable;
	BYTE btIndex;
	char szName[MAX_ACCOUNT_LEN + 1];
};

struct buyItemTransaction {
	DWORD dwTransactionGuid;
	DWORD dwPriceGuid;
	DWORD dwResult;
};

struct CHAT_LOG_DATA {
	PBMSG_HEAD h;
	WORD wServer;
	char AccountID[11];
	char Name[11];
	BYTE btType;
	char szChatMsg[90];
	char szTargetName[5][11];
};

struct CLASSDEF_DATA {
	int Class;
	int Str;
	int Dex;
	int Vit;
	int Energy;
	float Life;
	float Mana;
	float LevelLife;
	float LevelMana;
	float VitalityToLife;
	float EnergyToMana;
	int Leadership;
};

struct CONNECT_OBJECT {
	DWORD lPacketTime;
	int COUNT;
	char ip[16];
};

struct CSP_ANS_ALLGUILDREGINFO {
	struct PWMSG_HEAD h;
	int iResult;
	unsigned short wMapSvrNum;
	int iIndex;
	int iCount;
};

struct CSP_ANS_CALCREGGUILDLIST {
	PWMSG_HEAD h;
	int iResult;
	unsigned short wMapSvrNum;
	int iCount;
};

struct CSP_ANS_CASTLEOWNERCHANGE {
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
	int bIsCastleOccupied;
	char szOwnerGuildName[0x8];
};

struct CSP_ANS_CASTLESIEGEEND {
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
};

struct CSP_ANS_CASTLESTATESYNC {
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
	int iCastleState;
	int iTaxRateChaos;
	int iTaxRateStore;
	int iTaxHuntZone;
	char szOwnerGuildName[8];
};

struct CSP_ANS_CASTLETRIBUTEMONEY {
	PBMSG_HEAD2 h;
	int iResult;
	WORD wMapSvrNum;
};

struct CSP_ANS_CSGUILDUNIONINFO {
	PWMSG_HEAD h;
	int iResult;
	unsigned short wMapSvrNum;
	int iCount;
};

struct CSP_ANS_CSINITDATA {
	struct PWMSG_HEAD h;
	int iResult;
	unsigned short wMapSvrNum;
	unsigned short wStartYear;
	unsigned char btStartMonth;
	unsigned char btStartDay;
	unsigned short wEndYear;
	unsigned char btEndMonth;
	unsigned char btEndDay;
	unsigned char btIsSiegeGuildList;
	unsigned char btIsSiegeEnded;
	unsigned char btIsCastleOccupied;
	char szCastleOwnGuild[0x8];
	__int64 i64CastleMoney;
	int iTaxRateChaos;
	int iTaxRateStore;
	int iTaxHuntZone;
	int iFirstCreate;
	int iCount;
};

struct CSP_ANS_CSLOADTOTALGUILDINFO {
	PWMSG_HEAD h;
	int iResult;
	unsigned short wMapSvrNum;
	int iCount;
};

struct CSP_ANS_CSSAVETOTALGUILDINFO {
	PBMSG_HEAD h;
	int iResult;
	unsigned short wMapSvrNum;
};

struct CSP_ANS_GUILDREGINFO {
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
	int iIndex;
	char szGuildName[0x8];
	int iRegMarkCount;
	unsigned char bIsGiveUp;
	unsigned char btRegRank;
};

struct CSP_ANS_GUILDREGMARK {
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
	int iIndex;
	char szGuildName[0x8];
	int iItemPos;
	int iRegMarkCount;
};

struct CSP_ANS_GUILDRESETMARK {
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
	int iIndex;
	char szGuildName[0x8];
	int iRegMarkCount;
};

struct CSP_ANS_GUILDSETGIVEUP {
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
	int iIndex;
	char szGuildName[0x8];
	int bIsGiveUp;
	int iRegMarkCount;
};

struct CSP_ANS_MAPSVRMULTICAST {
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
	char szMsgText[128];
};

struct CSP_ANS_MONEYCHANGE {
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
	int iIndex;
	int iMoneyChanged;
	__int64 i64CastleMoney;
};

struct CSP_ANS_NPCBUY {
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
	int iIndex;
	int iNpcNumber;
	int iNpcIndex;
	int iBuyCost;
};

struct CSP_ANS_NPCDATA {
	PWMSG_HEAD h;
	int iResult;
	WORD wMapSvrNum;
	int iIndex;
	int iCount;
};

struct CSP_ANS_NPCREMOVE {
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
	int iNpcNumber;
	int iNpcIndex;
};

struct CSP_ANS_NPCREPAIR {
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
	int iIndex;
	int iNpcNumber;
	int iNpcIndex;
	int iNpcMaxHp;
	int iNpcHpl;
	int iRepairCost;
};

struct CSP_ANS_NPCSAVEDATA {
	PBMSG_HEAD h;
	int iResult;
	unsigned short wMapSvrNum;
};

struct CSP_ANS_NPCUPDATEDATA {
	PBMSG_HEAD h;
	int iResult;
	unsigned short wMapSvrNum;
};

struct CSP_ANS_NPCUPGRADE {
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
	int iIndex;
	int iNpcNumber;
	int iNpcIndex;
	int iNpcUpType;
	int iNpcUpValue;
	int iNpcUpIndex;
};

struct CSP_ANS_OWNERGUILDMASTER {
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
	int iIndex;
	char szCastleOwnGuild[0x8];
	char szCastleOwnGuildMaster[0xA];
};

struct CSP_ANS_REGATTACKGUILD {
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
	int iIndex;
	char szEnemyGuildName[0x8];
};

struct CSP_ANS_RESETCASTLETAXINFO {
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
};

struct CSP_ANS_RESETREGSIEGEINFO {
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
};

struct CSP_ANS_RESETSIEGEGUILDINFO {
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
};

struct CSP_ANS_SDEDCHANGE {
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
	int iIndex;
	unsigned short wStartYear;
	unsigned char btStartMonth;
	unsigned char btStartDay;
	unsigned short wEndYear;
	unsigned char btEndMonth;
	char btEndDay;
};

struct CSP_ANS_SIEGEENDCHANGE {
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
	int bIsSiegeEnded;
};

struct CSP_ANS_TAXINFO {
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
	int iIndex;
	__int64 i64CastleMoney;
	int iTaxRateChaos;
	int iTaxRateStore;
	int iTaxHuntZone;
};

struct CSP_ANS_TAXRATECHANGE {
	PBMSG_HEAD2 h;
	int iResult;
	unsigned short wMapSvrNum;
	int iIndex;
	int iTaxKind;
	int iTaxRate;
};

struct CSP_CALCREGGUILDLIST {
	char szGuildName[0x8];
	int iRegMarkCount;
	int iGuildMemberCount;
	int iGuildMasterLevel;
	int iSeqNum;
};

struct CSP_CSGUILDUNIONINFO {
	char szGuildName[0x8];
	int iCsGuildID;
};

struct CSP_CSINITDATA {
	int iNpcNumber;
	int iNpcIndex;
	int iNpcDfLevel;
	int iNpcRgLevel;
	int iNpcMaxHp;
	int iNpcHp;
	unsigned char btNpcX;
	unsigned char btNpcY;
	unsigned char btNpcDIR;
};

struct CSP_CSLOADTOTALGUILDINFO {
	char szGuildName[0x8];
	int iCsGuildID;
	int iGuildInvolved;
	int iGuildScore;
};

struct CSP_CSSAVETOTALGUILDINFO {
	char szGuildName[0x8];
	int iCsGuildID;
	int iGuildInvolved;
	int iGuildScore;
};

struct CSP_GLOBALPOST_MULTICAST {
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
	char szSender[MAX_ACCOUNT_LEN];
	char szMessage[MAX_CHAT_LEN];
	char szServerName[50];
	BYTE btColorRGB[3];
};

struct CSP_GUILDREGINFO {
	char szGuildName[0x8];
	int iRegMarkCount;
	unsigned char bIsGiveUp;
	unsigned char btRegRank;
};

struct CSP_NPCSAVEDATA {
	int iNpcNumber;
	int iNpcIndex;
	int iNpcDfLevel;
	int iNpcRgLevel;
	int iNpcMaxHp;
	int iNpcHp;
	unsigned char btNpcX;
	unsigned char btNpcY;
	unsigned char btNpcDIR;
};

struct CSP_NPCUPDATEDATA {
	int iNpcNumber;
	int iNpcIndex;
	int iNpcDfLevel;
	int iNpcRgLevel;
	int iNpcMaxHp;
	int iNpcHp;
	unsigned char btNpcX;
	unsigned char btNpcY;
	unsigned char btNpcDIR;
};

struct CSP_REQ_ALLGUILDREGINFO {
	PBMSG_HEAD h;
	WORD wMapSvrNum;
	int iIndex;
};

struct CSP_REQ_CALCREGGUILDLIST {
	PBMSG_HEAD h;
	WORD wMapSvrNum;
};

struct CSP_REQ_CASTLEDATA {
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
	int iCastleEventCycle;
};

struct CSP_REQ_CASTLEOWNERCHANGE {
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
	BOOL bIsCastleOccupied;
	char szOwnerGuildName[8];
};

struct CSP_REQ_CASTLESIEGEEND {
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
};

struct CSP_REQ_CASTLESTATESYNC {
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
	int iCastleState;
	int iTaxRateChaos;
	int iTaxRateStore;
	int iTaxHuntZone;
	char szOwnerGuildName[8];
};

struct CSP_REQ_CASTLETRIBUTEMONEY {
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
	int iCastleTributeMoney;
};

struct CSP_REQ_CSGUILDUNIONINFO {
	PWMSG_HEAD h;
	unsigned short wMapSvrNum;
	int iCount;
};

struct CSP_REQ_CSINITDATA {
	PBMSG_HEAD h;
	WORD wMapSvrNum;
	int iCastleEventCycle;
};

struct CSP_REQ_CSLOADTOTALGUILDINFO {
	PBMSG_HEAD h;
	WORD wMapSvrNum;
};

struct CSP_REQ_CSSAVETOTALGUILDINFO {
	PWMSG_HEAD h;
	unsigned short wMapSvrNum;
	int iCount;
};

struct CSP_REQ_GUILDREGINFO {
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
	int iIndex;
	char szGuildName[8];
};

struct CSP_REQ_GUILDREGMARK {
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
	int iIndex;
	char szGuildName[8];
	int iItemPos;
};

struct CSP_REQ_GUILDRESETMARK {
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
	int iIndex;
	char szGuildName[8];
};

struct CSP_REQ_GUILDSETGIVEUP {
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
	int iIndex;
	char szGuildName[8];
	BOOL bIsGiveUp;
};

struct CSP_REQ_MAPSVRMULTICAST {
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
	char szMsgText[128];
};

struct CSP_REQ_MONEYCHANGE {
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
	int iIndex;
	int iMoneyChanged;
};

struct CSP_REQ_NPCBUY {
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
	int iIndex;
	int iNpcNumber;
	int iNpcIndex;
	int iNpcDfLevel;
	int iNpcRgLevel;
	int iNpcMaxHp;
	int iNpcHp;
	BYTE btNpcX;
	BYTE btNpcY;
	BYTE btNpcDIR;
	int iBuyCost;
};

struct CSP_REQ_NPCDATA {
	PBMSG_HEAD h;
	WORD wMapSvrNum;
	int iIndex;
};

struct CSP_REQ_NPCREMOVE {
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
	int iNpcNumber;
	int iNpcIndex;
};

struct CSP_REQ_NPCREPAIR {
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
	int iIndex;
	int iNpcNumber;
	int iNpcIndex;
	int iRepairCost;
};

struct CSP_REQ_NPCSAVEDATA {
	PWMSG_HEAD h;
	unsigned short wMapSvrNum;
	int iCount;
};

struct CSP_REQ_NPCUPDATEDATA {
	PWMSG_HEAD h;
	unsigned short wMapSvrNum;
	int iCount;
};

struct CSP_REQ_NPCUPGRADE {
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
	int iIndex;
	int iNpcNumber;
	int iNpcIndex;
	int iNpcUpType;
	int iNpcUpValue;
	int iNpcUpIndex;
};

struct CSP_REQ_OWNERGUILDMASTER {
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
	int iIndex;
};

struct CSP_REQ_REGATTACKGUILD {
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
	int iIndex;
	char szEnemyGuildName[8];
};

struct CSP_REQ_RESETCASTLETAXINFO {
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
};

struct CSP_REQ_RESETREGSIEGEINFO {
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
};

struct CSP_REQ_RESETSIEGEGUILDINFO {
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
};

struct CSP_REQ_SDEDCHANGE {
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
	int iIndex;
	WORD wStartYear;
	BYTE btStartMonth;
	BYTE btStartDay;
	WORD wEndYear;
	BYTE btEndMonth;
	BYTE btEndDay;
};

struct CSP_REQ_SIEGEENDCHANGE {
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
	BOOL bIsSiegeEnded;
};

struct CSP_REQ_TAXINFO {
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
	int iIndex;
};

struct CSP_REQ_TAXRATECHANGE {
	PBMSG_HEAD2 h;
	WORD wMapSvrNum;
	int iIndex;
	int iTaxKind;
	int iTaxRate;
};

struct CWP_ANS_CRYWOLFINFOLOAD {
	PBMSG_HEAD h;
	BYTE btResult;
	int iCrywolfState;
	int iOccupationState;
};

struct CWP_ANS_CRYWOLFINFOSAVE {
	PBMSG_HEAD h;
	BYTE btResult;
};

struct CWP_ANS_CRYWOLFSYNC {
	PBMSG_HEAD h;
	BYTE btResult;
	int iCrywolfState;
	int iOccupationState;
	int iApplyBenefit;
	int iApplyPenalty;
	int iPlusChaosRate;
	int iGemDropPenaltyRate;
	int iGettingExpPenaltyRate;
	int iMinusMonHPBenefitRate;
	int iKundunHPRefillState;
};

struct CWP_REQ_CRYWOLFINFOLOAD {
	PBMSG_HEAD h;
	WORD wMapSvrNum;
};

struct CWP_REQ_CRYWOLFINFOSAVE {
	PBMSG_HEAD h;
	WORD wMapSvrNum;
	int iCrywolfState;
	int iOccupationState;
};

struct CWP_REQ_CRYWOLFSYNC {
	PBMSG_HEAD h;
	WORD wMapSvrNum;
	int iCrywolfState;
	int iOccupationState;
	int iApplyBenefit;
	int iApplyPenalty;
	int iPlusChaosRate;
	int iGemDropPenaltyRate;
	int iGettingExpPenaltyRate;
	int iMinusMonHPBenefitRate;
	int iKundunHPRefillState;
};

struct DS_GET_MONSTERCNT {
	PBMSG_HEAD2 h;
	char character[11];
	int cnt;
	short aIndex;
};

struct DS_SAVE_MONSTERCNT {
	PBMSG_HEAD2 h;
	char character[11];
	int type;
	int cnt;
};

struct DS_SAVE_PLAYERKILLER {
	PBMSG_HEAD2 h;
	char Victim[11];
	char Killer[11];
};

struct DSF_GO_FINAL_PARTY {
	DSF_GO_FINAL_PARTY() {
		this->Clear();
	}
	void Clear() {
		memset(szUserName1, 0x00, sizeof(szUserName1));
		memset(szUserName2, 0x00, sizeof(szUserName2));
		this->wServerCode1 = -1;
		this->wServerCode2 = -1;
		this->btRank = 0;
		this->btDSFType = 0;
		this->iPoint = 0;
		this->iEnterYear = 0;
		this->btEnterMonth = 0;
		this->btEnterDay = 0;
	}
	char szUserName1[MAX_ACCOUNT_LEN + 1];
	char szUserName2[MAX_ACCOUNT_LEN + 1];
	WORD wServerCode1;
	WORD wServerCode2;
	BYTE btRank;
	BYTE btDSFType;
	int iPoint;
	int iEnterYear;
	BYTE btEnterMonth;
	BYTE btEnterDay;
};

struct DSF_TEAM_PARTY_USER_NAME {
	int iTeamIndex;
	char szUserName1[MAX_ACCOUNT_LEN + 1];
	char szUserName2[MAX_ACCOUNT_LEN + 1];
	int iTeamLevel;
};

struct DSMSG_ANS_SUBSERVER_COUNT {
	PBMSG_HEAD2 h;
	WORD wSvrCount;
};

struct DSMSG_ANS_WHISPER_RESULT {
	PBMSG_HEAD2 h;
	int OriginGSIndex;
	int OriginPlayerIndex;
	BYTE btResult;
};

struct DSMSG_CHAOSMACHINE_LOSTITEMS {
	PWMSG_HEAD h;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
	BYTE btLostItems[CHAOS_BOX_SIZE * MAX_DBITEM_INFO];
};

struct DSMSG_GS_WHISPER_RESULT {
	PBMSG_HEAD2 h;
	int OriginPlayerIndex;
	BYTE btResult;
};

struct DSMSG_REQ_SUBSERVER_COUNT {
	PBMSG_HEAD2 h;
	WORD wMapSvrGroup;
};

struct EXSDHP_GUILD_ASSIGN_STATUS_REQ {
	PBMSG_HEAD h;
	WORD wUserIndex;
	BYTE btType;
	BYTE btGuildStatus;
	char szGuildName[9];
	char szTargetName[11];
};

struct EXSDHP_GUILD_ASSIGN_STATUS_RESULT {
	PBMSG_HEAD h;
	unsigned char btFlag;
	unsigned short wUserIndex;
	unsigned char btType;
	unsigned char btResult;
	unsigned char btGuildStatus;
	char szGuildName[9];
	char szTargetName[11];
};

struct EXSDHP_GUILD_ASSIGN_TYPE_REQ {
	PBMSG_HEAD h;
	WORD wUserIndex;
	BYTE btGuildType;
	char szGuildName[9];
};

struct EXSDHP_GUILD_ASSIGN_TYPE_RESULT {
	PBMSG_HEAD h;
	unsigned char btFlag;
	unsigned short wUserIndex;
	unsigned char btGuildType;
	unsigned char btResult;
	char szGuildName[9];
};

struct EXSDHP_KICKOUT_UNIONMEMBER_REQ {
	struct PBMSG_HEAD2 h;
	unsigned short wRequestUserIndex;
	unsigned char btRelationShipType;
	char szUnionMasterGuildName[8];
	char szUnionMemberGuildName[8];
};

struct EXSDHP_KICKOUT_UNIONMEMBER_RESULT {
	PBMSG_HEAD2 h;
	unsigned char btFlag;
	unsigned short wRequestUserIndex;
	unsigned char btRelationShipType;
	unsigned char btResult;
	char szUnionMasterGuildName[9];
	char szUnionMemberGuildName[9];
};

struct EXSDHP_NOTIFICATION_RELATIONSHIP {
	PWMSG_HEAD h;
	unsigned char btFlag;
	unsigned char btUpdateFlag;
	unsigned char btGuildListCount;
	int iGuildList[100];
};

struct EXSDHP_RELATIONSHIP_BREAKOFF_REQ {
	struct PBMSG_HEAD h;
	unsigned short wRequestUserIndex;
	unsigned short wTargetUserIndex;
	unsigned char btRelationShipType;
	int iRequestGuildNum;
	int iTargetGuildNum;
};

struct EXSDHP_RELATIONSHIP_BREAKOFF_RESULT {
	PBMSG_HEAD h;
	unsigned char btFlag;
	unsigned short wRequestUserIndex;
	unsigned short wTargetUserIndex;
	unsigned char btResult;
	unsigned char btRelationShipType;
	int iRequestGuildNum;
	int iTargetGuildNum;
};

struct EXSDHP_RELATIONSHIP_JOIN_REQ {
	struct PBMSG_HEAD h;
	unsigned short wRequestUserIndex;
	unsigned short wTargetUserIndex;
	unsigned char btRelationShipType;
	int iRequestGuildNum;
	int iTargetGuildNum;
};

struct EXSDHP_RELATIONSHIP_JOIN_RESULT {
	PBMSG_HEAD h;
	unsigned char btFlag;
	unsigned short wRequestUserIndex;
	unsigned short wTargetUserIndex;
	unsigned char btResult;
	unsigned char btRelationShipType;
	int iRequestGuildNum;
	int iTargetGuildNum;
	char szRequestGuildName[9];
	char szTargetGuildName[9];
};

struct EXSDHP_SERVERGROUP_GENS_CHATTING_RECV {
	PBMSG_HEAD h;
	int iGensNum;
	char szCharacterName[10];
	char szChattingMsg[90];
};

struct EXSDHP_SERVERGROUP_GUILD_CHATTING_RECV {
	PBMSG_HEAD h;
	int iGuildNum;
	char szCharacterName[10];
	char szChattingMsg[90];
};

struct EXSDHP_SERVERGROUP_UNION_CHATTING_RECV {
	PBMSG_HEAD h;
	int iUnionNum;
	char szCharacterName[10];
	char szChattingMsg[90];
};

struct EXSDHP_UNION_LIST_COUNT {
	struct PWMSG_HEAD h;
	unsigned char btCount;
	unsigned char btResult;
	unsigned short wRequestUserIndex;
	int iTimeStamp;
	unsigned char btRivalMemberNum;
	unsigned char btUnionMemberNum;
};

struct EXSDHP_UNION_LIST_REQ {
	struct PBMSG_HEAD h;
	unsigned short wRequestUserIndex;
	int iUnionMasterGuildNumber;
};

struct EXSDHP_UNION_LIST {
	unsigned char btMemberNum;
	unsigned char Mark[32];
	char szGuildName[8];
};

struct EXSDHP_UNION_RELATIONSHIP_LIST {
	PWMSG_HEAD h;
	unsigned char btFlag;
	unsigned char btRelationShipType;
	unsigned char btRelationShipMemberCount;
	char szUnionMasterGuildName[9];
	int iUnionMasterGuildNumber;
	int iRelationShipMember[100];
};

struct FCHP_CHATROOM_CREATE_REQ {
	PBMSG_HEAD h;
	char Name[10];
	char FriendName[10];
	BYTE Type;
	short Number;
	short ServerId;
	short FriendNumber;
	short FriendServerId;
};

struct FCHP_CHATROOM_CREATE_RESULT {
	PBMSG_HEAD h;
	BYTE Result;
	short RoomNumber;
	char Name[10];
	char FriendName[10];
	short Number;
	short ServerId;
	DWORD Ticket;
	DWORD Trash;
	BYTE Type;
};

struct FCHP_CHATROOM_INVITATION_REQ {
	PBMSG_HEAD h;
	short RoomNumber;
	char Name[10];
	short Number;
	short ServerId;
	BYTE Type;
};

struct FHP_FRIEND_ADD_REQ {
	struct PBMSG_HEAD h;
	short Number;
	char Name[10];
	char FriendName[10];
};

struct FHP_FRIEND_ADD_RESULT {
	PBMSG_HEAD h;
	short Number;
	unsigned char Result;
	char Name[10];
	char FriendName[10];
	unsigned char Server;
};

struct FHP_FRIEND_CHATROOM_CREATE_REQ {
	struct PBMSG_HEAD h;
	short Number;
	char Name[10];
	char fName[10];
};

struct FHP_FRIEND_CHATROOM_CREATE_RESULT {
	PBMSG_HEAD h;
	unsigned char Result;
	short Number;
	char Name[10];
	char FriendName[10];
	char ServerIp[15];
	unsigned short RoomNumber;
	unsigned long Ticket;
	unsigned char Type;
};

struct FHP_FRIEND_DEL_RESULT {
	PBMSG_HEAD h;
	short Number;
	unsigned char Result;
	char Name[10];
	char FriendName[10];
};

struct FHP_FRIEND_INVITATION_REQ {
	struct PBMSG_HEAD h;
	short Number;
	char Name[10];
	char FriendName[10];
	unsigned short RoomNumber;
	unsigned long WindowGuid;
};

struct FHP_FRIEND_INVITATION_RET {
	PBMSG_HEAD h;
	unsigned char Result;
	short Number;
	char Name[10];
	DWORD WindowGuid;
};

struct FHP_FRIEND_MEMO_DEL_REQ {
	struct PBMSG_HEAD h;
	short Number;
	unsigned short MemoIndex;
	char Name[10];
};

struct FHP_FRIEND_MEMO_DEL_RESULT {
	PBMSG_HEAD h;
	unsigned char Result;
	unsigned short MemoIndex;
	short Number;
	char Name[10];
};

struct FHP_FRIEND_MEMO_LIST_REQ {
	struct PBMSG_HEAD h;
	unsigned short Number;
	char Name[10];
};

struct FHP_FRIEND_MEMO_LIST {
	PWMSG_HEAD h;
	unsigned short Number;
	unsigned short MemoIndex;
	char SendName[10];
	char RecvName[10];
	char Date[30];
	char Subject[60];
	unsigned char read;
};

struct FHP_FRIEND_MEMO_RECV_OLD {
	PWMSG_HEAD h;
	short Number;
	char Name[10];
	unsigned short MemoIndex;
	short MemoSize;
	unsigned char Photo[13];
	unsigned char Dir;
	unsigned char Action;
	char Memo[1000];
};

struct FHP_FRIEND_MEMO_RECV_REQ {
	struct PBMSG_HEAD h;
	short Number;
	unsigned short MemoIndex;
	char Name[10];
};

struct FHP_FRIEND_MEMO_RECV {
	PWMSG_HEAD h;
	short Number;
	char Name[10];
	unsigned short MemoIndex;
	short MemoSize;
	unsigned char Photo[18];
	unsigned char Dir;
	unsigned char Action;
	char Memo[1000];
};

struct FHP_FRIEND_MEMO_SEND_OLD {
	struct PWMSG_HEAD h;
	short Number;
	unsigned long WindowGuid;
	char Name[10];
	char ToName[10];
	char Subject[32];
	unsigned char Dir;
	unsigned char Action;
	short MemoSize;
	unsigned char Photo[13];
	char Memo[1000];
};

struct FHP_FRIEND_MEMO_SEND_RESULT {
	PBMSG_HEAD h;
	short Number;
	char Name[10];
	unsigned char Result;
	unsigned long WindowGuid;
};

struct FHP_FRIEND_MEMO_SEND {
	struct PWMSG_HEAD h;
	short Number;
	unsigned long WindowGuid;
	char Name[10];
	char ToName[10];
	char Subject[60];
	unsigned char Dir;
	unsigned char Action;
	short MemoSize;
	unsigned char Photo[18];
	char Memo[1000];
};

struct FHP_FRIEND_STATE_C {
	struct PBMSG_HEAD h;
	short Number;
	char Name[10];
	unsigned char State;
};

struct FHP_FRIEND_STATE {
	PBMSG_HEAD h;
	short Number;
	char Name[10];
	char FriendName[10];
	unsigned char State;
};

struct FHP_FRIENDLIST_COUNT {
	struct PWMSG_HEAD h;
	short Number;
	char Name[10];
	unsigned char Count;
	unsigned char MailCount;
	unsigned char Server;
};

struct FHP_FRIENDLIST_REQ {
	struct PBMSG_HEAD h;
	short Number;
	char Name[10];
	unsigned char pServer;
};

struct FHP_FRIENDLIST {
	char Name[10];
	unsigned char Server;
};

struct FHP_WAITFRIEND_ADD_REQ {
	struct PBMSG_HEAD h;
	unsigned char Result;
	short Number;
	char Name[10];
	char FriendName[10];
};

struct FHP_WAITFRIEND_ADD_RESULT {
	PBMSG_HEAD h;
	short Number;
	unsigned char Result;
	char Name[10];
	char FriendName[10];
	unsigned char pServer;
};

struct FHP_WAITFRIENDLIST_COUNT {
	PBMSG_HEAD h;
	short Number;
	char Name[10];
	char FriendName[10];
};

struct FRIEND_MASTER {
	int m_iGUID;
	int m_iMemoTotal;
	int m_iNumber;
	BYTE m_btState;
	int m_iServer;
	std::vector < NAME > m_vecFriends;
	FRIEND_MASTER() {
		m_iGUID = -1;
		m_iMemoTotal = 0;
		m_iNumber = -1;
		m_btState = -1;
		m_iServer = -1;
		m_vecFriends.clear();
	}
};

struct GENS_USER {
	char Name[11];
	int iContributePoint;
	int iGensClass;
	int iRank;
	int iReward;
};

struct GS_CONNECT_INFO {
	PBMSG_HEAD h;
	UCHAR SubHead;
	char IP[16];
	USHORT Port;
};

struct ISHOP_ANS_ITEMLIST {
	PWMSG_HEAD2 h;
	short aIndex;
	BYTE InvType;
	BYTE InvNum;
	char AccountID[11];
	int Result;
	int Count;
};

struct ISHOP_ANS_POINT {
	PBMSG_HEAD h;
	short aIndex;
	char AccountID[11];
	BYTE Result;
	float CoinP;
	float CoinC;
	float Goblin;
};

struct ISHOP_CHARCARD_BUY {
	PBMSG_HEAD h;
	char AccountID[11];
	BYTE Type;
};

struct ISHOP_ITEM_BUY_PACKAGE {
	PWMSG_HEAD2 h;
	short aIndex;
	char AccountID[11];
	int Price;
	BYTE CoinType;
	WORD Count;
};

struct ISHOP_ITEM_BUY {
	PBMSG_HEAD h;
	short aIndex;
	char AccountID[11];
	int ID1;
	int ID2;
	int ID3;
	int Price;
	BYTE CoinType;
};

struct ISHOP_ITEM_BUYANS {
	PBMSG_HEAD h;
	short aIndex;
	char AccountID[11];
	BYTE Result;
	int ID1;
	int ID2;
	int ID3;
};

struct ISHOP_ITEM_DELETE {
	PBMSG_HEAD h;
	char AccountID[11];
	int UniqueCode;
	int AuthCode;
};

struct ISHOP_ITEM_GIFT_PACKAGE {
	PWMSG_HEAD2 h;
	short aIndex;
	char AccountID[11];
	int Price;
	BYTE CoinType;
	char Name[11];
	char TargetName[11];
	char Message[200];
	WORD Count;
};

struct ISHOP_ITEM_GIFT {
	PWMSG_HEAD h;
	short aIndex;
	char AccountID[11];
	int ID1;
	int ID2;
	int ID3;
	int Price;
	BYTE CoinType;
	char Name[11];
	char TargetName[11];
	char Message[200];
};

struct ISHOP_ITEM_GIFTANS {
	PBMSG_HEAD h;
	short aIndex;
	char AccountID[11];
	BYTE Result;
	int ID1;
	int ID2;
	int ID3;
};

struct ISHOP_ITEM_PACKAGE {
	int ID1;
	int ID2;
	int ID3;
};

struct ISHOP_ITEM_ROLLBACK {
	PBMSG_HEAD h;
	char AccountID[11];
	int UniqueCode;
	int AuthCode;
};

struct ISHOP_ITEM_USE {
	PBMSG_HEAD h;
	short aIndex;
	char AccountID[11];
	int UniqueCode;
	int AuthCode;
};

struct ISHOP_ITEM_USEANS {
	PBMSG_HEAD h;
	short aIndex;
	char AccountID[11];
	BYTE Result;
	int UniqueCode;
	int AuthCode;
	int ID1;
	int ID2;
	int ID3;
};

struct ISHOP_ITEMLIST {
	int UniqueCode;
	int AuthCode;
	int UniqueID1;
	int UniqueID2;
	int UniqueID3;
	BYTE InventoryType;
	char GiftName[10];
	char Message[200];
};

struct ISHOP_POINT_ADD {
	PBMSG_HEAD h;
	short aIndex;
	char AccountID[11];
	BYTE Type;
	float Coin;
};

struct ISHOP_REQ_ITEMLIST {
	PBMSG_HEAD2 h;
	short aIndex;
	BYTE InvType;
	BYTE InvNum;
	char AccountID[11];
};

struct ISHOP_REQ_POINT {
	PBMSG_HEAD h;
	short aIndex;
	char AccountID[11];
};

struct ISHOP_VIP_BUY {
	PBMSG_HEAD h;
	char AccountID[11];
	BYTE Type;
	int Days;
};

struct itemBranch {
	DWORD dwBranchID;
	DWORD dwGuid;
	DWORD dwItemType;
};

struct ITEMPERIOD_DATEINFO {
	BYTE btUsedInfo;
	BYTE btItemType;
	WORD wItemCode;
	BYTE btEffectCategory;
	BYTE btEffectType1;
	BYTE btEffectType2;
	UINT64 Serial;
	time_t lItemBuyDate;
	time_t lItemExpireDate;
};

struct MEMO_HEADER {
	unsigned short MemoIndex;
	char SendName[10];
	char RecvName[10];
	char Date[30];
	char Subject[60];
	unsigned char read;
};

struct MEMO_READ {
	unsigned char Dir;
	unsigned char Action;
	char Memo[1000];
};

struct MEMO_SEND_HEADER {
	char Name[10];
	char ToName[10];
	char Subject[60];
	unsigned char Dir;
	unsigned char Action;
};

typedef struct MUBOT_SETTINGS_REQ_SAVE {
	PWMSG_HEAD h;
	char szName[11];
	BYTE btDATA[512];
} *LPMUBOT_SETTINGS_REQ_SAVE;

struct MUBOT_SETTINGS_SEND {
	PWMSG_HEAD h;
	int aIndex;
	char szName[11];
	BYTE btDATA[512];
};

struct NAME {
	char m_szName[11];
	NAME() {
		std::ZeroMemory(this, sizeof(*this));
	}
	NAME(char * szName) {
		std::ZeroMemory(this, sizeof(*this));
		std::strncpy(m_szName, szName, 10);
	}
};

struct P_ADD_NEWS {
	BYTE day;
	BYTE month;
	WORD year;
	DWORD dateColor;
	DWORD titleColor;
	DWORD textColor;
	char title[40];
	WORD textLen;
	char Text[2048];
};

struct PARTYMATCHING_PARTYDATA {
	PARTYMATCHING_PARTYDATA() {
		this->m_PartyMatchingIndex = -1;
		this->m_UserCount = 0;
	}
	int m_PartyMatchingIndex;
	int m_UserCount;
	char szLeaderName[MAX_ACCOUNT_LEN + 1];
	PARTYMATCHING_PARTYUSERDATA m_UserData[5];
};

struct PARTYMATCHING_PARTYUSERDATA {
	PARTYMATCHING_PARTYUSERDATA() {
		this->iUsedInfo = FALSE;
		this->iUserConnected = FALSE;
		this->iUserIndex = -1;
		this->iServerNumber = -1;
		this->iServerIndex = -1;
	}
	int iUsedInfo;
	int iUserConnected;
	int iUserIndex;
	int iServerNumber;
	int iServerIndex;
	char szUserName[MAX_ACCOUNT_LEN + 1];
};

struct PERIOD_EXPIRED_ITEM {
	WORD wItemCode;
	UINT64 Serial;
};

struct PERIOD_EXPIRED_ITEMLIST {
	BYTE btItemCount;
	PERIOD_EXPIRED_ITEM ExpiredItem[90];
};

struct PERIOD_ITEMLIST {
	DWORD dwUserGuid;
	BYTE btItemCount;
	char chCharacterName[MAX_ACCOUNT_LEN + 1];
	ITEMPERIOD_DATEINFO ItemData[100];
};

struct PMSG_ACTION {
	PBMSG_HEAD h;
	BYTE Dir;
	BYTE ActionNumber;
	BYTE iTargetIndexH;
	BYTE iTargetIndexL;
};

struct PMSG_ACTIONRESULT {
	PBMSG_HEAD h;
	BYTE NumberH;
	BYTE NumberL;
	BYTE Dir;
	BYTE ActionNumber;
	BYTE TargetNumberH;
	BYTE TargetNumberL;
};

struct PMSG_ADDON_REQUEST {
	BYTE HEAD;
	BYTE SIZE;
	BYTE OPCODE;
	BYTE OPCODE2;
	WORD M_ADDON;
	DWORD M_CRC1;
	DWORD M_CRC2;
};

struct PMSG_ADDSTATS_RESULT {
	PBMSG_HEAD2 h;
	WORD str;
	WORD stradd;
	WORD agi;
	WORD agiadd;
	WORD vit;
	WORD vitadd;
	WORD ene;
	WORD eneadd;
	WORD cmd;
	WORD cmdadd;
	float mPrec;
};

struct PMSG_ADDSTATS {
	PBMSG_HEAD h;
	BYTE subcode;
	BYTE statstype;
	WORD Points;
	WORD LUP;
	int MaxLifeAndMana;
};

struct PMSG_AGILITYSEND {
	PBMSG_HEAD h;
	BYTE subcode;
	WORD Agility;
};

struct PMSG_AHINFO {
	PBMSG_HEAD h;
	BYTE SUB;
	DWORD Time;
	WORD AttackSpeed;
	WORD MagicSpeed;
	char dump1[15];
	BYTE IsModifiedGameSpeed;
	char dump2[15];
	BYTE IsModifiedAttackSpeed;
	char dump3[15];
	BYTE IsModifiedMagicSpeed;
	char dump4[15];
};

struct PMSG_ALTERPSHOPVAULT {
	PBMSG_HEAD h;
	BYTE subcode;
	BYTE type;
};

struct PMSG_ANS_2ANIV_SERIAL {
	struct PBMSG_HEAD h;
	int iINDEX;
	char szUID[11];
	unsigned char btIsRegistered;
	int iGiftNumber;
};

struct PMSG_ANS_ACCEPTMEMBER_PARTYMATCHING {
	PBMSG_HEAD2 h;
	char szMemberName[MAX_ACCOUNT_LEN + 1];
	BYTE btType;
	BYTE btFlag;
	int nResult;
};

struct PMSG_ANS_ALLOW_JOIN_GUILDMATCHING {
	PBMSG_HEAD2 h;
	char szMemberName[MAX_ACCOUNT_LEN + 1];
	int nAllowType;
	int nResult;
};

struct PMSG_ANS_ALLOWLIST_STATE_GUILDMATCHING {
	PBMSG_HEAD2 h;
	char szGuildMasterName[MAX_ACCOUNT_LEN + 1];
	char szGuildName[MAX_GUILD_LEN + 1];
	int nResult;
};

struct PMSG_ANS_BLOODCASTLE_ENTERCOUNT {
	struct PBMSG_HEAD2 h;
	char AccountID[10];
	char GameID[10];
	int ServerCode;
	int iObjIndex;
	int iLeftCount;
};

struct PMSG_ANS_BLOODCASTLESCORE_5TH {
	struct PBMSG_HEAD2 h;
	char AccountID[10];
	char GameID[10];
	int ServerCode;
	int Score;
	int Class;
	int BridgeNum;
	int iLeftTime;
	int iAlivePartyCount;
};

struct PMSG_ANS_CANCEL_GUILDMATCHINGLIST {
	PBMSG_HEAD2 h;
	int nResult;
};

struct PMSG_ANS_CANCEL_JOIN_GUILDMATCHING {
	PBMSG_HEAD2 h;
	int nResult;
};

struct PMSG_ANS_CANCEL_JOIN_PARTYMATCHING {
	PBMSG_HEAD2 h;
	BYTE btType;
	int nResult;
};

struct PMSG_ANS_CASTLESIEGESTATE {
	PBMSG_HEAD2 h;
	BYTE btResult;
	char cCastleSiegeState;
	BYTE btStartYearH;
	BYTE btStartYearL;
	BYTE btStartMonth;
	BYTE btStartDay;
	BYTE btStartHour;
	BYTE btStartMinute;
	BYTE btEndYearH;
	BYTE btEndYearL;
	BYTE btEndMonth;
	BYTE btEndDay;
	BYTE btEndHour;
	BYTE btEndMinute;
	BYTE btSiegeStartYearH;
	BYTE btSiegeStartYearL;
	BYTE btSiegeStartMonth;
	BYTE btSiegeStartDay;
	BYTE btSiegeStartHour;
	BYTE btSiegeStartMinute;
	char cOwnerGuild[8];
	char cOwnerGuildMaster[10];
	BYTE btStateLeftSec1;
	BYTE btStateLeftSec2;
	BYTE btStateLeftSec3;
	BYTE btStateLeftSec4;
};

struct PMSG_ANS_CHAOS_MULTIMIX_CHECK {
	PBMSG_HEAD h;
	BYTE btResult;
};

struct PMSG_ANS_CHAOSCASTLE_RANKING {
	struct PBMSG_HEAD2 h;
	char AccountID[10];
	char GameID[10];
	int ServerCode;
	int Class;
	int Castle;
	int PlayerKill;
	int MonsterKill;
	__int64 Experience;
	BYTE IsWinner;
};

struct PMSG_ANS_CLASSDEF {
	PWMSG_HEAD h;
	int iCount;
	CLASSDEF_DATA m_ClassData[MAX_TYPE_PLAYER];
};

struct PMSG_ANS_CSATTKGUILDLIST {
	PWMSG_HEAD h;
	BYTE btResult;
	int iCount;
};

struct PMSG_ANS_CSCOMMAND {
	PBMSG_HEAD2 h;
	BYTE btTeam;
	BYTE btX;
	BYTE btY;
	BYTE btCommand;
};

struct PMSG_ANS_CSGATECURSTATE {
	PBMSG_HEAD2 h;
	BYTE btOperate;
	BYTE btIndex1;
	BYTE btIndex2;
};

struct PMSG_ANS_CSGATEOPERATE {
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btOperate;
	BYTE btIndex1;
	BYTE btIndex2;
};

struct PMSG_ANS_CSGATESTATE {
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btIndex1;
	BYTE btIndex2;
};

struct PMSG_ANS_CSHUNTZONEENTER {
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btHuntZoneEnter;
};

struct PMSG_ANS_CSLEFTTIMEALARM {
	PBMSG_HEAD2 h;
	BYTE btHour;
	BYTE btMinute;
};

struct PMSG_ANS_DELETE_STONES {
	struct PBMSG_HEAD h;
	int iINDEX;
	char szUID[11];
	char bSUCCESS;
	int iStoneCount;
};

struct PMSG_ANS_DSF_ACCEPTENTER {
	PBMSG_HEAD2 h;
	BYTE btError;
	char Name[MAX_ACCOUNT_LEN + 1];
};

struct PMSG_ANS_DSF_CAN_PARTY_ENTER {
	PBMSG_HEAD2 h;
	int iUserIndex;
	BYTE btCount;
	BYTE btResult;
};

struct PMSG_ANS_DSF_ENTER {
	PBMSG_HEAD2 h;
	BYTE btResult;
	char Name[MAX_ACCOUNT_LEN + 1];
	BYTE btDSFType;
};

struct PMSG_ANS_DSF_GET_REWARD {
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_ANS_DSF_GO_FINAL_PARTY_INFO {
	PWMSG_HEAD2 h;
	BYTE btResult;
	BYTE btCnt;
	BYTE btDSFType;
};

struct PMSG_ANS_DSF_GO_FINAL_PARTY {
	PWMSG_HEAD2 h;
	BYTE btPartyCnt;
	BYTE btDSFType;
};

struct PMSG_ANS_DSF_PARTYRANKRENEW {
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_ANS_DSF_SCHEDULE {
	PBMSG_HEAD2 h;
	BYTE LeagueDay1;
	BYTE LeagueDay2;
	BYTE LeagueDay3;
	BYTE LeagueDay4;
	BYTE SemiFinalDay1;
	BYTE SemiFinalDay2;
	BYTE Final;
	BYTE CurDay;
	BYTE LeagueHour[10];
	BYTE LeagueMin[10];
	BYTE SemiFinalHour[10];
	BYTE SemiFinalMin[10];
	BYTE FinalHour;
	BYTE FinalMin;
	BYTE RewardStartDay;
	BYTE RewardEndDay;
};

struct PMSG_ANS_DUEL_ANSWER {
	PBMSG_HEAD2 h;
	BYTE bDuelOK;
	BYTE NumberH;
	BYTE NumberL;
};

struct PMSG_ANS_DUEL_OK {
	PBMSG_HEAD h;
	bool bDuelOK;
	BYTE NumberH;
	BYTE NumberL;
	char szName[10];
};

struct PMSG_ANS_END_DUEL {
	PBMSG_HEAD h;
	BYTE NumberH;
	BYTE NumberL;
	char szName[10];
};

struct PMSG_ANS_ENTER_ON_QUESTNPC {
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_ANS_EVENTENTRY_INFO {
	PBMSG_HEAD2 h;
	int nUserIndex;
	BYTE btEventEntryCount[6];
};

struct PMSG_ANS_EVENTUSERSCORE {
	struct PBMSG_HEAD2 h;
	char AccountID[10];
	char GameID[10];
	int ServerCode;
	int Score;
	int Class;
	int SquareNum;
};

struct PMSG_ANS_EVOMON_MAXSCORE {
	PBMSG_HEAD2 h;
	int nUserIndex;
	int nMaxScore;
};

struct PMSG_ANS_GET_DSF_REWARD {
	PBMSG_HEAD2 h;
	int nUserIndex;
	BYTE btResult;
};

struct PMSG_ANS_GET_PARTYMATCHINGLIST {
	PWMSG_HEAD2 h;
	int nListCount;
	int nPage;
	int nTotalPage;
	int nResult;
};

struct PMSG_ANS_GET_REWARD {
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_ANS_GET_UBF_REAL_NAME {
	PBMSG_HEAD2 h;
	int iUserIndex;
	char szUBFName[MAX_ACCOUNT_LEN + 1];
	char szRealName[MAX_ACCOUNT_LEN + 1];
	int iServerCode;
	BYTE btReturn;
};

struct PMSG_ANS_GIVEUPCASTLESIEGE {
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btIsGiveUp;
	char szGuildName[8];
};

struct PMSG_ANS_GUILDMARK_OF_CASTLEOWNER {
	PBMSG_HEAD2 h;
	BYTE GuildMarkOfCastleOwner[32];
};

struct PMSG_ANS_GUILDMATCHINGLIST {
	PWMSG_HEAD2 h;
	int nPage;
	int nTotalPage;
	int nListCount;
	int nResult;
	_stGuildMatchingList stGuildMatchingList;
};

struct PMSG_ANS_GUILDREGINFO {
	PBMSG_HEAD2 h;
	BYTE btResult;
	char szGuildName[8];
	BYTE btGuildMark1;
	BYTE btGuildMark2;
	BYTE btGuildMark3;
	BYTE btGuildMark4;
	BYTE btIsGiveUp;
	BYTE btRegRank;
};

struct PMSG_ANS_ILLUSIONTEMPLE_RANKING {
	struct PBMSG_HEAD2 h;
	char AccountID[10];
	char GameID[10];
	int ServerCode;
	int Class;
	int Team;
	int TempleNum;
	int KillCount;
	int RelicsGivenCount;
	__int64 Experience;
	int TotalScore;
	BYTE IsWinner;
};

struct PMSG_ANS_IN_PENTAGRAM_JEWEL {
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btJewelPos;
	BYTE btJewelIndex;
	BYTE btMainAttribute;
	BYTE btItemType;
	WORD wItemIndex;
	BYTE btLevel;
	BYTE btRank1OptionNum;
	BYTE btRank1Level;
	BYTE btRank2OptionNum;
	BYTE btRank2Level;
	BYTE btRank3OptionNum;
	BYTE btRank3Level;
	BYTE btRank4OptionNum;
	BYTE btRank4Level;
	BYTE btRank5OptionNum;
	BYTE btRank5Level;
};

struct PMSG_ANS_ITL_RELATE_RELICS {
	PBMSG_HEAD2 h;
	BYTE byError;
	BYTE byNumberH;
	BYTE byNumberL;
	WORD wNpcType;
	BYTE byAct;
};

struct PMSG_ANS_ITL_SCHEDULE {
	PWMSG_HEAD2 h;
	BYTE byCount;
	BYTE LeagueDay1;
	BYTE LeagueDay2;
	BYTE LeagueDay3;
	BYTE LeagueDay4;
	BYTE LeagueDay5;
	BYTE LeagueDay6;
	BYTE LeagueDay7;
	BYTE SemiFinal;
	BYTE Final;
	BYTE ITL_STATE;
};

struct PMSG_ANS_JEWEL_UNMIX {
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_ANS_JOIN_GUILDMATCHING {
	PBMSG_HEAD2 h;
	int nResult;
};

struct PMSG_ANS_LUCKYCOIN_REGISTER {
	PBMSG_HEAD2 h;
	unsigned char btResult;
	int iLuckyCoin;
};

struct PMSG_ANS_LUCKYCOIN_TRADE {
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_ANS_LUCKYCOIN {
	PBMSG_HEAD h;
	int iIndex;
	char szUID[11];
	int LuckyCoins;
};

struct PMSG_ANS_LUCKYCOININFO {
	PBMSG_HEAD2 h;
	int iLuckyCoin;
};

struct PMSG_ANS_LUCKYITEM_SELECT {
	PWMSG_HEAD head;
	WORD wUserIndex;
	BYTE btResultCode;
	BYTE btItemCnt;
};

struct PMSG_ANS_MAPMOVE {
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_ANS_MAPSVRAUTH {
	PBMSG_HEAD h;
	int iIndex;
	char szAccountID[11];
	char szCharName[11];
	char szPassword[20];
	BYTE btSecurityLock;
	DWORD dwSecurityCode;
	WORD wPrevMapSvrCode;
	WORD wMapNumber;
	BYTE btX;
	BYTE btY;
	int iResult;
	int iUserNumber;
	int iDBNumber;
	unsigned char btBlockCode;
};

struct PMSG_ANS_MAPSVRMOVE {
	PBMSG_HEAD h;
	int iIndex;
	char szAccountID[11];
	char szCharName[11];
	unsigned short wCurMapSvrCode;
	unsigned short wDstMapSvrCode;
	unsigned short wMapNumber;
	unsigned char btX;
	unsigned char btY;
	int iResult;
	int iJoinAuthCode1;
	int iJoinAuthCode2;
	int iJoinAuthCode3;
	int iJoinAuthCode4;
};

struct PMSG_ANS_MAPSVRTAXINFO {
	PBMSG_HEAD2 h;
	BYTE btTaxType;
	BYTE btTaxRate;
};

struct PMSG_ANS_MEMBERJOIN_PARTYMATCHINGLIST {
	PBMSG_HEAD2 h;
	int nResult;
};

struct PMSG_ANS_MEMBERJOIN_STATELIST_PARTYMATCHING {
	PBMSG_HEAD2 h;
	int nResult;
	int nLeaderChannel;
	char szLeaderName[MAX_ACCOUNT_LEN + 1];
};

struct PMSG_ANS_MINETWINKLE_CANCEL {
	PBMSG_HEAD2 h;
	WORD wUserIndex;
	WORD wTwinkleIndex;
	WORD wTwinkleType;
	BYTE byMapNumber;
	BYTE byCurrentStage;
};

struct PMSG_ANS_MINETWINKLE_END_ANIMATION {
	PBMSG_HEAD2 h;
	WORD wUserIndex;
	WORD wTwinkleIndex;
	BYTE byMapNumber;
};

struct PMSG_ANS_MINETWINKLE_REWARD {
	PBMSG_HEAD2 h;
	WORD wUserIndex;
	WORD wTwinkleIndex;
	WORD wTwinkleType;
	BYTE byMapNumber;
	BYTE byCurrentStage;
	int iResult;
};

struct PMSG_ANS_MINETWINKLE {
	PBMSG_HEAD2 h;
	WORD wUserIndex;
	WORD wTwinkleIndex;
	WORD wTwinkleType;
	BYTE byMapNumber;
	BYTE byCurrentStage;
	int iRewardJewelNumber;
	int iResult;
};

struct PMSG_ANS_MINIMAPDATA {
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_ANS_MONEYDRAWOUT {
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btMoney1;
	BYTE btMoney2;
	BYTE btMoney3;
	BYTE btMoney4;
	BYTE btMoney5;
	BYTE btMoney6;
	BYTE btMoney7;
	BYTE btMoney8;
};

struct PMSG_ANS_MOVE_TO_CASTLE_HUNTZONE {
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_ANS_MUUN_EXCHANGE {
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_ANS_NOTI_GUILDMATCHING_FORGUILDMASTER {
	PBMSG_HEAD2 h;
	int nResult;
};

struct PMSG_ANS_NOTI_GUILDMATCHING {
	PBMSG_HEAD2 h;
	int nResult;
};

struct PMSG_ANS_NOTI_JOINPARTYMATCHING {
	PBMSG_HEAD2 h;
};

struct PMSG_ANS_NOTIFYCROWNPROC {
	PBMSG_HEAD2 h;
	BYTE btCrownState;
	DWORD dwAccumulatedCrownAccessTime;
};

struct PMSG_ANS_NOTIFYCSPROGRESS {
	PBMSG_HEAD2 h;
	BYTE btCastleSiegeState;
	BYTE szGuildName[8];
};

struct PMSG_ANS_NOTIFYCSSTART {
	PBMSG_HEAD2 h;
	BYTE btStartState;
};

struct PMSG_ANS_NOTIFYSWITCHPROC {
	PBMSG_HEAD2 h;
	BYTE btIndex1;
	BYTE btIndex2;
	BYTE btUserIndex1;
	BYTE btUserIndex2;
	BYTE btSwitchState;
};

struct PMSG_ANS_NPCBUY {
	PBMSG_HEAD2 h;
	BYTE btResult;
	int iNpcNumber;
	int iNpcIndex;
};

struct PMSG_ANS_NPCDBLIST {
	PWMSG_HEAD h;
	BYTE btResult;
	int iCount;
};

struct PMSG_ANS_NPCREPAIR {
	PBMSG_HEAD2 h;
	BYTE btResult;
	int iNpcNumber;
	int iNpcIndex;
	int iNpcHP;
	int iNpcMaxHP;
};

struct PMSG_ANS_NPCUPGRADE {
	PBMSG_HEAD2 h;
	BYTE btResult;
	int iNpcNumber;
	int iNpcIndex;
	int iNpcUpType;
	int iNpcUpValue;
};

struct PMSG_ANS_OUT_PENTAGRAM_JEWEL {
	PBMSG_HEAD2 h;
	BYTE Result;
	BYTE btJewelPos;
	BYTE btJewelDBIndex;
};

struct PMSG_ANS_PARTYMATCHING_ERROR {
	PBMSG_HEAD2 h;
	int iResult;
};

struct PMSG_ANS_PENTAGRAMJEWEL {
	PWMSG_HEAD h;
	BYTE btJewelCnt;
	int iUserIndex;
	int iAnsType;
};

struct PMSG_ANS_PERIODBUFF_SELECT {
	PBMSG_HEAD2 h;
	WORD wUserIndex;
	char szCharacterName[MAX_ACCOUNT_LEN + 1];
	BYTE btResultCode;
	WORD wBuffIndex;
	BYTE btEffectType1;
	BYTE btEffectType2;
	time_t lExpireDate;
};

struct PMSG_ANS_PERIODITEMEX_DELETE {
	PBMSG_HEAD2 head;
	DWORD dwUserGuid;
	WORD wUserIndex;
	BYTE btResultCode;
};

struct PMSG_ANS_PERIODITEMEX_EXPIRED_ITEMLIST {
	PWMSG_HEAD2 head;
	WORD wUserIndex;
	BYTE btExpiredItemCount;
	WORD wItemCode[90];
	UINT64 Serial[90];
};

struct PMSG_ANS_PERIODITEMEX_INSERT {
	PBMSG_HEAD2 head;
	DWORD dwUserGuid;
	WORD wUserIndex;
	BYTE btResultCode;
	BYTE btItemtype;
	WORD wItemCode;
	BYTE btEffectCategory;
	BYTE btEffectType1;
	BYTE btEffectType2;
	UINT64 Serial;
	DWORD dwDuration;
	time_t lBuyDate;
	time_t lExpireDate;
};

struct PMSG_ANS_PERIODITEMEX_LIST_COUNT {
	PBMSG_HEAD2 head;
	WORD wUserIndex;
	BYTE btResult;
	BYTE btListCount;
	DWORD dwUserGuid;
};

struct PMSG_ANS_PERIODITEMEX_LIST {
	PBMSG_HEAD2 head;
	DWORD dwUserGuid;
	WORD wUserIndex;
	BYTE btUsedInfo;
	BYTE btItemType;
	UINT64 Serial;
	time_t lItemBuyDate;
	time_t lItemExpireDate;
	WORD wItemCode;
	BYTE btEffectCategory;
	BYTE btEffectType1;
	BYTE btEffectType2;
};

struct PMSG_ANS_PSHOPITEMVALUE_INFO {
	PWMSG_HEAD h;
	BYTE btItemCnt;
	int iUserIndex;
	PSHOP_ITEMVALUE_INFO_DS PShopItemValueInfo[32];
};

struct PMSG_ANS_QUESTEXP_INFO {
	PWMSG_HEAD head;
	BYTE btQuestCnt;
	int iUserIndex;
};

struct PMSG_ANS_REG_CC_OFFLINE_GIFT {
	struct PBMSG_HEAD h;
	int iINDEX;
	char szUID[11];
	unsigned short wServerCode;
	char szNAME[11];
	int iResultCode;
	char szGIFT_NAME[50];
};

struct PMSG_ANS_REG_DL_OFFLINE_GIFT {
	struct PBMSG_HEAD h;
	int iINDEX;
	char szUID[11];
	unsigned short wServerCode;
	char szNAME[11];
	int iResultCode;
	char szGIFT_NAME[50];
};

struct PMSG_ANS_REG_HT_OFFLINE_GIFT {
	struct PBMSG_HEAD h;
	int iINDEX;
	char szUID[11];
	unsigned short wServerCode;
	char szNAME[11];
	int iResultCode;
	char szGIFT_NAME[50];
};

struct PMSG_ANS_REG_LUCKYCOIN {
	PBMSG_HEAD h;
	int iIndex;
	char szUID[11];
	BYTE Result;
	BYTE Pos;
	int LuckyCoins;
};

struct PMSG_ANS_REG_RINGGIFT {
	struct PBMSG_HEAD h;
	int iINDEX;
	char szUID[11];
	unsigned char btIsRegistered;
	unsigned char btGiftSection;
	unsigned char btGiftKind;
};

struct PMSG_ANS_REG_WANTED_PARTYMEMBER {
	PBMSG_HEAD2 h;
	int nResult;
};

struct PMSG_ANS_REGCASTLESIEGE {
	PBMSG_HEAD2 h;
	BYTE btResult;
	char szGuildName[8];
};

struct PMSG_ANS_REGGUILDMARK {
	PBMSG_HEAD2 h;
	BYTE btResult;
	char szGuildName[8];
	BYTE btGuildMark1;
	BYTE btGuildMark2;
	BYTE btGuildMark3;
	BYTE btGuildMark4;
};

struct PMSG_ANS_REGGUILDMATCHINGDATA {
	PBMSG_HEAD2 h;
	int nResult;
};

struct PMSG_ANS_REGISTER_EVENTCHIP {
	struct PBMSG_HEAD h;
	int iINDEX;
	unsigned char Pos;
	char szUID[11];
	char bSUCCESS;
	short nEVENT_CHIPS;
};

struct PMSG_ANS_REGISTER_STONES {
	struct PBMSG_HEAD h;
	int iINDEX;
	unsigned char iPosition;
	char szUID[11];
	char bSUCCESS;
	int iStoneCount;
};

struct PMSG_ANS_RESET_EVENTCHIP {
	struct PBMSG_HEAD h;
	int iINDEX;
	char szUID[11];
	char bSUCCESS;
};

struct PMSG_ANS_REWARDLIST {
	PWMSG_HEAD2 h;
	BYTE byResult;
	BYTE byCount;
};

struct PMSG_ANS_RUUD_STORE_BUYITEM {
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btPos;
	BYTE btItemInfo[MAX_ITEM_INFO];
};

struct PMSG_ANS_SANTACHECK {
	PBMSG_HEAD2 h;
	char AccountID[11];
	short gGameServerCode;
	int aIndex;
	WORD Result;
	WORD UseCount;
};

struct PMSG_ANS_SANTAGIFT {
	PBMSG_HEAD2 h;
	char AccountID[11];
	short gGameServerCode;
	int aIndex;
	WORD Result;
	WORD UseCount;
};

struct PMSG_ANS_SWITCHWARE {
	PBMSG_HEAD h;
	int iIndex;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	int WarehouseID;
	int Result;
};

struct PMSG_ANS_TAXMONEYINFO {
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btTaxRateChaos;
	BYTE btTaxRateStore;
	BYTE btMoney1;
	BYTE btMoney2;
	BYTE btMoney3;
	BYTE btMoney4;
	BYTE btMoney5;
	BYTE btMoney6;
	BYTE btMoney7;
	BYTE btMoney8;
};

struct PMSG_ANS_TAXRATECHANGE {
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btTaxType;
	BYTE btTaxRate1;
	BYTE btTaxRate2;
	BYTE btTaxRate3;
	BYTE btTaxRate4;
};

struct PMSG_ANS_UBF_ACCOUNT_USERINFO {
	PBMSG_HEAD2 h;
	int iUserIndex;
	BYTE btResult;
	BYTE btRegisterState;
	BYTE btRegisterMonth;
	BYTE btRegisterDay;
	BYTE btObserverMode;
};

struct PMSG_ANS_UBF_CANCEL {
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_ANS_UBF_GET_REWARD {
	PBMSG_HEAD2 h;
	int iUserIndex;
	BYTE btBattleKind;
	BYTE btReturn;
	BYTE btStage1;
	BYTE btStage2;
	BYTE btStage3;
	BYTE btRewardCount1[2];
	BYTE btRewardCount2[2];
	BYTE btRewardCount3[2];
};

struct PMSG_ANS_UBF_INFO {
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_ANS_UBF_JOIN {
	PBMSG_HEAD2 h;
	BYTE btResult;
	WORD nLeftSec;
};

struct PMSG_ANS_UBF_SET_RECEIVED_REWARD {
	PBMSG_HEAD2 h;
	int iUserIndex;
	BYTE btReturn;
};

struct PMSG_ANS_USE_BOX {
	PBMSG_HEAD2 h;
	int iResult;
	int iItemType;
};

struct PMSG_ANS_USE_POPUP {
	PBMSG_HEAD2 h;
	int nPopupType;
};

struct PMSG_ANS_USEWEAPON {
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btObjIndexH;
	BYTE btObjIndexL;
	BYTE btWeaponType;
	BYTE btPointX;
	BYTE btPointY;
};

struct PMSG_ANS_VIEW_EC_MN {
	struct PBMSG_HEAD h;
	int iINDEX;
	char szUID[11];
	char bSUCCESS;
	short nEVENT_CHIPS;
	int iMUTO_NUM;
};

struct PMSG_ANS_VIEW_STONES {
	struct PBMSG_HEAD h;
	int iINDEX;
	char szUID[11];
	char bSUCCESS;
	int iStoneCount;
};

struct PMSG_ANS_WAIT_GUILDMATCHING {
	PWMSG_HEAD2 h;
	int nListCount;
	int nResult;
};

struct PMSG_ANS_WAITLIST_PARTYMATCHING {
	PWMSG_HEAD2 h;
	int nListCnt;
	int nResult;
};

struct PMSG_ANS_WARESAVE {
	PBMSG_HEAD h;
	int iIndex;
	BYTE Result;
	BYTE CloseWindow;
};

struct PMSG_ANTIHACK_BREACH {
	PBMSG_HEAD h;
	BYTE subcode;
	DWORD dwErrorCode;
};

struct PMSG_ANTIHACK_CHECK {
	PBMSG_HEAD h;
	BYTE subcode;
	BYTE checkdata[5];
};

struct PMSG_ATTACK {
	PBMSG_HEAD h;
	BYTE NumberH;
	BYTE NumberL;
	BYTE AttackAction;
	BYTE DirDis;
};

struct PMSG_ATTACKRESULT {
	PBMSG_HEAD h;
	BYTE NumberH;
	BYTE NumberL;
	BYTE DamageH;
	BYTE DamageL;
	BYTE DamageTypeH;
	BYTE DamageTypeL;
	BYTE btShieldDamageH;
	BYTE btShieldDamageL;
	int IGCDamage;
	int ElementalDmg;
};

struct PMSG_ATTACKSPEEDSEND {
	PBMSG_HEAD2 h;
	DWORD AttackSpeed;
	DWORD MagicSpeed;
};

struct PMSG_BEATTACK_COUNT {
	PBMSG_HEAD h;
	BYTE MagicNumberH;
	BYTE Count;
	BYTE MagicNumberL;
	BYTE X;
	BYTE Serial;
	BYTE Y;
};

struct PMSG_BEATTACK {
	BYTE NumberH;
	BYTE MagicKey;
	BYTE NumberL;
};

struct PMSG_BLOCKCHAT_LIST {
	PWMSG_HEAD2 h;
	BYTE btType;
	BYTE btSubType;
	BYTE Count;
	BLOCKCHAT_DATA m_BlockData[20];
};

struct PMSG_BUYREQUEST {
	PBMSG_HEAD h;
	BYTE Pos;
};

struct PMSG_BUYRESULT {
	PBMSG_HEAD h;
	BYTE Result;
	BYTE ItemInfo[MAX_ITEM_INFO];
};

struct PMSG_CALLMONSTER_VIEWPORTCREATE {
	BYTE NumberH;
	BYTE NumberL;
	BYTE Type_HI;
	BYTE Type_LO;
	BYTE X;
	BYTE Y;
	BYTE TX;
	BYTE TY;
	BYTE Path;
	BYTE Id[MAX_ACCOUNT_LEN];
	BYTE PentagramMainAttribute;
	BYTE LevelH;
	BYTE LevelL;
	BYTE MaxLifeHH;
	BYTE MaxLifeHL;
	BYTE MaxLifeLH;
	BYTE MaxLifeLL;
	BYTE CurLifeHH;
	BYTE CurLifeHL;
	BYTE CurLifeLH;
	BYTE CurLifeLL;
	BYTE CharSet[18];
	BYTE SummonType;
	BYTE BuffEffectCount;
	BYTE BuffEffectList[MAX_BUFFEFFECT];
};

struct PMSG_CCF_UI_ONOFF {
	PBMSG_HEAD2 h;
};

struct PMSG_CHAIN_MAGIC_OBJECT {
	WORD nTargetIndex;
};

struct PMSG_CHAIN_MAGIC {
	PBMSG_HEAD2 h;
	BYTE MagicNumberH;
	BYTE MagicNumberL;
	WORD wUserIndex;
	BYTE nCount;
};

struct PMSG_CHAOSMIX {
	PBMSG_HEAD h;
	BYTE Type;
	BYTE Pos;
	BYTE unk1;
	BYTE MixCount;
};

struct PMSG_CHARCHECK_RESULT {
	PBMSG_HEAD2 h;
	char szName[MAX_ACCOUNT_LEN];
	BYTE bIsFromOtherAccount;
};

struct PMSG_CHARCHECK {
	PBMSG_HEAD2 h;
	char szName[MAX_ACCOUNT_LEN];
};

struct PMSG_CHARCREATE {
	PBMSG_HEAD h;
	BYTE subcode;
	char Name[10];
	BYTE ClassSkin;
};

struct PMSG_CHARCREATERESULT {
	PBMSG_HEAD h;
	BYTE subcode;
	unsigned char Result;
	unsigned char Name[10];
	BYTE pos;
	WORD Level;
	BYTE Class;
	BYTE Equipment[24];
};

struct PMSG_CHARDELETE {
	PBMSG_HEAD h;
	BYTE subcode;
	char Name[10];
	char LastJoominNumber[20];
};

struct PMSG_CHARLIST {
	BYTE Index;
	char Name[10];
	WORD Level;
	BYTE CtlCode;
	BYTE CharSet[CHAR_SET_SIZE];
	BYTE btGuildStatus;
};

struct PMSG_CHARLISTCOUNT {
	PBMSG_HEAD h;
	BYTE subcode;
	BYTE MaxClass;
	BYTE MoveCnt;
	BYTE Count;
};

struct PMSG_CHARMAPJOIN {
	PBMSG_HEAD h;
	BYTE subcode;
	char Name[10];
	BYTE btSlotIndex;
};

struct PMSG_CHATDATA_NUMBER {
	PBMSG_HEAD h;
	BYTE NumberH;
	BYTE NumberL;
	char chatmsg[90];
};

struct PMSG_CHATDATA_WHISPER {
	PBMSG_HEAD h;
	char id[10];
	char chatmsg[90];
};

struct PMSG_CHATDATA {
	PBMSG_HEAD h;
	char chatid[10];
	char chatmsg[90];
};

struct PMSG_CHECK_MAINEXE_RESULT {
	PBMSG_HEAD2 h;
	DWORD m_dwKey;
};

struct PMSG_CHECK_MAINEXE {
	PBMSG_HEAD h;
	WORD m_wKey;
};

struct PMSG_CLIENTCLOSE {
	PBMSG_HEAD h;
	BYTE SubCode;
	BYTE Flag;
};

struct PMSG_CLIENTMSG {
	PBMSG_HEAD h;
	BYTE SubCode;
	BYTE Flag;
	BYTE subFlag;
};

struct PMSG_CLIENTTIME {
	PBMSG_HEAD h;
	WORD TimeH;
	WORD TimeL;
	WORD AttackSpeed;
	WORD Agility;
	WORD MagicSpeed;
	char Version[10];
	DWORD ServerSeason;
};

struct PMSG_CSATTKGUILDLIST {
	BYTE btCsJoinSide;
	BYTE btGuildInvolved;
	char szGuildName[8];
	int iGuildScore;
};

struct PMSG_DEL_PENTAGRAMJEWEL {
	PBMSG_HEAD h;
	int iUserGuid;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
	BYTE btJewelPos;
	BYTE btJewelIndex;
};

struct PMSG_DEL_PSHOPITEM {
	PBMSG_HEAD h;
	char AccountId[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
	int nPShopItemInvenNum;
};

struct PMSG_DISABLE_RECONNECT {
	PBMSG_HEAD h;
	BYTE subcode;
	BYTE Trash[100];
};

struct PMSG_DSF_PLAY_STATE {
	PBMSG_HEAD2 h;
	WORD wRemainSec;
	int iTeamPoint[20];
	BYTE btMyTeam;
};

struct PMSG_DSF_RESULT {
	PBMSG_HEAD2 h;
	int iTeamPoint[20];
};

struct PMSG_DSF_STATE {
	PBMSG_HEAD2 h;
	BYTE btDSFState;
};

struct PMSG_DSF_TEAM_USERNAME {
	PWMSG_HEAD2 h;
	int nPartyCount;
	BYTE btResult;
};

struct PMSG_DURATION_MAGIC_RECV {
	PBMSG_HEAD h;
	BYTE X;
	BYTE MagicNumberH;
	BYTE Y;
	BYTE MagicNumberL;
	BYTE Dir;
	BYTE NumberH;
	BYTE Dis;
	BYTE NumberL;
	BYTE TargetPos;
	BYTE MagicKey;
};

struct PMSG_DURATION_MAGIC_SEND {
	PBMSG_HEAD h;
	BYTE X;
	BYTE Y;
	BYTE Dir;
	BYTE MagicNumberH;
	BYTE NumberH;
	BYTE MagicNumberL;
	BYTE NumberL;
};

struct PMSG_ELEMENTALDAMAGE {
	PBMSG_HEAD h;
	BYTE NumberH;
	BYTE NumberL;
	BYTE btElement;
	BYTE btTargetH;
	BYTE btTargetL;
	int Damage;
	int New;
	int New1;
	int New2;
};

struct PMSG_ENTER_ZONE_RESULT {
	PBMSG_HEAD2 h;
	BYTE m_btResult;
	BYTE m_btDayOfWeek;
	BYTE m_btZoneIndex;
	BYTE m_btWeather;
	DWORD m_dwRemainTime;
};

struct PMSG_EQUIPMENTLIST {
	PBMSG_HEAD h;
	BYTE subcode;
	BYTE NumberH;
	BYTE NumberL;
	BYTE Equipment[CHAR_SET_SIZE];
};

struct PMSG_EVENT_INVEN_ITEM_MOD {
	PBMSG_HEAD2 h;
	BYTE Pos;
	BYTE ItemInfo[12];
};

struct PMSG_EVENT {
	PBMSG_HEAD h;
	BYTE State;
	BYTE Event;
};

struct PMSG_EVENTENTRY_NOTICE {
	PBMSG_HEAD2 h;
	BYTE btEventType;
	BYTE btNoticeState;
};

struct PMSG_EVENTINVENTORY_DELETE {
	PBMSG_HEAD2 h;
	BYTE IPos;
	BYTE Flag;
};

struct PMSG_EVENTITEM_DUR {
	PBMSG_HEAD2 h;
	BYTE IPos;
	BYTE Dur;
};

struct PMSG_EVENTITEM_GETREQUEST {
	PBMSG_HEAD2 h;
	BYTE NumberH;
	BYTE NumberL;
};

struct PMSG_EVENTITEM_GETRESULT {
	PBMSG_HEAD2 h;
	BYTE result;
	BYTE index[2];
	BYTE Data[12];
};

struct PMSG_EVENTITEM_THROW_RESULT {
	PBMSG_HEAD2 h;
	unsigned char Result;
	BYTE Ipos;
};

struct PMSG_EVENTITEM_THROW {
	PBMSG_HEAD2 h;
	BYTE px;
	BYTE py;
	BYTE Ipos;
};

struct PMSG_EX_GAMESERVER_MOVE_RESULT {
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_EX_GAMESERVER_MOVE_SEND {
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btGameServerCode;
};

struct PMSG_EX_SKILL_COUNT {
	PBMSG_HEAD h;
	BYTE NumberH;
	BYTE NumberL;
	WORD Type;
	BYTE Count;
};

struct PMSG_EXCHANGE_EVENTCHIP {
	PBMSG_HEAD h;
	BYTE btType;
};

struct PMSG_GETMUTONUMBER {
	PBMSG_HEAD h;
};

struct PMSG_GOALSEND {
	PBMSG_HEAD h;
	BYTE subcode;
	char RedTeamName[8];
	BYTE RedTeamScore;
	char BlueTeamName[8];
	BYTE BlueTeamScore;
};

struct PMSG_GUILD_ASSIGN_STATUS_REQ {
	PBMSG_HEAD h;
	BYTE btType;
	BYTE btGuildStatus;
	char szTagetName[10];
};

struct PMSG_GUILD_ASSIGN_STATUS_RESULT {
	PBMSG_HEAD h;
	BYTE btType;
	BYTE btResult;
	char szTagetName[10];
};

struct PMSG_GUILD_ASSIGN_TYPE_REQ {
	PBMSG_HEAD h;
	BYTE btGuildType;
};

struct PMSG_GUILD_ASSIGN_TYPE_RESULT {
	PBMSG_HEAD h;
	BYTE btGuildType;
	BYTE btResult;
};

struct PMSG_GUILDCREATED_RESULT {
	PBMSG_HEAD h;
	BYTE Result;
	BYTE btGuildType;
};

struct PMSG_GUILDDELUSER {
	PBMSG_HEAD h;
	char Name[10];
	char Password[20];
};

struct PMSG_GUILDINFOSAVE {
	PBMSG_HEAD h;
	BYTE btGuildType;
	char GuildName[8];
	BYTE Mark[32];
};

struct PMSG_GUILDJOINQ {
	PBMSG_HEAD h;
	BYTE NumberH;
	BYTE NumberL;
};

struct PMSG_GUILDMASTERANSWER {
	PBMSG_HEAD h;
	BYTE Result;
};

struct PMSG_GUILDQRESULT {
	PBMSG_HEAD h;
	BYTE Result;
	BYTE NumberH;
	BYTE NumberL;
};

struct PMSG_GUILDVIEWPORT_USER {
	BYTE NumberH;
	BYTE NumberL;
	BYTE GNumberH;
	BYTE GNumberL;
};

struct PMSG_GUILDVIEWPORT {
	BYTE NumberH;
	BYTE NumberL;
	char GuildName[8];
	BYTE Mark[32];
};

struct PMSG_GUILDWARSEND_RESULT {
	PBMSG_HEAD h;
	BYTE Result;
};

struct PMSG_HELLO {
	PBMSG_HEAD h;
	UCHAR result;
};

struct PMSG_IDPASS {
	PBMSG_HEAD h;
	BYTE subcode;
	char Id[10];
	char Pass[20];
	char HWID[100];
	DWORD TickCount;
	BYTE CliVersion[5];
	BYTE CliSerial[16];
	DWORD ServerSeason;
};

struct PMSG_ILLUSION_TEMPLE_USE_MAGIC_RESULT {
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE MagicNumberH;
	BYTE MagicNumberL;
	WORD wSourceObjIndex;
	WORD wTargetObjIndex;
};

struct PMSG_INSERT_PENTAGRAMJEWEL {
	PBMSG_HEAD h;
	int iUserGuid;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
	BYTE btJewelPos;
	BYTE btJewelIndex;
	BYTE btMainAttribute;
	BYTE btItemType;
	WORD wItemIndex;
	BYTE btLevel;
	BYTE btRank1OptionNum;
	BYTE btRank1Level;
	BYTE btRank2OptionNum;
	BYTE btRank2Level;
	BYTE btRank3OptionNum;
	BYTE btRank3Level;
	BYTE btRank4OptionNum;
	BYTE btRank4Level;
	BYTE btRank5OptionNum;
	BYTE btRank5Level;
};

struct PMSG_INVENTORYITEMMODIFY {
	PBMSG_HEAD h;
	BYTE subcode;
	BYTE Pos;
	BYTE ItemInfo[MAX_ITEM_INFO];
};

struct PMSG_INVENTORYITEMMOVE_RESULT {
	PBMSG_HEAD h;
	BYTE result;
	BYTE Pos;
	BYTE ItemInfo[MAX_ITEM_INFO];
};

struct PMSG_INVENTORYITEMMOVE {
	PBMSG_HEAD h;
	BYTE sFlag;
	BYTE source_item_num;
	BYTE sItemInfo[MAX_ITEM_INFO];
	BYTE tFlag;
	BYTE target_item_num;
};

struct PMSG_INVENTORYLIST {
	BYTE Pos;
	BYTE ItemInfo[MAX_ITEM_INFO];
};

struct PMSG_INVENTORYLISTCOUNT {
	PWMSG_HEAD h;
	BYTE subcode;
	BYTE Count;
};

struct PMSG_ITEMDURREPAIR {
	PBMSG_HEAD h;
	BYTE Position;
	BYTE Requestpos;
};

struct PMSG_ITEMGETREQUEST {
	PBMSG_HEAD h;
	BYTE NumberH;
	BYTE NumberL;
};

struct PMSG_ITEMGETRESULT {
	PBMSG_HEAD h;
	BYTE result;
	BYTE i;
	BYTE ii;
	BYTE Data[MAX_ITEM_INFO];
	BYTE x;
	BYTE xx;
};

struct PMSG_ITEMTHROW_RESULT {
	PBMSG_HEAD h;
	unsigned char Result;
	BYTE Ipos;
};

struct PMSG_ITEMTHROW {
	PBMSG_HEAD h;
	BYTE px;
	BYTE py;
	BYTE Ipos;
};

struct PMSG_ITEMVIEWPORTCREATE {
	BYTE NumberH;
	BYTE NumberL;
	BYTE px;
	BYTE py;
	BYTE ItemInfo[MAX_ITEM_INFO];
};

struct PMSG_ITL_GUILD_RANKINFO {
	PWMSG_HEAD2 h;
	BYTE byResult;
	BYTE byCnt;
};

struct PMSG_ITL_PLAY_STATE {
	PBMSG_HEAD2 h;
	WORD wRemainSec;
	BYTE byAlliedPoint;
	BYTE btIllusionPoint;
	BYTE btMyTeam;
};

struct PMSG_ITL_RELICS_GET_USER {
	PBMSG_HEAD2 h;
	WORD wUserIndex;
	char Name[MAX_ACCOUNT_LEN];
	BYTE byGet;
};

struct PMSG_ITL_RESULT {
	PBMSG_HEAD2 h;
	BYTE btUserCount;
	char szAlliedGuildName[MAX_GUILD_LEN + 1];
	char szIllusionGuildName[MAX_GUILD_LEN + 1];
	BYTE byWin;
};

struct PMSG_ITL_SKILL_END {
	PBMSG_HEAD2 h;
	BYTE MagicNumberH;
	BYTE MagicNumberL;
	WORD wObjIndex;
};

struct PMSG_ITL_STATE {
	PBMSG_HEAD2 h;
	BYTE btTempleNumber;
	BYTE btITLState;
};

struct PMSG_ITL_STONEINFO {
	PBMSG_HEAD2 h;
	BYTE byMapTagIndex;
	WORD wOccupiedStoneType;
	BYTE byOccupiedStoneState;
	int nNpcIndex;
};

struct PMSG_ITL_USE_MAGIC_RESULT {
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE MagicNumberH;
	BYTE MagicNumberL;
	WORD wSourceObjIndex;
	WORD wTargetObjIndex;
};

struct PMSG_ITL_USER_ADD_EXP {
	char GameID[MAX_ACCOUNT_LEN];
	BYTE btMapNumber;
	BYTE btTeam;
	BYTE btClass;
	BYTE btUserKillCount;
};

struct PMSG_ITR_RESULT {
	PBMSG_HEAD2 h;
	BYTE btAlliedPoint;
	BYTE btIllusionPoint;
	BYTE btUserCount;
	BYTE byWinTeamIndex;
};

struct PMSG_ITR_USER_ADD_EXP {
	char GameID[MAX_ACCOUNT_LEN];
	BYTE btMapNumber;
	BYTE btTeam;
	BYTE btClass;
	BYTE btUserKillCount;
};

struct PMSG_JOINRESULT {
	PBMSG_HEAD h;
	BYTE scode;
	BYTE result;
	BYTE NumberH;
	BYTE NumberL;
	BYTE CliVersion[5];
};

struct PMSG_KICKOUT_UNIONMEMBER_REQ {
	PBMSG_HEAD2 h;
	char szTargetGuildName[8];
};

struct PMSG_KILLCOUNT {
	PBMSG_HEAD2 h;
	BYTE btKillCount;
};

struct PMSG_KILLPLAYER_EXT {
	PBMSG_HEAD h;
	BYTE NumberH;
	BYTE NumberL;
	WORD ExpH;
	WORD ExpL;
	BYTE DamageH;
	BYTE DamageL;
};

struct PMSG_LEVELUP {
	PBMSG_HEAD h;
	BYTE subcode;
	WORD Level;
	WORD LevelUpPoint;
	WORD MaxLife;
	WORD MaxMana;
	WORD wMaxShield;
	WORD MaxBP;
	short AddPoint;
	short MaxAddPoint;
	short MinusPoint;
	short MaxMinusPoint;
	int IGCMaxLife;
	int IGCMaxMana;
};

struct PMSG_LOGINSEVER_LIVE {
	PBMSG_HEAD h;
	DWORD RQueue;
};

struct PMSG_LUCKYITME_DB_INFO {
	WORD wItemCode;
	UINT64 Serial;
	WORD wDurabilitySmall;
};

struct PMSG_LVPOINTADD {
	PBMSG_HEAD h;
	BYTE subcode;
	BYTE Type;
};

struct PMSG_LVPOINTADDRESULT {
	PBMSG_HEAD h;
	BYTE subcode;
	BYTE ResultType;
	WORD MaxLifeAndMana;
	WORD wMaxShield;
	WORD MaxBP;
	int IGCMaxLifeAndMana;
};

struct PMSG_MAGICATTACK_RESULT {
	PBMSG_HEAD h;
	BYTE SourceNumberH;
	BYTE SourceNumberL;
	BYTE MagicNumberH;
	BYTE MagicNumberL;
	BYTE TargetNumberH;
	BYTE TargetNumberL;
};

struct PMSG_MAGICATTACK {
	PBMSG_HEAD h;
	BYTE NumberH;
	BYTE MagicNumberH;
	BYTE NumberL;
	BYTE MagicNumberL;
	BYTE Dis;
};

struct PMSG_MAGICCANCEL {
	PBMSG_HEAD h;
	BYTE MagicNumberL;
	BYTE MagicNumberH;
	BYTE NumberH;
	BYTE NumberL;
};

struct PMSG_MAGICLIST {
	char Pos;
	WORD wSkillNum;
	BYTE btSkillLevel;
};

struct PMSG_MAGICLISTCOUNT {
	PWMSG_HEAD h;
	BYTE subcode;
	BYTE Count;
	BYTE btListType;
};

struct PMSG_MAPMOVE_CHECKSUM {
	PBMSG_HEAD2 h;
	DWORD dwKeyValue;
};

struct PMSG_MEMBER_POS_INFO_SEND {
	PBMSG_HEAD2 h;
	BYTE btCount;
	stMemberPosInfo stPosInfo[5];
};

struct PMSG_MONEY {
	PBMSG_HEAD h;
	BYTE Result;
	int wMoney;
	int iMoney;
};

struct PMSG_MONSTER_VIEWPORTCREATE {
	BYTE NumberH;
	BYTE NumberL;
	BYTE Type_HI;
	BYTE Type_LO;
	BYTE X;
	BYTE Y;
	BYTE TX;
	BYTE TY;
	BYTE Path;
	BYTE PentagramMainAttribute;
	BYTE LevelH;
	BYTE LevelL;
	BYTE MaxLifeHH;
	BYTE MaxLifeHL;
	BYTE MaxLifeLH;
	BYTE MaxLifeLL;
	BYTE CurLifeHH;
	BYTE CurLifeHL;
	BYTE CurLifeLH;
	BYTE CurLifeLL;
	BYTE BuffEffectCount;
	BYTE BuffEffectList[MAX_BUFFEFFECT];
};

struct PMSG_MOVE_PSHOPITEM {
	PBMSG_HEAD h;
	char AccountId[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
	int nOldPShopItemInvenNum;
	int nNewPShopItemInvenNum;
};

struct PMSG_MOVE {
	PBMSG_HEAD h;
	BYTE X;
	BYTE Y;
	BYTE Path[8];
};

struct PMSG_MUBOT_REQ_START {
	PBMSG_HEAD2 h;
	BYTE btType;
};

struct PMSG_MUBOT_SETTINGS_RECV {
	PWMSG_HEAD h;
	BYTE btType;
	BYTE btDATA[512];
};

struct PMSG_MUBOT_USE {
	PBMSG_HEAD2 h;
	WORD Time;
	DWORD Money;
	BYTE Status;
};

struct PMSG_MUUN_ATTACK_COMMAND {
	PBMSG_HEAD2 h;
	BYTE Slot;
	BYTE SkillType;
	BYTE NumberH;
	BYTE NumberL;
	BYTE TargetNumberH;
	BYTE TargetNumberL;
};

struct PMSG_MUUN_INVEN_ITEM_MOD {
	PBMSG_HEAD2 h;
	BYTE subcode;
	BYTE Pos;
	BYTE ItemInfo[12];
};

struct PMSG_MUUN_INVEN_SUB_EQUIP_OPEN {
	PBMSG_HEAD2 h;
	BYTE Result;
};

struct PMSG_MUUN_ITEM_SELLREQUEST {
	PBMSG_HEAD2 h;
	BYTE Pos;
};

struct PMSG_MUUN_ITEM_SELLRESULT {
	PBMSG_HEAD2 h;
	BYTE Result;
	DWORD Money;
};

struct PMSG_MUUN_RIDE_SELECT {
	PBMSG_HEAD2 h;
	WORD wItemNum;
};

struct PMSG_MUUNITEM_CONDITION_STATUS {
	PBMSG_HEAD2 h;
	BYTE btIPos;
	BYTE btStatus;
};

struct PMSG_MUUNITEM_DUR {
	PBMSG_HEAD2 h;
	BYTE IPos;
	BYTE Dur;
};

struct PMSG_MUUNITEM_GETREQUEST {
	PBMSG_HEAD2 h;
	BYTE NumberH;
	BYTE NumberL;
};

struct PMSG_MUUNITEM_GETRESULT {
	PBMSG_HEAD2 h;
	BYTE result;
	BYTE Data[12];
};

struct PMSG_NOTIFY_MAXUSER {
	PBMSG_HEAD h;
	int iSvrCode;
	int iMaxUserCount;
	int iCurUserCount;
};

struct PMSG_NOTIFY_OBJECT_CREATION_STATE {
	struct PBMSG_HEAD2 h;
	BYTE btObjIndexH;
	BYTE btObjIndexL;
	BYTE btCreationState;
};

struct PMSG_NOTIFY_ZONE_CLEAR {
	PBMSG_HEAD2 h;
	DWORD m_nType;
	DWORD m_nRewardExp;
};

struct PMSG_NOTIFY_ZONE_TIME {
	PBMSG_HEAD2 h;
	BYTE m_btMsgType;
	BYTE m_btDayOfWeek;
	WORD m_wZoneIndex;
	DWORD m_dwRemainTime;
	DWORD m_dwRemainMonster;
};

struct PMSG_OBJECT_LIFE_INFO {
	PBMSG_HEAD2 h;
	BYTE ObjNumberH;
	BYTE ObjNumberL;
	BYTE MaxLifeHH;
	BYTE MaxLifeHL;
	BYTE MaxLifeLH;
	BYTE MaxLifeLL;
	BYTE CurLifeHH;
	BYTE CurLifeHL;
	BYTE CurLifeLH;
	BYTE CurLifeLL;
};

struct PMSG_PACKETCHECKSUM {
	PBMSG_HEAD h;
	BYTE funcindex;
	DWORD CheckSum;
};

struct PMSG_PARTYDELUSER {
	PBMSG_HEAD h;
	BYTE Number;
	char szTargetName[MAX_ACCOUNT_LEN + 1];
};

struct PMSG_PARTYLIST {
	char szId[10];
	BYTE Number;
	BYTE MapNumber;
	BYTE X;
	BYTE Y;
	int Life;
	int MaxLife;
	int nServerChannel;
	int nMana;
	int nMaxMana;
	int Unk;
};

struct PMSG_PARTYLISTCOUNT {
	PBMSG_HEAD h;
	BYTE Result;
	BYTE Count;
};

struct PMSG_PARTYREQUEST {
	PBMSG_HEAD h;
	BYTE NumberH;
	BYTE NumberL;
};

struct PMSG_PARTYREQUESTRESULT {
	PBMSG_HEAD h;
	BYTE Result;
	BYTE NumberH;
	BYTE NumberL;
};

struct PMSG_PENTAGRAM_JEWEL_INOUT {
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_PENTAGRAMJEWEL_INFO {
	PWMSG_HEAD2 h;
	BYTE btResult;
	BYTE btJewelCnt;
	BYTE btJewelPos;
};

struct PMSG_PETINVENTORY_DELETE {
	PBMSG_HEAD2 h;
	BYTE IPos;
	BYTE Flag;
};

struct PMSG_PING_RESULT {
	PBMSG_HEAD h;
};

struct PMSG_PKLEVEL {
	PBMSG_HEAD h;
	BYTE subcode;
	BYTE NumberH;
	BYTE NumberL;
	BYTE PkLevel;
};

struct PMSG_POSISTION_SET {
	PBMSG_HEAD h;
	BYTE X;
	BYTE Y;
};

struct PMSG_POST_DATA {
	PBMSG_HEAD2 h;
	BYTE btColorRGB[3];
	char szServerName[50];
	char szTag[20];
	char szName[MAX_ACCOUNT_LEN];
	char szMessage[MAX_CHAT_LEN];
};

struct PMSG_PRICE_INFO {
	PWMSG_HEAD h;
	BYTE subc;
	BYTE Type;
	BYTE count;
};

struct PMSG_PRICE_ITEM_LIST {
	BYTE Pos;
	UINT64 SellPrice;
	UINT64 BuyPrice;
};

struct PMSG_PROPERTYITEMCOUNT {
	PBMSG_HEAD2 h;
	BYTE btCount;
};

struct PMSG_PROPERTYITEMINFO {
	PBMSG_HEAD2 h;
	short ItemID;
	short Pos;
	int Time;
};

struct PMSG_RAGE_ATTACK_RANGE_ANS {
	PBMSG_HEAD2 h;
	WORD MagicNumber;
	WORD Target[5];
};

struct PMSG_RAGE_ATTACK_RANGE {
	PBMSG_HEAD h;
	BYTE MagicNumberH;
	BYTE MagicNumberL;
	BYTE TargetIndexH;
	BYTE TargetIndexL;
};

struct PMSG_RAGEATTACK_REQ {
	PBMSG_HEAD h;
	BYTE NumberH;
	BYTE MagicNumberH;
	BYTE NumberL;
	BYTE MagicNumberL;
	BYTE Empty;
};

struct PMSG_RAGEATTACK {
	PBMSG_HEAD h;
	BYTE SourceNumberH;
	BYTE SourceNumberL;
	BYTE MagicNumberH;
	BYTE MagicNumberL;
	BYTE TargetNumberH;
	BYTE TargetNumberL;
};

struct PMSG_RANKING_SET {
	PBMSG_HEAD h;
	BYTE btSetType;
	int iRankingType;
	DWORD ValueH;
	DWORD ValueL;
};

struct PMSG_RAREITEMPIRCESEND {
	PWMSG_HEAD2 h;
	BYTE btItemSellPriceDivisor;
	int iJewelOfBlessPrice;
	int iJewelOfSoulPrice;
	int iJewelOfChaosPrice;
	int iJewelOfLifePrice;
	int iJewelOfCreationPrice;
	int iCrestOfMonarchPrice;
	int iLochFeatherPrice;
	int iJewelOfGuardianPrice;
	int iWereRabbitEggPrice;
};

struct PMSG_RECALLMONLIFE {
	PBMSG_HEAD h;
	BYTE subcode;
	BYTE Life;
};

struct PMSG_RECV_CHATDATA_WHISPER {
	PBMSG_HEAD h;
	WORD wMapSvrNum;
	int OriginGSIndex;
	int OriginPlayerIndex;
	char id[10];
	char fromId[10];
	char chatmsg[90];
};

struct PMSG_RECV_DC_OTHER_CHANNEL {
	PBMSG_HEAD2 h;
	char szName[MAX_ACCOUNT_LEN + 1];
	WORD wMapSrvGroup;
};

struct PMSG_RECV_POSISTION_SET {
	PBMSG_HEAD h;
	BYTE NumberH;
	BYTE NumberL;
	BYTE X;
	BYTE Y;
};

struct PMSG_RECV_TRAP {
	PBMSG_HEAD2 h;
	WORD wUserIndex;
	time_t CurrTime;
};

struct PMSG_RECVMOVE {
	PBMSG_HEAD h;
	BYTE NumberH;
	BYTE NumberL;
	BYTE X;
	BYTE Y;
	BYTE Path;
};

struct PMSG_REGEVENTCHIP {
	PBMSG_HEAD h;
	BYTE Type;
	BYTE ChipPos;
};

struct PMSG_RELATIONSHIP_JOIN_BREAKOFF_ANS {
	PBMSG_HEAD h;
	BYTE btRelationShipType;
	BYTE btRequestType;
	BYTE btResult;
	BYTE btTargetUserIndexH;
	BYTE btTargetUserIndexL;
};

struct PMSG_RELATIONSHIP_JOIN_BREAKOFF_REQ {
	PBMSG_HEAD h;
	BYTE btRelationShipType;
	BYTE btRequestType;
	BYTE btTargetUserIndexH;
	BYTE btTargetUserIndexL;
};

struct PMSG_REQ_2ANIV_SERIAL {
	struct PBMSG_HEAD h;
	int iINDEX;
	char szUID[11];
	char SERIAL1[5];
	char SERIAL2[5];
	char SERIAL3[5];
	int iMEMB_GUID;
};

struct PMSG_REQ_2ANV_LOTTO_EVENT {
	PBMSG_HEAD h;
	char SERIAL1[5];
	char SERIAL2[5];
	char SERIAL3[5];
};

struct PMSG_REQ_ACCEPTENTER_ITR {
	PBMSG_HEAD2 h;
	BYTE byEnterOK;
};

struct PMSG_REQ_ACCEPTMEMBER_PARTYMATCHING {
	PBMSG_HEAD2 h;
	char szMemberName[MAX_ACCOUNT_LEN + 1];
	BYTE btType;
};

struct PMSG_REQ_ALLOW_JOIN_GUILDMATCHING {
	PBMSG_HEAD2 h;
	int nAllowType;
	char szName[11];
};

struct PMSG_REQ_ALLOWLIST_GUILDMATCHING {
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_ARCA_BATTLE_JOIN {
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_BLOODCASTLE_ENTERCOUNT {
	struct PBMSG_HEAD2 h;
	char AccountID[10];
	char GameID[10];
	int ServerCode;
	int iObjIndex;
};

struct PMSG_REQ_BUY_PCPOINTSHOP {
	PBMSG_HEAD2 h;
	BYTE Pos;
};

struct PMSG_REQ_CANCEL_GUILDMATCHINGLIST {
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_CANCEL_JOIN_PARTYMATCHING {
	PBMSG_HEAD2 h;
	BYTE btType;
};

struct PMSG_REQ_CASTLESIEGESTATE {
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_CCF_RANKING {
	PBMSG_HEAD2 h;
	BYTE byRankingType;
};

struct PMSG_REQ_CHAOS_MULTIMIX_CHECK {
	PBMSG_HEAD h;
	BYTE btMixType;
	BYTE btMixCount;
};

struct PMSG_REQ_CL_ENTERCOUNT {
	PBMSG_HEAD h;
	BYTE btEventType;
};

struct PMSG_REQ_CLASSDEF {
	PBMSG_HEAD h;
};

struct PMSG_REQ_CRCDATA {
	PBMSG_HEAD2 h;
	DWORD MainExe;
	DWORD IGCDLL;
	DWORD PlayerBMD;
	DWORD HackDB;
	DWORD SkillCRC;
};

struct PMSG_REQ_CRYWOLF_ALTAR_CONTRACT {
	PBMSG_HEAD2 h;
	BYTE btObjIndexH;
	BYTE btObjIndexL;
};

struct PMSG_REQ_CRYWOLF_BENEFIT_PLUS_CHAOSRATE {
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_CRYWOLF_INFO {
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_CSATTKGUILDLIST {
	PBMSG_HEAD h;
};

struct PMSG_REQ_CSCOMMAND {
	PBMSG_HEAD2 h;
	BYTE btTeam;
	BYTE btX;
	BYTE btY;
	BYTE btCommand;
};

struct PMSG_REQ_CSGATEOPERATE {
	PBMSG_HEAD2 h;
	BYTE btOperate;
	BYTE btIndex1;
	BYTE btIndex2;
};

struct PMSG_REQ_CSHUNTZONEENTER {
	PBMSG_HEAD2 h;
	BYTE btHuntZoneEnter;
};

struct PMSG_REQ_CSREGGUILDLIST {
	PBMSG_HEAD h;
};

struct PMSG_REQ_DELETE_STONES {
	struct PBMSG_HEAD h;
	int iINDEX;
	char szUID[11];
	char bSUCCESS;
	int iStoneCount;
};

struct PMSG_REQ_DEVILSQUARE_REMAINTIME {
	PBMSG_HEAD h;
	BYTE hEventType;
	BYTE btItemLevel;
};

struct PMSG_REQ_DSF_ACCEPTENTER {
	PBMSG_HEAD2 h;
	BYTE btEnterOK;
};

struct PMSG_REQ_DSF_CAN_PARTY_ENTER {
	PBMSG_HEAD2 h;
	int iUserIndex;
	char szAccountID1[MAX_ACCOUNT_LEN + 1];
	char szUserName1[MAX_ACCOUNT_LEN + 1];
	char szAccountID2[MAX_ACCOUNT_LEN + 1];
	char szUserName2[MAX_ACCOUNT_LEN + 1];
	BYTE btDSFType;
	int iEnterYear;
	BYTE btEnterMonth;
	BYTE btEnterDay;
};

struct PMSG_REQ_DSF_GET_REWARD {
	PBMSG_HEAD2 h;
	BYTE btDSFType;
};

struct PMSG_REQ_DSF_GO_FINAL_PARTY_INFO {
	PBMSG_HEAD2 h;
	BYTE btDSFType;
};

struct PMSG_REQ_DSF_GO_FINAL_PARTY {
	PBMSG_HEAD2 h;
	BYTE btDSFType;
	int nEnterYear;
	BYTE btEnterMonth;
	BYTE btEnterDay;
};

struct PMSG_REQ_DSF_PARTYRANKRENEW {
	PBMSG_HEAD2 h;
	BYTE btDSFType;
	int nEnterYear;
	BYTE btEnterMonth;
	BYTE btEnterDay;
};

struct PMSG_REQ_DUEL_EXIT {
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_DUEL_INVITE {
	PBMSG_HEAD2 h;
	BYTE NumberH;
	BYTE NumberL;
	char szName[MAX_ACCOUNT_LEN];
};

struct PMSG_REQ_DUEL_JOINCNANNEL {
	PBMSG_HEAD2 h;
	BYTE nChannelId;
};

struct PMSG_REQ_DUEL_LEAVECNANNEL {
	PBMSG_HEAD2 h;
	BYTE nChannelId;
};

struct PMSG_REQ_END_DUEL {
	PBMSG_HEAD h;
};

struct PMSG_REQ_ENTER_DOPPELGANGER {
	PBMSG_HEAD2 h;
	BYTE btPos;
};

struct PMSG_REQ_ENTER_ITR {
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_ENTER_KANTURU_BOSS_MAP {
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_ENTER_ZONE {
	PBMSG_HEAD2 h;
	BYTE btResult;
};

struct PMSG_REQ_EVENTENTRY_INFO {
	PBMSG_HEAD2 h;
	int nUserIndex;
	char szName[MAX_ACCOUNT_LEN + 1];
};

struct PMSG_REQ_EVOMON_MAXSCORE {
	PBMSG_HEAD2 h;
	int nUserIndex;
	char szName[MAX_ACCOUNT_LEN + 1];
};

struct PMSG_REQ_GET_DSF_REWARD {
	PBMSG_HEAD2 h;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	char szUserName[MAX_ACCOUNT_LEN + 1];
	int nServerCode;
	int nUserIndex;
	int nRewardYear;
	BYTE btRewardMonth;
	BYTE btRewardDay;
};

struct PMSG_REQ_GET_PARTYMATCHINGLIST {
	PBMSG_HEAD2 h;
	int nPage;
	char szSearchWord[11];
	BYTE btUseSearchWord;
};

struct PMSG_REQ_GET_REWARD {
	PBMSG_HEAD2 h;
	BYTE btBattleKind;
};

struct PMSG_REQ_GET_UBF_REAL_NAME {
	PBMSG_HEAD2 h;
	int iUserIndex;
	char szUBFName[MAX_ACCOUNT_LEN + 1];
};

struct PMSG_REQ_GIVEUPCASTLESIEGE {
	PBMSG_HEAD2 h;
	BYTE btGiveUp;
};

struct PMSG_REQ_GUILD_PERIODBUFF_DELETE {
	PBMSG_HEAD2 head;
	char btGuildCnt;
	WORD wBuffIndex[5];
};

struct PMSG_REQ_GUILD_PERIODBUFF_INSERT {
	PBMSG_HEAD2 head;
	char szGuildName[MAX_GUILD_LEN + 1];
	WORD wBuffIndex;
	char btEffectType1;
	char btEffectType2;
	DWORD dwDuration;
	time_t lExpireDate;
};

struct PMSG_REQ_GUILDMARK_OF_CASTLEOWNER {
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_GUILDMATCHINGLIST {
	PBMSG_HEAD2 h;
	int nPage;
};

struct PMSG_REQ_GUILDMATCHINGLISTSEARCHWORD {
	PBMSG_HEAD2 h;
	int nPage;
	char szSearchWord[11];
};

struct PMSG_REQ_GUILDREGINFO {
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_GUILDVIEWPORT {
	PBMSG_HEAD h;
	int GuildNumber;
};

struct PMSG_REQ_ILLUSIONTEMPLE_ENTERCOUNT {
	struct PBMSG_HEAD2 h;
	char AccountID[10];
	char GameID[10];
	int ServerCode;
	int iObjIndex;
};

struct PMSG_REQ_IN_PENTAGRAM_JEWEL {
	PBMSG_HEAD2 h;
	int iPentagramPos;
	int iJewelPos;
};

struct PMSG_REQ_ITL_RELATE_RELICS {
	PBMSG_HEAD2 h;
	BYTE NumberH;
	BYTE NumberL;
	WORD wNpcType;
	BYTE byPrePacket;
	BYTE byAct;
};

struct PMSG_REQ_JEWEL_MIX {
	PBMSG_HEAD2 h;
	BYTE btJewelType;
	BYTE btJewelMix;
};

struct PMSG_REQ_JEWEL_UNMIX {
	PBMSG_HEAD2 h;
	BYTE btJewelType;
	BYTE btJewelLevel;
	BYTE btJewelPos;
};

struct PMSG_REQ_JOIN_GUILDMATCHING {
	PBMSG_HEAD2 h;
	int nGuildNumber;
};

struct PMSG_REQ_KANTURU_STATE_INFO {
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_LUCKYCOIN_REGISTER {
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_LUCKYCOIN_TRADE {
	PBMSG_HEAD2 h;
	int iLuckyCoinTradeCount;
};

struct PMSG_REQ_LUCKYCOIN {
	PBMSG_HEAD h;
	int iIndex;
	char szUID[11];
};

struct PMSG_REQ_LUCKYITEM_DELETE {
	PBMSG_HEAD2 head;
	WORD wUserIndex;
	char szCharName[MAX_ACCOUNT_LEN + 1];
	WORD wItemCode;
	UINT64 Serial;
};

struct PMSG_REQ_LUCKYITEM_INSERT_2ND {
	PBMSG_HEAD2 head;
	DWORD dwUserGuid;
	BYTE btItemCnt;
	char szCharName[MAX_ACCOUNT_LEN + 1];
	PMSG_LUCKYITME_DB_INFO LuckyItemDBInfo[5];
};

struct PMSG_REQ_LUCKYITEM_INSERT {
	PBMSG_HEAD2 head;
	DWORD dwUserGuid;
	char szCharName[MAX_ACCOUNT_LEN + 1];
	PMSG_LUCKYITME_DB_INFO LuckyItemDBInfo;
};

struct PMSG_REQ_LUCKYITEM_SELECT {
	PBMSG_HEAD2 head;
	WORD wUserIndex;
	char chCharacterName[MAX_ACCOUNT_LEN + 1];
};

struct PMSG_REQ_MAPMOVE {
	PBMSG_HEAD2 h;
	DWORD dwBlockKey;
	WORD wMapIndex;
	BYTE EventByte;
};

struct PMSG_REQ_MAPSERVERAUTH {
	PBMSG_HEAD2 h;
	char szAccountID[12];
	char szCharName[12];
	int iJoinAuthCode1;
	int iJoinAuthCode2;
	int iJoinAuthCode3;
	int iJoinAuthCode4;
	int iTickCount;
	BYTE btCliVersion[5];
	BYTE btCliSerial[16];
	BYTE EventMap;
};

struct PMSG_REQ_MAPSVRAUTH {
	PBMSG_HEAD h;
	int iIndex;
	char szAccountID[11];
	char szCharName[11];
	unsigned short wDstMapSvrCode;
	int iJoinAuthCode1;
	int iJoinAuthCode2;
	int iJoinAuthCode3;
	int iJoinAuthCode4;
};

struct PMSG_REQ_MAPSVRMOVE {
	PBMSG_HEAD h;
	int iIndex;
	char szAccountID[11];
	char szCharName[11];
	char szPassword[20];
	unsigned char btSecurityLock;
	unsigned int dwSecurityCode;
	unsigned short wCurMapSvrCode;
	unsigned short wDstMapSvrCode;
	unsigned short wMapNumber;
	unsigned char btX;
	unsigned char btY;
};

struct PMSG_REQ_MASTERLEVEL_SKILL {
	PBMSG_HEAD2 h;
	int iMasterLevelSkill;
};

struct PMSG_REQ_MEMBERJOIN_PARTYMATCHINGLIST {
	PBMSG_HEAD2 h;
	char szLeaderName[MAX_ACCOUNT_LEN + 1];
	char szPassword[5];
	BYTE btUsePassword;
	BYTE btUseRandomParty;
};

struct PMSG_REQ_MEMBERJOIN_STATELIST_PARTYMATCHING {
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_MINETWINKLE_REWARD {
	PBMSG_HEAD2 h;
	WORD wUserIndex;
	WORD wTwinkleIndex;
	WORD wTwinkleType;
	BYTE byMapNumber;
	BYTE byIsStopMine;
};

struct PMSG_REQ_MINETWINKLE {
	PBMSG_HEAD2 h;
	WORD wUserIndex;
	WORD wTwinkleIndex;
	WORD wTwinkleType;
	BYTE byMapNumber;
};

struct PMSG_REQ_MINIMAPDATA {
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_MONEYDRAWOUT {
	PBMSG_HEAD2 h;
	BYTE btMoney1;
	BYTE btMoney2;
	BYTE btMoney3;
	BYTE btMoney4;
};

struct PMSG_REQ_MOVE_OTHERSERVER {
	PBMSG_HEAD h;
	char LastJoominNumber[20];
};

struct PMSG_REQ_MOVE_TO_CASTLE_HUNTZONE {
	PBMSG_HEAD2 h;
	int iPrice;
};

struct PMSG_REQ_MOVEBLOODCASTLE {
	PBMSG_HEAD h;
	BYTE iBridgeNumber;
	BYTE iItemPos;
};

struct PMSG_REQ_MOVECHAOSCASTLE {
	PBMSG_HEAD2 h;
	BYTE iChaosCastleNumber;
	BYTE iItemPos;
};

struct PMSG_REQ_MOVEDEVILSQUARE {
	PBMSG_HEAD h;
	BYTE SquareNumber;
	BYTE InvitationItemPos;
};

struct PMSG_REQ_MOVEILLUSIONTEMPLE {
	PBMSG_HEAD2 h;
	BYTE ID;
	BYTE iItemPos;
};

struct PMSG_REQ_MUUN_EXCHANGE {
	PBMSG_HEAD2 h;
	BYTE btSelect;
};

struct PMSG_REQ_NPC_POSITION {
	PBMSG_HEAD2 h;
	BYTE btType;
};

struct PMSG_REQ_NPCBUY {
	PBMSG_HEAD2 h;
	int iNpcNumber;
	int iNpcIndex;
};

struct PMSG_REQ_NPCDBLIST {
	PBMSG_HEAD h;
	BYTE btMonsterCode;
};

struct PMSG_REQ_NPCREPAIR {
	PBMSG_HEAD2 h;
	int iNpcNumber;
	int iNpcIndex;
};

struct PMSG_REQ_NPCUPGRADE {
	PBMSG_HEAD2 h;
	int iNpcNumber;
	int iNpcIndex;
	int iNpcUpType;
	int iNpcUpValue;
};

struct PMSG_REQ_OPEN_PCPOINTSHOP {
	PBMSG_HEAD2 h;
	BYTE Result;
};

struct PMSG_REQ_OUT_PENTAGRAM_JEWEL {
	PBMSG_HEAD2 h;
	int iPentagramPos;
	BYTE btSocketIndex;
};

struct PMSG_REQ_PENTAGRAMJEWEL {
	PBMSG_HEAD h;
	int iUserIndex;
	BYTE btJewelPos;
	int iUserGuid;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
};

struct PMSG_REQ_PERIODBUFF_DELETE {
	PBMSG_HEAD2 head;
	WORD wUserIndex;
	WORD wBuffIndex;
	char szCharacterName[11];
};

struct PMSG_REQ_PERIODBUFF_INSERT {
	PBMSG_HEAD2 head;
	WORD wUserIndex;
	char szCharacterName[MAX_ACCOUNT_LEN + 1];
	WORD wBuffIndex;
	char btEffectType1;
	char btEffectType2;
	DWORD dwDuration;
	time_t lExpireDate;
};

struct PMSG_REQ_PERIODBUFF_SELECT {
	PBMSG_HEAD2 head;
	WORD wUserIndex;
	char szCharacterName[MAX_ACCOUNT_LEN + 1];
};

struct PMSG_REQ_PERIODITEMEX_DELETE {
	PBMSG_HEAD2 head;
	DWORD dwUserGuid;
	WORD wUserIndex;
	UINT64 Serial;
	WORD wItemCode;
	BYTE btItemType;
	char chCharacterName[MAX_ACCOUNT_LEN + 1];
};

struct PMSG_REQ_PERIODITEMEX_INSERT {
	PBMSG_HEAD2 head;
	DWORD dwUserGuid;
	WORD wUserIndex;
	char chCharacterName[MAX_ACCOUNT_LEN + 1];
	BYTE btItemType;
	WORD wItemCode;
	BYTE btEffectCategory;
	BYTE btEffectType1;
	BYTE btEffectType2;
	UINT64 Serial;
	DWORD dwDuration;
	time_t lBuyDate;
	time_t lExpireDate;
};

struct PMSG_REQ_PERIODITEMEX_SELECT {
	PBMSG_HEAD2 head;
	DWORD dwUserGuid;
	WORD wUserIndex;
	char chCharacterName[MAX_ACCOUNT_LEN + 1];
};

struct PMSG_REQ_PSHOPITEMVALUE_INFO {
	PBMSG_HEAD h;
	char AccountId[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
	int iUserIndex;
};

struct PMSG_REQ_QUESTEXP_INFO {
	PBMSG_HEAD h;
	int iUserIndex;
	char szCharName[MAX_ACCOUNT_LEN + 1];
};

struct PMSG_REQ_REFINE_PENTAGRAM_JEWEL {
	PBMSG_HEAD2 h;
	BYTE btRefineKind;
};

struct PMSG_REQ_REG_CC_OFFLINE_GIFT {
	struct PBMSG_HEAD h;
	int iINDEX;
	char szUID[11];
	unsigned short wServerCode;
	char szNAME[11];
};

struct PMSG_REQ_REG_DL_OFFLINE_GIFT {
	struct PBMSG_HEAD h;
	int iINDEX;
	char szUID[11];
	unsigned short wServerCode;
	char szNAME[11];
};

struct PMSG_REQ_REG_HT_OFFLINE_GIFT {
	struct PBMSG_HEAD h;
	int iINDEX;
	char szUID[11];
	unsigned short wServerCode;
	char szNAME[11];
};

struct PMSG_REQ_REG_RINGGIFT {
	struct PBMSG_HEAD h;
	int iINDEX;
	char szUID[11];
	unsigned char btGiftKind;
};

struct PMSG_REQ_REG_WANTED_PARTYMEMBER {
	PBMSG_HEAD2 h;
	char szTitle[41];
	char szPassword[5];
	WORD nMinLevel;
	WORD nMaxLevel;
	WORD nHuntingGround;
	BYTE btWantedClass;
	BYTE btWantedClassDetailInfo[7];
	BYTE btUsePassword;
	BYTE btApprovalType;
};

struct PMSG_REQ_REGCASTLESIEGE {
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_REGGUILDMARK {
	PBMSG_HEAD2 h;
	BYTE btItemPos;
};

struct PMSG_REQ_REGGUILDMATCHINGDATA {
	PBMSG_HEAD2 h;
	char szMemo[41];
	BYTE btInterestType;
	BYTE btLevelRange;
	BYTE btClassType;
};

struct PMSG_REQ_REGISTER_EVENTCHIP {
	struct PBMSG_HEAD h;
	int iINDEX;
	unsigned char Pos;
	char szUID[11];
};

struct PMSG_REQ_REGISTER_LUCKYCOIN {
	PBMSG_HEAD h;
	int iIndex;
	char szAccountID[11];
	BYTE btPos;
};

struct PMSG_REQ_REGISTER_STONES {
	struct PBMSG_HEAD h;
	int iINDEX;
	unsigned char iPosition;
	char szUID[11];
};

struct PMSG_REQ_REPOSUSER_IN_CC {
	PBMSG_HEAD2 h;
	BYTE btPX;
	BYTE btPY;
};

struct PMSG_REQ_REPOSUSER_IN_CCF {
	PBMSG_HEAD2 h;
	BYTE btPX;
	BYTE btPY;
};

struct PMSG_REQ_RESET_EVENTCHIP {
	struct PBMSG_HEAD h;
	int iINDEX;
	char szUID[11];
};

struct PMSG_REQ_RUUD_STORE_BUYITEM {
	PBMSG_HEAD2 h;
	BYTE btPos;
};

struct PMSG_REQ_SANTACHECK {
	PBMSG_HEAD2 h;
	char AccountID[11];
	short gGameServerCode;
	int aIndex;
};

struct PMSG_REQ_SANTAGIFT {
	PBMSG_HEAD2 h;
	char AccountID[11];
	short gGameServerCode;
	int aIndex;
};

struct PMSG_REQ_SAVE_DSF_PARTYPOINT {
	PBMSG_HEAD2 h;
	char szAccountID1[MAX_ACCOUNT_LEN + 1];
	char szUserName1[MAX_ACCOUNT_LEN + 1];
	int nUserLevel1;
	char szAccountID2[MAX_ACCOUNT_LEN + 1];
	char szUserName2[MAX_ACCOUNT_LEN + 1];
	int nUserLevel2;
	int nDSFType;
	int nPoint;
	BYTE btEnterCnt;
	int nEnterYear;
	BYTE btEnterMonth;
	BYTE btEnterDay;
};

struct PMSG_REQ_SAVE_DSF_REWARD_USER {
	PBMSG_HEAD2 h;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	char szUserName[MAX_ACCOUNT_LEN + 1];
	int iClass;
	BYTE btDSFType;
	int nRewardYear;
	BYTE btRewardMonth;
	BYTE btRewardStartDay;
	BYTE btRewardEndDay;
};

struct PMSG_REQ_SAVE_EVOMON_RESULT {
	PBMSG_HEAD2 h;
	int nUserIndex;
	char szName[MAX_ACCOUNT_LEN + 1];
	int nScore;
	int nTotalDamage;
};

struct PMSG_REQ_SET_DSF_WINNER_INFO {
	PBMSG_HEAD2 h;
	char UBFName[MAX_ACCOUNT_LEN + 1];
	BYTE btDSFType;
	BYTE btRewardType;
};

struct PMSG_REQ_SETPENTAGRAMJEWEL {
	PWMSG_HEAD h;
	int iUserIndex;
	int iUserGuid;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
	BYTE btJewelPos;
	BYTE btJewelCnt;
};

struct PMSG_REQ_START_DUEL {
	PBMSG_HEAD h;
	BYTE NumberH;
	BYTE NumberL;
	char szName[10];
};

struct PMSG_REQ_STOPMINIMAPDATA {
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_SWITCHWARE {
	PBMSG_HEAD h;
	int iIndex;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	int WarehouseID;
};

struct PMSG_REQ_TAXMONEYINFO {
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_TAXRATECHANGE {
	PBMSG_HEAD2 h;
	BYTE btTaxType;
	BYTE btTaxRate1;
	BYTE btTaxRate2;
	BYTE btTaxRate3;
	BYTE btTaxRate4;
};

struct PMSG_REQ_UBF_ACCOUNT_USERINFO {
	PBMSG_HEAD2 h;
	int iUserIndex;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
	int iServerCode;
	int IsUnityBattleFieldServer;
	BYTE btObserverMode;
};

struct PMSG_REQ_UBF_CANCEL {
	PBMSG_HEAD2 h;
	BYTE btCanceled;
};

struct PMSG_REQ_UBF_GET_REWARD {
	PBMSG_HEAD2 h;
	int iUserIndex;
	int iServerCode;
	BYTE btBattleKind;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
};

struct PMSG_REQ_UBF_INFO {
	PBMSG_HEAD2 h;
	BYTE btUBFKind;
};

struct PMSG_REQ_UBF_JOIN {
	PBMSG_HEAD2 h;
	BYTE btUBFKind;
};

struct PMSG_REQ_UBF_REAL_NAME {
	PBMSG_HEAD2 h;
	BYTE btRequest;
};

struct PMSG_REQ_UBF_SET_RECEIVED_REWARD {
	PBMSG_HEAD2 h;
	int iUserIndex;
	int iServerCode;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
	BYTE btReceivedReward;
	BYTE btBattleKind;
};

struct PMSG_REQ_UPGRADE_PENTAGRAM_JEWEL {
	PBMSG_HEAD2 h;
	BYTE btUpgradeType;
	BYTE btTargetValue;
};

struct PMSG_REQ_USE_BOX {
	PBMSG_HEAD2 h;
	BYTE btPos;
};

struct PMSG_REQ_USEILLUSIONTEMPLESKILL {
	PBMSG_HEAD2 h;
	BYTE MagicNumberH;
	BYTE MagicNumberL;
	BYTE btTargetObjIndexH;
	BYTE btTargetObjIndexL;
	BYTE btDis;
};

struct PMSG_REQ_USETRANSFORMATIONRING {
	PBMSG_HEAD2 h;
	BYTE btState;
};

struct PMSG_REQ_USEWEAPON {
	PBMSG_HEAD2 h;
	BYTE btObjIndexH;
	BYTE btObjIndexL;
	BYTE btTargetIndex;
};

struct PMSG_REQ_VIEW_EC_MN {
	struct PBMSG_HEAD h;
	int iINDEX;
	char szUID[11];
};

struct PMSG_REQ_WAITLIST_PARTYMATCHING {
	PBMSG_HEAD2 h;
};

struct PMSG_REQ_WEAPON_DAMAGE_VALUE {
	PBMSG_HEAD2 h;
	BYTE btObjIndexH;
	BYTE btObjIndexL;
};

struct PMSG_REQUEST_PET_ITEM_COMMAND {
	PBMSG_HEAD h;
	BYTE PetType;
	BYTE Command;
	BYTE NumberH;
	BYTE NumberL;
};

struct PMSG_REQUEST_PET_ITEMINFO {
	PBMSG_HEAD h;
	BYTE PetType;
	BYTE InvenType;
	BYTE nPos;
};

struct PMSG_RES_EXPEVENTINFO {
	PBMSG_HEAD2 h;
	WORD wPCBangRate;
	WORD wExpRate;
};

struct PMSG_RESULT_CCF_ENTERCHECK {
	PBMSG_HEAD2 h;
	BYTE byResult;
	BYTE byRemainTime;
	BYTE byRemainTime_LOW;
};

struct PMSG_RESULT_MOVEBLOODCASTLE {
	PBMSG_HEAD h;
	BYTE Result;
};

struct PMSG_RESULT {
	PBMSG_HEAD h;
	unsigned char subcode;
	unsigned char result;
};

struct PMSG_RSA_KEY {
	PWMSG_HEAD2 h;
	DWORD m_ESize;
	DWORD m_NSize;
};

struct PMSG_RUUD {
	PBMSG_HEAD2 h;
	int iTotalRuud;
	int iObtainedRuud;
	BYTE btObtainNotiSend;
};

struct PMSG_SELLREQUEST {
	PBMSG_HEAD h;
	BYTE Pos;
};

struct PMSG_SEND_AH_INFO {
	PBMSG_HEAD2 h;
	BYTE type;
	DWORD Agi;
	DWORD MainTick;
};

struct PMSG_SEND_CCF_INFO_ALL_SVR {
	PBMSG_HEAD2 h;
	int nMin;
	int nType;
	WORD wMapSvrNum;
};

struct PMSG_SEND_EVENT_BANNER {
	PBMSG_HEAD2 h;
	int iBannerType;
};

struct PMSG_SEND_HITHACK_INFO {
	PBMSG_HEAD2 h;
	DWORD dwHitHackVal1;
	DWORD dwHitHackVal2;
};

struct PMSG_SEND_MUUN_RIDE_VIEWPORT_INFO {
	PWMSG_HEAD2 h;
	BYTE Count;
};

struct PMSG_SEND_NEWS {
	PWMSG_HEAD2 h;
	BYTE Day;
	BYTE Month;
	WORD year;
	DWORD dateColor;
	DWORD titleColor;
	DWORD textColor;
	char title[40];
	WORD textLen;
};

struct PMSG_SEND_TITLE {
	PBMSG_HEAD2 h;
	char ServerName[12];
};

struct PMSG_SEND_PCPOINT {
	PBMSG_HEAD2 h;
	WORD PCPoint;
	WORD MaxPCPoint;
};

struct PMSG_SERVERCMD {
	PBMSG_HEAD h;
	BYTE subcode;
	BYTE CmdType;
	BYTE X;
	BYTE Y;
};

struct PMSG_SERVERINFO {
	PBMSG_HEAD h;
	short ServerCode;
	BYTE Percent;
	short UserCount;
	short AccountCount;
	short PCbangCount;
	short MaxUserCount;
};

struct PMSG_SERVERLIST_SERVER {
	WORD ServerCode;
	WORD Percent;
};

struct PMSG_SERVERMSG {
	PBMSG_HEAD h;
	BYTE MsgNumber;
};

struct PMSG_SERVERSLIST_COUNT {
	PWMSG_HEAD2 h;
	BYTE CountH;
	BYTE CountL;
};

struct PMSG_SET_CHAT_COLOR {
	PBMSG_HEAD2 h;
	BYTE btInfoMsg[3];
	BYTE btErrorMsg[3];
	BYTE btChatMsg[3];
	BYTE btWhisperMsg[3];
	BYTE btPartyMsg[3];
	BYTE btGuildMsg[3];
	BYTE btGensMsg[3];
	BYTE btAllianceMsg[3];
	BYTE btGMChatMsg[3];
};

struct PMSG_SET_DEVILSQUARE {
	PBMSG_HEAD h;
	BYTE Type;
};

struct PMSG_SET_OFFTRADE {
	PBMSG_HEAD h;
	char szAccountID[11];
	char szName[11];
	bool m_bState;
};

struct PMSG_SETAGILITYBUG {
	PBMSG_HEAD h;
	BYTE subcode;
	BYTE value;
};

struct PMSG_SETCHARSET {
	PBMSG_HEAD h;
	BYTE subcode;
	WORD charset;
};

struct PMSG_SETQUEST {
	PBMSG_HEAD h;
	BYTE QuestIndex;
	BYTE State;
};

struct PMSG_SHOPITEMCOUNT {
	PWMSG_HEAD h;
	BYTE Type;
	BYTE count;
};

struct PMSG_SIMPLE_GUILDVIEWPORT_COUNT {
	PWMSG_HEAD h;
	BYTE Count;
};

struct PMSG_SIMPLE_GUILDVIEWPORT {
	int GuildNumber;
	BYTE btGuildStatus;
	BYTE btGuildType;
	BYTE btGuildRelationShip;
	BYTE NumberH;
	BYTE NumberL;
	BYTE btOwnerStatus;
};

struct PMSG_SKILLKEY {
	PBMSG_HEAD h;
	BYTE subcode;
	BYTE SkillKey[20];
	BYTE GameOption;
	BYTE QkeyDefine;
	BYTE WkeyDefine;
	BYTE EkeyDefine;
	BYTE ChatWindow;
	BYTE RkeyDefine;
	int QWERLevel;
};

struct PMSG_STAT_BONUS {
	PBMSG_HEAD2 h;
	STAT_BONUS m_BonusOption[5];
};

struct PMSG_STATEBLOODCASTLE {
	PBMSG_HEAD h;
	BYTE btPlayState;
	WORD wRemainSec;
	WORD wMaxKillMonster;
	WORD wCurKillMonster;
	WORD wUserHaveWeapon;
	BYTE btWeaponNum;
};

struct PMSG_STATS_RESULT {
	PBMSG_HEAD2 h;
	DWORD CriticalDMGInc;
	DWORD unk1;
	WORD str;
	WORD stradd;
	WORD agi;
	WORD agiadd;
	WORD vit;
	WORD vitadd;
	WORD ene;
	WORD eneadd;
	WORD cmd;
	WORD cmdadd;
	DWORD SDRation;
	DWORD SDBypass;
	float unk4;
	float unk5;
	WORD ZenObincRate;
	float unk7;
	float unk8;
	DWORD unk9;
	float unk10;
	float unk11;
	float unk12;
	float unk13;
	float unk14;
	float unk15;
	float unk16;
	float unk17;
	float unk18;
	float unk19;
	float unk20;
	float unk21;
	float unk22;
	float unk23;
	float unk24;
	float IncreaseDamageRate;
	float unk26;
	float CritDMGRate;
	float ExcDMGRate;
	float DoubleDmgRate;
	BYTE unk28;
	BYTE unk29;
	BYTE unk30;
	float ExcellentDMGInc;
	float ExcellentDMGInc2;
};

struct PMSG_SUMMONER_INFO {
	PBMSG_HEAD2 h;
	BYTE Result;
};

struct PMSG_TALKREQUEST {
	PBMSG_HEAD h;
	BYTE NumberH;
	BYTE NumberL;
};

struct PMSG_TALKRESULT {
	PBMSG_HEAD h;
	unsigned char result;
	BYTE level1;
	BYTE level2;
	BYTE level3;
	BYTE level4;
	BYTE level5;
	BYTE level6;
	BYTE level7;
};

struct PMSG_TARGET_HP_DELETE {
	WORD m_Index;
};

struct PMSG_TARGET_HP_INFO {
	WORD m_Index;
	DWORD m_Hp;
	DWORD m_MaxHp;
	WORD m_Level;
	bool m_bIceEffect;
	bool m_bPoisonEffect;
};

struct PMSG_TARGET_HP {
	PBMSG_HEAD2 h;
	WORD m_Index;
	DWORD m_Hp;
	DWORD m_MaxHp;
	WORD m_Level;
	bool m_bIceEffect;
	bool m_bPoisonEffect;
};

struct PMSG_TARGET_TELEPORT {
	PBMSG_HEAD h;
	BYTE NumberH;
	BYTE NumberL;
	BYTE MapX;
	BYTE MapY;
};

struct PMSG_TELEPORT_RESULT {
	PBMSG_HEAD h;
	BYTE Unk;
	WORD Type;
	BYTE MapNumber;
	BYTE Unk1;
	BYTE MapX;
	BYTE MapY;
	BYTE Dir;
};

struct PMSG_TELEPORT {
	PBMSG_HEAD h;
	WORD MoveNumber;
	BYTE MapX;
	BYTE MapY;
};

struct PMSG_TIMEVIEW {
	PBMSG_HEAD h;
	BYTE subcode;
	WORD Second;
};

struct PMSG_TRADE_GOLD {
	PBMSG_HEAD h;
	DWORD Money;
};

struct PMSG_TRADE_OKBUTTON {
	PBMSG_HEAD h;
	BYTE Flag;
};

struct PMSG_TRADE_REQUEST {
	PBMSG_HEAD h;
	BYTE NumberH;
	BYTE NumberL;
};

struct PMSG_TRADE_RESPONSE {
	PBMSG_HEAD h;
	BYTE Response;
	BYTE Id[10];
	WORD Level;
	int GuildNumber;
};

struct PMSG_UBF_ACCOUNT_USER_COPY_PETITEM {
	PWMSG_HEAD h;
	char szAccountID[MAX_ACCOUNT_LEN];
	int iUserIndex;
	BYTE btCount;
	int IsUnityBattleFieldServer;
	int ServerCode;
};

struct PMSG_UBF_ACCOUNT_USER_COPY_RESULT {
	PBMSG_HEAD2 h;
	int iUserIndex;
	BYTE btResult;
	BYTE btSubResult;
};

struct PMSG_UBF_ACCOUNT_USER_COPY {
	PBMSG_HEAD2 h;
	int iUserIndex;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
	WORD ServerCode;
	BYTE btPromotionCode;
};

struct PMSG_UBF_ANS_CANCEL_REGISTER_USER {
	PBMSG_HEAD2 h;
	int iUserIndex;
	BYTE btCanceledResult;
	BYTE btDeletedResult;
};

struct PMSG_UBF_COPY_PETITEM {
	BYTE btItemPos;
	UINT64 i64ItemSerial;
};

struct PMSG_UBF_REGISTER_ACCOUNT_USER_RESULT {
	PBMSG_HEAD2 h;
	int iUserIndex;
	BYTE btResult;
	WORD nLeftSec;
};

struct PMSG_UBF_REGISTER_ACCOUNT_USER {
	PBMSG_HEAD2 h;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
	char szBattleFieldName[MAX_ACCOUNT_LEN + 1];
	int iUserIndex;
	WORD ServerCode;
	BYTE btRegisterState;
	BYTE btRegisterMonth;
	BYTE btRegisterDay;
};

struct PMSG_UBF_REQ_CANCEL_REGISTER_USER {
	PBMSG_HEAD2 h;
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
	int iUserIndex;
	WORD ServerCode;
	BYTE btCanceled;
};

struct PMSG_UNIONLIST_REQ {
	PBMSG_HEAD h;
};

struct PMSG_UPDATE_PSHOPITEMVALUE_INFO {
	PWMSG_HEAD h;
	BYTE btItemCnt;
	char AccountId[MAX_ACCOUNT_LEN + 1];
	char szName[MAX_ACCOUNT_LEN + 1];
	PSHOP_ITEMVALUE_INFO_DS PShopItemValueInfo[32];
};

struct PMSG_USE_STAT_FRUIT {
	PBMSG_HEAD h;
	BYTE result;
	WORD btStatValue;
	BYTE btFruitType;
};

struct PMSG_USEEXPANSIONITEM {
	PBMSG_HEAD h;
	BYTE Result;
};

struct PMSG_USEITEM_MUUN_INVEN_RESULT {
	PBMSG_HEAD2 h;
	BYTE btResult;
	BYTE btItemUseType;
};

struct PMSG_USEITEM_MUUN_INVEN {
	PBMSG_HEAD2 h;
	BYTE inventoryPos;
	BYTE invenrotyTarget;
	BYTE btItemUseType;
};

struct PMSG_USEITEM {
	PBMSG_HEAD h;
	BYTE inventoryPos;
	BYTE invenrotyTarget;
	BYTE btItemUseType;
};

struct PMSG_USER_MUUN_EQUIPMENT_CHANGED {
	PBMSG_HEAD2 h;
	BYTE NumberH;
	BYTE NumberL;
	BYTE ItemInfo[12];
};

struct PMSG_USEREQUIPMENTCHANGED {
	PBMSG_HEAD h;
	BYTE NumberH;
	BYTE NumberL;
	BYTE ItemInfo[MAX_ITEM_INFO];
	BYTE Element;
};

struct PMSG_VIEWPORTCREATE_CHANGE {
	BYTE NumberH;
	BYTE NumberL;
	BYTE X;
	BYTE Y;
	BYTE SkinH;
	BYTE SkinL;
	char Id[MAX_ACCOUNT_LEN];
	BYTE TX;
	BYTE TY;
	BYTE DirAndPkLevel;
	BYTE CharSet[18];
	BYTE PentagramMainAttribute;
	BYTE MuunItemH;
	BYTE MuunItemL;
	BYTE unk;
	BYTE MuunSubItemH;
	BYTE MuunSubItemL;
	BYTE unk1;
	BYTE MuunRideItemH;
	BYTE MuunRideItemL;
	BYTE unk2;
	BYTE LevelH;
	BYTE LevelL;
	BYTE MaxLifeHH;
	BYTE MaxLifeHL;
	BYTE MaxLifeLH;
	BYTE MaxLifeLL;
	BYTE CurLifeHH;
	BYTE CurLifeHL;
	BYTE CurLifeLH;
	BYTE CurLifeLL;
	WORD nServerCodeOfHomeWorld;
	BYTE BuffEffectCount;
	BYTE BuffEffectList[MAX_BUFFEFFECT];
	BYTE unk3;
};

struct PMSG_VIEWPORTCREATE {
	BYTE NumberH;
	BYTE NumberL;
	BYTE X;
	BYTE Y;
	BYTE CharSet[18];
	char Id[MAX_ACCOUNT_LEN];
	BYTE TX;
	BYTE TY;
	BYTE DirAndPkLevel;
	BYTE PentagramMainAttribute;
	BYTE MuunItemH;
	BYTE MuunItemL;
	BYTE unk;
	BYTE MuunSubItemH;
	BYTE MuunSubItemL;
	BYTE unk1;
	BYTE MuunRideItemH;
	BYTE MuunRideItemL;
	BYTE unk2;
	BYTE LevelH;
	BYTE LevelL;
	BYTE MaxLifeHH;
	BYTE MaxLifeHL;
	BYTE MaxLifeLH;
	BYTE MaxLifeLL;
	BYTE CurLifeHH;
	BYTE CurLifeHL;
	BYTE CurLifeLH;
	BYTE CurLifeLL;
	WORD nServerCodeOfHomeWorld;
	BYTE BuffEffectCount;
	BYTE BuffEffectList[MAX_BUFFEFFECT];
	BYTE unk3;
};

struct PMSG_VIEWPORTDESTROY {
	BYTE NumberH;
	BYTE NumberL;
};

struct PMSG_WAREHOUSEMONEYINOUT {
	PBMSG_HEAD h;
	BYTE Type;
	int Money;
};

struct PMSG_WAREHOUSEPASSSEND {
	PBMSG_HEAD h;
	BYTE Type;
	short Pass;
	char LastJoominNumber[20];
};

struct PSHOP_ITEMVALUE_INFO_DS {
	int nPShopItemInvenNum;
	UINT64 ItemSerial;
	int nMoney;
	WORD sBlessJewelValue;
	WORD sSoulJewelValue;
	WORD sChaosJewelValue;
};

struct PWMSG_COUNT {
	PWMSG_HEAD h;
	BYTE count;
};

struct PBMSG_COUNT2 {
	PBMSG_HEAD2 h;
	BYTE count;
};

struct Recv_PetItem_Info {
	BYTE nPos;
	UINT64 nSerial;
	BYTE Level;
	UINT64 Exp;
};

struct REQUEST_CMD {
	int m_aIndex;
	BYTE m_btData[1024];
	REQUEST_CMD() {
		std::ZeroMemory(this, sizeof(*this));
	}
};

struct Request_PetItem_Info {
	BYTE nPos;
	UINT64 nSerial;
};

struct Save_PetItem_Info {
	UINT64 nSerial;
	BYTE Level;
	UINT64 Exp;
};

struct SDHP_ANS_CCF_GETPERMISSION {
	PBMSG_HEAD2 h;
	int nResult;
	int nIndex;
};

struct SDHP_ANS_CCF_INFO_NOTIFY {
	PBMSG_HEAD2 h;
	int nMin;
	int nType;
};

struct SDHP_ANS_LOAD_MINESYSTEM_UPT_USERINFO {
	PBMSG_HEAD2 h;
	WORD wUserIndex;
	BYTE byResult;
	char szCharName[MAX_ACCOUNT_LEN + 1];
	WORD wTwinkleType;
	int iCurrentStage;
};

struct SDHP_ANS_MINESYSTEM_UPT_USERINFO {
	PBMSG_HEAD2 h;
	WORD wUserIndex;
	BYTE byRequestType;
	BYTE Result;
};

struct SDHP_BILLKILLUSER {
	struct PBMSG_HEAD h;
	char Id[10];
	short Number;
	bool ForceDisconnect;
};

struct SDHP_BILLSEARCH_RESULT {
	PBMSG_HEAD h;
	char Id[10];
	short Number;
	unsigned char cCertifyType;
	unsigned char PayCode;
	char EndsDays[12];
	int EndTime;
	unsigned char cVipType;
};

struct SDHP_BILLSEARCH {
	PBMSG_HEAD h;
	char Id[10];
	short Number;
};

struct SDHP_CHARACTER_TRANSFER_RESULT {
	PBMSG_HEAD h;
	char Account[10];
	short Number;
	unsigned char Result;
};

struct SDHP_CHARACTER_TRANSFER {
	PBMSG_HEAD h;
	char Account[10];
	short Number;
};

struct SDHP_CHARDELETE {
	PBMSG_HEAD h;
	short Number;
	char AccountID[11];
	char Name[11];
	BYTE Guild;
	char GuildName[8];
};

struct SDHP_CHARDELETERESULT {
	PBMSG_HEAD h;
	unsigned char Result;
	short Number;
	char AccountID[11];
};

struct SDHP_CHARLIST {
	BYTE Index;
	char Name[11];
	char UnityBFRealName[11];
	WORD ServerCodeOfHomeWorld;
	WORD Level;
	BYTE Class;
	BYTE CtlCode;
	BYTE dbInventory[48];
	BYTE btGuildStatus;
	WORD Resets;
	BYTE PK_Level;
};

struct SDHP_CHARLISTCOUNT {
	PWMSG_HEAD h;
	short Number;
	BYTE Count;
	int DbNumber;
	int SecurityCode;
	BYTE EnableCharacterCreate;
	char AccountId[11];
	BYTE MoveCnt;
	BYTE CharacterSlotCount;
	BYTE WhExpansion;
};

struct SDHP_COMMAND_BLOCK {
	PBMSG_HEAD h;
	short Number;
	char Id[11];
	int UserNumber;
	int DBNumber;
	unsigned char BlockCode;
};

struct SDHP_CREATECHAR {
	PBMSG_HEAD h;
	int UserNumber;
	int DBNumber;
	short Number;
	char AccountId[11];
	char Name[11];
	BYTE ClassSkin;
};

struct SDHP_CREATECHARRESULT {
	PBMSG_HEAD h;
	unsigned char Result;
	short Number;
	char AccountId[11];
	char Name[11];
	BYTE Pos;
	BYTE ClassSkin;
	BYTE Equipment[24];
	WORD Level;
};

struct SDHP_DBCHAR_INFORESULT {
	PWMSG_HEAD h;
	unsigned char result;
	short Number;
	char AccountID[11];
	char Name[11];
	BYTE Class;
	short Level;
	short mLevel;
	int LevelUpPoint;
	int mlPoint;
	UINT64 Exp;
	UINT64 NextExp;
	UINT64 mlExp;
	UINT64 mlNextExp;
	int Money;
	WORD Str;
	WORD Dex;
	WORD Vit;
	WORD Energy;
	DWORD Life;
	DWORD MaxLife;
	DWORD Mana;
	DWORD MaxMana;
	BYTE dbInventory[7584];
	BYTE dbMagicList[450];
	BYTE MapNumber;
	BYTE MapX;
	BYTE MapY;
	BYTE Dir;
	int PkCount;
	int PkLevel;
	int PkTime;
	BYTE CtlCode;
	DWORD GmCode;
	WORD GmExpDays;
	BYTE AccountCtlCode;
	BYTE dbQuest[100];
	WORD Leadership;
	WORD ChatLitmitTime;
	int iFruitPoint;
	int resets;
	char MarryName[11];
	char Married;
	char InventoryExpansion;
	char WarehouseExpansion;
	int WinDuels;
	int LoseDuels;
	int Ruud;
	time_t ChatBlockTime;
	DWORD PenaltyMask;
};

struct SDHP_DBCHAR_INFOSAVE {
	PWMSG_HEAD h;
	char AccountID[11];
	char Name[11];
	short Level;
	short mLevel;
	BYTE Class;
	int LevelUpPoint;
	int mlPoint;
	UINT64 Exp;
	UINT64 mlExp;
	UINT64 NextExp;
	UINT64 mlNextExp;
	int Money;
	WORD Str;
	WORD Dex;
	WORD Vit;
	WORD Energy;
	DWORD Life;
	DWORD MaxLife;
	DWORD Mana;
	DWORD MaxMana;
	BYTE dbInventory[7584];
	BYTE dbMagicList[450];
	BYTE MapNumber;
	BYTE MapX;
	BYTE MapY;
	BYTE Dir;
	int PkCount;
	int PkLevel;
	int PkTime;
	BYTE dbQuest[100];
	BYTE CharInfoSave;
	WORD Leadership;
	WORD ChatLitmitTime;
	int iFruitPoint;
	int resets;
	char MarryName[11];
	char Married;
	int WinDuels;
	int LoseDuels;
	time_t ChatBlockTime;
	DWORD PenaltyMask;
	int Ruud;
};

struct SDHP_DBCHAR_ITEMSAVE {
	PWMSG_HEAD h;
	char Name[11];
	BYTE dbInventory[7584];
};

struct SDHP_DBCHARINFOREQUEST {
	PBMSG_HEAD h;
	char AccountID[11];
	char Name[11];
	short Number;
};

struct SDHP_DELETE_TEMPUSERINFO_RESULT {
	PBMSG_HEAD h;
	char CharName[MAX_ACCOUNT_LEN + 1];
};

struct SDHP_DELETE_TEMPUSERINFO {
	PBMSG_HEAD h;
	unsigned short ServerCode;
	char CharName[MAX_ACCOUNT_LEN + 1];
};

struct SDHP_EVENTCONTROL {
	PBMSG_HEAD h;
	unsigned char Event;
	unsigned char Stat;
};

struct SDHP_EVENTSTART {
	PBMSG_HEAD h;
	unsigned char Event;
};

struct SDHP_EXITMSG {
	PBMSG_HEAD h;
	unsigned char ExitCode[3];
};

struct SDHP_EXPANDEDWAREHOUSE_SET {
	PBMSG_HEAD h;
	char AccountID[11];
	BYTE ExpansionType;
	BYTE ExpansionLevel;
};

struct SDHP_GETCHARLIST {
	PBMSG_HEAD h;
	char Id[10];
	short Number;
	int IsUnityBattleFieldServer;
};

struct SDHP_GETWAREHOUSEDB_RESULT {
	PBMSG_HEAD h;
	char AccountID[11];
	short aIndex;
	BYTE WarehouseID;
};

struct SDHP_GETWAREHOUSEDB_SAVE {
	PWMSG_HEAD h;
	char AccountID[11];
	short aIndex;
	int Money;
	BYTE dbItems[7680];
	short pw;
	BYTE WarehouseID;
	BYTE CloseWindow;
};

struct SDHP_GETWAREHOUSEDB {
	PBMSG_HEAD h;
	char AccountID[11];
	short aIndex;
};

struct SDHP_GUILDALL_COUNT {
	PWMSG_HEAD h;
	int Number;
	char GuildName[9];
	char Master[11];
	unsigned char Mark[32];
	int score;
	BYTE btGuildType;
	int iGuildUnion;
	int iGuildRival;
	char szGuildRivalName[9];
	unsigned char Count;
};

struct SDHP_GUILDALL {
	char MemberID[11];
	BYTE btGuildStatus;
	short pServer;
};

struct SDHP_GUILDCREATE_RESULT {
	PBMSG_HEAD h;
	unsigned char Result;
	unsigned char Flag;
	unsigned long GuildNumber;
	unsigned char NumberH;
	unsigned char NumberL;
	char Master[11];
	char GuildName[9];
	unsigned char Mark[32];
	unsigned char btGuildType;
};

struct SDHP_GUILDCREATE {
	PBMSG_HEAD h;
	char GuildName[9];
	char Master[11];
	BYTE Mark[32];
	BYTE NumberH;
	BYTE NumberL;
	BYTE btGuildType;
};

struct SDHP_GUILDCREATED {
	PBMSG_HEAD h;
	unsigned short Number;
	char GuildName[9];
	char Master[11];
	unsigned char Mark[32];
	int score;
};

struct SDHP_GUILDDESTROY_RESULT {
	PBMSG_HEAD h;
	unsigned char Result;
	unsigned char Flag;
	unsigned char NumberH;
	unsigned char NumberL;
	char GuildName[9];
	char Master[11];
};

struct SDHP_GUILDDESTROY {
	PBMSG_HEAD h;
	BYTE NumberH;
	BYTE NumberL;
	char GuildName[8];
	char Master[10];
};

struct SDHP_GUILDLISTSTATE {
	PBMSG_HEAD h;
	unsigned char State;
	int Count;
};

struct SDHP_GUILDMEMBER_INFO_GUILDNAME_REQUEST {
	PBMSG_HEAD h;
	char szGuildName[MAX_GUILD_LEN + 1];
};

struct SDHP_GUILDMEMBER_INFO_REQUEST {
	PBMSG_HEAD h;
	BYTE NumberH;
	BYTE NumberL;
	char MemberID[10];
};

struct SDHP_GUILDMEMBER_INFO {
	PBMSG_HEAD h;
	char GuildName[9];
	char MemberID[11];
	unsigned char btGuildStatus;
	unsigned char btGuildType;
	short pServer;
};

struct SDHP_GUILDMEMBERADD_RESULT_WITHOUT_USERINDEX {
	PBMSG_HEAD h;
	unsigned char Result;
	unsigned char Flag;
	char GuildName[9];
	char MemberID[11];
	short pServer;
	short Number;
};

struct SDHP_GUILDMEMBERADD_RESULT {
	PBMSG_HEAD h;
	unsigned char Result;
	unsigned char Flag;
	unsigned char NumberH;
	unsigned char NumberL;
	char GuildName[9];
	char MemberID[11];
	short pServer;
};

struct SDHP_GUILDMEMBERADD_WITHOUT_USERINDEX {
	PBMSG_HEAD h;
	char GuildName[9];
	char MemberID[11];
};

struct SDHP_GUILDMEMBERADD {
	PBMSG_HEAD h;
	char GuildName[9];
	char MemberID[11];
	BYTE NumberH;
	BYTE NumberL;
};

struct SDHP_GUILDMEMBERDEL_RESULT {
	PBMSG_HEAD h;
	unsigned char Result;
	unsigned char Flag;
	unsigned char NumberH;
	unsigned char NumberL;
	char GuildName[9];
	char MemberID[11];
};

struct SDHP_GUILDMEMBERDEL {
	PBMSG_HEAD h;
	BYTE NumberH;
	BYTE NumberL;
	char GuildName[8];
	char MemberID[10];
};

struct SDHP_GUILDNOTICE {
	PBMSG_HEAD h;
	char GuildName[9];
	char szGuildNotice[60];
};

struct SDHP_GUILDSCOREUPDATE {
	PBMSG_HEAD h;
	char GuildName[9];
	int Score;
};

struct SDHP_GUILDUPDATE {
	PBMSG_HEAD h;
	char GuildName[9];
	char Master[11];
	unsigned char Mark[32];
	int Score;
	unsigned char Count;
};

struct SDHP_IDPASS {
	PBMSG_HEAD h;
	char Id[10];
	char Pass[20];
	short Number;
	char HWID[100];
	char IpAddress[17];
};

struct SDHP_IDPASSRESULT {
	PBMSG_HEAD h;
	unsigned char result;
	short Number;
	char Id[10];
	int UserNumber;
	int DBNumber;
	char JoominNumber[20];
};

struct SDHP_ITEMCREATE {
	PBMSG_HEAD h;
	BYTE x;
	BYTE y;
	BYTE MapNumber;
	short Type;
	BYTE Level;
	BYTE Dur;
	BYTE Op1;
	BYTE Op2;
	BYTE Op3;
	BYTE NewOption;
	int aIndex;
	int lootindex;
	BYTE SetOption;
	time_t lDuration;
	BYTE SocketOption[5];
	BYTE MainAttribute;
};

struct SDHP_ITEMCREATERECV {
	PBMSG_HEAD h;
	BYTE x;
	BYTE y;
	BYTE MapNumber;
	UINT64 m_Number;
	short Type;
	BYTE Level;
	BYTE Dur;
	BYTE Op1;
	BYTE Op2;
	BYTE Op3;
	BYTE NewOption;
	int aIndex;
	int lootindex;
	BYTE SetOption;
	time_t lDuration;
	BYTE SocketOption[5];
	BYTE MainAttribute;
};

struct SDHP_JOINFAIL {
	PBMSG_HEAD h;
	short Number;
	char Id[11];
	int UserNumber;
	int DBNumber;
};

struct SDHP_LOVEHEARTCREATE {
	PBMSG_HEAD h;
	unsigned char x;
	unsigned char y;
	unsigned char MapNumber;
};

struct SDHP_LOVEHEARTEVENT_RESULT {
	PBMSG_HEAD h;
	unsigned char Result;
	char Name[10];
	int Number;
};

struct SDHP_LOVEHEARTEVENT {
	PBMSG_HEAD h;
	char Account[10];
	char Name[10];
};

struct SDHP_NOTICE {
	PBMSG_HEAD h;
	char Notice[61];
};

struct SDHP_OPTIONCONTROL {
	PBMSG_HEAD h;
	unsigned char Option;
};

struct SDHP_OTHERJOINMSG {
	struct PBMSG_HEAD h;
	char AccountID[10];
};

struct SDHP_RECV_PETITEM_INFO {
	PWMSG_HEAD h;
	char AccountID[11];
	WORD Number;
	BYTE InvenType;
	BYTE nCount;
};

struct SDHP_RENEW_RANKING {
	PBMSG_HEAD2 h;
	BYTE byRankingType;
};

struct SDHP_REQ_CCF_RANKING {
	PBMSG_HEAD2 h;
	BYTE byRankingType;
	int nServerCategory;
};

struct SDHP_REQ_DBMUUN_INVEN_LOAD {
	PBMSG_HEAD h;
	char AccountID[11];
	char Name[11];
	WORD aIndex;
};

struct SDHP_REQ_LOAD_MINESYSTEM_UPT_USERINFO {
	PBMSG_HEAD2 h;
	WORD wUserIndex;
	char szCharName[MAX_ACCOUNT_LEN + 1];
};

struct SDHP_REQ_MINESYSTEM_UPT_USERINFO {
	PBMSG_HEAD2 h;
	WORD wUserIndex;
	char szCharName[MAX_ACCOUNT_LEN + 1];
	WORD wTwinkleType;
	int iCurrentStage;
	BYTE byRequestType;
};

struct SDHP_REQ_SET_CC_WINNER_INFO_UBF {
	PBMSG_HEAD2 h;
	char UBFName[MAX_ACCOUNT_LEN + 1];
	BYTE btRewardType;
};

struct SDHP_REQ_SET_CCF_WINNER_INFO {
	PBMSG_HEAD2 h;
	char UBFName[MAX_ACCOUNT_LEN + 1];
	BYTE btCCFType;
	BYTE btRewardType;
};

struct SDHP_REQ_SET_EXGAMESERVERCODE {
	PBMSG_HEAD h;
	char szCharName[MAX_ACCOUNT_LEN + 1];
	short sExGameServerCode;
};

struct SDHP_REQUEST_PETITEM_INFO {
	PWMSG_HEAD h;
	char AccountID[11];
	WORD Number;
	BYTE InvenType;
	BYTE nCount;
	WORD ServerType;
	WORD ServerCode;
};

struct SDHP_SAVE_PETITEM_INFO {
	PWMSG_HEAD h;
	BYTE nCount;
	WORD ServerType;
	WORD ServerCode;
};

struct SDHP_SKILLKEYDATA_SEND {
	PBMSG_HEAD h;
	int aIndex;
	char Name[11];
	BYTE SkillKeyBuffer[20];
	BYTE GameOption;
	BYTE QkeyDefine;
	BYTE WkeyDefine;
	BYTE EkeyDefine;
	BYTE ChatWindow;
	BYTE RkeyDefine;
	int QWERLevelDefine;
	BYTE EnableTransformMode;
};

struct SDHP_SKILLKEYDATA {
	PBMSG_HEAD h;
	char Name[11];
	BYTE SkillKeyBuffer[20];
	BYTE GameOption;
	BYTE QkeyDefine;
	BYTE WkeyDefine;
	BYTE EkeyDefine;
	BYTE ChatWindow;
	BYTE RkeyDefine;
	int QWERLevelDefine;
	BYTE EnableTransformMode;
};

struct SDHP_USER_NOTICE {
	PBMSG_HEAD h;
	char szId[10];
	char Notice[61];
};

struct SDHP_USERCLOSE_ID {
	PBMSG_HEAD h;
	char szId[10];
	char szName[10];
	unsigned short Level;
	unsigned char DbClass;
};

struct SDHP_USERCLOSE {
	PBMSG_HEAD h;
	char CharName[10];
	char GuildName[8];
	unsigned char Type;
};

struct SDHP_VIPINFORECV {
	PBMSG_HEAD h;
	int iIndex;
	char AccountId[11];
	char Name[11];
};

struct SDHP_VIPINFORESULT {
	PBMSG_HEAD h;
	int iIndex;
	char AccountId[11];
	char Name[11];
	BYTE VipType;
};

struct SDHP_WAREHOUSEMONEY_SAVE {
	PBMSG_HEAD h;
	char AccountID[11];
	short aIndex;
	int Money;
};

struct SECLOCK_REQ_SAVE {
	PBMSG_HEAD h;
	char AccountID[11];
	int Code;
};

struct sellItem {
	DWORD dwItemGuid;
	DWORD dwItemCODE;
	DWORD dwCategoryID;
	DWORD dwUseType;
	DWORD dwBuyType;
	DWORD dwCoolTime;
	double dbVarEndDate;
	DWORD dwLimitSellCount;
	DWORD dwState;
	char szItemName[64];
	char szItemDesc[128];
	DWORD dwPriceCount;
	sellItemPrice itemPrice[8];
};

struct sellItemPrice {
	DWORD dwPriceGuid;
	DWORD dwItemGuid;
	DWORD dwUseTime;
	DWORD dwAmount;
	DWORD dwPrice;
	DWORD dwSellRate;
};

struct sellPackage {
	DWORD dwPackageGuid;
	sellItemPrice itemPrice;
	DWORD dwImageCode;
	DWORD dwCategory;
	DWORD dwBuyType;
	DWORD dwLimitSellCount;
	DWORD dwInItemCount;
	double dbVarEndDate;
	DWORD dwState;
	sellItemPrice inItemPrices[16];
	char szPackageName[64];
	char szPackageDesc[128];
};

struct ServerInfo {
	WORD UserCount;
	WORD MaxUserCount;
	WORD Percent;
	DWORD TickCount;
};

struct ServerList {
	ServerList() {
		this->btServerCode = 0;
		this->wServerPort = 0;
		memset(this->cIp, 0, 16);
	}
	BYTE btServerCode;
	WORD wServerPort;
	char cIp[16];
};

struct STAT_BONUS {
	WORD BonusOptionID;
	WORD BonusOptionVal1;
	WORD BonusOptionVal2;
};

struct stMemberPosInfo {
	char szName[MAX_ACCOUNT_LEN + 1];
	BYTE btMapNumber;
	BYTE btPosX;
	BYTE btPosY;
};

typedef struct GUILD_INFO_STRUCT {
	int m_iNumber;
	char m_szGuildName[9];
	char m_szGuildMaster[11];
	BYTE m_btGuildType;
	MAP_GUILD_MEMBER m_mapGuildMember;
	int m_iRivalGuild;
	int m_iUnionGuild;
	BYTE m_Mark[32];
	int m_Score;
	char m_Notice[128];
	BOOL m_bGSHasData;
	GUILD_INFO_STRUCT() {
		m_iNumber = -1;
		memset(m_szGuildName, 0, sizeof(m_szGuildName));
		memset(m_szGuildMaster, 0, sizeof(m_szGuildMaster));
		m_btGuildType = -1;
		memset(m_Mark, 0, sizeof(m_Mark));
		memset(m_Notice, 0, sizeof(m_Notice));
		m_iRivalGuild = 0;
		m_iUnionGuild = 0;
		m_Score = 0;
		m_mapGuildMember.clear();
		m_bGSHasData = FALSE;
	}
} *LPGUILD_INFO_STRUCT;

struct JOIN_SERVER_SERVER_DATA {
	int m_ServerIndex;
	char m_szServerName[50];
	WORD m_wServerCode;
	WORD m_wPort;
	WORD m_wCurrentUser;
	WORD m_wMaxUser;
	BYTE m_VipServer;
	WORD m_MaxHWIDUseCount;
};

struct JOIN_SERVER_USER_DATA {
	char m_AccoundID[11];
	char m_HWID[100];
	WORD m_ServerCode;
	WORD m_ServerGroup;
	bool m_bRequestMapSvrMove;
	int iUserNumber;
	DWORD dwPlayTime;
	char m_Ip[17];
	int iPayCode;
	int iUsedTime;
	char szEndDays[9];
	DWORD dwCheckTime;
	int iIndex;
	bool m_bOffTrade;
	char IpAddr[17];
};

typedef struct UNION_MEMBER_DATA {
	std::vector<int> m_vecUnionMember;
} UNION_MEMBER_DATA, *UNION_MEMBER_DATA*;

struct USER_CONNECT_DATA
{
	PMSG_ANS_MAPSVRMOVE pMapServerMoveData;
	int iUserNumber;
	DWORD dwTick;
	BYTE btSecurityLock;
	DWORD dwSecurityCode;
	char szPassword[20];
};


struct USERWAREHOUSE_DATA {
	char szAccountID[MAX_ACCOUNT_LEN + 1];
	int WarehouseID;
	DWORD LastChangeTick;
	int ChangeIDEnableState;
	bool WarehouseOpenState;
};


#endif // _PROTOCOLSTRUCTS_H


