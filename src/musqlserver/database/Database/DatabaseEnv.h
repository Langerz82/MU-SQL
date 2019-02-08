#ifndef DATABASEENV_H
#define DATABASEENV_H

#include "Define.h"
#include "DatabaseWorkerPool.h"

#include "old/Database/CSDatabase.h"
//#include "Implementation/CharacterDatabase.h"
//#include "Implementation/WorldDatabase.h"

#include "DatabaseLoader.h"
#include "PreparedStatement.h"
#include "QueryCallback.h"
#include "QueryResult.h"
#include "Transaction.h"

/// Accessor to the connection database
extern DatabaseWorkerPool<ConnectDatabaseConnection> ConnectDatabase;

#endif
