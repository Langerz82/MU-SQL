DELIMITER $$
CREATE DEFINER=`root`@`localhost` PROCEDURE `IGC_DeleteCharacter`(
	p_szCharName nvarchar(10))
sp_lbl:

BEGIN

	IF EXISTS (SELECT `Name` FROM `Character` WHERE `Name` = p_szCharName)
	THEN
		DELETE FROM `Character` WHERE `Name` = p_szCharName;
	ELSE
		SELECT 0 AS QueryResult;
		LEAVE sp_lbl;
	END IF;

	IF EXISTS (SELECT `Name` FROM OptionData WHERE `Name` = p_szCharName)
	THEN
		DELETE FROM `OptionData` WHERE `Name` = p_szCharName;
	END IF;

	IF EXISTS (SELECT `CHAR_NAME` FROM T_QUEST_EXP_INFO WHERE `CHAR_NAME` = p_szCharName)
	THEN
		DELETE FROM `T_QUEST_EXP_INFO` WHERE `CHAR_NAME` = p_szCharName;
	END IF;

	IF EXISTS (SELECT `Name` FROM IGC_Gens WHERE `Name` = p_szCharName)
	THEN
		DELETE FROM `IGC_Gens` WHERE `Name` = p_szCharName;
	END IF;

	IF EXISTS (SELECT `Name` FROM IGC_GensAbuse WHERE `Name` = p_szCharName)
	THEN
		DELETE FROM `IGC_GensAbuse` WHERE `Name` = p_szCharName;
	END IF;

	IF EXISTS (SELECT `CharacterName` FROM IGC_PeriodItemInfo WHERE `CharacterName` = p_szCharName)
	THEN
		DELETE FROM `IGC_PeriodItemInfo` WHERE `CharacterName` = p_szCharName;
	END IF;

	IF EXISTS (SELECT `CharName` FROM T_LUCKY_ITEM_INFO WHERE `CharName` = p_szCharName)
	THEN
		DELETE FROM `T_LUCKY_ITEM_INFO` WHERE `CharName` = p_szCharName;
	END IF;

	IF EXISTS (SELECT `Name` FROM T_PentagramInfo WHERE `Name` = p_szCharName)
	THEN
		DELETE FROM `T_PentagramInfo` WHERE `Name` = p_szCharName;
	END IF;

	IF EXISTS (SELECT `CharacterName` FROM IGC_PeriodBuffInfo WHERE `CharacterName` = p_szCharName)
	THEN
		DELETE FROM `IGC_PeriodBuffInfo` WHERE `CharacterName` = p_szCharName;
	END IF;

	IF EXISTS (SELECT `Name` FROM T_Event_Inventory WHERE `Name` = p_szCharName)
	THEN
		DELETE FROM `T_Event_Inventory` WHERE `Name` = p_szCharName;
	END IF;

	IF EXISTS (SELECT `Name` FROM PetWarehouse WHERE `Name` = p_szCharName)
	THEN
		DELETE FROM `PetWarehouse` WHERE `Name` = p_szCharName;
	END IF;

	IF EXISTS (SELECT `Name` FROM IGC_CancelItemSale WHERE `Name` = p_szCharName)
	THEN
		DELETE FROM `IGC_CancelItemSale` WHERE `Name` = p_szCharName;
	END IF;

	IF EXISTS (SELECT `Name` FROM IGC_GremoryCase WHERE `Name` = p_szCharName)
	THEN
		DELETE FROM `IGC_GremoryCase` WHERE `Name` = p_szCharName;
	END IF;

	IF EXISTS (SELECT `CharacterName` FROM T_MineSystem WHERE `CharacterName` = p_szCharName)
	THEN
		DELETE FROM `T_MineSystem` WHERE `CharacterName` = p_szCharName;
	END IF;

	IF EXISTS (SELECT `Name` FROM T_PSHOP_ITEMVALUE_INFO WHERE `Name` = p_szCharName)
	THEN
		DELETE FROM `dbo`.`T_PSHOP_ITEMVALUE_INFO` WHERE `Name` = p_szCharName;
	END IF;

	IF EXISTS (SELECT `Name` FROM T_MuRummy WHERE `Name` = p_szCharName)
	THEN
		DELETE FROM `T_MuRummy` WHERE `Name` = p_szCharName;
	END IF;

	IF EXISTS (SELECT `Name` FROM T_MuRummyInfo WHERE `Name` = p_szCharName)
	THEN
		DELETE FROM `T_MuRummyInfo` WHERE `Name` = p_szCharName;
	END IF;

	IF EXISTS (SELECT `Name` FROM T_MuRummyLog WHERE `Name` = p_szCharName)
	THEN
		DELETE FROM `T_MuRummyLog` WHERE `Name` = p_szCharName;
	END IF;

	IF EXISTS (SELECT `Name` FROM T_GMSystem WHERE `Name` = p_szCharName)
	THEN
		DELETE FROM `T_GMSystem` WHERE `Name` = p_szCharName;
	END IF;

	IF EXISTS (SELECT `name` FROM C_Monster_KillCount WHERE `name` = p_szCharName)
	THEN
		DELETE FROM `C_Monster_KillCount` WHERE `name` = p_szCharName;
	END IF;

	SELECT 1 AS QueryResult;

END$$
DELIMITER ;
