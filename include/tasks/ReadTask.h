#ifndef SIGMODBENCHMARKS_READTASK_H
#define SIGMODBENCHMARKS_READTASK_H

#include <ThreadPool.h>
#include <cstdint>
#include <BufferringWriter.h>
#include <Tuple.h>
#include <sys/mman.h>
#include <ReadReader.h>

class ReadTask : public ThreadPool::Task {
private:
    ReadReader reader;

public:
    ReadTask(int fd, uint64_t from, uint64_t to, uint64_t chunk_size, unsigned char* buffer)
        : reader(fd, from, to, chunk_size, buffer) {}

    void run() final {
        uint64_t sz;

        reader.next(&sz);
    }

    ~ReadTask() final = default;
};

#endif
