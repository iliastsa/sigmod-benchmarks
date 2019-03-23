#ifndef SIGMODBENCHMARKS_HISTOGRAMTASK_H
#define SIGMODBENCHMARKS_HISTOGRAMTASK_H

#include "thread_pool.h"
#include "FileReader.h"

class HistogramTask : public thread_pool::task {
private:
    FileReader* reader;
    uint64_t *histogram;

public:
    HistogramTask(uint64_t* histogram, int fd, uint64_t from, uint64_t to, uint64_t chunk_size);
    ~HistogramTask();

    void run();
};

#endif
