#ifndef SIGMODBENCHMARKS_HISTOGRAMTASK_H
#define SIGMODBENCHMARKS_HISTOGRAMTASK_H

#include <Partition.h>
#include "ThreadPool.h"
#include "FileReader.h"

template <class T>
class InterleavedHistogramTask : public ThreadPool::Task {
private:
    FileReader* reader;
    uint64_t *histogram;

public:
    InterleavedHistogramTask(uint64_t* histogram, int fd, uint64_t from, uint64_t to, uint64_t chunk_size) :
            histogram(histogram)
    {
        reader = new T(fd, from, to, chunk_size);
    }

    ~InterleavedHistogramTask() final {
        delete reader;
    }

    void run() final {
        Partition partition(histogram);

        uint64_t sz;
        unsigned char* mem;

        while ((mem = reader->next(&sz)) != nullptr)
            partition.update_histogram(mem, sz);

    }
};

#endif
