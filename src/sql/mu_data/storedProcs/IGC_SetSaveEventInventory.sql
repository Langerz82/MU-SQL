DELIMITER $$
CREATE PROCEDURE `IGC_SetSaveEventInventory`(
	p_AccountID	nvarchar(10)
,	p_Name		nvarchar(10)
,	p_Inventory	VARBINARY(1024)	-- 32 * 32
)
BEGIN  
	SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED;

	UPDATE T_Event_Inventory SET Inventory = p_Inventory WHERE AccountID = p_AccountID AND Name = p_Name;
	IF (FOUND_ROWS() = 0)
	THEN
		SET p_Inventory = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF;
		
		INSERT INTO T_Event_Inventory ( AccountID, Name, Inventory)
		VALUES (p_AccountID,p_Name,p_Inventory);
	END IF;
END$$
DELIMITER ;
