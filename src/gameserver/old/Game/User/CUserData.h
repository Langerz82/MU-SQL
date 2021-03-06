#ifndef __MU_USER_H__
#define __MU_USER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "generalStructs.h"
#include "QuestExpUserInfo.h"
#include "TMonsterSkillInfo.h"
#include "SkillDelay.h"
#include "ItemSystemFor380.h"
#include "PentagramSystem.h"
#include "ComboAttack.h"
#include "JewelOfHarmonySystem.h"

struct CharacterNameOfUBF;
struct STR_USER_CHARACTERS;

class CNameConvert;
class CAttackQueue;
class CEvolutionMonsterInfo;
class CMuRummyInfo;
class CSkillDelay;
class UserQuestInfo;

typedef struct STR_ACTION_STATE
{
	unsigned long Rest : 1;	// 0
	unsigned long Attack : 1;	// 1
	unsigned long Move : 1;	// 2
	unsigned long Escape : 1;	// 3
	unsigned long Emotion : 4;	// 4
	unsigned long EmotionCount : 8;	// 8

} *LPACTION_STATE;

typedef struct STR_INTERFACE_STATE
{
	DWORD use : 2;
	DWORD state : 4;
	DWORD type : 10;

} *LPINTERFACE_STATE;



struct CharacterNameOfUBF
{
	void Clear()
	{
		memset(this->szCharacterName, 0, MAX_ACCOUNT_LEN + 1);
		memset(this->szCharacterNameOfUBF, 0, MAX_ACCOUNT_LEN + 1);
		this->nServerCodeOfHomeWorld = 0;
	}

	char szCharacterName[MAX_ACCOUNT_LEN + 1];
	char szCharacterNameOfUBF[MAX_ACCOUNT_LEN + 1];
	WORD nServerCodeOfHomeWorld;
};

class CNameConvert
{
public:
	CNameConvert();
	~CNameConvert();

	void InitData();
	void InputData(int cnt, const char *name, const char *realNameOfUBF, WORD serverCode, int Length);
	char * ConvertNameToRealName(const char *name);
	char * ConvertRealNameToName(const char *realName);
	WORD FindServerCodeAtRealName(const char *realName);
	WORD FindServerCodeAtCharacterName(const char *Name);
	char * ConvertSlotIndexToName(int slotindex);
	int IsDuplicationOfName();

private:
	CharacterNameOfUBF* m_perCharacterInfo;
};


class CUserData
{
public:
	CUserData();
	virtual ~CUserData();

	void Init(bool VipReset = true);

public:
	struct STR_CS_USER* ConnectUser;
	//char ConnectUser->IP[16];	// 18

	UINT64 Experience;	// AC
	UINT64 NextExp; // B0
	UINT64 MasterExperience;
	UINT64 MasterNextExp;
	short MasterLevel;
	int LevelUpPoint; // A4
	int	MasterPoint;
	int	MasterPointUsed;
	int   iFruitPoint;	// A8
	int   Money; // B4
	WORD Strength; // B8
	WORD Dexterity;	// BA
	WORD Vitality;	// BC
	WORD Energy;	// BE
	BYTE  DbClass;	// 9E
	BYTE  ChangeUP; // 9F
	struct GUILD_INFO_STRUCT *lpGuild;	// 2D4
	char GuildName[11]; // 2D8
	int GuildStatus;	// 2E4
	int iGuildUnionTimeStamp;	// 2E8
	int GuildNumber;	// 2D0
	DWORD LastMoveTime;
	int m_iResets;


