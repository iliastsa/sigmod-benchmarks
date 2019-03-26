#ifndef SIGMODBENCHMARKS_TESTTASK_H
#define SIGMODBENCHMARKS_TESTTASK_H

#include <ThreadPool.h>
#include <cstdint>
#include <Partition.h>

class HistogramTask : public ThreadPool::task {
private:
    unsigned char *segment;
    uint64_t *histogram;

    uint64_t segment_size;
public:
    HistogramTask(uint64_t* histogram, unsigned char *segment, uint64_t segment_size) :
            histogram(histogram), segment(segment), segment_size(segment_size) {}


    void run() {
        Partition partition(histogram);

        partition.update_histogram(segment, segment_size);
    }
};

#endif
