DELIMITER $$
CREATE PROCEDURE `IGC_QuestExpUserInfoSave`(
p_CharacterName	nvarchar(10)
,p_Episode		smallint      
,p_Quest_Switch	smallint     
,p_ProgState		smallint
,p_StartDate		bigint	-- StartDateConvert
,p_EndDate 		bigint	-- EndDateConvert  
,p_AskIndex		varbinary(5)  
,p_AskValue		varbinary(5)  
,p_AskState		varbinary(5))
BEGIN       

DECLARE	v_ErrorCode	int;      
  DECLARE EXIT HANDLER FOR SQLEXCEPTION
  BEGIN
    ROLLBACK;
    SET v_ErrorCode = 2;
    SELECT v_ErrorCode;
  END;
  
SET	v_ErrorCode = 0;      
      
START TRANSACTION;

IF NOT EXISTS ( SELECT EPISODE FROM T_QUEST_EXP_INFO WHERE EPISODE = p_Episode AND CHAR_NAME = p_CharacterName ) THEN
BEGIN        
	INSERT T_QUEST_EXP_INFO 
	(CHAR_NAME,EPISODE,QUEST_SWITCH,PROG_STATE
	,StartDateConvert,EndDateConvert 
	,ASK_INDEX,ASK_VALUE,ASK_STATE) 
	SELECT	
	p_CharacterName,p_Episode,p_Quest_Switch,p_ProgState
	,p_StartDate,p_EndDate
	,p_AskIndex,p_AskValue,p_AskState;  
END;      

ELSE      
BEGIN 
	UPDATE	T_QUEST_EXP_INFO 
	SET	QUEST_SWITCH = p_Quest_Switch,PROG_STATE = p_ProgState
	,StartDateConvert = p_StartDate
	,EndDateConvert = p_EndDate
	,ASK_INDEX = p_AskIndex,ASK_VALUE = p_AskValue,ASK_STATE = p_AskState
	WHERE	EPISODE = p_Episode AND CHAR_NAME = p_CharacterName;  
END;
END IF;

COMMIT;

SELECT v_ErrorCode;

END$$
DELIMITER ;
