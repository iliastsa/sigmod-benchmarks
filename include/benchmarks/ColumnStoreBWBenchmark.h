
#ifndef SIGMODBENCHMARKS_COLUMNSTOREBWBENCHMARK_H
#define SIGMODBENCHMARKS_COLUMNSTOREBWBENCHMARK_H


#include "Benchmark.h"

class ColumnStoreBWBenchmark : public Benchmark {
private:
    int fd;
    int n_threads;

    const char *filename;
    uint64_t file_size;

    const char *out_filename;
    int out_fd;

    Tuple *tuples;

public:
    ColumnStoreBWBenchmark(const char *filename, const char *out_filename, int n_threads);

    virtual ~ColumnStoreBWBenchmark() = default;

    void run() override;
};


#endif
