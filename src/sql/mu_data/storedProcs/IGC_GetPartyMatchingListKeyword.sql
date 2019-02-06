DELIMITER $$
CREATE PROCEDURE `IGC_GetPartyMatchingListKeyword`(
   p_page int,
   p_pageSize int,
   p_keyword nvarchar(10))
BEGIN
	DECLARE v_nTemp int; DECLARE v_nPageSize int;
       DECLARE v_lastIdentNo int;
       DECLARE ROWCOUNT int;

	SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED;

       set v_nTemp = (((p_page - 1) * p_pageSize) + 1); 
       set v_nPageSize = p_pageSize;

       set ROWCOUNT = v_nTemp;

       set p_keyword = rtrim(replace(p_keyword,' ',''));
       set p_keyword = Concat('%' , p_keyword , '%');

       select IdentNo into v_lastIdentNo from IGC_PartyMatching  where replace(Title,' ','')  like @keyword order by IdentNo desc LIMIT ROWCOUNT;

       set ROWCOUNT = v_nPageSize;
  
       select  
	   IdentNo, PartyLeaderName, Title, MinLevel, MaxLevel, HuntingGround, WantedClass, CurMemberCount, AcceptType, UsePassWord, 
	   PassWord, WantedClassDetailInfo1, WantedClassDetailInfo2, WantedClassDetailInfo3, WantedClassDetailInfo4, WantedClassDetailInfo5,
	   WantedClassDetailInfo6, WantedClassDetailInfo7, LeaderChannel, GensType, LeaderLevel, LeaderClass, RegDate
	   from IGC_PartyMatching where IdentNo <= v_lastIdentNo and replace(Title,' ','') like @keyword
       order by IdentNo desc LIMIT ROWCOUNT;  
    
 end$$
DELIMITER ;
