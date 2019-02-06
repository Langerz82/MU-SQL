DELIMITER $$
CREATE PROCEDURE `IGC_GensAdd`(
	p_szCharName nvarchar(10),
	p_Influence tinyint unsigned,
	p_LeaveDayDifference smallint)
BEGIN
	DECLARE v_GensLeaveDate datetime;
	  DECLARE EXIT HANDLER FOR SQLEXCEPTION
	  BEGIN
		ROLLBACK;
	  END;
  
	START TRANSACTION;

	IF NOT EXISTS ( SELECT Name FROM IGC_Gens WHERE Name = p_szCharName ) THEN
		BEGIN
			INSERT INTO IGC_Gens (Name, Influence, Rank, LeaveDate)  VALUES (p_szCharName, p_Influence, 0, NULL);
			SELECT 0x00;
		END;
	ELSE
	BEGIN
		DECLARE v_CurInfluence tinyint unsigned;
		SELECT Influence INTO v_CurInfluence FROM IGC_Gens WHERE Name = p_szCharName;

		IF (v_CurInfluence > 0)
		THEN
			SELECT 0x01;
		ELSE
			
			SELECT LeaveDate INTO v_GensLeaveDate FROM IGC_Gens WHERE Name = p_szCharName;
			IF (TIMESTAMPDIFF(DAY, v_GensLeaveDate, NOW()) < p_LeaveDayDifference AND p_LeaveDayDifference != 0)
			THEN
				SELECT 0x02;
			ELSE
				UPDATE IGC_Gens SET Influence = p_Influence, Rank = 0, Reward = 0, Points = 0, LeaveDate = NULL WHERE Name = p_szCharName;
				SELECT 0x00;
			END IF;
		END IF;
	END;
    END IF;
	COMMIT;
END$$
DELIMITER ;
