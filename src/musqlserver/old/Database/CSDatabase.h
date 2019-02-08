#ifndef _CSDATABASE_H
#define _CSDATABASE_H

#include "StdAfx.h"
#include "database/Database/MySQLConnection.h"

enum ConnectDatabaseStatements : uint32
{
    /*  Naming standard for defines:
        {DB}_{SEL/INS/UPD/DEL/REP}_{Summary of data changed}
        When updating more than one field, consider looking at the calling function
        name for a suiting suffix.
    */

    CONNECT_UPD_EX1,
    CONNECT_UPD_EX2,
    MAX_CONNECTDATABASE_STATEMENTS
};

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

#endif
