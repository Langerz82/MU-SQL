#ifndef APPENDERDB_H
#define APPENDERDB_H

#include "Logging/Appender.h"

class  AppenderDB: public Appender
{
    public:
        typedef std::integral_constant<AppenderType, APPENDER_DB>::type TypeIndex;

        AppenderDB(uint8 id, std::string const& name, LogLevel level, AppenderFlags flags, std::vector<char const*> extraArgs);
        ~AppenderDB();

        AppenderType getType() const override { return TypeIndex::value; }

    private:
        bool enabled;
        void _write(LogMessage const* message) override;
};

#endif
