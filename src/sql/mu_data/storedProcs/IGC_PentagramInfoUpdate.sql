DELIMITER $$
CREATE PROCEDURE `IGC_PentagramInfoUpdate`(
	p_UserGuid	INT,
	p_AccountID	nvarchar(10),
	p_Name		nvarchar(10),
	p_JewelPos	TINYINT UNSIGNED,
	p_JewelIndex	SMALLINT,
	p_ItemType	SMALLINT,
	p_ItemIndex	INT,
	p_MainAttribute	SMALLINT,
	p_JewelLevel	SMALLINT,
	p_Rank1		TINYINT UNSIGNED,
	p_Rank1Level	TINYINT UNSIGNED,
	p_Rank2		TINYINT UNSIGNED,
	p_Rank2Level	TINYINT UNSIGNED,
	p_Rank3		TINYINT UNSIGNED,
	p_Rank3Level	TINYINT UNSIGNED,
	p_Rank4		TINYINT UNSIGNED,
	p_Rank4Level	TINYINT UNSIGNED,
	p_Rank5		TINYINT UNSIGNED,
	p_Rank5Level	TINYINT UNSIGNED)
sp_lbl:
BEGIN  
	
		
	DECLARE  v_Result  INT;
    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        ROLLBACK;
        SET v_Result = 1;
    END;

    SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED;
	SET 	   v_Result = 0;        
	
	START TRANSACTION;
	
	IF EXISTS ( SELECT JewelIndex FROM T_PentagramInfo  
	WHERE 	AccountID = p_AccountID AND JewelPos  = p_JewelPos AND JewelIndex = p_JewelIndex LIMIT 1)
	THEN
	
		IF (p_JewelPos = 0)
		THEN
	
			IF EXISTS ( SELECT JewelIndex FROM T_PentagramInfo  
			WHERE 	AccountID = p_AccountID AND Name = p_Name 
			AND JewelPos  = p_JewelPos AND JewelIndex = p_JewelIndex LIMIT 1)
			THEN
				UPDATE	T_PentagramInfo
				SET	JewelIndex = p_JewelIndex
				,ItemType	= p_ItemType
				,ItemIndex	= p_ItemIndex
				,MainAttribute	= p_MainAttribute
				,JewelLevel = p_JewelLevel
				,Rank1 = p_Rank1
				,Rank1Level = p_Rank1Level	
				,Rank2 = p_Rank2
				,Rank2Level = p_Rank2Level
				,Rank3 = p_Rank3
				,Rank3Level = p_Rank3Level
				,Rank4 = p_Rank4
				,Rank4Level = p_Rank4Level
				,Rank5 = p_Rank5
				,Rank5Level = p_Rank5Level 
				WHERE
					AccountID = p_AccountID	
				AND	Name	   = p_Name		
				AND	JewelPos  = p_JewelPos
				AND JewelIndex = p_JewelIndex;	
	
			ELSE
				INSERT	dbo.T_PentagramInfo
						(UserGuid,AccountID,Name,JewelPos,JewelIndex,ItemType,ItemIndex,MainAttribute,JewelLevel,Rank1, Rank1Level, Rank2, Rank2Level, Rank3, Rank3Level, Rank4, Rank4Level, Rank5, Rank5Level)
				SELECT	p_UserGuid,p_AccountID,p_Name,p_JewelPos,p_JewelIndex,p_ItemType,p_ItemIndex,p_MainAttribute,p_JewelLevel,p_Rank1,p_Rank1Level,p_Rank2,p_Rank2Level,p_Rank3,p_Rank3Level,p_Rank4,p_Rank4Level,p_Rank5,p_Rank5Level;
			END IF;		
		ELSE
	
			UPDATE	T_PentagramInfo
			SET	JewelIndex = p_JewelIndex
			,ItemType	= p_ItemType
			,ItemIndex	= p_ItemIndex
			,MainAttribute	= p_MainAttribute
			,JewelLevel = p_JewelLevel
			,Rank1 = p_Rank1
			,Rank1Level = p_Rank1Level	
			,Rank2 = p_Rank2
			,Rank2Level = p_Rank2Level
			,Rank3 = p_Rank3
			,Rank3Level = p_Rank3Level
			,Rank4 = p_Rank4
			,Rank4Level = p_Rank4Level
			,Rank5 = p_Rank5
			,Rank5Level = p_Rank5Level
			WHERE	
				AccountID = p_AccountID	
			AND	JewelPos  = p_JewelPos
			AND JewelIndex = p_JewelIndex;	
	
		END IF;	
	ELSE
		INSERT	dbo.T_PentagramInfo
				(UserGuid,AccountID,Name,JewelPos,JewelIndex,ItemType,ItemIndex,MainAttribute,JewelLevel,Rank1, Rank1Level, Rank2, Rank2Level, Rank3, Rank3Level, Rank4, Rank4Level, Rank5, Rank5Level)
		SELECT	p_UserGuid,p_AccountID,p_Name,p_JewelPos,p_JewelIndex,p_ItemType,p_ItemIndex,p_MainAttribute,p_JewelLevel,p_Rank1,p_Rank1Level,p_Rank2,p_Rank2Level,p_Rank3,p_Rank3Level,p_Rank4,p_Rank4Level,p_Rank5,p_Rank5Level;	 
	END IF;
	
	COMMIT ;
END$$
DELIMITER ;
