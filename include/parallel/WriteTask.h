#ifndef SIGMODBENCHMARKS_WRITETASK_H
#define SIGMODBENCHMARKS_WRITETASK_H

#include <ThreadPool.h>
#include <cstdint>
#include <unistd.h>

class WriteTask : public ThreadPool::task {
private:
    int fd;

    uint64_t offset;
    uint64_t size;

    unsigned char *buffer;
public:
    WriteTask(int fd, unsigned char *buffer, uint64_t offset, uint64_t size) :
        fd(fd), buffer(buffer), offset(offset), size(size) {}

    void run() override {
        uint64_t bytes_written = 0;

        while (bytes_written < size) {
            ssize_t bytes = pwrite(fd, buffer, size - bytes_written, offset + bytes_written);

            bytes_written += bytes;
        }
    }
};

#endif
