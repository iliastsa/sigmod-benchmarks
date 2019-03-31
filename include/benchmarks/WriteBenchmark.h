#ifndef SIGMODBENCHMARKS_WRITEBENCHMARK_H
#define SIGMODBENCHMARKS_WRITEBENCHMARK_H


#include <cstdint>
#include "Benchmark.h"

class WriteBenchmark : public Benchmark {
private:
    int n_threads;

    uint64_t filesize;

    char *filename;

public:
    WriteBenchmark(char *filename, uint64_t filesize, int n_threads);

    virtual ~WriteBenchmark();

    void run() override;
};


#endif
