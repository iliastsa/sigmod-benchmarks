

#ifndef SIGMODBENCHMARKS_FILEINMEMCOLUMNSTOREBENCHMARK_H
#define SIGMODBENCHMARKS_FILEINMEMCOLUMNSTOREBENCHMARK_H


#include <cstdint>
#include <Tuple.h>
#include "Benchmark.h"

class FileInMemColumnStoreBenchmark : public Benchmark {
private:
    int fd;

    const char *filename;
    const char *out_filename;

    uint64_t file_size;

    int n_threads;

    Tuple *tuples;

public:
    FileInMemColumnStoreBenchmark(const char *filename, const char *out_filename, int n_threads);

    ~FileInMemColumnStoreBenchmark() override = default;

    void run() override;
};

#endif 
