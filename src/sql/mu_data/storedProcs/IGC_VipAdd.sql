DELIMITER $$
CREATE PROCEDURE `IGC_VipAdd`(
 p_szCharName nvarchar(10),
 p_DayAdd int,
 p_VipType smallint)
BEGIN
	
	REPLACE T_VIPList SET 
			`Date` = TIMESTAMPADD(DAY, p_DayAdd, NOW()),
            `Type` = p_VipType,
            AccountID = p_szCharName;

	IF (ROW_COUNT() = 0) THEN
		REPLACE INTO T_VIPList (AccountID, `Date`, `Type`)
		VALUES (p_szCharName, p_VipType, TIMESTAMPADD(DAY, p_DayAdd, NOW()));
    END IF;

END$$
DELIMITER ;
