
#ifndef SIGMODBENCHMARKS_RANDOMWRITESORTEDTASK_H
#define SIGMODBENCHMARKS_RANDOMWRITESORTEDTASK_H

#include <ThreadPool.h>
#include <Tuple.h>
#include <ReadReader.h>
#include <Constants.h>

class RandomWriteSortedTask : public ThreadPool::Task {
private:
    int out_fd;

    uint64_t *tuples_by_row;
    uint64_t from;

    ReadReader reader;

public:
    RandomWriteSortedTask(int fd, int out_fd, uint64_t from, uint64_t to, uint64_t chunk_size, uint64_t *tuples_by_row)
            : out_fd(out_fd), tuples_by_row(tuples_by_row), from(from), reader(fd, from, to, chunk_size) {}

    void run() override {
        uint64_t sz;
        unsigned char *mem;
        while ((mem = reader.next(&sz)) != nullptr) {
            for (uint64_t i = 0; i < sz; i += Constants::TUPLE_SIZE) {
                uint64_t offset = from + tuples_by_row[i / Constants::TUPLE_SIZE] * Constants::TUPLE_SIZE;
                ssize_t written_bytes = pwrite(out_fd, &mem[i], Constants::TUPLE_SIZE, offset);

                if (written_bytes < 0)
                    exit(EXIT_FAILURE);
            }
        }
    }

    ~RandomWriteSortedTask() override {
        close(out_fd);
    }
};

#endif
