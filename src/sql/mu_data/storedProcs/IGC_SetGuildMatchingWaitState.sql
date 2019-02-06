DELIMITER $$
CREATE PROCEDURE `IGC_SetGuildMatchingWaitState`(
   p_Name nvarchar(10),
   p_State	  tinyint unsigned)
BEGIN

	SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED;

    IF EXISTS ( select `Name` from `Character` where `Name` = p_Name)
     THEN
		update  IGC_WaitGuildMatching set State = p_State where ApplicantName = p_Name; 
		select  0 as result;
     ELSE
		delete from IGC_WaitGuildMatching where ApplicantName = p_Name ;
		select -1 as result;
     END IF;
END$$
DELIMITER ;
