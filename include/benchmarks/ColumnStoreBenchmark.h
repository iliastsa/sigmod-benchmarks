
#ifndef SIGMODBENCHMARKS_COLUMNSTOREBENCHMARK_H
#define SIGMODBENCHMARKS_COLUMNSTOREBENCHMARK_H


#include <cstdint>
#include <Tuple.h>
#include "Benchmark.h"

class ColumnStoreBenchmark : public Benchmark {
private:
    int fd;

    const char *filename;
    const char *out_filename;

    uint64_t file_size;

    int n_threads;

public:
    ColumnStoreBenchmark(const char *filename, const char *out_filename, int n_threads);

    virtual ~ColumnStoreBenchmark();

    void run() override;
};


#endif
