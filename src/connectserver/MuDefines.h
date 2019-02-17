#ifndef _MU_MUDEFINE_H_
#define _MU_MUDEFINE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define CONNECTSERVER_VERSION "12.1"

// Resourse.h
#define IDC_MYICON                      2
#define IDD_IGCCS_DIALOG                102
#define IDS_APP_TITLE                   103
#define IDD_ABOUTBOX                    103
#define IDM_ABOUT                       104
#define IDD_DIALOGBAR                   104
#define IDM_EXIT                        105
#define IDI_IGCCS                       107
#define IDI_SMALL                       108
#define IDC_IGCCS                       109
#define IDR_MAINFRAME                   128
#define IDC_EDIT1                       1002
#define ID_RELOAD_SERVERS               32771
#define ID_RELOAD_NEWS                  32772
#define ID_RELOAD_AUTOUPDATE            32773
#define WM_AUTH_CLIENT_MSG_PROC         32774
#define ID_RELOAD_ANTIFLOODOPTIONS      32775
#define IDC_STATIC                      -1


// CQueue
#define MAX_BUFF_SIZE		100000
#define MAX_NODE		50000
#define SPIN_COUNT		4000

// Map.
#define MAX_NUMBER_MAP 115
#define MAX_MAP_RANGE(x) (((x)<0)?FALSE:((x)>MAX_NUMBER_MAP-1)?FALSE:TRUE)

// Quest Exp Define.
#define MAX_QUESTEXP_ASK_INFO				5
#define MAX_QUESTEXP_REWARD_INFO			5
#define MAX_QUESTEXP_EPISODE				25
#define MAX_QUESTEXP_SWITCH					255
#define MAX_QUESTEXP_INFO					MAX_QUESTEXP_EPISODE
#define MAX_QUESTEXP_USER_INFO				MAX_QUESTEXP_EPISODE
#define MAX_QUESTEXP_REWARDKIND_RANDOM		5
#define MAX_QUESTEXP_ACTIVE_QUEST			10

// Guild.
#define MAX_USER_GUILD			80
#define MAX_GUILD_NOTICE_LEN	60
#define MAX_WC_GUILD_NOTICE_LEN	(MAX_GUILD_NOTICE_LEN/2)
#define MIN_GUILD_COUNT_FOR_CS	20

// Member Type
// -------------------------------
#define GUILD_MASTER		0x80
#define GUILD_ASSISTANT		0x40
#define GUILD_BATTLE_MASTER	0x20
#define GUILD_NORMAL_USER	0x00

#define GUILD_RELATION_UNION	0x01
#define GUILD_RELATION_RIVAL	0x02

// ITEMS.
#define MAX_TYPE_PLAYER 8 
#define MAX_ITEM_LEVEL	15
// ItemAttribute Begin:9439368 END 9446B68 Array[512]

// ***************************
// Item Limits
// **************************
#define MAX_TYPE_ITEMS 16
#define MAX_SUBTYPE_ITEMS 512

#define MAX_EXOPTION_SIZE 8

// ********************
// MACRO for get Item
// ********************
#define ITEMGET(x,y) ( (x)*MAX_SUBTYPE_ITEMS + (y))

// **************
// Pet Level
// **************
#define MAX_PET_LEVEL 70

#define MAX_ITEM_DURABILITY	255

#define MAX_ITEMS (MAX_TYPE_ITEMS*MAX_SUBTYPE_ITEMS)

#define CHECK_ITEM(x) (((x)<0)?-1:((x)>=MAX_ITEMS)?-1:x)

#define MAX_ITEM_SPECIAL_ATTRIBUTE 7

#define DBI_GET_TYPE(x) ( ((x)&((MAX_TYPE_ITEMS-1)<<8))>>4 )
#define DBI_GET_INDEX(x) ( (x) & 0xFF )
#define DBI_GET_LEVEL(x)  ( ((x)>>3)& 0xF )
#define DBI_GET_SKILL(x)  ( ((x) >> 7) & 0x01)
#define DBI_GET_LUCK(x)  ( ((x) >> 2) & 0x01)
#define DBI_GET_OPTION(x)  ( ( (x) ) & 0x03 )
#define DBI_GET_OPTION16(x) ( (x) & 0x40  )
#define DBI_GET_DINO_OPTION(x)  ( DBI_GET_OPTION16((x)) >> 4 )
#define DBI_GET_NOPTION(x)  ( (x) & 0x3F  )
#define DBI_GET_380OPTION(x) ( ((x) & 0x08) << 4  )
#define ITEM_GET_TYPE(x)  ( (x)/MAX_SUBTYPE_ITEMS  )
#define ITEM_GET_INDEX(x) ( (x)%MAX_SUBTYPE_ITEMS  )

