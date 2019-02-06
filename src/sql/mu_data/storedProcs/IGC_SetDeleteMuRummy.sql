DELIMITER $$
CREATE PROCEDURE `IGC_SetDeleteMuRummy`(
	p_AccountID	nvarchar(10)
,	p_Name		nvarchar(10)
)
BEGIN  
	SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED;

	DELETE FROM dbo.T_MuRummyInfo WHERE AccountID = p_AccountID AND Name = p_Name;
	DELETE FROM dbo.T_MuRummy WHERE AccountID = p_AccountID AND Name = p_Name;
END$$
DELIMITER ;
