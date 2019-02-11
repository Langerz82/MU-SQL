#include "CGameObject.h"

//Y
//|-------
//||6|5|4|
//||7|9|3|
//||8|1|2|
//|------- X

int ChangeCount;
int lOfsChange;
int GuildUserCount;
int GuildUserOfs;
int GuildInfoCount;
int GuildInfoOfs;
int GensInfoOfs;
int GensInfoCount;
int gDisconnect;
int gObjTotalUser;
int gObjTotalMonster;
int gItemLoopMax;
int gItemLoop;
int gItemLoop2;

int MaxViewportMonster = 20;

CViewportGuild ViewGuildMng;	// line : 130
std::vector<LPGameObject> gGameObjects;

int gObjCount;
int gObjMonCount;
int gObjCallMonCount;
MessageStateMachine ** gSMMsg;
ExMessageStateMachine ** gSMAttackProcMsg;
BILL_CLASS * m_ObjBill;	// line : 193
static CRaklionUtil RAKLION_UTIL;
CItem * pTempInventory;
BYTE * pTempInventoryMap;

BYTE GuildInfoBuf[10000];
BYTE GuildUserBuf[10000];
BYTE GensInfoBuf[10000];
BYTE GensUserBuf[10000];
BYTE SendGBufChange[5000];

PMSG_VIEWPORTCREATE pViewportCreate;
PMSG_VIEWPORTCREATE_CHANGE pViewportCreateChange;
PMSG_VIEWPORTDESTROY pViewportDestroy;
PMSG_MONSTER_VIEWPORTCREATE pMonsterViewportCreate;
PMSG_CALLMONSTER_VIEWPORTCREATE pCallMonsterViewportCreate;

#pragma pack(1)

PWMSG_COUNT pItemCount;
PMSG_ITEMVIEWPORTCREATE pItemViewportCreate;
PMSG_VIEWPORTDESTROY pItemViewportDestroy;

#pragma pack()


short RoadPathTable[MAX_ROAD_PATH_TABLE] = { -1, -1, 0, -1, 1, -1, 1, 0, 1, 1, 0, 1, -1, 1, -1, 0 };	// THIS VARIABLE HAS SOME STRANGE THINGS ONT ITS UBICATION
BOOL g_EnergyCheckOff = 1; // THIS VARIABLE HAS SOME STRANGE THINGS ONT ITS UBICATION
