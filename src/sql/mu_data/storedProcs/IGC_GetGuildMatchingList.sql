DELIMITER $$
CREATE PROCEDURE `IGC_GetGuildMatchingList`(
   p_page int,
   p_pageSize int)
BEGIN

	

	DECLARE v_nTemp int; DECLARE v_nPageSize int;
       DECLARE v_lastIdentNo int;
       DECLARE ROWCOUNT int;
       
	SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED;

       set v_nTemp = (((p_page - 1) * p_pageSize) + 1); 
       set v_nPageSize = p_pageSize;

       set ROWCOUNT = v_nTemp;

       select identNo into v_lastIdentNo from IGC_GuildMatching order by identNo desc LIMIT ROWCOUNT;

       set ROWCOUNT = v_nPageSize;
  
       select  
	   identNo, GuildName, GuildNum, GuildMasterName, GuildMasterLevel, GuildMasterClass, GuildMemberCnt, Memo, InterestType, LevelRange, ClassType, GensType, RegDate
	   from IGC_GuildMatching where identNo <= v_lastIdentNo 
       order by identNo desc LIMIT ROWCOUNT;  
    
 end$$
DELIMITER ;
