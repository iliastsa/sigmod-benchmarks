#ifndef SIGMODBENCHMARKS_INMEMORYBENCHMARK_H
#define SIGMODBENCHMARKS_INMEMORYBENCHMARK_H


#include <Tuple.h>
#include "Benchmark.h"

class InMemoryBenchmark : public Benchmark {
private:
    int fd;

    const char *out_filename;

    uint64_t file_size;

public:
    InMemoryBenchmark(const char *filename, const char *out_filename);

    ~InMemoryBenchmark() override = default;

    void run() final;
};


#endif