#define DBI_TYPE			0
#define DBI_OPTION_DATA		1
#define DBI_DUR				2
#define DBI_SERIAL1			3
#define DBI_SERIAL2			4
#define DBI_SERIAL3			5
#define DBI_SERIAL4			6
#define DBI_NOPTION_DATA	7
#define DBI_SOPTION_DATA	8
#define DBI_OPTION380_DATA	9
#define DBI_JOH_DATA		10
#define DBI_SOCKET_1		11
#define DBI_SOCKET_2		12
#define DBI_SOCKET_3		13
#define DBI_SOCKET_4		14
#define DBI_SOCKET_5		15
#define DBI_SERIAL5			16
#define DBI_SERIAL6			17
#define DBI_SERIAL7			18
#define DBI_SERIAL8			19

#define MAX_ITEM_INFO		12
#define MAX_DBITEM_INFO		32

#define I_TYPE		0
#define I_OPTION	1
#define I_DUR		2
#define I_NOPTION	3
#define I_SOPTION	4
#define I_380OPTION	5
#define I_JOHOPTION	6
#define I_SOCKET1	7
#define I_SOCKET2	8
#define I_SOCKET3	9
#define I_SOCKET4	10
#define I_SOCKET5	11

#define MAX_SOCKET_OPTION 5

#define ITEMTYPE_COMMON		0
#define ITEMTYPE_NORMAL		1
#define ITEMTYPE_SOCKET		2
#define ITEMTYPE_380		3
#define ITEMTYPE_LUCKY		4
#define ITEMTYPE_EVENT		5
#define ITEMTYPE_ANGEL		6
#define ITEMTYPE_CHAOS		7



#define MAX_INFLUENCE_TYPE		3


#define MAX_JOH_ITEM_TYPE	4
#define MAX_JOH_ITEM_INDEX	16
#define MAX_JOH_ITEM_OPTION	16


#define KALIMA_FLOORS 7
#define MAX_KALIMA_ENTER 5
#define KALIMA_GATE_TIMEOUT 180000

#define KALIMA_MAP_RANGE(x) ( ((x)<MAP_INDEX_KALIMA1)?FALSE:((x)>MAP_INDEX_KALIMA6)?(((x)==MAP_INDEX_KALIMA7)?TRUE:FALSE):TRUE   )
#define KALIMA_FLOOR_RANGE(x) (  (x<0)?FALSE:(x>KALIMA_FLOORS-1)?FALSE:TRUE )
#define KALIMA_ATTRIBUTE_RANGE(x) ( ((x)<51)?FALSE:((x)>58)?FALSE:TRUE  )


#define FILE_CUSTOM_MONSTERSTATINFO		"IGC_AddMonsterStats.xml"


#define MAX_LARGE_RAND_STATE	625


#define MAX_SETITEM_OPTION	80
#define OPTION_TABLE_SIZE	6
#define FULL_OPTION_TABLE_SIZE	5


#define EL_NONE			0
#define EL_FIRE			1
#define EL_WATER		2
#define EL_EARTH		3
#define EL_WIND			4
#define EL_DARKNESS		5

#define IN_INVENTORY	0
#define IN_WAREHOUSE	1
#define IN_TRADE		2
#define IN_PSHOP		3

#define EL_SETOPTION_NONE	0
#define EL_SETOPTION_ADD_DAMAGE 1
#define EL_SETOPTION_ADD_DEFENSE 2
#define EL_SETOPTION_ADD_CRITICALDMG 3
#define EL_SETOPTION_ADD_FROM_NORMAL_DEFENSE 4
#define EL_SETOPTION_ADD_FROM_NORMAL_DAMAGE 5
#define EL_SETOPTION_ADD_RESIST_AGAINST_ELEMENT 6
#define EL_SETOPTION_ADD_RESIST_AGAINST_ATTACK 7


