DELIMITER $$
CREATE PROCEDURE `IGC_GensDelete`(
	p_szCharName nvarchar(10))
BEGIN
	  DECLARE EXIT HANDLER FOR SQLEXCEPTION
	  BEGIN
		ROLLBACK;
	  END;
      
	START TRANSACTION;

	myLoop : BEGIN
    IF NOT EXISTS ( SELECT Name FROM IGC_Gens WHERE Name = p_szCharName ) THEN
		SELECT 0x01;
		LEAVE myLoop;
	END IF;
	IF EXISTS ( SELECT G_Name FROM Guild WHERE G_Master = p_szCharName) THEN
		SELECT 0x02;
        LEAVE myLoop;
	END IF;
    
	UPDATE IGC_Gens SET Influence = 0, Points = 0, Rank = 0, Reward = 0, LeaveDate = NOW() WHERE Name = p_szCharName;
	IF EXISTS (SELECT Name from IGC_GensAbuse WHERE Name = p_szCharName) THEN
		DELETE FROM IGC_GensAbuse WHERE Name = p_szCharName;
	END IF;
	SELECT 0x00;

	END;
    
	commit;
END$$
DELIMITER ;
