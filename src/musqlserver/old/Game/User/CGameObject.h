#ifndef __MU_CGAMEOBJECT_H__
#define __MU_CGAMEOBJECT_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PrecompiledHeader/StdAfx.h"
#include "generalStructs.h"
#include "TSkillElement.h"
#include "TMonsterSkillElementInfo.h"

#include <vector>

class CEvolutionMonsterInfo;
class CGameObject;
class CItemObject;
class CMagicInf;
class CMuRummyInfo;
class CRaklionUtil;
class CUserData;
class CViewportGuild;
class TDurMagicKeyChecker;
class TMonsterAIAgro;

struct BOT_BUFF_LIST;
struct HITDAMAGE_STRUCT;
struct JEWELOFHARMONY_ITEM_EFFECT;
struct MOVE_MAPSERVER_AUTHINFO;
struct STR_ACTION_STATE;
struct STR_EFFECTLIST;
struct STR_INTERFACE_STATE;
struct VIEWPORT_PLAYER_STRUCT;
struct VIEWPORT_STRUCT;
struct WHISPER_STRUCT;
struct SKILL_INFO;
struct MUUN_EFFECT_LIST;





class CGameObject
{
public:
	CGameObject();
	~CGameObject();

	int m_Index;	// 0
	int Connected;	// 4
	char LoginMsgSnd;	// 8
	char LoginMsgCount;	// 9
	char CloseCount;	// A
	char CloseType;	// B
	bool bEnableDelCharacter;	// C
	//_PER_SOCKET_CONTEXT* PerSocketContext;	// 10
	unsigned int m_socket;	// 14
	int UserNumber;	// 28
	int DBNumber;	// 2C
	unsigned char EnableCharacterCreate;
	DWORD AutoSaveTime;	// 34
	DWORD ConnectCheckTime;	// 38
	DWORD CheckTick;	// 3C
	unsigned char CheckSpeedHack;	// 40
	DWORD CheckTick2;	// 44
	unsigned char CheckTickCount;	// 48
	int iPingTime;	// 50
	unsigned char m_TimeCount;	// 54
	DWORD m_dwPKTimer;	// 58
	short CheckSumTableNum;	// 5C
	DWORD CheckSumTime;	// 60
	WORD Type;	// 64
	unsigned char Live;	// 66
	char  AccountID[11];	// 68
	char  Name[11];	// 73
	WORD  Class; // 9C
	short Level; // A0
	double Life; // C0
	double MaxLife; // C4
	int   m_iScriptMaxLife;	// C8
	double FillLife;	// CC
	double FillLifeMax;	// D0
	double Mana;	// D4
	double MaxMana;	// D8
	WORD  Leadership;	// DC
	int AddLeadership;	// DE
	WORD  ChatLimitTime;	// E0
	BYTE  ChatLimitTimeSec;	// E2
	BYTE  FillLifeCount;	// E3
	int AddStrength;	// E4
	int AddDexterity;	// E6
	int AddVitality;	// E8
	int AddEnergy;	// EA
	int   BP;	// EC
	int   MaxBP;	// F0
	int	  AddBP;	// F4
	float VitalityToLife;	// F8
	float EnergyToMana;	// FC
	int  m_PK_Count;	// 100
	char  m_PK_Level;	// 101
	int   m_PK_Time;	// 104
	int	  m_PK_TotalCount;
	short X; // 108
	short Y; // 10A
	BYTE  Dir; // 10C
	BYTE  MapNumber; // 10D
	short SaveX;
	short SaveY;
	char  SaveMapNumber;
	short DieX;
	short DieY;
	char  DieMapNumber;
	int AddLife;	// 110
	int AddMana;	// 114
	int   iShield;	// 118
	int   iMaxShield;	// 11C
	int   iAddShield;	// 120
	int   iFillShieldMax;	// 124
	int   iFillShield;	// 128
	int   iFillShieldCount;	// 12C
	DWORD dwShieldAutoRefillTimer;	// 130
	BYTE  DamageDecrease;	// 134

#if(CUSTOM_DAMAGES12 == 1)
	BYTE  S12DamageDecrease;	// 
#endif

