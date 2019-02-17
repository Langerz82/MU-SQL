#ifndef _CSDATABASE_H
#define _CSDATABASE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/Common.h"
#include "Database/Database/DatabaseEnv.h"
#include "Database/Database/MySQLConnection.h"
#include "Database/Database/DatabaseLoader.h"

class ConnectDatabaseConnection;


class _MU_DATABASE_API_ ConnectDatabaseConnection : public MySQLConnection
{
public:
	typedef DatabaseStatements Statements;

    //- Constructors for sync and async connections
    ConnectDatabaseConnection(MySQLConnectionInfo& connInfo);
    ConnectDatabaseConnection(ProducerConsumerQueue<SQLOperation*>* q, MySQLConnectionInfo& connInfo);
    ~ConnectDatabaseConnection();

    //- Loads database type specific prepared statements
    void DoPrepareStatements() override;
};

//DatabaseWorkerPool<class ConnectDatabaseConnection> ConnectDatabase;

/// Accessor to the connection database
extern DatabaseWorkerPool<class ConnectDatabaseConnection> ConnectDatabase;

#endif
