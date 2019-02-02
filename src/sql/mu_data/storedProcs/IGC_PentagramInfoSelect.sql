DELIMITER $$
CREATE PROCEDURE `IGC_PentagramInfoSelect`(
	p_UserGuid	INT,
	p_AccountID	nvarchar(10),
	p_Name		nvarchar(10),
	p_JewelPos	TINYINT UNSIGNED)
BEGIN  
	SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED;
	
	IF (p_JewelPos = 0)
	THEN
	
		SELECT	JewelIndex,ItemType,ItemIndex,MainAttribute,JewelLevel,Rank1, Rank1Level, Rank2, Rank2Level, Rank3, Rank3Level, Rank4, Rank4Level, Rank5, Rank5Level
		FROM	T_PentagramInfo
		WHERE	AccountID	= p_AccountID	
		AND	Name		= p_Name		
		AND	JewelPos	= p_JewelPos;	
	
	ELSE
	
		SELECT	JewelIndex,ItemType,ItemIndex,MainAttribute,JewelLevel,Rank1, Rank1Level, Rank2, Rank2Level, Rank3, Rank3Level, Rank4, Rank4Level, Rank5, Rank5Level
		FROM	T_PentagramInfo
		WHERE	AccountID	= p_AccountID	
		AND	JewelPos	= p_JewelPos;	
	
	END IF;
	
	
END$$
DELIMITER ;
