DELIMITER $$
CREATE PROCEDURE `IGC_PeriodItemInsertEx`(
 p_UserGUID int,
 p_CharacterName nvarchar(10),
 p_ItemType smallint,
 p_ItemCode smallint,
 p_EffectCategory smallint,
 p_EffectType1 smallint,
 p_EffectType2 smallint,
 p_Serial bigint,
 p_Duration int,
 p_BuyDate bigint,
 p_ExpireDate bigint)
BEGIN
	DECLARE v_ErrorCode int;
	DECLARE v_PeriodItemIndex int;
    DECLARE v_CalcExpireDate datetime;
  DECLARE EXIT HANDLER FOR SQLEXCEPTION
  BEGIN
    ROLLBACK;
    SELECT v_ErrorCode;
  END;
  
	SET v_ErrorCode = 0;
	SET v_PeriodItemIndex = 0;

	START TRANSACTION;

	SET v_CalcExpireDate = TIMESTAMPADD( second, p_Duration, NOW() );

	IF ( p_ItemType = 1 ) THEN
		SELECT id INTO v_PeriodItemIndex FROM IGC_PeriodItemInfo WHERE UserGUID = p_UserGUID AND CharacterName = p_CharacterName AND UsedInfo = 1 AND EffectCategory = p_EffectCategory;
		IF( v_PeriodItemIndex <> 0 ) THEN
			UPDATE IGC_PeriodItemInfo SET UsedInfo = 0 WHERE id = v_PeriodItemIndex;
		END IF;

		INSERT INTO IGC_PeriodItemInfo ( UserGUID, CharacterName, ItemCode, ItemType, EffectType1, EffectType2, Duration, BuyDateString, ExpireDateString, UsedInfo, EffectCategory, Serial, BuyDate, ExpireDate )
		VALUES
		( p_UserGUID, p_CharacterName, p_ItemCode, p_ItemType, p_EffectType1, p_EffectType2, p_Duration, NOW(), v_CalcExpireDate, 1, p_EffectCategory, p_Serial, p_BuyDate, p_ExpireDate );
		iF ( ROW_COUNT() = 0 ) THEN
			SET v_ErrorCode = 2;
		END IF;
	END IF;
	IF ( p_ItemType = 2 ) THEN
		INSERT INTO IGC_PeriodItemInfo ( UserGUID, CharacterName, ItemCode, ItemType, EffectType1, EffectType2, Duration, BuyDateString, ExpireDateString, UsedInfo, EffectCategory, Serial, BuyDate, ExpireDate )
		VALUES
		( p_UserGUID, p_CharacterName, p_ItemCode, p_ItemType, 0, 0, p_Duration, NOW(), v_CalcExpireDate, 1, 0, p_Serial, p_BuyDate, p_ExpireDate );
		iF ( ROW_COUNT() = 0 ) THEN
			SET v_ErrorCode = 2;
		END IF;
	END IF;

	COMMIT;
    
	SELECT v_ErrorCode;
END$$
DELIMITER ;
