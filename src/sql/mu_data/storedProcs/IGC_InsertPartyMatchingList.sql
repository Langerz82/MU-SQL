DELIMITER $$
CREATE PROCEDURE `IGC_InsertPartyMatchingList`(
   p_PartyLeaderName nvarchar(10),
   p_Title nvarchar(40),
   p_PassWord nvarchar(4),
   p_MinLevel	smallint,
   p_MaxLevel smallint,
   p_HuntingGround smallint,
   p_WantedClass tinyint unsigned,
   p_CurMemberCount tinyint unsigned,
   p_UsePassWord tinyint unsigned,
   p_AcceptType tinyint unsigned,
   p_WantedClassDetailInfo1 tinyint unsigned,
   p_WantedClassDetailInfo2 tinyint unsigned,
   p_WantedClassDetailInfo3 tinyint unsigned,
   p_WantedClassDetailInfo4 tinyint unsigned,
   p_WantedClassDetailInfo5 tinyint unsigned,
   p_WantedClassDetailInfo6 tinyint unsigned,
   p_WantedClassDetailInfo7 tinyint unsigned,
   p_LeaderChannel tinyint unsigned,
   p_GensType tinyint unsigned,
   p_LeaderLevel smallint,
   p_LeaderClass tinyint unsigned)
sp_lbl:
 
BEGIN
	SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED;

	REPLACE INTO IGC_PartyMatching(PartyLeaderName,Title,MinLevel,MaxLevel,HuntingGround,WantedClass,CurMemberCount,PassWord,UsePassWord,AcceptType,WantedClassDetailInfo1,
	WantedClassDetailInfo2,WantedClassDetailInfo3,WantedClassDetailInfo4,WantedClassDetailInfo5,WantedClassDetailInfo6,WantedClassDetailInfo7
	,LeaderChannel,GensType,  LeaderLevel ,LeaderClass,RegDate)
	values(p_PartyLeaderName,p_Title,p_MinLevel,p_MaxLevel,p_HuntingGround,p_WantedClass,p_CurMemberCount,p_PassWord,p_UsePassWord,p_AcceptType,p_WantedClassDetailInfo1
	,p_WantedClassDetailInfo2,p_WantedClassDetailInfo3,p_WantedClassDetailInfo4,p_WantedClassDetailInfo5,p_WantedClassDetailInfo6,p_WantedClassDetailInfo7
		,p_LeaderChannel,p_GensType, p_LeaderLevel ,   p_LeaderClass,NOW());

	SELECT ROW_COUNT() AS result;       
   
END$$
DELIMITER ;
