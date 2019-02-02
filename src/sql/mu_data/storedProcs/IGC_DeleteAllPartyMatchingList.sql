DELIMITER $$
CREATE PROCEDURE `IGC_DeleteAllPartyMatchingList`()
BEGIN
	SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED;

       delete from dbo.IGC_PartyMatching;
       delete from dbo.IGC_WaitPartyMatching;
END$$
DELIMITER ;
