#include <iostream>

#include "thread_pool.h"
#include "Partition.h"
#include "ReadReader.h"
#include "FileReader.h"
#include "HistogramTask.h"

HistogramTask::HistogramTask(uint64_t* histogram, int fd, uint64_t from, uint64_t to, uint64_t chunk_size) :
    histogram(histogram) {
    reader = new ReadReader(fd, from, to, chunk_size);
}

void HistogramTask::run() {
    Partition partition(histogram);

    uint64_t sz;
    unsigned char* mem;

    while ((mem = reader->next(&sz)) != nullptr)
        partition.update_histogram(mem, sz);
}

HistogramTask::~HistogramTask() {
    delete reader;
}
