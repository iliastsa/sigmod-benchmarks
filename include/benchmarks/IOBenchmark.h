#ifndef SIGMODBENCHMARKS_IOBENCHMARK_H
#define SIGMODBENCHMARKS_IOBENCHMARK_H


#include <cstdint>
#include "Benchmark.h"

class IOBenchmark : public Benchmark {
private:
    int n_threads;

    uint64_t filesize;

    char *filename_in;
    char *filename_out;

public:
    IOBenchmark(char *filename_in, char *filename_out, uint64_t filesize, int n_threads);

    virtual ~IOBenchmark();

    void run() override;
};


#endif
