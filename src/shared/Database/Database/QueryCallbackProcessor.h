#ifndef QueryCallbackProcessor_h__
#define QueryCallbackProcessor_h__

//#include "Define.h"
#include <vector>

class QueryCallback;

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
