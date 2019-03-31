
#ifndef SIGMODBENCHMARKS_RANDOMREADSORTEDTASK_H
#define SIGMODBENCHMARKS_RANDOMREADSORTEDTASK_H

#include <ThreadPool.h>
#include <ReadReader.h>
#include <Tuple.h>
#include <unistd.h>
#include <Constants.h>
#include <BufferringWriter.h>

class RandomReadSortedTask : public ThreadPool::Task {
private:
    int fd;
    int out_fd;

    uint64_t from;

    Tuple* tuples;
    uint64_t num_tuples;

public:
    RandomReadSortedTask(int fd, int outFd, uint64_t from, Tuple *tuples, uint64_t num_tuples)
        : fd(fd), out_fd(outFd), from(from), tuples(tuples), num_tuples(num_tuples) {}

    void run() override {
        BufferringWriter writer(out_fd, from, 4096);

        unsigned char buf[Constants::TUPLE_SIZE];

        for (uint64_t i = 0; i < num_tuples; i++) {
            uint64_t offset = static_cast<uint64_t>(tuples[i].rowID) * Constants::TUPLE_SIZE;
            uint64_t bytes_read = 0;

            while (bytes_read < Constants::TUPLE_SIZE) {
               bytes_read += pread(fd, buf + bytes_read, Constants::TUPLE_SIZE - bytes_read, offset + bytes_read);
            }

            writer.write(buf, Constants::TUPLE_SIZE);
        }
    }
};

#endif
