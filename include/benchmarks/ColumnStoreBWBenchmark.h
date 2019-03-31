
#ifndef SIGMODBENCHMARKS_COLUMNSTOREBWBENCHMARK_H
#define SIGMODBENCHMARKS_COLUMNSTOREBWBENCHMARK_H


#include "Benchmark.h"

class ColumnStoreBWBenchmark : public Benchmark {
private:
    int fd;

    const char *filename;
    const char *out_filename;

    uint64_t file_size;

    int n_threads;

    Tuple *tuples;

public:
    ColumnStoreBWBenchmark(const char *filename, const char *out_filename, int n_threads);

    virtual ~ColumnStoreBWBenchmark() override = default;

    void run() override;
};


#endif
