DELIMITER $$
CREATE PROCEDURE `MD5CREATEACC`(
p_accountid nvarchar(10),
p_pass nvarchar(10),
p_mail_addr nvarchar(50),
p_personalid char(13))
begin
	DECLARE v_HashThis varbinary(16);
	DECLARE v_Hash varbinary(16);
	DECLARE v_salt nvarchar(32);

	SET v_salt = '1234567890'; -- your own salt, change this
	SET v_HashThis = HashBytes('MD5',Concat(p_accountid,p_pass,v_salt));
	IF EXISTS(SELECT memb___id from MEMB_INFO WHERE memb___id = p_accountid) THEN
		SELECT 0 AS Result;
	ELSE
		INSERT INTO MEMB_INFO(memb___id, memb__pwd, mail_addr, sno__numb, ctl1_code, bloc_code, memb_name) VALUES (p_accountid, v_HashThis, p_mail_addr, p_personalid, 0, 0, p_accountid);
		select 1 as Result;
	END IF;
end$$
DELIMITER ;
