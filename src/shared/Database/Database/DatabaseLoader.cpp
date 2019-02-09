
#include "DatabaseLoader.h"
//#include "Config.h"
#include "DatabaseEnv.h"
//#include "DBUpdater.h"
#include "Logging\Log.h"

#include <mysqld_error.h>

DatabaseLoader::DatabaseLoader(std::string const& logger, uint32 const defaultUpdateMask)
    : _logger(logger), _autoSetup(false),
    _updateFlags(0)
{
}

 BOOL DatabaseLoader::ConnectInfo(LPTSTR lpszServer, LPSTR lpszPort, LPTSTR lpszUser, LPTSTR lpszPassword, LPTSTR lpszDatabase)
 {
	 wsprintf((char *)_connConfig, "%s;%s;%s;%s;s", lpszServer, lpszPort,
		 lpszUser, lpszPassword, lpszDatabase);
	 return 1;
 }

template <class T>
DatabaseLoader& DatabaseLoader::AddDatabase(DatabaseWorkerPool<T>& pool, std::string const& name)
{
    bool const updatesEnabledForThis = DBUpdater<T>::IsEnabled(_updateFlags);

    _open.push([this, name, updatesEnabledForThis, &pool]() -> bool
    {
        uint8 const asyncThreads = 1;
        uint8 const synchThreads = 1);

        pool.SetConnectionInfo(_connConfig, asyncThreads, synchThreads);
        if (uint32 error = pool.Open())
        {
            // Database does not exist
            if ((error == ER_BAD_DB_ERROR) && updatesEnabledForThis && _autoSetup)
            {
                // Try to create the database and connect again if auto setup is enabled
                if (DBUpdater<T>::Create(pool) && (!pool.Open()))
                    error = 0;
            }

            // If the error wasn't handled quit
            if (error)
            {
                sLog.outError("DatabasePool %s NOT opened. There were errors opening the MySQL connections.", name.c_str());

                return false;
            }
        }
        // Add the close operation
        _close.push([&pool]
        {
            pool.Close();
        });
        return true;
    });

    // Populate and update only if updates are enabled for this pool
    /*if (updatesEnabledForThis)
    {
        _populate.push([this, name, &pool]() -> bool
        {
            if (!DBUpdater<T>::Populate(pool))
            {
                sLog->outMessage(_logger, "Could not populate the %s database, see log for details.", name.c_str());
                return false;
            }
            return true;
        });

        _update.push([this, name, &pool]() -> bool
        {
            if (!DBUpdater<T>::Update(pool))
            {
                sLog->outMessage(_logger, "Could not update the %s database, see log for details.", name.c_str());
                return false;
            }
            return true;
        });
    }*/

    _prepare.push([this, name, &pool]() -> bool
    {
        if (!pool.PrepareStatements())
        {
            sLog.outError("Could not prepare statements of the %s database, see log for details.", name.c_str());
            return false;
        }
        return true;
    });

    return *this;
}

bool DatabaseLoader::Load()
{
    //if (!_updateFlags)
    //    MUSQL_LOG_INFO("sql.updates", "Automatic database updates are disabled for all databases!");

    if (!OpenDatabases())
        return false;

    //if (!PopulateDatabases())
    //    return false;

    //if (!UpdateDatabases())
    //    return false;

    if (!PrepareStatements())
        return false;

    return true;
}

bool DatabaseLoader::OpenDatabases()
{
    return Process(_open);
}

bool DatabaseLoader::PopulateDatabases()
{
    return Process(_populate);
}

bool DatabaseLoader::UpdateDatabases()
{
    return Process(_update);
}

bool DatabaseLoader::PrepareStatements()
{
    return Process(_prepare);
}

bool DatabaseLoader::Process(std::queue<Predicate>& queue)
{
    while (!queue.empty())
    {
        if (!queue.front()())
        {
            // Close all open databases which have a registered close operation
            while (!_close.empty())
            {
                _close.top()();
                _close.pop();
            }

            return false;
        }

        queue.pop();
    }
    return true;
}


//template DatabaseLoader& DatabaseLoader::AddDatabase<CharacterDatabaseConnection>(DatabaseWorkerPool<CharacterDatabaseConnection>&, std::string const&);
//template DatabaseLoader& DatabaseLoader::AddDatabase<WorldDatabaseConnection>(DatabaseWorkerPool<WorldDatabaseConnection>&, std::string const&);