	BYTE SantaCount;
	DWORD m_GoblinTime;
	bool m_bSecurityCheck;
	int m_iSecurityCode;
	BYTE RegisterdLMS;
	BYTE RegisteredLMSRoom;
	struct JEWELOFHARMONY_ITEM_EFFECT* m_JewelOfHarmonyEffect;	// 1914
	struct ITEMOPTION_FOR380ITEM_EFFECT* m_ItemOptionExFor380;	// 193A
	BOOL	m_bKanturuEntranceByNPC;
	bool m_iGensInfoLoad;
	bool m_iQuestInfoLoad;
	int m_WCoinP;
	int m_WCoinC;
	int m_GoblinPoint;
	int m_iPeriodItemEffectIndex;	// 195C
	struct SOCKET_OPTION_LIST* m_SeedOptionList; // Size=210 Offset=9392
	struct SOCKET_OPTION_LIST* m_BonusOptionList; // Size=42 Offset=9602
	struct SOCKET_OPTION_LIST* m_SetOptionList; // Size=12 Offset=9644
	WORD m_btRefillHPSocketOption; // Size=2 Offset=9656
	WORD m_btRefillMPSocketOption; // Size=2 Offset=9658
	WORD m_wSocketOptionMonsterDieGetHp; // Size=2 Offset=9660
	WORD m_wSocketOptionMonsterDieGetMana; // Size=2 Offset=9662
	BYTE m_btAGReduceRate; // Size=1 Offset=9664
	//=========
	BYTE m_InventoryExpansion;
	BYTE m_WarehouseExpansion;
	DWORD LastAuthTime;
	BYTE LastXorKey1[4];
	BYTE LastXorKey2[4];
	bool ISBOT;
	WORD wBotIndex;
	BYTE SkillHellFire2State;	// FA4
	BYTE SkillHellFire2Count;	// FA5
	DWORD SkillHellFire2Time;	// FA8
	BYTE SkillStrengthenHellFire2State;	// FA4
	BYTE SkillStrengthenHellFire2Count;	// FA5
	DWORD SkillStrengthenHellFire2Time;	// FA8
	BYTE m_ReqWarehouseOpen;	// FAC
	short SetOpAddSkillAttack;	// FB4
	short SetOpAddExDamage;	// FB6
	short SetOpAddExDamageSuccessRate;	// FB8
	short SetOpAddCriticalDamage;	// FBA
	short SetOpAddCriticalDamageSuccessRate;	// FBC
	short SetOpIncAGValue;	// FBE
	short SetOpAddDamage;	// FC0
	short SetOpAddMinAttackDamage;	// FC2
	short SetOpAddMaxAttackDamage;	// FC4
	short SetOpAddAttackDamage;	// FC6
	short SetOpAddDefence;	// FC8
	short SetOpAddDefenceRate;	// FCA
	short SetOpAddMagicPower;	// FCC
	short SetOpAddMinMagicDamage;
	short SetOpAddMaxMagicDamage;
	BYTE SetOpIgnoreDefense;	// FCE
	BYTE SetOpDoubleDamage;	// FCF
	BYTE SetOpTwoHandSwordImproveDamage;	// FD0
	BYTE SetOpImproveSuccessAttackRate;	// FD1
	BYTE SetOpReflectionDamage;	// FD2
	BYTE SetOpImproveSheldDefence;	// FD3
	BYTE SetOpDecreaseAG;	// FD4
	BYTE SetOpImproveItemDropRate;	// FD5
	bool IsFullSetItem;	// FD6
	DWORD PotionTime;
	int PacketsPerSecond;
	DWORD PacketTime;
	DWORD LivePacketTime;
	BYTE bt_BotType;
	BYTE LiveCheckDetectCount;
	char  Password[20];	// 7E
	bool  PlusStatQuestClear; // 8C
	bool  ComboSkillquestClear;	// 8D
	ComboSkillData* comboSkill; // 90
	int m_GensRanking;
	int m_GensClass;
	int m_ContributePoint;
	int m_NextContributePoint;
	int m_GensInfluence;
	int m_UserBattleZoneEnable;
	char KillUserName[11 * MAX_ACCOUNT_LEN];
	int m_KillUserCnt[10];
	int m_KillUserTotCnt;
	ULONGLONG m_GensAbusingTick;
	bool GensReward;
	char SwearWarning;
	BYTE m_Quest[100];	// EE0
	bool m_SendQuestInfo;	// F12
	DWORD AgilityCheckDelay;
	BYTE AgilityCheckCount;
	//CRITICAL_SECTION AgiCheckCriti;
	BYTE RFSkillCount;
	WORD DarkSideCount;
	WORD DarkSideTarget[5];
	//CRITICAL_SECTION m_DarkSideRFCriti;
	//CRITICAL_SECTION m_MasterSkillCriti;
	BYTE m_btSculptPos;
	BYTE m_btCharmPos;
	BYTE m_btArtifactPos;
	BYTE m_btExpUpCharmPos;
	int IGC_KillCount;
	WORD MoveGate;
	BOOL PentagramInfoLoad;
	bool BlessSkillCast;
	DWORD m_dwMSBFlagAttackDisable;
	int m_iMiningLevel;
	int m_iMiningObjectId;
	int m_iMiningJewelId;
	struct _MASTERLEVEL_PASSIVE_SKILL_OPTION* m_MPSkillOpt;
	struct UserQuestInfo m_UserQuestInfo[MAX_QUESTEXP_USER_INFO];
	int m_bUserQuestInfoSent;
	bool m_bUsePolymorphSkin;
	ULONGLONG LastSkillUseTick;
	DWORD LastSkillUseNumber;
	DWORD LastSkillUseCount;
	struct PENTAGRAM_OPTION* m_PentagramOptions;
	class CItemObject** pPentagramMixBox;
	BYTE** pPentagramMixBoxMap;
	int PentagramMixMoney;
	int PentagramMixSuccessRate;
	int m_PentagramMixLock;
	bool m_bIsPentagramMixCompleted;
	std::vector<struct PENTAGRAMJEWEL_INFO> m_PentagramJewelInfo_Inven;
	std::vector<struct PENTAGRAMJEWEL_INFO> m_PentagramJewelInfo_Warehouse;
	DWORD MathAuthenElapsed;

