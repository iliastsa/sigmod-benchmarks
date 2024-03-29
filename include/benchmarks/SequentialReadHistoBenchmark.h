#ifndef SIGMODBENCHMARKS_SEQUENTIALREADHISTOBENCHMARK_H
#define SIGMODBENCHMARKS_SEQUENTIALREADHISTOBENCHMARK_H


#include <cstdint>
#include <Utils.h>
#include "Benchmark.h"

class SequentialReadHistoBenchmark : public Benchmark{
private:
    int fd;
    uint64_t file_size;
public:
    SequentialReadHistoBenchmark(char* filename);

    void run() final;
};


#endif
