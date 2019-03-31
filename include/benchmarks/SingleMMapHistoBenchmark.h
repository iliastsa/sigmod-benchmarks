#ifndef SIGMODBENCHMARKS_SINGLEMMAPHISTOBENCHMARK_H
#define SIGMODBENCHMARKS_SINGLEMMAPHISTOBENCHMARK_H


#include <cstdint>
#include "Benchmark.h"

class SingleMMapHistoBenchmark : public Benchmark {
private:
    int fd;
    int n_threads;

    uint64_t file_size;

    void *mem;
public:
    SingleMMapHistoBenchmark(char* filename, int n_threads);
    ~SingleMMapHistoBenchmark();

    void run();
};



#endif
