DELIMITER $$
CREATE PROCEDURE `WZ_CreateCharacter`( 

	p_AccountID		nvarchar(10),
	p_Name			nvarchar(10),
	p_Class			tinyint unsigned)
BEGIN
	DECLARE	v_Result tinyint;
    DECLARE v_g1 nvarchar(10);
    Declare v_g2 nvarchar(10);
    Declare v_g3 nvarchar(10);
    Declare v_g4 nvarchar(10);
    Declare v_g5 nvarchar(10);
	DECLARE EXIT HANDLER FOR SQLEXCEPTION
	BEGIN
		ROLLBACK;
        SET v_Result = 3;
	END;    
	SET v_Result = 0;

myMainLoop: BEGIN
	IF EXISTS ( SELECT Name FROM `Character` WHERE Name = p_Name ) THEN
		SET v_Result	= 1;
		LEAVE myMainLoop;					
	END IF;

	START TRANSACTION;

	IF NOT EXISTS ( SELECT Id FROM  AccountCharacter WHERE Id = p_AccountID )
		THEN
			INSERT INTO AccountCharacter(Id, GameID1, GameID2, GameID3, GameID4, GameID5, GameIDC) 
			VALUES(p_AccountID, p_Name, NULL, NULL, NULL, NULL, NULL);
	ELSE			
			SELECT GameID1, GameID2, GameID3, GameID4, GameID5 INTO v_g1, v_g2, v_g3, v_g4, v_g5 FROM AccountCharacter Where Id = p_AccountID; 			

			IF( ( v_g1 Is NULL) OR (Char_length(rtrim(v_g1)) = 0)) THEN
					UPDATE AccountCharacter SET  GameID1 = p_Name
					WHERE Id = p_AccountID;
			ELSEIF ( v_g2  Is NULL OR Char_length(rtrim(v_g2)) = 0) THEN
					UPDATE AccountCharacter SET  GameID2 = p_Name
					WHERE Id = p_AccountID;
			ELSEIF ( v_g3  Is NULL OR Char_length(rtrim(v_g3)) = 0) THEN	
					UPDATE AccountCharacter SET  GameID3 = p_Name
					WHERE Id = p_AccountID;
			ELSEIF ( v_g4 Is NULL OR Char_length(rtrim(v_g4)) = 0) THEN
					UPDATE AccountCharacter SET  GameID4 = p_Name
					WHERE Id = p_AccountID;
			ELSEIF ( v_g5 Is NULL OR Char_length(rtrim(v_g5)) = 0) THEN
					UPDATE AccountCharacter SET  GameID5 = p_Name
					WHERE Id = p_AccountID;
			ELSE
					SET v_Result	= 3;							
					LEAVE myMainLoop;
			END IF;	 
	END IF;

	IF( v_Result <> 0 ) THEN
		LEAVE myMainLoop;		
	ELSE
		INSERT INTO `Character`(AccountID, `Name`, cLevel, LevelUpPoint, Class, Strength, Dexterity, Vitality, Energy, Inventory,MagicList,
				Life, MaxLife, Mana, MaxMana, MapNumber, MapPosX, MapPosY,  MDate, LDate, Quest, Leadership )
			
		SELECT p_AccountID As AccountID, p_Name As `Name`, `Level`, LevelUpPoint, p_Class As Class, 
			Strength, Dexterity, Vitality, Energy, Inventory,MagicList, Life, MaxLife, Mana, MaxMana, MapNumber, MapPosX, MapPosY,
			now() As MDate, now() As LDate, Quest, Leadership
		FROM  DefaultClassType WHERE Class = p_Class;
	END IF;
		
	IF NOT EXISTS ( SELECT AccountID FROM T_InGameShop_Point WHERE AccountID = p_AccountID ) THEN
		   INSERT INTO T_InGameShop_Point (AccountID, WCoinC, WCoinP, GoblinPoint)
		   VALUES (p_AccountID, 0, 0, 0);
	END IF;
	
    COMMIT;

END myMainLoop;

SELECT CASE v_Result
	  WHEN 0 THEN 1
	  WHEN 1 THEN 0
	  WHEN 3 THEN 3
	  ELSE 2
END AS Result; 

END$$
DELIMITER ;
