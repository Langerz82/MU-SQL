////////////////////////////////////////////////////////////////////////////////
// BonusEvent.h
#ifndef BONUSEVENT_H
#define BONUSEVENT_H

struct BONUS_EVENT_DATA
{
	WORD wDayOfWeek;
	WORD wStartHour;
	WORD wEndHour;
	float AddExp;
	float AddMLExp;
	WORD AddDrop;
	WORD AddExcDrop;
};

class CBonusEvent
{
public:
	CBonusEvent();
	virtual ~CBonusEvent();

	void LoadFile(char *szFile);

	static void Run(void * lpParam);

	float GetAddExp();
	float GetAddMLExp();
	WORD GetAddDrop();
	WORD GetAddExcDrop();

private:
	bool m_bEventEnable;
	bool m_bEventNotice;
	std::shared_ptr<BONUS_EVENT_DATA> m_curEvent_ptr;
	std::vector<std::shared_ptr<BONUS_EVENT_DATA>> m_vtEventData;
	CRITICAL_SECTION m_criti;
	HANDLE hThread;
};

extern CBonusEvent g_BonusEvent;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

