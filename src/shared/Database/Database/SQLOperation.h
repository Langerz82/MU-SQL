#ifndef _SQLOPERATION_H
#define _SQLOPERATION_H

#include "Common/Common.h"
#include "DatabaseEnvFwd.h"
#include "MySQLConnection.h"

//- Union that holds element data
union SQLElementUnion
{
    PreparedStatement* stmt;
    char const* query;
};

//- Type specifier of our element data
enum SQLElementDataType
{
    SQL_ELEMENT_RAW,
    SQL_ELEMENT_PREPARED
};

//- The element
struct SQLElementData
{
    SQLElementUnion element;
    SQLElementDataType type;
};

//- For ambigious resultsets
union SQLResultSetUnion
{
    PreparedResultSet* presult;
    ResultSet* qresult;
};

class MySQLConnection;

class  SQLOperation
{
    public:
        SQLOperation(): m_conn(nullptr) { }
        virtual ~SQLOperation() { }

        virtual int call()
        {
            Execute();
            return 0;
        }
        virtual bool Execute() = 0;
        virtual void SetConnection(MySQLConnection* con) { m_conn = con; }

        MySQLConnection* m_conn;

    private:
        SQLOperation(SQLOperation const& right) = delete;
        SQLOperation& operator=(SQLOperation const& right) = delete;
};

#endif
