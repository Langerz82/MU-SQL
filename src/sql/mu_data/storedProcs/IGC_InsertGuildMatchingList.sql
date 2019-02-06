DELIMITER $$
CREATE PROCEDURE `IGC_InsertGuildMatchingList`(
   p_GuildName nvarchar(8),
   p_GuildMasterName nvarchar(10),
   p_GuildNum	int,
   p_GuildMasterLevel smallint,
   p_GuildMasterClass tinyint unsigned,
   p_GuildMemberCnt smallint,
   p_Memo nvarchar(40),
   p_InterestType tinyint unsigned,
   p_LevelRange tinyint unsigned,
   p_ClassType tinyint unsigned,
   p_GensType tinyint unsigned)
sp_lbl:
 
BEGIN

	SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED;

	REPLACE INTO IGC_GuildMatching(GuildName,GuildNum,GuildMasterName,GuildMasterLevel,GuildMasterClass,
	GuildMemberCnt,Memo,InterestType,LevelRange,ClassType,GensType,RegDate)
	values(p_GuildName,p_GuildNum,p_GuildMasterName,p_GuildMasterLevel, p_GuildMasterClass, p_GuildMemberCnt,
	p_Memo,p_InterestType,p_LevelRange,p_ClassType,p_GensType,NOW());

   
END$$
DELIMITER ;
