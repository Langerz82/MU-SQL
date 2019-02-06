DELIMITER $$
CREATE PROCEDURE `IGC_LuckyItemInsert`(  
	p_UserGuid 			int
	,p_CharName			nvarchar(10)
	,p_ItemCode			int
	,p_ItemSerial		bigint
	,p_DurabilitySmall	smallint)
BEGIN
		REPLACE T_LUCKY_ITEM_INFO 
		SET
            DurabilitySmall = p_DurabilitySmall,
			CharName = p_CharName,
            ItemCode = p_ItemCode,
            ItemSerial  = p_ItemSerial,
			UserGuid = p_UserGuid;
END$$
DELIMITER ;