	BYTE  DamageReflect;	// 135
	short MonsterDieGetMoney;	// 136
	int  MonsterDieGetLife;	// 138
	int  MonsterDieGetMana;	// 139
	BYTE  AutoHPRecovery;
	BYTE  StartX;	// 13A
	BYTE  StartY;	// 13B
	short m_OldX;	// 13C
	short m_OldY;	// 13E
	short TX;	// 140
	short TY;	// 142
	short MTX;	// 144
	short MTY;	// 146
	int   PathCount;	// 148
	int   PathCur;	// 14C
	char  PathStartEnd;	// 150
	short PathOri[15];	// 152
	short PathX[15];	// 170
	short PathY[15];	// 18E
	char  PathDir[15];	// 1AC
	DWORD PathTime; // 1BC
	DWORD Authority;	// 1C4
	DWORD AuthorityCode;	// 1C8
	DWORD Penalty;	// 1CC
	DWORD GameMaster;
	DWORD PenaltyMask;
	time_t ChatBlockTime;
	BYTE  m_cAccountItemBlock; // 1D0
	STR_ACTION_STATE* m_ActState;	// 1D4
	BYTE  m_ActionNumber;	// 1D8
	DWORD m_ActionTime;
	BYTE  m_ActionCount;
	DWORD m_ChatFloodTime;
	BYTE  m_ChatFloodCount;
	DWORD m_State;	// 1DC
	unsigned char m_Rest;	// 1E1
	BYTE		m_ViewState;
	BYTE		m_BuffEffectCount;
	STR_EFFECTLIST* m_BuffEffectList[MAX_BUFFEFFECT];
	DWORD m_LastMoveTime;	// 1E8
	DWORD m_LastAttackTime;	// 1EC
	BYTE m_FriendServerOnline;	// 1F0
	int m_DetectSpeedHackTime;	// 1F4
	unsigned long m_SumLastAttackTime;	// 1F8
	unsigned long m_DetectCount;	// 1FC
	int m_DetectedHackKickCount;	// 200
	int m_SpeedHackPenalty;	// 204
	unsigned char m_AttackSpeedHackDetectedCount;	// 208
	//unsigned long m_PacketCheckTime;	// 20C
	unsigned char m_ShopTime;	// 210
	unsigned long m_TotalAttackTime;	// 214
	int m_TotalAttackCount;	// 218
	unsigned long TeleportTime;	// 21C
	char Teleport;	// 220
	char KillerType;	// 221
	char DieRegen;	// 222
	char RegenOk;	// 223
	BYTE  RegenMapNumber;	// 224
	BYTE  RegenMapX;	// 225
	BYTE  RegenMapY;	// 226
	DWORD RegenTime;	// 228
	DWORD MaxRegenTime;	// 22C
	short m_PosNum;	// 230
	DWORD LifeRefillTimer;	// 234
	DWORD CurActionTime;	// 238
	DWORD NextActionTime;	// 23C
	DWORD DelayActionTime;	// 240
	char DelayLevel;	// 244
	char m_iMonsterBattleDelay;	// 24B
	char m_cKalimaGateExist; // 24C
	int m_iKalimaGateIndex; // 250
	char m_cKalimaGateEnterCount;	// 254
	CGameObject * lpAttackObj;	// 258
	short m_SkillNumber;	// 25C
	DWORD m_SkillTime;	// 260
	bool m_bAttackerKilled;	// 264
	char m_ManaFillCount;	// 265
	char m_LifeFillCount;	// 266
	int SelfDefense[MAX_SELF_DEFENSE];	// 268
	DWORD SelfDefenseTime[MAX_SELF_DEFENSE];	// 27C
	DWORD MySelfDefenseTime;	// 290
	int PartyNumber; // 2C8
	int PartyTargetUser;	// 2CC
	char Married;
	char MarryName[11];
	char MarryRequested;
	int WinDuels;
	int LoseDuels;
	short MarryRequestIndex;
	DWORD MarryRequestTime;
	int m_RecallMon;	// 2EC
	int m_Change;	// 2F0
	short TargetNumber;	// 2F4
	short TargetNpcNumber;	// 2F6
	short LastAttackerID;	// 2FA
	int m_AttackDamageMin;	// 2FC
	int m_AttackDamageMax;	// 300
	int m_MagicDamageMin;	// 304
	int m_MagicDamageMax;	// 308
	int m_CurseDamageMin;
	int m_CurseDamageMax;
	int m_AttackDamageLeft;	// 30C
	int m_AttackDamageRight;	// 310
	int m_AttackDamageMaxLeft;	// 314
	int m_AttackDamageMinLeft;	// 318
	int m_AttackDamageMaxRight;	// 31C
	int m_AttackDamageMinRight;	// 320
	int m_AttackRating;	// 324
	int m_AttackSpeed;	// 328
	int m_MagicSpeed;	// 32C
	int m_Defense;	// 330
	int m_MagicDefense;	// 334
	int m_BaseDefense;
	int m_SuccessfulBlocking;	// 338
	int m_CurseSpell;
	short m_MoveSpeed;	// 33C
	short m_MoveRange;	// 33E
	short m_AttackRange;	// 340
	short m_AttackType;	// 342
	short m_ViewRange;	// 344
	short m_Attribute;	// 346
	short m_ItemRate;	// 348
	short m_MoneyRate;	// 34A
	int m_CriticalDamage;	// 34C
	int m_ExcelentDamage;	// 350
	CMagicInf * m_lpMagicBack;	// 354
	CMagicInf * Magic;	// 358
	char MagicCount;	// 35C
	unsigned char UseMagicNumber;	// 35D
	unsigned long UseMagicTime;	// 360
	char UseMagicCount;	// 364
	short OSAttackSerial;	// 366
	unsigned char SASCount;	// 368
	DWORD SkillAttackTime;	// 36C
	char CharSet[18]; 	// 370
	char m_Resistance[MAX_RESISTENCE_TYPE];	// 382
	char m_AddResistance[MAX_RESISTENCE_TYPE];	// 389
	int FrustrumX[MAX_ARRAY_FRUSTRUM];	// 390
	int FrustrumY[MAX_ARRAY_FRUSTRUM]; // 3A4
	VIEWPORT_STRUCT* VpPlayer;	// 3B0
	VIEWPORT_PLAYER_STRUCT * VpPlayer2;	// 734
	int VPCount; // AB8
	int VPCount2;	// ABC
	HITDAMAGE_STRUCT* sHD;	// AC0
	short sHDCount;	// CA0
	STR_INTERFACE_STATE* m_IfState;	// CA4
	DWORD m_InterfaceTime;	// CA8
	CItemObject* pInventory; // CAC
	LPBYTE  pInventoryMap; // CB0
	char * pInventoryCount;	// CB4
	char pTransaction;	// CB8
	CItemObject * Inventory1;	//CBC
	LPBYTE  InventoryMap1;	// CC0
	char InventoryCount1;	// CC4
	CItemObject * Inventory2;	// CC8
	LPBYTE InventoryMap2;	// CCC
	char InventoryCount2;	// CD0
	CItemObject * Trade; // CD4
	LPBYTE TradeMap;	// unkCD8
	int TradeMoney;	// CDC
	bool TradeOk;	// CE0
	CItemObject * pWarehouse; // CE4
	char WarehouseID;
	DWORD WarehouseTick;
	LPBYTE pWarehouseMap; // CE8
	char WarehouseCount;	// CEC
	short WarehousePW; // CEE
	BYTE WarehouseLock;	// CF0
	BYTE WarehouseUnfailLock;	// CF1
	int WarehouseMoney;	// CF4
	CItemObject * pChaosBox;	// CFC
	LPBYTE pChaosBoxMap;	// D00
	int ChaosMoney;	// D04
	int ChaosSuccessRate;	// D08
	BYTE ChaosMassMixCurrItem;
	BYTE ChaosMassMixSuccessCount;
	BOOL ChaosLock;	// D0C
	DWORD m_Option;	// D10
	int m_nEventScore;	// D14
	int m_nEventExp;	// D18
	int m_nEventMoney;	// D1C
	BYTE m_bDevilSquareIndex;	// D20
	bool m_bDevilSquareAuth;	// D21
	char m_cBloodCastleIndex;	// D22
	char m_cBloodCastleSubIndex;	// D23
	int m_iBloodCastleEXP;	// D24
	bool m_bBloodCastleComplete;	// D28
	char m_cChaosCastleIndex;	// D29
	char m_cChaosCastleSubIndex;	// D2A
	int m_iChaosCastleBlowTime;	// D2C
	bool m_bCCF_UI_Using;
	BYTE m_byCCF_CanEnter;
	int m_nCCF_CertiTick;
	int m_nCCF_UserIndex;
	int m_nCCF_BlowTime;
	BYTE m_byKillUserCount;
	BYTE m_byKillMobCount;
	bool m_bCCF_Quit_Msg;
	char m_cIllusionTempleIndex;
	int m_nZoneIndex;
	char m_cKillUserCount;	// D30
	char m_cKillMonsterCount;	// D31
	int m_iDuelUserReserved;	// D34
	int m_iDuelUserRequested;	// D38
	int m_iDuelUser;	// D3C
	int m_iDuelRoom;
	BYTE m_btDuelScore;	// D40
	int m_iDuelTickCount;	// D44
	bool IsInBattleGround;	// ECA
	bool HaveWeaponInHand;	// ECB
	short EventChipCount;	// ECC
	int LuckyCoinCount;
	int MutoNumber;	// ED0
	bool UseEventServer;	// ED4
	bool LoadWareHouseInfo;	// ED8
	int iStoneCount;	// EDC
	int m_MaxLifePower;	// F20
	int m_CheckLifeTime;	// F30
	unsigned char m_MoveOtherServer;	// F34
	char BackName[11];	// F35
	bool m_bPShopOpen;	// D48
	bool m_bPShopTransaction;	// D49	
	bool m_bPShopItemChange;	// D4A
	bool m_bPShopRedrawAbs;	// D4B
	char m_szPShopText[37];		// D4C
	bool m_bPShopWantDeal;	// D70
	int m_iPShopDealerIndex;	// D74
	char m_szPShopDealerName[10];	// D78
	CRITICAL_SECTION m_critPShopTrade;	// D84
	int m_iVpPShopPlayer[MAX_VIEWPORT];	// D9C
	WORD m_wVpPShopPlayerCount;	// EC8
	char m_BossGoldDerconMapNumber;	// F40
	char m_LastTeleportTime;	// F42
	BYTE m_ClientHackLogCount;	// F43
	bool m_bIsInMonsterHerd;	// F44
	bool m_bIsMonsterAttackFirst;	// F48
	class MonsterHerd * m_lpMonsterHerd;	// F4C
	int fSkillFrustrumX[MAX_ARRAY_FRUSTRUM];	// F84
	int fSkillFrustrumY[MAX_ARRAY_FRUSTRUM];	// F94
	TDurMagicKeyChecker * DurMagicKeyChecker;	// FD8
	bool bIsChaosMixCompleted;	// 110E
	bool SkillLongSpearChange;	// 110F
	int iObjectSecTimer;	// 1514
	bool m_bMapSvrMoveQuit;	// 1518
	bool m_bMapSvrMoveReq;	// 1519
	bool m_bMapSvrMoveReq_2;
	DWORD m_dwMapSvrQuitTick;	// 151C
	short m_sPrevMapSvrCode;	// 1520
	short m_sDestMapNumber;	// 1522
	BYTE m_btDestX;	// 1524
	BYTE m_btDestY;	// 1525
	union
	{
		struct
		{
			BYTE	m_btCsNpcExistVal1;
			BYTE	m_btCsNpcExistVal2;
			BYTE	m_btCsNpcExistVal3;
			BYTE	m_btCsNpcExistVal4;
		};
		int		m_iCsNpcExistVal;
	};

