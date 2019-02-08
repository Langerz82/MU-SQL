
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
	 wsprintf(_connConfig, "%s;%s;%s;%s;s", lpszServer, lpszPort,
		 lpszUser, lpszPassword, lpszDatabase);
	 return 1;
 }

template <class T>
DatabaseLoader& DatabaseLoader::AddDatabase(DatabaseWorkerPool<T>& pool, std::string const& name)
{
    bool const updatesEnabledForThis = DBUpdater<T>::IsEnabled(_updateFlags);

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
