#ifndef _MUSQL_GDEFINE_H_
#define _MUSQL_GDEFINE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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


#define MAX_ACCOUNT_LEN 10
#define MAX_TYPE_PLAYER 8
#define MAX_NUMBER_MAP 101

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

#ifndef NULL
#define NULL 0
#endif

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
