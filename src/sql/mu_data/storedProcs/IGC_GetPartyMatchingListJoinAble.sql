DELIMITER $$
CREATE PROCEDURE `IGC_GetPartyMatchingListJoinAble`(
   p_UserLevel smallint,
   p_Class tinyint unsigned,
   p_GensType tinyint unsigned,
   p_page int,
   p_pageSize int)
BEGIN
       DECLARE v_nTemp int; DECLARE v_nPageSize int;
       DECLARE v_lastIdentNo int;
       DECLARE ROWCOUNT int;

       SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED;
       
       set v_nTemp = (((p_page - 1) * p_pageSize) + 1); 
       set v_nPageSize = p_pageSize;

       set ROWCOUNT = v_nTemp;

       select IdentNo into v_lastIdentNo from IGC_PartyMatching order by IdentNo desc LIMIT ROWCOUNT;

       set ROWCOUNT = v_nPageSize;
  
       select  
	   IdentNo, PartyLeaderName, Title, MinLevel, MaxLevel, HuntingGround, WantedClass, CurMemberCount, AcceptType, UsePassWord, 
	   `PassWord`, WantedClassDetailInfo1, WantedClassDetailInfo2, WantedClassDetailInfo3, WantedClassDetailInfo4, WantedClassDetailInfo5, 
	   WantedClassDetailInfo6, WantedClassDetailInfo7, LeaderChannel, GensType, LeaderLevel, LeaderClass, RegDate
	   from IGC_PartyMatching where  MinLevel <= p_UserLevel and (MaxLevel >= p_UserLevel  or MaxLevel = 0)
       and  (WantedClass = WantedClass | p_Class or WantedClass = 0 )  and CurMemberCount < 5 and IdentNo <= v_lastIdentNo 
       and UsePassWord = 0 and GensType = p_GensType
       order by IdentNo desc LIMIT ROWCOUNT;
    
 end$$
DELIMITER ;
