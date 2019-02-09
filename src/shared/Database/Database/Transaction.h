
#ifndef _SQL_TRANSACTION_H
#define _SQL_TRANSACTION_H

#include "Common.h"
#include "DatabaseEnvFwd.h"
#include "SQLOperation.h"
#include "Utilities/StringFormat.h"
#include <mutex>
#include <vector>

/*! Transactions, high level class. */
class  Transaction
{
    friend class TransactionTask;
    friend class MySQLConnection;

    template <typename T>
    friend class DatabaseWorkerPool;

    public:
        Transaction() : _cleanedUp(false) { }
        ~Transaction() { Cleanup(); }

        void Append(PreparedStatement* statement);
        void Append(char const* sql);
        template<typename Format, typename... Args>
        void PAppend(Format&& sql, Args&&... args)
        {
            Append(StringFormat(std::forward<Format>(sql), std::forward<Args>(args)...).c_str());
        }

        std::size_t GetSize() const { return m_queries.size(); }

    protected:
        void Cleanup();
        std::vector<SQLElementData> m_queries;

    private:
        bool _cleanedUp;

};

/*! Low level class*/
class  TransactionTask : public SQLOperation
{
    template <class T> friend class DatabaseWorkerPool;
    friend class DatabaseWorker;

    public:
        TransactionTask(SQLTransaction trans) : m_trans(trans) { }
        ~TransactionTask() { }

    protected:
        bool Execute() override;

        SQLTransaction m_trans;
        static std::mutex _deadlockLock;
};

#endif
