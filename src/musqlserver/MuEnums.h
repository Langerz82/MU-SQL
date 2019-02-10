#ifndef _MU_MUENUMS_H_
#define _MU_MUENUMS_H_

enum MAP_INDEX
{
	MAP_INDEX_EMPTY = -1,
	MAP_INDEX_RORENCIA = 0x0,
	MAP_INDEX_DUNGEON = 0x1,
	MAP_INDEX_DEVIAS = 0x2,
	MAP_INDEX_NORIA = 0x3,
	MAP_INDEX_LOSTTOWER = 0x4,
	MAP_INDEX_RESERVED = 0x5,
	MAP_INDEX_ARENA = 0x6,
	MAP_INDEX_ATHLANSE = 0x7,
	MAP_INDEX_TARKAN = 0x8,
	MAP_INDEX_DEVILSQUARE = 0x9,
	MAP_INDEX_ICARUS = 0xa,
	MAP_INDEX_BLOODCASTLE1 = 0xb,
	MAP_INDEX_BLOODCASTLE2 = 0xc,
	MAP_INDEX_BLOODCASTLE3 = 0xd,
	MAP_INDEX_BLOODCASTLE4 = 0xe,
	MAP_INDEX_BLOODCASTLE5 = 0xf,
	MAP_INDEX_BLOODCASTLE6 = 0x10,
	MAP_INDEX_BLOODCASTLE7 = 0x11,
	MAP_INDEX_CHAOSCASTLE1 = 0x12,
	MAP_INDEX_CHAOSCASTLE2 = 0x13,
	MAP_INDEX_CHAOSCASTLE3 = 0x14,
	MAP_INDEX_CHAOSCASTLE4 = 0x15,
	MAP_INDEX_CHAOSCASTLE5 = 0x16,
	MAP_INDEX_CHAOSCASTLE6 = 0x17,
	MAP_INDEX_KALIMA1 = 0x18,
	MAP_INDEX_KALIMA2 = 0x19,
	MAP_INDEX_KALIMA3 = 0x1a,
	MAP_INDEX_KALIMA4 = 0x1b,
	MAP_INDEX_KALIMA5 = 0x1c,
	MAP_INDEX_KALIMA6 = 0x1d,
	MAP_INDEX_CASTLESIEGE = 0x1e,
	MAP_INDEX_CASTLEHUNTZONE = 0x1f,
	MAP_INDEX_DEVILSQUARE2 = 0x20,
	MAP_INDEX_AIDA = 0x21,
	MAP_INDEX_CRYWOLF_FIRSTZONE = 0x22,
	MAP_INDEX_CRYWOLF_SECONDZONE = 0x23,
	MAP_INDEX_KALIMA7 = 0x24,
	MAP_INDEX_KANTURU1 = 0x25,
	MAP_INDEX_KANTURU2 = 0x26,
	MAP_INDEX_KANTURU_BOSS = 0x27,
	MAP_INDEX_GAMEMASTER_MAP = 0x28, //0x28 OK
	MAP_INDEX_BARRACKS = 0x29, //0x29 OK
	MAP_INDEX_REFUGEE = 0x2a, //0x2a OK
	MAP_INDEX_ILLUSIONTEMPLE1 = 0x2d, //0x2d OK
	MAP_INDEX_ILLUSIONTEMPLE2 = 0x2e, //0x2e OK
	MAP_INDEX_ILLUSIONTEMPLE3 = 0x2f, //0x32 OK
	MAP_INDEX_ILLUSIONTEMPLE4 = 0x30, //0x30 OK
	MAP_INDEX_ILLUSIONTEMPLE5 = 0x31, //0x31 OK
	MAP_INDEX_ILLUSIONTEMPLE6 = 0x32, //0x32 OK
	MAP_INDEX_ELBELAND = 0x33, // 0x33 OK
	MAP_INDEX_BLOODCASTLE8 = 0x34,
	MAP_INDEX_CHAOSCASTLE7 = 0x35,
	MAP_INDEX_CALMNESS = 0x38,
	MAP_INDEX_RAKLION = 0x39,
	MAP_INDEX_HATCHERY = 0x3A,
	MAP_INDEX_SANTAVILLAGE = 0x3E,
	MAP_INDEX_VULCAN = 0x3F,
	MAP_INDEX_VULCANROOM = 0x40,
	MAP_INDEX_DOUBLE_GOER1 = 0x41,
	MAP_INDEX_DOUBLE_GOER2 = 0x42,
	MAP_INDEX_DOUBLE_GOER3 = 0x43,
	MAP_INDEX_DOUBLE_GOER4 = 0x44,
	MAP_INDEX_IMPERIAL1 = 0x45,
	MAP_INDEX_IMPERIAL2 = 0x46,
	MAP_INDEX_IMPERIAL3 = 0x47,
	MAP_INDEX_IMPERIAL4 = 0x48,
	MAP_INDEX_LORENMARKET = 0x4f,
	MAP_INDEX_KALRUTAN1 = 0x50,
	MAP_INDEX_KALRUTAN2 = 0x51,

	MAP_INDEX_DOPPELGANGER1 = 0x52,
	MAP_INDEX_DOPPELGANGER2 = 0x53,
	MAP_INDEX_DOPPELGANGER3 = 0x54,
	MAP_INDEX_DOPPELGANGER4 = 0x55,

