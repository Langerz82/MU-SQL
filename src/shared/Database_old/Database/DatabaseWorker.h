#ifndef _WORKERTHREAD_H
#define _WORKERTHREAD_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DatabaseEnvFwd.h"
#include <atomic>
#include <thread>

template <typename T>
class ProducerConsumerQueue;

class MySQLConnection;
class SQLOperation;

class  DatabaseWorker
{
    public:
        DatabaseWorker(ProducerConsumerQueue<SQLOperation*>* newQueue, MySQLConnection* connection);
        ~DatabaseWorker();

    private:
        ProducerConsumerQueue<SQLOperation*>* _queue;
        MySQLConnection* _connection;

        void WorkerThread();
        std::thread _workerThread;

        std::atomic<bool> _cancelationToken;

        DatabaseWorker(DatabaseWorker const& right) = delete;
        DatabaseWorker& operator=(DatabaseWorker const& right) = delete;
};

#endif
