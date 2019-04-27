#ifndef SIGMODBENCHMARKS_WRITETASK_H
#define SIGMODBENCHMARKS_WRITETASK_H

#include <ThreadPool.h>
#include <cstdint>
#include <unistd.h>

class WriteTask : public ThreadPool::Task {
private:
    int fd;

    unsigned char *buffer;

    uint64_t offset;
    uint64_t size;

public:
    WriteTask(int fd, unsigned char *buffer, uint64_t offset, uint64_t size) :
        fd(fd), buffer(buffer), offset(offset), size(size) {}

    void run() override {
        uint64_t bytes_written = 0;

        Timer t;
        t.run();

        while (bytes_written < size) {
            ssize_t bytes = pwrite(fd, buffer, size - bytes_written, offset + bytes_written);

            bytes_written += bytes;
        }

        t.stop();

        std::cout << "Chunk with fd : " << fd  << " Write time : "
                  << std::fixed << t.elapsedMilliseconds() << " ms" << std::endl;
    }
};

#endif
