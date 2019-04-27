#ifndef SIGMODBENCHMARKS_REORDERTASK_H
#define SIGMODBENCHMARKS_REORDERTASK_H

#include <ThreadPool.h>
#include <cstdint>
#include <BigTuple.h>
#include <Tuple.h>

class ReorderTask : public ThreadPool::Task {
    Tuple* local_column_store;
    BigTuple* local_tuples_reordered;
    BigTuple* tuples;

    uint64_t local_num_tuples;

public:
    ReorderTask(Tuple* local_column_store, BigTuple* local_tuples_reordered, BigTuple* tuples,
                uint64_t local_num_tuples) : local_column_store(local_column_store),
                                             local_tuples_reordered(local_tuples_reordered), tuples(tuples),
                                             local_num_tuples(local_num_tuples) {}


    void run() override {
        for (uint64_t i = 0; i < local_num_tuples; i++) {
            BigTuple* next_tuple = tuples + local_column_store[i].rowID;
            memcpy(local_tuples_reordered + i, next_tuple, Constants::TUPLE_SIZE);
        }

    }

};


#endif
