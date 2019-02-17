#include "CSDatabase.h"
#include "Database\Database\DatabaseEnv.h"

//DatabaseWorkerPool<ConnectDatabaseConnection> ConnectDatabase;

void ConnectDatabaseConnection::DoPrepareStatements()
{
    if (!m_reconnecting)
        m_stmts.resize(MAX_CONNECTDATABASE_STATEMENTS);

    PrepareStatement(0, "blah", CONNECTION_ASYNC);
}

ConnectDatabaseConnection::ConnectDatabaseConnection(MySQLConnectionInfo& connInfo) : MySQLConnection(connInfo)
{
}

ConnectDatabaseConnection::ConnectDatabaseConnection(ProducerConsumerQueue<SQLOperation*>* q, MySQLConnectionInfo& connInfo) : MySQLConnection(q, connInfo)
{
}

ConnectDatabaseConnection::~ConnectDatabaseConnection()
{
}
