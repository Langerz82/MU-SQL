
#ifndef _MUSQL_TIMER_H
#define _MUSQL_TIMER_H

#include "Platform/Define.h"
#include <algorithm>
#include <ace/OS_NS_sys_time.h>

/**
 * @brief
 *
 */
class WorldTimer
{
    public:

        /**
         * @brief get current server time
         *
         * @return uint32
         */
        static uint32 WorldTimer::getMSTime();

        /**
         * @brief get time difference between two timestamps
         *
         * @param oldMSTime
         * @param newMSTime
         * @return uint32
         */
        static inline uint32 WorldTimer::getMSTimeDiff(const uint32& oldMSTime, const uint32& newMSTime)
        {
            if (oldMSTime > newMSTime)
            {
                const uint32 diff_1 = (uint32(0xFFFFFFFF) - oldMSTime) + newMSTime;
                const uint32 diff_2 = oldMSTime - newMSTime;

                return min(diff_1, diff_2);
            }

            return newMSTime - oldMSTime;
        }

		static inline uint32 GetMSTimeDiffToNow(uint32 oldMSTime)
		{
			return getMSTimeDiff(oldMSTime, getMSTime());
		}

        /**
         * @brief get last world tick time
         *
         * @return uint32
         */
        static  uint32 tickTime();
        /**
         * @brief get previous world tick time
         *
         * @return uint32
         */
        static  uint32 tickPrevTime();
        /**
         * @brief tick world timer
         *
         * @return uint32
         */
        static  uint32 tick();

    private:
        /**
         * @brief
         *
         */
        WorldTimer();
        /**
         * @brief
         *
         * @param
         */
        WorldTimer(const WorldTimer&);

        /**
         * @brief analogue to WorldTimer::getMSTime() but it persists m_SystemTickTime
         *
         * @param savetime
         * @return uint32
         */
        static uint32 WorldTimer::getMSTime_internal();

        static  uint32 m_iTime; /**< TODO */
        static  uint32 m_iPrevTime; /**< TODO */
};

/**
 * @brief
 *
 */
class IntervalTimer
{
    public:
        /**
         * @brief
         *
         */
        IntervalTimer() : _interval(0), _current(0) {}

        /**
         * @brief
         *
         * @param diff
         */
        void Update(time_t diff)
        {
            _current += diff;
            if (_current < 0)
                { _current = 0; }
        }
        /**
         * @brief
         *
         * @return bool
         */
        bool Passed() const { return _current >= _interval; }
        /**
         * @brief
         *
         */
        void Reset()
        {
            if (_current >= _interval)
                { _current -= _interval; }
        }

        /**
         * @brief
         *
         * @param current
         */
        void SetCurrent(time_t current) { _current = current; }
        /**
         * @brief
         *
         * @param interval
         */
        void SetInterval(time_t interval) { _interval = interval; }
        /**
         * @brief
         *
         * @return time_t
         */
        time_t GetInterval() const { return _interval; }
        /**
         * @brief
         *
         * @return time_t
         */
        time_t GetCurrent() const { return _current; }

    private:
        time_t _interval; /**< TODO */
        time_t _current; /**< TODO */
};

/**
 * @brief
 *
 */
class ShortIntervalTimer
{
    public:
        /**
         * @brief
         *
         */
        ShortIntervalTimer() : _interval(0), _current(0) {}

        /**
         * @brief
         *
         * @param diff
         */
        void Update(uint32 diff)
        {
            _current += diff;
        }

        /**
         * @brief
         *
         * @return bool
         */
        bool Passed() const { return _current >= _interval; }
        /**
         * @brief
         *
         */
        void Reset()
        {
            if (_current >= _interval)
                { _current -= _interval; }
        }

        /**
         * @brief
         *
         * @param current
         */
        void SetCurrent(uint32 current) { _current = current; }
        /**
         * @brief
         *
         * @param interval
         */
        void SetInterval(uint32 interval) { _interval = interval; }
        /**
         * @brief
         *
         * @return uint32
         */
        uint32 GetInterval() const { return _interval; }
        /**
         * @brief
         *
         * @return uint32
         */
        uint32 GetCurrent() const { return _current; }

    private:
        uint32 _interval; /**< TODO */
        uint32 _current; /**< TODO */
};

/**
 * @brief
 *
 */
struct TimeTracker
{
    public:
        /**
         * @brief
         *
         * @param expiry
         */
        TimeTracker(time_t expiry) : i_expiryTime(expiry) {}
        /**
         * @brief
         *
         * @param diff
         */
        void Update(time_t diff) { i_expiryTime -= diff; }
        /**
         * @brief
         *
         * @return bool
         */
        bool Passed() const { return (i_expiryTime <= 0); }
        /**
         * @brief
         *
         * @param interval
         */
        void Reset(time_t interval) { i_expiryTime = interval; }
        /**
         * @brief
         *
         * @return time_t
         */
        time_t GetExpiry() const { return i_expiryTime; }

    private:
        time_t i_expiryTime; /**< TODO */
};

/**
 * @brief
 *
 */
struct ShortTimeTracker
{
    public:
        /**
         * @brief
         *
         * @param expiry
         */
        ShortTimeTracker(int32 expiry = 0) : i_expiryTime(expiry) {}
        /**
         * @brief
         *
         * @param diff
         */
        void Update(int32 diff) { i_expiryTime -= diff; }
        /**
         * @brief
         *
         * @return bool
         */
        bool Passed() const { return (i_expiryTime <= 0); }
        /**
         * @brief
         *
         * @param interval
         */
        void Reset(int32 interval) { i_expiryTime = interval; }
        /**
         * @brief
         *
         * @return int32
         */
        int32 GetExpiry() const { return i_expiryTime; }

    private:
        int32 i_expiryTime; /**< TODO */
};

#endif