	BYTE	m_btCsNpcType;

	BYTE	m_btCsGateOpen;

	int		m_iCsGateLeverLinkIndex;

	BYTE	m_btCsNpcDfLevel;
	BYTE	m_btCsNpcRgLevel;

	BYTE	m_btCsJoinSide;
	bool	m_bCsGuildInvolved;
	bool m_bIsCastleNPCUpgradeCompleted;	// 1526
	BYTE m_btWeaponState;	// 1527
	int m_iWeaponUser;	// 1528
	BYTE m_btKillCount;	// 152C
	int m_iAccumulatedDamage;	// 1544
	BYTE	m_btLifeStoneCount;
	BYTE	m_btCreationState;
	int		m_iCreatedActivationTime;
	int m_iAccumulatedCrownAccessTime;	// 1550
	TMonsterSkillElementInfo m_MonsterSkillElementInfo;	// 1554
	int m_iBasicAI;	// 15A4
	int m_iCurrentAI;	// 15A8
	int m_iCurrentAIState;	// 15AC
	int m_iLastAIRunTime;	// 15B0
	int m_iGroupNumber;	// 15B4
	int m_iSubGroupNumber;	// 15B8
	int m_iGroupMemberGuid;	// 15BC
	int m_iRegenType;	// 15C0
	TMonsterAIAgro * m_Agro;	// 15C4
	int m_iLastAutomataRuntime;	// 18E8
	int m_iLastAutomataDelay;	// 18EC
	int m_iCrywolfMVPScore;	// 18F0
	DWORD m_dwLastCheckTick;	// 18F4
	int m_iAutoRecuperationTime;	// 18F8
	int m_iSkillDistanceErrorCount;	// 190C
	DWORD m_dwSkillDistanceErrorTick;	// 1910
	SKILL_INFO*	m_SkillInfo;
	int BufferIndex;
	int BuffId;
	int BuffPlayerIndex;
	DWORD AgiCheckTime;
	BYTE WarehouseSaveLock;
	DWORD CrcCheckTime;
	bool m_bOff;
	bool m_bOffLevel;
	DWORD64 m_dwOffLevelTime;
	CUserData* m_PlayerData;
	BOT_BUFF_LIST* m_BotBuffs;
	UINT64 MonsterMoneyDrop;
	BYTE m_btOpenWarehouse;
	WORD m_wMineCount;
	BYTE m_btMiningLevel;
	WORD m_LastAttackNumber;
	DWORD m_LastAttackTick;
	WORD m_SkillCheckWrongCastCount;
	short m_RegenSysGroupNum;
	MOVE_MAPSERVER_AUTHINFO* m_MapServerAuthInfo;
	bool m_bBlind;
	int m_iPentagramMainAttribute;
	int m_iPentagramAttributePattern;
	int m_iPentagramDefense;
	int m_iPentagramAttackMin;
	int m_iPentagramAttackMax;
	int m_iPentagramAttackRating;
	int m_iPentagramDefenseRating;
	int m_iPentagramDamageMax;
	int m_iPentagramDamageMin;
	int m_iPentagramDamageOrigin;
	BYTE m_btNpcType;
	bool m_bGMSummon;
	DWORD m_Disappear_Monster;
	DWORD m_Disappear_NewYearMonster;
	int m_SummonCharDBNum;
	bool m_bIsHelpMon;
	int m_nITR_Index;
	int m_nITR_RelicsTick;
	bool m_bITR_GettingRelics;
	bool m_bITR_RegisteringRelics;
	short m_wITR_NpcType;
	BYTE m_byITR_StoneState;
	int m_nITLIndex;
	bool m_bITL_ScheduleInfoSend;
	bool m_bITL_GuildRankInfoSend;
	bool m_bITL_TournamentInfoSend;
	int m_nITLRelicsTick;
	bool m_bGettingRelics;
	bool m_bRegisteringRelics;
	WORD m_wITLNpcType;
	BYTE m_byStoneState;
	BYTE m_byEnterITLCount;
	bool m_bEnterCountLoad;
	BYTE m_byEnterITLUserCount;
	DWORD m_PostCMDLastUseTick;
	int m_nDSFIndex;
	int m_nRecallMonIndex[3];
	int m_nBossIndexLordSilvester;
	int m_nPhaseLordSilvester;
	DWORD64 m_dwDCTimer;
	WORD m_wMuunItem;
	WORD m_wMuunSubItem;
	WORD m_wMuunRideItem;
	BYTE m_btInvenPetPos;
	WORD m_wInvenPet;
	CItemObject *pMuunInventory;
	CItemObject *pMuunInventory1;
	CItemObject *pMuunInventory2;
	CItemObject *pEventInventory;
	CItemObject *pEventInventory1;
	CItemObject *pEventInventory2;
	BYTE  *pEventInventoryMap;
	BYTE  *pEventInventoryMap1;
	BYTE  *pEventInventoryMap2;
	MUUN_EFFECT_LIST* m_MuunEffectList;
	bool EventInventoryLoad;
	bool bMuunInventoryLoad;
	DWORD dwCheckMuunItemTime;
	BOOL bSubEquip;
	int m_iMuunItmePeriodDataIndex;
	WHISPER_STRUCT* m_Whispering;
	BYTE m_btExpType;
	int m_nEvoMonOwnerIndex;
	BYTE EventNotification[EVENT_NOTIFICATION::EVENT_NOTIFICATION_MAX];
	BYTE EventMap;
	BYTE BCState = 0;
	BYTE DSState = 0;
	BYTE CCState = 0;
	BYTE CharsCount = 0;
	BYTE ITState = 0;
	char Name1[5][10];	// 1
						//char *Names = Name1;
						//char Name2[10];	// 1
						//char Name3[10];	// 1
						//char Name4[10];	// 1
						//char Name5[10];	// 1
	int		m_bMUBOT;
	int		m_iMUBOT_STAGE;
	int		m_iMUBOT_TIME;
};

