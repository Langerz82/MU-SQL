DELIMITER $$
CREATE PROCEDURE `IGC_InsertGuildMatchingWaitList`(

   p_GuildNumber	int,
   p_ApplicantName nvarchar(10),
   p_ApplicantClass tinyint unsigned,
   p_ApplicantLevel smallint,
   p_State tinyint unsigned)
sp_lbl:
 
BEGIN
	DECLARE v_aErrNo INT; 
	DECLARE v_aRowCnt INT;
    declare v_GuildName nvarchar(8);
	declare  v_GuildMasterName nvarchar(10);

	SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED;
	SET v_aErrNo = 0; 
    SET v_aRowCnt = 0;

    set  v_GuildName = (select GuildName from IGC_GuildMatching where GuildNum = p_GuildNumber LIMIT 1);
    set  v_GuildMasterName = (select GuildMasterName from IGC_GuildMatching where GuildNum = p_GuildNumber LIMIT 1);

		REPLACE INTO IGC_WaitGuildMatching (GuildName, GuildMasterName, GuildNumber, ApplicantName, ApplicantClass,
		ApplicantLevel,State,RegDate) values(v_GuildName, v_GuildMasterName,p_GuildNumber,p_ApplicantName,p_ApplicantClass,
		p_ApplicantLevel,p_State,NOW());   
END$$
DELIMITER ;
