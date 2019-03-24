#ifndef SIGMODBENCHMARKS_HISTOGRAMTASK_H
#define SIGMODBENCHMARKS_HISTOGRAMTASK_H

#include "ThreadPool.h"
#include "FileReader.h"


class HistogramTask : public ThreadPool::task {
private:
    FileReader* reader;
    uint64_t *histogram;

public:
    HistogramTask(uint64_t* histogram, int fd, uint64_t from, uint64_t to, uint64_t chunk_size);
    ~HistogramTask() final;

    void run() final;
};

#endif
