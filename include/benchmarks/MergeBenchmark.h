#ifndef SIGMODBENCHMARKS_MERGEBENCHMARK_H
#define SIGMODBENCHMARKS_MERGEBENCHMARK_H


#include <Benchmark.h>
#include <cstdint>
#include <ThreadPool.h>

class MergeBenchmark : public Benchmark {
private:
    struct ChunkInfo {
        unsigned char *buffer;
        uint64_t size;
    };

    const char *input_file;
    const char *output_file;

    unsigned char *merge_buffer;

    uint64_t file_size;
    uint64_t n_chunks;

    int n_threads;
    ThreadPool thread_pool;

    int *out_fds;

    void print_key(ChunkInfo *chunk_info);
    bool check_flushable(ChunkInfo *chunk_info, int i);
    int find_flushable(ChunkInfo *chunk_info);

    int single_run(uint16_t run, const char *output_file, uint64_t start,  Tuple *tuples, uint64_t chunk_size, unsigned char *buffer);

public:
    MergeBenchmark(const char *input_file, const char *output_file, uint64_t file_size, uint64_t n_chunks, int n_threads);

    void run() override;
};


#endif
