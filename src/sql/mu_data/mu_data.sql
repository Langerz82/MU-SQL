CREATE DATABASE  IF NOT EXISTS `mu_data` /*!40100 DEFAULT CHARACTER SET latin1 */;
USE `mu_data`;
-- MySQL dump 10.13  Distrib 5.7.17, for Win64 (x86_64)
--
-- Host: localhost    Database: mu_data
-- ------------------------------------------------------
-- Server version	5.5.60

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `accountcharacter`
--

DROP TABLE IF EXISTS `accountcharacter`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `accountcharacter` (
  `Number` int(11) NOT NULL AUTO_INCREMENT,
  `Id` varchar(10) COLLATE utf8_bin NOT NULL,
  `GameID1` varchar(10) COLLATE utf8_bin DEFAULT NULL,
  `GameID2` varchar(10) COLLATE utf8_bin DEFAULT NULL,
  `GameID3` varchar(10) COLLATE utf8_bin DEFAULT NULL,
  `GameID4` varchar(10) COLLATE utf8_bin DEFAULT NULL,
  `GameID5` varchar(10) COLLATE utf8_bin DEFAULT NULL,
  `GameIDC` varchar(10) COLLATE utf8_bin DEFAULT NULL,
  `MoveCnt` tinyint(3) unsigned DEFAULT '0',
  `Summoner` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `WarehouseExpansion` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `RageFighter` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `SecCode` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`Number`,`Id`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `c_monster_killcount`
--

DROP TABLE IF EXISTS `c_monster_killcount`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `c_monster_killcount` (
  `name` varchar(10) COLLATE utf8_bin NOT NULL,
  `MonsterId` int(11) NOT NULL,
  `count` int(11) NOT NULL,
  UNIQUE KEY `C_Monster_KillCount_IX_C_Monster_KillCount` (`name`,`MonsterId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `c_playerkiller_info`
--

DROP TABLE IF EXISTS `c_playerkiller_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `c_playerkiller_info` (
  `Victim` varchar(20) COLLATE utf8_bin NOT NULL,
  `Killer` varchar(20) COLLATE utf8_bin NOT NULL,
  `KillDate` datetime NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cashlog`
--

DROP TABLE IF EXISTS `cashlog`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `cashlog` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `UserID` varchar(16) COLLATE utf8_bin DEFAULT NULL,
  `Amount` decimal(19,4) DEFAULT NULL,
  `SentDate` datetime DEFAULT '0000-00-00 00:00:00',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `character`
--

DROP TABLE IF EXISTS `character`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `character` (
  `AccountID` varchar(10) COLLATE utf8_bin NOT NULL,
  `Name` varchar(10) COLLATE utf8_bin NOT NULL,
  `cLevel` int(11) DEFAULT '1',
  `LevelUpPoint` int(11) DEFAULT '0',
  `Class` tinyint(3) unsigned DEFAULT NULL,
  `Experience` bigint(20) DEFAULT '0',
  `Strength` int(11) DEFAULT NULL,
  `Dexterity` int(11) DEFAULT NULL,
  `Vitality` int(11) DEFAULT NULL,
  `Energy` int(11) DEFAULT NULL,
  `MagicList` blob,
  `Money` int(11) DEFAULT '0',
  `Life` double DEFAULT NULL,
  `MaxLife` double DEFAULT NULL,
  `Mana` double DEFAULT NULL,
  `MaxMana` double DEFAULT NULL,
  `MapNumber` smallint(6) DEFAULT NULL,
  `MapPosX` smallint(6) DEFAULT NULL,
  `MapPosY` smallint(6) DEFAULT NULL,
  `MapDir` tinyint(3) unsigned DEFAULT '0',
  `PkCount` int(11) DEFAULT '0',
  `PkLevel` int(11) DEFAULT '3',
  `PkTime` int(11) DEFAULT '0',
  `MDate` datetime DEFAULT NULL,
  `LDate` datetime DEFAULT NULL,
  `CtlCode` tinyint(3) unsigned DEFAULT '0',
  `Quest` blob,
  `Leadership` int(11) DEFAULT NULL,
  `ChatLimitTime` smallint(6) NOT NULL DEFAULT '0',
  `FruitPoint` int(11) NOT NULL DEFAULT '0',
  `RESETS` int(11) NOT NULL DEFAULT '0',
  `Inventory` blob,
  `Married` int(11) DEFAULT '0',
  `MarryName` varchar(10) COLLATE utf8_bin DEFAULT NULL,
  `MuBotData` blob,
  `mLevel` int(11) NOT NULL DEFAULT '0',
  `mlPoint` int(11) NOT NULL DEFAULT '0',
  `mlExperience` bigint(20) NOT NULL DEFAULT '0',
  `mlNextExp` bigint(20) NOT NULL DEFAULT '0',
  `InventoryExpansion` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `WinDuels` int(11) NOT NULL DEFAULT '0',
  `LoseDuels` int(11) NOT NULL DEFAULT '0',
  `PenaltyMask` int(11) NOT NULL DEFAULT '0',
  `BlockChatTime` bigint(20) DEFAULT '0',
  `Ruud` int(11) DEFAULT '0',
  PRIMARY KEY (`Name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `connectionhistory`
--

DROP TABLE IF EXISTS `connectionhistory`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `connectionhistory` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `AccountID` varchar(10) COLLATE utf8_bin DEFAULT NULL,
  `ServerName` varchar(40) COLLATE utf8_bin DEFAULT NULL,
  `IP` varchar(16) COLLATE utf8_bin DEFAULT NULL,
  `Date` datetime DEFAULT NULL,
  `State` varchar(12) COLLATE utf8_bin DEFAULT NULL,
  `HWID` varchar(100) COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB AUTO_INCREMENT=1113 DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `defaultclasstype`
--

DROP TABLE IF EXISTS `defaultclasstype`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `defaultclasstype` (
  `Class` tinyint(3) unsigned NOT NULL,
  `Strength` smallint(6) DEFAULT NULL,
  `Dexterity` smallint(6) DEFAULT NULL,
  `Vitality` smallint(6) DEFAULT NULL,
  `Energy` smallint(6) DEFAULT NULL,
  `MagicList` blob,
  `Life` double DEFAULT NULL,
  `MaxLife` double DEFAULT NULL,
  `Mana` double DEFAULT NULL,
  `MaxMana` double DEFAULT NULL,
  `MapNumber` smallint(6) DEFAULT NULL,
  `MapPosX` smallint(6) DEFAULT NULL,
  `MapPosY` smallint(6) DEFAULT NULL,
  `Quest` blob,
  `DbVersion` tinyint(3) unsigned DEFAULT NULL,
  `Leadership` smallint(6) DEFAULT '0',
  `Level` smallint(6) DEFAULT '0',
  `LevelUpPoint` smallint(6) DEFAULT '0',
  `Inventory` blob,
  `LevelLife` double DEFAULT NULL,
  `LevelMana` double DEFAULT NULL,
  `VitalityToLife` double DEFAULT NULL,
  `EnergyToMana` double DEFAULT NULL,
  PRIMARY KEY (`Class`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `gameserverinfo`
--

DROP TABLE IF EXISTS `gameserverinfo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `gameserverinfo` (
  `Number` int(11) NOT NULL DEFAULT '0',
  `ItemCount` bigint(20) DEFAULT NULL,
  `ZenCount` int(11) DEFAULT '0',
  `GmItemCount` int(11) DEFAULT NULL,
  `AceItemCount` int(11) DEFAULT NULL,
  `GensRankingMonth` int(11) DEFAULT '1',
  PRIMARY KEY (`Number`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `guild`
--

DROP TABLE IF EXISTS `guild`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `guild` (
  `G_Name` varchar(8) COLLATE utf8_bin NOT NULL,
  `G_Mark` blob,
  `G_Score` int(11) NOT NULL DEFAULT '0',
  `G_Master` varchar(10) COLLATE utf8_bin DEFAULT NULL,
  `G_Count` int(11) DEFAULT NULL,
  `G_Notice` varchar(60) COLLATE utf8_bin DEFAULT NULL,
  `Number` int(11) NOT NULL AUTO_INCREMENT,
  `G_Type` int(11) NOT NULL DEFAULT '0',
  `G_Rival` int(11) NOT NULL DEFAULT '0',
  `G_Union` int(11) NOT NULL DEFAULT '0',
  `G_Warehouse` blob,
  `G_WHPassword` int(11) DEFAULT NULL,
  `G_WHMoney` int(11) DEFAULT NULL,
  PRIMARY KEY (`Number`,`G_Name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `igc_arca_battle_guild_join_info`
--

DROP TABLE IF EXISTS `igc_arca_battle_guild_join_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `igc_arca_battle_guild_join_info` (
  `G_Name` varchar(8) COLLATE utf8_bin NOT NULL,
  `G_Master` varchar(10) COLLATE utf8_bin NOT NULL,
  `Number` int(11) NOT NULL,
  `JoinDate` datetime DEFAULT NULL,
  `GroupNum` tinyint(3) unsigned NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `igc_arca_battle_guildmark_reg`
--

DROP TABLE IF EXISTS `igc_arca_battle_guildmark_reg`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `igc_arca_battle_guildmark_reg` (
  `Index` int(11) NOT NULL AUTO_INCREMENT,
  `G_Number` int(11) NOT NULL,
  `G_Name` varchar(8) COLLATE utf8_bin NOT NULL,
  `G_Master` varchar(10) COLLATE utf8_bin NOT NULL,
  `RegDate` datetime DEFAULT NULL,
  `GuildRegRank` int(11) DEFAULT NULL,
  `MarkCnt` bigint(20) NOT NULL,
  PRIMARY KEY (`Index`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `igc_arca_battle_member_join_info`
--

DROP TABLE IF EXISTS `igc_arca_battle_member_join_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `igc_arca_battle_member_join_info` (
  `G_Name` varchar(8) COLLATE utf8_bin DEFAULT NULL,
  `Number` int(11) NOT NULL,
  `CharName` varchar(10) COLLATE utf8_bin NOT NULL,
  `JoinDate` datetime DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `igc_arca_battle_proc_state`
--

DROP TABLE IF EXISTS `igc_arca_battle_proc_state`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `igc_arca_battle_proc_state` (
  `Proc_State` tinyint(3) unsigned NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `igc_arca_battle_win_guild_info`
--

DROP TABLE IF EXISTS `igc_arca_battle_win_guild_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `igc_arca_battle_win_guild_info` (
  `G_Name` varchar(8) COLLATE utf8_bin NOT NULL,
  `G_Number` int(11) NOT NULL,
  `WinDate` datetime NOT NULL,
  `OuccupyObelisk` tinyint(3) unsigned NOT NULL,
  `ObeliskGroup` tinyint(3) unsigned NOT NULL,
  `LeftTime` bigint(20) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `igc_cancelitemsale`
--

DROP TABLE IF EXISTS `igc_cancelitemsale`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `igc_cancelitemsale` (
  `AccountID` varchar(10) COLLATE utf8_bin NOT NULL,
  `Name` varchar(10) COLLATE utf8_bin NOT NULL,
  `ItemNumber` tinyint(3) unsigned NOT NULL,
  `ItemCount` int(11) NOT NULL,
  `SellDate` bigint(20) NOT NULL,
  `SellExpireDate` bigint(20) NOT NULL,
  `SellPrice` int(11) NOT NULL,
  `ItemCode` smallint(6) NOT NULL,
  `ItemLevel` tinyint(3) unsigned NOT NULL,
  `ItemOp1` tinyint(3) unsigned NOT NULL,
  `ItemOp2` tinyint(3) unsigned NOT NULL,
  `ItemOp3` tinyint(3) unsigned NOT NULL,
  `ItemDur` tinyint(3) unsigned NOT NULL,
  `ItemExcOp` tinyint(3) unsigned NOT NULL,
  `ItemSetOp` tinyint(3) unsigned NOT NULL,
  `ItemHarmonyOp` smallint(6) NOT NULL,
  `ItemOpEx` smallint(6) NOT NULL,
  `ItemSocketOp1` smallint(6) NOT NULL,
  `ItemSocketOp2` smallint(6) NOT NULL,
  `ItemSocketOp3` smallint(6) NOT NULL,
  `ItemSocketOp4` smallint(6) NOT NULL,
  `ItemSocketOp5` smallint(6) NOT NULL,
  `ItemBonusSocketOp` smallint(6) NOT NULL,
  `ItemSerial` bigint(20) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `igc_evomonranking`
--

DROP TABLE IF EXISTS `igc_evomonranking`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `igc_evomonranking` (
  `Name` varchar(10) COLLATE utf8_bin NOT NULL,
  `TotalScore` int(11) NOT NULL DEFAULT '0',
  `TotalDamage` int(11) NOT NULL DEFAULT '0',
  `LastSummonDate` datetime NOT NULL,
  PRIMARY KEY (`Name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `igc_friendchat_bannedcharacters`
--

DROP TABLE IF EXISTS `igc_friendchat_bannedcharacters`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `igc_friendchat_bannedcharacters` (
  `Name` varchar(10) COLLATE utf8_bin NOT NULL,
  PRIMARY KEY (`Name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `igc_friendchat_bannedips`
--

DROP TABLE IF EXISTS `igc_friendchat_bannedips`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `igc_friendchat_bannedips` (
  `IP` varchar(15) COLLATE utf8_bin NOT NULL,
  PRIMARY KEY (`IP`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `igc_friendchat_messagelog`
--

DROP TABLE IF EXISTS `igc_friendchat_messagelog`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `igc_friendchat_messagelog` (
  `Name` varchar(10) COLLATE utf8_bin NOT NULL,
  `FriendName` text COLLATE utf8_bin,
  `Text` varchar(135) COLLATE utf8_bin NOT NULL,
  `Date` datetime NOT NULL,
  `ID` bigint(20) NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `igc_gens`
--

DROP TABLE IF EXISTS `igc_gens`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `igc_gens` (
  `Name` varchar(10) COLLATE utf8_bin NOT NULL,
  `Influence` tinyint(3) unsigned NOT NULL,
  `Rank` int(11) NOT NULL DEFAULT '0',
  `Points` int(11) NOT NULL DEFAULT '0',
  `Reward` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `Class` smallint(6) NOT NULL DEFAULT '0',
  `LeaveDate` datetime DEFAULT NULL,
  PRIMARY KEY (`Name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `igc_gensabuse`
--

DROP TABLE IF EXISTS `igc_gensabuse`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `igc_gensabuse` (
  `Name` varchar(10) COLLATE utf8_bin NOT NULL,
  `KillName` varchar(10) COLLATE utf8_bin NOT NULL,
  `KillCount` smallint(6) DEFAULT NULL,
  UNIQUE KEY `IGC_GensAbuse_IX_IGC_GensAbuse` (`Name`,`KillName`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `igc_gremorycase`
--

DROP TABLE IF EXISTS `igc_gremorycase`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `igc_gremorycase` (
  `AccountID` varchar(10) COLLATE utf8_bin NOT NULL,
  `Name` varchar(10) COLLATE utf8_bin DEFAULT NULL,
  `StorageType` tinyint(3) unsigned NOT NULL,
  `RewardSource` smallint(6) NOT NULL,
  `AuthCode` int(11) NOT NULL,
  `ItemGUID` smallint(6) NOT NULL,
  `ItemID` int(11) NOT NULL,
  `ItemLevel` tinyint(3) unsigned NOT NULL,
  `ItemDurability` smallint(6) NOT NULL,
  `ItemOp1` tinyint(3) unsigned NOT NULL,
  `ItemOp2` tinyint(3) unsigned NOT NULL,
  `ItemOp3` tinyint(3) unsigned NOT NULL,
  `ItemExcOption` smallint(6) NOT NULL,
  `ItemSetOption` smallint(6) NOT NULL,
  `ItemSocketCount` tinyint(3) unsigned NOT NULL,
  `ItemMainAttribute` tinyint(3) unsigned NOT NULL,
  `ItemMuunEvoItemType` smallint(6) NOT NULL,
  `ItemMuunEvoItemIndex` smallint(6) NOT NULL,
  `ReceiveDate` bigint(20) NOT NULL,
  `ExpireDate` bigint(20) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `igc_guildmatching`
--

DROP TABLE IF EXISTS `igc_guildmatching`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `igc_guildmatching` (
  `identNo` bigint(20) NOT NULL AUTO_INCREMENT,
  `GuildName` varchar(8) COLLATE utf8_bin NOT NULL,
  `GuildNum` int(11) DEFAULT NULL,
  `GuildMasterName` varchar(10) COLLATE utf8_bin NOT NULL,
  `GuildMasterLevel` smallint(6) DEFAULT NULL,
  `GuildMasterClass` tinyint(3) unsigned DEFAULT NULL,
  `GuildMemberCnt` smallint(6) DEFAULT NULL,
  `Memo` varchar(40) COLLATE utf8_bin NOT NULL,
  `InterestType` tinyint(3) unsigned DEFAULT '0',
  `LevelRange` tinyint(3) unsigned DEFAULT '0',
  `ClassType` tinyint(3) unsigned DEFAULT '0',
  `GensType` tinyint(3) unsigned DEFAULT NULL,
  `RegDate` datetime DEFAULT NULL,
  PRIMARY KEY (`identNo`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `igc_machineid_banned`
--

DROP TABLE IF EXISTS `igc_machineid_banned`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `igc_machineid_banned` (
  `HWID` varchar(100) COLLATE utf8_bin NOT NULL,
  `Note` varchar(200) COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`HWID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `igc_partymatching`
--

DROP TABLE IF EXISTS `igc_partymatching`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `igc_partymatching` (
  `IdentNo` bigint(20) NOT NULL AUTO_INCREMENT,
  `PartyLeaderName` varchar(10) COLLATE utf8_bin DEFAULT NULL,
  `Title` varchar(40) COLLATE utf8_bin DEFAULT NULL,
  `MinLevel` smallint(6) DEFAULT NULL,
  `MaxLevel` smallint(6) DEFAULT NULL,
  `HuntingGround` smallint(6) DEFAULT NULL,
  `WantedClass` tinyint(3) unsigned DEFAULT NULL,
  `CurMemberCount` tinyint(3) unsigned DEFAULT NULL,
  `AcceptType` tinyint(3) unsigned DEFAULT NULL,
  `UsePassWord` tinyint(3) unsigned DEFAULT '0',
  `PassWord` varchar(4) COLLATE utf8_bin DEFAULT NULL,
  `WantedClassDetailInfo1` tinyint(3) unsigned DEFAULT NULL,
  `WantedClassDetailInfo2` tinyint(3) unsigned DEFAULT NULL,
  `WantedClassDetailInfo3` tinyint(3) unsigned DEFAULT NULL,
  `WantedClassDetailInfo4` tinyint(3) unsigned DEFAULT NULL,
  `WantedClassDetailInfo5` tinyint(3) unsigned DEFAULT NULL,
  `WantedClassDetailInfo6` tinyint(3) unsigned DEFAULT NULL,
  `WantedClassDetailInfo7` tinyint(3) unsigned DEFAULT NULL,
  `LeaderChannel` tinyint(3) unsigned DEFAULT NULL,
  `GensType` tinyint(3) unsigned DEFAULT NULL,
  `LeaderLevel` smallint(6) DEFAULT NULL,
  `LeaderClass` tinyint(3) unsigned DEFAULT NULL,
  `RegDate` datetime DEFAULT NULL,
  PRIMARY KEY (`IdentNo`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `igc_periodbuffinfo`
--

DROP TABLE IF EXISTS `igc_periodbuffinfo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `igc_periodbuffinfo` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `CharacterName` varchar(10) COLLATE utf8_bin NOT NULL,
  `BuffIndex` smallint(6) NOT NULL,
  `EffectType1` smallint(6) NOT NULL,
  `EffectType2` smallint(6) DEFAULT NULL,
  `ExpireDate` bigint(20) NOT NULL,
  `Duration` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `igc_perioditeminfo`
--

DROP TABLE IF EXISTS `igc_perioditeminfo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `igc_perioditeminfo` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `UserGUID` int(11) NOT NULL,
  `CharacterName` varchar(10) COLLATE utf8_bin NOT NULL,
  `ItemType` smallint(6) NOT NULL,
  `ItemCode` smallint(6) NOT NULL,
  `EffectCategory` smallint(6) NOT NULL,
  `EffectType1` smallint(6) NOT NULL,
  `EffectType2` smallint(6) DEFAULT NULL,
  `Serial` bigint(20) NOT NULL,
  `Duration` int(11) NOT NULL,
  `BuyDate` bigint(20) NOT NULL,
  `ExpireDate` bigint(20) NOT NULL,
  `UsedInfo` int(11) NOT NULL,
  `BuyDateString` datetime DEFAULT NULL,
  `ExpireDateString` datetime DEFAULT NULL,
  `SetExpire` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `IGC_PeriodItemInfo_IX_IGC_PeriodItemInfo` (`Serial`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `igc_waitguildmatching`
--

DROP TABLE IF EXISTS `igc_waitguildmatching`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `igc_waitguildmatching` (
  `identNo` bigint(20) NOT NULL AUTO_INCREMENT,
  `GuildNumber` int(11) NOT NULL,
  `GuildName` varchar(8) COLLATE utf8_bin NOT NULL,
  `GuildMasterName` varchar(10) COLLATE utf8_bin NOT NULL,
  `ApplicantName` varchar(10) COLLATE utf8_bin NOT NULL,
  `ApplicantClass` tinyint(3) unsigned DEFAULT NULL,
  `ApplicantLevel` smallint(6) DEFAULT NULL,
  `State` tinyint(3) unsigned DEFAULT NULL,
  `RegDate` datetime DEFAULT NULL,
  PRIMARY KEY (`identNo`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `igc_waitpartymatching`
--

DROP TABLE IF EXISTS `igc_waitpartymatching`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `igc_waitpartymatching` (
  `IdentNo` bigint(20) NOT NULL AUTO_INCREMENT,
  `LeaderName` varchar(10) COLLATE utf8_bin DEFAULT NULL,
  `MemberName` varchar(10) COLLATE utf8_bin DEFAULT NULL,
  `Class` tinyint(3) unsigned DEFAULT NULL,
  `MemberLevel` smallint(6) DEFAULT NULL,
  `MemberDBNumber` int(11) DEFAULT NULL,
  `RegDate` datetime DEFAULT NULL,
  PRIMARY KEY (`IdentNo`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `itemlog`
--

DROP TABLE IF EXISTS `itemlog`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `itemlog` (
  `seq` int(11) NOT NULL AUTO_INCREMENT,
  `acc` varchar(10) COLLATE utf8_bin DEFAULT NULL,
  `name` varchar(10) COLLATE utf8_bin DEFAULT NULL,
  `itemcode` char(32) COLLATE utf8_bin DEFAULT NULL,
  `sn` int(11) DEFAULT NULL,
  `Iname` varchar(50) COLLATE utf8_bin DEFAULT NULL,
  `Ilvl` varchar(50) COLLATE utf8_bin DEFAULT NULL,
  `Iskill` smallint(6) DEFAULT NULL,
  `Iluck` smallint(6) DEFAULT NULL,
  `Iext` text COLLATE utf8_bin,
  `Iset` smallint(6) DEFAULT NULL,
  `I380` smallint(6) DEFAULT NULL,
  `Ijh` smallint(6) DEFAULT NULL,
  `sentdate` datetime DEFAULT '0000-00-00 00:00:00',
  PRIMARY KEY (`seq`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `memb_credits`
--

DROP TABLE IF EXISTS `memb_credits`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `memb_credits` (
  `memb___id` varchar(10) COLLATE utf8_bin NOT NULL,
  `credits` int(11) NOT NULL DEFAULT '0',
  `used` int(11) DEFAULT NULL,
  PRIMARY KEY (`memb___id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `memb_info`
--

DROP TABLE IF EXISTS `memb_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `memb_info` (
  `memb_guid` int(11) NOT NULL AUTO_INCREMENT,
  `memb___id` varchar(10) COLLATE utf8_bin NOT NULL,
  `memb__pwd` varchar(20) COLLATE utf8_bin NOT NULL,
  `memb_name` varchar(10) COLLATE utf8_bin NOT NULL,
  `sno__numb` char(13) COLLATE utf8_bin NOT NULL,
  `post_code` char(6) COLLATE utf8_bin DEFAULT NULL,
  `addr_info` varchar(50) COLLATE utf8_bin DEFAULT NULL,
  `addr_deta` varchar(50) COLLATE utf8_bin DEFAULT NULL,
  `tel__numb` varchar(20) COLLATE utf8_bin DEFAULT NULL,
  `phon_numb` varchar(15) COLLATE utf8_bin DEFAULT NULL,
  `mail_addr` varchar(50) COLLATE utf8_bin DEFAULT NULL,
  `fpas_ques` varchar(50) COLLATE utf8_bin DEFAULT NULL,
  `fpas_answ` varchar(50) COLLATE utf8_bin DEFAULT NULL,
  `job__code` char(2) COLLATE utf8_bin DEFAULT NULL,
  `appl_days` datetime DEFAULT NULL,
  `modi_days` datetime DEFAULT NULL,
  `out__days` datetime DEFAULT NULL,
  `true_days` datetime DEFAULT NULL,
  `mail_chek` char(1) COLLATE utf8_bin DEFAULT '0',
  `bloc_code` char(1) COLLATE utf8_bin NOT NULL,
  `ctl1_code` char(1) COLLATE utf8_bin NOT NULL,
  `cspoints` int(11) DEFAULT NULL,
  `VipType` int(11) DEFAULT NULL,
  `VipStart` datetime DEFAULT NULL,
  `VipDays` datetime DEFAULT NULL,
  `JoinDate` varchar(23) COLLATE utf8_bin DEFAULT NULL,
  PRIMARY KEY (`memb_guid`),
  UNIQUE KEY `IX_MEMB_INFO_1` (`memb___id`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `memb_stat`
--

DROP TABLE IF EXISTS `memb_stat`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `memb_stat` (
  `memb___id` varchar(10) COLLATE utf8_bin NOT NULL,
  `ConnectStat` tinyint(3) unsigned DEFAULT NULL,
  `ServerName` varchar(20) COLLATE utf8_bin DEFAULT NULL,
  `IP` varchar(15) COLLATE utf8_bin DEFAULT NULL,
  `ConnectTM` datetime DEFAULT NULL,
  `DisConnectTM` datetime DEFAULT NULL,
  PRIMARY KEY (`memb___id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `mu_dbid`
--

DROP TABLE IF EXISTS `mu_dbid`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `mu_dbid` (
  `DESC` varchar(20) COLLATE utf8_bin NOT NULL,
  `Version` int(11) NOT NULL DEFAULT '1'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `mucastle_data`
--

DROP TABLE IF EXISTS `mucastle_data`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `mucastle_data` (
  `MAP_SVR_GROUP` int(11) NOT NULL,
  `SIEGE_START_DATE` datetime NOT NULL,
  `SIEGE_END_DATE` datetime NOT NULL,
  `SIEGE_GUILDLIST_SETTED` tinyint(1) NOT NULL,
  `SIEGE_ENDED` tinyint(1) NOT NULL DEFAULT '0',
  `CASTLE_OCCUPY` tinyint(1) NOT NULL DEFAULT '0',
  `OWNER_GUILD` varchar(8) COLLATE utf8_bin NOT NULL,
  `MONEY` decimal(19,4) NOT NULL DEFAULT '0.0000',
  `TAX_RATE_CHAOS` int(11) NOT NULL DEFAULT '0',
  `TAX_RATE_STORE` int(11) NOT NULL DEFAULT '0',
  `TAX_HUNT_ZONE` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`MAP_SVR_GROUP`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `mucastle_money_statistics`
--

DROP TABLE IF EXISTS `mucastle_money_statistics`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `mucastle_money_statistics` (
  `MAP_SVR_GROUP` int(11) NOT NULL,
  `LOG_DATE` datetime NOT NULL,
  `MONEY_CHANGE` decimal(19,4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `mucastle_npc`
--

DROP TABLE IF EXISTS `mucastle_npc`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `mucastle_npc` (
  `MAP_SVR_GROUP` int(11) NOT NULL,
  `NPC_NUMBER` int(11) NOT NULL,
  `NPC_INDEX` int(11) NOT NULL,
  `NPC_DF_LEVEL` int(11) NOT NULL,
  `NPC_RG_LEVEL` int(11) NOT NULL,
  `NPC_MAXHP` int(11) NOT NULL,
  `NPC_HP` int(11) NOT NULL,
  `NPC_X` tinyint(3) unsigned NOT NULL,
  `NPC_Y` tinyint(3) unsigned NOT NULL,
  `NPC_DIR` tinyint(3) unsigned NOT NULL,
  `NPC_CREATEDATE` datetime NOT NULL,
  UNIQUE KEY `MuCastle_NPC_IX_NPC_SUBKEY` (`MAP_SVR_GROUP`,`NPC_NUMBER`,`NPC_INDEX`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `mucastle_reg_siege`
--

DROP TABLE IF EXISTS `mucastle_reg_siege`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `mucastle_reg_siege` (
  `MAP_SVR_GROUP` int(11) NOT NULL,
  `REG_SIEGE_GUILD` varchar(8) COLLATE utf8_bin NOT NULL,
  `REG_MARKS` int(11) NOT NULL,
  `IS_GIVEUP` tinyint(3) unsigned NOT NULL,
  `SEQ_NUM` int(11) NOT NULL,
  UNIQUE KEY `MuCastle_REG_SIEGE_IX_ATTACK_GUILD_SUBKEY` (`MAP_SVR_GROUP`,`REG_SIEGE_GUILD`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `mucastle_siege_guildlist`
--

DROP TABLE IF EXISTS `mucastle_siege_guildlist`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `mucastle_siege_guildlist` (
  `MAP_SVR_GROUP` int(11) NOT NULL,
  `GUILD_NAME` varchar(10) COLLATE utf8_bin NOT NULL,
  `GUILD_ID` int(11) NOT NULL,
  `GUILD_INVOLVED` tinyint(1) NOT NULL,
  `GUILD_SCORE` int(11) NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `mucrywolf_data`
--

DROP TABLE IF EXISTS `mucrywolf_data`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `mucrywolf_data` (
  `MAP_SVR_GROUP` int(11) NOT NULL,
  `CRYWOLF_OCCUFY` int(11) NOT NULL DEFAULT '0',
  `CRYWOLF_STATE` int(11) NOT NULL DEFAULT '0',
  `CHAOSMIX_PLUS_RATE` int(11) NOT NULL DEFAULT '0',
  `CHAOSMIX_MINUS_RATE` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`MAP_SVR_GROUP`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `optiondata`
--

DROP TABLE IF EXISTS `optiondata`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `optiondata` (
  `Name` varchar(10) COLLATE utf8_bin NOT NULL,
  `SkillKey` blob,
  `GameOption` tinyint(3) unsigned DEFAULT NULL,
  `Qkey` tinyint(3) unsigned DEFAULT NULL,
  `Wkey` tinyint(3) unsigned DEFAULT NULL,
  `Ekey` tinyint(3) unsigned DEFAULT NULL,
  `ChatWindow` tinyint(3) unsigned DEFAULT '255',
  `Rkey` tinyint(3) unsigned DEFAULT NULL,
  `QWERLevel` int(11) DEFAULT NULL,
  `EnableChangeMode` tinyint(3) unsigned DEFAULT '0',
  PRIMARY KEY (`Name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `petwarehouse`
--

DROP TABLE IF EXISTS `petwarehouse`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `petwarehouse` (
  `AccountID` varchar(12) COLLATE utf8_bin NOT NULL,
  `Name` varchar(12) COLLATE utf8_bin NOT NULL,
  `Items` blob,
  `DbVersion` tinyint(3) unsigned DEFAULT NULL,
  `SubEquip` tinyint(3) unsigned DEFAULT NULL,
  PRIMARY KEY (`Name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_cguid`
--

DROP TABLE IF EXISTS `t_cguid`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_cguid` (
  `GUID` int(11) NOT NULL AUTO_INCREMENT,
  `Name` varchar(10) COLLATE utf8_bin NOT NULL,
  PRIMARY KEY (`GUID`)
) ENGINE=InnoDB AUTO_INCREMENT=69 DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_curcharname`
--

DROP TABLE IF EXISTS `t_curcharname`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_curcharname` (
  `Name` char(10) COLLATE utf8_bin NOT NULL,
  `cDate` datetime NOT NULL DEFAULT '0000-00-00 00:00:00'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_event_inventory`
--

DROP TABLE IF EXISTS `t_event_inventory`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_event_inventory` (
  `AccountID` varchar(10) COLLATE utf8_bin NOT NULL,
  `Name` varchar(10) COLLATE utf8_bin NOT NULL,
  `Inventory` blob,
  PRIMARY KEY (`Name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_friendlist`
--

DROP TABLE IF EXISTS `t_friendlist`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_friendlist` (
  `GUID` int(11) NOT NULL,
  `FriendGuid` int(11) DEFAULT NULL,
  `FriendName` varchar(10) COLLATE utf8_bin DEFAULT NULL,
  `Del` tinyint(3) unsigned DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_friendmail`
--

DROP TABLE IF EXISTS `t_friendmail`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_friendmail` (
  `MemoIndex` int(11) NOT NULL DEFAULT '10',
  `GUID` int(11) NOT NULL,
  `FriendName` varchar(10) COLLATE utf8_bin DEFAULT NULL,
  `wDate` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `Subject` varchar(50) COLLATE utf8_bin DEFAULT NULL,
  `bRead` tinyint(1) NOT NULL DEFAULT '0',
  `Memo` blob,
  `Photo` blob,
  `Dir` tinyint(3) unsigned DEFAULT '0',
  `Act` tinyint(3) unsigned DEFAULT '0',
  PRIMARY KEY (`GUID`,`MemoIndex`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_friendmain`
--

DROP TABLE IF EXISTS `t_friendmain`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_friendmain` (
  `GUID` int(11) NOT NULL,
  `Name` varchar(10) COLLATE utf8_bin NOT NULL,
  `FriendCount` tinyint(3) unsigned DEFAULT NULL,
  `MemoCount` int(11) DEFAULT '10',
  `MemoTotal` int(11) DEFAULT '0',
  PRIMARY KEY (`GUID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_gmsystem`
--

DROP TABLE IF EXISTS `t_gmsystem`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_gmsystem` (
  `Name` varchar(10) COLLATE utf8_bin NOT NULL,
  `AuthorityMask` int(11) NOT NULL DEFAULT '0',
  `Expiry` datetime DEFAULT NULL,
  PRIMARY KEY (`Name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_ingameshop_items`
--

DROP TABLE IF EXISTS `t_ingameshop_items`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_ingameshop_items` (
  `AccountID` varchar(10) COLLATE utf8_bin NOT NULL,
  `UniqueCode` int(11) NOT NULL AUTO_INCREMENT,
  `AuthCode` int(11) NOT NULL,
  `UniqueID1` int(11) NOT NULL,
  `UniqueID2` int(11) NOT NULL,
  `UniqueID3` int(11) NOT NULL,
  `InventoryType` int(11) NOT NULL,
  `GiftName` varchar(10) COLLATE utf8_bin DEFAULT NULL,
  `Message` varchar(200) COLLATE utf8_bin DEFAULT NULL,
  `UsedItem` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`UniqueCode`)
) ENGINE=InnoDB AUTO_INCREMENT=36 DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_ingameshop_point`
--

DROP TABLE IF EXISTS `t_ingameshop_point`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_ingameshop_point` (
  `AccountID` varchar(10) COLLATE utf8_bin NOT NULL,
  `WCoinP` double NOT NULL DEFAULT '0',
  `WCoinC` double NOT NULL DEFAULT '0',
  `GoblinPoint` double NOT NULL DEFAULT '0',
  PRIMARY KEY (`AccountID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_lucky_item_info`
--

DROP TABLE IF EXISTS `t_lucky_item_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_lucky_item_info` (
  `nIndex` int(11) NOT NULL AUTO_INCREMENT,
  `UserGuid` int(11) NOT NULL,
  `CharName` varchar(10) COLLATE utf8_bin NOT NULL,
  `ItemCode` int(11) NOT NULL,
  `ItemSerial` bigint(20) NOT NULL,
  `DurabilitySmall` smallint(6) NOT NULL,
  PRIMARY KEY (`nIndex`,`CharName`,`ItemCode`,`ItemSerial`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_minesystem`
--

DROP TABLE IF EXISTS `t_minesystem`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_minesystem` (
  `CharacterName` varchar(10) COLLATE utf8_bin NOT NULL,
  `TwinkleType` int(11) NOT NULL,
  `CurrentStage` int(11) NOT NULL,
  PRIMARY KEY (`CharacterName`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_mu2003_event`
--

DROP TABLE IF EXISTS `t_mu2003_event`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_mu2003_event` (
  `AccountID` varchar(50) COLLATE utf8_bin NOT NULL,
  `EventChips` smallint(6) NOT NULL DEFAULT '0',
  `MuttoIndex` int(11) NOT NULL DEFAULT '-1',
  `MuttoNumber` int(11) NOT NULL DEFAULT '0',
  `Check_Code` char(1) COLLATE utf8_bin NOT NULL DEFAULT '0',
  PRIMARY KEY (`AccountID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_murummy`
--

DROP TABLE IF EXISTS `t_murummy`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_murummy` (
  `AccountID` varchar(10) COLLATE utf8_bin NOT NULL,
  `Name` varchar(10) COLLATE utf8_bin NOT NULL,
  `mSequence` int(11) DEFAULT NULL,
  `mNumber` int(11) DEFAULT NULL,
  `mColor` int(11) DEFAULT NULL,
  `mPosition` int(11) DEFAULT NULL,
  `mStatus` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_murummyinfo`
--

DROP TABLE IF EXISTS `t_murummyinfo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_murummyinfo` (
  `AccountID` varchar(10) COLLATE utf8_bin NOT NULL,
  `Name` varchar(10) COLLATE utf8_bin NOT NULL,
  `mTotalScore` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_murummylog`
--

DROP TABLE IF EXISTS `t_murummylog`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_murummylog` (
  `mDate` datetime NOT NULL,
  `AccountID` varchar(10) COLLATE utf8_bin NOT NULL,
  `Name` varchar(10) COLLATE utf8_bin NOT NULL,
  `mScore` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_pentagraminfo`
--

DROP TABLE IF EXISTS `t_pentagraminfo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_pentagraminfo` (
  `UserGuid` int(11) NOT NULL,
  `AccountID` varchar(10) COLLATE utf8_bin NOT NULL,
  `Name` varchar(10) COLLATE utf8_bin DEFAULT NULL,
  `JewelPos` tinyint(3) unsigned NOT NULL,
  `JewelIndex` smallint(6) NOT NULL DEFAULT '255',
  `ItemType` smallint(6) NOT NULL DEFAULT '255',
  `ItemIndex` int(11) NOT NULL DEFAULT '511',
  `MainAttribute` smallint(6) NOT NULL DEFAULT '255',
  `JewelLevel` smallint(6) NOT NULL,
  `Rank1` tinyint(3) unsigned NOT NULL DEFAULT '255',
  `Rank1Level` tinyint(3) unsigned DEFAULT NULL,
  `Rank2` tinyint(3) unsigned NOT NULL DEFAULT '255',
  `Rank2Level` tinyint(3) unsigned DEFAULT NULL,
  `Rank3` tinyint(3) unsigned NOT NULL DEFAULT '255',
  `Rank3Level` tinyint(3) unsigned DEFAULT NULL,
  `Rank4` tinyint(3) unsigned NOT NULL DEFAULT '255',
  `Rank4Level` tinyint(3) unsigned DEFAULT NULL,
  `Rank5` tinyint(3) unsigned NOT NULL DEFAULT '255',
  `Rank5Level` tinyint(3) unsigned DEFAULT NULL,
  `RegDate` datetime DEFAULT '0000-00-00 00:00:00'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_petitem_info`
--

DROP TABLE IF EXISTS `t_petitem_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_petitem_info` (
  `ItemSerial` bigint(20) NOT NULL,
  `Pet_Level` smallint(6) DEFAULT '0',
  `Pet_Exp` bigint(20) DEFAULT '0',
  PRIMARY KEY (`ItemSerial`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_pshop_itemvalue_info`
--

DROP TABLE IF EXISTS `t_pshop_itemvalue_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_pshop_itemvalue_info` (
  `AccountID` varchar(10) COLLATE utf8_bin NOT NULL,
  `Name` varchar(10) COLLATE utf8_bin NOT NULL,
  `ItemInvenNum` int(11) NOT NULL DEFAULT '0',
  `ItemSerial` bigint(20) NOT NULL DEFAULT '0',
  `Money` int(11) NOT NULL DEFAULT '0',
  `BlessJewelValue` int(11) NOT NULL DEFAULT '0',
  `SoulJewelValue` int(11) NOT NULL DEFAULT '0',
  `ChaosJewelValue` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`Name`,`ItemInvenNum`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_quest_exp_info`
--

DROP TABLE IF EXISTS `t_quest_exp_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_quest_exp_info` (
  `nINDEX` int(11) NOT NULL AUTO_INCREMENT,
  `CHAR_NAME` varchar(10) COLLATE utf8_bin NOT NULL,
  `EPISODE` smallint(6) NOT NULL,
  `QUEST_SWITCH` smallint(6) NOT NULL,
  `ASK_INDEX` blob NOT NULL,
  `ASK_VALUE` blob NOT NULL,
  `ASK_STATE` blob NOT NULL,
  `PROG_STATE` smallint(6) DEFAULT NULL,
  `QUEST_START_DATE` datetime DEFAULT '1753-01-01 00:00:00',
  `QUEST_END_DATE` datetime DEFAULT '1753-01-01 00:00:00',
  `StartDateConvert` bigint(20) NOT NULL DEFAULT '0',
  `EndDateConvert` bigint(20) NOT NULL DEFAULT '0',
  PRIMARY KEY (`nINDEX`,`CHAR_NAME`)
) ENGINE=InnoDB AUTO_INCREMENT=12 DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_viplist`
--

DROP TABLE IF EXISTS `t_viplist`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_viplist` (
  `AccountID` varchar(10) COLLATE utf8_bin NOT NULL,
  `Date` datetime DEFAULT NULL,
  `Type` tinyint(3) unsigned DEFAULT NULL,
  PRIMARY KEY (`AccountID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_waitfriend`
--

DROP TABLE IF EXISTS `t_waitfriend`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_waitfriend` (
  `GUID` int(11) NOT NULL,
  `FriendGuid` int(11) NOT NULL,
  `FriendName` varchar(10) COLLATE utf8_bin NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `warehouse`
--

DROP TABLE IF EXISTS `warehouse`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `warehouse` (
  `AccountID` varchar(10) COLLATE utf8_bin NOT NULL,
  `Items` blob,
  `Money` int(11) DEFAULT '0',
  `EndUseDate` datetime DEFAULT NULL,
  `pw` smallint(6) DEFAULT '0',
  `Expanded` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `WHOpen` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`AccountID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `zenevent`
--

DROP TABLE IF EXISTS `zenevent`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `zenevent` (
  `Guid` int(11) NOT NULL AUTO_INCREMENT,
  `AccountID` varchar(10) COLLATE utf8_bin NOT NULL,
  `Zen` int(11) DEFAULT '0',
  PRIMARY KEY (`Guid`,`AccountID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2019-02-02  7:35:17
