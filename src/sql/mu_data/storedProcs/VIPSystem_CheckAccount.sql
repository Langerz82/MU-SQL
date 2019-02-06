DELIMITER $$
CREATE PROCEDURE `VIPSystem_CheckAccount`(
 p_Account nvarchar(10))
BEGIN
 DECLARE v_DateInfo datetime;
 DECLARE v_VipType tinyint unsigned;
 
 
 SELECT Date, Type INTO v_DateInfo, v_VipType FROM T_VIPList WHERE AccountID = p_Account;
 
 IF (TIMESTAMPDIFF(MINUTE, NOW(), v_DateInfo) > 0)
 THEN
  SELECT 1 AS Result, v_DateInfo AS DateTime, v_VipType AS Type;
 ELSE
  SELECT 0 AS Result;
 END IF;
END$$
DELIMITER ;
