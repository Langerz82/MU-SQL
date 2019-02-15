#ifndef _MU_GOFUNCTIONS_H_
#define _MU_GOFUNCTIONS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include "CUserData.h"
#include "GameProtocol.h"

struct _stGremoryCaseItem;
class CMapItem;
class CGameObject;
class CUserData;

// TODO Match signatures with cpp definitions.

extern void gObjSkillUseProcTime500(CGameObject &Obj);
extern void MonsterAndMsgProc();
extern void MoveMonsterProc();
extern void gObjSetState();
extern int gObjInit(); // constructor ?
extern void gObjEnd(); // destructor ?
extern void gObjClearViewportOfMine(CGameObject &Obj);
extern void gObjClearViewport(CGameObject &Obj);
extern void gObjCloseSet(CGameObject &Obj, int Flag);
extern void gObjCharTradeClear(CGameObject &Obj);
extern void gObjCharZeroSet(CGameObject &Obj);
extern int gObjGetSocket(SOCKET socket);
extern int gObjGetHWIDUseCount(LPSTR HWID);
extern void gObjSetTradeOption(CGameObject &Obj, int option);
extern bool IsDuelEnable(CGameObject &Obj);
extern bool IsOnDuel(CGameObject &Obj1, CGameObject &Obj2);
extern void gObjSetDuelOption(CGameObject &Obj, int option);
//extern int GetMapMoveLevel(CGameObject &Obj, int mapnumber, int max_over);
extern void DbItemSetInByte(CGameObject &Obj, struct SDHP_DBCHAR_INFORESULT* lpMsg, bool* bAllItemExist);
extern void gObjSetBP(CGameObject &Obj);
extern bool gObjSetCharacter(LPBYTE lpdata, CGameObject &Obj);
extern int gObjCanItemTouch(CGameObject &Obj, int type);
extern void gObjMagicTextSave(CGameObject &Obj);
extern void ItemIsBufExOption(LPBYTE buf, CItemObject* lpItem);
extern void gObjStatTextSave(CGameObject &Obj);
extern void gObjItemTextSave(CGameObject &Obj);
extern void gObjWarehouseTextSave(CGameObject &Obj);
extern void gObjAuthorityCodeSet(CGameObject &Obj);
extern int gObjSetPosMonster(CGameObject &Obj, int PosTableNum);
extern int gObjSetMonster(CGameObject &Obj, int MonsterClass);
extern int gObjGetSocket(SOCKET socket);
extern short gObjAddSearch(SOCKET aSocket, char* ip);
extern short gObjAdd(SOCKET aSocket, char* ip, CGameObject &Obj);
extern short gObjMonsterRecall(int iMapNumber);
extern short gObjAddMonster(int iMapNumber);
extern short gObjAddCallMon();
extern void gObjUserKill(CGameObject &Obj);
extern void gObjAllLogOut();
extern void gObjAllDisconnect();
extern void gObjTradeSave(CGameObject &Obj);
extern void gObjBillRequest(CGameObject &Obj);
extern short gObjMemFree(CGameObject &Obj);
extern int gObjGameClose(CGameObject &Obj);
extern short gObjDel(CGameObject &Obj);
extern short gObjSometimeClose(CGameObject &Obj);
extern int gObjIsGamePlaing(CGameObject &Obj);
extern int gObjIsConnectedGP(CGameObject &Obj, char* CharName);
extern int gObjIsConnectedGP(CGameObject &Obj);
extern int gObjIsConnected(CGameObject &Obj, int dbnumber = -1);
extern bool gObjIsAccountValid(CGameObject &Obj, char* account);
extern int gObjIsConnectedEx(CGameObject &Obj);
extern int gObjIsAccontConnect(CGameObject &Obj, char* accountid);
extern int gObjPasswordCheck(CGameObject &Obj, char* szInPassword);
extern int gObjCheckXYMapTile(CGameObject &Obj, int iDbgName);
extern int gObjSetAccountLogin(CGameObject &Obj, char* szId, int aUserNumber, int aDBNumber, char* Password);
extern int gObjGetUserNumber(CGameObject &Obj);
extern int gObjGetNumber(CGameObject &Obj, int& UN, int& DBN);
extern LPSTR gObjGetAccountId(CGameObject &Obj);
//extern int gObjGetIndex(char* szId);
extern int gObjUserIdConnectCheck(char* szId, int index);
extern BYTE GetPathPacketDirPos(int px, int py);
extern int gObjCalDistance(CGameObject &Obj1, CGameObject &Obj2);
extern int gObjPositionCheck(CGameObject &Obj);
extern int gObjCheckTileArea(CGameObject &Obj, int x, int y, int dis);
extern int ExtentCheck(int x, int y, int w, int h);
extern BOOL CheckOutOfInventory(CGameObject &Obj, int sy, int height);	//-> 1.01.00
extern BOOL InventoryExtentCheck(int x, int y, int w, int h);		//-> 1.01.00
extern BOOL CheckOutOfWarehouse(CGameObject &Obj, int sy, int height);	//-> 1.01.00
extern BOOL WarehouseExtentCheck(int x, int y, int w, int h);		//-> 1.01.00
extern void gObjSetInventory1Pointer(CGameObject &Obj);
extern void gObjSetInventory2Pointer(CGameObject &Obj);
extern void gObjAddMsgSend(CGameObject &Obj, int aMsgCode, CGameObject &ObjTarget, int SubCode);
extern void gObjAddMsgSendDelay(CGameObject &Obj, int aMsgCode, CGameObject &ObjTarget, int delay, int SubCode);
extern void gObjAddMsgSendDelayInSpecificQPos(CGameObject &Obj, int aMsgCode, CGameObject &ObjTarget, int delay, int SubCode, int iQPosition);
extern void gObjAddAttackProcMsgSendDelay(CGameObject &Obj, int aMsgCode, CGameObject &ObjTarget, int delay, int SubCode, int SubCode2);
extern void gObjMsgProc(CGameObject &Obj);
extern void gObjStateProc(CGameObject &Obj, int aMsgCode, CGameObject &ObjTarget, int SubCode);
extern void gObjStateAttackProc(CGameObject &Obj, int aMsgCode, CGameObject &ObjTarget, int SubCode, int SubCode2);
extern int gObjBackSpring(CGameObject &Obj, CGameObject &TargetObj);
extern int BackSpringCheck(int& x, int& y, int& dir, BYTE map);
extern int gObjBackSpring2(CGameObject &Obj, CGameObject &TargetObj, int count);
extern bool gObjLevelUp(CGameObject &Obj, UINT64 addexp, int iMonsterType, const char * szEventType);
extern int gObjLevelUpPointAdd(BYTE type, CGameObject &Obj, CGameObject &ObjTarget);
//extern void gObjGiveItemSearch(CGameObject &Obj, int maxlevel);
//extern void gObjGiveItemWarehouseSearch(CGameObject &Obj, int maxlevel);
extern int gObjGuildMasterCapacityTest(CGameObject &Obj);
extern void gObjNextExpCal(CGameObject &Obj);
extern int retResistance(CGameObject &Obj, int Resistance_Type);
extern BOOL retCalcSkillResistance(CGameObject &TargetObj, BOOL isDouble);
extern int gObjAttackQ(CGameObject &Obj);
extern void gObjPlayerKiller(CGameObject &Obj, CGameObject &TargetObj, int MSBDamage);
extern void gObjUserDie(CGameObject &Obj, CGameObject &TargetObj);
extern void gObjInterfaceTimeCheck(CGameObject &Obj);
extern void gObjPkDownTimeCheck(CGameObject &Obj, int TargetLevel);
extern int gObjAngelSprite(CGameObject &Obj);
extern int gObjDemonSprite(CGameObject &Obj);
extern int gObjSatanSprite(CGameObject &Obj);
extern int gObjSafeGuardSprite(CGameObject &Obj);
extern int gObjFindInventoryEquipment(CGameObject &Obj);
extern int gObjSetInventoryEquipment(CGameObject &Obj, BYTE btPos);
extern int gObjInventoryEquipment(CGameObject &Obj);
extern int gObjWingSprite(CGameObject &Obj);
extern int gObjUniriaSprite(CGameObject &Obj);
extern void gObjSpriteDamage(CGameObject &Obj, int damage);
extern void gObjSecondDurDown(CGameObject &Obj);
extern void gObjChangeDurProc(CGameObject &Obj);
extern void gObjWingDurProc(CGameObject &Obj);
extern void gObjPenaltyDurDown(CGameObject &Obj, CGameObject &TargetObj);
extern void gObjWeaponDurDown(CGameObject &Obj, CGameObject &TargetObj, int type);
extern void gObjArmorRandomDurDown(CGameObject &Obj, CGameObject &AttackObj);
extern bool gObjIsSelfDefense(CGameObject &Obj, int aTargetIndex);
extern void gObjCheckSelfDefense(CGameObject &Obj, int aTargetIndex);
extern void gObjTimeCheckSelfDefense(CGameObject &Obj);
extern BOOL gObjAttack(CGameObject &Obj, CGameObject &TargetObj, class CMagicInf* lpMagic, BOOL magicsend, BYTE MSBFlag, int AttackDamage, BOOL bCombo, BYTE RFAttack, BYTE byReflect, BYTE byPentagramAttack = TRUE);
extern void gObjMonsterExpDivision(CGameObject &MonObj, CGameObject &Obj, int AttackDamage, int MSBFlag);
extern void gObjMonsterExpDivisionRenewal(CGameObject &MonObj, CGameObject &Obj, int AttackDamage, int MSBFlag);
extern UINT64 gObjMonsterExpSingle(CGameObject &Obj, CGameObject &TargetObj, int dmg, int tot_dmg, bool& bSendExp, UINT64& nDropZen);
extern UINT64 gObjMonsterExpSingleRenewal(CGameObject &Obj, CGameObject &TargetObj, int dmg, int tot_dmg, bool &bSendExp);
extern void gObjExpParty(CGameObject &Obj, CGameObject &TargetObj, int AttackDamage, int MSBFlag);
extern UINT64 gObjExpPartyRenewal(int nPartyNumber, CGameObject &Obj, CGameObject &TargetObj, int AttackDamage, int nTotalDamage, int MSBFlag);
extern void gObjMonsterDieLifePlus(CGameObject &Obj, CGameObject &ObjTarget);
extern void gObjLifeCheck(CGameObject &TargetObj, CGameObject &Obj, int AttackDamage, int DamageSendType, int MSBFlag, int MSBDamage, WORD Skill, int iShieldDamage, int iElementalDamage);
extern int gObjInventoryTrans(CGameObject &Obj);
extern int gObjInventoryCommit(CGameObject &Obj);
extern int gObjInventoryRollback(CGameObject &Obj);
extern void gObjInventoryItemSet(CGameObject &Obj, int itempos, BYTE set_byte);
extern void gObjInventoryItemBoxSet(CGameObject &Obj, int itempos, int xl, int yl, BYTE set_byte);
extern bool gObjFixInventoryPointer(CGameObject &Obj);
extern BYTE gObjInventoryDeleteItem(CGameObject &Obj, int itempos);
extern BYTE gObjWarehouseDeleteItem(CGameObject &Obj, int itempos);
extern BYTE gObjChaosBoxDeleteItem(CGameObject &Obj, int itempos);
extern BYTE gObjInventoryInsertItem(CGameObject &Obj, int type, int index, int level, int iSerial, int iDur);
extern DWORD gGetItemNumber();
extern void gPlusItemNumber();
extern BYTE gObjInventoryInsertItem(CGameObject &Obj, int type, int index, int level, int iSerial, int iDur);
extern BYTE gObjOnlyInventoryInsertItem(CGameObject &Obj, CGameObject &item);
extern BYTE gObjInventoryInsertItem(CGameObject &Obj, int type, int index, int level);
extern int gObjSearchItem(CGameObject &Obj, int item_type, int add_dur, int nLevel);
extern int gObjSearchItemMinus(CGameObject &Obj, int pos, int m_dur);
extern BYTE gObjShopBuyInventoryInsertItem(CGameObject &Obj, CItemObject &item);
extern BYTE gObjShopBuyMuunInventoryInsertItem(CGameObject &Obj, CItemObject &item);
extern BYTE gObjShopBuyEventInventoryInsertItem(CGameObject &Obj, CItemObject &item);
extern BYTE gObjInventoryInsertItemTemp(CGameObject &Obj, CMapItem &Item);
extern BYTE gObjInventoryInsertItem(CGameObject &Obj, CMapItem &item);
extern BYTE gObjInventoryInsertItem(CGameObject &Obj, CItemObject &item);
extern BYTE gObjMonsterInventoryInsertItem(CGameObject &Obj, int type, int index, int level, int op1, int op2, int op3);
extern BYTE gObjInventoryRectCheck(CGameObject &Obj, int sx, int sy, int width, int height);
extern BYTE gObjOnlyInventoryRectCheck(CGameObject &Obj, int sx, int sy, int width, int height);
extern int CheckInventoryEmptySpace(CGameObject &Obj, int iItemHeight, int iItemWidth);
extern int gObjIsItemPut(CGameObject &Obj, class CItemObject* lpItem, int pos);
extern BYTE gObjWerehouseRectCheck(CGameObject &Obj, int sx, int sy, int width, int height);
extern BYTE gObjMapRectCheck(LPBYTE lpMapBuf, int sx, int sy, int ex, int ey, int width, int height);
extern void gObjWarehouseItemBoxSet(CGameObject &Obj, int itempos, int xl, int yl, BYTE set_byte);
extern void gObjWarehouseItemSet(CGameObject &Obj, int itempos, BYTE set_byte);
extern void gObjChaosItemBoxSet(CGameObject &Obj, int itempos, int xl, int yl, BYTE set_byte);
extern void gObjChaosItemSet(CGameObject &Obj, int itempos, BYTE set_byte);
extern BYTE gObjChaosBoxInsertItemPos(CGameObject &Obj, CItemObject &item, int pos, int source);
extern BYTE gObjWarehouseInsertItemPos(CGameObject &Obj, CItemObject &item, int pos, int source);
extern BYTE gObjInventoryInsertItemPos(CGameObject &Obj, CItemObject &item, int pos, int RequestCheck);
extern int gObjInventorySearchSerialNumber(CGameObject &Obj, UINT64 serial);
extern int gObjWarehouseSearchSerialNumber(CGameObject &Obj, UINT64 sirial);
extern BYTE gObjInventoryMoveItem(CGameObject &Obj, BYTE source, BYTE target, int& durSsend, int& durTsend, BYTE sFlag, BYTE tFlag, LPBYTE siteminfo);
extern BYTE gObjTradeRectCheck(CGameObject &Obj, int sx, int sy, int width, int height);
extern int gObjTradeItemBoxSet(CGameObject &Obj, int itempos, int xl, int yl, BYTE set_byte);
extern BYTE gObjTradeInventoryMove(CGameObject &Obj, BYTE source, BYTE target);
extern BYTE gObjInventoryTradeMove(CGameObject &Obj, BYTE source, BYTE target);
extern BYTE gObjTradeTradeMove(CGameObject &Obj, BYTE source, BYTE target);
extern void gObjTradeCancel(CGameObject &Obj);
extern void gObjTempInventoryItemBoxSet(LPBYTE TempMap, int itempos, int xl, int yl, BYTE set_byte);
extern BYTE gObjTempInventoryRectCheck(CGameObject &Obj, LPBYTE TempMap, int sx, int sy, int width, int height);
extern BYTE gObjTempInventoryInsertItem(CGameObject &Obj, CItemObject &item, LPBYTE TempMap);
extern int TradeItemInventoryPutTest(CGameObject &Obj);
extern int TradeitemInventoryPut(CGameObject &Obj);
extern void gObjTradeOkButton(CGameObject &Obj);
extern BYTE LevelSmallConvert(int level);
extern BYTE LevelSmallConvert(CGameObject &Obj, int inventoryindex);
extern void gObjAttackDamageCalc(CGameObject &Obj);
extern void gObjMakePreviewCharSet(CGameObject &Obj);
extern void gObjViewportPaint(HWND hWnd);
extern void InitFrustrum();
extern void InitFrustrumEx(); // 1.01.10B GS KOR, changed values
extern void CreateFrustrum(int x, int y, CGameObject &Obj);
extern bool TestFrustrum2(int x, int y, CGameObject &Obj);
extern int gObjCheckViewport(CGameObject &Obj, int x, int y);
extern void gObjViewportClose(CGameObject &Obj);
extern void gObjViewportListCreate(CGameObject &Obj);
extern int ViewportAdd(CGameObject &Obj, CGameObject &ObjTarget, int aType);
//extern int ViewportAdd2(CGameObject &Obj, int aAddIndex, int aType);
extern void ViewportDel(CGameObject &Obj, int aDelIndex);
extern void Viewport2Del(CGameObject &Obj, int aDelIndex);
extern void gObjViewportAllDel(CGameObject &Obj);
extern void gObjViewportListDestroy(CGameObject &Obj);;
extern void gObjStateSetCreate(CGameObject &Obj);
extern void gObjSecondProc();
extern void gObjManaPotionFill(CGameObject &Obj);
extern void gObjRestPotionFill(CGameObject &Obj);
extern void gObjUseDrink(CGameObject &Obj, int level);
extern void gObjViewportListProtocolDestroy(CGameObject &Obj);
extern void gObjViewportListProtocolCreate(CGameObject &Obj);
extern void gObjViewportListProtocol(CGameObject &Obj);
extern void gObjTeleportMagicUse(CGameObject &Obj, BYTE x, BYTE y);
extern int gObjMoveGate(CGameObject &Obj, int gt);
extern void gObjTeleport(CGameObject &Obj, int map, int x, int y);
extern void gObjMoveDataLoadingOK(CGameObject &Obj);
extern class CMagicInf* gObjGetMagic(CGameObject &Obj, int mIndex);
extern class CMagicInf* gObjGetMagicSearch(CGameObject &Obj, WORD skillnumber);
extern int gObjMagicManaUse(CGameObject &Obj, CMagicInf* lpMagic);
extern int gObjMagicBPUse(CGameObject &Obj, CMagicInf* lpMagic);
extern void gObjMagicAddEnergyCheckOnOff(int flag);
extern int gObjMagicAdd(CGameObject &Obj, WORD aSkill, BYTE Level);
extern int gObjMagicDel(CGameObject &Obj, WORD aSkill, BYTE Level);
extern int gObjMagicAdd(CGameObject &Obj, WORD Type, WORD Index, BYTE Level, WORD& SkillNumber);
extern int gObjWeaponMagicAdd(CGameObject &Obj, WORD aSkill, BYTE Level);
extern int gObjMonsterMagicAdd(CGameObject &Obj, WORD aSkill, BYTE Level);
extern int gObjMagicEnergyCheck(CGameObject &Obj, WORD aSkill);
extern int gObjSpecialItemLevelUp(CGameObject &Obj, int source, int target);
extern int gObjSpecialItemRepair(CGameObject &Obj, int source, int target);
extern int gObjItemLevelUp(CGameObject &Obj, int source, int target);
extern int gObjItemRandomLevelUp(CGameObject &Obj, int source, int target);
extern int gObjItemRandomOption3Up(CGameObject &Obj, int source, int target);
extern void gObjAbilityReSet(CGameObject &Obj);
extern int gObjTargetGuildWarCheck(CGameObject &Obj, CGameObject &TargetObj);
extern void gObjGuildWarEndSend(GUILD_INFO_STRUCT &lpGuild1, GUILD_INFO_STRUCT &lpGuild2, BYTE Result1, BYTE Result2);
extern void gObjGuildWarEndSend(CGameObject &Obj, BYTE Result1, BYTE Result2);
extern void gObjGuildWarEnd(GUILD_INFO_STRUCT &lpGuild, GUILD_INFO_STRUCT &lpTargetGuild);
extern int gObjGuildWarProc(GUILD_INFO_STRUCT &lpGuild1, GUILD_INFO_STRUCT &lpGuild2, int score);
extern int gObjGuildWarCheck(CGameObject &Obj, CGameObject &TargetObj);
extern int gObjGuildWarMasterClose(CGameObject &Obj);
extern int gObjGuildWarItemGive(GUILD_INFO_STRUCT &lpWinGuild, GUILD_INFO_STRUCT &lpLoseGuild);
extern int gObjGetPkTime(CGameObject &Obj, int& hour, int& min);
extern int gObjMonsterCall(CGameObject &Obj, int MonsterType, int x, int y);
extern void gObjMonsterCallKill(CGameObject &Obj);
extern bool gObjCheckMoveArea(CGameObject &Obj, int X, int Y);
extern int gObjCheckTeleportArea(CGameObject &Obj, BYTE x, BYTE y);
extern int gObjCheckAttackAreaUsedPath(CGameObject &Obj, int TarObjIndex);
extern int gObjCheckattackAreaUsedViewPort(CGameObject &Obj, int TarObjIndex);
extern int gObjCheckAttackArea(CGameObject &Obj, int TarObjIndex);
extern int gUserFindDevilSquareInvitation(CGameObject &Obj);
extern int gUserFindDevilSquareKeyEyes(CGameObject &Obj);
extern void gObjSendUserStatistic(CGameObject &Obj, int startLevel, int endLevel);
extern CGameObject* gObjFind(LPTSTR targetcharname);
extern int gObjFind10EventChip(CGameObject &Obj);
extern int gObjDelete10EventChip(CGameObject &Obj);
extern void gObjSetPosition(CGameObject &Obj, int x, int y);
extern int gObjGetItemCountInEquipment(CGameObject &Obj, int itemtype, int itemindex, int itemlevel);
extern int gObjGetItemCountInIventory(CGameObject &Obj, int itemnum);
extern int gObjGetItemCountInIventory(CGameObject &Obj, int itemtype, int itemindex, int itemlevel);
extern int gObjGetManaItemPos(CGameObject &Obj);
extern void gObjDelteItemCountInInventory(CGameObject &Obj, int itemtype, int itemindex, int count);
extern void gObjGetStatPointState(CGameObject &Obj, short& AddPoint, short& MaxAddPoint, short& MinusPoint, short& MaxMinusPoint);
extern int gObjCheckStatPointUp(CGameObject &Obj);
extern int gObjCheckStatPointDown(CGameObject &Obj);
extern void gObjUseCircle(CGameObject &Obj, int pos);
extern void gObjUsePlusStatFruit(CGameObject &Obj, int pos);
extern void gObjUseMinusStatFruit(CGameObject &Obj, int pos);
extern void CashShopExMinusStatFruit(CGameObject &Obj, int pos);
extern void gObjCalcMaxLifePower(CGameObject &Obj);
extern void gObjDelayLifeCheck(CGameObject &Obj);
extern int gObjDuelCheck(CGameObject &Obj);
extern int gObjDuelCheck(CGameObject &Obj, CGameObject &TargetObj);
extern void gObjResetDuel(CGameObject &Obj);
extern void SkillFrustrum(BYTE bangle, CGameObject &Obj);
extern int SkillTestFrustrum(int x, int y, CGameObject &Obj);
extern int gObjCheckMaxZen(CGameObject &Obj, int nAddZen);
extern void MakeRandomSetItem(CGameObject &Obj);
extern void MakeRandomSetItem(CGameObject &Obj, _stGremoryCaseItem & stItem);
extern void MakeRewardSetItem(CGameObject &Obj, BYTE cDropX, BYTE cDropY, int iRewardType, int iMapnumber);
extern void MakeRewardSetItem(CGameObject &Obj, BYTE cDropX, BYTE cDropY, int iRewardType, int iMapnumber, _stGremoryCaseItem & stItem);
extern void gObjRecall(CGameObject &Obj, int mapnumber, int x, int y);
extern void gObjSetExpPetItem(CGameObject &Obj, UINT64 exp);
extern int gObjGetRandomItemDropLocation(int iMapNumber, BYTE& cX, BYTE& cY, int iRangeX, int iRangeY, int iLoopCount);
extern int gObjGetRandomFreeLocation(int iMapNumber, BYTE& cX, BYTE& cY, int iRangeX, int iRangeY, int iLoopCount);
extern int gObjCheckAttackTypeMagic(int iClass, int iSkill);
extern int gObjGetGuildUnionNumber(CGameObject &Obj);
extern void gObjGetGuildUnionName(CGameObject &Obj, char* szUnionName, int iUnionNameLen);
extern int gObjCheckRival(CGameObject &Obj, CGameObject &TargetObj);
extern int gObjGetRelationShip(CGameObject &Obj, CGameObject &TargetObj);
extern void gObjNotifyUpdateUnionV1(CGameObject &Obj);
extern void gObjNotifyUpdateUnionV2(CGameObject &Obj);
extern void gObjUnionUpdateProc(CGameObject &Obj);
extern void gObjSetKillCount(CGameObject &Obj, int iOption);
extern void gObjNotifyUseWeaponV1(CGameObject &OwnerObj, CGameObject &WeaponObj, int iTargetX, int iTargetY);
extern void gObjNotifyUseWeaponDamage(CGameObject &WeaponObj, int iTargetX, int iTargetY);
extern void gObjUseBlessAndSoulPotion(CGameObject &Obj, int iItemLevel);
extern void gObjWeaponDurDownInCastle(CGameObject &Obj, CGameObject &TargetObj, int iDecValue);
extern void gObjReady4Relife(CGameObject &Obj);
extern void gObjCheckTimeOutValue(CGameObject &Obj, DWORD& rNowTick);
extern void MsgOutput(CGameObject &Obj, char* msg, ...);
extern void gProcessAutoRecuperation(CGameObject &Obj);
extern void gObjShieldAutoRefill(CGameObject &Obj);
extern int gObjCheckOverlapItemUsingDur(CGameObject &Obj, int iMaxOverlapped, int iItemType, int iItemLevel);
extern int gObjOverlapItemUsingDur(CItemObject &Item, int iMapNumber, int iItemNumber, CGameObject &Obj, int iMaxOverlapped, int iItemType, int iItemLevel);
extern int gObjCheckOverlapEventItemUsingDur(CGameObject &Obj, int iMaxOverlapped, int iItemType, int iItemLevel);
extern int gObjOverlapEventItemUsingDur(CItemObject* lpItem, int iMapNumber, int iItemNumber, CGameObject &Obj, int iMaxOverlapped, int iItemType, int iItemLevel);
extern int gObjCheckSerial0ItemList(CItemObject* lpItem);
extern int gObjCheckInventorySerial0Item(CGameObject &Obj);
extern BOOL gCheckSkillDistance(CGameObject &Obj, int aTargetIndex, int iSkillNum);
extern void gObjSaveChaosBoxItemList(CGameObject &Obj);
extern void gObjQuestMonsterManagement(CGameObject &Obj, CGameObject &TargetObj);
extern BYTE gObjOnlyInventory1RectCheck(CGameObject &Obj, int sx, int sy, int width, int height);
extern BYTE gObjOnlyInventory2RectCheck(CGameObject &Obj, int sx, int sy, int width, int height);
extern void gObjReqMapSvrAuth(CGameObject &Obj);
extern BOOL gObjGetRandomFreeArea(int iMapNumber, BYTE &cX, BYTE &cY, int iSX, int iSY, int iDX, int iDY, int iLoopCount);
extern void gObjDeleteBokBlessEtc(CGameObject &Obj);
extern BOOL gObjItemLevelDown(CGameObject &Obj, int source, int target);
extern BOOL gObjItemExcessAdd(CGameObject &Obj, int source, int target);
extern BOOL gObjItemDoubleUp(CGameObject &Obj, int source, int target);
extern BOOL gObjItemRandomDoubleLevelUp(CGameObject &Obj, int source, int target);
extern BOOL gObjItemKondarAdd(CGameObject &Obj, int source, int target);
extern BOOL gObjItemRandomOptionNew3Up(CGameObject &Obj, int source, int target);
extern BOOL gObjItemKundumAdd(CGameObject &Obj, int source, int target);
extern BOOL gObjItemBalanceAdd(CGameObject &Obj, int source, int target);
extern BOOL gObjItemScienceAdd(CGameObject &Obj, int source, int target);
extern BOOL gObjItemLuckAdd(CGameObject &Obj, int source, int target);
extern BOOL gObjItemSpeedAdd(CGameObject &Obj, int source, int target);
extern int gObjCalcHPPercent(double Life, double MaxLife);
extern bool CheckAuthorityCondition(int AuthorityCode, CGameObject &Obj);
extern void GetBundleCount(int count, int div, int& itemcount, int& left);
extern int gObjGetJewelCountInInventory(CGameObject &Obj, BYTE type);
extern int gGetPartyMaxLevel(int nPartyNumber);
extern int gGetLowHPMonster(int nZoneIndex, int nIndex, int nDistance);
extern int gObjGamblingInventoryCheck(CGameObject &Obj, int iWidth, int iHeight);
extern void gObjAddSpecificSkillsForSiege(CGameObject &Obj);
extern void gObjPentagramMixBoxSet(CGameObject &Obj, int itempos, int xl, int yl, BYTE set_byte);
extern void gObjPentagramMixItemSet(CGameObject &Obj, int itempos, BYTE set_byte);
extern BYTE gObjPentagramMixBoxInsertItemPos(CGameObject &Obj, CItemObject &item, int pos, int source);
extern BYTE gObjPentagramMixBoxInsertItem(CGameObject &Obj, CItemObject &item);
extern BYTE gObjPentagramMixBoxRectCheck(CGameObject &Obj, int sx, int sy, int width, int height);
extern BYTE gObjPentagramMixBoxDeleteItem(CGameObject &Obj, int itempos);
extern int gTransPentagramJewelInfoTest(CGameObject &Obj, int targetIndex, CItemObject sourceItem);
extern int gTransPentagramJewelInfo(CGameObject &Obj, BYTE madeItemPos, int targetIndex, CItemObject sourceItem, int *nJewelCount);
extern void gObjAutoPartySet(CGameObject &Obj /* master */, CGameObject &TargetObj /*requestor*/);
extern BYTE gObjMuunInventoryInsertItemPos(CGameObject &Obj, CItemObject &item, int pos);
extern bool gObjFixMuunInventoryPointer(CGameObject &Obj);
extern void gObjSetMuunInventory1Pointer(CGameObject &Obj);
extern void gObjSetMuunInventory2Pointer(CGameObject &Obj);
extern BYTE gObjMuunInvenMove(CGameObject &Obj, int *durSsend, int *durTsend, BYTE source, BYTE target, BYTE *siteminfo);
extern BYTE gObjChkMuunInventoryEmpty(CGameObject &Obj);
extern BYTE gObjMuunInventoryInsertItem(CGameObject &Obj, CItemObject &item);
extern BYTE gObjMuunInventoryInsertItem(CGameObject &Obj, CMapItem *item);
extern BYTE gObjMuunInventoryDeleteItem(CGameObject &Obj, int itempos);