// sizeof ( BILL_CLASS ) == 0x18
class BILL_CLASS
{

private:

	char cCertifyType;	// 0
	BYTE PayCode;	// 1
	int EndTime;	// 4
	char EndsDays[13];	// 8

public:

	BILL_CLASS()	// line : 253
	{
		this->Init();
	}

	void Init()	// line : 219
	{
		this->cCertifyType = -1;
		this->PayCode = 0;
		this->EndTime = 0;
		this->EndsDays[0] = 0;
	}	// line : 224

	BOOL SetBill(BYTE certify_type, BYTE pay, DWORD end_time, char* ends_days)	// line : 228
	{
		this->cCertifyType = certify_type;
		this->PayCode = pay;
		this->EndTime = end_time;
		strcpy(this->EndsDays, ends_days);
		return TRUE;
	}	// line : 237

		//int __thiscall IsEndTime();
	BYTE GetPayCode() { return this->PayCode; }	// line : 252
	char* GetEndsDays() { return &this->EndsDays[0]; }	// line : 253
	int GetEndTime() { return this->EndTime; }	// line : 254
	int GetCertify() { return this->cCertifyType; }	// line : 255
};


//typedef CGameObject (*CGameObject);


//extern CViewportGuild ViewGuildMng;
extern std::vector<CGameObject*> gGameObjects;
extern BILL_CLASS* m_ObjBill;
extern int gItemLoop;
extern int gItemLoop2;
extern int gItemLoopMax;
extern int gObjTotalUser;
extern int gObjTotalMonster;
extern int gDisconnect;
extern int GuildInfoOfs;
extern int GuildInfoCount;
extern int GuilUserOfs;
extern int GuildUserCount;
extern int lOfsChange;
extern int ChangeCount; // 8bf8b44

extern int gObjCallMonCount;
extern int gObjMonCount;
extern int gObjCount;


#endif
