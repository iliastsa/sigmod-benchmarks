#ifndef SIGMODBENCHMARKS_BIGTUPLEMERGEBENCHMARK_H
#define SIGMODBENCHMARKS_BIGTUPLEMERGEBENCHMARK_H


#include <Benchmark.h>
#include <cstdint>
#include <cstring>
#include <Constants.h>
#include <ThreadPool.h>
#include <Tuple.h>

class BigTupleMergeBenchmark : public Benchmark {
private:

    //TODO REVIEW
    struct ChunkInfo {
        uint32_t idx;
        unsigned char *buffer;
        uint64_t size;
    };

    struct Compare {
        bool operator()(const ChunkInfo* a, const ChunkInfo* b) const {
            return memcmp(a->buffer, b->buffer, Constants::KEY_SIZE) > 0;
        }
    };

    const char *input_file;
    const char *output_file;

    unsigned char *merge_buffer;

    uint64_t file_size;
    uint64_t n_chunks;

    int *out_fds;

public:
    BigTupleMergeBenchmark(const char *input_file, const char *output_file, uint64_t file_size, uint64_t n_chunks);

    void run() override;
};


#endif