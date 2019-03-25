#ifndef SIGMODBENCHMARKS_PARALLELREADHISTOBENCHMARK_H
#define SIGMODBENCHMARKS_PARALLELREADHISTOBENCHMARK_H

#include "Benchmark.h"

class ParallelReadHistoBenchmark : public Benchmark {
private:
    int fd;
    int n_threads;

    const char *filename;
    uint64_t file_size;

public:
    ParallelReadHistoBenchmark(char* filename, int n_threads);
    ~ParallelReadHistoBenchmark();

    void run();
};

#endif
