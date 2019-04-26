#ifndef SIGMODBENCHMARKS_BIGTUPLECHUNKPROCESS_H
#define SIGMODBENCHMARKS_BIGTUPLECHUNKPROCESS_H

#include <ThreadPool.h>
#include <BigTuple.h>
#include <boost/sort/block_indirect_sort/block_indirect_sort.hpp>
#include "WriteTask.h"

class BigTupleChunkProcess : public ThreadPool::Task {
private:
    int out_fd;
    BigTuple* tuples;
    uint64_t num_tuples;

    ThreadPool* io_tpool;

public:

    BigTupleChunkProcess(int out_fd, BigTuple* tuples, uint64_t num_tuples, ThreadPool* io_tpool)
            : out_fd(out_fd), tuples(tuples), num_tuples(num_tuples), io_tpool(io_tpool) {}

    void run() override {

        boost::sort::block_indirect_sort(tuples, tuples + num_tuples, Constants::N_THREADS);


        unsigned char* buffer_residue = ((unsigned char*) tuples) + Constants::MERGE_BUFFER_SIZE;
        uint64_t residue_size = num_tuples * Constants::TUPLE_SIZE - Constants::MERGE_BUFFER_SIZE;

        io_tpool->add_task(new WriteTask(out_fd, buffer_residue, 0, residue_size));
    }

};


#endif
