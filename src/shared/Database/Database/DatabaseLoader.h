#ifndef _MUSQL_DATABASELOADER_H_
#define _MUSQL_DATABASELOADER_H_

#include "Common/Common.h"
#include <functional>
#include <queue>
#include <stack>
#include <string>

template <class T>
class DatabaseWorkerPool;

// A helper class to initiate all database worker pools,
// handles updating, delays preparing of statements and cleans up on failure.
class DatabaseLoader
{
public:
    DatabaseLoader(std::string const& logger, uint32 const defaultUpdateMask);

	BOOL ConnectInfo(LPTSTR lpszServer, LPSTR lpszPort, LPTSTR lpszUser, LPTSTR lpszPassword, LPTSTR lpszDatabase);

    // Register a database to the loader (lazy implemented)
    template <class T>
    DatabaseLoader& AddDatabase(DatabaseWorkerPool<T>& pool, std::string const& name);

    // Load all databases
    bool Load();

    enum DatabaseTypeFlags
    {
        DATABASE_NONE       = 0,

        DATABASE_LOGIN      = 1,
        DATABASE_CHARACTER  = 2,
        DATABASE_WORLD      = 4,

        DATABASE_MASK_ALL   = DATABASE_LOGIN | DATABASE_CHARACTER | DATABASE_WORLD
    };

private:
    bool OpenDatabases();
    bool PopulateDatabases();
    bool UpdateDatabases();
    bool PrepareStatements();

    using Predicate = std::function<bool()>;
    using Closer = std::function<void()>;

    // Invokes all functions in the given queue and closes the databases on errors.
    // Returns false when there was an error.
    bool Process(std::queue<Predicate>& queue);

    std::string const _logger;
    bool const _autoSetup;
    uint32 const _updateFlags;
	LPTSTR _connConfig[256];

    std::queue<Predicate> _open, _populate, _update, _prepare;
    std::stack<Closer> _close;
};

#endif // DatabaseLoader_h__
