DELIMITER $$
CREATE PROCEDURE `IGC_GremoryCase_AddItem`(
	p_szAccountID nvarchar(10),
	p_szName	nvarchar(10),
	p_StorageType tinyint unsigned,
	p_RewardSource tinyint unsigned,
	p_ItemID smallint,
	p_ItemLevel tinyint unsigned,
	p_ItemDurability smallint,
	p_ItemOp1 tinyint unsigned,
	p_ItemOp2 tinyint unsigned,
	p_ItemOp3 tinyint unsigned,
	p_ItemExcOption smallint,
	p_ItemSetOption smallint,
	p_ItemSocketCount tinyint unsigned,
	p_ItemMainAttribute tinyint unsigned,
	p_ItemMuunEvoItemType smallint,
	p_ItemMuunEvoItemIndex smallint,
	p_ReceiveDate bigint,
	p_ExpireDate bigint)
BEGIN
	DECLARE v_ItemsInStorage int;
    DECLARE v_FreeItemGUID int;
	DECLARE v_FreeAuthCode int;

	SET v_ItemsInStorage = (SELECT COUNT(*) FROM IGC_GremoryCase WHERE AccountID = p_szAccountID AND ((Name = p_szName AND StorageType = 2) OR StorageType = 1));

	IF (v_ItemsInStorage >= 50)
	THEN
		WHILE (v_ItemsInStorage >= 50)
		DO
			DELETE FROM IGC_GremoryCase WHERE AccountID = p_szAccountID AND ((Name = p_szName AND StorageType = 2) OR StorageType = 1) AND ReceiveDate = 
			(SELECT MIN(ReceiveDate) FROM IGC_GremoryCase WHERE AccountID = p_szAccountID AND ((Name = p_szName AND StorageType = 2) OR StorageType = 1));
			SET v_ItemsInStorage = v_ItemsInStorage - 1;
		END WHILE;
	END IF;

	SET v_FreeItemGUID = 0;

	WHILE (1 = 1)
	DO
		IF EXISTS (SELECT * FROM IGC_GremoryCase WHERE AccountID = p_szAccountID AND ((Name = p_szName AND StorageType = 2) OR StorageType = 1) AND ItemGUID = v_FreeItemGUID) THEN
			SET v_FreeItemGUID = v_FreeItemGUID + 1;
		END IF;
	END WHILE;

	SET v_FreeAuthCode = 0;

	myLoop: BEGIN
	WHILE (1 = 1)
	DO
		SET v_FreeAuthCode = ROUND(((2000000000 - 1 -1) * RAND() + 1), 0);
		IF NOT EXISTS (SELECT * FROM IGC_GremoryCase WHERE AuthCode = v_FreeAuthCode)
		THEN
			LEAVE myLoop;
		END IF;
	END WHILE;
	END myLoop;
    
	INSERT INTO IGC_GremoryCase (AccountID, Name, StorageType, RewardSource, ItemGUID, AuthCode, ItemID, ItemLevel, ItemDurability, ItemOp1, ItemOp2, ItemOp3, ItemExcOption, ItemSetOption, ItemSocketCount, ItemMainAttribute, ItemMuunEvoItemType, ItemMuunEvoItemIndex, ReceiveDate, ExpireDate) VALUES
	(p_szAccountID, p_szName, p_StorageType, p_RewardSource, v_FreeItemGUID, v_FreeAuthCode, p_ItemID, p_ItemLevel, p_ItemDurability, p_ItemOp1, p_ItemOp2, p_ItemOp3, p_ItemExcOption, p_ItemSetOption, p_ItemSocketCount, p_ItemMainAttribute, p_ItemMuunEvoItemType, p_ItemMuunEvoItemIndex, p_ReceiveDate, p_ExpireDate);

	SELECT v_FreeItemGUID AS ItemGUID, v_FreeAuthCode AS AuthCode;
END$$
DELIMITER ;
