DELIMITER $$
CREATE PROCEDURE `IGC_PeriodItemExpiredItemSelectEx`(
 -- Add the parameters for the stored procedure here
 p_UserGUID int,
 p_CharacterName nvarchar(10))
BEGIN
	DECLARE v_SetExpire int;
	SET v_SetExpire = 0;

	
	UPDATE IGC_PeriodItemInfo SET SetExpire = 2 WHERE UserGUID = p_UserGUID AND CharacterName = p_CharacterName AND ItemType = 2 AND `ExpireDateString` < NOW() AND SetExpire = 1;

	UPDATE IGC_PeriodItemInfo SET SetExpire = 1 WHERE UserGUID = p_UserGUID AND CharacterName = p_CharacterName AND ItemType = 2 AND `ExpireDateString` < NOW() AND SetExpire = 0;

	SELECT * FROM IGC_PeriodItemInfo WHERE UserGUID = p_UserGUID AND CharacterName = p_CharacterName AND ItemType = 2 AND SetExpire = 1;

END$$
DELIMITER ;
