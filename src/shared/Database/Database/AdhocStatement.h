#ifndef _ADHOCSTATEMENT_H
#define _ADHOCSTATEMENT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DatabaseEnvFwd.h"
#include "SQLOperation.h"

/*! Raw, ad-hoc query. */
class  BasicStatementTask : public SQLOperation
{
    public:
        BasicStatementTask(char const* sql, bool async = false);
        ~BasicStatementTask();

        bool Execute() override;
        QueryResultFuture GetFuture() const { return m_result->get_future(); }

    private:
        char const* m_sql;      //- Raw query to be executed
        bool m_has_result;
        QueryResultPromise* m_result;
};

#endif
