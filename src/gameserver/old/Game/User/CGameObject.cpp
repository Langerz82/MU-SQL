#include "PrecompiledHeader/StdAfx.h"
#include "CGameObject.h"

#include "AntiSpeed.h"
#include "ComboAttack.h"
#include "EvolutionMonsterInfo.h"
#include "generalStructs.h"
#include "GremoryCase.h"
#include "GuildClass.h"
#include "IOCP.h"
#include "ItemObject.h"
#include "itemsocketoptiondefine.h"
#include "ItemSystemFor380.h"
#include "JewelOfHarmonySystem.h"
#include "Lua/MuLua.h"
#include "MagicInf.h"
#include "MuRummyInfo.h"
#include "MuunInfo.h"
#include "QuestExpUserInfo.h"
#include "RaklionUtil.h"
#include "Shop.h"
#include "SkillDelay.h"
#include "TDurMagicKeyChecker.h"
#include "TMonsterAIAgro.h"
#include "TMonsterSkillElementInfo.h"
#include "UnityBattleField.h"
#include "ViewportGuild.h"

#include <map>

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

int gObjMonCount;
int gObjCallMonCount;
std::map<int, MessageStateMachine*> gSMMsg;
std::map<int, ExMessageStateMachine*> gSMAttackProcMsg;
BILL_CLASS * m_ObjBill;	// line : 193
CRaklionUtil RAKLION_UTIL;

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

CGameObject::CGameObject()
{
	this->pntBotBuffs = new BOT_BUFF_LIST*[10];
	this->pntMuunEffectList = new MUUN_EFFECT_LIST*[MAX_MUUN_EFFECT_LIST];
	this->pntBuffEffectList = new STR_EFFECTLIST*[MAX_BUFFEFFECT];
}

CGameObject::~CGameObject()
{
	delete[] this->pntBotBuffs;
	delete[] this->pntMuunEffectList;
	delete[] this->pntBuffEffectList;
}



int gObjGetIndex(char* szId)
{
	for each (std::pair<int,CGameObject*> user in gGameObjects)
	{
		if (user.second->Name[0] == *szId)
		{
			if (strcmp(user.second->Name, szId) == 0)
			{
				return user.first;
			}
		}
	}
	return -1;
}

