DELIMITER $$
CREATE PROCEDURE `IGC_ArcaBattleGuildGroupNumSelect`(  
 p_CharName nvarchar(10))
BEGIN      
    
 DECLARE v_GuildName nvarchar(8);
 DECLARE v_return int;    
 SET v_return = 0;      
         

set v_GuildName = (SELECT G_Name FROM IGC_ARCA_BATTLE_MEMBER_JOIN_INFO where CharName = p_CharName);

 SELECT GroupNum FROM IGC_ARCA_BATTLE_GUILD_JOIN_INFO
  WHERE G_Name =  v_GuildName;

-- SET XACT_ABORT OFF    
      
END$$
DELIMITER ;
