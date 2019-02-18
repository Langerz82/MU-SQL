#ifndef MUMYSQL_LOG_H
#define MUMYSQL_LOG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Platform/Define.h"

#include "Asio/AsioHacksFwd.h"
#include "LogCommon.h"
#include "Utilities/StringFormat.h"

#include <memory>
#include <unordered_map>
#include <vector>

class Appender;
class Logger;
struct LogMessage;

    namespace Asio
    {
        class IoContext;
    }


#define LOGGER_ROOT "root"

typedef Appender*(*AppenderCreatorFn)(uint8 id, std::string const& name, LogLevel level, AppenderFlags flags, std::vector<std::string>& extraArgs);

template <class AppenderImpl>
Appender* CreateAppender(uint8 id, std::string const& name, LogLevel level, AppenderFlags flags, std::vector<std::string>& extraArgs)
{
    return new AppenderImpl(id, name, level, flags, std::forward<std::vector<std::string>>(extraArgs));
}

class  Log
{
    typedef std::unordered_map<std::string, Logger> LoggerMap;

    private:
        Log();
        ~Log();
        Log(Log const&) = delete;
        Log(Log&&) = delete;
        Log& operator=(Log const&) = delete;
        Log& operator=(Log&&) = delete;

    public:
		//static Log* Create();
		static Log* Instance();
		//static Log* sInstance;

		void Initialize(Asio::IoContext* ioContext, std::string const& logsDir, std::vector<std::string> logOptions, std::vector<std::string> appendOptions, std::vector<std::string> logNames, std::vector<std::string> appendNames);
        void SetSynchronous();  // Not threadsafe - should only be called from main() after all threads are joined
        void Close();
        bool ShouldLog(std::string const& type, LogLevel level) const;
        bool SetLogLevel(std::string const& name, char const* level, bool isLogger = true);

		template<typename Format, typename... Args>
		inline void outBasic(Format&& fmt, Args&&... args)
		{
			outMessage("output", LOG_LEVEL_INFO, StringFormat(std::forward<Format>(fmt), std::forward<Args>(args)...));
		}

		template<typename Format, typename... Args>
		inline void outError(Format&& fmt, Args&&... args)
		{
			outMessage("output", LOG_LEVEL_ERROR, StringFormat(std::forward<Format>(fmt), std::forward<Args>(args)...));
		}

        template<typename Format, typename... Args>
        inline void outMessage(std::string const& filter, LogLevel const level, Format&& fmt, Args&&... args)
        {
            outMessage(filter, level, StringFormat(std::forward<Format>(fmt), std::forward<Args>(args)...));
        }

        template<typename Format, typename... Args>
        void outCommand(uint32 account, Format&& fmt, Args&&... args)
        {
            if (!ShouldLog("commands.gm", LOG_LEVEL_INFO))
                return;

            outCommand(StringFormat(std::forward<Format>(fmt), std::forward<Args>(args)...), std::to_string(account));
        }

        void outCharDump(char const* str, uint32 account_id, uint64 guid, char const* name);

        template<class AppenderImpl>
        void RegisterAppender()
        {
            using Index = typename AppenderImpl::TypeIndex;
            RegisterAppender(Index::value, &CreateAppender<AppenderImpl>);
        }

        std::string const& GetLogsDir() const { return m_logsDir; }
        std::string const& GetLogsTimestamp() const { return m_logsTimestamp; }

    private:
        static std::string GetTimestampStr();
        void write(std::unique_ptr<LogMessage>&& msg) const;

        Logger const* GetLoggerByType(std::string const& type) const;
        Appender* GetAppenderByName(std::string const& name);
        uint8 NextAppenderId();
        void CreateAppenderFromConfig(std::string const names, std::string const& options);
        void CreateLoggerFromConfig(std::string const names, std::string const& options);
		void ReadAppendersFromConfig(std::vector<std::string> names, std::vector<std::string> keys);
		void ReadLoggersFromConfig(std::vector<std::string> names, std::vector<std::string> keys);
        void RegisterAppender(uint8 index, AppenderCreatorFn appenderCreateFn);
        void outMessage(std::string const& filter, LogLevel level, std::string&& message);
        void outCommand(std::string&& message, std::string&& param1);

        std::unordered_map<uint8, AppenderCreatorFn> appenderFactory;
        std::unordered_map<uint8, std::unique_ptr<Appender>> appenders;
        std::unordered_map<std::string, std::unique_ptr<Logger>> loggers;
        uint8 AppenderId;
        LogLevel lowestLogLevel;

        std::string m_logsDir;
        std::string m_logsTimestamp;

        Asio::IoContext* _ioContext;
        Asio::Strand* _strand;
};

#define sLog Log::Instance()

#define LOG_EXCEPTION_FREE(filterType__, level__, ...) \
    { \
        try \
        { \
            sLog->outMessage(filterType__, level__, __VA_ARGS__); \
        } \
        catch (std::exception& e) \
        { \
            sLog->outBasic("server", LOG_LEVEL_ERROR, "Wrong format occurred (%s) at %s:%u.", \
                e.what(), __FILE__, __LINE__); \
        } \
    }

#ifndef WIN32
void check_args(char const*, ...) ATTR_PRINTF(1, 2);
void check_args(std::string const&, ...);

// This will catch format errors on build time
#define MUSQL_LOG_MESSAGE_BODY(filterType__, level__, ...)                 \
        do {                                                            \
            if (sLog->ShouldLog(filterType__, level__))                 \
            {                                                           \
                if (false)                                              \
                    check_args(__VA_ARGS__);                            \
                                                                        \
                LOG_EXCEPTION_FREE(filterType__, level__, __VA_ARGS__); \
            }                                                           \
        } while (0)
#else
#define MUSQL_LOG_MESSAGE_BODY(filterType__, level__, ...)              \
		__pragma(error(push))                                           \
        __pragma(error(disable:4127))                                   \
        do {                                                            \
            if (sLog->ShouldLog(filterType__, level__)) {}              \
        } while (0)                                                     \
        __pragma(error(pop))
		
#endif

#define MUSQL_LOG_TRACE(filterType__, ...) \
    MUSQL_LOG_MESSAGE_BODY(filterType__, LOG_LEVEL_TRACE, __VA_ARGS__)

#define MUSQL_LOG_DEBUG(filterType__, ...) \
    MUSQL_LOG_MESSAGE_BODY(filterType__, LOG_LEVEL_DEBUG, __VA_ARGS__)

#define MUSQL_LOG_INFO(filterType__, ...)  \
    MUSQL_LOG_MESSAGE_BODY(filterType__, LOG_LEVEL_INFO, __VA_ARGS__)

#define MUSQL_LOG_WARN(filterType__, ...)  \
    MUSQL_LOG_MESSAGE_BODY(filterType__, LOG_LEVEL_WARN, __VA_ARGS__)

#define MUSQL_LOG_ERROR(filterType__, ...) \
    MUSQL_LOG_MESSAGE_BODY(filterType__, LOG_LEVEL_ERROR, __VA_ARGS__)

#define MUSQL_LOG_FATAL(filterType__, ...) \
    MUSQL_LOG_MESSAGE_BODY(filterType__, LOG_LEVEL_FATAL, __VA_ARGS__)

#endif
