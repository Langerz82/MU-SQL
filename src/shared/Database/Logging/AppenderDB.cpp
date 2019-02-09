
#include "AppenderDB.h"
#include "Database/Database/DatabaseEnv.h"
#include "Logging/LogMessage.h"
#include "Database/Database/PreparedStatement.h"

AppenderDB::AppenderDB(uint8 id, std::string const& name, LogLevel level, AppenderFlags /*flags*/, std::vector<char const*> /*extraArgs*/)
    : Appender(id, name, level), enabled(false) { }

AppenderDB::~AppenderDB() { }

void AppenderDB::_write(LogMessage const* message)
{
    // Avoid infinite loop, PExecute triggers Logging with "sql.sql" type
    if (!enabled || (message->type.find("sql") != std::string::npos))
        return;

    PreparedStatement* stmt = ConnectDatabase.GetPreparedStatement(LOG_INS_LOG);
    stmt->setUInt64(0, message->mtime);
    stmt->setString(1, message->type);
    stmt->setUInt8(2, uint8(message->level));
    stmt->setString(3, message->text);
    ConnectDatabase.Execute(stmt);
}
