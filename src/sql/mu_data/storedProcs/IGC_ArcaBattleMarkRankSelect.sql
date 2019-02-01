DELIMITER $$
CREATE PROCEDURE `IGC_ArcaBattleMarkRankSelect`(
	p_G_Number		INT)
sp_lbl:
BEGIN
	
	
	DECLARE v_return int;
	DECLARE v_GuildRegRank int;

	SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED;
	SET v_return = 0;  
	SET v_GuildRegRank = 0;
	 
	IF NOT EXISTS ( SELECT G_Number FROM IGC_ARCA_BATTLE_GUILDMARK_REG WHERE G_Number =  p_G_Number)  
	THEN
		SELECT v_return, v_GuildRegRank;
		LEAVE sp_lbl;
	END IF;

	DROP TEMPORARY TABLE IF EXISTS Tbl_Rank;
	CREATE TEMPORARY TABLE Tbl_Rank 
	(
		mRank		INT AUTO_INCREMENT primary key,
		G_Number	INT,
		mMarkCnt	BIGINT
	);
	
	INSERT INTO Tbl_Rank (G_Number, mMarkCnt) 
	SELECT G_Number, MarkCnt FROM IGC_ARCA_BATTLE_GUILDMARK_REG ORDER BY MarkCnt DESC, GuildRegRank ASC;
	
	SELECT mRank, mMarkCnt  FROM Tbl_Rank WHERE G_Number = p_G_Number;
END$$
DELIMITER ;
