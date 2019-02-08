#ifndef QueryCallbackProcessor_h__
#define QueryCallbackProcessor_h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DatabaseEnvFwd.h"
#include <vector>

class  QueryCallbackProcessor
{
public:
    QueryCallbackProcessor();
    ~QueryCallbackProcessor();

    void AddQuery(QueryCallback&& query);
    void ProcessReadyQueries();

private:
    QueryCallbackProcessor(QueryCallbackProcessor const&) = delete;
    QueryCallbackProcessor& operator=(QueryCallbackProcessor const&) = delete;

    std::vector<QueryCallback> _callbacks;
};

#endif // QueryCallbackProcessor_h__
