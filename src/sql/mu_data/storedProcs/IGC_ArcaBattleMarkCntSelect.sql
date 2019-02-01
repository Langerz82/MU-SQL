DELIMITER $$
CREATE PROCEDURE `IGC_ArcaBattleMarkCntSelect`(
 p_G_Number int)
BEGIN 

 DECLARE v_return int;
 DECLARE v_GuildRegRank bigint;

EndProc: BEGIN
 SET v_return = 0;  
     

IF NOT EXISTS ( SELECT G_Number FROM IGC_ARCA_BATTLE_GUILDMARK_REG WHERE G_Number =  p_G_Number) THEN
  BEGIN

	SET v_GuildRegRank = (SELECT count (G_Number) FROM IGC_ARCA_BATTLE_GUILDMARK_REG);
	IF( v_GuildRegRank >= 250 ) THEN
	   SET  v_return = -1;
	   LEAVE EndProc;
	 END IF;	  
   END;
ELSE
  BEGIN
	SET v_return = (SELECT MarkCnt FROM IGC_ARCA_BATTLE_GUILDMARK_REG WHERE G_Number =  p_G_Number);
  END;
END IF;

END;
	SELECT v_return;
END$$
DELIMITER ;
