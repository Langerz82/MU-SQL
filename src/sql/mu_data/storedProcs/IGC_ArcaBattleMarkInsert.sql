DELIMITER $$
CREATE PROCEDURE `IGC_ArcaBattleMarkInsert`(
 p_G_Name nvarchar(8),      
 p_G_Number int,
 p_G_Master nvarchar(10),      
 p_MarkCnt bigint)
sp_lbl:
     
BEGIN  

 DECLARE v_return int;
 DECLARE v_GuildRegRank int;
 DECLARE v_CurrMarkCnt int;
  DECLARE EXIT HANDLER FOR SQLEXCEPTION
  BEGIN
    ROLLBACK;
    SET  v_return = -1;
  END;
  
 SET v_return = 0;  

EndProc: BEGIN

IF NOT EXISTS ( SELECT G_Number FROM IGC_ARCA_BATTLE_GUILDMARK_REG WHERE G_Number =  p_G_Number ) THEN
  BEGIN

	SET v_GuildRegRank = (SELECT count (G_Name) FROM IGC_ARCA_BATTLE_GUILDMARK_REG);
	IF( v_GuildRegRank >= 250 )
	THEN
	   SET  v_return = 3;
	   LEAVE EndProc;
	END IF;

   start transaction;      
  INSERT INTO IGC_ARCA_BATTLE_GUILDMARK_REG (G_Name, G_Number, G_Master, RegDate, GuildRegRank, MarkCnt ) VALUES      
   (  p_G_Name, p_G_Number, p_G_Master, NOW(), (v_GuildRegRank+1), p_MarkCnt );

  END;
ELSE
 BEGIN
	SET v_CurrMarkCnt  = (SELECT MarkCnt from IGC_ARCA_BATTLE_GUILDMARK_REG WHERE G_Number = p_G_Number AND G_Name = p_G_Name);
	 start transaction;
	 UPDATE IGC_ARCA_BATTLE_GUILDMARK_REG SET MarkCnt = (p_MarkCnt + v_CurrMarkCnt)  WHERE G_Number = p_G_Number AND G_Name = p_G_Name;
	 SET v_return = 1;
	LEAVE EndProc;
 END;
END IF;

END EndProc;


select v_return;

END$$
DELIMITER ;
