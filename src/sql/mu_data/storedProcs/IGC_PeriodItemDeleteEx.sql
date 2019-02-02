DELIMITER $$
CREATE PROCEDURE `IGC_PeriodItemDeleteEx`(
	-- Add the parameters for the stored procedure here
	p_UserGUID int,
	p_CharacterName nvarchar(10),
	p_ItemType tinyint unsigned,
	p_ItemCode smallint,
	p_Serial bigint)
BEGIN
	DECLARE v_ErrorCode int;
	DECLARE v_ItemInfoCount int;

	SET v_ErrorCode = 0;
	SET v_ItemInfoCount = 0;


	IF ( p_ItemType = 1 ) THEN
		SELECT COUNT(*) INTO v_ItemInfoCount FROM IGC_PeriodItemInfo WHERE UserGUID = p_UserGUID AND ItemCode = p_ItemCode;
		IF ( v_ItemInfoCount < 1 ) THEN
			SET v_ErrorCode = 1;
		ELSE
			UPDATE IGC_PeriodItemInfo SET UsedInfo = 0 WHERE UserGUID = p_UserGUID AND CharacterName = p_CharacterName AND ItemCode = p_ItemCode;
		END IF;
	END IF;
	IF ( p_ItemType = 2 ) THEN
		SELECT COUNT(*) INTO v_ItemInfoCount FROM IGC_PeriodItemInfo WHERE UserGUID = p_UserGUID AND CharacterName = p_CharacterName AND ItemCode = p_ItemCode AND Serial = p_Serial;
		IF ( v_ItemInfoCount < 1 ) THEN
			SET v_ErrorCode = 1;
		ELSE
			UPDATE IGC_PeriodItemInfo SET UsedInfo = 0 WHERE UserGUID = p_UserGUID AND ItemCode = p_ItemCode AND Serial = p_Serial;
		END IF;
	END IF;
    SELECT v_ErrorCode;
END$$
DELIMITER ;