#define PENTAGRAM_MIX_UPGRADE_FAILED				0xE1
#define PENTAGRAM_MIX_REFINE_FAILED					0xE2
#define PENTAGRAM_MIX_FAILED_WITH_TALISMAN			0xE3
#define PENTAGRAM_MIX_REFINE_SUCCESS_NOTFOUND		0xF8
#define PENTAGRAM_MIX_NOT_ENOUGH_ZEN				0xF9
#define PENTAGRAM_MIX_ITEMS_COUNT_MISMATCH			0xFA
#define PENTAGRAM_MIX_ATTRIBUTE_MISMATCH			0xFB
#define PENTAGRAM_MIX_REFINE_NOTFOUND				0xFC
#define PENTAGRAM_MIX_PSHOP_OPEN					0xFD
#define PENTAGRAM_MIX_ALREADY_OPEN					0xFE

#define CHECK_RANGE(x,y) (((x)<0)?0:((x)>=(y))?0:1)
#define MAX_PENTAGRAM_ELEMENTAL_ATTRIBUTE 5


#define MAX_EXCELLENT_COMMON_OPTIONS 20
#define MAX_EXCELLENT_WING_OPTIONS 40
#define MAX_EXC_OPTION_EFFECT 7
#define MAX_ITEM_EXC_OPTION 9


#define MAX_ITEM_ADD_OPTION	100

#define ADD_OPTION_SPEED	1
#define ADD_OPTION_ATTACK_DAMAGE	2
#define ADD_OPTION_DEFENSE	3
#define ADD_OPTION_LIFE	4
#define ADD_OPTION_MANA	5
#define ADD_OPTION_EXPERIENCE	6
#define ADD_OPTION_DROP_RATE	7
#define ADD_OPTION_DISABLE_EXPERIENCE 8
#define ADD_OPTION_STRENGTH	9
#define ADD_OPTION_DEXTERITY	10
#define ADD_OPTION_VITALITY	11
#define ADD_OPTION_ENERGY	12
#define ADD_OPTION_LEADERSHIP	13
#define ADD_OPTION_FORCE	14
#define ADD_OPTION_MAGIC	15
#define ADD_OPTION_MOVEABLE	16
#define ADD_OPTION_DARKLORD_CRITICALDMG 17
#define ADD_OPTION_DOT 18
#define ADD_OPTION_POISON 19
#define ADD_OPTION_ICE 20
#define ADD_OPTION_WIZARDDEFENSE 21
#define ADD_OPTION_MAGICDEF 22
#define ADD_OPTION_REDUCE_DEFENSE 23
#define ADD_OPTION_REFLECT 24
#define ADD_OPTION_SLEEP 25
#define ADD_OPTION_BLIND 26
#define ADD_OPTION_WIZARDDEFENSE2 28
#define ADD_OPTION_WEAKNESS 29
#define ADD_OPTION_INNOVATION 30
#define ADD_OPTION_BERSERKERMANA 31
#define ADD_OPTION_BERSERKERHP 32
#define ADD_OPTION_MAGICCIRCLE 33
#define ADD_OPTION_MAGICCIRCLE_STRENGTH 34
#define ADD_OPTION_ALLDAMAGE 51
#define ADD_OPTION_ALLDEFENSE 52
#define ADD_OPTION_LIFE_RECOVERY 53
#define ADD_OPTION_MANA_RECOVERY 54
#define ADD_OPTION_CRITICALDMG 55
#define ADD_OPTION_EXCDMG 56
#define ADD_OPTION_SANTA_DMG 57
#define ADD_OPTION_SANTA_DEF 58
#define ADD_OPTION_VULCANEXP 60
#define ADD_OPTION_VULCANDROP 61
#define ADD_OPTION_GUARDIAN 62
#define ADD_OPTION_ITEMPROTECT 63
#define ADD_OPTION_ZEN_DROP 64
#define ADD_OPTION_SD	66
#define ADD_OPTION_AG	67
#define ADD_OPTION_PARTYEXP 68
#define ADD_OPTION_VITALITY2 69
#define ADD_OPTION_ADDBP 70
#define ADD_OPTION_INCSTAT 71
#define ADD_OPTION_WEAKEN 72
#define ADD_OPTION_INNOVATE 73
#define ADD_OPTION_FIRE 74
#define ADD_OPTION_FROST 75
#define ADD_OPTION_TORNADO 76
#define ADD_OPTION_BIND 77
#define ADD_OPTION_DARKNESS 78

