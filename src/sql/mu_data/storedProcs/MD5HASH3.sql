DELIMITER $$
CREATE PROCEDURE `MD5HASH3`(
p_accountid nvarchar(10),
p_pass_salted nvarchar(50))
begin
	DECLARE v_HashThis varbinary(16);
	DECLARE v_Hash varbinary(16);
	set v_HashThis = HashBytes('MD5',Concat(p_accountid,p_pass_salted));
	set v_Hash = (select memb__pwd from MEMB_INFO where memb___id=p_accountid);
	if
		v_HashThis = v_Hash then
		SELECT 1 AS QueryResult;
	else
		SELECT 0 AS QueryResult;
	end if;
end$$
DELIMITER ;
