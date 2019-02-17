#include "CSDatabase.h"
#include "database/Database/PreparedStatement.h"

//DatabaseWorkerPool<ConnectDatabaseConnection> ConnectDatabase;

void ConnectDatabaseConnection::DoPrepareStatements()
{
    if (!m_reconnecting)
        m_stmts.resize(MAX_CONNECTDATABASE_STATEMENTS);

    //PrepareStatement(CONNECT_UPD_, "", CONNECTION_ASYNCH);
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
