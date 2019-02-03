DELIMITER $$
CREATE PROCEDURE `WZ_CONNECT_MEMB`(

	p_memb___id 	nvarchar(10),
	p_ServerName  	nvarchar(20),
	p_IP 		nvarchar(15))
BEGIN
	DECLARE v_find_id 		nvarchar(10);	
	DECLARE v_ConnectStat	 tinyint unsigned;
	SET v_ConnectStat = 1;

	REPLACE INTO MEMB_STAT (memb___id,ConnectStat,ServerName,IP,ConnectTM)
	values(p_memb___id,  v_ConnectStat, p_ServerName, p_IP, now());
END$$
DELIMITER ;
