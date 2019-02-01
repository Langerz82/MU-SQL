DELIMITER $$
CREATE PROCEDURE `IGC_ArcaBattleGuildMemberInsert`(
 p_GuildName nvarchar(8),      
 p_CharName nvarchar(10),      
 p_Number int)
BEGIN  

finish: BEGIN

DECLARE v_return int;
DECLARE v_GuildNum int;
DECLARE EXIT HANDLER FOR SQLEXCEPTION
	BEGIN
	rollback;
	SET v_return = -1;
	END;

SET v_return = 0;  

SELECT `Number` INTO v_GuildNum FROM IGC_ARCA_BATTLE_GUILD_JOIN_INFO WHERE Number =  p_Number;
IF( FOUND_ROWS()  < 1 ) THEN
   SET  v_return = 7;
   LEAVE finish;
END IF;

SELECT `Number` INTO v_GuildNum FROM IGC_ARCA_BATTLE_MEMBER_JOIN_INFO WHERE Number =  p_Number;
IF( FOUND_ROWS()  >= 30 ) THEN
   SET  v_return = 9;
   LEAVE finish;
END IF;

IF NOT EXISTS ( SELECT CharName FROM IGC_ARCA_BATTLE_MEMBER_JOIN_INFO WHERE CharName =  p_CharName ) THEN  
	  BEGIN
		  start transaction; 
		  INSERT INTO IGC_ARCA_BATTLE_MEMBER_JOIN_INFO (G_Name, Number, CharName, JoinDate) VALUES      
		   (  p_GuildName, p_Number, p_CharName, NOW() );
		  commit;
	   END;
ELSE
	 BEGIN
	   SET  v_return = 8;
	   LEAVE finish;
	 END;
END IF;

END finish;
SELECT v_return;

END$$
DELIMITER ;
