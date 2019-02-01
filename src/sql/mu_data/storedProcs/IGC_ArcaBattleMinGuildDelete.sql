DELIMITER $$
CREATE PROCEDURE `IGC_ArcaBattleMinGuildDelete`(
  p_G_Name nvarchar(8))
BEGIN

 DECLARE v_return int;
 DECLARE EXIT HANDLER FOR SQLEXCEPTION
 BEGIN
   ROLLBACK;
   SET v_return = -1;
 END;
 SET v_return = 0;

 start transaction;  

 DELETE FROM IGC_ARCA_BATTLE_GUILD_JOIN_INFO WHERE G_Name = p_G_Name;
 DELETE FROM IGC_ARCA_BATTLE_MEMBER_JOIN_INFO WHERE G_Name = p_G_Name;

 commit;
 SELECT v_return;

END$$
DELIMITER ;
