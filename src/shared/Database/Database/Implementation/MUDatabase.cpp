#include "MUDatabase.h"
#include "Database/Database/DatabaseEnv.h"
#include "Database/Database/DatabaseWorkerPool.h"

void MUDatabase::DoPrepareStatements()
{
    if (!m_reconnecting)
        m_stmts.resize(MAX_CONNECTDATABASE_STATEMENTS);

    //PrepareStatement(0, "blah", CONNECTION_ASYNC);
}

MUDatabase::MUDatabase(MySQLConnectionInfo& connInfo) : MySQLConnection(connInfo)
{
}

MUDatabase::MUDatabase(ProducerConsumerQueue<SQLOperation*>* q, MySQLConnectionInfo& connInfo) : MySQLConnection(q, connInfo)
{
}

MUDatabase::~MUDatabase()
{
}
