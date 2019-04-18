#ifndef SIGMODBENCHMARKS_PREPMERGEBUFFERTASK_H
#define SIGMODBENCHMARKS_PREPMERGEBUFFERTASK_H

#include <ThreadPool.h>
#include <Tuple.h>

class PrepMergeBufferTask : public ThreadPool::Task {
private:
    unsigned char* merge_buffer;

    unsigned char* big_tuples;

    Tuple* tuples;
    uint64_t tuple_limit;
public:
    PrepMergeBufferTask(unsigned char* merge_buffer, unsigned char* big_tuples, Tuple* tuples, uint64_t tuple_limit)
            : merge_buffer(merge_buffer), big_tuples(big_tuples), tuples(tuples), tuple_limit(tuple_limit) {}

    void run() override {
        uint64_t offset = 0;
        for (uint64_t i = 0; i < tuple_limit; i++, offset += Constants::TUPLE_SIZE) {
            unsigned char* tuple_pos = big_tuples + (tuples[i].rowID * Constants::TUPLE_SIZE);
            memcpy(merge_buffer + offset, tuple_pos, Constants::TUPLE_SIZE);
        }
    }
};

#endif
