#include "ScheduleManager.h"

// Hopefully this works ok since allot of code was deleted!!

CScheduleManager::CScheduleManager() // OK
{
	this->m_StartTime = boost::posix_time::second_clock::local_time();

	this->m_ScheduleTimeInfo.clear();
}

CScheduleManager::~CScheduleManager() // OK
{

}


bool CScheduleManager::GetSchedule(pt::ptime* lpScheduleTime)
{
	int count = 0;

	for(std::vector<pt::ptime>::iterator it=this->m_ScheduleTimeInfo.begin();it != this->m_ScheduleTimeInfo.end();it++)
	{
		if(count == 0 || (*it) < (*lpScheduleTime))
		{
			(*lpScheduleTime) = (*it);
			count++;
		}
	}

	return ((count==0)?0:1);
}

bool CScheduleManager::AddSchedule(int Day, int Hour, int Minute, int Second) // OK
{	
	pt::ptime tSchedule = this->m_StartTime;
	tSchedule += pt::hours(Day*24) + pt::hours(Hour) + pt::minutes(Minute) + pt::seconds(Second);

	while(this->m_StartTime > tSchedule)
	{
		if (this->AdjustSchedule(&tSchedule)) { return 0; }
	}

	this->m_ScheduleTimeInfo.push_back(tSchedule);

	return 1;
}


bool CScheduleManager::AdjustSchedule(pt::ptime* lpScheduleTime) // OK
{
	pt::ptime* tNow = &this->m_StartTime;
	pt::ptime tElapsed = *tNow;
	tElapsed += pt::hours(lpScheduleTime->date().day_of_year() * 24) + pt::hours(lpScheduleTime->time_of_day().hours()) + pt::minutes(lpScheduleTime->time_of_day().minutes()) + pt::seconds(lpScheduleTime->time_of_day().seconds());

	(*lpScheduleTime) = tElapsed;

	return 1;
}

