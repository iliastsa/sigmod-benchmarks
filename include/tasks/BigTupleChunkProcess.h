#ifndef SIGMODBENCHMARKS_BIGTUPLECHUNKPROCESS_H
#define SIGMODBENCHMARKS_BIGTUPLECHUNKPROCESS_H

#include <ThreadPool.h>
#include <BigTuple.h>
#include <boost/sort/block_indirect_sort/block_indirect_sort.hpp>
#include "WriteTask.h"
#include "MemColumnStoreTask.h"
#include "ReorderTask.h"
#include "CopyTask.h"

class BigTupleChunkProcess : public ThreadPool::Task {
private:


    int out_fd;
    BigTuple* tuples;
    uint64_t num_tuples;

    ThreadPool* io_tpool;

public:
    static ThreadPool local_tpool;

    BigTupleChunkProcess(int out_fd, BigTuple* tuples, uint64_t num_tuples, ThreadPool* io_tpool)
            : out_fd(out_fd), tuples(tuples), num_tuples(num_tuples), io_tpool(io_tpool) {}

    void run() override {

        /* NEW CODE BELOW */
        Timer t;
        t.run();

        Tuple* column_store = static_cast<Tuple*>(malloc(num_tuples * sizeof(Tuple)));

        uint64_t local_num_tuples = num_tuples / Constants::N_THREADS;

        for (uint64_t offset = 0; offset < num_tuples; offset += local_num_tuples) {
            local_tpool.add_task(new MemColumnStoreTask(tuples + offset, local_num_tuples, column_store + offset, offset));
        }

        local_tpool.wait_all();

        t.stop();

        std::cout << "Chunk with fd : " << out_fd << " ColumnStoreMem time : "
                  << std::fixed << t.elapsedMilliseconds() << " ms" << std::endl;

        t.run();
        boost::sort::block_indirect_sort(column_store, column_store + num_tuples, Constants::N_THREADS);
        t.stop();

        std::cout << "Chunk with fd : " << out_fd << " Sort time : "
                  << std::fixed << t.elapsedMilliseconds() << " ms" << std::endl;

        /* Reordering */
        t.run();
        BigTuple* tuples_reordered = static_cast<BigTuple*>(malloc(num_tuples * sizeof(BigTuple)));

        for (uint64_t offset = 0; offset < num_tuples; offset += local_num_tuples) {
            local_tpool.add_task(new ReorderTask(column_store + offset, tuples_reordered + offset, tuples, local_num_tuples));
        }

        local_tpool.wait_all();

        /* Copy back */

        for (uint64_t offset = 0; offset < num_tuples; offset += local_num_tuples) {
            local_tpool.add_task(new CopyTask(reinterpret_cast<unsigned char*>(tuples_reordered + offset),
                                              reinterpret_cast<unsigned char*>(tuples + offset),
                                              local_num_tuples * Constants::TUPLE_SIZE));
        }

        local_tpool.wait_all();
        t.stop();

        std::cout << "Chunk with fd : " << out_fd << " Reordering time : "
                  << std::fixed << t.elapsedMilliseconds() << " ms" << std::endl;

        /* NEW CODE ABOVE */

        unsigned char* buffer_residue = ((unsigned char*) tuples) + Constants::MERGE_BUFFER_SIZE;
        uint64_t residue_size = num_tuples * Constants::TUPLE_SIZE - Constants::MERGE_BUFFER_SIZE;

        io_tpool->add_task(new WriteTask(out_fd, buffer_residue, 0, residue_size));

        free(column_store);
        free(tuples_reordered);
    }

};

ThreadPool BigTupleChunkProcess::local_tpool(40);

#endif
