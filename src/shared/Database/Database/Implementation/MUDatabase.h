#ifndef _MU_MUDATABASE_H
#define _MU_MUDATABASE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "Common/Common.h"
#include "Platform/Define.h"

#include "../DatabaseWorkerPool.h"
#include "../MySQLConnection.h"

#include "../Field.h"
#include "../PreparedStatement.h"
#include "../QueryCallback.h"
#include "../QueryResult.h"
#include "../Transaction.h"


//#include "Database/Database/DatabaseLoader.h"


enum MuDatabaseStatements : uint32
{
	/*  Naming standard for defines:
	{DB}_{SEL/INS/UPD/DEL/REP}_{Summary of data changed}
	When updating more than one field, consider looking at the calling function
	name for a suiting suffix.
	*/
	MAX_CONNECTDATABASE_STATEMENTS
};

class _MU_DATABASE_API_ MUDatabase : public MySQLConnection
{
public:
	typedef MuDatabaseStatements Statements;

    //- Constructors for sync and async connections
	//ConnectDatabaseConnection();
	MUDatabase(MySQLConnectionInfo& connInfo);
	MUDatabase(ProducerConsumerQueue<SQLOperation*>* q, MySQLConnectionInfo& connInfo);
    ~MUDatabase();

    //- Loads database type specific prepared statements
    void DoPrepareStatements() override;
};

#endif
