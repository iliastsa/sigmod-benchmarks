
#ifndef SIGMODBENCHMARKS_READSORTEDRAMTASK_H
#define SIGMODBENCHMARKS_READSORTEDRAMTASK_H

#include <ThreadPool.h>
#include <cstdint>
#include <BufferringWriter.h>
#include <Tuple.h>

class ReadSortedRAMTask : public ThreadPool::Task {
private:
    unsigned char* buffer;
    int out_fd;

    uint64_t from;

    Tuple* tuples;
    uint64_t num_tuples;

public:
    ReadSortedRAMTask(unsigned char* buffer, int outFd, uint64_t from, Tuple *tuples, uint64_t num_tuples)
            : buffer(buffer), out_fd(outFd), from(from), tuples(tuples), num_tuples(num_tuples) {}

    void run() override {
        BufferringWriter writer(out_fd, from, Constants::WRITE_BUFFER_SIZE);

        for (uint64_t i = 0; i < num_tuples; i++) {
            unsigned char* tuple_pos = buffer + (tuples[i].rowID * Constants::TUPLE_SIZE);
            writer.write(tuple_pos, Constants::TUPLE_SIZE);
        }
    }
};

#endif
