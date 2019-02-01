CREATE DATABASE  IF NOT EXISTS `mu_ranking` /*!40100 DEFAULT CHARACTER SET latin1 */;
USE `mu_ranking`;
-- MySQL dump 10.13  Distrib 5.7.17, for Win64 (x86_64)
--
-- Host: localhost    Database: mu_ranking
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
-- Table structure for table `event_info`
--

DROP TABLE IF EXISTS `event_info`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_info` (
  `Server` smallint(6) NOT NULL,
  `Square` tinyint(3) unsigned NOT NULL,
  `AccountID` varchar(10) COLLATE utf8_bin NOT NULL,
  `CharacterName` varchar(10) COLLATE utf8_bin NOT NULL,
  `Class` tinyint(3) unsigned NOT NULL,
  `Point` int(11) NOT NULL,
  PRIMARY KEY (`Server`,`Square`,`AccountID`,`CharacterName`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_info_bc`
--

DROP TABLE IF EXISTS `event_info_bc`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_info_bc` (
  `Server` smallint(6) NOT NULL,
  `Bridge` tinyint(3) unsigned NOT NULL,
  `AccountID` varchar(10) COLLATE utf8_bin NOT NULL,
  `CharacterName` varchar(10) COLLATE utf8_bin NOT NULL,
  `Class` tinyint(3) unsigned NOT NULL,
  `Point` int(11) NOT NULL,
  `PlayCount` int(11) NOT NULL DEFAULT '1',
  PRIMARY KEY (`Server`,`Bridge`,`AccountID`,`CharacterName`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

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
-- Table structure for table `event_info_cc`
--

DROP TABLE IF EXISTS `event_info_cc`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_info_cc` (
  `Server` tinyint(3) unsigned NOT NULL,
  `Castle` tinyint(3) unsigned NOT NULL,
  `AccountID` varchar(10) COLLATE utf8_bin NOT NULL,
  `Name` varchar(10) COLLATE utf8_bin NOT NULL,
  `Class` int(11) DEFAULT NULL,
  `PKillCount` int(11) DEFAULT NULL,
  `MKillCount` int(11) DEFAULT NULL,
  `Experience` bigint(20) DEFAULT NULL,
  `Wins` int(11) DEFAULT NULL,
  PRIMARY KEY (`Server`,`Castle`,`AccountID`,`Name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `event_info_it`
--

DROP TABLE IF EXISTS `event_info_it`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `event_info_it` (
  `Server` tinyint(3) unsigned NOT NULL,
  `Temple` tinyint(3) unsigned NOT NULL,
  `AccountID` varchar(10) COLLATE utf8_bin NOT NULL,
  `Name` varchar(10) COLLATE utf8_bin NOT NULL,
  `TotalScore` int(11) DEFAULT NULL,
  `KillCount` int(11) DEFAULT NULL,
  `RelicsGivenCount` int(11) DEFAULT NULL,
  `Experience` bigint(20) DEFAULT NULL,
  `isWinner` tinyint(3) unsigned DEFAULT NULL,
  `PlayCount` smallint(6) NOT NULL DEFAULT '0',
  `Class` int(11) DEFAULT NULL,
  PRIMARY KEY (`Server`,`Temple`,`AccountID`,`Name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_bc_pcroom_playcount`
--

DROP TABLE IF EXISTS `t_bc_pcroom_playcount`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_bc_pcroom_playcount` (
  `PCROOM_GUID` int(11) NOT NULL,
  `AccountID` varchar(50) COLLATE utf8_bin NOT NULL,
  `PlayCount` int(11) NOT NULL DEFAULT '1',
  `Point` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`PCROOM_GUID`,`AccountID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_enter_check_bc`
--

DROP TABLE IF EXISTS `t_enter_check_bc`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_enter_check_bc` (
  `AccountID` varchar(10) COLLATE utf8_bin NOT NULL,
  `CharName` varchar(10) COLLATE utf8_bin NOT NULL,
  `ServerCode` smallint(6) NOT NULL,
  `ToDayEnterCount` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `LastEnterDate` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  PRIMARY KEY (`AccountID`,`CharName`,`ServerCode`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_bin;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `t_enter_check_illusion_temple`
--

DROP TABLE IF EXISTS `t_enter_check_illusion_temple`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `t_enter_check_illusion_temple` (
  `AccountID` varchar(10) COLLATE utf8_bin NOT NULL,
  `CharName` varchar(10) COLLATE utf8_bin NOT NULL,
  `ServerCode` tinyint(3) unsigned NOT NULL,
  `TodayEnterCount` tinyint(3) unsigned NOT NULL,
  `LastEnterDate` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  PRIMARY KEY (`AccountID`,`CharName`,`ServerCode`)
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

-- Dump completed on 2019-02-02  7:36:48
