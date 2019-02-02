DELIMITER $$
CREATE PROCEDURE `IGC_CancelItemSale_AddItem`(
	p_szAccountID nvarchar(10),
	p_szName nvarchar(10),
	p_SellDate bigint,
	p_SellExpireDate bigint,
	p_SellPrice int,
	p_ItemCode smallint,
	p_ItemLevel tinyint unsigned,
	p_ItemOp1 tinyint unsigned,
	p_ItemOp2 tinyint unsigned,
	p_ItemOp3 tinyint unsigned,
	p_ItemDur tinyint unsigned,
	p_ItemExcOp tinyint unsigned,
	p_ItemSetOp tinyint unsigned,
	p_ItemHarmonyOp smallint,
	p_ItemOpEx smallint,
	p_ItemSocketOp1 smallint,
	p_ItemSocketOp2 smallint,
	p_ItemSocketOp3 smallint,
	p_ItemSocketOp4 smallint,
	p_ItemSocketOp5 smallint,
	p_ItemBonusSocketOp smallint,
	p_ItemSerial bigint)
BEGIN
	DECLARE v_ItemDataEntry int;
	DECLARE v_FreeItemNumber int;
	SET v_ItemDataEntry = 0;
	SET v_FreeItemNumber = 0;

	IF EXISTS (SELECT * FROM IGC_CancelItemSale WHERE AccountID = p_szAccountID AND Name = p_szName AND
		ItemCode = p_ItemCode AND ItemLevel = p_ItemLevel AND ItemOp1 = p_ItemOp1 AND ItemOp2 = p_ItemOp2 AND p_ItemOp3 = p_ItemOp3
		AND ItemDur = p_ItemDur AND ItemExcOp = p_ItemExcOp AND ItemSetOp = p_ItemSetOp AND ItemHarmonyOp = p_ItemHarmonyOp AND
		ItemOpEx = p_ItemOpEx AND ItemSocketOp1 = p_ItemSocketOp1 AND ItemSocketOp2 = p_ItemSocketOp2 AND
		ItemSocketOp3 = p_ItemSocketOp3 AND ItemSocketOp4 = p_ItemSocketOp4 AND ItemSocketOp5 = p_ItemSocketOp5 AND
		ItemBonusSocketOp = p_ItemBonusSocketOp AND ItemSerial = p_ItemSerial) THEN
		
        UPDATE IGC_CancelItemSale SET ItemCount = ItemCount + 1, SellPrice = SellPrice + p_SellPrice, SellDate = p_SellDate,
		SellExpireDate = p_SellExpireDate WHERE AccountID = p_szAccountID AND Name = p_szName AND
		ItemCode = p_ItemCode AND ItemLevel = p_ItemLevel AND ItemOp1 = p_ItemOp1 AND ItemOp2 = p_ItemOp2 AND p_ItemOp3 = p_ItemOp3
		AND ItemDur = p_ItemDur AND ItemExcOp = p_ItemExcOp AND ItemSetOp = p_ItemSetOp AND ItemHarmonyOp = p_ItemHarmonyOp AND
		ItemOpEx = p_ItemOpEx AND ItemSocketOp1 = p_ItemSocketOp1 AND ItemSocketOp2 = p_ItemSocketOp2 AND
		ItemSocketOp3 = p_ItemSocketOp3 AND ItemSocketOp4 = p_ItemSocketOp4 AND ItemSocketOp5 = p_ItemSocketOp5 AND
		ItemBonusSocketOp = p_ItemBonusSocketOp AND ItemSerial = p_ItemSerial;
	ELSE
		
		SET v_ItemDataEntry = (SELECT COUNT(*) FROM IGC_CancelItemSale WHERE AccountID = p_szAccountID AND Name = p_szName);

		IF (v_ItemDataEntry > 4) THEN
			WHILE (v_ItemDataEntry > 4)
			DO
				DELETE FROM IGC_CancelItemSale WHERE AccountID = p_szAccountID AND Name = p_szName AND SellDate = (SELECT MIN(SellDate) FROM IGC_CancelItemSale);
				SET v_ItemDataEntry = v_ItemDataEntry - 1;
			END WHILE;
		END IF;

		myLoop: BEGIN
		WHILE (v_FreeItemNumber < 5)
		DO
			IF NOT EXISTS (SELECT * FROM IGC_CancelItemSale WHERE AccountID = p_szAccountID AND Name = p_szName AND ItemNumber = v_FreeItemNumber) THEN
				LEAVE myLoop;
			ELSE
				SET v_FreeItemNumber = v_FreeItemNumber + 1;
			END IF;
		END WHILE; 
        END myLoop;
			

		INSERT INTO IGC_CancelItemSale (AccountID, Name, ItemNumber, ItemCount, SellDate, SellExpireDate, SellPrice, ItemCode, 
		ItemLevel, ItemOp1, ItemOp2, ItemOp3, ItemDur, ItemExcOp, ItemSetOp, ItemHarmonyOp, ItemOpEx, 
		ItemSocketOp1, ItemSocketOp2, ItemSocketOp3, ItemSocketOp4, ItemSocketOp5, ItemBonusSocketOp, ItemSerial) VALUES
		(p_szAccountID, p_szName, v_FreeItemNumber, 1, p_SellDate, p_SellExpireDate, p_SellPrice, p_ItemCode, p_ItemLevel, p_ItemOp1, p_ItemOp2,
		p_ItemOp3, p_ItemDur, p_ItemExcOp, p_ItemSetOp, p_ItemHarmonyOp, p_ItemOpEx,
		p_ItemSocketOp1, p_ItemSocketOp2, p_ItemSocketOp3, p_ItemSocketOp4, p_ItemSocketOp5, p_ItemBonusSocketOp, p_ItemSerial);
	END IF;
END$$
DELIMITER ;
