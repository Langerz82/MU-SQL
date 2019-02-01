DELIMITER $$
CREATE PROCEDURE `IGC_ArcaBattleGuildInsert`(
 p_GuildName nvarchar(8),      
 p_CharName nvarchar(10),      
 p_Number int)
BEGIN  

 DECLARE v_return int;
 DECLARE v_GuildNum int;
 DECLARE v_GuildGroupNum tinyint unsigned;
 SET v_return = 0;  
     
finish:BEGIN

SELECT Number INTO v_GuildNum FROM IGC_ARCA_BATTLE_GUILD_JOIN_INFO;
SET v_GuildGroupNum = FOUND_ROWS();

IF( v_GuildGroupNum >= 6 ) THEN
   SET  v_return = 3;
ELSE
   SELECT v_return;
   LEAVE finish;
END IF;

IF NOT EXISTS ( SELECT Number FROM `IGC_ARCA_BATTLE_GUILD_JOIN_INFO` WHERE `Number` =  p_Number ) THEN 
	BEGIN 
		start transaction;      
	  INSERT INTO IGC_ARCA_BATTLE_GUILD_JOIN_INFO (G_Name, G_Master, Number, JoinDate, GroupNum) VALUES      
	   (  p_GuildName, p_CharName, p_Number, NOW(), v_GuildGroupNum+1 );

	  INSERT INTO IGC_ARCA_BATTLE_MEMBER_JOIN_INFO (G_Name, Number, CharName, JoinDate) VALUES      
	   (  p_GuildName, p_Number, p_CharName, NOW() );       
        commit;
	END;
ELSE
	BEGIN
		SET  v_return = 4;
		SELECT v_return;
	END;
END IF;

END;

END$$
DELIMITER ;
