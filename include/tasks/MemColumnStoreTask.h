#ifndef SIGMODBENCHMARKS_MEMCOLUMNSTORETASK_H
#define SIGMODBENCHMARKS_MEMCOLUMNSTORETASK_H


#include <ThreadPool.h>
#include <ReadReader.h>
#include <Tuple.h>
#include <Constants.h>
#include <cstring>
#include <ASyncReader.h>
#include <BigTuple.h>


class MemColumnStoreTask : public ThreadPool::Task {
private:
    BigTuple* local_tuples;
    uint64_t local_num_tuples;
    Tuple* local_column_store;

    uint64_t offset;

public:


    MemColumnStoreTask(BigTuple* local_tuples, uint64_t local_num_tuples, Tuple* local_column_store, uint64_t offset)
            : local_tuples(local_tuples), local_num_tuples(local_num_tuples), local_column_store(local_column_store),
              offset(offset) {}


    void run() final {
        for (uint64_t i = 0; i < local_num_tuples; i++) {
            memcpy(local_column_store[i].key, local_tuples[i].key, Constants::KEY_SIZE);

            local_column_store[i].rowID = static_cast<uint32_t>(offset);
            offset++;
        }
    }

    ~MemColumnStoreTask() = default;

};



#endif