extern WING_TYPE GetWingType(WORD type, WING_CHECK_FLAG eFlag);
extern int UseBundleOfBlessJewel(CGameObject &Obj, int source, int target);
extern int IsExceptionJewelOfBlessInchant(CGameObject &Obj, int source, int target);
extern int IsNotApplyJewelToItems(WORD sItemType);
extern int IsTransformCharacterSkin(int pChange);
extern int gCountBlessSoulChaosJewel(CGameObject &Obj, short & sBlessJewelCount, short & s10BlessJewelCount, short & s20BlessJewelCount, short & s30BlessJewelCount, short & sSoulJewelCount, short & s10SoulJewelCount, short & s20SoulJewelCount, short & s30SoulJewelCount, short & sChaosJewelCount, short & s10ChaosJewelCount, short & s20ChaosJewelCount, short & s30ChaosJewelCount);
extern int gJewelBuyItemValueTest(CGameObject &ObjBuyer, CGameObject &ObjSeller, short sBlessJewelPrice, short sSoulJewelPrice, short sChaosJewelPrice, short sBlessJewelCount, short s10BlessJewelCount, short s20BlessJewelCount, short s30BlessJewelCount, short sSoulJewelCount, short s10SoulJewelCount, short s20SoulJewelCount, short s30SoulJewelCount, short sChaosJewelCount, short s10ChaosJewelCount, short s20ChaosJewelCount, short s30ChaosJewelCount, short &sBlessJewelNeedCount, short &s10BlessJewelNeedCount, short &s20BlessJewelNeedCount, short &s30BlessJewelNeedCount, short &sSoulJewelNeedCount, short &s10SoulJewelNeedCount, short &s20SoulJewelNeedCount, short &s30SoulJewelNeedCount, short &sChaosJewelNeedCount, short &s10ChaosJewelNeedCount, short &s20ChaosJewelNeedCount, short &s30ChaosJewelNeedCount);
extern int gJewelInventoryPutTest(CGameObject &ObjBuyer, CGameObject &ObjSeller, short sBlessJewelPrice, short sSoulJewelPrice, short sChaosJewelPrice);
extern int gJewelInventoryPut(CGameObject &ObjBuyer, CGameObject &ObjSeller, short sBlessJewelPrice, short sSoulJewelPrice, short sChaosJewelPrice, short sBlessJewelNeedCount, short s10BlessJewelNeedCount, short s20BlessJewelNeedCount, short s30BlessJewelNeedCount, short sSoulJewelNeedCount, short s10SoulJewelNeedCount, short s20SoulJewelNeedCount, short s30SoulJewelNeedCount, short sChaosJewelNeedCount, short s10ChaosJewelNeedCount, short s20ChaosJewelNeedCount, short s30ChaosJewelNeedCount);
extern int CheckInventoryEmptySpaceCount(CGameObject &Obj, int iItemHeight, int iItemWidth);
extern BOOL SummonGoldColossusMonster(CGameObject &Obj, int monsterIndex, int nCount, int bIsHelpMon);
extern BOOL SummonPetEventMonster(CGameObject &Obj);
extern BOOL NewYearSummonMonster(CGameObject &Obj, int monsterIndex);
extern int gObjGetAutoPartyUserCount();
extern int gObjGetOffTradeUsercount();
extern void gObjDisconnectOffTraders();
extern void gObjDisconnectOffLevelers();
extern void gObjInvenPetDamage(CGameObject &Obj, int damage);
extern BOOL gObjUseInvenPetCheck(CGameObject &Obj, CItemObject *lpItem, int flag);
extern BOOL IsInvenPet(WORD sIndex);
extern BOOL IsInvenItem(WORD sIndex);
extern int gObjCalDistanceTX(CGameObject &Obj1, CGameObject &Obj2);
extern void Check_SameSerialItem(CGameObject &Obj, BYTE bCase, bool & bCheckFail);
extern bool gObjChaosBoxPutItemTest(CGameObject &Obj, CItemObject Item, BYTE btCount);
extern BYTE gObjChaosBoxInsertItemTemp(CGameObject &Obj, CItemObject * Item);
extern BOOL gObjUnicornSprite(CGameObject &Obj);
extern void gObjUseRecoveryPotion(CGameObject &Obj, int pos, double value);
extern void gObjMuBotPayForUse(CGameObject &Obj);
extern void gObjMuBotProc(CGameObject &Obj);
extern int GetWarehouseUsedHowMuch(int UserLevel, BOOL IsLock);
extern int ViewportAdd2(CGameObject &Obj, CGameObject &ObjTarget, int aType);

#endif