#define ADD_OPTION_MINWIZDMGPERC		90
#define ADD_OPTION_ADDAGRECOVERY		91
#define ADD_OPTION_IGNOREDEF			101
#define ADD_OPTION_ALLSTATS				103
#define ADD_OPTION_ABSORBHP				104
#define ADD_OPTION_DECDEFRATE			105
#define ADD_OPTION_DECATTACKRATE		106


#define MAX_GREMORYCASE_STORAGE_TYPES 2
#define MAX_GREMORYCASE_STORAGE_ITEMS 50

#define MAX_PLAYER_EQUIPMENT 12	// need to redo agin aññ with this macro
#define TEST_FIX_MUUN 0

#define TEST_FIX_DARKRAVEN 1

#define ENABLE_SEASON12 1
#define OBJ_STARTUSERINDEX 1
#define OBJMAX MAXINT

// #ifndef MAX_ACCOUNT_LEN
#define MAX_ACCOUNT_LEN	10
#define MAX_GUILD_LEN	8
//#endif


#define	MAX_LUCKYINFO_LEVELRATE			16
#define MAX_LUCKYINFO_ITEMEQUIPMENT		8
#define	MAX_LUCKYINFO_SMELTRATE			10

#define MAX_CHAT_LEN 90

#define MAX_CANCEL_ITEMS_SALE	5


#define MAX_ITEM_IN_SHOP 120
#define MAX_ITEM_INFO 12
#define MAX_DBITEM_INFO 32

#define MAX_ITEM_IN_SHOP 120

#define MAX_MONSTER_SEND_MSG	20
#define MAX_MONSTER_SEND_ATTACK_MSG 100
#define MAX_MONSTER_TYPE 1024
#define MAX_GUILD_LEN 8
#define MAX_CHARNAME_LEN 10
#define TRADE_BOX_SIZE 32
#define MAGIC_SIZE 150
#define INVETORY_WEAR_SIZE	12
#define INVENTORY_BAG_START INVETORY_WEAR_SIZE
#define TRADE_BOX_MAP_SIZE 32
#define PSHOP_SIZE 32
#define PSHOP_MAP_SIZE 32
#define PSHOP_START_RANGE 204
#define PSHOP_END_RANGE 236
#define MAX_SELF_DEFENSE 5
#define MAX_ST_HIT_DAMAGE 40
#define MAX_ZEN 2000000000
#define MAX_WAREHOUSE_ZEN	1000000000
#define ACTIVE_ITEM_SEARCH 0
#define CHAOS_BOX_SIZE 32
#define CHAOS_BOX_MAP_SIZE 32
#define MAX_BUFFEFFECT	32
#define MAX_MUUN_EFFECT_LIST 2

#define INVENTORY_SIZE				237	//-> 236
#define WAREHOUSE_SIZE				240	//-> 240
#define WAREHOUSE_BUFF				4096
#define MAIN_INVENTORY_SIZE			204	//-> 204
#define INVENTORY_MAP_SIZE			225	//-> 224
#define MAX_INVENTORY_H				24
#define MAX_INVENTORY_H2			28
#define MAX_INVENTORY_LEN			192
#define MAX_WAREHOUSE_H				30
#define MUUN_INVENTORY_SIZE			62
#define EVENT_INVENTORY_SIZE		32
#define EVENT_INVENTORY_MAP_SIZE	32

#define MAX_CS_GUILDLIST 100
#define MAX_CS_NPC 200
#define PMHC_WORD 0xC2
#define MAX_IDSTRING 10

#define MAX_USER		1500
#define MAX_IO_THREAD_HANDLES	16
#define HIDE_ERRORL10
#define MAX_EXSENDBUF_SIZE 14048
#define MAX_IO_BUFFER_SIZE		32768

// User
#define MAX_RESISTENCE_TYPE 7

#define R_ICE			0
#define R_POISON		1
#define R_LIGHTNING		2
#define R_FIRE			3
#define R_EARTH			4
#define R_WIND			5
#define R_WATER			6

