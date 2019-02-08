#ifndef _MUSQL_DATABASEENVWD_H_
#define _MUSQL_DATABASEENVWD_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DatabaseEnv.h"
//#include "QueryResult.h"
//#include "PreparedStatement.h"
//#include "Transaction.h"
#include "mysql.h"

#include <future>
#include <memory>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Field;

class ResultSet;
typedef std::shared_ptr<ResultSet> QueryResult;
typedef std::future<QueryResult> QueryResultFuture;
typedef std::promise<QueryResult> QueryResultPromise;

class PreparedStatement;

class PreparedResultSet;
typedef std::shared_ptr<PreparedResultSet> PreparedQueryResult;
typedef std::future<PreparedQueryResult> PreparedQueryResultFuture;
typedef std::promise<PreparedQueryResult> PreparedQueryResultPromise;

class QueryCallback;

class Transaction;
typedef std::shared_ptr<Transaction> SQLTransaction;

class SQLQueryHolder;
typedef std::future<SQLQueryHolder*> QueryResultHolderFuture;
typedef std::promise<SQLQueryHolder*> QueryResultHolderPromise;

// mysql
typedef struct st_mysql MYSQL;
typedef struct st_mysql_res MYSQL_RES;
typedef struct st_mysql_field MYSQL_FIELD;
typedef struct st_mysql_bind MYSQL_BIND;
typedef struct st_mysql_stmt MYSQL_STMT;

#endif // DatabaseEnvFwd_h__
