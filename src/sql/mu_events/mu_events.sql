CREATE DATABASE  IF NOT EXISTS `mu_events` /*!40100 DEFAULT CHARACTER SET latin1 */;
USE `mu_events`;
-- MySQL dump 10.13  Distrib 5.7.17, for Win64 (x86_64)
--
-- Host: localhost    Database: mu_events
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
-- Table structure for table `event_info_bc_3rd`
--

DROP TABLE IF EXISTS `event_info_bc_3rd`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_info_bc_3rd` (
  `Server` tinyint(3) unsigned NOT NULL,
  `Bridge` tinyint(3) unsigned NOT NULL,
  `AccountID` varchar(10) COLLATE utf8_bin NOT NULL,
  `CharacterName` varchar(10) COLLATE utf8_bin NOT NULL,
  `Class` tinyint(3) unsigned NOT NULL,
  `Point` int(11) NOT NULL,
  `PlayCount` int(11) NOT NULL,
  `SumLeftTime` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`Server`,`Bridge`,`AccountID`,`CharacterName`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_info_bc_4th`
--

DROP TABLE IF EXISTS `event_info_bc_4th`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_info_bc_4th` (
  `Server` tinyint(3) unsigned NOT NULL,
  `Bridge` tinyint(3) unsigned NOT NULL,
  `AccountID` varchar(10) COLLATE utf8_bin NOT NULL,
  `CharacterName` varchar(10) COLLATE utf8_bin NOT NULL,
  `Class` tinyint(3) unsigned NOT NULL,
  `Point` int(11) NOT NULL,
  `PlayCount` int(11) NOT NULL,
  `SumLeftTime` int(11) NOT NULL DEFAULT '0',
  `MinLeftTime` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`Server`,`Bridge`,`AccountID`,`CharacterName`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_info_bc_5th`
--

DROP TABLE IF EXISTS `event_info_bc_5th`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_info_bc_5th` (
  `Server` tinyint(3) unsigned NOT NULL,
  `Bridge` tinyint(3) unsigned NOT NULL,
  `AccountID` varchar(10) COLLATE utf8_bin NOT NULL,
  `CharacterName` varchar(10) COLLATE utf8_bin NOT NULL,
  `Class` tinyint(3) unsigned NOT NULL,
  `Point` int(11) NOT NULL,
  `PlayCount` int(11) NOT NULL,
  `SumLeftTime` int(11) NOT NULL DEFAULT '0',
  `MinLeftTime` int(11) NOT NULL DEFAULT '0',
  `RegDate` datetime NOT NULL,
  `AlivePartyCount` int(11) NOT NULL,
  `MaxPointLeftTime` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`Server`,`Bridge`,`AccountID`,`CharacterName`,`RegDate`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_cc_offline_gift`
--

DROP TABLE IF EXISTS `t_cc_offline_gift`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_cc_offline_gift` (
  `Guid` int(11) NOT NULL AUTO_INCREMENT,
  `Server` int(11) DEFAULT NULL,
  `AccountID` varchar(10) COLLATE utf8_bin DEFAULT NULL,
  `CharName` varchar(10) COLLATE utf8_bin DEFAULT NULL,
  `GiftKind` int(11) NOT NULL,
  `Date_Give` datetime NOT NULL,
  `Date_Reg` datetime DEFAULT '0000-00-00 00:00:00',
  `RegCheck` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`Guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_cc_offline_giftname`
--

DROP TABLE IF EXISTS `t_cc_offline_giftname`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_cc_offline_giftname` (
  `GiftKind` int(11) NOT NULL,
  `GiftName` varchar(50) COLLATE utf8_bin NOT NULL,
  PRIMARY KEY (`GiftKind`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_kanturutimeattackevent2006`
--

DROP TABLE IF EXISTS `t_kanturutimeattackevent2006`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_kanturutimeattackevent2006` (
  `AccountID` varchar(10) COLLATE utf8_bin NOT NULL,
  `Name` varchar(10) COLLATE utf8_bin NOT NULL,
  `ServerCode` smallint(6) NOT NULL,
  `BattleID` varchar(13) COLLATE utf8_bin NOT NULL,
  `StageNum` tinyint(3) unsigned NOT NULL,
  `ClearTime` int(11) NOT NULL,
  `Level` int(11) NOT NULL,
  `Experience` int(11) NOT NULL,
  `RegDate` datetime NOT NULL DEFAULT '0000-00-00 00:00:00'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_luckycoin`
--

DROP TABLE IF EXISTS `t_luckycoin`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_luckycoin` (
  `AccountID` varchar(10) COLLATE utf8_bin NOT NULL,
  `LuckyCoin` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`AccountID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_regcount_check`
--

DROP TABLE IF EXISTS `t_regcount_check`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_regcount_check` (
  `AccountID` varchar(10) COLLATE utf8_bin NOT NULL,
  `RegCount` int(11) NOT NULL DEFAULT '1',
  `RegAlready` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`AccountID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_register_info`
--

DROP TABLE IF EXISTS `t_register_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_register_info` (
  `F_Register_Section` smallint(6) NOT NULL,
  `F_Register_Name` varchar(50) COLLATE utf8_bin NOT NULL,
  `F_Register_TotalCount` int(11) NOT NULL,
  PRIMARY KEY (`F_Register_Section`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_santaevent`
--

DROP TABLE IF EXISTS `t_santaevent`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_santaevent` (
  `AccountID` varchar(10) COLLATE utf8_bin NOT NULL,
  `VisitCount` int(11) NOT NULL DEFAULT '0',
  `LastVisitDate` datetime DEFAULT NULL,
  `Server` smallint(6) DEFAULT NULL,
  PRIMARY KEY (`AccountID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_serial_bank`
--

DROP TABLE IF EXISTS `t_serial_bank`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_serial_bank` (
  `F_Serial_Guid` int(11) NOT NULL AUTO_INCREMENT,
  `P_Serial_1` char(4) COLLATE utf8_bin NOT NULL,
  `P_Serial_2` char(4) COLLATE utf8_bin NOT NULL,
  `P_Serial_3` char(4) COLLATE utf8_bin NOT NULL,
  `F_Serial_Section` smallint(6) NOT NULL,
  `F_Member_Guid` int(11) DEFAULT NULL,
  `F_Member_Id` char(10) COLLATE utf8_bin DEFAULT NULL,
  `F_Register_Section` smallint(6) DEFAULT NULL,
  `F_Register_Date` datetime DEFAULT NULL,
  `F_Create_Date` datetime NOT NULL,
  `F_RegisterCheck` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`F_Serial_Guid`,`P_Serial_1`,`P_Serial_2`,`P_Serial_3`)
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

-- Dump completed on 2019-02-02  7:36:10