	MAP_INDEX_ACHERON = 0x5b,
	MAP_INDEX_ARCA_WAR = 0x5c,
	MAP_INDEX_DEBENTER = 0x5F,
	MAP_INDEX_DEBENTER_ARCA_WAR = 0x60,
	MAP_INDEX_CHAOSCASTLE_SURVIVAL = 0x61,
	MAP_INDEX_ILLUSIONTEMPLE7 = 0x62,
	MAP_INDEX_ILLUSIONTEMPLE8 = 0x63,
	MAP_INDEX_URUK_MOUNTAIN = 0x64,
	MAP_INDEX_URUK_MOUNTAIN_ARCA_WAR = 0x65,
	MAP_INDEX_DEVILSQUARE_FINAL = 0x66,
	MAP_INDEX_TORMENTED_SQUARE1 = 103,
	MAP_INDEX_TORMENTED_SQUARE2 = 104,
	MAP_INDEX_TORMENTED_SQUARE3 = 105,
	MAP_INDEX_TORMENTED_SQUARE4 = 106,
	MAP_INDEX_NARS = 110,
	MAP_INDEX_FEREA = 112,
	MAP_INDEX_NIXIE_LAKE = 113
};

enum
{
	EVENT_NOTIFICATION_CHAOS_CASTLE = 1,
	EVENT_NOTIFICATION_BLOOD_CASTLE = 2,
	EVENT_NOTIFICATION_DEVIL_SQUARE = 3,
	EVENT_NOTIFICATION_ILLUSION_TEMPLE = 6,
	EVENT_NOTIFICATION_DOPPELGANGER = 7,

	EVENT_NOTIFICATION_MAX,
};

enum EXP_GETTING_EVENT_TYPE {
	EVENT_TYPE_NONE = 0x0,
	EVENT_TYPE_PARTY = 0x1,
	EVENT_TYPE_DEVILSQUARE = 0x2,
	EVENT_TYPE_CHAOSCASTLE = 0x3,
	EVENT_TYPE_BLOODCASTLE = 0x4,
	EVENT_TYPE_CRYWOLF = 0x5,
	EVENT_TYPE_ILLUSIONTEMPLE = 0x6,
};

enum WING_CHECK_FLAG
{
	WCF_ITEMNUMBER = 100,
	WCF_ITEMTYPE = 101,
};

enum WING_TYPE
{
	NONE_WING = 10,
	FIRST_WING = 11,
	SECOND_WING = 12,
	THIRD_WING = 13,
	LITTLE_WING = 14,
	MONSTER_WRING = 15,
	CHAOSCASTLE_WING = 16,
	GOODANDDEVIL_WING = 17,
};

enum EVENTENTRY_TYPE
{
	EVENT_ENTRY_BC = 0,
	EVENT_ENTRY_CC = 1,
	EVENT_ENTRY_DS = 2,
	EVENT_ENTRY_DG = 3,
	EVENT_ENTRY_IF = 4,
	EVENT_ENTRY_IT = 5,
};


enum NPC_TYPES
{
	NPC_NONE = 0,
	NPC_SHOP = 1,
	NPC_WAREHOUSE = 2,
	NPC_CHAOSMIX = 3,
	NPC_GOLDARCHER = 4,
	NPC_PENTAGRAMMIX = 5,
};

enum GM_SYSTEM_FLAGS
{
	GM_EVENT_MODIFY = 0x01,
	GM_COMMAND_FIRECRACKER = 0x02,
	GM_COMMAND_DC_MOVE = 0x04,
	GM_COMMAND_GUILD = 0x08,
	GM_COMMAND_BATTLESOCCER = 0x10,
	GM_COMMAND_ITEM_CREATE = 0x20,
	GM_COMMAND_PK_MODIFY = 0x40,
	GM_COMMAND_INVISIBLE = 0x80,
	GM_COMMAND_SKIN = 0x100,
	GM_COMMAND_BAN = 0x200,
	GM_USE_GMSHOP = 0x400,
	GM_MONSTERS_INVISIBLE = 0x800
};

enum DISABLE_MSB_DAMAGE_EFFECT
{
	MSB_DISABLE_MONSTER = 0,
	MSB_DISABLE_CHARACTER = 1,
	MSB_DISABLE_CHARACTER_FENRIR = 2
};

enum SERVER_TYPE
{
	SERVER_NORMAL = 0,
	SERVER_CASTLE = 1,
	SERVER_ARCA = 2,
	SERVER_BATTLECORE = 3
};

enum MU_EVENT_TABLE
{
	EV_TABLE_BC = 0,
	EV_TABLE_DS = 1,
	EV_TABLE_CC = 2,
	EV_TABLE_IT = 3,
	EV_TABLE_CS = 4,
	EV_TABLE_DEEP = 5,
	EV_TABLE_CW = 6,
	EV_TABLE_KANTURU = 7,
	EV_TABLE_RAKLION = 8,
	EV_TABLE_DG = 9,
	EV_TABLE_IF = 10,
	EV_TABLE_RING = 11,
	EV_TABLE_XMAS = 12,
	EV_TABLE_ARCA = 13,
	EV_TABLE_ACHERON = 14,
	EV_TABLE_CCF = 15,
	EV_TABLE_DSF = 16,
	EV_TABLE_LMS = 17
};


#endif