#define OBJ_EMPTY -1
#define OBJ_MONSTER 2
#define OBJ_USER 1
#define OBJ_NPC	3
#define MAX_PARTY_LEVEL_DIFFERENCE	130
#define MAX_MAGIC 150
#define MAX_VIEWPORT 75
#define MAX_ROAD_PATH_TABLE 16
#define MAX_ARRAY_FRUSTRUM 4
#define PLAYER_EMPTY  0
#define PLAYER_CONNECTED 1
#define PLAYER_LOGGED 2
#define PLAYER_PLAYING 3
#define MAX_CHECKSUM_KEY 1024
#define FRIEND_SERVER_STATE_LOGIN_FAIL	0
#define FRIEND_SERVER_STATE_OFFLINE		1
#define FRIEND_SERVER_STATE_ONLINE		2

#define CURRENT_DB_VERSION	3


#define CRYWOLF_MAP_RANGE(map) ( ( ((map)) == MAP_INDEX_CRYWOLF_FIRSTZONE  )?TRUE:FALSE  )
#define IMPERIAL_MAP_RANGE(mapnumber) ( ((mapnumber) < MAP_INDEX_IMPERIAL1)?FALSE:((mapnumber) > MAP_INDEX_IMPERIAL4 )?FALSE:TRUE )
#define DG_MAP_RANGE(mapnumber) ( ((mapnumber) < MAP_INDEX_DOPPELGANGER1)?FALSE:((mapnumber) > MAP_INDEX_DOPPELGANGER4 )?FALSE:TRUE )
#define IT_MAP_RANGE(value) ( (( (value)   )<MAP_INDEX_ILLUSIONTEMPLE1)?FALSE:(( (value)  )>MAP_INDEX_ILLUSIONTEMPLE6)?FALSE:TRUE  )
#define ITL_MAP_RANGE(mapnumber) ( ((mapnumber) < MAP_INDEX_ILLUSIONTEMPLE7)?FALSE:((mapnumber) > MAP_INDEX_ILLUSIONTEMPLE8 )?FALSE:TRUE )


#define INVENTORY_FULL_RANGE(x) (((x)<0)?0:((x)>=MAIN_INVENTORY_SIZE)?(((x)==236)?1:0):1)
#define MAIN_INVENTORY_RANGE(x) (((x)<0)?FALSE:((x)>MAIN_INVENTORY_SIZE-1)?FALSE:TRUE )
#define INVENTORY_RANGE(x) (((x)<0)?FALSE:((x)>INVENTORY_SIZE-1)?FALSE:TRUE )
#define EVENT_INVENTORY_RANGE(x) (((x)<0)?FALSE:((x)>EVENT_INVENTORY_SIZE-1)?FALSE:TRUE )
#define WAREHOUSE_RANGE(x) (((x)<0)?FALSE:((x)>WAREHOUSE_SIZE-1)?FALSE:TRUE )
#define TRADE_BOX_RANGE(x) (((x)<0)?FALSE:((x)>TRADE_BOX_SIZE-1)?FALSE:TRUE )
#define OBJMON_RANGE(x) ( ((x)<0)?FALSE:((x)>g_ConfigRead.server.GetObjectMaxMonster()-1)?FALSE:TRUE)
#define PSHOP_RANGE(x) ( (((x)-MAIN_INVENTORY_SIZE)<0)?FALSE:(((x)-MAIN_INVENTORY_SIZE)>(INVENTORY_SIZE-MAIN_INVENTORY_SIZE-1))?FALSE:TRUE )
#define ATTRIBUTE_RANGE(x) (((x)<51)?FALSE:((x)>58)?FALSE:TRUE )

