#ifndef SIGMODBENCHMARKS_BIGTUPLESORTTASK_H
#define SIGMODBENCHMARKS_BIGTUPLESORTTASK_H

#include <ThreadPool.h>
#include <cstdint>
#include <ReadReader.h>
#include "BigTupleChunkProcess.h"

class MergeChunkTask : public ThreadPool::Task {

private:
    int in_fd;
    int out_fd;
    unsigned char* mem_start;

    uint16_t chunk_num;
    uint64_t chunk_size;
    uint64_t file_offset;


    ThreadPool* sort_tpool;
    ThreadPool* io_tpool;
public:

    MergeChunkTask(int in_fd, int out_fd, unsigned char* free_mem, uint16_t chunk_num, uint64_t chunk_size,
                   uint64_t file_offset, ThreadPool* sort_tpool, ThreadPool* io_tpool)
            : in_fd(in_fd), out_fd(out_fd), mem_start(free_mem),
              chunk_num(chunk_num), chunk_size(chunk_size), file_offset(file_offset), sort_tpool(sort_tpool),
              io_tpool(io_tpool) {}

    void run() override {

        /* Read chunk */

        ReadReader reader(in_fd, file_offset, file_offset + chunk_size, 0, mem_start, true);
        uint64_t sz;
        reader.next(&sz);

        /* Process chunk */

        uint64_t num_tuples = chunk_size / Constants::TUPLE_SIZE;

        sort_tpool->add_task(new BigTupleChunkProcess(out_fd, (BigTuple*) mem_start, num_tuples, io_tpool));

    }
};

#endif