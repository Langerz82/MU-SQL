#ifndef _CSDATABASE_H
#define _CSDATABASE_H

#include "StdAfx.h"
#include "Database/Database/MySQLConnection.h"
#include "Database/Database/DatabaseLoader.h"
#include "database/Database/MySQLConnection.h"

class MySQLConnection;
class ConnectDatabaseConnection;

enum ConnectDatabaseStatements : uint32
{
    /*  Naming standard for defines:
        {DB}_{SEL/INS/UPD/DEL/REP}_{Summary of data changed}
        When updating more than one field, consider looking at the calling function
        name for a suiting suffix.
    */

    MAX_CONNECTDATABASE_STATEMENTS
};

DatabaseWorkerPool<class ConnectDatabaseConnection> ConnectDatabase;

class ConnectDatabaseConnection : public MySQLConnection
{
public:
    typedef ConnectDatabaseStatements Statements;

    //- Constructors for sync and async connections
    ConnectDatabaseConnection(MySQLConnectionInfo& connInfo);
    ConnectDatabaseConnection(ProducerConsumerQueue<SQLOperation*>* q, MySQLConnectionInfo& connInfo);
    ~ConnectDatabaseConnection();

    //- Loads database type specific prepared statements
    void DoPrepareStatements() override;
};

/// Accessor to the connection database
extern DatabaseWorkerPool<ConnectDatabaseConnection> ConnectDatabase;

template DatabaseLoader& DatabaseLoader::AddDatabase<ConnectDatabaseConnection>(DatabaseWorkerPool<ConnectDatabaseConnection>&, std::string const&);
#endif
