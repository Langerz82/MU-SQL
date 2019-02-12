////////////////////////////////////////////////////////////////////////////////
// BotSystem.h
#ifndef BOTSYSTEM_H
#define BOTSYSTEM_H

#include "User/CUserData.h"

#define MAX_BUFFS_PER_BOT 10

struct _sBOT_INVENTORY_WEAR_ITEMS
{
	WORD wItemID;
	BYTE btItemLv;
	BYTE btItemExc;
};

enum BOT_TYPE
{
	BOT_BUFFER = 0,
	BOT_HIDENSEEK = 1,
	BOT_TRADE = 2,
	BOT_SHOP = 3
};

struct s_BOT_CRAFTING_ITEM_STRUCT
{
	WORD wItemId;
	BYTE btLevel;
	BYTE btLuck;
	BYTE btSkill;
	BYTE btOpt;
	BYTE btExc;
	BYTE btSetItem;
	BYTE btCount;
};

struct s_BOT_SHOP_ITEM
{
	int iValue;
	WORD wItemId;
	BYTE btLevel;
	BYTE btOpt;
	BYTE btLuck;
	BYTE btSkill;
	BYTE btDur;
	BYTE btExc;
	BYTE btAnc;
	BYTE btSocket[5];
};

struct sBOT_PSHOP
{
	s_BOT_SHOP_ITEM pItems[32];
	int iCoinType;
	std::string szBotShopName;
	int iItemCount;
};

struct sBOT_CRAFTING
{
		std::map<int,s_BOT_CRAFTING_ITEM_STRUCT> m_Requirements;
		s_BOT_CRAFTING_ITEM_STRUCT m_Reward;
		int RewardCount;
		int iSuccessRate;
		int iReqCount;
		int iTotalReqCount;
};

struct sBOT_REWARD_STRUCT
{
	CItem m_Reward;
	int iCount;
	int iSuccessRate;
};

struct _sBOT_SETTINGS
{
	_sBOT_SETTINGS()
	{
		this->wID = -1;
		this->btType = -1;
		this->bEnabled = -1;
		this->sName = "Empty";
		this->btMap = -1;
		this->btX = 0;
		this->btY = 0;
		this->btDir = -1;
		this->sPathActionFile = "Empty";
		this->wStrength = 0;
		this->wDexterity = 0;
		this->wVitality = 0;
		this->wEnergy = 0;
		this->btClass = 0;
		this->btVipType = 0;
		this->btCoinType = 0;
		this->iCoinValue = 0;
		memset(this->m_Items,0,sizeof(m_Items));
	}
//-- General Settings
	WORD wID;
	BYTE btType;
	BOOL bEnabled;
	BYTE btVipType;
	BYTE btCoinType;
	int iCoinValue;
//-- Bot Class & Stats
	BYTE btClass;
	WORD wStrength;
	WORD wDexterity;
	WORD wVitality;
	WORD wEnergy;
//-- Bot Wear
	_sBOT_INVENTORY_WEAR_ITEMS m_Items[INVETORY_WEAR_SIZE];
//--
	std::string sName;
//-- Location
	BYTE btMap;
	BYTE btX;
	BYTE btY;
	BYTE btDir;
//--
	std::string sPathActionFile;
//-- index in gObj
	CGameObject &lpObj;
//-- Bot Count for reference
	std::map<int,sBOT_CRAFTING> m_Crafting;
	sBOT_PSHOP m_Shop;
};


class CBotSystem
{
	public:
	CBotSystem();
	~CBotSystem();

	void Init();
	void LoadData(LPSTR szFile);
	bool LoadBotSpecializationData(CGameObject &lpObj, LPSTR szFile, BYTE type);
	void SetAllBots();
	int AddBot(_sBOT_SETTINGS pBot); 
	void BuffPlayer(WORD  wBufferindex,short aIndex);
	void MakePreviewCharSet(CGameObject &lpObj,_sBOT_INVENTORY_WEAR_ITEMS * pInv);
	BYTE GetBotType(CGameObject &lpObj);
	int GetSkillTime(CGameObject &lpObj, WORD wSkillId);
	bool CheckAlchemist(CGameObject &lpObj, int botIndex );
	int AlchemistTradeItemCount(CGameObject &lpObj);
	void AlchemistTradeOpen(CGameObject &lpObj, int botIndex);
	void AlchemistTradeOk(CGameObject &lpObj, int botIndex);
	sBOT_REWARD_STRUCT ConfirmMixSuccess(CGameObject &lpObj,int botIndex);
	bool AlchemistVerifyItem(s_BOT_CRAFTING_ITEM_STRUCT lpReqItem, CItem TradeItem);
	bool StoreAddItems(int botIndex);
	BYTE PShopCheckSpace(CGameObject &lpObj, int type, BYTE * TempMap);
	std::map<int,_sBOT_SETTINGS> m_BotData;
private:

	int iCount;
};
extern CBotSystem g_BotSystem;

BYTE gObjTempPShopRectCheck(BYTE * TempMap, int sx, int sy, int width, int height);
int gObjGetItemCountInTradeWindow(CGameObject &lpObj, WORD itemtype, int itemlevel, BYTE btExc, BYTE btSet, BYTE btOpt, BYTE btLuck, BYTE btSkill);
#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