	int m_AttackRatePvM;
	double m_AttackRatePvP;
	double m_DefenseRatePvP;
	bool bActiveSetParty;
	char PartyPassword[10];
	bool m_bUseGuildMatching;
	bool m_bUsePartyMatching;
	bool m_bPartyMatchingLeader;
	int m_nServerChannel;
	int m_nPartyMatchingIndex;
	int m_nPartyMatchingMemberCount;
	class CMuRummyInfo *m_pCMuRummyInfo;
	bool m_bSendMemberPos;
	bool m_bSendNpcPos;
	int  m_Resistance_Stun;
	int  m_Resistance_Double;
	int  m_Resistance_Perfect;
	int  m_Resistance_SD;
	int  m_Resistance_Excellent;
	int  m_Resistance_Critical;
	bool m_bIsMining;
	bool m_EnableUseChangeSkin;
	DWORD m_dwMapMoveKeyValue;
	struct EXC_WING_OPTION* m_WingExcOption;
	struct STAT_USER_OPTION* m_StatSpecOption;
	bool m_IsOffLevelling;
	struct STR_USER_SHOP_REBUY_ITEM** m_CancelItemSaleList;
	bool m_bIsCancelItemSale;
	int m_JoinUnityBattle;
	int m_nServerCodeOfHomeWorld;
	char m_RealNameOfUBF[11];
	class CNameConvert m_NameConvertOfUBF;
	bool m_bGremoryCaseDBInfo;
	struct GREMORYCASE_ITEM_DATA** m_GremoryCaseData;
	class CSkillDelay SkillDelay;
	DWORD m_dwGensHitTeleportDelay; // Private custom for joinmu
	DWORD dwOpenWareCnt;
	DWORD dwCloseWareCnt;
	bool bHasSameSerialItem;
	DWORD dwHitHackCounter1;
	DWORD dwLastHitHackValue1[7];
	DWORD dwHitHackCounter2;
	DWORD dwLastHitHackValue2[7];

	int Ruud;
	BYTE EventEntryCount[6];
	class CEvolutionMonsterInfo * m_pCEvoMonInfo;
	//Antihack related (move speed)
	short MovingDistance;
	BYTE  MovingIgnore;
	int RageDMG;
	class CAttackQueue* m_AttackQueue;
	char  Name[5][11];
	BYTE CharacterSlotCount;
	BOOL bEnableDelCharacter;
	DWORD m_dwDCTimer;
	BYTE VipType;
	char VipEffect;
	DWORD AntiHackCheckTime;
	DWORD64 dwLastHitHackTick;

	DWORD CheckTick;	// 3C
	DWORD CheckTick2;	// 44
	BYTE CheckTickCount;	// 48
	BYTE CheckSpeedHack;	// 40
	DWORD AgiCheckTime;
	struct MOVE_MAPSERVER_AUTHINFO* m_MapServerAuthInfo;
	bool m_bMapSvrMoveQuit;	// 1518
	bool m_bMapSvrMoveReq;	// 1519
	bool m_bMapSvrMoveReq_2;
	DWORD m_dwMapSvrQuitTick;	// 151C
	short m_sPrevMapSvrCode;	// 1520
	short m_sDestMapNumber;	// 1522	
	BYTE m_btDestX;	// 1524
	BYTE m_btDestY;	// 1525
	bool m_bOff;
	short m_bOffLevel;

	struct STR_USER_CHARACTERS* Characters;
};

extern int MaxViewportMonster;

extern std::map<int, CUserData*> gUserObjects;

unsigned char LevelSmallConvert(int level);
//unsigned char LevelSmallConvert(int aIndex, int inventoryindex);

extern CUserData* getUserObject(int index);
extern void insertUserObject(CUserData* Obj);
extern void eraseUserObject(CUserData* Obj);

extern BOOL gObjIsAccountConnect(int aIndex, char* accountid);
extern BOOL gObjPasswordCheck(class STR_CS_USER* csUser, char* szInPassword);


#endif
