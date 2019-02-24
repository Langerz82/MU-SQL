
#include "DatabaseLoader.h"
//#include "Config.h"
#include "DatabaseEnv.h"
//#include "DBUpdater.h"
#include "Logging/Log.h"

#include <mysqld_error.h>

DatabaseLoader::DatabaseLoader(std::string const& logger, uint32 const defaultUpdateMask)
    : _logger(logger), _autoSetup(false),
    _updateFlags(0)
{
}

 bool DatabaseLoader::ConnectInfo(LPSTR lpszServer, LPSTR lpszPort, LPSTR lpszUser, LPSTR lpszPassword, LPSTR lpszDatabase)
 {
	 sprintf(_connConfig, "%s;%s;%s;%s;%s", lpszServer, lpszPort,
		 lpszUser, lpszPassword, lpszDatabase);
	 return 1;
 }

template <class T>
DatabaseLoader& DatabaseLoader::AddDatabase(DatabaseWorkerPool<T>& pool, std::string const& name)
{
	bool const updatesEnabledForThis = false;

	_open.push([this, name, updatesEnabledForThis, &pool]() -> bool
	{
		pool.SetConnectionInfo(_connConfig, 1, 1);
		if (uint32 error = pool.Open())
		{
			// If the error wasn't handled quit
			if (error)
			{
				sLog->outError("\nDatabasePool %s NOT opened. There were errors opening the MySQL connections. Check your SQLDriverLogFile "
					"for specific errors.", name.c_str());

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

    return *this;
}

bool DatabaseLoader::Load()
{
    if (!OpenDatabases())
        return false;

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

template _MU_DATABASE_API_
DatabaseLoader& DatabaseLoader::AddDatabase<MUDatabase>(DatabaseWorkerPool<MUDatabase>&, std::string const&);
