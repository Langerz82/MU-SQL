#ifndef __MU_CSCHEDULEMANAGER_H__
#define __MU_CSCHEDULEMANAGER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdAfx.h"
#include <boost/date_time/local_time/local_time.hpp>
#include "boost/date_time/time_zone_base.hpp"

namespace lt = boost::local_time;
namespace pt = boost::posix_time;
namespace gregorian = boost::gregorian;

class CScheduleManager
{
public:
	CScheduleManager();
	virtual ~CScheduleManager();
	bool GetSchedule(pt::ptime* lpScheduleTime);
	bool AddSchedule(int Day, int Hour, int Minute, int Second);
	bool AdjustSchedule(pt::ptime* lpScheduleTime);

private:
	pt::ptime m_StartTime;
	std::vector<pt::ptime> m_ScheduleTimeInfo;
};

#endif