/*
*	CHAR SET
*
*	It Contains the Character Settings
*	CS_CLASS(0)	Contains the Class and the ChangeUP
*				You can use CS_GET_CLASS and CS_GET_CHANGEUP
*	MAP of CHAR SET
*
*	    0         1			2		  3			4		  5			6		  7
*	0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
*	AAAB   CC EEEE EEEE GGGG GGGG JJJJ MMMM PPPP SSSS VVVV WWWW XXXX XXXX YYYY YYYY
*
*	    8         9			10		  11		12		  13		14		  15
*	0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
*	ZZZZ ZZZZ ILOR U    aaaa aaad bbbb bbbc DDDD    e FFFF HHHH KKKK NNNN QQQQ TTTT
*
*		16		  17
*	0000 0000 0000 0000
*	       gf
*
*	AAA Contain Class Character Definition (DK:1 DW:0 ELF:2 MG:3 DL:4)
*	B Contains the Change UP (0:Normal 1:Evolved)
*	CC Contains a value Acording m_ActionNumber fron gObj
*
*	DDDD Contains the 8..11 bits of Rigth Hand Item Index
*	EEEE EEEE Contains the 0..7 bits Rigth Hand Item Index
*
*	FFFF Contains the 8..11 bits of Left Hand Item Index
*	GGGG GGGG Contains the 0..7 bits of Left Hand Item Index
*
*	HHHH Contains the 5..8 Bits from Helmet Item Index
*	I Contains the 4 bit from Helmet Item Index
*	JJJJ Contains the 0..3 bits from Helmet Item Index
*
*	KKKK Contains the 5..8 Bits from Armor Item Index
*	L Contains the 4 bit from Armor Item Index
*	MMMM Contains the 0..3 bits from Armor Item Index
*
*	NNNN Contains the 5..8 Bits from Pants Item Index
*	O Contains the 4 bit from Pants Item Index
*	PPPP Contains the 0..3 bits from Pants Item Index
*
*	QQQQ Contains the 5..8 Bits from Gloves Item Index
*	R Contains the 4 bit from Gloves Item Index
*	SSSS Contains the 0..3 bits from Gloves Item Index
*
*	TTTT Contains the 5..8 Bits from Boots Item Index
*	U Contains the 4 bit from Boots Item Index
*	VVVV Contains the 0..3 bits from Boots Item Index
*
*	WWWW Contains Wings and Helper
*
*	XXXX XXXX Contains Levels (Shine Levels) from Wings and Boots
*	YYYY YYYY Contains Levels (Shine Levels) from Gloves and Pants and Armor
*	ZZZZ ZZZZ Contains Levels (Shine Levels) from Helmet , Right and Left HAnd
*
*	aaaa aaa Contains the If the item is Excellent
*
*	bbbb bbb Contains the If the item is SetItem
*	c	1 if the Char have the Full Set Item
*
*	d Unkown
*
*	e Have Dark Horse?
*
*	f	Fenrir Inc Attack Damage
*	g	Fenrir Dec Attack Damage
*/
#define CHAR_SET_SIZE		18

#define CS_CLASS		0

#define CS_WEAPON1_TYPE	1
#define CS_WEAPON2_TYPE	2

#define CS_WEAPON1_DATA	12
#define CS_WEAPON2_DATA	13

#define CS_HELMET1		13
#define CS_HELMET2		9
#define CS_HELMET3		3

#define CS_ARMOR1		14
#define CS_ARMOR2		9
#define CS_ARMOR3		3

#define CS_PANTS1		14
#define CS_PANTS2		9
#define CS_PANTS3		4

#define CS_GLOVES1		15
#define CS_GLOVES2		9
#define CS_GLOVES3		4

#define CS_BOOTS1		15
#define CS_BOOTS2		9
#define CS_BOOTS3		5

#ifndef CHECK_LIMIT
#define CHECK_LIMIT(value, limit) ( ((value)<0)?FALSE:((value)>((limit)-1))?FALSE:TRUE  )
#endif

#define CHECK_LIMIT2(value, base, limit) ( ((value)<base)?FALSE:((value)>((limit)-1))?FALSE:TRUE  )
#define CHECK_CLASS(value, type) ( ((value)!=(type))?FALSE:TRUE )

// Old CS defines
#define CS_SET_CLASS(x)			( ((x)<<5)& 0xE0 )		 //
#define CS_GET_CLASS(x)			( (((x)>>4)<<5)&(0xE0) ) //

#define CS_SET_CHANGEUP			0x10
#define CS_SET_CHANGEUP2		0x08

#define CS_GET_CHANGEUP(x)		( ((x)&0x07) )		 //

// Season X CS defines
#define CS_SET_CLASS_SX(x)		( ((x)<<4))		 //
#define CS_GET_CLASS_SX(x)		( (((x)>>4)<<4)) //

