
#ifndef SIGMODBENCHMARKS_COLUMNSTOREBENCHMARK_H
#define SIGMODBENCHMARKS_COLUMNSTOREBENCHMARK_H


#include <cstdint>
#include <Tuple.h>
#include "Benchmark.h"

class ColumnStoreBenchmark : public Benchmark {
private:
    int fd;
    int n_threads;

    const char *filename;
    uint64_t file_size;

    const char *out_filename;
    int out_fd;

public:
    ColumnStoreBenchmark(const char *filename, const char *out_filename, int n_threads);

    virtual ~ColumnStoreBenchmark();

    void run() override;
};


#endif