#define CS_SET_CHANGEUP_SX		0x08
#define CS_SET_CHANGEUP2_SX		0x04

#define CS_GET_CHANGEUP_SX(x)	( ((x)&0x07) )		 //

#define CS_SET_HELMET1(x) ( ((x) & 0x1E0) >> 5 )
#define CS_SET_HELMET2(x) ( ((x) & 0x10 ) << 3 )
#define CS_SET_HELMET3(x) ( ((x) & 0x0F ) << 4 )

#define CS_SET_ARMOR1(x) ( ((x) & 0x1E0) >> 1 )
#define CS_SET_ARMOR2(x) ( ((x) & 0x10 ) << 2 )
#define CS_SET_ARMOR3(x) ( ((x) & 0x0F )      )

#define CS_SET_PANTS1(x) ( ((x) & 0x1E0) >> 5 )
#define CS_SET_PANTS2(x) ( ((x) & 0x10 ) << 1 )
#define CS_SET_PANTS3(x) ( ((x) & 0x0F ) << 4 )

#define CS_SET_GLOVES1(x) ( ((x) & 0x1E0) >> 1 )
#define CS_SET_GLOVES2(x) ( ((x) & 0x10 )      )
#define CS_SET_GLOVES3(x) ( ((x) & 0x0F )      )

#define CS_SET_BOOTS1(x) ( ((x) & 0x1E0) >> 5 )
#define CS_SET_BOOTS2(x) ( ((x) & 0x10 ) >> 1 )
#define CS_SET_BOOTS3(x) ( ((x) & 0x0F ) << 4 )

#define CS_SET_WING1(x)  ( ((x) & 0x03 ) << 2 )
#define CS_SET_HELPER(x) ( ((x) & 0x03 )      )

#define CS_SET_SMALLLEVEL_RH(x)		( (x)       )
#define CS_SET_SMALLLEVEL_LH(x)		( (x) << 3  ) 
#define CS_SET_SMALLLEVEL_HELMET(x)	( (x) << 6  )
#define CS_SET_SMALLLEVEL_ARMOR(x)	( (x) << 9  )
#define CS_SET_SMALLLEVEL_PANTS(x)	( (x) << 12 )
#define CS_SET_SMALLLEVEL_GLOVES(x)	( (x) << 15 )
#define CS_SET_SMALLLEVEL_BOOTS(x)	( (x) << 18 )

#define CS_SET_SMALLLEVEL1(x)		( ((x) >> 16) & 0xFF )
#define CS_SET_SMALLLEVEL2(x)		( ((x) >> 8 ) & 0xFF )
#define CS_SET_SMALLLEVEL3(x)		((x) & 0xFF )
 
// End User Defines Funcs

#define MAX_VIP 4

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define ATTACK_TOLERANCE 3


#define MAX_DEVILSQUARE_GROUND	7
#define MAX_ST_DEVILSQUARE_MONSTER	15

#define DEVIL_SQUARE_GROUND_1	0
#define DEVIL_SQUARE_GROUND_2	1
#define DEVIL_SQUARE_GROUND_3	2
#define DEVIL_SQUARE_GROUND_4	3
#define DEVIL_SQUARE_GROUND_5	4
#define DEVIL_SQUARE_GROUND_6	5
#define DEVIL_SQUARE_GROUND_7	6

#define MAX_CUSTOM_EVENT_DROP 10
#define CUSTOMEVENT_EFFECT_NONE		0
#define CUSTOMEVENT_EFFECT_FIREWORK	1
#define CUSTOMEVENT_EFFECT_CHERRY		2

#define FILE_CUSTOM_EVENTDROP "Custom\CustomEventDrop.ini"

#define MAX_MESSAGE_SIZE 255
#define MAX_NOTICE 100

#define MAX_EVENTCHIP_TYPE	5
#define EVENCHIP_TYPE_RANGE(x)  ( ((x)<0)?FALSE:((x)>MAX_EVENTCHIP_TYPE-1)?FALSE:TRUE )


#define MAX_XMASMONSTER_MAP		3

#define MAX_GM_COMMAND 150
#define MAX_GMCOMMAND_LEN 20
#define MAX_GAME_MASTER 5


#endif
